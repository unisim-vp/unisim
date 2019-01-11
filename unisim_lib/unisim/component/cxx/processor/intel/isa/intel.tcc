/*
 *  Copyright (c) 2007-2015,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific 
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY 
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_INTEL_ISA_INTEL_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_INTEL_ISA_INTEL_TCC__

#include <unisim/component/cxx/processor/intel/isa/intel.hh>
#include <unisim/component/cxx/processor/intel/modrm.hh>
#include <unisim/component/cxx/processor/intel/execute.hh>
#include <unisim/component/cxx/processor/intel/math.hh>
#include <unisim/component/cxx/processor/intel/types.hh>
#include <unisim/component/cxx/processor/intel/tmp.hh>

#include <iostream>
#include <typeinfo>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace intel {

  enum operation_t { CALL = 0, JCC, LOOP, JCXZ, JMP, RETURN, INTRETURN, INTERRUPT, ENTER_LEAVE,
                     ADD, OR, ADC, SBB, AND, SUB, XOR, CMP, ROL, ROR, RCL, RCR, SHL, SHR, SAR,
                     TEST, NOT, NEG, DIV, IDIV, MUL, IMUL, LEA, INCDEC, SETCC, SHD, ZCNT, POPCNT,
                     CMC, AHF, CSX, XLAT, ADJUST, BOUND, ADF,
                     FWAIT, F2XM1, FABS, FADD, FBLD, FBSTP, FCHS, FCLEX, FCMOV, FCOM, FCOMI,
                     FINCDECSTP, FDIV, FDIVR, FFREE, FICOM, FILD, FINIT, FIST, FISTTP,
                     FLD, FLDCONST, FLSCW, FLSENV, FMUL, FNOP, FPREM, FRNDINT, FSCALE, FMATH,
                     FLSSTATE, FST, FSTSW, FSUB, FSUBR, FTST, FUCOM, FXAM, FXCH, FOBSOLETE,
                     PUSH, PUSHA, PUSHF, POP, POPA, POPF, MOV, MOVZX, MOVSX, STD, ARPL, CMPXCHG,
                     CMPXCHG8B, XCHG, XADD, CMOVCC, BT, BTC, BTR, BTS, BSWAP, MOVNTI, LFP,
                     MOVS, STOS, CMPS, SCAS, LODS, OUTS, INS, CPUID, RDTSC, 
                     PXOR, MOVDQU, PCMPEQ, PMOVMSKB, VMOVSD,
                     operation_count };

  template <class ARCH>
  void
  Operation<ARCH>::execute( ARCH& arch ) const
  {
    std::cerr << "error: no execute method for `";
    this->disasm( std::cerr );
    std::cerr << "' in " << (typeid(*this).name()) << "\n";
    throw 0;
  };
  
  
  template <typename INT>
  struct ImmValue { int index; INT value; ImmValue( int idx ) : index( idx ), value() {} };
  
  struct GReg { uint8_t idx; GReg() : idx() {} };
  struct EReg { uint8_t idx; EReg() : idx() {} };
  
  template <typename LEFT, typename RIGHT>
  struct AndSeq
  {
    typedef AndSeq<LEFT,RIGHT> this_type;
    template <typename RHS>
    AndSeq<this_type, RHS> operator & ( RHS const& rhs ) { return AndSeq<this_type, RHS>( *this, rhs ); }
    
    LEFT left;
    RIGHT right;
    
    AndSeq( LEFT const& _left, RIGHT const& _right ) : left( _left ), right( _right ) {}
    
    template <typename PROPERTY>
    uint8_t const* get( PROPERTY& out, uint8_t const* bytes )
    {
      if ((bytes = left.get( out, bytes )))
        bytes = right.get( out, bytes );
      return bytes;
    }
  };

  template <unsigned SIZE>
  struct OpSize
  {
    typedef OpSize<SIZE> this_type;
    template <typename RHS>
    AndSeq<this_type, RHS> operator & ( RHS const& rhs ) { return AndSeq<this_type, RHS>( *this, rhs ); }
    
    template <typename PROPERTY> uint8_t const* get( PROPERTY& value, uint8_t const* bytes ) { return bytes; }
    
    uint8_t const* get( CodeBase const& cb, uint8_t const* bytes ) { return (cb.opsize() != SIZE) ? 0 : bytes; }
  };
  
  template <uint8_t REP>
  struct RP
  {
    typedef RP<REP> this_type;
    template <typename RHS>
    AndSeq<this_type, RHS> operator & ( RHS const& rhs ) { return AndSeq<this_type, RHS>( *this, rhs ); }
    
    template <typename PROPERTY> uint8_t const* get( PROPERTY& value, uint8_t const* bytes ) { return bytes; }
    
    uint8_t const* get( CodeBase const& cb, uint8_t const* bytes ) { return (cb.rep != REP) ? 0 : bytes; }
  };
  
  typedef RP<0> RP_;
  typedef RP<2> RPF2;
  typedef RP<3> RPF3;

  template <uint8_t _66>
  struct OP
  {
    typedef OP<_66> this_type;
    template <typename RHS>
    AndSeq<this_type, RHS> operator & ( RHS const& rhs ) { return AndSeq<this_type, RHS>( *this, rhs ); }

    template <typename PROPERTY> uint8_t const* get( PROPERTY& value, uint8_t const* bytes ) { return bytes; }

    uint8_t const* get( CodeBase const& cb, uint8_t const* bytes ) { return (cb.opsz_66 != _66) ? 0 : bytes; }
  };
  
  typedef OP<0> OP_;
  typedef OP<1> OP66;
  
  template <unsigned SIZE>
  struct AddrSize
  {
    typedef AddrSize<SIZE> this_type;
    template <typename RHS>
    AndSeq<this_type, RHS> operator & ( RHS const& rhs ) { return AndSeq<this_type, RHS>( *this, rhs ); }
    
    template <typename PROPERTY> uint8_t const* get( PROPERTY& value, uint8_t const* bytes ) { return bytes; }
    
    uint8_t const* get( CodeBase const& cb, uint8_t const* bytes ) { return (cb.addrsize() != SIZE) ? 0 : bytes; }
  };
  
  template <typename intT, unsigned BYTECOUNT>
  intT getimm( uint8_t const* _bytes )
  {
    if (sizeof(intT) < BYTECOUNT) throw 0;
    intT tmp = 0;
    for (unsigned byte = 0; byte < BYTECOUNT; ++byte)
      tmp |= (intT( _bytes[byte] ) << (8*byte));
    unsigned const bitshift = 8*(sizeof(intT) - BYTECOUNT);
    tmp = (tmp << bitshift) >> bitshift;
    return tmp;
  }

  template <unsigned SIZE>
  struct Imm
  {
    static unsigned const size = SIZE;
    typedef Imm<SIZE> this_type;
    template <typename RHS>
    AndSeq<this_type, RHS> operator & ( RHS const& rhs ) { return AndSeq<this_type, RHS>( *this, rhs ); }
    
    template <typename PROPERTY> uint8_t const* get( PROPERTY& out, uint8_t const* bytes ) { return bytes + (SIZE/8); }
    
    template <typename INT>
    uint8_t const* get( ImmValue<INT>& out, uint8_t const* bytes )
    {
      if (out.index == 0)
        {
          out.index = -1;
          out.value = INT( getimm<typename CTypeFor<SIZE>::s,SIZE/8>( bytes ) );
          return 0;
        }
      if (out.index > 0) 
        out.index -= 1;
      return bytes + (SIZE/8);
    }
  };

  struct RMOpFabric
  {
    virtual ~RMOpFabric() {}
    virtual void newModBase( unsigned addr_size, uint8_t segment, uint8_t base ) = 0;
    virtual void newMod32Sib( uint8_t seg, uint8_t scale, uint8_t index, uint8_t base ) = 0;
    virtual void newMod32SiDisp( uint8_t seg, uint8_t scale, uint8_t index, int32_t disp ) = 0;
    virtual void newMod32Disp( uint8_t seg, int32_t disp ) = 0;
    virtual void newMod32BaseDisp8( uint8_t seg, uint8_t rm, int8_t disp ) = 0;
    virtual void newMod32SibDisp8( uint8_t seg, uint8_t scale, uint8_t index, uint8_t base, int8_t disp ) = 0;
    virtual void newMod32BaseDisp32( uint8_t seg, uint8_t rm, int32_t disp ) = 0;
    virtual void newMod32SibDisp32( uint8_t seg, uint8_t scale, uint8_t index, uint8_t base, int32_t disp ) = 0;
    virtual void makeROp( uintptr_t reg ) = 0;
  };

  struct VarValue { uint8_t value; VarValue() : value() {} };
  
  template <int BITS>
  struct OpCodeVar
  {
    OpCodeVar( uint8_t byte ) : code(byte>>BITS) {} uint8_t code;
    
    template <typename PROPERTY> uint8_t const* get( PROPERTY& out, uint8_t const* bytes ) { return bytes + 1; }
    uint8_t const* get( CodeBase const& out, uint8_t const* bytes ) { return (bytes[0] >> BITS) == code ? bytes + 1 : 0; }
    uint8_t const* get( VarValue& out, uint8_t const* bytes ) { out.value = bytes[0] & ((1<<BITS)-1); return 0; }
  };

  template <int BITS> struct Var { typedef OpCodeVar<BITS> B; };

  struct OpCodeReg
  {
    OpCodeReg( uint8_t byte ) : code(byte>>3), rexb() {}

    uint8_t code : 5;
    uint8_t rexb : 1;
    
    template <typename PROPERTY> uint8_t const* get( PROPERTY& out, uint8_t const* bytes ) { return bytes + 1; }

    uint8_t const* get( CodeBase const& cb, uint8_t const* bytes ) { rexb = cb.rex(); return (bytes[0] >> 3) == code ? bytes + 1 : 0; }
    uint8_t const* get( EReg& er, uint8_t const* bytes ) { er.idx = get_reg(bytes); return 0; }
    uint8_t const* get( RMOpFabric& out, uint8_t const* bytes ) { out.makeROp( get_reg(bytes) ); return 0; }
    unsigned get_reg( uint8_t const* bytes ) const { return (bytes[0] & 7) | (rexb << 3); }
  };

  struct Reg { typedef OpCodeReg B; };

  struct MRMOpCode
  {
    MRMOpCode( uint8_t _code ) : code(_code) {} uint8_t code;
    template <typename PROPERTY> uint8_t const* get( PROPERTY& out, uint8_t const* bytes ) { return bytes + 0; }
    uint8_t const* get( CodeBase const& cb, uint8_t const* bytes ) { return ((bytes[0] >> 3) & 7) == code ? bytes + 0 : 0; }
  };
  
  template <unsigned LENGTH>
  struct OpCode
  {
    typedef OpCode<LENGTH> this_type;
    template <typename RHS>
    AndSeq<this_type, RHS> operator & ( RHS const& rhs ) { return AndSeq<this_type, RHS>( *this, rhs ); }

    typedef OpCode<LENGTH-1> Head;
    template <typename RHS>
    AndSeq<Head, typename RHS::B> operator + ( RHS const& rhs ) { return AndSeq<Head, typename RHS::B>( &ref[0], ref[LENGTH-1] ); }
    // WithReg operator + ( Reg const& rhs ) { return WithReg( OpCode<LENGTH-1>( &ref[0] ), OpCodeReg( ref[LENGTH-1] ) ); }

    typedef AndSeq<this_type, MRMOpCode> WithMRMOpCode;
    WithMRMOpCode operator / ( int code ) { return WithMRMOpCode( *this, MRMOpCode( code ) ); }
    
    uint8_t ref[LENGTH];
    OpCode( uint8_t const* _ref ) { for (unsigned idx = 0; idx < LENGTH; ++idx ) ref[idx] = _ref[idx]; }
    
    template <typename PROPERTY> uint8_t const* get( PROPERTY& out, uint8_t const* bytes ) { return bytes + LENGTH; }
    
    uint8_t const*
    get( CodeBase const& cb, uint8_t const* bytes )
    {
      for (unsigned idx = 0; idx < LENGTH; ++idx)
        if (ref[idx] != bytes[idx]) return 0;
      return bytes + LENGTH;
    }
  };
  
  template <unsigned LENGTH>
  OpCode<LENGTH-1> opcode( char const (&ref)[LENGTH] )
  { return OpCode<LENGTH-1>( reinterpret_cast<uint8_t const*>( &ref[0] ) ); }
  
  template <class ARCH,unsigned ADDRSIZE>
  struct ModBase : public MOp<ARCH>
  {
    typedef typename ARCH::addr_t addr_t;
    ModBase( uint8_t _seg, uint8_t _rm ) : MOp<ARCH>( _seg ), rm( _rm ) {} uint8_t rm;

    void disasm_memory_operand( std::ostream& sink ) const { sink << DisasmMS( MOp<ARCH>::segment ) << '(' << DisasmG<ADDRSIZE>( rm ) << ')'; };
    
    addr_t effective_address( ARCH& arch ) const { return addr_t( arch.template regread<ADDRSIZE>( rm ) ); }
  };
  
  template <class ARCH>
  struct Mod32Sib : public MOp<ARCH>
  {
    typedef typename ARCH::addr_t addr_t;
    Mod32Sib( uint8_t _seg, uint8_t _scale, uint8_t _index, uint8_t _base ) : MOp<ARCH>( _seg ), scale( _scale ), index( _index ), base( _base ) {} uint8_t scale, index, base;
    
    void disasm_memory_operand( std::ostream& sink ) const
    {
      sink << DisasmMS( MOp<ARCH>::segment ) << '(' << DisasmGd( base );
      if                       (index != 4) sink << ',' << DisasmGd( index ) << ',' << (1 << scale);
      else if ((base != 4) or (scale != 0)) sink << ',' << "%eiz" << ',' << (1 << scale);
      sink << ')';
    }

    addr_t effective_address( ARCH& arch ) const { return arch.regread32( base ) + ((index != 4) ? (arch.regread32( index ) * addr_t( 1 << scale )) : addr_t( 0 )); }
  };
  
  template <class ARCH>
  struct Mod32Disp : public MOp<ARCH>
  {
    typedef typename ARCH::addr_t addr_t;
    Mod32Disp( uint8_t _seg, int32_t _disp ) : MOp<ARCH>( _seg ), disp( _disp ) {} int32_t disp;
    
    void disasm_memory_operand( std::ostream& sink ) const { sink << DisasmMS( MOp<ARCH>::segment ) << "0x" << std::hex << disp << std::dec; };
    
    addr_t effective_address( ARCH& arch ) const { return addr_t( disp ); };
  };
  
  template <class ARCH>
  struct Mod32SiDisp : public MOp<ARCH>
  {
    typedef typename ARCH::addr_t addr_t;
    Mod32SiDisp( uint8_t _seg, uint8_t _scale, uint8_t _index, int32_t _disp ) : MOp<ARCH>( _seg ), scale( _scale ), index( _index ), disp( _disp ) {}
    uint8_t scale, index; int32_t disp;
    
    void disasm_memory_operand( std::ostream& sink ) const
    {
      sink << DisasmMS( MOp<ARCH>::segment ) << (disp < 0 ? "-0x" : "0x") << std::hex << (disp < 0 ? -disp : disp) << "(,";
      if (index != 4) sink << DisasmGd( index );
      else            sink << "%eiz";
      sink << ',' << (1 << scale) << ')';
    }

    addr_t effective_address( ARCH& arch ) const { return addr_t( disp ) + ((index != 4) ? (arch.regread32( index ) * addr_t( 1 << scale )) : addr_t( 0 )); }
  };
  
  template <class ARCH,typename DISP>
  struct Mod32SibDisp : public MOp<ARCH>
  {
    typedef typename ARCH::addr_t addr_t;
    Mod32SibDisp( uint8_t _seg, uint8_t _scale, uint8_t _index, uint8_t _base, DISP _disp ) : MOp<ARCH>( _seg ), disp( _disp ), scale( _scale ), index( _index ), base( _base ) {}
    DISP disp; uint8_t scale, index, base;
    
    void disasm_memory_operand( std::ostream& sink ) const
    {
      sink << DisasmMS( MOp<ARCH>::segment ) << (disp < 0 ? "-0x" : "0x") << std::hex << (disp < 0 ? -disp : disp) << '(' << DisasmGd( base );
      if (index != 4) sink << ',' << DisasmGd( index ) << ',' << (1 << scale);
      else if ((base != 4) or (scale != 0)) sink << ',' << "%eiz" << ',' << (1 << scale);
      sink << ')';
    };

    addr_t effective_address( ARCH& arch ) const { return arch.regread32( base ) + addr_t( disp ) + ((index != 4) ? (arch.regread32( index ) * addr_t( 1 << scale )) : addr_t( 0 ) ); };
  };
  
  template <class ARCH,typename DISP>
  struct Mod32BaseDisp : public MOp<ARCH>
  {
    typedef typename ARCH::addr_t addr_t;
    Mod32BaseDisp( uint8_t _seg, uint8_t _rm, DISP _disp ) : MOp<ARCH>( _seg ), rm( _rm ), disp( _disp ) {} uint8_t rm; DISP disp;
    
    void disasm_memory_operand( std::ostream& sink ) const
    {
      sink << DisasmMS( MOp<ARCH>::segment ) << (disp < 0 ? "-0x" : "0x") << std::hex << (disp < 0 ? -disp : disp) << std::dec << '(' << DisasmGd( rm ) << ')';
    };

    addr_t effective_address( ARCH& arch ) const { return arch.regread32( rm ) + addr_t( disp ); };
  };
  
  template <class ARCH>
  MOp<ARCH>* make_rop( unsigned reg )  { return (MOp<ARCH>*)( uintptr_t( reg ) ); }

  template <class ARCH>
  struct RMOpFabricT : RMOpFabric
  {
    RMOpFabricT() : mop() {} MOp<ARCH>* mop;
    void newModBase( unsigned addr_size, uint8_t segment, uint8_t base ) override
    {
      if      (addr_size == 16) mop = new ModBase<ARCH,16>( segment, base );
      else if (addr_size == 32) mop = new ModBase<ARCH,32>( segment, base );
      else if (addr_size == 64) mop = new ModBase<ARCH,64>( segment, base );
      else throw 0;
    }
    void newMod32Sib( uint8_t seg, uint8_t scale, uint8_t index, uint8_t base ) override { mop = new Mod32Sib<ARCH>( seg, scale, index, base ); }
    void newMod32SiDisp( uint8_t seg, uint8_t scale, uint8_t index, int32_t disp ) override { mop = new Mod32SiDisp<ARCH>( seg, scale, index, disp ); }
    void newMod32Disp( uint8_t seg, int32_t disp ) override { mop = new Mod32Disp<ARCH>( seg, disp ); }
    void newMod32BaseDisp8( uint8_t seg, uint8_t rm, int8_t disp ) override { mop = new Mod32BaseDisp<ARCH,int8_t>( seg, rm, disp ); }
    void newMod32SibDisp8( uint8_t seg, uint8_t scale, uint8_t index, uint8_t base, int8_t _disp ) override { mop = new Mod32SibDisp<ARCH,int8_t>( seg, scale, index, base, _disp ); }
    void newMod32BaseDisp32( uint8_t seg, uint8_t rm, int32_t disp ) override { mop = new Mod32BaseDisp<ARCH,int32_t>( seg, rm, disp ); }
    void newMod32SibDisp32( uint8_t seg, uint8_t scale, uint8_t index, uint8_t base, int32_t disp ) override { mop = new Mod32SibDisp<ARCH,int32_t>( seg, scale, index, base, disp ); }
    void makeROp( uintptr_t reg ) override { mop = (MOp<ARCH>*)( reg ); }
  };
  
  struct RM
  {
    typedef RM this_type;
    template <typename RHS>
    AndSeq<this_type, RHS> operator & ( RHS const& rhs ) { return AndSeq<this_type, RHS>( *this, rhs ); }
    
    enum mod_type_t {
      mod_addr0 = 0,   /* 0b00[2]:gn[3]:rm[3] */
      mod_sib_addr0,   /* 0b00[2]:gn[3]:0b100[3]:> <:scale[2]:index[3]:base[3] */
      mod_sib_disp,    /* 0b00[2]:gn[3]:0b100[3]:> <:scale[2]:index[3]:0b101[3]:> <:sext disp[32] */
      mod_disp,        /* 0b00[2]:gn[3]:0b101[3]:> <:sext disp[32] */
      mod_rip_rel,     /* 0b00[2]:gn[3]:0b101[3]:> <:sext disp[32] (note: 64-bit mode RIP-relative in 64-bit mode) */
      mod_addr1,       /* 0b01[2]:gn[3]:rm[3]:> <:sext<32> disp[8] */
      mod_sib_addr1,   /* 0b01[2]:gn[3]:0b100[3]:> <:scale[2]:index[3]:base[3]:> <:sext <32> disp[8] */
      mod_addr2,       /* 0b10[2]:gn[3]:rm[3]:> <:sext disp[32] */
      mod_sib_addr2,   /* 0b10[2]:gn[3]:0b100[3]:> <:scale[2]:index[3]:base[3]:> <:sext disp[32] */
      mod_reg          /* 0b11[2]:gn[3]:0b101[3]  */
    };
    
    uint8_t len   : 3;
    uint8_t mode  : 4;
    uint8_t rexb  : 1;
    uint8_t rexx  : 1;
    uint8_t rexr  : 1;
    uint8_t seg   : 3;
    uint8_t adsz  : 3;
    
    RM()
      : len(), mode(), rexb(), rexx(), rexr(), seg(), adsz()
    {}

    template <typename PROPERTY> uint8_t const* get( PROPERTY& out, uint8_t const* bytes ) { return bytes + len; }

    uint8_t const* get( CodeBase const& cb, uint8_t const* bytes )
    {
      seg =  cb.segment;
      uint8_t rex = cb.rex();
      rexb = rex >> 0;
      rexx = rex >> 1;
      unsigned addrsize = cb.addrsize();
      if (addrsize & (addrsize-1))
        throw std::runtime_error("unsupported address size");
      
      switch (addrsize)
        {
        default:
          throw std::runtime_error("unsupported address size");
          break;
        
        case 32: case 64:
          {
            adsz = addrsize == 64 ? 3 : 2;
            uint8_t mod = (*bytes >> 6) & 3;
            // uint8_t gn =  (*bytes >> 3) & 7;
            uint8_t rm =  (*bytes >> 0) & 7;
            bytes += 1;
            
            if (mod == 3) { mode = mod_reg; len = 1; return bytes; }
      
            if (rm == 4)
              {
                /* SIB byte */
                // uint8_t scale = (*bytes >> 6) & 3;
                // uint8_t index = (*bytes >> 3) & 7;
                uint8_t base =  (*bytes >> 0) & 7;
                bytes += 1;
                if (mod == 2) { mode = mod_sib_addr2; len = 6; return bytes + 4; }
                if (mod == 1) { mode = mod_sib_addr1; len = 3; return bytes + 1; }
                /* mod == 0*/
                if (base == 5) { mode = mod_sib_disp; len = 6; return bytes + 4; }
                mode = mod_sib_addr0; len = 2;
                return bytes;
              }
      
            if (mod == 1) { mode = mod_addr1; len = 2; return bytes + 1; }
            if (mod == 2) { mode = mod_addr2; len = 5; return bytes + 4; }
            /* mod == 0 */
            if (rm != 5)     { mode = mod_addr0; len = 1; return bytes; }
            mode = cb.mode64() ? mod_rip_rel : mod_disp;
            len = 5;
            return bytes + 4;
          }
          break;
        }
      return 0;
    }

    uint8_t const* get( RMOpFabric& out, uint8_t const* bytes )
    {
      switch (mode)
        {
        case mod_addr0:     out.newModBase( addr_size(), seg, get_rm(bytes) ); return 0;
        case mod_sib_addr0: out.newMod32Sib( seg, get_scale(bytes), get_index(bytes), get_base(bytes) ); return 0;
        case mod_sib_disp:  out.newMod32SiDisp( seg, get_scale(bytes), get_index(bytes), getimm<int32_t,4>( &bytes[2] ) ); return 0;
        case mod_disp:      out.newMod32Disp( seg, getimm<int32_t,4>( &bytes[1] ) ); return 0;
        case mod_addr1:     out.newMod32BaseDisp8( seg, get_rm(bytes), getimm<int8_t,1>( &bytes[1] ) ); return 0;
        case mod_sib_addr1: out.newMod32SibDisp8( seg, get_scale(bytes), get_index(bytes), get_base(bytes), getimm<int8_t,1>( &bytes[2] ) ); return 0;
        case mod_addr2:     out.newMod32BaseDisp32( seg, get_rm(bytes), getimm<int32_t,4>( &bytes[1] ) ); return 0;
        case mod_sib_addr2: out.newMod32SibDisp32( seg, get_scale(bytes), get_index(bytes), get_base(bytes), getimm<int32_t,4>( &bytes[2] ) ); return 0;
        case mod_reg:       out.makeROp( get_rm(bytes) ); return 0;
        }
      return bytes + len;
    }
    
    uint8_t const* get( GReg& gr, uint8_t const* bytes ) { gr.idx = get_gn(bytes); return 0; }
    
    uint8_t const* get( EReg& er, uint8_t const* bytes ) { if (mode != mod_reg) throw 0; er.idx = get_rm(bytes); return 0; }

    unsigned get_rm( uint8_t const* bytes ) const { return ((bytes[0] >> 0) & 7) | (rexb << 3); }
    unsigned get_gn( uint8_t const* bytes ) const { return ((bytes[0] >> 3) & 7) | (rexr << 3); }

    unsigned get_scale( uint8_t const* bytes ) const { return  (bytes[1] >> 6) & 3; }
    unsigned get_index( uint8_t const* bytes ) const { return ((bytes[1] >> 3) & 7) | (rexx << 3) ; }
    unsigned get_base( uint8_t const* bytes ) const  { return ((bytes[1] >> 0) & 7) | (rexb << 3); }

    unsigned addr_size() { return 8 << adsz; }
  };

  struct Moffs
  {
    typedef Moffs this_type;
    template <typename RHS>
    AndSeq<this_type, RHS> operator & ( RHS const& rhs ) { return AndSeq<this_type, RHS>( *this, rhs ); }
    
    uint8_t length, segment;
    
    Moffs() : length() {}
    
    template <typename PROPERTY> uint8_t const* get( PROPERTY& out, uint8_t const* bytes ) { return bytes + length; }
    
    uint8_t const* get( CodeBase const& cb, uint8_t const* bytes )
    {
      length = cb.addrsize() / 8;
      if (length != 4) throw 0;
      segment = cb.segment;
      return bytes + length;
    }
    
    uint8_t const* get( RMOpFabric& out, uint8_t const* bytes )
    {
      out.newMod32Disp( segment, getimm<int32_t,4>( bytes ) );
      return 0;
    }
  };
  
  template <bool REGONLY>
  struct RM_RegOrMem
  {
    typedef RM_RegOrMem<REGONLY> this_type;
    template <typename RHS>
    AndSeq<this_type, RHS> operator & ( RHS const& rhs ) { return AndSeq<this_type, RHS>( *this, rhs ); }
    
    RM memRM;

    template <typename PROPERTY> uint8_t const* get( PROPERTY& out, uint8_t const* bytes ) { return memRM.get( out, bytes ); }
    uint8_t const* get( CodeBase const& cb, uint8_t const* bytes )
    {
      bytes = memRM.get( cb, bytes );
      if (bytes and ((memRM.mode == RM::mod_reg) xor (REGONLY))) return 0;
      return bytes;
    }
  };
  
  typedef RM_RegOrMem<true>  RM_reg;
  typedef RM_RegOrMem<false> RM_mem;

  template <class ARCH>
  struct BaseMatch
  {
    BaseMatch( InputCode<ARCH> const* _ic ) : ic(_ic) {}
    operator bool () const { return ic; }
    InputCode<ARCH> const* ic;
  };
  template <class ARCH, typename PATTERN>
  struct Match : BaseMatch<ARCH>
  {
    Match( InputCode<ARCH> const* ic, PATTERN const& _pattern )
      : BaseMatch<ARCH>( ic ), pattern( _pattern ) {}

    InputCode<ARCH> const& icode() const { return *BaseMatch<ARCH>::ic; };
    // TODO: following functions should be const
    
    OpBase<ARCH> opbase()
    {
      struct GetLength {} getlength;
      unsigned length = (pattern.get( getlength, icode().opcode() ) - &icode().bytes[0]);
      return OpBase<ARCH>( icode().header, icode().mode, length );
    }

    template <typename PROPERTY> void find( PROPERTY& out, uint8_t const* bytes )
    {
      if (pattern.get( out, bytes )) throw 0;
    }
    
    template <typename INT>
    INT imm( unsigned idx = 0 ) { ImmValue<INT> res( idx ); find( res, icode().opcode() ); return res.value; }
    
    template <typename INT>
    int32_t i( INT it, unsigned idx = 0 ) { ImmValue<int32_t> res( idx ); find( res, icode().opcode() ); return res.value; }
    
    MOp<ARCH>* rmop() { RMOpFabricT<ARCH> res; find( static_cast<RMOpFabric&>(res), icode().opcode() ); return res.mop; }
    
    uint8_t var() { VarValue res; find( res, icode().opcode() ); return res.value; }
    
    uint8_t greg() { GReg res; find( res, icode().opcode() ); return res.idx; }
    
    uint8_t ereg() { EReg res; find( res, icode().opcode() ); return res.idx; }
    
    PATTERN pattern;
  };
  
  template <class ARCH, typename PATTERN>
  Match<ARCH,PATTERN> match( InputCode<ARCH> const& ic, PATTERN const& _pattern )
  {
    PATTERN pattern( _pattern );
    bool good = bool( pattern.get( static_cast<CodeBase const&>( ic ), ic.opcode() ) );
    return Match<ARCH,PATTERN>( good ? &ic : 0, pattern );
  }
  
  // Empty template for decoding tables
  template <class ARCH, operation_t IDX> struct DC {};
  
  // Decoding tables and behavioral description
#include <unisim/component/cxx/processor/intel/isa/branch.hh>
#include <unisim/component/cxx/processor/intel/isa/integer.hh>
#include <unisim/component/cxx/processor/intel/isa/floatingpoint.hh>
#include <unisim/component/cxx/processor/intel/isa/move.hh>
#include <unisim/component/cxx/processor/intel/isa/string.hh>
#include <unisim/component/cxx/processor/intel/isa/special.hh>
#include <unisim/component/cxx/processor/intel/isa/simd.hh>
  
  // Recursive decoding structure
  template <class ARCH, operation_t IDX>
  struct DR
  {
    Operation<ARCH>*
    get( InputCode<ARCH> const& ic )
    {
      DC<ARCH,operation_t(IDX-1)> dc; if (Operation<ARCH>* op = dc.get( ic )) return op;
      DR<ARCH,operation_t(IDX-1)> dr; return dr.get( ic );
    }
  };
  
  // Recursion stop
  template <class ARCH>
  struct DR<ARCH,operation_t(0)>
  {
    Operation<ARCH>* get( InputCode<ARCH> const& ) { return 0; }
  };
  
  template <class ARCH>
  Operation<ARCH>*
  getoperation( InputCode<ARCH> const& ic )
  {
    DR<ARCH,operation_count> dr;
    return dr.get( ic );
  }

} // end of namespace intel
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_INTEL_ISA_INTEL_TCC__
