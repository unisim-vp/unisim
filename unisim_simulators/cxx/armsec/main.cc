#include <top_arm32.tcc>
#include <top_thumb.tcc>
#include <smart_types.hh>

#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <set>

namespace armsec
{
  typedef SmartValue<double>   F64;
  typedef SmartValue<float>    F32;
  typedef SmartValue<bool>     BOOL;
  typedef SmartValue<uint8_t>  U8;
  typedef SmartValue<uint16_t> U16;
  typedef SmartValue<uint32_t> U32;
  typedef SmartValue<uint64_t> U64;
  typedef SmartValue<int8_t>   S8;
  typedef SmartValue<int16_t>  S16;
  typedef SmartValue<int32_t>  S32;
  typedef SmartValue<int64_t>  S64;
  
  struct PathNode
  {
    PathNode( PathNode* _previous=0 )
      : cond(), sinks(), previous( _previous ), true_nxt(), false_nxt(), complete(false)
    {}
    
    PathNode* proceed( Expr const& _cond, bool& result )
    {
      if (not cond.node) {
        cond = _cond;
        false_nxt = new PathNode( this );
        true_nxt = new PathNode( this );
        result = false;
        return false_nxt;
      }
      
      if (cond != _cond)
        throw std::logic_error( "unexpected condition" );
      
      if (not false_nxt->complete) {
        result = false;
        return false_nxt;
      }
      
      if (true_nxt->complete)
        throw std::logic_error( "unexpected condition" );
      
      result = true;
      return true_nxt;
    }
    
    bool close()
    {
      complete = true;
      if (not previous)
        return true;
      if (previous->true_nxt == this)
        return previous->close();
      return false;
    }
    
    bool remove_dead_paths()
    {
      if (cond) {
        bool leaf = true;
        if (false_nxt) {
          if (false_nxt->remove_dead_paths()) {
            delete false_nxt;
            false_nxt = 0;
          } else
            leaf = false;
        }
        if (true_nxt) {
          if (true_nxt->remove_dead_paths()) {
            delete true_nxt;
            true_nxt = 0;
          } else
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
      if (not cond)
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
    }
    
    void Dump( std::ostream& sink, std::string indent ) const
    {
      for (std::set<Expr>::const_iterator itr = sinks.begin(), end = sinks.end(); itr != end; ++itr)
        sink << indent << *itr << '\n';
      
      if (not cond) {
        if (sinks.size() == 0)
          sink << indent << "pass\n";
        return;
      }
      
      if (true_nxt) {
        sink << "if " << cond << ":\n";
        true_nxt->Dump( sink, indent + "    " );
      }
      
      if (false_nxt) {
        if (true_nxt)
          sink << "else:\n";
        else
          sink << "if not " << cond << ":\n";
        false_nxt->Dump( sink, indent + "    " );
      }
    }
    
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
    
    typedef armsec::FP   FP;
    
    template <typename T>
    bool Cond( SmartValue<T> const& cond )
    {
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
    
    struct SourceID
    {
      enum Code
        {
          NA = 0,
          r0,r1,r2,r3,r4,r5,r6,r7,
          r8,r9,sl,fp,ip,sp,lr,
          cia, nia, cpsr, spsr,
          fpscr, fpexc
        };
    
      template <class SCANNER>
      static void
      ScanOp( SCANNER const& ops )
      {
        { static std::string const _("r0");    if (ops( _, r0))    return; }
        { static std::string const _("r1");    if (ops( _, r1))    return; }
        { static std::string const _("r2");    if (ops( _, r2))    return; }
        { static std::string const _("r3");    if (ops( _, r3))    return; }
        { static std::string const _("r4");    if (ops( _, r4))    return; }
        { static std::string const _("r5");    if (ops( _, r5))    return; }
        { static std::string const _("r6");    if (ops( _, r6))    return; }
        { static std::string const _("r7");    if (ops( _, r7))    return; }
        { static std::string const _("r8");    if (ops( _, r8))    return; }
        { static std::string const _("r9");    if (ops( _, r9))    return; }
        { static std::string const _("sl");    if (ops( _, sl))    return; }
        { static std::string const _("fp");    if (ops( _, fp))    return; }
        { static std::string const _("ip");    if (ops( _, ip))    return; }
        { static std::string const _("sp");    if (ops( _, sp))    return; }
        { static std::string const _("lr");    if (ops( _, lr))    return; }
        { static std::string const _("cia");   if (ops( _, cia))   return; }
        { static std::string const _("nia");   if (ops( _, nia))   return; }
        { static std::string const _("cpsr");  if (ops( _, cpsr))  return; }
        { static std::string const _("spsr");  if (ops( _, spsr))  return; }
        { static std::string const _("fpscr"); if (ops( _, fpscr)) return; }
        { static std::string const _("fpexc"); if (ops( _, fpexc)) return; }
      }
    
      SourceID( Code _code ) : code(_code) {}
      SourceID( std::string _code ) : code(NA) { ScanOp( Str2Enum<Code>( _code, code ) ); }
      SourceID( char const* _code ) : code(NA) { ScanOp( Str2Enum<Code>( _code, code ) ); }
      std::string ToString() const { std::string res("NA"); ScanOp( Enum2Str<Code>( code, res ) ); return res; }
      SourceID operator + ( int offset ) const { return SourceID( Code(int(code) + offset) ); }
      int operator - ( SourceID rid ) const { return int(code) - int(rid.code); }
      
      Code code;
    };
    
    struct Source : public ExprNode
    {
      Source( SourceID _id ) : id( _id ) {}
      Source( char const* name ) : id( name ) {}
      virtual void Repr( std::ostream& sink ) const { sink << "Get(" << id.ToString() << ")"; }
      virtual intptr_t cmp( ExprNode const& brhs ) const {
        Source const& rhs = dynamic_cast<Source const&>( brhs );
        return id - rhs.id;
      }
      virtual ExprNode* GetConstNode() { return 0; };
      
      SourceID id;
    };
    
    struct Sink : public ExprNode
    {
      Sink( SourceID _id, Expr const& _expr ) : id( _id ), expr( _expr ) {}
      Sink( char const* name, Expr const& _expr ) : id( name ), expr( _expr ) {}
      virtual void Repr( std::ostream& sink ) const { sink << id.ToString() << " := " << expr; }
      virtual intptr_t cmp( ExprNode const& brhs ) const {
        Sink const& rhs = dynamic_cast<Sink const&>( brhs );
        if (intptr_t delta = id - rhs.id) return delta;
        return expr.cmp( rhs.expr );
      }
      virtual ExprNode* GetConstNode() { return 0; };
      
      SourceID id;
      Expr expr;
    };
    
    State( PathNode& _path )
      : path( &_path )
      , current_insn_addr()
      , next_insn_addr()
      , cpsr( Expr( new Source("cpsr") ) )
      , spsr( Expr( new Source("spsr") ) )
      , fpscr( *this, Expr( new Source("fpscr") ) )
    {
      for (unsigned reg = 0; reg < 15; ++reg)
        reg_values[reg] = U32( armsec::Expr( new Source( SourceID("r0") + reg ) ) );
    }
    
    void SetInsnProps( Expr const& _expr, bool is_thumb, unsigned insn_length )
    {
      if ((insn_length != 32) and ((insn_length != 16) or not is_thumb))
        throw std::logic_error( "Bad instruction length" );
      current_insn_addr = _expr;
      reg_values[15] = current_insn_addr + U32( is_thumb ? 4 : 8 );
      next_insn_addr = current_insn_addr + U32( insn_length / 8 );
    }
    
    PathNode* path;
    
    U32 reg_values[16];
    U32 current_insn_addr, next_insn_addr;
    typedef unisim::component::cxx::processor::arm::FieldRegister<U32> FieldRegisterU32;
    struct psr_type : public FieldRegisterU32
    {
      psr_type( Expr const& expr )
        : FieldRegisterU32( expr )
      {}
      // void ITSetState( uint32_t cond, uint32_t mask )
      // {
      //   RegisterField<12,4>().Set( m_value, cond );
      //   RegisterField<10,2>().Set( m_value, (mask >> 2) & 3 );
      //   RegisterField<25,2>().Set( m_value, (mask >> 0) & 3 );
      // }
    
      // uint32_t ITGetState() const
      // {
      //   return (RegisterField<10,6>().Get( m_value ) << 2) | RegisterField<25,2>().Get( m_value );
      // }
  
      // bool InITBlock() const
      // { return RegisterField<10,2>().Get( m_value ) or RegisterField<25,2>().Get( m_value ); }
  
      // uint32_t ITGetCondition() const
      // { return this->InITBlock() ? RegisterField<12,4>().Get( m_value ) : 14; }
  
      // void ITAdvance()
      // {
      //   uint32_t state = (RegisterField<10,6>().Get( m_value ) << 2) | RegisterField<25,2>().Get( m_value );
      //   state = (state & 7) ? ((state & -32) | ((state << 1) & 31)) : 0;
      //   RegisterField<10,6>().Set( m_value, state >> 2 );
      //   RegisterField<25,2>().Set( m_value, state & 3 );
      // }
    } cpsr, spsr;
    
    void FPTrap( unsigned exc )
    {
      throw std::logic_error("unimplemented");
    }
    
    struct fpscr_type : public FieldRegisterU32
    {
      fpscr_type( State& _state, Expr const& expr )
        : FieldRegisterU32( expr )
        , state(_state)
      {}
      template <unsigned posT>
      void ProcessException( unisim::component::cxx::processor::arm::RegisterField<posT,1> const& rf )
      {
        unisim::component::cxx::processor::arm::RegisterField<posT+8,1> const enable;
        if (state.Cond(enable.Get( m_value )))
          state.FPTrap( posT );
        else
          rf.Set( m_value, U32(1u) );
      }
      State& state;
    } fpscr;
    U32 FPEXC;
    
    void not_implemented() { throw std::logic_error( "not implemented" ); }

    U32  GetGPR_usr( uint32_t id ) { /* Only work in system mode instruction */ not_implemented(); return U32(); }
    U32  GetGPR( uint32_t id ) { return reg_values[id]; }
    void SetGPR_usr( uint32_t id, U32 const& value ) { /* Only work in system mode instruction */ not_implemented(); }
    void SetGPR_mem( uint32_t id, U32 const& value ) { reg_values[id] = value; }
    void SetGPR( uint32_t id, U32 const& value ) {
      if (id != 15)
        reg_values[id] = value;
      else
        next_insn_addr = value;
    }
    
    U32 GetNIA() { return next_insn_addr; }

    psr_type& CPSR() { return cpsr; };
    psr_type& SPSR() { not_implemented(); return spsr; };
    
    void SetGPRMapping( uint32_t src_mode, uint32_t tar_mode ) { /* system related */ not_implemented(); }
    
    struct Load : public ExprNode
    {
      Load( Expr const& _addr, unsigned _size, bool _aligned )
        : addr( _addr ), size( _size ), aligned( _aligned )
      {}
      virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); addr->Traverse( visitor ); }
      virtual void Repr( std::ostream& sink ) const {
        sink << "Load" << (aligned ? "A" : "") << size << "( " << addr << " )";
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
    
    struct Store : public ExprNode
    {
      Store( Expr const& _addr, unsigned _size, bool _aligned, Expr const& _value )
        : addr( _addr ), size( _size ), aligned( _aligned ), value( _value )
      {}
      virtual void Repr( std::ostream& sink ) const { sink << "Store( " << addr << " )"; }
      intptr_t cmp( ExprNode const& brhs ) const
      {
        Store const& rhs = dynamic_cast<Store const&>( brhs );
        if (intptr_t delta = addr.cmp( rhs.addr )) return delta;
        if (intptr_t delta = value.cmp( rhs.value )) return delta;
        if (intptr_t delta = int(size - rhs.size)) return delta;
        return (int(aligned) - int(rhs.aligned));
      }
      ExprNode* GetConstNode() { return 0; };
      Expr addr;
      unsigned size;
      bool aligned;
      Expr value;
    };
    
    std::set<Expr> stores;
    
    U32  MemURead32( U32 const& addr ) { return U32( Expr( new Load( addr.expr, 4, false ) ) ); }
    U32  MemURead16( U32 const& addr ) { return U32( Expr( new Load( addr.expr, 2, false ) ) ); }
    U32  MemRead32( U32 const& addr ) { return U32( Expr( new Load( addr.expr, 4, true ) ) ); }
    U32  MemRead16( U32 const& addr ) { return U32( Expr( new Load( addr.expr, 2, true ) ) ); }
    U32  MemRead8( U32 const& addr ) { return U32( Expr( new Load( addr.expr, 1, false ) ) ); }
    
    void MemUWrite32( U32 const& addr, U32 const& value ) { stores.insert( new Store( addr.expr, 4, false, value.expr ) ); }
    void MemUWrite16( U32 const& addr, U16 const& value ) { stores.insert( new Store( addr.expr, 2, false, value.expr ) ); }
    void MemWrite32( U32 const& addr, U32 const& value ) { stores.insert( new Store( addr.expr, 4, true, value.expr ) ); }
    void MemWrite16( U32 const& addr, U16 const& value ) { stores.insert( new Store( addr.expr, 2, true, value.expr ) ); }
    void MemWrite8( U32 const& addr, U8 const& value ) { stores.insert( new Store( addr.expr, 1, false, value.expr ) ); }
    
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
  
    fpscr_type& FPSCR() { return fpscr; }
  
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
    char const* CP15DescribeRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 ) { not_implemented(); return ""; }
    
    bool
    close( State const& ref )
    {
      bool complete = path->close();
      path->sinks.insert( Expr( new Sink( "nia", next_insn_addr.expr ) ) );
      if (cpsr.m_value.expr != ref.cpsr.m_value.expr)
        path->sinks.insert( Expr( new Sink( "cpsr", cpsr.m_value.expr ) ) );
      if (spsr.m_value.expr != ref.spsr.m_value.expr)
        path->sinks.insert( Expr( new Sink( "spsr", spsr.m_value.expr ) ) );
      if (fpscr.m_value.expr != ref.fpscr.m_value.expr)
        path->sinks.insert( Expr( new Sink( "fpscr", fpscr.m_value.expr ) ) );
      for (unsigned reg = 0; reg < 15; ++reg) {
        if (reg_values[reg].expr != ref.reg_values[reg].expr)
          path->sinks.insert( Expr( new Sink( SourceID("r0") + reg, reg_values[reg].expr ) ) );
      }
      for (std::set<Expr>::const_iterator itr = stores.begin(), end = stores.end(); itr != end; ++itr)
        path->sinks.insert( *itr );
      return complete;
    }
  };
  
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
  static CodeType mkcode( uint32_t code ) {
    if ((code >> 28) == 15)
      return CodeType( code );
    return CodeType( (0x0fffffff & code) | 0xe0000000 );
  }
  static bool const is_thumb = true;
};


struct Decoder
{
  ARMISA armisa;
  THUMBISA thumbisa;
  
  template <class ISA>
  void
  do_isa( ISA& isa, uint32_t addr, uint32_t code )
  {
    typename ISA::Operation* op = isa.NCDecode( addr, ISA::mkcode( code ) );
    
    armsec::Expr insn_addr( new armsec::State::Source("cia") ); //<if instruction address shall a remain variable
    // armsec::Expr insn_addr( armsec::make_const( addr ) ); //<if instruction address shall be known
    
    armsec::PathNode path;
    std::cout << std::hex << addr << std::dec << ": ";
    
    armsec::State reference( path );
    reference.SetInsnProps( insn_addr, isa.is_thumb, op->GetLength() );
      
    op->disasm( reference, std::cout );
    std::cout << std::endl;
    for (bool end = false; not end;) {
      armsec::State state( path );
      state.SetInsnProps( insn_addr, isa.is_thumb, op->GetLength() );
      op->execute( state );
      end = state.close( reference );
    }
    path.factorize();
    path.remove_dead_paths();
    path.Dump( std::cout, "" );
  }
  void  do_arm( uint32_t addr, uint32_t code ) { do_isa( armisa, addr, code ); }
  void  do_thumb( uint32_t addr, uint32_t code ) { do_isa( thumbisa, addr, code ); }
};

uint32_t getu32( char const* arg )
{
  char *end;
  uint32_t res = strtoul( arg, &end, 0 );
  if ((*arg == '\0') or (*end != '\0'))
    throw std::runtime_error( "Invalid argument." );
  return res;
}

int
main( int argc, char** argv )
{
  if (argc != 4) {
    std::cerr << "usage: <prog> [arm|thumb] <addr> <code>\n";
    return 1;
  }
  
  uint32_t addr = getu32(argv[2]), code = getu32(argv[3]);
  
  Decoder decoder;
  std::string iset(argv[1]);
  if        (iset == std::string("arm")) {
    decoder.do_arm( addr, code );
  } else if (iset == std::string("thumb")) {
    decoder.do_thumb( addr, code );
  }
  
  return 0;
}
