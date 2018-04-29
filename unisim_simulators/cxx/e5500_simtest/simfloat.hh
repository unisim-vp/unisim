/*
 *  Copyright (c) 2015,
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

#ifndef E5500_SIMTEST_SIMFLOAT_HH
#define E5500_SIMTEST_SIMFLOAT_HH

#include <types.hh>
#include <unisim/util/symbolic/symbolic.hh>

namespace ut
{
  struct Flags
  {
    typedef unisim::util::symbolic::Expr Expr;
    typedef unisim::util::symbolic::ExprNode ExprNode;
    
    struct RoundingMode : public Expr
    {
      RoundingMode(UINT const& rm) : Expr( rm.expr ) {}
      RoundingMode(unsigned rm) : Expr( unisim::util::symbolic::make_const<unsigned>( rm ) ) {}
    };

    Flags( RoundingMode const& _rm ) : rm(_rm) {}
    
    // Flags() {}
    // BOOL isUpApproximate() { return make_unknown<BOOL>(); }
    // BOOL isDownApproximate() { return make_unknown<BOOL>(); }
    // BOOL takeOverflow() { return make_unknown<BOOL>(); }

    BOOL hasIncrementFraction(bool neg) const
    {
      return make_function(neg ? "hasIncrementFractionNeg" : "hasIncrementFractionPos", op);
    }
    BOOL isApproximate() const { return make_function("isApproximate", op); }         // FPSCR.FI
    BOOL isOverflow() const { return make_function("isOverflow", op); }               // FPSCR.OX
    BOOL isUnderflow() const { return make_function("isUnderflow", op); }             // FPSCR.UX
    BOOL isDivisionByZero() const { return make_function("isDivisionByZero", op); }   // FPSCR.ZX
    BOOL hasSNaNOperand() const { return make_function("hasSNaNOperand", op); }       // FPSCR.VXSNAN
    BOOL isInftyMinusInfty() const { return make_function("isInftyMinusInfty", op); } // FPSCR.VXISI
    BOOL isInftyOnInfty() const { return make_function("isInftyOnInfty", op); }       // FPSCR.VXIDI
    BOOL isInftyMultZero() const { return make_function("isInftyMultZero", op); }     // FPSCR.VXIMZ
    BOOL isZeroOnZero() const { return make_function("isZeroOnZero", op); }           // FPSCR.VXZDZ

    void from( Expr const& _op ) { op = _op; }

    Expr rm, op;
  };

  template <typename SRCTYPE>
  struct FromRawBitsNode : public unisim::util::symbolic::ExprNode
  {
    typedef unisim::util::symbolic::Expr Expr;
    typedef unisim::util::symbolic::ExprNode ExprNode;
    
    FromRawBitsNode( Expr const& _src ) : src(_src) {}
    virtual void Repr( std::ostream& sink ) const;
    virtual unsigned SubCount() const { return 1; };
    virtual Expr const& GetSub(unsigned idx) const { if (idx) return ExprNode::GetSub(idx); return src; };
    virtual intptr_t cmp( ExprNode const& brhs ) const { return src.cmp( dynamic_cast<FromRawBitsNode const&>( brhs ).src ); }
    Expr src;
  };

  struct SoftFloat;
  struct SoftDouble;

  template <typename SRC, typename DST, bool fraction>
  struct FPConvNode : public unisim::util::symbolic::ExprNode
  {
    typedef unisim::util::symbolic::Expr Expr;
    typedef unisim::util::symbolic::ExprNode ExprNode;
    
    FPConvNode( Expr const& _src, Expr const& _rm ) : src(_src), rm(_rm) {}
    virtual void Repr( std::ostream& sink ) const;
    virtual unsigned SubCount() const { return 2; };
    virtual Expr const& GetSub(unsigned idx) const
    {
      switch(idx) { case 0: return src; case 1: return rm; }
      return ExprNode::GetSub(idx);
    }
    virtual intptr_t cmp( ExprNode const& brhs ) const
    {
      auto && rhs = dynamic_cast<FPConvNode const&>( brhs );
      if (intptr_t delta = src.cmp( rhs.src )) return delta;
      return rm.cmp( rhs.rm );
    }
    Expr src, rm;
  };

  struct SoftFloat
  {
    typedef unisim::util::symbolic::Expr Expr;
    typedef unisim::util::symbolic::ExprNode ExprNode;

    enum ComparisonResult
      { CRNaN = 0, CRLess = 1, CRGreater = 2, CREqual = 3 };
    
    enum __FromRawBits__ { FromRawBits };
    SoftFloat( __FromRawBits__, U32 const& source ) : expr(new FromRawBitsNode<uint32_t>( source.expr )) {}
    SoftFloat( SoftDouble const& source, Flags& flags );
    
    U32 queryRawBits() const { return Expr(new FromRawBitsNode<float>( expr )); }

    void plusAssign(SoftFloat const&, Flags const& flags) {/*TODO*/}
    void minusAssign(SoftFloat const&, Flags const& flags) {/*TODO*/}
    void multAssign(SoftFloat const&, Flags const& flags) {/*TODO*/}
    void divAssign(SoftFloat const&, Flags const& flags) {/*TODO*/}
    void multAndAddAssign(SoftFloat const&, SoftFloat const&, Flags const& flags) {/*TODO*/}
    void multAndSubAssign(SoftFloat const&, SoftFloat const&, Flags const& flags) {/*TODO*/}
    //void sqrtAssign(Flags const& flags) {/*TODO*/}
    // void squareAssign(Flags const& flags) {/*TODO*/}
    void opposite() {/*TODO*/}
    void setQuiet() {/*TODO*/}
    void setPositive() {/*TODO*/}
    
    ComparisonResult compare( SoftFloat const& rhs ) const
    {
      return CRNaN; /*TODO*/
      // SoftFloat const& lhs = *this;
      // return ComparisonResult(2*int(evenly(lhs >= rhs)) + int(evenly(lhs <= rhs)));
    }
    
    // U32 queryU32( Flags& flags, unsigned fbits=0 )
    // {
    //   Expr res;
    //   if (fbits == 0) res = Expr(new FPConvNode<uint32_t,float,false>(expr,flags.rm));
    //   else            res = Expr(new FPConvNode<uint32_t,float,true>(expr,flags.rm));
    //   flags.from(res);
    //   return res;
    // }
    // S32 queryS32( Flags& flags, unsigned fbits=0 )
    // {
    //   Expr res;
    //   if (fbits == 0) res = Expr(new FPConvNode<int32_t,float,false>(expr,flags.rm));
    //   else            res = Expr(new FPConvNode<int32_t,float,true>(expr,flags.rm));
    //   flags.from(res);
    //   return res;
    // }

    // struct Cmp : public unisim::util::symbolic::Identifier<Cmp>
    // {
    //   enum Code { NA = 0, EQ, GT, LT, end } code;
    //   char const* c_str()
    //   {
    //     switch (code) { case EQ: return "EQ"; case GT: return "GT"; case LT: return "LT"; case NA: case end: break; }
    //     return "INVALID";
    //   }
    //   Cmp() : code(end) {}
    //   Cmp( Code _code ) : code(_code) {}
    //   Cmp( char const* _code ) : code(end) { init( _code ); }
    // };
    
    // BOOL operator == (SoftFloat const& rhs) { return make_function("EQ",expr,rhs.expr); }
    // BOOL operator < (SoftFloat const& rhs) { return make_function("LT",expr,rhs.expr); }
    // BOOL operator > (SoftFloat const& rhs) { return make_function("GT",expr,rhs.expr); }

    // BOOL isNegative() const { return make_function("isNeg",expr); }
    // BOOL isPositive() { return make_unknown<BOOL>(arch); }
    BOOL isZero() const { return make_function("isZero",expr); }
    //BOOL isInfty() const { return make_function("isInfty",expr); }
    BOOL isNaN() const { return make_function("isNaN", expr); }
    BOOL isQNaN() const { return make_function("isQNaN", expr); }
    // BOOL isNormalized() const { return make_function("isNormal",expr); }
    // void setNegative(bool=false) {}
    // void setNegative(BOOL) {}
    // void multNegativeAndAddAssign(SoftFloat const&, SoftFloat const&, Flags const& flags) {}
    // void multNegativeAndSubAssign(SoftFloat const&, SoftFloat const&, Flags const& flags) {}
    // void maxAssign(SoftFloat const&) {}
    // void minAssign(SoftFloat const&) {}
    
    // void clear() {}
    
    // BOOL hasInftyExponent() { return make_unknown<BOOL>(arch); }
    // BOOL isDenormalized() { return make_unknown<BOOL>(arch); }

    unisim::util::symbolic::Expr expr;
  };

  struct SoftDouble
  {
    typedef unisim::util::symbolic::Expr Expr;
    typedef unisim::util::symbolic::ExprNode ExprNode;

    enum ComparisonResult
      { CRNaN = 0, CRLess = 1, CRGreater = 2, CREqual = 3 };
  
    SoftDouble() : expr() {}
    
    enum __FromRawBits__ { FromRawBits };
    SoftDouble( __FromRawBits__, U64 const& source ) : expr(new FromRawBitsNode<uint64_t>( source.expr )) {}
    
    SoftDouble(SoftFloat const& source, Flags& flags)
      : expr(new FPConvNode<double,float,false>(source.expr,flags.rm)) { flags.from( expr ); }
    SoftDouble( S64 const& source, Flags& flags )
      : expr(new FPConvNode<double,int64_t,false>(source.expr,flags.rm)) { flags.from( expr ); }

    SoftDouble& convertAssign( SoftFloat const& source, Flags& flags )
    {
      expr = new FPConvNode<double,float,false>(source.expr,flags.rm);
      flags.from( expr );
      return *this;
    }
    SoftDouble& fromRawBitsAssign( U64 const& source )
    {
      expr = new FromRawBitsNode<uint64_t>( source.expr );
      return *this;
    }
    
    U64 queryRawBits() const { return Expr(new FromRawBitsNode<double>( expr )); }
    S64 queryS64( Flags& flags ) const
    {
      Expr res( Expr(new FPConvNode<int64_t,double,false>(expr,flags.rm)) );
      flags.from(res);
      return res;
    }
    S32 queryS32( Flags& flags, unsigned fbits=0 ) const
    {
      Expr res( Expr(new FPConvNode<int32_t,double,false>(expr,flags.rm)) );
      flags.from(res);
      return res;
    }

    void plusAssign(SoftDouble const&, Flags const& flags) {/*TODO*/}
    void minusAssign(SoftDouble const&, Flags const& flags) {/*TODO*/}
    void multAssign(SoftDouble const&, Flags const& flags) {/*TODO*/}
    void divAssign(SoftDouble const&, Flags const& flags) {/*TODO*/}
    void multAndAddAssign(SoftDouble const&, SoftDouble const&, Flags const& flags) {/*TODO*/}
    void multAndSubAssign(SoftDouble const&, SoftDouble const&, Flags const& flags) {/*TODO*/}
    void opposite() {/*TODO*/}
    void setQuiet() {/*TODO*/}
    void setPositive() {/*TODO*/}
    void setNegative() {/*TODO*/}

    void rSqrtEstimAssign() {/*TODO*/}
    void sqrtAssign(Flags const& flags) {/*TODO*/}
    // void squareAssign(Flags const& flags) {}
    // void multNegativeAndAddAssign(SoftDouble const&, SoftDouble const&, Flags const& flags) {}
    // void multNegativeAndSubAssign(SoftDouble const&, SoftDouble const&, Flags const& flags) {}
    // void maxAssign(SoftDouble const&) {}
    // void minAssign(SoftDouble const&) {}
    
    ComparisonResult compare( SoftDouble const& rhs ) const
    {
      return CRNaN; /*TODO*/
      // SoftFloat const& lhs = *this;
      // return ComparisonResult(2*int(evenly(lhs >= rhs)) + int(evenly(lhs <= rhs)));
    }
    
    // struct Cmp : public unisim::util::symbolic::Identifier<Cmp>
    // {
    //   enum Code { NA = 0, EQ, GT, LT, end } code;
    //   char const* c_str()
    //   {
    //     switch (code) { case EQ: return "EQ"; case GT: return "GT"; case LT: return "LT"; case NA: case end: break; }
    //     return "INVALID";
    //   }
    //   Cmp() : code(end) {}
    //   Cmp( Code _code ) : code(_code) {}
    //   Cmp( char const* _code ) : code(end) { init( _code ); }
    // };
    
    // BOOL operator == (SoftDouble const& rhs) { return make_function("EQ",expr,rhs.expr); }
    // BOOL operator < (SoftDouble const& rhs) { return make_function("LT",expr,rhs.expr); }
    // BOOL operator > (SoftDouble const& rhs) { return make_function("GT",expr,rhs.expr); }

    BOOL isNegative() const { return make_function("isNeg",expr); }
    BOOL isNaN() const { return make_function("isNaN",expr); }
    BOOL isQNaN() const { return make_function("isQNaN",expr); }
    BOOL isSNaN() const { return make_function("isSNaN",expr); }
    BOOL isZero() const { return make_function("isZero",expr); }
    BOOL isInfty() const { return make_function("isInfty",expr); }
    // BOOL isNormalized() const { return make_function("isNormal",expr); }
    // BOOL isPositive() { return make_unknown<BOOL>(arch); }
    // void setNegative(bool=false) {}
    // void setNegative(BOOL) {}
    
    // void clear() {}
    
    // BOOL hasInftyExponent() { return make_unknown<BOOL>(arch); }
    // BOOL isDenormalized() { return make_unknown<BOOL>(arch); }

    unisim::util::symbolic::Expr expr;
  };

  SoftFloat::SoftFloat( SoftDouble const& source, Flags& flags )
    : expr(new FPConvNode<float,double,false>(source.expr,flags.rm))
  { flags.from( expr ); }

  template <typename T> void GenFPSCR_OX(U64& r, Flags const& f) { r.expr = make_function("GenFPSCR_OX",r.expr,f.op); }
  template <typename T> void GenFPSCR_UX(U64& r, Flags const& f) { r.expr = make_function("GenFPSCR_UX",r.expr,f.op); }
  template <typename T> void GenFPSCR_VXSNAN(U64& r, Flags const& f) { r.expr = make_function("GenFPSCR_VXSNAN",r.expr,f.op); }
  template <typename T> void GenFPSCR_VXISI(U64& r, Flags const& f) { r.expr = make_function("GenFPSCR_VXISI",r.expr,f.op); }
  template <typename T> void GenFPSCR_VXCVI(U64& r, Flags const& f) { r.expr = make_function("GenFPSCR_VXISI",r.expr,f.op); }
  template <typename T,typename D> void GenFPSCR_FR(U64& r, D const& d, Flags const& f) { r.expr = make_function("GenFPSCR_FR",r.expr,f.op); }
  template <typename T> void GenFPSCR_FI(U64& r, Flags const& f) { r.expr = make_function("GenFPSCR_FI",r.expr,f.op); }
  template <typename T,typename D> void GenFPSCR_FPRF(U64& r, D const& d) { r.expr = make_function("GenFPSCR_FI",r.expr,d.expr); }
  template <typename T> void GenFPSCR_XX(U64& r) { r.expr = make_function("GenFPSCR_XX",r.expr); }
  template <typename T> void GenFPSCR_FX(U64& r, U64& ar) { r.expr = make_function("GenFPSCR_FX",r.expr,ar.expr); }
  template <typename T> void GenFPSCR_VX(U64& r) { r.expr = make_function("GenFPSCR_VX",r.expr); }
  template <typename T> void GenFPSCR_FEX(U64& r) { r.expr = make_function("GenFPSCR_FEX",r.expr); }
  
  // struct SoftHalfFloat
  // {
  //   typedef unisim::util::symbolic::Expr Expr;
  //   typedef unisim::util::symbolic::ExprNode ExprNode;
    
  //   enum __FromRawBits__ { FromRawBits };
  //   SoftHalfFloat( __FromRawBits__, U16 const& source ) : expr(new FromRawBitsNode<uint16_t>( source.expr )) {}
  //   SoftHalfFloat( SoftFloat const& source, Flags& flags )
  //     : expr(new FPConvNode<float,float,false>(source.expr,flags.rm)) { flags.from( expr ); }
  //   U16 queryRawBits() { return Expr(new FromRawBitsNode<float>(expr)); }

  //   Expr expr;
  // };
  

  // SoftFloat::SoftFloat( SoftHalfFloat const& source, Flags& flags )
  //   : expr(new FPConvNode<float,float,false>(source.expr,flags.rm))
  // { flags.from(expr); }
  
  // static const unsigned int RN_NEAREST = 0;
  // static const unsigned int RN_ZERO = 1;
  // static const unsigned int RN_UP = 2;
  // static const unsigned int RN_DOWN = 3;

  // inline void GenSPEFSCR_FOVF(SPEFSCR& spefscr, const Flags& flags) { spefscr = make_unknown<U32>(arch); }
  // inline void GenSPEFSCR_FUNF(SPEFSCR& spefscr, const Flags& flags) { spefscr = make_unknown<U32>(arch); }
  // inline void GenSPEFSCR_FINXS(SPEFSCR& spefscr, const Flags& flags) { spefscr = make_unknown<U32>(arch); }
  // inline void GenSPEFSCR_FDBZ(SPEFSCR& spefscr, const Flags& flags) { spefscr = make_unknown<U32>(arch); }
  // inline void GenSPEFSCR_FG(SPEFSCR& spefscr, const Flags& flags, bool x=false) { spefscr = make_unknown<U32>(arch); }
  // inline void GenSPEFSCR_FX(SPEFSCR& spefscr, const Flags& flags, bool x=false) { spefscr = make_unknown<U32>(arch); }
  // inline void GenSPEFSCR_FG(SPEFSCR& spefscr, const SoftFloat& result) { spefscr = make_unknown<U32>(arch); }
  // inline BOOL HasSPEFSCR_InvalidInput(const SoftFloat& input) { return make_unknown<BOOL>(arch); }
  // template <class RESULT>
  // inline void GenSPEFSCR_DefaultResults(SPEFSCR& spefscr, RESULT& result) { spefscr = make_unknown<U32>(arch); }
  // template <class FLOAT>
  // inline void GenSPEFSCR_FINV(SPEFSCR& spefscr, FLOAT& first, FLOAT* second = 0, FLOAT* third = 0, bool x=false, bool y=false) { spefscr = make_unknown<U32>(arch); }

  // inline BOOL DoesSPEFSCR_TriggerException(const SPEFSCR& spefscr) { return BOOL(false); }
  
  // inline void GenSPEFSCR_TriggerException(Arch* cpu) { cpu->donttest_system(); }

}

#endif // E5500_SIMTEST_SIMFLOAT_HH
