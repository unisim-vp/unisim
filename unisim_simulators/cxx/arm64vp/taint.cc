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
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#include <taint.hh>
#include <iostream>
#include <cmath>

void PrintBin( std::ostream& sink, uint64_t vbits, uint64_t ubits ) { Print(sink << "0b", 0, 1, vbits, ubits); }
void PrintHex( std::ostream& sink, unsigned ml, uint64_t vbits, uint64_t ubits ) { Print(sink << "0x", ml, 4, vbits, ubits); }

void Print( std::ostream& sink, unsigned minlength, unsigned logradix, uint64_t vbits, uint64_t ubits )
{
  struct
  {
    void recurse( std::ostream& sink, unsigned left, unsigned bsz, uint64_t vbits, uint64_t ubits )
    {
      {
        uint64_t _vbits = vbits >> bsz, _ubits = ubits >> bsz;
        left = left ? left - 1 : 0;
        if (_vbits | _ubits | left)
          recurse( sink, left, bsz, _vbits, _ubits );
      }
      uint64_t mask = (1<<bsz)-1;
      sink << (ubits & mask ? 'X' : "0123456789abcdef"[vbits & mask] );
    }
  } _;
  
  _.recurse(sink, minlength, logradix, vbits, ubits);
}

template <typename T>
TaintedValue<T>
MakeTaintedValue(T value, typename TaintedValue<T>::ubits_type ubits ) { return TaintedValue<T>(TVCtor(), value, ubits); }

#if HAVE_FLOAT16
// functions for _Float16
template <> TaintedValue<_Float16> FromUnpacked<TaintedValue<_Float16> >( UnpackedFloat const& unpacked ) { return MakeTaintedValue(FromUnpacked<_Float16>(unpacked), 0); }
template <> TaintedValue<_Float16> FromPacked<TaintedValue<_Float16>, TaintedValue<uint16_t> >( TaintedValue<uint16_t> a ) { return MakeTaintedValue(FromPacked<_Float16>(a.value), a.ubits ? -1 : 0); }
TaintedValue<uint16_t> ToPacked( TaintedValue<_Float16> a ) { return MakeTaintedValue(ToPacked(a.value), a.ubits ? -1 : 0); }
void ToUnpacked( UnpackedFloat& unpacked, TaintedValue<_Float16> a ) { ToUnpacked(unpacked, a.value); }
template <> TaintedValue<_Float16> FConvert<TaintedValue<_Float16>, TaintedValue<_Float16> >( TaintedValue<_Float16> a ) { return a; }
template <> TaintedValue<_Float16> FConvert<TaintedValue<_Float16>, TaintedValue<float> >( TaintedValue<float> a ) { return MakeTaintedValue(FConvert<_Float16, float>(a.value), a.ubits ? -1 : 0); }
template <> TaintedValue<_Float16> FConvert<TaintedValue<_Float16>, TaintedValue<double> >( TaintedValue<double> a ) { return MakeTaintedValue(FConvert<_Float16, double>(a.value), a.ubits ? -1 : 0); }
template <> TaintedValue<uint16_t> ToInt( TaintedValue<_Float16> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<uint16_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<uint32_t> ToInt( TaintedValue<_Float16> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<uint32_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<uint64_t> ToInt( TaintedValue<_Float16> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<uint64_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<int16_t> ToInt( TaintedValue<_Float16> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<int16_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<int32_t> ToInt( TaintedValue<_Float16> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<int32_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<int64_t> ToInt( TaintedValue<_Float16> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<int64_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<uint16_t> ToIntRoundMinMag( TaintedValue<_Float16> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<uint16_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<uint32_t> ToIntRoundMinMag( TaintedValue<_Float16> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<uint32_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<uint64_t> ToIntRoundMinMag( TaintedValue<_Float16> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<uint64_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<int16_t> ToIntRoundMinMag( TaintedValue<_Float16> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<int16_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<int32_t> ToIntRoundMinMag( TaintedValue<_Float16> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<int32_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<int64_t> ToIntRoundMinMag( TaintedValue<_Float16> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<int64_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<_Float16> DefaultNaN() { return TaintedValue<_Float16>(DefaultNaN<_Float16>()); }
TaintedValue<_Float16> RoundToInt( TaintedValue<_Float16> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(RoundToInt(a.value, rmode, exact), a.ubits ? -1 : 0); }
TaintedValue<_Float16> FMulAdd( TaintedValue<_Float16> a, TaintedValue<_Float16> const& b, TaintedValue<_Float16> const& c) { return MakeTaintedValue(FMulAdd(a.value, b.value, c.value), (a.ubits or b.ubits or c.ubits) ? -1 : 0); }
TaintedValue<_Float16> FRem( TaintedValue<_Float16> a, TaintedValue<_Float16> b ) { return MakeTaintedValue(FRem(a.value, b.value), (a.ubits or b.ubits) ? -1 : 0); }
TaintedValue<_Float16> FSqrt( TaintedValue<_Float16> a ) { return MakeTaintedValue(FSqrt(a.value), a.ubits ? -1 : 0); }
TaintedValue<_Float16> FAbs( TaintedValue<_Float16> a ) { return MakeTaintedValue(FAbs(a.value), a.ubits ? -1 : 0); }
TaintedValue<_Float16> FMin( TaintedValue<_Float16> a, TaintedValue<_Float16> b ) { return MakeTaintedValue(FMin(a.value, b.value), (a.ubits or b.ubits) ? -1 : 0); }
TaintedValue<_Float16> FMinNumber( TaintedValue<_Float16> a, TaintedValue<_Float16> b ) { return MakeTaintedValue(FMinNumber(a.value, b.value), (a.ubits or b.ubits) ? -1 : 0); }
TaintedValue<_Float16> FMax( TaintedValue<_Float16> a, TaintedValue<_Float16> b ) { return MakeTaintedValue(FMax(a.value, b.value), (a.ubits or b.ubits) ? -1 : 0); }
TaintedValue<_Float16> FMaxNumber( TaintedValue<_Float16> a, TaintedValue<_Float16> b ) { return MakeTaintedValue(FMaxNumber(a.value, b.value), (a.ubits or b.ubits) ? -1 : 0); }
TaintedValue<bool> IsNaN( TaintedValue<_Float16> a ) { return MakeTaintedValue(IsNaN(a.value), a.ubits); }
TaintedValue<bool> IsDenormal( TaintedValue<_Float16> a ) { return MakeTaintedValue(IsDenormal(a.value), a.ubits); }
TaintedValue<bool> IsSignaling( TaintedValue<_Float16> a ) { return MakeTaintedValue(IsSignaling(a.value), a.ubits); }
TaintedValue<_Float16> ClearSignaling( TaintedValue<_Float16> a ) { return MakeTaintedValue(ClearSignaling(a.value), a.ubits ? -1 : 0); }
TaintedValue<_Float16> Zeroes( TaintedValue<_Float16> a ) { return MakeTaintedValue(Zeroes(a.value), a.ubits ? -1 : 0); }
TaintedValue<bool> IsZero( TaintedValue<_Float16> a ) { return MakeTaintedValue(IsZero(a.value), a.ubits); }
TaintedValue<bool> IsNeg( TaintedValue<_Float16> a ) { return MakeTaintedValue(IsNeg(a.value), a.ubits); }
TaintedValue<bool> IsInf( TaintedValue<_Float16> a ) { return MakeTaintedValue(IsInf(a.value), a.ubits); }
#endif

// functions for float
template <> TaintedValue<float> FromUnpacked<TaintedValue<float> >( UnpackedFloat const& unpacked ) { return MakeTaintedValue(FromUnpacked<float>(unpacked), 0); }
template <> TaintedValue<float> FromPacked<TaintedValue<float>, TaintedValue<uint32_t> >( TaintedValue<uint32_t> a ) { return MakeTaintedValue(FromPacked<float>(a.value), a.ubits ? -1 : 0); }
TaintedValue<uint32_t> ToPacked( TaintedValue<float> a ) { return MakeTaintedValue(ToPacked(a.value), a.ubits ? -1 : 0); }
void ToUnpacked( UnpackedFloat& unpacked, TaintedValue<float> a ) { ToUnpacked(unpacked, a.value); }
#if HAVE_FLOAT16
template <> TaintedValue<float> FConvert<TaintedValue<float>, TaintedValue<_Float16> >( TaintedValue<_Float16> a ) { return MakeTaintedValue(FConvert<float, _Float16>(a.value), a.ubits ? -1 : 0); }
#endif
template <> TaintedValue<float> FConvert<TaintedValue<float>, TaintedValue<float> >( TaintedValue<float> a ) { return a; }
template <> TaintedValue<float> FConvert<TaintedValue<float>, TaintedValue<double> >( TaintedValue<double> a ) { return MakeTaintedValue(FConvert<float, double>(a.value), a.ubits ? -1 : 0); }
template <> TaintedValue<uint16_t> ToInt( TaintedValue<float> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<uint16_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<uint32_t> ToInt( TaintedValue<float> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<uint32_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<uint64_t> ToInt( TaintedValue<float> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<uint64_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<int16_t> ToInt( TaintedValue<float> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<int16_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<int32_t> ToInt( TaintedValue<float> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<int32_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<int64_t> ToInt( TaintedValue<float> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<int64_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<uint16_t> ToIntRoundMinMag( TaintedValue<float> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<uint16_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<uint32_t> ToIntRoundMinMag( TaintedValue<float> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<uint32_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<uint64_t> ToIntRoundMinMag( TaintedValue<float> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<uint64_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<int16_t> ToIntRoundMinMag( TaintedValue<float> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<int16_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<int32_t> ToIntRoundMinMag( TaintedValue<float> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<int32_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<int64_t> ToIntRoundMinMag( TaintedValue<float> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<int64_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<float> DefaultNaN() { return TaintedValue<float>(DefaultNaN<float>()); }
TaintedValue<float> RoundToInt( TaintedValue<float> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(RoundToInt(a.value, rmode, exact), a.ubits ? -1 : 0); }
TaintedValue<float> FMulAdd( TaintedValue<float> a, TaintedValue<float> const& b, TaintedValue<float> const& c) { return MakeTaintedValue(FMulAdd(a.value, b.value, c.value), (a.ubits or b.ubits or c.ubits) ? -1 : 0); }
TaintedValue<float> FRem( TaintedValue<float> a, TaintedValue<float> b ) { return MakeTaintedValue(FRem(a.value, b.value), (a.ubits or b.ubits) ? -1 : 0); }
TaintedValue<float> FSqrt( TaintedValue<float> a ) { return MakeTaintedValue(FSqrt(a.value), a.ubits ? -1 : 0); }
TaintedValue<float> FAbs( TaintedValue<float> a ) { return MakeTaintedValue(FAbs(a.value), a.ubits ? -1 : 0); }
TaintedValue<float> FMin( TaintedValue<float> a, TaintedValue<float> b ) { return MakeTaintedValue(FMin(a.value, b.value), (a.ubits or b.ubits) ? -1 : 0); }
TaintedValue<float> FMinNumber( TaintedValue<float> a, TaintedValue<float> b ) { return MakeTaintedValue(FMinNumber(a.value, b.value), (a.ubits or b.ubits) ? -1 : 0); }
TaintedValue<float> FMax( TaintedValue<float> a, TaintedValue<float> b ) { return MakeTaintedValue(FMax(a.value, b.value), (a.ubits or b.ubits) ? -1 : 0); }
TaintedValue<float> FMaxNumber( TaintedValue<float> a, TaintedValue<float> b ) { return MakeTaintedValue(FMaxNumber(a.value, b.value), (a.ubits or b.ubits) ? -1 : 0); }
TaintedValue<bool> IsNaN( TaintedValue<float> a ) { return MakeTaintedValue(IsNaN(a.value), a.ubits); }
TaintedValue<bool> IsDenormal( TaintedValue<float> a ) { return MakeTaintedValue(IsDenormal(a.value), a.ubits); }
TaintedValue<bool> IsSignaling( TaintedValue<float> a ) { return MakeTaintedValue(IsSignaling(a.value), a.ubits); }
TaintedValue<float> ClearSignaling( TaintedValue<float> a ) { return MakeTaintedValue(ClearSignaling(a.value), a.ubits ? -1 : 0); }
TaintedValue<float> Zeroes( TaintedValue<float> a ) { return MakeTaintedValue(Zeroes(a.value), a.ubits ? -1 : 0); }
TaintedValue<bool> IsZero( TaintedValue<float> a ) { return MakeTaintedValue(IsZero(a.value), a.ubits); }
TaintedValue<bool> IsNeg( TaintedValue<float> a ) { return MakeTaintedValue(IsNeg(a.value), a.ubits); }
TaintedValue<bool> IsInf( TaintedValue<float> a ) { return MakeTaintedValue(IsInf(a.value), a.ubits); }

// functions for double
template <> TaintedValue<double> FromUnpacked<TaintedValue<double> >( UnpackedFloat const& unpacked ) { return MakeTaintedValue(FromUnpacked<double>(unpacked), 0); }
template <> TaintedValue<double> FromPacked<TaintedValue<double>, TaintedValue<uint64_t> >( TaintedValue<uint64_t> a ) { return MakeTaintedValue(FromPacked<double>(a.value), a.ubits ? -1 : 0); }
TaintedValue<uint64_t> ToPacked( TaintedValue<double> a ) { return MakeTaintedValue(ToPacked(a.value), a.ubits ? -1 : 0); }
void ToUnpacked( UnpackedFloat& unpacked, TaintedValue<double> a ) { ToUnpacked(unpacked, a.value); }
#if HAVE_FLOAT16
template <> TaintedValue<double> FConvert<TaintedValue<double>, TaintedValue<_Float16> >( TaintedValue<_Float16> a ) { return MakeTaintedValue(FConvert<double, _Float16>(a.value), a.ubits ? -1 : 0); }
#endif
template <> TaintedValue<double> FConvert<TaintedValue<double>, TaintedValue<float> >( TaintedValue<float> a ) { return MakeTaintedValue(FConvert<double, float>(a.value), a.ubits ? -1 : 0); }
template <> TaintedValue<double> FConvert<TaintedValue<double>, TaintedValue<double> >( TaintedValue<double> a ) { return a; }
template <> TaintedValue<uint16_t> ToInt( TaintedValue<double> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<uint16_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<uint32_t> ToInt( TaintedValue<double> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<uint32_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<uint64_t> ToInt( TaintedValue<double> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<uint64_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<int16_t> ToInt( TaintedValue<double> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<int16_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<int32_t> ToInt( TaintedValue<double> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<int32_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<int64_t> ToInt( TaintedValue<double> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<int64_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<uint16_t> ToIntRoundMinMag( TaintedValue<double> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<uint16_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<uint32_t> ToIntRoundMinMag( TaintedValue<double> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<uint32_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<uint64_t> ToIntRoundMinMag( TaintedValue<double> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<uint64_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<int16_t> ToIntRoundMinMag( TaintedValue<double> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<int16_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<int32_t> ToIntRoundMinMag( TaintedValue<double> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<int32_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<int64_t> ToIntRoundMinMag( TaintedValue<double> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<int64_t>(a.value, exact), a.ubits ? -1 : 0); }
template <> TaintedValue<double> DefaultNaN() { return TaintedValue<double>(DefaultNaN<double>()); }
TaintedValue<double> RoundToInt( TaintedValue<double> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(RoundToInt(a.value, rmode, exact), a.ubits ? -1 : 0); }
TaintedValue<double> FMulAdd( TaintedValue<double> a, TaintedValue<double> const& b, TaintedValue<double> const& c) { return MakeTaintedValue(FMulAdd(a.value, b.value, c.value), (a.ubits or b.ubits or c.ubits) ? -1 : 0); }
TaintedValue<double> FRem( TaintedValue<double> a, TaintedValue<double> b ) { return MakeTaintedValue(FRem(a.value, b.value), (a.ubits or b.ubits) ? -1 : 0); }
TaintedValue<double> FSqrt( TaintedValue<double> a ) { return MakeTaintedValue(FSqrt(a.value), a.ubits ? -1 : 0); }
TaintedValue<double> FAbs( TaintedValue<double> a ) { return MakeTaintedValue(FAbs(a.value), a.ubits ? -1 : 0); }
TaintedValue<double> FMin( TaintedValue<double> a, TaintedValue<double> b ) { return MakeTaintedValue(FMin(a.value, b.value), (a.ubits or b.ubits) ? -1 : 0); }
TaintedValue<double> FMinNumber( TaintedValue<double> a, TaintedValue<double> b ) { return MakeTaintedValue(FMinNumber(a.value, b.value), (a.ubits or b.ubits) ? -1 : 0); }
TaintedValue<double> FMax( TaintedValue<double> a, TaintedValue<double> b ) { return MakeTaintedValue(FMax(a.value, b.value), (a.ubits or b.ubits) ? -1 : 0); }
TaintedValue<double> FMaxNumber( TaintedValue<double> a, TaintedValue<double> b ) { return MakeTaintedValue(FMaxNumber(a.value, b.value), (a.ubits or b.ubits) ? -1 : 0); }
TaintedValue<bool> IsNaN( TaintedValue<double> a ) { return MakeTaintedValue(IsNaN(a.value), a.ubits); }
TaintedValue<bool> IsDenormal( TaintedValue<double> a ) { return MakeTaintedValue(IsDenormal(a.value), a.ubits); }
TaintedValue<bool> IsSignaling( TaintedValue<double> a ) { return MakeTaintedValue(IsSignaling(a.value), a.ubits); }
TaintedValue<double> ClearSignaling( TaintedValue<double> a ) { return MakeTaintedValue(ClearSignaling(a.value), a.ubits ? -1 : 0); }
TaintedValue<double> Zeroes( TaintedValue<double> a ) { return MakeTaintedValue(Zeroes(a.value), a.ubits ? -1 : 0); }
TaintedValue<bool> IsZero( TaintedValue<double> a ) { return MakeTaintedValue(IsZero(a.value), a.ubits); }
TaintedValue<bool> IsNeg( TaintedValue<double> a ) { return MakeTaintedValue(IsNeg(a.value), a.ubits); }
TaintedValue<bool> IsInf( TaintedValue<double> a ) { return MakeTaintedValue(IsInf(a.value), a.ubits); }

#if HAVE_SOFTFLOAT_EMU

#undef DEF_TAINTED_SOFTFLOAT_EMU_FUNCTIONS
#undef DEF_TAINTED_SOFTFLOAT_EMU

#define DEF_TAINTED_SOFTFLOAT_EMU_FUNCTIONS(FLOAT, PACKED)                                                                                                                                                                 \
template <> TaintedValue<FLOAT> FromUnpacked( UnpackedFloat const& unpacked ) { return TaintedValue<FLOAT>(FromUnpacked<FLOAT>( unpacked )); }                                                                             \
template <> TaintedValue<FLOAT> FromPacked( TaintedValue<PACKED> packed ) { return MakeTaintedValue(FromPacked<FLOAT>(packed.value), packed.ubits ? -1 : 0); }                                                             \
template <> TaintedValue<uint16_t> ToInt( TaintedValue<FLOAT> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<uint16_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }                                   \
template <> TaintedValue<uint32_t> ToInt( TaintedValue<FLOAT> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<uint32_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }                                   \
template <> TaintedValue<uint64_t> ToInt( TaintedValue<FLOAT> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<uint64_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }                                   \
template <> TaintedValue<int16_t> ToInt( TaintedValue<FLOAT> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<int16_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }                                     \
template <> TaintedValue<int32_t> ToInt( TaintedValue<FLOAT> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<int32_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }                                     \
template <> TaintedValue<int64_t> ToInt( TaintedValue<FLOAT> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(ToInt<int64_t>(a.value, rmode, exact), a.ubits ? -1 : 0); }                                     \
template <> TaintedValue<uint16_t> ToIntRoundMinMag( TaintedValue<FLOAT> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<uint16_t>(a.value, exact), a.ubits ? -1 : 0); }                                        \
template <> TaintedValue<uint32_t> ToIntRoundMinMag( TaintedValue<FLOAT> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<uint32_t>(a.value, exact), a.ubits ? -1 : 0); }                                        \
template <> TaintedValue<uint64_t> ToIntRoundMinMag( TaintedValue<FLOAT> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<uint64_t>(a.value, exact), a.ubits ? -1 : 0); }                                        \
template <> TaintedValue<int16_t> ToIntRoundMinMag( TaintedValue<FLOAT> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<int16_t>(a.value, exact), a.ubits ? -1 : 0); }                                          \
template <> TaintedValue<int32_t> ToIntRoundMinMag( TaintedValue<FLOAT> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<int32_t>(a.value, exact), a.ubits ? -1 : 0); }                                          \
template <> TaintedValue<int64_t> ToIntRoundMinMag( TaintedValue<FLOAT> a, bool exact ) { return MakeTaintedValue(ToIntRoundMinMag<int64_t>(a.value, exact), a.ubits ? -1 : 0); }                                          \
template <> TaintedValue<FLOAT> DefaultNaN() { return TaintedValue<FLOAT>(DefaultNaN<FLOAT>()); }                                                                                                                          \
TaintedValue<PACKED> ToPacked( TaintedValue<FLOAT> a ) { return MakeTaintedValue(ToPacked(a.value), a.ubits ? -1 : 0); }                                                                                                   \
TaintedValue<FLOAT> RoundToInt( TaintedValue<FLOAT> a, uint_fast8_t rmode, bool exact ) { return MakeTaintedValue(RoundToInt(a.value, rmode, exact), a.ubits ? -1 : 0); }                                                  \
TaintedValue<FLOAT> FMulAdd( TaintedValue<FLOAT> a, TaintedValue<FLOAT> const& b, TaintedValue<FLOAT> const& c) { return MakeTaintedValue(FMulAdd(a.value, b.value, c.value), (a.ubits or b.ubits or c.ubits) ? -1 : 0); } \
TaintedValue<FLOAT> FRem( TaintedValue<FLOAT> a, TaintedValue<FLOAT> b ) { return MakeTaintedValue(FRem(a.value, b.value), (a.ubits or b.ubits) ? -1 : 0); }                                                               \
TaintedValue<FLOAT> FSqrt( TaintedValue<FLOAT> a ) { return MakeTaintedValue(FSqrt(a.value), a.ubits ? -1 : 0); }                                                                                                          \
TaintedValue<FLOAT> FAbs( TaintedValue<FLOAT> a ) { return MakeTaintedValue(FAbs(a.value), a.ubits ? -1 : 0); }                                                                                                            \
TaintedValue<FLOAT> FMin( TaintedValue<FLOAT> a, TaintedValue<FLOAT> b ) { return MakeTaintedValue(FMin(a.value, b.value), (a.ubits or b.ubits) ? -1 : 0); }                                                               \
TaintedValue<FLOAT> FMinNumber( TaintedValue<FLOAT> a, TaintedValue<FLOAT> b ) { return MakeTaintedValue(FMinNumber(a.value, b.value), (a.ubits or b.ubits) ? -1 : 0); }                                                   \
TaintedValue<FLOAT> FMax( TaintedValue<FLOAT> a, TaintedValue<FLOAT> b ) { return MakeTaintedValue(FMax(a.value, b.value), (a.ubits or b.ubits) ? -1 : 0); }                                                               \
TaintedValue<FLOAT> FMaxNumber( TaintedValue<FLOAT> a, TaintedValue<FLOAT> b ) { return MakeTaintedValue(FMaxNumber(a.value, b.value), (a.ubits or b.ubits) ? -1 : 0); }                                                   \
TaintedValue<bool> IsNaN( TaintedValue<FLOAT> a ) { return MakeTaintedValue(IsNaN(a.value), a.ubits); }                                                                                                                    \
TaintedValue<bool> IsDenormal( TaintedValue<FLOAT> a ) { return MakeTaintedValue(IsDenormal(a.value), a.ubits); }                                                                                                          \
TaintedValue<bool> IsSignaling( TaintedValue<FLOAT> a ) { return MakeTaintedValue(IsSignaling(a.value), a.ubits); }                                                                                                        \
TaintedValue<FLOAT> ClearSignaling( TaintedValue<FLOAT> a ) { return MakeTaintedValue(ClearSignaling(a.value), a.ubits ? -1 : 0); }                                                                                        \
TaintedValue<FLOAT> Zeroes( TaintedValue<FLOAT> a ) { return MakeTaintedValue(Zeroes(a.value), a.ubits ? -1 : 0); }                                                                                                        \
TaintedValue<bool> IsZero( TaintedValue<FLOAT> a ) { return MakeTaintedValue(IsZero(a.value), a.ubits); }                                                                                                                  \
TaintedValue<bool> IsNeg( TaintedValue<FLOAT> a ) { return MakeTaintedValue(IsNeg(a.value), a.ubits); }                                                                                                                    \
TaintedValue<bool> IsInf( TaintedValue<FLOAT> a ) { return MakeTaintedValue(IsInf(a.value), a.ubits); }

#define DEF_TAINTED_SOFTFLOAT_EMU_CONVERSION_FUNCTIONS( HALF, SINGLE, DOUBLE )                                                                                                                      \
template <> TaintedValue<HALF  > FConvert<TaintedValue<HALF  >, TaintedValue<HALF  > >(TaintedValue<HALF  > a ) { return a; }                                                                       \
template <> TaintedValue<HALF  > FConvert<TaintedValue<HALF  >, TaintedValue<SINGLE> >(TaintedValue<SINGLE> a ) { return MakeTaintedValue(FConvert<HALF  , SINGLE>( a.value ), a.ubits ? -1 : 0); } \
template <> TaintedValue<HALF  > FConvert<TaintedValue<HALF  >, TaintedValue<DOUBLE> >(TaintedValue<DOUBLE> a ) { return MakeTaintedValue(FConvert<HALF  , DOUBLE>( a.value ), a.ubits ? -1 : 0); } \
template <> TaintedValue<SINGLE> FConvert<TaintedValue<SINGLE>, TaintedValue<HALF  > >(TaintedValue<HALF  > a ) { return MakeTaintedValue(FConvert<SINGLE, HALF  >( a.value ), a.ubits ? -1 : 0); } \
template <> TaintedValue<SINGLE> FConvert<TaintedValue<SINGLE>, TaintedValue<SINGLE> >(TaintedValue<SINGLE> a ) { return a; }                                                                       \
template <> TaintedValue<SINGLE> FConvert<TaintedValue<SINGLE>, TaintedValue<DOUBLE> >(TaintedValue<DOUBLE> a ) { return MakeTaintedValue(FConvert<SINGLE, DOUBLE>( a.value ), a.ubits ? -1 : 0); } \
template <> TaintedValue<DOUBLE> FConvert<TaintedValue<DOUBLE>, TaintedValue<HALF  > >(TaintedValue<HALF  > a ) { return MakeTaintedValue(FConvert<DOUBLE, HALF  >( a.value ), a.ubits ? -1 : 0); } \
template <> TaintedValue<DOUBLE> FConvert<TaintedValue<DOUBLE>, TaintedValue<SINGLE> >(TaintedValue<SINGLE> a ) { return MakeTaintedValue(FConvert<DOUBLE, SINGLE>( a.value ), a.ubits ? -1 : 0); } \
template <> TaintedValue<DOUBLE> FConvert<TaintedValue<DOUBLE>, TaintedValue<DOUBLE> >(TaintedValue<DOUBLE> a ) { return a; }

#define DEF_TAINTED_SOFTFLOAT_EMU(NAMESPACE) \
DEF_TAINTED_SOFTFLOAT_EMU_FUNCTIONS(  NAMESPACE::Half, uint16_t) \
DEF_TAINTED_SOFTFLOAT_EMU_FUNCTIONS(NAMESPACE::Single, uint32_t) \
DEF_TAINTED_SOFTFLOAT_EMU_FUNCTIONS(NAMESPACE::Double, uint64_t) \
DEF_TAINTED_SOFTFLOAT_EMU_CONVERSION_FUNCTIONS(NAMESPACE::Half, NAMESPACE::Single, NAMESPACE::Double)

DEF_TAINTED_SOFTFLOAT_EMU(unisim::util::floating_point::softfloat_emu::x86          )
DEF_TAINTED_SOFTFLOAT_EMU(unisim::util::floating_point::softfloat_emu::x86_sse      )
DEF_TAINTED_SOFTFLOAT_EMU(unisim::util::floating_point::softfloat_emu::arm_vfpv2    )
DEF_TAINTED_SOFTFLOAT_EMU(unisim::util::floating_point::softfloat_emu::arm_vfpv2_dn ) 
DEF_TAINTED_SOFTFLOAT_EMU(unisim::util::floating_point::softfloat_emu::riscv        )
DEF_TAINTED_SOFTFLOAT_EMU(unisim::util::floating_point::softfloat_emu::arm_vfpv2_ddn)

#undef DEF_TAINTED_SOFTFLOAT_EMU
#undef DEF_TAINTED_SOFTFLOAT_EMU_FUNCTIONS

#endif // HAVE_SOFTFLOAT_EMU
