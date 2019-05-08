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

#ifndef ARCH_HH
#define ARCH_HH

#include <unisim/component/cxx/processor/intel/isa/intel.hh>
#include <unisim/component/cxx/processor/intel/vectorbank.hh>
#include <unisim/component/cxx/processor/intel/modrm.hh>
#include <unisim/component/cxx/processor/intel/types.hh>
#include <unisim/util/symbolic/symbolic.hh>
#include <vector>
#include <set>
#include <memory>
#include <inttypes.h>

namespace ut
{
  struct Arch;
  
  typedef unisim::component::cxx::processor::intel::Operation<ut::Arch> Operation;

  using unisim::util::symbolic::SmartValue;

  template <typename T> using VectorTypeInfo = unisim::component::cxx::processor::intel::VectorTypeInfo<T>;
  template <typename A, unsigned S> using TypeFor = typename unisim::component::cxx::processor::intel::TypeFor<A,S>;
  
  struct Untestable
  {
    Untestable(std::string const& _reason) : reason(_reason) {}
    std::string reason;
  };

  struct ActionNode : public unisim::util::symbolic::Choice<ActionNode>
  {
    typedef unisim::util::symbolic::Expr Expr;

    ActionNode() : Choice<ActionNode>(), updates() {}

    intptr_t cmp( ActionNode const& rhs ) const
    {
      if (intptr_t delta = updates.size() - rhs.updates.size()) return delta;
      auto rci = rhs.updates.begin();
      for (auto && update : updates)
        {
          if (intptr_t delta = update.cmp( *rci )) return delta;
          ++rci;
        }
      return 0;
    }
    
    void simplify();
    
    std::set<Expr>           updates;
  };

  struct Interface
  {
    Interface( Operation const& op );

    int  cmp( Interface const& rhs ) const { return action->cmp( *rhs.action ); }
    bool operator < ( Interface const& b ) const { return cmp( b ) < 0; }
    std::shared_ptr<ActionNode> action;
  };

  struct Arch
  {
    typedef SmartValue<uint8_t>     u8_t;
    typedef SmartValue<uint16_t>    u16_t;
    typedef SmartValue<uint32_t>    u32_t;
    typedef SmartValue<uint64_t>    u64_t;
    typedef SmartValue<int8_t>      s8_t;
    typedef SmartValue<int16_t>     s16_t;
    typedef SmartValue<int32_t>     s32_t;
    typedef SmartValue<int64_t>     s64_t;
    typedef SmartValue<bool>        bit_t;

    typedef u64_t addr_t;
  
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
    
    typedef unisim::util::symbolic::Expr Expr;
    typedef unisim::util::symbolic::ExprNode ExprNode;
    typedef unisim::util::symbolic::ScalarType ScalarType;
    
    typedef GOq   GR;
    typedef u64_t gr_type;
  
    struct OpHeader
    {
      OpHeader( uint64_t _address ) : address( _address ) {} uint64_t address;
    };

    static Operation* fetch( uint64_t _address, uint8_t const* bytes );

    void noexec( Operation const& op )
    {
      throw Untestable("Not implemented");
    }

    struct Update : public ExprNode
    {
      virtual ScalarType::id_t GetType() const override { return ScalarType::VOID; }
    };

    Arch( ActionNode* actions );
    
    ActionNode*     path;
    
    bool close( Arch const& ref );
    
    struct OfArch { OfArch( Arch& _core ) : core( _core ) {} Arch& core; };
    
    static Arch* FindRoot( unisim::util::symbolic::Expr const& expr )
    {
      if (OfArch const* node = dynamic_cast<OfArch const*>( expr.node ))
        return &node->core;
      
      for (unsigned idx = 0, end = expr->SubCount(); idx < end; ++idx)
        if (Arch* found = FindRoot( expr->GetSub(idx)))
          return found;
      return 0;
    }
    
    // struct RIRegID : public unisim::util::symbolic::Identifier<RIRegID>
    // {
    //   typedef uint64_t register_type;
    //   enum Code { rax = 0, rcx = 1, rdx = 2,  rbx = 3,  rsp = 4,  rbp = 5,  rsi = 6,  rdi = 7,
    //               r8 = 8,  r9 = 9,  r10 = 10, r11 = 11, r12 = 12, r13 = 13, r14 = 14, r15 = 15, end } code;

    //   char const* c_str() const
    //   {
    //     switch (code)
    //       {
    //       case rax: return "rax";
    //       case rcx: return "rcx";
    //       case rdx: return "rdx";
    //       case rbx: return "rbx";
    //       case rsp: return "rsp";
    //       case rbp: return "rbp";
    //       case rsi: return "rsi";
    //       case rdi: return "rdi";
    //       case r8:  return "r8";
    //       case r9:  return "r9";
    //       case r10: return "r10";
    //       case r11: return "r11";
    //       case r12: return "r12";
    //       case r13: return "r13";
    //       case r14: return "r14";
    //       case r15: return "r15";
    //       case end: break;
    //       }
    //     return "NA";
    //   }
      
    //   RIRegID() : code(end) {}
    //   RIRegID( Code _code ) : code(_code) {}
    //   RIRegID( char const* _code ) : code(end) { init( _code ); }
    // };

    //    typedef UWT::FLAG FLAG;
    
    struct SRegID : public unisim::util::symbolic::Identifier<SRegID>
    {
      typedef uint16_t register_type;
      enum Code { es, cs, ss, ds, fs, gs, end } code;
      
      char const* c_str() const
      {
        switch (code)
          {
          case  es: return "es";
          case  cs: return "cs";
          case  ss: return "ss";
          case  ds: return "ds";
          case  fs: return "fs";
          case  gs: return "gs";
          case end: break;
          }
        return "NA";
      }
      
      SRegID() : code(end) {}
      SRegID( Code _code ) : code(_code) {}
      SRegID( char const* _code ) : code(end) { init( _code ); }
    };

    struct FRegID : public unisim::util::symbolic::Identifier<FRegID>
    {
      typedef double register_type;
      enum Code { st0=0, st1, st2, st3, st4, st5, st6, st7, end } code;
      char const* c_str() const { return &"st0\0st1\0st2\0st3\0st4\0st5\0st6\0st7"[(unsigned(code) % 8)*4]; }
      FRegID() : code(end) {}
      FRegID( Code _code ) : code(_code) {}
      FRegID( char const* _code ) : code(end) { init( _code ); }
    };
    
    struct RegReadBase : public unisim::util::symbolic::ExprNode
    {
      virtual char const* GetRegName() const = 0;
      virtual void Repr( std::ostream& sink ) const override { sink << "RegRead(" << GetRegName() << ")"; }
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
      virtual intptr_t cmp( ExprNode const& rhs ) const override { return id.cmp( dynamic_cast<RegRead const&>( rhs ).id ); }

      RID id;
    };
    
    template <typename RID>
    RegRead<RID>* newRegRead( RID _id ) { return new RegRead<RID>(_id); }

    struct RegWriteBase : public Update
    {
      RegWriteBase( Expr const& _value ) : value( _value ) {}
      virtual unsigned SubCount() const { return 1; }
      virtual Expr const& GetSub(unsigned idx) const { if (idx != 0) return ExprNode::GetSub(idx); return value; }
      virtual char const* GetRegName() const = 0;
      virtual void Repr( std::ostream& sink ) const override { sink << GetRegName() << " := " << value; }
      virtual intptr_t compare( RegWriteBase const& rhs ) const { return value.cmp( rhs.value ); }
      virtual intptr_t cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<RegWriteBase const&>( rhs ) ); }
      Expr value;
    };

    template <typename RID>
    struct RegWrite : public RegWriteBase
    {
      typedef RegWrite<RID> this_type;
      RegWrite( RID _id, Expr const& _value ) : RegWriteBase( _value ), id( _id ) {}
      virtual this_type* Mutate() const override { return new this_type( *this ); }
      virtual char const* GetRegName() const override { return id.c_str(); };
      virtual intptr_t compare( this_type const& rhs ) const { if (intptr_t delta = RegWriteBase::compare( rhs )) return delta; return id.cmp( rhs.id ); }
      virtual intptr_t cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<RegWrite const&>( rhs ) ); }
      
      RID id;
    };

    template <typename RID> RegWrite<RID>* newRegWrite( RID _id, Expr const& _value ) { return new RegWrite<RID>(_id, _value); }

    int hash();
    
    u64_t                       tscread() { throw 0; return u64_t( 0 ); }
    
    struct FLAG : public unisim::util::symbolic::Identifier<FLAG>
    {
      typedef bool register_type;
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

    Expr                        segment_registers[6];
    
    u16_t                       segregread( unsigned idx ) { return u16_t(segment_registers[idx]); }
    void                        segregwrite( unsigned idx, u16_t value ) { segment_registers[idx] = value.expr; }
    
    static unsigned const REGCOUNT = 16;
    static unsigned const REGSIZE = 8;

    void                        eregtouch( unsigned reg );
    bool                        eregdiff( unsigned reg );
    Expr                        eregread( unsigned reg, unsigned size, unsigned pos );
    void                        eregwrite( unsigned reg, unsigned size, unsigned pos, Expr const& xpr );

    Expr                        regvalues[REGCOUNT][REGSIZE];
    std::map<unsigned,unsigned> regmap;

    template <class GOP>
    typename TypeFor<Arch,GOP::SIZE>::u regread( GOP const&, unsigned idx )
    {
      return typename TypeFor<Arch,GOP::SIZE>::u( eregread( idx, GOP::SIZE / 8, 0 ) );
    }

    u8_t regread( GObLH const&, unsigned idx ) { return u8_t( eregread( idx%4, 1, (idx >> 2) & 1 ) ); }

    template <class GOP> void regwrite( GOP const&, unsigned idx, typename TypeFor<Arch,GOP::SIZE>::u const& val )
    { eregwrite( idx, 8, 0, u64_t(val).expr ); }

    void regwrite( GObLH const&, unsigned idx, u8_t val ) { eregwrite( idx%4, 1, (idx>>2) & 1, val.expr ); }
    void regwrite( GOb const&, unsigned idx, u8_t val )   { eregwrite( idx, 1, 0, val.expr ); }
    void regwrite( GOw const&, unsigned idx, u16_t val )  { eregwrite( idx, 2, 0, val.expr ); }

    enum ipproc_t { ipjmp = 0, ipcall, ipret };
    Expr                        next_insn_addr;
    ipproc_t                    next_insn_mode;

    addr_t                      getnip() { return addr_t(next_insn_addr); }
    void                        setnip( addr_t nip, ipproc_t ipproc = ipjmp );
    
    struct RIPRead : public RegReadBase
    {
      virtual RIPRead* Mutate() const override { return new RIPRead( *this ); }
      virtual char const* GetRegName() const override { return "rip"; };
      virtual ScalarType::id_t GetType() const override { return ScalarType::U64; }
      virtual unsigned SubCount() const override { return 0; }
      virtual intptr_t cmp( ExprNode const& brhs ) const override { return 0; }
    };

    struct RIPWrite : public RegWriteBase
    {
      RIPWrite( Expr const& _value, ipproc_t _hint ) : RegWriteBase( _value ), hint(_hint) {}
      virtual RIPWrite* Mutate() const override { return new RIPWrite( *this ); }
      virtual char const* GetRegName() const override { return "rip"; }
      intptr_t compare( RIPWrite const& rhs ) const { if (intptr_t delta = RegWriteBase::compare( rhs )) return delta; return int(hint) - int(rhs.hint); }
      virtual intptr_t cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<RIPWrite const&>( rhs ) ); }
      ipproc_t hint;
    };

    void                        fnanchk( f64_t value ) {};

    struct FCWRead : public RegReadBase
    {
      virtual FCWRead* Mutate() const override { return new FCWRead( *this ); }
      virtual char const* GetRegName() const override { return "fcw"; };
      virtual ScalarType::id_t GetType() const override { return ScalarType::U16; }
      virtual unsigned SubCount() const override { return 0; }
      virtual intptr_t cmp( ExprNode const& brhs ) const override { return 0; }
    };

    struct FCWWrite : public RegWriteBase
    {
      FCWWrite( Expr const& _value ) : RegWriteBase( _value ) {}
      virtual FCWWrite* Mutate() const override { return new FCWWrite( *this ); }
      virtual char const* GetRegName() const override { return "fcw"; }
    };

    Expr                        fcw;
    int                         fcwreadRC() const { return 0; }
    u16_t                       fcwread() const { return u16_t(fcw); }
    void                        fcwwrite( u16_t value ) { fcw = value.expr; }
    void                        finit()
    {
      // FPU initialization
      flagwrite( FLAG::C0, bit_t(0) );
      flagwrite( FLAG::C1, bit_t(0) );
      flagwrite( FLAG::C2, bit_t(0) );
      flagwrite( FLAG::C3, bit_t(0) );
      fcwwrite( u16_t(0x037f) );
      // TODO: Complete FPU initialization
      // ftop = 0
      // fpustatus = 0
      // fputag = 0xffff
      // fpuoptr = 0
      // fpuiptr = 0
      // fpuopcode = 0
    }

    void                        fxam() { throw 0; }

    struct LoadBase : public ExprNode
    {
      LoadBase( unsigned _bytes, unsigned _segment, Expr const& _addr ) : addr(_addr), bytes(_bytes), segment(_segment) {}
      virtual void Repr( std::ostream& sink ) const override { sink << "Load" << (8*bytes) << "(" << (int)segment << "," << addr << ")"; }
      virtual unsigned SubCount() const override { return 1; }
      virtual Expr const& GetSub(unsigned idx) const override { if (idx != 0) return ExprNode::GetSub(idx); return addr; }
      intptr_t cmp( ExprNode const& brhs ) const override
      {
        LoadBase const& rhs = dynamic_cast<LoadBase const&>( brhs );
        if (intptr_t delta = addr.cmp( rhs.addr )) return delta;
        if (intptr_t delta = int(bytes) - int(rhs.bytes)) return delta;
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
      virtual ScalarType::id_t GetType() const { return unisim::util::symbolic::TypeInfo<dstT>::GetType(); }
      intptr_t cmp( ExprNode const& rhs ) const override { return LoadBase::cmp( rhs ); }
    };
      
    struct FRegWrite : public Update
    {
      FRegWrite( Arch& core, Expr const& _value, Expr const& _freg )
        : value(_value), freg(_freg)
      {}
      virtual FRegWrite* Mutate() const override { return new FRegWrite(*this); }
      virtual void Repr( std::ostream& sink ) const override { sink << "FRegWrite( " << freg << ", " << value << " )"; }
      virtual unsigned SubCount() const override { return 2; }
      virtual Expr const& GetSub(unsigned idx) const override { switch (idx) { case 0: return value; case 1: return freg; } return ExprNode::GetSub(idx); }
      intptr_t compare( FRegWrite const& rhs ) const { if (intptr_t delta = value.cmp(rhs.value)) return delta; return freg.cmp(rhs.freg); }
      intptr_t cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<FRegWrite const&>( rhs ) ); }
      Expr value, freg;
    };

    struct Store : public Update
    {
      Store( unsigned _bytes, unsigned _segment, Expr const& _addr, Expr const& _value )
        : addr( _addr ), value( _value ), bytes(_bytes), segment(_segment)
      {}
      virtual Store* Mutate() const override { return new Store( *this ); }
      virtual void Repr( std::ostream& sink ) const override { sink << "Store" << (8*bytes) << "( " << (int)segment << ", " << addr << ", " << value <<  " )"; }
      virtual unsigned SubCount() const override { return 2; }
      virtual Expr const& GetSub(unsigned idx) const override { switch (idx) { case 0: return addr; case 1: return value; } return ExprNode::GetSub(idx); }
      
      intptr_t cmp( ExprNode const& brhs ) const override
      {
        Store const& rhs = dynamic_cast<Store const&>( brhs );
        if (intptr_t delta = addr.cmp( rhs.addr )) return delta;
        if (intptr_t delta = value.cmp( rhs.value )) return delta;
        if (intptr_t delta = int(bytes) - int(rhs.bytes)) return delta;
        return int(segment) - int(rhs.segment);
      }
      
      Expr addr;
      Expr value;
      uint8_t bytes, segment;
    };

    std::set<Expr> stores;
    
    template<unsigned OPSIZE>
    typename TypeFor<Arch,OPSIZE>::u
    memread( unsigned seg, addr_t const& addr )
    {
      typedef typename TypeFor<Arch,OPSIZE>::u u_type;
      typedef typename u_type::value_type uval_type;
      return u_type( Expr( new Load<uval_type>( OPSIZE/8, seg, addr.expr ) ) );
    }

    template <unsigned OPSIZE>
    void
    memwrite( unsigned seg, addr_t const& addr, typename TypeFor<Arch,OPSIZE>::u val )
    {
      stores.insert( new Store( OPSIZE/8, seg, addr.expr, val.expr ) );
    }
    
    struct FTop : public unisim::util::symbolic::ExprNode
    {
      virtual FTop* Mutate() const { return new FTop(*this); }
      virtual unsigned SubCount() const { return 0; }
      virtual intptr_t cmp(ExprNode const&) const { return 0; }
      virtual ScalarType::id_t GetType() const { return ScalarType::U8; }
      virtual void Repr( std::ostream& sink ) const;
    };

    struct FTopWrite : public RegWriteBase
    {
      FTopWrite( Expr const& ftop ) : RegWriteBase( ftop ) {}
      virtual FTopWrite* Mutate() const override { return new FTopWrite(*this); }
      virtual char const* GetRegName() const { return "ftop"; }
    };
    
    Expr                        ftop_source;
    unsigned                    ftop;
    static u8_t                 ftop_update( Expr const& ftopsrc, unsigned ftop ) { if (ftop == 0) return ftopsrc; return (u8_t(ftopsrc) + u8_t(ftop)) & u8_t(7); }
    u8_t                        ftopread() { return ftop_update( ftop_source, ftop ); }
    
    Expr                        fpuregs[8];
    void                        fpush( f64_t value ) { ftop = (ftop+8-1) % 8; fpuregs[ftop] = value.expr; }
    void                        fwrite( unsigned idx, f64_t value ) { fpuregs[(ftop + idx) % 8] = value.expr; }
    f64_t                       fpop() { f64_t res( fpuregs[ftop] ); ftop = (ftop+1) % 8; return res; }
    f64_t                       fread( unsigned idx ) { return f64_t(fpuregs[(ftop + idx) % 8]); }
    
    void                        fmemwrite32( unsigned seg, addr_t const& addr, f32_t val ) { stores.insert( new Store(  4, seg, addr.expr, val.expr ) ); }
    void                        fmemwrite64( unsigned seg, addr_t const& addr, f64_t val ) { stores.insert( new Store(  8, seg, addr.expr, val.expr ) ); }
    void                        fmemwrite80( unsigned seg, addr_t const& addr, f80_t val ) { stores.insert( new Store( 10, seg, addr.expr, val.expr ) ); }
    
    f32_t                       fmemread32( unsigned seg, addr_t const& addr ) { return f32_t( Expr( new Load<float>( 4, seg, addr.expr ) ) ); }
    f64_t                       fmemread64( unsigned seg, addr_t const& addr ) { return f64_t( Expr( new Load<double>(  8, seg, addr.expr ) ) ); }
    f80_t                       fmemread80( unsigned seg, addr_t const& addr ) { return f80_t( Expr( new Load<long double>( 10, seg, addr.expr ) ) ); }

    template <unsigned OPSIZE>
    typename TypeFor<Arch,OPSIZE>::u
    regread( unsigned idx )
    {
      typedef typename TypeFor<Arch,OPSIZE>::u u_type;
      
      if (OPSIZE==8)                    return u_type( eregread( idx%4, 1, (idx>>2) & 1 ) );
      if ((OPSIZE==16) or (OPSIZE==32)) return u_type( eregread( idx, OPSIZE/8, 0 ) );
      throw 0;
      return u_type(0);
    }

    template <unsigned OPSIZE>
    void
    regwrite( unsigned idx, typename TypeFor<Arch,OPSIZE>::u const& value )
    {
      if  (OPSIZE==8)                   return eregwrite( idx%4, 1, (idx>>2) & 1, value.expr );
      if ((OPSIZE==16) or (OPSIZE==32)) return eregwrite( idx, OPSIZE/8, 0, value.expr );
      throw 0;
    }
    
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
      if (not rmop.is_memory_operand())
        return regread( g, rmop.ereg() );

      return memread<GOP::SIZE>( rmop->segment, rmop->effective_address( *this ) );
    }

   template <class GOP>
    void
    rmwrite( GOP const& g, RMOp const& rmop, typename TypeFor<Arch,GOP::SIZE>::u const& value )
    {
      if (not rmop.is_memory_operand())
        return regwrite( g, rmop.ereg(), value );

      return memwrite<GOP::SIZE>( rmop->segment, rmop->effective_address( *this ), value );
    }

    template <unsigned OPSIZE>
    typename TypeFor<Arch,OPSIZE>::f
    fpmemread( unsigned seg, addr_t const& addr )
    {
      typedef typename TypeFor<Arch,OPSIZE>::f f_type;
      typedef typename f_type::value_type f_ctype;
      return f_type( Expr( new Load<f_ctype>( OPSIZE/8, seg, addr.expr ) ) );
    }

    template <unsigned OPSIZE>
    typename TypeFor<Arch,OPSIZE>::f
    frmread( RMOp const& rmop )
    {
      typedef typename TypeFor<Arch,OPSIZE>::f f_type;
      if (not rmop.is_memory_operand()) return f_type( fread( rmop.ereg() ) );
      return this->fpmemread<OPSIZE>( rmop->segment, rmop->effective_address( *this ) );
    }
    
    template <unsigned OPSIZE>
    void
    fpmemwrite( unsigned seg, addr_t const& addr, typename TypeFor<Arch,OPSIZE>::f const& value )
    {
      stores.insert( new Store( OPSIZE/8, seg, addr.expr, value.expr ) );
    }
    
    template <unsigned OPSIZE>
    void
    frmwrite( RMOp const& rmop, typename TypeFor<Arch,OPSIZE>::f const& value )
    {
      if (not rmop.is_memory_operand()) return fwrite( rmop.ereg(), f64_t( value ) );
      fpmemwrite<OPSIZE>( rmop->segment, rmop->effective_address( *this ), value );
    }

    struct MXCSRRead : public RegReadBase
    {
      virtual MXCSRRead* Mutate() const override { return new MXCSRRead( *this ); }
      virtual char const* GetRegName() const override { return "mxcsr"; };
      virtual ScalarType::id_t GetType() const override { return ScalarType::U16; }
      virtual unsigned SubCount() const override { return 0; }
      virtual intptr_t cmp( ExprNode const& brhs ) const override { return 0; }
    };
    
    struct MXCSRWrite : public RegWriteBase
    {
      MXCSRWrite( Expr const& _value ) : RegWriteBase( _value ) {}
      virtual MXCSRWrite* Mutate() const override { return new MXCSRWrite( *this ); }
      virtual char const* GetRegName() const override { return "mxcsr"; }
    };
    
    u32_t mxcsr;

    struct VUConfig
    {
      static unsigned const BYTECOUNT = 32;
      struct Byte
      {
        Expr             src;
        ScalarType::id_t tp;
      };
      template <typename T>
      struct TypeInfo
      {
        enum { bytecount = unisim::component::cxx::processor::intel::atpinfo<Arch,T>::bitsize / 8 };
        static void ToBytes( Byte* dst, T& src ) { dst->src = src.expr; dst->tp = unisim::util::symbolic::TypeInfo<T>::GetType(); }
        static void FromBytes( T& dst, Byte const* src ) { throw 0; }
        static void Destroy( T& obj ) { obj.~T(); }
        static void Allocate( T& obj ) { new (&obj) T(); }
      };
    };

    struct VmmBrick { char _[sizeof(u8_t)]; };
    unisim::component::cxx::processor::intel::VUnion<VUConfig> umms[16];
    VmmBrick vmm_storage[16][VUConfig::BYTECOUNT];
    
    template <class VR> static unsigned vmm_wsize( VR const& vr ) { return VR::size() / 8; }
    static unsigned vmm_wsize( unisim::component::cxx::processor::intel::SSE const& ) { return VUConfig::BYTECOUNT; }

    template <class VR, class ELEM>
    ELEM vmm_read( VR const& vr, unsigned reg, u8_t const& sub, ELEM const& e )
    {
      throw 0;
      return ELEM();
    }
    
    template <class VR, class ELEM>
    ELEM vmm_read( VR const& vr, unsigned reg, unsigned sub, ELEM const& e )
    {
      ELEM const* elems = umms[reg].GetConstStorage( &vmm_storage[reg][0], e, vr.size() / 8 );
      return elems[sub];
    }
    
    template <class VR, class ELEM>
    void vmm_write( VR const& vr, unsigned reg, unsigned sub, ELEM const& e )
    {
      ELEM* elems = umms[reg].GetStorage( &vmm_storage[reg][0], e, vmm_wsize( vr ) );
      elems[sub] = e;
    }
    
    template <class VR, class ELEM>
    ELEM vmm_read( VR const&, RMOp const& rm, unsigned sub, ELEM const& )
    {
      throw 0;
      return ELEM();
    }

    template <class ELEM>
    ELEM vmm_memread( unsigned seg, addr_t const& addr, ELEM const& )
    {
      
      throw 0;
      return ELEM();
    }
    
    template <class ELEM>
    void vmm_memwrite( unsigned seg, addr_t const& addr, ELEM const& )
    {
      throw 0;
    }
    
    template <class VR, class ELEM>
    void vmm_write( VR const&, RMOp const& rm, unsigned sub, ELEM const& )
    {
      throw 0;
    }
    
    template<unsigned OPSIZE>
    typename TypeFor<Arch,OPSIZE>::u
    xmm_uread( unsigned reg, unsigned sub )
    {
      throw 0;
      return typename TypeFor<Arch,OPSIZE>::u( 0 );
    }

    template<unsigned OPSIZE>
    void
    xmm_uwrite( unsigned reg, unsigned sub, typename TypeFor<Arch,OPSIZE>::u const& val )
    {
      throw 0;
    }

    template<unsigned OPSIZE>
    typename TypeFor<Arch,OPSIZE>::u
    xmm_uread( RMOp const& rmop, unsigned sub )
    {
      if (not rmop.is_memory_operand()) return xmm_uread<OPSIZE>( rmop.ereg(), sub );
      return memread<OPSIZE>( rmop->segment, rmop->effective_address( *this ) + addr_t(sub*OPSIZE/8) );
    }

    template<unsigned OPSIZE>
    void
    xmm_uwrite( RMOp const& rmop, unsigned sub, typename TypeFor<Arch,OPSIZE>::u const& val )
    {
      if (not rmop.is_memory_operand()) return xmm_uwrite<OPSIZE>( rmop.ereg(), sub, val );
      return memwrite<OPSIZE>( rmop->segment, rmop->effective_address( *this ) + addr_t(sub*OPSIZE/8), val );
    }
    
    template <unsigned OPSIZE>
    typename TypeFor<Arch,OPSIZE>::f
    xmm_fread( unsigned reg, unsigned sub )
    {
      throw 0;
      return typename TypeFor<Arch,OPSIZE>::f( 0 );
    }
    
    template <unsigned OPSIZE>
    void
    xmm_fwrite( unsigned reg, unsigned sub, typename TypeFor<Arch,OPSIZE>::f const& val )
    {
      throw 0;
    }

    template <unsigned OPSIZE>
    typename TypeFor<Arch,OPSIZE>::f
    xmm_fread( RMOp const& rmop, unsigned sub )
    {
      if (not rmop.is_memory_operand()) return xmm_fread<OPSIZE>( rmop.ereg(), sub );
      return fpmemread<OPSIZE>( rmop->segment, rmop->effective_address( *this ) + addr_t(sub*OPSIZE/8) );
    }
    
    template <unsigned OPSIZE>
    void
    xmm_fwrite( RMOp const& rmop, unsigned sub, typename TypeFor<Arch,OPSIZE>::f const& val )
    {
      if (not rmop.is_memory_operand()) return xmm_fwrite<OPSIZE>( rmop.ereg(), sub, val );
      fpmemwrite<OPSIZE>( rmop->segment, rmop->effective_address( *this ) + addr_t(sub*OPSIZE/8), val );
    }

    void    interrupt( uint8_t _exc )   { throw ut::Untestable("system"); }
    void    syscall()                   { throw ut::Untestable("system"); }
    void    cpuid()                     { throw ut::Untestable("hardware"); }
    void    xgetbv()                    { throw ut::Untestable("hardware"); }
    void    stop()                      { throw ut::Untestable("hardware"); }
    void    _DE() { std::cerr << "Unimplemented division error trap.\n"; throw ut::Untestable("system"); }
    
    // bool Cond( bit_t b ) { return false; }
      
    template <typename T>
    bool Cond( unisim::util::symbolic::SmartValue<T> const& cond )
    {
      // if (not cond.expr.good()) throw std::logic_error( "Not a valid condition" );
      return Cond(bit_t(cond).expr);
    }
    bool Cond(Expr cond);
    
    void
    step( Operation const& op )
    {
      setnip( getnip() + addr_t(op.length) );
      op.execute( *this );
    }
    // typedef unisim::util::symbolic::Expr       Expr;
    // std::map<unsigned,VirtualRegister> irmap;
    // std::vector<unsigned>              iruse;
    // VirtualRegister                    xer, cr, spefscr;
    // std::set<Expr>                     mem_addrs;
    // uint8_t                            base_register;
    // bool                               aligned;
    // bool                               mem_writes;
    // uint8_t                            length;
    // bool                               retfalse;

  
    // typedef unisim::util::symbolic::ExprNode   ExprNode;
    
    // struct VirtualRegister
    // {
    //   VirtualRegister() : vindex(0), source(false), destination(false), bad(true) { throw 0; }
    //   VirtualRegister( unsigned _index ) : vindex(_index), source(false), destination(false), bad(false) {}
    
    //   uint8_t vindex      : 5;
    //   uint8_t source      : 1;
    //   uint8_t destination : 1;
    //   uint8_t bad         : 1;
    
    //   void addaccess( bool w ) { source |= not w; destination |= w; }
    
    //   int  cmp( VirtualRegister const& ) const;
    // };

    
    
    // void irappend( uint8_t _index, bool w );
    
    // //    bool usemem() const { return mem_addrs.size(); }
    
    
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
    
    //    void load( Expr const& addr ) { mem_addrs.insert( addr ); }
    //    void store( Expr const& addr ) { mem_addrs.insert( addr ); mem_writes = true; }
  };

  struct FIRoundBase : public unisim::util::symbolic::ExprNode
  {
    typedef unisim::util::symbolic::ScalarType ScalarType;
    typedef unisim::util::symbolic::Expr Expr;
    FIRoundBase( Expr const& _src, int _rmode ) : src(_src), rmode(_rmode) {} Expr src; int rmode;
    virtual void Repr( std::ostream& sink ) const;
    virtual unsigned SubCount() const { return 1; }
    virtual Expr const& GetSub(unsigned idx) const { if (idx != 0) return ExprNode::GetSub(idx); return src; }
  };

  template <typename fpT>
  struct FIRound : public FIRoundBase
  {
    typedef FIRound<fpT> this_type;
    FIRound( Expr const& src, int rmode ) : FIRoundBase(src, rmode) {}
    virtual this_type* Mutate() const override { return new this_type( *this ); }
    virtual ScalarType::id_t GetType() const { return unisim::util::symbolic::TypeInfo<fpT>::GetType(); }
    virtual intptr_t cmp( ExprNode const& brhs ) const
    {
      this_type const& rhs = dynamic_cast<this_type const&>( brhs );
      if (intptr_t delta = rmode - rhs.rmode) return delta;
      return src.cmp( rhs.src );
    }
  };

  struct DIV { static char const* name() { return "div;"; } };
  struct MUL { static char const* name() { return "mul;"; } };
  struct HI { static char const* name() { return "hi"; } };
  struct LO { static char const* name() { return "lo"; } };
  struct OF { static char const* name() { return "of"; } };

  struct DMopBase : public unisim::util::symbolic::ExprNode
  {
    typedef unisim::util::symbolic::Expr Expr;
    DMopBase( Expr const& _hi, Expr const& _lo, Expr const& _ro ) : hi(_hi), lo(_lo), ro(_ro) {} Expr hi, lo, ro;
    virtual ~DMopBase() {}
    virtual void Repr( std::ostream& sink ) const;
    virtual unsigned SubCount() const { return 3; }
    virtual Expr const& GetSub(unsigned idx) const { switch (idx) { case 0: return hi; case 1: return lo; case 2: return ro; } return ExprNode::GetSub(idx); }
    virtual char const* opname() const = 0;
    virtual char const* prodname() const = 0;
  };
  
  template <typename T, class OP, class PROD>
  struct DMop : public DMopBase
  {
    typedef unisim::util::symbolic::ScalarType ScalarType;
    typedef DMop<T,OP,PROD> this_type;
    DMop( Expr const& hi, Expr const& lo, Expr const& ro ) : DMopBase( hi, lo, ro ) {}
    virtual this_type* Mutate() const override { return new this_type( *this ); }
    virtual ScalarType::id_t GetType() const { return unisim::util::symbolic::TypeInfo<T>::GetType(); }
    virtual intptr_t cmp( ExprNode const& brhs ) const { return compare( dynamic_cast<this_type const&>( brhs ) ); }
    intptr_t compare( this_type const& rhs ) const
    {
      if (intptr_t delta = hi.cmp( rhs.hi )) return delta;
      if (intptr_t delta = lo.cmp( rhs.lo )) return delta;
      return ro.cmp( ro );
    }
    virtual char const* opname() const { return OP::name(); };
    virtual char const* prodname() const { return PROD::name(); };
  };
  
  template <class ARCH, typename INT>
  void eval_div64( ARCH& arch, INT& hi, INT& lo, INT const& divisor )
  {
    if (arch.Cond(divisor == INT(0))) arch._DE();
    lo = INT( unisim::util::symbolic::Expr( new DMop<INT,DIV,LO>( hi.expr, lo.expr, divisor.expr ) ) );
    hi = INT( unisim::util::symbolic::Expr( new DMop<INT,DIV,HI>( hi.expr, lo.expr, divisor.expr ) ) );
  }

  template <class ARCH, typename INT>
  void eval_mul64( ARCH& arch, INT& hi, INT& lo, INT const& multiplier )
  {
    typedef typename ARCH::bit_t bit_t;
    /*INT*/lo =   INT( unisim::util::symbolic::Expr( new DMop<INT,MUL,LO>( hi.expr, lo.expr, multiplier.expr ) ) );
    /*INT*/hi =   INT( unisim::util::symbolic::Expr( new DMop<INT,MUL,HI>( hi.expr, lo.expr, multiplier.expr ) ) );
    bit_t  of = bit_t( unisim::util::symbolic::Expr( new DMop<INT,MUL,OF>( hi.expr, lo.expr, multiplier.expr ) ) );
    arch.flagwrite( ARCH::FLAG::OF, of );
    arch.flagwrite( ARCH::FLAG::CF, of );
  }
  
  inline void eval_div( Arch& arch, Arch::u64_t& hi, Arch::u64_t& lo, Arch::u64_t const& divisor )    { eval_div64( arch, hi, lo, divisor ); }
  inline void eval_div( Arch& arch, Arch::s64_t& hi, Arch::s64_t& lo, Arch::s64_t const& divisor )    { eval_div64( arch, hi, lo, divisor ); }
  inline void eval_mul( Arch& arch, Arch::u64_t& hi, Arch::u64_t& lo, Arch::u64_t const& multiplier ) { eval_mul64( arch, hi, lo, multiplier ); }
  inline void eval_mul( Arch& arch, Arch::s64_t& hi, Arch::s64_t& lo, Arch::s64_t const& multiplier ) { eval_mul64( arch, hi, lo, multiplier ); }
  
} // end of namespace ut

#endif // ARCH_HH
