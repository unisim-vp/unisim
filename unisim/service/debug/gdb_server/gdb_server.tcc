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
 */
 
#ifndef __UNISIM_SERVICE_DEBUG_GDB_SERVER_GDB_SERVER_TCC__
#define __UNISIM_SERVICE_DEBUG_GDB_SERVER_GDB_SERVER_TCC__

#include <unisim/util/xml/xml.hh>

#include <iostream>
#include <list>

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#ifdef WIN32

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
using unisim::service::interfaces::operator<<;
using unisim::service::interfaces::Logger;
using unisim::service::interfaces::Hex;
using unisim::service::interfaces::Dec;
using unisim::service::interfaces::Endl;
using unisim::service::interfaces::DebugInfo;
using unisim::service::interfaces::DebugWarning;
using unisim::service::interfaces::DebugError;
using unisim::service::interfaces::EndDebugInfo;
using unisim::service::interfaces::EndDebugWarning;
using unisim::service::interfaces::EndDebugError;

inline char Nibble2HexChar(uint8_t v)
{
	v = v & 0xf; // keep only 4-bits
	return v < 10 ? '0' + v : 'a' + v - 10;
}

inline uint8_t HexChar2Nibble(char ch)
{
	if(ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
	if(ch >= '0' && ch <= '9') return ch - '0';
	if(ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
	return 0;
}

inline bool IsHexChar(char ch)
{
	if(ch >= 'a' && ch <= 'f') return true;
	if(ch >= '0' && ch <= '9') return true;
	if(ch >= 'A' && ch <= 'F') return true;
	return false;
}

template <class ADDRESS>
GDBServer<ADDRESS>::GDBServer(const char *_name, Object *_parent) :
	Object(_name, _parent),
	Service<DebugControl<ADDRESS> >(_name, _parent),
	Service<MemoryAccessReporting<ADDRESS> >(_name, _parent),
	Client<Memory<ADDRESS> >(_name, _parent),
	Client<Registers>(_name, _parent),
	Client<Logger>(_name, _parent),
	debug_control_export("debug-control-export", this),
	memory_access_reporting_export("memory-access-reporting-export", this),
	memory_import("memory-import", this),
	registers_import("cpu-registers-import", this),
	logger_import("logger-import", this),
	tcp_port(12345),
	architecture_description_filename(),
	sock(-1),
	gdb_registers(),
	gdb_pc(0),
	killed(false),
	trap(false),
	synched(false),
	breakpoint_registry(),
	watchpoint_registry(),
	running_mode(GDBSERVER_MODE_WAITING_GDB_CLIENT),
	extended_mode(false),
	counter(0),
	period(50),
	input_buffer_size(0),
	input_buffer_index(0),
	output_buffer_size(0),
	param_tcp_port("tcp-port", this, tcp_port),
	param_architecture_description_filename("architecture-description-filename", this, architecture_description_filename)
{
	SetupDependsOn(registers_import);

	counter = period;
}

template <class ADDRESS>
GDBServer<ADDRESS>::~GDBServer()
{
	if(sock >= 0)
	{
#ifdef WIN32
		closesocket(sock);
#else
		close(sock);
#endif
		sock = -1;
	}
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::Setup()
{
	input_buffer_size = 0;
	input_buffer_index = 0;
	output_buffer_size = 0;

	unisim::util::xml::Parser *parser = new unisim::util::xml::Parser();
	unisim::util::xml::Node *root_node = parser->Parse(architecture_description_filename);

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
					unisim::util::xml::Error((*root_node_property)->Filename(), (*root_node_property)->LineNo(), "WARNING! ignoring property '%s'",(*root_node_property)->Name().c_str());
				}
			}

			if(!has_architecture_name)
			{
				unisim::util::xml::Error(root_node->Filename(), root_node->LineNo(), "ERROR! architecture has no property 'name'");
				delete root_node;
				return false;
			}

			if(!has_architecture_endian)
			{
				unisim::util::xml::Error(root_node->Filename(), root_node->LineNo(), "WARNING! assuming target architecture endian is big endian");
			}

			const list<unisim::util::xml::Node *> *xml_nodes = root_node->Childs();
			list<unisim::util::xml::Node *>::const_iterator xml_node;
			bool has_program_counter = false;
			string program_counter_name;

			for(xml_node = xml_nodes->begin(); xml_node != xml_nodes->end(); xml_node++)
			{
				if((*xml_node)->Name() == string("register"))
				{
					string reg_name;
					bool has_reg_name = false;
					int reg_size = 0;
					bool has_reg_size = false;

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
							if((*xml_node_property)->Name() == string("size"))
							{
								reg_size = atoi((*xml_node_property)->Value().c_str());
								has_reg_size = true;
							}
							else
							{
								unisim::util::xml::Error((*xml_node_property)->Filename(), (*xml_node_property)->LineNo(), "WARNING! ignoring property '%s'",(*xml_node_property)->Name().c_str());
							}
						}
					}

					if(has_reg_name && has_reg_size)
					{
						bool cpu_has_reg = true;
						bool cpu_has_right_reg_size = true;
						
						typename Registers::Register *reg = registers_import->GetRegister(reg_name.c_str());
						
						if(!reg)
						{
							cpu_has_reg = false;
							if(logger_import)
							{
								(*logger_import) << DebugWarning;
								(*logger_import) << "CPU does not support register " << reg_name << Endl;
								(*logger_import) << EndDebugWarning;
							}
						}
						else
						{
							if(reg->GetSize() != reg_size)
							{
								cpu_has_right_reg_size = false;
								if(logger_import)
								{
									(*logger_import) << DebugWarning;
									(*logger_import) << ": register size (" << 8 * reg_size << " bits) doesn't match with size (" << 8 * reg->GetSize() << " bits) reported by CPU" << Endl;
									(*logger_import) << EndDebugWarning;
								}
							}
						}

						if(cpu_has_reg && cpu_has_right_reg_size)
						{
							gdb_registers.push_back(GDBRegister(reg, endian));
						}
						else
						{
							gdb_registers.push_back(GDBRegister(reg_name, reg_size, endian));
						}
					}
					else
					{
						unisim::util::xml::Error((*xml_node)->Filename(), (*xml_node)->LineNo(), "WARNING! node '%s' has no 'name' or 'size' property",(*xml_node)->Name().c_str());
					}
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
							unisim::util::xml::Error((*xml_node)->Filename(), (*xml_node)->LineNo(), "WARNING! node '%s' has no 'name'", (*xml_node)->Name().c_str());
							delete root_node;
							return false;
						}
					}
					else
					{
						unisim::util::xml::Error((*xml_node)->Filename(), (*xml_node)->LineNo(), "WARNING! ignoring tag '%s'", (*xml_node)->Name().c_str());
					}
				}
			}

			if(!has_program_counter)
			{
				unisim::util::xml::Error(root_node->Filename(), root_node->LineNo(), "ERROR! architecture has no program counter");
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
				unisim::util::xml::Error((*xml_program_counter_node)->Filename(), (*xml_program_counter_node)->LineNo(), "ERROR! program counter does not belong to registers");
				delete root_node;
				return false;
			}
		}
		else
		{
			unisim::util::xml::Error(root_node->Filename(), root_node->LineNo(), "ERROR! root node is not named 'architecture'");
			delete root_node;
			return false;
		}

		delete root_node;
	}
	else
	{
		unisim::util::xml::Error(architecture_description_filename, 0, "ERROR! no root node");
		return false;
	}
	
	struct sockaddr_in addr;
	int server_sock;

	server_sock = socket(AF_INET, SOCK_STREAM, 0);
	
	if(server_sock < 0)
	{
		if(logger_import)
		{
			(*logger_import) << DebugError;
			(*logger_import) << "socket failed" << Endl;
			(*logger_import) << EndDebugError;
		}
		return false;
	}
	
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(tcp_port);
	addr.sin_addr.s_addr = INADDR_ANY;
	if(bind(server_sock, (struct sockaddr *) &addr, sizeof(addr)) < 0)
	{
		if(logger_import)
		{
			(*logger_import) << DebugError;
			(*logger_import) << "bind failed" << Endl;
			(*logger_import) << EndDebugError;
		}
#ifdef WIN32
		closesocket(server_sock);
#else
		close(server_sock);
#endif
		return false;
	}

	if(listen(server_sock, 1))
	{
		if(logger_import)
		{
			(*logger_import) << DebugError;
			(*logger_import) << "listen failed" << Endl;
			(*logger_import) << EndDebugError;
		}
#ifdef WIN32
		closesocket(server_sock);
#else
		close(server_sock);
#endif
		return false;
	}

#ifdef WIN32
		int addr_len;
#else
		socklen_t addr_len;
#endif

	addr_len = sizeof(addr);
	sock = accept(server_sock, (struct sockaddr *) &addr, &addr_len);
	
	if(sock < 0)
	{
		if(logger_import)
		{
			(*logger_import) << DebugError;
			(*logger_import) << "accept failed" << Endl;
			(*logger_import) << EndDebugError;
		}
#ifdef WIN32
		closesocket(server_sock);
#else
		close(server_sock);
#endif
		return false;
	}
	
	if(logger_import)
	{
		(*logger_import) << DebugInfo;
		(*logger_import) << "Connection with GDB client established" << Endl;
		(*logger_import) << EndDebugInfo;
	}

    /* set short latency */
    int opt = 1;
    if(setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *) &opt, sizeof(opt)) < 0)
	{
		if(logger_import)
		{
			(*logger_import) << DebugWarning;
			(*logger_import) << "setsockopt failed requesting short latency" << Endl;
			(*logger_import) << EndDebugWarning;
		}
	}


#ifdef WIN32
	u_long NonBlock = 1;
	if(ioctlsocket(sock, FIONBIO, &NonBlock) != 0)
	{
		if(logger_import)
		{
			(*logger_import) << DebugError;
			(*logger_import) << "ioctlsocket failed" << Endl;
			(*logger_import) << EndDebugError;
		}
#ifdef WIN32
		closesocket(server_sock);
		closesocket(sock);
#else
		close(server_sock);
		close(sock);
#endif
		sock = -1;
		return false;
	}
#else
	int socket_flag = fcntl(sock, F_GETFL, 0);
	
	if(socket_flag < 0)
	{
		if(logger_import)
		{
			(*logger_import) << DebugError;
			(*logger_import) << "fcntl failed" << Endl;
			(*logger_import) << EndDebugError;
		}
#ifdef WIN32
		closesocket(server_sock);
		closesocket(sock);
#else
		close(server_sock);
		close(sock);
#endif
		sock = -1;
		return false;
	}
	
	/* Ask for non-blocking reads on socket */
	if(fcntl(sock, F_SETFL, socket_flag | O_NONBLOCK) < 0)
	{
		if(logger_import)
		{
			(*logger_import) << DebugError;
			(*logger_import) << "fcntl failed" << Endl;
			(*logger_import) << EndDebugError;
		}
#ifdef WIN32
		closesocket(server_sock);
		closesocket(sock);
#else
		close(server_sock);
		close(sock);
#endif
		sock = -1;
		return false;
	}
#endif
	
#ifdef WIN32
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

GDBRegister::GDBRegister(const string& reg_name, int reg_size, GDBEndian endian)
{
	this->name = reg_name;
	this->size = reg_size;
	this->endian = endian;
	this->reg = 0;
}

GDBRegister::GDBRegister(Registers::Register *reg, GDBEndian endian)
{
	this->name = reg->GetName();
	this->size = reg->GetSize();
	this->endian = endian;
	this->reg = reg;
}

bool GDBRegister::SetValue(const string& hex)
{
	int i;
	unsigned int len = hex.length();
	unsigned int pos = 0;
	uint8_t value[size];

	if(endian == GDB_BIG_ENDIAN)
	{
#if BYTE_ORDER == BIG_ENDIAN
		for(i = 0; i < size && pos < len; i++, pos += 2)
#else
		for(i = size - 1; i >= 0 && pos < len; i--, pos += 2)
#endif
		{
			if(!IsHexChar(hex[pos]) || !IsHexChar(hex[pos + 1])) return false;
	
			value[i] = (HexChar2Nibble(hex[pos]) << 4) | HexChar2Nibble(hex[pos + 1]);
		}
	}
	else
	{
#if BYTE_ORDER == LITTLE_ENDIAN
		for(i = 0; i < size && pos < len; i++, pos += 2)
#else
		for(i = size - 1; i >= 0 && pos < len; i--, pos += 2)
#endif
		{
			if(!IsHexChar(hex[pos]) || !IsHexChar(hex[pos + 1])) return false;
	
			value[i] = (HexChar2Nibble(hex[pos]) << 4) | HexChar2Nibble(hex[pos + 1]);
		}
	}

	if(reg)	reg->SetValue(&value);
	return true;
}

void GDBRegister::SetValue(const void *buffer)
{
	if(reg) reg->SetValue(buffer);
}

void GDBRegister::GetValue(string& hex) const
{
	int i;
	char c[2];
	c[1] = 0;
	uint8_t value[size];
	memset(value, 0, size);
	if(reg)
		reg->GetValue(&value);

	hex.erase();

	if(endian == GDB_BIG_ENDIAN)
	{
#if BYTE_ORDER == BIG_ENDIAN
		for(i = 0; i < size; i++)
#else
		for(i = size - 1; i >= 0; i--)
#endif
		{
			c[0] = Nibble2HexChar(value[i] >> 4);
			hex += c;
			c[0] = Nibble2HexChar(value[i] & 0xf);
			hex += c;
		}
	}
	else
	{
#if BYTE_ORDER == LITTLE_ENDIAN
		for(i = 0; i < size; i++)
#else
		for(i = size - 1; i >= 0; i--)
#endif
		{
			c[0] = Nibble2HexChar(value[i] >> 4);
			hex += c;
			c[0] = Nibble2HexChar(value[i] & 0xf);
			hex += c;
		}
	}
}

void GDBRegister::GetValue(void *buffer) const
{
	memset(buffer, 0, size);
	if(reg)
		reg->GetValue(buffer);
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::ParseHex(const string& s, unsigned int& pos, ADDRESS& value)
{
	unsigned int len = s.length();
	unsigned int n = 0;
	
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
void GDBServer<ADDRESS>::ReportMemoryAccess(typename MemoryAccessReporting<ADDRESS>::MemoryAccessType mat, typename MemoryAccessReporting<ADDRESS>::MemoryType mt, ADDRESS addr, uint32_t size)
{
	if(watchpoint_registry.HasWatchpoint(mat, mt, addr, size))
	{
		trap = true;
		synched = false;
	}
}

template <class ADDRESS>
void GDBServer<ADDRESS>::ReportFinishedInstruction(ADDRESS next_addr)
{
	if(breakpoint_registry.HasBreakpoint(next_addr))
	{
		trap = true;
		synched = false;
	}
}

template <class ADDRESS>
void GDBServer<ADDRESS>::Trap()
{
	//trap = true;
	//synched = false;
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
			return DebugControl<ADDRESS>::DBG_KILL;
		}
	
		unsigned int pos = 0;
		unsigned int len = packet.length();

		switch(packet[pos++])
		{
			case 'g':
				if(!ReadRegisters()) return DebugControl<ADDRESS>::DBG_KILL;
				break;

			case 'p':
				if(!ParseHex(packet, pos, reg_num)) break;
				if(!ReadRegister(reg_num)) return DebugControl<ADDRESS>::DBG_KILL;
				break;

			case 'G':
				if(WriteRegisters(packet.substr(1)))
					PutPacket("OK");
				else
					PutPacket("E00");
				break;

			case 'P':
				if(!ParseHex(packet, pos, reg_num)) break;
				if(packet[pos++] != '=') break;
				if(WriteRegister(reg_num, packet.substr(pos)))
					PutPacket("OK");
				else
					PutPacket("E00");
				break;

			case 'm':
				if(!ParseHex(packet, pos, addr)) break;
				if(packet[pos++] != ',') break;
				if(!ParseHex(packet, pos, size)) break;
				if(pos != len) break;
				if(!ReadMemory(addr, size)) return DebugControl<ADDRESS>::DBG_KILL;
				break;
	
			case 'M':
				if(!ParseHex(packet, pos, addr)) break;
				if(packet[pos++] != ',') break;
				if(!ParseHex(packet, pos, size)) break;
				if(packet[pos++] != ':') break;
				if(WriteMemory(addr, packet.substr(pos), size))
					PutPacket("OK");
				else
					PutPacket("E00");
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
					if(logger_import)
					{
						(*logger_import) << DebugWarning;
						(*logger_import) << "CPU has no program counter" << Endl;
						(*logger_import) << EndDebugWarning;
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
				if(gdb_pc)
					gdb_pc->SetValue(&addr);
				else
				{
					(*logger_import) << DebugWarning;
					(*logger_import) << "CPU has no program counter" << Endl;
					(*logger_import) << EndDebugWarning;
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
				if(SetBreakpointWatchpoint(type, addr, size))
					PutPacket("OK");
				else
					PutPacket("E00");
				break;
	
			case 'z':
				if(!ParseHex(packet, pos, type)) break;
				if(packet[pos++] != ',') break;
				if(!ParseHex(packet, pos, addr)) break;
				if(packet[pos++] != ',') break;
				if(!ParseHex(packet, pos, size)) break;
				if(RemoveBreakpointWatchpoint(type, addr, size))
					PutPacket("OK");
				else
					PutPacket("E00");
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
				else
				{
					if(logger_import)
					{
						(*logger_import) << DebugWarning;
						(*logger_import) << "Received an unknown GDB remote protocol packet" << Endl;
						(*logger_import) << EndDebugWarning;
					}
					PutPacket("");
				}
		} // end of switch
	}
	return DebugControl<ADDRESS>::DBG_KILL;
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::GetChar(char& c, bool blocking)
{
	if(input_buffer_size == 0)
	{
		do
		{
#ifdef WIN32
			int r = recv(sock, input_buffer, sizeof(input_buffer), 0);
			if(r == 0 || r == SOCKET_ERROR)
#else
			ssize_t r = read(sock, input_buffer, sizeof(input_buffer));
			if(r <= 0)
#endif
			{
#ifdef WIN32
				if(r == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK)
#else
				if(r < 0 && errno == EAGAIN)
#endif
				{
					if(blocking)
					{
#ifdef WIN32
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
			
				if(logger_import)
				{
					(*logger_import) << DebugError;
					(*logger_import) << "can't read from socket" << Endl;
					(*logger_import) << EndDebugError;
				}
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
#ifdef WIN32
			int r = send(sock, output_buffer + index, output_buffer_size, 0);
			if(r == 0 || r == SOCKET_ERROR)
#else
			ssize_t r = write(sock, output_buffer + index, output_buffer_size);
			if(r <= 0)
#endif
			{
				if(logger_import)
				{
					(*logger_import) << DebugError;
					(*logger_import) << "can't write into socket" << Endl;
					(*logger_import) << EndDebugError;
				}
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
			
			if(logger_import)
			{
				(*logger_import) << DebugInfo;
				(*logger_import) << "receiving $" << s << "#" << Nibble2HexChar((received_checksum >> 4) & 0xf) << Nibble2HexChar(received_checksum & 0xf) << Endl;
				(*logger_import) << EndDebugInfo;
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
		if(logger_import)
		{
			(*logger_import) << DebugInfo;
			(*logger_import) << "sending $" << s << "#" << Nibble2HexChar(checksum >> 4) << Nibble2HexChar(checksum & 0xf) << Endl;
			(*logger_import) << EndDebugInfo;
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
	for(i = 0; i < count; i++, s++, p += 2)
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
		packet += hex;
	}
	return PutPacket(packet);
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::WriteRegisters(const string& hex)
{
	vector<GDBRegister>::iterator gdb_reg;
	unsigned int pos = 0;

	for(gdb_reg = gdb_registers.begin(); gdb_reg != gdb_registers.end(); pos += gdb_reg->GetHexLength(), gdb_reg++)
	{
		if(!gdb_reg->SetValue(hex.substr(pos, gdb_reg->GetHexLength()))) return false;
	}

	return true;
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::ReadRegister(unsigned int regnum)
{
	if(regnum >= gdb_registers.size()) return false;
	const GDBRegister& gdb_reg = gdb_registers[regnum];
	string packet;
	gdb_reg.GetValue(packet);
	return PutPacket(packet);
}

template <class ADDRESS>
bool GDBServer<ADDRESS>::WriteRegister(unsigned int regnum, const string& hex)
{
	if(regnum >= gdb_registers.size()) return false;
	GDBRegister& gdb_reg = gdb_registers[regnum];
	return gdb_reg.SetValue(hex);
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

	if(read_error && logger_import)
	{
		(*logger_import) << DebugWarning;
		(*logger_import) << memory_import.GetName() << "->ReadMemory has reported an error" << Endl;
		(*logger_import) << EndDebugWarning;
	}

	return PutPacket(packet);
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

	if(write_error && logger_import)
	{
		(*logger_import) << DebugWarning;
		(*logger_import) << memory_import.GetName() << "->WriteMemory has reported an error" << Endl;
		(*logger_import) << EndDebugWarning;
	}

	return true;
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
	return PutPacket("T05");
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
				if(!breakpoint_registry.SetBreakpoint(addr + i)) return false;
			}
			return true;

		case 2:
			return watchpoint_registry.SetWatchpoint(MemoryAccessReporting<ADDRESS>::MAT_WRITE, MemoryAccessReporting<ADDRESS>::MT_DATA, addr, size);

		case 3:
			return watchpoint_registry.SetWatchpoint(MemoryAccessReporting<ADDRESS>::MAT_READ, MemoryAccessReporting<ADDRESS>::MT_DATA, addr, size);

		case 4:
			if(!watchpoint_registry.SetWatchpoint(MemoryAccessReporting<ADDRESS>::MAT_READ, MemoryAccessReporting<ADDRESS>::MT_DATA, addr, size)) return false;
			if(!watchpoint_registry.SetWatchpoint(MemoryAccessReporting<ADDRESS>::MAT_WRITE, MemoryAccessReporting<ADDRESS>::MT_DATA, addr, size)) return false;
			return true;
	}
	return false;
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
				if(!breakpoint_registry.RemoveBreakpoint(addr + i)) return false;
			}
			return true;

		case 2:
			return watchpoint_registry.RemoveWatchpoint(MemoryAccessReporting<ADDRESS>::MAT_WRITE, MemoryAccessReporting<ADDRESS>::MT_DATA, addr, size);

		case 3:
			return watchpoint_registry.RemoveWatchpoint(MemoryAccessReporting<ADDRESS>::MAT_READ, MemoryAccessReporting<ADDRESS>::MT_DATA, addr, size);

		case 4:
			if(!watchpoint_registry.RemoveWatchpoint(MemoryAccessReporting<ADDRESS>::MAT_READ, MemoryAccessReporting<ADDRESS>::MT_DATA, addr, size)) return false;
			if(!watchpoint_registry.RemoveWatchpoint(MemoryAccessReporting<ADDRESS>::MAT_WRITE, MemoryAccessReporting<ADDRESS>::MT_DATA, addr, size)) return false;
			return true;
	}
	return false;
}

} // end of namespace gdb_server
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif
