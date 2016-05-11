/*
 *  Copyright (c) 2010,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific 
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 *  SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 *  SUCH DAMAGE.
 *
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
#include <unisim/component/cxx/processor/arm/armemu/cpu.hh>
#include <unisim/component/cxx/processor/arm/armemu/cp15.hh>
#include <unisim/component/cxx/processor/arm/cpu.tcc>
#include <unisim/component/cxx/processor/arm/cache.hh>
#include <unisim/component/cxx/processor/arm/memory_op.hh>
#include <unisim/kernel/debug/debug.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/util/likely/likely.hh>

#include <sstream>
#include <string>
#include <cstring>
#include <stdexcept>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace armemu {

using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::service::Service;
using unisim::service::interfaces::MemoryInjection;
using unisim::service::interfaces::DebugControl;
using unisim::service::interfaces::MemoryAccessReporting;
using unisim::service::interfaces::TrapReporting;
using unisim::service::interfaces::Disassembly;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::LinuxOS;
using unisim::service::interfaces::SymbolTableLookup;
using unisim::util::endian::E_BIG_ENDIAN;
using unisim::util::endian::E_LITTLE_ENDIAN;
using unisim::util::endian::BigEndian2Host;
using unisim::util::endian::LittleEndian2Host;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;

/** Constructor.
 *
 * @param name the name that will be used by the UNISIM service 
 *   infrastructure and will identify this object
 * @param parent the parent object of this object
 */
CPU::CPU(const char *name, Object *parent)
  : unisim::kernel::service::Object(name, parent)
  , unisim::component::cxx::processor::arm::CPU<ARMv7emu>(name, parent)
  , Service<MemoryAccessReportingControl>(name, parent)
  , Client<MemoryAccessReporting<uint32_t> >(name, parent)
  , Service<MemoryInjection<uint32_t> >(name, parent)
  , Client<DebugControl<uint32_t> >(name, parent)
  , Client<TrapReporting>(name, parent)
  , Service<Disassembly<uint32_t> >(name, parent)
  , Service< Memory<uint32_t> >(name, parent)
  , Client< Memory<uint32_t> >(name, parent)
  , Client<LinuxOS>(name, parent)
  , Client<SymbolTableLookup<uint32_t> >(name, parent)
  , memory_access_reporting_control_export("memory-access-reporting-control-export", this)
  , memory_access_reporting_import("memory-access-reporting-import", this)
  , disasm_export("disasm-export", this)
  , memory_injection_export("memory-injection-export", this)
  , memory_export("memory-export", this)
  , memory_import("memory-import", this)
  , debug_control_import("debug-control-import", this)
  , symbol_table_lookup_import("symbol-table-lookup-import", this)
  , exception_trap_reporting_import("exception-trap-reporting-import", this)
  , instruction_counter_trap_reporting_import("instruction-counter-trap-reporting-import", this)
  , linux_os_import("linux-os-import", this)
  , requires_finished_instruction_reporting(true)
  , requires_memory_access_reporting(true)
  // , icache("icache", this)
  // , dcache("dcache", this)
  , arm32_decoder()
  , thumb_decoder()
  , csselr(0)
  , mmu()
  , instruction_counter(0)
  , voltage(0)
  , trap_on_instruction_counter(0)
  , param_midr( "MIDR", this, this->BaseCpu::midr, "Value of MIDR (Main ID Register)." )
  , sctlr_rstval( this->BaseCpu::sctlr )
  , param_sctlr_rstval("SCTLR", this, this->sctlr_rstval, "The processor reset value of the SCTLR register.")
  , param_cpu_cycle_time_ps("cpu-cycle-time-ps", this, cpu_cycle_time_ps, "The processor cycle time in picoseconds.")
  , param_voltage("voltage", this, this->voltage, "The processor voltage in mV.")
  , param_verbose("verbose", this, this->BaseCpu::verbose, "Activate the verbose system (0 = inactive, different than 0 = active).")
  , param_trap_on_instruction_counter("trap-on-instruction-counter", this, trap_on_instruction_counter,
                                      "Produce a trap when the given instruction count is reached.")
  , param_linux_printk_snooping( "linux_printk_snooping", this, linux_printk_snooping, "Activate the printk snooping" )
  , stat_instruction_counter("instruction-counter", this, instruction_counter, "Number of instructions executed.")
  , ipb_base_address( -1 )
  , linux_printk_buf_addr( 0 )
  , linux_printk_buf_size( 0 )
  , linux_printk_snooping( false )
{
  // Set the right format for various of the variables
  param_cpu_cycle_time_ps.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
  param_voltage.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
  param_trap_on_instruction_counter.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
  stat_instruction_counter.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
}

/** Destructor.
 */
CPU::~CPU()
{
}

/** Object setup method.
 * This method is called during the setup phase (1st phase).
 * 
 * @return true on success, false otherwise
 */
bool 
CPU::BeginSetup()
{
  if (verbose)
    logger << DebugInfo << "CPU::BeginSetup(): Verbose activated." << EndDebugInfo;

  if (cpu_cycle_time_ps == 0)
    {
      // We can't (don't want to) provide a default valid cycle time
      logger << DebugError << "cpu-cycle-time should be strictly positive" << EndDebugError;
      return false;
    }

  return true;
}

/** Resets the internal values of corresponding CP15 Registers
 */
void
CPU::CP15ResetRegisters()
{
  this->BaseCpu::CP15ResetRegisters();
  /* sctlr takes its reset value */
  sctlr = sctlr_rstval;
}
    
/** Object setup method.
 * This method is called during the setup phase (2nd and final phase).
 * 
 * @return true on success, false otherwise
 */
bool 
CPU::EndSetup()
{
  /* Finalizing LOAD job */
  if (not linux_os_import) {
    if (verbose)
      logger << DebugInfo << "No Linux OS connection ==> **bare metal/full system** mode" << EndDebugInfo;
    this->TakeReset();
  } else {
    /* Linux OS has setup Memory and User Registers */
    if (verbose)
      logger << DebugInfo << "Linux OS connection present ==> using linux os emulation" << EndDebugInfo;
    
    // TODO: Following should be done by linux_os
    /* We need to set System Registers as a standard linux would have
     * done, we only affects flags that impact a Linux OS emulation
     * (others are unaffected).
     */
    SCTLR::I.Set(       sctlr, 1 ); // Instruction Cache enable
    SCTLR::C.Set(       sctlr, 1 ); // Cache enable
    SCTLR::A.Set(       sctlr, 0 ); // Alignment check enable
    /*** Program Status Register (PSR) ***/
    cpsr.Set( J, 0 );
    cpsr.ITSetState( 0b0000, 0b0000 );
    cpsr.Set( E, 0 ); // TODO, should *REALLY* be done in LinuxOS
    // Thumb execution state bit already set by LinuxOS, as a side-effect of PC assignment */
    cpsr.Set( M, USER_MODE );
  }
  
  if (verbose)
    logger << DebugInfo << "Initial pc set to 0x" << std::hex << GetNPC() << std::dec << EndDebugInfo;
  
  /* Initialize the caches and power support as required. */
  {
    unsigned int min_cycle_time = 0;
    uint64_t il1_def_voltage = 0;
    uint64_t dl1_def_voltage = 0;

    // if (icache.power_mode_import)
    //   {
    //     min_cycle_time =  icache.power_mode_import->GetMinCycleTime();
    //     il1_def_voltage = icache.power_mode_import->GetDefaultVoltage();
    //   }
    // if (dcache.power_mode_import)
    //   {
    //     if ( dcache.power_mode_import->GetMinCycleTime() > min_cycle_time )
    //       min_cycle_time = dcache.power_mode_import->GetMinCycleTime();
    //     dl1_def_voltage =  dcache.power_mode_import->GetDefaultVoltage();
    //   }

    if (min_cycle_time > 0)
      {
        if ( cpu_cycle_time_ps < min_cycle_time )
          {
            logger << DebugWarning;
            logger << "A cycle time of " << cpu_cycle_time_ps << " ps is too low for the simulated hardware !" << std::endl;
            logger << "cpu cycle time should be >= " << min_cycle_time << " ps." << std::endl;
            logger << EndDebugWarning;
          }
      }

    if (voltage == 0)
      {
        voltage = (il1_def_voltage > dl1_def_voltage) ? il1_def_voltage : dl1_def_voltage;
        logger << DebugWarning
               << "A cpu voltage was not defined (set to 0). Using the maximum voltage found from the caches as current voltage. "
               << "Voltage used is " << voltage << " mV.";
        // if ( icache.power_mode_import )
        //   logger << std::endl << "  - instruction cache voltage = " << il1_def_voltage << " mV";
        // if ( dcache.power_mode_import )
        //   logger << std::endl << "  - data cache voltage = " << dl1_def_voltage << " mV";
        logger << EndDebugWarning;
      }
  
    // if (icache.power_mode_import)
    //   icache.power_mode_import->SetPowerMode(cpu_cycle_time_ps, voltage);
    // if (dcache.power_mode_import)
    //   dcache.power_mode_import->SetPowerMode(cpu_cycle_time_ps, voltage);

    if (verbose)
      logger << DebugInfo << "Setting cpu cycle time to " << cpu_cycle_time_ps << " ps." << std::endl
             << DebugInfo << "Setting cpu voltage to " << voltage << " mV." << EndDebugInfo;
  }
  
  /* If the memory access reporting import is not connected remove the
   *   need of reporting memory accesses and finished instruction.
   */
  if(!memory_access_reporting_import) {
    requires_memory_access_reporting = false;
    requires_finished_instruction_reporting = false;
  }


  if (linux_printk_snooping)
    {
      if (verbose)
        logger << DebugInfo << "Linux printk snooping enabled" << EndDebugInfo;
      
      typedef unisim::util::debug::Symbol<uint32_t> Symbol;
      Symbol const* symbol = 0;
      
      if (symbol_table_lookup_import)
        symbol = symbol_table_lookup_import->FindSymbolByName("__log_buf", Symbol::SYM_OBJECT);
      
      if (symbol)
        {
          linux_printk_buf_addr = symbol->GetAddress();
          linux_printk_buf_size = symbol->GetSize();
          linux_printk_snooping = true;
          
          if(verbose)
              logger << DebugInfo << "Found Linux printk buffer @["
                     << std::hex << linux_printk_buf_addr << ','
                     << std::dec << linux_printk_buf_size << "]" << EndDebugInfo;
        }
      else
        {
          logger << DebugWarning << "Linux printk buffer not found. Linux printk snooping will not work properly." << EndDebugWarning;
          linux_printk_snooping = false;
        }
    }
  
  
  return true;
}

struct SoftMemAcc { static bool const updateTLB=true; };

/** Performs a prefetch access.
 *
 * @param addr the address of the memory prefetch access
 */
void
CPU::PerformPrefetchAccess( uint32_t addr )
{
  // bool cache_access = SCTLR::C.Get( sctlr ) and dcache.GetSize();
  
  // if (likely(cache_access))
  //   {
  //     dcache.prefetch_accesses++;
  //     uint32_t cache_tag = dcache.GetTag(addr);
  //     uint32_t cache_set = dcache.GetSet(addr);
  //     uint32_t cache_way;
  //     bool cache_hit =
  //       dcache.GetWay(cache_tag, cache_set, &cache_way) and
  //       dcache.GetValid(cache_set, cache_way);
      
  //     // If the access was a miss, data needs to be fetched from main
  //     //   memory and placed into the cache
  //     if (likely(not cache_hit))
  //       {
  //         // get a way to replace
  //         cache_way = dcache.GetNewWay( cache_set );
  //         // get the valid and dirty bits from the way to replace
  //         bool cache_valid = dcache.GetValid( cache_set, cache_way );
  //         bool cache_dirty = dcache.GetDirty( cache_set, cache_way );
  //         if (cache_valid and cache_dirty)
  //           {
  //             // the cache line to replace is valid and dirty so it needs
  //             //   to be sent to the main memory
  //             uint8_t *rep_cache_data = 0;
  //             uint32_t rep_cache_address = dcache.GetBaseAddress( cache_set, cache_way );
  //             dcache.GetData( cache_set, cache_way, &rep_cache_data );
  //             PrWrite( rep_cache_address, rep_cache_data, dcache.LINE_SIZE );
  //           }
  //         // the new data can be requested
  //         uint8_t *cache_data = 0;
  //         uint32_t cache_address = dcache.GetBaseAddressFromAddress(addr);
  //         // When getting the data we get the pointer to the cache
  //         //   line containing the data, so no need to write the cache
  //         //   afterwards
  //         uint32_t cache_line_size = dcache.GetData( cache_set, cache_way, &cache_data );
  //         PrRead( cache_address, cache_data, cache_line_size );
  //         dcache.SetTag( cache_set, cache_way, cache_tag );
  //         dcache.SetValid( cache_set, cache_way, 1 );
  //         dcache.SetDirty( cache_set, cache_way, 0 );
  //       }
  //     else
  //       {
  //         dcache.prefetch_hits++;
  //       }
  //     if (unlikely( dcache.power_estimator_import ))
  //       dcache.power_estimator_import->ReportReadAccess();
  //   }
  // else
    {
      /* it is just a cache prefetch, ignore the request if cache is not active */
    }
  /* CHECK: should we report a memory access for a prefetch???? */
}

/** Performs an unaligned write access.
 * @param addr the address of the memory write access
 * @param size the size of the memory write access
 * @param value the value of the memory write access
 */
void
CPU::PerformUWriteAccess( uint32_t addr, uint32_t size, uint32_t value )
{
  uint32_t const lo_mask = size - 1;
  if (unlikely((lo_mask > 3) or (size & lo_mask))) throw exception::DataAbortException();
  uint32_t misalignment = addr & lo_mask;
  
  if (unlikely(misalignment and not SCTLR::A.Get( this->sctlr ))) {
    uint32_t eaddr = addr;
    if (GetEndianness() == unisim::util::endian::E_BIG_ENDIAN) {
      for (unsigned byte = size; --byte < size; ++eaddr)
        PerformWriteAccess( eaddr, 1, (value >> (8*byte)) & 0xff );
    } else {
      for (unsigned byte = 0; byte < size; ++byte, ++eaddr)
        PerformWriteAccess( eaddr, 1, (value >> (8*byte)) & 0xff );
    }
    return;
  } else {
    PerformWriteAccess( addr, size, value );
    return;
  }
}

/** Performs an aligned write access.
 * @param addr the address of the memory write access
 * @param size the size of the memory write access
 * @param value the value of the memory write access
 */
void
CPU::PerformWriteAccess( uint32_t addr, uint32_t size, uint32_t value )
{
  uint32_t const lo_mask = size - 1;
  if (unlikely((lo_mask > 3) or (size & lo_mask))) throw exception::DataAbortException();
  uint32_t misalignment = addr & lo_mask;
  
  if (unlikely(misalignment)) {
    if (this->linux_os_import) {
      // we are executing on linux emulation mode, handle all misalignemnt as if implemented
      PerformUWriteAccess( addr, size, value );
      return;
    }
    else {
      // TODO: Full misaligned DataAbort(mva, ipaddress, ...)
      throw unisim::component::cxx::processor::arm::exception::DataAbortException(); 
    }
  }

  uint8_t data[4];

  if (GetEndianness() == unisim::util::endian::E_BIG_ENDIAN) {
    uint32_t shifter = value;
    for (int byte = size; --byte >= 0;)
      { data[byte] = shifter; shifter >>= 8; }
  } else {
    uint32_t shifter = value;
    for (int byte = 0; byte < int( size ); ++byte)
      { data[byte] = shifter; shifter >>= 8; }
  }
  
  if (unlikely(linux_printk_snooping))
    {
      if (uint32_t(addr - linux_printk_buf_addr) < linux_printk_buf_size)
        {
          //cout << "\033[31m";
          for (unsigned idx = 0; idx < size; ++idx)
            {
              char c = data[idx];
              static bool new_line_inserted = true;
              if      (isgraph(c) or isspace(c)) {
                std::cout << c;
                new_line_inserted = (c == '\n');
              }
              else if (not new_line_inserted) {
                std::cout << '\n';
                new_line_inserted = true;
              }
            }
          //cout << "\033[37m";
        }
    }
  
  uint32_t write_addr = TranslateAddress<SoftMemAcc>( addr & ~lo_mask, false, true, size );
  
  // In armv7 no address mungling is performed
  // In armv5, fix the write address according to request size when big endian
  // write_addr ^= ((-size) & 3);
  
  // bool cache_access = dcache.GetSize() and SCTLR::C.Get( this->sctlr );
  // if (likely(cache_access))
  //   {
  //     dcache.write_accesses++;
  //     uint32_t cache_tag = dcache.GetTag(write_addr);
  //     uint32_t cache_set = dcache.GetSet(write_addr);
  //     uint32_t cache_way;
  //     bool cache_hit = false;
  //     if (dcache.GetWay( cache_tag, cache_set, &cache_way ))
  //       {
  //         if (dcache.GetValid( cache_set, cache_way))
  //           {
  //             // the access is a hit
  //             cache_hit = true;
  //           }
  //       }
  //     // if the access was a hit the data needs to be written into
  //     //   the cache, if the access was a miss the data needs to be
  //     //   written into memory, but the cache doesn't need to be updated
  //     if (likely(cache_hit))
  //       {
  //         dcache.write_hits++;
  //         uint32_t cache_index = dcache.GetIndex( write_addr );
  //         dcache.SetData( cache_set, cache_way, cache_index, size, data );
  //         dcache.SetDirty( cache_set, cache_way, 1 );
  //       }
  //     else
  //       {
  //         PrWrite( write_addr, data, size );
  //       }

  //     if (unlikely(dcache.power_estimator_import))
  //       dcache.power_estimator_import->ReportWriteAccess();
  //   }
  // else
    {
      // There is no data cache or data should not be cached.
      // Just send the request to the memory interface
      PrWrite( write_addr, data, size );
    }

  /* report read memory access if necessary */
  ReportMemoryAccess( unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, addr, size );
}

/** Performs an unaligned read access.
 * @param addr the address of the memory read access
 * @param size the size of the memory read access
 */
uint32_t
CPU::PerformUReadAccess( uint32_t addr, uint32_t size )
{
  uint32_t const lo_mask = size - 1;
  if (unlikely((lo_mask > 3) or (size & lo_mask))) throw exception::DataAbortException();
  uint32_t misalignment = addr & lo_mask;
  
  if (unlikely(misalignment and not SCTLR::A.Get( this->sctlr ))) {
    uint32_t result = 0;
    if (GetEndianness() == unisim::util::endian::E_BIG_ENDIAN) {
      for (unsigned byte = 0; byte < size; ++byte)
        result = (result << 8) | PerformReadAccess( addr + byte, 1 );
    } else {
      for (unsigned byte = size; --byte < size;)
        result = (result << 8) | PerformReadAccess( addr + byte, 1 );
    }
    return result;
  } else
    return PerformReadAccess( addr, size );
}

/** Performs an aligned read access.
 * @param addr the address of the memory read access
 * @param size the size of the memory read access
 */
uint32_t
CPU::PerformReadAccess(	uint32_t addr, uint32_t size )
{
  uint32_t const lo_mask = size - 1;
  if (unlikely((lo_mask > 3) or (size & lo_mask))) throw exception::DataAbortException();
  uint32_t misalignment = addr & lo_mask;
  
  if (unlikely(misalignment)) {
    if (this->linux_os_import) {
      // we are executing on linux emulation mode, handle all misalignemnt as if implemented
      return PerformUReadAccess( addr, size );
    }
    else {
      // TODO: Full misaligned DataAbort(mva, ipaddress, ...)
      throw unisim::component::cxx::processor::arm::exception::DataAbortException();
    }
  }
  
  uint32_t read_addr = TranslateAddress<SoftMemAcc>( addr & ~lo_mask, false, false, size );
  
  uint8_t data[4];

  // In armv7 no address mungling is performed
  // In armv5, fix the read address depending on the request size and endianess
  // if (GetEndianness() == unisim::util::endian::E_BIG_ENDIAN) {
  //   read_addr ^= ((-size) & 3);
  // }

  // bool cache_access = dcache.GetSize() and SCTLR::C.Get( this->sctlr );
  // if (likely(cache_access))
  //   {
  //     dcache.read_accesses++;
  //     uint32_t cache_tag = dcache.GetTag( read_addr );
  //     uint32_t cache_set = dcache.GetSet( read_addr );
  //     uint32_t cache_way;
  //     bool cache_hit =
  //       dcache.GetWay(cache_tag, cache_set, &cache_way) and
  //       dcache.GetValid(cache_set, cache_way);
  //     // If the access was a miss, data needs to be fetched from main
  //     //   memory and placed into the cache
  //     if (unlikely(not cache_hit))
  //       {
  //         // get a way to replace
  //         cache_way = dcache.GetNewWay( cache_set );
  //         // Check if the way to replace needs to be written back
  //         if (dcache.GetValid( cache_set, cache_way ) and dcache.GetDirty( cache_set, cache_way ))
  //           {
  //             // The cache line to replace is valid and dirty so it
  //             //   needs to be sent back
  //             uint8_t* rep_cache_data = 0;
  //             uint32_t rep_cache_address = dcache.GetBaseAddress( cache_set, cache_way );
  //             dcache.GetData( cache_set, cache_way, &rep_cache_data );
  //             PrWrite( rep_cache_address, rep_cache_data, dcache.LINE_SIZE );
  //           }
  //         // the new data can be requested
  //         uint8_t* cache_data = 0;
  //         uint32_t cache_address = dcache.GetBaseAddressFromAddress( read_addr );
  //         // When getting the data we get the pointer to the cache
  //         // line containing the data, so no need to write the cache
  //         // afterwards
  //         uint32_t cache_line_size = dcache.GetData( cache_set, cache_way, &cache_data );
  //         PrRead( cache_address, cache_data, cache_line_size );
  //         dcache.SetTag( cache_set, cache_way, cache_tag );
  //         dcache.SetValid( cache_set, cache_way, 1 );
  //         dcache.SetDirty( cache_set, cache_way, 0 );
  //       }
  //     else
  //       {
  //         // cache hit
  //         dcache.read_hits++;
  //       }

  //     // at this point the data is in the cache, we can read it from the
  //     //   cache
  //     uint32_t cache_index = dcache.GetIndex(read_addr);
  //     uint8_t* ptr;
  //     (void)dcache.GetData(cache_set, cache_way, cache_index, size, &ptr);
  //     memcpy( &data[0], ptr, size );
      
  //     if (unlikely(dcache.power_estimator_import))
  //       dcache.power_estimator_import->ReportReadAccess();
  //   }
  // else // there is no data cache
    {
      // just read the data from the memory system
      PrRead(read_addr, &data[0], size);
    }

  /* report read memory access if necessary */
  ReportMemoryAccess(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, addr, size);
  
  /* Compute return value */
  uint32_t value;
  if (GetEndianness() == unisim::util::endian::E_LITTLE_ENDIAN) {
    uint32_t shifter = 0;
    for (int byte = size; --byte >= 0;)
      { shifter = (shifter << 8) | uint32_t( data[byte] ); }
    value = shifter;
  } else {
    uint32_t shifter = 0;
    for (int byte = 0; byte < int( size ); ++byte)
      { shifter = (shifter << 8) | uint32_t( data[byte] ); }
    value = shifter;
  }

  return value;
}


/** Object disconnect method.
 * This method is called when this UNISIM object is disconnected from other
 *   UNISIM objects.
 */
void 
CPU::OnDisconnect()
{
  /* TODO */
}

/** Execute one complete instruction.
 */
void 
CPU::StepInstruction()
{
  /* Instruction boundary next_pc becomes current_pc */
  uint32_t insn_addr = this->current_pc = GetNPC();
  
  if (debug_control_import)
    {
      for (bool proceed = false; not proceed; )
        {
          switch (debug_control_import->FetchDebugCommand( insn_addr ))
            {
            case DebugControl<uint32_t>::DBG_STEP: 
              proceed = true;
              break;
            case DebugControl<uint32_t>::DBG_SYNC:
              Sync();
              continue;
              break;
            case DebugControl<uint32_t>::DBG_RESET: /* TODO : memory_interface->Reset(); */ break;
            case DebugControl<uint32_t>::DBG_KILL:
              Stop(0);
              return;
            }
        }
    }
  
  try {
    // Instruction Fetch Decode and Execution (may generate exceptions
    // known as synchronous aborts since their occurences are a direct
    // consequence of the instruction execution).
    
    if (cpsr.Get( T )) {
      /* Thumb state */
      isa::thumb2::CodeType insn;
      ReadInsn(insn_addr, insn);
    
      /* Decode current PC */
      isa::thumb2::Operation<CPU>* op;
      op = thumb_decoder.Decode(insn_addr, insn);
      unsigned insn_length = op->GetLength();
      if (insn_length % 16) throw std::logic_error("Bad T2 instruction length");
    
      /* update PC register value before execution */
      this->gpr[15] = this->next_pc + 4;
      this->next_pc += insn_length / 8;
    
      /* Execute instruction */
      asm volatile( "thumb2_operation_execute:" );
      op->execute( *this );
    
      this->ITAdvance();
      //op->profile(profile);
    }
  
    else {
      /* Arm32 state */
    
      /* fetch instruction word from memory */
      isa::arm32::CodeType insn;
      ReadInsn(insn_addr, insn);
      
      /* Decode current PC */
      isa::arm32::Operation<CPU>* op;
      op = arm32_decoder.Decode(insn_addr, insn);
    
      /* update PC register value before execution */
      this->gpr[15] = this->next_pc + 8;
      this->next_pc += 4;
    
      /* Execute instruction */
      asm volatile( "arm32_operation_execute:" );
      op->execute( *this );
      //op->profile(profile);
    }
    
    if (unlikely( requires_finished_instruction_reporting and memory_access_reporting_import ))
      memory_access_reporting_import->ReportFinishedInstruction(this->current_pc, this->next_pc);
    
    instruction_counter++;
    if (unlikely( instruction_counter_trap_reporting_import and (trap_on_instruction_counter == instruction_counter) ))
      instruction_counter_trap_reporting_import->ReportTrap(*this);
  
  }
  
  catch (exception::SVCException const& svexc) {
    /* Resuming execution, since SVC exceptions are explicitly
     * requested from regular instructions. ITState will be updated by
     * TakeSVCException (as done in the ARM spec). */
    if (unlikely( requires_finished_instruction_reporting and memory_access_reporting_import ))
      memory_access_reporting_import->ReportFinishedInstruction(this->current_pc, this->next_pc);

    instruction_counter++;
    if (unlikely( instruction_counter_trap_reporting_import and (trap_on_instruction_counter == instruction_counter) ))
      instruction_counter_trap_reporting_import->ReportTrap(*this);
    
    this->TakeSVCException();
  }
  
  catch (exception::UndefInstrException const& undexc) {
    logger << DebugError << "Undefined instruction"
           << " pc: " << std::hex << current_pc << std::dec
           << ", cpsr: " << std::hex << cpsr.bits() << std::dec
           << " (" << cpsr << ")"
           << EndDebugError;
    this->Stop(-1);
  }
  
  catch (exception::Exception const& exc) {
    logger << DebugError << "Unimplemented exception (" << exc.what() << ")"
           << " pc: " << std::hex << current_pc << std::dec
           << ", cpsr: " << std::hex << cpsr.bits() << std::dec
           << " (" << cpsr << ")"
           << EndDebugError;

  }
}

/** Inject an intrusive read memory operation.
 *
 * @param addr the target address to read from
 * @param buffer the buffer to fill with the read memory
 * @param size the amount of bytes to read from starting from the target 
 *   address
 *
 * @return true on success, false otherwise
 */
bool 
CPU::InjectReadMemory( uint32_t addr, void* buffer, uint32_t size )
{
  uint32_t index = 0;
  uint32_t base_addr = (uint32_t)addr;
  uint32_t ef_addr;
  uint8_t* rbuffer = (uint8_t*)buffer;

  // if (likely(dcache.GetSize()))
  //   {
  //     while (size != 0)
  //       {
  //         ef_addr = base_addr + index;

  //         // Need to access the data cache before the memory subsystem
  //         uint32_t cache_tag = dcache.GetTag( ef_addr );
  //         uint32_t cache_set = dcache.GetSet( ef_addr );
  //         uint32_t cache_way;
  //         bool cache_hit =
  //           dcache.GetWay(cache_tag, cache_set, &cache_way) and
  //           dcache.GetValid(cache_set, cache_way);
  //         if (cache_hit)
  //           {
  //             // Read data from the cache
  //             uint32_t cache_index = dcache.GetIndex( ef_addr );
  //             uint32_t read_data_size = dcache.GetDataCopy( cache_set, cache_way, cache_index, size, &rbuffer[index] );
  //             index += read_data_size;
  //             size -= read_data_size;
  //           }
  //         else
  //           {
  //             // Read data from memory subsystem
  //             PrRead( ef_addr, &rbuffer[index], 1);
  //             index++;
  //             size--;
  //           }
  //       }
  //   }
  // else
    {
      // No data cache, just send request to the memory subsystem
      while (size != 0)
        {
          ef_addr = base_addr + index;
          PrRead(ef_addr, &rbuffer[index], 1);
          index++;
          size--;
        }
    }

  return true;
}

/** Inject an intrusive write memory operation
 *
 * @param addr the target address to write to
 * @param buffer a buffer containing the data to write
 * @param size the amount of bytes to write
 *
 * @return true on success, false otherwise
 */
bool 
CPU::InjectWriteMemory( uint32_t addr, void const* buffer, uint32_t size )
{
  uint32_t index = 0;
  uint32_t base_addr = (uint32_t)addr;
  uint32_t ef_addr;
  uint8_t const* wbuffer = (uint8_t const*)buffer;
  
  // if (likely(dcache.GetSize()))
  //   {
  //     while (size != 0)
  //       {
  //         ef_addr = base_addr + index;

  //         // Need to access the data cache before accessing the main memory
  //         uint32_t cache_tag = dcache.GetTag(ef_addr);
  //         uint32_t cache_set = dcache.GetSet(ef_addr);
  //         uint32_t cache_way;
  //         bool cache_hit =
  //           dcache.GetWay( cache_tag, cache_set, &cache_way ) and
  //           dcache.GetValid( cache_set, cache_way );
  //         if (cache_hit)
  //           {
  //             // Write data in the cache
  //             uint32_t cache_index = dcache.GetIndex( ef_addr );
  //             uint32_t write_data_size = dcache.SetData( cache_set, cache_way, cache_index, size, &wbuffer[index] );
  //             dcache.SetDirty( cache_set, cache_way, 1 );
  //             index += write_data_size;
  //             size -= write_data_size;
  //           }
  //         else
  //           {
  //             PrWrite( ef_addr, &wbuffer[index], 1 );
  //             index++;
  //             size--;
  //           }
  //       }
  //   }
  // else
    {
      // No data cache, just send the request to the memory subsystem
      while (size != 0)
        {
          ef_addr = base_addr + index;
          PrWrite( ef_addr, &wbuffer[index], 1 );
          index++;
          size--;
        }
    }

  return true;
}

/** Set/unset the reporting of finishing instructions.
 * 
 * @param report if true set the reporting of finishing instructions, 
 *   unset otherwise
 */
void 
CPU::RequiresFinishedInstructionReporting( bool report )
{
  requires_finished_instruction_reporting = report;
}

/** Set/unset the reporting of memory accesses.
 *
 * @param report if true/false sets/unsets the reporting of memory
 *        acesseses
 */
void
CPU::RequiresMemoryAccessReporting( bool report )
{
  requires_memory_access_reporting = report;
}

struct DebugMemAcc
{
  static bool const updateTLB = false;
};

/** Perform a non intrusive read access.
 * This method performs a normal read, but it does not change the state
 *   of the caches. It calls ExternalReadMemory if data not in cache.
 *
 * @param addr the address to read from
 * @param buffer the buffer to fill with the read data (must be allocated
 *   by the caller)
 * @param size the amount of data to read
 *
 * @return true on success, false otherwise
 */
bool 
CPU::ReadMemory( uint32_t addr, void* buffer, uint32_t size )
{
  bool status = true;
  uint32_t index = 0;
  uint32_t base_addr = (uint32_t)addr;
  uint32_t ef_addr;
  uint8_t* rbuffer = (uint8_t*)buffer;

  // bool cache_access = SCTLR::C.Get( this->sctlr ) and dcache.GetSize();
  // if (likely(cache_access))
  //   {
  //     while ((size != 0) and status)
  //       {
  //         ef_addr = base_addr + index;

  //         // Need to access the data cache before memory subsystem
  //         uint32_t cache_tag = dcache.GetTag(ef_addr);
  //         uint32_t cache_set = dcache.GetSet(ef_addr);
  //         uint32_t cache_way;
  //         bool cache_hit =
  //           dcache.GetWay( cache_tag, cache_set, &cache_way ) and
  //           dcache.GetValid( cache_set, cache_way );
  //         if (cache_hit)
  //           {
  //             // Read data from the cache
  //             uint32_t cache_index = dcache.GetIndex(ef_addr);
  //             uint32_t data_read_size = dcache.GetDataCopy( cache_set, cache_way, cache_index, size, &rbuffer[index] );
  //             index += data_read_size;
  //             size -= data_read_size;
  //           }
  //         else
  //           {
  //             // Read data from the memory subsystem
  //             status &= ExternalReadMemory( ef_addr, &rbuffer[index], 1 );
  //             index++;
  //             size--;
  //           }
  //       }
  //   }
  // else
    {
      // No data cache, just send request to the memory subsystem
      while ((size != 0) and status)
        {
          try {
            ef_addr = TranslateAddress<DebugMemAcc>( base_addr + index, true, false, 1 );
          } catch (unisim::component::cxx::processor::arm::exception::DataAbortException const& x) {
            status = false;
            break;
          }
          status &= ExternalReadMemory( ef_addr, &rbuffer[index], 1 );
          index++;
          size--;
        }
    }

  return status;
}

/** Perform a non intrusive write access.
 * This method perform a normal write, but it does not change the state
 *   of the caches. It calls ExternalWriteMemory in order to modify external
 *   ocurrences of the address.
 *
 * @param addr the address to write to
 * @param buffer the data to write
 * @param size the amount of data to write
 *
 * @return true on success, false otherwise
 */
bool 
CPU::WriteMemory( uint32_t addr, void const* buffer, uint32_t size )
{
  bool status = true;
  uint32_t index = 0;
  uint32_t base_addr = (uint32_t)addr;
  uint32_t ef_addr;
  uint8_t const* wbuffer = (uint8_t const*)buffer;

  // bool cache_access = SCTLR::C.Get( this->sctlr ) and dcache.GetSize();
  // if (cache_access)
  //   {
  //     while ((size != 0) and status)
  //       {
  //         ef_addr = base_addr + index;
          
  //         // Need to access the data cache before accessing the main memory
  //         uint32_t cache_tag = dcache.GetTag( ef_addr );
  //         uint32_t cache_set = dcache.GetSet( ef_addr );
  //         uint32_t cache_way;
  //         bool cache_hit =
  //           dcache.GetWay( cache_tag, cache_set, &cache_way ) and
  //           dcache.GetValid( cache_set, cache_way );
  //         if (cache_hit)
  //           {
  //             // Write data to the cache
  //             uint32_t cache_index = dcache.GetIndex( ef_addr );
  //             uint32_t data_read_size = dcache.SetData( cache_set, cache_way, cache_index, size, &wbuffer[index] );
  //             dcache.SetDirty( cache_set, cache_way, 1 );
  //             index += data_read_size;
  //             size -= data_read_size;
  //           }
  //         else
  //           {
  //             // Write data to the memory subsytem
  //             status &= ExternalWriteMemory( ef_addr, &wbuffer[index], 1);
  //             index++;
  //             size--;
  //           }
  //       }
  //   }
  // else
    {
      // No data cache, just send request to the memory subsystem
      while ((size != 0) and status)
        {
          try {
            ef_addr = TranslateAddress<DebugMemAcc>( base_addr + index, true, true, 1 );
          } catch (unisim::component::cxx::processor::arm::exception::DataAbortException const& x) {
            status = false;
            break;
          }
          status &= ExternalWriteMemory( ef_addr, &wbuffer[index], 1 );
          index++;
          size--;
        }
    }

  return status;
}

/** Disasm an instruction address.
 * Returns a string with the disassembling of the instruction found 
 *   at address addr.
 * 
 * @param addr the address of the instruction to disassemble
 * @param next_addr the address following the requested instruction
 *
 * @return the disassembling of the requested instruction address
 */
std::string 
CPU::Disasm(uint32_t addr, uint32_t& next_addr)
{
  std::stringstream buffer;
  if (cpsr.Get( T ))
    {
      buffer << "[THUMB2]";
    
      uint8_t insn_bytes[4];
      isa::thumb2::CodeType insn;
      isa::thumb2::Operation<CPU> *op = 0;
      if (not ReadMemory(addr, insn_bytes, 4))
        {
          buffer << "Could not read from memory";
          return buffer.str();
        }
  
      // Instruction fetch ignores "Endianness execution state bit"
      insn.str[0] = insn_bytes[0];
      insn.str[1] = insn_bytes[1];
      insn.str[2] = insn_bytes[2];
      insn.str[3] = insn_bytes[3];
      insn.size = 32;
    
      op = thumb_decoder.Decode(addr, insn);
      unsigned insn_length = op->GetLength();
      if (insn_length % 16) throw std::logic_error("Bad T2 instruction size");
    
      buffer << "0x";
      buffer << op->GetEncoding() << " ";
      op->disasm(*this, buffer);

      next_addr = addr + (insn_length / 8);
    } 
  else 
    {
      buffer << "[ARM32]";
    
      isa::arm32::Operation<CPU> * op = NULL;
      uint32_t insn;
      if (not ReadMemory(addr, &insn, 4))
        {
          buffer << "Could not read from memory";
          return buffer.str();
        }
      if (GetEndianness() == unisim::util::endian::E_BIG_ENDIAN)
        insn = BigEndian2Host(insn);
      else
        insn = LittleEndian2Host(insn);

      op = arm32_decoder.Decode(addr, insn);
      buffer << "0x" << std::hex;
      buffer.fill('0'); buffer.width(8); 
      buffer << op->GetEncoding() << std::dec << " ";
      op->disasm(*this, buffer);

      next_addr = addr + 4;
    }

  return buffer.str();
}
    
/** Exit system call.
 * The LinuxOS service calls this method when the program has finished.
 *
 * @param ret the exit cade sent by the exit system call.
 */
void 
CPU::PerformExit(int ret)
{
  // running = false;
  Stop(ret);
}

/** Refill the Instruction Prefetch Buffer from the memory system
 * (through the instruction cache if present).
 *
 * This method allows the user to prefetch instructions from the memory
 * system, that is, it tries to read from the pertinent caches and if
 * failed from the external memory system.
 * 
 * @param base_address the (physical) address of the required
 *     instruction that the prefetch instruction buffer should
 *     encompass, once the refill is complete.
 */
void 
CPU::RefillInsnPrefetchBuffer(uint32_t base_address)
{
  this->ipb_base_address = base_address;
  
  // bool cache_access = SCTLR::I.Get( this->sctlr ) and icache.GetSize();
  // if (likely(cache_access))
  //   {
  //     icache.read_accesses++;
  //     // check the instruction cache
  //     uint32_t cache_tag = icache.GetTag(base_address);
  //     uint32_t cache_set = icache.GetSet(base_address);
  //     uint32_t cache_way;
  //     // Check for a cache hit
  //     bool cache_hit =
  //       icache.GetWay(cache_tag, cache_set, &cache_way) and
  //       icache.GetValid(cache_set, cache_way);
      
  //     if (likely(cache_hit))
  //       {
  //         // cache hit
  //         icache.read_hits++;
  //       }
  //     else
  //       {
  //         // get a way to replace (no need to check valid and dirty
  //         // bits, the new data can be requested immediately)
  //         cache_way = icache.GetNewWay(cache_set);
  //         uint8_t *cache_data = 0;
  //         // when getting the physical data of the cache line
  //         icache.GetData(cache_set, cache_way, &cache_data);
  //         PrRead(base_address, cache_data, Cache::LINE_SIZE);
  //         icache.SetTag(cache_set, cache_way, cache_tag);
  //         icache.SetValid(cache_set, cache_way, 1);
  //       }
      
  //     // At this point data is in the cache, so we can read it from it
  //     uint32_t cache_index = icache.GetIndex(base_address);
  //     icache.GetDataCopy(cache_set, cache_way, cache_index, Cache::LINE_SIZE, &this->ipb_bytes[0]);

  //     if (unlikely(icache.power_estimator_import))
  //       icache.power_estimator_import->ReportReadAccess();
  //   }
  // else
    {
      // No instruction cache present, just request the insn to the
      // memory system.
      PrRead(base_address, &this->ipb_bytes[0], Cache::LINE_SIZE);
    }

  if (unlikely(requires_memory_access_reporting and memory_access_reporting_import))
    memory_access_reporting_import->
      ReportMemoryAccess(unisim::util::debug::MAT_READ, unisim::util::debug::MT_INSN, base_address, Cache::LINE_SIZE);
}

/** Reads ARM32 instructions from the memory system
 * This method allows the user to read instructions from the memory system,
 *   that is, it tries to read from the pertinent caches and if failed from
 *   the external memory system.
 * 
 * @param address the address to read data from
 * @param val the buffer to fill with the read data
 */
void 
CPU::ReadInsn(uint32_t address, unisim::component::cxx::processor::arm::isa::arm32::CodeType& insn)
{
  uint32_t base_address = TranslateAddress<SoftMemAcc>( address & -(Cache::LINE_SIZE), false, false, Cache::LINE_SIZE );
  uint32_t buffer_index = address % (Cache::LINE_SIZE);
  
  if (unlikely(ipb_base_address != base_address))
    {
      RefillInsnPrefetchBuffer( base_address );
    }
  
  uint32_t word;
  memcpy( &word, &ipb_bytes[buffer_index], 4 );
  // In ARMv7, instruction fetch ignores "Endianness execution state bit"
  insn = Target2Host(unisim::util::endian::E_LITTLE_ENDIAN, word);
}

/** Reads THUMB instructions from the memory system
 * This method allows the user to read instructions from the memory system,
 *   that is, it tries to read from the pertinent caches and if failed from
 *   the external memory system.
 * 
 * @param address the address to read data from
 * @param insn the resulting instruction word (output reference)
 */
void
CPU::ReadInsn(uint32_t address, unisim::component::cxx::processor::arm::isa::thumb2::CodeType& insn)
{
  uint32_t base_address = TranslateAddress<SoftMemAcc>( address & -(Cache::LINE_SIZE), false, false, Cache::LINE_SIZE );
  intptr_t buffer_index = address % (Cache::LINE_SIZE);
    
  if (unlikely(ipb_base_address != base_address))
    {
      RefillInsnPrefetchBuffer( base_address );
    }
  
  // In ARMv7, instruction fetch ignores "Endianness execution state bit"
  insn.str[0] = ipb_bytes[buffer_index+0];
  insn.str[1] = ipb_bytes[buffer_index+1];
  if (unlikely((buffer_index+2) >= Cache::LINE_SIZE)) {
    RefillInsnPrefetchBuffer( base_address + Cache::LINE_SIZE );
    buffer_index = intptr_t(-2);
  }
  insn.str[2] = ipb_bytes[buffer_index+2];
  insn.str[3] = ipb_bytes[buffer_index+3];
  insn.size = 32;
}

/** CallSupervisor
 * 
 *  This method is called by SWI/SVC instructions to handle software interrupts.
 */
void
CPU::CallSupervisor( uint16_t imm )
{
  if (this->linux_os_import) {
    // we are executing on linux emulation mode, use linux_os_import
    try {
      this->linux_os_import->ExecuteSystemCall(imm);
    }
    catch (exception::Exception const& e)
      {
        std::cerr << e.what() << std::endl;
        this->Stop( -1 );
      }
  } else {
    // we are executing on full system mode
    this->BaseCpu::CallSupervisor( imm );
  }
}

/** Breakpoint
 *  This method is called by BKPT instructions to handle breakpoints.
 */
void
CPU::BKPT( uint32_t imm )
{
  // we are executing on linux emulation mode
  throw "TODO: Generate a debug event: virtual BKPTInstrDebugEvent() method ?";
}

void
CPU::UndefinedInstruction( isa::arm32::Operation<CPU>* insn )
{
  std::ostringstream oss;
  insn->disasm( *this, oss );
  
  logger << DebugWarning << "Undefined instruction"
         << " @" << std::hex << current_pc << std::dec
         << ": " << oss.str()
         << EndDebugWarning;
  
  throw exception::UndefInstrException();
}

void
CPU::UndefinedInstruction( isa::thumb2::Operation<CPU>* insn )
{
  std::ostringstream oss;
  insn->disasm( *this, oss );
  
  logger << DebugWarning << "Undefined instruction"
         << " @" << std::hex << current_pc << std::dec
         << ": " << oss.str()
         << EndDebugWarning;
  
  throw exception::UndefInstrException();
}

CPU::TLB::TLB()
 : entry_count(0)
{
  for (unsigned idx = 0; idx < ENTRY_CAPACITY; ++idx)
    {
      keys[idx] = (idx << 5);
    }
}

template <class POLICY>
bool
CPU::TLB::GetTranslation( TransAddrDesc& tad, uint32_t mva )
{
  unsigned lsb, hit;
  uint32_t key;
  for (hit = 0; hit < entry_count; ++hit)
    {
      key = keys[hit];
      lsb = key & 31;
      if (((mva ^ key) >> lsb) == 0)
        break;
    }
  if (hit >= entry_count)
    return false; // TLB miss
  // TLB hit

  if (POLICY::updateTLB) {
    // MRU sort
    for (unsigned idx = hit; idx > 0; idx -= 1)
      keys[idx] = keys[idx-1];
    keys[0] = key;
  }
  
  // Address translation and attributes
  tad = vals[(key >> 5) & 127];
  uint32_t himask = uint32_t(-1) << lsb;
  tad.pa = (tad.pa & himask) | (mva & ~himask);
  return true;
}

void
CPU::TLB::AddTranslation( unsigned lsb, uint32_t mva, TransAddrDesc const& tad )
{
  if (entry_count >= ENTRY_CAPACITY)
    entry_count = ENTRY_CAPACITY - 1; // Erase LRU
  
  // Generate new key
  uint32_t key = ((mva >> lsb) << lsb) | (keys[entry_count] & 0xfe0) | (lsb & 31);
  
  // MRU sort
  for (unsigned idx = entry_count; idx > 0; idx -= 1)
    keys[idx] = keys[idx-1];
  keys[0] = key;

  entry_count += 1;
  
  vals[(key >> 5) & 127] = tad;
}

template <class POLICY>
void
CPU::TranslationTableWalk( TransAddrDesc& tad, uint32_t mva )
{
  //  // this is only called when the MMU is enabled
  //  TLBRecord result;
  //  AddressDescriptor l1descaddr;
  //  AddressDescriptor l2descaddr;
  // // variables for DAbort function
  // taketohypmode = FALSE;
  // IA = bits(40) UNKNOWN;
  // ipavalid = FALSE;
  // stage2 = FALSE;
  // LDFSRformat = FALSE;
  // s2fs1walk = FALSE;
  // // default setting of the domain
  // domain = bits(4) UNKNOWN;
  // // Determine correct Translation Table Base Register to use.
  
  struct EndianReader
  {
    uint8_t b[4]; bool be;
    typedef uint32_t w;
    uint32_t Get() const
    {
      if (be) return (w(b[0]) << 24) | (w(b[1]) << 16) | (w(b[2]) <<  8) | (w(b[3]) <<  0);
      else    return (w(b[0]) <<  0) | (w(b[1]) <<  8) | (w(b[2]) << 16) | (w(b[3]) << 24);
    }
    uint8_t* data() { return &b[0]; }
    EndianReader( bool _be ) : be( _be ) {}
  } erd( SCTLR::EE.Get( sctlr ) );
  
  uint32_t lsb = 0;
  //bool     nG;
  
  uint32_t n = TTBCR::N.Get( mmu.ttbcr ), ttbr;
  if ((mva >> 1) >> (n^31)) {
    ttbr = mmu.ttbr1;
    n = 0;
  } else {
    ttbr = mmu.ttbr0;
  }
  
  // Obtain First level descriptor.
  uint32_t l1descaddr = (((ttbr >> (14-n)) << (14-n)) | ((mva << n) >> (n+18))) & -4;
  PrRead( l1descaddr, erd.data(), 4 );
  uint32_t l1desc = erd.Get();
  switch (l1desc&3) {
  case 0: {
    // Fault, Reserved
    // TODO: Full DAbort_Translation 
    throw unisim::component::cxx::processor::arm::exception::DataAbortException();
  } break;
    
  case 1: {
    // Large page or Small page
    // Obtain Second level descriptor.
    uint32_t l2descaddr = ((l1desc & 0xfffffc00) | ((mva << 12) >> 22)) & -4;
    PrRead( l2descaddr, erd.data(), 4 );
    uint32_t l2desc = erd.Get();
    // Process Second level descriptor.
    if ((l2desc&3) == 0) {
      // TODO: Full DAbort_Translation 
      throw unisim::component::cxx::processor::arm::exception::DataAbortException();
    }
    //nG = (l2desc >> 11) & 1;
    if (l2desc & 2) {
      // Small page (4kB)
      lsb = 12;
      tad.pa = (l2desc & 0xfffff000) | (mva & 0x00000fff);
    }
    else {
      // Large page (64kB)
      tad.pa = (l2desc & 0xffff0000) | (mva & 0x0000ffff);
      lsb = 16;
    }
  } break;
    
  case 2: case 3: {
    // Section or Supersection
    //nG = (l1desc >> 17) & 1;
    
    if ((l1desc >> 18) & 1) {
      // Supersection (16MB)
      lsb = 24;
      if (RegisterField<20,4>().Get( l1desc ) or RegisterField<5,4>().Get( l1desc ))
        throw std::logic_error("LPAE not implemented"); /* Large 40-bit extended address */
      tad.pa = (l1desc & 0xff000000) | (mva & 0x00ffffff);
    }
    else {
      // Section (1MB)
      lsb = 20;
      tad.pa = (l1desc & 0xfff00000) | (mva & 0x000fffff);
    }
  } break;
    
  }
  
  // Try to add entry to TLB
  if (POLICY::updateTLB)
    tlb.AddTranslation( lsb, mva, tad );
}

template <class POLICY>
uint32_t
CPU::TranslateAddress( uint32_t va, bool ispriv, bool iswrite, unsigned size )
{
  // bits(32) mva;
  // bits(40) ia_in;
  // AddressDescriptor result;
  uint32_t mva = va; /* No FCSE translation in this model*/
  TransAddrDesc tad;
  
  // FirstStageTranslation
  if (SCTLR::M.Get( this->sctlr )) {
    // Stage 1 MMU enabled
    if (not tlb.GetTranslation<POLICY>( tad, mva ))
      TranslationTableWalk<POLICY>( tad, mva );
  } else {
    tad.pa = mva;
  }
  // TODO: Check permissions here
  
 return tad.pa;
}


/** Get the Internal representation of the CP15 Register
 * 
 * @param crn     the "crn" field of the instruction code
 * @param opcode1 the "opcode1" field of the instruction code
 * @param crm     the "crm" field of the instruction code
 * @param opcode2 the "opcode2" field of the instruction code
 * @return        an internal CP15Reg
 */
CPU::CP15Reg&
CPU::CP15GetRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 )
{
  switch (CP15ENCODE( crn, opcode1, crm, opcode2 ))
    {
    case CP15ENCODE( 0, 0, 0, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "CTR, Cache Type Register"; }
          uint32_t Read( BaseCpu& _cpu ) { return 0x8403c003; }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 0, 1, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "CCSIDR, Cache Size ID Registers"; }
          uint32_t Read( BaseCpu& _cpu ) {
            CPU& cpu = dynamic_cast<CPU&>( _cpu );
            switch (cpu.csselr) {
              /*              LNSZ      ASSOC       NUMSETS        POLICY      */
            case 0:  return (1 << 0) | (3 << 3) | ( 255 << 13) | (0b0110 << 28); /* L1 dcache */
            case 1:  return (1 << 0) | (3 << 3) | ( 255 << 13) | (0b0100 << 28); /* L1 icache */
            case 2:  return (2 << 0) | (7 << 3) | (1023 << 13) | (0b0110 << 28); /* L2 ucache */
            default: return 0;
            }
          }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 0, 1, 0, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "CLIDR, Cache Level ID Register"; }
          uint32_t Read( BaseCpu& _cpu ) {
            CPU& cpu = dynamic_cast<CPU&>( _cpu );
            uint32_t
              LoUU =   0b010, /* Level of Unification Uniprocessor  */
              LoC =    0b010, /* Level of Coherency */
              LoUIS =  0b010, /* Level of Unification Inner Shareable */
              Ctype2 = 0b100, /* Level2 => unified */
              Ctype1 = 0b011; /* Level1 => Separate instruction and data caches */
            return (LoUU << 27) | (LoC << 24) | (LoUIS << 21) | (Ctype2 << 3) | (Ctype1 << 0);
          }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 0, 0, 1, 4 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "ID_MMFR0, Memory Model Feature Register 0"; }
          uint32_t Read( BaseCpu& _cpu ) { return 0x3; /* vmsav7 */ }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 0, 2, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "CSSELR, Cache Size Selection Register"; }
          void Write( BaseCpu& _cpu, uint32_t value ) {
            dynamic_cast<CPU&>( _cpu ).csselr = value;
          }
          uint32_t Read( BaseCpu& _cpu ) { return dynamic_cast<CPU&>( _cpu ).csselr; }
        } x;
        return x;
      } break;
      
      /*******************************************
       * Memory protection and control registers *
       *******************************************/
    case CP15ENCODE( 2, 0, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "TTBR0, Translation Table Base Register 0"; }
          /* TODO: handle SBZ(DGP=0x00003fffUL)... */
          void Write( BaseCpu& _cpu, uint32_t value ) { dynamic_cast<CPU&>( _cpu ).mmu.ttbr0 = value; }
          uint32_t Read( BaseCpu& _cpu ) { return dynamic_cast<CPU&>( _cpu ).mmu.ttbr0; }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 2, 0, 0, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "TTBR1, Translation Table Base Register 1"; }
          /* TODO: handle SBZ(DGP=0x00003fffUL)... */
          void Write( BaseCpu& _cpu, uint32_t value ) { dynamic_cast<CPU&>( _cpu ).mmu.ttbr1 = value; }
          uint32_t Read( BaseCpu& _cpu ) { return dynamic_cast<CPU&>( _cpu ).mmu.ttbr1; }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 2, 0, 0, 2 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "TTBCR, Translation Table Base Control Register"; }
          void Write( BaseCpu& _cpu, uint32_t value ) { dynamic_cast<CPU&>( _cpu ).mmu.ttbcr = value; }
          uint32_t Read( BaseCpu& _cpu ) { return dynamic_cast<CPU&>( _cpu ).mmu.ttbcr; }
        } x;
        return x;
      } break;

      
    case CP15ENCODE( 3, 0, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DACR, Domain Access Control Register"; }
          uint32_t Read( BaseCpu& _cpu ) { return dynamic_cast<CPU&>( _cpu ).mmu.dacr; }
          void Write( BaseCpu& _cpu, uint32_t value ) {
            dynamic_cast<CPU&>( _cpu ).mmu.dacr = value;
            _cpu.logger << DebugInfo << "DACR <- " << std::hex << value << std::dec << EndDebugInfo;
          }
        } x;
        return x;
      } break;
      
      /***************************************************************
       * Cache maintenance, address translation, and other functions *
       ***************************************************************/
    case CP15ENCODE( 7, 0, 5, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "ICIALLU, Invalidate all instruction caches to PoU"; }
          void Write( BaseCpu& _cpu, uint32_t value ) {
            /* No cache, basically nothing to do */
            //_cpu.logger << DebugWarning << "ICIALLU <- " << std::hex << value << std::dec << EndDebugWarning;
          }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 7, 0, 5, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "ICIMVAU, Clean data* cache line by MVA to PoU"; }
          void Write( BaseCpu& _cpu, uint32_t value ) {
            /* No cache, basically nothing to do */
            //_cpu.logger << DebugWarning << "ICIMVAU <- " << std::hex << value << std::dec << EndDebugWarning;
          }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 7, 0, 5, 6 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "BPIALL, Invalidate all branch predictors"; }
          void Write( BaseCpu& _cpu, uint32_t value ) {
            /* No branch predictor, basically nothing to do */
            //_cpu.logger << DebugWarning << "BPIALL <- " << std::hex << value << std::dec << EndDebugWarning;
          }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 7, 0, 10, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DCCMVAC, Clean data* cache line by MVA to PoC"; }
          void Write( BaseCpu& _cpu, uint32_t value ) {
            /* No cache, basically nothing to do */
            //_cpu.logger << DebugWarning << "DCCMVAC <- " << std::hex << value << std::dec << EndDebugWarning;
          }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 7, 0, 11, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DCCMVAU, Clean data* cache line by MVA to PoU"; }
          void Write( BaseCpu& _cpu, uint32_t value ) {
            /* No cache, basically nothing to do */
            //_cpu.logger << DebugWarning << "DCCMVAU <- " << std::hex << value << std::dec << EndDebugWarning;
          }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 7, 0, 14, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DCCIMVAC, Clean and invalidate data cache line by MVA to PoC"; }
          void Write( BaseCpu& _cpu, uint32_t value ) {
            /* No cache, basically nothing to do */
            //_cpu.logger << DebugWarning << "DCCIMVAC <- " << std::hex << value << std::dec << EndDebugWarning;
          }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 7, 0, 14, 2 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DCCISW, Clean and invalidate [d|u]cache line by set/way"; }
          void Write( BaseCpu& _cpu, uint32_t value ) {
            /* No cache, basically nothing to do */
            //_cpu.logger << DebugWarning << "DCCISW <- " << std::hex << value << std::dec << EndDebugWarning;
          }
        } x;
        return x;
      } break;
      
      /******************************
       * TLB maintenance operations *
       ******************************/
    case CP15ENCODE( 8, 0, 7, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "TLBIALL, invalidate unified TLB"; }
          void Write( BaseCpu& _cpu, uint32_t value ) {
            CPU& cpu( dynamic_cast<CPU&>( _cpu ) );
            cpu.logger << DebugInfo << "TLBIALL" << EndDebugInfo;
            cpu.tlb.Invalidate();
          }
        } x;
        return x;
      } break;
      
      /**********************************************
       * Memory remapping and TLB control registers *
       **********************************************/
    case CP15ENCODE( 10, 0, 2, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "PRRR, Primary Region Remap Register"; }
          void Write( BaseCpu& _cpu, uint32_t value ) { dynamic_cast<CPU&>( _cpu ).mmu.prrr = value; }
          uint32_t Read( BaseCpu& _cpu ) { return dynamic_cast<CPU&>( _cpu ).mmu.prrr; }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 10, 0, 2, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "NMRR, Normal Memory Remap Register"; }
          void Write( BaseCpu& _cpu, uint32_t value ) { dynamic_cast<CPU&>( _cpu ).mmu.nmrr = value; }
          uint32_t Read( BaseCpu& _cpu ) { return dynamic_cast<CPU&>( _cpu ).mmu.nmrr; }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 13, 0, 0, 3 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "TPIDRURO, Thread Id Privileged Read Write Only Register"; }
          unsigned RequiredPL() { return 0; /* Doesn't requires priviledges */ }
          uint32_t Read( BaseCpu& _cpu ) {
            CPU& cpu( dynamic_cast<CPU&>( _cpu ) );
            /* TODO: the following only works in linux os
             * emulation. We should really access the TPIDRURO
             * register. */
            return cpu.MemRead32( 0xffff0ff0 );
          }
        } x;
        return x;
      } break;
      
    }
  
  // Fall back to base cpu CP15 registers
  return this->BaseCpu::CP15GetRegister( crn, opcode1, crm, opcode2 );
}

} // end of namespace armemu

template struct unisim::component::cxx::processor::arm::CPU<unisim::component::cxx::processor::arm::armemu::ARMv7emu>;

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
