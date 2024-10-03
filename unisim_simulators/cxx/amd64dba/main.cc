/*
 *  Copyright (c) 2009-2021,
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

#include <unisim/component/cxx/processor/intel/dba/decoder.hh>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <inttypes.h>

bool getu64( uint64_t& res, char const* arg )
{
  char *end;
  uint64_t tmp = strtoull( arg, &end, 0 );
  if ((*arg == '\0') or (*end != '\0'))
    return false;
  res = tmp;
  return true;
}

bool getbytes( std::vector<uint8_t>& res, char const* arg )
{
  for (char const* end = 0; *arg; arg = end)
    {
      unsigned l = strtoul(arg,(char**)&end,16);
      if (end == arg) break;
      res.push_back(l);
    }
  return res.size();
}

void usage( std::ostream& sink, char const* progname )
{
  sink << "usage:\n"
       << "  " << progname << " x86|intel64 <address> <encoding>\n"
       << "  " << progname << " x86|intel64 <file>\n";
}

bool process( unisim::component::cxx::processor::intel::dba::Decoder& decoder, std::ostream& sink, char const* as, char const* cs )
{
  uint64_t addr;
  std::vector<uint8_t> code;

  if (not getu64(addr, as) or not getbytes(code, cs))
    {
      std::cerr << "<addr> and <code> should be numeric values (got " << as << " and " << cs << ").\n";
      return false;
    }

  decoder.process( sink, addr, std::move(code) );
  return true;
}

//extern void dbgate_keep();

int
main( int argc, char** argv )
{
  if (argc < 2)
    {
      std::cerr << "Missing mode (x86|intel64)\n";
      usage(std::cerr, argv[0]);
      return 1;
    }

  unisim::component::cxx::processor::intel::dba::Decoder decoder;
  decoder.mode64 = strcmp("intel64", argv[1]) == 0;

  if (argc == 3)
    {
      std::ifstream source(argv[2]);
      if (not source.good())
        {
          std::cerr << "Cannot open " << argv[2] << "\n";
          usage(std::cerr, argv[0]);
          return 1;
        }

      std::string abuf, cbuf;
      for (;;)
        {
          if (not (source >> abuf).good() or not getline(source, cbuf).good()) break;
          //std::ofstream sink("/dev/null");
          std::ostream& sink(std::cout);
          sink << "(";
          if (not process(decoder, sink, abuf.c_str(), cbuf.c_str()))
            { usage(std::cerr, argv[0]); return 1; }
          sink << ")\n";
        }

      return 0;
    }

  if (argc != 4)
    {
      std::cerr << "Wrong number of CLI arguments.\n";
      usage(std::cerr, argv[0]);
      return 1;
    }

  if (not process(decoder, std::cout, argv[2], argv[3] ))
    { usage(std::cerr, argv[0]); return 1; }

  //  dbgate_keep();

  return 0;
}


// #include <unisim/util/dbgate/client/client.hh>
// #include <unisim/util/symbolic/symbolic.hh>

// DBGATE_DEFS;

// extern "C" {
//   void dbgate_print_expr( unisim::util::symbolic::ExprNode const* node, int cd )
//   {
//     unisim::util::dbgate::client::odbgstream sink(cd);
//     sink << "<!DOCTYPE html>\n" << "<html><body>\n";
//     sink << "<p>ExprNode::" << "Repr@" << (void*)node << "</p>\n<p>";
//     node->Repr(sink);
//     sink << "</p></body></html>\n";
//   }
// }


// int dbgate_global;
// void
// dbgate_keep()
// {
//   if (dbgate_global)
//     {
//       puts((char const*)&dbgate_print_expr);
//     }
// }
