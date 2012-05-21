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

#include <unisim/kernel/service/service.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/util/debug/breakpoint_registry.hh>
#include <unisim/util/debug/watchpoint_registry.hh>
#include <unisim/util/loader/elf_loader/elf32_loader.hh>
#include <unisim/util/loader/elf_loader/elf64_loader.hh>
#include <unisim/util/loader/coff_loader/coff_loader.hh>
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
#include <unisim/service/interfaces/blob.hh>
#include <unisim/service/interfaces/debug_event.hh>
#include <unisim/service/interfaces/profiling.hh>
#include <unisim/service/interfaces/debug_info_loading.hh>

#include <string>

namespace unisim {
namespace service {
namespace debug {
namespace debugger {

using unisim::kernel::service::Service;
using unisim::kernel::service::Client;
using unisim::kernel::service::Object;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceExportBase;
using unisim::kernel::service::Parameter;
using unisim::service::interfaces::DebugControl;
using unisim::service::interfaces::MemoryAccessReporting;
using unisim::service::interfaces::TrapReporting;
using unisim::service::interfaces::SymbolTableLookup;
using unisim::service::interfaces::StatementLookup;
using unisim::service::interfaces::BackTrace;
using unisim::service::interfaces::MemoryAccessReportingControl;
using unisim::service::interfaces::Disassembly;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::Loader;
using unisim::service::interfaces::DebugEventTrigger;
using unisim::service::interfaces::DebugEventListener;
using unisim::service::interfaces::DebugInfoLoading;

template <class ADDRESS>
class Debugger
	: public Service<DebugEventTrigger<ADDRESS> >
	, public Service<DebugControl<ADDRESS> >
	, public Service<MemoryAccessReporting<ADDRESS> >
	, public Service<TrapReporting>
	, public Service<Disassembly<ADDRESS> >
	, public Service<Memory<ADDRESS> >
	, public Service<Registers>
	, public Service<SymbolTableLookup<ADDRESS> >
	, public Service<StatementLookup<ADDRESS> >
	, public Service<BackTrace<ADDRESS> >
	, public Service<DebugInfoLoading>
	, public Client<DebugEventListener<ADDRESS> >
	, public Client<DebugControl<ADDRESS> >
	, public Client<MemoryAccessReportingControl>
	, public Client<TrapReporting>
	, public Client<Disassembly<ADDRESS> >
	, public Client<Memory<ADDRESS> >
	, public Client<Registers>
	, public Client<Loader>
	, public Client<unisim::service::interfaces::Blob<ADDRESS> >
{
public:
	ServiceExport<DebugEventTrigger<ADDRESS> > debug_event_trigger_export;
	ServiceExport<DebugControl<ADDRESS> > debug_control_export;
	ServiceExport<MemoryAccessReporting<ADDRESS> > memory_access_reporting_export;
	ServiceExport<TrapReporting> trap_reporting_export;
	ServiceExport<Disassembly<ADDRESS> > disasm_export;
	ServiceExport<Memory<ADDRESS> > memory_export;
	ServiceExport<Registers> registers_export;
	ServiceExport<SymbolTableLookup<ADDRESS> > symbol_table_lookup_export;
	ServiceExport<StatementLookup<ADDRESS> > stmt_lookup_export;
	ServiceExport<BackTrace<ADDRESS> > backtrace_export;
	ServiceExport<DebugInfoLoading> debug_info_loading_export;
	
	ServiceImport<DebugEventListener<ADDRESS> > debug_event_listener_import;
	ServiceImport<DebugControl<ADDRESS> > debug_control_import;
	ServiceImport<Disassembly<ADDRESS> > disasm_import;
	ServiceImport<Memory<ADDRESS> > memory_import;
	ServiceImport<MemoryAccessReportingControl> memory_access_reporting_control_import;
	ServiceImport<TrapReporting> trap_reporting_import;
	ServiceImport<Registers> registers_import;
	ServiceImport<Loader> loader_import;
	ServiceImport<unisim::service::interfaces::Blob<ADDRESS> > blob_import;

	Debugger(const char *name, Object *parent = 0);
	virtual ~Debugger();
	
	virtual bool BeginSetup();
	virtual bool Setup(unisim::kernel::service::ServiceExportBase *srv_export);
	virtual bool EndSetup();

	virtual typename unisim::service::interfaces::DebugControl<ADDRESS>::DebugCommand FetchDebugCommand(ADDRESS cia);

	virtual bool Listen(const unisim::util::debug::Event<ADDRESS>& event);
	virtual bool Unlisten(const unisim::util::debug::Event<ADDRESS>& event);
	virtual bool IsEventListened(const unisim::util::debug::Event<ADDRESS>& event) const;
	virtual void EnumerateListenedEvents(std::list<const unisim::util::debug::Event<ADDRESS> *>& lst, typename unisim::util::debug::Event<ADDRESS>::Type ev_type = unisim::util::debug::Event<ADDRESS>::EV_UNKNOWN) const;
	virtual std::string Disasm(ADDRESS addr, ADDRESS& next_addr);
	virtual void Reset();
	virtual bool ReadMemory(ADDRESS addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(ADDRESS addr, const void *buffer, uint32_t size);
	virtual unisim::util::debug::Register *GetRegister(const char *name);
	
	virtual void ReportMemoryAccess(typename MemoryAccessReporting<ADDRESS>::MemoryAccessType mat, typename MemoryAccessReporting<ADDRESS>::MemoryType mt, ADDRESS addr, uint32_t size);
	virtual void ReportFinishedInstruction(ADDRESS addr, ADDRESS next_addr);
	
	virtual void ReportTrap();
	virtual void ReportTrap(const unisim::kernel::service::Object &obj);
	virtual void ReportTrap(const unisim::kernel::service::Object &obj,
							const std::string &str);
	virtual void ReportTrap(const unisim::kernel::service::Object &obj,
							const char *c_str);

	virtual void GetSymbols(typename std::list<const unisim::util::debug::Symbol<ADDRESS> *>& lst, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const;
	virtual const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbol(const char *name, ADDRESS addr, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const;
	virtual const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbolByAddr(ADDRESS addr) const;
	virtual const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbolByName(const char *name) const;
	virtual const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbolByName(const char *name, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const;
	virtual const typename unisim::util::debug::Symbol<ADDRESS> *FindSymbolByAddr(ADDRESS addr, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const;

	virtual void GetStatements(std::map<ADDRESS, const unisim::util::debug::Statement<ADDRESS> *>& stmts) const;
	virtual const unisim::util::debug::Statement<ADDRESS> *FindStatement(ADDRESS addr, typename unisim::service::interfaces::StatementLookup<ADDRESS>::FindStatementOption opt) const;
	virtual const unisim::util::debug::Statement<ADDRESS> *FindStatement(const char *filename, unsigned int lineno, unsigned int colno) const;

	virtual std::vector<ADDRESS> *GetBackTrace(ADDRESS pc) const;
	virtual bool GetReturnAddress(ADDRESS pc, ADDRESS& ret_addr) const;
	
	virtual bool LoadDebugInfo(const char *filename);
	virtual bool EnableBinary(const char *filename, bool enable);
	virtual void EnumerateBinaries(std::list<std::string>& lst) const;
	virtual bool IsBinaryEnabled(const char *filename) const;
private:
	bool verbose;
	std::string dwarf_to_html_output_directory;
	std::string dwarf_register_number_mapping_filename;
	bool parse_dwarf;
	std::string filename;

	Parameter<bool> param_verbose;
	Parameter<std::string> param_dwarf_to_html_output_directory;
	Parameter<std::string> param_dwarf_register_number_mapping_filename;
	Parameter<bool> param_parse_dwarf;

	unisim::kernel::logger::Logger logger;
	bool setup_debug_info_done;

	unisim::util::debug::BreakpointRegistry<ADDRESS> breakpoint_registry;
	unisim::util::debug::WatchpointRegistry<ADDRESS> watchpoint_registry;
	std::vector<unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *> elf32_loaders;
	std::vector<unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *> elf64_loaders;
	std::vector<unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *> coff_loaders;
	std::vector<bool> enable_elf32_loaders;
	std::vector<bool> enable_elf64_loaders;
	std::vector<bool> enable_coff_loaders;
	
	void SetupELFSymtab(const typename unisim::util::debug::blob::Blob<ADDRESS> *blob);
	void SetupDWARF(const typename unisim::util::debug::blob::Blob<ADDRESS> *blob);
	
	bool SetupDebugInfo(const unisim::util::debug::blob::Blob<ADDRESS> *blob);
	bool SetupDebugInfo();
};

} // end of namespace debugger
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif
