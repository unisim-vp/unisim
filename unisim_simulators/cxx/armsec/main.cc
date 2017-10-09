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
    struct Program :  public std::map<int,std::string>
    {
      typedef std::map<int,std::string> MapType;
      typedef MapType::iterator iterator;
      typedef MapType::const_iterator const_iterator;


      Program() : insn_count(0) {}
      
      int next_insn() const { return insn_count; }
      int allocate() { return insn_count++; }
      iterator write(int idx, std::string const& s)
      {
        iterator itr = lower_bound(idx);
        if (itr != end() and itr->first == idx)
          throw std::runtime_error("overwriting statement");
        return insert( itr, std::make_pair(idx,s) );
      }
    
      int insn_count;
    };
    
    
    Label( Program& _program ) : program(_program), id(-1) {}
    
    Label& operator= (Label const& l)
    {
      if (&program != &l.program) throw std::runtime_error("label programs must aggree");
      id = l.id;
      return *this;
    }
    
    int allocate() { return (id = program.allocate()); }
    
    bool valid() const { return id >= 0; }
    
    static bool subst_next( std::string& s, int next )
    {
      uintptr_t pos = s.find( "<next>" );
      if (pos == std::string::npos)
        return false;
      { std::ostringstream buf; buf << next; s.replace(pos, 6, buf.str()); }
      return true;
    }
    
    int write( std::string const& src )
    {
      Program::iterator insn = program.write( id, src );
      if (subst_next(insn->second, program.next_insn()))
        id = program.allocate();
      return insn->first;
    }
    
    int GetID() const { return id; }
    
  private:
    Program& program;
    int id;
  };
  
  typedef std::map<unisim::util::symbolic::Expr,unisim::util::symbolic::Expr> Variables;
  
  struct ASExprNode : public unisim::util::symbolic::ExprNode
  {
    typedef unisim::util::symbolic::Expr Expr;
    virtual int GenCode( Label& label, Variables& vars, std::ostream& sink ) const = 0;
    static  int GenerateCode( Expr const& expr, Variables& vars, Label& label, std::ostream& sink );
  };

  struct GetCode
  {
    GetCode(ASExprNode::Expr const& _expr, Variables& _vars, Label& _label) : expr(_expr), vars(_vars), label(_label) {} ASExprNode::Expr const& expr; Variables& vars; Label& label;
    friend std::ostream& operator << ( std::ostream& sink, GetCode const& gc )
    {
      ASExprNode::GenerateCode( gc.expr, gc.vars, gc.label, sink );
      return sink;
    }
  };
  
  int
  ASExprNode::GenerateCode(Expr const& expr, Variables& vars, Label& label, std::ostream& sink)
  {
    using unisim::util::symbolic::ConstNodeBase;
    using unisim::util::symbolic::ScalarType;
    using unisim::util::symbolic::CastNodeBase;
    using unisim::util::symbolic::Op;
    using unisim::util::symbolic::OpNodeBase;
    using unisim::util::symbolic::BONode;
    using unisim::util::symbolic::UONode;
    
    /*** Pre expression process ***/
    Variables::iterator itr = vars.find( expr );
    if (itr != vars.end())
      {
        return static_cast<ASExprNode const*>( itr->second.node ) ->GenCode( label, vars, sink );
      }
    
    /*** Sub expression process ***/
    if (ConstNodeBase const* node = expr->GetConstNode())
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
    else if (OpNodeBase const* node = expr->AsOpNode())
      {
        switch (node->SubCount())
          {
          case 2: {
            int retsz = GenerateCode( node->GetSub(0), vars, label, sink << '(' );
        
        
            switch (node->op.code)
              {
              default:                sink << " [" << node->op.c_str() << "] "; break;
            
              case Op::Add:     sink << " + "; break;
              case Op::Sub:     sink << " - "; break;
              case Op::Mul:     sink << " * "; break;
        
              case Op::Xor:     sink << " xor "; break;
              case Op::Or:      sink << " or "; break;
              case Op::And:     sink << " and "; break;
        
              case Op::Teq:     sink << " = "; retsz = 1; break;
              case Op::Tne:     sink << " <> "; retsz = 1; break;
        
              case Op::Tle:     sink << " <=s "; retsz = 1; break;
              case Op::Tleu:    sink << " <=u "; retsz = 1; break;
        
              case Op::Tge:     sink << " >=s "; retsz = 1; break;
              case Op::Tgeu:    sink << " >=u "; retsz = 1; break;
        
              case Op::Tlt:     sink << " <s "; retsz = 1; break;
              case Op::Tltu:    sink << " <u "; retsz = 1; break;
        
              case Op::Tgt:     sink << " >s "; retsz = 1; break;
              case Op::Tgtu:    sink << " >u "; retsz = 1; break;
      
              case Op::Lsl:     sink << " lshift "; break;
              case Op::Asr:     sink << " rshifts "; break;
              case Op::Lsr:     sink << " rshiftu "; break;
              case Op::Ror:     sink << " rrotate "; break;
                // case Op::Mod: break;
                // case Op::Div: break;
              }
          
            sink << GetCode( node->GetSub(1), vars, label ) << ')';
            return retsz;
          }
            
          case 1: {
            char const* operation = 0;
        
            switch (node->op.code)
              {
              default:              operation = node->op.c_str(); break;
        
              case Op::Not:    operation = "not "; break;
              case Op::Neg:    operation = "- "; break;
        
                // case Op::BSwp:  break;
                // case Op::BSF:   break;
            
              case Op::BSR:
                {
                  Label head(label);
                  int exit = label.allocate(), loop; 
                  {
                    std::ostringstream buffer;
                    buffer << "bsr_in<32> := " << GetCode(node->GetSub(0), vars, head) << " ; goto <next>";
                    head.write( buffer.str() );
                    head.write( "bsr_out<32> := 32<32> ; goto <next>" );
                  }
                  {
                    std::ostringstream buffer;
                    buffer << "if (bsr_in<32> = 0<32>) goto " << exit << " else goto <next>";
                    head.write( buffer.str() );
                    loop = head.write( "bsr_out<32> := bsr_out<32> - 1<32> ; goto <next>" );
                  }
                  {
                    std::ostringstream buffer;
                    buffer << "if ((bsr_in<32> rshiftu bsr_out<32>){0,0}) goto " << exit << " else goto " << loop;
                    head.write( buffer.str() );
                  }
      
                  sink << "bsr_out<32>";
      
                  return 32;
                }
              case Op::Cast:
                {
                  CastNodeBase const& cnb = dynamic_cast<CastNodeBase const&>( *expr.node );
                  ScalarType src( cnb.GetSrcType() ), dst( cnb.GetDstType() );
                  
                  if (dst.is_integer and src.is_integer)
                    {
                      int extend = dst.bitsize - src.bitsize;
                      if      (extend > 0)
                        {
                          sink << '(' << (src.is_signed ? "exts " : "extu ") << GetCode(cnb.src, vars, label) << ' ' << dst.bitsize << ')';
                        }
                      else if (extend < 0)
                        {
                          CastNodeBase const* src_node = dynamic_cast<CastNodeBase const*>( cnb.src.node );
                          if (src_node and (src_node->GetSrcType() == cnb.GetDstType())) {
                            sink << GetCode(src_node->src, vars, label);
                          } else  if (dst.bitsize == 1) {
                            sink << "(" << GetCode(cnb.src, vars, label) << " <> " << dbx(src.bitsize,0) << ")";
                          } else {
                            sink << "(" << GetCode(cnb.src, vars, label) << " {0," << (dst.bitsize-1) << "})";
                          }
                        }
                      else
                        sink << GetCode(cnb.src, vars, label);
                    }
                  else
                    {
                      sink << dst.name << "( " << GetCode(cnb.src, vars, label) << " )";
                    }
      
                  return dst.bitsize;
                }
              }
    
            sink << '(' << operation << ' ';
            int retsz = GenerateCode( node->GetSub(0), vars, label, sink );
            sink << ')';
            return retsz;
          }
          default:
            break;
          }
      }
    else if (ASExprNode const* node = dynamic_cast<ASExprNode const*>( expr.node ))
      {
        return node->GenCode( label, vars, sink );
      }
      
    throw std::logic_error("No GenCode method");
    return 0;
  }

  // virtual int DefaultNaNNode::GenCode( Label& label, Variables& vars, std::ostream& sink ) const
  //     { sink << "F" << fsz << "DefaultNaN()"; return fsz; }
  
  // virtual int FtoINode::GenCode( Label& label, Variables& vars, std::ostream& sink ) const
  //     {
  //       sink << "F" << fsz << "2I" << isz << "( " << GetCode(src,label) << ", " << fb << " )";
  //       return isz;
  //     }
  
  // virtual int ItoFNode::GenCode( Label& label, Variables& vars, std::ostream& sink ) const
  //     { sink << "I" << isz << "2F" << fsz << "( " << GetCode(src,label) << ", " << fb << " )"; return fsz; }
      
  // virtual int FtoFNode::GenCode( Label& label, Variables& vars, std::ostream& sink ) const
  //     {
  //       sink << "F" << ssz << "2F" << dsz << "( " << GetCode(src,label) << " )";
  //       return dsz;
  //     }
  
  // virtual int IsInvalidMulAddNode::GenCode( Label& label, Variables& vars, std::ostream& sink ) const
  //     {
  //       sink << "FIsInvalidMulAdd( " << GetCode(acc,label)
  //            << ", " << GetCode(left,label)
  //            << ", " << GetCode(right,label) << " )";
  //       return 1;
  //     }

  // virtual int MulAddNode::GenCode( Label& label, Variables& vars, std::ostream& sink ) const
  //     {
  //       sink << "FMulAdd( ";
  //       int retsize = 0;
  //       sink << GetCode(acc, label, retsize ) << ", " << GetCode(left,label) << ", " << GetCode(right,label) << " )";
  //       return retsize;
  //     }
      
  // virtual int IsNaNNode::GenCode( Label& label, Variables& vars, std::ostream& sink ) const
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
        else
          cond = Expr();
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
      if (UONode const* uon = dynamic_cast<UONode const*>( cond.node ))
        if (uon->op.cmp( unisim::util::symbolic::Op("Not") ) == 0) {
          cond = uon->GetSub(0);
          std::swap( false_nxt, true_nxt );
        }
    }
  
    struct Context
    {
      Context() : upper(0) {}
      Context( Context* _up ) : upper(_up)
      {
        if (_up) next_tmp = _up->next_tmp;
        if (_up) vars = _up->vars;
      }
      
      void add_pending( Expr e ) { pendings.push_back(e); }
      bool has_pending() const { return pendings.size() > 0; }
      
      struct TmpVar : public armsec::ASExprNode
      {
        TmpVar( std::string const& _ref, unsigned rsz )
          : ref(_ref), dsz(rsz)
        {}
        virtual int GenCode( Label& label, Variables& vars, std::ostream& sink ) const { sink << ref; return dsz; }
        virtual intptr_t cmp( ExprNode const& rhs ) const { return ref.compare( dynamic_cast<TmpVar const&>( rhs ).ref ); }
        virtual unsigned SubCount() const { return 0; }
        virtual void Repr( std::ostream& sink ) const { sink << ref; }
        std::string ref;
        int dsz;
      };
      
      Expr const&
      addvar( std::string const& name, unsigned size, Expr const& expr )
      {
        Expr& place = vars[expr];
        if (place.good())
          throw std::logic_error( "multiple temporary definitions" );
        place = new TmpVar( name, size );
        return place;
      }
      
      std::string nxtname( char const* rname, unsigned size ) { std::ostringstream buffer; buffer << "nxt_" << rname << "<" << size << ">"; return buffer.str(); }
      
      std::string tmpname( unsigned size ) { std::ostringstream buffer; buffer << "tmp" << size << '_' << (next_tmp[size]++) << "<" << size << ">"; return buffer.str(); }
      
      Context* upper;
      typedef std::vector<Expr> Pendings;
      Pendings pendings;
      Variables vars;
      std::map<unsigned,unsigned> next_tmp;
    };

    void GenCode( Label const&, Label const&, Context* ) const;
    
    Expr cond;
    std::set<Expr> sinks;
    PathNode* previous;
    PathNode* true_nxt;
    PathNode* false_nxt;
    bool complete;
  };
  
#define CP15ENCODE( CRN, OPC1, CRM, OPC2 ) ((OPC1 << 12) | (CRN << 8) | (CRM << 4) | (OPC2 << 0))

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
    bool Cond( unisim::util::symbolic::SmartValue<T> const& cond )
    {
      if (not cond.expr.good())
        throw std::logic_error( "Not a valid condition" );
      
      Expr cexp( BOOL(cond).expr );
      if (unisim::util::symbolic::ConstNodeBase const* cnode = cexp.ConstSimplify())
        return cnode->GetBoolean();
      
      bool result;
      path = path->proceed( cexp, result );
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
          NA = 0,
          r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, sp, lr, nia,
          n, z, c, v, itstate, // q, ge0, ge1, ge2, ge3,
          cpsr, spsr,
          fpscr, fpexc,
          SCTLR, ACTLR,
          CTR, MPIDR,
          ID_PFR0, CCSIDR, CLIDR, CSSELR,
          CPACR, NSACR,
          TTBR0, TTBR1, TTBCR,
          DACR,
          DFSR, IFSR, DFAR, IFAR,
          ICIALLUIS, BPIALLIS,
          ICIALLU, ICIMVAU, BPIALL,
          DCIMVAC, DCISW, DCCMVAC, DCCSW, DCCMVAU, DCCIMVAC,
          TLBIALLIS, TLBIALL, TLBIASID,
          VBAR,
          CONTEXTIDR,
          DIAGCR, CFGBAR,
          end
        } code;

      static RegID sreg_id(int idx) { return RegID(SCTLR) + idx; }
      int sreg_idx() { return code - SCTLR; }
      
      static int  const sregs_count = end - SCTLR;
      
      char const* c_str() const
      {
        switch (code)
          {
          case         r0: return "r0";
          case         r1: return "r1";
          case         r2: return "r2";
          case         r3: return "r3";
          case         r4: return "r4";
          case         r5: return "r5";
          case         r6: return "r6";
          case         r7: return "r7";
          case         r8: return "r8";
          case         r9: return "r9";
          case         sl: return "sl";
          case         fp: return "fp";
          case         ip: return "ip";
          case         sp: return "sp";
          case         lr: return "lr";
          case        nia: return "pc";
          case          n: return "n";
          case          z: return "z";
          case          c: return "c";
          case          v: return "v";
          case    itstate: return "itstate";
          case       cpsr: return "cpsr";
          case       spsr: return "spsr";
          case      fpscr: return "fpscr";
          case      fpexc: return "fpexc";
          case      SCTLR: return "sctlr";
          case      ACTLR: return "actlr";
          case        CTR: return "ctr";
          case      MPIDR: return "mpidr";
          case    ID_PFR0: return "id_pfr0";
          case     CCSIDR: return "ccsidr";
          case      CLIDR: return "clidr";
          case     CSSELR: return "csselr";
          case      CPACR: return "cpacr";
          case      NSACR: return "nsacr";
          case      TTBR0: return "ttbr0";
          case      TTBR1: return "ttbr1";
          case      TTBCR: return "ttbcr";
          case       DACR: return "dacr";
          case       DFSR: return "dfsr";
          case       IFSR: return "ifsr";
          case       DFAR: return "dfar";
          case       IFAR: return "ifar";
          case  ICIALLUIS: return "icialluis";
          case   BPIALLIS: return "bpiallis";
          case    ICIALLU: return "iciallu";
          case    ICIMVAU: return "icimvau";
          case     BPIALL: return "bpiall";
          case    DCIMVAC: return "dcimvac";
          case      DCISW: return "dcisw";
          case    DCCMVAC: return "dccmvac";
          case      DCCSW: return "dccsw";
          case    DCCMVAU: return "dccmvau";
          case   DCCIMVAC: return "dccimvac";
          case  TLBIALLIS: return "tlbiallis";
          case    TLBIALL: return "tlbiall";
          case   TLBIASID: return "tlbiasid";
          case       VBAR: return "vbar";
          case CONTEXTIDR: return "contextidr";
          case     DIAGCR: return "diagcr";
          case     CFGBAR: return "cfgbar";
          case         NA: return "NA";
          case        end: break;
          }
        return "INVALID";
      }
      
      
      RegID( Code _code ) : code(_code) {}
      RegID( char const* _code ) : code(NA) { unisim::util::symbolic::cstr2enum( *this, _code ); }
      
      RegID operator + ( int offset ) const { return RegID( Code(int(code) + offset) ); }
      //int operator - ( RegID rid ) const { return int(code) - int(rid.code); }
      //bool operator == ( RegID rid ) const { return code == rid.code; }
      intptr_t cmp( RegID rhs ) const { return intptr_t(code) - intptr_t(rhs.code); }
    };
    
    struct RegRead : public ASExprNode
    {
      RegRead( RegID _id, unsigned _bitsize ) : id(_id), bitsize(_bitsize) {}
      RegRead( char const* name, unsigned _bitsize ) : id( name ), bitsize(_bitsize) {}
      virtual int GenCode( Label& label, Variables& vars, std::ostream& sink ) const { sink << id.c_str() << "<" << bitsize << ">"; return bitsize; }
      virtual intptr_t cmp( ExprNode const& brhs ) const { return id.cmp( dynamic_cast<RegRead const&>( brhs ).id ); }
      
      virtual unsigned SubCount() const { return 0; }
      virtual void Repr( std::ostream& sink ) const { sink << id.c_str(); }
      RegID id;
      unsigned bitsize;
    };
    
    struct RegWrite : public ASExprNode
    {
      RegWrite( RegID _id, Expr const& _value, unsigned _bitsize ) : id(_id), value(_value), bitsize(_bitsize) {}
      RegWrite( char const* name, Expr const& _value, unsigned _bitsize ) : id(name), value(_value), bitsize(_bitsize) {}
      virtual int GenCode( Label& label, Variables& vars, std::ostream& sink ) const
      {
        sink << id.c_str() << "<" << std::dec << bitsize << "> := " << GetCode(value, vars, label);
        return 0;
      }
      
      virtual intptr_t cmp( ExprNode const& brhs ) const
      {
        RegWrite const& rhs = dynamic_cast<RegWrite const&>( brhs );
        if (intptr_t delta = id.cmp( rhs.id )) return delta;
        return value.cmp( rhs.value );
      }
      
      virtual unsigned SubCount() const { return 1; }
      virtual Expr const& GetSub(unsigned idx) const { if (idx != 0) return ExprNode::GetSub(idx); return value; }
      virtual void Repr( std::ostream& sink ) const { sink << id.c_str() << " := "; value->Repr(sink); }
      
      RegID id;
      Expr value;
      unsigned bitsize;
    };
    
    struct AssertFalse : public ASExprNode
    {
      AssertFalse() {}
      virtual int GenCode( Label& label, Variables& vars, std::ostream& sink ) const
      {
        sink << "assert (false)";
        return 0;
      }

      virtual intptr_t cmp( ExprNode const& brhs ) const { return 0; }
      virtual unsigned SubCount() const { return 0; }
      virtual void Repr( std::ostream& sink ) const { sink << "assert (false)"; }
    };
    
    State( PathNode* _path )
      : path( _path )
      , next_insn_addr()
      , cpsr( Expr( new RegRead("n",1) ),
              Expr( new RegRead("z",1) ),
              Expr( new RegRead("c",1) ),
              Expr( new RegRead("v",1) ),
              Expr( new RegRead("itstate",8) ),
              Expr( new RegRead("cpsr",32) ) )
      , spsr( Expr( new RegRead("spsr",32) ) )
      , FPSCR( Expr( new RegRead("fpscr",32) ) )
      , FPEXC( Expr( new RegRead("fpexc",32) ) )
      , unpredictable(false)
      , is_it_assigned(false)
    {
      for (unsigned reg = 0; reg < 15; ++reg)
        reg_values[reg] = U32( Expr( new RegRead( RegID("r0") + reg, 32 ) ) );
      for (int idx = 0; idx < RegID::sregs_count; ++idx)
        sregs[idx] = U32( Expr( new RegRead( RegID::sreg_id(idx), 32 ) ) );
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
      psr_type( Expr const& _n, Expr const& _z, Expr const& _c, Expr const& _v, Expr const& _itstate, Expr const& _bg )
        : n(_n), z(_z), c(_c), v(_v), itstate(_itstate), bg(_bg)
      {}
      Expr n, z, c, v;
      U8 itstate;
      U32 bg;
      
      typedef unisim::component::cxx::processor::arm::RegisterField<31,1> NRF; /* Negative Integer Condition Flag */
      typedef unisim::component::cxx::processor::arm::RegisterField<30,1> ZRF; /* Zero     Integer Condition Flag */
      typedef unisim::component::cxx::processor::arm::RegisterField<29,1> CRF; /* Carry    Integer Condition Flag */
      typedef unisim::component::cxx::processor::arm::RegisterField<28,1> VRF; /* Overflow Integer Condition Flag */
      
      typedef unisim::component::cxx::processor::arm::RegisterField<28,4> NZCVRF; /* Grouped Integer Condition Flags */
      
      typedef unisim::component::cxx::processor::arm::RegisterField<27,1> QRF; /* Cumulative saturation flag */
      
      typedef unisim::component::cxx::processor::arm::RegisterField< 9,1> ERF; /* Endianness execution state */
      typedef unisim::component::cxx::processor::arm::RegisterField< 0,5> MRF; /* Mode field */
      
      typedef unisim::component::cxx::processor::arm::RegisterField<10,6> ITHIRF;
      typedef unisim::component::cxx::processor::arm::RegisterField<25,2> ITLORF;
      
      typedef unisim::component::cxx::processor::arm::RegisterField< 0,32> ALL;
      
      template <typename RF>
      void Set( RF const& _, U32 const& value )
      {
        unisim::util::symbolic::StaticAssert<(RF::pos > 31) or ((RF::pos + RF::size) <= 28)>::check(); // NZCV
        unisim::util::symbolic::StaticAssert<(RF::pos > 26) or ((RF::pos + RF::size) <= 25)>::check(); // ITLO
        unisim::util::symbolic::StaticAssert<(RF::pos > 15) or ((RF::pos + RF::size) <= 10)>::check(); // ITHI
        
        return _.Set( bg, value );
      }
      
      void Set( NRF const& _, BOOL const& value ) { n = value.expr; }
      void Set( ZRF const& _, BOOL const& value ) { z = value.expr; }
      void Set( CRF const& _, BOOL const& value ) { c = value.expr; }
      void Set( VRF const& _, BOOL const& value ) { v = value.expr; }

      //void Set( QRF const& _, U32 const& value ) { unisim::util::symbolic::StaticAssert<false>::check();_.Set( bg, value ); }
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
        
        itstate = U8((ITHIRF().Get( value ) << 2) | ITLORF().Get( value ));
        
        unisim::component::cxx::processor::arm::RegisterField<0,28>().Set( bg, value );
      }
      
      void
      SetITState( uint8_t init_val )
      {
        itstate = U8(init_val);
      }
      
      BOOL InITBlock() const { return (itstate & U8(0b1111)) != U8(0); }
      
      template <typename RF>
      U32 Get( RF const& _ )
      {
        unisim::util::symbolic::StaticAssert<(RF::pos > 31) or ((RF::pos + RF::size) <= 28)>::check(); // NZCV
        unisim::util::symbolic::StaticAssert<(RF::pos > 26) or ((RF::pos + RF::size) <= 25)>::check(); // ITLO
        unisim::util::symbolic::StaticAssert<(RF::pos > 15) or ((RF::pos + RF::size) <= 10)>::check(); // ITHI
        
        return _.Get( bg );
      }
      
      U32 Get( NRF const& _ ) { return U32(BOOL(n)); }
      U32 Get( ZRF const& _ ) { return U32(BOOL(z)); }
      U32 Get( CRF const& _ ) { return U32(BOOL(c)); }
      U32 Get( VRF const& _ ) { return U32(BOOL(v)); }

      U32 Get( MRF const& _ ) { return U32(0b11111); /*SYSTEM_MODE*/ }
      // U32 Get( ALL const& _ ) { return (U32(BOOL(n)) << 31) | (U32(BOOL(z)) << 30) | (U32(BOOL(c)) << 29) | (U32(BOOL(v)) << 28) | bg; }
      
      U32 bits() const
      {
        return
          (U32(BOOL(n)) << 31) | (U32(BOOL(z)) << 30) | (U32(BOOL(c)) << 29) | (U32(BOOL(v)) << 28) |
          (U32(itstate >> U8(2)) << 10) | (U32(itstate & U8(0b11)) << 25) |
          bg;
      }
    } cpsr;
    
    U32 spsr;
    
    U32 sregs[RegID::sregs_count];
    
    U32& SReg( RegID rid )
    {
      if (rid.code == RegID::NA)
        throw 0;
      return sregs[rid.sreg_idx()];
    }
    
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
      
      virtual unsigned SubCount() const { return 1; }
      virtual Expr const& GetSub(unsigned idx) const { if (idx != 0) return ExprNode::GetSub(idx); return addr; }
      virtual void Repr( std::ostream& sink ) const { sink << "["; addr->Repr( sink ); sink << "," << size << "," << (aligned?'a':'u') << "]"; }
      virtual int GenCode( Label& label, Variables& vars, std::ostream& sink ) const
      {
        /* TODO: dont assume little endianness */
        /* TODO: exploit alignment info */
        sink << "@[" << GetCode(addr, vars, label) << ",<-," << size << "]";
        return 8*size;
      }
      intptr_t cmp( ExprNode const& brhs ) const
      {
        Load const& rhs = dynamic_cast<Load const&>( brhs );
        if (intptr_t delta = addr.cmp( rhs.addr )) return delta;
        if (intptr_t delta = int(size - rhs.size)) return delta;
        return (int(aligned) - int(rhs.aligned));
      }
      
      Expr addr;
      unsigned size;
      bool aligned;
    };
    
    struct Store : public ASExprNode
    {
      Store( unsigned _size, bool _aligned, Expr const& _addr, Expr const& _value )
        : value(_value), addr(_addr), size(_size), aligned(_aligned)
      {}
      
      virtual int GenCode( Label& label, Variables& vars, std::ostream& sink ) const
      {
        /* TODO: dont assume little endianness */
        /* TODO: exploit alignment info */
        sink << "@[" << GetCode(addr, vars, label) << ",<-," << size << "] := " << GetCode(value, vars, label);
        return 0;
      }
      
      virtual unsigned SubCount() const { return 2; }
      virtual Expr const& GetSub(unsigned idx) const { switch (idx) { case 0: return addr; case 1: return value; } return ExprNode::GetSub(idx); }
      virtual void Repr( std::ostream& sink ) const { sink << "["; addr->Repr( sink ); sink << "," << size << "," << (aligned?'a':'u') << "] := "; value->Repr(sink); }
      
      intptr_t cmp( ExprNode const& brhs ) const
      {
        Store const& rhs = dynamic_cast<Store const&>( brhs );
        if (intptr_t delta = value.cmp( rhs.value )) return delta;
        if (intptr_t delta = addr.cmp( rhs.addr )) return delta;
        if (intptr_t delta = int(size - rhs.size)) return delta;
        return (int(aligned) - int(rhs.aligned));
      }
      
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
    
    void SetExclusiveMonitors( U32 const& address, unsigned size ) { std::cerr << "SetExclusiveMonitors\n"; }
    bool ExclusiveMonitorsPass( U32 const& address, unsigned size ) { std::cerr << "ExclusiveMonitorsPass\n"; return true; }
    void ClearExclusiveLocal() { std::cerr << "ClearExclusiveMonitors\n"; }
    
    void BranchExchange( U32 const& target ) { next_insn_addr = target; }
    void Branch( U32 const& target ) { next_insn_addr = target; }
    
    void WaitForInterrupt() { not_implemented(); }
    void SWI( uint32_t imm ) { not_implemented(); }
    void BKPT( uint32_t imm ) { not_implemented(); }
    bool unpredictable;
    void UnpredictableInsnBehaviour() { unpredictable = true; }
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
    
    struct ITCond {};
    
    ITCond itcond() const { return ITCond(); }
    bool itblock() { return Cond(cpsr.InITBlock()); }
    bool is_it_assigned; /* determines wether current instruction is an IT one. */
    void ITSetState( uint32_t cond, uint32_t mask )
    {
      cpsr.SetITState( (cond << 4) | mask );
      is_it_assigned = true;
    }
    void
    ITAdvance()
    {
      if (is_it_assigned)
        is_it_assigned = false;
      else if (itblock())
        {
          U8 itstate( cpsr.itstate );
          itstate = (Cond((itstate & U8(7)) != U8(0))) ? ((itstate & U8(-32)) | ((itstate << 1) & U8(31))) : U8(0);
          cpsr.itstate = itstate;
        }
    }

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
    
    Mode&  CurrentMode() { // not_implemented(); 
      return mode; }
    Mode&  GetMode(uint8_t) { not_implemented(); return mode; }
    U32  GetBankedRegister( uint8_t foreign_mode, uint32_t idx ) { not_implemented(); return U32(); }
    void SetBankedRegister( uint8_t foreign_mode, uint32_t idx, U32 value ) { not_implemented(); }
    
    
    struct CP15Reg
    {
      virtual            ~CP15Reg() {}
      virtual unsigned    RequiredPL() { return 1; }
      virtual void        Write( State& cpu, U32 const& value ) { cpu.not_implemented(); }
      virtual U32         Read( State& cpu ) { cpu.not_implemented(); return U32(); }
      virtual char const* Describe() = 0;
    };

    virtual CP15Reg& CP15GetRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 )
    {
      switch (CP15ENCODE( crn, opcode1, crm, opcode2 ))
        {
          /****************************
           * Identification registers *
           ****************************/
        case CP15ENCODE( 0, 0, 0, 1 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "CTR, Cache Type Register"; }
              U32 Read( State& cpu ) { return cpu.SReg("ctr"); }
            } x;
            return x;
          } break;
          
        case CP15ENCODE( 0, 0, 0, 5 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "MPIDR, Multiprocessor Affinity Register"; }
              U32 Read( State& cpu ) { return cpu.SReg("mpidr"); }
            } x;
            return x;
          } break;
          
        case CP15ENCODE( 0, 0, 1, 0 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "ID_PFR0, Processor Feature Register 0"; }
              U32 Read( State& cpu ) { return cpu.SReg("id_pfr0"); }
            } x;
            return x;
          } break;
          
        case CP15ENCODE( 0, 1, 0, 0 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "CCSIDR, Cache Size ID Registers"; }
              U32 Read( State& cpu ) { return cpu.SReg("ccsidr"); }
            } x;
            return x;
          } break;

        case CP15ENCODE( 0, 1, 0, 1 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "CLIDR, Cache Level ID Register"; }
              U32 Read( State& cpu ) { return cpu.SReg("clidr"); }
            } x;
            return x;
          } break;

        case CP15ENCODE( 0, 2, 0, 0 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "CSSELR, Cache Size Selection Register"; }
              U32 Read( State& cpu ) { return cpu.SReg("csselr"); }
              void Write( State& cpu, U32 const& value ) { cpu.SReg("csselr") = value; }
            } x;
            return x;
          } break;
      
          /****************************
           * System control registers *
           ****************************/
        case CP15ENCODE( 1, 0, 0, 0 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "SCTLR, System Control Register"; }
              U32 Read( State& cpu ) { return cpu.SReg("sctlr"); }
              void Write( State& cpu, U32 const& value ) { cpu.SReg("sctlr") = value; }
            } x;
            return x;
          } break;

        case CP15ENCODE( 1, 0, 0, 1 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "ACTLR, Auxiliary Control Register"; }
              U32 Read( State& cpu ) { return cpu.SReg("actlr"); }
              void Write( State& cpu, U32 const& value ) { cpu.SReg("actlr") = value; }
            } x;
            return x;
          } break;

        case CP15ENCODE( 1, 0, 0, 2 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "CPACR, Coprocessor Access Control Register"; }
              U32 Read( State& cpu ) { return cpu.SReg("cpacr"); }
              void Write( State& cpu, U32 const& value ) { cpu.SReg("cpacr") = value; }
            } x;
            return x;
          } break;

        case CP15ENCODE( 1, 0, 1, 2 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "NSACR, Non-Secure Access Control Register"; }
              U32 Read( State& cpu ) { return cpu.SReg("nsacr"); }
              void Write( State& cpu, U32 const& value ) { cpu.SReg("nsacr") = value; }
            } x;
            return x;
          } break;

      /*******************************************
       * Memory protection and control registers *
       *******************************************/
        case CP15ENCODE( 2, 0, 0, 0 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "TTBR0, Translation Table Base Register 0"; }
              void Write( State& cpu, U32 const& value ) { cpu.SReg("ttbr0") = value; }
              U32 Read( State& cpu ) { return cpu.SReg("ttbr0"); }
            } x;
            return x;
          } break;

        case CP15ENCODE( 2, 0, 0, 1 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "TTBR1, Translation Table Base Register 1"; }
              void Write( State& cpu, U32 const& value ) { cpu.SReg("ttbr1") = value; }
              U32 Read( State& cpu ) { return cpu.SReg("ttbr1"); }
            } x;
            return x;
          } break;

        case CP15ENCODE( 2, 0, 0, 2 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "TTBCR, Translation Table Base Control Register"; }
              void Write( State& cpu, U32 const& value ) { cpu.SReg("ttbcr") = value; }
              U32 Read( State& cpu ) { return cpu.SReg("ttbcr"); }
            } x;
            return x;
          } break;

        case CP15ENCODE( 3, 0, 0, 0 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "DACR, Domain Access Control Register"; }
              U32 Read( State& cpu ) { return cpu.SReg("dacr"); }
              void Write( State& cpu, U32 const& value ) { cpu.SReg("dacr") = value; }
            } x;
            return x;
          } break;


          /*********************************
           * Memory system fault registers *
           *********************************/
        case CP15ENCODE( 5, 0, 0, 0 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "DFSR, Data Fault Status Register"; }
              U32 Read( State& cpu ) { return cpu.SReg("dfsr"); }
              void Write( State& cpu, U32 const& value ) { cpu.SReg("dfsr") = value; }
            } x;
            return x;
          } break;

        case CP15ENCODE( 5, 0, 0, 1 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "IFSR, Instruction Fault Status Register"; }
              U32 Read( State& cpu ) { return cpu.SReg("ifsr"); }
              void Write( State& cpu, U32 const& value ) { cpu.SReg("ifsr") = value; }
            } x;
            return x;
          } break;

        case CP15ENCODE( 6, 0, 0, 0 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "DFAR, Data Fault Status Register"; }
              U32 Read( State& cpu ) { return cpu.SReg("dfar"); }
              void Write( State& cpu, U32 const& value ) { cpu.SReg("dfar") = value; }
            } x;
            return x;
          } break;

        case CP15ENCODE( 6, 0, 0, 2 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "IFAR, Instruction Fault Status Register"; }
              U32 Read( State& cpu ) { return cpu.SReg("ifar"); }
              void Write( State& cpu, U32 const& value ) { cpu.SReg("ifar") = value; }
            } x;
            return x;
          } break;

          /***************************************************************
           * Cache maintenance, address translation, and other functions *
           ***************************************************************/
          
        case CP15ENCODE( 7, 0, 1, 0 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "ICIALLUIS, Invalidate all instruction caches to PoU Inner Shareable"; }
              U32 Read( State& cpu ) { return cpu.SReg("icialluis"); }
              void Write( State& cpu, U32 const& value ) { cpu.SReg("icialluis") = value; }
            } x;
            return x;
          } break;

        case CP15ENCODE( 7, 0, 1, 6 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "BPIALLIS, Invalidate all branch predictors Inner Shareable"; }
              U32 Read( State& cpu ) { return cpu.SReg("bpiallis"); }
              void Write( State& cpu, U32 const& value ) { cpu.SReg("bpiallis") = value; }
            } x;
            return x;
          } break;

        case CP15ENCODE( 7, 0, 5, 0 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "ICIALLU, Invalidate all instruction caches to PoU"; }
              U32 Read( State& cpu ) { return cpu.SReg("iciallu"); }
              void Write( State& cpu, U32 const& value ) { cpu.SReg("iciallu") = value; }
            } x;
            return x;
          } break;

        case CP15ENCODE( 7, 0, 5, 1 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "ICIMVAU, Clean data* cache line by MVA to PoU"; }
              U32 Read( State& cpu ) { return cpu.SReg("icimvau"); }
              void Write( State& cpu, U32 const& value ) { cpu.SReg("icimvau") = value; }
            } x;
            return x;
          } break;

        case CP15ENCODE( 7, 0, 5, 6 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "BPIALL, Invalidate all branch predictors"; }
              U32 Read( State& cpu ) { return cpu.SReg("bpiall"); }
              void Write( State& cpu, U32 const& value ) { cpu.SReg("bpiall") = value; }
            } x;
            return x;
          } break;

        case CP15ENCODE( 7, 0, 6, 1 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "DCIMVAC, Invalidate data* cache line by MVA to PoC"; }
              U32 Read( State& cpu ) { return cpu.SReg("dcimvac"); }
              void Write( State& cpu, U32 const& value ) { cpu.SReg("dcimvac") = value; }
            } x;
            return x;
          } break;

        case CP15ENCODE( 7, 0, 6, 2 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "DCISW, Invalidate data* cache line by set/way"; }
              U32 Read( State& cpu ) { return cpu.SReg("dcisw"); }
              void Write( State& cpu, U32 const& value ) { cpu.SReg("dcisw") = value; }
            } x;
            return x;
          } break;
          
        case CP15ENCODE( 7, 0, 10, 1 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "DCCMVAC, Clean data* cache line by MVA to PoC"; }
              U32 Read( State& cpu ) { return cpu.SReg("dccmvac"); }
              void Write( State& cpu, U32 const& value ) { cpu.SReg("dccmvac") = value; }
            } x;
            return x;
          } break;

        case CP15ENCODE( 7, 0, 10, 2 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "DCCSW, Clean data* cache line by set/way"; }
              U32 Read( State& cpu ) { return cpu.SReg("dccsw"); }
              void Write( State& cpu, U32 const& value ) { cpu.SReg("dccsw") = value; }
            } x;
            return x;
          } break;
          
        case CP15ENCODE( 7, 0, 11, 1 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "DCCMVAU, Clean data* cache line by MVA to PoU"; }
              U32 Read( State& cpu ) { return cpu.SReg("dccmvau"); }
              void Write( State& cpu, U32 const& value ) { cpu.SReg("dccmvau") = value; }
            } x;
            return x;
          } break;

        case CP15ENCODE( 7, 0, 14, 1 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "DCCIMVAC, Clean and invalidate data* cache line by MVA to PoC"; }
              U32 Read( State& cpu ) { return cpu.SReg("dccimvac"); }
              void Write( State& cpu, U32 const& value ) { cpu.SReg("dccimvac") = value; }
            } x;
            return x;
          } break;
          
          /******************************
           * TLB maintenance operations *
           ******************************/

        case CP15ENCODE( 8, 0, 3, 0 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "TLBIALLIS, Invalidate entire TLB Inner Shareable"; }
              U32 Read( State& cpu ) { return cpu.SReg("tlbiallis"); }
              void Write( State& cpu, U32 const& value ) { cpu.SReg("tlbiallis") = value; }
            } x;
            return x;
          } break;

        case CP15ENCODE( 8, 0, 7, 0 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "TLBIALL, invalidate unified TLB"; }
              U32 Read( State& cpu ) { return cpu.SReg("tlbiall"); }
              void Write( State& cpu, U32 const& value ) { cpu.SReg("tlbiall") = value; }
            } x;
            return x;
          } break;

        case CP15ENCODE( 8, 0, 7, 2 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "TLBIASID, invalidate unified TLB by ASID match"; }
              U32 Read( State& cpu ) { return cpu.SReg("tlbiasid"); }
              void Write( State& cpu, U32 const& value ) { cpu.SReg("tlbiasid") = value; }
            } x;
            return x;
          } break;
          
        case CP15ENCODE( 12, 0, 0, 0 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "VBAR, Vector Base Address Register"; }
              U32 Read( State& cpu ) { return cpu.SReg("vbar"); }
              void Write( State& cpu, U32 const& value ) { cpu.SReg("vbar") = value; }
            } x;
            return x;
          } break;
          
          /***********************************/
          /* Context and thread ID registers */
          /***********************************/

        case CP15ENCODE( 13, 0, 0, 1 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "CONTEXTIDR, Context ID Register"; }
              U32 Read( State& cpu ) { return cpu.SReg("contextidr"); }
              void Write( State& cpu, U32 const& value ) { cpu.SReg("contextidr") = value; }
            } x;
            return x;
          } break;

          /* BOARD specific */
          
        case CP15ENCODE( 15, 0, 0, 1 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "DIAGCR, undocumented Diagnostic Control register"; }
              U32 Read( State& cpu ) { return cpu.SReg("diagcr"); }
              void Write( State& cpu, U32 const& value ) { cpu.SReg("diagcr") = value; }
            } x;
            return x;
          } break;
          
        case CP15ENCODE( 15, 4, 0, 0 ):
          {
            static struct : public CP15Reg
            {
              char const* Describe() { return "CFGBAR, Configuration Base Address"; }
              U32 Read( State& cpu ) { return cpu.SReg("cfgbar"); }
              void Write( State& cpu, U32 const& value ) { cpu.SReg("cfgbar") = value; }
            } x;
            return x;
          } break;

        }

      static struct CP15Error : public CP15Reg {
        char const* Describe() { return "Unknown CP15 register"; }
      } err;
      return err;
    }
    // virtual void     CP15ResetRegisters();

    U32         CP15ReadRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 ) { return CP15GetRegister( crn, opcode1, crm, opcode2 ).Read( *this ); }
    void        CP15WriteRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2, U32 const& value ) { CP15GetRegister( crn, opcode1, crm, opcode2 ).Write( *this, value ); }
    char const* CP15DescribeRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 ) { return CP15GetRegister( crn, opcode1, crm, opcode2 ).Describe(); }

    bool
    close( State const& ref )
    {
      bool complete = path->close();
      path->sinks.insert( Expr( new RegWrite( "pc", next_insn_addr.expr, 32 ) ) );
      if (unpredictable)
        {
          path->sinks.insert( Expr( new AssertFalse() ) );
          return complete;
        }
      if (cpsr.n != ref.cpsr.n)
        path->sinks.insert( Expr( new RegWrite( "n", cpsr.n, 1 ) ) );
      if (cpsr.z != ref.cpsr.z)
        path->sinks.insert( Expr( new RegWrite( "z", cpsr.z, 1 ) ) );
      if (cpsr.c != ref.cpsr.c)
        path->sinks.insert( Expr( new RegWrite( "c", cpsr.c, 1 ) ) );
      if (cpsr.v != ref.cpsr.v)
        path->sinks.insert( Expr( new RegWrite( "v", cpsr.v, 1 ) ) );
      if (cpsr.itstate.expr != ref.cpsr.itstate.expr)
        path->sinks.insert( Expr( new RegWrite( "itstate", cpsr.itstate.expr, 8 ) ) );
      if (cpsr.bg.expr != ref.cpsr.bg.expr)
        path->sinks.insert( Expr( new RegWrite( "cpsr", cpsr.bg.expr, 32 ) ) );
      if (spsr.expr != ref.spsr.expr)
        path->sinks.insert( Expr( new RegWrite( "spsr", spsr.expr, 32 ) ) );
      for (int idx = 0; idx < RegID::sregs_count; ++idx)
        if (sregs[idx].expr != ref.sregs[idx].expr)
          path->sinks.insert( Expr( new RegWrite( RegID::sreg_id(idx), sregs[idx].expr, 32 ) ) );
      if (FPSCR.expr != ref.FPSCR.expr)
        path->sinks.insert( Expr( new RegWrite( "fpscr", FPSCR.expr, 32 ) ) );
      if (FPEXC.expr != ref.FPEXC.expr)
        path->sinks.insert( Expr( new RegWrite( "fpexc", FPEXC.expr, 32 ) ) );
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

  BOOL
  CheckCondition( State& state, State::ITCond const& cond )
  {
    BOOL N = state.cpsr.n, Z = state.cpsr.z, C = state.cpsr.c, V = state.cpsr.v;
    U8 cc = (state.cpsr.itstate >> 4);
    return
      ((state.cpsr.itstate & U8(0b1111)) == U8(0)) or // unconditional
      ((cc == U8(0)) and (Z)) or // eq; equal
      ((cc == U8(1)) and (not Z)) or // ne; not equal
      ((cc == U8(2)) and (C)) or // cs/hs; unsigned higher or same
      ((cc == U8(3)) and (not C)) or // cc/lo; unsigned lower
      ((cc == U8(4)) and (N)) or // mi; negative
      ((cc == U8(5)) and (not N)) or // pl; positive or zero
      ((cc == U8(6)) and (V)) or // vs; overflow set
      ((cc == U8(7)) and (not V)) or // vc; overflow clear
      ((cc == U8(8)) and (not (not C or Z))) or // hi; unsigned higher
      ((cc == U8(9)) and ((not C or Z))) or // ls; unsigned lower or same
      ((cc == U8(10)) and (not (N xor V))) or // ge; signed greater than or equal
      ((cc == U8(11)) and ((N xor V))) or // lt; signed less than
      ((cc == U8(12)) and (not(Z or (N xor V)))) or // gt; signed greater than
      ((cc == U8(13)) and ((Z or (N xor V)))) or // le; signed less than or equal
      ((cc == U8(14)) and (unisim::util::symbolic::make_const( true )));
  }

  // struct GenFlagsID
  // {
  //   enum Code { NA = 0, Sub, Add };

  //   template <class E> static void
  //   Enumeration( E& e )
  //   {
  //     case  Sub: return "Sub";
  //     case  Add: return "Add";
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
  //   GenFlagsID id;
  //   Expr ipsr, lhs, rhs;
  // };


  void
  UpdateStatusSub( State& state, U32 const& res, U32 const& lhs, U32 const& rhs )
  {
    BOOL neg = S32(res) < S32(0);
    state.cpsr.n = neg.expr;
    state.cpsr.z = ( res == U32(0) ).expr;
    state.cpsr.c = ( lhs >= rhs ).expr;
    state.cpsr.v = ( neg xor (S32(lhs) < S32(rhs)) ).expr;
  }

  void
  UpdateStatusSubWithBorrow( State& state, U32 const& res, U32 const& lhs, U32 const& rhs, U32 const& borrow )
  {
    BOOL neg = S32(res) < S32(0);
    state.cpsr.n = neg.expr;
    state.cpsr.z = ( res == U32(0) ).expr;
    if (state.Cond(borrow != U32(0)))
      {
        state.cpsr.c = ( lhs >  rhs ).expr;
        state.cpsr.v = ( neg xor (S32(lhs) <= S32(rhs)) ).expr;
      }
    else
      {
        state.cpsr.c = ( lhs >= rhs ).expr;
        state.cpsr.v = ( neg xor (S32(lhs) <  S32(rhs)) ).expr;
      }
  }

  void
  UpdateStatusAdd( State& state, U32 const& res, U32 const& lhs, U32 const& rhs )
  {
    BOOL neg = S32(res) < S32(0);
    state.cpsr.n = neg.expr;
    state.cpsr.z = ( res == U32(0) ).expr;
    state.cpsr.c = ( lhs >  ~rhs ).expr;
    state.cpsr.v = ( neg xor (S32(lhs) <= S32(~rhs)) ).expr;
  }

  void
  UpdateStatusAddWithCarry( State& state, U32 const& res, U32 const& lhs, U32 const& rhs, U32 const& carry )
  {
    BOOL neg = S32(res) < S32(0);
    state.cpsr.n = neg.expr;
    state.cpsr.z = ( res == U32(0) ).expr;
    if (state.Cond(carry != U32(0)))
      {
        state.cpsr.c = ( lhs >= ~rhs ).expr;
        state.cpsr.v = ( neg xor (S32(lhs) <  S32(~rhs)) ).expr;
      }
    else
      {
        state.cpsr.c = ( lhs >  ~rhs ).expr;
        state.cpsr.v = ( neg xor (S32(lhs) <= S32(~rhs)) ).expr;
      }
  }

  void
  PathNode::GenCode( Label const& start, Label const& after, Context* _up ) const
  {
    Context ctx( _up );

    Expr nia;

    struct Head
    {
      Head( Label const& _start, int _after ) : cur(_start), after(_after) {}
      ~Head()
      {
        if (not pending.size()) return;
        Label::subst_next(pending, after);
        cur.write( pending );
        pending.clear();
      }

      Label& current() { flush(); return cur; }
      void write( std::string const& s )
      {
        flush();
        if (s.size()==0) throw std::runtime_error("xxx");
        pending = s;
      }
      void flush()
      {
        if (not pending.size()) return;
        cur.write( pending );
        pending.clear();
      }
      Label cur;
      int after;
      std::string pending;
    } head( start, after.GetID() );

    {
      /* find common sub expressions */
      struct : public std::map<Expr,unsigned>
      {
        void Process( Expr const& expr )
        {
          for (unsigned idx = 0, end = expr->SubCount(); idx < end; ++idx)
            Recurse( expr->GetSub(idx) );
        }

        void Recurse( Expr const& expr )
        {
          if (not expr->SubCount())
            return;

          if ((*this)[expr]++)
            return;

          Process( expr );
        }
      } sestats;

      std::map<Expr,char const*> rtmps;

      for (std::set<Expr>::const_iterator itr = sinks.begin(), end = sinks.end(); itr != end; ++itr)
        {
          sestats.Process( *itr );
          if (armsec::State::RegWrite const* rw = dynamic_cast<armsec::State::RegWrite const*>( itr->node ))
            rtmps[rw->value] = rw->id.c_str();
        }

      // if (cond.good())
      //   sestats.Process( cond );

      struct CSE : public std::multimap<unsigned,Expr>
      {
        void Process( Expr const& expr )
        {
          insert( std::make_pair( CountSubs( expr ), expr ) );
        }
        unsigned CountSubs( Expr const& expr )
        {
          unsigned sum = 1;
          for (unsigned idx = 0, end = expr->SubCount(); idx < end; ++idx)
            sum += CountSubs( expr->GetSub(idx) );
          return sum;
        }
      } cse;

      for (std::map<Expr,unsigned>::iterator itr = sestats.begin(), end = sestats.end(); itr != end; ++itr)
        {
          if (itr->second < 2)
            continue; // No reuse
          if (ctx.vars.count(itr->first))
            continue; // Already defined
          cse.Process(itr->first);
        }

      for (std::multimap<unsigned,Expr>::const_iterator itr = cse.begin(), end = cse.end(); itr != end; ++itr)
        {
          std::string tmp_src, tmp_dst;
          int retsize;
          {
            std::ostringstream buffer;
            retsize = ASExprNode::GenerateCode( itr->second, ctx.vars, head.current(), buffer );
            tmp_src = buffer.str();
            std::map<Expr,char const*>::const_iterator rtmp = rtmps.find(itr->second);
            tmp_dst = rtmp != rtmps.end() ? ctx.nxtname( rtmp->second, retsize ) : ctx.tmpname( retsize );
            ctx.addvar( tmp_dst, retsize, itr->second );
          }
          {
            std::ostringstream buffer;
            buffer << tmp_dst << " := " << tmp_src << "; goto <next>";
            head.write( buffer.str() );
          }
        }
    }

    for (std::set<Expr>::const_iterator itr = sinks.begin(), end = sinks.end(); itr != end; ++itr)
      {
        if (armsec::State::RegWrite const* rw = dynamic_cast<armsec::State::RegWrite const*>( itr->node ))
          {
            armsec::State::RegID rid = rw->id;
            unsigned             rsz = rw->bitsize;
            Expr value( rw->value );


            if (not value.ConstSimplify() and not ctx.vars.count(value))
              {
                std::ostringstream buffer;
                buffer << "nxt_" << rid.c_str() << "<" << rsz << ">";
                std::string vname = buffer.str();
                buffer << " := " << GetCode(value, ctx.vars, head.current()) << "; goto <next>";
                head.write( buffer.str() );
                ctx.addvar( vname, rsz, value );
              }

            if (rid.code == rid.nia)
              nia = value;
            else
              ctx.add_pending( *itr );
          }
        else
          {
            std::ostringstream buffer;
            if (ASExprNode::GenerateCode( *itr, ctx.vars, head.current(), buffer ))
              throw 0;

            buffer << "; goto <next>";
            head.write( buffer.str() );
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
        Label ifinsn(head.current());
        buffer << "if " << GetCode(cond, ctx.vars, ifinsn) << " ";

        // Preparing room for if then else code
        Label endif( after );
        if (nia.good() or (after.valid() and (ctx.has_pending())))
          endif.allocate();

        if (not false_nxt) {
          Label ifthen(ifinsn);
          buffer << " goto " << ifthen.allocate() << " else goto " << endif.GetID();
          ifinsn.write( buffer.str() );
          true_nxt->GenCode( ifthen, endif, &ctx );
        } else if (not true_nxt) {
          Label ifelse(ifinsn);
          buffer << " goto " << endif.GetID() << " else goto " << ifelse.allocate();
          ifinsn.write( buffer.str() );
          false_nxt->GenCode( ifelse, endif, &ctx );
        } else {
          Label ifthen(ifinsn), ifelse(ifinsn);
          buffer << " goto " << ifthen.allocate() << " else goto " << ifelse.allocate();
          ifinsn.write( buffer.str() );
          true_nxt->GenCode( ifthen, endif, &ctx );
          false_nxt->GenCode( ifelse, endif, &ctx );
        }

        head.current() = endif;
      }

    for (Context::Pendings::iterator itr = ctx.pendings.begin(), end = ctx.pendings.end(); itr != end; ++itr)
      {
        std::ostringstream buffer;
        if (ASExprNode::GenerateCode( *itr, ctx.vars, head.current(), buffer ))
          throw 0;

        buffer << "; goto <next>";
        head.write( buffer.str() );
      }

    if (not nia.good())
      return;

    Label current( head.current() );

    for (Context* uc = ctx.upper; uc; uc = uc->upper)
      {
        for (Context::Pendings::iterator itr = uc->pendings.begin(), end = uc->pendings.end(); itr != end; ++itr)
          {
            std::ostringstream buffer;
            if (ASExprNode::GenerateCode( *itr, ctx.vars, current, buffer ))
              throw 0;

            buffer << "; goto <next>";
            current.write( buffer.str() );
          }
      }

    std::ostringstream buffer;
    buffer << "goto (" << GetCode(nia, ctx.vars, current) << (nia.ConstSimplify() ? ",0" : "") << ")";
    current.write( buffer.str() );
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
  };

  template <class ISA>
  void
  translate_isa( ISA& isa, uint32_t addr, uint32_t code )
  {
    std::cout << "(address . " << armsec::dbx( 32, addr ) << ")\n";

    struct Translation
    {
      typedef typename ISA::Operation Insn;
      typedef typename armsec::PathNode PathNode;
      typedef typename unisim::util::symbolic::Expr Expr;
      typedef typename armsec::State State;

      ~Translation() { delete insn; delete coderoot; }
      Translation( uint32_t _addr )
        : insn(0)
        , coderoot(new armsec::PathNode)
        , addr( _addr )
        , reference( coderoot )
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
                State state( coderoot );
                state.SetInsnProps( insn_addr, ISA::is_thumb, insn->GetLength() );
                insn->execute( state );
                if (ISA::is_thumb)
                  state.ITAdvance();
                end = state.close( reference );
              }
          }
        catch (...)
          {
            return false;
          }

        coderoot->factorize();
        coderoot->remove_dead_paths();

        armsec::Label beglabel(program), endlabel(program);
        beglabel.allocate();
        coderoot->GenCode( beglabel, endlabel, 0 );

        return true;
      }

      Insn*     insn;
      PathNode* coderoot;
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
        typedef armsec::Label::Program::const_iterator Iterator;
        for (Iterator itr = program.begin(), end = program.end(); itr != end; ++itr)
          std::cout << "(" << armsec::dbx( 32, addr ) << ',' << itr->first << ") " << itr->second << std::endl;
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
  return
    "usage: <program> arm|thumb <address> <encoding>\n";
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
      std::cerr << "<addr> and <code> should be 32bits numeric values.\n" << usage();
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
