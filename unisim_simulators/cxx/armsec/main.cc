#include <unisim/component/cxx/processor/arm/disasm.hh>
#include <unisim/component/cxx/processor/arm/psr.hh>
#include <unisim/util/symbolic/symbolic.hh>
#include <top_arm32.tcc>
#include <top_thumb.tcc>

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <sstream>
#include <cstdlib>

namespace armsec
{
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

  struct dbx
  {
    dbx( unsigned _bits, uint64_t _value ) : value(_value), bits(_bits) {} uint64_t value; unsigned bits;
    friend std::ostream& operator << ( std::ostream& sink, dbx const& _ )
    {
      sink << "0x" << std::hex << std::setw(_.bits/4) << std::setfill('0') << _.value << std::dec;
      return sink;
    }
  };
  
  struct Label
  {
    typedef std::vector<std::string> Program;
    
    Label( Program& _program ) : program(&_program), id(-1) {}
    
    int next() { id = program->size(); program->push_back(""); return id; }
    bool valid() const { return id >= 0; }
    void operator = (std::string const& src) { program->at(id) = src; }
    
    Program* program;
    int id;
  };
  
  struct ASExprNode : public unisim::util::symbolic::ExprNode
  {
    typedef unisim::util::symbolic::Expr Expr;
    virtual int GenCode( Label& label, std::ostream& sink ) const = 0;
    static  int GenerateCode( Expr const& expr, Label& label, std::ostream& sink );
  };

  struct GetCode
  {
    GetCode(ASExprNode::Expr const& _expr, Label& _label) : expr(_expr), label(_label) {} ASExprNode::Expr const& expr; Label& label;
    friend std::ostream& operator << ( std::ostream& sink, GetCode const& gc )
    {
      ASExprNode::GenerateCode( gc.expr, gc.label, sink );
      return sink;
    }
  };
    
  int
  ASExprNode::GenerateCode( ASExprNode::Expr const& expr, Label& label, std::ostream& sink )
  {
    typedef unisim::util::symbolic::ConstNodeBase ConstNodeBase;
    typedef unisim::util::symbolic::ScalarType ScalarType;
    typedef unisim::util::symbolic::CastNodeBase CastNodeBase;
    typedef unisim::util::symbolic::BinaryOp BinaryOp;
    typedef unisim::util::symbolic::UnaryOp UnaryOp;
    typedef unisim::util::symbolic::BONode BONode;
    typedef unisim::util::symbolic::UONode UONode;
    
    /*** Pre expression process ***/
    
    /*** Sub expression process ***/
    if (ASExprNode const* node = dynamic_cast<ASExprNode const*>( expr.node ))
      {
        return node->GenCode( label, sink );
      }
    else if (ConstNodeBase const* node = dynamic_cast<ConstNodeBase const*>( expr.node ))
      {
        switch (node->GetType())
          {
          case ScalarType::BOOL: sink << node->GetS32() << "<1>";  return 1;
          case ScalarType::U8:  case ScalarType::S8:  sink << dbx(8, node->GetU8());  return 8;
          case ScalarType::U16: case ScalarType::S16: sink << dbx(16,node->GetU16()); return 16;
          case ScalarType::U32: case ScalarType::S32: sink << dbx(32,node->GetU32()); return 32;
          case ScalarType::U64: case ScalarType::S64: sink << dbx(64,node->GetU64()); return 64;
          default: break;
          }
        throw std::logic_error("can't encode type");
      }
    else if (BONode const* node = dynamic_cast<BONode const*>( expr.node ))
      {
        int retsz = GenerateCode( node->left, label, sink << '(' );
          
        switch (node->binop.code)
          {
          default:                sink << " [" << node->binop.c_str() << "] "; break;
            
          case BinaryOp::Add:     sink << " + "; break;
          case BinaryOp::Sub:     sink << " - "; break;
        
          case BinaryOp::Xor:     sink << " xor "; break;
          case BinaryOp::Or:      sink << " or "; break;
          case BinaryOp::And:     sink << " and "; break;
        
          case BinaryOp::Teq:     sink << " = "; break;
          case BinaryOp::Tne:     sink << " <> "; break;
        
          case BinaryOp::Tle:     sink << " <=s "; break;
          case BinaryOp::Tleu:    sink << " <=u "; break;
        
          case BinaryOp::Tge:     sink << " >=s "; break;
          case BinaryOp::Tgeu:    sink << " >=u "; break;
        
          case BinaryOp::Tlt:     sink << " <s "; break;
          case BinaryOp::Tltu:    sink << " <u "; break;
        
          case BinaryOp::Tgt:     sink << " >s "; break;
          case BinaryOp::Tgtu:    sink << " >u "; break;
      
          case BinaryOp::Lsl:     sink << " lshift "; break;
          case BinaryOp::Asr:     sink << " rshifts "; break;
          case BinaryOp::Lsr:     sink << " rshiftu "; break;
          case BinaryOp::Ror:     sink << " rrotate "; break;
          case BinaryOp::Mul:     sink << " * "; break;
            // case BinaryOp::Mod: break;
            // case BinaryOp::Div: break;
          }
          
        sink << GetCode( node->right, label ) << ')';
        return retsz;
      }
    else if (UONode const* node = dynamic_cast<UONode const*>( expr.node ))
      {
        char const* operation = 0;
        
        switch (node->unop.code)
          {
          default:              operation = node->unop.c_str(); break;
        
          case UnaryOp::Not:    operation = "not "; break;
          case UnaryOp::Neg:    operation = "- "; break;
        
            // case UnaryOp::BSwp:  break;
            // case UnaryOp::BSF:   break;
            
          case UnaryOp::BSR:
            {
              Label tail(label);
              {
                std::ostringstream buffer;
                buffer << "bsr_in<32> := " << GetCode(node->src,tail);
                Label insn( tail );
                buffer << "; goto " << tail.next();
                insn = buffer.str();
              }
              {
                std::ostringstream code;
                Label insn( tail );
                code << "bsr_out<32> := 32<32> ; goto " << tail.next();
                insn = code.str();
              }
              Label exit(tail);
              exit.next();
              {
                std::ostringstream code;
                Label insn( tail );
                code << "if (bsr_in<32> = 0<32>) goto " << exit.id << " else goto " << tail.next();
                insn = code.str();
              }
              Label loop(tail);
              {
                std::ostringstream code;
                code << "bsr_out<32> := bsr_out<32> - 1<32> ; goto " << tail.next();
                loop = code.str();
              }
              {
                std::ostringstream code;
                Label insn( tail );
                code << "if ((bsr_in<32> rshiftu bsr_out<32>){0,0}) goto " << exit.id << " else goto " << loop.id;
                insn = code.str();
              }
              label = exit;
      
              sink << "bsr_out<32>";
      
              return 32;
            }
          }
    
        sink << '(' << operation << ' ';
        int retsz = GenerateCode( node->src, label, sink );
        sink << ')';
        return retsz;
      }
    else if (CastNodeBase const* node = dynamic_cast<CastNodeBase const*>( expr.node ))
      {
        ScalarType src( node->GetSrcType() ), dst( node->GetDstType() );
          
        if (dst.is_integer and src.is_integer)
          {
            int extend = dst.bitsize - src.bitsize;
            if      (extend > 0)
              {
                sink << (src.is_signed ? "exts " : "extu ") << GetCode(node->src, label ) << ' ' << dst.bitsize;
              }
            else
              {
                CastNodeBase const* src_node = dynamic_cast<CastNodeBase const*>( node->src.node );
                if (src_node and (src_node->GetSrcType() == node->GetDstType())) {
                  sink << GetCode(src_node->src, label);
                } else {
                  sink << GetCode(node->src, label);
                  if  (extend < 0)
                    sink << " {0," << (dst.bitsize-1) << "}";
                }
              }
          }
        else
          {
            sink << dst.name << "( " << GetCode(node->src, label ) << " )";
          }
      
        return dst.bitsize;
      }
      
    throw std::logic_error("No GenCode method");
    return 0;
  }

  // virtual int DefaultNaNNode::GenCode( Label& label, std::ostream& sink ) const
  //     { sink << "F" << fsz << "DefaultNaN()"; return fsz; }
  
  // virtual int FtoINode::GenCode( Label& label, std::ostream& sink ) const
  //     {
  //       sink << "F" << fsz << "2I" << isz << "( " << GetCode(src,label) << ", " << fb << " )";
  //       return isz;
  //     }
  
  // virtual int ItoFNode::GenCode( Label& label, std::ostream& sink ) const
  //     { sink << "I" << isz << "2F" << fsz << "( " << GetCode(src,label) << ", " << fb << " )"; return fsz; }
      
  // virtual int FtoFNode::GenCode( Label& label, std::ostream& sink ) const
  //     {
  //       sink << "F" << ssz << "2F" << dsz << "( " << GetCode(src,label) << " )";
  //       return dsz;
  //     }
  
  // virtual int IsInvalidMulAddNode::GenCode( Label& label, std::ostream& sink ) const
  //     {
  //       sink << "FIsInvalidMulAdd( " << GetCode(acc,label)
  //            << ", " << GetCode(left,label)
  //            << ", " << GetCode(right,label) << " )";
  //       return 1;
  //     }

  // virtual int MulAddNode::GenCode( Label& label, std::ostream& sink ) const
  //     {
  //       sink << "FMulAdd( ";
  //       int retsize = 0;
  //       sink << GetCode(acc, label, retsize ) << ", " << GetCode(left,label) << ", " << GetCode(right,label) << " )";
  //       return retsize;
  //     }
      
  // virtual int IsNaNNode::GenCode( Label& label, std::ostream& sink ) const
  //     { sink << "Is" << (signaling?'S':'Q') << "NaN( " << GetCode(src,label) << " )"; return 1; }

  struct Label;
  
  struct PathNode
  {
    typedef unisim::util::symbolic::Expr Expr;
    typedef unisim::util::symbolic::ExprNode ExprNode;
    
    PathNode( PathNode* _previous=0 )
      : cond(), sinks(), previous(_previous), true_nxt(), false_nxt(), complete(false)
    {}
    
    PathNode* proceed( Expr const& _cond, bool& result )
    {
      if (not cond.good())
        {
          cond = _cond;
          false_nxt = new PathNode(this);
          true_nxt = new PathNode(this);
          result = false;
          return false_nxt;
        }
      
      if (cond != _cond)
        throw std::logic_error( "unexpected condition" );
      
      if (not false_nxt->complete)
        {
          result = false;
          return false_nxt;
        }
      
      if (true_nxt->complete)
        throw std::logic_error( "unexpected path" );
      
      result = true;
      return true_nxt;
    }
    
    bool close()
    {
      complete = true;
      if (not previous)
        return true;
      if (this == previous->true_nxt)
        return previous->close();
      return false;
    }
    
    bool remove_dead_paths()
    {
      if (cond.good()) {
        bool leaf = true;
        if (false_nxt) {
          if (false_nxt->remove_dead_paths())
            {
              delete false_nxt;
              false_nxt = 0;
            }
          else
            leaf = false;
        }
        if (true_nxt) {
          if (true_nxt->remove_dead_paths())
            {
              delete true_nxt;
              true_nxt = 0;
            }
          else
            leaf = false;
        }
        if (not leaf)
          return false;
      }
      // This is a leaf; if no local sinks, signal dead path to parent
      return sinks.size() == 0;
    }
    
    void factorize()
    {
      if (not cond.good())
        return;
      
      false_nxt->factorize();
      true_nxt->factorize();
      
      std::vector<Expr> isect;
      {
        isect.resize( std::max( false_nxt->sinks.size(), true_nxt->sinks.size() ) );
        std::vector<Expr>::iterator end =
          std::set_intersection( false_nxt->sinks.begin(), false_nxt->sinks.end(),
                                 true_nxt->sinks.begin(), true_nxt->sinks.end(),
                                 isect.begin() );
        isect.resize( end - isect.begin() );
      }
      
      for (std::vector<Expr>::const_iterator itr = isect.begin(), end = isect.end(); itr != end; ++itr) {
        sinks.insert( *itr );
        false_nxt->sinks.erase( *itr );
        true_nxt->sinks.erase( *itr );
      }
      
      // If condition begins with a logical not, remove the not and
      //   swap if then else branches
      typedef unisim::util::symbolic::UONode UONode;
      if (UONode* uon = dynamic_cast<UONode*>( cond.node ))
        if (uon->unop.cmp( unisim::util::symbolic::UnaryOp("Not") ) == 0) {
          cond = uon->src;
          std::swap( false_nxt, true_nxt );
        }
    }
  
    struct Context
    {
      Context() : upper(0) {}
      Context( Context* _up ) : upper(_up) {}
      
      void add_pending( Expr e ) { pendings.push_back(e); }
      bool has_pending() const { return pendings.size() > 0; }
      
      void add_var( Expr e, Expr n ) { vars[e] = n; }
      
      Context* upper;
      typedef std::vector<Expr> Pendings;
      Pendings pendings;
      typedef std::map<Expr,Expr> Vars;
      Vars vars;
    };

    void GenCode( Label const&, Label const&, Context* ) const;
    
    Expr cond;
    std::set<Expr> sinks;
    PathNode* previous;
    PathNode* true_nxt;
    PathNode* false_nxt;
    bool complete;
  };
  
  struct State
  {
    typedef armsec::F64  F64;
    typedef armsec::F32  F32;
    typedef armsec::BOOL BOOL;
    typedef armsec::U8   U8;
    typedef armsec::U16  U16;
    typedef armsec::U32  U32;
    typedef armsec::U64  U64;
    typedef armsec::S8   S8;
    typedef armsec::S16  S16;
    typedef armsec::S32  S32;
    typedef armsec::S64  S64;
    
    typedef unisim::util::symbolic::FP   FP;
    typedef unisim::util::symbolic::Expr Expr;
    
    template <typename T>
    bool Cond( unisim::util::symbolic::SmartValue<T> cond )
    {
      if (not cond.expr.node)
        throw std::logic_error( "Not a cond" );
      
      typedef unisim::util::symbolic::ConstNodeBase ConstNodeBase;
      if (ConstNodeBase* cnode = dynamic_cast<ConstNodeBase*>( cond.expr->GetConstNode() ))
        return cnode->GetU8();
      
      bool result;
      path = path->proceed( cond.expr, result );
      return result;
    }
    
    struct Config
    {
      // Following a standard armv7 configuration
      static uint32_t const model = unisim::component::cxx::processor::arm::ARMEMU;
      static bool const     insns4T = true;
      static bool const     insns5E = true;
      static bool const     insns5J = true;
      static bool const     insns5T = true;
      static bool const     insns6  = true;
      static bool const     insnsRM = false;
      static bool const     insnsT2 = true;
      static bool const     insns7  = true;

      struct DisasmState {};
    
    };
    
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

    /* values of the different condition codes */
    static uint32_t const COND_EQ = 0x00;
    static uint32_t const COND_NE = 0x01;
    static uint32_t const COND_CS_HS = 0x02;
    static uint32_t const COND_CC_LO = 0x03;
    static uint32_t const COND_MI = 0x04;
    static uint32_t const COND_PL = 0x05;
    static uint32_t const COND_VS = 0x06;
    static uint32_t const COND_VC = 0x07;
    static uint32_t const COND_HI = 0x08;
    static uint32_t const COND_LS = 0x09;
    static uint32_t const COND_GE = 0x0a;
    static uint32_t const COND_LT = 0x0b;
    static uint32_t const COND_GT = 0x0c;
    static uint32_t const COND_LE = 0x0d;
    static uint32_t const COND_AL = 0x0e;
    
    /* mask for valid bits in processor control and status registers */
    static uint32_t const PSR_UNALLOC_MASK = 0x00f00000;
    
    struct RegID
    {
      enum Code
        {
          NA = 0
          , r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, sp, lr, nia
          , n, z, c, v// , q, ge0, ge1, ge2, ge3
          , cpsr, spsr
          , fpscr, fpexc
        };
      
      template <class E> static void
      Enumeration( E& e )
      {
        if (e(     "r0", r0     )) return;
        if (e(     "r1", r1     )) return;
        if (e(     "r2", r2     )) return;
        if (e(     "r3", r3     )) return;
        if (e(     "r4", r4     )) return;
        if (e(     "r5", r5     )) return;
        if (e(     "r6", r6     )) return;
        if (e(     "r7", r7     )) return;
        if (e(     "r8", r8     )) return;
        if (e(     "r9", r9     )) return;
        if (e(     "sl", sl     )) return;
        if (e(     "fp", fp     )) return;
        if (e(     "ip", ip     )) return;
        if (e(     "sp", sp     )) return;
        if (e(     "lr", lr     )) return;
        if (e(     "pc", nia    )) return;
        if (e(      "n", n      )) return;
        if (e(      "z", z      )) return;
        if (e(      "c", c      )) return;
        if (e(      "v", v      )) return;
        if (e(   "cpsr", cpsr   )) return;
        if (e(   "spsr", spsr   )) return;
        if (e(  "fpscr", fpscr  )) return;
        if (e(  "fpexc", fpexc  )) return;
      }
    
      RegID( Code _code ) : code(_code) {}
      RegID( char const* _code ) : code(NA) { unisim::util::symbolic::cstr2enum( *this, _code ); }
      
      char const* c_str() const { return unisim::util::symbolic::enum2cstr( *this, "NA" ); }
      RegID operator + ( int offset ) const { return RegID( Code(int(code) + offset) ); }
      int operator - ( RegID rid ) const { return int(code) - int(rid.code); }
      bool operator == ( RegID rid ) const { return code == rid.code; }
      
      Code code;
    };
    
    struct RegRead : public ASExprNode
    {
      RegRead( RegID _id, unsigned _bitsize ) : id(_id), bitsize(_bitsize) {}
      RegRead( char const* name, unsigned _bitsize ) : id( name ), bitsize(_bitsize) {}
      virtual int GenCode( Label& label, std::ostream& sink ) const { sink << id.c_str() << "<" << bitsize << ">"; return bitsize; }
      virtual intptr_t cmp( ExprNode const& brhs ) const {
        RegRead const& rhs = dynamic_cast<RegRead const&>( brhs );
        return id - rhs.id;
      }
      virtual void Traverse( Visitor& visitor ) {}
      virtual void Repr( std::ostream& sink ) const { sink << id.c_str(); }
      virtual ExprNode* GetConstNode() { return 0; };
      
      RegID id;
      unsigned bitsize;
    };
    
    struct RegWrite : public ASExprNode
    {
      RegWrite( RegID _id, Expr const& _value, unsigned _bitsize ) : id(_id), value(_value), bitsize(_bitsize) {}
      RegWrite( char const* name, Expr const& _value, unsigned _bitsize ) : id(name), value(_value), bitsize(_bitsize) {}
      virtual int GenCode( Label& label, std::ostream& sink ) const
      {
        sink << id.c_str() << "<" << std::dec << bitsize << "> := " << GetCode(value,label);
        return 0;
      }
      
      virtual intptr_t cmp( ExprNode const& brhs ) const
      {
        RegWrite const& rhs = dynamic_cast<RegWrite const&>( brhs );
        if (intptr_t delta = id - rhs.id) return delta;
        return value.cmp( rhs.value );
      }
      
      virtual void Traverse( Visitor& visitor ) { visitor.Process( value ); }
      virtual void Repr( std::ostream& sink ) const { sink << id.c_str() << " := "; value->Repr(sink); }
      
      virtual ExprNode* GetConstNode() { return 0; };
      
      RegID id;
      Expr value;
      unsigned bitsize;
    };
    
    State( PathNode* _path )
      : path( _path )
      , next_insn_addr()
      , cpsr( Expr( new RegRead("n",1) ),
              Expr( new RegRead("z",1) ),
              Expr( new RegRead("c",1) ),
              Expr( new RegRead("v",1) ),
              Expr( new RegRead("cpsr",32) ) )
      , spsr( Expr( new RegRead("spsr",32) ) )
      , FPSCR( Expr( new RegRead("fpscr",32) ) )
      , FPEXC( Expr( new RegRead("fpexc",32) ) )
    {
      for (unsigned reg = 0; reg < 15; ++reg)
        reg_values[reg] = U32( Expr( new RegRead( RegID("r0") + reg, 32 ) ) );
    }
    
    void SetInsnProps( Expr const& _expr, bool is_thumb, unsigned insn_length )
    {
      if ((insn_length != 32) and ((insn_length != 16) or not is_thumb))
        throw std::logic_error( "Bad instruction length" );
      U32 insn_addr = _expr;
      reg_values[15] = insn_addr + U32( is_thumb ? 4 : 8 );
      next_insn_addr = insn_addr + U32( insn_length / 8 );
    }
    
    PathNode* path;
    
    U32 reg_values[16];
    U32 next_insn_addr;
    //typedef unisim::component::cxx::processor::arm::FieldRegister<U32> FieldRegisterU32;
    //struct psr_type : public FieldRegisterU32
    struct psr_type
    {
      psr_type( Expr const& _n, Expr const& _z, Expr const& _c, Expr const& _v, Expr const& _bg )
        : n(_n), z(_z), c(_c), v(_v), bg(_bg)
      {}
      Expr n, z, c, v;
      U32 bg;
      
      typedef unisim::component::cxx::processor::arm::RegisterField<31,1> NRF; /* Negative Integer Condition Flag */
      typedef unisim::component::cxx::processor::arm::RegisterField<30,1> ZRF; /* Zero     Integer Condition Flag */
      typedef unisim::component::cxx::processor::arm::RegisterField<29,1> CRF; /* Carry    Integer Condition Flag */
      typedef unisim::component::cxx::processor::arm::RegisterField<28,1> VRF; /* Overflow Integer Condition Flag */
      
      typedef unisim::component::cxx::processor::arm::RegisterField<28,4> NZCVRF; /* Grouped Integer Condition Flags */
      
      typedef unisim::component::cxx::processor::arm::RegisterField<27,1> QRF; /* Cumulative saturation flag */
      
      typedef unisim::component::cxx::processor::arm::RegisterField< 9,1> ERF; /* Endianness execution state */
      typedef unisim::component::cxx::processor::arm::RegisterField< 0,5> MRF; /* Mode field */
      
      typedef unisim::component::cxx::processor::arm::RegisterField< 0,32> ALL;
      
      template <typename RF>
      U32 Set( RF const& _, U32 const& value )
      {
        unisim::util::symbolic::StaticAssert<(RF::pos > 31) or ((RF::pos + RF::size) <= 28)>::check();
        
        return _.Get( bg );
      }
      
      void Set( NRF const& _, BOOL const& value ) { n = value.expr; }
      void Set( ZRF const& _, BOOL const& value ) { z = value.expr; }
      void Set( CRF const& _, BOOL const& value ) { c = value.expr; }
      void Set( VRF const& _, BOOL const& value ) { v = value.expr; }

      void Set( QRF const& _, U32 const& value ) { _.Set( bg, value ); }
      void Set( ERF const& _, U32 const& value ) { _.Set( bg, value ); }
      
      void Set( NZCVRF const& _, U32 const& value )
      {
        n = BOOL( unisim::component::cxx::processor::arm::RegisterField< 3,1>().Get( value ) ).expr;
        z = BOOL( unisim::component::cxx::processor::arm::RegisterField< 2,1>().Get( value ) ).expr;
        c = BOOL( unisim::component::cxx::processor::arm::RegisterField< 1,1>().Get( value ) ).expr;
        v = BOOL( unisim::component::cxx::processor::arm::RegisterField< 0,1>().Get( value ) ).expr;
      }
      
      void Set( ALL const& _, U32 const& value )
      {
        n = BOOL( NRF().Get( value ) ).expr;
        z = BOOL( ZRF().Get( value ) ).expr;
        c = BOOL( CRF().Get( value ) ).expr;
        v = BOOL( VRF().Get( value ) ).expr;
        
        unisim::component::cxx::processor::arm::RegisterField<0,28>().Set( bg, value );
      }
      
      template <typename RF>
      U32 Get( RF const& _ )
      {
        unisim::util::symbolic::StaticAssert<(RF::pos > 31) or ((RF::pos + RF::size) <= 28)>::check();
        
        return _.Get( bg );
      }
      
      U32 Get( NRF const& _ ) { return U32(BOOL(n)); }
      U32 Get( ZRF const& _ ) { return U32(BOOL(z)); }
      U32 Get( CRF const& _ ) { return U32(BOOL(c)); }
      U32 Get( VRF const& _ ) { return U32(BOOL(v)); }
      
      // U32 Get( ALL const& _ ) { return (U32(BOOL(n)) << 31) | (U32(BOOL(z)) << 30) | (U32(BOOL(c)) << 29) | (U32(BOOL(v)) << 28) | bg; }
      
      U32 bits() const { return (U32(BOOL(n)) << 31) | (U32(BOOL(z)) << 30) | (U32(BOOL(c)) << 29) | (U32(BOOL(v)) << 28) | bg; }
    } cpsr;
    
    U32 spsr;
    
    void FPTrap( unsigned exc )
    {
      throw std::logic_error("unimplemented");
    }
    
    U32 FPSCR, FPEXC;
    
    //U32 StandardValuedFPSCR() const   { return AHP.Mask( FPSCR ) | 0x03000000; }
    
    U32 RoundTowardsZeroFPSCR() const
    {
      U32 fpscr = FPSCR;
      unisim::component::cxx::processor::arm::RMode.Set( fpscr, U32(unisim::component::cxx::processor::arm::RoundTowardsZero) );
      return fpscr;
    }
    
    U32 RoundToNearestFPSCR() const
    {
      U32 fpscr = FPSCR;
      unisim::component::cxx::processor::arm::RMode.Set( fpscr, U32(unisim::component::cxx::processor::arm::RoundToNearest) );
      return fpscr;
    }
    
    void not_implemented() { throw std::logic_error( "not implemented" ); }

    U32  GetGPR_usr( uint32_t id ) { /* Only work in system mode instruction */ not_implemented(); return U32(); }
    U32  GetGPR( uint32_t id ) { return reg_values[id]; }
    void SetGPR_usr( uint32_t id, U32 const& value ) { /* Only work in system mode instruction */ not_implemented(); }
    
    // TODO: interworking branches are not correctly handled
    void SetGPR_mem( uint32_t id, U32 const& value )
    {
      if (id != 15)
        reg_values[id] = value;
      else
        next_insn_addr = value;
    }
    void SetGPR( uint32_t id, U32 const& value ) {
      if (id != 15)
        reg_values[id] = value;
      else
        next_insn_addr = value;
    }
    
    U32 GetNIA() { return next_insn_addr; }

    psr_type& CPSR() { return cpsr; };
    U32& SPSR() { not_implemented(); return spsr; };
    
    void SetGPRMapping( uint32_t src_mode, uint32_t tar_mode ) { /* system related */ not_implemented(); }
    
    struct Load : public ASExprNode
    {
      Load( unsigned _size, bool _aligned, Expr const& _addr )
        : addr(_addr), size( _size ), aligned(_aligned)
      {}
      
      virtual void Traverse( Visitor& visitor ) { visitor.Process( addr ); }
      virtual void Repr( std::ostream& sink ) const { sink << "["; addr->Repr( sink ); sink << "," << size << "," << (aligned?'a':'u') << "]"; }
      virtual int GenCode( Label& label, std::ostream& sink ) const
      {
        /* TODO: dont assume little endianness */
        /* TODO: exploit alignment info */
        sink << "@[" << GetCode(addr,label) << ",<-," << size << "]";
        return 8*size;
      }
      intptr_t cmp( ExprNode const& brhs ) const
      {
        Load const& rhs = dynamic_cast<Load const&>( brhs );
        if (intptr_t delta = addr.cmp( rhs.addr )) return delta;
        if (intptr_t delta = int(size - rhs.size)) return delta;
        return (int(aligned) - int(rhs.aligned));
      }
      ExprNode* GetConstNode() { return 0; };
      Expr addr;
      unsigned size;
      bool aligned;
    };
    
    struct Store : public ASExprNode
    {
      Store( unsigned _size, bool _aligned, Expr const& _addr, Expr const& _value )
        : value(_value), addr(_addr), size(_size), aligned(_aligned)
      {}
      
      virtual int GenCode( Label& label, std::ostream& sink ) const
      {
        /* TODO: dont assume little endianness */
        /* TODO: exploit alignment info */
        sink << "@[" << GetCode(addr,label) << ",<-," << size << "] := " << GetCode(value, label);
        return 0;
      }
      
      virtual void Traverse( Visitor& visitor ) { visitor.Process( addr ); visitor.Process( value ); }
      virtual void Repr( std::ostream& sink ) const { sink << "["; addr->Repr( sink ); sink << "," << size << "," << (aligned?'a':'u') << "] := "; value->Repr(sink); }
      
      intptr_t cmp( ExprNode const& brhs ) const
      {
        Store const& rhs = dynamic_cast<Store const&>( brhs );
        if (intptr_t delta = value.cmp( rhs.value )) return delta;
        if (intptr_t delta = addr.cmp( rhs.addr )) return delta;
        if (intptr_t delta = int(size - rhs.size)) return delta;
        return (int(aligned) - int(rhs.aligned));
      }
      
      ExprNode* GetConstNode() { return 0; };
      
      Expr value, addr;
      unsigned size;
      bool aligned;
    };
    
    std::set<Expr> stores;
    
    U32  MemURead32( U32 const& addr ) { return U32( Expr( new Load( 4, false, addr.expr ) ) ); }
    U16  MemURead16( U32 const& addr ) { return U16( Expr( new Load( 2, false, addr.expr ) ) ); }
    U32  MemRead32( U32 const& addr ) { return U32( Expr( new Load( 4, true, addr.expr ) ) ); }
    U16  MemRead16( U32 const& addr ) { return U16( Expr( new Load( 2, true, addr.expr ) ) ); }
    U8   MemRead8( U32 const& addr ) { return U8( Expr( new Load( 1, false, addr.expr ) ) ); }
    
    void MemUWrite32( U32 const& addr, U32 const& value ) { stores.insert( new Store( 4, false, addr.expr, value.expr ) ); }
    void MemUWrite16( U32 const& addr, U16 const& value ) { stores.insert( new Store( 2, false, addr.expr, value.expr ) ); }
    void MemWrite32( U32 const& addr, U32 const& value ) { stores.insert( new Store( 4, true, addr.expr, value.expr ) ); }
    void MemWrite16( U32 const& addr, U16 const& value ) { stores.insert( new Store( 2, true, addr.expr, value.expr ) ); }
    void MemWrite8( U32 const& addr, U8 const& value ) { stores.insert( new Store( 1, false, addr.expr, value.expr ) ); }
    
    void SetExclusiveMonitors( U32 const& address, unsigned size ) { not_implemented(); }
    bool ExclusiveMonitorsPass( U32 const& address, unsigned size ) { not_implemented(); return false; }
    void ClearExclusiveLocal() { not_implemented(); }
    
    void BranchExchange( U32 const& target ) { next_insn_addr = target; }
    void Branch( U32 const& target ) { next_insn_addr = target; }
    
    void WaitForInterrupt() { not_implemented(); }
    void SWI( uint32_t imm ) { not_implemented(); }
    void BKPT( uint32_t imm ) { not_implemented(); }
    void UnpredictableInsnBehaviour() { not_implemented(); }
    template <typename OP>
    void UndefinedInstruction( OP* op ) { not_implemented(); }
    void CallSupervisor( uint16_t imm ) { not_implemented(); }
    bool IntegerZeroDivide( BOOL const& condition ) { return false; }

    U32  GetVU32( unsigned idx ) { return U32(); }
    void SetVU32( unsigned idx, U32 val ) {}
    U64  GetVU64( unsigned idx ) { return U64(); }
    void SetVU64( unsigned idx, U64 val ) {}
    F32  GetVSR( unsigned idx ) { return F32(); }
    void SetVSR( unsigned idx, F32 val ) {}
    F64  GetVDR( unsigned idx ) { return F64(); }
    void SetVDR( unsigned idx, F64 val ) {}
  
    uint32_t itcond() const { return this->COND_AL; }
    bool itblock() { return false; }
    void ITSetState( uint32_t cond, uint32_t mask ) { not_implemented(); }

    // /* masks for the different running modes */
    // static uint32_t const RUNNING_MODE_MASK = 0x1F;
    // static uint32_t const USER_MODE = 0x10;
    // static uint32_t const FIQ_MODE = 0x11;
    // static uint32_t const IRQ_MODE = 0x12;
    // static uint32_t const SUPERVISOR_MODE = 0x13;
    // static uint32_t const ABORT_MODE = 0x17;
    // static uint32_t const UNDEFINED_MODE = 0x1B;
    // static uint32_t const SYSTEM_MODE = 0x1F;
    
    struct Mode
    {
      Mode() {}
      bool     HasBR( unsigned index ) { return false; }
      bool     HasSPSR() { return false; }
      void     SetSPSR(U32 value) {};
      U32      GetSPSR() { return U32(); };
      void     Swap( State& ) {};
      
    } mode;
    
    Mode&  CurrentMode() { not_implemented(); return mode; }
    Mode&  GetMode(uint8_t) { not_implemented(); return mode; }
    U32  GetBankedRegister( uint8_t foreign_mode, uint32_t idx ) { not_implemented(); return U32(); }
    void SetBankedRegister( uint8_t foreign_mode, uint32_t idx, U32 value ) { not_implemented(); }
    
    U32         CP15ReadRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 ) { not_implemented(); return U32(); }
    void        CP15WriteRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2, U32 const& value ) { not_implemented(); }
    char const* CP15DescribeRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 ) { return "some CP15 register"; }
    
    bool
    close( State const& ref )
    {
      bool complete = path->close();
      path->sinks.insert( Expr( new RegWrite( "pc", next_insn_addr.expr, 32 ) ) );
      if (cpsr.n != ref.cpsr.n)
        path->sinks.insert( Expr( new RegWrite( "n", cpsr.n, 1 ) ) );
      if (cpsr.z != ref.cpsr.z)
        path->sinks.insert( Expr( new RegWrite( "z", cpsr.z, 1 ) ) );
      if (cpsr.c != ref.cpsr.c)
        path->sinks.insert( Expr( new RegWrite( "c", cpsr.c, 1 ) ) );
      if (cpsr.v != ref.cpsr.v)
        path->sinks.insert( Expr( new RegWrite( "v", cpsr.v, 1 ) ) );
      if (cpsr.bg.expr != ref.cpsr.bg.expr)
        path->sinks.insert( Expr( new RegWrite( "cpsr", cpsr.bg.expr, 32 ) ) );
      if (spsr.expr != ref.spsr.expr)
        path->sinks.insert( Expr( new RegWrite( "spsr", spsr.expr, 32 ) ) );
      if (FPSCR.expr != ref.FPSCR.expr)
        path->sinks.insert( Expr( new RegWrite( "fpscr", FPSCR.expr, 32 ) ) );
      for (unsigned reg = 0; reg < 15; ++reg) {
        if (reg_values[reg].expr != ref.reg_values[reg].expr)
          path->sinks.insert( Expr( new RegWrite( RegID("r0") + reg, reg_values[reg].expr, 32 ) ) );
      }
      for (std::set<Expr>::const_iterator itr = stores.begin(), end = stores.end(); itr != end; ++itr)
        path->sinks.insert( *itr );
      return complete;
    }
  };

  // struct ConditionTest : public ASExprNode
  // {
  //   ConditionTest( unsigned _cond ) : cond( _cond ) {} unsigned cond;
  //   virtual void Repr( std::ostream& sink ) const
  //   {
      
  //     sink << "CTST_" << unisim::component::cxx::processor::arm::DisasmCondition(cond) << "(flags)";
  //   }
  //   virtual intptr_t cmp( ExprNode const& brhs ) const {
  //     ConditionTest const& rhs = dynamic_cast<ConditionTest const&>( brhs );
  //     return int(cond) - int(rhs.cond);
  //   }
  //   virtual ExprNode* GetConstNode() { return 0; };
  // };

  BOOL
  CheckCondition( State& state, unsigned cond )
  {
    BOOL N = state.cpsr.n, Z = state.cpsr.z, C = state.cpsr.c, V = state.cpsr.v;
    
    switch (cond) {
    case  0: return                   Z; // eq; equal
    case  1: return               not Z; // ne; not equal
    case  2: return                   C; // cs/hs; unsigned higher or same
    case  3: return               not C; // cc/lo; unsigned lower
    case  4: return                   N; // mi; negative
    case  5: return               not N; // pl; positive or zero
    case  6: return                   V; // vs; overflow set
    case  7: return               not V; // vc; overflow clear
    case  8: return    not (not C or Z); // hi; unsigned higher
    case  9: return        (not C or Z); // ls; unsigned lower or same
    case 10: return       not (N xor V); // ge; signed greater than or equal
    case 11: return           (N xor V); // lt; signed less than
    case 12: return not(Z or (N xor V)); // gt; signed greater than
    case 13: return    (Z or (N xor V)); // le; signed less than or equal
    case 14: return unisim::util::symbolic::make_const( true ); // al; always
    default: break;                     // nv; never (illegal)     
    }
    
    throw std::logic_error( "undefined condition" );
    return unisim::util::symbolic::make_const( false );    
  }
  
  // struct GenFlagsID
  // {
  //   enum Code { NA = 0, Sub, Add };
    
  //   template <class E> static void
  //   Enumeration( E& e )
  //   {
  //     if (e( "Sub", Sub )) return;
  //     if (e( "Add", Add )) return;
  //   }
    
  //   GenFlagsID( Code _code ) : code(_code) {}
  //   GenFlagsID( char const* _code ) : code(NA) { cstr2enum( *this, _code ); }
    
  //   char const* c_str() const { return enum2cstr( *this, "NA" ); }
  //   GenFlagsID operator + ( int offset ) const { return GenFlagsID( Code(int(code) + offset) ); }
  //   int operator - ( GenFlagsID rid ) const { return int(code) - int(rid.code); }
      
  //   Code code;
  // };
  
  // struct GenFlags : public ASExprNode
  // {
  //   GenFlags( GenFlagsID _id, Expr const& _ipsr, Expr const& _lhs, Expr const& _rhs )
  //     : id(_id), ipsr(_ipsr), lhs(_lhs), rhs(_rhs) {}
    
  //   virtual void Repr( std::ostream& sink ) const
  //   {
  //     sink << "GenFlags(" << id.c_str() << ", " << ipsr << ", " << lhs << ", " << rhs << ")";
  //   }
  //   virtual intptr_t cmp( ExprNode const& bright ) const
  //   {
  //     GenFlags const& right = dynamic_cast<GenFlags const&>( bright );
  //     if (int delta = id - right.id) return delta;
  //     if (intptr_t delta = ipsr.cmp(right.ipsr)) return delta;
  //     if (intptr_t delta = lhs.cmp(right.lhs)) return delta;
  //     return rhs.cmp(right.rhs);
  //   }
  //   virtual ExprNode* GetConstNode() { return 0; };
  //   GenFlagsID id;
  //   Expr ipsr, lhs, rhs;
  // };

  
  void
  UpdateStatusSub( State& state, U32 const& res, U32 const& lhs, U32 const& rhs )
  {
    BOOL neg = S32(res) < S32(0);
    state.cpsr.n = neg.expr;
    state.cpsr.z = ( res == U32(0) ).expr;
    state.cpsr.c = ( lhs > rhs ).expr;
    state.cpsr.v = ( neg xor (S32(lhs) < S32(rhs)) ).expr;
  }
  
  void
  UpdateStatusAdd( State& state, U32 const& res, U32 const& lhs, U32 const& rhs )
  {
    UpdateStatusSub( state, res, lhs, -rhs );
  }
  
  void
  PathNode::GenCode( Label const& start, Label const& after, Context* _up ) const
  {
    Context ctx( _up );
    
    Expr nia;
    
    Label current( start );
    
    for (std::set<Expr>::const_iterator itr = sinks.begin(), end = sinks.end(); itr != end; ++itr)
      {
        
        struct CSE : public ExprNode::Visitor
        {
          virtual void Process( Expr& expr )
          {
            for (Context* c = &ctx; c; c = c->upper)
              {
                Context::Vars::iterator itr = c->vars.find( expr );
                if (itr != c->vars.end())
                  {
                    // Found a common sub-expression
                    //std::cerr << "Found a common sub-expression\n";
                    expr = itr->second;
                    return;
                  }
              }
            expr->Traverse( *this );
          }
          
          CSE( Context& _ctx ) : ctx(_ctx) {} Context& ctx;
        } cse( ctx );
        
        itr->node->Traverse( cse );
        
        if (armsec::State::RegWrite* rw = dynamic_cast<armsec::State::RegWrite*>( itr->node ))
          {
            Expr wb;
            armsec::State::RegID rid = rw->id;
            unsigned             rsz = rw->bitsize;
          
            if (rw->value.MakeConst()) {
              wb = *itr;
            }
          
            else {
              struct TmpVar : public armsec::ASExprNode
              {
                TmpVar( armsec::State::RegID rid, unsigned rsz )
                  : dsz(rsz)
                { std::ostringstream buffer; buffer << "nxt_" << rid.c_str() << "<" << rsz << ">"; ref = buffer.str(); }
                virtual int GenCode( Label& label, std::ostream& sink ) const { sink << ref; return dsz; }
                virtual intptr_t cmp( ExprNode const& rhs ) const { return ref.compare( dynamic_cast<TmpVar const&>( rhs ).ref ); }
                virtual ExprNode* GetConstNode() { return 0; };
                virtual void Traverse( Visitor& visitor ) {}
                virtual void Repr( std::ostream& sink ) const { sink << ref; }
                std::string ref;
                int dsz;
              }* tmp = new TmpVar( rid, rsz );
            
              std::ostringstream buffer;
              buffer << tmp->ref << " := " << GetCode(rw->value, current);
              Label insn( current );
              buffer << "; goto " << current.next();
              insn = buffer.str();
              ctx.add_var( rw->value, tmp );
            
              wb = new armsec::State::RegWrite( rid, tmp, rsz );
            }
          
            if (rid == armsec::State::RegID::nia)
              nia = dynamic_cast<armsec::State::RegWrite&>( *wb.node ).value;
            else
              ctx.add_pending( wb );
          }
        else
          {
            std::ostringstream buffer;
            int retsize = ASExprNode::GenerateCode( *itr, current, buffer );
            if (retsize) throw 0;
            Label insn( current );
            buffer << "; goto " << current.next();
            insn = buffer.str();
          }
      }
    
    if (not cond.good())
      {
        if (sinks.size() == 0)
          throw std::logic_error( "empty leaf" );
      }
    else
      {
        std::ostringstream buffer;
        buffer << "if " << GetCode(cond,current) << " ";
        Label ifinsn( current );
    
        current = after;
        if (nia.good() or (after.valid() and (ctx.has_pending())))
          current.next();
    
        if (not false_nxt) {
          Label ifthen(current);
          buffer << " goto " << ifthen.next() << " else goto " << current.id;
          ifinsn = buffer.str();
          true_nxt->GenCode( ifthen, current, &ctx );
        } else if (not true_nxt) {
          Label ifelse(current);
          buffer << " goto " << current.id << " else goto " << ifelse.next();
          ifinsn = buffer.str();
          false_nxt->GenCode( ifelse, current, &ctx );
        } else {
          Label ifthen(current), ifelse(current);
          buffer << " goto " << ifthen.next() << " else goto " << ifelse.next();
          ifinsn = buffer.str();
          true_nxt->GenCode( ifthen, current, &ctx );
          false_nxt->GenCode( ifelse, current, &ctx );
        }
      }
    
    for (Context::Pendings::iterator itr = ctx.pendings.begin(), end = ctx.pendings.end(); itr != end; ++itr)
      {
        std::ostringstream buffer;
        buffer << GetCode(*itr,current);
        Label insn( current );
        int next = (((itr+1) != end) or nia.good()) ? current.next() : after.id;
        buffer << "; goto " << next;
        insn = buffer.str();
      }
    
    if (not nia.good())
      return;
    
    for (Context* uc = ctx.upper; uc; uc = uc->upper)
      {
        for (Context::Pendings::iterator itr = uc->pendings.begin(), end = uc->pendings.end(); itr != end; ++itr)
          { 
            std::ostringstream buffer;
            buffer << GetCode(*itr,current);
            Label insn( current );
            buffer << "; goto " << current.next();
            insn = buffer.str();
          }
      }
    
    std::ostringstream buffer;
    buffer << "goto (" << GetCode(nia,current) << (nia.MakeConst() ? ",0" : "") << ")";
    current = buffer.str();
  }
}

struct ARMISA : public unisim::component::cxx::processor::arm::isa::arm32::Decoder<armsec::State>
{
  typedef unisim::component::cxx::processor::arm::isa::arm32::CodeType CodeType;
  typedef unisim::component::cxx::processor::arm::isa::arm32::Operation<armsec::State> Operation;
  static CodeType mkcode( uint32_t code ) { return CodeType( code ); }
  static bool const is_thumb = false;
};

struct THUMBISA : public unisim::component::cxx::processor::arm::isa::thumb2::Decoder<armsec::State>
{
  typedef unisim::component::cxx::processor::arm::isa::thumb2::CodeType CodeType;
  typedef unisim::component::cxx::processor::arm::isa::thumb2::Operation<armsec::State> Operation;
  static CodeType mkcode( uint32_t code ) { return CodeType( code ); }
  static bool const is_thumb = true;
};


struct Decoder
{
  ARMISA armisa;
  THUMBISA thumbisa;

  struct InstructionAddress : public armsec::ASExprNode
  {
    InstructionAddress() {}
    virtual void Repr( std::ostream& sink ) const { sink << "insn_addr"; }
    virtual intptr_t cmp( unisim::util::symbolic::ExprNode const& brhs ) const { dynamic_cast<InstructionAddress const&>( brhs ); return 0; }
    virtual unisim::util::symbolic::ExprNode* GetConstNode() { return 0; };
  };
  
  template <class ISA>
  void
  translate_isa( ISA& isa, uint32_t addr, uint32_t code )
  {
    std::cout << "(address . " << armsec::dbx( 32, addr ) << ")\n";
    
    struct Translation
    {
      typedef typename ISA::Operation Insn;
      typedef typename armsec::PathNode Code;
      typedef typename unisim::util::symbolic::Expr Expr;
      typedef typename armsec::State State;
      
      ~Translation() { delete insn; delete code; }
      Translation( uint32_t _addr )
        : insn(0)
        , code(new armsec::PathNode)
        , addr( _addr )
        , reference( code )
      {}
      
      int
      decode( ISA& isa, uint32_t _code )
      {
        try { insn = isa.NCDecode( addr, ISA::mkcode( _code ) ); }
        
        catch (unisim::component::cxx::processor::arm::Reject const&) { return 0; }
        
        return insn->GetLength();
      }
      
      void
      disasm( std::ostream& sink )
      {
        try { insn->disasm( reference, sink ); }
        
        catch (...) { sink << "(bad)"; }
      }
      
      bool
      Generate( armsec::Label::Program& program )
      {
        // Expr insn_addr( new InstructionAddress ); //< symbolic instruction address
        Expr insn_addr( unisim::util::symbolic::make_const( addr ) ); //< concrete instruction address
        
        reference.SetInsnProps( insn_addr, ISA::is_thumb, insn->GetLength() );
        
        try
          {
            for (bool end = false; not end;)
              {
                State state( code );
                state.SetInsnProps( insn_addr, ISA::is_thumb, insn->GetLength() );
                insn->execute( state );
                end = state.close( reference );
              }
          }
        catch (...)
          {
            return false;
          }
    
        code->factorize();
        code->remove_dead_paths();

        armsec::Label beglabel(program), endlabel(program);
        beglabel.next();
        code->GenCode( beglabel, endlabel, 0 );
        
        return true;
      }
      
      Insn*     insn;
      Code*     code;
      uint32_t  addr;
      State     reference;
    } trans( addr );
    
    unsigned bitlen = trans.decode( isa, code );
    if (bitlen != 16 and bitlen != 32)
      {
        std::cout << "(opcode . " << armsec::dbx( 32, code ) << ")\n(illegal)\n";
        return;
      }
    
    std::cout << "(opcode . " << armsec::dbx( bitlen, trans.insn->GetEncoding() ) << ")\n(size . " << (bitlen/8) << ")\n";
    
    // std::cout << "(int_name,\"" << trans.insn->GetName() << "\")\n";
    
    std::cout << "(mnemonic . \""; trans.disasm( std::cout ); std::cout << "\")\n";
    
    armsec::Label::Program program;
    if (trans.Generate( program ))
      {
        for (uintptr_t idx = 0; idx < program.size(); idx += 1)
          std::cout << '(' << armsec::dbx( 32, addr ) << ',' << idx << ") " << program[idx] << std::endl;
      }
    else
      {
        std::cout << "(unimplemented)\n";
      }
  }

  void  translate_arm( uint32_t addr, uint32_t code )   { translate_isa( armisa, addr, code ); }
  void  translate_thumb( uint32_t addr, uint32_t code ) { translate_isa( thumbisa, addr, code ); }
};

uint32_t getu32( uint32_t& res, char const* arg )
{
  char *end;
  uint32_t tmp = strtoul( arg, &end, 0 );
  if ((*arg == '\0') or (*end != '\0'))
    return false;
  res = tmp;
  return true;
}

char const* usage()
{
  return "usage: <prog> [arm|thumb] <addr> <code>\n";
}

int
main( int argc, char** argv )
{
  if (argc != 4)
    {
      std::cerr << "Wrong number of CLI arguments.\n" << usage();
      return 1;
    }
  
  uint32_t addr, code;
  
  if (not getu32(addr, argv[2]) or not getu32(code, argv[3]))
    {
      std::cerr << "<addr> and <code> whould be 32bits numeric values.\n" << usage();
      return 1;
    }
  
  Decoder decoder;
  std::string iset(argv[1]);
  if        (iset == std::string("arm")) {
    decoder.translate_arm( addr, code );
  } else if (iset == std::string("thumb")) {
    decoder.translate_thumb( addr, code );
  }
  
  return 0;
}
