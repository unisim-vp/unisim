// TODO: adress size should be managed correctly. In the end, memory
// reference should, most probably, all go through the ModRM mechanism
// (to hide address size management).

// struct StrOp
// {
//   SrtOp( uint8_t _segment, uint8_t _reg ) : segment( _segment ), reg( _reg ) {} uint8_t segment; uint8_t reg;
//   virtual ~MOp() {}
//   virtual void  disasm_memory_operand( std::ostream& _sink ) const { throw 0; };
//   virtual u32_t effective_address( ARCH& _arch ) const { throw 0; return u32_t( 0 ); };
// };

template <class ARCH>
struct StringEngine
{
  virtual bool tstcounter( ARCH& arch ) const = 0;
  virtual void deccounter( ARCH& arch ) const = 0;
  
  virtual MOp<ARCH> const* getdst() const = 0;
  virtual MOp<ARCH> const* getsrc( uint8_t segment ) const = 0;
  
  virtual void addsrc( ARCH& arch, int step ) const = 0;
  virtual void adddst( ARCH& arch, int step ) const = 0;
  
  virtual ~StringEngine() {};
};

template <class ARCH, unsigned ADDRSIZE>
struct _StringEngine : public StringEngine<ARCH>
{
  typedef typename TypeFor<ADDRSIZE>::u uaddr_type;
  typedef typename TypeFor<ADDRSIZE>::s saddr_type;
  ModBase<ARCH,ADDRSIZE> dst, src[6];
  _StringEngine() : dst( ES, 7 ), src{{0, 6}, {1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6}} {}
  
  bool tstcounter( ARCH& arch ) const { return arch.Cond( arch.template regread<ADDRSIZE>( 1 ) != uaddr_type( 0 )); }
  void deccounter( ARCH& arch ) const { arch.template regwrite<ADDRSIZE>( 1, arch.template regread<ADDRSIZE>( 1 ) - uaddr_type( 1 ) ); }
  
  MOp<ARCH> const* getdst() const { return &dst; }
  MOp<ARCH> const* getsrc( uint8_t segment ) const { return &src[segment]; }
  
  void addsrc( ARCH& arch, int step ) const { arch.template regwrite<ADDRSIZE>( 6, arch.template regread<ADDRSIZE>( 6 ) + uaddr_type( saddr_type( step ) ) ); }
  void adddst( ARCH& arch, int step ) const { arch.template regwrite<ADDRSIZE>( 7, arch.template regread<ADDRSIZE>( 7 ) + uaddr_type( saddr_type( step ) ) ); }
};

namespace {
  template <class ARCH>
  StringEngine<ARCH>*
  mkse( InputCode<ARCH> const& ic )
  {
    static _StringEngine<ARCH,16> se16;
    static _StringEngine<ARCH,32> se32;
    static _StringEngine<ARCH,64> se64;
    
    if (ic.addrsize()==16) return &se16;
    if (ic.addrsize()==32) return &se32;
    if (ic.addrsize()==64) return &se64;
    throw 0;
    return 0;
  }
}

template <class ARCH, unsigned OPSIZE, bool REP>
struct Movs : public Operation<ARCH>
{
  Movs( OpBase<ARCH> const& opbase, uint8_t _segment, StringEngine<ARCH>* _str ) : Operation<ARCH>( opbase ), segment( _segment ), str( _str ) {} uint8_t segment; StringEngine<ARCH>* str;
  
  void disasm( std::ostream& _sink ) const {
    _sink << (REP?"rep ":"") << DisasmMnemonic<OPSIZE>( "movs" ) << DisasmS( segment ) << ":(" << DisasmRd( 6 ) << ")," << DisasmS( 0 ) << ":(" << DisasmRd( 7 ) << ")";
  }
  
  void execute( ARCH& arch ) const
  {
    typedef typename ARCH::u32_t u32_t;
    
    if (REP and not str->tstcounter( arch )) return;
    
    arch.template rmwrite<OPSIZE>( str->getdst(), arch.template rmread<OPSIZE>( str->getsrc( segment ) ) );
    
    int32_t step = arch.Cond( arch.flagread( ARCH::DF ) ) ? -int32_t(OPSIZE/8) : +int32_t(OPSIZE/8);
    str->addsrc( arch, step );
    str->adddst( arch, step );
    
    if (REP) {
      str->deccounter( arch );
      arch.seteip( u32_t( Operation<ARCH>::address ) );
    }
  }
};

template <class ARCH> struct DC<ARCH,MOVS> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\xa4" ) ))
  
    {
      if (ic.rep==0) return new Movs<ARCH,8,false>( _.opbase(), ic.segment, mkse( ic ) );
      else           return new Movs<ARCH,8, true>( _.opbase(), ic.segment, mkse( ic ) );
    }
  
  if (auto _ = match( ic, opcode( "\xa5" ) ))
  
    {
      if (ic.opsize()==16) { if (ic.rep==0) return new Movs<ARCH,16,false>( _.opbase(), ic.segment, mkse( ic ) ); return new Movs<ARCH,16, true>( _.opbase(), ic.segment, mkse( ic ) ); }
      if (ic.opsize()==32) { if (ic.rep==0) return new Movs<ARCH,32,false>( _.opbase(), ic.segment, mkse( ic ) ); return new Movs<ARCH,32, true>( _.opbase(), ic.segment, mkse( ic ) ); }
      if (ic.opsize()==64) { if (ic.rep==0) return new Movs<ARCH,64,false>( _.opbase(), ic.segment, mkse( ic ) ); return new Movs<ARCH,64, true>( _.opbase(), ic.segment, mkse( ic ) ); }
      return 0;
    }
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE, bool REP>
struct Stos : public Operation<ARCH>
{
  Stos( OpBase<ARCH> const& opbase, StringEngine<ARCH>* _str ) : Operation<ARCH>( opbase ), str( _str ) {} StringEngine<ARCH>* str;
  
  void disasm( std::ostream& _sink ) const { _sink << (REP?"rep ":"") << "stos " << DisasmR<OPSIZE>( 0 ) << ',' << DisasmS( 0 ) << ":(" << DisasmRd( 7 ) << ")"; }
  
  void execute( ARCH& arch ) const
  {
    typedef typename ARCH::u32_t u32_t;
    
    if (REP and not str->tstcounter( arch )) return;
    
    arch.template rmwrite<OPSIZE>( str->getdst(), arch.template regread<OPSIZE>( 0 ) );

    int32_t step = arch.Cond( arch.flagread( ARCH::DF ) ) ? -int32_t(OPSIZE/8) : +int32_t(OPSIZE/8);
    str->adddst( arch, step );
    
    if (REP) {
      str->deccounter( arch );
      arch.seteip( u32_t( Operation<ARCH>::address ) );
    }
  }
};

template <class ARCH> struct DC<ARCH,STOS> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\xaa" ) ))
  
    {
      if (ic.rep==0) return new Stos<ARCH,8,false>( _.opbase(), mkse( ic ) );
      else           return new Stos<ARCH,8, true>( _.opbase(), mkse( ic ) );
    }
  
  if (auto _ = match( ic, opcode( "\xab" ) ))
  
    {
      if (ic.opsize()==16) { if (ic.rep==0) return new Stos<ARCH,16,false>( _.opbase(), mkse( ic ) ); return new Stos<ARCH,16, true>( _.opbase(), mkse( ic ) ); }
      if (ic.opsize()==32) { if (ic.rep==0) return new Stos<ARCH,32,false>( _.opbase(), mkse( ic ) ); return new Stos<ARCH,32, true>( _.opbase(), mkse( ic ) ); }
      if (ic.opsize()==64) { if (ic.rep==0) return new Stos<ARCH,64,false>( _.opbase(), mkse( ic ) ); return new Stos<ARCH,64, true>( _.opbase(), mkse( ic ) ); }
      return 0;
    }
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE, unsigned REP>
struct Cmps : public Operation<ARCH>
{
  Cmps( OpBase<ARCH> const& opbase, uint8_t _segment, StringEngine<ARCH>* _str ) : Operation<ARCH>( opbase ), segment( _segment ), str( _str ) {} uint8_t segment; StringEngine<ARCH>* str;
  void disasm( std::ostream& _sink ) const
  {
    _sink << ((REP==0) ? "" : (REP&1) ? "repz " : "repnz ") << "cmpsb "
          << DisasmS( 0 ) << ":(" << DisasmRd( 7 ) << "),"
          << DisasmS( segment ) << ":(" << DisasmRd( 6 ) << ")";
  }
  
  void execute( ARCH& arch ) const
  {
    typedef typename ARCH::bit_t bit_t;
    typedef typename ARCH::u32_t u32_t;
    
    if (REP and not str->tstcounter( arch )) return;
    
    eval_sub( arch, arch.template rmread<OPSIZE>( str->getsrc( segment ) ), arch.template rmread<OPSIZE>( str->getdst() ) );
    
    int32_t step = arch.Cond( arch.flagread( ARCH::DF ) ) ? -int32_t(OPSIZE/8) : +int32_t(OPSIZE/8);
    str->adddst( arch, step );
    str->addsrc( arch, step );
    
    if (REP) {
      str->deccounter( arch );
      if (arch.Cond( bit_t( REP&1 ) ^ arch.flagread( ARCH::ZF ) )) return;
      arch.seteip( u32_t( Operation<ARCH>::address ) );
    }
  }
};

template <class ARCH> struct DC<ARCH,CMPS> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\xa6" ) ))
  
    {
      if (ic.rep==0) return new Cmps<ARCH,8,3>( _.opbase(), ic.segment, mkse( ic ) );
      if (ic.rep==2) return new Cmps<ARCH,8,2>( _.opbase(), ic.segment, mkse( ic ) );
      else           return new Cmps<ARCH,8,0>( _.opbase(), ic.segment, mkse( ic ) );
    }

  
  if (auto _ = match( ic, opcode( "\xa7" ) ))
  
    {
      if (ic.opsize()==16) {
        if (ic.rep==0) return new Cmps<ARCH,16,0>( _.opbase(), ic.segment, mkse( ic ) );
        if (ic.rep==2) return new Cmps<ARCH,16,2>( _.opbase(), ic.segment, mkse( ic ) );
        else           return new Cmps<ARCH,16,3>( _.opbase(), ic.segment, mkse( ic ) );
      }
      if (ic.opsize()==32) {
        if (ic.rep==0) return new Cmps<ARCH,32,0>( _.opbase(), ic.segment, mkse( ic ) );
        if (ic.rep==2) return new Cmps<ARCH,32,2>( _.opbase(), ic.segment, mkse( ic ) );
        else           return new Cmps<ARCH,32,3>( _.opbase(), ic.segment, mkse( ic ) );
      }
      if (ic.opsize()==64) {
        if (ic.rep==0) return new Cmps<ARCH,64,0>( _.opbase(), ic.segment, mkse( ic ) );
        if (ic.rep==2) return new Cmps<ARCH,64,2>( _.opbase(), ic.segment, mkse( ic ) );
        else           return new Cmps<ARCH,64,3>( _.opbase(), ic.segment, mkse( ic ) );
      }
      return 0;
    }
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE, unsigned REP>
struct Scas : public Operation<ARCH>
{
  Scas( OpBase<ARCH> const& opbase, StringEngine<ARCH>* _str ) : Operation<ARCH>( opbase ), str( _str ) {} StringEngine<ARCH>* str;
  
  void disasm( std::ostream& _sink ) const { _sink << "scas " << DisasmS( ES ) << ":(" << DisasmRd( 7 ) << ")," << DisasmR<OPSIZE>( 0 ); }
  
  void execute( ARCH& arch ) const
  {
    typedef typename ARCH::bit_t bit_t;
    typedef typename ARCH::u32_t u32_t;
    
    if (REP and not str->tstcounter( arch )) return;
    
    eval_sub( arch, arch.template rmread<OPSIZE>( str->getdst() ), arch.template regread<OPSIZE>( 0 ) );

    int32_t step = arch.Cond( arch.flagread( ARCH::DF ) ) ? -int32_t(OPSIZE/8) : +int32_t(OPSIZE/8);
    str->adddst( arch, step );
    
    if (REP) {
      str->deccounter( arch );
      if (arch.Cond( bit_t( REP&1 ) ^arch.flagread( ARCH::ZF ) )) return;
      arch.seteip( u32_t( Operation<ARCH>::address ) );
    }
  }
};

template <class ARCH> struct DC<ARCH,SCAS> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\xae" ) ))
  
    {
      if (ic.rep==0) return new Scas<ARCH,8,3>( _.opbase(), mkse( ic ) );
      if (ic.rep==2) return new Scas<ARCH,8,2>( _.opbase(), mkse( ic ) );
      else           return new Scas<ARCH,8,0>( _.opbase(), mkse( ic ) );
    }

  
  if (auto _ = match( ic, opcode( "\xaf" ) ))
  
    {
      if (ic.opsize()==16) {
        if (ic.rep==0) return new Scas<ARCH,16,0>( _.opbase(), mkse( ic ) );
        if (ic.rep==2) return new Scas<ARCH,16,2>( _.opbase(), mkse( ic ) );
        else           return new Scas<ARCH,16,3>( _.opbase(), mkse( ic ) );
      }
      if (ic.opsize()==32) {
        if (ic.rep==0) return new Scas<ARCH,32,0>( _.opbase(), mkse( ic ) );
        if (ic.rep==2) return new Scas<ARCH,32,2>( _.opbase(), mkse( ic ) );
        else           return new Scas<ARCH,32,3>( _.opbase(), mkse( ic ) );
      }
      if (ic.opsize()==64) {
        if (ic.rep==0) return new Scas<ARCH,64,0>( _.opbase(), mkse( ic ) );
        if (ic.rep==2) return new Scas<ARCH,64,2>( _.opbase(), mkse( ic ) );
        else           return new Scas<ARCH,64,3>( _.opbase(), mkse( ic ) );
      }
      return 0;
    }
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE, bool REP>
struct Lods : public Operation<ARCH>
{
  Lods( OpBase<ARCH> const& opbase, uint8_t _segment, StringEngine<ARCH>* _str ) : Operation<ARCH>( opbase ), segment( _segment ), str( _str ) {} uint8_t segment; StringEngine<ARCH>* str;
  
  void disasm( std::ostream& _sink ) const { _sink << (REP?"rep ":"") << "lods " << DisasmS( segment ) << ":(" << DisasmRd( 6 ) << ")," << DisasmRb( 0 ); }
  
  void execute( ARCH& arch ) const
  {
    typedef typename ARCH::u32_t u32_t;
    
    if (REP and str->tstcounter( arch )) return;
    
    arch.template regwrite<OPSIZE>( 0, arch.template rmread<OPSIZE>( str->getsrc( segment ) ) );
    
    int32_t step = arch.Cond( arch.flagread( ARCH::DF ) ) ? -int32_t(OPSIZE/8) : +int32_t(OPSIZE/8);
    str->addsrc( arch, step );
    
    if (REP) {
      str->deccounter( arch );
      arch.seteip( u32_t( Operation<ARCH>::address ) );
    }
  }
};

template <class ARCH> struct DC<ARCH,LODS> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\xac" ) ))
  
    {
      if (ic.rep==0) return new Lods<ARCH,8,false>( _.opbase(), ic.segment, mkse( ic ) );
      else           return new Lods<ARCH,8, true>( _.opbase(), ic.segment, mkse( ic ) );
    }
  
  if (auto _ = match( ic, opcode( "\xad" ) ))
  
    {
      if (ic.opsize()==16) { if (ic.rep==0) return new Lods<ARCH,16,false>( _.opbase(), ic.segment, mkse( ic ) ); return new Lods<ARCH,16, true>( _.opbase(), ic.segment, mkse( ic ) ); }
      if (ic.opsize()==32) { if (ic.rep==0) return new Lods<ARCH,32,false>( _.opbase(), ic.segment, mkse( ic ) ); return new Lods<ARCH,32, true>( _.opbase(), ic.segment, mkse( ic ) ); }
      if (ic.opsize()==64) { if (ic.rep==0) return new Lods<ARCH,64,false>( _.opbase(), ic.segment, mkse( ic ) ); return new Lods<ARCH,64, true>( _.opbase(), ic.segment, mkse( ic ) ); }
      return 0;
    }
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE, bool REP>
struct Outs : public Operation<ARCH>
{
  Outs( OpBase<ARCH> const& opbase, uint8_t _segment ) : Operation<ARCH>( opbase ), segment( _segment ) {} uint8_t segment;
  void disasm( std::ostream& _sink ) const { _sink << (REP?"rep ":"") << DisasmMnemonic<OPSIZE>( "outs" ) << DisasmS( segment ) << ":(" << DisasmRd( 6 ) << "),(" << DisasmRw( 2 ) << ")"; }
};

template <class ARCH> struct DC<ARCH,OUTS> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\x6e" ) ))
  
    {
      if (ic.rep==0) return new Outs<ARCH,8,false>( _.opbase(), ic.segment );
      else           return new Outs<ARCH,8, true>( _.opbase(), ic.segment );
    }
  
  if (auto _ = match( ic, opcode( "\x6f" ) ))
  
    {
      if (ic.opsize()==16) { if (ic.rep==0) return new Outs<ARCH,16,false>( _.opbase(), ic.segment ); return new Outs<ARCH,16, true>( _.opbase(), ic.segment ); }
      if (ic.opsize()==32) { if (ic.rep==0) return new Outs<ARCH,32,false>( _.opbase(), ic.segment ); return new Outs<ARCH,32, true>( _.opbase(), ic.segment ); }
      return 0;
    }
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE, bool REP>
struct Ins : public Operation<ARCH>
{
  Ins( OpBase<ARCH> const& opbase ) : Operation<ARCH>( opbase ) {};
  void disasm( std::ostream& _sink ) const { _sink << (REP?"rep ":"") << DisasmMnemonic<OPSIZE>( "ins" ) << "(" << DisasmRw( 2 ) << ")," << DisasmS( 0 ) << ":(" << DisasmRd( 7 ) << ")"; }
};

template <class ARCH> struct DC<ARCH,INS> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\x6c" ) ))
  
    {
      if (ic.rep==0) return new Ins<ARCH,8,false>( _.opbase() );
      else           return new Ins<ARCH,8, true>( _.opbase() );
    }
  
  if (auto _ = match( ic, opcode( "\x6d" ) ))
  
    {
      if (ic.opsize()==16) { if (ic.rep==0) return new Ins<ARCH,16,false>( _.opbase() ); return new Ins<ARCH,16, true>( _.opbase() ); }
      if (ic.opsize()==32) { if (ic.rep==0) return new Ins<ARCH,32,false>( _.opbase() ); return new Ins<ARCH,32, true>( _.opbase() ); }
      return 0;
    }
  
  return 0;
}};
