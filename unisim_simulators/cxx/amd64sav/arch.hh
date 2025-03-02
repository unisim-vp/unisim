/*
 *  Copyright (c) 2019,
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

#ifndef AMD64SAV_ARCH_HH
#define AMD64SAV_ARCH_HH

#include <unisim/component/cxx/processor/intel/isa/intel.hh>
#include <unisim/component/cxx/processor/intel/vectorbank.hh>
#include <unisim/component/cxx/processor/intel/modrm.hh>
#include <unisim/component/cxx/processor/intel/types.hh>
#include <unisim/component/cxx/processor/intel/aes.hh>
#include <unisim/component/cxx/vector/vector.hh>
#include <unisim/util/sav/sav.hh>
#include <unisim/util/symbolic/vector/vector.hh>
#include <unisim/util/symbolic/symbolic.hh>
#include <unisim/util/arithmetic/integer.hh>
#include <bitset>
#include <set>
#include <memory>
#include <inttypes.h>

namespace review
{
  struct Arch;

  typedef unisim::component::cxx::processor::intel::Operation<Arch> Operation;

  struct AMD64
  {
    typedef review::Operation Operation;
    struct MemCode
    {
      uint8_t     bytes[15];
      unsigned    length;

      MemCode() : bytes(), length() {}
      MemCode( uint8_t fill ) : length(sizeof (bytes)) { std::fill( &bytes[0], &bytes[length], fill ); }
      MemCode( MemCode const& ) = default;
      MemCode( uint8_t const* _bytes, unsigned _length ) : length(_length) { std::copy( &_bytes[0], &_bytes[_length], &bytes[0] ); }
      MemCode( char const* _bytes, unsigned _length ) : length(_length) { std::copy( &_bytes[0], &_bytes[_length], &bytes[0] ); }

      uint8_t const* text() const { return &bytes[0]; }

      bool get( std::istream& source );

      uint8_t getbyte( unsigned idx, uint8_t _default ) const { return idx < length ? bytes[idx] : _default; }

      bool maximize( MemCode const& lo, MemCode const& hi )
      {
        MemCode delta(0);

        int carry = 0, dcarry = 0;
        for (unsigned idx = sizeof (bytes); idx-- > 0;)
          {
            carry = carry + int(hi.getbyte(idx, 0x00)) - int(lo.getbyte(idx,0xff));
            uint8_t byte = carry;
            delta.bytes[idx] = byte;
            carry >>= 8;
            dcarry  = dcarry + int(bytes[idx]) - int(byte);
            dcarry >>= 8;
          }
        if (dcarry == 0)
          return false;

        *this = delta;
        return true;
      }

      int compare( MemCode const& rhs ) const
      {
        for (unsigned idx = 0, end = std::min(length,rhs.length); idx < end; ++idx)
          if (int delta = int(bytes[idx]) - int(rhs.bytes[idx]))
            return delta;
        if (length != rhs.length) throw 0;
        return 0;
      }

      bool operator < ( MemCode const& rhs ) const { return compare( rhs ) < 0; }

      friend std::ostream& operator << ( std::ostream& sink, MemCode const& mc )
      {
        for (unsigned idx = 0; idx < mc.length; ++idx)
          {
            uint8_t byte = mc.bytes[idx];
            for (unsigned nibble = 2; nibble-- > 0;)
              sink << ("0123456789abcdef"[(byte >> nibble*4)&0xf]);
            sink << ' ';
          }
        return sink;
      }
    };

    Operation* decode( uint64_t addr, MemCode& ct, std::string& disasm );

    static char const* Name() { return "amd64"; }

    enum {VREGCOUNT = 16, GREGCOUNT = 16, FREGCOUNT=8};
  };

  using unisim::util::symbolic::SmartValue;

  template <typename T> using VectorTypeInfo = unisim::component::cxx::vector::VectorTypeInfo<T,0>;
  template <typename A, unsigned S> using TypeFor = typename unisim::component::cxx::processor::intel::TypeFor<A,S>;

  struct Interface
  {
    typedef AMD64::MemCode MemCode;
    typedef unisim::util::symbolic::Expr Expr;
    typedef void (*testcode_t)(uint64_t*);
    struct Text { virtual void write(uint8_t const*,unsigned) = 0; virtual ~Text() {} };

    Interface( Operation const& op, MemCode const& code, std::string const& disasm );

    void memaccess( Expr const& addr, bool iswrite );
    uintptr_t workcells() const;
    void gencode(Text& text) const;
    bool usemem() const { return addrs.size(); }
    uint32_t flagsmask() const { return ~uflags; }

    MemCode memcode;
    std::string asmcode;
    unisim::util::sav::OperandMap<uint8_t,AMD64::GREGCOUNT> gregs; /* integer (x86) registers */
    unisim::util::sav::OperandMap<uint8_t,AMD64::FREGCOUNT> fregs; /* floating (x87) registers */
    unisim::util::sav::OperandMap<uint8_t,AMD64::VREGCOUNT> vregs; /* vector (sse/avx) registers */
    std::shared_ptr<unisim::util::sav::ActionNode> behavior;
    struct RelCmp { bool operator () (uint64_t l, uint64_t r) const { return int64_t(l-r) < 0; } };
    std::set<uint64_t,RelCmp> addrs;
    Expr base_addr;
    unisim::util::sav::Addressings addressings;
    uint32_t uflags;
  };

  struct VmmRegister
  {
    enum { BYTECOUNT = 32 };
    struct value_type { char _[BYTECOUNT]; };
    VmmRegister() = default;
    VmmRegister(unisim::util::symbolic::Expr const& _expr) : expr(_expr) {}
    unisim::util::symbolic::Expr expr;
    static unisim::util::symbolic::ValueType GetType() { return unisim::util::symbolic::ValueType(unisim::util::symbolic::ValueType::NA, 8*BYTECOUNT); }
  };

  struct ArchTypes
  {
    typedef SmartValue<uint8_t>     u8_t;
    typedef SmartValue<uint16_t>    u16_t;
    typedef SmartValue<uint32_t>    u32_t;
    typedef SmartValue<uint64_t>    u64_t;
    typedef SmartValue<unisim::util::arithmetic::Integer<4,false>> u128_t;
    typedef SmartValue<int8_t>      s8_t;
    typedef SmartValue<int16_t>     s16_t;
    typedef SmartValue<int32_t>     s32_t;
    typedef SmartValue<int64_t>     s64_t;
    typedef SmartValue<unisim::util::arithmetic::Integer<4,true>> s128_t;
    typedef SmartValue<bool>        bit_t;

    typedef u64_t addr_t;
    typedef u8_t rc_t;

    typedef SmartValue<float>       f32_t;
    typedef SmartValue<double>      f64_t;
    typedef SmartValue<long double> f80_t;

    typedef unisim::component::cxx::processor::intel::GObLH GObLH;
    typedef unisim::component::cxx::processor::intel::GOb GOb;
    typedef unisim::component::cxx::processor::intel::GOw GOw;
    typedef unisim::component::cxx::processor::intel::GOd GOd;
    typedef unisim::component::cxx::processor::intel::GOq GOq;

    typedef unisim::component::cxx::processor::intel::SSE SSE;
    typedef unisim::component::cxx::processor::intel::XMM XMM;
    typedef unisim::component::cxx::processor::intel::YMM YMM;

    typedef unisim::component::cxx::processor::intel::RMOp<Arch> RMOp;

    typedef GOq   GR;
    typedef u64_t gr_type;

    struct OpHeader
    {
      OpHeader( uint64_t _address ) : address( _address ) {} uint64_t address;
    };
  };

  struct Arch
    : ArchTypes
    , unisim::component::cxx::processor::intel::AES<ArchTypes>
  {
    typedef unisim::util::symbolic::Expr Expr;
    typedef unisim::util::symbolic::ExprNode ExprNode;
    typedef unisim::util::symbolic::ValueType ValueType;

    void noexec( Operation const& op )
    {
      throw unisim::util::sav::Untestable("Not implemented");
    }

    struct Update : public ExprNode
    {
      virtual ValueType GetType() const override { return unisim::util::symbolic::NoValueType(); }
    };

    Arch( Interface& iif );
    ~Arch();

    Interface&                     interface;
    unisim::util::sav::ActionNode* path;

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
      virtual ValueType GetType() const { return RID::GetType(); }
      virtual unsigned SubCount() const override { return 0; }
      virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<RegRead const&>( rhs ) ); };
      int compare( RegRead const& rhs ) const { if (int delta = RegReadBase::compare( rhs )) return delta; return id.cmp( rhs.id ); }

      RID id;
    };

    template <typename RID>
    RegRead<RID>* newRegRead( RID _id ) { return new RegRead<RID>(_id); }

    struct VRReadBase : public unisim::util::sav::VirtualRegister, public ExprNode
    {
      VRReadBase( unsigned reg,  unsigned idx ) : VirtualRegister(reg, idx), ExprNode() {}
      virtual int cmp(ExprNode const& rhs) const override { return compare( dynamic_cast<VRReadBase const&>( rhs ) ); }
      virtual unsigned SubCount() const override { return 0; }
    };

    template <class T>
    struct VRRead : public VRReadBase
    {
      typedef VRRead<T> this_type;
      VRRead( unsigned reg,  unsigned idx ) : VRReadBase(reg, idx) {}
      virtual ExprNode* Mutate() const override { return new this_type(*this); }
      virtual void Repr( std::ostream& sink ) const override { sink << T::name() << "Read( " << idx << ", " << reg << " )"; }
      virtual ValueType GetType() const override { return T::scalar_type(); }
    };

    struct VRWriteBase : public unisim::util::sav::VirtualRegister, public Update
    {
      VRWriteBase( unsigned reg, unsigned idx, Expr const& _val ) : VirtualRegister(reg, idx), Update(), val(_val) {}
      virtual int cmp(ExprNode const& rhs) const override { return compare( dynamic_cast<VRWriteBase const&>( rhs ) ); }
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

    struct VReg { static ValueType scalar_type() { return unisim::util::symbolic::CValueType(uint64_t()); };  static char const* name() { return "VReg"; } };
    struct FReg { static ValueType scalar_type() { return unisim::util::symbolic::CValueType(double()); };  static char const* name() { return "FReg"; } };
    struct GReg { static ValueType scalar_type() { return unisim::util::symbolic::NoValueType(); }; static char const* name() { return "GReg"; } };

    typedef VRRead<VReg> VRegRead; typedef VRWrite<VReg> VRegWrite;
    typedef VRRead<FReg> FRegRead; typedef VRWrite<FReg> FRegWrite;
    /**/                           typedef VRWrite<GReg> GRegWrite;

    struct ExpectedAddress : public unisim::util::symbolic::ExprNode
    {
      ExpectedAddress() : unisim::util::symbolic::ExprNode() {}
      virtual ExpectedAddress* Mutate() const override { return new ExpectedAddress( *this ); }
      virtual int cmp(ExprNode const& rhs) const override { return 0; }
      virtual unsigned SubCount() const override { return 0; }
      virtual void Repr( std::ostream& sink ) const override { sink << "ExpectedAddress()"; }
      virtual unisim::util::symbolic::ValueType GetType() const override { return unisim::util::symbolic::CValueType(uint64_t()); }
    };

    struct GRegRead : public VRRead<GReg>, public unisim::util::sav::Addressings::Source
    {
      GRegRead( unsigned reg, unsigned idx ) : VRRead<GReg>( reg, idx ) {}
      typedef unisim::util::symbolic::ConstNodeBase ConstNodeBase;
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

    u64_t                       tscread() { throw unisim::util::sav::Untestable("hardware"); return u64_t( 0 ); }

    struct FLAG
      : public unisim::util::identifier::Identifier<FLAG>
      , public unisim::util::symbolic::WithValueType<FLAG>
    {
      typedef bool value_type;
      enum Code { CF = 0, PF, AF, ZF, SF, DF, OF, C0, C1, C2, C3, end } code;

      char const* c_str() const
      {
        switch (code)
          {
          case CF: return "cf";
          case PF: return "pf";
          case AF: return "af";
          case ZF: return "zf";
          case SF: return "sf";
          case DF: return "df";
          case OF: return "of";
          case C0: return "c0";
          case C1: return "c1";
          case C2: return "c2";
          case C3: return "c3";
          case end: break;
          }
        return "NA";
      }

      FLAG() : code(end) {}
      FLAG( Code _code ) : code(_code) {}
      FLAG( char const* _code ) : code(end) { init(_code); }
    };

    Expr                        flagvalues[FLAG::end];

    bit_t                       flagread( FLAG flag ) { return bit_t(flagvalues[flag.idx()]); }
    void                        flagwrite( FLAG flag, bit_t fval ) { flagvalues[flag.idx()] = fval.expr; }
    void                        flagsetdef( FLAG::Code flag, Expr def );
    void                        flagwrite( FLAG flag, bit_t fval, bit_t def ) { flagvalues[flag.idx()] = fval.expr; flagsetdef(flag.code, def.expr); }

    u16_t                       segregread( unsigned idx ) { throw unisim::util::sav::Untestable("segment register"); return u16_t(); }
    void                        segregwrite( unsigned idx, u16_t value ) { throw unisim::util::sav::Untestable("segment register"); }

    typedef std::map<unsigned,unsigned> RegMap;

    static unsigned const GREGCOUNT = AMD64::GREGCOUNT;
    static unsigned const GREGSIZE = 8;

    void                        eregtouch( unsigned reg, bool w );
    bool                        eregdiff( unsigned reg );
    Expr                        eregread( unsigned reg, unsigned size, unsigned pos );
    void                        eregwrite( unsigned reg, unsigned size, unsigned pos, Expr const& xpr );

    Expr                        regvalues[GREGCOUNT][GREGSIZE];

    template <class GOP>
    typename TypeFor<Arch,GOP::SIZE>::u regread( GOP const&, unsigned idx )
    {
      return typename TypeFor<Arch,GOP::SIZE>::u( gr_type( eregread(idx, GOP::SIZE / 8, 0) ) );
    }

    u8_t regread( GObLH const&, unsigned idx ) { return u8_t( gr_type( eregread(idx%4, 1, (idx >> 2) & 1) ) ); }

    template <class GOP>
    void regwrite( GOP const&, unsigned idx, typename TypeFor<Arch,GOP::SIZE>::u const& val )
    {
      eregwrite( idx, GOP::SIZE / 8, 0, gr_type(val).expr );
    }

    void regwrite( GObLH const&, unsigned idx, u8_t val ) { eregwrite( idx%4, 1, (idx>>2) & 1, gr_type(val).expr ); }
    void regwrite( GOd const&, unsigned idx, u32_t val ) { eregwrite( idx, GREGSIZE, 0, gr_type(val).expr ); }

    enum ipproc_t { ipjmp, ipcall, ipret };
    Expr                        next_insn_addr;
    ipproc_t                    next_insn_mode;

    addr_t                      getnip() { return addr_t(next_insn_addr); }
    void                        setnip( addr_t nip, ipproc_t ipproc = ipjmp ) { throw unisim::util::sav::Untestable("has jump"); }


    template <class T>
    struct SPRRead : public unisim::util::symbolic::ExprNode
    {
      typedef SPRRead<T> this_type;
      virtual this_type* Mutate() const override { return new this_type( *this ); }
      virtual unsigned SubCount() const override { return 0; }
      virtual int cmp( unisim::util::symbolic::ExprNode const& rhs ) const override { return 0; }
      virtual void Repr( std::ostream& sink ) const override { sink << T::name() << "Read()"; }
      virtual ValueType GetType() const override { return T::scalar_type(); }
    };

    struct RIP { static ValueType scalar_type() { return unisim::util::symbolic::CValueType(uint64_t()); }; static char const* name() { return "RIP"; } };

    struct RIPRead : public SPRRead<RIP>
    {
    };

    struct RIPWrite : public RegWriteBase
    {
      RIPWrite( Expr const& _value, ipproc_t _hint ) : RegWriteBase( _value ), hint(_hint) {}
      virtual RIPWrite* Mutate() const override { return new RIPWrite( *this ); }
      virtual char const* GetRegName() const override { return "rip"; }
      virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<RIPWrite const&>( rhs ) ); }
      int compare( RIPWrite const& rhs ) const { if (int delta = RegWriteBase::compare( rhs )) return delta; return int(hint) - int(rhs.hint); }
      ipproc_t hint;
    };

    void                        fnanchk( f64_t value ) {};

    rc_t                        fcwreadRC() const { return rc_t(0); }
    u16_t                       fcwread() const { throw unisim::util::sav::Untestable("FCW access"); return u16_t(); }
    void                        fcwwrite( u16_t value ) { throw unisim::util::sav::Untestable("FCW access"); }
    void                        finit() { throw unisim::util::sav::Untestable("FCW access"); }

    void                        fxam();

    struct LoadBase : public ExprNode
    {
      LoadBase( unsigned _bytes, unsigned _segment, Expr const& _addr )
        : addr(_addr), bytes(_bytes), segment(_segment)
      {
        if (segment >= 4) throw unisim::util::sav::Untestable("FS|GS relative addressing");
      }
      virtual void Repr( std::ostream& sink ) const override { sink << "Load" << (8*bytes) << "(" << (int)segment << "," << addr << ")"; }
      virtual unsigned SubCount() const override { return 1; }
      virtual Expr const& GetSub(unsigned idx) const override { if (idx != 0) return ExprNode::GetSub(idx); return addr; }
      virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<LoadBase const&>( rhs ) ); }
      int compare( LoadBase const& rhs ) const
      {
        if (int delta = int(bytes) - int(rhs.bytes)) return delta;
        return int(segment) - int(rhs.segment);
      }

      Expr addr;
      uint8_t bytes, segment;
    };

    template <typename dstT>
    struct Load : public LoadBase
    {
      typedef Load<dstT> this_type;
      Load( unsigned bytes, unsigned segment, Expr const& addr ) : LoadBase(bytes, segment, addr) {}
      virtual this_type* Mutate() const override { return new this_type( *this ); }
      virtual ValueType GetType() const { return unisim::util::symbolic::CValueType(dstT()); }
    };

    // struct FRegWrite : public Update
    // {
    //   FRegWrite( unsigned _idx, Expr const& _value )
    //     : value(_value), idx(_idx)
    //   {}
    //   virtual FRegWrite* Mutate() const override { return new FRegWrite(*this); }
    //   virtual void Repr( std::ostream& sink ) const override { sink << "FRegWrite( " << idx << ", " << value << " )"; }
    //   virtual unsigned SubCount() const override { return 1; }
    //   virtual Expr const& GetSub(unsigned idx) const override { if (idx == 0) return value; return ExprNode::GetSub(idx); }
    //   virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<FRegWrite const&>( rhs ) ); }
    //   int compare( FRegWrite const& rhs ) const { return int(idx) - int(rhs.idx); }
    //   Expr value;
    //   unsigned idx;
    // };

    struct Store : public Update
    {
      Store( unsigned _bytes, unsigned _segment, Expr const& _addr, Expr const& _value )
        : addr( _addr ), value( _value ), bytes(_bytes), segment(_segment)
      {
        if (segment >= 4) throw unisim::util::sav::Untestable("FS|GS relative addressing");
      }
      virtual Store* Mutate() const override { return new Store( *this ); }
      virtual void Repr( std::ostream& sink ) const override { sink << "Store" << (8*bytes) << "( " << (int)segment << ", " << addr << ", " << value <<  " )"; }
      virtual unsigned SubCount() const override { return 2; }
      virtual Expr const& GetSub(unsigned idx) const override { switch (idx) { case 0: return addr; case 1: return value; } return ExprNode::GetSub(idx); }
      virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<Store const&>( rhs ) ); }
      int compare( Store const& rhs ) const
      {
        if (int delta = int(bytes) - int(rhs.bytes)) return delta;
        return int(segment) - int(rhs.segment);
      }

      Expr addr;
      Expr value;
      uint8_t bytes, segment;
    };

    std::set<Expr> stores;

    template <typename dstT> Expr PerformLoad( dstT const&, unsigned bytes, unsigned segment, addr_t const& addr )
    {
      interface.memaccess( addr.expr, false );
      return new Load<dstT>( bytes, segment, addr.expr );
    }
    void PerformStore( unsigned bytes, unsigned segment, addr_t const& addr, Expr const& value )
    {
      interface.memaccess( addr.expr, true );
      stores.insert( Expr( new Store( bytes, segment, addr.expr, value ) ) );
    }

    template<unsigned OPSIZE>
    typename TypeFor<Arch,OPSIZE>::u
    memread( unsigned seg, addr_t const& addr )
    {
      typedef typename TypeFor<Arch,OPSIZE>::u u_type;
      typedef typename u_type::value_type uval_type;
      return u_type( PerformLoad( uval_type(), OPSIZE/8, seg, addr.expr ) );
    }

    template <unsigned OPSIZE>
    void
    memwrite( unsigned seg, addr_t const& addr, typename TypeFor<Arch,OPSIZE>::u val )
    {
      PerformStore( OPSIZE/8, seg, addr, val.expr );
    }

    struct FTop : public unisim::util::symbolic::ExprNode
    {
      virtual FTop* Mutate() const override { return new FTop(*this); }
      virtual unsigned SubCount() const { return 0; }
      virtual int cmp(ExprNode const&) const override { return 0; }
      virtual ValueType GetType() const { return unisim::util::symbolic::CValueType(uint8_t()); }
      virtual void Repr( std::ostream& sink ) const;
    };

    struct FTopWrite : public RegWriteBase
    {
      FTopWrite( u8_t const& ftop ) : RegWriteBase( ftop.expr ) {}
      virtual FTopWrite* Mutate() const override { return new FTopWrite(*this); }
      virtual char const* GetRegName() const { return "ftop"; }
    };

    u16_t                       ftopread() { throw unisim::util::sav::Untestable("FCW access"); return u16_t(); }
    unsigned                    ftop;

    Expr&                       fpaccess(unsigned r, bool w);
    bool                        fpdiff(unsigned r);

    Expr                        fpregs[8];
    void                        fpush( f64_t value ) { ftop = (ftop+8-1) % 8; fpaccess(ftop,true) = value.expr; }
    void                        fwrite( unsigned idx, f64_t value ) { fpaccess((ftop + idx) % 8,true) = value.expr; }
    f64_t                       fpop() { f64_t res( fpaccess(ftop,false) ); ftop = (ftop+1) % 8; return res; }
    f64_t                       fread( unsigned idx ) { return f64_t(fpaccess((ftop + idx) % 8,false)); }

    void                        fmemwrite32( unsigned seg, addr_t const& addr, f32_t val ) { PerformStore(  4, seg, addr, val.expr ); }
    void                        fmemwrite64( unsigned seg, addr_t const& addr, f64_t val ) { PerformStore(  8, seg, addr, val.expr ); }
    void                        fmemwrite80( unsigned seg, addr_t const& addr, f80_t val ) { PerformStore( 10, seg, addr, val.expr ); }

    f32_t                       fmemread32( unsigned seg, addr_t const& addr ) { return f32_t( PerformLoad( (float)0,        4, seg, addr ) ); }
    f64_t                       fmemread64( unsigned seg, addr_t const& addr ) { return f64_t( PerformLoad( (double)0,       8, seg, addr ) ); }
    f80_t                       fmemread80( unsigned seg, addr_t const& addr ) { return f80_t( PerformLoad( (long double)0, 10, seg, addr ) ); }

    template <unsigned OPSIZE>
    typename TypeFor<Arch,OPSIZE>::u
    pop()
    {
      // TODO: handle stack address size
      gr_type sptr = regread( GR(), 4 );
      regwrite( GR(), 4, sptr + gr_type( OPSIZE/8 ) );
      return memread<OPSIZE>( unisim::component::cxx::processor::intel::SS, addr_t(sptr) );
    }

    template <unsigned OPSIZE>
    void
    push( typename TypeFor<Arch,OPSIZE>::u const& value )
    {
      // TODO: handle stack address size
      gr_type sptr = regread( GR(), 4 ) - gr_type( OPSIZE/8 );
      memwrite<OPSIZE>( unisim::component::cxx::processor::intel::SS, addr_t(sptr), value );
      regwrite( GR(), 4, sptr );
    }

    void shrink_stack( addr_t const& offset ) { regwrite( GR(), 4, regread( GR(), 4 ) + offset ); }
    void grow_stack( addr_t const& offset ) { regwrite( GR(), 4, regread( GR(), 4 ) - offset ); }

    template <class GOP>
    typename TypeFor<Arch,GOP::SIZE>::u
    rmread( GOP const& g, RMOp const& rmop )
    {
      if (not rmop.ismem())
        return regread( g, rmop.ereg() );

      return memread<GOP::SIZE>( rmop->segment, rmop->effective_address( *this ) );
    }

   template <class GOP>
    void
    rmwrite( GOP const& g, RMOp const& rmop, typename TypeFor<Arch,GOP::SIZE>::u const& value )
    {
      if (not rmop.ismem())
        return regwrite( g, rmop.ereg(), value );

      return memwrite<GOP::SIZE>( rmop->segment, rmop->effective_address( *this ), value );
    }

    template <unsigned OPSIZE>
    typename TypeFor<Arch,OPSIZE>::f
    fpmemread( unsigned seg, addr_t const& addr )
    {
      typedef typename TypeFor<Arch,OPSIZE>::f f_type;
      typedef typename f_type::value_type f_ctype;
      return f_type( PerformLoad( f_ctype(), OPSIZE/8, seg, addr ) );
    }

    template <unsigned OPSIZE>
    typename TypeFor<Arch,OPSIZE>::f
    frmread( RMOp const& rmop )
    {
      typedef typename TypeFor<Arch,OPSIZE>::f f_type;
      if (not rmop.ismem()) return f_type( fread( rmop.ereg() ) );
      return this->fpmemread<OPSIZE>( rmop->segment, rmop->effective_address( *this ) );
    }

    template <unsigned OPSIZE>
    void
    fpmemwrite( unsigned seg, addr_t const& addr, typename TypeFor<Arch,OPSIZE>::f const& value )
    {
      PerformStore( OPSIZE/8, seg, addr, value.expr );
    }

    template <unsigned OPSIZE>
    void
    frmwrite( RMOp const& rmop, typename TypeFor<Arch,OPSIZE>::f const& value )
    {
      if (not rmop.ismem()) return fwrite( rmop.ereg(), f64_t( value ) );
      fpmemwrite<OPSIZE>( rmop->segment, rmop->effective_address( *this ), value );
    }

    // struct MXCSRRead : public RegReadBase
    // {
    //   virtual MXCSRRead* Mutate() const override { return new MXCSRRead( *this ); }
    //   virtual char const* GetRegName() const override { return "mxcsr"; };
    //   virtual ValueType GetType() const override { return ValueType::U16; }
    //   virtual unsigned SubCount() const override { return 0; }
    // };

    // struct MXCSRWrite : public RegWriteBase
    // {
    //   MXCSRWrite( Expr const& _value ) : RegWriteBase( _value ) {}
    //   virtual MXCSRWrite* Mutate() const override { return new MXCSRWrite( *this ); }
    //   virtual char const* GetRegName() const override { return "mxcsr"; }
    // };

    u32_t mxcsread() { throw unisim::util::sav::Untestable("mxcsr access"); return u32_t(); };
    void mxcswrite( u32_t const& value ) { throw unisim::util::sav::Untestable("mxcsr access"); }

    struct VUConfig : public unisim::util::symbolic::vector::VUConfig
    {
      static unsigned const BYTECOUNT = VmmRegister::BYTECOUNT;
    };
    static unsigned const VREGCOUNT = AMD64::VREGCOUNT;

    // struct VRegWrite : public Arch::Update
    // {
    //   VRegWrite( unsigned _idx, Expr const& _value ) : value(_value), idx(_idx) {}
    //   virtual VRegWrite* Mutate() const override { return new VRegWrite( *this ); }
    //   virtual void Repr( std::ostream& sink ) const override { sink << "VRegWrite(" << idx << ") := " << value; }
    //   virtual unsigned SubCount() const { return 1; }
    //   virtual Expr const& GetSub(unsigned idx) const { if (idx != 0) return ExprNode::GetSub(idx); return value; }
    //   virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<VRegWrite const&>( rhs ) ); }
    //   int compare( VRegWrite const& rhs ) const { return int(idx) - int(rhs.idx); }
    //   Expr value;
    //   unsigned idx;
    // };

    struct VmmBrick { char _[sizeof(u8_t)]; };
    typedef unisim::component::cxx::vector::VUnion<VUConfig> VUnion;
    VUnion umms[VREGCOUNT];
    VmmBrick vmm_storage[VREGCOUNT][VUConfig::BYTECOUNT];

    template <class VR> static unsigned vmm_wsize( VR const& vr ) { return VR::size() / 8; }
    static unsigned vmm_wsize( unisim::component::cxx::processor::intel::SSE const& ) { return VUConfig::BYTECOUNT; }

    template <class VR, class ELEM>
    struct VmmIndirectRead : public ExprNode
    {
      typedef unisim::util::symbolic::TypeInfo<typename ELEM::value_type> traits;
      enum { elemcount = VR::SIZE / traits::BITSIZE };
      VmmIndirectRead( ELEM const* elems, u8_t const& _sub) : sub(_sub.expr) { for (unsigned idx = 0, end = elemcount; idx < end; ++idx) sources[idx] = elems[idx].expr; }
      typedef VmmIndirectRead<VR,ELEM> this_type;
      virtual this_type* Mutate() const override { return new this_type( *this ); }
      virtual void Repr( std::ostream& sink ) const override
      {
        sink << "VmmIndirectRead<" << VR::SIZE << ",";
        GetType().Repr(sink);
        sink << ">(";
        for (unsigned idx = 0, end = elemcount; idx < end; ++idx)
          sink << sources[idx] << ", ";
        sink << sub << ")";
      }
      virtual unsigned SubCount() const { return elemcount+1; }
      virtual Expr const& GetSub(unsigned idx) const { if (idx < elemcount) return sources[idx]; if (idx == elemcount) return sub; return ExprNode::GetSub(idx); }
      virtual ValueType GetType() const override { return ELEM::GetType(); }
      virtual int cmp( ExprNode const& brhs ) const override { return compare( dynamic_cast<this_type const&>(brhs) ); }
      int compare( this_type const& rhs ) const { return 0; }
      Expr sources[elemcount];
      Expr sub;
    };

    void vmm_touch( unsigned reg, bool write );
    bool vmm_diff( unsigned reg );
    bool _vmm_diff( unsigned reg );

    template <class VR, class ELEM>
    ELEM vmm_read( VR const& vr, unsigned reg, u8_t const& sub, ELEM const& e )
    {
      vmm_touch( reg, false );
      ELEM const* elems = umms[reg].GetConstStorage( &vmm_storage[reg][0], e, vr.size() / 8 );
      return ELEM(Expr(new VmmIndirectRead<VR, ELEM>( elems, sub.expr )));
    }

    template <class VR, class ELEM>
    ELEM vmm_read( VR const& vr, unsigned reg, unsigned sub, ELEM const& e )
    {
      vmm_touch( reg, false );
      ELEM const* elems = umms[reg].GetConstStorage( &vmm_storage[reg][0], e, vr.size() / 8 );
      return elems[sub];
    }

    template <class VR, class ELEM>
    void vmm_write( VR const& vr, unsigned reg, unsigned sub, ELEM const& e )
    {
      vmm_touch( reg, true );
      ELEM* elems = umms[reg].GetStorage( &vmm_storage[reg][0], e, vmm_wsize( vr ) );
      elems[sub] = e;
    }

    template <class VR, class ELEM>
    ELEM
    vmm_read( VR const& vr, RMOp const& rmop, unsigned sub, ELEM const& e )
    {
      if (not rmop.ismem()) return vmm_read( vr, rmop.ereg(), sub, e );
      return vmm_memread( rmop->segment, rmop->effective_address( *this ), sub, e );
    }

    template <class VR, class ELEM>
    void
    vmm_write( VR const& vr, RMOp const& rmop, unsigned sub, ELEM const& e )
    {
      if (not rmop.ismem()) return vmm_write( vr, rmop.ereg(), sub, e );
      return vmm_memwrite( rmop->segment, rmop->effective_address( *this ), sub, e );
    }

    template <class ELEM> ELEM vmm_memread( unsigned seg, addr_t addr, unsigned sub, ELEM const& e )
    {
      any_memread( seg, addr, e); // Make sure that we access vector base
      return any_memread( seg, addr + addr_t(sub*VUConfig::TypeInfo<ELEM>::bytecount), e );
    }

    template <class ELEM> void vmm_memwrite( unsigned seg, addr_t addr, unsigned sub, ELEM const& e )
    {
      any_memwrite( seg, addr, e); // Make sure that we access vector base
      any_memwrite( seg, addr + addr_t(sub*VUConfig::TypeInfo<ELEM>::bytecount), e);
    }

    // Integer case
    template <class TYPE> TYPE any_memread( unsigned seg, addr_t addr, TYPE const& e )
    {
      typedef unisim::component::cxx::processor::intel::atpinfo<Arch,TYPE> atpinfo;
      return TYPE(memread<atpinfo::bitsize>(seg,addr));
    }

    template <class TYPE> void any_memwrite( unsigned seg, addr_t addr, TYPE const& e )
    {
      typedef unisim::component::cxx::processor::intel::atpinfo<Arch,TYPE> atpinfo;
      memwrite<atpinfo::bitsize>(seg,addr,typename atpinfo::utype(e));
    }

    // FP case
    f32_t any_memread( unsigned seg, addr_t addr, f32_t const& e ) { return fmemread32( seg, addr ); }
    f64_t any_memread( unsigned seg, addr_t addr, f64_t const& e ) { return fmemread64( seg, addr ); }
    f80_t any_memread( unsigned seg, addr_t addr, f80_t const& e ) { return fmemread80( seg, addr ); }

    void any_memwrite( unsigned seg, addr_t addr, f32_t const& e ) { return fmemwrite32( seg, addr, e ); }
    void any_memwrite( unsigned seg, addr_t addr, f64_t const& e ) { return fmemwrite64( seg, addr, e ); }
    void any_memwrite( unsigned seg, addr_t addr, f80_t const& e ) { return fmemwrite80( seg, addr, e ); }

    void    unimplemented()               { throw unisim::util::sav::Untestable("unimplemented"); }
    void    interrupt( int op, int code ) { throw unisim::util::sav::Untestable("system"); }
    void    syscall()                     { throw unisim::util::sav::Untestable("system"); }
    void    cpuid()                       { throw unisim::util::sav::Untestable("hardware"); }
    void    xgetbv()                      { throw unisim::util::sav::Untestable("hardware"); }
    void    stop()                        { throw unisim::util::sav::Untestable("hardware"); }
    void    _DE()                         { throw unisim::util::sav::Untestable("system"); }

    void    xsave (unisim::component::cxx::processor::intel::XSaveMode, bool, u64_t, RMOp const&) { throw unisim::util::sav::Untestable("hardware"); }
    void    xrstor(unisim::component::cxx::processor::intel::XSaveMode, bool, u64_t, RMOp const&) { throw unisim::util::sav::Untestable("hardware"); }

    // bool Test( bit_t b ) { return false; }

    template <typename T>
    bool Test( unisim::util::symbolic::SmartValue<T> const& cond )
    {
      // if (not cond.expr.good()) throw std::logic_error( "Not a valid condition" );
      return concretize(bit_t(cond).expr);
    }
    bool concretize(Expr cond);

    void
    step( Operation const& op )
    {
      next_insn_addr = (getnip() + addr_t(op.length)).expr;
      op.execute( *this );
    }
    // uint8_t                            base_register;
    // bool                               aligned;
    // bool                               mem_writes;
    // uint8_t                            length;
    // bool                               retfalse;


    // struct Prologue
    // {
    //   struct Error {};

    //   typedef std::map<unsigned,uint32_t> Regs;
    //   Prologue( Regs const& _regs, uint32_t _offset, bool _sign, uint8_t _base )
    //     : regs( _regs ), offset( _offset ), sign( _sign ), base( _base )
    //   {}
    //   Regs regs; uint32_t offset; bool sign; uint8_t base;
    // };

    // Prologue GetPrologue() const;

    // void PCRelPrologue( Prologue const& pc ) const;

  };

  template <class ARCH, typename INT>
  void eval_div64( ARCH& arch, INT& hi, INT& lo, INT const& divisor )
  {
    if (arch.Test(divisor == INT(0))) arch._DE();

    INT nlo = unisim::util::sav::make_weirdop<INT>("div.lo",hi,lo,divisor);
    INT nhi = unisim::util::sav::make_weirdop<INT>("div.hi",hi,lo,divisor);
    lo = nlo;
    hi = nhi;
  }

  // inline void eval_div( Arch& arch, Arch::u64_t& hi, Arch::u64_t& lo, Arch::u64_t const& divisor )    { eval_div64( arch, hi, lo, divisor ); }
  // inline void eval_div( Arch& arch, Arch::s64_t& hi, Arch::s64_t& lo, Arch::s64_t const& divisor )    { eval_div64( arch, hi, lo, divisor ); }

  inline Arch::f64_t eval_fprem ( Arch& arch, Arch::f64_t const& dividend, Arch::f64_t const& modulus )
  { return unisim::util::sav::make_weirdop<Arch::f64_t>("fprem", dividend, modulus); }
  inline Arch::f64_t eval_fprem1( Arch& arch, Arch::f64_t const& dividend, Arch::f64_t const& modulus )
  { return unisim::util::sav::make_weirdop<Arch::f64_t>("fprem1", dividend, modulus); }
} // end of namespace review

#endif // AMD64SAV_ARCH_HH
