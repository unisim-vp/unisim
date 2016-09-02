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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_INTEL_TYPES_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_INTEL_TYPES_HH__

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace intel {

  typedef uint8_t     u8_t;
  typedef uint16_t    u16_t;
  typedef uint32_t    u32_t;
  typedef uint64_t    u64_t;
  typedef int8_t      s8_t;
  typedef int16_t     s16_t;
  typedef int32_t     s32_t;
  typedef int64_t     s64_t;
  typedef bool        bit_t;
  typedef float       f32_t;
  typedef double      f64_t;
  typedef long double f80_t;
  typedef bool     mkbool;
  typedef int      mkint;

  template <typename T> struct tpinfo {};
  template <> struct tpinfo< u8_t> { typedef  s8_t stype; typedef  u8_t utype; typedef u16_t twice; enum { is_signed = 0, bitsize =  8 }; };
  template <> struct tpinfo< s8_t> { typedef  s8_t stype; typedef  u8_t utype; typedef s16_t twice; enum { is_signed = 1, bitsize =  8 }; };
  template <> struct tpinfo<u16_t> { typedef s16_t stype; typedef u16_t utype; typedef u32_t twice; enum { is_signed = 0, bitsize = 16 }; };
  template <> struct tpinfo<s16_t> { typedef s16_t stype; typedef u16_t utype; typedef s32_t twice; enum { is_signed = 1, bitsize = 16 }; };
  template <> struct tpinfo<u32_t> { typedef s32_t stype; typedef u32_t utype; typedef u64_t twice; enum { is_signed = 0, bitsize = 32 }; };
  template <> struct tpinfo<s32_t> { typedef s32_t stype; typedef u32_t utype; typedef s64_t twice; enum { is_signed = 1, bitsize = 32 }; };
  // TODO: should handle twice of 64-bit types
  template <typename T>
  struct Twice
  {
    Twice() {}
    Twice( T const& val ) {}
    template <typename X>
    Twice operator << (X) { return Twice(); }
    template <typename X>
    Twice operator >> (X) { return Twice(); }
    Twice operator | (Twice const& val) { return Twice(); }
    
    Twice& operator |= (Twice const& val) { return *this; }
    operator T () { return T(); }
  };
  template <> struct tpinfo<u64_t> { typedef s64_t stype; typedef u64_t utype; typedef Twice<u64_t> twice; enum { is_signed = 0, bitsize = 64 }; };
  template <> struct tpinfo<s64_t> { typedef s64_t stype; typedef u64_t utype; typedef Twice<s64_t> twice; enum { is_signed = 1, bitsize = 64 }; };

  template <unsigned OPSIZE> struct TypeFor {};
  template <> struct TypeFor<8> { typedef int8_t s; typedef uint8_t u; };
  template <> struct TypeFor<16> { typedef int16_t s; typedef uint16_t u; };
  template <> struct TypeFor<32> { typedef int32_t s; typedef uint32_t u; typedef float f; };
  template <> struct TypeFor<64> { typedef int64_t s; typedef uint64_t u; typedef double f; };
  template <> struct TypeFor<80> { typedef long double f; };

  template <unsigned SIZE> struct CTypeFor {};
  template <> struct CTypeFor<8> { typedef int8_t s; typedef uint8_t u; };
  template <> struct CTypeFor<16> { typedef int16_t s; typedef uint16_t u; };
  template <> struct CTypeFor<32> { typedef int32_t s; typedef uint32_t u; typedef float f; };
  template <> struct CTypeFor<64> { typedef int64_t s; typedef uint64_t u; typedef double f; };
  template <> struct CTypeFor<80> { typedef double f; };
  
} // end of namespace intel
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_INTEL_TYPES_HH__
