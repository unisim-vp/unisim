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

#ifndef __RISCVEMU_ARCHITECTURE_HH__
#define __RISCVEMU_ARCHITECTURE_HH__

#include <unisim/component/cxx/memory/sparse/memory.hh>
#include <unisim/component/cxx/processor/riscv/isa_rv64.tcc>
#include <unisim/component/cxx/processor/riscv/isa/disasm.hh>
#include <unisim/component/cxx/processor/opcache/opcache.hh>
#include <unisim/util/debug/simple_register.hh>
#include <unisim/service/interfaces/memory_injection.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/registers.hh>
#include <linuxsystem.hh>
#include <inttypes.h>

struct Arch
  : public unisim::service::interfaces::MemoryInjection<uint64_t>
  , public unisim::service::interfaces::Memory<uint64_t>
  , public unisim::service::interfaces::Registers

{
  typedef unisim::component::cxx::processor::riscv::isa::rv64::CodeType CodeType;
  typedef unisim::component::cxx::processor::riscv::isa::rv64::Operation<Arch> Operation;
  typedef unisim::component::cxx::processor::riscv::isa::rv64::Decoder<Arch> RV64Decoder;
  typedef unisim::component::cxx::processor::opcache::OpCache<RV64Decoder> Decoder;

  typedef bool  BOOL;
  typedef uint8_t  U8;
  typedef uint16_t U16;
  typedef uint32_t U32;
  typedef uint64_t U64;
  typedef int8_t  S8;
  typedef int16_t S16;
  typedef int32_t S32;
  typedef int64_t S64;

  typedef U64 UREG;
  typedef S64 SREG;
  
  enum branch_type_t { B_JMP = 0, B_CALL, B_RET, B_EXC, B_DBG, B_RFE };
  
  Arch()
    : unisim::service::interfaces::MemoryInjection<uint64_t>()
    , unisim::service::interfaces::Memory<uint64_t>()
    , unisim::service::interfaces::Registers()
    , decoder(), linux_os(0), regmap()
    , mem(), gprs(), current_insn_addr(), next_insn_addr()
    , disasm(false)
  {
    for (int idx = 1; idx < 32; ++idx)
      {
        unisim::service::interfaces::Register* reg = 0;
        {
          std::ostringstream buf;
          buf << unisim::component::cxx::processor::riscv::isa::PrintGPR(idx);
          regmap[buf.str()] = reg = new unisim::util::debug::SimpleRegister<uint64_t>(buf.str(), &gprs[idx]);
        }
        {
          std::ostringstream buf;
          buf << 'x' << std::dec << idx;
          regmap[buf.str()] = reg;
        }
      }
    
    struct ProgramCounter : public unisim::service::interfaces::Register
    {
      ProgramCounter( Arch& _core ) : core(_core) {}
      virtual const char *GetName() const override { return "pc"; }
      virtual void GetValue( void* buffer ) const override { *((uint64_t*)buffer) = core.current_insn_addr; }
      virtual void SetValue( void const* buffer ) override
      {
        uint64_t address = *((uint64_t*)buffer);
        core.Branch(address, core.B_DBG);
      }
      virtual int  GetSize() const override { return 8; }
      virtual void Clear() { /* Clear is meaningless for PC */ }
      Arch&        core;
    };
    regmap["pc"] = new ProgramCounter( *this );

  }
  
  ~Arch()
  {
    for (auto& reg : regmap)
      if (reg.first.compare(reg.second->GetName()))
        reg.second = 0;
    for (auto& reg : regmap)
      delete reg.second;
  }
  
  void SetLinuxOS( unisim::service::interfaces::LinuxOS* _linux_os )
  {
    linux_os = _linux_os;
  }

  Decoder decoder;
  unisim::service::interfaces::LinuxOS* linux_os;
  std::map<std::string, unisim::service::interfaces::Register*> regmap;
  
  struct ClearMemSet {
    void operator() ( uint8_t* base, uintptr_t size ) const {
      for (uintptr_t idx = 0; idx < size; ++idx) base[idx] = 0;
    }
  };
  typedef typename unisim::component::cxx::memory::sparse::Memory<uint64_t,12,12,ClearMemSet> Memory;
  Memory                      mem;

  void     SetGPR(unsigned idx, U64 value) { if (idx) gprs[idx] = value; }
  U64      GetGPR(unsigned idx) { return gprs[idx]; }
  
  uint64_t                    gprs[32];
  
  bool    Test( bool cond ) { return cond; }
  U64     GetPC() { return current_insn_addr; }
  U64     GetNPC() { return next_insn_addr; }
  void    Branch(U64 target, branch_type_t) { next_insn_addr = target; }
  uint64_t   current_insn_addr, next_insn_addr;
  
  // unisim::service::interfaces::Memory<uint64_t>
  void ResetMemory() {}
  bool ReadMemory(uint64_t addr, void* buffer, uint32_t size) { mem.read( (uint8_t*)buffer, addr, size ); return true; }
  bool WriteMemory(uint64_t addr, void const* buffer, uint32_t size) { mem.write( addr, (uint8_t*)buffer, size ); return true; }
  // unisim::service::interfaces::Registers
  unisim::service::interfaces::Register* GetRegister(char const* name)
  {
    auto reg = regmap.find( name );
    return (reg == regmap.end()) ? 0 : reg->second;
  }
  void ScanRegisters(unisim::service::interfaces::RegisterScanner& scanner)
  {
    // General purpose registers
  }
  // unisim::service::interfaces::MemoryInjection<ADDRESS>
  bool InjectReadMemory(uint64_t addr, void *buffer, uint32_t size) { mem.read( (uint8_t*)buffer, addr, size ); return true; }
  bool InjectWriteMemory(uint64_t addr, void const* buffer, uint32_t size) { mem.write( addr, (uint8_t*)buffer, size ); return true; }
  
  bool IsBigEndian() { return false; }

  template <typename U>
  U
  MemRead( U64 addr, U value )
  {
    uint8_t buffer[sizeof (U)];
    mem.read( &buffer[0], addr, sizeof buffer );
    U res = 0;
    for (int idx = sizeof buffer; --idx >= 0;)
      res = (res << 8) | buffer[idx];
    return res;
  }

  template <typename U>
  void
  MemWrite( U64 addr, U value )
  {
    uint8_t buffer[sizeof (U)];
    for (unsigned idx = 0; idx < sizeof buffer; ++idx)
      { buffer[idx] = value; value >>= 8; }
    mem.write( addr, &buffer[0], sizeof buffer );
  }

  void AtomicBegin(uint64_t addr) { }
  bool AtomicUpdate(uint64_t addr) { return true; }

  CodeType ReadInsn(uint64_t addr)
  {
    uint8_t buffer[4];
    mem.read( (uint8_t*)&buffer[0], addr, 4 );
    return (buffer[0] << 0) | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24);
  }

  void ECall()
  {
    if (not linux_os)
      { throw std::logic_error( "No linux OS emulation connected" ); }
    linux_os->ExecuteSystemCall(0);
  }

  void debug();

  Operation* StepInstruction();

  void UndefinedInstruction( Operation const* insn )
  {
    insn->disasm(std::cerr << "0x" << std::hex << insn->GetAddr() << ": ");
    std::cerr << " (" << insn->GetName() << ")" << std::endl;
    struct Unimplemented {};
    throw Unimplemented();
  }

  void  MemDump64(uint64_t addr);

  bool disasm;
};

#endif // __RISCVEMU_ARCHITECTURE_HH__
