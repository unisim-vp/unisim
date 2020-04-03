/*
 *  Copyright (c) 2009-2020,
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

#include <unisim/component/cxx/processor/intel/isa/intel.hh>
#include <unisim/component/cxx/processor/intel/modrm.hh>
#include <unisim/component/cxx/processor/intel/disasm.hh>
#include <unisim/component/cxx/processor/intel/vectorbank.hh>
#include <unisim/component/cxx/processor/intel/types.hh>
#include <unisim/util/symbolic/binsec/binsec.hh>
#include <unisim/util/symbolic/symbolic.hh>

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <sstream>
#include <cstdlib>
#include <cstdio>

#define CP15ENCODE( CRN, OPC1, CRM, OPC2 ) ((OPC1 << 12) | (CRN << 8) | (CRM << 4) | (OPC2 << 0))

// template <typename ARCH, unsigned OPSIZE> struct TypeFor {};

// template <typename ARCH> struct TypeFor<ARCH, 8> { typedef typename ARCH:: S8 S; typedef typename ARCH:: U8 U; };
// template <typename ARCH> struct TypeFor<ARCH,16> { typedef typename ARCH::S16 S; typedef typename ARCH::U16 U; };
// template <typename ARCH> struct TypeFor<ARCH,32> { typedef typename ARCH::S32 S; typedef typename ARCH::U32 U; typedef typename ARCH::F32 F; };
// template <typename ARCH> struct TypeFor<ARCH,64> { typedef typename ARCH::S64 S; typedef typename ARCH::U64 U; typedef typename ARCH::F64 F; };

struct Processor
{
  //   =====================================================================
  //   =                           Configuration                           =
  //   =====================================================================
  struct Unimplemented {};
  struct Undefined {};
    
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
  typedef unisim::util::symbolic::ScalarType           ScalarType;
  
  typedef unisim::util::symbolic::binsec::ActionNode   ActionNode;
  typedef unisim::util::symbolic::binsec::Store        Store;
  typedef unisim::util::symbolic::binsec::Branch       Br;

  template <typename RID>
  struct RegRead : public unisim::util::symbolic::binsec::RegRead
  {
    typedef RegRead<RID> this_type;
    typedef unisim::util::symbolic::binsec::RegRead Super;
    RegRead( RID _id, ScalarType::id_t _tp ) : Super(), tp(_tp), id(_id) {}
    virtual this_type* Mutate() const { return new this_type( *this ); }
    virtual ScalarType::id_t GetType() const { return tp; }
    virtual void GetRegName( std::ostream& sink ) const { sink << id.c_str(); }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<RegRead const&>( rhs ) ); }
    int compare( RegRead const& rhs ) const { if (int delta = int(tp) - int(rhs.tp)) return delta; if (int delta = id.cmp( rhs.id )) return delta; return Super::compare(rhs); }

    ScalarType::id_t tp;
    RID id;
  };

  template <typename RID>
  static Expr newRegRead( RID id, ScalarType::id_t tp ) { return new RegRead<RID>( id, tp ); }

  struct ForeignRegister : public unisim::util::symbolic::binsec::RegRead
  {
    typedef unisim::util::symbolic::binsec::RegRead Super;
    ForeignRegister( uint8_t _mode, unsigned _idx )
      : Super(), idx(_idx), mode(_mode)
    {
      if (mode == SYSTEM_MODE) mode = USER_MODE;
    }
    virtual ForeignRegister* Mutate() const { return new ForeignRegister( *this ); }
    virtual ScalarType::id_t GetType() const { return ScalarType::U32; }
    
    char const* mode_ident() const
    {
      switch (mode)
        {
        case USER_MODE: return "usr";
        case FIQ_MODE: return "fiq";
        case IRQ_MODE: return "irq";
        case SUPERVISOR_MODE: return "svc";
        case MONITOR_MODE: return "mon";
        case ABORT_MODE: return "abt";
        case HYPERVISOR_MODE: return "hyp";
        case UNDEFINED_MODE: return "und";
        }
      throw 0;
      return "";
    }

    virtual void GetRegName( std::ostream& sink ) const
    {
      sink << (RegID("r0") + idx).c_str() << '_' << mode_ident();
    }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<ForeignRegister const&>( rhs ) ); }
    int compare( ForeignRegister const& rhs ) const
    {
      if (int delta = int(mode) - int(rhs.mode)) return delta;
      return idx - rhs.idx;
    }
    
    unsigned idx;
    uint8_t mode;
  };
  
  template <typename RID>
  struct RegWrite : public unisim::util::symbolic::binsec::RegWrite
  {
    typedef RegWrite<RID> this_type;
    typedef unisim::util::symbolic::binsec::RegWrite Super;
    RegWrite( RID _id, Expr const& _value ) : Super(_value), id(_id) {}
    virtual this_type* Mutate() const { return new this_type( *this ); }
    virtual ScalarType::id_t GetType() const { return ScalarType::VOID; }
    virtual void GetRegName( std::ostream& sink ) const { sink << id.c_str(); }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<RegWrite const&>( rhs ) ); }
    int compare( RegWrite const& rhs ) const { if (int delta = id.cmp( rhs.id )) return delta; return Super::compare( rhs ); }
    
    RID id;
  };

  template <typename RID>
  static Expr newRegWrite( RID id, Expr const& value ) { return new RegWrite<RID>( id, value ); }
  
  struct Goto : public Br
  {
    Goto( Expr const& value ) : Br( value ) {}
    virtual Goto* Mutate() const override { return new Goto( *this ); }
    virtual void GetRegName( std::ostream& sink ) const override { sink << "pc"; }
    virtual ScalarType::id_t GetType() const override { return ScalarType::VOID; }
    virtual void annotate(std::ostream& sink) const override { return; }
  };

  struct Call : public Goto
  {
    Call( Expr const& value, uint32_t ra ) : Goto( value ), return_address( ra ) {}
    virtual Call* Mutate() const override { return new Call( *this ); }
    virtual void annotate(std::ostream& sink) const override { sink << " // call (" << unisim::util::symbolic::binsec::dbx(4,return_address) << ",0)"; }

    uint32_t return_address;
  };

  struct ITCond {};
    
  struct Mode
  {
    Mode() {}
    bool     HasBR( unsigned index ) { return false; }
    bool     HasSPSR() { return false; }
    void     SetSPSR(U32 const& value) {}
    U32      GetSPSR() { throw Unimplemented(); return U32(); }
    void     Swap( Processor& ) {}
  };
    
  typedef std::map<std::pair<uint8_t,uint32_t>,Expr> ForeignRegisters;
  
  struct CP15Reg
  {
    virtual            ~CP15Reg() {}
    virtual unsigned    RequiredPL() { return 1; }
    virtual void        Write( Processor& proc, U32 const& value ) { throw Unimplemented(); }
    virtual U32         Read( Processor& proc ) { throw Unimplemented(); return U32(); }
    virtual char const* Describe() = 0;
  };

  struct Load : public unisim::util::symbolic::binsec::Load
  {
    Load( Expr const& addr, unsigned size, unsigned alignment, bool bigendian )
      : unisim::util::symbolic::binsec::Load(addr, size, alignment, bigendian)
    {}
  };
  //   =====================================================================
  //   =                      Construction/Destruction                     =
  //   =====================================================================

private:
  Processor( Processor const& );
public:
  
  Processor()
    : path(0)
    , reg_values()
    , next_insn_addr()
    , branch_type(B_JMP)
    , spsr( newRegRead(RegID("spsr"), ScalarType::U32) )
    , sregs()
    , FPSCR( newRegRead(RegID("fpscr"), ScalarType::U32) )
    , FPEXC( newRegRead(RegID("fpexc"), ScalarType::U32) )
    , stores()
    , unpredictable(false)
    , is_it_assigned(false)
    , mode()
    , foreign_registers()
    , neonregs()
  {
    // GPR regs
    for (unsigned reg = 0; reg < 15; ++reg)
      reg_values[reg] = U32( newRegRead( RegID("r0") + reg, ScalarType::U32 ) );
      
    // Special registers
    for (SRegID reg; reg.next();)
      sregs[reg.idx()] = U32( newRegRead( reg, ScalarType::U32 ) );

    for (unsigned reg = 0; reg < 32; ++reg)
      neonregs[reg][0] = new NeonRead( reg );
  }

  bool close( Processor const& ref, uint32_t linear_nia )
  {
    bool complete = path->close();
    if (branch_type == B_CALL)
      path->add_sink( new Call( next_insn_addr.expr, linear_nia ) );
    else
      path->add_sink( new Goto( next_insn_addr.expr ) );
    if (unpredictable)
      {
        path->add_sink( new unisim::util::symbolic::binsec::AssertFalse() );
        return complete;
      }
    if (spsr.expr != ref.spsr.expr)
      path->add_sink( newRegWrite( RegID("spsr"), spsr.expr ) );
    for (SRegID reg; reg.next();)
      if (sregs[reg.idx()].expr != ref.sregs[reg.idx()].expr)
        path->add_sink( newRegWrite( reg, sregs[reg.idx()].expr ) );
    if (FPSCR.expr != ref.FPSCR.expr)
      path->add_sink( newRegWrite( RegID("fpscr"), FPSCR.expr ) );
    if (FPEXC.expr != ref.FPEXC.expr)
      path->add_sink( newRegWrite( RegID("fpexc"), FPEXC.expr ) );
    for (unsigned reg = 0; reg < 15; ++reg)
      {
        if (reg_values[reg].expr != ref.reg_values[reg].expr)
          path->add_sink( newRegWrite( RegID("r0") + reg, reg_values[reg].expr ) );
      }
    for (ForeignRegisters::iterator itr = foreign_registers.begin(), end = foreign_registers.end(); itr != end; ++itr)
      {
        ForeignRegister ref(itr->first.first, itr->first.second);
        ref.Retain(); // Prevent deletion of this static instance
        Expr xref( new ForeignRegister(itr->first.first, itr->first.second) );
        if (itr->second == Expr(&ref)) continue;
        std::ostringstream buf;
        ref.Repr( buf );
        path->add_sink( newRegWrite( RegID(buf.str().c_str()), itr->second ) );
      }
    for (unsigned reg = 0; reg < 32; ++reg)
      {
        if (neonregs[reg][0] != ref.neonregs[reg][0])
          GetNeonSinks(reg);
      }
    for (std::set<Expr>::const_iterator itr = stores.begin(), end = stores.end(); itr != end; ++itr)
      path->add_sink( *itr );
    return complete;
  }
  
  //   =====================================================================
  //   =                 Internal Instruction Control Flow                 =
  //   =====================================================================
  
  void UnpredictableInsnBehaviour() { unpredictable = true; }
  
  template <typename OP>
  void UndefinedInstruction( OP* op ) { throw Undefined(); }
    
  bool concretize( Expr const& cexp )
  {
    bool predicate = path->proceed( cexp );
    path = path->next( predicate );
    return predicate;
  }
  
  template <typename T>
  bool Test( unisim::util::symbolic::SmartValue<T> const& cond )
  {
    if (not cond.expr.good())
      throw std::logic_error( "Not a valid condition" );

    Expr cexp( BOOL(cond).expr );
    if (unisim::util::symbolic::ConstNodeBase const* cnode = cexp.ConstSimplify())
      return cnode->Get( bool() );

    return concretize( cexp );
  }
  
  void FPTrap( unsigned exc )
  {
    throw Unimplemented();
  }
    
  //   =====================================================================
  //   =             General Purpose Registers access methods              =
  //   =====================================================================
    
  U32  GetGPR( uint32_t id ) { return reg_values[id]; }
  
  // TODO: interworking branches are not correctly handled
  void SetGPR_mem( uint32_t id, U32 const& value )
  {
    if (id != 15)
      reg_values[id] = value;
    else
      SetNIA( value, B_JMP );
  }
  void SetGPR( uint32_t id, U32 const& value ) {
    if (id != 15)
      reg_values[id] = value;
    else
      SetNIA( value, B_JMP );
  }
    
  void SetGPR_usr( uint32_t id, U32 const& value ) { /* system mode */ throw Unimplemented(); }
  U32  GetGPR_usr( uint32_t id ) { /* system mode */ throw Unimplemented(); return U32(); }
    
  U32  GetNIA() { return next_insn_addr; }
  enum branch_type_t { B_JMP = 0, B_CALL, B_RET, B_EXC, B_DBG, B_RFE };
  void SetNIA( U32 const& nia, branch_type_t bt )
  {
    branch_type = bt;
    next_insn_addr = nia;
  }

  //   =====================================================================
  //   =              Special/System Registers access methods              =
  //   =====================================================================

  U32& SPSR() { throw Unimplemented(); static U32 spsr_dummy; return spsr_dummy; }
  
  struct NeonName
  { // Convenience class for name construction
    NeonName( unsigned idx ) : begin(&buf[sizeof(buf)]) { _('\0'); do { _('0'+idx%10); idx /= 10; } while (idx); _('d'); }
    void _(char c) { *--begin = c; } operator char const* () const { return begin; } char buf[4]; char* begin;
  };
  
  struct NeonRead : public unisim::util::symbolic::binsec::RegRead
  {
    typedef NeonRead this_type;
    typedef unisim::util::symbolic::binsec::RegRead Super;
    NeonRead( unsigned _reg ) : Super(), reg(_reg) {}
    virtual this_type* Mutate() const { return new this_type( *this ); }
    virtual ScalarType::id_t GetType() const { return ScalarType::U64; }
    virtual void GetRegName( std::ostream& sink ) const { sink << 'd' << std::dec << reg; }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<NeonRead const&>( rhs ) ); }
    int compare( this_type const& rhs ) const { return int(reg) - int(rhs.reg); }
    
    unsigned reg;
  };

  struct NeonWrite : public unisim::util::symbolic::binsec::RegWrite
  {
    typedef NeonWrite this_type;
    typedef unisim::util::symbolic::binsec::RegWrite Super;
    NeonWrite( unsigned _reg, Expr const& value ) : Super(value), reg(_reg) {}
    virtual this_type* Mutate() const { return new this_type( *this ); }
    virtual ScalarType::id_t GetType() const { return ScalarType::VOID; }
    virtual void GetRegName( std::ostream& sink ) const { sink << 'd' << std::dec << reg; }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<this_type const&>( rhs ) ); }
    int compare( this_type const& rhs ) const { if (int delta = int(reg) - int(rhs.reg)) return delta; return Super::compare( rhs ); }
    
    unsigned reg;
  };

  struct NeonPartialWrite : public unisim::util::symbolic::binsec::RegWrite
  {
    typedef NeonPartialWrite this_type;
    typedef unisim::util::symbolic::binsec::RegWrite Super;
    typedef unisim::util::symbolic::binsec::Label Label;
    typedef unisim::util::symbolic::binsec::Variables Variables;
    typedef unisim::util::symbolic::binsec::GetCode GetCode;

    NeonPartialWrite( unsigned _reg, unsigned _beg, unsigned _end, Expr const& _value ) : Super(_value), reg(_reg), beg(_beg), end(_end) {}
    virtual this_type* Mutate() const { return new this_type( *this ); }
    virtual ScalarType::id_t GetType() const { return ScalarType::VOID; }
    virtual void GetRegName( std::ostream& sink ) const { sink << 'd' << std::dec << reg << '_' << beg << '_' << end; }
    virtual int GenCode( Label& label, Variables& vars, std::ostream& sink ) const
    {
      sink << 'd' << std::dec << reg << '{' << beg << ',' << end << '}' << " := " << GetCode(value, vars, label);
      return 0;
    }
    virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<this_type const&>( rhs ) ); }
    int compare( this_type const& rhs ) const
    {
      if (int delta = int(reg) - int(rhs.reg)) return delta;
      if (int delta = int(beg) - int(rhs.beg)) return delta;
      if (int delta = int(end) - int(rhs.end)) return delta;
      return Super::compare( rhs );
    }
    
    unsigned reg, beg, end;
  };
  
  static unsigned const NEONSIZE = 8;

  void
  GetNeonSinks( unsigned reg )
  {
    using unisim::util::symbolic::binsec::BitFilter;
    // using unisim::util::symbolic::make_const;
    
    { // Check for constant values
      Expr dr = unisim::util::symbolic::binsec::ASExprNode::Simplify( GetVDU(reg).expr );
      if (dr.ConstSimplify())
        {
          path->add_sink( new NeonWrite( reg, dr ) );
          return;
        }
    }

    // Check for monolithic value
    if (not neonregs[reg][NEONSIZE/2].node)
      {
        path->add_sink( new NeonWrite( reg, eneonread(reg,NEONSIZE,0) ) );
        return;
      }
    
    // Requested read is a concatenation of multiple source values
    struct _
    {
      _( Processor& _core, unsigned _reg ) : core(_core), reg(_reg) { Process( 0, NEONSIZE ); } Processor& core; unsigned reg;
      void Process( unsigned pos, unsigned size )
      {
        unsigned half = size / 2, mid = pos+half;
        if (size > 1 and core.neonregs[reg][mid].node)
          {
            Process( pos, half );
            Process( mid, half );
          }
        else
          {
            unsigned begin = pos*8, end = begin+size*8;
            Expr value( new BitFilter( core.eneonread(reg,size,pos), 64, size*8, size*8, false ) );
            core.path->add_sink( new NeonPartialWrite( reg, begin, end, value ) );
          }
      }
    } concat( *this, reg );
  }
 
  Expr eneonread( unsigned reg, unsigned size, unsigned pos )
  {
    using unisim::util::symbolic::ExprNode;
    using unisim::util::symbolic::make_const;
    
    struct
    {
      Expr ui( unsigned sz, Expr const& src ) const
      {
        switch (sz) {
        default: throw 0;
        case 1: return new unisim::util::symbolic::CastNode<uint8_t,uint64_t>( src );
        case 2: return new unisim::util::symbolic::CastNode<uint16_t,uint64_t>( src );
        case 4: return new unisim::util::symbolic::CastNode<uint32_t,uint64_t>( src );
        case 8: return new unisim::util::symbolic::CastNode<uint64_t,uint64_t>( src );
        }
        return 0;
      }
    } cast;
    
    if (not neonregs[reg][pos].node)
      {
        // requested read is in the middle of a larger value
        unsigned src = pos;
        do { src = src & (src-1); } while (not neonregs[reg][src].node);
        unsigned shift = 8*(pos - src);
        return cast.ui( size, make_operation( "Lsr", neonregs[reg][src], make_const( shift ) ) );
      }
    else if (not neonregs[reg][(pos|size)&(NEONSIZE-1)].node)
      {
        // requested read is in lower bits of a larger value
        return cast.ui( size, neonregs[reg][pos] );
      }
    else if ((size > 1) and (neonregs[reg][pos|(size >> 1)].node))
      {
        // requested read is a concatenation of multiple source values
        Expr concat = cast.ui( size, neonregs[reg][pos] );
        for (unsigned idx = 0; ++idx < size;)
          {
            if (not neonregs[reg][pos+idx].node)
              continue;
            concat = make_operation( "Or", make_operation( "Lsl", cast.ui( size, neonregs[reg][idx] ), make_const( 8*idx ) ), concat );
          }
        return concat;
      }
    
    // requested read is directly available
    return neonregs[reg][pos];
  }
  
  void eneonwrite( unsigned reg, unsigned size, unsigned pos, Expr const& xpr )
  {
    Expr nxt[NEONSIZE];
    
    for (unsigned ipos = pos, isize = size, cpos;
         cpos = (ipos^isize) & (NEONSIZE-1), (not neonregs[reg][ipos].node) or (not neonregs[reg][cpos].node);
         isize *= 2, ipos &= -isize
         )
      {
        nxt[cpos] = eneonread( reg, isize, cpos );
      }
    
    for (unsigned ipos = 0; ipos < NEONSIZE; ++ipos)
      {
        if (nxt[ipos].node)
          neonregs[reg][ipos] = nxt[ipos];
      }
    
    neonregs[reg][pos] = xpr;
    
    for (unsigned rem = 1; rem < size; ++rem)
      {
        neonregs[reg][pos+rem] = 0;
      }
  }

  U32  GetVSU( unsigned idx ) { return U32( U64( eneonread( idx / 2, 4, (idx*4) & 4 ) ) ); }
  void SetVSU( unsigned idx, U32 val ) { eneonwrite( idx / 2, 4, (idx*4) & 4, U64(val).expr ); }
  U64  GetVDU( unsigned idx ) { return U64( eneonread( idx, 8, 0 ) ); }
  void SetVDU( unsigned idx, U64 val ) { eneonwrite( idx, 8, 0, U64(val).expr ); }
  F32  GetVSR( unsigned idx ) { return F32(); }
  void SetVSR( unsigned idx, F32 val ) {}
  F64  GetVDR( unsigned idx ) { return F64(); }
  void SetVDR( unsigned idx, F64 val ) {}

  static unsigned usizeof( U8 const& )  { return  1; }
  static unsigned usizeof( U16 const& ) { return  2; }
  static unsigned usizeof( U32 const& ) { return  4; }
  static unsigned usizeof( U64 const& ) { return  8; }

  template <typename T> T ucast( T const& x ) { return x; }
  U8 ucast( S8 const& x ) { return U8(x); }
  U16 ucast( S16 const& x ) { return U16(x); }
  U32 ucast( S32 const& x ) { return U32(x); }
  U64 ucast( S64 const& x ) { return U64(x); }
  // Get|Set elements
  template <class ELEMT>
  void
  SetVDE( unsigned reg, unsigned idx, ELEMT const& value )
  {
    using unisim::util::symbolic::binsec::BitFilter;
    auto uvalue = ucast( value );
    unsigned usz = usizeof( uvalue );
    Expr neonval( new BitFilter( uvalue.expr, usz*8, usz*8, 64, false ) );
    eneonwrite( reg, usz, usz*idx, neonval );
  }

  template <class ELEMT>
  ELEMT GetVDE( unsigned reg, unsigned idx, ELEMT const& trait )
  {
    unsigned usz = usizeof( ucast(trait) );
    return ELEMT( U64( eneonread( reg, usz, usz*idx ) ) );
  }
  
  //   =====================================================================
  //   =                      Control Transfer methods                     =
  //   =====================================================================

  void BranchExchange( U32 const& target, branch_type_t branch_type ) { SetNIA( target, branch_type ); }
  void Branch( U32 const& target, branch_type_t branch_type ) { SetNIA( target, branch_type ); }
    
  void WaitForInterrupt() { throw Unimplemented(); }
  void SWI( uint32_t imm ) { throw Unimplemented(); }
  void BKPT( uint32_t imm ) { throw Unimplemented(); }
  void CallSupervisor( uint32_t imm ) { throw Unimplemented(); }
  bool IntegerZeroDivide( BOOL const& condition ) { return false; }
  
  //   =====================================================================
  //   =                       Memory access methods                       =
  //   =====================================================================
  
  U32  MemURead32( U32 const& addr ) { return U32( Expr( new Load( addr.expr, 2, 0, false ) ) ); }
  U16  MemURead16( U32 const& addr ) { return U16( Expr( new Load( addr.expr, 1, 0, false ) ) ); }
  U32  MemRead32( U32 const& addr ) { return U32( Expr( new Load( addr.expr, 2, 2, false ) ) ); }
  U16  MemRead16( U32 const& addr ) { return U16( Expr( new Load( addr.expr, 1, 1, false ) ) ); }
  U8   MemRead8( U32 const& addr ) { return U8( Expr( new Load( addr.expr, 0, 0, false ) ) ); }
  
  void MemUWrite32( U32 const& addr, U32 const& value ) { stores.insert( new Store( addr.expr, value.expr, 2, 0, false ) ); }
  void MemUWrite16( U32 const& addr, U16 const& value ) { stores.insert( new Store( addr.expr, value.expr, 1, 0, false ) ); }
  void MemWrite32( U32 const& addr, U32 const& value ) { stores.insert( new Store( addr.expr, value.expr, 2, 2, false ) ); }
  void MemWrite16( U32 const& addr, U16 const& value ) { stores.insert( new Store( addr.expr, value.expr, 1, 1, false ) ); }
  void MemWrite8( U32 const& addr, U8 const& value ) { stores.insert( new Store( addr.expr, value.expr, 0, 0, false ) ); }
    
  void SetExclusiveMonitors( U32 const& address, unsigned size ) { std::cerr << "SetExclusiveMonitors\n"; }
  bool ExclusiveMonitorsPass( U32 const& address, unsigned size ) { std::cerr << "ExclusiveMonitorsPass\n"; return true; }
  void ClearExclusiveLocal() { std::cerr << "ClearExclusiveMonitors\n"; }
  
  //   =====================================================================
  //   =                         Processor Storage                         =
  //   =====================================================================
  
  static const unsigned PC_reg = 15;
  static const unsigned LR_reg = 14;
  static const unsigned SP_reg = 13;

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

  /* values of the different condition codes */
  static uint32_t const COND_EQ = 0x00;
  static uint32_t const COND_NE = 0x01;
  static uint32_t const COND_CS_HS = 0x02;
  static uint32_t const COND_CC_LO = 0x03;
  static uint32_t const COND_MI = 0x04;
  static uint32_t const COND_PL = 0x05;
  static uint32_t const COND_VS = 0x06;
  static uint32_t const COND_VC = 0x07;
  static uint32_t const COND_HI = 0x08;
  static uint32_t const COND_LS = 0x09;
  static uint32_t const COND_GE = 0x0a;
  static uint32_t const COND_LT = 0x0b;
  static uint32_t const COND_GT = 0x0c;
  static uint32_t const COND_LE = 0x0d;
  static uint32_t const COND_AL = 0x0e;
    
  /* mask for valid bits in processor control and status registers */
  static uint32_t const PSR_UNALLOC_MASK = 0x00f00000;

  struct SRegID : public unisim::util::identifier::Identifier<SRegID>
  {
    enum Code {
      SCTLR, ACTLR,
      CTR, MPIDR,
      ID_PFR0, CCSIDR, CLIDR, CSSELR,
      CPACR, NSACR,
      TTBR0, TTBR1, TTBCR,
      DACR,
      DFSR, IFSR, DFAR, IFAR,
      ICIALLUIS, BPIALLIS,
      ICIALLU, ICIMVAU, BPIALL,
      DCIMVAC, DCISW, DCCMVAC, DCCSW, DCCMVAU, DCCIMVAC,
      TLBIALLIS, TLBIALL, TLBIASID,
      VBAR,
      CONTEXTIDR,
      DIAGCR, CFGBAR, end
    } code;

    char const* c_str() const
    {
      switch (code)
        {
        case      SCTLR: return "sctlr";
        case      ACTLR: return "actlr";
        case        CTR: return "ctr";
        case      MPIDR: return "mpidr";
        case    ID_PFR0: return "id_pfr0";
        case     CCSIDR: return "ccsidr";
        case      CLIDR: return "clidr";
        case     CSSELR: return "csselr";
        case      CPACR: return "cpacr";
        case      NSACR: return "nsacr";
        case      TTBR0: return "ttbr0";
        case      TTBR1: return "ttbr1";
        case      TTBCR: return "ttbcr";
        case       DACR: return "dacr";
        case       DFSR: return "dfsr";
        case       IFSR: return "ifsr";
        case       DFAR: return "dfar";
        case       IFAR: return "ifar";
        case  ICIALLUIS: return "icialluis";
        case   BPIALLIS: return "bpiallis";
        case    ICIALLU: return "iciallu";
        case    ICIMVAU: return "icimvau";
        case     BPIALL: return "bpiall";
        case    DCIMVAC: return "dcimvac";
        case      DCISW: return "dcisw";
        case    DCCMVAC: return "dccmvac";
        case      DCCSW: return "dccsw";
        case    DCCMVAU: return "dccmvau";
        case   DCCIMVAC: return "dccimvac";
        case  TLBIALLIS: return "tlbiallis";
        case    TLBIALL: return "tlbiall";
        case   TLBIASID: return "tlbiasid";
        case       VBAR: return "vbar";
        case CONTEXTIDR: return "contextidr";
        case     DIAGCR: return "diagcr";
        case     CFGBAR: return "cfgbar";
        case end:        break;
        }
      return "NA";
    }

    SRegID() : code(end) {}
    SRegID( Code _code ) : code(_code) {}
    SRegID( char const* _code ) : code(end) { init(_code); }
  };
  
  U32& SReg( SRegID reg )
  {
    if (reg.code == SRegID::end)
      throw 0;
    return sregs[reg.idx()];
  }
    
  struct RegID : public unisim::util::identifier::Identifier<RegID>
  {
    enum Code
      {
        NA = 0,
        r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, sp, lr,
        n, z, c, v, itstate, // q, ge0, ge1, ge2, ge3,
        cpsr, spsr,
        fpscr, fpexc,
        r8_fiq,
        r9_fiq,
        sl_fiq,
        fp_fiq,
        ip_fiq,
        sp_fiq,
        lr_fiq,
        r8_usr,
        r9_usr,
        sl_usr,
        fp_usr,
        ip_usr,
        sp_usr,
        lr_usr,
        sp_irq,
        sp_svc,
        sp_abt,
        sp_hyp,
        sp_und,
        lr_irq,
        lr_svc,
        lr_abt,
        lr_hyp,
        lr_und,
        end
      } code;

    char const* c_str() const
    {
      switch (code)
        {
        case         r0: return "r0";
        case         r1: return "r1";
        case         r2: return "r2";
        case         r3: return "r3";
        case         r4: return "r4";
        case         r5: return "r5";
        case         r6: return "r6";
        case         r7: return "r7";
        case         r8: return "r8";
        case         r9: return "r9";
        case         sl: return "sl";
        case         fp: return "fp";
        case         ip: return "ip";
        case         sp: return "sp";
        case         lr: return "lr";
        case          n: return "n";
        case          z: return "z";
        case          c: return "c";
        case          v: return "v";
        case    itstate: return "itstate";
        case       cpsr: return "cpsr";
        case       spsr: return "spsr";
        case      fpscr: return "fpscr";
        case      fpexc: return "fpexc";
        case     r8_fiq: return "r8_fiq";
        case     r9_fiq: return "r9_fiq";
        case     sl_fiq: return "sl_fiq";
        case     fp_fiq: return "fp_fiq";
        case     ip_fiq: return "ip_fiq";
        case     sp_fiq: return "sp_fiq";
        case     lr_fiq: return "lr_fiq";
        case     r8_usr: return "r8_usr";
        case     r9_usr: return "r9_usr";
        case     sl_usr: return "sl_usr";
        case     fp_usr: return "fp_usr";
        case     ip_usr: return "ip_usr";
        case     sp_usr: return "sp_usr";
        case     lr_usr: return "lr_usr";
        case     sp_irq: return "sp_irq";
        case     sp_svc: return "sp_svc";
        case     sp_abt: return "sp_abt";
        case     sp_hyp: return "sp_hyp";
        case     sp_und: return "sp_und";
        case     lr_irq: return "lr_irq";
        case     lr_svc: return "lr_svc";
        case     lr_abt: return "lr_abt";
        case     lr_hyp: return "lr_hyp";
        case     lr_und: return "lr_und";
        case         NA:
        case        end: break;
        }
      return "NA";
    }
      
    RegID() : code(end) {}
    RegID( Code _code ) : code(_code) {}
    RegID( char const* _code ) : code(end) { init( _code ); }
  };

  ActionNode*      path;
  U32              reg_values[16];
  U32              next_insn_addr;
  branch_type_t    branch_type;
  U32              spsr;
  U32              sregs[SRegID::end];
  U32              FPSCR, FPEXC;
  std::set<Expr>   stores;
  bool             unpredictable;
  bool             is_it_assigned; /* determines wether current instruction is an IT one. */
  Mode             mode;
  ForeignRegisters foreign_registers;
  Expr             neonregs[32][NEONSIZE];
};

struct INTELISA
{
};

struct InstructionAddress : public unisim::util::symbolic::binsec::ASExprNode
{
  InstructionAddress() {}
  virtual void Repr( std::ostream& sink ) const { sink << "insn_addr"; }
  virtual int cmp( unisim::util::symbolic::ExprNode const& rhs ) const override { return compare( dynamic_cast<InstructionAddress const&>( rhs ) ); }
  int compare( InstructionAddress const& rhs ) const { return 0; }
};

struct Translator
{
  typedef unisim::util::symbolic::binsec::ActionNode ActionNode;
  
  Translator( uint32_t _addr, uint32_t _code )
    : addr(_addr), code(_code), coderoot(new ActionNode)
  {}
  ~Translator() { delete coderoot; }
  
  void
  extract( std::ostream& sink )
  {
    typedef unisim::component::cxx::processor::intel::Operation<Processor> Operation;
    typedef unisim::component::cxx::processor::intel::InputCode<Processor> InputCode;
    
    sink << "(address . " << unisim::util::symbolic::binsec::dbx(4, addr) << ")\n";
  
    // Instruction decoding
    struct Instruction
    {
      Instruction(uint32_t addr, uint32_t code)
        : operation(0), bytecount(0)
      {
        operation = getoperation( InputCode(mode, bytes, Arch::OpHeader(address) ) );
        
        // std::cerr << "No decoding for " << std::hex << address << ": " << unisim::component::cxx::processor::intel::DisasmBytes( bytes, 15 ) << std::dec << std::endl;
        
        unsigned bitlength = operation->GetLength(); 
        if ((bitlength != 32) and ((bitlength != 16) or not ISA::is_intel))
          { delete operation; operation = 0; }
        bytecount = bitlength/8;
      }
      ~Instruction() { delete operation; }
      Operation* operator -> () { return operation; }
      
      Operation* operation;
      unsigned   bytecount;
    };
    
    Instruction instruction( isa, addr, code );
    
    {
      uint32_t encoding = instruction->GetEncoding();
      if (instruction.bytecount == 2)
        encoding &= 0xffff;
      
      sink << "(opcode . " << unisim::util::symbolic::binsec::dbx(instruction.bytecount, encoding) << ")\n(size . " << (instruction.bytecount) << ")\n";
    }
    
    Processor::U32      insn_addr = unisim::util::symbolic::make_const(addr); //< concrete instruction address
    // Processor::U32      insn_addr = Expr(new InstructionAddress()); //< symbolic instruction address
    Processor reference;
    
    // Disassemble
    sink << "(mnemonic . \"";
    try { instruction->disasm( reference, sink ); }
    catch (...) { sink << "(bad)"; }
    sink << "\")\n";
    
    // Get actions
    for (bool end = false; not end;)
      {
        Processor state;
        state.path = coderoot;
        // Fetch
        uint32_t insn_addr = instruction->GetAddr(), nia = insn_addr + instruction.bytecount;
        state.SetNIA( Processor::U32(nia), Processor::B_JMP );
        state.reg_values[15] = Processor::U32(insn_addr + 4);
        // Execute
        instruction->execute( state );
        end = state.close( reference, nia );
      }
    coderoot->simplify();
    coderoot->commit_stats();
  }

  void translate( std::ostream& sink )
  {
    try
      {
        INTELISA intelisa;
        extract( sink, intelisa );
      }
    catch (Processor::Undefined const&)
      {
        sink << "(undefined)\n";
        return;
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
      sink << "(" << unisim::util::symbolic::binsec::dbx(4, addr) << ',' << itr->first << ") " << itr->second << std::endl;
  }

  uint32_t    addr, code;
  ActionNode* coderoot;
};
  
uint32_t getu32( uint32_t& res, char const* arg )
{
  char *end;
  uint32_t tmp = strtoul( arg, &end, 0 );
  if ((*arg == '\0') or (*end != '\0'))
    return false;
  res = tmp;
  return true;
}

char const* usage()
{
  return
    "usage: <program> arm|intel <address> <encoding>\n";
}

int
main( int argc, char** argv )
{
  if (argc != 4)
    {
      std::cerr << "Wrong number of CLI arguments.\n" << usage();
      return 1;
    }

  uint32_t addr, code;

  if (not getu32(addr, argv[2]) or not getu32(code, argv[3]))
    {
      std::cerr << "<addr> and <code> should be 32bits numeric values.\n" << usage();
      return 1;
    }

  Translator actset( addr, code );
  actset.translate( std::cout );
  
  return 0;
}

