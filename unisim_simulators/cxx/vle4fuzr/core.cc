/*
 *  Copyright (c) 2019,
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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr), Gilles Mouchard <gilles.mouchard@cea.fr>
 */

#include "core.hh"
#include <unisim/component/cxx/processor/arm/cpu.tcc>
#include <unisim/component/cxx/processor/arm/cpu.hh>
#include <unisim/component/cxx/processor/arm/disasm.hh>
#include <unisim/component/cxx/processor/arm/psr.hh>
#include <top_arm32.tcc>
#include <top_thumb.tcc>

// template class unisim::component::cxx::processor::arm::CPU<ARMv7cfg>;

Core::Core( char const* name, unisim::kernel::Object* parent, bool is_thumb )
  : unisim::kernel::Object(name, parent)
  , CPU(name, parent)
  , arm32_decoder()
  , thumb_decoder()
{}

void
Core::StepInstruction()
{
  /* Instruction boundary next_insn_addr becomes current_insn_addr */
  uint32_t insn_addr = this->current_insn_addr = this->next_insn_addr, insn_length = 0;

  using namespace unisim::component::cxx::processor::arm;
  
  try
    {
      // Instruction Fetch Decode and Execution (may generate exceptions
      // known as synchronous aborts since their occurences are a direct
      // consequence of the instruction execution).
      if (cpsr.Get( T ))
        {
          /* Thumb state */
          isa::thumb::CodeType insn;
          ReadInsn(insn_addr, insn);
    
          /* Decode current PC */
          isa::thumb::Operation<Core>* op = thumb_decoder.Decode(insn_addr, insn);
    
          /* update PC register value before execution */
          insn_length = op->GetLength() / 8;
          this->gpr[15] = insn_addr + 4;
          this->next_insn_addr = insn_addr + insn_length;
    
          /* Execute instruction */
          asm volatile( "thumb_operation_execute:" );
          op->execute( *this );
    
          this->ITAdvance();
          //op->profile(profile);
        }
  
      else
        {
          /* Arm32 state */
    
          /* fetch instruction word from memory */
          isa::arm32::CodeType insn;
          ReadInsn(insn_addr, insn);
      
          /* Decode current PC */
          isa::arm32::Operation<Core>* op = arm32_decoder.Decode(insn_addr, insn);
    
          /* update PC register value before execution */
          insn_length = op->GetLength() / 8;
          this->gpr[15] = insn_addr + 8;
          this->next_insn_addr = insn_addr + insn_length;
    
          /* Execute instruction */
          asm volatile( "arm32_operation_execute:" );
          op->execute( *this );
          //op->profile(profile);
        }
    
      if (unlikely(requires_commit_instruction_reporting and memory_access_reporting_import))
        memory_access_reporting_import->ReportCommitInstruction(this->current_insn_addr, insn_length);
    
      instruction_counter++; /* Instruction regularly finished */
    }
  
  catch (SVCException const& svexc) {
    /* Resuming execution, since SVC exceptions are explicitly
     * requested from regular instructions. ITState will be updated as
     * needed by TakeSVCException (as done in the ARM spec). */
    if (unlikely( requires_commit_instruction_reporting and memory_access_reporting_import ))
      memory_access_reporting_import->ReportCommitInstruction(this->current_insn_addr, insn_length);

    instruction_counter++; /* Instruction regularly finished */
    
    this->TakeSVCException();
  }
  
  catch (DataAbortException const& daexc) {
    /* Abort execution, and take processor to data abort handler */
    
    if (unlikely(trap_reporting_import))
      trap_reporting_import->ReportTrap( *this, "Data Abort Exception" );
    
    this->TakeDataOrPrefetchAbortException(true); // TakeDataAbortException
  }
  
  catch (PrefetchAbortException const& paexc) {
    /* Abort execution, and take processor to prefetch abort handler */
    
    if (unlikely(trap_reporting_import))
      trap_reporting_import->ReportTrap( *this, "Prefetch Abort Exception" );
    
    this->TakeDataOrPrefetchAbortException(false); // TakePrefetchAbortException
  }
  
  catch (UndefInstrException const& undexc) {
    /* Abort execution, and take processor to undefined handler */
    
    if (unlikely(trap_reporting_import))
      trap_reporting_import->ReportTrap( *this, "Undefined Exception" );
    
    this->TakeUndefInstrException();
  }
  
  catch (Exception const& exc) {
    logger << DebugError << "Unimplemented exception (" << exc.what() << ")"
           << " pc: " << std::hex << current_insn_addr << std::dec
           << EndDebugError;
    this->Stop(-1);
  }
}  


/** Refill the Instruction Prefetch Buffer from the memory system
 * (through the instruction cache if present).
 *
 * This method allows the user to prefetch instructions from the memory
 * system, that is, it tries to read from the pertinent caches and if
 * failed from the external memory system.
 * 
 * @param mva      the modified virtual address of the fetched instruction
 * @param line_loc the physical address descriptor used to refill the cache line
 */
void 
Core::RefillInsnPrefetchBuffer(uint32_t mva, AddressDescriptor const& line_loc)
{
  this->ipb_base_address = line_loc.address;
  
  // No instruction cache present, just request the insn to the
  // memory system.
  if (not PhysicalFetchMemory(mva & -(IPB_LINE_SIZE), line_loc.address, &this->ipb_bytes[0], IPB_LINE_SIZE, line_loc.attributes)) {
    DataAbort(mva, line_loc.address, 0, 0, mat_exec, DAbort_SyncExternal, false, false, true, false, false);
  }
  
  if (unlikely(requires_memory_access_reporting and memory_access_reporting_import))
    memory_access_reporting_import->
      ReportMemoryAccess(unisim::util::debug::MAT_READ, unisim::util::debug::MT_INSN, line_loc.address, IPB_LINE_SIZE);
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
Core::ReadInsn(uint32_t address, unisim::component::cxx::processor::arm::isa::arm32::CodeType& insn)
{
  AddressDescriptor loc(address & -(IPB_LINE_SIZE));
  TranslateAddress<PlainAccess>( loc, cpsr.Get(M) != USER_MODE, mat_exec, IPB_LINE_SIZE );
  uint32_t buffer_index = address % (IPB_LINE_SIZE);
  
  if (unlikely(ipb_base_address != loc.address))
    {
      RefillInsnPrefetchBuffer( address, loc );
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
Core::ReadInsn(uint32_t address, unisim::component::cxx::processor::arm::isa::thumb::CodeType& insn)
{
  AddressDescriptor loc(address & -(IPB_LINE_SIZE));
  bool ispriv = cpsr.Get(M) != USER_MODE;
  TranslateAddress<PlainAccess>( loc, ispriv, mat_exec, IPB_LINE_SIZE );
  intptr_t buffer_index = address % (IPB_LINE_SIZE);
    
  if (unlikely(ipb_base_address != loc.address))
    {
      RefillInsnPrefetchBuffer( address, loc );
    }
  
  // In ARMv7, instruction fetch ignores "Endianness execution state bit"
  insn.str[0] = ipb_bytes[buffer_index+0];
  insn.str[1] = ipb_bytes[buffer_index+1];
  if (unlikely((buffer_index+2) >= IPB_LINE_SIZE)) {
    address = loc.address = address + 2;
    TranslateAddress<PlainAccess>( loc, ispriv, mat_exec, IPB_LINE_SIZE );
    RefillInsnPrefetchBuffer( address, loc );
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
Core::CallSupervisor( uint16_t imm )
{
  if (linux_os_import) {
    // we are executing on linux emulation mode, use linux_os_import
    try {
      linux_os_import->ExecuteSystemCall(imm);
    }
    catch (Exception const& e)
      {
        std::cerr << e.what() << std::endl;
        this->Stop( -1 );
      }
  } else {
    //trap_reporting_import->ReportTrap(*this, "CallSupervisor");
    
    if (verbose) {
      static struct ArmLinuxOS : public unisim::util::os::linux_os::Linux<uint32_t, uint32_t>
      {
        typedef unisim::util::os::linux_os::ARMTS<unisim::util::os::linux_os::Linux<uint32_t,uint32_t> > ArmTarget;
      
        ArmLinuxOS( CPU* _cpu )
          : unisim::util::os::linux_os::Linux<uint32_t, uint32_t>( _cpu->logger.DebugInfoStream(), _cpu->logger.DebugWarningStream(), _cpu->logger.DebugErrorStream(), _cpu, _cpu, _cpu )
        {
          SetTargetSystem(new ArmTarget( "arm-eabi", *this ));
        }
        ~ArmLinuxOS() { delete GetTargetSystem(); }
      } arm_linux_os( this );
    
      logger << DebugInfo << "PC: 0x" << std::hex << GetCIA() << EndDebugInfo;
      arm_linux_os.LogSystemCall( imm );
    }

    // we are executing on full system mode
    this->PCore::CallSupervisor( imm );
  }
}

