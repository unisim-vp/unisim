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
 
#ifndef __UNISIM_SERVICE_DEBUG_GDB_SERVER_GDB_SERVER_HH__
#define __UNISIM_SERVICE_DEBUG_GDB_SERVER_GDB_SERVER_HH__

#include <unisim/service/interfaces/memory_access_reporting.hh>
#include <unisim/service/interfaces/debug_control.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/logger.hh>
#include <unisim/service/interfaces/memory.hh>

#include <unisim/kernel/service/service.hh>

#include <unisim/util/debug/breakpoint_registry.hh>
#include <unisim/util/debug/watchpoint_registry.hh>

#include <string>
#include <vector>

#include <inttypes.h>

namespace unisim {
namespace service {
namespace debug {
namespace gdb_server {

using std::string;
using std::vector;

using unisim::service::interfaces::DebugControl;
using unisim::service::interfaces::MemoryAccessReporting;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::Logger;

using unisim::util::debug::BreakpointRegistry;
using unisim::util::debug::WatchpointRegistry;

using unisim::kernel::service::Parameter;
using unisim::kernel::service::Service;
using unisim::kernel::service::Client;
using unisim::kernel::service::Object;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceImport;


typedef enum { GDBSERVER_MODE_WAITING_GDB_CLIENT, GDBSERVER_MODE_STEP, GDBSERVER_MODE_CONTINUE } GDBServerRunningMode; 

typedef enum { GDB_LITTLE_ENDIAN, GDB_BIG_ENDIAN } GDBEndian;

class GDBRegister
{
public:
	GDBRegister(const string& reg_name, int reg_size, GDBEndian endian);
	GDBRegister(Registers::Register *reg, GDBEndian endian);
	inline const char *GetName() const { return name.c_str(); }
	inline int GetSize() const { return size; }
	bool SetValue(const string& hex);
	void SetValue(const void *buffer);
	void GetValue(string& hex) const;
	void GetValue(void *buffer) const;
	inline int GetHexLength() const { return 2 * size; }
	inline Registers::Register *GetRegisterInterface() { return reg; }
	inline void SetRegisterInterface(Registers::Register *reg) { this->reg = reg; }
	inline GDBEndian GetEndian() const { return endian; }
private:
	string name;
	int size;
	Registers::Register *reg;
	GDBEndian endian;
};

template <class ADDRESS>
class GDBServer :
	public Service<DebugControl<ADDRESS> >,
	public Service<MemoryAccessReporting<ADDRESS> >,
	public Client<Memory<ADDRESS> >,
	public Client<Registers>,
	public Client<Logger>
{
public:
	ServiceExport<DebugControl<ADDRESS> > debug_control_export;
	ServiceExport<MemoryAccessReporting<ADDRESS> > memory_access_reporting_export;

	ServiceImport<Memory<ADDRESS> > memory_import;
	ServiceImport<Registers> registers_import;
	ServiceImport<Logger> logger_import;

	
	GDBServer(const char *name, Object *parent = 0);
	virtual ~GDBServer();
	
	virtual void ReportMemoryAccess(typename MemoryAccessReporting<ADDRESS>::MemoryAccessType mat, typename MemoryAccessReporting<ADDRESS>::MemoryType mt, ADDRESS addr, uint32_t size);
	virtual void ReportFinishedInstruction(ADDRESS next_addr);
	virtual typename DebugControl<ADDRESS>::DebugCommand FetchDebugCommand(ADDRESS cia);
	virtual void Trap();
	virtual bool Setup();
	virtual void OnDisconnect();

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
	
	int tcp_port;
	string architecture_description_filename;
	
	int sock;
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

	unsigned int input_buffer_size;
	unsigned int input_buffer_index;
	char input_buffer[MAX_BUFFER_SIZE];

	unsigned int output_buffer_size;
	char output_buffer[MAX_BUFFER_SIZE];

	Parameter<int> param_tcp_port;
	Parameter<string> param_architecture_description_filename;
};

} // end of namespace gdb_server
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif
