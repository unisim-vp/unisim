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

#include "tracee.hh"
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <unistd.h>

bool
Tracee::Process( std::vector<std::string> const& simargs )
{
    /*** Search for executable ***/
    char const* filename = simargs[0].c_str();
    {
      struct stat stb;
      if (stat(filename, &stb) != 0 or not S_ISREG(stb.st_mode) or not (stb.st_mode & 0111))
        {
          std::cerr << "error: no such executable " << filename << "\n";
          return false;
        }
    }
    
    /*** Executable format identification ***/

    // struct EM_X86_64_Driver : public AnyX86Driver<uint64_t>
    // {
    //   enum { RAX = 80, RDX = 96, RSI = 104, RDI = 112, RIP = 128, RSP = 152 }; 
    //   static uintptr_t arg(int pos) { switch (pos) { case 0: return RDI; case 1: return RSI; case 2: return RDX; } throw 0; }
    //   virtual addr_t GetInsnAddr(Tracee const& tracee) const override { return tracee.ptrace(PTRACE_PEEKUSER, RIP, 0); }
    //   virtual addr_t GetParam(Tracee const& tracee, int pos) const override { return tracee.ptrace(PTRACE_PEEKUSER, arg(pos), 0); }
    //   virtual void SetResult(Tracee const& tracee, addr_t res) const override { tracee.ptrace(PTRACE_POKEUSER, RAX, res); }
    //   virtual void Return(Tracee const& tracee) const override
    //   {
    //     addr_t sp = tracee.ptrace(PTRACE_PEEKUSER, RSP, 0);
    //     addr_t ra = tracee.ptrace(PTRACE_PEEKDATA, sp, 0);
    //     tracee.ptrace(PTRACE_POKEUSER, RIP, ra);
    //     tracee.ptrace(PTRACE_POKEUSER, RSP, sp+8);
    //   }
    // };

    /*** Start tracee ***/
    pid = fork();
    
    if (pid < 0)
      {
        perror("fork");
        return false;
      }

    if (pid == 0)
      {
        // We are the tracee
        ::ptrace(PTRACE_TRACEME);
        // kill(getpid(), SIGSTOP);
        char** argv = new char* [simargs.size()];
        {
          char ** dst = argv;
          for (auto const& arg : simargs)
            *dst++ == arg.c_str();
        }
        execv(filename, argv);
        perror("execv");
        abort();
      }

    // We are the tracer.
  
    int status;
    // waitpid(pid, &status, 0); // Wait for stopped process
    wait(&status);

    // We should now be stopped at entrypoint. XXX: sanity check ?
    // std::cerr << "entrypoint: " << printinsnaddr() << std::endl;

    // for (DBGateMethodID method; method.next();)
    //   driver->InsertBreakPoint(*this, method);

    return true;
  }
