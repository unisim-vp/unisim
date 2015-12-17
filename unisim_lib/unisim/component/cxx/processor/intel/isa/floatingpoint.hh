struct F2xm1 : public Operation
{
  F2xm1( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "f2xm1"; }
  void execute( Arch& arch ) const { arch.fwrite( 0, power( f64_t( 2.0 ), arch.fread( 0 ) ) - f64_t( 1.0 ) ); }
};

template <> Operation* decode<F2XM1>( CodeBase const& cb )
{
  // F2XM1 - Compute 2^x - 1
  if (auto _ = match( cb, opcode( "\xd9\xf0" ) ))
  
    return new F2xm1( _.opbase() );
  
  return 0;
}

struct Fabs : public Operation
{
  Fabs( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "fabs"; }
  void execute( Arch& arch ) const {
    f64_t val = arch.fread( 0 );
    if (mkbool( val < f64_t( 0.0 ) ))
      val = -val;
    arch.fwrite( 0, val );
  }
};

template <> Operation* decode<FABS>( CodeBase const& cb )
{
  // FABS - Absolute Value
  if (auto _ = match( cb, opcode( "\xd9\xe1" ) ))
  
    return new Fabs( _.opbase() );
  
  return 0;
}

template <unsigned OPSIZE>
struct Fadd : public Operation
{
  Fadd( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  void disasm( std::ostream& sink ) const { sink << "fadd" << ((OPSIZE==32) ? "s " : "l ") << DisasmM( rmop ); }
  void execute( Arch& arch ) const { arch.fwrite( 0, arch.fread( 0 ) + f64_t( arch.frmread<OPSIZE>( rmop ) ) ); }
};

template <bool P>
struct FaddReg : public Operation
{
  FaddReg( OpBase const& opbase, uint8_t _src, uint8_t _dst ) : Operation( opbase ), src( _src ), dst( _dst ) {} uint8_t src, dst;
  void disasm( std::ostream& sink ) const { sink << "fadd" << (P ? "p " : " ") << DisasmFPR(src) << ',' << DisasmFPR(dst); }
  void execute( Arch& arch ) const { arch.fwrite( dst, arch.fread( dst ) + arch.fread( src ) ); if (P) arch.fpop(); }
};

template <unsigned OPSIZE>
struct Fiadd : public Operation
{
  Fiadd( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  void disasm( std::ostream& sink ) const { sink << "fiadd" << ((OPSIZE==32) ? "l " : " ") << DisasmM( rmop ); }
  typedef typename TypeFor<OPSIZE>::s s_type;
  void execute( Arch& arch ) const { arch.fwrite( 0, arch.fread( 0 ) + f64_t( s_type( arch.rmread<OPSIZE>( rmop ) ) ) ); }
};

template <> Operation* decode<FADD>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xd8", 0 ) & RM_mem() ))
  
    return new Fadd<32>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xdc", 0 ) & RM_mem() ))
  
    return new Fadd<64>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xd8", 0 ) & RM_reg() ))
  
    return new FaddReg<false>( _.opbase(), _.ereg(), 0 );
  
  if (auto _ = match( cb, opcode( "\xdc", 0 ) & RM_reg() ))
  
    return new FaddReg<false>( _.opbase(), 0, _.ereg() );

  if (auto _ = match( cb, opcode( "\xde", 0 ) & RM_reg() ))
  
    return new FaddReg<true>( _.opbase(), 0, _.ereg() );
  
  if (auto _ = match( cb, opcode( "\xda", 0 ) & RM_mem() ))
  
    return new Fiadd<32>( _.opbase(), _.rmop() ); 
  
  if (auto _ = match( cb, opcode( "\xde", 0 ) & RM_mem() ))
  
    return new Fiadd<16>( _.opbase(), _.rmop() );
  
  return 0;
}

struct Fbld : public Operation
{
  Fbld( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  void disasm( std::ostream& sink ) const { sink << "fbld " << DisasmM( rmop ); }
};

template <> Operation* decode<FBLD>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xdf", 4 ) & RM_mem() ))
  
    return new Fbld( _.opbase(), _.rmop() );
  
  return 0;
}

struct Fbstp_m80 : public Operation
{
  Fbstp_m80( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  void disasm( std::ostream& sink ) const { sink << "fbstp " << DisasmM( rmop ); }
};

template <> Operation* decode<FBSTP>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xdf", 6 ) & RM_mem() ))
  
    return new Fbstp_m80( _.opbase(), _.rmop() );
  
  return 0;
}

struct Fchs : public Operation
{
  Fchs( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "fchs"; }
  void execute( Arch& arch ) const { arch.fwrite( 0, - arch.fread( 0 ) ); }
};

template <> Operation* decode<FCHS>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xd9\xe0" ) ))
  
    return new Fchs( _.opbase() );
  
  return 0;
}

struct Fnclex : public Operation
{
  Fnclex( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "fnclex"; }
};

template <> Operation* decode<FCLEX>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xdb\xe2" ) ))
  
    return new Fnclex( _.opbase() );

  return 0;
}

template <unsigned COND>
struct Fcmovcc : public Operation
{
  Fcmovcc( OpBase const& opbase, uint8_t _stidx ) : Operation( opbase ), stidx( _stidx ) {} uint8_t stidx;
  void disasm( std::ostream& sink ) const { sink << "fcmov" << (COND & 1 ? "n" : "") << (&"b\0\0e\0\0be\0u"[3*(COND >> 1)]) << " %st(" << unsigned(stidx) << "),%st";}
  void execute( Arch& arch ) const
  {
    bit_t ok( false );
    switch (COND) {
    case 0: ok = arch.flagread( CF ); break; // fcmovb
    case 1: ok = not arch.flagread( CF ); break; // fcmovnb
    case 2: ok = arch.flagread( ZF ); break; // fcmove
    case 3: ok = not arch.flagread( ZF ); break; // fcmovne
    case 4: ok = arch.flagread( CF ) or arch.flagread( ZF ); break; // fcmovbe
    case 5: ok = not arch.flagread( CF ) and not arch.flagread( ZF ); break; // fcmovnbe
    case 6: ok = arch.flagread( PF ); break; // fcmovu
    case 7: ok = not arch.flagread( PF ); break; // fcmovnu
    }
    if (mkbool( ok ))
      arch.fwrite( 0, arch.fread( stidx ) );
  }
};

template <> Operation* decode<FCMOV>( CodeBase const& cb )
{
  // FCMOVcc -- Floating-Point Conditional Move
  
  if (auto _ = match( cb, opcode( "\xda", 0 ) & RM_reg() ))
  
    return new Fcmovcc<0>( _.opbase(), _.ereg() );
  
  if (auto _ = match( cb, opcode( "\xda", 1 ) & RM_reg() ))
  
    return new Fcmovcc<2>( _.opbase(), _.ereg() );
  
  if (auto _ = match( cb, opcode( "\xda", 2 ) & RM_reg() ))
  
    return new Fcmovcc<4>( _.opbase(), _.ereg() );
  
  if (auto _ = match( cb, opcode( "\xda", 3 ) & RM_reg() ))
  
    return new Fcmovcc<6>( _.opbase(), _.ereg() );
  
  if (auto _ = match( cb, opcode( "\xdb", 0 ) & RM_reg() ))
  
    return new Fcmovcc<1>( _.opbase(), _.ereg() );
  
  if (auto _ = match( cb, opcode( "\xdb", 1 ) & RM_reg() ))
  
    return new Fcmovcc<3>( _.opbase(), _.ereg() );
  
  if (auto _ = match( cb, opcode( "\xdb", 2 ) & RM_reg() ))
  
    return new Fcmovcc<5>( _.opbase(), _.ereg() );
  
  if (auto _ = match( cb, opcode( "\xdb", 3 ) & RM_reg() ))
  
    return new Fcmovcc<7>( _.opbase(), _.ereg() );
  
  return 0;
}

template <bool P>
struct Fcom_m32 : public Operation
{
  Fcom_m32( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  void disasm( std::ostream& sink ) const { sink << "fcom" << (P?"p":"") << "s " << DisasmM( rmop ); }
  void execute( Arch& arch ) const
  {
    f64_t a = arch.fread( 0 );
    f64_t b = f64_t( arch.frmread<32>( rmop ) );
    bit_t notle = not (a <= b);
    bit_t notge = not (a >= b);
    
    arch.flagwrite( C3, notge == notle );
    arch.flagwrite( C2, notge and notle );
    arch.flagwrite( C0, notge );
    if (P) arch.fpop();
  }
};

template <bool P>
struct Fcom_m64 : public Operation
{
  Fcom_m64( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  void disasm( std::ostream& sink ) const { sink << "fcom" << (P?"p":"") << "l " << DisasmM( rmop ); }
  void execute( Arch& arch ) const
  {
    f64_t a = arch.fread( 0 );
    f64_t b = arch.frmread<64>( rmop );
    bit_t notle = not (a <= b);
    bit_t notge = not (a >= b);

    arch.flagwrite( C3, notge == notle );
    arch.flagwrite( C2, notge and notle );
    arch.flagwrite( C0, notge );
    if (P) arch.fpop();
  }
};

template <bool P>
struct Fcom_stn_st0 : public Operation
{
  Fcom_stn_st0( OpBase const& opbase, uint8_t _stidx ) : Operation( opbase ), stidx( _stidx ) {} uint8_t stidx;
  void disasm( std::ostream& sink ) const { sink << "fcom" << (P?"p":"") << " %st(" << unsigned(stidx) << ")"; }
  void execute( Arch& arch ) const {
    f64_t a = arch.fread( 0 );
    f64_t b = arch.fread( stidx );
    bit_t notle = not (a <= b);
    bit_t notge = not (a >= b);

    arch.flagwrite( C3, notge == notle );
    arch.flagwrite( C2, notge and notle );
    arch.flagwrite( C0, notge );
    if (P) arch.fpop();
  }
};

struct Fcompp : public Operation
{
  Fcompp( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "fcompp"; }
  void execute( Arch& arch ) const {
    f64_t a = arch.fpop();
    f64_t b = arch.fpop();
    bit_t notle = not (a <= b);
    bit_t notge = not (a >= b);

    arch.flagwrite( C3, notge == notle );
    arch.flagwrite( C2, notge and notle );
    arch.flagwrite( C0, notge );
  }
};

template <> Operation* decode<FCOM>( CodeBase const& cb )
{
  // FCOM/FCOMP/FCOMPP -- Compare Floating Point Values

  if (auto _ = match( cb, opcode( "\xd8", 2 ) & RM_mem() ))
  
    return new Fcom_m32<false>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xdc", 2 ) & RM_mem() ))
  
    return new Fcom_m64<false>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xd8", 2 ) & RM_reg() ))
  
    return new Fcom_stn_st0<false>( _.opbase(), _.ereg() );
  
  if (auto _ = match( cb, opcode( "\xd8", 3 ) & RM_mem() ))

    return new Fcom_m32<true>( _.opbase(), _.rmop() );

  if (auto _ = match( cb, opcode( "\xdc", 3 ) & RM_mem() ))
  
    return new Fcom_m64<true>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xd8", 3 ) & RM_reg() ))

    return new Fcom_stn_st0<true>( _.opbase(), _.ereg() );
  
  if (auto _ = match( cb, opcode( "\xde\xd9" ) ))
  
    return new Fcompp( _.opbase() );
  
  return 0;
}

template <bool P>
struct Fcomi_st0_stn : public Operation
{
  Fcomi_st0_stn( OpBase const& opbase, uint8_t _stidx ) : Operation( opbase ), stidx( _stidx ) {} uint8_t stidx;
  void disasm( std::ostream& sink ) const { sink << "fcomi" << (P?"p":"") << " %st(" << unsigned(stidx) << "),%st"; }
  void execute( Arch& arch ) const {
    f64_t a = arch.fread( 0 );
    f64_t b = arch.fread( stidx );
    bit_t notle = not (a <= b);
    bit_t notge = not (a >= b);

    arch.flagwrite( ZF, notge == notle );
    arch.flagwrite( PF, notge and notle );
    arch.flagwrite( CF, notge );
    if (P) arch.fpop();
  }
};

template <bool P>
struct Fucomi_st0_stn : public Operation
{
  Fucomi_st0_stn( OpBase const& opbase, uint8_t _stidx ) : Operation( opbase ), stidx( _stidx ) {} uint8_t stidx;
  void disasm( std::ostream& sink ) const { sink << "fucomi" << (P?"p":"") << " %st(" << unsigned(stidx) << "),%st"; }
  void execute( Arch& arch ) const {
    f64_t a = arch.fread( 0 );
    f64_t b = arch.fread( stidx );
    bit_t notle = not (a <= b);
    bit_t notge = not (a >= b);

    arch.flagwrite( ZF, notge == notle );
    arch.flagwrite( PF, notge and notle );
    arch.flagwrite( CF, notge );
    if (P) arch.fpop();
  }
};

template <> Operation* decode<FCOMI>( CodeBase const& cb )
{
  // FCOMI/FCOMIP/FUCOMI/FUCOMIP -- Compare Floating Point Values and Set EFLAGS

  if (auto _ = match( cb, opcode( "\xdb", 6 ) & RM_reg() ))
  
    return new Fcomi_st0_stn<false>( _.opbase(), _.ereg() );
  
  if (auto _ = match( cb, opcode( "\xdf", 6 ) & RM_reg() ))
  
    return new Fcomi_st0_stn<true>( _.opbase(), _.ereg() );
  
  if (auto _ = match( cb, opcode( "\xdb", 5 ) & RM_reg() ))
  
    return new Fucomi_st0_stn<false>( _.opbase(), _.ereg() );
  
  if (auto _ = match( cb, opcode( "\xdf", 5 ) & RM_reg() ))
  
    return new Fucomi_st0_stn<true>( _.opbase(), _.ereg() );
  
  return 0;
}

struct Fdecstp : public Operation
{
  Fdecstp( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "fdecstp"; }
};

struct Fincstp : public Operation
{
  Fincstp( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "fincstp"; }
};

template <> Operation* decode<FINCDECSTP>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xd9\xf6" ) ))

    return new Fdecstp( _.opbase() );
  
  if (auto _ = match( cb, opcode( "\xd9\xf7" ) ))

    return new Fincstp( _.opbase() );
  
  return 0;
}

template <unsigned OPSIZE>
struct Fdiv : public Operation
{
  Fdiv( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  void disasm( std::ostream& sink ) const { sink << "fdiv" << ((OPSIZE==32) ? "s " : "l ") << DisasmM( rmop ); }
  // void execute( Arch& arch ) const { arch.fwrite( 0, arch.fread( 0 ) / f64_t( arch.frmread<OPSIZE>( rmop ) ) ); }
};

template <bool P>
struct FdivReg : public Operation
{
  FdivReg( OpBase const& opbase, uint8_t _src, uint8_t _dst ) : Operation( opbase ), src( _src ), dst( _dst ) {} uint8_t src, dst;
  void disasm( std::ostream& sink ) const { sink << "fdiv" << (P ? "p " : " ") << DisasmFPR(src) << ',' << DisasmFPR(dst); }
  void execute( Arch& arch ) const { arch.fwrite( dst, arch.fread( dst ) / arch.fread( src ) ); if (P) arch.fpop(); }
};

template <unsigned OPSIZE>
struct Fidiv : public Operation
{
  Fidiv( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  void disasm( std::ostream& sink ) const { sink << "fidiv " << DisasmM( rmop ); }
  typedef typename TypeFor<OPSIZE>::s s_type;
  void execute( Arch& arch ) const { arch.fwrite( 0, arch.fread( 0 ) / f64_t( s_type( arch.rmread<OPSIZE>( rmop ) ) ) ); }
};

template <> Operation* decode<FDIV>( CodeBase const& cb )
{
  // FDIV/FDIVP/FIDIV -- Floating-Point Divide
 
  if (auto _ = match( cb,  opcode( "\xd8", 6 ) & RM_mem() ))
  
    return new Fdiv<32>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb,  opcode( "\xdc", 6 ) & RM_mem() ))
  
    return new Fdiv<64>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xd8", 6 ) & RM_reg() ))
  
    return new FdivReg<false>( _.opbase(), _.ereg(), 0 );
  
  if (auto _ = match( cb, opcode( "\xdc", 7 ) & RM_reg() ))
  
    return new FdivReg<false>( _.opbase(), 0, _.ereg() );
  
  if (auto _ = match( cb, opcode( "\xde", 7 ) & RM_reg() ))
  
    return new FdivReg<true>( _.opbase(), 0, _.ereg() );
  
  if (auto _ = match( cb, opcode( "\xda", 6 ) & RM_mem() ))
  
    return new Fidiv<16>( _.opbase(), _.rmop() );

  if (auto _ = match( cb, opcode( "\xde", 6 ) & RM_mem() ))
  
    return new Fidiv<32>( _.opbase(), _.rmop() );
  
  return 0;
}

template <unsigned OPSIZE>
struct Fdivr : public Operation
{
  Fdivr( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  void disasm( std::ostream& sink ) const { sink << "fdivr" << ((OPSIZE==32) ? "s " : "l ") << DisasmM( rmop ); }
  void execute( Arch& arch ) const { arch.fwrite( 0, f64_t( arch.frmread<OPSIZE>( rmop ) ) / arch.fread( 0 ) ); }
};

template <bool P>
struct FdivrReg : public Operation
{
  FdivrReg( OpBase const& opbase, uint8_t _src, uint8_t _dst ) : Operation( opbase ), src( _src ), dst( _dst ) {} uint8_t src, dst;
  void disasm( std::ostream& sink ) const { sink << "fdivr" << (P ? "p " : " ") << DisasmFPR(src) << ',' << DisasmFPR(dst); }
  void execute( Arch& arch ) const { arch.fwrite( dst, arch.fread( src ) / arch.fread( dst ) ); if (P) arch.fpop(); }
};

template <unsigned OPSIZE>
struct Fidivr : public Operation
{
  Fidivr( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  void disasm( std::ostream& sink ) const { sink << "fidivr" << ((OPSIZE==32) ? "l " : " ") << DisasmM( rmop ); }
  typedef typename TypeFor<OPSIZE>::s s_type;
  void execute( Arch& arch ) const { arch.fwrite( 0, f64_t( s_type( arch.rmread<OPSIZE>( rmop ) ) ) / arch.fread( 0 ) ); }
};

template <> Operation* decode<FDIVR>( CodeBase const& cb ) 
{
  // FDIVR/FDIVRP/FIDIVR -- Reverse Divide
  
  if (auto _ = match( cb, opcode( "\xd8", 7 ) & RM_mem() ))
  
    return new Fdivr<32>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xdc", 7 ) & RM_mem() ))
  
    return new Fdivr<64>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xd8", 7 ) & RM_reg() ))

    return new FdivrReg<false>( _.opbase(), _.ereg(), 0 );
  
  if (auto _ = match( cb, opcode( "\xdc", 6 ) & RM_reg() ))
  
    return new FdivrReg<false>( _.opbase(), 0, _.ereg() );

  if (auto _ = match( cb, opcode( "\xde", 6 ) & RM_reg() ))
  
    return new FdivrReg<true>( _.opbase(), 0, _.ereg() );
  
  if (auto _ = match( cb, opcode( "\xda", 7 ) & RM_mem() ))
  
    return new Fidivr<32>( _.opbase(), _.rmop() );

  if (auto _ = match( cb, opcode( "\xde", 7 ) & RM_mem() ))
  
    return new Fidivr<16>( _.opbase(), _.rmop() );
  
  return 0;
}

struct Ffree : public Operation
{
  Ffree( OpBase const& opbase, uint8_t _stidx ) : Operation( opbase ), stidx( _stidx ) {} uint8_t stidx;
  void disasm( std::ostream& sink ) const { sink << "ffree %st(" << unsigned(stidx) << ")"; }
};

template <> Operation* decode<FFREE>( CodeBase const& cb )
{
  // FFREE -- Free Floating-Point Register
  
  if (auto _ = match( cb, opcode( "\xdd", 0 ) & RM_reg() ))
  
    return new Ffree( _.opbase(), _.ereg() );
  
  return 0;
}

template <bool P>
struct Ficom_m32 : public Operation
{
  Ficom_m32( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  void disasm( std::ostream& sink ) const { sink << "ficom" << (P?"p":"") << "l " << DisasmEd( rmop ); }
};

template <bool P>
struct Ficom_m16 : public Operation
{
  Ficom_m16( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  void disasm( std::ostream& sink ) const { sink << "ficom" << (P?"p":"") << " " << DisasmM( rmop ); }
};

template <> Operation* decode<FICOM>( CodeBase const& cb )
{
  // FICOM/FICOMP -- Compare Integer
  
  if (auto _ = match( cb, opcode( "\xde", 2 ) & RM_mem() ))
  
    return new Ficom_m16<false>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xda", 2 ) & RM_mem() ))
  
    return new Ficom_m32<false>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xde", 3 ) & RM_mem() ))
  
    return new Ficom_m16<true>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xda", 3 ) & RM_mem() ))
  
    return new Ficom_m32<true>( _.opbase(), _.rmop() );
  
  return 0;
}

template <unsigned OPSIZE>
struct Fild : public Operation
{
  Fild( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  
  void disasm( std::ostream& sink ) const { sink << "fild" << (&"ll "[2-SB<OPSIZE/16>::begin]) << DisasmM( rmop ); }
  void execute( Arch& arch ) const { arch.fpush( f64_t( typename TypeFor<OPSIZE>::s( arch.rmread<OPSIZE>( rmop ) ) ) ); }
};

template <> Operation* decode<FILD>( CodeBase const& cb )
{
  // FILD -- Load Integer
  
  if (auto _ = match( cb, opcode( "\xdf", 0 ) & RM_mem() ))
  
    return new Fild<16>( _.opbase(), _.rmop() );

  if (auto _ = match( cb, opcode( "\xdb", 0 ) & RM_mem() ))
  
    return new Fild<32>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xdf", 5 ) & RM_mem() ))

    return new Fild<64>( _.opbase(), _.rmop() );
  
  return 0;
}

struct Fninit : public Operation
{
  Fninit( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "fninit"; }
};

template <> Operation* decode<FINIT>( CodeBase const& cb )
{
  // FINIT/FNINIT -- Initialize Floating-Point Unit
  
  if (auto _ = match( cb, opcode( "\xdb\xe3" ) ))
  
    return new Fninit( _.opbase() );
  
  return 0;
}

template <unsigned OPSIZE, bool P>
struct Fist : public Operation
{
  Fist( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  void disasm( std::ostream& sink ) const { sink << "fist" << (&"ll "[2-SB<OPSIZE/16>::begin]) << DisasmM( rmop ); }
  typedef typename TypeFor<OPSIZE>::s s_type;
  typedef typename TypeFor<OPSIZE>::u u_type;
  // void execute( Arch& arch ) const { rmop->write_e<OPSIZE>( arch, u_type( s_type( firound( arch.fread( 0 ), arch.fcwreadRC() ) ) ) ); if (P) arch.fpop(); }
};

template <> Operation* decode<FIST>( CodeBase const& cb )
{
  // FIST/FISTP -- Store Integer
  
  if (auto _ = match( cb, opcode( "\xdf", 2 ) & RM_mem() ))
  
    return new Fist<16,false>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xdb", 2 ) & RM_mem() ))
  
    return new Fist<32,false>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xdf", 3 ) & RM_mem() ))
  
    return new Fist<16,true>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xdb", 3 ) & RM_mem() ))
  
    return new Fist<32,true>( _.opbase(), _.rmop() );   
  
  if (auto _ = match( cb, opcode( "\xdf", 7 ) & RM_mem() ))
  
    return new Fist<64,true>( _.opbase(), _.rmop() );
  
  return 0;
}

template <unsigned OPSIZE>
struct Fisttp : public Operation
{
  Fisttp( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  void disasm( std::ostream& sink ) const { sink << "fisttp" << (&"ll "[2-SB<OPSIZE/16>::begin]) << DisasmM( rmop ); }
};

template <> Operation* decode<FISTTP>( CodeBase const& cb )
{
  // FISTTP -- Store Integer with Truncation
  
  if (auto _ = match( cb, opcode( "\xdf", 1 ) & RM_mem() ))
  
    return new Fisttp<16>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xdb", 1 ) & RM_mem() ))
  
    return new Fisttp<32>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xdd", 1 ) & RM_mem() ))
  
    return new Fisttp<64>( _.opbase(), _.rmop() );
  
  return 0;
}

template <unsigned OPSIZE>
struct Fld : public Operation
{
  Fld( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  void disasm( std::ostream& sink ) const { sink << "fld" << ((OPSIZE==32) ? "s " : (OPSIZE==64) ? "l " : "t ") << DisasmM( rmop ); }
  void execute( Arch& arch ) const { arch.fpush( f64_t( arch.frmread<OPSIZE>( rmop ) ) ); }
};

struct Fld_stn : public Operation
{
  Fld_stn( OpBase const& opbase, uint8_t _stidx ) : Operation( opbase ), stidx( _stidx ) {} uint8_t stidx;
  void disasm( std::ostream& sink ) const { sink << "fld %st(" << unsigned(stidx) << ")"; }
  void execute( Arch& arch ) const { arch.fpush( arch.fread( stidx ) ); }
};

template <> Operation* decode<FLD>( CodeBase const& cb )
{
  // FLD -- Load Floating Point Value

  if (auto _ = match( cb, opcode( "\xd9", 0 ) & RM_mem() ))
  
    return new Fld<32>( _.opbase(), _.rmop() );

  if (auto _ = match( cb, opcode( "\xdd", 0 ) & RM_mem() ))

    return new Fld<64>( _.opbase(), _.rmop() );

  if (auto _ = match( cb, opcode( "\xdb", 5 ) & RM_mem() ))

    return new Fld<80>( _.opbase(), _.rmop() );

  if (auto _ = match( cb, opcode( "\xd9", 0 ) & RM_reg() ))
  
    return new Fld_stn( _.opbase(), _.ereg() );
 
  return 0;
}

enum fldconst { FLD1 = 0, FLDL2T, FLDL2E, FLDPI, FLDLG2, FLDLN2, FLDZ };

template <fldconst FLDCONST>
struct FldConst : public Operation
{
  FldConst( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const
  {
    switch (FLDCONST) {
    default: throw 0;
    case FLD1:   sink << "fld1";   break;
    case FLDL2T: sink << "fldl2t"; break;
    case FLDL2E: sink << "fldl2e"; break;
    case FLDPI:  sink << "fldpi";  break;
    case FLDLG2: sink << "fldlg2"; break;
    case FLDLN2: sink << "fldln2"; break;
    case FLDZ:   sink << "fldz";   break;
    }
  }
  void execute( Arch& arch ) const
  {
    switch (FLDCONST) {
    default: throw 0;
    case FLD1:   arch.fpush( f64_t( 1.0 ) );                    break;
    case FLDL2T: throw 0;                                       break;
    case FLDL2E: arch.fpush( f64_t( 1.4426950408889634 ) );     break;
    case FLDPI:  arch.fpush( f64_t( 3.14159265358979323846 ) ); break;
    case FLDLG2: throw 0;                                       break;
    case FLDLN2: arch.fpush( f64_t( 0.69314718055994530941 ) ); break;
    case FLDZ:   arch.fpush( f64_t( 0.0 ) );                    break;
    }
  }
};

template <> Operation* decode<FLDCONST>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xd9\xe8" ) ))
  
    return new FldConst<FLD1>( _.opbase() );
  
  if (auto _ = match( cb, opcode( "\xd9\xe9" ) ))
  
    return new FldConst<FLDL2T>( _.opbase() );
  
  if (auto _ = match( cb, opcode( "\xd9\xea" ) ))
  
    return new FldConst<FLDL2E>( _.opbase() );
  
  if (auto _ = match( cb, opcode( "\xd9\xeb" ) ))
  
    return new FldConst<FLDPI>( _.opbase() );
  
  if (auto _ = match( cb, opcode( "\xd9\xec" ) ))
  
    return new FldConst<FLDLG2>( _.opbase() );
  
  if (auto _ = match( cb, opcode( "\xd9\xed" ) ))
  
    return new FldConst<FLDLN2>( _.opbase() );
  
  if (auto _ = match( cb, opcode( "\xd9\xee" ) ))
  
    return new FldConst<FLDZ>( _.opbase() );
  
  return 0;
}

struct FldCW : public Operation
{
  FldCW( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  void disasm( std::ostream& sink ) const { sink << "fldcw " << DisasmM( rmop ); }
  void execute( Arch& arch ) const { arch.fcwwrite( arch.rmread<16>( rmop ) ); }
};

struct FstCW : public Operation
{
  FstCW( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  void disasm( std::ostream& sink ) const { sink << "fnstcw " << DisasmEd( rmop ); }
  void execute( Arch& arch ) const { arch.rmwrite<16>( rmop, arch.fcwread() ); }
};

template <> Operation* decode<FLSCW>( CodeBase const& cb )
{
  // FLDCW/FSTCW/FNSTCW -- Load/Store x87 FPU Control Word
  
  if (auto _ = match( cb, opcode( "\xd9", 5 ) & RM_mem() ))
  
    return new FldCW( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xd9", 7 ) & RM_mem() ))
  
    return new FstCW( _.opbase(), _.rmop() );
  
  return 0;
}

struct Fldenv : public Operation
{
  Fldenv( OpBase const& opbase, MOp* _rmop, bool _o16 ) : Operation( opbase ), rmop( _rmop ), o16( _o16 ) {} RMOp rmop; bool o16;
  void disasm( std::ostream& sink ) const { sink << "fldenv" << (o16 ? "s " : " ") << DisasmM( rmop ); }
};

struct Fstenv : public Operation
{
  Fstenv( OpBase const& opbase, MOp* _rmop, bool _o16 ) : Operation( opbase ), rmop( _rmop ), o16( _o16 ) {} RMOp rmop; bool o16;
  void disasm( std::ostream& sink ) const { sink << "fnstenvs " << DisasmM( rmop ); }
};

template <> Operation* decode<FLSENV>( CodeBase const& cb )
{
  // FLDENV/FSTENV/FNSTENV -- Load/Store x87 FPU Environment
  
  if (auto _ = match( cb, opcode( "\xd9", 4 ) & RM_mem() ))
  
    return new Fldenv( _.opbase(), _.rmop(), cb.opsize() == 16 );
  
  if (auto _ = match( cb, opcode( "\xd9", 6 ) & RM_mem() ))
  
    return new Fstenv( _.opbase(), _.rmop(), cb.opsize() == 16 );
  
  return 0;
}

template <unsigned OPSIZE>
struct Fmul : public Operation
{
  Fmul( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  void disasm( std::ostream& sink ) const { sink << "fmul" << ((OPSIZE==32) ? "s " : "l ") << DisasmM( rmop ); }
  void execute( Arch& arch ) const { arch.fpush( arch.fpop() * arch.frmread<OPSIZE>( rmop ) ); }
};

template <bool P>
struct FmulReg : public Operation
{
  FmulReg( OpBase const& opbase, uint8_t _src, uint8_t _dst ) : Operation( opbase ), src( _src ), dst( _dst ) {} uint8_t src, dst;
  void disasm( std::ostream& sink ) const { sink << "fmul" << (P ? "p " : " ") << DisasmFPR( src ) << ',' << DisasmFPR( dst ); }
  void execute( Arch& arch ) const { arch.fwrite( dst, arch.fread( dst ) * arch.fread( src ) ); if (P) arch.fpop(); }
};

template <unsigned OPSIZE>
struct Fimul : public Operation
{
  Fimul( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  void disasm( std::ostream& sink ) const { sink << "fimul " << DisasmM( rmop ); }
  void execute( Arch& arch ) const { arch.fwrite( 0, arch.fread( 0 ) * f64_t( typename TypeFor<OPSIZE>::s( arch.rmread<OPSIZE>( rmop ) ) ) ); }
};

template <> Operation* decode<FMUL>( CodeBase const& cb )
{
  // FMUL/FMULP/FIMUL -- Multiply
  
  if (auto _ = match( cb, opcode( "\xd8", 1 ) & RM_mem() ))
  
    return new Fmul<32>( _.opbase(), _.rmop() );

  if (auto _ = match( cb, opcode( "\xdc", 1 ) & RM_mem() ))
  
    return new Fmul<64>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xd8", 1 ) & RM_reg() ))
  
    return new FmulReg<false>( _.opbase(), _.ereg(), 0 );

  if (auto _ = match( cb, opcode( "\xdc", 1 ) & RM_reg() ))
  
    return new FmulReg<false>( _.opbase(), 0, _.ereg() );
  
  if (auto _ = match( cb, opcode( "\xde", 1 ) & RM_reg() ))

    return new FmulReg<true>( _.opbase(), 0, _.ereg() );
  
  if (auto _ = match( cb, opcode( "\xda", 1 ) & RM_mem() ))
  
    return new Fimul<32>( _.opbase(), _.rmop() );

  if (auto _ = match( cb, opcode( "\xde", 1 ) & RM_mem() ))

    return new Fimul<16>( _.opbase(), _.rmop() );

  return 0;
}

struct Fnop : public Operation
{
  Fnop( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "fnop"; }
};

template <> Operation* decode<FNOP>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xd9\xd0" ) ))
  
    return new Fnop( _.opbase() );
  
  return 0;
}
 
struct Fsin : public Operation
{
  Fsin( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "fsin"; }
  // void execute( Arch& arch ) const {
  //   f64_t angle = arch.fpop();
  //   arch.fpush( sine( angle ) );
  // }
};

struct Fcos : public Operation
{
  Fcos( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "fcos"; }
  // void execute( Arch& arch ) const {
  //   f64_t angle = arch.fpop();
  //   arch.fpush( cosine( angle ) );
  // }
};

struct Fsincos : public Operation
{
  Fsincos( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "fsincos"; }
  // void execute( Arch& arch ) const
  // {
  //   f64_t angle = arch.fpop();
  //   arch.fpush( sine( angle ) );
  //   arch.fpush( cosine( angle ) );
  // }
};

struct Fpatan : public Operation
{
  Fpatan( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "fpatan"; }
  // void execute( Arch& arch ) const
  // {
  //   f64_t y = arch.fread( 1 );
  //   f64_t x = arch.fread( 0 );
  //   f64_t r;
  //   if (mkbool( x == f64_t( 0. ) )) {
  //     if (mkbool( y < f64_t( 0. ) ))
  //       r = f64_t( -1.57079632679489661923 );
  //     else
  //       r = f64_t( +1.57079632679489661923 );
  //   } else {
  //     r = arctan( y/x );
  //     if (mkbool( x < f64_t( 0. ) )) {
  //       if (mkbool( y < f64_t( 0. ) ))
  //         r -= f64_t( 3.14159265358979323846 );
  //       else
  //         r += f64_t( 3.14159265358979323846 );
  //     }
  //   }
  //   arch.flagwrite( C0, bit_t( 0 ) );
  //   arch.fwrite( 1, r );
  //   arch.fpop();
  // }
};

struct Fptan : public Operation
{
  Fptan( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "fptan"; }
  // void execute( Arch& arch ) const
  // {
  //   arch.fwrite( 1, tangent( arch.fread( 0 ) ) );
  //   arch.fpush( f64_t( 1.0 ) );
  //   arch.flagwrite( C2, bit_t( 0 ) );
  // }
};

struct Fsqrt : public Operation
{
  Fsqrt( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "fsqrt"; }
  // void execute( Arch& arch ) const { arch.fwrite( 0, square_root( arch.fread( 0 ) ) ); }
};

struct Fyl2x : public Operation
{
  Fyl2x( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "fyl2x"; }
  // void execute( Arch& arch ) const
  // {
  //   f64_t y = arch.fread( 1 );
  //   f64_t x = arch.fread( 0 );

  //   f64_t r = y*logarithm( x );

  //   arch.fwrite( 1, r );
  //   arch.fpop();
  // }
};

struct Fyl2xp1 : public Operation
{
  Fyl2xp1( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "fyl2xp1"; }
};

template <> Operation* decode<FMATH>( CodeBase const& cb )
{
  // FSIN -- Sine
  if (auto _ = match( cb, opcode( "\xd9\xfe" ) ))
  
    return new Fsin( _.opbase() );
  
  // FCOS -- Opcode
  if (auto _ = match( cb, opcode( "\xd9\xff" ) ))
 
    return new Fcos( _.opbase() );
  
  // FSINCOS -- Sine and Cosine
  if (auto _ = match( cb, opcode( "\xd9\xfb" ) ))

    return new Fsincos( _.opbase() );
  
  // FPATAN -- Partial Arctangent
  if (auto _ = match( cb, opcode( "\xd9\xf3" ) ))
  
    return new Fpatan( _.opbase() );
  
  // FPTAN -- Partial Tangent
  if (auto _ = match( cb, opcode( "\xd9\xf2" ) ))
  
    return new Fptan( _.opbase() );
  
  // FSQRT -- Square Root
  if (auto _ = match( cb, opcode( "\xd9\xfa" ) ))
  
    return new Fsqrt( _.opbase() );
  
  // FYL2X -- Compute y ∗ log2x
  if (auto _ = match( cb, opcode( "\xd9\xf1" ) ))
  
    return new Fyl2x( _.opbase() );

  // FYL2XP1 -- Compute y ∗ log2(x +1)
  if (auto _ = match( cb, opcode( "\xd9\xf9" ) ))
  
    return new Fyl2xp1( _.opbase() );

  return 0;
}

struct Fprem1 : public Operation
{
  Fprem1( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "fprem1"; }
  void execute( Arch& arch ) const
  {
    f64_t dividend = arch.fread( 0 );
    f64_t modulus = arch.fread( 1 );
    f64_t const threshold = power( f64_t( 2. ), f64_t( 64. ) ); // should be 2**64
    if (mkbool( (modulus * threshold) > dividend )) {
      f64_t quotient = firound( dividend / modulus, intel::x87frnd_nearest );
      arch.fwrite( 0, fmodulo( dividend, modulus ) );
      u64_t uq = (mkbool( quotient < f64_t( 0.0 ) )) ? u64_t( -quotient ) : u64_t( quotient );
      arch.flagwrite( C2, bit_t( false ) );
      arch.flagwrite( C0, bit_t( (uq >> 2) & u64_t( 1 ) ) );
      arch.flagwrite( C3, bit_t( (uq >> 1) & u64_t( 1 ) ) );
      arch.flagwrite( C1, bit_t( (uq >> 0) & u64_t( 1 ) ) );
    } else {
      f64_t const step = power( f64_t( 2. ), f64_t( 32. ) ); // should be 2**32
      f64_t pmodulus = modulus;
      while (mkbool( (pmodulus *step) <= dividend )) pmodulus = pmodulus * f64_t( 2. );
      arch.fwrite( 0, fmodulo( dividend, pmodulus ) );
      arch.flagwrite( C2, bit_t( true ) );
    }
  }
};

struct Fprem : public Operation
{
  Fprem( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "fprem"; }
  void execute( Arch& arch ) const
  {
    f64_t dividend = arch.fread( 0 );
    f64_t modulus = arch.fread( 1 );
    f64_t const threshold = power( f64_t( 2. ), f64_t( 64. ) ); // should be 2**64
    if (mkbool( (modulus * threshold) > dividend )) {
      f64_t quotient = firound( dividend / modulus, intel::x87frnd_toward0 );
      arch.fwrite( 0, fmodulo( dividend, modulus ) );
      u64_t uq = (mkbool( quotient < f64_t( 0.0 ) )) ? u64_t( -quotient ) : u64_t( quotient );
      arch.flagwrite( C2, bit_t( false ) );
      arch.flagwrite( C0, bit_t( (uq >> 2) & u64_t( 1 ) ) );
      arch.flagwrite( C3, bit_t( (uq >> 1) & u64_t( 1 ) ) );
      arch.flagwrite( C1, bit_t( (uq >> 0) & u64_t( 1 ) ) );
    } else {
      f64_t const step = power( f64_t( 2. ), f64_t( 32. ) ); // should be 2**32
      f64_t pmodulus = modulus;
      while (mkbool( (pmodulus *step) <= dividend )) pmodulus = pmodulus * f64_t( 2. );
      arch.fwrite( 0, fmodulo( dividend, pmodulus ) );
      arch.flagwrite( C2, bit_t( true ) );
    }
  }
};

template <> Operation* decode<FPREM>( CodeBase const& cb )
{
  // FPREM -- Partial Remainder
  
  if (auto _ = match( cb, opcode( "\xd9\xf8" ) ))
  
    return new Fprem( _.opbase() );
  
  if (auto _ = match( cb, opcode( "\xd9\xf5" ) ))
  
    return new Fprem1( _.opbase() );
  
  return 0;
}

struct Frndint : public Operation
{
  Frndint( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "frndint"; }
  void execute( Arch& arch ) const {
    f64_t value = arch.fread( 0 );
    value = firound( value, intel::x87frnd_mode_t( arch.fcwreadRC() ) );
    arch.fwrite( 0, value );
  }
};

template <> Operation* decode<FRNDINT>( CodeBase const& cb )
{
  // FRNDINT -- Round to Integer
  
  if (auto _ = match( cb, opcode( "\xd9\xfc" ) ))
  
    return new Frndint( _.opbase() );
  
  return 0;
}

struct Frstor : public Operation
{
  Frstor( OpBase const& opbase, MOp* _rmop, bool _o16 ) : Operation( opbase ), rmop( _rmop ), o16( _o16 ) {} RMOp rmop; bool o16;
  void disasm( std::ostream& sink ) const { sink << "frstor" << (o16 ? "s " : " ") << DisasmM( rmop ); }
};

struct Fsave : public Operation
{
  Fsave( OpBase const& opbase, MOp* _rmop, bool _o16 ) : Operation( opbase ), rmop( _rmop ), o16( _o16 ) {} RMOp rmop; bool o16;
  void disasm( std::ostream& sink ) const { sink << "fnsave" << (o16 ? "s " : " ") << DisasmM( rmop ); }
};

template <> Operation* decode<FLSSTATE>( CodeBase const& cb )
{
  // FRSTOR/FSAVE/FNSAVE  Restore/Store x87 FPU State
  
  if (auto _ = match( cb, opcode( "\xdd", 4 ) & RM_mem() ))
  
    return new  Frstor( _.opbase(), _.rmop(), cb.opsize() == 16 );
  
  if (auto _ = match( cb, opcode( "\xdd", 6 ) & RM_mem() ))
  
    return new  Fsave( _.opbase(), _.rmop(), cb.opsize() == 16 );
  
  return 0;
}

struct Fscale : public Operation
{
  Fscale( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "fscale"; }
  void execute( Arch& arch ) const { arch.fwrite( 0, arch.fread( 0 ) * power( f64_t( 2 ), firound( arch.fread( 1 ), intel::x87frnd_toward0 ) ) ); }
};

struct Fxtract : public Operation
{
  Fxtract( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "fxtract"; }
};

template <> Operation* decode<FSCALE>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xd9\xfd" ) ))
  
    return new Fscale( _.opbase() );

  if (auto _ = match( cb, opcode( "\xd9\xf4" ) ))
  
    return new Fxtract( _.opbase() );
  
  return 0;
}

template <unsigned OPSIZE, bool P>
struct Fst : public Operation
{
  Fst( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  void disasm( std::ostream& sink ) const { sink << "fst" << (P?"p":"") << ((OPSIZE==32) ? "s " : (OPSIZE==64) ? "l " : "t ") << DisasmM( rmop ); }
  void execute( Arch& arch ) const { arch.frmwrite<OPSIZE>( rmop, typename TypeFor<OPSIZE>::f( arch.fread( 0 ) ) ); if (P) arch.fpop(); }
};

template <bool P>
struct Fst_stn_st0 : public Operation
{
  Fst_stn_st0( OpBase const& opbase, uint8_t _stidx ) : Operation( opbase ), stidx( _stidx ) {} uint8_t stidx;
  void disasm( std::ostream& sink ) const { sink << "fst" << (P?"p":"") << " %st(" << unsigned(stidx) << ")"; }
  void execute( Arch& arch ) const { arch.fwrite( stidx, arch.fread( 0 ) ); if (P) arch.fpop(); }
};

template <> Operation* decode<FST>( CodeBase const& cb )
{
  
  if (auto _ = match( cb, opcode( "\xd9", 2 ) & RM_mem() ))
  
    return new  Fst<32,false>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xdd", 2 ) & RM_mem() ))
  
    return new  Fst<64,false>( _.opbase(), _.rmop() );

  if (auto _ = match( cb, opcode( "\xdd", 2 ) & RM_reg() ))
  
    return new  Fst_stn_st0<false>( _.opbase(), _.ereg() );
  
  if (auto _ = match( cb, opcode( "\xd9", 3 ) & RM_mem() ))
  
    return new  Fst<32,true>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xdd", 3 ) & RM_mem() ))
  
    return new  Fst<64,true>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xdb", 7 ) & RM_mem() ))
  
    return new  Fst<80,true>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xdd", 3 ) & RM_reg() ))

    return new  Fst_stn_st0<true>( _.opbase(), _.ereg() );
  
  return 0;
} 

struct Fnstsw : public Operation
{
  Fnstsw( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  void disasm( std::ostream& sink ) const { sink << "fnstsw " << DisasmEd( rmop ); }
  void execute( Arch& arch ) const { arch.rmwrite<16>( rmop, fswread( arch ) ); }
};

struct Fnstsw_ax : public Operation
{
  Fnstsw_ax( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "fnstsw %ax"; }
  void execute( Arch& arch ) const { arch.regwrite<16>( 0, fswread( arch ) ); }
};

template <> Operation* decode<FSTSW>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xdd", 7 ) & RM_mem() ))
  
    return new Fnstsw( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xdf\xe0" ) ))
  
    return new Fnstsw_ax( _.opbase() );
  
  return 0;
}

template <unsigned OPSIZE>
struct Fsub : public Operation
{
  Fsub( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  void disasm( std::ostream& sink ) const { sink << "fsub" << ((OPSIZE==32) ? "s " : "l ") << DisasmM( rmop ); }
  void execute( Arch& arch ) const { arch.fwrite( 0, arch.fread( 0 ) - f64_t( arch.frmread<OPSIZE>( rmop ) ) ); }
};

template <bool P>
struct FsubReg : public Operation
{
  FsubReg( OpBase const& opbase, uint8_t _src, uint8_t _dst ) : Operation( opbase ), src( _src ), dst( _dst ) {} uint8_t src, dst;
  void disasm( std::ostream& sink ) const { sink << "fsub" << (P ? "p " : " ") << DisasmFPR( src ) << ',' << DisasmFPR( dst ); }
  void execute( Arch& arch ) const { arch.fwrite( dst, arch.fread( dst ) - arch.fread( src ) ); if (P) arch.fpop(); }
};

template <unsigned OPSIZE>
struct Fisub : public Operation
{
  Fisub( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  void disasm( std::ostream& sink ) const { sink << "fisub" << ((OPSIZE==32) ? "l " : " ") << DisasmM( rmop ); }
  typedef typename TypeFor<OPSIZE>::s s_type;
  void execute( Arch& arch ) const { arch.fwrite( 0, arch.fread( 0 ) - f64_t( s_type( arch.rmread<OPSIZE>( rmop ) ) ) ); }
};

template <> Operation* decode<FSUB>( CodeBase const& cb )
{
  // FSUB/FSUBP/FISUB -- Subtract
  if (auto _ = match( cb, opcode( "\xd8", 4 ) & RM_mem() ))
  
    return new Fsub<32>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xdc", 4 ) & RM_mem() ))
  
    return new Fsub<64>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xd8", 4 ) & RM_reg() ))
  
    return new FsubReg<false>( _.opbase(), _.ereg(), 0 );
  
  if (auto _ = match( cb, opcode( "\xdc", 5 ) & RM_reg() ))
  
    return new FsubReg<false>( _.opbase(), 0, _.ereg() );
  
  if (auto _ = match( cb, opcode( "\xde", 5 ) & RM_reg() ))
  
    return new FsubReg<true>( _.opbase(), 0, _.ereg() );

  if (auto _ = match( cb, opcode( "\xda", 4 ) & RM_mem() ))
  
    return new Fisub<32>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xde", 4 ) & RM_mem() ))
  
    return new Fisub<16>( _.opbase(), _.rmop() );
  
  return 0;
}

template <unsigned OPSIZE>
struct Fsubr : public Operation
{
  Fsubr( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  void disasm( std::ostream& sink ) const { sink << "fsubr" << ((OPSIZE==32) ? "s " : "l ") << DisasmM( rmop ); }
  void execute( Arch& arch ) const { arch.fwrite( 0, f64_t( arch.frmread<OPSIZE>( rmop ) ) - arch.fread( 0 ) ); }
};

template <bool P>
struct FsubrReg : public Operation
{
  FsubrReg( OpBase const& opbase, uint8_t _src, uint8_t _dst ) : Operation( opbase ), src( _src ), dst( _dst ) {} uint8_t src, dst;
  void disasm( std::ostream& sink ) const { sink << "fsubr" << (P ? "p " : " ") << DisasmFPR(src) << ',' << DisasmFPR(dst); }
  void execute( Arch& arch ) const { arch.fwrite( dst, arch.fread( src ) - arch.fread( dst ) ); if (P) arch.fpop(); }
};

template <unsigned OPSIZE>
struct Fisubr : public Operation
{
  Fisubr( OpBase const& opbase, MOp* _rmop ) : Operation( opbase ), rmop( _rmop ) {} RMOp rmop;
  void disasm( std::ostream& sink ) const { sink << "fisubr" << ((OPSIZE==32) ? "l " : " ") << DisasmM( rmop ); }
};

template <> Operation* decode<FSUBR>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xd8", 5 ) & RM_mem() ))
  
    return new Fsubr<32>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xdc", 5 ) & RM_mem() ))
  
    return new Fsubr<64>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xd8", 5 ) & RM_reg() ))
  
    return new FsubrReg<false>( _.opbase(), _.ereg(), 0 );
  
  if (auto _ = match( cb, opcode( "\xdc", 4 ) & RM_reg() ))
  
    return new FsubrReg<false>( _.opbase(), 0, _.ereg() );
  
  if (auto _ = match( cb, opcode( "\xde", 4 ) & RM_reg() ))
  
    return new FsubrReg<true>( _.opbase(), 0, _.ereg() );
  
  if (auto _ = match( cb, opcode( "\xda", 5 ) & RM_mem() ))
  
    return new Fisubr<32>( _.opbase(), _.rmop() );
  
  if (auto _ = match( cb, opcode( "\xde", 5 ) & RM_mem() ))
 
    return new Fisubr<16>( _.opbase(), _.rmop() );
  
  return 0;
}

struct Ftst : public Operation
{
  Ftst( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "ftst"; }
};

template <> Operation* decode<FTST>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xd9\xe4" ) ))
  
    return new Ftst( _.opbase() );
  
  return 0;
}

template <bool P>
struct Fucom_st0_stn : public Operation
{
  Fucom_st0_stn( OpBase const& opbase, uint8_t _stidx ) : Operation( opbase ), stidx( _stidx ) {} uint8_t stidx;
  void disasm( std::ostream& sink ) const { sink << "fucom" << (P?"p":"") << " %st(" << unsigned(stidx) << ")"; }
  // void execute( Arch& arch ) const {
  //   f64_t a = arch.fread( 0 );
  //   f64_t b = arch.fread( stidx );
  //   bit_t notle = not (a <= b);
  //   bit_t notge = not (a >= b);

  //   arch.flagwrite( C3, notge == notle );
  //   arch.flagwrite( C2, notge and notle );
  //   arch.flagwrite( C0, notge );
  //   if (P) arch.fpop();
  // }
};

struct Fucompp : public Operation
{
  Fucompp( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "fucompp"; }
  // void execute( Arch& arch ) const {
  //   f64_t a = arch.fpop();
  //   f64_t b = arch.fpop();
  //   bit_t notle = not (a <= b);
  //   bit_t notge = not (a >= b);

  //   arch.flagwrite( C3, notge == notle );
  //   arch.flagwrite( C2, notge and notle );
  //   arch.flagwrite( C0, notge );
  // }
};

template <> Operation* decode<FUCOM>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xdd", 4 ) & RM_reg() ))
  
    return new Fucom_st0_stn<false>( _.opbase(), _.ereg() );

  if (auto _ = match( cb, opcode( "\xdd", 5 ) & RM_reg() ))
  
    return new Fucom_st0_stn<true>( _.opbase(), _.ereg() );
  
  if (auto _ = match( cb, opcode( "\xda\xe9" ) ))
  
    return new Fucompp( _.opbase() );

  return 0;
}

struct Fxam : public Operation
{
  Fxam( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "fxam"; }
  void execute( Arch& arch ) const { arch.fxam(); }
};

template <> Operation* decode<FXAM>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xd9\xe5" ) ))
  
    return new Fxam( _.opbase() );
  
  return 0;
}

struct Fxch : public Operation
{
  Fxch( OpBase const& opbase, uint8_t _stidx ) : Operation( opbase ), stidx( _stidx ) {} uint8_t stidx;
  void disasm( std::ostream& sink ) const { sink << "fxch %st(" << unsigned(stidx) << ")"; }
  void execute( Arch& arch ) const
  {
    f64_t a = arch.fread( 0 );
    f64_t b = arch.fread( stidx );
    arch.fwrite( 0, b );
    arch.fwrite( stidx, a );
  }
};

template <> Operation* decode<FXCH>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xd9", 1 ) & RM_reg() ))
  
    return new Fxch( _.opbase(), _.ereg() );
  
  return 0;
}

struct Fwait : public Operation
{
  Fwait( OpBase const& opbase ) : Operation( opbase ) {}
  void disasm( std::ostream& sink ) const { sink << "fwait"; }
  void execute( Arch& arch ) const {}
};

template <> Operation* decode<FWAIT>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\x9b" ) ))
  
    return new Fwait( _.opbase() );
  
  return 0;
}

struct Fobsolete : public Operation
{
  Fobsolete( OpBase const& opbase, char const* _msg ) : Operation( opbase ), msg( _msg ) {} char const* msg;
  void disasm( std::ostream& sink ) const { sink << msg; }
};

template <> Operation* decode<FOBSOLETE>( CodeBase const& cb )
{
  if (auto _ = match( cb, opcode( "\xdb\xe0" ) )) return new Fobsolete( _.opbase(), "fneni(8087 only)" );
  if (auto _ = match( cb, opcode( "\xdb\xe1" ) )) return new Fobsolete( _.opbase(), "fndisi(8087 only)" );
  if (auto _ = match( cb, opcode( "\xdb\xe4" ) )) return new Fobsolete( _.opbase(), "fnsetpm(287 only)" );
  if (auto _ = match( cb, opcode( "\xdb\xe5" ) )) return new Fobsolete( _.opbase(), "frstpm(287 only)" );
  return 0;
}

