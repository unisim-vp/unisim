/*
 *  Copyright (c) 2009-2020,
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

#include <unisim/component/cxx/processor/arm/dba/aarch64/decoder.hh>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <inttypes.h>

template <typename T>
bool getu( T& res, char const* arg )
{
  char *end;
  uint64_t tmp = strtoull( arg, &end, 16 );
  if ((*arg == '\0') or (*end != '\0'))
    return false;
  res = tmp;
  return true;
}

struct Usage
{
  char const* program;
  friend std::ostream& operator << (std::ostream& sink, Usage const& usage)
  {
    sink << "usage: " << usage.program << " [<flags>] <address> <encoding>\n";
    return sink;
  }
};

bool process(unisim::component::cxx::processor::arm::dba::aarch64::Decoder& decoder, std::ostream& sink, char const* addr_arg, char const* code_arg)
{
  uint64_t addr;
  uint32_t code;

  if (not getu(addr, addr_arg) or not getu(code, code_arg))
    {
      std::cerr << "<addr> and <code> should be, respectively, 64bits and 32bits hexadecimal numeric values.\n";
      return false;
    }

  decoder.process( sink, addr, code );
  return true;
}

int
main( int argc, char** argv )
{
  unisim::component::cxx::processor::arm::dba::aarch64::Decoder decoder;

  if (argc > 1 and strcmp(argv[1], "file") == 0)
    {
      if (argc < 3 or argc > 4)
        { std::cerr << Usage{argv[0]}; return 1; }
        
      std::string insns(argv[argc-1]);
      std::ifstream source(insns);

      if (not source.good())
        {
          std::cerr << "Cannot open " << insns << '\n' << Usage{argv[0]};
          return 1;
        }

      std::string args[2]; // mode, address, insn code
      std::string disasm;
      int cl_argcount = argc - 3;
      for (int idx = 0; idx < cl_argcount; ++idx )
        args[idx] = argv[2+idx];

      int line = 0;
      for (;;)
        {
          for (int idx = cl_argcount; idx < 2; ++idx)
            if (not (source >> args[idx]).good())
              return 0;
          if (not getline(source, disasm).good())
            return 0;
          line += 1;
          std::cout << "(";
          if (not process( decoder, std::cout, args[0].c_str(), args[1].c_str() ))
            {
              std::cerr << argv[2] << ':' << line << ": cannot process instruction " << args[1].c_str() << ' ' << disasm << '\n' << Usage{argv[0]};
              return 1;
            }
          std::cout << ")\n";
        }
      return 0;
    }

  if (argc == 3)
    {
      if (not process(decoder, std::cout, argv[argc-2], argv[argc-1]))
        {
          std::cerr << Usage{argv[0]};
          return 1;
        }
      return 0;
    }

  std::cerr << "Wrong number of CLI arguments.\n"
            << Usage{argv[0]};
  return 1;
}
