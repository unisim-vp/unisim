/*
 *  Copyright (c) 2007-2016,
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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_VMSAV8_CPU_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_VMSAV8_CPU_TCC__

#include <unisim/component/cxx/processor/arm/isa_arm64.tcc>
#include <unisim/component/cxx/processor/arm/isa/arm64/disasm.hh>
#include <unisim/util/likely/likely.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/debug/simple_register.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace vmsav8 {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;

/** Constructor.
 * Initializes CPU
 *
 * @param name the name that will be used by the UNISIM service 
 *   infrastructure and will identify this object
 * @param parent the parent object of this object
 */
template <class CONFIG>
CPU<CONFIG>::CPU(const char *name, Object *parent)
  : unisim::kernel::service::Object(name, parent)
  , Client<unisim::service::interfaces::DebugControl<uint64_t> >(name, parent)
  , Client<unisim::service::interfaces::TrapReporting>(name, parent)
  , Client<unisim::service::interfaces::SymbolTableLookup<uint64_t> >(name, parent)
  , Client<unisim::service::interfaces::Memory<uint64_t> >(name, parent)
  , Client<unisim::service::interfaces::LinuxOS>(name, parent)
  , Client<unisim::service::interfaces::MemoryAccessReporting<uint64_t> >(name, parent)
  , Service<unisim::service::interfaces::Registers>(name, parent) 
  , Service<unisim::service::interfaces::Memory<uint64_t> >(name, parent)
  , Service<unisim::service::interfaces::Disassembly<uint64_t> >(name, parent)
  , Service<unisim::service::interfaces::MemoryAccessReportingControl>(name, parent)
  , Service<unisim::service::interfaces::MemoryInjection<uint64_t> >(name, parent)
    /*Exports*/
  , registers_export("registers-export", this)
  , memory_export("memory-export", this)
  , disasm_export("disasm-export", this)
  , memory_access_reporting_control_export("memory-access-reporting-control-export", this)
  , memory_injection_export("memory-injection-export", this)
    /*Imports*/
  , debug_control_import("debug-control-import", this)
  , instruction_counter_trap_reporting_import("instruction-counter-trap-reporting-import", this)
  //, exception_trap_reporting_import("exception-trap-reporting-import", this)
  , symbol_table_lookup_import("symbol-table-lookup-import", this)
  , memory_import("memory-import", this)
  , memory_access_reporting_import("memory-access-reporting-import", this)
  , linux_os_import("linux-os-import", this)
  /* privates */
  , logger(*this)
  , verbose(false)
  , requires_finished_instruction_reporting(false)
  , requires_memory_access_reporting(false)
  // // , icache("icache", this)
  // // , dcache("dcache", this)
  // , arm32_decoder()
  // , thumb_decoder()
  // , csselr(0)
  // , DFSR()
  // , IFSR()
  // , DFAR()
  // , IFAR()
  // , mmu()
  // , instruction_counter(0)
  // , trap_on_instruction_counter(0)
  // , ipb_base_address( -1 )
  // , linux_printk_buf_addr( 0 )
  // , linux_printk_buf_size( 0 )
  // , linux_printk_snooping( false )
  // , halt_on_addr( uint32_t(-1) )
  // , halt_on_location()
  // , param_verbose("verbose", this, this->PCPU::verbose, "Activate the verbose system.")
  // , param_trap_on_instruction_counter("trap-on-instruction-counter", this, trap_on_instruction_counter,
  //                                     "Produce a trap when the given instruction count is reached.")
  // , param_linux_printk_snooping( "linux-printk-snooping", this, linux_printk_snooping, "Activate the printk snooping" )
  // , param_halt_on_location( "halt-on-location", this, halt_on_location,
  //                           "Tell the CPU to halt simulation on a specific instruction (address or symbol)." )
  // , stat_instruction_counter("instruction-counter", this, instruction_counter, "Number of instructions executed.")
{
  {
    unisim::service::interfaces::Register* dbg_reg = 0;
    unisim::kernel::service::Register<uint64_t>* var_reg = 0;
    // initialize the registers debugging interface for the 32 general purpose registers
    for (unsigned idx = 0; idx < 32; idx++)
      {
        std::string name, pretty_name, description;
        { std::stringstream ss; ss << "x" << idx; name = ss.str(); }
        { std::stringstream ss; ss << isa::arm64::DisasmGSXR( idx ); pretty_name = ss.str(); }
        { std::stringstream ss; ss << "Logical register #" << idx << ": " << pretty_name; description = ss.str(); }
        dbg_reg = new unisim::util::debug::SimpleRegister<uint64_t>( pretty_name, &gpr[idx] );
        registers_registry[pretty_name] = dbg_reg;
        if (name != pretty_name) {
          registers_registry[name] = dbg_reg;
          description =  description + ", " + name;
        }
        var_reg = new unisim::kernel::service::Register<uint64_t>( pretty_name.c_str(), this, gpr[idx], description.c_str() );
        variable_register_pool.insert( var_reg );
      }

    /** Specific Program Counter Register Debugging Accessor */
    struct ProgramCounterRegister : public unisim::service::interfaces::Register
    {
      ProgramCounterRegister( CPU& _cpu ) : cpu(_cpu) {}
      virtual const char *GetName() const { return "pc"; }
      virtual void GetValue( void* buffer ) const { *((uint64_t*)buffer) = cpu.next_insn_addr; }
      virtual void SetValue( void const* buffer ) { cpu.next_insn_addr = *((uint64_t*)buffer); }
      virtual int  GetSize() const { return 8; }
      virtual void Clear() { /* Clear is meaningless for PC */ }
      CPU&        cpu;
    };

    dbg_reg = new ProgramCounterRegister( *this );
    registers_registry["pc"] = registers_registry["r15"] = dbg_reg;
    var_reg = new unisim::kernel::service::Register<uint64_t>( "pc", this, this->next_insn_addr, "Logical Register #15: pc, r15" );
    variable_register_pool.insert( var_reg );
  }  
}

/** Destructor.
 *
 * Takes care of releasing:
 *  - Debug Registers
 */
template <class CONFIG>
CPU<CONFIG>::~CPU()
{
}

/** Get a register by its name.
 * Gets a register interface to the register specified by name.
 *
 * @param name the name of the requested register
 *
 * @return a pointer to the RegisterInterface corresponding to name
 */
template <class CONFIG>
unisim::service::interfaces::Register*
CPU<CONFIG>::GetRegister( const char* name )
{
  RegistersRegistry::iterator itr = registers_registry.find( name );
  if (itr != registers_registry.end())
    return itr->second;
  else
    return 0;
}

/** Scan available registers for the Registers interface
 * 
 *  Allows clients of the Registers interface to scan available
 * register by providing a suitable RegisterScanner interface.
 */
template <class CONFIG>
void
CPU<CONFIG>::ScanRegisters( unisim::service::interfaces::RegisterScanner& scanner )
{
  // scanner.Append( this->GetRegister( "r0" ) );
  // scanner.Append( this->GetRegister( "r1" ) );
  // scanner.Append( this->GetRegister( "r2" ) );
  // scanner.Append( this->GetRegister( "r3" ) );
  // scanner.Append( this->GetRegister( "r4" ) );
  // scanner.Append( this->GetRegister( "r5" ) );
  // scanner.Append( this->GetRegister( "r6" ) );
  // scanner.Append( this->GetRegister( "r7" ) );
  // scanner.Append( this->GetRegister( "r8" ) );
  // scanner.Append( this->GetRegister( "r9" ) );
  // scanner.Append( this->GetRegister( "r10" ) );
  // scanner.Append( this->GetRegister( "r11" ) );
  // scanner.Append( this->GetRegister( "r12" ) );
  // scanner.Append( this->GetRegister( "sp" ) );
  // scanner.Append( this->GetRegister( "lr" ) );
  // scanner.Append( this->GetRegister( "pc" ) );
  // TODO: should expose CPSR (and most probably the APSR view)
  // scanner.Append( this->GetRegister( "cpsr" ) );
}


/** Perform a non intrusive read access.
 *
 *   This method performs non-intrusive read. This method does not
 * change the architectural state of the system (caches, mmu, tlb...).
 * External accesses are done through External[Read|Write]Memory.
 *
 * @param addr the address to read from
 * @param buffer the buffer to fill with the read data (must be allocated
 *   by the caller)
 * @param size the amount of data to read
 *
 * @return true on success, false otherwise
 */
template <class CONFIG>
bool
CPU<CONFIG>::ReadMemory( uint64_t addr, void* buffer, uint32_t size )
{
  uint8_t* rbuffer = (uint8_t*)buffer;

  return ExternalReadMemory( addr, &rbuffer[0], size );
}

/** Perform a non intrusive write access.
 *
 *   This method performs non-intrusive write. This method does not
 * change the architectural state of the system (caches, mmu, tlb...).
 * External accesses are done through External[Read|Write]Memory.
 *
 * @param addr the address to write to
 * @param buffer the data to write
 * @param size the amount of data to write
 *
 * @return true on success, false otherwise
 */
template <class CONFIG>
bool
CPU<CONFIG>::WriteMemory( uint64_t addr, void const* buffer, uint32_t size )
{
  uint8_t const* wbuffer = (uint8_t const*)buffer;
  
  return ExternalWriteMemory( addr, &wbuffer[0], size );
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
template <class CONFIG>
std::string 
CPU<CONFIG>::Disasm(uint64_t addr, uint64_t& next_addr)
{
  throw 0;
  // std::stringstream buffer;
  // if (cpsr.Get( T ))
  //   {
  //     buffer << "[THUMB2]";
    
  //     uint8_t insn_bytes[4];
  //     isa::thumb2::CodeType insn;
  //     isa::thumb2::Operation<CPU> *op = 0;
  //     if (not ReadMemory(addr, insn_bytes, 4))
  //       {
  //         buffer << "Could not read from memory";
  //         return buffer.str();
  //       }
  
  //     // Instruction fetch ignores "Endianness execution state bit"
  //     insn.str[0] = insn_bytes[0];
  //     insn.str[1] = insn_bytes[1];
  //     insn.str[2] = insn_bytes[2];
  //     insn.str[3] = insn_bytes[3];
  //     insn.size = 32;
    
  //     op = thumb_decoder.Decode(addr, insn);
  //     unsigned insn_length = op->GetLength();
  //     if (insn_length % 16) throw std::logic_error("Bad T2 instruction size");
    
  //     buffer << "0x";
  //     buffer << op->GetEncoding() << " ";
  //     op->disasm(*this, buffer);

  //     next_addr = addr + (insn_length / 8);
  //   } 
  // else 
  //   {
  //     buffer << "[ARM32]";
    
  //     isa::arm32::Operation<CPU> * op = NULL;
  //     uint32_t insn;
  //     if (not ReadMemory(addr, &insn, 4))
  //       {
  //         buffer << "Could not read from memory";
  //         return buffer.str();
  //       }
  //     if (GetEndianness() == unisim::util::endian::E_BIG_ENDIAN)
  //       insn = BigEndian2Host(insn);
  //     else
  //       insn = LittleEndian2Host(insn);

  //     op = arm32_decoder.Decode(addr, insn);
  //     buffer << "0x" << std::hex;
  //     buffer.fill('0'); buffer.width(8); 
  //     buffer << op->GetEncoding() << std::dec << " ";
  //     op->disasm(*this, buffer);

  //     next_addr = addr + 4;
  //   }

  // return buffer.str();
  return "";
}

/** Set/unset the reporting of finishing instructions.
 * 
 * @param report if true set the reporting of finishing instructions, 
 *   unset otherwise
 */
template <class CONFIG>
void 
CPU<CONFIG>::RequiresFinishedInstructionReporting( bool report )
{
  requires_finished_instruction_reporting = report;
}

/** Set/unset the reporting of memory accesses.
 *
 * @param report if true/false sets/unsets the reporting of memory
 *        acesseses
 */
template <class CONFIG>
void
CPU<CONFIG>::RequiresMemoryAccessReporting( bool report )
{
  requires_memory_access_reporting = report;
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
template <class CONFIG>
bool 
CPU<CONFIG>::InjectReadMemory( uint64_t addr, void* buffer, uint32_t size )
{
  throw 0;
  // uint8_t* rbuffer = (uint8_t*)buffer;

  // // No data cache, just send request to the memory subsystem
  // for (uint32_t index = 0; size != 0; ++index, --size)
  //   {
  //     uint32_t ef_addr = addr + index;
  //     if (not PrRead(ef_addr, &rbuffer[index], 1))
  //       return false;
  //   }

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
template <class CONFIG>
bool
CPU<CONFIG>::InjectWriteMemory( uint64_t addr, void const* buffer, uint32_t size )
{
  throw 0;
  // uint8_t const* wbuffer = (uint8_t const*)buffer;
  
  // // No data cache, just send the request to the memory subsystem
  // for (uint32_t index = 0; size != 0; ++index, --size)
  //   {
  //     uint32_t ef_addr = addr + index;
  //     if (not PrWrite( ef_addr, &wbuffer[index], 1 ))
  //       return false;
  //   }

  return true;
}

/** Execute one complete instruction.
 *
 * Proceed to execute one instruction. Inter-instruction exceptions
 * should have been handles.  The "step" method, commit next
 * instruction has the current instruction and proceed with
 * instruction execution, as complete as possible.  Any synchronous
 * exception that occurs will immediately be signaled.
 */
template <class CONFIG>
void 
CPU<CONFIG>::StepInstruction()
{
  /* Instruction boundary next_insn_addr becomes current_insn_addr */
  uint64_t insn_addr = this->current_insn_addr = this->next_insn_addr;
  
  //  if (insn_addr == halt_on_addr) { Stop(0); return; }
  
  // if (unlikely(instruction_counter_trap_reporting_import and (trap_on_instruction_counter == instruction_counter)))
  //   instruction_counter_trap_reporting_import->ReportTrap(*this,"Reached instruction counter");
  
  if (unlikely(requires_finished_instruction_reporting and memory_access_reporting_import))
    memory_access_reporting_import->ReportFetchInstruction(insn_addr);

  typedef unisim::service::interfaces::DebugControl<uint64_t> DebugControl;
    
  if (debug_control_import)
    {
      for (bool proceed = false; not proceed; )
        {
          switch (debug_control_import->FetchDebugCommand( insn_addr ))
            {
            case DebugControl::DBG_STEP: 
              proceed = true;
              break;
            case DebugControl::DBG_SYNC:
              Sync();
              continue;
              break;
            case DebugControl::DBG_RESET: /* TODO : memory_interface->Reset(); */ break;
            case DebugControl::DBG_KILL:
              Stop(0);
              return;
            }
        }
    }
  
  try {
    // Instruction Fetch Decode and Execution (may generate exceptions
    // known as synchronous aborts since their occurences are a direct
    // consequence of the instruction execution).
    
    /* fetch instruction word from memory */
    isa::arm64::CodeType insn;
    ReadInsn(insn_addr, insn);
    
    /* Decode current PC */
    isa::arm64::Operation<CPU>* op;
    std::cerr << "0x" << std::hex << insn << std::endl;
    op = decoder.Decode(insn_addr, insn);
    
    this->next_insn_addr += 4;
    
    /* Execute instruction */
    asm volatile( "arm64_operation_execute:" );
    op->execute( *this );
    // //op->profile(profile);
    
    throw 0;
      
    
    // if (unlikely( requires_finished_instruction_reporting and memory_access_reporting_import ))
    //   memory_access_reporting_import->ReportCommitInstruction(this->current_insn_addr);
    
    // instruction_counter++; /* Instruction regularly finished */
  }
  
  // catch (SVCException const& svexc) {
  //   /* Resuming execution, since SVC exceptions are explicitly
  //    * requested from regular instructions. ITState will be updated as
  //    * needed by TakeSVCException (as done in the ARM spec). */
  //   if (unlikely( requires_finished_instruction_reporting and memory_access_reporting_import ))
  //     memory_access_reporting_import->ReportCommitInstruction(this->current_insn_addr);

  //   instruction_counter++; /* Instruction regularly finished */
    
  //   this->TakeSVCException();
  // }
  
  // catch (DataAbortException const& daexc) {
  //   /* Abort execution, and take processor to data abort handler */
    
  //   if (unlikely( exception_trap_reporting_import))
  //     exception_trap_reporting_import->ReportTrap( *this, "Data Abort Exception" );
    
  //   this->TakeDataOrPrefetchAbortException(true); // TakeDataAbortException
  // }
  
  // catch (PrefetchAbortException const& paexc) {
  //   /* Abort execution, and take processor to prefetch abort handler */
    
  //   if (unlikely( exception_trap_reporting_import))
  //     exception_trap_reporting_import->ReportTrap( *this, "Prefetch Abort Exception" );
    
  //   this->TakeDataOrPrefetchAbortException(false); // TakePrefetchAbortException
  // }
  
  // catch (UndefInstrException const& undexc) {
  //   /* Abort execution, and take processor to undefined handler */
    
  //   if (unlikely( exception_trap_reporting_import))
  //     exception_trap_reporting_import->ReportTrap( *this, "Undefined Exception" );
    
  //   this->TakeUndefInstrException();
  // }
  
  catch (std::exception const& exc) {
    logger << DebugError << "Unimplemented exception (" << exc.what() << ")"
           << " pc: " << std::hex << current_insn_addr << std::dec
           // << ", cpsr: " << std::hex << cpsr.bits() << std::dec
           // << " (" << cpsr << ")"
           << EndDebugError;
    this->Stop(-1);
  }
}

/** Reads ARM32 instructions from the memory system
 * This method allows the user to read instructions from the memory system,
 *   that is, it tries to read from the pertinent caches and if failed from
 *   the external memory system.
 * 
 * @param address the address to read data from
 * @param val the buffer to fill with the read data
 */
template <class CONFIG>
void 
CPU<CONFIG>::ReadInsn( uint64_t address, isa::arm64::CodeType& insn )
{
  uint64_t base_address = address & -IPB_LINE_SIZE;
  unsigned buffer_index = address % IPB_LINE_SIZE;
  
  if (unlikely(ipb_base_address != base_address))
    {
      RefillInsnPrefetchBuffer( base_address );
    }
  
  uint32_t word;
  memcpy( &word, &ipb_bytes[buffer_index], 4 );
  
  // In ARMv8
  insn = Target2Host(unisim::util::endian::E_LITTLE_ENDIAN, word);
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
template <class CONFIG>
bool
CPU<CONFIG>::RefillInsnPrefetchBuffer(uint64_t base_address)
{
  this->ipb_base_address = base_address;
  
  // No instruction cache present, just request the insn to the
  // memory system.
  if (not PrRead(base_address, &this->ipb_bytes[0], IPB_LINE_SIZE))
    return false;
  
  if (unlikely(requires_memory_access_reporting and memory_access_reporting_import))
    memory_access_reporting_import->
      ReportMemoryAccess(unisim::util::debug::MAT_READ, unisim::util::debug::MT_INSN, base_address, IPB_LINE_SIZE);
  
  return true;
}

/** Signal an undefined instruction
 *
 * This method allows an invalid instruction to signal the undefined
 * instruction to the CPU.
 */
template <class CONFIG>
void
CPU<CONFIG>::UndefinedInstruction( isa::arm64::Operation<CPU>* insn )
{
  std::ostringstream oss;
  insn->disasm( *this, oss );
  
  logger << DebugWarning << "Undefined instruction"
         << " @" << std::hex << current_insn_addr << std::dec
         << ": " << oss.str()
         << EndDebugWarning;
  
  if (linux_os_import)
    this->Stop( -1 );
  else
    throw 0;
}

} // end of namespace vmsav8
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_VMSAV8_CPU_TCC__
