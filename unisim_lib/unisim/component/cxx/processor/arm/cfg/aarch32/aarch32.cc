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

#include <unisim/component/cxx/processor/arm/cfg/aarch32/aarch32.hh>
#include <unisim/component/cxx/processor/arm/isa/arm32/arm32.tcc>
#include <unisim/component/cxx/processor/arm/isa/thumb2/thumb.tcc>
#include <unisim/util/symbolic/symbolic.hh>
#include <memory>

namespace unisim { namespace util { namespace cfg { namespace intro {
  template <class OP> XValue<OP> NeonSHL( XValue<OP> op, XValue<int8_t> sh )
  {
    return XValue<OP>( unisim::component::cxx::processor::arm::NeonSHL( op.value, sh.value ), op.determined and sh.determined );
  }
}}}}

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace cfg {
namespace aarch32 {

struct ActionNode : public unisim::util::symbolic::Choice<ActionNode> {};

struct Core
{
  struct Config
  {
    // Full armv7 configuration
    enum { model = unisim::component::cxx::processor::arm::ARMV7,
      insns4T = true, insns5E = true, insns5J = true, insns5T = true, insns6  = true,
      insnsRM = true, insnsT2 = true, insns7  = true, hasVFP  = true, hasAdvSIMD = true };
  };

  struct OpStat {};

  typedef unisim::util::cfg::intro::XValue<double>   F64;
  typedef unisim::util::cfg::intro::XValue<float>    F32;
  typedef unisim::util::cfg::intro::XValue<bool>     BOOL;
  typedef unisim::util::cfg::intro::XValue<uint8_t>  U8;
  typedef unisim::util::cfg::intro::XValue<uint16_t> U16;
  typedef unisim::util::cfg::intro::XValue<uint32_t> U32;
  typedef unisim::util::cfg::intro::XValue<uint64_t> U64;
  typedef unisim::util::cfg::intro::XValue<int8_t>   S8;
  typedef unisim::util::cfg::intro::XValue<int16_t>  S16;
  typedef unisim::util::cfg::intro::XValue<int32_t>  S32;
  typedef unisim::util::cfg::intro::XValue<int64_t>  S64;

  Core( ActionNode& root, uint32_t addr, uint32_t length, bool _thumb )
    : path(&root), r15(addr + (_thumb?4:8)), insn_addr(addr), next_insn_addr(addr+length), branch_type(), thumb(_thumb), has_branch(false)
  {}

  ActionNode* path;
  U32 r15, insn_addr, next_insn_addr;
  enum branch_type_t { B_JMP = 0, B_CALL, B_RET, B_EXC, B_DBG, B_RFE } branch_type;
  bool thumb, has_branch;
  BOOL next_thumb;

  void SetQC() {}

  U32 GetGPR(int idx) { if (idx != 15) return U32(); return r15; }
  void SetGPR(int idx, U32 val)
  {
    if (idx != 15) return;
    if (thumb) this->Branch( val, B_JMP );
    else this->BranchExchange( val, B_JMP );
  }
  void SetGPR_mem(int idx, U32 val)
  {
    if (idx != 15) return;
    this->BranchExchange( val, B_JMP );
  }

  void BranchExchange(U32 target, branch_type_t branch_type)
  {
    next_thumb = BOOL(target & U32(1));
    this->Branch( target, branch_type );
  }

  void Branch(U32 target, branch_type_t _branch_type)
  {
    this->next_insn_addr = target & U32(thumb ? -2 : -4);
    has_branch = true;
    branch_type = _branch_type;
  }

  U32 GetCIA() { return this->insn_addr; }
  U32 GetNIA() { return this->next_insn_addr; }

  U32  GetVU32( unsigned idx ) { return U32(); }
  void SetVU32( unsigned idx, U32 val ) {}
  U64  GetVU64( unsigned idx ) { return U64(); }
  void SetVU64( unsigned idx, U64 val ) {}
  F32  GetVSR( unsigned idx ) { return F32(); }
  void SetVSR( unsigned idx, F32 val ) {}
  F64  GetVDR( unsigned idx ) { return F64(); }
  void SetVDR( unsigned idx, F64 val ) {}

  template <class ELEMT> void SetVDE( unsigned reg, unsigned idx, ELEMT const& value ) {}
  template <class ELEMT> ELEMT GetVDE( unsigned reg, unsigned idx, ELEMT const& trait ) { return ELEMT(); }

  U8 GetTVU8(unsigned r0, unsigned elts, unsigned regs, U8 idx, U8 oob) { return U8(); }

  U32  GetVSU( unsigned idx ) { return U32(); }
  void SetVSU( unsigned idx, U32 val ) {}
  U64  GetVDU( unsigned idx ) { return U64(); }
  void SetVDU( unsigned idx, U64 val ) {}

  struct PSR
  {
    template <typename F, typename V> void Set( F, V ) {}
    template <typename F> U32 Get( F ) { return U32(); }
  } xpsr;

  PSR&     CPSR() { return xpsr; };

  void SetCPSR( U32 const&, uint32_t ) {}
  U32 GetCPSR() { return U32(); }
  U32 GetNZCV() { return U32(); }

  struct Mode
  {
    U32 GetSPSR() { return U32(); }
    void SetSPSR(U32) {}
  };
  Mode CurrentMode() { return Mode(); }
  Mode GetMode(int) { return Mode(); }

  struct ITCond {};
  ITCond itcond() const { return ITCond(); }
  bool itblock() { return concretize(); }
  void ITSetState(uint8_t, uint8_t) {}

  template <typename T>
  bool Test( unisim::util::cfg::intro::XValue<T> const& cond )
  {
    BOOL c = BOOL(cond);
    if (c.determined) return c.value;
    return this->concretize();
  }
  bool concretize()
  {
    bool predicate = path->proceed();
    path = path->next( predicate );
    return predicate;
  }

  U32 MemURead32( U32 const& ) { return U32(); }
  U16 MemURead16( U32 const& ) { return U16(); }
  U32  MemRead32( U32 const& ) { return U32(); }
  U16  MemRead16( U32 const& ) { return U16(); }
  U8    MemRead8( U32 const& ) { return  U8(); }

  void MemUWrite32( U32 const&, U32 const& ) {}
  void MemUWrite16( U32 const&, U16 const& ) {}
  void  MemWrite32( U32 const&, U32 const& ) {}
  void  MemWrite16( U32 const&, U16 const& ) {}
  void   MemWrite8( U32 const&,  U8 const& ) {}

  static const unsigned PC_reg = 15;
  static const unsigned LR_reg = 14;
  static const unsigned SP_reg = 13;
 /* masks for the different running modes */
  static uint32_t const USER_MODE = 0b10000;
  static uint32_t const FIQ_MODE = 0b10001;
  static uint32_t const IRQ_MODE = 0b10010;
  static uint32_t const SUPERVISOR_MODE = 0b10011;
  static uint32_t const MONITOR_MODE = 0b10110;
  static uint32_t const ABORT_MODE = 0b10111;
  static uint32_t const HYPERVISOR_MODE = 0b11010;
  static uint32_t const UNDEFINED_MODE = 0b11011;
  static uint32_t const SYSTEM_MODE = 0b11111;

  template <typename T> void UndefinedInstruction( T ) { /*Illegal branch, ignoring*/ }
  void UnpredictableInsnBehaviour() { /*Illegal branch, ignoring*/ }
  void FPTrap( unsigned exc ) { /*Illegal branch, ignoring*/ }

  void SetBankedRegister( int, int, U32 const& ) {}
  U32  GetBankedRegister( int, int ) { return U32(); }

  void SetExclusiveMonitors( U32 const&, int ) {}
  bool ExclusiveMonitorsPass( U32 const&, int ) { return true; }
  void ClearExclusiveLocal() {}
  void CheckAlignment( U32 addr, unsigned alignment ) {}

  bool IntegerZeroDivide( BOOL const& ) { return false; }
  void BKPT( int ) {  }

  void WaitForInterrupt() {}

  struct CP15Reg
  {
    void CheckPermissions(uint8_t, uint8_t, uint8_t, uint8_t, Core&, bool) const {}
    U32  Read(uint8_t, uint8_t, uint8_t, uint8_t, Core&) const { return U32(); }
    void Write(uint8_t, uint8_t, uint8_t, uint8_t, Core&, U32 value ) const {}
    void Describe(uint8_t, uint8_t, uint8_t, uint8_t, std::ostream& sink ) const { sink << "no"; }
  };

  static CP15Reg* CP15GetRegister(uint8_t, uint8_t, uint8_t, uint8_t) { static CP15Reg x; return &x; }

  void CallSupervisor( uint32_t imm ) {}

  U32 FPSCR, FPEXC;
  U32 RoundTowardsZeroFPSCR() { return U32(); }
  U32 RoundToNearestFPSCR() { return U32(); }

  struct FP
  {
    template <typename T> static T Abs(T) { return T(); }
    template <typename T> static T Neg(T) { return T(); }
    template <typename T> static void SetDefaultNan( T& ) {}
    template <typename T> static void SetQuietBit( T& ) {}
    template <typename X, typename Y> static void ItoF( X&, Y const&, int, Core&, U32 const& ) {}
    template <typename X, typename Y> static void FtoI( Y&, X const&, int, Core&, U32 const& ) {}
    template <typename X, typename Y> static void FtoF( X&, Y const&, Core&, U32 const& ) {}


    template <typename T> static void Add( T&, T const&, Core&, U32 const& ) {}
    template <typename T> static void Sub( T&, T const&, Core&, U32 const& ) {}
    template <typename T> static void Div( T&, T const&, Core&, U32 const& ) {}
    template <typename T> static void Mul( T&, T const&, Core&, U32 const& ) {}

    template <typename T> static BOOL IsSNaN(T const&) { return BOOL(); }
    template <typename T> static BOOL IsQNaN(T const&) { return BOOL(); }
    template <typename T> static BOOL IsInvalidMulAdd( T&, T const&, T const&, U32 const& ) { return BOOL(); }
    template <typename T> static BOOL FlushToZero( T&, U32 const& ) { return BOOL(); }
    template <typename T> static void MulAdd( T&, T const&, T const&, Core&, U32 const& ) {}
    template <typename T> static void Sqrt( T&, Core&, U32 const& ) {}

    template <typename T> static S32 Compare( T const&, T const&, U32 const& ) { return S32(); }
  };
};

inline bool CheckCondition( Core& ab, Core::ITCond const& ) { return ab.concretize(); }

template <class Instruction>
void ComputeBranchInfo(unisim::util::cfg::intro::BranchInfo& branch, Instruction&& insn, uint32_t insn_addr, unsigned insn_length)
{
  ActionNode root;

  for (bool end = false; not end;)
    {
      Core ab( root, insn_addr, insn_length/8, insn.thumb );
      using unisim::component::cxx::processor::arm::CheckCondition;
      insn.execute( ab );
      branch.update( ab.has_branch, ab.next_insn_addr, unsigned(ab.branch_type) );
      end = ab.path->close();
    }
}

template <class T> std::unique_ptr<T> mkuniq(T* obj) { return std::unique_ptr<T>(obj); }

template <>
void BranchAnalyzer<true>::GetInfo(unisim::util::cfg::intro::BranchInfo& branch, uint32_t insn_addr, uint32_t insn, unsigned insn_length)
{
  typedef unisim::component::cxx::processor::arm::isa::thumb::Decoder<Core> Decoder;
  struct Instruction
  {
    enum { thumb = true };
    std::unique_ptr<typename Decoder::operation_type> op;
    void execute(Core& ab) { if (CheckCondition(ab, ab.itcond())) op->execute( ab ); }
  };
  static Decoder decoder;
  ComputeBranchInfo(branch, Instruction{mkuniq(decoder.NCDecode( insn_addr, insn ))}, insn_addr, insn_length);
}

template <>
void BranchAnalyzer<false>::GetInfo(unisim::util::cfg::intro::BranchInfo& branch, uint32_t insn_addr, uint32_t insn, unsigned insn_length)
{
  typedef unisim::component::cxx::processor::arm::isa::arm32::Decoder<Core> Decoder;
  struct Instruction
  {
    enum { thumb = false };
    std::unique_ptr<typename Decoder::operation_type> op;
    void execute(Core& ab) { if (unisim::component::cxx::processor::arm::CheckCondition(ab, op->GetEncoding() >> 28)) op->execute( ab ); }
  };
  static Decoder decoder;
  ComputeBranchInfo(branch, Instruction{mkuniq(decoder.NCDecode( insn_addr, insn ))}, insn_addr, insn_length);
}

} // end of namespace aarch32
} // end of namespace cfg
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
