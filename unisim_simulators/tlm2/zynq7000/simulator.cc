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

#include <unisim/component/tlm2/interconnect/generic_router/router.hh>
#include <unisim/component/tlm2/interconnect/generic_router/router.tcc>
#include <simulator.hh>
#include <stdexcept>
#include <iostream>
#include <inttypes.h>

void
ZynqRouter::set_abs_mapping( unsigned output_port, uint64_t range_start, uint64_t range_end )
{
  this->mapping[output_port].used = true;
  this->mapping[output_port].range_start = range_start;
  this->mapping[output_port].range_end =   range_end;
  this->mapping[output_port].output_port = output_port;
  this->mapping[output_port].translation = range_start;
}

void
ZynqRouter::set_rel_mapping( unsigned output_port, uint64_t range_start, uint64_t range_end )
{
  this->mapping[output_port].used = true;
  this->mapping[output_port].range_start = range_start;
  this->mapping[output_port].range_end =   range_end;
  this->mapping[output_port].output_port = output_port;
  this->mapping[output_port].translation = 0;
}

ZynqRouter::ZynqRouter(const char* name, unisim::kernel::service::Object* parent)
  : unisim::kernel::service::Object( name, parent )
  , unisim::component::tlm2::interconnect::generic_router::Router<ZynqRouterConfig>( name, parent )
{
  this->set_abs_mapping( 0, 0x00000000, 0x3fffffff ); /* Main OCM RAM */
  this->set_abs_mapping( 1, 0xffff0000, 0xffffffff ); /* Boot OCM ROM */
  this->set_rel_mapping( 2, 0xf8f01000, 0xf8f01fff ); /* GIC */
}

/**
 * Constructor.
 * 
 * @param name the name of the module
 * @param parent the parent service
 */
GIC::GIC(const sc_module_name& name, unisim::kernel::service::Object* parent)
  : unisim::kernel::service::Object( name, parent )
  , sc_module(name)
  , unisim::kernel::service::Client<unisim::service::interfaces::TrapReporting>(name,parent)
  , dist_sock("dist_sock")
  , trap_reporting_import("trap-reporting-import", this)
{
  dist_sock( *this );
}

unsigned int GIC::transport_dbg(tlm::tlm_generic_payload& payload) { throw 0; return 0; }

tlm::tlm_sync_enum
GIC::nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
  if (phase != tlm::BEGIN_REQ) { throw 0; }
  
  this->b_transport(payload, t);
  
  return tlm::TLM_COMPLETED;
}


GIC::Reg&
GIC::GetRegister( uint32_t addr, unsigned size )
{
  static struct : public Reg
  {
    bool Read( GIC&, Data const& d ) { d << uint32_t(0); return true; }
    bool Write( GIC&, Data const& d ) { return true; }
  } RAZ_WI;
  
  if (((addr|size) & (size-1)) == 0) {
    if (size == 4) {
      if      ((addr & -0x400) == 0x800) {
        // GICD_ITARGETSRn
        return RAZ_WI;
      }
      
      if ((addr & -0x100) == 0xc00) {
        static struct : public Reg {
          uint32_t& RV( GIC& gic, uint32_t addr ) const {
            unsigned idx = ((addr & 0xff) >> 2);
            if (idx < gicd_icfgr_count) return gic.d_icfgr[idx];
            static uint32_t RAZ_RI;
            return RAZ_RI = 0;
          }
          bool  Read( GIC& gic, uint32_t addr, Data const& d ) { d << RV(gic,addr); return true; }
          bool Write( GIC& gic, uint32_t addr, Data const& d ) { d >> RV(gic,addr); return true; }
        } d32_icfgr;
        return d32_icfgr;
      }
      
      struct : public Reg
      {
        static int regidx(uint32_t addr) {
          switch (addr) {
          case 0: return GICD_CTLR; 
          }
          return -1;
        }
        bool  Read( GIC& gic, uint32_t addr, Data const& d ) { d << (gic.r32[regidx(addr)]); return true; }
        bool Write( GIC& gic, uint32_t addr, Data const& d ) { d >> (gic.r32[regidx(addr)]); return true; }
      } r32_access;
      
      if (r32_access.regidx(addr) >= 0)
        return r32_access;
  
      switch (addr) {
      case 4: {
        static struct : public Reg {
          bool Read( GIC& gic, Data const& d ) { d << uint32_t( gic.ITLinesNumber ); return true; }
        } GICD_TYPER;
        return GICD_TYPER;
      } break;
      }
    }
  }
  
  std::cerr << "Illegal GIC register address: [" << std::hex << addr << "," << std::dec << size << "].\n";
  static Reg error_reg;
  return error_reg;
}

void
GIC::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
{
  payload.set_dmi_allowed( false );
  
  uint32_t addr = payload.get_address();
  uint8_t* data_ptr = payload.get_data_ptr();
  unsigned data_length = payload.get_data_length();
  
  if (uint8_t* byte_enable_ptr = payload.get_byte_enable_ptr())
    {
      for (int idx = int(payload.get_byte_enable_length()); --idx >= 0; )
        if (not byte_enable_ptr[idx])
          throw 0;
    }
  // unsigned int streaming_width = payload.get_streaming_width();
  bool status = true;
  
  switch (payload.get_command())
    {
    case tlm::TLM_READ_COMMAND:  status = GetRegister( addr, data_length ) .Read( *this, addr, Data( data_ptr ) ); break;
    case tlm::TLM_WRITE_COMMAND: status = GetRegister( addr, data_length ).Write( *this, addr, Data( data_ptr ) ); break;
    case tlm::TLM_IGNORE_COMMAND: break;
    default: throw 0;
    }
  
  tlm::tlm_response_status resp_status = status ? tlm::TLM_OK_RESPONSE : tlm::TLM_ADDRESS_ERROR_RESPONSE;
  payload.set_response_status( resp_status );
}

PERIPH::PERIPH(const sc_module_name& name, unisim::kernel::service::Object* parent)
  : unisim::kernel::service::Object( name, parent )
  , sc_module(name)
  , unisim::kernel::service::Client<unisim::service::interfaces::TrapReporting>(name,parent)
  , slave_sock("slave_sock")
  , trap_reporting_import("trap-reporting-import", this)
{
  slave_sock(*this);
}

bool
PERIPH::get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data)
{
  throw 0;
  return false;
}

unsigned int
PERIPH::transport_dbg(tlm::tlm_generic_payload& payload)
{
  throw 0;
  return 0;
}

tlm::tlm_sync_enum
PERIPH::nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
  if (phase != tlm::BEGIN_REQ) { throw 0; }
  
  this->b_transport(payload, t);
  
  return tlm::TLM_COMPLETED;
}

void
PERIPH::Read( uint32_t addr, uint8_t* data, unsigned size )
{
  if (trap_reporting_import)
    trap_reporting_import->ReportTrap( *this );
}
void
PERIPH::Write( uint32_t addr, uint8_t const* data, unsigned size )
{
  if (trap_reporting_import)
    trap_reporting_import->ReportTrap( *this );
}

void
PERIPH::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t)
{
  payload.set_dmi_allowed( false );
  
  uint32_t addr = payload.get_address();
  uint8_t* data_ptr = payload.get_data_ptr();
  unsigned data_length = payload.get_data_length();
  
  if (uint8_t* byte_enable_ptr = payload.get_byte_enable_ptr())
    {
      for (int idx = int(payload.get_byte_enable_length()); --idx >= 0; )
        if (not byte_enable_ptr[idx])
          throw 0;
    }
  // unsigned int streaming_width = payload.get_streaming_width();
  // bool status = false;
  
  tlm::tlm_command cmd = payload.get_command();
  switch (cmd)
    {
    case tlm::TLM_READ_COMMAND: Read( addr, data_ptr, data_length ); break;
    case tlm::TLM_WRITE_COMMAND: Write( addr, data_ptr, data_length ); break;
    case tlm::TLM_IGNORE_COMMAND: break;
    default: throw 0;
    }
  
  payload.set_response_status(tlm::TLM_OK_RESPONSE);
}

Simulator::Simulator(int argc, char **argv)
  : unisim::kernel::service::Simulator(argc, argv, Simulator::DefaultConfiguration)
  , cpu( "cpu" )
  , router( "router" )
  , gic( "gic" )
  , main_ram( "main_ram" )
  , boot_rom( "boot_rom" )
  , nirq_signal("nIRQm")
  , nfiq_signal("nFIQm")
  , nrst_signal("nRESETm")
  , time("time")
  , host_time("host-time")
  // , linux_os(0)
  , loader("loader")
  , tee_memory_access_reporting(0)
  , simulation_spent_time(0.0)
  , gdb_server(0)
  , inline_debugger(0)
  , debugger(0)
  , profiler(0)
  , enable_gdb_server(false)
  , param_enable_gdb_server( "enable-gdb-server", 0, enable_gdb_server, "Enable GDB server." )
  , enable_inline_debugger(false)
  , param_enable_inline_debugger( "enable-inline-debugger", 0, enable_inline_debugger, "Enable inline debugger." )
  , exit_status(0)
{
  //linux_os = new LINUX_OS("linux-os");

  if (enable_gdb_server)
    gdb_server = new GDB_SERVER("gdb-server");
  
  if (enable_inline_debugger)
    inline_debugger = new INLINE_DEBUGGER( "inline-debugger" );
  
  // - debugger
  debugger = new DEBUGGER( "debugger" );
  // - profiler
  profiler = enable_inline_debugger ? new PROFILER( "profiler" ) : 0;
  // - Tee Memory Access Reporting
  tee_memory_access_reporting = (enable_gdb_server or enable_inline_debugger) ? new TEE_MEMORY_ACCESS_REPORTING("tee-memory-access-reporting") : 0;
  
  // In Linux mode, the system is not entirely simulated.
  // This mode allows to run Linux applications without simulating all the peripherals.

  nfiq_signal = true; 
  nirq_signal = true; 
  nrst_signal = true;
  
  cpu.master_socket( *router.targ_socket[0] );
  cpu.nIRQm( nirq_signal );
  cpu.nFIQm( nfiq_signal );
  cpu.nRESETm( nrst_signal );

  (*router.init_socket[0])( main_ram.slave_sock );
  (*router.init_socket[1])( boot_rom.slave_sock );
  (*router.init_socket[2])( gic.dist_sock );
  
  // Connect debugger to CPU
  cpu.debug_control_import >> debugger->debug_control_export;
  cpu.instruction_counter_trap_reporting_import >> debugger->trap_reporting_export;
  gic.trap_reporting_import >> debugger->trap_reporting_export;
  //cpu.symbol_table_lookup_import >> debugger->symbol_table_lookup_export;
	cpu.symbol_table_lookup_import >> loader.symbol_table_lookup_export;
  debugger->disasm_import >> cpu.disasm_export;
  debugger->memory_import >> cpu.memory_export;
  *loader.memory_import[0] >> main_ram.memory_export;
  *loader.memory_import[1] >> boot_rom.memory_export;
  debugger->registers_import >> cpu.registers_export;
  // debugger->blob_import >> linux_os->blob_export_;
  debugger->loader_import >> loader.loader_export;
  debugger->blob_import >> loader.blob_export;

  if(enable_inline_debugger)
    {
      // Connect tee-memory-access-reporting to CPU, debugger and profiler
      cpu.memory_access_reporting_import >> tee_memory_access_reporting->in;
      *tee_memory_access_reporting->out[0] >> profiler->memory_access_reporting_export;
      *tee_memory_access_reporting->out[1] >> debugger->memory_access_reporting_export;
      profiler->memory_access_reporting_control_import >> *tee_memory_access_reporting->in_control[0];
      debugger->memory_access_reporting_control_import >> *tee_memory_access_reporting->in_control[1];
      tee_memory_access_reporting->out_control >> cpu.memory_access_reporting_control_export;
    }
  else
    {
      cpu.memory_access_reporting_import >> debugger->memory_access_reporting_export;
      debugger->memory_access_reporting_control_import >> cpu.memory_access_reporting_control_export;
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
  // cpu.linux_os_import >> linux_os->linux_os_export_;
  // linux_os->memory_import_ >> cpu.memory_export;
  // linux_os->memory_injection_import_ >> cpu.memory_injection_export;
  // linux_os->registers_import_ >> cpu.registers_export;
}

Simulator::~Simulator()
{
  // delete linux_os;
  delete gdb_server;
  delete inline_debugger;
  delete debugger;
  delete profiler;
  delete tee_memory_access_reporting;
}

int
Simulator ::
Run()
{
  if ( unlikely(SimulationFinished()) ) return 0;

  double time_start = host_time.GetTime();

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
  cerr << "simulated time : " << sc_time_stamp().to_seconds() << " seconds (exactly " << sc_time_stamp() << ")" << endl;
  cerr << "host simulation speed: " << ((double) cpu["instruction-counter"] / spent_time / 1000000.0) << " MIPS" << endl;
  cerr << "time dilatation: " << spent_time / sc_time_stamp().to_seconds() << " times slower than target machine" << endl;

  return exit_status;
}

int
Simulator ::
Run(double time, sc_time_unit unit)
{
  if ( unlikely(SimulationFinished()) ) return 0;

  double time_start = host_time.GetTime();

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

  double time_stop = host_time.GetTime();
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
  
  // Build the Loader arguments from the command line arguments
  
  unisim::kernel::service::VariableBase *cmd_args = FindVariable("cmd-args");
  unsigned int cmd_args_length = cmd_args->GetLength();
  if(cmd_args_length > 0)
    {
      SetVariable( "loader.filename", ((std::string)(*cmd_args)[0]).c_str() );
    }

  unisim::kernel::service::Simulator::SetupStatus setup_status = unisim::kernel::service::Simulator::Setup();
  
  // inline-debugger and gdb-server are exclusive
  if(enable_inline_debugger && enable_gdb_server)
    {
      std::cerr << "ERROR! " << inline_debugger->GetName() << " and " << gdb_server->GetName()
                << " shall not be used together. Use " << param_enable_inline_debugger.GetName()
                << " and " << param_enable_gdb_server.GetName() << " to enable only one of the two" << std::endl;
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
  sim->SetVariable( "program-name", SIM_PROGRAM_NAME );
  sim->SetVariable( "authors", SIM_AUTHOR );
  sim->SetVariable( "version", SIM_VERSION );
  sim->SetVariable( "copyright", SIM_COPYRIGHT );
  sim->SetVariable( "license", SIM_LICENSE );
  sim->SetVariable( "description", SIM_DESCRIPTION );
  sim->SetVariable( "schematic", SIM_SCHEMATIC );


  sim->SetVariable( "kernel_logger.std_err", true );
  sim->SetVariable( "kernel_logger.std_err_color", true );
  
  sim->SetVariable( "router.cycle_time",        "10 ns" );
  
  sim->SetVariable( "cpu.SCTLR",                0x00c52078 );
  sim->SetVariable( "cpu.default-endianness",   "little-endian" );
  sim->SetVariable( "cpu.cpu-cycle-time",       "10 ns" ); // 32Mhz
  sim->SetVariable( "cpu.bus-cycle-time",       "10 ns" ); // 32Mhz
  sim->SetVariable( "cpu.icache.size",          0x020000 ); // 128 KB
  sim->SetVariable( "cpu.dcache.size",          0x020000 ); // 128 KB
  sim->SetVariable( "cpu.nice-time",            "1 us" ); // 1us
  sim->SetVariable( "cpu.ipc",                  1.0  );
  sim->SetVariable( "cpu.voltage",              1.8 * 1e3 ); // 1800 mV
  sim->SetVariable( "cpu.enable-dmi",           true ); // Enable SystemC TLM 2.0 DMI
  sim->SetVariable( "cpu.verbose",              true );
  sim->SetVariable( "cpu.verbose-tlm",          false );
  sim->SetVariable( "main_ram.bytesize",          0x40000000UL ); 
  sim->SetVariable( "main_ram.cycle-time",        "10 ns" );
  sim->SetVariable( "main_ram.read-latency",      "10 ns" );
  sim->SetVariable( "main_ram.write-latency",     "0 ps" );
  sim->SetVariable( "boot_rom.org",               0xffff0000UL );
  sim->SetVariable( "boot_rom.bytesize",          0x00010000UL ); 
  sim->SetVariable( "boot_rom.cycle-time",        "10 ns" );
  sim->SetVariable( "boot_rom.read-latency",      "10 ns" );
  sim->SetVariable( "boot_rom.write-latency",     "0 ps" );
  sim->SetVariable( "loader.memory-mapper.mapping", "main_ram:0x00000000-0x3fffffff,boot_rom:0xffff0000-0xffffffff" );
  
  
  sim->SetVariable( "gdb-server.architecture-description-filename", "gdb_armv5l.xml" ); // Current Cross-GDBs doesn't natively recognize armv7...
  sim->SetVariable( "debugger.parse-dwarf", false );
  sim->SetVariable( "debugger.dwarf-register-number-mapping-filename", "arm_eabi_dwarf_register_number_mapping.xml" );

  sim->SetVariable( "inline-debugger.num-loaders", 1 );
  sim->SetVariable( "inline-debugger.search-path", "" );
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
