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
#include <linuxsystem.hh>
#include <dtlib/loader.hh>
#include <dtlib/misc.hh>
#include <simulator.hh>
#include <iostream>

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

  Simulator simulator( argc, argv );

  typedef unisim::component::cxx::processor::intel::Arch Arch;
  Arch cpu;

  switch (simulator.Setup())
    {
    case unisim::kernel::service::Simulator::ST_ERROR:
      std::cerr << "ERROR: Can't start simulation because of previous errors" << std::endl;
      return 1;
      
    case unisim::kernel::service::Simulator::ST_OK_DONT_START:
      std::cerr << "Successfully configured the simulator." << std::endl;
      return 0;
      
    case unisim::kernel::service::Simulator::ST_WARNING:
      std::cerr << "WARNING: problems detected during setup, trying anyway." << std::endl;
    case unisim::kernel::service::Simulator::ST_OK_TO_START:
      std::cerr << "Starting simulation." << std::endl;
      break;
    }
  
  unisim::kernel::service::VariableBase* cmd_args = simulator.FindVariable("cmd-args");
  std::vector<std::string> args;
  
  unsigned cmd_args_length = cmd_args->GetLength();
  if (cmd_args_length == 0) {
    std::cerr << "Simulation command line empty." << std::endl;
    return 1;
  }
  for (unsigned idx = 0; idx < cmd_args_length; ++idx)
    args.push_back( ((std::string)(*cmd_args)[idx]) );
  
  std::cerr << "arguments";
  
  for (std::string const& arg : args)
    {
      std::cerr << "  " << arg << '\n';
    }
  
  cpu.m_linux_system.m_sink.redirect( std::cerr );
  cpu.m_events.redirect( std::cerr );
  cpu.m_disasm = false;
  
  // Loading image
  std::cerr << "*** Loading elf image: " << args[0] << " ***" << std::endl;
  
  {
    dtlib::ELF::Image elfimage( args[0].c_str() );
    
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
    cpu.m_linux_system.static_setup( cpu.target(), eiroot, args, envs );
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
