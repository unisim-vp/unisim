#include <unisim/component/cxx/processor/arm/disasm.hh>
#include <unisim/component/cxx/processor/arm/psr.hh>
#include <unisim/util/symbolic/binsec/binsec.hh>
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
#include <cstdio>

#define CP15ENCODE( CRN, OPC1, CRM, OPC2 ) ((OPC1 << 12) | (CRN << 8) | (CRM << 4) | (OPC2 << 0))

struct Processor
{
  //   =====================================================================
  //   =                           Configuration                           =
  //   =====================================================================
    
  struct Config
  {
    // Following a standard armv7-a configuration
    static uint32_t const model = unisim::component::cxx::processor::arm::ARMEMU;
    static bool const     insns4T = true;
    static bool const     insns5E = true;
    static bool const     insns5J = true;
    static bool const     insns5T = true;
    static bool const     insns6  = true;
    static bool const     insnsRM = false;
    static bool const     insnsT2 = true;
    static bool const     insns7  = true;
  };
    
  //   =====================================================================
  //   =                             Data Types                            =
  //   =====================================================================
    
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
    
  typedef unisim::util::symbolic::FP                   FP;
  typedef unisim::util::symbolic::Expr                 Expr;
  typedef unisim::util::symbolic::ActionNode           ActionNode;
  
  typedef unisim::util::symbolic::binsec::Load         Load;
  typedef unisim::util::symbolic::binsec::Store        Store;
  typedef unisim::util::symbolic::binsec::Branch       Br;

  template <typename RID>
  struct RegRead : public unisim::util::symbolic::binsec::RegRead
  {
    typedef unisim::util::symbolic::binsec::RegRead Super;
    RegRead( RID _id, unsigned _bitsize ) : Super(_bitsize), id(_id) {}
    
    virtual char const* GetRegName() const { return id.c_str(); }
    virtual intptr_t cmp( ExprNode const& brhs ) const
    { if (intptr_t delta = id.cmp( dynamic_cast<RegRead const&>( brhs ).id )) return delta; return Super::cmp( brhs ); }
    
    RID id;
  };

  template <typename RID>
  static RegRead<RID>* newRegRead( RID id, unsigned bitsize ) { return new RegRead<RID>( id, bitsize ); }

  struct ForeignRegister : public unisim::util::symbolic::binsec::RegRead
  {
    typedef unisim::util::symbolic::binsec::RegRead Super;
    ForeignRegister( uint8_t _mode, unsigned _idx )
      : Super(32), name(), idx(_idx), mode(_mode)
    {
      if (mode == SYSTEM_MODE) mode = USER_MODE;
      std::ostringstream buf;
      buf << (RegID("r0") + idx).c_str() << '_' << mode_ident();
      strncpy(&name[0],buf.str().c_str(),sizeof(name)-1);
    }
    
    char const* mode_ident() const
    {
      switch (mode)
        {
        case USER_MODE: return "usr";
        case FIQ_MODE: return "fiq";
        case IRQ_MODE: return "irq";
        case SUPERVISOR_MODE: return "svc";
        case MONITOR_MODE: return "mon";
        case ABORT_MODE: return "abt";
        case HYPERVISOR_MODE: return "hyp";
        case UNDEFINED_MODE: return "und";
        }
      throw 0;
      return "";
    }

    virtual char const* GetRegName() const { return &name[0]; }
    virtual intptr_t cmp( ExprNode const& brhs ) const
    {
      ForeignRegister const& rhs =  dynamic_cast<ForeignRegister const&>( brhs );
      if (int delta = int(mode) - int(rhs.mode)) return delta;
      return idx - rhs.idx;
    }
    
    char name[8];
    unsigned idx;
    uint8_t mode;
  };
  
  template <typename RID>
  struct RegWrite : public unisim::util::symbolic::binsec::RegWrite
  {
    typedef unisim::util::symbolic::binsec::RegWrite Super;
    RegWrite( RID _id, Expr const& _value, unsigned _bitsize ) : Super(_value, _bitsize), id(_id) {}
    
    virtual char const* GetRegName() const { return id.c_str(); }
    virtual intptr_t cmp( ExprNode const& brhs ) const
    { if (intptr_t delta = id.cmp( dynamic_cast<RegWrite const&>( brhs ).id )) return delta; return Super::cmp( brhs ); }
    
    RID id;
  };

  template <typename RID>
  static RegWrite<RID>* newRegWrite( RID id, Expr const& value, unsigned bitsize ) { return new RegWrite<RID>( id, value, bitsize ); }
  
  struct PCWrite : public Br
  {
    PCWrite( Expr const& value, Br::type_t bt ) : Br( value, 32, bt ) {}
    virtual char const* GetRegName() const { return "pc"; }
  };

  struct ITCond {};
    
  struct Mode
  {
    Mode() {}
    bool     HasBR( unsigned index ) { return false; }
    bool     HasSPSR() { return false; }
    void     SetSPSR(U32 const& value) {}
    U32      GetSPSR() { return U32(); }
    void     Swap( Processor& ) {}
  };
    
  typedef std::map<std::pair<uint8_t,uint32_t>,Expr> ForeignRegisters;
  
  struct CP15Reg
  {
    virtual            ~CP15Reg() {}
    virtual unsigned    RequiredPL() { return 1; }
    virtual void        Write( Processor& proc, U32 const& value ) { proc.not_implemented(); }
    virtual U32         Read( Processor& proc ) { proc.not_implemented(); return U32(); }
    virtual char const* Describe() = 0;
  };

  //   =====================================================================
  //   =                      Construction/Destruction                     =
  //   =====================================================================
  
  struct StatusRegister
  {
    enum InstructionSet { Arm, Thumb, Jazelle, ThumbEE };
      
    typedef unisim::util::symbolic::Expr       Expr;
    StatusRegister()
      : n(newRegRead(RegID("n"),1))
      , z(newRegRead(RegID("z"),1))
      , c(newRegRead(RegID("c"),1))
      , v(newRegRead(RegID("v"),1))
      , itstate(newRegRead(RegID("itstate"),8)) // Default is variable
      , bg(newRegRead(RegID("cpsr"),32))        
      , iset(Arm)                               // Default is ARM instruction set
      , bigendian(false)                        // Default is Little Endian
      , mode(SUPERVISOR_MODE)                   // Default is SUPERVISOR_MODE
    {}

    void FixITState( unsigned is ) { itstate = U8(is); }
    bool IsThumb() const { return iset == Thumb; }
      
    Expr n, z, c, v; /* TODO: should handle q */
    U8 itstate;
    U32 bg;
    InstructionSet iset;
    bool bigendian;
    uint8_t mode;
  };
  
  struct PSR : public StatusRegister
  {
    typedef unisim::component::cxx::processor::arm::RegisterField<31,1> NRF; /* Negative Integer Condition Flag */
    typedef unisim::component::cxx::processor::arm::RegisterField<30,1> ZRF; /* Zero     Integer Condition Flag */
    typedef unisim::component::cxx::processor::arm::RegisterField<29,1> CRF; /* Carry    Integer Condition Flag */
    typedef unisim::component::cxx::processor::arm::RegisterField<28,1> VRF; /* Overflow Integer Condition Flag */
    //typedef unisim::component::cxx::processor::arm::RegisterField<27,1> QRF; /* Cumulative saturation flag */
      
    typedef unisim::component::cxx::processor::arm::RegisterField<28,4> NZCVRF; /* Grouped Integer Condition Flags */
      
      
    typedef unisim::component::cxx::processor::arm::RegisterField<24,1> JRF; /* Jazelle execution state bit */
    typedef unisim::component::cxx::processor::arm::RegisterField< 9,1> ERF; /* Endianness execution state */
    typedef unisim::component::cxx::processor::arm::RegisterField< 5,1> TRF; /* Thumb execution state bit */
      
    typedef unisim::component::cxx::processor::arm::RegisterField< 0,5> MRF; /* Mode field */
      
    typedef unisim::component::cxx::processor::arm::RegisterField<10,6> ITHIRF;
    typedef unisim::component::cxx::processor::arm::RegisterField<25,2> ITLORF;
      
    typedef unisim::component::cxx::processor::arm::RegisterField< 0,32> ALLRF;
      
    static uint32_t const bg_mask = 0x08ff01c0; /* Q, 23-20, GE[3:0], A, I, F, are not handled for now */
      
    PSR( StatusRegister const& ref, Processor& _proc ) : StatusRegister(ref), proc(_proc) {}
    
    bool   GetJ() const { return (iset == Jazelle) or (iset == ThumbEE); }
    bool   GetT() const { return (iset ==   Thumb) or (iset == ThumbEE); }

    template <typename RF>
    void   Set( RF const& _, U32 const& value )
    {
      unisim::util::symbolic::StaticAssert<(RF::pos > 31) or ((RF::pos + RF::size) <= 28)>::check(); // NZCV
      unisim::util::symbolic::StaticAssert<(RF::pos > 26) or ((RF::pos + RF::size) <= 24)>::check(); // ITLO, J
      unisim::util::symbolic::StaticAssert<(RF::pos > 15) or ((RF::pos + RF::size) <=  9)>::check(); // ITHI, E
      unisim::util::symbolic::StaticAssert<(RF::pos >  5) or ((RF::pos + RF::size) <=  0)>::check(); // T, MODE
        
      return _.Set( bg, value );
    }
      
    template <typename RF>
    U32    Get( RF const& _ )
    {
      unisim::util::symbolic::StaticAssert<(RF::pos > 31) or ((RF::pos + RF::size) <= 28)>::check(); // NZCV
      unisim::util::symbolic::StaticAssert<(RF::pos > 26) or ((RF::pos + RF::size) <= 24)>::check(); // ITLO, J
      unisim::util::symbolic::StaticAssert<(RF::pos > 15) or ((RF::pos + RF::size) <=  9)>::check(); // ITHI, E
      unisim::util::symbolic::StaticAssert<(RF::pos >  5) or ((RF::pos + RF::size) <=  0)>::check(); // T, MODE
        
      return _.Get( bg );
    }
      
    void   SetBits( U32 const& bits, uint32_t mask );
    U32    GetBits();
    
    void   Set( NRF const& _, BOOL const& value ) { n = value.expr; }
    void   Set( ZRF const& _, BOOL const& value ) { z = value.expr; }
    void   Set( CRF const& _, BOOL const& value ) { c = value.expr; }
    void   Set( VRF const& _, BOOL const& value ) { v = value.expr; }
    void   Set( ERF const& _, U32 const& value ) { if (proc.Cond(value != U32(bigendian))) proc.UnpredictableInsnBehaviour(); }
    void   Set( NZCVRF const& _, U32 const& value );
      
    void   SetITState( uint8_t init_val ) { itstate = U8(init_val); }
    BOOL   InITBlock() const { return (itstate & U8(0b1111)) != U8(0); }
      
    U32    Get( NRF const& _ ) { return U32(BOOL(n)); }
    U32    Get( ZRF const& _ ) { return U32(BOOL(z)); }
    U32    Get( CRF const& _ ) { return U32(BOOL(c)); }
    U32    Get( VRF const& _ ) { return U32(BOOL(v)); }
      
    /* ISetState */
    U32    Get( JRF const& _ ) { return U32(GetJ()); }
    U32    Get( TRF const& _ ) { return U32(GetT()); }
      
    /* Endianness */
    U32    Get( ERF const& _ ) { return U32(bigendian); }
    U32    Get( MRF const& _ ) { return U32(mode); }
    // U32 Get( ALL const& _ ) { return (U32(BOOL(n)) << 31) | (U32(BOOL(z)) << 30) | (U32(BOOL(c)) << 29) | (U32(BOOL(v)) << 28) | bg; }
      
    Processor& proc;
  };
  
  Processor( StatusRegister const& ref_psr, U32 const& insn_addr, unsigned insn_bytes )
    : path(0)
    , reg_values()
    , next_insn_addr( insn_addr + U32(insn_bytes) )
    , branch_type( Br::Jump )
    , cpsr( ref_psr, *this )
    , spsr( Expr( newRegRead(RegID("spsr"),32) ) )
    , sregs()
    , FPSCR( Expr( newRegRead(RegID("fpscr"),32) ) )
    , FPEXC( Expr( newRegRead(RegID("fpexc"),32) ) )
    , stores()
    , unpredictable(false)
    , is_it_assigned(false)
    , mode()
    , foreign_registers()
  {
    // GPR regs
    for (unsigned reg = 0; reg < 15; ++reg)
      reg_values[reg] = U32( Expr( newRegRead( RegID("r0") + reg, 32 ) ) );
    reg_values[15] = insn_addr + U32(ref_psr.IsThumb() ? 4 : 8 );
      
    // Special registers
    for (SRegID reg; reg.next();)
      sregs[reg.idx()] = U32( Expr( newRegRead( reg, 32 ) ) );
  }

  bool close( Processor const& ref )
  {
    bool complete = path->close();
    path->sinks.insert( Expr( new PCWrite( next_insn_addr.expr, branch_type ) ) );
    if (unpredictable)
      {
        path->sinks.insert( Expr( new unisim::util::symbolic::binsec::AssertFalse() ) );
        return complete;
      }
    if (cpsr.n != ref.cpsr.n)
      path->sinks.insert( Expr( newRegWrite( RegID("n"), cpsr.n, 1 ) ) );
    if (cpsr.z != ref.cpsr.z)
      path->sinks.insert( Expr( newRegWrite( RegID("z"), cpsr.z, 1 ) ) );
    if (cpsr.c != ref.cpsr.c)
      path->sinks.insert( Expr( newRegWrite( RegID("c"), cpsr.c, 1 ) ) );
    if (cpsr.v != ref.cpsr.v)
      path->sinks.insert( Expr( newRegWrite( RegID("v"), cpsr.v, 1 ) ) );
    if (cpsr.itstate.expr != ref.cpsr.itstate.expr)
      path->sinks.insert( Expr( newRegWrite( RegID("itstate"), cpsr.itstate.expr, 8 ) ) );
    if (cpsr.bg.expr != ref.cpsr.bg.expr)
      path->sinks.insert( Expr( newRegWrite( RegID("cpsr"), cpsr.bg.expr, 32 ) ) );
    if (spsr.expr != ref.spsr.expr)
      path->sinks.insert( Expr( newRegWrite( RegID("spsr"), spsr.expr, 32 ) ) );
    for (SRegID reg; reg.next();)
      if (sregs[reg.idx()].expr != ref.sregs[reg.idx()].expr)
        path->sinks.insert( Expr( newRegWrite( reg, sregs[reg.idx()].expr, 32 ) ) );
    if (FPSCR.expr != ref.FPSCR.expr)
      path->sinks.insert( Expr( newRegWrite( RegID("fpscr"), FPSCR.expr, 32 ) ) );
    if (FPEXC.expr != ref.FPEXC.expr)
      path->sinks.insert( Expr( newRegWrite( RegID("fpexc"), FPEXC.expr, 32 ) ) );
    for (unsigned reg = 0; reg < 15; ++reg) {
      if (reg_values[reg].expr != ref.reg_values[reg].expr)
        path->sinks.insert( Expr( newRegWrite( RegID("r0") + reg, reg_values[reg].expr, 32 ) ) );
    }
    for (ForeignRegisters::iterator itr = foreign_registers.begin(), end = foreign_registers.end(); itr != end; ++itr)
      {
        ForeignRegister ref(itr->first.first, itr->first.second);
        ref.Retain(); // Prevent deletion of this static instance
        Expr xref( new ForeignRegister(itr->first.first, itr->first.second) );
        if (itr->second == Expr(&ref)) continue;
        std::ostringstream buf;
        ref.Repr( buf );
        path->sinks.insert( Expr( newRegWrite( RegID(buf.str().c_str()), itr->second, 32 ) ) );
      }
    for (std::set<Expr>::const_iterator itr = stores.begin(), end = stores.end(); itr != end; ++itr)
      path->sinks.insert( *itr );
    return complete;
  }
  
  //   =====================================================================
  //   =                 Internal Instruction Control Flow                 =
  //   =====================================================================
  
  void UnpredictableInsnBehaviour() { unpredictable = true; }
  
  template <typename OP>
  void UndefinedInstruction( OP* op ) { not_implemented(); }
    
  template <typename T>
  bool Cond( unisim::util::symbolic::SmartValue<T> const& cond )
  {
    if (not cond.expr.good())
      throw std::logic_error( "Not a valid condition" );
      
    Expr cexp( BOOL(cond).expr );
    if (unisim::util::symbolic::ConstNodeBase const* cnode = cexp.ConstSimplify())
      return cnode->GetBoolean();
      
    bool predicate = path->proceed( cexp );
    path = path->next( predicate );
    return predicate;
  }
    
  void FPTrap( unsigned exc )
  {
    throw std::logic_error("unimplemented");
  }
    
  void not_implemented() { throw std::logic_error( "not implemented" ); }

  //   =====================================================================
  //   =             General Purpose Registers access methods              =
  //   =====================================================================
    
  U32  GetGPR( uint32_t id ) { return reg_values[id]; }
  
  // TODO: interworking branches are not correctly handled
  void SetGPR_mem( uint32_t id, U32 const& value )
  {
    if (id != 15)
      reg_values[id] = value;
    else
      SetNIA( value, B_JMP );
  }
  void SetGPR( uint32_t id, U32 const& value ) {
    if (id != 15)
      reg_values[id] = value;
    else
      SetNIA( value, B_JMP );
  }
    
  void SetGPR_usr( uint32_t id, U32 const& value ) { /* Only work in system mode instruction */ not_implemented(); }
  U32  GetGPR_usr( uint32_t id ) { /* Only work in system mode instruction */ not_implemented(); return U32(); }
    
  U32  GetNIA() { return next_insn_addr; }
  enum branch_type_t { B_JMP = 0, B_CALL, B_RET, B_EXC, B_DBG, B_RFE };
  void SetNIA( U32 const& nia, branch_type_t bt )
  {
    next_insn_addr = nia;
    branch_type = (bt == B_CALL) ? Br::Call : (bt == B_RET) ? Br::Return : Br::Jump;
  }

  Expr& GetForeignRegister( uint8_t foreign_mode, uint32_t idx )
  {
    Expr& result = foreign_registers[std::make_pair( foreign_mode, idx )];
    if (not result.node)
      result = new ForeignRegister( foreign_mode, idx );
    return result;
  }
    
  U32  GetBankedRegister( uint8_t foreign_mode, uint32_t idx )
  {
    if ((cpsr.mode == foreign_mode) or
        (idx < 8) or
        (idx >= 15) or
        ((foreign_mode != FIQ_MODE) and (cpsr.mode != FIQ_MODE) and (idx < 13))
        )
      return GetGPR( idx );
    return U32( GetForeignRegister( foreign_mode, idx ) );
  }
    
  void SetBankedRegister( uint8_t foreign_mode, uint32_t idx, U32 value )
  {
    if ((cpsr.mode == foreign_mode) or
        (idx < 8) or
        (idx >= 15) or
        ((foreign_mode != FIQ_MODE) and (cpsr.mode != FIQ_MODE) and (idx < 13))
        )
      return SetGPR( idx, value );
    GetForeignRegister( foreign_mode, idx ) = value.expr;
  }
    
  //   =====================================================================
  //   =              Special/System Registers access methods              =
  //   =====================================================================

  PSR& CPSR() { return cpsr; }
  
  U32  GetCPSR()                                 { return cpsr.GetBits(); }
  void SetCPSR( U32 const& bits, uint32_t mask ) { cpsr.SetBits( bits, mask ); }
    
  U32& SPSR() { not_implemented(); static U32 spsr_dummy; return spsr_dummy; }
  
  ITCond itcond() const { return ITCond(); }
  bool itblock() { return Cond(cpsr.InITBlock()); }
  
  void ITSetState( uint32_t cond, uint32_t mask )
  {
    cpsr.SetITState( (cond << 4) | mask );
    is_it_assigned = true;
  }
  
  void ITAdvance()
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
  
  Mode&  CurrentMode() { /* not_implemented(); */ return mode; }
  Mode&  GetMode(uint8_t) { not_implemented(); return mode; }
  
  virtual CP15Reg& CP15GetRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 );
  
  U32         CP15ReadRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 )
  { return CP15GetRegister( crn, opcode1, crm, opcode2 ).Read( *this ); }
  void        CP15WriteRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2, U32 const& value )
  { CP15GetRegister( crn, opcode1, crm, opcode2 ).Write( *this, value ); }
  char const* CP15DescribeRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 )
  { return CP15GetRegister( crn, opcode1, crm, opcode2 ).Describe(); }

  //   ====================================================================
  //   =         Vector and Floating-point Registers access methods       =
  //   ====================================================================

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
    
  // U32 StandardValuedFPSCR() const   { return AHP.Mask( FPSCR ) | 0x03000000; }
    
  U32  GetVU32( unsigned idx ) { return U32(); }
  void SetVU32( unsigned idx, U32 val ) {}
  U64  GetVU64( unsigned idx ) { return U64(); }
  void SetVU64( unsigned idx, U64 val ) {}
  F32  GetVSR( unsigned idx ) { return F32(); }
  void SetVSR( unsigned idx, F32 val ) {}
  F64  GetVDR( unsigned idx ) { return F64(); }
  void SetVDR( unsigned idx, F64 val ) {}
    
  //   =====================================================================
  //   =                      Control Transfer methods                     =
  //   =====================================================================

  void BranchExchange( U32 const& target, branch_type_t branch_type ) { SetNIA( target, branch_type ); }
  void Branch( U32 const& target, branch_type_t branch_type ) { SetNIA( target, branch_type ); }
    
  void WaitForInterrupt() { not_implemented(); }
  void SWI( uint32_t imm ) { not_implemented(); }
  void BKPT( uint32_t imm ) { not_implemented(); }
  void CallSupervisor( uint16_t imm ) { not_implemented(); }
  bool IntegerZeroDivide( BOOL const& condition ) { return false; }
  
  //   =====================================================================
  //   =                       Memory access methods                       =
  //   =====================================================================
  
  U32  MemURead32( U32 const& addr ) { return U32( Expr( new Load( addr.expr, 2, 0, false ) ) ); }
  U16  MemURead16( U32 const& addr ) { return U16( Expr( new Load( addr.expr, 1, 0, false ) ) ); }
  U32  MemRead32( U32 const& addr ) { return U32( Expr( new Load( addr.expr, 2, 2, false ) ) ); }
  U16  MemRead16( U32 const& addr ) { return U16( Expr( new Load( addr.expr, 1, 1, false ) ) ); }
  U8   MemRead8( U32 const& addr ) { return U8( Expr( new Load( addr.expr, 0, 0, false ) ) ); }
  
  void MemUWrite32( U32 const& addr, U32 const& value ) { stores.insert( new Store( addr.expr, value.expr, 2, 0, false ) ); }
  void MemUWrite16( U32 const& addr, U16 const& value ) { stores.insert( new Store( addr.expr, value.expr, 1, 0, false ) ); }
  void MemWrite32( U32 const& addr, U32 const& value ) { stores.insert( new Store( addr.expr, value.expr, 2, 2, false ) ); }
  void MemWrite16( U32 const& addr, U16 const& value ) { stores.insert( new Store( addr.expr, value.expr, 1, 1, false ) ); }
  void MemWrite8( U32 const& addr, U8 const& value ) { stores.insert( new Store( addr.expr, value.expr, 0, 0, false ) ); }
    
  void SetExclusiveMonitors( U32 const& address, unsigned size ) { std::cerr << "SetExclusiveMonitors\n"; }
  bool ExclusiveMonitorsPass( U32 const& address, unsigned size ) { std::cerr << "ExclusiveMonitorsPass\n"; return true; }
  void ClearExclusiveLocal() { std::cerr << "ClearExclusiveMonitors\n"; }
  
  //   =====================================================================
  //   =                         Processor Storage                         =
  //   =====================================================================
  
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

  struct SRegID : public unisim::util::symbolic::Identifier<SRegID>
  {
    enum Code {
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
      DIAGCR, CFGBAR, end
    } code;

    char const* c_str() const
    {
      switch (code)
        {
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
        case end:        break;
        }
      return "NA";
    }

    SRegID() : code(end) {}
    SRegID( Code _code ) : code(_code) {}
    SRegID( char const* _code ) : code(end) { init(_code); }
  };
  
  U32& SReg( SRegID reg )
  {
    if (reg.code == SRegID::end)
      throw 0;
    return sregs[reg.idx()];
  }
    
  struct RegID : public unisim::util::symbolic::Identifier<RegID>
  {
    enum Code
      {
        NA = 0,
        r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, sp, lr,
        n, z, c, v, itstate, // q, ge0, ge1, ge2, ge3,
        cpsr, spsr,
        fpscr, fpexc,
        r8_fiq,
        r9_fiq,
        sl_fiq,
        fp_fiq,
        ip_fiq,
        sp_fiq,
        lr_fiq,
        r8_usr,
        r9_usr,
        sl_usr,
        fp_usr,
        ip_usr,
        sp_usr,
        lr_usr,
        sp_irq,
        sp_svc,
        sp_abt,
        sp_hyp,
        sp_und,
        lr_irq,
        lr_svc,
        lr_abt,
        lr_hyp,
        lr_und,
        end
      } code;

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
        case          n: return "n";
        case          z: return "z";
        case          c: return "c";
        case          v: return "v";
        case    itstate: return "itstate";
        case       cpsr: return "cpsr";
        case       spsr: return "spsr";
        case      fpscr: return "fpscr";
        case      fpexc: return "fpexc";
        case     r8_fiq: return "r8_fiq";
        case     r9_fiq: return "r9_fiq";
        case     sl_fiq: return "sl_fiq";
        case     fp_fiq: return "fp_fiq";
        case     ip_fiq: return "ip_fiq";
        case     sp_fiq: return "sp_fiq";
        case     lr_fiq: return "lr_fiq";
        case     r8_usr: return "r8_usr";
        case     r9_usr: return "r9_usr";
        case     sl_usr: return "sl_usr";
        case     fp_usr: return "fp_usr";
        case     ip_usr: return "ip_usr";
        case     sp_usr: return "sp_usr";
        case     lr_usr: return "lr_usr";
        case     sp_irq: return "sp_irq";
        case     sp_svc: return "sp_svc";
        case     sp_abt: return "sp_abt";
        case     sp_hyp: return "sp_hyp";
        case     sp_und: return "sp_und";
        case     lr_irq: return "lr_irq";
        case     lr_svc: return "lr_svc";
        case     lr_abt: return "lr_abt";
        case     lr_hyp: return "lr_hyp";
        case     lr_und: return "lr_und";
        case         NA:
        case        end: break;
        }
      return "NA";
    }
      
    RegID() : code(end) {}
    RegID( Code _code ) : code(_code) {}
    RegID( char const* _code ) : code(end) { init( _code ); }
  };
    
  ActionNode*      path;
  U32              reg_values[16];
  U32              next_insn_addr;
  Br::type_t       branch_type;
  PSR              cpsr;
  U32              spsr;
  U32              sregs[SRegID::end];
  U32              FPSCR, FPEXC;
  std::set<Expr>   stores;
  bool             unpredictable;
  bool             is_it_assigned; /* determines wether current instruction is an IT one. */
  Mode             mode;
  ForeignRegisters foreign_registers;
};

Processor::BOOL CheckCondition( Processor& state, unsigned cond )
{
  Processor::BOOL N = state.cpsr.n, Z = state.cpsr.z, C = state.cpsr.c, V = state.cpsr.v;

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

Processor::BOOL CheckCondition( Processor& state, Processor::ITCond const& cond )
{
  typedef Processor::BOOL BOOL;
  typedef Processor::U8   U8;
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

void UpdateStatusSub( Processor& state, Processor::U32 const& res, Processor::U32 const& lhs, Processor::U32 const& rhs )
{
  typedef Processor::S32 S32;
  typedef Processor::U32 U32;
  Processor::BOOL neg = S32(res) < S32(0);
  state.cpsr.n = neg.expr;
  state.cpsr.z = ( res == U32(0) ).expr;
  state.cpsr.c = ( lhs >= rhs ).expr;
  state.cpsr.v = ( neg xor (S32(lhs) < S32(rhs)) ).expr;
}

void UpdateStatusSubWithBorrow( Processor& state, Processor::U32 const& res, Processor::U32 const& lhs, Processor::U32 const& rhs, Processor::U32 const& borrow )
{
  typedef Processor::S32 S32;
  typedef Processor::U32 U32;
  Processor::BOOL neg = S32(res) < S32(0);
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

void UpdateStatusAdd( Processor& state, Processor::U32 const& res, Processor::U32 const& lhs, Processor::U32 const& rhs )
{
  typedef Processor::S32 S32;
  typedef Processor::U32 U32;
  Processor::BOOL neg = S32(res) < S32(0);
  state.cpsr.n = neg.expr;
  state.cpsr.z = ( res == U32(0) ).expr;
  state.cpsr.c = ( lhs >  ~rhs ).expr;
  state.cpsr.v = ( neg xor (S32(lhs) <= S32(~rhs)) ).expr;
}

void UpdateStatusAddWithCarry( Processor& state, Processor::U32 const& res, Processor::U32 const& lhs, Processor::U32 const& rhs, Processor::U32 const& carry )
{
  typedef Processor::S32 S32;
  typedef Processor::U32 U32;
  Processor::BOOL neg = S32(res) < S32(0);
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


struct ARMISA : public unisim::component::cxx::processor::arm::isa::arm32::Decoder<Processor>
{
  typedef unisim::component::cxx::processor::arm::isa::arm32::CodeType CodeType;
  typedef unisim::component::cxx::processor::arm::isa::arm32::Operation<Processor> Operation;
  static CodeType mkcode( uint32_t code ) { return CodeType( code ); }
  static bool const is_thumb = false;
};

struct THUMBISA : public unisim::component::cxx::processor::arm::isa::thumb2::Decoder<Processor>
{
  typedef unisim::component::cxx::processor::arm::isa::thumb2::CodeType CodeType;
  typedef unisim::component::cxx::processor::arm::isa::thumb2::Operation<Processor> Operation;
  static CodeType mkcode( uint32_t code ) { return CodeType( code ); }
  static bool const is_thumb = true;
};

struct InstructionAddress : public unisim::util::symbolic::binsec::ASExprNode
{
  InstructionAddress() {}
  virtual void Repr( std::ostream& sink ) const { sink << "insn_addr"; }
  virtual intptr_t cmp( unisim::util::symbolic::ExprNode const& brhs ) const { dynamic_cast<InstructionAddress const&>( brhs ); return 0; }
};

struct Translator
{
  typedef unisim::util::symbolic::ActionNode ActionNode;
  typedef Processor::StatusRegister StatusRegister;
  
  Translator( uint32_t _addr, uint32_t _code )
    : addr(_addr), code(_code), coderoot(new ActionNode)
  {}
  ~Translator() { delete coderoot; }
  
  template <class ISA>
  void
  extract( std::ostream& sink, ISA& isa )
  {
    sink << "(address . " << unisim::util::symbolic::binsec::dbx(4, addr) << ")\n";
  
    // Instruction decoding
    struct Instruction
    {
      typedef typename ISA::Operation Operation;
      Instruction(ISA& isa, uint32_t addr, uint32_t code)
        : operation(0), bytecount(0)
      {
        try {
          operation = isa.NCDecode( addr, ISA::mkcode( code ) );
          unsigned bitlength = operation->GetLength(); 
          if ((bitlength != 32) and ((bitlength != 16) or not ISA::is_thumb))
            { delete operation; operation = 0; }
          bytecount = bitlength/8;
        }
        catch (unisim::component::cxx::processor::arm::isa::Reject const&)
          { operation = 0; }
      }
      ~Instruction() { delete operation; }
      Operation* operator -> () { return operation; }
      
      Operation* operation;
      unsigned   bytecount;
    };
    
    Instruction instruction( isa, addr, code );
    
    if (not instruction.operation)
      {
        sink << "(opcode . " << unisim::util::symbolic::binsec::dbx(4, code) << ")\n(illegal)\n";
        return;
      }

    {
      uint32_t encoding = instruction->GetEncoding();
      if (instruction.bytecount == 2)
        encoding &= 0xffff;
      
      sink << "(opcode . " << unisim::util::symbolic::binsec::dbx(instruction.bytecount, encoding) << ")\n(size . " << (instruction.bytecount) << ")\n";
    }
    
    Processor::U32      insn_addr = unisim::util::symbolic::make_const(addr); //< concrete instruction address
    // Processor::U32      insn_addr = Expr(new InstructionAddress); //< symbolic instruction address
    Processor reference( status, insn_addr, instruction.bytecount );
    
    // Disassemble
    sink << "(mnemonic . \"";
    try { instruction->disasm( reference, sink ); }
    catch (...) { sink << "(bad)"; }
    sink << "\")\n";
    
    // Get actions
    try
      {
        for (bool end = false; not end;)
          {
            Processor state( reference );
            state.path = coderoot;
            instruction->execute( state );
            if (state.cpsr.IsThumb())
              state.ITAdvance();
            end = state.close( reference );
          }
        coderoot->factorize();
        coderoot->remove_dead_paths();
      }
    catch (...)
      {
        sink << "(unimplemented)\n";
        return;
      }
  }

  void translate( std::ostream& sink )
  {
    if      (status.iset == status.Arm)
      {
        ARMISA armisa;
        extract( sink, armisa );
      }
    else if (status.iset == status.Thumb)
      {
        THUMBISA thumbisa;
        extract( sink, thumbisa );
      }
    else
      throw 0;

    // Translate to DBA
    unisim::util::symbolic::binsec::Program program;
    program.Generate( coderoot );
    typedef unisim::util::symbolic::binsec::Program::const_iterator Iterator;
    for (Iterator itr = program.begin(), end = program.end(); itr != end; ++itr)
      sink << "(" << unisim::util::symbolic::binsec::dbx(4, addr) << ',' << itr->first << ") " << itr->second << std::endl;
  }

  Processor::StatusRegister status;
  uint32_t    addr, code;
  ActionNode* coderoot;
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

  Translator actset( addr, code );
  Translator::StatusRegister& status = actset.status;
  
  std::string exec_flags(argv[1]);
  exec_flags += ',';
  for (std::string::iterator cur = exec_flags.begin(), end = exec_flags.end(), nxt; cur != end; cur = nxt+1 )
    {
      nxt = std::find( cur, end, ',' );
      std::string flag( cur, nxt );
      if (flag.size() == 0)
        continue;
      if      (flag == "arm")        { status.iset = status.Arm; }
      else if (flag == "thumb")      { status.iset = status.Thumb; }
      else if (flag == "little")     { status.bigendian = false; }
      else if (flag == "big")        { status.bigendian = true; }
      else if (flag == "user")       { status.mode = Processor::USER_MODE; }
      else if (flag == "fiq")        { status.mode = Processor::FIQ_MODE; }
      else if (flag == "irq")        { status.mode = Processor::IRQ_MODE; }
      else if (flag == "supervisor") { status.mode = Processor::SUPERVISOR_MODE; }
      else if (flag == "monitor")    { status.mode = Processor::MONITOR_MODE; }
      else if (flag == "abort")      { status.mode = Processor::ABORT_MODE; }
      else if (flag == "hypervisor") { status.mode = Processor::HYPERVISOR_MODE; }
      else if (flag == "undefined")  { status.mode = Processor::UNDEFINED_MODE; }
      else if (flag == "system")     { status.mode = Processor::SYSTEM_MODE; }
      else if (flag == "outitb")     { status.FixITState(0); }
      else
        {
          std::cerr << "Unknown execution state flag: " << flag << std::endl;
          return 1;
        }

    }

  actset.translate( std::cout );
  
  return 0;
}

Processor::CP15Reg&
Processor::CP15GetRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 )
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
          U32 Read( Processor& proc ) { return proc.SReg("ctr"); }
        } x;
        return x;
      } break;
          
    case CP15ENCODE( 0, 0, 0, 5 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "MPIDR, Multiprocessor Affinity Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("mpidr"); }
        } x;
        return x;
      } break;
          
    case CP15ENCODE( 0, 0, 1, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "ID_PFR0, Processor Feature Register 0"; }
          U32 Read( Processor& proc ) { return proc.SReg("id_pfr0"); }
        } x;
        return x;
      } break;
          
    case CP15ENCODE( 0, 1, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "CCSIDR, Cache Size ID Registers"; }
          U32 Read( Processor& proc ) { return proc.SReg("ccsidr"); }
        } x;
        return x;
      } break;

    case CP15ENCODE( 0, 1, 0, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "CLIDR, Cache Level ID Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("clidr"); }
        } x;
        return x;
      } break;

    case CP15ENCODE( 0, 2, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "CSSELR, Cache Size Selection Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("csselr"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("csselr") = value; }
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
          U32 Read( Processor& proc ) { return proc.SReg("sctlr"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("sctlr") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 1, 0, 0, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "ACTLR, Auxiliary Control Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("actlr"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("actlr") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 1, 0, 0, 2 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "CPACR, Coprocessor Access Control Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("cpacr"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("cpacr") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 1, 0, 1, 2 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "NSACR, Non-Secure Access Control Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("nsacr"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("nsacr") = value; }
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
          void Write( Processor& proc, U32 const& value ) { proc.SReg("ttbr0") = value; }
          U32 Read( Processor& proc ) { return proc.SReg("ttbr0"); }
        } x;
        return x;
      } break;

    case CP15ENCODE( 2, 0, 0, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "TTBR1, Translation Table Base Register 1"; }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("ttbr1") = value; }
          U32 Read( Processor& proc ) { return proc.SReg("ttbr1"); }
        } x;
        return x;
      } break;

    case CP15ENCODE( 2, 0, 0, 2 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "TTBCR, Translation Table Base Control Register"; }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("ttbcr") = value; }
          U32 Read( Processor& proc ) { return proc.SReg("ttbcr"); }
        } x;
        return x;
      } break;

    case CP15ENCODE( 3, 0, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DACR, Domain Access Control Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("dacr"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("dacr") = value; }
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
          U32 Read( Processor& proc ) { return proc.SReg("dfsr"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("dfsr") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 5, 0, 0, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "IFSR, Instruction Fault Status Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("ifsr"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("ifsr") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 6, 0, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DFAR, Data Fault Status Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("dfar"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("dfar") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 6, 0, 0, 2 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "IFAR, Instruction Fault Status Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("ifar"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("ifar") = value; }
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
          U32 Read( Processor& proc ) { return proc.SReg("icialluis"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("icialluis") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 7, 0, 1, 6 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "BPIALLIS, Invalidate all branch predictors Inner Shareable"; }
          U32 Read( Processor& proc ) { return proc.SReg("bpiallis"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("bpiallis") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 7, 0, 5, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "ICIALLU, Invalidate all instruction caches to PoU"; }
          U32 Read( Processor& proc ) { return proc.SReg("iciallu"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("iciallu") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 7, 0, 5, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "ICIMVAU, Clean data* cache line by MVA to PoU"; }
          U32 Read( Processor& proc ) { return proc.SReg("icimvau"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("icimvau") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 7, 0, 5, 6 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "BPIALL, Invalidate all branch predictors"; }
          U32 Read( Processor& proc ) { return proc.SReg("bpiall"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("bpiall") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 7, 0, 6, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DCIMVAC, Invalidate data* cache line by MVA to PoC"; }
          U32 Read( Processor& proc ) { return proc.SReg("dcimvac"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("dcimvac") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 7, 0, 6, 2 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DCISW, Invalidate data* cache line by set/way"; }
          U32 Read( Processor& proc ) { return proc.SReg("dcisw"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("dcisw") = value; }
        } x;
        return x;
      } break;
          
    case CP15ENCODE( 7, 0, 10, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DCCMVAC, Clean data* cache line by MVA to PoC"; }
          U32 Read( Processor& proc ) { return proc.SReg("dccmvac"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("dccmvac") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 7, 0, 10, 2 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DCCSW, Clean data* cache line by set/way"; }
          U32 Read( Processor& proc ) { return proc.SReg("dccsw"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("dccsw") = value; }
        } x;
        return x;
      } break;
          
    case CP15ENCODE( 7, 0, 11, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DCCMVAU, Clean data* cache line by MVA to PoU"; }
          U32 Read( Processor& proc ) { return proc.SReg("dccmvau"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("dccmvau") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 7, 0, 14, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DCCIMVAC, Clean and invalidate data* cache line by MVA to PoC"; }
          U32 Read( Processor& proc ) { return proc.SReg("dccimvac"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("dccimvac") = value; }
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
          U32 Read( Processor& proc ) { return proc.SReg("tlbiallis"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("tlbiallis") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 8, 0, 7, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "TLBIALL, invalidate unified TLB"; }
          U32 Read( Processor& proc ) { return proc.SReg("tlbiall"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("tlbiall") = value; }
        } x;
        return x;
      } break;

    case CP15ENCODE( 8, 0, 7, 2 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "TLBIASID, invalidate unified TLB by ASID match"; }
          U32 Read( Processor& proc ) { return proc.SReg("tlbiasid"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("tlbiasid") = value; }
        } x;
        return x;
      } break;
          
    case CP15ENCODE( 12, 0, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "VBAR, Vector Base Address Register"; }
          U32 Read( Processor& proc ) { return proc.SReg("vbar"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("vbar") = value; }
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
          U32 Read( Processor& proc ) { return proc.SReg("contextidr"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("contextidr") = value; }
        } x;
        return x;
      } break;

      /* BOARD specific */
          
    case CP15ENCODE( 15, 0, 0, 1 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "DIAGCR, undocumented Diagnostic Control register"; }
          U32 Read( Processor& proc ) { return proc.SReg("diagcr"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("diagcr") = value; }
        } x;
        return x;
      } break;
          
    case CP15ENCODE( 15, 4, 0, 0 ):
      {
        static struct : public CP15Reg
        {
          char const* Describe() { return "CFGBAR, Configuration Base Address"; }
          U32 Read( Processor& proc ) { return proc.SReg("cfgbar"); }
          void Write( Processor& proc, U32 const& value ) { proc.SReg("cfgbar") = value; }
        } x;
        return x;
      } break;

    }

  static struct CP15Error : public CP15Reg {
    char const* Describe() { return "Unknown CP15 register"; }
  } err;
  return err;
}

void
Processor::PSR::Set( NZCVRF const& _, U32 const& value )
{
  n = BOOL( unisim::component::cxx::processor::arm::RegisterField< 3,1>().Get( value ) ).expr;
  z = BOOL( unisim::component::cxx::processor::arm::RegisterField< 2,1>().Get( value ) ).expr;
  c = BOOL( unisim::component::cxx::processor::arm::RegisterField< 1,1>().Get( value ) ).expr;
  v = BOOL( unisim::component::cxx::processor::arm::RegisterField< 0,1>().Get( value ) ).expr;
}

Processor::U32
Processor::PSR::GetBits()
{
  return
    (U32(BOOL(n)) << 31) |
    (U32(BOOL(z)) << 30) |
    (U32(BOOL(c)) << 29) |
    (U32(BOOL(v)) << 28) |
    (U32(itstate >> U8(2)) << 10) | (U32(itstate & U8(0b11)) << 25) |
    U32((uint32_t(GetJ()) << 24) | (uint32_t(GetT()) << 5) | uint32_t(mode)) |
    bg;
}
      
void
Processor::PSR::SetBits( U32 const& bits, uint32_t mask )
{
  if (NRF().Get(mask)) { n = BOOL( NRF().Get(bits) ).expr; NRF().Set(mask, 0u); }
  if (ZRF().Get(mask)) { z = BOOL( ZRF().Get(bits) ).expr; ZRF().Set(mask, 0u); }
  if (CRF().Get(mask)) { c = BOOL( CRF().Get(bits) ).expr; CRF().Set(mask, 0u); }
  if (VRF().Get(mask)) { v = BOOL( VRF().Get(bits) ).expr; VRF().Set(mask, 0u); }
        
  if (ITHIRF().Get(mask) or ITLORF().Get(mask))
    {
      itstate = U8((ITHIRF().Get(bits) << 2) | ITLORF().Get(bits));
      uint32_t itmask = ITHIRF().getmask<uint32_t>() | ITLORF().getmask<uint32_t>();
      if ((mask & itmask) != itmask)
        throw 0;
      mask &= ~itmask;
      ITHIRF().Set(mask, 0u); ITLORF().Set(mask, 0u);
    }
        
  if (MRF().Get(mask))
    {
      if (MRF().Get(mask) != 0x1f)
        throw 0;
      U32       nmode = MRF().Get(bits);
      MRF().Set(mask, 0u);
      if (proc.Cond(nmode != U32(mode)))
        proc.UnpredictableInsnBehaviour();
    }
        
  if (JRF().Get(mask)) { if (proc.Cond(JRF().Get(bits) != U32(GetJ())))    { proc.UnpredictableInsnBehaviour(); } JRF().Set(mask, 0u); }
  if (TRF().Get(mask)) { if (proc.Cond(TRF().Get(bits) != U32(GetT())))    { proc.UnpredictableInsnBehaviour(); } TRF().Set(mask, 0u); }
  if (ERF().Get(mask)) { if (proc.Cond(ERF().Get(bits) != U32(bigendian))) { proc.UnpredictableInsnBehaviour(); } ERF().Set(mask, 0u); }
        
  bg = (bg & U32(~mask)) | (bits & U32(mask));
}
