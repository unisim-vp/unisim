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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#include <simulator.hh>
#include <unisim/kernel/logger/logger_server.hh>
#include <unisim/service/debug/debugger/debugger.tcc>
#include <stdexcept>

bool debug_enabled;

using namespace std;

bool Simulator::enable_monitor = false;

Simulator::Simulator(int argc, char **argv, const sc_core::sc_module_name& name)
  : unisim::kernel::tlm2::Simulator(name, argc, argv, Simulator::DefaultConfiguration)
  , cpu("cpu", this)
  , memory("memory", this)
  , time("time")
  , host_time("host-time")
  , linux_os("linux-os")
  , simulation_spent_time(0.0)
  , debugger(0)
  , gdb_server(0)
  , inline_debugger(0)
  , monitor(0)
  , profiler(0)
  , http_server(0)
  , instrumenter(0)
  , enable_gdb_server(false)
  , param_enable_gdb_server("enable-gdb-server", 0,enable_gdb_server,"Enable GDB server.")
  , enable_inline_debugger(false)
  , param_enable_inline_debugger("enable-inline-debugger", 0, enable_inline_debugger, "Enable inline debugger.")
  , enable_profiler(false)
  , param_enable_profiler("enable-profiler", 0, enable_profiler, "Enable profiler.")
  , exit_status(0)
{
  param_enable_gdb_server.SetMutable(false);
  param_enable_inline_debugger.SetMutable(false);
  param_enable_profiler.SetMutable(false);

  if(enable_profiler)
  {
    this->SetVariable("HARDWARE.instrumenter.enable-user-interface", true); // When profiler is enabled, enable also instrumenter user interface so that profiler interface is periodically refreshed too
  }
  
  instrumenter = new INSTRUMENTER("instrumenter", this);
  http_server = new HTTP_SERVER("http-server");
  
  //  - Debug and Monitor
  if (enable_inline_debugger or enable_gdb_server or enable_monitor or enable_profiler)
    debugger = new DEBUGGER("debugger");
  if (enable_gdb_server)
    gdb_server = new GDB_SERVER("gdb-server");
  if (enable_inline_debugger)
    inline_debugger = new INLINE_DEBUGGER("inline-debugger");
  if (enable_monitor)
    monitor = new MONITOR("monitor");
  if (enable_profiler)
    profiler = new PROFILER("profiler");
  
  instrumenter->CreateSignal("nIRQm", true);
  instrumenter->CreateSignal("nFIQm", true);
  instrumenter->CreateSignal("nRESETm", true);
  
  instrumenter->RegisterPort(cpu.nIRQm);
  instrumenter->RegisterPort(cpu.nFIQm);
  instrumenter->RegisterPort(cpu.nRESETm);
  
  // In Linux mode, the system is not entirely simulated.
  // This mode allows to run Linux applications without simulating all the peripherals.
  cpu.master_socket( memory.slave_sock );
  instrumenter->Bind("HARDWARE.cpu.nIRQm", "HARDWARE.nIRQm");
  instrumenter->Bind("HARDWARE.cpu.nFIQm", "HARDWARE.nFIQm");
  instrumenter->Bind("HARDWARE.cpu.nRESETm", "HARDWARE.nRESETm");
  
  
  // CPU <-> Memory connections
  cpu.memory_import >> memory.memory_export;

  // CPU <-> LinuxOS connections
  cpu.linux_os_import >> linux_os.linux_os_export_;
  // cpu.symbol_table_lookup_import >> linux_os.symbol_table_lookup_export;
  linux_os.memory_import_ >> cpu.memory_export;
  linux_os.memory_injection_import_ >> cpu.memory_injection_export;
  linux_os.registers_import_ >> cpu.registers_export;
  

  if (debugger)
    {
      // Debugger <-> CPU connections
      cpu.debug_yielding_import                            >> *debugger->debug_yielding_export[0];
      cpu.trap_reporting_import                            >> *debugger->trap_reporting_export[0];
      cpu.memory_access_reporting_import                   >> *debugger->memory_access_reporting_export[0];
      *debugger->disasm_import[0]                          >> cpu.disasm_export;
      *debugger->memory_import[0]                          >> cpu.memory_export;
      *debugger->registers_import[0]                       >> cpu.registers_export;
      *debugger->memory_access_reporting_control_import[0] >> cpu.memory_access_reporting_control_export;
      
      // Debugger <-> Loader connections
      debugger->blob_import >> linux_os.blob_export_;
    }
  
  if (inline_debugger)
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
  
  if (gdb_server)
    {
      // gdb-server <-> debugger connections
      *debugger->debug_event_listener_import[1] >> gdb_server->debug_event_listener_export;
      *debugger->debug_yielding_import[1]       >> gdb_server->debug_yielding_export;
      gdb_server->debug_yielding_request_import >> *debugger->debug_yielding_request_export[1];
      gdb_server->debug_event_trigger_import    >> *debugger->debug_event_trigger_export[1];
      gdb_server->memory_import                 >> *debugger->memory_export[1];
      gdb_server->registers_import              >> *debugger->registers_export[1];
    }
  
  if (monitor)
    {
      // monitor <-> debugger connections
      *debugger->debug_event_listener_import[2] >> monitor->debug_event_listener_export;
      monitor->debug_event_trigger_import       >> *debugger->debug_event_trigger_export[2];
      monitor->memory_import                    >> *debugger->memory_export[2];
      monitor->registers_import                 >> *debugger->registers_export[2];
      monitor->stmt_lookup_import               >> *debugger->stmt_lookup_export[2];
      monitor->symbol_table_lookup_import       >> *debugger->symbol_table_lookup_export[2];
      monitor->backtrace_import                 >> *debugger->backtrace_export[2];
      monitor->debug_info_loading_import        >> *debugger->debug_info_loading_export[2];
      monitor->data_object_lookup_import        >> *debugger->data_object_lookup_export[2];
      monitor->subprogram_lookup_import         >> *debugger->subprogram_lookup_export[2];
    }
    
   if (profiler)
   {
      *debugger->debug_event_listener_import[3] >> profiler->debug_event_listener_export;
      *debugger->debug_yielding_import[3]       >> profiler->debug_yielding_export;
      profiler->debug_yielding_request_import   >> *debugger->debug_yielding_request_export[3];
      profiler->debug_event_trigger_import      >> *debugger->debug_event_trigger_export[3];
      profiler->disasm_import                   >> *debugger->disasm_export[3];
      profiler->memory_import                   >> *debugger->memory_export[3];
      profiler->registers_import                >> *debugger->registers_export[3];
      profiler->stmt_lookup_import              >> *debugger->stmt_lookup_export[3];
      profiler->symbol_table_lookup_import      >> *debugger->symbol_table_lookup_export[3];
      profiler->backtrace_import                >> *debugger->backtrace_export[3];
      profiler->debug_info_loading_import       >> *debugger->debug_info_loading_export[3];
      profiler->data_object_lookup_import       >> *debugger->data_object_lookup_export[3];
      profiler->subprogram_lookup_import        >> *debugger->subprogram_lookup_export[3];
   }
   
   *http_server->http_server_import[0] >> unisim::kernel::logger::Logger::StaticServerInstance()->http_server_export;
   *http_server->http_server_import[1] >> instrumenter->http_server_export;
   if (profiler)
   {
     *http_server->http_server_import[2] >> profiler->http_server_export;
   }
   
   *http_server->registers_import[0] >> cpu.registers_export;
}

Simulator::~Simulator()
{
  delete gdb_server;
  delete inline_debugger;
  delete monitor;
  delete profiler;
  delete debugger;
  delete http_server;
  delete instrumenter;
}

int
Simulator::Run()
{
  if ( unlikely(SimulationFinished()) ) return 0;

  double time_start = host_time.GetTime();

  sc_core::sc_report_handler::set_actions(sc_core::SC_INFO, sc_core::SC_DO_NOTHING); // disable SystemC messages
  
  try
  {
    sc_core::sc_start();
  }
  catch(std::runtime_error& e)
  {
    cerr << "FATAL ERROR! an abnormal error occured during simulation. Bailing out..." << endl;
    cerr << e.what() << endl;
  }

  cerr << "Simulation finished" << endl;

  double time_stop = host_time.GetTime();
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
  cerr << "simulated time : " << sc_core::sc_time_stamp().to_seconds() << " seconds (exactly " << sc_core::sc_time_stamp() << ")" << endl;
  cerr << "host simulation speed: " << ((double) cpu["instruction-counter"] / spent_time / 1000000.0) << " MIPS" << endl;
  cerr << "time dilatation: " << spent_time / sc_core::sc_time_stamp().to_seconds() << " times slower than target machine" << endl;

  if (profiler)
  {
    profiler->Output();
  }
  
  return exit_status;
}

int
Simulator::Run(double time, sc_core::sc_time_unit unit)
{
  if ( unlikely(SimulationFinished()) ) return 0;

  double time_start = host_time.GetTime();

  sc_core::sc_report_handler::set_actions(sc_core::SC_INFO, sc_core::SC_DO_NOTHING); // disable SystemC messages

  try
  {
    sc_core::sc_start(time, unit);
  }
  catch(std::runtime_error& e)
  {
    cerr << "FATAL ERROR! an abnormal error occured during simulation. Bailing out..." << endl;
    cerr << e.what() << endl;
  }

  double time_stop = host_time.GetTime();
  double spent_time = time_stop - time_start;
  simulation_spent_time += spent_time;

  cerr << "Simulation statistics:" << endl;
  DumpStatistics(cerr);
  cerr << endl;

  if (profiler)
  {
    profiler->Output();
  }
  
  return exit_status;
}

bool
Simulator::IsRunning() const
{
  return sc_core::sc_is_running();
}

bool
Simulator::SimulationStarted() const
{
  return sc_core::sc_start_of_simulation_invoked();
}

bool
Simulator::SimulationFinished() const
{
  return sc_core::sc_end_of_simulation_invoked();
}

unisim::kernel::Simulator::SetupStatus Simulator::Setup()
{
  if (enable_inline_debugger or enable_monitor)
    {
      SetVariable("debugger.parse-dwarf", true);
    }
  
  // Build the Linux OS arguments from the command line arguments
  std::vector<std::string> const& simargs = GetCmdArgs();
  if (not simargs.empty())
    {
      SetVariable("linux-os.binary", simargs[0].c_str());
      SetVariable("linux-os.argc", simargs.size());

      for (unsigned i = 0; i < simargs.size(); i++)
        {
          std::stringstream sstr;
          sstr << "linux-os.argv[" << i << "]";
          SetVariable(sstr.str().c_str(), simargs[i].c_str());
        }
    }
  
  unisim::kernel::Simulator::SetupStatus setup_status = unisim::kernel::Simulator::Setup();
  
  return setup_status;
}

bool Simulator::EndSetup()
{
  if (profiler)
  {
    http_server->AddJSAction(
      unisim::service::interfaces::ToolbarOpenTabAction(
        /* name */      profiler->GetName(), 
        /* label */     "<img src=\"/unisim/service/debug/profiler/icon_profile_cpu0.svg\">",
        /* tips */      std::string("Profile of ") + cpu.GetName(),
        /* tile */      unisim::service::interfaces::OpenTabAction::TOP_MIDDLE_TILE,
        /* uri */       profiler->URI()
    ));
  }
  
  return true;
}

void Simulator::Stop(unisim::kernel::Object *object, int _exit_status, bool asynchronous)
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
  sc_core::sc_stop();
  if(!asynchronous)
    {
      sc_core::sc_process_handle h = sc_core::sc_get_current_process_handle();
      switch(h.proc_kind())
        {
        case sc_core::SC_THREAD_PROC_: 
        case sc_core::SC_CTHREAD_PROC_:
          sc_core::wait();
          break;
        default:
          break;
        }
    }
}

void
Simulator::DefaultConfiguration(unisim::kernel::Simulator *sim)
{
  // meta information
  sim->SetVariable("program-name", "UNISIM ARMEMU");
  sim->SetVariable("copyright", "Copyright (C) 2017, Commissariat a l'Energie Atomique (CEA)");
  sim->SetVariable("license", "BSD (see file COPYING)");
  sim->SetVariable("authors", "Yves Lhuillier <yves.lhuillier@cea.fr>");
  sim->SetVariable("version", VERSION);
  sim->SetVariable("description", "UNISIM ARMEMU, ARMv7 generic simulator with linux emulation.");

  //=========================================================================
  //===                     Component run-time configuration              ===
  //=========================================================================

  sim->SetVariable("logger.std_err", true);
  sim->SetVariable("logger.std_err_color", true);

  sim->SetVariable("HARDWARE.cpu.default-endianness",   "little-endian");
  sim->SetVariable("HARDWARE.cpu.cpu-cycle-time",       "31250 ps"); // 32Mhz
  sim->SetVariable("HARDWARE.cpu.bus-cycle-time",       "31250 ps"); // 32Mhz
  sim->SetVariable("HARDWARE.cpu.icache.size",          0x020000); // 128 KB
  sim->SetVariable("HARDWARE.cpu.dcache.size",          0x020000); // 128 KB
  sim->SetVariable("HARDWARE.cpu.nice-time",            "1 ms"); // 1ms
  sim->SetVariable("HARDWARE.cpu.ipc",                  1.0);
  sim->SetVariable("HARDWARE.cpu.voltage",              1.8 * 1e3); // 1800 mV
  sim->SetVariable("HARDWARE.cpu.enable-dmi",           true); // Enable SystemC TLM 2.0 DMI
  sim->SetVariable("HARDWARE.memory.bytesize",          0xffffffffUL); 
  sim->SetVariable("HARDWARE.memory.cycle-time",        "31250 ps");
  sim->SetVariable("HARDWARE.memory.read-latency",      "31250 ps");
  sim->SetVariable("HARDWARE.memory.write-latency",     "0 ps");
  sim->SetVariable("linux-os.system",          "arm-eabi");
  sim->SetVariable("linux-os.endianness",      "little-endian");
  sim->SetVariable("linux-os.memory-page-size",0x01000UL);
  sim->SetVariable("linux-os.stack-base",      0x40000000UL);
  sim->SetVariable("linux-os.envc",            0);
  sim->SetVariable("linux-os.utsname-sysname", "Linux");
  sim->SetVariable("linux-os.utsname-nodename","localhost");
  sim->SetVariable("linux-os.utsname-release", "2.6.27.35");
  sim->SetVariable("linux-os.utsname-version", "#UNISIM SMP Fri Mar 12 05:23:09 UTC 2010");
  sim->SetVariable("linux-os.utsname-machine", "armv7");
  sim->SetVariable("linux-os.utsname-domainname","localhost");
  sim->SetVariable("linux-os.apply-host-environment", false);
  sim->SetVariable("linux-os.hwcap", "swp half fastmult");

  sim->SetVariable("gdb-server.architecture-description-filename", "unisim/service/debug/gdb_server/gdb_arm_with_neon.xml");
  sim->SetVariable("debugger.parse-dwarf", false);
  sim->SetVariable("debugger.dwarf-register-number-mapping-filename", "unisim/util/debug/dwarf/arm_eabi_dwarf_register_number_mapping.xml");

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
  
  sim->SetVariable("http-server.http-port", 12360);
}

void Simulator::SigInt()
{
  if(!inline_debugger)
  {
    unisim::kernel::Simulator::Instance()->Stop(0, 0, true);
  }
}

void Simulator::EnableMonitor(int (*_monitor_callback)(void))
{
	enable_monitor = true;
	unisim::service::debug::monitor::MonitorBase::SetCallback(_monitor_callback);
}
