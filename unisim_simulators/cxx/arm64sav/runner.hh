/*
 *  Copyright (c) 2019-2023,
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

#ifndef __ARM64SAV_RUNNER_HH__
#define __ARM64SAV_RUNNER_HH__

#include <test.hh>
#include <arm64sav.hh>
#include <unisim/component/cxx/processor/arm/regs64/cpu.hh>
#include <unisim/util/arithmetic/bitfield.hh>
#include <iosfwd>
#include <cstdlib>
#include <cassert>
#include <inttypes.h>

struct RunnerTypes
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
  typedef _Float16     F16;
  typedef float        F32;
  typedef double       F64;

  template <typename T>
  using VectorTypeInfo = unisim::component::cxx::vector::VectorTypeInfo<T,0>;
  using VectorByte = U8;
  using VectorByteShadow = U8;
};

struct Runner
  : public RunnerTypes
  , unisim::component::cxx::processor::arm::regs64::CPU<Runner, RunnerTypes>
{
  typedef Runner DisasmState;

  typedef unisim::component::cxx::processor::arm::isa::arm64::Decoder<Runner> Decoder;
  typedef unisim::component::cxx::processor::arm::isa::arm64::Operation<Runner> Operation;

  enum ReportAccess { report_none = 0, report_simd_access = report_none, report_gzr_access = report_none, report_gsr_access = report_none, report_nzcv_access = report_none };
  void report(ReportAccess, unsigned, bool) {}

  void step_instruction();
  std::unique_ptr<Operation> decode(uint64_t insn_addr, uint32_t code);
  void run(Interface::testcode_t testcode, uint64_t* data);
  static void dont(char const*);

  void UndefinedInstruction( Operation const* insn );
  void UndefinedInstruction() { dont("undefined"); }

  struct SysReg
  {
    virtual ~SysReg() {}
    virtual void Write(int, int, int, int, int, int, Runner&, U64) const = 0;
    virtual U64 Read(int, int, int, int, int, int, Runner&) const = 0;
    virtual void DisasmWrite(int, int, int, int, int, int, std::ostream&) const = 0;
    virtual void DisasmRead(int, int, int, int, int, int, std::ostream&) const = 0;
    void dont() const { Runner::dont("system"); }
  };
  static SysReg const* GetSystemRegister(int op0, int op1, int crn, int crm, int op2);
  void CheckSystemAccess(int) {}

  U8  GetTVU8(unsigned reg0, unsigned elements, unsigned regs, U8 index, U8 oob_value);

  void SetNZCV( uint32_t n, uint32_t z, uint32_t c, uint32_t v ) { nzcv = (n << 3) | (z << 2) | (c << 1) | (v << 0); }

  uint8_t GetNZCV() const { return nzcv; }
  uint8_t GetCarry() const { return (nzcv >> 1) & 1; }

  U64 GetPC() { return current_insn_addr; }
  U64 GetNPC() { return next_insn_addr; }

  void BranchTo( U64 addr, branch_type_t branch_type ) { next_insn_addr = addr; }
  bool Test( bool cond ) { return cond; }

  void SoftwareBreakpoint( uint32_t imm ) { dont("system"); }
  void CallSupervisor( uint32_t imm ) { dont("system"); }
  void CallHypervisor( uint32_t imm ) { dont("system"); }
  void ExceptionReturn() { dont("system"); }
  void CheckSPAlignment(U64 addr) {}

  template <typename T> T MemReadT(U64 addr) { return *reinterpret_cast<T const*>(addr); }
  U64 MemRead64(U64 addr) { return MemReadT<U64>(addr); }
  U32 MemRead32(U64 addr) { return MemReadT<uint32_t>(addr); }
  U16 MemRead16(U64 addr) { return MemReadT<uint16_t>(addr); }
  U8  MemRead8 (U64 addr) { return MemReadT<uint8_t> (addr); }
  U64 MemReadUnprivileged64(U64 addr) { return MemReadT<U64>(addr); }
  U32 MemReadUnprivileged32(U64 addr) { return MemReadT<uint32_t>(addr); }
  U16 MemReadUnprivileged16(U64 addr) { return MemReadT<uint16_t>(addr); }
  U8  MemReadUnprivileged8 (U64 addr) { return MemReadT<uint8_t> (addr); }

  template <typename T> void MemWriteT(U64 addr, T val) { *reinterpret_cast<T*>(addr) = val; }
  void MemWrite64(U64 addr, U64 val) { MemWriteT(addr, val); }
  void MemWrite32(U64 addr, U32 val) { MemWriteT(addr, val); }
  void MemWrite16(U64 addr, U16 val) { MemWriteT(addr, val); }
  void MemWrite8 (U64 addr, U8  val) { MemWriteT(addr, val); }
  void MemWriteUnprivileged64(U64 addr, U64 val) { MemWriteT(addr, val); }
  void MemWriteUnprivileged32(U64 addr, U32 val) { MemWriteT(addr, val); }
  void MemWriteUnprivileged16(U64 addr, U16 val) { MemWriteT(addr, val); }
  void MemWriteUnprivileged8 (U64 addr, U8  val) { MemWriteT(addr, val); }

  void     SetExclusiveMonitors( U64 addr, unsigned size ) { dont("mp"); }
  bool     ExclusiveMonitorsPass( U64 addr, unsigned size ) { dont("mp"); return false; }
  void     ClearExclusiveLocal() { dont("mp"); }
  void     PrefetchMemory( int, U64 ) { dont("prefetch"); }

  void SetQC() { dont("floating-point"); }
  U32 FPCR() { dont("floating-point"); return U32(); }

  Decoder    decoder;
  Operation* current_instruction;

  uint32_t   nzcv;
  U64   current_insn_addr, next_insn_addr;
};

template <unsigned posT> void FPProcessException( Runner&, unisim::util::arithmetic::BitField<posT,1> const& ) {}
extern float clearsignaling( float );
extern double clearsignaling( double );
template <typename FLOAT> FLOAT FPNaN( Runner&, FLOAT value ) { return clearsignaling(value); }

template <typename T> T FPMulAdd(Runner& cpu, T const& acc, T const& op1, T const& op2) { return acc + (op1 * op2); }
template <typename T> T FPMulSub(Runner& cpu, T const& acc, T const& op1, T const& op2) { return acc - (op1 * op2); }


#endif // __ARM64SAV_RUNNER_HH__
