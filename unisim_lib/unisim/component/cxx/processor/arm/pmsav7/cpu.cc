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
#include <unisim/component/cxx/processor/arm/pmsav7/cpu.hh>
#include <unisim/component/cxx/processor/arm/pmsav7/cp15.hh>
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
namespace pmsav7 {

using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::service::Service;
using unisim::service::interfaces::MemoryInjection;
using unisim::service::interfaces::DebugControl;
using unisim::service::interfaces::MemoryAccessReporting;
using unisim::service::interfaces::TrapReporting;
using unisim::service::interfaces::Disassembly;
using unisim::service::interfaces::Memory;
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
  , memory_access_reporting_control_export("memory-access-reporting-control-export", this)
  , memory_access_reporting_import("memory-access-reporting-import", this)
  , disasm_export("disasm-export", this)
  , memory_injection_export("memory-injection-export", this)
  , memory_export("memory-export", this)
  , memory_import("memory-import", this)
  , debug_control_import("debug-control-import", this)
  , exception_trap_reporting_import("exception-trap-reporting-import", this)
  , instruction_counter_trap_reporting_import("instruction-counter-trap-reporting-import", this)
  , requires_finished_instruction_reporting(true)
  , requires_memory_access_reporting(true)
  // , icache("icache", this)
  // , dcache("dcache", this)
  , arm32_decoder()
  , thumb_decoder()
  , csselr(0)
  , DFSR(0)
  , IFSR(0)
  , DFAR(0)
  , IFAR(0)
  , mpu()
  , instruction_counter(0)
  , trap_on_instruction_counter(0)
  , ipb_base_address( -1 )
  , sctlr_rstval( this->PCPU::SCTLR )
  , param_sctlr_rstval("SCTLR", this, this->sctlr_rstval, "The processor reset value of the SCTLR register.")
  , param_cpu_cycle_time_ps("cpu-cycle-time-ps", this, cpu_cycle_time_ps, "The processor cycle time in picoseconds.")
  , param_verbose("verbose", this, this->PCPU::verbose, "Activate the verbose system (0 = inactive, different than 0 = active).")
  , param_trap_on_instruction_counter("trap-on-instruction-counter", this, trap_on_instruction_counter,
                                      "Produce a trap when the given instruction count is reached.")
  , stat_instruction_counter("instruction-counter", this, instruction_counter, "Number of instructions executed.")
{
  // Set the right format for various of the variables
  param_cpu_cycle_time_ps.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
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
  this->PCPU::CP15ResetRegisters();
  /* sctlr takes its reset value */
  this->SCTLR = sctlr_rstval;
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
  if (verbose)
    logger << DebugInfo << "**bare metal/full system** mode: TakeReset" << EndDebugInfo;
  this->TakeReset();
  
  if (verbose)
    logger << DebugInfo << "Initial pc set to 0x" << std::hex << GetNPC() << std::dec << EndDebugInfo;
  
  if (verbose)
    logger << DebugInfo << "Setting cpu cycle time to " << cpu_cycle_time_ps << " ps." << EndDebugInfo;
  
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
  
  if (unlikely(misalignment and not sctlr::A.Get( this->SCTLR ))) {
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
    // TODO: Full misaligned DataAbort(mva, ipaddress, ...)
    throw unisim::component::cxx::processor::arm::exception::DataAbortException(); 
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
  
  //uint32_t write_addr = TranslateAddress<SoftMemAcc>( addr & ~lo_mask, false, true, size );
  uint32_t write_addr = addr & ~lo_mask;

  // There is no data cache or data should not be cached.
  // Just send the request to the memory interface
  PrWrite( write_addr, data, size );
  
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
  
  if (unlikely(misalignment and not sctlr::A.Get( this->SCTLR ))) {
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
    // TODO: Full misaligned DataAbort(mva, ipaddress, ...)
    throw unisim::component::cxx::processor::arm::exception::DataAbortException();
  }
  
  //uint32_t read_addr = TranslateAddress<SoftMemAcc>( addr & ~lo_mask, false, false, size );
  uint32_t read_addr = addr & ~lo_mask;
  
  uint8_t data[4];

  // just read the data from the memory system
  PrRead(read_addr, &data[0], size);
  
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
    this->Stop(-1);
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

  // No data cache, just send request to the memory subsystem
  while (size != 0)
    {
      ef_addr = base_addr + index;
      PrRead(ef_addr, &rbuffer[index], 1);
      index++;
      size--;
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
  
  // No data cache, just send the request to the memory subsystem
  while (size != 0)
    {
      ef_addr = base_addr + index;
      PrWrite( ef_addr, &wbuffer[index], 1 );
      index++;
      size--;
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
  uint8_t* rbuffer = (uint8_t*)buffer;

  // No data cache, just send request to the memory subsystem
  while ((size != 0) and status)
    {
      try {
        //ef_addr = TranslateAddress<DebugMemAcc>( base_addr + index, true, false, 1 );
        ef_addr = base_addr + index;
      } catch (unisim::component::cxx::processor::arm::exception::DataAbortException const& x) {
        status = false;
        break;
      }
      status &= ExternalReadMemory( ef_addr, &rbuffer[index], 1 );
      index++;
      size--;
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
  
  // No data cache, just send request to the memory subsystem
  while ((size != 0) and status)
    {
      try {
        // ef_addr = TranslateAddress<DebugMemAcc>( base_addr + index, true, true, 1 );
        ef_addr = base_addr + index;
      } catch (unisim::component::cxx::processor::arm::exception::DataAbortException const& x) {
        status = false;
        break;
      }
      status &= ExternalWriteMemory( ef_addr, &wbuffer[index], 1 );
      index++;
      size--;
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
  
  // No instruction cache present, just request the insn to the memory
  // system.
  PrRead(base_address, &this->ipb_bytes[0], Cache::LINE_SIZE);

  if (unlikely(requires_memory_access_reporting and memory_access_reporting_import))
    memory_access_reporting_import->
      ReportMemoryAccess(unisim::util::debug::MAT_READ, unisim::util::debug::MT_INSN,
                         base_address, Cache::LINE_SIZE);
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
  //uint32_t base_address = TranslateAddress<SoftMemAcc>( address & -(Cache::LINE_SIZE), false, false, Cache::LINE_SIZE );
  uint32_t base_address = address & -(Cache::LINE_SIZE);
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
  //uint32_t base_address = TranslateAddress<SoftMemAcc>( address & -(Cache::LINE_SIZE), false, false, Cache::LINE_SIZE );
  uint32_t base_address = address & -(Cache::LINE_SIZE);
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

/** Breakpoint
 *  This method is called by BKPT instructions to handle breakpoints.
 */
void
CPU::BKPT( uint32_t imm )
{
  throw std::logic_error( "TODO: Generate a debug event: virtual BKPTInstrDebugEvent() method ?" );
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

bool
CPU::MPU::GetAccessControl( uint32_t va, uint32_t& access_control )
{
  bool region_found = false;
  for (unsigned idx = 0; idx < DRegion; ++idx) {
    Region& region = DR[idx];
    uint32_t size_enable = region.size_enable;
    if (not (size_enable & 1))
      continue; // Region disabled
    unsigned lsbit = ((size_enable >> 1) & 0x1f) + 1;
    uint32_t base_address = region.base_address;
    if (uint64_t(va ^ base_address) >> lsbit)
      continue; // Region doesn't match
    if (lsbit >= 8) {
      // can have subregions
      unsigned subregion = (va >> (lsbit-3)) & 7;
      if ((size_enable >> (8+subregion)) & 1)
        continue; // Subregion doesn't match
    }
    access_control = region.access_control;
    region_found = true;
  }
  return region_found;
}

void
CPU::CheckPermissions( uint32_t va, bool ispriv, bool iswrite, unsigned size )
{
  if (not sctlr::M.Get( this->SCTLR ))
    return; // MPU disabled
  
  // MPU enabled
  uint32_t access_control;
  if (not mpu.GetAccessControl( va, access_control )) {
    if (sctlr::BR.Get( SCTLR ) and ispriv) {
      access_control = 0x300 |
        ((((va >> 28) == 0xf ? (not sctlr::V.Get( SCTLR )) : ((va >> 31) == 1))) ? 0x1000 : 0);
    }
    else {
      access_control = 0;
    }
  }
    
  bool abort = false;
  unsigned ap = (access_control >> 8) & 7;
  switch (ap) {
  case 0b000: abort = true; break;
  case 0b001: abort = not ispriv; break;
  case 0b010: abort = not ispriv and iswrite; break;
  case 0b011: abort = false; break;
  case 0b100: abort = true; break;
  case 0b101: abort = not ispriv or iswrite; break;
  case 0b110: abort = iswrite; break;
  case 0b111: abort = true; break;
  }
  
  if (abort) {
    // TODO: Full DAbort_Translation 
    throw unisim::component::cxx::processor::arm::exception::DataAbortException();
  }
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
          uint32_t Read( CP15CPU& _cpu ) { return 0x8403c003; }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 0, 0, 0, 4 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "MPUIR, MPU Type Register"; }
          uint32_t Read( CP15CPU& _cpu )
          { return ((MPU::IRegion << 16) | (MPU::DRegion << 8) | (MPU::Unified ? 0 : 1)); }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 0, 1, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "CCSIDR, Cache Size ID Registers"; }
          uint32_t Read( CP15CPU& _cpu ) {
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
          uint32_t Read( CP15CPU& _cpu ) {
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
          uint32_t Read( CP15CPU& _cpu ) { return 0x30; /* pmsav7 */ }
        } x;
        return x;
      } break;
      
    case CP15ENCODE( 0, 2, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "CSSELR, Cache Size Selection Register"; }
          void Write( CP15CPU& _cpu, uint32_t value ) {
            dynamic_cast<CPU&>( _cpu ).csselr = value;
          }
          uint32_t Read( CP15CPU& _cpu ) { return dynamic_cast<CPU&>( _cpu ).csselr; }
        } x;
        return x;
      } break;
      
      /*********************************/
      /* Memory system fault registers */
      /*********************************/
    case CP15ENCODE( 5, 0, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DFSR, Data Fault Status Register"; }
          void Write( CP15CPU& _cpu, uint32_t value ) { dynamic_cast<CPU&>( _cpu ).DFSR = value; }
          uint32_t Read( CP15CPU& _cpu ) { return dynamic_cast<CPU&>( _cpu ).DFSR; }
        } x;

      } break;

    case CP15ENCODE( 5, 0, 0, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "IFSR, Instruction Fault Status Register"; }
          void Write( CP15CPU& _cpu, uint32_t value ) { dynamic_cast<CPU&>( _cpu ).IFSR = value; }
          uint32_t Read( CP15CPU& _cpu ) { return dynamic_cast<CPU&>( _cpu ).IFSR; }
        } x;

      } break;

    case CP15ENCODE( 6, 0, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DFAR, Data Fault Address Register"; }
          void Write( CP15CPU& _cpu, uint32_t value ) { dynamic_cast<CPU&>( _cpu ).DFAR = value; }
          uint32_t Read( CP15CPU& _cpu ) { return dynamic_cast<CPU&>( _cpu ).DFAR; }
        } x;

      } break;

    case CP15ENCODE( 6, 0, 0, 2 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "IFAR, Instruction Fault Address Register"; }
          void Write( CP15CPU& _cpu, uint32_t value ) { dynamic_cast<CPU&>( _cpu ).IFAR = value; }
          uint32_t Read( CP15CPU& _cpu ) { return dynamic_cast<CPU&>( _cpu ).IFAR; }
        } x;

      } break;

    case CP15ENCODE( 6, 0, 1, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DRBAR, Data Region Base Address Register"; }
          uint32_t& reg( CPU& cpu ) { return cpu.mpu.DR[cpu.mpu.RGNR].base_address; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( dynamic_cast<CPU&>( _cpu ) ) = value; }
          uint32_t Read( CP15CPU& _cpu ) { return reg( dynamic_cast<CPU&>( _cpu ) ); }
        } x;

      } break;
      
    case CP15ENCODE( 6, 0, 1, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "IRBAR, Instruction Region Base Address Register"; }
          uint32_t& reg( CPU& cpu ) { return cpu.mpu.IR[cpu.mpu.RGNR].base_address; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( dynamic_cast<CPU&>( _cpu ) ) = value; }
          uint32_t Read( CP15CPU& _cpu ) { return reg( dynamic_cast<CPU&>( _cpu ) ); }
        } x;
        
      } break;
      
    case CP15ENCODE( 6, 0, 1, 2 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DRSR, Data Region Size and Enable Register"; }
          uint32_t& reg( CPU& cpu ) { return cpu.mpu.DR[cpu.mpu.RGNR].size_enable; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( dynamic_cast<CPU&>( _cpu ) ) = value; }
          uint32_t Read( CP15CPU& _cpu ) { return reg( dynamic_cast<CPU&>( _cpu ) ); }
        } x;

      } break;
      
    case CP15ENCODE( 6, 0, 1, 3 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "IRSR, Instruction Region Size and Enable Register"; }
          uint32_t& reg( CPU& cpu ) { return cpu.mpu.IR[cpu.mpu.RGNR].size_enable; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( dynamic_cast<CPU&>( _cpu ) ) = value; }
          uint32_t Read( CP15CPU& _cpu ) { return reg( dynamic_cast<CPU&>( _cpu ) ); }
        } x;
        
      } break;
      
    case CP15ENCODE( 6, 0, 1, 4 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DRACR, Data Region Access Control Register"; }
          uint32_t& reg( CPU& cpu ) { return cpu.mpu.DR[cpu.mpu.RGNR].access_control; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( dynamic_cast<CPU&>( _cpu ) ) = value; }
          uint32_t Read( CP15CPU& _cpu ) { return reg( dynamic_cast<CPU&>( _cpu ) ); }
        } x;

      } break;
      
    case CP15ENCODE( 6, 0, 1, 5 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "IRACR, Instruction Region Access Control Register"; }
          uint32_t& reg( CPU& cpu ) { return cpu.mpu.IR[cpu.mpu.RGNR].access_control; }
          void Write( CP15CPU& _cpu, uint32_t value ) { reg( dynamic_cast<CPU&>( _cpu ) ) = value; }
          uint32_t Read( CP15CPU& _cpu ) { return reg( dynamic_cast<CPU&>( _cpu ) ); }
        } x;
        
      } break;
      
      /***************************************************************
       * Cache maintenance, address translation, and other functions *
       ***************************************************************/
    case CP15ENCODE( 7, 0, 5, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "ICIALLU, Invalidate all instruction caches to PoU"; }
          void Write( CP15CPU& _cpu, uint32_t value ) {
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
          void Write( CP15CPU& _cpu, uint32_t value ) {
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
          void Write( CP15CPU& _cpu, uint32_t value ) {
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
          void Write( CP15CPU& _cpu, uint32_t value ) {
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
          void Write( CP15CPU& _cpu, uint32_t value ) {
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
          void Write( CP15CPU& _cpu, uint32_t value ) {
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
          void Write( CP15CPU& _cpu, uint32_t value ) {
            /* No cache, basically nothing to do */
            //_cpu.logger << DebugWarning << "DCCISW <- " << std::hex << value << std::dec << EndDebugWarning;
          }
        } x;
        return x;
      } break;
      
    }
  
  // Fall back to parent cpu CP15 registers
  return this->PCPU::CP15GetRegister( crn, opcode1, crm, opcode2 );
}

} // end of namespace pmsav7

template struct unisim::component::cxx::processor::arm::CPU<unisim::component::cxx::processor::arm::pmsav7::ARMv7emu>;

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
