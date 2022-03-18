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
#include <unisim/component/cxx/processor/arm/isa/arm64/disasm.hh>
#include <iomanip>
#include <ostream>
#include <cmath>
#include <ieee754.h>

void
Runner::step_instruction()
{
  uint64_t insn_addr = this->current_insn_addr = this->next_insn_addr;
    
  asm volatile ("operation_fetch:");
  uint32_t code = MemRead32(insn_addr);
    
  asm volatile ("operation_decode:");
  std::unique_ptr<Operation> op = decode(insn_addr, code);
    
  this->next_insn_addr += 4;
    
  asm volatile ("operation_execute:");
  op->execute( *this );
}

void
Runner::run(Interface::testcode_t testcode, uint64_t* data)
{
  U64 const magic_return_address = 0xdeadc0dedeadc0deULL;

  SetGZR(0, uint64_t(data));
  uint64_t sim_stack[32];
  SetGSR(31, reinterpret_cast<uint64_t>( &sim_stack[0] ) + sizeof sim_stack);
  SetGZR(30, magic_return_address);
  BranchTo( reinterpret_cast<uint64_t>(testcode), B_CALL );
    
  for (int ttl = 100; GetNPC() != magic_return_address;)
    {
      if (--ttl < 0) { struct Issue {}; throw Issue(); }
      step_instruction();
    }
}
  
namespace { template <unsigned W> struct HEX { friend std::ostream& operator << (std::ostream& os, HEX&&) { return (os << std::hex << std::setfill('0') << std::setw(W)); } }; }

void
Runner::UndefinedInstruction( Runner::Operation const* insn )
{
  std::cerr << "Undefined Instruction @" << HEX<16>() << insn->GetAddr() << " (<" << insn->GetName() << ">)\n";
  insn->disasm(*this,std::cerr << HEX<8>() << insn->GetEncoding() << ":\t");
  std::cerr << std::endl;
  UndefinedInstruction();
}

void Runner::dont( char const* reason ) { throw std::runtime_error(reason); }
  
Runner::SysReg const*
Runner::GetSystemRegister(int op0, int op1, int crn, int crm, int op2)
{
  // Need to support NZCV ^^
  if (op0==3 and op1==3 and crn==4 and crm==2 and op2==0)
    {
      static struct : public SysReg {
        void DisasmWrite(int, int, int, int, int, int rt, std::ostream& s) const override
        { s << "msr\tNZCV, " << unisim::component::cxx::processor::arm::isa::arm64::DisasmGZXR(rt); }
        void DisasmRead(int, int, int, int, int, int rt, std::ostream& s) const override
        { s << "mrs\t" << unisim::component::cxx::processor::arm::isa::arm64::DisasmGZXR(rt) << ", NZCV"; }
        U64 Read(int, int, int, int, int, Runner& cpu) const override { return U32(cpu.nzcv << 28); }
        void Write(int, int, int, int, int, Runner& cpu, U64 value) const override { cpu.nzcv = U32(value) >> 28; }
      } x; return &x;
    }
  else
    dont("system");
  return 0;
}

Runner::U8
Runner::GetTVU8(unsigned reg0, unsigned elements, unsigned regs, U8 index, U8 oob_value)
{
  unsigned e = index % elements, r = index / elements;
  return r < regs ? GetVU8((reg0+r)%32, e) : oob_value;
}

float clearsignaling( float value )
{
  ieee754_float u;
  u.f = value;
  u.ieee_nan.quiet_nan = 1;
  return u.f;
}

double clearsignaling( double value )
{
  ieee754_double u;
  u.d = value;
  u.ieee_nan.quiet_nan = 1;
  return u.d;
}
