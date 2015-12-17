#ifndef ISA_TCC
#define ISA_TCC

#include <unisim/component/cxx/processor/intel/isa/intel.hh>
#include <unisim/component/cxx/processor/intel/modrm.hh>
#include <unisim/component/cxx/processor/intel/execute.hh>
#include <unisim/component/cxx/processor/intel/arch.hh>
#include <unisim/component/cxx/processor/intel/math.hh>
#include <unisim/component/cxx/processor/intel/types.hh>
#include <unisim/component/cxx/processor/intel/tmp.hh>

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
                     PXOR, MOVDQU, PCMPEQ, PMOVMSKB, 
                     operation_count };
  
  template <operation_t operation> Operation* decode( CodeBase const& cb ) { return 0; }
  
  template <typename INT>
  struct ImmValue { int index; INT value; ImmValue( int idx ) : index( idx ), value() {} };
  
  struct VBValue { int index; uint8_t value; VBValue( int idx ) : index( idx ), value() {} };

  struct GReg { uint8_t idx; GReg() : idx() {} };
  struct EReg { uint8_t idx; EReg() : idx() {} };
  
  template <typename PATTERN>
  struct Match
  {
    Match( CodeBase const& _cb, PATTERN const& _pattern, bool _good ) : cb( _cb ), pattern( _pattern ), good( _good ) {} CodeBase const& cb; PATTERN pattern; bool good;
    
    operator bool () const { return good; }
    
    // TODO: following functions should be const
    
    OpBase opbase()
    {
      struct GetLength {} getlength;
      unsigned length = (pattern.get( getlength, &cb.bytes[cb.opc_idx] ) - &cb.bytes[0]);
      return OpBase( cb.address, cb.mode, length );
    }
    
    template <typename INT>
    INT imm( unsigned idx = 0 ) { ImmValue<INT> res( idx ); pattern.get( res, &cb.bytes[cb.opc_idx] ); return res.value; }

    MOp* rmop() { MOp* res = 0; pattern.get( res, &cb.bytes[cb.opc_idx] ); return res; }
    
    uint8_t vbval( int idx = 0 ) { VBValue res( idx ); pattern.get( res, &cb.bytes[cb.opc_idx] ); return res.value; }
    
    uint8_t greg() { GReg res; pattern.get( res, &cb.bytes[cb.opc_idx] ); return res.idx; }
    
    uint8_t ereg() { EReg res; pattern.get( res, &cb.bytes[cb.opc_idx] ); return res.idx; }
  };
  
  template <typename PATTERN>
  Match<PATTERN> match( CodeBase const& cb, PATTERN const& _pattern )
  {
    PATTERN pattern( _pattern );
    bool good = bool( pattern.get( cb, &cb.bytes[cb.opc_idx] ) );
    return Match<PATTERN>( cb, pattern, good );
  }
  
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
    uint8_t const* get( PROPERTY& out, uint8_t const* code )
    {
      if (code = left.get( out, code ))
        code = right.get( out, code );
      return code;
    }
  };

  template <unsigned SIZE>
  struct OpSize
  {
    typedef OpSize<SIZE> this_type;
    template <typename RHS>
    AndSeq<this_type, RHS> operator & ( RHS const& rhs ) { return AndSeq<this_type, RHS>( *this, rhs ); }
    
    template <typename PROPERTY> uint8_t const* get( PROPERTY& value, uint8_t const* code ) { return code; }
    
    uint8_t const* get( CodeBase const& cb, uint8_t const* code ) { return (cb.opsize() != SIZE) ? 0 : code; }
  };
  
  template <uint8_t REP>
  struct RP
  {
    typedef RP<REP> this_type;
    template <typename RHS>
    AndSeq<this_type, RHS> operator & ( RHS const& rhs ) { return AndSeq<this_type, RHS>( *this, rhs ); }
    
    template <typename PROPERTY> uint8_t const* get( PROPERTY& value, uint8_t const* code ) { return code; }
    
    uint8_t const* get( CodeBase const& cb, uint8_t const* code ) { return (cb.rep != REP) ? 0 : code; }
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

    template <typename PROPERTY> uint8_t const* get( PROPERTY& value, uint8_t const* code ) { return code; }

    uint8_t const* get( CodeBase const& cb, uint8_t const* code ) { return (cb.opsz_66 != _66) ? 0 : code; }
  };
  
  typedef OP<0> OP_;
  typedef OP<1> OP66;
  
  template <unsigned SIZE>
  struct AddrSize
  {
    typedef AddrSize<SIZE> this_type;
    template <typename RHS>
    AndSeq<this_type, RHS> operator & ( RHS const& rhs ) { return AndSeq<this_type, RHS>( *this, rhs ); }
    
    template <typename PROPERTY> uint8_t const* get( PROPERTY& value, uint8_t const* code ) { return code; }
    
    uint8_t const* get( CodeBase const& cb, uint8_t const* code ) { return (cb.addrsize() != SIZE) ? 0 : code; }
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
    
    template <typename PROPERTY> uint8_t const* get( PROPERTY& out, uint8_t const* code ) { return code + (SIZE/8); }
    
    template <typename INT>
    uint8_t const* get( ImmValue<INT>& out, uint8_t const* code )
    {
      if (out.index == 0)
        {
          out.index = -1;
          out.value = INT( getimm<typename CTypeFor<SIZE>::s,SIZE/8>( code ) );
          return 0;
        }
      if (out.index > 0) 
        out.index -= 1;
      return code + (SIZE/8);
    }
  };
  
  template <uint8_t CODE,  uint8_t MASK>
  struct VarByte
  {
    typedef VarByte<CODE,MASK> this_type;
    template <typename RHS>
    AndSeq<this_type, RHS> operator & ( RHS const& rhs ) { return AndSeq<this_type, RHS>( *this, rhs ); }
    
    template <typename PROPERTY> uint8_t const* get( PROPERTY& out, uint8_t const* code ) { return code + 1; }
    
    uint8_t const* get( VBValue& out, uint8_t const* code )
    {
      if (out.index == 0)
        {
          out.index = -1;
          out.value = (*code) & ~MASK;
          return 0;
        }
      if (out.index > 0) 
        out.index -= 1;
      return code + 1;
    }
    
    uint8_t const* get( CodeBase const& out, uint8_t const* code ) { return ((*code & MASK) == CODE) ? code + 1 : 0; }
  };
  
  template <unsigned BYTES>
  struct OpCode
  {
    typedef OpCode<BYTES> this_type;
    template <typename RHS>
    AndSeq<this_type, RHS> operator & ( RHS const& rhs ) { return AndSeq<this_type, RHS>( *this, rhs ); }
    
    uint8_t bytes[BYTES];
    uint8_t post;
    OpCode( uint8_t const* _bytes, uint8_t _post ) : post( _post ) { for (int idx = 0; idx < BYTES; ++idx ) bytes[idx] = _bytes[idx]; }
    
    template <typename PROPERTY> uint8_t const* get( PROPERTY& out, uint8_t const* code ) { return code + BYTES; }
    
    uint8_t const*
    get( CodeBase const& cb, uint8_t const* code )
    {
      for (unsigned idx = 0; idx < BYTES; ++idx)
        if (bytes[idx] != code[idx]) return 0;
      if ((post != 0xff) and (((code[BYTES] >> 3) & 7) != post)) return 0;
      return code + BYTES;
    }
  };
  
  template <unsigned BYTES>
  OpCode<BYTES-1> opcode( char const (&bytes)[BYTES], uint8_t post = 0xff )
  { return OpCode<BYTES-1>( reinterpret_cast<uint8_t const*>( &bytes[0] ), post ); }
  
  template <unsigned ADDRSIZE>
  struct ModBase : public MOp
  {
    ModBase( uint8_t _seg, uint8_t _rm ) : MOp( _seg ), rm( _rm ) {} uint8_t rm;

    void disasm_memory_operand( std::ostream& sink ) const { sink << DisasmMS( segment ) << '(' << DisasmR<ADDRSIZE>( rm ) << ')'; };
    
    u32_t effective_address( Arch& arch ) const { return u32_t( arch.regread<ADDRSIZE>( rm ) ); }
  };
  
  struct Mod32Sib : public MOp
  {
    Mod32Sib( uint8_t _seg, uint8_t _scale, uint8_t _index, uint8_t _base ) : MOp( _seg ), scale( _scale ), index( _index ), base( _base ) {} uint8_t scale, index, base;
    
    void disasm_memory_operand( std::ostream& sink ) const
    {
      sink << DisasmMS( segment ) << '(' << DisasmRd( base );
      if                       (index != 4) sink << ',' << DisasmRd( index ) << ',' << (1 << scale);
      else if ((base != 4) or (scale != 0)) sink << ',' << "%eiz" << ',' << (1 << scale);
      sink << ')';
    }

    u32_t effective_address( Arch& arch ) const { return arch.regread32( base ) + ((index != 4) ? (arch.regread32( index ) * u32_t( 1 << scale )) : u32_t( 0 )); }
  };
  
  struct Mod32Disp : public MOp
  {
    Mod32Disp( uint8_t _seg, int32_t _disp ) : MOp( _seg ), disp( _disp ) {} int32_t disp;
    
    void disasm_memory_operand( std::ostream& sink ) const { sink << DisasmMS( segment ) << "0x" << std::hex << disp << std::dec; };
    
    u32_t effective_address( Arch& arch ) const { return u32_t( disp ); };
  };
  
  struct Mod32SiDisp : public MOp
  {
    Mod32SiDisp( uint8_t _seg, uint8_t _scale, uint8_t _index, int32_t _disp ) : MOp( _seg ), scale( _scale ), index( _index ), disp( _disp ) {}
    uint8_t scale, index; int32_t disp;
    
    void disasm_memory_operand( std::ostream& sink ) const
    {
      sink << DisasmMS( segment ) << (disp < 0 ? "-0x" : "0x") << std::hex << (disp < 0 ? -disp : disp) << "(,";
      if (index != 4) sink << DisasmRd( index );
      else            sink << "%eiz";
      sink << ',' << (1 << scale) << ')';
    }

    u32_t effective_address( Arch& arch ) const { return u32_t( disp ) + ((index != 4) ? (arch.regread32( index ) * u32_t( 1 << scale )) : u32_t( 0 )); }
  };
  
  template <typename DISP>
  struct Mod32SibDisp : public MOp
  {
    Mod32SibDisp( uint8_t _seg, uint8_t _scale, uint8_t _index, uint8_t _base, DISP _disp ) : MOp( _seg ), disp( _disp ), scale( _scale ), index( _index ), base( _base ) {}
    DISP disp; uint8_t scale, index, base;
    
    void disasm_memory_operand( std::ostream& sink ) const
    {
      sink << DisasmMS( segment ) << (disp < 0 ? "-0x" : "0x") << std::hex << (disp < 0 ? -disp : disp) << '(' << DisasmRd( base );
      if (index != 4) sink << ',' << DisasmRd( index ) << ',' << (1 << scale);
      else if ((base != 4) or (scale != 0)) sink << ',' << "%eiz" << ',' << (1 << scale);
      sink << ')';
    };

    u32_t effective_address( Arch& arch ) const { return arch.regread32( base ) + u32_t( disp ) + ((index != 4) ? (arch.regread32( index ) * u32_t( 1 << scale )) : u32_t( 0 ) ); };
  };
  
  template <typename DISP>
  struct Mod32BaseDisp : public MOp
  {
    Mod32BaseDisp( uint8_t _seg, uint8_t _rm, DISP _disp ) : MOp( _seg ), rm( _rm ), disp( _disp ) {} uint8_t rm; DISP disp;
    
    void disasm_memory_operand( std::ostream& sink ) const
    {
      sink << DisasmMS( segment ) << (disp < 0 ? "-0x" : "0x") << std::hex << (disp < 0 ? -disp : disp) << std::dec << '(' << DisasmRd( rm ) << ')';
    };

    u32_t effective_address( Arch& arch ) const { return arch.regread32( rm ) + u32_t( disp ); };
  };
  
  struct RM
  {
    typedef RM this_type;
    template <typename RHS>
    AndSeq<this_type, RHS> operator & ( RHS const& rhs ) { return AndSeq<this_type, RHS>( *this, rhs ); }
    
    enum mod_type_t {
      mod32_addr0 = 0, /* 0b00[2]:gn[3]:rm[3] */
      mod32_sib_addr0, /* 0b00[2]:gn[3]:0b100[3]:> <:scale[2]:index[3]:base[3] */
      mod32_sib_disp,  /* 0b00[2]:gn[3]:0b100[3]:> <:scale[2]:index[3]:0b101[3]:> <:sext disp[32] */
      mod32_disp,      /* 0b00[2]:gn[3]:0b101[3]:> <:sext disp[32] */
      mod32_addr1,     /* 0b01[2]:gn[3]:rm[3]:> <:sext<32> disp[8] */
      mod32_sib_addr1, /* 0b01[2]:gn[3]:0b100[3]:> <:scale[2]:index[3]:base[3]:> <:sext <32> disp[8] */
      mod32_addr2,     /* 0b10[2]:gn[3]:rm[3]:> <:sext disp[32] */
      mod32_sib_addr2, /* 0b10[2]:gn[3]:0b100[3]:> <:scale[2]:index[3]:base[3]:> <:sext disp[32] */
      mod_reg        /* 0b11[2]:gn[3]:0b101[3]  */
    };
    
    uint8_t type  : 8;
    uint8_t len   : 4;
    uint8_t seg   : 4;
    uint8_t mod   : 2;
    uint8_t gn    : 3;
    uint8_t rm    : 3;
    uint8_t scale : 2;
    uint8_t index : 3;
    uint8_t base  : 3;
    
    template <typename PROPERTY> uint8_t const* get( PROPERTY& out, uint8_t const* code ) { return code + len; }
    
    uint8_t const* get( CodeBase const& cb, uint8_t const* code )
    {
      seg = cb.segment;
      mod = (*code >> 6) & 3;
      gn =  (*code >> 3) & 7;
      rm =  (*code >> 0) & 7;
      code += 1;
      
      if (mod == 3) { type = mod_reg; len = 1; return code; }
      
      if (rm == 4) {
        scale = (*code >> 6) & 3;
        index = (*code >> 3) & 7;
        base =  (*code >> 0) & 7;
        code += 1;
        if (mod == 2) { type = mod32_sib_addr2; len = 6; return code + 4; }
        if (mod == 1) { type = mod32_sib_addr1; len = 3; return code + 1; }
        /* mod == 0*/
        if (base == 5) { type = mod32_sib_disp; len = 6; return code + 4; }
        type = mod32_sib_addr0; len = 2;
        return code;
      }
      
      if (mod == 1) { type = mod32_addr1; len = 2; return code + 1; }
      if (mod == 2) { type = mod32_addr2; len = 5; return code + 4; }
      /* mod == 0 */
      if (rm == 5) { type = mod32_disp; len = 5; return code + 4; };
      type = mod32_addr0; len = 1;
      return code;
    }
    
    static MOp* MkReg( unsigned reg ) { return (MOp*)( uintptr_t( reg ) ); }
    
    uint8_t const* get( MOp*& out, uint8_t const* code )
    {
      switch (type) {
      case mod32_addr0: out = new ModBase<32>( seg, rm ); return 0;
      case mod32_sib_addr0: out = new Mod32Sib( seg, scale, index, base ); return 0;
      case mod32_sib_disp: out = new Mod32SiDisp( seg, scale, index, getimm<int32_t,4>( &code[2] ) ); return 0;
      case mod32_disp: out = new Mod32Disp( seg, getimm<int32_t,4>( &code[1] ) ); return 0;
      case mod32_addr1: out = new Mod32BaseDisp<int8_t>( seg, rm, getimm<int8_t,1>( &code[1] ) ); return 0;
      case mod32_sib_addr1: out = new Mod32SibDisp<int8_t>( seg, scale, index, base, getimm<int8_t,1>( &code[2] ) ); return 0;
      case mod32_addr2: out = new Mod32BaseDisp<int32_t>( seg, rm, getimm<int32_t,4>( &code[1] ) ); return 0;
      case mod32_sib_addr2: out = new Mod32SibDisp<int32_t>( seg, scale, index, base, getimm<int32_t,4>( &code[2] ) ); return 0;
      case mod_reg: out = MkReg( rm ); return 0;
      }
      out = 0; return 0;
    }
    
    uint8_t const* get( GReg& gr, uint8_t const* code ) { gr.idx = gn; return 0; }
    
    uint8_t const* get( EReg& er, uint8_t const* code ) { if (type != mod_reg) throw 0; er.idx = rm; return 0; }
  };
  
  
  struct Moffs
  {
    typedef Moffs this_type;
    template <typename RHS>
    AndSeq<this_type, RHS> operator & ( RHS const& rhs ) { return AndSeq<this_type, RHS>( *this, rhs ); }
    
    uint8_t length, segment;
    
    Moffs() : length() {}
    
    template <typename PROPERTY> uint8_t const* get( PROPERTY& out, uint8_t const* code ) { return code + length; }
    
    uint8_t const* get( CodeBase const& cb, uint8_t const* code )
    {
      length = cb.addrsize() / 8;
      if (length != 4) throw 0;
      segment = cb.segment;
    }
    
    uint8_t const* get( MOp*& out, uint8_t const* code )
    {
      out = new Mod32Disp( segment, getimm<int32_t,4>( &code[0] ) ); return 0;
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

    template <typename PROPERTY> uint8_t const* get( PROPERTY& out, uint8_t const* code ) { return memRM.get( out, code ); }
    uint8_t const* get( CodeBase const& cb, uint8_t const* code )
    {
      code = memRM.get( cb, code );
      if (code and ((memRM.type == RM::mod_reg) xor (REGONLY))) return 0;
      return code;
    }
  };
  
  typedef RM_RegOrMem<true>  RM_reg;
  typedef RM_RegOrMem<false> RM_mem;
    
#include <unisim/component/cxx/processor/intel/isa/branch.hh>
#include <unisim/component/cxx/processor/intel/isa/integer.hh>
#include <unisim/component/cxx/processor/intel/isa/floatingpoint.hh>
#include <unisim/component/cxx/processor/intel/isa/move.hh>
#include <unisim/component/cxx/processor/intel/isa/string.hh>
#include <unisim/component/cxx/processor/intel/isa/special.hh>
#include <unisim/component/cxx/processor/intel/isa/simd.hh>

  template <unsigned COUNT>
  Operation* decode_all( CodeBase const& cb )
  {
    Operation* op = decode<operation_t(COUNT-1)>( cb );
    return op ? op : decode_all<operation_t(COUNT-1)>( cb );
  }
  
  template <>
  Operation* decode_all<0>( CodeBase const& cb )
  {
    return 0;
  }
  
  Operation* getoperation( CodeBase const& cb ) { return decode_all<operation_count>( cb ); }
}

#endif // ISA_TCC
