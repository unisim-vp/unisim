/*
 *  Copyright (c) 2007,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 *          Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifndef __UNISIM_SERVICE_DEBUG_GDB_SERVER_GDB_SERVER_TCC__
#define __UNISIM_SERVICE_DEBUG_GDB_SERVER_GDB_SERVER_TCC__

#include <unisim/util/xml/xml.hh>

#include <iostream>
#include <sstream>
#include <list>

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>

#if defined(WIN32) || defined(WIN64)

#include <winsock2.h>

#else

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <sys/times.h>
#include <fcntl.h>

#endif

namespace unisim {
namespace service {
namespace debug {
namespace gdb_server {

using std::cerr;
using std::endl;
using std::hex;
using std::dec;
using std::list;
//using unisim::service::interfaces::operator<<;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class ADDRESS>
GDBServer<ADDRESS>::GDBServer(const char *_name, Object *_parent)
	: Object(_name, _parent,
		"this service implements the GDB server remote serial protocol over TCP/IP. "
		"Standards GDB clients (e.g. gdb, eclipse, ddd) can connect to the simulator to debug the target application "
		"that runs within the simulator.")
	, Service<DebugControl<ADDRESS> >(_name, _parent)
	, Service<DebugEventListener<ADDRESS> >(_name, _parent)
	, Service<TrapReporting>(_name, _parent)
	, Client<DebugEventTrigger<ADDRESS> >(_name, _parent)
	, Client<Memory<ADDRESS> >(_name, _parent)
	, Client<Disassembly<ADDRESS> >(_name, _parent)
	, Client<SymbolTableLookup<ADDRESS> >(_name, _parent)
	, Client<Registers>(_name, _parent)
	, debug_control_export("debug-control-export", this)
	, debug_event_listener_export("debug-event-listener-export", this)
	, trap_reporting_export("trap-reporting-export", this)
	, debug_event_trigger_import("debug-event-trigger-import", this)
	, memory_import("memory-import", this)
	, registers_import("cpu-registers-import", this)
	, disasm_import("disasm-import", this)
	, symbol_table_lookup_import("symbol-table-lookup-import", this)
	, logger(*this)
	, tcp_port(12345)
	, architecture_description_filename()
	, sock(-1)
	, gdb_registers()
	, gdb_pc(0)
	, killed(false)
	, trap(false)
	, synched(false)
	, running_mode(GDBSERVER_MODE_WAITING_GDB_CLIENT)
	, extended_mode(false)
	, counter(0)
	, period(50)
	, disasm_addr(0)
	, input_buffer_size(0)
	, input_buffer_index(0)
	, output_buffer_size(0)
	, memory_atom_size(1)
	, verbose(false)
	, param_memory_atom_size("memory-atom-size", this, memory_atom_size, "size of the smallest addressable element in memory")
	, param_tcp_port("tcp-port", this, tcp_port, "TCP/IP port to listen waiting for a GDB client connection")
	, param_architecture_description_filename("architecture-description-filename", this, architecture_description_filename, "filename of a XML description of the connected processor")
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
{
	param_tcp_port.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	
	counter = period;
}

template <class ADDRESS>
GDBServer<ADDRESS>::~GDBServer()
{
	if(sock >= 0)
	{
		string packet("W00");
		PutPacket(packet);
#if defined(WIN32) || defined(WIN64)
		closesocket(sock);
#else
		close(sock);
#endif
		sock = -1;
	}
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::EndSetup()
{
	if(memory_atom_size != 1 &&
	   memory_atom_size != 2 &&
	   memory_atom_size != 4 &&
	   memory_atom_size != 8 &&
	   memory_atom_size != 16)
	{
		cerr << Object::GetName() << "ERROR! memory-atom-size must be either 1, 2, 4, 8 or 16" << endl;
		return false;
	}

	if(!registers_import) return false;
	
	input_buffer_size = 0;
	input_buffer_index = 0;
	output_buffer_size = 0;

	unisim::util::xml::Parser *parser = new unisim::util::xml::Parser(logger);
	unisim::util::xml::Node *root_node = parser->Parse(Object::GetSimulator()->SearchSharedDataFile(architecture_description_filename.c_str()));

	delete parser;

	if(root_node)
	{
		if(root_node->Name() == string("architecture"))
		{
			list<unisim::util::xml::Node *>::const_iterator xml_program_counter_node;

			bool has_architecture_name = false;
			bool has_architecture_endian = false;
			GDBEndian endian = GDB_BIG_ENDIAN;

			const list<unisim::util::xml::Property *> *root_node_properties = root_node->Properties();

			list<unisim::util::xml::Property *>::const_iterator root_node_property;
			for(root_node_property = root_node_properties->begin(); root_node_property != root_node_properties->end(); root_node_property++)
			{
				if((*root_node_property)->Name() == string("name"))
				{
					/*
					const string& architecture_name = (*root_node_property)->Value();
					if(exp->GetArchitectureName() != architecture_name)
					{
						if(logger_import)
						{
							(*logger_import) << DebugError;
							(*logger_import) << "CPU architecture (\"" << exp->GetArchitectureName() << "\") is not \"" << architecture_name << "\"" << Endl;
							(*logger_import) << EndDebugError;
						}
						delete root_node;
						return false;
					}
					*/
					has_architecture_name = true;
				}
				else if((*root_node_property)->Name() == string("endian"))
				{
					const string& architecture_endian = (*root_node_property)->Value();
					if(architecture_endian == "little")
					{
						endian = GDB_LITTLE_ENDIAN;
						has_architecture_endian = true;
					}
					else if(architecture_endian == "big")
					{
						endian = GDB_BIG_ENDIAN;
						has_architecture_endian = true;
					}
				}
				else
				{
					logger << DebugWarning << (*root_node_property)->Filename() << ":" << (*root_node_property)->LineNo() << ":ignoring property '" << (*root_node_property)->Name() << "'" << EndDebugWarning;
				}
			}

			if(!has_architecture_name)
			{
				logger << DebugWarning << root_node->Filename() << ":" << root_node->LineNo() << ":architecture has no property 'name'" << EndDebugWarning;
				delete root_node;
				return false;
			}

			if(!has_architecture_endian)
			{
				logger << DebugWarning << root_node->Filename() << ":" << root_node->LineNo() << ":assuming target architecture endian is big endian" << EndDebugWarning;
			}

			const list<unisim::util::xml::Node *> *xml_nodes = root_node->Childs();
			list<unisim::util::xml::Node *>::const_iterator xml_node;
			bool has_program_counter = false;
			string program_counter_name;

			int reg_num = 0;
			for(xml_node = xml_nodes->begin(); xml_node != xml_nodes->end(); xml_node++)
			{
				if((*xml_node)->Name() == string("register"))
				{
					string reg_name;
					bool has_reg_name = false;
					int reg_bitsize = 0;
					bool has_reg_bitsize = false;

					const list<unisim::util::xml::Property *> *xml_node_properties = (*xml_node)->Properties();

					list<unisim::util::xml::Property *>::const_iterator xml_node_property;
					for(xml_node_property = xml_node_properties->begin(); xml_node_property != xml_node_properties->end(); xml_node_property++)
					{
						if((*xml_node_property)->Name() == string("name"))
						{
							reg_name = (*xml_node_property)->Value();
							has_reg_name = true;
						}
						else
						{
							if((*xml_node_property)->Name() == string("bitsize"))
							{
								reg_bitsize = atoi((*xml_node_property)->Value().c_str());
								has_reg_bitsize = true;
								if(reg_bitsize == 0)
								{
									logger << DebugWarning << (*xml_node_property)->Filename() << ":" << (*xml_node_property)->LineNo() << ": bitsize should not be 0" << EndDebugWarning;
								}
							}
							else
							{
								if((*xml_node_property)->Name() == string("regnum"))
								{
									reg_num = atoi((*xml_node_property)->Value().c_str());
								}
								else
								{
									logger << DebugWarning << (*xml_node_property)->Filename() << ":" << (*xml_node_property)->LineNo() << ":ignoring property '" << (*xml_node_property)->Name() << "'" << EndDebugWarning;
								}
							}
						}
					}

					if(has_reg_name && has_reg_bitsize)
					{
						bool cpu_has_reg = true;
						bool cpu_has_right_reg_bitsize = true;

						unisim::util::debug::Register *reg = 0;
						
						if(!reg_name.empty())
						{
							reg = registers_import->GetRegister(reg_name.c_str());

							if(!reg)
							{
								cpu_has_reg = false;
								if(verbose)
								{
									logger << DebugWarning << "CPU does not support register " << reg_name << EndDebugWarning;
								}
							}
							else
							{
								if((8 * reg->GetSize()) != reg_bitsize)
								{
									cpu_has_right_reg_bitsize = false;
									if(verbose)
									{
										logger << DebugWarning << ": register size (" << reg_bitsize << " bits) doesn't match with size (" << 8 * reg->GetSize() << " bits) reported by CPU" << EndDebugWarning;
									}
								}
							}
						}

						if(reg_num >= gdb_registers.size())
						{
							gdb_registers.resize(reg_num + 1);
						}
						
						if(cpu_has_reg && cpu_has_right_reg_bitsize)
						{
							gdb_registers[reg_num] = GDBRegister(reg, endian, reg_num);
						}
						else
						{
							gdb_registers[reg_num] = GDBRegister(reg_name, reg_bitsize, endian, reg_num);
						}
					}
					else
					{
						logger << DebugWarning << (*xml_node)->Filename() << ":" << (*xml_node)->LineNo() << ":node '" << (*xml_node)->Name() << "' has no 'name' or 'bitsize' property" << EndDebugWarning;
					}
					
					reg_num++;
				}
				else
				{
					if((*xml_node)->Name() == string("program_counter"))
					{
						xml_program_counter_node = xml_node;
						has_program_counter = true;
						bool has_program_counter_name = false;
						const list<unisim::util::xml::Property *> *xml_node_properties = (*xml_node)->Properties();

						list<unisim::util::xml::Property *>::const_iterator xml_node_property;
						for(xml_node_property = xml_node_properties->begin(); xml_node_property != xml_node_properties->end(); xml_node_property++)
						{
							if((*xml_node_property)->Name() == string("name"))
							{
								program_counter_name = (*xml_node_property)->Value();
								has_program_counter_name = true;
							}
						}

						if(!has_program_counter_name)
						{
							logger << DebugWarning << (*xml_node)->Filename() << ":" << (*xml_node)->LineNo() << ":node '" << (*xml_node)->Name() << "' has no 'name'" << EndDebugWarning;
							delete root_node;
							return false;
						}
					}
					else
					{
						logger << DebugWarning << (*xml_node)->Filename() << ":" << (*xml_node)->LineNo() << ":ignoring tag '" << (*xml_node)->Name() << "'" << EndDebugWarning;
					}
				}
			}

			if(!has_program_counter)
			{
				logger << DebugError << root_node->Filename() << ":" << root_node->LineNo() << ":architecture has no program counter" << EndDebugError;
				delete root_node;
				return false;
			}

			vector<GDBRegister>::iterator reg_iter;

			for(reg_iter = gdb_registers.begin(); reg_iter != gdb_registers.end(); reg_iter++)
			{
				if(reg_iter->GetName() == program_counter_name)
				{
					gdb_pc = &(*reg_iter);
					break;
				}
			}

			if(!gdb_pc)
			{
				logger << DebugError << (*xml_program_counter_node)->Filename() << ":" << (*xml_program_counter_node)->LineNo() << ":program counter does not belong to registers" << EndDebugError;
				delete root_node;
				return false;
			}
		}
		else
		{
			logger << DebugError << root_node->Filename() << ":" << root_node->LineNo() << ":root node is not named 'architecture'" << EndDebugError;
			delete root_node;
			return false;
		}

		delete root_node;
	}
	else
	{
		logger << DebugError << architecture_description_filename << ":no root node" << EndDebugError;
		return false;
	}

	struct sockaddr_in addr;
	int server_sock;

	server_sock = socket(AF_INET, SOCK_STREAM, 0);

	if(server_sock < 0)
	{
		logger << DebugError << "socket failed" << EndDebugError;
		return false;
	}

    /* ask for reusing TCP port */
    int opt_so_reuseaddr = 1;
    if(setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, (char *) &opt_so_reuseaddr, sizeof(opt_so_reuseaddr)) < 0)
	{
		if(verbose)
		{
			logger << DebugWarning << "setsockopt failed requesting port reuse" << EndDebugWarning;
		}
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(tcp_port);
	addr.sin_addr.s_addr = INADDR_ANY;
	if(bind(server_sock, (struct sockaddr *) &addr, sizeof(addr)) < 0)
	{
		logger << DebugError << "Bind failed. TCP Port #" << tcp_port << " may be already in use. Please specify another port in " << param_tcp_port.GetName() << EndDebugError;
#if defined(WIN32) || defined(WIN64)
		closesocket(server_sock);
#else
		close(server_sock);
#endif
		return false;
	}

	if(listen(server_sock, 1))
	{
		logger << DebugError << "Listen failed" << EndDebugError;
#if defined(WIN32) || defined(WIN64)
		closesocket(server_sock);
#else
		close(server_sock);
#endif
		return false;
	}

#if defined(WIN32) || defined(WIN64)
		int addr_len;
#else
		socklen_t addr_len;
#endif

	logger << DebugInfo << "Listening on TCP port " << tcp_port << EndDebugInfo;
	addr_len = sizeof(addr);
	sock = accept(server_sock, (struct sockaddr *) &addr, &addr_len);

	if(sock < 0)
	{
		logger << DebugError << "accept failed" << EndDebugError;
#if defined(WIN32) || defined(WIN64)
		closesocket(server_sock);
#else
		close(server_sock);
#endif
		return false;
	}

	if(verbose)
	{
		logger << DebugInfo << "Connection with GDB client established" << EndDebugInfo;
	}

    /* set short latency */
    int opt_tcp_nodelay = 1;
    if(setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *) &opt_tcp_nodelay, sizeof(opt_tcp_nodelay)) < 0)
	{
		if(verbose)
		{
			logger << DebugWarning << "setsockopt failed requesting short latency" << EndDebugWarning;
		}
	}


#if defined(WIN32) || defined(WIN64)
	u_long NonBlock = 1;
	if(ioctlsocket(sock, FIONBIO, &NonBlock) != 0)
	{
		logger << DebugError << "ioctlsocket failed" << EndDebugError;
		closesocket(server_sock);
		closesocket(sock);
		sock = -1;
		return false;
	}
#else
	int socket_flag = fcntl(sock, F_GETFL, 0);

	if(socket_flag < 0)
	{
		logger << DebugError << "fcntl failed" << EndDebugError;
		close(server_sock);
		close(sock);
		sock = -1;
		return false;
	}

	/* Ask for non-blocking reads on socket */
	if(fcntl(sock, F_SETFL, socket_flag | O_NONBLOCK) < 0)
	{
		logger << DebugError << "fcntl failed" << EndDebugError;
		close(server_sock);
		close(sock);
		sock = -1;
		return false;
	}
#endif

#if defined(WIN32) || defined(WIN64)
	closesocket(server_sock);
#else
	close(server_sock);
#endif

	return true;
}

template <class ADDRESS>
void GDBServer<ADDRESS>::OnDisconnect()
{
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::ParseHex(const string& s, size_t& pos, ADDRESS& value)
{
	size_t len = s.length();
	size_t n = 0;

	value = 0;
	while(pos < len && n < 2 * sizeof(ADDRESS))
	{
		uint8_t nibble;
		if(!IsHexChar(s[pos])) break;
		nibble = HexChar2Nibble(s[pos]);
		value <<= 4;
		value |= nibble;
		pos++;
		n++;
	}
	return n > 0;
}

template <class ADDRESS>
void GDBServer<ADDRESS>::OnDebugEvent(const unisim::util::debug::Event<ADDRESS>& event)
{
	switch(event.GetType())
	{
		case unisim::util::debug::Event<ADDRESS>::EV_BREAKPOINT:
			break;
		case unisim::util::debug::Event<ADDRESS>::EV_WATCHPOINT:
			break;
		default:
			// ignore event
			return;
	}
	
	trap = true;
	synched = true;
}

template <class ADDRESS>
void GDBServer<ADDRESS>::ReportTrap()
{
	trap = true;
	synched = false;
}

template <class ADDRESS>
void
GDBServer<ADDRESS>::
ReportTrap(const unisim::kernel::service::Object &obj)
{
	ReportTrap();
}

template <class ADDRESS>
void
GDBServer<ADDRESS>::
ReportTrap(const unisim::kernel::service::Object &obj,
		   const std::string &str)
{
	ReportTrap();
}

template <class ADDRESS>
void
GDBServer<ADDRESS>::
ReportTrap(const unisim::kernel::service::Object &obj,
		   const char *c_str)
{
	ReportTrap();
}

template <class ADDRESS>
typename DebugControl<ADDRESS>::DebugCommand GDBServer<ADDRESS>::FetchDebugCommand(ADDRESS cia)
{
	ADDRESS addr;
	ADDRESS size;
	ADDRESS reg_num;
	ADDRESS type;

	if(running_mode == GDBSERVER_MODE_CONTINUE && !trap)
	{
		if(--counter > 0)
		{
			return DebugControl<ADDRESS>::DBG_STEP;
		}

		counter = period;

		char c;

		if(GetChar(c, false))
		{
			if(c == 0x03)
			{
				running_mode = GDBSERVER_MODE_WAITING_GDB_CLIENT;
				ReportTracePointTrap();
			}
		}
		else
		{
			return DebugControl<ADDRESS>::DBG_STEP;
		}
	}

	if((trap || running_mode == GDBSERVER_MODE_STEP) && !synched)
	{
		synched = true;
		return DebugControl<ADDRESS>::DBG_SYNC;
	}

	if(trap || running_mode == GDBSERVER_MODE_STEP)
	{
		ReportTracePointTrap();
		running_mode = GDBSERVER_MODE_WAITING_GDB_CLIENT;
		trap = false;
	}

	string packet;

	while(!killed)
	{
		if(!GetPacket(packet, true))
		{
			Object::Stop(0);
			return DebugControl<ADDRESS>::DBG_KILL;
		}

		size_t pos = 0;
		size_t len = packet.length();

		switch(packet[pos++])
		{
			case 'g':
				if(!ReadRegisters())
				{
					Object::Stop(0);
					return DebugControl<ADDRESS>::DBG_KILL;
				}
				break;

			case 'p':
				if(!ParseHex(packet, pos, reg_num)) break;
				if(!ReadRegister(reg_num))
				{
					Object::Stop(0);
					return DebugControl<ADDRESS>::DBG_KILL;
				}
				break;

			case 'G':
				if(!WriteRegisters(packet.substr(1)))
				{
					Object::Stop(0);
					return DebugControl<ADDRESS>::DBG_KILL;
				}
				break;

			case 'P':
				if(!ParseHex(packet, pos, reg_num)) break;
				if(packet[pos++] != '=') break;
				if(!WriteRegister(reg_num, packet.substr(pos)))
				{
					Object::Stop(0);
					return DebugControl<ADDRESS>::DBG_KILL;
				}
				break;

			case 'm':
				if(!ParseHex(packet, pos, addr)) break;
				if(packet[pos++] != ',') break;
				if(!ParseHex(packet, pos, size)) break;
				if(pos != len) break;
				if(!ReadMemory(addr * memory_atom_size, size))
				{
					Object::Stop(0);
					return DebugControl<ADDRESS>::DBG_KILL;
				}
				break;

			case 'M':
				if(!ParseHex(packet, pos, addr)) break;
				if(packet[pos++] != ',') break;
				if(!ParseHex(packet, pos, size)) break;
				if(packet[pos++] != ':') break;
				if(!WriteMemory(addr * memory_atom_size, packet.substr(pos), size))
				{
					Object::Stop(0);
					return DebugControl<ADDRESS>::DBG_KILL;
				}
				break;

			case 's':
				if(!ParseHex(packet, pos, addr))
				{
					running_mode = GDBSERVER_MODE_STEP;
					synched = false;
					return DebugControl<ADDRESS>::DBG_STEP;
				}
				if(gdb_pc)
					gdb_pc->SetValue(&addr);
				else
				{
					if(verbose)
					{
						logger << DebugWarning << "CPU has no program counter" << EndDebugWarning;
					}
				}
				running_mode = GDBSERVER_MODE_STEP;
				synched = false;
				return DebugControl<ADDRESS>::DBG_STEP;
				break;

			case 'c':
				if(!ParseHex(packet, pos, addr))
				{
					running_mode = GDBSERVER_MODE_CONTINUE;
					return DebugControl<ADDRESS>::DBG_STEP;
				}
				addr *= memory_atom_size;
				if(gdb_pc)
					gdb_pc->SetValue(&addr);
				else
				{
					if(verbose)
					{
						logger << DebugWarning << "CPU has no program counter" << EndDebugWarning;
					}
				}
				running_mode = GDBSERVER_MODE_CONTINUE;
				return DebugControl<ADDRESS>::DBG_STEP;
				break;

			case 'H':
				PutPacket("E01");
				break;

			case 'k':

				if(!extended_mode)
				{
					Kill();
					killed = true;
				}
				break;

			case 'R':
				return DebugControl<ADDRESS>::DBG_RESET;
				break;

			case '?':
				ReportTracePointTrap();
				break;

			case '!':
				extended_mode = true;
				PutPacket("OK");
				break;

			case 'Z':
				if(!ParseHex(packet, pos, type)) break;
				if(packet[pos++] != ',') break;
				if(!ParseHex(packet, pos, addr)) break;
				if(packet[pos++] != ',') break;
				if(!ParseHex(packet, pos, size)) break;
				if(!SetBreakpointWatchpoint(type, addr * memory_atom_size, size))
				{
					Object::Stop(0);
					return DebugControl<ADDRESS>::DBG_KILL;
				}
				break;

			case 'z':
				if(!ParseHex(packet, pos, type)) break;
				if(packet[pos++] != ',') break;
				if(!ParseHex(packet, pos, addr)) break;
				if(packet[pos++] != ',') break;
				if(!ParseHex(packet, pos, size)) break;
				if(!RemoveBreakpointWatchpoint(type, addr * memory_atom_size, size))
				{
					Object::Stop(0);
					return DebugControl<ADDRESS>::DBG_KILL;
				}
				break;

			default:

				if(packet == "qSymbol" || packet == "qSymbol::")
				{
					PutPacket("OK");
				}
				else if(packet == "vCont?")
				{
					PutPacket("vCont;c;C;s;S");
				}
				else if(packet.substr(0, 7) == "vCont;c")
				{
					running_mode = GDBSERVER_MODE_CONTINUE;
					return DebugControl<ADDRESS>::DBG_STEP;
				}
				else if(packet.substr(0, 7) == "vCont;s")
				{
					running_mode = GDBSERVER_MODE_STEP;
					synched = false;
					return DebugControl<ADDRESS>::DBG_STEP;
				}
				else if(packet.substr(0, 6) == "qRcmd,")
				{
					HandleQRcmd(packet.substr(6));
				}
				else
				{
					if(verbose)
					{
						logger << DebugWarning << "Received an unknown GDB remote protocol packet" << EndDebugWarning;
					}
					PutPacket("");
				}
				break;
		} // end of switch
	}
	Object::Stop(0);
	return (DebugControl<ADDRESS>::DBG_KILL);
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::GetChar(char& c, bool blocking)
{
	if(input_buffer_size == 0)
	{
		do
		{
#if defined(WIN32) || defined(WIN64)
			int r = recv(sock, input_buffer, sizeof(input_buffer), 0);
			if(r == 0 || r == SOCKET_ERROR)
#else
			ssize_t r = read(sock, input_buffer, sizeof(input_buffer));
			if(r <= 0)
#endif
			{
#if defined(WIN32) || defined(WIN64)
				if(r == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK)
#else
				if(r < 0 && errno == EAGAIN)
#endif
				{
					if(blocking)
					{
#if defined(WIN32) || defined(WIN64)
						Sleep(1); // sleep for 10ms
#else
						usleep(1000); // sleep for 10ms
#endif
						continue;
					}
					else
					{
						return false;
					}
				}

				logger << DebugError << "can't read from socket" << EndDebugError;
				return false;
			}
			input_buffer_index = 0;
			input_buffer_size = r;
			break;
		} while(1);
	}

	c = input_buffer[input_buffer_index++];
	input_buffer_size--;
	return true;
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::FlushOutput()
{
	if(output_buffer_size > 0)
	{
//		cerr << "begin: output_buffer_size = " << output_buffer_size << endl;
		unsigned int index = 0;
		do
		{
#if defined(WIN32) || defined(WIN64)
			int r = send(sock, output_buffer + index, output_buffer_size, 0);
			if(r == 0 || r == SOCKET_ERROR)
#else
			ssize_t r = write(sock, output_buffer + index, output_buffer_size);
			if(r <= 0)
#endif
			{
				logger << DebugError << "can't write into socket" << EndDebugError;
				return false;
			}

			index += r;
			output_buffer_size -= r;
//			cerr << "output_buffer_size = " << output_buffer_size << endl;
		}
		while(output_buffer_size > 0);
	}
//	cerr << "end: output_buffer_size = " << output_buffer_size << endl;
	return true;
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::PutChar(char c)
{
	if(output_buffer_size >= sizeof(output_buffer))
	{
		if(!FlushOutput()) return false;
	}

	output_buffer[output_buffer_size++] = c;
	return true;
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::GetPacket(string& s, bool blocking)
{
	uint8_t checksum;
	uint8_t received_checksum;
	char c;
	char ch[2];
	ch[1] = 0;
	s.erase();

	while(1)
	{
		while(1)
		{
			if(!GetChar(c, blocking)) return false;
			if(c == '$') break;
		}

		checksum = 0;

		while(1)
		{
			if(!GetChar(c, true)) return false;
			if(c == '$')
			{
				checksum = 0;
				s.erase();
				continue;
			}

			if(c == '#') break;
			checksum = checksum + (uint8_t ) c;
			ch[0] = c;
			s += ch;
		}

		if(c == '#')
		{
			if(!GetChar(c, true)) break;
			received_checksum = HexChar2Nibble(c) << 4;
			if(!GetChar(c, true)) break;
			received_checksum += HexChar2Nibble(c);

			if(verbose)
			{
				logger << DebugInfo << "receiving $" << s << "#" << Nibble2HexChar((received_checksum >> 4) & 0xf) << Nibble2HexChar(received_checksum & 0xf) << EndDebugInfo;
			}
			if(checksum != received_checksum)
			{
				if(!PutChar('-')) return false;
				if(!FlushOutput()) return false;
			}
			else
			{
				if(!PutChar('+')) return false;
				if(!FlushOutput()) return false;

				if(s.length() >= 3 && s[2] == ':')
				{
					if(!PutChar(s[0])) return false;
					if(!PutChar(s[1])) return false;
					if(!FlushOutput()) return false;
					s.erase(0, 3);
				}
				return true;
			}
		}
	}
	return false;
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::PutPacket(const string& s)
{
	uint8_t checksum;
	unsigned int pos;
	unsigned int len;
	char c;

	do
	{
		if(!PutChar('$')) return false;
		checksum = 0;
		pos = 0;
		len = s.length();

		while(pos < len)
		{
			c = s[pos];
			if(!PutChar(c)) return false;
			checksum += (uint8_t) c;
			pos++;
		}
		if(!PutChar('#')) return false;
		if(!PutChar(Nibble2HexChar(checksum >> 4))) return false;
		if(!PutChar(Nibble2HexChar(checksum & 0xf))) return false;
		if(verbose)
		{
			logger << DebugInfo << "sending $" << s << "#" << Nibble2HexChar(checksum >> 4) << Nibble2HexChar(checksum & 0xf) << EndDebugInfo;
		}
		if(!FlushOutput()) return false;
	} while(GetChar(c, true) && c != '+');
	return true;
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::OutputText(const char *s, int count)
{
	int i;
	uint8_t packet[1 + 2 * count + 1];
	uint8_t *p = packet;

	*p = 'O';
	p++;
	for(i = 0; i < count; i++, p += 2)
	{
		p[0] = Nibble2HexChar((uint8_t) s[i] >> 4);
		p[1] = Nibble2HexChar((uint8_t) s[i] & 0xf);
	}
	*p = 0;
	return PutPacket((const char *) packet);
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::ReadRegisters()
{
	string packet;
	vector<GDBRegister>::const_iterator gdb_reg;

	for(gdb_reg = gdb_registers.begin(); gdb_reg != gdb_registers.end(); gdb_reg++)
	{
		string hex;
		gdb_reg->GetValue(hex);
		//std::cerr << gdb_reg->GetName() << "=" << hex << std::endl;
		packet += hex;
	}
	return PutPacket(packet);
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::WriteRegisters(const string& hex)
{
	bool write_error = false;
	vector<GDBRegister>::iterator gdb_reg;
	unsigned int pos = 0;

	for(gdb_reg = gdb_registers.begin(); gdb_reg != gdb_registers.end(); pos += gdb_reg->GetHexLength(), gdb_reg++)
	{
		if(!gdb_reg->SetValue(hex.substr(pos, gdb_reg->GetHexLength())))
		{
			write_error = true;
			break;
		}
	}

	return write_error ? PutPacket("E00") : PutPacket("OK");
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::ReadRegister(unsigned int regnum)
{
	if(regnum >= gdb_registers.size())
	{
		logger << DebugError << "Register #" << regnum << " can't be read because it is unknown" << EndDebugError;
		return PutPacket("E00");
	}
	const GDBRegister& gdb_reg = gdb_registers[regnum];
	string packet;
	if(gdb_reg.IsEmpty())
	{
		logger << DebugError << "Register #" << regnum << " can't be read because it is unknown" << EndDebugError;
		return PutPacket("E00");
	}
	gdb_reg.GetValue(packet);
	return PutPacket(packet);
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::WriteRegister(unsigned int regnum, const string& hex)
{
	if(regnum >= gdb_registers.size())
	{
		logger << DebugError << "Register #" << regnum << " can't be written because it is unknown" << EndDebugError;
		return PutPacket("E00");
	}
	GDBRegister& gdb_reg = gdb_registers[regnum];
	return gdb_reg.SetValue(hex) ? PutPacket("OK") : PutPacket("E00");
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::ReadMemory(ADDRESS addr, uint32_t size)
{
	bool read_error = false;
	bool overwrapping = false;
	string packet;
	char ch[2];
	ch[1] = 0;

	if(size > 0)
	{
		do
		{
			uint8_t value = 0;
			if(!overwrapping)
			{
				if(!memory_import->ReadMemory(addr, &value, 1))
				{
					read_error = true;
					value = 0;
				}
			}
			ch[0] = Nibble2HexChar(value >> 4);
			packet += ch;
			ch[0] = Nibble2HexChar(value & 0xf);
			packet += ch;
			if((addr + 1) == 0) overwrapping = true;
		} while(++addr, --size);
	}

	if(read_error)
	{
		if(verbose)
		{
			logger << DebugWarning << memory_import.GetName() << "->ReadMemory has reported an error" << EndDebugWarning;
		}
	}

	return /*read_error ? PutPacket("E00") : PutPacket(packet); */PutPacket(packet);
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::WriteMemory(ADDRESS addr, const string& hex, uint32_t size)
{
	bool write_error = false;
	bool overwrapping = false;
	unsigned int pos = 0;
	int len = hex.length();

	if(size > 0 && len > 0)
	{
		do
		{
			uint8_t value;
			value = (HexChar2Nibble(hex[pos]) << 4) | HexChar2Nibble(hex[pos + 1]);
			if(!overwrapping)
			{
				if(!memory_import->WriteMemory(addr, &value, 1))
				{
					write_error = true;
				}
			}
			if((addr + 1) == 0) overwrapping = true;
		} while(pos += 2, ++addr, (--size) && ((len -= 2) >= 0));
	}

	if(write_error)
	{
		if(verbose)
		{
			logger << DebugWarning << memory_import.GetName() << "->WriteMemory has reported an error" << EndDebugWarning;
		}
	}

	return write_error ? PutPacket("E00") :  PutPacket("OK");
}

template <class ADDRESS>
void GDBServer<ADDRESS>::Kill()
{
	if(sock >= 0)
	{
#if defined(WIN32) || defined(WIN64)
		closesocket(sock);
#else
		close(sock);
#endif
		sock = -1;
	}
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::ReportProgramExit()
{
	return PutPacket("W00");
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::ReportSignal(unsigned int signum)
{
	char packet[4];
	sprintf(packet, "S%02x", signum);
	return PutPacket(packet);
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::ReportTracePointTrap()
{
	string packet("T05");
	vector<GDBRegister>::const_iterator gdb_reg;

	if(gdb_pc)
	{
		std::stringstream sstr;
		string hex;
		unsigned int reg_num = gdb_pc->GetRegNum();
		gdb_pc->GetValue(hex);
		sstr << std::hex << reg_num;
		packet += sstr.str();
		packet += ":";
		packet += hex;
		packet += ";";
	}

	return PutPacket(packet);
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::SetBreakpointWatchpoint(uint32_t type, ADDRESS addr, uint32_t size)
{
	uint32_t i;

// 	cout << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__
// 	     << ": type = " << type
// 	     << " address = 0x" << hex << addr << dec
// 	     << " size = " << size << endl;

	switch(type)
	{
		case 0:
		case 1:
			for(i = 0; i < size; i++)
			{
				if(!debug_event_trigger_import->Listen(unisim::util::debug::Breakpoint<ADDRESS>(addr + i))) return PutPacket("E00");
			}
			return PutPacket("OK");
		case 2:
			if(debug_event_trigger_import->Listen(unisim::util::debug::Watchpoint<ADDRESS>(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, addr, size)))
			{
				return PutPacket("OK");
			}
			else
			{
				return PutPacket("E00");
			}
		case 3:
			if(debug_event_trigger_import->Listen(unisim::util::debug::Watchpoint<ADDRESS>(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, addr, size)))
			{
				return PutPacket("OK");
			}
			else
			{
				return PutPacket("E00");
			}

		case 4:
			if(!debug_event_trigger_import->Listen(unisim::util::debug::Watchpoint<ADDRESS>(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, addr, size)))
			{
				return PutPacket("E00");
			}
			
			if(debug_event_trigger_import->Listen(unisim::util::debug::Watchpoint<ADDRESS>(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, addr, size)))
			{
				return PutPacket("OK");
			}
			else
			{
				debug_event_trigger_import->Unlisten(unisim::util::debug::Watchpoint<ADDRESS>(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, addr, size));
				return PutPacket("E00");
			}
	}
	return PutPacket("E00");
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::RemoveBreakpointWatchpoint(uint32_t type, ADDRESS addr, uint32_t size)
{
	uint32_t i;

// 	cout << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__
// 	     << ": type = " << type
// 	     << " address = 0x" << hex << addr << dec
// 	     << " size = " << size << endl;

	switch(type)
	{
		case 0:
		case 1:
			for(i = 0; i < size; i++)
			{
				if(!debug_event_trigger_import->Unlisten(unisim::util::debug::Breakpoint<ADDRESS>(addr + i))) return PutPacket("E00");
			}
			return PutPacket("OK");

		case 2:
			if(debug_event_trigger_import->Unlisten(unisim::util::debug::Watchpoint<ADDRESS>(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, addr, size)))
			{
				return PutPacket("OK");
			}
			else
			{
				return PutPacket("E00");
			}
		case 3:
			if(debug_event_trigger_import->Unlisten(unisim::util::debug::Watchpoint<ADDRESS>(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, addr, size)))
			{
				return PutPacket("OK");
			}
			else
			{
				return PutPacket("E00");
			}
		case 4:
			{
				bool status = true;
				if(!debug_event_trigger_import->Unlisten(unisim::util::debug::Watchpoint<ADDRESS>(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, addr, size))) status = false;
				if(!debug_event_trigger_import->Unlisten(unisim::util::debug::Watchpoint<ADDRESS>(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, addr, size))) status = false;
				if(status)
				{
					return PutPacket("OK");
				}
				else
				{
					return PutPacket("E00");
				}
			}
	}
	return PutPacket("E00");
}

// *** Start ************ REDA ADDED CODE ****************

template <class ADDRESS>
void GDBServer<ADDRESS>::HandleQRcmd(string command) {

	size_t separator_index = command.find_first_of(':');
	string cmdPrefix;
	if (separator_index == string::npos) {
		cmdPrefix = command;
	} else {
		cmdPrefix = command.substr(0, separator_index);
	}

	if (cmdPrefix.compare("symboles") == 0) {

		/**
		 * command: qRcmd, symboles
		 * return "symbName:symbAddress:symbSize:symbType"
		 *
		 * with: symbType in {"FUNCTION", "VARIABLE"}
		 */
		if(symbol_table_lookup_import)
		{
			list<const Symbol<ADDRESS> *> func_symbols;
			symbol_table_lookup_import->GetSymbols(func_symbols, Symbol<ADDRESS>::SYM_FUNC);

			if (!func_symbols.empty()) {

				typename list<const Symbol<ADDRESS> *>::const_iterator symbol_iter;
				std::stringstream strstm;

				for(symbol_iter = func_symbols.begin(); symbol_iter != func_symbols.end(); symbol_iter++)
				{
					strstm << "O";
					strstm << (*symbol_iter)->GetName();

					strstm << ":" << std::hex;
					strstm.width(8);
					strstm << (*symbol_iter)->GetAddress();

					strstm << ":" << std::dec << (*symbol_iter)->GetSize();

					strstm << ":" << "FUNCTION";

					PutPacket(strstm.str());

					strstm.str(std::string());
				}
			}
		
			list<const Symbol<ADDRESS> *> obj_symbols;
			symbol_table_lookup_import->GetSymbols(func_symbols, Symbol<ADDRESS>::SYM_OBJECT);
		
			if (!obj_symbols.empty()) {

				typename list<const Symbol<ADDRESS> *>::const_iterator symbol_iter;
				std::stringstream strstm;

				for(symbol_iter = obj_symbols.begin(); symbol_iter != obj_symbols.end(); symbol_iter++)
				{

					strstm << "O";
					strstm << (*symbol_iter)->GetName();

					strstm << ":" << std::hex;
					strstm.width(8);
					strstm << (*symbol_iter)->GetAddress();

					strstm << ":" << std::dec << (*symbol_iter)->GetSize();

					strstm << ":" << "VARIABLE";

					PutPacket(strstm.str());

					strstm.str(std::string());

				}
			}

			PutPacket("T05");

		} else {
			PutPacket("E00");
		}

	}
	else if (cmdPrefix.compare("disasm") == 0) {
		/**
		 * command: qRcmd, disasm:address:size
		 *
		 * return "O<next_address>
         *        "O<disassembled code>"
         *        "T05"
		 *
		 */

		if (separator_index == string::npos) {
			PutPacket("E00");
		} else {
			separator_index++;
			ADDRESS symbol_address;
			ADDRESS symbol_size = 0;
			if(!ParseHex(command, separator_index, symbol_address)) {
				PutPacket("E00");
			} else if(command[separator_index++] != ':') {
				PutPacket("E00");
			} else if(!ParseHex(command, separator_index, symbol_size)) {
				PutPacket("E00");
			} else if(separator_index != command.length()) {
				PutPacket("E00");
			}

			Disasm(symbol_address, symbol_size);

			PutPacket("T05");

		}

	}
	else {
		PutPacket("E00");
	}

}

template <class ADDRESS>
void GDBServer<ADDRESS>::Disasm(ADDRESS symbol_address, unsigned int symbol_size)
{

	ADDRESS current_address = symbol_address;
	ADDRESS next_address = symbol_address;
	ADDRESS disassembled_size = 0;
	std::stringstream strstm;

	while (disassembled_size < symbol_size) {

		strstm.str(std::string());

		std::string dis = disasm_import->Disasm(current_address, next_address);

		if (current_address == next_address) {
			break;
		}

		strstm << "O" << hex << current_address << ":";

		strstm << hex;
		strstm.width(8);
		strstm << (current_address / memory_atom_size) << ":" << dec << dis << endl;
		strstm.fill(' ');

		PutPacket(strstm.str());

		disassembled_size += next_address - current_address;
		current_address = next_address;

	}

}

// *** End ************ REDA ADDED CODE ****************


} // end of namespace gdb_server
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif
