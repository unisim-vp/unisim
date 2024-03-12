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
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */


#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_VMSAV8_CPU_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_VMSAV8_CPU_TCC__

#include <unisim/component/cxx/processor/arm/vmsav8/cpu.hh>
#include <unisim/component/cxx/processor/arm/isa_arm64.tcc>
#include <unisim/component/cxx/processor/opcache/opcache.tcc>
#include <unisim/component/cxx/processor/arm/isa/arm64/disasm.hh>
#include <unisim/component/cxx/processor/arm/exception.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/variable/variable.hh>
#include <unisim/util/debug/simple_register.hh>
#include <unisim/util/likely/likely.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/debug/simple_register.hh>
#include <iomanip>

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
template <class CPU_IMPL>
CPU<CPU_IMPL>::CPU(const char *name, Object *parent)
  : unisim::kernel::Object(name, parent)
  , Client<unisim::service::interfaces::DebugYielding>(name, parent)
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
  , debug_yielding_import("debug-yielding-import", this)
  , trap_reporting_import("trap-reporting-import", this)
  , symbol_table_lookup_import("symbol-table-lookup-import", this)
  , memory_import("memory-import", this)
  , memory_access_reporting_import("memory-access-reporting-import", this)
  , linux_os_import("linux-os-import", this)
  /* privates */
  , logger(*this)
  , verbose(false)
  , ipb()
  , requires_memory_access_reporting(false)
  , requires_fetch_instruction_reporting(false)
  , requires_commit_instruction_reporting(false)
  // , instruction_counter(0)
  // , trap_on_instruction_counter(0)
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
    unisim::kernel::VariableBase* var_reg = 0;
    //unisim::kernel::variable::Register<uint64_t>* var_reg = 0;

    // initialize the registers debugging interface for the 32 general purpose registers
    for (unsigned idx = 0; idx < 32; idx++)
      {
        std::string name, pretty_name, description;
        { std::stringstream ss; ss << "x" << idx; name = ss.str(); }
        { std::stringstream ss; ss << isa::arm64::DisasmGSXR( idx ); pretty_name = ss.str(); }
        { std::stringstream ss; ss << "Logical register #" << idx << ": " << pretty_name; description = ss.str(); }
        dbg_reg = new unisim::util::debug::SimpleRegister<uint64_t>( pretty_name, &gpr[idx] );
        registers_registry.AddRegisterInterface( dbg_reg );
        if (name != pretty_name) {
          dbg_reg = new unisim::util::debug::SimpleRegister<uint64_t>( name, &gpr[idx] );
          registers_registry.AddRegisterInterface( dbg_reg );
          description =  description + ", " + name;
        }
        var_reg = new unisim::kernel::variable::Register<uint64_t>( pretty_name.c_str(), this, gpr[idx], description.c_str() );
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
    registers_registry.AddRegisterInterface( dbg_reg );
    var_reg = new unisim::kernel::variable::Register<uint64_t>( "pc", this, this->next_insn_addr, "Program Counter (Current Instruction Address)" );
    variable_register_pool.insert( var_reg );

    dbg_reg = new unisim::util::debug::SimpleRegister<uint32_t>( "nzcv", &this->nzcv );
    registers_registry.AddRegisterInterface( dbg_reg );
    var_reg = new unisim::kernel::variable::Register<uint32_t>( "nzcv", this, this->nzcv, "PSTATE.{N,Z,C,V}" );
    variable_register_pool.insert( var_reg );

    /** Specific Current Program Status Register Debugging Accessor */
    struct CurrentProgramStatusRegister : unisim::service::interfaces::Register
    {
      CurrentProgramStatusRegister( CPU& _cpu ) : cpu(_cpu) {}
      virtual const char *GetName() const { return "cpsr"; }
      virtual void GetValue( void* buffer ) const { *((uint32_t *)buffer) = cpu.nzcv << 28; }
      virtual void SetValue( void const* buffer ) { cpu.nzcv = (*(uint32_t *)buffer >> 28) & 0xf; }
      virtual int  GetSize() const { return 4; }
      virtual void Clear() { cpu.nzcv = 0; }
      CPU&        cpu;
    };
    dbg_reg = new CurrentProgramStatusRegister( *this );
    registers_registry.AddRegisterInterface( dbg_reg );
    
    struct VectorRegister : unisim::service::interfaces::Register
    {
      VectorRegister( CPU& _cpu, unsigned _reg, std::string const& _name ) : cpu(_cpu), reg(_reg), name(_name) {}
      virtual const char *GetName() const { return name.c_str(); }
      virtual void GetValue( void* buffer ) const { for(unsigned sub = 0; sub < 2; ++sub) *((uint64_t*)buffer + sub) = cpu.GetVU64(reg, sub); }
      virtual void SetValue( void const* buffer ) { for(unsigned sub = 0; sub < 2; ++sub) cpu.SetVU64(reg, sub, typename ArchTypes::U64(*((uint64_t const*)buffer + sub))); }
      virtual int  GetSize() const { return 16; }
      virtual void Clear() { cpu.ClearHighV(reg, 0); }
      CPU&            cpu;
      unsigned        reg;
      std::string     name;
    };

    for (int idx = 0; idx < 32; ++idx)
      {
        std::ostringstream regname;
        regname << "v" << idx;
        dbg_reg = new VectorRegister( *this, idx, regname.str());
        registers_registry.AddRegisterInterface( dbg_reg );
      }
    
    dbg_reg = new unisim::util::debug::SimpleRegister<uint32_t>( "fpcr", &this->fpcr );
    registers_registry.AddRegisterInterface( dbg_reg );
    var_reg = new unisim::kernel::variable::Register<uint32_t>( "fpcr", this, this->fpcr, "Floating-point Control Register" );
    variable_register_pool.insert( var_reg );
    
    dbg_reg = new unisim::util::debug::SimpleRegister<uint32_t>( "fpsr", &this->fpsr );
    registers_registry.AddRegisterInterface( dbg_reg );
    var_reg = new unisim::kernel::variable::Register<uint32_t>( "fpsr", this, this->fpsr, "Floating-point Status Register" );
    variable_register_pool.insert( var_reg );
  }
}

/** Destructor.
 *
 * Takes care of releasing:
 *  - Debug Registers
 */
template <class CPU_IMPL>
CPU<CPU_IMPL>::~CPU()
{
}

/** Get a register by its name.
 * Gets a register interface to the register specified by name.
 *
 * @param name the name of the requested register
 *
 * @return a pointer to the RegisterInterface corresponding to name
 */
template <class CPU_IMPL>
unisim::service::interfaces::Register*
CPU<CPU_IMPL>::GetRegister( const char* name )
{
  return registers_registry.GetRegister(name);
}

/** Scan available registers for the Registers interface
 *
 *  Allows clients of the Registers interface to scan available
 * register by providing a suitable RegisterScanner interface.
 */
template <class CPU_IMPL>
void
CPU<CPU_IMPL>::ScanRegisters( unisim::service::interfaces::RegisterScanner& scanner )
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
  registers_registry.ScanRegisters(scanner);
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
template <class CPU_IMPL>
bool
CPU<CPU_IMPL>::ExternalReadMemory( uint64_t addr, uint8_t* buffer, uint32_t size )
{
  return memory_import->ReadMemory( addr, buffer, size );
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
template <class CPU_IMPL>
bool
CPU<CPU_IMPL>::ExternalWriteMemory( uint64_t addr, uint8_t const* buffer, uint32_t size )
{
  return memory_import->WriteMemory( addr, buffer, size );
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
template <class CPU_IMPL>
std::string
CPU<CPU_IMPL>::Disasm(uint64_t addr, uint64_t& next_addr)
{
  std::stringstream buffer;

  uint32_t insn;
  if (not ReadMemory(addr, &insn, 4))
    {
      buffer << "Could not read from memory";
      return buffer.str();
    }

  insn = unisim::util::endian::LittleEndian2Host(insn);

  isa::arm64::Operation<CPU_IMPL>* op = decoder.Decode(addr, insn);

  buffer << std::hex << std::setfill('0') << std::setw(8) << op->GetEncoding() << std::dec << " ";
  op->disasm(*static_cast<CPU_IMPL*>(this), buffer);

  next_addr = addr + 4;

  return buffer.str();
}

/** Set/unset the reporting of memory accesses.
 *
 * @param report if true/false sets/unsets the reporting of memory
 *        acesseses
 */
template <class CPU_IMPL>
void
CPU<CPU_IMPL>::RequiresMemoryAccessReporting( unisim::service::interfaces::MemoryAccessReportingType type, bool report )
{
  switch (type) {
  case unisim::service::interfaces::REPORT_MEM_ACCESS:  requires_memory_access_reporting = report; break;
  case unisim::service::interfaces::REPORT_FETCH_INSN:  requires_fetch_instruction_reporting = report; break;
  case unisim::service::interfaces::REPORT_COMMIT_INSN: requires_commit_instruction_reporting = report; break;
  default: throw 0;
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
template <class CPU_IMPL>
bool
CPU<CPU_IMPL>::InjectReadMemory( uint64_t addr, void* buffer, uint32_t size )
{
  uint8_t* rbuffer = (uint8_t*)buffer;

  // No data cache, just send request to the memory subsystem
  for (uint32_t index = 0; size != 0; ++index, --size)
    {
      uint32_t ef_addr = addr + index;
      if (not static_cast<CPU_IMPL*>(this)->PhysicalReadMemory(ef_addr, &rbuffer[index], 1))
        return false;
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
template <class CPU_IMPL>
bool
CPU<CPU_IMPL>::InjectWriteMemory( uint64_t addr, void const* buffer, uint32_t size )
{
  uint8_t const* wbuffer = (uint8_t const*)buffer;

  // No data cache, just send the request to the memory subsystem
  for (uint32_t index = 0; size != 0; ++index, --size)
    {
      uint32_t ef_addr = addr + index;
      if (not static_cast<CPU_IMPL*>(this)->PhysicalWriteMemory( ef_addr, &wbuffer[index], 1 ))
        return false;
    }

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
template <class CPU_IMPL>
void
CPU<CPU_IMPL>::StepInstruction()
{
  /* Instruction boundary next_insn_addr becomes current_insn_addr */
  uint64_t insn_addr = this->current_insn_addr = this->next_insn_addr;

  //  if (insn_addr == halt_on_addr) { Stop(0); return; }

  // if (unlikely(trap_reporting_import and (trap_on_instruction_counter == instruction_counter)))
  //   trap_reporting_import->ReportTrap(*this,"Reached instruction counter");

  if (unlikely(requires_fetch_instruction_reporting and memory_access_reporting_import))
    memory_access_reporting_import->ReportFetchInstruction(insn_addr);

  if (debug_yielding_import)
    {
      debug_yielding_import->DebugYield();
    }

  try {
    // Instruction Fetch Decode and Execution (may generate exceptions
    // known as synchronous aborts since their occurences are a direct
    // consequence of the instruction execution).

    /* fetch instruction word from memory */
    isa::arm64::CodeType insn;
    ReadInsn(insn_addr, insn);

    /* Decode current PC */
    isa::arm64::Operation<CPU_IMPL>* op = decoder.Decode(insn_addr, insn);

    this->next_insn_addr += 4;

    // std::cerr << "@" << std::hex << insn_addr << ": " << std::hex << std::setfill('0') << std::setw(8) << insn << "; ";
    // op->disasm( *this, std::cerr );
    // std::cerr << std::endl;

    // uint64_t oldspval = GetGSR(31);

    /* Execute instruction */
    asm volatile( "arm64_operation_execute:" );
    op->execute( *static_cast<CPU_IMPL*>(this) );

    // uint64_t newspval = GetGSR(31);

    // if (newspval != oldspval) {
    //   std::cerr << std::hex  << "  SP: 0x" << oldspval << " => 0x" << newspval << std::endl;
    // }

    if (unlikely(requires_commit_instruction_reporting and memory_access_reporting_import))
      memory_access_reporting_import->ReportCommitInstruction(this->current_insn_addr, 4);

    //instruction_counter++; /* Instruction regularly finished */
  }

  // catch (SVCException const& svexc) {
  //   /* Resuming execution, since SVC exceptions are explicitly
  //    * requested from regular instructions. */
  //   if (unlikely( requires_finished_instruction_reporting and memory_access_reporting_import ))
  //     memory_access_reporting_import->ReportCommitInstruction(this->current_insn_addr);

  //   instruction_counter++; /* Instruction regularly finished */

  //   this->TakeSVCException();
  // }

  // catch (DataAbortException const& daexc) {
  //   /* Abort execution, and take processor to data abort handler */

  //   if (unlikely(trap_reporting_import))
  //     trap_reporting_import->ReportTrap( *this, "Data Abort Exception" );

  //   this->TakeDataOrPrefetchAbortException(true); // TakeDataAbortException
  // }

  // catch (PrefetchAbortException const& paexc) {
  //   /* Abort execution, and take processor to prefetch abort handler */

  //   if (unlikely(trap_reporting_import))
  //     trap_reporting_import->ReportTrap( *this, "Prefetch Abort Exception" );

  //   this->TakeDataOrPrefetchAbortException(false); // TakePrefetchAbortException
  // }

  // catch (UndefInstrException const& undexc) {
  //   /* Abort execution, and take processor to undefined handler */

  //   if (unlikely( trap_reporting_import))
  //     trap_reporting_import->ReportTrap( *this, "Undefined Exception" );

  //   this->TakeUndefInstrException();
  // }

  catch (std::exception const& exc) {
    logger << DebugError << "Unimplemented exception (" << exc.what() << ")"
           << " pc: " << std::hex << current_insn_addr << std::dec
           << EndDebugError;
    this->Stop(-1);
  }
}

template <class CPU_IMPL>
uint8_t*
CPU<CPU_IMPL>::IPB::get(CPU<CPU_IMPL>& core, U64 address)
{
  unsigned idx = address % LINE_SIZE;
  uint64_t req_base_address = address & -LINE_SIZE;

  if (base_address != req_base_address)
    {
      // No instruction cache present, just request the insn to the
      // memory system.
      if (not static_cast<CPU_IMPL&>(core).PhysicalReadMemory(req_base_address, &bytes[0], LINE_SIZE))
        {
          base_address = -1;
          return 0;
        }
      base_address = req_base_address;

      core.ReportMemoryAccess(unisim::util::debug::MAT_READ, unisim::util::debug::MT_INSN, base_address, LINE_SIZE);
    }

  return &bytes[idx];

}
/** Reads ARM32 instructions from the memory system
 * This method allows the user to read instructions from the memory system,
 *   that is, it tries to read from the pertinent caches and if failed from
 *   the external memory system.
 *
 * @param address the address to read data from
 * @param val the buffer to fill with the read data
 */
template <class CPU_IMPL>
void
CPU<CPU_IMPL>::ReadInsn( uint64_t address, isa::arm64::CodeType& insn )
{
  // ARMv8 instruction streams are always little endian
  uint32_t word = 0;
  for (uint8_t *beg = ipb.get(*this, address), *itr = &beg[4]; --itr >= beg;)
    word = word << 8 | *itr;
  insn = word;
}

/** Signal an undefined instruction with an associated instruction
 *
 * This method allows an invalid instruction to signal the undefined
 * instruction to the CPU.
 */
template <class CPU_IMPL>
void
CPU<CPU_IMPL>::UndefinedInstruction( isa::arm64::Operation<CPU_IMPL> const* insn )
{
  std::ostringstream oss;
  insn->disasm( *static_cast<CPU_IMPL*>(this), oss );

  logger << DebugWarning << "Undefined instruction (" << insn->GetName() << ") @"
         << std::hex << current_insn_addr << std::dec << ": " << oss.str() << EndDebugWarning;
  this->UndefinedInstruction();
}

/** Signal an undefined instruction
 *
 * This method allows an invalid instruction to signal the undefined
 * instruction to the CPU.
 */
template <class CPU_IMPL>
void
CPU<CPU_IMPL>::UndefinedInstruction()
{
  if (linux_os_import)
    this->Stop( -1 );
  else
    throw 0;
}

/** Performs a memory read access.
 *
 * @param buffer the byte buffer to store bytes read from memory
 * @param addr   the address of the memory read access
 * @param size   the size of the memory read access
 */
template <class CPU_IMPL>
void
CPU<CPU_IMPL>::MemRead( uint8_t* buffer, uint64_t addr, unsigned size )
{
  // Over-simplistic read from memory system
  if (not static_cast<CPU_IMPL*>(this)->PhysicalReadMemory(addr, buffer, size))
    {
      throw 0;
    }

  /* report read memory access if necessary */
  this->ReportMemoryAccess(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, addr, size);
}

/** Performs a memory write access.
 *
 * @param addr   the address of the memory write access
 * @param buffer the byte buffer from which bytes are written to memory
 * @param size   the size of the memory write access
 */
template <class CPU_IMPL>
void
CPU<CPU_IMPL>::MemWrite( uint64_t addr, uint8_t const* buffer, unsigned size )
{
  // Over-simplistic read from memory system
  if (not static_cast<CPU_IMPL*>(this)->PhysicalWriteMemory( addr, buffer, size ))
    {
      throw 0;
    }

  /* report read memory access if necessary */
  this->ReportMemoryAccess( unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, addr, size );
}

/** SoftwareBreakpoint
 *
 *  This method is called by BRK instructions to handle software breakpoints.
 * @param imm     the "imm" field of the instruction code
 */
template <class CPU_IMPL>
void
CPU<CPU_IMPL>::SoftwareBreakpoint( uint32_t imm )
{
  if (linux_os_import)
    {
      // we are executing on linux emulation mode, just report trap
      if (trap_reporting_import)
        trap_reporting_import->ReportTrap(*this, "");
    }
  else
    {
      // we are executing on full system mode
      struct SWBreakpointException {};
      throw SWBreakpointException();
    }
}

/** CallSupervisor
 *
 *  This method is called by SWI/SVC instructions to handle software interrupts.
 * @param imm     the "imm" field of the instruction code
 */
template <class CPU_IMPL>
void
CPU<CPU_IMPL>::CallSupervisor( uint32_t imm )
{
  if (linux_os_import)
    {
      // we are executing on linux emulation mode, use linux_os_import
      try
        {
          linux_os_import->ExecuteSystemCall(imm);
          // if (trap_reporting_import)
          //   trap_reporting_import->ReportTrap(*this, "CallSupervisor");
        }
      catch (std::exception const& e)
        {
          std::cerr << e.what() << std::endl;
          this->Stop( -1 );
        }
    }
  else
    {
      // if (verbose) {
      //   static struct ArmLinuxOS : public unisim::util::os::linux_os::Linux<uint32_t, uint32_t>
      //   {
      //     typedef unisim::util::os::linux_os::ARMTS<unisim::util::os::linux_os::Linux<uint32_t,uint32_t> > ArmTarget;

      //     ArmLinuxOS( CPU* _cpu )
      //       : unisim::util::os::linux_os::Linux<uint32_t, uint32_t>( _cpu->logger, _cpu, _cpu, _cpu )
      //     {
      //       SetTargetSystem(new ArmTarget( "arm-eabi", *this ));
      //     }
      //     ~ArmLinuxOS() { delete GetTargetSystem(); }
      //   } arm_linux_os( this );

      //   logger << DebugInfo << "PC: 0x" << std::hex << GetCIA() << EndDebugInfo;
      //   arm_linux_os.LogSystemCall( imm );
      // }

      // we are executing on full system mode
      throw SVCException();
    }
}

/** CallHypervisor
 *
 *  This method is called by HVC instructions to handle call to hypervisor.
 * @param imm     the "imm" field of the instruction code
 */
template <class CPU_IMPL>
void
CPU<CPU_IMPL>::CallHypervisor( uint32_t imm )
{
  struct HVCException {};
  throw HVCException();
}

/** ExceptionReturn
 *
 *  This method is called by ERET instructions to handle exception return
 */
template <class CPU_IMPL>
void
CPU<CPU_IMPL>::ExceptionReturn()
{
  struct ERET {};
  throw ERET();
  
}

/** Check access permissions for op1-n system registers
 *
 * @param op1     the "op1" field of the instruction code
 */
template <class CPU_IMPL>
void
CPU<CPU_IMPL>::CheckSystemAccess(uint8_t op1)
{

}

template <class CPU_IMPL>
void
CPU<CPU_IMPL>::SysReg::Describe(typename CPU<CPU_IMPL>::SysReg::Encoding e, char const* prefix, std::ostream& sink) const
{
  std::ostringstream buf;
  this->Describe(e, buf);
  auto && s = buf.str();
  if (not s.size())
    return;
  sink << prefix << s;
};

template <class CPU_IMPL>
void
CPU<CPU_IMPL>::SysReg::DisasmRead(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, std::ostream& sink) const
{
  Encoding e{op0, op1, crn, crm, op2};
  sink << ReadOperation() << '\t' << unisim::component::cxx::processor::arm::isa::arm64::DisasmGZXR(rt) << ", ";
  Name(e, sink);
  Describe(e, "\t; ", sink);
}

template <class CPU_IMPL>
void
CPU<CPU_IMPL>::SysReg::DisasmWrite(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, std::ostream& sink) const
{
  Encoding e{op0, op1, crn, crm, op2};
  sink << WriteOperation() << '\t';
  Name(e, sink);
  sink << ", " << unisim::component::cxx::processor::arm::isa::arm64::DisasmGZXR(rt);
  Describe(e, "\t; ", sink);
}

template <class CPU_IMPL>
void
CPU<CPU_IMPL>::SysReg::Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, CPU_IMPL& cpu, U64 value) const
{
  std::cerr << "Write error for system register.\n";
  DisasmWrite(op0, op1, crn, crm, op2, 0b11111, std::cerr); std::cerr << std::endl;
  cpu.UndefinedInstruction();
}

template <class CPU_IMPL>
typename CPU<CPU_IMPL>::U64
CPU<CPU_IMPL>::SysReg::Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, CPU_IMPL& cpu) const
{
  std::cerr << "Read error for system register.\n";
  DisasmRead(op0, op1, crn, crm, op2, 0b11111, std::cerr); std::cerr << std::endl;
  cpu.UndefinedInstruction();
  return U64();
}

#define SYSENCODE( OP0, OP1, CRN, CRM, OP2 ) ((OP0 << 16) | (OP1 << 12) | (CRN << 8) | (CRM << 4) | (OP2 << 0))

/** Get the Internal representation of the sytem register
 *
 * @param crn     the "crn" field of the instruction code
 * @param op1     the "op1" field of the instruction code
 * @param crm     the "crm" field of the instruction code
 * @param op2     the "op2" field of the instruction code
 * @return        an internal System Register
 */
template <class CPU_IMPL>
typename CPU<CPU_IMPL>::SysReg const*
CPU<CPU_IMPL>::GetSystemRegister( uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2 )
{
  typedef typename SysReg::Encoding Encoding;
  
  struct DCSysReg : public SysReg
  {
    virtual char const* ReadOperation() const override { return "dc<bad-read>"; }
    virtual char const* WriteOperation() const override { return "dc"; }
  };

  struct ICSysReg : public SysReg
  {
    virtual char const* ReadOperation() const override { return "ic<bad-read>"; }
    virtual char const* WriteOperation() const override { return "ic"; }
  };

  struct PStateSysReg : public SysReg
  {
    char const* ReadOperation() const { return "mrs<bad-read>"; }
    void DisasmWrite(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, std::ostream& sink) const override
    {
      Encoding e{op0, op1, crn, crm, op2};
      this->Name(e, sink << "msr\t");
      sink << ", " << isa::arm64::DisasmI(crm);
      this->Describe(e, "\t; ", sink);
    }
  };

  struct WOOpSysReg : public SysReg
  {
    virtual void Disasm(Encoding, std::ostream&) const = 0;
    void DisasmWrite(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, std::ostream& sink) const override
    { Encoding e{op0, op1, crn, crm, op2}; Disasm(e, sink << (rt == 31 ? "" : "<illegal write>")); }
    void DisasmRead(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, std::ostream& sink) const override
    { Encoding e{op0, op1, crn, crm, op2}; Disasm(e, sink << "<illegal read>"); }
    /* TODO: most of this instructions (if not all), doesn't accept rt != 31, and we cannot test that for now */
    virtual void Write(uint8_t, uint8_t, uint8_t, uint8_t crm, uint8_t, CPU_IMPL& cpu, U64) const override
    { /* Most of this operations have no side effect on instruction-level simulations. Thus, we provide a default empty implementation. */ }
  };

  switch (SYSENCODE( op0, op1, crn, crm, op2 ))
    {
    case SYSENCODE(0b11,0b011,0b0100,0b0010,0b000): // NZCV
      {
        static struct : public SysReg {
          char const* Name() const { return "NZCV"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "NZCV Condition flags"; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, CPU_IMPL& cpu, U64 value) const override { cpu.nzcv = U32(value >> 28) & U32(0xf); }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2,  CPU_IMPL& cpu) const override { return U64(cpu.nzcv) << 28; }
        } x; return &x;
      }

    case SYSENCODE(0b11,0b011,0b0100,0b0100,0b000): // FPCR, Floating-point Control Register
      {
        static struct : public SysReg {
          char const* Name() const { return "FPCR"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Floating-point Control Register"; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, CPU_IMPL& cpu, U64 value) const override { cpu.fpcr = U32(value) & U32(CPU_IMPL::FPCR_MASK); }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2,  CPU_IMPL& cpu) const override { return U64(cpu.fpcr); }
        } x; return &x;
      }
      
    case SYSENCODE(0b11,0b011,0b0100,0b0100,0b001): // FPSR, Floating-point Status Register
      {
        static struct : public SysReg {
          char const* Name() const { return "FPSR"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Floating-point Status Register"; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, CPU_IMPL& cpu, U64 value) const override { cpu.fpsr = U32(value) & U32(CPU_IMPL::FPSR_MASK); }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2,  CPU_IMPL& cpu) const override { return U64(cpu.fpsr); }
        } x; return &x;
      }
      
    case SYSENCODE( 0b00, 0b011, 0b0010, 0b0000, 0b111 ):
      {
        static struct : public WOOpSysReg {
          void Disasm(Encoding, std::ostream& sink) const override { sink << "xpaclri\t; Strip Pointer Authentication Code from LR"; }
        } x; return &x;
      }

    case SYSENCODE( 0b00, 0b011, 0b0010, 0b0100, 0b000 ):
    case SYSENCODE( 0b00, 0b011, 0b0010, 0b0100, 0b010 ):
    case SYSENCODE( 0b00, 0b011, 0b0010, 0b0100, 0b100 ):
    case SYSENCODE( 0b00, 0b011, 0b0010, 0b0100, 0b110 ):
      {
        static struct : public WOOpSysReg
        {
          void Disasm(typename WOOpSysReg::Encoding e, std::ostream& sink) const override
          {
            sink << "bti\t" << (e.op2 & 4 ? "j" : "") << (e.op2 & 2 ? "c" : "") << "; Branch Target Identification";
          }
        } x; return &x;
      } break;

      /*** Instruction Cache Maintenance ***/
    case SYSENCODE( 0b01, 0b000, 0b0111, 0b0101, 0b000 ):
      {
        static struct : public DCSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "iallu"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Invalidate all instruction caches to Point of Unification"; }
        } x; return &x;
      } break;

    case SYSENCODE( 0b01, 0b000, 0b0111, 0b0001, 0b000 ):
      {
        static struct : public DCSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ialluis"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Invalidate all instruction caches in Inner Shareable domain to Point of Unification"; }
        } x; return &x;
      } break;

    case SYSENCODE( 0b01, 0b011, 0b0111, 0b0101, 0b001 ):
      {
        static struct : public DCSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ivau"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Invalidate instruction cache by address to Point of Unification"; }
        } x; return &x;
      } break;

      /*** Data Cache Maintenance ***/
    case SYSENCODE(0b01,0b011,0b0111,0b0100,0b001):
      {
        static struct : public DCSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "zva"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Zero data cache by address"; }
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu, U64 addr) const override { cpu.dc_zva(addr); }
        } x; return &x;
      } break;
    case SYSENCODE(0b01,0b000,0b0111,0b1110,0b010):
      {
        static struct : public DCSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "cisw"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Clean and Invalidate data cache by set/way"; }
        } x; return &x;
      } break;
    case SYSENCODE(0b01,0b000,0b0111,0b1010,0b010):
      {
        static struct : public DCSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "csw"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Clean data cache by set/way"; }
        } x; return &x;
      } break;
    case SYSENCODE(0b01,0b011,0b0111,0b1010,0b001):
      {
        static struct : public DCSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "cvac"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Clean data cache by address to Point of Coherency"; }
        } x; return &x;
      } break;
    case SYSENCODE(0b01,0b011,0b0111,0b1011,0b001):
      {
        static struct : public DCSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "cvau"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Clean data cache by address to Point of Unification"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b01,0b000,0b0111,0b0110,0b010):
      {
        static struct : public DCSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "isw"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Invalidate data cache by set/way"; }
        } x; return &x;
      } break;
    case SYSENCODE(0b01,0b000,0b0111,0b0110,0b001):
      {
        static struct : public DCSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "ivac"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Invalidate instruction cache by address to Point of Coherency"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b01,0b011,0b0111,0b1110,0b001):
      {
        static struct : public DCSysReg {
          void Name(Encoding, std::ostream& sink) const override { sink << "civac"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Clean and Invalidate data cache by address to Point of Coherency"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0001,0b0000,0b001): // 2.1: ACTLR_EL1, Auxiliary Control Register (EL1)
      {
        static struct : public SysReg {
          char const* Name() const { return "ACTLR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Auxiliary Control Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0000,0b001): // 2.2: ACTLR_EL2, Auxiliary Control Register (EL2)
      {
        static struct : public SysReg {
          char const* Name() const { return "ACTLR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Auxiliary Control Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0001,0b0000,0b001): // 2.3: ACTLR_EL3, Auxiliary Control Register (EL3)
      {
        static struct : public SysReg {
          char const* Name() const { return "ACTLR_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Auxiliary Control Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0101,0b0001,0b000): // 2.4: AFSR0_EL1, Auxiliary Fault Status Register 0 (EL1)
      {
        static struct : public SysReg {
          char const* Name() const { return "AFSR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Auxiliary Fault Status Register 0 (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0101,0b0001,0b000): // 2.5: AFSR0_EL2, Auxiliary Fault Status Register 0 (EL2)
      {
        static struct : public SysReg {
          char const* Name() const { return "AFSR0_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Auxiliary Fault Status Register 0 (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0101,0b0001,0b000): // 2.6: AFSR0_EL3, Auxiliary Fault Status Register 0 (EL3)
      {
        static struct : public SysReg {
          char const* Name() const { return "AFSR0_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Auxiliary Fault Status Register 0 (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0101,0b0001,0b001): // 2.7: AFSR1_EL1, Auxiliary Fault Status Register 1 (EL1)
      {
        static struct : public SysReg {
          char const* Name() const { return "AFSR1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Auxiliary Fault Status Register 1 (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0101,0b0001,0b001): // 2.8: AFSR1_EL2, Auxiliary Fault Status Register 1 (EL2)
      {
        static struct : public SysReg {
          char const* Name() const { return "AFSR1_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Auxiliary Fault Status Register 1 (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0101,0b0001,0b001): // 2.9: AFSR1_EL3, Auxiliary Fault Status Register 1 (EL3)
      {
        static struct : public SysReg {
          char const* Name() const { return "AFSR1_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Auxiliary Fault Status Register 1 (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b001,0b0000,0b0000,0b111): // 2.10: AIDR_EL1, Auxiliary ID Register
      {
        static struct : public SysReg {
          char const* Name() const { return "AIDR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Auxiliary ID Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1010,0b0011,0b000): // 2.11: AMAIR_EL1, Auxiliary Memory Attribute Indirection Register (EL1)
      {
        static struct : public SysReg {
          char const* Name() const { return "AMAIR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Auxiliary Memory Attribute Indirection Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1010,0b0011,0b000): // 2.12: AMAIR_EL2, Auxiliary Memory Attribute Indirection Register (EL2)
      {
        static struct : public SysReg {
          char const* Name() const { return "AMAIR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Auxiliary Memory Attribute Indirection Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1010,0b0011,0b000): // 2.13: AMAIR_EL3, Auxiliary Memory Attribute Indirection Register (EL3)
      {
        static struct : public SysReg {
          char const* Name() const { return "AMAIR_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Auxiliary Memory Attribute Indirection Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b001,0b0000,0b0000,0b000): // 2.14: CCSIDR_EL1, Current Cache Size ID Register
      {
        static struct : public SysReg {
          char const* Name() const { return "CCSIDR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Current Cache Size ID Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b001,0b0000,0b0000,0b001): // 2.15: CLIDR_EL1, Cache Level ID Register
      {
        static struct : public SysReg {
          char const* Name() const { return "CLIDR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Cache Level ID Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1101,0b0000,0b001): // 2.16: CONTEXTIDR_EL1, Context ID Register
      {
        static struct : public SysReg {
          char const* Name() const { return "CONTEXTIDR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Context ID Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0001,0b0000,0b010): // 2.17: CPACR_EL1, Architectural Feature Access Control Register
      {
        static struct : public SysReg {
          char const* Name() const { return "CPACR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Architectural Feature Access Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0001,0b010): // 2.18: CPTR_EL2, Architectural Feature Trap Register (EL2)
      {
        static struct : public SysReg {
          char const* Name() const { return "CPTR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Architectural Feature Trap Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0001,0b0001,0b010): // 2.19: CPTR_EL3, Architectural Feature Trap Register (EL3)
      {
        static struct : public SysReg {
          char const* Name() const { return "CPTR_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Architectural Feature Trap Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b010,0b0000,0b0000,0b000): // 2.20: CSSELR_EL1, Cache Size Selection Register
      {
        static struct : public SysReg {
          char const* Name() const { return "CSSELR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Cache Size Selection Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b0000,0b0000,0b001): // 2.21: CTR_EL0, Cache Type Register
      {
        static struct : public SysReg {
          char const* Name() const { return "CTR_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Cache Type Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0011,0b0000,0b000): // 2.22: DACR32_EL2, Domain Access Control Register
      {
        static struct : public SysReg {
          char const* Name() const { return "DACR32_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Domain Access Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0101,0b0010,0b000): // 2.24: ESR_EL1, Exception Syndrome Register (EL1)
      {
        static struct : public SysReg {
          char const* Name() const { return "ESR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Exception Syndrome Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0101,0b0010,0b000): // 2.25: ESR_EL2, Exception Syndrome Register (EL2)
      {
        static struct : public SysReg {
          char const* Name() const { return "ESR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Exception Syndrome Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0101,0b0010,0b000): // 2.26: ESR_EL3, Exception Syndrome Register (EL3)
      {
        static struct : public SysReg {
          char const* Name() const { return "ESR_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Exception Syndrome Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0110,0b0000,0b000): // 2.27: FAR_EL1, Fault Address Register (EL1)
      {
        static struct : public SysReg {
          char const* Name() const { return "FAR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Fault Address Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0110,0b0000,0b000): // 2.28: FAR_EL2, Fault Address Register (EL2)
      {
        static struct : public SysReg {
          char const* Name() const { return "FAR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Fault Address Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0110,0b0000,0b000): // 2.29: FAR_EL3, Fault Address Register (EL3)
      {
        static struct : public SysReg {
          char const* Name() const { return "FAR_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Fault Address Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0101,0b0011,0b000): // 2.30: FPEXC32_EL2, Floating-point Exception Control register
      {
        static struct : public SysReg {
          char const* Name() const { return "FPEXC32_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Floating-point Exception Control register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0001,0b111): // 2.31: HACR_EL2, Hypervisor Auxiliary Control Register
      {
        static struct : public SysReg {
          char const* Name() const { return "HACR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Hypervisor Auxiliary Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0001,0b000): // 2.32: HCR_EL2, Hypervisor Configuration Register
      {
        static struct : public SysReg {
          char const* Name() const { return "HCR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Hypervisor Configuration Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0110,0b0000,0b100): // 2.33: HPFAR_EL2, Hypervisor IPA Fault Address Register
      {
        static struct : public SysReg {
          char const* Name() const { return "HPFAR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Hypervisor IPA Fault Address Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0001,0b011): // 2.34: HSTR_EL2, Hypervisor System Trap Register
      {
        static struct : public SysReg {
          char const* Name() const { return "HSTR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Hypervisor System Trap Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0101,0b100): // 2.35: ID_AA64AFR0_EL1, AArch64 Auxiliary Feature Register 0
      {
        static struct : public SysReg {
          char const* Name() const { return "ID_AA64AFR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch64 Auxiliary Feature Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0101,0b101): // 2.36: ID_AA64AFR1_EL1, AArch64 Auxiliary Feature Register 1
      {
        static struct : public SysReg {
          char const* Name() const { return "ID_AA64AFR1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch64 Auxiliary Feature Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0101,0b000): // 2.37: ID_AA64DFR0_EL1, AArch64 Debug Feature Register 0
      {
        static struct : public SysReg {
          char const* Name() const { return "ID_AA64DFR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch64 Debug Feature Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0101,0b001): // 2.38: ID_AA64DFR1_EL1, AArch64 Debug Feature Register 1
      {
        static struct : public SysReg {
          char const* Name() const { return "ID_AA64DFR1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch64 Debug Feature Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0110,0b000): // 2.39: ID_AA64ISAR0_EL1, AArch64 Instruction Set Attribute Register 0
      {
        static struct : public SysReg {
          char const* Name() const { return "ID_AA64ISAR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch64 Instruction Set Attribute Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0110,0b001): // 2.40: ID_AA64ISAR1_EL1, AArch64 Instruction Set Attribute Register 1
      {
        static struct : public SysReg {
          char const* Name() const { return "ID_AA64ISAR1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch64 Instruction Set Attribute Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0110,0b010): // 2.40: ID_AA64ISAR1_EL1, AArch64 Instruction Set Attribute Register 2
      {
        static struct : public SysReg {
          char const* Name() const { return "ID_AA64ISAR2_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch64 Instruction Set Attribute Register 2"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0111,0b000): // 2.41: ID_AA64MMFR0_EL1, AArch64 Memory Model Feature Register 0
      {
        static struct : public SysReg {
          char const* Name() const { return "ID_AA64MMFR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch64 Memory Model Feature Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0111,0b001): // 2.42: ID_AA64MMFR1_EL1, AArch64 Memory Model Feature Register 1
      {
        static struct : public SysReg {
          char const* Name() const { return "ID_AA64MMFR1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch64 Memory Model Feature Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0100,0b000): // 2.43: ID_AA64PFR0_EL1, AArch64 Processor Feature Register 0
      {
        static struct : public SysReg {
          char const* Name() const { return "ID_AA64PFR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch64 Processor Feature Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0100,0b001): // 2.44: ID_AA64PFR1_EL1, AArch64 Processor Feature Register 1
      {
        static struct : public SysReg {
          char const* Name() const { return "ID_AA64PFR1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch64 Processor Feature Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0001,0b011): // 2.45: ID_AFR0_EL1, AArch32 Auxiliary Feature Register 0
      {
        static struct : public SysReg {
          char const* Name() const { return "ID_AFR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch32 Auxiliary Feature Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0001,0b010): // 2.46: ID_DFR0_EL1, AArch32 Debug Feature Register 0
      {
        static struct : public SysReg {
          char const* Name() const { return "ID_DFR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch32 Debug Feature Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0010,0b000): // 2.47: ID_ISAR0_EL1, AArch32 Instruction Set Attribute Register 0
      {
        static struct : public SysReg {
          char const* Name() const { return "ID_ISAR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch32 Instruction Set Attribute Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0010,0b001): // 2.48: ID_ISAR1_EL1, AArch32 Instruction Set Attribute Register 1
      {
        static struct : public SysReg {
          char const* Name() const { return "ID_ISAR1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch32 Instruction Set Attribute Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0010,0b010): // 2.49: ID_ISAR2_EL1, AArch32 Instruction Set Attribute Register 2
      {
        static struct : public SysReg {
          char const* Name() const { return "ID_ISAR2_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch32 Instruction Set Attribute Register 2"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0010,0b011): // 2.50: ID_ISAR3_EL1, AArch32 Instruction Set Attribute Register 3
      {
        static struct : public SysReg {
          char const* Name() const { return "ID_ISAR3_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch32 Instruction Set Attribute Register 3"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0010,0b100): // 2.51: ID_ISAR4_EL1, AArch32 Instruction Set Attribute Register 4
      {
        static struct : public SysReg {
          char const* Name() const { return "ID_ISAR4_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch32 Instruction Set Attribute Register 4"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0010,0b101): // 2.52: ID_ISAR5_EL1, AArch32 Instruction Set Attribute Register 5
      {
        static struct : public SysReg {
          char const* Name() const { return "ID_ISAR5_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch32 Instruction Set Attribute Register 5"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0001,0b100): // 2.53: ID_MMFR0_EL1, AArch32 Memory Model Feature Register 0
      {
        static struct : public SysReg {
          char const* Name() const { return "ID_MMFR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch32 Memory Model Feature Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0001,0b101): // 2.54: ID_MMFR1_EL1, AArch32 Memory Model Feature Register 1
      {
        static struct : public SysReg {
          char const* Name() const { return "ID_MMFR1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch32 Memory Model Feature Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0001,0b110): // 2.55: ID_MMFR2_EL1, AArch32 Memory Model Feature Register 2
      {
        static struct : public SysReg {
          char const* Name() const { return "ID_MMFR2_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch32 Memory Model Feature Register 2"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0001,0b111): // 2.56: ID_MMFR3_EL1, AArch32 Memory Model Feature Register 3
      {
        static struct : public SysReg {
          char const* Name() const { return "ID_MMFR3_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch32 Memory Model Feature Register 3"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0001,0b000): // 2.57: ID_PFR0_EL1, AArch32 Processor Feature Register 0
      {
        static struct : public SysReg {
          char const* Name() const { return "ID_PFR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch32 Processor Feature Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0001,0b001): // 2.58: ID_PFR1_EL1, AArch32 Processor Feature Register 1
      {
        static struct : public SysReg {
          char const* Name() const { return "ID_PFR1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch32 Processor Feature Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0101,0b0000,0b001): // 2.59: IFSR32_EL2, Instruction Fault Status Register (EL2)
      {
        static struct : public SysReg {
          char const* Name() const { return "IFSR32_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Instruction Fault Status Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b0001,0b000): // 2.60: ISR_EL1, Interrupt Status Register
      {
        static struct : public SysReg {
          char const* Name() const { return "ISR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Status Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1010,0b0010,0b000): // 2.61: MAIR_EL1, Memory Attribute Indirection Register (EL1)
      {
        static struct : public SysReg {
          char const* Name() const { return "MAIR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Memory Attribute Indirection Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1010,0b0010,0b000): // 2.62: MAIR_EL2, Memory Attribute Indirection Register (EL2)
      {
        static struct : public SysReg {
          char const* Name() const { return "MAIR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Memory Attribute Indirection Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1010,0b0010,0b000): // 2.63: MAIR_EL3, Memory Attribute Indirection Register (EL3)
      {
        static struct : public SysReg {
          char const* Name() const { return "MAIR_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Memory Attribute Indirection Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0000,0b000): // 2.64: MIDR_EL1, Main ID Register
      {
        static struct : public SysReg {
          char const* Name() const { return "MIDR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Main ID Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0000,0b101): // 2.65: MPIDR_EL1, Multiprocessor A0nity Register
      {
        static struct : public SysReg {
          char const* Name() const { return "MPIDR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Multiprocessor A0nity Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0011,0b000): // 2.66: MVFR0_EL1, Media and VFP Feature Register 0
      {
        static struct : public SysReg {
          char const* Name() const { return "MVFR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Media and VFP Feature Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0011,0b001): // 2.67: MVFR1_EL1, Media and VFP Feature Register 1
      {
        static struct : public SysReg {
          char const* Name() const { return "MVFR1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Media and VFP Feature Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0011,0b010): // 2.68: MVFR2_EL1, Media and VFP Feature Register 2
      {
        static struct : public SysReg {
          char const* Name() const { return "MVFR2_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Media and VFP Feature Register 2"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0111,0b0100,0b000): // 2.69: PAR_EL1, Physical Address Register
      {
        static struct : public SysReg {
          char const* Name() const { return "PAR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Physical Address Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0000,0b0000,0b110): // 2.70: REVIDR_EL1, Revision ID Register
      {
        static struct : public SysReg {
          char const* Name() const { return "REVIDR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Revision ID Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b0000,0b010): // 2.71: RMR_EL1, Reset Management Register (if EL2 and EL3 not implemented)
      {
        static struct : public SysReg {
          char const* Name() const { return "RMR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Reset Management Register (if EL2 and EL3 not implemented)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b0000,0b010): // 2.72: RMR_EL2, Reset Management Register (if EL3 not implemented)
      {
        static struct : public SysReg {
          char const* Name() const { return "RMR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Reset Management Register (if EL3 not implemented)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1100,0b0000,0b010): // 2.73: RMR_EL3, Reset Management Register (if EL3 implemented)
      {
        static struct : public SysReg {
          char const* Name() const { return "RMR_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Reset Management Register (if EL3 implemented)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b0000,0b001): // 2.74: RVBAR_EL1, Reset Vector Base Address Register (if EL2 and EL3 not implemented)
      {
        static struct : public SysReg {
          char const* Name() const { return "RVBAR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Reset Vector Base Address Register (if EL2 and EL3 not implemented)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b0000,0b001): // 2.75: RVBAR_EL2, Reset Vector Base Address Register (if EL3 not implemented)
      {
        static struct : public SysReg {
          char const* Name() const { return "RVBAR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Reset Vector Base Address Register (if EL3 not implemented)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1100,0b0000,0b001): // 2.76: RVBAR_EL3, Reset Vector Base Address Register (if EL3 implemented)
      {
        static struct : public SysReg {
          char const* Name() const { return "RVBAR_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Reset Vector Base Address Register (if EL3 implemented)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0001,0b0001,0b000): // 2.78: SCR_EL3, Secure Configuration Register
      {
        static struct : public SysReg {
          char const* Name() const { return "SCR_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Secure Configuration Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0001,0b0000,0b000): // 2.79: SCTLR_EL1, System Control Register (EL1)
      {
        static struct : public SysReg {
          char const* Name() const { return "SCTLR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "System Control Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0000,0b000): // 2.80: SCTLR_EL2, System Control Register (EL2)
      {
        static struct : public SysReg {
          char const* Name() const { return "SCTLR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "System Control Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0001,0b0000,0b000): // 2.81: SCTLR_EL3, System Control Register (EL3)
      {
        static struct : public SysReg {
          char const* Name() const { return "SCTLR_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "System Control Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0010,0b0000,0b010): // 2.82: TCR_EL1, Translation Control Register (EL1)
      {
        static struct : public SysReg {
          char const* Name() const { return "TCR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Translation Control Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0010,0b0000,0b010): // 2.83: TCR_EL2, Translation Control Register (EL2)
      {
        static struct : public SysReg {
          char const* Name() const { return "TCR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Translation Control Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0010,0b0000,0b010): // 2.84: TCR_EL3, Translation Control Register (EL3)
      {
        static struct : public SysReg {
          char const* Name() const { return "TCR_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Translation Control Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b010,0b0000,0b0000,0b000): // 2.85: TEECR32_EL1, T32EE Configuration Register
      {
        static struct : public SysReg {
          char const* Name() const { return "TEECR32_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "T32EE Configuration Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b010,0b0001,0b0000,0b000): // 2.86: TEEHBR32_EL1, T32EE Handler Base Register
      {
        static struct : public SysReg {
          char const* Name() const { return "TEEHBR32_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "T32EE Handler Base Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1101,0b0000,0b010): // 2.87: TPIDR_EL0, Thread Pointer / ID Register (EL0)
      {
        static struct : public SysReg {
          char const* Name() const { return "TPIDR_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Thread Pointer / ID Register (EL0)"; }
          void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, CPU_IMPL& cpu, U64 value) const override { cpu.TPIDRURW = value; }
          U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2,  CPU_IMPL& cpu) const override { return cpu.TPIDRURW; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1101,0b0000,0b100): // 2.88: TPIDR_EL1, Thread Pointer / ID Register (EL1)
      {
        static struct : public SysReg {
          char const* Name() const { return "TPIDR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Thread Pointer / ID Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1101,0b0000,0b010): // 2.89: TPIDR_EL2, Thread Pointer / ID Register (EL2)
      {
        static struct : public SysReg {
          char const* Name() const { return "TPIDR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Thread Pointer / ID Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1101,0b0000,0b010): // 2.90: TPIDR_EL3, Thread Pointer / ID Register (EL3)
      {
        static struct : public SysReg {
          char const* Name() const { return "TPIDR_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Thread Pointer / ID Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1101,0b0000,0b011): // 2.91: TPIDRRO_EL0, Thread Pointer / ID Register, Read-Only (EL0)
      {
        static struct : public SysReg {
          char const* Name() const { return "TPIDRRO_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Thread Pointer / ID Register, Read-Only (EL0)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0010,0b0000,0b000): // 2.92: TTBR0_EL1, Translation Table Base Register 0 (EL1)
      {
        static struct : public SysReg {
          char const* Name() const { return "TTBR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Translation Table Base Register 0 (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0010,0b0000,0b000): // 2.93: TTBR0_EL2, Translation Table Base Register 0 (EL2)
      {
        static struct : public SysReg {
          char const* Name() const { return "TTBR0_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Translation Table Base Register 0 (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0010,0b0000,0b000): // 2.94: TTBR0_EL3, Translation Table Base Register 0 (EL3)
      {
        static struct : public SysReg {
          char const* Name() const { return "TTBR0_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Translation Table Base Register 0 (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0010,0b0000,0b001): // 2.95: TTBR1_EL1, Translation Table Base Register 1
      {
        static struct : public SysReg {
          char const* Name() const { return "TTBR1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Translation Table Base Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b0000,0b000): // 2.96: VBAR_EL1, Vector Base Address Register (EL1)
      {
        static struct : public SysReg {
          char const* Name() const { return "VBAR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Vector Base Address Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b0000,0b000): // 2.97: VBAR_EL2, Vector Base Address Register (EL2)
      {
        static struct : public SysReg {
          char const* Name() const { return "VBAR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Vector Base Address Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1100,0b0000,0b000): // 2.98: VBAR_EL3, Vector Base Address Register (EL3)
      {
        static struct : public SysReg {
          char const* Name() const { return "VBAR_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Vector Base Address Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0000,0b0000,0b101): // 2.99: VMPIDR_EL2, Virtualization Multiprocessor ID Register
      {
        static struct : public SysReg {
          char const* Name() const { return "VMPIDR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Virtualization Multiprocessor ID Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0000,0b0000,0b000): // 2.100: VPIDR_EL2, Virtualization Processor ID Register
      {
        static struct : public SysReg {
          char const* Name() const { return "VPIDR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Virtualization Processor ID Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0010,0b0001,0b010): // 2.101: VTCR_EL2, Virtualization Translation Control Register
      {
        static struct : public SysReg {
          char const* Name() const { return "VTCR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Virtualization Translation Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0010,0b0001,0b000): // 2.102: VTTBR_EL2, Virtualization Translation Table Base Register
      {
        static struct : public SysReg {
          char const* Name() const { return "VTTBR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Virtualization Translation Table Base Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0111,0b1110,0b110): // 3.1: DBGAUTHSTATUS_EL1, Debug Authentication Status register
      {
        static struct : public SysReg {
          char const* Name() const { return "DBGAUTHSTATUS_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Debug Authentication Status register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0000,0b101): // 3.2: DBGBCR<n>_EL1, Debug Breakpoint Control Registers, n = 0 - 15
      {
        static struct : public SysReg {
          char const* Name() const { return "DBGBCR<n>_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Debug Breakpoint Control Registers, n = 0 - 15"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0000,0b100): // 3.3: DBGBVR<n>_EL1, Debug Breakpoint Value Registers, n = 0 - 15
      {
        static struct : public SysReg {
          char const* Name() const { return "DBGBVR<n>_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Debug Breakpoint Value Registers, n = 0 - 15"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0111,0b1001,0b110): // 3.4: DBGCLAIMCLR_EL1, Debug Claim Tag Clear register
      {
        static struct : public SysReg {
          char const* Name() const { return "DBGCLAIMCLR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Debug Claim Tag Clear register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0111,0b1000,0b110): // 3.5: DBGCLAIMSET_EL1, Debug Claim Tag Set register
      {
        static struct : public SysReg {
          char const* Name() const { return "DBGCLAIMSET_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Debug Claim Tag Set register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b011,0b0000,0b0100,0b000): // 3.6: DBGDTR_EL0, Debug Data Transfer Register, half-duplex
      {
        static struct : public SysReg {
          char const* Name() const { return "DBGDTR_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Debug Data Transfer Register, half-duplex"; }
        } x; return &x;
      } break;

    // case SYSENCODE(0b10,0b011,0b0000,0b0101,0b000): 3.7: DBGDTRRX_EL0, Debug Data Transfer Register, Receive
    //   {
    //     static struct : public SysReg {
    //       char const* Name() const { return "DBGDTRRX_EL0"; }
    //       void Describe(Encoding, std::ostream& sink) const override { sink << "Debug Data Transfer Register, Receive"; }
    //     } x; return &x;
    //   } break;

    // case SYSENCODE(0b10,0b011,0b0000,0b0101,0b000): 3.8: DBGDTRTX_EL0, Debug Data Transfer Register, Transmit
    //   {
    //     static struct : public SysReg {
    //       char const* Name() const { return "DBGDTRTX_EL0"; }
    //       void Describe(Encoding, std::ostream& sink) const override { sink << "Debug Data Transfer Register, Transmit"; }
    //     } x; return &x;
    //   } break;

    case SYSENCODE(0b10,0b011,0b0000,0b0101,0b000): // 3.8: DBGDTR_EL0, Debug Data Transfer Register
      {
        static struct : public SysReg {
          char const* Name() const { return "DBGDTR_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Debug Data Transfer Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0001,0b0100,0b100): // 3.9: DBGPRCR_EL1, Debug Power Control Register
      {
        static struct : public SysReg {
          char const* Name() const { return "DBGPRCR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Debug Power Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b100,0b0000,0b0111,0b000): // 3.10: DBGVCR32_EL2, Debug Vector Catch Register
      {
        static struct : public SysReg {
          char const* Name() const { return "DBGVCR32_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Debug Vector Catch Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0000,0b111): // 3.11: DBGWCR<n>_EL1, Debug Watchpoint Control Registers, n = 0 - 15
      {
        static struct : public SysReg {
          char const* Name() const { return "DBGWCR<n>_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Debug Watchpoint Control Registers, n = 0 - 15"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0000,0b110): // 3.12: DBGWVR<n>_EL1, Debug Watchpoint Value Registers, n = 0 - 15
      {
        static struct : public SysReg {
          char const* Name() const { return "DBGWVR<n>_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Debug Watchpoint Value Registers, n = 0 - 15"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b0100,0b0101,0b001): // 3.13: DLR_EL0, Debug Link Register
      {
        static struct : public SysReg {
          char const* Name() const { return "DLR_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Debug Link Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b0100,0b0101,0b000): // 3.14: DSPSR_EL0, Debug Saved Program Status Register
      {
        static struct : public SysReg {
          char const* Name() const { return "DSPSR_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Debug Saved Program Status Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0010,0b000): // 3.15: MDCCINT_EL1, Monitor DCC Interrupt Enable Register
      {
        static struct : public SysReg {
          char const* Name() const { return "MDCCINT_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Monitor DCC Interrupt Enable Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b011,0b0000,0b0001,0b000): // 3.16: MDCCSR_EL0, Monitor DCC Status Register
      {
        static struct : public SysReg {
          char const* Name() const { return "MDCCSR_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Monitor DCC Status Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b0001,0b0001,0b001): // 3.17: MDCR_EL2, Monitor Debug Configuration Register (EL2)
      {
        static struct : public SysReg {
          char const* Name() const { return "MDCR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Monitor Debug Configuration Register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0001,0b0011,0b001): // 3.18: MDCR_EL3, Monitor Debug Configuration Register (EL3)
      {
        static struct : public SysReg {
          char const* Name() const { return "MDCR_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Monitor Debug Configuration Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0001,0b0000,0b000): // 3.19: MDRAR_EL1, Monitor Debug ROM Address Register
      {
        static struct : public SysReg {
          char const* Name() const { return "MDRAR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Monitor Debug ROM Address Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0010,0b010): // 3.20: MDSCR_EL1, Monitor Debug System Control Register
      {
        static struct : public SysReg {
          char const* Name() const { return "MDSCR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Monitor Debug System Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0001,0b0011,0b100): // 3.21: OSDLR_EL1, OS Double Lock Register
      {
        static struct : public SysReg {
          char const* Name() const { return "OSDLR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "OS Double Lock Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0000,0b010): // 3.22: OSDTRRX_EL1, OS Lock Data Transfer Register, Receive
      {
        static struct : public SysReg {
          char const* Name() const { return "OSDTRRX_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "OS Lock Data Transfer Register, Receive"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0011,0b010): // 3.23: OSDTRTX_EL1, OS Lock Data Transfer Register, Transmit
      {
        static struct : public SysReg {
          char const* Name() const { return "OSDTRTX_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "OS Lock Data Transfer Register, Transmit"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0000,0b0110,0b010): // 3.24: OSECCR_EL1, OS Lock Exception Catch Control Register
      {
        static struct : public SysReg {
          char const* Name() const { return "OSECCR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "OS Lock Exception Catch Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0001,0b0000,0b100): // 3.25: OSLAR_EL1, OS Lock Access Register
      {
        static struct : public SysReg {
          char const* Name() const { return "OSLAR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "OS Lock Access Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b10,0b000,0b0001,0b0001,0b100): // 3.26: OSLSR_EL1, OS Lock Status Register
      {
        static struct : public SysReg {
          char const* Name() const { return "OSLSR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "OS Lock Status Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b0001,0b0001,0b001): // 3.27: SDER32_EL3, AArch32 Secure Debug Enable Register
      {
        static struct : public SysReg {
          char const* Name() const { return "SDER32_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "AArch32 Secure Debug Enable Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b1111,0b111): // 4.1: PMCCFILTR_EL0, Performance Monitors Cycle Count Filter Register
      {
        static struct : public SysReg {
          char const* Name() const { return "PMCCFILTR_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors Cycle Count Filter Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1101,0b000): // 4.2: PMCCNTR_EL0, Performance Monitors Cycle Count Register
      {
        static struct : public SysReg {
          char const* Name() const { return "PMCCNTR_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors Cycle Count Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1100,0b110): // 4.3: PMCEID0_EL0, Performance Monitors Common Event Identification register 0
      {
        static struct : public SysReg {
          char const* Name() const { return "PMCEID0_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors Common Event Identification register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1100,0b111): // 4.4: PMCEID1_EL0, Performance Monitors Common Event Identification register 1
      {
        static struct : public SysReg {
          char const* Name() const { return "PMCEID1_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors Common Event Identification register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1100,0b010): // 4.5: PMCNTENCLR_EL0, Performance Monitors Count Enable Clear register
      {
        static struct : public SysReg {
          char const* Name() const { return "PMCNTENCLR_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors Count Enable Clear register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1100,0b001): // 4.6: PMCNTENSET_EL0, Performance Monitors Count Enable Set register
      {
        static struct : public SysReg {
          char const* Name() const { return "PMCNTENSET_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors Count Enable Set register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1100,0b000): // 4.7: PMCR_EL0, Performance Monitors Control Register
      {
        static struct : public SysReg {
          char const* Name() const { return "PMCR_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b1000,0b000): // 4.8: PMEVCNTR<n>_EL0, Performance Monitors Event Count Registers, n = 0 - 30
      {
        static struct : public SysReg {
          char const* Name() const { return "PMEVCNTR<n>_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors Event Count Registers, n = 0 - 30"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b1100,0b000): // 4.9: PMEVTYPER<n>_EL0, Performance Monitors Event Type Registers, n = 0 - 30
      {
        static struct : public SysReg {
          char const* Name() const { return "PMEVTYPER<n>_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors Event Type Registers, n = 0 - 30"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1001,0b1110,0b010): // 4.10: PMINTENCLR_EL1, Performance Monitors Interrupt Enable Clear register
      {
        static struct : public SysReg {
          char const* Name() const { return "PMINTENCLR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors Interrupt Enable Clear register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1001,0b1110,0b001): // 4.11: PMINTENSET_EL1, Performance Monitors Interrupt Enable Set register
      {
        static struct : public SysReg {
          char const* Name() const { return "PMINTENSET_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors Interrupt Enable Set register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1100,0b011): // 4.12: PMOVSCLR_EL0, Performance Monitors Overflow Flag Status Clear Register
      {
        static struct : public SysReg {
          char const* Name() const { return "PMOVSCLR_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors Overflow Flag Status Clear Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1110,0b011): // 4.13: PMOVSSET_EL0, Performance Monitors Overflow Flag Status Set register
      {
        static struct : public SysReg {
          char const* Name() const { return "PMOVSSET_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors Overflow Flag Status Set register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1100,0b101): // 4.14: PMSELR_EL0, Performance Monitors Event Counter Selection Register
      {
        static struct : public SysReg {
          char const* Name() const { return "PMSELR_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors Event Counter Selection Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1100,0b100): // 4.15: PMSWINC_EL0, Performance Monitors Software Increment register
      {
        static struct : public SysReg {
          char const* Name() const { return "PMSWINC_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors Software Increment register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1110,0b000): // 4.16: PMUSERENR_EL0, Performance Monitors User Enable Register
      {
        static struct : public SysReg {
          char const* Name() const { return "PMUSERENR_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors User Enable Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1101,0b010): // 4.17: PMXEVCNTR_EL0, Performance Monitors Selected Event Count Register
      {
        static struct : public SysReg {
          char const* Name() const { return "PMXEVCNTR_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors Selected Event Count Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1001,0b1101,0b001): // 4.18: PMXEVTYPER_EL0, Performance Monitors Selected Event Type Register
      {
        static struct : public SysReg {
          char const* Name() const { return "PMXEVTYPER_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Performance Monitors Selected Event Type Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0000,0b000): // 5.1: CNTFRQ_EL0, Counter-timer Frequency register
      {
        static struct : public SysReg {
          char const* Name() const { return "CNTFRQ_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Frequency register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1110,0b0001,0b000): // 5.2: CNTHCTL_EL2, Counter-timer Hypervisor Control register
      {
        static struct : public SysReg {
          char const* Name() const { return "CNTHCTL_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Hypervisor Control register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1110,0b0010,0b001): // 5.3: CNTHP_CTL_EL2, Counter-timer Hypervisor Physical Timer Control register
      {
        static struct : public SysReg {
          char const* Name() const { return "CNTHP_CTL_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Hypervisor Physical Timer Control register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1110,0b0010,0b010): // 5.4: CNTHP_CVAL_EL2, Counter-timer Hypervisor Physical Timer CompareValue register
      {
        static struct : public SysReg {
          char const* Name() const { return "CNTHP_CVAL_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Hypervisor Physical Timer CompareValue register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1110,0b0010,0b000): // 5.5: CNTHP_TVAL_EL2, Counter-timer Hypervisor Physical Timer TimerValue register
      {
        static struct : public SysReg {
          char const* Name() const { return "CNTHP_TVAL_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Hypervisor Physical Timer TimerValue register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1110,0b0001,0b000): // 5.6: CNTKCTL_EL1, Counter-timer Kernel Control register
      {
        static struct : public SysReg {
          char const* Name() const { return "CNTKCTL_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Kernel Control register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0010,0b001): // 5.7: CNTP_CTL_EL0, Counter-timer Physical Timer Control register
      {
        static struct : public SysReg {
          char const* Name() const { return "CNTP_CTL_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Physical Timer Control register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0010,0b010): // 5.8: CNTP_CVAL_EL0, Counter-timer Physical Timer CompareValue register
      {
        static struct : public SysReg {
          char const* Name() const { return "CNTP_CVAL_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Physical Timer CompareValue register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0010,0b000): // 5.9: CNTP_TVAL_EL0, Counter-timer Physical Timer TimerValue register
      {
        static struct : public SysReg {
          char const* Name() const { return "CNTP_TVAL_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Physical Timer TimerValue register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0000,0b001): // 5.10: CNTPCT_EL0, Counter-timer Physical Count register
      {
        static struct : public SysReg {
          char const* Name() const { return "CNTPCT_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Physical Count register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b111,0b1110,0b0010,0b001): // 5.11: CNTPS_CTL_EL1, Counter-timer Physical Secure Timer Control register
      {
        static struct : public SysReg {
          char const* Name() const { return "CNTPS_CTL_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Physical Secure Timer Control register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b111,0b1110,0b0010,0b010): // 5.12: CNTPS_CVAL_EL1, Counter-timer Physical Secure Timer CompareValue register
      {
        static struct : public SysReg {
          char const* Name() const { return "CNTPS_CVAL_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Physical Secure Timer CompareValue register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b111,0b1110,0b0010,0b000): // 5.13: CNTPS_TVAL_EL1, Counter-timer Physical Secure Timer TimerValue register
      {
        static struct : public SysReg {
          char const* Name() const { return "CNTPS_TVAL_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Physical Secure Timer TimerValue register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0011,0b001): // 5.14: CNTV_CTL_EL0, Counter-timer Virtual Timer Control register
      {
        static struct : public SysReg {
          char const* Name() const { return "CNTV_CTL_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Virtual Timer Control register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0011,0b010): // 5.15: CNTV_CVAL_EL0, Counter-timer Virtual Timer CompareValue register
      {
        static struct : public SysReg {
          char const* Name() const { return "CNTV_CVAL_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Virtual Timer CompareValue register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0011,0b000): // 5.16: CNTV_TVAL_EL0, Counter-timer Virtual Timer TimerValue register
      {
        static struct : public SysReg {
          char const* Name() const { return "CNTV_TVAL_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Virtual Timer TimerValue register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b011,0b1110,0b0000,0b010): // 5.17: CNTVCT_EL0, Counter-timer Virtual Count register
      {
        static struct : public SysReg {
          char const* Name() const { return "CNTVCT_EL0"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Virtual Count register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1110,0b0000,0b011): // 5.18: CNTVOFF_EL2, Counter-timer Virtual Offset register
      {
        static struct : public SysReg {
          char const* Name() const { return "CNTVOFF_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Counter-timer Virtual Offset register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1000,0b100): // 6.1: ICC_AP0R0_EL1, Interrupt Controller Active Priorities Register (0,0)
      {
        static struct : public SysReg {
          char const* Name() const { return "ICC_AP0R0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Active Priorities Register (0,0)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1000,0b101): // 6.2: ICC_AP0R1_EL1, Interrupt Controller Active Priorities Register (0,1)
      {
        static struct : public SysReg {
          char const* Name() const { return "ICC_AP0R1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Active Priorities Register (0,1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1000,0b110): // 6.3: ICC_AP0R2_EL1, Interrupt Controller Active Priorities Register (0,2)
      {
        static struct : public SysReg {
          char const* Name() const { return "ICC_AP0R2_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Active Priorities Register (0,2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1000,0b111): // 6.4: ICC_AP0R3_EL1, Interrupt Controller Active Priorities Register (0,3)
      {
        static struct : public SysReg {
          char const* Name() const { return "ICC_AP0R3_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Active Priorities Register (0,3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1001,0b000): // 6.5: ICC_AP1R0_EL1, Interrupt Controller Active Priorities Register (1,0)
      {
        static struct : public SysReg {
          char const* Name() const { return "ICC_AP1R0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Active Priorities Register (1,0)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1001,0b001): // 6.6: ICC_AP1R1_EL1, Interrupt Controller Active Priorities Register (1,1)
      {
        static struct : public SysReg {
          char const* Name() const { return "ICC_AP1R1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Active Priorities Register (1,1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1001,0b010): // 6.7: ICC_AP1R2_EL1, Interrupt Controller Active Priorities Register (1,2)
      {
        static struct : public SysReg {
          char const* Name() const { return "ICC_AP1R2_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Active Priorities Register (1,2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1001,0b011): // 6.8: ICC_AP1R3_EL1, Interrupt Controller Active Priorities Register (1,3)
      {
        static struct : public SysReg {
          char const* Name() const { return "ICC_AP1R3_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Active Priorities Register (1,3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1011,0b110): // 6.9: ICC_ASGI1R_EL1, Interrupt Controller Alias Software Generated Interrupt group 1 Register
      {
        static struct : public SysReg {
          char const* Name() const { return "ICC_ASGI1R_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Alias Software Generated Interrupt group 1 Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1000,0b011): // 6.10: ICC_BPR0_EL1, Interrupt Controller Binary Point Register 0
      {
        static struct : public SysReg {
          char const* Name() const { return "ICC_BPR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Binary Point Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1100,0b011): // 6.11: ICC_BPR1_EL1, Interrupt Controller Binary Point Register 1
      {
        static struct : public SysReg {
          char const* Name() const { return "ICC_BPR1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Binary Point Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1100,0b100): // 6.12: ICC_CTLR_EL1, Interrupt Controller Control Register (EL1)
      {
        static struct : public SysReg {
          char const* Name() const { return "ICC_CTLR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Control Register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1100,0b1100,0b100): // 6.13: ICC_CTLR_EL3, Interrupt Controller Control Register (EL3)
      {
        static struct : public SysReg {
          char const* Name() const { return "ICC_CTLR_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Control Register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1011,0b001): // 6.14: ICC_DIR_EL1, Interrupt Controller Deactivate Interrupt Register
      {
        static struct : public SysReg {
          char const* Name() const { return "ICC_DIR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Deactivate Interrupt Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1000,0b001): // 6.15: ICC_EOIR0_EL1, Interrupt Controller End Of Interrupt Register 0
      {
        static struct : public SysReg {
          char const* Name() const { return "ICC_EOIR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller End Of Interrupt Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1100,0b001): // 6.16: ICC_EOIR1_EL1, Interrupt Controller End Of Interrupt Register 1
      {
        static struct : public SysReg {
          char const* Name() const { return "ICC_EOIR1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller End Of Interrupt Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1000,0b010): // 6.17: ICC_HPPIR0_EL1, Interrupt Controller Highest Priority Pending Interrupt Register 0
      {
        static struct : public SysReg {
          char const* Name() const { return "ICC_HPPIR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Highest Priority Pending Interrupt Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1100,0b010): // 6.18: ICC_HPPIR1_EL1, Interrupt Controller Highest Priority Pending Interrupt Register 1
      {
        static struct : public SysReg {
          char const* Name() const { return "ICC_HPPIR1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Highest Priority Pending Interrupt Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1000,0b000): // 6.19: ICC_IAR0_EL1, Interrupt Controller Interrupt Acknowledge Register 0
      {
        static struct : public SysReg {
          char const* Name() const { return "ICC_IAR0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Interrupt Acknowledge Register 0"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1100,0b000): // 6.20: ICC_IAR1_EL1, Interrupt Controller Interrupt Acknowledge Register 1
      {
        static struct : public SysReg {
          char const* Name() const { return "ICC_IAR1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Interrupt Acknowledge Register 1"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1100,0b110): // 6.21: ICC_IGRPEN0_EL1, Interrupt Controller Interrupt Group 0 Enable register
      {
        static struct : public SysReg {
          char const* Name() const { return "ICC_IGRPEN0_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Interrupt Group 0 Enable register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1100,0b111): // 6.22: ICC_IGRPEN1_EL1, Interrupt Controller Interrupt Group 1 Enable register
      {
        static struct : public SysReg {
          char const* Name() const { return "ICC_IGRPEN1_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Interrupt Group 1 Enable register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1100,0b1100,0b111): // 6.23: ICC_IGRPEN1_EL3, Interrupt Controller Interrupt Group 1 Enable register (EL3)
      {
        static struct : public SysReg {
          char const* Name() const { return "ICC_IGRPEN1_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Interrupt Group 1 Enable register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b0100,0b0110,0b000): // 6.24: ICC_PMR_EL1, Interrupt Controller Interrupt Priority Mask Register
      {
        static struct : public SysReg {
          char const* Name() const { return "ICC_PMR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Interrupt Priority Mask Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1011,0b011): // 6.25: ICC_RPR_EL1, Interrupt Controller Running Priority Register
      {
        static struct : public SysReg {
          char const* Name() const { return "ICC_RPR_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Running Priority Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1101,0b000): // 6.26: ICC_SEIEN_EL1, Interrupt Controller System Error Interrupt Enable register
      {
        static struct : public SysReg {
          char const* Name() const { return "ICC_SEIEN_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller System Error Interrupt Enable register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1011,0b111): // 6.27: ICC_SGI0R_EL1, Interrupt Controller Software Generated Interrupt group 0 Register
      {
        static struct : public SysReg {
          char const* Name() const { return "ICC_SGI0R_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Software Generated Interrupt group 0 Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1011,0b101): // 6.28: ICC_SGI1R_EL1, Interrupt Controller Software Generated Interrupt group 1 Register
      {
        static struct : public SysReg {
          char const* Name() const { return "ICC_SGI1R_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Software Generated Interrupt group 1 Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b000,0b1100,0b1100,0b101): // 6.29: ICC_SRE_EL1, Interrupt Controller System Register Enable register (EL1)
      {
        static struct : public SysReg {
          char const* Name() const { return "ICC_SRE_EL1"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller System Register Enable register (EL1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1001,0b101): // 6.30: ICC_SRE_EL2, Interrupt Controller System Register Enable register (EL2)
      {
        static struct : public SysReg {
          char const* Name() const { return "ICC_SRE_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller System Register Enable register (EL2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b110,0b1100,0b1100,0b101): // 6.31: ICC_SRE_EL3, Interrupt Controller System Register Enable register (EL3)
      {
        static struct : public SysReg {
          char const* Name() const { return "ICC_SRE_EL3"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller System Register Enable register (EL3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1000,0b000): // 6.32: ICH_AP0R0_EL2, Interrupt Controller Hyp Active Priorities Register (0,0)
      {
        static struct : public SysReg {
          char const* Name() const { return "ICH_AP0R0_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Hyp Active Priorities Register (0,0)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1000,0b001): // 6.33: ICH_AP0R1_EL2, Interrupt Controller Hyp Active Priorities Register (0,1)
      {
        static struct : public SysReg {
          char const* Name() const { return "ICH_AP0R1_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Hyp Active Priorities Register (0,1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1000,0b010): // 6.34: ICH_AP0R2_EL2, Interrupt Controller Hyp Active Priorities Register (0,2)
      {
        static struct : public SysReg {
          char const* Name() const { return "ICH_AP0R2_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Hyp Active Priorities Register (0,2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1000,0b011): // 6.35: ICH_AP0R3_EL2, Interrupt Controller Hyp Active Priorities Register (0,3)
      {
        static struct : public SysReg {
          char const* Name() const { return "ICH_AP0R3_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Hyp Active Priorities Register (0,3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1001,0b000): // 6.36: ICH_AP1R0_EL2, Interrupt Controller Hyp Active Priorities Register (1,0)
      {
        static struct : public SysReg {
          char const* Name() const { return "ICH_AP1R0_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Hyp Active Priorities Register (1,0)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1001,0b001): // 6.37: ICH_AP1R1_EL2, Interrupt Controller Hyp Active Priorities Register (1,1)
      {
        static struct : public SysReg {
          char const* Name() const { return "ICH_AP1R1_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Hyp Active Priorities Register (1,1)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1001,0b010): // 6.38: ICH_AP1R2_EL2, Interrupt Controller Hyp Active Priorities Register (1,2)
      {
        static struct : public SysReg {
          char const* Name() const { return "ICH_AP1R2_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Hyp Active Priorities Register (1,2)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1001,0b011): // 6.39: ICH_AP1R3_EL2, Interrupt Controller Hyp Active Priorities Register (1,3)
      {
        static struct : public SysReg {
          char const* Name() const { return "ICH_AP1R3_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Hyp Active Priorities Register (1,3)"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1011,0b011): // 6.40: ICH_EISR_EL2, Interrupt Controller End of Interrupt Status Register
      {
        static struct : public SysReg {
          char const* Name() const { return "ICH_EISR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller End of Interrupt Status Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1011,0b101): // 6.41: ICH_ELSR_EL2, Interrupt Controller Empty List Register Status Register
      {
        static struct : public SysReg {
          char const* Name() const { return "ICH_ELSR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Empty List Register Status Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1011,0b000): // 6.42: ICH_HCR_EL2, Interrupt Controller Hyp Control Register
      {
        static struct : public SysReg {
          char const* Name() const { return "ICH_HCR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Hyp Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1100,0b000): // 6.43: ICH_LR<n>_EL2, Interrupt Controller List Registers, n = 0 - 15
      {
        static struct : public SysReg {
          char const* Name() const { return "ICH_LR<n>_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller List Registers, n = 0 - 15"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1011,0b010): // 6.44: ICH_MISR_EL2, Interrupt Controller Maintenance Interrupt State Register
      {
        static struct : public SysReg {
          char const* Name() const { return "ICH_MISR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Maintenance Interrupt State Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1011,0b111): // 6.45: ICH_VMCR_EL2, Interrupt Controller Virtual Machine Control Register
      {
        static struct : public SysReg {
          char const* Name() const { return "ICH_VMCR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Virtual Machine Control Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1001,0b100): // 6.46: ICH_VSEIR_EL2, Interrupt Controller Virtual System Error Interrupt Register
      {
        static struct : public SysReg {
          char const* Name() const { return "ICH_VSEIR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller Virtual System Error Interrupt Register"; }
        } x; return &x;
      } break;

    case SYSENCODE(0b11,0b100,0b1100,0b1011,0b001): // 6.47: ICH_VTR_EL2, Interrupt Controller VGIC Type Register
      {
        static struct : public SysReg {
          char const* Name() const { return "ICH_VTR_EL2"; }
          void Describe(Encoding, std::ostream& sink) const override { sink << "Interrupt Controller VGIC Type Register"; }
        } x; return &x;
      } break;

    }

  /*  System instructions with immediate, CRm used as immediate field,
   *  and thus ignored during decode.  (TODO: rt should be 0b11111).
   */
  switch (SYSENCODE(op0, op1, crn, 0, op2))
    {
    case SYSENCODE(0b00,0b011,0b0011,0b0000,0b100):
    case SYSENCODE(0b00,0b011,0b0011,0b0000,0b101):
    case SYSENCODE(0b00,0b011,0b0011,0b0000,0b110):
      {
        static struct : public WOOpSysReg
        {
          virtual void Disasm(Encoding e, std::ostream& sink) const override
          {
            char const* ops[] = {"dsb", "dmb", "isb"};
            sink << ops[e.op2&3] << '\t' << unisim::component::cxx::processor::arm::isa::arm64::DisasmBarrierOption(e.crm);
          }
        } x; return &x;
      } break;

    case SYSENCODE(0b00,0b000,0b0100,0b0000,0b101):
      {
        static struct : public PStateSysReg {
          virtual void Name(Encoding, std::ostream& sink) const override { sink << "PSTATEField_SP"; }
          virtual void Write(uint8_t, uint8_t, uint8_t, uint8_t crm, uint8_t, CPU_IMPL& cpu, U64) const override { throw 0; }
        } x; return &x;
      } break;

    case SYSENCODE(0b00,0b011,0b0100,0b0000,0b110):
      {
        static struct : public PStateSysReg {
          virtual void Name(Encoding, std::ostream& sink) const override { sink << "PSTATEField_DAIFSet"; }
          virtual void Write(uint8_t, uint8_t, uint8_t, uint8_t crm, uint8_t, CPU_IMPL& cpu, U64) const override { throw 0; }
        } x; return &x;
      } break;

    case SYSENCODE(0b00,0b011,0b0100,0b0000,0b111):
      {
        static struct : public PStateSysReg {
          virtual void Name(Encoding, std::ostream& sink) const override { sink << "PSTATEField_DAIFClr"; }
          virtual void Write(uint8_t, uint8_t, uint8_t, uint8_t crm, uint8_t, CPU_IMPL& cpu, U64) const override { throw 0; }
        } x; return &x;
      } break;
    }

  static struct UnknownSysReg : public SysReg
  {
    void fields(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, std::ostream& sink) const
    {
      sink << "op0=" << unsigned(op0) << ", op1=" << unsigned(op1) << ", crn=" << unsigned(crn) << ", crm=" << unsigned(crm) << ", op2=" << unsigned(op2);
    }
    virtual void DisasmRead(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, std::ostream& sink) const override
    {
      sink << "sysl|mrs\t" << unisim::component::cxx::processor::arm::isa::arm64::DisasmGZXR(rt) << ", <unknown>\t; ";
      fields(op0, op1, crn, crm, op2, sink);
    }
    virtual void DisasmWrite(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, uint8_t rt, std::ostream& sink) const override
    {
      sink << "sys|msr\t<unknown>, " << unisim::component::cxx::processor::arm::isa::arm64::DisasmGZXR(rt) << "\t; ";
      fields(op0, op1, crn, crm, op2, sink);
    }
    virtual void Write(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2, CPU_IMPL& cpu, U64 value) const override
    {
      fields(op0, op1, crn, crm, op2, std::cerr << std::hex << cpu.current_insn_addr << ": unimplemented, cannot write " << std::dec);
      std::cerr << std::endl;
      cpu.UndefinedInstruction();
    }
    virtual U64 Read(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2,  CPU_IMPL& cpu) const override
    {
      fields(op0, op1, crn, crm, op2, std::cerr << std::hex << cpu.current_insn_addr << ": unimplemented, cannot read " << std::dec);
      std::cerr << std::endl;
      cpu.UndefinedInstruction();
      return U64();
    }
  } err;

  err.fields(op0, op1, crn, crm, op2, std::cerr << "Unknown system register: ");
  std::cerr << std::endl;
  return &err;
}

/** Resets the internal values of corresponding system registers
 */
template <class CPU_IMPL>
void
CPU<CPU_IMPL>::ResetSystemRegisters()
{
}


} // end of namespace vmsav8
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_VMSAV8_CPU_TCC__
