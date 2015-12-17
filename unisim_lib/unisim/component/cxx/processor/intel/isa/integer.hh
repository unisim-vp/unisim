// TODO: check for clearing operation (sub and xor with same registers)
template <unsigned OPSIZE, bool GTOE>
struct AddRM : public Operation
{
  AddRM( OpBase const& opbase, MOp* _rmop, uint8_t _gn ) : Operation( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp rmop; uint8_t gn;
  
  void disasm( std::ostream& sink ) const {
    if (GTOE) sink << "add " << DisasmR<OPSIZE>( gn ) << ',' << DisasmE<OPSIZE>( rmop );
    else      sink << "add " << DisasmE<OPSIZE>( rmop ) << ',' << DisasmR<OPSIZE>( gn );
  }
  
  typedef typename TypeFor<OPSIZE>::u u_type;
  
  void execute( Arch& arch ) const
  {
    if (GTOE) {
      u_type res = eval_add( arch, arch.rmread<OPSIZE>( rmop ), arch.regread<OPSIZE>( gn ) );
      arch.rmwrite<OPSIZE>( rmop, res );
    } else {
      u_type res = eval_add( arch, arch.regread<OPSIZE>( gn ), arch.rmread<OPSIZE>( rmop ) );
      arch.regwrite<OPSIZE>( gn, res );
    }
  }
};

template <unsigned OPSIZE>
struct AddRMI : public Operation
{
  typedef typename TypeFor<OPSIZE>::s s_type;
  typedef typename TypeFor<OPSIZE>::u u_type;
  AddRMI( OpBase const& opbase, MOp* _rmop, s_type _imm ) : Operation( opbase ), rmop( _rmop ), imm( u_type( _imm ) ) {} RMOp rmop; u_type imm;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "add", rmop ) << DisasmI( imm ) << ',' << DisasmE<OPSIZE>( rmop ); }

  void execute( Arch& arch ) const { arch.rmwrite<OPSIZE>( rmop, eval_add( arch, arch.rmread<OPSIZE>( rmop ), imm ) ); }
};

template <> Operation* decode<ADD>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\000" ) & RM() ))
    return new AddRM<8,true>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( cb, opcode( "\001" ) & RM() ))
    {
      if      (cb.opsize() == 16) return new AddRM<16,true>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 32) return new AddRM<32,true>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 64) return new AddRM<64,true>( _.opbase(), _.rmop(), _.greg() );
      else throw 0;
    };

  if (auto _ = match( cb, opcode( "\002" ) & RM() ))
    return new AddRM<8,false>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( cb, opcode( "\003" ) & RM() ))
    {
      if      (cb.opsize() == 16) return new AddRM<16,false>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 32) return new AddRM<32,false>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 64) return new AddRM<64,false>( _.opbase(), _.rmop(), _.greg() );
      else throw 0;
    };

  if (auto _ = match( cb, opcode( "\004" ) & Imm<8>() ))
    return new AddRMI<8>( _.opbase(), RM::MkReg( 0 ), _.imm<int8_t>() );
  
  if (auto _ = match( cb, OpSize<16>() & opcode( "\005" ) & Imm<16>() ))
    return new AddRMI<16>( _.opbase(), RM::MkReg( 0 ), _.imm<int16_t>() );
  
  if (auto _ = match( cb, OpSize<32>() & opcode( "\005" ) & Imm<32>() ))
    return new AddRMI<32>( _.opbase(), RM::MkReg( 0 ), _.imm<int32_t>() );
  
  if (auto _ = match( cb, OpSize<64>() & opcode( "\005" ) & Imm<32>() ))
    return new AddRMI<64>( _.opbase(), RM::MkReg( 0 ), _.imm<int32_t>() );

  if (auto _ = match( cb, opcode( "\x80", 0 ) & RM() & Imm<8>() ))
    return new AddRMI<8>( _.opbase(), _.rmop(), _.imm<int8_t>() );

  if (auto _ = match( cb, OpSize<16>() & opcode( "\x81", 0 ) & RM() & Imm<16>() ))
    return new AddRMI<16>( _.opbase(), _.rmop(), _.imm<int16_t>() );

  if (auto _ = match( cb, OpSize<32>() & opcode( "\x81", 0 ) & RM() & Imm<32>() ))
    return new AddRMI<32>( _.opbase(), _.rmop(), _.imm<int32_t>() );

  if (auto _ = match( cb, OpSize<64>() & opcode( "\x81", 0 ) & RM() & Imm<32>() ))
    return new AddRMI<64>( _.opbase(), _.rmop(), _.imm<int32_t>() );

  if (auto _ = match( cb, opcode( "\x83", 0 ) & RM() & Imm<8>() ))
    {
      if      (cb.opsize() == 16) return new AddRMI<16>( _.opbase(), _.rmop(), _.imm<int16_t>() );
      else if (cb.opsize() == 32) return new AddRMI<32>( _.opbase(), _.rmop(), _.imm<int32_t>() );
      else if (cb.opsize() == 64) return new AddRMI<64>( _.opbase(), _.rmop(), _.imm<int32_t>() );
      else throw 0;
    }
  return 0;
}

template <unsigned OPSIZE, bool GTOE>
struct OrRM : public Operation
{
  OrRM( OpBase const& opbase, MOp* _rmop, uint8_t _gn ) : Operation( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp rmop; uint8_t gn;
  
  void disasm( std::ostream& sink ) const {
    if (GTOE) sink << "or " << DisasmR<OPSIZE>( gn ) << ',' << DisasmE<OPSIZE>( rmop );
    else      sink << "or " << DisasmE<OPSIZE>( rmop ) << ',' << DisasmR<OPSIZE>( gn );
  }
  
  typedef typename TypeFor<OPSIZE>::u u_type;
  
  void execute( Arch& arch ) const
  {
    if (GTOE) {
      u_type res = eval_or( arch, arch.rmread<OPSIZE>( rmop ), arch.regread<OPSIZE>( gn ) );
      arch.rmwrite<OPSIZE>( rmop, res );
    } else {
      u_type res = eval_or( arch, arch.regread<OPSIZE>( gn ), arch.rmread<OPSIZE>( rmop ) );
      arch.regwrite<OPSIZE>( gn, res );
    }
  }
};

template <unsigned OPSIZE>
struct OrRMI : public Operation
{
  typedef typename TypeFor<OPSIZE>::s s_type;
  typedef typename TypeFor<OPSIZE>::u u_type;
  OrRMI( OpBase const& opbase, MOp* _rmop, s_type _imm ) : Operation( opbase ), rmop( _rmop ), imm( u_type( _imm ) ) {} RMOp rmop; u_type imm;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "or", rmop ) << DisasmI( imm ) << ',' << DisasmE<OPSIZE>( rmop ); }

  void execute( Arch& arch ) const { arch.rmwrite<OPSIZE>( rmop, eval_or( arch, arch.rmread<OPSIZE>( rmop ), imm ) ); }
};

template <> Operation* decode<OR>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\010" ) & RM() ))
    return new OrRM<8,true>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( cb, opcode( "\011" ) & RM() ))
    {
      if      (cb.opsize() == 16) return new OrRM<16,true>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 32) return new OrRM<32,true>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 64) return new OrRM<64,true>( _.opbase(), _.rmop(), _.greg() );
      else throw 0;
    };

  if (auto _ = match( cb, opcode( "\012" ) & RM() ))
    return new OrRM<8,false>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( cb, opcode( "\013" ) & RM() ))
    {
      if      (cb.opsize() == 16) return new OrRM<16,false>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 32) return new OrRM<32,false>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 64) return new OrRM<64,false>( _.opbase(), _.rmop(), _.greg() );
      else throw 0;
    };

  if (auto _ = match( cb, opcode( "\014" ) & Imm<8>() ))
    return new OrRMI<8>( _.opbase(), RM::MkReg( 0 ), _.imm<int8_t>() );
  
  if (auto _ = match( cb, OpSize<16>() & opcode( "\015" ) & Imm<16>() ))
    return new OrRMI<16>( _.opbase(), RM::MkReg( 0 ), _.imm<int16_t>() );
  
  if (auto _ = match( cb, OpSize<32>() & opcode( "\015" ) & Imm<32>() ))
    return new OrRMI<32>( _.opbase(), RM::MkReg( 0 ), _.imm<int32_t>() );
  
  if (auto _ = match( cb, OpSize<64>() & opcode( "\015" ) & Imm<32>() ))
    return new OrRMI<64>( _.opbase(), RM::MkReg( 0 ), _.imm<int32_t>() );

  if (auto _ = match( cb, opcode( "\x80", 1 ) & RM() & Imm<8>() ))
    return new OrRMI<8>( _.opbase(), _.rmop(), _.imm<int8_t>() );

  if (auto _ = match( cb, OpSize<16>() & opcode( "\x81", 1 ) & RM() & Imm<16>() ))
    return new OrRMI<16>( _.opbase(), _.rmop(), _.imm<int16_t>() );

  if (auto _ = match( cb, OpSize<32>() & opcode( "\x81", 1 ) & RM() & Imm<32>() ))
    return new OrRMI<32>( _.opbase(), _.rmop(), _.imm<int32_t>() );

  if (auto _ = match( cb, OpSize<64>() & opcode( "\x81", 1 ) & RM() & Imm<32>() ))
    return new OrRMI<64>( _.opbase(), _.rmop(), _.imm<int32_t>() );

  if (auto _ = match( cb, opcode( "\x83", 1 ) & RM() & Imm<8>() ))
    {
      if      (cb.opsize() == 16) return new OrRMI<16>( _.opbase(), _.rmop(), _.imm<int16_t>() );
      else if (cb.opsize() == 32) return new OrRMI<32>( _.opbase(), _.rmop(), _.imm<int32_t>() );
      else if (cb.opsize() == 64) return new OrRMI<64>( _.opbase(), _.rmop(), _.imm<int32_t>() );
      else throw 0;
    }
  return 0;
}

template <unsigned OPSIZE, bool GTOE>
struct AdcRM : public Operation
{
  AdcRM( OpBase const& opbase, MOp* _rmop, uint8_t _gn ) : Operation( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp rmop; uint8_t gn;
  
  void disasm( std::ostream& sink ) const {
    if (GTOE) sink << "adc " << DisasmR<OPSIZE>( gn ) << ',' << DisasmE<OPSIZE>( rmop );
    else      sink << "adc " << DisasmE<OPSIZE>( rmop ) << ',' << DisasmR<OPSIZE>( gn );
  }
  
  typedef typename TypeFor<OPSIZE>::u u_type;
  
  void execute( Arch& arch ) const
  {
    if (GTOE) {
      u_type res = eval_adc( arch, arch.rmread<OPSIZE>( rmop ), arch.regread<OPSIZE>( gn ) );
      arch.rmwrite<OPSIZE>( rmop, res );
    } else {
      u_type res = eval_adc( arch, arch.regread<OPSIZE>( gn ), arch.rmread<OPSIZE>( rmop ) );
      arch.regwrite<OPSIZE>( gn, res );
    }
  }
};

template <unsigned OPSIZE>
struct AdcRMI : public Operation
{
  typedef typename TypeFor<OPSIZE>::s s_type;
  typedef typename TypeFor<OPSIZE>::u u_type;
  AdcRMI( OpBase const& opbase, MOp* _rmop, s_type _imm ) : Operation( opbase ), rmop( _rmop ), imm( u_type( _imm ) ) {} RMOp rmop; u_type imm;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "adc", rmop ) << DisasmI( imm ) << ',' << DisasmE<OPSIZE>( rmop ); }

  void execute( Arch& arch ) const { arch.rmwrite<OPSIZE>( rmop, eval_adc( arch, arch.rmread<OPSIZE>( rmop ), imm ) ); }
};

template <> Operation* decode<ADC>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\020" ) & RM() ))
    return new AdcRM<8,true>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( cb, opcode( "\021" ) & RM() ))
    {
      if      (cb.opsize() == 16) return new AdcRM<16,true>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 32) return new AdcRM<32,true>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 64) return new AdcRM<64,true>( _.opbase(), _.rmop(), _.greg() );
      else throw 0;
    };

  if (auto _ = match( cb, opcode( "\022" ) & RM() ))
    return new AdcRM<8,false>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( cb, opcode( "\023" ) & RM() ))
    {
      if      (cb.opsize() == 16) return new AdcRM<16,false>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 32) return new AdcRM<32,false>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 64) return new AdcRM<64,false>( _.opbase(), _.rmop(), _.greg() );
      else throw 0;
    };

  if (auto _ = match( cb, opcode( "\024" ) & Imm<8>() ))
    return new AdcRMI<8>( _.opbase(), RM::MkReg( 0 ), _.imm<int8_t>() );
  
  if (auto _ = match( cb, OpSize<16>() & opcode( "\025" ) & Imm<16>() ))
    return new AdcRMI<16>( _.opbase(), RM::MkReg( 0 ), _.imm<int16_t>() );
  
  if (auto _ = match( cb, OpSize<32>() & opcode( "\025" ) & Imm<32>() ))
    return new AdcRMI<32>( _.opbase(), RM::MkReg( 0 ), _.imm<int32_t>() );
  
  if (auto _ = match( cb, OpSize<64>() & opcode( "\025" ) & Imm<32>() ))
    return new AdcRMI<64>( _.opbase(), RM::MkReg( 0 ), _.imm<int32_t>() );

  if (auto _ = match( cb, opcode( "\x80", 2 ) & RM() & Imm<8>() ))
    return new AdcRMI<8>( _.opbase(), _.rmop(), _.imm<int8_t>() );

  if (auto _ = match( cb, OpSize<16>() & opcode( "\x81", 2 ) & RM() & Imm<16>() ))
    return new AdcRMI<16>( _.opbase(), _.rmop(), _.imm<int16_t>() );

  if (auto _ = match( cb, OpSize<32>() & opcode( "\x81", 2 ) & RM() & Imm<32>() ))
    return new AdcRMI<32>( _.opbase(), _.rmop(), _.imm<int32_t>() );

  if (auto _ = match( cb, OpSize<64>() & opcode( "\x81", 2 ) & RM() & Imm<32>() ))
    return new AdcRMI<64>( _.opbase(), _.rmop(), _.imm<int32_t>() );

  if (auto _ = match( cb, opcode( "\x83", 2 ) & RM() & Imm<8>() ))
    {
      if      (cb.opsize() == 16) return new AdcRMI<16>( _.opbase(), _.rmop(), _.imm<int16_t>() );
      else if (cb.opsize() == 32) return new AdcRMI<32>( _.opbase(), _.rmop(), _.imm<int32_t>() );
      else if (cb.opsize() == 64) return new AdcRMI<64>( _.opbase(), _.rmop(), _.imm<int32_t>() );
      else throw 0;
    }
  return 0;
}

template <unsigned OPSIZE, bool GTOE>
struct SbbRM : public Operation
{
  SbbRM( OpBase const& opbase, MOp* _rmop, uint8_t _gn ) : Operation( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp rmop; uint8_t gn;
  
  void disasm( std::ostream& sink ) const {
    if (GTOE) sink << "sbb " << DisasmR<OPSIZE>( gn ) << ',' << DisasmE<OPSIZE>( rmop );
    else      sink << "sbb " << DisasmE<OPSIZE>( rmop ) << ',' << DisasmR<OPSIZE>( gn );
  }
  
  typedef typename TypeFor<OPSIZE>::u u_type;
  
  void execute( Arch& arch ) const
  {
    if (GTOE) {
      u_type res = eval_sbb( arch, arch.rmread<OPSIZE>( rmop ), arch.regread<OPSIZE>( gn ) );
      arch.rmwrite<OPSIZE>( rmop, res );
    } else {
      u_type res = eval_sbb( arch, arch.regread<OPSIZE>( gn ), arch.rmread<OPSIZE>( rmop ) );
      arch.regwrite<OPSIZE>( gn, res );
    }
  }
};

template <unsigned OPSIZE>
struct SbbRMI : public Operation
{
  typedef typename TypeFor<OPSIZE>::s s_type;
  typedef typename TypeFor<OPSIZE>::u u_type;
  SbbRMI( OpBase const& opbase, MOp* _rmop, s_type _imm ) : Operation( opbase ), rmop( _rmop ), imm( u_type( _imm ) ) {} RMOp rmop; u_type imm;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "sbb", rmop ) << DisasmI( imm ) << ',' << DisasmE<OPSIZE>( rmop ); }

  void execute( Arch& arch ) const { arch.rmwrite<OPSIZE>( rmop, eval_sbb( arch, arch.rmread<OPSIZE>( rmop ), imm ) ); }
};

template <> Operation* decode<SBB>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\030" ) & RM() ))
    return new SbbRM<8,true>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( cb, opcode( "\031" ) & RM() ))
    {
      if      (cb.opsize() == 16) return new SbbRM<16,true>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 32) return new SbbRM<32,true>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 64) return new SbbRM<64,true>( _.opbase(), _.rmop(), _.greg() );
      else throw 0;
    };

  if (auto _ = match( cb, opcode( "\032" ) & RM() ))
    return new SbbRM<8,false>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( cb, opcode( "\033" ) & RM() ))
    {
      if      (cb.opsize() == 16) return new SbbRM<16,false>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 32) return new SbbRM<32,false>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 64) return new SbbRM<64,false>( _.opbase(), _.rmop(), _.greg() );
      else throw 0;
    };

  if (auto _ = match( cb, opcode( "\034" ) & Imm<8>() ))
    return new SbbRMI<8>( _.opbase(), RM::MkReg( 0 ), _.imm<int8_t>() );
  
  if (auto _ = match( cb, OpSize<16>() & opcode( "\035" ) & Imm<16>() ))
    return new SbbRMI<16>( _.opbase(), RM::MkReg( 0 ), _.imm<int16_t>() );
  
  if (auto _ = match( cb, OpSize<32>() & opcode( "\035" ) & Imm<32>() ))
    return new SbbRMI<32>( _.opbase(), RM::MkReg( 0 ), _.imm<int32_t>() );
  
  if (auto _ = match( cb, OpSize<64>() & opcode( "\035" ) & Imm<32>() ))
    return new SbbRMI<64>( _.opbase(), RM::MkReg( 0 ), _.imm<int32_t>() );

  if (auto _ = match( cb, opcode( "\x80", 3 ) & RM() & Imm<8>() ))
    return new SbbRMI<8>( _.opbase(), _.rmop(), _.imm<int8_t>() );

  if (auto _ = match( cb, OpSize<16>() & opcode( "\x81", 3 ) & RM() & Imm<16>() ))
    return new SbbRMI<16>( _.opbase(), _.rmop(), _.imm<int16_t>() );

  if (auto _ = match( cb, OpSize<32>() & opcode( "\x81", 3 ) & RM() & Imm<32>() ))
    return new SbbRMI<32>( _.opbase(), _.rmop(), _.imm<int32_t>() );

  if (auto _ = match( cb, OpSize<64>() & opcode( "\x81", 3 ) & RM() & Imm<32>() ))
    return new SbbRMI<64>( _.opbase(), _.rmop(), _.imm<int32_t>() );

  if (auto _ = match( cb, opcode( "\x83", 3 ) & RM() & Imm<8>() ))
    {
      if      (cb.opsize() == 16) return new SbbRMI<16>( _.opbase(), _.rmop(), _.imm<int16_t>() );
      else if (cb.opsize() == 32) return new SbbRMI<32>( _.opbase(), _.rmop(), _.imm<int32_t>() );
      else if (cb.opsize() == 64) return new SbbRMI<64>( _.opbase(), _.rmop(), _.imm<int32_t>() );
      else throw 0;
    }
  return 0;
}

template <unsigned OPSIZE, bool GTOE>
struct AndRM : public Operation
{
  AndRM( OpBase const& opbase, MOp* _rmop, uint8_t _gn ) : Operation( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp rmop; uint8_t gn;
  
  void disasm( std::ostream& sink ) const {
    if (GTOE) sink << "and " << DisasmR<OPSIZE>( gn ) << ',' << DisasmE<OPSIZE>( rmop );
    else      sink << "and " << DisasmE<OPSIZE>( rmop ) << ',' << DisasmR<OPSIZE>( gn );
  }
  
  typedef typename TypeFor<OPSIZE>::u u_type;
  
  void execute( Arch& arch ) const
  {
    if (GTOE) {
      u_type res = eval_and( arch, arch.rmread<OPSIZE>( rmop ), arch.regread<OPSIZE>( gn ) );
      arch.rmwrite<OPSIZE>( rmop, res );
    } else {
      u_type res = eval_and( arch, arch.regread<OPSIZE>( gn ), arch.rmread<OPSIZE>( rmop ) );
      arch.regwrite<OPSIZE>( gn, res );
    }
  }
};

template <unsigned OPSIZE>
struct AndRMI : public Operation
{
  typedef typename TypeFor<OPSIZE>::s s_type;
  typedef typename TypeFor<OPSIZE>::u u_type;
  AndRMI( OpBase const& opbase, MOp* _rmop, s_type _imm ) : Operation( opbase ), rmop( _rmop ), imm( u_type( _imm ) ) {} RMOp rmop; u_type imm;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "and", rmop ) << DisasmI( imm ) << ',' << DisasmE<OPSIZE>( rmop ); }
  
  void execute( Arch& arch ) const { arch.rmwrite<OPSIZE>( rmop, eval_and( arch, arch.rmread<OPSIZE>( rmop ), imm ) ); }
};

template <> Operation* decode<AND>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\040" ) & RM() ))
    return new AndRM<8,true>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( cb, opcode( "\041" ) & RM() ))
    {
      if      (cb.opsize() == 16) return new AndRM<16,true>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 32) return new AndRM<32,true>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 64) return new AndRM<64,true>( _.opbase(), _.rmop(), _.greg() );
      else throw 0;
    };

  if (auto _ = match( cb, opcode( "\042" ) & RM() ))
    return new AndRM<8,false>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( cb, opcode( "\043" ) & RM() ))
    {
      if      (cb.opsize() == 16) return new AndRM<16,false>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 32) return new AndRM<32,false>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 64) return new AndRM<64,false>( _.opbase(), _.rmop(), _.greg() );
      else throw 0;
    };

  if (auto _ = match( cb, opcode( "\044" ) & Imm<8>() ))
    return new AndRMI<8>( _.opbase(), RM::MkReg( 0 ), _.imm<int8_t>() );
  
  if (auto _ = match( cb, OpSize<16>() & opcode( "\045" ) & Imm<16>() ))
    return new AndRMI<16>( _.opbase(), RM::MkReg( 0 ), _.imm<int16_t>() );
  
  if (auto _ = match( cb, OpSize<32>() & opcode( "\045" ) & Imm<32>() ))
    return new AndRMI<32>( _.opbase(), RM::MkReg( 0 ), _.imm<int32_t>() );
  
  if (auto _ = match( cb, OpSize<64>() & opcode( "\045" ) & Imm<32>() ))
    return new AndRMI<64>( _.opbase(), RM::MkReg( 0 ), _.imm<int32_t>() );

  if (auto _ = match( cb, opcode( "\x80", 4 ) & RM() & Imm<8>() ))
    return new AndRMI<8>( _.opbase(), _.rmop(), _.imm<int8_t>() );

  if (auto _ = match( cb, OpSize<16>() & opcode( "\x81", 4 ) & RM() & Imm<16>() ))
    return new AndRMI<16>( _.opbase(), _.rmop(), _.imm<int16_t>() );

  if (auto _ = match( cb, OpSize<32>() & opcode( "\x81", 4 ) & RM() & Imm<32>() ))
    return new AndRMI<32>( _.opbase(), _.rmop(), _.imm<int32_t>() );

  if (auto _ = match( cb, OpSize<64>() & opcode( "\x81", 4 ) & RM() & Imm<32>() ))
    return new AndRMI<64>( _.opbase(), _.rmop(), _.imm<int32_t>() );

  if (auto _ = match( cb, opcode( "\x83", 4 ) & RM() & Imm<8>() ))
    {
      if      (cb.opsize() == 16) return new AndRMI<16>( _.opbase(), _.rmop(), _.imm<int16_t>() );
      else if (cb.opsize() == 32) return new AndRMI<32>( _.opbase(), _.rmop(), _.imm<int32_t>() );
      else if (cb.opsize() == 64) return new AndRMI<64>( _.opbase(), _.rmop(), _.imm<int32_t>() );
      else throw 0;
    }
  return 0;
}

template <unsigned OPSIZE, bool GTOE>
struct SubRM : public Operation
{
  SubRM( OpBase const& opbase, MOp* _rmop, uint8_t _gn ) : Operation( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp rmop; uint8_t gn;
  
  void disasm( std::ostream& sink ) const {
    if (GTOE) sink << "sub " << DisasmR<OPSIZE>( gn ) << ',' << DisasmE<OPSIZE>( rmop );
    else      sink << "sub " << DisasmE<OPSIZE>( rmop ) << ',' << DisasmR<OPSIZE>( gn );
  }
  
  typedef typename TypeFor<OPSIZE>::u u_type;
  
  void execute( Arch& arch ) const
  {
    if (GTOE) {
      u_type res = eval_sub( arch, arch.rmread<OPSIZE>( rmop ), arch.regread<OPSIZE>( gn ) );
      arch.rmwrite<OPSIZE>( rmop, res );
    } else {
      u_type res = eval_sub( arch, arch.regread<OPSIZE>( gn ), arch.rmread<OPSIZE>( rmop ) );
      arch.regwrite<OPSIZE>( gn, res );
    }
  }
};

template <unsigned OPSIZE>
struct SubRMI : public Operation
{
  typedef typename TypeFor<OPSIZE>::s s_type;
  typedef typename TypeFor<OPSIZE>::u u_type;
  SubRMI( OpBase const& opbase, MOp* _rmop, s_type _imm ) : Operation( opbase ), rmop( _rmop ), imm( u_type( _imm ) ) {} RMOp rmop; u_type imm;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "sub", rmop ) << DisasmI( imm ) << ',' << DisasmE<OPSIZE>( rmop ); }
  
  void execute( Arch& arch ) const { arch.rmwrite<OPSIZE>( rmop, eval_sub( arch, arch.rmread<OPSIZE>( rmop ), imm ) ); }
};

template <> Operation* decode<SUB>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\050" ) & RM() ))
    return new SubRM<8,true>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( cb, opcode( "\051" ) & RM() ))
    {
      if      (cb.opsize() == 16) return new SubRM<16,true>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 32) return new SubRM<32,true>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 64) return new SubRM<64,true>( _.opbase(), _.rmop(), _.greg() );
      else throw 0;
    };

  if (auto _ = match( cb, opcode( "\052" ) & RM() ))
    return new SubRM<8,false>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( cb, opcode( "\053" ) & RM() ))
    {
      if      (cb.opsize() == 16) return new SubRM<16,false>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 32) return new SubRM<32,false>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 64) return new SubRM<64,false>( _.opbase(), _.rmop(), _.greg() );
      else throw 0;
    };

  if (auto _ = match( cb, opcode( "\054" ) & Imm<8>() ))
    return new SubRMI<8>( _.opbase(), RM::MkReg( 0 ), _.imm<int8_t>() );
  
  if (auto _ = match( cb, OpSize<16>() & opcode( "\055" ) & Imm<16>() ))
    return new SubRMI<16>( _.opbase(), RM::MkReg( 0 ), _.imm<int16_t>() );
  
  if (auto _ = match( cb, OpSize<32>() & opcode( "\055" ) & Imm<32>() ))
    return new SubRMI<32>( _.opbase(), RM::MkReg( 0 ), _.imm<int32_t>() );
  
  if (auto _ = match( cb, OpSize<64>() & opcode( "\055" ) & Imm<32>() ))
    return new SubRMI<64>( _.opbase(), RM::MkReg( 0 ), _.imm<int32_t>() );

  if (auto _ = match( cb, opcode( "\x80", 5 ) & RM() & Imm<8>() ))
    return new SubRMI<8>( _.opbase(), _.rmop(), _.imm<int8_t>() );

  if (auto _ = match( cb, OpSize<16>() & opcode( "\x81", 5 ) & RM() & Imm<16>() ))
    return new SubRMI<16>( _.opbase(), _.rmop(), _.imm<int16_t>() );

  if (auto _ = match( cb, OpSize<32>() & opcode( "\x81", 5 ) & RM() & Imm<32>() ))
    return new SubRMI<32>( _.opbase(), _.rmop(), _.imm<int32_t>() );

  if (auto _ = match( cb, OpSize<64>() & opcode( "\x81", 5 ) & RM() & Imm<32>() ))
    return new SubRMI<64>( _.opbase(), _.rmop(), _.imm<int32_t>() );

  if (auto _ = match( cb, opcode( "\x83", 5 ) & RM() & Imm<8>() ))
    {
      if      (cb.opsize() == 16) return new SubRMI<16>( _.opbase(), _.rmop(), _.imm<int16_t>() );
      else if (cb.opsize() == 32) return new SubRMI<32>( _.opbase(), _.rmop(), _.imm<int32_t>() );
      else if (cb.opsize() == 64) return new SubRMI<64>( _.opbase(), _.rmop(), _.imm<int32_t>() );
      else throw 0;
    }
  return 0;
}

template <unsigned OPSIZE, bool GTOE>
struct XorRM : public Operation
{
  XorRM( OpBase const& opbase, MOp* _rmop, uint8_t _gn ) : Operation( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp rmop; uint8_t gn;
  
  void disasm( std::ostream& sink ) const {
    if (GTOE) sink << "xor " << DisasmR<OPSIZE>( gn ) << ',' << DisasmE<OPSIZE>( rmop );
    else      sink << "xor " << DisasmE<OPSIZE>( rmop ) << ',' << DisasmR<OPSIZE>( gn );
  }
  
  typedef typename TypeFor<OPSIZE>::u u_type;
  
  void execute( Arch& arch ) const
  {
    if (GTOE) {
      u_type res = eval_xor( arch, arch.rmread<OPSIZE>( rmop ), arch.regread<OPSIZE>( gn ) );
      arch.rmwrite<OPSIZE>( rmop, res );
    } else {
      u_type res = eval_xor( arch, arch.regread<OPSIZE>( gn ), arch.rmread<OPSIZE>( rmop ) );
      arch.regwrite<OPSIZE>( gn, res );
    }
  }
};

template <unsigned OPSIZE>
struct XorRMI : public Operation
{
  typedef typename TypeFor<OPSIZE>::s s_type;
  typedef typename TypeFor<OPSIZE>::u u_type;
  XorRMI( OpBase const& opbase, MOp* _rmop, s_type _imm ) : Operation( opbase ), rmop( _rmop ), imm( u_type( _imm ) ) {} RMOp rmop; u_type imm;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "xor", rmop ) << DisasmI( imm ) << ',' << DisasmE<OPSIZE>( rmop ); }
  
  void execute( Arch& arch ) const { arch.rmwrite<OPSIZE>( rmop, eval_xor( arch, arch.rmread<OPSIZE>( rmop ), imm ) ); }
};

template <> Operation* decode<XOR>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\060" ) & RM() ))
    return new XorRM<8,true>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( cb, opcode( "\061" ) & RM() ))
    {
      if      (cb.opsize() == 16) return new XorRM<16,true>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 32) return new XorRM<32,true>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 64) return new XorRM<64,true>( _.opbase(), _.rmop(), _.greg() );
      else throw 0;
    };

  if (auto _ = match( cb, opcode( "\062" ) & RM() ))
    return new XorRM<8,false>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( cb, opcode( "\063" ) & RM() ))
    {
      if      (cb.opsize() == 16) return new XorRM<16,false>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 32) return new XorRM<32,false>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 64) return new XorRM<64,false>( _.opbase(), _.rmop(), _.greg() );
      else throw 0;
    };

  if (auto _ = match( cb, opcode( "\064" ) & Imm<8>() ))
    return new XorRMI<8>( _.opbase(), RM::MkReg( 0 ), _.imm<int8_t>() );
  
  if (auto _ = match( cb, OpSize<16>() & opcode( "\065" ) & Imm<16>() ))
    return new XorRMI<16>( _.opbase(), RM::MkReg( 0 ), _.imm<int16_t>() );
  
  if (auto _ = match( cb, OpSize<32>() & opcode( "\065" ) & Imm<32>() ))
    return new XorRMI<32>( _.opbase(), RM::MkReg( 0 ), _.imm<int32_t>() );
  
  if (auto _ = match( cb, OpSize<64>() & opcode( "\065" ) & Imm<32>() ))
    return new XorRMI<64>( _.opbase(), RM::MkReg( 0 ), _.imm<int32_t>() );

  if (auto _ = match( cb, opcode( "\x80", 6 ) & RM() & Imm<8>() ))
    return new XorRMI<8>( _.opbase(), _.rmop(), _.imm<int8_t>() );

  if (auto _ = match( cb, OpSize<16>() & opcode( "\x81", 6 ) & RM() & Imm<16>() ))
    return new XorRMI<16>( _.opbase(), _.rmop(), _.imm<int16_t>() );

  if (auto _ = match( cb, OpSize<32>() & opcode( "\x81", 6 ) & RM() & Imm<32>() ))
    return new XorRMI<32>( _.opbase(), _.rmop(), _.imm<int32_t>() );

  if (auto _ = match( cb, OpSize<64>() & opcode( "\x81", 6 ) & RM() & Imm<32>() ))
    return new XorRMI<64>( _.opbase(), _.rmop(), _.imm<int32_t>() );

  if (auto _ = match( cb, opcode( "\x83", 6 ) & RM() & Imm<8>() ))
    {
      if      (cb.opsize() == 16) return new XorRMI<16>( _.opbase(), _.rmop(), _.imm<int16_t>() );
      else if (cb.opsize() == 32) return new XorRMI<32>( _.opbase(), _.rmop(), _.imm<int32_t>() );
      else if (cb.opsize() == 64) return new XorRMI<64>( _.opbase(), _.rmop(), _.imm<int32_t>() );
      else throw 0;
    }
  return 0;
}

template <unsigned OPSIZE, bool GTOE>
struct CmpRM : public Operation
{
  CmpRM( OpBase const& opbase, MOp* _rmop, uint8_t _gn ) : Operation( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp rmop; uint8_t gn;
  
  void disasm( std::ostream& sink ) const {
    if (GTOE) sink << "cmp " << DisasmR<OPSIZE>( gn ) << ',' << DisasmE<OPSIZE>( rmop );
    else      sink << "cmp " << DisasmE<OPSIZE>( rmop ) << ',' << DisasmR<OPSIZE>( gn );
  }
  
  typedef typename TypeFor<OPSIZE>::u u_type;
  
  void execute( Arch& arch ) const
  {
    if (GTOE) eval_sub( arch, arch.rmread<OPSIZE>( rmop ), arch.regread<OPSIZE>( gn ) );
    else      eval_sub( arch, arch.regread<OPSIZE>( gn ), arch.rmread<OPSIZE>( rmop ) );
  }
};

template <unsigned OPSIZE>
struct CmpRMI : public Operation
{
  typedef typename TypeFor<OPSIZE>::s s_type;
  typedef typename TypeFor<OPSIZE>::u u_type;
  CmpRMI( OpBase const& opbase, MOp* _rmop, s_type _imm ) : Operation( opbase ), rmop( _rmop ), imm( u_type( _imm ) ) {} RMOp rmop; u_type imm;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "cmp", rmop ) << DisasmI( imm ) << ',' << DisasmE<OPSIZE>( rmop ); }

  void execute( Arch& arch ) const { eval_sub( arch, arch.rmread<OPSIZE>( rmop ), imm ); }
};

template <> Operation* decode<CMP>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\070" ) & RM() ))
    return new CmpRM<8,true>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( cb, opcode( "\071" ) & RM() ))
    {
      if      (cb.opsize() == 16) return new CmpRM<16,true>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 32) return new CmpRM<32,true>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 64) return new CmpRM<64,true>( _.opbase(), _.rmop(), _.greg() );
      else throw 0;
    };

  if (auto _ = match( cb, opcode( "\072" ) & RM() ))
    return new CmpRM<8,false>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( cb, opcode( "\073" ) & RM() ))
    {
      if      (cb.opsize() == 16) return new CmpRM<16,false>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 32) return new CmpRM<32,false>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 64) return new CmpRM<64,false>( _.opbase(), _.rmop(), _.greg() );
      else throw 0;
    };

  if (auto _ = match( cb, opcode( "\074" ) & Imm<8>() ))
    return new CmpRMI<8>( _.opbase(), RM::MkReg( 0 ), _.imm<int8_t>() );
  
  if (auto _ = match( cb, OpSize<16>() & opcode( "\075" ) & Imm<16>() ))
    return new CmpRMI<16>( _.opbase(), RM::MkReg( 0 ), _.imm<int16_t>() );
  
  if (auto _ = match( cb, OpSize<32>() & opcode( "\075" ) & Imm<32>() ))
    return new CmpRMI<32>( _.opbase(), RM::MkReg( 0 ), _.imm<int32_t>() );
  
  if (auto _ = match( cb, OpSize<64>() & opcode( "\075" ) & Imm<32>() ))
    return new CmpRMI<64>( _.opbase(), RM::MkReg( 0 ), _.imm<int32_t>() );

  if (auto _ = match( cb, opcode( "\x80", 7 ) & RM() & Imm<8>() ))
    return new CmpRMI<8>( _.opbase(), _.rmop(), _.imm<int8_t>() );

  if (auto _ = match( cb, OpSize<16>() & opcode( "\x81", 7 ) & RM() & Imm<16>() ))
    return new CmpRMI<16>( _.opbase(), _.rmop(), _.imm<int16_t>() );

  if (auto _ = match( cb, OpSize<32>() & opcode( "\x81", 7 ) & RM() & Imm<32>() ))
    return new CmpRMI<32>( _.opbase(), _.rmop(), _.imm<int32_t>() );

  if (auto _ = match( cb, OpSize<64>() & opcode( "\x81", 7 ) & RM() & Imm<32>() ))
    return new CmpRMI<64>( _.opbase(), _.rmop(), _.imm<int32_t>() );

  if (auto _ = match( cb, opcode( "\x83", 7 ) & RM() & Imm<8>() ))
    {
      if      (cb.opsize() == 16) return new CmpRMI<16>( _.opbase(), _.rmop(), _.imm<int16_t>() );
      else if (cb.opsize() == 32) return new CmpRMI<32>( _.opbase(), _.rmop(), _.imm<int32_t>() );
      else if (cb.opsize() == 64) return new CmpRMI<64>( _.opbase(), _.rmop(), _.imm<int32_t>() );
      else throw 0;
    }
  return 0;
}

/* ROL */

template <unsigned OPSIZE>
struct RolRMI : public Operation
{
  RolRMI( OpBase const& opbase, MOp* _rmop, uint8_t _imm ) : Operation( opbase ), rmop( _rmop ), imm( _imm ) {} RMOp rmop; uint8_t imm;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "rol", rmop ) << DisasmI( imm ) << ',' << DisasmE<OPSIZE>( rmop ); }
  
  void execute( Arch& arch ) const { arch.rmwrite<OPSIZE>( rmop, eval_rol( arch, arch.rmread<OPSIZE>( rmop ), u8_t( imm ) ) ); }
};

template <unsigned OPSIZE>
struct RolRMCL : public Operation
{
  RolRMCL( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "rol", rmop ) << "%cl," << DisasmE<OPSIZE>( rmop ); }
  
  void execute( Arch& arch ) const { arch.rmwrite<OPSIZE>( rmop, eval_rol( arch, arch.rmread<OPSIZE>( rmop ), arch.regread8( 1 ) ) ); }
};

template <> Operation* decode<ROL>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xc0", 0 ) & RM() & Imm<8>() ))
  
    return new RolRMI<8>( _.opbase(), _.rmop(), _.imm<int8_t>() );
  
  if (auto _ = match( cb, opcode( "\xc1", 0 ) & RM() & Imm<8>() ))
  
    {
      if      (cb.opsize() == 16) return new RolRMI<16>( _.opbase(), _.rmop(), _.imm<int8_t>() );
      else if (cb.opsize() == 32) return new RolRMI<32>( _.opbase(), _.rmop(), _.imm<int8_t>() );
      else if (cb.opsize() == 64) return new RolRMI<64>( _.opbase(), _.rmop(), _.imm<int8_t>() );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\xd0", 0 ) & RM() ))
  
    return new RolRMI<8>( _.opbase(), _.rmop(), 1 );
  
  if (auto _ = match( cb, opcode( "\xd1", 0 ) & RM() ))
  
    {
      if      (cb.opsize() == 16) return new RolRMI<16>( _.opbase(), _.rmop(), 1 );
      else if (cb.opsize() == 32) return new RolRMI<32>( _.opbase(), _.rmop(), 1 );
      else if (cb.opsize() == 64) return new RolRMI<64>( _.opbase(), _.rmop(), 1 );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\xd2", 0 ) & RM() ))
  
    return new RolRMCL<8>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xd3", 0 ) & RM() ))
  
    {
      if      (cb.opsize() == 16) return new RolRMCL<16>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 32) return new RolRMCL<32>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 64) return new RolRMCL<64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  return 0;
}

/* ROR */

template <unsigned OPSIZE>
struct RorRMI : public Operation
{
  RorRMI( OpBase const& opbase, MOp* _rmop, uint8_t _imm ) : Operation( opbase ), rmop( _rmop ), imm( _imm ) {} RMOp rmop; uint8_t imm;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "ror", rmop ) << DisasmI( imm ) << ',' << DisasmE<OPSIZE>( rmop ); }
  
  void execute( Arch& arch ) const { arch.rmwrite<OPSIZE>( rmop, eval_ror( arch, arch.rmread<OPSIZE>( rmop ), u8_t( imm ) ) ); }
};

template <unsigned OPSIZE>
struct RorRMCL : public Operation
{
  RorRMCL( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "ror", rmop ) << "%cl," << DisasmE<OPSIZE>( rmop ); }
  
  void execute( Arch& arch ) const { arch.rmwrite<OPSIZE>( rmop, eval_ror( arch, arch.rmread<OPSIZE>( rmop ), arch.regread8( 1 ) ) ); }
};

template <> Operation* decode<ROR>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xc0", 1 ) & RM() & Imm<8>() ))
  
    return new RorRMI<8>( _.opbase(), _.rmop(), _.imm<int8_t>() );
  
  if (auto _ = match( cb, opcode( "\xc1", 1 ) & RM() & Imm<8>() ))
  
    {
      if      (cb.opsize() == 16) return new RorRMI<16>( _.opbase(), _.rmop(), _.imm<int8_t>() );
      else if (cb.opsize() == 32) return new RorRMI<32>( _.opbase(), _.rmop(), _.imm<int8_t>() );
      else if (cb.opsize() == 64) return new RorRMI<64>( _.opbase(), _.rmop(), _.imm<int8_t>() );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\xd0", 1 ) & RM() ))
  
    return new RorRMI<8>( _.opbase(), _.rmop(), 1 );
  
  if (auto _ = match( cb, opcode( "\xd1", 1 ) & RM() ))
  
    {
      if      (cb.opsize() == 16) return new RorRMI<16>( _.opbase(), _.rmop(), 1 );
      else if (cb.opsize() == 32) return new RorRMI<32>( _.opbase(), _.rmop(), 1 );
      else if (cb.opsize() == 64) return new RorRMI<64>( _.opbase(), _.rmop(), 1 );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\xd2", 1 ) & RM() ))
  
    return new RorRMCL<8>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xd3", 1 ) & RM() ))
  
    {
      if      (cb.opsize() == 16) return new RorRMCL<16>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 32) return new RorRMCL<32>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 64) return new RorRMCL<64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  return 0;
}

/* RCL */

template <unsigned OPSIZE>
struct RclRMI : public Operation
{
  RclRMI( OpBase const& opbase, MOp* _rmop, uint8_t _imm ) : Operation( opbase ), rmop( _rmop ), imm( _imm ) {} RMOp rmop; uint8_t imm;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "rcl", rmop ) << DisasmI( imm ) << ',' << DisasmE<OPSIZE>( rmop ); }
  
  void execute( Arch& arch ) const { arch.rmwrite<OPSIZE>( rmop, eval_rcl( arch, arch.rmread<OPSIZE>( rmop ), u8_t( imm ) ) ); }
};

template <unsigned OPSIZE>
struct RclRMCL : public Operation
{
  RclRMCL( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "rcl", rmop ) << "%cl," << DisasmE<OPSIZE>( rmop ); }
  
  void execute( Arch& arch ) const { arch.rmwrite<OPSIZE>( rmop, eval_rcl( arch, arch.rmread<OPSIZE>( rmop ), arch.regread8( 1 ) ) ); }
};

template <> Operation* decode<RCL>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xc0", 2 ) & RM() & Imm<8>() ))
  
    return new RclRMI<8>( _.opbase(), _.rmop(), _.imm<int8_t>() );
  
  if (auto _ = match( cb, opcode( "\xc1", 2 ) & RM() & Imm<8>() ))
  
    {
      if      (cb.opsize() == 16) return new RclRMI<16>( _.opbase(), _.rmop(), _.imm<int8_t>() );
      else if (cb.opsize() == 32) return new RclRMI<32>( _.opbase(), _.rmop(), _.imm<int8_t>() );
      else if (cb.opsize() == 64) return new RclRMI<64>( _.opbase(), _.rmop(), _.imm<int8_t>() );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\xd0", 2 ) & RM() ))
  
    return new RclRMI<8>( _.opbase(), _.rmop(), 1 );
  
  if (auto _ = match( cb, opcode( "\xd1", 2 ) & RM() ))
  
    {
      if      (cb.opsize() == 16) return new RclRMI<16>( _.opbase(), _.rmop(), 1 );
      else if (cb.opsize() == 32) return new RclRMI<32>( _.opbase(), _.rmop(), 1 );
      else if (cb.opsize() == 64) return new RclRMI<64>( _.opbase(), _.rmop(), 1 );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\xd2", 2 ) & RM() ))
  
    return new RclRMCL<8>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xd3", 2 ) & RM() ))
  
    {
      if      (cb.opsize() == 16) return new RclRMCL<16>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 32) return new RclRMCL<32>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 64) return new RclRMCL<64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  return 0;
}

/* RCR */

template <unsigned OPSIZE>
struct RcrRMI : public Operation
{
  RcrRMI( OpBase const& opbase, MOp* _rmop, uint8_t _imm ) : Operation( opbase ), rmop( _rmop ), imm( _imm ) {} RMOp rmop; uint8_t imm;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "rcr", rmop ) << DisasmI( imm ) << ',' << DisasmE<OPSIZE>( rmop ); }
  
  void execute( Arch& arch ) const { arch.rmwrite<OPSIZE>( rmop, eval_rcr( arch, arch.rmread<OPSIZE>( rmop ), u8_t( imm ) ) ); }
};

template <unsigned OPSIZE>
struct RcrRMCL : public Operation
{
  RcrRMCL( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "rcr", rmop ) << "%cl," << DisasmE<OPSIZE>( rmop ); }
  
  void execute( Arch& arch ) const { arch.rmwrite<OPSIZE>( rmop, eval_rcr( arch, arch.rmread<OPSIZE>( rmop ), arch.regread8( 1 ) ) ); }
};

template <> Operation* decode<RCR>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xc0", 3 ) & RM() & Imm<8>() ))
  
    return new RcrRMI<8>( _.opbase(), _.rmop(), _.imm<int8_t>() );
  
  if (auto _ = match( cb, opcode( "\xc1", 3 ) & RM() & Imm<8>() ))
  
    {
      if      (cb.opsize() == 16) return new RcrRMI<16>( _.opbase(), _.rmop(), _.imm<int8_t>() );
      else if (cb.opsize() == 32) return new RcrRMI<32>( _.opbase(), _.rmop(), _.imm<int8_t>() );
      else if (cb.opsize() == 64) return new RcrRMI<64>( _.opbase(), _.rmop(), _.imm<int8_t>() );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\xd0", 3 ) & RM() ))
  
    return new RcrRMI<8>( _.opbase(), _.rmop(), 1 );
  
  if (auto _ = match( cb, opcode( "\xd1", 3 ) & RM() ))
  
    {
      if      (cb.opsize() == 16) return new RcrRMI<16>( _.opbase(), _.rmop(), 1 );
      else if (cb.opsize() == 32) return new RcrRMI<32>( _.opbase(), _.rmop(), 1 );
      else if (cb.opsize() == 64) return new RcrRMI<64>( _.opbase(), _.rmop(), 1 );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\xd2", 3 ) & RM() ))
  
    return new RcrRMCL<8>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xd3", 3 ) & RM() ))
  
    {
      if      (cb.opsize() == 16) return new RcrRMCL<16>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 32) return new RcrRMCL<32>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 64) return new RcrRMCL<64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  return 0;
}

/* SHL */

template <unsigned OPSIZE>
struct ShlRMI : public Operation
{
  ShlRMI( OpBase const& opbase, MOp* _rmop, uint8_t _imm ) : Operation( opbase ), rmop( _rmop ), imm( _imm ) {} RMOp rmop; uint8_t imm;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "shl", rmop ) << DisasmI( imm ) << ',' << DisasmE<OPSIZE>( rmop ); }
  
  void execute( Arch& arch ) const { arch.rmwrite<OPSIZE>( rmop, eval_shl( arch, arch.rmread<OPSIZE>( rmop ), u8_t( imm ) ) ); }
};

template <unsigned OPSIZE>
struct ShlRMCL : public Operation
{
  ShlRMCL( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "shl", rmop ) << "%cl," << DisasmE<OPSIZE>( rmop ); }
  
  void execute( Arch& arch ) const { arch.rmwrite<OPSIZE>( rmop, eval_shl( arch, arch.rmread<OPSIZE>( rmop ), arch.regread8( 1 ) ) ); }
};

template <> Operation* decode<SHL>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xc0", 4 ) & RM() & Imm<8>() ))
  
    return new ShlRMI<8>( _.opbase(), _.rmop(), _.imm<int8_t>() );
  
  if (auto _ = match( cb, opcode( "\xc1", 4 ) & RM() & Imm<8>() ))
  
    {
      if      (cb.opsize() == 16) return new ShlRMI<16>( _.opbase(), _.rmop(), _.imm<int8_t>() );
      else if (cb.opsize() == 32) return new ShlRMI<32>( _.opbase(), _.rmop(), _.imm<int8_t>() );
      else if (cb.opsize() == 64) return new ShlRMI<64>( _.opbase(), _.rmop(), _.imm<int8_t>() );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\xd0", 4 ) & RM() ))
  
    return new ShlRMI<8>( _.opbase(), _.rmop(), 1 );
  
  if (auto _ = match( cb, opcode( "\xd1", 4 ) & RM() ))
  
    {
      if      (cb.opsize() == 16) return new ShlRMI<16>( _.opbase(), _.rmop(), 1 );
      else if (cb.opsize() == 32) return new ShlRMI<32>( _.opbase(), _.rmop(), 1 );
      else if (cb.opsize() == 64) return new ShlRMI<64>( _.opbase(), _.rmop(), 1 );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\xd2", 4 ) & RM() ))
  
    return new ShlRMCL<8>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xd3", 4 ) & RM() ))
  
    {
      if      (cb.opsize() == 16) return new ShlRMCL<16>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 32) return new ShlRMCL<32>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 64) return new ShlRMCL<64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  return 0;
}

/* SHR */

template <unsigned OPSIZE>
struct ShrRMI : public Operation
{
  ShrRMI( OpBase const& opbase, MOp* _rmop, uint8_t _imm ) : Operation( opbase ), rmop( _rmop ), imm( _imm ) {} RMOp rmop; uint8_t imm;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "shr", rmop ) << DisasmI( imm ) << ',' << DisasmE<OPSIZE>( rmop ); }
  
  void execute( Arch& arch ) const { arch.rmwrite<OPSIZE>( rmop, eval_shr( arch, arch.rmread<OPSIZE>( rmop ), u8_t( imm ) ) ); }
};

template <unsigned OPSIZE>
struct ShrRMCL : public Operation
{
  ShrRMCL( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "shr", rmop ) << "%cl," << DisasmE<OPSIZE>( rmop ); }
  
  void execute( Arch& arch ) const { arch.rmwrite<OPSIZE>( rmop, eval_shr( arch, arch.rmread<OPSIZE>( rmop ), arch.regread8( 1 ) ) ); }
};

template <> Operation* decode<SHR>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xc0", 5 ) & RM() & Imm<8>() ))
  
    return new ShrRMI<8>( _.opbase(), _.rmop(), _.imm<int8_t>() );
  
  if (auto _ = match( cb, opcode( "\xc1", 5 ) & RM() & Imm<8>() ))
  
    {
      if      (cb.opsize() == 16) return new ShrRMI<16>( _.opbase(), _.rmop(), _.imm<int8_t>() );
      else if (cb.opsize() == 32) return new ShrRMI<32>( _.opbase(), _.rmop(), _.imm<int8_t>() );
      else if (cb.opsize() == 64) return new ShrRMI<64>( _.opbase(), _.rmop(), _.imm<int8_t>() );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\xd0", 5 ) & RM() ))
  
    return new ShrRMI<8>( _.opbase(), _.rmop(), 1 );
  
  if (auto _ = match( cb, opcode( "\xd1", 5 ) & RM() ))
  
    {
      if      (cb.opsize() == 16) return new ShrRMI<16>( _.opbase(), _.rmop(), 1 );
      else if (cb.opsize() == 32) return new ShrRMI<32>( _.opbase(), _.rmop(), 1 );
      else if (cb.opsize() == 64) return new ShrRMI<64>( _.opbase(), _.rmop(), 1 );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\xd2", 5 ) & RM() ))
  
    return new ShrRMCL<8>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xd3", 5 ) & RM() ))
  
    {
      if      (cb.opsize() == 16) return new ShrRMCL<16>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 32) return new ShrRMCL<32>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 64) return new ShrRMCL<64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  return 0;
}

/* SAR */

template <unsigned OPSIZE>
struct SarRMI : public Operation
{
  SarRMI( OpBase const& opbase, MOp* _rmop, uint8_t _imm ) : Operation( opbase ), rmop( _rmop ), imm( _imm ) {} RMOp rmop; uint8_t imm;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "sar", rmop ) << DisasmI( imm ) << ',' << DisasmE<OPSIZE>( rmop ); }
  
  void execute( Arch& arch ) const { arch.rmwrite<OPSIZE>( rmop, eval_sar( arch, arch.rmread<OPSIZE>( rmop ), u8_t( imm ) ) ); }
};

template <unsigned OPSIZE>
struct SarRMCL : public Operation
{
  SarRMCL( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "sar", rmop ) << "%cl," << DisasmE<OPSIZE>( rmop ); }
  
  void execute( Arch& arch ) const { arch.rmwrite<OPSIZE>( rmop, eval_sar( arch, arch.rmread<OPSIZE>( rmop ), arch.regread8( 1 ) ) ); }
};

template <> Operation* decode<SAR>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xc0", 7 ) & RM() & Imm<8>() ))
  
    return new SarRMI<8>( _.opbase(), _.rmop(), _.imm<int8_t>() );
  
  if (auto _ = match( cb, opcode( "\xc1", 7 ) & RM() & Imm<8>() ))
  
    {
      if      (cb.opsize() == 16) return new SarRMI<16>( _.opbase(), _.rmop(), _.imm<int8_t>() );
      else if (cb.opsize() == 32) return new SarRMI<32>( _.opbase(), _.rmop(), _.imm<int8_t>() );
      else if (cb.opsize() == 64) return new SarRMI<64>( _.opbase(), _.rmop(), _.imm<int8_t>() );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\xd0", 7 ) & RM() ))
  
    return new SarRMI<8>( _.opbase(), _.rmop(), 1 );
  
  if (auto _ = match( cb, opcode( "\xd1", 7 ) & RM() ))
  
    {
      if      (cb.opsize() == 16) return new SarRMI<16>( _.opbase(), _.rmop(), 1 );
      else if (cb.opsize() == 32) return new SarRMI<32>( _.opbase(), _.rmop(), 1 );
      else if (cb.opsize() == 64) return new SarRMI<64>( _.opbase(), _.rmop(), 1 );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\xd2", 7 ) & RM() ))
  
    return new SarRMCL<8>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xd3", 7 ) & RM() ))
  
    {
      if      (cb.opsize() == 16) return new SarRMCL<16>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 32) return new SarRMCL<32>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 64) return new SarRMCL<64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  return 0;
}

template <unsigned OPSIZE>
struct TestRMG : public Operation
{
  TestRMG( OpBase const& opbase, MOp* _rmop, uint8_t _gn ) : Operation( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp rmop; uint8_t gn;
  
  void disasm( std::ostream& sink ) const { sink << "test " << DisasmR<OPSIZE>( gn ) << ',' << DisasmE<OPSIZE>( rmop ); }
  
  void execute( Arch& arch ) const { eval_and( arch, arch.rmread<OPSIZE>( rmop ), arch.regread<OPSIZE>( gn ) ); }
};

template <unsigned OPSIZE>
struct TestRMI : public Operation
{
  typedef typename TypeFor<OPSIZE>::s s_type;
  typedef typename TypeFor<OPSIZE>::u u_type;
  TestRMI( OpBase const& opbase, MOp* _rmop, s_type _imm ) : Operation( opbase ), rmop( _rmop ), imm( u_type( _imm ) ) {} RMOp rmop; u_type imm;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "test", rmop ) << DisasmI( imm ) << ',' << DisasmE<OPSIZE>( rmop ); }
  
  void execute( Arch& arch ) const { eval_and( arch, arch.rmread<OPSIZE>( rmop ), imm ); }
};

template <> Operation* decode<TEST>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\x84" ) & RM() ))
  
    return new TestRMG<8>( _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( cb, opcode( "\x85" ) & RM() ))
  
    {
      if      (cb.opsize() == 16) return new TestRMG<16>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 32) return new TestRMG<32>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 64) return new TestRMG<64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\xf6", 0 ) & RM() & Imm<8>() ))
  
    return new TestRMI<8>( _.opbase(), _.rmop(), _.imm<int8_t>() );
  
  if (auto _ = match( cb, OpSize<16>() & opcode( "\xf7", 0 ) & RM() & Imm<16>() ))
  
    return new TestRMI<16>( _.opbase(), _.rmop(), _.imm<int16_t>() );
  
  if (auto _ = match( cb, OpSize<32>() & opcode( "\xf7", 0 ) & RM() & Imm<32>() ))
  
    return new TestRMI<32>( _.opbase(), _.rmop(), _.imm<int32_t>() );
  
  if (auto _ = match( cb, OpSize<64>() & opcode( "\xf7", 0 ) & RM() & Imm<32>() ))
  
    return new TestRMI<64>( _.opbase(), _.rmop(), _.imm<int32_t>() );
  
  if (auto _ = match( cb, opcode( "\xa8" ) & Imm<8>() ))
  
    return new TestRMI<8>( _.opbase(), RM::MkReg( 0 ), _.imm<int8_t>() );
  
  if (auto _ = match( cb, OpSize<16>() & opcode( "\xa9" ) & Imm<16>() ))
  
    return new TestRMI<16>( _.opbase(), RM::MkReg( 0 ), _.imm<int16_t>() );
  
  if (auto _ = match( cb, OpSize<32>() & opcode( "\xa9" ) & Imm<32>() ))
  
    return new TestRMI<32>( _.opbase(), RM::MkReg( 0 ), _.imm<int32_t>() );
  
  if (auto _ = match( cb, OpSize<64>() & opcode( "\xa9" ) & Imm<32>() ))
  
    return new TestRMI<64>( _.opbase(), RM::MkReg( 0 ), _.imm<int32_t>() );
  
  return 0;
}

template <unsigned OPSIZE>
struct NotRM : public Operation
{
  NotRM( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "not", rmop ) << DisasmE<OPSIZE>( rmop ); }
  
  void execute( Arch& arch ) const { arch.rmwrite<OPSIZE>( rmop, ~(arch.rmread<OPSIZE>( rmop )) ); }
};

template <> Operation* decode<NOT>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xf6", 2 ) & RM() ))
  
    return new NotRM<8>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xf7", 2 ) & RM() ))
  
    {
      if      (cb.opsize() == 16) return new NotRM<16>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 32) return new NotRM<32>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 64) return new NotRM<64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  return 0;
}

template <unsigned OPSIZE>
struct NegRM : public Operation
{
  NegRM( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "neg", rmop ) << DisasmE<OPSIZE>( rmop ); }
  
  void execute( Arch& arch ) const { arch.rmwrite<OPSIZE>( rmop, eval_sub( arch, typename TypeFor<OPSIZE>::u( 0 ), arch.rmread<OPSIZE>( rmop ) ) ); }
};

template <> Operation* decode<NEG>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xf6", 3 ) & RM() ))
  
    return new NegRM<8>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xf7", 3 ) & RM() ))
  
    {
      if      (cb.opsize() == 16) return new NegRM<16>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 32) return new NegRM<32>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 64) return new NegRM<64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  return 0;
}

template <unsigned OPSIZE>
struct DivE : public Operation
{
  DivE( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "div", rmop ) << DisasmE<OPSIZE>( rmop ); }
  
  void execute( Arch& arch ) const
  {
    unsigned const hidx = (OPSIZE == 8) ? 4 : 2;
    typedef typename TypeFor<OPSIZE>::u u_type;
    u_type hi = arch.regread<OPSIZE>( hidx ), lo = arch.regread<OPSIZE>( 0 );
    eval_div( arch, hi, lo, arch.rmread<OPSIZE>( rmop ) );
    arch.regwrite<OPSIZE>( hidx, hi );
    arch.regwrite<OPSIZE>( 0, lo );
  }
};

template <> Operation* decode<DIV>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xf6", 6 ) & RM() ))
  
    return new DivE<8>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xf7", 6 ) & RM() ))
  
    {
      if      (cb.opsize() == 16) return new DivE<16>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 32) return new DivE<32>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 64) return new DivE<64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  return 0;
};

template <unsigned OPSIZE>
struct IDivE : public Operation
{
  IDivE( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "idiv", rmop ) << DisasmE<OPSIZE>( rmop ); }
  
  void execute( Arch& arch ) const {
    unsigned const hidx = (OPSIZE == 8) ? 4 : 2;
    typedef typename TypeFor<OPSIZE>::s s_type;
    typedef typename TypeFor<OPSIZE>::u u_type;
    s_type hi = s_type( arch.regread<OPSIZE>( hidx ) ), lo = s_type( arch.regread<OPSIZE>( 0 ) );
    eval_div( arch, hi, lo, s_type( arch.rmread<OPSIZE>( rmop ) ) );
    arch.regwrite<OPSIZE>( hidx, u_type( hi ) );
    arch.regwrite<OPSIZE>( 0, u_type( lo ) );
  }
};

template <> Operation* decode<IDIV>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xf6", 7 ) & RM() ))
  
    return new IDivE<8>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xf7", 7 ) & RM() ))
  
    {
      if      (cb.opsize() == 16) return new IDivE<16>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 32) return new IDivE<32>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 64) return new IDivE<64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  return 0;
};

template <unsigned OPSIZE>
struct MulE : public Operation
{
  MulE( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "mul", rmop ) << DisasmE<OPSIZE>( rmop ); }
  
  void execute( Arch& arch ) const
  {
    unsigned const hidx = (OPSIZE == 8) ? 4 : 2;
    typename TypeFor<OPSIZE>::u hi, lo = arch.regread<OPSIZE>( 0 );
    eval_mul( arch, hi, lo, arch.rmread<OPSIZE>( rmop ) );
    arch.regwrite<OPSIZE>( hidx, hi );
    arch.regwrite<OPSIZE>( 0, lo );
  }
};

template <> Operation* decode<MUL>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xf6", 4 ) & RM() ))
  
    return new MulE<8>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xf7", 4 ) & RM() ))
  
    {
      if      (cb.opsize() == 16) return new MulE<16>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 32) return new MulE<32>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 64) return new MulE<64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  return 0;
};

template <unsigned OPSIZE>
struct IMulE : public Operation
{
  IMulE( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "imul", rmop ) << DisasmE<OPSIZE>( rmop ); }
  
  void execute( Arch& arch ) const
  {
    unsigned const hidx = (OPSIZE == 8) ? 4 : 2;
    typedef typename TypeFor<OPSIZE>::s s_type;
    typedef typename TypeFor<OPSIZE>::u u_type;
    s_type hi, lo = s_type( arch.regread<OPSIZE>( 0 ) );
    eval_mul( arch, hi, lo, s_type( arch.rmread<OPSIZE>( rmop ) ) );
    arch.regwrite<OPSIZE>( hidx, u_type( hi ) );
    arch.regwrite<OPSIZE>( 0, u_type( lo ) );
  }
};

template <unsigned OPSIZE>
struct IMulGE : public Operation
{
  IMulGE( OpBase const& opbase, MOp* _rmop, uint8_t _gn ) : Operation( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp rmop; uint8_t gn;
  
  void disasm( std::ostream& sink ) const { sink << "imul " << DisasmE<OPSIZE>( rmop )  << ',' << DisasmR<OPSIZE>( gn ); }
  
  void execute( Arch& arch ) const { arch.regwrite<OPSIZE>( gn, arch.regread<OPSIZE>( gn ) * arch.rmread<OPSIZE>( rmop ) ); }
};

template <unsigned OPSIZE>
struct IMulGEI : public Operation
{
  typedef typename TypeFor<OPSIZE>::s s_type;
  IMulGEI( OpBase const& opbase, MOp* _rmop, uint8_t _gn, s_type _imm )
    : Operation( opbase ), rmop( _rmop ), imm( _imm ), gn( _gn ) {} RMOp rmop; s_type imm; uint8_t gn;
  
  void disasm( std::ostream& sink ) const { sink << "imul" << DisasmI( imm ) << ',' << DisasmE<OPSIZE>( rmop )  << ',' << DisasmR<OPSIZE>( gn ); }
  
  void execute( Arch& arch ) const
  {
    typedef typename tpinfo<s_type>::twice twice;
    
    twice res  = twice( imm ) * twice( s_type( arch.rmread<OPSIZE>( rmop ) ) );
    arch.rmwrite<OPSIZE>( rmop, s_type( res ) );
    bit_t flag = res != twice( s_type( res ) );
    arch.flagwrite( OF, flag );
    arch.flagwrite( CF, flag );
  }
};

template <> Operation* decode<IMUL>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xf6", 5 ) & RM() ))
  
    return new IMulE<8>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xf7", 5 ) & RM() ))
  
    {
      if      (cb.opsize() == 16) return new IMulE<16>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 32) return new IMulE<32>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 64) return new IMulE<64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\x0f\xaf" ) & RM() ))
  
    {
      if      (cb.opsize() == 16) return new IMulGE<16>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 32) return new IMulGE<32>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 64) return new IMulGE<64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\x6b" ) & RM() & Imm<8>() ))
  
    {
      if      (cb.opsize() == 16) return new IMulGEI<16>( _.opbase(), _.rmop(), _.greg(), _.imm<int16_t>() );
      else if (cb.opsize() == 32) return new IMulGEI<32>( _.opbase(), _.rmop(), _.greg(), _.imm<int32_t>() );
      //else if (cb.opsize() == 64) return new IMulGEI<64>( _.opbase(), _.rmop(), _.greg(), _.imm<int64_t>() );
      return 0;
    }
  
  if (auto _ = match( cb, OpSize<16>() & opcode( "\x69" ) & RM() & Imm<16>() ))
  
    return new IMulGEI<16>( _.opbase(), _.rmop(), _.greg(), _.imm<int16_t>() );
  
  if (auto _ = match( cb, OpSize<32>() & opcode( "\x69" ) & RM() & Imm<32>() ))
  
    return new IMulGEI<16>( _.opbase(), _.rmop(), _.greg(), _.imm<int32_t>() );
  
  if (auto _ = match( cb, OpSize<64>() & opcode( "\x69" ) & RM() & Imm<32>() ))
  
    return new IMulGEI<16>( _.opbase(), _.rmop(), _.greg(), _.imm<int64_t>() );
  
  return 0;
};

template <unsigned OPSIZE>
struct Lea : public Operation
{
  Lea( OpBase const& opbase, MOp* _rmop, uint8_t _gn ) : Operation( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp rmop; uint8_t gn;
  
  void disasm( std::ostream& sink ) const { sink << "lea " << DisasmM( rmop ) << ',' << DisasmR<OPSIZE>( gn ); }
  
  void execute( Arch& arch ) const { arch.regwrite<OPSIZE>( gn, typename TypeFor<OPSIZE>::u( rmop->effective_address( arch ) ) ); }
};

template <> Operation* decode<LEA>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\x8d" ) & RM_mem() ))
  
    {
      if      (cb.opsize() == 16) return new Lea<16>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 32) return new Lea<32>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 64) return new Lea<64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  return 0;
}

template <unsigned OPSIZE>
struct Inc : public Operation
{
  Inc( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "inc", rmop ) << DisasmE<OPSIZE>( rmop ); }
  
  typedef typename TypeFor<OPSIZE>::u u_type;
  void execute( Arch& arch ) const
  {
    // CF is not affected
    bit_t savedCF = arch.flagread( CF );
    arch.rmwrite<OPSIZE>( rmop, eval_add( arch, arch.rmread<OPSIZE>( rmop ), u_type( 1 ) ) );
    arch.flagwrite( CF, savedCF );
  }
};

template <unsigned OPSIZE>
struct Dec : public Operation
{
  Dec( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "dec", rmop ) << DisasmE<OPSIZE>( rmop ); }
  
  typedef typename TypeFor<OPSIZE>::u u_type;
  void execute( Arch& arch ) const
  {
    // CF is not affected
    bit_t savedCF = arch.flagread( CF );
    arch.rmwrite<OPSIZE>( rmop, eval_sub( arch, arch.rmread<OPSIZE>( rmop ), u_type( 1 ) ) );
    arch.flagwrite( CF, savedCF );
  }
};

template <> Operation* decode<INCDEC>( CodeBase const& cb )
{
  if (auto _ = match( cb, VarByte<0x40,0xF8>() ))

    {
      if      (cb.opsize() == 16) return new Inc<16>( _.opbase(), RM::MkReg( _.vbval() ) );
      else if (cb.opsize() == 32) return new Inc<32>( _.opbase(), RM::MkReg( _.vbval() ) );
      return 0;
    }

  if (auto _ = match( cb, opcode( "\xfe", 0 ) & RM() ))
  
    return new Inc<8>( _.opbase(), _.rmop() );

  if (auto _ = match( cb, opcode( "\xff", 0 ) & RM() ))
  
    {
      if      (cb.opsize() == 16) return new Inc<16>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 32) return new Inc<32>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 64) return new Inc<64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  if (auto _ = match( cb, VarByte<0x48,0xF8>() ))

    {
      if      (cb.opsize() == 16) return new Dec<16>( _.opbase(), RM::MkReg( _.vbval() ) );
      else if (cb.opsize() == 32) return new Dec<32>( _.opbase(), RM::MkReg( _.vbval() ) );
      return 0;
    }

  if (auto _ = match( cb, opcode( "\xfe", 1 ) & RM() ))
  
    return new Dec<8>( _.opbase(), _.rmop() );

  if (auto _ = match( cb, opcode( "\xff", 1 ) & RM() ))
  
    {
      if      (cb.opsize() == 16) return new Dec<16>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 32) return new Dec<32>( _.opbase(), _.rmop() );
      else if (cb.opsize() == 64) return new Dec<64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  return 0;
}

struct SetCC : public Operation
{
  SetCC( OpBase const& opbase, MOp* _rmop, uint8_t _cc ) : Operation( opbase ), rmop( _rmop ), cc( _cc ) {} RMOp rmop; uint8_t cc;
  
  void disasm( std::ostream& sink ) const { sink << "set" << DisasmCond( cc ) << ' ' << DisasmE<8>( rmop ); }
  
  void execute( Arch& arch ) const { arch.rmwrite<8>( rmop, u8_t( eval_cond( arch, cc ) ) ); }
};

template <> Operation* decode<SETCC>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\x0f" ) & VarByte<0x90,0xF0>() & RM() ))
  
    return new SetCC( _.opbase(), _.rmop(), _.vbval() );
  
  return 0;
}

template <unsigned OPSIZE>
struct ShldIM : public Operation
{
  ShldIM( OpBase const& opbase, MOp* _rmop, uint8_t _gn, uint8_t _sh )
    : Operation( opbase ), rmop( _rmop ), gn( _gn ), sh( _sh ) {} RMOp rmop; uint8_t gn; uint8_t sh;
  
  void disasm( std::ostream& sink ) const { sink << "shld " << DisasmI( sh ) << ',' << DisasmR<OPSIZE>( gn ) << ',' << DisasmE<OPSIZE>( rmop ); }

  void execute( Arch& arch ) const
  {
    u8_t shift = sh & 0x1f;
    typename TypeFor<OPSIZE>::u result = eval_shl( arch, arch.rmread<OPSIZE>( rmop ), shift ) | (arch.regread<OPSIZE>( gn ) >> (u8_t( OPSIZE ) - shift));
    arch.rmwrite<OPSIZE>( rmop, result );
  }
};

template <unsigned OPSIZE>
struct ShldCL : public Operation
{
  ShldCL( OpBase const& opbase, MOp* _rmop, uint8_t _gn )
    : Operation( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp rmop; uint8_t gn;
  
  void disasm( std::ostream& sink ) const { sink << "shld %cl," << DisasmR<OPSIZE>( gn ) << ',' << DisasmE<OPSIZE>( rmop ); }

  void execute( Arch& arch ) const
  {
    u8_t shift = arch.regread<8>( 1 ) & u8_t( 0x1f );
    typename TypeFor<OPSIZE>::u result = eval_shl( arch, arch.rmread<OPSIZE>( rmop ), shift ) | (arch.regread<OPSIZE>( gn ) >> (u8_t( OPSIZE ) - shift));
    arch.rmwrite<OPSIZE>( rmop, result );
  }
};

template <unsigned OPSIZE>
struct ShrdIM : public Operation
{
  ShrdIM( OpBase const& opbase, MOp* _rmop, uint8_t _gn, uint8_t _sh )
    : Operation( opbase ), rmop( _rmop ), gn( _gn ), sh( _sh ) {} RMOp rmop; uint8_t gn; uint8_t sh;
  
  void disasm( std::ostream& sink ) const { sink << "shrd " << DisasmI( sh ) << ',' << DisasmR<OPSIZE>( gn ) << ',' << DisasmE<OPSIZE>( rmop ); }

  void execute( Arch& arch ) const
  {
    u8_t shift = sh & 0x1f;
    typename TypeFor<OPSIZE>::u result = eval_shr( arch, arch.rmread<OPSIZE>( rmop ), shift ) | (arch.regread<OPSIZE>( gn ) << (u8_t( OPSIZE ) - shift));
    arch.rmwrite<OPSIZE>( rmop, result );
  }
};

template <unsigned OPSIZE>
struct ShrdCL : public Operation
{
  ShrdCL( OpBase const& opbase, MOp* _rmop, uint8_t _gn )
    : Operation( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp rmop; uint8_t gn;
  
  void disasm( std::ostream& sink ) const { sink << "shrd %cl," << DisasmR<OPSIZE>( gn ) << ',' << DisasmE<OPSIZE>( rmop ); }

  void execute( Arch& arch ) const
  {
    u8_t shift = arch.regread<8>( 1 ) & u8_t( 0x1f );
    typename TypeFor<OPSIZE>::u result = eval_shr( arch, arch.rmread<OPSIZE>( rmop ), shift ) | (arch.regread<OPSIZE>( gn ) << (u8_t( OPSIZE ) - shift));
    arch.rmwrite<OPSIZE>( rmop, result );
  }
};

template <> Operation* decode<SHD>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\x0f\xa4" ) & RM() & Imm<8>() ))
  
    {
      if      (cb.opsize() == 16) return new ShldIM<16>( _.opbase(), _.rmop(), _.greg(), _.imm<uint8_t>() );
      else if (cb.opsize() == 32) return new ShldIM<32>( _.opbase(), _.rmop(), _.greg(), _.imm<uint8_t>() );
      else if (cb.opsize() == 64) return new ShldIM<64>( _.opbase(), _.rmop(), _.greg(), _.imm<uint8_t>() );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\x0f\xa5" ) & RM() ))
  
    {
      if      (cb.opsize() == 16) return new ShldCL<16>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 32) return new ShldCL<32>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 64) return new ShldCL<64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\x0f\xac" ) & RM() & Imm<8>() ))
  
    {
      if      (cb.opsize() == 16) return new ShrdIM<16>( _.opbase(), _.rmop(), _.greg(), _.imm<uint8_t>() );
      else if (cb.opsize() == 32) return new ShrdIM<32>( _.opbase(), _.rmop(), _.greg(), _.imm<uint8_t>() );
      else if (cb.opsize() == 64) return new ShrdIM<64>( _.opbase(), _.rmop(), _.greg(), _.imm<uint8_t>() );
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\x0f\xad" ) & RM() ))
  
    {
      if      (cb.opsize() == 16) return new ShrdCL<16>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 32) return new ShrdCL<32>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 64) return new ShrdCL<64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  return 0;
}

template <unsigned OPSIZE, bool LEAD>
struct BitScan : public Operation
{
  BitScan( OpBase const& opbase, MOp* _rmop, uint8_t _gn ) : Operation( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp rmop; uint8_t gn;
  
  void disasm( std::ostream& sink ) const { sink << (LEAD ? "bsr " : "bsf ") << DisasmE<OPSIZE>( rmop ) << ',' << DisasmR<OPSIZE>( gn ); }
  
  void execute( Arch& arch ) const
  {
    typedef typename TypeFor<OPSIZE>::u u_type;
    u_type src = arch.rmread<OPSIZE>( rmop );
    
    bit_t zero = src == u_type( zero );
    arch.flagwrite( ZF, zero );
    if (mkbool( zero)) return;
    
    u_type res = LEAD ? u_type( OPSIZE-1 ) : u_type( 0 ), mask = u_type(1) << res;
    
    while ((src & mask) != mask)
      {
        if (LEAD) { res -= 1; mask >>= 1; }
        else      { res += 1; mask <<= 1; }
      }
    arch.regwrite<OPSIZE>( gn, res );
  }
};

template <unsigned OPSIZE, bool LEAD>
struct CountZeros : public Operation
{
  CountZeros( OpBase const& opbase, MOp* _rmop, uint8_t _gn ) : Operation( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp rmop; uint8_t gn;
  
  void disasm( std::ostream& sink ) const { sink << (LEAD ? "lzcnt " : "tzcnt ") << DisasmE<OPSIZE>( rmop ) << ',' << DisasmR<OPSIZE>( gn ); }
  
  void execute( Arch& arch ) const
  {
    typedef typename TypeFor<OPSIZE>::u u_type;
    u_type const end = u_type( OPSIZE ), mask = LEAD ? (u_type(1) << (OPSIZE-1)) : u_type(1), zero = u_type( 0 );
    u_type src = arch.rmread<OPSIZE>( rmop ), res = u_type( 0 );
    
    while ((res < end) and ((src & mask) == zero))
      {
        src = LEAD ? (src << 1) : (src >> 1);
        res += 1;
      }
    
    arch.flagwrite( CF, res == end );
    arch.flagwrite( ZF, res == zero );
    
    arch.regwrite<OPSIZE>( gn, res );
  }
};

template <> Operation* decode<ZCNT>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\x0f\xbc" ) & RM() ))
  
    {
      if (not cb.f3()) {
        if      (cb.opsize() == 16) return new BitScan<16,false>( _.opbase(), _.rmop(), _.greg() );
        else if (cb.opsize() == 32) return new BitScan<32,false>( _.opbase(), _.rmop(), _.greg() );
        else if (cb.opsize() == 64) return new BitScan<64,false>( _.opbase(), _.rmop(), _.greg() );
      } else {
        if      (cb.opsize() == 16) return new CountZeros<16,false>( _.opbase(), _.rmop(), _.greg() );
        else if (cb.opsize() == 32) return new CountZeros<32,false>( _.opbase(), _.rmop(), _.greg() );
        else if (cb.opsize() == 64) return new CountZeros<64,false>( _.opbase(), _.rmop(), _.greg() );
      }
      return 0;
    }
  
  if (auto _ = match( cb, opcode( "\x0f\xbd" ) & RM() ))
  
    {
      if (not cb.f3()) {
        if      (cb.opsize() == 16) return new BitScan<16,true>( _.opbase(), _.rmop(), _.greg() );
        else if (cb.opsize() == 32) return new BitScan<32,true>( _.opbase(), _.rmop(), _.greg() );
        else if (cb.opsize() == 64) return new BitScan<64,true>( _.opbase(), _.rmop(), _.greg() );
      } else {
        if      (cb.opsize() == 16) return new CountZeros<16,true>( _.opbase(), _.rmop(), _.greg() );
        else if (cb.opsize() == 32) return new CountZeros<32,true>( _.opbase(), _.rmop(), _.greg() );
        else if (cb.opsize() == 64) return new CountZeros<64,true>( _.opbase(), _.rmop(), _.greg() );
      }
      return 0;
    }
  
  return 0;
}

template <unsigned OPSIZE>
struct Popcnt : public Operation
{
  Popcnt( OpBase const& opbase, MOp* _rmop, uint8_t _gn )  : Operation( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp rmop; uint8_t gn;
  
  void disasm( std::ostream& sink ) const { sink << "popcnt " << DisasmE<OPSIZE>( rmop ) << DisasmR<OPSIZE>( gn ); }
  
  // void execute( Arch& arch ) const
  // {
  //   typedef typename TypeFor<OPSIZE>::u op_type;
  //   op_type const zero = op_type( 0 ), lsb = op_type( 1 );
  //   op_type tmp = rmop->read_e<OPSIZE>( arch ), cnt = op_type( 0 );
    
  //   while (tmp != zero) {
  //     cnt += (tmp & lsb);
  //     tmp >>= 1;
  //   }
    
  //   arch.regwrite<OPSIZE>( gn, cnt );
  // }
};

template <> Operation* decode<POPCNT>( CodeBase const& cb )
{
  if (cb.f0()) return 0;
  
  if (auto _ = match( cb, RPF3() & opcode( "\x0f\xb8" ) & RM() ))
  
    {
      if      (cb.opsize() == 16) return new Popcnt<16>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 32) return new Popcnt<32>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 64) return new Popcnt<64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  return 0;
}

struct Cmc : public Operation
{
  Cmc( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "cmc"; }
  void execute( Arch& arch ) const { arch.flagwrite( CF, not arch.flagread( CF ) ); }
};

template <> Operation* decode<CMC>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xf5" ) ))

    return new Cmc( _.opbase() );

  return 0;
}

struct Sahf : public Operation
{
  Sahf( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "sahf"; }
  void execute( Arch& arch ) const
  {
    u8_t ah = arch.regread8( 4 );
    arch.flagwrite( SF, bit_t( (ah >> 7) & u8_t( 1 ) ) );
    arch.flagwrite( ZF, bit_t( (ah >> 6) & u8_t( 1 ) ) );
    arch.flagwrite( AF, bit_t( (ah >> 4) & u8_t( 1 ) ) );
    arch.flagwrite( PF, bit_t( (ah >> 2) & u8_t( 1 ) ) );
    arch.flagwrite( CF, bit_t( (ah >> 0) & u8_t( 1 ) ) );
  }
};

struct Lahf : public Operation
{
  Lahf( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "lahf"; }
  void execute( Arch& arch ) const
  {
    u8_t ah =
      (u8_t( arch.flagread( SF ) ) << 7) |
      (u8_t( arch.flagread( ZF ) ) << 6) |
      (u8_t(                 0   ) << 5) |
      (u8_t( arch.flagread( AF ) ) << 4) |
      (u8_t(                 0   ) << 3) |
      (u8_t( arch.flagread( PF ) ) << 2) |
      (u8_t(                 1   ) << 1) |
      (u8_t( arch.flagread( CF ) ) << 0);
    arch.regwrite<8>( 4, ah );
  }
};

template <> Operation* decode<AHF>( CodeBase const& cb )
{
  if (cb.f0()) return 0;
  
  if (auto _ = match( cb, opcode( "\x9e" ) ))

    return new Sahf( _.opbase() );

  if (auto _ = match( cb, opcode( "\x9f" ) ))

    return new Lahf( _.opbase() );

  return 0;
}

template <unsigned OPSIZE>
struct Csxe : public Operation
{
  Csxe( OpBase const& opbase ) : Operation( opbase ) {}
  
  void disasm( std::ostream& sink ) const { sink << 'c' << (&"btw\0wtl\0ltq\0"[4*SB<OPSIZE/16>::begin]); }
  void execute( Arch& arch ) const
  {
    typedef typename TypeFor<OPSIZE>::s sop_type;
    typedef typename TypeFor<OPSIZE>::u uop_type;
    arch.regwrite<OPSIZE>( 0, uop_type( sop_type( arch.regread<OPSIZE>( 0 )  << (OPSIZE / 2) ) >> (OPSIZE / 2) ) );
  }
};

template <unsigned OPSIZE>
struct Csx : public Operation
{
  Csx( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << 'c' << (&"wtd\0ltd\0qtd\0"[4*SB<OPSIZE/16>::begin]); }
  // void execute( Arch& arch ) const
  // {
  //   typedef typename TypeFor<OPSIZE>::s sop_type;
  //   typedef typename TypeFor<OPSIZE>::u uop_type;
  //   arch.regwrite<OPSIZE>( 2, uop_type( sop_type( arch.regread<OPSIZE>( 0 )  << (OPSIZE - 1) ) >> (OPSIZE - 1) ) );
  // }
};

template <> Operation* decode<CSX>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\x98" ) ))
  
    {
      if      (cb.opsize() == 16) return new Csxe<16>( _.opbase() );
      else if (cb.opsize() == 32) return new Csxe<32>( _.opbase() );
      else if (cb.opsize() == 64) return new Csxe<64>( _.opbase() );
      return 0;
    }
  
  if (auto _ = match( cb,  opcode( "\x99" ) ))
  
    {
      if      (cb.opsize() == 16) return new Csx<16>( _.opbase() );
      else if (cb.opsize() == 32) return new Csx<32>( _.opbase() );
      else if (cb.opsize() == 64) return new Csx<64>( _.opbase() );
      return 0;
    }
  
  return 0;
}

template <unsigned ADDRSIZE>
struct Xlat : public Operation
{
  Xlat( OpBase const& opbase, uint8_t _segment ) : Operation( opbase ), segment( _segment ) {} uint8_t segment;
  void disasm( std::ostream& sink ) const { sink << "xlat"; }
  // void execute( Arch& arch ) const
  // {
  //   typedef typename TypeFor<ADDRSIZE>::u addr_type;
  //   arch.regwrite<8>( 0, arch.memread<8>( segment, arch.regread<ADDRSIZE>( 3 ) + addr_type( arch.regread<8>( 0 ) ) ) );
  // }
};

template <> Operation* decode<XLAT>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xd7" ) ))

    {
      if      (cb.addrsize() == 16) return new Xlat<16>( _.opbase(), cb.segment );
      else if (cb.addrsize() == 32) return new Xlat<32>( _.opbase(), cb.segment );
      else if (cb.addrsize() == 64) return new Xlat<64>( _.opbase(), cb.segment );
      return 0;
    }

  return 0;
}

struct AAA : public Operation
{
  AAA( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "aaa"; }
};

struct AAS : public Operation
{
  AAS( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "aas"; }
};

struct DAA : public Operation
{
  DAA( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "daa"; }
};

struct DAS : public Operation
{
  DAS( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "das"; }
};

struct AAD : public Operation
{
  AAD( OpBase const& opbase, uint8_t _imm ) : Operation( opbase ), imm( _imm ) {} uint8_t imm;
  void disasm( std::ostream& sink ) const { sink << "aad" << DisasmI( imm ); }
};
  
struct AAM : public Operation
{
  AAM( OpBase const& opbase, uint8_t _imm ) : Operation( opbase ), imm( _imm ) {} uint8_t imm;
  void disasm( std::ostream& sink ) const { sink << "aam" << DisasmI( imm ); }
};

template <> Operation* decode<ADJUST>( CodeBase const& cb )
{
  if (cb.mode64()) return 0;
  
  if (auto _ = match( cb, opcode( "\xd5" ) & Imm<8>() ))
  
    return new AAD( _.opbase(), _.imm<uint8_t>() );

  if (auto _ = match( cb, opcode( "\xd4" ) & Imm<8>() ))

    return new AAM( _.opbase(), _.imm<uint8_t>() );

  if (auto _ = match( cb, opcode( "\x27" ) ))
 
    return new DAA( _.opbase() );

  if (auto _ = match( cb, opcode( "\x2f" ) ))

    return new DAS( _.opbase() );

  if (auto _ = match( cb, opcode( "\x37" ) ))

    return new AAA( _.opbase() );

  if (auto _ = match( cb, opcode( "\x3f" ) ))

    return new AAS( _.opbase() );
  
  return 0;
}

template <unsigned OPSIZE>
struct Bound : public Operation
{
  Bound( OpBase const& opbase, MOp* _rmop, uint8_t _gn ) : Operation( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "bound " << DisasmR<OPSIZE>( gn ) << ',' << DisasmM( rmop ); }
};

template <> Operation* decode<BOUND>( CodeBase const& cb )
{
  if (cb.mode64()) return 0;
  
  if (auto _ = match( cb, opcode( "\x62" ) & RM() ))
  
    {
      if      (cb.opsize() == 16) return new Bound<16>( _.opbase(), _.rmop(), _.greg() );
      else if (cb.opsize() == 16) return new Bound<16>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  return 0;
}

template <unsigned OPSIZE>
struct Adcx : public Operation
{
  Adcx( OpBase const& opbase, MOp* _rmop, uint8_t _gn ) : Operation( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "adcx " << DisasmE<OPSIZE>( rmop ) << ',' << DisasmR<OPSIZE>( gn ); }
};

template <unsigned OPSIZE>
struct Adox : public Operation
{
  Adox( OpBase const& opbase, MOp* _rmop, uint8_t _gn ) : Operation( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "adox " << DisasmE<OPSIZE>( rmop ) << ',' << DisasmR<OPSIZE>( gn ); }
};

template <> Operation* decode<ADF>( CodeBase const& cb )
{
  if (cb.f0()) return 0;
  
  /* ADCX -- Unsigned Integer Addition of Two Operands with Carry Flag */
  if (auto _ = match( cb, RP_() & OP66() & opcode( "\x0f\x38\xf6" ) & RM() ))
  
    {
      if (cb.opsize() == 64) return new Adcx<64>( _.opbase(), _.rmop(), _.greg() );
      else                   return new Adcx<32>( _.opbase(), _.rmop(), _.greg() );
    }
  
  if (auto _ = match( cb, RPF3() & opcode( "\x0f\x38\xf6" ) & RM() ))
  
    {
      if (cb.opsize() == 64) return new Adox<64>( _.opbase(), _.rmop(), _.greg() );
      else                   return new Adox<32>( _.opbase(), _.rmop(), _.greg() );
    }
  
  return 0;
}

