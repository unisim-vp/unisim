/*
 *  Copyright (c) 2018-2022,
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

#include <arch.hh>
// #include <unisim/component/cxx/processor/intel/modrm.hh>
// #include <unisim/component/cxx/processor/intel/disasm.hh>
// #include <unisim/component/cxx/processor/intel/vectorbank.hh>
// #include <unisim/component/cxx/processor/intel/types.hh>
// #include <unisim/util/arithmetic/arithmetic.hh>
#include <linuxsystem.hh>
// #include <algorithm>
#include <iostream>
// #include <fstream>
#include <string>
#include <vector>
// #include <set>
// #include <iomanip>
// #include <cmath>
// #include <cctype>
// #include <cassert>
#include <inttypes.h>

int
main( int argc, char *argv[] )
{
  uintptr_t simargs_idx = 1;
  std::vector<std::string> simargs(&argv[simargs_idx], &argv[argc]);
  
  {
    std::cerr << "arguments:\n";
    unsigned idx = 0;
    for (std::string const& arg : simargs) {
      std::cerr << "  args[" << idx << "]: " << arg << '\n';
    }
  }
  
  if (simargs.size() == 0) {
    std::cerr << "Simulation command line empty." << std::endl;
    return 1;
  }

  Arch cpu;
  LinuxOS linux64( std::cerr, &cpu, &cpu, &cpu );
  cpu.SetLinuxOS( &linux64 );
  cpu.do_disasm = false;
  linux64.Setup( false );
  
  // Loading image
  std::cerr << "*** Loading elf image: " << simargs[0] << " ***" << std::endl;
  if (char* var = getenv("CORE_SETUP"))
    {
      linux64.Core( simargs[0] );
      linux64.SetBrk( strtoull(var,&var,0) );
      if (*var != ':')  throw 0;
      cpu.fs_base = strtoull(var+1,&var,0);
      if (*var != '\0') throw 0;
      cpu.finit();
    }
  else
    {
      //linux64.ApplyHostEnvironment();
      {
        std::vector<std::string> envs{"LANG=C"};
        linux64.SetEnvironment( envs );
      }
      linux64.Process( simargs );
    }
  
  std::cerr << "\n*** Run ***" << std::endl;
  //cpu.gdbchecker.start(cpu);
  while (not linux64.exited)
    {
      Arch::Operation* op = cpu.fetch();
      if (not op)
        return 1;
      // op->disasm( std::cerr );
      // std::cerr << std::endl;
      asm volatile ("operation_execute:");
      op->execute( cpu );
      //cpu.gdbchecker.step(cpu);
      // { uint64_t chksum = 0; for (unsigned idx = 0; idx < 8; ++idx) chksum ^= cpu.regread( GOd(), idx ); std::cerr << '[' << std::hex << chksum << std::dec << ']'; }
      if (cpu.instruction_count >= 0x100000)
        break;
      // if ((cpu.instruction_count % 0x1000000) == 0)
      //   { std::cerr << "Executed instructions: " << std::dec << cpu.instruction_count << " (" << std::hex << op->address << std::dec << ")"<< std::endl; }
    }

  std::cerr << "Program exited with status:" << linux64.app_ret_status << std::endl;
  
  return 0;
}
