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

#include <arch.hh>
#include <linuxsystem.hh>
#include <debugger.hh>
#include <unisim/kernel/logger/console/console_printer.hh>
#include <unisim/kernel/logger/logger.hh>
#include <unisim/kernel/config/json/json_config_file_helper.hh>
#include <unisim/kernel/kernel.hh>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <inttypes.h>

struct Simulator
  : public unisim::kernel::Simulator
  , public unisim::kernel::Object
{
  struct SetupError {};

  Simulator(int argc, char** argv)
    : unisim::kernel::Simulator(argc, argv, &default_config)
    , unisim::kernel::Object("top", 0)
    , printer()
    , cpu("amd64", this, &linux64)
    , linux64("amd64linux", this, std::cerr, &cpu, &cpu, &cpu)
    , debugger()
    , enable_debug(false)
    , verbose_linux(false)
    , param_enable_debug("enable-debug", this, enable_debug, "Enable debug session")
    , param_verbose_linux("verbose-linux", this, verbose_linux, "Verbose linux session")
  {
    if (enable_debug)
      debugger = std::make_unique<Debugger>("debugger", this, cpu, linux64);
    linux64.SetVerbose( verbose_linux );

    linux64.Setup();

    std::vector<std::string> const& simargs = GetCmdArgs();
    if (simargs.size() == 0)
      {
        std::cerr << "Simulation command line empty." << std::endl;
        throw SetupError();
      }

    // Loading image
    std::cerr << "*** Loading elf image: " << simargs[0] << " ***" << std::endl;
    if (char* var = getenv("CORE_SETUP"))
      {
        linux64.Core( simargs[0] );
        linux64.SetBrk( strtoull(var,&var,0) );
        if (*var != ':')  throw 0;
        cpu.fs_base = strtoull(var+1,&var,0);
        if (*var != '\0') throw 0;
        cpu.finit();
      }
    else
      {
        //linux64.ApplyHostEnvironment();
        {
          std::vector<std::string> envs{"LANG=C"};
          linux64.SetEnvironment( envs );
        }
        if (not linux64.Process( simargs ))
          {
            std::cerr << "Error: failed to execute command:\n";
            {
              unsigned idx = 0;
              for (std::string const& arg : simargs)
                std::cerr << "  argv[" << idx++ << "]: " << arg << '\n';
            }
            throw SetupError();
          }
      }
  }

  void run()
  {
    std::cerr << "\n*** Run ***" << std::endl;
    //cpu.gdbchecker.start(cpu);
    while (not linux64.exited)
      {
        cpu.StepInstruction();
        // if (cpu.instruction_count >= 0x100000)
        //   break;
        // if ((cpu.instruction_count % 0x1000000) == 0)
        //   { std::cerr << "Executed instructions: " << std::dec << cpu.instruction_count << " (" << std::hex << op->address << std::dec << ")"<< std::endl; }
      }

    std::cerr << "Program exited with status:" << linux64.app_ret_status << std::endl;
  }

  static void default_config(unisim::kernel::Simulator* sim);

  unisim::kernel::logger::console::Printer printer;
  Arch cpu;
  LinuxOS linux64;
  std::unique_ptr<Debugger> debugger;
  bool enable_debug, verbose_linux;
  unisim::kernel::variable::Parameter<bool> param_enable_debug;
  unisim::kernel::variable::Parameter<bool> param_verbose_linux;
};

void Simulator::default_config(unisim::kernel::Simulator* sim)
{
  sim->SetVariable("top.debugger.debug-hub.parse-dwarf", false);
  sim->SetVariable("top.debugger.debug-hub.dwarf-register-number-mapping-filename", "unisim/util/debug/dwarf/amd64_dwarf_register_number_mapping.xml");
  sim->SetVariable("top.debugger.debug-hub.architecture[0]", "amd64");
  sim->SetVariable("top.debugger.gdb-server.architecture-description-filename", "unisim/service/debug/gdb_server/gdb_amd64.xml");
  sim->SetVariable("top.debugger.gdb-server.verbose", false);
  sim->SetVariable("top.debugger.inline-debugger.program-counter-name", "rip");

  new unisim::kernel::config::json::JSONConfigFileHelper(sim);
}

int
main( int argc, char *argv[] )
{
  Simulator simulator(argc, argv);

  switch (simulator.Setup())
    {
    case simulator.ST_ERROR:         return 1;
    case simulator.ST_OK_DONT_START: return 0;
    default: break;
    }

  simulator.run();

  return 0;
}
