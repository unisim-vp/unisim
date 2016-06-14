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
#include <unisim/util/os/linux_os/i386.hh>
#include <unisim/util/debug/simple_register.hh>
#include <unisim/kernel/logger/logger_server.hh>
#include <unisim/kernel/logger/logger.hh>
#include <iostream>

struct LinuxOS
  : public unisim::service::interfaces::LinuxOS
{
  unisim::kernel::logger::Logger logger;
  unisim::util::os::linux_os::Linux<uint32_t, uint32_t> linux_impl;
  
  bool exited;
  int app_ret_status;
  LinuxOS( unisim::service::interfaces::Registers *regs_if,
           unisim::service::interfaces::Memory<uint32_t> *mem_if,
           unisim::service::interfaces::MemoryInjection<uint32_t> *mem_inject_if
           )
    : unisim::service::interfaces::LinuxOS()
    , logger( "linux32" )
    , linux_impl( logger, regs_if, mem_if, mem_inject_if )
    , exited( false )
    , app_ret_status( -1 )
  {}
  
  void Setup( std::vector<std::string> const& simargs, std::vector<std::string> const& envs )
  {
    // Set up the different linuxlib parameters
    linux_impl.SetVerbose(false);
  
    if (not linux_impl.SetCommandLine(simargs))
      throw 0;
    
    // Set the linuxlib option to set the target environment with the
    // host environment
    linux_impl.SetApplyHostEnvironment(false);
    linux_impl.SetEnvironment(envs);
    
    // Set the binary that will be simulated in the target simulator
    if (not linux_impl.AddLoadFile( simargs[0].c_str() ))
      throw 0;
  
    // Set the system type of the target simulator (should be the same than the
    // binary)
    auto i386_target = new unisim::util::os::linux_os::I386TS<unisim::util::os::linux_os::Linux<uint32_t,uint32_t> >( linux_impl );
    linux_impl.SetTargetSystem(i386_target);
    
    linux_impl.SetEndianness( unisim::util::endian::E_LITTLE_ENDIAN );
    linux_impl.SetStackBase( 0x40000000UL );
    linux_impl.SetMemoryPageSize( 0x1000UL );
    linux_impl.SetUname("Linux" /* sysname */,
                     "localhost" /* nodename */,
                     "3.14.43-unisim" /* release */,
                     "#1 SMP Fri Mar 12 05:23:09 UTC 2010" /* version */,
                     "i386" /* machine */,
                     "localhost" /* domainname */);
    // linux_impl.SetStdinPipeFilename(stdin_pipe_filename.c_str());
    // linux_impl.SetStdoutPipeFilename(stdout_pipe_filename.c_str());
    // linux_impl.SetStderrPipeFilename(stderr_pipe_filename.c_str());

    // now it is time to try to run the initialization of the linuxlib
    if (not linux_impl.Load())
      throw 0;
  
    if (!linux_impl.SetupTarget())
      throw 0;
  
  }
  void ExecuteSystemCall( int id )
  {
    linux_impl.ExecuteSystemCall( id, exited, app_ret_status );
  }
};

struct Arch
  : public unisim::component::cxx::processor::intel::Arch
  , public unisim::service::interfaces::MemoryInjection<uint32_t>
  , public unisim::service::interfaces::Memory<uint32_t>
  , public unisim::service::interfaces::Registers

{
  typedef unisim::component::cxx::processor::intel::Arch::f64_t f64_t;
  
  Arch()
    : unisim::component::cxx::processor::intel::Arch()
    , unisim::service::interfaces::MemoryInjection<uint32_t>()
    , unisim::service::interfaces::Memory<uint32_t>()
    , unisim::service::interfaces::Registers()
    , linux_os(0)
  {
    for (int idx = 0; idx < 8; ++idx) {
      std::ostringstream regname;
      regname << unisim::component::cxx::processor::intel::DisasmRd(idx);
      regmap[regname.str()] = new unisim::util::debug::SimpleRegister<uint32_t>(regname.str(), &m_regs[idx]);
    }
    regmap["%eip"] = new unisim::util::debug::SimpleRegister<uint32_t>("%eip", &m_EIP);
    
    struct SegmentRegister : public unisim::service::interfaces::Register
    {
      SegmentRegister( Arch& _arch, std::string _name, unsigned _idx ) : arch(_arch), name(_name), idx(_idx) {}
      virtual const char *GetName() const { return "pc"; }
      virtual void GetValue( void* buffer ) const { *((uint16_t*)buffer) = arch.segregread( idx ); }
      virtual void SetValue( void const* buffer ) { arch.segregwrite( idx, *((uint16_t*)buffer) ); }
      virtual int  GetSize() const { return 2; }
      virtual void Clear() {} // Clear is meaningless for segment registers
      Arch&        arch;
      std::string  name;
      unsigned     idx;
    };

    for (int idx = 0; idx < 6; ++idx) {
      std::ostringstream regname;
      regname << unisim::component::cxx::processor::intel::DisasmS(idx);
      regmap[regname.str()] = new SegmentRegister(*this, regname.str(), idx);
    }
    
    for (int idx = 0; idx < 4; ++idx) {
      std::ostringstream regname;
      regname << "@gdt[" << idx<< "].base";
      regmap[regname.str()] = new unisim::util::debug::SimpleRegister<uint32_t>(regname.str(), &m_gdt_bases[idx]);
    }
    
    struct X87Register : public unisim::service::interfaces::Register
    {
      X87Register( std::string _name, Arch& _arch, unsigned _idx ) : name(_name), arch(_arch), idx(_idx) {}
      char const* GetName() const { return name.c_str(); }
      void GetValue(void *buffer) const { *((f64_t*)buffer) = arch.fread( idx ); }
      void SetValue(const void *buffer) { arch.fwrite( idx, *((f64_t*)buffer) ); }
      int GetSize() const { return 8; }
      std::string name;
      Arch& arch;
      unsigned idx;
    };
    for (int idx = 0; idx < 8; ++idx) {
      std::ostringstream regname;
      regname << unisim::component::cxx::processor::intel::DisasmFPR(idx);
      regmap[regname.str()] = new X87Register( regname.str(), *this, idx );
    }
  }
  ~Arch()
  {
    for (auto reg : regmap)
      delete reg.second;
  }
  
  void SetLinuxOS( unisim::service::interfaces::LinuxOS* _linux_os ) { linux_os = _linux_os; }
  unisim::service::interfaces::LinuxOS* GetLinuxOS() { return linux_os; }
  
  unisim::service::interfaces::LinuxOS* linux_os;
  std::map<std::string,unisim::service::interfaces::Register*> regmap;
  
  using unisim::component::cxx::processor::intel::Arch::m_mem;
  // unisim::service::interfaces::Memory<uint32_t>
  void Reset() {}
  bool ReadMemory(uint32_t addr, void* buffer, uint32_t size ) { m_mem.read( (uint8_t*)buffer, addr, size ); return true; }
  bool WriteMemory(uint32_t addr, void const* buffer, uint32_t size) { m_mem.write( addr, (uint8_t*)buffer, size ); return true; }
  // unisim::service::interfaces::Registers
  unisim::service::interfaces::Register* GetRegister(char const* name)
  {
    auto reg = regmap.find( name );
    return (reg == regmap.end()) ? 0 : reg->second;
  }
  void ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
  {
    // General purpose registers
    scanner.Append( GetRegister( "%eax" ) );
    scanner.Append( GetRegister( "%ecx" ) );
    scanner.Append( GetRegister( "%edx" ) );
    scanner.Append( GetRegister( "%ebx" ) );
    scanner.Append( GetRegister( "%esp" ) );
    scanner.Append( GetRegister( "%ebp" ) );
    scanner.Append( GetRegister( "%esi" ) );
    scanner.Append( GetRegister( "%edi" ) );
    // Program counter
    scanner.Append( GetRegister( "%eip" ) );
    // Segments
    scanner.Append( GetRegister( "%es" ) );
    scanner.Append( GetRegister( "%cs" ) );
    scanner.Append( GetRegister( "%ss" ) );
    scanner.Append( GetRegister( "%ds" ) );
    scanner.Append( GetRegister( "%fs" ) );
    scanner.Append( GetRegister( "%gs" ) );
    // FP registers
    scanner.Append( GetRegister( "%st" ) );
    scanner.Append( GetRegister( "%st(1)" ) );
    scanner.Append( GetRegister( "%st(2)" ) );
    scanner.Append( GetRegister( "%st(3)" ) );
    scanner.Append( GetRegister( "%st(4)" ) );
    scanner.Append( GetRegister( "%st(5)" ) );
    scanner.Append( GetRegister( "%st(6)" ) );
    scanner.Append( GetRegister( "%st(7)" ) );
  }
  // unisim::service::interfaces::MemoryInjection<ADDRESS>
  bool InjectReadMemory(uint32_t addr, void *buffer, uint32_t size) { m_mem.read( (uint8_t*)buffer, addr, size ); return true; }
  bool InjectWriteMemory(uint32_t addr, void const* buffer, uint32_t size) { m_mem.write( addr, (uint8_t*)buffer, size ); return true; }
  
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

  std::vector<std::string> envs;
  envs.push_back( "LANG=C" );
  
  Arch cpu;
  LinuxOS linux32( &cpu, &cpu, &cpu );
  cpu.SetLinuxOS( &linux32 );
  
  linux32.Setup( simargs, envs );
  
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
  
  cpu.m_disasm = false;
  
  // Loading image
  std::cerr << "*** Loading elf image: " << simargs[0] << " ***" << std::endl;
  
  typedef unisim::component::cxx::processor::intel::Operation Operation;
  std::cerr << "\n*** Run ***" << std::endl;
  
  while (not linux32.exited)
    {
      Operation* op = cpu.fetch();
      // op->disasm( std::cerr );
      // std::cerr << std::endl;
      asm volatile ("operation_execute:");
      op->execute( cpu );
      //{ uint64_t chksum = 0; for (unsigned idx = 0; idx < 8; ++idx) chksum ^= cpu.regread32( idx ); std::cerr << '[' << std::hex << chksum << std::dec << ']'; }
      
      // if ((cpu.m_instcount % 0x1000000) == 0)
      //   { std::cerr << "Executed instructions: " << std::dec << cpu.m_instcount << " (" << std::hex << op->address << std::dec << ")"<< std::endl; }
    }
  
  std::cerr << "Program exited with status:" << linux32.app_ret_status << std::endl;
  //  dtlib::osprintf( std::cerr, "Executed instructions: %lld\n", cpu.m_instcount );
  
  
  return 0;
}
