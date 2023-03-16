/*
 *  Copyright (c) 2019-2020,
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

#ifndef ARM64SAV_SCANNER_HH
#define ARM64SAV_SCANNER_HH

#include <test.hh>
#include <unisim/component/cxx/processor/arm/isa_arm64.hh>
#include <unisim/component/cxx/processor/arm/regs64/cpu.hh>
#include <unisim/component/cxx/processor/arm/register_field.hh>
#include <unisim/util/sav/sav.hh>
#include <unisim/util/symbolic/vector/vector.hh>
#include <unisim/util/symbolic/symbolic.hh>
#include <bitset>
#include <set>
#include <memory>
#include <inttypes.h>

struct ScannerTypes
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

  typedef SmartValue<float>       F32;
  typedef SmartValue<double>      F64;

  struct NeonRegister
  {
    enum { BYTECOUNT = 16 };
    struct value_type { char _[BYTECOUNT]; };
    NeonRegister() = default;
    NeonRegister(unisim::util::symbolic::Expr const& _expr) : expr(_expr) {}
    unisim::util::symbolic::Expr expr;
    static unisim::util::symbolic::ValueType const* GetType()
    {
      static struct NRType : public unisim::util::symbolic::ValueType
      {
        NRType() : ValueType(unisim::util::symbolic::ValueType::NA) {}
        
        //return unisim::util::symbolic::ValueType::VOID;
        virtual unsigned GetBitSize() const override { return 8*BYTECOUNT; }
        virtual void GetName(std::ostream& sink) const override { sink << "NeonQ"; }
      } _;
      return &_;
    }
  };

  template <typename T>
  using  VectorTypeInfo = unisim::util::symbolic::vector::VUConfig::TypeInfo<T>;
  using  VectorByte = unisim::util::symbolic::vector::VUConfig::Byte;
  struct VectorByteShadow { char _[sizeof(U8)]; };
};

struct Scanner
  : ScannerTypes
  , unisim::component::cxx::processor::arm::regs64::CPU<Scanner, ScannerTypes>
{
    
  // typedef u64_t addr_t;

  struct DisasmState {};

  typedef unisim::component::cxx::processor::arm::isa::arm64::Operation<Scanner> Operation;
  typedef unisim::component::cxx::processor::arm::isa::arm64::Decoder<Scanner> Decoder;

  struct ISA : public Decoder
  {
    ISA();
    ~ISA();
    Operation* decode(uint64_t addr, uint32_t code, std::string& disasm);
  };

  typedef unisim::util::symbolic::Expr Expr;
  typedef unisim::util::symbolic::ExprNode ExprNode;
  typedef unisim::util::symbolic::ValueType ValueType;
  typedef unisim::util::symbolic::ConstNodeBase ConstNodeBase;
  typedef unisim::util::symbolic::EvalSpace EvalSpace;

  void noexec( Operation const& op )
  {
    throw unisim::util::sav::Untestable("Not implemented");
  }

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
  struct GReg { static ValueType const* GetType() { return unisim::util::symbolic::CValueType(uint64_t()); } static char const* name() { return "GReg"; } };

  typedef VRRead<VReg> VRegRead; typedef VRWrite<VReg> VRegWrite;
  /**/                           typedef VRWrite<GReg> GRegWrite;

  struct SP
    : public unisim::util::symbolic::WithValueType<SP>
  {
    typedef uint64_t value_type;
    char const* c_str() const { return "sp"; }
    int cmp( SP const& ) const { return 0; }
    ConstNodeBase const* eval(EvalSpace const& evs, ConstNodeBase const**) const
    {
      if (dynamic_cast<AddrEval const*>( &evs ))
        dont("sp-relative addressing");
      return 0;
    }
  };
    
  struct PC
    : public unisim::util::symbolic::WithValueType<PC>
  {
    typedef uint64_t value_type;
    char const* c_str() const { return "pc"; }
    int cmp( PC const& ) const { return 0; }
    ConstNodeBase const* eval(EvalSpace const& evs, ConstNodeBase const**) const
    {
      if (dynamic_cast<AddrEval const*>( &evs ))
        dont("pc-relative addressing");
      return 0;
    }
  };

  struct Flag
    : public unisim::util::identifier::Identifier<Flag>
    , public unisim::util::symbolic::WithValueType<Flag>
  {
    typedef bool value_type;
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

  struct Load : public ExprNode
  {
    Load( ValueType const* _tp, Expr const& _addr ) : addr(_addr), tp(_tp) {}
    virtual Load* Mutate() const override { return new Load( *this ); }
    virtual void Repr( std::ostream& sink ) const override { GetType()->GetName(sink << "Load"); sink << "(" << addr << ")"; }
    virtual unsigned SubCount() const override { return 1; }
    virtual Expr const& GetSub(unsigned idx) const override { if (idx != 0) return ExprNode::GetSub(idx); return addr; }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<Load const&>( rhs ) ); }
    int compare( Load const& rhs ) const { return tp < rhs.tp ? -1 : tp > rhs.tp ? +1 : 0; }
    virtual ValueType const* GetType() const { return tp; }

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
    int compare( Store const& rhs ) const { return tp < rhs.tp ? -1 : tp > rhs.tp ? +1 : 0; }
      
    Expr addr;
    Expr value;
    ValueType const* tp;
  };
    

  //  typedef unisim::component::cxx::vector::VUnion<VUConfig> VectorView;

  struct AddrEval : public EvalSpace {};
  
  struct RelocEval : public EvalSpace
  {
    RelocEval( uint64_t const* _regvalues, uint64_t _address ) : regvalues(_regvalues), address(_address) {}
    uint64_t const* regvalues;
    uint64_t address;
  };

  struct GRegRead : public VRRead<GReg>, public unisim::util::sav::Addressings::Source
  {
    GRegRead( unsigned reg, unsigned idx ) : VRRead<GReg>( reg, idx ) {}
    virtual ConstNodeBase const* Eval( EvalSpace const& evs, ConstNodeBase const** ) const override
    {
      if (dynamic_cast<AddrEval const*>( &evs ))
        return new unisim::util::symbolic::ConstNode<uint64_t>( uint64_t(reg) << 60 );
      if (auto l = dynamic_cast<RelocEval const*>( &evs ))
        return new unisim::util::symbolic::ConstNode<uint64_t>( l->regvalues[idx] );
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

  Scanner( Interface& iif );
  ~Scanner();

  static void dont(char const* reason) { throw unisim::util::sav::Untestable(reason); }

  void UndefinedInstruction( Operation const* op ) { UndefinedInstruction(); }
  void UndefinedInstruction() { dont("undefined"); }

  struct SysReg
  {
    void dont() const { Scanner::dont("system"); }
    void Write(int, int, int, int, int, Scanner&, U64) const { dont(); }
    U64 Read(int, int, int, int, int, Scanner&) const { dont(); return U64(0); }
    void DisasmWrite(int, int, int, int, int, int, std::ostream&) const { dont(); }
    void DisasmRead(int, int, int, int, int, int, std::ostream&) const { dont(); }
  };
  static SysReg const* GetSystemRegister(int, int, int, int, int) { dont("system"); return 0; }
  void CheckSystemAccess(int) { dont("system"); }

  enum AccessReport { report_simd_access = 1, report_gsr_access, report_gzr_access };
  void report( AccessReport acc, unsigned reg, bool is_write );
  
  U8  GetTVU8(unsigned reg0, unsigned elements, unsigned regs, U8 const& index, U8 const& oob_value);

  void SetNZCV(BOOL const& N, BOOL const& Z, BOOL const& C, BOOL const& V)
  {
    flags[3] = N.expr; flags[2] = Z.expr; flags[1] = C.expr; flags[0] = V.expr;
  }
  U8   GetNZCV() const
  {
    U8 res = U8(BOOL(flags[0]));
    for (int idx = 1; idx < 4; ++idx)
      res = res | (U8(BOOL(flags[idx])) << idx );
    return res;
  }
  BOOL GetCarry() const { return BOOL(flags[1]); }

  U64 GetPC() { return current_insn_addr; }
  U64 GetNPC() { return next_insn_addr; }

  void BranchTo(U64 addr, branch_type_t) { dont("branch"); }
  bool concretize( Expr cexp );
  
  template <typename T>
  bool Test( unisim::util::symbolic::SmartValue<T> const& cond )
  {
    if (not cond.expr.good())
      throw std::logic_error( "Not a valid condition" );

    return concretize( BOOL(cond).expr );
  }
  //bool     Test(bool cond) { return cond; }

  void SoftwareBreakpoint( uint32_t imm ) { dont("system"); }
  void CallSupervisor( uint32_t imm ) { dont("system"); }
  void CallHypervisor( uint32_t imm ) { dont("system"); }
  void ExceptionReturn() { dont("system"); }

  Expr MemRead(ValueType const* tp, Expr const& addr)
  {
    interface.memaccess( addr, false );
    return new Load( tp, addr );
  }
  template <typename T>
  T MemReadT( U64 const& addr ) { return MemRead(T::GetType(), addr.expr); }
  U64 MemRead64(U64 addr) { return MemReadT<U64>(addr); }
  U32 MemRead32(U64 addr) { return MemReadT<U32>(addr); }
  U16 MemRead16(U64 addr) { return MemReadT<U16>(addr); }
  U8  MemRead8 (U64 addr) { return MemReadT<U8> (addr); }

  void MemWrite(ValueType const* tp, Expr const& addr, Expr const& data)
  {
    interface.memaccess( addr, true );
    stores.insert( new Store( tp, addr, data ) );
  }
  template <typename T> void MemWriteT(U64 const& addr, T const& data) { MemWrite(T::GetType(), addr.expr, data.expr); }
  
  void     MemWrite64(U64 addr, U64 val) { MemWriteT(addr, val); }
  void     MemWrite32(U64 addr, U32 val) { MemWriteT(addr, val); }
  void     MemWrite16(U64 addr, U16 val) { MemWriteT(addr, val); }
  void     MemWrite8 (U64 addr, U8  val) { MemWriteT(addr, val); }

  void     SetExclusiveMonitors( U64 addr, unsigned size ) { dont("mp"); }
  bool     ExclusiveMonitorsPass( U64 addr, unsigned size ) { dont("mp"); return false; }
  void     ClearExclusiveLocal() { dont("mp"); }
  void     PrefetchMemory( int, U64 ) { dont("prefetch"); }

  bool     close( Scanner const& ref );

  void
  step( Operation const& op )
  {
    U64 insn_addr =  this->current_insn_addr = this->next_insn_addr;
      
    this->next_insn_addr += U64(4);
      
    op.execute( *this );
  }

  Interface&     interface;
  unisim::util::sav::ActionNode*    path;

  std::set<Expr> stores;

  BOOL           flags[Flag::end];
  U64            current_insn_addr, next_insn_addr;
};

template <unsigned posT> void FPProcessException( Scanner&, unisim::component::cxx::processor::arm::RegisterField<posT,1> const& ) {}
template <typename FLOAT> FLOAT FPNaN( Scanner&, FLOAT value ) { return value; }

template <class FTP> FTP FPMulAdd(Scanner& cpu, FTP const&, FTP const&, FTP const&) { cpu.dont("floating-point"); return FTP(); }
template <class FTP> FTP FPMulSub(Scanner& cpu, FTP const&, FTP const&, FTP const&) { cpu.dont("floating-point"); return FTP(); }

struct PM2
{
  PM2(int _S, uint32_t _poly) : S(_S), poly(_poly) {} int S; uint32_t poly;
  friend std::ostream& operator << (std::ostream& sink, PM2 const& pm2) { sink << "PM2[" << pm2.S << "," << pm2.poly << "]"; return sink; }
  friend int strcmp(PM2 const& a, PM2 const& b)
  {
    if (int delta = a.S - b.S) return delta;
    return (int64_t(a.poly) - int64_t(b.poly)) >> 32;
  }
};

namespace unisim {
namespace util {
namespace symbolic {
  
  // Poly32Mod2 on a bitstring does a polynomial Modulus over {0,1} operation
  template <typename OUT, unsigned S, typename TIN>
  OUT PolyMod2(SmartValue<TIN> const& value, uint32_t _poly) { return sav::make_weirdop<OUT>(PM2(S,_poly),value); }

  template <typename OP>
  OP NeonSHL(OP const& op, SmartValue<int8_t> const& sh) { return sav::make_weirdop<OP>("NeonSHL", op, sh); }
  
}
}
}

#endif // ARM64SAV_SCANNER_HH

