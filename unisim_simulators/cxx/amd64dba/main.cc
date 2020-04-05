/*
 *  Copyright (c) 2009-2020,
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

#include <unisim/component/cxx/processor/intel/isa/intel.hh>
#include <unisim/component/cxx/processor/intel/isa/intel.tcc>
#include <unisim/component/cxx/processor/intel/modrm.hh>
#include <unisim/component/cxx/processor/intel/disasm.hh>
#include <unisim/component/cxx/processor/intel/vectorbank.hh>
#include <unisim/component/cxx/processor/intel/types.hh>
#include <unisim/util/symbolic/binsec/binsec.hh>
#include <unisim/util/symbolic/symbolic.hh>

#include <cstdint>

// template <typename ARCH, unsigned OPSIZE> struct TypeFor {};

// template <typename ARCH> struct TypeFor<ARCH, 8> { typedef typename ARCH:: S8 S; typedef typename ARCH:: U8 U; };
// template <typename ARCH> struct TypeFor<ARCH,16> { typedef typename ARCH::S16 S; typedef typename ARCH::U16 U; };
// template <typename ARCH> struct TypeFor<ARCH,32> { typedef typename ARCH::S32 S; typedef typename ARCH::U32 U; typedef typename ARCH::F32 F; };
// template <typename ARCH> struct TypeFor<ARCH,64> { typedef typename ARCH::S64 S; typedef typename ARCH::U64 U; typedef typename ARCH::F64 F; };

struct Processor
{
  //   =====================================================================
  //   =                           Configuration                           =
  //   =====================================================================
  struct Unimplemented {};
  struct Undefined {};
    
  //   =====================================================================
  //   =                             Data Types                            =
  //   =====================================================================
  template <typename T> using SmartValue = unisim::util::symbolic::SmartValue<T>;
  template <typename T> using VectorTypeInfo = unisim::component::cxx::processor::intel::VectorTypeInfo<T>;
  template <typename A, unsigned S> using TypeFor = typename unisim::component::cxx::processor::intel::TypeFor<A,S>;
  
  typedef SmartValue<uint8_t>     u8_t;
  typedef SmartValue<uint16_t>    u16_t;
  typedef SmartValue<uint32_t>    u32_t;
  typedef SmartValue<uint64_t>    u64_t;
  typedef SmartValue<int8_t>      s8_t;
  typedef SmartValue<int16_t>     s16_t;
  typedef SmartValue<int32_t>     s32_t;
  typedef SmartValue<int64_t>     s64_t;
  typedef SmartValue<bool>        bit_t;

  typedef u64_t addr_t;
  
  typedef SmartValue<float>       f32_t;
  typedef SmartValue<double>      f64_t;
  typedef SmartValue<long double> f80_t;

  typedef unisim::component::cxx::processor::intel::GObLH GObLH;
  typedef unisim::component::cxx::processor::intel::GOb GOb;
  typedef unisim::component::cxx::processor::intel::GOw GOw;
  typedef unisim::component::cxx::processor::intel::GOd GOd;
  typedef unisim::component::cxx::processor::intel::GOq GOq;

  typedef unisim::component::cxx::processor::intel::SSE SSE;
  typedef unisim::component::cxx::processor::intel::XMM XMM;
  typedef unisim::component::cxx::processor::intel::YMM YMM;
    
  typedef unisim::component::cxx::processor::intel::RMOp<Processor> RMOp;
    
  typedef unisim::util::symbolic::Expr Expr;
  typedef unisim::util::symbolic::ExprNode ExprNode;
  typedef unisim::util::symbolic::ScalarType ScalarType;
    
  typedef GOq   GR;
  typedef u64_t gr_type;
  
  typedef unisim::util::symbolic::binsec::ActionNode   ActionNode;
  typedef unisim::util::symbolic::binsec::Store        Store;
  typedef unisim::util::symbolic::binsec::Branch       Br;

  struct OpHeader { OpHeader( uint64_t _address ) : address( _address ) {} uint64_t address; };

  template <typename RID>
  struct RegRead : public unisim::util::symbolic::binsec::RegRead
  {
    typedef unisim::util::symbolic::binsec::RegRead Super;
    RegRead( RID _id ) : Super(), id(_id) {}
    virtual RegRead* Mutate() const { return new RegRead( *this ); }
    virtual ScalarType::id_t GetType() const
    { return unisim::util::symbolic::TypeInfo<typename RID::register_type>::GetType(); }
    virtual void GetRegName( std::ostream& sink ) const { sink << id.c_str(); }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<RegRead const&>( rhs ) ); }
    int compare( RegRead const& rhs ) const { if (int delta = Super::compare(rhs)) return delta; return id.cmp( rhs.id ); }

    RID id;
  };

  template <typename RID>
  static Expr newRegRead( RID id ) { return new RegRead<RID>( id ); }

  template <typename RID>
  struct RegWrite : public unisim::util::symbolic::binsec::RegWrite
  {
    typedef RegWrite<RID> this_type;
    typedef unisim::util::symbolic::binsec::RegWrite Super;
    RegWrite( RID _id, Expr const& _value ) : Super(_value), id(_id) {}
    virtual this_type* Mutate() const { return new this_type( *this ); }
    virtual ScalarType::id_t GetType() const { return ScalarType::VOID; }
    virtual void GetRegName( std::ostream& sink ) const { sink << id.c_str(); }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<RegWrite const&>( rhs ) ); }
    int compare( RegWrite const& rhs ) const { if (int delta = id.cmp( rhs.id )) return delta; return Super::compare( rhs ); }
    
    RID id;
  };

  template <typename RID>
  static Expr newRegWrite( RID id, Expr const& value ) { return new RegWrite<RID>( id, value ); }
  
  struct Goto : public Br
  {
    Goto( Expr const& value ) : Br( value ) {}
    virtual Goto* Mutate() const override { return new Goto( *this ); }
    virtual void GetRegName( std::ostream& sink ) const override { sink << "pc"; }
    virtual ScalarType::id_t GetType() const override { return ScalarType::VOID; }
    virtual void annotate(std::ostream& sink) const override { return; }
  };

  struct Call : public Goto
  {
    Call( Expr const& value, uint64_t ra ) : Goto( value ), return_address( ra ) {}
    virtual Call* Mutate() const override { return new Call( *this ); }
    virtual void annotate(std::ostream& sink) const override { sink << " // call (" << unisim::util::symbolic::binsec::dbx(8,return_address) << ",0)"; }

    uint32_t return_address;
  };

  
  struct Load : public unisim::util::symbolic::binsec::Load
  {
    Load( Expr const& addr, unsigned size, unsigned alignment, bool bigendian )
      : unisim::util::symbolic::binsec::Load(addr, size, alignment, bigendian)
    {}
  };

  u64_t                       tscread() { throw Unimplemented(); return u64_t( 0 ); }
    
  struct FLAG : public unisim::util::identifier::Identifier<FLAG>
  {
    typedef bool register_type;
    enum Code { CF = 0, PF, AF, ZF, SF, DF, OF, C0, C1, C2, C3, end } code;
      
    char const* c_str() const
    {
      switch (code)
        {
        case CF: return "cf";
        case PF: return "pf";
        case AF: return "af";
        case ZF: return "zf";
        case SF: return "sf";
        case DF: return "df";
        case OF: return "of";
        case C0: return "c0";
        case C1: return "c1";
        case C2: return "c2";
        case C3: return "c3";
        case end: break;
        }
      return "NA";
    }

    FLAG() : code(end) {}
    FLAG( Code _code ) : code(_code) {}
    FLAG( char const* _code ) : code(end) { init(_code); }
  };

  Expr                        flagvalues[FLAG::end];
    
  bit_t                       flagread( FLAG flag ) { return bit_t(flagvalues[flag.idx()]); }
  void                        flagwrite( FLAG flag, bit_t fval ) { flagvalues[flag.idx()] = fval.expr; }

  u16_t                       segregread( unsigned idx ) { throw Unimplemented(); return u16_t(); }
  void                        segregwrite( unsigned idx, u16_t value ) { throw Unimplemented(); }

  typedef std::map<unsigned,unsigned> RegMap;

  enum {VREGCOUNT = 16, GREGCOUNT = 16, FREGCOUNT=8};
  
  static unsigned const REGCOUNT = GREGCOUNT;
  static unsigned const REGSIZE = 8;

  void                        eregtouch( unsigned reg, bool w );
  bool                        eregdiff( unsigned reg );
  Expr                        eregread( unsigned reg, unsigned size, unsigned pos );
  void                        eregwrite( unsigned reg, unsigned size, unsigned pos, Expr const& xpr );

  Expr                        regvalues[REGCOUNT][REGSIZE];

  template <class GOP>
  typename TypeFor<Processor,GOP::SIZE>::u regread( GOP const&, unsigned idx )
  {
    return typename TypeFor<Processor,GOP::SIZE>::u( eregread( idx, GOP::SIZE / 8, 0 ) );
  }

  u8_t regread( GObLH const&, unsigned idx ) { return u8_t( eregread( idx%4, 1, (idx >> 2) & 1 ) ); }

  template <class GOP> void regwrite( GOP const&, unsigned idx, typename TypeFor<Processor,GOP::SIZE>::u const& val )
  { eregwrite( idx, 8, 0, u64_t(val).expr ); }

  void regwrite( GObLH const&, unsigned idx, u8_t val ) { eregwrite( idx%4, 1, (idx>>2) & 1, val.expr ); }
  void regwrite( GOb const&, unsigned idx, u8_t val )   { eregwrite( idx, 1, 0, val.expr ); }
  void regwrite( GOw const&, unsigned idx, u16_t val )  { eregwrite( idx, 2, 0, val.expr ); }

  enum ipproc_t { ipjmp, ipcall, ipret };
    
  addr_t                      getnip() { return next_insn_addr; }
  void                        setnip( addr_t nip, ipproc_t ipproc = ipjmp )
  {
    next_insn_addr = nip;
    next_insn_mode = ipproc;
  }

  //   =====================================================================
  //   =                      Construction/Destruction                     =
  //   =====================================================================

private:
  Processor( Processor const& );
public:
  
  Processor()
    : path(0)
    , next_insn_addr()
      //    , branch_type(B_JMP)
  {
  }

  bool close( Processor const& ref, uint32_t linear_nia )
  {
    bool complete = path->close();
    // if (branch_type == B_CALL)
    //   path->add_sink( new Call( next_insn_addr.expr, linear_nia ) );
    // else
    //   path->add_sink( new Goto( next_insn_addr.expr ) );
    // if (unpredictable)
    //   {
    //     path->add_sink( new unisim::util::symbolic::binsec::AssertFalse() );
    //     return complete;
    //   }
    // if (spsr.expr != ref.spsr.expr)
    //   path->add_sink( newRegWrite( RegID("spsr"), spsr.expr ) );
    // for (SRegID reg; reg.next();)
    //   if (sregs[reg.idx()].expr != ref.sregs[reg.idx()].expr)
    //     path->add_sink( newRegWrite( reg, sregs[reg.idx()].expr ) );
    // if (FPSCR.expr != ref.FPSCR.expr)
    //   path->add_sink( newRegWrite( RegID("fpscr"), FPSCR.expr ) );
    // if (FPEXC.expr != ref.FPEXC.expr)
    //   path->add_sink( newRegWrite( RegID("fpexc"), FPEXC.expr ) );
    // for (unsigned reg = 0; reg < 15; ++reg)
    //   {
    //     if (reg_values[reg].expr != ref.reg_values[reg].expr)
    //       path->add_sink( newRegWrite( RegID("r0") + reg, reg_values[reg].expr ) );
    //   }
    // for (unsigned reg = 0; reg < 32; ++reg)
    //   {
    //     if (neonregs[reg][0] != ref.neonregs[reg][0])
    //       GetNeonSinks(reg);
    //   }
    // for (std::set<Expr>::const_iterator itr = stores.begin(), end = stores.end(); itr != end; ++itr)
    //   path->add_sink( *itr );
    // return complete;
  }

  void step( unisim::component::cxx::processor::intel::Operation<Processor> const& op )
  {
    next_insn_addr = getnip() + addr_t(op.length);
    op.execute( *this );
  }
  
  void noexec( unisim::component::cxx::processor::intel::Operation<Processor> const& op )
  {
    std::cerr
      << "error: no execute method in `" << typeid(op).name() << "'\n"
      << std::hex << op.address << ":\t";
    op.disasm( std::cerr );
    std::cerr << '\n';
    throw Unimplemented();
  }
  
  //   =====================================================================
  //   =                 Internal Instruction Control Flow                 =
  //   =====================================================================

  bool concretize( Expr const& cexp )
  {
    bool predicate = path->proceed( cexp );
    path = path->next( predicate );
    return predicate;
  }
  
  template <typename T>
  bool Test( unisim::util::symbolic::SmartValue<T> const& cond )
  {
    if (not cond.expr.good())
      throw std::logic_error( "Not a valid condition" );

    Expr cexp( BOOL(cond).expr );
    if (unisim::util::symbolic::ConstNodeBase const* cnode = cexp.ConstSimplify())
      return cnode->Get( bool() );

    return concretize( cexp );
  }
  
  ActionNode*      path;
  addr_t           next_insn_addr;
  ipproc_t         next_insn_mode;
  //  branch_type_t    branch_type;
};

struct InstructionAddress : public unisim::util::symbolic::binsec::ASExprNode
{
  InstructionAddress() {}
  virtual void Repr( std::ostream& sink ) const { sink << "insn_addr"; }
  virtual int cmp( unisim::util::symbolic::ExprNode const& rhs ) const override { return compare( dynamic_cast<InstructionAddress const&>( rhs ) ); }
  int compare( InstructionAddress const& rhs ) const { return 0; }
};

struct Translator
{
  typedef unisim::util::symbolic::binsec::ActionNode ActionNode;
  
  Translator( uint64_t _addr, std::vector<uint8_t>&& _code )
    : addr(_addr), code(std::move(_code)), coderoot(new ActionNode)
  {}
  ~Translator() { delete coderoot; }
  
  void
  extract( std::ostream& sink )
  {
    typedef unisim::component::cxx::processor::intel::Operation<Processor> Operation;
    typedef unisim::component::cxx::processor::intel::InputCode<Processor> InputCode;
    
    sink << "(address . " << unisim::util::symbolic::binsec::dbx(8, addr) << ")\n";
  
    // Instruction decoding
    struct Instruction
    {
      Instruction(uint64_t address, uint8_t* bytes) : operation(0)
      {
        unisim::component::cxx::processor::intel::Mode mode( 1, 0, 1 ); /* intel64 mode */
        operation = getoperation( InputCode(mode, bytes, Processor::OpHeader(address) ) );
      }
      ~Instruction() { delete operation; }
      Operation* operator -> () { return operation; }
      Operation& operator * () { return *operation; }
      Operation* operation;
    };
    
    Instruction instruction( addr, &code[0] );

    if (instruction->length > code.size()) { struct LengthError {}; throw LengthError(); }
    code.resize(instruction->length);

    sink << "(opcode . ";
    for (auto byte : code) sink << unisim::util::symbolic::binsec::dbx(2, byte);
    sink << ")";
    
    Processor::addr_t insn_addr = unisim::util::symbolic::make_const(addr); //< concrete instruction address
    uint64_t nia = addr + instruction->length;
    // Processor::U32      insn_addr = Expr(new InstructionAddress()); //< symbolic instruction address
    Processor reference;
    
    // Disassemble
    sink << "(mnemonic . \"";
    try { instruction->disasm( sink ); }
    catch (...) { sink << "(bad)"; }
    sink << "\")\n";
    
    // Get actions
    for (bool end = false; not end;)
      {
        Processor state;
        state.path = coderoot;
        state.step(*instruction);
        end = state.close( reference, nia );
      }
    coderoot->simplify();
    coderoot->commit_stats();
  }

  void translate( std::ostream& sink )
  {
    try
      {
        extract( sink );
      }
    catch (Processor::Undefined const&)
      {
        sink << "(undefined)\n";
        return;
      }
    catch (...)
      {
        sink << "(unimplemented)\n";
        return;
      }

    // Translate to DBA
    unisim::util::symbolic::binsec::Program program;
    program.Generate( coderoot );
    typedef unisim::util::symbolic::binsec::Program::const_iterator Iterator;
    for (Iterator itr = program.begin(), end = program.end(); itr != end; ++itr)
      sink << "(" << unisim::util::symbolic::binsec::dbx(8, addr) << ',' << itr->first << ") " << itr->second << std::endl;
  }
  std::vector<uint8_t> code;
  uint64_t addr;
  ActionNode* coderoot;
};
  
bool getu64( uint64_t& res, char const* arg )
{
  char *end;
  uint32_t tmp = strtoull( arg, &end, 0 );
  if ((*arg == '\0') or (*end != '\0'))
    return false;
  res = tmp;
  return true;
}

bool getbytes( std::vector<uint8_t>& res, char const* arg )
{
  uintptr_t len = strlen(arg);
  res.assign(&arg[0],&arg[len]);
  return true;
}

char const* usage()
{
  return
    "usage: <program> arm|intel <address> <encoding>\n";
}

int
main( int argc, char** argv )
{
  if (argc != 4)
    {
      std::cerr << "Wrong number of CLI arguments.\n" << usage();
      return 1;
    }

  uint64_t addr;
  std::vector<uint8_t> code;

  if (not getu64(addr, argv[1]) or not getbytes(code, argv[2]))
    {
      std::cerr << "<addr> and <code> should be 32bits numeric values.\n" << usage();
      return 1;
    }

  Translator actset( addr, std::move(code) );
  actset.translate( std::cout );
  
  return 0;
}

