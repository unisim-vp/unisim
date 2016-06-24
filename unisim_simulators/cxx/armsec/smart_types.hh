#ifndef SMART_TYPES_HH
#define SMART_TYPES_HH

#include <limits>       // std::numeric_limits
#include <ostream>
#include <stdint.h>
#include <typeinfo>
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
    virtual intptr_t cmp( ExprNode const& ) const = 0;
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
  };
  
  template <typename VALUE_TYPE>
  struct ConstNode : public ExprNode
  {
    ConstNode( VALUE_TYPE _value ) : value( _value ) {} VALUE_TYPE value;
    virtual void Repr( std::ostream& sink ) const { sink << "0x" << std::hex << value << std::dec; }
    intptr_t cmp( ExprNode const& brhs ) const
    {
      ConstNode<VALUE_TYPE> const& rhs = dynamic_cast<ConstNode<VALUE_TYPE> const&>( brhs );
      return (value < rhs.value) ? -1 : (value > rhs.value) ? +1 : 0;
    }
  };
  
  template <typename VALUE_TYPE>
  Expr make_const( VALUE_TYPE value ) { return Expr( new ConstNode<VALUE_TYPE>( value ) ); }
  
  struct UONode : public ExprNode
  {
    UONode( std::string _name, Expr const& _src )
      : name(_name), src( _src ) {}
    virtual void Traverse( Visitor& visitor ) const
    { visitor.Process( this ); src->Traverse( visitor ); }
    virtual void Repr( std::ostream& sink ) const
    { sink << name << "( "; src->Repr( sink ); sink << " )"; }
    std::string name; Expr src;
    intptr_t cmp( ExprNode const& brhs ) const
    {
      UONode const& rhs = dynamic_cast<UONode const&>( brhs );
      if (intptr_t delta = name.compare( rhs.name )) return delta;
      return src.cmp( rhs.src );
    }
  };
  
  struct SXTNode : public ExprNode
  {
    SXTNode( Expr const& _src, unsigned _size ) : src( _src ), size( _size ) {} Expr src; unsigned size;
    virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); src->Traverse( visitor ); }
    virtual void Repr( std::ostream& sink ) const { sink << "SXT<" << size << ">( "; src->Repr( sink ); sink << " )"; }
    intptr_t cmp( ExprNode const& brhs ) const
    {
      SXTNode const& rhs = dynamic_cast<SXTNode const&>( brhs );
      if (intptr_t delta = src.cmp( rhs.src )) return delta;
      return int(size - rhs.size);
    }
  };
  
  struct BONode : public ExprNode
  {
    BONode( std::string _name, Expr const& _left, Expr const& _right )
      : name(_name), left( _left ), right( _right ) {}
    virtual void Traverse( Visitor& visitor ) const
    { visitor.Process( this ); left->Traverse( visitor ); right->Traverse( visitor ); }
    virtual void Repr( std::ostream& sink ) const
    { sink << name << "( "; left->Repr( sink ); sink << ", "; right->Repr( sink ); sink << " )"; }
    std::string name; Expr left; Expr right;
    intptr_t cmp( ExprNode const& brhs ) const
    {
      BONode const& rhs = dynamic_cast<BONode const&>( brhs );
      if (intptr_t delta = name.compare( rhs.name )) return delta;
      if (intptr_t delta = left.cmp( rhs.left )) return delta;
      return right.cmp( rhs.right );
    }
  };
  
  template <typename Bool> struct AssertBool {};
  template <>              struct AssertBool<bool> { static void check() {} };

  template <typename VALUE_TYPE>
  struct SmartValue
  {
    typedef VALUE_TYPE value_type;
    
    Expr                  expr;
    
    SmartValue() : expr() {}
    
    SmartValue( Expr const& _expr ) : expr( _expr ) {}
    
    explicit SmartValue( value_type value ) : expr( make_const( value ) ) {}

    template <typename SRC_VALUE_TYPE>
    explicit SmartValue( SmartValue<SRC_VALUE_TYPE> const& other )
    {
      unsigned src_size = 8*sizeof(SRC_VALUE_TYPE);
      unsigned dst_size = 8*sizeof(value_type);
      bool src_signed = std::numeric_limits<SRC_VALUE_TYPE>::is_signed;
      bool dst_signed = std::numeric_limits<value_type>::is_signed;
      
      if ((src_size == dst_size) and (src_signed == dst_signed))
        expr = other.expr;
      if (dst_signed and ((dst_size <= src_size) or src_signed))
        expr = new SXTNode( other.expr, std::min(dst_size,src_size) );
      else {
        uint32_t mask = uint32_t((uint64_t(1) << dst_size) - 1);
        expr = new BONode( "And", other.expr, make_const( mask ) );
      }
    }
    
    SmartValue<value_type>& operator = ( SmartValue<value_type> const& other ) { expr = other.expr; return *this; }
    
    template <typename SHIFT_TYPE>
    SmartValue<value_type> operator << ( SHIFT_TYPE shift ) const { return SmartValue<value_type>( new BONode( "SHL", expr, make_const( shift ) ) ); }
    template <typename SHIFT_TYPE>
    SmartValue<value_type> operator >> ( SHIFT_TYPE shift ) const {
      if (std::numeric_limits<value_type>::is_signed)
        return SmartValue<value_type>( new BONode( "SAR", expr, make_const( shift ) ) );
      return SmartValue<value_type>( new BONode( "SHR", expr, make_const( shift ) ) );
    }
    template <typename SHIFT_TYPE>
    SmartValue<value_type>& operator <<= ( SHIFT_TYPE shift ) { expr = new BONode( "SHL", expr, make_const( shift ) ); return *this; }
    template <typename SHIFT_TYPE>
    SmartValue<value_type>& operator >>= ( SHIFT_TYPE shift ) {
      if (std::numeric_limits<value_type>::is_signed)
        expr = new BONode( "SAR", expr, make_const( shift ) );
      else
        expr = new BONode( "SHR", expr, make_const( shift ) );
      return *this;
    }
    
    template <typename SHIFT_TYPE>
    SmartValue<value_type> operator << ( SmartValue<SHIFT_TYPE> const& other ) const { return SmartValue<value_type>( new BONode( "SHL", expr, other.expr ) ); }
    template <typename SHIFT_TYPE>
    SmartValue<value_type> operator >> ( SmartValue<SHIFT_TYPE> const& other ) const {
      if (std::numeric_limits<value_type>::is_signed)
        return SmartValue<value_type>( new BONode( "SAR", expr, other.expr ) );
      return SmartValue<value_type>( new BONode( "SHR", expr, other.expr ) );
    }
    
    SmartValue<value_type> operator - () const { return SmartValue<value_type>( new BONode( "Sub", make_const( value_type( 0 ) ), expr ) ); }
    SmartValue<value_type> operator ~ () const { return SmartValue<value_type>( new BONode( "Xor", make_const( ~value_type( 0 ) ), expr ) ); }
    
    SmartValue<value_type>& operator += ( SmartValue<value_type> const& other ) { expr = new BONode( "Add", expr, other.expr ); return *this; }
    SmartValue<value_type>& operator -= ( SmartValue<value_type> const& other ) { expr = new BONode( "Sub", expr, other.expr ); return *this; }
    SmartValue<value_type>& operator *= ( SmartValue<value_type> const& other ) { expr = new BONode( "Mul", expr, other.expr ); return *this; }
    SmartValue<value_type>& operator /= ( SmartValue<value_type> const& other ) { expr = new BONode( "Div", expr, other.expr ); return *this; }
    SmartValue<value_type>& operator %= ( SmartValue<value_type> const& other ) { expr = new BONode( "Mod", expr, other.expr ); return *this; }
    SmartValue<value_type>& operator ^= ( SmartValue<value_type> const& other ) { expr = new BONode( "Xor", expr, other.expr ); return *this; }
    SmartValue<value_type>& operator &= ( SmartValue<value_type> const& other ) { expr = new BONode( "And", expr, other.expr ); return *this; }
    SmartValue<value_type>& operator |= ( SmartValue<value_type> const& other ) { expr = new  BONode( "Or", expr, other.expr ); return *this; }
    
    SmartValue<value_type> operator + ( SmartValue<value_type> const& other ) const { return SmartValue<value_type>( new BONode( "Add", expr, other.expr ) ); }
    SmartValue<value_type> operator - ( SmartValue<value_type> const& other ) const { return SmartValue<value_type>( new BONode( "Sub", expr, other.expr ) ); }
    SmartValue<value_type> operator * ( SmartValue<value_type> const& other ) const { return SmartValue<value_type>( new BONode( "Mul", expr, other.expr ) ); }
    SmartValue<value_type> operator / ( SmartValue<value_type> const& other ) const { return SmartValue<value_type>( new BONode( "Div", expr, other.expr ) ); }
    SmartValue<value_type> operator % ( SmartValue<value_type> const& other ) const { return SmartValue<value_type>( new BONode( "Mod", expr, other.expr ) ); }
    SmartValue<value_type> operator ^ ( SmartValue<value_type> const& other ) const { return SmartValue<value_type>( new BONode( "Xor", expr, other.expr ) ); }
    SmartValue<value_type> operator & ( SmartValue<value_type> const& other ) const { return SmartValue<value_type>( new BONode( "And", expr, other.expr ) ); }
    SmartValue<value_type> operator | ( SmartValue<value_type> const& other ) const { return SmartValue<value_type>( new  BONode( "Or", expr, other.expr ) ); }
    
    SmartValue<bool> operator == ( SmartValue<value_type> const& other ) const { return SmartValue<bool>( new BONode( "Teq", expr, other.expr ) ); }
    SmartValue<bool> operator != ( SmartValue<value_type> const& other ) const { return SmartValue<bool>( new BONode( "Tne", expr, other.expr ) ); }
    SmartValue<bool> operator <= ( SmartValue<value_type> const& other ) const { return SmartValue<bool>( new BONode( "Tle", expr, other.expr ) ); }
    SmartValue<bool> operator >= ( SmartValue<value_type> const& other ) const { return SmartValue<bool>( new BONode( "Tge", expr, other.expr ) ); }
    SmartValue<bool> operator < ( SmartValue<value_type> const& other ) const  { return SmartValue<bool>( new BONode( "Tlt", expr, other.expr ) ); }
    SmartValue<bool> operator > ( SmartValue<value_type> const& other ) const  { return SmartValue<bool>( new BONode( "Tgt", expr, other.expr ) ); }

    SmartValue<bool> operator ! () const
    { AssertBool<value_type>::check(); return SmartValue<bool>( new BONode( "Xor", make_const( true ), expr ) ); }

    SmartValue<bool> operator && ( SmartValue<bool> const& other ) const
    { AssertBool<value_type>::check(); return SmartValue<bool>( new BONode( "And", expr, other.expr ) ); }
    
    SmartValue<bool> operator || ( SmartValue<bool> const& other ) const
    { AssertBool<value_type>::check(); return SmartValue<bool>( new  BONode( "Or", expr, other.expr ) ); }
  };
  
  template <typename UTP>
  UTP ByteSwap( UTP const& value ) { return UTP( new UONode( "ByteSwap", value.expr ) ); }
  
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
    };
    
    template <typename fpscrT> static
    void SetDefaultNan( SmartValue<float>& result, fpscrT const& fpscr )
    { result = SmartValue<float>( new DefaultNaNNode( 32 ) ); }
    
    template <typename fpscrT> static
    void SetDefaultNan( SmartValue<double>& result, fpscrT const& fpscr )
    { result = SmartValue<double>( new DefaultNaNNode( 64 ) ); }
    
    template <typename FLOAT, typename fpscrT> static
    void SetQuietBit( FLOAT& op, fpscrT const& fpscr )
    {
      op = FLOAT( new UONode( "FSetQuietBit", op.expr ) );
    }

    template <typename FLOAT, typename fpscrT> static
    bool
    FlushToZero( FLOAT& op, fpscrT& fpscr )
    {
      op = FLOAT( new UONode( "FFlushToZero", op.expr ) );
      return false;
    }

    template <typename FLOAT, typename fpscrT> static
    SmartValue<int32_t> Compare( FLOAT op1, FLOAT op2, fpscrT& fpscr )
    {
      return SmartValue<int32_t>( new BONode( "FCmp", op1.expr, op2.expr ) );
    }

    struct IsNaNNode : public ExprNode
    {
      IsNaNNode( Expr const& _src, bool _signaling ) : src(_src), signaling(_signaling) {} Expr src; bool signaling;
      virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); src->Traverse( visitor ); }
      virtual void Repr( std::ostream& sink ) const { sink << "Is" << (signaling?'S':'Q') << "NaN( "; src->Repr( sink ); sink << " )"; }
      intptr_t cmp( ExprNode const& brhs ) const
      {
        IsNaNNode const& rhs = dynamic_cast<IsNaNNode const&>( brhs );
        if (intptr_t delta = src.cmp( rhs.src )) return delta;
        return int(signaling) - int(rhs.signaling);
      }
    };
    
    template <typename FLOAT, typename fpscrT> static
    SmartValue<bool>
    IsSNaN( FLOAT const& op, fpscrT const& fpscr )
    {
      return SmartValue<bool>( new IsNaNNode( op.expr, false ) );
    }
    
    template <typename FLOAT, typename fpscrT> static
    SmartValue<bool>
    IsQNaN( FLOAT const& op, fpscrT const& fpscr )
    {
      return SmartValue<bool>( new IsNaNNode( op.expr, true ) );
    }
    
    template <typename FLOAT, typename fpscrT> static
    void Add( FLOAT& res, FLOAT const& op1, FLOAT const& op2, fpscrT& fpscr )
    {
      res = FLOAT( new BONode( "FAdd", op1.expr, op2.expr ) );
    }

    template <typename FLOAT, typename fpscrT> static
    void Sub( FLOAT& res, FLOAT const& op1, FLOAT const& op2, fpscrT& fpscr )
    {
      res = FLOAT( new BONode( "FSub", op1.expr, op2.expr ) );
    }

    template <typename FLOAT, typename fpscrT> static
    void Div( FLOAT& res, FLOAT const& op1, FLOAT const& op2, fpscrT& fpscr )
    {
      res = FLOAT( new BONode( "FDiv", op1.expr, op2.expr ) );
    }

    template <typename FLOAT, typename fpscrT> static
    void Mul( FLOAT& res, FLOAT const& op1, FLOAT const& op2, fpscrT& fpscr )
    {
      res = FLOAT( new BONode( "FMul", op1.expr, op2.expr ) );
    }

    struct MulAddNode : public ExprNode
    {
      MulAddNode( Expr const& _acc, Expr const& _left, Expr const& _right )
        : acc( _acc ), left( _left ), right( _right ) {}
      Expr acc; Expr left; Expr right;
      virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); acc->Traverse( visitor ); left->Traverse( visitor ); right->Traverse( visitor ); }
      virtual void Repr( std::ostream& sink ) const { sink << "FMulAdd( "; acc->Repr( sink ); sink << ", "; left->Repr( sink ); sink << ", "; right->Repr( sink ); sink << " )"; }
      intptr_t cmp( ExprNode const& brhs ) const
      {
        MulAddNode const& rhs = dynamic_cast<MulAddNode const&>( brhs );
        if (intptr_t delta = left.cmp( rhs.left )) return delta;
        if (intptr_t delta = right.cmp( rhs.right )) return delta;
        return acc.cmp( rhs.acc );
      }
    };
    
    template <typename FLOAT, typename fpscrT> static
    void MulAdd( FLOAT& acc, FLOAT const& op1, FLOAT const& op2, fpscrT& fpscr )
    {
      acc = FLOAT( new MulAddNode( acc.expr, op1.expr, op2.expr ) );
    }

    template <typename FLOAT, typename fpscrT> static
    void Neg( FLOAT& res, FLOAT const& op, fpscrT& fpscr ) { res = FLOAT( new UONode( "FNeg", op.expr ) ); }

    template <typename FLOAT, typename fpscrT> static
    void Sqrt( FLOAT& res, FLOAT const& op, fpscrT& fpscr ) { res = FLOAT( new UONode( "FSqrt", op.expr ) ); }

    struct FtoFNode : public ExprNode
    {
      FtoFNode( Expr const& _src, int _ssz, int _dsz )
        : src( _src ), ssz( _ssz ), dsz( _dsz )
      {} Expr src; int ssz; int dsz;
      virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); src->Traverse( visitor ); }
      virtual void Repr( std::ostream& sink ) const { sink << "F" << ssz << "2F" << dsz << "( "; src->Repr( sink ); sink << " )"; }
      intptr_t cmp( ExprNode const& brhs ) const
      {
        FtoFNode const& rhs = dynamic_cast<FtoFNode const&>( brhs );
        if (intptr_t delta = src.cmp( rhs.src )) return delta;
        if (intptr_t delta = ssz - rhs.ssz) return delta;
        return dsz - rhs.dsz;
      }
    };
    
    template <typename ofpT, typename ifpT, typename fpscrT> static
    void FtoF( SmartValue<ofpT>& res, SmartValue<ifpT> const& op, fpscrT& fpscr )
    {
      res = SmartValue<ofpT>( new FtoFNode( op.expr, 8*sizeof (ifpT), 8*sizeof (ofpT) ) );
    }

    struct FtoINode : public ExprNode
    {
      FtoINode( Expr const& _src, int _fsz, int _isz, int _fb )
        : src( _src ), fsz( _fsz ), isz( _isz ), fb( _fb )
      {} Expr src; int fsz; int isz; int fb; 
      virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); src->Traverse( visitor ); }
      virtual void Repr( std::ostream& sink ) const { sink << "F" << fsz << "2I" << isz << "( "; src->Repr( sink ); sink << ", " << fb << " )"; }
      intptr_t cmp( ExprNode const& brhs ) const
      {
        FtoINode const& rhs = dynamic_cast<FtoINode const&>( brhs );
        if (intptr_t delta = src.cmp( rhs.src )) return delta;
        if (intptr_t delta = isz - rhs.isz) return delta;
        if (intptr_t delta = fsz - rhs.fsz) return delta;
        return fb - rhs.fb;
      }
    };

    template <typename intT, typename fpT, typename fpscrT> static
    void FtoI( SmartValue<intT>& res, SmartValue<fpT> const& op, int fracbits, fpscrT& fpscr )
    {
      res = SmartValue<intT>( new FtoINode( op.expr, 8*sizeof (fpT), 8*sizeof (intT), fracbits) );
    }

    struct ItoFNode : public ExprNode
    {
      ItoFNode( Expr const& _src, int _isz, int _fsz, int _fb )
        : src( _src ), isz( _isz ), fsz( _fsz ), fb( _fb )
      {} Expr src; int isz; int fsz; int fb;
      virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); src->Traverse( visitor ); }
      virtual void Repr( std::ostream& sink ) const { sink << "I" << isz << "2F" << fsz << "( "; src->Repr( sink ); sink << ", " << fb << " )"; }
      intptr_t cmp( ExprNode const& brhs ) const
      {
        ItoFNode const& rhs = dynamic_cast<ItoFNode const&>( brhs );
        if (intptr_t delta = src.cmp( rhs.src )) return delta;
        if (intptr_t delta = isz - rhs.isz) return delta;
        if (intptr_t delta = fsz - rhs.fsz) return delta;
        return fb - rhs.fb;
      }
    };
    
    template <typename fpT, typename intT, typename fpscrT> static
    void ItoF( SmartValue<fpT>& res, SmartValue<intT> const& op, int fracbits, fpscrT& fpscr )
    {
      res = SmartValue<fpT>( new ItoFNode( op.expr, 8*sizeof (intT), 8*sizeof (fpT), fracbits ) );
    }
    
    template <typename FLOAT, typename fpscrT> static
    void Abs( FLOAT& res, FLOAT const& op, fpscrT& fpscr )
    {
      res = FLOAT( new UONode( "FAbs", op.expr ) );
    }
    
  };
} // end of namespace armsec

#endif /* SMART_TYPES_HH */
