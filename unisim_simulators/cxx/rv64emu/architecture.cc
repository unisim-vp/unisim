/*
 *  Copyright (c) 2021,
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

#include "architecture.hh"
#include <unisim/component/cxx/processor/opcache/opcache.tcc>
#include <iostream>
#include <fstream>
#include <iomanip>

void
Arch::debug()
{
  static std::ofstream sink("trace.usim");
  sink << " pc       " << std::setfill('0') << std::setw(16) << std::hex << GetPC();
  for (unsigned reg = 0; reg < 32; ++reg)
    {
      
      sink
        << (reg % 4 ? " " : "\n ")
        << 'x' << std::dec << reg
        << '/' << unisim::component::cxx::processor::riscv::isa::PrintGPR(reg)
        << ' ' << std::setfill('0') << std::setw(16) << std::hex << gprs[reg];
    }
  sink << std::endl;
 // x0/zero 0000000000000000 x1/ra 0000000000014b50 x2/sp 00000040007ff4a0 x3/gp 000000000001e520
 // x4/tp 0000000000000000 x5/t0 0000000000009dbd x6/t1 0000000000000000 x7/t2 000000000000000c
 // x8/s0 000000000001e120 x9/s1 000000000001db38 x10/a0 000000000001f010 x11/a1 0000000000000000
 // x12/a2 0000000000000000 x13/a3 0000000000000000 x14/a4 0000000000000400 x15/a5 0000000000002089
 // x16/a6 0000000000020000 x17/a7 00000000000000d6 x18/s2 0000000000000800 x19/s3 000000000001e120
 // x20/s4 000000000001db38 x21/s5 0000000000000000 x22/s6 0000000000000000 x23/s7 0000000000000000
 // x24/s8 0000000000000000 x25/s9 0000000000000000 x26/s10 000000000001be10 x27/s11 0000000000000000
 // x28/t3 00000000000003e8 x29/t4 00000000000003e8 x30/t5 0000000000000001 x31/t6 0000000000001180
   }

void
Arch::MemDump64(uint64_t addr)
{
  addr &= -8;

  unsigned const size = 8;
  uint8_t buffer[size];
  mem.read( &buffer[0], addr, sizeof buffer );

  std::cerr << "@" << std::setfill('0') << std::setw(16) << std::hex << addr << std::dec << ": ";
  std::cerr << "0x";
  for (unsigned nibble = size*2; nibble-- > 0; )
    {
      uint8_t nbits = (buffer[nibble/2] >> (nibble * 4 & 4)) & 15;
      std::cerr << "0123456789abcdef"[nbits];
    }
  std::cerr << ", " << '"';
  for (unsigned idx = 0; idx < size; ++idx)
    {
      uint8_t byte = buffer[idx];
      std::cerr << (byte < 32 or byte > 126 ? '.' : char(byte));
    }
  std::cerr << '"' << std::endl;
}

template class unisim::component::cxx::processor::opcache::OpCache< unisim::component::cxx::processor::riscv::isa::rv64::Decoder<Arch> >;

Arch::Operation*
Arch::StepInstruction()
{
  /* Start new instruction */
  uint64_t insn_addr = current_insn_addr = next_insn_addr;
    
  /* Fetch instruction word from memory */
  CodeType insn = ReadInsn(insn_addr);

  /* Decode instruction */
  Operation* op = decoder.Decode(insn_addr, insn);
  next_insn_addr += op->GetLength() / 8;
    
  /* Disassemble instruction */
  if (disasm)
    {
      op->disasm(std::cerr << "0x" << std::hex << insn_addr << ": ");
      std::cerr << '\n';
    }

  debug();
  /* Execute instruction*/
  asm volatile ("operation_execute:");
  op->execute( *this );
    
  return op;
}
