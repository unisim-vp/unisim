/*
 *  Copyright (c) 2009-2023,
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
#include <unisim/component/cxx/processor/arm/isa/constants.hh>
#include <unisim/component/cxx/processor/arm/isa/disasm.hh>
#include <iostream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <inttypes.h>

uint32_t getu32( uint32_t& res, char const* arg )
{
  char *end;
  uint32_t tmp = strtoul( arg, &end, 0 );
  if ((*arg == '\0') or (*end != '\0'))
    return false;
  res = tmp;
  return true;
}

char const* usage()
{
  return
    "usage: <program> arm|thumb <address> <encoding>\n";
}

template <unsigned N>
char const* after(std::string const& s, char const (&ref)[N])
{
  uintptr_t const size = N-1;
  if (s.compare(0,size,&ref[0]))
    return 0;
  return &s[size];
}

int
main( int argc, char** argv )
{
  if (argc != 4)
    {
      std::cerr << "Wrong number of CLI arguments.\n" << usage();
      return 1;
    }

  uint32_t addr, code;

  if (not getu32(addr, argv[2]) or not getu32(code, argv[3]))
    {
      std::cerr << "<addr> and <code> should be 32bits numeric values.\n" << usage();
      return 1;
    }

  aarch32::Decoder decoder;

  std::string exec_flags(argv[1]);
  exec_flags += ',';
  for (std::string::iterator cur = exec_flags.begin(), end = exec_flags.end(), nxt; cur != end; cur = nxt+1 )
    {
      nxt = std::find( cur, end, ',' );
      std::string flag( cur, nxt );
      if (flag.size() == 0)
        continue;
      if      (flag == "arm")        { decoder.iset = decoder.Arm; }
      else if (flag == "thumb")      { decoder.iset = decoder.Thumb; }
      else if (flag == "little")     { decoder.bigendian = false; }
      else if (flag == "big")        { decoder.bigendian = true; }
      else if (flag == "user")       { decoder.mode = unisim::component::cxx::processor::arm::USER_MODE; }
      else if (flag == "fiq")        { decoder.mode = unisim::component::cxx::processor::arm::FIQ_MODE; }
      else if (flag == "irq")        { decoder.mode = unisim::component::cxx::processor::arm::IRQ_MODE; }
      else if (flag == "supervisor") { decoder.mode = unisim::component::cxx::processor::arm::SUPERVISOR_MODE; }
      else if (flag == "monitor")    { decoder.mode = unisim::component::cxx::processor::arm::MONITOR_MODE; }
      else if (flag == "abort")      { decoder.mode = unisim::component::cxx::processor::arm::ABORT_MODE; }
      else if (flag == "hypervisor") { decoder.mode = unisim::component::cxx::processor::arm::HYPERVISOR_MODE; }
      else if (flag == "undefined")  { decoder.mode = unisim::component::cxx::processor::arm::UNDEFINED_MODE; }
      else if (flag == "system")     { decoder.mode = unisim::component::cxx::processor::arm::SYSTEM_MODE; }
      else if (flag == "outitb")     { decoder.itstate = 0; }
      else if (char const* it = after(flag, "it"))
        {
          unsigned bits = 0;
          if (char const* itstate = after(flag, "itstate="))
            {
              while (unsigned bit = *itstate++)
                {
                  if ((bit -= '0') < 2)
                    bits = (bits << 1) | bit;
                }
            }
          else
            {
              bits = int(unisim::component::cxx::processor::arm::Condition(it).code) << 4 | 0b1000;
            }
          decoder.itstate = bits;
        }
      else
        {
          std::cerr << "Unknown execution state flag: " << flag << std::endl;
          return 1;
        }

    }

  decoder.process( std::cout, addr, code );

  return 0;
}
