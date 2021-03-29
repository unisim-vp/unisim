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

#ifndef __ARM32SAV_RUNNER_HH__
#define __ARM32SAV_RUNNER_HH__

#include <test.hh>
#include <top_arm32.hh>
#include <top_thumb.hh>
#include <unisim/component/cxx/processor/arm/psr.hh>
#include <unisim/component/cxx/processor/arm/models.hh>
#include <unisim/component/cxx/processor/arm/cpu.hh>
#include <unisim/component/cxx/processor/arm/hostfloat.hh>
#include <iosfwd>
#include <cstdlib>
#include <cassert>
#include <inttypes.h>

struct Runner : public unisim::component::cxx::processor::arm::CPU<unisim::component::cxx::processor::arm::hostfloat::FP,Runner>
{
  typedef unisim::component::cxx::processor::arm::CPU<unisim::component::cxx::processor::arm::hostfloat::FP,Runner> CPU;
  struct Config
  {
    //=====================================================================
    //=                  ARM architecture model description               =
    //=====================================================================
    
    // Following a standard armv7 configuration
    static uint32_t const model = unisim::component::cxx::processor::arm::ARMV7;
    static bool const     insns4T = true;
    static bool const     insns5E = true;
    static bool const     insns5J = true;
    static bool const     insns5T = true;
    static bool const     insns6  = true;
    static bool const     insnsRM = true;
    static bool const     insnsT2 = true;
    static bool const     insns7  = true;
    static bool const     hasVFP  = true;
    static bool const     hasAdvSIMD = false;
  };

  struct Arm32 : public unisim::component::cxx::processor::arm::isa::arm32::Decoder<Runner>
  {
    typedef unisim::component::cxx::processor::arm::isa::arm32::Operation<Runner> Operation;

    Arm32();
    ~Arm32();

    Operation* decode(uint32_t addr, uint32_t code);
  };

  struct Thumb2 : public unisim::component::cxx::processor::arm::isa::thumb::Decoder<Runner>
  {
    typedef unisim::component::cxx::processor::arm::isa::thumb::Operation<Runner> Operation;

    Thumb2();
    ~Thumb2();

    Operation* decode(uint32_t addr, uint32_t code);
  };
  
  Runner( char const* name );
  ~Runner();
  
  void  run(Interface::testcode_t code, uint32_t*);
  void  step_instruction();

  virtual void Sync() {}
  unisim::util::endian::endian_type endianess();

  template <typename T> T MemReadT(U32 addr, bool aligned)
  {
    if (aligned and (addr % sizeof(T)))
      dont("data alignment");
    T res = *reinterpret_cast<T const*>(addr);
    res = Target2Host(endianess(), res);
    return res;
  }
  //  U64 MemRead64(U32 addr) { return MemReadT<U64>(addr); }
  U32 MemURead32(U32 addr) { return MemReadT<U32>(addr, false); }
  U16 MemURead16(U32 addr) { return MemReadT<U16>(addr, false); }
  U32 MemRead32(U32 addr) { return MemReadT<U32>(addr, true); }
  U16 MemRead16(U32 addr) { return MemReadT<U16>(addr, true); }
  U8  MemRead8 (U32 addr) { return MemReadT<U8> (addr, true); }

  template <typename T> void MemWriteT(U32 addr, T val, bool aligned)
  {
    if (aligned and (addr % sizeof(T)))
      dont("data alignment");
    val = Target2Host(endianess(), val);
    *reinterpret_cast<T*>(addr) = val;
  }
  //  void MemWrite64(U32 addr, U64 val) { MemWriteT(addr, val); }
  void MemUWrite32(U32 addr, U32 val) { MemWriteT(addr, val, false); }
  void MemUWrite16(U32 addr, U16 val) { MemWriteT(addr, val, false); }
  void MemWrite32(U32 addr, U32 val) { MemWriteT(addr, val, true); }
  void MemWrite16(U32 addr, U16 val) { MemWriteT(addr, val, true); }
  void MemWrite8 (U32 addr, U8  val) { MemWriteT(addr, val, true); }

  static void dont(char const* reason);
  template <typename T> static T dontread(char const* reason) { dont(reason); return T(); }

  void WaitForInterrupt() { dont("system"); }
  void SWI( uint32_t imm ) { dont("system"); }
  void BKPT( uint32_t imm ) { dont("system"); }
  void UnpredictableInsnBehaviour() { dont("unpredictable"); }
  template <typename OP> void UndefinedInstruction( OP* op ) { dont("undefined"); }
  void CallSupervisor( uint32_t imm ) { dont("system"); }
  void UnpredictableIf( BOOL const& condition ) { dont("unpredictable"); }
  bool IntegerZeroDivide( BOOL const& condition ) { return false; }

  void     SetExclusiveMonitors( U32 addr, unsigned size ) { dont("mp"); }
  bool     ExclusiveMonitorsPass( U32 addr, unsigned size ) { dont("mp"); return false; }
  void     ClearExclusiveLocal() { dont("mp"); }
  
  struct CP15Reg
  {
    void CheckPermissions( uint8_t, uint8_t, uint8_t, uint8_t, CPU&, bool ) const { dont("cp15"); }
    U32  Read( uint8_t, uint8_t, uint8_t, uint8_t, CPU& ) const { return dontread<U32>("cp15"); }
    void Write( uint8_t, uint8_t, uint8_t, uint8_t, CPU&, U32 const& ) const { dont("cp15"); }
    void Describe( uint8_t crn, uint8_t op1, uint8_t crm, uint8_t op2, std::ostream& sink ) const
    {
      sink << "CR15{crn=" << crn << ", op1=" << op1 << ", crm=" << crm << ", op2=" << op2 << "}";
    }
  };

  static CP15Reg* CP15GetRegister(uint8_t, uint8_t, uint8_t, uint8_t) { static CP15Reg _; return &_; }
  
  Arm32 arm32iset;
  Thumb2 thumb2iset;
};

#endif // __ARM32SAV_RUNNER_HH__
