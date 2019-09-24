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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#include <tracee.hh>
#include <iostream>
#include <cassert>

int
main(int argc, char** argv)
{
  std::cerr << "DBGate tracer.\n";

  int optidx = 1;

  assert(not argv[argc]);

  if (optidx >= argc)
    {
      std::cerr << "Usage: " << argv[0] << " <program> <program args...>\n";
      return 1;
    }

  Tracee tracee(argv+optidx);

  if (tracee.good())
    {
      std::cerr << "No tracee, giving up.\n";
      return 1;
    }

  for (DBGateMethodID method; (method = tracee.nextcall()).good();)
    {
      tracee.PrintProgramCounter( std::cerr << "In '" << method.c_str() << "'@" ) << std::endl;
  
      switch (method.code)
        {
        case DBGateMethodID::open:
          {
            Tracee::OpenArgs args;
            tracee.getargs(args);
          } break;
          
        case DBGateMethodID::close:
          {
            Tracee::CloseArgs args;
            tracee.getargs(args);
          } break;
        case DBGateMethodID::write:
          {
            Tracee::WriteArgs args;
            tracee.getargs(args);   
          } break;
          
        default:
          throw 0;
        }
    }
  
  /* We are the tracer and tracee is launched,  */
  // for (;;)
  //   #
  // int status;
  // waitpid(tracee.pid, &status, 0);

  // while (WIFSTOPPED(wait_status)) {
  //   icounter++;
  //   /* Make the child execute another instruction */
  //   if (ptrace(PTRACE_SINGLESTEP, child_pid, 0, 0) < 0) {
  //     perror("ptrace");
  //     return;
  //   }

  //   /* Wait for child to stop on its next instruction */
  //   wait(&wait_status);
  // }

  
  return 0;
}

