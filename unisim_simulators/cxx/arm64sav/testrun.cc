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

#include <testrun.hh>
#include <unisim/component/cxx/processor/arm/isa_arm64.tcc>
#include <unisim/util/endian/endian.hh>
#include <ostream>
#include <cmath>

namespace test
{
  void
  Arch::step_instruction()
  {
    static Decoder decoder;
    uint64_t insn_addr = this->current_insn_addr = this->next_insn_addr;
    
    asm volatile ("operation_fetch:");
    uint32_t code = MemRead32(insn_addr);
    
    asm volatile ("operation_decode:");
    Operation* op = current_instruction = decoder.Decode( insn_addr, code );
    if (not op) { struct BadWolf {}; throw BadWolf(); }
    
    this->next_insn_addr += 4;
    
    asm volatile ("operation_execute:");
    op->execute( *this );
  }

  void
  Arch::run(review::Interface::testcode_t testcode, uint64_t* data)
  {
    U64 const magic_return_address = 0xdeadc0dedeadc0deULL;

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

  void
  Arch::UndefinedInstruction( Arch::Operation const* insn )
  {
    insn->disasm(*this,std::cerr << "Exception (Undefined ): ");
    std::cerr << std::endl;
    UndefinedInstruction();
  }

  void Arch::dont( char const* reason ) { throw std::runtime_error(reason); }
  
  Arch::SysReg const*
  Arch::GetSystemRegister(int op0, int op1, int crn, int crm, int op2)
    {
      // Need to support NZCV ^^
      if (op0==3 and op1==3 and crn==4 and crm==2 and op2==0)
        {
          static struct : public SysReg {
            void DisasmWrite(int, int, int, int, int, int rt, std::ostream& s) const override
            { s << "msr\tNZCV, " << unisim::component::cxx::processor::arm::isa::arm64::DisasmGZXR(rt); }
            void DisasmRead(int, int, int, int, int, int rt, std::ostream& s) const override
            { s << "mrs\t" << unisim::component::cxx::processor::arm::isa::arm64::DisasmGZXR(rt) << ", NZCV"; }
            U64 Read(int, int, int, int, int, Arch& cpu) const override { return U32(cpu.nzcv << 28); }
            void Write(int, int, int, int, int, Arch& cpu, U64 value) const override { cpu.nzcv = U32(value) >> 28; }
          } x; return &x;
        }
      else
        dont("system");
      return 0;
    }
} /* end of namespace ut */
