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

#include <unisim/component/tlm2/processor/arm/cortex_a9/cpu.hh>
#include <unisim/component/tlm2/memory/ram/memory.hh>
#include <unisim/component/tlm2/interconnect/generic_router/router.hh>
#include <unisim/service/time/sc_time/time.hh>
#include <unisim/service/time/host_time/time.hh>
#include <unisim/service/loader/multiformat_loader/multiformat_loader.hh>
#include <unisim/service/trap_handler/trap_handler.hh>
#include <unisim/service/telnet/telnet.hh>
#include <unisim/service/debug/gdb_server/gdb_server.hh>
#include <unisim/service/debug/inline_debugger/inline_debugger.hh>
#include <unisim/service/debug/profiler/profiler.hh>
#include <unisim/service/debug/debugger/debugger.hh>
#include <unisim/service/interfaces/char_io.hh>
#include <unisim/kernel/service/service.hh>
#include <unisim/util/cache/cache.hh>
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

struct MSSConfig
{
  enum STORAGE_ATTR
    {
      SA_DEFAULT = 0,           // not cache inhibited and not guarded                                                                                                              
      SA_I       = 1,           // cache inhibited                                                                                                                                  
      SA_G       = 2,           // guarded                                                                                                                                          
      SA_IG      = SA_I | SA_G, // cache inhibited and guarded                                                                                                                      
    };

  typedef uint32_t ADDRESS;
  typedef uint32_t PHYSICAL_ADDRESS;
};

struct CPU
  : public unisim::component::tlm2::processor::arm::cortex_a9::CPU
  , public unisim::util::cache::MemorySubSystem<MSSConfig, CPU>
{
  CPU(const sc_core::sc_module_name& name, Object* parent = 0)
    : unisim::kernel::service::Object(name, parent)
    , unisim::component::tlm2::processor::arm::cortex_a9::CPU(name, parent)
  {}
  
  struct ACCESS_CONTROLLER
  {
    template <bool debug>
    bool ControlAccess(unisim::util::cache::AccessControl<MSSConfig>& ac) { return true; }
  } access_controller;
  
  ACCESS_CONTROLLER* GetAccessController() { return &access_controller; }
  
  typedef CPU CACHE_CPU;
  
  struct L1D_CONFIG
  {
    typedef CACHE_CPU CPU;
    static const unsigned int SIZE                                      = 4096;
    static const unisim::util::cache::CacheWritingPolicy WRITING_POLICY = unisim::util::cache::CACHE_WRITE_THROUGH_AND_NO_WRITE_ALLOCATE_POLICY;
    static const unisim::util::cache::CacheType TYPE                    = unisim::util::cache::DATA_CACHE;
    static const unsigned int ASSOCIATIVITY                             = 4;
    static const unsigned int BLOCK_SIZE                                = 32;
    static const unsigned int BLOCKS_PER_LINE                           = 1;

    struct SET_STATUS : unisim::util::cache::CacheSetStatus
    {
      SET_STATUS() : victim_way(0), lockout(0) {}

      unsigned victim_way, lockout;
    };

    struct LINE_STATUS : unisim::util::cache::CacheLineStatus {};

    struct BLOCK_STATUS : unisim::util::cache::CacheBlockStatus {};
  };
  
  struct L1D : public unisim::util::cache::Cache<MSSConfig, L1D_CONFIG, L1D>
  {
    static const char *GetCacheName() { return "L1D"; }
  } l1d;
  L1D* GetCache(const L1D* ) ALWAYS_INLINE { return &l1d; }
  
  struct L1I_CONFIG
  {
    typedef CACHE_CPU CPU;
    static const unsigned int SIZE                                      = 4096;
    static const unisim::util::cache::CacheWritingPolicy WRITING_POLICY = unisim::util::cache::CACHE_WRITE_THROUGH_AND_NO_WRITE_ALLOCATE_POLICY;
    static const unisim::util::cache::CacheType TYPE                    = unisim::util::cache::DATA_CACHE;
    static const unsigned int ASSOCIATIVITY                             = 4;
    static const unsigned int BLOCK_SIZE                                = 32;
    static const unsigned int BLOCKS_PER_LINE                           = 1;

    struct SET_STATUS : unisim::util::cache::CacheSetStatus
    {
      SET_STATUS() : victim_way(0), lockout(0) {}

      unsigned victim_way, lockout;
    };

    struct LINE_STATUS : unisim::util::cache::CacheLineStatus {};

    struct BLOCK_STATUS : unisim::util::cache::CacheBlockStatus {};
  };
  
  struct L1I : public unisim::util::cache::Cache<MSSConfig, L1I_CONFIG, L1I>
  {
    static const char *GetCacheName() { return "L1I"; }
  } l1i;
  L1I* GetCache(const L1I* ) ALWAYS_INLINE { return &l1i; }

  
  typedef unisim::util::cache::LocalMemorySet<MSSConfig> DATA_LOCAL_MEMORIES;
  typedef unisim::util::cache::LocalMemorySet<MSSConfig> INSTRUCTION_LOCAL_MEMORIES;
  typedef unisim::util::cache::CacheHierarchy<MSSConfig, L1D> DATA_CACHE_HIERARCHY;
  typedef unisim::util::cache::CacheHierarchy<MSSConfig, L1I> INSTRUCTION_CACHE_HIERARCHY;

  // virtual bool PhysicalWriteMemory(uint32_t addr, const uint8_t* buffer, uint32_t size, uint32_t attrs) { return DataStore(addr, buffer, size); }
  // virtual bool PhysicalReadMemory(uint32_t addr, uint8_t* buffer, uint32_t size, uint32_t attrs) { return DataLoad(addr, buffer, size); }
	
  // virtual bool ExternalReadMemory(uint32_t addr, void* buffer, uint32_t size) { return DebugDataLoad(addr, buffer, size); }
  // virtual bool ExternalWriteMemory(uint32_t addr, void const* buffer, uint32_t size) { return DebugDataStore(addr, buffer, size); }
  
  // {
  //   typename TYPES::ADDRESS cur_addr = addr;
  //   unsigned int rem_size = size;
	
  //   do
  //     {
  //       // Access control
  //       AccessControl<TYPES> access_control;
  //       access_control.addr = cur_addr;
  //       access_control.mem_access_type = MAT_READ;
		
  //       typename MSS::ACCESS_CONTROLLER *access_controller = __MSS_GetAccessController__<typename MSS::ACCESS_CONTROLLER>();
		
  //       if(!access_controller->template ControlAccess</* debug */ false>(access_control)) return false;
		
  //       unsigned int cur_size = access_control.size_to_protection_boundary ? std::min(rem_size, access_control.size_to_protection_boundary) : rem_size;

  //       if(!LoadFromMSS<typename MSS::DATA_LOCAL_MEMORIES, typename MSS::DATA_CACHE_HIERARCHY, typename MSS::DATA_LOCAL_MEMORIES::LOC_MEM1, typename MSS::DATA_CACHE_HIERARCHY::L1CACHE>(access_control.phys_addr, buffer, cur_size, access_control.storage_attr)) return false;
	
  //       cur_addr += cur_size;
  //       rem_size -= cur_size;
  //     }
  //   while(unlikely(rem_size));
	
  //   if(unlikely(__MSS_IsVerboseDataLoad__())) Trace("Loading Data", addr, buffer, size);
	
  //   num_data_load_accesses++;
  //   num_data_load_xfered_bytes += size;
	
  //   return true;
  // }
};

struct ZynqRouterConfig
{
  typedef uint32_t ADDRESS;
  static unsigned const OUTPUT_PORTS = 9;
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

struct MMDevice
  : public sc_core::sc_module
  , public unisim::kernel::service::Client<unisim::service::interfaces::TrapReporting>
  , public tlm::tlm_fw_transport_if<>
{
  static unsigned const BUSWIDTH = 32;
    
  tlm::tlm_target_socket<BUSWIDTH> socket;
  unisim::kernel::service::ServiceImport<unisim::service::interfaces::TrapReporting> trap_reporting_import;
  bool verbose, hardfail;
  
  
  MMDevice( sc_core::sc_module_name const& name, unisim::kernel::service::Object* parent );
    
  struct Data
  {
    Data( uint8_t* _ptr, bool _wnr, unsigned _size )
      : ptr( _ptr ), wnr( _wnr ), size( _size ) {}
    uint8_t* ptr; bool wnr; unsigned size;
    template <typename T>
    void Access( T& val ) const
    {
      T tmp(val);
      if (wnr)
        { for (int idx = sizeof(T); --idx >= 0;) tmp = (tmp << 8) | T(ptr[idx]); val = tmp; }
      else
        { for (int idx = 0, end = sizeof(T); idx < end; ++ idx, tmp >>= 8) ptr[idx] = tmp; }
    }
    void Copy( uint8_t* bytes, unsigned size ) const
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
  /* MMDevice methods */
  void               DumpRegisterAccess( std::ostream& sink, uint32_t addr, Data const& d );
  virtual bool       AccessRegister( uint32_t addr, Data const& d, sc_core::sc_time const& update_time ) = 0;

};

struct MPCore : public MMDevice
{
  //typedef tlm::tlm_base_protocol_types TYPES;
  MPCore(sc_core::sc_module_name const& name, unisim::kernel::service::Object* parent = 0);

  bool AccessRegister( uint32_t addr, Data const& d, sc_core::sc_time const& update_time );
  
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
  
  sc_core::sc_event interrupt_line_events[ITLinesCount];
  template <unsigned IDX> void ITProcess() { ITProcess( IDX ); }
  void ITProcess( unsigned idx );
  
  sc_core::sc_event generate_exceptions_event;
  void GenerateExceptionsProcess();
  
  unsigned HighestPriorityPendingInterrupt( uint8_t required, uint8_t enough );
  uint32_t ReadGICC_IAR();
};

struct Simulator;

struct SLCR : public MMDevice
{
  SLCR( sc_core::sc_module_name const& name, Simulator& _simulator, unisim::kernel::service::Object* parent = 0 );
  
  Simulator& simulator;
  uint32_t ARM_PLL_CTRL, DDR_PLL_CTRL, IO_PLL_CTRL, ARM_CLK_CTRL, DDR_CLK_CTRL, CLK_621_TRUE, UART_CLK_CTRL;
  
  bool AccessRegister( uint32_t addr, Data const& d, sc_core::sc_time const& update_time );
};

struct TTC : public MMDevice
{
  TTC( sc_core::sc_module_name const& name, unisim::kernel::service::Object* parent, MPCore& _mpcore, unsigned _id, unsigned _base_it );
  
  MPCore& mpcore;
  unsigned id, base_it;
  
  uint32_t
    Clock_Control[3],
    Counter_Control[3],
    Counter_Value[3],
    Interval_Counter[3],
    Interrupt_Register[3],
    Interrupt_Enable[3];
  
  uint64_t
    update_counters[3];
  
  bool AccessRegister( uint32_t addr, Data const& d, sc_core::sc_time const& update_time );
  
  sc_core::sc_event update_state_event;
  sc_core::sc_time  clock_period;
  sc_core::sc_time  last_state_update_time[3];
  int               it_lines[3];
  
  void UpdateStateProcess();
  void UpdateState( sc_core::sc_time const& update_time );
  void UpdateCounterState( unsigned idx, sc_core::sc_time const& update_time );
};

struct PS_UART : public MMDevice, public unisim::kernel::service::Client<unisim::service::interfaces::CharIO>
{
  PS_UART( sc_core::sc_module_name const& name, unisim::kernel::service::Object* parent, MPCore& _mpcore, int _it_line );
  
  bool AccessRegister( uint32_t addr, Data const& d, sc_core::sc_time const& update_time );

  unisim::kernel::service::ServiceImport<unisim::service::interfaces::CharIO> char_io_import;
  sc_core::sc_event exchange_event;
  sc_core::sc_time  bit_period, last_rx;
  bool              rx_timeout_active;
  MPCore&           mpcore;
  int               it_line;
  
  struct FIFO {
    static unsigned const CAPACITY=64;
    FIFO() : head(0), size(0) {}
    uint8_t items[CAPACITY];
    unsigned head, size;
    void Push( uint8_t item ) { head = (head + CAPACITY - 1) % CAPACITY; size +=1; items[head] = item; }
    uint8_t Pull() { size -= 1; return items[(head+size)%CAPACITY]; }
    bool Full() const { return size >= CAPACITY; }
    bool NearlyFull() const { return size >= (CAPACITY-1); }
    bool Empty() const { return size == 0; }
    bool Trig( unsigned level ) { return level and (size >= level); }
    void Clear() { size = 0; }
  };
  
  FIFO TxFIFO;
  FIFO RxFIFO;
  unsigned CR, MR, IMR, ISR, BAUDGEN, RXTOUT, BDIV, TTRIG, RTRIG, FDEL;
  
  void PutChar( Data const& d );
  void GetChar( Data const& d );
  
  sc_dt::uint64 rx_timeout_ticks() const { return (RXTOUT*4+3); }
  void reload_rxtout( sc_core::sc_time const& );
  
  void ExchangeProcess();
};

struct L2C : public MMDevice
{
  L2C( sc_core::sc_module_name const& name, unisim::kernel::service::Object* parent );
  
  uint32_t reg1_control;
  
  typedef std::map<uint32_t,uint32_t> FastRegs;
  FastRegs fastregs;
  uint32_t& fastreg( uint32_t addr, uint32_t rval )
  {
    FastRegs::iterator itr = fastregs.lower_bound(addr);
    // itr->first is greater than or equivalent to addr.                                                                                                   
    if (itr == fastregs.end() or (addr != itr->first))
      itr = fastregs.insert(itr, std::make_pair( addr, rval ));
    return itr->second;
  }
  
  bool AccessRegister( uint32_t addr, Data const& d, sc_core::sc_time const& update_time );
};

struct Simulator : public unisim::kernel::service::Simulator
{
  Simulator( int argc, char **argv );
  virtual ~Simulator();
  
  int Run();
  int Run(double time, sc_core::sc_time_unit unit);
  bool IsRunning() const;
  bool SimulationStarted() const;
  bool SimulationFinished() const;
  virtual unisim::kernel::service::Simulator::SetupStatus Setup();
  virtual void Stop(unisim::kernel::service::Object *object, int exit_status, bool asynchronous = false);
  int GetExitStatus() const;
  void UpdateClocks();

 private:
  static void DefaultConfiguration(unisim::kernel::service::Simulator *sim);
  typedef unisim::component::tlm2::memory::ram::Memory<32, uint32_t, 8, 1024 * 1024, true> MAIN_RAM;
  typedef unisim::component::tlm2::memory::ram::Memory<32, uint32_t, 8, 1024 * 1024, true> BOOT_ROM;
  
  //typedef unisim::service::os::linux_os::Linux<uint32_t, uint32_t> LINUX_OS;
  typedef unisim::service::loader::multiformat_loader::MultiFormatLoader<uint32_t> LOADER;
  
  struct DEBUGGER_CONFIG
  {
    typedef uint32_t ADDRESS;
    static const unsigned int NUM_PROCESSORS = 1;
    /* gdb_server, inline_debugger and/or profiler */
    static const unsigned int MAX_FRONT_ENDS = 3;
  };
  
  typedef unisim::service::debug::debugger::Debugger<DEBUGGER_CONFIG> DEBUGGER;
  typedef unisim::service::debug::gdb_server::GDBServer<DEBUGGER_CONFIG::ADDRESS> GDB_SERVER;
  typedef unisim::service::debug::inline_debugger::InlineDebugger<DEBUGGER_CONFIG::ADDRESS> INLINE_DEBUGGER;
  typedef unisim::service::debug::profiler::Profiler<DEBUGGER_CONFIG::ADDRESS> PROFILER;
  typedef unisim::service::time::sc_time::ScTime ScTime;
  typedef unisim::service::time::host_time::HostTime HostTime;
  typedef unisim::service::telnet::Telnet Telnet;
  
  sc_core::sc_time             ps_clk_period;
  CPU                          cpu;
  ZynqRouter                   router;
  MPCore                       mpcore;
  MAIN_RAM                     main_ram;
  BOOT_ROM                     boot_rom;
  SLCR                         slcr;
  TTC                          ttc0;
  TTC                          ttc1;
  PS_UART                      uart0;
  PS_UART                      uart1;
  L2C                          l2c;
  Telnet                       telnet;
  
  sc_core::sc_signal<bool>              nirq_signal;
  sc_core::sc_signal<bool>              nfiq_signal;
  sc_core::sc_signal<bool>              nrst_signal;
  
  ScTime                       time;
  HostTime                     host_time;
  //LINUX_OS*                    linux_os;
  LOADER                       loader;
  
  double                       simulation_spent_time;
  
  DEBUGGER*                    debugger;
  GDB_SERVER*                  gdb_server;
  INLINE_DEBUGGER*             inline_debugger;
  PROFILER*                    profiler;
  
  bool                                     enable_gdb_server;
  unisim::kernel::service::Parameter<bool> param_enable_gdb_server;
  bool                                     enable_inline_debugger;
  unisim::kernel::service::Parameter<bool> param_enable_inline_debugger;
  bool                                     enable_profiler;
  unisim::kernel::service::Parameter<bool> param_enable_profiler;

  int exit_status;
  virtual void SigInt();
};

#endif /* SIMULATOR_HH_ */
