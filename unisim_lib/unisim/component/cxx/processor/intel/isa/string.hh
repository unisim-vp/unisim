// TODO: adress size should be managed correctly. In the end, memory
// reference should, most probably, all go through the ModRM mechanism
// (to hide address size management).

// struct StrOp
// {
//   SrtOp( uint8_t _segment, uint8_t _reg ) : segment( _segment ), reg( _reg ) {} uint8_t segment; uint8_t reg;
//   virtual ~MOp() {}
//   virtual void  disasm_memory_operand( std::ostream& _sink ) const { throw 0; };
//   virtual u32_t effective_address( Arch& _arch ) const { throw 0; return u32_t( 0 ); };
// };

struct StringEngine
{
  virtual bool tstcounter( Arch& arch ) const = 0;
  virtual void deccounter( Arch& arch ) const = 0;
  
  virtual _RMOp getdst() const = 0;
  virtual _RMOp getsrc( uint8_t segment ) const = 0;
  
  virtual void addsrc( Arch& arch, int step ) const = 0;
  virtual void adddst( Arch& arch, int step ) const = 0;
  
  virtual ~StringEngine() {};
};

template <unsigned ADDRSIZE>
struct _StringEngine : public StringEngine
{
  typedef typename TypeFor<ADDRSIZE>::u uaddr_type;
  typedef typename TypeFor<ADDRSIZE>::s saddr_type;
  ModBase<ADDRSIZE> dst, src[6];
  _StringEngine() : dst( ES, 7 ), src{{0, 6}, {1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6}} {}
  
  bool tstcounter( Arch& arch ) const { return mkbool( arch.regread<ADDRSIZE>( 1 ) != uaddr_type( 0 )); }
  void deccounter( Arch& arch ) const { arch.regwrite<ADDRSIZE>( 1, arch.regread<ADDRSIZE>( 1 ) - uaddr_type( 1 ) ); }
  
  _RMOp getdst() const { return &dst; }
  _RMOp getsrc( uint8_t segment ) const { return &src[segment]; }
  
  void addsrc( Arch& arch, int step ) const { arch.regwrite<ADDRSIZE>( 6, arch.regread<ADDRSIZE>( 6 ) + uaddr_type( saddr_type( step ) ) ); }
  void adddst( Arch& arch, int step ) const { arch.regwrite<ADDRSIZE>( 7, arch.regread<ADDRSIZE>( 7 ) + uaddr_type( saddr_type( step ) ) ); }
};

namespace {
  StringEngine*
  mkse( CodeBase const& cb )
  {
    static _StringEngine<16> se16;
    static _StringEngine<32> se32;
    static _StringEngine<64> se64;
    
    if (cb.addrsize()==16) return &se16;
    if (cb.addrsize()==32) return &se32;
    if (cb.addrsize()==64) return &se64;
    throw 0;
    return 0;
  }
}

template <unsigned OPSIZE, bool REP>
struct Movs : public Operation
{
  Movs( OpBase const& opbase, uint8_t _segment, StringEngine* _str ) : Operation( opbase ), segment( _segment ), str( _str ) {} uint8_t segment; StringEngine* str;
  
  void disasm( std::ostream& _sink ) const {
    _sink << (REP?"rep ":"") << DisasmMnemonic<OPSIZE>( "movs" ) << DisasmS( segment ) << ":(" << DisasmRd( 6 ) << ")," << DisasmS( 0 ) << ":(" << DisasmRd( 7 ) << ")";
  }
  
  void execute( Arch& arch ) const
  {
    if (REP and not str->tstcounter( arch )) return;
    
    arch.rmwrite<OPSIZE>( str->getdst(), arch.rmread<OPSIZE>( str->getsrc( segment ) ) );
    
    int32_t step = mkbool( arch.flagread( DF ) ) ? -int32_t(OPSIZE/8) : +int32_t(OPSIZE/8);
    str->addsrc( arch, step );
    str->adddst( arch, step );
    
    if (REP) {
      str->deccounter( arch );
      arch.seteip( u32_t( address ) );
    }
  }
};

template <> Operation* decode<MOVS>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xa4" ) ))
  
    {
      if (cb.rep==0) return new Movs<8,false>( _.opbase(), cb.segment, mkse( cb ) );
      else           return new Movs<8, true>( _.opbase(), cb.segment, mkse( cb ) );
    }
  
  if (auto _ = match( cb, opcode( "\xa5" ) ))
  
    {
      if (cb.opsize()==16) { if (cb.rep==0) return new Movs<16,false>( _.opbase(), cb.segment, mkse( cb ) ); return new Movs<16, true>( _.opbase(), cb.segment, mkse( cb ) ); }
      if (cb.opsize()==32) { if (cb.rep==0) return new Movs<32,false>( _.opbase(), cb.segment, mkse( cb ) ); return new Movs<32, true>( _.opbase(), cb.segment, mkse( cb ) ); }
      if (cb.opsize()==64) { if (cb.rep==0) return new Movs<64,false>( _.opbase(), cb.segment, mkse( cb ) ); return new Movs<64, true>( _.opbase(), cb.segment, mkse( cb ) ); }
      return 0;
    }
  
  return 0;
}

template <unsigned OPSIZE, bool REP>
struct Stos : public Operation
{
  Stos( OpBase const& opbase, StringEngine* _str ) : Operation( opbase ), str( _str ) {} StringEngine* str;
  
  void disasm( std::ostream& _sink ) const { _sink << (REP?"rep ":"") << "stos " << DisasmR<OPSIZE>( 0 ) << ',' << DisasmS( 0 ) << ":(" << DisasmRd( 7 ) << ")"; }
  
  void execute( Arch& arch ) const
  {
    if (REP and not str->tstcounter( arch )) return;
    
    arch.rmwrite<OPSIZE>( str->getdst(), arch.regread<OPSIZE>( 0 ) );

    int32_t step = mkbool( arch.flagread( DF ) ) ? -int32_t(OPSIZE/8) : +int32_t(OPSIZE/8);
    str->adddst( arch, step );
    
    if (REP) {
      str->deccounter( arch );
      arch.seteip( u32_t( address ) );
    }
  }
};

template <> Operation* decode<STOS>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xaa" ) ))
  
    {
      if (cb.rep==0) return new Stos<8,false>( _.opbase(), mkse( cb ) );
      else           return new Stos<8, true>( _.opbase(), mkse( cb ) );
    }
  
  if (auto _ = match( cb, opcode( "\xab" ) ))
  
    {
      if (cb.opsize()==16) { if (cb.rep==0) return new Stos<16,false>( _.opbase(), mkse( cb ) ); return new Stos<16, true>( _.opbase(), mkse( cb ) ); }
      if (cb.opsize()==32) { if (cb.rep==0) return new Stos<32,false>( _.opbase(), mkse( cb ) ); return new Stos<32, true>( _.opbase(), mkse( cb ) ); }
      if (cb.opsize()==64) { if (cb.rep==0) return new Stos<64,false>( _.opbase(), mkse( cb ) ); return new Stos<64, true>( _.opbase(), mkse( cb ) ); }
      return 0;
    }
  
  return 0;
}

template <unsigned OPSIZE, unsigned REP>
struct Cmps : public Operation
{
  Cmps( OpBase const& opbase, uint8_t _segment, StringEngine* _str ) : Operation( opbase ), segment( _segment ), str( _str ) {} uint8_t segment; StringEngine* str;
  void disasm( std::ostream& _sink ) const
  {
    _sink << ((REP==0) ? "" : (REP&1) ? "repz " : "repnz ") << "cmpsb "
          << DisasmS( 0 ) << ":(" << DisasmRd( 7 ) << "),"
          << DisasmS( segment ) << ":(" << DisasmRd( 6 ) << ")";
  }
  
  void execute( Arch& arch ) const
  {
    if (REP and not str->tstcounter( arch )) return;
    
    eval_sub( arch, arch.rmread<OPSIZE>( str->getsrc( segment ) ), arch.rmread<OPSIZE>( str->getdst() ) );
    
    int32_t step = mkbool( arch.flagread( DF ) ) ? -int32_t(OPSIZE/8) : +int32_t(OPSIZE/8);
    str->adddst( arch, step );
    str->addsrc( arch, step );
    
    if (REP) {
      str->deccounter( arch );
      if (mkbool( bit_t( REP&1 ) ^ arch.flagread( ZF ) )) return;
      arch.seteip( u32_t( address ) );
    }
  }
};

template <> Operation* decode<CMPS>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xa6" ) ))
  
    {
      if (cb.rep==0) return new Cmps<8,3>( _.opbase(), cb.segment, mkse( cb ) );
      if (cb.rep==2) return new Cmps<8,2>( _.opbase(), cb.segment, mkse( cb ) );
      else           return new Cmps<8,0>( _.opbase(), cb.segment, mkse( cb ) );
    }

  
  if (auto _ = match( cb, opcode( "\xa7" ) ))
  
    {
      if (cb.opsize()==16) {
        if (cb.rep==0) return new Cmps<16,0>( _.opbase(), cb.segment, mkse( cb ) );
        if (cb.rep==2) return new Cmps<16,2>( _.opbase(), cb.segment, mkse( cb ) );
        else           return new Cmps<16,3>( _.opbase(), cb.segment, mkse( cb ) );
      }
      if (cb.opsize()==32) {
        if (cb.rep==0) return new Cmps<32,0>( _.opbase(), cb.segment, mkse( cb ) );
        if (cb.rep==2) return new Cmps<32,2>( _.opbase(), cb.segment, mkse( cb ) );
        else           return new Cmps<32,3>( _.opbase(), cb.segment, mkse( cb ) );
      }
      if (cb.opsize()==64) {
        if (cb.rep==0) return new Cmps<64,0>( _.opbase(), cb.segment, mkse( cb ) );
        if (cb.rep==2) return new Cmps<64,2>( _.opbase(), cb.segment, mkse( cb ) );
        else           return new Cmps<64,3>( _.opbase(), cb.segment, mkse( cb ) );
      }
      return 0;
    }
  
  return 0;
}

template <unsigned OPSIZE, unsigned REP>
struct Scas : public Operation
{
  Scas( OpBase const& opbase, StringEngine* _str ) : Operation( opbase ), str( _str ) {} StringEngine* str;
  
  void disasm( std::ostream& _sink ) const { _sink << "scas " << DisasmS( ES ) << ":(" << DisasmRd( 7 ) << ")," << DisasmR<OPSIZE>( 0 ); }
  
  void execute( Arch& arch ) const
  {
    if (REP and not str->tstcounter( arch )) return;
    
    eval_sub( arch, arch.rmread<OPSIZE>( str->getdst() ), arch.regread<OPSIZE>( 0 ) );

    int32_t step = mkbool( arch.flagread( DF ) ) ? -int32_t(OPSIZE/8) : +int32_t(OPSIZE/8);
    str->adddst( arch, step );
    
    if (REP) {
      str->deccounter( arch );
      if (mkbool( bit_t( REP&1 ) ^arch.flagread( ZF ) )) return;
      arch.seteip( u32_t( address ) );
    }
  }
};

template <> Operation* decode<SCAS>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xae" ) ))
  
    {
      if (cb.rep==0) return new Scas<8,3>( _.opbase(), mkse( cb ) );
      if (cb.rep==2) return new Scas<8,2>( _.opbase(), mkse( cb ) );
      else           return new Scas<8,0>( _.opbase(), mkse( cb ) );
    }

  
  if (auto _ = match( cb, opcode( "\xaf" ) ))
  
    {
      if (cb.opsize()==16) {
        if (cb.rep==0) return new Scas<16,0>( _.opbase(), mkse( cb ) );
        if (cb.rep==2) return new Scas<16,2>( _.opbase(), mkse( cb ) );
        else           return new Scas<16,3>( _.opbase(), mkse( cb ) );
      }
      if (cb.opsize()==32) {
        if (cb.rep==0) return new Scas<32,0>( _.opbase(), mkse( cb ) );
        if (cb.rep==2) return new Scas<32,2>( _.opbase(), mkse( cb ) );
        else           return new Scas<32,3>( _.opbase(), mkse( cb ) );
      }
      if (cb.opsize()==64) {
        if (cb.rep==0) return new Scas<64,0>( _.opbase(), mkse( cb ) );
        if (cb.rep==2) return new Scas<64,2>( _.opbase(), mkse( cb ) );
        else           return new Scas<64,3>( _.opbase(), mkse( cb ) );
      }
      return 0;
    }
  
  return 0;
}

template <unsigned OPSIZE, bool REP>
struct Lods : public Operation
{
  Lods( OpBase const& opbase, uint8_t _segment, StringEngine* _str ) : Operation( opbase ), segment( _segment ), str( _str ) {} uint8_t segment; StringEngine* str;
  
  void disasm( std::ostream& _sink ) const { _sink << (REP?"rep ":"") << "lods " << DisasmS( segment ) << ":(" << DisasmRd( 6 ) << ")," << DisasmRb( 0 ); }
  
  void execute( Arch& arch ) const
  {
    if (REP and str->tstcounter( arch )) return;
    
    arch.regwrite<OPSIZE>( 0, arch.rmread<OPSIZE>( str->getsrc( segment ) ) );
    
    int32_t step = mkbool( arch.flagread( DF ) ) ? -int32_t(OPSIZE/8) : +int32_t(OPSIZE/8);
    str->addsrc( arch, step );
    
    if (REP) {
      str->deccounter( arch );
      arch.seteip( u32_t( address ) );
    }
  }
};

template <> Operation* decode<LODS>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xac" ) ))
  
    {
      if (cb.rep==0) return new Lods<8,false>( _.opbase(), cb.segment, mkse( cb ) );
      else           return new Lods<8, true>( _.opbase(), cb.segment, mkse( cb ) );
    }
  
  if (auto _ = match( cb, opcode( "\xad" ) ))
  
    {
      if (cb.opsize()==16) { if (cb.rep==0) return new Lods<16,false>( _.opbase(), cb.segment, mkse( cb ) ); return new Lods<16, true>( _.opbase(), cb.segment, mkse( cb ) ); }
      if (cb.opsize()==32) { if (cb.rep==0) return new Lods<32,false>( _.opbase(), cb.segment, mkse( cb ) ); return new Lods<32, true>( _.opbase(), cb.segment, mkse( cb ) ); }
      if (cb.opsize()==64) { if (cb.rep==0) return new Lods<64,false>( _.opbase(), cb.segment, mkse( cb ) ); return new Lods<64, true>( _.opbase(), cb.segment, mkse( cb ) ); }
      return 0;
    }
  
  return 0;
}

template <unsigned OPSIZE, bool REP>
struct Outs : public Operation
{
  Outs( OpBase const& opbase, uint8_t _segment ) : Operation( opbase ), segment( _segment ) {} uint8_t segment;
  void disasm( std::ostream& _sink ) const { _sink << (REP?"rep ":"") << DisasmMnemonic<OPSIZE>( "outs" ) << DisasmS( segment ) << ":(" << DisasmRd( 6 ) << "),(" << DisasmRw( 2 ) << ")"; }
};

template <> Operation* decode<OUTS>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\x6e" ) ))
  
    {
      if (cb.rep==0) return new Outs<8,false>( _.opbase(), cb.segment );
      else           return new Outs<8, true>( _.opbase(), cb.segment );
    }
  
  if (auto _ = match( cb, opcode( "\x6f" ) ))
  
    {
      if (cb.opsize()==16) { if (cb.rep==0) return new Outs<16,false>( _.opbase(), cb.segment ); return new Outs<16, true>( _.opbase(), cb.segment ); }
      if (cb.opsize()==32) { if (cb.rep==0) return new Outs<32,false>( _.opbase(), cb.segment ); return new Outs<32, true>( _.opbase(), cb.segment ); }
      return 0;
    }
  
  return 0;
}

template <unsigned OPSIZE, bool REP>
struct Ins : public Operation
{
  Ins( OpBase const& opbase ) : Operation( opbase ) {};
  void disasm( std::ostream& _sink ) const { _sink << (REP?"rep ":"") << DisasmMnemonic<OPSIZE>( "ins" ) << "(" << DisasmRw( 2 ) << ")," << DisasmS( 0 ) << ":(" << DisasmRd( 7 ) << ")"; }
};

template <> Operation* decode<INS>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\x6c" ) ))
  
    {
      if (cb.rep==0) return new Ins<8,false>( _.opbase() );
      else           return new Ins<8, true>( _.opbase() );
    }
  
  if (auto _ = match( cb, opcode( "\x6d" ) ))
  
    {
      if (cb.opsize()==16) { if (cb.rep==0) return new Ins<16,false>( _.opbase() ); return new Ins<16, true>( _.opbase() ); }
      if (cb.opsize()==32) { if (cb.rep==0) return new Ins<32,false>( _.opbase() ); return new Ins<32, true>( _.opbase() ); }
      return 0;
    }
  
  return 0;
}
