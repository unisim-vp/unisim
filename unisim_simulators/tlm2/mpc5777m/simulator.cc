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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#include <simulator.hh>
#include "unisim/component/tlm2/interconnect/generic_router/router.tcc"

Simulator::Simulator(const sc_core::sc_module_name& name, int argc, char **argv)
	: unisim::kernel::tlm2::Simulator(name, argc, argv, LoadBuiltInConfig)
	, cpu(0)
	, ram(0)
	, interconnect(0)
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
	//=========================================================================
	//===                     Component instantiations                      ===
	//=========================================================================
	//  - PowerPC processor
	cpu = new CPU("CPU", this);

	//  - RAM
	ram = new RAM("RAM", this);
	
	//  - Interconnect
	interconnect = new INTERCONNECT("INTERCONNECT", this);

	//=========================================================================
	//===                         Service instantiations                    ===
	//=========================================================================
	//  - Multiformat loader
	loader = new MultiFormatLoader<CPU_ADDRESS_TYPE>("loader");
	//  - debugger
	debugger = (enable_inline_debugger || enable_gdb_server) ? new Debugger<CPU_ADDRESS_TYPE>("debugger") : 0;
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
	//===                          Port registration                        ===
	//=========================================================================
	RegisterPort(cpu->m_por);
	RegisterPort(cpu->p_reset_b);
	RegisterPort(cpu->p_nmi_b);
	RegisterPort(cpu->p_mcp_b);
	RegisterPort(cpu->p_rstbase);
	RegisterPort(cpu->p_cpuid);
	RegisterPort(cpu->p_extint_b);
	RegisterPort(cpu->p_crint_b);
	RegisterPort(cpu->p_avec_b);
	RegisterPort(cpu->p_voffset);
	RegisterPort(cpu->p_iack);

	//=========================================================================
	//===                           Signal creation                         ===
	//=========================================================================
	
	CreateSignal("m_por", false);
	CreateSignal("p_reset_b", false);
	CreateSignal("p_nmi_b", true);
	CreateSignal("p_mcp_b", true);
	CreateSignal("p_rstbase", sc_dt::sc_uint<30>(0));
	CreateSignal("p_cpuid", sc_dt::sc_uint<8>(0));
	CreateSignal("p_extint_b", true);
	CreateSignal("p_crint_b", true);
	CreateSignal("p_avec_b", true);
	CreateSignal("p_voffset", sc_dt::sc_uint<14>(0));
	CreateSignal("p_iack", false);
	
	//=========================================================================
	//===                        Components connection                      ===
	//=========================================================================

	cpu->ahb_if(*interconnect->targ_socket[0]); // CPU>AHB_IF <-> Memory Router
	(*interconnect->init_socket[0])(ram->slave_sock); // Memory Router <-> RAM

	Bind("HARDWARE.CPU.m_por"           , "HARDWARE.m_por");
	Bind("HARDWARE.CPU.p_reset_b"       , "HARDWARE.p_reset_b");
	Bind("HARDWARE.CPU.p_nmi_b"         , "HARDWARE.p_nmi_b");
	Bind("HARDWARE.CPU.p_mcp_b"         , "HARDWARE.p_mcp_b");
	Bind("HARDWARE.CPU.p_rstbase"       , "HARDWARE.p_rstbase");
	Bind("HARDWARE.CPU.p_cpuid"         , "HARDWARE.p_cpuid");
	Bind("HARDWARE.CPU.p_extint_b"      , "HARDWARE.p_extint_b");
	Bind("HARDWARE.CPU.p_crint_b"       , "HARDWARE.p_crint_b");
	Bind("HARDWARE.CPU.p_avec_b"        , "HARDWARE.p_avec_b");
	Bind("HARDWARE.CPU.p_voffset"       , "HARDWARE.p_voffset");
	Bind("HARDWARE.CPU.p_iack"          , "HARDWARE.p_iack");
	
	//=========================================================================
	//===                        Clients/Services connection                ===
	//=========================================================================

	cpu->memory_import >> interconnect->memory_export;

	(*interconnect->memory_import[0]) >> ram->memory_export;
//	cpu->loader_import >> loader->loader_export;
		
	if(enable_inline_debugger || enable_gdb_server)
	{
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
			// Connect CPU to debugger
			cpu->memory_access_reporting_import >> debugger->memory_access_reporting_export;
			debugger->memory_access_reporting_control_import >> cpu->memory_access_reporting_control_export;
		}

		// Connect debugger to CPU
		cpu->debug_control_import >> debugger->debug_control_export;
		cpu->trap_reporting_import >> debugger->trap_reporting_export;
		debugger->disasm_import >> cpu->disasm_export;
		debugger->memory_import >> cpu->memory_export;
		debugger->registers_import >> cpu->registers_export;
		debugger->loader_import >> loader->loader_export;
		debugger->blob_import >> loader->blob_export;
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
		inline_debugger->subprogram_lookup_import >> debugger->subprogram_lookup_export;
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

	(*loader->memory_import[0]) >> ram->memory_export;
	loader->registers_import >> cpu->registers_export;
	cpu->symbol_table_lookup_import >> loader->symbol_table_lookup_export;
	
	SC_HAS_PROCESS(Simulator);
	
	SC_THREAD(ResetProcess);
}

Simulator::~Simulator()
{
	if(ram) delete ram;
	if(interconnect) delete interconnect;
	if(debugger) delete debugger;
	if(gdb_server) delete gdb_server;
	if(inline_debugger) delete inline_debugger;
	if(profiler) delete profiler;
	if(cpu) delete cpu;
	if(sim_time) delete sim_time;
	if(host_time) delete host_time;
	if(loader) delete loader;
	if(tee_memory_access_reporting) delete tee_memory_access_reporting;
}

void Simulator::ResetProcess()
{
	sc_core::sc_signal<sc_dt::sc_uint<30> >& p_rstbase = GetSignal<sc_dt::sc_uint<30> >("HARDWARE.p_rstbase");
	p_rstbase = sc_dt::sc_uint<30>(/*0x404100*/ /*0x13a0000*/ 0x1500000 >> 2);
	sc_core::sc_signal<bool>& p_reset_b = GetSignal<bool>("HARDWARE.p_reset_b");
	p_reset_b = false;
	wait(sc_core::sc_time(10.0, sc_core::SC_NS));
	p_reset_b = true;
	
	wait(sc_core::sc_time(40.0, sc_core::SC_NS));
	sc_core::sc_signal<sc_dt::sc_uint<14> >& p_voffset = GetSignal<sc_dt::sc_uint<14> >("HARDWARE.p_voffset");
	p_voffset = sc_dt::sc_uint<14>(0x1234);

	sc_core::sc_signal<bool>& p_avec_b = GetSignal<bool>("HARDWARE.p_avec_b");
	p_avec_b = true;

	sc_core::sc_signal<bool>& p_extint_b = GetSignal<bool>("HARDWARE.p_extint_b");
	p_extint_b = false;
	wait(sc_core::sc_time(10.0, sc_core::SC_NS));
	p_extint_b = true;
}

void Simulator::LoadBuiltInConfig(unisim::kernel::service::Simulator *simulator)
{
	// meta information
	simulator->SetVariable("program-name", "UNISIM MPC5777M");
	simulator->SetVariable("copyright", "Copyright (C) 2017, Commissariat a l'Energie Atomique (CEA)");
	simulator->SetVariable("license", "BSD (see file COPYING)");
	simulator->SetVariable("authors", "Gilles Mouchard <gilles.mouchard@cea.fr>");
	simulator->SetVariable("version", VERSION);
	simulator->SetVariable("description", "UNISIM MPC5777M, full system e200z-based simulator");
	simulator->SetVariable("schematic", "mpc5777m/fig_schematic.pdf");

	int gdb_server_tcp_port = 0;
	const char *gdb_server_arch_filename = "gdb_powerpc_vle.xml";
	const char *dwarf_register_number_mapping_filename = "powerpc_eabi_gcc_dwarf_register_number_mapping.xml";
	uint64_t maxinst = 0xffffffffffffffffULL; // maximum number of instruction to simulate
	double cpu_frequency = 200.0; // in Mhz
	double cpu_clock_multiplier = 2.0;
	double ext_timer_clock_divisor = 2.0;
	double cpu_ipc = 2.0; // in instructions per cycle
	double cpu_cycle_time = (double)(1.0e6 / cpu_frequency); // in picoseconds
	double fsb_cycle_time = cpu_clock_multiplier * cpu_cycle_time;
	double ext_timer_cycle_time = ext_timer_clock_divisor * cpu_cycle_time;
	double mem_cycle_time = fsb_cycle_time;

	//=========================================================================
	//===                     Component run-time configuration              ===
	//=========================================================================

	//  - PowerPC processor
	// if the following line ("cpu-cycle-time") is commented, the cpu will use the power estimators to find min cpu cycle time
	simulator->SetVariable("HARDWARE.CPU.cpu-cycle-time", sc_time(cpu_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("HARDWARE.CPU.bus-cycle-time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("HARDWARE.CPU.ext-timer-cycle-time", sc_time(ext_timer_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("HARDWARE.CPU.voltage", 1.0 * 1e3); // mV
	simulator->SetVariable("HARDWARE.CPU.max-inst", maxinst);
	simulator->SetVariable("HARDWARE.CPU.nice-time", "200 ns"); // 200 ns (currently geared to the minimum interval between capture trigger samples)
	simulator->SetVariable("HARDWARE.CPU.ipc", cpu_ipc);
	simulator->SetVariable("HARDWARE.CPU.enable-dmi", true); // Allow CPU to use of SystemC TLM 2.0 DMI

	simulator->SetVariable("HARDWARE.CPU.dmem-base-addr", 0x52800000);
	simulator->SetVariable("HARDWARE.CPU.dmem-size", 64 * 1024);

	simulator->SetVariable("HARDWARE.CPU.imem-base-addr", 0x52000000);
	simulator->SetVariable("HARDWARE.CPU.imem-size", 16 * 1024);
	
	//  - Interconnect
	simulator->SetVariable("HARDWARE.INTERCONNECT.cycle_time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());

	//  - Memory router
	simulator->SetVariable("HARDWARE.INTERCONNECT.cycle_time", sc_time(fsb_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("HARDWARE.INTERCONNECT.mapping_0", "range_start=\"0x0\" range_end=\"0xffffffff\" output_port=\"0\" translation=\"0x0\""); // RAM

	// - Loader memory router
	std::stringstream sstr_loader_mapping;
	sstr_loader_mapping << "HARDWARE.RAM:0x0-0x0fffffff:+0x0" << std::dec;
	simulator->SetVariable("loader.memory-mapper.mapping", sstr_loader_mapping.str().c_str());

	//  - RAM
	simulator->SetVariable("HARDWARE.RAM.cycle-time", sc_time(mem_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("HARDWARE.RAM.read-latency", sc_time(mem_cycle_time, SC_PS).to_string().c_str());
	simulator->SetVariable("HARDWARE.RAM.write-latency", SC_ZERO_TIME.to_string().c_str());
	simulator->SetVariable("HARDWARE.RAM.org", 0x0);
	simulator->SetVariable("HARDWARE.RAM.bytesize", 256 * 1024 * 1024);
	
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

void Simulator::Run()
{
	cerr << "Starting simulation at supervisor privilege level" << endl;
	
	double time_start = host_time->GetTime();

#ifndef WIN32
	void (*prev_sig_int_handler)(int) = 0;
#endif

	if(!inline_debugger)
	{
#ifdef WIN32
		SetConsoleCtrlHandler(&Simulator::ConsoleCtrlHandler, TRUE);
#else
		prev_sig_int_handler = signal(SIGINT, &Simulator::SigIntHandler);
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
		SetConsoleCtrlHandler(&Simulator::ConsoleCtrlHandler, FALSE);
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

unisim::kernel::service::Simulator::SetupStatus Simulator::Setup()
{
	if(enable_inline_debugger)
	{
		SetVariable("debugger.parse-dwarf", true);
	}
	
	// Optionally get the program to load from the command line arguments
	VariableBase *cmd_args = FindVariable("cmd-args");
	unsigned int cmd_args_length = cmd_args->GetLength();
	if(cmd_args_length > 0)
	{
		SetVariable("loader.filename", ((string)(*cmd_args)[0]).c_str());
	}

	unisim::kernel::service::Simulator::SetupStatus setup_status = unisim::kernel::service::Simulator::Setup();
	
	// inline-debugger and gdb-server are exclusive
	if(enable_inline_debugger && enable_gdb_server)
	{
		std::cerr << "ERROR! " << inline_debugger->GetName() << " and " << gdb_server->GetName() << " shall not be used together. Use " << param_enable_inline_debugger.GetName() << " and " << param_enable_gdb_server.GetName() << " to enable only one of the two" << std::endl;
		if(setup_status != unisim::kernel::service::Simulator::ST_OK_DONT_START)
		{
			setup_status = unisim::kernel::service::Simulator::ST_ERROR;
		}
	}
	
	return setup_status;
}

void Simulator::Stop(Object *object, int _exit_status, bool asynchronous)
{
	exit_status = _exit_status;
	if(object)
	{
		std::cerr << object->GetName() << " has requested simulation stop" << std::endl << std::endl;
	}
	if(DEBUG_ENABLE)
	{
		std::cerr << "Call stack:" << std::endl;
		std::cerr << unisim::util::backtrace::BackTrace() << std::endl;
	}
	std::cerr << "Program exited with status " << exit_status << std::endl;
	sc_stop();
	if(!asynchronous)
	{
		sc_process_handle h = sc_get_current_process_handle();
		switch(h.proc_kind())
// 		switch(sc_get_curr_simcontext()->get_curr_proc_info()->kind)
		{
			case SC_THREAD_PROC_: 
			case SC_CTHREAD_PROC_:
				sc_core::wait();
				break;
			default:
				break;
		}
	}
}

int Simulator::GetExitStatus() const
{
	return exit_status;
}

#ifdef WIN32
BOOL WINAPI Simulator::ConsoleCtrlHandler(DWORD dwCtrlType)
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
void Simulator::SigIntHandler(int signum)
{
	cerr << "Interrupted by Ctrl-C or SIGINT signal" << endl;
	unisim::kernel::service::Simulator::simulator->Stop(0, 0, true);
}
#endif
