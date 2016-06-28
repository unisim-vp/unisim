#include <top_arm32.tcc>
#include <top_thumb.tcc>
#include <smart_types.hh>

#include <iostream>
#include <cstdlib>

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
    PathNode( PathNode* _previous=0 ) : expr(), previous( _previous ), true_nxt(), false_nxt(), complete(false) {}
    
    PathNode* proceed( Expr const& cond, bool& result )
    {
      if (not expr.node) {
        expr = cond;
        false_nxt = new PathNode( this );
        true_nxt = new PathNode( this );
        result = false;
        return false_nxt;
      }
      
      if (expr != cond)
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
    
    void Dump( std::ostream& sink )
    {
      sink << ((previous->true_nxt != this) ? "not " : "    ");
      previous->expr->Repr( sink );
    }
    
    Expr expr;
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
    
    struct SourceReg : public ExprNode
    {
      SourceReg( unsigned _reg ) : reg( _reg ) {} unsigned reg;
      virtual void Repr( std::ostream& sink ) const { sink << "SourceReg(" << reg << ")"; }
      intptr_t cmp( ExprNode const& brhs ) const
      {
        SourceReg const& rhs = dynamic_cast<SourceReg const&>( brhs );
        return int(reg - rhs.reg);
      }
      ExprNode* GetConstNode() { return 0; };
    };
  
    struct SourceCIA : public ExprNode
    {
      SourceCIA() {}
      virtual void Repr( std::ostream& sink ) const { sink << "SourceCIA()"; }
      intptr_t cmp( ExprNode const& brhs ) const { return 0; }
      ExprNode* GetConstNode() { return 0; };
    };
    
    struct SourceCPSR : public ExprNode
    {
      SourceCPSR() {}
      virtual void Repr( std::ostream& sink ) const { sink << "SourceCPSR()"; }
      intptr_t cmp( ExprNode const& brhs ) const { return 0; }
      ExprNode* GetConstNode() { return 0; };
    };
    
    struct SourceSPSR : public ExprNode
    {
      SourceSPSR() {}
      virtual void Repr( std::ostream& sink ) const { sink << "SourceSPSR()"; }
      intptr_t cmp( ExprNode const& brhs ) const { return 0; }
      ExprNode* GetConstNode() { return 0; };
    };
    
    struct SourceFPSCR : public ExprNode
    {
      SourceFPSCR() {}
      virtual void Repr( std::ostream& sink ) const { sink << "SourceFPSCR()"; }
      intptr_t cmp( ExprNode const& brhs ) const { return 0; }
      ExprNode* GetConstNode() { return 0; };
    };
    
    struct SourceFPEXC : public ExprNode
    {
      SourceFPEXC() {}
      virtual void Repr( std::ostream& sink ) const { sink << "SourceFPEXC()"; }
      intptr_t cmp( ExprNode const& brhs ) const { return 0; }
      ExprNode* GetConstNode() { return 0; };
    };
    
    
    State( PathNode& _path, bool is_thumb, unsigned insn_length )
      : path( &_path )
      , current_insn_addr( Expr( new SourceCIA ) )
      , next_insn_addr()
      , cpsr( Expr( new SourceCPSR ) )
      , spsr( Expr( new SourceSPSR ) )
      , fpscr( *this, Expr( new SourceFPSCR ) )
    {
      for (unsigned reg = 0; reg < 15; ++reg)
        reg_values[reg] = U32( armsec::Expr( new SourceReg( reg ) ) );
      reg_values[15] = current_insn_addr + U32( is_thumb ? 4 : 8 );
      if ((insn_length != 32) and ((insn_length != 16) or not is_thumb))
        throw std::logic_error( "Bad instruction length" );
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
        sink << "Load" << (aligned ? "A" : "") << size << "( "; addr->Repr( sink ); sink << " )";
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
      virtual void Repr( std::ostream& sink ) const { sink << "Store( "; addr->Repr( sink ); sink << " )"; }
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
    
    std::vector<Expr> stores;
    
    U32  MemURead32( U32 const& addr ) { return U32( Expr( new Load( addr.expr, 4, false ) ) ); }
    U32  MemURead16( U32 const& addr ) { return U32( Expr( new Load( addr.expr, 2, false ) ) ); }
    U32  MemRead32( U32 const& addr ) { return U32( Expr( new Load( addr.expr, 4, true ) ) ); }
    U32  MemRead16( U32 const& addr ) { return U32( Expr( new Load( addr.expr, 2, true ) ) ); }
    U32  MemRead8( U32 const& addr ) { return U32( Expr( new Load( addr.expr, 1, false ) ) ); }
    
    void MemUWrite32( U32 const& addr, U32 const& value ) { stores.push_back( new Store( addr.expr, 4, false, value.expr ) ); }
    void MemUWrite16( U32 const& addr, U16 const& value ) { stores.push_back( new Store( addr.expr, 2, false, value.expr ) ); }
    void MemWrite32( U32 const& addr, U32 const& value ) { stores.push_back( new Store( addr.expr, 4, true, value.expr ) ); }
    void MemWrite16( U32 const& addr, U16 const& value ) { stores.push_back( new Store( addr.expr, 2, true, value.expr ) ); }
    void MemWrite8( U32 const& addr, U8 const& value ) { stores.push_back( new Store( addr.expr, 1, false, value.expr ) ); }
    
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
    
    void dump()
    {
      std::cout << "hello\n";
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
    armsec::PathNode path;
    std::cout << std::hex << addr << std::dec << ": ";
    {
      armsec::State for_disasm_purpose( path, isa.is_thumb, op->GetLength() );
      op->disasm( for_disasm_purpose, std::cout );
    }
    std::cout << std::endl;
    for (bool end = false; not end;) {
      armsec::State state( path, isa.is_thumb, op->GetLength() );
      op->execute( state );
      end = state.path->close();
      char const* sep = "if    ";
      for (armsec::PathNode* pn = state.path; pn->previous; pn = pn->previous, sep = "  and ")
        {
          std::cout << sep; pn->Dump( std::cout ); std::cout << std::endl;
        }
    }
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
