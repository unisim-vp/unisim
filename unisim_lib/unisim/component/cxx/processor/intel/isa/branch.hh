/* TODO:
 * - In 64 bit mode, for some branch instructions (if not all) operand-size is forced to 64 whatever the actual operand size is ...
 * - Some instructions may incoherently depend on StackAddressSize and OperandSize (e.g. Enter & Leave)
 * - some branch struct offsets should depend on operand size
 * - the dissambly of non-default targetting mode needs a suffix
 */
template <class ARCH, unsigned OPSIZE>
struct NearCallJ : public Operation<ARCH>
{
  typedef typename ARCH::u32_t u32_t;
  NearCallJ( OpBase<ARCH> const& opbase, int32_t _offset ) : Operation<ARCH>( opbase ), offset( _offset ) {} uint32_t offset;
    
  void disasm( std::ostream& sink ) const { sink << "call " << "0x" << std::hex << (Operation<ARCH>::address + Operation<ARCH>::length + offset); };
    
  void execute( ARCH& arch ) const
  {
    if (OPSIZE != 32) throw 0;
    arch.template push<32>( arch.geteip() );
    arch.seteip( arch.geteip() + u32_t( offset ), ARCH::ipcall );
  }
};
  
template <class ARCH, unsigned OPSIZE>
struct NearCallE : public Operation<ARCH>
{
  typedef typename ARCH::u32_t u32_t;
  NearCallE( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop ) : Operation<ARCH>( opbase ), rmop( _rmop ) {} RMOp<ARCH> rmop;
    
  void disasm( std::ostream& sink ) const { sink << "call *" << DisasmE( UI<OPSIZE>(), rmop ); }
    
  void execute( ARCH& arch ) const
  {
    if (OPSIZE != 32) throw 0;
    u32_t target = arch.template rmread<32>( rmop );
    arch.template push<32>( arch.geteip() );
    arch.seteip( target, ARCH::ipcall );
  }
};
  
template <class ARCH, unsigned OPSIZE>
struct FarCallA : public Operation<ARCH>
{
  FarCallA( OpBase<ARCH> const& opbase, int32_t _offset, int16_t _seg ) : Operation<ARCH>( opbase ), offset( _offset ), mseg( _seg ) {} int32_t offset; int16_t mseg;
    
  void disasm( std::ostream& sink ) const { sink << "lcall " << "0x" << std::hex << (Operation<ARCH>::address + Operation<ARCH>::length + offset); };
};
  
template <class ARCH, unsigned OPSIZE>
struct FarCallE : public Operation<ARCH>
{
  FarCallE( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop ) : Operation<ARCH>( opbase ), rmop( _rmop ) {} RMOp<ARCH> rmop;
  
  void disasm( std::ostream& sink ) const { sink << "lcall " << '*' << DisasmM( rmop ); }
};

template <class ARCH> struct DC<ARCH,CALL> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (ic.adsz_67 or (ic.rep != 0)) return 0;
    
  if (auto _ = match( ic, OpSize<16>() & opcode( "\xe8" ) & Imm<16>() ))
    
    return new NearCallJ<ARCH,16>( _.opbase(), _.i( int32_t() ) );
    
  if (auto _ = match( ic, OpSize<32>() & opcode( "\xe8" ) & Imm<32>() ))
    
    return new NearCallJ<ARCH,32>( _.opbase(), _.i( int32_t() ) );
    
  if (auto _ = match( ic, opcode( "\xff", 2 ) & RM() ))
    {
      if      (ic.opsize() == 16) return new NearCallE<ARCH,16>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 32) return new NearCallE<ARCH,32>( _.opbase(), _.rmop() );
      else return 0;
    }
    
  if (auto _ = match( ic, OpSize<16>() & opcode( "\x9a" ) & Imm<16>() & Imm<16>() ))
    
    return new FarCallA<ARCH,16>( _.opbase(), _.i( int32_t(), 0 ), _.i( int16_t(), 1 ) );
    
  if (auto _ = match( ic, OpSize<32>() & opcode( "\x9a" ) & Imm<32>() & Imm<16>() ))
    
    return new FarCallA<ARCH,32>( _.opbase(), _.i( int32_t(), 0 ), _.i( int16_t(), 1 ) );
    
  if (auto _ = match( ic, opcode( "\xff", 3 ) & RM() ))
    {
      if      (ic.opsize() == 16) return new FarCallE<ARCH,16>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 32) return new FarCallE<ARCH,32>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 64) return new FarCallE<ARCH,64>( _.opbase(), _.rmop() );
      else return 0;
    }
    
  return 0;
}};

template <class ARCH, unsigned OPSIZE>
struct JccJ : public Operation<ARCH>
{
  typedef typename ARCH::u32_t u32_t;
  JccJ( OpBase<ARCH> const& opbase, uint8_t _cond, int32_t _offset ) : Operation<ARCH>( opbase ), offset( _offset ), cond( _cond ) {} int32_t offset; uint8_t cond;
    
  void disasm( std::ostream& sink ) const { sink << 'j' << DisasmCond( cond ) << " 0x" << std::hex << (Operation<ARCH>::address + Operation<ARCH>::length + offset); }
    
  void execute( ARCH& arch ) const
  {
    if (OPSIZE != 32) throw 0;
    if (arch.Cond( eval_cond( arch, cond ) )) arch.seteip( arch.geteip() + u32_t( offset ) ); 
  }
};
  
template <class ARCH> struct DC<ARCH,JCC> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, VarByte<0x70,0xF0>() & Imm<8>() ))
    
    {
      if      (ic.opsize() == 16) return new JccJ<ARCH,16>( _.opbase(), _.vbval(), _.i( int32_t() ) );
      else if (ic.opsize() == 32) return new JccJ<ARCH,32>( _.opbase(), _.vbval(), _.i( int32_t() ) );
      else return 0;
    }
    
  if (auto _ = match( ic, OpSize<16>() & opcode( "\x0f" ) & VarByte<0x80,0xf0>() & Imm<16>() ))
    
    return new JccJ<ARCH,16>( _.opbase(), _.vbval(), _.i( int32_t() ) );
    
  if (auto _ = match( ic, OpSize<32>() & opcode( "\x0f" ) & VarByte<0x80,0xf0>() & Imm<32>() ))
    
    return new JccJ<ARCH,32>( _.opbase(), _.vbval(), _.i( int32_t() ) );
    
  return 0;
}};

template <class ARCH, unsigned MOD>
struct Loop : public Operation<ARCH>
{
  int32_t offset;
    
  Loop( OpBase<ARCH> const& opbase, int32_t _offset )
    : Operation<ARCH>( opbase ), offset( _offset )
  {}
    
  void disasm( std::ostream& sink ) const { sink << "loop" << (&"ne\0e\0\0"[MOD*3]) << " 0x" << std::hex << (Operation<ARCH>::address + Operation<ARCH>::length + offset); };
    
  void execute( ARCH& arch ) const
  {
    typedef typename ARCH::u32_t u32_t;
    typedef typename ARCH::bit_t bit_t;
    
    // Decrement count register
    u32_t count = arch.regread32( 1 ) - u32_t( 1 );
    arch.regwrite32( 1, count );
    // Stop if count is zero
    if (arch.Cond( count == u32_t(0) )) return;
    // or ZF is set (loopne)
    if ((MOD == 0) and arch.Cond(arch.flagread( ARCH::FLAG::ZF ) == bit_t( 1 ))) return;
    // or ZF is cleared (loope)
    if ((MOD == 1) and arch.Cond(arch.flagread( ARCH::FLAG::ZF ) == bit_t( 0 ))) return;
    // else jump short
    arch.seteip( arch.geteip() + u32_t( offset ) );
  }
};
  
template <class ARCH> struct DC<ARCH,LOOP> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\xe0" ) & Imm<8>() ))
    
    return new Loop<ARCH,0>( _.opbase(), _.i( int32_t() ) );
    
  if (auto _ = match( ic, opcode( "\xe1" ) & Imm<8>() ))
    
    return new Loop<ARCH,1>( _.opbase(), _.i( int32_t() ) );
    
  if (auto _ = match( ic, opcode( "\xe2" ) & Imm<8>() ))
    
    return new Loop<ARCH,2>( _.opbase(), _.i( int32_t() ) );
    
  return 0;
}};

template <class ARCH, unsigned ADDRSZ>
struct Jcxz : public Operation<ARCH>
{
  int32_t offset;
    
  Jcxz( OpBase<ARCH> const& opbase, int32_t _offset ) : Operation<ARCH>( opbase ), offset( _offset ) {}
    
  void disasm( std::ostream& sink ) const { sink << 'j' << (&"\0\0e\0\0\0r"[(ADDRSZ/16)-1]) << "cxz 0x" << std::hex << (Operation<ARCH>::address + Operation<ARCH>::length + offset); };
    
  void execute( ARCH& arch ) const
  {
    typedef typename ARCH::u32_t u32_t;
    
    if (ADDRSZ != 32) throw 0;
    if (arch.Cond( arch.regread32( 1 ) == u32_t( 0 ) ))
      arch.seteip( arch.geteip() + u32_t( offset ) );
  }
};
  
template <class ARCH> struct DC<ARCH,JCXZ> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, AddrSize<16>() & opcode( "\xe3" ) & Imm<8>() ))
    
    return new Jcxz<ARCH,16>( _.opbase(), _.i( int32_t() ) );
    
  if (auto _ = match( ic, AddrSize<32>() & opcode( "\xe3" ) & Imm<8>() ))
    
    return new Jcxz<ARCH,32>( _.opbase(), _.i( int32_t() ) );
    
  if (auto _ = match( ic, AddrSize<64>() & opcode( "\xe3" ) & Imm<8>() ))
    
    return new Jcxz<ARCH,64>( _.opbase(), _.i( int32_t() ) );
    
  return 0;
}};
  
template <class ARCH, unsigned OPSIZE>
struct JmpE : public Operation<ARCH>
{
  JmpE( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop ) : Operation<ARCH>( opbase ), rmop( _rmop ) {} RMOp<ARCH> rmop;
    
  void disasm( std::ostream& sink ) const { sink << "jmp " << '*' << DisasmE( UI<OPSIZE>(), rmop ); }
     
  void execute( ARCH& arch ) const { if (OPSIZE != 32) throw 0; arch.seteip( arch.template rmread<32>( rmop ) ); }
};

template <class ARCH, unsigned OPSIZE>
struct JmpJ : public Operation<ARCH>
{
  typedef typename ARCH::u32_t u32_t;
  JmpJ( OpBase<ARCH> const& opbase, int32_t _offset ) : Operation<ARCH>( opbase ), offset( _offset ) {} int32_t offset;
  
  void disasm( std::ostream& sink ) const { sink << "jmp 0x" << std::hex << (Operation<ARCH>::address + Operation<ARCH>::length + offset); }
     
  void execute( ARCH& arch ) const { if (OPSIZE != 32) throw 0; arch.seteip( arch.geteip() + u32_t( offset ) ); }
};
  
template <class ARCH, unsigned OPSIZE>
struct JmpA : public Operation<ARCH>
{
  JmpA( OpBase<ARCH> const& opbase, int32_t _offset, int16_t _seg ) : Operation<ARCH>( opbase ), offset( _offset ), mseg( _seg ) {} int32_t offset; int16_t mseg;
  
  void disasm( std::ostream& sink ) const { sink << "ljmp " << DisasmI( mseg ) << ',' << DisasmI( offset ); }
};
  
template <class ARCH, unsigned OPSIZE>
struct JmpM : public Operation<ARCH>
{
  JmpM( OpBase<ARCH> const& opbase, MOp<ARCH> const* _rmop ) : Operation<ARCH>( opbase ), rmop( _rmop ) {} RMOp<ARCH> rmop;
    
  void disasm( std::ostream& sink ) const { sink << "ljmp " << '*' << DisasmM( rmop ); }
};
  
template <class ARCH> struct DC<ARCH,JMP> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\xff", 4 ) & RM() ))
    
    {
      if      (ic.opsize() == 16) return new JmpE<ARCH,16>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 32) return new JmpE<ARCH,32>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 64) return new JmpE<ARCH,64>( _.opbase(), _.rmop() );
      else return 0;
    }
    
  if (auto _ = match( ic, opcode( "\xeb" ) & Imm<8>() ))
    
    {
      if      (ic.opsize() == 16) return new JmpJ<ARCH,16>( _.opbase(), _.i( int32_t() ) );
      else if (ic.opsize() == 32) return new JmpJ<ARCH,32>( _.opbase(), _.i( int32_t() ) );
      else return 0;
    }
    
  if (auto _ = match( ic, OpSize<16>() & opcode( "\xe9" ) & Imm<16>() ))
    
    return new JmpJ<ARCH,16>( _.opbase(), _.i( int32_t() ) );
    
  if (auto _ = match( ic, OpSize<32>() & opcode( "\xe9" ) & Imm<32>() ))
    
    return new JmpJ<ARCH,32>( _.opbase(), _.i( int32_t() ) );
    
  if (auto _ = match( ic, OpSize<16>() & opcode( "\xea" ) & Imm<16>() & Imm<16>() ))
    
    return new JmpA<ARCH,16>( _.opbase(), _.i( int32_t() ), _.i( int16_t() ) );
    
  if (auto _ = match( ic, OpSize<32>() & opcode( "\xea" ) & Imm<32>() & Imm<16>() ))
    
    return new JmpA<ARCH,32>( _.opbase(), _.i( int32_t() ), _.i( int16_t() ) );
    
  if (auto _ = match( ic, opcode( "\xff", 5 ) & RM() ))
    
    {
      if      (ic.opsize() == 16) return new JmpM<ARCH,16>( _.opbase(), _.rmop() );
      else if (ic.opsize() == 32) return new JmpM<ARCH,32>( _.opbase(), _.rmop() );
      else return 0;
    }
    
  return 0;
}};
  
template <class ARCH, unsigned OPSIZE>
struct NearReturn : public Operation<ARCH>
{
  typedef typename ARCH::u32_t u32_t;
  NearReturn( OpBase<ARCH> const& opbase ) : Operation<ARCH>( opbase ) {}
    
  void disasm( std::ostream& sink ) const { sink << "ret"; }

  void execute( ARCH& arch ) const
  {
    if (OPSIZE != 32) throw 0;
    u32_t src = arch.regread32( 4 );
    arch.regwrite32( 4, src + u32_t( 4 ) );
    arch.seteip( arch.template memread<32>( SS, src ), ARCH::ipret );
  }
};
  
template <class ARCH, unsigned OPSIZE>
struct NearParamReturn : public Operation<ARCH>
{
  typedef typename ARCH::u32_t u32_t;
  NearParamReturn( OpBase<ARCH> const& opbase, uint16_t _paramsize ) : Operation<ARCH>( opbase ), paramsize( _paramsize ) {} uint16_t paramsize;

  void disasm( std::ostream& sink ) const { sink << "ret " << DisasmI( paramsize ); }

  void execute( ARCH& arch ) const
  {
    if (OPSIZE != 32) throw 0;
    u32_t src = arch.regread32( 4 );
    arch.regwrite32( 4, src + u32_t( 4 + paramsize ) );
    arch.seteip( arch.template memread<32>( SS, src ), ARCH::ipret );
  }
};
  
template <class ARCH, unsigned OPSIZE>
struct FarReturn : public Operation<ARCH>
{
  FarReturn( OpBase<ARCH> const& opbase ) : Operation<ARCH>( opbase ) {}
    
  void disasm( std::ostream& sink ) const { sink << "lret "; }
};
  
template <class ARCH, unsigned OPSIZE>
struct FarParamReturn : public Operation<ARCH>
{
  FarParamReturn( OpBase<ARCH> const& opbase, uint16_t _paramsize ) : Operation<ARCH>( opbase ), paramsize( _paramsize ) {} uint16_t paramsize;

  void disasm( std::ostream& sink ) const { sink << "lret " << DisasmI( paramsize ); }
};
  
template <class ARCH> struct DC<ARCH,RETURN> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\xc3" ) ))
    
    {
      if      (ic.opsize() == 16) return new NearReturn<ARCH,16>( _.opbase() );
      else if (ic.opsize() == 32) return new NearReturn<ARCH,32>( _.opbase() );
      else if (ic.opsize() == 64) return new NearReturn<ARCH,64>( _.opbase() );
      else return 0;
    }
    
  if (auto _ = match( ic, opcode( "\xc2" ) & Imm<16>() ))
    
    {
      if      (ic.opsize() == 16) return new NearParamReturn<ARCH,16>( _.opbase(), _.i( uint16_t() ) );
      else if (ic.opsize() == 32) return new NearParamReturn<ARCH,32>( _.opbase(), _.i( uint16_t() ) );
      else if (ic.opsize() == 64) return new NearParamReturn<ARCH,64>( _.opbase(), _.i( uint16_t() ) );
    }
    
  if (auto _ = match( ic, opcode( "\xcb" ) ))
    
    {
      if      (ic.opsize() == 16) return new FarReturn<ARCH,16>( _.opbase() );
      else if (ic.opsize() == 32) return new FarReturn<ARCH,32>( _.opbase() );
      else if (ic.opsize() == 64) return new FarReturn<ARCH,64>( _.opbase() );
      else return 0;
    }
    
  if (auto _ = match( ic, opcode( "\xca" ) & Imm<16>() ))
    
    {
      if      (ic.opsize() == 16) return new FarParamReturn<ARCH,16>( _.opbase(), _.i( uint16_t() ) );
      else if (ic.opsize() == 32) return new FarParamReturn<ARCH,32>( _.opbase(), _.i( uint16_t() ) );
      else if (ic.opsize() == 64) return new FarParamReturn<ARCH,64>( _.opbase(), _.i( uint16_t() ) );
    }
    
  return 0;
}};

template <class ARCH, unsigned OPSIZE>
struct InterruptReturn : public Operation<ARCH>
{
  InterruptReturn( OpBase<ARCH> const& opbase ) : Operation<ARCH>( opbase ) {}

  void disasm( std::ostream& sink ) const { sink << "iret "; }
};

template <class ARCH> struct DC<ARCH,INTRETURN> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\xcf" ) ))

    {
      if      (ic.opsize() == 16) return new InterruptReturn<ARCH,16>( _.opbase() );
      else if (ic.opsize() == 32) return new InterruptReturn<ARCH,32>( _.opbase() );
      else if (ic.opsize() == 64) return new InterruptReturn<ARCH,64>( _.opbase() );
      else return 0;
    }
    
  return 0;
}};

template <class ARCH>
struct Interrupt : public Operation<ARCH>
{
  Interrupt( OpBase<ARCH> const& opbase, uint8_t _num ) : Operation<ARCH>( opbase ), vector_number( _num ) {} uint8_t vector_number;

  void disasm( std::ostream& sink ) const { sink << "int " << DisasmI( vector_number ); }

  void execute( ARCH& arch ) const { arch.interrupt( vector_number ); }
};

template <class ARCH>
struct Interrupt3 : public Operation<ARCH>
{
  Interrupt3( OpBase<ARCH> const& opbase ) : Operation<ARCH>( opbase ) {}

  void disasm( std::ostream& sink ) const { sink << "int3"; }
};

template <class ARCH>
struct InterruptOF : public Operation<ARCH>
{
  InterruptOF( OpBase<ARCH> const& opbase ) : Operation<ARCH>( opbase ) {}

  void disasm( std::ostream& sink ) const { sink << "into"; }
};

template <class ARCH> struct DC<ARCH,INTERRUPT> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\xcc" ) ))
  
    return new Interrupt3<ARCH>( _.opbase() );
  
  if (auto _ = match( ic, opcode( "\xcd" ) & Imm<8>() ))
  
    return new Interrupt<ARCH>( _.opbase(), _.i( uint8_t() ) );
  
  if (auto _ = match( ic, opcode( "\xce" ) ))
  
    return new Interrupt3<ARCH>( _.opbase() );
  
  return 0;
}};

template <class ARCH, unsigned OPSIZE>
struct Enter : public Operation<ARCH>
{
  uint16_t allocsize; uint8_t nestinglevel;
  Enter( OpBase<ARCH> const& opbase, uint16_t _allocsize, uint8_t _nestinglevel )
    : Operation<ARCH>( opbase ), allocsize( _allocsize ), nestinglevel( _nestinglevel ) {}

  void disasm( std::ostream& sink ) const { sink << "enter " << DisasmI( allocsize ) << ',' << DisasmI( nestinglevel ); }
};

template <class ARCH, unsigned OPSIZE/*, unsigned SPSIZE*/>
struct Leave : public Operation<ARCH>
{
  typedef typename ARCH::u32_t u32_t;
  Leave( OpBase<ARCH> const& opbase ) : Operation<ARCH>( opbase ) {}
  
  void disasm( std::ostream& sink ) const { sink << "leave"; }
  
  void execute( ARCH& arch ) const {
    /* TODO: STACKSIZE */
    if (OPSIZE != 32) throw 0;
    arch.regwrite32( 4, arch.regread32( 5 ) );
    u32_t src = arch.regread32( 4 );
    arch.regwrite32( 4, src + u32_t( 4 ) );
    arch.regwrite32( 5, arch.template memread<32>( SS, src ) );
  }
};

template <class ARCH> struct DC<ARCH,ENTER_LEAVE> { Operation<ARCH>* get( InputCode<ARCH> const& ic )
{
  if (auto _ = match( ic, opcode( "\xc8" ) & Imm<16>() & Imm<8>() ))
  
    {
      if      (ic.opsize() == 16) return new Enter<ARCH,16>( _.opbase(), _.i( uint16_t() ), _.i( uint8_t() ) );
      else if (ic.opsize() == 32) return new Enter<ARCH,32>( _.opbase(), _.i( uint16_t() ), _.i( uint8_t() ) );
      else if (ic.opsize() == 64) return new Enter<ARCH,64>( _.opbase(), _.i( uint16_t() ), _.i( uint8_t() ) );
      else return 0;
    }
  
  if (auto _ = match( ic, opcode( "\xc9" ) ))
  
    {
      if      (ic.opsize() == 16) return new Leave<ARCH,16>( _.opbase() );
      else if (ic.opsize() == 32) return new Leave<ARCH,32>( _.opbase() );
      else if (ic.opsize() == 64) return new Leave<ARCH,64>( _.opbase() );
    }
  return 0;
}};
  
