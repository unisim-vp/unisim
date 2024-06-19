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
	, stack_frame_export()
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
	, processors()
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
	, max_stack_frames(256)
	, sel_cpu()
	, architecture()
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, param_dwarf_to_html_output_directory("dwarf-to-html-output-directory", this, dwarf_to_html_output_directory, "DWARF v2/v3 to HTML output directory")
	, param_dwarf_register_number_mapping_filename("dwarf-register-number-mapping-filename", this, dwarf_register_number_mapping_filename, "DWARF register number mapping filename")
	, param_parse_dwarf("parse-dwarf", this, parse_dwarf, "Enable/Disable parsing of DWARF debugging informations")
	, param_debug_dwarf("debug-dwarf", this, debug_dwarf, "Enable/Disable debugging of DWARF")
	, param_max_stack_frames("max-stack-frames", this, max_stack_frames, "Maximum number of stack frames (zero means unlimited)")
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
	, next_insn_event_set()
	, next_stmt_event_set()
	, finish_event_set()
	, trap_event_set()
	, fetch_stmt_event_set()
	, source_code_breakpoint_registry()
	, subprogram_breakpoint_registry()
	, stub_registry()
	, hook_registry()
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
	
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; ++front_end_num)
	{
		param_sel_cpu[front_end_num].SetFormat(unisim::kernel::VariableBase::FMT_DEC);
	}
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; ++prc_num)
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
	
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; ++front_end_num)
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
		
		std::stringstream stack_frame_export_name_sstr;
		stack_frame_export_name_sstr << "backtrace-export[" << front_end_num << "]";
		stack_frame_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::StackFrame<ADDRESS> >(stack_frame_export_name_sstr.str().c_str(), this);
		
		std::stringstream debug_info_loading_export_name_sstr;
		debug_info_loading_export_name_sstr << "debug-info-loading-export[" << front_end_num << "]";
		debug_info_loading_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::DebugInfoLoading>(debug_info_loading_export_name_sstr.str().c_str(), this);
		
		std::stringstream data_object_lookup_export_name_sstr;
		data_object_lookup_export_name_sstr << "data-object-lookup-export[" << front_end_num << "]";
		data_object_lookup_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::DataObjectLookup<ADDRESS> >(data_object_lookup_export_name_sstr.str().c_str(), this);
		
		std::stringstream subprogram_lookup_export_name_sstr;
		subprogram_lookup_export_name_sstr << "subprogram-lookup-export[" << front_end_num << "]";
		subprogram_lookup_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::SubProgramLookup<ADDRESS> >(subprogram_lookup_export_name_sstr.str().c_str(), this);
		
		std::stringstream stubbing_export_name_sstr;
		stubbing_export_name_sstr << "stubbing-export[" << front_end_num << "]";
		stubbing_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::Stubbing<ADDRESS> >(stubbing_export_name_sstr.str().c_str(), this);
		
		std::stringstream hooking_export_name_sstr;
		hooking_export_name_sstr << "hooking-export[" << front_end_num << "]";
		hooking_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::Hooking<ADDRESS> >(hooking_export_name_sstr.str().c_str(), this);
		
		std::stringstream debug_timing_export_name_sstr;
		debug_timing_export_name_sstr << "debug-timing-export[" << front_end_num << "]";
		debug_timing_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::DebugTiming<TIME_TYPE> >(debug_timing_export_name_sstr.str().c_str(), this);
		
		std::stringstream debug_processors_export_name_sstr;
		debug_processors_export_name_sstr << "debug-processors-export[" << front_end_num << "]";
		debug_processors_export[front_end_num] = new unisim::kernel::ServiceExport<unisim::service::interfaces::DebugProcessors<ADDRESS, TIME_TYPE> >(debug_processors_export_name_sstr.str().c_str(), this);
		
		std::stringstream debug_event_listener_import_name_sstr;
		debug_event_listener_import_name_sstr << "debug-event-listener-import[" << front_end_num << "]";
		debug_event_listener_import[front_end_num] = new unisim::kernel::ServiceImport<unisim::service::interfaces::DebugEventListener<ADDRESS> >(debug_event_listener_import_name_sstr.str().c_str(), this);
		
		std::stringstream debug_yielding_import_name_sstr;
		debug_yielding_import_name_sstr << "debug-yielding-import[" << front_end_num << "]";
		debug_yielding_import[front_end_num] = new unisim::kernel::ServiceImport<unisim::service::interfaces::DebugYielding>(debug_yielding_import_name_sstr.str().c_str(), this);
	}
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; ++prc_num)
	{
		std::stringstream prc_gate_name_sstr;
		prc_gate_name_sstr << "processor" << prc_num << "-gate";
		prc_gate[prc_num] = new ProcessorGate(prc_gate_name_sstr.str().c_str(), prc_num, this);
	}
	
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; ++front_end_num)
	{
		std::stringstream front_end_gate_name_sstr;
		front_end_gate_name_sstr << "front-end" << front_end_num << "-gate";
		front_end_gate[front_end_num] = new FrontEndGate(front_end_gate_name_sstr.str().c_str(), front_end_num, this); // Note: Processor gates shall be created before front-end gates
	}
	
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; ++front_end_num)
	{
		SelectProcessor(front_end_num, sel_cpu[front_end_num]);
	}
	
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; ++front_end_num)
	{
		for(prc_num = 0; prc_num < NUM_PROCESSORS; ++prc_num)
		{
			processors[front_end_num][prc_num] = new Processor(*this, front_end_num, prc_num);
		}
	}
}

template <typename CONFIG>
Debugger<CONFIG>::~Debugger()
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; ++i)
	{
		delete elf32_loaders[i];
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; ++i)
	{
		delete elf64_loaders[i];
	}

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; ++i)
	{
		delete coff_loaders[i];
	}

	unsigned int prc_num;
	unsigned int front_end_num;
	
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; ++front_end_num)
	{
		for(prc_num = 0; prc_num < NUM_PROCESSORS; ++prc_num)
		{
			if(stub_registry[front_end_num][prc_num].size())
			{
				do
				{
					typename StubRegistry::iterator it = stub_registry[front_end_num][prc_num].begin();
					StubHandler *stub_handler = (*it).second;
					delete stub_handler;
					stub_registry[front_end_num][prc_num].erase(it);
				}
				while(stub_registry[front_end_num][prc_num].size());
			}
			
			if(hook_registry[front_end_num][prc_num].size())
			{
				do
				{
					typename HookRegistry::iterator it = hook_registry[front_end_num][prc_num].begin();
					HookHandler *hook_handler = (*it).second;
					delete hook_handler;
					hook_registry[front_end_num][prc_num].erase(it);
				}
				while(hook_registry[front_end_num][prc_num].size());
			}
		}
	}

	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; ++front_end_num)
	{
		ClearEvents(front_end_num);
	}
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; ++prc_num)
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
	
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; ++front_end_num)
	{
		delete debug_yielding_request_export[front_end_num];
		delete debug_selecting_export[front_end_num];
		delete debug_event_trigger_export[front_end_num];
		delete disasm_export[front_end_num];
		delete memory_export[front_end_num];
		delete registers_export[front_end_num];
		delete symbol_table_lookup_export[front_end_num];
		delete stmt_lookup_export[front_end_num];
		delete stack_frame_export[front_end_num];
		delete debug_info_loading_export[front_end_num];
		delete data_object_lookup_export[front_end_num];
		delete subprogram_lookup_export[front_end_num];
		delete stubbing_export[front_end_num];
		delete hooking_export[front_end_num];
		delete debug_timing_export[front_end_num];
		delete debug_processors_export[front_end_num];
		delete debug_event_listener_import[front_end_num];
		delete debug_yielding_import[front_end_num];
	}
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; ++prc_num)
	{
		delete prc_gate[prc_num];
	}
	
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; ++front_end_num)
	{
		delete front_end_gate[front_end_num];
	}
	
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; ++front_end_num)
	{
		for(prc_num = 0; prc_num < NUM_PROCESSORS; ++prc_num)
		{
			delete processors[front_end_num][prc_num];
		}
	}
	
	pthread_mutex_destroy(&curr_front_end_num_mutex);
	pthread_mutex_destroy(&schedule_mutex);
	pthread_mutex_destroy(&mutex);
}

template <typename CONFIG>
bool Debugger<CONFIG>::BeginSetup()
{
	unsigned int prc_num;
	for(prc_num = 0; prc_num < NUM_PROCESSORS; ++prc_num)
	{
		requires_fetch_instruction_reporting[prc_num] = false;
		requires_commit_instruction_reporting[prc_num] = false;
		requires_memory_access_reporting[prc_num] = false;
		
		if(architecture[prc_num].empty())
		{
			logger << DebugWarning << "Architecture is unset for processor #" << prc_num << ", debugging support will extremely limited" << EndDebugWarning;
		}
	}
	
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; ++i)
	{
		delete elf32_loaders[i];
	}
	elf32_loaders.clear();

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; ++i)
	{
		delete elf64_loaders[i];
	}
	elf64_loaders.clear();

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; ++i)
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
				for(prc_num = 0; prc_num < NUM_PROCESSORS; ++prc_num)
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
				for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; ++front_end_num)
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
				for(prc_num = 0; prc_num < NUM_PROCESSORS; ++prc_num)
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
				for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; ++front_end_num)
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
				for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; ++front_end_num)
				{
					enable_coff_loaders[front_end_num].push_back(true);
				}
			}
			break;
	}
	
	typedef std::vector<const unisim::util::blob::Blob<ADDRESS> *> SiblingBlobs;
	const SiblingBlobs& sibling_blobs = blob->GetBlobs();
	
	for(typename SiblingBlobs::const_iterator it = sibling_blobs.begin(); it != sibling_blobs.end(); ++it)
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
		dw_mach_state[front_end_num].SetOption(unisim::util::debug::dwarf::OPT_MAX_STACK_FRAMES, (int) max_stack_frames);
	
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
	requires_fetch_instruction_reporting[prc_num] = breakpoint_registry.HasBreakpoints(prc_num) || !fetch_insn_event_set[prc_num].empty() || !fetch_stmt_event_set[prc_num].empty();
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

template <typename CONFIG>
bool Debugger<CONFIG>::IsScheduled(unsigned int front_end_num) const
{
	return schedule & (1 << front_end_num);
}

// Exports to CPUs

// unisim::service::interfaces::DebugYielding (tagged)
template <typename CONFIG>
void Debugger<CONFIG>::DebugYield(unsigned int prc_num)
{
	if(likely(IsScheduleEmpty())) return;

	unsigned int front_end_num = 0;
	uint64_t invalidate_frames_mask = 0;
	
	while(NextScheduledFrontEnd(front_end_num))
	{
		front_end_gate[front_end_num]->DebugYield();
		if(dw_mach_state[front_end_num].DirtyFrames())
		{
			invalidate_frames_mask |= 1 << front_end_num;
		}
	}
	if(unlikely(invalidate_frames_mask))
	{
		InvalidateDirtyFrames(invalidate_frames_mask);
	}
}

template <typename CONFIG>
void Debugger<CONFIG>::InvalidateDirtyFrames(uint64_t mask)
{
	unsigned int front_end_num;
	while(unlikely(unisim::util::arithmetic::BitScanForward(front_end_num, mask)))
	{
		dw_mach_state[front_end_num].InvalidateDirtyFrames();
		mask &= ~(1 << front_end_num);
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
		uint64_t invalidate_frames_mask = 0;
		
		unsigned int front_end_num;
		
		for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; ++front_end_num)
		{
			Dispatcher<unisim::util::debug::Watchpoint<ADDRESS> > watchpoint_dispatcher(*this, front_end_num, overlook);
			
			// Note: this function may alter local variable "overlook"
			if(watchpoint_registry.template FindWatchpoints<Dispatcher<unisim::util::debug::Watchpoint<ADDRESS> > >(mat, mt, addr, size, prc_num, front_end_num, watchpoint_dispatcher))
			{
				ScheduleFrontEnd(front_end_num);
				if(dw_mach_state[front_end_num].DirtyFrames())
				{
					invalidate_frames_mask |= 1 << front_end_num;
				}
			}
		}
		
		if(unlikely(invalidate_frames_mask))
		{
			InvalidateDirtyFrames(invalidate_frames_mask);
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
		uint64_t invalidate_frames_mask = 0;
		
		// beware of reentrancy
		typename CommitInsnEventSet::size_type i = 0, n = commit_insn_event_set[prc_num].size();
		CommitInsnEvent *commit_insn_events[n];
		for(typename CommitInsnEventSet::iterator it = commit_insn_event_set[prc_num].begin(); it != commit_insn_event_set[prc_num].end(); ++i, ++it)
		{
			CommitInsnEvent *commit_insn_event = *it;
			commit_insn_event->SetAddress(addr);
			commit_insn_event->SetLength(length);
			commit_insn_event->Catch();
			commit_insn_events[i] = commit_insn_event;
		}
		for(i = 0; i < n; ++i)
		{
			CommitInsnEvent *commit_insn_event = commit_insn_events[i];
			unsigned int front_end_num = commit_insn_event->GetFrontEndNumber();
			commit_insn_event->Trigger();
			commit_insn_event->Release();
			ScheduleFrontEnd(front_end_num);
			if(dw_mach_state[front_end_num].DirtyFrames())
			{
				invalidate_frames_mask |= 1 << front_end_num;
			}
		}
		
		if(unlikely(invalidate_frames_mask))
		{
			InvalidateDirtyFrames(invalidate_frames_mask);
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
	uint64_t invalidate_frames_mask = 0;
	
	if(unlikely(breakpoint_registry.HasBreakpoints(next_addr, prc_num)))
	{
		unsigned int front_end_num;
		
		for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; ++front_end_num)
		{
			Dispatcher<unisim::util::debug::Breakpoint<ADDRESS> > breakpoint_dispatcher(*this, front_end_num);
			
			if(breakpoint_registry.template FindBreakpoints<Dispatcher<unisim::util::debug::Breakpoint<ADDRESS> > >(next_addr, prc_num, front_end_num, breakpoint_dispatcher))
			{
				ScheduleFrontEnd(front_end_num);
				if(dw_mach_state[front_end_num].DirtyFrames())
				{
					invalidate_frames_mask |= 1 << front_end_num;
				}
			}
		}
	}
	
	if(unlikely(!fetch_insn_event_set[prc_num].empty()))
	{
		// beware of reentrancy
		typename FetchInsnEventSet::size_type i = 0, n = fetch_insn_event_set[prc_num].size();
		FetchInsnEvent *fetch_insn_events[n];
		for(typename FetchInsnEventSet::iterator it = fetch_insn_event_set[prc_num].begin(); it != fetch_insn_event_set[prc_num].end(); ++i, ++it)
		{
			FetchInsnEvent *fetch_insn_event = *it;
			fetch_insn_event->SetAddress(next_addr);
			fetch_insn_event->Catch();
			fetch_insn_events[i] = fetch_insn_event;
		}
		for(i = 0; i < n; ++i)
		{
			FetchInsnEvent *fetch_insn_event = fetch_insn_events[i];
			unsigned int front_end_num = fetch_insn_event->GetFrontEndNumber();
			fetch_insn_event->Trigger();
			fetch_insn_event->Release();
			ScheduleFrontEnd(front_end_num);
			if(dw_mach_state[front_end_num].DirtyFrames())
			{
				invalidate_frames_mask |= 1 << front_end_num;
			}
		}
	}
	
	if(unlikely(!fetch_stmt_event_set[prc_num].empty()))
	{
		// beware of reentrancy
		typename FetchStmtEventSet::size_type i = 0, n = fetch_stmt_event_set[prc_num].size(), m = 0;
		FetchStmtEvent *fetch_stmt_events[n];
		for(typename FetchStmtEventSet::iterator it = fetch_stmt_event_set[prc_num].begin(); it != fetch_stmt_event_set[prc_num].end(); ++it)
		{
			FetchStmtEvent *fetch_stmt_event = *it;
			unsigned int front_end_num = fetch_stmt_event->GetFrontEndNumber();
			
			if(fetch_stmt_event->Stepped(next_addr))
			{
				fetch_stmt_event->Catch();
				fetch_stmt_events[m] = fetch_stmt_event;
				++m;
			}
			
			if(dw_mach_state[front_end_num].DirtyFrames())
			{
				invalidate_frames_mask |= 1 << front_end_num;
			}
		}
		for(i = 0; i < m; ++i)
		{
			FetchStmtEvent *fetch_stmt_event = fetch_stmt_events[i];
			unsigned int front_end_num = fetch_stmt_event->GetFrontEndNumber();
			fetch_stmt_event->Trigger();
			fetch_stmt_event->Release();
			ScheduleFrontEnd(front_end_num);
			if(dw_mach_state[front_end_num].DirtyFrames())
			{
				invalidate_frames_mask |= 1 << front_end_num;
			}
		}
	}
	
	if(unlikely(invalidate_frames_mask))
	{
		InvalidateDirtyFrames(invalidate_frames_mask);
	}
}


// unisim::service::interfaces::TrapReporting (tagged)
template <typename CONFIG>
void Debugger<CONFIG>::ReportTrap(unsigned int prc_num, const unisim::kernel::Object *obj, const std::string& msg)
{
	if(unlikely(!trap_event_set[prc_num].empty()))
	{
		uint64_t invalidate_frames_mask = 0;
		
		// beware of reentrancy
		typename TrapEventSet::size_type i = 0, n = trap_event_set[prc_num].size();
		TrapEvent *trap_events[n];
		for(typename TrapEventSet::iterator it = trap_event_set[prc_num].begin(); it != trap_event_set[prc_num].end(); ++i, ++it)
		{
			TrapEvent *trap_event = *it;
			trap_event->SetTrapObject(obj);
			trap_event->SetTrapMessage(msg);
			trap_event->Catch();
			trap_events[i] = trap_event;
		}
		for(i = 0; i < n; ++i)
		{
			TrapEvent *trap_event = trap_events[i];
			unsigned int front_end_num = trap_event->GetFrontEndNumber();
			trap_event->Trigger();
			trap_event->Release();
			ScheduleFrontEnd(front_end_num);
			if(dw_mach_state[front_end_num].DirtyFrames())
			{
				invalidate_frames_mask |= 1 << front_end_num;
			}
		}
		
		if(unlikely(invalidate_frames_mask))
		{
			InvalidateDirtyFrames(invalidate_frames_mask);
		}
	}
}

// Export to Front-ends

// unisim::service::interfaces::DebugSelecting (tagged)
template <typename CONFIG>
bool Debugger<CONFIG>::SelectProcessor(unsigned int front_end_num, unsigned int prc_num)
{
	if(front_end_num >= MAX_FRONT_ENDS) return false;
	if(prc_num >= NUM_PROCESSORS) return false;
	
	sel_prc_gate[front_end_num] = prc_gate[prc_num];
	sel_cpu[front_end_num] = prc_num;
	
	return true;
}

template <typename CONFIG>
unsigned int Debugger<CONFIG>::GetSelectedProcessor(unsigned int front_end_num) const
{
	if(front_end_num >= MAX_FRONT_ENDS) return 0;
	
	return sel_cpu[front_end_num];
}

// unisim::service::interfaces::DebugYieldingRequest (tagged)
template <typename CONFIG>
void Debugger<CONFIG>::DebugYieldRequest(unsigned int front_end_num)
{
	if(front_end_num >= MAX_FRONT_ENDS) return;

	ScheduleFrontEnd(front_end_num);
}

// unisim::service::interfaces::DebugEventFactory<ADDRESS> (tagged)

template <typename CONFIG>
unisim::util::debug::Breakpoint<typename CONFIG::ADDRESS> *Debugger<CONFIG>::CreateBreakpoint(unsigned int front_end_num, ADDRESS addr, bool internal)
{
	return new Breakpoint(front_end_num, sel_cpu[front_end_num], addr, internal ? -1 : AllocateId(front_end_num));
}

template <typename CONFIG>
unisim::util::debug::Breakpoint<typename CONFIG::ADDRESS> *Debugger<CONFIG>::CreateBreakpoint(unsigned int front_end_num, unsigned int prc_num, ADDRESS addr, bool internal)
{
	return new Breakpoint(front_end_num, prc_num, addr, internal ? -1 : AllocateId(front_end_num));
}

template <typename CONFIG>
unisim::util::debug::Watchpoint<typename CONFIG::ADDRESS> *Debugger<CONFIG>::CreateWatchpoint(unsigned int front_end_num, unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size, bool overlook, bool internal)
{
	return new Watchpoint(front_end_num, sel_cpu[front_end_num], mat, mt, addr, size, overlook, internal ? -1 : AllocateId(front_end_num));
}

template <typename CONFIG>
unisim::util::debug::Watchpoint<typename CONFIG::ADDRESS> *Debugger<CONFIG>::CreateWatchpoint(unsigned int front_end_num, unsigned int prc_num, unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size, bool overlook, bool internal)
{
	return new Watchpoint(front_end_num, prc_num, mat, mt, addr, size, overlook, internal ? -1 : AllocateId(front_end_num));
}

template <typename CONFIG>
unisim::util::debug::FetchInsnEvent<typename CONFIG::ADDRESS> *Debugger<CONFIG>::CreateFetchInsnEvent(unsigned int front_end_num, bool internal)
{
	return new FetchInsnEvent(*this, front_end_num, sel_cpu[front_end_num], internal);
}

template <typename CONFIG>
unisim::util::debug::FetchInsnEvent<typename CONFIG::ADDRESS> *Debugger<CONFIG>::CreateFetchInsnEvent(unsigned int front_end_num, unsigned int prc_num, bool internal)
{
	return new FetchInsnEvent(*this, front_end_num, prc_num, internal);
}

template <typename CONFIG>
unisim::util::debug::FetchStmtEvent<typename CONFIG::ADDRESS> *Debugger<CONFIG>::CreateFetchStmtEvent(unsigned int front_end_num, bool internal)
{
	return new FetchStmtEvent(*this, front_end_num, sel_cpu[front_end_num], internal);
}

template <typename CONFIG>
unisim::util::debug::FetchStmtEvent<typename CONFIG::ADDRESS> *Debugger<CONFIG>::CreateFetchStmtEvent(unsigned int front_end_num, unsigned int prc_num, bool internal)
{
	return new FetchStmtEvent(*this, front_end_num, prc_num, internal);
}

template <typename CONFIG>
unisim::util::debug::CommitInsnEvent<typename CONFIG::ADDRESS> *Debugger<CONFIG>::CreateCommitInsnEvent(unsigned int front_end_num, bool internal)
{
	return new CommitInsnEvent(*this, front_end_num, sel_cpu[front_end_num], internal);
}

template <typename CONFIG>
unisim::util::debug::CommitInsnEvent<typename CONFIG::ADDRESS> *Debugger<CONFIG>::CreateCommitInsnEvent(unsigned int front_end_num, unsigned int prc_num, bool internal)
{
	return new CommitInsnEvent(*this, front_end_num, prc_num, internal);
}

template <typename CONFIG>
unisim::util::debug::NextInsnEvent<typename CONFIG::ADDRESS> *Debugger<CONFIG>::CreateNextInsnEvent(unsigned int front_end_num, bool internal)
{
	return new NextInsnEvent(*this, front_end_num, sel_cpu[front_end_num], internal);
}

template <typename CONFIG>
unisim::util::debug::NextInsnEvent<typename CONFIG::ADDRESS> *Debugger<CONFIG>::CreateNextInsnEvent(unsigned int front_end_num, unsigned int prc_num, bool internal)
{
	return new NextInsnEvent(*this, front_end_num, prc_num, internal);
}

template <typename CONFIG>
unisim::util::debug::NextStmtEvent<typename CONFIG::ADDRESS> *Debugger<CONFIG>::CreateNextStmtEvent(unsigned int front_end_num, bool internal)
{
	return new NextStmtEvent(*this, front_end_num, sel_cpu[front_end_num], internal);
}

template <typename CONFIG>
unisim::util::debug::NextStmtEvent<typename CONFIG::ADDRESS> *Debugger<CONFIG>::CreateNextStmtEvent(unsigned int front_end_num, unsigned int prc_num, bool internal)
{
	return new NextStmtEvent(*this, front_end_num, prc_num, internal);
}

template <typename CONFIG>
unisim::util::debug::FinishEvent<typename CONFIG::ADDRESS> *Debugger<CONFIG>::CreateFinishEvent(unsigned int front_end_num, bool internal)
{
	return new FinishEvent(*this, front_end_num, sel_cpu[front_end_num], internal);
}

template <typename CONFIG>
unisim::util::debug::FinishEvent<typename CONFIG::ADDRESS> *Debugger<CONFIG>::CreateFinishEvent(unsigned int front_end_num, unsigned int prc_num, bool internal)
{
	return new FinishEvent(*this, front_end_num, prc_num, internal);
}

template <typename CONFIG>
unisim::util::debug::TrapEvent<typename CONFIG::ADDRESS> *Debugger<CONFIG>::CreateTrapEvent(unsigned int front_end_num, bool internal)
{
	return new TrapEvent(*this, front_end_num, sel_cpu[front_end_num], internal);
}

template <typename CONFIG>
unisim::util::debug::TrapEvent<typename CONFIG::ADDRESS> *Debugger<CONFIG>::CreateTrapEvent(unsigned int front_end_num, unsigned int prc_num, bool internal)
{
	return new TrapEvent(*this, front_end_num, prc_num, internal);
}

template <typename CONFIG>
unisim::util::debug::SourceCodeBreakpoint<typename CONFIG::ADDRESS> *Debugger<CONFIG>::CreateSourceCodeBreakpoint(unsigned int front_end_num, const unisim::util::debug::SourceCodeLocation& source_code_location, const std::string& filename, bool internal)
{
	return new SourceCodeBreakpoint(*this, front_end_num, sel_cpu[front_end_num], source_code_location, filename, internal ? -1 : AllocateId(front_end_num));
}

template <typename CONFIG>
unisim::util::debug::SourceCodeBreakpoint<typename CONFIG::ADDRESS> *Debugger<CONFIG>::CreateSourceCodeBreakpoint(unsigned int front_end_num, unsigned int prc_num, const unisim::util::debug::SourceCodeLocation& source_code_location, const std::string& filename, bool internal)
{
	return new SourceCodeBreakpoint(*this, front_end_num, prc_num, source_code_location, filename, internal ? -1 : AllocateId(front_end_num));
}

template <typename CONFIG>
unisim::util::debug::SubProgramBreakpoint<typename CONFIG::ADDRESS> *Debugger<CONFIG>::CreateSubProgramBreakpoint(unsigned int front_end_num, const unisim::util::debug::SubProgram<ADDRESS> *subprogram, bool internal)
{
	return new SubProgramBreakpoint(*this, front_end_num, sel_cpu[front_end_num], subprogram, internal ? -1 : AllocateId(front_end_num));
}

template <typename CONFIG>
unisim::util::debug::SubProgramBreakpoint<typename CONFIG::ADDRESS> *Debugger<CONFIG>::CreateSubProgramBreakpoint(unsigned int front_end_num, unsigned int prc_num, const unisim::util::debug::SubProgram<ADDRESS> *subprogram, bool internal)
{
	return new SubProgramBreakpoint(*this, front_end_num, prc_num, subprogram, internal ? -1 : AllocateId(front_end_num));
}

// unisim::service::interfaces::DebugEventTrigger<ADDRESS> (tagged)

template <typename CONFIG>
template <typename EVENT_SET, typename EVENT>
bool Debugger<CONFIG>::AddEvent(unsigned int front_end_num, EVENT_SET (&event_set)[], unisim::util::debug::Event<ADDRESS> *_event)
{
	EVENT *event = dynamic_cast<EVENT *>(_event);
	if(!event || (event->GetFrontEndNumber() != front_end_num)) return false;
	unsigned int prc_num = event->GetProcessorNumber();
	if(prc_num >= NUM_PROCESSORS) return false;
	if(event_set[prc_num].find(event) != event_set[prc_num].end()) return true; // already set
	if(!event->Set()) return false;
	event_set[prc_num].insert(event);
	event->Catch();
	return true;
}

template <typename CONFIG>
template <typename EVENT_SET, typename EVENT>
bool Debugger<CONFIG>::RemoveEvent(unsigned int front_end_num, EVENT_SET (&event_set)[], unisim::util::debug::Event<ADDRESS> *_event)
{
	EVENT *event = dynamic_cast<EVENT *>(_event);
	if(!event || (event->GetFrontEndNumber() != front_end_num)) return false;
	unsigned int prc_num = event->GetProcessorNumber();
	if(prc_num >= NUM_PROCESSORS) return false;
	typename EVENT_SET::const_iterator it = event_set[prc_num].find(event);
	if(it == event_set[prc_num].end()) return false;
	if(!event->Remove()) return false;
	event->Release();
	event_set[prc_num].erase(it);
	return true;
}

template <typename CONFIG>
bool Debugger<CONFIG>::Listen(unsigned int front_end_num, unisim::util::debug::Event<ADDRESS> *event)
{
//	std::cerr << "Listen(" << front_end_num << ", " << event << ")" << std::endl;
	
	if(Breakpoint::IsInstanceOf(event))
	{
		Breakpoint *brkp = dynamic_cast<Breakpoint *>(event);
		
		if(!brkp || (brkp->GetFrontEndNumber() != front_end_num) || !breakpoint_registry.SetBreakpoint(brkp)) return false;
	}
	else if(Watchpoint::IsInstanceOf(event))
	{
		Watchpoint *wp = dynamic_cast<Watchpoint *>(event);
		
		if(!wp || (wp->GetFrontEndNumber() != front_end_num) || !watchpoint_registry.SetWatchpoint(wp)) return false;
	}
	else if(FetchInsnEvent::IsInstanceOf(event))
	{
		if(!this->template AddEvent<FetchInsnEventSet, FetchInsnEvent>(front_end_num, fetch_insn_event_set, event)) return false;
	}
	else if(CommitInsnEvent::IsInstanceOf(event))
	{
		if(!this->template AddEvent<CommitInsnEventSet, CommitInsnEvent>(front_end_num, commit_insn_event_set, event)) return false;
	}
	else if(NextInsnEvent::IsInstanceOf(event))
	{
		if(!this->template AddEvent<NextInsnEventSet, NextInsnEvent>(front_end_num, next_insn_event_set, event)) return false;
	}
	else if(NextStmtEvent::IsInstanceOf(event))
	{
		if(!this->template AddEvent<NextStmtEventSet, NextStmtEvent>(front_end_num, next_stmt_event_set, event)) return false;
	}
	else if(FinishEvent::IsInstanceOf(event))
	{
		if(!this->template AddEvent<FinishEventSet, FinishEvent>(front_end_num, finish_event_set, event)) return false;
	}
	else if(TrapEvent::IsInstanceOf(event))
	{
		if(!this->template AddEvent<TrapEventSet, TrapEvent>(front_end_num, trap_event_set, event)) return false;
	}
	else if(FetchStmtEvent::IsInstanceOf(event))
	{
		if(!this->template AddEvent<FetchStmtEventSet, FetchStmtEvent>(front_end_num, fetch_stmt_event_set, event)) return false;
	}
	else if(SourceCodeBreakpoint::IsInstanceOf(event))
	{
		SourceCodeBreakpoint *src_code_brkp = dynamic_cast<SourceCodeBreakpoint *>(event);
		if(!src_code_brkp || (src_code_brkp->GetFrontEndNumber() != front_end_num)) return false;
		std::pair<typename SourceCodeBreakpointRegistry::iterator, bool> r = source_code_breakpoint_registry[front_end_num].insert(src_code_brkp);
		if(!r.second) return true; // source code breakpoint already set
		if(!src_code_brkp->Set())
		{
			source_code_breakpoint_registry[front_end_num].erase(r.first);
			return false;
		}
	}
	else if(SubProgramBreakpoint::IsInstanceOf(event))
	{
		SubProgramBreakpoint *subprogram_brkp = dynamic_cast<SubProgramBreakpoint *>(event);
		if(!subprogram_brkp || (subprogram_brkp->GetFrontEndNumber() != front_end_num)) return false;
		std::pair<typename SubProgramBreakpointRegistry::iterator, bool> r = subprogram_breakpoint_registry[front_end_num].insert(subprogram_brkp);
		if(!r.second) return true; // subprogram breakpoint already set
		if(!subprogram_brkp->Set())
		{
			subprogram_breakpoint_registry[front_end_num].erase(r.first);
			return false;
		}
	}
	else
	{
		return false;
	}
	
	if(!event->HasEventListeners())
	{
		event->AddEventListener(front_end_gate[front_end_num]->debug_event_listener_import);
	}
	
	UpdateReportingRequirements(event->GetProcessorNumber());
	
	return true;
}

template <typename CONFIG>
bool Debugger<CONFIG>::Unlisten(unsigned int front_end_num, unisim::util::debug::Event<ADDRESS> *event)
{
	if(Breakpoint::IsInstanceOf(event))
	{
		Breakpoint *brkp = dynamic_cast<Breakpoint *>(event);
		if(!brkp || (brkp->GetFrontEndNumber() != front_end_num) || !breakpoint_registry.RemoveBreakpoint(brkp)) return false;
	}
	else if(Watchpoint::IsInstanceOf(event))
	{
		Watchpoint *wp = dynamic_cast<Watchpoint *>(event);
		if(!wp || (wp->GetFrontEndNumber() != front_end_num) || !watchpoint_registry.RemoveWatchpoint(wp)) return false;
	}
	else if(FetchInsnEvent::IsInstanceOf(event))
	{
		if(!this->template RemoveEvent<FetchInsnEventSet, FetchInsnEvent>(front_end_num, fetch_insn_event_set, event)) return false;
	}
	else if(CommitInsnEvent::IsInstanceOf(event))
	{
		if(!this->template RemoveEvent<CommitInsnEventSet, CommitInsnEvent>(front_end_num, commit_insn_event_set, event)) return false;
	}
	else if(NextInsnEvent::IsInstanceOf(event))
	{
		if(!this->template RemoveEvent<NextInsnEventSet, NextInsnEvent>(front_end_num, next_insn_event_set, event)) return false;
	}
	else if(NextStmtEvent::IsInstanceOf(event))
	{
		if(!this->template RemoveEvent<NextStmtEventSet, NextStmtEvent>(front_end_num, next_stmt_event_set, event)) return false;
	}
	else if(FinishEvent::IsInstanceOf(event))
	{
		if(!this->template RemoveEvent<FinishEventSet, FinishEvent>(front_end_num, finish_event_set, event)) return false;
	}
	else if(TrapEvent::IsInstanceOf(event))
	{
		if(!this->template RemoveEvent<TrapEventSet, TrapEvent>(front_end_num, trap_event_set, event)) return false;
	}
	else if(FetchStmtEvent::IsInstanceOf(event))
	{
		if(!this->template RemoveEvent<FetchStmtEventSet, FetchStmtEvent>(front_end_num, fetch_stmt_event_set, event)) return false;
	}
	else if(SourceCodeBreakpoint::IsInstanceOf(event))
	{
		SourceCodeBreakpoint *src_code_brkp = dynamic_cast<SourceCodeBreakpoint *>(event);
		if(!src_code_brkp || (src_code_brkp->GetFrontEndNumber() != front_end_num)) return false;
		typename SourceCodeBreakpointRegistry::const_iterator it = source_code_breakpoint_registry[front_end_num].find(src_code_brkp);
		if(it == source_code_breakpoint_registry[front_end_num].end()) return false;
		if(!src_code_brkp->Remove()) return false;
		source_code_breakpoint_registry[front_end_num].erase(it);
	}
	else if(SubProgramBreakpoint::IsInstanceOf(event))
	{
		SubProgramBreakpoint *subprogram_brkp = dynamic_cast<SubProgramBreakpoint *>(event);
		if(!subprogram_brkp || (subprogram_brkp->GetFrontEndNumber() != front_end_num)) return false;
		typename SubProgramBreakpointRegistry::const_iterator it = subprogram_breakpoint_registry[front_end_num].find(subprogram_brkp);
		if(it == subprogram_breakpoint_registry[front_end_num].end()) return false;
		if(!subprogram_brkp->Remove()) return false;
		subprogram_breakpoint_registry[front_end_num].erase(it);
	}
	else
	{
		// ignore
		return false;
	}
	
	UpdateReportingRequirements(event->GetProcessorNumber());
	
	return true;
}

template <typename CONFIG>
bool Debugger<CONFIG>::IsEventListened(unsigned int front_end_num, unisim::util::debug::Event<ADDRESS> *event) const
{
	unsigned int prc_num = event->GetProcessorNumber();
	
	if(Breakpoint::IsInstanceOf(event))
	{
		Breakpoint *brkp = dynamic_cast<Breakpoint *>(event);
		return brkp && breakpoint_registry.HasBreakpoint(brkp);
	}
	else if(Watchpoint::IsInstanceOf(event))
	{
		Watchpoint *wp = dynamic_cast<Watchpoint *>(event);
		return wp && watchpoint_registry.HasWatchpoint(wp);
	}
	else if(FetchInsnEvent::IsInstanceOf(event))
	{
		FetchInsnEvent *fetch_insn_event = dynamic_cast<FetchInsnEvent *>(event);
		return fetch_insn_event && (fetch_insn_event_set[prc_num].find(fetch_insn_event) != fetch_insn_event_set[prc_num].end());
	}
	else if(CommitInsnEvent::IsInstanceOf(event))
	{
		CommitInsnEvent *commit_insn_event = dynamic_cast<CommitInsnEvent *>(event);
		return commit_insn_event && (commit_insn_event_set[prc_num].find(commit_insn_event) != commit_insn_event_set[prc_num].end());
	}
	else if(NextInsnEvent::IsInstanceOf(event))
	{
		NextInsnEvent *next_insn_event = dynamic_cast<NextInsnEvent *>(event);
		return next_insn_event && (next_insn_event_set[prc_num].find(next_insn_event) != next_insn_event_set[prc_num].end());
	}
	else if(NextStmtEvent::IsInstanceOf(event))
	{
		NextStmtEvent *next_stmt_event = dynamic_cast<NextStmtEvent *>(event);
		return next_stmt_event && (next_stmt_event_set[prc_num].find(next_stmt_event) != next_stmt_event_set[prc_num].end());
	}
	else if(FinishEvent::IsInstanceOf(event))
	{
		FinishEvent *commit_insn_event = dynamic_cast<FinishEvent *>(event);
		return commit_insn_event && (finish_event_set[prc_num].find(commit_insn_event) != finish_event_set[prc_num].end());
	}
	else if(TrapEvent::IsInstanceOf(event))
	{
		TrapEvent *trap_event = dynamic_cast<TrapEvent *>(event);
		return trap_event && (trap_event_set[prc_num].find(trap_event) != trap_event_set[prc_num].end());
	}
	else if(FetchStmtEvent::IsInstanceOf(event))
	{
		FetchStmtEvent *fetch_stmt_event = dynamic_cast<FetchStmtEvent *>(event);
		return fetch_stmt_event && (fetch_stmt_event_set[prc_num].find(fetch_stmt_event) != fetch_stmt_event_set[prc_num].end());
	}
	else if(SourceCodeBreakpoint::IsInstanceOf(event))
	{
		SourceCodeBreakpoint *src_code_brkp = dynamic_cast<SourceCodeBreakpoint *>(event);
		return src_code_brkp && (source_code_breakpoint_registry[front_end_num].find(src_code_brkp) != source_code_breakpoint_registry[front_end_num].end());
	}
	else if(SubProgramBreakpoint::IsInstanceOf(event))
	{
		SubProgramBreakpoint *subprogram_brkp = dynamic_cast<SubProgramBreakpoint *>(event);
		return subprogram_brkp && (subprogram_breakpoint_registry[front_end_num].find(subprogram_brkp) != subprogram_breakpoint_registry[front_end_num].end());
	}
	return false;
}

template <typename CONFIG>
template <typename EVENT_SET, typename EVENT, typename SCANNER>
void Debugger<CONFIG>::ScanEvents(unsigned int front_end_num, const EVENT_SET (&event_set)[], SCANNER& scanner) const
{
	typedef std::vector<unisim::util::debug::Event<ADDRESS> *> ListenedEvents;
	ListenedEvents listened_events;
	
	for(unsigned int prc_num = 0; prc_num < NUM_PROCESSORS; ++prc_num)
	{
		for(typename EVENT_SET::const_iterator it = event_set[prc_num].begin(); it != event_set[prc_num].end(); ++it)
		{
			EVENT *event = *it;
			if(!event->IsInternal() && (event->GetFrontEndNumber() == front_end_num))
			{
				event->Catch();
				listened_events.push_back(event);
			}
		}
	}
	
	for(typename ListenedEvents::iterator it = listened_events.begin(); it != listened_events.end(); ++it)
	{
		unisim::util::debug::Event<ADDRESS> *event = *it;
		scanner.Append(*it);
		event->Release();
	}
}

template <typename CONFIG>
void Debugger<CONFIG>::ScanListenedEvents(unsigned int front_end_num, unisim::service::interfaces::DebugEventScanner<ADDRESS>& scanner) const
{
	// beware of reentrancy
	this->template ScanEvents<FetchInsnEventSet, FetchInsnEvent, unisim::service::interfaces::DebugEventScanner<ADDRESS> >(front_end_num, fetch_insn_event_set, scanner);
	this->template ScanEvents<CommitInsnEventSet, CommitInsnEvent, unisim::service::interfaces::DebugEventScanner<ADDRESS> >(front_end_num, commit_insn_event_set, scanner);
	this->template ScanEvents<NextInsnEventSet, NextInsnEvent, unisim::service::interfaces::DebugEventScanner<ADDRESS> >(front_end_num, next_insn_event_set, scanner);
	this->template ScanEvents<FetchStmtEventSet, FetchStmtEvent, unisim::service::interfaces::DebugEventScanner<ADDRESS> >(front_end_num, fetch_stmt_event_set, scanner);
	this->template ScanEvents<NextStmtEventSet, NextStmtEvent, unisim::service::interfaces::DebugEventScanner<ADDRESS> >(front_end_num, next_stmt_event_set, scanner);
	this->template ScanEvents<FinishEventSet, FinishEvent, unisim::service::interfaces::DebugEventScanner<ADDRESS> >(front_end_num, finish_event_set, scanner);
	this->template ScanEvents<TrapEventSet, TrapEvent, unisim::service::interfaces::DebugEventScanner<ADDRESS> >(front_end_num, trap_event_set, scanner);

	struct Filter
	{
		Filter()
			: reorder_buffer()
		{
		}
		
		void Append(unisim::util::debug::Breakpoint<ADDRESS> *brkp)
		{
			if(brkp->GetId() >= 0)
			{
				unsigned int id = brkp->GetId();
				if(id >= reorder_buffer.size())
				{
					reorder_buffer.resize(id + 1);
				}
				brkp->Catch();
				reorder_buffer[id] = brkp;
			}
		}
		
		void Append(unisim::util::debug::SourceCodeBreakpoint<ADDRESS> *src_code_brkp)
		{
			if(src_code_brkp->GetId() >= 0)
			{
				unsigned int id = src_code_brkp->GetId();
				if(id >= reorder_buffer.size())
				{
					reorder_buffer.resize(id + 1);
				}
				src_code_brkp->Catch();
				reorder_buffer[id] = src_code_brkp;
			}
		}
		
		void Append(unisim::util::debug::Watchpoint<ADDRESS> *wp)
		{
			if(wp->GetId() >= 0)
			{
				unsigned int id = wp->GetId();
				if(id >= reorder_buffer.size())
				{
					reorder_buffer.resize(id + 1);
				}
				wp->Catch();
				reorder_buffer[id] = wp;
			}
		}
		
		void Append(unisim::util::debug::SubProgramBreakpoint<ADDRESS> *subprogram_brkp)
		{
			if(subprogram_brkp->GetId() >= 0)
			{
				unsigned int id = subprogram_brkp->GetId();
				if(id >= reorder_buffer.size())
				{
					reorder_buffer.resize(id + 1);
				}
				subprogram_brkp->Catch();
				reorder_buffer[id] = subprogram_brkp;
			}
		}
		
		void Scan(unisim::service::interfaces::DebugEventScanner<ADDRESS>& scanner)
		{
			for(typename ReorderBuffer::iterator it = reorder_buffer.begin(); it != reorder_buffer.end(); ++it)
			{
				unisim::util::debug::Event<ADDRESS> *event = *it;
				if(event)
				{
					scanner.Append(event);
					event->Release();
				}
			}
			reorder_buffer.clear();
		}
		
	private:
		typedef std::vector<unisim::util::debug::Event<ADDRESS> *> ReorderBuffer;
		ReorderBuffer reorder_buffer;
	};
	
	Filter filter;
	
	breakpoint_registry.ScanBreakpoints(front_end_num, filter);
	
	watchpoint_registry.ScanWatchpoints(front_end_num, filter);
	
	for(unsigned int front_end_num = 0; front_end_num < MAX_FRONT_ENDS; ++front_end_num)
	{
		for(typename SourceCodeBreakpointRegistry::const_iterator it = source_code_breakpoint_registry[front_end_num].begin(); it != source_code_breakpoint_registry[front_end_num].end(); ++it)
		{
			unisim::util::debug::SourceCodeBreakpoint<ADDRESS> *src_code_brkp = *it;
			filter.Append(src_code_brkp);
		}
		
		for(typename SubProgramBreakpointRegistry::const_iterator it = subprogram_breakpoint_registry[front_end_num].begin(); it != subprogram_breakpoint_registry[front_end_num].end(); ++it)
		{
			unisim::util::debug::SubProgramBreakpoint<ADDRESS> *subprogram_brkp = *it;
			filter.Append(subprogram_brkp);
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
	Breakpoint *brkp = breakpoint_registry.SetBreakpoint(addr, prc_num, front_end_num);
	if(brkp && (brkp->GetId() < 0))
	{
		brkp->SetId(AllocateId(front_end_num));
	}
	brkp->AddEventListener(front_end_gate[front_end_num]->debug_event_listener_import);
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
	Watchpoint *wp = watchpoint_registry.SetWatchpoint(mat, mt, addr, size, overlook, prc_num, front_end_num);
	if(wp && (wp->GetId() < 0))
	{
		wp->SetId(AllocateId(front_end_num));
	}
	wp->AddEventListener(front_end_gate[front_end_num]->debug_event_listener_import);
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

template <typename CONFIG>
std::string Debugger<CONFIG>::Disasm(unsigned int front_end_num, unsigned int prc_num, ADDRESS addr, ADDRESS& next_addr)
{
	return ((front_end_num < MAX_FRONT_ENDS) && (prc_num < NUM_PROCESSORS)) ? prc_gate[prc_num]->Disasm(addr, next_addr) : std::string();
}

// unisim::service::interfaces::Memory<ADDRESS> (tagged)
template <typename CONFIG>
void Debugger<CONFIG>::ResetMemory(unsigned int front_end_num)
{
	sel_prc_gate[front_end_num]->ResetMemory();
}

template <typename CONFIG>
void Debugger<CONFIG>::ResetMemory(unsigned int front_end_num, unsigned int prc_num)
{
	if((front_end_num < MAX_FRONT_ENDS) && (prc_num < NUM_PROCESSORS)) prc_gate[prc_num]->ResetMemory();
}

template <typename CONFIG>
bool Debugger<CONFIG>::ReadMemory(unsigned int front_end_num, ADDRESS addr, void *buffer, uint32_t size)
{
	return sel_prc_gate[front_end_num]->ReadMemory(addr, buffer, size);
}

template <typename CONFIG>
bool Debugger<CONFIG>::ReadMemory(unsigned int front_end_num, unsigned int prc_num, ADDRESS addr, void *buffer, uint32_t size)
{
	return (front_end_num < MAX_FRONT_ENDS) && (prc_num < NUM_PROCESSORS) && prc_gate[prc_num]->ReadMemory(addr, buffer, size);
}

template <typename CONFIG>
bool Debugger<CONFIG>::WriteMemory(unsigned int front_end_num, ADDRESS addr, const void *buffer, uint32_t size)
{
	return sel_prc_gate[front_end_num]->WriteMemory(addr, buffer, size);
}

template <typename CONFIG>
bool Debugger<CONFIG>::WriteMemory(unsigned int front_end_num, unsigned int prc_num, ADDRESS addr, const void *buffer, uint32_t size)
{
	return (front_end_num < MAX_FRONT_ENDS) && (prc_num < NUM_PROCESSORS) &&  prc_gate[prc_num]->WriteMemory(addr, buffer, size);
}

// unisim::service::interfaces::Registers (tagged)
template <typename CONFIG>
unisim::service::interfaces::Register *Debugger<CONFIG>::GetRegister(unsigned int front_end_num, const char *name)
{
	return dw_mach_state[front_end_num].GetRegister(sel_cpu[front_end_num], name);
}

template <typename CONFIG>
unisim::service::interfaces::Register *Debugger<CONFIG>::GetRegister(unsigned int front_end_num, unsigned int prc_num, const char *name)
{
	return ((front_end_num < MAX_FRONT_ENDS) && (prc_num < NUM_PROCESSORS)) ? dw_mach_state[front_end_num].GetRegister(prc_num, name) : 0;
}

template <typename CONFIG>
void Debugger<CONFIG>::ScanRegisters(unsigned int front_end_num, unisim::service::interfaces::RegisterScanner& scanner)
{
	dw_mach_state[front_end_num].ScanRegisters(sel_cpu[front_end_num], scanner);
}

template <typename CONFIG>
void Debugger<CONFIG>::ScanRegisters(unsigned int front_end_num, unsigned int prc_num, unisim::service::interfaces::RegisterScanner& scanner)
{
	if((front_end_num < MAX_FRONT_ENDS) && (prc_num < NUM_PROCESSORS)) dw_mach_state[front_end_num].ScanRegisters(prc_num, scanner);
}

// unisim::service::interfaces::SymbolTableLookup<ADDRESS> (tagged)
template <typename CONFIG>
void Debugger<CONFIG>::GetSymbols(unsigned int front_end_num, typename std::list<const unisim::util::debug::Symbol<ADDRESS> *>& lst, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const
{
	struct Visitor
	{
		typename std::list<const unisim::util::debug::Symbol<ADDRESS> *>& lst;
		typename unisim::util::debug::Symbol<ADDRESS>::Type type;
		Visitor(typename std::list<const unisim::util::debug::Symbol<ADDRESS> *>& _lst, typename unisim::util::debug::Symbol<ADDRESS>::Type _type) : lst(_lst), type(_type) {}
		bool Visit(unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader) { elf32_loader->GetSymbols(lst, type); return false; }
		bool Visit(unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader) { elf64_loader->GetSymbols(lst, type); return false; }
		bool Visit(unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader) { coff_loader->GetSymbols(lst, type); return false; }
	};
	
	Visitor visitor(lst, type);
	ScanEnabledLoaders(front_end_num, visitor);
}

template <typename CONFIG>
const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *Debugger<CONFIG>::FindSymbol(unsigned int front_end_num, const char *name, ADDRESS addr, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const
{
	struct Visitor
	{
		const char *name;
		ADDRESS addr;
		typename unisim::util::debug::Symbol<ADDRESS>::Type type;
		Visitor(const char *_name, ADDRESS _addr, typename unisim::util::debug::Symbol<ADDRESS>::Type _type) : name(_name), addr(_addr), type(_type) {}
		const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *Visit(unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader) { return elf32_loader->FindSymbol(name, addr, type); }
		const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *Visit(unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader) { return elf64_loader->FindSymbol(name, addr, type); }
		const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *Visit(unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader) { return coff_loader->FindSymbol(name, addr, type); }
	};
	
	Visitor visitor(name, addr, type);
	return this->template ScanEnabledLoaders<Visitor, const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *>(front_end_num, visitor);
}

template <typename CONFIG>
const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *Debugger<CONFIG>::FindSymbolByAddr(unsigned int front_end_num, ADDRESS addr) const
{
	struct Visitor
	{
		ADDRESS addr;
		Visitor(ADDRESS _addr) : addr(_addr) {}
		const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *Visit(unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader) { return elf32_loader->FindSymbolByAddr(addr); }
		const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *Visit(unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader) { return elf64_loader->FindSymbolByAddr(addr); }
		const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *Visit(unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader) { return coff_loader->FindSymbolByAddr(addr); }
	};
	
	Visitor visitor(addr);
	return this->template ScanEnabledLoaders<Visitor, const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *>(front_end_num, visitor);
}

template <typename CONFIG>
const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *Debugger<CONFIG>::FindSymbolByName(unsigned int front_end_num, const char *name) const
{
	struct Visitor
	{
		const char *name;
		Visitor(const char *_name) : name(_name) {}
		const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *Visit(unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader) { return elf32_loader->FindSymbolByName(name); }
		const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *Visit(unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader) { return elf64_loader->FindSymbolByName(name); }
		const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *Visit(unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader) { return coff_loader->FindSymbolByName(name); }
	};
	
	Visitor visitor(name);
	return this->template ScanEnabledLoaders<Visitor, const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *>(front_end_num, visitor);
}

template <typename CONFIG>
const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *Debugger<CONFIG>::FindSymbolByName(unsigned int front_end_num, const char *name, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const
{
	struct Visitor
	{
		const char *name;
		typename unisim::util::debug::Symbol<ADDRESS>::Type type;
		Visitor(const char *_name, typename unisim::util::debug::Symbol<ADDRESS>::Type _type) : name(_name), type(_type) {}
		const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *Visit(unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader) { return elf32_loader->FindSymbolByName(name, type); }
		const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *Visit(unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader) { return elf64_loader->FindSymbolByName(name, type); }
		const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *Visit(unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader) { return coff_loader->FindSymbolByName(name, type); }
	};
	
	Visitor visitor(name, type);
	return this->template ScanEnabledLoaders<Visitor, const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *>(front_end_num, visitor);
}

template <typename CONFIG>
const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *Debugger<CONFIG>::FindSymbolByAddr(unsigned int front_end_num, ADDRESS addr, typename unisim::util::debug::Symbol<ADDRESS>::Type type) const
{
	struct Visitor
	{
		ADDRESS addr;
		typename unisim::util::debug::Symbol<ADDRESS>::Type type;
		Visitor(ADDRESS _addr, typename unisim::util::debug::Symbol<ADDRESS>::Type _type) : addr(_addr), type(_type) {}
		const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *Visit(unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader) { return elf32_loader->FindSymbolByAddr(addr, type); }
		const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *Visit(unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader) { return elf64_loader->FindSymbolByAddr(addr, type); }
		const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *Visit(unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader) { return coff_loader->FindSymbolByAddr(addr, type); }
	};
	
	Visitor visitor(addr, type);
	return this->template ScanEnabledLoaders<Visitor, const typename unisim::util::debug::Symbol<typename CONFIG::ADDRESS> *>(front_end_num, visitor);
}


// unisim::service::interfaces::StatementLookup<ADDRESS> (tagged)
template <typename CONFIG>
void Debugger<CONFIG>::ScanStatements(unsigned int front_end_num, unisim::service::interfaces::StatementScanner<ADDRESS>& scanner, const char *filename) const
{
	struct Visitor
	{
		unisim::service::interfaces::StatementScanner<ADDRESS>& scanner;
		Visitor(unisim::service::interfaces::StatementScanner<ADDRESS>& _scanner) : scanner(_scanner) {}
		bool Visit(unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader) { elf32_loader->ScanStatements(scanner); return false; }
		bool Visit(unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader) { elf64_loader->ScanStatements(scanner); return false; }
		bool Visit(unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader) { return false; }
	};
	
	Visitor visitor(scanner);
	ScanAndFilterLoadersByFilename(front_end_num, filename, visitor);
}

template <typename CONFIG>
const unisim::util::debug::Statement<typename CONFIG::ADDRESS> *Debugger<CONFIG>::FindStatement(unsigned int front_end_num, ADDRESS addr, const char *filename, typename unisim::service::interfaces::StatementLookup<ADDRESS>::FindStatementOption opt) const
{
	struct Visitor : StatementFilter
	{
		Visitor(ADDRESS _addr, typename unisim::service::interfaces::StatementLookup<ADDRESS>::FindStatementOption _opt) : StatementFilter(_addr, _opt) {}
		const typename unisim::util::debug::Statement<ADDRESS> *Visit(unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader) { return this->Filter(elf32_loader->FindStatement(this->addr, this->opt)); }
		const typename unisim::util::debug::Statement<ADDRESS> *Visit(unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader) { return this->Filter(elf64_loader->FindStatement(this->addr, this->opt)); }
		const typename unisim::util::debug::Statement<ADDRESS> *Visit(unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader) { return 0; }
	};
	
	Visitor visitor(addr, opt);
	const unisim::util::debug::Statement<typename CONFIG::ADDRESS> *stmt = this->template ScanAndFilterLoadersByFilename<Visitor, const typename unisim::util::debug::Statement<ADDRESS> *>(front_end_num, filename, visitor);
	return stmt ? stmt : visitor.ret_stmt;
}

template <typename CONFIG>
const unisim::util::debug::Statement<typename CONFIG::ADDRESS> *Debugger<CONFIG>::FindStatements(unsigned int front_end_num, std::vector<const unisim::util::debug::Statement<ADDRESS> *> &stmts, ADDRESS addr, const char *filename, typename unisim::service::interfaces::StatementLookup<ADDRESS>::FindStatementOption opt) const
{
	struct Visitor : StatementFilter
	{
		std::vector<const unisim::util::debug::Statement<ADDRESS> *> &stmts;
		Visitor(std::vector<const unisim::util::debug::Statement<ADDRESS> *> &_stmts, ADDRESS _addr, typename unisim::service::interfaces::StatementLookup<ADDRESS>::FindStatementOption _opt) : StatementFilter(_addr, _opt), stmts(_stmts) {}
		const typename unisim::util::debug::Statement<ADDRESS> *Visit(unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader) { return this->Filter(elf32_loader->FindStatements(stmts, this->addr, this->opt)); }
		const typename unisim::util::debug::Statement<ADDRESS> *Visit(unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader) { return this->Filter(elf64_loader->FindStatements(stmts, this->addr, this->opt)); }
		const typename unisim::util::debug::Statement<ADDRESS> *Visit(unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader) { return 0; }
	};
	
	Visitor visitor(stmts, addr, opt);
	const unisim::util::debug::Statement<typename CONFIG::ADDRESS> *stmt = this->template ScanAndFilterLoadersByFilename<Visitor, const typename unisim::util::debug::Statement<ADDRESS> *>(front_end_num, filename, visitor);
	return stmt ? stmt : visitor.ret_stmt;
}

template <typename CONFIG>
const unisim::util::debug::Statement<typename CONFIG::ADDRESS> *Debugger<CONFIG>::FindStatement(unsigned int front_end_num, const unisim::util::debug::SourceCodeLocation& source_code_location, const char *filename) const
{
	struct Visitor
	{
		const unisim::util::debug::SourceCodeLocation& source_code_location;
		Visitor(const unisim::util::debug::SourceCodeLocation& _source_code_location) : source_code_location(_source_code_location) {}
		const typename unisim::util::debug::Statement<ADDRESS> *Visit(unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader) { return elf32_loader->FindStatement(source_code_location); }
		const typename unisim::util::debug::Statement<ADDRESS> *Visit(unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader) { return elf64_loader->FindStatement(source_code_location); }
		const typename unisim::util::debug::Statement<ADDRESS> *Visit(unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader) { return 0; }
	};
	
	Visitor visitor(source_code_location);
	return this->template ScanAndFilterLoadersByFilename<Visitor, const typename unisim::util::debug::Statement<ADDRESS> *>(front_end_num, filename, visitor);
}

template <typename CONFIG>
const unisim::util::debug::Statement<typename CONFIG::ADDRESS> *Debugger<CONFIG>::FindStatements(unsigned int front_end_num, std::vector<const unisim::util::debug::Statement<ADDRESS> *> &stmts, const unisim::util::debug::SourceCodeLocation& source_code_location, const char *filename) const
{
	struct Visitor
	{
		std::vector<const unisim::util::debug::Statement<ADDRESS> *> &stmts;
		const unisim::util::debug::SourceCodeLocation& source_code_location;
		const typename unisim::util::debug::Statement<ADDRESS> *ret;
		Visitor(std::vector<const unisim::util::debug::Statement<ADDRESS> *> &_stmts, const unisim::util::debug::SourceCodeLocation& _source_code_location) : stmts(_stmts), source_code_location(_source_code_location), ret(0) {}
		bool Visit(unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader) { const typename unisim::util::debug::Statement<ADDRESS> *stmt = elf32_loader->FindStatements(stmts, source_code_location); if(!ret) { ret = stmt; } return false; }
		bool Visit(unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader) { const typename unisim::util::debug::Statement<ADDRESS> *stmt = elf64_loader->FindStatements(stmts, source_code_location); if(!ret) { ret = stmt; } return false; }
		bool Visit(unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader) { return false; }
	};
	
	Visitor visitor(stmts, source_code_location);
	this->ScanAndFilterLoadersByFilename(front_end_num, filename, visitor);
	return visitor.ret;
}

// unisim::service::interfaces::StackFrame<ADDRESS> (tagged)
template <typename CONFIG>
bool Debugger<CONFIG>::SelectStackFrame(unsigned int front_end_num, unsigned int frame_num)
{
	return dw_mach_state[front_end_num].SelectStackFrame(sel_cpu[front_end_num], frame_num);
}

template <typename CONFIG>
bool Debugger<CONFIG>::SelectStackFrame(unsigned int front_end_num, unsigned int prc_num, unsigned int frame_num)
{
	return dw_mach_state[front_end_num].SelectStackFrame(prc_num, frame_num);
}

template <typename CONFIG>
unsigned int Debugger<CONFIG>::GetSelectedStackFrameNumber(unsigned int front_end_num) const
{
	return dw_mach_state[front_end_num].GetSelectedFrame(sel_cpu[front_end_num]);
}

template <typename CONFIG>
unsigned int Debugger<CONFIG>::GetSelectedStackFrameNumber(unsigned int front_end_num, unsigned int prc_num) const
{
	return dw_mach_state[front_end_num].GetSelectedFrame(prc_num);
}

template <typename CONFIG>
std::vector<typename CONFIG::ADDRESS> *Debugger<CONFIG>::GetBackTrace(unsigned int front_end_num) const
{
	return dw_mach_state[front_end_num].GetBackTrace(sel_cpu[front_end_num]);
}

template <typename CONFIG>
std::vector<typename CONFIG::ADDRESS> *Debugger<CONFIG>::GetBackTrace(unsigned int front_end_num, unsigned int prc_num) const
{
	return dw_mach_state[front_end_num].GetBackTrace(prc_num);
}

template <typename CONFIG>
bool Debugger<CONFIG>::GetReturnAddress(unsigned int front_end_num, ADDRESS& ret_addr) const
{
	return dw_mach_state[front_end_num].GetReturnAddress(sel_cpu[front_end_num], ret_addr);
}

template <typename CONFIG>
bool Debugger<CONFIG>::GetReturnAddress(unsigned int front_end_num, unsigned int prc_num, ADDRESS& ret_addr) const
{
	return dw_mach_state[front_end_num].GetReturnAddress(prc_num, ret_addr);
}

template <typename CONFIG>
bool Debugger<CONFIG>::UnwindStack(unsigned int front_end_num, unsigned int frame_num)
{
	return dw_mach_state[front_end_num].UnwindStack(sel_cpu[front_end_num], frame_num);
}

template <typename CONFIG>
bool Debugger<CONFIG>::UnwindStack(unsigned int front_end_num, unsigned int prc_num, unsigned int frame_num)
{
	return dw_mach_state[front_end_num].UnwindStack(prc_num, frame_num);
}

template <typename CONFIG>
void Debugger<CONFIG>::ScanStackFrameInfos(unsigned int front_end_num, unisim::service::interfaces::StackFrameInfoScanner<ADDRESS>& scanner, unsigned int max_stack_frames) const
{
	dw_mach_state[front_end_num].ScanStackFrameInfos(sel_cpu[front_end_num], scanner, max_stack_frames);
}

template <typename CONFIG>
void Debugger<CONFIG>::ScanStackFrameInfos(unsigned int front_end_num, unsigned int prc_num, unisim::service::interfaces::StackFrameInfoScanner<ADDRESS>& scanner, unsigned int max_stack_frames) const
{
	dw_mach_state[front_end_num].ScanStackFrameInfos(prc_num, scanner, max_stack_frames);
}

template <typename CONFIG>
unsigned int Debugger<CONFIG>::GetStackFrameInfos(unsigned int front_end_num, unisim::service::interfaces::StackFrameInfo<ADDRESS> *stack_frame_infos, unsigned int max_stack_frames) const
{
	return dw_mach_state[front_end_num].GetStackFrameInfos(sel_cpu[front_end_num], stack_frame_infos, max_stack_frames);
}

template <typename CONFIG>
unsigned int Debugger<CONFIG>::GetStackFrameInfos(unsigned int front_end_num, unsigned int prc_num, unisim::service::interfaces::StackFrameInfo<ADDRESS> *stack_frame_infos, unsigned int max_stack_frames) const
{
	return dw_mach_state[front_end_num].GetStackFrameInfos(prc_num, stack_frame_infos, max_stack_frames);
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
	for(size = 0; size < sizeof(magic); ++size)
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
			for(i = 0; i < MAX_FRONT_ENDS; ++i)
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
						for(prc_num = 0; prc_num < NUM_PROCESSORS; ++prc_num)
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
						for(i = 0; i < MAX_FRONT_ENDS; ++i)
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
						for(prc_num = 0; prc_num < NUM_PROCESSORS; ++prc_num)
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
						for(i = 0; i < MAX_FRONT_ENDS; ++i)
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
	for(i = 0; i < num_elf32_loaders; ++i)
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
	for(i = 0; i < num_elf64_loaders; ++i)
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
	for(i = 0; i < num_coff_loaders; ++i)
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
	for(i = 0; i < num_elf32_loaders; ++i)
	{
		typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
		const unisim::util::blob::Blob<ADDRESS> *blob = elf32_loader->GetBlob();
		if(blob->GetCapability() & unisim::util::blob::CAP_FILENAME)
		{
			lst.push_back(std::string(blob->GetFilename()));
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; ++i)
	{
		typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
		const unisim::util::blob::Blob<ADDRESS> *blob = elf64_loader->GetBlob();
		if(blob->GetCapability() & unisim::util::blob::CAP_FILENAME)
		{
			lst.push_back(std::string(blob->GetFilename()));
		}
	}

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; ++i)
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
	for(i = 0; i < num_elf32_loaders; ++i)
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
	for(i = 0; i < num_elf64_loaders; ++i)
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
	for(i = 0; i < num_coff_loaders; ++i)
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
	return dw_mach_state[front_end_num].FindDataObject(sel_cpu[front_end_num], data_object_name);
}

template <typename CONFIG>
unisim::util::debug::DataObjectRef<typename CONFIG::ADDRESS> Debugger<CONFIG>::FindDataObject(unsigned int front_end_num, unsigned int prc_num, const char *data_object_name) const
{
	return dw_mach_state[front_end_num].FindDataObject(prc_num, data_object_name);
}

template <typename CONFIG>
unisim::util::debug::DataObjectRef<typename CONFIG::ADDRESS> Debugger<CONFIG>::GetReturnValue(unsigned int front_end_num) const
{
	return dw_mach_state[front_end_num].GetReturnValue(sel_cpu[front_end_num]);
}

template <typename CONFIG>
unisim::util::debug::DataObjectRef<typename CONFIG::ADDRESS> Debugger<CONFIG>::GetReturnValue(unsigned int front_end_num, unsigned int prc_num) const
{
	return dw_mach_state[front_end_num].GetReturnValue(prc_num);
}

template <typename CONFIG>
unisim::util::debug::DataObjectRef<typename CONFIG::ADDRESS> Debugger<CONFIG>::GetSubProgramParameter(unsigned int front_end_num, unsigned int index) const
{
	return dw_mach_state[front_end_num].GetSubProgramParameter(sel_cpu[front_end_num], index);
}

template <typename CONFIG>
unisim::util::debug::DataObjectRef<typename CONFIG::ADDRESS> Debugger<CONFIG>::GetSubProgramParameter(unsigned int front_end_num, unsigned int prc_num, unsigned int index) const
{
	return dw_mach_state[front_end_num].GetSubProgramParameter(prc_num, index);
}

template <typename CONFIG>
void Debugger<CONFIG>::EnumerateDataObjectNames(unsigned int front_end_num, std::set<std::string>& name_set, typename unisim::service::interfaces::DataObjectLookup<ADDRESS>::Scope scope) const
{
	dw_mach_state[front_end_num].EnumerateDataObjectNames(sel_cpu[front_end_num], name_set, scope);
}

template <typename CONFIG>
void Debugger<CONFIG>::EnumerateDataObjectNames(unsigned int front_end_num, unsigned int prc_num, std::set<std::string>& name_set, typename unisim::service::interfaces::DataObjectLookup<ADDRESS>::Scope scope) const
{
	dw_mach_state[front_end_num].EnumerateDataObjectNames(prc_num, name_set, scope);
}


// unisim::service::interfaces::SubProgramLookup<ADDRESS> (tagged)
template <typename CONFIG>
const unisim::util::debug::SubProgram<typename CONFIG::ADDRESS> *Debugger<CONFIG>::FindSubProgram(unsigned int front_end_num, const char *subprogram_name, const char *filename, const char *compilation_unit_name) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; ++i)
	{
		if(enable_elf32_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			const unisim::util::debug::SubProgram<ADDRESS> *subprogram = elf32_loader->FindSubProgram(subprogram_name, filename, compilation_unit_name);
			if(subprogram) return subprogram;
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; ++i)
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
	for(i = 0; i < num_elf32_loaders; ++i)
	{
		if(enable_elf32_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			const unisim::util::debug::SubProgram<ADDRESS> *subprogram = elf32_loader->FindSubProgram(pc, filename);
			if(subprogram) return subprogram;
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; ++i)
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

// unisim::service::interfaces::Stubbing<ADDRESS> (tagged)

template <typename CONFIG>
void Debugger<CONFIG>::ScanStubs(unsigned int front_end_num, unisim::service::interfaces::StubScanner<ADDRESS>& scanner) const
{
	ScanStubs(front_end_num, sel_cpu[front_end_num], scanner);
}

template <typename CONFIG>
void Debugger<CONFIG>::ScanStubs(unsigned int front_end_num, unsigned int prc_num, unisim::service::interfaces::StubScanner<ADDRESS>& scanner) const
{
	if(!stub_registry[front_end_num][prc_num].empty())
	{
		// beware of reentrancy
		typename StubRegistry::size_type i = 0, n = stub_registry[front_end_num][prc_num].size();
		unisim::util::debug::Stub<ADDRESS> *stubs[n];
		for(typename StubRegistry::const_iterator it = stub_registry[front_end_num][prc_num].begin(); it != stub_registry[front_end_num][prc_num].end(); ++i, ++it)
		{
			unisim::util::debug::Stub<ADDRESS> *stub = (*it).first;
			stubs[i] = stub;
		}
		for(i = 0; i < n; ++i)
		{
			unisim::util::debug::Stub<ADDRESS> *stub = stubs[i];
			if(stub_registry[front_end_num][prc_num].find(stub) != stub_registry[front_end_num][prc_num].end()) scanner.Append(stub);
		}
	}
}

template <typename CONFIG>
bool Debugger<CONFIG>::SetStub(unsigned int front_end_num, unisim::util::debug::Stub<ADDRESS> *stub)
{
	return SetStub(front_end_num, sel_cpu[front_end_num], stub);
}

template <typename CONFIG>
bool Debugger<CONFIG>::SetStub(unsigned int front_end_num, unsigned int prc_num, unisim::util::debug::Stub<ADDRESS> *stub)
{
	StubHandler *stub_handler = new StubHandler(*this, front_end_num, prc_num, stub);
	std::pair<typename StubRegistry::iterator, bool> r = stub_registry[front_end_num][prc_num].insert(typename StubRegistry::value_type(stub, stub_handler));
	if(!r.second)
	{
		// stub already set
		delete stub_handler;
		return true;
	}
	if(!stub_handler->Set())
	{
		stub_registry[front_end_num][prc_num].erase(r.first);
		delete stub_handler;
		return false;
	}
	UpdateReportingRequirements(prc_num);
	return true;
}

template <typename CONFIG>
bool Debugger<CONFIG>::RemoveStub(unsigned int front_end_num, unisim::util::debug::Stub<ADDRESS> *stub)
{
	return RemoveStub(front_end_num, sel_cpu[front_end_num], stub);
}

template <typename CONFIG>
bool Debugger<CONFIG>::RemoveStub(unsigned int front_end_num, unsigned int prc_num, unisim::util::debug::Stub<ADDRESS> *stub)
{
	typename StubRegistry::iterator it = stub_registry[front_end_num][prc_num].find(stub);
	if(it == stub_registry[front_end_num][prc_num].end()) return false;
	StubHandler *stub_handler = (*it).second;
	if(!stub_handler->Remove()) return false;
	delete stub_handler;
	stub_registry[front_end_num][prc_num].erase(it);
	UpdateReportingRequirements(prc_num);
	return true;
}

// unisim::service::interfaces::Hooking<ADDRESS> (tagged)
template <typename CONFIG>
void Debugger<CONFIG>::ScanHooks(unsigned int front_end_num, unisim::service::interfaces::HookScanner<ADDRESS>& scanner) const
{
	ScanHooks(front_end_num, sel_cpu[front_end_num], scanner);
}

template <typename CONFIG>
void Debugger<CONFIG>::ScanHooks(unsigned int front_end_num, unsigned int prc_num, unisim::service::interfaces::HookScanner<ADDRESS>& scanner) const
{
	if(!hook_registry[front_end_num][prc_num].empty())
	{
		// beware of reentrancy
		typename HookRegistry::size_type i = 0, n = hook_registry[front_end_num][prc_num].size();
		unisim::util::debug::Hook<ADDRESS> *hooks[n];
		for(typename HookRegistry::const_iterator it = hook_registry[front_end_num][prc_num].begin(); it != hook_registry[front_end_num][prc_num].end(); ++i, ++it)
		{
			unisim::util::debug::Hook<ADDRESS> *hook = (*it).first;
			hooks[i] = hook;
		}
		for(i = 0; i < n; ++i)
		{
			unisim::util::debug::Hook<ADDRESS> *hook = hooks[i];
			if(hook_registry[front_end_num][prc_num].find(hook) != hook_registry[front_end_num][prc_num].end()) scanner.Append(hook);
		}
	}
}

template <typename CONFIG>
bool Debugger<CONFIG>::SetHook(unsigned int front_end_num, unisim::util::debug::Hook<ADDRESS> *hook)
{
	return SetHook(front_end_num, sel_cpu[front_end_num], hook);
}

template <typename CONFIG>
bool Debugger<CONFIG>::SetHook(unsigned int front_end_num, unsigned int prc_num, unisim::util::debug::Hook<ADDRESS> *hook)
{
	HookHandler *hook_handler = new HookHandler(*this, front_end_num, prc_num, hook);
	std::pair<typename HookRegistry::iterator, bool> r = hook_registry[front_end_num][prc_num].insert(typename HookRegistry::value_type(hook, hook_handler));
	if(!r.second)
	{
		// hook already set
		delete hook_handler;
		return true;
	}
	if(!hook_handler->Set())
	{
		hook_registry[front_end_num][prc_num].erase(r.first);
		delete hook_handler;
		return false;
	}
	UpdateReportingRequirements(prc_num);
	return true;
}

template <typename CONFIG>
bool Debugger<CONFIG>::RemoveHook(unsigned int front_end_num, unisim::util::debug::Hook<ADDRESS> *hook)
{
	return RemoveHook(front_end_num, sel_cpu[front_end_num], hook);
}

template <typename CONFIG>
bool Debugger<CONFIG>::RemoveHook(unsigned int front_end_num, unsigned int prc_num, unisim::util::debug::Hook<ADDRESS> *hook)
{
	typename HookRegistry::iterator it = hook_registry[front_end_num][prc_num].find(hook);
	if(it == hook_registry[front_end_num][prc_num].end()) return false;
	HookHandler *hook_handler = (*it).second;
	if(!hook_handler->Remove()) return false;
	delete hook_handler;
	hook_registry[front_end_num][prc_num].erase(it);
	UpdateReportingRequirements(prc_num);
	return true;
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

// unisim::service::interfaces::DebugProcessors<ADDRESS, TIME_TYPE> (tagged)

template <typename CONFIG>
unsigned int Debugger<CONFIG>::DebugGetProcessorCount(unsigned int front_end_num) const
{
	return NUM_PROCESSORS;
}

template <typename CONFIG>
unisim::service::interfaces::DebugProcessor<typename CONFIG::ADDRESS, typename CONFIG::TIME_TYPE> *Debugger<CONFIG>::DebugGetProcessor(unsigned int front_end_num, unsigned int prc_num) const
{
	return processors[front_end_num][prc_num];
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
int Debugger<CONFIG>::AllocateId(unsigned int front_end_num) const
{
	return next_id[front_end_num]++;
}

template <typename CONFIG>
bool Debugger<CONFIG>::ReadProgramCounterRegister(unsigned int front_end_num, unsigned int prc_num, ADDRESS& pc) const
{
	if(front_end_num >= MAX_FRONT_ENDS) return false;
	return dw_mach_state[front_end_num].ReadProgramCounterRegister(prc_num, pc);
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
	for(i = 0; i < num_elf32_loaders; ++i)
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
	for(i = 0; i < num_elf64_loaders; ++i)
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
	typedef typename unisim::util::debug::Stub<ADDRESS>::Parameter Parameter;
	typedef typename unisim::util::debug::Stub<ADDRESS>::ReturnValue ReturnValue;
	
	struct Parameters : unisim::util::debug::Stub<ADDRESS>::Parameters
	{
		Parameters(Debugger<CONFIG>& _debugger, unsigned int _front_end_num, unsigned int _prc_num, unsigned int _arity)
			: debugger(_debugger)
			, front_end_num(_front_end_num)
			, prc_num(_prc_num)
			, arity(_arity)
			, params(_arity)
			, return_value()
		{
		}
		
		virtual unsigned int Length() const
		{
			return arity;
		}
		
		virtual Parameter& GetParameter(unsigned int index) const
		{
			if(index >= arity) return unisim::util::debug::DataObjectRef<ADDRESS>::Undefined;
			
			Parameter& param = params[index];
			if(!param.IsUndefined()) return param;
			
			return params[index] = debugger.dw_mach_state[front_end_num].GetSubProgramParameter(prc_num, index);
		}
		
		virtual ReturnValue& GetReturnValue() const
		{
			return return_value.IsUndefined() ? (return_value = unisim::util::debug::DataObjectRef<ADDRESS>(debugger.dw_mach_state[front_end_num].FindDataObject(prc_num, "$return_value"))) : return_value;
		}
		
		Debugger<CONFIG>& debugger;
		unsigned int front_end_num;
		unsigned int prc_num;
		unsigned int arity;
		mutable std::vector<Parameter> params;
		mutable ReturnValue return_value;
	};
	
	const unisim::util::debug::SubProgram<ADDRESS> *subprogram = stub->GetSubProgram();
	
	Parameters params(*this, front_end_num, prc_num, subprogram->GetArity());
	
	unsigned int save_prc_num = GetSelectedProcessor(front_end_num);
	SelectProcessor(front_end_num, prc_num);
	stub->Run(params);
	UnwindStack(front_end_num, prc_num, 1);
	SelectProcessor(front_end_num, save_prc_num);
}

template <typename CONFIG>
void Debugger<CONFIG>::CallHook(unsigned int front_end_num, unsigned int prc_num, unisim::util::debug::Hook<ADDRESS> *hook)
{
	unsigned int save_prc_num = GetSelectedProcessor(front_end_num);
	SelectProcessor(front_end_num, prc_num);
	hook->Run();
	SelectProcessor(front_end_num, save_prc_num);
}

template <typename CONFIG>
template <typename VISITOR, typename T>
T Debugger<CONFIG>::ScanEnabledLoaders(unsigned int front_end_num, VISITOR& visitor) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; ++i)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf32_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
			T ret = visitor.Visit(elf32_loader);
			if(ret) return ret;
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; ++i)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_elf64_loaders[front_end_num][i])
		{
			typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
			T ret = visitor.Visit(elf64_loader);
			if(ret) return ret;
		}
	}

	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; ++i)
	{
		if((front_end_num >= MAX_FRONT_ENDS) || enable_coff_loaders[front_end_num][i])
		{
			typename unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = coff_loaders[i];
			T ret = visitor.Visit(coff_loader);
			if(ret) return ret;
		}
	}
	return 0;
}

template <typename CONFIG>
template <typename VISITOR, typename T>
T Debugger<CONFIG>::ScanAndFilterLoadersByFilename(unsigned int front_end_num, const char *filename, VISITOR& visitor) const
{
	unsigned int i;
	
	unsigned int num_elf32_loaders = elf32_loaders.size();
	for(i = 0; i < num_elf32_loaders; ++i)
	{
		typename unisim::util::loader::elf_loader::Elf32Loader<ADDRESS> *elf32_loader = elf32_loaders[i];
		const unisim::util::blob::Blob<ADDRESS> *blob = elf32_loader->GetBlob();
		if((filename && ((blob->GetCapability() & unisim::util::blob::CAP_FILENAME) && (strcmp(blob->GetFilename(), filename) == 0))) ||
			 (!filename && ((front_end_num >= MAX_FRONT_ENDS) || enable_elf32_loaders[front_end_num][i])))
		{
			T ret = visitor.Visit(elf32_loader);
			if(ret) return ret;
		}
	}

	unsigned int num_elf64_loaders = elf64_loaders.size();
	for(i = 0; i < num_elf64_loaders; ++i)
	{
		typename unisim::util::loader::elf_loader::Elf64Loader<ADDRESS> *elf64_loader = elf64_loaders[i];
		const unisim::util::blob::Blob<ADDRESS> *blob = elf64_loader->GetBlob();
		if((filename && ((blob->GetCapability() & unisim::util::blob::CAP_FILENAME) && (strcmp(blob->GetFilename(), filename) == 0))) ||
			 (!filename && ((front_end_num >= MAX_FRONT_ENDS) || enable_elf64_loaders[front_end_num][i])))
		{
			T ret = visitor.Visit(elf64_loader);
			if(ret) return ret;
		}
	}
	
	unsigned int num_coff_loaders = coff_loaders.size();
	for(i = 0; i < num_coff_loaders; ++i)
	{
		typename unisim::util::loader::coff_loader::CoffLoader<ADDRESS> *coff_loader = coff_loaders[i];
		const unisim::util::blob::Blob<ADDRESS> *blob = coff_loader->GetBlob();
		if((filename && ((blob->GetCapability() & unisim::util::blob::CAP_FILENAME) && (strcmp(blob->GetFilename(), filename) == 0))) ||
			 (!filename && ((front_end_num >= MAX_FRONT_ENDS) || enable_coff_loaders[front_end_num][i])))
		{
			T ret = visitor.Visit(coff_loader);
			if(ret) return ret;
		}
	}
	
	return T();
}

} // end of namespace debugger
} // end of namespace debug
} // end of namespace service
} // end of namespace unisim

#endif
