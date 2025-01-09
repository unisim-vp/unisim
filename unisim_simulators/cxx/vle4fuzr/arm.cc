/*
 *  Copyright (c) 2019,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 * FUZR RENAULT CEA FILE
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr), Gilles Mouchard <gilles.mouchard@cea.fr>
 */

#include "arm.hh"
#include <unisim/component/cxx/processor/arm/isa/arm32/arm32.tcc>
#include <unisim/component/cxx/processor/arm/isa/thumb2/thumb.tcc>
#include <unisim/component/cxx/processor/arm/cpu.tcc>
#include <unisim/component/cxx/processor/arm/cfg/aarch32/aarch32.hh>
#include <unisim/component/cxx/processor/opcache/opcache.tcc>
#include <memory>

ArmProcessor::ArmProcessor( char const* name, bool is_thumb )
  : unisim::kernel::Object( name, 0 )
  , Processor()
  , CPU( name, 0 )
{
  cpsr.Set( unisim::component::cxx::processor::arm::T, uint32_t(is_thumb) );
}

ArmProcessor::~ArmProcessor()
{
}

Processor::RegView const*
ArmProcessor::get_reg(char const* id, uintptr_t size, int regid)
{
  if (regname("apsr",id,size))
    {
      static struct : public RegView
      {
        static uint32_t apsr_mask() { return 0xf80f0000; }
        virtual void write( Processor& proc, int id, uint64_t value ) const
        { Self(proc).SetCPSR(uint32_t(value), apsr_mask()); }
        virtual void read( Processor& proc, int id, uint64_t* value ) const
        { *value = Self(proc).GetCPSR() & apsr_mask(); }
      } _;
      return &_;
    }

  if (regname("gpr",id,size))
    {
      if ((unsigned)regid < 15)
        {
          static struct : public RegView
          {
            void write( Processor& proc, int id, uint64_t value ) const { Self(proc).SetGPR(id, value); }
            void read( Processor& proc, int id, uint64_t* value ) const { *value = Self(proc).GetGPR(id); }
          } _;
          return &_;
        }
      else if (regid == 15)
        {
          static struct : public RegView
          {
            void write( Processor& proc, int id, uint64_t value ) const { Self(proc).DebugBranch(value); }
            void read( Processor& proc, int id, uint64_t* value ) const { *value = Self(proc).GetCIA(); }
          } _;
          return &_;
        }
    }

  std::cerr << "Register '" << std::string(id,size) << "' not found.\n";
  return 0;
}


template <typename Decoder>
void
ArmProcessor::Step( Decoder& decoder )
{
  typedef typename Decoder::operation_type Operation;
  typedef typename Decoder::code_type      CodeType;
  bool const is_thumb = Operation::minsize == 16;

  // Instruction boundary next_insn_addr becomes current_insn_addr
  uint32_t insn_addr = this->current_insn_addr = this->next_insn_addr;

  // Fetch
  CodeType insn = ReadInsn(insn_addr);

  // Decode
  Operation* op = decoder.Decode(insn_addr, insn);
  SetCurrent(op);

  unisim::component::cxx::processor::arm::cfg::aarch32::ComputeBranchInfo(op);

  // Monitor
  if (this->disasm)
    {
      op->disasm(*this, std::cerr << std::hex << insn_addr << ": (" << (is_thumb?'T':'A') << ") " );
      std::cerr << std::endl;
    }

  uint32_t insn_length = op->GetLength() / 8;
  insn_hooks(insn_addr, insn_length);

  // Execute
  this->gpr[15] = insn_addr + (is_thumb ? 4 : 8);
  this->next_insn_addr = insn_addr + insn_length;

  if (CheckCondition(*this, is_thumb ? itcond() : op->GetEncoding() >> 28))
    op->execute( *this );

  if (is_thumb)
    this->ITAdvance();

  if (debug_branch != uint64_t(-1))
    {
      this->bblock = true;
      BranchExchange(debug_branch, B_DBG);
      DebugBranch(-1);
    }
  else
    this->bblock = (op->branch.has_branch());
}

template <typename Insn>
void
ArmProcessor::Disasm( Insn* op )
{
  if (op)
    {
      std::ostringstream buf;
      op->disasm(*this, buf);
      std::cerr << std::endl;
      asmbuf = buf.str();
    }
  else
    asmbuf = "?";
}

char const*
ArmProcessor::get_asm()
{
  if (cpsr.Get( unisim::component::cxx::processor::arm::T ))
    Disasm(current_thumb_operation);
  else
    Disasm(current_arm32_operation);

  return asmbuf.c_str();
}

void
ArmProcessor::run( uint64_t begin, uint64_t until, uint64_t count )
{
  this->Branch(begin, B_DBG);
  this->bblock = true;

  do
    {
      if (cpsr.Get( unisim::component::cxx::processor::arm::T ))
        Step(thumb_decoder);
      else
        Step(arm32_decoder);
    }
  while (next_insn_addr != until and --count != 0);
}

uint32_t
ArmProcessor::ReadInsn(uint32_t address)
{
  uint64_t value = 0;
  PhysicalFetchMemory( address, 4, 0, &value );
  return value;
}

void
ArmProcessor::UndefinedInstruction( ArmProcessor::AOperation* insn )
{
  insn->disasm(*this, std::cerr << "Undefined instruction @" << std::hex << current_insn_addr << std::dec << ": ");
  std::cerr << std::endl;
  abort("ProcessorException('undefined instruction')");
}

void
ArmProcessor::UndefinedInstruction( ArmProcessor::TOperation* insn )
{
  insn->disasm(*this, std::cerr << "Undefined instruction @" << std::hex << current_insn_addr << std::dec << ": ");
  std::cerr << std::endl;
  abort("ProcessorException('undefined instruction')");
}

void
ArmProcessor::PerformUWriteAccess( uint32_t addr, uint32_t size, uint32_t value )
{
  uint32_t const lo_mask = size - 1;
  if (unlikely((lo_mask > 3) or (size & lo_mask))) { struct BadSize {}; throw BadSize(); }
  uint32_t misalignment = addr & lo_mask;

  if (unlikely(misalignment and not unisim::component::cxx::processor::arm::sctlr::A.Get( this->SCTLR )))
    {
      uint32_t eaddr = addr;
      if (GetEndianness() == unisim::util::endian::E_BIG_ENDIAN) {
        for (unsigned byte = size; --byte < size; ++eaddr)
          PerformWriteAccess( eaddr, 1, (value >> (8*byte)) & 0xff );
      } else {
        for (unsigned byte = 0; byte < size; ++byte, ++eaddr)
          PerformWriteAccess( eaddr, 1, (value >> (8*byte)) & 0xff );
      }
      return;
    }
  else
    PerformWriteAccess( addr, size, value );
}

void
ArmProcessor::PerformWriteAccess( uint32_t addr, uint32_t size, uint32_t value )
{
  uint32_t const lo_mask = size - 1;
  if (unlikely((lo_mask > 3) or (size & lo_mask))) { struct BadSize {}; throw BadSize(); }
  uint32_t misalignment = addr & lo_mask;

  if (unlikely(misalignment))
    MemoryException(1/*Write*/, addr, "misalignment");

  unsigned endianness = lo_mask*(GetEndianness() == unisim::util::endian::E_BIG_ENDIAN);
  PhysicalWriteMemory( addr, size, endianness, value );
}

uint32_t
ArmProcessor::PerformUReadAccess( uint32_t addr, uint32_t size )
{
  uint32_t const lo_mask = size - 1;
  if (unlikely((lo_mask > 3) or (size & lo_mask))) { struct BadSize {}; throw BadSize(); }
  uint32_t misalignment = addr & lo_mask;

  if (unlikely(misalignment and not unisim::component::cxx::processor::arm::sctlr::A.Get( this->SCTLR )))
    {
      uint32_t result = 0;
      if (GetEndianness() == unisim::util::endian::E_BIG_ENDIAN) {
        for (unsigned byte = 0; byte < size; ++byte)
          result = (result << 8) | PerformReadAccess( addr + byte, 1 );
      } else {
        for (unsigned byte = size; --byte < size;)
          result = (result << 8) | PerformReadAccess( addr + byte, 1 );
      }
      return result;
    }
  else
    return PerformReadAccess( addr, size );
}

uint32_t
ArmProcessor::PerformReadAccess( uint32_t addr, uint32_t size )
{
  uint32_t const lo_mask = size - 1;
  if (unlikely((lo_mask > 3) or (size & lo_mask))) { struct BadSize {}; throw BadSize(); }
  uint32_t misalignment = addr & lo_mask;

  if (unlikely(misalignment))
    MemoryException(0/*Read*/, addr, "misalignment");

  uint64_t value = 0;
  unsigned endianness = lo_mask*(GetEndianness() == unisim::util::endian::E_BIG_ENDIAN);
  PhysicalReadMemory(addr, size, endianness, &value);

  return value;
}

void
ArmProcessor::CallSupervisor( uint32_t imm )
{
  syscall_hooks( this->current_insn_addr, imm );
}

void
ArmProcessor::UnpredictableInsnBehaviour()
{
  abort("Unpredictable()");
}

void
ArmProcessor::FPTrap( unsigned fpx )
{
  std::ostringstream buf;
  buf << "ProcessorException('floating-point'," << std::dec << fpx << ")";
  abort(buf.str());
}

void
ArmProcessor::BKPT( int bkpt )
{
  std::ostringstream buf;
  buf << "ProcessorException('breakpoint'," << std::dec << bkpt << ")";
  abort(buf.str());
}

