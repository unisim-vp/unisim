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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_INTEL_DISASM_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_INTEL_DISASM_HH__

#include <unisim/component/cxx/processor/intel/segments.hh>
#include <unisim/component/cxx/processor/intel/tmp.hh>
#include <limits>
#include <iosfwd>
#include <stdint.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace intel {

  struct DisasmObject
  {
    virtual void operator() ( std::ostream& sink ) const = 0;
    virtual ~DisasmObject() {};
  };
  
  std::ostream& operator << ( std::ostream& sink, DisasmObject const& dobj );
  
  /* General purpose registers */
  struct DisasmGb : public DisasmObject
  {
    DisasmGb( unsigned _reg ) : reg( _reg ) {} unsigned reg;
    void operator() ( std::ostream& sink ) const;
  };
  struct DisasmGw : public DisasmObject
  {
    DisasmGw( unsigned _reg ) : reg( _reg ) {} unsigned reg;
    void operator() ( std::ostream& sink ) const;
  };
  struct DisasmGd : public DisasmObject
  {
    DisasmGd( unsigned _reg ) : reg( _reg ) {} unsigned reg;
    void operator() ( std::ostream& sink ) const;
  };
  struct DisasmGq : public DisasmObject
  {
    DisasmGq( unsigned _reg ) : reg( _reg ) {} unsigned reg;
    void operator() (std::ostream& sink ) const;
  };

  template <unsigned OPSIZE>
  struct DisasmG : public DisasmObject
  {
    DisasmG( unsigned _reg ) : reg( _reg ) {} unsigned reg;
    void operator() ( std::ostream& sink ) const {
      if      (OPSIZE == 8)  sink << DisasmGb( reg );
      else if (OPSIZE == 16) sink << DisasmGw( reg );
      else if (OPSIZE == 32) sink << DisasmGd( reg );
      else if (OPSIZE == 64) sink << DisasmGq( reg );
      else throw 0;
    }
  };
  
  /* Quadword registers*/
  struct DisasmPq : public DisasmObject
  {
    DisasmPq( unsigned _reg ) : reg( _reg ) {} unsigned reg;
    void operator() ( std::ostream& sink ) const;
  };
  
  /* Double quadword registers */
  struct DisasmVdq : public DisasmObject
  {
    DisasmVdq( unsigned _reg ) : reg( _reg ) {} unsigned reg;
    void operator() ( std::ostream& sink ) const;
  };
  
  /* Debug registers */
  struct DisasmDd : public DisasmObject
  {
    DisasmDd( unsigned _reg ) : reg( _reg ) {} unsigned reg;
    void operator() ( std::ostream& sink ) const;
  };
  /* Control registers */
  struct DisasmCd : public DisasmObject
  {
    DisasmCd( unsigned _reg ) : reg( _reg ) {} unsigned reg;
    void operator() ( std::ostream& sink ) const;
  };

  /* Segments */
  struct DisasmS : public DisasmObject
  {
    DisasmS( unsigned _seg ) : segment( _seg ) {} unsigned segment;
    void operator () ( std::ostream& sink ) const;
  };
  struct DisasmMS : public DisasmObject
  {
    DisasmMS( unsigned _seg ) : segment( _seg ) {} unsigned segment;
    void operator () ( std::ostream& sink ) const;
  };
  
  template <class ARCH> struct RMOp;
  
  template <class ARCH, class REGDIS>
  struct DisasmRegOrMem : public DisasmObject
  {
    DisasmRegOrMem( RMOp<ARCH> const& _rmop ) : rmop( _rmop ) {}

    
    void operator() ( std::ostream& sink ) const
    {
      typedef REGDIS DisasmReg;
      
      if (rmop.is_memory_operand())   rmop->disasm_memory_operand( sink );
      else                            sink << DisasmReg( rmop.ereg() );
    }
    
    RMOp<ARCH> const& rmop;    
  };
  
  template <class ARCH>
  DisasmRegOrMem<ARCH,DisasmGb>  DisasmEb( RMOp<ARCH> const& rmop ) { return DisasmRegOrMem<ARCH,DisasmGb>( rmop ); }
  
  template <class ARCH>
  DisasmRegOrMem<ARCH,DisasmGw>  DisasmEw( RMOp<ARCH> const& rmop ) { return DisasmRegOrMem<ARCH,DisasmGw>( rmop ); }

  template <class ARCH>
  DisasmRegOrMem<ARCH,DisasmGd>  DisasmEd( RMOp<ARCH> const& rmop ) { return DisasmRegOrMem<ARCH,DisasmGd>( rmop ); }
  
  
  template <class ARCH,unsigned OPSIZE>
  DisasmRegOrMem<ARCH,DisasmG<OPSIZE> > DisasmE( UI<OPSIZE> const&, RMOp<ARCH> const& rmop ) { return DisasmRegOrMem<ARCH,DisasmG<OPSIZE> >( rmop ); }
  
  template <class ARCH>
  DisasmRegOrMem<ARCH,DisasmPq>  DisasmQq( RMOp<ARCH> const& rmop ) { return DisasmRegOrMem<ARCH,DisasmPq>( rmop ); }
  
  template <class ARCH>
  DisasmRegOrMem<ARCH,DisasmVdq> DisasmWdq( RMOp<ARCH> const& rmop ) { return DisasmRegOrMem<ARCH,DisasmVdq>( rmop ); }

  struct DisasmBadReg : public DisasmObject
  {
    DisasmBadReg( unsigned _reg ) {}
    void operator() ( std::ostream& sink ) const;
  };
  
  template <class ARCH>
  DisasmRegOrMem<ARCH,DisasmBadReg> DisasmM( RMOp<ARCH> const& rmop ) { return DisasmRegOrMem<ARCH,DisasmBadReg>( rmop ); }
  
  void PutString( std::ostream& sink, char const* string );
  void PutChar( std::ostream& sink, char chr );
  
  /* Immediate disassembly */
  template <typename T>
  struct DisasmHex : public DisasmObject
  {
    DisasmHex( T _value ) : value( _value ) {} T value;
    
    void operator () ( std::ostream& sink ) const
    {
      typename __unsigned<T>::type rem = value;
      if (std::numeric_limits<T>::is_signed and value < T(0))
        { PutChar(sink, '-'); rem = -value; }
      uintptr_t const bcnt = sizeof(T);
      char buf[4+2*bcnt];
      char* ptr = &buf[sizeof(buf)];
      *--ptr = '\0';
      for (uintptr_t idx = 0; idx < 2*sizeof (T); idx++) {
        *--ptr = "0123456789abcdef"[rem&0xf];
        rem >>= 4;
        if (not rem) break;
      }
      *--ptr = 'x';
      *--ptr = '0';
      PutString( sink, ptr );
    }
  };
  
  template <typename T> struct DisasmImm : public DisasmHex<T>
  {
    DisasmImm( T value ) : DisasmHex<T>( value ) {}
    void operator () (std::ostream& sink) const { PutChar(sink,'$'); DisasmHex<T>::operator() ( sink ); }
  };

  template <typename T> DisasmHex<T> DisasmX( T imm ) { return DisasmHex<T>( imm ); }
  template <typename T> DisasmImm<T> DisasmI( T imm ) { return DisasmImm<T>( imm ); }
  
  struct DisasmCond : public DisasmObject
  {
    DisasmCond( unsigned _cond ) : cond( _cond ) {} unsigned cond;
    
    void operator() ( std::ostream& _sink ) const
    {
      static char const* conds[] = {"o", "no", "b", "ae", "e", "ne", "be", "a", "s", "ns", "p", "np", "l", "ge", "le", "g"};
      PutString( _sink, (cond & -16) ? "?" : conds[cond] );
    }
    
  };

  template <unsigned OPSIZE>
  struct DisasmMnemonic : public DisasmObject
  {
    DisasmMnemonic( char const* _mnemonic, bool _implicit_size = true ) : mnemonic( _mnemonic ), implicit_size( _implicit_size ) {}
    char const* mnemonic;
    bool implicit_size;
    
    void operator() ( std::ostream& _sink ) const
    {
      PutString( _sink, mnemonic );
      if (not implicit_size) PutChar( _sink, ("bwlq"[SB<OPSIZE/8>::begin]) );
      PutChar( _sink, ' ' );
    }
  };
  
  struct DisasmFPR : public DisasmObject
  {
    DisasmFPR( uint8_t _reg ) : reg( _reg ) {} uint8_t reg;
    
    void operator() ( std::ostream& _sink ) const;
  };
  
  struct DisasmBytes : public DisasmObject
  {
    DisasmBytes( uint8_t const* _bytes, unsigned _length ) : bytes( _bytes ), length( _length ) {} uint8_t const* bytes; unsigned length;
    
    void operator() ( std::ostream& _sink ) const;
  };

} // end of namespace intel
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_INTEL_DISASM_HH__
