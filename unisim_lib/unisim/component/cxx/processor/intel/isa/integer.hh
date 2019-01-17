// TODO: check for clearing operation (sub and xor with same registers)

template <class ARCH, class GOP, bool GTOE>
struct AddRM : public Operation<ARCH>
{
  AddRM( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn )
    : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {}
  RMOp<ARCH> rmop; uint8_t gn;
  
  void disasm( std::ostream& sink ) const
  {
    if (GTOE) sink << "add " << DisasmG( GOP(), gn ) << ',' << DisasmE( GOP(), rmop );
    else      sink << "add " << DisasmE( GOP(), rmop ) << ',' << DisasmG( GOP(), gn );
  }
  
  typedef typename TypeFor<ARCH,GOP::OPSIZE>::u u_type;
  
  void execute( ARCH& arch ) const
  {
    if (GTOE) {
      u_type res = eval_add( arch, arch.rmread( GOP(), rmop ), arch.regread( GOP(), gn ) );
      arch.rmwrite( GOP(), rmop, res );
    } else {
      u_type res = eval_add( arch, arch.regread( GOP(), gn ), arch.rmread( GOP(), rmop ) );
      arch.regwrite( GOP(), gn, res );
    }
  }
};

template <class ARCH, unsigned OPSIZE>
struct AddRMI : public Operation<ARCH>
{
  typedef typename CTypeFor<OPSIZE>::s imm_type;
  typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
  AddRMI( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, imm_type _imm ) : Operation<ARCH>( opbase ), rmop( _rmop ), imm(_imm) {} RMOp<ARCH> rmop; imm_type imm;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "add", rmop.isreg() ) << DisasmI( imm ) << ',' << DisasmE( UI<OPSIZE>(), rmop ); }

  void execute( ARCH& arch ) const { arch.template rmwrite<OPSIZE>( rmop, eval_add( arch, arch.template rmread<OPSIZE>( rmop ), u_type( imm ) ) ); }
};

template <class ARCH> struct DC<ARCH,ADD> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\000" ) & RM() ))
    {
      if (ic.rex()) return new AddRM<ARCH,GOb,  true>( _.opbase(), _.rmop(), _.greg() );
      else          return new AddRM<ARCH,GObLH,true>( _.opbase(), _.rmop(), _.greg() );
    }

  if (auto _ = match( ic, opcode( "\001" ) & RM() ))
    {
      if      (ic.opsize() == 16) return new AddRM<ARCH,GOw,true>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 32) return new AddRM<ARCH,GOd,true>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 64) return new AddRM<ARCH,GOq,true>( _.opbase(), _.rmop(), _.greg() );
      else throw 0;
    };

  if (auto _ = match( ic, opcode( "\002" ) & RM() ))
    {
      if (ic.rex()) return new AddRM<ARCH,GOb,  false>( _.opbase(), _.rmop(), _.greg() );
      else          return new AddRM<ARCH,GObLH,false>( _.opbase(), _.rmop(), _.greg() );
    }
  
  if (auto _ = match( ic, opcode( "\003" ) & RM() ))
    {
      if      (ic.opsize() == 16) return new AddRM<ARCH,GOw,false>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 32) return new AddRM<ARCH,GOd,false>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 64) return new AddRM<ARCH,GOq,false>( _.opbase(), _.rmop(), _.greg() );
      else throw 0;
    };

  if (auto _ = match( ic, opcode( "\004" ) & Imm<8>() ))
    return new AddRMI<ARCH,8>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int8_t() ) );
  
  if (auto _ = match( ic, OpSize<16>() & opcode( "\005" ) & Imm<16>() ))
    return new AddRMI<ARCH,16>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int16_t() ) );
  
  if (auto _ = match( ic, OpSize<32>() & opcode( "\005" ) & Imm<32>() ))
    return new AddRMI<ARCH,32>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int32_t() ) );
  
  if (auto _ = match( ic, OpSize<64>() & opcode( "\005" ) & Imm<32>() ))
    return new AddRMI<ARCH,64>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int32_t() ) );

  if (auto _ = match( ic, opcode( "\x80" ) /0 & RM() & Imm<8>() ))
    return new AddRMI<ARCH,8>( _.opbase(), _.rmop(), _.i( int8_t() ) );

  if (auto _ = match( ic, OpSize<16>() & opcode( "\x81" ) /0 & RM() & Imm<16>() ))
    return new AddRMI<ARCH,16>( _.opbase(), _.rmop(), _.i( int16_t() ) );

  if (auto _ = match( ic, OpSize<32>() & opcode( "\x81" ) /0 & RM() & Imm<32>() ))
    return new AddRMI<ARCH,32>( _.opbase(), _.rmop(), _.i( int32_t() ) );

  if (auto _ = match( ic, OpSize<64>() & opcode( "\x81" ) /0 & RM() & Imm<32>() ))
    return new AddRMI<ARCH,64>( _.opbase(), _.rmop(), _.i( int32_t() ) );

  if (auto _ = match( ic, opcode( "\x83" ) /0 & RM() & Imm<8>() ))
    {
      if      (ic.opsize() == 16) return new AddRMI<ARCH,16>( _.opbase(), _.rmop(), _.i( int16_t() ) );
      else if (ic.opsize() == 32) return new AddRMI<ARCH,32>( _.opbase(), _.rmop(), _.i( int32_t() ) );
      else if (ic.opsize() == 64) return new AddRMI<ARCH,64>( _.opbase(), _.rmop(), _.i( int32_t() ) );
      else throw 0;
    }
  return 0;
}};

template <class ARCH, unsigned OPSIZE, bool GTOE>
struct OrRM : public Operation<ARCH>
{
  OrRM( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  
  void disasm( std::ostream& sink ) const {
    if (GTOE) sink << "or " << DisasmG<OPSIZE>( gn ) << ',' << DisasmE( UI<OPSIZE>(), rmop );
    else      sink << "or " << DisasmE( UI<OPSIZE>(), rmop ) << ',' << DisasmG<OPSIZE>( gn );
  }
  
  typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
  
  void execute( ARCH& arch ) const
  {
    if (GTOE) {
      u_type res = eval_or( arch, arch.template rmread<OPSIZE>( rmop ), arch.template regread<OPSIZE>( gn ) );
      arch.template rmwrite<OPSIZE>( rmop, res );
    } else {
      u_type res = eval_or( arch, arch.template regread<OPSIZE>( gn ), arch.template rmread<OPSIZE>( rmop ) );
      arch.template regwrite<OPSIZE>( gn, res );
    }
  }
};

template <class ARCH, unsigned OPSIZE>
struct OrRMI : public Operation<ARCH>
{
  typedef typename CTypeFor<OPSIZE>::s imm_type;
  typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
  OrRMI( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, imm_type _imm ) : Operation<ARCH>( opbase ), rmop( _rmop ), imm(_imm) {} RMOp<ARCH> rmop; imm_type imm;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "or", rmop.isreg() ) << DisasmI( imm ) << ',' << DisasmE( UI<OPSIZE>(), rmop ); }

  void execute( ARCH& arch ) const { arch.template rmwrite<OPSIZE>( rmop, eval_or( arch, arch.template rmread<OPSIZE>( rmop ), u_type( imm ) ) ); }
};

template <class ARCH> struct DC<ARCH,OR> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\010" ) & RM() ))
    return new OrRM<ARCH,8,true>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, opcode( "\011" ) & RM() ))
    {
      if      (ic.opsize() == 16) return new OrRM<ARCH,16,true>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 32) return new OrRM<ARCH,32,true>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 64) return new OrRM<ARCH,64,true>( _.opbase(), _.rmop(), _.greg() );
      else throw 0;
    };

  if (auto _ = match( ic, opcode( "\012" ) & RM() ))
    return new OrRM<ARCH,8,false>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, opcode( "\013" ) & RM() ))
    {
      if      (ic.opsize() == 16) return new OrRM<ARCH,16,false>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 32) return new OrRM<ARCH,32,false>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 64) return new OrRM<ARCH,64,false>( _.opbase(), _.rmop(), _.greg() );
      else throw 0;
    };

  if (auto _ = match( ic, opcode( "\014" ) & Imm<8>() ))
    return new OrRMI<ARCH,8>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int8_t() ) );
  
  if (auto _ = match( ic, OpSize<16>() & opcode( "\015" ) & Imm<16>() ))
    return new OrRMI<ARCH,16>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int16_t() ) );
  
  if (auto _ = match( ic, OpSize<32>() & opcode( "\015" ) & Imm<32>() ))
    return new OrRMI<ARCH,32>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int32_t() ) );
  
  if (auto _ = match( ic, OpSize<64>() & opcode( "\015" ) & Imm<32>() ))
    return new OrRMI<ARCH,64>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int32_t() ) );

  if (auto _ = match( ic, opcode( "\x80" ) /1 & RM() & Imm<8>() ))
    return new OrRMI<ARCH,8>( _.opbase(), _.rmop(), _.i( int8_t() ) );

  if (auto _ = match( ic, OpSize<16>() & opcode( "\x81" ) /1 & RM() & Imm<16>() ))
    return new OrRMI<ARCH,16>( _.opbase(), _.rmop(), _.i( int16_t() ) );

  if (auto _ = match( ic, OpSize<32>() & opcode( "\x81" ) /1 & RM() & Imm<32>() ))
    return new OrRMI<ARCH,32>( _.opbase(), _.rmop(), _.i( int32_t() ) );

  if (auto _ = match( ic, OpSize<64>() & opcode( "\x81" ) /1 & RM() & Imm<32>() ))
    return new OrRMI<ARCH,64>( _.opbase(), _.rmop(), _.i( int32_t() ) );

  if (auto _ = match( ic, opcode( "\x83" ) /1 & RM() & Imm<8>() ))
    {
      if      (ic.opsize() == 16) return new OrRMI<ARCH,16>( _.opbase(), _.rmop(), _.i( int16_t() ) );
      else if (ic.opsize() == 32) return new OrRMI<ARCH,32>( _.opbase(), _.rmop(), _.i( int32_t() ) );
      else if (ic.opsize() == 64) return new OrRMI<ARCH,64>( _.opbase(), _.rmop(), _.i( int32_t() ) );
      else throw 0;
    }
  return 0;
}};

template <class ARCH, unsigned OPSIZE, bool GTOE>
struct AdcRM : public Operation<ARCH>
{
  AdcRM( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  
  void disasm( std::ostream& sink ) const {
    if (GTOE) sink << "adc " << DisasmG<OPSIZE>( gn ) << ',' << DisasmE( UI<OPSIZE>(), rmop );
    else      sink << "adc " << DisasmE( UI<OPSIZE>(), rmop ) << ',' << DisasmG<OPSIZE>( gn );
  }
  
  typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
  
  void execute( ARCH& arch ) const
  {
    if (GTOE) {
      u_type res = eval_adc( arch, arch.template rmread<OPSIZE>( rmop ), arch.template regread<OPSIZE>( gn ) );
      arch.template rmwrite<OPSIZE>( rmop, res );
    } else {
      u_type res = eval_adc( arch, arch.template regread<OPSIZE>( gn ), arch.template rmread<OPSIZE>( rmop ) );
      arch.template regwrite<OPSIZE>( gn, res );
    }
  }
};

template <class ARCH, unsigned OPSIZE>
struct AdcRMI : public Operation<ARCH>
{
  typedef typename CTypeFor<OPSIZE>::s imm_type;
  typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
  AdcRMI( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, imm_type _imm ) : Operation<ARCH>( opbase ), rmop( _rmop ), imm(_imm) {} RMOp<ARCH> rmop; imm_type imm;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "adc", rmop.isreg() ) << DisasmI( imm ) << ',' << DisasmE( UI<OPSIZE>(), rmop ); }

  void execute( ARCH& arch ) const { arch.template rmwrite<OPSIZE>( rmop, eval_adc( arch, arch.template rmread<OPSIZE>( rmop ), u_type(imm) ) ); }
};

template <class ARCH> struct DC<ARCH,ADC> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\020" ) & RM() ))
    return new AdcRM<ARCH,8,true>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, opcode( "\021" ) & RM() ))
    {
      if      (ic.opsize() == 16) return new AdcRM<ARCH,16,true>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 32) return new AdcRM<ARCH,32,true>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 64) return new AdcRM<ARCH,64,true>( _.opbase(), _.rmop(), _.greg() );
      else throw 0;
    };

  if (auto _ = match( ic, opcode( "\022" ) & RM() ))
    return new AdcRM<ARCH,8,false>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, opcode( "\023" ) & RM() ))
    {
      if      (ic.opsize() == 16) return new AdcRM<ARCH,16,false>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 32) return new AdcRM<ARCH,32,false>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 64) return new AdcRM<ARCH,64,false>( _.opbase(), _.rmop(), _.greg() );
      else throw 0;
    };

  if (auto _ = match( ic, opcode( "\024" ) & Imm<8>() ))
    return new AdcRMI<ARCH,8>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int8_t() ) );
  
  if (auto _ = match( ic, OpSize<16>() & opcode( "\025" ) & Imm<16>() ))
    return new AdcRMI<ARCH,16>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int16_t() ) );
  
  if (auto _ = match( ic, OpSize<32>() & opcode( "\025" ) & Imm<32>() ))
    return new AdcRMI<ARCH,32>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int32_t() ) );
  
  if (auto _ = match( ic, OpSize<64>() & opcode( "\025" ) & Imm<32>() ))
    return new AdcRMI<ARCH,64>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int32_t() ) );

  if (auto _ = match( ic, opcode( "\x80" ) /2 & RM() & Imm<8>() ))
    return new AdcRMI<ARCH,8>( _.opbase(), _.rmop(), _.i( int8_t() ) );

  if (auto _ = match( ic, OpSize<16>() & opcode( "\x81" ) /2 & RM() & Imm<16>() ))
    return new AdcRMI<ARCH,16>( _.opbase(), _.rmop(), _.i( int16_t() ) );

  if (auto _ = match( ic, OpSize<32>() & opcode( "\x81" ) /2 & RM() & Imm<32>() ))
    return new AdcRMI<ARCH,32>( _.opbase(), _.rmop(), _.i( int32_t() ) );

  if (auto _ = match( ic, OpSize<64>() & opcode( "\x81" ) /2 & RM() & Imm<32>() ))
    return new AdcRMI<ARCH,64>( _.opbase(), _.rmop(), _.i( int32_t() ) );

  if (auto _ = match( ic, opcode( "\x83" ) /2 & RM() & Imm<8>() ))
    {
      if      (ic.opsize() == 16) return new AdcRMI<ARCH,16>( _.opbase(), _.rmop(), _.i( int16_t() ) );
      else if (ic.opsize() == 32) return new AdcRMI<ARCH,32>( _.opbase(), _.rmop(), _.i( int32_t() ) );
      else if (ic.opsize() == 64) return new AdcRMI<ARCH,64>( _.opbase(), _.rmop(), _.i( int32_t() ) );
      else throw 0;
    }
  return 0;
}};

template <class ARCH, unsigned OPSIZE, bool GTOE>
struct SbbRM : public Operation<ARCH>
{
  SbbRM( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  
  void disasm( std::ostream& sink ) const {
    if (GTOE) sink << "sbb " << DisasmG<OPSIZE>( gn ) << ',' << DisasmE( UI<OPSIZE>(), rmop );
    else      sink << "sbb " << DisasmE( UI<OPSIZE>(), rmop ) << ',' << DisasmG<OPSIZE>( gn );
  }
  
  typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
  
  void execute( ARCH& arch ) const
  {
    if (GTOE) {
      u_type res = eval_sbb( arch, arch.template rmread<OPSIZE>( rmop ), arch.template regread<OPSIZE>( gn ) );
      arch.template rmwrite<OPSIZE>( rmop, res );
    } else {
      u_type res = eval_sbb( arch, arch.template regread<OPSIZE>( gn ), arch.template rmread<OPSIZE>( rmop ) );
      arch.template regwrite<OPSIZE>( gn, res );
    }
  }
};

template <class ARCH, unsigned OPSIZE>
struct SbbRMI : public Operation<ARCH>
{
  typedef typename CTypeFor<OPSIZE>::s imm_type;
  typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
  SbbRMI( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, imm_type _imm ) : Operation<ARCH>( opbase ), rmop( _rmop ), imm(_imm) {} RMOp<ARCH> rmop; imm_type imm;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "sbb", rmop.isreg() ) << DisasmI( imm ) << ',' << DisasmE( UI<OPSIZE>(), rmop ); }

  void execute( ARCH& arch ) const { arch.template rmwrite<OPSIZE>( rmop, eval_sbb( arch, arch.template rmread<OPSIZE>( rmop ), u_type(imm) ) ); }
};

template <class ARCH> struct DC<ARCH,SBB> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\030" ) & RM() ))
    return new SbbRM<ARCH,8,true>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, opcode( "\031" ) & RM() ))
    {
      if      (ic.opsize() == 16) return new SbbRM<ARCH,16,true>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 32) return new SbbRM<ARCH,32,true>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 64) return new SbbRM<ARCH,64,true>( _.opbase(), _.rmop(), _.greg() );
      else throw 0;
    };

  if (auto _ = match( ic, opcode( "\032" ) & RM() ))
    return new SbbRM<ARCH,8,false>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, opcode( "\033" ) & RM() ))
    {
      if      (ic.opsize() == 16) return new SbbRM<ARCH,16,false>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 32) return new SbbRM<ARCH,32,false>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 64) return new SbbRM<ARCH,64,false>( _.opbase(), _.rmop(), _.greg() );
      else throw 0;
    };

  if (auto _ = match( ic, opcode( "\034" ) & Imm<8>() ))
    return new SbbRMI<ARCH,8>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int8_t() ) );
  
  if (auto _ = match( ic, OpSize<16>() & opcode( "\035" ) & Imm<16>() ))
    return new SbbRMI<ARCH,16>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int16_t() ) );
  
  if (auto _ = match( ic, OpSize<32>() & opcode( "\035" ) & Imm<32>() ))
    return new SbbRMI<ARCH,32>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int32_t() ) );
  
  if (auto _ = match( ic, OpSize<64>() & opcode( "\035" ) & Imm<32>() ))
    return new SbbRMI<ARCH,64>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int32_t() ) );

  if (auto _ = match( ic, opcode( "\x80" ) /3 & RM() & Imm<8>() ))
    return new SbbRMI<ARCH,8>( _.opbase(), _.rmop(), _.i( int8_t() ) );

  if (auto _ = match( ic, OpSize<16>() & opcode( "\x81" ) /3 & RM() & Imm<16>() ))
    return new SbbRMI<ARCH,16>( _.opbase(), _.rmop(), _.i( int16_t() ) );

  if (auto _ = match( ic, OpSize<32>() & opcode( "\x81" ) /3 & RM() & Imm<32>() ))
    return new SbbRMI<ARCH,32>( _.opbase(), _.rmop(), _.i( int32_t() ) );

  if (auto _ = match( ic, OpSize<64>() & opcode( "\x81" ) /3 & RM() & Imm<32>() ))
    return new SbbRMI<ARCH,64>( _.opbase(), _.rmop(), _.i( int32_t() ) );

  if (auto _ = match( ic, opcode( "\x83" ) /3 & RM() & Imm<8>() ))
    {
      if      (ic.opsize() == 16) return new SbbRMI<ARCH,16>( _.opbase(), _.rmop(), _.i( int16_t() ) );
      else if (ic.opsize() == 32) return new SbbRMI<ARCH,32>( _.opbase(), _.rmop(), _.i( int32_t() ) );
      else if (ic.opsize() == 64) return new SbbRMI<ARCH,64>( _.opbase(), _.rmop(), _.i( int32_t() ) );
      else throw 0;
    }
  return 0;
}};

template <class ARCH, unsigned OPSIZE, bool GTOE>
struct AndRM : public Operation<ARCH>
{
  AndRM( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  
  void disasm( std::ostream& sink ) const {
    if (GTOE) sink << "and " << DisasmG<OPSIZE>( gn ) << ',' << DisasmE( UI<OPSIZE>(), rmop );
    else      sink << "and " << DisasmE( UI<OPSIZE>(), rmop ) << ',' << DisasmG<OPSIZE>( gn );
  }
  
  typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
  
  void execute( ARCH& arch ) const
  {
    if (GTOE) {
      u_type res = eval_and( arch, arch.template rmread<OPSIZE>( rmop ), arch.template regread<OPSIZE>( gn ) );
      arch.template rmwrite<OPSIZE>( rmop, res );
    } else {
      u_type res = eval_and( arch, arch.template regread<OPSIZE>( gn ), arch.template rmread<OPSIZE>( rmop ) );
      arch.template regwrite<OPSIZE>( gn, res );
    }
  }
};

template <class ARCH, unsigned OPSIZE>
struct AndRMI : public Operation<ARCH>
{
  typedef typename CTypeFor<OPSIZE>::s imm_type;
  typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
  AndRMI( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, imm_type _imm ) : Operation<ARCH>( opbase ), rmop( _rmop ), imm(_imm) {} RMOp<ARCH> rmop; imm_type imm;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "and", rmop.isreg() ) << DisasmI( imm ) << ',' << DisasmE( UI<OPSIZE>(), rmop ); }
  
  void execute( ARCH& arch ) const { arch.template rmwrite<OPSIZE>( rmop, eval_and( arch, arch.template rmread<OPSIZE>( rmop ), u_type(imm) ) ); }
};

template <class ARCH> struct DC<ARCH,AND> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\040" ) & RM() ))
    return new AndRM<ARCH,8,true>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, opcode( "\041" ) & RM() ))
    {
      if      (ic.opsize() == 16) return new AndRM<ARCH,16,true>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 32) return new AndRM<ARCH,32,true>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 64) return new AndRM<ARCH,64,true>( _.opbase(), _.rmop(), _.greg() );
      else throw 0;
    };

  if (auto _ = match( ic, opcode( "\042" ) & RM() ))
    return new AndRM<ARCH,8,false>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, opcode( "\043" ) & RM() ))
    {
      if      (ic.opsize() == 16) return new AndRM<ARCH,16,false>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 32) return new AndRM<ARCH,32,false>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 64) return new AndRM<ARCH,64,false>( _.opbase(), _.rmop(), _.greg() );
      else throw 0;
    };

  if (auto _ = match( ic, opcode( "\044" ) & Imm<8>() ))
    return new AndRMI<ARCH,8>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int8_t() ) );
  
  if (auto _ = match( ic, OpSize<16>() & opcode( "\045" ) & Imm<16>() ))
    return new AndRMI<ARCH,16>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int16_t() ) );
  
  if (auto _ = match( ic, OpSize<32>() & opcode( "\045" ) & Imm<32>() ))
    return new AndRMI<ARCH,32>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int32_t() ) );
  
  if (auto _ = match( ic, OpSize<64>() & opcode( "\045" ) & Imm<32>() ))
    return new AndRMI<ARCH,64>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int32_t() ) );

  if (auto _ = match( ic, opcode( "\x80" ) /4 & RM() & Imm<8>() ))
    return new AndRMI<ARCH,8>( _.opbase(), _.rmop(), _.i( int8_t() ) );

  if (auto _ = match( ic, OpSize<16>() & opcode( "\x81" ) /4 & RM() & Imm<16>() ))
    return new AndRMI<ARCH,16>( _.opbase(), _.rmop(), _.i( int16_t() ) );

  if (auto _ = match( ic, OpSize<32>() & opcode( "\x81" ) /4 & RM() & Imm<32>() ))
    return new AndRMI<ARCH,32>( _.opbase(), _.rmop(), _.i( int32_t() ) );

  if (auto _ = match( ic, OpSize<64>() & opcode( "\x81" ) /4 & RM() & Imm<32>() ))
    return new AndRMI<ARCH,64>( _.opbase(), _.rmop(), _.i( int32_t() ) );

  if (auto _ = match( ic, opcode( "\x83" ) /4 & RM() & Imm<8>() ))
    {
      if      (ic.opsize() == 16) return new AndRMI<ARCH,16>( _.opbase(), _.rmop(), _.i( int16_t() ) );
      else if (ic.opsize() == 32) return new AndRMI<ARCH,32>( _.opbase(), _.rmop(), _.i( int32_t() ) );
      else if (ic.opsize() == 64) return new AndRMI<ARCH,64>( _.opbase(), _.rmop(), _.i( int32_t() ) );
      else throw 0;
    }
  return 0;
}};

template <class ARCH, unsigned OPSIZE, bool GTOE>
struct SubRM : public Operation<ARCH>
{
  SubRM( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  
  void disasm( std::ostream& sink ) const {
    if (GTOE) sink << "sub " << DisasmG<OPSIZE>( gn ) << ',' << DisasmE( UI<OPSIZE>(), rmop );
    else      sink << "sub " << DisasmE( UI<OPSIZE>(), rmop ) << ',' << DisasmG<OPSIZE>( gn );
  }
  
  typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
  
  void execute( ARCH& arch ) const
  {
    if (GTOE) {
      u_type res = eval_sub( arch, arch.template rmread<OPSIZE>( rmop ), arch.template regread<OPSIZE>( gn ) );
      arch.template rmwrite<OPSIZE>( rmop, res );
    } else {
      u_type res = eval_sub( arch, arch.template regread<OPSIZE>( gn ), arch.template rmread<OPSIZE>( rmop ) );
      arch.template regwrite<OPSIZE>( gn, res );
    }
  }
};

template <class ARCH, unsigned OPSIZE>
struct SubRMI : public Operation<ARCH>
{
  typedef typename CTypeFor<OPSIZE>::s imm_type;
  typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
  SubRMI( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, imm_type _imm ) : Operation<ARCH>( opbase ), rmop( _rmop ), imm(_imm) {} RMOp<ARCH> rmop; imm_type imm;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "sub", rmop.isreg() ) << DisasmI( imm ) << ',' << DisasmE( UI<OPSIZE>(), rmop ); }
  
  void execute( ARCH& arch ) const { arch.template rmwrite<OPSIZE>( rmop, eval_sub( arch, arch.template rmread<OPSIZE>( rmop ), u_type(imm) ) ); }
};

template <class ARCH> struct DC<ARCH,SUB> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\050" ) & RM() ))
    return new SubRM<ARCH,8,true>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, opcode( "\051" ) & RM() ))
    {
      if      (ic.opsize() == 16) return new SubRM<ARCH,16,true>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 32) return new SubRM<ARCH,32,true>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 64) return new SubRM<ARCH,64,true>( _.opbase(), _.rmop(), _.greg() );
      else throw 0;
    };

  if (auto _ = match( ic, opcode( "\052" ) & RM() ))
    return new SubRM<ARCH,8,false>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, opcode( "\053" ) & RM() ))
    {
      if      (ic.opsize() == 16) return new SubRM<ARCH,16,false>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 32) return new SubRM<ARCH,32,false>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 64) return new SubRM<ARCH,64,false>( _.opbase(), _.rmop(), _.greg() );
      else throw 0;
    };

  if (auto _ = match( ic, opcode( "\054" ) & Imm<8>() ))
    return new SubRMI<ARCH,8>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int8_t() ) );
  
  if (auto _ = match( ic, OpSize<16>() & opcode( "\055" ) & Imm<16>() ))
    return new SubRMI<ARCH,16>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int16_t() ) );
  
  if (auto _ = match( ic, OpSize<32>() & opcode( "\055" ) & Imm<32>() ))
    return new SubRMI<ARCH,32>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int32_t() ) );
  
  if (auto _ = match( ic, OpSize<64>() & opcode( "\055" ) & Imm<32>() ))
    return new SubRMI<ARCH,64>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int32_t() ) );

  if (auto _ = match( ic, opcode( "\x80" ) /5 & RM() & Imm<8>() ))
    return new SubRMI<ARCH,8>( _.opbase(), _.rmop(), _.i( int8_t() ) );

  if (auto _ = match( ic, OpSize<16>() & opcode( "\x81" ) /5 & RM() & Imm<16>() ))
    return new SubRMI<ARCH,16>( _.opbase(), _.rmop(), _.i( int16_t() ) );

  if (auto _ = match( ic, OpSize<32>() & opcode( "\x81" ) /5 & RM() & Imm<32>() ))
    return new SubRMI<ARCH,32>( _.opbase(), _.rmop(), _.i( int32_t() ) );

  if (auto _ = match( ic, OpSize<64>() & opcode( "\x81" ) /5 & RM() & Imm<32>() ))
    return new SubRMI<ARCH,64>( _.opbase(), _.rmop(), _.i( int32_t() ) );

  if (auto _ = match( ic, opcode( "\x83" ) /5 & RM() & Imm<8>() ))
    {
      if      (ic.opsize() == 16) return new SubRMI<ARCH,16>( _.opbase(), _.rmop(), _.i( int16_t() ) );
      else if (ic.opsize() == 32) return new SubRMI<ARCH,32>( _.opbase(), _.rmop(), _.i( int32_t() ) );
      else if (ic.opsize() == 64) return new SubRMI<ARCH,64>( _.opbase(), _.rmop(), _.i( int32_t() ) );
      else throw 0;
    }
  return 0;
}};

template <class ARCH, unsigned OPSIZE, bool GTOE>
struct XorRM : public Operation<ARCH>
{
  XorRM( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  
  void disasm( std::ostream& sink ) const {
    if (GTOE) sink << "xor " << DisasmG<OPSIZE>( gn ) << ',' << DisasmE( UI<OPSIZE>(), rmop );
    else      sink << "xor " << DisasmE( UI<OPSIZE>(), rmop ) << ',' << DisasmG<OPSIZE>( gn );
  }
  
  typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
  
  void execute( ARCH& arch ) const
  {
    if (GTOE) {
      u_type res = eval_xor( arch, arch.template rmread<OPSIZE>( rmop ), arch.template regread<OPSIZE>( gn ) );
      arch.template rmwrite<OPSIZE>( rmop, res );
    } else {
      u_type res = eval_xor( arch, arch.template regread<OPSIZE>( gn ), arch.template rmread<OPSIZE>( rmop ) );
      arch.template regwrite<OPSIZE>( gn, res );
    }
  }
};

template <class ARCH, unsigned OPSIZE>
struct XorRMI : public Operation<ARCH>
{
  typedef typename CTypeFor<OPSIZE>::s imm_type;
  typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
  XorRMI( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, imm_type _imm ) : Operation<ARCH>( opbase ), rmop( _rmop ), imm(_imm) {} RMOp<ARCH> rmop; imm_type imm;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "xor", rmop.isreg() ) << DisasmI( imm ) << ',' << DisasmE( UI<OPSIZE>(), rmop ); }
  
  void execute( ARCH& arch ) const { arch.template rmwrite<OPSIZE>( rmop, eval_xor( arch, arch.template rmread<OPSIZE>( rmop ), u_type(imm) ) ); }
};

template <class ARCH> struct DC<ARCH,XOR> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\060" ) & RM() ))
    return new XorRM<ARCH,8,true>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, opcode( "\061" ) & RM() ))
    {
      if      (ic.opsize() == 16) return new XorRM<ARCH,16,true>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 32) return new XorRM<ARCH,32,true>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 64) return new XorRM<ARCH,64,true>( _.opbase(), _.rmop(), _.greg() );
      else throw 0;
    };

  if (auto _ = match( ic, opcode( "\062" ) & RM() ))
    return new XorRM<ARCH,8,false>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, opcode( "\063" ) & RM() ))
    {
      if      (ic.opsize() == 16) return new XorRM<ARCH,16,false>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 32) return new XorRM<ARCH,32,false>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 64) return new XorRM<ARCH,64,false>( _.opbase(), _.rmop(), _.greg() );
      else throw 0;
    };

  if (auto _ = match( ic, opcode( "\064" ) & Imm<8>() ))
    return new XorRMI<ARCH,8>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int8_t() ) );
  
  if (auto _ = match( ic, OpSize<16>() & opcode( "\065" ) & Imm<16>() ))
    return new XorRMI<ARCH,16>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int16_t() ) );
  
  if (auto _ = match( ic, OpSize<32>() & opcode( "\065" ) & Imm<32>() ))
    return new XorRMI<ARCH,32>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int32_t() ) );
  
  if (auto _ = match( ic, OpSize<64>() & opcode( "\065" ) & Imm<32>() ))
    return new XorRMI<ARCH,64>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int32_t() ) );

  if (auto _ = match( ic, opcode( "\x80" ) /6 & RM() & Imm<8>() ))
    return new XorRMI<ARCH,8>( _.opbase(), _.rmop(), _.i( int8_t() ) );

  if (auto _ = match( ic, OpSize<16>() & opcode( "\x81" ) /6 & RM() & Imm<16>() ))
    return new XorRMI<ARCH,16>( _.opbase(), _.rmop(), _.i( int16_t() ) );

  if (auto _ = match( ic, OpSize<32>() & opcode( "\x81" ) /6 & RM() & Imm<32>() ))
    return new XorRMI<ARCH,32>( _.opbase(), _.rmop(), _.i( int32_t() ) );

  if (auto _ = match( ic, OpSize<64>() & opcode( "\x81" ) /6 & RM() & Imm<32>() ))
    return new XorRMI<ARCH,64>( _.opbase(), _.rmop(), _.i( int32_t() ) );

  if (auto _ = match( ic, opcode( "\x83" ) /6 & RM() & Imm<8>() ))
    {
      if      (ic.opsize() == 16) return new XorRMI<ARCH,16>( _.opbase(), _.rmop(), _.i( int16_t() ) );
      else if (ic.opsize() == 32) return new XorRMI<ARCH,32>( _.opbase(), _.rmop(), _.i( int32_t() ) );
      else if (ic.opsize() == 64) return new XorRMI<ARCH,64>( _.opbase(), _.rmop(), _.i( int32_t() ) );
      else throw 0;
    }
  return 0;
}};

template <class ARCH, unsigned OPSIZE, bool GTOE>
struct CmpRM : public Operation<ARCH>
{
  CmpRM( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  
  void disasm( std::ostream& sink ) const {
    if (GTOE) sink << "cmp " << DisasmG<OPSIZE>( gn ) << ',' << DisasmE( UI<OPSIZE>(), rmop );
    else      sink << "cmp " << DisasmE( UI<OPSIZE>(), rmop ) << ',' << DisasmG<OPSIZE>( gn );
  }
  
  typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
  
  void execute( ARCH& arch ) const
  {
    if (GTOE) eval_sub( arch, arch.template rmread<OPSIZE>( rmop ), arch.template regread<OPSIZE>( gn ) );
    else      eval_sub( arch, arch.template regread<OPSIZE>( gn ), arch.template rmread<OPSIZE>( rmop ) );
  }
};

template <class ARCH, unsigned OPSIZE>
struct CmpRMI : public Operation<ARCH>
{
  typedef typename CTypeFor<OPSIZE>::s imm_type;
  typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
  CmpRMI( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, imm_type _imm ) : Operation<ARCH>( opbase ), rmop( _rmop ), imm(_imm) {} RMOp<ARCH> rmop; imm_type imm;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "cmp", rmop.isreg() ) << DisasmI( imm ) << ',' << DisasmE( UI<OPSIZE>(), rmop ); }

  void execute( ARCH& arch ) const { eval_sub( arch, arch.template rmread<OPSIZE>( rmop ), u_type(imm) ); }
};

template <class ARCH> struct DC<ARCH,CMP> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\070" ) & RM() ))
    return new CmpRM<ARCH,8,true>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, opcode( "\071" ) & RM() ))
    {
      if      (ic.opsize() == 16) return new CmpRM<ARCH,16,true>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 32) return new CmpRM<ARCH,32,true>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 64) return new CmpRM<ARCH,64,true>( _.opbase(), _.rmop(), _.greg() );
      else throw 0;
    };

  if (auto _ = match( ic, opcode( "\072" ) & RM() ))
    return new CmpRM<ARCH,8,false>( _.opbase(), _.rmop(), _.greg() );

  if (auto _ = match( ic, opcode( "\073" ) & RM() ))
    {
      if      (ic.opsize() == 16) return new CmpRM<ARCH,16,false>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 32) return new CmpRM<ARCH,32,false>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 64) return new CmpRM<ARCH,64,false>( _.opbase(), _.rmop(), _.greg() );
      else throw 0;
    };

  if (auto _ = match( ic, opcode( "\074" ) & Imm<8>() ))
    return new CmpRMI<ARCH,8>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int8_t() ) );
  
  if (auto _ = match( ic, OpSize<16>() & opcode( "\075" ) & Imm<16>() ))
    return new CmpRMI<ARCH,16>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int16_t() ) );
  
  if (auto _ = match( ic, OpSize<32>() & opcode( "\075" ) & Imm<32>() ))
    return new CmpRMI<ARCH,32>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int32_t() ) );
  
  if (auto _ = match( ic, OpSize<64>() & opcode( "\075" ) & Imm<32>() ))
    return new CmpRMI<ARCH,64>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int32_t() ) );

  if (auto _ = match( ic, opcode( "\x80" ) /7 & RM() & Imm<8>() ))
    return new CmpRMI<ARCH,8>( _.opbase(), _.rmop(), _.i( int8_t() ) );

  if (auto _ = match( ic, OpSize<16>() & opcode( "\x81" ) /7 & RM() & Imm<16>() ))
    return new CmpRMI<ARCH,16>( _.opbase(), _.rmop(), _.i( int16_t() ) );

  if (auto _ = match( ic, OpSize<32>() & opcode( "\x81" ) /7 & RM() & Imm<32>() ))
    return new CmpRMI<ARCH,32>( _.opbase(), _.rmop(), _.i( int32_t() ) );

  if (auto _ = match( ic, OpSize<64>() & opcode( "\x81" ) /7 & RM() & Imm<32>() ))
    return new CmpRMI<ARCH,64>( _.opbase(), _.rmop(), _.i( int32_t() ) );

  if (auto _ = match( ic, opcode( "\x83" ) /7 & RM() & Imm<8>() ))
    {
      if      (ic.opsize() == 16) return new CmpRMI<ARCH,16>( _.opbase(), _.rmop(), _.i( int16_t() ) );
      else if (ic.opsize() == 32) return new CmpRMI<ARCH,32>( _.opbase(), _.rmop(), _.i( int32_t() ) );
      else if (ic.opsize() == 64) return new CmpRMI<ARCH,64>( _.opbase(), _.rmop(), _.i( int32_t() ) );
      else throw 0;
    }
  return 0;
}};

/* ROL */

template <class ARCH, unsigned OPSIZE>
struct RolRMI : public Operation<ARCH>
{
  typedef typename ARCH::u8_t u8_t;
  RolRMI( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _imm ) : Operation<ARCH>( opbase ), rmop( _rmop ), imm(_imm) {} RMOp<ARCH> rmop; uint8_t imm;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "rol", rmop.isreg() ) << DisasmI( imm ) << ',' << DisasmE( UI<OPSIZE>(), rmop ); }
  
  void execute( ARCH& arch ) const { arch.template rmwrite<OPSIZE>( rmop, eval_rol( arch, arch.template rmread<OPSIZE>( rmop ), u8_t( imm ) ) ); }
};

template <class ARCH, unsigned OPSIZE>
struct RolRMCL : public Operation<ARCH>
{
  RolRMCL( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop ) : Operation<ARCH>( opbase ), rmop( _rmop ) {} RMOp<ARCH> rmop;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "rol", rmop.isreg() ) << "%cl," << DisasmE( UI<OPSIZE>(), rmop ); }
  
  void execute( ARCH& arch ) const { arch.template rmwrite<OPSIZE>( rmop, eval_rol( arch, arch.template rmread<OPSIZE>( rmop ), arch.regread8( 1 ) ) ); }
};

template <class ARCH> struct DC<ARCH,ROL> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\xc0" ) /0 & RM() & Imm<8>() ))
  
    return new RolRMI<ARCH,8>( _.opbase(), _.rmop(), _.i( int8_t() ) );
  
  if (auto _ = match( ic, opcode( "\xc1" ) /0 & RM() & Imm<8>() ))
  
    {
      if      (ic.opsize() == 16) return new RolRMI<ARCH,16>( _.opbase(), _.rmop(), _.i( int8_t() ) );
      else if (ic.opsize() == 32) return new RolRMI<ARCH,32>( _.opbase(), _.rmop(), _.i( int8_t() ) );
      else if (ic.opsize() == 64) return new RolRMI<ARCH,64>( _.opbase(), _.rmop(), _.i( int8_t() ) );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\xd0" ) /0 & RM() ))
  
    return new RolRMI<ARCH,8>( _.opbase(), _.rmop(), 1 );
  
  if (auto _ = match( ic, opcode( "\xd1" ) /0 & RM() ))
  
    {
      if      (ic.opsize() == 16) return new RolRMI<ARCH,16>( _.opbase(), _.rmop(), 1 );
      else if (ic.opsize() == 32) return new RolRMI<ARCH,32>( _.opbase(), _.rmop(), 1 );
      else if (ic.opsize() == 64) return new RolRMI<ARCH,64>( _.opbase(), _.rmop(), 1 );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\xd2" ) /0 & RM() ))
  
    return new RolRMCL<ARCH,8>( _.opbase(), _.rmop() );
  
  if (auto _ = match( ic, opcode( "\xd3" ) /0 & RM() ))
  
    {
      if      (ic.opsize() == 16) return new RolRMCL<ARCH,16>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 32) return new RolRMCL<ARCH,32>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 64) return new RolRMCL<ARCH,64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  return 0;
}};

/* ROR */

template <class ARCH, unsigned OPSIZE>
struct RorRMI : public Operation<ARCH>
{
  typedef typename ARCH::u8_t u8_t;
  RorRMI( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _imm ) : Operation<ARCH>( opbase ), rmop( _rmop ), imm(_imm) {} RMOp<ARCH> rmop; uint8_t imm;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "ror", rmop.isreg() ) << DisasmI( imm ) << ',' << DisasmE( UI<OPSIZE>(), rmop ); }
  
  void execute( ARCH& arch ) const { arch.template rmwrite<OPSIZE>( rmop, eval_ror( arch, arch.template rmread<OPSIZE>( rmop ), u8_t( imm ) ) ); }
};

template <class ARCH, unsigned OPSIZE>
struct RorRMCL : public Operation<ARCH>
{
  RorRMCL( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop ) : Operation<ARCH>( opbase ), rmop( _rmop ) {} RMOp<ARCH> rmop;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "ror", rmop.isreg() ) << "%cl," << DisasmE( UI<OPSIZE>(), rmop ); }
  
  void execute( ARCH& arch ) const { arch.template rmwrite<OPSIZE>( rmop, eval_ror( arch, arch.template rmread<OPSIZE>( rmop ), arch.regread8( 1 ) ) ); }
};

template <class ARCH> struct DC<ARCH,ROR> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\xc0" ) /1 & RM() & Imm<8>() ))
  
    return new RorRMI<ARCH,8>( _.opbase(), _.rmop(), _.i( int8_t() ) );
  
  if (auto _ = match( ic, opcode( "\xc1" ) /1 & RM() & Imm<8>() ))
  
    {
      if      (ic.opsize() == 16) return new RorRMI<ARCH,16>( _.opbase(), _.rmop(), _.i( int8_t() ) );
      else if (ic.opsize() == 32) return new RorRMI<ARCH,32>( _.opbase(), _.rmop(), _.i( int8_t() ) );
      else if (ic.opsize() == 64) return new RorRMI<ARCH,64>( _.opbase(), _.rmop(), _.i( int8_t() ) );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\xd0" ) /1 & RM() ))
  
    return new RorRMI<ARCH,8>( _.opbase(), _.rmop(), 1 );
  
  if (auto _ = match( ic, opcode( "\xd1" ) /1 & RM() ))
  
    {
      if      (ic.opsize() == 16) return new RorRMI<ARCH,16>( _.opbase(), _.rmop(), 1 );
      else if (ic.opsize() == 32) return new RorRMI<ARCH,32>( _.opbase(), _.rmop(), 1 );
      else if (ic.opsize() == 64) return new RorRMI<ARCH,64>( _.opbase(), _.rmop(), 1 );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\xd2" ) /1 & RM() ))
  
    return new RorRMCL<ARCH,8>( _.opbase(), _.rmop() );
  
  if (auto _ = match( ic, opcode( "\xd3" ) /1 & RM() ))
  
    {
      if      (ic.opsize() == 16) return new RorRMCL<ARCH,16>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 32) return new RorRMCL<ARCH,32>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 64) return new RorRMCL<ARCH,64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  return 0;
}};

/* RCL */

template <class ARCH, unsigned OPSIZE>
struct RclRMI : public Operation<ARCH>
{
  typedef typename ARCH::u8_t u8_t;
  RclRMI( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _imm ) : Operation<ARCH>( opbase ), rmop( _rmop ), imm(_imm) {} RMOp<ARCH> rmop; uint8_t imm;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "rcl", rmop.isreg() ) << DisasmI( imm ) << ',' << DisasmE( UI<OPSIZE>(), rmop ); }
  
  void execute( ARCH& arch ) const { arch.template rmwrite<OPSIZE>( rmop, eval_rcl( arch, arch.template rmread<OPSIZE>( rmop ), u8_t( imm ) ) ); }
};

template <class ARCH, unsigned OPSIZE>
struct RclRMCL : public Operation<ARCH>
{
  RclRMCL( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop ) : Operation<ARCH>( opbase ), rmop( _rmop ) {} RMOp<ARCH> rmop;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "rcl", rmop.isreg() ) << "%cl," << DisasmE( UI<OPSIZE>(), rmop ); }
  
  void execute( ARCH& arch ) const { arch.template rmwrite<OPSIZE>( rmop, eval_rcl( arch, arch.template rmread<OPSIZE>( rmop ), arch.regread8( 1 ) ) ); }
};

template <class ARCH> struct DC<ARCH,RCL> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\xc0" ) /2 & RM() & Imm<8>() ))
  
    return new RclRMI<ARCH,8>( _.opbase(), _.rmop(), _.i( int8_t() ) );
  
  if (auto _ = match( ic, opcode( "\xc1" ) /2 & RM() & Imm<8>() ))
  
    {
      if      (ic.opsize() == 16) return new RclRMI<ARCH,16>( _.opbase(), _.rmop(), _.i( int8_t() ) );
      else if (ic.opsize() == 32) return new RclRMI<ARCH,32>( _.opbase(), _.rmop(), _.i( int8_t() ) );
      else if (ic.opsize() == 64) return new RclRMI<ARCH,64>( _.opbase(), _.rmop(), _.i( int8_t() ) );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\xd0" ) /2 & RM() ))
  
    return new RclRMI<ARCH,8>( _.opbase(), _.rmop(), 1 );
  
  if (auto _ = match( ic, opcode( "\xd1" ) /2 & RM() ))
  
    {
      if      (ic.opsize() == 16) return new RclRMI<ARCH,16>( _.opbase(), _.rmop(), 1 );
      else if (ic.opsize() == 32) return new RclRMI<ARCH,32>( _.opbase(), _.rmop(), 1 );
      else if (ic.opsize() == 64) return new RclRMI<ARCH,64>( _.opbase(), _.rmop(), 1 );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\xd2" ) /2 & RM() ))
  
    return new RclRMCL<ARCH,8>( _.opbase(), _.rmop() );
  
  if (auto _ = match( ic, opcode( "\xd3" ) /2 & RM() ))
  
    {
      if      (ic.opsize() == 16) return new RclRMCL<ARCH,16>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 32) return new RclRMCL<ARCH,32>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 64) return new RclRMCL<ARCH,64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  return 0;
}};

/* RCR */

template <class ARCH, unsigned OPSIZE>
struct RcrRMI : public Operation<ARCH>
{
  typedef typename ARCH::u8_t u8_t;
  RcrRMI( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _imm ) : Operation<ARCH>( opbase ), rmop( _rmop ), imm(_imm) {} RMOp<ARCH> rmop; uint8_t imm;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "rcr", rmop.isreg() ) << DisasmI( imm ) << ',' << DisasmE( UI<OPSIZE>(), rmop ); }
  
  void execute( ARCH& arch ) const { arch.template rmwrite<OPSIZE>( rmop, eval_rcr( arch, arch.template rmread<OPSIZE>( rmop ), u8_t( imm ) ) ); }
};

template <class ARCH, unsigned OPSIZE>
struct RcrRMCL : public Operation<ARCH>
{
  RcrRMCL( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop ) : Operation<ARCH>( opbase ), rmop( _rmop ) {} RMOp<ARCH> rmop;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "rcr", rmop.isreg() ) << "%cl," << DisasmE( UI<OPSIZE>(), rmop ); }
  
  void execute( ARCH& arch ) const { arch.template rmwrite<OPSIZE>( rmop, eval_rcr( arch, arch.template rmread<OPSIZE>( rmop ), arch.regread8( 1 ) ) ); }
};

template <class ARCH> struct DC<ARCH,RCR> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\xc0" ) /3 & RM() & Imm<8>() ))
  
    return new RcrRMI<ARCH,8>( _.opbase(), _.rmop(), _.i( int8_t() ) );
  
  if (auto _ = match( ic, opcode( "\xc1" ) /3 & RM() & Imm<8>() ))
  
    {
      if      (ic.opsize() == 16) return new RcrRMI<ARCH,16>( _.opbase(), _.rmop(), _.i( int8_t() ) );
      else if (ic.opsize() == 32) return new RcrRMI<ARCH,32>( _.opbase(), _.rmop(), _.i( int8_t() ) );
      else if (ic.opsize() == 64) return new RcrRMI<ARCH,64>( _.opbase(), _.rmop(), _.i( int8_t() ) );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\xd0" ) /3 & RM() ))
  
    return new RcrRMI<ARCH,8>( _.opbase(), _.rmop(), 1 );
  
  if (auto _ = match( ic, opcode( "\xd1" ) /3 & RM() ))
  
    {
      if      (ic.opsize() == 16) return new RcrRMI<ARCH,16>( _.opbase(), _.rmop(), 1 );
      else if (ic.opsize() == 32) return new RcrRMI<ARCH,32>( _.opbase(), _.rmop(), 1 );
      else if (ic.opsize() == 64) return new RcrRMI<ARCH,64>( _.opbase(), _.rmop(), 1 );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\xd2" ) /3 & RM() ))
  
    return new RcrRMCL<ARCH,8>( _.opbase(), _.rmop() );
  
  if (auto _ = match( ic, opcode( "\xd3" ) /3 & RM() ))
  
    {
      if      (ic.opsize() == 16) return new RcrRMCL<ARCH,16>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 32) return new RcrRMCL<ARCH,32>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 64) return new RcrRMCL<ARCH,64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  return 0;
}};

/* SHL */

template <class ARCH, unsigned OPSIZE>
struct ShlRMI : public Operation<ARCH>
{
  typedef typename ARCH::u8_t u8_t;
  ShlRMI( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _imm ) : Operation<ARCH>( opbase ), rmop( _rmop ), imm(_imm) {} RMOp<ARCH> rmop; uint8_t imm;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "shl", rmop.isreg() ) << DisasmI( imm ) << ',' << DisasmE( UI<OPSIZE>(), rmop ); }
  
  void execute( ARCH& arch ) const { arch.template rmwrite<OPSIZE>( rmop, eval_shl( arch, arch.template rmread<OPSIZE>( rmop ), u8_t( imm ) ) ); }
};

template <class ARCH, unsigned OPSIZE>
struct ShlRMCL : public Operation<ARCH>
{
  ShlRMCL( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop ) : Operation<ARCH>( opbase ), rmop( _rmop ) {} RMOp<ARCH> rmop;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "shl", rmop.isreg() ) << "%cl," << DisasmE( UI<OPSIZE>(), rmop ); }
  
  void execute( ARCH& arch ) const { arch.template rmwrite<OPSIZE>( rmop, eval_shl( arch, arch.template rmread<OPSIZE>( rmop ), arch.regread8( 1 ) ) ); }
};

template <class ARCH> struct DC<ARCH,SHL> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\xc0" ) /4 & RM() & Imm<8>() ))
  
    return new ShlRMI<ARCH,8>( _.opbase(), _.rmop(), _.i( int8_t() ) );
  
  if (auto _ = match( ic, opcode( "\xc1" ) /4 & RM() & Imm<8>() ))
  
    {
      if      (ic.opsize() == 16) return new ShlRMI<ARCH,16>( _.opbase(), _.rmop(), _.i( int8_t() ) );
      else if (ic.opsize() == 32) return new ShlRMI<ARCH,32>( _.opbase(), _.rmop(), _.i( int8_t() ) );
      else if (ic.opsize() == 64) return new ShlRMI<ARCH,64>( _.opbase(), _.rmop(), _.i( int8_t() ) );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\xd0" ) /4 & RM() ))
  
    return new ShlRMI<ARCH,8>( _.opbase(), _.rmop(), 1 );
  
  if (auto _ = match( ic, opcode( "\xd1" ) /4 & RM() ))
  
    {
      if      (ic.opsize() == 16) return new ShlRMI<ARCH,16>( _.opbase(), _.rmop(), 1 );
      else if (ic.opsize() == 32) return new ShlRMI<ARCH,32>( _.opbase(), _.rmop(), 1 );
      else if (ic.opsize() == 64) return new ShlRMI<ARCH,64>( _.opbase(), _.rmop(), 1 );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\xd2" ) /4 & RM() ))
  
    return new ShlRMCL<ARCH,8>( _.opbase(), _.rmop() );
  
  if (auto _ = match( ic, opcode( "\xd3" ) /4 & RM() ))
  
    {
      if      (ic.opsize() == 16) return new ShlRMCL<ARCH,16>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 32) return new ShlRMCL<ARCH,32>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 64) return new ShlRMCL<ARCH,64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  return 0;
}};

/* SHR */

template <class ARCH, unsigned OPSIZE>
struct ShrRMI : public Operation<ARCH>
{
  typedef typename ARCH::u8_t u8_t;
  ShrRMI( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _imm ) : Operation<ARCH>( opbase ), rmop( _rmop ), imm(_imm) {} RMOp<ARCH> rmop; uint8_t imm;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "shr", rmop.isreg() ) << DisasmI( imm ) << ',' << DisasmE( UI<OPSIZE>(), rmop ); }
  
  void execute( ARCH& arch ) const { arch.template rmwrite<OPSIZE>( rmop, eval_shr( arch, arch.template rmread<OPSIZE>( rmop ), u8_t( imm ) ) ); }
};

template <class ARCH, unsigned OPSIZE>
struct ShrRMCL : public Operation<ARCH>
{
  ShrRMCL( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop ) : Operation<ARCH>( opbase ), rmop( _rmop ) {} RMOp<ARCH> rmop;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "shr", rmop.isreg() ) << "%cl," << DisasmE( UI<OPSIZE>(), rmop ); }
  
  void execute( ARCH& arch ) const { arch.template rmwrite<OPSIZE>( rmop, eval_shr( arch, arch.template rmread<OPSIZE>( rmop ), arch.regread8( 1 ) ) ); }
};

template <class ARCH> struct DC<ARCH,SHR> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\xc0" ) /5 & RM() & Imm<8>() ))
  
    return new ShrRMI<ARCH,8>( _.opbase(), _.rmop(), _.i( int8_t() ) );
  
  if (auto _ = match( ic, opcode( "\xc1" ) /5 & RM() & Imm<8>() ))
  
    {
      if      (ic.opsize() == 16) return new ShrRMI<ARCH,16>( _.opbase(), _.rmop(), _.i( int8_t() ) );
      else if (ic.opsize() == 32) return new ShrRMI<ARCH,32>( _.opbase(), _.rmop(), _.i( int8_t() ) );
      else if (ic.opsize() == 64) return new ShrRMI<ARCH,64>( _.opbase(), _.rmop(), _.i( int8_t() ) );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\xd0" ) /5 & RM() ))
  
    return new ShrRMI<ARCH,8>( _.opbase(), _.rmop(), 1 );
  
  if (auto _ = match( ic, opcode( "\xd1" ) /5 & RM() ))
  
    {
      if      (ic.opsize() == 16) return new ShrRMI<ARCH,16>( _.opbase(), _.rmop(), 1 );
      else if (ic.opsize() == 32) return new ShrRMI<ARCH,32>( _.opbase(), _.rmop(), 1 );
      else if (ic.opsize() == 64) return new ShrRMI<ARCH,64>( _.opbase(), _.rmop(), 1 );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\xd2" ) /5 & RM() ))
  
    return new ShrRMCL<ARCH,8>( _.opbase(), _.rmop() );
  
  if (auto _ = match( ic, opcode( "\xd3" ) /5 & RM() ))
  
    {
      if      (ic.opsize() == 16) return new ShrRMCL<ARCH,16>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 32) return new ShrRMCL<ARCH,32>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 64) return new ShrRMCL<ARCH,64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  return 0;
}};

/* SAR */

template <class ARCH, unsigned OPSIZE>
struct SarRMI : public Operation<ARCH>
{
  typedef typename ARCH::u8_t u8_t;
  SarRMI( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _imm ) : Operation<ARCH>( opbase ), rmop( _rmop ), imm(_imm) {} RMOp<ARCH> rmop; uint8_t imm;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "sar", rmop.isreg() ) << DisasmI( imm ) << ',' << DisasmE( UI<OPSIZE>(), rmop ); }
  
  void execute( ARCH& arch ) const { arch.template rmwrite<OPSIZE>( rmop, eval_sar( arch, arch.template rmread<OPSIZE>( rmop ), u8_t( imm ) ) ); }
};

template <class ARCH, unsigned OPSIZE>
struct SarRMCL : public Operation<ARCH>
{
  SarRMCL( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop ) : Operation<ARCH>( opbase ), rmop( _rmop ) {} RMOp<ARCH> rmop;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "sar", rmop.isreg() ) << "%cl," << DisasmE( UI<OPSIZE>(), rmop ); }
  
  void execute( ARCH& arch ) const { arch.template rmwrite<OPSIZE>( rmop, eval_sar( arch, arch.template rmread<OPSIZE>( rmop ), arch.regread8( 1 ) ) ); }
};

template <class ARCH> struct DC<ARCH,SAR> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\xc0" ) /7 & RM() & Imm<8>() ))
  
    return new SarRMI<ARCH,8>( _.opbase(), _.rmop(), _.i( int8_t() ) );
  
  if (auto _ = match( ic, opcode( "\xc1" ) /7 & RM() & Imm<8>() ))
  
    {
      if      (ic.opsize() == 16) return new SarRMI<ARCH,16>( _.opbase(), _.rmop(), _.i( int8_t() ) );
      else if (ic.opsize() == 32) return new SarRMI<ARCH,32>( _.opbase(), _.rmop(), _.i( int8_t() ) );
      else if (ic.opsize() == 64) return new SarRMI<ARCH,64>( _.opbase(), _.rmop(), _.i( int8_t() ) );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\xd0" ) /7 & RM() ))
  
    return new SarRMI<ARCH,8>( _.opbase(), _.rmop(), 1 );
  
  if (auto _ = match( ic, opcode( "\xd1" ) /7 & RM() ))
  
    {
      if      (ic.opsize() == 16) return new SarRMI<ARCH,16>( _.opbase(), _.rmop(), 1 );
      else if (ic.opsize() == 32) return new SarRMI<ARCH,32>( _.opbase(), _.rmop(), 1 );
      else if (ic.opsize() == 64) return new SarRMI<ARCH,64>( _.opbase(), _.rmop(), 1 );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\xd2" ) /7 & RM() ))
  
    return new SarRMCL<ARCH,8>( _.opbase(), _.rmop() );
  
  if (auto _ = match( ic, opcode( "\xd3" ) /7 & RM() ))
  
    {
      if      (ic.opsize() == 16) return new SarRMCL<ARCH,16>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 32) return new SarRMCL<ARCH,32>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 64) return new SarRMCL<ARCH,64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE>
struct TestRMG : public Operation<ARCH>
{
  TestRMG( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  
  void disasm( std::ostream& sink ) const { sink << "test " << DisasmG<OPSIZE>( gn ) << ',' << DisasmE( UI<OPSIZE>(), rmop ); }
  
  void execute( ARCH& arch ) const { eval_and( arch, arch.template rmread<OPSIZE>( rmop ), arch.template regread<OPSIZE>( gn ) ); }
};

template <class ARCH, unsigned OPSIZE>
struct TestRMI : public Operation<ARCH>
{
  typedef typename CTypeFor<OPSIZE>::s imm_type;
  typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
  TestRMI( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, imm_type _imm ) : Operation<ARCH>( opbase ), rmop( _rmop ), imm(_imm) {} RMOp<ARCH> rmop; imm_type imm;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "test", rmop.isreg() ) << DisasmI( imm ) << ',' << DisasmE( UI<OPSIZE>(), rmop ); }
  
  void execute( ARCH& arch ) const { eval_and( arch, arch.template rmread<OPSIZE>( rmop ), u_type(imm) ); }
};

template <class ARCH> struct DC<ARCH,TEST> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\x84" ) & RM() ))
  
    return new TestRMG<ARCH,8>( _.opbase(), _.rmop(), _.greg() );
  
  if (auto _ = match( ic, opcode( "\x85" ) & RM() ))
  
    {
      if      (ic.opsize() == 16) return new TestRMG<ARCH,16>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 32) return new TestRMG<ARCH,32>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 64) return new TestRMG<ARCH,64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\xf6" ) /0 & RM() & Imm<8>() ))
  
    return new TestRMI<ARCH,8>( _.opbase(), _.rmop(), _.i( int8_t() ) );
  
  if (auto _ = match( ic, OpSize<16>() & opcode( "\xf7" ) /0 & RM() & Imm<16>() ))
  
    return new TestRMI<ARCH,16>( _.opbase(), _.rmop(), _.i( int16_t() ) );
  
  if (auto _ = match( ic, OpSize<32>() & opcode( "\xf7" ) /0 & RM() & Imm<32>() ))
  
    return new TestRMI<ARCH,32>( _.opbase(), _.rmop(), _.i( int32_t() ) );
  
  if (auto _ = match( ic, OpSize<64>() & opcode( "\xf7" ) /0 & RM() & Imm<32>() ))
  
    return new TestRMI<ARCH,64>( _.opbase(), _.rmop(), _.i( int32_t() ) );
  
  if (auto _ = match( ic, opcode( "\xa8" ) & Imm<8>() ))
  
    return new TestRMI<ARCH,8>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int8_t() ) );
  
  if (auto _ = match( ic, OpSize<16>() & opcode( "\xa9" ) & Imm<16>() ))
  
    return new TestRMI<ARCH,16>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int16_t() ) );
  
  if (auto _ = match( ic, OpSize<32>() & opcode( "\xa9" ) & Imm<32>() ))
  
    return new TestRMI<ARCH,32>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int32_t() ) );
  
  if (auto _ = match( ic, OpSize<64>() & opcode( "\xa9" ) & Imm<32>() ))
  
    return new TestRMI<ARCH,64>( _.opbase(), make_rop<ARCH>( 0 ), _.i( int32_t() ) );
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE>
struct NotRM : public Operation<ARCH>
{
  NotRM( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop ) : Operation<ARCH>( opbase ), rmop( _rmop ) {} RMOp<ARCH> rmop;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "not", rmop.isreg() ) << DisasmE( UI<OPSIZE>(), rmop ); }
  
  void execute( ARCH& arch ) const { arch.template rmwrite<OPSIZE>( rmop, ~(arch.template rmread<OPSIZE>( rmop )) ); }
};

template <class ARCH> struct DC<ARCH,NOT> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\xf6" ) /2 & RM() ))
  
    return new NotRM<ARCH,8>( _.opbase(), _.rmop() );
  
  if (auto _ = match( ic, opcode( "\xf7" ) /2 & RM() ))
  
    {
      if      (ic.opsize() == 16) return new NotRM<ARCH,16>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 32) return new NotRM<ARCH,32>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 64) return new NotRM<ARCH,64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE>
struct NegRM : public Operation<ARCH>
{
  NegRM( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop ) : Operation<ARCH>( opbase ), rmop( _rmop ) {} RMOp<ARCH> rmop;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "neg", rmop.isreg() ) << DisasmE( UI<OPSIZE>(), rmop ); }
  
  void execute( ARCH& arch ) const { arch.template rmwrite<OPSIZE>( rmop, eval_sub( arch, typename TypeFor<ARCH,OPSIZE>::u( 0 ), arch.template rmread<OPSIZE>( rmop ) ) ); }
};

template <class ARCH> struct DC<ARCH,NEG> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\xf6" ) /3 & RM() ))
  
    return new NegRM<ARCH,8>( _.opbase(), _.rmop() );
  
  if (auto _ = match( ic, opcode( "\xf7" ) /3 & RM() ))
  
    {
      if      (ic.opsize() == 16) return new NegRM<ARCH,16>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 32) return new NegRM<ARCH,32>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 64) return new NegRM<ARCH,64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE>
struct DivE : public Operation<ARCH>
{
  DivE( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop ) : Operation<ARCH>( opbase ), rmop( _rmop ) {} RMOp<ARCH> rmop;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "div", rmop.isreg() ) << DisasmE( UI<OPSIZE>(), rmop ); }
  
  void execute( ARCH& arch ) const
  {
    unsigned const hidx = (OPSIZE == 8) ? 4 : 2;
    typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
    u_type hi = arch.template regread<OPSIZE>( hidx ), lo = arch.template regread<OPSIZE>( 0 );
    eval_div( arch, hi, lo, arch.template rmread<OPSIZE>( rmop ) );
    arch.template regwrite<OPSIZE>( hidx, hi );
    arch.template regwrite<OPSIZE>( 0, lo );
  }
};

template <class ARCH> struct DC<ARCH,DIV> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\xf6" ) /6 & RM() ))
  
    return new DivE<ARCH,8>( _.opbase(), _.rmop() );
  
  if (auto _ = match( ic, opcode( "\xf7" ) /6 & RM() ))
  
    {
      if      (ic.opsize() == 16) return new DivE<ARCH,16>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 32) return new DivE<ARCH,32>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 64) return new DivE<ARCH,64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE>
struct IDivE : public Operation<ARCH>
{
  IDivE( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop ) : Operation<ARCH>( opbase ), rmop( _rmop ) {} RMOp<ARCH> rmop;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "idiv", rmop.isreg() ) << DisasmE( UI<OPSIZE>(), rmop ); }
  
  void execute( ARCH& arch ) const
  {
    unsigned const hidx = (OPSIZE == 8) ? 4 : 2;
    typedef typename TypeFor<ARCH,OPSIZE>::s s_type;
    typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
    s_type hi = s_type( arch.template regread<OPSIZE>( hidx ) ), lo = s_type( arch.template regread<OPSIZE>( 0 ) );
    eval_div( arch, hi, lo, s_type( arch.template rmread<OPSIZE>( rmop ) ) );
    arch.template regwrite<OPSIZE>( hidx, u_type( hi ) );
    arch.template regwrite<OPSIZE>( 0, u_type( lo ) );
  }
};

template <class ARCH> struct DC<ARCH,IDIV> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\xf6" ) /7 & RM() ))
  
    return new IDivE<ARCH,8>( _.opbase(), _.rmop() );
  
  if (auto _ = match( ic, opcode( "\xf7" ) /7 & RM() ))
  
    {
      if      (ic.opsize() == 16) return new IDivE<ARCH,16>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 32) return new IDivE<ARCH,32>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 64) return new IDivE<ARCH,64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE>
struct MulE : public Operation<ARCH>
{
  MulE( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop ) : Operation<ARCH>( opbase ), rmop( _rmop ) {} RMOp<ARCH> rmop;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "mul", rmop.isreg() ) << DisasmE( UI<OPSIZE>(), rmop ); }
  
  void execute( ARCH& arch ) const
  {
    unsigned const hidx = (OPSIZE == 8) ? 4 : 2;
    typename TypeFor<ARCH,OPSIZE>::u hi, lo = arch.template regread<OPSIZE>( 0 );
    eval_mul( arch, hi, lo, arch.template rmread<OPSIZE>( rmop ) );
    arch.template regwrite<OPSIZE>( hidx, hi );
    arch.template regwrite<OPSIZE>( 0, lo );
  }
};

template <class ARCH> struct DC<ARCH,MUL> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\xf6" ) /4 & RM() ))
  
    return new MulE<ARCH,8>( _.opbase(), _.rmop() );
  
  if (auto _ = match( ic, opcode( "\xf7" ) /4 & RM() ))
  
    {
      if      (ic.opsize() == 16) return new MulE<ARCH,16>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 32) return new MulE<ARCH,32>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 64) return new MulE<ARCH,64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE>
struct IMulE : public Operation<ARCH>
{
  IMulE( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop ) : Operation<ARCH>( opbase ), rmop( _rmop ) {} RMOp<ARCH> rmop;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "imul", rmop.isreg() ) << DisasmE( UI<OPSIZE>(), rmop ); }
  
  void execute( ARCH& arch ) const
  {
    unsigned const hidx = (OPSIZE == 8) ? 4 : 2;
    typedef typename TypeFor<ARCH,OPSIZE>::s s_type;
    typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
    s_type hi, lo = s_type( arch.template regread<OPSIZE>( 0 ) );
    eval_mul( arch, hi, lo, s_type( arch.template rmread<OPSIZE>( rmop ) ) );
    arch.template regwrite<OPSIZE>( hidx, u_type( hi ) );
    arch.template regwrite<OPSIZE>( 0, u_type( lo ) );
  }
};

template <class ARCH, unsigned OPSIZE>
struct IMulGE : public Operation<ARCH>
{
  IMulGE( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  
  void disasm( std::ostream& sink ) const { sink << "imul " << DisasmE( UI<OPSIZE>(), rmop )  << ',' << DisasmG<OPSIZE>( gn ); }
  
  void execute( ARCH& arch ) const { arch.template regwrite<OPSIZE>( gn, arch.template regread<OPSIZE>( gn ) * arch.template rmread<OPSIZE>( rmop ) ); }
};

template <class ARCH, unsigned OPSIZE>
struct IMulGEI : public Operation<ARCH>
{
  typedef typename CTypeFor<OPSIZE>::s imm_type;
  
  IMulGEI( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn, imm_type _imm )
    : Operation<ARCH>( opbase ), rmop( _rmop ), imm(_imm), gn( _gn ) {} RMOp<ARCH> rmop; imm_type imm; uint8_t gn;
  
  void disasm( std::ostream& sink ) const { sink << "imul" << DisasmI( imm ) << ',' << DisasmE( UI<OPSIZE>(), rmop )  << ',' << DisasmG<OPSIZE>( gn ); }
  
  void execute( ARCH& arch ) const
  {
    typedef typename TypeFor<ARCH,OPSIZE>::s s_type;
    typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
    typedef typename atpinfo<ARCH,s_type>::twice twice;
    typedef typename ARCH::bit_t bit_t;
    
    twice res  = twice( imm ) * twice( s_type( arch.template rmread<OPSIZE>( rmop ) ) );
    arch.template rmwrite<OPSIZE>( rmop, u_type( res ) );
    bit_t flag = res != twice( s_type( res ) );
    arch.flagwrite( ARCH::FLAG::OF, flag );
    arch.flagwrite( ARCH::FLAG::CF, flag );
  }
};

template <class ARCH> struct DC<ARCH,IMUL> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\xf6" ) /5 & RM() ))
  
    return new IMulE<ARCH,8>( _.opbase(), _.rmop() );
  
  if (auto _ = match( ic, opcode( "\xf7" ) /5 & RM() ))
  
    {
      if      (ic.opsize() == 16) return new IMulE<ARCH,16>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 32) return new IMulE<ARCH,32>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 64) return new IMulE<ARCH,64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\x0f\xaf" ) & RM() ))
  
    {
      if      (ic.opsize() == 16) return new IMulGE<ARCH,16>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 32) return new IMulGE<ARCH,32>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 64) return new IMulGE<ARCH,64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\x6b" ) & RM() & Imm<8>() ))
  
    {
      if      (ic.opsize() == 16) return new IMulGEI<ARCH,16>( _.opbase(), _.rmop(), _.greg(), _.i( int16_t() ) );
      else if (ic.opsize() == 32) return new IMulGEI<ARCH,32>( _.opbase(), _.rmop(), _.greg(), _.i( int32_t() ) );
      //else if (ic.opsize() == 64) return new IMulGEI<ARCH,64>( _.opbase(), _.rmop(), _.greg(), _.i( int64_t() ) );
      return 0;
    }
  
  if (auto _ = match( ic, OpSize<16>() & opcode( "\x69" ) & RM() & Imm<16>() ))
  
    return new IMulGEI<ARCH,16>( _.opbase(), _.rmop(), _.greg(), _.i( int16_t() ) );
  
  if (auto _ = match( ic, OpSize<32>() & opcode( "\x69" ) & RM() & Imm<32>() ))
  
    return new IMulGEI<ARCH,16>( _.opbase(), _.rmop(), _.greg(), _.i( int32_t() ) );
  
  if (auto _ = match( ic, OpSize<64>() & opcode( "\x69" ) & RM() & Imm<32>() ))
  
    return new IMulGEI<ARCH,16>( _.opbase(), _.rmop(), _.greg(), _.i( int64_t() ) );
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE>
struct Lea : public Operation<ARCH>
{
  Lea( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  
  void disasm( std::ostream& sink ) const { sink << "lea " << DisasmM( rmop ) << ',' << DisasmG<OPSIZE>( gn ); }
  
  void execute( ARCH& arch ) const { arch.template regwrite<OPSIZE>( gn, typename TypeFor<ARCH,OPSIZE>::u( rmop->effective_address( arch ) ) ); }
};

template <class ARCH> struct DC<ARCH,LEA> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\x8d" ) & RM_mem() ))
  
    {
      if      (ic.opsize() == 16) return new Lea<ARCH,16>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 32) return new Lea<ARCH,32>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 64) return new Lea<ARCH,64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE>
struct Inc : public Operation<ARCH>
{
  Inc( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop ) : Operation<ARCH>( opbase ), rmop( _rmop ) {} RMOp<ARCH> rmop;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "inc", rmop.isreg() ) << DisasmE( UI<OPSIZE>(), rmop ); }
  
  typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
  void execute( ARCH& arch ) const
  {
    typedef typename ARCH::bit_t bit_t;
    // CF is not affected
    bit_t savedCF = arch.flagread( ARCH::FLAG::CF );
    arch.template rmwrite<OPSIZE>( rmop, eval_add( arch, arch.template rmread<OPSIZE>( rmop ), u_type( 1 ) ) );
    arch.flagwrite( ARCH::FLAG::CF, savedCF );
  }
};

template <class ARCH, unsigned OPSIZE>
struct Dec : public Operation<ARCH>
{
  Dec( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop ) : Operation<ARCH>( opbase ), rmop( _rmop ) {} RMOp<ARCH> rmop;
  
  void disasm( std::ostream& sink ) const { sink << DisasmMnemonic<OPSIZE>( "dec", rmop.isreg() ) << DisasmE( UI<OPSIZE>(), rmop ); }
  
  typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
  void execute( ARCH& arch ) const
  {
    typedef typename ARCH::bit_t bit_t;
    // CF is not affected
    bit_t savedCF = arch.flagread( ARCH::FLAG::CF );
    arch.template rmwrite<OPSIZE>( rmop, eval_sub( arch, arch.template rmread<OPSIZE>( rmop ), u_type( 1 ) ) );
    arch.flagwrite( ARCH::FLAG::CF, savedCF );
  }
};

template <class ARCH> struct DC<ARCH,INCDEC> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\x40" ) + Reg() ))

    {
      if      (ic.opsize() == 16) return new Inc<ARCH,16>( _.opbase(), make_rop<ARCH>( _.ereg() ) );
      else if (ic.opsize() == 32) return new Inc<ARCH,32>( _.opbase(), make_rop<ARCH>( _.ereg() ) );
      return 0;
    }

  if (auto _ = match( ic, opcode( "\xfe" ) /0 & RM() ))
  
    return new Inc<ARCH,8>( _.opbase(), _.rmop() );

  if (auto _ = match( ic, opcode( "\xff" ) /0 & RM() ))
  
    {
      if      (ic.opsize() == 16) return new Inc<ARCH,16>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 32) return new Inc<ARCH,32>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 64) return new Inc<ARCH,64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\x48" ) + Reg() ))

    {
      if      (ic.opsize() == 16) return new Dec<ARCH,16>( _.opbase(), make_rop<ARCH>( _.ereg() ) );
      else if (ic.opsize() == 32) return new Dec<ARCH,32>( _.opbase(), make_rop<ARCH>( _.ereg() ) );
      return 0;
    }

  if (auto _ = match( ic, opcode( "\xfe" ) /1 & RM() ))
  
    return new Dec<ARCH,8>( _.opbase(), _.rmop() );

  if (auto _ = match( ic, opcode( "\xff" ) /1 & RM() ))
  
    {
      if      (ic.opsize() == 16) return new Dec<ARCH,16>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 32) return new Dec<ARCH,32>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 64) return new Dec<ARCH,64>( _.opbase(), _.rmop() );
      return 0;
    }
  
  return 0;
}};

template <class ARCH>
struct SetCC : public Operation<ARCH>
{
  typedef typename ARCH::u8_t u8_t;
  SetCC( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _cc ) : Operation<ARCH>( opbase ), rmop( _rmop ), cc( _cc ) {} RMOp<ARCH> rmop; uint8_t cc;
  
  void disasm( std::ostream& sink ) const { sink << "set" << DisasmCond( cc ) << ' ' << DisasmEb( rmop ); }
  
  void execute( ARCH& arch ) const { arch.template rmwrite<8>( rmop, u8_t( eval_cond( arch, cc ) ) ); }
};

template <class ARCH> struct DC<ARCH,SETCC> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, (opcode( "\x0f\x90" ) + Var<4>()) & RM() ))
    
    return new SetCC<ARCH>( _.opbase(), _.rmop(), _.var() );
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE>
struct ShldIM : public Operation<ARCH>
{
  typedef typename ARCH::u8_t u8_t;
  ShldIM( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn, uint8_t _sh )
    : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ), shift( _sh & 0b11111 ) {} RMOp<ARCH> rmop; uint8_t gn; uint8_t shift;
  /* TODO: shouldn't shift be truncated according to OPSIZE (especially in 64bit) */
  
  void disasm( std::ostream& sink ) const { sink << "shld " << DisasmI( shift ) << ',' << DisasmG<OPSIZE>( gn ) << ',' << DisasmE( UI<OPSIZE>(), rmop ); }

  void execute( ARCH& arch ) const
  {
    typename TypeFor<ARCH,OPSIZE>::u result = eval_shl( arch, arch.template rmread<OPSIZE>( rmop ), u8_t( shift ) ) | (arch.template regread<OPSIZE>( gn ) >> (OPSIZE - shift));
    arch.template rmwrite<OPSIZE>( rmop, result );
  }
};

template <class ARCH, unsigned OPSIZE>
struct ShldCL : public Operation<ARCH>
{
  typedef typename ARCH::u8_t u8_t;
  ShldCL( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn )
    : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  
  void disasm( std::ostream& sink ) const { sink << "shld %cl," << DisasmG<OPSIZE>( gn ) << ',' << DisasmE( UI<OPSIZE>(), rmop ); }

  void execute( ARCH& arch ) const
  {
    u8_t shift = arch.template regread<8>( 1 ) & u8_t( 0x1f );
    typename TypeFor<ARCH,OPSIZE>::u result = eval_shl( arch, arch.template rmread<OPSIZE>( rmop ), shift ) | (arch.template regread<OPSIZE>( gn ) >> (u8_t(OPSIZE) - shift));
    arch.template rmwrite<OPSIZE>( rmop, result );
  }
};

template <class ARCH, unsigned OPSIZE>
struct ShrdIM : public Operation<ARCH>
{
  typedef typename ARCH::u8_t u8_t;
  ShrdIM( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn, uint8_t _sh )
    : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ), sh( _sh ) {} RMOp<ARCH> rmop; uint8_t gn; uint8_t sh;
  
  void disasm( std::ostream& sink ) const { sink << "shrd " << DisasmI( sh ) << ',' << DisasmG<OPSIZE>( gn ) << ',' << DisasmE( UI<OPSIZE>(), rmop ); }

  void execute( ARCH& arch ) const
  {
    u8_t shift = u8_t( sh & 0b11111 );
    typename TypeFor<ARCH,OPSIZE>::u result = eval_shr( arch, arch.template rmread<OPSIZE>( rmop ), shift ) | (arch.template regread<OPSIZE>( gn ) << (u8_t( OPSIZE ) - shift));
    arch.template rmwrite<OPSIZE>( rmop, result );
  }
};

template <class ARCH, unsigned OPSIZE>
struct ShrdCL : public Operation<ARCH>
{
  typedef typename ARCH::u8_t u8_t;
  ShrdCL( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn )
    : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  
  void disasm( std::ostream& sink ) const { sink << "shrd %cl," << DisasmG<OPSIZE>( gn ) << ',' << DisasmE( UI<OPSIZE>(), rmop ); }

  void execute( ARCH& arch ) const
  {
    u8_t shift = arch.template regread<8>( 1 ) & u8_t( 0x1f );
    typename TypeFor<ARCH,OPSIZE>::u result = eval_shr( arch, arch.template rmread<OPSIZE>( rmop ), shift ) | (arch.template regread<OPSIZE>( gn ) << (u8_t( OPSIZE ) - shift));
    arch.template rmwrite<OPSIZE>( rmop, result );
  }
};

template <class ARCH> struct DC<ARCH,SHD> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\x0f\xa4" ) & RM() & Imm<8>() ))
  
    {
      if      (ic.opsize() == 16) return new ShldIM<ARCH,16>( _.opbase(), _.rmop(), _.greg(), _.i( uint8_t() ) );
      else if (ic.opsize() == 32) return new ShldIM<ARCH,32>( _.opbase(), _.rmop(), _.greg(), _.i( uint8_t() ) );
      else if (ic.opsize() == 64) return new ShldIM<ARCH,64>( _.opbase(), _.rmop(), _.greg(), _.i( uint8_t() ) );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\x0f\xa5" ) & RM() ))
  
    {
      if      (ic.opsize() == 16) return new ShldCL<ARCH,16>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 32) return new ShldCL<ARCH,32>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 64) return new ShldCL<ARCH,64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\x0f\xac" ) & RM() & Imm<8>() ))
  
    {
      if      (ic.opsize() == 16) return new ShrdIM<ARCH,16>( _.opbase(), _.rmop(), _.greg(), _.i( uint8_t() ) );
      else if (ic.opsize() == 32) return new ShrdIM<ARCH,32>( _.opbase(), _.rmop(), _.greg(), _.i( uint8_t() ) );
      else if (ic.opsize() == 64) return new ShrdIM<ARCH,64>( _.opbase(), _.rmop(), _.greg(), _.i( uint8_t() ) );
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\x0f\xad" ) & RM() ))
  
    {
      if      (ic.opsize() == 16) return new ShrdCL<ARCH,16>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 32) return new ShrdCL<ARCH,32>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 64) return new ShrdCL<ARCH,64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE, bool LEAD>
struct BitScan : public Operation<ARCH>
{
  BitScan( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  
  void disasm( std::ostream& sink ) const { sink << (LEAD ? "bsr " : "bsf ") << DisasmE( UI<OPSIZE>(), rmop ) << ',' << DisasmG<OPSIZE>( gn ); }
  
  void execute( ARCH& arch ) const
  {
    typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
    typedef typename ARCH::bit_t bit_t;
    u_type src = arch.template rmread<OPSIZE>( rmop );
    
    bit_t src_is_zero = (src == u_type( 0 ));
    arch.flagwrite( ARCH::FLAG::ZF, src_is_zero );
    if (arch.Cond( src_is_zero )) return;
    
    u_type res = LEAD ? (BitScanReverse( src )) : BitScanForward( src );
    
    arch.template regwrite<OPSIZE>( gn, res );
  }
};

template <class ARCH, unsigned OPSIZE, bool LEAD>
struct CountZeros : public Operation<ARCH>
{
  CountZeros( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  
  void disasm( std::ostream& sink ) const { sink << (LEAD ? "lzcnt " : "tzcnt ") << DisasmE( UI<OPSIZE>(), rmop ) << ',' << DisasmG<OPSIZE>( gn ); }
  
  void execute( ARCH& arch ) const
  {
    typedef typename TypeFor<ARCH,OPSIZE>::u u_type;
    typedef typename ARCH::bit_t bit_t;
    u_type src = arch.template rmread<OPSIZE>( rmop ), res;
    
    bit_t src_is_zero = (src == u_type( 0 ));
    arch.flagwrite( ARCH::FLAG::CF, src_is_zero );
    
    if (arch.Cond( src_is_zero )) {
      res = u_type( OPSIZE );
      arch.flagwrite( ARCH::FLAG::ZF, bit_t(0) );
    }
    else {
      res = LEAD ? (u_type(OPSIZE-1) - BitScanReverse( src )) : BitScanForward( src );
      arch.flagwrite( ARCH::FLAG::ZF, res == u_type(0) );
    }
    
    arch.template regwrite<OPSIZE>( gn, res );
  }
};

template <class ARCH> struct DC<ARCH,ZCNT> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\x0f\xbc" ) & RM() ))
  
    {
      if (not ic.f3()) {
        if      (ic.opsize() == 16) return new BitScan<ARCH,16,false>( _.opbase(), _.rmop(), _.greg() );
        else if (ic.opsize() == 32) return new BitScan<ARCH,32,false>( _.opbase(), _.rmop(), _.greg() );
        else if (ic.opsize() == 64) return new BitScan<ARCH,64,false>( _.opbase(), _.rmop(), _.greg() );
      } else {
        if      (ic.opsize() == 16) return new CountZeros<ARCH,16,false>( _.opbase(), _.rmop(), _.greg() );
        else if (ic.opsize() == 32) return new CountZeros<ARCH,32,false>( _.opbase(), _.rmop(), _.greg() );
        else if (ic.opsize() == 64) return new CountZeros<ARCH,64,false>( _.opbase(), _.rmop(), _.greg() );
      }
      return 0;
    }
  
  if (auto _ = match( ic, opcode( "\x0f\xbd" ) & RM() ))
  
    {
      if (not ic.f3()) {
        if      (ic.opsize() == 16) return new BitScan<ARCH,16,true>( _.opbase(), _.rmop(), _.greg() );
        else if (ic.opsize() == 32) return new BitScan<ARCH,32,true>( _.opbase(), _.rmop(), _.greg() );
        else if (ic.opsize() == 64) return new BitScan<ARCH,64,true>( _.opbase(), _.rmop(), _.greg() );
      } else {
        if      (ic.opsize() == 16) return new CountZeros<ARCH,16,true>( _.opbase(), _.rmop(), _.greg() );
        else if (ic.opsize() == 32) return new CountZeros<ARCH,32,true>( _.opbase(), _.rmop(), _.greg() );
        else if (ic.opsize() == 64) return new CountZeros<ARCH,64,true>( _.opbase(), _.rmop(), _.greg() );
      }
      return 0;
    }
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE>
struct Popcnt : public Operation<ARCH>
{
  Popcnt( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn )  : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  
  void disasm( std::ostream& sink ) const { sink << "popcnt " << DisasmE( UI<OPSIZE>(), rmop ) << DisasmG<OPSIZE>( gn ); }
  
  // void execute( ARCH& arch ) const
  // {
  //   typedef typename TypeFor<ARCH,OPSIZE>::u op_type;
  //   op_type const zero = op_type( 0 ), lsb = op_type( 1 );
  //   op_type tmp = rmop->read_e<OPSIZE>( arch ), cnt = op_type( 0 );
    
  //   while (tmp != zero) {
  //     cnt += (tmp & lsb);
  //     tmp >>= 1;
  //   }
    
  //   arch.template regwrite<OPSIZE>( gn, cnt );
  // }
};

template <class ARCH> struct DC<ARCH,POPCNT> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;
  
  if (auto _ = match( ic, RPF3() & opcode( "\x0f\xb8" ) & RM() ))
  
    {
      if      (ic.opsize() == 16) return new Popcnt<ARCH,16>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 32) return new Popcnt<ARCH,32>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 64) return new Popcnt<ARCH,64>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  return 0;
}};

template <class ARCH>
struct Cmc : public Operation<ARCH>
{
  Cmc( OpBase<ARCH> const& opbase ) : Operation<ARCH>( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "cmc"; }
  void execute( ARCH& arch ) const { arch.flagwrite( ARCH::FLAG::CF, not arch.flagread( ARCH::FLAG::CF ) ); }
};

template <class ARCH> struct DC<ARCH,CMC> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\xf5" ) ))

    return new Cmc<ARCH>( _.opbase() );

  return 0;
}};

template <class ARCH>
struct Sahf : public Operation<ARCH>
{
  typedef typename ARCH::u8_t u8_t;
  Sahf( OpBase<ARCH> const& opbase ) : Operation<ARCH>( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "sahf"; }
  void execute( ARCH& arch ) const
  {
    typedef typename ARCH::bit_t bit_t;
    u8_t ah = arch.regread8( 4 );
    arch.flagwrite( ARCH::FLAG::SF, bit_t( (ah >> 7) & u8_t( 1 ) ) );
    arch.flagwrite( ARCH::FLAG::ZF, bit_t( (ah >> 6) & u8_t( 1 ) ) );
    arch.flagwrite( ARCH::FLAG::AF, bit_t( (ah >> 4) & u8_t( 1 ) ) );
    arch.flagwrite( ARCH::FLAG::PF, bit_t( (ah >> 2) & u8_t( 1 ) ) );
    arch.flagwrite( ARCH::FLAG::CF, bit_t( (ah >> 0) & u8_t( 1 ) ) );
  }
};

template <class ARCH>
struct Lahf : public Operation<ARCH>
{
  typedef typename ARCH::u8_t u8_t;
  Lahf( OpBase<ARCH> const& opbase ) : Operation<ARCH>( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "lahf"; }
  void execute( ARCH& arch ) const
  {
    u8_t ah =
      (u8_t( arch.flagread( ARCH::FLAG::SF ) ) << 7) |
      (u8_t( arch.flagread( ARCH::FLAG::ZF ) ) << 6) |
      (u8_t(                 0   ) << 5) |
      (u8_t( arch.flagread( ARCH::FLAG::AF ) ) << 4) |
      (u8_t(                 0   ) << 3) |
      (u8_t( arch.flagread( ARCH::FLAG::PF ) ) << 2) |
      (u8_t(                 1   ) << 1) |
      (u8_t( arch.flagread( ARCH::FLAG::CF ) ) << 0);
    arch.template regwrite<8>( 4, ah );
  }
};

template <class ARCH> struct DC<ARCH,AHF> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;
  
  if (auto _ = match( ic, opcode( "\x9e" ) ))

    return new Sahf<ARCH>( _.opbase() );

  if (auto _ = match( ic, opcode( "\x9f" ) ))

    return new Lahf<ARCH>( _.opbase() );

  return 0;
}};

template <class ARCH, unsigned OPSIZE>
struct Csxe : public Operation<ARCH>
{
  Csxe( OpBase<ARCH> const& opbase ) : Operation<ARCH>( opbase ) {}
  
  void disasm( std::ostream& sink ) const { sink << 'c' << (&"btw\0wtl\0ltq\0"[4*SB<OPSIZE/16>::begin]); }
  void execute( ARCH& arch ) const
  {
    typedef typename TypeFor<ARCH,OPSIZE>::s sop_type;
    typedef typename TypeFor<ARCH,OPSIZE>::u uop_type;
    arch.template regwrite<OPSIZE>( 0, uop_type( sop_type( arch.template regread<OPSIZE>( 0 )  << (OPSIZE / 2) ) >> (OPSIZE / 2) ) );
  }
};

template <class ARCH, unsigned OPSIZE>
struct Csx : public Operation<ARCH>
{
  Csx( OpBase<ARCH> const& opbase ) : Operation<ARCH>( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << 'c' << (&"wtd\0ltd\0qtd\0"[4*SB<OPSIZE/16>::begin]); }
  // void execute( ARCH& arch ) const
  // {
  //   typedef typename TypeFor<ARCH,OPSIZE>::s sop_type;
  //   typedef typename TypeFor<ARCH,OPSIZE>::u uop_type;
  //   arch.template regwrite<OPSIZE>( 2, uop_type( sop_type( arch.template regread<OPSIZE>( 0 )  << (OPSIZE - 1) ) >> (OPSIZE - 1) ) );
  // }
};

template <class ARCH> struct DC<ARCH,CSX> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\x98" ) ))
  
    {
      if      (ic.opsize() == 16) return new Csxe<ARCH,16>( _.opbase() );
      else if (ic.opsize() == 32) return new Csxe<ARCH,32>( _.opbase() );
      else if (ic.opsize() == 64) return new Csxe<ARCH,64>( _.opbase() );
      return 0;
    }
  
  if (auto _ = match( ic,  opcode( "\x99" ) ))
  
    {
      if      (ic.opsize() == 16) return new Csx<ARCH,16>( _.opbase() );
      else if (ic.opsize() == 32) return new Csx<ARCH,32>( _.opbase() );
      else if (ic.opsize() == 64) return new Csx<ARCH,64>( _.opbase() );
      return 0;
    }
  
  return 0;
}};

template <class ARCH, unsigned ADDRSIZE>
struct Xlat : public Operation<ARCH>
{
  Xlat( OpBase<ARCH> const& opbase, uint8_t _segment ) : Operation<ARCH>( opbase ), segment( _segment ) {} uint8_t segment;
  void disasm( std::ostream& sink ) const { sink << "xlat"; }
  // void execute( ARCH& arch ) const
  // {
  //   typedef typename TypeFor<ARCH,ADDRSIZE>::u addr_type;
  //   arch.template regwrite<8>( 0, arch.template memread<8>( segment, arch.template regread<ADDRSIZE>( 3 ) + addr_type( arch.template regread<8>( 0 ) ) ) );
  // }
};

template <class ARCH> struct DC<ARCH,XLAT> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\xd7" ) ))

    {
      if      (ic.addrsize() == 16) return new Xlat<ARCH,16>( _.opbase(), ic.segment );
      else if (ic.addrsize() == 32) return new Xlat<ARCH,32>( _.opbase(), ic.segment );
      else if (ic.addrsize() == 64) return new Xlat<ARCH,64>( _.opbase(), ic.segment );
      return 0;
    }

  return 0;
}};

template <class ARCH>
struct AAA : public Operation<ARCH>
{
  AAA( OpBase<ARCH> const& opbase ) : Operation<ARCH>( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "aaa"; }
};

template <class ARCH>
struct AAS : public Operation<ARCH>
{
  AAS( OpBase<ARCH> const& opbase ) : Operation<ARCH>( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "aas"; }
};

template <class ARCH>
struct DAA : public Operation<ARCH>
{
  DAA( OpBase<ARCH> const& opbase ) : Operation<ARCH>( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "daa"; }
};

template <class ARCH>
struct DAS : public Operation<ARCH>
{
  DAS( OpBase<ARCH> const& opbase ) : Operation<ARCH>( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "das"; }
};

template <class ARCH>
struct AAD : public Operation<ARCH>
{
  AAD( OpBase<ARCH> const& opbase, uint8_t _imm ) : Operation<ARCH>( opbase ), imm(_imm) {} uint8_t imm;
  void disasm( std::ostream& sink ) const { sink << "aad" << DisasmI( imm ); }
};
  
template <class ARCH>
struct AAM : public Operation<ARCH>
{
  AAM( OpBase<ARCH> const& opbase, uint8_t _imm ) : Operation<ARCH>( opbase ), imm(_imm) {} uint8_t imm;
  void disasm( std::ostream& sink ) const { sink << "aam" << DisasmI( imm ); }
};

template <class ARCH> struct DC<ARCH,ADJUST> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.mode64()) return 0;
  
  if (auto _ = match( ic, opcode( "\xd5" ) & Imm<8>() ))
  
    return new AAD<ARCH>( _.opbase(), _.i( uint8_t() ) );

  if (auto _ = match( ic, opcode( "\xd4" ) & Imm<8>() ))

    return new AAM<ARCH>( _.opbase(), _.i( uint8_t() ) );

  if (auto _ = match( ic, opcode( "\x27" ) ))
 
    return new DAA<ARCH>( _.opbase() );

  if (auto _ = match( ic, opcode( "\x2f" ) ))

    return new DAS<ARCH>( _.opbase() );

  if (auto _ = match( ic, opcode( "\x37" ) ))

    return new AAA<ARCH>( _.opbase() );

  if (auto _ = match( ic, opcode( "\x3f" ) ))

    return new AAS<ARCH>( _.opbase() );
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE>
struct Bound : public Operation<ARCH>
{
  Bound( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "bound " << DisasmG<OPSIZE>( gn ) << ',' << DisasmM( rmop ); }
};

template <class ARCH> struct DC<ARCH,BOUND> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.mode64()) return 0;
  
  if (auto _ = match( ic, opcode( "\x62" ) & RM() ))
  
    {
      if      (ic.opsize() == 16) return new Bound<ARCH,16>( _.opbase(), _.rmop(), _.greg() );
      else if (ic.opsize() == 32) return new Bound<ARCH,32>( _.opbase(), _.rmop(), _.greg() );
      return 0;
    }
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE>
struct Adcx : public Operation<ARCH>
{
  Adcx( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "adcx " << DisasmE( UI<OPSIZE>(), rmop ) << ',' << DisasmG<OPSIZE>( gn ); }
};

template <class ARCH, unsigned OPSIZE>
struct Adox : public Operation<ARCH>
{
  Adox( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop, uint8_t _gn ) : Operation<ARCH>( opbase ), rmop( _rmop ), gn( _gn ) {} RMOp<ARCH> rmop; uint8_t gn;
  void disasm( std::ostream& sink ) const { sink << "adox " << DisasmE( UI<OPSIZE>(), rmop ) << ',' << DisasmG<OPSIZE>( gn ); }
};

template <class ARCH> struct DC<ARCH,ADF> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.f0()) return 0;
  
  /* ADCX -- Unsigned Integer Addition of Two Operands with Carry Flag */
  if (auto _ = match( ic, RP_() & OP66() & opcode( "\x0f\x38\xf6" ) & RM() ))
  
    {
      if (ic.opsize() == 64) return new Adcx<ARCH,64>( _.opbase(), _.rmop(), _.greg() );
      else                   return new Adcx<ARCH,32>( _.opbase(), _.rmop(), _.greg() );
    }
  
  if (auto _ = match( ic, RPF3() & opcode( "\x0f\x38\xf6" ) & RM() ))
  
    {
      if (ic.opsize() == 64) return new Adox<ARCH,64>( _.opbase(), _.rmop(), _.greg() );
      else                   return new Adox<ARCH,32>( _.opbase(), _.rmop(), _.greg() );
    }
  
  return 0;
}};

