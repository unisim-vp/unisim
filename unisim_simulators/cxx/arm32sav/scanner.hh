/*
 *  Copyright (c) 2020-2023,
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

#ifndef ARM32SAV_SCANNER_HH
#define ARM32SAV_SCANNER_HH

#include <test.hh>
#include <top_arm32.hh>
#include <top_thumb.hh>
#include <unisim/component/cxx/processor/arm/isa/constants.hh>
#include <unisim/component/cxx/processor/arm/isa/models.hh>
#include <unisim/component/cxx/vector/vector.hh>
#include <unisim/util/symbolic/symbolic.hh>
#include <unisim/util/symbolic/vector/vector.hh>
#include <map>
#include <vector>
#include <list>

struct NeonRegister
  : public unisim::util::symbolic::WithValueType<NeonRegister>
{
  enum { BYTECOUNT = 8 };
  struct value_type { char _[BYTECOUNT]; };
  NeonRegister() = default;
  NeonRegister(unisim::util::symbolic::Expr const& _expr) : expr(_expr) {}
  unisim::util::symbolic::Expr expr;
};

struct Scanner
{
  struct Arm32 : public unisim::component::cxx::processor::arm::isa::arm32::Decoder<Scanner>
  {
    typedef unisim::component::cxx::processor::arm::isa::arm32::Operation<Scanner> Operation;

    Arm32();
    ~Arm32();

    Operation* decode(uint32_t addr, uint32_t code, std::string& disasm);
  };

  struct Thumb2 : public unisim::component::cxx::processor::arm::isa::thumb::Decoder<Scanner>
  {
    typedef unisim::component::cxx::processor::arm::isa::thumb::Operation<Scanner> Operation;

    Thumb2();
    ~Thumb2();

    Operation* decode(uint32_t addr, uint32_t code, std::string& disasm);
  };

  Scanner(Interface& iif);
  ~Scanner();
  void step( Arm32::Operation const& op );
  void step( Thumb2::Operation const& op );
  bool close( Scanner const& ref );

  template <typename T> using SmartValue = unisim::util::symbolic::SmartValue<T>;

  typedef SmartValue<uint8_t>     U8;
  typedef SmartValue<uint16_t>    U16;
  typedef SmartValue<uint32_t>    U32;
  typedef SmartValue<uint64_t>    U64;
  typedef SmartValue<int8_t>      S8;
  typedef SmartValue<int16_t>     S16;
  typedef SmartValue<int32_t>     S32;
  typedef SmartValue<int64_t>     S64;
  typedef SmartValue<bool>        BOOL;

  // typedef u64_t addr_t;

  typedef SmartValue<float>       F32;
  typedef SmartValue<double>      F64;

  typedef unisim::util::symbolic::Expr Expr;
  typedef unisim::util::symbolic::ExprNode ExprNode;
  typedef unisim::util::symbolic::ValueType ValueType;
  typedef unisim::util::symbolic::ConstNodeBase ConstNodeBase;

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
  };

  struct Update : public ExprNode
  {
    virtual ValueType const* GetType() const override { return unisim::util::symbolic::NoValueType(); }
  };

  struct RegReadBase : public unisim::util::symbolic::ExprNode
  {
    virtual char const* GetRegName() const = 0;
    virtual void Repr( std::ostream& sink ) const override { sink << "RegRead(" << GetRegName() << ")"; }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<RegReadBase const&>( rhs ) ); };
    int compare( RegReadBase const& rhs ) const { return 0; }
  };

  template <typename RID>
  struct RegRead : public RegReadBase
  {
    RegRead( RID _id ) : id(_id) {}
    virtual RegRead* Mutate() const override { return new RegRead( *this ); }
    virtual char const* GetRegName() const override { return id.c_str(); };
    virtual ValueType const* GetType() const { return RID::GetType(); }
    virtual unsigned SubCount() const override { return 0; }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<RegRead const&>( rhs ) ); };
    int compare( RegRead const& rhs ) const { if (int delta = RegReadBase::compare( rhs )) return delta; return id.cmp( rhs.id ); }

    RID id;
  };

  template <typename RID>
  Expr newRegRead( RID _id ) { return new RegRead<RID>(_id); }

  struct VRReadBase : public unisim::util::sav::VirtualRegister, public ExprNode
  {
    typedef unisim::util::sav::VirtualRegister VirtualRegister;
    VRReadBase( unsigned reg,  unsigned idx ) : VirtualRegister(reg, idx), ExprNode() {}
    virtual int cmp(ExprNode const& rhs) const override { return compare( dynamic_cast<VirtualRegister const&>( rhs ) ); }
    virtual unsigned SubCount() const override { return 0; }
  };

  template <class T>
  struct VRRead : public VRReadBase
  {
    typedef VRRead<T> this_type;
    VRRead( unsigned reg,  unsigned idx ) : VRReadBase(reg, idx) {}
    virtual ExprNode* Mutate() const override { return new this_type(*this); }
    virtual void Repr( std::ostream& sink ) const override { sink << T::name() << "Read( " << idx << ", " << reg << " )"; }
    virtual ValueType const* GetType() const override { return T::GetType(); }
  };

  struct VRWriteBase : public unisim::util::sav::VirtualRegister, public Update
  {
    typedef unisim::util::sav::VirtualRegister VirtualRegister;
    VRWriteBase( unsigned reg, unsigned idx, Expr const& _val ) : VirtualRegister(reg, idx), Update(), val(_val) {}
    virtual int cmp(ExprNode const& rhs) const override { return compare( dynamic_cast<VirtualRegister const&>( rhs ) ); }
    virtual unsigned SubCount() const override { return 1; }
    virtual Expr const& GetSub(unsigned idx) const override { if (idx == 0) return val; return ExprNode::GetSub(idx); }
    Expr val;
  };

  template <class T>
  struct VRWrite : public VRWriteBase
  {
    typedef VRWrite<T> this_type;
    VRWrite( unsigned reg,  unsigned idx, Expr const& val ) : VRWriteBase(reg, idx, val) {}
    virtual ExprNode* Mutate() const override { return new this_type(*this); }
    virtual void Repr( std::ostream& sink ) const override { sink << T::name() << "Write( " << reg << ", " << idx << ", " << val << " )"; }
  };

  struct VReg { static ValueType const* GetType() { return unisim::util::symbolic::CValueType(uint64_t()); } static char const* name() { return "VReg"; } };
  // struct FReg { static ValueType const* GetType() { return unisim::util::symbolic::CValueType(double()); } static char const* name() { return "FReg"; } };
  struct GReg { static ValueType const* GetType() { return unisim::util::symbolic::CValueType(uint64_t()); } static char const* name() { return "GReg"; } };

  typedef VRRead<VReg> VRegRead; typedef VRWrite<VReg> VRegWrite;
  //  typedef VRRead<FReg> FRegRead; typedef VRWrite<FReg> FRegWrite;
  /**/                           typedef VRWrite<GReg> GRegWrite;

  struct GRegRead : public VRRead<GReg>, public unisim::util::sav::Addressings::Source
  {
    GRegRead( unsigned reg, unsigned idx ) : VRRead<GReg>( reg, idx ) {}
  };

  struct RegWriteBase : public Update
  {
    RegWriteBase( Expr const& _value ) : value( _value ) {}
    virtual unsigned SubCount() const override { return 1; }
    virtual Expr const& GetSub(unsigned idx) const override { if (idx != 0) return ExprNode::GetSub(idx); return value; }
    virtual char const* GetRegName() const = 0;
    virtual void Repr( std::ostream& sink ) const override { sink << GetRegName() << " := " << value; }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<RegWriteBase const&>( rhs ) ); }
    int compare( RegWriteBase const& rhs ) const { return 0; }
    Expr value;
  };

  template <typename RID>
  struct RegWrite : public RegWriteBase
  {
    typedef RegWrite<RID> this_type;
    RegWrite( RID _id, Expr const& _value ) : RegWriteBase( _value ), id( _id ) {}
    virtual this_type* Mutate() const override { return new this_type( *this ); }
    virtual char const* GetRegName() const override { return id.c_str(); };
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<this_type const&>( rhs ) ); }
    int compare( this_type const& rhs ) const { if (int delta = RegWriteBase::compare( rhs )) return delta; return id.cmp( rhs.id ); }

    RID id;
  };

  template <typename RID> RegWrite<RID>* newRegWrite( RID _id, Expr const& _value ) { return new RegWrite<RID>(_id, _value); }

  struct Load : public ExprNode
  {
    Load( ValueType const* _tp, Expr const& _addr ) : addr(_addr), tp(_tp) {}
    virtual Load* Mutate() const override { return new Load( *this ); }
    virtual void Repr( std::ostream& sink ) const override { GetType()->GetName(sink << "Load"); sink << "(" << addr << ")"; }
    virtual unsigned SubCount() const override { return 1; }
    virtual Expr const& GetSub(unsigned idx) const override { if (idx != 0) return ExprNode::GetSub(idx); return addr; }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<Load const&>( rhs ) ); }
    int compare( Load const& rhs ) const { return tp->cmp(*rhs.tp); }
    virtual ValueType const* GetType() const override { return tp; }

    Expr addr;
    ValueType const* tp;
  };

  struct Store : public Update
  {
    Store( ValueType const* _tp, Expr const& _addr, Expr const& _value )
      : addr( _addr ), value( _value ), tp(_tp)
    {}
    virtual Store* Mutate() const override { return new Store( *this ); }
    virtual void Repr( std::ostream& sink ) const override { GetType()->GetName(sink << "Store"); sink << "( " << addr << ", " << value <<  " )"; }
    virtual unsigned SubCount() const override { return 2; }
    virtual Expr const& GetSub(unsigned idx) const override { switch (idx) { case 0: return addr; case 1: return value; } return ExprNode::GetSub(idx); }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<Store const&>( rhs ) ); }
    int compare( Store const& rhs ) const { return tp->cmp(*rhs.tp); }

    Expr addr;
    Expr value;
    ValueType const* tp;
  };

  static void dont(char const* reason) { throw unisim::util::sav::Untestable(reason); }
  template <typename T>
  static T dontread(char const* reason) { dont(reason); return T(); }

  void gregtouch(unsigned id, bool write);
  U32 gregread(unsigned id) { gregtouch(id, false); return gpr[id]; }
  void gregwrite(unsigned id, U32 const& value) { gregtouch(id, true); gpr[id] = value; }

  U32  GetGPR_usr( int id ) { /* Only work in system mode instruction */ return dontread<U32>("system"); }
  U32  GetGPR( int id ) { return id == 13 ? dontread<U32>("=SP") : id == 15 ? gpr[15] : gregread(id); }
  void SetGPR_usr( int id, U32 const& ) { /* Only work in system mode instruction */ dont("system"); }
  void SetGPR_mem( int id, U32 const& value ) { SetGPR(id, value); }
  void SetGPR( int id, U32 const& value ) { switch (id) {case 15: dont("PC="); case 13: dont("SP="); } gregwrite(id, value); }

  struct PC : public unisim::util::symbolic::WithValueType<PC>
  {
    typedef uint32_t value_type;
    char const* c_str() const { return "pc"; }
    int cmp( PC const& ) const { return 0; }
  };

  struct Flag : public unisim::util::identifier::Identifier<Flag>, public unisim::util::symbolic::WithValueType<Flag>
  {
    typedef bool value_type;
    enum Code { N, Z, C, V, end } code;

    char const* c_str() const
    {
      static char const* names[] = {"n", "z", "c", "v", "NA"};
      return names[int(code)];
    }

    Flag() : code(end) {}
    Flag( Code _code ) : code(_code) {}
    Flag( char const* _code ) : code(end) { init( _code ); }
  };

  struct PSRTracer
  {
    PSRTracer( Scanner& _arch ) : arch( _arch ) {} Scanner& arch;
    template <typename RF>  U32      Get( RF const& rf ) const { return U32(arch.GetPSR( rf )); }
    template <typename RF>  void     Set( RF const& rf, U32 const& value ) { arch.SetPSR( rf, value ); }
    template <typename RF>  void     Set( RF const& rf, BOOL const& value ) { arch.SetPSR( rf, U32(value) ); }
    U32 bits() const { return arch.GetPSR( unisim::component::cxx::processor::arm::ALL32 ); }
  };

  template <unsigned POS, unsigned SIZE> using BitField = unisim::util::arithmetic::BitField<POS, SIZE>;

  U32 GetPSR(BitField<31, 1> const&) { return U32(flags[Flag::N]); }
  U32 GetPSR(BitField<30, 1> const&) { return U32(flags[Flag::Z]); }
  U32 GetPSR(BitField<29, 1> const&) { return U32(flags[Flag::C]); }
  U32 GetPSR(BitField<28, 1> const&) { return U32(flags[Flag::V]); }

  template <class T> U32 GetPSR(T const&) { return dontread<U32>("cpsr"); }

  void SetPSR(BitField<31, 1> const&, U32 const& value) { flags[Flag::N] = BOOL(value); }
  void SetPSR(BitField<30, 1> const&, U32 const& value) { flags[Flag::Z] = BOOL(value); }
  void SetPSR(BitField<29, 1> const&, U32 const& value) { flags[Flag::C] = BOOL(value); }
  void SetPSR(BitField<28, 1> const&, U32 const& value) { flags[Flag::V] = BOOL(value); }

  template <class T> void SetPSR(T const&, U32 const& value) { dont("cpsr"); }

  typedef PSRTracer psr_type;
  PSRTracer CPSR() { return PSRTracer(*this); };
  //  PSRTracer& SPSR() { /* Only work in system mode instruction */ dont("system"); return cpsr; };

  U32 GetCPSR() { return dontread<U32>("cpsr"); }
  void SetCPSR(U32 mask, uint32_t bits) { dont("cpsr"); }

  bool concretize( Expr cexp );

  template <typename T>
  bool Test( unisim::util::symbolic::SmartValue<T> const& cond )
  {
    if (not cond.expr.good())
      throw std::logic_error( "Not a valid condition" );

    return concretize( BOOL(cond).expr );
  }

//     Scanner() : psr_value( 0 ), psr_ok_mask( 0xf80f0000 ), cpsr(*this), fpscr(*this)
//     {
//       for (unsigned reg = 0; reg < 16; ++reg)
//         reg_values[reg] = U32( new SourceReg( reg ) );
//     }

//     Interface interface;
//     UniqueVId gpr_uvi;
//     U32   reg_values[16];

//     void gpr_append( unsigned idx, bool w ) { interface.irappend( idx, w, gpr_uvi ); }

//     void reject();

  U32 GetNIA() { return next_insn_addr; }


  U8   GetNZCV() const
  {
    U8 res = U8(BOOL(flags[0]));
    for (int idx = 1; idx < 4; ++idx)
      res = res | (U8(BOOL(flags[idx])) << idx );
    return res;
  }

//     void SetGPRMapping( uint32_t src_mode, uint32_t tar_mode ) { /* system related */ dont("system"); }

  Expr MemRead(ValueType const* tp, Expr const& addr, bool aligned)
  {
    interface.memaccess( addr, tp->GetBitSize()/8, false, aligned );
    return new Load( tp, addr );
  }
  template <typename T>
  T MemReadT( U32 const& addr, bool aligned ) { return MemRead(T::GetType(), addr.expr, aligned); }

  U32  MemURead32( U32 const& addr ) { return MemReadT<U32>( addr, false ); }
  U16  MemURead16( U32 const& addr ) { return MemReadT<U16>( addr, false ); }
  U32  MemRead32( U32 const& addr ) { return MemReadT<U32>( addr, true ); }
  U16  MemRead16( U32 const& addr ) { return MemReadT<U16>( addr, true ); }
  U8  MemRead8( U32 const& addr ) { return MemReadT<U8>( addr, true ); }

  void MemWrite(ValueType const* tp, Expr const& addr, Expr const& data, bool aligned)
  {
    interface.memaccess( addr, tp->GetBitSize()/8, true, aligned );
    stores.insert( new Store( tp, addr, data ) );
  }
  template <typename T>
  void MemWriteT(U32 const& addr, T const& data, bool aligned) { MemWrite(T::GetType(), addr.expr, data.expr, aligned); }

  void MemUWrite32( U32 const& addr, U32 const& value ) { MemWriteT( addr, value, false ); }
  void MemUWrite16( U32 const& addr, U16 const& value ) { MemWriteT( addr, value, false ); }
  void MemWrite32( U32 const& addr, U32 const& value ) { MemWriteT( addr, value, true ); }
  void MemWrite16( U32 const& addr, U16 const& value ) { MemWriteT( addr, value, true ); }
  void MemWrite8( U32 const& addr, U8 const& value ) { MemWriteT( addr, value, true ); }

  void SetExclusiveMonitors( U32 const& address, unsigned size ) { dont("mp"); }
  bool ExclusiveMonitorsPass( U32 const& address, unsigned size ) { return dontread<bool>("mp"); }
  void ClearExclusiveLocal() { dont("mp"); }

  enum branch_type_t { B_JMP = 0, B_CALL, B_RET, B_EXC, B_DBG, B_RFE };
  void BranchExchange( U32 const& target, branch_type_t ) { dont("branch"); }
  void Branch( U32 const& target, branch_type_t ) { dont("branch"); }

  void WaitForInterrupt() { dont("system"); }
  void SWI( uint32_t imm ) { dont("system"); }
  void BKPT( uint32_t imm ) { dont("system"); }
  void UnpredictableInsnBehaviour() { dont("unpredictable"); }
  template <typename OP> void UndefinedInstruction( OP* op ) { dont("undefined"); }
  void CallSupervisor( uint32_t imm ) { dont("system"); }
  void UnpredictableIf( BOOL const& condition ) { dont("unpredictable"); }
  bool IntegerZeroDivide( BOOL const& condition ) { return false; }

  void CheckAlignment(U32 const&, uint32_t alignement) { if (alignement > 4) dont("alignement"); }

  static unsigned const VREGCOUNT = 32;

  struct VUConfig : public unisim::util::symbolic::vector::VUConfig
  { static unsigned const BYTECOUNT = NeonRegister::BYTECOUNT; };

  typedef unisim::component::cxx::vector::VUnion<VUConfig> VectorView;
  struct VectorBrick { char _[sizeof(U8)]; };

  VectorView& vregtouch(unsigned reg, bool is_write)
  {
    unsigned idx = interface.vregs.touch(reg, is_write);
    VectorView& vv = vector_views[reg];
    if (vv.transfer == vv.initial)
      {
        NeonRegister v( new VRegRead( reg, idx ) );
        *(vv.GetStorage( &vector_data[reg][0], v, VUConfig::BYTECOUNT )) = v;
      }
    return vv;
  }
  template <typename T>
  T vector_read(unsigned reg, unsigned sub)
  {
    return (vregtouch(reg, false).GetConstStorage(&vector_data[reg][0], T(), VUConfig::BYTECOUNT))[sub];
  }

  U32 GetVSU( unsigned reg )                 { return vector_read<U32> (reg/2, reg%2); }
  U64 GetVDU( unsigned reg )                 { return vector_read<U64> (reg, 0); }
  F32 GetVSR( unsigned reg )                 { return vector_read<F32> (reg/2, reg%2); }
  F64 GetVDR( unsigned reg )                 { return vector_read<F64> (reg, 0); }

  template <class ELEMT>
  ELEMT GetVDE( unsigned reg, unsigned idx, ELEMT const& trait )
  {
    return ELEMT( vector_read<ELEMT>(reg, idx) );
  }
  
  U8 GetTVU8(unsigned reg0, unsigned elements, unsigned regs, Scanner::U8 const& index, Scanner::U8 const& oob_value);

  template <typename T>
  void vector_write(unsigned reg, unsigned sub, T value )
  {
    (vregtouch(reg, true).GetStorage(&vector_data[reg][0], value, VUConfig::BYTECOUNT))[sub] = value;
  }

  void SetVSU( unsigned reg, U32 const& val ) { vector_write( reg/2, reg%2, val ); }
  void SetVDU( unsigned reg, U64 const& val ) { vector_write( reg, 0, val ); }
  void SetVSR( unsigned reg, F32 const& val ) { vector_write( reg/2, reg%2, val ); }
  void SetVDR( unsigned reg, F64 const& val ) { vector_write( reg, 0, val ); }

  template <class ELEMT>
  void SetVDE( unsigned reg, unsigned idx, ELEMT const& value )
  {
    vector_write(reg, idx, value);
  }
  
  uint32_t itcond() const { return 14; }
  bool itblock() { interface.itsensitive = true; return false; }
  void ITSetState( uint32_t cond, uint32_t mask ) { dont("cpsr"); }

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

  struct Mode
  {
    Mode() {}
    bool     HasBR( unsigned index ) { return false; }
    bool     HasSPSR() { return false; }
    void     SetSPSR(U32 value) {};
    U32      GetSPSR() { return U32(0); };
    void     Swap( Scanner& ) {};

  } mode;

  Mode&  CurrentMode() { dont("system"); return mode; }
  Mode&  GetMode(uint8_t) { dont("system"); return mode; }
  U32  GetBankedRegister( uint8_t foreign_mode, uint32_t idx ) { return dontread<U32>("system"); }
  void SetBankedRegister( uint8_t foreign_mode, uint32_t idx, U32 value ) { dont("system"); }

  struct CP15Reg
  {
    void CheckPermissions( uint8_t, uint8_t, uint8_t, uint8_t, Scanner&, bool ) const { dont("cp15"); }
    U32 Read( uint8_t, uint8_t, uint8_t, uint8_t, Scanner& ) const { return dontread<U32>("cp15"); }
    void Write( uint8_t, uint8_t, uint8_t, uint8_t, Scanner&, U32 const& ) const { dont("cp15"); }
    void Describe( uint8_t crn, uint8_t op1, uint8_t crm, uint8_t op2, std::ostream& sink ) const
    {
      sink << "CR15{crn=" << int(crn) << ", op1=" << int(op1) << ", crm=" << int(crm) << ", op2=" << int(op2) << "}";
    }
  };

  static CP15Reg* CP15GetRegister(uint8_t, uint8_t, uint8_t, uint8_t) { static CP15Reg _; return &_; }

  Interface&     interface;
  unisim::util::sav::ActionNode*    path;

  std::set<Expr> stores;

  U32            gpr[14];
  BOOL           flags[Flag::end];
  U32            current_insn_addr, next_insn_addr;

  VectorView     vector_views[VREGCOUNT];
  VectorBrick    vector_data[VREGCOUNT][VUConfig::BYTECOUNT];
};

namespace unisim {
namespace util {
namespace symbolic {
  template <class OP> SmartValue<OP> NeonSHL( SmartValue<OP> op, SmartValue<int8_t> sh )
  {
    return unisim::util::sav::make_weirdop<SmartValue<OP>>("NeonSHL", op, sh);
  }
}
}
}

#endif // ARM32SAV_SCANNER_HH
