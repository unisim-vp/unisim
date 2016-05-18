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
ZynqRouter::relative_mapping( unsigned output_port, uint64_t range_start, uint64_t range_end, tlm::tlm_target_socket<32u>& sock )
{
  this->mapping[output_port].used = true;
  this->mapping[output_port].range_start = range_start;
  this->mapping[output_port].range_end =   range_end;
  this->mapping[output_port].output_port = output_port;
  this->mapping[output_port].translation = 0;
  (*init_socket[output_port])( sock );
}

void
ZynqRouter::absolute_mapping( unsigned output_port, uint64_t range_start, uint64_t range_end, tlm::tlm_target_socket<32u>& sock )
{
  this->mapping[output_port].used = true;
  this->mapping[output_port].range_start = range_start;
  this->mapping[output_port].range_end =   range_end;
  this->mapping[output_port].output_port = output_port;
  this->mapping[output_port].translation = range_start;
  (*init_socket[output_port])( sock );
}

ZynqRouter::ZynqRouter(const char* name, unisim::kernel::service::Object* parent)
  : unisim::kernel::service::Object( name, parent )
  , unisim::component::tlm2::interconnect::generic_router::Router<ZynqRouterConfig>( name, parent )
{
}

RegMap::RegMap( char const* socket_name )
  : socket( socket_name )
{
  socket( *this );
}

unsigned int RegMap::transport_dbg(tlm::tlm_generic_payload& payload) { throw std::runtime_error("Not implemented"); return 0; }

tlm::tlm_sync_enum
RegMap::nb_transport_fw(tlm::tlm_generic_payload& payload, tlm::tlm_phase& phase, sc_core::sc_time& t)
{
  if (phase != tlm::BEGIN_REQ) { throw std::logic_error("internal error"); }
  
  this->b_transport(payload, t);
  
  return tlm::TLM_COMPLETED;
}

void
RegMap::b_transport(tlm::tlm_generic_payload& payload, sc_core::sc_time& t )
{
  payload.set_dmi_allowed( false );
  
  if (uint8_t* byte_enable_ptr = payload.get_byte_enable_ptr())
    {
      for (int idx = int(payload.get_byte_enable_length()); --idx >= 0; )
        if (not byte_enable_ptr[idx])
          throw std::logic_error("internal error");
    }
  
  // unsigned int streaming_width = payload.get_streaming_width();
  bool status = false;
  
  tlm::tlm_command cmd = payload.get_command();
  switch (cmd)
    {
    case tlm::TLM_READ_COMMAND:
    case tlm::TLM_WRITE_COMMAND: {
      uint32_t addr = payload.get_address(), size = payload.get_data_length();
      
      if ((addr|size) & (size-1))
        std::cerr << "Malformed register address: [" << std::hex << addr << "," << std::dec << size << "].\n";
      else
        status = AccessRegister( cmd == tlm::TLM_WRITE_COMMAND, addr, size, Data( payload.get_data_ptr() ), sc_core::sc_time_stamp() + t );
    } break;
    case tlm::TLM_IGNORE_COMMAND:
      break;
    default:
      throw std::logic_error("internal error");
    }
  
  tlm::tlm_response_status resp_status = status ? tlm::TLM_OK_RESPONSE : tlm::TLM_ADDRESS_ERROR_RESPONSE;
  payload.set_response_status( resp_status );
}

/**
 * Constructor.
 * 
 * @param name the name of the module
 * @param parent the parent service
 */
MPCore::MPCore(const sc_module_name& name, unisim::kernel::service::Object* parent)
  : unisim::kernel::service::Object( name, parent )
  , sc_module(name)
  , RegMap( "mpcore_socket" )
  , unisim::kernel::service::Client<unisim::service::interfaces::TrapReporting>(name,parent)
  , trap_reporting_import("trap-reporting-import", this)
  , nIRQ("nIRQ")
  , nFIQ("nFIQ")
  , ICCICR(0)
  , ICCPMR(0)
  , ICDDCR(0)
  , generate_exceptions_event("generate_exceptions_event")
{
  std::fill_n(IENABLE, state32_count, 0);
  std::fill_n(IPENDING, state32_count, 0);
  std::fill_n(IACTIVE, state32_count, 0);
  memset( &IPRIORITYR[0], 0, sizeof (IPRIORITYR) );
  memset( &ICDIPTR[0], 0, sizeof (ICDIPTR) );
  memset( &ICDICFR[0], 0, sizeof (ICDICFR) );
  
  SC_HAS_PROCESS(MPCore);
  
  SC_METHOD(GenerateExceptionsProcess);
  sc_core::sc_module::sensitive << generate_exceptions_event;
}

namespace {
  template <uint32_t BITS, uint32_t MASK>
  struct Match {
    Match( uint32_t value ) : ok( ((value ^ BITS) & MASK) == 0 ), var( value & ~MASK ) {} bool ok; uint32_t var;
    operator bool () const { return ok; }
  };
  
  void
  dump_register_access( std::ostream& sink, bool wnr, uint32_t addr, unsigned size, RegMap::Data const& d )
  {
    sink << " register " << (wnr?"write":"read") << " @[" << std::hex << addr << ',' << std::dec << size << "]";
    if (wnr) sink << " := 0x" << std::hex << d.Value<uint32_t>() << std::dec;
    sink << ".\n";
  }
}

bool
MPCore::AccessRegister( bool wnr, uint32_t addr, unsigned size, Data const& d, sc_core::sc_time const& update_time )
{
  dump_register_access( std::cerr << "MPCore", wnr, addr, size, d );
  
  bool status = _AccessRegister( wnr, addr, size, d, update_time );
  if (status) {
    if (wnr)
      generate_exceptions_event.notify(sc_core::SC_ZERO_TIME);
  } else {
    std::cerr << "Unknown MPCORE register " << (wnr?"write":"read") << " @[" << std::hex << addr << ',' << std::dec << size << "].\n";
  }
  
  return status;
}

bool
MPCore::_AccessRegister( bool wnr, uint32_t addr, unsigned size, Data const& d, sc_core::sc_time const& update_time )
{
  if (size == 4) {
    uint32_t RAZ_WI(0);
    
    if (addr == 0x100) /* ICCICR: CPU Interface Control Register */ {
      d.Access( wnr, ICCICR );
      return true;
    }
    
    if (addr == 0x104) /* ICCPMR: Interrupt Priority Mask Register */ {
      d.Access( wnr, ICCPMR );
      return true;
    }
    
    if (addr == 0x10c) /* ICCIAR: Interrupt Acknowledge Register */ {
      if (wnr) return false;
      uint32_t iar = ReadGICC_IAR();
      d.Access( wnr, iar );
      return true;
    }
    
    if (addr == 0x1000) /* ICDDCR:  Distributor Control Register */ {
      d.Access( wnr, ICDDCR );
      return true;
    }
    
    if (addr == 0x1004) /* ICDICTR:  Interrupt Controller Type Register */ {
      if (wnr) return false;
      uint32_t result = ITLinesNumber;
      d.Access( false, result );
      return true;
    }
    
    if (Match<0x1100,-0x80> m = addr) /* ICDISER: Interrupt Set-enable Register  */ {
      unsigned idx = m.var >> 2;
      if (idx > state32_count)
        d.Access( wnr, RAZ_WI );
      else {
        uint32_t value = IENABLE[idx];
        d.Access( wnr, value );
        if (wnr) IENABLE[idx] |= value;
      }
      return true;
    }
    
    if (Match<0x1180,-0x80> m = addr) /* ICDICER: Interrupt Clear-Enable Register */ {
      unsigned idx = m.var >> 2;
      if (idx > state32_count)
        d.Access( wnr, RAZ_WI );
      else {
        uint32_t value = IENABLE[idx];
        d.Access( wnr, value );
        if (wnr) IENABLE[idx] &= ~value;
      }
      return true;
    }
    
    if (Match<0x1400,-0x400> m = addr) /* ICDIPR: Interrupt Priority Register */ {
      unsigned idx = m.var;
      if (idx < ITLinesCount)
        d.Access( wnr, &IPRIORITYR[idx], 4 );
      else
        d.Access( wnr, RAZ_WI );
      return true;
    }
    
    if (Match<0x1800,-0x400> m = addr) /* ICDIPTR: Interrupt Processor Targets Register */ {
      unsigned idx = m.var;
      if (idx < 16) {
        uint32_t value = 0x01010101;
        d.Access( wnr, value );
        return true;
      }
      if ((24 <= idx) and (idx < ITLinesCount)) {
        d.Access( wnr, &ICDIPTR[idx], 4 );
        return true;
      }
      d.Access( wnr, RAZ_WI );
      return true;
    }
    
    if (Match<0x1c00,-0x100> m = addr) /* ICDICFR: Interrupt Configuration Register */ {
      unsigned idx = m.var >> 2;
      d.Access( wnr, (idx < icfgr_count) ? ICDICFR[idx] : RAZ_WI );
      return true;
    }
  }
  
  return false;
}

void
MPCore::SendInterrupt( unsigned idx )
{
  IPENDING[idx/32] |= (1ul << (idx%32));
  generate_exceptions_event.notify(sc_core::SC_ZERO_TIME);
}

unsigned
MPCore::HighestPriorityPendingInterrupt( uint8_t required, uint8_t enough )
{
  unsigned hppi = 1023;
  
  if ((ICCICR & ICDDCR & 1) == 0)
    return hppi;
  
  // Preemption not supported                                                                                                                               
  for (unsigned idx = 0; idx < state32_count; ++ idx)
    if (IACTIVE[idx])
      return hppi;
  
  for (unsigned int_id = 0; int_id < ITLinesCount; int_id += 1) {
    unsigned word = int_id / 32;
    uint32_t bit = 1u << (int_id % 32);
    if (not (IPENDING[word] & IENABLE[word] & bit))
      continue;
    uint8_t priority = IPRIORITYR[int_id];
    if (priority < enough)
      return int_id;
    if (priority < required) {
      hppi = int_id;
      required = priority;
    }
  }
    
  return hppi;
}

void
MPCore::GenerateExceptionsProcess()
{
  nIRQ = not (HighestPriorityPendingInterrupt( ICCPMR, ICCPMR ) < ITLinesCount);
}

uint32_t
MPCore::ReadGICC_IAR()
{
  unsigned int_id = HighestPriorityPendingInterrupt( ICCPMR, 0 );
  if (int_id < 16)
    throw std::logic_error("not implemented");
  
  // Acknowledging
  unsigned word = int_id / 32;
  uint32_t bit = 1u << (int_id % 32);
  IPENDING[word] &= ~bit;
  IACTIVE[word] |= bit;
  
  return int_id;
}

TTC::TTC( const sc_module_name& name, unisim::kernel::service::Object* parent, MPCore& _mpcore, unsigned _id, unsigned _base_it )
  : unisim::kernel::service::Object( name, parent )
  , sc_module(name)
  , RegMap("ttc_sock")
  , unisim::kernel::service::Client<unisim::service::interfaces::TrapReporting>(name,parent)
  , mpcore(_mpcore)
  , id(_id)
  , base_it(_base_it)
  , update_state_event("update_state_event")
  , clock_period(sc_core::SC_ZERO_TIME)
{
  std::fill_n(Clock_Control, 3, 0);
  std::fill_n(Counter_Control, 3, 0x21);
  std::fill_n(Counter_Value, 3, 0);
  std::fill_n(Interval_Counter, 3, 0);
  std::fill_n(Interrupt_Enable, 3, 0);
  std::fill_n(Interrupt_Enable, 3, 0);
  std::fill_n(it_lines, 3, -1);
  
  SC_HAS_PROCESS(TTC);
  
  SC_METHOD(UpdateStateProcess);
  sc_core::sc_module::sensitive << update_state_event;
}

void
TTC::UpdateStateProcess()
{
  UpdateState( sc_core::sc_time_stamp() );
}

void
TTC::UpdateCounterState( unsigned idx, sc_core::sc_time const& update_time )
{
  if (Clock_Control[idx] & 0x28)
    throw std::logic_error( "internal error" );
  if (Counter_Control[idx] & 0x10) {
    Counter_Value[idx] = 0;
    Counter_Control[idx] &= ~0x10;
  }
  if (Counter_Control[idx] & 1) {
    // Disabled
    last_state_update_time[idx] = update_time;
    return;
  }
  
  // Computing counter ticks since last update
  sc_dt::uint64 ticks;
  sc_core::sc_time  tick_period( clock_period );
  if (Clock_Control[idx] & 1) // Prescaling
    tick_period *= sc_dt::uint64( 1 << (((Clock_Control[idx] >> 1) & 0xf) + 1) );
  
  ticks = (update_time - last_state_update_time[idx]) / tick_period;
  last_state_update_time[idx] += ticks*tick_period;
  
  bool interval_mode = (Counter_Control[idx] & 2);
  bool interrupt_enable = interval_mode ? (Interrupt_Enable[idx] & 0x01) : (Interrupt_Enable[idx] & 0x10);
  sc_dt::uint64 interval = interval_mode ? Interval_Counter[idx] : 0x10000;
  sc_dt::uint64 counter_value = Counter_Value[idx];
  
  // Computing ticks to next zero and updating the counter value to its current state
  sc_dt::uint64 ticks_to_next_zero = 0;
  if (Counter_Control[idx] & 4) {
    // Decrement
    ticks_to_next_zero = counter_value ? counter_value : interval;
    if (ticks > counter_value) {
      counter_value = interval - 1 - ((ticks - counter_value - 1) % interval);
    } else {
      counter_value = counter_value - ticks;
    }
  } else {
    // Increment
    if (interval > counter_value) {
      ticks_to_next_zero = interval - counter_value;
      counter_value = (counter_value + ticks) % interval;
    } else {
      ticks_to_next_zero = 1;
      if (ticks > 0)
        counter_value = (ticks - 1) % interval;
    }
  }
  
  if (interrupt_enable) {
    if (ticks > ticks_to_next_zero)
      throw std::logic_error("internal error");
    else if (ticks == ticks_to_next_zero)
      mpcore.SendInterrupt( base_it + idx );
    else
      update_state_event.notify( last_state_update_time[idx] + (ticks_to_next_zero - ticks)*tick_period - update_time );
  }
  
  Counter_Value[idx] = counter_value;
}

void
TTC::UpdateState( sc_core::sc_time const& update_time )
{
  for (unsigned idx = 0; idx < 3; ++idx)
    UpdateCounterState( idx, update_time );
}

bool
TTC::AccessRegister( bool wnr, uint32_t addr, unsigned size, Data const& d, sc_core::sc_time const& update_time )
{
  dump_register_access( std::cerr << this->GetName(), wnr, addr, size, d );
  
  UpdateState( update_time );
  int update_idx = -1;
  
  if (size == 4) {
    //uint32_t RAZ_WI(0);
    
    switch (addr) {
    case 0x00: d.Access( wnr, Clock_Control[0] ); update_idx = 0; break;
    case 0x04: d.Access( wnr, Clock_Control[1] ); update_idx = 1; break;
    case 0x08: d.Access( wnr, Clock_Control[2] ); update_idx = 2; break;
    case 0x0c: d.Access( wnr, Counter_Control[0] ); update_idx = 0; break;
    case 0x10: d.Access( wnr, Counter_Control[1] ); update_idx = 1; break;
    case 0x14: d.Access( wnr, Counter_Control[2] ); update_idx = 2; break;
    case 0x24: d.Access( wnr, Interval_Counter[0] ); update_idx = 0; break;
    case 0x28: d.Access( wnr, Interval_Counter[1] ); update_idx = 1; break;
    case 0x2c: d.Access( wnr, Interval_Counter[2] ); update_idx = 2; break;
    case 0x60: d.Access( wnr, Interrupt_Enable[0] ); update_idx = 0; break;
    case 0x64: d.Access( wnr, Interrupt_Enable[1] ); update_idx = 1; break;
    case 0x68: d.Access( wnr, Interrupt_Enable[2] ); update_idx = 2; break;
    default:
      std::cerr << "Unknown TTC register " << (wnr?"write":"read") << " @[" << std::hex << addr << ',' << std::dec << size << "].\n";
      return false;
      
    }
  }
  
  if (wnr and (update_idx >= 0))
    UpdateCounterState( update_idx, update_time );
  
  return true;
}

SLCR::SLCR(const sc_module_name& name, unisim::kernel::service::Object* parent)
  : unisim::kernel::service::Object( name, parent )
  , sc_module( name )
  , RegMap("slcr_sock")
  , unisim::kernel::service::Client<unisim::service::interfaces::TrapReporting>(name,parent)
  , ARM_PLL_CTRL(0x1a008)
  , DDR_PLL_CTRL(0x1a008)
  , IO_PLL_CTRL(0x1a008)
  , ARM_CLK_CTRL(0x1f000400)
  , CLK_621_TRUE(0x1)
  , UART_CLK_CTRL(0x3f03)
{
}

bool
SLCR::AccessRegister( bool wnr, uint32_t addr, unsigned size, Data const& d, sc_core::sc_time const& update_time )
{
  dump_register_access( std::cerr << "SLCR", wnr, addr, size, d );
  
  if (size == 4) {
    //uint32_t RAZ_WI(0);
    
    switch (addr) {
    case 0x100: d.Access( wnr,  ARM_PLL_CTRL ); return true;
    case 0x104: d.Access( wnr,  DDR_PLL_CTRL ); return true;
    case 0x108: d.Access( wnr,   IO_PLL_CTRL ); return true;
    case 0x120: d.Access( wnr,  ARM_CLK_CTRL ); return true;
    case 0x154: d.Access( wnr, UART_CLK_CTRL ); return true;
    case 0x1c4: d.Access( wnr,  CLK_621_TRUE ); return true;
    }
  }
  
  std::cerr << "Unknown SLCR register " << (wnr?"write":"read") << " @[" << std::hex << addr << ',' << std::dec << size << "].\n";
  return false;
}

Simulator::Simulator(int argc, char **argv)
  : unisim::kernel::service::Simulator(argc, argv, Simulator::DefaultConfiguration)
  , cpu( "cpu" )
  , router( "router" )
  , mpcore( "mpcore" )
  , main_ram( "main_ram" )
  , boot_rom( "boot_rom" )
  , slcr( "slcr" )
  , ttc0( "ttc0", 0, mpcore, 0, 42 )
  , ttc1( "ttc1", 0, mpcore, 1, 69 )
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
  
  router.relative_mapping( 0, 0x00000000, 0x3fffffff, main_ram.slave_sock ); /* Main OCM RAM */
  router.absolute_mapping( 1, 0xffff0000, 0xffffffff, boot_rom.slave_sock ); /* Boot OCM ROM */
  router.relative_mapping( 2, 0xf8f00000, 0xf8f01fff, mpcore.socket ); /* Mpcore - SCU, Interrupt controller, Counters and Timers */
  router.relative_mapping( 3, 0xf8000000, 0xf8000fff, slcr.socket ); /* SLCR */
  router.relative_mapping( 4, 0xf8001000, 0xf8001fff, ttc0.socket ); /* TTC0 */
  router.relative_mapping( 5, 0xf8002000, 0xf8002fff, ttc1.socket ); /* TTC1 */
  
  mpcore.nIRQ( nirq_signal );
  mpcore.nFIQ( nfiq_signal );
  
  // Connect debugger to CPU
  cpu.debug_control_import >> debugger->debug_control_export;
  cpu.instruction_counter_trap_reporting_import >> debugger->trap_reporting_export;
  cpu.exception_trap_reporting_import >> debugger->trap_reporting_export;
  mpcore.trap_reporting_import >> debugger->trap_reporting_export;
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
  
  ttc0.clock_period = ttc1.clock_period = cpu.GetCpuCycleTime();
  
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
  sim->SetVariable( "loader.memory-mapper.mapping", "main_ram:0x00000000-0x3fffffff,boot_rom:0xffff0000-0xffffffff:+0xffff0000" );
  
  
  sim->SetVariable( "gdb-server.architecture-description-filename", "gdb_arm_with_neon.xml" );
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
