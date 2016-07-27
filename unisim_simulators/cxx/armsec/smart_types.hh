#ifndef SMART_TYPES_HH
#define SMART_TYPES_HH

#include <limits>       // std::numeric_limits
#include <ostream>
#include <stdexcept>
#include <stdint.h>
#include <typeinfo>
// #include <cmath>

namespace armsec
{
  template <typename Bool> struct AssertBool {};
  template <>              struct AssertBool<bool> { static void check() {} };
  
  template <class T, class U>
  struct CmpTypes { static bool const same = false; };

  template <class T>
  struct CmpTypes<T,T> { static bool const same = true; };

  struct ExprNode
  {
    virtual ~ExprNode() {}
    ExprNode() : refs(0) {}
    uintptr_t refs;
    void Retain() { ++refs; }
    void Release() { if (--refs == 0) delete this; }
    /* Generic functions */
    struct Error {};
    struct Visitor
    {
      virtual ~Visitor() {}
      virtual void Process( ExprNode const* ) = 0;
    };
    virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); }
    virtual void Repr( std::ostream& sink ) const = 0;
    virtual intptr_t cmp( ExprNode const& ) const = 0;
    virtual ExprNode* GetConstNode() = 0;
  };
  
  template <typename ENUM_TYPE>
  struct Str2Enum
  {
    Str2Enum( std::string _src, ENUM_TYPE& _dst ) : src(_src), dst(_dst) {}
    std::string src; ENUM_TYPE& dst;
    bool operator() ( std::string strcode, ENUM_TYPE numcode ) const
    { if (strcode != src) return false; dst=numcode; return true; }
  };
    
  template <class ENUM_TYPE>
  struct Enum2Str
  {
    Enum2Str( ENUM_TYPE _src, std::string& _dst ) : src(_src), dst(_dst) {}
    ENUM_TYPE src; std::string& dst;
    bool operator() ( std::string strcode, ENUM_TYPE numcode ) const
    { if (numcode != src) return false; dst=strcode; return true; }
  };
    
  
  struct BinaryOp
  {
    enum Code {
      NA=0,
      Xor, And, Or,
      ROR, SHL, SHR,
      Add, Sub, Div, Mod, Mul,
      Teq, Tge, Tgt, Tle, Tlt, Tne, 
      FCmp
    };
    
    template <class SCANNER>
    static void
    ScanOp( SCANNER const& ops )
    {
      { static std::string const _("Xor"); if (ops( _, Xor)) return; }
      { static std::string const _("And"); if (ops( _, And)) return; }
      { static std::string const _( "Or"); if (ops( _,  Or)) return; }
      { static std::string const _("Add"); if (ops(_, Add)) return; }
      { static std::string const _("Sub"); if (ops(_, Sub)) return; }
      { static std::string const _("Div"); if (ops(_, Div)) return; }
      { static std::string const _("Mod"); if (ops(_, Mod)) return; }
      { static std::string const _("Mul"); if (ops(_, Mul)) return; }
      { static std::string const _("ROR"); if (ops(_, ROR)) return; }
      { static std::string const _("SHL"); if (ops(_, SHL)) return; }
      { static std::string const _("SHR"); if (ops(_, SHR)) return; }
      { static std::string const _("Teq"); if (ops(_, Teq)) return; }
      { static std::string const _("Tge"); if (ops(_, Tge)) return; }
      { static std::string const _("Tgt"); if (ops(_, Tgt)) return; }
      { static std::string const _("Tle"); if (ops(_, Tle)) return; }
      { static std::string const _("Tlt"); if (ops(_, Tlt)) return; }
      { static std::string const _("Tne"); if (ops(_, Tne)) return; }
      { static std::string const _("FCmp"); if (ops(_, FCmp)) return; }
    }
    
    BinaryOp( Code _code ) : code(_code) {}
    BinaryOp( std::string _code ) : code(NA) { ScanOp( Str2Enum<Code>( _code, code ) ); }
    BinaryOp( char const* _code ) : code(NA) { ScanOp( Str2Enum<Code>( _code, code ) ); }
    std::string ToString() const { std::string res("NA"); ScanOp( Enum2Str<Code>( code, res ) ); return res; }
    intptr_t cmp( BinaryOp rhs ) const { return intptr_t(code) - intptr_t(rhs.code); }
    
    Code code;
  };
  
  struct UnaryOp
  {
    enum Code {
      NA=0,
      BSwp, CLZ, Not,
      FSQB, FFZ, FNeg, FSqrt, FAbs, FDen
    };
    
    template <class SCANNER>
    static void
    ScanOp( SCANNER const& ops )
    {
      { static std::string const _("BSwp");  if (ops( _, BSwp)) return; }
      { static std::string const _("CLZ");   if (ops( _, CLZ)) return; }
      { static std::string const _("Not");   if (ops( _, Not)) return; }
      { static std::string const _("FSQB");  if (ops( _, FSQB)) return; }
      { static std::string const _("FFZ");   if (ops( _, FFZ)) return; }
      { static std::string const _("FNeg");  if (ops( _, FNeg)) return; }
      { static std::string const _("FSqrt"); if (ops( _, FSqrt)) return; }
      { static std::string const _("FAbs");  if (ops( _, FAbs)) return; }
      { static std::string const _("FDen");  if (ops( _, FDen)) return; }
    }
    
    UnaryOp( Code _code ) : code(_code) {}
    UnaryOp( std::string _code ) : code(NA) { ScanOp( Str2Enum<Code>( _code, code ) ); }
    UnaryOp( char const* _code ) : code(NA) { ScanOp( Str2Enum<Code>( _code, code ) ); }
    std::string ToString() const { std::string res("NA"); ScanOp( Enum2Str<Code>( code, res ) ); return res; }
    intptr_t cmp( UnaryOp rhs ) const { return intptr_t(code) - intptr_t(rhs.code); }
    
    Code code;
  };
  
  struct ConstNodeBase : public ExprNode
  {
    ExprNode* GetConstNode() { return this; };
    virtual ConstNodeBase* bop( BinaryOp op, ConstNodeBase& cnb ) const = 0;
    virtual ConstNodeBase* uop( UnaryOp op ) const = 0;
    virtual float GetFloat() const = 0;
    virtual double GetDouble() const = 0;
    virtual uint8_t GetU8() const = 0;
    virtual uint16_t GetU16() const = 0;
    virtual uint32_t GetU32() const = 0;
    virtual uint64_t GetU64() const = 0;
    virtual int8_t GetS8() const = 0;
    virtual int16_t GetS16() const = 0;
    virtual int32_t GetS32() const = 0;
    virtual int64_t GetS64() const = 0;
    static std::ostream& warn();
  };
  
  template <typename VALUE_TYPE>
  VALUE_TYPE BinaryXor( VALUE_TYPE l, VALUE_TYPE r ) { return l ^ r; }
  double BinaryXor( double l, double r );
  float BinaryXor( float l, float r );
  template <typename VALUE_TYPE>
  VALUE_TYPE BinaryAnd( VALUE_TYPE l, VALUE_TYPE r ) { return l & r; }
  double BinaryAnd( double l, double r );
  float BinaryAnd( float l, float r );
  template <typename VALUE_TYPE>
  VALUE_TYPE BinaryOr( VALUE_TYPE l, VALUE_TYPE r ) { return l | r; }
  double BinaryOr( double l, double r );
  float BinaryOr( float l, float r );
  // template <typename VALUE_TYPE>
  // VALUE_TYPE BinaryROR( VALUE_TYPE l, uint8_t shift ) { return rotate_right( l, shift ); }
  // double BinaryROR( double, uint8_t );
  // float BinaryROR( float, uint8_t );
  template <typename VALUE_TYPE>
  VALUE_TYPE BinarySHL( VALUE_TYPE l, uint8_t shift ) { return l << shift; }
  double BinarySHL( double, uint8_t );
  float BinarySHL( float, uint8_t );
  template <typename VALUE_TYPE>
  VALUE_TYPE BinarySHR( VALUE_TYPE l, uint8_t shift ) { return l >> shift; }
  double BinarySHR( double, uint8_t );
  float BinarySHR( float, uint8_t );
  template <typename VALUE_TYPE>
  VALUE_TYPE BSwp( VALUE_TYPE v ) { throw std::logic_error( "No ByteSwap for this type" ); }
  uint32_t BSwp( uint32_t v );
  uint16_t BSwp( uint16_t v );
  
  template <typename VALUE_TYPE>
  struct ConstNode : public ConstNodeBase
  {
    ConstNode( VALUE_TYPE _value ) : value( _value ) {} VALUE_TYPE value;
    virtual void Repr( std::ostream& sink ) const { sink << "0x" << std::hex << value << std::dec; }
    intptr_t cmp( ExprNode const& brhs ) const
    {
      ConstNode<VALUE_TYPE> const& rhs = dynamic_cast<ConstNode<VALUE_TYPE> const&>( brhs );
      return (value < rhs.value) ? -1 : (value > rhs.value) ? +1 : 0;
    }
    ConstNodeBase*
    bop( BinaryOp op, ConstNodeBase& cnb ) const
    {
      switch (op.code)
        {
        case BinaryOp::Xor: {
          ConstNode<VALUE_TYPE>& rop( dynamic_cast<ConstNode<VALUE_TYPE>&>( cnb ) );
          return new ConstNode<VALUE_TYPE>( BinaryXor( value, rop.value ) );
        }
        case BinaryOp::And: {
          ConstNode<VALUE_TYPE>& rop( dynamic_cast<ConstNode<VALUE_TYPE>&>( cnb ) );
          return new ConstNode<VALUE_TYPE>( BinaryAnd( value, rop.value ) );
        }
        case BinaryOp::Or: {
          ConstNode<VALUE_TYPE>& rop( dynamic_cast<ConstNode<VALUE_TYPE>&>( cnb ) );
          return new ConstNode<VALUE_TYPE>( BinaryOr( value, rop.value ) );
        }
        case BinaryOp::SHL: {
          ConstNodeBase& rop( dynamic_cast<ConstNodeBase&>( cnb ) );
          return new ConstNode<VALUE_TYPE>( BinarySHL( value, rop.GetU8() ) );
        }
        case BinaryOp::SHR: {
          ConstNodeBase& rop( dynamic_cast<ConstNodeBase&>( cnb ) );
          return new ConstNode<VALUE_TYPE>( BinarySHR( value, rop.GetU8() ) );
        }
        case BinaryOp::Add: {
          ConstNode<VALUE_TYPE>& rop( dynamic_cast<ConstNode<VALUE_TYPE>&>( cnb ) );
          return new ConstNode<VALUE_TYPE>( value + rop.value );
        }
        case BinaryOp::Sub: {
          ConstNode<VALUE_TYPE>& rop( dynamic_cast<ConstNode<VALUE_TYPE>&>( cnb ) );
          return new ConstNode<VALUE_TYPE>( value - rop.value );
        }
        case BinaryOp::Mul: {
          ConstNode<VALUE_TYPE>& rop( dynamic_cast<ConstNode<VALUE_TYPE>&>( cnb ) );
          return new ConstNode<VALUE_TYPE>( value * rop.value );
        }
        case BinaryOp::Div: {
          ConstNode<VALUE_TYPE>& rop( dynamic_cast<ConstNode<VALUE_TYPE>&>( cnb ) );
          return new ConstNode<VALUE_TYPE>( value / rop.value );
        }

        case BinaryOp::Teq: {
          ConstNode<VALUE_TYPE>& rop( dynamic_cast<ConstNode<VALUE_TYPE>&>( cnb ) );
          return new ConstNode<bool>( value == rop.value );
        }
        
        case BinaryOp::Tne: {
          ConstNode<VALUE_TYPE>& rop( dynamic_cast<ConstNode<VALUE_TYPE>&>( cnb ) );
          return new ConstNode<bool>( value != rop.value );
        }
        
        case BinaryOp::Tle: {
          ConstNode<VALUE_TYPE>& rop( dynamic_cast<ConstNode<VALUE_TYPE>&>( cnb ) );
          return new ConstNode<bool>( value <= rop.value );
        }
        
        case BinaryOp::Tlt: {
          ConstNode<VALUE_TYPE>& rop( dynamic_cast<ConstNode<VALUE_TYPE>&>( cnb ) );
          return new ConstNode<bool>( value < rop.value );
        }
        
        case BinaryOp::Tge: {
          ConstNode<VALUE_TYPE>& rop( dynamic_cast<ConstNode<VALUE_TYPE>&>( cnb ) );
          return new ConstNode<bool>( value >= rop.value );
        }
        
        case BinaryOp::Tgt: {
          ConstNode<VALUE_TYPE>& rop( dynamic_cast<ConstNode<VALUE_TYPE>&>( cnb ) );
          return new ConstNode<bool>( value > rop.value );
        }
        
        case BinaryOp::ROR: break;
        case BinaryOp::Mod: break;
        case BinaryOp::FCmp: break;
        case BinaryOp::NA: throw std::logic_error("???");
        }
      
      warn() << "Unhandled binary operation: " << op.ToString() << std::endl;
      return 0;
    }
    ConstNodeBase*
    uop( UnaryOp op ) const
    {
      switch (op.code)
        {
        case UnaryOp::BSwp:
          return new ConstNode<VALUE_TYPE>( BSwp( value ) );
        case UnaryOp::Not:
          return new ConstNode<bool>( not value );
        case UnaryOp::CLZ: break;
        case UnaryOp::FSQB: break;
        case UnaryOp::FFZ: break;
        case UnaryOp::FNeg: break;
        case UnaryOp::FSqrt: break;
        case UnaryOp::FAbs: break;
        case UnaryOp::FDen: break;
        case UnaryOp::NA: throw std::logic_error("???");
          
        }
      
      warn() << "Unhandled unary operation: " << op.ToString() << std::endl;
      return 0;
    }
    float GetFloat() const { return value; }
    double GetDouble() const { return value; }
    uint8_t GetU8() const { return value; }
    uint16_t GetU16() const { return value; }
    uint32_t GetU32() const { return value; }
    uint64_t GetU64() const { return value; }
    int8_t GetS8() const { return value; }
    int16_t GetS16() const { return value; }
    int32_t GetS32() const { return value; }
    int64_t GetS64() const { return value; }
  };
  
  template <typename VALUE_TYPE>
  struct TypeInfo
  {
    static void name( std::ostream& sink )
    { sink << (std::numeric_limits<VALUE_TYPE>::is_signed ? 'S' : 'U') << (8*sizeof(VALUE_TYPE)); }
  };
  
  template <> struct TypeInfo<float> { static void name( std::ostream& sink ) { sink << "F32"; } };
  template <> struct TypeInfo<double> { static void name( std::ostream& sink ) { sink << "F64"; } };
  
  struct Expr
  {
    Expr() : node() {} ExprNode* node;
    Expr( Expr const& expr ) : node( expr.node ) { if (node) node->Retain(); }
    Expr( ExprNode* const& _node ) : node( _node ) { if (node) node->Retain(); }
    Expr&  operator = ( Expr const& expr )
      { if (expr.node) expr.node->Retain();
        ExprNode* old_node = node;
        node = expr.node;
        if (old_node) old_node->Release();
        return *this;
      }
    ~Expr() { if (node) node->Release(); }
    ExprNode const* operator->() const { return node; }
    ExprNode* operator->() { return node; }
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
    bool MakeConst()
    {
      if (ExprNode* cn = node->GetConstNode()) {
        *this = Expr( cn );
        return true;
      }
      return false;
    }
    operator bool () const { return node; }
  };
  
  std::ostream& operator << (std::ostream&, Expr const&);
  
  template <typename VALUE_TYPE>
  Expr make_const( VALUE_TYPE value ) { return Expr( new ConstNode<VALUE_TYPE>( value ) ); }
  
  template <typename VALUE_TYPE>
  struct CastNode : public ExprNode
  {
    CastNode( Expr const& _src )
      : src(_src) {}
    virtual void Repr( std::ostream& sink ) const
    {
      TypeInfo<VALUE_TYPE>::name( sink );
      sink << "( "; src->Repr( sink );sink << " )";
    }
    intptr_t cmp( ExprNode const& brhs ) const
    {
      CastNode<VALUE_TYPE> const& rhs = dynamic_cast<CastNode<VALUE_TYPE> const&>( brhs );
      return src.cmp( rhs.src );
    }
    void Traverse( Visitor& visitor ) const
    { visitor.Process( this ); src->Traverse( visitor ); }
    virtual ExprNode* GetConstNode()
    {
      if (src.MakeConst()) {
        ConstNodeBase& const_src( dynamic_cast<ConstNodeBase&>( *src.node ) );
        if (CmpTypes<VALUE_TYPE,   float>::same) return new ConstNode<   float>( const_src.GetFloat() );
        if (CmpTypes<VALUE_TYPE,  double>::same) return new ConstNode<  double>( const_src.GetDouble() );
        if (CmpTypes<VALUE_TYPE, uint8_t>::same) return new ConstNode< uint8_t>( const_src.GetU8() );
        if (CmpTypes<VALUE_TYPE,uint16_t>::same) return new ConstNode<uint16_t>( const_src.GetU16() );
        if (CmpTypes<VALUE_TYPE,uint32_t>::same) return new ConstNode<uint32_t>( const_src.GetU32() );
        if (CmpTypes<VALUE_TYPE,uint64_t>::same) return new ConstNode<uint64_t>( const_src.GetU64() );
        if (CmpTypes<VALUE_TYPE,  int8_t>::same) return new ConstNode<  int8_t>( const_src.GetS8() );
        if (CmpTypes<VALUE_TYPE, int16_t>::same) return new ConstNode< int16_t>( const_src.GetS16() );
        if (CmpTypes<VALUE_TYPE, int32_t>::same) return new ConstNode< int32_t>( const_src.GetS32() );
        if (CmpTypes<VALUE_TYPE, int64_t>::same) return new ConstNode< int64_t>( const_src.GetS64() );
        throw std::logic_error("Unkown Type");
      }
      return 0;
    }
    Expr src;
  };
  
  struct UONode : public ExprNode
  {
    UONode( UnaryOp _unop, Expr const& _src )
      : unop(_unop), src( _src ) {}
    virtual void Traverse( Visitor& visitor ) const
    { visitor.Process( this ); src->Traverse( visitor ); }
    virtual void Repr( std::ostream& sink ) const
    { sink << unop.ToString() << "( " << src << " )"; }
    intptr_t cmp( ExprNode const& brhs ) const
    {
      UONode const& rhs = dynamic_cast<UONode const&>( brhs );
      if (intptr_t delta = unop.cmp( rhs.unop )) return delta;
      return src.cmp( rhs.src );
    }
    virtual ExprNode* GetConstNode()
    {
      if (src.MakeConst()) {
        return dynamic_cast<ConstNodeBase&>( *src.node ).uop( unop );
      }
      return 0;
    }
    UnaryOp unop; Expr src;
  };
  
  struct BONode : public ExprNode
  {
    BONode( BinaryOp _binop, Expr const& _left, Expr const& _right )
      : binop(_binop), left( _left ), right( _right ) {}
    virtual void Traverse( Visitor& visitor ) const
    { visitor.Process( this ); left->Traverse( visitor ); right->Traverse( visitor ); }
    virtual void Repr( std::ostream& sink ) const
    { sink << binop.ToString() << "( " << left << ", " << right << " )"; }
    intptr_t cmp( ExprNode const& brhs ) const
    {
      BONode const& rhs = dynamic_cast<BONode const&>( brhs );
      if (intptr_t delta = binop.cmp( rhs.binop)) return delta;
      if (intptr_t delta = left.cmp( rhs.left )) return delta;
      return right.cmp( rhs.right );
    }
    virtual ExprNode* GetConstNode()
    {
      bool left_const = left.MakeConst();
      bool right_const = right.MakeConst();
      if (left_const and right_const)
        return dynamic_cast<ConstNodeBase&>( *left.node ).bop( binop, dynamic_cast<ConstNodeBase&>( *right.node ) );
      return 0;
    }
    BinaryOp binop; Expr left; Expr right;
  };
  
  template <typename VALUE_TYPE>
  struct SmartValue
  {
    typedef VALUE_TYPE value_type;
    
    Expr                  expr;
    
    SmartValue() : expr( make_const( value_type() ) ) {}
    
    SmartValue( Expr const& _expr )
      : expr( _expr )
    {
      expr.MakeConst();
    }
    
    explicit SmartValue( value_type value ) : expr( make_const( value ) ) {}

    template <typename SRC_VALUE_TYPE>
    explicit SmartValue( SmartValue<SRC_VALUE_TYPE> const& other )
    {
      if (CmpTypes<SRC_VALUE_TYPE,VALUE_TYPE>::same) {
        expr = other.expr;
      } else {
        expr = new CastNode<VALUE_TYPE>( other.expr );
        expr.MakeConst();
      }
    }
    
    SmartValue<value_type>& operator = ( SmartValue<value_type> const& other ) { expr = other.expr; return *this; }
    
    template <typename SHIFT_TYPE>
    SmartValue<value_type> operator << ( SHIFT_TYPE shift ) const { return SmartValue<value_type>( Expr( new BONode( "SHL", expr, make_const( shift ) ) ) ); }
    template <typename SHIFT_TYPE>
    SmartValue<value_type> operator >> ( SHIFT_TYPE shift ) const { return SmartValue<value_type>( Expr( new BONode( "SHR", expr, make_const( shift ) ) ) );
    }
    template <typename SHIFT_TYPE>
    SmartValue<value_type>& operator <<= ( SHIFT_TYPE shift ) { expr = new BONode( "SHL", expr, make_const( shift ) ); return *this; }
    template <typename SHIFT_TYPE>
    SmartValue<value_type>& operator >>= ( SHIFT_TYPE shift ) { expr = new BONode( "SHR", expr, make_const( shift ) ); return *this; }
    
    template <typename SHIFT_TYPE>
    SmartValue<value_type> operator << ( SmartValue<SHIFT_TYPE> const& other ) const { return SmartValue<value_type>( Expr( new BONode( "SHL", expr, other.expr ) ) ); }
    template <typename SHIFT_TYPE>
    SmartValue<value_type> operator >> ( SmartValue<SHIFT_TYPE> const& other ) const { return SmartValue<value_type>( Expr( new BONode( "SHR", expr, other.expr ) ) ); }
    
    SmartValue<value_type> operator - () const { return SmartValue<value_type>( Expr( new BONode( "Sub", make_const( value_type( 0 ) ), expr ) ) ); }
    SmartValue<value_type> operator ~ () const { return SmartValue<value_type>( Expr( new BONode( "Xor", make_const( value_type(~value_type(0)) ), expr ) ) ); }
    
    SmartValue<value_type>& operator += ( SmartValue<value_type> const& other ) { expr = new BONode( "Add", expr, other.expr ); return *this; }
    SmartValue<value_type>& operator -= ( SmartValue<value_type> const& other ) { expr = new BONode( "Sub", expr, other.expr ); return *this; }
    SmartValue<value_type>& operator *= ( SmartValue<value_type> const& other ) { expr = new BONode( "Mul", expr, other.expr ); return *this; }
    SmartValue<value_type>& operator /= ( SmartValue<value_type> const& other ) { expr = new BONode( "Div", expr, other.expr ); return *this; }
    SmartValue<value_type>& operator %= ( SmartValue<value_type> const& other ) { expr = new BONode( "Mod", expr, other.expr ); return *this; }
    SmartValue<value_type>& operator ^= ( SmartValue<value_type> const& other ) { expr = new BONode( "Xor", expr, other.expr ); return *this; }
    SmartValue<value_type>& operator &= ( SmartValue<value_type> const& other ) { expr = new BONode( "And", expr, other.expr ); return *this; }
    SmartValue<value_type>& operator |= ( SmartValue<value_type> const& other ) { expr = new  BONode( "Or", expr, other.expr ); return *this; }
    
    SmartValue<value_type> operator + ( SmartValue<value_type> const& other ) const { return SmartValue<value_type>( Expr( new BONode( "Add", expr, other.expr ) ) ); }
    SmartValue<value_type> operator - ( SmartValue<value_type> const& other ) const { return SmartValue<value_type>( Expr( new BONode( "Sub", expr, other.expr ) ) ); }
    SmartValue<value_type> operator * ( SmartValue<value_type> const& other ) const { return SmartValue<value_type>( Expr( new BONode( "Mul", expr, other.expr ) ) ); }
    SmartValue<value_type> operator / ( SmartValue<value_type> const& other ) const { return SmartValue<value_type>( Expr( new BONode( "Div", expr, other.expr ) ) ); }
    SmartValue<value_type> operator % ( SmartValue<value_type> const& other ) const { return SmartValue<value_type>( Expr( new BONode( "Mod", expr, other.expr ) ) ); }
    SmartValue<value_type> operator ^ ( SmartValue<value_type> const& other ) const { return SmartValue<value_type>( Expr( new BONode( "Xor", expr, other.expr ) ) ); }
    SmartValue<value_type> operator & ( SmartValue<value_type> const& other ) const { return SmartValue<value_type>( Expr( new BONode( "And", expr, other.expr ) ) ); }
    SmartValue<value_type> operator | ( SmartValue<value_type> const& other ) const { return SmartValue<value_type>( Expr( new  BONode( "Or", expr, other.expr ) ) ); }
    
    SmartValue<bool> operator == ( SmartValue<value_type> const& other ) const { return SmartValue<bool>( Expr( new BONode( "Teq", expr, other.expr ) ) ); }
    SmartValue<bool> operator != ( SmartValue<value_type> const& other ) const { return SmartValue<bool>( Expr( new BONode( "Tne", expr, other.expr ) ) ); }
    SmartValue<bool> operator <= ( SmartValue<value_type> const& other ) const { return SmartValue<bool>( Expr( new BONode( "Tle", expr, other.expr ) ) ); }
    SmartValue<bool> operator >= ( SmartValue<value_type> const& other ) const { return SmartValue<bool>( Expr( new BONode( "Tge", expr, other.expr ) ) ); }
    SmartValue<bool> operator < ( SmartValue<value_type> const& other ) const  { return SmartValue<bool>( Expr( new BONode( "Tlt", expr, other.expr ) ) ); }
    SmartValue<bool> operator > ( SmartValue<value_type> const& other ) const  { return SmartValue<bool>( Expr( new BONode( "Tgt", expr, other.expr ) ) ); }

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
  UTP RotateRight( UTP const& value, unsigned shift ) { return UTP( new BONode( "ROR", value.expr, make_const( shift ) ) ); }
  template <typename UTP>
  UTP RotateRight( UTP const& value, UTP const& shift ) { return UTP( new BONode( "ROR", value.expr, shift.expr ) ); }
  
  template <typename UTP>
  UTP CountLeadingZeros( UTP const& value ) { return UTP( new UONode( "CLZ", value.expr ) ); }
  
  struct FP
  {
    struct DefaultNaNNode : public ExprNode
    {
      DefaultNaNNode( int _fsz ) : fsz( _fsz ) {} int fsz;
      virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); }
      virtual void Repr( std::ostream& sink ) const { sink << "F" << fsz << "DefaultNaN()"; }
      intptr_t cmp( ExprNode const& brhs ) const
      {
        DefaultNaNNode const& rhs = dynamic_cast<DefaultNaNNode const&>( brhs );
        return fsz - rhs.fsz;
      }
      ExprNode* GetConstNode() { return 0; };
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
      virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); src->Traverse( visitor ); }
      virtual void Repr( std::ostream& sink ) const { sink << "Is" << (signaling?'S':'Q') << "NaN( " << src << " )"; }
      intptr_t cmp( ExprNode const& brhs ) const
      {
        IsNaNNode const& rhs = dynamic_cast<IsNaNNode const&>( brhs );
        if (intptr_t delta = src.cmp( rhs.src )) return delta;
        return int(signaling) - int(rhs.signaling);
      }
      ExprNode* GetConstNode() { return 0; };
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
        : acc( _acc ), left( _left ), right( _right ) {}
      Expr acc; Expr left; Expr right;
      virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); acc->Traverse( visitor ); left->Traverse( visitor ); right->Traverse( visitor ); }
      virtual void Repr( std::ostream& sink ) const { sink << "FMulAdd( " << acc << ", " << left << ", " << right << " )"; }
      intptr_t cmp( ExprNode const& brhs ) const
      {
        MulAddNode const& rhs = dynamic_cast<MulAddNode const&>( brhs );
        if (intptr_t delta = left.cmp( rhs.left )) return delta;
        if (intptr_t delta = right.cmp( rhs.right )) return delta;
        return acc.cmp( rhs.acc );
      }
      ExprNode* GetConstNode() { return 0; };
    };
    
    template <typename FLOAT, class ARCH> static
    void MulAdd( FLOAT& acc, FLOAT const& op1, FLOAT const& op2, ARCH& arch, SmartValue<uint32_t> const& fpscr_val )
    {
      acc = FLOAT( Expr( new MulAddNode( acc.expr, op1.expr, op2.expr ) ) );
    }
    
    struct IsInvalidMulAddNode : public ExprNode
    {
      IsInvalidMulAddNode( Expr const& _acc, Expr const& _left, Expr const& _right )
        : acc( _acc ), left( _left ), right( _right ) {}
      Expr acc; Expr left; Expr right;
      virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); acc->Traverse( visitor ); left->Traverse( visitor ); right->Traverse( visitor ); }
      virtual void Repr( std::ostream& sink ) const { sink << "FIsInvalidMulAdd( " << acc << ", " << left << ", " << right << " )"; }
      intptr_t cmp( ExprNode const& brhs ) const
      {
        IsInvalidMulAddNode const& rhs = dynamic_cast<IsInvalidMulAddNode const&>( brhs );
        if (intptr_t delta = left.cmp( rhs.left )) return delta;
        if (intptr_t delta = right.cmp( rhs.right )) return delta;
        return acc.cmp( rhs.acc );
      }
      ExprNode* GetConstNode() { return 0; };
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
    
    template <typename FLOAT> static
    void Sqrt( FLOAT& acc, SmartValue<uint32_t> const& fpscr_val ) { acc = FLOAT( Expr( new UONode( "FSqrt", acc.expr ) ) ); }

    struct FtoFNode : public ExprNode
    {
      FtoFNode( Expr const& _src, int _ssz, int _dsz )
        : src( _src ), ssz( _ssz ), dsz( _dsz )
      {} Expr src; int ssz; int dsz;
      virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); src->Traverse( visitor ); }
      virtual void Repr( std::ostream& sink ) const { sink << "F" << ssz << "2F" << dsz << "( " << src << " )"; }
      intptr_t cmp( ExprNode const& brhs ) const
      {
        FtoFNode const& rhs = dynamic_cast<FtoFNode const&>( brhs );
        if (intptr_t delta = src.cmp( rhs.src )) return delta;
        if (intptr_t delta = ssz - rhs.ssz) return delta;
        return dsz - rhs.dsz;
      }
      ExprNode* GetConstNode() { return 0; };
    };
    
    template <typename ofpT, typename ifpT, class ARCH> static
    void FtoF( SmartValue<ofpT>& dst, SmartValue<ifpT> const& src, ARCH& arch, SmartValue<uint32_t> const& fpscr_val )
    {
      dst = SmartValue<ofpT>( Expr( new FtoFNode( src.expr, 8*sizeof (ifpT), 8*sizeof (ofpT) ) ) );
    }

    struct FtoINode : public ExprNode
    {
      FtoINode( Expr const& _src, int _fsz, int _isz, int _fb )
        : src( _src ), fsz( _fsz ), isz( _isz ), fb( _fb )
      {} Expr src; int fsz; int isz; int fb; 
      virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); src->Traverse( visitor ); }
      virtual void Repr( std::ostream& sink ) const { sink << "F" << fsz << "2I" << isz << "( " << src << ", " << fb << " )"; }
      intptr_t cmp( ExprNode const& brhs ) const
      {
        FtoINode const& rhs = dynamic_cast<FtoINode const&>( brhs );
        if (intptr_t delta = src.cmp( rhs.src )) return delta;
        if (intptr_t delta = isz - rhs.isz) return delta;
        if (intptr_t delta = fsz - rhs.fsz) return delta;
        return fb - rhs.fb;
      }
      ExprNode* GetConstNode() { return 0; };
    };

    template <typename intT, typename fpT, class ARCH> static
    void FtoI( SmartValue<intT>& dst, SmartValue<fpT> const& src, int fracbits, ARCH& arch, SmartValue<uint32_t> const& fpscr_val )
    {
      dst = SmartValue<intT>( Expr( new FtoINode( src.expr, 8*sizeof (fpT), 8*sizeof (intT), fracbits) ) );
    }

    struct ItoFNode : public ExprNode
    {
      ItoFNode( Expr const& _src, int _isz, int _fsz, int _fb )
        : src( _src ), isz( _isz ), fsz( _fsz ), fb( _fb )
      {} Expr src; int isz; int fsz; int fb;
      virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); src->Traverse( visitor ); }
      virtual void Repr( std::ostream& sink ) const { sink << "I" << isz << "2F" << fsz << "( " << src << ", " << fb << " )"; }
      intptr_t cmp( ExprNode const& brhs ) const
      {
        ItoFNode const& rhs = dynamic_cast<ItoFNode const&>( brhs );
        if (intptr_t delta = src.cmp( rhs.src )) return delta;
        if (intptr_t delta = isz - rhs.isz) return delta;
        if (intptr_t delta = fsz - rhs.fsz) return delta;
        return fb - rhs.fb;
      }
      ExprNode* GetConstNode() { return 0; };
    };
    
    template <typename fpT, typename intT, class ARCH> static
    void ItoF( SmartValue<fpT>& dst, SmartValue<intT> const& src, int fracbits, ARCH& arch, SmartValue<uint32_t> const& fpscr_val )
    {
      dst = SmartValue<fpT>( Expr( new ItoFNode( src.expr, 8*sizeof (intT), 8*sizeof (fpT), fracbits ) ) );
    }
    
  };
} // end of namespace armsec

#endif /* SMART_TYPES_HH */
