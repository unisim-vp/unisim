/*
 *  Copyright (c) 2018,
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
#include <linuxsystem.hh>
#include <iostream>
#include <iomanip>

struct Disasm
{
  Disasm( Arch::Operation* _op ) : op(*_op) {}
  Arch::Operation& op;
  friend std::ostream& operator << (std::ostream& sink, Disasm const& d) { d.op.disasm( sink ); return sink; }
};

int
main( int argc, char* argv[] )
{
  uintptr_t simargs_idx = 1;
  std::vector<std::string> simargs(&argv[simargs_idx], &argv[argc]);
  
  {
    std::cerr << "arguments:\n";

    unsigned idx = 0;
    for (auto && arg : simargs)
      {
        std::cerr << "  args[" << idx++ << "]: " << arg << '\n';
      }
  }
  
  if (simargs.size() == 0)
    {
      std::cerr << "Simulation command line empty." << std::endl;
      return 1;
    }

  std::vector<std::string> envs;
  envs.push_back( "LANG=C" );
  
  Arch cpu;
  
  LinuxOS linux32( std::cerr, &cpu, &cpu, &cpu );
  cpu.SetLinuxOS( &linux32 );
  
  linux32.Setup( simargs, envs );
  
  //  cpu.disasm = false;
  
  // Loading image
  std::cerr << "*** Loading elf image: " << simargs[0] << " ***" << std::endl;
  
  std::cerr << "\n*** Run ***" << std::endl;
  
  try
    {
      do
        {
          Arch::Operation* op = cpu.fetch();
          //std::cerr << std::hex << op->GetAddr() << std::dec << ": " << Disasm( op ) << std::endl;
          asm volatile ("operation_execute:");
          bool success = op->execute( &cpu );
          if (not success)
            throw 0;

          cpu.commit();
          //{ uint64_t chksum = 0; for (unsigned idx = 0; idx < 8; ++idx) chksum ^= cpu.regread32( idx ); std::cerr << '[' << std::hex << chksum << std::dec << ']'; }
      
          // if ((cpu.m_instcount % 0x1000000) == 0)
          //   { std::cerr << "Executed instructions: " << std::dec << cpu.m_instcount << " (" << std::hex << op->address << std::dec << ")"<< std::endl; }
        }
      while (not linux32.exited);      
    }
  catch (MisInsn& insn)
    {
      std::cerr << std::hex << insn.addr << ":	";
      for (int idx = 4; --idx >= 0;)
        std::cerr << std::hex << std::setw(2) << std::setfill('0') << ((insn.code >> idx*8) & 0xff) << ' ';
      std::cerr << std::endl;
    }
  
  std::cerr << "Program exited with status:" << std::dec << linux32.app_ret_status << std::endl;
  std::cerr << "Executed instructions: " << cpu.instcount << std::endl;
  
    
  return 0;
}

