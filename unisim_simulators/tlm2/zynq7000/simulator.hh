/*
 *  Copyright (c) 2010,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifndef SIMULATOR_HH_
#define SIMULATOR_HH_

#include <unisim/component/tlm2/processor/arm/armemu/armemu.hh>
#include <unisim/component/tlm2/memory/ram/memory.hh>
#include <unisim/component/tlm2/interconnect/generic_router/router.hh>
#include <unisim/service/time/sc_time/time.hh>
#include <unisim/service/time/host_time/time.hh>
//#include <unisim/service/os/linux_os/linux.hh>
#include <unisim/service/loader/multiformat_loader/multiformat_loader.hh>
#include <unisim/service/trap_handler/trap_handler.hh>
#include <unisim/service/debug/gdb_server/gdb_server.hh>
#include <unisim/service/debug/inline_debugger/inline_debugger.hh>
#include <unisim/service/debug/debugger/debugger.hh>
#include <unisim/service/profiling/addr_profiler/profiler.hh>
#include <unisim/service/tee/memory_access_reporting/tee.hh>
#include <unisim/kernel/service/service.hh>
#include <unisim/util/likely/likely.hh>
#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <cstdlib>

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <signal.h>
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

struct ZynqRouterConfig
{
  typedef uint32_t ADDRESS;
  static unsigned const OUTPUT_PORTS = 6;
  static unsigned const INPUT_SOCKETS = 1;
  static unsigned const OUTPUT_SOCKETS = OUTPUT_PORTS;
  static unsigned const MAX_NUM_MAPPINGS = OUTPUT_PORTS;
  static unsigned const BUSWIDTH = 32;
  typedef tlm::tlm_base_protocol_types TYPES;
  static const bool VERBOSE = false;
};

struct ZynqRouter : public unisim::component::tlm2::interconnect::generic_router::Router<ZynqRouterConfig>
{
  ZynqRouter( char const* name, unisim::kernel::service::Object* parent = 0 );
  void relative_mapping( unsigned output_port, uint64_t range_start, uint64_t range_end, tlm::tlm_target_socket<32u>& sock );
  void absolute_mapping( unsigned output_port, uint64_t range_start, uint64_t range_end, tlm::tlm_target_socket<32u>& sock );
};

struct RegMap
  : public tlm::tlm_fw_transport_if<>
{
  static unsigned const BUSWIDTH = 32;
    
  tlm::tlm_target_socket<BUSWIDTH> socket;
  RegMap( char const* );
    
  struct Data
  {
    Data( uint8_t* _ptr ) : ptr( _ptr ) {} uint8_t* ptr;
    template <typename T>
    void Access( bool wnr, T& val ) const
    {
      T tmp(val);
      if (wnr)
        { for (int idx = sizeof(T); --idx >= 0;) tmp = (tmp << 8) | T(ptr[idx]); val = tmp; }
      else
        { for (int idx = 0, end = sizeof(T); idx < end; ++ idx, tmp >>= 8) ptr[idx] = tmp; }
    }
    void Access( bool wnr, uint8_t* bytes, unsigned size ) const
    {
      if (wnr)
        for (unsigned idx = 0; idx < size; ++idx) bytes[idx] = ptr[idx];
      else
        for (unsigned idx = 0; idx < size; ++idx) ptr[idx] = bytes[idx];
    }
    template <typename T>
    T Value() const
    {
      T val(0);
      Access( true, val );
      return val;
    }
  };
  
  /*** TLM2 Slave methods ***/
  bool               get_direct_mem_ptr(tlm::tlm_generic_payload& payload, tlm::tlm_dmi& dmi_data) { return false; }
  unsigned int       transport_dbg(tlm::tlm_generic_payload& payload);
  tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t);
  void               b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t);
  virtual bool       AccessRegister( bool wnr, uint32_t addr, unsigned size, Data const& d, sc_core::sc_time const& update_time ) = 0;

};

struct MPCore
  : public sc_module
  , public RegMap
  , public unisim::kernel::service::Client<unisim::service::interfaces::TrapReporting>
{
  //typedef tlm::tlm_base_protocol_types TYPES;
  MPCore(const sc_module_name& name, unisim::kernel::service::Object* parent = 0);

  bool AccessRegister( bool wnr, uint32_t addr, unsigned size, Data const& d, sc_core::sc_time const& update_time );
  bool _AccessRegister( bool wnr, uint32_t addr, unsigned size, Data const& d, sc_core::sc_time const& update_time );
  
  void SendInterrupt( unsigned idx );

  unisim::kernel::service::ServiceImport<unisim::service::interfaces::TrapReporting> trap_reporting_import;
  
  sc_core::sc_out<bool> nIRQ;
  sc_core::sc_out<bool> nFIQ;
  
  static unsigned const ITLinesNumber = 2;
  static unsigned const ITLinesCount = 32*(ITLinesNumber+1);
  uint32_t ICCICR; /* CPU Interface Control Register */
  uint32_t ICCPMR; /* Interrupt Priority Mask Register */
  uint32_t ICDDCR; /* Distributor Control Register */
  static unsigned const state32_count = (ITLinesNumber+1);
  uint32_t IENABLE[state32_count];
  uint32_t IPENDING[state32_count];
  uint32_t IACTIVE[state32_count];
  uint8_t  IPRIORITYR[ITLinesCount];
  uint8_t  ICDIPTR[ITLinesCount];
  static unsigned const icfgr_count = 2*(ITLinesNumber+1);
  uint32_t ICDICFR[ITLinesCount/16];
  
  sc_core::sc_event generate_exceptions_event;
  void GenerateExceptionsProcess();
  unsigned HighestPriorityPendingInterrupt( uint8_t required, uint8_t enough );
  uint32_t ReadGICC_IAR();
};

struct SLCR
  : public sc_module
  , public RegMap
  , public unisim::kernel::service::Client<unisim::service::interfaces::TrapReporting>
{
  SLCR( const sc_module_name& name, unisim::kernel::service::Object* parent = 0 );
  
  uint32_t ARM_PLL_CTRL, DDR_PLL_CTRL, IO_PLL_CTRL, ARM_CLK_CTRL, CLK_621_TRUE, UART_CLK_CTRL;
  
  bool AccessRegister( bool wnr, uint32_t addr, unsigned size, Data const& d, sc_core::sc_time const& update_time );
};

struct TTC
  : public sc_module
  , public RegMap
  , public unisim::kernel::service::Client<unisim::service::interfaces::TrapReporting>
{
  TTC( const sc_module_name& name, unisim::kernel::service::Object* parent, MPCore& _mpcore, unsigned _id, unsigned _base_it );
  
  MPCore& mpcore;
  unsigned id, base_it;
  
  uint32_t
    Clock_Control[3],
    Counter_Control[3],
    Counter_Value[3],
    Interval_Counter[3],
    Interrupt_Enable[3];
  
  bool AccessRegister( bool wnr, uint32_t addr, unsigned size, Data const& d, sc_core::sc_time const& update_time );
  
  sc_core::sc_event update_state_event;
  sc_core::sc_time  clock_period;
  sc_core::sc_time  last_state_update_time[3];
  int               it_lines[3];
  
  void UpdateStateProcess();
  void UpdateState( sc_core::sc_time const& update_time );
  void UpdateCounterState( unsigned idx, sc_core::sc_time const& update_time
 );
};

struct Simulator : public unisim::kernel::service::Simulator
{
  Simulator( int argc, char **argv );
  virtual ~Simulator();
  
  int Run();
  int Run(double time, sc_time_unit unit);
  bool IsRunning() const;
  bool SimulationStarted() const;
  bool SimulationFinished() const;
  virtual unisim::kernel::service::Simulator::SetupStatus Setup();
  virtual void Stop(unisim::kernel::service::Object *object, int exit_status, bool asynchronous = false);
  int GetExitStatus() const;

 private:
  static void DefaultConfiguration(unisim::kernel::service::Simulator *sim);
  typedef unisim::component::tlm2::processor::arm::armemu::ARMEMU CPU;
  typedef unisim::component::tlm2::memory::ram::Memory<32, uint32_t, 8, 1024 * 1024, true> MAIN_RAM;
  typedef unisim::component::tlm2::memory::ram::Memory<32, uint32_t, 8, 1024 * 1024, true> BOOT_ROM;
  
  //typedef unisim::service::os::linux_os::Linux<uint32_t, uint32_t> LINUX_OS;
  typedef unisim::service::loader::multiformat_loader::MultiFormatLoader<uint32_t> LOADER;
  
  typedef unisim::service::debug::gdb_server::GDBServer<uint32_t> GDB_SERVER;
  typedef unisim::service::debug::inline_debugger::InlineDebugger<uint32_t> INLINE_DEBUGGER;
  typedef unisim::service::debug::debugger::Debugger<uint32_t> DEBUGGER;
  typedef unisim::service::profiling::addr_profiler::Profiler<uint32_t> PROFILER;
  typedef unisim::service::tee::memory_access_reporting::Tee<uint32_t> TEE_MEMORY_ACCESS_REPORTING;
  typedef unisim::service::time::sc_time::ScTime ScTime;
  typedef unisim::service::time::host_time::HostTime HostTime;
  
  CPU                          cpu;
  ZynqRouter                   router;
  MPCore                       mpcore;
  MAIN_RAM                     main_ram;
  BOOT_ROM                     boot_rom;
  SLCR                         slcr;
  TTC                          ttc0;
  TTC                          ttc1;
  sc_signal<bool>              nirq_signal;
  sc_signal<bool>              nfiq_signal;
  sc_signal<bool>              nrst_signal;
  
  ScTime                       time;
  HostTime                     host_time;
  //LINUX_OS*                    linux_os;
  LOADER                       loader;
  TEE_MEMORY_ACCESS_REPORTING* tee_memory_access_reporting;

  double                       simulation_spent_time;

  GDB_SERVER*                  gdb_server;
  INLINE_DEBUGGER*             inline_debugger;
  DEBUGGER*                    debugger;
  PROFILER*                    profiler;
  bool                         enable_gdb_server;
  unisim::kernel::service::Parameter<bool> param_enable_gdb_server;
  bool                         enable_inline_debugger;
  unisim::kernel::service::Parameter<bool> param_enable_inline_debugger;

  int exit_status;
#ifdef WIN32
  static BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType);
#else
  static void SigIntHandler(int signum);
#endif
};

#endif /* SIMULATOR_HH_ */
