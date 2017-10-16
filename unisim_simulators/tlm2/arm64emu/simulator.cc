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

#include <simulator.hh>
#include <unisim/component/tlm2/memory/ram/memory.tcc>
#include <unisim/service/debug/debugger/debugger.tcc>
#include <string>
#include <stdexcept>

template class unisim::component::tlm2::memory::ram::Memory<64, uint64_t, 8, 1024 * 1024, true>;

bool debug_enabled;

Simulator::Simulator(int argc, char **argv)
  : unisim::kernel::service::Simulator(argc, argv, Simulator::DefaultConfiguration)
  , cpu("cpu")
  , memory("memory")
  , time("time")
  , host_time("host-time")
  , linux_os("linux-os")
  , simulation_spent_time(0.0)
  , debugger(0)
  , gdb_server(0)
  , inline_debugger(0)
  , enable_gdb_server(false)
  , param_enable_gdb_server("enable-gdb-server", 0, enable_gdb_server, "Enable GDB server.")
  , enable_inline_debugger(false)
  , param_enable_inline_debugger("enable-inline-debugger", 0, enable_inline_debugger, "Enable inline debugger.")
  , exit_status(0)
{
  // - debugger
  if (enable_gdb_server or enable_inline_debugger)
    debugger = new DEBUGGER("debugger");
  if (enable_gdb_server)
    gdb_server = new GDB_SERVER("gdb-server");
  if (enable_inline_debugger)
    inline_debugger = new INLINE_DEBUGGER("inline-debugger");
  
  // In Linux mode, the system is not entirely simulated.
  // This mode allows to run Linux applications without simulating all the peripherals.

  cpu.master_socket(memory.slave_sock);
  
  // CPU <-> Memory connections                                                                                                                       
  cpu.memory_import >> memory.memory_export;

  // CPU <-> LinuxOS connections
  cpu.linux_os_import >> linux_os.linux_os_export_;
  // cpu.symbol_table_lookup_import >> linux_os.symbol_table_lookup_export;
  linux_os.memory_import_ >> cpu.memory_export;
  linux_os.memory_injection_import_ >> cpu.memory_injection_export;
  linux_os.registers_import_ >> cpu.registers_export;

  if (enable_gdb_server or enable_inline_debugger)
    {
      // Debugger <-> CPU connections
      cpu.debug_yielding_import                           >> *debugger->debug_yielding_export[0];
      cpu.trap_reporting_import                           >> *debugger->trap_reporting_export[0];
      cpu.memory_access_reporting_import                  >> *debugger->memory_access_reporting_export[0];
      *debugger->disasm_import[0]                          >> cpu.disasm_export;
      *debugger->memory_import[0]                          >> cpu.memory_export;
      *debugger->registers_import[0]                       >> cpu.registers_export;
      *debugger->memory_access_reporting_control_import[0] >> cpu.memory_access_reporting_control_export;
      
      // Debugger <-> LinuxOS connections
      debugger->blob_import >> linux_os.blob_export_;
    }
  
  if (enable_inline_debugger)
    {
      // inline-debugger <-> debugger connections
      *debugger->debug_event_listener_import[0]      >> inline_debugger->debug_event_listener_export;
      *debugger->debug_yielding_import[0]            >> inline_debugger->debug_yielding_export;
      inline_debugger->debug_yielding_request_import >> *debugger->debug_yielding_request_export[0];
      inline_debugger->debug_event_trigger_import    >> *debugger->debug_event_trigger_export[0];
      inline_debugger->disasm_import                 >> *debugger->disasm_export[0];
      inline_debugger->memory_import                 >> *debugger->memory_export[0];
      inline_debugger->registers_import              >> *debugger->registers_export[0];
      inline_debugger->stmt_lookup_import            >> *debugger->stmt_lookup_export[0];
      inline_debugger->symbol_table_lookup_import    >> *debugger->symbol_table_lookup_export[0];
      inline_debugger->backtrace_import              >> *debugger->backtrace_export[0];
      inline_debugger->debug_info_loading_import     >> *debugger->debug_info_loading_export[0];
      inline_debugger->data_object_lookup_import     >> *debugger->data_object_lookup_export[0];
      inline_debugger->subprogram_lookup_import      >> *debugger->subprogram_lookup_export[0];
    }
  
  if (enable_gdb_server)
    {
      // gdb-server <-> debugger connections
      *debugger->debug_event_listener_import[1] >> gdb_server->debug_event_listener_export;
      *debugger->debug_yielding_import[1]       >> gdb_server->debug_yielding_export;
      gdb_server->debug_yielding_request_import >> *debugger->debug_yielding_request_export[1];
      gdb_server->debug_event_trigger_import    >> *debugger->debug_event_trigger_export[1];
      gdb_server->memory_import                 >> *debugger->memory_export[1];
      gdb_server->registers_import              >> *debugger->registers_export[1];
    }
}

Simulator::~Simulator()
{
  delete debugger;
  delete gdb_server;
  delete inline_debugger;
}

int
Simulator::Run()
{
  if ( unlikely(SimulationFinished()) ) return 0;

  //  double time_start = host_time->GetTime();

  sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING); // disable SystemC messages
  
  try
    {
      sc_start();
    }
  catch(std::runtime_error& e)
    {
      std::cerr << "FATAL ERROR! an abnormal error occured during simulation. Bailing out..." << std::endl;
      std::cerr << e.what() << std::endl;
    }

  std::cerr << "Simulation finished" << std::endl;

  // double time_stop = host_time->GetTime();
  // double spent_time = time_stop - time_start;
  // simulation_spent_time += spent_time;

  // std::cerr << "Simulation run-time parameters:" << std::endl;
  // DumpParameters(std::cerr);
  // std::cerr << std::endl;
  // std::cerr << "Simulation formulas:" << std::endl;
  // DumpFormulas(std::cerr);
  // std::cerr << std::endl;
  // std::cerr << "Simulation statistics:" << std::endl;
  // DumpStatistics(std::cerr);
  // std::cerr << std::endl;

  // std::cerr << "simulation time: " << simulation_spent_time << " seconds" << std::endl;
  // std::cerr << "simulated time : " << sc_time_stamp().to_seconds() << " seconds (exactly " << sc_time_stamp() << ")" << std::endl;
  // std::cerr << "host simulation speed: " << ((double) (*cpu)["instruction-counter"] / spent_time / 1000000.0) << " MIPS" << std::endl;
  // std::cerr << "time dilatation: " << spent_time / sc_time_stamp().to_seconds() << " times slower than target machine" << std::endl;

  return exit_status;
}

int
Simulator::Run(double time, sc_time_unit unit)
{
  if ( unlikely(SimulationFinished()) ) return 0;

  // double time_start = host_time->GetTime();

  sc_report_handler::set_actions(SC_INFO, SC_DO_NOTHING); // disable SystemC messages

  try
    {
      sc_start(time, unit);
    }
  catch(std::runtime_error& e)
    {
      std::cerr << "FATAL ERROR! an abnormal error occured during simulation. Bailing out..." << std::endl;
      std::cerr << e.what() << std::endl;
    }

  // double time_stop = host_time->GetTime();
  // double spent_time = time_stop - time_start;
  // simulation_spent_time += spent_time;

  // std::cerr << "Simulation statistics:" << std::endl;
  // DumpStatistics(std::cerr);
  // std::cerr << std::endl;

  return exit_status;
}

bool
Simulator::IsRunning() const
{
  return sc_is_running();
}

bool
Simulator::SimulationStarted() const
{
  return sc_start_of_simulation_invoked();
}

bool
Simulator::SimulationFinished() const
{
  return sc_end_of_simulation_invoked();
}

unisim::kernel::service::Simulator::SetupStatus Simulator::Setup()
{
  if (enable_inline_debugger)
    {
      SetVariable("debugger.parse-dwarf", true);
    }
	
  // Build the Linux OS arguments from the command line arguments
	
  unisim::kernel::service::VariableBase *cmd_args = FindVariable("cmd-args");
  unsigned int cmd_args_length = cmd_args->GetLength();
  if (cmd_args_length > 0)
    {
      SetVariable("linux-os.binary", ((std::string)(*cmd_args)[0]).c_str());
      SetVariable("linux-os.argc", cmd_args_length);
		
      unsigned int i;
      for(i = 0; i < cmd_args_length; i++)
        {
          std::stringstream sstr;
          sstr << "linux-os.argv[" << i << "]";
          SetVariable(sstr.str().c_str(), ((std::string)(*cmd_args)[i]).c_str());
        }
    }

  unisim::kernel::service::Simulator::SetupStatus setup_status = unisim::kernel::service::Simulator::Setup();
	
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
  std::cerr << unisim::util::backtrace::BackTrace() << std::endl;
#endif
  std::cerr << "Program exited with status " << exit_status << std::endl;
  sc_stop();
  if(!asynchronous)
    {
      sc_process_handle h = sc_get_current_process_handle();
      switch(h.proc_kind())
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

void
Simulator::DefaultConfiguration(unisim::kernel::service::Simulator *sim)
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
  sim->SetVariable("linux-os.utsname-release", "3.14.43-unisim");
  sim->SetVariable("linux-os.utsname-version", "#UNISIM SMP Fri Mar 12 05:23:09 UTC 2010");
  sim->SetVariable("linux-os.utsname-machine", "armv7");
  sim->SetVariable("linux-os.utsname-domainname","localhost");
  sim->SetVariable("linux-os.apply-host-environment", false);
  sim->SetVariable("linux-os.hwcap", "swp half fastmult");

  sim->SetVariable("gdb-server.architecture-description-filename", "gdb_arm_with_neon.xml");
  sim->SetVariable("debugger.parse-dwarf", false);
  sim->SetVariable("debugger.dwarf-register-number-mapping-filename", "aarch64_eabi_dwarf_register_number_mapping.xml");

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
      std::cerr << "Interrupted by Ctrl-C" << std::endl;
      stop = true;
      break;
    case CTRL_BREAK_EVENT:
      std::cerr << "Interrupted by Ctrl-Break" << std::endl;
      stop = true;
      break;
    case CTRL_CLOSE_EVENT:
      std::cerr << "Interrupted by a console close" << std::endl;
      stop = true;
      break;
    case CTRL_LOGOFF_EVENT:
      std::cerr << "Interrupted because of logoff" << std::endl;
      stop = true;
      break;
    case CTRL_SHUTDOWN_EVENT:
      std::cerr << "Interrupted because of shutdown" << std::endl;
      stop = true;
      break;
    }
  if(stop) sc_stop();
  return stop ? TRUE : FALSE;
}
#else
void Simulator::SigIntHandler(int signum)
{
  std::cerr << "Interrupted by Ctrl-C or SIGINT signal" << std::endl;
  unisim::kernel::service::Simulator::simulator->Stop(0, 0, true);
}
#endif

