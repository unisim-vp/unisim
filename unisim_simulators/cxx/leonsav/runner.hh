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

#ifndef __LEONSAV_RUNNER_HH__
#define __LEONSAV_RUNNER_HH__

#include <test.hh>
#include <unisim/component/cxx/processor/sparc/isa_sv8.hh>
#include <unisim/component/cxx/processor/sparc/isa/sv8/arch.hh>
#include <unisim/component/cxx/processor/sparc/asi.hh>
#include <unisim/component/cxx/processor/sparc/trap.hh>
#include <iosfwd>
// #include <cstdlib>
// #include <cassert>
#include <inttypes.h>

struct Runner : public unisim::component::cxx::processor::sparc::isa::sv8::Arch<Runner>
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

  typedef unisim::component::cxx::processor::sparc::isa::sv8::Operation<Runner> Operation;

  typedef unisim::component::cxx::processor::sparc::ASI ASI;
  typedef unisim::component::cxx::processor::sparc::Trap_t Trap_t;

  void step_instruction();
  std::unique_ptr<Operation> decode(uint32_t insn_addr, uint32_t code);
  void run(Interface::testcode_t testcode, uint32_t* data);
  static void dont(char const*);

  Runner();

  /***************************/
  /*** Architectural state ***/
  /***************************/

  struct Conditions
  {
    template <unsigned BIT> using Bit = unisim::util::truth_table::InBit<uint16_t,BIT>;
    template <unsigned CST> using All = unisim::util::truth_table::Always<uint16_t,CST>;

    auto N() const { return Bit<3>(); }
    auto Z() const { return Bit<2>(); }
    auto V() const { return Bit<1>(); }
    auto C() const { return Bit<0>(); }
    auto F() const { return All<false>(); }
    auto T() const { return All<true>(); }

    Conditions(Runner& _runner) : runner(_runner) {} Runner& runner;

    template <class TT> bool Eval( TT const& tt ) { unsigned nzcv = (runner.psr >> 20) & 15; return (tt.tt >> nzcv) & 1; }
  };

  struct FConditions
  {
    FConditions(Runner& _runner) : runner(_runner) {} Runner& runner;
    bool Eval( bool cond ) const { return cond; }
    BOOL F() const { return false; }
    BOOL T() const { return true; }
    BOOL E() const { dont("floating-point"); return false; }
    BOOL G() const { dont("floating-point"); return false; }
    BOOL L() const { dont("floating-point"); return false; }
    BOOL U() const { dont("floating-point"); return false; }
  };

  void UndefinedInstruction( Operation const& op ) { UndefinedInstruction(); }
  void UndefinedInstruction() { dont("undefined"); }

  bool IsSuper() const { return psr >> 7 & 1; }
  bool IsSuper() { return (psr & (1ul << 7)); }
  ASI  rqasi() const { return (psr & (1ul << 7)) ? ASI::supervisor_data : ASI::user_data; }
  ASI  insn_asi() { return (psr & (1ul << 7)) ? ASI::supervisor_instruction : ASI::user_instruction; }

  void Abort( Trap_t::TrapType_t trap ) { dont("trap"); }
  void StopFetch() { dont("trap"); }
  void SWTrap(U32) { dont("trap"); }

  // void gregtouch(unsigned id, bool write);
  // U32  gregread(unsigned id) { gregtouch(id, false); return gprs[id]; }
  // void gregwrite(unsigned id, U32 const& value) { gregtouch(id, true); gprs[id] = value; }

  void SetGPR( unsigned id, U32 const& value ) { if (id != 0) gprs[id] = value; }
  U32  GetGPR( unsigned id ) { if (id != 0) return gprs[id]; return U32(0); }
  U32  GetY() { return y; }
  void SetY(U32 const& value) { y = value; }
  
  void SetNZVC( uint32_t n, uint32_t z, uint32_t v, uint32_t c )
  {
    psr = (psr & ~0xf00000) | (n << 23) | (z << 22) | (v << 21) | (c << 20);
  }
  
  BOOL GetCarry() { return psr >> 20 & 1; }
  BOOL GetN() { return psr >> 23 & 1; }
  BOOL GetV() { return psr >> 21 & 1; }
  BOOL GetET() { dont("trap"); return BOOL(); }
  void SetET(BOOL) { dont("trap"); }
  BOOL GetS() { dont("trap"); return BOOL(); }
  void SetS(BOOL) { dont("trap"); }
  BOOL GetPS() { dont("trap"); return BOOL(); }
  void SetPS(BOOL) { dont("trap"); }
  

  void SetF32( unsigned id, F32 const& value ) { dont("floating-point"); }
  void SetF64( unsigned id, F64 const& value ) { dont("floating-point"); }
  void SetS32( unsigned id, S32 const& value ) { dont("floating-point"); }
  F32  GetF32( unsigned id ) { dont("floating-point"); return F32(); }
  F64  GetF64( unsigned id ) { dont("floating-point"); return F64(); }
  S32  GetS32( unsigned id ) { dont("floating-point"); return S32(); }
  void SetFCC( BOOL const& l, BOOL const& g ) { dont("floating-point"); }
  U32  GetFSR() { dont("floating-point"); return U32(); }
  void SetFSR( U32 const& ) { dont("floating-point"); }

  template <typename T> T MemRead(T const&, ASI asi, uint32_t addr) { return *reinterpret_cast<T const*>(addr); }
  template <typename T> void MemWrite(ASI asi, uint32_t addr, T const& val) { *reinterpret_cast<T*>(addr) = val; }

  template <typename T> T FMemRead(T const&, ASI asi, U32 const& addr) { dont("floating-point"); return T(); }
  template <typename T> void FMemWrite(ASI asi, U32 const& addr, T const&) { dont("floating-point"); }

  bool Test( bool cond ) { return cond; }

  bool InvalidWindow(int) { return false; }
  void RotateWindow(int) { dont("register-window"); }

  enum branch_type_t { B_JMP = 0, B_CALL, B_RET };
  U32  GetPC() { return pc; }
  void DelayBranch(U32 target, branch_type_t) { nnpc = target; }
  void SetAnnul(bool annul) { dont("annul"); }

  bool asr_perm(unsigned) { return false; }
  U32 rdasr(unsigned) { dont("asr"); return U32(); }
  void wrasr(unsigned, U32) { dont("asr"); }

  U32 GetWIM() { dont("wim"); return U32(); }
  U32 GetTBR() { dont("tbr"); return U32(); }
  void SetWIM(U32) { dont("wim"); }
  void SetTBR(U32) { dont("tbr"); }
  unsigned nwindows() { return 31; }
  
  U32 GetPSR() { return psr; }
  void SetPSR(U32 value) { psr = value; }
  
  U32 gprs[32];
  U32 y;
  uint32_t psr;
  uint32_t nnpc, npc, pc;

  Operation* current_instruction;
  bool disasm;
};

#endif // __LEONSAV_RUNNER_HH__
