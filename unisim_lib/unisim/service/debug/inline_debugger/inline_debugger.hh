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
 
#ifndef __UNISIM_SERVICE_DEBUG_INLINE_DEBUGGER_INLINE_DEBUGGER_HH__
#define __UNISIM_SERVICE_DEBUG_INLINE_DEBUGGER_INLINE_DEBUGGER_HH__

#include <unisim/service/interfaces/memory_access_reporting.hh>
#include <unisim/service/interfaces/debug_control.hh>
#include <unisim/service/interfaces/disassembly.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/trap_reporting.hh>
#include <unisim/service/interfaces/loader.hh>
#include <unisim/service/interfaces/stmt_lookup.hh>

#include <unisim/util/debug/breakpoint_registry.hh>
#include <unisim/util/debug/watchpoint_registry.hh>
#include <unisim/util/debug/profile.hh>

#include <unisim/kernel/service/service.hh>

#include <inttypes.h>
#include <string>

namespace unisim {
namespace service {
namespace debug {
namespace inline_debugger {

using unisim::service::interfaces::DebugControl;
using unisim::service::interfaces::Disassembly;
using unisim::service::interfaces::MemoryAccessReporting;
using unisim::service::interfaces::MemoryAccessReportingControl;
using unisim::service::interfaces::SymbolTableLookup;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::TrapReporting;
using unisim::service::interfaces::Loader;
using unisim::service::interfaces::StatementLookup;

using unisim::util::debug::BreakpointRegistry;
using unisim::util::debug::Breakpoint;
using unisim::util::debug::WatchpointRegistry;
using unisim::util::debug::Watchpoint;
using unisim::util::debug::Symbol;

using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::service::Parameter;

using std::string;

typedef enum
{
	INLINE_DEBUGGER_MODE_WAITING_USER,
	INLINE_DEBUGGER_MODE_STEP,
	INLINE_DEBUGGER_MODE_CONTINUE,
	INLINE_DEBUGGER_MODE_CONTINUE_UNTIL,
	INLINE_DEBUGGER_MODE_RESET
} InlineDebuggerRunningMode;

class InlineDebuggerBase
{
public:
	InlineDebuggerBase();
	virtual ~InlineDebuggerBase();
protected:
	static bool trap;
private:
	static void (*prev_sig_int_handler)(int);
	static int alive_instances;
	static void SigIntHandler(int signum);
};

template <class ADDRESS>
class InlineDebugger :
	public Service<DebugControl<ADDRESS> >,
	public Service<MemoryAccessReporting<ADDRESS> >,
	public Service<TrapReporting>,
	public Client<MemoryAccessReportingControl>,
	public Client<Disassembly<ADDRESS> >,
	public Client<Memory<ADDRESS> >,
	public Client<Registers>,
	public Client<SymbolTableLookup<ADDRESS> >,
	public Client<Loader>,
	public Client<StatementLookup<ADDRESS> >,
	public InlineDebuggerBase
{
public:
	ServiceExport<DebugControl<ADDRESS> > debug_control_export;
	ServiceExport<MemoryAccessReporting<ADDRESS> > memory_access_reporting_export;
	ServiceExport<TrapReporting> trap_reporting_export;
	ServiceImport<Disassembly<ADDRESS> > disasm_import;
	ServiceImport<Memory<ADDRESS> > memory_import;
	ServiceImport<MemoryAccessReportingControl> memory_access_reporting_control_import;
	ServiceImport<Registers> registers_import;
	ServiceImport<SymbolTableLookup<ADDRESS> > **symbol_table_lookup_import;
	ServiceImport<Loader> **loader_import;
	ServiceImport<StatementLookup<ADDRESS> > **stmt_lookup_import;
	
	InlineDebugger(const char *name, Object *parent = 0);
	virtual ~InlineDebugger();

	// MemoryAccessReportingInterface
	virtual void ReportMemoryAccess(typename MemoryAccessReporting<ADDRESS>::MemoryAccessType mat, typename MemoryAccessReporting<ADDRESS>::MemoryType mt, ADDRESS addr, uint32_t size);
	virtual void ReportFinishedInstruction(ADDRESS next_addr);
	virtual void ReportTrap();
	virtual void ReportTrap(const unisim::kernel::service::Object &obj);
	virtual void ReportTrap(const unisim::kernel::service::Object &obj,
							const std::string &str);
	virtual void ReportTrap(const unisim::kernel::service::Object &obj,
							const char *c_str);
	
	// DebugControlInterface
	virtual typename DebugControl<ADDRESS>::DebugCommand FetchDebugCommand(ADDRESS cia);

	virtual bool EndSetup();
	virtual void OnDisconnect();
private:
	unsigned int memory_atom_size;
	unsigned int num_loaders;
	std::string search_path;
	Parameter<unsigned int> param_memory_atom_size;
	Parameter<unsigned int> param_num_loaders;
	Parameter<std::string> param_search_path;

	BreakpointRegistry<ADDRESS> breakpoint_registry;
	WatchpointRegistry<ADDRESS> watchpoint_registry;
	unisim::util::debug::Profile<ADDRESS> program_profile;
	unisim::util::debug::Profile<ADDRESS> data_read_profile;
	unisim::util::debug::Profile<ADDRESS> data_write_profile;
	InlineDebuggerRunningMode running_mode;

	ADDRESS disasm_addr;
	ADDRESS dump_addr;

	string prompt;
	char *hex_addr_fmt;
	char *int_addr_fmt;
	char last_line[256];
	char line[256];

	bool ParseAddr(const char *s, ADDRESS& addr);
	bool ParseAddrRange(const char *s, ADDRESS& addr, unsigned int& size);
	bool GetLine(char *line, int size);
	bool IsBlankLine(const char *line);
	bool IsQuitCommand(const char *cmd);
	bool IsStepCommand(const char *cmd);
	bool IsNextCommand(const char *cmd);
	bool IsContinueCommand(const char *cmd);
	bool IsDisasmCommand(const char *cmd);
	bool IsBreakCommand(const char *cmd);
	bool IsWatchCommand(const char *cmd);
	bool IsDeleteCommand(const char *cmd);
	bool IsDeleteWatchCommand(const char *cmd);
	bool IsDumpCommand(const char *cmd);
	bool IsHelpCommand(const char *cmd);
	bool IsResetCommand(const char *cmd);
	bool IsMonitorCommand(const char *cmd, const char *format = 0);
	bool IsRegisterCommand(const char *cmd, const char *format = 0);
	bool IsStatisticCommand(const char *cmd, const char *format = 0);
	bool IsParameterCommand(const char *cmd, const char *format = 0);
	bool IsMonitorSetCommand(const char *cmd);
	bool IsProfileCommand(const char *cmd);
	bool IsLoadCommand(const char *cmd);

	void Help();
	void Disasm(ADDRESS addr, int count);
	bool HasBreakpoint(ADDRESS addr);
	void SetBreakpoint(ADDRESS addr);
	void SetReadWatchpoint(ADDRESS addr, uint32_t size);
	void SetWriteWatchpoint(ADDRESS addr, uint32_t size);
	void DeleteBreakpoint(ADDRESS addr);
	void DeleteReadWatchpoint(ADDRESS addr, uint32_t size);
	void DeleteWriteWatchpoint(ADDRESS addr, uint32_t size);
	void DumpBreakpoints();
	void DumpWatchpoints();
	void DumpMemory(ADDRESS addr);
	void DumpVariables();
	void DumpRegisters();
	void DumpStatistics();
	void DumpParameters();
	void MonitorGetFormat(const char *cmd, char &format);
	bool DumpVariable(const char *name);
	void DumpVariable(const char *cmd, const unisim::kernel::service::VariableBase *variable);
	void DumpVariable(const char *cmd, const char *name);
	void DumpRegister(const char *cmd, const char *name);
	void DumpStatistic(const char *cmd, const char *name);
	void DumpParameter(const char *cmd, const char *name);
	void SetVariable(const char *name, const char *value);
	void DumpProgramProfile();
	void DumpDataProfile(bool write);
	void DumpAvailableLoaders();
	void Load(const char *loader_name);
	void DumpSource(const char *filename, unsigned int lineno, unsigned int colno, unsigned int count);
};

} // end of namespace inline_debugger
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif
