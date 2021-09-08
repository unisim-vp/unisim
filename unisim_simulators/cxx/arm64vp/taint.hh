/*
 *  Copyright (c) 2019-2021,
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

#ifndef __ARM64VP_TAINT_HH__
#define __ARM64VP_TAINT_HH__

#include <unisim/util/arithmetic/arithmetic.hh>
#include <algorithm>
#include <limits>
#include <iosfwd>
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
template <> struct TX<    bool> { typedef bool as_mask; };

template <typename DST, typename SRC> struct CastUBits { typedef typename TX<SRC>::as_mask mask_t; static typename TX<DST>::as_mask process( mask_t mask, mask_t bits ) { return DST(SRC(mask)); } };
template <typename SRC> struct CastUBits<float,SRC> { typedef typename TX<SRC>::as_mask mask_t; static uint32_t process( mask_t mask, mask_t bits ) { return mask ? -1 : 0; } };
template <typename SRC> struct CastUBits<double,SRC> { typedef typename TX<SRC>::as_mask mask_t; static uint64_t process( mask_t mask, mask_t bits ) { return mask ? -1 : 0; } };
template <typename SRC> struct CastUBits<bool,SRC> { typedef typename TX<SRC>::as_mask mask_t; static bool process( mask_t mask, mask_t bits ) { return mask and not (~mask & bits); } };
template <typename DST> struct CastUBits<DST,float> { static typename TX<DST>::as_mask process( uint32_t mask, uint32_t bits ) { return mask ? -1 : 0; } };
template <typename DST> struct CastUBits<DST,double> { static typename TX<DST>::as_mask process( uint64_t mask, uint64_t bits ) { return mask ? -1 : 0; } };
template <> struct CastUBits<float,double> { static uint32_t process( uint32_t mask, uint32_t bits ) { return mask ? -1 : 0; } };
template <> struct CastUBits<double,float> { static uint64_t process( uint64_t mask, uint64_t bits ) { return mask ? -1 : 0; } };
template <> struct CastUBits<float,float> { static uint32_t process( uint32_t mask, uint32_t bits ) { return mask; } };
template <> struct CastUBits<double,double> { static uint64_t process( uint64_t mask, uint64_t bits ) { return mask; } };
template <> struct CastUBits<bool,bool> { static bool process( bool mask, bool bits ) { return mask; } };

template <typename VALUE_TYPE>
struct TaintedValue
{ 
  typedef TaintedValue<VALUE_TYPE>         this_type;
  typedef VALUE_TYPE                       value_type;
  typedef typename TX<value_type>::as_mask ubits_type;

  value_type value; /* concrete value */
  ubits_type ubits; /* uninitialized bits */
  
  ubits_type value_as_mask() const { return *reinterpret_cast<ubits_type const*>(&value); }
    
  TaintedValue() : value(), ubits(-1) {}

  TaintedValue(value_type _value, ubits_type _ubits) : value(_value), ubits(_ubits) {}
  explicit TaintedValue( value_type _value ) : value(_value), ubits(0) {}

  template <typename SRC_VALUE_TYPE>
  explicit TaintedValue( TaintedValue<SRC_VALUE_TYPE> const& src )
    : value(src.value), ubits(CastUBits<VALUE_TYPE, SRC_VALUE_TYPE>::process(src.ubits, src.value_as_mask()))
  {}
  
  static bool const is_signed = std::numeric_limits<value_type>::is_signed;

  template <typename SHT> this_type operator << (SHT sh) const { return this_type(value << sh, ubits << sh); }
  template <typename SHT> this_type operator >> (SHT sh) const { return this_type(value >> sh, value_type(ubits) >> sh); }
  
  template <typename SHT> this_type& operator <<= (SHT sh) { value <<= sh; ubits <<= sh; return *this; }
  template <typename SHT> this_type& operator >>= (SHT sh) { value >>= sh; ubits = value_type(ubits) >> sh; return *this; }

  template <typename SHT> this_type operator << (TaintedValue<SHT> const& sh) const
  { return this_type(value << sh.value, sh.ubits ? -1 : (ubits << sh.value) ); }
  template <typename SHT> this_type operator >> (TaintedValue<SHT> const& sh) const
  { return this_type(value >> sh.value, sh.ubits ? -1 : (value_type(ubits) >> sh.value) ); }

  this_type operator - () const { return this_type( -value, ubits ? -1 : 0 ); }
  this_type operator ~ () const { return this_type( ~value, ubits ); }

  this_type& operator += ( this_type const& other ) { value += other.value; ubits = (ubits or other.ubits) ? -1 : 0; return *this; }
  this_type& operator -= ( this_type const& other ) { value -= other.value; ubits = (ubits or other.ubits) ? -1 : 0; return *this; }
  this_type& operator *= ( this_type const& other ) { value *= other.value; ubits = (ubits or other.ubits) ? -1 : 0; return *this; }
  this_type& operator /= ( this_type const& other ) { value /= other.value; ubits = (ubits or other.ubits) ? -1 : 0; return *this; }
  this_type& operator %= ( this_type const& other ) { value %= other.value; ubits = (ubits or other.ubits) ? -1 : 0; return *this; }
  
  this_type& operator ^= ( this_type const& other ) { value ^= other.value; ubits |= other.ubits; return *this; }
  this_type& operator &= ( this_type const& other ) { value &= other.value; ubits = (ubits | other.ubits) & (value | ubits) & (other.value | other.ubits); return *this; }
  this_type& operator |= ( this_type const& other ) { value |= other.value; ubits = (ubits | other.ubits) & (~value | ubits) & (~other.value | other.ubits); return *this; }

  this_type operator + ( this_type const& other ) const { return this_type( value + other.value, (ubits or other.ubits) ? -1 : 0 ); }
  this_type operator - ( this_type const& other ) const { return this_type( value - other.value, (ubits or other.ubits) ? -1 : 0 ); }
  this_type operator * ( this_type const& other ) const { return this_type( value * other.value, (ubits or other.ubits) ? -1 : 0 ); }
  this_type operator / ( this_type const& other ) const { return this_type( value / other.value, (ubits or other.ubits) ? -1 : 0 ); }
  this_type operator % ( this_type const& other ) const { return this_type( value % other.value, (ubits or other.ubits) ? -1 : 0 ); }
  
  this_type operator ^ ( this_type const& other ) const { return this_type( value ^ other.value, ubits | other.ubits ); }
  this_type operator & ( this_type const& other ) const { return this_type( value & other.value, (ubits | other.ubits) & (value | ubits) & (other.value | other.ubits) ); }
  this_type operator | ( this_type const& other ) const { return this_type( value | other.value, (ubits | other.ubits) & (~value | ubits) & (~other.value | other.ubits) ); }

  TaintedValue<bool> operator == ( this_type const& r ) const { return TaintedValue<bool>( value == r.value, (ubits | r.ubits) and not (~ubits & ~r.ubits & (value ^ r.value)) ); }
  TaintedValue<bool> operator != ( this_type const& r ) const { return TaintedValue<bool>( value != r.value, (ubits | r.ubits) and not (~ubits & ~r.ubits & (value ^ r.value)) ); }
  TaintedValue<bool> operator <= ( this_type const& other ) const { return TaintedValue<bool>( value <= other.value, ubits or other.ubits ); }
  TaintedValue<bool> operator >= ( this_type const& other ) const { return TaintedValue<bool>( value >= other.value, ubits or other.ubits ); }
  TaintedValue<bool> operator < ( this_type const& other ) const  { return TaintedValue<bool>( value <  other.value, ubits or other.ubits ); }
  TaintedValue<bool> operator > ( this_type const& other ) const  { return TaintedValue<bool>( value >  other.value, ubits or other.ubits ); }

  TaintedValue<bool> operator ! () const
  { AssertBool<value_type>::check(); return TaintedValue<bool>( not value, ubits ); }

  TaintedValue<bool> operator && ( TaintedValue<bool> const& other ) const
  { AssertBool<value_type>::check(); return TaintedValue<bool>( value and other.value, ubits or other.ubits ); }

  TaintedValue<bool> operator || ( TaintedValue<bool> const& other ) const
  { AssertBool<value_type>::check(); return TaintedValue<bool>( value or other.value, ubits or other.ubits ); }
};

template <typename T>
struct TaintedTypeInfo
{
  enum { bytecount = sizeof (typename T::ubits_type) };
  static void ToBytes( TaintedValue<uint8_t>* dst, T const& src )
  {
    //typedef typename TX<typename T::value_type>::as_mask bits_type;
    typedef typename T::ubits_type ubits_type;

    ubits_type value = src.value_as_mask(), ubits = src.ubits;
    
    for (unsigned idx = 0; idx < sizeof (ubits_type); ++idx)
      {
        dst[idx].value = value & 0xff; value >>= 8;
        dst[idx].ubits = ubits & 0xff; ubits >>= 8;
      }
  }
  static void FromBytes( T& dst, TaintedValue<uint8_t> const* src )
  {
    typedef typename T::value_type value_type;
    //typedef typename TX<value_type>::as_mask bits_type;
    typedef typename T::ubits_type ubits_type;

    ubits_type value = 0, ubits = 0;
    for (unsigned idx = sizeof (ubits_type); idx-- > 0;)
      {
        value <<= 8; value |= ubits_type( src[idx].value );
        ubits <<= 8; ubits |= ubits_type( src[idx].ubits );
      }
    dst.value = *reinterpret_cast<value_type const*>(&value);
    dst.ubits = ubits;
  }
  static void Destroy( T& obj ) { obj.~T(); }
  static void Allocate( T& obj ) { new (&obj) T(); }
};

template <typename UTP>
UTP RotateRight( UTP const& value, uint8_t sh )
{
  return UTP( unisim::util::arithmetic::RotateRight(value.value, sh), unisim::util::arithmetic::RotateRight(value.ubits, sh) );
}
template <typename UTP, typename STP>
UTP RotateRight( UTP const& value, STP const& sh )
{
  return UTP( unisim::util::arithmetic::RotateRight(value.value, sh.value), sh.ubits ? -1 : unisim::util::arithmetic::RotateRight(value.ubits, sh.value) );
}

template <typename UTP>
UTP BitScanReverse( UTP const& value )
{
  auto bit = unisim::util::arithmetic::BitScanReverse(value.value);
  return UTP( bit, (value.ubits >> bit) ? -1 : 0 );
}

extern void Print( std::ostream& sink, unsigned minlength, unsigned radix, uint64_t vbits, uint64_t ubits );
extern void PrintBin( std::ostream& sink, uint64_t vbits, uint64_t ubits );

template <typename T>
void Print( std::ostream& sink, TaintedValue<T> const& tv )
{
  typedef typename TX<T>::as_mask bits;
  bits value = *reinterpret_cast<bits const*>(&tv.value);
  PrintBin(sink, value, tv.ubits);
}

extern void PrintHex( std::ostream& sink, unsigned ml, uint64_t vbits, uint64_t ubits );

template <typename T>
void PrintHex( std::ostream& sink, unsigned ml, TaintedValue<T> const& tv )
{
  typedef typename TX<T>::as_mask bits;
  bits value = *reinterpret_cast<bits const*>(&tv.value);
  PrintHex(sink, ml, value, tv.ubits);
}

TaintedValue<float> round( TaintedValue<float> const& _value );
TaintedValue<double> round( TaintedValue<double> const& _value );

TaintedValue<float> floor( TaintedValue<float> const& _value );
TaintedValue<double> floor( TaintedValue<double> const& _value );

TaintedValue<float> ceil( TaintedValue<float> const& _value );
TaintedValue<double> ceil( TaintedValue<double> const& _value );

TaintedValue<float> trunc( TaintedValue<float> const& _value );
TaintedValue<double> trunc( TaintedValue<double> const& _value );

TaintedValue<float> fabs( TaintedValue<float> const& _value );
TaintedValue<double> fabs( TaintedValue<double> const& _value );

TaintedValue<float> sqrt( TaintedValue<float> const& _value );
TaintedValue<double> sqrt( TaintedValue<double> const& _value );

template <typename T>
TaintedValue<T> PopCount(TaintedValue<T> const& v) { return TaintedValue<T>(unisim::util::arithmetic::PopCount(v.value), v.ubits ? -1 : 0); }

template <typename T>
TaintedValue<T> Minimum(TaintedValue<T> const& l, TaintedValue<T> const& r)
{ return TaintedValue<T>(std::min(l.value, r.value), (l.ubits or r.ubits) ? -1 : 0 ); }

template <typename T>
TaintedValue<T> Maximum(TaintedValue<T> const& l, TaintedValue<T> const& r)
{ return TaintedValue<T>(std::max(l.value, r.value), (l.ubits or r.ubits) ? -1 : 0 ); }

#endif /* __ARM64VP_TAINT_HH__ */
