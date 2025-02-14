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

#include <iostream>
#include <arm.hh>

template <unsigned N> void append( std::vector<uint8_t>& buffer, char const (&data)[N] ) { buffer.insert(buffer.end(), &data[0], &data[N-1]); }

// callback for tracing instructions
typedef void (*cb_code)();
void hook_code(void* uc, uint64_t address, unsigned size)
{
  std::cout << ">>> Tracing instruction at " << std::hex << address << std::dec << ", size = " << size << "\n";
}

// callback for tracing basic blocks
void hook_block(void* uc, uint64_t address, unsigned size)
{
  std::cout << ">>> Tracing basic block at " << std::hex << address << std::dec << ", block size = " << std::hex << size << std::dec << "\n";
}

int
main(int argc, char** argv)
{
  std::vector<uint8_t> code;
  // code to be emulated
  append(code, "\x00\x00\x50\xe3"); //     0: cmp     r0, #0
  append(code, "\x00\x00\x60\xb2"); //     4: rsblt   r0, r0, #0
  append(code, "\x00\x00\x51\xe3"); //     8: cmp     r1, #0
  append(code, "\x00\x10\x61\xb2"); //     c: rsblt   r1, r1, #0
  append(code, "\x01\x00\x50\xe1"); //    10: cmp     r0, r1
  append(code, "\x02\x00\x00\xaa"); //    14: bge     24 <gcd+0x24>
  append(code, "\x00\x30\xa0\xe1"); //    18: mov     r3, r0
  append(code, "\x01\x00\xa0\xe1"); //    1c: mov     r0, r1
  append(code, "\x03\x10\xa0\xe1"); //    20: mov     r1, r3
  append(code, "\x00\x00\x51\xe3"); //    24: cmp     r1, #0
  append(code, "\x1e\xff\x2f\x01"); //    28: bxeq    lr
  append(code, "\x00\x30\xa0\xe1"); //    2c: mov     r3, r0
  append(code, "\x01\x00\x53\xe1"); //    30: cmp     r3, r1
  append(code, "\xf8\xff\xff\xba"); //    34: blt     1c <gcd+0x1c>
  append(code, "\x03\x30\x61\xe0"); //    38: rsb     r3, r1, r3
  append(code, "\xfb\xff\xff\xea"); //    3c: b       30 <gcd+0x30>

  uint32_t entrypoint = 0x1000;
  uint32_t exitpoint = entrypoint + code.size();

  ArmProcessor proc("ArmTest", false);
  proc.mem_map(entrypoint, 1024, 7, 0, 0, 0);

  // write machine code to be emulated to memory
  proc.mem_write(entrypoint, code.data(), code.size());
  proc.mem_write(exitpoint, (uint8_t*)"\x00\x00\xa0\xe1", 4); // mov r0, r0

  proc.SetGPR(0, 0x1234);
  proc.SetGPR(1, 0x6789);
  proc.SetGPR(14, exitpoint);
  //proc.reg_write(ctx, unipy.EMU_ARM_REG_LR, exitpoint);
  //proc.reg_write(ctx, unipy.EMU_ARM_REG_APSR, 0xFFFFFFFF); // All application flags turned on

  // tracing all basic blocks with customized callback
  proc.add_hook( 1<<Processor::Hook::BLOCK, (void*)&hook_block, 1, 0 );

  // tracing one instruction at entrypoint with customized callback
  proc.add_hook( 1<<Processor::Hook::CODE, (void*)&hook_code, entrypoint, entrypoint);

  std::cout << "Emulate ARM code\n";

  // (de)activate disassembly
  proc.set_disasm(false);

  try { proc.run(entrypoint, exitpoint, 0); }

  catch (Processor::Abort) { return -1; }

  std::cout << ">>> Emulation done. Below is the CPU context\n";

  for (unsigned reg = 0; reg <= 1; ++reg)
    {
      uint64_t rval = proc.GetGPR(reg);
      std::cout << ">>> R" << reg << " = " << rval << std::endl;
    }

  return 0;
}
