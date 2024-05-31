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

#include <decoder.hh>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <inttypes.h>

template <typename T>
bool getu( T& res, char const* arg )
{
  char *end;
  uint64_t tmp = strtoull( arg, &end, 0 );
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

bool process(aarch64::Decoder& decoder, char const* addr_arg, char const* code_arg)
{
  uint64_t addr;
  uint32_t code;

  if (not getu(addr, addr_arg) or not getu(code, code_arg))
    {
      std::cerr << "<addr> and <code> should be, respectively, 64bits and 32bits numeric values.\n";
      return false;
    }

  decoder.process( std::cout, addr, code );
  return true;
}

int
main( int argc, char** argv )
{
  aarch64::Decoder decoder;

  if (0 == strcmp(argv[1],"sav"))
    {
      // Special treatment of SAV files
      std::ifstream source(argv[2]);
      
      if (not source.good()) { std::cerr << "Cannot open SAV file: " << argv[2] << "\n"; return 1; }

      std::string code, disasm;
      int line = 0;
      for (;;)
        {
          if (not (source >> code).good() or not getline(source, disasm).good()) break;
          line += 1;
          code.insert(0,"0x");
          if (not process( decoder, "0x4000", code.c_str() ))
            {
              std::cerr << argv[2] << ':' << line << ": cannot process SAV instruction " << code << ' ' << disasm << std::endl;
              break;
            }
        }
    }

  if (argc < 3)
    {
      std::cerr << "Wrong number of CLI arguments.\n"
                << Usage{argv[0]};
      return 1;
    }

  if (not process(decoder, argv[argc-2], argv[argc-1]))
    {
      std::cerr << Usage{argv[0]};
      return 1;
    }
  
  return 0;
}
