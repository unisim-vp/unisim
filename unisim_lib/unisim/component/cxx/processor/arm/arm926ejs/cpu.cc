/*
 *  Copyright (c) 2010-2015,
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
#include <unisim/component/cxx/processor/arm/arm926ejs/cpu.hh>
#include <unisim/component/cxx/processor/arm/cpu.tcc>
#include <unisim/component/cxx/processor/arm/arm926ejs/tlb.hh>
#include <unisim/component/cxx/processor/arm/memory_op.hh>
#include <unisim/component/cxx/processor/arm/cpu.hh>
#include <unisim/util/likely/likely.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <sstream>
#include <string>
#include <string.h>


#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif // __STDC_CONSTANT_MACROS
#include <assert.h>
#include <stdint.h>

#if (defined(__GNUC__) && (__GNUC__ >= 3))
#define INLINE __attribute__((always_inline))
#else
#define INLINE
#endif

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace arm926ejs {

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
  : Object(name, parent)
  , unisim::component::cxx::processor::arm::CPU<ARM926ejs>(name, parent)
  , Service<MemoryAccessReportingControl>(name, parent)
  , Client<MemoryAccessReporting<uint64_t> >(name, parent)
  , Service<MemoryInjection<uint64_t> >(name, parent)
  , Client<DebugControl<uint64_t> >(name, parent)
  , Client<TrapReporting>(name, parent)
  , Service<Disassembly<uint64_t> >(name, parent)
  , Service<Memory<uint64_t> >(name, parent)
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
  , ltlb("lockdown-tlb", this)
  , tlb("tlb", this)
  , fcseidr(0)
  , icache("icache", this)
  , dcache("dcache", this)
  , arm32_decoder()
  , thumb_decoder()
  , instruction_counter(0)
  , cur_instruction_address(0)
  , verbose(0)
  , trap_on_instruction_counter(0)
  , trap_on_exception(false)
  , requires_memory_access_reporting(true)
  , requires_finished_instruction_reporting(true)
  , param_cpu_cycle_time_ps("cpu-cycle-time-ps", this, cpu_cycle_time_ps, "The processor cycle time in picoseconds.")
  , param_voltage("voltage", this, voltage, "The processor voltage in mV.")
  , param_verbose("verbose", this, verbose, "Activate the verbose system (0 = inactive, different than 0 = active).")
  , param_trap_on_instruction_counter("trap-on-instruction-counter", this, trap_on_instruction_counter,
                                      "Produce a trap when the given instruction count is reached.")
  , param_trap_on_exception("trap-on-exception", this, trap_on_exception, "Produce a trap when an exception occurs.")
  , stat_instruction_counter("instruction-counter", this, instruction_counter, "Number of instructions executed.")
  , stat_cur_instruction_address("cur-instruction-address", this, cur_instruction_address,
                                 "Address of the instruction currently being executed.")
  , num_data_prefetches(0)
  , num_data_reads(0)
  , num_data_writes(0)
  , num_insn_reads(0)
  , stat_num_data_prefetches("num-data-prefetches", this,  num_data_prefetches, "Number of data prefetches issued to the memory system.")
  , stat_num_data_reads("num-data-reads", this, num_data_reads, "Number of data reads issued to the memory system.")
  , stat_num_data_writes("num-data-writes", this, num_data_writes, "Number of data writes issued to the memory system.")
  , stat_num_insn_reads("num-insn-reads", this, num_insn_reads, "Number of instruction reads issued to the memory system.")
{
  // Init the processor at SUPERVISOR mode
  cpsr.Set(M, SUPERVISOR_MODE);
  // Disable fast and normal interruptions
  cpsr.Set(I, 1);
  cpsr.Set(F, 1);

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
 * This method is required for all UNISIM objects and will be called during
 *   the setup phase.
 * 
 * @return true on success, false otherwise
 */
bool 
CPU::BeginSetup()
{
  logger << DebugInfo << "CPU Setup" << EndDebugInfo;
  if (verbose)
    logger << DebugInfo
           << "Verbose activated."
           << EndDebugInfo;

  /* Looking at endianness in SCTLR.B, Note: Configuring endianness
   * through SCTLR.B is obsolete in ARMv7 */
  if (SCTLR::B.Get( sctlr )) {
    if (verbose)
      logger << DebugInfo << "Setting endianness to little endian" << EndDebugInfo;
    cpsr.Set( E, 0 );
  }
  else {
    if (verbose)
      logger << DebugInfo << "Setting endianness to big endian" << EndDebugInfo;
    cpsr.Set( E, 1 );
  }

  /* setting initial pc by the cp15 vinithi parameter */
  uint32_t init_pc = SCTLR::V.Get( sctlr ) ? 0xFFFF0000UL : 0;
  if (verbose)
    logger << DebugInfo << "Setting initial pc to 0x" << std::hex << init_pc << std::dec << EndDebugInfo;
  BranchExchange( init_pc );

  if (cpu_cycle_time_ps == 0)
    {
      // We can't provide a valid cpu cycle time configuration
      //   automatically
      logger << DebugError << "cpu-cycle-time should be strictly positive" << EndDebugError;
      return false;
    }

  return true;
}

/** Object end setup method.
 * This method is required for all UNISIM objects and will be called during
 *   the end setup phase.
 * 
 * @return true on success, false otherwise
 */
bool
CPU::EndSetup()
{
  /* Initialize the caches and power support as required. */
  unsigned int min_cycle_time = 0;
  uint64_t il1_def_voltage = 0;
  uint64_t dl1_def_voltage = 0;

  if ( icache.power_mode_import )
    {
      min_cycle_time =
        icache.power_mode_import->GetMinCycleTime();
      il1_def_voltage =
        icache.power_mode_import->GetDefaultVoltage();
    }
  if ( dcache.power_mode_import )
    {
      if ( dcache.power_mode_import->GetMinCycleTime() >
           min_cycle_time )
        min_cycle_time =
          dcache.power_mode_import->GetMinCycleTime();
      dl1_def_voltage = 
        dcache.power_mode_import->GetDefaultVoltage();
    }

  if ( min_cycle_time > 0 )
    {
      if ( cpu_cycle_time_ps < min_cycle_time )
        {
          logger << DebugWarning;
          logger << "A cycle time of " << cpu_cycle_time_ps
                 << " ps is too low for the simulated"
                 << " hardware !" << std::endl;
          logger << "cpu cycle time should be >= "
                 << min_cycle_time << " ps." << std::endl;
          logger << EndDebugWarning;
        }
    }

  if ( voltage == 0 )
    {
      voltage = (il1_def_voltage > dl1_def_voltage) ? 
        il1_def_voltage :
        dl1_def_voltage;
      logger << DebugWarning
             << "A cpu voltage was not defined (set to 0)."
             << " Using the maximum voltage found from the caches as "
             << " current voltage. Voltage used is "
             << voltage
             << " mV." << std::endl;
      if ( icache.power_mode_import )
        logger << "  - instruction cache voltage = "
               << il1_def_voltage
               << " mV";
      if ( dcache.power_mode_import )
        {
          if ( icache.power_mode_import )
            logger << std::endl;
          logger << "  - data cache voltage = "
                 << dl1_def_voltage
                 << " mV";
        }
      logger << EndDebugWarning;
    }
  
  if ( icache.power_mode_import )
    icache.power_mode_import->SetPowerMode(cpu_cycle_time_ps, voltage);
  if ( dcache.power_mode_import )
    dcache.power_mode_import->SetPowerMode(cpu_cycle_time_ps, voltage);

  if ( verbose )
    {
      logger << DebugInfo
             << "Setting cpu cycle time to "
             << cpu_cycle_time_ps
             << " ps."
             << EndDebugInfo;
      logger << DebugInfo
             << "Setting cpu voltage to "
             << voltage
             << " mV."
             << EndDebugInfo;
    }
      
  /* If the memory access reporting import is not connected remove the need of
   *   reporting memory accesses and finished instruction.
   */
  if(!memory_access_reporting_import) {
    requires_memory_access_reporting = false;
    requires_finished_instruction_reporting = false;
  }
  
  return true;
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
  uint32_t current_pc = this->current_pc = GetNPC();

  cur_instruction_address = current_pc;

  if (debug_control_import) 
    {
      DebugControl<uint64_t>::DebugCommand dbg_cmd;

      do 
        {
          dbg_cmd = debug_control_import->FetchDebugCommand(current_pc);
  
          if (dbg_cmd == DebugControl<uint64_t>::DBG_STEP) 
            {
              /* Nothing to do */
              break;
            }
          if (dbg_cmd == DebugControl<uint64_t>::DBG_SYNC) 
            {
              // Sync();
              continue;
            }

          if (dbg_cmd == DebugControl<uint64_t>::DBG_KILL) 
            {
              Stop(0);
            }
          if(dbg_cmd == DebugControl<uint64_t>::DBG_RESET) 
            {
              // TODO : memory_interface->Reset(); 
            }
        }
      while(1);
    }

  if (cpsr.Get(T))
    {
      /* Thumb state */
  
      /* fetch instruction word from memory */
      isa::thumb::CodeType insn;
      ReadInsn(current_pc, insn);
      
      /* Decode current PC */
      isa::thumb::Operation<CPU>* op;
      op = thumb_decoder.Decode(current_pc, insn);
      
      /* update PC register value before execution */
      this->gpr[15] = this->next_pc + 4;
      this->next_pc += 2;
      
      /* Execute instruction */
      op->execute(*this);
      //op->profile(profile);
    }
  
  else {
    /* Arm32 state */
    
    /* fetch instruction word from memory */
    isa::arm32::CodeType insn;
    ReadInsn(current_pc, insn);
      
    /* Decode current PC */
    isa::arm32::Operation<CPU>* op;
    op = arm32_decoder.Decode(current_pc, insn);
    
    /* update PC register value before execution */
    this->gpr[15] = this->next_pc + 8;
    this->next_pc += 4;
    
    /* Execute instruction */
    op->execute(*this);
    //op->profile(profile);
  }
  
  bool exception_occurred = false;
  if ( unlikely(exception) )
    exception_occurred = HandleException();
  
  instruction_counter++;
  stat_instruction_counter.NotifyListeners();
  if ( unlikely((trap_on_instruction_counter == instruction_counter)
                && instruction_counter_trap_reporting_import) )
    instruction_counter_trap_reporting_import->ReportTrap(*this);

  if ( unlikely(trap_on_exception && exception_occurred
                && exception_trap_reporting_import) )
    exception_trap_reporting_import->ReportTrap(*this);
  
  if(requires_finished_instruction_reporting)
    if(memory_access_reporting_import)
      memory_access_reporting_import->ReportFinishedInstruction(this->current_pc, this->next_pc);
}

/** Get caches info
 *
 */
void
CPU::GetCacheInfo(bool &unified, 
                  uint32_t &isize, uint32_t &iassoc, uint32_t &ilen,
                  uint32_t &dsize, uint32_t &dassoc, uint32_t &dlen)
{
  unified = false;
  isize = icache.GetSize();
  iassoc = icache.GetNumWays();
  ilen = icache.LINE_SIZE;
  dsize = icache.GetSize();
  dassoc = dcache.GetNumWays();
  dlen = dcache.LINE_SIZE;
}

/** Drain write buffer.
 * Perform a memory barrier by draining the write buffer.
 */
void
CPU::DrainWriteBuffer()
{
  logger << DebugWarning << "TODO: Drain write buffer once implemented" << EndDebugWarning;
}

/** Invalidate ICache single entry using MVA
 *
 * Perform an invalidation of a single entry in the ICache using the
 *   given address in MVA format.
 *
 * @param mva the address to invalidate
 */
void 
CPU::InvalidateICacheSingleEntryWithMVA(uint32_t init_mva)
{
  uint32_t mva = init_mva & ~(uint32_t)(dcache.LINE_SIZE - 1);

  logger << DebugInfo
         << "Invalidating ICache single entry with MVA:" << std::endl
         << " - mva               = 0x" << std::hex << init_mva << std::endl
         << " - cache aligned mva = 0x" << mva << std::dec
         << EndDebugInfo;

  if ( likely(// SCTLR::C.Get( sctlr ) &&
              icache.GetSize()) )
    {
      uint32_t cache_tag = icache.GetTag(mva);
      uint32_t cache_set = icache.GetSet(mva);
      uint32_t cache_way = 0;
      bool cache_hit = false;
      if ( likely(icache.GetWay(cache_tag, cache_set, &cache_way)) )
        cache_hit = true;

      if ( likely(cache_hit) )
        {
          logger << DebugInfo
                 << "ICache hit (set = "
                 << cache_set << ", way = "
                 << cache_way << "), invalidating it"
                 << EndDebugInfo;
          icache.SetValid(cache_set, cache_way, 0);
        }
      else
        {
          logger << DebugInfo
                 << "ICache miss (set = "
                 << cache_set
                 << "), nothing to do"
                 << EndDebugInfo;
        }
    }
}

/** Clean DCache single entry using MVA
 *
 * Perform a clean of a single entry in the DCache using the given
 *   address in MVA format.
 *
 * @param mva the address to clean
 * @param invalidate true if the line needs to be also invalidated
 */
void 
CPU::CleanDCacheSingleEntryWithMVA(uint32_t init_mva, bool invalidate)
{
  uint32_t mva = init_mva & ~(uint32_t)(dcache.LINE_SIZE - 1);
  uint32_t pa = mva;

  logger << DebugInfo
         << "Cleaning DCache single entry with MVA:" << std::endl
         << " - mva               = 0x" << std::hex << init_mva << std::endl
         << " - cache aligned mva = 0x" << mva << std::dec
         << EndDebugInfo;

  if ( likely(// cp15.IsDCacheEnabled() &&
              dcache.GetSize()) )
    {
      uint32_t cache_tag = dcache.GetTag(mva);
      uint32_t cache_set = dcache.GetSet(mva);
      uint32_t cache_way;
      bool cache_hit = false;
      if ( likely(dcache.GetWay(cache_tag, cache_set, &cache_way)) )
        cache_hit = true;

      if ( likely(cache_hit) )
        {
          logger << DebugInfo
                 << "Cache hit (set = "
                 << cache_set << ", way = "
                 << cache_way << ")"
                 << EndDebugInfo;
          uint8_t cache_dirty = dcache.GetDirty(cache_set, cache_way);

          if ( cache_dirty != 0 )
            {
              logger << DebugInfo
                     << "Line is dirty, performing a cleaning"
                     << EndDebugInfo;
              uint8_t *data = 0;
              // translate the address
              // if ( likely(cp15.IsMMUEnabled()) )
              //   TranslateMVA(mva, pa);
              dcache.GetData(cache_set, cache_way, &data);
              {
                logger << DebugInfo
                       << "Cleaning cache line:" << std::endl
                       << " - mva = 0x" << std::hex << mva << std::endl
                       << " - pa  = 0x" << pa << std::endl
                       << " - tag = 0x" << cache_tag << std::endl
                       << " - set = " << std::dec << cache_set << std::endl
                       << " - way = " << cache_way << std::endl
                       << " - data =" << std::hex;
                for ( unsigned int i = 0; i < dcache.LINE_SIZE; i++ )
                  logger << " " << (unsigned int)data[i];
                logger << std::dec
                       << EndDebugInfo;
              }
              PrWrite(pa, data, dcache.LINE_SIZE);
              dcache.SetDirty(cache_set, cache_way, 0);
            }
          else
            {
              logger << DebugInfo
                     << "Line is already cleaned, doing nothing"
                     << EndDebugInfo;
            }
          if ( invalidate )
            dcache.SetValid(cache_set, cache_way, 0);
        }
      else
        {
          logger << DebugInfo
                 << "Cache miss (set = "
                 << cache_set
                 << ")"
                 << EndDebugInfo;
        }
    }
}

/** Invalidate the caches.
 * Perform a complete invalidation of the instruction cache and/or the 
 *   data cache.
 *
 * @param insn_cache whether or not the instruction cache should be 
 *   invalidated
 * @param data_cache whether or not the data cache should be invalidated
 */
void 
CPU::InvalidateCache(bool insn_cache, bool data_cache)
{
  if ( insn_cache )
    {
      icache.Invalidate();
    }
  if ( data_cache )
    {
      dcache.Invalidate();
    }
}

/** Invalidate the TLBs.
 * Perform a complete invalidation of the instruction TLB and/or the 
 *   data TLB.
 *
 * @param insn_tlb whether or not the instruction tlb should be invalidated
 * @param data_tlb whether or not the data tlb should be invalidated
 */
void 
CPU::InvalidateTLB()
{
  // only the tlb needs to be invalidated, do not touch the lockdown tlb
  // tlb.Invalidate();
}

/** Test and clean DCache.
 * Perform a test and clean operation of the DCache.
 *
 * @return return true if the complete cache is clean, false otherwise
 */
bool 
CPU::TestAndCleanDCache()
{
  bool cleaned = true;
  uint32_t num_sets = dcache.GetNumSets();
  uint32_t num_ways = dcache.GetNumWays();
  uint32_t set_index = 0;
  uint32_t way_index = 0;

  uint32_t dirty_set = 0;
  uint32_t dirty_way = 0;

  for ( set_index = 0; 
        cleaned && (set_index < num_sets);
        set_index++ )
    {
      for ( way_index = 0;
            cleaned && (way_index < num_ways); 
            way_index++ )
        {
          if ( dcache.GetValid(set_index, way_index) != 0 )
			{
              if ( dcache.GetDirty(set_index, way_index) != 0 )
                {
                  cleaned = false;
                  dirty_set = set_index;
                  dirty_way = way_index;
                }
			}
        }
    }

  if ( !cleaned )
    {
      /* Get the address and data of the dirty line */
      uint32_t mva = dcache.GetBaseAddress(dirty_set, dirty_way);
      uint32_t pa = mva;
      // if ( likely(cp15.IsMMUEnabled()) )
      //   TranslateMVA(mva, pa);
      uint8_t *data = 0;
      dcache.GetData(dirty_set, dirty_way, &data);

      /* Write the data into memory */
      PrWrite(pa, data, dcache.LINE_SIZE);
      /* Set the line as clean */
      dcache.SetDirty(dirty_set, dirty_way, 0);

      /* Check if there are still dirty lines */
      cleaned = true;
      for ( set_index = dirty_set;
            cleaned && (set_index < num_sets);
            set_index++ )
        {
          for ( way_index = 0; // we might be doing some useless work
                cleaned && (way_index < num_ways);
                way_index++ )
			{
              if ( (dcache.GetValid(set_index, way_index) != 0)
                   && (dcache.GetDirty(set_index, way_index) != 0) )
                {
                  cleaned = false;
                }
			}
        }
    }

  return cleaned;
}

/** Test, clean and invalidate DCache.
 * Perform a test and clean operation of the DCache, and invalidate the
 *   complete cache if it is clean.
 *
 * @return return true if the complete cache is clean, false otherwise
 */
bool 
CPU::TestCleanAndInvalidateDCache()
{
  bool cleaned = TestAndCleanDCache();

  if ( cleaned ) dcache.Invalidate();

  return cleaned;
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
CPU::InjectReadMemory(uint64_t addr, 
          void *buffer,
          uint32_t size)
{
  uint32_t index = 0;
  uint32_t base_addr = (uint32_t)addr;
  uint32_t ef_addr;

  if ( likely(SCTLR::C.Get( sctlr ) && dcache.GetSize()) )
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
CPU::InjectWriteMemory(uint64_t addr, 
           const void *buffer, 
           uint32_t size)
{
  uint32_t index = 0;
  uint32_t base_addr = (uint32_t)addr;
  uint32_t ef_addr;
  
  if ( likely((SCTLR::C.Get( sctlr )) && dcache.GetSize()) )
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

/** Set/unset the reporting of memory accesses.
 *
 * @param report if true set the reporting of memory acesses, unset 
 *   otherwise
 */
void 
CPU::RequiresMemoryAccessReporting(bool report)
{
  requires_memory_access_reporting = report;
}

/** Set/unset the reporting of finishing instructions.
 * 
 * @param report if true set the reporting of finishing instructions, 
 *   unset otherwise
 */
void 
CPU::RequiresFinishedInstructionReporting(bool report)
{
  requires_finished_instruction_reporting = report;
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
CPU::ReadMemory(uint64_t addr, 
    void *buffer, 
    uint32_t size)
{
  bool status = true;
  uint32_t index = 0;
  uint32_t va = addr;
  uint32_t mva = va;
  uint32_t pa = mva;
  uint32_t cacheable = 1;
  // uint32_t bufferable = 1;

  if ( likely((SCTLR::C.Get( sctlr )) && dcache.GetSize()) )
    {
      // non intrusive access with linux support
      while (size != 0 && status)
        {
          mva = va + index;
          pa = mva;
          // if ( likely(cp15.IsMMUEnabled()) )
          //   status = NonIntrusiveTranslateVA(true, 
          //va + index, mva, pa, cacheable, bufferable);

          bool cache_hit = false;
          if ( likely(// status && 
                      cacheable) )
            {
              uint32_t cache_tag = dcache.GetTag(mva);
              uint32_t cache_set = dcache.GetSet(mva);
              uint32_t cache_way;
              if ( dcache.GetWay(cache_tag, cache_set, &cache_way) )
                {
                  if ( dcache.GetValid(cache_set, cache_way) )
                    {
                      // the cache access is a hit, data can be simply read
                      //   from the cache
                      uint32_t cache_index =
                        dcache.GetIndex(mva);
                      uint32_t data_read_size =
                        dcache.GetDataCopy(cache_set,
                                           cache_way, cache_index, size,
                                           &(((uint8_t *)buffer)[index]));
                      index += data_read_size;
                      size -= data_read_size;
                      cache_hit = true;
                    }
                }
            }
          if ( status && !cache_hit )
            {
              status = status &&
                ExternalReadMemory(pa,
                                   &(((uint8_t *)buffer)[index]),
                                   1);
              if ( status )
                {
                  index++;
                  size--;
                }
            }
        }
    }
  else
    {
      // there is no data cache in this system just perform the request
      //   to the memory subsystem
      while ( size != 0 && status )
        {
          // if ( likely(cp15.IsMMUEnabled()) )
          //   status = NonIntrusiveTranslateVA(true, 
          //va + index, mva, pa, cacheable, bufferable);

          status = // status &&
            ExternalReadMemory(pa + index,
                               &(((uint8_t *)buffer)[index]),
                               1);
          if ( status )
            {
              index++;
              size--;
            }
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
CPU::WriteMemory(uint64_t addr, 
     const void *buffer, 
     uint32_t size)
{
  bool status = true;
  uint32_t index = 0;
  uint32_t va = addr;
  uint32_t mva = va;
  uint32_t pa = mva;
  uint32_t cacheable = 1;
  // uint32_t bufferable = 1;

  if ( likely((SCTLR::C.Get( sctlr )) && dcache.GetSize()) )
    {
      // non intrusive access with linux support
      while ( size != 0 && status )
        {
          mva = va + index;
          pa = mva;
          // if ( likely(cp15.IsMMUEnabled()) )
          //   status = NonIntrusiveTranslateVA(false,
          //va + index, mva, pa, cacheable, bufferable);

          bool cache_hit = false;
          if ( likely(// status && 
                      cacheable) )
            {
              uint32_t cache_tag = dcache.GetTag(mva);
              uint32_t cache_set = dcache.GetSet(mva);
              uint32_t cache_way;
              if ( dcache.GetWay(cache_tag, cache_set, &cache_way) )
                {
                  if ( dcache.GetValid(cache_set, cache_way) )
                    {
                      uint32_t cache_index = dcache.GetIndex(mva);
                      uint32_t write_size =
                        dcache.SetData(cache_set, cache_way, cache_index, 
                                       size, &(((uint8_t *)buffer)[index]));
                      dcache.SetDirty(cache_set, cache_way, 1);
                      index += write_size;
                      size -= write_size;
                      cache_hit = true;
                    }
                }
            }
          if ( status && !cache_hit )
            {
              status = status &&
                ExternalWriteMemory(pa,
                                    &(((uint8_t *)buffer)[index]),
                                    1);
              if ( status )
                {
                  index++;
                  size--;
                }
            }
        }
    }
  else
    {
      // there is no data cache in this system, just write the data to
      //   the memory subsystem
      while ( size != 0 && status )
        {
          // if ( likely(cp15.IsMMUEnabled()) )
          //   status = NonIntrusiveTranslateVA(false,
          //va + index, mva, pa, cacheable, bufferable);
          status = // status &&
            ExternalWriteMemory(pa,
                                &(((uint8_t *)buffer)[index]),
                                1);
          if ( status )
            {
              index++;
              size--;
            }
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
CPU::Disasm(uint64_t addr, uint64_t &next_addr)
{
  isa::arm32::Operation<CPU>* op= NULL;
  uint32_t insn;
  
  std::stringstream buffer;
  if (cpsr.Get(T)) 
    {
      assert("Thumb instructions not supported" != 0);
    } 
  else 
    {
      if (!ReadMemory(addr, &insn, 4)) 
        {
          buffer << "Could not read from memory";
          return buffer.str();
        }
      if (GetEndianness() == E_BIG_ENDIAN)
        insn = BigEndian2Host(insn);
      else
        insn = LittleEndian2Host(insn);
      op = arm32_decoder.Decode(addr, insn);
      op->disasm(*this, buffer);
    }

  return buffer.str();
}
    
/** Reads 32bits instructions from the memory system
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
  uint32_t size = 4;
  uint32_t word;
  uint8_t *data;
  uint32_t va = address;
  uint32_t mva = va;
  uint32_t pa = mva;
  uint32_t cacheable = 1;
  // uint32_t bufferable = 1; // instruction cache ignores the bufferable status

  num_insn_reads++;

  if ( verbose & 0x04 )
    logger << DebugInfo
           << "Fetching instruction at address 0x" << std::hex << va
           << std::dec
           << EndDebugInfo;

  // if ( likely(cp15.IsMMUEnabled()) )
  //   if ( !TranslateVA(true, va, mva, pa, cacheable, bufferable) )
  //     {
  //       logger << DebugError
  //  << "Could not translate address when performing instruction read"
  //  << std::endl
  //  << " - va = 0x" << std::hex << va << std::dec << std::endl
  //  << " - instruction counter = " << instruction_counter
  //  << EndDebugError;
  //       unisim::kernel::service::Simulator::simulator->Stop(this, __LINE__);
  //     }


  if ( likely(SCTLR::I.Get( sctlr ) && icache.GetSize() && cacheable) )
    {
      icache.read_accesses++;
      // check the instruction cache
      uint32_t cache_tag = icache.GetTag(mva);
      uint32_t cache_set = icache.GetSet(mva);
      uint32_t cache_way;
      bool cache_hit = false;
      if ( icache.GetWay(cache_tag, cache_set, &cache_way) )
        {
          if ( icache.GetValid(cache_set, cache_way) )
            {
              // the access is a hit, nothing needs to be done
              cache_hit = true;
              if ( verbose & 0x04 )
                logger << DebugInfo
                       << "Cache hit when fetching instruction with:" << std::endl
                       << " - va = 0x" << std::hex << va << std::endl
                       << " - mva = 0x" << mva << std::endl
                       << " - pa = 0x" << pa << std::dec
                       << EndDebugInfo;
            }
        }
      if ( unlikely(!cache_hit) )
        {
          // get a way to replace
          cache_way = icache.GetNewWay(cache_set);
          // no need to check valiad and dirty bits
          // the new data can be requested
          uint8_t *cache_data = 0;
          uint32_t cache_address =
            icache.GetBaseAddressFromAddress(pa);
          // when getting the data we get the pointer to the cache line
          //   containing the data, so no need to write the cache
          //   afterwards
          if ( verbose & 0x04 )
            logger << DebugInfo
                   << "Cache miss when fetching instruction at address with:" << std::endl
                   << " - va = 0x" << std::hex << va << std::endl
                   << " - mva = 0x" << mva << std::endl
                   << " - pa = 0x" << pa << std::dec << std::endl
                   << "Accessing the memory system."
                   << EndDebugInfo;
          uint32_t cache_line_size = icache.GetData(cache_set,
                                                    cache_way, &cache_data);
          PrRead(cache_address, cache_data,
                 cache_line_size);
          icache.SetTag(cache_set, cache_way, cache_tag);
          icache.SetValid(cache_set, cache_way, 1);
        }
      else
        {
          // cache hit
          icache.read_hits++;
        }

      // at this point the data is in the cache, we can read it from the
      //   cache
      uint32_t cache_index = icache.GetIndex(mva);
      (void)icache.GetData(cache_set, cache_way, cache_index,
                           size, &data);
      icache.GetDataCopy(cache_set, cache_way, cache_index, size,
                         (uint8_t *)&word);

      if ( unlikely(icache.power_estimator_import != 0) )
        icache.power_estimator_import->ReportReadAccess();
    }
  else
    {
      if ( verbose & 0x04 )
        logger << DebugInfo
               << "Fetching instruction from memory (no cache or entry not cacheable):" << std::endl
               << " - va = 0x" << std::hex << va << std::endl
               << " - mva = 0x" << mva << std::endl
               << " - pa = 0x" << pa << std::dec
               << EndDebugInfo;

      // no instruction cache present, just request the insn to the
      //   memory system
      // another possibility is that the access is not cacheable
      PrRead(pa, (uint8_t *)&word, size);
    }
  
  insn = Target2Host(GetEndianness(), word);

  if (unlikely(requires_memory_access_reporting and memory_access_reporting_import))
    memory_access_reporting_import->
      ReportMemoryAccess(unisim::util::debug::MAT_READ, unisim::util::debug::MT_INSN, address, size);
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
CPU::ReadInsn(uint32_t address, unisim::component::cxx::processor::arm::isa::thumb::CodeType& insn)
{
  uint32_t size = 2;
  uint8_t buffer[2];
  uint8_t *data;
  uint32_t va = address;
  uint32_t mva = va;
  uint32_t pa = mva;
  uint32_t cacheable = 1;
  // uint32_t bufferable = 1; // instruction cache ignores the bufferable status

  num_insn_reads++;

  if ( verbose & 0x04 )
    logger << DebugInfo
           << "Fetching instruction at address 0x" << std::hex << va
           << std::dec
           << EndDebugInfo;

  // if ( likely(cp15.IsMMUEnabled()) )
  //         if ( !TranslateVA(true, va, mva, pa, cacheable, bufferable) )
  //     {
  //       logger << DebugError
  //  << "Could not translate address when performing instruction read"
  //  << std::endl
  //  << " - va = 0x" << std::hex << va << std::dec << std::endl
  //  << " - instruction counter = " << instruction_counter
  //  << EndDebugError;
  //       unisim::kernel::service::Simulator::simulator->Stop(this, __LINE__);
  //     }


  if ( likely(SCTLR::I.Get( sctlr ) && icache.GetSize() && cacheable) )
    {
      icache.read_accesses++;
      // check the instruction cache
      uint32_t cache_tag = icache.GetTag(mva);
      uint32_t cache_set = icache.GetSet(mva);
      uint32_t cache_way;
      bool cache_hit = false;
      if ( icache.GetWay(cache_tag, cache_set, &cache_way) )
        {
          if ( icache.GetValid(cache_set, cache_way) )
            {
              // the access is a hit, nothing needs to be done
              cache_hit = true;
              if ( verbose & 0x04 )
                logger << DebugInfo
                       << "Cache hit when fetching instruction with:" << std::endl
                       << " - va = 0x" << std::hex << va << std::endl
                       << " - mva = 0x" << mva << std::endl
                       << " - pa = 0x" << pa << std::dec
                       << EndDebugInfo;
            }
        }
      if ( unlikely(!cache_hit) )
        {
          // get a way to replace
          cache_way = icache.GetNewWay(cache_set);
          // no need to check valiad and dirty bits
          // the new data can be requested
          uint8_t *cache_data = 0;
          uint32_t cache_address =
            icache.GetBaseAddressFromAddress(pa);
          // when getting the data we get the pointer to the cache line
          //   containing the data, so no need to write the cache
          //   afterwards
          if ( verbose & 0x04 )
            logger << DebugInfo
                   << "Cache miss when fetching instruction at address with:" << std::endl
                   << " - va = 0x" << std::hex << va << std::endl
                   << " - mva = 0x" << mva << std::endl
                   << " - pa = 0x" << pa << std::dec << std::endl
                   << "Accessing the memory system."
                   << EndDebugInfo;
          uint32_t cache_line_size = icache.GetData(cache_set,
                                                    cache_way, &cache_data);
          PrRead(cache_address, cache_data,
                 cache_line_size);
          icache.SetTag(cache_set, cache_way, cache_tag);
          icache.SetValid(cache_set, cache_way, 1);
        }
      else
        {
          // cache hit
          icache.read_hits++;
        }

      // at this point the data is in the cache, we can read it from the
      //   cache
      uint32_t cache_index = icache.GetIndex(mva);
      (void)icache.GetData(cache_set, cache_way, cache_index,
                           size, &data);
      icache.GetDataCopy(cache_set, cache_way, cache_index, size,
                         buffer);

      if ( unlikely(icache.power_estimator_import != 0) )
        icache.power_estimator_import->ReportReadAccess();
    }
  else
    {
      if ( verbose & 0x04 )
        logger << DebugInfo
               << "Fetching instruction from memory (no cache or entry not cacheable):" << std::endl
               << " - va = 0x" << std::hex << va << std::endl
               << " - mva = 0x" << mva << std::endl
               << " - pa = 0x" << pa << std::dec
               << EndDebugInfo;

      // no instruction cache present, just request the insn to the
      //   memory system
      // another possibility is that the access is not cacheable
      PrRead(pa, buffer, size);
    }
  
  if (GetEndianness() == E_LITTLE_ENDIAN) {
    insn.str[0] = buffer[0];
    insn.str[1] = buffer[1];
  } else {
    /* Endianness is not the standard ARM instruction endianness */ 
    insn.str[0] = buffer[1];
    insn.str[1] = buffer[0];
  }
  insn.size = 16;
  if (unlikely(requires_memory_access_reporting and memory_access_reporting_import))
    memory_access_reporting_import->
      ReportMemoryAccess(unisim::util::debug::MAT_READ, unisim::util::debug::MT_INSN, address, size);
}


/** Software Interrupt
 *  This method is called by SWI instructions to handle software interrupts.
 */
void
CPU::SWI( uint32_t imm )
{
  // we are executing on full system mode
  this->MarkVirtualExceptionVector(unisim::component::cxx::processor::arm::exception::SWI);
}

/** Breakpoint
 *  This method is called by BKPT instructions to handle breakpoints.
 */
void
CPU::BKPT( uint32_t imm )
{
  // we are executing on full system mode
  this->MarkVirtualExceptionVector(unisim::component::cxx::processor::arm::exception::PABRT);
}
  
/** Check access permission and domain bits.
 *
 * @param is_read true if the access is a read
 * @param ap the access permission bits
 * @param domain the domain being used
 * 
 * @return true if correct, false otherwise
 */
bool
CPU::CheckAccessPermission( bool is_read, uint32_t ap, uint32_t domain )
{
	if ( ap == 0x00UL )
	{
		return false;
	}

	else if ( ap == 0x01UL )
	{
		uint32_t mode = cpsr.Get( M );
		if ( mode == USER_MODE )
		{
			logger << DebugError
				<< "Accessing privileged data under USER mode: "
				<< std::endl
				<< "- is_read = " << is_read << std::endl
				<< "- ap  = " << ap << std::endl
				<< "- domain = " << domain << std::endl
				<< EndDebugError;
			return false;
		}
		else
		{
			if ( verbose & 0x010 )
				logger << DebugInfo
					<< "Accessing privileged data under privileged mode."
					<< EndDebugInfo;
		}
	}

	else if ( ap == 0x02UL )
	{
		uint32_t mode = cpsr.Get(M);
		if ( mode == USER_MODE )
		{
			if ( !is_read )
			{
				logger << DebugError
					<< "Accessing privileged data under USER mode: "
					<< std::endl
					<< "- is_read = " << is_read << std::endl
					<< "- ap  = " << ap << std::endl
					<< "- domain = " << domain << std::endl
					<< EndDebugError;
				return false;
			}
			else
			{
				if ( verbose & 0x010 )
					logger << DebugInfo
						<< "Accessing privileged data under user mode"
						<< " (read-only)"
						<< EndDebugInfo;
			}
		}
		else
		{
			if ( verbose & 0x010 )
			{
				logger << DebugInfo
					<< "Accessing privileged data under privileged mode."
					<< EndDebugInfo;
			}
		}
	}	

	else if ( ap == 0x03UL )
	{
		if ( verbose & 0x010 )
			logger << DebugInfo
				<< "Accessing non protected data."
				<< EndDebugInfo;
	}

	// all the checks succeded
	return true;
}

/** Translate address from VA to MVA and physical address.
 *
 * @param va the virtual address to handle
 * @param mva the generated modified virtual address
 * @param pa the generated physicial address
 * @return true on success, false on error
 */
bool
CPU::TranslateVA( bool is_read, uint32_t va, uint32_t &mva, uint32_t &pa, uint32_t &cacheable, uint32_t &bufferable )
{
	// transform the VA into MVA
	if ( (va & FCSEIDR_MASK) == 0 )
		mva = this->fcseidr | va;
	else
		mva = va;
	// Build first the address of the Translation Table Base
	uint32_t ttb_addr = 
		this->ttbr0 | ((mva & 0xfff00000UL) >> 18);
	if ( verbose & 0x010 )
	{
		logger << DebugInfo
			<< "Translating virtual address:" << std::endl
			<< " - va = 0x" << std::hex << va << std::endl
			<< " - mva = 0x" << mva << std::endl
			<< " - ttb = 0x" << this->ttbr0 << std::dec
			<< EndDebugInfo;
	}

	uint32_t first_level = 0;
	GetTLBEntry(ttb_addr, first_level);

	// check wether is a section page or a second access is required
	if ( (first_level & 0x03UL) == 0 )
	{
		// fault page
		// this is an invalid entry, provoke a data abort
		logger << DebugError
			<< "Address translation for 0x"
			<< std::hex << va << std::dec
			<< " was not found (or returned a fault page)" << std::endl
			<< " - ttb address = 0x" << std::hex << ttb_addr << std::dec
			<< std::endl
			<< " - first level fetched = 0x" << std::hex
			<< first_level << std::endl
			<< " - pc = 0x" << cur_instruction_address << std::dec
			<< EndDebugError;
		return false;
	}

	else if ( (first_level & 0x03UL) == 0x01UL ) // Coarse page
	{
		// a second level descriptor fetch is initiated
		// build the address of the 2nd level descriptor
		uint32_t coarse_addr =
			(first_level & 0xfffffc00UL) |
			((mva & 0x000ff000UL) >> 10);
		if ( verbose & 0x010)
		{
			logger << DebugInfo
				<< "First level TLB entry was a coarse page descriptor,"
				<< " fetching second level entry:" << std::endl
				<< " - first level entry   = 0x" << std::hex
				<< first_level << std::endl
				<< " - mva                 = 0x" << mva << std::endl
				<< " - coarse page address = 0x"
				<< coarse_addr << std::dec
				<< EndDebugInfo;
		}

		uint32_t second_level = 0;
		GetTLBEntry(coarse_addr, second_level);

		// check whether is a large, small or tiny page
		if ( (second_level & 0x03UL) == 0 )
		{
			// fault page
			assert("Translation fault (TODO: handle second level descriptor"
					" fault)" == 0);
		}

		else if ( (second_level & 0x03UL) == 1 )
		{
			// large page
			pa = (second_level & 0xffff0000UL) |
				(mva & 0x0000ffffUL);
			assert("TODO: Large page descriptor" == 0);
		}

		else if ( (second_level & 0x03UL) == 2 )
		{
			// small page
			pa = (second_level & 0xfffff000UL ) |
				(mva & 0x00000fffUL);
			uint32_t ap[4];
			ap[0] = (second_level >> 4) & 0x03UL;
			ap[1] = (second_level >> 6) & 0x03UL;
			ap[2] = (second_level >> 8) & 0x03UL;
			ap[3] = (second_level >> 10) & 0x03UL;
			uint32_t ap_used = 0;
			switch ( ((mva & 0x0c00UL) >> 10) & 0x03UL )
			{
				case 0x00UL: ap_used = ap[0]; break;
				case 0x01UL: ap_used = ap[1]; break;
				case 0x02UL: ap_used = ap[2]; break;
				case 0x03UL: ap_used = ap[3]; break;
			}
			uint32_t domain = (first_level >> 5) & 0x0fUL;
			uint32_t c = (second_level >> 3) & 0x01UL;
			cacheable = c;
			uint32_t b = (second_level >> 2) & 0x01UL;
			bufferable = b;
			if ( verbose & 0x010 )
				logger << DebugInfo
					<< "Address translated using a small page descriptor:"
					<< std::endl
					<< " - current pc = 0x" << std::hex << cur_instruction_address
					<< std::endl
					<< " - va  = 0x" << va << std::endl
					<< " - mva = 0x" << mva << std::endl
					<< " - pa  = 0x" << pa << std::dec << std::endl
					<< " - ap[0-3] = " << ap[0]
					<< " " << ap[1] << " " << ap[2] << " " << ap[3] << std::endl
					<< " - ap  = " << ap_used << std::endl
					<< " - domain = " << domain << std::endl
					<< " - c   = " << c << std::endl
					<< " - b   = " << b
					<< EndDebugInfo;

			// check access permissions
			if ( unlikely(!CheckAccessPermission(is_read,
							ap_used, domain)) )
			{
				logger << DebugError
					<< "Check access permission and domain"
					<< std::endl
					<< " - current pc = 0x" << std::hex << cur_instruction_address
					<< " - va  = 0x" << va << std::endl
					<< " - mva = 0x" << mva << std::endl
					<< " - pa  = 0x" << pa << std::dec << std::endl
					<< " - ap[0-3] = " << ap[0]
					<< " " << ap[1] << " " << ap[2] << " " << ap[3] << std::endl
					<< " - ap  = " << ap_used << std::endl
					<< " - domain = " << domain << std::endl
					<< " - c   = " << c << std::endl
					<< " - b   = " << b
					<< EndDebugError;
				assert("TODO: check access permission and domain" == 0);
			}

		}

		else // 3 tiny page
		{
			// tiny page
			assert("TODO: Tiny page descriptor" == 0);
		}
	}
	
	else if ( (first_level & 0x03UL) == 0x02UL ) // Section page
	{
		// the descriptor is a section descriptor
		pa = (first_level & 0xfff00000UL) |
			(mva & 0x000fffffUL);
		uint32_t ap = (first_level >> 10) & 0x03UL;
		uint32_t domain = (first_level >> 5) & 0x0fUL;
		uint32_t c = (first_level >> 3) & 0x01UL;
		cacheable = c;
		uint32_t b = (first_level >> 2) & 0x01UL;
		bufferable = b;
		if ( verbose & 0x010 )
			logger << DebugInfo
				<< "Address translated using a section descriptor:" << std::endl
				<< "- current pc = 0x" << std::hex << cur_instruction_address
				<< std::dec << std::endl
				<< "- va  = 0x" << std::hex << va << std::endl
				<< "- mva = 0x" << mva << std::endl
				<< "- pa  = 0x" << pa << std::dec << std::endl
				<< "- ap  = " << ap << std::endl
				<< "- domain = " << domain << std::endl
				<< "- c   = " << c << std::endl
				<< "- b   = " << b
				<< EndDebugInfo;

		// check the access permissions - domain couple
		//  if ap == 3 then no need to check anything else,
		//    read/write access are allowed
		if ( unlikely(!CheckAccessPermission(is_read,
						ap, domain)) )
		{
			logger << DebugError
				<< "Check access permission and domain"
				<< std::endl
				<< " - current pc = 0x" << std::hex << cur_instruction_address
				<< " - va  = 0x" << va << std::endl
				<< " - mva = 0x" << mva << std::endl
				<< " - pa  = 0x" << pa << std::dec << std::endl
				<< " - ap  = " << ap << std::endl
				<< " - domain = " << domain << std::endl
				<< " - c   = " << c << std::endl
				<< " - b   = " << b
				<< EndDebugError;
			assert("TODO: check access permission and domain" == 0);
		}
	}

	else // 0x03UL fine page table
	{
		assert("Translation fault (TODO: fine page table)" == 0);
	}

	return true;
}

/** Translate address from MVA to physical address.
 *
 * @param mva the generated modified virtual address
 * @param pa the generated physicial address
 * @return true on success, false on error
 */
bool
CPU::TranslateMVA( uint32_t mva, uint32_t &pa )
{
	// Build first the address of the Translation Table Base
	uint32_t ttb_addr = this->ttbr0 | ((mva & 0xfff00000UL) >> 18);
	if ( verbose & 0x010 )
	{
		logger << DebugInfo
			<< "Translating modified virtual address:" << std::endl
			<< " - mva = 0x" << mva << std::endl
			<< " - ttb = 0x" << this->ttbr0 << std::dec
			<< EndDebugInfo;
	}

	uint32_t first_level = 0;
	GetTLBEntry(ttb_addr, first_level);

	// check wether is a section page or a second access is required
	if ( first_level & 0x02UL )
	{
		// the descriptor is a section descriptor
		pa = (first_level & 0xfff00000UL) |
			(mva & 0x000fffffUL);
	}
	else
	{
		// the page is not a section a second access to the tlb is required
		assert("Translation fault (TODO: second level access)" == 0);
	}

	return true;
}

/** Get the TLB descriptor pointed by the given Translation Table Base address.
 *
 * This method check the logical TLB for the given Translation Table address, by
 *   accessing first the lockdown TLB, then the main TLB or otherwise the memory
 *   TLB.
 *
 * @param ttb_addr the address on the Translation Table Base to look for
 * @param descriptor the found descriptor
 */
void
CPU::GetTLBEntry( uint32_t ttb_addr, uint32_t &descriptor )
{
	if ( unlikely(verbose & 0x010) )
	{
		logger << DebugInfo
			<< "Looking for TLB descriptor entry with Translation Table address"
			<< " 0x" << std::hex << ttb_addr << std::dec
			<< EndDebugInfo;
	}

	// Get the first level descriptor
	// Two TLB are available, the lockdown TLB and the regular TLB
	// If the address is not found on the TLBs then perform a page walk
	// Check first the lockdown TLB and afterwards the regular TLB
	bool found = false;
	uint32_t way = 0;
	ltlb.read_accesses++;
	if ( ltlb.GetWay(ttb_addr, &way) )
	{
		if ( ltlb.GetValid(way) )
		{
			ltlb.read_hits++;
			descriptor = ltlb.GetData(way);
			found = true;
		}
	}
	if ( unlikely(ltlb.power_estimator_import != 0) )
		ltlb.power_estimator_import->ReportReadAccess();

	if ( !found)
	{
		if ( unlikely(verbose & 0x010) )
		{
			logger << DebugInfo
				<< "TLB descriptor not found in the lockdown TLB,"
				<< " checking main TLB."
				<< EndDebugInfo;
		}
		// the entry was not found in the lockdown tlb
		// check the main tlb
		uint32_t tag = tlb.GetTag(ttb_addr);
		uint32_t set = tlb.GetSet(ttb_addr);
		tlb.read_accesses++;
		if ( tlb.GetWay(tag, set, &way) )
		{
			if ( tlb.GetValid(set, way) )
			{
				tlb.read_hits++;
				descriptor = tlb.GetData(set, way);
				found = true;
			}
		}
		if ( unlikely(tlb.power_estimator_import != 0) )
			tlb.power_estimator_import->ReportReadAccess();
	}
	if ( !found )
	{
		if ( verbose & 0x010 )
		{
			logger << DebugInfo
				<< "TLB descriptor not found in the main TLB, "
				<< "reading it from memory."
				<< EndDebugInfo;
		}
		uint8_t descriptor_data[4];
		// oops, we are forced to perform a page walk
		PrRead(ttb_addr, descriptor_data, 4);
		memcpy(&descriptor, descriptor_data, 4);
		descriptor = LittleEndian2Host(descriptor);
	
		// ok, we have the entry add it to the main tlb
		uint32_t tag = tlb.GetTag(ttb_addr);
		uint32_t set = tlb.GetSet(ttb_addr);
		uint32_t way = tlb.GetNewWay(set);
		if ( verbose & 0x010 )
		{
			logger << DebugInfo
				<< "Filling main tlb with fetched descriptor:"
				<< " - descriptor address = 0x" 
				<< std::hex << ttb_addr << std::endl
				<< " - descriptor value   = 0x"
				<< descriptor << std::endl
				<< " - tag = 0x" << tag << std::dec << std::endl
				<< " - set = " << set << std::endl
				<< " - way = " << way
				<< EndDebugInfo;
		}
		tlb.write_accesses++;
		tlb.SetEntry(tag, set, way, descriptor, 1);
	}
}

/************************************************************************/
/* Exception handling         START */
/************************************************************************/
  
/************************************************************************/
/* Exception handling           END */
/************************************************************************/

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
    case CP15ENCODE( 0, 0, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "MIDR, Main ID Register"; }
          void Write( BaseCpu& _cpu, uint32_t value ) { throw 0; }
          uint32_t Read( BaseCpu& _cpu ) {
            return
              ((uint32_t)0x041  << 24) |
              ((uint32_t)0x0    << 20) |
              ((uint32_t)0x06   << 16) |
              ((uint32_t)0x0926 <<  4) |
              ((uint32_t)0x05   <<  0);
          }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 0, 0, 0, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "CTR, Cache Type Register"; }
          void Write( BaseCpu& _cpu, uint32_t value ) { throw 0; }
          uint32_t Read( BaseCpu& _cpu ) {
            bool unified = false;
            uint32_t isize = 0, iassoc = 0, ilen = 0, dsize = 0, dassoc = 0, dlen = 0;
            dynamic_cast<CPU&>( _cpu ).GetCacheInfo( unified, isize, iassoc, ilen, dsize, dassoc, dlen );
            uint32_t result = 0x1c000000UL;
            if (not unified) result |= 0x01000000UL;
            else            { dsize = isize; dassoc = iassoc; dlen = ilen; }
            
            switch ( isize / 1024 )
              {
              case 4:        result |= (0x03UL << 6); break;
              case 8:        result |= (0x04UL << 6); break;
              case 16:       result |= (0x05UL << 6); break;
              case 32:       result |= (0x06UL << 6); break;
              case 64:       result |= (0x07UL << 6); break;
              case 128:      result |= (0x08UL << 6); break;
              default: throw 0;
              }
            switch ( dsize / 1024 )
              {
              case 4:        result |= ((0x03UL << 6) << 12); break;
              case 8:        result |= ((0x04UL << 6) << 12); break;
              case 16:       result |= ((0x05UL << 6) << 12); break;
              case 32:       result |= ((0x06UL << 6) << 12); break;
              case 64:       result |= ((0x07UL << 6) << 12); break;
              case 128:      result |= ((0x08UL << 6) << 12); break;
              default: throw 0;
              }
            switch ( iassoc )
              {
              case 4:        result |= (0x02UL << 3); break;
              default: throw 0;
              }
            switch ( dassoc )
              {
              case 4:        result |= ((0x02UL << 3) << 12); break;
              default: throw 0;
              }
            switch ( ilen )
              {
              case 32:       result |= 0x02UL; break;
              default: throw 0;
              }
            switch ( dlen )
              {
              case 32:       result |= (0x02UL << 12); break;
              default: throw 0;
              }
            // No need to set the M bit which should be always 0
            //   meaning that the caches are present
            return result;
          }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 7, 0, 5, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "ICIALLU, Invalidate all instruction caches to PoU"; }
          uint32_t Read( BaseCpu& _cpu ) { throw 0; return 0; }
          void Write( BaseCpu& _cpu, uint32_t value ) { dynamic_cast<CPU&>( _cpu ).InvalidateCache(true, false); }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 7, 0, 5, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "ICIMVAU, Invalidate instruction caches by MVA to PoU"; }
          uint32_t Read( BaseCpu& _cpu ) { throw 0; return 0; }
          void Write( BaseCpu& _cpu, uint32_t value ) { dynamic_cast<CPU&>( _cpu ).InvalidateICacheSingleEntryWithMVA(value); }
        } x;
        return x;
      } break;

    case CP15ENCODE( 7, 0, 7, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "?, Invalidating instruction and data"; }
          uint32_t Read( BaseCpu& _cpu ) { throw 0; return 0; }
          void Write( BaseCpu& _cpu, uint32_t value ) { dynamic_cast<CPU&>( _cpu ).InvalidateCache(true, true); }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 7, 0, 10, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DCCMVAC, Clean data cache line by MVA to PoC"; }
          uint32_t Read( BaseCpu& _cpu ) { throw 0; return 0; }
          void Write( BaseCpu& _cpu, uint32_t value ) { dynamic_cast<CPU&>( _cpu ).CleanDCacheSingleEntryWithMVA(value, false); }
        } x;
        return x;
      } break;

    case CP15ENCODE( 7, 0, 10, 3 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "Test And Clean DCache"; }
          void Write( BaseCpu& _cpu, uint32_t value ) { throw 0; }
          uint32_t Read( BaseCpu& _cpu ) { return dynamic_cast<CPU&>( _cpu ).TestAndCleanDCache() ? 0x40000000UL : 0; }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 7, 0, 10, 4 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "CP15DSB, Data Synchronization Barrier operation"; }
          uint32_t Read( BaseCpu& _cpu ) { throw 0; return 0; }
          void Write( BaseCpu& _cpu, uint32_t value ) { dynamic_cast<CPU&>( _cpu ).DrainWriteBuffer(); }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 7, 0, 14, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DCCIMVAC, Clean and invalidate data cache line by MVA to PoC"; }
          uint32_t Read( BaseCpu& _cpu ) { throw 0; return 0; }
          void Write( BaseCpu& _cpu, uint32_t value ) { dynamic_cast<CPU&>( _cpu ).CleanDCacheSingleEntryWithMVA(value, true); }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 7, 0, 14, 3 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "?, Test Clean And Invalidate DCache"; }
          uint32_t Read( BaseCpu& _cpu ) { return dynamic_cast<CPU&>( _cpu ).TestCleanAndInvalidateDCache() ? 0x40000000UL : 0; }
          void Write( BaseCpu& _cpu, uint32_t value ) { throw 0; }
        } x;
        return x;
      } break;
       
    case CP15ENCODE( 8, 0, 5, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "ITLBIALL, invalidate instruction TLB"; }
          uint32_t Read( BaseCpu& _cpu ) { throw 0; return 0; }
          void Write( BaseCpu& _cpu, uint32_t value ) { dynamic_cast<CPU&>( _cpu ).InvalidateTLB(); }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 8, 0, 6, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DTLBIALL, invalidate data TLB"; }
          uint32_t Read( BaseCpu& _cpu ) { throw 0; return 0; }
          void Write( BaseCpu& _cpu, uint32_t value ) { dynamic_cast<CPU&>( _cpu ).InvalidateTLB(); }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 8, 0, 7, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "TLBIALL, invalidate unified TLB"; }
          uint32_t Read( BaseCpu& _cpu ) { throw 0; return 0; }
          void Write( BaseCpu& _cpu, uint32_t value ) { dynamic_cast<CPU&>( _cpu ).InvalidateTLB(); }
        } x;
        return x;
      } break;
      
    }
  
  // Fall back to base cpu CP15 registers
  return this->BaseCpu::CP15GetRegister( crn, opcode1, crm, opcode2 );
}

/** Performs a prefetch access.
 *
 * @param addr the memory address of the prefetch access
 */
void 
CPU::PerformPrefetchAccess( uint32_t addr )
{
	uint32_t va = addr;
	uint32_t mva = va;
	uint32_t pa = mva;
	uint32_t cacheable = 1;
	uint32_t bufferable = 1;

	if ( unlikely(verbose & 0x02) )
		logger << DebugInfo
			<< "Performing prefetch with memop values:" << std::endl
			<< " - addr = 0x" << std::hex << va << std::dec 
			<< EndDebugInfo;

	if ( likely(SCTLR::M.Get( this->sctlr )) )
		if ( !TranslateVA(true, va, mva, pa, cacheable, bufferable) )
		{
			if ( unlikely(verbose & 0x02) )
				logger << DebugInfo
					<< "Prefetch not performed because there is no translation"
					<< " for the requested address (0x" << std::hex
					<< va << std::dec << ")"
					<< EndDebugInfo;
			// no translation found for the given address, do nothing
			return;
		}

	if ( likely(SCTLR::C.Get(this->sctlr) && dcache.GetSize() && cacheable) )
	{
		dcache.prefetch_accesses++;
		uint32_t cache_tag = dcache.GetTag(mva);
		uint32_t cache_set = dcache.GetSet(mva);
		uint32_t cache_way;
		bool cache_hit = false;
		if ( dcache.GetWay(cache_tag, cache_set, &cache_way) )
		{
			if ( dcache.GetValid(cache_set, cache_way) )
			{
				// the access is a hit, nothing needs to be done
				cache_hit = true;
				if ( unlikely(verbose & 0x02) )
					logger << DebugInfo
						<< "Cache prefetch hit"
						<< EndDebugInfo;
			}
		}
		// if the access was a miss, data needs to be fetched from main
		//   memory and placed into the cache
		if ( unlikely(!cache_hit) )
		{
			if ( unlikely(verbose & 0x02) )
				logger << DebugInfo
					<< "Cache prefetch miss"
					<< EndDebugInfo;

			// get a way to replace
			cache_way = dcache.GetNewWay(cache_set);
			// get the valid and dirty bits from the way to replace
			uint32_t cache_valid = dcache.GetValid(cache_set,
					cache_way);
			uint32_t cache_dirty = dcache.GetDirty(cache_set,
					cache_way);
			if ( (cache_valid != 0) && (cache_dirty != 0) )
			{
				// the cache line to replace is valid and dirty so it needs
				//   to be sent to the main memory
				uint8_t *rep_cache_data = 0;
				uint32_t mva_cache_address =
						dcache.GetBaseAddress(cache_set,
								cache_way);
				uint32_t pa_cache_address = mva_cache_address;
				// translate the address
				if ( likely(SCTLR::M.Get( this->sctlr )) )
					TranslateMVA(mva_cache_address,
							pa_cache_address);
				if ( unlikely(verbose & 0x02) )
					logger << DebugInfo
						<< "Replace cache line with:" << std::endl
						<< " - mva = 0x" << std::hex << mva_cache_address
						<< std::endl
						<< " - pa = 0x" << pa_cache_address << std::dec
						<< EndDebugInfo;
				dcache.GetData(cache_set, cache_way,
						&rep_cache_data);
				PrWrite(pa_cache_address, rep_cache_data,
						dcache.LINE_SIZE);
			}
			// the new data can be requested
			uint8_t *cache_data = 0;
			uint32_t cache_address =
					dcache.GetBaseAddressFromAddress(pa);
			// when getting the data we get the pointer to the cache line
			//   containing the data, so no need to write the cache
			//   afterwards
			if ( unlikely(verbose & 0x02) )
				logger << DebugInfo
					<< "Fetching cache line with:" << std::endl
					<< " - pa (base_address) = 0x" << std::hex << cache_address
					<< std::endl
					<< " - set = " << std::dec << cache_set << std::endl
					<< " - way = " << cache_way
					<< EndDebugInfo;
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
 *
 * @param addr the memory address of the write access
 * @param size the memory size of the write access
 * @param value the value of the write access
 */
void 
CPU::PerformWriteAccess( uint32_t addr, uint32_t size, uint32_t value )
{
	uint32_t va = addr;
	uint32_t mva = va;
	uint32_t pa = va;
	uint8_t val8 = 0;
	uint16_t val16 = 0;
	uint32_t val32 = 0;
	uint8_t data8, data16[2], data32[4];
	uint8_t *data = 0;
	uint32_t cacheable = 1;
	uint32_t bufferable = 1;

	if ( unlikely(verbose & 0x02) )
		logger << DebugInfo
			<< "Performing write with memop values:" << std::endl
			<< " - addr = 0x" << std::hex << va << std::dec << std::endl
			<< "- size = " << size
			<< EndDebugInfo;

	data8 = 0;
	memset(data16, 0, sizeof(uint8_t) * 2);
	memset(data32, 0, sizeof(uint8_t) * 4);
	// fix the write address depending on the request size and endianess
	//   and prepare the data to write
        if (cpsr.Get(E)) va ^= (-size) & 3;
	switch(size)
	{
	case 1:
		val8 = (uint8_t)value;
		memcpy(&data8, &val8, 1);
		data = &data8;
		break;
	case 2:
		val16 = (uint16_t)value;
		val16 = Host2Target(GetEndianness(), val16);
		memcpy(data16, &val16, 2);
		data = data16;
		break;
	case 4:
		val32 = value;
		val32 = Host2Target(GetEndianness(), val32);
		memcpy(data32, &val32, 4);
		data = data32;
		break;
	default: // should never happen
		break;
	}

	if ( likely(SCTLR::M.Get( this->sctlr )) )
		if ( !TranslateVA(false, va, mva, pa, cacheable, bufferable) )
		{
			logger << DebugError
				<< "Could not translate address when performing write:"
				<< std::endl
				<< " - va = 0x" << std::hex << va << std::dec << std::endl
				<< " - size = " << size << std::endl
				<< " - pc = 0x" << std::hex << cur_instruction_address << std::dec << std::endl
				<< " - instruction counter = " << instruction_counter
				<< EndDebugError;
			unisim::kernel::service::Simulator::simulator->Stop(this, __LINE__);
		}

	if ( unlikely(verbose & 0x02) )
	{
		logger << DebugInfo
			<< "Writing into 0x" << std::hex << pa
			<< " with data = "
			<< (unsigned int)data[0]
			<< " " << (unsigned int)data[1]
			<< " " << (unsigned int)data[2]
			<< " " << (unsigned int)data[3]
			<< std::dec;
		if ( SCTLR::C.Get( this->sctlr ) && dcache.GetSize() && cacheable )
			logger << std::endl
				<< " - line cacheable"
				<< EndDebugInfo;
		else
			logger << EndDebugInfo;
	}

	if ( likely(SCTLR::C.Get( this->sctlr ) && dcache.GetSize() && cacheable) )
	{
		dcache.write_accesses++;
		uint32_t cache_tag = dcache.GetTag(mva);
		uint32_t cache_set = dcache.GetSet(mva);
		uint32_t cache_way;
		bool cache_hit = false;
		if ( dcache.GetWay(cache_tag, cache_set, &cache_way) )
		{
			if ( dcache.GetValid(cache_set, cache_way) )
			{
				// the access is a hit
				cache_hit = true;
			}
			else
				if ( unlikely(verbose & 0x02) )
					logger << DebugInfo
						<< "Way found but not valid (way = "
						<< cache_way << ")"
						<< EndDebugInfo;
		}
		if ( likely(cache_hit) )
		{
			if ( unlikely(verbose & 0x02) )
				logger << DebugInfo
					<< "Cache hit (set = "
					<< cache_set << ", way = "
					<< cache_way << ")"
					<< EndDebugInfo;
			dcache.write_hits++;
			uint32_t cache_index = dcache.GetIndex(mva);
			dcache.SetData(cache_set, cache_way, cache_index,
					size, data);
			dcache.SetDirty(cache_set, cache_way, 1);
			// if the access is not bufferable the data has to be written to the
			//   memory system like a write through cache
			if ( bufferable == 0 )
			{
				if ( unlikely(verbose & 0x02) )
					logger << DebugInfo
						<< "Seding to memory"
						<< EndDebugInfo;
				PrWrite(pa, data, size);
			}
		}
		else
		{
			if ( unlikely(verbose & 0x02) )
				logger << DebugInfo
					<< "Cache miss"
					<< EndDebugInfo;
			if ( bufferable )
			{
				if ( unlikely(verbose & 0x02) )
					logger << DebugInfo
						<< "Bufferable"
						<< EndDebugInfo;
				// the cache act as a write back cache, so the line is fetched
				//   into cache and modified in the cache but not sent into
				//   memory
				// get a way to replace
				cache_way = dcache.GetNewWay(cache_set);
				// get the valid and dirty bits from the way to replace
				uint8_t cache_valid = dcache.GetValid(cache_set,
						cache_way);
				uint8_t cache_dirty = dcache.GetDirty(cache_set,
						cache_way);

				if ( (cache_valid != 0) && (cache_dirty != 0) )
				{
					// the cache line to replace is valid and dirty so it needs
					//   to be sent to the main memory
					uint8_t *rep_cache_data = 0;
					uint32_t mva_cache_address =
						dcache.GetBaseAddress(cache_set, cache_way);
					dcache.GetData(cache_set, cache_way, &rep_cache_data);
					uint32_t pa_cache_address = mva_cache_address;
					// translate the address
					if ( likely(SCTLR::M.Get( this->sctlr )) )
						TranslateMVA(mva_cache_address,
								pa_cache_address);
					if ( unlikely(verbose & 0x02) )
					{
						uint32_t rep_tag = dcache.GetTag(cache_set, cache_way);
						logger << DebugInfo
							<< "Evicting cache line:" << std::endl
							<< " - mva = 0x" << std::hex << mva_cache_address << std::endl
							<< " - pa  = 0x" << pa_cache_address << std::endl
							<< " - tag = 0x" << rep_tag << std::endl
							<< " - set = " << std::dec << cache_set << std::endl
							<< " - way = " << cache_way << std::hex << std::endl
							<< " - data =";
						for ( unsigned int i = 0; i < dcache.LINE_SIZE; i++ )
							logger << " " << (unsigned int)rep_cache_data[i];
						logger << std::dec
							<< EndDebugInfo;
					}
					PrWrite(pa_cache_address, rep_cache_data, 
							dcache.LINE_SIZE);
				}
				// the new data can be requested
				uint8_t *cache_data = 0;
				uint8_t prev_data[64];
				uint32_t cache_address =
						dcache.GetBaseAddressFromAddress(pa);
				// when getting the data we get the pointer to the cache line
				//   containing the data, so no need to write the cache
				//   afterwards
				uint32_t cache_line_size = dcache.GetData(cache_set,
						cache_way, &cache_data);
				PrRead(cache_address, cache_data,
						cache_line_size);
				memcpy(prev_data, cache_data, cache_line_size);
				dcache.SetTag(cache_set, cache_way, cache_tag);
				dcache.SetValid(cache_set, cache_way, 1);
				uint32_t cache_index = dcache.GetIndex(mva);
				dcache.SetData(cache_set, cache_way, cache_index,
						size, data);
				dcache.SetDirty(cache_set, cache_way, 1);
				if ( unlikely(verbose & 0x02) )
				{
					cache_line_size = dcache.GetData(cache_set,
							cache_way, &cache_data);
					logger << DebugInfo
						<< "Written line:" << std::endl
						<< " - mva = 0x" << std::hex << mva << std::dec << std::endl
						<< " - pa  = 0x" << std::hex << pa << std::dec << std::endl
						<< " - tag = 0x" << std::hex << cache_tag << std::dec << std::endl
						<< " - set = " << cache_set << std::endl
						<< " - way = " << cache_way << std::endl
						<< " - index = " << cache_index << std::endl
						<< " - size = " << size << std::endl
						<< " - valid = " << (uint32_t)dcache.GetValid(cache_set, cache_way) << std::endl
						<< " - dirty = " << (uint32_t)dcache.GetDirty(cache_set, cache_way) << std::endl
						<< " - data =" << std::hex;
					for ( unsigned int i = 0; i < size; i++ )
						logger << " " << (unsigned int)data[i];
					logger << std::endl
						<< " - pre line =";
					for ( unsigned int i = 0; i < cache_line_size; i++ )
						logger << " " << (unsigned int)prev_data[i];
					logger << std::endl
						<< " - new line =";
					for ( unsigned int i = 0; i < cache_line_size; i++ )
						logger << " " << (unsigned int)cache_data[i];
					logger << std::dec
						<< EndDebugInfo;
				}
			}
			else
				PrWrite(pa, data, size);
		}

		if ( unlikely(dcache.power_estimator_import != 0) )
			dcache.power_estimator_import->ReportWriteAccess();
	}
	else // there is no data cache
	{
		if ( unlikely(verbose & 0x02) )
			logger << DebugInfo
				<< "No cache, sending to memory"
				<< EndDebugInfo;
		// there is no data cache, so just send the request to the
		//   memory interface
		PrWrite(pa, data, size);
	}

	/* report read memory access if necessary */
	if ( requires_memory_access_reporting )
		if ( memory_access_reporting_import )
			memory_access_reporting_import->ReportMemoryAccess(
					unisim::util::debug::MAT_WRITE,
					unisim::util::debug::MT_DATA,
					addr, size);
}

/** Performs a read access.
 *
 * @param addr the memory address of the read access
 * @param size the memory size of the read access
 * @param _signed wether the value of the read access should be sign extended
 */
uint32_t
CPU::PerformReadAccess( uint32_t addr, uint32_t size, bool _signed )
{
	uint32_t va = addr & ~(uint32_t)(size - 1);
	uint32_t cacheable = 1;
	uint32_t bufferable = 1;
	uint8_t data32[4];
	uint8_t *data;

	if ( unlikely(verbose & 0x02) )
		logger << DebugInfo
			<< "Performing read with memop values:" << std::endl
			<< " - addr = 0x" << std::hex << va << std::dec << std::endl
			<< " - size = " << size
			<< EndDebugInfo;

	// fix the read address depending on the request size and endianess
        if (cpsr.Get(E)) va ^= (-size) & 3;
	uint32_t mva = va;
	uint32_t pa = mva;

	if ( likely(SCTLR::M.Get( this->sctlr )) )
		if ( !TranslateVA(true, va, mva, pa, cacheable, bufferable) )
		{
			logger << DebugError
				<< "Could not translate address when performing read:"
				<< std::endl
				<< " - va = 0x" << std::hex << va << std::dec << std::endl
				<< " - size = " << size << std::endl
				<< " - pc = 0x" << std::hex << cur_instruction_address << std::dec << std::endl
				<< " - instruction counter = " << instruction_counter
				<< EndDebugError;
			unisim::kernel::service::Simulator::simulator->Stop(this, __LINE__);
		}

	if ( unlikely(verbose & 0x02) )
		logger << DebugInfo
			<< "Read address translated:" << std::endl
			<< " - va = 0x" << std::hex << va << std::endl
			<< " - mva = 0x" << mva << std::endl
			<< " - pa = 0x" << pa << std::dec << std::endl
			<< " - cacheable = " << cacheable << std::endl
			<< " - bufferable = " << bufferable
			<< EndDebugInfo;

	if ( likely(SCTLR::C.Get(this->sctlr) && dcache.GetSize() && cacheable) )
	{
		dcache.read_accesses++;
		uint32_t cache_tag = dcache.GetTag(mva);
		uint32_t cache_set = dcache.GetSet(mva);
		uint32_t cache_way;
		bool cache_hit = false;
		if ( dcache.GetWay(cache_tag, cache_set, &cache_way) )
		{
			if ( dcache.GetValid(cache_set, cache_way) )
			{
				// the access is a hit, nothing needs to be done
				cache_hit = true;
				if ( unlikely(verbose & 0x02) )
					logger << DebugInfo
						<< "Cache read hit"
						<< EndDebugInfo;
			}
		}
		// if the access was a miss, data needs to be fetched from main
		//   memory and placed into the cache
		if ( unlikely(!cache_hit) )
		{
			if ( unlikely(verbose & 0x02) )
				logger << DebugInfo
					<< "Cache read miss"
					<< EndDebugInfo;
			// get a way to replace
			cache_way = dcache.GetNewWay(cache_set);
			// get the valid and dirty bits from the way to replace
			uint8_t cache_valid = dcache.GetValid(cache_set,
					cache_way);
			uint8_t cache_dirty = dcache.GetDirty(cache_set,
					cache_way);

			if ( (cache_valid != 0) && (cache_dirty != 0) )
			{
				// the cache line to replace is valid and dirty so it needs
				//   to be sent to the main memory
				uint8_t *rep_cache_data = 0;
				uint32_t mva_cache_address =
					dcache.GetBaseAddress(cache_set, cache_way);
				uint32_t pa_cache_address = mva_cache_address;
				// translate the address
				if ( likely(SCTLR::M.Get( this->sctlr )) )
					TranslateMVA(mva_cache_address,
							pa_cache_address);
				if ( unlikely(verbose & 0x02) )
					logger << DebugInfo
						<< "Replacing cache line with:" << std::endl
						<< " - mva = 0x" << std::hex << mva_cache_address 
						<< std::endl
						<< " - pa = 0x" << pa_cache_address << std::dec
						<< EndDebugInfo;
				dcache.GetData(cache_set, cache_way, &rep_cache_data);
				PrWrite(pa_cache_address, rep_cache_data,
							dcache.LINE_SIZE);
			}
			// the new data can be requested
			uint8_t *cache_data = 0;
			uint32_t cache_address =
					dcache.GetBaseAddressFromAddress(pa);
			// when getting the data we get the pointer to the cache line
			//   containing the data, so no need to write the cache
			//   afterwards
			if ( unlikely(verbose & 0x02) )
				logger << DebugInfo
					<< "Fetching cache line with:" << std::endl
					<< " - pa (base_address) = 0x" << std::hex << cache_address
					<< std::endl
					<< " - set = " << std::dec << cache_set << std::endl
					<< " - way = " << cache_way
					<< EndDebugInfo;
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
		uint32_t cache_index = dcache.GetIndex(mva);
		(void)dcache.GetData(cache_set, cache_way, cache_index,
				size, &data);
	}
	else // there is no data cache
	{
		if ( unlikely(verbose & 0x02) )
			logger << DebugInfo
				<< "Fetching data from memory with:" << std::endl
				<< " - pa = 0x" << std::hex << pa << std::dec << std::endl
				<< " - size = " << size
				<< EndDebugInfo;
		// just read the data from the memory system
		PrRead(pa, data32, size);
		data = data32;
	}

	// fix the data depending on its size
	uint32_t value = 0;
	if (size == 1)
	{
		uint8_t val8 = *data;
		if (_signed)
			value = (int32_t)(int8_t)val8;
		else
			value = val8;
	}
	else if (size == 2)
	{
		uint16_t val16 = 0;
		memcpy(&val16, data, 2);
		val16 = Target2Host(GetEndianness(), val16);
		if (_signed)
			value = (int32_t)(int16_t)val16;
		else
			value = val16;
	}
	else if (size == 4)
	{
		uint32_t val32;
		uint32_t val32_l, val32_r;
		uint32_t align;

		memcpy(&val32, data, 4);
		val32 = Target2Host(GetEndianness(), val32);
		// we need to check alignment
		align = addr & (uint32_t)0x03;
		if (align != 0)
		{
			val32_l = (val32 << (align*8)) &
					((~((uint32_t)0)) << (align*8));
			val32_r = (val32 >> ((4 - align) * 8)) &
					((~((uint32_t)0)) >> ((4 - align) * 8));
			val32 = val32_l + val32_r;
		}
		value = val32;
	}

	if ( unlikely(verbose & 0x02) )
		logger << DebugInfo
			<< "Finished performing read access, setting register with:" 
			<< std::endl
			<< " - value = 0x" << std::hex << value << std::dec
			<< EndDebugInfo;
	
	if ( likely(dcache.GetSize()) )
		if ( unlikely(dcache.power_estimator_import != 0) )
			dcache.power_estimator_import->ReportReadAccess();

	/* report read memory access if necessary */
	if ( requires_memory_access_reporting )
		if ( memory_access_reporting_import )
			memory_access_reporting_import->ReportMemoryAccess(
					unisim::util::debug::MAT_READ,
					unisim::util::debug::MT_DATA,
					addr, size);
	return value;
}

} // end of namespace arm926ejs

template struct unisim::component::cxx::processor::arm::CPU<unisim::component::cxx::processor::arm::arm926ejs::ARM926ejs>;

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
