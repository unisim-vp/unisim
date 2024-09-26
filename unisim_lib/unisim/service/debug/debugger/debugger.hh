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
#include <unisim/util/debug/finish_event.hh>
#include <unisim/util/debug/trap_event.hh>
#include <unisim/util/debug/next_insn_event.hh>
#include <unisim/util/debug/fetch_stmt_event.hh>
#include <unisim/util/debug/next_stmt_event.hh>
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
#include <unisim/service/interfaces/stack_frame.hh>
#include <unisim/service/interfaces/blob.hh>
#include <unisim/service/interfaces/debug_event.hh>
#include <unisim/service/interfaces/profiling.hh>
#include <unisim/service/interfaces/debug_info_loading.hh>
#include <unisim/service/interfaces/data_object_lookup.hh>
#include <unisim/service/interfaces/subprogram_lookup.hh>
#include <unisim/service/interfaces/stubbing.hh>
#include <unisim/service/interfaces/hooking.hh>
#include <unisim/service/interfaces/debug_timing.hh>
#include <unisim/service/interfaces/debug_processors.hh>

#include <string>
#include <set>
#include <map>
#include <stdexcept>

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
	struct InternalError : std::runtime_error
	{
		InternalError(const char *msg) : std::runtime_error(std::string("Internal error: ") + msg) {}
	};
	
	typedef typename CONFIG::ADDRESS ADDRESS;
	typedef typename CONFIG::TIME_TYPE TIME_TYPE;
	static const unsigned int NUM_PROCESSORS = CONFIG::NUM_PROCESSORS;
	static const unsigned int MAX_FRONT_ENDS = CONFIG::MAX_FRONT_ENDS;
	
	// Exports to CPUs
	unisim::kernel::ServiceExport<unisim::service::interfaces::DebugYielding>                   *debug_yielding_export[NUM_PROCESSORS];
	unisim::kernel::ServiceExport<unisim::service::interfaces::MemoryAccessReporting<ADDRESS> > *memory_access_reporting_export[NUM_PROCESSORS];
	unisim::kernel::ServiceExport<unisim::service::interfaces::TrapReporting>                   *trap_reporting_export[NUM_PROCESSORS];

	// Exports to Front-ends
	unisim::kernel::ServiceExport<unisim::service::interfaces::DebugYieldingRequest>                 *debug_yielding_request_export[MAX_FRONT_ENDS];
	unisim::kernel::ServiceExport<unisim::service::interfaces::DebugSelecting>                       *debug_selecting_export[MAX_FRONT_ENDS];
	unisim::kernel::ServiceExport<unisim::service::interfaces::DebugEventTrigger<ADDRESS> >          *debug_event_trigger_export[MAX_FRONT_ENDS]; // depends on selected CPU number
	unisim::kernel::ServiceExport<unisim::service::interfaces::Disassembly<ADDRESS> >                *disasm_export[MAX_FRONT_ENDS];              // depends on selected CPU number
	unisim::kernel::ServiceExport<unisim::service::interfaces::Memory<ADDRESS> >                     *memory_export[MAX_FRONT_ENDS];              // depends on selected CPU number
	unisim::kernel::ServiceExport<unisim::service::interfaces::Registers>                            *registers_export[MAX_FRONT_ENDS];           // depends on selected CPU number
	unisim::kernel::ServiceExport<unisim::service::interfaces::SymbolTableLookup<ADDRESS> >          *symbol_table_lookup_export[MAX_FRONT_ENDS];
	unisim::kernel::ServiceExport<unisim::service::interfaces::StatementLookup<ADDRESS> >            *stmt_lookup_export[MAX_FRONT_ENDS];
	unisim::kernel::ServiceExport<unisim::service::interfaces::StackFrame<ADDRESS> >                 *stack_frame_export[MAX_FRONT_ENDS];         // depends on selected CPU number 
	unisim::kernel::ServiceExport<unisim::service::interfaces::DebugInfoLoading>                     *debug_info_loading_export[MAX_FRONT_ENDS];
	unisim::kernel::ServiceExport<unisim::service::interfaces::DataObjectLookup<ADDRESS> >           *data_object_lookup_export[MAX_FRONT_ENDS];  // depends on selected CPU number
	unisim::kernel::ServiceExport<unisim::service::interfaces::SubProgramLookup<ADDRESS> >           *subprogram_lookup_export[MAX_FRONT_ENDS];   // depends on selected CPU number
	unisim::kernel::ServiceExport<unisim::service::interfaces::Stubbing<ADDRESS> >                   *stubbing_export[MAX_FRONT_ENDS];            // depends on selected CPU number
	unisim::kernel::ServiceExport<unisim::service::interfaces::Hooking<ADDRESS> >                    *hooking_export[MAX_FRONT_ENDS];             // depends on selected CPU number
	unisim::kernel::ServiceExport<unisim::service::interfaces::DebugTiming<TIME_TYPE> >              *debug_timing_export[MAX_FRONT_ENDS];        // depends on selected CPU number
	unisim::kernel::ServiceExport<unisim::service::interfaces::DebugProcessors<ADDRESS, TIME_TYPE> > *debug_processors_export[MAX_FRONT_ENDS];

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
	void ScanSymbols(unisim::service::interfaces::SymbolTableScanner<ADDRESS>& scanner) const { ScanSymbols(MAX_FRONT_ENDS, scanner); }
	void ScanSymbols(unisim::service::interfaces::SymbolTableScanner<ADDRESS>& scanner, typename unisim::util::debug::SymbolBase::Type type) const { ScanSymbols(MAX_FRONT_ENDS, scanner, type); }
	const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbolByAddr(ADDRESS addr) const { return FindSymbolByAddr(MAX_FRONT_ENDS, addr); }
	const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbolByName(const char *name) const { return FindSymbolByName(MAX_FRONT_ENDS, name); }
	const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbolByName(const char *name, typename unisim::util::debug::SymbolBase::Type type) const { return FindSymbolByName(MAX_FRONT_ENDS, name, type); }
	const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbolByAddr(ADDRESS addr, typename unisim::util::debug::SymbolBase::Type type) const { return FindSymbolByAddr(MAX_FRONT_ENDS, addr, type); }

	// Statements
	void ScanStatements(unisim::service::interfaces::StatementScanner<ADDRESS>& scanner, const char *filename) const { ScanStatements(MAX_FRONT_ENDS, scanner, filename); }
	const unisim::util::debug::Statement<ADDRESS> *FindStatement(ADDRESS addr, const char *filename, typename unisim::service::interfaces::StatementLookup<ADDRESS>::Scope scope) const { return FindStatement(MAX_FRONT_ENDS, addr, filename, scope); }
	const unisim::util::debug::Statement<ADDRESS> *FindStatements(unisim::service::interfaces::StatementScanner<ADDRESS>& scanner, ADDRESS addr, const char *filename, typename unisim::service::interfaces::StatementLookup<ADDRESS>::Scope scope) const { return FindStatements(MAX_FRONT_ENDS, scanner, addr, filename, scope); }
	const unisim::util::debug::Statement<ADDRESS> *FindStatement(const unisim::util::debug::SourceCodeLocation& source_code_location, const char *filename) const { return FindStatement(MAX_FRONT_ENDS, source_code_location, filename); }
	const unisim::util::debug::Statement<ADDRESS> *FindStatements(unisim::service::interfaces::StatementScanner<ADDRESS>& scanner, const unisim::util::debug::SourceCodeLocation& source_code_location, const char *filename) const { return FindStatements(MAX_FRONT_ENDS, scanner, source_code_location, filename); }
	void ScanStatements(unisim::service::interfaces::StatementScanner<ADDRESS>& scanner, const std::string& filename) const { ScanStatements(MAX_FRONT_ENDS, scanner, filename.length() ? filename.c_str() : 0); }
	const unisim::util::debug::Statement<ADDRESS> *FindStatement(ADDRESS addr, const std::string& filename, typename unisim::service::interfaces::StatementLookup<ADDRESS>::Scope scope) const { return FindStatement(MAX_FRONT_ENDS, addr, filename.length() ? filename.c_str() : 0, scope); }
	const unisim::util::debug::Statement<ADDRESS> *FindStatements(unisim::service::interfaces::StatementScanner<ADDRESS>& scanner, ADDRESS addr, const std::string& filename, typename unisim::service::interfaces::StatementLookup<ADDRESS>::Scope scope) const { return FindStatements(MAX_FRONT_ENDS, scanner, addr, filename.length() ? filename.c_str() : 0, scope); }
	const unisim::util::debug::Statement<ADDRESS> *FindStatement(const unisim::util::debug::SourceCodeLocation& source_code_location, const std::string& filename) const { return FindStatement(MAX_FRONT_ENDS, source_code_location, filename.length() ? filename.c_str() : 0); }
	const unisim::util::debug::Statement<ADDRESS> *FindStatements(unisim::service::interfaces::StatementScanner<ADDRESS>& scanner, const unisim::util::debug::SourceCodeLocation& source_code_location, const std::string& filename) const { return FindStatements(MAX_FRONT_ENDS, scanner, source_code_location, filename.length() ? filename.c_str() : 0); }
	
private:
	// Exports to CPUs
	
	// unisim::service::interfaces::DebugYielding (tagged)
	void DebugYield(unsigned int prc_num);
	
	// unisim::service::interfaces::MemoryAccessReporting<ADDRESS> (tagged)
	bool ReportMemoryAccess(unsigned int prc_num, unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size);
	void ReportCommitInstruction(unsigned int prc_num, ADDRESS addr, unsigned int length);
	void ReportFetchInstruction(unsigned int prc_num, ADDRESS next_addr);
	
	// unisim::service::interfaces::TrapReporting (tagged)
	void ReportTrap(unsigned int prc_num, const unisim::kernel::Object *obj = 0, const std::string& msg = std::string());

	// Export to Front-ends
	
	// unisim::service::interfaces::DebugYieldingRequest (tagged)
	void DebugYieldRequest(unsigned int front_end_num);

	// unisim::service::interfaces::DebugSelecting (tagged)
	bool SelectProcessor(unsigned int front_end_num, unsigned int prc_num);
	unsigned int GetSelectedProcessor(unsigned int front_end_num) const;

	// unisim::service::interfaces::DebugEventFactory<ADDRESS> (tagged)
	unisim::util::debug::Breakpoint<ADDRESS> *CreateBreakpoint(unsigned int front_end_num, ADDRESS addr, bool internal);
	unisim::util::debug::Breakpoint<ADDRESS> *CreateBreakpoint(unsigned int front_end_num, unsigned int prc_num, ADDRESS addr, bool internal);
	unisim::util::debug::Watchpoint<ADDRESS> *CreateWatchpoint(unsigned int front_end_num, unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size, bool overlook, bool internal);
	unisim::util::debug::Watchpoint<ADDRESS> *CreateWatchpoint(unsigned int front_end_num, unsigned int prc_num, unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size, bool overlook, bool internal);
	unisim::util::debug::FetchInsnEvent<ADDRESS> *CreateFetchInsnEvent(unsigned int front_end_num, bool internal);
	unisim::util::debug::FetchInsnEvent<ADDRESS> *CreateFetchInsnEvent(unsigned int front_end_num, unsigned int prc_num, bool internal);
	unisim::util::debug::FetchStmtEvent<ADDRESS> *CreateFetchStmtEvent(unsigned int front_end_num, bool internal);
	unisim::util::debug::FetchStmtEvent<ADDRESS> *CreateFetchStmtEvent(unsigned int front_end_num, unsigned int prc_num, bool internal);
	unisim::util::debug::CommitInsnEvent<ADDRESS> *CreateCommitInsnEvent(unsigned int front_end_num, bool internal);
	unisim::util::debug::CommitInsnEvent<ADDRESS> *CreateCommitInsnEvent(unsigned int front_end_num, unsigned int prc_num, bool internal);
	unisim::util::debug::NextInsnEvent<ADDRESS> *CreateNextInsnEvent(unsigned int front_end_num, bool internal);
	unisim::util::debug::NextInsnEvent<ADDRESS> *CreateNextInsnEvent(unsigned int front_end_num, unsigned int prc_num, bool internal);
	unisim::util::debug::NextStmtEvent<ADDRESS> *CreateNextStmtEvent(unsigned int front_end_num, bool internal);
	unisim::util::debug::NextStmtEvent<ADDRESS> *CreateNextStmtEvent(unsigned int front_end_num, unsigned int prc_num, bool internal);
	unisim::util::debug::FinishEvent<ADDRESS> *CreateFinishEvent(unsigned int front_end_num, bool internal);
	unisim::util::debug::FinishEvent<ADDRESS> *CreateFinishEvent(unsigned int front_end_num, unsigned int prc_num, bool internal);
	unisim::util::debug::TrapEvent<ADDRESS> *CreateTrapEvent(unsigned int front_end_num, bool internal);
	unisim::util::debug::TrapEvent<ADDRESS> *CreateTrapEvent(unsigned int front_end_num, unsigned int prc_num, bool internal);
	unisim::util::debug::SourceCodeBreakpoint<ADDRESS> *CreateSourceCodeBreakpoint(unsigned int front_end_num, const unisim::util::debug::SourceCodeLocation& source_code_location, const std::string& filename, bool internal);
	unisim::util::debug::SourceCodeBreakpoint<ADDRESS> *CreateSourceCodeBreakpoint(unsigned int front_end_num, unsigned int prc_num, const unisim::util::debug::SourceCodeLocation& source_code_location, const std::string& filename, bool internal);
	unisim::util::debug::SubProgramBreakpoint<ADDRESS> *CreateSubProgramBreakpoint(unsigned int front_end_num, const unisim::util::debug::SubProgram<ADDRESS> *subprogram, bool internal);
	unisim::util::debug::SubProgramBreakpoint<ADDRESS> *CreateSubProgramBreakpoint(unsigned int front_end_num, unsigned int prc_num, const unisim::util::debug::SubProgram<ADDRESS> *subprogram, bool internal);
	
	// unisim::service::interfaces::DebugEventTrigger<ADDRESS> (tagged)
	
	// "named" events
	bool Listen(unsigned int front_end_num, unisim::util::debug::Event<ADDRESS> *event);
	template <typename EVENT_SET, typename EVENT> bool AddEvent(unsigned int front_end_num, EVENT_SET (&event_set)[], unisim::util::debug::Event<ADDRESS> *event);
	template <typename EVENT_SET, typename EVENT> bool RemoveEvent(unsigned int front_end_num, EVENT_SET (&event_set)[], unisim::util::debug::Event<ADDRESS> *event);
	bool Unlisten(unsigned int front_end_num, unisim::util::debug::Event<ADDRESS> *event);
	bool IsEventListened(unsigned int front_end_num, unisim::util::debug::Event<ADDRESS> *event) const;
	template <typename EVENT_SET, typename EVENT, typename SCANNER> void ScanEvents(unsigned int front_end_num, const EVENT_SET (&event_set)[], SCANNER& scanner) const;
	void ScanListenedEvents(unsigned int front_end_num, unisim::service::interfaces::DebugEventScanner<ADDRESS>& scanner) const;
	void ClearEvents(unsigned int front_end_num);
	
	// idem potent interface: anonymous events
	bool SetBreakpoint(unsigned int front_end_num, ADDRESS addr);
	bool RemoveBreakpoint(unsigned int front_end_num, ADDRESS addr);
	bool HasBreakpoints(unsigned int front_end_num, ADDRESS addr);
	bool SetWatchpoint(unsigned int front_end_num, unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size, bool overlook);
	bool RemoveWatchpoint(unsigned int front_end_num, unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size);
	bool HasWatchpoints(unsigned int front_end_num, unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size);
	
	// unisim::service::interfaces::Disassembly<ADDRESS> (tagged)
	std::string Disasm(unsigned int front_end_num, ADDRESS addr, ADDRESS& next_addr);
	std::string Disasm(unsigned int front_end_num, unsigned int prc_num, ADDRESS addr, ADDRESS& next_addr);
	
	// unisim::service::interfaces::Memory<ADDRESS> (tagged)
	void ResetMemory(unsigned int front_end_num);
	void ResetMemory(unsigned int front_end_num, unsigned int prc_num);
	bool ReadMemory(unsigned int front_end_num, ADDRESS addr, void *buffer, uint32_t size);
	bool ReadMemory(unsigned int front_end_num, unsigned int prc_num, ADDRESS addr, void *buffer, uint32_t size);
	bool WriteMemory(unsigned int front_end_num, ADDRESS addr, const void *buffer, uint32_t size);
	bool WriteMemory(unsigned int front_end_num, unsigned int prc_num, ADDRESS addr, const void *buffer, uint32_t size);
	
	// unisim::service::interfaces::Registers (tagged)
	unisim::service::interfaces::Register *GetRegister(unsigned int front_end_num, const char *name);
	unisim::service::interfaces::Register *GetRegister(unsigned int front_end_num, unsigned int prc_num, const char *name);
	void ScanRegisters(unsigned int front_end_num, unisim::service::interfaces::RegisterScanner& scanner);
	void ScanRegisters(unsigned int front_end_num, unsigned int prc_num, unisim::service::interfaces::RegisterScanner& scanner);
	
	// unisim::service::interfaces::SymbolTableLookup<ADDRESS> (tagged)
	void ScanSymbols(unsigned int front_end_num, unisim::service::interfaces::SymbolTableScanner<ADDRESS>& scanner) const;
	void ScanSymbols(unsigned int front_end_num, unisim::service::interfaces::SymbolTableScanner<ADDRESS>& scanner, typename unisim::util::debug::SymbolBase::Type type) const;
	const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbolByAddr(unsigned int front_end_num, ADDRESS addr) const;
	const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbolByName(unsigned int front_end_num, const char *name) const;
	const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbolByName(unsigned int front_end_num, const char *name, typename unisim::util::debug::SymbolBase::Type type) const;
	const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbolByAddr(unsigned int front_end_num, ADDRESS addr, typename unisim::util::debug::SymbolBase::Type type) const;
	
	// unisim::service::interfaces::StatementLookup<ADDRESS> (tagged)
	void ScanStatements(unsigned int front_end_num, unisim::service::interfaces::StatementScanner<ADDRESS>& scanner, const char *filename) const;
	const unisim::util::debug::Statement<ADDRESS> *FindStatement(unsigned int front_end_num, ADDRESS addr, const char *filename, typename unisim::service::interfaces::StatementLookup<ADDRESS>::Scope scope) const;
	const unisim::util::debug::Statement<ADDRESS> *FindStatements(unsigned int front_end_num, unisim::service::interfaces::StatementScanner<ADDRESS>& scanner, ADDRESS addr, const char *filename, typename unisim::service::interfaces::StatementLookup<ADDRESS>::Scope scope) const;
	const unisim::util::debug::Statement<ADDRESS> *FindStatement(unsigned int front_end_num, const unisim::util::debug::SourceCodeLocation& source_code_location, const char *filename) const;
	const unisim::util::debug::Statement<ADDRESS> *FindStatements(unsigned int front_end_num, unisim::service::interfaces::StatementScanner<ADDRESS>& scanner, const unisim::util::debug::SourceCodeLocation& source_code_location, const char *filename) const;
	void ScanStatements(unsigned int front_end_num, unisim::service::interfaces::StatementScanner<ADDRESS>& scanner, const std::string& filename) const { ScanStatements(front_end_num, scanner, filename.length() ? filename.c_str() : 0); }
	const unisim::util::debug::Statement<ADDRESS> *FindStatement(unsigned int front_end_num, ADDRESS addr, const std::string& filename, typename unisim::service::interfaces::StatementLookup<ADDRESS>::Scope scope) const { return FindStatement(front_end_num, addr, filename.length() ? filename.c_str() : 0, scope); }
	const unisim::util::debug::Statement<ADDRESS> *FindStatements(unsigned int front_end_num, unisim::service::interfaces::StatementScanner<ADDRESS>& scanner, ADDRESS addr, const std::string& filename, typename unisim::service::interfaces::StatementLookup<ADDRESS>::Scope scope) const { return FindStatements(front_end_num, scanner, addr, filename.length() ? filename.c_str() : 0, scope); }
	const unisim::util::debug::Statement<ADDRESS> *FindStatement(unsigned int front_end_num, const unisim::util::debug::SourceCodeLocation& source_code_location, const std::string& filename) const { return FindStatement(front_end_num, source_code_location, filename.length() ? filename.c_str() : 0); }
	const unisim::util::debug::Statement<ADDRESS> *FindStatements(unsigned int front_end_num, unisim::service::interfaces::StatementScanner<ADDRESS>& scanner, const unisim::util::debug::SourceCodeLocation& source_code_location, const std::string& filename) const { return FindStatements(front_end_num, scanner, source_code_location, filename.length() ? filename.c_str() : 0); }
	
	// unisim::service::interfaces::StackFrame<ADDRESS> (tagged)
	bool SelectStackFrame(unsigned int front_end_num, unsigned int frame_num);
	bool SelectStackFrame(unsigned int front_end_num, unsigned int prc_num, unsigned int frame_num);
	unsigned int GetSelectedStackFrameNumber(unsigned int front_end_num) const;
	unsigned int GetSelectedStackFrameNumber(unsigned int front_end_num, unsigned int prc_num) const;
	std::vector<ADDRESS> *GetBackTrace(unsigned int front_end_num) const;
	std::vector<ADDRESS> *GetBackTrace(unsigned int front_end_num, unsigned int prc_num) const;
	bool GetReturnAddress(unsigned int front_end_num, ADDRESS& ret_addr) const;
	bool GetReturnAddress(unsigned int front_end_num, unsigned int prc_num, ADDRESS& ret_addr) const;
	bool UnwindStack(unsigned int front_end_num, unsigned int frame_num);
	bool UnwindStack(unsigned int front_end_num, unsigned int prc_num, unsigned int frame_num);
	void ScanStackFrameInfos(unsigned int front_end_num, unisim::service::interfaces::StackFrameInfoScanner<ADDRESS>& scanner, unsigned int max_stack_frames) const;
	void ScanStackFrameInfos(unsigned int front_end_num, unsigned int prc_num, unisim::service::interfaces::StackFrameInfoScanner<ADDRESS>& scanner, unsigned int max_stack_frames) const;
	unsigned int GetStackFrameInfos(unsigned int front_end_num, unisim::service::interfaces::StackFrameInfo<ADDRESS> *stack_frame_infos, unsigned int max_stack_frames) const;
	unsigned int GetStackFrameInfos(unsigned int front_end_num, unsigned int prc_num, unisim::service::interfaces::StackFrameInfo<ADDRESS> *stack_frame_infos, unsigned int max_stack_frames) const;
	
	// unisim::service::interfaces::DebugInfoLoading (tagged)
	bool LoadDebugInfo(unsigned int front_end_num, const char *filename);
	void ScanExecutableBinaryFiles(unsigned int front_end_num, unisim::service::interfaces::ExecutableBinaryFileScanner& scanner) const;
	
	// unisim::service::interfaces::DataObjectLookup<ADDRESS> (tagged)
	unisim::util::debug::DataObjectRef<ADDRESS> FindDataObject(unsigned int front_end_num, const char *data_object_name) const;
	unisim::util::debug::DataObjectRef<ADDRESS> FindDataObject(unsigned int front_end_num, unsigned int prc_num, const char *data_object_name) const;
	unisim::util::debug::DataObjectRef<ADDRESS> GetReturnValue(unsigned int front_end_num) const;
	unisim::util::debug::DataObjectRef<ADDRESS> GetReturnValue(unsigned int front_end_num, unsigned int prc_num) const;
	unisim::util::debug::DataObjectRef<ADDRESS> GetSubProgramParameter(unsigned int front_end_num, unsigned int index) const;
	unisim::util::debug::DataObjectRef<ADDRESS> GetSubProgramParameter(unsigned int front_end_num, unsigned int prc_num, unsigned int index) const;
	void ScanDataObjectNames(unsigned int front_end_num, unisim::service::interfaces::DataObjectNameScanner& scanner, typename unisim::service::interfaces::DataObjectLookupBase::Scope scope) const;
	void ScanDataObjectNames(unsigned int front_end_num, unsigned int prc_num, unisim::service::interfaces::DataObjectNameScanner& scanner, typename unisim::service::interfaces::DataObjectLookupBase::Scope scope) const;
	
	// unisim::service::interfaces::SubProgramLookup<ADDRESS> (tagged)
	const unisim::util::debug::SubProgram<ADDRESS> *FindSubProgram(unsigned int front_end_num, const char *subprogram_name, const char *filename = 0, const char *compilation_unit_name = 0) const;
	const unisim::util::debug::SubProgram<ADDRESS> *FindSubProgram(unsigned int front_end_num, ADDRESS pc, const char *filename = 0) const;
	
	// unisim::service::interfaces::Stubbing<ADDRESS> (tagged)
	void ScanStubs(unsigned int front_end_num, unisim::service::interfaces::StubScanner<ADDRESS>& scanner) const;
	void ScanStubs(unsigned int front_end_num, unsigned int prc_num, unisim::service::interfaces::StubScanner<ADDRESS>& scanner) const;
	bool SetStub(unsigned int front_end_num, unisim::util::debug::Stub<ADDRESS> *stub);
	bool SetStub(unsigned int front_end_num, unsigned int prc_num, unisim::util::debug::Stub<ADDRESS> *stub);
	bool RemoveStub(unsigned int front_end_num, unisim::util::debug::Stub<ADDRESS> *stub);
	bool RemoveStub(unsigned int front_end_num, unsigned int prc_num, unisim::util::debug::Stub<ADDRESS> *stub);
	
	// unisim::service::interfaces::Hooking<ADDRESS> (tagged)
	void ScanHooks(unsigned int front_end_num, unisim::service::interfaces::HookScanner<ADDRESS>& scanner) const;
	void ScanHooks(unsigned int front_end_num, unsigned int prc_num, unisim::service::interfaces::HookScanner<ADDRESS>& scanner) const;
	bool SetHook(unsigned int front_end_num, unisim::util::debug::Hook<ADDRESS> *hook);
	bool SetHook(unsigned int front_end_num, unsigned int prc_num, unisim::util::debug::Hook<ADDRESS> *hook);
	bool RemoveHook(unsigned int front_end_num, unisim::util::debug::Hook<ADDRESS> *hook);
	bool RemoveHook(unsigned int front_end_num, unsigned int prc_num, unisim::util::debug::Hook<ADDRESS> *hook);

	// unisim::service::interfaces::DebugTiming<TIME_TYPE> (tagged)
	const TIME_TYPE& DebugGetTime(unsigned int front_end_num) const;
	const TIME_TYPE& DebugGetTime(unsigned int front_end_num, unsigned int prc_num) const;
	
	// unisim::service::interfaces::DebugProcessors<ADDRESS, TIME_TYPE> (tagged)
	unsigned int DebugGetProcessorCount(unsigned int front_end_num) const;
	unisim::service::interfaces::DebugProcessor<ADDRESS, TIME_TYPE> *DebugGetProcessor(unsigned int front_end_num, unsigned int prc_num) const;
	
	struct Processor : unisim::service::interfaces::DebugProcessor<ADDRESS, TIME_TYPE>
	{
		Processor(
			Debugger<CONFIG>& _dbg,
			unsigned int _front_end_num,
			unsigned int _prc_num)
			: dbg(_dbg)
			, front_end_num(_front_end_num)
			, prc_num(_prc_num)
		{
		}
		
		// unisim::service::interfaces::DebugProcessor<ADDRESS, TIME_TYPE>
		virtual unsigned int GetProcessorNumber() const { return prc_num; }
		
		// unisim::service::interfaces::Disassembly<ADDRESS>
		virtual std::string Disasm(ADDRESS addr, ADDRESS& next_addr) { bool l = dbg.Lock(front_end_num); std::string ret = dbg.Disasm(front_end_num, prc_num, addr, next_addr); if(l) { dbg.Unlock(front_end_num); } return ret; }
		
		// unisim::service::interfaces::Memory<ADDRESS>
		virtual void ResetMemory() { bool l = dbg.Lock(front_end_num); dbg.ResetMemory(front_end_num, prc_num); if(l) { dbg.Unlock(front_end_num); } }
		virtual bool ReadMemory(ADDRESS addr, void *buffer, uint32_t size) { bool l = dbg.Lock(front_end_num); bool ret = dbg.ReadMemory(front_end_num, prc_num, addr, buffer, size); if(l) { dbg.Unlock(front_end_num); } return ret; }
		virtual bool WriteMemory(ADDRESS addr, const void *buffer, uint32_t size) { bool l = dbg.Lock(front_end_num); bool ret = dbg.WriteMemory(front_end_num, prc_num, addr, buffer, size); if(l) { dbg.Unlock(front_end_num); } return ret; }
		
		// unisim::service::interfaces::DebugTiming<TIME_TYPE>
		virtual const TIME_TYPE& DebugGetTime() const { bool l = dbg.Lock(front_end_num); const TIME_TYPE& ret = dbg.DebugGetTime(front_end_num, prc_num); if(l) { dbg.Unlock(front_end_num); } return ret; }

		// unisim::service::interfaces::Stubbing<ADDRESS>
		virtual void ScanStubs(unisim::service::interfaces::StubScanner<ADDRESS>& scanner) const { bool l = dbg.Lock(front_end_num); dbg.ScanStubs(front_end_num, prc_num, scanner); if(l) { dbg.Unlock(front_end_num); } }
		virtual bool SetStub(unisim::util::debug::Stub<ADDRESS> *stub) { bool l = dbg.Lock(front_end_num); bool ret = dbg.SetStub(front_end_num, prc_num, stub); if(l) { dbg.Unlock(front_end_num); } return ret; }
		virtual bool RemoveStub(unisim::util::debug::Stub<ADDRESS> *stub) { bool l = dbg.Lock(front_end_num); bool ret = dbg.RemoveStub(front_end_num, prc_num, stub); if(l) { dbg.Unlock(front_end_num); } return ret; }
		
		// unisim::service::interfaces::Hooking<ADDRESS>
		virtual void ScanHooks(unisim::service::interfaces::HookScanner<ADDRESS>& scanner) const { bool l = dbg.Lock(front_end_num); dbg.ScanHooks(front_end_num, prc_num, scanner); if(l) { dbg.Unlock(front_end_num); } } 
		virtual bool SetHook(unisim::util::debug::Hook<ADDRESS> *hook) { bool l = dbg.Lock(front_end_num); bool ret = dbg.SetHook(front_end_num, prc_num, hook); if(l) { dbg.Unlock(front_end_num); } return ret; }
		virtual bool RemoveHook(unisim::util::debug::Hook<ADDRESS> *hook) { bool l = dbg.Lock(front_end_num); bool ret = dbg.RemoveHook(front_end_num, prc_num, hook); if(l) { dbg.Unlock(front_end_num); } return ret; }
		
		// unisim::service::interfaces::DebugEventFactory<ADDRESS>
		virtual unisim::util::debug::Breakpoint<ADDRESS> *CreateBreakpoint(ADDRESS addr) { bool l = dbg.Lock(front_end_num); unisim::util::debug::Breakpoint<ADDRESS> *ret = dbg.CreateBreakpoint(front_end_num, prc_num, addr, false); if(l) { dbg.Unlock(front_end_num); } return ret; }
		virtual unisim::util::debug::Watchpoint<ADDRESS> *CreateWatchpoint(unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size, bool overlook) { bool l = dbg.Lock(front_end_num); unisim::util::debug::Watchpoint<ADDRESS> *ret = dbg.CreateWatchpoint(front_end_num, prc_num, mat, mt, addr, size, overlook, false); if(l) { dbg.Unlock(front_end_num); } return ret; }
		virtual unisim::util::debug::FetchInsnEvent<ADDRESS> *CreateFetchInsnEvent() { bool l = dbg.Lock(front_end_num); unisim::util::debug::FetchInsnEvent<ADDRESS> *ret = dbg.CreateFetchInsnEvent(front_end_num, prc_num, false); if(l) { dbg.Unlock(front_end_num); } return ret; }
		virtual unisim::util::debug::FetchStmtEvent<ADDRESS> *CreateFetchStmtEvent() { bool l = dbg.Lock(front_end_num); unisim::util::debug::FetchStmtEvent<ADDRESS> *ret = dbg.CreateFetchStmtEvent(front_end_num, prc_num, false); if(l) { dbg.Unlock(front_end_num); } return ret; }
		virtual unisim::util::debug::CommitInsnEvent<ADDRESS> *CreateCommitInsnEvent() { bool l = dbg.Lock(front_end_num); unisim::util::debug::CommitInsnEvent<ADDRESS> *ret = dbg.CreateCommitInsnEvent(front_end_num, prc_num, false); if(l) { dbg.Unlock(front_end_num); } return ret; }
		virtual unisim::util::debug::NextInsnEvent<ADDRESS> *CreateNextInsnEvent() { bool l = dbg.Lock(front_end_num); unisim::util::debug::NextInsnEvent<ADDRESS> *ret = dbg.CreateNextInsnEvent(front_end_num, prc_num, false); if(l) { dbg.Unlock(front_end_num); } return ret; }
		virtual unisim::util::debug::NextStmtEvent<ADDRESS> *CreateNextStmtEvent() { bool l = dbg.Lock(front_end_num); unisim::util::debug::NextStmtEvent<ADDRESS> *ret = dbg.CreateNextStmtEvent(front_end_num, prc_num, false); if(l) { dbg.Unlock(front_end_num); } return ret; }
		virtual unisim::util::debug::FinishEvent<ADDRESS> *CreateFinishEvent() { bool l = dbg.Lock(front_end_num); unisim::util::debug::FinishEvent<ADDRESS> *ret = dbg.CreateFinishEvent(front_end_num, prc_num, false); if(l) { dbg.Unlock(front_end_num); } return ret; }
		virtual unisim::util::debug::TrapEvent<ADDRESS> *CreateTrapEvent() { bool l = dbg.Lock(front_end_num); unisim::util::debug::TrapEvent<ADDRESS> *ret = dbg.CreateTrapEvent(front_end_num, prc_num, false); if(l) { dbg.Unlock(front_end_num); } return ret; }
		virtual unisim::util::debug::SourceCodeBreakpoint<ADDRESS> *CreateSourceCodeBreakpoint(const unisim::util::debug::SourceCodeLocation& source_code_location, const std::string& filename) { bool l = dbg.Lock(front_end_num); unisim::util::debug::SourceCodeBreakpoint<ADDRESS> *ret = dbg.CreateSourceCodeBreakpoint(front_end_num, prc_num, source_code_location, filename, false); if(l) { dbg.Unlock(front_end_num); } return ret; }
		virtual unisim::util::debug::SubProgramBreakpoint<ADDRESS> *CreateSubProgramBreakpoint(const unisim::util::debug::SubProgram<ADDRESS> *subprogram) { bool l = dbg.Lock(front_end_num); unisim::util::debug::SubProgramBreakpoint<ADDRESS> *ret = dbg.CreateSubProgramBreakpoint(front_end_num, prc_num, subprogram, false); if(l) { dbg.Unlock(front_end_num); } return ret; }
		
		// unisim::service::interfaces::Registers
		virtual unisim::service::interfaces::Register *GetRegister(const char *name) { bool l = dbg.Lock(front_end_num); unisim::service::interfaces::Register *ret = dbg.GetRegister(front_end_num, prc_num, name); if(l) { dbg.Unlock(front_end_num); } return ret; }
		virtual void ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner) { bool l = dbg.Lock(front_end_num); dbg.ScanRegisters(front_end_num, prc_num, scanner); if(l) { dbg.Unlock(front_end_num); } }
		
		// unisim::service::interfaces::StackFrame<ADDRESS>
		virtual bool SelectStackFrame(unsigned int frame_num) { /*dbg.Lock();*/ bool ret = dbg.SelectStackFrame(front_end_num, prc_num, frame_num); /*dbg.Unlock();*/ return ret; }
		virtual unsigned int GetSelectedStackFrameNumber() const { /*dbg.Lock();*/ unsigned int ret = dbg.GetSelectedStackFrameNumber(front_end_num, prc_num); /*dbg.Unlock();*/ return ret; }
		virtual std::vector<ADDRESS> *GetBackTrace() const { bool l = dbg.Lock(front_end_num); std::vector<ADDRESS> *ret = dbg.GetBackTrace(front_end_num, prc_num); if(l) { dbg.Unlock(front_end_num); } return ret; }
		virtual bool GetReturnAddress(ADDRESS& ret_addr) const { bool l = dbg.Lock(front_end_num); bool ret = dbg.GetReturnAddress(front_end_num, prc_num, ret_addr); if(l) { dbg.Unlock(front_end_num); } return ret; }
		virtual bool UnwindStack(unsigned int frame_num) { bool l = dbg.Lock(front_end_num); bool ret = dbg.UnwindStack(front_end_num, prc_num, frame_num); if(l) { dbg.Unlock(front_end_num); } return ret; }
		virtual void ScanStackFrameInfos(unisim::service::interfaces::StackFrameInfoScanner<ADDRESS>& scanner, unsigned int max_stack_frames) const { bool l = dbg.Lock(front_end_num); dbg.ScanStackFrameInfos(front_end_num, prc_num, scanner, max_stack_frames); if(l) { dbg.Unlock(front_end_num); } }
		virtual unsigned int GetStackFrameInfos(unisim::service::interfaces::StackFrameInfo<ADDRESS> *stack_frame_infos, unsigned int max_stack_frames) const { bool l = dbg.Lock(front_end_num); unsigned int ret = dbg.GetStackFrameInfos(front_end_num, prc_num, stack_frame_infos, max_stack_frames); if(l) { dbg.Unlock(front_end_num); } return ret; }
		
		// unisim::service::interfaces::DataObjectLookup<ADDRESS>
		virtual unisim::util::debug::DataObjectRef<ADDRESS> FindDataObject(const char *data_object_name) const { bool l = dbg.Lock(front_end_num); unisim::util::debug::DataObjectRef<ADDRESS> ret = dbg.FindDataObject(front_end_num, prc_num, data_object_name); if(l) { dbg.Unlock(front_end_num); } return ret; }
		virtual unisim::util::debug::DataObjectRef<ADDRESS> GetReturnValue() const { bool l = dbg.Lock(front_end_num); unisim::util::debug::DataObjectRef<ADDRESS> ret = dbg.GetReturnValue(front_end_num, prc_num); if(l) { dbg.Unlock(front_end_num); } return ret; }
		virtual unisim::util::debug::DataObjectRef<ADDRESS> GetSubProgramParameter(unsigned int index) const { bool l = dbg.Lock(front_end_num); unisim::util::debug::DataObjectRef<ADDRESS> ret = dbg.GetSubProgramParameter(front_end_num, prc_num, index); if(l) { dbg.Unlock(front_end_num); } return ret; }
		virtual void ScanDataObjectNames(unisim::service::interfaces::DataObjectNameScanner& scanner, typename unisim::service::interfaces::DataObjectLookupBase::Scope scope) const { bool l = dbg.Lock(front_end_num); dbg.ScanDataObjectNames(front_end_num, prc_num, scanner, scope); if(l) { dbg.Unlock(front_end_num); } } 
	private:
		Debugger<CONFIG>& dbg;
		unsigned int front_end_num;
		unsigned int prc_num;
	};
	
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
		virtual void ReportTrap(const unisim::kernel::Object &obj) { dbg.ReportTrap(id, &obj); }
		virtual void ReportTrap(const unisim::kernel::Object &obj, const std::string &str) { dbg.ReportTrap(id, &obj, str); }
		virtual void ReportTrap(const unisim::kernel::Object &obj, const char *c_str) { dbg.ReportTrap(id, &obj, c_str); }
		
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
		inline const TIME_TYPE& DebugGetTime() const { static TIME_TYPE t; return debug_timing_import ? debug_timing_import->DebugGetTime() : t; }
		
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
		, unisim::kernel::Service<unisim::service::interfaces::StackFrame<ADDRESS> >
		, unisim::kernel::Service<unisim::service::interfaces::DebugInfoLoading>
		, unisim::kernel::Service<unisim::service::interfaces::DataObjectLookup<ADDRESS> >
		, unisim::kernel::Service<unisim::service::interfaces::SubProgramLookup<ADDRESS> >
		, unisim::kernel::Service<unisim::service::interfaces::Stubbing<ADDRESS> >
		, unisim::kernel::Service<unisim::service::interfaces::Hooking<ADDRESS> >
		, unisim::kernel::Service<unisim::service::interfaces::DebugTiming<TIME_TYPE> >
		, unisim::kernel::Service<unisim::service::interfaces::DebugProcessors<ADDRESS, TIME_TYPE> >
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
		unisim::kernel::ServiceExport<unisim::service::interfaces::StackFrame<ADDRESS> > stack_frame_export;                 // depends on selected CPU number 
		unisim::kernel::ServiceExport<unisim::service::interfaces::DebugInfoLoading> debug_info_loading_export;
		unisim::kernel::ServiceExport<unisim::service::interfaces::DataObjectLookup<ADDRESS> > data_object_lookup_export; // depends on selected CPU number
		unisim::kernel::ServiceExport<unisim::service::interfaces::SubProgramLookup<ADDRESS> > subprogram_lookup_export; // depends on selected CPU number
		unisim::kernel::ServiceExport<unisim::service::interfaces::Stubbing<ADDRESS> > stubbing_export;                  // depends on selected CPU number
		unisim::kernel::ServiceExport<unisim::service::interfaces::Hooking<ADDRESS> > hooking_export;                    // depends on selected CPU number
		unisim::kernel::ServiceExport<unisim::service::interfaces::DebugTiming<TIME_TYPE> > debug_timing_export;         // depends on selected CPU number
		unisim::kernel::ServiceExport<unisim::service::interfaces::DebugProcessors<ADDRESS, TIME_TYPE> > debug_processors_export;
		
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
			, unisim::kernel::Service<unisim::service::interfaces::StackFrame<ADDRESS> >(name, parent)
			, unisim::kernel::Service<unisim::service::interfaces::DebugInfoLoading>(name, parent)
			, unisim::kernel::Service<unisim::service::interfaces::DataObjectLookup<ADDRESS> >(name, parent)
			, unisim::kernel::Service<unisim::service::interfaces::SubProgramLookup<ADDRESS> >(name, parent)
			, unisim::kernel::Service<unisim::service::interfaces::Stubbing<ADDRESS> >(name, parent)
			, unisim::kernel::Service<unisim::service::interfaces::Hooking<ADDRESS> >(name, parent)
			, unisim::kernel::Service<unisim::service::interfaces::DebugTiming<TIME_TYPE> >(name, parent)
			, unisim::kernel::Service<unisim::service::interfaces::DebugProcessors<ADDRESS, TIME_TYPE> >(name, parent)
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
			, stack_frame_export("backtrace-export", this)
			, debug_info_loading_export("debug-info-loading-export", this)
			, data_object_lookup_export("data-object-lookup-export", this)
			, subprogram_lookup_export("subprogram-lookup-export", this)
			, stubbing_export("stubbing-export", this)
			, hooking_export("hooking-export", this)
			, debug_timing_export("debug-timing-export", this)
			, debug_processors_export("debug-processors-export", this)
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
			*dbg.stack_frame_export           [id] >> stack_frame_export;
			*dbg.debug_info_loading_export    [id] >> debug_info_loading_export;
			*dbg.data_object_lookup_export    [id] >> data_object_lookup_export;
			*dbg.subprogram_lookup_export     [id] >> subprogram_lookup_export;
			*dbg.stubbing_export              [id] >> stubbing_export;
			*dbg.hooking_export               [id] >> hooking_export;
			*dbg.debug_timing_export          [id] >> debug_timing_export;
			*dbg.debug_processors_export      [id] >> debug_processors_export;
			
			debug_event_listener_import >> *dbg.debug_event_listener_import[id];
			debug_yielding_import       >> *dbg.debug_yielding_import[id];
		}

		virtual void Setup(interfaces::DebugEventTrigger<ADDRESS>*) { dbg.RequireSetup(&ProcessorGate::memory_access_reporting_control_import); }
		virtual void Setup(interfaces::Memory<ADDRESS>*) { dbg.RequireSetup(&ProcessorGate::memory_import); }
		virtual void Setup(interfaces::Registers*) { dbg.RequireSetup(&ProcessorGate::registers_import); }
		virtual void Setup(interfaces::Disassembly<ADDRESS>*) { dbg.RequireSetup(&ProcessorGate::disasm_import); }
		
		virtual void Setup(interfaces::SymbolTableLookup<ADDRESS>*) { dbg.SetupDebugInfo(); }
		virtual void Setup(interfaces::StatementLookup<ADDRESS>*) { dbg.SetupDebugInfo(); }
		virtual void Setup(interfaces::StackFrame<ADDRESS>*)
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
		virtual void Setup(interfaces::DebugProcessors<ADDRESS, TIME_TYPE>*)
		{
			dbg.RequireSetup(&ProcessorGate::memory_access_reporting_control_import);
			dbg.RequireSetup(&ProcessorGate::memory_import);
			dbg.RequireSetup(&ProcessorGate::registers_import);
			dbg.RequireSetup(&ProcessorGate::disasm_import);
			dbg.SetupDebugInfo();
		}
		
		// From Front-end
		
		// unisim::service::interfaces::DebugYieldingRequest
		virtual void DebugYieldRequest() { /*dbg.Lock(); */dbg.DebugYieldRequest(id); /*dbg.Unlock();*/ }

		// unisim::service::interfaces::DebugSelecting
		virtual bool SelectProcessor(unsigned int prc_num) { /*dbg.Lock();*/ bool ret = dbg.SelectProcessor(id, prc_num); /*dbg.Unlock();*/ return ret; }
		virtual unsigned int GetSelectedProcessor() const { /*dbg.Lock();*/ unsigned int ret = dbg.GetSelectedProcessor(id); /*dbg.Unlock();*/ return ret; }
		
		// unisim::service::interfaces::DebugEventFactory<ADDRESS>
		virtual unisim::util::debug::Breakpoint<ADDRESS> *CreateBreakpoint(ADDRESS addr) { bool l = dbg.Lock(id); unisim::util::debug::Breakpoint<ADDRESS> *ret = dbg.CreateBreakpoint(id, addr, false); if(l) { dbg.Unlock(id); } return ret; }
		virtual unisim::util::debug::Watchpoint<ADDRESS> *CreateWatchpoint(unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size, bool overlook) { bool l = dbg.Lock(id); unisim::util::debug::Watchpoint<ADDRESS> *ret = dbg.CreateWatchpoint(id, mat, mt, addr, size, overlook, false); if(l) { dbg.Unlock(id); } return ret; }
		virtual unisim::util::debug::FetchInsnEvent<ADDRESS> *CreateFetchInsnEvent() { bool l = dbg.Lock(id); unisim::util::debug::FetchInsnEvent<ADDRESS> *ret = dbg.CreateFetchInsnEvent(id, false); if(l) { dbg.Unlock(id); } return ret; }
		virtual unisim::util::debug::FetchStmtEvent<ADDRESS> *CreateFetchStmtEvent() { bool l = dbg.Lock(id); unisim::util::debug::FetchStmtEvent<ADDRESS> *ret = dbg.CreateFetchStmtEvent(id, false); if(l) { dbg.Unlock(id); } return ret; }
		virtual unisim::util::debug::CommitInsnEvent<ADDRESS> *CreateCommitInsnEvent() { bool l = dbg.Lock(id); unisim::util::debug::CommitInsnEvent<ADDRESS> *ret = dbg.CreateCommitInsnEvent(id, false); if(l) { dbg.Unlock(id); } return ret; }
		virtual unisim::util::debug::NextInsnEvent<ADDRESS> *CreateNextInsnEvent() { bool l = dbg.Lock(id); unisim::util::debug::NextInsnEvent<ADDRESS> *ret = dbg.CreateNextInsnEvent(id, false); if(l) { dbg.Unlock(id); } return ret; }
		virtual unisim::util::debug::NextStmtEvent<ADDRESS> *CreateNextStmtEvent() { bool l = dbg.Lock(id); unisim::util::debug::NextStmtEvent<ADDRESS> *ret = dbg.CreateNextStmtEvent(id, false); if(l) { dbg.Unlock(id); } return ret; }
		virtual unisim::util::debug::FinishEvent<ADDRESS> *CreateFinishEvent() { bool l = dbg.Lock(id); unisim::util::debug::FinishEvent<ADDRESS> *ret = dbg.CreateFinishEvent(id, false); if(l) { dbg.Unlock(id); } return ret; }
		virtual unisim::util::debug::TrapEvent<ADDRESS> *CreateTrapEvent() { bool l = dbg.Lock(id); unisim::util::debug::TrapEvent<ADDRESS> *ret = dbg.CreateTrapEvent(id, false); if(l) { dbg.Unlock(id); } return ret; }
		virtual unisim::util::debug::SourceCodeBreakpoint<ADDRESS> *CreateSourceCodeBreakpoint(const unisim::util::debug::SourceCodeLocation& source_code_location, const std::string& filename) { bool l = dbg.Lock(id); unisim::util::debug::SourceCodeBreakpoint<ADDRESS> *ret = dbg.CreateSourceCodeBreakpoint(id, source_code_location, filename, false); if(l) { dbg.Unlock(id); } return ret; }
		virtual unisim::util::debug::SubProgramBreakpoint<ADDRESS> *CreateSubProgramBreakpoint(const unisim::util::debug::SubProgram<ADDRESS> *subprogram) { bool l = dbg.Lock(id); unisim::util::debug::SubProgramBreakpoint<ADDRESS> *ret = dbg.CreateSubProgramBreakpoint(id, subprogram, false); if(l) { dbg.Unlock(id); } return ret; }
		
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
		virtual void ScanSymbols(unisim::service::interfaces::SymbolTableScanner<ADDRESS>& scanner) const { bool l = dbg.Lock(id); dbg.ScanSymbols(id, scanner); if(l) { dbg.Unlock(id); } }
		virtual void ScanSymbols(unisim::service::interfaces::SymbolTableScanner<ADDRESS>& scanner, typename unisim::util::debug::SymbolBase::Type type) const { bool l = dbg.Lock(id); dbg.ScanSymbols(id, scanner, type); if(l) { dbg.Unlock(id); }}
		virtual const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbolByAddr(ADDRESS addr) const { bool l = dbg.Lock(id); const typename unisim::util::debug::Symbol<ADDRESS> *ret = dbg.FindSymbolByAddr(id, addr); if(l) { dbg.Unlock(id); } return ret; }
		virtual const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbolByName(const char *name) const { bool l = dbg.Lock(id); const typename unisim::util::debug::Symbol<ADDRESS> *ret = dbg.FindSymbolByName(id, name); if(l) { dbg.Unlock(id); } return ret; }
		virtual const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbolByName(const char *name, typename unisim::util::debug::SymbolBase::Type type) const { bool l = dbg.Lock(id); const typename unisim::util::debug::Symbol<ADDRESS> *ret = dbg.FindSymbolByName(id, name, type); if(l) { dbg.Unlock(id); } return ret; }
		virtual const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbolByAddr(ADDRESS addr, typename unisim::util::debug::SymbolBase::Type type) const { bool l = dbg.Lock(id); const typename unisim::util::debug::Symbol<ADDRESS> *ret = dbg.FindSymbolByAddr(id, addr, type); if(l) { dbg.Unlock(id); } return ret; }
		
		// unisim::service::interfaces::StatementLookup<ADDRESS>
		virtual void ScanStatements(unisim::service::interfaces::StatementScanner<ADDRESS>& scanner, const char *filename) const { bool l = dbg.Lock(id); dbg.ScanStatements(id, scanner, filename); if(l) { dbg.Unlock(id); } }
		virtual const unisim::util::debug::Statement<ADDRESS> *FindStatement(ADDRESS addr, const char *filename, typename unisim::service::interfaces::StatementLookup<ADDRESS>::Scope scope) const { bool l = dbg.Lock(id); const unisim::util::debug::Statement<ADDRESS> *ret = dbg.FindStatement(id, addr, filename, scope); if(l) { dbg.Unlock(id); } return ret; }
		virtual const unisim::util::debug::Statement<ADDRESS> *FindStatements(unisim::service::interfaces::StatementScanner<ADDRESS>& scanner, ADDRESS addr, const char *filename, typename unisim::service::interfaces::StatementLookup<ADDRESS>::Scope scope) const { bool l = dbg.Lock(id); const unisim::util::debug::Statement<ADDRESS> *ret = dbg.FindStatements(id, scanner, addr, filename, scope); if(l) { dbg.Unlock(id); } return ret; }
		virtual const unisim::util::debug::Statement<ADDRESS> *FindStatement(const unisim::util::debug::SourceCodeLocation& source_code_location, const char *filename) const { bool l = dbg.Lock(id); const unisim::util::debug::Statement<ADDRESS> *ret = dbg.FindStatement(id, source_code_location, filename); if(l) { dbg.Unlock(id); } return ret; }
		virtual const unisim::util::debug::Statement<ADDRESS> *FindStatements(unisim::service::interfaces::StatementScanner<ADDRESS>& scanner, const unisim::util::debug::SourceCodeLocation& source_code_location, const char *filename) const { bool l = dbg.Lock(id); const unisim::util::debug::Statement<ADDRESS> *ret = dbg.FindStatements(id, scanner, source_code_location, filename); if(l) { dbg.Unlock(id); } return ret; }
		
		// unisim::service::interfaces::StackFrame<ADDRESS>
		virtual bool SelectStackFrame(unsigned int frame_num) { /*dbg.Lock();*/ bool ret = dbg.SelectStackFrame(id, frame_num); /*dbg.Unlock();*/ return ret; }
		virtual unsigned int GetSelectedStackFrameNumber() const { /*dbg.Lock();*/ unsigned int ret = dbg.GetSelectedStackFrameNumber(id); /*dbg.Unlock();*/ return ret; }
		virtual std::vector<ADDRESS> *GetBackTrace() const { bool l = dbg.Lock(id); std::vector<ADDRESS> *ret = dbg.GetBackTrace(id); if(l) { dbg.Unlock(id); } return ret; }
		virtual bool GetReturnAddress(ADDRESS& ret_addr) const { bool l = dbg.Lock(id); bool ret = dbg.GetReturnAddress(id, ret_addr); if(l) { dbg.Unlock(id); } return ret; }
		virtual bool UnwindStack(unsigned int frame_num) { bool l = dbg.Lock(id); bool ret = dbg.UnwindStack(id, frame_num); if(l) { dbg.Unlock(id); } return ret; }
		virtual void ScanStackFrameInfos(unisim::service::interfaces::StackFrameInfoScanner<ADDRESS>& scanner, unsigned int max_stack_frames) const { bool l = dbg.Lock(id); dbg.ScanStackFrameInfos(id, scanner, max_stack_frames); if(l) { dbg.Unlock(id); } }
		virtual unsigned int GetStackFrameInfos(unisim::service::interfaces::StackFrameInfo<ADDRESS> *stack_frame_infos, unsigned int max_stack_frames) const { bool l = dbg.Lock(id); unsigned int ret = dbg.GetStackFrameInfos(id, stack_frame_infos, max_stack_frames); if(l) { dbg.Unlock(id); } return ret; }
		
		// unisim::service::interfaces::DebugInfoLoading
		virtual bool LoadDebugInfo(const char *filename) { bool l = dbg.Lock(id); bool ret = dbg.LoadDebugInfo(id, filename); if(l) { dbg.Unlock(id); } return ret; }
		virtual void ScanExecutableBinaryFiles(unisim::service::interfaces::ExecutableBinaryFileScanner& scanner) const { bool l = dbg.Lock(id); dbg.ScanExecutableBinaryFiles(id, scanner); if(l) { dbg.Unlock(id); } }
		
		// unisim::service::interfaces::DataObjectLookup<ADDRESS>
		virtual unisim::util::debug::DataObjectRef<ADDRESS> FindDataObject(const char *data_object_name) const { bool l = dbg.Lock(id); unisim::util::debug::DataObjectRef<ADDRESS> ret = dbg.FindDataObject(id, data_object_name); if(l) { dbg.Unlock(id); } return ret; }
		virtual unisim::util::debug::DataObjectRef<ADDRESS> GetReturnValue() const { bool l = dbg.Lock(id); unisim::util::debug::DataObjectRef<ADDRESS> ret = dbg.GetReturnValue(id); if(l) { dbg.Unlock(id); } return ret; }
		virtual unisim::util::debug::DataObjectRef<ADDRESS> GetSubProgramParameter(unsigned int index) const { bool l = dbg.Lock(id); unisim::util::debug::DataObjectRef<ADDRESS> ret = dbg.GetSubProgramParameter(id, index); if(l) { dbg.Unlock(id); } return ret; }
		virtual void ScanDataObjectNames(unisim::service::interfaces::DataObjectNameScanner& scanner, typename unisim::service::interfaces::DataObjectLookupBase::Scope scope) const { bool l = dbg.Lock(id); dbg.ScanDataObjectNames(id, scanner, scope); if(l) { dbg.Unlock(id); } }
		
		// unisim::service::interfaces::SubProgramLookup<ADDRESS>
		virtual const unisim::util::debug::SubProgram<ADDRESS> *FindSubProgram(const char *subprogram_name, const char *filename, const char *compilation_unit_name) const { bool l = dbg.Lock(id); const unisim::util::debug::SubProgram<ADDRESS> *ret = dbg.FindSubProgram(id, subprogram_name, filename, compilation_unit_name); if(l) { dbg.Unlock(id); } return ret; }
		virtual const unisim::util::debug::SubProgram<ADDRESS> *FindSubProgram(ADDRESS pc, const char *filename) const { bool l = dbg.Lock(id); const unisim::util::debug::SubProgram<ADDRESS> *ret = dbg.FindSubProgram(id, pc, filename); if(l) { dbg.Unlock(id); } return ret; }
		
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
		
		// unisim::service::interfaces::DebugProcessors<ADDRESS, TIME_TYPE>
		virtual unsigned int DebugGetProcessorCount() const { return dbg.DebugGetProcessorCount(id); }
		virtual unisim::service::interfaces::DebugProcessor<ADDRESS, TIME_TYPE> *DebugGetProcessor(unsigned int prc_num) const { return dbg.DebugGetProcessor(id, prc_num); }
		
		// To Front-end
		
		// unisim::service::interfaces::DebugYielding
		inline void DebugYield() { if(debug_yielding_import) debug_yielding_import->DebugYield(); }
		
	private:
		Debugger<CONFIG>& dbg;
		unsigned int id;
	};
	
	struct MultiEventListener : unisim::service::interfaces::DebugEventListener<ADDRESS>
	{
		MultiEventListener()
			: source_events()
		{
		}
		
		virtual ~MultiEventListener()
		{
			DetachAll();
		}
		
		bool Attached()
		{
			return !source_events.empty();
		}
		
		void Attach(unisim::util::debug::Event<ADDRESS> *source_event)
		{
			std::pair<typename SourceEvents::iterator, bool> r = source_events.insert(source_event);
			if(r.second)
			{
				source_event->Catch();
				source_event->AddEventListener(this);
			}
		}
		
		void Detach(unisim::util::debug::Event<ADDRESS> *source_event)
		{
			if(source_events.erase(source_event))
			{
				source_event->RemoveEventListener(this);
				source_event->Release();
			}
		}
		
		void DetachAll()
		{
			for(typename SourceEvents::iterator it = source_events.begin(); it != source_events.end(); ++it)
			{
				unisim::util::debug::Event<ADDRESS> *source_event = *it;
				source_event->RemoveEventListener(this);
				source_event->Release();
			}
			source_events.clear();
		}
		
		/* struct Visitor { void Visit(unisim::util::debug::Event<ADDRESS> *) {} }; */
		template <class VISITOR> void Scan(VISITOR& visitor)
		{
			for(typename SourceEvents::iterator it = source_events.begin(); it != source_events.end(); ++it)
			{
				unisim::util::debug::Event<ADDRESS> *source_event = *it;
				visitor.Visit(source_event);
			}
		}
		
	private:
		typedef std::set<unisim::util::debug::Event<ADDRESS> *> SourceEvents;
		SourceEvents source_events;
	};
	
	struct EventCombinator : MultiEventListener
	{
		EventCombinator(const unisim::util::debug::Event<ADDRESS> *_target_event)
			: MultiEventListener()
			, target_event(_target_event)
		{
			target_event->Catch();
		}
		
		virtual ~EventCombinator()
		{
			target_event->Release();
		}
		
		virtual void OnDebugEvent(const unisim::util::debug::Event<ADDRESS> *source_event)
		{
			target_event->Trigger();
		}
		
	private:
		const unisim::util::debug::Event<ADDRESS> *target_event;
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
			brkp->Trigger();
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
			wp->Trigger();
		}
		Debugger<CONFIG>& dbg;
		unsigned int front_end_num;
		bool& overlook;
	};
	
	struct EventRemover
	{
		EventRemover(Debugger<CONFIG>& _dbg) : dbg(_dbg), status(true) {}
		void Visit(unisim::util::debug::Event<ADDRESS> *event)
		{
			if(Breakpoint::IsInstanceOf(event))
			{
				Breakpoint *brkp = dynamic_cast<Breakpoint *>(event);
				if(!dbg.Unlisten(brkp->GetFrontEndNumber(), brkp)) status = false;
			}
			else if(Watchpoint::IsInstanceOf(event))
			{
				Watchpoint *wp = dynamic_cast<Watchpoint *>(event);
				if(!dbg.Unlisten(wp->GetFrontEndNumber(), wp)) status = false;
			}
			else if(FetchInsnEvent::IsInstanceOf(event))
			{
				FetchInsnEvent *fetch_insn_event = dynamic_cast<FetchInsnEvent *>(event);
				if(!dbg.Unlisten(fetch_insn_event->GetFrontEndNumber(), fetch_insn_event)) status = false;
			}
			else if(CommitInsnEvent::IsInstanceOf(event))
			{
				CommitInsnEvent *commit_insn_event = dynamic_cast<CommitInsnEvent *>(event);
				if(!dbg.Unlisten(commit_insn_event->GetFrontEndNumber(), commit_insn_event)) status = false;
			}
			else if(NextInsnEvent::IsInstanceOf(event))
			{
				NextInsnEvent *next_insn_event = dynamic_cast<NextInsnEvent *>(event);
				if(!dbg.Unlisten(next_insn_event->GetFrontEndNumber(), next_insn_event)) status = false;
			}
			else if(NextStmtEvent::IsInstanceOf(event))
			{
				NextStmtEvent *next_stmt_event = dynamic_cast<NextStmtEvent *>(event);
				if(!dbg.Unlisten(next_stmt_event->GetFrontEndNumber(), next_stmt_event)) status = false;
			}
			else if(FinishEvent::IsInstanceOf(event))
			{
				FinishEvent *finish_event = dynamic_cast<FinishEvent *>(event);
				if(!dbg.Unlisten(finish_event->GetFrontEndNumber(), finish_event)) status = false;
			}
			else if(TrapEvent::IsInstanceOf(event))
			{
				TrapEvent *trap_event = dynamic_cast<TrapEvent *>(event);
				if(!dbg.Unlisten(trap_event->GetFrontEndNumber(), trap_event)) status = false;
			}
			else if(FetchStmtEvent::IsInstanceOf(event))
			{
				FetchStmtEvent *fetch_stmt_event = dynamic_cast<FetchStmtEvent *>(event);
				if(!dbg.Unlisten(fetch_stmt_event->GetFrontEndNumber(), fetch_stmt_event)) status = false;
			}
			else if(SourceCodeBreakpoint::IsInstanceOf(event))
			{
				SourceCodeBreakpoint *src_code_brkp = dynamic_cast<SourceCodeBreakpoint *>(event);
				if(!dbg.Unlisten(src_code_brkp->GetFrontEndNumber(), src_code_brkp)) status = false;
			}
			else if(SubProgramBreakpoint::IsInstanceOf(event))
			{
				SubProgramBreakpoint *subprogram_brkp = dynamic_cast<SubProgramBreakpoint *>(event);
				if(!dbg.Unlisten(subprogram_brkp->GetFrontEndNumber(), subprogram_brkp)) status = false;
			}
		}
		Debugger<CONFIG>& dbg;
		bool status;
	};
	
	struct Breakpoint : unisim::util::debug::Breakpoint<ADDRESS>
	{
		typedef unisim::util::debug::Breakpoint<ADDRESS> Super;
		Breakpoint(unsigned int _front_end_num, unsigned int _prc_num, ADDRESS _addr, int _id = -1) : Super(_prc_num, _addr, _id), front_end_num(_front_end_num) {}
		unsigned int GetFrontEndNumber() const { return front_end_num; }
		using Super::SetId;
	private:
		unsigned int front_end_num;
	};
	
	struct Watchpoint : unisim::util::debug::Watchpoint<ADDRESS>
	{
		typedef unisim::util::debug::Watchpoint<ADDRESS> Super;
		Watchpoint(unsigned int _front_end_num, unsigned int _prc_num, unisim::util::debug::MemoryAccessType _mat, unisim::util::debug::MemoryType _mt, ADDRESS _addr, uint32_t _size, bool _overlook, int _id = -1) : Super(_prc_num, _mat, _mt, _addr, _size, _overlook, _id), front_end_num(_front_end_num) {}
		unsigned int GetFrontEndNumber() const { return front_end_num; }
		using Super::SetId;
	private:
		unsigned int front_end_num;
	};
	
	template <typename EVENT>
	struct Event : EVENT
	{
		Event(Debugger<CONFIG>& _dbg, unsigned int _front_end_num, unsigned int _prc_num, bool _internal = false) : EVENT(_prc_num), dbg(_dbg), front_end_num(_front_end_num), internal(_internal) {}
		Debugger<CONFIG>& GetDebugger() const { return dbg; }
		unsigned int GetFrontEndNumber() const { return front_end_num; }
		bool IsInternal() const { return internal; }
		bool Set() { return true; }
		bool Remove() { return true; }
	private:
		Debugger<CONFIG>& dbg;
		unsigned int front_end_num;
		bool internal;
	};
	
	struct FetchInsnEvent : Event<unisim::util::debug::FetchInsnEvent<ADDRESS> >
	{
		typedef Event<unisim::util::debug::FetchInsnEvent<ADDRESS> > Super;
		FetchInsnEvent(Debugger<CONFIG>& _dbg, unsigned int _front_end_num, unsigned int _prc_num, bool _internal = false) : Super(_dbg, _front_end_num, _prc_num, _internal) {}
		using Super::SetAddress;
	};
	
	// Note: the "nexti" support is crappy at the moment because it relies exclusively on DWARF in the end (for the stack frame information) while it should be aware of "call" instructions from underlying processor architectures.
	struct NextInsnEvent : Event<unisim::util::debug::NextInsnEvent<ADDRESS> >
	{
		typedef Event<unisim::util::debug::NextInsnEvent<ADDRESS> > Super;
		
		NextInsnEvent(
			Debugger<CONFIG>& _dbg,
			unsigned int _front_end_num,
			unsigned int _prc_num,
			bool _internal = false)
			: Super(_dbg, _front_end_num, _prc_num, _internal)
			, state(-1)
			, stack_frame_infos_depth(0)
			, stack_frame_infos()
			, fetch_insn_event(0)
			, finish_event(0)
			, event_listener(*this)
		{
		}
		
		virtual ~NextInsnEvent()
		{
			Remove();
			if(fetch_insn_event) fetch_insn_event->Release();
			if(finish_event) finish_event->Release();
		}
		
		bool Set()
		{
			if(state != -1) return false;
			if(((stack_frame_infos_depth = this->GetDebugger().GetStackFrameInfos(this->GetFrontEndNumber(), this->GetProcessorNumber(), stack_frame_infos, 2)) < 1) || !SetFetchInsnEvent()) return false;
			state = 0;
			return true;
		}
		
		bool Remove()
		{
			if(((state >= 0) && (state <= 1)) && ((state != 0) || RemoveFetchInsnEvent()) && ((state != 1) || RemoveFinishEvent()))
			{
				state = -1;
				return true;
			}
			return false;
		}
		
	private:
		struct EventListener : unisim::service::interfaces::DebugEventListener<ADDRESS>
		{
			EventListener(NextInsnEvent& _next_insn_event)
				: next_insn_event(_next_insn_event)
			{
			}
			
			virtual void OnDebugEvent(const unisim::util::debug::Event<ADDRESS> *event)
			{
				next_insn_event.OnDebugEvent(event);
			}
		private:
			NextInsnEvent& next_insn_event;
		};
		
		bool SetFetchInsnEvent()
		{
			if(!fetch_insn_event)
			{
				fetch_insn_event = this->GetDebugger().CreateFetchInsnEvent(this->GetFrontEndNumber(), this->GetProcessorNumber(), /* internal */ true);
				if(!fetch_insn_event) return false;
				fetch_insn_event->Catch();
				fetch_insn_event->AddEventListener(&event_listener);
			}
			
			return this->GetDebugger().Listen(this->GetFrontEndNumber(), fetch_insn_event);
		}
		
		bool RemoveFetchInsnEvent()
		{
			return !fetch_insn_event || this->GetDebugger().Unlisten(this->GetFrontEndNumber(), fetch_insn_event);
		}
		
		bool SetFinishEvent()
		{
			if(!finish_event)
			{
				finish_event = this->GetDebugger().CreateFinishEvent(this->GetFrontEndNumber(), this->GetProcessorNumber(), /* internal */ true);
				if(!finish_event) return false;
				finish_event->Catch();
				finish_event->AddEventListener(&event_listener);
			}
			
			return this->GetDebugger().Listen(this->GetFrontEndNumber(), finish_event);
		}
		
		bool RemoveFinishEvent()
		{
			return !finish_event || this->GetDebugger().Unlisten(this->GetFrontEndNumber(), finish_event);
		}
		
		void OnDebugEvent(const unisim::util::debug::Event<ADDRESS> *event)
		{
			switch(state)
			{
				case 0:
					if(event == fetch_insn_event)
					{
						// stepped instruction
						if(!Remove()) throw InternalError("Can't reset instruction fetch event");
						
						unisim::service::interfaces::StackFrameInfo<ADDRESS> stack_frame_info;
						
						if(this->GetDebugger().GetStackFrameInfos(this->GetFrontEndNumber(), this->GetProcessorNumber(), &stack_frame_info, 1) != 0)
						{
							if((stack_frame_infos_depth > 1) && (stack_frame_info.addr == stack_frame_infos[1].addr))
							{
								// outer stack frame reached
								if(!Set()) throw InternalError("Can't reset instruction fetch event");
								this->Trigger();
							}
							else if(stack_frame_info.addr == stack_frame_infos[0].addr)
							{
								// same stack frame
								if(!Set()) throw InternalError("Can't reset instruction fetch event");
								this->Trigger();
							}
							else
							{
								// call
								state = 1;
								if(!SetFinishEvent()) throw InternalError("Can't set finish event");
							}
						}
						else
						{
							throw InternalError("Can't get inner frame information");
						}
					}
					break;
				case 1:
					if(event == finish_event)
					{
						// finished call
						if(!Remove() || !Set()) throw InternalError("Can't reset instruction fetch event");
						this->Trigger();
					}
					break;
			}
		}
		
		int state;
		unsigned int stack_frame_infos_depth;
		unisim::service::interfaces::StackFrameInfo<ADDRESS> stack_frame_infos[2];
		unisim::util::debug::FetchInsnEvent<ADDRESS> *fetch_insn_event;
		unisim::util::debug::FinishEvent<ADDRESS> *finish_event;
		EventListener event_listener;
	};
	
	struct FetchStmtEvent : Event<unisim::util::debug::FetchStmtEvent<ADDRESS> >
	{
		typedef Event<unisim::util::debug::FetchStmtEvent<ADDRESS> > Super;
		FetchStmtEvent(
			Debugger<CONFIG>& _dbg,
			unsigned int _front_end_num,
			unsigned int _prc_num,
			bool _internal)
			: Super(_dbg, _front_end_num, _prc_num, _internal)
			, has_stack_frame_info(false)
			, stack_frame_info()
		{
		}
		
		virtual ~FetchStmtEvent()
		{
			Remove();
		}
		
		bool Set()
		{
			has_stack_frame_info = this->GetDebugger().GetStackFrameInfos(this->GetFrontEndNumber(), this->GetProcessorNumber(), &stack_frame_info, 1) != 0;
			
			const unisim::util::debug::Statement<ADDRESS> *stmt = this->GetDebugger().FindStatement(this->GetFrontEndNumber(), stack_frame_info.pc, /* filename */ 0, unisim::service::interfaces::StatementLookup<ADDRESS>::SCOPE_EXACT_STMT);
			
			this->SetStatement(stmt);
			
			return true;
		}
		
		bool Remove()
		{
			has_stack_frame_info = false;
			this->SetStatement(0);
			
			return true;
		}
		
		bool Stepped(ADDRESS next_addr)
		{
			bool stepped = false;
			
			const unisim::util::debug::Statement<ADDRESS> *stmt = this->GetDebugger().FindStatement(this->GetFrontEndNumber(), next_addr, /* filename */ 0, unisim::service::interfaces::StatementLookup<ADDRESS>::SCOPE_EXACT_STMT);
			
			if(stmt && stmt->IsBeginningOfSourceStatement())
			{
				if(stmt != this->GetStatement())
				{
					this->SetStatement(stmt);
					stepped = true;
				}
			
				unisim::service::interfaces::StackFrameInfo<ADDRESS> _stack_frame_info;
				
				if((this->GetDebugger().GetStackFrameInfos(this->GetFrontEndNumber(), this->GetProcessorNumber(), &_stack_frame_info, 1) != 0) && (!has_stack_frame_info || (_stack_frame_info.addr != stack_frame_info.addr)))
				{
					stack_frame_info = _stack_frame_info;
					has_stack_frame_info = true;
					stepped = true;
				}
			}
			
			return stepped;
		}
	private:
		bool has_stack_frame_info;
		unisim::service::interfaces::StackFrameInfo<ADDRESS> stack_frame_info;
	};
	
	struct CommitInsnEvent : Event<unisim::util::debug::CommitInsnEvent<ADDRESS> >
	{
		typedef Event<unisim::util::debug::CommitInsnEvent<ADDRESS> > Super;
		CommitInsnEvent(Debugger<CONFIG>& _dbg, unsigned int _front_end_num, unsigned int _prc_num, bool _internal) : Super(_dbg, _front_end_num, _prc_num, _internal) {}
		using Super::SetAddress;
		using Super::SetLength;
	};
	
	struct FinishEvent : Event<unisim::util::debug::FinishEvent<ADDRESS> >
	{
		typedef Event<unisim::util::debug::FinishEvent<ADDRESS> > Super;
		
		FinishEvent(
			Debugger<CONFIG>& _dbg,
			unsigned int _front_end_num,
			unsigned int _prc_num,
			bool _internal)
			: Super(_dbg, _front_end_num, _prc_num, _internal)
			, stack_frame_infos()
			, ret_brkp(0)
			, ret_brkp_listener(*this)
		{
		}
		
		virtual ~FinishEvent()
		{
			Remove();
		}
		
		bool Set()
		{
			if(this->GetDebugger().GetStackFrameInfos(this->GetFrontEndNumber(), this->GetProcessorNumber(), stack_frame_infos, 2) != 2) return false;
			
			ADDRESS brkp_addr = stack_frame_infos[0].ret_addr;
			
			ret_brkp = this->GetDebugger().CreateBreakpoint(this->GetFrontEndNumber(), this->GetProcessorNumber(), brkp_addr, /* internal */ true);
			
			if(!ret_brkp) return false;
			
			ret_brkp->Catch();
			
			ret_brkp->AddEventListener(&ret_brkp_listener);
			
			if(!this->GetDebugger().Listen(this->GetFrontEndNumber(), ret_brkp)) return false;
			
			return true;
		}
		
		bool Remove()
		{
			if(ret_brkp)
			{
				if(!this->GetDebugger().Unlisten(this->GetFrontEndNumber(), ret_brkp)) return false;
				ret_brkp->Release();
				ret_brkp = 0;
			}
			
			return true;
		}
		
	private:
		struct EventListener : unisim::service::interfaces::DebugEventListener<ADDRESS>
		{
			EventListener(FinishEvent& _finish_event)
				: finish_event(_finish_event)
			{
			}
			
			virtual void OnDebugEvent(const unisim::util::debug::Event<ADDRESS> *event)
			{
				finish_event.OnDebugEvent(event);
			}
		private:
			FinishEvent& finish_event;
		};
		
		void OnDebugEvent(const unisim::util::debug::Event<ADDRESS> *event)
		{
			if(event == ret_brkp)
			{
				if(!Remove()) throw InternalError("Can't reset finish event");
				
				// return breakpoint reached
				unisim::service::interfaces::StackFrameInfo<ADDRESS> stack_frame_info;
				
				if(this->GetDebugger().GetStackFrameInfos(this->GetFrontEndNumber(), this->GetProcessorNumber(), &stack_frame_info, 1) != 0)
				{
					if(stack_frame_info.addr == stack_frame_infos[1].addr)
					{
						// outer stack frame reached
						Set();
						this->Trigger();
					}
				}
				else
				{
					throw InternalError("Can't get inner frame information");
				}
			}
		}
		
		unisim::service::interfaces::StackFrameInfo<ADDRESS> stack_frame_infos[2];
		unisim::util::debug::Breakpoint<ADDRESS> *ret_brkp;
		EventListener ret_brkp_listener;
	};
	
	struct NextStmtEvent : Event<unisim::util::debug::NextStmtEvent<ADDRESS> >
	{
		typedef Event<unisim::util::debug::NextStmtEvent<ADDRESS> > Super;
		
		NextStmtEvent(
			Debugger<CONFIG>& _dbg,
			unsigned int _front_end_num,
			unsigned int _prc_num,
			bool _internal)
			: Super(_dbg, _front_end_num, _prc_num, _internal)
			, state(-1)
			, stack_frame_infos_depth(0)
			, stack_frame_infos()
			, fetch_stmt_event(0)
			, finish_event(0)
			, event_listener(*this)
		{
		}
		
		virtual ~NextStmtEvent()
		{
			Remove();
			if(fetch_stmt_event) fetch_stmt_event->Release();
			if(finish_event) finish_event->Release();
		}
		
		bool Set()
		{
			if(state != -1) return false;
			if(((stack_frame_infos_depth = this->GetDebugger().GetStackFrameInfos(this->GetFrontEndNumber(), this->GetProcessorNumber(), stack_frame_infos, 2)) < 1) || !SetFetchStmtEvent()) return false;
			state = 0;
			return true;
		}
		
		bool Remove()
		{
			if(((state >= 0) && (state <= 2)) && (((state != 0) && (state != 2)) || RemoveFetchStmtEvent()) && ((state != 1) || RemoveFinishEvent()))
			{
				state = -1;
				return true;
			}
			return false;
		}
		
	private:
		struct EventListener : unisim::service::interfaces::DebugEventListener<ADDRESS>
		{
			EventListener(NextStmtEvent& _next_stmt_event)
				: next_stmt_event(_next_stmt_event)
			{
			}
			
			virtual void OnDebugEvent(const unisim::util::debug::Event<ADDRESS> *event)
			{
				next_stmt_event.OnDebugEvent(event);
			}
		private:
			NextStmtEvent& next_stmt_event;
		};
		
		bool SetFetchStmtEvent()
		{
			if(!fetch_stmt_event)
			{
				fetch_stmt_event = this->GetDebugger().CreateFetchStmtEvent(this->GetFrontEndNumber(), this->GetProcessorNumber(), /* internal */ true);
				if(!fetch_stmt_event) return false;
				fetch_stmt_event->Catch();
				fetch_stmt_event->AddEventListener(&event_listener);
			}
			
			return this->GetDebugger().Listen(this->GetFrontEndNumber(), fetch_stmt_event);
		}
		
		bool RemoveFetchStmtEvent()
		{
			return !fetch_stmt_event || this->GetDebugger().Unlisten(this->GetFrontEndNumber(), fetch_stmt_event);
		}
		
		bool SetFinishEvent()
		{
			if(!finish_event)
			{
				finish_event = this->GetDebugger().CreateFinishEvent(this->GetFrontEndNumber(), this->GetProcessorNumber(), /* internal */ true);
				if(!finish_event) return false;
				finish_event->Catch();
				finish_event->AddEventListener(&event_listener);
			}
			
			return this->GetDebugger().Listen(this->GetFrontEndNumber(), finish_event);
		}
		
		bool RemoveFinishEvent()
		{
			return !finish_event || this->GetDebugger().Unlisten(this->GetFrontEndNumber(), finish_event);
		}
		
		void OnDebugEvent(const unisim::util::debug::Event<ADDRESS> *event)
		{
			switch(state)
			{
				case 0:
					if(event == fetch_stmt_event)
					{
						// stepped
						if(!Remove()) throw InternalError("Can't reset fetch statement event");
						
						unisim::service::interfaces::StackFrameInfo<ADDRESS> stack_frame_info;
						
						if(this->GetDebugger().GetStackFrameInfos(this->GetFrontEndNumber(), this->GetProcessorNumber(), &stack_frame_info, 1) != 0)
						{
							if((stack_frame_infos_depth > 1) && (stack_frame_info.addr == stack_frame_infos[1].addr))
							{
								// outer stack frame reached
								if(!Set()) throw InternalError("Can't reset fetch statement event");
								this->Trigger();
							}
							else if(stack_frame_info.addr == stack_frame_infos[0].addr)
							{
								// same stack frame
								if(!Set()) throw InternalError("Can't reset fetch statement event");
								this->Trigger();
							}
							else
							{
								// call
								state = 1;
								if(!SetFinishEvent()) throw InternalError("Can't set finish event");
							}
						}
						else
						{
							throw InternalError("Can't get inner frame information");
						}
					}
					break;
				case 1:
					if(event == finish_event)
					{
						// finished call
						Remove();
						
						unisim::service::interfaces::StackFrameInfo<ADDRESS> stack_frame_info;
						
						if(this->GetDebugger().GetStackFrameInfos(this->GetFrontEndNumber(), this->GetProcessorNumber(), &stack_frame_info, 1) != 0)
						{
							const unisim::util::debug::Statement<ADDRESS> *stmt = this->GetDebugger().FindStatement(this->GetFrontEndNumber(), stack_frame_info.pc, /* filename */ 0, unisim::service::interfaces::StatementLookup<ADDRESS>::SCOPE_EXACT_STMT);
							
							if(stmt && stmt->IsBeginningOfSourceStatement())
							{
								// stepped
								Set();
								this->Trigger();
								return;
							}
						}
						else
						{
							throw InternalError("Can't get inner frame information");
						}
						
						state = 2;
						if(!SetFetchStmtEvent()) throw InternalError("Can't set fetch statement event");
					}
					break;
				case 2:
					if(event == fetch_stmt_event)
					{
						// stepped
						if(!Remove() || !Set()) throw InternalError("Can't reset next statement event");
						this->Trigger();
					}
					break;
			}
		}
		
		int state;
		unsigned int stack_frame_infos_depth;
		unisim::service::interfaces::StackFrameInfo<ADDRESS> stack_frame_infos[2];
		unisim::util::debug::FetchStmtEvent<ADDRESS> *fetch_stmt_event;
		unisim::util::debug::FinishEvent<ADDRESS> *finish_event;
		EventListener event_listener;
	};
	
	struct TrapEvent : Event<unisim::util::debug::TrapEvent<ADDRESS> >
	{
		typedef Event<unisim::util::debug::TrapEvent<ADDRESS> > Super;
		TrapEvent(Debugger<CONFIG>& _dbg, unsigned int _front_end_num, unsigned int _prc_num, bool _internal) : Super(_dbg, _front_end_num, _prc_num, _internal) {}
		using Super::SetTrapObject;
		using Super::SetTrapMessage;
	};
	
	struct SourceCodeBreakpoint : unisim::util::debug::SourceCodeBreakpoint<ADDRESS>
	{
		typedef unisim::util::debug::SourceCodeBreakpoint<ADDRESS> Super;
		
		SourceCodeBreakpoint(
			Debugger<CONFIG>& _dbg,
			unsigned int _front_end_num,
			unsigned int _prc_num,
			const unisim::util::debug::SourceCodeLocation& _source_code_location,
			const std::string& _filename = std::string(),
			int _id = -1)
			: Super(_prc_num, _source_code_location, _filename, _id)
			, dbg(_dbg)
			, front_end_num(_front_end_num)
			, event_combinator(this)
		{
		}
		
		virtual ~SourceCodeBreakpoint()
		{
			Remove();
		}

		unsigned int GetFrontEndNumber() const { return front_end_num; }
		
		bool Set()
		{
			if(event_combinator.Attached()) return true; // already set
			
			typedef std::vector<const unisim::util::debug::Statement<ADDRESS> *> Statements;
			Statements stmts;
			
			struct StatementScanner : unisim::service::interfaces::StatementScanner<ADDRESS>
			{
				SourceCodeBreakpoint& source_code_breakpoint;
				bool found;
				bool status;
				
				StatementScanner(SourceCodeBreakpoint& _source_code_breakpoint) : source_code_breakpoint(_source_code_breakpoint), found(false), status(true) {}
				
				virtual void Append(const unisim::util::debug::Statement<ADDRESS> *stmt)
				{
					if(!status) return;
					
					found = true;
					
					ADDRESS brkp_addr = stmt->GetAddress();
					
					unisim::util::debug::Breakpoint<ADDRESS> *brkp = source_code_breakpoint.dbg.CreateBreakpoint(source_code_breakpoint.front_end_num, source_code_breakpoint.GetProcessorNumber(), brkp_addr, /* internal */ true);
					
					source_code_breakpoint.event_combinator.Attach(brkp);
					
					if(!source_code_breakpoint.dbg.Listen(source_code_breakpoint.front_end_num, brkp))
					{
						status = false;
						source_code_breakpoint.event_combinator.Detach(brkp);
						if(!source_code_breakpoint.Remove()) throw InternalError("Can't remove source code breakpoint internal resources");
					}
				}
			};
			
			StatementScanner stmt_scanner(*this);
			
			dbg.FindStatements(front_end_num, stmt_scanner, this->GetSourceCodeLocation(), this->GetFilename());
			
			return stmt_scanner.found && stmt_scanner.status;
		}
		
		bool Remove()
		{
			EventRemover event_remover(dbg);
			event_combinator.Scan(event_remover);
			event_combinator.DetachAll();
			return event_remover.status;
		}
	private:
		Debugger<CONFIG>& dbg;
		unsigned int front_end_num;
		EventCombinator event_combinator;
	};
	
	struct SubProgramBreakpoint : unisim::util::debug::SubProgramBreakpoint<ADDRESS>
	{
		typedef unisim::util::debug::SubProgramBreakpoint<ADDRESS> Super;
		SubProgramBreakpoint(
			Debugger<CONFIG>& _dbg,
			unsigned int _front_end_num,
			unsigned int _prc_num,
			const unisim::util::debug::SubProgram<ADDRESS> *_subprogram,
			int _id = -1)
			: Super(_prc_num, _subprogram, _id)
			, dbg(_dbg)
			, front_end_num(_front_end_num)
			, event_combinator(this)
		{
		}
		
		virtual ~SubProgramBreakpoint()
		{
			Remove();
		}
		
		unsigned int GetFrontEndNumber() const { return front_end_num; }
		
		bool Set()
		{
			if(event_combinator.Attached()) return true; // already set
			
			const unisim::util::debug::SubProgram<ADDRESS> *subprogram = this->GetSubProgram();
			
			ADDRESS brkp_addr = subprogram->GetAddress();
				
			unisim::util::debug::Breakpoint<ADDRESS> *brkp = dbg.CreateBreakpoint(front_end_num, this->GetProcessorNumber(), brkp_addr, /* internal */ true);
			
			event_combinator.Attach(brkp);
			
			if(!dbg.Listen(front_end_num, brkp))
			{
				event_combinator.Detach(brkp);
				if(!this->Remove()) throw InternalError("Can't remove subprogram breakpoint internal resources");
				return false;
			}
			
			return true;
		}
		
		bool Remove()
		{
			EventRemover event_remover(dbg);
			event_combinator.Scan(event_remover);
			event_combinator.DetachAll();
			return event_remover.status;
		}
		
	private:
		Debugger<CONFIG>& dbg;
		unsigned int front_end_num;
		EventCombinator event_combinator;
	};
	
	struct StubHandler : MultiEventListener
	{
		StubHandler(Debugger<CONFIG>& _dbg, unsigned int _front_end_num, unsigned int _prc_num, unisim::util::debug::Stub<ADDRESS> *_stub)
			: MultiEventListener()
			, dbg(_dbg)
			, front_end_num(_front_end_num)
			, prc_num(_prc_num)
			, stub(_stub)
		{
			stub->Catch();
		}
		
		virtual ~StubHandler()
		{
			stub->Release();
		}
		
		bool Set()
		{
			const unisim::util::debug::SubProgram<ADDRESS> *subprogram = stub->GetSubProgram();
			ADDRESS brkp_addr = subprogram->GetAddress();
			unisim::util::debug::Breakpoint<ADDRESS> *brkp = dbg.CreateBreakpoint(front_end_num, this->GetProcessorNumber(), brkp_addr, /* internal */ true);
			this->Attach(brkp);
			
			if(!dbg.Listen(front_end_num, brkp))
			{
				this->Detach(brkp);
				if(!this->Remove()) throw InternalError("Can't remove stub handler");
				return false;
			}
			
			return true;
		}
		
		bool Remove()
		{
			EventRemover event_remover(dbg);
			this->Scan(event_remover);
			this->DetachAll();
			return event_remover.status;
		}
		
		virtual void OnDebugEvent(const unisim::util::debug::Event<ADDRESS> *source_event)
		{
			dbg.CallStub(front_end_num, prc_num, stub);
		}
		
		unisim::util::debug::Stub<ADDRESS> *GetStub() const { return stub; }
		
		unsigned int GetProcessorNumber() const { return prc_num; }
		
	private:
		Debugger<CONFIG>& dbg;
		unsigned int front_end_num;
		unsigned int prc_num;
		unisim::util::debug::Stub<ADDRESS> *stub;
	};
	
	struct HookHandler : MultiEventListener
	{
		HookHandler(Debugger<CONFIG>& _dbg, unsigned int _front_end_num, unsigned int _prc_num, unisim::util::debug::Hook<ADDRESS> *_hook)
			: MultiEventListener()
			, dbg(_dbg)
			, front_end_num(_front_end_num)
			, prc_num(_prc_num)
			, hook(_hook)
		{
			hook->Catch();
		}
		
		virtual ~HookHandler()
		{
			hook->Release();
		}
		
		bool Set()
		{
			if(unisim::util::debug::SourceCodeHook<ADDRESS>::IsInstanceOf(hook))
			{
				unisim::util::debug::SourceCodeHook<ADDRESS> *src_code_hook = static_cast<unisim::util::debug::SourceCodeHook<ADDRESS> *>(hook);
				
				struct StatementScanner : unisim::service::interfaces::StatementScanner<ADDRESS>
				{
					HookHandler& hook_handler;
					bool found;
					bool status;
					
					StatementScanner(HookHandler& _hook_handler) : hook_handler(_hook_handler), found(false), status(true) {}
					
					virtual void Append(const unisim::util::debug::Statement<ADDRESS> *stmt)
					{
						if(!status) return;
						
						found = true;
						
						ADDRESS brkp_addr = stmt->GetAddress();
						
						unisim::util::debug::Breakpoint<ADDRESS> *brkp = hook_handler.dbg.CreateBreakpoint(hook_handler.front_end_num, hook_handler.GetProcessorNumber(), brkp_addr, /* internal */ true);
						
						hook_handler.Attach(brkp);
						
						if(!hook_handler.dbg.Listen(hook_handler.front_end_num, brkp))
						{
							status = false;
							hook_handler.Detach(brkp);
							if(!hook_handler.Remove()) throw InternalError("Can't remove hook handler");
						}
					}
				};
				
				StatementScanner stmt_scanner(*this);
				
				dbg.FindStatements(front_end_num, stmt_scanner, src_code_hook->GetSourceCodeLocation(), src_code_hook->GetFilename());
				
				if(!stmt_scanner.found || !stmt_scanner.status)
				{
					return false;
				}
			}
			else if(unisim::util::debug::AddressHook<ADDRESS>::IsInstanceOf(hook))
			{
				unisim::util::debug::AddressHook<ADDRESS> *addr_hook = static_cast<unisim::util::debug::AddressHook<ADDRESS> *>(hook);
				
				ADDRESS brkp_addr = addr_hook->GetAddress();
				
				unisim::util::debug::Breakpoint<ADDRESS> *brkp = dbg.CreateBreakpoint(front_end_num, this->GetProcessorNumber(), brkp_addr, /* internal */ true);
				
				this->Attach(brkp);
				
				if(!dbg.Listen(front_end_num, brkp))
				{
					this->Detach(brkp);
					if(!this->Remove()) throw InternalError("Can't remove hook handler");
					return false;
				}
			}
			else if(unisim::util::debug::SubProgramHook<ADDRESS>::IsInstanceOf(hook))
			{
				unisim::util::debug::SubProgramHook<ADDRESS> *subprogram_hook = static_cast<unisim::util::debug::SubProgramHook<ADDRESS> *>(hook);
				
				const unisim::util::debug::SubProgram<ADDRESS> *subprogram = subprogram_hook->GetSubProgram();
				
				ADDRESS brkp_addr = subprogram->GetAddress();
				
				unisim::util::debug::Breakpoint<ADDRESS> *brkp = dbg.CreateBreakpoint(front_end_num, this->GetProcessorNumber(), brkp_addr, /* internal */ true);
				
				this->Attach(brkp);
				
				if(!dbg.Listen(front_end_num, brkp))
				{
					this->Detach(brkp);
					if(!this->Remove()) throw InternalError("Can't remove hook handler");
				}
			}
			else
			{
				return false;
			}
			
			return true;
		}
		
		bool Remove()
		{
			EventRemover event_remover(dbg);
			this->Scan(event_remover);
			this->DetachAll();
			return event_remover.status;
		}
		
		virtual void OnDebugEvent(const unisim::util::debug::Event<ADDRESS> *source_event)
		{
			dbg.CallHook(front_end_num, prc_num, hook);
		}
		
		unisim::util::debug::Hook<ADDRESS> *GetHook() const { return hook; }
		
		unsigned int GetProcessorNumber() const { return prc_num; }
		
	private:
		Debugger<CONFIG>& dbg;
		unsigned int front_end_num;
		unsigned int prc_num;
		unisim::util::debug::Hook<ADDRESS> *hook;
	};

	Processor *processors[MAX_FRONT_ENDS][NUM_PROCESSORS];
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
	unsigned int max_stack_frames;
	unsigned int sel_cpu[MAX_FRONT_ENDS];
	std::string architecture[NUM_PROCESSORS];

	unisim::kernel::variable::Parameter<bool> param_verbose;
	unisim::kernel::variable::Parameter<std::string> param_dwarf_to_html_output_directory;
	unisim::kernel::variable::Parameter<std::string> param_dwarf_register_number_mapping_filename;
	unisim::kernel::variable::Parameter<bool> param_parse_dwarf;
	unisim::kernel::variable::Parameter<bool> param_debug_dwarf;
	unisim::kernel::variable::Parameter<unsigned int> param_max_stack_frames;
	unisim::kernel::variable::ParameterArray<unsigned int> param_sel_cpu;
	unisim::kernel::variable::ParameterArray<std::string> param_architecture;

	unisim::kernel::logger::Logger logger;
	bool setup_debug_info_done;
	pthread_mutex_t mutex;
	
	pthread_mutex_t curr_front_end_num_mutex;
	int curr_front_end_num;
	
	struct BreakpointRegistryConfig : CONFIG
	{
		typedef Breakpoint BREAKPOINT;
	};
	
	struct WatchpointRegistryConfig : CONFIG
	{
		typedef Watchpoint WATCHPOINT;
	};
	
	unisim::util::debug::BreakpointRegistry<BreakpointRegistryConfig> breakpoint_registry;
	unisim::util::debug::WatchpointRegistry<WatchpointRegistryConfig> watchpoint_registry;
	typedef std::set<FetchInsnEvent *> FetchInsnEventSet;
	FetchInsnEventSet fetch_insn_event_set[NUM_PROCESSORS];
	typedef std::set<CommitInsnEvent *> CommitInsnEventSet;
	CommitInsnEventSet commit_insn_event_set[NUM_PROCESSORS];
	typedef std::set<NextInsnEvent *> NextInsnEventSet;
	NextInsnEventSet next_insn_event_set[NUM_PROCESSORS];
	typedef std::set<NextStmtEvent *> NextStmtEventSet;
	NextStmtEventSet next_stmt_event_set[NUM_PROCESSORS];
	typedef std::set<FinishEvent *> FinishEventSet;
	FinishEventSet finish_event_set[NUM_PROCESSORS];
	typedef std::set<TrapEvent *> TrapEventSet;
	TrapEventSet trap_event_set[NUM_PROCESSORS];
	typedef std::set<FetchStmtEvent *> FetchStmtEventSet;
	FetchStmtEventSet fetch_stmt_event_set[NUM_PROCESSORS];
	typedef std::set<unisim::util::debug::SourceCodeBreakpoint<ADDRESS> *> SourceCodeBreakpointRegistry;
	SourceCodeBreakpointRegistry source_code_breakpoint_registry[MAX_FRONT_ENDS];
	typedef std::set<unisim::util::debug::SubProgramBreakpoint<ADDRESS> *> SubProgramBreakpointRegistry;
	SubProgramBreakpointRegistry subprogram_breakpoint_registry[MAX_FRONT_ENDS];
	typedef std::map<unisim::util::debug::Stub<ADDRESS> *, StubHandler *> StubRegistry;
	StubRegistry stub_registry[MAX_FRONT_ENDS][NUM_PROCESSORS];
	typedef std::map<unisim::util::debug::Hook<ADDRESS> *, HookHandler *> HookRegistry;
	HookRegistry hook_registry[MAX_FRONT_ENDS][NUM_PROCESSORS];
	
	mutable int next_id[MAX_FRONT_ENDS];
	
	pthread_mutex_t schedule_mutex;
	uint64_t schedule;
	uint64_t invalidate_frames_mask;
	std::vector<unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *> elf32_loaders;
	std::vector<unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *> elf64_loaders;
	std::vector<unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *> coff_loaders;
	std::vector<bool> enable_elf32_loaders[MAX_FRONT_ENDS];
	std::vector<bool> enable_elf64_loaders[MAX_FRONT_ENDS];
	std::vector<bool> enable_coff_loaders[MAX_FRONT_ENDS];
	struct ExecutableBinaryFile : unisim::service::interfaces::ExecutableBinaryFile
	{
		Debugger<CONFIG>& dbg;
		unsigned front_end_num;
		unsigned loader_num;
		const unisim::util::blob::Blob<ADDRESS> *blob;
		
		ExecutableBinaryFile(Debugger<CONFIG>& _dbg, unsigned int _front_end_num, unsigned int _loader_num, const unisim::util::blob::Blob<ADDRESS> *_blob)
			: dbg(_dbg), front_end_num(_front_end_num), loader_num(_loader_num), blob(_blob)
		{
		}
		virtual const char *GetFilename() const { return (blob->GetCapability() & unisim::util::blob::CAP_FILENAME) ? blob->GetFilename() : ""; }
		virtual unisim::util::blob::FileFormat GetFileFormat() const { return blob->GetFileFormat(); }
		virtual bool IsEnabled() const { return Enable(); }
		virtual void Enable(bool enable) { Enable() = enable; dbg.dw_mach_state[front_end_num].EnableBinary(blob, enable); }
		
		std::vector<bool>::reference Enable() const
		{
			switch(blob->GetFileFormat())
			{
				case unisim::util::blob::FFMT_UNKNOWN: break;
				case unisim::util::blob::FFMT_ELF32: return dbg.enable_elf32_loaders[front_end_num][loader_num];
				case unisim::util::blob::FFMT_ELF64: return dbg.enable_elf64_loaders[front_end_num][loader_num];
				case unisim::util::blob::FFMT_COFF: return dbg.enable_coff_loaders[front_end_num][loader_num];
			}
			static std::vector<bool> _(1, false);
			return _[0];
		}
	};
	typedef std::vector<ExecutableBinaryFile *> ExecutableBinaryFiles;
	ExecutableBinaryFiles exec_bin_files[MAX_FRONT_ENDS];
	
	void SetupELFSymtab(const typename unisim::util::blob::Blob<ADDRESS> *blob);
	void SetupDWARF(const typename unisim::util::blob::Blob<ADDRESS> *blob);
	
	bool SetupDebugInfo(const unisim::util::blob::Blob<ADDRESS> *blob);
	void SetupDebugInfo();
	void RequireSetup(int);
	
	void UpdateReportingRequirements(unsigned int prc_num);
	void ScheduleFrontEnd(unsigned int front_end_num);
	bool NextScheduledFrontEnd(unsigned int& front_end_num);
	bool IsScheduleEmpty() const;
	bool IsScheduled(unsigned int front_end_num) const;
	
	void InvalidateDirtyFrames(uint64_t mask);
	
	bool Lock(int front_end_num = -1);
	void Unlock(int front_end_num = -1);
	
	int AllocateId(unsigned int front_end_num) const;
	
	bool ReadProgramCounterRegister(unsigned int front_end_num, unsigned int prc_num, ADDRESS& pc) const;
	
	virtual void VariableBaseNotify(const unisim::kernel::VariableBase *var);
	
	
	void CallStub(unsigned int front_end_num, unsigned int prc_num, unisim::util::debug::Stub<ADDRESS> *stub);
	void CallHook(unsigned int front_end_num, unsigned int prc_num, unisim::util::debug::Hook<ADDRESS> *hook);
	
	template <typename VISITOR, typename T = bool>
	T ScanEnabledLoaders(unsigned int front_end_num, VISITOR& visitor) const;
	
	template <typename VISITOR, typename T = bool>
	T ScanAndFilterLoadersByFilename(unsigned int front_end_num, const char *filename, VISITOR& visitor) const;
	
	struct StatementFilter
	{
		const unisim::util::debug::Statement<ADDRESS> *ret_stmt;
		ADDRESS addr;
		typename unisim::service::interfaces::StatementLookup<ADDRESS>::Scope scope;
		StatementFilter(ADDRESS _addr, typename unisim::service::interfaces::StatementLookup<ADDRESS>::Scope _scope) : ret_stmt(0), addr(_addr), scope(_scope) {}
		const typename unisim::util::debug::Statement<ADDRESS> *Filter(const typename unisim::util::debug::Statement<ADDRESS> *stmt)
		{
			if(stmt)
			{
				switch(scope)
				{
					case unisim::service::interfaces::StatementLookup<ADDRESS>::SCOPE_NEAREST_LOWER_OR_EQUAL_STMT:
					case unisim::service::interfaces::StatementLookup<ADDRESS>::SCOPE_NEAREST_LOWER_OR_EQUAL_STMT_WITHIN_FUNCTION:
						if(stmt->GetAddress() <= addr)
						{
							if(!ret_stmt || ((addr - stmt->GetAddress()) < (addr - ret_stmt->GetAddress()))) ret_stmt = stmt;
						}
						break;
					case unisim::service::interfaces::StatementLookup<ADDRESS>::SCOPE_NEXT_STMT:
					case unisim::service::interfaces::StatementLookup<ADDRESS>::SCOPE_NEXT_STMT_WITHIN_FUNCTION:
						if(stmt->GetAddress() > addr)
						{
							if(!ret_stmt || ((stmt->GetAddress() - addr) < (ret_stmt->GetAddress() - addr))) ret_stmt = stmt;
						}
						break;
					case unisim::service::interfaces::StatementLookup<ADDRESS>::SCOPE_EXACT_STMT:
						return stmt;
						break;
				}
			}
			return 0;
		}
	};
};

} // end of namespace debugger
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif
