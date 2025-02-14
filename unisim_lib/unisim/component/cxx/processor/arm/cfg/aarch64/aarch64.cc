/*
 *  Copyright (c) 2025,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#include <unisim/component/cxx/processor/arm/cfg/aarch64/aarch64.hh>
#include <unisim/component/cxx/processor/arm/isa/arm64/arm64.tcc>
#include <unisim/util/symbolic/symbolic.hh>
#include <memory>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace cfg {
namespace aarch64 {

struct ActionNode : public unisim::util::symbolic::Choice<ActionNode> {};

template <typename T> using XValue = unisim::util::cfg::intro::XValue<T>;

struct Core
{
  typedef XValue<uint8_t>     U8;
  typedef XValue<uint16_t>    U16;
  typedef XValue<uint32_t>    U32;
  typedef XValue<uint64_t>    U64;
  typedef XValue<int8_t>      S8;
  typedef XValue<int16_t>     S16;
  typedef XValue<int32_t>     S32;
  typedef XValue<int64_t>     S64;
  typedef XValue<bool>        BOOL;

  typedef XValue<_Float16>    F16;
  typedef XValue<float>       F32;
  typedef XValue<double>      F64;

  struct DisasmState {};
  struct OpStat {};

  enum branch_type_t { B_JMP = 0, B_CALL, B_RET, B_EXC, B_ERET, B_DBG, B_COND } ;
  enum branch_mode_t { B_DIRECT = 0, B_INDIRECT };

  Core( ActionNode& root, uint32_t addr )
    : path(&root), insn_addr(addr), next_insn_addr(addr +  4), branch_type(), has_branch(false)
  {}

  ActionNode* path;
  U64 insn_addr, next_insn_addr;
  branch_type_t branch_type;
  bool has_branch;

  template <typename T>
  bool Test( unisim::util::cfg::intro::XValue<T> const& cond )
  {
    BOOL c = BOOL(cond);
    if (c.determined) return c.value;
    return this->concretize();
  }
  bool     Test(bool cond) { return cond; }

  bool concretize()
  {
    bool predicate = path->proceed();
    path = path->next( predicate );
    return predicate;
  }

  U64 GetPC() { return U64(insn_addr); }
  void BranchTo( U64 const& npc, branch_type_t bt ) { BranchTo(true, npc, bt, B_DIRECT); }
  void BranchTo( U64 const& npc, branch_type_t bt, branch_mode_t bm ) { BranchTo(true, npc, bt, bm); }
  void BranchTo( bool predicate, U64 const& npc, branch_type_t bt ) { BranchTo( predicate, npc, bt, B_DIRECT); }
  void BranchTo( bool predicate, U64 const& npc, branch_type_t bt, branch_mode_t bm )
  {
    if(predicate)
    {
      next_insn_addr = npc;
      has_branch = true;
      branch_type = bt;
    }
  }

  template <class Insn> void UndefinedInstruction(Insn const*) { throw 0; }

  struct SysReg
  {
    void Write(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, Core& cpu, U64 value) const {};
    U64 Read(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, Core& cpu) const { return U64(); };
    void DisasmRead(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const { sink << "?"; };
    void DisasmWrite(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink) const { sink << "?"; };
  };

  static SysReg const* GetSystemRegister(uint8_t op0, uint8_t op1, uint8_t crn, uint8_t crm, uint8_t op2)  { static SysReg x; return &x; }

  void CheckSystemAccess(uint8_t) {}
  U64 GetGZR(unsigned reg) { return (reg != 31) ? U64() : U64(0); }
  U64 GetGSR(unsigned) { return U64(); }
  template <typename U> void SetGZR(unsigned, U) {}
  template <typename U> void SetGSR(unsigned, U) {}
  BOOL GetCarry() { return BOOL(); }
  void SetNZCV(BOOL,BOOL,BOOL,BOOL) {}
  U8 GetNZCV() { return U8(); }
  void CheckSPAlignment(U64) {}
  void MemWrite8(U64, U8) {}
  void MemWrite16(U64, U16) {}
  void MemWrite32(U64, U32) {}
  void MemWrite64(U64, U64) {}
  U8 MemRead8(U64) { return U8(); }
  U16 MemRead16(U64) { return U16(); }
  U32 MemRead32(U64) { return U32(); }
  U64 MemRead64(U64) { return U64(); }
  U8 MemReadUnprivileged8(U64) { return U8(); }
  U16 MemReadUnprivileged16(U64) { return U16(); }
  U32 MemReadUnprivileged32(U64) { return U32(); }
  U64 MemReadUnprivileged64(U64) { return U64(); }
  void MemWriteUnprivileged8(U64, U8) {}
  void MemWriteUnprivileged16(U64, U16) {}
  void MemWriteUnprivileged32(U64, U32) {}
  void MemWriteUnprivileged64(U64, U64) {}
  U8 GetVU8(unsigned, unsigned) { return U8(); }
  U16 GetVU16(unsigned, unsigned) { return U16(); }
  U32 GetVU32(unsigned, unsigned) { return U32(); }
  U64 GetVU64(unsigned, unsigned) { return U64(); }
  S8 GetVS8(unsigned, unsigned) { return S8(); }
  S16 GetVS16(unsigned, unsigned) { return S16(); }
  S32 GetVS32(unsigned, unsigned) { return S32(); }
  S64 GetVS64(unsigned, unsigned) { return S64(); }
  F16 GetVF16(unsigned, unsigned) { return F16(); }
  F32 GetVF32(unsigned, unsigned) { return F32(); }
  F64 GetVF64(unsigned, unsigned) { return F64(); }
  void SetVU8(unsigned, unsigned, U8) {}
  void SetVU16(unsigned, unsigned, U16) {}
  void SetVU32(unsigned, unsigned, U32) {}
  void SetVU64(unsigned, unsigned, U64) {}
  void SetVU8(unsigned, U8) {}
  void SetVU16(unsigned, U16) {}
  void SetVU32(unsigned, U32) {}
  void SetVU64(unsigned, U64) {}
  void SetVS8(unsigned, unsigned, S8) {}
  void SetVS16(unsigned, unsigned, S16) {}
  void SetVS32(unsigned, unsigned, S32) {}
  void SetVS64(unsigned, unsigned, S64) {}
  void SetVS8(unsigned, S8) {}
  void SetVS16(unsigned, S16) {}
  void SetVS32(unsigned, S32) {}
  void SetVS64(unsigned, S64) {}
  void SetVF16(unsigned, unsigned, F16) {}
  void SetVF32(unsigned, unsigned, F32) {}
  void SetVF64(unsigned, unsigned, F64) {}
  void SetVF16(unsigned, F16) {}
  void SetVF32(unsigned, F32) {}
  void SetVF64(unsigned, F64) {}
  void ClearHighV(unsigned, unsigned) {}
  U8 GetTVU8(unsigned, unsigned, unsigned, U8, U8) { return U8(); }

  void CallHypervisor(unsigned) {}
  void CallSupervisor(unsigned) {}
  void SoftwareBreakpoint(unsigned) {}
  void ClearExclusiveLocal() {}
  void SetExclusiveMonitors(U64,unsigned) {}
  bool ExclusiveMonitorsPass(U64,unsigned) { return this->concretize(); }
  void ExceptionReturn() {}
  void PrefetchMemory(unsigned, U64) {}
  void SetQC() {}

  U32  fpcr;
  U32& FPCR() { return fpcr; }
};

template <class FLOAT> FLOAT FPAdd( Core& core, FLOAT op1, FLOAT op2 ) { return FLOAT(); }
template <class FLOAT> FLOAT FPSub( Core& core, FLOAT op1, FLOAT op2 ) { return FLOAT(); }
template <class FLOAT> FLOAT FPMul( Core& core, FLOAT op1, FLOAT op2 ) { return FLOAT(); }
template <class FLOAT> FLOAT FPMulX( Core& core, FLOAT op1, FLOAT op2 ) { return FLOAT(); }
template <class FLOAT> FLOAT FPDiv( Core& core, FLOAT op1, FLOAT op2 ) { return FLOAT(); }
template <class FLOAT> FLOAT FPMulAdd( Core& core, FLOAT op1, FLOAT op2, FLOAT op3 ) { return FLOAT(); }
template <class DST, class SRC> DST FPConvert( Core& core, SRC op ) { return DST(); }
template <class DST, class SRC> DST FPConvertRoundOdd( Core& core, SRC op ) { return DST(); }
template <class FLOAT> Core::BOOL FPCompareGe( Core& core, FLOAT op1, FLOAT op2 ) { return Core::BOOL(); }
template <class FLOAT> Core::BOOL FPCompareGt( Core& core, FLOAT op1, FLOAT op2 ) { return Core::BOOL(); }
template <class FLOAT> FLOAT FPSqrt( Core& core, FLOAT op ) { return FLOAT(); }
template <class FLOAT, class INTEGER> FLOAT FixedToFP(Core& core, INTEGER op, unsigned fbits) { return FLOAT(); }
template <class FLOAT, class RM> FLOAT FPRoundInt(Core& Core, RM, FLOAT, bool) { return FLOAT(); }
template <unsigned posT> void FPProcessException( Core& core, unisim::util::arithmetic::BitField<posT,1> ) {}
template <class FLOAT> Core::BOOL FPQuietCompare( Core& core, FLOAT op1, FLOAT op2 ) { return Core::BOOL(); }
template <class INTEGER, class RM, class FLOAT> INTEGER FPToInt( Core& core, RM, FLOAT op, bool exact) { return INTEGER(); }
template <class INTEGER, class FLOAT> INTEGER FPToFixedRoundTowardZero(Core& core, FLOAT op, unsigned fbits, bool exact) { return INTEGER(); }
template <class FLOAT> FLOAT FPMax( Core& core, FLOAT op1, FLOAT op2 ) { return FLOAT(); }
template <class FLOAT> FLOAT FPMin( Core& core, FLOAT op1, FLOAT op2 ) { return FLOAT(); }
template <class FLOAT> FLOAT FPMaxNumber( Core& core, FLOAT op1, FLOAT op2 ) { return FLOAT(); }
template <class FLOAT> FLOAT FPMinNumber( Core& core, FLOAT op1, FLOAT op2 ) { return FLOAT(); }
template <class FLOAT, class INTEGER> FLOAT IntToFP(Core& core, INTEGER op) { return FLOAT(); }
template <class FLOAT> FLOAT FPNaN( Core& core, FLOAT value ) { return FLOAT(); }

void ComputeBranchInfo(unisim::util::cfg::intro::BranchInfo& branch, uint64_t insn_addr, uint32_t insn, unsigned insn_length)
{
  typedef unisim::component::cxx::processor::arm::isa::arm64::Decoder<Core> Decoder;
  typedef typename Decoder::operation_type Operation;
  static Decoder decoder;

  auto op = std::unique_ptr<Operation>(decoder.NCDecode( insn_addr, insn ));

  ActionNode root;

  for (bool end = false; not end;)
    {
      Core ab( root, insn_addr );
      op->execute( ab );
      branch.update( ab.has_branch, ab.next_insn_addr, ab.branch_type == ab.B_CALL ? branch.Call : ab.branch_type == ab.B_RET ? branch.Return : branch.NoHint );
      end = ab.path->close();
    }
}

} // end of namespace aarch64
} // end of namespace cfg
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#if HAVE_FLOAT16

template <>
unisim::util::cfg::intro::XValue<_Float16>
FromPacked<unisim::util::cfg::intro::XValue<_Float16>,unisim::util::cfg::intro::XValue<uint16_t>>(unisim::util::cfg::intro::XValue<uint16_t> f)
{
  return unisim::util::cfg::intro::XValue<_Float16>(FromPacked<_Float16,uint16_t>(f.value), f.determined);
}

#endif

template <>
unisim::util::cfg::intro::XValue<float>
FromPacked<unisim::util::cfg::intro::XValue<float>,unisim::util::cfg::intro::XValue<uint32_t>>(unisim::util::cfg::intro::XValue<uint32_t> f)
{
  return unisim::util::cfg::intro::XValue<float>(FromPacked<float,uint32_t>(f.value), f.determined);
}

template <>
unisim::util::cfg::intro::XValue<double>
FromPacked<unisim::util::cfg::intro::XValue<double>,unisim::util::cfg::intro::XValue<uint64_t>>(unisim::util::cfg::intro::XValue<uint64_t> f)
{
  return unisim::util::cfg::intro::XValue<double>(FromPacked<double,uint64_t>(f.value), f.determined);
}

