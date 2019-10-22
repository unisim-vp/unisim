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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr), Gilles Mouchard <gilles.mouchard@cea.fr>
 */

#include "bindings.hh"

#include <unisim/component/cxx/processor/arm/cpu.tcc>
#include <unisim/component/cxx/processor/arm/cpu.hh>
#include <unisim/component/cxx/processor/arm/disasm.hh>
#include <unisim/component/cxx/processor/arm/psr.hh>
#include <top_arm32.tcc>
#include <top_thumb.tcc>

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <cassert>

#define CP15ENCODE( CRN, OPC1, CRM, OPC2 ) ((OPC1 << 12) | (CRN << 8) | (CRM << 4) | (OPC2 << 0))

// template <typename ARCH, unsigned OPSIZE> struct TypeFor {};

// template <typename ARCH> struct TypeFor<ARCH, 8> { typedef typename ARCH:: S8 S; typedef typename ARCH:: U8 U; };
// template <typename ARCH> struct TypeFor<ARCH,16> { typedef typename ARCH::S16 S; typedef typename ARCH::U16 U; };
// template <typename ARCH> struct TypeFor<ARCH,32> { typedef typename ARCH::S32 S; typedef typename ARCH::U32 U; typedef typename ARCH::F32 F; };
// template <typename ARCH> struct TypeFor<ARCH,64> { typedef typename ARCH::S64 S; typedef typename ARCH::U64 U; typedef typename ARCH::F64 F; };

struct ARMv7cfg
{
  // Following a standard armv7-a configuration
  static uint32_t const model = unisim::component::cxx::processor::arm::ARMEMU;
  static bool const     insns4T = true;
  static bool const     insns5E = true;
  static bool const     insns5J = true;
  static bool const     insns5T = true;
  static bool const     insns6  = true;
  static bool const     insnsRM = false;
  static bool const     insnsT2 = true;
  static bool const     insns7  = true;
};

struct Processor
  : public unisim::component::cxx::processor::arm::CPU<ARMv7cfg>
{
  //   =====================================================================
  //   =                           Configuration                           =
  //   =====================================================================
  typedef unisim::component::cxx::processor::arm::CPU<ARMv7cfg> CPU;
    
  //   =====================================================================
  //   =                             Data Types                            =
  //   =====================================================================
  typedef double   F64;
  typedef float    F32;
  typedef bool     BOOL;
  typedef uint8_t  U8;
  typedef uint16_t U16;
  typedef uint32_t U32;
  typedef uint64_t U64;
  typedef int8_t   S8;
  typedef int16_t  S16;
  typedef int32_t  S32;
  typedef int64_t  S64;
    
  // struct CP15Reg
  // {
  //   virtual            ~CP15Reg() {}
  //   virtual unsigned    RequiredPL() { return 1; }
  //   virtual void        Write( Processor& proc, U32 const& value ) { proc.not_implemented(); }
  //   virtual U32         Read( Processor& proc ) { proc.not_implemented(); return U32(); }
  //   virtual char const* Describe() = 0;
  // };

  //   =====================================================================
  //   =                      Construction/Destruction                     =
  //   =====================================================================
  

  Processor( char const* name, unisim::kernel::Object* parent, bool is_thumb )
    : unisim::kernel::Object(name, parent)
    , CPU(name, parent)
    // : path(0)
    // , reg_values()
    // , next_insn_addr()
    // , branch_type()
    // , cpsr( *this, ref_psr )
    // , spsr( newRegRead(*this, RegID("spsr"), ScalarType::U32) )
    // , sregs()
    // , FPSCR( newRegRead(*this, RegID("fpscr"), ScalarType::U32) )
    // , FPEXC( newRegRead(*this, RegID("fpexc"), ScalarType::U32) )
    // , stores()
    // , unpredictable(false)
    // , is_it_assigned(false)
    // , mode()
    // , foreign_registers()
    // , neonregs()
  {
    // // GPR regs
    // for (unsigned reg = 0; reg < 15; ++reg)
    //   reg_values[reg] = 0;
      
    // // Special registers
    // for (SRegID reg; reg.next();)
    //   sregs[reg.idx()] = 0;

    // for (unsigned reg = 0; reg < 32; ++reg)
    //   neonregs[reg][0] = 0;
  }

  // //   =====================================================================
  // //   =                 Internal Instruction Control Flow                 =
  // //   =====================================================================
  
  // void UnpredictableInsnBehaviour() { unpredictable = true; }
  
  // template <typename OP>
  // void UndefinedInstruction( OP* op ) { not_implemented(); }
    
  // bool Choose( bool cond ) { return cond; }
  // {
  //   if (not cond.expr.good())
  //     throw std::logic_error( "Not a valid condition" );

  //   Expr cexp( BOOL(cond).expr );
  //   if (unisim::util::symbolic::ConstNodeBase const* cnode = cexp.ConstSimplify())
  //     return cnode->Get( bool() );

  //   return Choose( cexp );
  // }
  
  // void FPTrap( unsigned exc )
  // {
  //   throw std::logic_error("unimplemented");
  // }
    
  // void not_implemented() { throw std::logic_error( "not implemented" ); }

  // //   =====================================================================
  // //   =             General Purpose Registers access methods              =
  // //   =====================================================================
    
  // U32  GetGPR( uint32_t id ) { return reg_values[id]; }
  
  // // TODO: interworking branches are not correctly handled
  // void SetGPR_mem( uint32_t id, U32 const& value )
  // {
  //   if (id != 15)
  //     reg_values[id] = value;
  //   else
  //     SetNIA( value, B_JMP );
  // }
  // void SetGPR( uint32_t id, U32 const& value ) {
  //   if (id != 15)
  //     reg_values[id] = value;
  //   else
  //     SetNIA( value, B_JMP );
  // }
    
  // void SetGPR_usr( uint32_t id, U32 const& value ) { /* Only work in system mode instruction */ not_implemented(); }
  // U32  GetGPR_usr( uint32_t id ) { /* Only work in system mode instruction */ not_implemented(); return U32(); }
    
  // U32  GetNIA() { return next_insn_addr; }
  // enum branch_type_t { B_JMP = 0, B_CALL, B_RET, B_EXC, B_DBG, B_RFE };
  // void SetNIA( U32 const& nia, branch_type_t bt )
  // {
  //   next_insn_addr = nia;
  //   branch_type = (bt == B_CALL) ? Br::Call : (bt == B_RET) ? Br::Return : Br::Jump;
  // }

  // Expr& GetForeignRegister( uint8_t foreign_mode, uint32_t idx )
  // {
  //   Expr& result = foreign_registers[std::make_pair( foreign_mode, idx )];
  //   if (not result.node)
  //     result = new ForeignRegister( foreign_mode, idx );
  //   return result;
  // }
    
  // U32  GetBankedRegister( uint8_t foreign_mode, uint32_t idx )
  // {
  //   if ((cpsr.mode == foreign_mode) or
  //       (idx < 8) or
  //       (idx >= 15) or
  //       ((foreign_mode != FIQ_MODE) and (cpsr.mode != FIQ_MODE) and (idx < 13))
  //       )
  //     return GetGPR( idx );
  //   return U32( GetForeignRegister( foreign_mode, idx ) );
  // }
    
  // void SetBankedRegister( uint8_t foreign_mode, uint32_t idx, U32 value )
  // {
  //   if ((cpsr.mode == foreign_mode) or
  //       (idx < 8) or
  //       (idx >= 15) or
  //       ((foreign_mode != FIQ_MODE) and (cpsr.mode != FIQ_MODE) and (idx < 13))
  //       )
  //     return SetGPR( idx, value );
  //   GetForeignRegister( foreign_mode, idx ) = value.expr;
  // }
    
  // //   =====================================================================
  // //   =              Special/System Registers access methods              =
  // //   =====================================================================

  // PSR& CPSR() { return cpsr; }
  
  // U32  GetCPSR()                                 { return cpsr.GetBits(); }
  // void SetCPSR( U32 const& bits, uint32_t mask ) { cpsr.SetBits( bits, mask ); }
    
  // U32& SPSR() { not_implemented(); static U32 spsr_dummy; return spsr_dummy; }
  
  // ITCond itcond() const { return ITCond(); }
  // bool itblock() { return Choose(cpsr.InITBlock()); }
  
  // void ITSetState( uint32_t cond, uint32_t mask )
  // {
  //   cpsr.SetITState( (cond << 4) | mask );
  //   is_it_assigned = true;
  // }
  
  // void ITAdvance()
  // {
  //   if (is_it_assigned)
  //     is_it_assigned = false;
  //   else if (itblock())
  //     {
  //       U8 itstate( cpsr.itstate );
  //       itstate = (Choose((itstate & U8(7)) != U8(0))) ? ((itstate & U8(-32)) | ((itstate << 1) & U8(31))) : U8(0);
  //       cpsr.itstate = itstate;
  //     }
  // }
  
  // Mode&  CurrentMode() { /* not_implemented(); */ return mode; }
  // Mode&  GetMode(uint8_t) { not_implemented(); return mode; }
  
  // virtual CP15Reg& CP15GetRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 );
  
  // U32         CP15ReadRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 )
  // { return CP15GetRegister( crn, opcode1, crm, opcode2 ).Read( *this ); }
  // void        CP15WriteRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2, U32 const& value )
  // { CP15GetRegister( crn, opcode1, crm, opcode2 ).Write( *this, value ); }
  // char const* CP15DescribeRegister( uint8_t crn, uint8_t opcode1, uint8_t crm, uint8_t opcode2 )
  // { return CP15GetRegister( crn, opcode1, crm, opcode2 ).Describe(); }

  // //   ====================================================================
  // //   =         Vector and Floating-point Registers access methods       =
  // //   ====================================================================

  // U32 RoundTowardsZeroFPSCR() const
  // {
  //   U32 fpscr = FPSCR;
  //   unisim::component::cxx::processor::arm::RMode.Set( fpscr, U32(unisim::component::cxx::processor::arm::RoundTowardsZero) );
  //   return fpscr;
  // }
    
  // U32 RoundToNearestFPSCR() const
  // {
  //   U32 fpscr = FPSCR;
  //   unisim::component::cxx::processor::arm::RMode.Set( fpscr, U32(unisim::component::cxx::processor::arm::RoundToNearest) );
  //   return fpscr;
  // }
    
  // // U32 StandardValuedFPSCR() const   { return AHP.Mask( FPSCR ) | 0x03000000; }
    
  // struct NeonName
  // { // Convenience class for name construction
  //   NeonName( unsigned idx ) : begin(&buf[sizeof(buf)]) { _('\0'); do { _('0'+idx%10); idx /= 10; } while (idx); _('d'); }
  //   void _(char c) { *--begin = c; } operator char const* () const { return begin; } char buf[4]; char* begin;
  // };
  // struct NeonRead : public unisim::util::symbolic::binsec::RegRead, Input
  // {
  //   typedef NeonRead this_type;
  //   typedef unisim::util::symbolic::binsec::RegRead Super;
  //   NeonRead( Processor& p, unsigned _reg ) : Super(), Input(p), reg(_reg) {}
  //   virtual this_type* Mutate() const { return new this_type( *this ); }
  //   virtual ScalarType::id_t GetType() const { return ScalarType::U64; }
  //   virtual void GetRegName( std::ostream& sink ) const { sink << 'd' << std::dec << reg; }
  //   virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<NeonRead const&>( rhs ) ); }
  //   int compare( this_type const& rhs ) const { return int(reg) - int(rhs.reg); }
    
  //   unsigned reg;
  // };

  // struct NeonWrite : public unisim::util::symbolic::binsec::RegWrite
  // {
  //   typedef NeonWrite this_type;
  //   typedef unisim::util::symbolic::binsec::RegWrite Super;
  //   NeonWrite( unsigned _reg, Expr const& value ) : Super(value), reg(_reg) {}
  //   virtual this_type* Mutate() const { return new this_type( *this ); }
  //   virtual ScalarType::id_t GetType() const { return ScalarType::VOID; }
  //   virtual void GetRegName( std::ostream& sink ) const { sink << 'd' << std::dec << reg; }
  //   virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<this_type const&>( rhs ) ); }
  //   int compare( this_type const& rhs ) const { if (int delta = int(reg) - int(rhs.reg)) return delta; return Super::compare( rhs ); }
    
  //   unsigned reg;
  // };

  // struct NeonPartialWrite : public unisim::util::symbolic::binsec::RegWrite
  // {
  //   typedef NeonPartialWrite this_type;
  //   typedef unisim::util::symbolic::binsec::RegWrite Super;
  //   typedef unisim::util::symbolic::binsec::Label Label;
  //   typedef unisim::util::symbolic::binsec::Variables Variables;
  //   typedef unisim::util::symbolic::binsec::GetCode GetCode;

  //   NeonPartialWrite( unsigned _reg, unsigned _beg, unsigned _end, Expr const& _value ) : Super(_value), reg(_reg), beg(_beg), end(_end) {}
  //   virtual this_type* Mutate() const { return new this_type( *this ); }
  //   virtual ScalarType::id_t GetType() const { return ScalarType::VOID; }
  //   virtual void GetRegName( std::ostream& sink ) const { sink << 'd' << std::dec << reg << '_' << beg << '_' << end; }
  //   virtual int GenCode( Label& label, Variables& vars, std::ostream& sink ) const
  //   {
  //     sink << 'd' << std::dec << reg << '{' << beg << ',' << end << '}' << " := " << GetCode(value, vars, label);
  //     return 0;
  //   }
  //   virtual int cmp( ExprNode const& rhs ) const override { return compare( dynamic_cast<this_type const&>( rhs ) ); }
  //   int compare( this_type const& rhs ) const
  //   {
  //     if (int delta = int(reg) - int(rhs.reg)) return delta;
  //     if (int delta = int(beg) - int(rhs.beg)) return delta;
  //     if (int delta = int(end) - int(rhs.end)) return delta;
  //     return Super::compare( rhs );
  //   }
    
  //   unsigned reg, beg, end;
  // };
  
  // static unsigned const NEONSIZE = 8;

  // void
  // GetNeonSinks( unsigned reg )
  // {
  //   using unisim::util::symbolic::binsec::BitFilter;
  //   // using unisim::util::symbolic::make_const;
    
  //   { // Check for constant values
  //     Expr dr = unisim::util::symbolic::binsec::ASExprNode::Simplify( GetVDU(reg).expr );
  //     if (dr.ConstSimplify())
  //       {
  //         path->add_sink( new NeonWrite( reg, dr ) );
  //         return;
  //       }
  //   }

  //   // Check for monolithic value
  //   if (not neonregs[reg][NEONSIZE/2].node)
  //     {
  //       path->add_sink( new NeonWrite( reg, eneonread(reg,NEONSIZE,0) ) );
  //       return;
  //     }
    
  //   // Requested read is a concatenation of multiple source values
  //   struct _
  //   {
  //     _( Processor& _core, unsigned _reg ) : core(_core), reg(_reg) { Process( 0, NEONSIZE ); } Processor& core; unsigned reg;
  //     void Process( unsigned pos, unsigned size )
  //     {
  //       unsigned half = size / 2, mid = pos+half;
  //       if (size > 1 and core.neonregs[reg][mid].node)
  //         {
  //           Process( pos, half );
  //           Process( mid, half );
  //         }
  //       else
  //         {
  //           unsigned begin = pos*8, end = begin+size*8;
  //           Expr value( new BitFilter( core.eneonread(reg,size,pos), 64, size*8, size*8, false ) );
  //           core.path->add_sink( new NeonPartialWrite( reg, begin, end, value ) );
  //         }
  //     }
  //   } concat( *this, reg );
  // }
 
  // Expr eneonread( unsigned reg, unsigned size, unsigned pos )
  // {
  //   using unisim::util::symbolic::ExprNode;
  //   using unisim::util::symbolic::make_const;
    
  //   struct
  //   {
  //     Expr ui( unsigned sz, Expr const& src ) const
  //     {
  //       switch (sz) {
  //       default: throw 0;
  //       case 1: return new unisim::util::symbolic::CastNode<uint8_t,uint64_t>( src );
  //       case 2: return new unisim::util::symbolic::CastNode<uint16_t,uint64_t>( src );
  //       case 4: return new unisim::util::symbolic::CastNode<uint32_t,uint64_t>( src );
  //       case 8: return new unisim::util::symbolic::CastNode<uint64_t,uint64_t>( src );
  //       }
  //       return 0;
  //     }
  //   } cast;
    
  //   if (not neonregs[reg][pos].node)
  //     {
  //       // requested read is in the middle of a larger value
  //       unsigned src = pos;
  //       do { src = src & (src-1); } while (not neonregs[reg][src].node);
  //       unsigned shift = 8*(pos - src);
  //       return cast.ui( size, make_operation( "Lsr", neonregs[reg][src], make_const( shift ) ) );
  //     }
  //   else if (not neonregs[reg][(pos|size)&(NEONSIZE-1)].node)
  //     {
  //       // requested read is in lower bits of a larger value
  //       return cast.ui( size, neonregs[reg][pos] );
  //     }
  //   else if ((size > 1) and (neonregs[reg][pos|(size >> 1)].node))
  //     {
  //       // requested read is a concatenation of multiple source values
  //       Expr concat = cast.ui( size, neonregs[reg][pos] );
  //       for (unsigned idx = 0; ++idx < size;)
  //         {
  //           if (not neonregs[reg][pos+idx].node)
  //             continue;
  //           concat = make_operation( "Or", make_operation( "Lsl", cast.ui( size, neonregs[reg][idx] ), make_const( 8*idx ) ), concat );
  //         }
  //       return concat;
  //     }
    
  //   // requested read is directly available
  //   return neonregs[reg][pos];
  // }
  
  // void eneonwrite( unsigned reg, unsigned size, unsigned pos, Expr const& xpr )
  // {
  //   Expr nxt[NEONSIZE];
    
  //   for (unsigned ipos = pos, isize = size, cpos;
  //        cpos = (ipos^isize) & (NEONSIZE-1), (not neonregs[reg][ipos].node) or (not neonregs[reg][cpos].node);
  //        isize *= 2, ipos &= -isize
  //        )
  //     {
  //       nxt[cpos] = eneonread( reg, isize, cpos );
  //     }
    
  //   for (unsigned ipos = 0; ipos < NEONSIZE; ++ipos)
  //     {
  //       if (nxt[ipos].node)
  //         neonregs[reg][ipos] = nxt[ipos];
  //     }
    
  //   neonregs[reg][pos] = xpr;
    
  //   for (unsigned rem = 1; rem < size; ++rem)
  //     {
  //       neonregs[reg][pos+rem] = 0;
  //     }
  // }

  // U32  GetVSU( unsigned idx ) { return U32( U64( eneonread( idx / 2, 4, (idx*4) & 4 ) ) ); }
  // void SetVSU( unsigned idx, U32 val ) { eneonwrite( idx / 2, 4, (idx*4) & 4, U64(val).expr ); }
  // U64  GetVDU( unsigned idx ) { return U64( eneonread( idx, 8, 0 ) ); }
  // void SetVDU( unsigned idx, U64 val ) { eneonwrite( idx, 8, 0, U64(val).expr ); }
  // F32  GetVSR( unsigned idx ) { return F32(); }
  // void SetVSR( unsigned idx, F32 val ) {}
  // F64  GetVDR( unsigned idx ) { return F64(); }
  // void SetVDR( unsigned idx, F64 val ) {}

  // static unsigned usizeof( U8 const& )  { return  1; }
  // static unsigned usizeof( U16 const& ) { return  2; }
  // static unsigned usizeof( U32 const& ) { return  4; }
  // static unsigned usizeof( U64 const& ) { return  8; }

  // template <typename T> T ucast( T const& x ) { return x; }
  // U8 ucast( S8 const& x ) { return U8(x); }
  // U16 ucast( S16 const& x ) { return U16(x); }
  // U32 ucast( S32 const& x ) { return U32(x); }
  // U64 ucast( S64 const& x ) { return U64(x); }
  // // Get|Set elements
  // template <class ELEMT>
  // void
  // SetVDE( unsigned reg, unsigned idx, ELEMT const& value )
  // {
  //   using unisim::util::symbolic::binsec::BitFilter;
  //   auto uvalue = ucast( value );
  //   unsigned usz = usizeof( uvalue );
  //   Expr neonval( new BitFilter( uvalue.expr, usz*8, usz*8, 64, false ) );
  //   eneonwrite( reg, usz, usz*idx, neonval );
  // }

  // template <class ELEMT>
  // ELEMT GetVDE( unsigned reg, unsigned idx, ELEMT const& trait )
  // {
  //   unsigned usz = usizeof( ucast(trait) );
  //   return ELEMT( U64( eneonread( reg, usz, usz*idx ) ) );
  // }
  
  // //   =====================================================================
  // //   =                      Control Transfer methods                     =
  // //   =====================================================================

  // void BranchExchange( U32 const& target, branch_type_t branch_type ) { SetNIA( target, branch_type ); }
  // void Branch( U32 const& target, branch_type_t branch_type ) { SetNIA( target, branch_type ); }
    
  // void WaitForInterrupt() { not_implemented(); }
  // void SWI( uint32_t imm ) { not_implemented(); }
  // void BKPT( uint32_t imm ) { not_implemented(); }
  // void CallSupervisor( uint16_t imm ) { not_implemented(); }
  // bool IntegerZeroDivide( BOOL const& condition ) { return false; }
  
  // //   =====================================================================
  // //   =                       Memory access methods                       =
  // //   =====================================================================
  
  // U32  MemURead32( U32 const& addr ) { return U32( Expr( new Load( *this, addr.expr, 2, 0, false ) ) ); }
  // U16  MemURead16( U32 const& addr ) { return U16( Expr( new Load( *this, addr.expr, 1, 0, false ) ) ); }
  // U32  MemRead32( U32 const& addr ) { return U32( Expr( new Load( *this, addr.expr, 2, 2, false ) ) ); }
  // U16  MemRead16( U32 const& addr ) { return U16( Expr( new Load( *this, addr.expr, 1, 1, false ) ) ); }
  // U8   MemRead8( U32 const& addr ) { return U8( Expr( new Load( *this, addr.expr, 0, 0, false ) ) ); }
  
  // void MemUWrite32( U32 const& addr, U32 const& value ) { stores.insert( new Store( addr.expr, value.expr, 2, 0, false ) ); }
  // void MemUWrite16( U32 const& addr, U16 const& value ) { stores.insert( new Store( addr.expr, value.expr, 1, 0, false ) ); }
  // void MemWrite32( U32 const& addr, U32 const& value ) { stores.insert( new Store( addr.expr, value.expr, 2, 2, false ) ); }
  // void MemWrite16( U32 const& addr, U16 const& value ) { stores.insert( new Store( addr.expr, value.expr, 1, 1, false ) ); }
  // void MemWrite8( U32 const& addr, U8 const& value ) { stores.insert( new Store( addr.expr, value.expr, 0, 0, false ) ); }
    
  // void SetExclusiveMonitors( U32 const& address, unsigned size ) { std::cerr << "SetExclusiveMonitors\n"; }
  // bool ExclusiveMonitorsPass( U32 const& address, unsigned size ) { std::cerr << "ExclusiveMonitorsPass\n"; return true; }
  // void ClearExclusiveLocal() { std::cerr << "ClearExclusiveMonitors\n"; }
  
  // //   =====================================================================
  // //   =                         Processor Storage                         =
  // //   =====================================================================
  
  // static const unsigned PC_reg = 15;
  // static const unsigned LR_reg = 14;
  // static const unsigned SP_reg = 13;

  // /* masks for the different running modes */
  // static uint32_t const USER_MODE = 0b10000;
  // static uint32_t const FIQ_MODE = 0b10001;
  // static uint32_t const IRQ_MODE = 0b10010;
  // static uint32_t const SUPERVISOR_MODE = 0b10011;
  // static uint32_t const MONITOR_MODE = 0b10110;
  // static uint32_t const ABORT_MODE = 0b10111;
  // static uint32_t const HYPERVISOR_MODE = 0b11010;
  // static uint32_t const UNDEFINED_MODE = 0b11011;
  // static uint32_t const SYSTEM_MODE = 0b11111;

  // /* values of the different condition codes */
  // static uint32_t const COND_EQ = 0x00;
  // static uint32_t const COND_NE = 0x01;
  // static uint32_t const COND_CS_HS = 0x02;
  // static uint32_t const COND_CC_LO = 0x03;
  // static uint32_t const COND_MI = 0x04;
  // static uint32_t const COND_PL = 0x05;
  // static uint32_t const COND_VS = 0x06;
  // static uint32_t const COND_VC = 0x07;
  // static uint32_t const COND_HI = 0x08;
  // static uint32_t const COND_LS = 0x09;
  // static uint32_t const COND_GE = 0x0a;
  // static uint32_t const COND_LT = 0x0b;
  // static uint32_t const COND_GT = 0x0c;
  // static uint32_t const COND_LE = 0x0d;
  // static uint32_t const COND_AL = 0x0e;
    
  // /* mask for valid bits in processor control and status registers */
  // static uint32_t const PSR_UNALLOC_MASK = 0x00f00000;

  // struct SRegID : public unisim::util::identifier::Identifier<SRegID>
  // {
  //   enum Code {
  //     SCTLR, ACTLR,
  //     CTR, MPIDR,
  //     ID_PFR0, CCSIDR, CLIDR, CSSELR,
  //     CPACR, NSACR,
  //     TTBR0, TTBR1, TTBCR,
  //     DACR,
  //     DFSR, IFSR, DFAR, IFAR,
  //     ICIALLUIS, BPIALLIS,
  //     ICIALLU, ICIMVAU, BPIALL,
  //     DCIMVAC, DCISW, DCCMVAC, DCCSW, DCCMVAU, DCCIMVAC,
  //     TLBIALLIS, TLBIALL, TLBIASID,
  //     VBAR,
  //     CONTEXTIDR,
  //     DIAGCR, CFGBAR, end
  //   } code;

  //   char const* c_str() const
  //   {
  //     switch (code)
  //       {
  //       case      SCTLR: return "sctlr";
  //       case      ACTLR: return "actlr";
  //       case        CTR: return "ctr";
  //       case      MPIDR: return "mpidr";
  //       case    ID_PFR0: return "id_pfr0";
  //       case     CCSIDR: return "ccsidr";
  //       case      CLIDR: return "clidr";
  //       case     CSSELR: return "csselr";
  //       case      CPACR: return "cpacr";
  //       case      NSACR: return "nsacr";
  //       case      TTBR0: return "ttbr0";
  //       case      TTBR1: return "ttbr1";
  //       case      TTBCR: return "ttbcr";
  //       case       DACR: return "dacr";
  //       case       DFSR: return "dfsr";
  //       case       IFSR: return "ifsr";
  //       case       DFAR: return "dfar";
  //       case       IFAR: return "ifar";
  //       case  ICIALLUIS: return "icialluis";
  //       case   BPIALLIS: return "bpiallis";
  //       case    ICIALLU: return "iciallu";
  //       case    ICIMVAU: return "icimvau";
  //       case     BPIALL: return "bpiall";
  //       case    DCIMVAC: return "dcimvac";
  //       case      DCISW: return "dcisw";
  //       case    DCCMVAC: return "dccmvac";
  //       case      DCCSW: return "dccsw";
  //       case    DCCMVAU: return "dccmvau";
  //       case   DCCIMVAC: return "dccimvac";
  //       case  TLBIALLIS: return "tlbiallis";
  //       case    TLBIALL: return "tlbiall";
  //       case   TLBIASID: return "tlbiasid";
  //       case       VBAR: return "vbar";
  //       case CONTEXTIDR: return "contextidr";
  //       case     DIAGCR: return "diagcr";
  //       case     CFGBAR: return "cfgbar";
  //       case end:        break;
  //       }
  //     return "NA";
  //   }

  //   SRegID() : code(end) {}
  //   SRegID( Code _code ) : code(_code) {}
  //   SRegID( char const* _code ) : code(end) { init(_code); }
  // };
  
  // U32& SReg( SRegID reg )
  // {
  //   if (reg.code == SRegID::end)
  //     throw 0;
  //   return sregs[reg.idx()];
  // }
    
  // struct RegID : public unisim::util::identifier::Identifier<RegID>
  // {
  //   enum Code
  //     {
  //       NA = 0,
  //       r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, sp, lr,
  //       n, z, c, v, itstate, // q, ge0, ge1, ge2, ge3,
  //       cpsr, spsr,
  //       fpscr, fpexc,
  //       r8_fiq,
  //       r9_fiq,
  //       sl_fiq,
  //       fp_fiq,
  //       ip_fiq,
  //       sp_fiq,
  //       lr_fiq,
  //       r8_usr,
  //       r9_usr,
  //       sl_usr,
  //       fp_usr,
  //       ip_usr,
  //       sp_usr,
  //       lr_usr,
  //       sp_irq,
  //       sp_svc,
  //       sp_abt,
  //       sp_hyp,
  //       sp_und,
  //       lr_irq,
  //       lr_svc,
  //       lr_abt,
  //       lr_hyp,
  //       lr_und,
  //       end
  //     } code;

  //   char const* c_str() const
  //   {
  //     switch (code)
  //       {
  //       case         r0: return "r0";
  //       case         r1: return "r1";
  //       case         r2: return "r2";
  //       case         r3: return "r3";
  //       case         r4: return "r4";
  //       case         r5: return "r5";
  //       case         r6: return "r6";
  //       case         r7: return "r7";
  //       case         r8: return "r8";
  //       case         r9: return "r9";
  //       case         sl: return "sl";
  //       case         fp: return "fp";
  //       case         ip: return "ip";
  //       case         sp: return "sp";
  //       case         lr: return "lr";
  //       case          n: return "n";
  //       case          z: return "z";
  //       case          c: return "c";
  //       case          v: return "v";
  //       case    itstate: return "itstate";
  //       case       cpsr: return "cpsr";
  //       case       spsr: return "spsr";
  //       case      fpscr: return "fpscr";
  //       case      fpexc: return "fpexc";
  //       case     r8_fiq: return "r8_fiq";
  //       case     r9_fiq: return "r9_fiq";
  //       case     sl_fiq: return "sl_fiq";
  //       case     fp_fiq: return "fp_fiq";
  //       case     ip_fiq: return "ip_fiq";
  //       case     sp_fiq: return "sp_fiq";
  //       case     lr_fiq: return "lr_fiq";
  //       case     r8_usr: return "r8_usr";
  //       case     r9_usr: return "r9_usr";
  //       case     sl_usr: return "sl_usr";
  //       case     fp_usr: return "fp_usr";
  //       case     ip_usr: return "ip_usr";
  //       case     sp_usr: return "sp_usr";
  //       case     lr_usr: return "lr_usr";
  //       case     sp_irq: return "sp_irq";
  //       case     sp_svc: return "sp_svc";
  //       case     sp_abt: return "sp_abt";
  //       case     sp_hyp: return "sp_hyp";
  //       case     sp_und: return "sp_und";
  //       case     lr_irq: return "lr_irq";
  //       case     lr_svc: return "lr_svc";
  //       case     lr_abt: return "lr_abt";
  //       case     lr_hyp: return "lr_hyp";
  //       case     lr_und: return "lr_und";
  //       case         NA:
  //       case        end: break;
  //       }
  //     return "NA";
  //   }
      
  //   RegID() : code(end) {}
  //   RegID( Code _code ) : code(_code) {}
  //   RegID( char const* _code ) : code(end) { init( _code ); }
  // };

  // ActionNode*      path;
  // U32              reg_values[16];
  // U32              next_insn_addr;
  // Br::type_t       branch_type;
  // PSR              cpsr;
  // U32              spsr;
  // U32              sregs[SRegID::end];
  // U32              FPSCR, FPEXC;
  // std::set<Expr>   stores;
  // bool             unpredictable;
  // bool             is_it_assigned; /* determines wether current instruction is an IT one. */
  // Mode             mode;
  // ForeignRegisters foreign_registers;
  // Expr             neonregs[32][NEONSIZE];

  struct Page
  {
    std::vector<uint8_t> storage;
    uint32_t base;
    unsigned perms;

    uint32_t hi() const { return base + storage.size(); }

    bool operator < (Page const& p) const { return hi() <= p.base; }
    bool operator > (Page const& p) const { return p.hi() <= base; }

    struct Above
    {
      using is_transparent = void;
      bool operator() (Page const& a, Page const& b) const { return a > b; }
      bool operator() (Page const& a, unsigned b) const { return b < a.base; }
    };

    Page( uint32_t addr, uint32_t size, unsigned _perms )
      : storage( size ), base(addr), perms(_perms)
    {}
    uint32_t len() const { return storage.size(); }
    uint32_t write(uint32_t addr, uint8_t const* bytes, uint32_t size) const
    {
      if (addr < base) throw 0;
      addr -= base;
      if (addr > len()) throw 0;
      uint32_t written = std::min(len()-addr,size);
      std::copy( &bytes[0], &bytes[written], at(addr) );
      return written;
    }
    uint8_t* at(uint32_t x) const { return const_cast<uint8_t*>(&storage[x]); }
  };

  std::set<Page, Page::Above> pages;

  int mem_map(uint32_t addr, uint32_t size, unsigned perms)
  {
    Page page(addr, size, perms);
    auto below = pages.lower_bound(page);
    if (below != pages.end() and not (*below < page))
      {
        return -1;
      }
    if (pages.size())
      {
        auto above = std::prev(below);
        if (above != pages.end() and not (page < *above))
          {
            return -1;
          }
      }
    pages.insert(below,std::move(page));
    return 0;
  }

  int
  mem_write(uint32_t addr, uint8_t const* bytes, uintptr_t size)
  {
    for (auto pi = pages.lower_bound(addr); pi != pages.end(); --pi)
      {
        uintptr_t written = pi->write(addr, bytes, size);
        if (written >= size)
          return 0;
        addr += written;
        bytes += written;
        size -= written;
      }
    
    throw 0;
    return 0;
  }

  struct RegView
  {
    virtual ~RegView() {}
    virtual void write( Processor& proc, int id, uint8_t const* bytes ) const = 0;
    virtual void read( Processor& proc, int id, uint8_t* bytes ) const = 0;
  };

  RegView const*
  get_reg(int regid)
  {
    switch (regid)
      {
      case 0: /* UC_ARM_REG_INVALID */
        break;
      case 1: /* UC_ARM_REG_APSR */
        break;
      case 2: /* UC_ARM_REG_APSR_NZCV */
        break;
      case 3: /* UC_ARM_REG_CPSR */
        break;
      case 4: /* UC_ARM_REG_FPEXC */
        break;
      case 5: /* UC_ARM_REG_FPINST */
        break;
      case 6: /* UC_ARM_REG_FPSCR */
        break;
      case 7: /* UC_ARM_REG_FPSCR_NZCV */
        break;
      case 8: /* UC_ARM_REG_FPSID */
        break;
      case 9: /* UC_ARM_REG_ITSTATE */
        break;
      case 10: /* UC_ARM_REG_LR, UC_ARM_REG_R14 */
        break;
      case 11: /* UC_ARM_REG_PC, UC_ARM_REG_R15 */
        break;
      case 12: /* UC_ARM_REG_SP, UC_ARM_REG_R13 */
        break;
      case 13: /* UC_ARM_REG_SPSR */
        break;
      case 14: /* UC_ARM_REG_D0 */
        break;
      case 15: /* UC_ARM_REG_D1 */
        break;
      case 16: /* UC_ARM_REG_D2 */
        break;
      case 17: /* UC_ARM_REG_D3 */
        break;
      case 18: /* UC_ARM_REG_D4 */
        break;
      case 19: /* UC_ARM_REG_D5 */
        break;
      case 20: /* UC_ARM_REG_D6 */
        break;
      case 21: /* UC_ARM_REG_D7 */
        break;
      case 22: /* UC_ARM_REG_D8 */
        break;
      case 23: /* UC_ARM_REG_D9 */
        break;
      case 24: /* UC_ARM_REG_D10 */
        break;
      case 25: /* UC_ARM_REG_D11 */
        break;
      case 26: /* UC_ARM_REG_D12 */
        break;
      case 27: /* UC_ARM_REG_D13 */
        break;
      case 28: /* UC_ARM_REG_D14 */
        break;
      case 29: /* UC_ARM_REG_D15 */
        break;
      case 30: /* UC_ARM_REG_D16 */
        break;
      case 31: /* UC_ARM_REG_D17 */
        break;
      case 32: /* UC_ARM_REG_D18 */
        break;
      case 33: /* UC_ARM_REG_D19 */
        break;
      case 34: /* UC_ARM_REG_D20 */
        break;
      case 35: /* UC_ARM_REG_D21 */
        break;
      case 36: /* UC_ARM_REG_D22 */
        break;
      case 37: /* UC_ARM_REG_D23 */
        break;
      case 38: /* UC_ARM_REG_D24 */
        break;
      case 39: /* UC_ARM_REG_D25 */
        break;
      case 40: /* UC_ARM_REG_D26 */
        break;
      case 41: /* UC_ARM_REG_D27 */
        break;
      case 42: /* UC_ARM_REG_D28 */
        break;
      case 43: /* UC_ARM_REG_D29 */
        break;
      case 44: /* UC_ARM_REG_D30 */
        break;
      case 45: /* UC_ARM_REG_D31 */
        break;
      case 46: /* UC_ARM_REG_FPINST2 */
        break;
      case 47: /* UC_ARM_REG_MVFR0 */
        break;
      case 48: /* UC_ARM_REG_MVFR1 */
        break;
      case 49: /* UC_ARM_REG_MVFR2 */
        break;
      case 50: /* UC_ARM_REG_Q0 */
        break;
      case 51: /* UC_ARM_REG_Q1 */
        break;
      case 52: /* UC_ARM_REG_Q2 */
        break;
      case 53: /* UC_ARM_REG_Q3 */
        break;
      case 54: /* UC_ARM_REG_Q4 */
        break;
      case 55: /* UC_ARM_REG_Q5 */
        break;
      case 56: /* UC_ARM_REG_Q6 */
        break;
      case 57: /* UC_ARM_REG_Q7 */
        break;
      case 58: /* UC_ARM_REG_Q8 */
        break;
      case 59: /* UC_ARM_REG_Q9 */
        break;
      case 60: /* UC_ARM_REG_Q10 */
        break;
      case 61: /* UC_ARM_REG_Q11 */
        break;
      case 62: /* UC_ARM_REG_Q12 */
        break;
      case 63: /* UC_ARM_REG_Q13 */
        break;
      case 64: /* UC_ARM_REG_Q14 */
        break;
      case 65: /* UC_ARM_REG_Q15 */
        break;
      case 66: case 67: case 68: case 69: case 70: case 71: case 72: case 73: case 74: case 75: case 76: case 77: case 78:
        /* UC_ARM_REG_R0,  UC_ARM_REG_R1,  UC_ARM_REG_R2,  UC_ARM_REG_R3,  UC_ARM_REG_R4,  UC_ARM_REG_R5, UC_ARM_REG_R6, UC_ARM_REG_R7,
         * UC_ARM_REG_R8,  UC_ARM_REG_R9,  UC_ARM_REG_R10, UC_ARM_REG_R11, UC_ARM_REG_R12,
         * UC_ARM_REG_SB=R9, UC_ARM_REG_SL=R10, UC_ARM_REG_FP=R11 */
        {
          static struct : public RegView
          {
            void write( Processor& proc, int id, uint8_t const* bytes ) const { proc.SetGPR(id - 66, *(uint32_t*)bytes); }
            void read( Processor& proc, int id, uint8_t* bytes ) const { *(uint32_t*)bytes = proc.GetGPR(id - 66); }
          } _;
          return &_;
        } break;
      case 79: /* UC_ARM_REG_S0 */
        break;
      case 80: /* UC_ARM_REG_S1 */
        break;
      case 81: /* UC_ARM_REG_S2 */
        break;
      case 82: /* UC_ARM_REG_S3 */
        break;
      case 83: /* UC_ARM_REG_S4 */
        break;
      case 84: /* UC_ARM_REG_S5 */
        break;
      case 85: /* UC_ARM_REG_S6 */
        break;
      case 86: /* UC_ARM_REG_S7 */
        break;
      case 87: /* UC_ARM_REG_S8 */
        break;
      case 88: /* UC_ARM_REG_S9 */
        break;
      case 89: /* UC_ARM_REG_S10 */
        break;
      case 90: /* UC_ARM_REG_S11 */
        break;
      case 91: /* UC_ARM_REG_S12 */
        break;
      case 92: /* UC_ARM_REG_S13 */
        break;
      case 93: /* UC_ARM_REG_S14 */
        break;
      case 94: /* UC_ARM_REG_S15 */
        break;
      case 95: /* UC_ARM_REG_S16 */
        break;
      case 96: /* UC_ARM_REG_S17 */
        break;
      case 97: /* UC_ARM_REG_S18 */
        break;
      case 98: /* UC_ARM_REG_S19 */
        break;
      case 99: /* UC_ARM_REG_S20 */
        break;
      case 100: /* UC_ARM_REG_S21 */
        break;
      case 101: /* UC_ARM_REG_S22 */
        break;
      case 102: /* UC_ARM_REG_S23 */
        break;
      case 103: /* UC_ARM_REG_S24 */
        break;
      case 104: /* UC_ARM_REG_S25 */
        break;
      case 105: /* UC_ARM_REG_S26 */
        break;
      case 106: /* UC_ARM_REG_S27 */
        break;
      case 107: /* UC_ARM_REG_S28 */
        break;
      case 108: /* UC_ARM_REG_S29 */
        break;
      case 109: /* UC_ARM_REG_S30 */
        break;
      case 110: /* UC_ARM_REG_S31 */
        break;
      case 111: /* UC_ARM_REG_C1_C0_2 */
        break;
      case 112: /* UC_ARM_REG_C13_C0_2 */
        break;
      case 113: /* UC_ARM_REG_C13_C0_3 */
        break;
      case 114: /* UC_ARM_REG_ENDING */
        break;
      }
    return 0;
  }
  
  int
  reg_write(int regid, uint8_t const* bytes)
  {
    RegView const* rv = get_reg(regid);
    if (not rv) return -1;
    rv->write(*this, regid, bytes);
    return 0;
  }
  int
  reg_read(int regid, uint8_t* bytes)
  {
    RegView const* rv = get_reg(regid);
    if (not rv) return -1;
    rv->read(*this, regid, bytes);
    return 0;
  }
  void Sync() override {};
};

// Processor::BOOL CheckCondition( Processor& state, unsigned cond )
// {
//   Processor::BOOL N = state.cpsr.n, Z = state.cpsr.z, C = state.cpsr.c, V = state.cpsr.v;

//   switch (cond) {
//   case  0: return                   Z; // eq; equal
//   case  1: return               not Z; // ne; not equal
//   case  2: return                   C; // cs/hs; unsigned higher or same
//   case  3: return               not C; // cc/lo; unsigned lower
//   case  4: return                   N; // mi; negative
//   case  5: return               not N; // pl; positive or zero
//   case  6: return                   V; // vs; overflow set
//   case  7: return               not V; // vc; overflow clear
//   case  8: return    not (not C or Z); // hi; unsigned higher
//   case  9: return        (not C or Z); // ls; unsigned lower or same
//   case 10: return       not (N xor V); // ge; signed greater than or equal
//   case 11: return           (N xor V); // lt; signed less than
//   case 12: return not(Z or (N xor V)); // gt; signed greater than
//   case 13: return    (Z or (N xor V)); // le; signed less than or equal
//   case 14: return unisim::util::symbolic::make_const( true ); // al; always
//   default: break;                     // nv; never (illegal)
//   }

//   throw std::logic_error( "undefined condition" );
//   return unisim::util::symbolic::make_const( false );
// }

// Processor::BOOL CheckCondition( Processor& state, Processor::ITCond const& cond )
// {
//   typedef Processor::BOOL BOOL;
//   typedef Processor::U8   U8;
//   BOOL N = state.cpsr.n, Z = state.cpsr.z, C = state.cpsr.c, V = state.cpsr.v;
//   U8 cc = (state.cpsr.itstate >> 4);
//   return
//     ((state.cpsr.itstate & U8(0b1111)) == U8(0)) or // unconditional
//     ((cc == U8(0)) and (Z)) or // eq; equal
//     ((cc == U8(1)) and (not Z)) or // ne; not equal
//     ((cc == U8(2)) and (C)) or // cs/hs; unsigned higher or same
//     ((cc == U8(3)) and (not C)) or // cc/lo; unsigned lower
//     ((cc == U8(4)) and (N)) or // mi; negative
//     ((cc == U8(5)) and (not N)) or // pl; positive or zero
//     ((cc == U8(6)) and (V)) or // vs; overflow set
//     ((cc == U8(7)) and (not V)) or // vc; overflow clear
//     ((cc == U8(8)) and (not (not C or Z))) or // hi; unsigned higher
//     ((cc == U8(9)) and ((not C or Z))) or // ls; unsigned lower or same
//     ((cc == U8(10)) and (not (N xor V))) or // ge; signed greater than or equal
//     ((cc == U8(11)) and ((N xor V))) or // lt; signed less than
//     ((cc == U8(12)) and (not(Z or (N xor V)))) or // gt; signed greater than
//     ((cc == U8(13)) and ((Z or (N xor V)))) or // le; signed less than or equal
//     ((cc == U8(14)) and (unisim::util::symbolic::make_const( true )));
// }

extern "C"
{
  int uc_open(unsigned arm_arch, unsigned is_thumb, void** ucengine)
  {
    assert( arm_arch == 0 );

    *ucengine = new Processor( "cpu", 0, is_thumb );

    return 0;
  }


  int uc_mem_map(void* uc, uint64_t addr, uintptr_t size, uint32_t perms)
  {
    Processor& proc = *(Processor*)uc;
    return proc.mem_map(addr, size, perms);
  }

  int uc_mem_write(void* uc, uint64_t addr, void const* bytes, uintptr_t size)
  {
    Processor& proc = *(Processor*)uc;
    return proc.mem_write(addr,(uint8_t const*)bytes,size);
  }
  
  int uc_reg_write(void* uc, int regid, void const* bytes)
  {
    Processor& proc = *(Processor*)uc;
    return proc.reg_write(regid, (uint8_t const*)bytes);
  }

  int uc_reg_read(void* uc, int regid, void* bytes)
  {
    Processor& proc = *(Processor*)uc;
    return proc.reg_read(regid, (uint8_t*)bytes);
  }

}
