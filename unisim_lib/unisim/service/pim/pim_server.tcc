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
 *          Reda   Nouacer      (reda.nouacer@cea.fr)
 */

#ifndef __UNISIM_SERVICE_PIM_PIM_SERVER_TCC__
#define __UNISIM_SERVICE_PIM_PIM_SERVER_TCC__

#include <unisim/util/xml/xml.hh>

#include <iostream>
#include <sstream>
#include <list>

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <math.h>

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
namespace pim {

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

using unisim::kernel::service::Statistic;

using unisim::util::debug::Statement;

using unisim::service::pim::PIMThread;

template <class ADDRESS>
PIMServer<ADDRESS>::PIMServer(const char *_name, Object *_parent)
	: Object(_name, _parent, "PIM Server")
	, Service<DebugControl<ADDRESS> >(_name, _parent)
	, Service<MemoryAccessReporting<ADDRESS> >(_name, _parent)
	, Service<TrapReporting>(_name, _parent)
	, Client<MemoryAccessReportingControl>(_name, _parent)
	, Client<Memory<ADDRESS> >(_name, _parent)
	, Client<Disassembly<ADDRESS> >(_name, _parent)
	, Client<SymbolTableLookup<ADDRESS> >(_name, _parent)
	, Client<StatementLookup<ADDRESS> >(_name, _parent)
	, Client<Registers>(_name, _parent)
	, SocketThread()
	, debug_control_export("debug-control-export", this)
	, memory_access_reporting_export("memory-access-reporting-export", this)
	, trap_reporting_export("trap-reporting-export", this)
	, memory_access_reporting_control_import("memory_access_reporting_control_import", this)
	, memory_import("memory-import", this)
	, registers_import("cpu-registers-import", this)
	, disasm_import("disasm-import", this)
	, symbol_table_lookup_import("symbol-table-lookup-import", this)
	, stmt_lookup_import("stmt-lookup-import", this)

	, logger(*this)
	, tcp_port(12345)
	, architecture_description_filename()
//	, gdb_registers()
//	, gdb_pc(0)
	, pc_reg(0)
	, endian (GDB_BIG_ENDIAN)
	, killed(false)
	, trap(false)
	, synched(false)
	, breakpoint_registry()
	, watchpoint_registry()
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
	, param_host("host", this, fHost)

//	, last_time_ratio(1e+9)

{

	memory_access_reporting_export.SetupDependsOn(memory_access_reporting_control_import);

	counter = period;

}

template <class ADDRESS>
PIMServer<ADDRESS>::~PIMServer()
{
	if(sockfd >= 0)
	{
		string packet("W00");
		PutPacket(packet);
#ifdef WIN32
		closesocket(sockfd);
#else
		close(sockfd);
#endif
		sockfd = -1;
	}

	if (socketServer) { delete socketServer; socketServer = NULL;}
	if (target) { delete target; target = NULL; }
	if (pimServerThread) { delete pimServerThread; pimServerThread = NULL; }

//	pim_trace_file.close();

}

template <class ADDRESS>
double PIMServer<ADDRESS>::GetSimTime() {
	return Object::GetSimulator()->GetSimTime();
}

template <class ADDRESS>
double PIMServer<ADDRESS>::GetHostTime() {
	return Object::GetSimulator()->GetHostTime();
}

template <class ADDRESS>
bool PIMServer<ADDRESS>::BeginSetup() {

	if(memory_atom_size != 1 &&
	   memory_atom_size != 2 &&
	   memory_atom_size != 4 &&
	   memory_atom_size != 8 &&
	   memory_atom_size != 16)
	{
		cerr << Object::GetName() << "ERROR! memory-atom-size must be either 1, 2, 4, 8 or 16" << endl;
		return false;
	}

	// Open Socket Stream
	// connection_req_nbre parameter has to be set to two "2" connections. once is for GDB requests and the second is for PIM requests
	socketServer = new SocketServerThread(GetHost(), GetTCPPort(), true, 2);

	socketServer->setProtocolHandler(this);

	socketServer->start();

	// Load Architecture informations from simulator
	bool has_architecture_name = false;
	bool has_architecture_endian = false;

	bool has_program_counter = false;
	string program_counter_name;

	VariableBase* architecture_name = Simulator::simulator->FindParameter("program-name");
	has_architecture_name = (architecture_name != NULL);
	if(!has_architecture_name)
	{
		logger << DebugError << "architecture has no property 'name'" << std::endl << EndDebugError;
		return false;
	}

	VariableBase* architecture_endian = Simulator::simulator->FindParameter("endian");
	if (architecture_endian != NULL) {
		string endianstr = *architecture_endian;
		if(endianstr == "little")
		{
			endian = GDB_LITTLE_ENDIAN;
			has_architecture_endian = true;
		}
		else if(endianstr == "big")
		{
			endian = GDB_BIG_ENDIAN;
			has_architecture_endian = true;
		}
	}

	if(!has_architecture_endian)
	{
		logger << DebugWarning << "assuming target architecture endian is 'big endian'" << std::endl << EndDebugWarning;
	}

	VariableBase* param_program_counter_name = Simulator::simulator->FindParameter("program-counter-name");

	if (param_program_counter_name != NULL) {
		pc_reg = (VariableBase *) Simulator::simulator->FindRegister(((string) *param_program_counter_name).c_str());
		if (pc_reg != NULL) {
			has_program_counter = true;
		} else {
			logger << DebugWarning << "Simulator has no <program-counter> register named '" << (string) *param_program_counter_name << "'" << std::endl << EndDebugWarning;
		}
	} else {
		logger << DebugWarning << "Simulator has no <program-counter-name> parameter" << std::endl << EndDebugWarning;
	}

	std::list<VariableBase *> lst;

	Simulator::simulator->GetRegisters(lst);

	uint32_t index = 0;
	for (std::list<VariableBase *>::iterator it = lst.begin(); it != lst.end(); it++) {

		if (!((VariableBase *) *it)->IsVisible()) continue;

		simulator_registers.push_back((VariableBase *) *it);

		if (has_program_counter) {
			if (((VariableBase *) *it) == pc_reg) {
				pc_reg_index = index;
			}
		}

		index++;
	}

	lst.clear();

	return true;
}

template <class ADDRESS>
bool PIMServer<ADDRESS>::Setup(ServiceExportBase *srv_export) {

	if(memory_access_reporting_control_import)
	{
		memory_access_reporting_control_import->RequiresMemoryAccessReporting(
				false);
		memory_access_reporting_control_import->RequiresFinishedInstructionReporting(
				false);
	}


	return true;
}

template <class ADDRESS>
bool PIMServer<ADDRESS>::EndSetup() {

//	pim_trace_file.open ("pim_trace.xls");

	return true;
}

template <class ADDRESS>
void PIMServer<ADDRESS>::Stop(int exit_status) {

	if (socketServer) { socketServer->stop();}
	if (target) { target->stop();}
	if (pimServerThread) { pimServerThread->stop();}

}

template <class ADDRESS>
void PIMServer<ADDRESS>::OnDisconnect()
{
}

template <class ADDRESS>
bool PIMServer<ADDRESS>::ParseHex(const string& s, unsigned int& pos, ADDRESS& value)
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
void PIMServer<ADDRESS>::ReportMemoryAccess(typename MemoryAccessReporting<ADDRESS>::MemoryAccessType mat, typename MemoryAccessReporting<ADDRESS>::MemoryType mt, ADDRESS addr, uint32_t size)
{

	if(watchpoint_registry.HasWatchpoint(mat, mt, addr, size))
	{

		trap = true;
		synched = false;
	}
}

template <class ADDRESS>
void PIMServer<ADDRESS>::ReportFinishedInstruction(ADDRESS addr, ADDRESS next_addr)
{
	if(breakpoint_registry.HasBreakpoint(next_addr))
	{
		trap = true;
		synched = false;
	}
}

template <class ADDRESS>
void PIMServer<ADDRESS>::ReportTrap()
{
	trap = true;
	synched = false;
}

template <class ADDRESS>
void
PIMServer<ADDRESS>::
ReportTrap(const unisim::kernel::service::Object &obj)
{
	ReportTrap();
}

template <class ADDRESS>
void
PIMServer<ADDRESS>::
ReportTrap(const unisim::kernel::service::Object &obj,
		   const std::string &str)
{
	ReportTrap();
}

template <class ADDRESS>
void
PIMServer<ADDRESS>::
ReportTrap(const unisim::kernel::service::Object &obj,
		   const char *c_str)
{
	ReportTrap();
}

template <class ADDRESS>
typename DebugControl<ADDRESS>::DebugCommand PIMServer<ADDRESS>::FetchDebugCommand(ADDRESS cia)
{
	ADDRESS addr;
	ADDRESS size;
	ADDRESS reg_num;
	ADDRESS type;

/*
	- add a time_ratio = HotsTime/SimulatedTime response
	- the time_ratio is used by timed/periodic operations
*/


//	double new_time_ratio = last_time_ratio;
//	double sim_time = GetSimTime();
//	double host_time = GetHostTime();
//	if (sim_time > 0) {
//		new_time_ratio = host_time / sim_time;
//	}
//	if ((sim_time == 0) || (fabs(last_time_ratio - new_time_ratio) > 0.1)) {
//		pim_trace_file << (sim_time * 1000) << " \t" << (new_time_ratio) << endl;
//		last_time_ratio = new_time_ratio;
//	}


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

		unsigned int pos = 0;
		unsigned int len = packet.length();

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
				if(WriteRegisters(packet.substr(1)))
//					PutPacket("OK");
					OutputText("OK", 2);
				else
//					PutPacket("E00");
					OutputText("E00", 3);
				break;

			case 'P':
				if(!ParseHex(packet, pos, reg_num)) break;
				if(packet[pos++] != '=') break;
				if(WriteRegister(reg_num, packet.substr(pos)))
//					PutPacket("OK");
					OutputText("OK", 2);
				else
//					PutPacket("E00");
					OutputText("E00", 3);
				break;

			case 'm':
				if(!ParseHex(packet, pos, addr)) break;
				if(packet[pos++] != ',') break;
				if(!ParseHex(packet, pos, size)) break;
				if(pos != len) break;
				if(!ReadMemory(addr, size))
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
				if(WriteMemory(addr, packet.substr(pos), size))
//					PutPacket("OK");
					OutputText("OK", 2);
				else
//					PutPacket("E00");
					OutputText("E00", 3);
				break;

			case 's':
				if(!ParseHex(packet, pos, addr))
				{
					running_mode = GDBSERVER_MODE_STEP;
					synched = false;
					return DebugControl<ADDRESS>::DBG_STEP;
				}
				if (pc_reg) {
					*pc_reg = addr;
				}
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
				if (pc_reg) {
					*pc_reg = addr;
				}
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
//				PutPacket("OK");
				OutputText("OK", 2);
				break;

			case 'Z':
				if(!ParseHex(packet, pos, type)) break;
				if(packet[pos++] != ',') break;
				if(!ParseHex(packet, pos, addr)) break;
				if(packet[pos++] != ',') break;
				if(!ParseHex(packet, pos, size)) break;
				if(SetBreakpointWatchpoint(type, addr, size))
//					PutPacket("OK");
					OutputText("OK", 2);
				else
//					PutPacket("E00");
					OutputText("E00", 3);
				break;

			case 'z':
				if(!ParseHex(packet, pos, type)) break;
				if(packet[pos++] != ',') break;
				if(!ParseHex(packet, pos, addr)) break;
				if(packet[pos++] != ',') break;
				if(!ParseHex(packet, pos, size)) break;
				if(RemoveBreakpointWatchpoint(type, addr, size))
//					PutPacket("OK");
					OutputText("OK", 2);
				else
//					PutPacket("E00");
					OutputText("E00", 3);
				break;

			default:

				if(packet.substr(0, 9) == "qSymbol::")
				{
					PutPacket("qSymbol::");

					if (!HandleSymbolLookup()) {
						Object::Stop(0);
						return DebugControl<ADDRESS>::DBG_KILL;
					}
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
		} // end of switch

	}
	Object::Stop(0);
	return DebugControl<ADDRESS>::DBG_KILL;
}


template <class ADDRESS>
bool PIMServer<ADDRESS>::FlushOutput()
{
	if(output_buffer_size > 0)
	{
		unsigned int index = 0;
		do
		{
#ifdef WIN32
			int r = send(sockfd, output_buffer + index, output_buffer_size, 0);
			if(r == 0 || r == SOCKET_ERROR)
#else
			ssize_t r = write(sockfd, output_buffer + index, output_buffer_size);
			if(r <= 0)
#endif
			{
				logger << DebugError << "can't write into socket" << EndDebugError;
				return false;
			}

			index += r;
			output_buffer_size -= r;
		}
		while(output_buffer_size > 0);
	}

	return true;
}

template <class ADDRESS>
bool PIMServer<ADDRESS>::PutChar(char c)
{
	if(output_buffer_size >= sizeof(output_buffer))
	{
		if(!FlushOutput()) return false;
	}

	output_buffer[output_buffer_size++] = c;
	return true;
}

template <class ADDRESS>
bool PIMServer<ADDRESS>::GetPacket(string& s, bool blocking)
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
bool PIMServer<ADDRESS>::PutPacket(const string& s)
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
bool PIMServer<ADDRESS>::OutputText(const char *s, int count)
{
	string packet = "";
	string tmpPacket;

	TextToHex(s, count, tmpPacket);

	packet.append("O");

	packet.append(tmpPacket);

	return PutPacket(packet);
}

template <class ADDRESS>
bool PIMServer<ADDRESS>::ReadRegisters()
{

	string packet;

	for (vector<VariableBase *>::iterator it = simulator_registers.begin(); it != simulator_registers.end(); it++) {
		ADDRESS val = **it;
		string hex;

		Number2HexString((uint8_t*) &val, ((VariableBase *) *it)->GetBitSize()/8, hex, (endian == GDB_BIG_ENDIAN)? "big":"little");

		packet += hex;
	}

	return PutPacket(packet);

}

template <class ADDRESS>
bool PIMServer<ADDRESS>::WriteRegisters(const string& hex)
{

	unsigned int pos = 0;

	for(vector<VariableBase *>::iterator it = simulator_registers.begin(); it != simulator_registers.end(); pos += 2 * (*it)->GetBitSize()/8, it++)
	{
		ADDRESS val = 0;
		string subhex = hex.substr(pos, 2 * (*it)->GetBitSize()/8);
		if (HexString2Number(subhex, &val, (*it)->GetBitSize()/8, (endian == GDB_BIG_ENDIAN)? "big":"little")) {
			*(*it) = val;
		} else {
			return false;
		}

	}

	return true;
}

template <class ADDRESS>
bool PIMServer<ADDRESS>::ReadRegister(unsigned int regnum)
{

	string packet;
	string endianstr;
	ADDRESS val = *(simulator_registers[regnum]);
	Number2HexString((uint8_t*) &val, (simulator_registers[regnum])->GetBitSize()/8, packet, (endian == GDB_BIG_ENDIAN)? "big":"little");

	return PutPacket(packet);

}

template <class ADDRESS>
bool PIMServer<ADDRESS>::WriteRegister(unsigned int regnum, const string& hex)
{

	string packet;
	ADDRESS val = 0;
	unsigned int pos = 0;

	if (HexString2Number(hex, &val, (simulator_registers[regnum])->GetBitSize()/8, (endian == GDB_BIG_ENDIAN)? "big":"little")) {
		*(simulator_registers[regnum]) = val;
	} else {
		return false;
	}

	return true;
}

template <class ADDRESS>
bool PIMServer<ADDRESS>::HandleSymbolLookup() {

	while (true) {

		string packet;
		bool	found_error = false;

		if(!GetPacket(packet, true))
		{
			return false;
		}

		if (packet.compare("OK") == 0) {
			break;
		} else if (packet.substr(0, 8).compare("qSymbol:") == 0) {
			vector<string> segments;

			StringSplit(packet, ":", segments);

			string name;
			string value;

			if (segments.size() == 2) // read request
			{
				HexToText(segments[1].c_str(), segments[1].size(), name);

				ReadSymbol(name);

			}
			else if (segments.size() == 3) // write_symbol request
			{
				HexToText(segments[2].c_str(), segments[2].size(), name);

				WriteSymbol(name, segments[1]);
			}
			else {
				found_error = true;
			}

		} else {
			found_error = true;
		}

		if (found_error) {
			if(verbose)
			{
				logger << DebugWarning << "HandleSymbolLookup:: unknown command (" << packet << ")" << EndDebugWarning;
			}
		}
	}

	return true;
}

template <class ADDRESS>
bool PIMServer<ADDRESS>::WriteSymbol(const string name, const string& hexValue) {

	list<const Symbol<ADDRESS> *> symbol_registries;

	if (symbol_table_lookup_import) {
		symbol_table_lookup_import->GetSymbols(symbol_registries, Symbol<ADDRESS>::SYM_OBJECT);
	}
	typename list<const Symbol<ADDRESS> *>::const_iterator symbol_iter;

	string packet = "";

	for(symbol_iter = symbol_registries.begin(); symbol_iter != symbol_registries.end(); symbol_iter++)
	{

		if (name.compare((*symbol_iter)->GetName()) == 0) {

			uint32_t size = (*symbol_iter)->GetSize();
			uint8_t *value = (uint8_t*) malloc(size);
			memset(value, 0, size);

			uint32_t value_index;
			uint8_t val;
			uint32_t hex_index;
			if ((hexValue.length() % 2) == 0) {
				hex_index = 0;
				value_index = size - (hexValue.length() / 2);
			} else {
				hex_index = 1;
				value_index = size - (hexValue.length() / 2) - 1;
				value[value_index++] = HexChar2Nibble(hexValue[0]);
			}
			for (; (hex_index < hexValue.length()); hex_index = hex_index+2) {
				val = (HexChar2Nibble(hexValue[hex_index]) << 4) | HexChar2Nibble(hexValue[hex_index+1]);
				value[value_index++] = val;
			}

			if (!memory_import->WriteMemory((*symbol_iter)->GetAddress(), value, size)) {
				if(verbose)
				{
					logger << DebugWarning << memory_import.GetName() << "->WriteSymbol has reported an error" << EndDebugWarning;
				}

//				PutPacket("NOK");
				OutputText("NOK", 3);
				return false;
			} else {
//				PutPacket("OK");
				OutputText("OK", 2);
			}

			break;
		}

	}

	return true;

}

template <class ADDRESS>
bool PIMServer<ADDRESS>::ReadSymbol(const string name)
{

	list<const Symbol<ADDRESS> *> symbol_registries;

	if (symbol_table_lookup_import) {
		symbol_table_lookup_import->GetSymbols(symbol_registries, Symbol<ADDRESS>::SYM_OBJECT);
	}
	typename list<const Symbol<ADDRESS> *>::const_iterator symbol_iter;

	string packet = "";
	string value;
	int count = 0;

	for(symbol_iter = symbol_registries.begin(); symbol_iter != symbol_registries.end(); symbol_iter++)
	{

		if ((name.compare((*symbol_iter)->GetName()) == 0) || (name.compare("*") == 0)) {

			value = "";

			if(!InternalReadMemory((*symbol_iter)->GetAddress(), (*symbol_iter)->GetSize(), value))
			{
				if(verbose)
				{
					logger << DebugWarning << memory_import.GetName() << "->ReadSymbol has reported an error" << EndDebugWarning;
				}
			}

			string hexName;

			TextToHex((*symbol_iter)->GetName(), strlen((*symbol_iter)->GetName()), hexName);

			packet.append("qSymbol:");
			packet.append(value);
			packet.append(":");
			packet.append(hexName);

			count = (count + 1) % 10;

			if (name.compare("*") != 0) {
				break;
			} else {
				if (count == 0) {
					PutPacket(packet);
					packet = "";
				}

			}
		}

	}

	if (count > 0) {
		PutPacket(packet);
	}

	if (name.compare("*") == 0) {
		PutPacket("qSymbol:");
	}


	return true;

}

template <class ADDRESS>
bool PIMServer<ADDRESS>::InternalReadMemory(ADDRESS addr, uint32_t size, string& packet)
{
	bool read_error = false;
	bool overwrapping = false;

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
		return false;
	}

	return true;
}


template <class ADDRESS>
bool PIMServer<ADDRESS>::ReadMemory(ADDRESS addr, uint32_t size)
{
	string packet;

	if(!InternalReadMemory(addr, size, packet))
	{
		if(verbose)
		{
			logger << DebugWarning << memory_import.GetName() << "->ReadMemory has reported an error" << EndDebugWarning;
		}
	}

	return PutPacket(packet);
}

template <class ADDRESS>
bool PIMServer<ADDRESS>::WriteMemory(ADDRESS addr, const string& hex, uint32_t size)
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

	return true;
}

template <class ADDRESS>
void PIMServer<ADDRESS>::Kill()
{
	if(sockfd >= 0)
	{
#ifdef WIN32
		closesocket(sockfd);
#else
		close(sockfd);
#endif
		sockfd = -1;
	}
}

template <class ADDRESS>
bool PIMServer<ADDRESS>::ReportProgramExit()
{
	return PutPacket("W00");
}

template <class ADDRESS>
bool PIMServer<ADDRESS>::ReportSignal(unsigned int signum)
{
	char packet[4];
	sprintf(packet, "S%02x", signum);
	return PutPacket(packet);
}

template <class ADDRESS>
bool PIMServer<ADDRESS>::ReportTracePointTrap()
{
	string packet("T05");

	if(pc_reg)
	{
		std::stringstream sstr;
		string hex;
		ADDRESS val = (ADDRESS) *pc_reg;
		Number2HexString((uint8_t*) &val, pc_reg->GetBitSize()/8, hex, (endian == GDB_BIG_ENDIAN)? "big":"little");

		sstr << std::hex << pc_reg_index;
		packet += sstr.str();
		packet += ":";
		packet += hex;
		packet += ";";
	}

	return PutPacket(packet);
}

template <class ADDRESS>
bool PIMServer<ADDRESS>::SetBreakpointWatchpoint(uint32_t type, ADDRESS addr, uint32_t size)
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
			if(memory_access_reporting_control_import)
				memory_access_reporting_control_import->RequiresFinishedInstructionReporting(
						breakpoint_registry.HasBreakpoints());
			return true;
		case 2:
			if(watchpoint_registry.SetWatchpoint(MemoryAccessReporting<ADDRESS>::MAT_WRITE, MemoryAccessReporting<ADDRESS>::MT_DATA, addr, size))
			{
				if(memory_access_reporting_control_import)
					memory_access_reporting_control_import->RequiresMemoryAccessReporting(
							watchpoint_registry.HasWatchpoints());
				return true;
			}
			else
				return false;
		case 3:
			if(watchpoint_registry.SetWatchpoint(MemoryAccessReporting<ADDRESS>::MAT_READ, MemoryAccessReporting<ADDRESS>::MT_DATA, addr, size))
			{
				if(memory_access_reporting_control_import)
					memory_access_reporting_control_import->RequiresMemoryAccessReporting(
							watchpoint_registry.HasWatchpoints());
				return true;
			}
			else
				return false;

		case 4:
			if(watchpoint_registry.SetWatchpoint(MemoryAccessReporting<ADDRESS>::MAT_READ, MemoryAccessReporting<ADDRESS>::MT_DATA, addr, size))
			{
				if(memory_access_reporting_control_import)
					memory_access_reporting_control_import->RequiresMemoryAccessReporting(
							watchpoint_registry.HasWatchpoints());
			}
			else
				return false;
			if(watchpoint_registry.SetWatchpoint(MemoryAccessReporting<ADDRESS>::MAT_WRITE, MemoryAccessReporting<ADDRESS>::MT_DATA, addr, size))
			{
				if(memory_access_reporting_control_import)
					memory_access_reporting_control_import->RequiresMemoryAccessReporting(
							watchpoint_registry.HasWatchpoints());
				return true;
			}
			else
				return false;
	}
	return false;
}

template <class ADDRESS>
bool PIMServer<ADDRESS>::RemoveBreakpointWatchpoint(uint32_t type, ADDRESS addr, uint32_t size)
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
			if(memory_access_reporting_control_import)
				memory_access_reporting_control_import->RequiresFinishedInstructionReporting(
						breakpoint_registry.HasBreakpoints());
			return true;

		case 2:
			if(watchpoint_registry.RemoveWatchpoint(MemoryAccessReporting<ADDRESS>::MAT_WRITE, MemoryAccessReporting<ADDRESS>::MT_DATA, addr, size))
			{
				if(memory_access_reporting_control_import)
					memory_access_reporting_control_import->RequiresMemoryAccessReporting(
							breakpoint_registry.HasBreakpoints());
				return true;
			}
			else
				return false;
		case 3:
			if(watchpoint_registry.RemoveWatchpoint(MemoryAccessReporting<ADDRESS>::MAT_READ, MemoryAccessReporting<ADDRESS>::MT_DATA, addr, size))
			{
				if(memory_access_reporting_control_import)
					memory_access_reporting_control_import->RequiresMemoryAccessReporting(
							breakpoint_registry.HasBreakpoints());
				return true;
			}
			else
				return false;
		case 4:
			if(watchpoint_registry.RemoveWatchpoint(MemoryAccessReporting<ADDRESS>::MAT_READ, MemoryAccessReporting<ADDRESS>::MT_DATA, addr, size))
			{
				if(memory_access_reporting_control_import)
					memory_access_reporting_control_import->RequiresMemoryAccessReporting(
							breakpoint_registry.HasBreakpoints());
			}
			else
				return false;
			if(!watchpoint_registry.RemoveWatchpoint(MemoryAccessReporting<ADDRESS>::MAT_WRITE, MemoryAccessReporting<ADDRESS>::MT_DATA, addr, size))
			{
				if(memory_access_reporting_control_import)
					memory_access_reporting_control_import->RequiresMemoryAccessReporting(
							breakpoint_registry.HasBreakpoints());
				return true;
			}
			else
				return false;
	}
	return false;
}

// *** Start ************ REDA ADDED CODE ****************

template <class ADDRESS>
void PIMServer<ADDRESS>::HandleQRcmd(string command) {

	unsigned int separator_index = command.find_first_of(':');
	string cmdPrefix;

	if (separator_index == static_cast<unsigned int>(string::npos)) {
		cmdPrefix = command;
	} else {
		cmdPrefix = command.substr(0, separator_index);
	}

	if (cmdPrefix.compare("parameters") == 0) {
		Simulator *simulator = Object::GetSimulator();

		std::list<VariableBase *> lst;
		simulator->GetParameters(lst);

		std::stringstream strstm;

		for (std::list<VariableBase *>::iterator it = lst.begin(); it != lst.end(); it++) {

			if (!((VariableBase *) *it)->IsVisible()) continue;

			std::string name = ((VariableBase *) *it)->GetName();
			std::string dataType = ((VariableBase *) *it)->GetDataTypeName();
			std::string value = *((VariableBase *) *it);
			std::string type = ((VariableBase *) *it)->GetTypeName();
			std::string format = "";
			switch (((VariableBase *) *it)->GetFormat()) {
				case VariableBase::FMT_HEX: format = "HEX"; break;
				case VariableBase::FMT_DEC: format = "DEC"; break;
				default: format = "Default";
			}

			const char *description = ((VariableBase *) *it)->GetDescription();

			strstm << type << ":" << name << ":" << dataType << ":" << value << ":" << format << ":" << ((strlen(description) != 0)? std::string(description): "No description available!");

			string str = strstm.str();

			OutputText(str.c_str(), str.size());

			strstm.str(std::string());

		}

		lst.clear();

		PutPacket("T05");

	}
	else if (cmdPrefix.compare("parameter") == 0) {

		std::string str = command.substr(separator_index+1);

		separator_index = str.find_first_of(':');
		string name;
		if (separator_index == static_cast<unsigned int>(string::npos)) {
			name = str;
		} else {
			name = str.substr(0, separator_index);
		}

		VariableBase* parameter = Simulator::simulator->FindParameter(name.c_str());
		if (parameter) {
			if (separator_index == static_cast<unsigned int>(string::npos)) // read parameter request
			{
				std::ostringstream os;

				if (strcmp(parameter->GetDataTypeName(), "double precision floating-point") == 0) {
					double val = *parameter;
					os << stringify(val);
				}
				else if (strcmp(parameter->GetDataTypeName(), "single precision floating-point") == 0) {
					float val = *parameter;
					os << stringify(val);
				}
				else if (strcmp(parameter->GetDataTypeName(), "boolean") == 0) {
					bool val = *parameter;
					os << stringify(val);
				}
				else {
					uint64_t val = *parameter;
					os << stringify(val);
				}

				std::string value = os.str();
				OutputText(value.c_str(), value.size());
			}
			else // write parameter request
			{
				std::string value = str.substr(separator_index+1);

				*parameter = value.c_str();
			}

		} else {

		}

		PutPacket("T05");

	}
	else if (cmdPrefix.compare("symboles") == 0) {

		/**
		 * command: qRcmd, symboles
		 * return "symbName:symbAddress:symbSize:symbType"
		 *
		 * with: symbType in {"FUNCTION", "VARIABLE"}
		 */

		list<const Symbol<ADDRESS> *> symbol_registries;

		if (symbol_table_lookup_import) {
			symbol_table_lookup_import->GetSymbols(symbol_registries, Symbol<ADDRESS>::SYM_FUNC);
		}

		typename list<const Symbol<ADDRESS> *>::const_iterator symbol_iter;
		std::stringstream strstm;

		for(symbol_iter = symbol_registries.begin(); symbol_iter != symbol_registries.end(); symbol_iter++)
		{
			strstm << (*symbol_iter)->GetName();

			strstm << ":" << std::hex;
			strstm.width(8);
			strstm << (*symbol_iter)->GetAddress();

			strstm << ":" << std::dec << (*symbol_iter)->GetSize();

			strstm << ":" << "FUNCTION";

			string str = strstm.str();
			OutputText(str.c_str(), str.size());

			strstm.str(std::string());
		}

		symbol_registries.clear();
		symbol_table_lookup_import->GetSymbols(symbol_registries, Symbol<ADDRESS>::SYM_OBJECT);

		for(symbol_iter = symbol_registries.begin(); symbol_iter != symbol_registries.end(); symbol_iter++)
		{

			strstm << (*symbol_iter)->GetName();

			strstm << ":" << std::hex;
			strstm.width(8);
			strstm << (*symbol_iter)->GetAddress();

			strstm << ":" << std::dec << (*symbol_iter)->GetSize();

			strstm << ":" << "VARIABLE";

			string str = strstm.str();
			OutputText(str.c_str(), str.size());

			strstm.str(std::string());

		}

		PutPacket("T05");

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
			ADDRESS symbol_size;
			if(!ParseHex(command, separator_index, symbol_address)) {
				PutPacket("E00");
			} else if(command[separator_index++] != ':') {
				PutPacket("E00");
			} else if(!ParseHex(command, separator_index, symbol_size)) {
				PutPacket("E00");
			} else if(separator_index != command.length()) {
				PutPacket("E00");
			}

			if (disasm_import) {
				Disasm(symbol_address, symbol_size);
			}

			PutPacket("T05");

		}

	}
	else if (cmdPrefix.compare("start-pim") == 0) {
		// Start PIMThread
		target = new PIMThread("pim-thread");

		// Open Socket Stream
		// restart the SocketServer to get connection with Pim-Client

		socketServer->setProtocolHandler(target);

		socketServer->start();

		OutputText("OK", 2);

	}
	else if (cmdPrefix.compare("time") == 0) {
		string packet("");

		std::stringstream sstr;
		sstr << GetSimTime();
		packet += sstr.str();

		PutPacket(packet);

	}
	else if (cmdPrefix.compare("endian") == 0) {
		string packet("");

		std::stringstream sstr;
		sstr << GetEndian();
		packet += sstr.str();

		PutPacket(packet);

	}
	else if (cmdPrefix.compare("registers") == 0) {

		std::stringstream sstr;

		std::list<VariableBase *> lst;

		Simulator::simulator->GetRegisters(lst);

		for (std::list<VariableBase *>::iterator it = lst.begin(); it != lst.end(); it++) {

			if (!((VariableBase *) *it)->IsVisible()) continue;

			sstr << ((VariableBase *) *it)->GetName() << ":" << ((VariableBase *) *it)->GetBitSize() << ";";

		}

		lst.clear();

		string str = sstr.str();
		PutPacket(str);

	}
	else if (cmdPrefix.compare("stack") == 0) {

		std::stringstream sstr;

		if(pc_reg)
		{
			string hex;

			const Statement<ADDRESS> *stmt = 0;
			ADDRESS addr = *pc_reg;
			long mcuAddress = Object::GetSimulator()->GetStructuredAddress(addr);

			Number2HexString((uint8_t*) &mcuAddress, sizeof(mcuAddress), hex, (endian == GDB_BIG_ENDIAN)? "big":"little");

			sstr << hex << ":";

			unsigned int i;
			if(stmt_lookup_import)
			{
//				stmt = stmt_lookup_import->FindStatement(addr);
				stmt = stmt_lookup_import->FindStatement(mcuAddress);
			}

			if(stmt)
			{
				const char *source_filename = stmt->GetSourceFilename();
				if(source_filename)
				{
					const char *source_dirname = stmt->GetSourceDirname();
					if(source_dirname)
					{
						sstr << string(source_dirname);

					}
					sstr << ";" << string(source_filename);

					unsigned int lineno = stmt->GetLineNo();
					unsigned int colno = stmt->GetColNo();

					sstr << ";" << lineno << ";" << colno;

				}
			}
		}

		sstr << ";";

		string packet(sstr.str());

		OutputText(packet.c_str(), packet.length());

	}
	else if (cmdPrefix.compare("physicalAddress") == 0) {

		if (separator_index == string::npos) {
			PutPacket("E00");
		} else {
			separator_index++;
			long logical_address;

			int start_index = 0;
			int end_index = command.find(':', start_index);
			if(end_index == string::npos) {
				PutPacket("E00");
			} else {

				string cmd = command.substr(start_index, end_index-start_index);
				start_index = end_index+1;

				string hex = command.substr(start_index);
				start_index = end_index+1;

				HexString2Number(hex, &logical_address, hex.size()/2, (endian == GDB_BIG_ENDIAN)? "big":"little");

				string packet("T05");
				std::stringstream sstr;

				uint64_t physicalAddress = Object::GetSimulator()->GetPhysicalAddress(logical_address);

				Number2HexString((uint8_t*) &physicalAddress, sizeof(uint64_t), hex, (endian == GDB_BIG_ENDIAN)? "big":"little");

				packet += hex;

				PutPacket(packet);

			}

		}

	}
	else if (cmdPrefix.compare("structuredAddress") == 0) {

		if (separator_index == string::npos) {
			PutPacket("E00");
		} else {
			separator_index++;
			long logical_address;

			int start_index = 0;
			int end_index = command.find(':', start_index);
			if(end_index == string::npos) {
				PutPacket("E00");
			} else {

				string cmd = command.substr(start_index, end_index-start_index);
				start_index = end_index+1;

				string hex = command.substr(start_index);
				start_index = end_index+1;

				HexString2Number(hex, &logical_address, hex.size()/2, (endian == GDB_BIG_ENDIAN)? "big":"little");

				string packet("T05");
				std::stringstream sstr;

				uint64_t mcuAddress = Object::GetSimulator()->GetStructuredAddress(logical_address);

				Number2HexString((uint8_t*) &mcuAddress, sizeof(uint64_t), hex, (endian == GDB_BIG_ENDIAN)? "big":"little");

				packet += hex;

				PutPacket(packet);

			}

		}

	}
	else if (cmdPrefix.compare("srcaddr") == 0) {

		// qRcmd,saddr:filename;lineno;colno
		std::stringstream sstr;

		int start_index = separator_index + 1;
		int end_index = command.find(';');
		string source_filename = command.substr(start_index, end_index-start_index);

		start_index = end_index+1;

		end_index = command.find(';', start_index);
		string lineno = command.substr(start_index, end_index-start_index);
		start_index = end_index+1;

		start_index = end_index+1;

		end_index = command.find(';', start_index);
		string colno = command.substr(start_index, end_index-start_index);
		start_index = end_index+1;

		const Statement<ADDRESS> *stmt = 0;

		if(stmt_lookup_import)
		{
			stmt = stmt_lookup_import->FindStatement(source_filename.c_str(), convertTo<unsigned int>(lineno), convertTo<unsigned int>(colno));
		}

		if(stmt)
		{
			ADDRESS addr = stmt->GetAddress();
			sstr << std::hex << addr;

			string packet(sstr.str());

			PutPacket(packet.c_str());

		} else {
			PutPacket("E00");
		}

	}
	else if (cmdPrefix.compare("statistics") == 0) {

		list<VariableBase *> lst;
		list<VariableBase *>::iterator iter;

		Object::GetSimulator()->GetStatistics(lst);
		std::stringstream sstr;
		sstr << "simulated time" << ":" << GetSimTime()*1000 << " ms";

		for (iter = lst.begin(); iter != lst.end(); iter++) {
			sstr << ";" << (*iter)->GetName() << ":" << (string) *(*iter);
		}

		string str = sstr.str();

		OutputText(str.c_str(), str.size());

	} else {
		PutPacket("E00");
	}

}

template <class ADDRESS>
void PIMServer<ADDRESS>::Disasm(ADDRESS symbol_address, unsigned int symbol_size)
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

		strstm << hex << current_address << ":";

		strstm << hex;
		strstm.width(8);
		strstm << (current_address / memory_atom_size) << ":" << dec << dis << endl;
		strstm.fill(' ');

		string str = strstm.str();
		OutputText(str.c_str(), str.size());

		disassembled_size += next_address - current_address;
		current_address = next_address;

	}

}

// *** End ************ REDA ADDED CODE ****************


} // end of namespace pim
} // end of namespace service
} // end of namespace unisim

#endif
