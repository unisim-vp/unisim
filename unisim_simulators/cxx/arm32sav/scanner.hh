/*
 *  Copyright (c) 2020,
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
#include <unisim/util/symbolic/symbolic.hh>
#include <unisim/component/cxx/processor/arm/psr.hh>
#include <unisim/component/cxx/processor/arm/models.hh>
#include <map>
#include <vector>
#include <list>

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

  typedef float                   F32;
  typedef double                  F64;

  typedef unisim::util::symbolic::Expr Expr;
  typedef unisim::util::symbolic::ExprNode ExprNode;
  typedef unisim::util::symbolic::ScalarType ScalarType;
  typedef unisim::util::symbolic::ConstNodeBase ConstNodeBase;
  typedef unisim::util::symbolic::EvalSpace EvalSpace;

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

  struct AddrEval : public EvalSpace {};

  struct RelocEval : public EvalSpace
  {
    RelocEval( uint32_t const* _regs, Interface const& _tif, uint32_t _address )
      : regs(_regs), tif(_tif), address(_address) {}
    uint32_t GetReg(unsigned reg) const { return regs[tif.gindex(reg)]; }
    uint32_t const* regs;
    Interface const& tif;
    uint32_t address;
  };

  struct Update : public ExprNode
  {
    virtual ScalarType::id_t GetType() const override { return ScalarType::VOID; }
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
    virtual ScalarType::id_t GetType() const
    { return unisim::util::symbolic::TypeInfo<typename RID::register_type>::GetType(); }
    virtual unsigned SubCount() const override { return 0; }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<RegRead const&>( rhs ) ); };
    int compare( RegRead const& rhs ) const { if (int delta = RegReadBase::compare( rhs )) return delta; return id.cmp( rhs.id ); }
    ConstNodeBase const* Eval( EvalSpace const& evs, ConstNodeBase const** cnbs) const override { return id.eval(evs, cnbs); }

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
    virtual ScalarType::id_t GetType() const override { return T::scalar_type; }
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

  struct GReg { static ScalarType::id_t const scalar_type = ScalarType::VOID; static char const* name() { return "GReg"; } };

  // typedef VRRead<VReg> VRegRead; typedef VRWrite<VReg> VRegWrite;
  // typedef VRRead<FReg> FRegRead; typedef VRWrite<FReg> FRegWrite;
  /**/                           typedef VRWrite<GReg> GRegWrite;

  struct GRegRead : public VRRead<GReg>, public unisim::util::sav::Addressings::Source
  {
    GRegRead( unsigned reg, unsigned idx ) : VRRead<GReg>( reg, idx ) {}
    virtual ConstNodeBase const* Eval( EvalSpace const& evs, ConstNodeBase const** ) const override
    {
      if (dynamic_cast<AddrEval const*>( &evs ))
        return new unisim::util::symbolic::ConstNode<uint32_t>( uint64_t(reg) << 28 );
      if (auto l = dynamic_cast<RelocEval const*>( &evs ))
        return new unisim::util::symbolic::ConstNode<uint32_t>( l->GetReg(reg) );
      return 0;
    };
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
    Load( ScalarType::id_t _tp, Expr const& _addr ) : addr(_addr), tp(_tp) {}
    virtual Load* Mutate() const override { return new Load( *this ); }
    virtual void Repr( std::ostream& sink ) const override { sink << "Load" << ScalarType(tp).name << "(" << addr << ")"; }
    virtual unsigned SubCount() const override { return 1; }
    virtual Expr const& GetSub(unsigned idx) const override { if (idx != 0) return ExprNode::GetSub(idx); return addr; }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<Load const&>( rhs ) ); }
    int compare( Load const& rhs ) const { return int(tp) - int(rhs.tp); }
    virtual ScalarType::id_t GetType() const override { return tp; }

    Expr addr;
    ScalarType::id_t tp;
  };

  struct Store : public Update
  {
    Store( ScalarType::id_t _tp, Expr const& _addr, Expr const& _value )
      : addr( _addr ), value( _value ), tp(_tp)
    {}
    virtual Store* Mutate() const override { return new Store( *this ); }
    virtual void Repr( std::ostream& sink ) const override { sink << "Store" << ScalarType(tp).name << "( " << addr << ", " << value <<  " )"; }
    virtual unsigned SubCount() const override { return 2; }
    virtual Expr const& GetSub(unsigned idx) const override { switch (idx) { case 0: return addr; case 1: return value; } return ExprNode::GetSub(idx); }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<Store const&>( rhs ) ); }
    int compare( Store const& rhs ) const { return int(tp) - int(rhs.tp); }

    Expr addr;
    Expr value;
    ScalarType::id_t tp;
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

  struct PC
  {
    typedef uint32_t register_type;
    char const* c_str() const { return "pc"; }
    int cmp( PC const& ) const { return 0; }
    ConstNodeBase const* eval(EvalSpace const& evs, ConstNodeBase const**) const
    {
      if (dynamic_cast<AddrEval const*>( &evs ))
        dont("pc-relative addressing");
      return 0;
    }
  };

  struct Flag : public unisim::util::identifier::Identifier<Flag>
  {
    typedef bool register_type;
    enum Code { N, Z, C, V, end } code;

    char const* c_str() const
    {
      static char const* names[] = {"n", "z", "c", "v", "NA"};
      return names[int(code)];
    }
    ConstNodeBase const* eval(EvalSpace const& evs, ConstNodeBase const**) const
    {
      if (dynamic_cast<AddrEval const*>( &evs ))
        dont("flag-dependant addressing");
      return 0;
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

  template <unsigned POS, unsigned SIZE> using RegisterField = unisim::component::cxx::processor::arm::RegisterField<POS, SIZE>;

  U32 GetPSR(RegisterField<31, 1> const&) { return U32(flags[Flag::N]); }
  U32 GetPSR(RegisterField<30, 1> const&) { return U32(flags[Flag::Z]); }
  U32 GetPSR(RegisterField<29, 1> const&) { return U32(flags[Flag::C]); }
  U32 GetPSR(RegisterField<28, 1> const&) { return U32(flags[Flag::V]); }

  template <class T> U32 GetPSR(T const&) { return dontread<U32>("cpsr"); }

  void SetPSR(RegisterField<31, 1> const&, U32 const& value) { flags[Flag::N] = BOOL(value); }
  void SetPSR(RegisterField<30, 1> const&, U32 const& value) { flags[Flag::Z] = BOOL(value); }
  void SetPSR(RegisterField<29, 1> const&, U32 const& value) { flags[Flag::C] = BOOL(value); }
  void SetPSR(RegisterField<28, 1> const&, U32 const& value) { flags[Flag::V] = BOOL(value); }

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

  Expr MemRead(ScalarType::id_t tp, Expr const& addr, bool aligned)
  {
    interface.memaccess( addr, ScalarType(tp).bitsize / 8, false, aligned );
    return new Load( tp, addr );
  }
  template <typename T>
  T MemReadT( U32 const& addr, bool aligned ) { return MemRead(T::GetType(), addr.expr, aligned); }

  U32  MemURead32( U32 const& addr ) { return MemReadT<U32>( addr, false ); }
  U16  MemURead16( U32 const& addr ) { return MemReadT<U16>( addr, false ); }
  U32  MemRead32( U32 const& addr ) { return MemReadT<U32>( addr, true ); }
  U16  MemRead16( U32 const& addr ) { return MemReadT<U16>( addr, true ); }
  U8  MemRead8( U32 const& addr ) { return MemReadT<U8>( addr, true ); }

  void MemWrite(ScalarType::id_t tp, Expr const& addr, Expr const& data, bool aligned)
  {

    interface.memaccess( addr, ScalarType(tp).bitsize / 8, true, aligned );
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

//     U32  GetVU32( unsigned idx ) { return U32( 0 ); }
//     void SetVU32( unsigned idx, U32 val ) {}
//     U64  GetVU64( unsigned idx ) { return U64( 0 ); }
//     void SetVU64( unsigned idx, U64 val ) {}
//     F32  GetVSR( unsigned idx ) { return F32( 0 ); }
//     void SetVSR( unsigned idx, F32 val ) {}
//     F64  GetVDR( unsigned idx ) { return F64( 0 ); }
//     void SetVDR( unsigned idx, F64 val ) {}

//     struct FPSCRTracer
//     {
//       FPSCRTracer( Scanner& _arch ) : arch( _arch ) {} Scanner& arch;
//       template <typename RF>  uint32_t Get( RF const& rf ) const { return 0; }
//       template <typename RF>  void     Set( RF const& rf, uint32_t value ) {}
//     };

//     FPSCRTracer fpscr;
//     FPSCRTracer& FPSCR() { return fpscr; }

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
};


#endif // ARM32SAV_SCANNER_HH