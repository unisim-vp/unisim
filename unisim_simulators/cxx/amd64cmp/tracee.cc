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
#include "arch.hh"
#include <iostream>
#include <cassert>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>

uintptr_t
Tracee::ptrace(enum __ptrace_request request, uintptr_t addr, uintptr_t data) const
{
  return ::ptrace(request, pid, addr, data);
}

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
      char** argv = new char* [simargs.size()+1];
      {
        char const** dst = const_cast<char const**>(argv);
        for (auto const& arg : simargs)
          *dst++ = arg.c_str();
      }
      argv[simargs.size()] = 0;
      execv(filename, argv);
      perror("execv");
      abort();
    }

  // We are the tracer.
  
  int status;
  // waitpid(pid, &status, 0); // Wait for stopped process
  wait(&status);
  
  // We should now have the tracee stopped at entrypoint.  XXX: sanity check ?
  
  std::cerr << "entrypoint: " << GetInsnAddr() << std::endl;

  return true;
}

void
Tracee::StepInstruction() const
{
  for (;;)
    {
      if (this->ptrace(PTRACE_SINGLESTEP, 0, 0) < 0)
        {
          perror( "Tracee::StepInstruction() @ptrace" );
          throw Unexpected{"ptrace for ", pid};
        }

      int status;
      int child_pid = wait(&status);
      
      if (child_pid != pid)
        {
          throw Unexpected{"child", child_pid};
        }
  
      if (WIFEXITED(status))
        {
          throw Stopped{WEXITSTATUS(status)};
        }          
      
      if (not WIFSTOPPED(status))
        {
          throw Unexpected{"exit status", status};
        }

      int stopsig = WSTOPSIG(status);
      switch (stopsig)
        {
        default:
          std::cerr << "error: unhandled signal " << stopsig << std::endl;
          throw Unexpected{"signal", stopsig};

        case SIGTRAP:
          // We sucessfully moved to next instruction
          return;
      
        case SIGCHLD:
          std::cerr << "warning: process has forked...\n";
          break;
      
        }
    }
}

void
Tracee::MemRead( uint8_t* buffer, uint64_t addr, unsigned size ) const
{
  assert (sizeof (long) == 8);
  uint64_t begin = addr & -8, end = (addr+size+7) & -8;
  uint64_t bufsize = end-begin;
  
  uint8_t lbuf[32];
  uint8_t* buf = bufsize <= 32 ? &lbuf[0] : new uint8_t[bufsize];

  for (uint64_t offset = 0; offset < bufsize; offset += 8)
    {
      uint64_t cell = this->ptrace(PTRACE_PEEKDATA, begin+offset, NULL);
      for (unsigned byte = 0; byte < 8; ++byte)
        buf[offset+byte] = cell >> 8*byte;
    }

  memcpy(buffer, &buf[addr-begin], size);
  if (bufsize > 32) delete [] buf;
}

void
Tracee::Load(Arch& cpu) const
{
  struct user_regs_struct registers;
  //  this->rip = tracee.GetInsnAddr();
  this->ptrace(PTRACE_GETREGS, NULL, (uintptr_t)&registers);
  
  cpu.rip = registers.rip;
}
