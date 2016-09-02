/*
 *  Copyright (c) 2007-2015,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific 
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY 
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_INTEL_EXECUTE_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_INTEL_EXECUTE_HH__

#include <unisim/component/cxx/processor/intel/types.hh>
#include <stdexcept>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace intel {

  template <class ARCH, typename INT>
  void
  eval_PSZ( ARCH& arch, INT const& res )
  {
    {
      INT red = res & INT( 0xff );
      for (int shift = 4; shift > 0; shift >>= 1) red ^= (red >> shift);
      arch.flagwrite( ARCH::PF, not bit_t( red & INT( 1 ) ) );
    }
    
    INT const msbmask( INT( 1 ) << (tpinfo<INT>::bitsize-1) );
    arch.flagwrite( ARCH::SF, bit_t( res & msbmask ) );
    arch.flagwrite( ARCH::ZF, bit_t( res == INT( 0 ) ) );
  }
  
  template <class ARCH, typename INT>
  INT
  eval_add( ARCH& arch, INT const& arg1, INT const& arg2 )
  {
    INT res = arg1 + arg2;
    
    INT const msbmask( INT( 1 ) << (tpinfo<INT>::bitsize-1) );
    arch.flagwrite( ARCH::OF, bit_t( (((arg1 & arg2 & ~res) | (~arg1 & ~arg2 & res)) & msbmask) == msbmask ) );
    arch.flagwrite( ARCH::CF, bit_t( ((((arg1 | arg2) & ~res) | (arg1 & arg2 & res)) & msbmask) == msbmask ) );
    
    eval_PSZ( arch, res );
    
    return res;
  }

  template <class ARCH, typename INT>
  INT
  eval_or( ARCH& arch, INT const& arg1, INT const& arg2 )
  {
    INT res = arg1 | arg2;
    
    arch.flagwrite( ARCH::OF, bit_t( false ) );
    arch.flagwrite( ARCH::CF, bit_t( false ) );
    
    eval_PSZ( arch, res );
      
    return res;
  }

  template <class ARCH, typename INT>
  INT
  eval_adc( ARCH& arch, INT const& arg1, INT const& arg2 )
  {
    INT op2 = arg2 + INT( arch.flagread( ARCH::CF ) );
    INT res = arg1 + op2;
    
    INT const msbmask( INT( 1 ) << (tpinfo<INT>::bitsize-1) );
    arch.flagwrite( ARCH::OF, bit_t( (((arg1 & op2 & ~res) | (~arg1 & ~op2 & res)) & msbmask) == msbmask ) );
    arch.flagwrite( ARCH::CF, bit_t( ((((arg1 | op2) & ~res) | (arg1 & op2 & res)) & msbmask) == msbmask ) );
    
    eval_PSZ( arch, res );
      
    return res;
  }

  template <class ARCH, typename INT>
  INT
  eval_sbb( ARCH& arch, INT const& arg1, INT const& arg2 )
  {
    INT op2 = arg2 + INT( arch.flagread( ARCH::CF ) );
    INT res = arg1 - op2;
    
    INT const msbmask( INT( 1 ) << (tpinfo<INT>::bitsize-1) );
    arch.flagwrite( ARCH::OF, bit_t( (((arg1 & ~op2 & ~res) | (~arg1 & op2 & res)) & msbmask) == msbmask ) );
    arch.flagwrite( ARCH::CF, bit_t( ((((~arg1 | op2) & res) | (~arg1 & op2 & ~res)) & msbmask) == msbmask ) );
    
    eval_PSZ( arch, res );
      
    return res;
  }

  template <class ARCH, typename INT>
  INT
  eval_and( ARCH& arch, INT const& arg1, INT const& arg2 )
  {
    INT res = arg1 & arg2;
    
    arch.flagwrite( ARCH::OF, bit_t( false ) );
    arch.flagwrite( ARCH::CF, bit_t( false ) );
    
    eval_PSZ( arch, res );
      
    return res;
  }

  template <class ARCH, typename INT>
  INT
  eval_sub( ARCH& arch, INT const& arg1, INT const& arg2 )
  {
    INT res = arg1 - arg2;
    
    INT const msbmask( INT( 1 ) << (tpinfo<INT>::bitsize-1) );
    arch.flagwrite( ARCH::OF, bit_t( (((arg1 & ~arg2 & ~res) | (~arg1 & arg2 & res)) & msbmask) == msbmask ) );
    arch.flagwrite( ARCH::CF, bit_t( ((((~arg1 | arg2) & res) | (~arg1 & arg2 & ~res)) & msbmask) == msbmask ) );
    
    eval_PSZ( arch, res );
      
    return res;
  }

  template <class ARCH, typename INT>
  INT
  eval_xor( ARCH& arch, INT const& arg1, INT const& arg2 )
  {
    INT res = arg1 ^ arg2;
    
    arch.flagwrite( ARCH::OF, bit_t( false ) );
    arch.flagwrite( ARCH::CF, bit_t( false ) );
    
    eval_PSZ( arch, res );
      
    return res;
  }
  
  /* NOTE: since 286, only the 5 lsbs of shift values are considered by
   * intel shifters (even for words and bytes). TODO: and in mode64 ?
   */

  template <class ARCH, typename INT>
  INT
  eval_rol( ARCH& arch, INT const& arg1, u8_t const& arg2 )
  {
    intptr_t const bitsize = tpinfo<INT>::bitsize;
    u8_t const u8bitsize( bitsize );
    INT const msb = INT( 1 ) << (bitsize-1);
    INT res( 0 );
    
    u8_t sharg = arg2 & u8_t( 0x1f ); 
    
    sharg = sharg % u8bitsize;
    res = (arg1 << sharg) | (arg1 >> (u8bitsize - sharg));
    arch.flagwrite( ARCH::CF, bit_t( res & INT( 1 ) ) );
    arch.flagwrite( ARCH::OF, bit_t( (arg1 ^ res) & msb ) );
      
    eval_PSZ( arch, res );
      
    return res;
  }

  template <class ARCH, typename INT>
  INT
  eval_ror( ARCH& arch, INT const& arg1, u8_t const& arg2 )
  {
    intptr_t const bitsize = tpinfo<INT>::bitsize;
    u8_t const u8bitsize( bitsize );
    INT const msb = INT( 1 ) << (bitsize-1);
    INT res( 0 );
    
    u8_t sharg = arg2 & u8_t( 0x1f ); 
    
    sharg = sharg % u8bitsize;
    res = (arg1 << (u8bitsize - sharg)) | (arg1 >> sharg);
    arch.flagwrite( ARCH::CF, bit_t( res & msb ) );
    arch.flagwrite( ARCH::OF, bit_t( (arg1 ^ res) & msb ) );
      
    eval_PSZ( arch, res );
      
    return res;
  }

  template <class ARCH, typename INT>
  INT
  eval_rcl( ARCH& arch, INT const& arg1, u8_t const& arg2 )
  {
    intptr_t const bitsize = tpinfo<INT>::bitsize;
    INT const msb = INT( 1 ) << (bitsize-1);
    INT res( 0 );
    
    u8_t sharg = arg2 & u8_t( 0x1f ); 
    
    sharg = sharg % u8_t( bitsize + 1 );
    typename tpinfo<INT>::twice tmp( arg1 );
    tmp |= typename tpinfo<INT>::twice( arch.flagread( ARCH::CF ) ) << bitsize;
    tmp = (tmp << sharg) | (tmp >> (u8_t( bitsize + 1 ) - sharg));
    res = INT( tmp );
    arch.flagwrite( ARCH::CF, bit_t( (tmp >> bitsize) & typename tpinfo<INT>::twice( 1 ) ) );
    arch.flagwrite( ARCH::OF, bit_t( (arg1 ^ res) & msb ) );
      
    eval_PSZ( arch, res );
      
    return res;
  }
  
  /* TODO: should implement large rotate through carry */
  //  template <> u64_t eval_rcl( ARCH& arch, u64_t const& arg1, u8_t const& arg2 ) { throw 0; }
  
  template <class ARCH, typename INT>
  INT
  eval_rcr( ARCH& arch, INT const& arg1, u8_t const& arg2 )
  {
    intptr_t const bitsize = tpinfo<INT>::bitsize;
    INT const msb = INT( 1 ) << (bitsize-1);
    INT res( 0 );
    
    u8_t sharg = arg2 & u8_t( 0x1f ); 
    
    sharg = sharg % u8_t( bitsize + 1 );
    typename tpinfo<INT>::twice tmp( arg1 );
    tmp |= typename tpinfo<INT>::twice( arch.flagread( ARCH::CF ) ) << bitsize;
    tmp = (tmp << (u8_t( bitsize + 1 ) - sharg)) | (tmp >> sharg);
    res = INT( tmp );
    arch.flagwrite( ARCH::CF, bit_t( (tmp >> bitsize) & typename tpinfo<INT>::twice( 1 ) ) );
    arch.flagwrite( ARCH::OF, bit_t( (arg1 ^ res) & msb ) );
      
    eval_PSZ( arch, res );
      
    return res;
  }
  
  /* TODO: whould implement large rotate through carry */
  // template <> u64_t eval_rcr( ARCH& arch, u64_t const& arg1, u8_t const& arg2 ) { throw 0; }
  
  template <class ARCH, typename INT>
  INT
  eval_shl( ARCH& arch, INT const& arg1, u8_t const& arg2 )
  {
    intptr_t const bitsize = tpinfo<INT>::bitsize;
    INT const msb = INT( 1 ) << (bitsize-1);
    INT res( 0 );
    
    u8_t sharg = arg2 & u8_t( 0x1f ); 
    
    res = arg1 << sharg;
    arch.flagwrite( ARCH::CF, bit_t( mkbool( sharg >= u8_t( 1 ) ) ? ((arg1 << (sharg - u8_t( 1 ))) & msb) : INT( 0 ) ) );
    arch.flagwrite( ARCH::OF, bit_t( (arg1 ^ res) & msb ) );
      
    eval_PSZ( arch, res );
      
    return res;
  }

  template <class ARCH, typename INT>
  INT
  eval_shr( ARCH& arch, INT const& arg1, u8_t const& arg2 )
  {
    intptr_t const bitsize = tpinfo<INT>::bitsize;
    INT const msb = INT( 1 ) << (bitsize-1);
    INT res( 0 );
    
    u8_t sharg = arg2 & u8_t( 0x1f ); 
    
    res = arg1 >> sharg;
    arch.flagwrite( ARCH::CF, bit_t( mkbool( sharg >= u8_t( 1 ) ) ? ((arg1 >> (sharg - u8_t( 1 ))) & INT( 1 )) : INT( 0 ) ) );
    arch.flagwrite( ARCH::OF, bit_t( arg1 & msb ) );
      
    eval_PSZ( arch, res );
      
    return res;
  }
  
  template <class ARCH, typename INT>
  INT
  eval_sar( ARCH& arch, INT const& arg1, u8_t const& arg2 )
  {
    //intptr_t const bitsize = tpinfo<INT>::bitsize;
    //INT const msb = INT( 1 ) << (bitsize-1);
    INT res( 0 );
    
    u8_t sharg = arg2 & u8_t( 0x1f ); 
    
    res = INT( (typename tpinfo<INT>::stype( arg1 )) >> sharg );
    arch.flagwrite( ARCH::CF, bit_t( mkbool( sharg >= u8_t( 1 ) ) ? ((arg1 >> (sharg - u8_t( 1 ))) & INT( 1 )) : INT( 0 ) ) );
    arch.flagwrite( ARCH::OF, bit_t( false ) );
      
    eval_PSZ( arch, res );
      
    return res;
  }
  
  template <class ARCH>
  bit_t
  eval_cond( ARCH& a, uint32_t _cc )
  {
    bit_t res = bit_t( _cc & 1 );
    switch ((_cc >> 1) & 0x7) {
    case 0: res ^= a.flagread( ARCH::OF ); break;
    case 1: res ^= a.flagread( ARCH::CF ); break;
    case 2: res ^= a.flagread( ARCH::ZF ); break;
    case 3: res ^= a.flagread( ARCH::CF ) | a.flagread( ARCH::ZF ); break;
    case 4: res ^= a.flagread( ARCH::SF ); break;
    case 5: res ^= a.flagread( ARCH::PF ); break;
    case 6: res ^= a.flagread( ARCH::SF ) ^ a.flagread( ARCH::OF ); break;
    case 7: res ^= a.flagread( ARCH::ZF ) | (a.flagread( ARCH::SF ) ^ a.flagread( ARCH::OF )); break;
    }
    return res;
  }
  
  template <class ARCH, typename INT>
  void eval_div( ARCH& arch, INT& hi, INT& lo, INT const& divisor )
  {
    typedef typename tpinfo<INT>::utype utype;
    typedef typename tpinfo<INT>::twice twice;
    twice dividend = (twice( hi ) << (tpinfo<INT>::bitsize)) | twice( utype( lo ) );
    twice result = dividend / divisor;
    //if (twice( INT( result ) ) != result) arch.DivError();
    lo = INT( result );
    hi = dividend % divisor;
  }
  
  /* TODO: need to implement very large divisions */
  //  template <> void eval_div( ARCH& arch, u64_t& hi, u64_t& lo, u64_t const& divisor ) { throw 0; }
  //  template <> void eval_div( ARCH& arch, s64_t& hi, s64_t& lo, s64_t const& divisor ) { throw 0; }
  
  template <class ARCH, typename INT>
  void eval_mul( ARCH& arch, INT& hi, INT& lo, INT const& multiplier )
  {
    // typedef typename tpinfo<INT>::utype utype;
    typedef typename tpinfo<INT>::twice twice;
    twice result = twice( lo ) * twice( multiplier );
    hi = INT( result >> tpinfo<INT>::bitsize );
    INT lores = INT( result );
    lo = lores;
    bit_t flag = twice( lores ) == result;
    arch.flagwrite( ARCH::OF, flag );
    arch.flagwrite( ARCH::CF, flag );
  }
  
  /* TODO: need to implement very large multiplications */
  // template <> void eval_mul( ARCH& arch, u64_t& hi, u64_t& lo, u64_t const& divisor ) { throw 0; }
  // template <> void eval_mul( ARCH& arch, s64_t& hi, s64_t& lo, s64_t const& divisor ) { throw 0; }
  
  template <class ARCH>
  u16_t
  fswread( ARCH& a )
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
      (u16_t( a.flagread( ARCH::C0 ) ) <<  8 /* C0 */) |
      (u16_t( a.flagread( ARCH::C1 ) ) <<  9 /* C1 */) |
      (u16_t( a.flagread( ARCH::C2 ) ) << 10 /* C2 */) |
      (u16_t( a.ftopread() )           << 11 /* ST */) |
      (u16_t( a.flagread( ARCH::C3 ) ) << 14 /* C3 */) |
      (u16_t( 0 )                      << 15 /*  B */);
  }

  template <class ARCH>
  u32_t
  eflagsread( ARCH& a )
  {
    //00000286
    return
      (u32_t( a.flagread( ARCH::CF ) ) <<  0 /* CF */ ) |
      (u32_t( 1 )                      <<  1 /*  1 */ ) |
      (u32_t( a.flagread( ARCH::PF ) ) <<  2 /* PF */ ) |
      (u32_t( 0 )                      <<  3 /*  0 */ ) |
      (u32_t( a.flagread( ARCH::AF ) ) <<  4 /* AF */ ) |
      (u32_t( 0 )                      <<  5 /*  0 */ ) |
      (u32_t( a.flagread( ARCH::ZF ) ) <<  6 /* ZF */ ) |
      (u32_t( a.flagread( ARCH::SF ) ) <<  7 /* SF */ ) |
      (u32_t( 0 )                      <<  8 /* TF */ ) |
      (u32_t( 0 )                      <<  9 /* IF */ ) |
      (u32_t( a.flagread( ARCH::DF ) ) << 10 /* DF */ ) |
      (u32_t( a.flagread( ARCH::OF ) ) << 11 /* OF */ ) |
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

} // end of namespace intel
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_INTEL_EXECUTE_HH__
