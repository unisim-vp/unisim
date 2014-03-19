/*
 *  Copyright (c) 2007-2011,
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
 * Authors: Julien Lisita (julien.lisita@cea.fr)
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __AT32UC3C_SIMULATOR_TCC__
#define __AT32UC3C_SIMULATOR_TCC__

template <class CONFIG>
Simulator<CONFIG>::Simulator(int argc, char **argv)
	: unisim::kernel::service::Simulator(argc, argv, LoadBuiltInConfig)
	, cpu(0)
	, ram(0)
	, loader(0)
	, debugger(0)
	, gdb_server(0)
	, inline_debugger(0)
	, profiler(0)
	, sim_time(0)
	, host_time(0)
	, tee_memory_access_reporting(0)
	, enable_gdb_server(false)
	, enable_inline_debugger(false)
	, param_enable_gdb_server("enable-gdb-server", 0, enable_gdb_server, "Enable/Disable GDB server instantiation")
	, param_enable_inline_debugger("enable-inline-debugger", 0, enable_inline_debugger, "Enable/Disable inline debugger instantiation")
	, exit_status(0)
{
	// Optionally get the program to load from the command line arguments
	VariableBase *cmd_args = FindVariable("cmd-args");
	unsigned int cmd_args_length = cmd_args->GetLength();
	if(cmd_args_length > 0)
	{
		SetVariable("loader.filename", ((string)(*cmd_args)[0]).c_str());
	}

	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	//  - AVR32UC processor
	cpu = new CPU("cpu");
	//  - RAM
	ram = new RAM("ram");
	//  - Memory Router
	memory_router = new MEMORY_ROUTER("memory-router");
	//  - NMIREQ Stub
	nmireq_stub = new NMIREQ_STUB("nmireq-stub");
	//  - IRQ Stubs
	unsigned int irq;
	for(irq = 0; irq < CONFIG::CPU_CONFIG::NUM_IRQS; irq++)
	{
		std::stringstream sstr_irq_stub_name;
		sstr_irq_stub_name << "irq-stub" << irq;
		irq_stub[irq] = new IRQ_STUB(sstr_irq_stub_name.str().c_str());
	}
	
	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================
	//  - Multiformat loader
	loader = new MultiFormatLoader<CPU_ADDRESS_TYPE>("loader");
	//  - debugger
	debugger = new Debugger<CPU_ADDRESS_TYPE>("debugger");
	//  - GDB server
	gdb_server = enable_gdb_server ? new GDBServer<CPU_ADDRESS_TYPE>("gdb-server") : 0;
	//  - Inline debugger
	inline_debugger = enable_inline_debugger ? new InlineDebugger<CPU_ADDRESS_TYPE>("inline-debugger") : 0;
	//  - profiler
	profiler = enable_inline_debugger ? new Profiler<CPU_ADDRESS_TYPE>("profiler") : 0;
	//  - SystemC Time
	sim_time = new unisim::service::time::sc_time::ScTime("time");
	//  - Host Time
	host_time = new unisim::service::time::host_time::HostTime("host-time");
	//  - Tee Memory Access Reporting
	tee_memory_access_reporting = enable_inline_debugger ? new unisim::service::tee::memory_access_reporting::Tee<CPU_ADDRESS_TYPE>("tee-memory-access-reporting") : 0;
	
	//=========================================================================
	//===                        Components connection                      ===
	//=========================================================================

	cpu->dhsb_master_sock(*memory_router->targ_socket[0]); // CPU>DHSB      <-> Memory Router
	cpu->ihsb_master_sock(*memory_router->targ_socket[1]); // CPU>IHSB      <-> Memory Router
	(*memory_router->init_socket[0])(ram->slave_sock);     // Memory Router <-> RAM
	nmireq_stub->master_sock(cpu->nmireq_slave_sock);      // NMIREQ Stub   <-> CPU>NMIREQ
	for(irq = 0; irq < CONFIG::CPU_CONFIG::NUM_IRQS; irq++)
	{
		irq_stub[irq]->master_sock(*cpu->irq_slave_sock[irq]);  // IRQ Stub <-> CPU>IRQ
	}

	//=========================================================================
	//===                        Clients/Services connection                ===
	//=========================================================================

	cpu->memory_import >> memory_router->memory_export;
	*memory_router->memory_import[0] >> ram->memory_export;
	
	// Connect debugger to CPU
	cpu->debug_control_import >> debugger->debug_control_export;
	cpu->trap_reporting_import >> debugger->trap_reporting_export;
	cpu->symbol_table_lookup_import >> debugger->symbol_table_lookup_export;
	debugger->disasm_import >> cpu->disasm_export;
	debugger->memory_import >> cpu->memory_export;
	debugger->registers_import >> cpu->registers_export;
	debugger->blob_import >> loader->blob_export;
	
	if(enable_inline_debugger)
	{
		// Connect tee-memory-access-reporting to CPU, debugger and profiler
		cpu->memory_access_reporting_import >> tee_memory_access_reporting->in;
		*tee_memory_access_reporting->out[0] >> profiler->memory_access_reporting_export;
		*tee_memory_access_reporting->out[1] >> debugger->memory_access_reporting_export;
		profiler->memory_access_reporting_control_import >> *tee_memory_access_reporting->in_control[0];
		debugger->memory_access_reporting_control_import >> *tee_memory_access_reporting->in_control[1];
		tee_memory_access_reporting->out_control >> cpu->memory_access_reporting_control_export;
	}
	else
	{
		cpu->memory_access_reporting_import >> debugger->memory_access_reporting_export;
		debugger->memory_access_reporting_control_import >> cpu->memory_access_reporting_control_export;
	}
	
	if(enable_inline_debugger)
	{
		// Connect inline-debugger to debugger
		debugger->debug_event_listener_import >> inline_debugger->debug_event_listener_export;
		debugger->trap_reporting_import >> inline_debugger->trap_reporting_export;
		debugger->debug_control_import >> inline_debugger->debug_control_export;
		inline_debugger->debug_event_trigger_import >> debugger->debug_event_trigger_export;
		inline_debugger->disasm_import >> debugger->disasm_export;
		inline_debugger->memory_import >> debugger->memory_export;
		inline_debugger->registers_import >> debugger->registers_export;
		inline_debugger->stmt_lookup_import >> debugger->stmt_lookup_export;
		inline_debugger->symbol_table_lookup_import >> debugger->symbol_table_lookup_export;
		inline_debugger->backtrace_import >> debugger->backtrace_export;
		inline_debugger->debug_info_loading_import >> debugger->debug_info_loading_export;
		inline_debugger->data_object_lookup_import >> debugger->data_object_lookup_export;
		inline_debugger->profiling_import >> profiler->profiling_export;
	}
	else if(enable_gdb_server)
	{
		// Connect gdb-server to debugger
		debugger->debug_control_import >> gdb_server->debug_control_export;
		debugger->debug_event_listener_import >> gdb_server->debug_event_listener_export;
		debugger->trap_reporting_import >> gdb_server->trap_reporting_export;
		gdb_server->debug_event_trigger_import >> debugger->debug_event_trigger_export;
		gdb_server->memory_import >> debugger->memory_export;
		gdb_server->registers_import >> debugger->registers_export;
	}
	
	*loader->memory_import[0] >> ram->memory_export;
}

template <class CONFIG>
Simulator<CONFIG>::~Simulator()
{
	if(ram) delete ram;
	if(memory_router) delete memory_router;
	if(debugger) delete debugger;
	if(gdb_server) delete gdb_server;
	if(inline_debugger) delete inline_debugger;
	if(profiler) delete profiler;
	if(cpu) delete cpu;
	if(sim_time) delete sim_time;
	if(host_time) delete host_time;
	if(loader) delete loader;
	if(tee_memory_access_reporting) delete tee_memory_access_reporting;
	if(nmireq_stub) delete nmireq_stub;
	unsigned int irq;
	for(irq = 0; irq < CONFIG::CPU_CONFIG::NUM_IRQS; irq++)
	{
		if(irq_stub[irq]) delete irq_stub[irq];
	}
}

template <class CONFIG>
void Simulator<CONFIG>::LoadBuiltInConfig(unisim::kernel::service::Simulator *simulator)
{
	// meta information
	simulator->SetVariable("program-name", "UNISIM AT32UC3C");
	simulator->SetVariable("copyright", "Copyright (C) 2014, Commissariat a l'Energie Atomique (CEA)");
	simulator->SetVariable("license", "BSD (see file COPYING)");
	simulator->SetVariable("authors", "Julien Lisita <julien.lisita@cea.fr>, Gilles Mouchard <gilles.mouchard@cea.fr>");
	simulator->SetVariable("version", VERSION);
	simulator->SetVariable("description", "UNISIM AT32UC3C, AVR32UC simulator");
	simulator->SetVariable("schematic", "at32uc3c/fig_schematic.pdf");

	int gdb_server_tcp_port = 0;
	const char *gdb_server_arch_filename = "gdb_avr32.xml";
	const char *dwarf_register_number_mapping_filename = "avr32_dwarf_register_number_mapping.xml";
	uint64_t maxinst = 0xffffffffffffffffULL; // maximum number of instruction to simulate
	double cpu_frequency = 66.67; // in Mhz
	double cpu_clock_multiplier = 1.0;
	double cpu_ipc = 1.0; // in instructions per cycle: AVR32UC is RISC pipelined processor that can execute up to 1 instruction per cycle
	double cpu_cycle_time = (double)(1.0e6 / cpu_frequency); // in picoseconds
	double hsb_cycle_time = cpu_clock_multiplier * cpu_cycle_time;
	double mem_cycle_time = hsb_cycle_time;

	//=========================================================================
	//===                     Component run-time configuration              ===
	//=========================================================================

	//  - PowerPC processor
	// if the following line ("cpu-cycle-time") is commented, the cpu will use the power estimators to find min cpu cycle time
	simulator->SetVariable("cpu.cpu-cycle-time", sc_time(cpu_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("cpu.bus-cycle-time", sc_time(hsb_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("cpu.max-inst", maxinst);
	simulator->SetVariable("cpu.nice-time", "1 ms");
	simulator->SetVariable("cpu.ipc", cpu_ipc);
	simulator->SetVariable("cpu.enable-dmi", true); // Allow CPU to use of SystemC TLM 2.0 DMI

	//  - Memory router
	simulator->SetVariable("memory-router.cycle_time", sc_time(hsb_cycle_time, SC_PS).to_string().c_str());	simulator->SetVariable("memory-router.mapping_0", "range_start=\"0x0\" range_end=\"0xffffffffffffffff\" output_port=\"0\" translation=\"0x0\""); // RAM

	// - Loader memory router
	std::stringstream sstr_loader_mapping;
	simulator->SetVariable("loader.memory-mapper.mapping", "ram:0x0-0xffffffff"); // FIXME: this is all address space

	//  - RAM
	simulator->SetVariable("ram.cycle-time", sc_time(mem_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("ram.read-latency", sc_time(mem_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("ram.write-latency", SC_ZERO_TIME.to_string().c_str());
	simulator->SetVariable("ram.org", CONFIG::RAM_BASE_ADDR);
	simulator->SetVariable("ram.bytesize", CONFIG::RAM_BYTE_SIZE);
	
	//=========================================================================
	//===                      Service run-time configuration               ===
	//=========================================================================

	//  - GDB Server run-time configuration
	simulator->SetVariable("gdb-server.tcp-port", gdb_server_tcp_port);
	simulator->SetVariable("gdb-server.architecture-description-filename", gdb_server_arch_filename);
	
	//  - Debugger run-time configuration
	simulator->SetVariable("debugger.parse-dwarf", false);
	simulator->SetVariable("debugger.dwarf-register-number-mapping-filename", dwarf_register_number_mapping_filename);
}

template <class CONFIG>
void Simulator<CONFIG>::Run()
{
	cerr << "Starting simulation" << endl;
	
	double time_start = host_time->GetTime();

#ifndef WIN32
	void (*prev_sig_int_handler)(int) = 0;
#endif

	if(!inline_debugger)
	{
#ifdef WIN32
		SetConsoleCtrlHandler(&Simulator<CONFIG>::ConsoleCtrlHandler, TRUE);
#else
		prev_sig_int_handler = signal(SIGINT, &Simulator<CONFIG>::SigIntHandler);
#endif
	}

	sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING); // disable SystemC messages
	
	try
	{
		sc_start();
	}
	catch(std::runtime_error& e)
	{
		cerr << "FATAL ERROR! an abnormal error occured during simulation. Bailing out..." << endl;
		cerr << e.what() << endl;
	}

	if(!inline_debugger)
	{
#ifdef WIN32
		SetConsoleCtrlHandler(&Simulator<CONFIG>::ConsoleCtrlHandler, FALSE);
#else
		signal(SIGINT, prev_sig_int_handler);
#endif
	}

	cerr << "Simulation finished" << endl;

	double time_stop = host_time->GetTime();
	double spent_time = time_stop - time_start;

	cerr << "Simulation run-time parameters:" << endl;
	DumpParameters(cerr);
	cerr << endl;
	cerr << "Simulation formulas:" << endl;
	DumpFormulas(cerr);
	cerr << endl;
	cerr << "Simulation statistics:" << endl;
	DumpStatistics(cerr);
	cerr << endl;

	cerr << "simulation time: " << spent_time << " seconds" << endl;
	cerr << "simulated time : " << sc_time_stamp().to_seconds() << " seconds (exactly " << sc_time_stamp() << ")" << endl;
	cerr << "target speed: " << ((double) (*cpu)["instruction-counter"] / ((double) (*cpu)["run-time"] - (double) (*cpu)["idle-time"]) / 1000000.0) << " MIPS" << endl;
	cerr << "host simulation speed: " << ((double) (*cpu)["instruction-counter"] / spent_time / 1000000.0) << " MIPS" << endl;
	cerr << "time dilatation: " << spent_time / sc_time_stamp().to_seconds() << " times slower than target machine" << endl;
}

template <class CONFIG>
unisim::kernel::service::Simulator::SetupStatus Simulator<CONFIG>::Setup()
{
	if(enable_inline_debugger)
	{
		SetVariable("debugger.parse-dwarf", true);
	}
	
	unisim::kernel::service::Simulator::SetupStatus setup_status = unisim::kernel::service::Simulator::Setup();
	
	// inline-debugger and gdb-server are exclusive
	if(enable_inline_debugger && enable_gdb_server)
	{
		std::cerr << "ERROR! " << inline_debugger->GetName() << " and " << gdb_server->GetName() << " shall not be used together. Use " << param_enable_inline_debugger.GetName() << " and " << param_enable_gdb_server.GetName() << " to enable only one of the two" << std::endl;
		return unisim::kernel::service::Simulator::ST_ERROR;
	}
	
	return setup_status;
}

template <class CONFIG>
void Simulator<CONFIG>::Stop(Object *object, int _exit_status, bool asynchronous)
{
	exit_status = _exit_status;
	if(object)
	{
		std::cerr << object->GetName() << " has requested simulation stop" << std::endl << std::endl;
	}
#ifdef DEBUG_AT32UC3C
	std::cerr << "Call stack:" << std::endl;
	std::cerr << unisim::kernel::debug::BackTrace() << std::endl;
#endif
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

template <class CONFIG>
int Simulator<CONFIG>::GetExitStatus() const
{
	return exit_status;
}

#ifdef WIN32
template <class CONFIG>
BOOL WINAPI Simulator<CONFIG>::ConsoleCtrlHandler(DWORD dwCtrlType)
{
	bool stop = false;
	switch(dwCtrlType)
	{
		case CTRL_C_EVENT:
			cerr << "Interrupted by Ctrl-C" << endl;
			stop = true;
			break;
		case CTRL_BREAK_EVENT:
			cerr << "Interrupted by Ctrl-Break" << endl;
			stop = true;
			break;
		case CTRL_CLOSE_EVENT:
			cerr << "Interrupted by a console close" << endl;
			stop = true;
			break;
		case CTRL_LOGOFF_EVENT:
			cerr << "Interrupted because of logoff" << endl;
			stop = true;
			break;
		case CTRL_SHUTDOWN_EVENT:
			cerr << "Interrupted because of shutdown" << endl;
			stop = true;
			break;
	}
	if(stop) sc_stop();
	return stop ? TRUE : FALSE;
}
#else
template <class CONFIG>
void Simulator<CONFIG>::SigIntHandler(int signum)
{
	cerr << "Interrupted by Ctrl-C or SIGINT signal" << endl;
	unisim::kernel::service::Simulator::simulator->Stop(0, 0, true);
}
#endif

#endif // __AT32UC3C_SIMULATOR_TCC__
