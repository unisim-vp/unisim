/*
 *  Copyright (c) 2009-2023,
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

#include <unisim/util/symbolic/binsec/binsec.hh>
#include <unisim/util/symbolic/symbolic.hh>
// #include <ppc64dec.hh>
// #include <iostream>
// #include <iomanip>
// #include <cstdlib>
// #include <set>
#include <inttypes.h>

namespace ppc64 {

  typedef unisim::util::symbolic::SmartValue<double>   F64;
  typedef unisim::util::symbolic::SmartValue<float>    F32;
  typedef unisim::util::symbolic::SmartValue<bool>     BOOL;
  typedef unisim::util::symbolic::SmartValue<uint8_t>  U8;
  typedef unisim::util::symbolic::SmartValue<uint16_t> U16;
  typedef unisim::util::symbolic::SmartValue<uint32_t> U32;
  typedef unisim::util::symbolic::SmartValue<uint64_t> U64;
  typedef unisim::util::symbolic::SmartValue<int8_t>   S8;
  typedef unisim::util::symbolic::SmartValue<int16_t>  S16;
  typedef unisim::util::symbolic::SmartValue<int32_t>  S32;
  typedef unisim::util::symbolic::SmartValue<int64_t>  S64;
  
  typedef U64 UINT;
  typedef S64 SINT;
  typedef U64 ADDRESS;

  struct Arch
  {
    //   =====================================================================
    //   =                             Data Types                            =
    //   =====================================================================

    //    using FP = unisim::util::symbolic::FP;
    using Expr = unisim::util::symbolic::Expr;
    using ExprNode = unisim::util::symbolic::ExprNode;
    using ValueType = unisim::util::symbolic::ValueType;
    using ActionNode = unisim::util::symbolic::binsec::ActionNode;
    using Label = unisim::util::symbolic::binsec::Label;
    using Variables = unisim::util::symbolic::binsec::Variables;

    struct Source : public unisim::util::symbolic::binsec::ASExprNode
    {
      typedef unisim::util::symbolic::ValueType ValueType;
      Source( Arch& _arch ) : arch(_arch) {} Arch& arch;
      unsigned SubCount() const override { return 0; };

      static Arch* SeekArch( unisim::util::symbolic::Expr const& expr )
      {
        if (Source const* node = dynamic_cast<Source const*>( expr.node ))
          return &node->arch;
        for (unsigned idx = 0, end = expr->SubCount(); idx < end; ++idx)
          if (Arch* found = SeekArch( expr->GetSub(idx)))
            return found;
        return 0;
      }
    };

    struct SourceForward : Source
    {
      using Expr = unisim::util::symbolic::Expr;
      SourceForward( Arch& arch, Expr const& _src ) : Source(arch), src(_src) {}
      unsigned SubCount() const override { return 1; };
      Expr const& GetSub(unsigned idx) const override { if (idx!= 0) return unisim::util::symbolic::ExprNode::GetSub(idx); return src; }
      Expr const& src;
      void Repr(std::ostream& sink) const override { return src->Repr(sink); }
      int cmp(const unisim::util::symbolic::ExprNode&) const override { return 0; }
      ExprNode* Mutate() const { return new SourceForward(*this); }
      ValueType const* GetType() const override { return src->GetType(); }
      int GenCode(Label& label, Variables& vars, std::ostream& sink) const { return GenerateCode(src, vars, label, sink); }
    };
  
    template <typename RID>
    Expr newRegRead( RID id ) { return new SourceForward( *this, new unisim::util::symbolic::binsec::RegRead<RID>( id ) ); }

    template <typename RID>
    Expr newRegWrite( RID id, Expr const& value ) { return new unisim::util::symbolic::binsec::RegWrite<RID>( id, value ); }

    struct Unimplemented {};

    Arch(U64 const& insn_addr);
  
    bool  close( Arch const& ref, uint64_t linear_nia );

    bool choose( Expr const& cexp )
    {
      bool predicate = path->proceed( cexp );
      path = path->next( predicate );
      return predicate;
    }
  
    static bool concretize( Expr cexp )
    {
      if (unisim::util::symbolic::ConstNodeBase const* cnode = cexp.ConstSimplify())
        return dynamic_cast<unisim::util::symbolic::ConstNode<bool> const&>(*cnode).value;
    
      Arch* arch = Source::SeekArch(cexp);
      if (not arch)
        throw 0;
    
      return arch->choose( cexp );
    }

    template <typename T>
    static bool Test( unisim::util::symbolic::SmartValue<T> const& cond )
    {
      if (not cond.expr.good())
        throw std::logic_error( "Not a valid condition" );
    
      return concretize( BOOL(cond).expr );
    }
  
    static bool Test( bool c ) { return c; }

    //   =====================================================================
    //   =                         Condition Register                        =
    //   =====================================================================
  
    struct CR
    {
      struct CR0 { struct OV {}; struct SO {}; struct LT {}; struct GT {}; struct EQ {}; struct ALL {}; };
      struct CR1 { struct OV {}; struct SO {}; struct LT {}; struct GT {}; struct EQ {}; struct ALL {}; };
      struct CR2 { struct OV {}; struct SO {}; struct LT {}; struct GT {}; struct EQ {}; struct ALL {}; };
      struct CR3 { struct OV {}; struct SO {}; struct LT {}; struct GT {}; struct EQ {}; struct ALL {}; };
      struct CR4 { struct OV {}; struct SO {}; struct LT {}; struct GT {}; struct EQ {}; struct ALL {}; };
      struct CR5 { struct OV {}; struct SO {}; struct LT {}; struct GT {}; struct EQ {}; struct ALL {}; };
      struct CR6 { struct OV {}; struct SO {}; struct LT {}; struct GT {}; struct EQ {}; struct ALL {}; };
      struct CR7 { struct OV {}; struct SO {}; struct LT {}; struct GT {}; struct EQ {}; struct ALL {}; };

      // template <typename PART,typename T> void Set( SmartValue<T> const& value ) { CRAccess(true); cr_value = make_function( "mix", cr_value.expr, value.expr ); }
      // template <typename PART> void Set( uint32_t value ) { Set<PART,uint32_t>( unisim::util::symbolic::make_const(value) ); }
      // template <typename PART> U32 Get() { CRAccess(false); return cr_value; }
      operator U32 () { return value; }
      CR& operator= ( U32 const& _value ) { value = _value; return *this; }

      struct ID : public unisim::util::symbolic::WithValueType<ID> { typedef uint32_t value_type; void Repr(std::ostream& sink) const {} int cmp(ID) const { return 0; } };

      CR( Arch& arch ) : value( arch.newRegRead(ID()) ) {}

      U32 value;
    };

    CR& GetCR() { return cr; }

    struct XER
    {
      struct OV {};
      struct SO {};
      struct CA {};
      struct _0_3 {};

      typedef unisim::util::symbolic::Expr Expr;
      typedef unisim::util::symbolic::ExprNode ExprNode;

      // template <typename PART,typename T> void Set( SmartValue<T> const& value ) { XERAccess(true); xer_value = make_function("mix", xer_value.expr, value.expr ); }
      // template <typename PART> void Set( uint32_t value ) { Set<PART,uint32_t>( unisim::util::symbolic::make_const(value) ); }
      // template <typename PART> U64 Get() { XERAccess(false); return xer_value; }
      operator U64 () { return value; }
      XER& operator= ( U64 const& _value ) { value = value; return *this; }

      struct ID : public unisim::util::symbolic::WithValueType<ID> { typedef uint32_t value_type; void Repr(std::ostream& sink) const {} int cmp(ID) const { return 0; } };

      XER( Arch& arch ) : value( arch.newRegRead(ID()) ) {}

      U64 value;
    };

    XER& GetXER() { return xer; }

    //   =====================================================================
    //   =                         Integer registers                         =
    //   =====================================================================

    struct IRegID
      : public unisim::util::identifier::Identifier<IRegID>
      , public unisim::util::symbolic::WithValueType<IRegID>
    {
      typedef uint64_t value_type;

      enum Code {
        r0=0, r1,  r2,  r3,  r4,  r5,  r6,  r7,  r8,  r9, r10, r11, r12, r13, r14, r15,
        r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31,
        ctr, lr,
        end
      } code;

      char const* c_str() const
      {
        switch (code)
          {
          case r0: return "r0";
          case r1: return "r1";
          case r2: return "r2";
          case r3: return "r3";
          case r4: return "r4";
          case r5: return "r5";
          case r6: return "r6";
          case r7: return "r7";
          case r8: return "r8";
          case r9: return "r9";
          case r10: return "r10";
          case r11: return "r11";
          case r12: return "r12";
          case r13: return "r13";
          case r14: return "r14";
          case r15: return "r15";
          case r16: return "r16";
          case r17: return "r17";
          case r18: return "r18";
          case r19: return "r19";
          case r20: return "r20";
          case r21: return "r21";
          case r22: return "r22";
          case r23: return "r23";
          case r24: return "r24";
          case r25: return "r25";
          case r26: return "r26";
          case r27: return "r27";
          case r28: return "r28";
          case r29: return "r29";
          case r30: return "r30";
          case r31: return "r31";
          case ctr: return "ctr";
          case lr:  return "lr";

          case end: break;
          }
        return "NA";
      }

      void Repr( std::ostream& sink ) const;

      IRegID() : code(end) {}
      IRegID( Code _code ) : code(_code) {}
      IRegID( unsigned idx ) : code(Code(idx)) {}
      IRegID( char const* _code ) : code(end) { init( _code ); }
    };

    typedef U64 CTR;
    // struct CTR
    // {
    //   CTR( Arch& _arch ) : arch( _arch ) {}

    //   Arch& arch;
    // };

    U64  GetGPR(unsigned idx) { return regvalues[idx]; }

    U64& GetCTR() { return regvalues[IRegID::ctr]; }
    U64& GetLR() { return regvalues[IRegID::lr]; }

    //   =====================================================================
    //   =                      Control Transfer methods                     =
    //   =====================================================================
  
    enum branch_type_t { B_JMP = 0, B_CALL, B_RET };
    void Branch(U64 const& npc, branch_type_t bt)
    {
      next_instruction_address = npc;
      branch_type = bt;
    }

    void Branch(U64 const& npc) { Branch(npc, B_JMP); }
    U64 GetCIA() const { return next_instruction_address; }

    struct ProgramInterrupt
    {
      struct IllegalInstruction {};
    };
    template <typename T>  void ThrowException() { throw Unimplemented(); }

    ActionNode*      path;
    U64              current_instruction_address;
    U64              next_instruction_address;
    branch_type_t    branch_type;
    U64 regvalues[IRegID::end];
    CR cr;
    XER xer;
  };

  typedef Arch CPU;
  typedef Arch CONFIG;

  typedef Arch::CTR CTR;
  typedef Arch::CR CR;
  typedef Arch::XER XER;

} // end of namespace ppc64

