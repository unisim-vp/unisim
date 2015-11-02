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
#include <unisim/util/debug/simple_register.hh>
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
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::Memory;
using unisim::util::endian::E_BIG_ENDIAN;
using unisim::util::endian::E_LITTLE_ENDIAN;
using unisim::util::endian::BigEndian2Host;
using unisim::util::endian::LittleEndian2Host;
using unisim::util::debug::SimpleRegister;
using unisim::util::debug::Register;
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
  , Service<MemoryInjection<uint64_t> >(name, parent)
  , Client<DebugControl<uint64_t> >(name, parent)
  , Client<MemoryAccessReporting<uint64_t> >(name, parent)
  , Client<TrapReporting>(name, parent)
  , Service<MemoryAccessReportingControl>(name, parent)
  , Service<Disassembly<uint64_t> >(name, parent)
  , Service<Registers>(name, parent)
  , Service<Memory<uint64_t> >(name, parent)
  , disasm_export("disasm-export", this)
  , registers_export("registers-export", this)
  , memory_injection_export("memory-injection-export", this)
  , memory_export("memory-export", this)
  , memory_access_reporting_control_export(
                                           "memory-access-reporting-control-export", this)
  , debug_control_import("debug-control-import", this)
  , memory_access_reporting_import("memory-access-reporting-import", this)
  , symbol_table_lookup_import("symbol-table-lookup-import", this)
  , instruction_counter_trap_reporting_import(
                                              "instruction-counter-trap-reporting-import", this)
  , exception_trap_reporting_import(
                                    "exception-trap-reporting-import", this)
  , ltlb("lockdown-tlb", this)
  , tlb("tlb", this)
  , arm32_decoder()
  , thumb_decoder()
  , exception(0)
  , instruction_counter(0)
  , cur_instruction_address(0)
  , verbose(0)
  , trap_on_instruction_counter(0)
  , trap_on_exception(false)
  , requires_memory_access_reporting(true)
  , requires_finished_instruction_reporting(true)
  , param_cpu_cycle_time_ps("cpu-cycle-time-ps", this,
                            cpu_cycle_time_ps,
                            "The processor cycle time in picoseconds.")
  , param_voltage("voltage", this,
                  voltage,
                  "The processor voltage in mV.")
  , param_verbose("verbose", this,
                  verbose,
                  "Activate the verbose system (0 = inactive, different than 0 = "
                  "active).")
  , param_trap_on_instruction_counter("trap-on-instruction-counter", this,
                                      trap_on_instruction_counter,
                                      "Produce a trap when the given instruction count is reached.")
  , param_trap_on_exception("trap-on-exception", this,
                            trap_on_exception,
                            "Produce a trap when an exception occurs.")
  , stat_instruction_counter("instruction-counter", this,
                             instruction_counter,
                             "Number of instructions executed.")
  , stat_cur_instruction_address("cur-instruction-address", this,
                                 cur_instruction_address,
                                 "Address of the instruction currently being executed.")
  , reg_sp("SP", this, gpr[13],
           "The stack pointer (SP) register (alias of GPR[13]).")
  , reg_lr("LR", this, gpr[14],
           "The link register (LR) (alias of GPR[14]).")
  , reg_pc("PC", this, gpr[15],
           "The program counter (PC) register (alias of debug logical GPR[15]).")
  , reg_cpsr("CPSR", this, cpsr.m_value,
             "The CPSR register.")
  , num_data_prefetches(0)
  , num_data_reads(0)
  , num_data_writes(0)
  , num_insn_reads(0)
  , stat_num_data_prefetches("num-data-prefetches", this,	num_data_prefetches,
                             "Number of data prefetches issued to the memory system.")
  , stat_num_data_reads("num-data-reads", this, num_data_reads,
                        "Number of data reads issued to the memory system.")
  , stat_num_data_writes("num-data-writes", this, num_data_writes,
                         "Number of data writes issued to the memory system.")
  , stat_num_insn_reads("num-insn-reads", this, num_insn_reads,
                        "Number of instruction reads issued to the memory system.")
{
  for (unsigned int i = 0; i < (num_log_gprs-1); i++)
    {
      std::string name, description;
      std::stringstream ss;
      ss << "GPR[" << i << "]";
      name = ss.str();
      ss.clear();
      ss << "Logical register " << i;
      description = ss.str();
      reg_gpr[i] = new unisim::kernel::service::Register<uint32_t>(name.c_str(), this, gpr[i], description.c_str());
    }
  reg_gpr[15] = new unisim::kernel::service::Register<uint32_t>("GPR[15]", this, this->next_pc, "Logical register 15");

  // Init the processor at SUPERVISOR mode
  cpsr.Set(M, SUPERVISOR_MODE);
  // Disable fast and normal interruptions
  cpsr.Set(I, 1);
  cpsr.Set(F, 1);

  // Set the right format for various of the variables
  param_cpu_cycle_time_ps.SetFormat(
                                    unisim::kernel::service::VariableBase::FMT_DEC);
  param_voltage.SetFormat(
                          unisim::kernel::service::VariableBase::FMT_DEC);
  param_trap_on_instruction_counter.SetFormat(
                                              unisim::kernel::service::VariableBase::FMT_DEC);
  stat_instruction_counter.SetFormat(
                                     unisim::kernel::service::VariableBase::FMT_DEC);
}

/** Destructor.
 */
CPU::~CPU()
{
  for (unsigned int i = 0; i < num_log_gprs; i++)
    if (reg_gpr[i]) delete reg_gpr[i];

  for (RegistersRegistry::iterator itr = registers_registry.begin(), end = registers_registry.end(); itr != end; itr++)
    delete itr->second;
  registers_registry.clear();
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

  /* TODO: Remove this section once all the debuggers use the unisim
   *   kernel interface for registers.
   */
  /* setting debugging registers */
  if (verbose)
    logger << DebugInfo << "Initializing debugging registers" << EndDebugError;
	
  for (int i = 0; i < 16; i++) 
    {
      std::stringstream str;
      str << "r" << i;
      registers_registry[str.str().c_str()] = new SimpleRegister<uint32_t>(str.str().c_str(), &gpr[i]);
    }
  
  registers_registry["sp"] = new SimpleRegister<uint32_t>("sp", &gpr[13]);
  registers_registry["lr"] = new SimpleRegister<uint32_t>("lr", &gpr[14]);
  registers_registry["pc"] = new SimpleRegister<uint32_t>("pc", &this->next_pc);
  registers_registry["cpsr"] = new SimpleRegister<uint32_t>("cpsr", &(cpsr.m_value));
  /* End TODO */

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
			
      /* update PC registers value before execution */
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
		
    /* update PC registers value before execution */
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
          //                                    va + index, mva, pa, cacheable, bufferable);

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
          //                                    va + index, mva, pa, cacheable, bufferable);

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
          //                                    va + index, mva, pa, cacheable, bufferable);

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
          //                                    va + index, mva, pa, cacheable, bufferable);
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

/** Get a register by its name.
 * Gets a register interface to the register specified by name.
 *
 * @param name the name of the requested register
 *
 * @return a pointer to the RegisterInterface corresponding to name
 */
Register *
CPU::GetRegister(const char *name)
{
  RegistersRegistry::iterator itr = registers_registry.find( name );
  if (itr != registers_registry.end())
    return itr->second;
  else
    return 0;
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
  //       	if ( !TranslateVA(true, va, mva, pa, cacheable, bufferable) )
  //     {
  //       logger << DebugError
  //              << "Could not translate address when performing instruction read"
  //              << std::endl
  //              << " - va = 0x" << std::hex << va << std::dec << std::endl
  //              << " - instruction counter = " << instruction_counter
  //              << EndDebugError;
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
  //       	if ( !TranslateVA(true, va, mva, pa, cacheable, bufferable) )
  //     {
  //       logger << DebugError
  //              << "Could not translate address when performing instruction read"
  //              << std::endl
  //              << " - va = 0x" << std::hex << va << std::dec << std::endl
  //              << " - instruction counter = " << instruction_counter
  //              << EndDebugError;
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
  this->MarkVirtualExceptionVector(unisim::component::cxx::processor::arm::exception::PREFETCH_ABORT);
}
	
/** Unpredictable Instruction Behaviour.
 * This method is just called when an unpredictable behaviour is detected to
 *   notifiy the processor.
 */
void 
CPU::UnpredictableInsnBehaviour()
{
  logger << DebugWarning
         << "Trying to execute unpredictable behavior instruction"
         << "Location: " 
         << __FUNCTION__ << ":" 
         << __FILE__ << ":" 
         << __LINE__ << ": "
         << EndDebugWarning;
}

/************************************************************************/
/* Exception handling                                             START */
/************************************************************************/
	
/** Process exceptions
 *
 * Returns true if there is an exception to handle.
 * @return true if an exception handling begins
 */
bool
CPU::HandleException()
{
  bool handled = false;
  bool report = false;

  // Exception priorities (from higher to lower)
  // - 1 Reset
  // - 2 Data Abort (including data TLB miss)
  // - 3 FIQ
  // - 4 IRQ
  // - 5 Imprecise Abort (external abort) - ARMv6 (so ignored here)
  // - 6 Prefetch Abort (including prefetch TLB miss)
  // - 7 Undefined instruction / SWI
	
  if ( exception & 
       unisim::component::cxx::processor::arm::exception::RESET )
    {
    }

  else if ( exception & 
			unisim::component::cxx::processor::arm::exception::DATA_ABORT )
    {
    }

  else if ( (exception &
             unisim::component::cxx::processor::arm::exception::FIQ) &&
			!cpsr.Get(F) )
    {
      report = true;
    }

  else if ( (exception & unisim::component::cxx::processor::arm::exception::IRQ) and not cpsr.Get(I) )
    {
      report = true;
      logger << DebugInfo << "Received IRQ interrupt, handling it." << EndDebugInfo;
      handled = true;
      // Determine return information. SPSR is to be the current CPSR, and LR is to be the
      // current PC minus 0 for Thumb or 4 for ARM, to change the PC offsets of 4 or 8
      // respectively from the address of the current instruction into the required address
      // of the instruction boundary at which the interrupt occurred plus 4. For this
      // purpose, the PC and CPSR are considered to have already moved on to their values
      // for the instruction following that boundary. TODO: what does that mean ?!?!?
      uint32_t new_lr_value = GetNPC();
      uint32_t new_spsr_value = cpsr.Get( ALL32 );
      uint32_t vect_offset = 0x18;
      
      // TODO: IRQs may be routed to monitor (if HaveSecurityExt() and
      // SCR.IRQ) or to Hypervisor if (HaveVirtExt() &&
      // HaveSecurityExt() && SCR.IRQ == '0' && HCR.IMO == '1' &&
      // !IsSecure()) || CPSR.M == '11010');
      
      // Handle in IRQ mode. TODO: Ensure Secure state if initially in Monitor mode. This
      // affects the Banked versions of various registers accessed later in the code.
      uint8_t oldmode = cpsr.Get( M ), newmode = 0b10010;  // IRQ mode
      GetMode(oldmode).Swap( *this ); // OUT
      cpsr.Set( M, newmode ); // IRQ mode
      GetMode(newmode).Swap( *this ); // IN
      // Write return information to registers, and make further CPSR changes:
      // IRQs disabled, IT state reset, instruction set and endianness set to
      // SCTLR-configured values.
      Mode& irqmode = GetMode(newmode);
      irqmode.SetSPSR( new_spsr_value );
      SetGPR( 14, new_lr_value );
      cpsr.Set( I, 1 ); // IRQs disabled
      cpsr.Set( A, 1 );
      cpsr.ITSetState( 0b0000, 0b0000 ); // IT state reset
      cpsr.Set( J, 0 );
      cpsr.Set( T, 0 ); // TODO: controlled by SCTLR.TE
      cpsr.Set( E, 0 ); // TODO: controlled by SCTLR.EE
      // Branch to correct IRQ vector ("implementation defined" if SCTLR.VE == '1').
      uint32_t exc_vector_base = SCTLR::V.Get( sctlr ) ? 0xffff0000 : 0x00000000;
      Branch(exc_vector_base + vect_offset);
    }
	
  else if ( exception &
			unisim::component::cxx::processor::arm::exception::PREFETCH_ABORT )
    {
    }
	
  else if ( exception &
			unisim::component::cxx::processor::arm::exception::SWI )
    {
    }

  else if ( exception &
			unisim::component::cxx::processor::arm::exception::UNDEFINED_INSN )
    {
    }

  if ( !handled )
    {
      if ( (exception &
            unisim::component::cxx::processor::arm::exception::FIQ) &&
           cpsr.Get(F) )
        handled = true;

      if ( (exception & 
            unisim::component::cxx::processor::arm::exception::IRQ) &&
           cpsr.Get(I) )
        handled = true;
    }

  if ( !handled )
    {
      logger << DebugError
             << "Exception not handled (" << (unsigned int)exception << ")" 
             << std::endl
             << " - CPSR = 0x" << std::hex << cpsr.bits() << std::dec
             << " - irq? = " 
             << (exception & 
                 unisim::component::cxx::processor::arm::exception::IRQ)
             << std::endl
             << " - CPSR_I = " << cpsr.Get(I)
             << EndDebugError;
      unisim::kernel::service::Simulator::simulator->Stop(this, __LINE__);
    }

  return report;
}

/************************************************************************/
/* Exception handling                                               END */
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
          void Write( base_cpu& cpu, uint32_t value ) { throw 0; }
          uint32_t Read( base_cpu& cpu ) {
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
      
    case CP15ENCODE( 7, 0, 7, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "?, Invalidating instruction and data"; }
          uint32_t Read( base_cpu& cpu ) { throw 0; return 0; }
          void Write( base_cpu& cpu, uint32_t value ) { cpu.InvalidateCache(true, true); }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 7, 0, 10, 3 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "Test And Clean DCache"; }
          void Write( base_cpu& cpu, uint32_t value ) { throw 0; }
          uint32_t Read( base_cpu& cpu ) { return cpu.TestAndCleanDCache() ? 0x40000000UL : 0; }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 7, 0, 14, 3 ):
    {
      static struct : public CP15Reg
      {
        char const* Describe() { return "?, Test Clean And Invalidate DCache"; }
        uint32_t Read( base_cpu& cpu ) { return cpu.TestCleanAndInvalidateDCache() ? 0x40000000UL : 0; }
        void Write( base_cpu& cpu, uint32_t value ) { throw 0; }
      } x;
      return x;
    } break;
       
    }
  
  // Fall back to base cpu CP15 registers
  return this->base_cpu::CP15GetRegister( crn, opcode1, crm, opcode2 );
}

} // end of namespace arm926ejs

template struct unisim::component::cxx::processor::arm::CPU<unisim::component::cxx::processor::arm::arm926ejs::ARM926ejs>;

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
