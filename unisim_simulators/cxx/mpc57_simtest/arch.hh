/*
 *  Copyright (c) 2015,
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

#include <unisim/util/symbolic/symbolic.hh>
#include <map>
#include <vector>
#include <list>

namespace ut
{
  using unisim::util::symbolic::SmartValue;
  typedef SmartValue<bool>     BOOL;
  typedef SmartValue<uint8_t>  U8;
  typedef SmartValue<uint16_t> U16;
  typedef SmartValue<uint32_t> U32;
  typedef SmartValue<uint64_t> U64;
  typedef SmartValue<int8_t>   S8;
  typedef SmartValue<int16_t>  S16;
  typedef SmartValue<int32_t>  S32;
  typedef SmartValue<int64_t>  S64;

  typedef SmartValue<uint32_t> ADDRESS;
  
  template <unsigned BITS> struct TypeFor {};
  template <> struct TypeFor<8> { typedef U8 U; typedef S8 S; };
  template <> struct TypeFor<16> { typedef U16 U; typedef S16 S; };
  template <> struct TypeFor<32> { typedef U32 U; typedef S32 S; };
  template <> struct TypeFor<64> { typedef U64 U; typedef S64 S; };
  
  struct UniqueVId {
    UniqueVId() : index(0) {} unsigned index;
    unsigned next() { return index++; }
    void reset() { index = 0; }
  };
  
  struct VirtualRegister
  {
    VirtualRegister() : vindex(0), source(false), destination(false), bad(true) {}
    VirtualRegister( unsigned _index ) : vindex(_index), source(false), destination(false), bad(false) {}
    
    uint8_t vindex      : 5;
    uint8_t source      : 1;
    uint8_t destination : 1;
    uint8_t bad         : 1;
    
    void allocate( UniqueVId& uvi ) { if (not bad) throw 0; vindex = uvi.next(); bad = false; }
    void addaccess( bool w ) { source |= not w; destination |= w; }
    
    int  cmp( VirtualRegister const& ) const;
  };
  
  struct Interface
  {
    typedef unisim::util::symbolic::Expr Expr;
    typedef unisim::util::symbolic::ExprNode ExprNode;
    
    Interface()
      : irmap(), iruse(), load_addrs(), store_addrs()
      , psr(0), length(0), itsensitive(false), base_register(-1), aligned(true)
    {}
    
    std::map<unsigned,VirtualRegister> irmap;
    std::vector<unsigned>              iruse;
    std::list<Expr>                    load_addrs;
    std::list<Expr>                    store_addrs;
    VirtualRegister                    psr;
    uint8_t                            length;
    bool                               itsensitive;
    uint8_t                            base_register;
    bool                               aligned;
    
    void irappend( uint8_t _index, bool w, UniqueVId& uvi );
    void readflags() { psr.addaccess( false ); }
    void writeflags() { psr.addaccess( true ); }
    
    int  cmp( Interface const& ) const;
    bool operator < ( Interface const& b ) const { return cmp( b ) < 0; }
    bool usemem() const { return store_addrs.size() or load_addrs.size(); }
    
    void finalize( uint8_t _length );
    
    struct Prologue
    {
      struct Error {};
      
      typedef std::map<unsigned,uint32_t> Regs;
      Prologue( Regs const& _regs, uint32_t _offset, bool _sign, uint8_t _base )
        : regs( _regs ), offset( _offset ), sign( _sign ), base( _base )
      {}
      Regs regs; uint32_t offset; bool sign; uint8_t base;
    };
    
    Prologue GetPrologue() const;
    
    void PCRelPrologue( Prologue const& pc ) const;
    struct Untestable
    {
      std::string argument;
      Untestable( std::string part ) : argument( part ) {}
      Untestable( std::string part, Untestable const& ut ) : argument( part + ' ' + ut.argument ) {}
    };
  };
  
  struct SourceReg : public unisim::util::symbolic::ExprNode
  {
    SourceReg( unsigned _reg ) : reg( _reg ) {} unsigned reg;
    virtual void Repr( std::ostream& sink ) const;
    virtual void Traverse( ExprNode::Visitor& visitor ) {}
    virtual intptr_t cmp( unisim::util::symbolic::ExprNode const& brhs ) const
    {
      unsigned ref = dynamic_cast<SourceReg const&>( brhs ).reg;
      return (reg < ref) ? -1 : (reg > ref) ? +1 : 0;
    }
    virtual unisim::util::symbolic::ExprNode* GetConstNode() { return 0; }
  };
  
  struct Reject {};
  struct CPU;
  
  struct XER
  {
    struct OV {};
    struct SO {};
    struct CA {};
    struct _0_3 {};
    template <typename PART,typename T> void Set( SmartValue<T> const& value ) {}
    template <typename PART> void Set( unsigned ) {}
    template <typename PART> U32 Get() { return U32(0); }
    operator U32 () const { return U32(0); }
    XER& operator= (U32 const& v) { return *this; }
    XER& GetXER() { return *this; }
  };

  struct CR
  {
    struct CR0 { struct OV {}; struct SO {}; struct LT {}; struct GT {}; struct EQ {}; struct ALL {}; };
    struct CR1 { struct OV {}; struct SO {}; struct LT {}; struct GT {}; struct EQ {}; struct ALL {}; };
    struct CR2 { struct OV {}; struct SO {}; struct LT {}; struct GT {}; struct EQ {}; struct ALL {}; };
    struct CR3 { struct OV {}; struct SO {}; struct LT {}; struct GT {}; struct EQ {}; struct ALL {}; };
    struct CR4 { struct OV {}; struct SO {}; struct LT {}; struct GT {}; struct EQ {}; struct ALL {}; };
    struct CR5 { struct OV {}; struct SO {}; struct LT {}; struct GT {}; struct EQ {}; struct ALL {}; };
    struct CR6 { struct OV {}; struct SO {}; struct LT {}; struct GT {}; struct EQ {}; struct ALL {}; };
    struct CR7 { struct OV {}; struct SO {}; struct LT {}; struct GT {}; struct EQ {}; struct ALL {}; };
    template <typename PART,typename T> void Set( SmartValue<T> const& value ) {}
    template <typename PART> void Set( unsigned ) {}
    template <typename PART> U32 Get() { return U32(0); }
    operator U32 () const { return U32(0); }
    CR& operator= (U32 const& v) { return *this; }
    CR& GetCR() { return *this; }
  };
  
  struct LR
  {
    template <typename PART,typename T> void Set( SmartValue<T> const& value ) {}
    template <typename PART> void Set( unsigned ) {}
    template <typename PART> U32 Get() { return U32(0); }
    operator U32 () const { return U32(0); }
    LR& operator= (U32 const& v) { return *this; }
    LR& GetLR() { return *this; }
    void SetLR(U32 const& v) {}
  };
  
  struct CTR
  {
    template <typename PART,typename T> void Set( SmartValue<T> const& value ) {}
    template <typename PART> void Set( unsigned ) {}
    template <typename PART> U32 Get() { return U32(0); }
    operator U32 () const { return U32(0); }
    CTR& operator= (U32 const& v) { return *this; }
    CTR& GetCTR() { return *this; }
    void SetCTR(U32 const& v) {}
  };
  
  struct MSR
  {
    struct PR {};
    struct EE {};
    template <typename PART,typename T> void Set( SmartValue<T> const& value ) {}
    template <typename PART> void Set( unsigned ) {}
    template <typename PART> U32 Get() { return U32(0); }
    operator U32 () const { return U32(0); }
    MSR& operator= (U32 const& v) { return *this; }
    MSR& GetMSR() { return *this; }
  };

  struct CPU : public XER, public CR, public MSR, public LR, public CTR
  {
    typedef ut::BOOL BOOL;
    typedef ut::U8   U8;
    typedef ut::U16  U16;
    typedef ut::U32  U32;
    typedef ut::U64  U64;
    typedef ut::S8   S8;
    typedef ut::S16  S16;
    typedef ut::S16  S32;
    typedef ut::S16  S64;
    
    typedef unisim::util::symbolic::Expr Expr;
    typedef unisim::util::symbolic::ExprNode ExprNode;
    
    typedef MSR MSR;
    
    struct Interrupt { void SetELEV(unsigned x); };
    
    struct SystemCallInterrupt
    {
      struct SystemCall {};
    };
    
    struct AlignmentInterrupt
    {
      struct UnalignedLoadStoreMultiple {};
    };
    
    struct ProgramInterrupt
    {
      struct UnimplementedInstruction {};
      struct IllegalInstruction {};
      struct Trap {};
      struct PrivilegeViolation {};
    };
    
    template <class T> Interrupt ThrowException() { reject(); return Interrupt(); }
    
    template <typename T> bool Cond( SmartValue<T> const& c ) { return false; }

    Interface interface;
    UniqueVId gpr_uvi;
    U32   reg_values[32];
    U32   cia;
    
    U32 GetCIA() { return cia; };
    bool EqualCIA(uint32_t pc) { return false; };
    U32 GetGPR(unsigned n) { return reg_values[n]; };
    void SetGPR(unsigned n, U32 value) { reg_values[n] = value; }
    void SetGPR(unsigned n, S32 value) { reg_values[n] = U32(value); }
    
    static void LoadRepr( std::ostream& sink, Expr const& _addr, unsigned bits );
    
    template <unsigned BITS>
    struct Load : public ExprNode
    {
      Load( Expr const& _addr ) : addr(_addr) {}
      virtual void Repr( std::ostream& sink ) const { LoadRepr( sink, addr, BITS ); }
      virtual void Traverse( ExprNode::Visitor& visitor ) { visitor.Process(addr); }
      virtual intptr_t cmp( unisim::util::symbolic::ExprNode const& brhs ) const
      {
        return addr.cmp( dynamic_cast<Load<BITS> const&>( brhs ).addr );
      }
      virtual unisim::util::symbolic::ExprNode* GetConstNode() { return 0; }
      Expr addr;
    };
    
    template <unsigned BITS> Expr MemRead( U32 const& _addr )
    {
      interface.load_addrs.push_back( _addr.expr );
      return new Load<BITS>( _addr.expr );
    }
    template <unsigned BITS> void MemWrite( U32 const& _addr, typename TypeFor<BITS>::U const& _val )
    {
      interface.store_addrs.push_back( _addr.expr );
    }
    
    bool Int8Load(unsigned n, U32 const& address) { SetGPR(n, CPU::U32(CPU::U8(MemRead<8>(address)))); return true; }
    bool Int16Load(unsigned n, U32 const& address) { SetGPR(n, CPU::U32(CPU::U16(MemRead<16>(address)))); return true; }
    bool Int32Load(unsigned n, U32 const& address) { SetGPR(n, CPU::U32(MemRead<32>(address))); return true; }
    
    bool SInt8Load(unsigned n, U32 const& address) { SetGPR(n, CPU::S32(CPU::S8(MemRead<8>(address)))); return true; }
    bool SInt16Load(unsigned n, U32 const& address) { SetGPR(n, CPU::S32(CPU::S16(MemRead<16>(address)))); return true; }

    bool Int16LoadByteReverse(unsigned n, U32 const& address) { SetGPR(n, ByteSwap(CPU::U32(CPU::U16(MemRead<16>(address))))); return true; }
    bool Int32LoadByteReverse(unsigned n, U32 const& address) { SetGPR(n, ByteSwap(CPU::U32(MemRead<32>(address)))); return true; }
    
    bool Int8Store(unsigned n, U32 const& address ) { MemWrite<8>( address, U8(GetGPR(n)) ); return true; }
    bool Int16Store(unsigned n, U32 const& address ) { MemWrite<16>( address, U16(GetGPR(n)) ); return true; }
    bool Int32Store(unsigned n, U32 const& address ) { MemWrite<32>( address, U32(GetGPR(n)) ); return true; }

    bool Int16StoreByteReverse(unsigned n, U32 const& address ) { MemWrite<16>( address, ByteSwap(U16(GetGPR(n))) ); return true; }
    bool Int32StoreByteReverse(unsigned n, U32 const& address ) { MemWrite<32>( address, ByteSwap(U32(GetGPR(n))) ); return true; }

    
    void gpr_append( unsigned idx, bool w ) { interface.irappend( idx, w, gpr_uvi ); }

    void reject();
    
    void donttest_system();
    void donttest_branch();
    
    char const* GetObjectFriendlyName(U32) { return "???"; }
    
    bool Branch(U32 const& addr) { donttest_branch(); return false; }
    
    bool Rfmci() { donttest_system(); return false; }
    bool Rfci() { donttest_system(); return false; }
    bool Rfdi() { donttest_system(); return false; }
    bool Rfi() { donttest_system(); return false; }

    bool Dcba(U32 const& addr) { donttest_system(); return false; }
    bool Dcbf(U32 const& addr) { donttest_system(); return false; }
    bool Dcbst(U32 const& addr) { donttest_system(); return false; }
    bool Dcbz(U32 const& addr) { donttest_system(); return false; }
    bool Dcbi(U32 const& addr) { donttest_system(); return false; }
    bool Icbi(U32 const& addr) { donttest_system(); return false; }
    bool Icbt(U32 const& addr) { donttest_system(); return false; }
    
    bool Msync() { donttest_system(); return false; }
    bool Isync() { donttest_system(); return false; }
    bool Mpure() { donttest_system(); return false; }
    bool Mpuwe() { donttest_system(); return false; }
    bool Mpusync() { donttest_system(); return false; }
    
    bool Lbarx(unsigned n, U32 const& addr) { donttest_system(); return false; }
    bool Lharx(unsigned n, U32 const& addr) { donttest_system(); return false; }
    bool Lwarx(unsigned n, U32 const& addr) { donttest_system(); return false; }
    bool Stbcx(unsigned n, U32 const& addr) { donttest_system(); return false; }
    bool Sthcx(unsigned n, U32 const& addr) { donttest_system(); return false; }
    bool Stwcx(unsigned n, U32 const& addr) { donttest_system(); return false; }
    bool MoveFromDCR(unsigned dcrn, U32& result) { donttest_system(); return false; }
    bool MoveFromSPR(unsigned dcrn, U32& result) { donttest_system(); return false; }
    bool MoveToSPR(unsigned dcrn, U32 const& result) { donttest_system(); return false; }
  };
  
  extern void SignedAdd32(U32& result, U8& carry_out, U8& overflow, U8& sign, U32 x, U32 y, U8 carry_in);
  extern inline void SignedAdd32(U32& result, U8& carry_out, U8& overflow, U8& sign, U32 x, U32 y, int carry_in) { return SignedAdd32(result, carry_out, overflow, sign, x, y, U8(carry_in)); }

  struct MaskNode : public unisim::util::symbolic::ExprNode
  {
    typedef unisim::util::symbolic::Expr Expr;
    
    MaskNode( Expr const& _mb, Expr const& _me ) : mb(_mb), me(_me) {}
    virtual void Repr( std::ostream& sink ) const;
    virtual void Traverse( ExprNode::Visitor& visitor ) { visitor.Process(mb); visitor.Process(me); }
    virtual intptr_t cmp( ExprNode const& brhs ) const
    {
      MaskNode const& rhs = dynamic_cast<MaskNode const&>( brhs );
      if (intptr_t delta = mb.cmp( rhs.mb )) return delta;
      return me.cmp( rhs.me );
    }
    virtual ExprNode* GetConstNode() { return 0; }
    Expr mb, me;
  };
    
  extern U32 Mask(U32 mb, U32 me);
}

#endif // ARCH_HH
