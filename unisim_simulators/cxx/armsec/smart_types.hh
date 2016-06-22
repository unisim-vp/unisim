#ifndef SMART_TYPES_HH
#define SMART_TYPES_HH

#include <limits>       // std::numeric_limits
#include <ostream>
#include <stdint.h>
// #include <cmath>

namespace armsec
{
  struct BaseNotFound {};
  struct NotAnOffset {};
  
  struct Variables { virtual ~Variables() {} };
  
  struct RelTrans
  {
    void negate() {}
    void add( uint32_t offset ) {}
    void sub( uint32_t offset ) {}
  };
  
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
  };
  
  
  struct Expr
  {
    Expr() : node() {} ExprNode* node;
    Expr( Expr const& expr ) : node( expr.node ) { node->Retain(); }
    Expr( ExprNode* const& _node ) : node( _node ) { node->Retain(); }
    Expr&  operator = ( Expr const& expr ) { expr.node->Retain(); if (node) node->Release(); node = expr.node; return *this; }
    void clear() { }
    ~Expr() { if (node) node->Release(); }
    ExprNode const* operator->() const { return node; }
    ExprNode* operator->() { return node; }
  };
  
  template <typename VALUE_TYPE>
  struct ConstNode : public ExprNode
  {
    ConstNode( VALUE_TYPE _value ) : value( _value ) {} VALUE_TYPE value;
    virtual void Repr( std::ostream& sink ) const { sink << "0x" << std::hex << value << std::dec; }
  };
  
  template <typename VALUE_TYPE>
  Expr make_const( VALUE_TYPE value ) { return Expr( new ConstNode<VALUE_TYPE>( value ) ); }
  
  struct SXTNode : public ExprNode
  {
    SXTNode( Expr const& expr, unsigned _size ) : src( expr ), size( _size ) {} Expr src; unsigned size;
    virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); src->Traverse( visitor ); }
    virtual void Repr( std::ostream& sink ) const { sink << "SXT<" << size << ">( "; src->Repr( sink ); sink << " )"; }
  };
  
  struct SHLNode : public ExprNode
  {
    SHLNode( Expr const& _left, Expr const& _right ) : left( _left ), right( _right ) {} Expr left; Expr right;
    virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); left->Traverse( visitor ); right->Traverse( visitor ); }
    virtual void Repr( std::ostream& sink ) const { sink << "SHL( "; left->Repr( sink ); sink << ", "; right->Repr( sink ); sink << " )"; }
  };
  
  struct SHRNode : public ExprNode
  {
    SHRNode( Expr const& _left, Expr const& _right ) : left( _left ), right( _right ) {} Expr left; Expr right;
    virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); left->Traverse( visitor ); right->Traverse( visitor ); }
    virtual void Repr( std::ostream& sink ) const { sink << "SHR( "; left->Repr( sink ); sink << ", "; right->Repr( sink ); sink << " )"; }
  };
  
  struct SARNode : public ExprNode
  {
    SARNode( Expr const& _left, Expr const& _right ) : left( _left ), right( _right ) {} Expr left; Expr right;
    virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); left->Traverse( visitor ); right->Traverse( visitor ); }
    virtual void Repr( std::ostream& sink ) const { sink << "SAR( "; left->Repr( sink ); sink << ", "; right->Repr( sink ); sink << " )"; }
  };
  
  struct AddNode : public ExprNode
  {
    AddNode( Expr const& _left, Expr const& _right ) : left( _left ), right( _right ) {} Expr left; Expr right;
    virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); left->Traverse( visitor ); right->Traverse( visitor ); }
    virtual void Repr( std::ostream& sink ) const { sink << "Add( "; left->Repr( sink ); sink << ", "; right->Repr( sink ); sink << " )"; }
  };
  struct SubNode : public ExprNode
  {
    SubNode( Expr const& _left, Expr const& _right ) : left( _left ), right( _right ) {} Expr left; Expr right;
    virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); left->Traverse( visitor ); right->Traverse( visitor ); }
    virtual void Repr( std::ostream& sink ) const { sink << "Sub( "; left->Repr( sink ); sink << ", "; right->Repr( sink ); sink << " )"; }
  };
  struct MulNode : public ExprNode
  {
    MulNode( Expr const& _left, Expr const& _right ) : left( _left ), right( _right ) {} Expr left; Expr right;
    virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); left->Traverse( visitor ); right->Traverse( visitor ); }
    virtual void Repr( std::ostream& sink ) const { sink << "Mul( "; left->Repr( sink ); sink << ", "; right->Repr( sink ); sink << " )"; }
  };
  struct DivNode : public ExprNode
  {
    DivNode( Expr const& _left, Expr const& _right ) : left( _left ), right( _right ) {} Expr left; Expr right;
    virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); left->Traverse( visitor ); right->Traverse( visitor ); }
    virtual void Repr( std::ostream& sink ) const { sink << "Div( "; left->Repr( sink ); sink << ", "; right->Repr( sink ); sink << " )"; }
  };
  struct ModNode : public ExprNode
  {
    ModNode( Expr const& _left, Expr const& _right ) : left( _left ), right( _right ) {} Expr left; Expr right;
    virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); left->Traverse( visitor ); right->Traverse( visitor ); }
    virtual void Repr( std::ostream& sink ) const { sink << "Mod( "; left->Repr( sink ); sink << ", "; right->Repr( sink ); sink << " )"; }
  };
  struct XorNode : public ExprNode
  {
    XorNode( Expr const& _left, Expr const& _right ) : left( _left ), right( _right ) {} Expr left; Expr right;
    virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); left->Traverse( visitor ); right->Traverse( visitor ); }
    virtual void Repr( std::ostream& sink ) const { sink << "Xor( "; left->Repr( sink ); sink << ", "; right->Repr( sink ); sink << " )"; }
  };
  struct AndNode : public ExprNode
  {
    AndNode( Expr const& _left, Expr const& _right ) : left( _left ), right( _right ) {} Expr left; Expr right;
    virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); left->Traverse( visitor ); right->Traverse( visitor ); }
    virtual void Repr( std::ostream& sink ) const { sink << "And( "; left->Repr( sink ); sink << ", "; right->Repr( sink ); sink << " )"; }
  };
  struct OrNode : public ExprNode
  {
    OrNode( Expr const& _left, Expr const& _right ) : left( _left ), right( _right ) {} Expr left; Expr right;
    virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); left->Traverse( visitor ); right->Traverse( visitor ); }
    virtual void Repr( std::ostream& sink ) const { sink << "Or( "; left->Repr( sink ); sink << ", "; right->Repr( sink ); sink << " )"; }
  };
  
  struct TeqNode : public ExprNode
  {
    TeqNode( Expr const& _left, Expr const& _right ) : left( _left ), right( _right ) {} Expr left; Expr right;
    virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); left->Traverse( visitor ); right->Traverse( visitor ); }
    virtual void Repr( std::ostream& sink ) const { sink << "Teq( "; left->Repr( sink ); sink << ", "; right->Repr( sink ); sink << " )"; }
  };
  
  struct TneNode : public ExprNode
  {
    TneNode( Expr const& _left, Expr const& _right ) : left( _left ), right( _right ) {} Expr left; Expr right;
    virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); left->Traverse( visitor ); right->Traverse( visitor ); }
    virtual void Repr( std::ostream& sink ) const { sink << "Tne( "; left->Repr( sink ); sink << ", "; right->Repr( sink ); sink << " )"; }
  };
  
  struct TgeNode : public ExprNode
  {
    TgeNode( Expr const& _left, Expr const& _right ) : left( _left ), right( _right ) {} Expr left; Expr right;
    virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); left->Traverse( visitor ); right->Traverse( visitor ); }
    virtual void Repr( std::ostream& sink ) const { sink << "Tge( "; left->Repr( sink ); sink << ", "; right->Repr( sink ); sink << " )"; }
  };
  
  struct TleNode : public ExprNode
  {
    TleNode( Expr const& _left, Expr const& _right ) : left( _left ), right( _right ) {} Expr left; Expr right;
    virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); left->Traverse( visitor ); right->Traverse( visitor ); }
    virtual void Repr( std::ostream& sink ) const { sink << "Tle( "; left->Repr( sink ); sink << ", "; right->Repr( sink ); sink << " )"; }
  };
  
  struct TgtNode : public ExprNode
  {
    TgtNode( Expr const& _left, Expr const& _right ) : left( _left ), right( _right ) {} Expr left; Expr right;
    virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); left->Traverse( visitor ); right->Traverse( visitor ); }
    virtual void Repr( std::ostream& sink ) const { sink << "Tgt( "; left->Repr( sink ); sink << ", "; right->Repr( sink ); sink << " )"; }
  };
  
  struct TltNode : public ExprNode
  {
    TltNode( Expr const& _left, Expr const& _right ) : left( _left ), right( _right ) {} Expr left; Expr right;
    virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); left->Traverse( visitor ); right->Traverse( visitor ); }
    virtual void Repr( std::ostream& sink ) const { sink << "Tlt( "; left->Repr( sink ); sink << ", "; right->Repr( sink ); sink << " )"; }
  };
  
  
  template <typename Bool> struct AssertBool {};
  template <>              struct AssertBool<bool> { static void check() {} };

  template <typename VALUE_TYPE>
  struct SmartValue
  {
    typedef VALUE_TYPE value_type;
    
    Expr                  m_expr;
    
    SmartValue() : m_expr() {}
    
    SmartValue( Expr const& expr ) : m_expr( expr ) {}
    
    explicit SmartValue( value_type value ) : m_expr( make_const( value ) ) {}

    template <typename SRC_VALUE_TYPE>
    explicit SmartValue( SmartValue<SRC_VALUE_TYPE> const& other )
    {
      unsigned src_size = 8*sizeof(SRC_VALUE_TYPE);
      unsigned dst_size = 8*sizeof(value_type);
      bool src_signed = std::numeric_limits<SRC_VALUE_TYPE>::is_signed;
      bool dst_signed = std::numeric_limits<value_type>::is_signed;
      
      if ((src_size == dst_size) and (src_signed == dst_signed))
        m_expr = other.m_expr;
      if (dst_signed and ((dst_size <= src_size) or src_signed))
        m_expr = new SXTNode( other.m_expr, std::min(dst_size,src_size) );
      else {
        uint32_t mask = uint32_t((uint64_t(1) << dst_size) - 1);
        m_expr = new AndNode( other.m_expr, make_const( mask ) );
      }
    }
    
    SmartValue<value_type>& operator = ( SmartValue<value_type> const& other ) { m_expr = other.m_expr; return *this; }
    
    template <typename SHIFT_TYPE>
    SmartValue<value_type> operator << ( SHIFT_TYPE shift ) const { return SmartValue<value_type>( new SHLNode( m_expr, make_const( shift ) ) ); }
    template <typename SHIFT_TYPE>
    SmartValue<value_type> operator >> ( SHIFT_TYPE shift ) const {
      if (std::numeric_limits<value_type>::is_signed)
        return SmartValue<value_type>( new SARNode( m_expr, make_const( shift ) ) );
      return SmartValue<value_type>( new SHRNode( m_expr, make_const( shift ) ) );
    }
    template <typename SHIFT_TYPE>
    SmartValue<value_type>& operator <<= ( SHIFT_TYPE shift ) { m_expr = new SHLNode( m_expr, make_const( shift ) ); return *this; }
    template <typename SHIFT_TYPE>
    SmartValue<value_type>& operator >>= ( SHIFT_TYPE shift ) {
      if (std::numeric_limits<value_type>::is_signed)
        m_expr = new SARNode( m_expr, make_const( shift ) );
      else
        m_expr = new SHRNode( m_expr, make_const( shift ) );
      return *this;
    }
    
    template <typename SHIFT_TYPE>
    SmartValue<value_type> operator << ( SmartValue<SHIFT_TYPE> const& other ) const { return SmartValue<value_type>( new SHLNode( m_expr, other.m_expr ) ); }
    template <typename SHIFT_TYPE>
    SmartValue<value_type> operator >> ( SmartValue<SHIFT_TYPE> const& other ) const {
      if (std::numeric_limits<value_type>::is_signed)
        return SmartValue<value_type>( new SARNode( m_expr, other.m_expr ) );
      return SmartValue<value_type>( new SHRNode( m_expr, other.m_expr ) );
    }
    
    SmartValue<value_type> operator - () const { return SmartValue<value_type>( new SubNode( make_const( value_type( 0 ) ), m_expr ) ); }
    SmartValue<value_type> operator ~ () const { return SmartValue<value_type>( new XorNode( make_const( ~value_type( 0 ) ), m_expr ) ); }
    
    SmartValue<value_type>& operator += ( SmartValue<value_type> const& other ) { m_expr = new AddNode( m_expr, other.m_expr ); return *this; }
    SmartValue<value_type>& operator -= ( SmartValue<value_type> const& other ) { m_expr = new SubNode( m_expr, other.m_expr ); return *this; }
    SmartValue<value_type>& operator *= ( SmartValue<value_type> const& other ) { m_expr = new MulNode( m_expr, other.m_expr ); return *this; }
    SmartValue<value_type>& operator /= ( SmartValue<value_type> const& other ) { m_expr = new DivNode( m_expr, other.m_expr ); return *this; }
    SmartValue<value_type>& operator %= ( SmartValue<value_type> const& other ) { m_expr = new ModNode( m_expr, other.m_expr ); return *this; }
    SmartValue<value_type>& operator ^= ( SmartValue<value_type> const& other ) { m_expr = new XorNode( m_expr, other.m_expr ); return *this; }
    SmartValue<value_type>& operator &= ( SmartValue<value_type> const& other ) { m_expr = new AndNode( m_expr, other.m_expr ); return *this; }
    SmartValue<value_type>& operator |= ( SmartValue<value_type> const& other ) { m_expr = new  OrNode( m_expr, other.m_expr ); return *this; }
    
    SmartValue<value_type> operator + ( SmartValue<value_type> const& other ) const { return SmartValue<value_type>( new AddNode( m_expr, other.m_expr ) ); }
    SmartValue<value_type> operator - ( SmartValue<value_type> const& other ) const { return SmartValue<value_type>( new SubNode( m_expr, other.m_expr ) ); }
    SmartValue<value_type> operator * ( SmartValue<value_type> const& other ) const { return SmartValue<value_type>( new MulNode( m_expr, other.m_expr ) ); }
    SmartValue<value_type> operator / ( SmartValue<value_type> const& other ) const { return SmartValue<value_type>( new DivNode( m_expr, other.m_expr ) ); }
    SmartValue<value_type> operator % ( SmartValue<value_type> const& other ) const { return SmartValue<value_type>( new ModNode( m_expr, other.m_expr ) ); }
    SmartValue<value_type> operator ^ ( SmartValue<value_type> const& other ) const { return SmartValue<value_type>( new XorNode( m_expr, other.m_expr ) ); }
    SmartValue<value_type> operator & ( SmartValue<value_type> const& other ) const { return SmartValue<value_type>( new AndNode( m_expr, other.m_expr ) ); }
    SmartValue<value_type> operator | ( SmartValue<value_type> const& other ) const { return SmartValue<value_type>( new  OrNode( m_expr, other.m_expr ) ); }
    
    SmartValue<bool> operator == ( SmartValue<value_type> const& other ) const { return SmartValue<bool>( new TeqNode( m_expr, other.m_expr ) ); }
    SmartValue<bool> operator != ( SmartValue<value_type> const& other ) const { return SmartValue<bool>( new TneNode( m_expr, other.m_expr ) ); }
    SmartValue<bool> operator <= ( SmartValue<value_type> const& other ) const { return SmartValue<bool>( new TleNode( m_expr, other.m_expr ) ); }
    SmartValue<bool> operator >= ( SmartValue<value_type> const& other ) const { return SmartValue<bool>( new TgeNode( m_expr, other.m_expr ) ); }
    SmartValue<bool> operator < ( SmartValue<value_type> const& other ) const  { return SmartValue<bool>( new TltNode( m_expr, other.m_expr ) ); }
    SmartValue<bool> operator > ( SmartValue<value_type> const& other ) const  { return SmartValue<bool>( new TgtNode( m_expr, other.m_expr ) ); }

    SmartValue<bool> operator ! () const
    { AssertBool<value_type>::check(); return SmartValue<bool>( new XorNode( make_const( true ), m_expr ) ); }

    SmartValue<bool> operator && ( SmartValue<bool> const& other ) const
    { AssertBool<value_type>::check(); return SmartValue<bool>( new AndNode( m_expr, other.m_expr ) ); }
    
    SmartValue<bool> operator || ( SmartValue<bool> const& other ) const
    { AssertBool<value_type>::check(); return SmartValue<bool>( new  OrNode( m_expr, other.m_expr ) ); }
  };
  
  struct ByteSwapNode : public ExprNode
  {
    ByteSwapNode( Expr const& _src ) : src( _src ) {} Expr src;
    virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); src->Traverse( visitor ); }
    virtual void Repr( std::ostream& sink ) const { sink << "ByteSwap( "; src->Repr( sink ); sink << " )"; }
  };
  template <typename UTP>
  UTP ByteSwap( UTP const& value ) { return UTP( new ByteSwapNode( value.m_expr ) ); }
  
  struct RORNode : public ExprNode
  {
    RORNode( Expr const& _src, Expr const& _shift ) : src( _src ), shift( _shift ) {} Expr src; Expr shift;
    virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); src->Traverse( visitor ); shift->Traverse( visitor ); }
    virtual void Repr( std::ostream& sink ) const { sink << "ROR( "; src->Repr( sink ); sink << ", "; shift->Repr( sink ); sink << " )"; }
  };
  template <typename UTP>
  UTP RotateRight( UTP const& value, unsigned shift ) { return UTP( new RORNode( value.m_expr, make_const( shift ) ) ); }
  template <typename UTP>
  UTP RotateRight( UTP const& value, UTP const& shift ) { return UTP( new RORNode( value.m_expr, shift.m_expr ) ); }
  
  struct CLZNode : public ExprNode
  {
    CLZNode( Expr const& _src ) : src( _src ) {} Expr src;
    virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); src->Traverse( visitor ); }
    virtual void Repr( std::ostream& sink ) const { sink << "CLZ( "; src->Repr( sink ); sink << " )"; }
  };
  template <typename UTP>
  UTP CountLeadingZeros( UTP const& value ) { return UTP( new CLZNode( value.m_expr ) ); }
  
  struct FPU
  {
    struct DefaultNaNNode : public ExprNode
    {
      DefaultNaNNode( int _fsz ) : fsz( _fsz ) {} int fsz;
      virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); }
      virtual void Repr( std::ostream& sink ) const { sink << "F" << fsz << "DefaultNaN()"; }
    };
    
    template <typename fpscrT> static
    void FloatSetDefaultNan( SmartValue<float>& result, fpscrT const& fpscr )
    { result = SmartValue<float>( new DefaultNaNNode( 32 ) ); }
    
    template <typename fpscrT> static
    void FloatSetDefaultNan( SmartValue<double>& result, fpscrT const& fpscr )
    { result = SmartValue<double>( new DefaultNaNNode( 64 ) ); }
    
    struct SetQuietBitNode : public ExprNode
    {
      SetQuietBitNode( Expr const& _src ) : src( _src ) {} Expr src;
      virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); src->Traverse( visitor ); }
      virtual void Repr( std::ostream& sink ) const { sink << "FSetQuietBit( "; src->Repr( sink ); sink << " )"; }
    };
    
    template <typename FLOAT, typename fpscrT> static
    void FloatSetQuietBit( FLOAT& op, fpscrT const& fpscr )
    {
      op = FLOAT( new SetQuietBitNode( op.m_expr ) );
    }

    struct FlushToZeroNode : public ExprNode
    {
      FlushToZeroNode( Expr const& _src ) : src( _src ) {} Expr src;
      virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); src->Traverse( visitor ); }
      virtual void Repr( std::ostream& sink ) const { sink << "FFlushToZero( "; src->Repr( sink ); sink << " )"; }
    };
    
    template <typename FLOAT, typename fpscrT> static
    bool
    FloatFlushToZero( FLOAT& op, fpscrT& fpscr )
    {
      op = FLOAT( new FlushToZeroNode( op.m_expr ) );
      return false;
    }

    struct CmpNode : public ExprNode
    {
      CmpNode( Expr const& _left, Expr const& _right ) : left( _left ), right( _right ) {} Expr left; Expr right;
      virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); left->Traverse( visitor ); right->Traverse( visitor ); }
      virtual void Repr( std::ostream& sink ) const { sink << "FCmp( "; left->Repr( sink ); sink << ", "; right->Repr( sink ); sink << " )"; }
    };

    template <typename FLOAT, typename fpscrT> static
    SmartValue<int32_t> FloatCompare( FLOAT op1, FLOAT op2, fpscrT& fpscr )
    {
      return SmartValue<int32_t>( new CmpNode( op1.m_expr, op2.m_expr ) );
    }

    struct IsNaNNode : public ExprNode
    {
      IsNaNNode( Expr const& _src, bool _signaling ) : src(_src), signaling(_signaling) {} Expr src; bool signaling;
      virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); src->Traverse( visitor ); }
      virtual void Repr( std::ostream& sink ) const { sink << "Is" << (signaling?'S':'Q') << "NaN( "; src->Repr( sink ); sink << " )"; }
    };
    
    template <typename FLOAT, typename fpscrT> static
    SmartValue<bool>
    FloatIsSNaN( FLOAT const& op, fpscrT const& fpscr )
    {
      return SmartValue<bool>( new IsNaNNode( op.m_expr, false ) );
    }
    
    template <typename FLOAT, typename fpscrT> static
    SmartValue<bool>
    FloatIsQNaN( FLOAT const& op, fpscrT const& fpscr )
    {
      return SmartValue<bool>( new IsNaNNode( op.m_expr, true ) );
    }
    
    struct AddNode : public ExprNode
    {
      AddNode( Expr const& _left, Expr const& _right ) : left( _left ), right( _right ) {} Expr left; Expr right;
      virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); left->Traverse( visitor ); right->Traverse( visitor ); }
      virtual void Repr( std::ostream& sink ) const { sink << "FAdd( "; left->Repr( sink ); sink << ", "; right->Repr( sink ); sink << " )"; }
    };
    
    template <typename FLOAT, typename fpscrT> static
    void FloatAdd( FLOAT& res, FLOAT const& op1, FLOAT const& op2, fpscrT& fpscr )
    {
      res = FLOAT( new AddNode( op1.m_expr, op2.m_expr ) );
    }

    struct SubNode : public ExprNode
    {
      SubNode( Expr const& _left, Expr const& _right ) : left( _left ), right( _right ) {} Expr left; Expr right;
      virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); left->Traverse( visitor ); right->Traverse( visitor ); }
      virtual void Repr( std::ostream& sink ) const { sink << "FSub( "; left->Repr( sink ); sink << ", "; right->Repr( sink ); sink << " )"; }
    };
    
    template <typename FLOAT, typename fpscrT> static
    void FloatSub( FLOAT& res, FLOAT const& op1, FLOAT const& op2, fpscrT& fpscr )
    {
      res = FLOAT( new SubNode( op1.m_expr, op2.m_expr ) );
    }

    struct DivNode : public ExprNode
    {
      DivNode( Expr const& _left, Expr const& _right ) : left( _left ), right( _right ) {} Expr left; Expr right;
      virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); left->Traverse( visitor ); right->Traverse( visitor ); }
      virtual void Repr( std::ostream& sink ) const { sink << "FDiv( "; left->Repr( sink ); sink << ", "; right->Repr( sink ); sink << " )"; }
    };
    
    template <typename FLOAT, typename fpscrT> static
    void FloatDiv( FLOAT& res, FLOAT const& op1, FLOAT const& op2, fpscrT& fpscr )
    {
      res = FLOAT( new DivNode( op1.m_expr, op2.m_expr ) );
    }

    struct MulNode : public ExprNode
    {
      MulNode( Expr const& _left, Expr const& _right ) : left( _left ), right( _right ) {} Expr left; Expr right;
      virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); left->Traverse( visitor ); right->Traverse( visitor ); }
      virtual void Repr( std::ostream& sink ) const { sink << "FMul( "; left->Repr( sink ); sink << ", "; right->Repr( sink ); sink << " )"; }
    };
    
    template <typename FLOAT, typename fpscrT> static
    void FloatMul( FLOAT& res, FLOAT const& op1, FLOAT const& op2, fpscrT& fpscr )
    {
      res = FLOAT( new MulNode( op1.m_expr, op2.m_expr ) );
    }

    struct MulAddNode : public ExprNode
    {
      MulAddNode( Expr const& _acc, Expr const& _left, Expr const& _right )
        : acc( _acc ), left( _left ), right( _right ) {}
      Expr acc; Expr left; Expr right;
      virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); acc->Traverse( visitor ); left->Traverse( visitor ); right->Traverse( visitor ); }
      virtual void Repr( std::ostream& sink ) const { sink << "FMulAdd( "; acc->Repr( sink ); sink << ", "; left->Repr( sink ); sink << ", "; right->Repr( sink ); sink << " )"; }
    };
    
    template <typename FLOAT, typename fpscrT> static
    void FloatMulAdd( FLOAT& acc, FLOAT const& op1, FLOAT const& op2, fpscrT& fpscr )
    {
      acc = FLOAT( new MulAddNode( acc.m_expr, op1.m_expr, op2.m_expr ) );
    }

    struct NegNode : public ExprNode
    {
      NegNode( Expr const& _src ) : src( _src ) {} Expr src;
      virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); src->Traverse( visitor ); }
      virtual void Repr( std::ostream& sink ) const { sink << "FNeg( "; src->Repr( sink ); sink << " )"; }
    };
  
    template <typename FLOAT, typename fpscrT> static
    void FloatNeg( FLOAT& res, FLOAT const& op, fpscrT& fpscr ) { res = FLOAT( new NegNode( op.m_expr ) ); }

    struct SqrtNode : public ExprNode
    {
      SqrtNode( Expr const& _src ) : src( _src ) {} Expr src;
      virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); src->Traverse( visitor ); }
      virtual void Repr( std::ostream& sink ) const { sink << "FSqrt( "; src->Repr( sink ); sink << " )"; }
    };
  
    template <typename FLOAT, typename fpscrT> static
    void FloatSqrt( FLOAT& res, FLOAT const& op, fpscrT& fpscr ) { res = FLOAT( new SqrtNode( op.m_expr ) ); }

    struct FtoFNode : public ExprNode
    {
      FtoFNode( Expr const& _src, int _ssz, int _dsz )
        : src( _src ), ssz( _ssz ), dsz( _dsz )
      {} Expr src; int ssz; int dsz;
      virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); src->Traverse( visitor ); }
      virtual void Repr( std::ostream& sink ) const { sink << "F" << ssz << "2F" << dsz << "( "; src->Repr( sink ); sink << " )"; }
    };
    
    template <typename ofpT, typename ifpT, typename fpscrT> static
    void FloatFtoF( SmartValue<ofpT>& res, SmartValue<ifpT> const& op, fpscrT& fpscr )
    {
      res = SmartValue<ofpT>( new FtoFNode( op.m_expr, 8*sizeof (ifpT), 8*sizeof (ofpT) ) );
    }

    struct FtoINode : public ExprNode
    {
      FtoINode( Expr const& _src, int _fsz, int _isz, int _fb )
        : src( _src ), fsz( _fsz ), isz( _isz ), fb( _fb )
      {} Expr src; int fsz; int isz; int fb; 
      virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); src->Traverse( visitor ); }
      virtual void Repr( std::ostream& sink ) const { sink << "F" << fsz << "2I" << isz << "( "; src->Repr( sink ); sink << ", " << fb << " )"; }
    };

    template <typename intT, typename fpT, typename fpscrT> static
    void FloatFtoI( SmartValue<intT>& res, SmartValue<fpT> const& op, int fracbits, fpscrT& fpscr )
    {
      res = SmartValue<intT>( new FtoINode( op.m_expr, 8*sizeof (fpT), 8*sizeof (intT), fracbits) );
    }

    struct ItoFNode : public ExprNode
    {
      ItoFNode( Expr const& _src, int _isz, int _fsz, int _fb )
        : src( _src ), isz( _isz ), fsz( _fsz ), fb( _fb )
      {} Expr src; int isz; int fsz; int fb;
      virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); src->Traverse( visitor ); }
      virtual void Repr( std::ostream& sink ) const { sink << "I" << isz << "2F" << fsz << "( "; src->Repr( sink ); sink << ", " << fb << " )"; }
    };
    
    template <typename fpT, typename intT, typename fpscrT> static
    void FloatItoF( SmartValue<fpT>& res, SmartValue<intT> const& op, int fracbits, fpscrT& fpscr )
    {
      res = SmartValue<fpT>( new ItoFNode( op.m_expr, 8*sizeof (intT), 8*sizeof (fpT), fracbits ) );
    }
    
    struct AbsNode : public ExprNode
    {
      AbsNode( Expr const& _src ) : src( _src ) {} Expr src;
      virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); src->Traverse( visitor ); }
      virtual void Repr( std::ostream& sink ) const { sink << "FAbs( "; src->Repr( sink ); sink << " )"; }
    };
  
    template <typename FLOAT, typename fpscrT> static
    void FloatAbs( FLOAT& res, FLOAT const& op, fpscrT& fpscr )
    {
      res = FLOAT( new AbsNode( op.m_expr ) );
    }
    
  };
} // end of namespace armsec

#endif /* SMART_TYPES_HH */
