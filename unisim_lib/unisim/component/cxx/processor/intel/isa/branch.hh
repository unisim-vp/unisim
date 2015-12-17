/* TODO:
 * - In 64 bit mode, for some breanch (if not all) operand-size is forced to 64 whatever the actual operand size is ...
 * - Some instructions may incoherently depend on StackAddressSize and OperandSize (e.g. Enter & Leave)
 * - some branch struct offsets should depend on operand size
 * - the dissambly of non-default targetting mode needs a suffix
 */
template <unsigned OPSIZE>
struct NearCallJ : public Operation
{
  NearCallJ( OpBase const& opbase, int32_t _offset ) : Operation( opbase ), offset( _offset ) {} uint32_t offset;
    
  void disasm( std::ostream& sink ) const { sink << "call " << "0x" << std::hex << (address + length + offset); };
    
  void execute( Arch& arch ) const
  {
    if (OPSIZE != 32) throw 0;
    arch.push<32>( arch.geteip() );
    arch.addeip( u32_t( offset ) );
  }
};
  
template <unsigned OPSIZE>
struct NearCallE : public Operation
{
  NearCallE( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
    
  void disasm( std::ostream& sink ) const { sink << "call *" << DisasmE<OPSIZE>( rmop ); }
    
  void execute( Arch& arch ) const
  {
    if (OPSIZE != 32) throw 0;
    u32_t target = arch.rmread<32>( rmop );
    arch.push<32>( arch.geteip() );
    arch.seteip( target );
  }
};
  
template <unsigned OPSIZE>
struct FarCallA : public Operation
{
  FarCallA( OpBase const& opbase, int32_t _offset, int16_t _seg ) : Operation( opbase ), offset( _offset ), mseg( _seg ) {} int32_t offset; int16_t mseg;
    
  void disasm( std::ostream& sink ) const { sink << "lcall " << "0x" << std::hex << (address + length + offset); };
};
  
template <unsigned OPSIZE>
struct FarCallE : public Operation
{
  FarCallE( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  
  void disasm( std::ostream& sink ) const { sink << "lcall " << '*' << DisasmM( rmop ); }
};
  
template <> Operation* decode<CALL>( CodeBase const& cb )
{
  if (cb.adsz_67 or (cb.rep != 0)) return 0;
    
  if (auto _ = match( cb, OpSize<16>() & opcode( "\xe8" ) & Imm<16>() ))
    
    return new NearCallJ<16>( _.opbase(), _.imm<int32_t>() );
    
  if (auto _ = match( cb, OpSize<32>() & opcode( "\xe8" ) & Imm<32>() ))
    
    return new NearCallJ<32>( _.opbase(), _.imm<int32_t>() );
    
  if (auto _ = match( cb, opcode( "\xff", 2 ) & RM() ))
    {
      if      (cb.opsize() == 16) return new NearCallE<16>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 32) return new NearCallE<32>( _.opbase(), _.rmop() );
      else return 0;
    }
    
  if (auto _ = match( cb, OpSize<16>() & opcode( "\x9a" ) & Imm<16>() & Imm<16>() ))
    
    return new FarCallA<16>( _.opbase(), _.imm<int32_t>( 0 ), _.imm<int16_t>( 1 ) );
    
  if (auto _ = match( cb, OpSize<32>() & opcode( "\x9a" ) & Imm<32>() & Imm<16>() ))
    
    return new FarCallA<32>( _.opbase(), _.imm<int32_t>( 0 ), _.imm<int16_t>( 1 ) );
    
  if (auto _ = match( cb, opcode( "\xff", 3 ) & RM() ))
    {
      if      (cb.opsize() == 16) return new FarCallE<16>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 32) return new FarCallE<32>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 64) return new FarCallE<64>( _.opbase(), _.rmop() );
      else return 0;
    }
    
  return 0;
}

template <unsigned OPSIZE>
struct JccJ : public Operation
{
  JccJ( OpBase const& opbase, uint8_t _cond, int32_t _offset ) : Operation( opbase ), offset( _offset ), cond( _cond ) {} int32_t offset; uint8_t cond;
    
  void disasm( std::ostream& sink ) const { sink << 'j' << DisasmCond( cond ) << " 0x" << std::hex << (address + length + offset); }
    
  void execute( Arch& arch ) const
  {
    if (OPSIZE != 32) throw 0;
    if (mkbool( eval_cond( arch, cond ) )) arch.addeip( u32_t( offset ) ); 
  }
};
  
template <> Operation* decode<JCC>( CodeBase const& cb )
{
  if (auto _ = match( cb, VarByte<0x70,0xF0>() & Imm<8>() ))
    
    {
      if      (cb.opsize() == 16) return new JccJ<16>( _.opbase(), _.vbval(), _.imm<int32_t>() );
      else if (cb.opsize() == 32) return new JccJ<32>( _.opbase(), _.vbval(), _.imm<int32_t>() );
      else return 0;
    }
    
  if (auto _ = match( cb, OpSize<16>() & opcode( "\x0f" ) & VarByte<0x80,0xf0>() & Imm<16>() ))
    
    return new JccJ<16>( _.opbase(), _.vbval(), _.imm<int32_t>() );
    
  if (auto _ = match( cb, OpSize<32>() & opcode( "\x0f" ) & VarByte<0x80,0xf0>() & Imm<32>() ))
    
    return new JccJ<32>( _.opbase(), _.vbval(), _.imm<int32_t>() );
    
  return 0;
}

template <unsigned MOD>
struct Loop : public Operation
{
  int32_t offset;
    
  Loop( OpBase const& opbase, int32_t _offset )
    : Operation( opbase ), offset( _offset )
  {}
    
  void disasm( std::ostream& sink ) const { sink << "loop" << (&"ne\0e\0\0"[MOD*3]) << " 0x" << std::hex << (address + length + offset); };
    
  void execute( Arch& arch ) const
  {
    // Decrement count register
    u32_t count = arch.regread32( 1 ) - u32_t( 1 );
    arch.regwrite32( 1, count );
    // Stop if count is zero
    if (mkbool(count == u32_t(0))) return;
    // or ZF is set (loopne)
    if ((MOD == 0) and (arch.flagread( ZF ) == bit_t( 1 ))) return;
    // or ZF is cleared (loope)
    if ((MOD == 1) and (arch.flagread( ZF ) == bit_t( 0 ))) return;
    // else jump short
    arch.addeip( u32_t( offset ) );
  }
};
  
template <> Operation* decode<LOOP>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xe0" ) & Imm<8>() ))
    
    return new Loop<0>( _.opbase(), _.imm<int32_t>() );
    
  if (auto _ = match( cb, opcode( "\xe1" ) & Imm<8>() ))
    
    return new Loop<1>( _.opbase(), _.imm<int32_t>() );
    
  if (auto _ = match( cb, opcode( "\xe2" ) & Imm<8>() ))
    
    return new Loop<2>( _.opbase(), _.imm<int32_t>() );
    
  return 0;
}

template <unsigned ADDRSZ>
struct Jcxz : public Operation
{
  int32_t offset;
    
  Jcxz( OpBase const& opbase, int32_t _offset ) : Operation( opbase ), offset( _offset ) {}
    
  void disasm( std::ostream& sink ) const { sink << 'j' << (&"\0\0e\0\0\0r"[(ADDRSZ/16)-1]) << "cxz 0x" << std::hex << (address + length + offset); };
    
  void execute( Arch& arch ) const { if (ADDRSZ != 32) throw 0; if (mkbool( arch.regread32( 1 ) == u32_t( 0 ) )) arch.addeip( u32_t( offset ) ); }
};
  
template <> Operation* decode<JCXZ>( CodeBase const& cb )
{
  if (auto _ = match( cb, AddrSize<16>() & opcode( "\xe3" ) & Imm<8>() ))
    
    return new Jcxz<16>( _.opbase(), _.imm<int32_t>() );
    
  if (auto _ = match( cb, AddrSize<32>() & opcode( "\xe3" ) & Imm<8>() ))
    
    return new Jcxz<32>( _.opbase(), _.imm<int32_t>() );
    
  if (auto _ = match( cb, AddrSize<64>() & opcode( "\xe3" ) & Imm<8>() ))
    
    return new Jcxz<64>( _.opbase(), _.imm<int32_t>() );
    
  return 0;
}
  
template <unsigned OPSIZE>
struct JmpE : public Operation
{
  JmpE( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
    
  void disasm( std::ostream& sink ) const { sink << "jmp " << '*' << DisasmE<OPSIZE>( rmop ); }
     
  void execute( Arch& arch ) const { if (OPSIZE != 32) throw 0; arch.seteip( arch.rmread<32>( rmop ) ); }
};

template <unsigned OPSIZE>
struct JmpJ : public Operation
{
  JmpJ( OpBase const& opbase, int32_t _offset ) : Operation( opbase ), offset( _offset ) {} int32_t offset;
  
  void disasm( std::ostream& sink ) const { sink << "jmp 0x" << std::hex << (address + length + offset); }
     
  void execute( Arch& arch ) const { if (OPSIZE != 32) throw 0; arch.addeip( u32_t( offset ) ); }
};
  
template <unsigned OPSIZE>
struct JmpA : public Operation
{
  JmpA( OpBase const& opbase, int32_t _offset, int16_t _seg ) : Operation( opbase ), offset( _offset ), mseg( _seg ) {} int32_t offset; int16_t mseg;
  
  void disasm( std::ostream& sink ) const { sink << "ljmp " << DisasmI( mseg ) << ',' << DisasmI( offset ); }
};
  
template <unsigned OPSIZE>
struct JmpM : public Operation
{
  JmpM( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
    
  void disasm( std::ostream& sink ) const { sink << "ljmp " << '*' << DisasmM( rmop ); }
};
  
template <> Operation* decode<JMP>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xff", 4 ) & RM() ))
    
    {
      if      (cb.opsize() == 16) return new JmpE<16>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 32) return new JmpE<32>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 64) return new JmpE<64>( _.opbase(), _.rmop() );
      else return 0;
    }
    
  if (auto _ = match( cb, opcode( "\xeb" ) & Imm<8>() ))
    
    {
      if      (cb.opsize() == 16) return new JmpJ<16>( _.opbase(), _.imm<int32_t>() );
      else if (cb.opsize() == 32) return new JmpJ<32>( _.opbase(), _.imm<int32_t>() );
      else return 0;
    }
    
  if (auto _ = match( cb, OpSize<16>() & opcode( "\xe9" ) & Imm<16>() ))
    
    return new JmpJ<16>( _.opbase(), _.imm<int32_t>() );
    
  if (auto _ = match( cb, OpSize<32>() & opcode( "\xe9" ) & Imm<32>() ))
    
    return new JmpJ<32>( _.opbase(), _.imm<int32_t>() );
    
  if (auto _ = match( cb, OpSize<16>() & opcode( "\xea" ) & Imm<16>() & Imm<16>() ))
    
    return new JmpA<16>( _.opbase(), _.imm<int32_t>(), _.imm<int16_t>() );
    
  if (auto _ = match( cb, OpSize<32>() & opcode( "\xea" ) & Imm<32>() & Imm<16>() ))
    
    return new JmpA<32>( _.opbase(), _.imm<int32_t>(), _.imm<int16_t>() );
    
  if (auto _ = match( cb, opcode( "\xff", 5 ) & RM() ))
    
    {
      if      (cb.opsize() == 16) return new JmpM<16>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 32) return new JmpM<32>( _.opbase(), _.rmop() );
      else return 0;
    }
    
  return 0;
}
  
template <unsigned OPSIZE>
struct NearReturn : public Operation
{
  NearReturn( OpBase const& opbase ) : Operation( opbase ) {}
    
  void disasm( std::ostream& sink ) const { sink << "ret"; }

  void execute( Arch& arch ) const
  {
    if (OPSIZE != 32) throw 0;
    u32_t src = arch.regread32( 4 );
    arch.regwrite32( 4, src + u32_t( 4 ) );
    arch.seteip( arch.memread32( SS, src ) );
  }
};
  
template <unsigned OPSIZE>
struct NearParamReturn : public Operation
{
  NearParamReturn( OpBase const& opbase, uint16_t _paramsize ) : Operation( opbase ), paramsize( _paramsize ) {} uint16_t paramsize;

  void disasm( std::ostream& sink ) const { sink << "ret " << DisasmI( paramsize ); }

  void execute( Arch& arch ) const
  {
    if (OPSIZE != 32) throw 0;
    u32_t src = arch.regread32( 4 );
    arch.regwrite32( 4, src + u32_t( 4 + paramsize ) );
    arch.seteip( arch.memread32( SS, src ) );
  }
};
  
template <unsigned OPSIZE>
struct FarReturn : public Operation
{
  FarReturn( OpBase const& opbase ) : Operation( opbase ) {}
    
  void disasm( std::ostream& sink ) const { sink << "lret "; }
};
  
template <unsigned OPSIZE>
struct FarParamReturn : public Operation
{
  FarParamReturn( OpBase const& opbase, uint16_t _paramsize ) : Operation( opbase ), paramsize( _paramsize ) {} uint16_t paramsize;

  void disasm( std::ostream& sink ) const { sink << "lret " << DisasmI( paramsize ); }
};
  
template <> Operation* decode<RETURN>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xc3" ) ))
    
    {
      if      (cb.opsize() == 16) return new NearReturn<16>( _.opbase() );
      else if (cb.opsize() == 32) return new NearReturn<32>( _.opbase() );
      else if (cb.opsize() == 64) return new NearReturn<64>( _.opbase() );
      else return 0;
    }
    
  if (auto _ = match( cb, opcode( "\xc2" ) & Imm<16>() ))
    
    {
      if      (cb.opsize() == 16) return new NearParamReturn<16>( _.opbase(), _.imm<uint16_t>() );
      else if (cb.opsize() == 32) return new NearParamReturn<32>( _.opbase(), _.imm<uint16_t>() );
      else if (cb.opsize() == 64) return new NearParamReturn<64>( _.opbase(), _.imm<uint16_t>() );
    }
    
  if (auto _ = match( cb, opcode( "\xcb" ) ))
    
    {
      if      (cb.opsize() == 16) return new FarReturn<16>( _.opbase() );
      else if (cb.opsize() == 32) return new FarReturn<32>( _.opbase() );
      else if (cb.opsize() == 64) return new FarReturn<64>( _.opbase() );
      else return 0;
    }
    
  if (auto _ = match( cb, opcode( "\xca" ) & Imm<16>() ))
    
    {
      if      (cb.opsize() == 16) return new FarParamReturn<16>( _.opbase(), _.imm<uint16_t>() );
      else if (cb.opsize() == 32) return new FarParamReturn<32>( _.opbase(), _.imm<uint16_t>() );
      else if (cb.opsize() == 64) return new FarParamReturn<64>( _.opbase(), _.imm<uint16_t>() );
    }
    
  return 0;
}

template <unsigned OPSIZE>
struct InterruptReturn : public Operation
{
  InterruptReturn( OpBase const& opbase ) : Operation( opbase ) {}

  void disasm( std::ostream& sink ) const { sink << "iret "; }
};

template <> Operation* decode<INTRETURN>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xcf" ) ))

    {
      if      (cb.opsize() == 16) return new InterruptReturn<16>( _.opbase() );
      else if (cb.opsize() == 32) return new InterruptReturn<32>( _.opbase() );
      else if (cb.opsize() == 64) return new InterruptReturn<64>( _.opbase() );
      else return 0;
    }
    
  return 0;
}

struct Interrupt : public Operation
{
  Interrupt( OpBase const& opbase, uint8_t _num ) : Operation( opbase ), vector_number( _num ) {} uint8_t vector_number;

  void disasm( std::ostream& sink ) const { sink << "int " << DisasmI( vector_number ); }

  void execute( Arch& arch ) const { arch.interrupt( vector_number ); }
};

struct Interrupt3 : public Operation
{
  Interrupt3( OpBase const& opbase ) : Operation( opbase ) {}

  void disasm( std::ostream& sink ) const { sink << "int3"; }
};

struct InterruptOF : public Operation
{
  InterruptOF( OpBase const& opbase ) : Operation( opbase ) {}

  void disasm( std::ostream& sink ) const { sink << "into"; }
};

template <> Operation* decode<INTERRUPT>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xcc" ) ))
  
    return new Interrupt3( _.opbase() );
  
  if (auto _ = match( cb, opcode( "\xcd" ) & Imm<8>() ))
  
    return new Interrupt( _.opbase(), _.imm<uint8_t>() );
  
  if (auto _ = match( cb, opcode( "\xce" ) ))
  
    return new Interrupt3( _.opbase() );
  
  return 0;
}

template <unsigned OPSIZE>
struct Enter : public Operation
{
  uint16_t allocsize; uint8_t nestinglevel;
  Enter( OpBase const& opbase, uint16_t _allocsize, uint8_t _nestinglevel )
    : Operation( opbase ), allocsize( _allocsize ), nestinglevel( _nestinglevel ) {}

  void disasm( std::ostream& sink ) const { sink << "enter " << DisasmI( allocsize ) << ',' << DisasmI( nestinglevel ); }
};

template <unsigned OPSIZE/*, unsigned SPSIZE*/>
struct Leave : public Operation
{
  Leave( OpBase const& opbase ) : Operation( opbase ) {}
  
  void disasm( std::ostream& sink ) const { sink << "leave"; }
  
  void execute( Arch& arch ) const {
    /* TODO: STACKSIZE */
    if (OPSIZE != 32) throw 0;
    arch.regwrite32( 4, arch.regread32( 5 ) );
    u32_t src = arch.regread32( 4 );
    arch.regwrite32( 4, src + u32_t( 4 ) );
    arch.regwrite32( 5, arch.memread32( SS, src ) );
  }
};

template <> Operation* decode<ENTER_LEAVE>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xc8" ) & Imm<16>() & Imm<8>() ))
  
    {
      if      (cb.opsize() == 16) return new Enter<16>( _.opbase(), _.imm<uint16_t>(), _.imm<uint8_t>() );
      else if (cb.opsize() == 32) return new Enter<32>( _.opbase(), _.imm<uint16_t>(), _.imm<uint8_t>() );
      else if (cb.opsize() == 64) return new Enter<64>( _.opbase(), _.imm<uint16_t>(), _.imm<uint8_t>() );
      else return 0;
    }
  
  if (auto _ = match( cb, opcode( "\xc9" ) ))
  
    {
      if      (cb.opsize() == 16) return new Leave<16>( _.opbase() );
      else if (cb.opsize() == 32) return new Leave<32>( _.opbase() );
      else if (cb.opsize() == 64) return new Leave<64>( _.opbase() );
    }
  return 0;
}
  
