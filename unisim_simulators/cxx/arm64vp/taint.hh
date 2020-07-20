/*
 *  Copyright (c) 2019-2020,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#include <limits>
#include <inttypes.h>

#ifndef __ARM64VP_TAINT_HH__
#define __ARM64VP_TAINT_HH__

#include <inttypes.h>

template <typename Bool> struct AssertBool {};
template <>              struct AssertBool<bool> { static void check() {} };

template <bool test> struct StaticAssert {};
template <> struct StaticAssert<true> { static void check() {}; };

template <class T, class U>  struct CmpTypes { static bool const same = false; };

template <class T>  struct CmpTypes<T,T> { static bool const same = true; };

template <typename T> struct TX {};
template <> struct TX< uint8_t> { typedef  uint8_t as_mask; };
template <> struct TX<uint16_t> { typedef uint16_t as_mask; };
template <> struct TX<uint32_t> { typedef uint32_t as_mask; };
template <> struct TX<uint64_t> { typedef uint64_t as_mask; };
template <> struct TX<  int8_t> { typedef  uint8_t as_mask; };
template <> struct TX< int16_t> { typedef uint16_t as_mask; };
template <> struct TX< int32_t> { typedef uint32_t as_mask; };
template <> struct TX< int64_t> { typedef uint64_t as_mask; };
template <> struct TX<   float> { typedef uint32_t as_mask; };
template <> struct TX<  double> { typedef uint64_t as_mask; };

template <typename DST, typename SRC> struct CastUBits { TX<DST> process( TX<SRC> mask ) { return DST(SRC(mask)); } };
template <typename SRC> struct CastUBits<float,SRC> { uint32_t process( TX<SRC> mask ) { return mask ? -1 : 0; } };
template <typename SRC> struct CastUBits<double,SRC> { uint64_t process( TX<SRC> mask ) { return mask ? -1 : 0; } };
template <typename DST> struct CastUBits<DST,float> { TX<DST> process( uint32_t mask ) { return mask ? -1 : 0; } };
template <typename DST> struct CastUBits<DST,double> { TX<DST> process( uint64_t mask ) { return mask ? -1 : 0; } };
template <> struct CastUBits<float,float> { uint32_t process( uint32_t mask ) { return mask; } };
template <> struct CastUBits<double,double> { uint64_t process( uint64_t mask ) { return mask; } };

template <typename VALUE_TYPE>
struct TaintedValue
{
  typedef VALUE_TYPE               value_type;
  typedef TaintedValue<value_type>  this_type;
  typedef TX<value_type>           ubits_type;

  value_type value; /* concrete value */
  ubits_type ubits; /* uninitialized bits */
  
  TaintedValue() : value(), ubits(-1) {}

  TaintedValue(value_type _value, ubits_type _ubits) : value(_value), ubits(_ubits) {}
  explicit TaintedValue( value_type _value ) : value(_value), ubits(0) {}

  template <typename SRC_VALUE_TYPE>
  explicit TaintedValue( TaintedValue<SRC_VALUE_TYPE> const& other )
    : value(other.value), ubits(CastUBits<VALUE_TYPE, SRC_VALUE_TYPE>::process(other.ubits))
  {}
  
  static bool const is_signed = std::numeric_limits<value_type>::is_signed;

  template <typename SHT> this_type operator << (SHT sh) const { return this_type(value << sh, ubits << sh); }
  template <typename SHT> this_type operator >> (SHT sh) const { return this_type(value >> sh, ubits >> sh); }
  
  template <typename SHT> this_type& operator <<= (SHT sh) { value <<= sh; ubits <<= sh; return *this; }
  template <typename SHT> this_type& operator >>= (SHT sh) { value >>= sh; ubits >>= sh; return *this; }

  template <typename SHT> this_type operator << (TaintedValue<SHT> const& sh) const
  { return this_type(value << sh.value, sh.ubits ? -1 : (ubits << sh.value) ); }
  template <typename SHT> this_type operator >> (TaintedValue<SHT> const& sh) const
  { return this_type(value >> sh.value, sh.ubits ? -1 : (ubits >> sh.value) ); }

  this_type operator - () const { return this_type( -value, ubits ? -1 : 0 ); }
  this_type operator ~ () const { return this_type( ~value, ubits ); }

  this_type& operator += ( this_type const& other ) { value += other.value; ubits = (ubits or other.ubits) ? -1 : 0; return *this; }
  this_type& operator -= ( this_type const& other ) { value -= other.value; ubits = (ubits or other.ubits) ? -1 : 0; return *this; }
  this_type& operator *= ( this_type const& other ) { value *= other.value; ubits = (ubits or other.ubits) ? -1 : 0; return *this; }
  this_type& operator /= ( this_type const& other ) { value /= other.value; ubits = (ubits or other.ubits) ? -1 : 0; return *this; }
  this_type& operator %= ( this_type const& other ) { value %= other.value; ubits = (ubits or other.ubits) ? -1 : 0; return *this; }
  
  this_type& operator ^= ( this_type const& other ) { value ^= other.value; ubits |= other.ubits; return *this; }
  this_type& operator &= ( this_type const& other ) { value &= other.value; ubits |= other.ubits; return *this; }
  this_type& operator |= ( this_type const& other ) { value |= other.value; ubits |= other.ubits; return *this; }

  this_type operator + ( this_type const& other ) const { return this_type( value + other.value, (ubits or other.ubits) ? -1 : 0 ); }
  this_type operator - ( this_type const& other ) const { return this_type( value - other.value, (ubits or other.ubits) ? -1 : 0 ); }
  this_type operator * ( this_type const& other ) const { return this_type( value * other.value, (ubits or other.ubits) ? -1 : 0 ); }
  this_type operator / ( this_type const& other ) const { return this_type( value / other.value, (ubits or other.ubits) ? -1 : 0 ); }
  this_type operator % ( this_type const& other ) const { return this_type( value % other.value, (ubits or other.ubits) ? -1 : 0 ); }
  
  this_type operator ^ ( this_type const& other ) const { return this_type( value ^ other.value, ubits | other.ubits ); }
  this_type operator & ( this_type const& other ) const { return this_type( value & other.value, ubits | other.ubits ); }
  this_type operator | ( this_type const& other ) const { return this_type( value | other.value, ubits | other.ubits ); }
};

#endif /* __ARM64VP_TAINT_HH__ */
