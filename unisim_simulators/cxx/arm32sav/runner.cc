/*
 *  Copyright (c) 2019-2020,
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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#include <runner.hh>
#include <unisim/component/cxx/processor/arm/cpu.tcc>
#include <unisim/component/cxx/processor/arm/execute.hh>
#include <iomanip>
#include <ostream>
#include <cmath>

Runner::Runner( char const* name )
  : unisim::kernel::Object( name, 0 )
  , CPU(name,0)
{
  cpsr.Set(unisim::component::cxx::processor::arm::M, this->USER_MODE);
}

Runner::~Runner()
{
}

unisim::util::endian::endian_type
Runner::endianess()
{
  using namespace unisim::util::endian;
  return cpsr.Get( unisim::component::cxx::processor::arm::E ) ? E_BIG_ENDIAN : E_LITTLE_ENDIAN;
}

void
Runner::step_instruction()
{
  // Instruction boundary next_insn_addr becomes current_insn_addr
  uint32_t insn_addr = this->current_insn_addr = this->next_insn_addr, insn_length = 0;
  
  // Fetch (always little endian in armv7)
  uint32_t insn = *reinterpret_cast<uint32_t const*>(insn_addr);
  
  // Instruction Fetch Decode and Execution
  if (cpsr.Get( unisim::component::cxx::processor::arm::T ))
    {
      /* Thumb state */
      Thumb2::Operation* op = thumb2iset.decode(insn_addr, insn);
        
      /* update PC register value before execution */
      insn_length = op->GetLength() / 8;
      this->gpr[15] = insn_addr + 4;
      this->next_insn_addr = insn_addr + insn_length;
        
      /* Execute instruction */
      asm volatile( "thumb_operation_execute:" );
      if (unisim::component::cxx::processor::arm::CheckCondition(*this, itcond()))
        op->execute( *this );

      this->ITAdvance();
    }
  else
    {
      /* Arm32 state */
      Arm32::Operation* op = arm32iset.decode(insn_addr, insn);
    
      /* update PC register value before execution */
      insn_length = op->GetLength() / 8;
      this->gpr[15] = insn_addr + 8;
      this->next_insn_addr = insn_addr + insn_length;
        
      /* Execute instruction */
      asm volatile( "arm32_operation_execute:" );
      if (unisim::component::cxx::processor::arm::CheckCondition(*this, insn >> 28))
        op->execute( *this );
    }
}

void Runner::run(Interface::testcode_t testcode, uint32_t* data)
{
  uint32_t const magic_return_address = 0xfeb0d0d0;

  SetGPR(0, uintptr_t(data));
  uint32_t sim_stack[32];
  SetGPR(13, reinterpret_cast<uintptr_t>( &sim_stack[0] ) + sizeof sim_stack);
  SetGPR(14, magic_return_address);
  BranchExchange( reinterpret_cast<uintptr_t>(testcode), B_CALL );
    
  for (int ttl = 100; GetNIA() != magic_return_address;)
    {
      if (--ttl < 0) { struct Issue {}; throw Issue(); }
      step_instruction();
    }
}
  
// namespace { template <unsigned W> struct HEX { friend std::ostream& operator << (std::ostream& os, HEX&&) { return (os << std::hex << std::setfill('0') << std::setw(W)); } }; }

// void
// Runner::UndefinedInstruction( Runner::Operation const* insn )
// {
//   std::cerr << "Undefined Instruction @" << HEX<16>() << insn->GetAddr() << " (<" << insn->GetName() << ">)\n";
//   insn->disasm(*this,std::cerr << HEX<8>() << insn->GetEncoding() << ":\t");
//   std::cerr << std::endl;
//   UndefinedInstruction();
// }

void Runner::dont( char const* reason ) { throw std::runtime_error(reason); }
  
// Runner::SysReg const*
// Runner::GetSystemRegister(int op0, int op1, int crn, int crm, int op2)
// {
//   // Need to support NZCV ^^
//   if (op0==3 and op1==3 and crn==4 and crm==2 and op2==0)
//     {
//       static struct : public SysReg {
//         void DisasmWrite(int, int, int, int, int, int rt, std::ostream& s) const override
//         { s << "msr\tNZCV, " << unisim::component::cxx::processor::arm::isa::arm32::DisasmGZXR(rt); }
//         void DisasmRead(int, int, int, int, int, int rt, std::ostream& s) const override
//         { s << "mrs\t" << unisim::component::cxx::processor::arm::isa::arm32::DisasmGZXR(rt) << ", NZCV"; }
//         U32 Read(int, int, int, int, int, Runner& cpu) const override { return U32(cpu.nzcv << 28); }
//         void Write(int, int, int, int, int, Runner& cpu, U32 value) const override { cpu.nzcv = U32(value) >> 28; }
//       } x; return &x;
//     }
//   else
//     dont("system");
//   return 0;
// }
