/*
 *  Copyright (c) 2019-2020,
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
#include <unisim/service/interfaces/interface.hh>
#include <unisim/kernel/logger/console/console_printer.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/kernel.hh>
#include <inttypes.h>

struct Runner : public unisim::service::interfaces::ServiceInterface
{
  ~Runner() {}
  virtual void run() const = 0;
};

struct BegPT
  : public unisim::kernel::Client<Runner>
{
  BegPT(char const* _name)
    : unisim::kernel::Object(_name,0)
    , unisim::kernel::Client<Runner>(_name, 0)
  {}
  
};

struct EndPT
  : public unisim::kernel::Service<Runner>
{
  EndPT(char const* _name)
    : unisim::kernel::Object(_name,0)
    , unisim::kernel::Service<Runner>(_name, 0)
  {}
  virtual void run() const override { std::cout << "EndPt::run()\n"; }
};

void
die( char const* reason )
{
  std::cerr << "aborted: " << reason << std::endl;
  throw 0;
}

struct Lab
{
  BegPT begpoint;
  unisim::kernel::Object midpoints[2];
  EndPT endpoint;

  unisim::kernel::ServiceImport<Runner> srv_imports[2];
  unisim::kernel::ServiceExport<Runner> srv_exports[2];
  
  Lab()
    : begpoint("beg-point")
    , midpoints{{"midpoint[0]",0}, {"midpoint[1]",0}}
    , endpoint("end-point")
    , srv_imports{{"i0", &begpoint}, {"i1", &midpoints[0]}}
    , srv_exports{{"o2", &midpoints[1]}, {"o3", &endpoint}}
  {}

  void
  connect(unsigned src, unsigned end)
  {
    if (src >= end)
      die("backward connection");
    if (end < 2)
      srv_imports[src] >> srv_imports[end];
    else if (src < 2)
      srv_imports[src] >> srv_exports[end-2];
    else
      srv_exports[src-2] >> srv_exports[end-2];
  }
  
};

void simdefault(unisim::kernel::Simulator* sim)
{
}

int
main(int argc, char** argv)
{
  unisim::kernel::Simulator simulator(argc, argv, &simdefault);
  unisim::kernel::logger::console::Printer printer;
  
  Lab lab;

  for (char **argp = &argv[1], **end = &argv[argc]; argp < end; ++argp)
    {
      char* arg = *argp;
      if (not isdigit(*arg)) die("exected a number");
      unsigned tail =  strtoul(arg, &arg, 10);
      if (',' != *arg++)     die("exected a comma");
      if (not isdigit(*arg)) die("exected a number");
      unsigned head =  strtoul(arg, &arg, 10);
      if ('\0' != *arg)      die("unexected trailing input");

      lab.connect(tail, head);
    }
  
  
  if (simulator.Setup() != simulator.ST_OK_TO_START)
    return 1;

  lab.srv_imports[0]->run();
  
  return 0;
}

