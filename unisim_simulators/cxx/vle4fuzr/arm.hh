/*
 *  Copyright (c) 2019,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 * FUZR RENAULT CEA FILE
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr), Gilles Mouchard <gilles.mouchard@cea.fr>
 */

#ifndef __VLE4FUZR_ARM_HH__
#define __VLE4FUZR_ARM_HH__

#include <emu.hh>
#include <unisim/component/cxx/processor/arm/vmsav7/cpu.hh>
#include <unisim/component/cxx/processor/arm/isa_arm32.hh>
#include <unisim/util/symbolic/symbolic.hh>

struct ArmProcessor
  : public Processor
  , public unisim::component::cxx::processor::arm::vmsav7::CPU
{
  typedef unisim::component::cxx::processor::arm::vmsav7::CPU CPU;

  ArmProcessor( char const* name, bool is_thumb );
  ~ArmProcessor();

  static ArmProcessor& Self( Processor& proc ) { return dynamic_cast<ArmProcessor&>( proc ); }
  
  Processor::RegView const* get_reg(int regid) override;
  
  virtual void Sync() override { throw 0; }
  virtual void ResetMemory() override { throw 0; }
  virtual bool ExternalReadMemory( uint32_t addr, void* buffer, uint32_t size ) override { throw 0; }
  virtual bool ExternalWriteMemory( uint32_t addr, void const* buffer, uint32_t size ) override { throw 0; }
  virtual bool PhysicalWriteMemory( uint32_t addr, uint32_t paddr, uint8_t const* buffer, uint32_t size, uint32_t attrs ) override
  {
    auto pi = pages.lower_bound(addr);
    if (pi == pages.end())
      pi = mem_allocate(0, addr, (pages.size() ? (--pi)->base : 0)-1);
    else if (pi->hi() <= addr)
      pi = mem_allocate(pi->hi(), addr, (--pi != pages.end() ? pi->base : 0)-1);
    
    if (pi == pages.end())
      return false;
    uint32_t pos = addr - pi->base;
    std::copy(&buffer[0], &buffer[size], pi->at(pos));
    return true;
  }
  
  virtual bool PhysicalReadMemory( uint32_t addr, uint32_t paddr, uint8_t* buffer, uint32_t size, uint32_t attrs ) override
  {
    auto pi = pages.lower_bound(addr);
    if (pi == pages.end())
      pi = mem_allocate(0, addr, (pages.size() ? (--pi)->base : 0)-1);
    else if (pi->hi() <= addr)
      pi = mem_allocate(pi->hi(), addr, (--pi != pages.end() ? pi->base : 0)-1);
    
    if (pi == pages.end())
      return false;
    uint32_t pos = addr - pi->base;
    std::copy(pi->at(pos), pi->at(pos+size), buffer);
    return true;
  }
  virtual bool PhysicalFetchMemory( uint32_t addr, uint32_t paddr, uint8_t* buffer, uint32_t size, uint32_t attrs ) override
  {
    auto pi = pages.lower_bound(addr);
    if (pi == pages.end())
      pi = mem_allocate(0, addr, (pages.size() ? (--pi)->base : 0)-1);
    else if (pi->hi() <= addr)
      pi = mem_allocate(pi->hi(), addr, (--pi != pages.end() ? pi->base : 0)-1);
    
    if (pi == pages.end())
      return false;
    uint32_t pos = addr - pi->base;
    std::copy(pi->at(pos), pi->at(pos+size), buffer);
    return true;
  }
  
  int emu_start( uint64_t begin, uint64_t until, uint64_t timeout, uintptr_t count ) override;

};

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
  
  ArmBranch( ActionNode& root, uint32_t addr, bool _thumb )
    : path(&root), insn_addr(addr), thumb(_thumb)
  {}
  
  enum branch_type_t { B_JMP = 0, B_CALL, B_RET, B_EXC, B_DBG, B_RFE };
  ActionNode* path;
  U32 r15, insn_addr, next_insn_addr;
  bool thumb;
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

  static void PrintNextPC(std::ostream& sink, ArmProcessor& pr,
                          unisim::component::cxx::processor::arm::isa::thumb::CodeType const& insn);
  static void PrintNextPC(std::ostream& sink, ArmProcessor& pr,
                          unisim::component::cxx::processor::arm::isa::arm32::CodeType const& insn);
};

#endif /* __VLE4FUZR_ARM_HH__ */

