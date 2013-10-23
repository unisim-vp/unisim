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
#include <unisim/service/interfaces/backtrace.hh>
#include <unisim/service/interfaces/debug_event.hh>
#include <unisim/service/interfaces/profiling.hh>
#include <unisim/service/interfaces/debug_info_loading.hh>
#include <unisim/service/interfaces/data_object_lookup.hh>

#include <unisim/util/debug/profile.hh>
#include <unisim/util/debug/breakpoint.hh>
#include <unisim/util/debug/watchpoint.hh>
#include <unisim/util/loader/elf_loader/elf32_loader.hh>
#include <unisim/util/loader/elf_loader/elf64_loader.hh>

#include <unisim/kernel/service/service.hh>
#include <unisim/kernel/logger/logger.hh>

#include <inttypes.h>
#include <string>
#include <vector>
#include <queue>
#include <list>
#include <stack>

#ifdef WIN32
#include <windows.h>
#endif

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
using unisim::service::interfaces::BackTrace;
using unisim::service::interfaces::DebugEventTrigger;
using unisim::service::interfaces::DebugEventListener;
using unisim::service::interfaces::Profiling;
using unisim::service::interfaces::DebugInfoLoading;
using unisim::service::interfaces::DataObjectLookup;

using unisim::util::debug::Event;
using unisim::util::debug::Breakpoint;
using unisim::util::debug::Watchpoint;
using unisim::util::debug::Symbol;
using unisim::util::debug::Statement;
using unisim::util::debug::DataObject;

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
	INLINE_DEBUGGER_MODE_STEP_INSTRUCTION,
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
#ifndef WIN32
	static void (*prev_sig_int_handler)(int);
#endif
	static int alive_instances;
#ifdef WIN32
	static BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType);
#else
	static void SigIntHandler(int signum);
#endif
};

template <class ADDRESS>
class InlineDebugger
	: public Service<DebugControl<ADDRESS> >
	, public Service<DebugEventListener<ADDRESS> >
	, public Service<TrapReporting>
	, public Client<DebugEventTrigger<ADDRESS> >
	, public Client<Disassembly<ADDRESS> >
	, public Client<Memory<ADDRESS> >
	, public Client<Registers>
	, public Client<SymbolTableLookup<ADDRESS> >
	, public Client<StatementLookup<ADDRESS> >
	, public Client<BackTrace<ADDRESS> >
	, public Client<Profiling<ADDRESS> >
	, public Client<DebugInfoLoading>
	, public Client<DataObjectLookup<ADDRESS> >
	, public InlineDebuggerBase
{
public:
	ServiceExport<DebugControl<ADDRESS> > debug_control_export;
	ServiceExport<DebugEventListener<ADDRESS> > debug_event_listener_export;
	ServiceExport<TrapReporting> trap_reporting_export;
	ServiceImport<DebugEventTrigger<ADDRESS> > debug_event_trigger_import;
	ServiceImport<Disassembly<ADDRESS> > disasm_import;
	ServiceImport<Memory<ADDRESS> > memory_import;
	ServiceImport<Registers> registers_import;
	ServiceImport<SymbolTableLookup<ADDRESS> > symbol_table_lookup_import;
	ServiceImport<StatementLookup<ADDRESS> > stmt_lookup_import;
	ServiceImport<BackTrace<ADDRESS> > backtrace_import;
	ServiceImport<Profiling<ADDRESS> > profiling_import;
	ServiceImport<DebugInfoLoading> debug_info_loading_import;
	ServiceImport<DataObjectLookup<ADDRESS> > data_object_lookup_import;
	
	InlineDebugger(const char *name, Object *parent = 0);
	virtual ~InlineDebugger();

	// TrapReporting
	virtual void ReportTrap();
	virtual void ReportTrap(const unisim::kernel::service::Object &obj);
	virtual void ReportTrap(const unisim::kernel::service::Object &obj,
							const std::string &str);
	virtual void ReportTrap(const unisim::kernel::service::Object &obj,
							const char *c_str);
	
	// DebugControlInterface
	virtual typename DebugControl<ADDRESS>::DebugCommand FetchDebugCommand(ADDRESS cia);
	
	// DebugEventListener
	virtual void OnDebugEvent(const unisim::util::debug::Event<ADDRESS>& event);

	virtual bool EndSetup();
	virtual void OnDisconnect();
private:
	unisim::kernel::logger::Logger logger;
	unsigned int memory_atom_size;
	std::string search_path;
	std::string init_macro;
	std::string output;
	Parameter<unsigned int> param_memory_atom_size;
	Parameter<std::string> param_search_path;
	Parameter<std::string> param_init_macro;
	Parameter<std::string> param_output;

	InlineDebuggerRunningMode running_mode;

	ADDRESS disasm_addr;
	ADDRESS dump_addr;
	ADDRESS cont_until_addr;
	const Statement<ADDRESS> *last_stmt;

	std::list<std::string> exec_queue;
	string prompt;
	char *hex_addr_fmt;
	char *int_addr_fmt;
	std::string last_line;
	std::string line;
	std::vector<std::string> parm;
	std::ostream *output_stream;
	std::ostream *std_output_stream;
	std::ostream *std_error_stream;

	void Tokenize(const std::string& str, std::vector<std::string>& tokens);
	bool ParseAddr(const char *s, ADDRESS& addr);
	bool ParseAddrRange(const char *s, ADDRESS& addr, unsigned int& size);
	bool ParseValue(const char *s, uint64_t& value);
	bool GetLine(const char *prompt, std::string& line, bool& interactive);
	bool IsBlankLine(const std::string& line) const;
	bool IsQuitCommand(const char *cmd) const;
	bool IsStepInstructionCommand(const char *cmd) const;
	bool IsStepCommand(const char *cmd) const;
	bool IsNextInstructionCommand(const char *cmd) const;
	bool IsContinueCommand(const char *cmd) const;
	bool IsDisasmCommand(const char *cmd) const;
	bool IsBreakCommand(const char *cmd) const;
	bool IsFinishCommand(const char *cmd) const;
	bool IsWatchCommand(const char *cmd) const;
	bool IsDeleteCommand(const char *cmd) const;
	bool IsDeleteWatchCommand(const char *cmd) const;
	bool IsDumpCommand(const char *cmd) const;
	bool IsEditCommand(const char *cmd) const;
	bool IsHelpCommand(const char *cmd) const;
	bool IsResetCommand(const char *cmd) const;
	bool IsMonitorCommand(const char *cmd, const char *format = 0) const;
	bool IsRegisterCommand(const char *cmd, const char *format = 0) const;
	bool IsStatisticCommand(const char *cmd, const char *format = 0) const;
	bool IsParameterCommand(const char *cmd, const char *format = 0) const;
	bool IsSymbolCommand(const char *cmd) const;
	bool IsMonitorSetCommand(const char *cmd) const;
	bool IsProfileCommand(const char *cmd) const;
	bool IsLoadCommand(const char *cmd) const;
	bool IsBackTraceCommand(const char *cmd) const;
	bool IsLoadSymbolTableCommand(const char *cmd) const;
	bool IsListSymbolsCommand(const char *cmd) const;
	bool IsMacroCommand(const char *cmd) const;
	bool IsLoadConfigCommand(const char *cmd) const;
	bool IsListSourcesCommand(const char *cmd) const;
	bool IsEnableBinaryCommand(const char *cmd) const;
	bool IsDisableBinaryCommand(const char *cmd) const;
	bool IsListBinariesCommand(const char *cmd) const;
	bool IsDumpDataObjectCommand(const char *cmd) const;
	bool IsPrintDataObjectCommand(const char *cmd) const;
	bool IsEditDataObjectCommand(const char *cmd) const;
	bool IsSetDataObjectCommand(const char *cmd) const;
	bool IsListDataObjectsCommand(const char *cmd) const;

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
	bool EditBuffer(ADDRESS addr, std::vector<uint8_t>& buffer);
	bool EditMemory(ADDRESS addr);
	void DumpSymbols(const typename std::list<const unisim::util::debug::Symbol<ADDRESS> *>& symbols, const char *name = 0);
	void DumpSymbols(const char *name = 0);
	void MonitorGetFormat(const char *cmd, char &format);
	void DumpVariables(const char *cmd, const char *name = 0, typename unisim::kernel::service::VariableBase::Type type = unisim::kernel::service::VariableBase::VAR_VOID);
	void DumpVariable(const char *cmd, const unisim::kernel::service::VariableBase *variable);
	void SetVariable(const char *name, const char *value);
	void DumpProgramProfile();
	void DumpDataProfile(bool write);
	std::string SearchFile(const char *filename);
	void LoadSymbolTable(const char *filename);
	void LoadMacro(const char *filename);
	bool LocateFile(const char *file_path, std::string& match_file_path);
	void DumpSource(const char *filename, unsigned int lineno, unsigned int colno, unsigned int count);
	void DumpBackTrace(ADDRESS cia);
	bool GetReturnAddress(ADDRESS cia, ADDRESS& ret_addr) const;
	const Statement<ADDRESS> *FindStatement(ADDRESS addr, typename unisim::service::interfaces::StatementLookup<ADDRESS>::FindStatementOption opt = unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_EXACT_STMT) const;
	void EnableProgramProfiling();
	void EnableDataReadProfiling();
	void EnableDataWriteProfiling();
	void EnableDataProfiling();
	void EnableProfiling();
	void DisableProgramProfiling();
	void DisableDataReadProfiling();
	void DisableDataWriteProfiling();
	void DisableDataProfiling();
	void DisableProfiling();
	void ClearProgramProfile();
	void ClearDataReadProfile();
	void ClearDataWriteProfile();
	void ClearDataProfile();
	void ClearProfile();
	void DumpProgramProfilingStatus();
	void DumpDataReadProfilingStatus();
	void DumpDataWriteProfilingStatus();
	void DumpDataProfilingStatus();
	void DumpProfilingStatus();
	void ListSourceFiles();
	void EnableBinary(const char *filename, bool enable);
	void ListBinaryFiles();
	void DumpDataObject(const char *data_object_name, ADDRESS cia);
	void PrintDataObject(const char *data_object_name, ADDRESS cia);
	bool EditDataObject(const char *data_object_name, ADDRESS cia);
	bool SetDataObject(const char *data_object_name, ADDRESS cia, uint64_t value);
	void ListDataObjects(ADDRESS cia, typename unisim::service::interfaces::DataObjectLookup<ADDRESS>::Scope scope = unisim::service::interfaces::DataObjectLookup<ADDRESS>::SCOPE_BOTH_GLOBAL_AND_LOCAL);
};

} // end of namespace inline_debugger
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif
