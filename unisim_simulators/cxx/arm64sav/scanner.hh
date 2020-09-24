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
#include <unisim/component/cxx/vector/vector.hh>
#include <unisim/util/sav/sav.hh>
#include <unisim/util/symbolic/vector/vector.hh>
#include <unisim/util/symbolic/symbolic.hh>
#include <bitset>
#include <set>
#include <memory>
#include <inttypes.h>

struct NeonRegister
{
  enum { BYTECOUNT = 16 };
  struct value_type { char _[BYTECOUNT]; };
  NeonRegister() = default;
  NeonRegister(unisim::util::symbolic::Expr const& _expr) : expr(_expr) {}
  unisim::util::symbolic::Expr expr;
  static unisim::util::symbolic::ScalarType::id_t GetType() { return unisim::util::symbolic::ScalarType::VOID; }
};

struct Scanner
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

  typedef SmartValue<float>       F32;
  typedef SmartValue<double>      F64;

  struct DisasmState {};

  enum branch_type_t { B_JMP = 0, B_CALL, B_RET };
    
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
  typedef unisim::util::symbolic::ScalarType ScalarType;
  typedef unisim::util::symbolic::ConstNodeBase ConstNodeBase;
  typedef unisim::util::symbolic::EvalSpace EvalSpace;

  void noexec( Operation const& op )
  {
    throw unisim::util::sav::Untestable("Not implemented");
  }

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

  struct VReg { static ScalarType::id_t const scalar_type = ScalarType::U64;  static char const* name() { return "VReg"; } };
  struct FReg { static ScalarType::id_t const scalar_type = ScalarType::F64;  static char const* name() { return "FReg"; } };
  struct GReg { static ScalarType::id_t const scalar_type = ScalarType::VOID; static char const* name() { return "GReg"; } };

  typedef VRRead<VReg> VRegRead; typedef VRWrite<VReg> VRegWrite;
  typedef VRRead<FReg> FRegRead; typedef VRWrite<FReg> FRegWrite;
  /**/                           typedef VRWrite<GReg> GRegWrite;

  struct SP {
    typedef uint64_t register_type;
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
  {
    typedef uint64_t register_type;
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

  struct Load : public ExprNode
  {
    Load( ScalarType::id_t _tp, Expr const& _addr ) : addr(_addr), tp(_tp) {}
    virtual Load* Mutate() const override { return new Load( *this ); }
    virtual void Repr( std::ostream& sink ) const override { sink << "Load" << ScalarType(tp).name << "(" << addr << ")"; }
    virtual unsigned SubCount() const override { return 1; }
    virtual Expr const& GetSub(unsigned idx) const override { if (idx != 0) return ExprNode::GetSub(idx); return addr; }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<Load const&>( rhs ) ); }
    int compare( Load const& rhs ) const { return int(tp) - int(rhs.tp); }
    virtual ScalarType::id_t GetType() const { return tp; }

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

  static unsigned const VREGCOUNT = 32;
  static unsigned const GREGCOUNT = 32;
    
  struct VUConfig : public unisim::util::symbolic::vector::VUConfig
  { static unsigned const BYTECOUNT = NeonRegister::BYTECOUNT; };

  typedef unisim::component::cxx::vector::VUnion<VUConfig> VectorView;
  struct VectorBrick { char _[sizeof(U8)]; };

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

  void gregtouch(unsigned id, bool write);
  U64 gregread(unsigned id) { gregtouch(id, false); return gpr[id]; }
  void gregwrite(unsigned id, U64 const& value) { gregtouch(id, true); gpr[id] = value; }
    
  U64 GetGSR(unsigned id) { return (id != 31) ? gregread(id) : gpr[31]; }
  U64 GetGZR(unsigned id) { return (id != 31) ? gregread(id) : U64(0); }
  template <typename T> void SetGSR(unsigned id, unisim::util::symbolic::SmartValue<T> const& val)
  { U64 v64(val); if (id != 31) gregwrite(id, v64); else gpr[31] = v64; }
  template <typename T> void SetGZR(unsigned id, unisim::util::symbolic::SmartValue<T> const& val)
  { U64 v64(val); if (id != 31) gregwrite(id, v64);  }

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

  U8  GetVU8 ( unsigned reg, unsigned sub ) { return vector_read<U8> (reg, sub); }
  U16 GetVU16( unsigned reg, unsigned sub ) { return vector_read<U16>(reg, sub); }
  U32 GetVU32( unsigned reg, unsigned sub ) { return vector_read<U32>(reg, sub); }
  U64 GetVU64( unsigned reg, unsigned sub ) { return vector_read<U64>(reg, sub); }
  S8  GetVS8 ( unsigned reg, unsigned sub ) { return vector_read<S8> (reg, sub); }
  S16 GetVS16( unsigned reg, unsigned sub ) { return vector_read<S16>(reg, sub); }
  S32 GetVS32( unsigned reg, unsigned sub ) { return vector_read<S32>(reg, sub); }
  S64 GetVS64( unsigned reg, unsigned sub ) { return vector_read<S64>(reg, sub); }

  template <typename T>
  void vector_write(unsigned reg, unsigned sub, T value )
  {
    (vregtouch(reg, true).GetStorage(&vector_data[reg][0], value, VUConfig::BYTECOUNT))[sub] = value;
  }

  void SetVU8 ( unsigned reg, unsigned sub, U8  value ) { vector_write( reg, sub, value ); }
  void SetVU16( unsigned reg, unsigned sub, U16 value ) { vector_write( reg, sub, value ); }
  void SetVU32( unsigned reg, unsigned sub, U32 value ) { vector_write( reg, sub, value ); }
  void SetVU64( unsigned reg, unsigned sub, U64 value ) { vector_write( reg, sub, value ); }
  void SetVS8 ( unsigned reg, unsigned sub, S8  value ) { vector_write( reg, sub, value ); }
  void SetVS16( unsigned reg, unsigned sub, S16 value ) { vector_write( reg, sub, value ); }
  void SetVS32( unsigned reg, unsigned sub, S32 value ) { vector_write( reg, sub, value ); }
  void SetVS64( unsigned reg, unsigned sub, S64 value ) { vector_write( reg, sub, value ); }

  template <typename T>
  void vector_write(unsigned reg, T value )
  {
    *(vregtouch(reg, true).GetStorage(&vector_data[reg][0], value, VUConfig::template TypeInfo<T>::bytecount)) = value;
  }

  void SetVU8 ( unsigned reg, U8 value )  { vector_write(reg, value); }
  void SetVU16( unsigned reg, U16 value ) { vector_write(reg, value); }
  void SetVU32( unsigned reg, U32 value ) { vector_write(reg, value); }
  void SetVU64( unsigned reg, U64 value ) { vector_write(reg, value); }
  void SetVS8 ( unsigned reg, S8 value )  { vector_write(reg, value); }
  void SetVS16( unsigned reg, S16 value ) { vector_write(reg, value); }
  void SetVS32( unsigned reg, S32 value ) { vector_write(reg, value); }
  void SetVS64( unsigned reg, S64 value ) { vector_write(reg, value); }

  void ClearHighV( unsigned reg, unsigned bytes )
  {
    vregtouch(reg, true).Truncate(bytes);
  }

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

  void CallSupervisor( uint32_t imm ) { dont("system"); }
  void CallHypervisor( uint32_t imm ) { dont("system"); }

  template <typename T> T MemReadT( U64 const& addr )
  {
    interface.memaccess( addr.expr, false );
    return T( Expr(new Load( T::GetType(), addr.expr )) );
  }
  U64 MemRead64(U64 addr) { return MemReadT<U64>(addr); }
  U32 MemRead32(U64 addr) { return MemReadT<U32>(addr); }
  U16 MemRead16(U64 addr) { return MemReadT<U16>(addr); }
  U8  MemRead8 (U64 addr) { return MemReadT<U8> (addr); }

  template <typename T> void MemWriteT(U64 const addr, T data)
  {
    interface.memaccess( addr.expr, true );
    stores.insert( new Store( T::GetType(), addr.expr, data.expr ) );
  }
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

  U64            gpr[GREGCOUNT];
  BOOL           flags[Flag::end];
  U64            current_insn_addr, next_insn_addr;

  VectorView     vector_views[VREGCOUNT];
  VectorBrick    vector_data[VREGCOUNT][VUConfig::BYTECOUNT];
};

#endif // ARM64SAV_SCANNER_HH

