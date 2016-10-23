#include <unisim/component/cxx/processor/arm/disasm.hh>
#include <top_arm32.tcc>
#include <top_thumb.tcc>
#include <smart_types.hh>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <set>

#if __cplusplus >= 201103L
#include <memory>
#else
#include <tr1/memory>
namespace std {

template< class T > class shared_ptr : public tr1::shared_ptr<T> {
public:
  shared_ptr() {}
  shared_ptr( T* ptr ) : tr1::shared_ptr<T>(ptr) {}
  shared_ptr( const shared_ptr& r ) : tr1::shared_ptr<T>(r) {}
};

} // end of namespace std
#endif

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
  
  struct Label;
  
  struct PathNode
  {
    PathNode( PathNode* _previous=0 )
      : cond(), sinks(), previous( _previous ), true_nxt(), false_nxt(), complete(false)
    {}
    
    static void proceed( std::shared_ptr<PathNode>& this_node,
      Expr const& _cond, bool& result )
    {
      if (not this_node->cond.node) {
        PathNode* previous = this_node.get();
        previous->cond = _cond;
        previous->false_nxt.reset(new PathNode(previous));
        previous->true_nxt.reset(new PathNode(previous));
        result = false;
        this_node = previous->false_nxt;
        return;
      }
      
      if (this_node->cond != _cond)
        throw std::logic_error( "unexpected condition" );
      
      if (not this_node->false_nxt->complete) {
        result = false;
        this_node = this_node->false_nxt;
        return;
      }
      
      if (this_node->true_nxt->complete)
        throw std::logic_error( "unexpected condition" );
      
      result = true;
      this_node = this_node->true_nxt;
    }
    
    bool close()
    {
      complete = true;
      if (not previous)
        return true;
      if (previous->true_nxt.get() == this)
        return previous->close();
      return false;
    }
    
    bool remove_dead_paths()
    {
      if (cond) {
        bool leaf = true;
        if (false_nxt) {
          if (false_nxt->remove_dead_paths()) {
            false_nxt.reset();
          } else
            leaf = false;
        }
        if (true_nxt) {
          if (true_nxt->remove_dead_paths()) {
            true_nxt.reset();
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
      
      // If condition begins with a logical not, remove the not and
      //   swap if then else branches
      if (armsec::UONode* uon = dynamic_cast<armsec::UONode*>( cond.node ))
        if (uon->unop.cmp( armsec::UnaryOp("Not") ) == 0) {
          cond = uon->src;
          std::swap( false_nxt, true_nxt );
        }
    }
  
    typedef std::vector<armsec::Expr> ExprStack;

    void GenCode( std::ostream&, Label const&, Label const&, ExprStack& ) const;
    
    Expr cond;
    std::set<Expr> sinks;
    PathNode* previous;
    std::shared_ptr<PathNode> true_nxt;
    std::shared_ptr<PathNode> false_nxt;
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
    bool Cond( SmartValue<T> cond )
    {
      if (not cond.expr.node)
        throw std::logic_error( "Not a cond" );
      
      if (ExprNode* enode = cond.expr->GetConstNode())
        if (ConstNodeBase* cnode = dynamic_cast<ConstNodeBase*>( enode ))
          return cnode->GetU8();
      
      bool result;
      PathNode::proceed( path, cond.expr, result );
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
        if (e(    "nia", nia    )) return;
        if (e(   "cpsr", cpsr   )) return;
        if (e(   "spsr", spsr   )) return;
        if (e(  "fpscr", fpscr  )) return;
        if (e(  "fpexc", fpexc  )) return;
      }
    
      RegID( Code _code ) : code(_code) {}
      RegID( char const* _code ) : code(NA) { cstr2enum( *this, _code ); }
      
      char const* c_str() const { return enum2cstr( *this, "NA" ); }
      RegID operator + ( int offset ) const { return RegID( Code(int(code) + offset) ); }
      int operator - ( RegID rid ) const { return int(code) - int(rid.code); }
      bool operator == ( RegID rid ) const { return code == rid.code; }
      
      Code code;
    };
    
    struct RegRead : public ExprNode
    {
      RegRead( RegID _id ) : id( _id ) {}
      RegRead( char const* name ) : id( name ) {}
      virtual void Repr( std::ostream& sink ) const { sink << id.c_str(); }
      virtual intptr_t cmp( ExprNode const& brhs ) const {
        RegRead const& rhs = dynamic_cast<RegRead const&>( brhs );
        return id - rhs.id;
      }
      virtual ExprNode* GetConstNode() { return 0; };
      
      RegID id;
    };
    
    struct RegWrite : public ExprNode
    {
      RegWrite( RegID _id, Expr const& _value ) : id( _id ), value( _value ) {}
      RegWrite( char const* name, Expr const& _value ) : id( name ), value( _value ) {}
      virtual void Repr( std::ostream& sink ) const { sink << id.c_str() << " := " << value; }
      virtual intptr_t cmp( ExprNode const& brhs ) const {
        RegWrite const& rhs = dynamic_cast<RegWrite const&>( brhs );
        if (intptr_t delta = id - rhs.id) return delta;
        return value.cmp( rhs.value );
      }
      virtual ExprNode* GetConstNode() { return 0; };
      
      RegID id;
      Expr value;
    };
    
    State( std::shared_ptr<PathNode> _path )
      : path( _path )
      , next_insn_addr()
      , cpsr( Expr( new RegRead("cpsr") ) )
      , spsr( Expr( new RegRead("spsr") ) )
      , FPSCR( Expr( new RegRead("fpscr") ) )
      , FPEXC( Expr( new RegRead("fpexc") ) )
    {
      for (unsigned reg = 0; reg < 15; ++reg)
        reg_values[reg] = U32( armsec::Expr( new RegRead( RegID("r0") + reg ) ) );
    }
    
    void SetInsnProps( Expr const& _expr, bool is_thumb, unsigned insn_length )
    {
      if ((insn_length != 32) and ((insn_length != 16) or not is_thumb))
        throw std::logic_error( "Bad instruction length" );
      U32 insn_addr = _expr;
      reg_values[15] = insn_addr + U32( is_thumb ? 4 : 8 );
      next_insn_addr = insn_addr + U32( insn_length / 8 );
    }
    
    std::shared_ptr<PathNode> path;
    
    U32 reg_values[16];
    U32 next_insn_addr;
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
    
    U32 FPSCR, FPEXC;
    
    U32 RoundTowardsZeroFPSCR() const
    {
      return unisim::component::cxx::processor::arm::RMode.Insert( FPSCR, U32(unisim::component::cxx::processor::arm::RoundTowardsZero) );
    }
    
    U32 RoundToNearestFPSCR() const
    {
      return unisim::component::cxx::processor::arm::RMode.Insert( FPSCR, U32(unisim::component::cxx::processor::arm::RoundToNearest) );
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
    psr_type& SPSR() { not_implemented(); return spsr; };
    
    void SetGPRMapping( uint32_t src_mode, uint32_t tar_mode ) { /* system related */ not_implemented(); }
    
    struct Load : public ExprNode
    {
      Load( unsigned _size, bool _aligned, Expr const& _addr )
        : addr(_addr), size( _size ), aligned(_aligned)
      {}
      virtual void Traverse( Visitor& visitor ) const { visitor.Process( this ); addr->Traverse( visitor ); }
      virtual void Repr( std::ostream& sink ) const {
        sink << "Load" << (aligned ? "A" : "") << 8*size << "( " << addr << " )";
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
      Store( unsigned _size, bool _aligned, Expr const& _addr, Expr const& _value )
        : value(_value), addr(_addr), size(_size), aligned(_aligned)
      {}
      
      virtual void Repr( std::ostream& sink ) const { sink << "@[" << addr << ",<-," << size << "] := " << value; }
      
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
    U32  MemURead16( U32 const& addr ) { return U32( Expr( new Load( 2, false, addr.expr ) ) ); }
    U32  MemRead32( U32 const& addr ) { return U32( Expr( new Load( 4, true, addr.expr ) ) ); }
    U32  MemRead16( U32 const& addr ) { return U32( Expr( new Load( 2, true, addr.expr ) ) ); }
    U32  MemRead8( U32 const& addr ) { return U32( Expr( new Load( 1, false, addr.expr ) ) ); }
    
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
    char const* CP15DescribeRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 ) { not_implemented(); return ""; }
    
    bool
    close( State const& ref )
    {
      bool complete = path->close();
      path->sinks.insert( Expr( new RegWrite( "nia", next_insn_addr.expr ) ) );
      if (cpsr.m_value.expr != ref.cpsr.m_value.expr)
        path->sinks.insert( Expr( new RegWrite( "cpsr", cpsr.m_value.expr ) ) );
      if (spsr.m_value.expr != ref.spsr.m_value.expr)
        path->sinks.insert( Expr( new RegWrite( "spsr", spsr.m_value.expr ) ) );
      if (FPSCR.expr != ref.FPSCR.expr)
        path->sinks.insert( Expr( new RegWrite( "fpscr", FPSCR.expr ) ) );
      for (unsigned reg = 0; reg < 15; ++reg) {
        if (reg_values[reg].expr != ref.reg_values[reg].expr)
          path->sinks.insert( Expr( new RegWrite( RegID("r0") + reg, reg_values[reg].expr ) ) );
      }
      for (std::set<Expr>::const_iterator itr = stores.begin(), end = stores.end(); itr != end; ++itr)
        path->sinks.insert( *itr );
      return complete;
    }
  };

  struct ConditionTest : public ExprNode
  {
    ConditionTest( unsigned _cond ) : cond( _cond ) {} unsigned cond;
    virtual void Repr( std::ostream& sink ) const { sink << "CTST_" << unisim::component::cxx::processor::arm::DisasmCondition(cond) << "(flags)"; }
    virtual intptr_t cmp( ExprNode const& brhs ) const {
      ConditionTest const& rhs = dynamic_cast<ConditionTest const&>( brhs );
      return int(cond) - int(rhs.cond);
    }
    virtual ExprNode* GetConstNode() { return 0; };
  };

  BOOL
  CheckCondition( State& state, unsigned cond )
  {
    if (cond == 14) return make_const( true );
    return BOOL( Expr( new ConditionTest( cond ) ) );
  }
  
  struct GenFlagsID
  {
    enum Code { NA = 0, Sub, Add };
    
    template <class E> static void
    Enumeration( E& e )
    {
      if (e( "Sub", Sub )) return;
      if (e( "Add", Add )) return;
    }
    
    GenFlagsID( Code _code ) : code(_code) {}
    GenFlagsID( char const* _code ) : code(NA) { cstr2enum( *this, _code ); }
    
    char const* c_str() const { return enum2cstr( *this, "NA" ); }
    GenFlagsID operator + ( int offset ) const { return GenFlagsID( Code(int(code) + offset) ); }
    int operator - ( GenFlagsID rid ) const { return int(code) - int(rid.code); }
      
    Code code;
  };
  
  struct GenFlags : public ExprNode
  {
    GenFlags( GenFlagsID _id, Expr const& _ipsr, Expr const& _lhs, Expr const& _rhs )
      : id(_id), ipsr(_ipsr), lhs(_lhs), rhs(_rhs) {}
    
    virtual void Repr( std::ostream& sink ) const
    {
      sink << "GenFlags(" << id.c_str() << ", " << ipsr << ", " << lhs << ", " << rhs << ")";
    }
    virtual intptr_t cmp( ExprNode const& bright ) const
    {
      GenFlags const& right = dynamic_cast<GenFlags const&>( bright );
      if (int delta = id - right.id) return delta;
      if (intptr_t delta = ipsr.cmp(right.ipsr)) return delta;
      if (intptr_t delta = lhs.cmp(right.lhs)) return delta;
      return rhs.cmp(right.rhs);
    }
    virtual ExprNode* GetConstNode() { return 0; };
    GenFlagsID id;
    Expr ipsr, lhs, rhs;
  };

  
  void
  UpdateStatusSub( State& state, U32 const& res, U32 const& lhs, U32 const& rhs )
  {
    state.cpsr.m_value.expr = Expr( new GenFlags( "Sub", state.cpsr.m_value.expr, lhs.expr, rhs.expr ) );
  }
  
  void
  UpdateStatusAdd( State& state, U32 const& res, U32 const& lhs, U32 const& rhs )
  {
    state.cpsr.m_value.expr = Expr( new GenFlags( "Add", state.cpsr.m_value.expr, lhs.expr, rhs.expr ) );
  }
  
  struct Label
  {
    Label() : id(-1) {}
    
    static int gid;
    
    struct Statement
    {
      Statement( uint32_t _addr, int _id ) : addr(_addr), id(_id) {} uint32_t addr; int id;
      friend std::ostream& operator << ( std::ostream& sink, Statement const& stmt )
      { sink << "(0x" << std::hex << stmt.addr << ',' << std::dec << stmt.id << ") "; return sink; }
    };
    
    Statement statement( uint32_t addr ) const { return Statement( addr, id ); }
    int next() { id = gid++; return id; }
    bool valid() const { return id >= 0; }
    static Label get() { Label r; r.next(); return r; }
    
    int id;
  };
  
  int Label::gid = 0;
  
  void
  PathNode::GenCode( std::ostream& sink, Label const& start, Label const& after, ExprStack& pending ) const
  {
    struct F
    {
      F( ExprStack& _p ) : p(_p), s(p.size()) {}
      ~F() { p.resize(s); }
      ExprStack& p; uintptr_t s;
    } frame( pending );
    
    armsec::Expr nia;
    
    Label current( start );
    
    for (std::set<armsec::Expr>::const_iterator itr = sinks.begin(), end = sinks.end(); itr != end; ++itr) {
      if (armsec::State::RegWrite* rw = dynamic_cast<armsec::State::RegWrite*>( itr->node ))
        {
          armsec::Expr wb;
          armsec::State::RegID rid = rw->id;
          
          if (rw->value.MakeConst()) {
            wb = *itr;
          }
          
          else {
            struct TmpVar : public armsec::ExprNode
            {
              TmpVar( std::string const& _id ) : id(_id) {} std::string id;
              virtual void Repr( std::ostream& sink ) const { sink << id; }
              virtual intptr_t cmp( ExprNode const& rhs ) const { return id.compare( dynamic_cast<TmpVar const&>( rhs ).id ); }
              virtual ExprNode* GetConstNode() { return 0; };
            } *tmp = new TmpVar( std::string( "n_" ) + rid.c_str() );
            
            sink << current.statement(0x0) << tmp->id << " := " << rw->value;
            sink << "; goto " << current.next() << '\n';
            wb = new armsec::State::RegWrite( rid, tmp );
          }
          
          if (rid == armsec::State::RegID::nia)
            nia = dynamic_cast<armsec::State::RegWrite&>( *wb.node ).value;
          else
            pending.push_back( wb );
        }
      else
        {
          sink << current.statement(0x0) << *itr;
          sink << "; goto " << current.next() << '\n';
        }
    }
    
    if (not cond)
      {
        if (sinks.size() == 0)
          throw std::logic_error( "empty leaf" );
      }
    else
      {
        sink << current.statement(0x0) << "if (" << cond << ") ";
    
        current = after;
        if (nia or (after.valid() and (pending.size() > frame.s)))
          current.next();
    
        if (not false_nxt) {
          Label ifthen;
          sink << " goto " << ifthen.next() << " else goto " << current.id << '\n';
          true_nxt->GenCode( sink, ifthen, current, pending );
        } else if (not true_nxt) {
          Label ifelse;
          sink << " goto " << current.id << " else goto " << ifelse.next() << '\n';
          false_nxt->GenCode( sink, ifelse, current, pending );
        } else {
          Label ifthen, ifelse;
          sink << " goto " << ifthen.next() << " else goto " << ifelse.next() << '\n';
          true_nxt->GenCode( sink, ifthen, current, pending );
          false_nxt->GenCode( sink, ifelse, current, pending );
        }
      }
    
    int step = 0;
    uintptr_t idx = pending.size();
    while (idx > frame.s)
      {
        idx -= 1;
        if (step++>0) sink << "; goto " << current.next() << '\n';
        sink << current.statement(0x0) << pending[idx];
      }
    
    if (not nia) {
      if (step>0) sink << "; goto " << after.id << "\n";
      return;
    }
    
    while (idx > 0)
      {
        idx -= 1;
        if (step++>0) sink << "; goto " << current.next() << '\n';
        sink << current.statement(0x0) << pending[idx];
      }
    
    if (step>0) sink << "; goto " << current.next() << '\n';
    sink << current.statement(0x0) << "goto (" << nia << ")\n";
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

  struct InstructionAddress : public armsec::ExprNode
  {
    InstructionAddress() {}
    virtual void Repr( std::ostream& sink ) const { sink << "insn_addr"; }
    virtual intptr_t cmp( ExprNode const& brhs ) const { dynamic_cast<InstructionAddress const&>( brhs ); return 0; }
    virtual ExprNode* GetConstNode() { return 0; };
  };
  
  template <class ISA>
  void
  do_isa( ISA& isa, uint32_t addr, uint32_t code )
  {
#if __cplusplus >= 201103L
    std::unique_ptr
#else
    std::auto_ptr
#endif
      <typename ISA::Operation> op(isa.NCDecode( addr, ISA::mkcode( code ) ) );
    
    // armsec::Expr insn_addr( new InstructionAddress() ); //<if instruction address shall a remain variable
    armsec::Expr insn_addr( armsec::make_const( addr ) ); //<if instruction address shall be known
    
    std::shared_ptr<armsec::PathNode> path ( new armsec::PathNode );
    std::cout << op->GetName() << std::endl;
    std::cout << '@' << std::hex << addr << ',' << op->GetEncoding() << std::dec <<": ";
    
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
    path->factorize();
    path->remove_dead_paths();
    
    std::vector<armsec::Expr> init;
    path->GenCode( std::cout, armsec::Label::get(), armsec::Label(), init );
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
