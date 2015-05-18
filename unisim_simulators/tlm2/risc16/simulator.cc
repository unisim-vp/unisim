/*
 *  Copyright (c) 2007-2015,
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
 * Authors: Réda Nouacer (reda.nouacer@cea.fr)
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#include <simulator.hh>

Simulator::Simulator(int argc, char **argv)
	: unisim::kernel::service::Simulator(argc, argv, LoadBuiltInConfig)
	, cpu(0)
	, memory(0)
	, debugger(0)
	, inline_debugger(0)
	, raw_loader(0)
	, tee_memory_access_reporting(0)
	, profiler(0)
	, exit_status(0)
{
	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	// - RISC16 processor
	cpu = new CPU("cpu");

	// - Memory
	memory = new RAM("memory");

	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================

	// - Debugger
	debugger = new Debugger<uint64_t>("debugger");
	
	// - Built-in console debugger (Inline-debugger)
	inline_debugger = new InlineDebugger<uint64_t>("inline-debugger");
	
	// - Raw loader
	raw_loader = new RawLoader("raw-loader");
	
	// - Tee Memory Access Reporting (broadcast memory access reports of CPU to debugger and profiler)
	tee_memory_access_reporting = new MemoryAccessReportingTee("tee-memory-access-reporting");

	// - Profiler
	profiler = new Profiler<uint64_t>("profiler");

	//=========================================================================
	//===                        Components connection                      ===
	//=========================================================================

	cpu->master_sock(memory->slave_sock);

	//=========================================================================
	//===                       Clients/Services connections                ===
	//=========================================================================

	// CPU imports
	cpu->memory_import >> memory->memory_export;                    // memory
	cpu->debug_control_import >> debugger->debug_control_export;    // makes debugger control CPU execution
	cpu->trap_reporting_import >> debugger->trap_reporting_export;  // report trap
	cpu->memory_access_reporting_import >> tee_memory_access_reporting->in; // memory access reports of CPU
	
	// raw-loader imports
	raw_loader->memory_import >> memory->memory_export;    // memory

	// tee imports
	*tee_memory_access_reporting->out[0] >> profiler->memory_access_reporting_export;
	*tee_memory_access_reporting->out[1] >> debugger->memory_access_reporting_export;
	tee_memory_access_reporting->out_control >> cpu->memory_access_reporting_control_export;

	// profiler imports
	profiler->memory_access_reporting_control_import >> *tee_memory_access_reporting->in_control[0];

	// central debugger imports
	debugger->blob_import >> raw_loader->blob_export;                                     // blob (binary large object), i.e. executable binary files
	debugger->memory_access_reporting_control_import >> *tee_memory_access_reporting->in_control[1]; // turn on/off memory access reporting
	debugger->disasm_import >> cpu->disasm_export;                                        // disassemble
	debugger->memory_import >> cpu->memory_export;                                        // memory (programmer's view)
	debugger->registers_import >> cpu->registers_export;                                  // CPU registers
	debugger->debug_control_import >> inline_debugger->debug_control_export;              // passthrough debug control
	debugger->debug_event_listener_import >> inline_debugger->debug_event_listener_export; // notify debug events to listeners
	debugger->trap_reporting_import >> inline_debugger->trap_reporting_export;             // report trap
	
	// built-in console debugger imports
	inline_debugger->debug_event_trigger_import >> debugger->debug_event_trigger_export;   // listen of debugging events
	inline_debugger->disasm_import >> debugger->disasm_export;                             // disassemble
	inline_debugger->memory_import >> debugger->memory_export;                             // memory
	inline_debugger->registers_import >> debugger->registers_export;                       // access to registers
	inline_debugger->data_object_lookup_import >> debugger->data_object_lookup_export;     // lookup of data object (if debug info is available)
	inline_debugger->stmt_lookup_import >> debugger->stmt_lookup_export;                   // lookup instruction/source statements (if debug info is available)                  
	inline_debugger->symbol_table_lookup_import >> debugger->symbol_table_lookup_export;   // lookup symbol table
	inline_debugger->backtrace_import >> debugger->backtrace_export;                       // backtrace (if debug info is available)
	inline_debugger->debug_info_loading_import >> debugger->debug_info_loading_export;     // on-demand loading of debug info
	inline_debugger->profiling_import >> profiler->profiling_export;                       // profiling
}

Simulator::~Simulator()
{
	if(cpu) delete cpu;
	if(memory) delete memory;
	if(raw_loader) delete raw_loader;
	if(tee_memory_access_reporting) delete tee_memory_access_reporting;
	if(profiler) delete profiler;
}

void Simulator::LoadBuiltInConfig(unisim::kernel::service::Simulator *simulator)
{
	// meta information
	simulator->SetVariable("program-name", "UNISIM risc16");
	simulator->SetVariable("copyright", "Copyright (C) 2008-2015, Commissariat a l'Energie Atomique (CEA)");
	simulator->SetVariable("license", "BSD (see file COPYING)");
	simulator->SetVariable("authors", "Réda Nouacer <reda.nouacer@cea.fr>, Gilles Mouchard <gilles.mouchard@cea.fr>");

#ifdef HAVE_CONFIG_H
	simulator->SetVariable("version", VERSION);
#endif

	simulator->SetVariable("description", "UNISIM risc16, a simple 16-bit RISC simulator for teaching");

	//=========================================================================
	//===                     Component run-time configuration              ===
	//=========================================================================

	simulator->SetVariable("memory.org", 0x0);
	simulator->SetVariable("memory.bytesize", 65536);
	simulator->SetVariable("memory.cycle-time", 10000); // 10 000 ps
	simulator->SetVariable("memory.verbose", false);
	
	//=========================================================================
	//===                      Service run-time configuration               ===
	//=========================================================================

	simulator->SetVariable("inline-debugger.memory-atom-size", 2); // memory is not byte addressable
	simulator->SetVariable("kernel_logger.std_err", true);
	simulator->SetVariable("kernel_logger.std_out", false);
	simulator->SetVariable("kernel_logger.std_err_color", false);
	simulator->SetVariable("kernel_logger.std_out_color", false);
	simulator->SetVariable("kernel_logger.file", false);
	simulator->SetVariable("kernel_logger.filename", "logger_output.txt");
	simulator->SetVariable("kernel_logger.xml_file", false);
	simulator->SetVariable("kernel_logger.xml_filename", "logger_output.xml");
	simulator->SetVariable("kernel_logger.xml_file_gzipped", false);
}

void Simulator::Stop(Object *object, int _exit_status, bool asynchronous)
{
	exit_status = _exit_status;
	if(object)
	{
		std::cerr << object->GetName() << " has requested simulation stop" << std::endl << std::endl;
	}

	std::cerr << "Program exited with status " << exit_status << std::endl;
	sc_stop();
	if(!asynchronous)
	{
		switch(sc_get_curr_simcontext()->get_curr_proc_info()->kind)
		{
			case SC_THREAD_PROC_:
			case SC_CTHREAD_PROC_:
				wait();
				break;
			default:
				break;
		}
	}
}

void Simulator::Run() {

	cerr << "Starting simulation ..." << endl;

	try
	{
		sc_start();
	}
	catch(std::runtime_error& e)
	{
		cerr << "FATAL ERROR! an abnormal error occurred during simulation. Bailing out..." << endl;
		cerr << e.what() << endl;
	}

	cerr << "Simulation finished" << endl << endl;
}

