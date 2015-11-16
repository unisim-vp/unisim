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
#include <cassert>
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
  , Client<LinuxOS>(name, parent)
  , memory_access_reporting_control_export("memory-access-reporting-control-export", this)
  , memory_access_reporting_import("memory-access-reporting-import", this)
  , disasm_export("disasm-export", this)
  , memory_injection_export("memory-injection-export", this)
  , memory_export("memory-export", this)
  , debug_control_import("debug-control-import", this)
  , symbol_table_lookup_import("symbol-table-lookup-import", this)
  , exception_trap_reporting_import("exception-trap-reporting-import", this)
  , instruction_counter_trap_reporting_import("instruction-counter-trap-reporting-import", this)
  , linux_os_import("linux-os-import", this)
  , requires_finished_instruction_reporting(true)
  , requires_memory_access_reporting(true)
  , icache("icache", this)
  , dcache("dcache", this)
  , arm32_decoder()
  , thumb_decoder()
  , instruction_counter(0)
  , voltage(0)
  , verbose(0)
  , trap_on_instruction_counter(0)
  , sctlr_rstval( this->BaseCpu::sctlr )
  , param_sctlr_rstval("SCTLR", this, this->sctlr, "The processor reset value of the SCTLR register.")
  , param_cpu_cycle_time_ps("cpu-cycle-time-ps", this, cpu_cycle_time_ps, "The processor cycle time in picoseconds.")
  , param_voltage("voltage", this, voltage, "The processor voltage in mV.")
  , param_verbose("verbose", this, verbose, "Activate the verbose system (0 = inactive, different than 0 = active).")
  , param_trap_on_instruction_counter("trap-on-instruction-counter", this, trap_on_instruction_counter,
                                      "Produce a trap when the given instruction count is reached.")
  , stat_instruction_counter("instruction-counter", this, instruction_counter, "Number of instructions executed.")
  , ipb_base_address( -1 )
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
  logger << DebugInfo << "CPU::BeginSetup" << EndDebugInfo;
  if (verbose)
    logger << DebugInfo << "Verbose activated." << EndDebugInfo;

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

    if (icache.power_mode_import)
      {
        min_cycle_time =  icache.power_mode_import->GetMinCycleTime();
        il1_def_voltage = icache.power_mode_import->GetDefaultVoltage();
      }
    if (dcache.power_mode_import)
      {
        if ( dcache.power_mode_import->GetMinCycleTime() > min_cycle_time )
          min_cycle_time = dcache.power_mode_import->GetMinCycleTime();
        dl1_def_voltage =  dcache.power_mode_import->GetDefaultVoltage();
      }

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
        if ( icache.power_mode_import )
          logger << std::endl << "  - instruction cache voltage = " << il1_def_voltage << " mV";
        if ( dcache.power_mode_import )
          logger << std::endl << "  - data cache voltage = " << dl1_def_voltage << " mV";
        logger << EndDebugWarning;
      }
  
    if (icache.power_mode_import)
      icache.power_mode_import->SetPowerMode(cpu_cycle_time_ps, voltage);
    if (dcache.power_mode_import)
      dcache.power_mode_import->SetPowerMode(cpu_cycle_time_ps, voltage);

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
  
  return true;
}

/** Performs a prefetch access.
 *
 * @param addr the address of the memory prefetch access
 */
void
CPU::PerformPrefetchAccess( uint32_t addr )
{
  if ( likely(dcache.GetSize()) )
    {
      dcache.prefetch_accesses++;
      uint32_t cache_tag = dcache.GetTag(addr);
      uint32_t cache_set = dcache.GetSet(addr);
      uint32_t cache_way;
      bool cache_hit = false;
      if ( dcache.GetWay(cache_tag, cache_set, &cache_way) )
        {
          if ( dcache.GetValid(cache_set, cache_way) )
            {
              // the access is a hit, nothing needs to be done
              cache_hit = true;
            }
        }
      // if the access was a miss, data needs to be fetched from main
      //   memory and placed into the cache
      if ( likely(!cache_hit) )
        {
          // get a way to replace
          cache_way = dcache.GetNewWay(cache_set);
          // get the valid and dirty bits from the way to replace
          bool cache_valid = dcache.GetValid(cache_set,
                                             cache_way);
          bool cache_dirty = dcache.GetDirty(cache_set,
                                             cache_way);
          if ( cache_valid & cache_dirty )
            {
              // the cache line to replace is valid and dirty so it needs
              //   to be sent to the main memory
              uint8_t *rep_cache_data = 0;
              uint32_t rep_cache_address =
                dcache.GetBaseAddress(cache_set,
                                      cache_way);
              dcache.GetData(cache_set, cache_way,
                             &rep_cache_data);
              PrWrite(rep_cache_address, rep_cache_data,
                      dcache.LINE_SIZE);
            }
          // the new data can be requested
          uint8_t *cache_data = 0;
          uint32_t cache_address =
            dcache.GetBaseAddressFromAddress(addr);
          // when getting the data we get the pointer to the cache line
          //   containing the data, so no need to write the cache
          //   afterwards
          uint32_t cache_line_size = dcache.GetData(cache_set,
                                                    cache_way, &cache_data);
          PrRead(cache_address, cache_data,
                 cache_line_size);
          dcache.SetTag(cache_set, cache_way, cache_tag);
          dcache.SetValid(cache_set, cache_way, 1);
          dcache.SetDirty(cache_set, cache_way, 0);
        }
      else
        {
          dcache.prefetch_hits++;
        }
      if ( unlikely(dcache.power_estimator_import != 0) )
        dcache.power_estimator_import->ReportReadAccess();
    }
  else
    {
      /* it is just a cache prefetch, ignore the request if there is 
       * no cache */
    }
  /* CHECK: should we report a memory access for a prefetch???? */
}

/** Performs a write access.
 * @param addr the address of the memory write access
 * @param size the size of the memory write access
 * @param value the value of the memory write access
 */
void
CPU::PerformWriteAccess( uint32_t addr, uint32_t size, uint32_t value )
{
  uint32_t write_addr = addr;
  uint8_t data[4];

  if (size > 4) throw 0; // should never happen

  if (GetEndianness() == unisim::util::endian::E_BIG_ENDIAN) {
    // fix the write address according to request size when big endian
    write_addr ^= ((-size) & 3);
    // TODO: need to check that because it seems astonishingly incorrect
    uint32_t shifter = value;
    for (int byte = size; --byte >= 0;)
      { data[byte] = shifter; shifter >>= 8; }
  } else {
    uint32_t shifter = value;
    for (int byte = 0; byte < int( size ); ++byte)
      { data[byte] = shifter; shifter >>= 8; }
  }

  if ( likely(dcache.GetSize()) )
    {
      dcache.write_accesses++;
      uint32_t cache_tag = dcache.GetTag(write_addr);
      uint32_t cache_set = dcache.GetSet(write_addr);
      uint32_t cache_way;
      bool cache_hit = false;
      if ( dcache.GetWay(cache_tag, cache_set, &cache_way) )
        {
          if ( dcache.GetValid(cache_set, cache_way) != 0 )
            {
              // the access is a hit
              cache_hit = true;
            }
        }
      // if the access was a hit the data needs to be written into
      //   the cache, if the access was a miss the data needs to be
      //   written into memory, but the cache doesn't need to be updated
      if ( likely(cache_hit) )
        {
          dcache.write_hits++;
          uint32_t cache_index = dcache.GetIndex(write_addr);
          dcache.SetData(cache_set, cache_way, cache_index,
                         size, data);
          dcache.SetDirty(cache_set, cache_way, 1);
        }
      else
        {
          PrWrite(write_addr, data, size);
        }

      if ( unlikely(dcache.power_estimator_import != 0) )
        dcache.power_estimator_import->ReportWriteAccess();
    }
  else // there is no data cache
    {
      // there is no data cache, so just send the request to the
      //   memory interface
      PrWrite(write_addr, data, size);
    }

  /* report read memory access if necessary */
  ReportMemoryAccess(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, addr, size);
}

/** Performs a read access.
 * @param addr the address of the memory read access
 * @param size the size of the memory read access
 * @param _signed the nature of the memory read access (signed or unsigned)
 */
uint32_t
CPU::PerformReadAccess(	uint32_t addr, uint32_t size, bool _signed )
{
  uint32_t read_addr = addr & ~(uint32_t)(size - 1);
  uint8_t data[4];

  if (size > 4) throw 0;
  //uint32_t misalignment = addr & (size-1);

  // fix the read address depending on the request size and endianess
  if (GetEndianness() == unisim::util::endian::E_BIG_ENDIAN) {
    // TODO: need to check that because it seems astonishingly
    // incorrect to perform address masking and endianness conversion
    read_addr ^= ((-size) & 3);
  }

  if ( likely(dcache.GetSize()) )
    {
      dcache.read_accesses++;
      uint32_t cache_tag = dcache.GetTag(read_addr);
      uint32_t cache_set = dcache.GetSet(read_addr);
      uint32_t cache_way;
      bool cache_hit = false;
      if ( dcache.GetWay(cache_tag, cache_set, &cache_way) )
        {
          if ( dcache.GetValid(cache_set, cache_way) )
            {
              // the access is a hit, nothing needs to be done
              cache_hit = true;
            }
        }
      // if the access was a miss, data needs to be fetched from main
      //   memory and placed into the cache
      if ( unlikely(!cache_hit) )
        {
          // get a way to replace
          cache_way = dcache.GetNewWay(cache_set);
          // get the valid and dirty bits from the way to replace
          uint8_t cache_valid = dcache.GetValid(cache_set,
                                                cache_way);
          uint8_t cache_dirty = dcache.GetDirty(cache_set,
                                                cache_way);

          if ( (cache_valid != 0) & (cache_dirty != 0) )
            {
              // the cache line to replace is valid and dirty so it needs
              //   to be sent to the main memory
              uint8_t *rep_cache_data = 0;
              uint32_t rep_cache_address =
                dcache.GetBaseAddress(cache_set, cache_way);
              dcache.GetData(cache_set, cache_way, &rep_cache_data);
              PrWrite(rep_cache_address, rep_cache_data,
                      dcache.LINE_SIZE);
            }
          // the new data can be requested
          uint8_t *cache_data = 0;
          uint32_t cache_address =
            dcache.GetBaseAddressFromAddress(read_addr);
          // when getting the data we get the pointer to the cache line
          //   containing the data, so no need to write the cache
          //   afterwards
          uint32_t cache_line_size = dcache.GetData(cache_set,
                                                    cache_way, &cache_data);
          PrRead(cache_address, cache_data,
                 cache_line_size);
          dcache.SetTag(cache_set, cache_way, cache_tag);
          dcache.SetValid(cache_set, cache_way, 1);
          dcache.SetDirty(cache_set, cache_way, 0);
        }
      else
        {
          // cache hit
          dcache.read_hits++;
        }

      // at this point the data is in the cache, we can read it from the
      //   cache
      uint32_t cache_index = dcache.GetIndex(read_addr);
      uint8_t* ptr;
      (void)dcache.GetData(cache_set, cache_way, cache_index, size, &ptr);
      memcpy( &data[0], ptr, size );
    }
  else // there is no data cache
    {
      // just read the data from the memory system
      PrRead(read_addr, &data[0], size);
    }

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

  if ( likely(dcache.GetSize()) )
    if ( unlikely(dcache.power_estimator_import != 0) )
      dcache.power_estimator_import->ReportReadAccess();

  /* report read memory access if necessary */
  ReportMemoryAccess(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, addr, size);
      
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
      DebugControl<uint32_t>::DebugCommand dbg_cmd;

      do 
        {
          dbg_cmd = debug_control_import->FetchDebugCommand( insn_addr );
  
          if (likely(dbg_cmd == DebugControl<uint32_t>::DBG_STEP)) 
            {
              /* Nothing to do */
              break;
            }
          if (dbg_cmd == DebugControl<uint32_t>::DBG_SYNC) 
            {
              // Sync();
              continue;
            }

          if (dbg_cmd == DebugControl<uint32_t>::DBG_KILL) {
            Stop(0);
          }
          if(dbg_cmd == DebugControl<uint32_t>::DBG_RESET) {
            // TODO : memory_interface->Reset(); 
          }
        } while(1);
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
      if (insn_length % 16) throw 0;
    
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
  
  catch (exception::SynchronousAbort sa) {
    /* Instruction didn't execute has expected. TODO: ensure that an
     * exception handler handles it (e.g. using a synchronous abort
     * bit in the exception vector). */
  }
  
  /* Handle any exception that may  have occured */
  if (unlikely(this->exception))
    {
      if (unlikely(this->linux_os_import)) {
        // we are executing on linux emulation mode, no exception
        // handling in this mode.
        logger << DebugError
               << "An exception has been found, this should never happen when simulating at user level."
               << EndDebugError;
        Stop(-1);
        return;
      }
      // bool exception_occurred = 
      HandleException();
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
CPU::InjectReadMemory( uint32_t addr, void *buffer, uint32_t size )
{
  uint32_t index = 0;
  uint32_t base_addr = (uint32_t)addr;
  uint32_t ef_addr;

  if ( likely(dcache.GetSize()) )
    {
      while (size != 0)
        {
          // need to access the data cache before accessing the main memory
          ef_addr = base_addr + index;

          uint32_t cache_tag = dcache.GetTag(ef_addr);
          uint32_t cache_set = dcache.GetSet(ef_addr);
          uint32_t cache_way;
          bool cache_hit = false;
          if ( dcache.GetWay(cache_tag, cache_set, &cache_way) )
            {
              if ( dcache.GetValid(cache_set, cache_way) )
                {
                  // the cache access is a hit, data can be simply read from 
                  //   the cache
                  uint32_t cache_index = dcache.GetIndex(ef_addr);
                  uint32_t read_data_size =
                    dcache.GetDataCopy(cache_set, cache_way, 
                                       cache_index, size, 
                                       &(((uint8_t *)buffer)[index]));
                  index += read_data_size;
                  size -= read_data_size;
                  cache_hit = true;
                }
            }
          if ( !cache_hit )
            {
              PrRead(ef_addr,
                     &(((uint8_t *)buffer)[index]),
                     1);
              index++;
              size--;
            }
        }
    }
  else
    {
      // no data cache on this system, just send request to the memory
      //   subsystem
      while ( size != 0 )
        {
          ef_addr = base_addr + index;
          PrRead(ef_addr,
                 &(((uint8_t *)buffer)[index]),
                 1);
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
CPU::InjectWriteMemory( uint32_t addr, const void* buffer, uint32_t size )
{
  uint32_t index = 0;
  uint32_t base_addr = (uint32_t)addr;
  uint32_t ef_addr;
  
  if ( likely(dcache.GetSize()) )
    {
      // access memory while using the linux_os_import
      //   tcm is ignored
      while ( size != 0 )
        {
          // need to access the data cache before accessing the main memory
          ef_addr = base_addr + index;

          uint32_t cache_tag = dcache.GetTag(ef_addr);
          uint32_t cache_set = dcache.GetSet(ef_addr);
          uint32_t cache_way;
          bool cache_hit = false;
          if ( dcache.GetWay(cache_tag, cache_set, &cache_way) )
            {
              if ( dcache.GetValid(cache_set, cache_way) )
                {
                  // the cache access is a hit, data can be simply read from 
                  //   the cache
                  uint32_t cache_index = dcache.GetIndex(ef_addr);
                  uint32_t write_data_size =
                    dcache.SetData(cache_set, cache_way, 
                                   cache_index, size, 
                                   &(((uint8_t *)buffer)[index]));
                  dcache.SetDirty(cache_set, cache_way, 1);
                  index += write_data_size;
                  size -= write_data_size;
                  cache_hit = true;
                }
            }
          if ( !cache_hit )
            {
              PrWrite(ef_addr,
                      &(((uint8_t *)buffer)[index]),
                      1);
              index++;
              size--;
            }
        }
    }
  else
    {
      // there is no data cache in this system just send the request to
      //   the memory subsystem
      while ( size != 0 )
        {
          ef_addr = base_addr + index;
          PrWrite(ef_addr,
                  &(((uint8_t *)buffer)[index]),
                  1);
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

  if ( likely(dcache.GetSize()) )
    {
      // non intrusive access with linux support
      while (size != 0 && status)
        {
          // need to access the data cache before accessing the main
          //   memory
          ef_addr = base_addr + index;

          uint32_t cache_tag = dcache.GetTag(ef_addr);
          uint32_t cache_set = dcache.GetSet(ef_addr);
          uint32_t cache_way;
          bool cache_hit = false;
          if ( dcache.GetWay(cache_tag, cache_set, &cache_way) )
            {
              if ( dcache.GetValid(cache_set, cache_way) )
                {
                  // the cache access is a hit, data can be simply read
                  //   from the cache
                  uint32_t cache_index =
                    dcache.GetIndex(ef_addr);
                  uint32_t data_read_size =
                    dcache.GetDataCopy(cache_set,
                                       cache_way, cache_index, size,
                                       &(((uint8_t *)buffer)[index]));
                  index += data_read_size;
                  size -= data_read_size;
                  cache_hit = true;
                }
            }
          if ( !cache_hit )
            {
              status = status &&
                ExternalReadMemory(ef_addr,
                                   &(((uint8_t *)buffer)[index]),
                                   1);
              index++;
              size--;
            }
        }
    }
  else
    {
      // there is no data cache in this system just perform the request
      //   to the memory subsystem
      while ( size != 0 && status )
        {
          ef_addr = base_addr + index;
          status = status &&
            ExternalReadMemory(ef_addr,
                               &(((uint8_t *)buffer)[index]),
                               1);
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
CPU::WriteMemory(uint32_t addr, 
    const void *buffer, 
    uint32_t size)
{
  bool status = true;
  uint32_t index = 0;
  uint32_t base_addr = (uint32_t)addr;
  uint32_t ef_addr;

  if ( dcache.GetSize() )
    {
      // non intrusive access with linux support
      while (size != 0 && status)
        {
          // need to access the data cache before accessing the main memory
          ef_addr = base_addr + index;

          uint32_t cache_tag = dcache.GetTag(ef_addr);
          uint32_t cache_set = dcache.GetSet(ef_addr);
          uint32_t cache_way;
          bool cache_hit = false;
          if ( dcache.GetWay(cache_tag, cache_set, &cache_way) )
            {
              if ( dcache.GetValid(cache_set, cache_way) )
                {
                  // the cache access is a hit, data can be simply written to 
                  //   the cache
                  uint32_t cache_index = dcache.GetIndex(ef_addr);
                  uint32_t data_read_size =
                    dcache.SetData(cache_set, cache_way, cache_index, 
                                   size, &(((uint8_t *)buffer)[index]));
                  dcache.SetDirty(cache_set, cache_way, 1);
                  index += data_read_size;
                  size -= data_read_size;
                  cache_hit = true;
                }
            }
          if ( !cache_hit )
            {
              status = status &&
                ExternalWriteMemory(ef_addr,
                                    &(((uint8_t *)buffer)[index]),
                                    1);
              index++;
              size--;
            }
        }
    }
  else
    {
      // there is no data cache in this system, just write the data to
      //   the memory subsystem
      while ( size != 0 && status )
        {
          ef_addr = base_addr + index;
          status = status &&
            ExternalWriteMemory(ef_addr,
                                &(((uint8_t *)buffer)[index]),
                                1);
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
CPU::Disasm(uint32_t addr, uint32_t &next_addr)
{
  std::stringstream buffer;
  if (cpsr.Get( T ))
    {
      buffer << "[THUMB2]";
    
      uint8_t insn_bytes[4];
      isa::thumb2::CodeType insn;
      isa::thumb2::Operation<CPU> *op = 0;
      if (!ReadMemory(addr, insn_bytes, 4)) 
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
      if (insn_length % 16) throw 0;
    
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
      if (!ReadMemory(addr, &insn, 4)) 
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
 * @param address the address of the requiered instruction that the
 *                prefetch instruction buffer should encompass, once
 *                the refill is complete.
 */
void 
CPU::RefillInsnPrefetchBuffer(uint32_t base_address)
{
  this->ipb_base_address = base_address;
  
  if (likely(icache.GetSize()))
    {
      icache.read_accesses++;
      // check the instruction cache
      uint32_t cache_tag = icache.GetTag(base_address);
      uint32_t cache_set = icache.GetSet(base_address);
      uint32_t cache_way;
      // Check for a cache hit
      bool cache_hit =
        icache.GetWay(cache_tag, cache_set, &cache_way) and
        icache.GetValid(cache_set, cache_way);
      
      if (likely(cache_hit))
        {
          // cache hit
          icache.read_hits++;
        }
      else
        {
          // get a way to replace (no need to check valid and dirty
          // bits, the new data can be requested immediately)
          cache_way = icache.GetNewWay(cache_set);
          uint8_t *cache_data = 0;
          // when getting the physical data of the cache line
          icache.GetData(cache_set, cache_way, &cache_data);
          PrRead(base_address, cache_data, Cache::LINE_SIZE);
          icache.SetTag(cache_set, cache_way, cache_tag);
          icache.SetValid(cache_set, cache_way, 1);
        }
      
      // At this point data is in the cache, so we can read it from it
      uint32_t cache_index = icache.GetIndex(base_address);
      icache.GetDataCopy(cache_set, cache_way, cache_index, Cache::LINE_SIZE, &this->ipb_bytes[0]);

      if ( unlikely(icache.power_estimator_import != 0) )
        icache.power_estimator_import->ReportReadAccess();
    }
  else
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
  uint32_t base_address = address & -(Cache::LINE_SIZE);
  uint32_t buffer_index = address % (Cache::LINE_SIZE);
  
  if (unlikely(ipb_base_address != base_address))
    {
      RefillInsnPrefetchBuffer( base_address );
    }
  
  uint32_t word = *((uint32_t*)(&ipb_bytes[buffer_index]));
  insn = Target2Host(GetEndianness(), word);
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
  uint32_t base_address = address & -(Cache::LINE_SIZE);
  intptr_t buffer_index = address % (Cache::LINE_SIZE);
    
  if (unlikely(ipb_base_address != base_address))
    {
      RefillInsnPrefetchBuffer( base_address );
    }
  
  // Instruction fetch ignores "Endianness execution state bit"
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

/** Software Interrupt
 *  This method is called by SWI instructions to handle software interrupts.
 */
void
CPU::SWI( uint32_t imm )
{
  if (this->linux_os_import) {
    // we are executing on linux emulation mode, use linux_os_import
    try {
      this->linux_os_import->ExecuteSystemCall(imm);
    }
    catch(std::exception &e)
      {
        std::cerr << e.what() << std::endl;
        this->Stop( -1 );
      }
  } else {
    // we are executing on full system mode
    this->MarkVirtualExceptionVector(unisim::component::cxx::processor::arm::exception::SWI);
  }
}

/** Breakpoint
 *  This method is called by BKPT instructions to handle breakpoints.
 */
void
CPU::BKPT( uint32_t imm )
{
  // we are executing on linux emulation mode
  // what should we do with this kind of call? ignore it
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
    case CP15ENCODE( 13, 0, 0, 3 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "TPIDRURO, Thread Id Privileged Read Write Only Register"; }
          void Write( BaseCpu& cpu, uint32_t value ) { throw 0; }
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
