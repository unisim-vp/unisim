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
      void dont() const { Arch::dont("system"); }
      void Write(int, int, int, int, int, Arch&, U64) const { dont(); }
      U64 Read(int, int, int, int, int, Arch&) const { dont(); return 0; }
      void DisasmWrite(int, int, int, int, int, int, std::ostream&) const { dont(); }
      void DisasmRead(int, int, int, int, int, int, std::ostream&) const { dont(); }
    };
    static SysReg const* GetSystemRegister(int, int, int, int, int) { dont("system"); return 0; }
    void CheckSystemAccess(int) { dont("system"); }

    U64 GetGSR(unsigned id) const { return gpr[id]; }
    U64 GetGZR(unsigned id) const { return (id != 31) ? gpr[id] : 0; }
    void SetGSR(unsigned id, U64 val) { gpr[id] = val; }
    void SetGZR(unsigned id, U64 val) { if (id != 31) gpr[id] = val; }

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

    U64 GetPC() { return current_insn_addr; }
    U64 GetNPC() { return next_insn_addr; }

    enum branch_type_t { B_JMP = 0, B_CALL, B_RET };
    void BranchTo( U64 addr, branch_type_t branch_type ) { next_insn_addr = addr; }
    bool Test( bool cond ) { return cond; }

    void CallSupervisor( uint32_t imm ) { dont("system"); }
    void CallHypervisor( uint32_t imm ) { dont("system"); }

    template <typename T> T MemReadT(U64 addr) { return *reinterpret_cast<T const*>(addr); }
    U64 MemRead64(U64 addr) { return MemReadT<U64>(addr); }
    U32 MemRead32(U64 addr) { return MemReadT<uint32_t>(addr); }
    U16 MemRead16(U64 addr) { return MemReadT<uint16_t>(addr); }
    U8  MemRead8 (U64 addr) { return MemReadT<uint8_t> (addr); }

    template <typename T> void MemWriteT(U64 addr, T val) { *reinterpret_cast<T*>(addr) = val; }
    void MemWrite64(U64 addr, U64 val) { MemWriteT(addr, val); }
    void MemWrite32(U64 addr, U32 val) { MemWriteT(addr, val); }
    void MemWrite16(U64 addr, U16 val) { MemWriteT(addr, val); }
    void MemWrite8 (U64 addr, U8  val) { MemWriteT(addr, val); }

    void     SetExclusiveMonitors( U64 addr, unsigned size ) { dont("mp"); }
    bool     ExclusiveMonitorsPass( U64 addr, unsigned size ) { dont("mp"); return false; }
    void     ClearExclusiveLocal() { dont("mp"); }
    void     PrefetchMemory( int, U64 ) { dont("prefetch"); }

    Decoder    decoder;
    Operation* current_instruction;

    U64   gpr[32];
    uint32_t   nzcv;
    U64   current_insn_addr, next_insn_addr;

    VectorView vector_views[VECTORCOUNT];
    uint8_t    vector_data[VECTORCOUNT][VUConfig::BYTECOUNT];
  };

} /* end of namespace test */


#endif // __TESTRUN_HH__
