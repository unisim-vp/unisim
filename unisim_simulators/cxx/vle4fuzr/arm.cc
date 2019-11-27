/*
 *  Copyright (c) 2019,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *  
 * FUZR RENAULT CEA FILE
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr), Gilles Mouchard <gilles.mouchard@cea.fr>
 */

#include "arm.hh"
#include <unisim/component/cxx/processor/arm/isa_arm32.tcc>
#include <unisim/component/cxx/processor/arm/isa_thumb.tcc>
#include <unisim/component/cxx/processor/arm/cpu.tcc>

ArmProcessor::ArmProcessor( char const* name, bool is_thumb )
  : unisim::kernel::Object( name, 0 )
  , Processor()
  , CPU( name, 0 )
  , bblock(false)
{
  cpsr.Set( unisim::component::cxx::processor::arm::T, uint32_t(is_thumb) );
}

ArmProcessor::~ArmProcessor()
{
}

Processor::RegView const*
ArmProcessor::get_reg(int regid)
{
  switch (regid)
    {
    case 0: /* UC_ARM_REG_INVALID */
      break;
    case 1: /* UC_ARM_REG_APSR */
      static struct : public RegView
      {
        static uint32_t apsr_mask() { return 0xf80f0000; }
        virtual void write( Processor& proc, int id, uint8_t const* bytes ) const { Self(proc).SetCPSR(*(uint32_t*)bytes, apsr_mask()); }
        virtual void read( Processor& proc, int id, uint8_t* bytes ) const { *(uint32_t*)bytes = Self(proc).GetCPSR() & apsr_mask(); }
      } _;
      return &_;
      break;
    case 2: /* UC_ARM_REG_APSR_NZCV */
      break;
    case 3: /* UC_ARM_REG_CPSR */
      break;
    case 4: /* UC_ARM_REG_FPEXC */
      break;
    case 5: /* UC_ARM_REG_FPINST */
      break;
    case 6: /* UC_ARM_REG_FPSCR */
      break;
    case 7: /* UC_ARM_REG_FPSCR_NZCV */
      break;
    case 8: /* UC_ARM_REG_FPSID */
      break;
    case 9: /* UC_ARM_REG_ITSTATE */
      break;
    case 10: /* UC_ARM_REG_LR, UC_ARM_REG_R14 */
    case 11: /* UC_ARM_REG_PC, UC_ARM_REG_R15 */
      {
        static struct : public RegView
        {
          void write( Processor& proc, int id, uint8_t const* bytes ) const { Self(proc).SetGPR(id-10+14, *(uint32_t*)bytes); }
          void read( Processor& proc, int id, uint8_t* bytes ) const { *(uint32_t*)bytes = Self(proc).GetGPR(id-10+14); }
        } _;
        return &_;
      } break;
    case 12: /* UC_ARM_REG_SP, UC_ARM_REG_R13 */
      {
        static struct : public RegView
        {
          void write( Processor& proc, int id, uint8_t const* bytes ) const { Self(proc).SetGPR(13, *(uint32_t*)bytes); }
          void read( Processor& proc, int id, uint8_t* bytes ) const { *(uint32_t*)bytes = Self(proc).GetGPR(13); }
        } _;
        return &_;
      } break;
    case 13: /* UC_ARM_REG_SPSR */
      break;
    case 14: /* UC_ARM_REG_D0 */
      break;
    case 15: /* UC_ARM_REG_D1 */
      break;
    case 16: /* UC_ARM_REG_D2 */
      break;
    case 17: /* UC_ARM_REG_D3 */
      break;
    case 18: /* UC_ARM_REG_D4 */
      break;
    case 19: /* UC_ARM_REG_D5 */
      break;
    case 20: /* UC_ARM_REG_D6 */
      break;
    case 21: /* UC_ARM_REG_D7 */
      break;
    case 22: /* UC_ARM_REG_D8 */
      break;
    case 23: /* UC_ARM_REG_D9 */
      break;
    case 24: /* UC_ARM_REG_D10 */
      break;
    case 25: /* UC_ARM_REG_D11 */
      break;
    case 26: /* UC_ARM_REG_D12 */
      break;
    case 27: /* UC_ARM_REG_D13 */
      break;
    case 28: /* UC_ARM_REG_D14 */
      break;
    case 29: /* UC_ARM_REG_D15 */
      break;
    case 30: /* UC_ARM_REG_D16 */
      break;
    case 31: /* UC_ARM_REG_D17 */
      break;
    case 32: /* UC_ARM_REG_D18 */
      break;
    case 33: /* UC_ARM_REG_D19 */
      break;
    case 34: /* UC_ARM_REG_D20 */
      break;
    case 35: /* UC_ARM_REG_D21 */
      break;
    case 36: /* UC_ARM_REG_D22 */
      break;
    case 37: /* UC_ARM_REG_D23 */
      break;
    case 38: /* UC_ARM_REG_D24 */
      break;
    case 39: /* UC_ARM_REG_D25 */
      break;
    case 40: /* UC_ARM_REG_D26 */
      break;
    case 41: /* UC_ARM_REG_D27 */
      break;
    case 42: /* UC_ARM_REG_D28 */
      break;
    case 43: /* UC_ARM_REG_D29 */
      break;
    case 44: /* UC_ARM_REG_D30 */
      break;
    case 45: /* UC_ARM_REG_D31 */
      break;
    case 46: /* UC_ARM_REG_FPINST2 */
      break;
    case 47: /* UC_ARM_REG_MVFR0 */
      break;
    case 48: /* UC_ARM_REG_MVFR1 */
      break;
    case 49: /* UC_ARM_REG_MVFR2 */
      break;
    case 50: /* UC_ARM_REG_Q0 */
      break;
    case 51: /* UC_ARM_REG_Q1 */
      break;
    case 52: /* UC_ARM_REG_Q2 */
      break;
    case 53: /* UC_ARM_REG_Q3 */
      break;
    case 54: /* UC_ARM_REG_Q4 */
      break;
    case 55: /* UC_ARM_REG_Q5 */
      break;
    case 56: /* UC_ARM_REG_Q6 */
      break;
    case 57: /* UC_ARM_REG_Q7 */
      break;
    case 58: /* UC_ARM_REG_Q8 */
      break;
    case 59: /* UC_ARM_REG_Q9 */
      break;
    case 60: /* UC_ARM_REG_Q10 */
      break;
    case 61: /* UC_ARM_REG_Q11 */
      break;
    case 62: /* UC_ARM_REG_Q12 */
      break;
    case 63: /* UC_ARM_REG_Q13 */
      break;
    case 64: /* UC_ARM_REG_Q14 */
      break;
    case 65: /* UC_ARM_REG_Q15 */
      break;
    case 66: case 67: case 68: case 69: case 70: case 71: case 72: case 73: case 74: case 75: case 76: case 77: case 78:
      /* UC_ARM_REG_R0,  UC_ARM_REG_R1,  UC_ARM_REG_R2,  UC_ARM_REG_R3,  UC_ARM_REG_R4,  UC_ARM_REG_R5, UC_ARM_REG_R6, UC_ARM_REG_R7,
       * UC_ARM_REG_R8,  UC_ARM_REG_R9,  UC_ARM_REG_R10, UC_ARM_REG_R11, UC_ARM_REG_R12,
       * UC_ARM_REG_SB=R9, UC_ARM_REG_SL=R10, UC_ARM_REG_FP=R11 */
      {
        static struct : public RegView
        {
          void write( Processor& proc, int id, uint8_t const* bytes ) const { Self(proc).SetGPR(id - 66, *(uint32_t*)bytes); }
          void read( Processor& proc, int id, uint8_t* bytes ) const { *(uint32_t*)bytes = Self(proc).GetGPR(id - 66); }
        } _;
        return &_;
      } break;
    case 79: /* UC_ARM_REG_S0 */
      break;
    case 80: /* UC_ARM_REG_S1 */
      break;
    case 81: /* UC_ARM_REG_S2 */
      break;
    case 82: /* UC_ARM_REG_S3 */
      break;
    case 83: /* UC_ARM_REG_S4 */
      break;
    case 84: /* UC_ARM_REG_S5 */
      break;
    case 85: /* UC_ARM_REG_S6 */
      break;
    case 86: /* UC_ARM_REG_S7 */
      break;
    case 87: /* UC_ARM_REG_S8 */
      break;
    case 88: /* UC_ARM_REG_S9 */
      break;
    case 89: /* UC_ARM_REG_S10 */
      break;
    case 90: /* UC_ARM_REG_S11 */
      break;
    case 91: /* UC_ARM_REG_S12 */
      break;
    case 92: /* UC_ARM_REG_S13 */
      break;
    case 93: /* UC_ARM_REG_S14 */
      break;
    case 94: /* UC_ARM_REG_S15 */
      break;
    case 95: /* UC_ARM_REG_S16 */
      break;
    case 96: /* UC_ARM_REG_S17 */
      break;
    case 97: /* UC_ARM_REG_S18 */
      break;
    case 98: /* UC_ARM_REG_S19 */
      break;
    case 99: /* UC_ARM_REG_S20 */
      break;
    case 100: /* UC_ARM_REG_S21 */
      break;
    case 101: /* UC_ARM_REG_S22 */
      break;
    case 102: /* UC_ARM_REG_S23 */
      break;
    case 103: /* UC_ARM_REG_S24 */
      break;
    case 104: /* UC_ARM_REG_S25 */
      break;
    case 105: /* UC_ARM_REG_S26 */
      break;
    case 106: /* UC_ARM_REG_S27 */
      break;
    case 107: /* UC_ARM_REG_S28 */
      break;
    case 108: /* UC_ARM_REG_S29 */
      break;
    case 109: /* UC_ARM_REG_S30 */
      break;
    case 110: /* UC_ARM_REG_S31 */
      break;
    case 111: /* UC_ARM_REG_C1_C0_2 */
      break;
    case 112: /* UC_ARM_REG_C13_C0_2 */
      break;
    case 113: /* UC_ARM_REG_C13_C0_3 */
      break;
    case 114: /* UC_ARM_REG_ENDING */
      break;
    }
  std::cerr << "Register: " << regid << " not found.\n";
  return 0;
}

namespace x
{
  template <typename Bool> struct AssertBool {};
  template <>              struct AssertBool<bool> { static void check() {} };

  template <typename VALUE_TYPE>
  struct XValue
  {
    typedef VALUE_TYPE value_type;
    typedef XValue<value_type> this_type;

    XValue() : value(), determined(false) {}
    XValue( value_type _value, bool _determined ) : value(_value), determined(_determined) {}
    explicit XValue( value_type _value ) : value(_value), determined(true) {}

    template <typename SRC_VALUE_TYPE>  explicit XValue( XValue<SRC_VALUE_TYPE> const& other ) : value(other.value), determined(other.determined) {}

    template <typename SHT> this_type operator << ( SHT sh ) const { return this_type( value << sh, determined ); }
    template <typename SHT> this_type operator >> ( SHT sh ) const { return this_type( value >> sh, determined ); }
    template <typename SHT> this_type& operator <<= ( SHT sh ) { value <<= sh; return *this; }
    template <typename SHT> this_type& operator >>= ( SHT sh ) { value >>= sh; return *this; }
    template <typename SHT> this_type operator << ( XValue<SHT> const& sh ) const { return this_type( value << sh.value, determined and sh.determined ); }
    template <typename SHT> this_type operator >> ( XValue<SHT> const& sh ) const { return this_type( value >> sh.value, determined and sh.determined ); }

    this_type operator - () const { return this_type( -value, determined ); }
    this_type operator ~ () const { return this_type( ~value, determined ); }

    this_type& operator += ( this_type const& other ) { value += other.value; determined &= other.determined; return *this; }
    this_type& operator -= ( this_type const& other ) { value -= other.value; determined &= other.determined; return *this; }
    this_type& operator *= ( this_type const& other ) { value *= other.value; determined &= other.determined; return *this; }
    this_type& operator /= ( this_type const& other ) { value /= other.value; determined &= other.determined; return *this; }
    this_type& operator %= ( this_type const& other ) { value %= other.value; determined &= other.determined; return *this; }
    this_type& operator ^= ( this_type const& other ) { value ^= other.value; determined &= other.determined; return *this; }
    this_type& operator &= ( this_type const& other ) { value &= other.value; determined &= other.determined; return *this; }
    this_type& operator |= ( this_type const& other ) { value |= other.value; determined &= other.determined; return *this; }

    this_type operator + ( this_type const& other ) const { return this_type( value + other.value, determined and other.determined ); }
    this_type operator - ( this_type const& other ) const { return this_type( value - other.value, determined and other.determined ); }
    this_type operator * ( this_type const& other ) const { return this_type( value * other.value, determined and other.determined ); }
    this_type operator / ( this_type const& other ) const { return this_type( value / other.value, determined and other.determined ); }
    this_type operator % ( this_type const& other ) const { return this_type( value % other.value, determined and other.determined ); }
    this_type operator ^ ( this_type const& other ) const { return this_type( value ^ other.value, determined and other.determined ); }
    this_type operator & ( this_type const& other ) const { return this_type( value & other.value, determined and other.determined ); }
    this_type operator | ( this_type const& other ) const { return this_type( value | other.value, determined and other.determined ); }
  
    XValue<bool> operator == ( this_type const& other ) const { return XValue<bool>( value == other.value, determined and other.determined ); }
    XValue<bool> operator != ( this_type const& other ) const { return XValue<bool>( value != other.value, determined and other.determined ); }
    XValue<bool> operator <= ( this_type const& other ) const { return XValue<bool>( value <= other.value, determined and other.determined ); }
    XValue<bool> operator >= ( this_type const& other ) const { return XValue<bool>( value >= other.value, determined and other.determined ); }
    XValue<bool> operator < ( this_type const& other ) const  { return XValue<bool>( value < other.value, determined and other.determined ); }
    XValue<bool> operator > ( this_type const& other ) const  { return XValue<bool>( value > other.value, determined and other.determined ); }

    XValue<bool> operator ! () const { AssertBool<value_type>::check(); return XValue<bool>( not value, determined ); }

    XValue<bool> operator && ( XValue<bool> const& other ) const { AssertBool<value_type>::check(); return XValue<bool>( value and other.value, determined and other.determined ); }

    XValue<bool> operator || ( XValue<bool> const& other ) const { AssertBool<value_type>::check(); return XValue<bool>( value or other.value, determined and other.determined ); }
  
    value_type value;
    bool determined;
  };

  template <typename T, class F>
  XValue<T> XApply( F const& f, XValue<T> const& l, XValue<T> const& r ) { return XValue<T>( f(l.value, r.value), l.determined and r.determined ); }
  
  template <typename T, class F>
  XValue<T> XApply( F const& f, XValue<T> const& v ) { return XValue<T>( f(v.value), v.determined ); }
  
  template <typename T> XValue<T> Minimum( XValue<T> const& l, XValue<T> const& r ) { return XApply( std::min, l, r ); }
  template <typename T> XValue<T> Maximum( XValue<T> const& l, XValue<T> const& r ) { return XApply( std::max, l, r ); }

  template <typename UTP> UTP ByteSwap( UTP const& v ) { return UTP( unisim::util::endian::ByteSwap( v.value ), v.determined ); }
  template <typename UTP> UTP BitScanReverse( UTP const& v ) { return UTP( unisim::util::arithmetic::BitScanReverse( v.value ), v.determined ); }

  template <typename UTP, typename STP>
  UTP RotateRight( UTP const& v, STP const& _s ) { XValue<uint8_t> s(_s); return UTP( unisim::util::arithmetic::RotateRight( v.value, s.value ), v.determined and s.determined ); }

  // template <typename UTP>
  // UTP RotateLeft( UTP const& value, uint8_t sh ) { return UTP( make_operation( "Rol", value.expr, make_const<uint8_t>(sh) ) ); }
  // template <typename UTP, typename STP>
  // UTP RotateLeft( UTP const& value, STP const& sh ) { return UTP( make_operation( "Rol", value.expr, XValue<uint8_t>(sh).expr ) ); }

  // template <typename UTP>
  // UTP BitScanReverse( UTP const& value ) { return UTP( make_operation( "BSR", value.expr ) ); }

  // template <typename UTP>
  // UTP BitScanForward( UTP const& value ) { return UTP( make_operation( "BSF", value.expr ) ); }

  // template <typename T>
  // XValue<T> power( XValue<T> const& left, XValue<T> const& right ) { return XValue<T>( make_operation( "FPow", left.expr, right.expr ) ); }

  // template <typename T>
  // XValue<T> fmodulo( XValue<T> const& left, XValue<T> const& right ) { return XValue<T>( make_operation( "FMod", left.expr, right.expr ) ); }
}

struct ActionNode : public unisim::util::symbolic::Choice<ActionNode> {};

struct ArmBranch
{
  typedef ArmProcessor::Config Config;
  
  typedef x::XValue<double>   F64;
  typedef x::XValue<float>    F32;
  typedef x::XValue<bool>     BOOL;
  typedef x::XValue<uint8_t>  U8;
  typedef x::XValue<uint16_t> U16;
  typedef x::XValue<uint32_t> U32;
  typedef x::XValue<uint64_t> U64;
  typedef x::XValue<int8_t>   S8;
  typedef x::XValue<int16_t>  S16;
  typedef x::XValue<int32_t>  S32;
  typedef x::XValue<int64_t>  S64;

  ArmBranch( ActionNode& root, uint32_t addr, uint32_t length, bool _thumb )
    : path(&root), r15(addr + (_thumb?4:8)), insn_addr(addr), next_insn_addr(addr+length), thumb(_thumb), has_branch(false)
  {
  }
  
  enum branch_type_t { B_JMP = 0, B_CALL, B_RET, B_EXC, B_DBG, B_RFE };
  ActionNode* path;
  U32 r15, insn_addr, next_insn_addr;
  bool thumb, has_branch;
  BOOL next_thumb;
  
  U32 GetGPR(int idx) { if (idx != 15) return U32(); return r15; }
  void SetGPR(int idx, U32 val)
  {
    if (idx != 15) return;
    if (thumb) this->Branch( val, B_JMP );
    else this->BranchExchange( val, B_JMP );
  }
  void SetGPR_mem(int idx, U32 val)
  {
    if (idx != 15) return;
    this->BranchExchange( val, B_JMP );
  }
  
  void BranchExchange(U32 target, branch_type_t branch_type)
  {
    next_thumb = BOOL(target & U32(1));
    this->Branch( target, branch_type );
  }
	
  void Branch(U32 target, branch_type_t branch_type)
  {
    this->next_insn_addr = target & U32(thumb ? -2 : -4);
    has_branch = true;
  }
	
  U32 GetCIA() { return this->insn_addr; }
  U32 GetNIA() { return this->next_insn_addr; }

  U32  GetVU32( unsigned idx ) { return U32( 0 ); }
  void SetVU32( unsigned idx, U32 val ) {}
  U64  GetVU64( unsigned idx ) { return U64( 0 ); }
  void SetVU64( unsigned idx, U64 val ) {}
  F32  GetVSR( unsigned idx ) { return F32( 0 ); }
  void SetVSR( unsigned idx, F32 val ) {}
  F64  GetVDR( unsigned idx ) { return F64( 0 ); }
  void SetVDR( unsigned idx, F64 val ) {}

  U32  GetVSU( unsigned idx ) { return U32(); }
  void SetVSU( unsigned idx, U32 val ) {}
  U64  GetVDU( unsigned idx ) { return U64(); }
  void SetVDU( unsigned idx, U64 val ) {}

  struct PSR
  {
    template <typename F, typename V> void Set( F, V ) {}
    template <typename F> U32 Get( F ) { return U32(); }
  } xpsr;
  
  PSR&     CPSR() { return xpsr; };

  void SetCPSR( U32 const&, uint32_t ) {}
  U32 GetCPSR() { return U32(); }
  U32 GetNZCV() { return U32(); }

  struct Mode
  {
    U32 GetSPSR() { return U32(); }
    void SetSPSR(U32) {}
  };
  Mode CurrentMode() { return Mode(); }
  Mode GetMode(int) { return Mode(); }

  struct ITCond {};
  ITCond itcond() const { return ITCond(); }
  bool itblock() { return Choose(); }
  void ITSetState(uint8_t, uint8_t) {}

  static bool Concretize( bool c ) { return c; }
  template <typename T>
  bool Concretize( x::XValue<T> const& cond )
  {
    BOOL c = BOOL(cond);
    if (c.determined) return c.value;
    return this->Choose();
  }
  bool Choose()
  {
    bool predicate = path->proceed();
    path = path->next( predicate );
    return predicate;
  }

  U32 MemURead32( U32 const& ) { return U32(); }
  U16 MemURead16( U32 const& ) { return U16(); }
  U32  MemRead32( U32 const& ) { return U32(); }
  U16  MemRead16( U32 const& ) { return U16(); }
  U8    MemRead8( U32 const& ) { return  U8(); }
  
  void MemUWrite32( U32 const&, U32 const& ) {}
  void MemUWrite16( U32 const&, U16 const& ) {}
  void  MemWrite32( U32 const&, U32 const& ) {}
  void  MemWrite16( U32 const&, U16 const& ) {}
  void   MemWrite8( U32 const&,  U8 const& ) {}

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

  struct Abort {};
  
  template <typename T> void UndefinedInstruction( T ) { throw Abort(); }
  void UnpredictableInsnBehaviour() { throw Abort(); }
  void FPTrap( unsigned exc ) { throw Abort(); }

  void SetBankedRegister( int, int, U32 const& ) {}
  U32 GetBankedRegister( int, int ) { return U32(); }

  void SetExclusiveMonitors( U32 const&, int ) {}
  bool ExclusiveMonitorsPass( U32 const&, int ) { return true; }
  void ClearExclusiveLocal() {}
  bool IntegerZeroDivide( BOOL const& ) { return false; }
  void BKPT( int ) {  }

  void WaitForInterrupt() {}
  
  U32    CP15ReadRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 ) { return U32(); }
  void        CP15WriteRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2, U32 value ) {}
  char const* CP15DescribeRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 ) { return "no"; }

  void     CallSupervisor( uint16_t imm ) { }

  U32 FPSCR, FPEXC;
  U32 RoundTowardsZeroFPSCR() { return U32(); }
  U32 RoundToNearestFPSCR() { return U32(); }
  
  struct FP
  {
    template <typename T> static T Abs(T) { return T(); }
    template <typename T> static T Neg(T) { return T(); }
    template <typename T> static void SetDefaultNan( T& ) {}
    template <typename T> static void SetQuietBit( T& ) {}
    template <typename X, typename Y> static void ItoF( X&, Y const&, int, ArmBranch&, U32 const& ) {}
    template <typename X, typename Y> static void FtoI( Y&, X const&, int, ArmBranch&, U32 const& ) {}
    template <typename X, typename Y> static void FtoF( X&, Y const&, ArmBranch&, U32 const& ) {}


    template <typename T> static void Add( T&, T const&, ArmBranch&, U32 const& ) {}
    template <typename T> static void Sub( T&, T const&, ArmBranch&, U32 const& ) {}
    template <typename T> static void Div( T&, T const&, ArmBranch&, U32 const& ) {}
    template <typename T> static void Mul( T&, T const&, ArmBranch&, U32 const& ) {}
    
    template <typename T> static BOOL IsSNaN(T const&) { return BOOL(); }
    template <typename T> static BOOL IsQNaN(T const&) { return BOOL(); }
    template <typename T> static BOOL IsInvalidMulAdd( T&, T const&, T const&, U32 const& ) { return BOOL(); }
    template <typename T> static BOOL FlushToZero( T&, U32 const& ) { return BOOL(); }
    template <typename T> static void MulAdd( T&, T const&, T const&, ArmBranch&, U32 const& ) {}
    template <typename T> static void Sqrt( T&, ArmBranch&, U32 const& ) {}

    template <typename T> static S32 Compare( T const&, T const&, U32 const& ) { return S32(); }
  };
};

template <> struct InsnBranch<ArmBranch> {};

inline bool CheckCondition( ArmBranch& ab, ArmBranch::ITCond const& ) { return ab.Choose(); }

template <class T> struct AMO {};

template <class CPU> using Arm32Decoder = unisim::component::cxx::processor::arm::isa::arm32::Decoder<CPU>;
template <class CPU> using ThumbDecoder = unisim::component::cxx::processor::arm::isa::thumb::Decoder<CPU>;

template <> struct AMO< Arm32Decoder<ArmProcessor> >
{
  typedef unisim::component::cxx::processor::arm::isa::arm32::CodeType CodeType;
  typedef unisim::component::cxx::processor::arm::isa::arm32::Operation<ArmProcessor> Operation;
  enum { thumb = 0 };
  typedef ArmProcessor::AOpPage OpPage;
  static OpPage& GetOpPage(ArmProcessor& ap, uint32_t tag) { return ap.arm32_op_cache[tag]; }
  static bool CodeMatch(Operation* op, CodeType insn) { return op and op->GetEncoding() == insn; }
  static Arm32Decoder<ArmBranch> bdecoder;
};

Arm32Decoder<ArmProcessor> ArmProcessor::arm32_decoder;
Arm32Decoder<ArmBranch> AMO< Arm32Decoder<ArmProcessor> >::bdecoder;

template <> struct AMO< ThumbDecoder<ArmProcessor> >
{
  typedef unisim::component::cxx::processor::arm::isa::thumb::CodeType CodeType;
  typedef unisim::component::cxx::processor::arm::isa::thumb::Operation<ArmProcessor> Operation;
  enum { thumb = 1 };
  typedef ArmProcessor::TOpPage OpPage;
  static OpPage& GetOpPage(ArmProcessor& ap, uint32_t tag) { return ap.thumb_op_cache[tag]; }
  static bool CodeMatch(Operation* op, CodeType insn) { return op and insn.match( op->GetEncoding() ); }
  static ThumbDecoder<ArmBranch> bdecoder;
};

ThumbDecoder<ArmProcessor> ArmProcessor::thumb_decoder;
ThumbDecoder<ArmBranch> AMO< ThumbDecoder<ArmProcessor> >::bdecoder;

template <typename Decoder>
void
ArmProcessor::Step( Decoder& decoder )
{
  typedef typename AMO<Decoder>::CodeType CodeType;
  typedef typename AMO<Decoder>::Operation Operation;
  typedef typename AMO<Decoder>::OpPage OpPage;

  // Fetch
  uint32_t insn_addr = this->current_insn_addr = this->next_insn_addr, insn_length = 0;
  CodeType insn;
  this->ReadInsn(insn_addr, insn);

  // Decode
  uint32_t insn_idx = insn_addr/(AMO<Decoder>::thumb ? 2 : 4),
    insn_tag = insn_idx / ArmProcessor::OPPAGESIZE, insn_offset = insn_idx % ArmProcessor::OPPAGESIZE;
  OpPage& page = AMO<Decoder>::GetOpPage(*this, insn_tag);
  Operation* op = page.ops[insn_offset];
  if (AMO<Decoder>::CodeMatch(op, insn))
    { insn_length = op->GetLength() / 8; }
  else
    {
      delete op;
      op = page.ops[insn_offset] = decoder.NCDecode( insn_addr, insn );
      insn_length = op->GetLength() / 8;
        
      {
        auto bop = AMO<Decoder>::bdecoder.NCDecode( insn_addr, insn );
          
        ActionNode root;
        for (bool end = false; not end;)
          {
            ArmBranch ab( root, insn_addr, insn_length, AMO<Decoder>::thumb );
            bop->execute( ab );
            if (not ab.has_branch)
              { op->branch.pass = true; }
            else if (not ab.next_insn_addr.determined)
              {
                if (op->branch.target == Branch::Direct) throw op->branch;
                op->branch.target = Branch::Indirect;
              }
            else if (op->branch.target != Branch::Indirect and
                     (op->branch.target != Branch::Direct or op->branch.address == ab.next_insn_addr.value))
              {
                op->branch.target = Branch::Direct;
                op->branch.address = ab.next_insn_addr.value;
              }
            else
              { throw op->branch; }
            end = ab.path->close();
          }
    
        delete bop;
      }
    }

  // Monitor
  if (this->disasm)
    {
      op->disasm(*this, std::cerr << std::hex << insn_addr << ": (" << ("AT"[AMO<Decoder>::thumb]) << ") " );
      std::cerr << std::endl;
    }
    
  if (unlikely(this->bblock))
    {
      for (auto h : hooks[Hook::BLOCK])
        {
          if (h->bound_check(next_insn_addr))
            h->cb<Hook::cb_code>()(this, insn_addr, 0);
        }
    }
  for (auto h : this->hooks[Hook::CODE])
    {
      if (h->bound_check(insn_addr))
        h->cb<Hook::cb_code>()(this, insn_addr, insn_length);
    }

  // Execute
  this->gpr[15] = insn_addr + (AMO<Decoder>::thumb ? 4 : 8);
  this->next_insn_addr = insn_addr + insn_length;
    
  op->execute( *this );

  if (AMO<Decoder>::thumb)
    this->ITAdvance();

  this->bblock = (op->branch.target != op->branch.BNone);
}

int
ArmProcessor::emu_start( uint64_t begin, uint64_t until, uint64_t timeout, uintptr_t count )
{
  if (timeout)
    {
      std::cerr << "Error: timeout unimplemented." << timeout << std::endl;
      throw 0;
    }
    
  // std::cerr << "until: " << until << std::endl;
  // std::cerr << "count: " << count << std::endl;

  this->Branch(begin, B_DBG);
  this->bblock = true;
  
  while (next_insn_addr != until)
    {
      /* Instruction boundary next_insn_addr becomes current_insn_addr */
      try {
        if (cpsr.Get( unisim::component::cxx::processor::arm::T ))
          {
            Step(thumb_decoder);
          }
        else
          {
            Step(arm32_decoder);
          }
            
        // if (unlikely(requires_commit_instruction_reporting and memory_access_reporting_import))
        //   memory_access_reporting_import->ReportCommitInstruction(this->current_insn_addr, insn_length);
            
        //instruction_counter++; /* Instruction regularly finished */
      }
          
      //         catch (SVCException const& svexc) {
      //           /* Resuming execution, since SVC exceptions are explicitly
      //    * requested from regular instructions. ITState will be updated as
      //    * needed by TakeSVCException (as done in the ARM spec). */
      //   if (unlikely( requires_commit_instruction_reporting and memory_access_reporting_import ))
      //     memory_access_reporting_import->ReportCommitInstruction(this->current_insn_addr, insn_length);

      //   instruction_counter++; /* Instruction regularly finished */
    
      //   this->TakeSVCException();
      // }
  
      // catch (DataAbortException const& daexc) {
      //   /* Abort execution, and take processor to data abort handler */
    
      //   if (unlikely(trap_reporting_import))
      //     trap_reporting_import->ReportTrap( *this, "Data Abort Exception" );
    
      //   this->TakeDataOrPrefetchAbortException(true); // TakeDataAbortException
      // }
  
      // catch (PrefetchAbortException const& paexc) {
      //   /* Abort execution, and take processor to prefetch abort handler */
    
      //   if (unlikely(trap_reporting_import))
      //     trap_reporting_import->ReportTrap( *this, "Prefetch Abort Exception" );
    
      //   this->TakeDataOrPrefetchAbortException(false); // TakePrefetchAbortException
      // }
  
      // catch (UndefInstrException const& undexc) {
      //   /* Abort execution, and take processor to undefined handler */
    
      //   if (unlikely(trap_reporting_import))
      //     trap_reporting_import->ReportTrap( *this, "Undefined Exception" );
    
      //   this->TakeUndefInstrException();
      // }
  
      catch (unisim::component::cxx::processor::arm::Exception const& exc)
        {
          throw 0;
          // logger << DebugError << "Unimplemented exception (" << exc.what() << ")"
          //        << " pc: " << std::hex << current_insn_addr << std::dec
          //        << EndDebugError;
          // this->Stop(-1);
        }
    }
    
  //  std::cerr << "Stopped: current=0x" << std::hex << current_insn_addr << ", next: " << std::hex << next_insn_addr << std::dec << std::endl;
  return 0;
}

void 
ArmProcessor::ReadInsn(uint32_t address, unisim::component::cxx::processor::arm::isa::arm32::CodeType& insn)
{
  uint32_t word;
  PhysicalFetchMemory( address, (uint8_t*)&word, 4 );
  insn = Target2Host(unisim::util::endian::E_LITTLE_ENDIAN, word);
}

void
ArmProcessor::ReadInsn(uint32_t address, unisim::component::cxx::processor::arm::isa::thumb::CodeType& insn)
{
  PhysicalFetchMemory( address, &insn.str[0], 4 );
  insn.size = 32;
}

void
ArmProcessor::UndefinedInstruction( unisim::component::cxx::processor::arm::isa::arm32::Operation<ArmProcessor>* insn )
{
  insn->disasm(*this, std::cerr << "Undefined instruction @" << std::hex << current_insn_addr << std::dec << ": " );
  std::cerr << std::endl;
  // throw UndefInstrException();
  throw 0;
}

void
ArmProcessor::UndefinedInstruction( unisim::component::cxx::processor::arm::isa::thumb::Operation<ArmProcessor>* insn )
{
  insn->disasm(*this, std::cerr << "Undefined instruction @" << std::hex << current_insn_addr << std::dec << ": " );
  std::cerr << std::endl;
  // throw UndefInstrException();
  throw 0;
}

void
ArmProcessor::PerformUWriteAccess( uint32_t addr, uint32_t size, uint32_t value )
{
  uint32_t const lo_mask = size - 1;
  if (unlikely((lo_mask > 3) or (size & lo_mask))) throw std::logic_error("Bad size");
  uint32_t misalignment = addr & lo_mask;
  
  if (unlikely(misalignment and not unisim::component::cxx::processor::arm::sctlr::A.Get( this->SCTLR )))
    {
      uint32_t eaddr = addr;
      if (GetEndianness() == unisim::util::endian::E_BIG_ENDIAN) {
        for (unsigned byte = size; --byte < size; ++eaddr)
          PerformWriteAccess( eaddr, 1, (value >> (8*byte)) & 0xff );
      } else {
        for (unsigned byte = 0; byte < size; ++byte, ++eaddr)
          PerformWriteAccess( eaddr, 1, (value >> (8*byte)) & 0xff );
      }
      return;
    }
  else
    PerformWriteAccess( addr, size, value );
}

void
ArmProcessor::PerformWriteAccess( uint32_t addr, uint32_t size, uint32_t value )
{
  uint32_t const lo_mask = size - 1;
  if (unlikely((lo_mask > 3) or (size & lo_mask))) throw std::logic_error("bad size");
  uint32_t misalignment = addr & lo_mask;
  
  if (unlikely(misalignment))
    DataAbort( addr, mat_write, unisim::component::cxx::processor::arm::DAbort_Alignment );

  uint8_t data[4];

  if (GetEndianness() == unisim::util::endian::E_BIG_ENDIAN) {
    uint32_t shifter = value;
    for (int byte = size; --byte >= 0;)
      { data[byte] = shifter; shifter >>= 8; }
  } else {
    uint32_t shifter = value;
    for (int byte = 0; byte < int( size ); ++byte)
      { data[byte] = shifter; shifter >>= 8; }
  }
  
  if (not PhysicalWriteMemory( addr, data, size ))
    DataAbort(addr, mat_write, unisim::component::cxx::processor::arm::DAbort_SyncExternal);
}

uint32_t
ArmProcessor::PerformUReadAccess( uint32_t addr, uint32_t size )
{
  uint32_t const lo_mask = size - 1;
  if (unlikely((lo_mask > 3) or (size & lo_mask))) throw std::logic_error("bad size");
  uint32_t misalignment = addr & lo_mask;
  
  if (unlikely(misalignment and not unisim::component::cxx::processor::arm::sctlr::A.Get( this->SCTLR )))
    {
      uint32_t result = 0;
      if (GetEndianness() == unisim::util::endian::E_BIG_ENDIAN) {
        for (unsigned byte = 0; byte < size; ++byte)
          result = (result << 8) | PerformReadAccess( addr + byte, 1 );
      } else {
        for (unsigned byte = size; --byte < size;)
          result = (result << 8) | PerformReadAccess( addr + byte, 1 );
      }
      return result;
    }
  else
    return PerformReadAccess( addr, size );
}

uint32_t
ArmProcessor::PerformReadAccess( uint32_t addr, uint32_t size )
{
  uint32_t const lo_mask = size - 1;
  if (unlikely((lo_mask > 3) or (size & lo_mask))) throw std::logic_error("bad size");
  uint32_t misalignment = addr & lo_mask;
  
  if (unlikely(misalignment))
    DataAbort(addr, mat_read, unisim::component::cxx::processor::arm::DAbort_Alignment);

  uint8_t data[4];

  if (not PhysicalReadMemory(addr, &data[0], size))
    DataAbort(addr, mat_read, unisim::component::cxx::processor::arm::DAbort_SyncExternal);

  /* Compute return value */
  uint32_t value;
  if (GetEndianness() == unisim::util::endian::E_LITTLE_ENDIAN) {
    uint32_t shifter = 0;
    for (int byte = size; --byte >= 0;)
      { shifter = (shifter << 8) | uint32_t( data[byte] ); }
    value = shifter;
  } else {
    uint32_t shifter = 0;
    for (int byte = 0; byte < int( size ); ++byte)
      { shifter = (shifter << 8) | uint32_t( data[byte] ); }
    value = shifter;
  }

  return value;
}

void
ArmProcessor::DataAbort(uint32_t address, mem_acc_type_t mat, unisim::component::cxx::processor::arm::DAbort type)
{
  throw 0;
  // if (mat == mat_exec)
  //   throw PrefetchAbortException();
  // else
  //   throw DataAbortException();
}
