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

#include <linuxsystem.hh>
#include <architecture.hh>
#include <iostream>

int main(int argc, char *argv[])
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

  std::vector<std::string> envs;
  envs.push_back( "LANG=C" );
  
  Arch core;
  LinuxOS linux64( std::cerr, &core, &core, &core );
  core.SetLinuxOS( &linux64 );
  
  linux64.Setup( simargs, envs );
  
  core.disasm = false;
  
  // Loading image
  std::cerr << "*** Loading elf image: " << simargs[0] << " ***" << std::endl;
  
  std::cerr << "\n*** Run ***" << std::endl;
  
  while (not linux64.exited)
    {
      core.StepInstruction();
      //      Arch::Operation* op = core.fetch();
      // op->disasm( std::cerr );
      // std::cerr << std::endl;
      //      asm volatile ("operation_execute:");
      //      op->execute( core );
      //{ uint64_t chksum = 0; for (unsigned idx = 0; idx < 8; ++idx) chksum ^= core.regread32( idx ); std::cerr << '[' << std::hex << chksum << std::dec << ']'; }
      
      // if ((core.m_instcount % 0x1000000) == 0)
      //   { std::cerr << "Executed instructions: " << std::dec << core.m_instcount << " (" << std::hex << op->address << std::dec << ")"<< std::endl; }
    }
  
  std::cerr << "Program exited with status:" << linux64.app_ret_status << std::endl;
  
  
  return 0;
}
