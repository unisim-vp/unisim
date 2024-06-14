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

#include <unisim/service/debug/debugger/debugger.hh>
#include <unisim/util/debug/breakpoint_registry.tcc>
#include <unisim/util/debug/watchpoint_registry.tcc>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <stdexcept>
#include <fstream>
#include <cassert>

namespace unisim {
namespace service {
namespace debug {
namespace debugger {

template <typename CONFIG>
Debugger<CONFIG>::Debugger(const char *name, unisim::kernel::Object *parent)
	: unisim::kernel::Object(name, parent, "Debugger back-end")
	, unisim::kernel::Client<unisim::service::interfaces::Blob<ADDRESS> >(name, parent)
	, debug_yielding_export()
	, memory_access_reporting_export()
	, trap_reporting_export()
	, debug_yielding_request_export()
	, debug_selecting_export()
	, debug_event_trigger_export()
	, disasm_export()
	, memory_export()
	, registers_export()
	, symbol_table_lookup_export()
	, stmt_lookup_export()
	, backtrace_export()
	, debug_info_loading_export()
	, data_object_lookup_export()
	, subprogram_lookup_export()
	, blob_import("blob-import", this)
	, memory_access_reporting_control_import()
	, disasm_import()
	, memory_import()
	, registers_import()
	, debug_event_listener_import()
	, debug_yielding_import()
	, prc_gate()
	, front_end_gate()
	, sel_prc_gate()
	, dw_mach_state()
	, requires_fetch_instruction_reporting()
	, requires_commit_instruction_reporting()
	, requires_memory_access_reporting()
	, verbose(false)
	, dwarf_to_html_output_directory()
	, dwarf_register_number_mapping_filename()
	, parse_dwarf(false)
	, debug_dwarf(false)
	, sel_cpu()
	, architecture()
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, param_dwarf_to_html_output_directory("dwarf-to-html-output-directory", this, dwarf_to_html_output_directory, "DWARF v2/v3 to HTML output directory")
	, param_dwarf_register_number_mapping_filename("dwarf-register-number-mapping-filename", this, dwarf_register_number_mapping_filename, "DWARF register number mapping filename")
	, param_parse_dwarf("parse-dwarf", this, parse_dwarf, "Enable/Disable parsing of DWARF debugging informations")
	, param_debug_dwarf("debug-dwarf", this, debug_dwarf, "Enable/Disable debugging of DWARF")
	, param_sel_cpu("sel-cpu", this, sel_cpu, MAX_FRONT_ENDS, "CPU being debugged by front-end")
	, param_architecture("architecture", this, architecture, NUM_PROCESSORS, "CPU architecture")
	, logger(*this)
	, setup_debug_info_done(false)
	, mutex()
	, curr_front_end_num_mutex()
	, curr_front_end_num(-1)
	, breakpoint_registry()
	, watchpoint_registry()
	, fetch_insn_event_set()
	, commit_insn_event_set()
	, trap_event_set()
	, source_code_breakpoint_registry()
	, subprogram_breakpoint_registry()
	, stub_event_registry()
	, hook_event_registry()
	, next_id()
	, schedule_mutex()
	, schedule(0)
	, elf32_loaders()
	, elf64_loaders()
	, coff_loaders()
	, enable_elf32_loaders()
	, enable_elf64_loaders()
	, enable_coff_loaders()
{
	param_verbose.AddListener(this);
	param_debug_dwarf.AddListener(this);
	
	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&schedule_mutex, NULL);
	pthread_mutex_init(&curr_front_end_num_mutex, NULL);
	
	unsigned int prc_num;
	unsigned int front_end_num;
	
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
	{
		param_sel_cpu[front_end_num].SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	}
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		std::stringstream debug_yielding_export_name_sstr;
		debug_yielding_export_name_sstr << "debug-yielding-export[" << prc_num << "]";
		debug_yielding_export[prc_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::DebugYielding>(debug_yielding_export_name_sstr.str().c_str(), this);
		
		std::stringstream memory_access_reporting_export_name_sstr;
		memory_access_reporting_export_name_sstr << "memory-access-reporting-export[" << prc_num << "]";
		memory_access_reporting_export[prc_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::MemoryAccessReporting<ADDRESS> >(memory_access_reporting_export_name_sstr.str().c_str(), this);

		std::stringstream trap_reporting_export_name_sstr;
		trap_reporting_export_name_sstr << "trap-reporting-export[" << prc_num << "]";
		trap_reporting_export[prc_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::TrapReporting>(trap_reporting_export_name_sstr.str().c_str(), this);
		
		std::stringstream memory_access_reporting_control_import_name_sstr;
		memory_access_reporting_control_import_name_sstr << "memory-access-reporting-control-import[" << prc_num << "]";
		memory_access_reporting_control_import[prc_num] = new unisim::kernel::ServiceImport<unisim::service::interfaces::MemoryAccessReportingControl>(memory_access_reporting_control_import_name_sstr.str().c_str(), this);

		std::stringstream disasm_import_name_sstr;
		disasm_import_name_sstr << "disasm-import[" << prc_num << "]";
		disasm_import[prc_num] = new unisim::kernel::ServiceImport<unisim::service::interfaces::Disassembly<ADDRESS> >(disasm_import_name_sstr.str().c_str(), this);

		std::stringstream memory_import_name_sstr;
		memory_import_name_sstr << "memory-import[" << prc_num << "]";
		memory_import[prc_num] = new unisim::kernel::ServiceImport<unisim::service::interfaces::Memory<ADDRESS> >(memory_import_name_sstr.str().c_str(), this);

		std::stringstream registers_import_name_sstr;
		registers_import_name_sstr << "registers-import[" << prc_num << "]";
		registers_import[prc_num] = new unisim::kernel::ServiceImport<unisim::service::interfaces::Registers>(registers_import_name_sstr.str().c_str(), this);
		
		std::stringstream debug_timing_import_name_sstr;
		debug_timing_import_name_sstr << "debug-timing-import[" << prc_num << "]";
		debug_timing_import[prc_num] = new unisim::kernel::ServiceImport<unisim::service::interfaces::DebugTiming<TIME_TYPE> >(debug_timing_import_name_sstr.str().c_str(), this);
	}
	
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
	{
		std::stringstream debug_yielding_request_export_name_sstr;
		debug_yielding_request_export_name_sstr << "debug-yielding-request-export[" << front_end_num << "]";
		debug_yielding_request_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::DebugYieldingRequest>(debug_yielding_request_export_name_sstr.str().c_str(), this);

		std::stringstream debug_selecting_export_name_sstr;
		debug_selecting_export_name_sstr << "debug-selecting-export[" << front_end_num << "]";
		debug_selecting_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::DebugSelecting>(debug_selecting_export_name_sstr.str().c_str(), this);
		
		std::stringstream debug_event_trigger_export_name_sstr;
		debug_event_trigger_export_name_sstr << "debug-event-trigger-export[" << front_end_num << "]";
		debug_event_trigger_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::DebugEventTrigger<ADDRESS> >(debug_event_trigger_export_name_sstr.str().c_str(), this);
		
		std::stringstream disasm_export_name_sstr;
		disasm_export_name_sstr << "disasm-export[" << front_end_num << "]";
		disasm_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::Disassembly<ADDRESS> >(disasm_export_name_sstr.str().c_str(), this);
		
		std::stringstream memory_export_name_sstr;
		memory_export_name_sstr << "memory-export[" << front_end_num << "]";
		memory_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::Memory<ADDRESS> >(memory_export_name_sstr.str().c_str(), this);
		
		std::stringstream registers_export_name_sstr;
		registers_export_name_sstr << "registers-export[" << front_end_num << "]";
		registers_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::Registers>(registers_export_name_sstr.str().c_str(), this);
		
		std::stringstream symbol_table_lookup_export_name_sstr;
		symbol_table_lookup_export_name_sstr << "symbol-table-lookup-export[" << front_end_num << "]";
		symbol_table_lookup_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::SymbolTableLookup<ADDRESS> >(symbol_table_lookup_export_name_sstr.str().c_str(), this);
		
		std::stringstream stmt_lookup_export_name_sstr;
		stmt_lookup_export_name_sstr << "stmt-lookup-export[" << front_end_num << "]";
		stmt_lookup_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::StatementLookup<ADDRESS> >(stmt_lookup_export_name_sstr.str().c_str(), this);
		
		std::stringstream backtrace_export_name_sstr;
		backtrace_export_name_sstr << "backtrace-export[" << front_end_num << "]";
		backtrace_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::BackTrace<ADDRESS> >(backtrace_export_name_sstr.str().c_str(), this);
		
		std::stringstream debug_info_loading_export_name_sstr;
		debug_info_loading_export_name_sstr << "debug-info-loading-export[" << front_end_num << "]";
		debug_info_loading_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::DebugInfoLoading>(debug_info_loading_export_name_sstr.str().c_str(), this);
		
		std::stringstream data_object_lookup_export_name_sstr;
		data_object_lookup_export_name_sstr << "data-object-lookup-export[" << front_end_num << "]";
		data_object_lookup_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::DataObjectLookup<ADDRESS> >(data_object_lookup_export_name_sstr.str().c_str(), this);
		
		std::stringstream subprogram_lookup_export_name_sstr;
		subprogram_lookup_export_name_sstr << "subprogram-lookup-export[" << front_end_num << "]";
		subprogram_lookup_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::SubProgramLookup<ADDRESS> >(subprogram_lookup_export_name_sstr.str().c_str(), this);
		
		std::stringstream stack_unwinding_export_name_sstr;
		stack_unwinding_export_name_sstr << "stack-unwinding-export[" << front_end_num << "]";
		stack_unwinding_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::StackUnwinding>(stack_unwinding_export_name_sstr.str().c_str(), this);
		
		std::stringstream stubbing_export_name_sstr;
		stubbing_export_name_sstr << "stubbing-export[" << front_end_num << "]";
		stubbing_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::Stubbing<ADDRESS> >(stubbing_export_name_sstr.str().c_str(), this);
		
		std::stringstream hooking_export_name_sstr;
		hooking_export_name_sstr << "hooking-export[" << front_end_num << "]";
		hooking_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::Hooking<ADDRESS> >(hooking_export_name_sstr.str().c_str(), this);
		
		std::stringstream debug_timing_export_name_sstr;
		debug_timing_export_name_sstr << "debug-timing-export[" << front_end_num << "]";
		debug_timing_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::DebugTiming<TIME_TYPE> >(debug_timing_export_name_sstr.str().c_str(), this);
		
		std::stringstream debug_event_listener_import_name_sstr;
		debug_event_listener_import_name_sstr << "debug-event-listener-import[" << front_end_num << "]";
		debug_event_listener_import[front_end_num] = new unisim::kernel::ServiceImport<unisim::service::interfaces::DebugEventListener<ADDRESS> >(debug_event_listener_import_name_sstr.str().c_str(), this);
		
		std::stringstream debug_yielding_import_name_sstr;
		debug_yielding_import_name_sstr << "debug-yielding-import[" << front_end_num << "]";
		debug_yielding_import[front_end_num] = new unisim::kernel::ServiceImport<unisim::service::interfaces::DebugYielding>(debug_yielding_import_name_sstr.str().c_str(), this);
	}
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		std::stringstream prc_gate_name_sstr;
		prc_gate_name_sstr << "processor" << prc_num << "-gate";
		prc_gate[prc_num] = new ProcessorGate(prc_gate_name_sstr.str().c_str(), prc_num, this);
	}
	
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
	{
		std::stringstream front_end_gate_name_sstr;
		front_end_gate_name_sstr << "front-end" << front_end_num << "-gate";
		front_end_gate[front_end_num] = new FrontEndGate(front_end_gate_name_sstr.str().c_str(), front_end_num, this); // Note: Processor gates shall be created before front-end gates
	}
	
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
	{
		DebugSelect(front_end_num, sel_cpu[front_end_num]);
	}
}

template <typename CONFIG>
Debugger<CONFIG>::~Debugger()
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

	unsigned int prc_num;
	unsigned int front_end_num;

	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		if(fetch_insn_event_set[prc_num].size())
		{
			do
			{
				typename std::set<unisim::util::debug::FetchInsnEvent<ADDRESS> *>::iterator it = fetch_insn_event_set[prc_num].begin();
				unisim::util::debug::FetchInsnEvent<ADDRESS> *fetch_insn_event = *it;
				fetch_insn_event_set[prc_num].erase(it);
				fetch_insn_event->Release();
			}
			while(fetch_insn_event_set[prc_num].size());
		}

		if(commit_insn_event_set[prc_num].size())
		{
			do
			{
				typename std::set<unisim::util::debug::CommitInsnEvent<ADDRESS> *>::iterator it = commit_insn_event_set[prc_num].begin();
				unisim::util::debug::CommitInsnEvent<ADDRESS> *commit_insn_event = *it;
				commit_insn_event_set[prc_num].erase(it);
				commit_insn_event->Release();
			}
			while(commit_insn_event_set[prc_num].size());
		}

		if(trap_event_set[prc_num].size())
		{
			do
			{
				typename std::set<unisim::util::debug::TrapEvent<ADDRESS> *>::iterator it = trap_event_set[prc_num].begin();
				unisim::util::debug::TrapEvent<ADDRESS> *trap_event = *it;
				trap_event_set[prc_num].erase(it);
				trap_event->Release();
			}
			while(trap_event_set[prc_num].size());
		}
	}
	
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
	{
		if(source_code_breakpoint_registry[front_end_num].size())
		{
			do
			{
				typename SourceCodeBreakpointRegistry::iterator it = source_code_breakpoint_registry[front_end_num].begin();
				unisim::util::debug::SourceCodeBreakpoint<ADDRESS> *source_code_breakpoint = *it;
				source_code_breakpoint_registry[front_end_num].erase(it);
				source_code_breakpoint->Release();
			}
			while(source_code_breakpoint_registry[front_end_num].size());
		}
		
		if(subprogram_breakpoint_registry[front_end_num].size())
		{
			do
			{
				typename SubProgramBreakpointRegistry::iterator it = subprogram_breakpoint_registry[front_end_num].begin();
				unisim::util::debug::SubProgramBreakpoint<ADDRESS> *subprogram_breakpoint = *it;
				subprogram_breakpoint_registry[front_end_num].erase(it);
				subprogram_breakpoint->Release();
			}
			while(subprogram_breakpoint_registry[front_end_num].size());
		}
		
		if(stub_event_registry[front_end_num].size())
		{
			do
			{
				typename StubEventRegistry::iterator it = stub_event_registry[front_end_num].begin();
				StubEvent<ADDRESS> *stub_event = (*it).second;
				stub_event_registry[front_end_num].erase(it);
				stub_event->Release();
			}
			while(stub_event_registry[front_end_num].size());
		}
		
		if(hook_event_registry[front_end_num].size())
		{
			do
			{
				typename HookEventRegistry::iterator it = hook_event_registry[front_end_num].begin();
				HookEvent<ADDRESS> *hook_event = (*it).second;
				hook_event_registry[front_end_num].erase(it);
				hook_event->Release();
			}
			while(hook_event_registry[front_end_num].size());
		}
	}

	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		delete debug_yielding_export[prc_num];
		delete memory_access_reporting_export[prc_num];
		delete trap_reporting_export[prc_num];
		delete memory_access_reporting_control_import[prc_num];
		delete disasm_import[prc_num];
		delete memory_import[prc_num];
		delete registers_import[prc_num];
		delete debug_timing_import[prc_num];
	}
	
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
	{
		delete debug_yielding_request_export[front_end_num];
		delete debug_selecting_export[front_end_num];
		delete debug_event_trigger_export[front_end_num];
		delete disasm_export[front_end_num];
		delete memory_export[front_end_num];
		delete registers_export[front_end_num];
		delete symbol_table_lookup_export[front_end_num];
		delete stmt_lookup_export[front_end_num];
		delete backtrace_export[front_end_num];
		delete debug_info_loading_export[front_end_num];
		delete data_object_lookup_export[front_end_num];
		delete subprogram_lookup_export[front_end_num];
		delete stack_unwinding_export[front_end_num];
		delete stubbing_export[front_end_num];
		delete hooking_export[front_end_num];
		delete debug_timing_export[front_end_num];
		delete debug_event_listener_import[front_end_num];
		delete debug_yielding_import[front_end_num];
	}
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		delete prc_gate[prc_num];
	}
	
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
	{
		delete front_end_gate[front_end_num];
	}
	
	pthread_mutex_destroy(&curr_front_end_num_mutex);
	pthread_mutex_destroy(&schedule_mutex);
	pthread_mutex_destroy(&mutex);
}

template <typename CONFIG>
bool Debugger<CONFIG>::BeginSetup()
{
	unsigned int prc_num;
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		requires_fetch_instruction_reporting[prc_num] = false;
		requires_commit_instruction_reporting[prc_num] = false;
		requires_memory_access_reporting[prc_num] = false;
	}
	
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		delete elf32_loaders[i];
	}
	elf32_loaders.clear();

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		delete elf64_loaders[i];
	}
	elf64_loaders.clear();

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; i++)
	{
		delete coff_loaders[i];
	}
	coff_loaders.clear();
	
	setup_debug_info_done = false;
	
	return true;
}

template <typename CONFIG>
bool Debugger<CONFIG>::SetupDebugInfo(const unisim::util::blob::Blob<ADDRESS> *blob)
{
	typename unisim::util::blob::FileFormat ffmt = blob->GetFileFormat();
	
	switch(ffmt)
	{
		case unisim::util::blob::FFMT_UNKNOWN:
			break;
		case unisim::util::blob::FFMT_ELF32:
			{
				unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = new unisim::util::loader::elf_loader::Elf32Loader<ADDRESS>(blob);
				
				elf32_loader->SetDebugInfoStream(logger.DebugInfoStream());
				elf32_loader->SetDebugWarningStream(logger.DebugWarningStream());
				elf32_loader->SetDebugErrorStream(logger.DebugErrorStream());
				unsigned int prc_num;
				for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
				{
					elf32_loader->SetRegistersInterface(prc_num, prc_gate[prc_num]->registers_import);
					elf32_loader->SetMemoryInterface(prc_num, prc_gate[prc_num]->memory_import);
				}
				elf32_loader->SetOption(unisim::util::loader::elf_loader::OPT_PARSE_DWARF, parse_dwarf);
				elf32_loader->SetOption(unisim::util::loader::elf_loader::OPT_VERBOSE, verbose);
				elf32_loader->SetOption(unisim::util::loader::elf_loader::OPT_DWARF_REGISTER_NUMBER_MAPPING_FILENAME, unisim::kernel::Object::GetSimulator()->SearchSharedDataFile(dwarf_register_number_mapping_filename.c_str()).c_str());
				elf32_loader->SetOption(unisim::util::loader::elf_loader::OPT_DEBUG_DWARF, debug_dwarf);

				elf32_loader->ParseSymbols();
				elf32_loaders.push_back(elf32_loader);
				unsigned int front_end_num;
				for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
				{
					enable_elf32_loaders[front_end_num].push_back(true);
					if(parse_dwarf)
					{
						dw_mach_state[front_end_num].Register(elf32_loader->GetDWARFHandler());
					}
				}
			}
			break;
		case unisim::util::blob::FFMT_ELF64:
			{
				unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = new unisim::util::loader::elf_loader::Elf64Loader<ADDRESS>(blob);
				
				elf64_loader->SetDebugInfoStream(logger.DebugInfoStream());
				elf64_loader->SetDebugWarningStream(logger.DebugWarningStream());
				elf64_loader->SetDebugErrorStream(logger.DebugErrorStream());
				unsigned int prc_num;
				for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
				{
					elf64_loader->SetRegistersInterface(prc_num, prc_gate[prc_num]->registers_import);
					elf64_loader->SetMemoryInterface(prc_num, prc_gate[prc_num]->memory_import);
				}
				elf64_loader->SetOption(unisim::util::loader::elf_loader::OPT_PARSE_DWARF, parse_dwarf);
				elf64_loader->SetOption(unisim::util::loader::elf_loader::OPT_VERBOSE, verbose);
				elf64_loader->SetOption(unisim::util::loader::elf_loader::OPT_DWARF_REGISTER_NUMBER_MAPPING_FILENAME, unisim::kernel::Object::GetSimulator()->SearchSharedDataFile(dwarf_register_number_mapping_filename.c_str()).c_str());
				elf64_loader->SetOption(unisim::util::loader::elf_loader::OPT_DEBUG_DWARF, debug_dwarf);

				elf64_loader->ParseSymbols();
				elf64_loaders.push_back(elf64_loader);
				unsigned int front_end_num;
				for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
				{
					enable_elf64_loaders[front_end_num].push_back(true);
					if(parse_dwarf)
					{
						dw_mach_state[front_end_num].Register(elf64_loader->GetDWARFHandler());
					}
				}
			}
			break;
		case unisim::util::blob::FFMT_COFF:
			{
				unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = new unisim::util::loader::coff_loader::CoffLoader<ADDRESS>(logger.DebugInfoStream(), logger.DebugWarningStream(), logger.DebugErrorStream(), blob);
				
				coff_loader->ParseSymbols();
				coff_loaders.push_back(coff_loader);
				unsigned int front_end_num;
				for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
				{
					enable_coff_loaders[front_end_num].push_back(true);
				}
			}
			break;
	}
	
	const typename std::vector<const unisim::util::blob::Blob<ADDRESS> *>& sibling_blobs = blob->GetBlobs();
	
	typename std::vector<const unisim::util::blob::Blob<ADDRESS> *>::const_iterator it;
	for(it = sibling_blobs.begin(); it != sibling_blobs.end(); it++)
	{
		const unisim::util::blob::Blob<ADDRESS> *sibling_blob = *it;
		SetupDebugInfo(sibling_blob);
	}
	return true;
}

template <typename CONFIG>
void Debugger<CONFIG>::SetupDebugInfo()
{
	if(setup_debug_info_done)
		return;
		
	unsigned int front_end_num;
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; ++front_end_num)
	{
		dw_mach_state[front_end_num].SetDebugInfoStream(logger.DebugInfoStream());
		dw_mach_state[front_end_num].SetDebugWarningStream(logger.DebugWarningStream());
		dw_mach_state[front_end_num].SetDebugErrorStream(logger.DebugErrorStream());
		dw_mach_state[front_end_num].SetOption(unisim::util::debug::dwarf::OPT_VERBOSE, verbose);
		dw_mach_state[front_end_num].SetOption(unisim::util::debug::dwarf::OPT_DEBUG, debug_dwarf);
		dw_mach_state[front_end_num].SetOption(unisim::util::debug::dwarf::OPT_REG_NUM_MAPPING_FILENAME, unisim::kernel::Object::GetSimulator()->SearchSharedDataFile(dwarf_register_number_mapping_filename.c_str()).c_str());
	
		unsigned int prc_num;
		for(prc_num = 0; prc_num < NUM_PROCESSORS; ++prc_num)
		{
			dw_mach_state[front_end_num].SetRegistersInterface(prc_num, prc_gate[prc_num]->registers_import);
			dw_mach_state[front_end_num].SetMemoryInterface(prc_num, prc_gate[prc_num]->memory_import);
			dw_mach_state[front_end_num].SetArchitecture(prc_num, architecture[prc_num].c_str());
		}
	
		dw_mach_state[front_end_num].Initialize();
	}
	
	if(blob_import)
	{
		blob_import.RequireSetup();
		const unisim::util::blob::Blob<ADDRESS> *blob = blob_import->GetBlob();
		if(blob)
		{
			if(!SetupDebugInfo(blob))
				throw unisim::kernel::ServiceAgent::SetupError();
		}
	}
	setup_debug_info_done = true;
}

template <typename CONFIG>
void Debugger<CONFIG>::UpdateReportingRequirements(unsigned int prc_num)
{
	requires_fetch_instruction_reporting[prc_num] = breakpoint_registry.HasBreakpoints(prc_num) || !fetch_insn_event_set[prc_num].empty();
	requires_commit_instruction_reporting[prc_num] = !commit_insn_event_set[prc_num].empty();
	requires_memory_access_reporting[prc_num] = watchpoint_registry.HasWatchpoints(prc_num);

	prc_gate[prc_num]->RequiresMemoryAccessReporting(unisim::service::interfaces::REPORT_MEM_ACCESS, requires_memory_access_reporting[prc_num]);
	prc_gate[prc_num]->RequiresMemoryAccessReporting(unisim::service::interfaces::REPORT_FETCH_INSN, requires_fetch_instruction_reporting[prc_num]);
	prc_gate[prc_num]->RequiresMemoryAccessReporting(unisim::service::interfaces::REPORT_COMMIT_INSN, requires_commit_instruction_reporting[prc_num]);
}

template <typename CONFIG>
void Debugger<CONFIG>::ScheduleFrontEnd(unsigned int front_end_num)
{
	pthread_mutex_lock(&schedule_mutex);
	schedule |= 1 << front_end_num;
	pthread_mutex_unlock(&schedule_mutex);
}

template <typename CONFIG>
bool Debugger<CONFIG>::NextScheduledFrontEnd(unsigned int& front_end_num)
{
	pthread_mutex_lock(&schedule_mutex);
	if(unisim::util::arithmetic::BitScanForward(front_end_num, schedule))
	{
		schedule &= ~(1 << front_end_num);
		pthread_mutex_unlock(&schedule_mutex);
		return true;
	}
	pthread_mutex_unlock(&schedule_mutex);
	return false;
}

template <typename CONFIG>
bool Debugger<CONFIG>::IsScheduleEmpty() const
{
	return schedule == 0;
}

// Exports to CPUs

// unisim::service::interfaces::DebugYielding (tagged)
template <typename CONFIG>
void Debugger<CONFIG>::DebugYield(unsigned int prc_num)
{
	if(likely(IsScheduleEmpty())) return;

	unsigned int front_end_num = 0;
	
	while(NextScheduledFrontEnd(front_end_num))
	{
		front_end_gate[front_end_num]->DebugYield();
		dw_mach_state[front_end_num].InvalidateFrames(prc_num);
	}
}

// unisim::service::interfaces::MemoryAccessReporting<ADDRESS> (tagged)
template <typename CONFIG>
bool Debugger<CONFIG>::ReportMemoryAccess(unsigned int prc_num, unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size)
{
	bool overlook = true;
	
#if 0
	if(unlikely(!requires_memory_access_reporting[prc_num]))
	{
		logger << DebugWarning << "Processor #" << prc_num << " reports memory access even if it has been asked not to" << EndDebugWarning;
		return overlook;
	}
#endif
	
	if(unlikely(watchpoint_registry.HasWatchpoints(mat, mt, addr, size, prc_num)))
	{
		unsigned int front_end_num;
		
		for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
		{
			Dispatcher<unisim::util::debug::Watchpoint<ADDRESS> > watchpoint_dispatcher(*this, front_end_num, overlook);
			
			// Note: this function may alter local variable "overlook"
			if(watchpoint_registry.template FindWatchpoints<Dispatcher<unisim::util::debug::Watchpoint<ADDRESS> > >(mat, mt, addr, size, prc_num, front_end_num, watchpoint_dispatcher))
			{
				ScheduleFrontEnd(front_end_num);
			}
		}
	}
	
	return overlook;
}

template <typename CONFIG>
void Debugger<CONFIG>::ReportCommitInstruction(unsigned int prc_num, ADDRESS addr, unsigned int length)
{
#if 0
	if(unlikely(!requires_commit_instruction_reporting[prc_num]))
	{
		logger << DebugWarning << "Processor #" << prc_num << " reports instruction commit even if it has been asked not to" << EndDebugWarning;
	}
#endif

	if(unlikely(!commit_insn_event_set[prc_num].empty()))
	{
		typename std::set<unisim::util::debug::CommitInsnEvent<ADDRESS> *>::iterator it;
		
		for(it = commit_insn_event_set[prc_num].begin(); it != commit_insn_event_set[prc_num].end(); it++)
		{
			unisim::util::debug::CommitInsnEvent<ADDRESS> *commit_insn_event = *it;
			unsigned int front_end_num = commit_insn_event->GetFrontEndNumber();
			commit_insn_event->SetAddress(addr);
			commit_insn_event->SetLength(length);
			front_end_gate[front_end_num]->OnDebugEvent(commit_insn_event);
			ScheduleFrontEnd(front_end_num);
		}
	}
}

template <typename CONFIG>
void Debugger<CONFIG>::ReportFetchInstruction(unsigned int prc_num, ADDRESS next_addr)
{
#if 0
	if(unlikely(!requires_fetch_instruction_reporting[prc_num]))
	{
		logger << DebugWarning << "Processor #" << prc_num << " reports instruction fetch even if it has been asked not to" << EndDebugWarning;
	}
#endif
	if(unlikely(breakpoint_registry.HasBreakpoints(next_addr, prc_num)))
	{
		unsigned int front_end_num;
		
		for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
		{
			Dispatcher<unisim::util::debug::Breakpoint<ADDRESS> > breakpoint_dispatcher(*this, front_end_num);
			
			if(breakpoint_registry.template FindBreakpoints<Dispatcher<unisim::util::debug::Breakpoint<ADDRESS> > >(next_addr, prc_num, front_end_num, breakpoint_dispatcher))
			{
				ScheduleFrontEnd(front_end_num);
			}
		}
	}
	
	if(unlikely(!fetch_insn_event_set[prc_num].empty()))
	{
		typename std::set<unisim::util::debug::FetchInsnEvent<ADDRESS> *>::iterator it;
		
		for(it = fetch_insn_event_set[prc_num].begin(); it != fetch_insn_event_set[prc_num].end(); it++)
		{
			unisim::util::debug::FetchInsnEvent<ADDRESS> *fetch_insn_event = *it;
			unsigned int front_end_num = fetch_insn_event->GetFrontEndNumber();
			fetch_insn_event->SetAddress(next_addr);
			front_end_gate[front_end_num]->OnDebugEvent(fetch_insn_event);
			ScheduleFrontEnd(front_end_num);
		}
	}
}


// unisim::service::interfaces::TrapReporting (tagged)
template <typename CONFIG>
void Debugger<CONFIG>::ReportTrap(unsigned int prc_num)
{
	if(unlikely(!trap_event_set[prc_num].empty()))
	{
		typename std::set<unisim::util::debug::TrapEvent<ADDRESS> *>::iterator it;
		
		for(it = trap_event_set[prc_num].begin(); it != trap_event_set[prc_num].end(); it++)
		{
			unisim::util::debug::TrapEvent<ADDRESS> *trap_event = *it;
			unsigned int front_end_num = trap_event->GetFrontEndNumber();
			trap_event->SetTrapObject(0);
			trap_event->SetTrapMessage("");
			front_end_gate[front_end_num]->OnDebugEvent(trap_event);
			ScheduleFrontEnd(front_end_num);
		}
	}
}

template <typename CONFIG>
void Debugger<CONFIG>::ReportTrap(unsigned int prc_num, const unisim::kernel::Object &obj)
{
	if(unlikely(!trap_event_set[prc_num].empty()))
	{
		typename std::set<unisim::util::debug::TrapEvent<ADDRESS> *>::iterator it;
		
		for(it = trap_event_set[prc_num].begin(); it != trap_event_set[prc_num].end(); it++)
		{
			unisim::util::debug::TrapEvent<ADDRESS> *trap_event = *it;
			unsigned int front_end_num = trap_event->GetFrontEndNumber();
			trap_event->SetTrapObject(&obj);
			trap_event->SetTrapMessage("");
			front_end_gate[front_end_num]->OnDebugEvent(trap_event);
			ScheduleFrontEnd(front_end_num);
		}
	}
}

template <typename CONFIG>
void Debugger<CONFIG>::ReportTrap(unsigned int prc_num, const unisim::kernel::Object &obj, const std::string &str)
{
	if(unlikely(!trap_event_set[prc_num].empty()))
	{
		typename std::set<unisim::util::debug::TrapEvent<ADDRESS> *>::iterator it;
		
		for(it = trap_event_set[prc_num].begin(); it != trap_event_set[prc_num].end(); it++)
		{
			unisim::util::debug::TrapEvent<ADDRESS> *trap_event = *it;
			unsigned int front_end_num = trap_event->GetFrontEndNumber();
			trap_event->SetTrapObject(&obj);
			trap_event->SetTrapMessage(str);
			front_end_gate[front_end_num]->OnDebugEvent(trap_event);
			ScheduleFrontEnd(front_end_num);
		}
	}
}

template <typename CONFIG>
void Debugger<CONFIG>::ReportTrap(unsigned int prc_num, const unisim::kernel::Object &obj, const char *c_str)
{
	if(unlikely(!trap_event_set[prc_num].empty()))
	{
		typename std::set<unisim::util::debug::TrapEvent<ADDRESS> *>::iterator it;
		
		for(it = trap_event_set[prc_num].begin(); it != trap_event_set[prc_num].end(); it++)
		{
			unisim::util::debug::TrapEvent<ADDRESS> *trap_event = *it;
			unsigned int front_end_num = trap_event->GetFrontEndNumber();
			trap_event->SetTrapObject(&obj);
			trap_event->SetTrapMessage(c_str);
			front_end_gate[front_end_num]->OnDebugEvent(trap_event);
			ScheduleFrontEnd(front_end_num);
		}
	}
}

// Export to Front-ends

// unisim::service::interfaces::DebugSelecting (tagged)
template <typename CONFIG>
bool Debugger<CONFIG>::DebugSelect(unsigned int front_end_num, unsigned int prc_num)
{
	if(front_end_num >= MAX_FRONT_ENDS) return false;
	if(prc_num >= NUM_PROCESSORS) return false;
	
	sel_prc_gate[front_end_num] = prc_gate[prc_num];
	sel_cpu[front_end_num] = prc_num;
	
	return true;
}

template <typename CONFIG>
unsigned int Debugger<CONFIG>::DebugGetSelected(unsigned int front_end_num) const
{
	if(front_end_num >= MAX_FRONT_ENDS) return 0;
	
	return sel_cpu[front_end_num];
}

template <typename CONFIG>
bool Debugger<CONFIG>::DebugFrameSelect(unsigned int front_end_num, unsigned int frame_num)
{
	if(front_end_num >= MAX_FRONT_ENDS) return false;
	
	return dw_mach_state[front_end_num].SelectFrame(sel_cpu[front_end_num], frame_num);
}

template <typename CONFIG>
unsigned int Debugger<CONFIG>::DebugFrameGetSelected(unsigned int front_end_num) const
{
	if(front_end_num >= MAX_FRONT_ENDS) return 0;
	return dw_mach_state[front_end_num].GetSelectedFrame(sel_cpu[front_end_num]);
}

// unisim::service::interfaces::DebugYieldingRequest (tagged)
template <typename CONFIG>
void Debugger<CONFIG>::DebugYieldRequest(unsigned int front_end_num)
{
	if(front_end_num >= MAX_FRONT_ENDS) return;

	ScheduleFrontEnd(front_end_num);
}

// unisim::service::interfaces::DebugEventTrigger<ADDRESS> (tagged)
template <typename CONFIG>
bool Debugger<CONFIG>::Listen(unsigned int front_end_num, unisim::util::debug::Event<ADDRESS> *event)
{
//	std::cerr << "Listen(" << front_end_num << ", " << event << ")" << std::endl;
	int event_prc_num = event->GetProcessorNumber();
	
	unsigned int prc_num = (event_prc_num < 0) ? sel_cpu[front_end_num] : event_prc_num;
	
	if(prc_num >= NUM_PROCESSORS) return false;
	
	event->SetProcessorNumber(prc_num);
	
	int event_front_end_num = event->GetFrontEndNumber();
	if(event_front_end_num < 0)
	{
		event->SetFrontEndNumber(front_end_num);
	}
	else if((unsigned int) event_front_end_num != front_end_num)
	{
		return false;
	}
	
	typename unisim::util::debug::Event<ADDRESS>::Type event_type = event->GetType();
	if(event_type == unisim::util::debug::Breakpoint<ADDRESS>::TYPE)
	{
		unisim::util::debug::Breakpoint<ADDRESS> *brkp = static_cast<unisim::util::debug::Breakpoint<ADDRESS> *>(event);
		
		if(!breakpoint_registry.SetBreakpoint(brkp)) return false;
		
		if(brkp->GetId() < 0)
		{
			brkp->SetId(AllocateId(front_end_num));
		}
	}
	else if(event_type == unisim::util::debug::Watchpoint<ADDRESS>::TYPE)
	{
		unisim::util::debug::Watchpoint<ADDRESS> *wp = static_cast<unisim::util::debug::Watchpoint<ADDRESS> *>(event);
		
		if(!watchpoint_registry.SetWatchpoint(wp)) return false;
		
		if(wp->GetId() < 0)
		{
			wp->SetId(AllocateId(front_end_num));
		}
	}
	else if(event_type == unisim::util::debug::FetchInsnEvent<ADDRESS>::TYPE)
	{
		unisim::util::debug::FetchInsnEvent<ADDRESS> *fetch_insn_event = static_cast<unisim::util::debug::FetchInsnEvent<ADDRESS> *>(event);

		if(fetch_insn_event_set[prc_num].find(fetch_insn_event) != fetch_insn_event_set[prc_num].end())
		{
			return true; // already set
		}
		
		fetch_insn_event_set[prc_num].insert(fetch_insn_event);
		fetch_insn_event->Catch();
	}
	else if(event_type == unisim::util::debug::CommitInsnEvent<ADDRESS>::TYPE)
	{
		unisim::util::debug::CommitInsnEvent<ADDRESS> *commit_insn_event = static_cast<unisim::util::debug::CommitInsnEvent<ADDRESS> *>(event);

		if(commit_insn_event_set[prc_num].find(commit_insn_event) != commit_insn_event_set[prc_num].end())
		{
			return true; // already set
		}

		commit_insn_event_set[prc_num].insert(commit_insn_event);
		commit_insn_event->Catch();
	}
	else if(event_type == unisim::util::debug::TrapEvent<ADDRESS>::TYPE)
	{
		unisim::util::debug::TrapEvent<ADDRESS> *trap_event = static_cast<unisim::util::debug::TrapEvent<ADDRESS> *>(event);

		if(trap_event_set[prc_num].find(trap_event) != trap_event_set[prc_num].end())
		{
			return true; // already set
		}
		
		trap_event_set[prc_num].insert(trap_event);
		trap_event->Catch();
	}
	else if(event_type == unisim::util::debug::SourceCodeBreakpoint<ADDRESS>::TYPE)
	{
		unisim::util::debug::SourceCodeBreakpoint<ADDRESS> *src_code_brkp = static_cast<unisim::util::debug::SourceCodeBreakpoint<ADDRESS> *>(event);
		
		std::pair<typename SourceCodeBreakpointRegistry::iterator, bool> r = source_code_breakpoint_registry[front_end_num].insert(src_code_brkp);
		
		if(!r.second) return true; // source code breakpoint already set
		
		std::vector<const unisim::util::debug::Statement<ADDRESS> *> stmts;
		
		FindStatements(front_end_num, stmts, src_code_brkp->GetSourceCodeLocation(), src_code_brkp->GetFilename());
		
		if(stmts.size())
		{
			for(typename std::vector<const unisim::util::debug::Statement<ADDRESS> *>::iterator it = stmts.begin(); it != stmts.end(); ++it)
			{
				const unisim::util::debug::Statement<ADDRESS> *stmt = *it;
				
				ADDRESS brkp_addr = stmt->GetAddress();
				
				unisim::util::debug::Breakpoint<ADDRESS> *brkp = new unisim::util::debug::Breakpoint<ADDRESS>(brkp_addr, src_code_brkp);
				brkp->SetProcessorNumber(prc_num);
				brkp->SetFrontEndNumber(front_end_num);
				
				if(!breakpoint_registry.SetBreakpoint(brkp))
				{
					source_code_breakpoint_registry[front_end_num].erase(r.first);
					delete brkp;
					return false;
				}
				
				src_code_brkp->Attach(brkp);
			}
		}
		else
		{
			// no statements
			source_code_breakpoint_registry[front_end_num].erase(r.first);
			return false;
		}
		
		src_code_brkp->SetId(AllocateId(front_end_num));
		src_code_brkp->Catch();
	}
	else if(event_type == unisim::util::debug::SubProgramBreakpoint<ADDRESS>::TYPE)
	{
		unisim::util::debug::SubProgramBreakpoint<ADDRESS> *subprogram_brkp = static_cast<unisim::util::debug::SubProgramBreakpoint<ADDRESS> *>(event);
		
		std::pair<typename SubProgramBreakpointRegistry::iterator, bool> r = subprogram_breakpoint_registry[front_end_num].insert(subprogram_brkp);
		
		if(!r.second) return true; // subprogram breakpoint already set
		
		const unisim::util::debug::SubProgram<ADDRESS> *subprogram = subprogram_brkp->GetSubProgram();
		
		ADDRESS brkp_addr = subprogram->GetAddress();
			
		unisim::util::debug::Breakpoint<ADDRESS> *brkp = new unisim::util::debug::Breakpoint<ADDRESS>(brkp_addr, subprogram_brkp);
		brkp->SetProcessorNumber(prc_num);
		brkp->SetFrontEndNumber(front_end_num);
			
		if(!breakpoint_registry.SetBreakpoint(brkp))
		{
			subprogram_breakpoint_registry[front_end_num].erase(r.first);
			delete brkp;
			return false;
		}
			
		subprogram_brkp->Attach(brkp);
		
		subprogram_brkp->SetId(AllocateId(front_end_num));
		subprogram_brkp->Catch();
	}
	else
	{
		return false;
	}
	
	UpdateReportingRequirements(prc_num);
	
	return true;
}

template <typename CONFIG>
bool Debugger<CONFIG>::Unlisten(unsigned int front_end_num, unisim::util::debug::Event<ADDRESS> *event)
{
	int event_prc_num = event->GetProcessorNumber();
	
	unsigned int prc_num = (event_prc_num < 0) ? sel_cpu[front_end_num] : event_prc_num;

	if(prc_num >= NUM_PROCESSORS) return false;
	
	event->SetProcessorNumber(prc_num);
	
	int event_front_end_num = event->GetFrontEndNumber();
	if(event_front_end_num < 0)
	{
		event->SetFrontEndNumber(front_end_num);
	}
	else if((unsigned int) event_front_end_num != front_end_num)
	{
		return false;
	}

	typename unisim::util::debug::Event<ADDRESS>::Type event_type = event->GetType();
	if(event_type == unisim::util::debug::Breakpoint<ADDRESS>::TYPE)
	{
		unisim::util::debug::Breakpoint<ADDRESS> *brkp = static_cast<unisim::util::debug::Breakpoint<ADDRESS> *>(event);
		
		if(!breakpoint_registry.RemoveBreakpoint(brkp)) return false;
	}
	else if(event_type == unisim::util::debug::Watchpoint<ADDRESS>::TYPE)
	{
		unisim::util::debug::Watchpoint<ADDRESS> *wp = static_cast<unisim::util::debug::Watchpoint<ADDRESS> *>(event);
		
		if(!watchpoint_registry.RemoveWatchpoint(wp)) return false;
	}
	else if(event_type == unisim::util::debug::FetchInsnEvent<ADDRESS>::TYPE)
	{
		unisim::util::debug::FetchInsnEvent<ADDRESS> *fetch_insn_event = static_cast<unisim::util::debug::FetchInsnEvent<ADDRESS> *>(event);

		typename std::set<unisim::util::debug::FetchInsnEvent<ADDRESS> *>::const_iterator it = fetch_insn_event_set[prc_num].find(fetch_insn_event);
		if(it == fetch_insn_event_set[prc_num].end())
		{
			return false;
		}
		
		fetch_insn_event_set[prc_num].erase(fetch_insn_event);
		fetch_insn_event->Release();
	}
	else if(event_type == unisim::util::debug::CommitInsnEvent<ADDRESS>::TYPE)
	{
		unisim::util::debug::CommitInsnEvent<ADDRESS> *commit_insn_event = static_cast<unisim::util::debug::CommitInsnEvent<ADDRESS> *>(event);

		typename std::set<unisim::util::debug::CommitInsnEvent<ADDRESS> *>::const_iterator it = commit_insn_event_set[prc_num].find(commit_insn_event);
		if(it == commit_insn_event_set[prc_num].end())
		{
			return false;
		}
		
		commit_insn_event_set[prc_num].erase(commit_insn_event);
		commit_insn_event->Release();
	}
	else if(event_type == unisim::util::debug::TrapEvent<ADDRESS>::TYPE)
	{
		unisim::util::debug::TrapEvent<ADDRESS> *trap_event = static_cast<unisim::util::debug::TrapEvent<ADDRESS> *>(event);

		typename std::set<unisim::util::debug::TrapEvent<ADDRESS> *>::const_iterator it = trap_event_set[prc_num].find(trap_event);
		if(it == trap_event_set[prc_num].end())
		{
			return false;
		}
		
		trap_event_set[prc_num].erase(trap_event);
		trap_event->Release();
	}
	else if(event_type == unisim::util::debug::SourceCodeBreakpoint<ADDRESS>::TYPE)
	{
		unisim::util::debug::SourceCodeBreakpoint<ADDRESS> *src_code_brkp = static_cast<unisim::util::debug::SourceCodeBreakpoint<ADDRESS> *>(event);
		
		typename SourceCodeBreakpointRegistry::const_iterator it = source_code_breakpoint_registry[front_end_num].find(src_code_brkp);
		if(it == source_code_breakpoint_registry[front_end_num].end())
		{
			return false;
		}
		
		BreakpointRemover brkp_remover(*this);
		src_code_brkp->Scan(brkp_remover);
		
		source_code_breakpoint_registry[front_end_num].erase(it);
		src_code_brkp->Release();
		
		if(!brkp_remover.status) return false;
	}
	else
	{
		// ignore
		return false;
	}
	
	UpdateReportingRequirements(prc_num);
	
	return true;
}

template <typename CONFIG>
bool Debugger<CONFIG>::IsEventListened(unsigned int front_end_num, unisim::util::debug::Event<ADDRESS> *event) const
{
	unsigned int prc_num = event->GetProcessorNumber();
	
	typename unisim::util::debug::Event<ADDRESS>::Type event_type = event->GetType();
	if(event_type == unisim::util::debug::Breakpoint<ADDRESS>::TYPE)
	{
		typename unisim::util::debug::Breakpoint<ADDRESS> *brkp = static_cast<unisim::util::debug::Breakpoint<ADDRESS> *>(event);
		
		return breakpoint_registry.HasBreakpoint(brkp);
	}
	else if(event_type == unisim::util::debug::Watchpoint<ADDRESS>::TYPE)
	{
		typename unisim::util::debug::Watchpoint<ADDRESS> *wp = static_cast<unisim::util::debug::Watchpoint<ADDRESS> *>(event);
		
		return watchpoint_registry.HasWatchpoint(wp);
	}
	else if(event_type == unisim::util::debug::FetchInsnEvent<ADDRESS>::TYPE)
	{
		unisim::util::debug::FetchInsnEvent<ADDRESS> *fetch_insn_event = static_cast<unisim::util::debug::FetchInsnEvent<ADDRESS> *>(event);

		typename std::set<unisim::util::debug::FetchInsnEvent<ADDRESS> *>::const_iterator it = fetch_insn_event_set[prc_num].find(fetch_insn_event);
		return it != fetch_insn_event_set[prc_num].end();
	}
	else if(event_type == unisim::util::debug::CommitInsnEvent<ADDRESS>::TYPE)
	{
		unisim::util::debug::CommitInsnEvent<ADDRESS> *commit_insn_event = static_cast<unisim::util::debug::CommitInsnEvent<ADDRESS> *>(event);

		typename std::set<unisim::util::debug::CommitInsnEvent<ADDRESS> *>::const_iterator it = commit_insn_event_set[prc_num].find(commit_insn_event);
		return it != commit_insn_event_set[prc_num].end();
	}
	else if(event_type == unisim::util::debug::TrapEvent<ADDRESS>::TYPE)
	{
		unisim::util::debug::TrapEvent<ADDRESS> *trap_event = static_cast<unisim::util::debug::TrapEvent<ADDRESS> *>(event);

		typename std::set<unisim::util::debug::TrapEvent<ADDRESS> *>::const_iterator it = trap_event_set[prc_num].find(trap_event);
		return it != trap_event_set[prc_num].end();
	}
	else if(event_type == unisim::util::debug::SourceCodeBreakpoint<ADDRESS>::TYPE)
	{
		unisim::util::debug::SourceCodeBreakpoint<ADDRESS> *src_code_brkp = static_cast<unisim::util::debug::SourceCodeBreakpoint<ADDRESS> *>(event);
		
		typename SourceCodeBreakpointRegistry::const_iterator it = source_code_breakpoint_registry[front_end_num].find(src_code_brkp);
		return it != source_code_breakpoint_registry[front_end_num].end();
	}
	else if(event_type == unisim::util::debug::SubProgramBreakpoint<ADDRESS>::TYPE)
	{
		unisim::util::debug::SubProgramBreakpoint<ADDRESS> *subprogram_brkp = static_cast<unisim::util::debug::SubProgramBreakpoint<ADDRESS> *>(event);
		
		typename SubProgramBreakpointRegistry::const_iterator it = subprogram_breakpoint_registry[front_end_num].find(subprogram_brkp);
		return it != subprogram_breakpoint_registry[front_end_num].end();
	}
	return false;
}

template <typename CONFIG>
void Debugger<CONFIG>::ScanListenedEvents(unsigned int front_end_num, unisim::service::interfaces::DebugEventScanner<ADDRESS>& scanner) const
{
	for(unsigned int prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		typename std::set<unisim::util::debug::FetchInsnEvent<ADDRESS> *>::const_iterator it;
		for(it = fetch_insn_event_set[prc_num].begin(); it != fetch_insn_event_set[prc_num].end(); it++)
		{
			unisim::util::debug::FetchInsnEvent<ADDRESS> *fetch_insn_event = *it;
			if((unsigned int) fetch_insn_event->GetFrontEndNumber() == front_end_num)
			{
				scanner.Append(fetch_insn_event);
			}
		}
	}

	for(unsigned int prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		typename std::set<unisim::util::debug::CommitInsnEvent<ADDRESS> *>::const_iterator it;
		for(it = commit_insn_event_set[prc_num].begin(); it != commit_insn_event_set[prc_num].end(); it++)
		{
			unisim::util::debug::CommitInsnEvent<ADDRESS> *commit_insn_event = *it;
			if((unsigned int) commit_insn_event->GetFrontEndNumber() == front_end_num)
			{
				scanner.Append(commit_insn_event);
			}
		}
	}

	for(unsigned int prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		typename std::set<unisim::util::debug::TrapEvent<ADDRESS> *>::const_iterator it;
		for(it = trap_event_set[prc_num].begin(); it != trap_event_set[prc_num].end(); it++)
		{
			unisim::util::debug::TrapEvent<ADDRESS> *trap_event = *it;
			if((unsigned int) trap_event->GetFrontEndNumber() == front_end_num)
			{
				scanner.Append(trap_event);
			}
		}
	}

	struct Filter : unisim::service::interfaces::DebugEventScanner<ADDRESS>
	{
		Filter()
			: reorder_buffer()
		{
		}
		
		virtual void Append(unisim::util::debug::Event<ADDRESS> *event)
		{
			if(event->GetType() == unisim::util::debug::Breakpoint<ADDRESS>::TYPE)
			{
				unisim::util::debug::Breakpoint<ADDRESS> *brkp = static_cast<unisim::util::debug::Breakpoint<ADDRESS> *>(event);
				if(!brkp->GetReference())
				{
					unsigned int id = brkp->GetId();
					if(id >= reorder_buffer.size())
					{
						reorder_buffer.resize(id + 1);
					}
					reorder_buffer[id] = event;
				}
			}
			else if(event->GetType() == unisim::util::debug::SourceCodeBreakpoint<ADDRESS>::TYPE)
			{
				unisim::util::debug::SourceCodeBreakpoint<ADDRESS> *src_code_brkp = static_cast<unisim::util::debug::SourceCodeBreakpoint<ADDRESS> *>(event);
				unsigned int id = src_code_brkp->GetId();
				if(id >= reorder_buffer.size())
				{
					reorder_buffer.resize(id + 1);
				}
				reorder_buffer[id] = event;
			}
			else if(event->GetType() == unisim::util::debug::Watchpoint<ADDRESS>::TYPE)
			{
				unisim::util::debug::Watchpoint<ADDRESS> *wp = static_cast<unisim::util::debug::Watchpoint<ADDRESS> *>(event);
				unsigned int id = wp->GetId();
				if(id >= reorder_buffer.size())
				{
					reorder_buffer.resize(id + 1);
				}
				reorder_buffer[id] = event;
			}
		}
		
		void Scan(unisim::service::interfaces::DebugEventScanner<ADDRESS>& scanner)
		{
			for(typename std::vector<unisim::util::debug::Event<ADDRESS> *>::iterator it = reorder_buffer.begin(); it != reorder_buffer.end(); ++it)
			{
				unisim::util::debug::Event<ADDRESS> *event = *it;
				if(event)
				{
					scanner.Append(event);
				}
			}
		}
		
	private:
		std::vector<unisim::util::debug::Event<ADDRESS> *> reorder_buffer;
	};
	
	Filter filter;
	
	breakpoint_registry.ScanBreakpoints(front_end_num, filter);
	
	watchpoint_registry.ScanWatchpoints(front_end_num, filter);
	
	for(unsigned int front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
	{
		if(source_code_breakpoint_registry[front_end_num].size())
		{
			typename SourceCodeBreakpointRegistry::const_iterator it = source_code_breakpoint_registry[front_end_num].begin();
			
			do
			{
				unisim::util::debug::SourceCodeBreakpoint<ADDRESS> *src_code_brkp = *it;
				typename SourceCodeBreakpointRegistry::const_iterator next_it = ++it;
				
				filter.Append(src_code_brkp);
				
				it = next_it;
			}
			while(it != source_code_breakpoint_registry[front_end_num].end());
		}
		
		if(subprogram_breakpoint_registry[front_end_num].size())
		{
			typename SubProgramBreakpointRegistry::const_iterator it = subprogram_breakpoint_registry[front_end_num].begin();
			
			do
			{
				unisim::util::debug::SubProgramBreakpoint<ADDRESS> *subprogram_brkp = *it;
				typename SubProgramBreakpointRegistry::const_iterator next_it = ++it;
				
				filter.Append(subprogram_brkp);
				
				it = next_it;
			}
			while(it != subprogram_breakpoint_registry[front_end_num].end());
		}
	}
	
	filter.Scan(scanner);
}

template <typename CONFIG>
void Debugger<CONFIG>::ClearEvents(unsigned int front_end_num)
{
	struct DebugEventScanner : unisim::service::interfaces::DebugEventScanner<ADDRESS>
	{
		DebugEventScanner(Debugger<CONFIG>& _dbg, unsigned int _front_end_num)
			: dbg(_dbg)
			, front_end_num(_front_end_num)
		{
		}
		
		virtual void Append(unisim::util::debug::Event<ADDRESS> *event)
		{
			dbg.Unlisten(front_end_num, event);
		}
	private:
		Debugger<CONFIG>& dbg;
		unsigned int front_end_num;
	};
	
	DebugEventScanner event_scanner(*this, front_end_num);
	ScanListenedEvents(front_end_num, event_scanner);
}

template <typename CONFIG>
bool Debugger<CONFIG>::SetBreakpoint(unsigned int front_end_num, ADDRESS addr)
{
	unsigned int prc_num = sel_cpu[front_end_num];
	unisim::util::debug::Breakpoint<ADDRESS> *brkp = breakpoint_registry.SetBreakpoint(addr, prc_num, front_end_num);
	if(brkp && (brkp->GetId() < 0))
	{
		brkp->SetId(AllocateId(front_end_num));
	}
	UpdateReportingRequirements(prc_num);
	return brkp != 0;
}

template <typename CONFIG>
bool Debugger<CONFIG>::RemoveBreakpoint(unsigned int front_end_num, ADDRESS addr)
{
	unsigned int prc_num = sel_cpu[front_end_num];
	bool status = breakpoint_registry.RemoveBreakpoint(addr, prc_num, front_end_num);
	UpdateReportingRequirements(prc_num);
	return status;
}

template <typename CONFIG>
bool Debugger<CONFIG>::HasBreakpoints(unsigned int front_end_num, ADDRESS addr)
{
	unsigned int prc_num = sel_cpu[front_end_num];
	return breakpoint_registry.HasBreakpoints(addr, prc_num, front_end_num);
}

template <typename CONFIG>
bool Debugger<CONFIG>::SetWatchpoint(unsigned int front_end_num, unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size, bool overlook)
{
	unsigned int prc_num = sel_cpu[front_end_num];
	unisim::util::debug::Watchpoint<ADDRESS> *wp = watchpoint_registry.SetWatchpoint(mat, mt, addr, size, overlook, prc_num, front_end_num);
	if(wp && (wp->GetId() < 0))
	{
		wp->SetId(AllocateId(front_end_num));
	}
	UpdateReportingRequirements(prc_num);
	return wp != 0;
}

template <typename CONFIG>
bool Debugger<CONFIG>::RemoveWatchpoint(unsigned int front_end_num, unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size)
{
	unsigned int prc_num = sel_cpu[front_end_num];
	bool status = watchpoint_registry.RemoveWatchpoint(mat, mt, addr, size, prc_num, front_end_num);
	UpdateReportingRequirements(prc_num);
	return status;
}

template <typename CONFIG>
bool Debugger<CONFIG>::HasWatchpoints(unsigned int front_end_num, unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size)
{
	unsigned int prc_num = sel_cpu[front_end_num];
	return watchpoint_registry.HasWatchpoints(mat, mt, addr, size, prc_num, front_end_num);
}

// unisim::service::interfaces::Disassembly<ADDRESS> (tagged)
template <typename CONFIG>
std::string Debugger<CONFIG>::Disasm(unsigned int front_end_num, ADDRESS addr, ADDRESS& next_addr)
{
	return sel_prc_gate[front_end_num]->Disasm(addr, next_addr);
}

// unisim::service::interfaces::Memory<ADDRESS> (tagged)
template <typename CONFIG>
void Debugger<CONFIG>::ResetMemory(unsigned int front_end_num)
{
	sel_prc_gate[front_end_num]->ResetMemory();
}

template <typename CONFIG>
bool Debugger<CONFIG>::ReadMemory(unsigned int front_end_num, ADDRESS addr, void *buffer, uint32_t size)
{
	return sel_prc_gate[front_end_num]->ReadMemory(addr, buffer, size);
}

template <typename CONFIG>
bool Debugger<CONFIG>::WriteMemory(unsigned int front_end_num, ADDRESS addr, const void *buffer, uint32_t size)
{
	return sel_prc_gate[front_end_num]->WriteMemory(addr, buffer, size);
}


// unisim::service::interfaces::Registers (tagged)
template <typename CONFIG>
unisim::service::interfaces::Register *Debugger<CONFIG>::GetRegister(unsigned int front_end_num, const char *name)
{
	assert(sel_cpu[front_end_num] == sel_cpu[front_end_num]);
	return dw_mach_state[front_end_num].GetRegister(sel_cpu[front_end_num], name);
}

template <typename CONFIG>
void Debugger<CONFIG>::ScanRegisters(unsigned int front_end_num, unisim::service::interfaces::RegisterScanner& scanner)
{
	dw_mach_state[front_end_num].ScanRegisters(sel_cpu[front_end_num], scanner);
}

// unisim::service::interfaces::SymbolTableLookup<ADDRESS> (tagged)
template <typename CONFIG>
void Debugger<CONFIG>::GetSymbols(unsigned int front_end_num, typename std::list<const unisim::util::debug::Symbol<ADDRESS> *>& lst, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf32_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			elf32_loader->GetSymbols(lst, type);
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf64_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			elf64_loader->GetSymbols(lst, type);
		}
	}

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_coff_loaders[front_end_num][i])
		{
			typename unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = coff_loaders[i];
			coff_loader->GetSymbols(lst, type);
		}
	}
}

template <typename CONFIG>
const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *Debugger<CONFIG>::FindSymbol(unsigned int front_end_num, const char *name, ADDRESS addr, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf32_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = elf32_loader->FindSymbol(name, addr, type);
			if(symbol) return symbol;
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf64_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = elf64_loader->FindSymbol(name, addr, type);
			if(symbol) return symbol;
		}
	}

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_coff_loaders[front_end_num][i])
		{
			typename unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = coff_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = coff_loader->FindSymbol(name, addr, type);
			if(symbol) return symbol;
		}
	}
	return 0;
}

template <typename CONFIG>
const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *Debugger<CONFIG>::FindSymbolByAddr(unsigned int front_end_num, ADDRESS addr) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf32_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = elf32_loader->FindSymbolByAddr(addr);
			if(symbol) return symbol;
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf64_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = elf64_loader->FindSymbolByAddr(addr);
			if(symbol) return symbol;
		}
	}

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_coff_loaders[front_end_num][i])
		{
			typename unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = coff_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = coff_loader->FindSymbolByAddr(addr);
			if(symbol) return symbol;
		}
	}
	return 0;
}

template <typename CONFIG>
const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *Debugger<CONFIG>::FindSymbolByName(unsigned int front_end_num, const char *name) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf32_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = elf32_loader->FindSymbolByName(name);
			if(symbol) return symbol;
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf64_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = elf64_loader->FindSymbolByName(name);
			if(symbol) return symbol;
		}
	}

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_coff_loaders[front_end_num][i])
		{
			typename unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = coff_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = coff_loader->FindSymbolByName(name);
			if(symbol) return symbol;
		}
	}
	return 0;
}

template <typename CONFIG>
const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *Debugger<CONFIG>::FindSymbolByName(unsigned int front_end_num, const char *name, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf32_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = elf32_loader->FindSymbolByName(name, type);
			if(symbol) return symbol;
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf64_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = elf64_loader->FindSymbolByName(name, type);
			if(symbol) return symbol;
		}
	}

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_coff_loaders[front_end_num][i])
		{
			typename unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = coff_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = coff_loader->FindSymbolByName(name, type);
			if(symbol) return symbol;
		}
	}
	return 0;
}

template <typename CONFIG>
const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *Debugger<CONFIG>::FindSymbolByAddr(unsigned int front_end_num, ADDRESS addr, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf32_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = elf32_loader->FindSymbolByAddr(addr, type);
			if(symbol) return symbol;
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf64_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = elf64_loader->FindSymbolByAddr(addr, type);
			if(symbol) return symbol;
		}
	}

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; i++)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_coff_loaders[front_end_num][i])
		{
			typename unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = coff_loaders[i];
			const typename unisim::util::debug::Symbol<ADDRESS> *symbol = coff_loader->FindSymbolByAddr(addr, type);
			if(symbol) return symbol;
		}
	}
	return 0;
}


// unisim::service::interfaces::StatementLookup<ADDRESS> (tagged)
template <typename CONFIG>
void Debugger<CONFIG>::ScanStatements(unsigned int front_end_num, unisim::service::interfaces::StatementScanner<ADDRESS>& scanner, const char *filename) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
		const unisim::util::blob::Blob<ADDRESS> *blob = elf32_loader->GetBlob();
		if((filename && ((blob->GetCapability() & unisim::util::blob::CAP_FILENAME) && (strcmp(blob->GetFilename(), filename) == 0))) ||
			 (!filename && ((front_end_num >= MAX_FRONT_ENDS) || enable_elf32_loaders[front_end_num][i])))
		{
			elf32_loader->ScanStatements(scanner);
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
		const unisim::util::blob::Blob<ADDRESS> *blob = elf64_loader->GetBlob();
		if((filename && ((blob->GetCapability() & unisim::util::blob::CAP_FILENAME) && (strcmp(blob->GetFilename(), filename) == 0))) ||
			 (!filename && ((front_end_num >= MAX_FRONT_ENDS) || enable_elf64_loaders[front_end_num][i])))
		{
			elf64_loader->ScanStatements(scanner);
		}
	}
}

template <typename CONFIG>
const unisim::util::debug::Statement<typename CONFIG::ADDRESS> *Debugger<CONFIG>::FindStatement(unsigned int front_end_num, ADDRESS addr, const char *filename, typename unisim::service::interfaces::StatementLookup<ADDRESS>::FindStatementOption opt) const
{
	const unisim::util::debug::Statement<ADDRESS> *ret_stmt = 0;
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
		const unisim::util::blob::Blob<ADDRESS> *blob = elf32_loader->GetBlob();
		if((filename && ((blob->GetCapability() & unisim::util::blob::CAP_FILENAME) && (strcmp(blob->GetFilename(), filename) == 0))) ||
			 (!filename && ((front_end_num >= MAX_FRONT_ENDS) || enable_elf32_loaders[front_end_num][i])))
		{
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
		typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
		const unisim::util::blob::Blob<ADDRESS> *blob = elf64_loader->GetBlob();
		if((filename && ((blob->GetCapability() & unisim::util::blob::CAP_FILENAME) && (strcmp(blob->GetFilename(), filename) == 0))) ||
			 (!filename && ((front_end_num >= MAX_FRONT_ENDS) || enable_elf64_loaders[front_end_num][i])))
		{
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

template <typename CONFIG>
const unisim::util::debug::Statement<typename CONFIG::ADDRESS> *Debugger<CONFIG>::FindStatements(unsigned int front_end_num, std::vector<const unisim::util::debug::Statement<ADDRESS> *> &stmts, ADDRESS addr, const char *filename, typename unisim::service::interfaces::StatementLookup<ADDRESS>::FindStatementOption opt) const
{
	const unisim::util::debug::Statement<ADDRESS> *ret_stmt = 0;
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
		const unisim::util::blob::Blob<ADDRESS> *blob = elf32_loader->GetBlob();
		if((filename && ((blob->GetCapability() & unisim::util::blob::CAP_FILENAME) && (strcmp(blob->GetFilename(), filename) == 0))) ||
			 (!filename && ((front_end_num >= MAX_FRONT_ENDS) || enable_elf32_loaders[front_end_num][i])))
		{
			const typename unisim::util::debug::Statement<ADDRESS> *stmt = elf32_loader->FindStatements(stmts, addr, opt);
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
		typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
		const unisim::util::blob::Blob<ADDRESS> *blob = elf64_loader->GetBlob();
		if((filename && ((blob->GetCapability() & unisim::util::blob::CAP_FILENAME) && (strcmp(blob->GetFilename(), filename) == 0))) ||
			 (!filename && ((front_end_num >= MAX_FRONT_ENDS) || enable_elf64_loaders[front_end_num][i])))
		{
			const typename unisim::util::debug::Statement<ADDRESS> *stmt = elf64_loader->FindStatements(stmts, addr, opt);
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

template <typename CONFIG>
const unisim::util::debug::Statement<typename CONFIG::ADDRESS> *Debugger<CONFIG>::FindStatement(unsigned int front_end_num, const unisim::util::debug::SourceCodeLocation& source_code_location, const char *filename) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
		const unisim::util::blob::Blob<ADDRESS> *blob = elf32_loader->GetBlob();
		if((filename && ((blob->GetCapability() & unisim::util::blob::CAP_FILENAME) && (strcmp(blob->GetFilename(), filename) == 0))) ||
			 (!filename && ((front_end_num >= MAX_FRONT_ENDS) || enable_elf32_loaders[front_end_num][i])))
		{
			const typename unisim::util::debug::Statement<ADDRESS> *stmt = elf32_loader->FindStatement(source_code_location);
			if(stmt) return stmt;
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
		const unisim::util::blob::Blob<ADDRESS> *blob = elf64_loader->GetBlob();
		if((filename && ((blob->GetCapability() & unisim::util::blob::CAP_FILENAME) && (strcmp(blob->GetFilename(), filename) == 0))) ||
			 (!filename && ((front_end_num >= MAX_FRONT_ENDS) || enable_elf64_loaders[front_end_num][i])))
		{
			const typename unisim::util::debug::Statement<ADDRESS> *stmt = elf64_loader->FindStatement(source_code_location);
			if(stmt) return stmt;
		}
	}
	return 0;
}

template <typename CONFIG>
const unisim::util::debug::Statement<typename CONFIG::ADDRESS> *Debugger<CONFIG>::FindStatements(unsigned int front_end_num, std::vector<const unisim::util::debug::Statement<ADDRESS> *> &stmts, const unisim::util::debug::SourceCodeLocation& source_code_location, const char *filename) const
{
	const typename unisim::util::debug::Statement<ADDRESS> *ret = 0;
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
		const unisim::util::blob::Blob<ADDRESS> *blob = elf32_loader->GetBlob();
		if((filename && ((blob->GetCapability() & unisim::util::blob::CAP_FILENAME) && (strcmp(blob->GetFilename(), filename) == 0))) ||
			 (!filename && ((front_end_num >= MAX_FRONT_ENDS) || enable_elf32_loaders[front_end_num][i])))
		{
			const typename unisim::util::debug::Statement<ADDRESS> *stmt = elf32_loader->FindStatements(stmts, source_code_location);
			
			if(!ret) ret = stmt;
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
		const unisim::util::blob::Blob<ADDRESS> *blob = elf64_loader->GetBlob();
		if((filename && ((blob->GetCapability() & unisim::util::blob::CAP_FILENAME) && (strcmp(blob->GetFilename(), filename) == 0))) ||
			 (!filename && ((front_end_num >= MAX_FRONT_ENDS) || enable_elf64_loaders[front_end_num][i])))
		{
			const typename unisim::util::debug::Statement<ADDRESS> *stmt = elf64_loader->FindStatements(stmts, source_code_location);
			
			if(!ret) ret = stmt;
		}
	}
	
	return ret;
}

// unisim::service::interfaces::BackTrace<ADDRESS> (tagged)
template <typename CONFIG>
std::vector<typename CONFIG::ADDRESS> *Debugger<CONFIG>::GetBackTrace(unsigned int front_end_num) const
{
	unsigned int prc_num = sel_cpu[front_end_num];
	
	return dw_mach_state[front_end_num].GetBackTrace(prc_num);
}

template <typename CONFIG>
bool Debugger<CONFIG>::GetReturnAddress(unsigned int front_end_num, ADDRESS& ret_addr) const
{
	unsigned int prc_num = sel_cpu[front_end_num];

	return dw_mach_state[front_end_num].GetReturnAddress(prc_num, ret_addr);
}


// unisim::service::interfaces::DebugInfoLoading (tagged)
template <typename CONFIG>
bool Debugger<CONFIG>::LoadDebugInfo(unsigned int front_end_num, const char *filename)
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
			unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = new unisim::util::loader::coff_loader::CoffLoader<ADDRESS>(logger.DebugInfoStream(), logger.DebugWarningStream(), logger.DebugErrorStream());
			
			coff_loader->SetOption(unisim::util::loader::coff_loader::OPT_FILENAME, filename);
			coff_loader->SetOption(unisim::util::loader::coff_loader::OPT_VERBOSE, verbose);
			
			if(!coff_loader->Load())
			{
				logger << DebugError << "Loading input \"" << filename << "\" failed" << EndDebugError;
				delete coff_loader;
				return false;
			}
			coff_loaders.push_back(coff_loader);
			unsigned int i;
			for(i = 0; i < MAX_FRONT_ENDS; i++)
			{
				enable_coff_loaders[front_end_num].push_back((front_end_num == i));
			}
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
						unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = new unisim::util::loader::elf_loader::Elf32Loader<ADDRESS>();
						
						elf32_loader->SetDebugInfoStream(logger.DebugInfoStream());
						elf32_loader->SetDebugWarningStream(logger.DebugWarningStream());
						elf32_loader->SetDebugErrorStream(logger.DebugErrorStream());
						unsigned int prc_num;
						for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
						{
							elf32_loader->SetRegistersInterface(prc_num, prc_gate[prc_num]->registers_import);
							elf32_loader->SetMemoryInterface(prc_num, prc_gate[prc_num]->memory_import);
						}
						elf32_loader->SetOption(unisim::util::loader::elf_loader::OPT_FILENAME, filename);
						elf32_loader->SetOption(unisim::util::loader::elf_loader::OPT_VERBOSE, verbose);
						elf32_loader->SetOption(unisim::util::loader::elf_loader::OPT_PARSE_DWARF, parse_dwarf);
						elf32_loader->SetOption(unisim::util::loader::elf_loader::OPT_DWARF_REGISTER_NUMBER_MAPPING_FILENAME, unisim::kernel::Object::GetSimulator()->SearchSharedDataFile(dwarf_register_number_mapping_filename.c_str()).c_str());
						elf32_loader->SetOption(unisim::util::loader::elf_loader::OPT_DEBUG_DWARF, debug_dwarf);
						
						if(!elf32_loader->Load())
						{
							logger << DebugError << "Loading input \"" << filename << "\" failed" << EndDebugError;
							delete elf32_loader;
							return false;
						}
						elf32_loaders.push_back(elf32_loader);
						unsigned int i;
						for(i = 0; i < MAX_FRONT_ENDS; i++)
						{
							enable_elf32_loaders[i].push_back(front_end_num == i);
						}
						if(parse_dwarf)
						{
							dw_mach_state[front_end_num].Register(elf32_loader->GetDWARFHandler());
						}
						return true;
					}
					break;
				case 2:
					{
						unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = new unisim::util::loader::elf_loader::Elf64Loader<ADDRESS>();
						
						elf64_loader->SetDebugInfoStream(logger.DebugInfoStream());
						elf64_loader->SetDebugWarningStream(logger.DebugWarningStream());
						elf64_loader->SetDebugErrorStream(logger.DebugErrorStream());
						unsigned int prc_num;
						for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
						{
							elf64_loader->SetRegistersInterface(prc_num, prc_gate[prc_num]->registers_import);
							elf64_loader->SetMemoryInterface(prc_num, prc_gate[prc_num]->memory_import);
						}
						elf64_loader->SetOption(unisim::util::loader::elf_loader::OPT_FILENAME, filename);
						elf64_loader->SetOption(unisim::util::loader::elf_loader::OPT_VERBOSE, verbose);
						elf64_loader->SetOption(unisim::util::loader::elf_loader::OPT_PARSE_DWARF, parse_dwarf);
						elf64_loader->SetOption(unisim::util::loader::elf_loader::OPT_DWARF_REGISTER_NUMBER_MAPPING_FILENAME, unisim::kernel::Object::GetSimulator()->SearchSharedDataFile(dwarf_register_number_mapping_filename.c_str()).c_str());
						elf64_loader->SetOption(unisim::util::loader::elf_loader::OPT_DEBUG_DWARF, debug_dwarf);

						if(!elf64_loader->Load())
						{
							logger << DebugError << "Loading input \"" << filename << "\" failed" << EndDebugError;
							delete elf64_loader;
							return false;
						}
						elf64_loaders.push_back(elf64_loader);
						unsigned int i;
						for(i = 0; i < MAX_FRONT_ENDS; i++)
						{
							enable_elf64_loaders[i].push_back(front_end_num == i);
						}
						if(parse_dwarf)
						{
							dw_mach_state[front_end_num].Register(elf64_loader->GetDWARFHandler());
						}
						return true;
					}
					break;
			}
		}
	}
	logger << DebugWarning << "Can't handle symbol table of input \"" << filename << "\"" << EndDebugWarning;
	return false;
}

template <typename CONFIG>
bool Debugger<CONFIG>::EnableBinary(unsigned int front_end_num, const char *filename, bool enable)
{
	dw_mach_state[front_end_num].EnableBinary(filename, enable);
	
	bool found = false;
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
		const unisim::util::blob::Blob<ADDRESS> *blob = elf32_loader->GetBlob();
		if(blob->GetCapability() & unisim::util::blob::CAP_FILENAME)
		{
			if(strcmp(blob->GetFilename(), filename) == 0)
			{
				enable_elf32_loaders[front_end_num][i] = enable;
				found = true;
			}
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
		const unisim::util::blob::Blob<ADDRESS> *blob = elf64_loader->GetBlob();
		if(blob->GetCapability() & unisim::util::blob::CAP_FILENAME)
		{
			if(strcmp(blob->GetFilename(), filename) == 0)
			{
				enable_elf64_loaders[front_end_num][i] = enable;
				found = true;
			}
		}
	}

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; i++)
	{
		typename unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = coff_loaders[i];
		const unisim::util::blob::Blob<ADDRESS> *blob = coff_loader->GetBlob();
		if(blob->GetCapability() & unisim::util::blob::CAP_FILENAME)
		{
			if(strcmp(blob->GetFilename(), filename) == 0)
			{
				enable_coff_loaders[front_end_num][i] = enable;
				found = true;
			}
		}
	}
	
	return found;
}

template <typename CONFIG>
void Debugger<CONFIG>::EnumerateBinaries(unsigned int front_end_num, std::list<std::string>& lst) const
{
	// Note: For now all front-ends see all binaries
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
		const unisim::util::blob::Blob<ADDRESS> *blob = elf32_loader->GetBlob();
		if(blob->GetCapability() & unisim::util::blob::CAP_FILENAME)
		{
			lst.push_back(std::string(blob->GetFilename()));
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
		const unisim::util::blob::Blob<ADDRESS> *blob = elf64_loader->GetBlob();
		if(blob->GetCapability() & unisim::util::blob::CAP_FILENAME)
		{
			lst.push_back(std::string(blob->GetFilename()));
		}
	}

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; i++)
	{
		typename unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = coff_loaders[i];
		const unisim::util::blob::Blob<ADDRESS> *blob = coff_loader->GetBlob();
		if(blob->GetCapability() & unisim::util::blob::CAP_FILENAME)
		{
			lst.push_back(std::string(blob->GetFilename()));
		}
	}
}

template <typename CONFIG>
bool Debugger<CONFIG>::IsBinaryEnabled(unsigned int front_end_num, const char *filename) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
		const unisim::util::blob::Blob<ADDRESS> *blob = elf32_loader->GetBlob();
		if(blob->GetCapability() & unisim::util::blob::CAP_FILENAME)
		{
			if(strcmp(blob->GetFilename(), filename) == 0)
			{
				return enable_elf32_loaders[front_end_num][i];
			}
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
		const unisim::util::blob::Blob<ADDRESS> *blob = elf64_loader->GetBlob();
		if(blob->GetCapability() & unisim::util::blob::CAP_FILENAME)
		{
			if(strcmp(blob->GetFilename(), filename) == 0)
			{
				return enable_elf64_loaders[front_end_num][i];
			}
		}
	}

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; i++)
	{
		typename unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = coff_loaders[i];
		const unisim::util::blob::Blob<ADDRESS> *blob = coff_loader->GetBlob();
		if(blob->GetCapability() & unisim::util::blob::CAP_FILENAME)
		{
			if(strcmp(blob->GetFilename(), filename) == 0)
			{
				return enable_coff_loaders[front_end_num][i];
			}
		}
	}
	
	return false;
}


// unisim::service::interfaces::DataObjectLookup<ADDRESS> (tagged)
template <typename CONFIG>
unisim::util::debug::DataObjectRef<typename CONFIG::ADDRESS> Debugger<CONFIG>::FindDataObject(unsigned int front_end_num, const char *data_object_name) const
{
	unsigned int prc_num = sel_cpu[front_end_num];

	return dw_mach_state[front_end_num].FindDataObject(prc_num, data_object_name);
}

template <typename CONFIG>
void Debugger<CONFIG>::EnumerateDataObjectNames(unsigned int front_end_num, std::set<std::string>& name_set, typename unisim::service::interfaces::DataObjectLookup<ADDRESS>::Scope scope) const
{
	unsigned int prc_num = sel_cpu[front_end_num];
	
	dw_mach_state[front_end_num].EnumerateDataObjectNames(prc_num, name_set, scope);
}


// unisim::service::interfaces::SubProgramLookup<ADDRESS> (tagged)
template <typename CONFIG>
const unisim::util::debug::SubProgram<typename CONFIG::ADDRESS> *Debugger<CONFIG>::FindSubProgram(unsigned int front_end_num, const char *subprogram_name, const char *filename, const char *compilation_unit_name) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if(enable_elf32_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			const unisim::util::debug::SubProgram<ADDRESS> *subprogram = elf32_loader->FindSubProgram(subprogram_name, filename, compilation_unit_name);
			if(subprogram) return subprogram;
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if(enable_elf64_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			const unisim::util::debug::SubProgram<ADDRESS> *subprogram = elf64_loader->FindSubProgram(subprogram_name, filename, compilation_unit_name);
			if(subprogram) return subprogram;
		}
	}
	
	return 0;
}

template <typename CONFIG>
const unisim::util::debug::SubProgram<typename CONFIG::ADDRESS> *Debugger<CONFIG>::FindSubProgram(unsigned int front_end_num, ADDRESS pc, const char *filename) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		if(enable_elf32_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			const unisim::util::debug::SubProgram<ADDRESS> *subprogram = elf32_loader->FindSubProgram(pc, filename);
			if(subprogram) return subprogram;
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		if(enable_elf64_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			const unisim::util::debug::SubProgram<ADDRESS> *subprogram = elf64_loader->FindSubProgram(pc, filename);
			if(subprogram) return subprogram;
		}
	}
	
	return 0;
}

// unisim::service::interfaces::StackUnwinding (tagged)

template <typename CONFIG>
bool Debugger<CONFIG>::UnwindStack(unsigned int front_end_num, unsigned int frame_num)
{
	if(front_end_num >= MAX_FRONT_ENDS) return false;
	
	return dw_mach_state[front_end_num].UnwindStack(sel_cpu[front_end_num], frame_num);
}

// unisim::service::interfaces::Stubbing<ADDRESS> (tagged)

template <typename CONFIG>
void Debugger<CONFIG>::ScanStubs(unsigned int front_end_num, unisim::service::interfaces::StubScanner<ADDRESS>& scanner) const
{
	for(typename StubEventRegistry::const_iterator it = stub_event_registry[front_end_num].begin(); it != stub_event_registry[front_end_num].end(); ++it)
	{
		unisim::util::debug::Stub<ADDRESS> *stub = (*it).first;
		scanner.Append(stub);
	}
}

template <typename CONFIG>
bool Debugger<CONFIG>::SetStub(unsigned int front_end_num, unisim::util::debug::Stub<ADDRESS> *stub)
{
	unsigned int prc_num = sel_cpu[front_end_num];
	
	if(prc_num >= NUM_PROCESSORS) return false;
	
	StubEvent<ADDRESS> *stub_event = new StubEvent<ADDRESS>(stub);
	
	stub_event->SetProcessorNumber(prc_num);
	stub_event->SetFrontEndNumber(front_end_num);
	
	std::pair<typename StubEventRegistry::iterator, bool> r = stub_event_registry[front_end_num].insert(std::pair<unisim::util::debug::Stub<ADDRESS> *, StubEvent<ADDRESS> *>(stub, stub_event));
	
	if(!r.second)
	{
		// stub already set
		delete stub_event;
		return true;
	}

	const unisim::util::debug::SubProgram<ADDRESS> *subprogram = stub->GetSubProgram();
	
	ADDRESS brkp_addr = subprogram->GetAddress();
	
	unisim::util::debug::Breakpoint<ADDRESS> *brkp = new unisim::util::debug::Breakpoint<ADDRESS>(brkp_addr, stub_event);
	
	brkp->SetProcessorNumber(prc_num);
	brkp->SetFrontEndNumber(front_end_num);
		
	if(!breakpoint_registry.SetBreakpoint(brkp))
	{
		stub_event_registry[front_end_num].erase(r.first);
		delete brkp;
		delete stub_event;
		return false;
	}
	
	stub_event->Attach(brkp);
	
	stub_event->Catch();
	
	stub->SetProcessorNumber(prc_num);
	
	UpdateReportingRequirements(prc_num);
	
	return true;
}

template <typename CONFIG>
bool Debugger<CONFIG>::RemoveStub(unsigned int front_end_num, unisim::util::debug::Stub<ADDRESS> *stub)
{
	typename StubEventRegistry::iterator it = stub_event_registry[front_end_num].find(stub);
	
	if(it == stub_event_registry[front_end_num].end())
	{
		return false;
	}
	
	StubEvent<ADDRESS> *stub_event = (*it).second;
	
	unsigned int prc_num = stub_event->GetProcessorNumber();
	
	BreakpointRemover brkp_remover(*this);
	stub_event->Scan(brkp_remover);
	
	stub_event_registry[front_end_num].erase(it);
	stub_event->Release();
	
	if(!brkp_remover.status) return false;
	
	UpdateReportingRequirements(prc_num);
	
	return true;
}

// unisim::service::interfaces::Hooking<ADDRESS> (tagged)
template <typename CONFIG>
void Debugger<CONFIG>::ScanHooks(unsigned int front_end_num, unisim::service::interfaces::HookScanner<ADDRESS>& scanner) const
{
	for(typename HookEventRegistry::const_iterator it = hook_event_registry[front_end_num].begin(); it != hook_event_registry[front_end_num].end(); ++it)
	{
		unisim::util::debug::Hook<ADDRESS> *hook = (*it).first;
		scanner.Append(hook);
	}
}

template <typename CONFIG>
bool Debugger<CONFIG>::SetHook(unsigned int front_end_num, unisim::util::debug::Hook<ADDRESS> *hook)
{
	unsigned int prc_num = sel_cpu[front_end_num];
	
	if(prc_num >= NUM_PROCESSORS) return false;
	
	HookEvent<ADDRESS> *hook_event = new HookEvent<ADDRESS>(hook);
	
	hook_event->SetProcessorNumber(prc_num);
	hook_event->SetFrontEndNumber(front_end_num);
	
	std::pair<typename HookEventRegistry::iterator, bool> r = hook_event_registry[front_end_num].insert(std::pair<unisim::util::debug::Hook<ADDRESS> *, HookEvent<ADDRESS> *>(hook, hook_event));
	
	if(!r.second)
	{
		// hook already set
		delete hook_event;
		return true;
	}

	typename unisim::util::debug::Hook<ADDRESS>::Type hook_type = hook->GetType();
	
	if(hook_type == unisim::util::debug::SourceCodeHook<ADDRESS>::TYPE)
	{
		unisim::util::debug::SourceCodeHook<ADDRESS> *src_code_hook = static_cast<unisim::util::debug::SourceCodeHook<ADDRESS> *>(hook);
		
		std::vector<const unisim::util::debug::Statement<ADDRESS> *> stmts;
		
		FindStatements(front_end_num, stmts, src_code_hook->GetSourceCodeLocation(), src_code_hook->GetFilename());
		
		if(stmts.size())
		{
			for(typename std::vector<const unisim::util::debug::Statement<ADDRESS> *>::iterator it = stmts.begin(); it != stmts.end(); ++it)
			{
				const unisim::util::debug::Statement<ADDRESS> *stmt = *it;
				
				ADDRESS brkp_addr = stmt->GetAddress();
				
				unisim::util::debug::Breakpoint<ADDRESS> *brkp = new unisim::util::debug::Breakpoint<ADDRESS>(brkp_addr, hook_event);
				brkp->SetProcessorNumber(prc_num);
				brkp->SetFrontEndNumber(front_end_num);
				
				if(!breakpoint_registry.SetBreakpoint(brkp))
				{
					hook_event_registry[front_end_num].erase(r.first);
					delete brkp;
					delete hook_event;
					return false;
				}
				
				hook_event->Attach(brkp);
			}
		}
		else
		{
			// no statements
			hook_event_registry[front_end_num].erase(r.first);
			return false;
		}
	}
	else if(hook_type == unisim::util::debug::AddressHook<ADDRESS>::TYPE)
	{
		unisim::util::debug::AddressHook<ADDRESS> *addr_hook = static_cast<unisim::util::debug::AddressHook<ADDRESS> *>(hook);
		
		ADDRESS brkp_addr = addr_hook->GetAddress();
		
		unisim::util::debug::Breakpoint<ADDRESS> *brkp = new unisim::util::debug::Breakpoint<ADDRESS>(brkp_addr, hook_event);
		brkp->SetProcessorNumber(prc_num);
		brkp->SetFrontEndNumber(front_end_num);
		
		if(!breakpoint_registry.SetBreakpoint(brkp))
		{
			hook_event_registry[front_end_num].erase(r.first);
			delete brkp;
			delete hook_event;
			return false;
		}
		
		hook_event->Attach(brkp);
	}
	else if(hook_type == unisim::util::debug::SubProgramHook<ADDRESS>::TYPE)
	{
		unisim::util::debug::SubProgramHook<ADDRESS> *subprogram_hook = static_cast<unisim::util::debug::SubProgramHook<ADDRESS> *>(hook);
		
		const unisim::util::debug::SubProgram<ADDRESS> *subprogram = subprogram_hook->GetSubProgram();
		
		ADDRESS brkp_addr = subprogram->GetAddress();
		
		unisim::util::debug::Breakpoint<ADDRESS> *brkp = new unisim::util::debug::Breakpoint<ADDRESS>(brkp_addr, hook_event);
		brkp->SetProcessorNumber(prc_num);
		brkp->SetFrontEndNumber(front_end_num);
		
		if(!breakpoint_registry.SetBreakpoint(brkp))
		{
			hook_event_registry[front_end_num].erase(r.first);
			delete brkp;
			delete hook_event;
			return false;
		}
		
		hook_event->Attach(brkp);
	}
	
	hook_event->Catch();
	
	hook->SetProcessorNumber(prc_num);
	
	UpdateReportingRequirements(prc_num);
	
	return true;
}

template <typename CONFIG>
bool Debugger<CONFIG>::RemoveHook(unsigned int front_end_num, unisim::util::debug::Hook<ADDRESS> *hook)
{
	typename HookEventRegistry::iterator it = hook_event_registry[front_end_num].find(hook);
	
	if(it == hook_event_registry[front_end_num].end())
	{
		return false;
	}
	
	HookEvent<ADDRESS> *hook_event = (*it).second;
	
	unsigned int prc_num = hook_event->GetProcessorNumber();
	
	BreakpointRemover brkp_remover(*this);
	hook_event->Scan(brkp_remover);
	
	hook_event_registry[front_end_num].erase(it);
	hook_event->Release();
	
	if(!brkp_remover.status) return false;
	
	UpdateReportingRequirements(prc_num);
	
	return true;
}

// unisim::service::interfaces::DebugEventListener<ADDRESS> (tagged)

template <typename CONFIG>
void Debugger<CONFIG>::OnDebugEvent(unsigned int front_end_num, const unisim::util::debug::Event<ADDRESS> *event)
{
	typename unisim::util::debug::Event<ADDRESS>::Type event_type = event->GetType();
	if(event_type == StubEvent<ADDRESS>::TYPE)
	{
		const StubEvent<ADDRESS> *stub_event = static_cast<const StubEvent<ADDRESS> *>(event);
		
		unisim::util::debug::Stub<ADDRESS> *stub = stub_event->GetStub();
		unsigned int prc_num = stub_event->GetProcessorNumber();
		
		CallStub(front_end_num, prc_num, stub);
		return;
	}
	else if(event_type == HookEvent<ADDRESS>::TYPE)
	{
		const HookEvent<ADDRESS> *hook_event = static_cast<const HookEvent<ADDRESS> *>(event);
		
		unisim::util::debug::Hook<ADDRESS> *hook = hook_event->GetHook();
		unsigned int prc_num = hook_event->GetProcessorNumber();
		
		CallHook(front_end_num, prc_num, hook);
		return;
	}
	
	front_end_gate[front_end_num]->OnDebugEvent(event);
}

// unisim::service::interfaces::DebugTiming<TIME_TYPE> (tagged)

template <typename CONFIG>
const typename Debugger<CONFIG>::TIME_TYPE& Debugger<CONFIG>::DebugGetTime(unsigned int front_end_num) const
{
	return sel_prc_gate[front_end_num]->DebugGetTime();
}

template <typename CONFIG>
const typename Debugger<CONFIG>::TIME_TYPE& Debugger<CONFIG>::DebugGetTime(unsigned int front_end_num, unsigned int prc_num) const
{
	return prc_gate[prc_num]->DebugGetTime();
}

template <typename CONFIG>
bool Debugger<CONFIG>::Lock(int front_end_num)
{
	bool locked = false;
	if(front_end_num >= 0)
	{
		pthread_mutex_lock(&curr_front_end_num_mutex);
		if(curr_front_end_num != front_end_num)
		{
			curr_front_end_num = front_end_num;
			pthread_mutex_lock(&mutex);
			locked = true;
		}
		pthread_mutex_unlock(&curr_front_end_num_mutex);
	}
	else
	{
		pthread_mutex_lock(&mutex);
		locked = true;
	}
	return locked;
}

template <typename CONFIG>
void Debugger<CONFIG>::Unlock(int front_end_num)
{
	if(front_end_num >= 0)
	{
		pthread_mutex_lock(&curr_front_end_num_mutex);
		curr_front_end_num = -1;
		pthread_mutex_unlock(&mutex);
		pthread_mutex_unlock(&curr_front_end_num_mutex);
	}
	else
	{
		pthread_mutex_unlock(&mutex);
	}
}

template <typename CONFIG>
int Debugger<CONFIG>::AllocateId(unsigned int front_end_num)
{
	return next_id[front_end_num]++;
}

template <typename CONFIG>
void Debugger<CONFIG>::VariableBaseNotify(const unisim::kernel::VariableBase *var)
{
	unsigned int front_end_num;
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; ++front_end_num)
	{
		dw_mach_state[front_end_num].SetOption(unisim::util::debug::dwarf::OPT_VERBOSE, verbose);
		dw_mach_state[front_end_num].SetOption(unisim::util::debug::dwarf::OPT_DEBUG, debug_dwarf);
	}
	
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; i++)
	{
		typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
		if(var == &param_verbose)
		{
			elf32_loader->SetOption(unisim::util::loader::elf_loader::OPT_VERBOSE, verbose);
		}
		else if(var == &param_debug_dwarf)
		{
			elf32_loader->SetOption(unisim::util::loader::elf_loader::OPT_DEBUG_DWARF, debug_dwarf);
		}
		
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; i++)
	{
		typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
		if(var == &param_verbose)
		{
			elf64_loader->SetOption(unisim::util::loader::elf_loader::OPT_VERBOSE, verbose);
		}
		else if(var == &param_debug_dwarf)
		{
			elf64_loader->SetOption(unisim::util::loader::elf_loader::OPT_DEBUG_DWARF, debug_dwarf);
		}
	}
}

template <typename CONFIG>
void Debugger<CONFIG>::CallStub(unsigned int front_end_num, unsigned int prc_num, unisim::util::debug::Stub<ADDRESS> *stub)
{
	typedef typename unisim::util::debug::Stub<ADDRESS>::Parameters Parameters;
	Parameters params;
	
	const unisim::util::debug::SubProgram<ADDRESS> *subprogram = stub->GetSubProgram();
	
	unsigned int arity = subprogram->GetArity();
	
	for(unsigned int i = 0; i < arity; ++i)
	{
		const unisim::util::debug::FormalParameter *formal_param = subprogram->GetFormalParameter(i);
		
		const char *param_name = formal_param->GetName();
		
		unisim::util::debug::DataObjectRef<ADDRESS> param = dw_mach_state[front_end_num].FindDataObject(prc_num, param_name);
		
		params[param_name] = unisim::util::debug::DataObjectRef<ADDRESS>(param);
	}
	
	unisim::util::debug::DataObjectRef<ADDRESS> return_value = unisim::util::debug::DataObjectRef<ADDRESS>(dw_mach_state[front_end_num].FindDataObject(prc_num, "$return_value"));
	
	if(stub->Run(params, return_value))
	{
		UnwindStack(front_end_num);
	}
}

template <typename CONFIG>
void Debugger<CONFIG>::CallHook(unsigned int front_end_num, unsigned int prc_num, unisim::util::debug::Hook<ADDRESS> *hook)
{
	unisim::util::debug::DataObjectRef<ADDRESS> return_value = unisim::util::debug::DataObjectRef<ADDRESS>(dw_mach_state[front_end_num].FindDataObject(prc_num, "$return_value"));
	
	if(hook->Run(return_value))
	{
		UnwindStack(front_end_num);
	}
}

} // end of namespace debugger
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif
