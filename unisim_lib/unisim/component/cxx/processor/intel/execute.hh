#ifndef INTEL_EXEC_HH
#define INTEL_EXEC_HH

#include <unisim/component/cxx/processor/intel/types.hh>
#include <stdexcept>

namespace intel
{
  template <class INTEGER>
  void
  eval_PSZ( Arch& arch, INTEGER const& res )
  {
    {
      INTEGER red = res & INTEGER( 0xff );
      for (int shift = 4; shift > 0; shift >>= 1) red ^= (red >> shift);
      arch.flagwrite( PF, not bit_t( red & INTEGER( 1 ) ) );
    }
    
    INTEGER const msbmask( INTEGER( 1 ) << (tpinfo<INTEGER>::bitsize-1) );
    arch.flagwrite( SF, bit_t( res & msbmask ) );
    arch.flagwrite( ZF, bit_t( res == INTEGER( 0 ) ) );
  }
  
  template <class INTEGER>
  INTEGER
  eval_add( Arch& arch, INTEGER const& arg1, INTEGER const& arg2 )
  {
    INTEGER res = arg1 + arg2;
    
    INTEGER const msbmask( INTEGER( 1 ) << (tpinfo<INTEGER>::bitsize-1) );
    arch.flagwrite( OF, bit_t( (((arg1 & arg2 & ~res) | (~arg1 & ~arg2 & res)) & msbmask) == msbmask ) );
    arch.flagwrite( CF, bit_t( ((((arg1 | arg2) & ~res) | (arg1 & arg2 & res)) & msbmask) == msbmask ) );
    
    eval_PSZ( arch, res );
    
    return res;
  }

  template <class INTEGER>
  INTEGER
  eval_or( Arch& arch, INTEGER const& arg1, INTEGER const& arg2 )
  {
    INTEGER res = arg1 | arg2;
    
    arch.flagwrite( OF, bit_t( false ) );
    arch.flagwrite( CF, bit_t( false ) );
    
    eval_PSZ( arch, res );
      
    return res;
  }

  template <class INTEGER>
  INTEGER
  eval_adc( Arch& arch, INTEGER const& arg1, INTEGER const& arg2 )
  {
    INTEGER op2 = arg2 + INTEGER( arch.flagread( CF ) );
    INTEGER res = arg1 + op2;
    
    INTEGER const msbmask( INTEGER( 1 ) << (tpinfo<INTEGER>::bitsize-1) );
    arch.flagwrite( OF, bit_t( (((arg1 & op2 & ~res) | (~arg1 & ~op2 & res)) & msbmask) == msbmask ) );
    arch.flagwrite( CF, bit_t( ((((arg1 | op2) & ~res) | (arg1 & op2 & res)) & msbmask) == msbmask ) );
    
    eval_PSZ( arch, res );
      
    return res;
  }

  template <class INTEGER>
  INTEGER
  eval_sbb( Arch& arch, INTEGER const& arg1, INTEGER const& arg2 )
  {
    INTEGER op2 = arg2 + INTEGER( arch.flagread( CF ) );
    INTEGER res = arg1 - op2;
    
    INTEGER const msbmask( INTEGER( 1 ) << (tpinfo<INTEGER>::bitsize-1) );
    arch.flagwrite( OF, bit_t( (((arg1 & ~op2 & ~res) | (~arg1 & op2 & res)) & msbmask) == msbmask ) );
    arch.flagwrite( CF, bit_t( ((((~arg1 | op2) & res) | (~arg1 & op2 & ~res)) & msbmask) == msbmask ) );
    
    eval_PSZ( arch, res );
      
    return res;
  }

  template <class INTEGER>
  INTEGER
  eval_and( Arch& arch, INTEGER const& arg1, INTEGER const& arg2 )
  {
    INTEGER res = arg1 & arg2;
    
    arch.flagwrite( OF, bit_t( false ) );
    arch.flagwrite( CF, bit_t( false ) );
    
    eval_PSZ( arch, res );
      
    return res;
  }

  template <class INTEGER>
  INTEGER
  eval_sub( Arch& arch, INTEGER const& arg1, INTEGER const& arg2 )
  {
    INTEGER res = arg1 - arg2;
    
    INTEGER const msbmask( INTEGER( 1 ) << (tpinfo<INTEGER>::bitsize-1) );
    arch.flagwrite( OF, bit_t( (((arg1 & ~arg2 & ~res) | (~arg1 & arg2 & res)) & msbmask) == msbmask ) );
    arch.flagwrite( CF, bit_t( ((((~arg1 | arg2) & res) | (~arg1 & arg2 & ~res)) & msbmask) == msbmask ) );
    
    eval_PSZ( arch, res );
      
    return res;
  }

  template <class INTEGER>
  INTEGER
  eval_xor( Arch& arch, INTEGER const& arg1, INTEGER const& arg2 )
  {
    INTEGER res = arg1 ^ arg2;
    
    arch.flagwrite( OF, bit_t( false ) );
    arch.flagwrite( CF, bit_t( false ) );
    
    eval_PSZ( arch, res );
      
    return res;
  }
  
  /* NOTE: since 286, only the 5 lsbs of shift values are considered by
   * intel shifters (even for words and bytes). TODO: and in mode64 ?
   */

  template <class INTEGER>
  INTEGER
  eval_rol( Arch& arch, INTEGER const& arg1, u8_t const& arg2 )
  {
    intptr_t const bitsize = tpinfo<INTEGER>::bitsize;
    u8_t const u8bitsize( bitsize );
    INTEGER const msb = INTEGER( 1 ) << (bitsize-1);
    INTEGER res( 0 );
    
    u8_t sharg = arg2 & u8_t( 0x1f ); 
    
    sharg = sharg % u8bitsize;
    res = (arg1 << sharg) | (arg1 >> (u8bitsize - sharg));
    arch.flagwrite( CF, bit_t( res & INTEGER( 1 ) ) );
    arch.flagwrite( OF, bit_t( (arg1 ^ res) & msb ) );
      
    eval_PSZ( arch, res );
      
    return res;
  }

  template <class INTEGER>
  INTEGER
  eval_ror( Arch& arch, INTEGER const& arg1, u8_t const& arg2 )
  {
    intptr_t const bitsize = tpinfo<INTEGER>::bitsize;
    u8_t const u8bitsize( bitsize );
    INTEGER const msb = INTEGER( 1 ) << (bitsize-1);
    INTEGER res( 0 );
    
    u8_t sharg = arg2 & u8_t( 0x1f ); 
    
    sharg = sharg % u8bitsize;
    res = (arg1 << (u8bitsize - sharg)) | (arg1 >> sharg);
    arch.flagwrite( CF, bit_t( res & msb ) );
    arch.flagwrite( OF, bit_t( (arg1 ^ res) & msb ) );
      
    eval_PSZ( arch, res );
      
    return res;
  }

  template <class INTEGER>
  INTEGER
  eval_rcl( Arch& arch, INTEGER const& arg1, u8_t const& arg2 )
  {
    intptr_t const bitsize = tpinfo<INTEGER>::bitsize;
    INTEGER const msb = INTEGER( 1 ) << (bitsize-1);
    INTEGER res( 0 );
    
    u8_t sharg = arg2 & u8_t( 0x1f ); 
    
    sharg = sharg % u8_t( bitsize + 1 );
    typename tpinfo<INTEGER>::twice tmp( arg1 );
    tmp |= typename tpinfo<INTEGER>::twice( arch.flagread( CF ) ) << bitsize;
    tmp = (tmp << sharg) | (tmp >> (u8_t( bitsize + 1 ) - sharg));
    res = INTEGER( tmp );
    arch.flagwrite( CF, bit_t( (tmp >> bitsize) & typename tpinfo<INTEGER>::twice( 1 ) ) );
    arch.flagwrite( OF, bit_t( (arg1 ^ res) & msb ) );
      
    eval_PSZ( arch, res );
      
    return res;
  }
  
  /* TODO: whould implement large rotate through carry */
  template <> u64_t eval_rcl( Arch& arch, u64_t const& arg1, u8_t const& arg2 ) { throw 0; }
  
  template <class INTEGER>
  INTEGER
  eval_rcr( Arch& arch, INTEGER const& arg1, u8_t const& arg2 )
  {
    intptr_t const bitsize = tpinfo<INTEGER>::bitsize;
    INTEGER const msb = INTEGER( 1 ) << (bitsize-1);
    INTEGER res( 0 );
    
    u8_t sharg = arg2 & u8_t( 0x1f ); 
    
    sharg = sharg % u8_t( bitsize + 1 );
    typename tpinfo<INTEGER>::twice tmp( arg1 );
    tmp |= typename tpinfo<INTEGER>::twice( arch.flagread( CF ) ) << bitsize;
    tmp = (tmp << (u8_t( bitsize + 1 ) - sharg)) | (tmp >> sharg);
    res = INTEGER( tmp );
    arch.flagwrite( CF, bit_t( (tmp >> bitsize) & typename tpinfo<INTEGER>::twice( 1 ) ) );
    arch.flagwrite( OF, bit_t( (arg1 ^ res) & msb ) );
      
    eval_PSZ( arch, res );
      
    return res;
  }
  
  /* TODO: whould implement large rotate through carry */
  template <> u64_t eval_rcr( Arch& arch, u64_t const& arg1, u8_t const& arg2 ) { throw 0; }
  
  template <class INTEGER>
  INTEGER
  eval_shl( Arch& arch, INTEGER const& arg1, u8_t const& arg2 )
  {
    intptr_t const bitsize = tpinfo<INTEGER>::bitsize;
    INTEGER const msb = INTEGER( 1 ) << (bitsize-1);
    INTEGER res( 0 );
    
    u8_t sharg = arg2 & u8_t( 0x1f ); 
    
    res = arg1 << sharg;
    arch.flagwrite( CF, bit_t( mkbool( sharg >= u8_t( 1 ) ) ? ((arg1 << (sharg - u8_t( 1 ))) & msb) : INTEGER( 0 ) ) );
    arch.flagwrite( OF, bit_t( (arg1 ^ res) & msb ) );
      
    eval_PSZ( arch, res );
      
    return res;
  }

  template <class INTEGER>
  INTEGER
  eval_shr( Arch& arch, INTEGER const& arg1, u8_t const& arg2 )
  {
    intptr_t const bitsize = tpinfo<INTEGER>::bitsize;
    INTEGER const msb = INTEGER( 1 ) << (bitsize-1);
    INTEGER res( 0 );
    
    u8_t sharg = arg2 & u8_t( 0x1f ); 
    
    res = arg1 >> sharg;
    arch.flagwrite( CF, bit_t( mkbool( sharg >= u8_t( 1 ) ) ? ((arg1 >> (sharg - u8_t( 1 ))) & INTEGER( 1 )) : INTEGER( 0 ) ) );
    arch.flagwrite( OF, bit_t( arg1 & msb ) );
      
    eval_PSZ( arch, res );
      
    return res;
  }
  
  template <class INTEGER>
  INTEGER
  eval_sar( Arch& arch, INTEGER const& arg1, u8_t const& arg2 )
  {
    intptr_t const bitsize = tpinfo<INTEGER>::bitsize;
    INTEGER const msb = INTEGER( 1 ) << (bitsize-1);
    INTEGER res( 0 );
    
    u8_t sharg = arg2 & u8_t( 0x1f ); 
    
    res = INTEGER( (typename tpinfo<INTEGER>::stype( arg1 )) >> sharg );
    arch.flagwrite( CF, bit_t( mkbool( sharg >= u8_t( 1 ) ) ? ((arg1 >> (sharg - u8_t( 1 ))) & INTEGER( 1 )) : INTEGER( 0 ) ) );
    arch.flagwrite( OF, bit_t( false ) );
      
    eval_PSZ( arch, res );
      
    return res;
  }
  
  template <class t_ARCH>
  bit_t
  eval_cond( t_ARCH& a, uint32_t _cc )
  {
    bit_t res = bit_t( _cc & 1 );
    switch ((_cc >> 1) & 0x7) {
    case 0: res ^= a.flagread( intel::OF ); break;
    case 1: res ^= a.flagread( intel::CF ); break;
    case 2: res ^= a.flagread( intel::ZF ); break;
    case 3: res ^= a.flagread( intel::CF ) | a.flagread( intel::ZF ); break;
    case 4: res ^= a.flagread( intel::SF ); break;
    case 5: res ^= a.flagread( intel::PF ); break;
    case 6: res ^= a.flagread( intel::SF ) ^ a.flagread( intel::OF ); break;
    case 7: res ^= a.flagread( intel::ZF ) | (a.flagread( intel::SF ) ^ a.flagread( intel::OF )); break;
    }
    return res;
  }
  
  template <typename INTEGER>
  void eval_div( Arch& arch, INTEGER& hi, INTEGER& lo, INTEGER const& divisor )
  {
    typedef typename tpinfo<INTEGER>::utype utype;
    typedef typename tpinfo<INTEGER>::twice twice;
    twice dividend = (twice( hi ) << (tpinfo<INTEGER>::bitsize)) | twice( utype( lo ) );
    twice result = dividend / divisor;
    //if (twice( INTEGER( result ) ) != result) arch.DivError();
    lo = INTEGER( result );
    hi = dividend % divisor;
  }
  
  /* TODO: need to implement very large divisions */
  template <> void eval_div( Arch& arch, u64_t& hi, u64_t& lo, u64_t const& divisor ) { throw 0; }
  template <> void eval_div( Arch& arch, s64_t& hi, s64_t& lo, s64_t const& divisor ) { throw 0; }
  
  template <typename INTEGER>
  void eval_mul( Arch& arch, INTEGER& hi, INTEGER& lo, INTEGER const& multiplier )
  {
    typedef typename tpinfo<INTEGER>::utype utype;
    typedef typename tpinfo<INTEGER>::twice twice;
    twice result = twice( lo ) * twice( multiplier );
    hi = INTEGER( result >> tpinfo<INTEGER>::bitsize );
    INTEGER lores = INTEGER( result );
    lo = lores;
    bit_t flag = twice( lores ) == result;
    arch.flagwrite( OF, flag );
    arch.flagwrite( CF, flag );
  }
  
  /* TODO: need to implement very large multiplications */
  template <> void eval_mul( Arch& arch, u64_t& hi, u64_t& lo, u64_t const& divisor ) { throw 0; }
  template <> void eval_mul( Arch& arch, s64_t& hi, s64_t& lo, s64_t const& divisor ) { throw 0; }
  
  template <class t_ARCH>
  u16_t
  fswread( t_ARCH& a )
  {
    return
      (u16_t( 0 )                      <<  0 /* IE */) |
      (u16_t( 0 )                      <<  1 /* DE */) |
      (u16_t( 0 )                      <<  2 /* ZE */) |
      (u16_t( 0 )                      <<  3 /* OE */) |
      (u16_t( 0 )                      <<  4 /* UE */) |
      (u16_t( 0 )                      <<  5 /* PE */) |
      (u16_t( 0 )                      <<  6 /* SF */) |
      (u16_t( 0 )                      <<  7 /* ES */) |
      (u16_t( a.flagread( intel::C0 ) ) <<  8 /* C0 */) |
      (u16_t( a.flagread( intel::C1 ) ) <<  9 /* C1 */) |
      (u16_t( a.flagread( intel::C2 ) ) << 10 /* C2 */) |
      (u16_t( a.ftopread() )           << 11 /* ST */) |
      (u16_t( a.flagread( intel::C3 ) ) << 14 /* C3 */) |
      (u16_t( 0 )                      << 15 /*  B */);
  }

  template <class t_ARCH>
  u32_t
  eflagsread( t_ARCH& a )
  {
    //00000286
    return
      (u32_t( a.flagread( intel::CF ) ) <<  0 /* CF */ ) |
      (u32_t( 1 )                      <<  1 /*  1 */ ) |
      (u32_t( a.flagread( intel::PF ) ) <<  2 /* PF */ ) |
      (u32_t( 0 )                      <<  3 /*  0 */ ) |
      (u32_t( a.flagread( intel::AF ) ) <<  4 /* AF */ ) |
      (u32_t( 0 )                      <<  5 /*  0 */ ) |
      (u32_t( a.flagread( intel::ZF ) ) <<  6 /* ZF */ ) |
      (u32_t( a.flagread( intel::SF ) ) <<  7 /* SF */ ) |
      (u32_t( 0 )                      <<  8 /* TF */ ) |
      (u32_t( 0 )                      <<  9 /* IF */ ) |
      (u32_t( a.flagread( intel::DF ) ) << 10 /* DF */ ) |
      (u32_t( a.flagread( intel::OF ) ) << 11 /* OF */ ) |
      (u32_t( 0 )                      << 12 /*IOPL*/ ) |
      (u32_t( 0 )                      << 14 /* NT */ ) |
      (u32_t( 0 )                      << 15 /*  0 */ ) |
      (u32_t( 0 )                      << 16 /* RF */ ) |
      (u32_t( 0 )                      << 17 /* VM */ ) |
      (u32_t( 0 )                      << 18 /* AC */ ) |
      (u32_t( 0 )                      << 19 /* VIF*/ ) |
      (u32_t( 0 )                      << 20 /* VIP*/ ) |
      (u32_t( 0 )                      << 21 /* ID */ );
  }
}; // end of namespace intel

#endif // INTEL_EXEC_HH
    
