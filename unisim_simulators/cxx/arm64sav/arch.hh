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

#ifndef ARM64SAV_ARCH_HH
#define ARM64SAV_ARCH_HH

#include <unisim/component/cxx/processor/arm/isa_arm64.hh>
#include <unisim/component/cxx/vector/vector.hh>
#include <unisim/util/symbolic/symbolic.hh>
#include <bitset>
#include <set>
#include <memory>
#include <inttypes.h>

namespace review
{
  struct Arch;
  
  typedef unisim::component::cxx::processor::arm::isa::arm64::Operation<Arch> Operation;
  typedef unisim::component::cxx::processor::arm::isa::arm64::Decoder<Arch> Decoder;
  
  using unisim::util::symbolic::SmartValue;

  template <typename T> using VectorTypeInfo = unisim::component::cxx::vector::VectorTypeInfo<T,0>;
  
  struct Untestable
  {
    Untestable(std::string const& _reason) : reason(_reason) {}
    std::string reason;
  };

  struct ActionNode : public unisim::util::symbolic::Conditional<ActionNode>
  {
    typedef unisim::util::symbolic::Expr Expr;

    ActionNode() : Conditional<ActionNode>(), updates() {}

    friend std::ostream& operator << (std::ostream& sink, ActionNode const& an)
    {
      for (auto && update : an.updates)
        sink << update << ";\n";
      if (not an.cond.node)
        return sink;
      sink << "if (" << an.cond << ")\n{\n";
      if (auto nxt = an.nexts[0]) sink << *nxt;
      sink << "}\nelse\n{\n";
      if (auto nxt = an.nexts[1]) sink << *nxt;
      sink << "}\n";
      return sink;
    }

    void add_update( Expr expr ) { expr.ConstSimplify(); updates.insert( expr ); }
    
    void simplify();
    
    std::set<Expr>           updates;
  };

  template <typename T>
  struct Operand
  {
    Operand() : raw(0) {}

    template <typename F> T Get( F const& f ) const { return (raw >> f.bit) & f.mask(); }
    template <typename F> void Set( F const& f, T v ) { raw &= ~(f.mask() << f.bit); raw |= (v & f.mask()) << f.bit; }
    
    bool access( bool w )
    {
      bool src = Get( Src() ), dst = Get( Dst() );
      Set( Src(), src or not w );
      Set( Dst(), dst or w );
      return src or dst;
    }

    bool is_accessed() const { return Get( Dst() ) | Get( Src() ); }
    bool is_modified() const { return Get( Dst() ); }

    T get_index() const { return Get( Idx() ); }
    T set_index( T idx ) { Set( Idx(), idx ); return idx; }

  private:
    struct Src { unsigned const bit = 0; static T mask() { return T( 1); } };
    struct Dst { unsigned const bit = 1; static T mask() { return T( 1); } };
    struct Idx { unsigned const bit = 2; static T mask() { return T(-1); } };

    T raw;
  };

  template <typename T, unsigned COUNT>
  struct OperandMap
  {
    typedef Operand<T> Op;

    OperandMap() : omap(), allocated() {}

    T touch(unsigned idx, bool w)
    {
      if (idx >= COUNT) throw "ouch";
      Operand<T>& op = omap[idx];
      if (op.access(w))
        return op.get_index();
      return op.set_index( allocated++ );
    }

    bool modified(unsigned idx) const
    {
      if (idx >= COUNT) throw "ouch";
      return omap[idx].is_modified();
    }

    bool accessed(unsigned idx) const
    {
      if (idx >= COUNT) throw "ouch";
      return omap[idx].is_accessed();
    }

    T index(unsigned idx) const
    {
      if (idx >= COUNT) throw "ouch";
      return omap[idx].get_index();
    }

    unsigned count() const { return COUNT; }
    unsigned used() const { return allocated; }

  private:
    Op omap[COUNT];
    T allocated;
  };

  struct Interface
  {
    typedef unisim::util::symbolic::Expr Expr;
    
    Interface( Operation const& op, uint32_t code, std::string const& disasm );

    void memaccess( Expr const& addr, bool iswrite );
    uintptr_t workquads() const;
    
    uint32_t memcode;
    std::string asmcode;
    OperandMap<uint8_t,32> gregs; /* general purpose registers */
    OperandMap<uint8_t,32> vregs; /* vector registers */
    std::shared_ptr<ActionNode> behavior;
    struct RelCmp { bool operator () (uint64_t l, uint64_t r) const { return int64_t(l - r) < 0; } };
    std::set<uint64_t,RelCmp> addrs;
    Expr base_addr;
    std::map<unsigned,Expr> relocs;
    bool has_write, has_jump;
    struct Text { virtual void write(uint8_t const*,unsigned) = 0; virtual ~Text() {} };
    void gencode(Text& text) const;
    //  std::vector<uint8_t> text;
    typedef void (*testcode_t)(uint64_t*);
    //  testcode_t testcode() const { return testcode_t(&text[0]); }
  };
  
  template <typename T>
  struct SmartValueTraits
  {
    typedef typename unisim::util::symbolic::TypeInfo<typename T::value_type> traits;
    enum { bytecount = traits::BYTECOUNT };
    static unisim::util::symbolic::ScalarType::id_t GetType() { return traits::GetType(); }
  };

  struct VmmRegister
  {
    VmmRegister() = default;
    VmmRegister(unisim::util::symbolic::Expr const& _expr) : expr(_expr) {}
    unisim::util::symbolic::Expr expr;
  };
    
  template <>
  struct SmartValueTraits<VmmRegister>
  {
    enum { bytecount = 32 };
    static unisim::util::symbolic::ScalarType::id_t GetType() { return unisim::util::symbolic::ScalarType::VOID; }
  };

  struct Arch
  {
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
  
    // typedef SmartValue<float>       f32_t;
    // typedef SmartValue<double>      f64_t;
    // typedef SmartValue<long double> f80_t;

    struct DisasmState {};
    
    typedef unisim::util::symbolic::Expr Expr;
    typedef unisim::util::symbolic::ExprNode ExprNode;
    typedef unisim::util::symbolic::ScalarType ScalarType;
    
    // typedef GOq   GR;
    // typedef u64_t gr_type;
  
    struct OpHeader
    {
      OpHeader( uint64_t _address ) : address( _address ) {} uint64_t address;
    };

    void noexec( Operation const& op )
    {
      throw Untestable("Not implemented");
    }

    struct Update : public ExprNode
    {
      virtual ScalarType::id_t GetType() const override { return ScalarType::VOID; }
    };

    struct ForDisasm {};
    Arch( ForDisasm&& );
    Arch( Interface& iif );

    Interface&      interface;
    ActionNode*     path;
    
    bool close( Arch const& ref );
    
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

      RID id;
    };

    template <typename RID>
    RegRead<RID>* newRegRead( RID _id ) { return new RegRead<RID>(_id); }

    struct VirtualRegister
    {
      VirtualRegister( unsigned _reg,  unsigned _idx ) : reg(_reg), idx(_idx) {} unsigned reg, idx;
      int compare(VirtualRegister const& rhs) const
      {
        if (int delta = int(idx) - int(rhs.idx)) return delta;
        return int(reg) - int(rhs.reg);
      }
    };
    
    struct VRReadBase : public VirtualRegister, public ExprNode
    {
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

    struct VRWriteBase : public VirtualRegister, public Update
    {
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

    struct AddrEval : public unisim::util::symbolic::EvalSpace {};
    struct RelocEval : public unisim::util::symbolic::EvalSpace
    {
      RelocEval( uint64_t const* _regvalues, uint64_t _address ) : regvalues(_regvalues), address(_address) {}
      uint64_t const* regvalues;
      uint64_t address;
    };

    struct GRegRead : public VRRead<GReg>
    {
      GRegRead( unsigned reg, unsigned idx ) : VRRead<GReg>( reg, idx ) {}
      typedef unisim::util::symbolic::ConstNodeBase ConstNodeBase;
      virtual ConstNodeBase const* Eval( unisim::util::symbolic::EvalSpace const& evs, ConstNodeBase const** ) const override
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

    int hash();
    
    // template <typename T>
    // bool Test( unisim::util::symbolic::SmartValue<T> const& cond )
    // {
    //   // if (not cond.expr.good()) throw std::logic_error( "Not a valid condition" );
    //   return Test(bit_t(cond).expr);
    // }
    bool Test(Expr cond);
    
    void
    step( Operation const& op )
    {
      uint64_t insn_addr = op.GetAddr();
      current_insn_addr = U64(insn_addr);
      next_insn_addr = U64(insn_addr+4);
      op.execute( *this );
    }

    U64 current_insn_addr, next_insn_addr;
  };
} // end of namespace review

#endif // ARM64SAV_ARCH_HH
