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
 *          Reda   Nouacer  (reda.nouacer@cea.fr)
 */

#ifndef __UNISIM_SERVICE_PIM_PIM_SERVER_HH__
#define __UNISIM_SERVICE_PIM_PIM_SERVER_HH__

#include <unisim/util/endian/endian.hh>
#include <unisim/service/interfaces/memory_access_reporting.hh>
#include <unisim/service/interfaces/debug_control.hh>
#include <unisim/service/interfaces/disassembly.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/trap_reporting.hh>

#include <unisim/service/pim/network/GenericThread.hpp>
#include <unisim/service/pim/network/SocketThread.hpp>
#include <unisim/service/pim/network/SocketServerThread.hpp>
#include <unisim/service/pim/pim_thread.hh>

#include <unisim/kernel/service/service.hh>
#include <unisim/kernel/logger/logger.hh>

#include <unisim/util/debug/breakpoint_registry.hh>
#include <unisim/util/debug/watchpoint_registry.hh>
#include <unisim/util/debug/register.hh>

#include <string>
#include <vector>

#include <inttypes.h>

#include <unisim/service/pim/convert.hh>

namespace unisim {
namespace service {
namespace pim {

using std::string;
using std::vector;

using unisim::service::interfaces::DebugControl;
using unisim::service::interfaces::Disassembly;
using unisim::service::interfaces::MemoryAccessReporting;
using unisim::service::interfaces::MemoryAccessReportingControl;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::SymbolTableLookup;
using unisim::service::interfaces::TrapReporting;

using unisim::util::debug::BreakpointRegistry;
using unisim::util::debug::WatchpointRegistry;
using unisim::util::debug::Symbol;

using unisim::kernel::service::Parameter;
using unisim::kernel::service::Service;
using unisim::kernel::service::Client;
using unisim::kernel::service::Object;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceImport;

using unisim::service::pim::network::SocketThread;
using unisim::service::pim::network::GenericThread;
using unisim::service::pim::network::SocketServerThread;

typedef enum { GDBSERVER_MODE_WAITING_GDB_CLIENT, GDBSERVER_MODE_STEP, GDBSERVER_MODE_CONTINUE } GDBServerRunningMode;

typedef enum { GDB_LITTLE_ENDIAN, GDB_BIG_ENDIAN } GDBEndian;

class GDBRegister
{
public:
	GDBRegister(const string& reg_name, int reg_size, GDBEndian endian, unsigned int reg_num);
	GDBRegister(unisim::util::debug::Register *reg, GDBEndian endian, unsigned int reg_num);
	inline const char *GetName() const { return name.c_str(); }
	inline int GetSize() const { return size; }
	bool SetValue(const string& hex);
	void SetValue(const void *buffer);
	void GetValue(string& hex) const;
	void GetValue(void *buffer) const;
	inline int GetHexLength() const { return 2 * size; }
	inline unisim::util::debug::Register *GetRegisterInterface() { return reg; }
	inline void SetRegisterInterface(unisim::util::debug::Register *reg) { this->reg = reg; }
	inline GDBEndian GetEndian() const { return endian; }
	unsigned int GetRegNum() const { return reg_num; }
private:
	string name;
	int size;
	unisim::util::debug::Register *reg;
	GDBEndian endian;
	unsigned int reg_num;
};

template <class ADDRESS>
class PIMGDBServer :
	public Service<DebugControl<ADDRESS> >,
	public Service<MemoryAccessReporting<ADDRESS> >,
	public Service<TrapReporting>,
	public Client<MemoryAccessReportingControl>,
	public Client<Memory<ADDRESS> >,
	public Client<Disassembly<ADDRESS> >,
	public Client<SymbolTableLookup<ADDRESS> >,
	public Client<Registers>,
	public SocketThread
{
public:
	ServiceExport<DebugControl<ADDRESS> > debug_control_export;
	ServiceExport<MemoryAccessReporting<ADDRESS> > memory_access_reporting_export;
	ServiceExport<TrapReporting> trap_reporting_export;

	ServiceImport<MemoryAccessReportingControl> memory_access_reporting_control_import;
	ServiceImport<Memory<ADDRESS> > memory_import;
	ServiceImport<Registers> registers_import;
	ServiceImport<Disassembly<ADDRESS> > disasm_import;
	ServiceImport<SymbolTableLookup<ADDRESS> > symbol_table_lookup_import;

	PIMGDBServer(const char *name, Object *parent = 0);
	virtual ~PIMGDBServer();

	virtual void ReportMemoryAccess(typename MemoryAccessReporting<ADDRESS>::MemoryAccessType mat, typename MemoryAccessReporting<ADDRESS>::MemoryType mt, ADDRESS addr, uint32_t size);
	virtual void ReportFinishedInstruction(ADDRESS next_addr);
	virtual typename DebugControl<ADDRESS>::DebugCommand FetchDebugCommand(ADDRESS cia);
	virtual void ReportTrap();
	virtual void ReportTrap(const unisim::kernel::service::Object &obj);
	virtual void ReportTrap(const unisim::kernel::service::Object &obj,
							const std::string &str);
	virtual void ReportTrap(const unisim::kernel::service::Object &obj,
							const char *c_str);
	virtual bool Setup();
	virtual void OnDisconnect();

	virtual void Run() { }
	virtual string getProtocol() { return "GDB"; }

	uint16_t GetTCPPort() { return tcp_port;}
	string GetHost() { return fHost; }

//protected:
//	int sock;

private:
	static const unsigned int MAX_BUFFER_SIZE = 256;
	bool ParseHex(const string& s, unsigned int& pos, ADDRESS& value);
	bool GetChar(char& c, bool blocking);
	bool PutChar(char c);
	bool GetPacket(string& s, bool blocking);
	bool PutPacket(const string& s);
	bool FlushOutput();
	bool OutputText(const char *s, int count);
	bool ReadRegisters();
	bool WriteRegisters(const string& hex);
	bool ReadRegister(unsigned int regnum);
	bool WriteRegister(unsigned int regnum, const string& hex);
	bool ReadMemory(ADDRESS addr, uint32_t size);
	bool WriteMemory(ADDRESS addr, const string& hex, uint32_t size);
	bool ReportProgramExit();
	bool ReportSignal(unsigned int signum);
	bool ReportTracePointTrap();
	bool SetBreakpointWatchpoint(uint32_t type, ADDRESS addr, uint32_t size);
	bool RemoveBreakpointWatchpoint(uint32_t type, ADDRESS addr, uint32_t size);

	void HandleQRcmd(string command);

	void Disasm(ADDRESS addr, unsigned int size);
	
	void Kill();

	unisim::kernel::logger::Logger logger;

	uint16_t tcp_port;
	string architecture_description_filename;

//	int sock;
	vector<GDBRegister> gdb_registers;
	GDBRegister *gdb_pc;
	bool killed;
	bool trap;
	bool synched;
	BreakpointRegistry<ADDRESS> breakpoint_registry;
	WatchpointRegistry<ADDRESS> watchpoint_registry;
	GDBServerRunningMode running_mode;
	bool extended_mode;
	int32_t counter;
	int32_t period;

	ADDRESS disasm_addr;

	unsigned int input_buffer_size;
	unsigned int input_buffer_index;
	char input_buffer[MAX_BUFFER_SIZE];

	unsigned int output_buffer_size;
	char output_buffer[MAX_BUFFER_SIZE];

	unsigned int memory_atom_size;
	bool verbose;

	string fHost;

	Parameter<unsigned int> param_memory_atom_size;
	Parameter<uint16_t> param_tcp_port;
	Parameter<string> param_architecture_description_filename;
	Parameter<bool> param_verbose;
	Parameter<string> param_host;

};

} // end of namespace pim
} // end of namespace service
} // end of namespace unisim

#endif