template <unsigned OPSIZE>
struct PushReg : public Operation
{
  PushReg( OpBase const& opbase, uint8_t _rn ) : Operation( opbase ), rn( _rn ) {} uint8_t rn;
  void disasm( std::ostream& sink ) const { sink << "push " << DisasmR<OPSIZE>( rn ); }
  void execute( Arch& arch ) const { arch.push<OPSIZE>( arch.regread<OPSIZE>( rn ) ); }
};

template <unsigned OPSIZE>
struct PushImm : public Operation
{
  typedef typename TypeFor<OPSIZE>::u imm_type;
  PushImm( OpBase const& opbase, imm_type _imm ) : Operation( opbase ), imm( _imm ) {} imm_type imm;
  void disasm( std::ostream& sink ) const { sink << "push" << ((OPSIZE==16) ? "w " : (OPSIZE==32) ? " " : "q ") << DisasmI( imm ); }
  void execute( Arch& arch ) const { arch.push<OPSIZE>( imm ); }
};

template <unsigned OPSIZE>
struct Push : public Operation
{
  Push( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<16>( "push", rmop ) << DisasmEw( rmop ); }
  void execute( Arch& arch ) const { arch.push<OPSIZE>( arch.rmread<OPSIZE>( rmop ) ); }
};

template <unsigned OPSIZE>
struct PushSeg : public Operation
{
  PushSeg( OpBase const& opbase, uint8_t _seg ) : Operation( opbase ), seg( _seg ) {} uint8_t seg;
  void disasm( std::ostream& sink ) const { sink << "push" << ((OPSIZE==16) ? "w " : (OPSIZE==32) ? " " : "q ") << DisasmS( seg ); }
};

namespace {
  Operation*
  newPushSeg( unsigned opsize, OpBase const& opbase, uint8_t _seg )
  {
    if (opsize==16) return new PushSeg<16>( opbase, _seg );
    if (opsize==32) return new PushSeg<32>( opbase, _seg );
    if (opsize==64) return new PushSeg<64>( opbase, _seg );
    return 0;
  }
}

template <> Operation* decode<PUSH>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xff", 6 ) & RM() ))
  
    {
      if (cb.opsize()==16) return new Push<16>( _.opbase(), _.rmop() );
      if (cb.opsize()==32) return new Push<32>( _.opbase(), _.rmop() );
      if (cb.opsize()==64) return new Push<64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  if (auto _ = match( cb, VarByte<0x50,0xF8>() ))
  
    {
      if (cb.opsize()==16) return new PushReg<16>( _.opbase(), _.vbval() );
      if (cb.opsize()==32) return new PushReg<32>( _.opbase(), _.vbval() );
      if (cb.opsize()==64) return new PushReg<64>( _.opbase(), _.vbval() );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\x6a" ) & Imm<8>() ))
  
    {
      if (cb.opsize()==16) return new PushImm<16>( _.opbase(), _.imm<int16_t>()  );
      if (cb.opsize()==32) return new PushImm<32>( _.opbase(), _.imm<int32_t>()  );
      if (cb.opsize()==64) return new PushImm<64>( _.opbase(), _.imm<int64_t>()  );
      return 0;
    }
  
  if (auto _ = match( cb, OpSize<16>() & opcode( "\x68" ) & Imm<16>() ))
  
    return new PushImm<16>( _.opbase(), _.imm<int16_t>()  );
  
  if (auto _ = match( cb, OpSize<32>() & opcode( "\x68" ) & Imm<32>() ))
  
    return new PushImm<32>( _.opbase(), _.imm<int32_t>()  );
  
  if (auto _ = match( cb, OpSize<64>() & opcode( "\x68" ) & Imm<64>() ))
  
    return new PushImm<64>( _.opbase(), _.imm<int64_t>()  );
  
  if (not cb.mode64()) {
    if (auto _ = match( cb, opcode( "\x06" ) )) return newPushSeg( cb.opsize(), _.opbase(), ES );
    if (auto _ = match( cb, opcode( "\x0e" ) )) return newPushSeg( cb.opsize(), _.opbase(), CS );
    if (auto _ = match( cb, opcode( "\x16" ) )) return newPushSeg( cb.opsize(), _.opbase(), SS );
    if (auto _ = match( cb, opcode( "\x1e" ) )) return newPushSeg( cb.opsize(), _.opbase(), DS );
  }
  
  if (auto _ = match( cb, opcode( "\x0f\xa0" ) )) return newPushSeg( cb.opsize(), _.opbase(), FS );
  if (auto _ = match( cb, opcode( "\x0f\xa8" ) )) return newPushSeg( cb.opsize(), _.opbase(), GS );

  return 0;
}

template <unsigned OPSIZE>
struct PushAll : public Operation
{
  PushAll( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "push" << ((OPSIZE==16) ? "w" : (OPSIZE==32) ? "" : "q"); }
  void execute( Arch& arch ) const {
    typename TypeFor<OPSIZE>::u temp = arch.regread<OPSIZE>( 4 );
    for (int rn = 0; rn < 8; ++rn) arch.push<OPSIZE>( (rn == 4) ? temp : arch.regread<OPSIZE>( 4 ) );
  }
};

template <> Operation* decode<PUSHA>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\x60" ) ))
  
    {
      if (cb.opsize()==16) return new PushAll<16>( _.opbase() );
      if (cb.opsize()==32) return new PushAll<32>( _.opbase() );
      if (cb.opsize()==64) return new PushAll<64>( _.opbase() );
      return 0;
    }
  
  return 0;
} 

template <unsigned OPSIZE>
struct Pushf : public Operation
{
  Pushf( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "pushf" << ((OPSIZE==16) ? "w" : (OPSIZE==32) ? "" : "q"); }
  // void execute( Arch& arch ) const {
  //   u32_t dst = arch.regread32( 4 ) - u32_t( OPSIZE/8 );
  //   arch.memwrite<OPSIZE>( segment, dst, eflagsread<OPSIZE>( arch ) );
  //   arch.regwrite32( 4, dst );
  // }
};

template <> Operation* decode<PUSHF>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\x9c" ) ))
  
    {
      if (cb.opsize()==16) return new Pushf<16>( _.opbase() );
      if (cb.opsize()==32) return new Pushf<32>( _.opbase() );
      if (cb.opsize()==64) return new Pushf<64>( _.opbase() );
    }
  
  return 0;
}

template <unsigned OPSIZE>
struct PopReg : public Operation
{
  PopReg( OpBase const& opbase, uint8_t _rn ) : Operation( opbase ), rn( _rn ) {} uint8_t rn;
  void disasm( std::ostream& sink ) const { sink << "pop " << DisasmR<OPSIZE>( rn ); }
  void execute( Arch& arch ) const { arch.regwrite<OPSIZE>( rn, arch.pop<OPSIZE>() ); }
};

template <unsigned OPSIZE>
struct Pop : public Operation
{
  Pop( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "pop", rmop ) << DisasmE<OPSIZE>( rmop ); }
  void execute( Arch& arch ) const { arch.rmwrite<OPSIZE>( rmop, arch.pop<OPSIZE>() ); }
};

template <unsigned OPSIZE>
struct PopSeg : public Operation
{
  PopSeg( OpBase const& opbase, uint8_t _seg ) : Operation( opbase ), seg( _seg ) {} uint8_t seg;
  void disasm( std::ostream& sink ) const { sink << "pop"  << ((OPSIZE==16) ? "w " : (OPSIZE==32) ? " " : "q ") << DisasmS( seg ); }
};

namespace {
  Operation*
  newPopSeg( unsigned opsize, OpBase const& opbase, uint8_t _seg )
  {
    if (opsize==16) return new PopSeg<16>( opbase, _seg );
    if (opsize==32) return new PopSeg<32>( opbase, _seg );
    if (opsize==64) return new PopSeg<64>( opbase, _seg );
  }
}

template <> Operation* decode<POP>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\x8f", 0 ) & RM() ))
  
    {
      if (cb.opsize()==16) return new Pop<16>( _.opbase(), _.rmop() );
      if (cb.opsize()==32) return new Pop<32>( _.opbase(), _.rmop() );
      if (cb.opsize()==64) return new Pop<64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  if (auto _ = match( cb, VarByte<0x58,0xF8>() ))
  
    {
      if (cb.opsize()==16) return new PopReg<16>( _.opbase(), _.vbval() );
      if (cb.opsize()==32) return new PopReg<32>( _.opbase(), _.vbval() );
      if (cb.opsize()==64) return new PopReg<64>( _.opbase(), _.vbval() );
      return 0;
    }
  
  if (not cb.mode64()) {
    if (auto _ = match( cb, opcode( "\x07" ) )) return newPopSeg( cb.opsize(), _.opbase(), ES );
    if (auto _ = match( cb, opcode( "\x17" ) )) return newPopSeg( cb.opsize(), _.opbase(), SS );
    if (auto _ = match( cb, opcode( "\x1f" ) )) return newPopSeg( cb.opsize(), _.opbase(), DS );
  }
  
  if (auto _ = match( cb, opcode( "\x0f\xa1" ) )) return newPopSeg( cb.opsize(), _.opbase(), FS );
  if (auto _ = match( cb, opcode( "\x0f\xa9" ) )) return newPopSeg( cb.opsize(), _.opbase(), GS );

  return 0;
}

template <unsigned OPSIZE>
struct PopAll : public Operation
{
  PopAll( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "popa" << ((OPSIZE==16) ? "w" : (OPSIZE==32) ? "" : "q"); }
  void execute( Arch& arch ) const {
    for (int rn = 0; rn < 8; ++rn) {
      typename TypeFor<OPSIZE>::u value = arch.pop<OPSIZE>();
      if (rn != 4) arch.regwrite<OPSIZE>( rn, value );
    }
  }
};

template <> Operation* decode<POPA>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\x61" ) ))
  
    {
      if (cb.opsize()==16) return new PopAll<16>( _.opbase() );
      if (cb.opsize()==32) return new PopAll<32>( _.opbase() );
      if (cb.opsize()==64) return new PopAll<64>( _.opbase() );
      return 0;
    }
  
  return 0;
}

template <unsigned OPSIZE>
struct Popf : public Operation
{
  Popf( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "popf"; }
  // void execute( Arch& arch ) const {
  //   u32_t src = arch.regread32( 4 );
  //   arch.regwrite32( 4, src + u32_t( OPSIZE/8 ) );
  //   u16_t word = arch.memread<16>( SS, src );
  //   arch.flagwrite( CF, bit_t( (word >>  0) & u32_t( 1 ) ) );
  //   arch.flagwrite( PF, bit_t( (word >>  2) & u32_t( 1 ) ) );
  //   arch.flagwrite( AF, bit_t( (word >>  4) & u32_t( 1 ) ) );
  //   arch.flagwrite( ZF, bit_t( (word >>  6) & u32_t( 1 ) ) );
  //   arch.flagwrite( SF, bit_t( (word >>  7) & u32_t( 1 ) ) );
  //   arch.flagwrite( DF, bit_t( (word >> 10) & u32_t( 1 ) ) );
  //   arch.flagwrite( OF, bit_t( (word >> 11) & u32_t( 1 ) ) );
  // }
};

template <> Operation* decode<POPF>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\x9d" ) ))
  
    {
      if (cb.opsize()==16) return new Popf<16>( _.opbase() );
      if (cb.opsize()==32) return new Popf<32>( _.opbase() );
      if (cb.opsize()==64) return new Popf<64>( _.opbase() );
    }
  
  return 0;
}

template <unsigned OPSIZE, bool GTOE>
struct MovRM : public Operation
{
  MovRM( OpBase const& opbase, MOp* _rmop, uint8_t _gn ) : Operation( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const {
    if (GTOE) sink << "mov " << DisasmR<OPSIZE>( gn ) << ',' << DisasmE<OPSIZE>( rmop );
    else      sink << "mov " << DisasmE<OPSIZE>( rmop ) << ',' << DisasmR<OPSIZE>( gn );
  }
  void execute( Arch& arch ) const {
    if (GTOE) arch.rmwrite<OPSIZE>( rmop, arch.regread<OPSIZE>( gn ) );
    else      arch.regwrite<OPSIZE>( gn, arch.rmread<OPSIZE>( rmop ) ); 
  }
};

template <unsigned OPSIZE>
struct MovImm : public Operation
{
  typedef typename TypeFor<OPSIZE>::u imm_type;
  MovImm( OpBase const& opbase, MOp* _rmop, imm_type _imm ) : Operation( opbase ), rmop( _rmop ), imm( _imm ) {} RMOp rmop; imm_type imm;
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "mov", rmop ) << DisasmI( imm ) << ',' << DisasmE<OPSIZE>( rmop ); }
  void execute( Arch& arch ) const { arch.rmwrite<OPSIZE>( rmop, imm ); }
};

template <unsigned OPSIZE, bool STOE>
struct MovSeg : public Operation
{
  MovSeg( OpBase const& opbase, MOp* _rmop, uint8_t _seg ) : Operation( opbase ), rmop( _rmop ), seg( _seg ) {} uint8_t seg; RMOp rmop;
  void disasm( std::ostream& sink ) const {
    if (STOE) sink << "mov " << DisasmS( seg ) << ',' << DisasmE<OPSIZE>( rmop );
    else      sink << "mov " << DisasmE<OPSIZE>( rmop ) << ',' << DisasmS( seg );
  }
  void execute( Arch& arch ) const {
    if (STOE) arch.rmwrite<OPSIZE>( rmop, typename TypeFor<OPSIZE>::u( arch.segregread( seg ) ) );
    else      arch.segregwrite( seg, u16_t( arch.rmread<OPSIZE>( rmop ) ) );
  }
};

template <> Operation* decode<MOV>( CodeBase const& cb )
{
  // MOV -- Move
  if (auto _ = match( cb, opcode( "\x88" ) & RM() ))
  
    return new MovRM<8,true>( _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( cb, opcode( "\x89" ) & RM() ))
  
    {
      if (cb.opsize()==16) return new MovRM<16,true>( _.opbase(), _.rmop(), _.greg() );
      if (cb.opsize()==32) return new MovRM<32,true>( _.opbase(), _.rmop(), _.greg() );
      if (cb.opsize()==64) return new MovRM<64,true>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\x8a" ) & RM() ))
  
    return new MovRM<8,false>( _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( cb, opcode( "\x8b" ) & RM() ))
  
    {
      if (cb.opsize()==16) return new MovRM<16,false>( _.opbase(), _.rmop(), _.greg() );
      if (cb.opsize()==32) return new MovRM<32,false>( _.opbase(), _.rmop(), _.greg() );
      if (cb.opsize()==64) return new MovRM<64,false>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\x8c" ) & RM() ))
  
    {
      if (cb.opsize()==16) return new MovSeg<16,true>( _.opbase(), _.rmop(), _.greg() );
      if (cb.opsize()==32) return new MovSeg<32,true>( _.opbase(), _.rmop(), _.greg() );
      if (cb.opsize()==64) return new MovSeg<64,true>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\x8e" ) & RM() ))

    {
      if (cb.opsize()==16) return new MovSeg<16,false>( _.opbase(), _.rmop(), _.greg() );
      if (cb.opsize()==32) return new MovSeg<32,false>( _.opbase(), _.rmop(), _.greg() );
      if (cb.opsize()==64) return new MovSeg<64,false>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\xa0" ) & Moffs() ))
  
    return new MovRM<8,false>( _.opbase(), _.rmop(), 0 );
  
  if (auto _ = match( cb, opcode( "\xa1" ) & Moffs() ))
  
    {
      if (cb.opsize()==16) return new MovRM<16,false>( _.opbase(), _.rmop(), 0 );
      if (cb.opsize()==32) return new MovRM<32,false>( _.opbase(), _.rmop(), 0 );
      if (cb.opsize()==64) return new MovRM<64,false>( _.opbase(), _.rmop(), 0 );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\xa2" ) & Moffs() ))
  
    return new MovRM<8,true>( _.opbase(), _.rmop(), 0 );

  if (auto _ = match( cb, opcode( "\xa3" ) & Moffs() ))

    {
      if (cb.opsize()==16) return new MovRM<16,true>( _.opbase(), _.rmop(), 0 );
      if (cb.opsize()==32) return new MovRM<32,true>( _.opbase(), _.rmop(), 0 );
      if (cb.opsize()==64) return new MovRM<64,true>( _.opbase(), _.rmop(), 0 );
      return 0;
    }
  
  if (auto _ = match( cb, VarByte<0xb0,0xf8>() & Imm<8>() ))
  
    return new MovImm<8>( _.opbase(), RM::MkReg( _.vbval() ), _.imm<int8_t>() );
  
  if (auto _ = match( cb, OpSize<16>() & VarByte<0xb8,0xf8>() & Imm<16>() ))
  
    return new MovImm<16>( _.opbase(), RM::MkReg( _.vbval() ), _.imm<int16_t>() );
  
  if (auto _ = match( cb, OpSize<32>() & VarByte<0xb8,0xf8>() & Imm<32>() ))
  
    return new MovImm<32>( _.opbase(), RM::MkReg( _.vbval() ), _.imm<int32_t>() );
  
  if (auto _ = match( cb, OpSize<64>() & VarByte<0xb8,0xf8>() & Imm<32>() ))
  
    return new MovImm<64>( _.opbase(), RM::MkReg( _.vbval() ), _.imm<int64_t>() );
  
  if (auto _ = match( cb, opcode( "\xc6", 0 ) & RM() & Imm<8>() ))
  
    return new MovImm<8>( _.opbase(), _.rmop(), _.imm<int8_t>() );
  
  if (auto _ = match( cb, OpSize<16>() & opcode( "\xc7", 0 ) & RM() & Imm<16>() ))
  
    return new MovImm<16>( _.opbase(), _.rmop(), _.imm<int16_t>() );
  
  if (auto _ = match( cb, OpSize<32>() & opcode( "\xc7", 0 ) & RM() & Imm<32>() ))
  
    return new MovImm<32>( _.opbase(), _.rmop(), _.imm<int32_t>() );
  
  if (auto _ = match( cb, OpSize<64>() & opcode( "\xc7", 0 ) & RM() & Imm<32>() ))
  
    return new MovImm<64>( _.opbase(), _.rmop(), _.imm<int64_t>() );
  
  return 0;
}

template <unsigned SOPSIZE, unsigned DOPSIZE>
struct Movzx : public Operation
{
  Movzx( OpBase const& opbase, MOp* _rmop, uint8_t _gn ) : Operation( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const {
    sink << "movz" << ((SOPSIZE==8) ? 'b' : 'w') << ((DOPSIZE==16) ? 'w' : (DOPSIZE==32) ? 'l' : 'q') << ' ' << DisasmE<SOPSIZE>( rmop ) << ',' << DisasmR<DOPSIZE>( gn );
  }
  void execute( Arch& arch ) const { arch.regwrite<DOPSIZE>( gn, typename TypeFor<DOPSIZE>::u( arch.rmread<SOPSIZE>( rmop ) ) ); }
};

template <> Operation* decode<MOVZX>( CodeBase const& cb )
{
  // MOVZX -- Move with Zero-Extend
  if (auto _ = match( cb, opcode( "\x0f\xb6" ) & RM() ))
  
    {
      if (cb.opsize()==16) return new Movzx<8,16>( _.opbase(), _.rmop(), _.greg() );
      if (cb.opsize()==32) return new Movzx<8,32>( _.opbase(), _.rmop(), _.greg() );
      if (cb.opsize()==64) return new Movzx<8,64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\x0f\xb7" ) & RM() ))
  
    {
      if (cb.opsize()==32) return new Movzx<16,32>( _.opbase(), _.rmop(), _.greg() );
      if (cb.opsize()==64) return new Movzx<16,64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  return 0;
}

template <unsigned SOPSIZE, unsigned DOPSIZE>
struct Movsx : public Operation
{
  Movsx( OpBase const& opbase, MOp* _rmop, uint8_t _gn ) : Operation( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const {
    sink << "movs" << ((SOPSIZE==8) ? 'b' : 'w') << ((DOPSIZE==16) ? 'w' : (DOPSIZE==32) ? 'l' : 'q') << ' ' << DisasmE<SOPSIZE>( rmop ) << ',' << DisasmR<DOPSIZE>( gn );
  }
  typedef typename TypeFor<SOPSIZE>::s ssrc_type;
  typedef typename TypeFor<DOPSIZE>::u udst_type;
  typedef typename TypeFor<DOPSIZE>::s sdst_type;
  void execute( Arch& arch ) const { arch.regwrite<DOPSIZE>( gn, udst_type( sdst_type( ssrc_type( arch.rmread<SOPSIZE>( rmop ) ) ) ) ); }
};

template <> Operation* decode<MOVSX>( CodeBase const& cb )
{
  // MOVSX -- Move with Zero-Extend
  if (auto _ = match( cb, opcode( "\x0f\xbe" ) & RM() ))
  
    {
      if (cb.opsize()==16) return new Movsx<8,16>( _.opbase(), _.rmop(), _.greg() );
      if (cb.opsize()==32) return new Movsx<8,32>( _.opbase(), _.rmop(), _.greg() );
      if (cb.opsize()==64) return new Movsx<8,64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\x0f\xbf" ) & RM() ))
  
    {
      if (cb.opsize()==32) return new Movsx<16,32>( _.opbase(), _.rmop(), _.greg() );
      if (cb.opsize()==64) return new Movsx<16,64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  if (auto _ = match( cb, OpSize<64>() & opcode( "\x63" ) & RM() ))
  
    return new Movsx<32,64>( _.opbase(), _.rmop(), _.greg() );
  
  return 0;
}

struct WriteDF : public Operation
{
  WriteDF( OpBase const& opbase, uint8_t _df ) : Operation( opbase ), df( _df ) {} uint8_t df;
  void disasm( std::ostream& sink ) const { sink << (df ? "std" : "cld"); }
  void execute( Arch& arch ) const { arch.flagwrite( DF, bit_t( df ) ); }
};

template <> Operation* decode<STD>( CodeBase const& cb )
{
  if (auto _ = match( cb, VarByte<0xfc,0xfe>() ))
    
    return new WriteDF( _.opbase(), _.vbval() );
  
  return 0;
}

struct Arpl : public Operation
{
  Arpl( OpBase const& opbase, MOp* _rmop, uint8_t _gn ) : Operation( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "arpl " << DisasmR<16>( gn ) << ',' << DisasmE<16>( rmop ); }
};

template <> Operation* decode<ARPL>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\x63" ) & RM() ))
    return new Arpl( _.opbase(), _.rmop(), _.greg() );
  return 0;
}

template <unsigned OPSIZE>
struct CmpXchg : public Operation
{
  CmpXchg( OpBase const& opbase, MOp* _rmop, uint8_t _gn ) : Operation( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "cmpxchg " << DisasmR<OPSIZE>( gn ) << ',' << DisasmE<OPSIZE>( rmop ); }
  
  void execute( Arch& arch ) const
  {
    typename TypeFor<OPSIZE>::u mem_operand = arch.rmread<OPSIZE>( rmop );
    bit_t equal = (arch.regread<OPSIZE>( 0 ) == mem_operand);
    arch.flagwrite( ZF, equal );
    if (mkbool( equal )) arch.rmwrite<OPSIZE>( rmop, arch.regread<OPSIZE>( gn ) );
    else                 arch.regwrite<OPSIZE>( 0, mem_operand );
  }
};

template <> Operation* decode<CMPXCHG>( CodeBase const& cb )
{
  // CMPXCHG -- Compare and Exchange
  if (auto _ = match( cb, opcode( "\x0f\xb0" ) & RM() ))
    return new CmpXchg<8>( _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( cb, opcode( "\x0f\xb1" ) & RM() ))
    {
      if (cb.opsize()==16) return new CmpXchg<16>( _.opbase(), _.rmop(), _.greg() );
      if (cb.opsize()==32) return new CmpXchg<32>( _.opbase(), _.rmop(), _.greg() );
      if (cb.opsize()==64) return new CmpXchg<64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  return 0;
}

template <unsigned BOPSIZE>
struct CmpXchgBytes : public Operation
{
  CmpXchgBytes( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  void disasm( std::ostream& sink ) const { sink << "cmpxchg" << BOPSIZE << "b " << DisasmM( rmop ); }
};

template <> Operation* decode<CMPXCHG8B>( CodeBase const& cb )
{
  // CMPXCHG8B/CMPXCHG16B -- Compare and Exchange Bytes
  if (auto _ = match( cb, opcode( "\x0f\xc7", 1 ) & RM_mem() ))
  
    {
      if (cb.opsize()==32) return new CmpXchgBytes<8>( _.opbase(), _.rmop() );
      if (cb.opsize()==64) return new CmpXchgBytes<16>( _.opbase(), _.rmop() );
      return 0;
    }
  
  return 0;
}

template <unsigned OPSIZE>
struct Xchg : public Operation
{
  Xchg( OpBase const& opbase, MOp* _rmop, uint8_t _gn ) : Operation( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "xchg " << DisasmR<OPSIZE>( gn ) << ',' << DisasmE<OPSIZE>( rmop ); }
  typedef typename TypeFor<OPSIZE>::u u_type;
  void execute( Arch& arch ) const {
    u_type a = arch.regread<OPSIZE>( gn );
    u_type b = arch.rmread<OPSIZE>( rmop );
    arch.regwrite<OPSIZE>( gn, b );
    arch.rmwrite<OPSIZE>( rmop, a );
  }
};

template <> Operation* decode<XCHG>( CodeBase const& cb )
{
  if (auto _ = match( cb, VarByte<0x90,0xF8>() ))
  
    {
      if (cb.opsize()==16) return new Xchg<16>( _.opbase(), RM::MkReg( _.vbval() ), 0 );
      if (cb.opsize()==32) return new Xchg<32>( _.opbase(), RM::MkReg( _.vbval() ), 0 ); 
      if (cb.opsize()==64) return new Xchg<64>( _.opbase(), RM::MkReg( _.vbval() ), 0 );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\x86" ) & RM() ))
  
    return new Xchg<8>( _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( cb, opcode( "\x87" ) & RM() ))
  
    {
      if (cb.opsize()==16) return new Xchg<16>( _.opbase(), _.rmop(), _.greg() );
      if (cb.opsize()==32) return new Xchg<32>( _.opbase(), _.rmop(), _.greg() );
      if (cb.opsize()==64) return new Xchg<64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }

  return 0;
}

// TODO: Special nops processing ?
// struct Nop : public Operation
// {
//   Nop( OpBase const& opbase ) : Operation( opbase ) {}
//   void disasm( std::ostream& sink ) const { sink << "nop"; }
//   void execute( Arch& arch ) const {}
// };

// struct Nopw : public Operation
// {
//   Nopw( OpBase const& opbase ) : Operation( opbase ) {}
//   void disasm( std::ostream& sink ) const { sink << "nopw"; }
//   // void execute( Arch& arch ) const {}
// };

// struct Nop_ed : public Operation
// {
//   Nop_ed( OpBase const& opbase, MOp* _rmop, uint8_t _gn ) : Operation( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp rmop; uint8_t gn;
//   void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<32>( "nop", rmop ) << DisasmE<32>( rmop ); }
//   // void execute( Arch& arch ) const {}
// };

// struct Nop_ew : public Operation
// {
//   Nop_ew( OpBase const& opbase, MOp* _rmop, uint8_t _gn ) : Operation( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp rmop; uint8_t gn;
//   void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<16>( "nop", rmop ) << DisasmE<16>( rmop ); }
//   // void execute( Arch& arch ) const {}
// };
// op nop( 0x90[8] )
// op nopw( 0x66[8]:> <:0x90[8] );
// op nop_ed( 0x0f[8]:> <:0x1f[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*rmop[Rmop] );
// op nop_ew( 0x66[8]:> <:0x0f[8]:> <:0x1f[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*rmop[Rmop] );


template <unsigned OPSIZE>
struct XAddEG : public Operation
{
  XAddEG( OpBase const& opbase, MOp* _rmop, uint8_t _gn ) : Operation( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "xadd " << DisasmR<OPSIZE>( gn ) << ',' << DisasmE<OPSIZE>( rmop ); }
  typedef typename TypeFor<OPSIZE>::u u_type;
  // void execute( Arch& arch ) const {
  //   u_type a = arch.rmread<OPSIZE>( rmop );
  //   u_type b = arch.regread<OPSIZE>( gn );
  //   arch.rmwrite<OPSIZE>( rmop, a + b  );
  //   arch.regwrite<OPSIZE>( gn, a );
  // }
};

template <> Operation* decode<XADD>( CodeBase const& cb )
{
  // XADD -- Exchange and Add
  if (auto _ = match( cb, opcode( "\x0f\xc0" ) & RM() ))
  
    return new XAddEG<8>( _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( cb, opcode( "\x0f\xc1" ) & RM() ))
  
    {
      if (cb.opsize()==16) return new XAddEG<16>( _.opbase(), _.rmop(), _.greg() );
      if (cb.opsize()==32) return new XAddEG<32>( _.opbase(), _.rmop(), _.greg() );
      if (cb.opsize()==64) return new XAddEG<64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  return 0;
}

template <unsigned OPSIZE>
struct Cmovcc : public Operation
{
  Cmovcc( OpBase const& opbase, MOp* _rmop, uint8_t _gn, uint8_t _cc )
    : Operation( opbase ), rmop( _rmop ), gn( _gn ), cc( _cc ) {} RMOp rmop; uint8_t gn; uint8_t cc; 
  void disasm( std::ostream& sink ) const { sink << "cmov" << DisasmCond( cc ) << ' ' << DisasmE<OPSIZE>( rmop ) << ',' << DisasmR<OPSIZE>( gn ); }
  void execute( Arch& arch ) const { if (mkbool( eval_cond( arch, cc ) )) { arch.regwrite<OPSIZE>( gn, arch.rmread<OPSIZE>( rmop ) ); } }
};

template <> Operation* decode<CMOVCC>( CodeBase const& cb )
{
  // CMOVcc -- Conditional Move
  if (auto _ = match( cb, opcode( "\x0f" ) & VarByte<0x40,0xF0>() & RM() ))
  
    {
      if (cb.opsize()==16) return new Cmovcc<16>( _.opbase(), _.rmop(), _.greg(), _.vbval() );
      if (cb.opsize()==32) return new Cmovcc<32>( _.opbase(), _.rmop(), _.greg(), _.vbval() );
      if (cb.opsize()==64) return new Cmovcc<64>( _.opbase(), _.rmop(), _.greg(), _.vbval() );
      return 0;
    }
  
  return 0;
}

template <unsigned OPSIZE>
struct BtImm : public Operation
{
  BtImm( OpBase const& opbase, MOp* _rmop, uint8_t _imm ) : Operation( opbase ), rmop( _rmop ), imm( _imm ) {} RMOp rmop; uint8_t imm;
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "bt", rmop ) << DisasmI( imm ) << ',' << DisasmE<OPSIZE>( rmop ); }
  typedef typename TypeFor<OPSIZE>::u u_type;
  // void execute( Arch& arch ) const {
  //   unsigned bitoffset = imm % OPSIZE;
  //   u_type opr = arch.rmread<OPSIZE>( rmop )
  //   arch.flagwrite( CF, bit_t( (opr >> bitoffset) & u_type( 1 ) ) );
  // }
};

template <unsigned OPSIZE>
struct BtRM : public Operation
{
  BtRM( OpBase const& opbase, MOp* _rmop, uint8_t _gn ) : Operation( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "bt " << DisasmR<OPSIZE>( gn ) << ',' << DisasmE<OPSIZE>( rmop ); }
  typedef typename TypeFor<OPSIZE>::u u_type;
  typedef typename TypeFor<OPSIZE>::s s_type;
  // void execute( Arch& arch ) const {
  //   s_type str_bit = s_type( arch.regread<OPSIZE>( _gn ) );
  //   int64_t addr_offset = int64_t( (bt_offset >> TypeFor<OPSIZE>::logsize) * (OPSIZE / 8) );
  //   u_type str_opr = arch.rmstrread<OPSIZE>( rmop, addr_offset );
  //   u_type opr_bit = u_type( str_bit % OPSIZE );
  //   arch.flagwrite( CF, bit_t( (str_opr >> opr_bit) & u_type( 1 ) ) );
  // }
};

template <> Operation* decode<BT>( CodeBase const& cb )
{
  // BT -- BitOpcode
  if (auto _ = match( cb, opcode( "\x0f\xba", 4 ) & RM() & Imm<8>() ))
  
    {
      if (cb.opsize()==16) return new BtImm<16>( _.opbase(), _.rmop(), _.imm<uint8_t>() );
      if (cb.opsize()==32) return new BtImm<32>( _.opbase(), _.rmop(), _.imm<uint8_t>() );
      if (cb.opsize()==64) return new BtImm<64>( _.opbase(), _.rmop(), _.imm<uint8_t>() );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\x0f\xa3" ) & RM() ))
  
    {
      if (cb.opsize()==16) return new BtRM<16>( _.opbase(), _.rmop(), _.greg() );
      if (cb.opsize()==32) return new BtRM<32>( _.opbase(), _.rmop(), _.greg() );
      if (cb.opsize()==64) return new BtRM<64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  return 0;
}

template <unsigned OPSIZE>
struct BtcImm : public Operation
{
  BtcImm( OpBase const& opbase, MOp* _rmop, uint8_t _imm ) : Operation( opbase ), rmop( _rmop ), imm( _imm ) {} RMOp rmop; uint8_t imm;
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "btc", rmop ) << DisasmI( imm ) << ',' << DisasmE<OPSIZE>( rmop ); }
  typedef typename TypeFor<OPSIZE>::u u_type;
  // void execute( Arch& arch ) const {
  //   unsigned bitoffset = imm % OPSIZE;
  //   u_type opr = arch.rmread<OPSIZE>( rmop )
  //   arch.flagwrite( CF, bit_t( (opr >> bitoffset) & u_type( 1 ) ) );
  //   arch.rmwrite<OPSIZE>( rmop, opr ^ (u_type( 1 ) << bitoffset) );
  // }
};

template <unsigned OPSIZE>
struct BtcRM : public Operation
{
  BtcRM( OpBase const& opbase, MOp* _rmop, uint8_t _gn ) : Operation( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "btc " << DisasmR<OPSIZE>( gn ) << ',' << DisasmE<OPSIZE>( rmop ); }
  typedef typename TypeFor<OPSIZE>::u u_type;
  typedef typename TypeFor<OPSIZE>::s s_type;
  // void execute( Arch& arch ) const {
  //   s_type str_bit = s_type( arch.regread<OPSIZE>( _gn ) );
  //   int64_t addr_offset = int64_t( (bt_offset >> TypeFor<OPSIZE>::logsize) * (OPSIZE / 8) );
  //   u_type str_opr = arch.rmstrread<OPSIZE>( rmop, addr_offset );
  //   u_type opr_bit = u_type( str_bit % OPSIZE );
  //   arch.flagwrite( CF, bit_t( (str_opr >> opr_bit) & u_type( 1 ) ) );
  //   arch.rmstrwrite<OPSIZE>( rmop, addr_offset, str_opr ^ (u_type( 1 ) << opr_bit) );
  // }
};

template <> Operation* decode<BTC>( CodeBase const& cb )
{
  // BTC -- Bit Test and Complement
  
  if (auto _ = match( cb, opcode( "\x0f\xba", 7 ) & RM() & Imm<8>() ))
  
    {
      if (cb.opsize()==16) return new BtcImm<16>( _.opbase(), _.rmop(), _.imm<uint8_t>() );
      if (cb.opsize()==32) return new BtcImm<32>( _.opbase(), _.rmop(), _.imm<uint8_t>() );
      if (cb.opsize()==64) return new BtcImm<64>( _.opbase(), _.rmop(), _.imm<uint8_t>() );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\x0f\xbb" ) & RM() ))
  
    {
      if (cb.opsize()==16) return new BtcRM<16>( _.opbase(), _.rmop(), _.greg() );
      if (cb.opsize()==32) return new BtcRM<32>( _.opbase(), _.rmop(), _.greg() );
      if (cb.opsize()==64) return new BtcRM<64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  return 0;
}

template <unsigned OPSIZE>
struct BtrImm : public Operation
{
  BtrImm( OpBase const& opbase, MOp* _rmop, uint8_t _imm ) : Operation( opbase ), rmop( _rmop ), imm( _imm ) {} RMOp rmop; uint8_t imm;
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "btr", rmop ) << DisasmI( imm ) << ',' << DisasmE<OPSIZE>( rmop ); }
  typedef typename TypeFor<OPSIZE>::u u_type;
  // void execute( Arch& arch ) const {
  //   unsigned bitoffset = imm % OPSIZE;
  //   u_type opr = arch.rmread<OPSIZE>( rmop )
  //   arch.flagwrite( CF, bit_t( (opr >> bitoffset) & u_type( 1 ) ) );
  //   arch.rmwrite<OPSIZE>( rmop, opr & ~(u_type( 1 ) << bitoffset) );
  // }
};

template <unsigned OPSIZE>
struct BtrRM : public Operation
{
  BtrRM( OpBase const& opbase, MOp* _rmop, uint8_t _gn ) : Operation( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "btc " << DisasmR<OPSIZE>( gn ) << ',' << DisasmE<OPSIZE>( rmop ); }
  typedef typename TypeFor<OPSIZE>::u u_type;
  typedef typename TypeFor<OPSIZE>::s s_type;
  // void execute( Arch& arch ) const {
  //   s_type str_bit = s_type( arch.regread<OPSIZE>( _gn ) );
  //   int64_t addr_offset = int64_t( (bt_offset >> TypeFor<OPSIZE>::logsize) * (OPSIZE / 8) );
  //   u_type str_opr = arch.rmstrread<OPSIZE>( rmop, addr_offset );
  //   u_type opr_bit = u_type( str_bit % OPSIZE );
  //   arch.flagwrite( CF, bit_t( (str_opr >> opr_bit) & u_type( 1 ) ) );
  //   arch.rmstrwrite<OPSIZE>( rmop, addr_offset, str_opr & ~(u_type( 1 ) << opr_bit) );
  // }
};

template <> Operation* decode<BTR>( CodeBase const& cb )
{
  // BTR -- Bit Test and Reset
  
  if (auto _ = match( cb, opcode( "\x0f\xba", 6 ) & RM() & Imm<8>() ))
  
    {
      if (cb.opsize()==16) return new BtrImm<16>( _.opbase(), _.rmop(), _.imm<uint8_t>() );
      if (cb.opsize()==32) return new BtrImm<32>( _.opbase(), _.rmop(), _.imm<uint8_t>() );
      if (cb.opsize()==64) return new BtrImm<64>( _.opbase(), _.rmop(), _.imm<uint8_t>() );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\x0f\xb3" ) & RM() ))
  
    {
      if (cb.opsize()==16) return new BtrRM<16>( _.opbase(), _.rmop(), _.greg() );
      if (cb.opsize()==32) return new BtrRM<32>( _.opbase(), _.rmop(), _.greg() );
      if (cb.opsize()==64) return new BtrRM<64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  return 0;
}

template <unsigned OPSIZE>
struct BtsImm : public Operation
{
  BtsImm( OpBase const& opbase, MOp* _rmop, uint8_t _imm ) : Operation( opbase ), rmop( _rmop ), imm( _imm ) {} RMOp rmop; uint8_t imm;
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "bts", rmop ) << DisasmI( imm ) << ',' << DisasmE<OPSIZE>( rmop ); }
  typedef typename TypeFor<OPSIZE>::u u_type;
  // void execute( Arch& arch ) const {
  //   unsigned bitoffset = imm % OPSIZE;
  //   u_type opr = arch.rmread<OPSIZE>( rmop )
  //   arch.flagwrite( CF, bit_t( (opr >> bitoffset) & u_type( 1 ) ) );
  //   arch.rmwrite<OPSIZE>( rmop, opr | (u_type( 1 ) << bitoffset) );
  // }
};

template <unsigned OPSIZE>
struct BtsRM : public Operation
{
  BtsRM( OpBase const& opbase, MOp* _rmop, uint8_t _gn ) : Operation( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "btc " << DisasmR<OPSIZE>( gn ) << ',' << DisasmE<OPSIZE>( rmop ); }
  typedef typename TypeFor<OPSIZE>::u u_type;
  typedef typename TypeFor<OPSIZE>::s s_type;
  // void execute( Arch& arch ) const {
  //   s_type str_bit = s_type( arch.regread<OPSIZE>( _gn ) );
  //   int64_t addr_offset = int64_t( (bt_offset >> TypeFor<OPSIZE>::logsize) * (OPSIZE / 8) );
  //   u_type str_opr = arch.rmstrread<OPSIZE>( rmop, addr_offset );
  //   u_type opr_bit = u_type( str_bit % OPSIZE );
  //   arch.flagwrite( CF, bit_t( (str_opr >> opr_bit) & u_type( 1 ) ) );
  //   arch.rmstrwrite<OPSIZE>( rmop, addr_offset, str_opr | (u_type( 1 ) << opr_bit) );
  // }
};

template <> Operation* decode<BTS>( CodeBase const& cb )
{
  // BTS -- Bit Test and Reset
  
  if (auto _ = match( cb, opcode( "\x0f\xba", 5 ) & RM() & Imm<8>() ))
  
    {
      if (cb.opsize()==16) return new BtsImm<16>( _.opbase(), _.rmop(), _.imm<uint8_t>() );
      if (cb.opsize()==32) return new BtsImm<32>( _.opbase(), _.rmop(), _.imm<uint8_t>() );
      if (cb.opsize()==64) return new BtsImm<64>( _.opbase(), _.rmop(), _.imm<uint8_t>() );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\x0f\xab" ) & RM() ))
  
    {
      if (cb.opsize()==16) return new BtsRM<16>( _.opbase(), _.rmop(), _.greg() );
      if (cb.opsize()==32) return new BtsRM<32>( _.opbase(), _.rmop(), _.greg() );
      if (cb.opsize()==64) return new BtsRM<64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  return 0;
}

template <unsigned OPSIZE>
struct Bswap : public Operation
{
  Bswap( OpBase const& opbase, uint8_t _rn ) : Operation( opbase ), rn( _rn ) {} uint8_t rn;
  void disasm( std::ostream& sink ) const { sink << "bswap " << DisasmR<OPSIZE>( rn ); }
  typedef typename TypeFor<OPSIZE>::u u_type;
  // void execute( Arch& arch ) const {
  //   u_type src = arch.regread<OPSIZE>( rn ), dst = 0;
  //   for (unsigned byte = 0; byte < (OPSIZE/8); ++byte)
  //     {
  //       dst = (dst << 8) | (src & u_type( 0xff ));
  //       src >>= 8;
  //     }
  //   arch.regwrite<OPSIZE>( rn, dst );
  // }
};

template <> Operation* decode<BSWAP>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\x0f" ) & VarByte<0xc8,0xf8>() ))
  
    {
      if (cb.opsize()==16) return new Bswap<16>( _.opbase(), _.vbval() );
      if (cb.opsize()==32) return new Bswap<32>( _.opbase(), _.vbval() );
      if (cb.opsize()==64) return new Bswap<64>( _.opbase(), _.vbval() );
      return 0;
    }
  
  return 0;
}

template <unsigned OPSIZE>
struct Movnti : public Operation
{
  Movnti( OpBase const& opbase, MOp* _rmop, uint8_t _gn ) : Operation( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "movnti " << DisasmR<OPSIZE>( gn ) << ',' << DisasmM( rmop ); }
};

template <> Operation* decode<MOVNTI>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\x0f\xc3" ) & RM() ))
  
    {
      if (cb.opsize()==16) return new Movnti<16>( _.opbase(), _.rmop(), _.greg() );
      if (cb.opsize()==32) return new Movnti<32>( _.opbase(), _.rmop(), _.greg() );
      if (cb.opsize()==64) return new Movnti<64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  return 0;
}

template <unsigned OPSIZE>
struct LoadFarPointer : public Operation
{
  LoadFarPointer( OpBase const& opbase, MOp* _rmop, uint8_t _gn, uint8_t _seg )
    : Operation( opbase ), rmop( _rmop ), gn( _gn ), seg( _seg ) {} RMOp rmop; uint8_t gn; uint8_t seg;
  void disasm( std::ostream& sink ) const { sink << "l" << DisasmS( seg ) << ' ' << DisasmM( rmop ) << ',' << DisasmR<OPSIZE>( gn ); }
};

template <> Operation* decode<LFP>( CodeBase const& cb )
{
  // LDS/LES/LFS/LGS/LSS -- Load Far Pointer
  
  if (auto _ = match( cb, opcode( "\xc5" ) & RM_mem() ))
  
    {
      if (cb.mode64()) return 0;
      if (cb.opsize()==16) return new LoadFarPointer<16>( _.opbase(), _.rmop(), _.greg(), DS );
      if (cb.opsize()==32) return new LoadFarPointer<32>( _.opbase(), _.rmop(), _.greg(), DS );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\x0f\xb2" ) & RM_mem() ))
  
    {
      if (cb.opsize()==16) return new LoadFarPointer<16>( _.opbase(), _.rmop(), _.greg(), SS );
      if (cb.opsize()==32) return new LoadFarPointer<32>( _.opbase(), _.rmop(), _.greg(), SS );
      if (cb.opsize()==64) return new LoadFarPointer<64>( _.opbase(), _.rmop(), _.greg(), SS );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\xc4" ) & RM_mem() ))
  
    {
      if (cb.mode64()) return 0;
      if (cb.opsize()==16) return new LoadFarPointer<16>( _.opbase(), _.rmop(), _.greg(), ES );
      if (cb.opsize()==32) return new LoadFarPointer<32>( _.opbase(), _.rmop(), _.greg(), ES );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\x0f\xb4" ) & RM_mem() ))
  
    {
      if (cb.opsize()==16) return new LoadFarPointer<16>( _.opbase(), _.rmop(), _.greg(), FS );
      if (cb.opsize()==32) return new LoadFarPointer<32>( _.opbase(), _.rmop(), _.greg(), FS );
      if (cb.opsize()==64) return new LoadFarPointer<64>( _.opbase(), _.rmop(), _.greg(), FS );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\x0f\xb5" ) & RM_mem() ))
  
    {
      if (cb.opsize()==16) return new LoadFarPointer<16>( _.opbase(), _.rmop(), _.greg(), GS );
      if (cb.opsize()==32) return new LoadFarPointer<32>( _.opbase(), _.rmop(), _.greg(), GS );
      if (cb.opsize()==64) return new LoadFarPointer<64>( _.opbase(), _.rmop(), _.greg(), GS );
      return 0;
    }
  
  return 0;
}

