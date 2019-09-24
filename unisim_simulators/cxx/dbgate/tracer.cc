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

/* UNISIM */
#include <unisim/util/dbgate/dbgate.hh>
#include <unisim/util/identifier/identifier.hh>
#include <unisim/util/loader/elf_loader/elf_loader.hh>
#include <unisim/util/loader/elf_loader/elf32_loader.hh>
#include <unisim/util/loader/elf_loader/elf64_loader.hh>
#include <unisim/util/debug/elf_symtab/elf_symtab.hh>
/* C++ */
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <sstream>
/* C */
#include <cassert>
#include <cstring>
#include <csignal>
/* POSIX */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

struct DBGateMethodID  : public unisim::util::identifier::Identifier<DBGateMethodID>
{
  enum Code { dbgate_open, dbgate_close, dbgate_write, end } code;

  char const* c_str() const
  {
    switch (code)
      {
      case dbgate_open:  return "dbgate_open";
      case dbgate_close: return "dbgate_close";
      case dbgate_write: return "dbgate_write";
      case end: break;
      }
    return "NA";
  }

  DBGateMethodID() : code(end) {}
  DBGateMethodID( Code _code ) : code(_code) {}
  DBGateMethodID( char const* _code ) : code(end) { init(_code); }
};

struct Tracee
{
  Tracee( char** _argv )
    : argv(_argv)
    , pathname()
    , pid()
    , methods()
  {
    char const* filename = _argv[0];
    
    if (strchr(filename,'/'))
      {
        pathname.assign( filename );
        check_executable();
      }
    else
      {
        bool cwdcheck = false;
        for (char const* path = getenv("PATH"), *npath; path and *path; path = npath)
          {
            if (char const* colon = strchr(path, ':'))
              {
                pathname.assign(path, colon);
                npath = colon + 1;
              }
            else
              {
                pathname.assign(path);
                npath = 0;
              }

            if (not pathname.size() or pathname == ".")
              {
                if (cwdcheck) continue;
                cwdcheck = true;
                pathname.assign( filename );
              }
            else
              {
                if (pathname.back() != '/')
                  pathname += '/';
                pathname += filename;
              }
            if (check_executable())
              break;
          }
      }
  }

  bool check_executable()
  {
    struct stat stb;
    if (stat(pathname.c_str(), &stb) == 0 and S_ISREG(stb.st_mode) and (stb.st_mode & 0111))
      {
        // SETUID/GID ?
        return true;
      }
    pathname.clear();
    return false;
  }
  
  // std::string cwd;

  // std::string const& getcwd()
  // {
  //   if (not cwd.size())
  //     for (intptr_t capacity = 128; true; capacity *= 2)
  //       {
  //         char storage[capacity];
  //         if (not getcwd( storage, capacity ))
  //           {
  //             if (errno != ERANGE) throw 0;
  //             continue; 
  //           }
  //         cwd.assign(storage);
  //       }
  //   return cwd;
  // }

  void
  startup_child()
  {
    pid = fork();
    
    if (pid < 0)
      { perror("fork"); throw 0; }

    if (pid == 0)
      {
        // start tracing, hang and start running                                                                                            ptrace(PTRACE_TRACEME);
        kill(getpid(), SIGSTOP);
        execv(pathname.c_str(),argv);
        perror("execvp");
        throw 0;
      }

    /* We are the tracer */
    int status;
    waitpid(pid, &status, 0);
    
    // We set the PTRACE_O_TRACESYSGOOD option; when the child stops for
    // a syscall-related reason, it stops with signal (SIGTRAP | 0x80) so
    // we can easily distinguish syscall stops from other stops.
    // ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_TRACESYSGOOD);

  }

  void getbreakpoints();

  char** argv;
  std::string pathname;
  pid_t pid;
  uintptr_t methods[DBGateMethodID::end];
};

int
main(int argc, char** argv)
{
  std::cerr << "DBGate tracer.\n";

  int optidx = 1;

  assert(not argv[argc]);

  Tracee tracee(argv+optidx);

  if (not tracee.pathname.size())
    {
      std::cerr << "Executable not found.\n";
      return 1;
    }

  std::cerr << "Executable path: " << tracee.pathname << std::endl;

  tracee.getbreakpoints();
  
  //  tracee.startup_child();

  return 0;
}

void
Tracee::getbreakpoints()
{
  // Loading a core file (memory + register state)
  typedef typename unisim::util::loader::elf_loader::StdElf<uintptr_t,uintptr_t>::Loader Loader;
  // typedef typename Loader::Elf_Ehdr_type Elf_Ehdr;
  // typedef typename Loader::Elf_Phdr_type Elf_Phdr;
  typedef typename Loader::Symbol Symbol;
  
  //  std::ifstream is(pathname.c_str(), std::ifstream::in | std::ifstream::binary);

  Loader loader;
  loader.SetOption(unisim::util::loader::elf_loader::OPT_FILENAME, pathname.c_str());
  loader.Load();

  for (DBGateMethodID method; method.next();)
    {
      if (Symbol const* symbol = loader.FindSymbolByName(method.c_str(), Symbol::SYM_FUNC))
        {
          uintptr_t addr = symbol->GetAddress(), size = symbol->GetSize();
          std::cerr << method.c_str() << ": <0x" << std::hex << addr << ", " << std::dec << size << '>' << std::endl;
          methods[method.idx()] = addr;
        }
      else
        {
          std::cerr << "Can't find dbgate symbol: " << method.c_str() << std::endl;
          throw 0;
        }
    }      
  
  // hdr.p = Loader::ReadElfHeader(is);
  // //bool need_endian_swap = Loader::NeedEndianSwap(hdr.p);
    
  // if (not hdr.p) throw "Could not read ELF header";
  
  // struct phdr { ~phdr() { free(p); } Elf_Phdr* p; } phdr_table;
  
  // phdr_table.p = Loader::ReadProgramHeaders(hdr.p, is);
  // if (not phdr_table.p)
  //   throw "Can't read program headers";

  
  // unisim::util::debug::elf_symtab::ELF_SymtabHandler<MEMORY_ADDR, Elf_Sym> *symtab_handler;
  
  
  // if (Symbol const* symbol = symbol_table_lookup_import ?
  //     symbol_table_lookup_import->FindSymbolByName("__log_buf", Symbol::SYM_OBJECT) : 0)
  //   {
  //     linux_printk_buf_addr = symbol->GetAddress();
  //     linux_printk_buf_size = symbol->GetSize();
  //     linux_printk_snooping = true;

  //     if(verbose)
  //       logger << DebugInfo << "Found Linux printk buffer @["
  //              << std::hex << linux_printk_buf_addr << ','
  //              << std::dec << linux_printk_buf_size << "]" << EndDebugInfo;
  //   }
  // else
  //   {
  //     logger << DebugWarning << "Linux printk buffer not found. Linux printk snooping will not work properly." << EndDebugWarning;
  //     linux_printk_snooping = false;
  //   }
}
