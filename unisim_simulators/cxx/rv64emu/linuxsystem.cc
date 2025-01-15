/*
 *  Copyright (c) 2021,
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

#include <linuxsystem.hh>
#include <unisim/util/os/linux_os/riscv.hh>
#include <unisim/util/os/linux_os/linux.tcc>
#include <unisim/util/os/linux_os/calls.tcc>
#include <unisim/util/loader/elf_loader/elf64_loader.hh>

LinuxOS::LinuxOS( std::ostream& log,
         unisim::service::interfaces::Registers *regs_if,
         unisim::service::interfaces::Memory<uint64_t> *mem_if,
         unisim::service::interfaces::MemoryInjection<uint64_t> *mem_inject_if
         )
  : unisim::service::interfaces::LinuxOS()
  , linux_lib( log, log, log, regs_if, mem_if, mem_inject_if )
  , exited( false )
  , app_ret_status( -1 )
{}

void
LinuxOS::Setup( std::vector<std::string> const& simargs, std::vector<std::string> const& envs )
{
  bool const verbosity = false;
  // Set up the different linuxlib parameters
  linux_lib.SetVerbose(verbosity);

  linux_lib.SetCommandLine(simargs);

  // Set the linuxlib option to set the target environment with the
  // host environment
  linux_lib.SetApplyHostEnvironment(false);
  linux_lib.SetEnvironment(envs);

  // Load the binary that will be simulated in the target simulator
  {
    typedef typename unisim::util::loader::elf_loader::StdElf<addr_t,addr_t>::Loader Loader;
    Loader loader(linux_lib.DebugInfoStream(), linux_lib.DebugWarningStream(), linux_lib.DebugErrorStream());
    loader.SetOption(unisim::util::loader::elf_loader::OPT_VERBOSE, verbosity);
    loader.SetFileName(std::string(simargs[0]));
    if (not loader.Load())
      throw 0;
    linux_lib.SetFileBlob(loader.GetBlob());
  }

  // Set the system type of the target simulator (should be the same than the
  // binary)
  auto target = new unisim::util::os::linux_os::RISCVTS<unisim::util::os::linux_os::Linux<uint64_t,uint64_t> >( linux_lib );
  linux_lib.SetTargetSystem(target);

  linux_lib.SetEndianness( unisim::util::endian::E_LITTLE_ENDIAN );
  linux_lib.SetStackBase( 0x00000040007ff800UL );
  linux_lib.SetMemoryPageSize( 0x1000UL );
  linux_lib.SetUname("Linux" /* sysname */,
                      "localhost" /* nodename */,
                      "5.5.9-unisim" /* release */,
                      "#1 SMP Fri Mar 12 05:23:09 UTC 2010" /* version */,
                      "riscv" /* machine */,
                      "localhost" /* domainname */);
  // linux_lib.SetStdinPipeFilename(stdin_pipe_filename.c_str());
  // linux_lib.SetStdoutPipeFilename(stdout_pipe_filename.c_str());
  // linux_lib.SetStderrPipeFilename(stderr_pipe_filename.c_str());

  // now it is time to try to run the initialization of the linuxlib
  if (not linux_lib.Load())
    throw 0;

  if (!linux_lib.SetupTarget())
    throw 0;
}

void
LinuxOS::ExecuteSystemCall( int id )
{
  //  linux_lib.LogSystemCall(id);
  linux_lib.ExecuteSystemCall( id, exited, app_ret_status );
}

