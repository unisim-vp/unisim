/*
 *  Copyright (c) 2019-2023,
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
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __ARM64VP_TAINT_HH__
#define __ARM64VP_TAINT_HH__

#if HAVE_CONFIG_H
#include "config.h"
#endif

#if HAVE_SOFTFLOAT_EMU
#include <unisim/util/floating_point/softfloat_emu/softfloat_emu.hh>
#else
#include <unisim/util/floating_point/floating_point.hh>
#endif
#include <unisim/component/cxx/processor/arm/isa/execute.hh>
#include <unisim/component/cxx/processor/arm/isa/arm64/execute.hh>
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

template <typename SRC> bool EqUBits( typename TX<SRC>::as_mask ubits, SRC lhs, SRC rhs ) { return ubits and not (~ubits & (lhs ^ rhs)); }
inline bool EqUBits( uint32_t ubits, float lhs, float rhs ) { return ubits; }
inline bool EqUBits( uint64_t ubits, double lhs, double rhs ) { return ubits; }

#if HAVE_FLOAT16
template <> struct TX<_Float16> { typedef uint16_t as_mask; };
template <> struct CastUBits<_Float16,double> { static uint16_t process( uint16_t mask, uint16_t bits ) { return mask ? -1 : 0; } };
template <> struct CastUBits<float,_Float16> { static uint64_t process( uint32_t mask, uint32_t bits ) { return mask ? -1 : 0; } };
template <> struct CastUBits<double,_Float16> { static uint64_t process( uint64_t mask, uint64_t bits ) { return mask ? -1 : 0; } };
template <> struct CastUBits<_Float16,float> { static uint16_t process( uint16_t mask, uint16_t bits ) { return mask ? -1 : 0; } };
template <> struct CastUBits<_Float16,_Float16> { static uint16_t process( uint16_t mask, uint16_t bits ) { return mask; } };
inline bool EqUBits( uint16_t ubits, _Float16 lhs, _Float16 rhs ) { return ubits; }
#endif

template <typename DST, typename SRC>
DST TypePunning( SRC value )
{
  union { DST as_dst; SRC as_src; } tmp;
  tmp.as_src = value;
  return tmp.as_dst;
}

struct TVCtor {};

template <typename VALUE_TYPE>
struct TaintedValue;

template <typename VALUE_TYPE>
inline std::ostream& operator << (std::ostream& stream, TaintedValue<VALUE_TYPE> const& tv);

template <typename VALUE_TYPE>
struct TaintedValue
{ 
  typedef TaintedValue<VALUE_TYPE>         this_type;
  typedef VALUE_TYPE                       value_type;
  typedef typename TX<value_type>::as_mask ubits_type;

  value_type value; /* concrete value */
  ubits_type ubits; /* uninitialized bits */
  
  ubits_type value_as_mask() const { return TypePunning<ubits_type>(value); }
    
  TaintedValue() : value(), ubits(-1) {}
  TaintedValue(TVCtor, value_type _value, ubits_type _ubits) : value(_value), ubits(_ubits) {}
  explicit TaintedValue( value_type _value ) : value(_value), ubits(0) {}

  template <typename SRC_VALUE_TYPE>
  explicit TaintedValue( TaintedValue<SRC_VALUE_TYPE> const& src )
    : value(src.value), ubits(CastUBits<VALUE_TYPE, SRC_VALUE_TYPE>::process(src.ubits, src.value_as_mask()))
  {}
  
  static bool const is_signed = std::numeric_limits<value_type>::is_signed;

  template <typename SHT> this_type operator << (SHT sh) const { return this_type(TVCtor(), value << sh, ubits << sh); }
  template <typename SHT> this_type operator >> (SHT sh) const { return this_type(TVCtor(), value >> sh, value_type(ubits) >> sh); }
  
  template <typename SHT> this_type& operator <<= (SHT sh) { value <<= sh; ubits <<= sh; return *this; }
  template <typename SHT> this_type& operator >>= (SHT sh) { value >>= sh; ubits = value_type(ubits) >> sh; return *this; }

  template <typename SHT> this_type operator << (TaintedValue<SHT> const& sh) const
  { return this_type(TVCtor(), value << sh.value, sh.ubits ? -1 : (ubits << sh.value) ); }
  template <typename SHT> this_type operator >> (TaintedValue<SHT> const& sh) const
  { return this_type(TVCtor(), value >> sh.value, sh.ubits ? -1 : (value_type(ubits) >> sh.value) ); }

  template <typename SHT> this_type& operator <<= (TaintedValue<SHT> const& sh) { value <<= sh.value; ubits = sh.ubits ? -1 : (ubits << sh.value); return *this; }
  template <typename SHT> this_type& operator >>= (TaintedValue<SHT> const& sh) { value >>= sh.value; ubits = sh.ubits ? -1 : (value_type(ubits) >> sh.value); return *this; }

  this_type operator - () const { return this_type( TVCtor(), -value, ubits ? -1 : 0 ); }
  this_type operator ~ () const { return this_type( TVCtor(), ~value, ubits ); }

  this_type& operator += ( this_type const& other ) { value += other.value; ubits = (ubits or other.ubits) ? -1 : 0; return *this; }
  this_type& operator -= ( this_type const& other ) { value -= other.value; ubits = (ubits or other.ubits) ? -1 : 0; return *this; }
  this_type& operator *= ( this_type const& other ) { value *= other.value; ubits = (ubits or other.ubits) ? -1 : 0; return *this; }
  this_type& operator /= ( this_type const& other ) { value /= other.value; ubits = (ubits or other.ubits) ? -1 : 0; return *this; }
  this_type& operator %= ( this_type const& other ) { value %= other.value; ubits = (ubits or other.ubits) ? -1 : 0; return *this; }
  
  this_type& operator ^= ( this_type const& other ) { value ^= other.value; ubits |= other.ubits; return *this; }
  this_type& operator &= ( this_type const& other ) { value &= other.value; ubits = (ubits | other.ubits) & (value | ubits) & (other.value | other.ubits); return *this; }
  this_type& operator |= ( this_type const& other ) { value |= other.value; ubits = (ubits | other.ubits) & (~value | ubits) & (~other.value | other.ubits); return *this; }

  this_type operator + ( this_type const& other ) const { return this_type( TVCtor(), value + other.value, (ubits or other.ubits) ? -1 : 0 ); }
  this_type operator - ( this_type const& other ) const { return this_type( TVCtor(), value - other.value, (ubits or other.ubits) ? -1 : 0 ); }
  this_type operator * ( this_type const& other ) const { return this_type( TVCtor(), value * other.value, (ubits or other.ubits) ? -1 : 0 ); }
  this_type operator / ( this_type const& other ) const { return this_type( TVCtor(), value / other.value, (ubits or other.ubits) ? -1 : 0 ); }
  this_type operator % ( this_type const& other ) const { return this_type( TVCtor(), value % other.value, (ubits or other.ubits) ? -1 : 0 ); }
  
  this_type operator ^ ( this_type const& other ) const { return this_type( TVCtor(), value ^ other.value, ubits | other.ubits ); }
  this_type operator & ( this_type const& other ) const { return this_type( TVCtor(), value & other.value, (ubits | other.ubits) & (value | ubits) & (other.value | other.ubits) ); }
  this_type operator | ( this_type const& other ) const { return this_type( TVCtor(), value | other.value, (ubits | other.ubits) & (~value | ubits) & (~other.value | other.ubits) ); }

  TaintedValue<bool> operator == ( this_type const& r ) const { return TaintedValue<bool>( TVCtor(), value == r.value, EqUBits(ubits|r.ubits, value, r.value) ); }
  TaintedValue<bool> operator != ( this_type const& r ) const { return TaintedValue<bool>( TVCtor(), value != r.value, EqUBits(ubits|r.ubits, value, r.value) ); }
  TaintedValue<bool> operator <= ( this_type const& other ) const { return TaintedValue<bool>( TVCtor(), value <= other.value, ubits or other.ubits ); }
  TaintedValue<bool> operator >= ( this_type const& other ) const { return TaintedValue<bool>( TVCtor(), value >= other.value, ubits or other.ubits ); }
  TaintedValue<bool> operator < ( this_type const& other ) const  { return TaintedValue<bool>( TVCtor(), value <  other.value, ubits or other.ubits ); }
  TaintedValue<bool> operator > ( this_type const& other ) const  { return TaintedValue<bool>( TVCtor(), value >  other.value, ubits or other.ubits ); }

  TaintedValue<bool> operator ! () const
  { AssertBool<value_type>::check(); return TaintedValue<bool>( TVCtor(), not value, ubits ); }

  TaintedValue<bool> operator && ( TaintedValue<bool> const& other ) const
  { AssertBool<value_type>::check(); return TaintedValue<bool>( TVCtor(), value and other.value, ubits or other.ubits ); }

  TaintedValue<bool> operator || ( TaintedValue<bool> const& other ) const
  { AssertBool<value_type>::check(); return TaintedValue<bool>( TVCtor(), value or other.value, ubits or other.ubits ); }
};

template <typename VALUE_TYPE>
inline std::ostream& operator << (std::ostream& stream, TaintedValue<VALUE_TYPE> const& tv)
{
  return stream << tv.value;
}

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
    dst.value = TypePunning<value_type>(value);
    dst.ubits = ubits;
  }
  static void Destroy( T& obj ) { obj.~T(); }
  static void Allocate( T& obj ) { new (&obj) T(); }
};

template <typename UTP>
UTP RotateRight( UTP const& value, uint8_t sh )
{
  return UTP( TVCtor(), unisim::util::arithmetic::RotateRight(value.value, sh), unisim::util::arithmetic::RotateRight(value.ubits, sh) );
}
template <typename UTP, typename STP>
UTP RotateRight( UTP const& value, STP const& sh )
{
  return UTP( TVCtor(), unisim::util::arithmetic::RotateRight(value.value, sh.value), sh.ubits ? -1 : unisim::util::arithmetic::RotateRight(value.ubits, sh.value) );
}

template <typename UTP>
UTP BitScanReverse( UTP const& value )
{
  auto bit = unisim::util::arithmetic::BitScanReverse(value.value);
  return UTP( TVCtor(), bit, (value.ubits >> bit) ? -1 : 0 );
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

template <typename T>
TaintedValue<T>
MakeTaintedValue(T value, typename TaintedValue<T>::ubits_type ubits ) { return TaintedValue<T>(TVCtor(), value, ubits); }

#if HAVE_FLOAT16
// functions for _Float16
template <> inline TaintedValue<_Float16> FromUnpacked<TaintedValue<_Float16> >( UnpackedFloat const& unpacked ) { return MakeTaintedValue(FromUnpacked<_Float16>(unpacked), 0); }
template <> inline TaintedValue<_Float16> FromPacked<TaintedValue<_Float16>, TaintedValue<uint16_t> >( TaintedValue<uint16_t> a ) { return MakeTaintedValue(FromPacked<_Float16>(a.value), a.ubits ? -1 : 0); }
template <> inline TaintedValue<_Float16> FConvert<TaintedValue<_Float16>, TaintedValue<_Float16> >( TaintedValue<_Float16> a ) { return a; }
template <> inline TaintedValue<_Float16> FConvert<TaintedValue<_Float16>, TaintedValue<float> >( TaintedValue<float> a ) { return MakeTaintedValue(FConvert<_Float16, float>(a.value), a.ubits ? -1 : 0); }
template <> inline TaintedValue<_Float16> FConvert<TaintedValue<_Float16>, TaintedValue<double> >( TaintedValue<double> a ) { return MakeTaintedValue(FConvert<_Float16, double>(a.value), a.ubits ? -1 : 0); }
template <> inline TaintedValue<uint16_t> ToInt( TaintedValue<_Float16> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<uint16_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<uint32_t> ToInt( TaintedValue<_Float16> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<uint32_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<uint64_t> ToInt( TaintedValue<_Float16> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<uint64_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<int16_t> ToInt( TaintedValue<_Float16> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<int16_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<int32_t> ToInt( TaintedValue<_Float16> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<int32_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<int64_t> ToInt( TaintedValue<_Float16> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<int64_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<uint16_t> ToIntRoundMinMag( TaintedValue<_Float16> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<uint16_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<uint32_t> ToIntRoundMinMag( TaintedValue<_Float16> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<uint32_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<uint64_t> ToIntRoundMinMag( TaintedValue<_Float16> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<uint64_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<int16_t> ToIntRoundMinMag( TaintedValue<_Float16> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<int16_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<int32_t> ToIntRoundMinMag( TaintedValue<_Float16> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<int32_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<int64_t> ToIntRoundMinMag( TaintedValue<_Float16> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<int64_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<_Float16> DefaultNaN() { return TaintedValue<_Float16>(DefaultNaN<_Float16>()); }
TaintedValue<uint16_t> ToPacked( TaintedValue<_Float16> a );
void ToUnpacked( UnpackedFloat& unpacked, TaintedValue<_Float16> a );
TaintedValue<_Float16> RoundToInt( TaintedValue<_Float16> a, uint_fast8_t rmode, bool exact );
TaintedValue<_Float16> FMulAdd( TaintedValue<_Float16> a, TaintedValue<_Float16> b, TaintedValue<_Float16> c);
TaintedValue<_Float16> FRem( TaintedValue<_Float16> a, TaintedValue<_Float16> b );
TaintedValue<_Float16> FSqrt( TaintedValue<_Float16> a );
TaintedValue<_Float16> FAbs( TaintedValue<_Float16> a );
TaintedValue<_Float16> FMin( TaintedValue<_Float16> a, TaintedValue<_Float16> b );
TaintedValue<_Float16> FMinNumber( TaintedValue<_Float16> a, TaintedValue<_Float16> b );
TaintedValue<_Float16> FMax( TaintedValue<_Float16> a, TaintedValue<_Float16> b );
TaintedValue<_Float16> FMaxNumber( TaintedValue<_Float16> a, TaintedValue<_Float16> b );
TaintedValue<bool> IsNaN( TaintedValue<_Float16> a );
TaintedValue<bool> IsDenormal( TaintedValue<_Float16> a );
TaintedValue<bool> IsSignaling( TaintedValue<_Float16> a );
TaintedValue<_Float16> ClearSignaling( TaintedValue<_Float16> a );
TaintedValue<_Float16> Zeroes( TaintedValue<_Float16> a );
TaintedValue<bool> IsZero( TaintedValue<_Float16> a );
TaintedValue<bool> IsNeg( TaintedValue<_Float16> a );
TaintedValue<bool> IsInf( TaintedValue<_Float16> a );
#endif

// functions for float
template <> inline TaintedValue<float> FromUnpacked<TaintedValue<float> >( UnpackedFloat const& unpacked ) { return MakeTaintedValue(FromUnpacked<float>(unpacked), 0); }
template <> inline TaintedValue<float> FromPacked<TaintedValue<float>, TaintedValue<uint32_t> >( TaintedValue<uint32_t> a ) { return MakeTaintedValue(FromPacked<float>(a.value), a.ubits ? -1 : 0); }
template <> inline TaintedValue<float> FConvert<TaintedValue<float>, TaintedValue<float> >( TaintedValue<float> a ) { return a; }
template <> inline TaintedValue<float> FConvert<TaintedValue<float>, TaintedValue<double> >( TaintedValue<double> a ) { return MakeTaintedValue(FConvert<float, double>(a.value), a.ubits ? -1 : 0); }
template <> inline TaintedValue<uint16_t> ToInt( TaintedValue<float> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<uint16_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<uint32_t> ToInt( TaintedValue<float> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<uint32_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<uint64_t> ToInt( TaintedValue<float> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<uint64_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<int16_t> ToInt( TaintedValue<float> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<int16_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<int32_t> ToInt( TaintedValue<float> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<int32_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<int64_t> ToInt( TaintedValue<float> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<int64_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<uint16_t> ToIntRoundMinMag( TaintedValue<float> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<uint16_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<uint32_t> ToIntRoundMinMag( TaintedValue<float> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<uint32_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<uint64_t> ToIntRoundMinMag( TaintedValue<float> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<uint64_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<int16_t> ToIntRoundMinMag( TaintedValue<float> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<int16_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<int32_t> ToIntRoundMinMag( TaintedValue<float> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<int32_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<int64_t> ToIntRoundMinMag( TaintedValue<float> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<int64_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<float> DefaultNaN() { return TaintedValue<float>(DefaultNaN<float>()); }
TaintedValue<uint32_t> ToPacked( TaintedValue<float> a );
void ToUnpacked( UnpackedFloat& unpacked, TaintedValue<float> a );
#if HAVE_FLOAT16
template <> inline TaintedValue<float> FConvert<TaintedValue<float>, TaintedValue<_Float16> >( TaintedValue<_Float16> a ) { return MakeTaintedValue(FConvert<float, _Float16>(a.value), a.ubits ? -1 : 0); }
#endif
TaintedValue<float> RoundToInt( TaintedValue<float> a, uint_fast8_t rmode, bool exact );
TaintedValue<float> FMulAdd( TaintedValue<float> a, TaintedValue<float> b, TaintedValue<float> c);
TaintedValue<float> FRem( TaintedValue<float> a, TaintedValue<float> b );
TaintedValue<float> FSqrt( TaintedValue<float> a );
TaintedValue<float> FAbs( TaintedValue<float> a );
TaintedValue<float> FMin( TaintedValue<float> a, TaintedValue<float> b );
TaintedValue<float> FMinNumber( TaintedValue<float> a, TaintedValue<float> b );
TaintedValue<float> FMax( TaintedValue<float> a, TaintedValue<float> b );
TaintedValue<float> FMaxNumber( TaintedValue<float> a, TaintedValue<float> b );
TaintedValue<bool> IsNaN( TaintedValue<float> a );
TaintedValue<bool> IsDenormal( TaintedValue<float> a );
TaintedValue<bool> IsSignaling( TaintedValue<float> a );
TaintedValue<float> ClearSignaling( TaintedValue<float> a );
TaintedValue<float> Zeroes( TaintedValue<float> a );
TaintedValue<bool> IsZero( TaintedValue<float> a );
TaintedValue<bool> IsNeg( TaintedValue<float> a );
TaintedValue<bool> IsInf( TaintedValue<float> a );

// functions for double
template <> inline TaintedValue<double> FromUnpacked<TaintedValue<double> >( UnpackedFloat const& unpacked ) { return MakeTaintedValue(FromUnpacked<double>(unpacked), 0); }
template <> inline TaintedValue<double> FromPacked<TaintedValue<double>, TaintedValue<uint64_t> >( TaintedValue<uint64_t> a ) { return MakeTaintedValue(FromPacked<double>(a.value), a.ubits ? -1 : 0); }
#if HAVE_FLOAT16
template <> inline TaintedValue<double> FConvert<TaintedValue<double>, TaintedValue<_Float16> >( TaintedValue<_Float16> a ) { return MakeTaintedValue(FConvert<double, _Float16>(a.value), a.ubits ? -1 : 0); }
#endif
template <> inline TaintedValue<double> FConvert<TaintedValue<double>, TaintedValue<float> >( TaintedValue<float> a ) { return MakeTaintedValue(FConvert<double, float>(a.value), a.ubits ? -1 : 0); }
template <> inline TaintedValue<double> FConvert<TaintedValue<double>, TaintedValue<double> >( TaintedValue<double> a ) { return a; }
template <> inline TaintedValue<uint16_t> ToInt( TaintedValue<double> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<uint16_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<uint32_t> ToInt( TaintedValue<double> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<uint32_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<uint64_t> ToInt( TaintedValue<double> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<uint64_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<int16_t> ToInt( TaintedValue<double> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<int16_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<int32_t> ToInt( TaintedValue<double> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<int32_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<int64_t> ToInt( TaintedValue<double> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<int64_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<uint16_t> ToIntRoundMinMag( TaintedValue<double> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<uint16_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<uint32_t> ToIntRoundMinMag( TaintedValue<double> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<uint32_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<uint64_t> ToIntRoundMinMag( TaintedValue<double> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<uint64_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<int16_t> ToIntRoundMinMag( TaintedValue<double> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<int16_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<int32_t> ToIntRoundMinMag( TaintedValue<double> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<int32_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<int64_t> ToIntRoundMinMag( TaintedValue<double> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<int64_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> inline TaintedValue<double> DefaultNaN() { return TaintedValue<double>(DefaultNaN<double>()); }
TaintedValue<uint64_t> ToPacked( TaintedValue<double> a );
void ToUnpacked( UnpackedFloat& unpacked, TaintedValue<double> a );
TaintedValue<double> RoundToInt( TaintedValue<double> a, uint_fast8_t rmode, bool exact );
TaintedValue<double> FMulAdd( TaintedValue<double> a, TaintedValue<double> b, TaintedValue<double> c);
TaintedValue<double> FRem( TaintedValue<double> a, TaintedValue<double> b );
TaintedValue<double> FSqrt( TaintedValue<double> a );
TaintedValue<double> FAbs( TaintedValue<double> a );
TaintedValue<double> FMin( TaintedValue<double> a, TaintedValue<double> b );
TaintedValue<double> FMinNumber( TaintedValue<double> a, TaintedValue<double> b );
TaintedValue<double> FMax( TaintedValue<double> a, TaintedValue<double> b );
TaintedValue<double> FMaxNumber( TaintedValue<double> a, TaintedValue<double> b );
TaintedValue<bool> IsNaN( TaintedValue<double> a );
TaintedValue<bool> IsDenormal( TaintedValue<double> a );
TaintedValue<bool> IsSignaling( TaintedValue<double> a );
TaintedValue<double> ClearSignaling( TaintedValue<double> a );
TaintedValue<double> Zeroes( TaintedValue<double> a );
TaintedValue<bool> IsZero( TaintedValue<double> a );
TaintedValue<bool> IsNeg( TaintedValue<double> a );
TaintedValue<bool> IsInf( TaintedValue<double> a );

template <typename T>
TaintedValue<T> PopCount(TaintedValue<T> const& v) { return TaintedValue<T>(TVCtor(), unisim::util::arithmetic::PopCount(v.value), v.ubits ? -1 : 0); }

template <typename T>
TaintedValue<T> Minimum(TaintedValue<T> const& l, TaintedValue<T> const& r)
{ return TaintedValue<T>(TVCtor(), std::min(l.value, r.value), (l.ubits or r.ubits) ? -1 : 0 ); }

template <typename T>
TaintedValue<T> Maximum(TaintedValue<T> const& l, TaintedValue<T> const& r)
{ return TaintedValue<T>(TVCtor(), std::max(l.value, r.value), (l.ubits or r.ubits) ? -1 : 0 ); }

template <typename T>
class TaintedValueNumericLimits : public std::numeric_limits<T>
{
public:
#if __cplusplus >= 201103L
  static constexpr TaintedValue<T> min() noexcept { return TaintedValue<T>(std::numeric_limits<T>::min()); }
  static constexpr TaintedValue<T> lowest() noexcept { return TaintedValue<T>(std::numeric_limits<T>::lowest()); }
  static constexpr TaintedValue<T> max() noexcept { return TaintedValue<T>(std::numeric_limits<T>::max()); }
  static constexpr TaintedValue<T> epsilon() noexcept { return TaintedValue<T>(std::numeric_limits<T>::epsilon()); }
  static constexpr TaintedValue<T> round_error() noexcept { return TaintedValue<T>(std::numeric_limits<T>::round_error()); }
  static constexpr TaintedValue<T> infinity() noexcept { return TaintedValue<T>(std::numeric_limits<T>::infinity()); }
  static constexpr TaintedValue<T> quiet_NaN() noexcept { return TaintedValue<T>(std::numeric_limits<T>::quiet_NaN()); }
  static constexpr TaintedValue<T> signaling_NaN() noexcept { return TaintedValue<T>(std::numeric_limits<T>::signaling_NaN()); }
  static constexpr TaintedValue<T> denorm_min() noexcept { return TaintedValue<T>(std::numeric_limits<T>::denorm_min()); }
#else
  static TaintedValue<T> min() throw { return TaintedValue<T>(std::numeric_limits<T>::min()); }
  static TaintedValue<T> max() throw { return TaintedValue<T>(std::numeric_limits<T>::max()); }
  static TaintedValue<T> epsilon() throw { return TaintedValue<T>(std::numeric_limits<T>::epsilon()); }
  static TaintedValue<T> round_error() throw { return TaintedValue<T>(std::numeric_limits<T>::round_error()); }
  static TaintedValue<T> infinity() throw { return TaintedValue<T>(std::numeric_limits<T>::infinity()); }
  static TaintedValue<T> quiet_NaN() throw { return TaintedValue<T>(std::numeric_limits<T>::quiet_NaN()); }
  static TaintedValue<T> signaling_NaN() throw { return TaintedValue<T>(std::numeric_limits<T>::signaling_NaN()); }
  static TaintedValue<T> denorm_min() throw { return TaintedValue<T>(std::numeric_limits<T>::denorm_min()); }
#endif
};

template <> class std::numeric_limits<TaintedValue<bool> > : public TaintedValueNumericLimits<bool> {};
template <> class std::numeric_limits<TaintedValue<char> > : public TaintedValueNumericLimits<char> {};
template <> class std::numeric_limits<TaintedValue<signed char> > : public TaintedValueNumericLimits<signed char> {};
template <> class std::numeric_limits<TaintedValue<unsigned char> > : public TaintedValueNumericLimits<unsigned char> {};
template <> class std::numeric_limits<TaintedValue<short> > : public TaintedValueNumericLimits<short> {};
template <> class std::numeric_limits<TaintedValue<unsigned short> > : public TaintedValueNumericLimits<unsigned short> {};
template <> class std::numeric_limits<TaintedValue<int> > : public TaintedValueNumericLimits<int> {};
template <> class std::numeric_limits<TaintedValue<unsigned int> > : public TaintedValueNumericLimits<unsigned int> {};
template <> class std::numeric_limits<TaintedValue<long> > : public TaintedValueNumericLimits<long> {};
template <> class std::numeric_limits<TaintedValue<unsigned long> > : public TaintedValueNumericLimits<unsigned long> {};
template <> class std::numeric_limits<TaintedValue<long long> > : public TaintedValueNumericLimits<long long> {};
template <> class std::numeric_limits<TaintedValue<unsigned long long> > : public TaintedValueNumericLimits<unsigned long long> {};
template <> class std::numeric_limits<TaintedValue<float> > : public TaintedValueNumericLimits<float> {};
template <> class std::numeric_limits<TaintedValue<double> > : public TaintedValueNumericLimits<double> {};
template <> class std::numeric_limits<TaintedValue<long double> > : public TaintedValueNumericLimits<long double> {};

#if HAVE_FLOAT16
template <> class std::numeric_limits<TaintedValue<_Float16> > : public TaintedValueNumericLimits<_Float16> {};
#endif

template <typename FLOAT>
struct TaintedValueFloatingPointStatusAndControl
{
	typedef unisim::util::floating_point::Context<FloatingPointStatusAndControl<FLOAT> > Context;
  
	static void defaultNaN( bool dn ) { FloatingPointStatusAndControl<FLOAT>::defaultNaN( dn ); }
	static bool defaultNaN() { return FloatingPointStatusAndControl<FLOAT>::defaultNaN(); }
	static void detectTininess( uint_fast8_t dt ) { FloatingPointStatusAndControl<FLOAT>::detectTininess( dt ); }
	static uint_fast8_t detectTininess() { return FloatingPointStatusAndControl<FLOAT>::detectTininess(); }
	static void roundingMode( uint_fast8_t rmode ) { FloatingPointStatusAndControl<FLOAT>::roundingMode( rmode ); }
	static uint_fast8_t roundingMode() { return FloatingPointStatusAndControl<FLOAT>::roundingMode(); }
	static void exceptionFlags( uint_fast8_t eflags ) { FloatingPointStatusAndControl<FLOAT>::exceptionFlags( eflags ); }
	static uint_fast8_t exceptionFlags() { return FloatingPointStatusAndControl<FLOAT>::exceptionFlags(); }
	static void extF80_roundingPrecision( uint_fast8_t rp ) { FloatingPointStatusAndControl<FLOAT>::extF80_roundingPrecision( rp ); }
	static uint_fast8_t extF80_roundingPrecision() { return FloatingPointStatusAndControl<FLOAT>::extF80_roundingPrecision(); }
};

#if HAVE_FLOAT16
template <> struct FloatingPointStatusAndControl<TaintedValue<_Float16> > : TaintedValueFloatingPointStatusAndControl<_Float16> {};
#endif

template <> struct FloatingPointStatusAndControl<TaintedValue<float> > : TaintedValueFloatingPointStatusAndControl<float> {};
template <> struct FloatingPointStatusAndControl<TaintedValue<double> > : TaintedValueFloatingPointStatusAndControl<double> {};

#if HAVE_SOFTFLOAT_EMU

#undef DECL_TAINTED_SOFTFLOAT_EMU_FUNCTIONS
#define DECL_TAINTED_SOFTFLOAT_EMU_FUNCTIONS(FLOAT, PACKED)                                                      \
template <> inline TaintedValue<FLOAT> FromUnpacked( UnpackedFloat const& unpacked ) { return TaintedValue<FLOAT>(FromUnpacked<FLOAT>( unpacked )); }                                                                             \
template <> inline TaintedValue<FLOAT> FromPacked( TaintedValue<PACKED> packed ) { return MakeTaintedValue(FromPacked<FLOAT>(packed.value), packed.ubits ? -1 : 0); }                                                             \
template <> inline TaintedValue<uint16_t> ToInt( TaintedValue<FLOAT> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<uint16_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }                                   \
template <> inline TaintedValue<uint32_t> ToInt( TaintedValue<FLOAT> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<uint32_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }                                   \
template <> inline TaintedValue<uint64_t> ToInt( TaintedValue<FLOAT> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<uint64_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }                                   \
template <> inline TaintedValue<int16_t> ToInt( TaintedValue<FLOAT> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<int16_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }                                     \
template <> inline TaintedValue<int32_t> ToInt( TaintedValue<FLOAT> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<int32_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }                                     \
template <> inline TaintedValue<int64_t> ToInt( TaintedValue<FLOAT> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<int64_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }                                     \
template <> inline TaintedValue<uint16_t> ToIntRoundMinMag( TaintedValue<FLOAT> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<uint16_t>(a.value, exact), a.ubits ? -1 : 0); }                                        \
template <> inline TaintedValue<uint32_t> ToIntRoundMinMag( TaintedValue<FLOAT> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<uint32_t>(a.value, exact), a.ubits ? -1 : 0); }                                        \
template <> inline TaintedValue<uint64_t> ToIntRoundMinMag( TaintedValue<FLOAT> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<uint64_t>(a.value, exact), a.ubits ? -1 : 0); }                                        \
template <> inline TaintedValue<int16_t> ToIntRoundMinMag( TaintedValue<FLOAT> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<int16_t>(a.value, exact), a.ubits ? -1 : 0); }                                          \
template <> inline TaintedValue<int32_t> ToIntRoundMinMag( TaintedValue<FLOAT> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<int32_t>(a.value, exact), a.ubits ? -1 : 0); }                                          \
template <> inline TaintedValue<int64_t> ToIntRoundMinMag( TaintedValue<FLOAT> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<int64_t>(a.value, exact), a.ubits ? -1 : 0); }                                          \
template <> inline TaintedValue<FLOAT> DefaultNaN() { return TaintedValue<FLOAT>(DefaultNaN<FLOAT>()); }                                                                                                                          \
TaintedValue<PACKED> ToPacked( TaintedValue<FLOAT> a );                                                          \
void ToUnpacked( UnpackedFloat& unpacked, TaintedValue<FLOAT> a );                                               \
TaintedValue<FLOAT> RoundToInt( TaintedValue<FLOAT> a, uint_fast8_t rmode, bool exact );                         \
TaintedValue<FLOAT> FMulAdd( TaintedValue<FLOAT> a, TaintedValue<FLOAT> b, TaintedValue<FLOAT> c);               \
TaintedValue<FLOAT> FRem( TaintedValue<FLOAT> a, TaintedValue<FLOAT> b );                                        \
TaintedValue<FLOAT> FSqrt( TaintedValue<FLOAT> a );                                                              \
TaintedValue<FLOAT> FAbs( TaintedValue<FLOAT> a );                                                               \
TaintedValue<FLOAT> FMin( TaintedValue<FLOAT> a, TaintedValue<FLOAT> b );                                        \
TaintedValue<FLOAT> FMinNumber( TaintedValue<FLOAT> a, TaintedValue<FLOAT> b );                                  \
TaintedValue<FLOAT> FMax( TaintedValue<FLOAT> a, TaintedValue<FLOAT> b );                                        \
TaintedValue<FLOAT> FMaxNumber( TaintedValue<FLOAT> a, TaintedValue<FLOAT> b );                                  \
TaintedValue<bool> IsNaN( TaintedValue<FLOAT> a );                                                               \
TaintedValue<bool> IsDenormal( TaintedValue<FLOAT> a );                                                          \
TaintedValue<bool> IsSignaling( TaintedValue<FLOAT> a );                                                         \
TaintedValue<FLOAT> ClearSignaling( TaintedValue<FLOAT> a );                                                     \
TaintedValue<FLOAT> Zeroes( TaintedValue<FLOAT> a );                                                             \
TaintedValue<bool> IsZero( TaintedValue<FLOAT> a );                                                              \
TaintedValue<bool> IsNeg( TaintedValue<FLOAT> a );                                                               \
TaintedValue<bool> IsInf( TaintedValue<FLOAT> a );

#define DECL_TAINTED_SOFTFLOAT_EMU_CONVERSION_FUNCTIONS( HALF, SINGLE, DOUBLE )                                                                                                                            \
template <> inline TaintedValue<HALF  > FConvert<TaintedValue<HALF  >, TaintedValue<HALF  > >(TaintedValue<HALF  > a ) { return a; }                                                                       \
template <> inline TaintedValue<HALF  > FConvert<TaintedValue<HALF  >, TaintedValue<SINGLE> >(TaintedValue<SINGLE> a ) { return MakeTaintedValue(FConvert<HALF  , SINGLE>( a.value ), a.ubits ? -1 : 0); } \
template <> inline TaintedValue<HALF  > FConvert<TaintedValue<HALF  >, TaintedValue<DOUBLE> >(TaintedValue<DOUBLE> a ) { return MakeTaintedValue(FConvert<HALF  , DOUBLE>( a.value ), a.ubits ? -1 : 0); } \
template <> inline TaintedValue<SINGLE> FConvert<TaintedValue<SINGLE>, TaintedValue<HALF  > >(TaintedValue<HALF  > a ) { return MakeTaintedValue(FConvert<SINGLE, HALF  >( a.value ), a.ubits ? -1 : 0); } \
template <> inline TaintedValue<SINGLE> FConvert<TaintedValue<SINGLE>, TaintedValue<SINGLE> >(TaintedValue<SINGLE> a ) { return a; }                                                                       \
template <> inline TaintedValue<SINGLE> FConvert<TaintedValue<SINGLE>, TaintedValue<DOUBLE> >(TaintedValue<DOUBLE> a ) { return MakeTaintedValue(FConvert<SINGLE, DOUBLE>( a.value ), a.ubits ? -1 : 0); } \
template <> inline TaintedValue<DOUBLE> FConvert<TaintedValue<DOUBLE>, TaintedValue<HALF  > >(TaintedValue<HALF  > a ) { return MakeTaintedValue(FConvert<DOUBLE, HALF  >( a.value ), a.ubits ? -1 : 0); } \
template <> inline TaintedValue<DOUBLE> FConvert<TaintedValue<DOUBLE>, TaintedValue<SINGLE> >(TaintedValue<SINGLE> a ) { return MakeTaintedValue(FConvert<DOUBLE, SINGLE>( a.value ), a.ubits ? -1 : 0); } \
template <> inline TaintedValue<DOUBLE> FConvert<TaintedValue<DOUBLE>, TaintedValue<DOUBLE> >(TaintedValue<DOUBLE> a ) { return a; }

#undef DECL_TAINTED_SOFTFLOAT_EMU
#define DECL_TAINTED_SOFTFLOAT_EMU(NAMESPACE)                                                                                                                                                \
template <> struct TX<  NAMESPACE::Half> { typedef uint16_t as_mask; };                                                                                                                      \
template <> struct TX<NAMESPACE::Single> { typedef uint32_t as_mask; };                                                                                                                      \
template <> struct TX<NAMESPACE::Double> { typedef uint64_t as_mask; };                                                                                                                      \
template <typename SRC> struct CastUBits<NAMESPACE::Half,SRC> { typedef typename TX<SRC>::as_mask mask_t; static uint16_t process( mask_t mask, mask_t bits ) { return mask ? -1 : 0; } };   \
template <typename SRC> struct CastUBits<NAMESPACE::Single,SRC> { typedef typename TX<SRC>::as_mask mask_t; static uint32_t process( mask_t mask, mask_t bits ) { return mask ? -1 : 0; } }; \
template <typename SRC> struct CastUBits<NAMESPACE::Double,SRC> { typedef typename TX<SRC>::as_mask mask_t; static uint64_t process( mask_t mask, mask_t bits ) { return mask ? -1 : 0; } }; \
template <typename DST> struct CastUBits<DST,NAMESPACE::Half> { static typename TX<DST>::as_mask process( uint16_t mask, uint16_t bits ) { return mask ? -1 : 0; } };                        \
template <typename DST> struct CastUBits<DST,NAMESPACE::Single> { static typename TX<DST>::as_mask process( uint32_t mask, uint32_t bits ) { return mask ? -1 : 0; } };                      \
template <typename DST> struct CastUBits<DST,NAMESPACE::Double> { static typename TX<DST>::as_mask process( uint64_t mask, uint64_t bits ) { return mask ? -1 : 0; } };                      \
template <> struct CastUBits<NAMESPACE::Half,NAMESPACE::Double> { static uint16_t process( uint16_t mask, uint16_t bits ) { return mask ? -1 : 0; } };                                       \
template <> struct CastUBits<NAMESPACE::Single,NAMESPACE::Double> { static uint32_t process( uint32_t mask, uint32_t bits ) { return mask ? -1 : 0; } };                                     \
template <> struct CastUBits<NAMESPACE::Single,NAMESPACE::Half> { static uint64_t process( uint32_t mask, uint32_t bits ) { return mask ? -1 : 0; } };                                       \
template <> struct CastUBits<NAMESPACE::Double,NAMESPACE::Half> { static uint64_t process( uint64_t mask, uint64_t bits ) { return mask ? -1 : 0; } };                                       \
template <> struct CastUBits<NAMESPACE::Half,NAMESPACE::Single> { static uint16_t process( uint16_t mask, uint16_t bits ) { return mask ? -1 : 0; } };                                       \
template <> struct CastUBits<NAMESPACE::Double,NAMESPACE::Single> { static uint64_t process( uint64_t mask, uint64_t bits ) { return mask ? -1 : 0; } };                                     \
template <> struct CastUBits<NAMESPACE::Half,NAMESPACE::Half> { static uint16_t process( uint16_t mask, uint16_t bits ) { return mask; } };                                                  \
template <> struct CastUBits<NAMESPACE::Single,NAMESPACE::Single> { static uint32_t process( uint32_t mask, uint32_t bits ) { return mask; } };                                              \
template <> struct CastUBits<NAMESPACE::Double,NAMESPACE::Double> { static uint64_t process( uint64_t mask, uint64_t bits ) { return mask; } };                                              \
inline bool EqUBits( uint16_t ubits, NAMESPACE::Half lhs, NAMESPACE::Half rhs ) { return ubits; }                                                                                            \
inline bool EqUBits( uint32_t ubits, NAMESPACE::Single lhs, NAMESPACE::Single rhs ) { return ubits; }                                                                                        \
inline bool EqUBits( uint64_t ubits, NAMESPACE::Double lhs, NAMESPACE::Double rhs ) { return ubits; }                                                                                        \
template <> inline uint16_t TypePunning<uint16_t, NAMESPACE::Half>( NAMESPACE::Half value ) { return ToPacked( value ); }                                                                    \
template <> inline uint32_t TypePunning<uint32_t, NAMESPACE::Single>( NAMESPACE::Single value ) { return ToPacked( value ); }                                                                \
template <> inline uint64_t TypePunning<uint64_t, NAMESPACE::Double>( NAMESPACE::Double value ) { return ToPacked( value ); }                                                                \
template <> inline NAMESPACE::Half TypePunning<NAMESPACE::Half, uint16_t>( uint16_t value ) { return FromPacked<NAMESPACE::Half>( value ); }                                                 \
template <> inline NAMESPACE::Single TypePunning<NAMESPACE::Single, uint32_t>( uint32_t value ) { return FromPacked<NAMESPACE::Single>( value ); }                                           \
template <> inline NAMESPACE::Double TypePunning<NAMESPACE::Double, uint64_t>( uint64_t value ) { return FromPacked<NAMESPACE::Double>( value ); }                                           \
DECL_TAINTED_SOFTFLOAT_EMU_FUNCTIONS(  NAMESPACE::Half, uint16_t)                                                                                                                            \
DECL_TAINTED_SOFTFLOAT_EMU_FUNCTIONS(NAMESPACE::Single, uint32_t)                                                                                                                            \
DECL_TAINTED_SOFTFLOAT_EMU_FUNCTIONS(NAMESPACE::Double, uint64_t)                                                                                                                            \
DECL_TAINTED_SOFTFLOAT_EMU_CONVERSION_FUNCTIONS(NAMESPACE::Half, NAMESPACE::Single, NAMESPACE::Double)                                                                                       \
template <> class std::numeric_limits<TaintedValue<NAMESPACE::Half> > : public TaintedValueNumericLimits<NAMESPACE::Half> {};                                                                \
template <> class std::numeric_limits<TaintedValue<NAMESPACE::Single> > : public TaintedValueNumericLimits<NAMESPACE::Single> {};                                                            \
template <> class std::numeric_limits<TaintedValue<NAMESPACE::Double> > : public TaintedValueNumericLimits<NAMESPACE::Double> {};                                                            \
template <> struct FloatingPointStatusAndControl<TaintedValue<NAMESPACE::Half> > : TaintedValueFloatingPointStatusAndControl<NAMESPACE::Half> {};                                            \
template <> struct FloatingPointStatusAndControl<TaintedValue<NAMESPACE::Single> > : TaintedValueFloatingPointStatusAndControl<NAMESPACE::Single> {};                                        \
template <> struct FloatingPointStatusAndControl<TaintedValue<NAMESPACE::Double> > : TaintedValueFloatingPointStatusAndControl<NAMESPACE::Double> {};

DECL_TAINTED_SOFTFLOAT_EMU(unisim::util::floating_point::softfloat_emu::x86          )
DECL_TAINTED_SOFTFLOAT_EMU(unisim::util::floating_point::softfloat_emu::x86_sse      )
DECL_TAINTED_SOFTFLOAT_EMU(unisim::util::floating_point::softfloat_emu::arm_vfpv2    )
DECL_TAINTED_SOFTFLOAT_EMU(unisim::util::floating_point::softfloat_emu::arm_vfpv2_dn )
DECL_TAINTED_SOFTFLOAT_EMU(unisim::util::floating_point::softfloat_emu::riscv        )
DECL_TAINTED_SOFTFLOAT_EMU(unisim::util::floating_point::softfloat_emu::arm_vfpv2_ddn)

#undef DECL_TAINTED_SOFTFLOAT_EMU
#undef DECL_TAINTED_SOFTFLOAT_EMU_FUNCTIONS

#endif // HAVE_SOFTFLOAT_EMU

#endif /* __ARM64VP_TAINT_HH__ */
