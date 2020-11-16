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
#include <unisim/component/cxx/processor/sparc/isa/sv8/disasm.hh>
#include <iomanip>
#include <ostream>
#include <cmath>

void
Runner::step_instruction()
{
  uint32_t insn_addr = pc;
    
  asm volatile ("operation_fetch:");
  uint32_t code = MemRead(uint32_t(), insn_asi(), insn_addr);
    
  asm volatile ("operation_decode:");
  Operation* operation = decode(insn_addr, code);

  if (disasm)
    {
      std::cerr << "0x" << std::hex << pc << " (0x" << operation->GetEncoding() << "): ";
      operation->disasm( std::cerr, pc );
      std::cerr << "\n";
    }
  
  asm volatile ("operation_execute:");
  operation->execute( *this );
  
  pc = npc;
  npc = nnpc;
  nnpc += 4;
}

void
Runner::run(Interface::testcode_t testcode, uint32_t* data)
{
  U64 const magic_return_address = 0xdeadc0de;

  SetGPR(8, uintptr_t(data));
  uint32_t sim_stack[32];
  SetGPR(14, uintptr_t(&sim_stack[0]) + sizeof sim_stack);
  SetGPR(15, magic_return_address);
  nnpc = (npc = (pc = uintptr_t(testcode)) + 4) + 4;
    
  for (int ttl = 100; GetPC() != magic_return_address;)
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
  
