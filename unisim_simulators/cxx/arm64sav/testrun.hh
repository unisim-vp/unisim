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

#ifndef __TESTRUN_HH__
#define __TESTRUN_HH__

#include <arch.hh>
#include <unisim/util/random/random.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <iosfwd>
#include <cstdlib>
#include <cassert>
#include <inttypes.h>

namespace test
{
  struct Random : public unisim::util::random::Random
  {
    Random() : unisim::util::random::Random(0,0,0,0) {}
    uint32_t generate32() { return Generate(); }
    uint64_t generate64() { return (uint64_t(generate32()) << 32) | generate32(); }
    uint64_t operand()
    {
      int64_t flips = int64_t(generate32()) << 48;
      uint64_t value = 0;
      for (int idx = 0; idx < 5; ++idx)
        {
          value |= generate64();
          value ^= flips >> 63;
          flips <<= 1;
        }
      return value;
    }
  };

  struct TestbedBase
  {
    TestbedBase( char const* seed, uint64_t* elems, uintptr_t count );

    void serial( std::ostream& sink, uint64_t* elems, uintptr_t count ) const;

    Random rng;
    uintptr_t counter;
  };
  
  template <uintptr_t COUNT>
  struct Testbed : public TestbedBase
  {
    Testbed( char const* seed ) : TestbedBase( seed, &buffer[0], COUNT ) {}
    
    void serial( std::ostream& sink ) { TestbedBase::serial( sink, &buffer[0], COUNT ); }
      
    uintptr_t head(uintptr_t idx=0) const { return (counter+idx) % COUNT; }
    
    void next()
    {
      buffer[head()] = rng.operand();
      counter += 1;
    }
    template <typename T> typename T::value_type const& select( T const& tests ) { return tests[counter%tests.size()]; }
    
    uint64_t buffer[COUNT];
    
    void load( uint64_t* ws, unsigned size ) const
    {
      if (size >= COUNT) throw 0;
      unsigned rnd_idx = head();
      for (unsigned idx = 0; idx < size; ++idx)
        {
          ws[idx] = buffer[rnd_idx];
          rnd_idx = (rnd_idx + 1) % COUNT;
        }
      
    }

    //   bool check( Workspace& ref, Workspace& sim, Arch& arch, fut_t const& fut ) const
    //   {
    //     for (unsigned idx = 0; idx < (ref.wordcount-1); ++idx)
    //       {
    //         if (ref.data[idx] != sim.data[idx])
    //           {
    //             Workspace src;
    //             load( src );
    //             return error(idx, src, ref, sim, arch, fut), false;
    //           }
    //       }
    //     return true;
    //   }

    //   template <uintptr_t N>
    //   fut_t const& select( fut_t const (&tests)[N] ) const
    //   {
    //     return tests[counter%N];
    //   }
    
    // private:
    // Workspace rnd;
    // uint64_t counter;
  
    // void error( unsigned field, Workspace const& src, Workspace const& ref, Workspace const& sim, Arch& arch, fut_t const& fut ) const
    // {
    //   std::cerr << "id: ";
    //   serial( std::cerr );
    //   std::cerr << "\n";
    //   std::cerr << "op: ";
    //   uint64_t addr = 0;
    //   for (unsigned idx = 1; idx < 32; ++idx)
    //     {
    //       uint32_t* insn = &((uint32_t*)fut.test)[idx];
    //       if (*insn == fut.insn)
    //         { addr = (uint64_t)(insn); break; }
    //     }
    //   if (not addr) { std::cerr << "<mimic>"; addr = (uint64_t)&fut.insn; }
    //   arch.DumpInsnInfo( std::cerr, addr );
    //   std::cerr << std::endl;

    //   for (unsigned idx = 0; idx < src.wordcount; ++idx)
    //     {
    //       std::cerr << std::setw(2)  << std::setfill(' ') << std::dec << idx << ": ";
    //       std::cerr << std::setw(16) << std::setfill(' ') << std::hex << src.data[idx] << " | ";
    //       char const* sym = (ref.data[idx] == sim.data[idx]) ? " = " : " ~ ";
    //       std::cerr << std::setw(16) << std::setfill(' ') << std::hex << ref.data[idx] << sym;
    //       sym = (idx == field) ? " <== " : "     ";
    //       std::cerr << std::setw(16) << std::setfill(' ') << std::hex << sim.data[idx];
    //       std::cerr << " (" << FPoint(ref.data[idx]) << ")\n";
    //       if (idx != field) continue;
    //       std::cerr << "ref:    " << Hexa(ref.data[idx]) << std::endl;
    //       std::cerr << "sim:    " << Hexa(sim.data[idx]) << std::endl;
    //     }
    // }
  };
  

  struct Arch
  {
    typedef uint8_t      U8;
    typedef uint16_t     U16;
    typedef uint32_t     U32;
    typedef uint64_t     U64;
    typedef int8_t       S8;
    typedef int16_t      S16;
    typedef int32_t      S32;
    typedef int64_t      S64;
    typedef bool         BOOL;
    typedef float        F32;
    typedef double       F64;

    typedef Arch DisasmState;
    
    static unsigned const VECTORCOUNT = 32;
    
    struct VUConfig
    {
      static unsigned const BYTECOUNT = 16;
      template <typename T> using TypeInfo = unisim::component::cxx::vector::VectorTypeInfo<T,0>;
      typedef U8 Byte;
    };
    
    typedef unisim::component::cxx::processor::arm::isa::arm64::Decoder<Arch> Decoder;
    typedef unisim::component::cxx::processor::arm::isa::arm64::Operation<Arch> Operation;
    typedef unisim::component::cxx::vector::VUnion<VUConfig> VectorView;
    
    void step_instruction();
    void run(review::Interface::testcode_t testcode, uint64_t* data);
    static void dont(char const*);
  
    void UndefinedInstruction( Operation const* insn );
    void UndefinedInstruction() { dont("undefined"); }

    struct SysReg
    {
      void dont() const { dont("system"); }
      void Write(int, int, int, int, int, Arch&, uint64_t) const { dont(); }
      uint64_t Read(int, int, int, int, int, Arch&) const { dont(); return 0; }
      void DisasmWrite(int, int, int, int, int, int, std::ostream&) const { dont(); }
      void DisasmRead(int, int, int, int, int, int, std::ostream&) const { dont(); }
    };
    static SysReg const* GetSystemRegister(int, int, int, int, int);
    void CheckSystemAccess(int) { dont("system"); }

    uint64_t GetGSR(unsigned id) const { return gpr[id]; }
    uint64_t GetGZR(unsigned id) const { return (id != 31) ? gpr[id] : 0; }
    void SetGSR(unsigned id, uint64_t val) { gpr[id] = val; }
    void SetGZR(unsigned id, uint64_t val) { if (id != 31) gpr[id] = val; }

    template <typename T>
    T vector_read(unsigned reg, unsigned sub)
    {
      return (vector_views[reg].GetConstStorage(&vector_data[reg], T(), VUConfig::BYTECOUNT))[sub];
    }

    U8  GetVU8 ( unsigned reg, unsigned sub ) { return vector_read<U8> (reg, sub); }
    U16 GetVU16( unsigned reg, unsigned sub ) { return vector_read<U16>(reg, sub); }
    U32 GetVU32( unsigned reg, unsigned sub ) { return vector_read<U32>(reg, sub); }
    U64 GetVU64( unsigned reg, unsigned sub ) { return vector_read<U64>(reg, sub); }
    S8  GetVS8 ( unsigned reg, unsigned sub ) { return vector_read<S8> (reg, sub); }
    S16 GetVS16( unsigned reg, unsigned sub ) { return vector_read<S16>(reg, sub); }
    S32 GetVS32( unsigned reg, unsigned sub ) { return vector_read<S32>(reg, sub); }
    S64 GetVS64( unsigned reg, unsigned sub ) { return vector_read<S64>(reg, sub); }

    template <typename T>
    void vector_write(unsigned reg, unsigned sub, T value )
    {
      (vector_views[reg].GetStorage(&vector_data[reg], value, VUConfig::BYTECOUNT))[sub] = value;
    }

    void SetVU8 ( unsigned reg, unsigned sub, U8  value ) { vector_write( reg, sub, value ); }
    void SetVU16( unsigned reg, unsigned sub, U16 value ) { vector_write( reg, sub, value ); }
    void SetVU32( unsigned reg, unsigned sub, U32 value ) { vector_write( reg, sub, value ); }
    void SetVU64( unsigned reg, unsigned sub, U64 value ) { vector_write( reg, sub, value ); }
    void SetVS8 ( unsigned reg, unsigned sub, S8  value ) { vector_write( reg, sub, value ); }
    void SetVS16( unsigned reg, unsigned sub, S16 value ) { vector_write( reg, sub, value ); }
    void SetVS32( unsigned reg, unsigned sub, S32 value ) { vector_write( reg, sub, value ); }
    void SetVS64( unsigned reg, unsigned sub, S64 value ) { vector_write( reg, sub, value ); }

    template <typename T>
    void vector_write(unsigned reg, T value )
    {
      *(vector_views[reg].GetStorage(&vector_data[reg][0], value, VUConfig::template TypeInfo<T>::bytecount)) = value;
    }

    void SetVU8 ( unsigned reg, U8 value )  { vector_write(reg, value); }
    void SetVU16( unsigned reg, U16 value ) { vector_write(reg, value); }
    void SetVU32( unsigned reg, U32 value ) { vector_write(reg, value); }
    void SetVU64( unsigned reg, U64 value ) { vector_write(reg, value); }
    void SetVS8 ( unsigned reg, S8 value )  { vector_write(reg, value); }
    void SetVS16( unsigned reg, S16 value ) { vector_write(reg, value); }
    void SetVS32( unsigned reg, S32 value ) { vector_write(reg, value); }
    void SetVS64( unsigned reg, S64 value ) { vector_write(reg, value); }

    void ClearHighV( unsigned reg, unsigned bytes )
    {
      vector_views[reg].Truncate(bytes);
    }

    void SetNZCV( uint32_t n, uint32_t z, uint32_t c, uint32_t v ) { nzcv = (n << 3) | (z << 2) | (c << 1) | (v << 0); }
    
    uint8_t GetNZCV() const { return nzcv; }
    uint8_t GetCarry() const { return (nzcv >> 1) & 1; }

    uint64_t GetPC() { return current_insn_addr; }
    uint64_t GetNPC() { return next_insn_addr; }

    enum branch_type_t { B_JMP = 0, B_CALL, B_RET };
    void BranchTo( uint64_t addr, branch_type_t branch_type ) { next_insn_addr = addr; }
    bool Test( bool cond ) { return cond; }

    void CallSupervisor( uint32_t imm );
    void CallHypervisor( uint32_t imm );
    
    template <typename T> T MemReadT(uint64_t addr) { return *reinterpret_cast<T const*>(addr); }
    uint64_t MemRead64(uint64_t addr) { return MemReadT<uint64_t>(addr); }
    uint32_t MemRead32(uint64_t addr) { return MemReadT<uint32_t>(addr); }
    uint16_t MemRead16(uint64_t addr) { return MemReadT<uint16_t>(addr); }
    uint8_t  MemRead8 (uint64_t addr) { return MemReadT<uint8_t> (addr); }

    template <typename T> void MemWriteT(uint64_t addr, T val) { *reinterpret_cast<T*>(addr) = val; }
    void MemWrite64(uint64_t addr, uint64_t val) { MemWriteT(addr, val); }
    void MemWrite32(uint64_t addr, uint32_t val) { MemWriteT(addr, val); }
    void MemWrite16(uint64_t addr, uint16_t val) { MemWriteT(addr, val); }
    void MemWrite8 (uint64_t addr, uint8_t  val) { MemWriteT(addr, val); }
    
    void     SetExclusiveMonitors( U64 addr, unsigned size );
    bool     ExclusiveMonitorsPass( U64 addr, unsigned size );
    void     ClearExclusiveLocal();
    void     PrefetchMemory( int, uint64_t ) { dont("prefetch"); }
    
    Decoder    decoder;
    Operation* current_instruction;

    uint64_t   gpr[32];
    uint32_t   nzcv;
    uint64_t   current_insn_addr, next_insn_addr;
    
    VectorView vector_views[VECTORCOUNT];
    uint8_t    vector_data[VECTORCOUNT][VUConfig::BYTECOUNT];
  };

} /* end of namespace test */


#endif // __TESTRUN_HH__
