/*
 *  Copyright (c) 2007-2017,
 *  Commissariat a l'Energie Atomique (CEA),
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
 
#ifndef __UNISIM_UTIL_SYMBOLIC_SYMBOLIC_HH__
#define __UNISIM_UTIL_SYMBOLIC_SYMBOLIC_HH__

#include <unisim/util/arithmetic/arithmetic.hh>
#include <ostream>
#include <stdexcept>
#include <limits>
#include <typeinfo>
#include <cstring>
#include <inttypes.h>

namespace unisim {
namespace util {
namespace symbolic {

  template <typename Bool> struct AssertBool {};
  template <>              struct AssertBool<bool> { static void check() {} };
  
  template <bool test> struct StaticAssert {};
  template <> struct StaticAssert<true> { static void check() {}; };
  
  template <class T, class U>  struct CmpTypes { static bool const same = false; };

  template <class T>  struct CmpTypes<T,T> { static bool const same = true; };
  
  struct Expr;
  
  struct ConstNodeBase;
  struct OpNodeBase;
  
  struct ExprNode
  {
    virtual ~ExprNode() {}
    ExprNode() : refs(0) {}
    mutable uintptr_t refs;
    void Retain() const { ++refs; }
    void Release() const { if (--refs == 0) delete this; }
    /* Generic functions */
    virtual unsigned SubCount() const = 0;
    virtual Expr const& GetSub(unsigned idx) const { throw std::logic_error("out of bound sub expression"); }
    virtual void Repr( std::ostream& sink ) const = 0;
    virtual intptr_t cmp( ExprNode const& ) const = 0;
    virtual ConstNodeBase const* GetConstNode() const { return 0; };
    virtual OpNodeBase const* AsOpNode() const { return 0; }
  };
  
  template <class EnumCLASS>
  void cstr2enum( EnumCLASS& ecl, char const* src )
  {
    for (int idx = 1, end = EnumCLASS::end; idx != end; ++idx)
      {
        typedef typename EnumCLASS::Code Code;
        Code code = Code(idx);
        if (strcmp(EnumCLASS(code).c_str(), src) == 0)
          { ecl.code = code; break; }
      }
  }

  struct Op
  {
    enum Code
      {
        NA = 0,
        Xor, And, Or,
        Ror, Rol, Lsl, Asr, Lsr,
        Add, Sub, Div, Mod, Mul,
        Teq, Tne, Tge, Tgt, Tle, Tlt, Tgeu, Tgtu, Tleu, Tltu,
        BSwp, BSR, BSF, Not, Neg,
        FCmp,FSQB, FFZ, FNeg, FSqrt, FAbs, FDen,
        Cast,
        end
      } code;
    
    
    char const* c_str() const
    {
      switch (code)
        {
        case   Xor: return "Xor";
        case   And: return "And";
        case    Or: return "Or";
        case   Add: return "Add";
        case   Sub: return "Sub";
        case   Div: return "Div";
        case   Mod: return "Mod";
        case   Mul: return "Mul";
        case   Ror: return "Ror";
        case   Rol: return "Rol";
        case   Lsl: return "Lsl";
        case   Asr: return "Asr";
        case   Lsr: return "Lsr";
        case   Teq: return "Teq";
        case   Tne: return "Tne";
        case   Tge: return "Tge";
        case   Tgt: return "Tgt";
        case   Tle: return "Tle";
        case   Tlt: return "Tlt";
        case  Tgeu: return "Tgeu";
        case  Tgtu: return "Tgtu";
        case  Tleu: return "Tleu";
        case  Tltu: return "Tltu";
        case  FCmp: return "FCmp";
        case  BSwp: return "BSwp";
        case   BSR: return "BSR";
        case   BSF: return "BSF";
        case   Not: return "Not";
        case   Neg: return "Neg";
        case  FSQB: return "FSQB";
        case   FFZ: return "FFZ";
        case  FNeg: return "FNeg";
        case FSqrt: return "FSqrt";
        case  FAbs: return "FAbs";
        case  FDen: return "FDen";
        case    NA: return "NA";
        case  Cast: return "Cast";
        case   end: break;
        }
      return "INVALID";
    }
    
    Op( Code _code ) : code(_code) {}
    Op( char const* _code ) : code(NA) { cstr2enum( *this, _code ); }
    intptr_t cmp( Op rhs ) const { return intptr_t(code) - intptr_t(rhs.code); }
  };
  
  struct ScalarType
  {
    enum id_t { BOOL, U8, U16, U32, U64, S8, S16, S32, S64, F32, F64 };
    ScalarType( id_t id )
      : name(0), bitsize(0), is_signed(false), is_integer(false)
    {
      switch (id)
        {
        case BOOL: bitsize = 1;  is_integer = true;  is_signed = false; name = "BOOL"; break;
        case U8:   bitsize = 8;  is_integer = true;  is_signed = false; name = "U8";  break;
        case S8:   bitsize = 8;  is_integer = true;  is_signed = true;  name = "S8";  break;
        case U16:  bitsize = 16; is_integer = true;  is_signed = false; name = "U16"; break;
        case S16:  bitsize = 16; is_integer = true;  is_signed = true;  name = "S16"; break;
        case U32:  bitsize = 32; is_integer = true;  is_signed = false; name = "U32"; break;
        case S32:  bitsize = 32; is_integer = true;  is_signed = true;  name = "S32"; break;
        case U64:  bitsize = 64; is_integer = true;  is_signed = false; name = "U64"; break;
        case S64:  bitsize = 64; is_integer = true;  is_signed = true;  name = "S64"; break;
        case F32:  bitsize = 32; is_integer = false; is_signed = true;  name = "F32"; break;
        case F64:  bitsize = 64; is_integer = false; is_signed = true;  name = "F64"; break;
        }
    }
    char const* name;
    unsigned bitsize;
    bool is_signed, is_integer;
  };
  
  template <typename VALUE_TYPE>
  struct TypeInfo
  {
    static void Repr( std::ostream& sink, VALUE_TYPE v )
    {
      sink << (std::numeric_limits<VALUE_TYPE>::is_signed ? 'S' : 'U')
           << (8*sizeof(VALUE_TYPE)) << "( 0x"
           << std::hex << uint64_t(v) << " )"<< std::dec;
    };
    static unsigned bitsize() { return 8*sizeof(VALUE_TYPE); }
    static ScalarType::id_t GetType()
    {
      if (std::numeric_limits<VALUE_TYPE>::is_integer)
        {
          bool is_signed = std::numeric_limits<VALUE_TYPE>::is_signed;
          //int bits = std::numeric_limits<VALUE_TYPE>::digits + (is_signed ? 1 : 0);
          int bits = 8*sizeof(VALUE_TYPE);
          switch (bits) {
          case 8:  return is_signed ? ScalarType::S8 :  ScalarType::U8;
          case 16: return is_signed ? ScalarType::S16 : ScalarType::U16;
          case 32: return is_signed ? ScalarType::S32 : ScalarType::U32;
          case 64: return is_signed ? ScalarType::S64 : ScalarType::U64;
          }
        }
      throw std::logic_error("not an integer type");
    }
  };
  
  template <> struct TypeInfo<bool>
  {
    static void Repr( std::ostream& sink, bool v ) { sink << "BOOL( " << int(v) << " )"; }
    static unsigned bitsize() { return 1; }
    static ScalarType::id_t GetType() { return ScalarType::BOOL; }
  };
  template <> struct TypeInfo<float>
  {
    static void Repr( std::ostream& sink, float v ) { sink << "F32( " << v << " )"; }
    static unsigned bitsize() { return 32; }
    static ScalarType::id_t GetType() { return ScalarType::F32; }
  };
  template <> struct TypeInfo<double>
  {
    static void Repr( std::ostream& sink, double v ) { sink << "F64( " << v << " )"; }
    static unsigned bitsize() { return 64; }
    static ScalarType::id_t GetType() { return ScalarType::F64; }
  };
  
  struct ConstNodeBase : public ExprNode
  {
    virtual unsigned SubCount() const { return 0; };
    ConstNodeBase const* GetConstNode() const { return this; };
    virtual ConstNodeBase* apply( Op op, ConstNodeBase const** args ) const = 0;
    virtual float GetFloat() const = 0;
    virtual double GetDouble() const = 0;
    virtual bool GetBoolean() const = 0;
    virtual uint8_t GetU8() const = 0;
    virtual uint16_t GetU16() const = 0;
    virtual uint32_t GetU32() const = 0;
    virtual uint64_t GetU64() const = 0;
    virtual int8_t GetS8() const = 0;
    virtual int16_t GetS16() const = 0;
    virtual int32_t GetS32() const = 0;
    virtual int64_t GetS64() const = 0;
    virtual ScalarType::id_t GetType() const = 0;
    static std::ostream& warn();
  };
  
  template <typename VALUE_TYPE>
  VALUE_TYPE EvalXor( VALUE_TYPE l, VALUE_TYPE r ) { return l ^ r; }
  double     EvalXor( double l, double r );
  float      EvalXor( float l, float r );
  template <typename VALUE_TYPE>
  VALUE_TYPE EvalAnd( VALUE_TYPE l, VALUE_TYPE r ) { return l & r; }
  double     EvalAnd( double l, double r );
  float      EvalAnd( float l, float r );
  template <typename VALUE_TYPE>
  VALUE_TYPE EvalOr( VALUE_TYPE l, VALUE_TYPE r ) { return l | r; }
  double     EvalOr( double l, double r );
  float      EvalOr( float l, float r );
  template <typename VALUE_TYPE>
  VALUE_TYPE EvalNot( VALUE_TYPE val ) { return ~val; }
  inline bool EvalNot( bool val ) { return not val; }
  double     EvalNot( double val );
  float      EvalNot( float val );

  template <typename VALUE_TYPE>
  VALUE_TYPE EvalSHL( VALUE_TYPE l, uint8_t shift ) { return l << shift; }
  double     EvalSHL( double, uint8_t );
  float      EvalSHL( float, uint8_t );
  template <typename VALUE_TYPE>
  VALUE_TYPE EvalSHR( VALUE_TYPE l, uint8_t shift ) { return l >> shift; }
  double     EvalSHR( double, uint8_t );
  float      EvalSHR( float, uint8_t );
  template <typename VALUE_TYPE>
  VALUE_TYPE EvalByteSwap( VALUE_TYPE v ) { throw std::logic_error( "No ByteSwap for this type" ); }
  uint32_t   EvalByteSwap( uint32_t v );
  uint16_t   EvalByteSwap( uint16_t v );
  template <typename VALUE_TYPE>
  VALUE_TYPE EvalBitScanReverse( VALUE_TYPE v ) { throw std::logic_error( "No BitScanReverse for this type" ); }
  uint32_t   EvalBitScanReverse( uint32_t v );
  template <typename VALUE_TYPE>
  VALUE_TYPE EvalBitScanForward( VALUE_TYPE v ) { throw std::logic_error( "No BitScanForward for this type" ); }
  uint32_t   EvalBitScanForward( uint32_t v );
  template <typename VALUE_TYPE>
  VALUE_TYPE EvalRotateRight( VALUE_TYPE v, uint8_t shift ) { throw std::logic_error( "No RotateRight for this type" ); }
  uint32_t   EvalRotateRight( uint32_t v, uint8_t shift );
  template <typename VALUE_TYPE>
  VALUE_TYPE EvalRotateLeft( VALUE_TYPE v, uint8_t shift ) { throw std::logic_error( "No RotateLeft for this type" ); }
  uint32_t   EvalRotateLeft( uint32_t v, uint8_t shift );
  
  struct OpNodeBase : public ExprNode
  {
    OpNodeBase( Op _op ) : op(_op) {}
    virtual OpNodeBase const* AsOpNode() const { return this; }
    
    Op op;
  };
  
  template <typename VALUE_TYPE>
  struct ConstNode : public ConstNodeBase
  {
    ConstNode( VALUE_TYPE _value ) : value( _value ) {} VALUE_TYPE value;
    
    intptr_t cmp( ExprNode const& brhs ) const
    {
      ConstNode<VALUE_TYPE> const& rhs = dynamic_cast<ConstNode<VALUE_TYPE> const&>( brhs );
      return (value < rhs.value) ? -1 : (value > rhs.value) ? +1 : 0;
    }
    
    virtual void Repr( std::ostream& sink ) const
    {
      TypeInfo<VALUE_TYPE>::Repr( sink, value );
    }
    
    static VALUE_TYPE GetValue( ConstNodeBase const* cnb );
  
    ConstNodeBase*
    apply( Op op, ConstNodeBase const** args ) const
    {
      switch (op.code)
        {
        case Op::BSwp:  return new ConstNode<VALUE_TYPE>( EvalByteSwap( value ) );
        case Op::Not:   return new ConstNode<VALUE_TYPE>( EvalNot( value ) );
        case Op::Neg:   return new ConstNode<VALUE_TYPE>( - value );
        case Op::BSR:   return new ConstNode<VALUE_TYPE>( EvalBitScanReverse( value ) );
        case Op::BSF:   return new ConstNode<VALUE_TYPE>( EvalBitScanForward( value ) );
        case Op::FSQB:  break;
        case Op::FFZ:   break;
        case Op::FNeg:  break;
        case Op::FSqrt: break;
        case Op::FAbs:  break;
        case Op::FDen:  break;
        case Op::Xor:   return new ConstNode<VALUE_TYPE>( EvalXor( value, GetValue( args[1] ) ) );
        case Op::And:   return new ConstNode<VALUE_TYPE>( EvalAnd( value, GetValue( args[1] ) ) );
        case Op::Or:    return new ConstNode<VALUE_TYPE>( EvalOr( value, GetValue( args[1] ) ) );
        case Op::Lsl:   return new ConstNode<VALUE_TYPE>( EvalSHL( value, args[1]->GetU8() ) );
        case Op::Lsr:   
        case Op::Asr:   return new ConstNode<VALUE_TYPE>( EvalSHR( value, args[1]->GetU8() ) );
        case Op::Add:   return new ConstNode<VALUE_TYPE>( value + GetValue( args[1] ) );
        case Op::Sub:   return new ConstNode<VALUE_TYPE>( value - GetValue( args[1] ) );
        case Op::Mul:   return new ConstNode<VALUE_TYPE>( value * GetValue( args[1] ) );
        case Op::Div:   return new ConstNode<VALUE_TYPE>( value / GetValue( args[1] ) );
        case Op::Teq:   return new ConstNode   <bool>   ( value == GetValue( args[1] ) );
        case Op::Tne:   return new ConstNode   <bool>   ( value != GetValue( args[1] ) );
        case Op::Tleu:  
        case Op::Tle:   return new ConstNode   <bool>   ( value <= GetValue( args[1] ) );
        case Op::Tltu:  
        case Op::Tlt:   return new ConstNode   <bool>   ( value <  GetValue( args[1] ) );
        case Op::Tgeu:  
        case Op::Tge:   return new ConstNode   <bool>   ( value >= GetValue( args[1] ) );
        case Op::Tgtu:  
        case Op::Tgt:   return new ConstNode   <bool>   ( value >  GetValue( args[1] ) );
        case Op::Ror:   return new ConstNode<VALUE_TYPE>( EvalRotateRight( value, args[1]->GetU8() ) );
        case Op::Rol:   return new ConstNode<VALUE_TYPE>( EvalRotateLeft( value, args[1]->GetU8() ) );
        case Op::Mod:   break;
        case Op::FCmp:  break;
          
        case Op::NA:
        case Op::Cast: /* Should have been handled elsewhere */
        case Op::end:   throw std::logic_error("???");
        }
      
      warn() << "Unhandled unary operation: " << op.c_str() << "\n";
      return 0;
    }
    float GetFloat() const { return value; }
    double GetDouble() const { return value; }
    bool GetBoolean() const { return value; }
    uint8_t GetU8() const { return value; }
    uint16_t GetU16() const { return value; }
    uint32_t GetU32() const { return value; }
    uint64_t GetU64() const { return value; }
    int8_t GetS8() const { return value; }
    int16_t GetS16() const { return value; }
    int32_t GetS32() const { return value; }
    int64_t GetS64() const { return value; }
    ScalarType::id_t GetType() const { return TypeInfo<VALUE_TYPE>::GetType(); }
  };
  
  struct Expr
  {
    Expr() : node() {} ExprNode const* node;
    Expr( Expr const& expr ) : node( expr.node ) { if (node) node->Retain(); }
    Expr( ExprNode const* _node ) : node( _node ) { if (node) node->Retain(); }
    ~Expr() { if (node) node->Release(); }
    
    Expr&  operator = ( Expr const& expr )
    {
      if (expr.node) expr.node->Retain();
      ExprNode const* old_node = node;
      node = expr.node;
      if (old_node) old_node->Release();
      return *this;
    }
    
    ExprNode const* operator->() const { return node; }
    ExprNode const* operator->() { return node; }
    ExprNode const& operator* () const { return *node; }
    
    intptr_t cmp( Expr const& rhs ) const
    {
      /* First compare actual types */
      const std::type_info& til = typeid(*node);
      const std::type_info& tir = typeid(*rhs.node);
      if (intptr_t delta = &til - &tir)
        return delta;
      /* Same types, call derived comparator */
      return node->cmp( *rhs.node );
    }
    
    bool operator != ( Expr const& rhs ) const { return cmp( rhs ) != 0; }
    bool operator == ( Expr const& rhs ) const { return cmp( rhs ) == 0; }
    bool operator  < ( Expr const& rhs ) const { return cmp( rhs )  < 0; }
    bool operator  > ( Expr const& rhs ) const { return cmp( rhs )  > 0; }
    
    ConstNodeBase const* ConstSimplify()
    {
      if (ConstNodeBase const* cn = node->GetConstNode())
        {
          *this = Expr( cn );
          return cn;
        }
      
      return 0;
    }
    
    bool good() const { return node; }
    friend std::ostream& operator << (std::ostream&, Expr const&);
  };
  
  template <typename VALUE_TYPE>
  VALUE_TYPE ConstNode<VALUE_TYPE>::GetValue( ConstNodeBase const* cnb ) { return dynamic_cast<ConstNode<VALUE_TYPE> const&>( *cnb ).value; }

  template <typename VALUE_TYPE>
  Expr make_const( VALUE_TYPE value ) { return Expr( new ConstNode<VALUE_TYPE>( value ) ); }
  
  template <unsigned SUBCOUNT>
  struct OpNode : public OpNodeBase
  {
    typedef OpNode<SUBCOUNT> this_type;
    
    OpNode( Op _op ) : OpNodeBase(_op) {}

    virtual ConstNodeBase const* GetConstNode() const
    {
      Expr args[SUBCOUNT];
      ConstNodeBase const* cnbs[SUBCOUNT];
      for (unsigned idx = 0; idx < SUBCOUNT; ++idx)
        {
          args[idx] = subs[idx];
          if (not (cnbs[idx] = args[idx].ConstSimplify()))
            return 0;
        }
      return cnbs[0]->apply( op, &cnbs[0] );
    }
    virtual unsigned SubCount() const { return SUBCOUNT; };
    virtual Expr const& GetSub(unsigned idx) const
    {
      if (idx >= SUBCOUNT)
        return ExprNode::GetSub(idx);
      return subs[idx];
    };
    virtual intptr_t cmp( ExprNode const& brhs ) const
    {
      this_type const& rhs = dynamic_cast<this_type const&>( brhs );
      
      if (intptr_t delta = op.cmp( rhs.op ))
        return delta;
      for (unsigned idx = 0; idx < SUBCOUNT; ++idx)
        if (intptr_t delta = subs[idx].cmp( rhs.subs[idx] ))
          return delta;
      return 0;
    }
    
    virtual void Repr( std::ostream& sink ) const
    {
      sink << op.c_str() << "( ";
      char const* sep = "";
      for (unsigned idx = 0; idx < SUBCOUNT; ++idx)
        sink << sep << subs[idx];
      sink << " )";
    }
    
    Expr subs[SUBCOUNT];
  };
  
  struct CastNodeBase : public OpNodeBase
  {
    CastNodeBase( Expr const& _src ) : OpNodeBase( Op::Cast ), src(_src) {}
    virtual ScalarType::id_t GetSrcType() const = 0;
    virtual ScalarType::id_t GetDstType() const = 0;
    virtual unsigned SubCount() const { return 1; };
    virtual Expr const& GetSub(unsigned idx) const { if (idx!= 0) return ExprNode::GetSub(idx); return src; }
    Expr src;
  };
  
  template <typename DST_VALUE_TYPE, typename SRC_VALUE_TYPE>
  struct CastNode : public CastNodeBase
  {
    CastNode( Expr const& _src ) : CastNodeBase( _src ) {}
    
    virtual ScalarType::id_t GetSrcType() const { return TypeInfo<SRC_VALUE_TYPE>::GetType(); }
    virtual ScalarType::id_t GetDstType() const { return TypeInfo<DST_VALUE_TYPE>::GetType(); }
    
    intptr_t cmp( ExprNode const& brhs ) const
    {
      CastNode<DST_VALUE_TYPE,SRC_VALUE_TYPE> const& rhs = dynamic_cast<CastNode<DST_VALUE_TYPE,SRC_VALUE_TYPE> const&>( brhs );
      return src.cmp( rhs.src );
    }
    
    virtual void Repr( std::ostream& sink ) const { sink << ScalarType( TypeInfo<DST_VALUE_TYPE>::GetType() ).name; sink << "( "; src->Repr(sink); sink << " )"; }
    
    virtual ConstNodeBase const* GetConstNode() const
    {
      
      if (ConstNodeBase const* cnb = src->GetConstNode())
        {
          if (CmpTypes<DST_VALUE_TYPE,   float>::same) return new ConstNode<   float>( cnb->GetFloat() );
          if (CmpTypes<DST_VALUE_TYPE,  double>::same) return new ConstNode<  double>( cnb->GetDouble() );
          if (CmpTypes<DST_VALUE_TYPE,    bool>::same) return new ConstNode<    bool>( cnb->GetBoolean() );
          if (CmpTypes<DST_VALUE_TYPE, uint8_t>::same) return new ConstNode< uint8_t>( cnb->GetU8() );
          if (CmpTypes<DST_VALUE_TYPE,uint16_t>::same) return new ConstNode<uint16_t>( cnb->GetU16() );
          if (CmpTypes<DST_VALUE_TYPE,uint32_t>::same) return new ConstNode<uint32_t>( cnb->GetU32() );
          if (CmpTypes<DST_VALUE_TYPE,uint64_t>::same) return new ConstNode<uint64_t>( cnb->GetU64() );
          if (CmpTypes<DST_VALUE_TYPE,  int8_t>::same) return new ConstNode<  int8_t>( cnb->GetS8() );
          if (CmpTypes<DST_VALUE_TYPE, int16_t>::same) return new ConstNode< int16_t>( cnb->GetS16() );
          if (CmpTypes<DST_VALUE_TYPE, int32_t>::same) return new ConstNode< int32_t>( cnb->GetS32() );
          if (CmpTypes<DST_VALUE_TYPE, int64_t>::same) return new ConstNode< int64_t>( cnb->GetS64() );
          throw std::logic_error("Unkown Type");
        }
      return 0;
    }
  };
  
  struct UONode : public OpNode<1>
  {
    UONode( Op _op, Expr const& _src ) : OpNode<1>( _op ) { subs[0] = _src; }
  };
  
  struct BONode : public OpNode<2>
  {
    BONode( Op _op, Expr const& _left, Expr const& _right ) : OpNode<2>(_op) { subs[0] = _left; subs[1] = _right; }
  };
  
  template <typename VALUE_TYPE>
  struct SmartValue
  {
    typedef VALUE_TYPE value_type;
    typedef SmartValue<value_type> this_type;
    
    Expr expr;
    
    SmartValue() : expr( make_const( value_type() ) ) {}
    
    SmartValue( Expr const& _expr )
      : expr( _expr )
    {
      expr.ConstSimplify();
    }
    
    explicit SmartValue( value_type value ) : expr( make_const( value ) ) {}

    template <typename SRC_VALUE_TYPE>
    explicit SmartValue( SmartValue<SRC_VALUE_TYPE> const& other )
    {
      if (CmpTypes<SRC_VALUE_TYPE,VALUE_TYPE>::same) {
        expr = other.expr;
      } else {
        expr = new CastNode<VALUE_TYPE,SRC_VALUE_TYPE>( other.expr );
        expr.ConstSimplify();
      }
    }

    static bool const is_signed = std::numeric_limits<value_type>::is_signed;
    
    this_type& operator = ( this_type const& other ) { expr = other.expr; return *this; }
    
    template <typename SHIFT_TYPE>
    this_type operator << ( SHIFT_TYPE shift ) const { return this_type( Expr( new BONode( "Lsl", expr, make_const( shift ) ) ) ); }
    template <typename SHIFT_TYPE>
    this_type operator >> ( SHIFT_TYPE shift ) const { return this_type( Expr( new BONode(is_signed?"Asr":"Lsr",expr,make_const(shift)) ) ); }
    template <typename SHIFT_TYPE>
    this_type& operator <<= ( SHIFT_TYPE shift ) { expr = new BONode( "Lsl", expr, make_const( shift ) ); return *this; }
    template <typename SHIFT_TYPE>
    this_type& operator >>= ( SHIFT_TYPE shift ) { expr = new BONode(is_signed?"Asr":"Lsr",expr,make_const(shift)); return *this; }
    
    template <typename SHIFT_TYPE>
    this_type operator << ( SmartValue<SHIFT_TYPE> const& other ) const { return this_type( Expr( new BONode( "Lsl", expr, other.expr ) ) ); }
    template <typename SHIFT_TYPE>
    this_type operator >> ( SmartValue<SHIFT_TYPE> const& other ) const { return this_type( Expr( new BONode(is_signed?"Asr":"Lsr",expr,other.expr) ) ); }
    
    this_type operator - () const { return this_type( Expr( new UONode( "Neg", expr ) ) ); }
    this_type operator ~ () const { return this_type( Expr( new UONode( "Not", expr ) ) ); }
    
    this_type& operator += ( this_type const& other ) { expr = new BONode( "Add", expr, other.expr ); return *this; }
    this_type& operator -= ( this_type const& other ) { expr = new BONode( "Sub", expr, other.expr ); return *this; }
    this_type& operator *= ( this_type const& other ) { expr = new BONode( "Mul", expr, other.expr ); return *this; }
    this_type& operator /= ( this_type const& other ) { expr = new BONode( "Div", expr, other.expr ); return *this; }
    this_type& operator %= ( this_type const& other ) { expr = new BONode( "Mod", expr, other.expr ); return *this; }
    this_type& operator ^= ( this_type const& other ) { expr = new BONode( "Xor", expr, other.expr ); return *this; }
    this_type& operator &= ( this_type const& other ) { expr = new BONode( "And", expr, other.expr ); return *this; }
    this_type& operator |= ( this_type const& other ) { expr = new  BONode( "Or", expr, other.expr ); return *this; }
    
    this_type operator + ( this_type const& other ) const { return this_type( Expr( new BONode( "Add", expr, other.expr ) ) ); }
    this_type operator - ( this_type const& other ) const { return this_type( Expr( new BONode( "Sub", expr, other.expr ) ) ); }
    this_type operator * ( this_type const& other ) const { return this_type( Expr( new BONode( "Mul", expr, other.expr ) ) ); }
    this_type operator / ( this_type const& other ) const { return this_type( Expr( new BONode( "Div", expr, other.expr ) ) ); }
    this_type operator % ( this_type const& other ) const { return this_type( Expr( new BONode( "Mod", expr, other.expr ) ) ); }
    this_type operator ^ ( this_type const& other ) const { return this_type( Expr( new BONode( "Xor", expr, other.expr ) ) ); }
    this_type operator & ( this_type const& other ) const { return this_type( Expr( new BONode( "And", expr, other.expr ) ) ); }
    this_type operator | ( this_type const& other ) const { return this_type( Expr( new  BONode( "Or", expr, other.expr ) ) ); }
    
    SmartValue<bool> operator == ( this_type const& other ) const { return SmartValue<bool>( Expr( new BONode( "Teq", expr, other.expr ) ) ); }
    SmartValue<bool> operator != ( this_type const& other ) const { return SmartValue<bool>( Expr( new BONode( "Tne", expr, other.expr ) ) ); }
    SmartValue<bool> operator <= ( this_type const& other ) const { return SmartValue<bool>( Expr( new BONode( is_signed ? "Tle" : "Tleu", expr, other.expr ) ) ); }
    SmartValue<bool> operator >= ( this_type const& other ) const { return SmartValue<bool>( Expr( new BONode( is_signed ? "Tge" : "Tgeu", expr, other.expr ) ) ); }
    SmartValue<bool> operator < ( this_type const& other ) const  { return SmartValue<bool>( Expr( new BONode( is_signed ? "Tlt" : "Tltu", expr, other.expr ) ) ); }
    SmartValue<bool> operator > ( this_type const& other ) const  { return SmartValue<bool>( Expr( new BONode( is_signed ? "Tgt" : "Tgtu", expr, other.expr ) ) ); }
    
    SmartValue<bool> operator ! () const
    { AssertBool<value_type>::check(); return SmartValue<bool>( Expr( new UONode( "Not", expr ) ) ); }

    SmartValue<bool> operator && ( SmartValue<bool> const& other ) const
    { AssertBool<value_type>::check(); return SmartValue<bool>( Expr( new BONode( "And", expr, other.expr ) ) ); }
    
    SmartValue<bool> operator || ( SmartValue<bool> const& other ) const
    { AssertBool<value_type>::check(); return SmartValue<bool>( Expr( new  BONode( "Or", expr, other.expr ) ) ); }
  };
  
  template <typename UTP>
  UTP ByteSwap( UTP const& value ) { return UTP( new UONode( "BSwp", value.expr ) ); }
  
  template <typename UTP>
  UTP RotateRight( UTP const& value, unsigned shift ) { return UTP( new BONode( "Ror", value.expr, make_const( shift ) ) ); }
  template <typename UTP>
  UTP RotateRight( UTP const& value, UTP const& shift ) { return UTP( new BONode( "Ror", value.expr, shift.expr ) ); }
  
  template <typename UTP>
  UTP RotateLeft( UTP const& value, unsigned shift ) { return UTP( new BONode( "Rol", value.expr, make_const( shift ) ) ); }
  template <typename UTP>
  UTP RotateLeft( UTP const& value, UTP const& shift ) { return UTP( new BONode( "Rol", value.expr, shift.expr ) ); }
  
  template <typename UTP>
  UTP BitScanReverse( UTP const& value ) { return UTP( new UONode( "BSR", value.expr ) ); }
  
  template <typename UTP>
  UTP BitScanForward( UTP const& value ) { return UTP( new UONode( "BSF", value.expr ) ); }
  
  template <typename T>
  SmartValue<T> power( SmartValue<T> const& left, SmartValue<T> const& right ) { return SmartValue<T>( new BONode( "Pow", left.expr, right.expr ) ); }
  
  template <typename T>
  SmartValue<T> fmodulo( SmartValue<T> const& left, SmartValue<T> const& right ) { return SmartValue<T>( new BONode( "FMod", left.expr, right.expr ) ); }
  
  struct FP
  {
    struct DefaultNaNNode : public ExprNode
    {
      DefaultNaNNode( int _fsz ) : fsz( _fsz ) {} int fsz;
      virtual void Repr( std::ostream& sink ) const { sink << "DefaultNaN()"; }
      virtual unsigned SubCount() const { return 0; };
      intptr_t cmp( ExprNode const& brhs ) const
      {
        DefaultNaNNode const& rhs = dynamic_cast<DefaultNaNNode const&>( brhs );
        return fsz - rhs.fsz;
      }
    };
    
    static inline
    void SetDefaultNan( SmartValue<float>& result )
    { result = SmartValue<float>( Expr( new DefaultNaNNode( 32 ) ) ); }
    
    static inline
    void SetDefaultNan( SmartValue<double>& result )
    { result = SmartValue<double>( Expr( new DefaultNaNNode( 64 ) ) ); }
    
    template <typename FLOAT> static
    void SetQuietBit( FLOAT& op )
    {
      op = FLOAT( Expr( new UONode( "FSQB", op.expr ) ) );
    }

    template <typename FLOAT> static
    SmartValue<bool>
    FlushToZero( FLOAT& op, SmartValue<uint32_t> const& fpscr_val )
    {
      op = FLOAT( Expr( new UONode( "FFZ", op.expr ) ) );
      return SmartValue<bool>( Expr( new UONode( "FDen", op.expr ) ) );
    }

    template <typename FLOAT> static
    SmartValue<int32_t> Compare( FLOAT op1, FLOAT op2, SmartValue<uint32_t> const& fpscr_val )
    {
      return SmartValue<int32_t>( Expr( new BONode( "FCmp", op1.expr, op2.expr ) ) );
    }

    struct IsNaNNode : public ExprNode
    {
      IsNaNNode( Expr const& _src, bool _signaling ) : src(_src), signaling(_signaling) {} Expr src; bool signaling;
      virtual void Repr( std::ostream& sink ) const { sink << "IsNaN("; src->Repr(sink); sink << ")"; }
      intptr_t cmp( ExprNode const& brhs ) const
      {
        IsNaNNode const& rhs = dynamic_cast<IsNaNNode const&>( brhs );
        if (intptr_t delta = src.cmp( rhs.src )) return delta;
        return int(signaling) - int(rhs.signaling);
      }
      virtual unsigned SubCount() const { return 1; };
      virtual Expr const& GetSub(unsigned idx) const { if (idx != 0) return ExprNode::GetSub(idx); return src; };
    };
    
    template <typename FLOAT> static
    SmartValue<bool>
    IsSNaN( FLOAT const& op )
    {
      return SmartValue<bool>( Expr( new IsNaNNode( op.expr, true ) ) );
    }
    
    template <typename FLOAT> static
    SmartValue<bool>
    IsQNaN( FLOAT const& op )
    {
      return SmartValue<bool>( Expr( new IsNaNNode( op.expr, false ) ) );
    }
    
    template <typename FLOAT, class ARCH> static
    void Add( FLOAT& acc, FLOAT const& op2, ARCH& arch, SmartValue<uint32_t> const& fpscr_val )
    {
      acc = FLOAT( Expr( new BONode( "Add", acc.expr, op2.expr ) ) );
    }

    template <typename FLOAT, class ARCH> static
    void Sub( FLOAT& acc, FLOAT const& op2, ARCH& arch, SmartValue<uint32_t> const& fpscr_val )
    {
      acc = FLOAT( Expr( new BONode( "Sub", acc.expr, op2.expr ) ) );
    }

    template <typename FLOAT, class ARCH> static
    void Div( FLOAT& acc, FLOAT const& op2, ARCH& arch, SmartValue<uint32_t> const& fpscr_val )
    {
      acc = FLOAT( Expr( new BONode( "Div", acc.expr, op2.expr ) ) );
    }

    template <typename FLOAT, class ARCH> static
    void Mul( FLOAT& acc, FLOAT const& op2, ARCH& arch, SmartValue<uint32_t> const& fpscr_val )
    {
      acc = FLOAT( Expr( new BONode( "Mul", acc.expr, op2.expr ) ) );
    }

    struct MulAddNode : public ExprNode
    {
      MulAddNode( Expr const& _acc, Expr const& _left, Expr const& _right )
        : acc( _acc ), left( _left ), right( _right )
      {}
      
      Expr acc, left, right;
      
      virtual unsigned SubCount() const { return 3; };
      virtual Expr const& GetSub(unsigned idx) const { switch (idx) { case 0: return acc; case 1: return left; case 2: return right; } return ExprNode::GetSub(idx); };
      
      virtual void Repr( std::ostream& sink ) const
      {
        sink << "MulAdd( ";
        acc->Repr(sink);
        sink << ", ";
        left->Repr(sink);
        sink << ", ";
        right->Repr(sink);
        sink << " )";
      }
      
      
      intptr_t cmp( ExprNode const& brhs ) const
      {
        MulAddNode const& rhs = dynamic_cast<MulAddNode const&>( brhs );
        if (intptr_t delta = left.cmp( rhs.left )) return delta;
        if (intptr_t delta = right.cmp( rhs.right )) return delta;
        return acc.cmp( rhs.acc );
      }
    };
    
    template <typename FLOAT, class ARCH> static
    void MulAdd( FLOAT& acc, FLOAT const& op1, FLOAT const& op2, ARCH& arch, SmartValue<uint32_t> const& fpscr_val )
    {
      acc = FLOAT( Expr( new MulAddNode( acc.expr, op1.expr, op2.expr ) ) );
    }
    
    struct IsInvalidMulAddNode : public ExprNode
    {
      IsInvalidMulAddNode( Expr const& _acc, Expr const& _left, Expr const& _right )
        : acc( _acc ), left( _left ), right( _right )
      {}
      Expr acc, left, right;
      
      virtual unsigned SubCount() const { return 3; };
      virtual Expr const& GetSub(unsigned idx) const { switch (idx) { case 0: return acc; case 1: return left; case 2: return right; } return ExprNode::GetSub(idx); };
      
      virtual void Repr( std::ostream& sink ) const
      {
        sink << "IsInvalidMulAdd( ";
        acc->Repr(sink);
        sink << ", ";
        left->Repr(sink);
        sink << ", ";
        right->Repr(sink);
        sink << " )";
      }
      
      intptr_t cmp( ExprNode const& brhs ) const
      {
        IsInvalidMulAddNode const& rhs = dynamic_cast<IsInvalidMulAddNode const&>( brhs );
        if (intptr_t delta = left.cmp( rhs.left )) return delta;
        if (intptr_t delta = right.cmp( rhs.right )) return delta;
        return acc.cmp( rhs.acc );
      }
    };
    
    template <typename SOFTDBL> static
    SmartValue<bool>
    IsInvalidMulAdd( SOFTDBL const& acc, SOFTDBL const& op1, SOFTDBL const& op2, SmartValue<uint32_t> const& fpscr_val )
    {
      return SmartValue<bool>( Expr( new IsInvalidMulAddNode( acc.expr, op1.expr, op2.expr ) ) );
    }
    
    template <typename FLOAT> static
    void Neg( FLOAT& acc ) { acc = FLOAT( Expr( new UONode( "FNeg", acc.expr ) ) ); }

    template <typename FLOAT> static
    void Abs( FLOAT& acc ) { acc = FLOAT( Expr( new UONode( "FAbs", acc.expr ) ) ); }
    
    template <typename FLOAT, class ARCH> static
    void Sqrt( FLOAT& acc, ARCH& arch, SmartValue<uint32_t> const& fpscr_val ) { acc = FLOAT( Expr( new UONode( "FSqrt", acc.expr ) ) ); }

    struct FtoFNode : public ExprNode
    {
      FtoFNode( Expr const& _src, int _ssz, int _dsz )
        : src( _src ), ssz( _ssz ), dsz( _dsz )
      {} Expr src; int ssz; int dsz;
      
      virtual void Repr( std::ostream& sink ) const { sink << "FtoF( "; src->Repr(sink); sink << " )"; }
      virtual unsigned SubCount() const { return 1; }
      virtual Expr const& GetSub(unsigned idx) const { if (idx != 0) return ExprNode::GetSub(idx); return src; }
      intptr_t cmp( ExprNode const& brhs ) const
      {
        FtoFNode const& rhs = dynamic_cast<FtoFNode const&>( brhs );
        if (intptr_t delta = src.cmp( rhs.src )) return delta;
        if (intptr_t delta = ssz - rhs.ssz) return delta;
        return dsz - rhs.dsz;
      }
    };
    
    template <typename ofpT, typename ifpT, class ARCH> static
    void FtoF( SmartValue<ofpT>& dst, SmartValue<ifpT> const& src, ARCH& arch, SmartValue<uint32_t> const& fpscr_val )
    {
      dst = SmartValue<ofpT>( Expr( new FtoFNode( src.expr, TypeInfo<ifpT>::bitsize(), TypeInfo<ofpT>::bitsize() ) ) );
    }

    struct FtoINode : public ExprNode
    {
      FtoINode( Expr const& _src, int _fsz, int _isz, int _fb )
        : src( _src ), fsz( _fsz ), isz( _isz ), fb( _fb )
      {} Expr src; int fsz; int isz; int fb; 
      
      virtual void Repr( std::ostream& sink ) const { sink << "FtoI( "; src->Repr(sink); sink << " )"; }
      virtual unsigned SubCount() const { return 1; }
      virtual Expr const& GetSub(unsigned idx) const { if (idx != 0) return ExprNode::GetSub(idx); return src; }
      intptr_t cmp( ExprNode const& brhs ) const
      {
        FtoINode const& rhs = dynamic_cast<FtoINode const&>( brhs );
        if (intptr_t delta = src.cmp( rhs.src )) return delta;
        if (intptr_t delta = isz - rhs.isz) return delta;
        if (intptr_t delta = fsz - rhs.fsz) return delta;
        return fb - rhs.fb;
      }
    };

    template <typename intT, typename fpT, class ARCH> static
    void FtoI( SmartValue<intT>& dst, SmartValue<fpT> const& src, int fracbits, ARCH& arch, SmartValue<uint32_t> const& fpscr_val )
    {
      dst = SmartValue<intT>( Expr( new FtoINode( src.expr, TypeInfo<fpT>::bitsize(), TypeInfo<intT>::bitsize(), fracbits) ) );
    }

    struct ItoFNode : public ExprNode
    {
      ItoFNode( Expr const& _src, int _isz, int _fsz, int _fb )
        : src( _src ), isz( _isz ), fsz( _fsz ), fb( _fb )
      {} Expr src; int isz; int fsz; int fb;
      
      virtual void Repr( std::ostream& sink ) const { sink << "ItoF( "; src->Repr(sink); sink << " )"; }
      virtual unsigned SubCount() const { return 1; }
      virtual Expr const& GetSub(unsigned idx) const { if (idx != 0) return ExprNode::GetSub(idx); return src; }
      intptr_t cmp( ExprNode const& brhs ) const
      {
        ItoFNode const& rhs = dynamic_cast<ItoFNode const&>( brhs );
        if (intptr_t delta = src.cmp( rhs.src )) return delta;
        if (intptr_t delta = isz - rhs.isz) return delta;
        if (intptr_t delta = fsz - rhs.fsz) return delta;
        return fb - rhs.fb;
      }
    };
    
    template <typename fpT, typename intT, class ARCH> static
    void ItoF( SmartValue<fpT>& dst, SmartValue<intT> const& src, int fracbits, ARCH& arch, SmartValue<uint32_t> const& fpscr_val )
    {
      dst = SmartValue<fpT>( Expr( new ItoFNode( src.expr, TypeInfo<intT>::bitsize(), TypeInfo<fpT>::bitsize(), fracbits ) ) );
    }
    
  };

} /* end of namespace symbolic */
} /* end of namespace util */
} /* end of namespace unisim */

#endif /* __UNISIM_UTIL_SYMBOLIC_SYMBOLIC_HH__ */