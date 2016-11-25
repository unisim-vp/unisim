template <class ARCH, unsigned OPSIZE>
struct PushReg : public Operation<ARCH>
{
  PushReg( OpBase<ARCH> const& opbase, uint8_t _rn ) : Operation<ARCH>( opbase ), rn( _rn ) {} uint8_t rn;
  void disasm( std::ostream& sink ) const { sink << "push " << DisasmR<OPSIZE>( rn ); }
  void execute( ARCH& arch ) const { arch.template push<OPSIZE>( arch.template regread<OPSIZE>( rn ) ); }
};

template <class ARCH, unsigned OPSIZE>
struct PushImm : public Operation<ARCH>
{
  typedef typename CTypeFor<OPSIZE>::u imm_type;
  PushImm( OpBase<ARCH> const& opbase, imm_type _imm ) : Operation<ARCH>( opbase ), imm( _imm ) {} imm_type imm;
  void disasm( std::ostream& sink ) const { sink << "push" << ((OPSIZE==16) ? "w " : (OPSIZE==32) ? " " : "q ") << DisasmI( imm ); }
  void execute( ARCH& arch ) const { arch.template push<OPSIZE>( typename TypeFor<ARCH,OPSIZE>::u( imm ) ); }
};

template <class ARCH, unsigned OPSIZE>
struct Push : public Operation<ARCH>
{
  Push( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop ) : Operation<ARCH>( opbase ), rmop( _rmop ) {} RMOp<ARCH> rmop;
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<16>( "push", rmop.isreg() ) << DisasmEw( rmop ); }
  void execute( ARCH& arch ) const { arch.template push<OPSIZE>( arch.template rmread<OPSIZE>( rmop ) ); }
};

template <class ARCH, unsigned OPSIZE>
struct PushSeg : public Operation<ARCH>
{
  PushSeg( OpBase<ARCH> const& opbase, uint8_t _seg ) : Operation<ARCH>( opbase ), seg( _seg ) {} uint8_t seg;
  void disasm( std::ostream& sink ) const { sink << "push" << ((OPSIZE==16) ? "w " : (OPSIZE==32) ? " " : "q ") << DisasmS( seg ); }
  void execute( ARCH& arch ) const { arch.template push<OPSIZE>( typename TypeFor<ARCH,OPSIZE>::u( arch.segregread( seg ) ) ); }
};

template <class ARCH>
Operation<ARCH>*
newPushSeg( unsigned opsize, OpBase<ARCH> const& opbase, uint8_t _seg )
{
  if (opsize==16) return new PushSeg<ARCH,16>( opbase, _seg );
  if (opsize==32) return new PushSeg<ARCH,32>( opbase, _seg );
  if (opsize==64) return new PushSeg<ARCH,64>( opbase, _seg );
  return 0;
}

template <class ARCH> struct DC<ARCH,PUSH> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\xff", 6 ) & RM() ))
  
    {
      if (ic.opsize()==16) return new Push<ARCH,16>( _.opbase(), _.rmop() );
      if (ic.opsize()==32) return new Push<ARCH,32>( _.opbase(), _.rmop() );
      if (ic.opsize()==64) return new Push<ARCH,64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  if (auto _ = match( ic, VarByte<0x50,0xF8>() ))
  
    {
      if (ic.opsize()==16) return new PushReg<ARCH,16>( _.opbase(), _.vbval() );
      if (ic.opsize()==32) return new PushReg<ARCH,32>( _.opbase(), _.vbval() );
      if (ic.opsize()==64) return new PushReg<ARCH,64>( _.opbase(), _.vbval() );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\x6a" ) & Imm<8>() ))
  
    {
      if (ic.opsize()==16) return new PushImm<ARCH,16>( _.opbase(), _.i( int16_t() )  );
      if (ic.opsize()==32) return new PushImm<ARCH,32>( _.opbase(), _.i( int32_t() )  );
      if (ic.opsize()==64) return new PushImm<ARCH,64>( _.opbase(), _.i( int64_t() )  );
      return 0;
    }
  
  if (auto _ = match( ic, OpSize<16>() & opcode( "\x68" ) & Imm<16>() ))
  
    return new PushImm<ARCH,16>( _.opbase(), _.i( int16_t() )  );
  
  if (auto _ = match( ic, OpSize<32>() & opcode( "\x68" ) & Imm<32>() ))
  
    return new PushImm<ARCH,32>( _.opbase(), _.i( int32_t() )  );
  
  if (auto _ = match( ic, OpSize<64>() & opcode( "\x68" ) & Imm<64>() ))
  
    return new PushImm<ARCH,64>( _.opbase(), _.i( int64_t() )  );
  
  if (not ic.mode64()) {
    if (auto _ = match( ic, opcode( "\x06" ) )) return newPushSeg( ic.opsize(), _.opbase(), ES );
    if (auto _ = match( ic, opcode( "\x0e" ) )) return newPushSeg( ic.opsize(), _.opbase(), CS );
    if (auto _ = match( ic, opcode( "\x16" ) )) return newPushSeg( ic.opsize(), _.opbase(), SS );
    if (auto _ = match( ic, opcode( "\x1e" ) )) return newPushSeg( ic.opsize(), _.opbase(), DS );
  }
  
  if (auto _ = match( ic, opcode( "\x0f\xa0" ) )) return newPushSeg( ic.opsize(), _.opbase(), FS );
  if (auto _ = match( ic, opcode( "\x0f\xa8" ) )) return newPushSeg( ic.opsize(), _.opbase(), GS );

  return 0;
}};

template <class ARCH, unsigned OPSIZE>
struct PushAll : public Operation<ARCH>
{
  PushAll( OpBase<ARCH> const& opbase ) : Operation<ARCH>( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "push" << ((OPSIZE==16) ? "w" : (OPSIZE==32) ? "" : "q"); }
  void execute( ARCH& arch ) const {
    typename TypeFor<ARCH,OPSIZE>::u temp = arch.template regread<OPSIZE>( 4 );
    for (int rn = 0; rn < 8; ++rn) arch.template push<OPSIZE>( (rn == 4) ? temp : arch.template regread<OPSIZE>( 4 ) );
  }
};

template <class ARCH> struct DC<ARCH,PUSHA> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\x60" ) ))
  
    {
      if (ic.opsize()==16) return new PushAll<ARCH,16>( _.opbase() );
      if (ic.opsize()==32) return new PushAll<ARCH,32>( _.opbase() );
      if (ic.opsize()==64) return new PushAll<ARCH,64>( _.opbase() );
      return 0;
    }
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE>
struct Pushf : public Operation<ARCH>
{
  Pushf( OpBase<ARCH> const& opbase ) : Operation<ARCH>( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "pushf" << ((OPSIZE==16) ? "w" : (OPSIZE==32) ? "" : "q"); }
  // void execute( ARCH& arch ) const {
  //   u32_t dst = arch.regread32( 4 ) - u32_t( OPSIZE/8 );
  //   arch.template memwrite<OPSIZE>( segment, dst, eflagsread<OPSIZE>( arch ) );
  //   arch.regwrite32( 4, dst );
  // }
};

template <class ARCH> struct DC<ARCH,PUSHF> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\x9c" ) ))
  
    {
      if (ic.opsize()==16) return new Pushf<ARCH,16>( _.opbase() );
      if (ic.opsize()==32) return new Pushf<ARCH,32>( _.opbase() );
      if (ic.opsize()==64) return new Pushf<ARCH,64>( _.opbase() );
    }
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE>
struct PopReg : public Operation<ARCH>
{
  PopReg( OpBase<ARCH> const& opbase, uint8_t _rn ) : Operation<ARCH>( opbase ), rn( _rn ) {} uint8_t rn;
  void disasm( std::ostream& sink ) const { sink << "pop " << DisasmR<OPSIZE>( rn ); }
  void execute( ARCH& arch ) const { arch.template regwrite<OPSIZE>( rn, arch.template pop<OPSIZE>() ); }
};

template <class ARCH, unsigned OPSIZE>
struct Pop : public Operation<ARCH>
{
  Pop( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop ) : Operation<ARCH>( opbase ), rmop( _rmop ) {} RMOp<ARCH> rmop;
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "pop", rmop.isreg() ) << DisasmE( UI<OPSIZE>(), rmop ); }
  void execute( ARCH& arch ) const { arch.template rmwrite<OPSIZE>( rmop, arch.template pop<OPSIZE>() ); }
};

template <class ARCH, unsigned OPSIZE>
struct PopSeg : public Operation<ARCH>
{
  PopSeg( OpBase<ARCH> const& opbase, uint8_t _seg ) : Operation<ARCH>( opbase ), seg( _seg ) {} uint8_t seg;
  void disasm( std::ostream& sink ) const { sink << "pop"  << ((OPSIZE==16) ? "w " : (OPSIZE==32) ? " " : "q ") << DisasmS( seg ); }
  void execute( ARCH& arch ) const { arch.segregwrite( seg, typename ARCH::u16_t( arch.template pop<OPSIZE>() )  ); }
};

template <class ARCH>
Operation<ARCH>*
newPopSeg( unsigned opsize, OpBase<ARCH> const& opbase, uint8_t _seg )
{
  if (opsize==16) return new PopSeg<ARCH,16>( opbase, _seg );
  if (opsize==32) return new PopSeg<ARCH,32>( opbase, _seg );
  if (opsize==64) return new PopSeg<ARCH,64>( opbase, _seg );
  return 0;
}

template <class ARCH> struct DC<ARCH,POP> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\x8f", 0 ) & RM() ))
  
    {
      if (ic.opsize()==16) return new Pop<ARCH,16>( _.opbase(), _.rmop() );
      if (ic.opsize()==32) return new Pop<ARCH,32>( _.opbase(), _.rmop() );
      if (ic.opsize()==64) return new Pop<ARCH,64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  if (auto _ = match( ic, VarByte<0x58,0xF8>() ))
  
    {
      if (ic.opsize()==16) return new PopReg<ARCH,16>( _.opbase(), _.vbval() );
      if (ic.opsize()==32) return new PopReg<ARCH,32>( _.opbase(), _.vbval() );
      if (ic.opsize()==64) return new PopReg<ARCH,64>( _.opbase(), _.vbval() );
      return 0;
    }
  
  if (not ic.mode64()) {
    if (auto _ = match( ic, opcode( "\x07" ) )) return newPopSeg( ic.opsize(), _.opbase(), ES );
    if (auto _ = match( ic, opcode( "\x17" ) )) return newPopSeg( ic.opsize(), _.opbase(), SS );
    if (auto _ = match( ic, opcode( "\x1f" ) )) return newPopSeg( ic.opsize(), _.opbase(), DS );
  }
  
  if (auto _ = match( ic, opcode( "\x0f\xa1" ) )) return newPopSeg( ic.opsize(), _.opbase(), FS );
  if (auto _ = match( ic, opcode( "\x0f\xa9" ) )) return newPopSeg( ic.opsize(), _.opbase(), GS );

  return 0;
}};

template <class ARCH, unsigned OPSIZE>
struct PopAll : public Operation<ARCH>
{
  PopAll( OpBase<ARCH> const& opbase ) : Operation<ARCH>( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "popa" << ((OPSIZE==16) ? "w" : (OPSIZE==32) ? "" : "q"); }
  void execute( ARCH& arch ) const {
    for (int rn = 0; rn < 8; ++rn) {
      typename TypeFor<ARCH,OPSIZE>::u value = arch.template pop<OPSIZE>();
      if (rn != 4) arch.template regwrite<OPSIZE>( rn, value );
    }
  }
};

template <class ARCH> struct DC<ARCH,POPA> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\x61" ) ))
  
    {
      if (ic.opsize()==16) return new PopAll<ARCH,16>( _.opbase() );
      if (ic.opsize()==32) return new PopAll<ARCH,32>( _.opbase() );
      if (ic.opsize()==64) return new PopAll<ARCH,64>( _.opbase() );
      return 0;
    }
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE>
struct Popf : public Operation<ARCH>
{
  Popf( OpBase<ARCH> const& opbase ) : Operation<ARCH>( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "popf"; }
  // void execute( ARCH& arch ) const {
  //   u32_t src = arch.regread32( 4 );
  //   arch.regwrite32( 4, src + u32_t( OPSIZE/8 ) );
  //   u16_t word = arch.template memread<16>( SS, src );
  //   arch.flagwrite( CF, bit_t( (word >>  0) & u32_t( 1 ) ) );
  //   arch.flagwrite( PF, bit_t( (word >>  2) & u32_t( 1 ) ) );
  //   arch.flagwrite( AF, bit_t( (word >>  4) & u32_t( 1 ) ) );
  //   arch.flagwrite( ZF, bit_t( (word >>  6) & u32_t( 1 ) ) );
  //   arch.flagwrite( SF, bit_t( (word >>  7) & u32_t( 1 ) ) );
  //   arch.flagwrite( DF, bit_t( (word >> 10) & u32_t( 1 ) ) );
  //   arch.flagwrite( OF, bit_t( (word >> 11) & u32_t( 1 ) ) );
  // }
};

template <class ARCH> struct DC<ARCH,POPF> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\x9d" ) ))
  
    {
      if (ic.opsize()==16) return new Popf<ARCH,16>( _.opbase() );
      if (ic.opsize()==32) return new Popf<ARCH,32>( _.opbase() );
      if (ic.opsize()==64) return new Popf<ARCH,64>( _.opbase() );
    }
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE, bool GTOE>
struct MovRM : public Operation<ARCH>
{
  MovRM( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const {
    if (GTOE) sink << "mov " << DisasmR<OPSIZE>( gn ) << ',' << DisasmE( UI<OPSIZE>(), rmop );
    else      sink << "mov " << DisasmE( UI<OPSIZE>(), rmop ) << ',' << DisasmR<OPSIZE>( gn );
  }
  void execute( ARCH& arch ) const {
    if (GTOE) arch.template rmwrite<OPSIZE>( rmop, arch.template regread<OPSIZE>( gn ) );
    else      arch.template regwrite<OPSIZE>( gn, arch.template rmread<OPSIZE>( rmop ) ); 
  }
};

template <class ARCH, unsigned OPSIZE>
struct MovImm : public Operation<ARCH>
{
  typedef typename CTypeFor<OPSIZE>::u imm_type;
  MovImm( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, imm_type _imm ) : Operation<ARCH>( opbase ), rmop( _rmop ), imm( _imm ) {} RMOp<ARCH> rmop; imm_type imm;
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "mov", rmop.isreg() ) << DisasmI( imm ) << ',' << DisasmE( UI<OPSIZE>(), rmop ); }
  void execute( ARCH& arch ) const { arch.template rmwrite<OPSIZE>( rmop, typename TypeFor<ARCH,OPSIZE>::u( imm ) ); }
};

template <class ARCH, unsigned OPSIZE, bool STOE>
struct MovSeg : public Operation<ARCH>
{
  MovSeg( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _seg ) : Operation<ARCH>( opbase ), seg( _seg ), rmop( _rmop ) {} uint8_t seg; RMOp<ARCH> rmop;
  void disasm( std::ostream& sink ) const {
    if (STOE) sink << "mov " << DisasmS( seg ) << ',' << DisasmE( UI<OPSIZE>(), rmop );
    else      sink << "mov " << DisasmE( UI<OPSIZE>(), rmop ) << ',' << DisasmS( seg );
  }
  void execute( ARCH& arch ) const
  {
    typedef typename ARCH::u16_t u16_t;
    
    if (STOE) arch.template rmwrite<OPSIZE>( rmop, typename TypeFor<ARCH,OPSIZE>::u( arch.segregread( seg ) ) );
    else      arch.segregwrite( seg, u16_t( arch.template rmread<OPSIZE>( rmop ) ) );
  }
};

template <class ARCH> struct DC<ARCH,MOV> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  // MOV -- Move
  if (auto _ = match( ic, opcode( "\x88" ) & RM() ))
  
    return new MovRM<ARCH,8,true>( _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, opcode( "\x89" ) & RM() ))
  
    {
      if (ic.opsize()==16) return new MovRM<ARCH,16,true>( _.opbase(), _.rmop(), _.greg() );
      if (ic.opsize()==32) return new MovRM<ARCH,32,true>( _.opbase(), _.rmop(), _.greg() );
      if (ic.opsize()==64) return new MovRM<ARCH,64,true>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\x8a" ) & RM() ))
  
    return new MovRM<ARCH,8,false>( _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, opcode( "\x8b" ) & RM() ))
  
    {
      if (ic.opsize()==16) return new MovRM<ARCH,16,false>( _.opbase(), _.rmop(), _.greg() );
      if (ic.opsize()==32) return new MovRM<ARCH,32,false>( _.opbase(), _.rmop(), _.greg() );
      if (ic.opsize()==64) return new MovRM<ARCH,64,false>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\x8c" ) & RM() ))
  
    {
      if (ic.opsize()==16) return new MovSeg<ARCH,16,true>( _.opbase(), _.rmop(), _.greg() );
      if (ic.opsize()==32) return new MovSeg<ARCH,32,true>( _.opbase(), _.rmop(), _.greg() );
      if (ic.opsize()==64) return new MovSeg<ARCH,64,true>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\x8e" ) & RM() ))

    {
      if (ic.opsize()==16) return new MovSeg<ARCH,16,false>( _.opbase(), _.rmop(), _.greg() );
      if (ic.opsize()==32) return new MovSeg<ARCH,32,false>( _.opbase(), _.rmop(), _.greg() );
      if (ic.opsize()==64) return new MovSeg<ARCH,64,false>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\xa0" ) & Moffs() ))
  
    return new MovRM<ARCH,8,false>( _.opbase(), _.rmop(), 0 );
  
  if (auto _ = match( ic, opcode( "\xa1" ) & Moffs() ))
  
    {
      if (ic.opsize()==16) return new MovRM<ARCH,16,false>( _.opbase(), _.rmop(), 0 );
      if (ic.opsize()==32) return new MovRM<ARCH,32,false>( _.opbase(), _.rmop(), 0 );
      if (ic.opsize()==64) return new MovRM<ARCH,64,false>( _.opbase(), _.rmop(), 0 );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\xa2" ) & Moffs() ))
  
    return new MovRM<ARCH,8,true>( _.opbase(), _.rmop(), 0 );

  if (auto _ = match( ic, opcode( "\xa3" ) & Moffs() ))

    {
      if (ic.opsize()==16) return new MovRM<ARCH,16,true>( _.opbase(), _.rmop(), 0 );
      if (ic.opsize()==32) return new MovRM<ARCH,32,true>( _.opbase(), _.rmop(), 0 );
      if (ic.opsize()==64) return new MovRM<ARCH,64,true>( _.opbase(), _.rmop(), 0 );
      return 0;
    }
  
  if (auto _ = match( ic, VarByte<0xb0,0xf8>() & Imm<8>() ))
  
    return new MovImm<ARCH,8>( _.opbase(), make_rop<ARCH>( _.vbval() ), _.i( int8_t() ) );
  
  if (auto _ = match( ic, OpSize<16>() & VarByte<0xb8,0xf8>() & Imm<16>() ))
  
    return new MovImm<ARCH,16>( _.opbase(), make_rop<ARCH>( _.vbval() ), _.i( int16_t() ) );
  
  if (auto _ = match( ic, OpSize<32>() & VarByte<0xb8,0xf8>() & Imm<32>() ))
  
    return new MovImm<ARCH,32>( _.opbase(), make_rop<ARCH>( _.vbval() ), _.i( int32_t() ) );
  
  if (auto _ = match( ic, OpSize<64>() & VarByte<0xb8,0xf8>() & Imm<32>() ))
  
    return new MovImm<ARCH,64>( _.opbase(), make_rop<ARCH>( _.vbval() ), _.i( int64_t() ) );
  
  if (auto _ = match( ic, opcode( "\xc6", 0 ) & RM() & Imm<8>() ))
  
    return new MovImm<ARCH,8>( _.opbase(), _.rmop(), _.i( int8_t() ) );
  
  if (auto _ = match( ic, OpSize<16>() & opcode( "\xc7", 0 ) & RM() & Imm<16>() ))
  
    return new MovImm<ARCH,16>( _.opbase(), _.rmop(), _.i( int16_t() ) );
  
  if (auto _ = match( ic, OpSize<32>() & opcode( "\xc7", 0 ) & RM() & Imm<32>() ))
  
    return new MovImm<ARCH,32>( _.opbase(), _.rmop(), _.i( int32_t() ) );
  
  if (auto _ = match( ic, OpSize<64>() & opcode( "\xc7", 0 ) & RM() & Imm<32>() ))
  
    return new MovImm<ARCH,64>( _.opbase(), _.rmop(), _.i( int64_t() ) );
  
  return 0;
}};

template <class ARCH, unsigned SOPSIZE, unsigned DOPSIZE>
struct Movzx : public Operation<ARCH>
{
  Movzx( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const {
    sink << "movz" << ((SOPSIZE==8) ? 'b' : 'w') << ((DOPSIZE==16) ? 'w' : (DOPSIZE==32) ? 'l' : 'q') << ' ' << DisasmE( UI<SOPSIZE>(), rmop ) << ',' << DisasmR<DOPSIZE>( gn );
  }
  void execute( ARCH& arch ) const { arch.template regwrite<DOPSIZE>( gn, typename TypeFor<ARCH,DOPSIZE>::u( arch.template rmread<SOPSIZE>( rmop ) ) ); }
};

template <class ARCH> struct DC<ARCH,MOVZX> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  // MOVZX -- Move with Zero-Extend
  if (auto _ = match( ic, opcode( "\x0f\xb6" ) & RM() ))
  
    {
      if (ic.opsize()==16) return new Movzx<ARCH,8,16>( _.opbase(), _.rmop(), _.greg() );
      if (ic.opsize()==32) return new Movzx<ARCH,8,32>( _.opbase(), _.rmop(), _.greg() );
      if (ic.opsize()==64) return new Movzx<ARCH,8,64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\x0f\xb7" ) & RM() ))
  
    {
      if (ic.opsize()==32) return new Movzx<ARCH,16,32>( _.opbase(), _.rmop(), _.greg() );
      if (ic.opsize()==64) return new Movzx<ARCH,16,64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  return 0;
}};

template <class ARCH, unsigned SOPSIZE, unsigned DOPSIZE>
struct Movsx : public Operation<ARCH>
{
  Movsx( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const {
    sink << "movs" << ((SOPSIZE==8) ? 'b' : 'w') << ((DOPSIZE==16) ? 'w' : (DOPSIZE==32) ? 'l' : 'q') << ' ' << DisasmE( UI<SOPSIZE>(), rmop ) << ',' << DisasmR<DOPSIZE>( gn );
  }
  typedef typename TypeFor<ARCH,SOPSIZE>::s ssrc_type;
  typedef typename TypeFor<ARCH,DOPSIZE>::u udst_type;
  typedef typename TypeFor<ARCH,DOPSIZE>::s sdst_type;
  void execute( ARCH& arch ) const { arch.template regwrite<DOPSIZE>( gn, udst_type( sdst_type( ssrc_type( arch.template rmread<SOPSIZE>( rmop ) ) ) ) ); }
};

template <class ARCH> struct DC<ARCH,MOVSX> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  // MOVSX -- Move with Zero-Extend
  if (auto _ = match( ic, opcode( "\x0f\xbe" ) & RM() ))
  
    {
      if (ic.opsize()==16) return new Movsx<ARCH,8,16>( _.opbase(), _.rmop(), _.greg() );
      if (ic.opsize()==32) return new Movsx<ARCH,8,32>( _.opbase(), _.rmop(), _.greg() );
      if (ic.opsize()==64) return new Movsx<ARCH,8,64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\x0f\xbf" ) & RM() ))
  
    {
      if (ic.opsize()==32) return new Movsx<ARCH,16,32>( _.opbase(), _.rmop(), _.greg() );
      if (ic.opsize()==64) return new Movsx<ARCH,16,64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  if (auto _ = match( ic, OpSize<64>() & opcode( "\x63" ) & RM() ))
  
    return new Movsx<ARCH,32,64>( _.opbase(), _.rmop(), _.greg() );
  
  return 0;
}};

template <class ARCH>
struct WriteDF : public Operation<ARCH>
{
  WriteDF( OpBase<ARCH> const& opbase, uint8_t _df ) : Operation<ARCH>( opbase ), df( _df ) {} uint8_t df;
  void disasm( std::ostream& sink ) const { sink << (df ? "std" : "cld"); }
  void execute( ARCH& arch ) const { arch.flagwrite( ARCH::DF, typename ARCH::bit_t( df ) ); }
};

template <class ARCH> struct DC<ARCH,STD> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, VarByte<0xfc,0xfe>() ))
    
    return new WriteDF<ARCH>( _.opbase(), _.vbval() );
  
  return 0;
}};

template <class ARCH>
struct Arpl : public Operation<ARCH>
{
  Arpl( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "arpl " << DisasmR<16>( gn ) << ',' << DisasmE( UI<16>(), rmop ); }
};

template <class ARCH> struct DC<ARCH,ARPL> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\x63" ) & RM() ))
    return new Arpl<ARCH>( _.opbase(), _.rmop(), _.greg() );
  return 0;
}};

template <class ARCH, unsigned OPSIZE>
struct CmpXchg : public Operation<ARCH>
{
  CmpXchg( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "cmpxchg " << DisasmR<OPSIZE>( gn ) << ',' << DisasmE( UI<OPSIZE>(), rmop ); }
  
  void execute( ARCH& arch ) const
  {
    typename TypeFor<ARCH,OPSIZE>::u mem_operand = arch.template rmread<OPSIZE>( rmop );
    typename ARCH::bit_t equal = (arch.template regread<OPSIZE>( 0 ) == mem_operand);
    arch.flagwrite( ARCH::ZF, equal );
    if (arch.Cond( equal )) arch.template rmwrite<OPSIZE>( rmop, arch.template regread<OPSIZE>( gn ) );
    else                 arch.template regwrite<OPSIZE>( 0, mem_operand );
  }
};

template <class ARCH> struct DC<ARCH,CMPXCHG> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  // CMPXCHG -- Compare and Exchange
  if (auto _ = match( ic, opcode( "\x0f\xb0" ) & RM() ))
    return new CmpXchg<ARCH,8>( _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, opcode( "\x0f\xb1" ) & RM() ))
    {
      if (ic.opsize()==16) return new CmpXchg<ARCH,16>( _.opbase(), _.rmop(), _.greg() );
      if (ic.opsize()==32) return new CmpXchg<ARCH,32>( _.opbase(), _.rmop(), _.greg() );
      if (ic.opsize()==64) return new CmpXchg<ARCH,64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  return 0;
}};

template <class ARCH, unsigned BOPSIZE>
struct CmpXchgBytes : public Operation<ARCH>
{
  CmpXchgBytes( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop ) : Operation<ARCH>( opbase ), rmop( _rmop ) {} RMOp<ARCH> rmop;
  void disasm( std::ostream& sink ) const { sink << "cmpxchg" << BOPSIZE << "b " << DisasmM( rmop ); }
};

template <class ARCH> struct DC<ARCH,CMPXCHG8B> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  // CMPXCHG8B/CMPXCHG16B -- Compare and Exchange Bytes
  if (auto _ = match( ic, opcode( "\x0f\xc7", 1 ) & RM_mem() ))
  
    {
      if (ic.opsize()==32) return new CmpXchgBytes<ARCH,8>( _.opbase(), _.rmop() );
      if (ic.opsize()==64) return new CmpXchgBytes<ARCH,16>( _.opbase(), _.rmop() );
      return 0;
    }
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE>
struct Xchg : public Operation<ARCH>
{
  Xchg( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "xchg " << DisasmR<OPSIZE>( gn ) << ',' << DisasmE( UI<OPSIZE>(), rmop ); }
  typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
  void execute( ARCH& arch ) const {
    u_type a = arch.template regread<OPSIZE>( gn );
    u_type b = arch.template rmread<OPSIZE>( rmop );
    arch.template regwrite<OPSIZE>( gn, b );
    arch.template rmwrite<OPSIZE>( rmop, a );
  }
};

template <class ARCH> struct DC<ARCH,XCHG> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, VarByte<0x90,0xF8>() ))
  
    {
      if (ic.opsize()==16) return new Xchg<ARCH,16>( _.opbase(), make_rop<ARCH>( _.vbval() ), 0 );
      if (ic.opsize()==32) return new Xchg<ARCH,32>( _.opbase(), make_rop<ARCH>( _.vbval() ), 0 ); 
      if (ic.opsize()==64) return new Xchg<ARCH,64>( _.opbase(), make_rop<ARCH>( _.vbval() ), 0 );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\x86" ) & RM() ))
  
    return new Xchg<ARCH,8>( _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, opcode( "\x87" ) & RM() ))
  
    {
      if (ic.opsize()==16) return new Xchg<ARCH,16>( _.opbase(), _.rmop(), _.greg() );
      if (ic.opsize()==32) return new Xchg<ARCH,32>( _.opbase(), _.rmop(), _.greg() );
      if (ic.opsize()==64) return new Xchg<ARCH,64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }

  return 0;
}};

// TODO: Special nops processing ?
// template <class ARCH>
// struct Nop : public Operation<ARCH>
// {
//   Nop( OpBase<ARCH> const& opbase ) : Operation<ARCH>( opbase ) {}
//   void disasm( std::ostream& sink ) const { sink << "nop"; }
//   void execute( ARCH& arch ) const {}
// };

// template <class ARCH>
// struct Nopw : public Operation<ARCH>
// {
//   Nopw( OpBase<ARCH> const& opbase ) : Operation<ARCH>( opbase ) {}
//   void disasm( std::ostream& sink ) const { sink << "nopw"; }
//   // void execute( ARCH& arch ) const {}
// };

// template <class ARCH>
// struct Nop_ed : public Operation<ARCH>
// {
//   Nop_ed( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
//   void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<32>( "nop", rmop.isreg() ) << DisasmE( UI<32>(), rmop ); }
//   // void execute( ARCH& arch ) const {}
// };

// template <class ARCH>
// struct Nop_ew : public Operation<ARCH>
// {
//   Nop_ew( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
//   void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<16>( "nop", rmop.isreg() ) << DisasmE( UI<16>(), rmop ); }
//   // void execute( ARCH& arch ) const {}
// };
// op nop( 0x90[8] )
// op nopw( 0x66[8]:> <:0x90[8] );
// op nop_ed( 0x0f[8]:> <:0x1f[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*rmop[Rmop] );
// op nop_ew( 0x66[8]:> <:0x0f[8]:> <:0x1f[8]:> <:?[2]:gn[3]:?[3]:> rewind <:*rmop[Rmop] );


template <class ARCH, unsigned OPSIZE>
struct XAddEG : public Operation<ARCH>
{
  XAddEG( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "xadd " << DisasmR<OPSIZE>( gn ) << ',' << DisasmE( UI<OPSIZE>(), rmop ); }
  typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
  // void execute( ARCH& arch ) const {
  //   u_type a = arch.template rmread<OPSIZE>( rmop );
  //   u_type b = arch.template regread<OPSIZE>( gn );
  //   arch.template rmwrite<OPSIZE>( rmop, a + b  );
  //   arch.template regwrite<OPSIZE>( gn, a );
  // }
};

template <class ARCH> struct DC<ARCH,XADD> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  // XADD -- Exchange and Add
  if (auto _ = match( ic, opcode( "\x0f\xc0" ) & RM() ))
  
    return new XAddEG<ARCH,8>( _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, opcode( "\x0f\xc1" ) & RM() ))
  
    {
      if (ic.opsize()==16) return new XAddEG<ARCH,16>( _.opbase(), _.rmop(), _.greg() );
      if (ic.opsize()==32) return new XAddEG<ARCH,32>( _.opbase(), _.rmop(), _.greg() );
      if (ic.opsize()==64) return new XAddEG<ARCH,64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE>
struct Cmovcc : public Operation<ARCH>
{
  Cmovcc( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn, uint8_t _cc )
    : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ), cc( _cc ) {} RMOp<ARCH> rmop; uint8_t gn; uint8_t cc; 
  void disasm( std::ostream& sink ) const { sink << "cmov" << DisasmCond( cc ) << ' ' << DisasmE( UI<OPSIZE>(), rmop ) << ',' << DisasmR<OPSIZE>( gn ); }
  void execute( ARCH& arch ) const
  {
    if (arch.Cond( eval_cond( arch, cc ) )) {
      arch.template regwrite<OPSIZE> ( gn, arch.template rmread<OPSIZE>( rmop ) );
    }
  }
};

template <class ARCH> struct DC<ARCH,CMOVCC> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  // CMOVcc -- Conditional Move
  if (auto _ = match( ic, opcode( "\x0f" ) & VarByte<0x40,0xF0>() & RM() ))
  
    {
      if (ic.opsize()==16) return new Cmovcc<ARCH,16>( _.opbase(), _.rmop(), _.greg(), _.vbval() );
      if (ic.opsize()==32) return new Cmovcc<ARCH,32>( _.opbase(), _.rmop(), _.greg(), _.vbval() );
      if (ic.opsize()==64) return new Cmovcc<ARCH,64>( _.opbase(), _.rmop(), _.greg(), _.vbval() );
      return 0;
    }
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE>
struct BtImm : public Operation<ARCH>
{
  BtImm( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _imm ) : Operation<ARCH>( opbase ), rmop( _rmop ), imm( _imm ) {} RMOp<ARCH> rmop; uint8_t imm;
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "bt", rmop.isreg() ) << DisasmI( imm ) << ',' << DisasmE( UI<OPSIZE>(), rmop ); }
  typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
  // void execute( ARCH& arch ) const {
  //   unsigned bitoffset = imm % OPSIZE;
  //   u_type opr = arch.template rmread<OPSIZE>( rmop )
  //   arch.flagwrite( ARCH::CF, bit_t( (opr >> bitoffset) & u_type( 1 ) ) );
  // }
};

template <class ARCH, unsigned OPSIZE>
struct BtRM : public Operation<ARCH>
{
  BtRM( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "bt " << DisasmR<OPSIZE>( gn ) << ',' << DisasmE( UI<OPSIZE>(), rmop ); }
  typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
  typedef typename TypeFor<ARCH,OPSIZE>::s s_type;
  // void execute( ARCH& arch ) const {
  //   s_type str_bit = s_type( arch.template regread<OPSIZE>( _gn ) );
  //   int64_t addr_offset = int64_t( (bt_offset >> TypeFor<ARCH,OPSIZE>::logsize) * (OPSIZE / 8) );
  //   u_type str_opr = arch.template rmstrread<OPSIZE>( rmop, addr_offset );
  //   u_type opr_bit = u_type( str_bit % OPSIZE );
  //   arch.flagwrite( ARCH::CF, bit_t( (str_opr >> opr_bit) & u_type( 1 ) ) );
  // }
};

template <class ARCH> struct DC<ARCH,BT> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  // BT -- BitOpcode
  if (auto _ = match( ic, opcode( "\x0f\xba", 4 ) & RM() & Imm<8>() ))
  
    {
      if (ic.opsize()==16) return new BtImm<ARCH,16>( _.opbase(), _.rmop(), _.i( uint8_t() ) );
      if (ic.opsize()==32) return new BtImm<ARCH,32>( _.opbase(), _.rmop(), _.i( uint8_t() ) );
      if (ic.opsize()==64) return new BtImm<ARCH,64>( _.opbase(), _.rmop(), _.i( uint8_t() ) );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\x0f\xa3" ) & RM() ))
  
    {
      if (ic.opsize()==16) return new BtRM<ARCH,16>( _.opbase(), _.rmop(), _.greg() );
      if (ic.opsize()==32) return new BtRM<ARCH,32>( _.opbase(), _.rmop(), _.greg() );
      if (ic.opsize()==64) return new BtRM<ARCH,64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE>
struct BtcImm : public Operation<ARCH>
{
  BtcImm( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _imm ) : Operation<ARCH>( opbase ), rmop( _rmop ), imm( _imm ) {} RMOp<ARCH> rmop; uint8_t imm;
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "btc", rmop.isreg() ) << DisasmI( imm ) << ',' << DisasmE( UI<OPSIZE>(), rmop ); }
  typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
  // void execute( ARCH& arch ) const {
  //   unsigned bitoffset = imm % OPSIZE;
  //   u_type opr = arch.template rmread<OPSIZE>( rmop )
  //   arch.flagwrite( ARCH::CF, bit_t( (opr >> bitoffset) & u_type( 1 ) ) );
  //   arch.template rmwrite<OPSIZE>( rmop, opr ^ (u_type( 1 ) << bitoffset) );
  // }
};

template <class ARCH, unsigned OPSIZE>
struct BtcRM : public Operation<ARCH>
{
  BtcRM( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "btc " << DisasmR<OPSIZE>( gn ) << ',' << DisasmE( UI<OPSIZE>(), rmop ); }
  typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
  typedef typename TypeFor<ARCH,OPSIZE>::s s_type;
  // void execute( ARCH& arch ) const {
  //   s_type str_bit = s_type( arch.template regread<OPSIZE>( _gn ) );
  //   int64_t addr_offset = int64_t( (bt_offset >> TypeFor<ARCH,OPSIZE>::logsize) * (OPSIZE / 8) );
  //   u_type str_opr = arch.template rmstrread<OPSIZE>( rmop, addr_offset );
  //   u_type opr_bit = u_type( str_bit % OPSIZE );
  //   arch.flagwrite( ARCH::CF, bit_t( (str_opr >> opr_bit) & u_type( 1 ) ) );
  //   arch.template rmstrwrite<OPSIZE>( rmop, addr_offset, str_opr ^ (u_type( 1 ) << opr_bit) );
  // }
};

template <class ARCH> struct DC<ARCH,BTC> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  // BTC -- Bit Test and Complement
  
  if (auto _ = match( ic, opcode( "\x0f\xba", 7 ) & RM() & Imm<8>() ))
  
    {
      if (ic.opsize()==16) return new BtcImm<ARCH,16>( _.opbase(), _.rmop(), _.i( uint8_t() ) );
      if (ic.opsize()==32) return new BtcImm<ARCH,32>( _.opbase(), _.rmop(), _.i( uint8_t() ) );
      if (ic.opsize()==64) return new BtcImm<ARCH,64>( _.opbase(), _.rmop(), _.i( uint8_t() ) );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\x0f\xbb" ) & RM() ))
  
    {
      if (ic.opsize()==16) return new BtcRM<ARCH,16>( _.opbase(), _.rmop(), _.greg() );
      if (ic.opsize()==32) return new BtcRM<ARCH,32>( _.opbase(), _.rmop(), _.greg() );
      if (ic.opsize()==64) return new BtcRM<ARCH,64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE>
struct BtrImm : public Operation<ARCH>
{
  BtrImm( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _imm ) : Operation<ARCH>( opbase ), rmop( _rmop ), imm( _imm ) {} RMOp<ARCH> rmop; uint8_t imm;
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "btr", rmop.isreg() ) << DisasmI( imm ) << ',' << DisasmE( UI<OPSIZE>(), rmop ); }
  typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
  // void execute( ARCH& arch ) const {
  //   unsigned bitoffset = imm % OPSIZE;
  //   u_type opr = arch.template rmread<OPSIZE>( rmop )
  //   arch.flagwrite( ARCH::CF, bit_t( (opr >> bitoffset) & u_type( 1 ) ) );
  //   arch.template rmwrite<OPSIZE>( rmop, opr & ~(u_type( 1 ) << bitoffset) );
  // }
};

template <class ARCH, unsigned OPSIZE>
struct BtrRM : public Operation<ARCH>
{
  BtrRM( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "btc " << DisasmR<OPSIZE>( gn ) << ',' << DisasmE( UI<OPSIZE>(), rmop ); }
  typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
  typedef typename TypeFor<ARCH,OPSIZE>::s s_type;
  // void execute( ARCH& arch ) const {
  //   s_type str_bit = s_type( arch.template regread<OPSIZE>( _gn ) );
  //   int64_t addr_offset = int64_t( (bt_offset >> TypeFor<ARCH,OPSIZE>::logsize) * (OPSIZE / 8) );
  //   u_type str_opr = arch.template rmstrread<OPSIZE>( rmop, addr_offset );
  //   u_type opr_bit = u_type( str_bit % OPSIZE );
  //   arch.flagwrite( ARCH::CF, bit_t( (str_opr >> opr_bit) & u_type( 1 ) ) );
  //   arch.template rmstrwrite<OPSIZE>( rmop, addr_offset, str_opr & ~(u_type( 1 ) << opr_bit) );
  // }
};

template <class ARCH> struct DC<ARCH,BTR> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  // BTR -- Bit Test and Reset
  
  if (auto _ = match( ic, opcode( "\x0f\xba", 6 ) & RM() & Imm<8>() ))
  
    {
      if (ic.opsize()==16) return new BtrImm<ARCH,16>( _.opbase(), _.rmop(), _.i( uint8_t() ) );
      if (ic.opsize()==32) return new BtrImm<ARCH,32>( _.opbase(), _.rmop(), _.i( uint8_t() ) );
      if (ic.opsize()==64) return new BtrImm<ARCH,64>( _.opbase(), _.rmop(), _.i( uint8_t() ) );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\x0f\xb3" ) & RM() ))
  
    {
      if (ic.opsize()==16) return new BtrRM<ARCH,16>( _.opbase(), _.rmop(), _.greg() );
      if (ic.opsize()==32) return new BtrRM<ARCH,32>( _.opbase(), _.rmop(), _.greg() );
      if (ic.opsize()==64) return new BtrRM<ARCH,64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE>
struct BtsImm : public Operation<ARCH>
{
  BtsImm( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _imm ) : Operation<ARCH>( opbase ), rmop( _rmop ), imm( _imm ) {} RMOp<ARCH> rmop; uint8_t imm;
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "bts", rmop.isreg() ) << DisasmI( imm ) << ',' << DisasmE( UI<OPSIZE>(), rmop ); }
  typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
  // void execute( ARCH& arch ) const {
  //   unsigned bitoffset = imm % OPSIZE;
  //   u_type opr = arch.template rmread<OPSIZE>( rmop )
  //   arch.flagwrite( ARCH::CF, bit_t( (opr >> bitoffset) & u_type( 1 ) ) );
  //   arch.template rmwrite<OPSIZE>( rmop, opr | (u_type( 1 ) << bitoffset) );
  // }
};

template <class ARCH, unsigned OPSIZE>
struct BtsRM : public Operation<ARCH>
{
  BtsRM( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "btc " << DisasmR<OPSIZE>( gn ) << ',' << DisasmE( UI<OPSIZE>(), rmop ); }
  typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
  typedef typename TypeFor<ARCH,OPSIZE>::s s_type;
  // void execute( ARCH& arch ) const {
  //   s_type str_bit = s_type( arch.template regread<OPSIZE>( _gn ) );
  //   int64_t addr_offset = int64_t( (bt_offset >> TypeFor<ARCH,OPSIZE>::logsize) * (OPSIZE / 8) );
  //   u_type str_opr = arch.template rmstrread<OPSIZE>( rmop, addr_offset );
  //   u_type opr_bit = u_type( str_bit % OPSIZE );
  //   arch.flagwrite( ARCH::CF, bit_t( (str_opr >> opr_bit) & u_type( 1 ) ) );
  //   arch.template rmstrwrite<OPSIZE>( rmop, addr_offset, str_opr | (u_type( 1 ) << opr_bit) );
  // }
};

template <class ARCH> struct DC<ARCH,BTS> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  // BTS -- Bit Test and Reset
  
  if (auto _ = match( ic, opcode( "\x0f\xba", 5 ) & RM() & Imm<8>() ))
  
    {
      if (ic.opsize()==16) return new BtsImm<ARCH,16>( _.opbase(), _.rmop(), _.i( uint8_t() ) );
      if (ic.opsize()==32) return new BtsImm<ARCH,32>( _.opbase(), _.rmop(), _.i( uint8_t() ) );
      if (ic.opsize()==64) return new BtsImm<ARCH,64>( _.opbase(), _.rmop(), _.i( uint8_t() ) );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\x0f\xab" ) & RM() ))
  
    {
      if (ic.opsize()==16) return new BtsRM<ARCH,16>( _.opbase(), _.rmop(), _.greg() );
      if (ic.opsize()==32) return new BtsRM<ARCH,32>( _.opbase(), _.rmop(), _.greg() );
      if (ic.opsize()==64) return new BtsRM<ARCH,64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE>
struct Bswap : public Operation<ARCH>
{
  Bswap( OpBase<ARCH> const& opbase, uint8_t _rn ) : Operation<ARCH>( opbase ), rn( _rn ) {} uint8_t rn;
  void disasm( std::ostream& sink ) const { sink << "bswap " << DisasmR<OPSIZE>( rn ); }
  typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
  // void execute( ARCH& arch ) const {
  //   u_type src = arch.template regread<OPSIZE>( rn ), dst = 0;
  //   for (unsigned byte = 0; byte < (OPSIZE/8); ++byte)
  //     {
  //       dst = (dst << 8) | (src & u_type( 0xff ));
  //       src >>= 8;
  //     }
  //   arch.template regwrite<OPSIZE>( rn, dst );
  // }
};

template <class ARCH> struct DC<ARCH,BSWAP> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\x0f" ) & VarByte<0xc8,0xf8>() ))
  
    {
      if (ic.opsize()==16) return new Bswap<ARCH,16>( _.opbase(), _.vbval() );
      if (ic.opsize()==32) return new Bswap<ARCH,32>( _.opbase(), _.vbval() );
      if (ic.opsize()==64) return new Bswap<ARCH,64>( _.opbase(), _.vbval() );
      return 0;
    }
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE>
struct Movnti : public Operation<ARCH>
{
  Movnti( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "movnti " << DisasmR<OPSIZE>( gn ) << ',' << DisasmM( rmop ); }
};

template <class ARCH> struct DC<ARCH,MOVNTI> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\x0f\xc3" ) & RM() ))
  
    {
      if (ic.opsize()==16) return new Movnti<ARCH,16>( _.opbase(), _.rmop(), _.greg() );
      if (ic.opsize()==32) return new Movnti<ARCH,32>( _.opbase(), _.rmop(), _.greg() );
      if (ic.opsize()==64) return new Movnti<ARCH,64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE>
struct LoadFarPointer : public Operation<ARCH>
{
  LoadFarPointer( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn, uint8_t _seg )
    : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ), seg( _seg ) {} RMOp<ARCH> rmop; uint8_t gn; uint8_t seg;
  void disasm( std::ostream& sink ) const { sink << "l" << DisasmS( seg ) << ' ' << DisasmM( rmop ) << ',' << DisasmR<OPSIZE>( gn ); }
};

template <class ARCH> struct DC<ARCH,LFP> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  // LDS/LES/LFS/LGS/LSS -- Load Far Pointer
  
  if (auto _ = match( ic, opcode( "\xc5" ) & RM_mem() ))
  
    {
      if (ic.mode64()) return 0;
      if (ic.opsize()==16) return new LoadFarPointer<ARCH,16>( _.opbase(), _.rmop(), _.greg(), DS );
      if (ic.opsize()==32) return new LoadFarPointer<ARCH,32>( _.opbase(), _.rmop(), _.greg(), DS );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\x0f\xb2" ) & RM_mem() ))
  
    {
      if (ic.opsize()==16) return new LoadFarPointer<ARCH,16>( _.opbase(), _.rmop(), _.greg(), SS );
      if (ic.opsize()==32) return new LoadFarPointer<ARCH,32>( _.opbase(), _.rmop(), _.greg(), SS );
      if (ic.opsize()==64) return new LoadFarPointer<ARCH,64>( _.opbase(), _.rmop(), _.greg(), SS );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\xc4" ) & RM_mem() ))
  
    {
      if (ic.mode64()) return 0;
      if (ic.opsize()==16) return new LoadFarPointer<ARCH,16>( _.opbase(), _.rmop(), _.greg(), ES );
      if (ic.opsize()==32) return new LoadFarPointer<ARCH,32>( _.opbase(), _.rmop(), _.greg(), ES );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\x0f\xb4" ) & RM_mem() ))
  
    {
      if (ic.opsize()==16) return new LoadFarPointer<ARCH,16>( _.opbase(), _.rmop(), _.greg(), FS );
      if (ic.opsize()==32) return new LoadFarPointer<ARCH,32>( _.opbase(), _.rmop(), _.greg(), FS );
      if (ic.opsize()==64) return new LoadFarPointer<ARCH,64>( _.opbase(), _.rmop(), _.greg(), FS );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\x0f\xb5" ) & RM_mem() ))
  
    {
      if (ic.opsize()==16) return new LoadFarPointer<ARCH,16>( _.opbase(), _.rmop(), _.greg(), GS );
      if (ic.opsize()==32) return new LoadFarPointer<ARCH,32>( _.opbase(), _.rmop(), _.greg(), GS );
      if (ic.opsize()==64) return new LoadFarPointer<ARCH,64>( _.opbase(), _.rmop(), _.greg(), GS );
      return 0;
    }
  
  return 0;
}};

