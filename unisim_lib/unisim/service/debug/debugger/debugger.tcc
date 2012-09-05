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

#ifndef __UNISIM_SERVICE_DEBUG_DEBUGGER_DEBUGGER_TCC__
#define __UNISIM_SERVICE_DEBUG_DEBUGGER_DEBUGGER_TCC__

#include <stdexcept>
#include <fstream>

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
	
template <class ADDRESS>
Debugger<ADDRESS>::Debugger(const char *name, Object *parent)
	: Object(name, parent)
	, Service<DebugEventTrigger<ADDRESS> >(name, parent)
	, Service<DebugControl<ADDRESS> >(name, parent)
	, Service<MemoryAccessReporting<ADDRESS> >(name, parent)
	, Service<TrapReporting>(name, parent)
	, Service<Disassembly<ADDRESS> >(name, parent)
	, Service<Memory<ADDRESS> >(name, parent)
	, Service<Registers>(name, parent)
	, Service<SymbolTableLookup<ADDRESS> >(name, parent)
	, Service<StatementLookup<ADDRESS> >(name, parent)
	, Service<BackTrace<ADDRESS> >(name, parent)
	, Service<DebugInfoLoading>(name, parent)
	, Client<DebugEventListener<ADDRESS> >(name, parent)
	, Client<DebugControl<ADDRESS> >(name, parent)
	, Client<MemoryAccessReportingControl>(name, parent)
	, Client<TrapReporting>(name, parent)
	, Client<Disassembly<ADDRESS> >(name, parent)
	, Client<Memory<ADDRESS> >(name, parent)
	, Client<Registers>(name, parent)
	, Client<Loader>(name, parent)
	, Client<unisim::service::interfaces::Blob<ADDRESS> >(name, parent)
	, debug_event_trigger_export("debug-event-trigger-export", this)
	, debug_control_export("debug-control-export", this)
	, memory_access_reporting_export("memory-access-reporting-export", this)
	, trap_reporting_export("trap-reporting-export", this)
	, disasm_export("disasm-export", this)
	, memory_export("memory-export", this)
	, registers_export("registers-export", this)
	, symbol_table_lookup_export("symbol-table-lookup-export", this)
	, stmt_lookup_export("stmt-lookup-export", this)
	, backtrace_export("backtrace-export", this)
	, debug_info_loading_export("debug-info-loading-export", this)
	, debug_event_listener_import("debug-event-listener", this)
	, debug_control_import("debug-control-import", this)
	, disasm_import("disasm-import", this)
	, memory_import("memory-import", this)
	, memory_access_reporting_control_import("memory-access-reporting-control-import", this)
	, trap_reporting_import("trap-reporting-import", this)
	, registers_import("registers-import", this)
	, loader_import("loader-import", this)
	, blob_import("blob-import", this)
	, verbose(false)
	, dwarf_to_html_output_directory()
	, dwarf_register_number_mapping_filename()
	, parse_dwarf(false)
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, param_dwarf_to_html_output_directory("dwarf-to-html-output-directory", this, dwarf_to_html_output_directory, "DWARF v2/v3 to HTML output directory")
	, param_dwarf_register_number_mapping_filename("dwarf-register-number-mapping-filename", this, dwarf_register_number_mapping_filename, "DWARF register number mapping filename")
	, param_parse_dwarf("parse-dwarf", this, parse_dwarf, "Enable/Disable parsing of DWARF debugging informations")
	, logger(*this)
	, setup_debug_info_done(false)
{
	backtrace_export.SetupDependsOn(registers_import);
	backtrace_export.SetupDependsOn(memory_import);
}

template <class ADDRESS>
Debugger<ADDRESS>::~Debugger()
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		delete elf32_loaders[i];
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		delete elf64_loaders[i];
	}

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; i++)
	{
		delete coff_loaders[i];
	}
}

template <class ADDRESS>
bool Debugger<ADDRESS>::BeginSetup()
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		delete elf32_loaders[i];
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		delete elf64_loaders[i];
	}

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; i++)
	{
		delete coff_loaders[i];
	}
	
	setup_debug_info_done = false;
	
	return true;
}

template <class ADDRESS>
bool Debugger<ADDRESS>::Setup(unisim::kernel::service::ServiceExportBase *srv_export)
{
	if(srv_export == &debug_event_trigger_export) return true;
	if(srv_export == &debug_control_export) return true;
	if(srv_export == &memory_access_reporting_export) return true;
	if(srv_export == &trap_reporting_export) return true;
	if(srv_export == &disasm_export) return true;
	if(srv_export == &memory_export) return true;
	if(srv_export == &registers_export) return true;
	if(srv_export == &debug_info_loading_export) return true;
	if(srv_export == &symbol_table_lookup_export) return SetupDebugInfo();
	if(srv_export == &stmt_lookup_export) return SetupDebugInfo();
	if(srv_export == &backtrace_export) return SetupDebugInfo();

	logger << DebugError << "Internal Error" << EndDebugError;
	return false;
}

template <class ADDRESS>
bool Debugger<ADDRESS>::EndSetup()
{
	return true;
}

template <class ADDRESS>
typename unisim::service::interfaces::DebugControl<ADDRESS>::DebugCommand Debugger<ADDRESS>::FetchDebugCommand(ADDRESS cia)
{
	return debug_control_import ? debug_control_import->FetchDebugCommand(cia) : unisim::service::interfaces::DebugControl<ADDRESS>::DBG_STEP;
}

template <class ADDRESS>
bool Debugger<ADDRESS>::Listen(const typename unisim::util::debug::Event<ADDRESS>& event)
{
	switch(event.GetType())
	{
		case unisim::util::debug::Event<ADDRESS>::EV_BREAKPOINT:
			{
				const typename unisim::util::debug::Breakpoint<ADDRESS> *brkp = (const typename unisim::util::debug::Breakpoint<ADDRESS> *) &event;
				
				if(!breakpoint_registry.SetBreakpoint(*brkp)) return false;
				
				if(memory_access_reporting_control_import)
					memory_access_reporting_control_import->RequiresFinishedInstructionReporting(
							breakpoint_registry.HasBreakpoints());
			}
			break;
		case unisim::util::debug::Event<ADDRESS>::EV_WATCHPOINT:
			{
				const typename unisim::util::debug::Watchpoint<ADDRESS> *wp = (const typename unisim::util::debug::Watchpoint<ADDRESS> *) &event;
				
				if(!watchpoint_registry.SetWatchpoint(*wp)) return false;
				
				if(memory_access_reporting_control_import)
					memory_access_reporting_control_import->RequiresMemoryAccessReporting(
							watchpoint_registry.HasWatchpoints());
			}
			break;
		case unisim::util::debug::Event<ADDRESS>::EV_UNKNOWN:
			break;
			
	}
	return true;
}

template <class ADDRESS>
bool Debugger<ADDRESS>::Unlisten(const typename unisim::util::debug::Event<ADDRESS>& event)
{
	switch(event.GetType())
	{
		case unisim::util::debug::Event<ADDRESS>::EV_BREAKPOINT:
			{
				const typename unisim::util::debug::Breakpoint<ADDRESS> *brkp = (const typename unisim::util::debug::Breakpoint<ADDRESS> *) &event;
				
				if(!breakpoint_registry.RemoveBreakpoint(*brkp)) return false;
				
				if(memory_access_reporting_control_import)
					memory_access_reporting_control_import->RequiresFinishedInstructionReporting(
							breakpoint_registry.HasBreakpoints());
			}
			break;
		case unisim::util::debug::Event<ADDRESS>::EV_WATCHPOINT:
			{
				const typename unisim::util::debug::Watchpoint<ADDRESS> *wp = (const typename unisim::util::debug::Watchpoint<ADDRESS> *) &event;
				
				if(!watchpoint_registry.RemoveWatchpoint(*wp)) return false;
				
				if(memory_access_reporting_control_import)
					memory_access_reporting_control_import->RequiresMemoryAccessReporting(
							watchpoint_registry.HasWatchpoints());
			}
			break;
		case unisim::util::debug::Event<ADDRESS>::EV_UNKNOWN:
			break;
			
	}
	return true;
}

template <class ADDRESS>
void Debugger<ADDRESS>::EnumerateListenedEvents(std::list<const unisim::util::debug::Event<ADDRESS> *>& lst, typename unisim::util::debug::Event<ADDRESS>::Type ev_type) const
{
	lst.clear();
	if((ev_type == unisim::util::debug::Event<ADDRESS>::EV_UNKNOWN) || (ev_type == unisim::util::debug::Event<ADDRESS>::EV_BREAKPOINT))
	{
		const std::list<unisim::util::debug::Breakpoint<ADDRESS> >& breakpoints = breakpoint_registry.GetBreakpoints();
		typename std::list<unisim::util::debug::Breakpoint<ADDRESS> >::const_iterator brkp_it;
		for(brkp_it = breakpoints.begin(); brkp_it != breakpoints.end(); brkp_it++)
		{
			lst.push_back(&(*brkp_it));
		}
	}
	if((ev_type == unisim::util::debug::Event<ADDRESS>::EV_UNKNOWN) || (ev_type == unisim::util::debug::Event<ADDRESS>::EV_WATCHPOINT))
	{
		const std::list<unisim::util::debug::Watchpoint<ADDRESS> >& watchpoints = watchpoint_registry.GetWatchpoints();
		typename std::list<unisim::util::debug::Watchpoint<ADDRESS> >::const_iterator wt_it;
		for(wt_it = watchpoints.begin(); wt_it != watchpoints.end(); wt_it++)
		{
			lst.push_back(&(*wt_it));
		}
	}
}

template <class ADDRESS>
bool Debugger<ADDRESS>::IsEventListened(const unisim::util::debug::Event<ADDRESS>& event) const
{
	switch(event.GetType())
	{
		case unisim::util::debug::Event<ADDRESS>::EV_BREAKPOINT:
			{
				const typename unisim::util::debug::Breakpoint<ADDRESS> *brkp = (const typename unisim::util::debug::Breakpoint<ADDRESS> *) &event;
				
				return breakpoint_registry.HasBreakpoint(*brkp);
			}
			break;
		case unisim::util::debug::Event<ADDRESS>::EV_WATCHPOINT:
			{
				const typename unisim::util::debug::Watchpoint<ADDRESS> *wp = (const typename unisim::util::debug::Watchpoint<ADDRESS> *) &event;
				
				return watchpoint_registry.HasWatchpoint(*wp);
			}
			break;
		case unisim::util::debug::Event<ADDRESS>::EV_UNKNOWN:
			return false;
			
	}
	return false;
}

template <class ADDRESS>
std::string Debugger<ADDRESS>::Disasm(ADDRESS addr, ADDRESS& next_addr)
{
	return disasm_import ? disasm_import->Disasm(addr, next_addr) : std::string();
}

template <class ADDRESS>
void Debugger<ADDRESS>::Reset()
{
}

template <class ADDRESS>
bool Debugger<ADDRESS>::ReadMemory(ADDRESS addr, void *buffer, uint32_t size)
{
	return memory_import ? memory_import->ReadMemory(addr, buffer, size) : false;
}

template <class ADDRESS>
bool Debugger<ADDRESS>::WriteMemory(ADDRESS addr, const void *buffer, uint32_t size)
{
	return memory_import ? memory_import->WriteMemory(addr, buffer, size) : false;
}

template <class ADDRESS>
unisim::util::debug::Register *Debugger<ADDRESS>::GetRegister(const char *name)
{
	return registers_import ? registers_import->GetRegister(name) : 0;
}

template <class ADDRESS>
void Debugger<ADDRESS>::ReportMemoryAccess(unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size)
{
	if(watchpoint_registry.HasWatchpoint(mat, mt, addr, size))
	{
		const typename unisim::util::debug::Watchpoint<ADDRESS> *watchpoint = watchpoint_registry.FindWatchpoint(mat, mt, addr, size);
		
		if(!watchpoint) throw std::runtime_error("Internal error");
		
		if(debug_event_listener_import) debug_event_listener_import->OnDebugEvent(*watchpoint);
	}
}

template <class ADDRESS>
void Debugger<ADDRESS>::ReportFinishedInstruction(ADDRESS addr, ADDRESS next_addr)
{
	if(breakpoint_registry.HasBreakpoint(next_addr))
	{
		const typename unisim::util::debug::Breakpoint<ADDRESS> *breakpoint = breakpoint_registry.FindBreakpoint(next_addr);
		
		if(!breakpoint) throw std::runtime_error("Internal error");
		
		if(debug_event_listener_import) debug_event_listener_import->OnDebugEvent(*breakpoint);
	}
}

template <class ADDRESS>
void Debugger<ADDRESS>::ReportTrap()
{
	if(trap_reporting_import) trap_reporting_import->ReportTrap();
}

template <class ADDRESS>
void Debugger<ADDRESS>::ReportTrap(const unisim::kernel::service::Object &obj)
{
	if(trap_reporting_import) trap_reporting_import->ReportTrap(obj);
}

template <class ADDRESS>
void Debugger<ADDRESS>::ReportTrap(const unisim::kernel::service::Object &obj,
						const std::string &str)
{
	if(trap_reporting_import) trap_reporting_import->ReportTrap(obj, str);
}

template <class ADDRESS>
void Debugger<ADDRESS>::ReportTrap(const unisim::kernel::service::Object &obj,
						const char *c_str)
{
	if(trap_reporting_import) trap_reporting_import->ReportTrap(obj, c_str);
}

template <class ADDRESS>
bool Debugger<ADDRESS>::EnableBinary(const char *filename, bool enable)
{
	bool found = false;
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
		const unisim::util::debug::blob::Blob<ADDRESS> *blob = elf32_loader->GetBlob();
		if(blob->GetCapability() & unisim::util::debug::blob::CAP_FILENAME)
		{
			if(strcmp(blob->GetFilename(), filename) == 0)
			{
				enable_elf32_loaders[i] = enable;
				found = true;
			}
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
		const unisim::util::debug::blob::Blob<ADDRESS> *blob = elf64_loader->GetBlob();
		if(blob->GetCapability() & unisim::util::debug::blob::CAP_FILENAME)
		{
			if(strcmp(blob->GetFilename(), filename) == 0)
			{
				enable_elf64_loaders[i] = enable;
				found = true;
			}
		}
	}

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; i++)
	{
		typename unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = coff_loaders[i];
		const unisim::util::debug::blob::Blob<ADDRESS> *blob = coff_loader->GetBlob();
		if(blob->GetCapability() & unisim::util::debug::blob::CAP_FILENAME)
		{
			if(strcmp(blob->GetFilename(), filename) == 0)
			{
				enable_coff_loaders[i] = enable;
				found = true;
			}
		}
	}
	
	return found;
}

template <class ADDRESS>
void Debugger<ADDRESS>::EnumerateBinaries(std::list<std::string>& lst) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
		const unisim::util::debug::blob::Blob<ADDRESS> *blob = elf32_loader->GetBlob();
		if(blob->GetCapability() & unisim::util::debug::blob::CAP_FILENAME)
		{
			lst.push_back(std::string(blob->GetFilename()));
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
		const unisim::util::debug::blob::Blob<ADDRESS> *blob = elf64_loader->GetBlob();
		if(blob->GetCapability() & unisim::util::debug::blob::CAP_FILENAME)
		{
			lst.push_back(std::string(blob->GetFilename()));
		}
	}

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; i++)
	{
		typename unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = coff_loaders[i];
		const unisim::util::debug::blob::Blob<ADDRESS> *blob = coff_loader->GetBlob();
		if(blob->GetCapability() & unisim::util::debug::blob::CAP_FILENAME)
		{
			lst.push_back(std::string(blob->GetFilename()));
		}
	}
}

template <class ADDRESS>
bool Debugger<ADDRESS>::IsBinaryEnabled(const char *filename) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
		const unisim::util::debug::blob::Blob<ADDRESS> *blob = elf32_loader->GetBlob();
		if(blob->GetCapability() & unisim::util::debug::blob::CAP_FILENAME)
		{
			if(strcmp(blob->GetFilename(), filename) == 0)
			{
				return enable_elf32_loaders[i];
			}
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
		const unisim::util::debug::blob::Blob<ADDRESS> *blob = elf64_loader->GetBlob();
		if(blob->GetCapability() & unisim::util::debug::blob::CAP_FILENAME)
		{
			if(strcmp(blob->GetFilename(), filename) == 0)
			{
				return enable_elf64_loaders[i];
			}
		}
	}

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; i++)
	{
		typename unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = coff_loaders[i];
		const unisim::util::debug::blob::Blob<ADDRESS> *blob = coff_loader->GetBlob();
		if(blob->GetCapability() & unisim::util::debug::blob::CAP_FILENAME)
		{
			if(strcmp(blob->GetFilename(), filename) == 0)
			{
				return enable_coff_loaders[i];
			}
		}
	}
	
	return false;
}

template <class ADDRESS>
void Debugger<ADDRESS>::GetSymbols(typename std::list<const unisim::util::debug::Symbol<ADDRESS> *>& lst, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if(enable_elf32_loaders[i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			elf32_loader->GetSymbols(lst, type);
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if(enable_elf64_loaders[i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			elf64_loader->GetSymbols(lst, type);
		}
	}

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; i++)
	{
		if(enable_coff_loaders[i])
		{
			typename unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = coff_loaders[i];
			coff_loader->GetSymbols(lst, type);
		}
	}
}

template <class ADDRESS>
const typename unisim::util::debug::Symbol<ADDRESS> *Debugger<ADDRESS>::FindSymbol(const char *name, ADDRESS addr, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if(enable_elf32_loaders[i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = elf32_loader->FindSymbol(name, addr, type);
			if(symbol) return symbol;
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if(enable_elf64_loaders[i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = elf64_loader->FindSymbol(name, addr, type);
			if(symbol) return symbol;
		}
	}

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; i++)
	{
		if(enable_coff_loaders[i])
		{
			typename unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = coff_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = coff_loader->FindSymbol(name, addr, type);
			if(symbol) return symbol;
		}
	}
	return 0;
}

template <class ADDRESS>
const typename unisim::util::debug::Symbol<ADDRESS> *Debugger<ADDRESS>::FindSymbolByAddr(ADDRESS addr) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if(enable_elf32_loaders[i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = elf32_loader->FindSymbolByAddr(addr);
			if(symbol) return symbol;
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if(enable_elf64_loaders[i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = elf64_loader->FindSymbolByAddr(addr);
			if(symbol) return symbol;
		}
	}

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; i++)
	{
		if(enable_coff_loaders[i])
		{
			typename unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = coff_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = coff_loader->FindSymbolByAddr(addr);
			if(symbol) return symbol;
		}
	}
	return 0;
}

template <class ADDRESS>
const typename unisim::util::debug::Symbol<ADDRESS> *Debugger<ADDRESS>::FindSymbolByName(const char *name) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if(enable_elf32_loaders[i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = elf32_loader->FindSymbolByName(name);
			if(symbol) return symbol;
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if(enable_elf64_loaders[i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = elf64_loader->FindSymbolByName(name);
			if(symbol) return symbol;
		}
	}

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; i++)
	{
		if(enable_coff_loaders[i])
		{
			typename unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = coff_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = coff_loader->FindSymbolByName(name);
			if(symbol) return symbol;
		}
	}
	return 0;
}

template <class ADDRESS>
const typename unisim::util::debug::Symbol<ADDRESS> *Debugger<ADDRESS>::FindSymbolByName(const char *name, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if(enable_elf32_loaders[i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = elf32_loader->FindSymbolByName(name, type);
			if(symbol) return symbol;
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if(enable_elf64_loaders[i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = elf64_loader->FindSymbolByName(name, type);
			if(symbol) return symbol;
		}
	}

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; i++)
	{
		if(enable_coff_loaders[i])
		{
			typename unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = coff_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = coff_loader->FindSymbolByName(name, type);
			if(symbol) return symbol;
		}
	}
	return 0;
}

template <class ADDRESS>
const typename unisim::util::debug::Symbol<ADDRESS> *Debugger<ADDRESS>::FindSymbolByAddr(ADDRESS addr, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if(enable_elf32_loaders[i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = elf32_loader->FindSymbolByAddr(addr, type);
			if(symbol) return symbol;
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if(enable_elf64_loaders[i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = elf64_loader->FindSymbolByAddr(addr, type);
			if(symbol) return symbol;
		}
	}

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; i++)
	{
		if(enable_coff_loaders[i])
		{
			typename unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = coff_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = coff_loader->FindSymbolByAddr(addr, type);
			if(symbol) return symbol;
		}
	}
	return 0;
}

template <class ADDRESS>
void Debugger<ADDRESS>::GetStatements(std::map<ADDRESS, const unisim::util::debug::Statement<ADDRESS> *>& stmts) const
{
	typename std::map<ADDRESS, const unisim::util::debug::Statement<ADDRESS> *>::const_iterator iter;
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if(enable_elf32_loaders[i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			const typename std::map<ADDRESS, const unisim::util::debug::Statement<ADDRESS> *>& elf32_stmts = elf32_loader->GetStatements();
			for(iter = elf32_stmts.begin(); iter != elf32_stmts.end(); iter++)
			{
				stmts.insert(std::pair<ADDRESS, const unisim::util::debug::Statement<ADDRESS> *>((*iter).first, (*iter).second));
			}
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if(enable_elf64_loaders[i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			const typename std::map<ADDRESS, const unisim::util::debug::Statement<ADDRESS> *>& elf64_stmts = elf64_loader->GetStatements();
			for(iter = elf64_stmts.begin(); iter != elf64_stmts.end(); iter++)
			{
				stmts.insert(std::pair<ADDRESS, const unisim::util::debug::Statement<ADDRESS> *>((*iter).first, (*iter).second));
			}
		}
	}
}

template <class ADDRESS>
const typename unisim::util::debug::Statement<ADDRESS> *Debugger<ADDRESS>::FindStatement(ADDRESS addr, typename unisim::service::interfaces::StatementLookup<ADDRESS>::FindStatementOption opt) const
{
	const unisim::util::debug::Statement<ADDRESS> *ret_stmt = 0;
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if(enable_elf32_loaders[i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			const typename unisim::util::debug::Statement<ADDRESS> *stmt = elf32_loader->FindStatement(addr, opt);
			if(stmt)
			{
				switch(opt)
				{
					case unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_NEAREST_LOWER_OR_EQUAL_STMT:
						if(stmt->GetAddress() <= addr)
						{
							if(!ret_stmt || ((addr - stmt->GetAddress()) < (addr - ret_stmt->GetAddress()))) ret_stmt = stmt;
						}
						break;
					case unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_NEXT_STMT:
						if(stmt->GetAddress() > addr)
						{
							if(!ret_stmt || ((stmt->GetAddress() - addr) < (ret_stmt->GetAddress() - addr))) ret_stmt = stmt;
						}
						break;
					case unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_EXACT_STMT:
						return stmt;
						break;
				}
			}
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if(enable_elf64_loaders[i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			const typename unisim::util::debug::Statement<ADDRESS> *stmt = elf64_loader->FindStatement(addr, opt);
			if(stmt)
			{
				switch(opt)
				{
					case unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_NEAREST_LOWER_OR_EQUAL_STMT:
						if(stmt->GetAddress() <= addr)
						{
							if(!ret_stmt || ((addr - stmt->GetAddress()) < (addr - ret_stmt->GetAddress()))) ret_stmt = stmt;
						}
						break;
					case unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_NEXT_STMT:
						if(stmt->GetAddress() > addr)
						{
							if(!ret_stmt || ((stmt->GetAddress() - addr) < (ret_stmt->GetAddress() - addr))) ret_stmt = stmt;
						}
						break;
					case unisim::service::interfaces::StatementLookup<ADDRESS>::OPT_FIND_EXACT_STMT:
						return stmt;
						break;
				}
			}
		}
	}
	
	return ret_stmt;
}

template <class ADDRESS>
const unisim::util::debug::Statement<ADDRESS> *Debugger<ADDRESS>::FindStatement(const char *filename, unsigned int lineno, unsigned int colno) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if(enable_elf32_loaders[i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			const typename unisim::util::debug::Statement<ADDRESS> *stmt = elf32_loader->FindStatement(filename, lineno, colno);
			if(stmt) return stmt;
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if(enable_elf64_loaders[i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			const typename unisim::util::debug::Statement<ADDRESS> *stmt = elf64_loader->FindStatement(filename, lineno, colno);
			if(stmt) return stmt;
		}
	}
	return 0;
}

template <class ADDRESS>
std::vector<ADDRESS> *Debugger<ADDRESS>::GetBackTrace(ADDRESS pc) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if(enable_elf32_loaders[i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			std::vector<ADDRESS> *backtrace = elf32_loader->GetBackTrace(pc);
			if(backtrace) return backtrace;
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if(enable_elf64_loaders[i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			std::vector<ADDRESS> *backtrace = elf64_loader->GetBackTrace(pc);
			if(backtrace) return backtrace;
		}
	}
	
	return 0;
}

template <class ADDRESS>
bool Debugger<ADDRESS>::GetReturnAddress(ADDRESS pc, ADDRESS& ret_addr) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if(enable_elf32_loaders[i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			if(elf32_loader->GetReturnAddress(pc, ret_addr)) return true;
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if(enable_elf64_loaders[i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			if(elf64_loader->GetReturnAddress(pc, ret_addr)) return true;
		}
	}
	
	return false;
}
template <class ADDRESS>
bool Debugger<ADDRESS>::LoadDebugInfo(const char *filename)
{
	uint8_t magic[8];
	
	std::ifstream f(filename, std::ifstream::in | std::ifstream::binary);
	
	if(f.fail())
	{
		logger << DebugError << "Can't open input \"" << filename << "\"" << EndDebugError;
		return false;
	}

	// Note: code below is nearly equivalent to istream::readsome
	// I no longer use it because it is bugged with i586-mingw32msvc-g++ (version 4.2.1-sjlj on Ubuntu)
	unsigned int size;
	for(size = 0; size < sizeof(magic); size++)
	{
		f.read((char *) &magic[size], 1);
		if(!f.good()) break;
	}
	
	if(size >= 2)
	{
		if(((magic[0] == 0xc1) && (magic[1] == 0x00))
		|| ((magic[0] == 0xc2) && (magic[1] == 0x00))
		|| ((magic[0] == 0x92) && (magic[1] == 0x00))
		|| ((magic[0] == 0x93) && (magic[1] == 0x00))
		|| ((magic[0] == 0x95) && (magic[1] == 0x00))
		|| ((magic[0] == 0x98) && (magic[1] == 0x00))
		|| ((magic[0] == 0x99) && (magic[1] == 0x00))
		|| ((magic[0] == 0x9d) && (magic[1] == 0x00))
		|| ((magic[0] == 0x00) && (magic[1] == 0xc1))
		|| ((magic[0] == 0x00) && (magic[1] == 0xc2))
		|| ((magic[0] == 0x00) && (magic[1] == 0x92))
		|| ((magic[0] == 0x00) && (magic[1] == 0x93))
		|| ((magic[0] == 0x00) && (magic[1] == 0x95))
		|| ((magic[0] == 0x00) && (magic[1] == 0x98))
		|| ((magic[0] == 0x00) && (magic[1] == 0x99))
		|| ((magic[0] == 0x00) && (magic[1] == 0x9d)))
		{
			// TI COFF file detected
			unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = new unisim::util::loader::coff_loader::CoffLoader<ADDRESS>(logger);
			
			coff_loader->SetOption(unisim::util::loader::coff_loader::OPT_FILENAME, filename);
			coff_loader->SetOption(unisim::util::loader::coff_loader::OPT_VERBOSE, verbose);
			
			if(!coff_loader->Load())
			{
				logger << DebugError << "Loading input \"" << filename << "\" failed" << EndDebugError;
				delete coff_loader;
				return false;
			}
			coff_loaders.push_back(coff_loader);
			enable_coff_loaders.push_back(true);
			return true;
		}
	}

	if(size >= 5)
	{
		if((magic[0] == 0x7f) && (magic[1] == 'E') && (magic[2] == 'L') && (magic[3] == 'F'))
		{
			// ELF file detected
			switch(magic[4])
			{
				case 1:
					{
						unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = new unisim::util::loader::elf_loader::Elf32Loader<ADDRESS>(logger, registers_import, memory_import);
						
						elf32_loader->SetOption(unisim::util::loader::elf_loader::OPT_FILENAME, filename);
						elf32_loader->SetOption(unisim::util::loader::elf_loader::OPT_VERBOSE, verbose);
						elf32_loader->SetOption(unisim::util::loader::elf_loader::OPT_PARSE_DWARF, parse_dwarf);
						elf32_loader->SetOption(unisim::util::loader::elf_loader::OPT_DWARF_REGISTER_NUMBER_MAPPING_FILENAME, Object::GetSimulator()->SearchSharedDataFile(dwarf_register_number_mapping_filename.c_str()).c_str());
						
						if(!elf32_loader->Load())
						{
							logger << DebugError << "Loading input \"" << filename << "\" failed" << EndDebugError;
							delete elf32_loader;
							return false;
						}
						elf32_loaders.push_back(elf32_loader);
						enable_elf32_loaders.push_back(true);
						return true;
					}
					break;
				case 2:
					{
						unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = new unisim::util::loader::elf_loader::Elf64Loader<ADDRESS>(logger, registers_import, memory_import);
						
						elf64_loader->SetOption(unisim::util::loader::elf_loader::OPT_FILENAME, filename);
						elf64_loader->SetOption(unisim::util::loader::elf_loader::OPT_VERBOSE, verbose);
						elf64_loader->SetOption(unisim::util::loader::elf_loader::OPT_PARSE_DWARF, parse_dwarf);
						elf64_loader->SetOption(unisim::util::loader::elf_loader::OPT_DWARF_REGISTER_NUMBER_MAPPING_FILENAME, Object::GetSimulator()->SearchSharedDataFile(dwarf_register_number_mapping_filename.c_str()).c_str());

						if(!elf64_loader->Load())
						{
							logger << DebugError << "Loading input \"" << filename << "\" failed" << EndDebugError;
							delete elf64_loader;
							return false;
						}
						elf64_loaders.push_back(elf64_loader);
						enable_elf64_loaders.push_back(true);
						return true;
					}
					break;
			}
		}
	}
	logger << DebugWarning << "Can't handle symbol table of input \"" << filename << "\"" << EndDebugWarning;
	return false;
}

template <class ADDRESS>
bool Debugger<ADDRESS>::SetupDebugInfo(const unisim::util::debug::blob::Blob<ADDRESS> *blob)
{
	typename unisim::util::debug::blob::FileFormat ffmt = blob->GetFileFormat();
	
	switch(ffmt)
	{
		case unisim::util::debug::blob::FFMT_UNKNOWN:
			break;
		case unisim::util::debug::blob::FFMT_ELF32:
			{
				unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = new unisim::util::loader::elf_loader::Elf32Loader<ADDRESS>(logger, registers_import, memory_import, blob);
				
				elf32_loader->SetOption(unisim::util::loader::elf_loader::OPT_PARSE_DWARF, parse_dwarf);
				elf32_loader->SetOption(unisim::util::loader::elf_loader::OPT_VERBOSE, verbose);
				elf32_loader->SetOption(unisim::util::loader::elf_loader::OPT_DWARF_REGISTER_NUMBER_MAPPING_FILENAME, Object::GetSimulator()->SearchSharedDataFile(dwarf_register_number_mapping_filename.c_str()).c_str());

				elf32_loader->ParseSymbols();
				elf32_loaders.push_back(elf32_loader);
				enable_elf32_loaders.push_back(true);
			}
			break;
		case unisim::util::debug::blob::FFMT_ELF64:
			{
				unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = new unisim::util::loader::elf_loader::Elf64Loader<ADDRESS>(logger, registers_import, memory_import, blob);
				
				elf64_loader->SetOption(unisim::util::loader::elf_loader::OPT_PARSE_DWARF, parse_dwarf);
				elf64_loader->SetOption(unisim::util::loader::elf_loader::OPT_VERBOSE, verbose);
				elf64_loader->SetOption(unisim::util::loader::elf_loader::OPT_DWARF_REGISTER_NUMBER_MAPPING_FILENAME, Object::GetSimulator()->SearchSharedDataFile(dwarf_register_number_mapping_filename.c_str()).c_str());

				elf64_loader->ParseSymbols();
				elf64_loaders.push_back(elf64_loader);
				enable_elf64_loaders.push_back(true);
			}
			break;
		case unisim::util::debug::blob::FFMT_COFF:
			{
				unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = new unisim::util::loader::coff_loader::CoffLoader<ADDRESS>(logger, blob);
				
				coff_loader->ParseSymbols();
				coff_loaders.push_back(coff_loader);
				enable_coff_loaders.push_back(true);
			}
			break;
	}
	
	const typename std::vector<const unisim::util::debug::blob::Blob<ADDRESS> *>& sibling_blobs = blob->GetBlobs();
	
	typename std::vector<const unisim::util::debug::blob::Blob<ADDRESS> *>::const_iterator it;
	for(it = sibling_blobs.begin(); it != sibling_blobs.end(); it++)
	{
		const unisim::util::debug::blob::Blob<ADDRESS> *sibling_blob = *it;
		SetupDebugInfo(sibling_blob);
	}
	return true;
}

template <class ADDRESS>
bool Debugger<ADDRESS>::SetupDebugInfo()
{
	if(setup_debug_info_done) return true;
	if(!blob_import) return false;
	const unisim::util::debug::blob::Blob<ADDRESS> *blob = blob_import->GetBlob();
	if(!blob) return false;
	bool status = SetupDebugInfo(blob);
	if(status) setup_debug_info_done = true;
	return setup_debug_info_done;
}

} // end of namespace debugger
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif
