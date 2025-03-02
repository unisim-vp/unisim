/*
 *  Copyright (c) 2019-2023,
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

#ifndef LEONSAV_SCANNER_HH
#define LEONSAV_SCANNER_HH

#include <test.hh>
#include <unisim/component/cxx/processor/sparc/isa_sv8.hh>
#include <unisim/component/cxx/processor/sparc/isa/sv8/arch.hh>
#include <unisim/component/cxx/processor/sparc/trap.hh>
#include <unisim/component/cxx/processor/sparc/asi.hh>
// #include <unisim/component/cxx/vector/vector.hh>
// #include <unisim/util/sav/sav.hh>
// #include <unisim/util/symbolic/vector/vector.hh>
// #include <unisim/util/symbolic/symbolic.hh>
// #include <bitset>
// #include <set>
// #include <memory>
#include <inttypes.h>

struct Scanner : public unisim::component::cxx::processor::sparc::isa::sv8::Arch<Scanner>
{
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
  typedef unisim::component::cxx::processor::sparc::ASI ASI;
  typedef unisim::component::cxx::processor::sparc::Trap_t Trap_t;
  
  typedef SmartValue<float>       F32;
  typedef SmartValue<double>      F64;
    
  typedef unisim::component::cxx::processor::sparc::isa::sv8::Operation<Scanner> Operation;
  typedef unisim::component::cxx::processor::sparc::isa::sv8::Decoder<Scanner> Decoder;

  struct ISA : public Decoder
  {
    ISA();
    ~ISA();
    Operation* decode(uint32_t addr, uint32_t code, std::string& disasm);
  };

  typedef unisim::util::symbolic::Expr Expr;
  typedef unisim::util::symbolic::ExprNode ExprNode;
  typedef unisim::util::symbolic::ValueType ValueType;
  typedef unisim::util::symbolic::ConstNodeBase ConstNodeBase;

  void noexec( Operation const& op )
  {
    throw unisim::util::sav::Untestable("Not implemented");
  }

  struct Update : public ExprNode
  {
    virtual ValueType GetType() const override { return unisim::util::symbolic::NoValueType(); }
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
    virtual ValueType GetType() const { return RID::GetType(); }
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
    virtual ValueType GetType() const override { return T::GetType(); }
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

  struct GReg { static ValueType GetType() { return unisim::util::symbolic::CValueType(uint32_t()); } static char const* name() { return "GReg"; } };

  // typedef VRRead<VReg> VRegRead; typedef VRWrite<VReg> VRegWrite;
  // typedef VRRead<FReg> FRegRead; typedef VRWrite<FReg> FRegWrite;
  /**/                           typedef VRWrite<GReg> GRegWrite;

  struct Y : public unisim::util::symbolic::WithValueType<Y>
  {
    typedef uint32_t value_type;
    char const* c_str() const { return "%y"; }
    int cmp( Y const& ) const { return 0; }
    ConstNodeBase const* eval(ConstNodeBase const**) const
    {
      return 0;
    }
  };
    
  struct Load : public ExprNode
  {
    Load( ValueType _tp, ASI _asi, Expr const& _addr ) : addr(_addr), asi(_asi), tp(_tp) {}
    virtual Load* Mutate() const override { return new Load( *this ); }
    virtual void Repr( std::ostream& sink ) const override { tp.Repr( sink << "Load" ); sink << "(" << addr << ")"; }
    virtual unsigned SubCount() const override { return 1; }
    virtual Expr const& GetSub(unsigned idx) const override { if (idx != 0) return ExprNode::GetSub(idx); return addr; }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<Load const&>( rhs ) ); }
    int compare( Load const& rhs ) const { return tp.cmp(rhs.tp); }
    virtual ValueType GetType() const { return tp; }

    Expr addr;
    ASI asi;
    ValueType tp;
  };

  struct Store : public Update
  {
    Store( ValueType _tp, Expr const& _addr, Expr const& _value )
      : addr( _addr ), value( _value ), tp(_tp)
    {}
    virtual Store* Mutate() const override { return new Store( *this ); }
    virtual void Repr( std::ostream& sink ) const override { tp.Repr( sink << "Store" ); sink << "( " << addr << ", " << value <<  " )"; }
    virtual unsigned SubCount() const override { return 2; }
    virtual Expr const& GetSub(unsigned idx) const override { switch (idx) { case 0: return addr; case 1: return value; } return ExprNode::GetSub(idx); }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<Store const&>( rhs ) ); }
    int compare( Store const& rhs ) const { return tp.cmp(rhs.tp); }
      
    Expr addr;
    Expr value;
    ValueType tp;
  };

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

  struct Flag : public unisim::util::identifier::Identifier<Flag>, public unisim::util::symbolic::WithValueType<Flag>
  {
    typedef bool value_type;
    enum Code { C, V, Z, N, end } code;

    char const* c_str() const
    {
      unsigned idx( code );
      static char const* names[] = {"c", "v", "z", "n"};
      return (idx < 4) ? names[idx] : "NA";
    }

    Flag() : code(end) {}
    Flag( Code _code ) : code(_code) {}
    Flag( char const* _code ) : code(end) { init( _code ); }
  };

  struct ExpectedAddress : public unisim::util::symbolic::ExprNode
  {
    ExpectedAddress() : unisim::util::symbolic::ExprNode() {}
    virtual ExpectedAddress* Mutate() const override { return new ExpectedAddress( *this ); }
    virtual int cmp(ExprNode const& rhs) const override { return 0; }
    virtual unsigned SubCount() const override { return 0; }
    virtual void Repr( std::ostream& sink ) const override { sink << "ExpectedAddress()"; }
    typedef unisim::util::symbolic::ConstNodeBase ConstNodeBase;
    typedef unisim::util::symbolic::ValueType ValueType;
    virtual ValueType GetType() const override { return unisim::util::symbolic::CValueType(uint32_t()); }
  };
      
  Scanner( Interface& iif );
  ~Scanner();

  void step( Operation const& op );
  bool close( Scanner const& ref );

  static void dont(char const* reason) { throw unisim::util::sav::Untestable(reason); }

  /***************************/
  /*** Architectural state ***/
  /***************************/

  struct Conditions
  {
    Conditions(Scanner& _scanner) : scanner(_scanner) {}
    bool Eval( BOOL&& res ) const { return scanner.Test(res); }
    BOOL F() const { return BOOL(false); }
    BOOL T() const { return BOOL(true); }
    BOOL N() const { return scanner.flags[Flag::N]; }
    BOOL Z() const { return scanner.flags[Flag::Z]; }
    BOOL V() const { return scanner.flags[Flag::V]; }
    BOOL C() const { return scanner.flags[Flag::C]; }
    Scanner& scanner;
  };

  struct FConditions
  {
    FConditions(Scanner& _scanner) : scanner(_scanner) {}
    bool Eval( BOOL&& res ) const { return scanner.Test(res); }
    BOOL F() const { return BOOL(false); }
    BOOL T() const { return BOOL(true); }
    BOOL E() const { dont("floating-point"); return BOOL(false); }
    BOOL G() const { dont("floating-point"); return BOOL(false); }
    BOOL L() const { dont("floating-point"); return BOOL(false); }
    BOOL U() const { dont("floating-point"); return BOOL(false); }
    Scanner& scanner;
  };

  void UndefinedInstruction( Operation const& op ) { UndefinedInstruction(); }
  void UndefinedInstruction() { dont("undefined"); }

  bool IsSuper() const { return false; }

  void Abort( Trap_t::TrapType_t trap ) { dont("trap"); }
  void StopFetch() { dont("trap"); }
  void SWTrap(U32) { dont("trap"); }

  void gregtouch(unsigned id, bool write);
  U32  gregread(unsigned id) { gregtouch(id, false); return gprs[id]; }
  void gregwrite(unsigned id, U32 const& value) { gregtouch(id, true); gprs[id] = value; }

  void SetGPR( unsigned id, U32 const& value ) { if (id != 0) gregwrite(id, value); }
  U32  GetGPR( unsigned id ) { if (id != 0) return gregread(id); return U32(0); }
  U32  GetY() { interface.yaccess = true; return y; }
  void SetY(U32 const& value) { y = value; interface.yaccess = true; }
  
  void SetNZVC(BOOL const& N, BOOL const& Z, BOOL const& V, BOOL const& C)
  {
    flags[Flag::N] = N.expr;
    flags[Flag::Z] = Z.expr;
    flags[Flag::V] = V.expr;
    flags[Flag::C] = C.expr;
  }
  BOOL GetCarry() { return flags[0]; }
  BOOL GetN() { return flags[3]; }
  BOOL GetV() { return flags[1]; }
  BOOL GetET() { dont("trap"); return BOOL(); }
  void SetET(BOOL) { dont("trap"); }
  BOOL GetS() { dont("trap"); return BOOL(); }
  void SetS(BOOL) { dont("trap"); }
  BOOL GetPS() { dont("trap"); return BOOL(); }
  void SetPS(BOOL) { dont("trap"); }
  

  void SetF32( unsigned id, F32 const& value ) { dont("floating-point"); }
  void SetF64( unsigned id, F64 const& value ) { dont("floating-point"); }
  void SetS32( unsigned id, S32 const& value ) { dont("floating-point"); }
  F32  GetF32( unsigned id ) { dont("floating-point"); return F32(); }
  F64  GetF64( unsigned id ) { dont("floating-point"); return F64(); }
  S32  GetS32( unsigned id ) { dont("floating-point"); return S32(); }
  void SetFCC( BOOL const& l, BOOL const& g ) { dont("floating-point"); }
  U32  GetFSR() { dont("floating-point"); return U32(); }
  void SetFSR( U32 const& ) { dont("floating-point"); }

  template <typename T> T MemRead( T const&, ASI asi, U32 const& addr )
  {
    interface.memaccess( addr.expr, false );
    return T( Expr(new Load( T::GetType(), asi, addr.expr )) );
  }
  template <typename T> void MemWrite(ASI asi, U32 const& addr, T const& data)
  {
    interface.memaccess( addr.expr, true );
    stores.insert( new Store( T::GetType(), addr.expr, data.expr ) );
  }

  template <typename T> T FMemRead( T const&, ASI asi, U32 const& addr ) { dont("floating-point"); return T(); }
  template <typename T> void FMemWrite( ASI asi, U32 const& addr, T const& ) { dont("floating-point"); }

  unisim::component::cxx::processor::sparc::ASI rqasi() const { return ASI::user_data; }

  bool concretize( Expr cexp );
  
  template <typename T>
  bool Test( unisim::util::symbolic::SmartValue<T> const& cond )
  {
    if (not cond.expr.good())
      throw std::logic_error( "Not a valid condition" );

    return concretize( BOOL(cond).expr );
  }

  bool InvalidWindow(int) { return false; }
  void RotateWindow(int) { dont("register-window"); }

  U32  GetPC() { dont("branch"); return U32(); }
  void DelayBranch(U32 target, branch_type_t) { dont("branch"); }
  void SetAnnul(bool annul) { dont("branch"); }

  bool asr_perm(unsigned) { return false; }
  U32 rdasr(unsigned) { dont("asr"); return U32(); }
  void wrasr(unsigned, U32) { dont("asr"); }

  U32 GetPSR() { dont("psr"); return U32(); }
  U32 GetWIM() { dont("wim"); return U32(); }
  U32 GetTBR() { dont("tbr"); return U32(); }
  void SetPSR(U32) { dont("psr"); }
  void SetWIM(U32) { dont("wim"); }
  void SetTBR(U32) { dont("tbr"); }
  unsigned nwindows() { dont("windows"); return 0; }
  
  Interface&     interface;
  unisim::util::sav::ActionNode*    path;
  
  U32 gprs[32];
  U32 y;
  std::set<Expr> stores;
  Expr flags[4];
};

#endif // LEONSAV_SCANNER_HH

