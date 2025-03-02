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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#include <unisim/component/cxx/processor/arm/vmsav7/cpu.hh>
#include <unisim/component/cxx/processor/arm/vmsav7/cp15.hh>
#include <unisim/component/cxx/processor/arm/cpu.tcc>
#include <unisim/component/cxx/processor/arm/isa/decode.hh>
#include <unisim/component/cxx/processor/arm/memattrs.hh>
using unisim::util::arithmetic::RotateRight;
#include <unisim/component/cxx/processor/arm/isa/execute.hh>
#include <unisim/component/cxx/processor/arm/isa/arm32/arm32.tcc>
#include <unisim/component/cxx/processor/arm/isa/thumb2/thumb.tcc>
#include <unisim/component/cxx/processor/arm/cfg/aarch32/aarch32.hh>
#include <unisim/component/cxx/processor/opcache/opcache.tcc>
#include <unisim/util/backtrace/backtrace.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/util/truth_table/truth_table.hh>
#include <unisim/util/os/linux_os/linux.hh>
#include <unisim/util/os/linux_os/arm.hh>
#include <unisim/util/simfloat/floating.tcc>

#include <memory>
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


namespace vmsav7 {

// using unisim::kernel::Object;
// using unisim::kernel::Client;
// using unisim::kernel::Service;
// using unisim::service::interfaces::MemoryInjection;
// using unisim::service::interfaces::TrapReporting;
// using unisim::service::interfaces::Disassembly;
// using unisim::service::interfaces::Memory;
// using unisim::service::interfaces::LinuxOS;
// using unisim::service::interfaces::SymbolTableLookup;
// using unisim::util::endian::E_BIG_ENDIAN;
// using unisim::util::endian::E_LITTLE_ENDIAN;
// using unisim::util::endian::BigEndian2Host;
// using unisim::util::endian::LittleEndian2Host;
// using unisim::kernel::logger::DebugInfo;
// using unisim::kernel::logger::EndDebugInfo;
// using unisim::kernel::logger::DebugWarning;
// using unisim::kernel::logger::EndDebugWarning;
// using unisim::kernel::logger::DebugError;
// using unisim::kernel::logger::EndDebugError;

namespace {
  template <unsigned NIBBLES>
  struct HexDump
  {
    char buffer[NIBBLES+1];
    HexDump( uint32_t value ) {
      char* ptr = &buffer[NIBBLES];
      *ptr-- = '\0';
      while (ptr >= &buffer[0])
        { *ptr-- = "0123456789abcdef"[value&0xf]; value >>= 4; }
    }
    char const* s() const { return &buffer[0]; }
  };
}

struct DebugAccess { static bool const DEBUG = true; static bool const VERBOSE = true; };
struct QuietAccess { static bool const DEBUG = true; static bool const VERBOSE = false; };
struct PlainAccess { static bool const DEBUG = false; static bool const VERBOSE = false; };


/** Constructor.
 *
 * @param name the name that will be used by the UNISIM service
 *   infrastructure and will identify this object
 * @param parent the parent object of this object
 */
template <class CPU_IMPL>
CPU<CPU_IMPL>::CPU(const char* name, unisim::kernel::Object* parent)
  : unisim::kernel::Object(name, parent)
  , unisim::component::cxx::processor::arm::CPU<simfloat::FP,CPU_IMPL>(name, parent)
  , Client<unisim::service::interfaces::InstructionCollecting<uint32_t> >(name, parent)
  , Service<MemoryAccessReportingControl>(name, parent)
  , Client<unisim::service::interfaces::MemoryAccessReporting<uint32_t> >(name, parent)
  , Service<MemoryInjection<uint32_t> >(name, parent)
  , Client<unisim::service::interfaces::DebugYielding>(name, parent)
  , Client<unisim::service::interfaces::TrapReporting>(name, parent)
  , Service<Disassembly<uint32_t> >(name, parent)
  , Service< Memory<uint32_t> >(name, parent)
  , Client< Memory<uint32_t> >(name, parent)
  , Client<unisim::service::interfaces::LinuxOS>(name, parent)
  , Client<unisim::service::interfaces::SymbolTableLookup<uint32_t> >(name, parent)
  , instruction_collecting_import("instruction-collecting-import", this)
  , memory_access_reporting_control_export("memory-access-reporting-control-export", this)
  , memory_access_reporting_import("memory-access-reporting-import", this)
  , disasm_export("disasm-export", this)
  , memory_injection_export("memory-injection-export", this)
  , memory_export("memory-export", this)
  , memory_import("memory-import", this)
  , debug_yielding_import("debug-yielding-import", this)
  , symbol_table_lookup_import("symbol-table-lookup-import", this)
  , trap_reporting_import("trap-reporting-import", this)
  , linux_os_import("linux-os-import", this)
  , requires_memory_access_reporting(false)
  , requires_fetch_instruction_reporting(false)
  , requires_commit_instruction_reporting(false)
  // , icache("icache", this)
  // , dcache("dcache", this)
  , arm32_decoder()
  , thumb_decoder()
  , csselr(0)
  , DFSR()
  , IFSR()
  , DFAR()
  , IFAR()
  , mmu()
  , instruction_counter(0)
  , trap_on_instruction_counter(0)
  , ipb_base_address( -1 )
  , linux_printk_buf_addr( 0 )
  , linux_printk_buf_size( 0 )
  , linux_printk_snooping( false )
  , halt_on_addr( uint32_t(-1) )
  , halt_on_location()
  , param_verbose("verbose", this, this->PCPU::verbose, "Activate the verbose system.")
  , param_trap_on_instruction_counter("trap-on-instruction-counter", this, trap_on_instruction_counter,
                                      "Produce a trap when the given instruction count is reached.")
  , param_linux_printk_snooping( "linux-printk-snooping", this, linux_printk_snooping, "Activate the printk snooping" )
  , param_halt_on_location( "halt-on-location", this, halt_on_location,
                            "Tell the CPU to halt simulation on a specific instruction (address or symbol)." )
  , stat_instruction_counter("instruction-counter", this, instruction_counter, "Number of instructions executed.")
{
  // Set the right format for various of the variables
  param_trap_on_instruction_counter.SetFormat(unisim::kernel::VariableBase::FMT_DEC);
  stat_instruction_counter.SetFormat(unisim::kernel::VariableBase::FMT_DEC);

  // Active Variables for specific CPU handling
  unisim::kernel::VariableBase* var = 0;

  struct ReadPhysicalMemory : public unisim::kernel::VariableBase
  {
    ReadPhysicalMemory( CPU& _cpu )
      : VariableBase("read-phys-mem", &_cpu, unisim::kernel::VariableBase::VAR_PARAMETER, ""), cpu( _cpu )
    {
      SetVisible(false);
      SetSerializable(false);
    }

    VariableBase& operator = (char const* args)
    {
      uint32_t addr = strtoul(args,0,0);
      uint32_t byte = addr % 16;
      addr &= -uint32_t(16);
      uint8_t buffer[16];

      cpu.logger << DebugInfo
                 << " address  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f" << std::endl
                 << HexDump<8>(addr).s();

      if (not static_cast<CPU_IMPL&>(cpu).ExternalReadMemory( addr, &buffer[0], 16 )) {
        cpu.logger <<       " ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ??" << std::endl;
      } else {
        for (int idx = 0; idx < 16; ++idx)
          cpu.logger << ' ' << HexDump<2>(buffer[idx]).s();
        cpu.logger << std::endl;
      }
      cpu.logger << "         "; for (;byte > 0; --byte) cpu.logger << "   "; cpu.logger << "^" << EndDebugInfo;
      return *this;
    }
    CPU& cpu;
  };
  var = new ReadPhysicalMemory( *this );
  this->variable_register_pool.insert( var );

  struct VirtToPhys : public unisim::kernel::VariableBase
  {
    VirtToPhys( CPU& _cpu )
      : VariableBase("virt2phys", &_cpu, unisim::kernel::VariableBase::VAR_PARAMETER, ""), cpu( _cpu )
    {
      SetVisible(false);
      SetSerializable(false);
    }

    VariableBase& operator = (char const* args)
    {
      uint32_t addr = strtoul(args,0,0);

      try {
        AddressDescriptor loc( addr );
        cpu.TranslateAddress<DebugAccess>( loc, true, mat_read, 1 );
        cpu.logger << DebugInfo << "V[0x" << std::hex << addr << "] : P[0x" << std::hex << loc.address << "]";
      } catch (DataAbortException const&)
        { cpu.logger << DebugInfo << "V[0x" << std::hex << addr << "] : unmapped"; }
      cpu.logger << EndDebugInfo;

      return *this;
    }
    CPU& cpu;
  };
  var = new VirtToPhys( *this );
  this->variable_register_pool.insert( var );

  /* Debug Register */
  this->registers_registry["ttbcr"] = new unisim::util::debug::SimpleRegister<uint32_t>( "TTBCR", &mmu.ttbcr );
  this->registers_registry["ttbr0"] = new unisim::util::debug::SimpleRegister<uint32_t>( "TTBR0", &mmu.ttbr0 );
  this->registers_registry["ttbr1"] = new unisim::util::debug::SimpleRegister<uint32_t>( "TTBR1", &mmu.ttbr1 );
}

/** Destructor.
 */
template <class CPU_IMPL>
CPU<CPU_IMPL>::~CPU()
{
}

/** Object setup method.
 * This method is called during the setup phase (1st phase).
 *
 * @return true on success, false otherwise
 */
template <class CPU_IMPL>
bool
CPU<CPU_IMPL>::BeginSetup()
{
  if (verbose)
    logger << DebugInfo << "CPU::BeginSetup(): Verbose activated." << EndDebugInfo;

  return true;
}

/** Resets the internal values of corresponding CP15 Registers
 */
template <class CPU_IMPL>
void
CPU<CPU_IMPL>::TakeReset()
{
  this->PCPU::TakeReset();

  // VMSA default values for SCTLR (may be overwritten by implementations)
  sctlr::AFE.Set(     this->SCTLR, 0 ); // Access flag enable.
  sctlr::TRE.Set(     this->SCTLR, 0 ); // TEX remap enable
  sctlr::UWXN.Set(    this->SCTLR, 0 ); // Unprivileged write permission implies PL1 XN (Virtualization Extensions)
  sctlr::WXN.Set(     this->SCTLR, 0 ); // Write permission implies XN (Virtualization Extensions)
  sctlr::HA.Set(      this->SCTLR, 0 ); // Hardware Access flag enable.
}

/** Object setup method.
 * This method is called during the setup phase (2nd and final phase).
 *
 * @return true on success, false otherwise
 */
template <class CPU_IMPL>
bool
CPU<CPU_IMPL>::EndSetup()
{
  /* Finalizing LOAD job */
  if (not linux_os_import)
    {
      if (verbose)
        logger << DebugInfo << "No Linux OS emulation ==> **bare metal/full system** mode" << EndDebugInfo;
      static_cast<CPU_IMPL*>(this)->TakeReset();
    }
  else
    {
      /* Linux OS has setup Memory and User Registers */
      if (verbose)
        logger << DebugInfo << "With Linux OS emulation" << EndDebugInfo;
    }

  if (verbose)
    logger << DebugInfo << "Initial pc set to 0x" << std::hex << next_insn_addr << std::dec << EndDebugInfo;

  /* If the memory access reporting import is not connected remove the
   *   need of reporting memory accesses and finished instruction.
   */
  if(!memory_access_reporting_import) {
    requires_memory_access_reporting = false;
    requires_fetch_instruction_reporting = false;
    requires_commit_instruction_reporting = false;
  }

  typedef unisim::util::debug::Symbol<uint32_t> Symbol;

  if (halt_on_location.size()) {
    if (verbose)
      logger << DebugInfo << "Halt-On instruction enabled" << EndDebugInfo;

    if (Symbol const* symbol = symbol_table_lookup_import ?
        symbol_table_lookup_import->FindSymbolByName(halt_on_location.c_str(), Symbol::SYM_FUNC) : 0)
      {
        halt_on_addr = symbol->GetAddress();

        if (verbose) logger << DebugInfo << "Found " << halt_on_location << EndDebugInfo;
      }
    else
      {
        char const* location = halt_on_location.c_str();
        char* endptr;
        halt_on_addr = strtoul(location, &endptr, 0);
        if ((*location == '\0') or (*endptr != '\0'))
          halt_on_addr = uint32_t(-1);
      }

    if (verbose) {
      if (halt_on_addr != uint32_t(-1))
        logger << DebugInfo << "Simulation will stop on instruction @" << std::hex << halt_on_addr << EndDebugInfo;
      else
        logger << DebugWarning
               << halt_on_location << " is not a valid instruction address; halt-on feature disabled."
               << EndDebugWarning;
    }

  }

  if (linux_printk_snooping)
    {
      if (verbose)
        logger << DebugInfo << "Linux printk snooping enabled" << EndDebugInfo;

      if (Symbol const* symbol = symbol_table_lookup_import ?
          symbol_table_lookup_import->FindSymbolByName("__log_buf", Symbol::SYM_OBJECT) : 0)
        {
          linux_printk_buf_addr = symbol->GetAddress();
          linux_printk_buf_size = symbol->GetSize();
          linux_printk_snooping = true;

          if (verbose)
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

/** Performs a prefetch access.
 *
 * @param addr the address of the memory prefetch access
 */
template <class CPU_IMPL>
void
CPU<CPU_IMPL>::PerformPrefetchAccess( uint32_t addr )
{
  /* it is just a cache prefetch, ignore the request if cache is not active */

  /* TODO: shouldn't we report a memory access for a prefetch???? */
}

/** Performs an unaligned write access.
 * @param addr the address of the memory write access
 * @param size the size of the memory write access
 * @param value the value of the memory write access
 */
template <class CPU_IMPL>
void
CPU<CPU_IMPL>::PerformUWriteAccess( uint32_t addr, uint32_t size, uint32_t value )
{
  uint32_t const lo_mask = size - 1;
  if (unlikely((lo_mask > 3) or (size & lo_mask))) throw std::logic_error("Bad size");
  uint32_t misalignment = addr & lo_mask;

  if (unlikely(misalignment and not arm::sctlr::A.Get( this->SCTLR ))) {
    uint32_t eaddr = addr;
    if (GetEndianness() == unisim::util::endian::E_BIG_ENDIAN)
      {
        for (unsigned byte = size; --byte < size; ++eaddr)
          PerformWriteAccess( eaddr, 1, (value >> (8*byte)) & 0xff );
      }
    else
      {
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
template <class CPU_IMPL>
void
CPU<CPU_IMPL>::PerformWriteAccess( uint32_t addr, uint32_t size, uint32_t value )
{
  uint32_t const lo_mask = size - 1;
  if (unlikely((lo_mask > 3) or (size & lo_mask))) throw std::logic_error("bad size");
  uint32_t misalignment = addr & lo_mask;

  if (unlikely(misalignment)) {
    if (linux_os_import) {
      // we are executing on linux emulation mode, handle all misalignemnt as if implemented
      PerformUWriteAccess( addr, size, value );
      return;
    }
    else {
      // TODO: provide correct alignment fault mva (va + FCSE
      // translation) + provide correct LDFSRformat (see ARM Doc
      // AlignmentFaultV)
      DataAbort( addr, 0, 0, 0, mat_write, DAbort::Alignment, cpsr.Get(M) == HYPERVISOR_MODE, false, false, false, false );
    }
  }

  uint8_t data[4];

  if (GetEndianness() == unisim::util::endian::E_BIG_ENDIAN)
    {
      uint32_t shifter = value;
      for (int byte = size; --byte >= 0;)
        { data[byte] = shifter; shifter >>= 8; }
    }
  else
    {
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

  uint32_t mva = addr & ~lo_mask;
  AddressDescriptor loc( mva );
  TranslateAddress<PlainAccess>( loc, cpsr.Get(M) != USER_MODE, mat_write, size );

  // Send the request to the memory interface
  if (not static_cast<CPU_IMPL*>(this)->PhysicalWriteMemory( mva, loc.address, data, size, loc.attributes )) {
    DataAbort(addr, loc.address, 0, 0, mat_write, DAbort::SyncExternal, false, false, true, false, false);
  }

  /* report read memory access if necessary */
  this->ReportMemoryAccess( unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, addr, size );
}

/** Performs an unaligned read access.
 * @param addr the address of the memory read access
 * @param size the size of the memory read access
 */
template <class CPU_IMPL>
uint32_t
CPU<CPU_IMPL>::PerformUReadAccess( uint32_t addr, uint32_t size )
{
  uint32_t const lo_mask = size - 1;
  if (unlikely((lo_mask > 3) or (size & lo_mask))) throw std::logic_error("bad size");
  uint32_t misalignment = addr & lo_mask;

  if (unlikely(misalignment and not arm::sctlr::A.Get( this->SCTLR ))) {
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
template <class CPU_IMPL>
uint32_t
CPU<CPU_IMPL>::PerformReadAccess(	uint32_t addr, uint32_t size )
{
  uint32_t const lo_mask = size - 1;
  if (unlikely((lo_mask > 3) or (size & lo_mask))) throw std::logic_error("bad size");
  uint32_t misalignment = addr & lo_mask;

  if (unlikely(misalignment)) {
    if (linux_os_import) {
      // we are executing on linux emulation mode, handle all misalignemnt as if implemented
      return PerformUReadAccess( addr, size );
    }
    else {
      // TODO: provide correct alignment fault mva (va + FCSE
      // translation) + provide correct LDFSRformat (see ARM Doc
      // AlignmentFaultV)
      DataAbort( addr, 0, 0, 0, mat_read, DAbort::Alignment, cpsr.Get(M) == HYPERVISOR_MODE, false, false, false, false );
    }
  }

  uint32_t mva = addr & ~lo_mask;
  AddressDescriptor loc( mva );

  TranslateAddress<PlainAccess>( loc, cpsr.Get(M) != USER_MODE, mat_read, size );

  uint8_t data[4];

  // just read the data from the memory system
  if (not static_cast<CPU_IMPL*>(this)->PhysicalReadMemory(mva, loc.address, &data[0], size, loc.attributes)) {
    DataAbort(addr, loc.address, 0, 0, mat_read, DAbort::SyncExternal, false, false, true, false, false);
  }

  /* report read memory access if necessary */
  this->ReportMemoryAccess(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, addr, size);

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

template <class CPU_IMPL>
template <class Operation>
void
CPU<CPU_IMPL>::CollectInstruction(Operation* op)
{
  unisim::component::cxx::processor::arm::cfg::aarch32::ComputeBranchInfo(op, this->itcond());
  op->branch.Collect(op, this->current_insn_addr, this->next_insn_addr, *instruction_collecting_import);
}


/** Execute one complete instruction.
 */
template <class CPU_IMPL>
void
CPU<CPU_IMPL>::StepInstruction()
{
  /* Instruction boundary next_insn_addr becomes current_insn_addr */
  uint32_t insn_addr = current_insn_addr = next_insn_addr, insn_length = 0;

  if (insn_addr == halt_on_addr) { Stop(0); return; }

  if (unlikely(trap_reporting_import and (trap_on_instruction_counter == instruction_counter)))
    trap_reporting_import->ReportTrap(*this,"Reached instruction counter");

  if (unlikely(requires_fetch_instruction_reporting and memory_access_reporting_import))
    memory_access_reporting_import->ReportFetchInstruction(current_insn_addr);

  if (debug_yielding_import)
    debug_yielding_import->DebugYield();

  try {
    // Instruction Fetch Decode and Execution (may generate exceptions
    // known as synchronous aborts since their occurences are a direct
    // consequence of the instruction execution).
    if (cpsr.Get( T )) {
      /* Thumb state */
      isa::thumb::CodeType insn;
      ReadInsnT(insn_addr, insn);

      /* Decode current PC */
      isa::thumb::Operation<CPU_IMPL>* op = thumb_decoder.Decode(insn_addr, insn);

      /* update PC register value before execution */
      insn_length = op->GetLength() / 8;
      this->gpr[15] = insn_addr + 4;
      next_insn_addr = insn_addr + insn_length;

      /* Execute instruction */
      CPU_IMPL& self = static_cast<CPU_IMPL&>(*this);
      asm volatile( "thumb_operation_execute:" );
      if (likely(CheckCondition(self, this->itcond())))
        op->execute( self );

      if(unlikely(instruction_collecting_import))
        CollectInstruction(op);

      this->ITAdvance();
    }

    else {
      /* Arm32 state */

      /* fetch instruction word from memory */
      isa::arm32::CodeType insn;
      ReadInsnA(insn_addr, insn);

      /* Decode current PC */
      isa::arm32::Operation<CPU_IMPL>* op = arm32_decoder.Decode(insn_addr, insn);

      /* update PC register value before execution */
      insn_length = op->GetLength() / 8;
      this->gpr[15] = insn_addr + 8;
      next_insn_addr = insn_addr + insn_length;

      /* Execute instruction */
      CPU_IMPL& self = static_cast<CPU_IMPL&>(*this);
      asm volatile( "arm32_operation_execute:" );
      if (likely(CheckCondition(self, (insn >> 28) & 0xf)))
        op->execute( self );

      if(unlikely(instruction_collecting_import))
        CollectInstruction(op);
    }

    if (unlikely(requires_commit_instruction_reporting and memory_access_reporting_import))
      memory_access_reporting_import->ReportCommitInstruction(current_insn_addr, insn_length);

    instruction_counter++; /* Instruction regularly finished */
  }

  catch (SVCException const& svexc) {
    /* Resuming execution, since SVC exceptions are explicitly
     * requested from regular instructions.  ITState will be updated
     * as needed by TakeSVCException (as required by the ARM reference
     * specification).
     */
    if (unlikely( requires_commit_instruction_reporting and memory_access_reporting_import ))
      memory_access_reporting_import->ReportCommitInstruction(current_insn_addr, insn_length);

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
CPU<CPU_IMPL>::InjectReadMemory( uint32_t addr, void* buffer, uint32_t size )
{
  uint8_t* rbuffer = (uint8_t*)buffer;

  // No data cache, just send request to the memory subsystem
  for (uint32_t index = 0; size != 0; ++index, --size)
    {
      uint32_t ef_addr = addr + index;
      if (not static_cast<CPU_IMPL*>(this)->PhysicalReadMemory(ef_addr, ef_addr, &rbuffer[index], 1, 0))
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
CPU<CPU_IMPL>::InjectWriteMemory( uint32_t addr, void const* buffer, uint32_t size )
{
  uint8_t const* wbuffer = (uint8_t const*)buffer;

  // No data cache, just send the request to the memory subsystem
  for (uint32_t index = 0; size != 0; ++index, --size)
    {
      uint32_t ef_addr = addr + index;
      if (not static_cast<CPU_IMPL*>(this)->PhysicalWriteMemory( ef_addr, ef_addr, &wbuffer[index], 1, 0 ))
        return false;
    }

  return true;
}

/** Set/unset the reporting of memory accesses.
 *
 * @param type specify the type of report that are being modified
 * @param report if true/false sets/unsets the reporting of memory
 * acesseses
 */
template <class CPU_IMPL>
void
CPU<CPU_IMPL>::RequiresMemoryAccessReporting( unisim::service::interfaces::MemoryAccessReportingType type, bool report )
{
  if (report and not memory_access_reporting_import)
    throw std::logic_error("missing memory_access_reporting_import");

  switch (type)
    {
    case unisim::service::interfaces::REPORT_MEM_ACCESS:  requires_memory_access_reporting = report; break;
    case unisim::service::interfaces::REPORT_FETCH_INSN:  requires_fetch_instruction_reporting = report; break;
    case unisim::service::interfaces::REPORT_COMMIT_INSN: requires_commit_instruction_reporting = report; break;
    default: throw std::logic_error("Unknown reporting types");
    }
}

/** Perform a non intrusive read access. Local implementation using memory_import (see CPU::ReadMemory) */
template <class CPU_IMPL>
bool
CPU<CPU_IMPL>::ExternalReadMemory( uint32_t addr, void* buffer, uint32_t size )
{
  return memory_import->ReadMemory( addr, buffer, size );
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
CPU<CPU_IMPL>::ReadMemory( uint32_t addr, void* buffer, uint32_t size )
{
  uint8_t* rbuffer = (uint8_t*)buffer;

  // No data cache, just send request to the memory subsystem
  for (uint32_t index = 0; size != 0; ++index, --size)
    {
      try {
	AddressDescriptor loc(addr + index);
        TranslateAddress<QuietAccess>( loc, true, mat_read, 1 );
        if (not static_cast<CPU_IMPL*>(this)->ExternalReadMemory( loc.address, &rbuffer[index], 1 ))
          return false;
      }
      catch (DataAbortException const& x)
        { return false; }
    }

  return true;
}

/** Perform a non intrusive write access. Local implementation using memory_import (see CPU::WriteMemory) */
template <class CPU_IMPL>
bool
CPU<CPU_IMPL>::ExternalWriteMemory( uint32_t addr, void const* buffer, uint32_t size )
{
  return memory_import->WriteMemory( addr, buffer, size );
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
CPU<CPU_IMPL>::WriteMemory( uint32_t addr, void const* buffer, uint32_t size )
{
  uint8_t const* wbuffer = (uint8_t const*)buffer;

  // No data cache, just send request to the memory subsystem
  for (uint32_t index = 0; size != 0; ++index, --size)
    {
      try {
	AddressDescriptor loc(addr + index);
        TranslateAddress<QuietAccess>( loc, true, mat_write, 1 );
        if (not static_cast<CPU_IMPL*>(this)->ExternalWriteMemory( loc.address, &wbuffer[index], 1 ))
          return false;
      } catch (DataAbortException const& x)
        { return false; }
    }

  return true;
}

/** Disasm an instruction address.
 *
 * Returns a string with the disassembling of the instruction found at
 * address addr. The strategy to distinguish between arm32 and thumb2
 * consists of following steps:
 * - Detect an unambiguous thumb2 alignment signature
 * - Find an existing cached op (arm32 then thumb2) matching encoding
 * - Use current Thumb state
 *
 * @param addr the address of the instruction to disassemble
 * @param next_addr the address following the requested instruction
 *
 * @return the disassembling of the requested instruction address
 */
template <class CPU_IMPL>
std::string
CPU<CPU_IMPL>::Disasm(uint32_t addr, uint32_t& next_addr)
{
  std::stringstream buffer;
  bool is_thumb = cpsr.Get( T );
  uint32_t insn, insn_addr = addr&-2;
  if (not ReadMemory(insn_addr, (void*)&insn, 4)) return "??";
  // In ARMv7am, instruction fetch ignores "Endianness execution state bit"
  insn = util::endian::LittleEndian2Host(insn);

  struct insn_bits { static unsigned pitch(std::ostream& sink, uint32_t insn, unsigned len) {
    int m = 32-len; sink << "0x" << std::hex << std::setw(len/4) << std::setfill('0') << (insn << m >> m);
    return len;
  }};

  struct arm32_code { static std::string disasm(this_type* self, isa::arm32::Operation<CPU_IMPL>& op, uint32_t insn, uint32_t cur_addr, uint32_t& nxt_addr) {
    std::ostringstream buffer;
    nxt_addr = cur_addr + insn_bits::pitch(buffer, insn, op.GetLength());
    op.disasm(*static_cast<CPU_IMPL*>(self), buffer << "(A) ");
    return buffer.str();
  }};

  struct thumb_code { static std::string disasm(this_type* self, isa::thumb::Operation<CPU_IMPL>& op, uint32_t insn, uint32_t cur_addr, uint32_t& nxt_addr) {
    std::ostringstream buffer;
    nxt_addr = cur_addr + insn_bits::pitch(buffer, insn, op.GetLength());
    op.disasm(*static_cast<CPU_IMPL*>(self), buffer << "(T) ");
    return buffer.str();
  }};

  if (addr & 3)
    is_thumb = true;
  else if (auto arm32_insn = arm32_decoder.Retrieve(insn_addr, insn))
    return arm32_code::disasm(this, *arm32_insn, insn, addr, next_addr);
  if (auto thumb_insn = thumb_decoder.Retrieve(insn_addr, insn))
    return thumb_code::disasm(this, *thumb_insn, insn, addr, next_addr);

  try
    {
      if (is_thumb)
        {
          auto op = std::unique_ptr<isa::thumb::Operation<CPU_IMPL>>(thumb_decoder.NCDecode(insn_addr, insn));
          return thumb_code::disasm(this, *op, insn, addr, next_addr);
        }
      auto op = std::unique_ptr<isa::arm32::Operation<CPU_IMPL>>(arm32_decoder.NCDecode(insn_addr, insn));
      return arm32_code::disasm(this, *op, insn, addr, next_addr);
    }
  catch (isa::Reject const&) {}

  next_addr = addr + 4;
  return "??";
}

/** Exit system call.
 * The LinuxOS service calls this method when the program has finished.
 *
 * @param ret the exit cade sent by the exit system call.
 */
template <class CPU_IMPL>
void
CPU<CPU_IMPL>::PerformExit(int ret)
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
 * @param mva      the modified virtual address of the fetched instruction
 * @param line_loc the physical address descriptor used to refill the cache line
 */
template <class CPU_IMPL>
void
CPU<CPU_IMPL>::RefillInsnPrefetchBuffer(uint32_t mva, AddressDescriptor const& line_loc)
{
  this->ipb_base_address = line_loc.address;

  // No instruction cache present, just request the insn to the
  // memory system.
  if (not static_cast<CPU_IMPL*>(this)->PhysicalFetchMemory(mva & -(IPB_LINE_SIZE), line_loc.address, &this->ipb_bytes[0], IPB_LINE_SIZE, line_loc.attributes)) {
    DataAbort(mva, line_loc.address, 0, 0, mat_exec, DAbort::SyncExternal, false, false, true, false, false);
  }

  this->ReportMemoryAccess(unisim::util::debug::MAT_READ, unisim::util::debug::MT_INSN, line_loc.address, IPB_LINE_SIZE);
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
CPU<CPU_IMPL>::ReadInsnA(uint32_t address, unisim::component::cxx::processor::arm::isa::arm32::CodeType& insn)
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
  // In ARMv7am, instruction fetch ignores "Endianness execution state bit"
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
template <class CPU_IMPL>
void
CPU<CPU_IMPL>::ReadInsnT(uint32_t address, unisim::component::cxx::processor::arm::isa::thumb::CodeType& insn)
{
  AddressDescriptor loc(address & -(IPB_LINE_SIZE));
  bool ispriv = cpsr.Get(M) != USER_MODE;
  TranslateAddress<PlainAccess>( loc, ispriv, mat_exec, IPB_LINE_SIZE );
  intptr_t buffer_index = address % (IPB_LINE_SIZE);
  uint8_t bytes[4];

  if (unlikely(ipb_base_address != loc.address))
    {
      RefillInsnPrefetchBuffer( address, loc );
    }

  // In ARMv7, instruction fetch ignores "Endianness execution state bit"
  bytes[0] = ipb_bytes[buffer_index+0];
  bytes[1] = ipb_bytes[buffer_index+1];
  if (unlikely((buffer_index+2) >= IPB_LINE_SIZE)) {
    address = loc.address = address + 2;
    TranslateAddress<PlainAccess>( loc, ispriv, mat_exec, IPB_LINE_SIZE );
    RefillInsnPrefetchBuffer( address, loc );
    buffer_index = intptr_t(-2);
  }
  bytes[2] = ipb_bytes[buffer_index+2];
  bytes[3] = ipb_bytes[buffer_index+3];
  uint32_t word;
  memcpy( &word, &bytes[0], 4 );
  // In ARMv7, instruction fetch ignores "Endianness execution state bit"
  insn = Target2Host(unisim::util::endian::E_LITTLE_ENDIAN, word);
}

/** CallSupervisor
 *
 *  This method is called by SWI/SVC instructions to handle software interrupts.
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
        }
      catch (Exception const& e)
        {
          std::cerr << e.what() << std::endl;
          this->Stop( -1 );
        }
    }
  else
    {
      //trap_reporting_import->ReportTrap(*this, "CallSupervisor");

      if (verbose)
        {
          static struct ArmLinuxOS : public unisim::util::os::linux_os::Linux<uint32_t, uint32_t>
          {
            ArmLinuxOS( CPU* _cpu )
              : unisim::util::os::linux_os::Linux<uint32_t, uint32_t>( _cpu->logger.DebugInfoStream(), _cpu->logger.DebugWarningStream(), _cpu->logger.DebugErrorStream())
            {
              SetInterfaces(_cpu, _cpu, _cpu);
              typedef unisim::util::os::linux_os::ARMTS<unisim::util::os::linux_os::Linux<uint32_t,uint32_t> > ArmTarget;
              SetTargetSystem(new ArmTarget( "arm-eabi", *this ));
            }
            ~ArmLinuxOS() { delete GetTargetSystem(); }
          } arm_linux_os( this );

          logger << DebugInfo << "PC: 0x" << std::hex << current_insn_addr << EndDebugInfo;
          arm_linux_os.LogSystemCall( imm );
        }

      // we are executing on full system mode
      this->PCPU::CallSupervisor( imm );
    }
}

/** Breakpoint
 *  This method is called by BKPT instructions to handle breakpoints.
 */
template <class CPU_IMPL>
void
CPU<CPU_IMPL>::BKPT( uint32_t imm )
{
  // we are executing on linux emulation mode
  throw std::logic_error( "TODO: Generate a debug event: virtual BKPTInstrDebugEvent() method ?" );
}

template <class CPU_IMPL>
void
CPU<CPU_IMPL>::UndefinedInstruction( isa::arm32::Operation<CPU_IMPL>* insn )
{
  std::ostringstream disasm;
  insn->disasm( *static_cast<CPU_IMPL*>(this), disasm );

  logger << DebugWarning << "Undefined instruction @" << std::hex << current_insn_addr << std::dec
         << ": " << disasm.str() << " ; (" << insn->GetName() << ")" << EndDebugWarning;

  if (linux_os_import)
    this->Stop( -1 );
  else
    throw UndefInstrException();
}

template <class CPU_IMPL>
void
CPU<CPU_IMPL>::UndefinedInstruction( isa::thumb::Operation<CPU_IMPL>* insn )
{
  std::ostringstream disasm;
  insn->disasm( *static_cast<CPU_IMPL*>(this), disasm );

  logger << DebugWarning << "Undefined instruction @" << std::hex << current_insn_addr << std::dec
         << ": " << disasm.str() << " ; (" << insn->GetName() << ")" << EndDebugWarning;

  if (linux_os_import)
    this->Stop( -1 );
  else
    throw UndefInstrException();
}

template <class CPU_IMPL>
void
CPU<CPU_IMPL>::DataAbort(uint32_t va, uint64_t ipa,
               unsigned domain, int level, mem_acc_type_t mat,
               DAbort type, bool taketohypmode, bool s2abort,
               bool ipavalid, bool LDFSRformat, bool s2fs1walk)
{
  uint32_t& UFSR = (mat == mat_exec) ? IFSR : DFSR;
  uint32_t& UFAR = (mat == mat_exec) ? IFAR : DFAR;

  if (not taketohypmode) {
    // UFSR = bits(32) UNKNOWN;
    // UFAR = bits(32) UNKNOWN;

    // Asynchronous abort don't update DFAR. Synchronous Watchpoint
    // (DAbort::DebugEvent) update DFAR since debug v7.1.
    switch (type.code) {
    default: UFAR = va; break;
    case DAbort::AsyncParity: case DAbort::AsyncExternal: break;
    }
    if (LDFSRformat) {
      throw std::logic_error("Long descriptors format not supported");
      // // new format
      // UFSR<13> = TLBLookupCameFromCacheMaintenance();
      // if (type IN (DAbort::AsyncExternal,DAbort::SyncExternal))
      //   UFSR<12> = IMPLEMENTATION_DEFINED;
      // else
      //   UFSR<12> = '0';
      // UFSR<11> = if (mat == mat_write) then '1' else '0';
      // UFSR<10> = bit UNKNOWN;
      // UFSR<9> = '1';
      // UFSR<8:6> = bits(3) UNKNOWN;
      // UFSR<5:0> = EncodeLDFSR(type, level);
    }
    else { // Short descriptor format
      // UFSR<13> = TLBLookupCameFromCacheMaintenance();
      unisim::util::arithmetic::BitField<13,1>().Set( UFSR, 0 );
      if ((type != DAbort::SyncExternal) and (type != DAbort::AsyncExternal))
        unisim::util::arithmetic::BitField<12,1>().Set( UFSR, 0 );
      unisim::util::arithmetic::BitField<11,1>().Set( UFSR, (mat == mat_write) ? 1 : 0 );
      unisim::util::arithmetic::BitField<9,1>().Set( UFSR, 0 );
      // UFSR<8> = bit UNKNOWN;
      unisim::util::arithmetic::BitField<8,1>().Set( UFSR, 0 );
      // domain_valid = ((type == DAbort::Domain) ||
      //                 ((level == 2) &&
      //                  (type IN {DAbort::Translation, DAbort::AccessFlag,
      //                      DAbort::SyncExternalonWalk, DAbort::SyncParityonWalk})) ||
      //                 (!HaveLPAE() && (type == DAbort::Permission)));
      // if (domain_valid)   UFSR<7:4> = domain;
      // else                UFSR<7:4> = bits(4) UNKNOWN;
      unisim::util::arithmetic::BitField<4,4>().Set( UFSR, domain );
      struct FS {
        FS( uint32_t& _dfsr ) : dfsr( _dfsr ) {} uint32_t& dfsr;
        void Set( uint32_t value ) {
          unisim::util::arithmetic::BitField<10,1>().Set( dfsr, value >> 4 );
          unisim::util::arithmetic::BitField<0,4>() .Set( dfsr, value >> 0 );
        }
      } fault_status( UFSR );
      switch (type.code) {
      case DAbort::AccessFlag:         fault_status.Set( level==1 ? 0b00011 : 0b00110 ); break;
      case DAbort::Alignment:          fault_status.Set( 0b00001 ); break;
      case DAbort::Permission:         fault_status.Set( 0b01101 | (level&2) ); break;
      case DAbort::Domain:             fault_status.Set( 0b01001 | (level&2) ); break;
      case DAbort::Translation:        fault_status.Set( 0b00101 | (level&2) ); break;
      case DAbort::SyncExternal:       fault_status.Set( 0b01000 ); break;
      case DAbort::SyncExternalOnWalk: fault_status.Set( 0b01100 | (level&2) ); break;
      case DAbort::SyncParity:         fault_status.Set( 0b11001 ); break;
      case DAbort::SyncParityOnWalk:   fault_status.Set( 0b11100 | (level&2) ); break;
      case DAbort::AsyncParity:        fault_status.Set( 0b11000 ); break;
      case DAbort::AsyncExternal:      fault_status.Set( 0b10110 ); break;
      case DAbort::Debug:              fault_status.Set( 0b00010 ); break;
      case DAbort::TLBConflict:        fault_status.Set( 0b10000 ); break;
      case DAbort::Lockdown:           fault_status.Set( 0b10100 ); break;
      case DAbort::Coproc:             fault_status.Set( 0b11010 ); break;
      case DAbort::ICacheMaint:        fault_status.Set( 0b00100 ); break;
      default: throw std::logic_error("Unhandled case");
      }
    }
  }
  else { // taketohypmode
    throw std::logic_error("Hypervision not supported");
    // bits(25) HSRString = Zeros(25);
    // bits(6) ec;
    // HDFAR = vaddress;
    // if (ipavalid)
    //   HPFAR<31:4> = ipaddress<39:12>;
    // if (secondstageabort) {
    //   ec = '100100';
    //   HSRString<24:16> = LSInstructionSyndrome();
    // } else {
    //   ec = '100101';
    //   HSRString<24> = '0'; // Instruction syndrome not valid
    // }
    // if (type IN (DAbort::AsyncExternal,DAbort::SyncExternal))
    //   HSRString<9> = IMPLEMENTATION_DEFINED;
    // else
    //   HSRString<9> = '0';
    // HSRString<8> = TLBLookupCameFromCacheMaintenance();
    // HSRString<7> = if s2fs1walk then '1' else '0';
    // HSRString<6> = if mat_write then '1' else '0';
    // HSRString<5:0> = EncodeLDFSR(type, level);
    // WriteHSR(ec, HSRString);
  }

  if (mat == mat_exec)
    throw PrefetchAbortException();
  else
    throw DataAbortException();
}

template <class CPU_IMPL>
void
CPU<CPU_IMPL>::MMU::refresh_attr_cache(bool tre)
{
  for (unsigned idx = 0; idx < 64; ++idx)
    {
      uint32_t attributes = 0;
      unsigned S = idx >> 5;

      if (tre)
        {
          unsigned n = idx & 7; /*TEX[0],C,B*/
          MemAttrs::type_t type = MemAttrs::type_t( (prrr >> 2*n) & 3 );
          MemAttrs::type().Set(attributes, type);
          MemAttrs::ConvAttrsHints( MemAttrs::Inner(), attributes, nmrr >> (2*n+ 0) );
          MemAttrs::ConvAttrsHints( MemAttrs::Outer(), attributes, nmrr >> (2*n+16) );
          unsigned shareable = (type == MemAttrs::Normal) ? ((prrr >> (18+S)) & 1) : (type == MemAttrs::Device) ? ((prrr >> (16+S)) & 1) : 1;
          unsigned outershareable = (((prrr >> (n+24)) & 1) or not shareable) ? 0 : 1;
          MemAttrs::shareable().Set(attributes,shareable);
          MemAttrs::outershareable().Set(attributes,outershareable);
        }
      else
        {
          if ((idx >> 4)&1) /*TEX[2]*/
            {
              MemAttrs::type().Set(attributes,MemAttrs::Normal);
              MemAttrs::ConvAttrsHints( MemAttrs::Inner(), attributes, idx >> 0 );
              MemAttrs::ConvAttrsHints( MemAttrs::Outer(), attributes, idx >> 2 );
            }
          else
            {
              switch (idx & 0xf) /*TEX[1:0],C,B*/
                {
                case 0b0000:
                  MemAttrs::type().Set(attributes,MemAttrs::StronglyOrdered);
                  break;
                case 0b0001:
                case 0b1000:
                  MemAttrs::type().Set(attributes,MemAttrs::Device);
                  S = idx & 1;
                  break;
                case 0b0010:
                  MemAttrs::type().Set(attributes,MemAttrs::Normal);
                  MemAttrs::ConvAttrsHints( MemAttrs::Inner(), attributes, 2 ); /* WriteThrough, ReadAllocate */
                  MemAttrs::ConvAttrsHints( MemAttrs::Outer(), attributes, 2 ); /* WriteThrough, ReadAllocate */
                  break;
                case 0b0011:
                  MemAttrs::type().Set(attributes,MemAttrs::Normal);
                  MemAttrs::ConvAttrsHints( MemAttrs::Inner(), attributes, 3 ); /* WriteBack, ReadAllocate */
                  MemAttrs::ConvAttrsHints( MemAttrs::Outer(), attributes, 3 ); /* WriteBack, ReadAllocate */
                  break;
                case 0b0100:
                  MemAttrs::type().Set(attributes,MemAttrs::Normal);
                  MemAttrs::ConvAttrsHints( MemAttrs::Inner(), attributes, 0 ); /* NonCacheable */
                  MemAttrs::ConvAttrsHints( MemAttrs::Outer(), attributes, 0 ); /* NonCacheable */
                  break;
                case 0b0111: break;
                  MemAttrs::type().Set(attributes,MemAttrs::Normal);
                  MemAttrs::ConvAttrsHints( MemAttrs::Inner(), attributes, 1 ); /* WriteBack, MemAttrs::RWAllocate */
                  MemAttrs::ConvAttrsHints( MemAttrs::Outer(), attributes, 1 ); /* WriteBack, MemAttrs::RWAllocate */
                  break;
                default:
                  MemAttrs::type().Set(attributes,3/*error*/);
                  break;
                }
            }
          MemAttrs::shareable().Set(attributes,S);
          MemAttrs::outershareable().Set(attributes,S);
        }

      attr_cache[idx] = attributes;
    }
}

template <class POLICY>
bool
TLB::GetTranslation( TLB::Entry& tlbe, uint32_t mva, uint32_t asid )
{
  unsigned lsb = 0, hit;
  uint32_t key = 0;
  Entry*   matching_tlbe = 0;

  for (hit = 0; hit < entry_count; ++hit)
    {
      key = keys[hit];
      lsb = key & 31;
      if ((mva ^ key) >> lsb)
        continue;
      matching_tlbe = &vals[(key >> 5) & 127];
      if (not matching_tlbe->nG or matching_tlbe->asid == asid)
        break;
    }
  if (hit >= entry_count)
    return false; // TLB miss

  // TLB hit
  if (not POLICY::DEBUG) {
    // MRU sort
    for (unsigned idx = hit; idx > 0; idx -= 1)
      keys[idx] = keys[idx-1];
    keys[0] = key;
  }

  // Address translation and attributes
  tlbe = *matching_tlbe;
  uint32_t himask = uint32_t(-1) << lsb;
  tlbe.pa = (tlbe.pa & himask) | (mva & ~himask);
  return true;
}

template <class CPU_IMPL>
template <class POLICY>
void
CPU<CPU_IMPL>::TranslationTableWalk( TLB::Entry& tlbe, uint32_t mva, mem_acc_type_t mat, unsigned size )
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
  if (POLICY::DEBUG and POLICY::VERBOSE) {
    logger << DebugInfo << "Translation comes from memory table" << EndDebugInfo;
  }

  struct EndianReader
  {
    uint8_t b[4]; bool be;
    uint32_t Get() const
    {
      typedef uint32_t w;
      if (be) return (w(b[0]) << 24) | (w(b[1]) << 16) | (w(b[2]) <<  8) | (w(b[3]) <<  0);
      else    return (w(b[0]) <<  0) | (w(b[1]) <<  8) | (w(b[2]) << 16) | (w(b[3]) << 24);
    }
    uint8_t* data() { return &b[0]; }
    EndianReader( bool _be ) : be( _be ) {}
  } erd( arm::sctlr::EE.Get( SCTLR ) );

  //bool     nG;

  // Determine correct Translation Table Base Register to use.
  uint32_t n = TTBCR::N.Get( mmu.ttbcr ), ttbr;
  if ((mva >> 1) >> (n^31)) {
    ttbr = mmu.ttbr1;
    n = 0;
  } else {
    ttbr = mmu.ttbr0;
  }

  // Obtain First level descriptor.
  uint32_t l1descaddr = (((ttbr >> (14-n)) << (14-n)) | ((mva << n) >> (n+18))) & -4;
  {
    bool success;
    uint32_t attrs=0;
    MemAttrs::type().Set(attrs,MemAttrs::Normal);

    if (POLICY::DEBUG) success = static_cast<CPU_IMPL*>(this)->ExternalReadMemory( l1descaddr, erd.data(), 4 );
    else               success = static_cast<CPU_IMPL*>(this)->PhysicalReadMemory( l1descaddr, l1descaddr, erd.data(), 4, 0 );
    if (not success)
      DataAbort(l1descaddr, l1descaddr, 0, 0, mat_read, DAbort::SyncExternalOnWalk, false, false, false, false, false);
  }
  uint32_t l1desc = erd.Get();
  switch (l1desc&3) {
  case 0: {
    // Fault, Reserved
    DataAbort(mva, 0, 0, 1, mat, DAbort::Translation, false, false, false, false, false);
  } break;

  case 1: {
    // Large page or Small page
    tlbe.domain = unisim::util::arithmetic::BitField<5,4>().Get( l1desc );
    // level = 2;
    tlbe.pxn = unisim::util::arithmetic::BitField<2,1>().Get( l1desc );
    tlbe.NS = unisim::util::arithmetic::BitField<3,1>().Get( l1desc );

    // Obtain Second level descriptor.
    uint32_t l2descaddr = ((l1desc & 0xfffffc00) | ((mva << 12) >> 22)) & -4;
    {
      bool success;
      if (POLICY::DEBUG) success = static_cast<CPU_IMPL*>(this)->ExternalReadMemory( l2descaddr, erd.data(), 4 );
      else               success = static_cast<CPU_IMPL*>(this)->PhysicalReadMemory( l2descaddr, l2descaddr, erd.data(), 4, 0 );
      if (not success)
        DataAbort(l2descaddr, 0, tlbe.domain, 0, mat_read, DAbort::SyncExternalOnWalk, false,false,false,false,false);
    }
    uint32_t l2desc = erd.Get();
    // Process Second level descriptor.
    if (unisim::util::arithmetic::BitField<0,2>().Get(l2desc) == 0)
      {
        DataAbort(mva, 0, tlbe.domain, 2, mat, DAbort::Translation, false, false, false, false, false);
      }
    tlbe.memattrs = (unisim::util::arithmetic::BitField<10,1>().Get( l2desc ) << 5) | unisim::util::arithmetic::BitField<2,2>().Get( l2desc ); /* S[1] : ?[3] : CB[2]*/
    tlbe.ap = (unisim::util::arithmetic::BitField<9,1>().Get( l2desc ) << 2) | unisim::util::arithmetic::BitField<4,2>().Get( l2desc );
    tlbe.nG = unisim::util::arithmetic::BitField<11,1>().Get( l2desc );
    if (unisim::util::arithmetic::BitField<1,1>().Get(l2desc))
      {
        // Small page (4kB)
        tlbe.memattrs |= (unisim::util::arithmetic::BitField<12,3>().Get( l2desc ) << 2); /* ?[1] : TEX[3] : ?[2] */
        tlbe.xn = unisim::util::arithmetic::BitField<0,1>().Get( l2desc );
        tlbe.lsb = 12;
        tlbe.pa = (l2desc & 0xfffff000) | (mva & 0x00000fff);
      }
    else
      {
        // Large page (64kB)
        tlbe.memattrs |= (unisim::util::arithmetic::BitField<6,3>().Get( l2desc ) << 2); /* ?[1] : TEX[3] : ?[2] */
        tlbe.xn = unisim::util::arithmetic::BitField<15,1>().Get( l2desc );
        tlbe.lsb = 16;
        tlbe.pa = (l2desc & 0xffff0000) | (mva & 0x0000ffff);
      }
  } break;

  case 2: case 3: {
    // Section or Supersection
    tlbe.memattrs = (unisim::util::arithmetic::BitField<16,1>().Get( l1desc ) << 5) | (unisim::util::arithmetic::BitField<12,3>().Get( l1desc ) << 2) | unisim::util::arithmetic::BitField<2,2>().Get( l1desc ); /* S[1] : TEX[3] : CB[2]*/
    tlbe.ap = (unisim::util::arithmetic::BitField<15,1>().Get( l1desc ) << 2) | unisim::util::arithmetic::BitField<10,2>().Get( l1desc );
    tlbe.xn = unisim::util::arithmetic::BitField<4,1>().Get( l1desc );
    tlbe.pxn = unisim::util::arithmetic::BitField<0,1>().Get( l1desc );
    tlbe.nG = unisim::util::arithmetic::BitField<17,1>().Get( l1desc );
    // level = 1;
    tlbe.NS = unisim::util::arithmetic::BitField<19,1>().Get( l1desc );

    if (unisim::util::arithmetic::BitField<18,1>().Get(l1desc))
      {
        // Supersection (16MB)
        tlbe.domain = 0b0000;
        tlbe.lsb = 24;
        if (unisim::util::arithmetic::BitField<20,4>().Get( l1desc ) or unisim::util::arithmetic::BitField<5,4>().Get( l1desc ))
          throw std::logic_error("LPAE not implemented"); /* Large 40-bit extended address */
        tlbe.pa = (l1desc & 0xff000000) | (mva & 0x00ffffff);
      }
    else
      {
        // Section (1MB)
        tlbe.domain = unisim::util::arithmetic::BitField<5,4>().Get( l1desc );
        tlbe.lsb = 20;
        tlbe.pa = (l1desc & 0xfff00000) | (mva & 0x000fffff);
      }
  } break;

  }

  if (tlbe.nG) // Non Global entries refer to the current ASID
    tlbe.asid = GetASID();

  // Try to add entry to TLB
  if (not POLICY::DEBUG)
    tlb.AddTranslation( mva, tlbe );
}

namespace {
  template <unsigned VAL, unsigned Tbits = 32>
  struct Case
  {
    typedef Case<VAL, Tbits> this_type;
    typedef uint32_t tt_type;

    static unsigned const msb = (Tbits-1);
    static tt_type const tt = (((msb >> 2) == VAL) ? (tt_type(1) << msb) : tt_type(0)) | Case<VAL,msb>::tt;

    template <typename RHS> unisim::util::truth_table::LUT<tt_type,(tt&RHS::tt)>
    operator && ( RHS const& ) const { return unisim::util::truth_table::LUT<tt_type,(tt&RHS::tt)>(); }
    template <typename RHS> unisim::util::truth_table::LUT<tt_type,(tt|RHS::tt)>
    operator || ( RHS const& ) const { return unisim::util::truth_table::LUT<tt_type,(tt|RHS::tt)>(); }
    unisim::util::truth_table::LUT<tt_type,(~tt)>
    operator ! () const { return unisim::util::truth_table::LUT<tt_type,(~tt)>(); }
  };

  template <unsigned VAL> struct Case<VAL,1> { static uint32_t const tt = 0; };
}

template <class CPU_IMPL>
template <class POLICY>
void
CPU<CPU_IMPL>::TranslateAddress( AddressDescriptor& loc, bool ispriv, mem_acc_type_t mat, unsigned size )
{
  uint32_t mva = loc.address; /* No FCSE translation in this model*/

  // FirstStageTranslation

  if (arm::sctlr::M.Get( this->SCTLR )) {
    bool ishyp = cpsr.Get(M) == HYPERVISOR_MODE;
    TLB::Entry tlbe;

    // Stage 1 MMU enabled
    if (unlikely(not tlb.GetTranslation<POLICY>( tlbe, mva, GetASID() )))
      TranslationTableWalk<POLICY>( tlbe, mva, mat, size );
    // else {
    //   // Check if hit is coherent
    //   TLB::Entry tlbe_chk;
    //   TranslationTableWalk<QuietAccess>( tlbe_chk, mva, mat, size );
    //   if (tlbe_chk.pa != tlbe.pa)
    //     trap_reporting_import->ReportTrap( *this, "Incoherent TLB access" );
    // }

    /* Permission Check */
    unsigned dac = (mmu.dacr >> (tlbe.domain*2)) & 3;

    if (POLICY::DEBUG) {
      if (POLICY::VERBOSE) {
        logger << DebugInfo;

        logger << "Translation size: 0x" << std::hex << (1 << tlbe.lsb) << std::dec << std::endl;

        logger << "Address Space: ";
        if (tlbe.nG) logger << "asid=" << tlbe.asid;
        else        logger << "global";
        logger << std::endl;

        logger << "Domain access control: " << dac << ", ";
        switch (dac) {
        case 0: logger << "domain fault"; break;
        case 2: logger << "undefined"; break;
        case 1: logger << "permission checking enabled"; break;
        case 3: logger << "permission checking disabled"; break;
        }
        logger << std::endl << "Access permissions: " << tlbe.ap << ", ";
        switch (tlbe.ap) {
        case 0b001: logger << "access only at PL1 or higher"; break;
        case 0b011: logger << "full access"; break;
        case 0b101: logger << "read-only, only at PL1 or higher"; break;
        case 0b111: logger << "read-only at any privilege level"; break;
        case 0b000: logger << "all accesses generate Permission faults"; break;
        case 0b010: logger << "writes at PL0 generate Permission faults"; break;
        case 0b100: logger << "undefined"; break;
        case 0b110: logger << "read-only at any privilege level (deprecated)"; break;
        }
        logger << std::endl << "Execution permissions"
               << ": XN=" << (tlbe.xn ? "true" : "false")
               << ", PXN=" << (tlbe.pxn ? "true" : "false")
               << EndDebugInfo;
      }
    } else {
      if (unlikely((dac & 1) == 0))
        DataAbort( mva, 0, tlbe.domain, (tlbe.lsb <= 16 ? 2 : 1), mat, DAbort::Domain, false, false, false, false, false );

      unisim::util::truth_table::InBit<uint32_t,1> const P;
      unisim::util::truth_table::InBit<uint32_t,0> const W;
      Case<0b000> const AP000;
      Case<0b001> const AP001;
      Case<0b010> const AP010;
      //Case<0b011> const AP011;
      Case<0b100> const AP100;
      Case<0b101> const AP101;
      Case<0b110> const AP110;
      Case<0b111> const AP111;

      unsigned sel = (tlbe.ap << 2) | (unsigned(ispriv) << 1) | (unsigned(mat == mat_write) << 0);

      uint32_t const perm_table =
        ((AP000) or
         (AP001 and not P) or
         (AP010 and not P and W) or
         /* AP011 */
         (AP100) or
         (AP101 and (not P or W)) or
         (AP110 and W) or
         (AP111 and W)).tt;

      uint32_t abort = uint32_t( (dac & 2) == 0 ) & (perm_table >> sel);
      /* TODO: check that ispriv is correct for pxn (only PL1 is concerned ?) */
      uint32_t xabort = uint32_t(mat == mat_exec) & (tlbe.xn | (tlbe.pxn & ispriv));

      /* TODO: Long descriptor format + (Hardware flags ? really deprecated ? at least sw AFE ?) */
      if (unlikely((abort | xabort) & 1))
        DataAbort( mva, 0, tlbe.domain, (tlbe.lsb <= 16 ? 2 : 1), mat, DAbort::Permission, ishyp, false, false, false, false );
    }

    loc.address = tlbe.pa;
    loc.attributes = mmu.attr_cache[tlbe.memattrs];

    return;
  }

  // Address unchanged
  static struct AttrCache
  {
    uint16_t table[2][2];
    AttrCache()
      : table()
    {
      for (int isfetch = 0; isfetch <= 1; ++isfetch)
        {
          for (int icenable = 0; icenable <= 1; ++icenable)
            {
              uint32_t attributes = 0;
              if (isfetch)
                {
                  MemAttrs::type().Set( attributes, MemAttrs::Normal);
                  if (icenable)
                    {
                      MemAttrs::innerattrs().Set( attributes, MemAttrs::WriteThrough );
                      MemAttrs::innerhints().Set( attributes, MemAttrs::ReadAllocate );
                      MemAttrs::outerattrs().Set( attributes, MemAttrs::WriteThrough );
                      MemAttrs::outerhints().Set( attributes, MemAttrs::ReadAllocate );
                    }
                }
              else
                {
                  MemAttrs::type().Set( attributes, MemAttrs::StronglyOrdered );
                  MemAttrs::shareable().Set( attributes, 1 );
                  MemAttrs::outershareable().Set( attributes, 1 );
                }
              table[isfetch][icenable] = attributes;
            }
        }
    }

  } attrcache;

  loc.attributes = attrcache.table[(mat != mat_exec)][(arm::sctlr::I.Get(SCTLR) != 0)];
}

#define CP15ENCODE( CRN, OPC1, CRM, OPC2 ) ((OPC1 << 12) | (CRN << 8) | (CRM << 4) | (OPC2 << 0))

/** Get the Internal representation of the CP15 Register
 *
 * @param crn     the "crn" field of the instruction code
 * @param opcode1 the "opcode1" field of the instruction code
 * @param crm     the "crm" field of the instruction code
 * @param opcode2 the "opcode2" field of the instruction code
 * @return        an internal CP15Reg
 */
template <class CPU_IMPL>
typename CPU<CPU_IMPL>::CP15Reg*
CPU<CPU_IMPL>::CP15GetRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 )
{
  switch (CP15ENCODE( crn, opcode1, crm, opcode2 ))
    {
    case CP15ENCODE( 0, 0, 0, 1 ):
      {
        static struct : public CP15Reg
        {
          void Describe(uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "CTR, Cache Type Register"; }
          uint32_t Read(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& _cpu) const override
          {
            /*        FORMAT          CWG         ERG      DminLine        L1Ip       IminLine */
            /*         ARMv7        8 words     8 words     8 words        PIPT        8 words */
            return (0b100 << 29) | (3 << 24) | (3 << 20) | (3 << 16) | (0b11 << 14) | (3 <<  0);
          }
        } x;
        return &x;
      } break;

    case CP15ENCODE( 0, 1, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          void Describe(uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "CCSIDR, Cache Size ID Registers"; }
          uint32_t Read(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu) const override
          {
            switch (cpu.csselr)
              {
                /*              LNSZ      ASSOC       NUMSETS        POLICY      */
              case 0:  return (1 << 0) | (3 << 3) | ( 255 << 13) | (0b0110 << 28); /* L1 dcache */
              case 1:  return (1 << 0) | (3 << 3) | ( 255 << 13) | (0b0100 << 28); /* L1 icache */
              case 2:  return (2 << 0) | (7 << 3) | (1023 << 13) | (0b0110 << 28); /* L2 ucache */
              default: return 0;
              }
          }
        } x;
        return &x;
      } break;

    case CP15ENCODE( 0, 1, 0, 1 ):
      {
        static struct : public CP15Reg
        {
          void Describe(uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "CLIDR, Cache Level ID Register"; }
          uint32_t Read(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& _cpu) const override
          {
            uint32_t
              LoUU =   0b010, /* Level of Unification Uniprocessor  */
              LoC =    0b010, /* Level of Coherency */
              LoUIS =  0b010, /* Level of Unification Inner Shareable */
              Ctype2 = 0b100, /* Level2 => unified */
              Ctype1 = 0b011; /* Level1 => Separate instruction and data caches */
            return (LoUU << 27) | (LoC << 24) | (LoUIS << 21) | (Ctype2 << 3) | (Ctype1 << 0);
          }
        } x;
        return &x;
      } break;

    case CP15ENCODE( 0, 0, 1, 4 ):
      {
        static struct : public CP15Reg
        {
          void Describe(uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "ID_MMFR0, Memory Model Feature Register 0"; }
          uint32_t Read(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& _cpu) const override { return 0x3; /* vmsav7 */ }
        } x;
        return &x;
      } break;

    case CP15ENCODE( 0, 2, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          void Describe(uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "CSSELR, Cache Size Selection Register"; }
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu, uint32_t value) const override{ cpu.csselr = value; }
          uint32_t Read(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu) const override { return cpu.csselr; }
        } x;
        return &x;
      } break;

    case CP15ENCODE( 1, 0, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          void Describe(uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "SCTLR, System Control Register"; }
          /* TODO: handle SBO(DGP=0x00050078U) and SBZ(DGP=0xfffa0c00U)... */
          uint32_t Read(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu) const override { return cpu.SCTLR; }
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu, uint32_t value) const override
          {
            /* Checking bits values */
            sctlr::HA.Set( value, 0 ); // No Hardware Access flag management
            if (sctlr::AFE.Get( value ))
              cpu.Stop(-1); // Software Access Flag management not supported

            uint32_t diff = cpu.SCTLR ^ value;

            cpu.SCTLR = value;

            // Side effects
            if (sctlr::TRE.Get(diff))
              cpu.mmu.refresh_attr_cache(sctlr::TRE.Get(value));
          }
        } x;
        return &x;
      } break;

      /*******************************************
       * Memory protection and control registers *
       *******************************************/
    case CP15ENCODE( 2, 0, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          void Describe(uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "TTBR0, Translation Table Base Register 0"; }
          /* TODO: handle SBZ(DGP=0x00003fffUL)... */
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu, uint32_t value) const override{ cpu.mmu.ttbr0 = value; }
          uint32_t Read(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu) const override { return cpu.mmu.ttbr0; }
        } x;
        return &x;
      } break;

    case CP15ENCODE( 2, 0, 0, 1 ):
      {
        static struct : public CP15Reg
        {
          void Describe(uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "TTBR1, Translation Table Base Register 1"; }
          /* TODO: handle SBZ(DGP=0x00003fffUL)... */
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu, uint32_t value) const override{ cpu.mmu.ttbr1 = value; }
          uint32_t Read(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu) const override { return cpu.mmu.ttbr1; }
        } x;
        return &x;
      } break;

    case CP15ENCODE( 2, 0, 0, 2 ):
      {
        static struct : public CP15Reg
        {
          void Describe(uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "TTBCR, Translation Table Base Control Register"; }
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu, uint32_t value) const override{ cpu.mmu.ttbcr = value; }
          uint32_t Read(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu) const override { return cpu.mmu.ttbcr; }
        } x;
        return &x;
      } break;


    case CP15ENCODE( 3, 0, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          void Describe(uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "DACR, Domain Access Control Register"; }
          uint32_t Read(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu) const override { return cpu.mmu.dacr; }
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu, uint32_t value) const override
          {
            cpu.mmu.dacr = value;
            if (cpu.verbose)
              cpu.logger << DebugInfo << "DACR <- " << std::hex << value << std::dec << EndDebugInfo;
          }
        } x;
        return &x;
      } break;

      /*********************************
       * Memory system fault registers *
       *********************************/
    case CP15ENCODE( 5, 0, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          void Describe(uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "DFSR, Data Fault Status Register"; }
          static uint32_t& reg( CPU_IMPL& cpu ) { return cpu.DFSR; }
          uint32_t Read(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu) const override { return reg( cpu ); }
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu, uint32_t value) const override{ reg( cpu ) = value; }
        } x;
        return &x;
      } break;

    case CP15ENCODE( 5, 0, 0, 1 ):
      {
        static struct : public CP15Reg
        {
          void Describe(uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "IFSR, Instruction Fault Status Register"; }
          static uint32_t& reg( CPU_IMPL& cpu ) { return cpu.IFSR; }
          uint32_t Read(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu) const override { return reg( cpu ); }
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu, uint32_t value) const override{ reg( cpu ) = value; }
        } x;
        return &x;
      } break;

    case CP15ENCODE( 6, 0, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          void Describe(uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "DFAR, Data Fault Status Register"; }
          static uint32_t& reg( CPU_IMPL& cpu ) { return cpu.DFAR; }
          uint32_t Read(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu) const override { return reg( cpu ); }
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu, uint32_t value) const override{ reg( cpu ) = value; }
        } x;
        return &x;
      } break;

    case CP15ENCODE( 6, 0, 0, 2 ):
      {
        static struct : public CP15Reg
        {
          void Describe(uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "IFAR, Instruction Fault Status Register"; }
          static uint32_t& reg( CPU_IMPL& cpu ) { return cpu.IFAR; }
          uint32_t Read(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu) const override { return reg( cpu ); }
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu, uint32_t value) const override{ reg( cpu ) = value; }
        } x;
        return &x;
      } break;

      /***************************************************************
       * Cache maintenance, address translation, and other functions *
       ***************************************************************/
    case CP15ENCODE( 7, 0, 5, 0 ):
      {
        static struct : public CP15Reg
        {
          void Describe(uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "ICIALLU, Invalidate all instruction caches to PoU"; }
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu, uint32_t value) const override{
            /* No cache, basically nothing to do */
            //cpu.logger << DebugWarning << "ICIALLU <- " << std::hex << value << std::dec << EndDebugWarning;
          }
        } x;
        return &x;
      } break;

    case CP15ENCODE( 7, 0, 5, 1 ):
      {
        static struct : public CP15Reg
        {
          void Describe(uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "ICIMVAU, Clean data* cache line by MVA to PoU"; }
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu, uint32_t value) const override{
            /* No cache, basically nothing to do */
            //cpu.logger << DebugWarning << "ICIMVAU <- " << std::hex << value << std::dec << EndDebugWarning;
          }
        } x;
        return &x;
      } break;

    case CP15ENCODE( 7, 0, 5, 6 ):
      {
        static struct : public CP15Reg
        {
          void Describe(uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "BPIALL, Invalidate all branch predictors"; }
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu, uint32_t value) const override{
            /* No branch predictor, basically nothing to do */
            //cpu.logger << DebugWarning << "BPIALL <- " << std::hex << value << std::dec << EndDebugWarning;
          }
        } x;
        return &x;
      } break;

    case CP15ENCODE( 7, 0, 6, 1 ):
      {
        static struct : public CP15Reg
        {
          void Describe(uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "DCIMVAC, Invalidate data* cache line by MVA to PoC"; }
          void Write( CPU_IMPL& cpu, uint32_t value )
          {
            /* No cache, basically nothing to do */
            //cpu.logger << DebugWarning << "DCIMVAC <- " << std::hex << value << std::dec << EndDebugWarning;
          }
        } x;
        return &x;
      } break;

    case CP15ENCODE( 7, 0, 6, 2 ):
      {
        static struct : public CP15Reg
        {
          void Describe(uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "DCISW, Invalidate data* cache line by set/way"; }
          void Write( CPU_IMPL& cpu, uint32_t value )
          {
            /* No cache, basically nothing to do */
            //cpu.logger << DebugWarning << "CDISW <- " << std::hex << value << std::dec << EndDebugWarning;
          }
        } x;
        return &x;
      } break;

    case CP15ENCODE( 7, 0, 10, 1 ):
      {
        static struct : public CP15Reg
        {
          void Describe(uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "DCCMVAC, Clean data* cache line by MVA to PoC"; }
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu, uint32_t value) const override{
            /* No cache, basically nothing to do */
            //cpu.logger << DebugWarning << "DCCMVAC <- " << std::hex << value << std::dec << EndDebugWarning;
          }
        } x;
        return &x;
      } break;

    case CP15ENCODE( 7, 0, 11, 1 ):
      {
        static struct : public CP15Reg
        {
          void Describe(uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "DCCMVAU, Clean data* cache line by MVA to PoU"; }
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu, uint32_t value) const override{
            /* No cache, basically nothing to do */
            //cpu.logger << DebugWarning << "DCCMVAU <- " << std::hex << value << std::dec << EndDebugWarning;
          }
        } x;
        return &x;
      } break;

    case CP15ENCODE( 7, 0, 14, 1 ):
      {
        static struct : public CP15Reg
        {
          void Describe(uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "DCCIMVAC, Clean and invalidate data cache line by MVA to PoC"; }
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu, uint32_t value) const override{
            /* No cache, basically nothing to do */
            //cpu.logger << DebugWarning << "DCCIMVAC <- " << std::hex << value << std::dec << EndDebugWarning;
          }
        } x;
        return &x;
      } break;

    case CP15ENCODE( 7, 0, 14, 2 ):
      {
        static struct : public CP15Reg
        {
          void Describe(uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "DCCISW, Clean and invalidate [d|u]cache line by set/way"; }
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu, uint32_t value) const override{
            /* No cache, basically nothing to do */
            //cpu.logger << DebugWarning << "DCCISW <- " << std::hex << value << std::dec << EndDebugWarning;
          }
        } x;
        return &x;
      } break;

      /******************************
       * TLB maintenance operations *
       ******************************/
    case CP15ENCODE( 8, 0, 7, 0 ):
      {
        static struct : public CP15Reg
        {
          void Describe(uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "TLBIALL, invalidate unified TLB"; }
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu, uint32_t value) const override
          {
            if (cpu.verbose)
              cpu.logger << DebugInfo << "TLBIALL" << EndDebugInfo;
            cpu.tlb.InvalidateAll();
          }
        } x;
        return &x;
      } break;

    case CP15ENCODE( 8, 0, 7, 1 ):
      {
        static struct : public CP15Reg
        {
          void Describe(uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "TLBIMVA, invalidate unified TLB entry by MVA and ASID"; }
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu, uint32_t value) const override
          {
            if (cpu.verbose)
              cpu.logger << DebugInfo << "TLBIMVA(0x" << std::hex << value << std::dec << ")" << EndDebugInfo;

            for (TLB::Iterator itr(cpu.tlb); itr.Next();) {
              if (itr.MatchMVA( value ) and (itr.IsGlobal() or itr.MatchASID( value )))
                itr.Invalidate();
            }
          }
        } x;
        return &x;
      } break;

    case CP15ENCODE( 8, 0, 7, 2 ):
      {
        static struct : public CP15Reg
        {
          void Describe(uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "TLBIASID, invalidate unified TLB by ASID match"; }
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu, uint32_t value) const override
          {
            if (cpu.verbose)
              cpu.logger << DebugInfo << "TLBIASID(0x" << std::hex << value << std::dec << ")" << EndDebugInfo;

            for (TLB::Iterator itr(cpu.tlb); itr.Next();) {
              if (not itr.IsGlobal() and itr.MatchASID( value ))
                itr.Invalidate();
            }
          }
        } x;
        return &x;
      } break;

      /**********************************************
       * Memory remapping and TLB control registers *
       **********************************************/
    case CP15ENCODE( 10, 0, 2, 0 ):
      {
        static struct : public CP15Reg
        {
          void Describe(uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "PRRR, Primary Region Remap Register"; }
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu, uint32_t value) const override
          {
            MMU& mmu( cpu.mmu );
            mmu.prrr = value;
            mmu.refresh_attr_cache(sctlr::TRE.Get(cpu.SCTLR));
          }
          uint32_t Read(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu) const override { return cpu.mmu.prrr; }
        } x;
        return &x;
      } break;

    case CP15ENCODE( 10, 0, 2, 1 ):
      {
        static struct : public CP15Reg
        {
          void Describe(uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "NMRR, Normal Memory Remap Register"; }
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu, uint32_t value) const override
          {
            MMU& mmu( cpu.mmu );
            mmu.nmrr = value;
            mmu.refresh_attr_cache(sctlr::TRE.Get(cpu.SCTLR));
          }
          uint32_t Read(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu) const override { return cpu.mmu.nmrr; }
        } x;
        return &x;
      } break;

      /***********************************/
      /* Context and thread ID registers */
      /***********************************/

    case CP15ENCODE( 13, 0, 0, 3 ):
      {
        static struct : public CP15Reg
        {
          void Describe(uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const override { sink << "TPIDRURO, User Read-Only Thread ID Register (with linux_os emulation)"; }
          void CheckPermissions(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu, bool w) const override { return cpu.RequiresPL(w ? 1 : 0); }
          /* When using linux os emulation, this register overrides the base one */
          uint32_t Read(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu) const override
          {
            return cpu.linux_os_import ? cpu.MemRead32( 0xffff0ff0 ) : cpu.TPIDRURO;
          }
          void Write(uint8_t, uint8_t, uint8_t, uint8_t, CPU_IMPL& cpu, uint32_t val) const override
          {
            if (cpu.linux_os_import) { struct Bad {}; throw Bad(); }
            cpu.TPIDRURO = val;
          }
        } x;
        return &x;
      } break;

    }

  // Fall back to parent cpu CP15 registers
  return PCPU::CP15GetRegister( crn, opcode1, crm, opcode2 );
}

#undef CP15ENCODE

} // end of namespace vmsav7
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
