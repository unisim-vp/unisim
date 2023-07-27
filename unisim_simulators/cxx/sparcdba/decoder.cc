/*
 *  Copyright (c) 2009-2023,
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

#include "decoder.hh"

#include <unisim/component/cxx/processor/sparc/asi.hh>
#include <unisim/component/cxx/processor/sparc/trap.hh>
#include <unisim/component/cxx/processor/sparc/isa/sv8/arch.hh>
#include <unisim/util/symbolic/binsec/binsec.hh>
#include <unisim/util/symbolic/symbolic.hh>
#include <sparc.tcc>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <set>

namespace sparc {

struct Processor : public unisim::component::cxx::processor::sparc::isa::sv8::Arch<Processor>
{
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
  typedef unisim::util::symbolic::ValueType            ValueType;
  typedef unisim::util::symbolic::binsec::ActionNode   ActionNode;

  typedef unisim::component::cxx::processor::sparc::isa::sv8::Operation<Processor> Operation;
  typedef unisim::component::cxx::processor::sparc::ASI ASI;
  typedef unisim::component::cxx::processor::sparc::Trap_t::TrapType_t TrapType_t;  

  template <typename RID>
  static Expr newRegRead( RID id ) { return new unisim::util::symbolic::binsec::RegRead<RID>( id ); }

  template <typename RID>
  static Expr newRegWrite( RID id, Expr const& value ) { return new unisim::util::symbolic::binsec::RegWrite<RID>( id, value ); }

  struct Flag
    : public unisim::util::identifier::Identifier<Flag>
    , public unisim::util::symbolic::WithValueType<Flag>
  {
    typedef bool value_type;
    enum Code { C, V, Z, N, end } code;

    char const* c_str() const
    {
      unsigned idx( code );
      static char const* names[] = {"c", "v", "z", "n"};
      return (idx < 4) ? names[idx] : "NA";
    }

    void Repr(std::ostream& sink) const { sink << c_str(); }
    
    Flag() : code(end) {}
    Flag( Code _code ) : code(_code) {}
    Flag( char const* _code ) : code(end) { init( _code ); }
  };

  struct GPR
    : public unisim::util::identifier::Identifier<GPR>
    , public unisim::util::symbolic::WithValueType<GPR>
  {
    typedef uint32_t value_type;

    enum Code { g0, g1, g2, g3, g4, g5, g6, g7, o0, o1, o2, o3, o4, o5, o6, o7, l0, l1, l2, l3, l4, l5, l6, l7, i0, i1, i2, i3, i4, i5, i6, i7, end } code;

    char const* c_str() const
    {
      switch (code)
        {
        case g0: return "g0"; case g1: return "g1"; case g2: return "g2"; case g3: return "g3"; case g4: return "g4"; case g5: return "g5"; case g6: return "g6"; case g7: return "g7";
        case o0: return "o0"; case o1: return "o1"; case o2: return "o2"; case o3: return "o3"; case o4: return "o4"; case o5: return "o5"; case o6: return "o6"; case o7: return "o7";
        case l0: return "l0"; case l1: return "l1"; case l2: return "l2"; case l3: return "l3"; case l4: return "l4"; case l5: return "l5"; case l6: return "l6"; case l7: return "l7";
        case i0: return "i0"; case i1: return "i1"; case i2: return "i2"; case i3: return "i3"; case i4: return "i4"; case i5: return "i5"; case i6: return "i6"; case i7: return "i7";
        case end: break;
        }
      return "NA";
    }

    void Repr( std::ostream& sink ) const { sink << c_str(); }

    GPR() : code(end) {}
    GPR( Code _code ) : code(_code) {}
    GPR( unsigned idx ) : code(Code(idx)) {}
    GPR( char const* _code ) : code(end) { init( _code ); }
  };
  
  struct YREG
    : public unisim::util::symbolic::WithValueType<YREG>
  {
    typedef uint32_t value_type;
    void Repr( std::ostream& sink ) const { sink << "y"; }
    int cmp( YREG const& rhs ) const { return 0; }
  };
  
  struct Conditions
  {
    Conditions(Processor& _scanner) : scanner(_scanner) {}
    bool Eval( BOOL&& res ) const { return scanner.Test(res); }
    BOOL F() const { return BOOL(false); }
    BOOL T() const { return BOOL(true); }
    BOOL N() const { return scanner.flags[Flag::N]; }
    BOOL Z() const { return scanner.flags[Flag::Z]; }
    BOOL V() const { return scanner.flags[Flag::V]; }
    BOOL C() const { return scanner.flags[Flag::C]; }
    Processor& scanner;
  };

  struct FConditions
  {
    FConditions(Processor& _scanner) : scanner(_scanner) {}
    bool Eval( BOOL&& res ) const { return scanner.Test(res); }
    BOOL F() const { return BOOL(false); }
    BOOL T() const { return BOOL(true); }
    BOOL E() const { throw 0; return BOOL(false); }
    BOOL G() const { throw 0; return BOOL(false); }
    BOOL L() const { throw 0; return BOOL(false); }
    BOOL U() const { throw 0; return BOOL(false); }
    Processor& scanner;
  };

  // //   =====================================================================
  // //   =                      Construction/Destruction                     =
  // //   =====================================================================
    
  Processor(U32 const& insn_addr)
    : path(0)
    , gpr()
    , yreg(newRegRead(YREG()))
    , flags()
    , current_instruction_address(insn_addr)
    , branch_addr()
    , branch_type(B_JMP)
    , branch_annul(false)
  {
    for (GPR reg; reg.next();)
      gpr[reg.idx()] = newRegRead(reg);
    for (Flag flag; flag.next();)
      flags[flag.idx()] = newRegRead(flag);
  }
  
  bool
  close( Processor const& ref, uint32_t linear_nia )
  {
    bool complete = path->close();

    if (branch_addr.expr.node)
      {
        if (branch_type == B_CALL)
          path->add_sink( Expr( new unisim::util::symbolic::binsec::Call<uint32_t>( branch_addr.expr, linear_nia ) ) );
        else
          path->add_sink( Expr( new unisim::util::symbolic::binsec::Branch( branch_addr.expr ) ) );
      }

    if (unpredictable)
      {
        path->add_sink( Expr( new unisim::util::symbolic::binsec::AssertFalse() ) );
        return complete;
      }
    
    for (GPR reg; reg.next();)
      if (gpr[reg.idx()].expr != ref.gpr[reg.idx()].expr)
        path->add_sink( newRegWrite( reg, gpr[reg.idx()].expr ) );

    if (yreg.expr != ref.yreg.expr)
      path->add_sink( newRegWrite( YREG(), yreg.expr ) );
      
    for (Flag flag; flag.next();)
      if (flags[flag.idx()].expr != ref.flags[flag.idx()].expr)
        path->add_sink( newRegWrite( flag, flags[flag.idx()].expr ) );

    for (std::set<Expr>::const_iterator itr = stores.begin(), end = stores.end(); itr != end; ++itr)
      path->add_sink( *itr );
    
    return complete;
  }

  //   =====================================================================
  //   =                 Internal Instruction Control Flow                 =
  //   =====================================================================
    
  void UndefinedInstruction(Operation const&) { throw 0; }
    
  bool concretize( Expr cexp )
  {
    if (unisim::util::symbolic::ConstNodeBase const* cnode = cexp.ConstSimplify())
      return dynamic_cast<unisim::util::symbolic::ConstNode<bool> const&>(*cnode).value;

    bool predicate = path->proceed( cexp );
    path = path->next( predicate );
    return predicate;
  }
  
  template <typename T>
  bool Test( unisim::util::symbolic::SmartValue<T> const& cond )
  {
    if (not cond.expr.good())
      throw std::logic_error( "Not a valid condition" );

    return concretize( BOOL(cond).expr );
  }
  
  //   =====================================================================
  //   =             General Purpose Registers access methods              =
  //   =====================================================================
    
  void SetGPR( unsigned id, U32 const& value ) { if (id != 0) gpr[id] = value; }
  U32  GetGPR( unsigned id ) { return id == 0 ? U32(0) : gpr[id];  }
  U32  GetY() { return yreg; }
  void SetY(U32 const& value) { yreg = value; }

  //   =====================================================================
  //   =                  Arithmetic Flags access methods                  =
  //   =====================================================================

  void SetNZVC(BOOL const& N, BOOL const& Z, BOOL const& V, BOOL const& C)
  {
    flags[Flag::N] = N; flags[Flag::Z] = Z; flags[Flag::V] = V; flags[Flag::C] = C;
  }
  BOOL GetCarry() { return flags[Flag::C]; }
  BOOL GetN() { return flags[Flag::N]; }
  BOOL GetV() { return flags[Flag::V]; }
  BOOL GetET() { throw 0; return BOOL(); }
  void SetET(BOOL) { throw 0; }
  BOOL GetS() { throw 0; return BOOL(); }
  void SetS(BOOL) { throw 0; }
  BOOL GetPS() { throw 0; return BOOL(); }
  void SetPS(BOOL) { throw 0; }
  
  //   ====================================================================
  //   =                 Vector  Registers access methods                 
  //   ====================================================================

  void SetF32( unsigned id, F32 const& value ) { throw 0; }
  void SetF64( unsigned id, F64 const& value ) { throw 0; }
  void SetS32( unsigned id, S32 const& value ) { throw 0; }
  F32  GetF32( unsigned id ) { throw 0; return F32(); }
  F64  GetF64( unsigned id ) { throw 0; return F64(); }
  S32  GetS32( unsigned id ) { throw 0; return S32(); }
  void SetFCC( BOOL const& l, BOOL const& g ) { throw 0; }
  U32  GetFSR() { throw 0; return U32(); }
  void SetFSR( U32 const& ) { throw 0; }

  //   U8  GetVU8 ( unsigned reg, unsigned sub ) { return VectorStorage<U8> (reg)[sub]; }
  //   U16 GetVU16( unsigned reg, unsigned sub ) { return VectorStorage<U16>(reg)[sub]; }
  //   U32 GetVU32( unsigned reg, unsigned sub ) { return VectorStorage<U32>(reg)[sub]; }
  //   U64 GetVU64( unsigned reg, unsigned sub ) { return VectorStorage<U64>(reg)[sub]; }
  //   S8  GetVS8 ( unsigned reg, unsigned sub ) { return VectorStorage<S8> (reg)[sub]; }
  //   S16 GetVS16( unsigned reg, unsigned sub ) { return VectorStorage<S16>(reg)[sub]; }
  //   S32 GetVS32( unsigned reg, unsigned sub ) { return VectorStorage<S32>(reg)[sub]; }
  //   S64 GetVS64( unsigned reg, unsigned sub ) { return VectorStorage<S64>(reg)[sub]; }
  
  //   void SetVU8 ( unsigned reg, unsigned sub, U8  value ) { VectorStorage<U8> (reg)[sub] = value; }
  //   void SetVU16( unsigned reg, unsigned sub, U16 value ) { VectorStorage<U16>(reg)[sub] = value; }
  //   void SetVU32( unsigned reg, unsigned sub, U32 value ) { VectorStorage<U32>(reg)[sub] = value; }
  //   void SetVU64( unsigned reg, unsigned sub, U64 value ) { VectorStorage<U64>(reg)[sub] = value; }
  //   void SetVS8 ( unsigned reg, unsigned sub, S8  value ) { VectorStorage<S8> (reg)[sub] = value; }
  //   void SetVS16( unsigned reg, unsigned sub, S16 value ) { VectorStorage<S16>(reg)[sub] = value; }
  //   void SetVS32( unsigned reg, unsigned sub, S32 value ) { VectorStorage<S32>(reg)[sub] = value; }
  //   void SetVS64( unsigned reg, unsigned sub, S64 value ) { VectorStorage<S64>(reg)[sub] = value; }
  
  //   void SetVU8 ( unsigned reg, U8 value )  { VectorZeroedStorage<U8> (reg)[0] = value; }
  //   void SetVU16( unsigned reg, U16 value ) { VectorZeroedStorage<U16>(reg)[0] = value; }
  //   void SetVU32( unsigned reg, U32 value ) { VectorZeroedStorage<U32>(reg)[0] = value; }
  //   void SetVU64( unsigned reg, U64 value ) { VectorZeroedStorage<U64>(reg)[0] = value; }
  //   void SetVS8 ( unsigned reg, S8 value )  { VectorZeroedStorage<S8> (reg)[0] = value; }
  //   void SetVS16( unsigned reg, S16 value ) { VectorZeroedStorage<S16>(reg)[0] = value; }
  //   void SetVS32( unsigned reg, S32 value ) { VectorZeroedStorage<S32>(reg)[0] = value; }
  //   void SetVS64( unsigned reg, S64 value ) { VectorZeroedStorage<S64>(reg)[0] = value; }
  
  //   void ClearHighV( unsigned reg, unsigned bytes ) { for (unsigned idx = bytes; idx < VUnion::BYTECOUNT; idx+=1 ) vector_data[reg][idx] = 0; }

  //   =====================================================================
  //   =              Special/System Registers access methods              =
  //   =====================================================================

  bool asr_perm(unsigned) { return false; }
  U32 rdasr(unsigned) { throw 0; return U32(); }
  void wrasr(unsigned, U32) { throw 0; }

  bool InvalidWindow(int) { return false; }
  void RotateWindow(int) { throw 0; }
  unsigned nwindows() { throw 0; return 0; }

  bool IsSuper() const { return false; }

  U32 GetPSR() { throw 0; return U32(); }
  U32 GetWIM() { throw 0; return U32(); }
  U32 GetTBR() { throw 0; return U32(); }
  void SetPSR(U32) { throw 0; }
  void SetWIM(U32) { throw 0; }
  void SetTBR(U32) { throw 0; }
  
  //   =====================================================================
  //   =                      Control Transfer methods                     =
  //   =====================================================================
  
  enum branch_type_t { B_JMP = 0, B_CALL, B_RET };
  U32  GetPC() { return current_instruction_address; }
  void DelayBranch(U32 target) { branch_addr = target; }
  void SetAnnul(bool annul) { branch_annul = annul; }
  
  void Abort( TrapType_t trap ) { throw 0; }
  void StopFetch() { throw 0; }
  void SWTrap(U32) { throw 0; }

  //   =====================================================================
  //   =                       Memory access methods                       =
  //   =====================================================================

  template <typename T> T MemRead( T const&, ASI asi, U32 const& addr )
  {
    if (asi.code != asi.user_data) throw 0;
    auto tp = T::GetType();
    return T( Expr(new unisim::util::symbolic::binsec::Load( addr.expr, tp.bitsize/8, 0, true )) );
  }
  template <typename T> void MemWrite(ASI asi, U32 const& addr, T const& data)
  {
    if (asi.code != asi.user_data) throw 0;
    auto tp = T::GetType();
    stores.insert( new unisim::util::symbolic::binsec::Store( addr.expr, data.expr, tp.bitsize/8, 0, true ) );
  }

  template <typename T> T FMemRead( T const&, ASI asi, U32 const& addr ) { throw 0; return T(); }
  template <typename T> void FMemWrite( ASI asi, U32 const& addr, T const& ) { throw 0; }

  unisim::component::cxx::processor::sparc::ASI rqasi() const { return ASI::user_data; }
  
  //   =====================================================================
  //   =                         Processor Storage                         =
  //   =====================================================================

  // // typedef u64_t addr_t;
  // typedef unisim::component::cxx::processor::sparc::ASI ASI;
  // typedef unisim::component::cxx::processor::sparc::Trap_t Trap_t;
  
  // typedef SmartValue<float>       F32;
  // typedef SmartValue<double>      F64;
    
  // struct ISA : public Decoder
  // {
  //   ISA();
  //   ~ISA();
  //   Operation* decode(uint32_t addr, uint32_t code, std::string& disasm);
  // };

  // typedef unisim::util::symbolic::Expr Expr;
  // typedef unisim::util::symbolic::ExprNode ExprNode;
  // typedef unisim::util::symbolic::ValueType ValueType;
  // typedef unisim::util::symbolic::ConstNodeBase ConstNodeBase;
  // typedef unisim::util::symbolic::EvalSpace EvalSpace;

  // void noexec( Operation const& op )
  // {
  //   throw unisim::util::sav::Untestable("Not implemented");
  // }

  // struct Update : public ExprNode
  // {
  //   virtual ValueType const* GetType() const override { return unisim::util::symbolic::NoValueType(); }
  // };

  // struct RegReadBase : public unisim::util::symbolic::ExprNode
  // {
  //   virtual char const* GetRegName() const = 0;
  //   virtual void Repr( std::ostream& sink ) const override { sink << "RegRead(" << GetRegName() << ")"; }
  //   virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<RegReadBase const&>( rhs ) ); };
  //   int compare( RegReadBase const& rhs ) const { return 0; }
  // };

  // template <typename RID>
  // struct RegRead : public RegReadBase
  // {
  //   RegRead( RID _id ) : id(_id) {}
  //   virtual RegRead* Mutate() const override { return new RegRead( *this ); }
  //   virtual char const* GetRegName() const override { return id.c_str(); };
  //   virtual ValueType const* GetType() const { return RID::GetType(); }
  //   virtual unsigned SubCount() const override { return 0; }
  //   virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<RegRead const&>( rhs ) ); };
  //   int compare( RegRead const& rhs ) const { if (int delta = RegReadBase::compare( rhs )) return delta; return id.cmp( rhs.id ); }
  //   ConstNodeBase const* Eval( EvalSpace const& evs, ConstNodeBase const** cnbs) const override { return id.eval(evs, cnbs); }

  //   RID id;
  // };

  // template <typename RID>
  // Expr newRegRead( RID _id ) { return new RegRead<RID>(_id); }

  // struct VRReadBase : public unisim::util::sav::VirtualRegister, public ExprNode
  // {
  //   typedef unisim::util::sav::VirtualRegister VirtualRegister;
  //   VRReadBase( unsigned reg,  unsigned idx ) : VirtualRegister(reg, idx), ExprNode() {}
  //   virtual int cmp(ExprNode const& rhs) const override { return compare( dynamic_cast<VirtualRegister const&>( rhs ) ); }
  //   virtual unsigned SubCount() const override { return 0; }
  // };

  // template <class T>
  // struct VRRead : public VRReadBase
  // {
  //   typedef VRRead<T> this_type;
  //   VRRead( unsigned reg,  unsigned idx ) : VRReadBase(reg, idx) {}
  //   virtual ExprNode* Mutate() const override { return new this_type(*this); }
  //   virtual void Repr( std::ostream& sink ) const override { sink << T::name() << "Read( " << idx << ", " << reg << " )"; }
  //   virtual ValueType const* GetType() const override { return T::GetType(); }
  // };

  // struct VRWriteBase : public unisim::util::sav::VirtualRegister, public Update
  // {
  //   typedef unisim::util::sav::VirtualRegister VirtualRegister;
  //   VRWriteBase( unsigned reg, unsigned idx, Expr const& _val ) : VirtualRegister(reg, idx), Update(), val(_val) {}
  //   virtual int cmp(ExprNode const& rhs) const override { return compare( dynamic_cast<VirtualRegister const&>( rhs ) ); }
  //   virtual unsigned SubCount() const override { return 1; }
  //   virtual Expr const& GetSub(unsigned idx) const override { if (idx == 0) return val; return ExprNode::GetSub(idx); }
  //   Expr val;
  // };

  // template <class T>
  // struct VRWrite : public VRWriteBase
  // {
  //   typedef VRWrite<T> this_type;
  //   VRWrite( unsigned reg,  unsigned idx, Expr const& val ) : VRWriteBase(reg, idx, val) {}
  //   virtual ExprNode* Mutate() const override { return new this_type(*this); }
  //   virtual void Repr( std::ostream& sink ) const override { sink << T::name() << "Write( " << reg << ", " << idx << ", " << val << " )"; }
  // };

  // struct GReg { static ValueType const* GetType() { return unisim::util::symbolic::CValueType(uint32_t()); } static char const* name() { return "GReg"; } };

  // // typedef VRRead<VReg> VRegRead; typedef VRWrite<VReg> VRegWrite;
  // // typedef VRRead<FReg> FRegRead; typedef VRWrite<FReg> FRegWrite;
  // /**/                           typedef VRWrite<GReg> GRegWrite;

  // struct Y : public unisim::util::symbolic::WithValueType<Y>
  // {
  //   typedef uint32_t value_type;
  //   char const* c_str() const { return "%y"; }
  //   int cmp( Y const& ) const { return 0; }
  //   ConstNodeBase const* eval(EvalSpace const& evs, ConstNodeBase const**) const
  //   {
  //     if (dynamic_cast<AddrEval const*>( &evs ))
  //       throw 0;
  //     return 0;
  //   }
  // };
    
  // struct Load : public ExprNode
  // {
  //   Load( ValueType const* _tp, ASI _asi, Expr const& _addr ) : addr(_addr), asi(_asi), tp(_tp) {}
  //   virtual Load* Mutate() const override { return new Load( *this ); }
  //   virtual void Repr( std::ostream& sink ) const override { tp->GetName( sink << "Load" ); sink << "(" << addr << ")"; }
  //   virtual unsigned SubCount() const override { return 1; }
  //   virtual Expr const& GetSub(unsigned idx) const override { if (idx != 0) return ExprNode::GetSub(idx); return addr; }
  //   virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<Load const&>( rhs ) ); }
  //   int compare( Load const& rhs ) const { return tp->cmp(*rhs.tp); }
  //   virtual ValueType const* GetType() const { return tp; }

  //   Expr addr;
  //   ASI asi;
  //   ValueType const* tp;
  // };

  // struct Store : public Update
  // {
  //   Store( ValueType const* _tp, Expr const& _addr, Expr const& _value )
  //     : addr( _addr ), value( _value ), tp(_tp)
  //   {}
  //   virtual Store* Mutate() const override { return new Store( *this ); }
  //   virtual void Repr( std::ostream& sink ) const override { tp->GetName( sink << "Store" ); sink << "( " << addr << ", " << value <<  " )"; }
  //   virtual unsigned SubCount() const override { return 2; }
  //   virtual Expr const& GetSub(unsigned idx) const override { switch (idx) { case 0: return addr; case 1: return value; } return ExprNode::GetSub(idx); }
  //   virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<Store const&>( rhs ) ); }
  //   int compare( Store const& rhs ) const { return tp->cmp(*rhs.tp); }
      
  //   Expr addr;
  //   Expr value;
  //   ValueType const* tp;
  // };

  // struct AddrEval : public EvalSpace {};
  
  // struct RelocEval : public EvalSpace
  // {
  //   RelocEval( uint32_t const* _regvalues, uint32_t _address ) : regvalues(_regvalues), address(_address) {}
  //   uint32_t const* regvalues;
  //   uint32_t address;
  // };

  // struct GRegRead : public VRRead<GReg>, public unisim::util::sav::Addressings::Source
  // {
  //   GRegRead( unsigned reg, unsigned idx ) : VRRead<GReg>( reg, idx ) {}
  //   virtual ConstNodeBase const* Eval( EvalSpace const& evs, ConstNodeBase const** ) const override
  //   {
  //     if (dynamic_cast<AddrEval const*>( &evs ))
  //       return new unisim::util::symbolic::ConstNode<uint32_t>( uint32_t(reg) << 16 );
  //     if (auto l = dynamic_cast<RelocEval const*>( &evs ))
  //       return new unisim::util::symbolic::ConstNode<uint32_t>( l->regvalues[idx] );
  //     return 0;
  //   };
  // };

  // struct RegWriteBase : public Update
  // {
  //   RegWriteBase( Expr const& _value ) : value( _value ) {}
  //   virtual unsigned SubCount() const override { return 1; }
  //   virtual Expr const& GetSub(unsigned idx) const override { if (idx != 0) return ExprNode::GetSub(idx); return value; }
  //   virtual char const* GetRegName() const = 0;
  //   virtual void Repr( std::ostream& sink ) const override { sink << GetRegName() << " := " << value; }
  //   virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<RegWriteBase const&>( rhs ) ); }
  //   int compare( RegWriteBase const& rhs ) const { return 0; }
  //   Expr value;
  // };

  // template <typename RID>
  // struct RegWrite : public RegWriteBase
  // {
  //   typedef RegWrite<RID> this_type;
  //   RegWrite( RID _id, Expr const& _value ) : RegWriteBase( _value ), id( _id ) {}
  //   virtual this_type* Mutate() const override { return new this_type( *this ); }
  //   virtual char const* GetRegName() const override { return id.c_str(); };
  //   virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<this_type const&>( rhs ) ); }
  //   int compare( this_type const& rhs ) const { if (int delta = RegWriteBase::compare( rhs )) return delta; return id.cmp( rhs.id ); }

  //   RID id;
  // };

  // template <typename RID> RegWrite<RID>* newRegWrite( RID _id, Expr const& _value ) { return new RegWrite<RID>(_id, _value); }

  // ~Processor();

  // void step( Operation const& op );
  // bool close( Processor const& ref );

  // static void dont(char const* reason) { throw unisim::util::sav::Untestable(reason); }

  // /***************************/
  // /*** Architectural state ***/
  // /***************************/

  // void UndefinedInstruction( Operation const& op ) { UndefinedInstruction(); }
  // void UndefinedInstruction() { throw 0; }


  // bool concretize( Expr cexp );
  
  // template <typename T>
  // bool Test( unisim::util::symbolic::SmartValue<T> const& cond )
  // {
  //   if (not cond.expr.good())
  //     throw std::logic_error( "Not a valid condition" );

  //   return concretize( BOOL(cond).expr );
  // }

  ActionNode*      path;
  U32              gpr[32];
  U32              yreg;
  BOOL             flags[Flag::end];
  U32              current_instruction_address;
  U32              branch_addr;
  branch_type_t    branch_type;
  bool             branch_annul;
  bool             unpredictable;
  std::set<Expr>   stores;
};

struct Translator
{
  typedef unisim::component::cxx::processor::sparc::isa::sv8::Decoder<Processor> Decoder;
  typedef unisim::component::cxx::processor::sparc::isa::sv8::Operation<Processor> Operation;
  
  typedef unisim::util::symbolic::binsec::ActionNode ActionNode;
  
  Translator( uint32_t _addr, uint32_t _code )
    : coderoot(new ActionNode), addr(_addr), code(_code)
  {}
  ~Translator() { delete coderoot; }
   
  void
  translate( std::ostream& sink )
  {
    sink << "(address . " << unisim::util::symbolic::binsec::dbx(8, addr) << ")\n";
    sink << "(opcode . " << unisim::util::symbolic::binsec::dbx(4, code) << ")\n";
    sink << "(size . 4)\n";
    
    struct Instruction
    {
      Instruction(uint32_t addr, uint32_t code)
        : operation(0)
      {
        Decoder decoder;
        operation = decoder.NCDecode( addr, code );
      }
      ~Instruction() { delete operation; }
      Operation* operator -> () { return operation; }

      Operation* operation;
    };

    Instruction instruction(addr, code);

    if (not instruction.operation)
      {
        sink << "(illegal)\n";
        return;
      }

    Processor::U32      insn_addr = unisim::util::symbolic::make_const(addr); //< concrete instruction address
    // Processor::U64      insn_addr = Expr(new InstructionAddress); //< symbolic instruction address
    Processor reference( insn_addr );

    // Disassemble
    sink << "(mnemonic . \"";
    try { instruction->disasm( sink, addr ); }
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
            end = state.close( reference, addr + 4 );
          }
        coderoot->simplify();
        coderoot->commit_stats();
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
      sink << "(" << unisim::util::symbolic::binsec::dbx(8, addr) << ',' << std::dec << itr->first << ") " << itr->second << std::endl;
  }

  ActionNode*               coderoot;
  uint32_t                  addr;
  uint32_t                  code;
};

void
Decoder::process( std::ostream& sink, uint32_t addr, uint32_t code )
{
  Translator translator( addr, code );

  translator.translate( sink );
}

}
