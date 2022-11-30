/*
 *  Copyright (c) 2012,
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

#ifndef __UNISIM_SERVICE_DEBUG_DEBUGGER_DEBUGGER_HH__
#define __UNISIM_SERVICE_DEBUG_DEBUGGER_DEBUGGER_HH__

#include <unisim/kernel/kernel.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/util/debug/breakpoint_registry.hh>
#include <unisim/util/debug/watchpoint_registry.hh>
#include <unisim/util/debug/fetch_insn_event.hh>
#include <unisim/util/debug/commit_insn_event.hh>
#include <unisim/util/debug/trap_event.hh>
#include <unisim/util/debug/source_code_breakpoint.hh>
#include <unisim/util/debug/subprogram_breakpoint.hh>
#include <unisim/util/debug/stub.hh>
#include <unisim/util/debug/dwarf/machine_state.hh>
#include <unisim/util/loader/elf_loader/elf32_loader.hh>
#include <unisim/util/loader/elf_loader/elf64_loader.hh>
#include <unisim/util/loader/coff_loader/coff_loader.hh>
#include <unisim/util/likely/likely.hh>
#include <unisim/service/interfaces/memory_access_reporting.hh>
#include <unisim/service/interfaces/debug_yielding.hh>
#include <unisim/service/interfaces/debug_selecting.hh>
#include <unisim/service/interfaces/disassembly.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/trap_reporting.hh>
#include <unisim/service/interfaces/loader.hh>
#include <unisim/service/interfaces/stmt_lookup.hh>
#include <unisim/service/interfaces/backtrace.hh>
#include <unisim/service/interfaces/blob.hh>
#include <unisim/service/interfaces/debug_event.hh>
#include <unisim/service/interfaces/profiling.hh>
#include <unisim/service/interfaces/debug_info_loading.hh>
#include <unisim/service/interfaces/data_object_lookup.hh>
#include <unisim/service/interfaces/subprogram_lookup.hh>
#include <unisim/service/interfaces/stack_unwinding.hh>
#include <unisim/service/interfaces/stubbing.hh>
#include <unisim/service/interfaces/hooking.hh>
#include <unisim/service/interfaces/debug_timing.hh>

#include <string>
#include <set>
#include <map>

#include <pthread.h>

namespace unisim {
namespace service {
namespace debug {
namespace debugger {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

#if 0
struct CONFIG
{
	typedef uint32_t ADDRESS;
	typedef sc_core::sc_time TIME_TYPE;
	static const unsigned int NUM_PROCESSORS = 1;
	static const unsigned int MAX_FRONT_ENDS = 1;
};
#endif

template <typename CONFIG>
class Debugger
	: public unisim::kernel::Client<unisim::service::interfaces::Blob<typename CONFIG::ADDRESS> >
	, public unisim::kernel::VariableBaseListener
{
public:
	typedef typename CONFIG::ADDRESS ADDRESS;
	typedef typename CONFIG::TIME_TYPE TIME_TYPE;
	static const unsigned int NUM_PROCESSORS = CONFIG::NUM_PROCESSORS;
	static const unsigned int MAX_FRONT_ENDS = CONFIG::MAX_FRONT_ENDS;
	
	// Exports to CPUs
	unisim::kernel::ServiceExport<unisim::service::interfaces::DebugYielding>                   *debug_yielding_export[NUM_PROCESSORS];
	unisim::kernel::ServiceExport<unisim::service::interfaces::MemoryAccessReporting<ADDRESS> > *memory_access_reporting_export[NUM_PROCESSORS];
	unisim::kernel::ServiceExport<unisim::service::interfaces::TrapReporting>                   *trap_reporting_export[NUM_PROCESSORS];

	// Exports to Front-ends
	unisim::kernel::ServiceExport<unisim::service::interfaces::DebugYieldingRequest>        *debug_yielding_request_export[MAX_FRONT_ENDS];
	unisim::kernel::ServiceExport<unisim::service::interfaces::DebugSelecting>              *debug_selecting_export[MAX_FRONT_ENDS];
	unisim::kernel::ServiceExport<unisim::service::interfaces::DebugEventTrigger<ADDRESS> > *debug_event_trigger_export[MAX_FRONT_ENDS]; // depends on selected CPU number
	unisim::kernel::ServiceExport<unisim::service::interfaces::Disassembly<ADDRESS> >       *disasm_export[MAX_FRONT_ENDS];              // depends on selected CPU number
	unisim::kernel::ServiceExport<unisim::service::interfaces::Memory<ADDRESS> >            *memory_export[MAX_FRONT_ENDS];              // depends on selected CPU number
	unisim::kernel::ServiceExport<unisim::service::interfaces::Registers>                   *registers_export[MAX_FRONT_ENDS];           // depends on selected CPU number
	unisim::kernel::ServiceExport<unisim::service::interfaces::SymbolTableLookup<ADDRESS> > *symbol_table_lookup_export[MAX_FRONT_ENDS];
	unisim::kernel::ServiceExport<unisim::service::interfaces::StatementLookup<ADDRESS> >   *stmt_lookup_export[MAX_FRONT_ENDS];
	unisim::kernel::ServiceExport<unisim::service::interfaces::BackTrace<ADDRESS> >         *backtrace_export[MAX_FRONT_ENDS];           // depends on selected CPU number 
	unisim::kernel::ServiceExport<unisim::service::interfaces::DebugInfoLoading>            *debug_info_loading_export[MAX_FRONT_ENDS];
	unisim::kernel::ServiceExport<unisim::service::interfaces::DataObjectLookup<ADDRESS> >  *data_object_lookup_export[MAX_FRONT_ENDS];  // depends on selected CPU number
	unisim::kernel::ServiceExport<unisim::service::interfaces::SubProgramLookup<ADDRESS> >  *subprogram_lookup_export[MAX_FRONT_ENDS];   // depends on selected CPU number
	unisim::kernel::ServiceExport<unisim::service::interfaces::StackUnwinding>              *stack_unwinding_export[MAX_FRONT_ENDS];     // depends on selected CPU number
	unisim::kernel::ServiceExport<unisim::service::interfaces::Stubbing<ADDRESS> >          *stubbing_export[MAX_FRONT_ENDS];
	unisim::kernel::ServiceExport<unisim::service::interfaces::Hooking<ADDRESS> >           *hooking_export[MAX_FRONT_ENDS];
	unisim::kernel::ServiceExport<unisim::service::interfaces::DebugTiming<TIME_TYPE> >     *debug_timing_export[MAX_FRONT_ENDS];        // depends on selected CPU number

	// Import from Loader
	unisim::kernel::ServiceImport<unisim::service::interfaces::Blob<ADDRESS> > blob_import;

	// Imports from CPUs
	unisim::kernel::ServiceImport<unisim::service::interfaces::MemoryAccessReportingControl> *memory_access_reporting_control_import[NUM_PROCESSORS];
	unisim::kernel::ServiceImport<unisim::service::interfaces::Disassembly<ADDRESS> >        *disasm_import[NUM_PROCESSORS];
	unisim::kernel::ServiceImport<unisim::service::interfaces::Memory<ADDRESS> >             *memory_import[NUM_PROCESSORS];
	unisim::kernel::ServiceImport<unisim::service::interfaces::Registers>                    *registers_import[NUM_PROCESSORS];
	unisim::kernel::ServiceImport<unisim::service::interfaces::DebugTiming<TIME_TYPE> >      *debug_timing_import[NUM_PROCESSORS];

	// Imports from Front-ends
	unisim::kernel::ServiceImport<unisim::service::interfaces::DebugEventListener<ADDRESS> > *debug_event_listener_import[MAX_FRONT_ENDS];
	unisim::kernel::ServiceImport<unisim::service::interfaces::DebugYielding>                *debug_yielding_import[MAX_FRONT_ENDS];

	Debugger(const char *name, unisim::kernel::Object *parent = 0);
	virtual ~Debugger();
	
	virtual bool BeginSetup();
	
	// Symbols
	void GetSymbols(typename std::list<const unisim::util::debug::Symbol<ADDRESS> *>& lst, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const { GetSymbols(MAX_FRONT_ENDS, lst, type); }
	const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbol(const char *name, ADDRESS addr, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const { return FindSymbol(MAX_FRONT_ENDS, name, addr, type); }
	const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbolByAddr(ADDRESS addr) const { return FindSymbolByAddr(MAX_FRONT_ENDS, addr); }
	const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbolByName(const char *name) const { return FindSymbolByName(MAX_FRONT_ENDS, name); }
	const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbolByName(const char *name, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const { return FindSymbolByName(MAX_FRONT_ENDS, name, type); }
	const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbolByAddr(ADDRESS addr, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const { return FindSymbolByAddr(MAX_FRONT_ENDS, addr, type); }

	// Statements
	void ScanStatements(unisim::service::interfaces::StatementScanner<ADDRESS>& scanner, const char *filename) const { ScanStatements(MAX_FRONT_ENDS, scanner, filename); }
	const unisim::util::debug::Statement<ADDRESS> *FindStatement(ADDRESS addr, const char *filename, typename unisim::service::interfaces::StatementLookup<ADDRESS>::FindStatementOption opt) const { return FindStatement(MAX_FRONT_ENDS, addr, filename, opt); }
	const unisim::util::debug::Statement<ADDRESS> *FindStatements(std::vector<const unisim::util::debug::Statement<ADDRESS> *> &stmts, ADDRESS addr, const char *filename, typename unisim::service::interfaces::StatementLookup<ADDRESS>::FindStatementOption opt) const { return FindStatements(MAX_FRONT_ENDS, stmts, addr, filename, opt); }
	const unisim::util::debug::Statement<ADDRESS> *FindStatement(const unisim::util::debug::SourceCodeLocation& source_code_location, const char *filename) const { return FindStatement(MAX_FRONT_ENDS, source_code_location, filename); }
	const unisim::util::debug::Statement<ADDRESS> *FindStatements(std::vector<const unisim::util::debug::Statement<ADDRESS> *> &stmts, const unisim::util::debug::SourceCodeLocation& source_code_location, const char *filename) const { return FindStatements(MAX_FRONT_ENDS, stmts, source_code_location, filename); }
	void ScanStatements(unisim::service::interfaces::StatementScanner<ADDRESS>& scanner, const std::string& filename) const { ScanStatements(MAX_FRONT_ENDS, scanner, filename.length() ? filename.c_str() : 0); }
	const unisim::util::debug::Statement<ADDRESS> *FindStatement(ADDRESS addr, const std::string& filename, typename unisim::service::interfaces::StatementLookup<ADDRESS>::FindStatementOption opt) const { return FindStatement(MAX_FRONT_ENDS, addr, filename.length() ? filename.c_str() : 0, opt); }
	const unisim::util::debug::Statement<ADDRESS> *FindStatements(std::vector<const unisim::util::debug::Statement<ADDRESS> *> &stmts, ADDRESS addr, const std::string& filename, typename unisim::service::interfaces::StatementLookup<ADDRESS>::FindStatementOption opt) const { return FindStatements(MAX_FRONT_ENDS, stmts, addr, filename.length() ? filename.c_str() : 0, opt); }
	const unisim::util::debug::Statement<ADDRESS> *FindStatement(const unisim::util::debug::SourceCodeLocation& source_code_location, const std::string& filename) const { return FindStatement(MAX_FRONT_ENDS, source_code_location, filename.length() ? filename.c_str() : 0); }
	const unisim::util::debug::Statement<ADDRESS> *FindStatements(std::vector<const unisim::util::debug::Statement<ADDRESS> *> &stmts, const unisim::util::debug::SourceCodeLocation& source_code_location, const std::string& filename) const { return FindStatements(MAX_FRONT_ENDS, stmts, source_code_location, filename.length() ? filename.c_str() : 0); }
	
private:
	// Exports to CPUs
	
	// unisim::service::interfaces::DebugYielding (tagged)
	void DebugYield(unsigned int prc_num);
	
	// unisim::service::interfaces::MemoryAccessReporting<ADDRESS> (tagged)
	bool ReportMemoryAccess(unsigned int prc_num, unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size);
	void ReportCommitInstruction(unsigned int prc_num, ADDRESS addr, unsigned int length);
	void ReportFetchInstruction(unsigned int prc_num, ADDRESS next_addr);
	
	// unisim::service::interfaces::TrapReporting (tagged)
	void ReportTrap(unsigned int prc_num);
	void ReportTrap(unsigned int prc_num, const unisim::kernel::Object &obj);
	void ReportTrap(unsigned int prc_num, const unisim::kernel::Object &obj, const std::string &str);
	void ReportTrap(unsigned int prc_num, const unisim::kernel::Object &obj, const char *c_str);

	// Export to Front-ends
	
	// unisim::service::interfaces::DebugYieldingRequest (tagged)
	void DebugYieldRequest(unsigned int front_end_num);

	// unisim::service::interfaces::DebugSelecting (tagged)
	bool DebugSelect(unsigned int front_end_num, unsigned int prc_num);
	unsigned int DebugGetSelected(unsigned int front_end_num) const;
	bool DebugFrameSelect(unsigned int front_end_num, unsigned int frame_num);
	unsigned int DebugFrameGetSelected(unsigned int front_end_num) const;

	// unisim::service::interfaces::DebugEventTrigger<ADDRESS> (tagged)
	bool Listen(unsigned int front_end_num, unisim::util::debug::Event<ADDRESS> *event);
	bool Unlisten(unsigned int front_end_num, unisim::util::debug::Event<ADDRESS> *event);
	bool IsEventListened(unsigned int front_end_num, unisim::util::debug::Event<ADDRESS> *event) const;
	void ScanListenedEvents(unsigned int front_end_num, unisim::service::interfaces::DebugEventScanner<ADDRESS>& scanner) const;
	void ClearEvents(unsigned int front_end_num);
	bool SetBreakpoint(unsigned int front_end_num, ADDRESS addr);
	bool RemoveBreakpoint(unsigned int front_end_num, ADDRESS addr);
	bool HasBreakpoints(unsigned int front_end_num, ADDRESS addr);
	bool SetWatchpoint(unsigned int front_end_num, unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size, bool overlook);
	bool RemoveWatchpoint(unsigned int front_end_num, unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size);
	bool HasWatchpoints(unsigned int front_end_num, unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size);
	
	// unisim::service::interfaces::Disassembly<ADDRESS> (tagged)
	std::string Disasm(unsigned int front_end_num, ADDRESS addr, ADDRESS& next_addr);
	
	// unisim::service::interfaces::Memory<ADDRESS> (tagged)
	void ResetMemory(unsigned int front_end_num);
	bool ReadMemory(unsigned int front_end_num, ADDRESS addr, void *buffer, uint32_t size);
	bool WriteMemory(unsigned int front_end_num, ADDRESS addr, const void *buffer, uint32_t size);
	
	// unisim::service::interfaces::Registers (tagged)
	unisim::service::interfaces::Register *GetRegister(unsigned int front_end_num, const char *name);
	void ScanRegisters(unsigned int front_end_num, unisim::service::interfaces::RegisterScanner& scanner);
	
	// unisim::service::interfaces::SymbolTableLookup<ADDRESS> (tagged)
	void GetSymbols(unsigned int front_end_num, typename std::list<const unisim::util::debug::Symbol<ADDRESS> *>& lst, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const;
	const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbol(unsigned int front_end_num, const char *name, ADDRESS addr, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const;
	const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbolByAddr(unsigned int front_end_num, ADDRESS addr) const;
	const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbolByName(unsigned int front_end_num, const char *name) const;
	const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbolByName(unsigned int front_end_num, const char *name, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const;
	const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbolByAddr(unsigned int front_end_num, ADDRESS addr, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const;
	
	// unisim::service::interfaces::StatementLookup<ADDRESS> (tagged)
	void ScanStatements(unsigned int front_end_num, unisim::service::interfaces::StatementScanner<ADDRESS>& scanner, const char *filename) const;
	const unisim::util::debug::Statement<ADDRESS> *FindStatement(unsigned int front_end_num, ADDRESS addr, const char *filename, typename unisim::service::interfaces::StatementLookup<ADDRESS>::FindStatementOption opt) const;
	const unisim::util::debug::Statement<ADDRESS> *FindStatements(unsigned int front_end_num, std::vector<const unisim::util::debug::Statement<ADDRESS> *> &stmts, ADDRESS addr, const char *filename, typename unisim::service::interfaces::StatementLookup<ADDRESS>::FindStatementOption opt) const;
	const unisim::util::debug::Statement<ADDRESS> *FindStatement(unsigned int front_end_num, const unisim::util::debug::SourceCodeLocation& source_code_location, const char *filename) const;
	const unisim::util::debug::Statement<ADDRESS> *FindStatements(unsigned int front_end_num, std::vector<const unisim::util::debug::Statement<ADDRESS> *> &stmts, const unisim::util::debug::SourceCodeLocation& source_code_location, const char *filename) const;
	void ScanStatements(unsigned int front_end_num, unisim::service::interfaces::StatementScanner<ADDRESS>& scanner, const std::string& filename) const { ScanStatements(front_end_num, scanner, filename.length() ? filename.c_str() : 0); }
	const unisim::util::debug::Statement<ADDRESS> *FindStatement(unsigned int front_end_num, ADDRESS addr, const std::string& filename, typename unisim::service::interfaces::StatementLookup<ADDRESS>::FindStatementOption opt) const { return FindStatement(front_end_num, addr, filename.length() ? filename.c_str() : 0, opt); }
	const unisim::util::debug::Statement<ADDRESS> *FindStatements(unsigned int front_end_num, std::vector<const unisim::util::debug::Statement<ADDRESS> *> &stmts, ADDRESS addr, const std::string& filename, typename unisim::service::interfaces::StatementLookup<ADDRESS>::FindStatementOption opt) const { return FindStatements(front_end_num, stmts, addr, filename.length() ? filename.c_str() : 0, opt); }
	const unisim::util::debug::Statement<ADDRESS> *FindStatement(unsigned int front_end_num, const unisim::util::debug::SourceCodeLocation& source_code_location, const std::string& filename) const { return FindStatement(front_end_num, source_code_location, filename.length() ? filename.c_str() : 0); }
	const unisim::util::debug::Statement<ADDRESS> *FindStatements(unsigned int front_end_num, std::vector<const unisim::util::debug::Statement<ADDRESS> *> &stmts, const unisim::util::debug::SourceCodeLocation& source_code_location, const std::string& filename) const { return FindStatements(front_end_num, stmts, source_code_location, filename.length() ? filename.c_str() : 0); }
	
	// unisim::service::interfaces::BackTrace<ADDRESS> (tagged)
	std::vector<ADDRESS> *GetBackTrace(unsigned int front_end_num) const;
	bool GetReturnAddress(unsigned int front_end_num, ADDRESS& ret_addr) const;
	
	// unisim::service::interfaces::DebugInfoLoading (tagged)
	bool LoadDebugInfo(unsigned int front_end_num, const char *filename);
	bool EnableBinary(unsigned int front_end_num, const char *filename, bool enable);
	void EnumerateBinaries(unsigned int front_end_num, std::list<std::string>& lst) const;
	bool IsBinaryEnabled(unsigned int front_end_num, const char *filename) const;
	
	// unisim::service::interfaces::DataObjectLookup<ADDRESS> (tagged)
	unisim::util::debug::DataObjectRef<ADDRESS> FindDataObject(unsigned int front_end_num, const char *data_object_name) const;
	void EnumerateDataObjectNames(unsigned int front_end_num, std::set<std::string>& name_set, typename unisim::service::interfaces::DataObjectLookup<ADDRESS>::Scope scope) const;
	
	// unisim::service::interfaces::SubProgramLookup<ADDRESS> (tagged)
	const unisim::util::debug::SubProgram<ADDRESS> *FindSubProgram(unsigned int front_end_num, const char *subprogram_name, const char *filename = 0, const char *compilation_unit_name = 0) const;
	const unisim::util::debug::SubProgram<ADDRESS> *FindSubProgram(unsigned int front_end_num, ADDRESS pc, const char *filename = 0) const;
	
	// unisim::service::interfaces::StackUnwinding (tagged)
	bool UnwindStack(unsigned int front_end_num, unsigned int frame_num = 1);
	
	// unisim::service::interfaces::Stubbing<ADDRESS> (tagged)
	void ScanStubs(unsigned int front_end_num, unisim::service::interfaces::StubScanner<ADDRESS>& scanner) const;
	bool SetStub(unsigned int front_end_num, unisim::util::debug::Stub<ADDRESS> *stub);
	bool RemoveStub(unsigned int front_end_num, unisim::util::debug::Stub<ADDRESS> *stub);
	
	// unisim::service::interfaces::Hooking<ADDRESS> (tagged)
	void ScanHooks(unsigned int front_end_num, unisim::service::interfaces::HookScanner<ADDRESS>& scanner) const;
	bool SetHook(unsigned int front_end_num, unisim::util::debug::Hook<ADDRESS> *hook);
	bool RemoveHook(unsigned int front_end_num, unisim::util::debug::Hook<ADDRESS> *hook);

	// unisim::service::interfaces::DebugEventListener<ADDRESS> (tagged)
	void OnDebugEvent(unsigned int front_end_num, const unisim::util::debug::Event<ADDRESS> *event);
	
	// unisim::service::interfaces::DebugTiming<TIME_TYPE> (tagged)
	const TIME_TYPE& DebugGetTime(unsigned int front_end_num) const;
	const TIME_TYPE& DebugGetTime(unsigned int front_end_num, unsigned int prc_num) const;
	
	struct ProcessorGate
		: unisim::kernel::Service<unisim::service::interfaces::DebugYielding>
		, unisim::kernel::Service<unisim::service::interfaces::MemoryAccessReporting<ADDRESS> >
		, unisim::kernel::Service<unisim::service::interfaces::TrapReporting>
		, unisim::kernel::Client<unisim::service::interfaces::MemoryAccessReportingControl>
		, unisim::kernel::Client<unisim::service::interfaces::Disassembly<ADDRESS> >
		, unisim::kernel::Client<unisim::service::interfaces::Memory<ADDRESS> >
		, unisim::kernel::Client<unisim::service::interfaces::Registers>
		, unisim::kernel::Client<unisim::service::interfaces::DebugTiming<TIME_TYPE> >
	{
		// From Processor
		unisim::kernel::ServiceExport<unisim::service::interfaces::DebugYielding> debug_yielding_export;
		unisim::kernel::ServiceExport<unisim::service::interfaces::MemoryAccessReporting<ADDRESS> > memory_access_reporting_export;
		unisim::kernel::ServiceExport<unisim::service::interfaces::TrapReporting> trap_reporting_export;
		
		// To Processor
		unisim::kernel::ServiceImport<unisim::service::interfaces::MemoryAccessReportingControl> memory_access_reporting_control_import;
		unisim::kernel::ServiceImport<unisim::service::interfaces::Disassembly<ADDRESS> > disasm_import;
		unisim::kernel::ServiceImport<unisim::service::interfaces::Memory<ADDRESS> > memory_import;
		unisim::kernel::ServiceImport<unisim::service::interfaces::Registers> registers_import;
		unisim::kernel::ServiceImport<unisim::service::interfaces::DebugTiming<TIME_TYPE> > debug_timing_import;

		ProcessorGate(const char *name, unsigned int _id, Debugger<CONFIG> *parent)
			: unisim::kernel::Object(name, parent)
			, unisim::kernel::Service<unisim::service::interfaces::DebugYielding>(name, parent)
			, unisim::kernel::Service<unisim::service::interfaces::MemoryAccessReporting<ADDRESS> >(name, parent)
			, unisim::kernel::Service<unisim::service::interfaces::TrapReporting>(name, parent)
			, unisim::kernel::Client<unisim::service::interfaces::MemoryAccessReportingControl>(name, parent)
			, unisim::kernel::Client<unisim::service::interfaces::Disassembly<ADDRESS> >(name, parent)
			, unisim::kernel::Client<unisim::service::interfaces::Memory<ADDRESS> >(name, parent)
			, unisim::kernel::Client<unisim::service::interfaces::Registers>(name, parent)
			, unisim::kernel::Client<unisim::service::interfaces::DebugTiming<TIME_TYPE> >(name, parent)
			, debug_yielding_export("debug-yielding-export", this)
			, memory_access_reporting_export("memory-access-reporting-export", this)
			, trap_reporting_export("trap-reporting-export", this)
			, memory_access_reporting_control_import("memory-access-reporting-control-import", this)
			, disasm_import("disasm-import", this)
			, memory_import("memory-import", this)
			, registers_import("registers-import", this)
			, debug_timing_import("debug-timing-import", this)
			, dbg(*parent)
			, id(_id)
		{
			*dbg.debug_yielding_export         [id] >> debug_yielding_export;
			*dbg.memory_access_reporting_export[id] >> memory_access_reporting_export;
			*dbg.trap_reporting_export         [id] >> trap_reporting_export;
			
			memory_access_reporting_control_import >> *dbg.memory_access_reporting_control_import[id];
			disasm_import                          >> *dbg.disasm_import[id];
			memory_import                          >> *dbg.memory_import[id];
			registers_import                       >> *dbg.registers_import[id];
			debug_timing_import                    >> *dbg.debug_timing_import[id];
		}
		
		unsigned int GetProcessorNumber() const { return id; }

		// From Processor
		
		// unisim::service::interfaces::DebugYielding
		virtual void DebugYield() { return dbg.DebugYield(id); }
		
		// unisim::service::interfaces::MemoryAccessReporting<ADDRESS>
		virtual bool ReportMemoryAccess(unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size) { return dbg.ReportMemoryAccess(id, mat, mt, addr, size); }
		virtual void ReportCommitInstruction(ADDRESS addr, unsigned int length) { dbg.ReportCommitInstruction(id, addr, length); }
		virtual void ReportFetchInstruction(ADDRESS next_addr) { dbg.ReportFetchInstruction(id, next_addr); }
		
		// unisim::service::interfaces::TrapReporting
		virtual void ReportTrap() { dbg.ReportTrap(id); }
		virtual void ReportTrap(const unisim::kernel::Object &obj) { dbg.ReportTrap(id, obj); }
		virtual void ReportTrap(const unisim::kernel::Object &obj, const std::string &str) { dbg.ReportTrap(id, obj, str); }
		virtual void ReportTrap(const unisim::kernel::Object &obj, const char *c_str) { dbg.ReportTrap(id, obj, c_str); }
		
		// To Processor
		
		// unisim::service::interfaces::Disassembly<ADDRESS>
		inline std::string Disasm(ADDRESS addr, ADDRESS& next_addr) { return disasm_import ? disasm_import->Disasm(addr, next_addr) : std::string(); }
		
		// unisim::service::interfaces::Memory<ADDRESS>
		inline void ResetMemory() { if(memory_import) memory_import->ResetMemory(); }
		inline bool ReadMemory(ADDRESS addr, void *buffer, uint32_t size) { return memory_import ? memory_import->ReadMemory(addr, buffer, size) : false; }
		inline bool WriteMemory(ADDRESS addr, const void *buffer, uint32_t size) { return memory_import ? memory_import->WriteMemory(addr, buffer, size) : false; }
		
		// unisim::service::interfaces::Registers
		inline unisim::service::interfaces::Register *GetRegister(const char *name) { return registers_import ? registers_import->GetRegister(name) : 0; }
		inline void ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner) { if(registers_import) registers_import->ScanRegisters(scanner); }
		
		// unisim::service::interfaces::MemoryAccessReportingControl
		inline void RequiresMemoryAccessReporting(unisim::service::interfaces::MemoryAccessReportingType type, bool report) { if(memory_access_reporting_control_import) memory_access_reporting_control_import->RequiresMemoryAccessReporting(type, report); }
		
		// unisim::service::interfaces::DebugTiming<TIME_TYPE>
		inline const TIME_TYPE& DebugGetTime() const { static TIME_TYPE t; return debug_timing_import ? debug_timing_import->DebugGetTime(id) : t; }
		inline const TIME_TYPE& DebugGetTime(unsigned int prc_num) const { static TIME_TYPE t; return debug_timing_import ? debug_timing_import->DebugGetTime(id) : t; }
		
	private:
		Debugger<CONFIG>& dbg;
		unsigned int id;
	};

	template <typename IMPORT>
	void RequireSetup( IMPORT ProcessorGate::*member )
	{
		for (unsigned idx = 0; idx < NUM_PROCESSORS; ++idx)
		{
			(prc_gate[idx]->*member).RequireSetup();
		}
	}
                      
	struct FrontEndGate
		: unisim::kernel::Service<unisim::service::interfaces::DebugYieldingRequest>
		, unisim::kernel::Service<unisim::service::interfaces::DebugSelecting>
		, unisim::kernel::Service<unisim::service::interfaces::DebugEventTrigger<ADDRESS> >
		, unisim::kernel::Service<unisim::service::interfaces::Disassembly<ADDRESS> >
		, unisim::kernel::Service<unisim::service::interfaces::Memory<ADDRESS> >
		, unisim::kernel::Service<unisim::service::interfaces::Registers>
		, unisim::kernel::Service<unisim::service::interfaces::SymbolTableLookup<ADDRESS> >
		, unisim::kernel::Service<unisim::service::interfaces::StatementLookup<ADDRESS> >
		, unisim::kernel::Service<unisim::service::interfaces::BackTrace<ADDRESS> >
		, unisim::kernel::Service<unisim::service::interfaces::DebugInfoLoading>
		, unisim::kernel::Service<unisim::service::interfaces::DataObjectLookup<ADDRESS> >
		, unisim::kernel::Service<unisim::service::interfaces::SubProgramLookup<ADDRESS> >
		, unisim::kernel::Service<unisim::service::interfaces::StackUnwinding>
		, unisim::kernel::Service<unisim::service::interfaces::Stubbing<ADDRESS> >
		, unisim::kernel::Service<unisim::service::interfaces::Hooking<ADDRESS> >
		, unisim::kernel::Service<unisim::service::interfaces::DebugTiming<TIME_TYPE> >
		, unisim::kernel::Client<unisim::service::interfaces::DebugYielding>
		, unisim::kernel::Client<unisim::service::interfaces::DebugEventListener<ADDRESS> >
	{
		// Exports to Front-end
		unisim::kernel::ServiceExport<unisim::service::interfaces::DebugYieldingRequest> debug_yielding_request_export;
		unisim::kernel::ServiceExport<unisim::service::interfaces::DebugSelecting> debug_selecting_export;
		unisim::kernel::ServiceExport<unisim::service::interfaces::DebugEventTrigger<ADDRESS> > debug_event_trigger_export; // depends on selected CPU number
		unisim::kernel::ServiceExport<unisim::service::interfaces::Disassembly<ADDRESS> > disasm_export;                  // depends on selected CPU number
		unisim::kernel::ServiceExport<unisim::service::interfaces::Memory<ADDRESS> > memory_export;                       // depends on selected CPU number
		unisim::kernel::ServiceExport<unisim::service::interfaces::Registers> registers_export;                           // depends on selected CPU number
		unisim::kernel::ServiceExport<unisim::service::interfaces::SymbolTableLookup<ADDRESS> > symbol_table_lookup_export;
		unisim::kernel::ServiceExport<unisim::service::interfaces::StatementLookup<ADDRESS> > stmt_lookup_export;
		unisim::kernel::ServiceExport<unisim::service::interfaces::BackTrace<ADDRESS> > backtrace_export;                 // depends on selected CPU number 
		unisim::kernel::ServiceExport<unisim::service::interfaces::DebugInfoLoading> debug_info_loading_export;
		unisim::kernel::ServiceExport<unisim::service::interfaces::DataObjectLookup<ADDRESS> > data_object_lookup_export; // depends on selected CPU number
		unisim::kernel::ServiceExport<unisim::service::interfaces::SubProgramLookup<ADDRESS> > subprogram_lookup_export; // depends on selected CPU number
		unisim::kernel::ServiceExport<unisim::service::interfaces::StackUnwinding> stack_unwinding_export;               // depends on selected CPU number 
		unisim::kernel::ServiceExport<unisim::service::interfaces::Stubbing<ADDRESS> > stubbing_export;
		unisim::kernel::ServiceExport<unisim::service::interfaces::Hooking<ADDRESS> > hooking_export;
		unisim::kernel::ServiceExport<unisim::service::interfaces::DebugTiming<TIME_TYPE> > debug_timing_export;
		
		// Imports from Front-end
		unisim::kernel::ServiceImport<unisim::service::interfaces::DebugEventListener<ADDRESS> > debug_event_listener_import;
		unisim::kernel::ServiceImport<unisim::service::interfaces::DebugYielding> debug_yielding_import;

		FrontEndGate(const char *name, unsigned int _id, Debugger<CONFIG> *parent)
			: unisim::kernel::Object(name, parent)
			, unisim::kernel::Service<unisim::service::interfaces::DebugYieldingRequest>(name, parent)
			, unisim::kernel::Service<unisim::service::interfaces::DebugSelecting>(name, parent)
			, unisim::kernel::Service<unisim::service::interfaces::DebugEventTrigger<ADDRESS> >(name, parent)
			, unisim::kernel::Service<unisim::service::interfaces::Disassembly<ADDRESS> >(name, parent)
			, unisim::kernel::Service<unisim::service::interfaces::Memory<ADDRESS> >(name, parent)
			, unisim::kernel::Service<unisim::service::interfaces::Registers>(name, parent)
			, unisim::kernel::Service<unisim::service::interfaces::SymbolTableLookup<ADDRESS> >(name, parent)
			, unisim::kernel::Service<unisim::service::interfaces::StatementLookup<ADDRESS> >(name, parent)
			, unisim::kernel::Service<unisim::service::interfaces::BackTrace<ADDRESS> >(name, parent)
			, unisim::kernel::Service<unisim::service::interfaces::DebugInfoLoading>(name, parent)
			, unisim::kernel::Service<unisim::service::interfaces::DataObjectLookup<ADDRESS> >(name, parent)
			, unisim::kernel::Service<unisim::service::interfaces::SubProgramLookup<ADDRESS> >(name, parent)
			, unisim::kernel::Service<unisim::service::interfaces::StackUnwinding>(name, parent)
			, unisim::kernel::Service<unisim::service::interfaces::Stubbing<ADDRESS> >(name, parent)
			, unisim::kernel::Service<unisim::service::interfaces::Hooking<ADDRESS> >(name, parent)
			, unisim::kernel::Service<unisim::service::interfaces::DebugTiming<TIME_TYPE> >(name, parent)
			, unisim::kernel::Client<unisim::service::interfaces::DebugYielding>(name, parent)
			, unisim::kernel::Client<unisim::service::interfaces::DebugEventListener<ADDRESS> >(name, parent)
			, debug_yielding_request_export("debug-yielding-request-export", this)
			, debug_selecting_export("debug-selecting-export", this)
			, debug_event_trigger_export("debug-event-trigger-export", this)
			, disasm_export("disasm-export", this)
			, memory_export("memory-export", this)
			, registers_export("registers-export", this)
			, symbol_table_lookup_export("symbol-table-lookup-export", this)
			, stmt_lookup_export("stmt-lookup-export", this)
			, backtrace_export("backtrace-export", this)
			, debug_info_loading_export("debug-info-loading-export", this)
			, data_object_lookup_export("data-object-lookup-export", this)
			, subprogram_lookup_export("subprogram-lookup-export", this)
			, stack_unwinding_export("stack-unwinding-export", this)
			, stubbing_export("stubbing-export", this)
			, hooking_export("hooking-export", this)
			, debug_timing_export("debug-timing-export", this)
			, debug_event_listener_import("debug-event-listener-import", this)
			, debug_yielding_import("debug-yielding-import", this)
			, dbg(*parent)
			, id(_id)
		{
			*dbg.debug_yielding_request_export[id] >> debug_yielding_request_export;
			*dbg.debug_selecting_export       [id] >> debug_selecting_export;
			*dbg.debug_event_trigger_export   [id] >> debug_event_trigger_export;
			*dbg.disasm_export                [id] >> disasm_export;
			*dbg.memory_export                [id] >> memory_export;
			*dbg.registers_export             [id] >> registers_export;
			*dbg.symbol_table_lookup_export   [id] >> symbol_table_lookup_export;
			*dbg.stmt_lookup_export           [id] >> stmt_lookup_export;
			*dbg.backtrace_export             [id] >> backtrace_export;
			*dbg.debug_info_loading_export    [id] >> debug_info_loading_export;
			*dbg.data_object_lookup_export    [id] >> data_object_lookup_export;
			*dbg.subprogram_lookup_export     [id] >> subprogram_lookup_export;
			*dbg.stack_unwinding_export       [id] >> stack_unwinding_export;
			*dbg.stubbing_export              [id] >> stubbing_export;
			*dbg.hooking_export               [id] >> hooking_export;
			*dbg.debug_timing_export          [id] >> debug_timing_export;
			
			debug_event_listener_import >> *dbg.debug_event_listener_import[id];
			debug_yielding_import       >> *dbg.debug_yielding_import[id];
		}

		virtual void Setup(interfaces::DebugEventTrigger<ADDRESS>*) { dbg.RequireSetup(&ProcessorGate::memory_access_reporting_control_import); }
		virtual void Setup(interfaces::Memory<ADDRESS>*) { dbg.RequireSetup(&ProcessorGate::memory_import); }
		virtual void Setup(interfaces::Registers*) { dbg.RequireSetup(&ProcessorGate::registers_import); }
		virtual void Setup(interfaces::Disassembly<ADDRESS>*) { dbg.RequireSetup(&ProcessorGate::disasm_import); }
		
		virtual void Setup(interfaces::SymbolTableLookup<ADDRESS>*) { dbg.SetupDebugInfo(); }
		virtual void Setup(interfaces::StatementLookup<ADDRESS>*) { dbg.SetupDebugInfo(); }
		virtual void Setup(interfaces::BackTrace<ADDRESS>*)
		{
			dbg.RequireSetup(&ProcessorGate::registers_import);
			dbg.RequireSetup(&ProcessorGate::memory_import);
			dbg.SetupDebugInfo();
		}
		virtual void Setup(interfaces::DebugInfoLoading*) { dbg.SetupDebugInfo(); }
		virtual void Setup(interfaces::DataObjectLookup<ADDRESS>*)
		{
			dbg.RequireSetup(&ProcessorGate::registers_import);
			dbg.RequireSetup(&ProcessorGate::memory_import);
			dbg.SetupDebugInfo();
		}
		virtual void Setup(interfaces::SubProgramLookup<ADDRESS>*) { dbg.SetupDebugInfo(); }
		
		// From Front-end
		
		// unisim::service::interfaces::DebugYieldingRequest
		virtual void DebugYieldRequest() { /*dbg.Lock(); */dbg.DebugYieldRequest(id); /*dbg.Unlock();*/ }

		// unisim::service::interfaces::DebugSelecting
		virtual bool DebugSelect(unsigned int prc_num) { /*dbg.Lock();*/ bool ret = dbg.DebugSelect(id, prc_num); /*dbg.Unlock();*/ return ret; }
		virtual unsigned int DebugGetSelected() const { /*dbg.Lock();*/ unsigned int ret = dbg.DebugGetSelected(id); /*dbg.Unlock();*/ return ret; }
		virtual bool DebugFrameSelect(unsigned int frame_num) { /*dbg.Lock();*/ bool ret = dbg.DebugFrameSelect(id, frame_num); /*dbg.Unlock();*/ return ret; }
		virtual unsigned int DebugFrameGetSelected() const { /*dbg.Lock();*/ unsigned int ret = dbg.DebugFrameGetSelected(id); /*dbg.Unlock();*/ return ret; }
		
		// unisim::service::interfaces::DebugEventTrigger<ADDRESS>
		virtual bool Listen(unisim::util::debug::Event<ADDRESS> *event) { bool l = dbg.Lock(id); bool ret = dbg.Listen(id, event); if(l) { dbg.Unlock(id); } return ret; }
		virtual bool Unlisten(unisim::util::debug::Event<ADDRESS> *event) { bool l = dbg.Lock(id); bool ret = dbg.Unlisten(id, event); if(l) { dbg.Unlock(id); } return ret; }
		virtual bool IsEventListened(unisim::util::debug::Event<ADDRESS> *event) const { bool l = dbg.Lock(id); bool ret = dbg.IsEventListened(id, event); if(l) { dbg.Unlock(id); } return ret; }
		virtual void ScanListenedEvents(unisim::service::interfaces::DebugEventScanner<ADDRESS>& scanner) const { bool l = dbg.Lock(id); dbg.ScanListenedEvents(id, scanner); if(l) { dbg.Unlock(id); } }
		virtual void ClearEvents() { bool l = dbg.Lock(id); dbg.ClearEvents(id); if(l) { dbg.Unlock(id); } }
		virtual bool SetBreakpoint(ADDRESS addr) { bool l = dbg.Lock(id); bool ret = dbg.SetBreakpoint(id, addr); if(l) { dbg.Unlock(id); } return ret; }
		virtual bool RemoveBreakpoint(ADDRESS addr) { bool l = dbg.Lock(id); bool ret = dbg.RemoveBreakpoint(id, addr); if(l) { dbg.Unlock(id); } return ret; }
		virtual bool HasBreakpoints(ADDRESS addr) { bool l = dbg.Lock(id); bool ret = dbg.HasBreakpoints(id, addr); if(l) { dbg.Unlock(id); } return ret; }
		virtual bool SetWatchpoint(unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size, bool overlook) { bool l = dbg.Lock(id); bool ret = dbg.SetWatchpoint(id, mat, mt, addr, size, overlook); if(l) { dbg.Unlock(id); } return ret; }
		virtual bool RemoveWatchpoint(unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size) { bool l = dbg.Lock(id); bool ret = dbg.RemoveWatchpoint(id, mat, mt, addr, size); if(l) { dbg.Unlock(id); } return ret; }
		virtual bool HasWatchpoints(unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size) { bool l = dbg.Lock(id); bool ret = dbg.HasWatchpoints(id, mat, mt, addr, size); if(l) { dbg.Unlock(id); } return ret; }
		
		// unisim::service::interfaces::Disassembly<ADDRESS>
		virtual std::string Disasm(ADDRESS addr, ADDRESS& next_addr) { bool l = dbg.Lock(id); std::string ret = dbg.Disasm(id, addr, next_addr); if(l) { dbg.Unlock(id); } return ret; }
		
		// unisim::service::interfaces::Memory<ADDRESS>
		virtual void ResetMemory() { bool l = dbg.Lock(id); dbg.ResetMemory(id); if(l) { dbg.Unlock(id); } }
		virtual bool ReadMemory(ADDRESS addr, void *buffer, uint32_t size) { bool l = dbg.Lock(id); bool ret = dbg.ReadMemory(id, addr, buffer, size); if(l) { dbg.Unlock(id); } return ret; }
		virtual bool WriteMemory(ADDRESS addr, const void *buffer, uint32_t size) { bool l = dbg.Lock(id); bool ret = dbg.WriteMemory(id, addr, buffer, size); if(l) { dbg.Unlock(id); } return ret; }
		
		// unisim::service::interfaces::Registers
		virtual unisim::service::interfaces::Register *GetRegister(const char *name) { bool l = dbg.Lock(id); unisim::service::interfaces::Register *ret = dbg.GetRegister(id, name); if(l) { dbg.Unlock(id); } return ret; }
		virtual void ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner) { bool l = dbg.Lock(id); dbg.ScanRegisters(id, scanner); if(l) { dbg.Unlock(id); } }
		
		// unisim::service::interfaces::SymbolTableLookup<ADDRESS>
		virtual void GetSymbols(typename std::list<const unisim::util::debug::Symbol<ADDRESS> *>& lst, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const { bool l = dbg.Lock(id); dbg.GetSymbols(id, lst, type); if(l) { dbg.Unlock(id); } }
		virtual const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbol(const char *name, ADDRESS addr, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const { bool l = dbg.Lock(id); const typename unisim::util::debug::Symbol<ADDRESS> *ret = dbg.FindSymbol(id, name, addr, type); if(l) { dbg.Unlock(id); } return ret; }
		virtual const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbolByAddr(ADDRESS addr) const { bool l = dbg.Lock(id); const typename unisim::util::debug::Symbol<ADDRESS> *ret = dbg.FindSymbolByAddr(id, addr); if(l) { dbg.Unlock(id); } return ret; }
		virtual const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbolByName(const char *name) const { bool l = dbg.Lock(id); const typename unisim::util::debug::Symbol<ADDRESS> *ret = dbg.FindSymbolByName(id, name); if(l) { dbg.Unlock(id); } return ret; }
		virtual const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbolByName(const char *name, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const { bool l = dbg.Lock(id); const typename unisim::util::debug::Symbol<ADDRESS> *ret = dbg.FindSymbolByName(id, name, type); if(l) { dbg.Unlock(id); } return ret; }
		virtual const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbolByAddr(ADDRESS addr, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const { bool l = dbg.Lock(id); const typename unisim::util::debug::Symbol<ADDRESS> *ret = dbg.FindSymbolByAddr(id, addr, type); if(l) { dbg.Unlock(id); } return ret; }
		
		// unisim::service::interfaces::StatementLookup<ADDRESS>
		virtual void ScanStatements(unisim::service::interfaces::StatementScanner<ADDRESS>& scanner, const char *filename) const { bool l = dbg.Lock(id); dbg.ScanStatements(id, scanner, filename); if(l) { dbg.Unlock(id); } }
		virtual const unisim::util::debug::Statement<ADDRESS> *FindStatement(ADDRESS addr, const char *filename, typename unisim::service::interfaces::StatementLookup<ADDRESS>::FindStatementOption opt) const { bool l = dbg.Lock(id); const unisim::util::debug::Statement<ADDRESS> *ret = dbg.FindStatement(id, addr, filename, opt); if(l) { dbg.Unlock(id); } return ret; }
		virtual const unisim::util::debug::Statement<ADDRESS> *FindStatements(std::vector<const unisim::util::debug::Statement<ADDRESS> *> &stmts, ADDRESS addr, const char *filename, typename unisim::service::interfaces::StatementLookup<ADDRESS>::FindStatementOption opt) const { bool l = dbg.Lock(id); const unisim::util::debug::Statement<ADDRESS> *ret = dbg.FindStatements(id, stmts, addr, filename, opt); if(l) { dbg.Unlock(id); } return ret; }
		virtual const unisim::util::debug::Statement<ADDRESS> *FindStatement(const unisim::util::debug::SourceCodeLocation& source_code_location, const char *filename) const { bool l = dbg.Lock(id); const unisim::util::debug::Statement<ADDRESS> *ret = dbg.FindStatement(id, source_code_location, filename); if(l) { dbg.Unlock(id); } return ret; }
		virtual const unisim::util::debug::Statement<ADDRESS> *FindStatements(std::vector<const unisim::util::debug::Statement<ADDRESS> *> &stmts, const unisim::util::debug::SourceCodeLocation& source_code_location, const char *filename) const { bool l = dbg.Lock(id); const unisim::util::debug::Statement<ADDRESS> *ret = dbg.FindStatements(id, stmts, source_code_location, filename); if(l) { dbg.Unlock(id); } return ret; }
		
		// unisim::service::interfaces::BackTrace<ADDRESS>
		virtual std::vector<ADDRESS> *GetBackTrace() const { bool l = dbg.Lock(id); std::vector<ADDRESS> *ret = dbg.GetBackTrace(id); if(l) { dbg.Unlock(id); } return ret; }
		virtual bool GetReturnAddress(ADDRESS& ret_addr) const { bool l = dbg.Lock(id); bool ret = dbg.GetReturnAddress(id, ret_addr); if(l) { dbg.Unlock(id); } return ret; }
		
		// unisim::service::interfaces::DebugInfoLoading
		virtual bool LoadDebugInfo(const char *filename) { bool l = dbg.Lock(id); bool ret = dbg.LoadDebugInfo(id, filename); if(l) { dbg.Unlock(id); } return ret; }
		virtual bool EnableBinary(const char *filename, bool enable) { bool l = dbg.Lock(id); bool ret = dbg.EnableBinary(id, filename, enable); if(l) { dbg.Unlock(id); } return ret; }
		virtual void EnumerateBinaries(std::list<std::string>& lst) const { bool l = dbg.Lock(id); dbg.EnumerateBinaries(id, lst); if(l) { dbg.Unlock(id); } }
		virtual bool IsBinaryEnabled(const char *filename) const { bool l = dbg.Lock(id); bool ret = dbg.IsBinaryEnabled(id, filename); if(l) { dbg.Unlock(id); } return ret; }
		
		// unisim::service::interfaces::DataObjectLookup<ADDRESS>
		virtual unisim::util::debug::DataObjectRef<ADDRESS> FindDataObject(const char *data_object_name) const { bool l = dbg.Lock(id); unisim::util::debug::DataObjectRef<ADDRESS> ret = dbg.FindDataObject(id, data_object_name); if(l) { dbg.Unlock(id); } return ret; }
		virtual void EnumerateDataObjectNames(std::set<std::string>& name_set, typename unisim::service::interfaces::DataObjectLookup<ADDRESS>::Scope scope) const { bool l = dbg.Lock(id); dbg.EnumerateDataObjectNames(id, name_set, scope); if(l) { dbg.Unlock(id); } }
		
		// unisim::service::interfaces::SubProgramLookup<ADDRESS>
		virtual const unisim::util::debug::SubProgram<ADDRESS> *FindSubProgram(const char *subprogram_name, const char *filename, const char *compilation_unit_name) const { bool l = dbg.Lock(id); const unisim::util::debug::SubProgram<ADDRESS> *ret = dbg.FindSubProgram(id, subprogram_name, filename, compilation_unit_name); if(l) { dbg.Unlock(id); } return ret; }
		virtual const unisim::util::debug::SubProgram<ADDRESS> *FindSubProgram(ADDRESS pc, const char *filename) const { bool l = dbg.Lock(id); const unisim::util::debug::SubProgram<ADDRESS> *ret = dbg.FindSubProgram(id, pc, filename); if(l) { dbg.Unlock(id); } return ret; }
		
		// unisim::service::interfaces::StackUnwinding
		virtual bool UnwindStack(unsigned int frame_num) { bool l = dbg.Lock(id); bool ret = dbg.UnwindStack(id, frame_num); if(l) { dbg.Unlock(id); } return ret; }
		
		// unisim::service::interfaces::Stubbing<ADDRESS>
		virtual void ScanStubs(unisim::service::interfaces::StubScanner<ADDRESS>& scanner) const { bool l = dbg.Lock(id); dbg.ScanStubs(id, scanner); if(l) { dbg.Unlock(id); } }
		virtual bool SetStub(unisim::util::debug::Stub<ADDRESS> *stub) { bool l = dbg.Lock(id); bool ret = dbg.SetStub(id, stub); if(l) { dbg.Unlock(id); } return ret; }
		virtual bool RemoveStub(unisim::util::debug::Stub<ADDRESS> *stub) { bool l = dbg.Lock(id); bool ret = dbg.RemoveStub(id, stub); if(l) { dbg.Unlock(id); } return ret; }
		
		// unisim::service::interfaces::Hooking<ADDRESS>
		virtual void ScanHooks(unisim::service::interfaces::HookScanner<ADDRESS>& scanner) const { bool l = dbg.Lock(id); dbg.ScanHooks(id, scanner); if(l) { dbg.Unlock(id); } } 
		virtual bool SetHook(unisim::util::debug::Hook<ADDRESS> *hook) { bool l = dbg.Lock(id); bool ret = dbg.SetHook(id, hook); if(l) { dbg.Unlock(id); } return ret; }
		virtual bool RemoveHook(unisim::util::debug::Hook<ADDRESS> *hook) { bool l = dbg.Lock(id); bool ret = dbg.RemoveHook(id, hook); if(l) { dbg.Unlock(id); } return ret; }
		
		// unisim::service::interfaces::DebugTiming<TIME_TYPE>
		virtual const TIME_TYPE& DebugGetTime() const { bool l = dbg.Lock(id); const TIME_TYPE& ret = dbg.DebugGetTime(id); if(l) { dbg.Unlock(id); } return ret; }
		virtual const TIME_TYPE& DebugGetTime(unsigned int prc_num) const { bool l = dbg.Lock(id); const TIME_TYPE& ret = dbg.DebugGetTime(id, prc_num); if(l) { dbg.Unlock(id); } return ret; }
		
		// To Front-end
		
		// unisim::service::interfaces::DebugYielding
		inline void DebugYield() { if(debug_yielding_import) debug_yielding_import->DebugYield(); }
		
		// unisim::service::interfaces::DebugEventListener<ADDRESS>
		inline void OnDebugEvent(const unisim::util::debug::Event<ADDRESS> *event) { if(debug_event_listener_import) debug_event_listener_import->OnDebugEvent(event); }
	private:
		Debugger<CONFIG>& dbg;
		unsigned int id;
	};
	
	template <typename T, bool dummy = true>
	struct Dispatcher
	{
		Dispatcher(Debugger<CONFIG>& _dbg, unsigned int _front_end_num) {}
		void Visit(T *ev) {}
	};
	
	template <bool dummy>
	struct Dispatcher<unisim::util::debug::Breakpoint<ADDRESS>, dummy>
	{
		Dispatcher(Debugger<CONFIG>& _dbg, unsigned int _front_end_num) : dbg(_dbg), front_end_num(_front_end_num) {}
		void Visit(unisim::util::debug::Breakpoint<ADDRESS> *brkp)
		{
			unisim::util::debug::Event<ADDRESS> *event = brkp;
			unisim::util::debug::Event<ADDRESS> *ref;
			do
			{
				ref = event->GetReference();
				if(ref) event = ref;
			}
			while(ref);
			
			dbg.OnDebugEvent(front_end_num, event);
		}
		Debugger<CONFIG>& dbg;
		unsigned int front_end_num;
	};

	template <bool dummy>
	struct Dispatcher<unisim::util::debug::Watchpoint<ADDRESS>, dummy>
	{
		Dispatcher(Debugger<CONFIG>& _dbg, unsigned int _front_end_num, bool& _overlook) : dbg(_dbg), front_end_num(_front_end_num), overlook(_overlook) {}
		void Visit(unisim::util::debug::Watchpoint<ADDRESS> *wp)
		{
			if(!wp->Overlooks())
			{
				overlook = false;
			}
// 			dbg.front_end_gate[front_end_num]->OnDebugEvent(wp);
// 			dbg.OnDebugEvent(front_end_num, wp);
			unisim::util::debug::Event<ADDRESS> *event = wp;
			unisim::util::debug::Event<ADDRESS> *ref;
			do
			{
				ref = event->GetReference();
				if(ref) event = ref;
			}
			while(ref);
			
			dbg.OnDebugEvent(front_end_num, event);
		}
		Debugger<CONFIG>& dbg;
		unsigned int front_end_num;
		bool& overlook;
	};

	struct BreakpointRemover
	{
		BreakpointRemover(Debugger<CONFIG>& _dbg) : dbg(_dbg), status(true) {}
		void Visit(unisim::util::debug::Breakpoint<ADDRESS> *brkp) { if(!dbg.breakpoint_registry.RemoveBreakpoint(brkp)) { status = false; } }
		Debugger<CONFIG>& dbg;
		bool status;
	};
	
	template <typename ADDRESS>
	class StubEvent : public unisim::util::debug::CustomEvent<ADDRESS, StubEvent<ADDRESS> >
	{
	public:
		StubEvent(unisim::util::debug::Stub<ADDRESS> *_stub) : unisim::util::debug::CustomEvent<ADDRESS, StubEvent<ADDRESS> >(), stub(_stub) {}
		virtual ~StubEvent() { delete stub; if(breakpoint) breakpoint->Release(); }
		
		unisim::util::debug::Stub<ADDRESS> *GetStub() const { return stub; }
		unsigned int GetProcessorNumber() const { return breakpoint->GetProcessorNumber(); }
		
		void Attach(unisim::util::debug::Breakpoint<ADDRESS> *brkp) { breakpoint = brkp; brkp->Catch(); }
		
		/* struct Visitor { void Visit(Breakpoint<ADDRESS> *) {} }; */
		template <class VISITOR> void Scan(VISITOR& visitor) { visitor.Visit(breakpoint); }
	private:
		unisim::util::debug::Stub<ADDRESS> *stub;
		unisim::util::debug::Breakpoint<ADDRESS> *breakpoint;
	};
	
	template <typename ADDRESS>
	class HookEvent : public unisim::util::debug::CustomEvent<ADDRESS, HookEvent<ADDRESS> >
	{
	public:
		HookEvent(unisim::util::debug::Hook<ADDRESS> *_hook)
			: unisim::util::debug::CustomEvent<ADDRESS, HookEvent<ADDRESS> >()
			, hook(_hook)
		{
		}
		
		virtual ~HookEvent()
		{
			delete hook;
			for(typename Breakpoints::iterator it = breakpoints.begin(); it != breakpoints.end(); ++it)
			{
				unisim::util::debug::Breakpoint<ADDRESS> *breakpoint = *it;
				breakpoint->Release();
			}
		}
		
		unisim::util::debug::Hook<ADDRESS> *GetHook() const { return hook; }
		
		void Attach(unisim::util::debug::Breakpoint<ADDRESS> *brkp)
		{
			std::pair<typename Breakpoints::iterator , bool> r = breakpoints.insert(brkp);
			if(r.second)
			{
				brkp->Catch();
			}
		}
		
		/* struct Visitor { void Visit(unisim::util::debug::Breakpoint<ADDRESS> *) {} }; */
		template <class VISITOR> void Scan(VISITOR& visitor)
		{
			for(typename Breakpoints::iterator it = breakpoints.begin(); it != breakpoints.end(); ++it)
			{
				unisim::util::debug::Breakpoint<ADDRESS> *breakpoint = *it;
				visitor.Visit(breakpoint);
			}
		}
	private:
		unisim::util::debug::Hook<ADDRESS> *hook;
		unisim::util::debug::SourceCodeLocation source_code_location;
		typedef std::set<unisim::util::debug::Breakpoint<ADDRESS> *> Breakpoints;
		Breakpoints breakpoints;
	};
	
	ProcessorGate *prc_gate[NUM_PROCESSORS];
	FrontEndGate *front_end_gate[MAX_FRONT_ENDS];
	
	// Currently selected processor gate
	ProcessorGate *sel_prc_gate[MAX_FRONT_ENDS];
	
	// Machine state
	unisim::util::debug::dwarf::DWARF_MachineState<ADDRESS> dw_mach_state[MAX_FRONT_ENDS];

	bool requires_fetch_instruction_reporting[NUM_PROCESSORS];
	bool requires_commit_instruction_reporting[NUM_PROCESSORS];
	bool requires_memory_access_reporting[NUM_PROCESSORS];

	bool verbose;
	std::string dwarf_to_html_output_directory;
	std::string dwarf_register_number_mapping_filename;
	bool parse_dwarf;
	bool debug_dwarf;
	unsigned int sel_cpu[MAX_FRONT_ENDS];
	std::string architecture[NUM_PROCESSORS];

	unisim::kernel::variable::Parameter<bool> param_verbose;
	unisim::kernel::variable::Parameter<std::string> param_dwarf_to_html_output_directory;
	unisim::kernel::variable::Parameter<std::string> param_dwarf_register_number_mapping_filename;
	unisim::kernel::variable::Parameter<bool> param_parse_dwarf;
	unisim::kernel::variable::Parameter<bool> param_debug_dwarf;
	unisim::kernel::variable::ParameterArray<unsigned int> param_sel_cpu;
	unisim::kernel::variable::ParameterArray<std::string> param_architecture;

	unisim::kernel::logger::Logger logger;
	bool setup_debug_info_done;
	pthread_mutex_t mutex;
	
	pthread_mutex_t curr_front_end_num_mutex;
	int curr_front_end_num;
	
	unisim::util::debug::BreakpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS> breakpoint_registry;
	unisim::util::debug::WatchpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS> watchpoint_registry;
	std::set<unisim::util::debug::FetchInsnEvent<ADDRESS> *> fetch_insn_event_set[NUM_PROCESSORS];
	std::set<unisim::util::debug::CommitInsnEvent<ADDRESS> *> commit_insn_event_set[NUM_PROCESSORS];
	std::set<unisim::util::debug::TrapEvent<ADDRESS> *> trap_event_set[NUM_PROCESSORS];
	typedef std::set<unisim::util::debug::SourceCodeBreakpoint<ADDRESS> *> SourceCodeBreakpointRegistry;
	SourceCodeBreakpointRegistry source_code_breakpoint_registry[MAX_FRONT_ENDS];
	typedef std::set<unisim::util::debug::SubProgramBreakpoint<ADDRESS> *> SubProgramBreakpointRegistry;
	SubProgramBreakpointRegistry subprogram_breakpoint_registry[MAX_FRONT_ENDS];
	typedef std::map<unisim::util::debug::Stub<ADDRESS> *, StubEvent<ADDRESS> *> StubEventRegistry;
	StubEventRegistry stub_event_registry[MAX_FRONT_ENDS];
	typedef std::map<unisim::util::debug::Hook<ADDRESS> *, HookEvent<ADDRESS> *> HookEventRegistry;
	HookEventRegistry hook_event_registry[MAX_FRONT_ENDS];
	
	int next_id[MAX_FRONT_ENDS];
	
	pthread_mutex_t schedule_mutex;
	uint64_t schedule;
	std::vector<unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *> elf32_loaders;
	std::vector<unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *> elf64_loaders;
	std::vector<unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *> coff_loaders;
	std::vector<bool> enable_elf32_loaders[MAX_FRONT_ENDS];
	std::vector<bool> enable_elf64_loaders[MAX_FRONT_ENDS];
	std::vector<bool> enable_coff_loaders[MAX_FRONT_ENDS];
	
	void SetupELFSymtab(const typename unisim::util::blob::Blob<ADDRESS> *blob);
	void SetupDWARF(const typename unisim::util::blob::Blob<ADDRESS> *blob);
	
	bool SetupDebugInfo(const unisim::util::blob::Blob<ADDRESS> *blob);
	void SetupDebugInfo();
	void RequireSetup(int);
	
	void UpdateReportingRequirements(unsigned int prc_num);
	void ScheduleFrontEnd(unsigned int front_end_num);
	bool NextScheduledFrontEnd(unsigned int& front_end_num);
	bool IsScheduleEmpty() const;
	
	bool Lock(int front_end_num = -1);
	void Unlock(int front_end_num = -1);
	
	int AllocateId(unsigned int front_end_num);
	
	virtual void VariableBaseNotify(const unisim::kernel::VariableBase *var);
	
	void CallStub(unsigned int front_end_num, unsigned int prc_num, unisim::util::debug::Stub<ADDRESS> *stub);
	void CallHook(unsigned int front_end_num, unsigned int prc_num, unisim::util::debug::Hook<ADDRESS> *hook);
};

} // end of namespace debugger
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif
