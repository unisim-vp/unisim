/*
 *  Copyright (c) 2010, Commissariat a l'Energie Atomique (CEA) All rights
 *  reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *   endorse or promote products derived from this software without specific
 *   prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#include "simulator.hh"
#include <stdexcept>

bool debug_enabled;

using namespace std;

Simulator::Simulator(int argc, char **argv)
  : unisim::kernel::service::Simulator(argc, argv, Simulator::DefaultConfiguration)
  , cpu(0)
  , memory(0)
  , time(0)
  , host_time(0)
  , linux_os(0)
  , tee_memory_access_reporting(0)
  , nirq_signal("nIRQm")
  , nfiq_signal("nFIQm")
  , simulation_spent_time(0.0)
  , gdb_server(0)
  , inline_debugger(0)
  , debugger(0)
  , profiler(0)
  , enable_gdb_server(false)
  , param_enable_gdb_server(0)
  , enable_inline_debugger(false)
  , param_enable_inline_debugger(0)
  , il1_power_estimator(0)
  , dl1_power_estimator(0)
  , enable_power_estimation(false)
  , param_enable_power_estimation("enable-power-estimation", 0,
                                  enable_power_estimation,
                                  "Activate caches power estimation.")
  , formula_caches_total_dynamic_energy(0)
  , formula_caches_total_dynamic_power(0)
  , formula_caches_total_leakage_power(0)
  , formula_caches_total_power(0)
  , exit_status(0)
{
  cpu = new CPU("cpu");
  memory = new MEMORY("memory");
  time = new unisim::service::time::sc_time::ScTime("time");
  host_time = new unisim::service::time::host_time::HostTime("host-time");
  linux_os = new LINUX_OS("linux-os");

  param_enable_gdb_server = new unisim::kernel::service::Parameter<bool>(
      "enable-gdb-server", 0,
      enable_gdb_server,
      "Enable GDB server.");
  if ( enable_gdb_server )
    gdb_server = new GDB_SERVER("gdb-server");
  param_enable_inline_debugger = new unisim::kernel::service::Parameter<bool>(
      "enable-inline-debugger", 0,
      enable_inline_debugger,
      "Enable inline debugger.");
  if ( enable_inline_debugger )
    inline_debugger = new INLINE_DEBUGGER("inline-debugger");
  if ( enable_power_estimation )
  {
    il1_power_estimator = new POWER_ESTIMATOR("il1-power-estimator");
    dl1_power_estimator = new POWER_ESTIMATOR("dl1-power-estimator");
    formula_caches_total_dynamic_energy =
        new unisim::kernel::service::Formula<double> (
            "caches-total-dynamic-energy", 0,
            unisim::kernel::service::Formula<double>::OP_ADD,
            &(*dl1_power_estimator)["dynamic-energy"],
            &(*il1_power_estimator)["dynamic-energy"],
            0,
            "caches total dynamic energy (in J)");
    formula_caches_total_dynamic_power =
        new unisim::kernel::service::Formula<double> (
            "caches-total-dynamic-power", 0,
            unisim::kernel::service::Formula<double>::OP_ADD,
            &(*dl1_power_estimator)["dynamic-power"],
            &(*il1_power_estimator)["dynamic-power"],
            0,
            "caches total dynamic power (in J)");
    formula_caches_total_leakage_power =
        new unisim::kernel::service::Formula<double> (
            "caches-total-leakage-power", 0,
            unisim::kernel::service::Formula<double>::OP_ADD,
            &(*dl1_power_estimator)["leakage-power"],
            &(*il1_power_estimator)["leakage-power"],
            0,
            "caches total leakage power (in J)");
    formula_caches_total_power =
        new unisim::kernel::service::Formula<double> (
            "caches-total-power", 0,
            unisim::kernel::service::Formula<double>::OP_ADD,
            formula_caches_total_dynamic_power,
            formula_caches_total_leakage_power,
            0,
            "caches total (dynamic+leakage) power (in J)");
  }

	//  - debugger
	debugger = new DEBUGGER("debugger");
	//  - profiler
	profiler = enable_inline_debugger ? new PROFILER("profiler") : 0;
	//  - Tee Memory Access Reporting
	tee_memory_access_reporting = 
		(enable_gdb_server || enable_inline_debugger) ?
			new TEE_MEMORY_ACCESS_REPORTING("tee-memory-access-reporting") :
			0;
  
  
  nfiq_signal = true; 
  nirq_signal = true; 
  
  // In Linux mode, the system is not entirely simulated.
  // This mode allows to run Linux applications without simulating all the peripherals.

  cpu->master_socket(memory->slave_sock);
  cpu->nirq( nirq_signal );
  cpu->nfiq( nfiq_signal );

  // Connect debugger to CPU
  cpu->debug_control_import >> debugger->debug_control_export;
  cpu->instruction_counter_trap_reporting_import >> debugger->trap_reporting_export;
  cpu->symbol_table_lookup_import >> debugger->symbol_table_lookup_export;
  debugger->disasm_import >> cpu->disasm_export;
  debugger->memory_import >> cpu->memory_export;
  debugger->registers_import >> cpu->registers_export;
  debugger->blob_import >> linux_os->blob_export_;

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

  // Connect everything
  cpu->linux_os_import >> linux_os->linux_os_export_;
  linux_os->memory_import_ >> cpu->memory_export;
  linux_os->memory_injection_import_ >> cpu->memory_injection_export;
  linux_os->registers_import_ >> cpu->registers_export;
  // connecting power estimator
  if ( enable_power_estimation )
  {
    cpu->icache.power_estimator_import >> il1_power_estimator->power_estimator_export;
    cpu->icache.power_mode_import >> il1_power_estimator->power_mode_export;
    cpu->dcache.power_estimator_import >> dl1_power_estimator->power_estimator_export;
    cpu->dcache.power_mode_import >> dl1_power_estimator->power_mode_export;

    il1_power_estimator->time_import >> time->time_export;
    dl1_power_estimator->time_import >> time->time_export;
  }
}

Simulator::~Simulator()
{
  if ( cpu ) delete cpu;
  if ( memory ) delete memory;
  if ( time ) delete time;
  if ( host_time ) delete host_time;
  if ( linux_os ) delete linux_os;
  if ( param_enable_gdb_server ) delete param_enable_gdb_server;
  if ( gdb_server ) delete gdb_server;
  if ( param_enable_inline_debugger ) delete param_enable_inline_debugger;
  if ( inline_debugger ) delete inline_debugger;
  if ( debugger ) delete debugger;
  if ( profiler ) delete profiler;
  if ( tee_memory_access_reporting ) delete tee_memory_access_reporting;
  if ( il1_power_estimator ) delete il1_power_estimator;
  if ( dl1_power_estimator ) delete dl1_power_estimator;
}

int
Simulator ::
Run()
{
  if ( unlikely(SimulationFinished()) ) return 0;

  double time_start = host_time->GetTime();

#ifndef WIN32
  void (*prev_sig_int_handler)(int) = 0;
#endif
  
  if ( ! inline_debugger )
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

  if ( !inline_debugger )
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
  simulation_spent_time += spent_time;

  cerr << "Simulation run-time parameters:" << endl;
  DumpParameters(cerr);
  cerr << endl;
  cerr << "Simulation formulas:" << endl;
  DumpFormulas(cerr);
  cerr << endl;
  cerr << "Simulation statistics:" << endl;
  DumpStatistics(cerr);
  cerr << endl;

  cerr << "simulation time: " << simulation_spent_time << " seconds" << endl;
  cerr << "simulated time : " << sc_time_stamp().to_seconds() << " seconds (exactly " << sc_time_stamp() << ")" << endl;
  cerr << "host simulation speed: " << ((double) (*cpu)["instruction-counter"] / spent_time / 1000000.0) << " MIPS" << endl;
  cerr << "time dilatation: " << spent_time / sc_time_stamp().to_seconds() << " times slower than target machine" << endl;

  return exit_status;
}

int
Simulator ::
Run(double time, sc_time_unit unit)
{
  if ( unlikely(SimulationFinished()) ) return 0;

  double time_start = host_time->GetTime();

#ifndef WIN32
  void (*prev_sig_int_handler)(int) = 0;
#endif

  if ( ! inline_debugger )
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
    sc_start(time, unit);
  }
  catch(std::runtime_error& e)
  {
    cerr << "FATAL ERROR! an abnormal error occured during simulation. Bailing out..." << endl;
    cerr << e.what() << endl;
  }

  if ( !inline_debugger )
  {
#ifdef WIN32
    SetConsoleCtrlHandler(&Simulator::ConsoleCtrlHandler, FALSE);
#else
    signal(SIGINT, prev_sig_int_handler);
#endif
  }

  double time_stop = host_time->GetTime();
  double spent_time = time_stop - time_start;
  simulation_spent_time += spent_time;

  cerr << "Simulation statistics:" << endl;
  DumpStatistics(cerr);
  cerr << endl;

  return exit_status;
}

bool
Simulator ::
IsRunning() const
{
  return sc_is_running();
}

bool
Simulator ::
SimulationStarted() const
{
  return sc_start_of_simulation_invoked();
}

bool
Simulator ::
SimulationFinished() const
{
  return sc_end_of_simulation_invoked();
}

unisim::kernel::service::Simulator::SetupStatus Simulator::Setup()
{
	if(enable_inline_debugger)
	{
		SetVariable("debugger.parse-dwarf", true);
	}
	
	// Build the Linux OS arguments from the command line arguments
	
	unisim::kernel::service::VariableBase *cmd_args = FindVariable("cmd-args");
	unsigned int cmd_args_length = cmd_args->GetLength();
	if(cmd_args_length > 0)
	{
		SetVariable("linux-os.binary", ((string)(*cmd_args)[0]).c_str());
		SetVariable("linux-os.argc", cmd_args_length);
		
		unsigned int i;
		for(i = 0; i < cmd_args_length; i++)
		{
			std::stringstream sstr;
			sstr << "linux-os.argv[" << i << "]";
			SetVariable(sstr.str().c_str(), ((string)(*cmd_args)[i]).c_str());
		}
	}

	unisim::kernel::service::Simulator::SetupStatus setup_status = unisim::kernel::service::Simulator::Setup();
	
	// inline-debugger and gdb-server are exclusive
	if(enable_inline_debugger && enable_gdb_server)
	{
		std::cerr << "ERROR! " << inline_debugger->GetName() << " and " << gdb_server->GetName() << " shall not be used together. Use " << param_enable_inline_debugger->GetName() << " and " << param_enable_gdb_server->GetName() << " to enable only one of the two" << std::endl;
		if(setup_status != unisim::kernel::service::Simulator::ST_OK_DONT_START)
		{
			setup_status = unisim::kernel::service::Simulator::ST_ERROR;
		}
	}
	
	return setup_status;
}

void Simulator::Stop(unisim::kernel::service::Object *object, int _exit_status, bool asynchronous)
{
	exit_status = _exit_status;
	if(object)
	{
		std::cerr << object->GetName() << " has requested simulation stop" << std::endl << std::endl;
	}
#ifdef DEBUG_ARMEMU
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

void
Simulator ::
DefaultConfiguration(unisim::kernel::service::Simulator *sim)
{
  sim->SetVariable("program-name", SIM_PROGRAM_NAME);
  sim->SetVariable("authors", SIM_AUTHOR);
  sim->SetVariable("version", SIM_VERSION);
  sim->SetVariable("copyright", SIM_COPYRIGHT);
  sim->SetVariable("license", SIM_LICENSE);
  sim->SetVariable("description", SIM_DESCRIPTION);
  sim->SetVariable("schematic", SIM_SCHEMATIC);


  sim->SetVariable("kernel_logger.std_err", true);
  sim->SetVariable("kernel_logger.std_err_color", true);

  sim->SetVariable("cpu.default-endianness",   "little-endian");
  sim->SetVariable("cpu.cpu-cycle-time",       "31250 ps"); // 32Mhz
  sim->SetVariable("cpu.bus-cycle-time",       "31250 ps"); // 32Mhz
  sim->SetVariable("cpu.icache.size",          0x020000); // 128 KB
  sim->SetVariable("cpu.dcache.size",          0x020000); // 128 KB
  sim->SetVariable("cpu.nice-time",            "1 ms"); // 1ms
  sim->SetVariable("cpu.ipc",                  1.0);
  sim->SetVariable("cpu.voltage",              1.8 * 1e3); // 1800 mV
  sim->SetVariable("cpu.enable-dmi",           true); // Enable SystemC TLM 2.0 DMI
  sim->SetVariable("memory.bytesize",          0xffffffffUL); 
  sim->SetVariable("memory.cycle-time",        "31250 ps");
  sim->SetVariable("memory.read-latency",      "31250 ps");
  sim->SetVariable("memory.write-latency",     "0 ps");
  sim->SetVariable("linux-os.system",          "arm-eabi");
  sim->SetVariable("linux-os.endianness",      "little-endian");
  sim->SetVariable("linux-os.memory-page-size",0x01000UL);
  sim->SetVariable("linux-os.stack-base",      0x40000000UL);
  sim->SetVariable("linux-os.envc",            0);
  sim->SetVariable("linux-os.utsname-sysname", "Linux");
  sim->SetVariable("linux-os.utsname-nodename","localhost");
  sim->SetVariable("linux-os.utsname-release", "2.6.27.35");
  sim->SetVariable("linux-os.utsname-version", "#UNISIM SMP Fri Mar 12 05:23:09 UTC 2010");
  sim->SetVariable("linux-os.utsname-machine", "armv5");
  sim->SetVariable("linux-os.utsname-domainname","localhost");
  sim->SetVariable("linux-os.apply-host-environment", false);
  sim->SetVariable("linux-os.hwcap", "swp half fastmult");

  sim->SetVariable("gdb-server.architecture-description-filename", "gdb_armv7l.xml");
  sim->SetVariable("debugger.parse-dwarf", false);
  sim->SetVariable("debugger.dwarf-register-number-mapping-filename", "arm_eabi_dwarf_register_number_mapping.xml");

  sim->SetVariable("inline-debugger.num-loaders", 1);
  sim->SetVariable("inline-debugger.search-path", "");
  sim->SetVariable("il1-power-estimator.cache-size", 32 * 128);
  sim->SetVariable("il1-power-estimator.line-size", 32);
  sim->SetVariable("il1-power-estimator.associativity", 4);
  sim->SetVariable("il1-power-estimator.rw-ports", 0);
  sim->SetVariable("il1-power-estimator.excl-read-ports", 1);
  sim->SetVariable("il1-power-estimator.excl-write-ports", 0);
  sim->SetVariable("il1-power-estimator.single-ended-read-ports", 0);
  sim->SetVariable("il1-power-estimator.banks", 1);
  sim->SetVariable("il1-power-estimator.tech-node", 130); // in nm
  sim->SetVariable("il1-power-estimator.output-width", 32 * 8);
  sim->SetVariable("il1-power-estimator.tag-width", 32); // to fix
  sim->SetVariable("il1-power-estimator.access-mode", "fast");
  sim->SetVariable("il1-power-estimator.verbose", false);

  sim->SetVariable("dl1-power-estimator.cache-size", 32 * 128);
  sim->SetVariable("dl1-power-estimator.line-size", 32);
  sim->SetVariable("dl1-power-estimator.associativity", 4);
  sim->SetVariable("dl1-power-estimator.rw-ports", 1);
  sim->SetVariable("dl1-power-estimator.excl-read-ports", 0);
  sim->SetVariable("dl1-power-estimator.excl-write-ports", 0);
  sim->SetVariable("dl1-power-estimator.single-ended-read-ports", 0);
  sim->SetVariable("dl1-power-estimator.banks", 1);
  sim->SetVariable("dl1-power-estimator.tech-node", 130); // in nm
  sim->SetVariable("dl1-power-estimator.output-width", 32 * 8);
  sim->SetVariable("dl1-power-estimator.tag-width", 32); // to fix
  sim->SetVariable("dl1-power-estimator.access-mode", "fast");
  sim->SetVariable("dl1-power-estimator.verbose", false);
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
