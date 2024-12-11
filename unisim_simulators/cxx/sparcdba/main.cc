/*
 *  Copyright (c) 2024,
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

#include <unisim/component/cxx/processor/sparc/dba/arch32/decoder.hh>
#include <iostream>
#include <fstream>
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
    sink << "usage:\t" << usage.program << " [file] <address> <encoding0> <encoding1>\n";
    sink <<       "\t" << usage.program << " [file] <address> delayed_nop <encoding>\n";
    return sink;
  }
};

struct Decoder : public unisim::component::cxx::processor::sparc::dba::arch32::Decoder
{
  Decoder( char const* _app_name) : app_name(_app_name) {}

  bool
  run(std::ostream& sink, char const* addr_arg, char const* code_arg0, char const* code_arg1)
  {
    uint32_t addr, code0, code1;

    if (strcmp("delayed_nop", code_arg0) == 0)
      {
        code_arg0 = code_arg1;
        code_arg1 = "01000000";
      }

    if (not getu(addr, addr_arg) or not getu(code0, code_arg0) or not getu(code1, code_arg1))
      {
        std::cerr << "<address>, <encoding0> and <encoding1> should all be 32bits numeric values.\n" << Usage{app_name};
        return false;
      }

    process( sink, addr, code0, code1 );

    return true;
  }
  char const* app_name;
};

int
main( int argc, char** argv )
{
  Decoder decoder(argv[0]);

  if (argc > 1 and strcmp(argv[1], "file") == 0)
    {
      if (argc < 3 or argc > 5)
        { std::cerr << Usage{argv[0]}; return 1; }

      std::string insns(argv[argc-1]);
      std::ifstream source(insns);

      if (not source.good())
        {
          std::cerr << "Cannot open " << insns << '\n' << Usage{argv[0]};
          return 1;
        }

      std::string args[3]; // address, encoding0, encoding1
      std::string disasm;
      int cl_argcount = argc - 3;
      for (int idx = 0; idx < cl_argcount; ++idx)
        args[idx] = argv[2+idx];

      int line = 0;
      for (;;)
        {
          for (int idx = cl_argcount; idx < 3; ++idx)
            if (not (source >> args[idx]).good())
              return 0;
          if (not getline(source, disasm).good())
            return 0;
          line += 1;
          std::cout << "(";
          if (not decoder.run( std::cout, args[0].c_str(), args[1].c_str(), args[2].c_str() ))
            {
              std::cerr << argv[2] << ':' << line << ": cannot process instruction " << args[1].c_str() << ' ' << args[2].c_str() << ' ' << disasm << '\n' << Usage{argv[0]};
              return 1;
            }
          std::cout << ")\n";
        }
      return 0;
    }

  if (argc == 4)
    {
      if (not decoder.run( std::cout, argv[1], argv[2], argv[3] ))
        {
          std::cerr << Usage{argv[0]};
          return 1;
        }
      return 0;
    }


  std::cerr << "Wrong number of CLI arguments.\n" << Usage{argv[0]};
  return 1;
}

