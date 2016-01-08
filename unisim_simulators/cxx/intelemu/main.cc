/*
 *  Copyright (c) 2009,
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

#include <unisim/component/cxx/processor/intel/arch.hh>
#include <unisim/component/cxx/processor/intel/isa/intel.hh>
#include <unisim/service/interfaces/memory_injection.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/util/os/linux_os/linux.hh>
#include <unisim/kernel/logger/logger_server.hh>
#include <unisim/kernel/logger/logger.hh>
#include <linuxsystem.hh>
#include <dtlib/loader.hh>
#include <dtlib/misc.hh>
#include <iostream>

struct Arch
  : public unisim::component::cxx::processor::intel::Arch
  , public unisim::service::interfaces::MemoryInjection<uint32_t>
  , public unisim::service::interfaces::Memory<uint32_t>
  , public unisim::service::interfaces::Registers

{
  
  Arch()
    : unisim::component::cxx::processor::intel::Arch()
    , unisim::service::interfaces::MemoryInjection<uint32_t>()
    , unisim::service::interfaces::Memory<uint32_t>()
    , unisim::service::interfaces::Registers()
  {
  }
  
  // unisim::service::interfaces::Memory<uint32_t>
  void Reset() {}
  bool ReadMemory(uint32_t addr, void* buffer, uint32_t size ) { throw 0; return false; }
  bool WriteMemory(uint32_t addr, void const* buffer, uint32_t size) { throw 0; return false; }
  // unisim::service::interfaces::Registers
  unisim::util::debug::Register* GetRegister(char const* name) { throw 0; return 0; }
  // unisim::service::interfaces::MemoryInjection<ADDRESS>
  bool InjectReadMemory(uint32_t addr, void *buffer, uint32_t size) { throw 0; return false; }
  bool InjectWriteMemory(uint32_t addr, void const* buffer, uint32_t size) { throw 0; return false; }
  
};

int
main( int argc, char *argv[] )
{
#ifdef WIN32
  struct { // Loads/Unloads the winsock2 dll
    WSADATA wsaData;
    WSAEnv()
    {
      WORD wVersionRequested = MAKEWORD( 2, 2 );
      if(WSAStartup(wVersionRequested, &wsaData) == 0)
        return;
      std::cerr << "WSAStartup failed" << std::endl;
      throw 0;
    }
    ~WSAEnv() { WSACleanup(); }
  } wsa_env;
#endif

  // Simulator simulator( argc, argv );
  unisim::kernel::logger::Logger::StaticServerInstance()->opt_std_err_color_ = true;

  Arch cpu;
  
  unisim::kernel::logger::Logger logger("linux32");
  unisim::util::os::linux_os::Linux<uint32_t, uint32_t> linux32(logger, &cpu, &cpu, &cpu);
  
  
  // Set up the different linuxlib parameters
  linux32.SetVerbose(true);
  
  uintptr_t simargs_idx = 1;
  std::vector<std::string> simargs(&argv[simargs_idx], &argv[argc]);
  
  {
    std::cerr << "arguments:\n";
    unsigned idx = 0;
    for (std::string const& arg : simargs) {
      std::cerr << "  args[" << idx << "]: " << arg << '\n';
    }
  }
  
  if (simargs.size() == 0) {
    std::cerr << "Simulation command line empty." << std::endl;
    return 1;
  }
  
  if (not linux32.SetCommandLine(simargs))
    throw 0;
  
  // Set the linuxlib option to set the target environment with the
  // host environment
  linux32.SetApplyHostEnvironment(false);

  // Set the binary that will be simulated in the target simulator
  if (not linux32.AddLoadFile( simargs[0].c_str() ))
    throw 0;
  
  // Set the system type of the target simulator (should be the same than the
  // binary)
  if (not linux32.SetSystemType("i386"))
    throw 0;
  
  // linux32.SetEndianness( unisim::util::endian::E_LITTLE_ENDIAN );
  // linux32.SetStackBase( 0x40000000UL );
  // linux32.SetMemoryPageSize( 0x1000UL );
  
  // linux32.SetUname("Linux" /* sysname */,
  //                     "localhost" /* nodename */,
  //                     "2.6.27.35" /* release */,
  //                     "#UNISIM SMP Fri Mar 12 05:23:09 UTC 2010" /* version */,
  //                     "i386" /* machine */,
  //                     "localhost" /* domainname */);
  // linux32.SetHWCap("swp half fastmult");
  // linux32.SetStdinPipeFilename(stdin_pipe_filename.c_str());
  // linux32.SetStdoutPipeFilename(stdout_pipe_filename.c_str());
  // linux32.SetStderrPipeFilename(stderr_pipe_filename.c_str());

  // now it is time to try to run the initialization of the linuxlib
  // if (not linux32.Load())
  //   throw 0;
  
  // if (!linux32.SetupTarget())
  //   throw 0;
  
  // switch (simulator.Setup())
  //   {
  //   case unisim::kernel::service::Simulator::ST_ERROR:
  //     std::cerr << "ERROR: Can't start simulation because of previous errors" << std::endl;
  //     return 1;
      
  //   case unisim::kernel::service::Simulator::ST_OK_DONT_START:
  //     std::cerr << "Successfully configured the simulator." << std::endl;
  //     return 0;
      
  //   case unisim::kernel::service::Simulator::ST_WARNING:
  //     std::cerr << "WARNING: problems detected during setup, trying anyway." << std::endl;
  //   case unisim::kernel::service::Simulator::ST_OK_TO_START:
  //     std::cerr << "Starting simulation." << std::endl;
  //     break;
  //   }
  
  cpu.m_linux_system.m_sink.redirect( std::cerr );
  cpu.m_linux_system.m_verbose = false;
  cpu.m_events.redirect( std::cerr );
  cpu.m_disasm = false;
  
  // Loading image
  std::cerr << "*** Loading elf image: " << simargs[0] << " ***" << std::endl;
  
  {
    dtlib::ELF::Image elfimage( simargs[0].c_str() );
    
    elfimage.m_verbose = true;
    
    // uint32_t loadedbytes = 
    elfimage.load( cpu.target() );
    elfimage.ia32chk();

    // uintptr_t envcount = 0;
    // for (envcount = 0; envp[envcount]; ++envcount);
    // std::vector<std::string> envs( &envp[0], &envp[envcount] );
    std::vector<std::string> envs;
    envs.push_back( "LANG=C" );
    dtlib::ELF::Root eiroot = elfimage.getroot();
    assert( eiroot.object_file_type() == dtlib::ELF::Root::etexec );
    cpu.m_linux_system.static_setup( cpu.target(), eiroot, simargs, envs );
  }
  
  typedef unisim::component::cxx::processor::intel::Operation Operation;
  std::cerr << "\n*** Run ***" << std::endl;
  while (cpu.m_running) {
    Operation* op = cpu.fetch();
    // op->disasm( std::cerr );
    // std::cerr << std::endl;
    asm volatile ("operation_execute:");
    op->execute( cpu );
    //{ uint64_t chksum = 0; for (unsigned idx = 0; idx < 8; ++idx) chksum ^= cpu.regread32( idx ); std::cerr << '[' << std::hex << chksum << std::dec << ']'; }
      
    // if ((cpu.m_instcount % 0x1000000) == 0)
    //   { std::cerr << "Executed instructions: " << std::dec << cpu.m_instcount << " (" << std::hex << op->address << std::dec << ")"<< std::endl; }
  }
  
  dtlib::osprintf( std::cerr, "Executed instructions: %lld\n", cpu.m_instcount );
  
  return 0;
}
