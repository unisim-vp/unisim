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
#include <set>

namespace e5500
{
  struct Operation;
}

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

  typedef SmartValue<uint64_t> ADDRESS;
  typedef SmartValue<uint64_t> UINT;
  typedef SmartValue<int64_t>  SINT;
  
  template <unsigned BITS> struct TypeFor {};
  template <> struct TypeFor<8> { typedef U8 U; typedef S8 S; };
  template <> struct TypeFor<16> { typedef U16 U; typedef S16 S; };
  template <> struct TypeFor<32> { typedef U32 U; typedef S32 S; };
  template <> struct TypeFor<64> { typedef U64 U; typedef S64 S; };
  
  struct Untestable
  {
    Untestable(std::string const& _reason) : reason(_reason) {}
    std::string reason;
  };
  
  struct PathNode
  {
    typedef unisim::util::symbolic::Expr Expr;

    PathNode( PathNode* _previous=0 );
    ~PathNode();

    bool        proceed( Expr const& _cond );
    bool        close();
    PathNode*   next( bool predicate ) const { return predicate ? true_nxt : false_nxt; }
    
    Expr cond;
    PathNode* previous;
    PathNode* true_nxt;
    PathNode* false_nxt;
    bool complete;
  };

  struct Arch;

  struct ArchExprNode : public unisim::util::symbolic::ExprNode
  {
    ArchExprNode( Arch& _arch ) : arch(_arch) {} Arch& arch;

    static Arch* SeekArch( unisim::util::symbolic::Expr const& expr )
    {
      if (ArchExprNode const* node = dynamic_cast<ArchExprNode const*>( expr.node ))
        return &node->arch;
      for (unsigned idx = 0, end = expr->SubCount(); idx < end; ++idx)
        if (Arch* found = SeekArch( expr->GetSub(idx)))
          return found;
      return 0;
    }
  };
  
  struct Interface
  {
    typedef unisim::util::symbolic::Expr Expr;
    typedef unisim::util::symbolic::ExprNode ExprNode;
    
    Interface( e5500::Operation& op );
    
    struct VirtualRegister
    {
      VirtualRegister() : vindex(0), source(false), destination(false), bad(true) { throw 0; }
      VirtualRegister( unsigned _index ) : vindex(_index), source(false), destination(false), bad(false) {}
    
      uint8_t vindex      : 5;
      uint8_t source      : 1;
      uint8_t destination : 1;
      uint8_t bad         : 1;
    
      void addaccess( bool w ) { source |= not w; destination |= w; }
    
      int  cmp( VirtualRegister const& ) const;
    };
    
    std::map<unsigned,VirtualRegister> irmap;
    std::vector<unsigned>              iruse;
    VirtualRegister                    xer, cr, spefscr;
    std::set<Expr>                     mem_addrs;
    uint8_t                            base_register;
    bool                               aligned;
    bool                               mem_writes;
    uint8_t                            length;
    bool                               retfalse;

    
    void irappend( uint8_t _index, bool w );
    
    int  cmp( Interface const& ) const;
    bool operator < ( Interface const& b ) const { return cmp( b ) < 0; }
    bool usemem() const { return mem_addrs.size(); }
    
    
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
    
    void load( Expr const& addr ) { mem_addrs.insert( addr ); }
    void store( Expr const& addr ) { mem_addrs.insert( addr ); mem_writes = true; }
  };
  
  struct SourceReg : public ArchExprNode
  {
    SourceReg( Arch& _arch, unsigned _reg ) : ArchExprNode( _arch ), reg( _reg ) {}
    virtual void Repr( std::ostream& sink ) const;
    virtual unsigned SubCount() const { return 0; };
    virtual intptr_t cmp( unisim::util::symbolic::ExprNode const& brhs ) const
    {
      unsigned ref = dynamic_cast<SourceReg const&>( brhs ).reg;
      return (reg < ref) ? -1 : (reg > ref) ? +1 : 0;
    }
    unsigned reg;
  };
  
  struct MixNode : public unisim::util::symbolic::ExprNode
  {
    typedef unisim::util::symbolic::Expr Expr;
    typedef unisim::util::symbolic::ExprNode ExprNode;
    
    MixNode( Expr const& _left, Expr const& _right ) : left(_left), right(_right) {}
    virtual void Repr( std::ostream& sink ) const;
    virtual unsigned SubCount() const { return 2; };
    virtual Expr const& GetSub(unsigned idx) const { switch (idx) { case 0: return left; case 1: return right; } return ExprNode::GetSub(idx); };
    virtual intptr_t cmp( ExprNode const& brhs ) const
    {
      MixNode const& rhs = dynamic_cast<MixNode const&>( brhs );
      if (intptr_t delta = left.cmp( rhs.left )) return delta;
      return right.cmp( rhs.right );
    }
    Expr left, right;
  };
  
  struct Unknown : public ArchExprNode
  {
    typedef unisim::util::symbolic::Expr Expr;
    typedef unisim::util::symbolic::ExprNode ExprNode;
    
    Unknown( Arch& _arch ) : ArchExprNode( _arch ) {}
    virtual void Repr( std::ostream& sink ) const { sink << "?"; }
    virtual unsigned SubCount() const { return 0; };
    virtual Expr const& GetSub(unsigned idx) const { return ExprNode::GetSub(idx); };
    virtual intptr_t cmp( ExprNode const& brhs ) const { return 0; }
  };
  
  template <class T>
  T make_unknown( Arch* arch ) { if (not arch) throw 0; return T( Unknown::Expr( new Unknown( *arch ) ) ); }
  
  struct XER
  {
    struct OV {};
    struct SO {};
    struct CA {};
    struct _0_3 {};
    
    typedef unisim::util::symbolic::Expr Expr;
    typedef unisim::util::symbolic::ExprNode ExprNode;
    
    template <typename PART,typename T> void Set( SmartValue<T> const& value ) { XERAccess(true); xer_value = Expr( new MixNode( xer_value.expr, value.expr ) ); }
    template <typename PART> void Set( uint32_t value ) { Set<PART,uint32_t>( unisim::util::symbolic::make_const(value) ); }
    template <typename PART> U64 Get() { XERAccess(false); return xer_value; }
    operator U64 () { XERAccess(false); return xer_value; }
    XER& operator= ( U64 const& value ) { XERAccess(true); xer_value = value; return *this; }
    XER& GetXER() { return *this; }
    
    struct XERNode : public ArchExprNode
    {
      XERNode( Arch& _arch ) : ArchExprNode( _arch ) {}
      virtual void Repr( std::ostream& sink ) const;
      virtual unsigned SubCount() const { return 0; };
      virtual intptr_t cmp( ExprNode const& brhs ) const { return 0; }
    };
    
    XER( Arch& _arch ) : xer_value( new XERNode( _arch ) ) {}
    
    U64 xer_value;
    virtual void XERAccess( bool is_write ) = 0;
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
    
    typedef unisim::util::symbolic::Expr Expr;
    typedef unisim::util::symbolic::ExprNode ExprNode;
    
    template <typename PART,typename T> void Set( SmartValue<T> const& value ) { CRAccess(true); cr_value = Expr( new MixNode( cr_value.expr, value.expr ) ); }
    template <typename PART> void Set( uint32_t value ) { Set<PART,uint32_t>( unisim::util::symbolic::make_const(value) ); }
    template <typename PART> U64 Get() { CRAccess(false); return cr_value; }
    operator U64 () { CRAccess(false); return cr_value; }
    CR& operator= ( U64 const& value ) { CRAccess(true); cr_value = value; return *this; }
    CR& GetCR() { return *this; }
    
    struct CRNode : public ArchExprNode
    {
      CRNode( Arch& _arch ) : ArchExprNode( _arch ) {}
      virtual void Repr( std::ostream& sink ) const;
      virtual unsigned SubCount() const { return 0; };
      virtual intptr_t cmp( ExprNode const& brhs ) const { return 0; }
    };

    CR( Arch& _arch ) : cr_value( new CRNode( _arch ) ) {}
    
    U64 cr_value;
    virtual void CRAccess( bool is_write ) = 0;
  };
  
  struct LR
  {
    template <typename PART,typename T> void Set( SmartValue<T> const& value ) { throw Untestable("LR access");  }
    template <typename PART> void Set( unsigned ) { throw Untestable("LR access");  }
    template <typename PART> U64 Get() { return U64(0); }
    operator U64 () { return U64(0); }
    LR& operator= (U64 const& v) { throw Untestable("LR access"); return *this; }
    LR& GetLR() { return *this; }
    void SetLR(U64 const& v) { throw Untestable("LR access"); }
  };
  
  struct CTR
  {
    template <typename PART,typename T> void Set( SmartValue<T> const& value ) { throw Untestable("CTR access"); }
    template <typename PART> void Set( unsigned ) { throw Untestable("CTR access"); }
    template <typename PART> U64 Get() { return U64(0); }
    operator U64 () { return U64(0); }
    CTR& operator= (U64 const& v) { throw Untestable("CTR access"); return *this; }
    CTR& GetCTR() { return *this; }
    void SetCTR(U64 const& v) { throw Untestable("CTR access"); }
  };
  
  struct MSR
  {
    struct PR {};
    struct EE {};
    template <typename PART,typename T> void Set( SmartValue<T> const& value ) { throw Untestable("MSR access"); }
    template <typename PART> void Set( unsigned ) { throw Untestable("MSR access"); }
    template <typename PART> U64 Get() { throw Untestable("MSR access"); return U64(0); }
    operator U64 () { throw Untestable("MSR access"); return U64(0); }
    MSR& operator= (U64 const& v) { throw Untestable("MSR access"); return *this; }
    MSR& GetMSR() { throw Untestable("MSR access"); return *this; }
  };
    
  struct Arch : public XER, public CR, public MSR, public LR, public CTR
  {
    typedef ut::BOOL BOOL;
    typedef ut::U8   U8;
    typedef ut::U16  U16;
    typedef ut::U32  U32;
    typedef ut::U64  U64;
    typedef ut::S8   S8;
    typedef ut::S16  S16;
    typedef ut::S32  S32;
    typedef ut::S64  S64;

    typedef ut::UINT UINT;
    typedef ut::SINT SINT;
    typedef ut::ADDRESS ADDRESS;
    
    
    typedef unisim::util::symbolic::Expr Expr;
    typedef unisim::util::symbolic::ExprNode ExprNode;
    
    typedef MSR MSR;
    // typedef SPEFSCR SPEFSCR;
    
    Arch( Interface& _interface, PathNode& root )
      : XER(*this), CR(*this), interface(_interface), path(&root), cia( new CIA( *this ) )
    {     
      for (unsigned reg = 0; reg < 32; ++reg)
        reg_values[reg] = U64( new SourceReg( *this, reg ) );
    }
    
    virtual void XERAccess( bool is_write ) { interface.xer.addaccess(is_write); }
    virtual void CRAccess( bool is_write ) { interface.cr.addaccess(is_write); }
    virtual void SPEFSCRAccess( bool is_write ) { interface.spefscr.addaccess(is_write); }
    
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
    
    template <class T> Interrupt ThrowException() { DispatchException( T() ); return Interrupt(); }
    
    template <class T> void DispatchException( T const& exc ) { donttest_system(); }
    void DispatchException( ProgramInterrupt::UnimplementedInstruction const& exc ) { throw Untestable("not implemented"); }
    void DispatchException( AlignmentInterrupt::UnalignedLoadStoreMultiple const& exc ) { interface.aligned = true; }
    
    
    bool Choose( Expr const& cexp )
    {
      bool predicate = path->proceed( cexp );
      path = path->next( predicate );
      return predicate;
    }
    
    template <typename T>
    static bool Concretize( SmartValue<T> const& cond )
    {
      if (not cond.expr.good())
        throw std::logic_error( "Not a valid condition" );

      Expr cexp( BOOL(cond).expr );
      if (unisim::util::symbolic::ConstNodeBase const* cnode = cexp.ConstSimplify())
        return cnode->GetBoolean();
      
      // struct
      // {
      //   Arch*
      //   arch( Expr const& expr )
      //   {
      //     if (ArchExprNode const* node = dynamic_cast<ArchExprNode const*>( expr.node ))
      //       return node->arch;
      //     for (unsigned idx = 0, end = expr->SubCount(); idx < end; ++idx)
      //       if (Arch* found = arch( expr->GetSub(idx)))
      //         return found;
      //     return 0;
      //   }
      // } seeker;
      
      Arch* arch = ArchExprNode::SeekArch(cexp);
      if (not arch)
        throw 0;
      
      return arch->Choose( cexp );
    }
    
    static bool Concretize( bool c ) { return c; }
    
    bool close() { return path->close(); }

    Interface& interface;
    PathNode*  path;
    U64        reg_values[32];
    U64        cia;
    
    
    struct CIA : public ArchExprNode
    {
      CIA( Arch& _arch ) : ArchExprNode( _arch ) {}
      
      virtual void Repr( std::ostream& sink ) const { sink << "CIA"; }
      virtual unsigned SubCount() const { return 0; };
      virtual intptr_t cmp( unisim::util::symbolic::ExprNode const& brhs ) const { return 0; }
    };
    
    // U32 GetCIA() { return cia; };
    // bool EqualCIA(uint32_t pc) { return false; };
    // U32 GetGPR(unsigned n) { gpr_append(n,false); return reg_values[n]; };
    // void SetGPR(unsigned n, U32 value) { gpr_append(n,true); reg_values[n] = value; }
    
    // static void LoadRepr( std::ostream& sink, Expr const& _addr, unsigned bits );
    
    // template <unsigned BITS>
    // struct Load : public ArchExprNode
    // {
    //   Load( Expr const& _addr ) : addr(_addr) {}
    //   virtual void Repr( std::ostream& sink ) const { LoadRepr( sink, addr, BITS ); }
    //   virtual unsigned SubCount() const { return 2; };
    //   virtual Expr const& GetSub(unsigned idx) const { switch (idx) { case 0: return addr; } return ExprNode::GetSub(idx); };
    //   virtual intptr_t cmp( unisim::util::symbolic::ExprNode const& brhs ) const
    //   { return addr.cmp( dynamic_cast<Load<BITS> const&>( brhs ).addr ); }
    //   Expr addr;
    // };
    
    // template <unsigned BITS> Expr MemRead( U32 const& _addr )
    // {
    //   interface.load( _addr.expr );
    //   return new Load<BITS>( _addr.expr );
    // }
    // template <unsigned BITS> void MemWrite( U32 const& _addr, typename TypeFor<BITS>::U const& _val )
    // {
    //   interface.store( _addr.expr );
    // }
    
    // bool Int8Load(unsigned n, U32 const& address) { SetGPR(n, Arch::U32(Arch::U8(MemRead<8>(address)))); return true; }
    // bool Int16Load(unsigned n, U32 const& address) { SetGPR(n, Arch::U32(Arch::U16(MemRead<16>(address)))); return true; }
    // bool Int32Load(unsigned n, U32 const& address) { SetGPR(n, Arch::U32(MemRead<32>(address))); return true; }
    
    // bool SInt8Load(unsigned n, U32 const& address) { SetGPR(n, Arch::U32(Arch::S8(MemRead<8>(address)))); return true; }
    // bool SInt16Load(unsigned n, U32 const& address) { SetGPR(n, Arch::U32(Arch::S16(MemRead<16>(address)))); return true; }

    // bool Int16LoadByteReverse(unsigned n, U32 const& address) { SetGPR(n, ByteSwap(Arch::U32(Arch::U16(MemRead<16>(address))))); return true; }
    // bool Int32LoadByteReverse(unsigned n, U32 const& address) { SetGPR(n, ByteSwap(Arch::U32(MemRead<32>(address)))); return true; }
    
    // bool Int8Store(unsigned n, U32 const& address ) { MemWrite<8>( address, U8(GetGPR(n)) ); return true; }
    // bool Int16Store(unsigned n, U32 const& address ) { MemWrite<16>( address, U16(GetGPR(n)) ); return true; }
    // bool Int32Store(unsigned n, U32 const& address ) { MemWrite<32>( address, U32(GetGPR(n)) ); return true; }

    // bool Int16StoreByteReverse(unsigned n, U32 const& address ) { MemWrite<16>( address, ByteSwap(U16(GetGPR(n))) ); return true; }
    // bool Int32StoreByteReverse(unsigned n, U32 const& address ) { MemWrite<32>( address, ByteSwap(U32(GetGPR(n))) ); return true; }

    // void gpr_append( unsigned idx, bool w ) { interface.irappend( idx, w ); }

    void donttest_system();
    // void donttest_branch();
    // void donttest_illegal();
    
    // char const* GetObjectFriendlyName(U32) { return "???"; }
    
    // bool Branch(U32 const& addr) { donttest_branch(); return false; }
    
    // bool Rfmci() { donttest_system(); return false; }
    // bool Rfci() { donttest_system(); return false; }
    // bool Rfdi() { donttest_system(); return false; }
    // bool Rfi() { donttest_system(); return false; }

    // bool Dcba(U32 const& addr) { donttest_system(); return false; }
    // bool Dcbf(U32 const& addr) { donttest_system(); return false; }
    // bool Dcbst(U32 const& addr) { donttest_system(); return false; }
    // bool Dcbz(U32 const& addr) { donttest_system(); return false; }
    // bool Dcbi(U32 const& addr) { donttest_system(); return false; }
    // bool Icbi(U32 const& addr) { donttest_system(); return false; }
    // bool Icbt(U32 const& addr) { donttest_system(); return false; }
    
    // bool Msync() { donttest_system(); return false; }
    // bool Isync() { donttest_system(); return false; }
    // bool Mpure() { donttest_system(); return false; }
    // bool Mpuwe() { donttest_system(); return false; }
    // bool Mpusync() { donttest_system(); return false; }
    
    // bool Lbarx(unsigned n, U32 const& addr) { donttest_system(); return false; }
    // bool Lharx(unsigned n, U32 const& addr) { donttest_system(); return false; }
    // bool Lwarx(unsigned n, U32 const& addr) { donttest_system(); return false; }
    // bool Stbcx(unsigned n, U32 const& addr) { donttest_system(); return false; }
    // bool Sthcx(unsigned n, U32 const& addr) { donttest_system(); return false; }
    // bool Stwcx(unsigned n, U32 const& addr) { donttest_system(); return false; }
    // bool MoveFromDCR(unsigned dcrn, U32& result) { donttest_system(); return false; }
    // bool MoveFromSPR(unsigned dcrn, U32& result) { donttest_system(); return false; }
    // bool MoveToSPR(unsigned dcrn, U32 const& result) { donttest_system(); return false; }
    
    // bool Wait() { return false; }
  };
  
  struct Flags
  {
    struct RoundingMode { RoundingMode(UINT const& rm) : arch(ArchExprNode::SeekArch(rm.expr)) {} RoundingMode(unsigned rm) : arch(0) {} Arch* arch; };
    Flags( RoundingMode const& rm ) : arch(rm.arch) {}
    // Flags() {}
    // BOOL isUpApproximate() { return make_unknown<BOOL>(); }
    // BOOL isDownApproximate() { return make_unknown<BOOL>(); }
    // BOOL takeOverflow() { return make_unknown<BOOL>(); }

    BOOL hasIncrementFraction(bool neg) const { return make_unknown<BOOL>(arch); }           // FPSCR.FR
    BOOL isApproximate() const { return make_unknown<BOOL>(arch); }                          // FPSCR.FI
    BOOL isOverflow() const { return make_unknown<BOOL>(arch); }                             // FPSCR.OX
    BOOL isUnderflow() const { return make_unknown<BOOL>(arch); }                            // FPSCR.UX
    BOOL isDivisionByZero() const { return make_unknown<BOOL>(arch); }                       // FPSCR.ZX
    BOOL hasSNaNOperand() const { return make_unknown<BOOL>(arch); }                         // FPSCR.VXSNAN
    BOOL isInftyMinusInfty() const { return make_unknown<BOOL>(arch); }                      // FPSCR.VXISI
    BOOL isInftyOnInfty() const { return make_unknown<BOOL>(arch); }                         // FPSCR.VXIDI
    BOOL isInftyMultZero() const { return make_unknown<BOOL>(arch); }                        // FPSCR.VXIMZ
    BOOL isZeroOnZero() const { return make_unknown<BOOL>(arch); }                           // FPSCR.VXZDZ

    void touch( unisim::util::symbolic::Expr const& _expr ) { if (not arch) arch = ArchExprNode::SeekArch(_expr); }
    void touch( Arch* _arch ) { if (not arch) arch = _arch; }

    Arch* arch;
  };
  
  struct SoftHalfFloat;
  
  struct SoftFloat
  {
    enum __FromRawBits__ { FromRawBits };
    SoftFloat( __FromRawBits__, U32 const& source ) : arch(ArchExprNode::SeekArch(source.expr)) {}
    
    SoftFloat( U32 const& source, Flags& flags ) : arch(ArchExprNode::SeekArch(source.expr)) { flags.touch(source.expr); }
    SoftFloat( S32 const& source, Flags& flags ) : arch(ArchExprNode::SeekArch(source.expr)) { flags.touch(source.expr); }
    enum __FromFraction__ { FromFraction };
    SoftFloat( __FromFraction__, U32 const& source, Flags& flags ) : arch(ArchExprNode::SeekArch(source.expr)) { flags.touch(source.expr); }
    SoftFloat( __FromFraction__, S32 const& source, Flags& flags ) : arch(ArchExprNode::SeekArch(source.expr)) { flags.touch(source.expr); }
    SoftFloat( SoftHalfFloat const& source, Flags& flags );
    
    U32 queryU32( Flags& flags, unsigned fbits=0 ) { flags.touch(arch); return make_unknown<U32>(arch); }
    S32 queryS32( Flags& flags, unsigned fbits=0 ) { flags.touch(arch); return make_unknown<S32>(arch); }

    // SoftFloat& convertAssign( SoftHalfFloat const& source, Flags& flags );
    // SoftFloat& fromRawBitsAssign( U32 const& );
    
    U32 queryRawBits() { return make_unknown<U32>(arch); }
    BOOL isNaN() { return make_unknown<BOOL>(arch); }
    BOOL operator == (SoftFloat const&) { return make_unknown<BOOL>(arch); }
    BOOL operator < (SoftFloat const&) { return make_unknown<BOOL>(arch); }
    BOOL operator > (SoftFloat const&) { return make_unknown<BOOL>(arch); }
    // void retrieveInteger(IntConversion const&, Flags const&) {}
    // BOOL isPositive() { return make_unknown<BOOL>(arch); }
    BOOL isNegative() { return make_unknown<BOOL>(arch); }
    void setNegative(bool=false) {}
    void setNegative(BOOL) {}
    void plusAssign(SoftFloat const&, Flags const& flags) {}
    void divAssign(SoftFloat const&, Flags const& flags) {}
    void multAssign(SoftFloat const&, Flags const& flags) {}
    void minusAssign(SoftFloat const&, Flags const& flags) {}
    void sqrtAssign(Flags const& flags) {}
    // void squareAssign(Flags const& flags) {}
    void multAndAddAssign(SoftFloat const&, SoftFloat const&, Flags const& flags) {}
    void multAndSubAssign(SoftFloat const&, SoftFloat const&, Flags const& flags) {}
    // void multNegativeAndAddAssign(SoftFloat const&, SoftFloat const&, Flags const& flags) {}
    // void multNegativeAndSubAssign(SoftFloat const&, SoftFloat const&, Flags const& flags) {}
    void maxAssign(SoftFloat const&) {}
    void minAssign(SoftFloat const&) {}
    
    // BigInt querySBasicExponent() { return BigInt(); }
    // BigInt queryBasicExponent() { return BigInt(); }
    BOOL isZero() { return make_unknown<BOOL>(arch); }
    BOOL isNormalized() { return make_unknown<BOOL>(arch); }
    // void clear() {}
    
    // BOOL hasInftyExponent() { return make_unknown<BOOL>(arch); }
    // BOOL isDenormalized() { return make_unknown<BOOL>(arch); }

    Arch* arch;
  };

  struct SoftHalfFloat
  {
    enum __FromRawBits__ { FromRawBits };
    SoftHalfFloat( __FromRawBits__, U16 const& source ) : arch(ArchExprNode::SeekArch(source.expr)) {}
    SoftHalfFloat( SoftFloat const& source, Flags& flags ) : arch(source.arch) { flags.touch( source.arch ); }
    U16 queryRawBits() { return make_unknown<U16>(arch); }

    Arch* arch;
  };

  SoftFloat::SoftFloat( SoftHalfFloat const& source, Flags& flags )
    : arch(source.arch)
  { flags.touch(source.arch); }
  
  // static const unsigned int RN_NEAREST = 0;
  // static const unsigned int RN_ZERO = 1;
  // static const unsigned int RN_UP = 2;
  // static const unsigned int RN_DOWN = 3;

  // inline void GenSPEFSCR_FOVF(SPEFSCR& spefscr, const Flags& flags) { spefscr = make_unknown<U32>(arch); }
  // inline void GenSPEFSCR_FUNF(SPEFSCR& spefscr, const Flags& flags) { spefscr = make_unknown<U32>(arch); }
  // inline void GenSPEFSCR_FINXS(SPEFSCR& spefscr, const Flags& flags) { spefscr = make_unknown<U32>(arch); }
  // inline void GenSPEFSCR_FDBZ(SPEFSCR& spefscr, const Flags& flags) { spefscr = make_unknown<U32>(arch); }
  // inline void GenSPEFSCR_FG(SPEFSCR& spefscr, const Flags& flags, bool x=false) { spefscr = make_unknown<U32>(arch); }
  // inline void GenSPEFSCR_FX(SPEFSCR& spefscr, const Flags& flags, bool x=false) { spefscr = make_unknown<U32>(arch); }
  // inline void GenSPEFSCR_FG(SPEFSCR& spefscr, const SoftFloat& result) { spefscr = make_unknown<U32>(arch); }
  // inline BOOL HasSPEFSCR_InvalidInput(const SoftFloat& input) { return make_unknown<BOOL>(arch); }
  // template <class RESULT>
  // inline void GenSPEFSCR_DefaultResults(SPEFSCR& spefscr, RESULT& result) { spefscr = make_unknown<U32>(arch); }
  // template <class FLOAT>
  // inline void GenSPEFSCR_FINV(SPEFSCR& spefscr, FLOAT& first, FLOAT* second = 0, FLOAT* third = 0, bool x=false, bool y=false) { spefscr = make_unknown<U32>(arch); }

  // inline BOOL DoesSPEFSCR_TriggerException(const SPEFSCR& spefscr) { return BOOL(false); }
  
  // inline void GenSPEFSCR_TriggerException(Arch* cpu) { cpu->donttest_system(); }
  
  // extern void SignedAdd32(U32& result, U8& carry_out, U8& overflow, U8& sign, U32 x, U32 y, U8 carry_in);
  // extern inline void SignedAdd32(U32& result, U8& carry_out, U8& overflow, U8& sign, U32 x, U32 y, int carry_in)
  // { return SignedAdd32(result, carry_out, overflow, sign, x, y, U8(carry_in)); }

  // struct MaskNode : public ArchExprNode
  // {
  //   typedef unisim::util::symbolic::Expr Expr;
    
  //   MaskNode( Expr const& _mb, Expr const& _me ) : mb(_mb), me(_me) {}
  //   virtual void Repr( std::ostream& sink ) const;
  //   virtual unsigned SubCount() const { return 2; };
  //   virtual Expr const& GetSub(unsigned idx) const { switch (idx) { case 0: return mb; case 1: return me; } return ExprNode::GetSub(idx); };
  //   virtual intptr_t cmp( ExprNode const& brhs ) const
  //   {
  //     MaskNode const& rhs = dynamic_cast<MaskNode const&>( brhs );
  //     if (intptr_t delta = mb.cmp( rhs.mb )) return delta;
  //     return me.cmp( rhs.me );
  //   }
  //   Expr mb, me;
  // };
  
  // extern U32 Mask(U32 mb, U32 me);
  
}

#endif // ARCH_HH
