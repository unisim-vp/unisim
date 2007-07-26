/***************************************************************************
                Floating.template  -  Template for various types of floating point computations
                             -------------------
    first release        : 15 Jul 2005
    copyright ©          : (C) 2004-2005 CEA
    authors              : Franck Védrine, Bruno Marre, Benjamin Blanc, Gilles Mouchard
    email                : Franck.Vedrine@cea.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 * This program is free software; you can redistribute it and/or           *
 * modify it under the terms of the GNU Lesser General Public License      *
 * as published by the Free Software Foundation; either version 2.1        *
 * of the License, or (at your option) any later version.                  *
 *                                                                         *
 * This program is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU Lesser General Public License for more details.                     *
 *                                                                         *
 ***************************************************************************/

#ifndef Numerics_Double_FloatingTemplate
#define Numerics_Double_FloatingTemplate
#ifndef __GNUC__
#include "Floating.h"
#else
#include "Floating_gccopt.h"
#endif

namespace Numerics {} // for precompiled headers

#include <math.h>
#if defined(__GNUC__) && (GCC_VERSION < 30000)
#include <ctype.h>
#endif
#include <vector>

namespace Numerics { namespace Double {

namespace Details { namespace DTDoubleElement {

template <class TypeMantissa, class TypeStatusAndControlFlags>
Access::Carry
Access::trightShift(TypeMantissa& mMantissa, int uShift,
      unsigned int uValue, TypeStatusAndControlFlags& scfFlags, bool fNegative, int uBitSizeMantissa) {
   assert(uShift >= 0);
   bool fAdd = false;
   bool fRoundToEven = false;
   bool fApproximate = false;
   Carry cResult;
   scfFlags.clearEffectiveRoundToEven();
   if (!mMantissa.hasZero(uShift)) {
      if (scfFlags.isNearestRound()) {
         if (!scfFlags.isApproximate(fNegative ? TypeStatusAndControlFlags::Down : TypeStatusAndControlFlags::Up)
               || !mMantissa.hasZero(uShift-1))
            fAdd = mMantissa.cbitArray(uShift-1);
         if (fAdd && !scfFlags.isApproximate() && scfFlags.isRoundToEven()) {
            fRoundToEven = mMantissa.hasZero(uShift-1) && !scfFlags.isApproximate();
            if (fRoundToEven)
               scfFlags.setEffectiveRoundToEven();
         };
      }
      else if (scfFlags.isHighestRound())
         fAdd = !fNegative;
      else if (scfFlags.isLowestRound())
         fAdd = fNegative;
      fApproximate = true;
   };
   mMantissa >>= uShift;
   if (fAdd && ((fAdd = (!fRoundToEven || mMantissa.cbitArray(0))) != false)) {
      mMantissa.inc();
      if (mMantissa.cbitArray(uBitSizeMantissa-uShift)) {
         cResult.carry() = true;
         mMantissa.setFalseBitArray(uBitSizeMantissa-uShift);
      };
   };
   if (fApproximate) {
      if (fNegative)
         scfFlags.setApproximate(fAdd ? TypeStatusAndControlFlags::Down : TypeStatusAndControlFlags::Up);
      else
         scfFlags.setApproximate(fAdd ? TypeStatusAndControlFlags::Up : TypeStatusAndControlFlags::Down);
   };
   if (cResult.hasCarry()) {
      ++uValue;
      if ((uValue == 0U) || ((uShift < (int) (8*sizeof(unsigned int))) && (uValue >= (1U << uShift))))
         return cResult;
      else
         cResult.carry() = false;
   };
   if ((((uBitSizeMantissa-1) % (8*sizeof(unsigned int)))+1) >= (uShift % (8*sizeof(unsigned int))))
      mMantissa[(uBitSizeMantissa-uShift) / (8*sizeof(unsigned int))]
         |= (uValue << ((uBitSizeMantissa-uShift) % (8*sizeof(unsigned int))));
   else {
      mMantissa[(uBitSizeMantissa-uShift) / (8*sizeof(unsigned int))]
         |= (uValue << (uBitSizeMantissa-uShift) % (8*sizeof(unsigned int)));
      mMantissa[(uBitSizeMantissa-uShift) / (8*sizeof(unsigned int)) + 1]
         = (uValue >> (8*sizeof(unsigned int) - (uBitSizeMantissa-uShift) % (8*sizeof(unsigned int))));
   };
   return cResult;
}

}} // end of namespace Details::DTDoubleElement

/******************************************/
/* Implementation - template TBuiltDouble */
/******************************************/

template <class TypeTraits>
typename TypeTraits::Exponent
#ifndef __BORLANDC__
TBuiltDouble<TypeTraits>::eZeroExponent = typename TypeTraits::Exponent(typename TypeTraits::Exponent::Zero());
#else
TBuiltDouble<TypeTraits>::eZeroExponent = TypeTraits::Exponent(Exponent::Zero());
#endif

template <class TypeTraits>
typename TypeTraits::Exponent
#ifndef __BORLANDC__
TBuiltDouble<TypeTraits>::eOneExponent = typename TypeTraits::Exponent(typename TypeTraits::Exponent::One());
#else
TBuiltDouble<TypeTraits>::eOneExponent = TypeTraits::Exponent(Exponent::One());
#endif

template <class TypeTraits>
typename TypeTraits::Exponent
#ifndef __BORLANDC__
TBuiltDouble<TypeTraits>::eMinusOneExponent = typename TypeTraits::Exponent(typename TypeTraits::Exponent::MinusOne());
#else
TBuiltDouble<TypeTraits>::eMinusOneExponent = TypeTraits::Exponent(Exponent::MinusOne());
#endif

template <class TypeTraits>
typename TypeTraits::Exponent
#ifndef __BORLANDC__
TBuiltDouble<TypeTraits>::eInftyExponent = typename TypeTraits::Exponent(typename TypeTraits::Exponent::Max());
#else
TBuiltDouble<TypeTraits>::eInftyExponent = TypeTraits::Exponent(Exponent::Max());
#endif

template <class TypeTraits>
TBuiltDouble<TypeTraits>::TBuiltDouble(unsigned int uValue)
#ifndef __BORLANDC__
   :  biMantissa(), biExponent(typename Exponent::Min()), fNegative(false) {
#else
   :  biMantissa(), biExponent(Exponent::Min()), fNegative(false) {
#endif
   if (uValue != 0U) {
      int uLogResult = log_base_2(uValue);
      assert(uLogResult <= bitSizeMantissa());
      biMantissa = uValue;
      biMantissa <<= (bitSizeMantissa()-uLogResult+1);
      biExponent = getZeroExponent();
      biExponent.add(uLogResult-1);
   };
}

template <class TypeTraits>
TBuiltDouble<TypeTraits>::TBuiltDouble(const IntConversion& icValueSource, StatusAndControlFlags& scfFlags)
#ifndef __BORLANDC__
   :  biMantissa(), biExponent(typename Exponent::Min()), fNegative(icValueSource.isNegative()) {
#else
   :  biMantissa(), biExponent(Exponent::Min()), fNegative(icValueSource.isNegative()) {
#endif
   IntConversion icValue(icValueSource);
   if (fNegative)
      icValue.opposite();
   icValue.setUnsigned();
   if (icValue.isDifferentZero()) {
      int uLogResult = icValue.log_base_2();
      if ((icValue.querySize() > bitSizeMantissa()) && (uLogResult > bitSizeMantissa()+1)) {
         trightShift(icValue, (uLogResult-bitSizeMantissa()-1), 0U, scfFlags, fNegative, icValue.querySize());
         for (unsigned int uIndex = 0; uIndex < (icValue.querySize()-1)/(sizeof(unsigned int)*8)+1;
               ++uIndex)
            biMantissa[uIndex] = icValue[uIndex];
         biMantissa.normalize();
         biExponent = getZeroExponent();
         biExponent.add(uLogResult-1);
      }
      else if (uLogResult <= bitSizeMantissa()) {
         for (unsigned int uIndex = 0; uIndex < (icValue.querySize()-1)/(sizeof(unsigned int)*8)+1;
               ++uIndex)
            biMantissa[uIndex] = icValue[uIndex];
         biMantissa.normalize();
         biMantissa <<= (bitSizeMantissa()-uLogResult+1);
         biExponent = getZeroExponent();
         biExponent.add(uLogResult-1);
      }
      else if (icValue.querySize() > bitSizeMantissa()) { // uLogResult == bitSizeMantissa()+1
         icValue &= (IntConversion().neg() <<= bitSizeMantissa()).neg();
         for (unsigned int uIndex = 0; uIndex < (icValue.querySize()-1)/(sizeof(unsigned int)*8)+1;
               ++uIndex)
            biMantissa[uIndex] = icValue[uIndex];
         biMantissa.normalize();
         biExponent = getZeroExponent();
         biExponent.add(bitSizeMantissa());
      };
   };
}

template <class TypeTraits>
TBuiltDouble<TypeTraits>::TBuiltDouble(const FloatConversion& fcValue, StatusAndControlFlags& scfFlags)
#ifndef __BORLANDC__
   :  biMantissa(), biExponent(typename Exponent::Min()), fNegative(fcValue.isNegative()) {
#else
   :  biMantissa(), biExponent(Exponent::Min()), fNegative(fcValue.isNegative()) {
#endif
   if (fcValue.isZeroExponent() && fcValue.isZeroMantissa())
      return;
   if (fcValue.isInftyExponent()) {
      biExponent = getInftyExponent();
      if (fcValue.isZeroMantissa()) {
         scfFlags.setOverflow();
         return;
      };
      if (fcValue.querySizeMantissa() < bitSizeMantissa()) {
         for (register int uIndex = (fcValue.querySizeMantissa()-1)/(8*sizeof(unsigned int)); 
               uIndex >= 0; --uIndex)
            biMantissa[uIndex] = fcValue.mantissa()[uIndex];
         biMantissa.normalize();
         biMantissa <<= (bitSizeMantissa()-fcValue.querySizeMantissa());
      }
      else { // fcValue.querySizeMantissa() >= bitSizeMantissa()
         typename FloatConversion::Mantissa fcmMantissa(fcValue.mantissa());
         fcmMantissa >>= (fcValue.querySizeMantissa()-bitSizeMantissa());
         for (register int uIndex = (bitSizeMantissa()-1)/(8*sizeof(unsigned int)); 
               uIndex >= 0; --uIndex)
            biMantissa[uIndex] = fcmMantissa[uIndex];
         if (scfFlags.keepSignalingConversion() && biMantissa.isZero()) {
            biMantissa.neg();
            biMantissa.setFalseBitArray(bitSizeMantissa()-1);
         };
      };
      if (!biMantissa.cbitArray(bitSizeMantissa()-1)) {
         scfFlags.setSNaNOperand();
         if (!scfFlags.keepSignalingConversion())
            biMantissa.setTrueBitArray(bitSizeMantissa()-1);
      };
   }
   else if (fcValue.querySizeExponent() < bitSizeExponent()) { // double <- float
      assert((bitSizeExponent() >= log_base_2(bitSizeMantissa()))
         && (fcValue.querySizeMantissa() <= bitSizeMantissa()));

      int uLogMantissa = fcValue.isZeroExponent() ? fcValue.mantissa().log_base_2()
         : (fcValue.querySizeMantissa() + 1);

      for (register int uIndex = (fcValue.querySizeExponent()-1)/(8*sizeof(unsigned int)); 
            uIndex >= 0; --uIndex)
         biExponent[uIndex] = fcValue.exponent()[uIndex];
      biExponent.normalize();
      biExponent.add(Exponent(typename Exponent::Min()).neg(bitSizeExponent()-fcValue.querySizeExponent())
         <<= (fcValue.querySizeExponent()-1));
         
      if (fcValue.isZeroExponent()) 
         biExponent.sub(fcValue.querySizeMantissa() - uLogMantissa);

      for (register int uIndex = (fcValue.querySizeMantissa()-1)/(8*sizeof(unsigned int)); 
            uIndex >= 0; --uIndex)
         biMantissa[uIndex] = fcValue.mantissa()[uIndex];
      biMantissa <<= (bitSizeMantissa() - uLogMantissa + 1);
      biMantissa.normalize();
   }
   else if (fcValue.querySizeExponent() > bitSizeExponent()) { // float <- double
      assert((fcValue.querySizeExponent() >= log_base_2(fcValue.querySizeMantissa()))
         && fcValue.querySizeMantissa() >= bitSizeMantissa());

      typename FloatConversion::Exponent fceSubExponent;
      fceSubExponent.neg(fcValue.querySizeExponent()-bitSizeExponent())
         <<= (bitSizeExponent()-1);
      typename FloatConversion::Exponent fceNewExponent = fcValue.exponent();
      if (!fceNewExponent.sub(fceSubExponent).hasCarry() && !fceNewExponent.isZero()) {
         if (fceNewExponent >= typename FloatConversion::Exponent().neg(bitSizeExponent())) {
            scfFlags.setOverflow();
            biExponent = getInftyExponent();
            biMantissa = 0U;
            if (scfFlags.upApproximateInfty()) {
               if (scfFlags.upApproximateInversionForNear() && scfFlags.isNearestRound()) {
                  typename FloatConversion::Mantissa fcmMantissa = fcValue.mantissa();
                  int uShift = fcValue.querySizeMantissa() - bitSizeMantissa();
                  if (uShift > 0)
                     trightShift(fcmMantissa, uShift, 0U, scfFlags, fNegative, fcValue.querySizeMantissa());
               }
               else
                  scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
            };
            if (scfFlags.isInftyAvoided() && scfFlags.doesAvoidInfty(fNegative)) {
               biMantissa.neg();
               biExponent.dec();
               scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
            };
            return;
         };
         for (register int uIndex = (fcValue.querySizeExponent()-1)/(8*sizeof(unsigned int)); 
               uIndex >= 0; --uIndex)
            biExponent[uIndex] = fceNewExponent[uIndex];
         biExponent.normalize();

         typename FloatConversion::Mantissa fcmMantissa = fcValue.mantissa();
         int uShift = fcValue.querySizeMantissa() - bitSizeMantissa();
         if (uShift > 0) {
            trightShift(fcmMantissa, uShift, 0U, scfFlags, fNegative, fcValue.querySizeMantissa());
            if (fcmMantissa.cbitArray(bitSizeMantissa())) {
               ++biExponent;
               fcmMantissa.setFalseBitArray(bitSizeMantissa());
               if (biExponent == getInftyExponent()) {
                  scfFlags.setOverflow();
                  if (!biMantissa.isZero()) {
                     biMantissa = 0U;
                     if (!scfFlags.isApproximate())
                        scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
                  };
                  if (scfFlags.upApproximateInfty()) {
                     if (!(scfFlags.upApproximateInversionForNear() && scfFlags.isNearestRound()))
                        scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
                  };
                  if (scfFlags.isInftyAvoided() && scfFlags.doesAvoidInfty(fNegative)) {
                     biMantissa.neg();
                     biExponent.dec();
                     scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
                  };
                  return;
               };
            };
         };
         for (register int uIndex = (fcValue.querySizeExponent()-1)/(8*sizeof(unsigned int)); 
               uIndex >= 0; --uIndex)
            biMantissa[uIndex] = fcmMantissa[uIndex];
         biMantissa.normalize();
      }
      else {
         biExponent = 0U;
         fceNewExponent.neg().inc();
         if (fceNewExponent < bitSizeMantissa()) {
            typename FloatConversion::Mantissa fcmMantissa = fcValue.mantissa();
            int uShift = (fcValue.querySizeMantissa() - bitSizeMantissa())
               + fceNewExponent.queryValue() + 1;
            if (uShift > 0) {
               trightShift(fcmMantissa, uShift, 1U, scfFlags, fNegative, fcValue.querySizeMantissa());
               if (fcmMantissa.cbitArray(bitSizeMantissa())) {
                  ++biExponent;
                  fcmMantissa.setFalseBitArray(bitSizeMantissa());
               }
               else
                  scfFlags.setUnderflow();
            }
            else
               scfFlags.setUnderflow();
            for (register int uIndex = (fcValue.querySizeExponent()-1)/(8*sizeof(unsigned int)); 
                  uIndex >= 0; --uIndex)
               biMantissa[uIndex] = fcmMantissa[uIndex];
            biMantissa.normalize();
         }
         else {
            scfFlags.setUnderflow();
            bool fAdd = false;
            if (scfFlags.isNearestRound())
               fAdd = (fceNewExponent == bitSizeMantissa())
                  && (!scfFlags.isRoundToEven() || !fcValue.mantissa().isZero());
            else if (scfFlags.isHighestRound())
               fAdd = !fNegative;
            else if (scfFlags.isLowestRound())
               fAdd = fNegative;
            if (fNegative)
               scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
            else
               scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
            biMantissa = fAdd ? 1U : 0U;
         };
      };
   }
   else { // fcValue.querySizeExponent() == bitSizeExponent()
      for (register int uIndex = (fcValue.querySizeExponent()-1)/(8*sizeof(unsigned int)); 
            uIndex >= 0; --uIndex)
         biExponent[uIndex] = fcValue.exponent()[uIndex];
      biExponent.normalize();

      if (fcValue.querySizeMantissa() <= bitSizeMantissa()) {
         for (register int uIndex = (fcValue.querySizeMantissa()-1)/(8*sizeof(unsigned int)); 
               uIndex >= 0; --uIndex)
            biMantissa[uIndex] = fcValue.mantissa()[uIndex];
         biMantissa.normalize();
      }
      else { // fcValue.querySizeMantissa() > bitSizeMantissa()
         typename FloatConversion::Mantissa fcmSourceMantissa(fcValue.mantissa());
         trightShift(fcmSourceMantissa, (fcValue.querySizeMantissa() - bitSizeMantissa()),
            0U, scfFlags, fNegative, fcValue.querySizeMantissa());
         if (fcmSourceMantissa.cbitArray(bitSizeMantissa())) {
            ++biExponent;
            fcmSourceMantissa.setFalseBitArray(bitSizeMantissa());
            if (biExponent == getInftyExponent()) {
               scfFlags.setOverflow();
               biMantissa = 0U;
               if (!biMantissa.isZero()) {
                  biMantissa = 0U;
                  if (!scfFlags.isApproximate())
                     scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
               };
               if (scfFlags.upApproximateInfty()) {
                  if (!(scfFlags.upApproximateInversionForNear() && scfFlags.isNearestRound()))
                     scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
               };
               if (scfFlags.isInftyAvoided() && scfFlags.doesAvoidInfty(fNegative)) {
                  biMantissa.neg();
                  biExponent.dec();
                  scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
               };
               return;
            };
         };
         for (register int uIndex = (bitSizeMantissa()-1)/(8*sizeof(unsigned int)); 
               uIndex >= 0; --uIndex)
            biMantissa[uIndex] = fcmSourceMantissa[uIndex];
         biMantissa.normalize();
      };
   };
}

template <class TypeTraits>
void
TBuiltDouble<TypeTraits>::retrieveInteger(IntConversion& icResult, StatusAndControlFlags& scfFlags) const {
   bool fNaN = false;
   if (isNaN()) {
      if (isSNaN())
         scfFlags.setSNaNOperand();
      fNaN = true;
   };
   if (fNegative && icResult.isUnsigned()) {
      scfFlags.setUpApproximate();
      icResult = 0;
      return;
   };
   if (biExponent < getZeroExponent()) {
      bool fAdd = false;
      if (!isZero()) {
         if (scfFlags.isNearestRound()) {
            if ((fAdd = (biExponent == getMinusOneExponent())) != false)
               fAdd = !biMantissa.isZero();
         }
         else if (scfFlags.isHighestRound())
            fAdd = !fNegative;
         else if (scfFlags.isLowestRound())
            fAdd = fNegative;
         if (fNegative)
            scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
         else
            scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
      };
      icResult = icResult.isUnsigned() ? (fAdd ? 1 : 0)
                                       : (fAdd ? (fNegative ? -1 : 1) : 0);
      return;
   };
   Exponent biDigits = biExponent;
   biDigits.sub(getZeroExponent());
   if (biDigits >= (icResult.queryMaxDigits())) {
      if (!fNegative || (biDigits > (icResult.queryMaxDigits())) || !biMantissa.isZero()) {
         scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
         scfFlags.setOverflow();
      };
      if (fNegative || (fNaN && scfFlags.isConvertNaNNegative()))
         icResult.setMin();
      else
         icResult.setMax();
      return;
   };

   Mantissa biResult = biMantissa;
   if ((unsigned int) bitSizeMantissa() > biDigits.queryValue()) {
      if (trightShift(biResult, bitSizeMantissa()-biDigits.queryValue(), 1, scfFlags, fNegative, bitSizeMantissa())
            .hasCarry()
         || ((bitSizeMantissa() > icResult.queryMaxDigits()) && biResult.cbitArray(icResult.queryMaxDigits()))) {
         if (!fNegative || ((bitSizeMantissa() > icResult.queryMaxDigits())
               && !biResult.hasZero(icResult.queryMaxDigits()))) {
            scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
            scfFlags.setOverflow();
         };
         if (fNegative)
            icResult.setMin();
         else
            icResult.setMax();
         return;
      };
   }
   else {
      biResult <<= (biDigits.queryValue()-bitSizeMantissa());
      biResult.setTrueBitArray(biDigits.queryValue());
   };
   for (unsigned int uIndex = 0; uIndex < (icResult.querySize()-1)/(sizeof(unsigned int)*8)+1;
         ++uIndex)
      icResult[uIndex] = biResult[uIndex];
   if (fNegative)
      icResult.opposite();
}

template <class TypeTraits>
TBuiltDouble<TypeTraits>&
TBuiltDouble<TypeTraits>::plusAssignSureNN(const thisType& bdSource, StatusAndControlFlags& scfFlags) {
   assert(isNormalized() && bdSource.isNormalized());
   if (biExponent == bdSource.biExponent) {
      typename Mantissa::Carry cCarry = biMantissa.add(bdSource.biMantissa);
      trightShift(biMantissa, 1, cCarry.carry(), scfFlags, fNegative, bitSizeMantissa());
      biExponent.inc();
      if (biExponent == getInftyExponent()) {
         scfFlags.setOverflow();
         if (!biMantissa.isZero()) {
            biMantissa = 0U;
            if (!scfFlags.isApproximate())
               scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
         };
         if (scfFlags.upApproximateInfty())
            scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
         if (scfFlags.isInftyAvoided() && scfFlags.doesAvoidInfty(fNegative)) {
            biMantissa.neg();
            biExponent.dec();
            scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
         };
      };
      return *this;
   };
   if (biExponent < bdSource.biExponent) {
      if (bdSource.biExponent - biExponent > bitSizeMantissa()) {
         bool fAdd = false;
         bool fRoundToEven = false;
         if (scfFlags.isNearestRound()) {
            fAdd = ((bdSource.biExponent - biExponent) == bitSizeMantissa()+1);
            if (fAdd && scfFlags.isRoundToEven()) {
               fRoundToEven = biMantissa.isZero();
               if (fRoundToEven)
                  scfFlags.setEffectiveRoundToEven();
            };
         }
         else if (scfFlags.isHighestRound())
            fAdd = !fNegative;
         else if (scfFlags.isLowestRound())
            fAdd = fNegative;

         biExponent = bdSource.biExponent;
         biMantissa = bdSource.biMantissa;
         if (fAdd && ((fAdd = (!fRoundToEven || biMantissa.cbitArray(0))) != false)
               && biMantissa.inc().hasCarry()) {
            biExponent.inc();
            if (biExponent == getInftyExponent()) {
               scfFlags.setOverflow();
               if (scfFlags.isInftyAvoided() && scfFlags.doesAvoidInfty(fNegative)) {
                  biMantissa.neg();
                  biExponent.dec();
                  scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
                  return *this;
               }
               else if (scfFlags.upApproximateInfty()) {
                  scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
                  return *this;
               };
            };
         };
         if (fNegative)
            scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
         else
            scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
         return *this;
      };
      int uShift = (bdSource.biExponent - biExponent).queryValue();
      biExponent = bdSource.biExponent;
      register bool fOverflow
         = trightShift(biMantissa, uShift, 1, scfFlags, fNegative, bitSizeMantissa()).hasCarry();
      if (biMantissa.add(bdSource.biMantissa).hasCarry() || fOverflow) {
         trightShift(biMantissa, 1, 0, scfFlags, fNegative, bitSizeMantissa());
         biExponent.inc();
         if (biExponent == getInftyExponent()) {
            scfFlags.setOverflow();
            if (!biMantissa.isZero()) {
               biMantissa = 0U;
               if (!scfFlags.isApproximate())
                  scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
            };
            if (scfFlags.upApproximateInfty())
               scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
            if (scfFlags.isInftyAvoided() && scfFlags.doesAvoidInfty(fNegative)) {
               biMantissa.neg();
               biExponent.dec();
               scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
            };
         };
      }
      else if (scfFlags.hasEffectiveRoundToEven()) {
         if (biMantissa.cbitArray(0)) {
            if (scfFlags.isApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down)) {
               scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
               if (biMantissa.inc().hasCarry()) {
                  ++biExponent;
                  if (biExponent == getInftyExponent()) {
                     scfFlags.setOverflow();
                     if (!biMantissa.isZero()) {
                        biMantissa = 0U;
                        if (!scfFlags.isApproximate())
                           scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
                     };
                     if (scfFlags.upApproximateInfty())
                        scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
                     if (scfFlags.isInftyAvoided() && scfFlags.doesAvoidInfty(fNegative)) {
                        biMantissa.neg();
                        biExponent.dec();
                        scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
                     };
                  };
               };
            }
            else { // scfFlags.isApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up)
               biMantissa.setFalseBitArray(0);
               scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
            };
         };
      };
      return *this;
   };
   // biExponent > bdSource.biExponent

   if ((biExponent - bdSource.biExponent) > bitSizeMantissa()) {
      bool fAdd = false;
      bool fRoundToEven = false;
      if (scfFlags.isNearestRound()) {
         fAdd = ((biExponent - bdSource.biExponent) == bitSizeMantissa()+1);
         if (fAdd && scfFlags.isRoundToEven()) {
            fRoundToEven = bdSource.biMantissa.isZero();
            if (fRoundToEven)
               scfFlags.setEffectiveRoundToEven();
         };
      }
      else if (scfFlags.isHighestRound())
         fAdd = !fNegative;
      else if (scfFlags.isLowestRound())
         fAdd = fNegative;

      if (fAdd && ((fAdd = (!fRoundToEven || biMantissa.cbitArray(0))) != false)
            && biMantissa.inc().hasCarry()) {
         ++biExponent;
         if (biExponent == getInftyExponent()) {
            scfFlags.setOverflow();
            if (scfFlags.isInftyAvoided() && scfFlags.doesAvoidInfty(fNegative)) {
               biMantissa.neg();
               biExponent.dec();
               scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
               return *this;
            }
            else if (scfFlags.upApproximateInfty()) {
               scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
               return *this;
            };
         };
      };
      if (fNegative)
         scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
      else
         scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
      return *this;
   };
   int uShift = (biExponent - bdSource.biExponent).queryValue();
   Mantissa biSourceMantissa = bdSource.biMantissa;
   if (trightShift(biSourceMantissa, uShift, 1, scfFlags, fNegative, bitSizeMantissa()).hasCarry()
         || biMantissa.add(biSourceMantissa).hasCarry()) {
      trightShift(biMantissa, 1, 0, scfFlags, fNegative, bitSizeMantissa());
      biExponent.inc();
      if (biExponent == getInftyExponent()) {
         scfFlags.setOverflow();
         if (!biMantissa.isZero()) {
            biMantissa = 0U;
            if (!scfFlags.isApproximate())
               scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
         };
         if (scfFlags.upApproximateInfty())
            scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
         if (scfFlags.isInftyAvoided() && scfFlags.doesAvoidInfty(fNegative)) {
            biMantissa.neg();
            biExponent.dec();
            scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
         };
      };
   }
   else if (scfFlags.hasEffectiveRoundToEven()) {
      if (biMantissa.cbitArray(0)) {
         if (scfFlags.isApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down)) {
            scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
            if (biMantissa.inc().hasCarry()) {
               ++biExponent;
               if (biExponent == getInftyExponent()) {
                  scfFlags.setOverflow();
                  if (!biMantissa.isZero()) {
                     biMantissa = 0U;
                     if (!scfFlags.isApproximate())
                        scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
                  };
                  if (scfFlags.upApproximateInfty())
                     scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
                  if (scfFlags.isInftyAvoided() && scfFlags.doesAvoidInfty(fNegative)) {
                     biMantissa.neg();
                     biExponent.dec();
                     scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
                  };
               };
            };
         }
         else { // scfFlags.isApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up)
            biMantissa.setFalseBitArray(0);
            scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
         };
      };
   };
   return *this;
}

template <class TypeTraits>
TBuiltDouble<TypeTraits>&
TBuiltDouble<TypeTraits>::plusAssignSureND(const thisType& bdSource, StatusAndControlFlags& scfFlags) {
   assert(isNormalized() && (bdSource.biExponent.isZero()));
   int uMaxShift = bdSource.biMantissa.log_base_2();
   if ((uMaxShift == 1) && (bdSource.biMantissa.queryValue() == 0))
      return *this;
   if (biExponent > uMaxShift) {
      bool fAdd = false;
      bool fRoundToEven = false;
      if (scfFlags.isNearestRound()) {
         fAdd = biExponent == uMaxShift+1;
         if (fAdd && scfFlags.isRoundToEven()) {
            fRoundToEven = bdSource.biMantissa.isZero();
            if (fRoundToEven)
               scfFlags.setEffectiveRoundToEven();
         };
      }
      else if (scfFlags.isHighestRound())
         fAdd = !fNegative;
      else if (scfFlags.isLowestRound())
         fAdd = fNegative;
      if (fAdd && ((fAdd = (!fRoundToEven || biMantissa.cbitArray(0))) != false)
            && biMantissa.inc().hasCarry())
         ++biExponent;
      if (fNegative)
         scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
      else
         scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
      return *this;
   };
   Mantissa biSourceMantissa = bdSource.biMantissa;
   if (trightShift(biSourceMantissa, biExponent.queryValue() - 1, 0, scfFlags, fNegative,
            bitSizeMantissa()).hasCarry()
         || biMantissa.add(biSourceMantissa).hasCarry()) {
      trightShift(biMantissa, 1, 0, scfFlags, fNegative, bitSizeMantissa());
      biExponent.inc();
      if (biExponent == getInftyExponent()) {
         scfFlags.setOverflow();
         if (!biMantissa.isZero()) {
            biMantissa = 0U;
            if (!scfFlags.isApproximate())
               scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
         };
         if (scfFlags.upApproximateInfty())
            scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
         if (scfFlags.isInftyAvoided() && scfFlags.doesAvoidInfty(fNegative)) {
            biMantissa.neg();
            biExponent.dec();
            scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
         };
      };
   }
   else if (scfFlags.hasEffectiveRoundToEven()) {
      if (biMantissa.cbitArray(0)) {
         if (scfFlags.isApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down)) {
            scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
            if (biMantissa.inc().hasCarry()) {
               ++biExponent;
               if (biExponent == getInftyExponent()) {
                  scfFlags.setOverflow();
                  if (!biMantissa.isZero()) {
                     biMantissa = 0U;
                     if (!scfFlags.isApproximate())
                        scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
                  };
                  if (scfFlags.upApproximateInfty())
                     scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
                  if (scfFlags.isInftyAvoided() && scfFlags.doesAvoidInfty(fNegative)) {
                     biMantissa.neg();
                     biExponent.dec();
                     scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
                  };
               };
            };
         }
         else { // scfFlags.isApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up)
            scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
            biMantissa.setFalseBitArray(0);
         };
      };
   };
   return *this;
}

template <class TypeTraits>
TBuiltDouble<TypeTraits>&
TBuiltDouble<TypeTraits>::plusAssignSureDN(const thisType& bdSource, StatusAndControlFlags& scfFlags) {
   assert(biExponent.isZero() && bdSource.isNormalized());
   int uMaxShift = biMantissa.log_base_2();
   if ((uMaxShift == 1) && (biMantissa.queryValue() == 0)) {
      biExponent = bdSource.biExponent;
      biMantissa = bdSource.biMantissa;
      return *this;
   };
   if (bdSource.biExponent > uMaxShift) {
      bool fAdd = false;
      bool fRoundToEven = false;
      if (scfFlags.isNearestRound()) {
         fAdd = (bdSource.biExponent == uMaxShift+1);
         if (fAdd && scfFlags.isRoundToEven()) {
            fRoundToEven = biMantissa.isZero();
            if (fRoundToEven)
               scfFlags.setEffectiveRoundToEven();
         };
      }
      else if (scfFlags.isHighestRound())
         fAdd = !fNegative;
      else if (scfFlags.isLowestRound())
         fAdd = fNegative;
      biExponent = bdSource.biExponent;
      biMantissa = bdSource.biMantissa;
      if (fAdd && ((fAdd = (!fRoundToEven || biMantissa.cbitArray(0))) != false)
            && biMantissa.inc().hasCarry()) {
         biExponent.inc();
         if (biExponent == getInftyExponent()) {
            if (scfFlags.upApproximateInfty())
               scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
            scfFlags.setOverflow();
            if (scfFlags.isInftyAvoided() && scfFlags.doesAvoidInfty(fNegative)) {
               biMantissa.neg();
               biExponent.dec();
               scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
               return *this;
            };
         };
      };
      if (fNegative)
         scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
      else
         scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
      return *this;
   };
   biExponent = bdSource.biExponent;
   register bool fOverflow = trightShift(biMantissa, bdSource.biExponent.queryValue() - 1, 0,
      scfFlags, fNegative, bitSizeMantissa()).hasCarry();
   if (biMantissa.add(bdSource.biMantissa).hasCarry() || fOverflow) {
      trightShift(biMantissa, 1, 0, scfFlags, fNegative, bitSizeMantissa());
      biExponent.inc();
      if (biExponent == getInftyExponent()) {
         scfFlags.setOverflow();
         if (!biMantissa.isZero()) {
            biMantissa = 0U;
            if (!scfFlags.isApproximate())
               scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
         };
         if (scfFlags.upApproximateInfty())
            scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
         if (scfFlags.isInftyAvoided() && scfFlags.doesAvoidInfty(fNegative)) {
            biMantissa.neg();
            biExponent.dec();
            scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
         };
      };
   }
   else if (scfFlags.hasEffectiveRoundToEven()) {
      if (biMantissa.cbitArray(0)) {
         if (scfFlags.isApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down)) {
            scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
            if (biMantissa.inc().hasCarry()) {
               biExponent.inc();
               if (biExponent == getInftyExponent()) {
                  scfFlags.setOverflow();
                  if (scfFlags.upApproximateInfty())
                     scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
                  if (scfFlags.isInftyAvoided() && scfFlags.doesAvoidInfty(fNegative)) {
                     biMantissa.neg();
                     biExponent.dec();
                     scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
                  };
               };
            };
         }
         else { // scfFlags.isApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up)
            scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
            biMantissa.setFalseBitArray(0);
         };
      };
   };
   return *this;
}

template <class TypeTraits>
TBuiltDouble<TypeTraits>&
TBuiltDouble<TypeTraits>::plusAssignSureDD(const thisType& bdSource, StatusAndControlFlags& scfFlags) {
   assert(biExponent.isZero() && bdSource.biExponent.isZero());
   if (biMantissa.add(bdSource.biMantissa).hasCarry())
      biExponent.inc();
   scfFlags.setUnderflow();
   return *this;
}

template <class TypeTraits>
TBuiltDouble<TypeTraits>&
TBuiltDouble<TypeTraits>::minusAssignSureNN(const thisType& bdSource, StatusAndControlFlags& scfFlags) {
   assert(isNormalized() && bdSource.isNormalized());
   if (biExponent == bdSource.biExponent) {
      typename Mantissa::Carry cCarry = biMantissa.sub(bdSource.biMantissa);
      assert(!cCarry.hasCarry());
      if (biMantissa.isZero()) {
         biExponent = 0U;
         fNegative = (scfFlags.acceptMinusZero() && scfFlags.isLowestRound()) ? true : false;
         scfFlags.setUnderflow();
      }
      else {
         int uShift = bitSizeMantissa() - biMantissa.log_base_2() + 1;
         biMantissa <<= uShift;
         if (biExponent <= uShift) {
            biMantissa >>= (uShift-biExponent.queryValue()+1);
            biMantissa.setTrueBitArray(bitSizeMantissa()-(uShift-biExponent.queryValue()+1));
            biExponent = 0U;
            scfFlags.setUnderflow();
         }
         else
#ifndef __BORLANDC__
            biExponent -= Exponent(typename Exponent::Basic(), uShift);
#else
            biExponent -= Exponent(Exponent::Basic(), uShift);
#endif
      };
      return *this;
   };

   assert(biExponent > bdSource.biExponent);
   if ((biExponent - bdSource.biExponent) > bitSizeMantissa()) {
      bool fSub = false;
      bool fRoundToEven = false;
      if (scfFlags.isNearestRound()) {
         fSub = ((biExponent - bdSource.biExponent) == bitSizeMantissa()+1);
         if (fSub && scfFlags.isRoundToEven()) {
            fRoundToEven = bdSource.biMantissa.isZero();
            if (fRoundToEven)
               scfFlags.setEffectiveRoundToEven();
         };
      }
      else if (scfFlags.isHighestRound())
         fSub = fNegative;
      else if (scfFlags.isLowestRound())
         fSub = !fNegative;
      else // scfFlags.isZeroRound()
         fSub = true;
      if (fSub && ((fSub = (!fRoundToEven || biMantissa.cbitArray(0))) != false)
            && biMantissa.dec().hasCarry()) {
         --biExponent;
         if ((biExponent - bdSource.biExponent) == bitSizeMantissa()+1) {
            biMantissa.setFalseBitArray(0);
            if (bdSource.biMantissa.isZero()) // no approximation
               return *this;
         };
      };
      if (fNegative)
         scfFlags.setApproximate(fSub ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
      else
         scfFlags.setApproximate(fSub ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
      return *this;
   };

   int uShift = (biExponent - bdSource.biExponent).queryValue();
   Mantissa biSourceMantissa(bdSource.biMantissa);
   if (uShift == 1) {
      bool fMinor = biSourceMantissa.cbitArray(0);
      biSourceMantissa >>= 1;
      biSourceMantissa.setTrueBitArray(bitSizeMantissa()-1);

      if (biMantissa.sub(biSourceMantissa).hasCarry()) {
         int uNewShift = bitSizeMantissa() - biMantissa.log_base_2() + 1;
         if (fMinor) {
            assert(biMantissa.cbitArray(bitSizeMantissa() - uNewShift));
            biMantissa <<= 1;
            if (biMantissa.dec().hasCarry() || ((uNewShift > 1)
                  && (!biMantissa.cbitArray(bitSizeMantissa() - uNewShift + 1))))
              ++uNewShift;
            if (uNewShift > 1)
               biMantissa <<= (uNewShift-1);
         }
         else
            biMantissa <<= uNewShift;

         if (biExponent <= uNewShift) {
            biMantissa >>= (uNewShift-biExponent.queryValue()+1);
            biMantissa.setTrueBitArray(bitSizeMantissa()-(uNewShift-biExponent.queryValue()+1));
            biExponent = 0;
            scfFlags.setUnderflow();
         }
         else
#ifndef __BORLANDC__
            biExponent -= Exponent(typename Exponent::Basic(), uNewShift);
#else
            biExponent -= Exponent(Exponent::Basic(), uNewShift);
#endif
      }
      else if (fMinor) {
         bool fSub = false;
         if (scfFlags.isHighestRound())
            fSub = fNegative;
         else if (scfFlags.isLowestRound())
            fSub = !fNegative;
         else // scfFlags.isZeroRound()
            fSub = true;
         if ((biMantissa.isZero() && !scfFlags.isApproximate() && biMantissa.dec().hasCarry())
               || (fSub && ((fSub = (!scfFlags.isRoundToEven() || biMantissa.cbitArray(0))) != false)
                     && biMantissa.dec().hasCarry())) {
            --biExponent;
            return *this; // no precision loss
         };
         if (fNegative)
            scfFlags.setApproximate(fSub ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
         else
            scfFlags.setApproximate(fSub ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
      };

      return *this;
   };

   bool fMinorTwo = biSourceMantissa.cbitArray(uShift-1),
        fMinorOne = biSourceMantissa.cbitArray(uShift-2);
   bool fSureApproximate = !biSourceMantissa.hasZero(uShift-2);
   biSourceMantissa >>= uShift;
   biSourceMantissa.setTrueBitArray(bitSizeMantissa()-uShift);
   bool fHasDigits = !biMantissa.sub(biSourceMantissa).hasCarry();
   if (!fHasDigits) {
      biExponent.dec();
      fHasDigits = biExponent.isZero();
   };
   if (fHasDigits) {
      if (fMinorOne || fMinorTwo || fSureApproximate) {
         bool fSub = false;
         bool fRoundToEven = false;
         if (scfFlags.isNearestRound()) {
            fSub = fMinorTwo;
            if (fSub && scfFlags.isRoundToEven()) {
               fRoundToEven = !fMinorOne && !fSureApproximate;
               if (fRoundToEven)
                  scfFlags.setEffectiveRoundToEven();
            };
         }
         else if (scfFlags.isHighestRound())
            fSub = fNegative;
         else if (scfFlags.isLowestRound())
            fSub = !fNegative;
         else // scfFlags.isZeroRound()
            fSub = true;
         if ((biMantissa.isZero() && !fMinorOne && !fSureApproximate
                  && !scfFlags.isApproximate() && biMantissa.dec().hasCarry())
            || (fSub && ((fSub = (!fRoundToEven || biMantissa.cbitArray(0))) != false)
                  && biMantissa.dec().hasCarry())) {
            biExponent.dec();
            biMantissa.setBitArray(0, fMinorTwo ? !fMinorOne : fMinorOne);
            if (scfFlags.isNearestRound())
               fSub = fMinorOne;
            if (!fMinorOne && !fSureApproximate)
               return *this;
         };
         if (fNegative)
            scfFlags.setApproximate(fSub ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
         else
            scfFlags.setApproximate(fSub ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
      };
      return *this;
   };

   biMantissa <<= 1;
   if (fMinorTwo)
      biMantissa.dec();
   if (fMinorOne || fSureApproximate) {
      bool fSub = false;
      if (scfFlags.isNearestRound())
         fSub = fMinorOne && (!scfFlags.isRoundToEven() || fSureApproximate || biMantissa.cbitArray(0));
      else if (scfFlags.isHighestRound())
         fSub = fNegative;
      else if (scfFlags.isLowestRound())
         fSub = !fNegative;
      else // scfFlags.isZeroRound()
         fSub = true;
      if (fSub)
         biMantissa.dec();
      if (fNegative)
         scfFlags.setApproximate(fSub ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
      else
         scfFlags.setApproximate(fSub ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
   };
   return *this;
}

template <class TypeTraits>
TBuiltDouble<TypeTraits>&
TBuiltDouble<TypeTraits>::minusAssignSureND(const thisType& bdSource, StatusAndControlFlags& scfFlags) {
   assert(isNormalized() && bdSource.biExponent.isZero());
   if (bdSource.biMantissa.isZero())
      return *this;

   int uNbSourceDigits = bdSource.biMantissa.log_base_2();
   if (biExponent > uNbSourceDigits+1) {
      bool fSub = false;
      bool fRoundToEven = false;
      if (scfFlags.isNearestRound()) {
         fSub = (biExponent == uNbSourceDigits);
         if (fSub && scfFlags.isRoundToEven()) {
            fRoundToEven = bdSource.biMantissa.isZero();
            if (fRoundToEven)
               scfFlags.setEffectiveRoundToEven();
         };
      }
      else if (scfFlags.isHighestRound())
         fSub = fNegative;
      else if (scfFlags.isLowestRound())
         fSub = !fNegative;
      else // scfFlags.isZeroRound()
         fSub = true;
      if (fSub && ((fSub = (!fRoundToEven || biMantissa.cbitArray(0))) != false)
            && biMantissa.dec().hasCarry()) {
         biExponent.dec();
         if (biExponent == uNbSourceDigits) {
            biMantissa.setFalseBitArray(0);
            if (bdSource.biMantissa.hasZero(uNbSourceDigits-1)) // no approximation
               return *this;
         };
      };
      if (fNegative)
         scfFlags.setApproximate(fSub ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
      else
         scfFlags.setApproximate(fSub ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
      return *this;
   };

   int uSourceShift = biExponent.queryValue()-1;
   if (uSourceShift == 0) {
      if (biMantissa.sub(bdSource.biMantissa).hasCarry()) {
         biExponent = 0;
         scfFlags.setUnderflow();
      };
      return *this;
   };

   Mantissa biSourceMantissa(bdSource.biMantissa);
   bool fMinorTwo = biSourceMantissa.cbitArray(uSourceShift-1),
        fMinorOne = (uSourceShift > 1) ? biSourceMantissa.cbitArray(uSourceShift-2) : 0;
   bool fSureApproximate = (uSourceShift > 1) ? !biSourceMantissa.hasZero(uSourceShift-2) : false;
   biSourceMantissa >>= uSourceShift;
   if (!biMantissa.sub(biSourceMantissa).hasCarry()) {
      if (fMinorOne || fMinorTwo || fSureApproximate) {
         bool fSub = false;
         bool fRoundToEven = false;
         if (scfFlags.isNearestRound()) {
            fSub = fMinorTwo;
            if (fSub && scfFlags.isRoundToEven()) {
               fRoundToEven = !fMinorOne && !fSureApproximate;
               if (fRoundToEven)
                  scfFlags.setEffectiveRoundToEven();
            };
         }
         else if (scfFlags.isHighestRound())
            fSub = fNegative;
         else if (scfFlags.isLowestRound())
            fSub = !fNegative;
         else // scfFlags.isZeroRound()
            fSub = true;
         if (fSub && ((fSub = (!fRoundToEven || biMantissa.cbitArray(0))) != false)
               && biMantissa.dec().hasCarry()) {
            biExponent.dec();
            biMantissa.setBitArray(0, fMinorTwo ? !fMinorOne : fMinorOne);
            if (scfFlags.isNearestRound())
               fSub = fMinorOne;
            if (!fMinorOne && !fSureApproximate)
               return *this;
         };
         if (fNegative)
            scfFlags.setApproximate(fSub ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
         else
            scfFlags.setApproximate(fSub ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
      };
      return *this;
   };

   biExponent.dec();
   biMantissa <<= 1;
   if (fMinorTwo) {
      if (biMantissa.dec().hasCarry())
         biExponent.dec();
   };
   if (fMinorOne || fSureApproximate) {
      bool fSub = false;
      if (scfFlags.isNearestRound())
         fSub = fMinorOne && (!scfFlags.isRoundToEven() || fSureApproximate || biMantissa.cbitArray(0));
      else if (scfFlags.isHighestRound())
         fSub = fNegative;
      else if (scfFlags.isLowestRound())
         fSub = !fNegative;
      else // scfFlags.isZeroRound()
         fSub = true;
      if (fSub) {
         if (biMantissa.dec().hasCarry())
            biExponent.dec();
      };
      if (fNegative)
         scfFlags.setApproximate(fSub ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
      else
         scfFlags.setApproximate(fSub ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
   };
   return *this;
}

template <class TypeTraits>
TBuiltDouble<TypeTraits>&
TBuiltDouble<TypeTraits>::minusAssignSureDD(const thisType& bdSource, StatusAndControlFlags& scfFlags) {
   assert(biExponent.isZero() && bdSource.biExponent.isZero());
   typename Mantissa::Carry cCarry = biMantissa.sub(bdSource.biMantissa);
   scfFlags.setUnderflow();
   assert(!cCarry.hasCarry());
   if (biMantissa.isZero())
      fNegative = (scfFlags.acceptMinusZero() && scfFlags.isLowestRound()) ? true : false;
   return *this;
}

template <class TypeTraits>
TBuiltDouble<TypeTraits>&
TBuiltDouble<TypeTraits>::plusAssign(const thisType& bdSource, StatusAndControlFlags& scfFlags) {
   if (!biExponent.isZero()) {
      if (biExponent != getInftyExponent()) {
         if (!bdSource.biExponent.isZero()) {
            if (bdSource.biExponent != getInftyExponent()) {
               if (fNegative == bdSource.fNegative)
                  return plusAssignSureNN(bdSource, scfFlags);
               typename Exponent::ComparisonResult crCompareExponent = biExponent.compare(bdSource.biExponent);
               if ((crCompareExponent == Exponent::CRGreater)
                     || ((crCompareExponent == Exponent::CREqual) && biMantissa >= bdSource.biMantissa))
                  return minusAssignSureNN(bdSource, scfFlags);
               thisType ttThisCopy = *this;
               *this = bdSource;
               minusAssignSureNN(ttThisCopy, scfFlags);
               return *this;
            };
            // bdSource.biExponent == getInftyExponent()
            fNegative = bdSource.fNegative;
            biMantissa = bdSource.biMantissa;
            if (!biMantissa.isZero()) {
               if (!biMantissa.cbitArray(bitSizeMantissa()-1)) {
                  scfFlags.setSNaNOperand();
                  biMantissa.setTrueBitArray(bitSizeMantissa()-1);
               };
            };
            biExponent = getInftyExponent();
            return *this;
         };

         if (fNegative == bdSource.fNegative)
            return plusAssignSureND(bdSource, scfFlags);
         return minusAssignSureND(bdSource, scfFlags);
      };

      // biExponent == getInftyExponent()
      if (biMantissa.isZero()) {
         if (bdSource.biExponent == getInftyExponent()) {
            if (!bdSource.biMantissa.isZero()) {
               biMantissa = bdSource.biMantissa;
               if (!biMantissa.cbitArray(bitSizeMantissa()-1)) {
                  scfFlags.setSNaNOperand();
                  biMantissa.setTrueBitArray(bitSizeMantissa()-1);
               };
               fNegative = bdSource.fNegative;
            }
            else if (bdSource.fNegative != fNegative) {
               scfFlags.setInftyMinusInfty();
               biMantissa = 0U;
               biMantissa.neg();
               biMantissa.setFalseBitArray(bitSizeMantissa()-1);
            };
         };
      }
      else {
         if (!biMantissa.cbitArray(bitSizeMantissa()-1)) {
            scfFlags.setSNaNOperand();
            biMantissa.setTrueBitArray(bitSizeMantissa()-1);
         }
         else if (bdSource.isSNaN())
            scfFlags.setSNaNOperand();
      };
      return *this;
   };
   
   // biExponent == 0
   if (bdSource.biExponent.isZero()) {
      if (fNegative == bdSource.fNegative)
         return plusAssignSureDD(bdSource, scfFlags);
      if (biMantissa >= bdSource.biMantissa)
         minusAssignSureDD(bdSource, scfFlags);
      else {
         thisType ttThisCopy = *this;
         *this = bdSource;
         minusAssignSureDD(ttThisCopy, scfFlags);
      };
      return *this;
   };
      
   // biExponent == 0 && bdSource.biExponent != 0
   if (bdSource.biExponent == getInftyExponent()) {
      fNegative = bdSource.fNegative;
      biExponent = bdSource.biExponent;
      biMantissa = bdSource.biMantissa;
      if (!biMantissa.isZero()) {
         if (!biMantissa.cbitArray(bitSizeMantissa()-1)) {
            scfFlags.setSNaNOperand();
            biMantissa.setTrueBitArray(bitSizeMantissa()-1);
         };
      };
      return *this;
   };
   
   // biExponent == 0 && bdSource.biExponent != 0 && bdSource.biExponent != getInftyExponent()
   if (fNegative == bdSource.fNegative)
      return plusAssignSureDN(bdSource, scfFlags);
   thisType ttThisCopy = *this;
   *this = bdSource;
   minusAssignSureND(ttThisCopy, scfFlags);
   return *this;
}

template <class TypeTraits>
TBuiltDouble<TypeTraits>&
TBuiltDouble<TypeTraits>::minusAssign(const thisType& bdSource, StatusAndControlFlags& scfFlags) {
   if (!biExponent.isZero()) {
      if (biExponent != getInftyExponent()) {
         if (!bdSource.biExponent.isZero()) {
            if (bdSource.biExponent != getInftyExponent()) {
               if (fNegative != bdSource.fNegative)
                  return plusAssignSureNN(bdSource, scfFlags);
               typename Mantissa::ComparisonResult crCompareExponent = biExponent.compare(bdSource.biExponent);
               if ((crCompareExponent == Mantissa::CRGreater)
                     || ((crCompareExponent == Mantissa::CREqual) && biMantissa >= bdSource.biMantissa))
                  return minusAssignSureNN(bdSource, scfFlags);
               thisType ttThisCopy = *this;
               *this = bdSource;
               fNegative = !fNegative;
               minusAssignSureNN(ttThisCopy, scfFlags);
               return *this;
            };
            
            // bdSource.biExponent == getInftyExponent()
            if (scfFlags.keepNaNSign())
               fNegative = bdSource.fNegative;
            biMantissa = bdSource.biMantissa;
            if (!biMantissa.isZero()) {
               if (!biMantissa.cbitArray(bitSizeMantissa()-1)) {
                  scfFlags.setSNaNOperand();
                  biMantissa.setTrueBitArray(bitSizeMantissa()-1);
               };
            };
            biExponent = bdSource.biExponent;
            return *this;
         };

         // bdSource.biExponent == 0
         if (fNegative != bdSource.fNegative)
            return plusAssignSureND(bdSource, scfFlags);
         minusAssignSureND(bdSource, scfFlags);
         return *this;
      };

      // biExponent == getInftyExponent()
      if (biMantissa.isZero()) {
         if (bdSource.biExponent == getInftyExponent()) {
            if (!bdSource.biMantissa.isZero()) {
               biMantissa = bdSource.biMantissa;
               if (!biMantissa.cbitArray(bitSizeMantissa()-1)) {
                  scfFlags.setSNaNOperand();
                  biMantissa.setTrueBitArray(bitSizeMantissa()-1);
               };
               fNegative = bdSource.fNegative;
            }
            else if (bdSource.fNegative == fNegative) {
               scfFlags.setInftyMinusInfty();
               biMantissa = 0U;
               biMantissa.neg();
               biMantissa.setFalseBitArray(bitSizeMantissa()-1);
            };
         };
      }
      else {
         if (!biMantissa.cbitArray(bitSizeMantissa()-1)) {
            scfFlags.setSNaNOperand();
            biMantissa.setTrueBitArray(bitSizeMantissa()-1);
         }
         else if (bdSource.isSNaN())
            scfFlags.setSNaNOperand();
      };

      return *this;
   };
   
   // biExponent == 0
   if (bdSource.biExponent.isZero()) {
      if (fNegative != bdSource.fNegative)
         return plusAssignSureDD(bdSource, scfFlags);
      if (biMantissa >= bdSource.biMantissa)
         minusAssignSureDD(bdSource, scfFlags);
      else {
        thisType ttThisCopy = *this;
        *this = bdSource;
        fNegative = !fNegative;
        minusAssignSureDD(ttThisCopy, scfFlags);
      };
      return *this;
   };

   // biExponent == 0 && bdSource.biExponent != 0
   if (bdSource.biExponent == getInftyExponent()) {
      fNegative = !bdSource.fNegative;
      biExponent = getInftyExponent();
      biMantissa = bdSource.biMantissa;
      if (!biMantissa.isZero()) {
         if (!biMantissa.cbitArray(bitSizeMantissa()-1)) {
            scfFlags.setSNaNOperand();
            biMantissa.setTrueBitArray(bitSizeMantissa()-1);
         };
         if (scfFlags.keepNaNSign())
            fNegative = !fNegative;
      };
      return *this;
   };
   
   // biExponent == 0 && bdSource.biExponent != 0 && bdSource.biExponent != getInftyExponent()
   if (fNegative != bdSource.fNegative)
      return plusAssignSureDN(bdSource, scfFlags);
   thisType ttThisCopy = *this;
   *this = bdSource;
   fNegative = !fNegative;
   minusAssignSureND(ttThisCopy, scfFlags);
   return *this;
}

template <class TypeTraits>
TBuiltDouble<TypeTraits>&
TBuiltDouble<TypeTraits>::multAssign(unsigned int uValue, StatusAndControlFlags& scfFlags) {
   if (uValue != 0) {
      register int uValueDigits = log_base_2(uValue);
      if (isNormalized()) {
         typename TypeTraits::ExtendedMantissa emMantissa(biMantissa);
         typename TypeTraits::ExtendedMantissa::Carry cCarry = emMantissa.multAssign(uValue);
         if (cCarry.hasCarry()) {
            assert((uValueDigits == log_base_2(cCarry.carry()))
               || (uValueDigits == 1+log_base_2(cCarry.carry())));
            if ((cCarry.carry() & (1U << (uValueDigits-1))) == 0) {
               --uValueDigits;
               assert((cCarry.carry() & (1U << (uValueDigits-1))) != 0);
            };
            trightShift(emMantissa, uValueDigits, cCarry.carry() & ~(~0U << (uValueDigits-1)),
               scfFlags, fNegative, bitSizeMantissa()+1);
            biMantissa = emMantissa;
            register bool fOverflowExponent = emMantissa.cbitArray(bitSizeMantissa())
                ? biExponent.plusAssign(uValueDigits + 1).hasCarry()
                : biExponent.plusAssign(uValueDigits).hasCarry();
            if (fOverflowExponent || (biExponent >= getInftyExponent())) {
               scfFlags.setOverflow();
               if (fOverflowExponent || (biExponent > getInftyExponent()) || !biMantissa.isZero()) {
                  biExponent = getInftyExponent();
                  biMantissa = 0U;
                  if (!scfFlags.isApproximate())
                     scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
               };
               if (scfFlags.isInftyAvoided() && scfFlags.doesAvoidInfty(fNegative)) {
                  biMantissa.neg();
                  biExponent.dec();
                  scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
               }
               else if (scfFlags.upApproximateInfty())
                  scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
            };
         };
      }
      else if (biExponent.isZero()) {
         typename TypeTraits::Mantissa::Carry cCarry = biMantissa.multAssign(uValue);
         if (cCarry.hasCarry()) {
            int uShift = log_base_2(cCarry.carry())-1;
            if (uShift > 0) {
               trightShift(biMantissa, uShift, cCarry.carry(), scfFlags, fNegative, bitSizeMantissa());
               biExponent = uShift+1;
            }
            else
               biExponent = 1;
         }
         else
            scfFlags.setUnderflow();
      }
      else { // biExponent == getInftyExponent();
         if (!queryMantissa().isZero())
            biMantissa.setTrueBitArray(bitSizeMantissa()-1);
      };
   }
   else if (biExponent != getInftyExponent()) { // uValue == 0
      biMantissa = 0U;
      biExponent = 0U;
      scfFlags.setUnderflow();
   }
   else { // biExponent == getInftyExponent()) -> NaN
      if (biMantissa.isZero()) {
         biMantissa = 0U;
         biMantissa.neg();
         biMantissa.setFalseBitArray(bitSizeMantissa()-1);
      }
      else
         biMantissa.setTrueBitArray(bitSizeMantissa()-1);
   };
   return *this;
}

template <class TypeTraits>
void
TBuiltDouble<TypeTraits>::addExtension(const thisType& bdSource,
      typename Integer::TBigInt<typename TypeTraits::ExtendedMantissa::MultResult>& mprResult,
      StatusAndControlFlags& scfFlags, bool fPositiveAdd, int& uLogResult, bool& fExponentHasCarry,
      bool& fResultPositiveExponent, bool& fAddExponent) {
   TBuiltDouble<typename TypeTraits::MultExtension> bdMult, bdAdd;
   bdMult.setSign(!fNegative);
   for (register int uIndex = 0; uIndex < biExponent.queryCellSize(); ++uIndex)
      bdMult.querySBasicExponent()[uIndex] = biExponent[uIndex];
   for (register int uIndex = 0; uIndex < mprResult.queryCellSize(); ++uIndex)
      bdMult.querySMantissa()[uIndex] = mprResult[uIndex];
   bdMult.querySMantissa().normalize();
   if (uLogResult < 2*bitSizeMantissa()+1)
      bdMult.querySMantissa() <<= 2*bitSizeMantissa()-uLogResult+1;
   if (bdMult.querySBasicExponent().cbitArray(bitSizeExponent()-1)) {
      bdMult.querySBasicExponent().setFalseBitArray(bitSizeExponent()-1);
      bdMult.querySBasicExponent().setTrueBitArray(bitSizeExponent());
      if (fExponentHasCarry) {
         if (fResultPositiveExponent)
            bdMult.querySBasicExponent().setTrueBitArray(bitSizeExponent()-1);
         else {
            bdMult.querySBasicExponent().setFalseBitArray(bitSizeExponent());
            if (bdMult.querySBasicExponent().isZero())
               bdMult.querySBasicExponent() = 1U;
         };
      };
   }
   else {
      if (fExponentHasCarry) {
         if (fResultPositiveExponent) {
            bdMult.querySBasicExponent().setTrueBitArray(bitSizeExponent()-1);
            bdMult.querySBasicExponent().setTrueBitArray(bitSizeExponent());
         }
         else {
            if (!bdMult.queryBasicExponent().cbitArray(bitSizeExponent()-1))
               bdMult.querySBasicExponent() = 1U;
            else
               bdMult.querySBasicExponent().setFalseBitArray(bitSizeExponent()-1);
         };
      }
      else
         bdMult.querySBasicExponent().setTrueBitArray(bitSizeExponent()-1);
   };
   uLogResult = 2*bitSizeMantissa()+1;
   if (fAddExponent) {
      bdMult.querySBasicExponent().inc();
      fAddExponent = false;
   };

   bdAdd.setSign(fPositiveAdd ? bdSource.isPositive() : !bdSource.isPositive());
   for (register int uIndex = 0; uIndex < bdSource.queryBasicExponent().queryCellSize(); ++uIndex)
      bdAdd.querySBasicExponent()[uIndex] = bdSource.queryBasicExponent()[uIndex];
   for (register int uIndex = 0; uIndex < bdSource.queryMantissa().queryCellSize(); ++uIndex)
      bdAdd.querySMantissa()[uIndex] = bdSource.queryMantissa()[uIndex];
   int uDenormalizedShift = 0;
   if (bdSource.queryBasicExponent().isZero()) {
      if (bdSource.queryMantissa().isZero())
         goto LSetResult;
      uDenormalizedShift = bitSizeMantissa()-bdSource.queryMantissa().log_base_2()+1;
      scfFlags.clearFlowException();
   };
   bdAdd.querySMantissa() <<= bitSizeMantissa()+1+uDenormalizedShift;
   if (bdAdd.querySBasicExponent().cbitArray(bitSizeExponent()-1)) {
      bdAdd.querySBasicExponent().setFalseBitArray(bitSizeExponent()-1);
      bdAdd.querySBasicExponent().setTrueBitArray(bitSizeExponent());
   }
   else {
      bdAdd.querySBasicExponent().setTrueBitArray(bitSizeExponent()-1);
      if (uDenormalizedShift > 1)
         bdAdd.querySBasicExponent().sub(uDenormalizedShift-1);
   };

   if (bdMult.isNegative() == bdAdd.isNegative())
      bdMult.plusAssignSureNN(bdAdd, scfFlags);
   else if (bdMult.isNegative() ? (bdMult <= bdAdd.opposite()) : (bdMult >= bdAdd.opposite()))
      bdMult.minusAssignSureNN(bdAdd, scfFlags);
   else {
      bdAdd.opposite();
      bdAdd.minusAssignSureNN(bdMult, scfFlags);
      bdMult = bdAdd;
   };
LSetResult:
   fNegative = bdMult.isNegative();
   for (register int uIndex = 0; uIndex < biExponent.queryCellSize(); ++uIndex)
      biExponent[uIndex] = bdMult.querySBasicExponent()[uIndex];
   biExponent.normalize();
   for (register int uIndex = 0; uIndex < mprResult.queryCellSize(); ++uIndex)
      mprResult[uIndex] = bdMult.querySMantissa()[uIndex];
   mprResult.setTrueBitArray(2*bitSizeMantissa()+1);
   if (bdMult.querySBasicExponent().cbitArray(bitSizeExponent())) {
      if (bdMult.querySBasicExponent().cbitArray(bitSizeExponent()-1)) { // infty
         fResultPositiveExponent = true;
         fExponentHasCarry = true;
      }
      else { // positive
         biExponent.setTrueBitArray(bitSizeExponent()-1);
         fExponentHasCarry = false;
      };
   }
   else {
      if (bdMult.querySBasicExponent().cbitArray(bitSizeExponent()-1)) { // negative
         biExponent.setFalseBitArray(bitSizeExponent()-1);
         fExponentHasCarry = false;
      }
      else { // zero
         biExponent.setTrueBitArray(bitSizeExponent()-1);
         fResultPositiveExponent = false;
         fExponentHasCarry = true;
      };
   };
}

template <class TypeTraits>
TBuiltDouble<TypeTraits>&
TBuiltDouble<TypeTraits>::multAssignNN(const thisType& bdSource, const MultParameters& mpParams) {
   assert(isNormalized() && bdSource.isNormalized());
   StatusAndControlFlags& scfFlags = mpParams.readParams();
   typename TypeTraits::ExtendedMantissa::MultResult mrResult;
   typename Integer::TBigInt<typename TypeTraits::ExtendedMantissa::MultResult>&
      mprResult = (typename Integer::TBigInt<typename TypeTraits::ExtendedMantissa::MultResult>&) mrResult;

   typename TypeTraits::ExtendedMantissa emMantissa(biMantissa);
   typename TypeTraits::ExtendedMantissa emSourceMantissa(bdSource.biMantissa);
   emMantissa.mult(emSourceMantissa, mrResult);
   int uLogResult = 2*bitSizeMantissa()+1;
   bool fAddExponent = false;
   if (!mprResult.cbitArray(uLogResult))
      --uLogResult;
   else
      fAddExponent = true;
   assert(mprResult.cbitArray(uLogResult));

   bool fAdd = false;
   bool fRoundToEven = false;
   bool fApproximate = false;
   bool fExponentHasCarry = false;
   bool fResultPositiveExponent = !bdSource.hasNegativeExponent();
   if (fResultPositiveExponent)
      fExponentHasCarry = biExponent.plusAssign(bdSource.biExponent-getZeroExponent()).hasCarry();
   else {
      Exponent biSubExponent = getZeroExponent();
      biSubExponent -= bdSource.biExponent;
      fExponentHasCarry = biExponent.sub(biSubExponent).hasCarry();
      fResultPositiveExponent = !fExponentHasCarry && (biExponent >= getZeroExponent());
   };

   bool fAddExtension = false;
   if (mpParams.hasAdd()) {
      fAddExtension = mpParams.hasSameSign(fNegative);
      addExtension(mpParams.queryAddSource(), mprResult, scfFlags,
         mpParams.isPositiveAdditive(), uLogResult, fExponentHasCarry, fResultPositiveExponent,
         fAddExponent);
   };

   int uShift = uLogResult-bitSizeMantissa();
   if (((fApproximate = !mprResult.hasZero(uShift)) != false) || scfFlags.isApproximate()) {
      if (scfFlags.isNearestRound()) {
         fAdd = mprResult.cbitArray(uShift-1);
         if (fAdd && scfFlags.isRoundToEven()) {
            fRoundToEven = mprResult.hasZero(uShift-1) && !scfFlags.isApproximate();
            if (fRoundToEven)
               scfFlags.setEffectiveRoundToEven();
         };
      }
      else if (scfFlags.isHighestRound())
         fAdd = (fApproximate || !mpParams.hasAdd()) ? !fNegative : (!fNegative && fAddExtension);
      else if (scfFlags.isLowestRound())
         fAdd = (fApproximate || !mpParams.hasAdd()) ? fNegative : (fNegative && fAddExtension);
   };
   mprResult >>= uShift;
   assert(mprResult.cbitArray(bitSizeMantissa()));
   if (fAdd && ((fAdd = (!fRoundToEven || mprResult.cbitArray(0))) != false)) {
      mprResult.inc();
      if (!mprResult.cbitArray(bitSizeMantissa())) {
         mprResult >>= 1;
         assert(!fAddExponent);
         fAddExponent = true;
      };
   };
   if (fApproximate) {
      if (fNegative)
         scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
      else
         scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
   };
   for (register int uIndex = 0; uIndex <= biMantissa.lastCellIndex(); ++uIndex)
      biMantissa[uIndex] = mrResult[uIndex];
   biMantissa.normalizeLastCell();

   if (fResultPositiveExponent) {
      if (fExponentHasCarry || (fAddExponent && biExponent.inc().hasCarry())) {
         biExponent = getInftyExponent();
         biMantissa = 0U;
      }
      else if (biExponent == getInftyExponent()) {
         if (!biMantissa.isZero()) {
            biMantissa = 0U;
            if (!scfFlags.isApproximate())
               scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
         };
      }
      else
         return *this;

      scfFlags.setOverflow();
      if (scfFlags.upApproximateInfty())
         scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
      if (scfFlags.isInftyAvoided() && scfFlags.doesAvoidInfty(fNegative)) {
         biMantissa.neg();
         biExponent.dec();
         scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
      };
      return *this;
   };

   if (fAddExponent && biExponent.inc().hasCarry())
      fExponentHasCarry = false;
   if (fExponentHasCarry || biExponent.isZero()) {
      biExponent.neg().inc();
      Exponent biSubExponent = biExponent;
      biExponent = 0U;
      scfFlags.setUnderflow();
      if (biSubExponent < bitSizeMantissa()) // denormalized
         trightShift(biMantissa, biSubExponent.queryValue()+1, 1, scfFlags, fNegative, bitSizeMantissa());
      else {
         bool fAdd = false;
         if (scfFlags.isNearestRound()) {
            fAdd = (biSubExponent == bitSizeMantissa());
            if (fAdd && scfFlags.isRoundToEven())
               fAdd = !biMantissa.isZero() || scfFlags.isApproximate();
         }
         else if (scfFlags.isHighestRound())
            fAdd = !fNegative;
         else if (scfFlags.isLowestRound())
            fAdd = fNegative;
         biMantissa = fAdd ? 1U : 0U;
         if (fNegative)
            scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
         else
            scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
      };
   }
   else if ((biExponent == 1U) && biMantissa.isZero()
         && scfFlags.isApproximate(fAdd ? StatusAndControlFlags::Down : StatusAndControlFlags::Up))
      scfFlags.setUnderflow();
   return *this;
}

template <class TypeTraits>
TBuiltDouble<TypeTraits>&
TBuiltDouble<TypeTraits>::multAssignND(const thisType& bdSource, const MultParameters& mpParams) {
   assert(isNormalized() && bdSource.isDenormalized());
   StatusAndControlFlags& scfFlags = mpParams.readParams();
   typename TypeTraits::ExtendedMantissa::MultResult mrResult;
   typename Integer::TBigInt<typename TypeTraits::ExtendedMantissa::MultResult>&
      mprResult = (typename Integer::TBigInt<typename TypeTraits::ExtendedMantissa::MultResult>&) mrResult;

   typename TypeTraits::ExtendedMantissa emMantissa(biMantissa);
   typename TypeTraits::ExtendedMantissa emSourceMantissa(bdSource.biMantissa);
   emSourceMantissa.setFalseBitArray(bitSizeMantissa());
   emMantissa.mult(emSourceMantissa, mrResult);
   int uLogResult = mprResult.log_base_2()-1; // <= 2*bitSizeMantissa()
   assert(mprResult.cbitArray(uLogResult));

   bool fAdd = false;
   bool fRoundToEven = false;
   bool fApproximate = false;
   Exponent biSubExponent = getZeroExponent();
   int uAdd = 2*bitSizeMantissa()-uLogResult -1;
   if (uAdd > 0)
      biSubExponent.add(uAdd);
   else if (uAdd < 0)
      biSubExponent.sub(-uAdd);
   bool fResultPositiveExponent = false; 
   bool fExponentHasCarry = biExponent.sub(biSubExponent).hasCarry();

   bool fAddExtension = false;
   if (mpParams.hasAdd()) {
      bool fAddExponent = false;
      fAddExtension = mpParams.hasSameSign(fNegative);
      addExtension(mpParams.queryAddSource(), mprResult, scfFlags,
         mpParams.isPositiveAdditive(), uLogResult, fExponentHasCarry, fResultPositiveExponent,
         fAddExponent);
   };

   assert(mprResult.cbitArray(uLogResult));
   int uShift = uLogResult-bitSizeMantissa();
   if (((fApproximate = !mprResult.hasZero(uShift)) != false) || scfFlags.isApproximate()) {
      if (scfFlags.isNearestRound()) {
         fAdd = mprResult.cbitArray(uShift-1);
         if (fAdd && scfFlags.isRoundToEven()) {
            fRoundToEven = mprResult.hasZero(uShift-1) && !scfFlags.isApproximate();
            if (fRoundToEven)
               scfFlags.setEffectiveRoundToEven();
         };
      }
      else if (scfFlags.isHighestRound())
         fAdd = (fApproximate || !mpParams.hasAdd()) ? !fNegative : (!fNegative && fAddExtension);
      else if (scfFlags.isLowestRound())
         fAdd = (fApproximate || !mpParams.hasAdd()) ? fNegative : (fNegative && fAddExtension);
   };
   mprResult >>= uShift;
   assert(mprResult.cbitArray(bitSizeMantissa()));
   if (fAdd && ((fAdd = (!fRoundToEven || mprResult.cbitArray(0))) != false)) {
      mprResult.inc();
      if (!mprResult.cbitArray(bitSizeMantissa())) {
         mprResult >>= 1;
         biExponent.inc();
         if (biExponent.isZero())
            fExponentHasCarry = false;
      };
   };
   if (fApproximate) {
      if (fNegative)
         scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
      else
         scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
   };
   for (register int uIndex = 0; uIndex <= biMantissa.lastCellIndex(); ++uIndex)
      biMantissa[uIndex] = mrResult[uIndex];
   biMantissa.normalizeLastCell();

   if (fExponentHasCarry || biExponent.isZero()) {
      scfFlags.setUnderflow();
      biExponent.neg().inc();
      if (biExponent < bitSizeMantissa()) // denormalized
         trightShift(biMantissa, biExponent.queryValue()+1, 1, scfFlags, fNegative, bitSizeMantissa());
      else {
         bool fAdd = false;
         if (scfFlags.isNearestRound()) {
            fAdd = biExponent == bitSizeMantissa();
            if (fAdd && scfFlags.isRoundToEven())
               fAdd = !biMantissa.isZero() || scfFlags.isApproximate();
         }
         else if (scfFlags.isHighestRound())
            fAdd = !fNegative;
         else if (scfFlags.isLowestRound())
            fAdd = fNegative;
         if (fNegative)
            scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
         else
            scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
         biMantissa = fAdd ? 1U : 0U;
      };
      biExponent = 0U;
   }
   else if ((biExponent == 1U) && biMantissa.isZero()
         && scfFlags.isApproximate(fAdd ? StatusAndControlFlags::Down : StatusAndControlFlags::Up))
      scfFlags.setUnderflow();
   return *this;
}

template <class TypeTraits>
TBuiltDouble<TypeTraits>&
TBuiltDouble<TypeTraits>::multAssignDN(const thisType& bdSource, const MultParameters& mpParams) {
   assert(isDenormalized() && bdSource.isNormalized());
   StatusAndControlFlags& scfFlags = mpParams.readParams();
   typename TypeTraits::ExtendedMantissa::MultResult mrResult;
   typename Integer::TBigInt<typename TypeTraits::ExtendedMantissa::MultResult>&
      mprResult = (typename Integer::TBigInt<typename TypeTraits::ExtendedMantissa::MultResult>&) mrResult;

   typename TypeTraits::ExtendedMantissa emMantissa(biMantissa);
   typename TypeTraits::ExtendedMantissa emSourceMantissa(bdSource.biMantissa);
   emMantissa.setFalseBitArray(bitSizeMantissa());
   emMantissa.mult(emSourceMantissa, mrResult);
   int uLogResult = mprResult.log_base_2()-1; // <= 2*bitSizeMantissa()
   assert(mprResult.cbitArray(uLogResult));

   bool fAdd = false;
   bool fRoundToEven = false;
   bool fApproximate = false;
   Exponent biSubExponent = getZeroExponent();
   int uAdd = 2*bitSizeMantissa()-uLogResult -1;
   if (uAdd > 0)
      biSubExponent.add(uAdd);
   else if (uAdd < 0)
      biSubExponent.sub(-uAdd);
   bool fResultPositiveExponent = false; 
   biExponent = bdSource.biExponent;
   bool fExponentHasCarry = biExponent.sub(biSubExponent).hasCarry();

   bool fAddExtension = false;
   if (mpParams.hasAdd()) {
      bool fAddExponent = false;
      fAddExtension = mpParams.hasSameSign(fNegative);
      addExtension(mpParams.queryAddSource(), mprResult, scfFlags,
         mpParams.isPositiveAdditive(), uLogResult, fExponentHasCarry, fResultPositiveExponent,
         fAddExponent);
   };
   
   assert(mprResult.cbitArray(uLogResult));
   int uShift = uLogResult-bitSizeMantissa();
   if (((fApproximate = !mprResult.hasZero(uShift)) != false) || scfFlags.isApproximate()) {
      if (scfFlags.isNearestRound()) {
         fAdd = mprResult.cbitArray(uShift-1);
         if (fAdd && scfFlags.isRoundToEven()) {
            fRoundToEven = mprResult.hasZero(uShift-1) && !scfFlags.isApproximate();
            if (fRoundToEven)
               scfFlags.setEffectiveRoundToEven();
         };
      }
      else if (scfFlags.isHighestRound())
         fAdd = (fApproximate || !mpParams.hasAdd()) ? !fNegative : (!fNegative && fAddExtension);
      else if (scfFlags.isLowestRound())
         fAdd = (fApproximate || !mpParams.hasAdd()) ? fNegative : (fNegative && fAddExtension);
   };
   mprResult >>= uShift;
   assert(mprResult.cbitArray(bitSizeMantissa()));
   if (fAdd && ((fAdd = (!fRoundToEven || mprResult.cbitArray(0))) != false)) {
      mprResult.inc();
      if (!mprResult.cbitArray(bitSizeMantissa())) {
         mprResult >>= 1;
         ++uLogResult;
      };
   };
   if (fApproximate) {
      if (fNegative)
         scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
      else
         scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
   };
   for (register int uIndex = 0; uIndex <= biMantissa.lastCellIndex(); ++uIndex)
      biMantissa[uIndex] = mrResult[uIndex];
   biMantissa.normalizeLastCell();

   if (fExponentHasCarry || biExponent.isZero()) {
      scfFlags.setUnderflow();
      biExponent.neg().inc();
      if (biExponent < bitSizeMantissa()) // denormalized
         trightShift(biMantissa, biExponent.queryValue()+1, 1, scfFlags, fNegative, bitSizeMantissa());
      else {
         bool fAdd = false;
         if (scfFlags.isNearestRound()) {
            fAdd = biExponent == bitSizeMantissa();
            if (fAdd && scfFlags.isRoundToEven())
               fAdd = !biMantissa.isZero() || scfFlags.isApproximate();
         }
         else if (scfFlags.isHighestRound())
            fAdd = !fNegative;
         else if (scfFlags.isLowestRound())
            fAdd = fNegative;
         if (fNegative)
            scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
         else
            scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
         biMantissa = fAdd ? 1U : 0U;
      };
      biExponent = 0U;
   }
   else if ((biExponent == 1U) && biMantissa.isZero()
         && scfFlags.isApproximate(fAdd ? StatusAndControlFlags::Down : StatusAndControlFlags::Up))
      scfFlags.setUnderflow();
   return *this;
}

template <class TypeTraits>
TBuiltDouble<TypeTraits>&
TBuiltDouble<TypeTraits>::multAssignDD(const thisType& bdSource, const MultParameters& mpParams) {
   assert(isDenormalized() && bdSource.isDenormalized());
   StatusAndControlFlags& scfFlags = mpParams.readParams();
   scfFlags.setUnderflow();
   bool fAdd = false;
   if (scfFlags.isHighestRound())
      fAdd = !fNegative;
   else if (scfFlags.isLowestRound())
      fAdd = fNegative;
   if (fNegative)
      scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
   else
      scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
   if (mpParams.hasAdd() && !mpParams.queryAddSource().isZero()) {
      scfFlags.clearFlowException();
      const thisType& bdAddSource = mpParams.queryAddSource();
      if (bdAddSource.biExponent == getInftyExponent()) {
         scfFlags.clearApproximate();
         fNegative = mpParams.isPositiveAdditive() ? bdAddSource.fNegative
            : !bdAddSource.fNegative;
         biMantissa = bdAddSource.biMantissa;
         if (!biMantissa.isZero()) {
            fNegative = (scfFlags.keepNaNSign() || mpParams.isPositiveAdditive())
               ? bdAddSource.fNegative : !bdAddSource.fNegative;
            if (!biMantissa.cbitArray(bitSizeMantissa()-1)) {
               scfFlags.setSNaNOperand();
               biMantissa.setTrueBitArray(bitSizeMantissa()-1);
            };
         };
         biExponent = getInftyExponent();
         return *this;
      };
      biMantissa = bdAddSource.queryMantissa();
      biExponent = bdAddSource.queryBasicExponent();
      register bool fSub =  mpParams.isPositiveAdditive()
         ? (fNegative != bdAddSource.fNegative) : (fNegative == bdAddSource.fNegative);
      if (fSub) {
         fNegative = mpParams.isPositiveAdditive() ? bdAddSource.fNegative : !bdAddSource.fNegative;
         if (scfFlags.isHighestRound() || scfFlags.isLowestRound())
            fSub = fAdd;
         else if (scfFlags.isNearestRound())
            fSub = false;
         fAdd = false;
      };
      if (fAdd && biMantissa.inc().hasCarry()) {
         biExponent.inc();
         if (biExponent == getInftyExponent()) {
            if (scfFlags.isInftyAvoided() && scfFlags.doesAvoidInfty(fNegative)) {
               biMantissa.neg();
               biExponent.dec();
               scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
            }
            else if (scfFlags.upApproximateInfty())
               scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
         };
      }
      else if (fSub) {
         if (biMantissa.dec().hasCarry())
            biExponent.dec();
         scfFlags.clearApproximate();
         scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
      };
      if (isDenormalized())
         scfFlags.setUnderflow();
      else if (isInfty())
         scfFlags.setOverflow();
   }
   else
      biMantissa = fAdd ? 1U : 0U;
   return *this;
}

template <class TypeTraits>
TBuiltDouble<TypeTraits>&
TBuiltDouble<TypeTraits>::multAssign(const thisType& bdSource, const MultParameters& mpParams) {
   StatusAndControlFlags& scfFlags = mpParams.readParams();
   bool fOldNegative = fNegative;
   fNegative = fNegative ? !bdSource.fNegative : bdSource.fNegative;
   if (mpParams.hasAdd() && mpParams.isNegativeMult())
      fNegative = !fNegative;
   if (!biExponent.isZero()) {
      if (biExponent != getInftyExponent()) {
         if (!bdSource.biExponent.isZero()) {
            if (bdSource.biExponent != getInftyExponent()) {
               if (mpParams.hasAdd() && (mpParams.queryAddSource().biExponent == getInftyExponent())) {
                  biExponent = mpParams.queryAddSource().biExponent;
                  fNegative = mpParams.queryAddSource().fNegative;
                  if (mpParams.isNegativeAdditive())
                     fNegative = !fNegative;
                  biMantissa = mpParams.queryAddSource().biMantissa;
                  if (!biMantissa.isZero()) {
                     if (scfFlags.keepNaNSign())
                        fNegative = mpParams.queryAddSource().fNegative;
                     if (!biMantissa.cbitArray(bitSizeMantissa()-1)) {
                        scfFlags.setSNaNOperand();
                        biMantissa.setTrueBitArray(bitSizeMantissa()-1);
                     };
                  };
                  return *this;
               };
               return multAssignNN(bdSource, mpParams);
            };
            // bdSource.biExponent == getInftyExponent()
            biMantissa = bdSource.biMantissa;
            biExponent = getInftyExponent();
            if (!biMantissa.isZero()) {
               if (!biMantissa.cbitArray(bitSizeMantissa()-1)) {
                  scfFlags.setSNaNOperand();
                  biMantissa.setTrueBitArray(bitSizeMantissa()-1);
               };
            };
            if (scfFlags.keepNaNSign())
               fNegative = bdSource.fNegative;
            if (mpParams.hasAdd()) {
               if (scfFlags.chooseNaNAddBeforeMult() && mpParams.queryAddSource().isNaN()) {
                  biExponent = mpParams.queryAddSource().biExponent;
                  fNegative = mpParams.isPositiveAdditive()
                     ? mpParams.queryAddSource().fNegative : !mpParams.queryAddSource().fNegative;
                  biMantissa = mpParams.queryAddSource().biMantissa;
                  if (!biMantissa.isZero()) {
                     if (scfFlags.keepNaNSign())
                        fNegative = mpParams.queryAddSource().fNegative;
                     if (!biMantissa.cbitArray(bitSizeMantissa()-1)) {
                        scfFlags.setSNaNOperand();
                        biMantissa.setTrueBitArray(bitSizeMantissa()-1);
                     };
                  };
                  return *this;
               };
               if (mpParams.isPositiveAdditive())
                  plusAssign(mpParams.queryAddSource(), scfFlags);
               else
                  minusAssign(mpParams.queryAddSource(), scfFlags);
            };
            return *this;
         };
         
         // bdSource.biExponent == 0
         if (mpParams.hasAdd() && (mpParams.queryAddSource().biExponent == getInftyExponent())) {
            biExponent = mpParams.queryAddSource().biExponent;
            fNegative = mpParams.isPositiveAdditive()
               ? mpParams.queryAddSource().fNegative : !mpParams.queryAddSource().fNegative;
            biMantissa = mpParams.queryAddSource().biMantissa;
            if (!biMantissa.isZero()) {
               if (scfFlags.keepNaNSign())
                  fNegative = mpParams.queryAddSource().fNegative;
               if (!biMantissa.cbitArray(bitSizeMantissa()-1)) {
                  scfFlags.setSNaNOperand();
                  biMantissa.setTrueBitArray(bitSizeMantissa()-1);
               };
            };
            return *this;
         };
         if (bdSource.biMantissa.isZero()) {
            biMantissa = 0U;
            biExponent = 0U;
            if (mpParams.hasAdd()) {
               if (!mpParams.queryAddSource().isZero()) {
                  biExponent = mpParams.queryAddSource().biExponent;
                  fNegative = mpParams.isPositiveAdditive()
                     ? mpParams.queryAddSource().fNegative : !mpParams.queryAddSource().fNegative;
                  biMantissa = mpParams.queryAddSource().biMantissa;
                  if (mpParams.queryAddSource().isNaN()) {
                     if (scfFlags.keepNaNSign())
                        fNegative = mpParams.queryAddSource().fNegative;
                     if (!biMantissa.cbitArray(bitSizeMantissa()-1)) {
                        scfFlags.setSNaNOperand();
                        biMantissa.setTrueBitArray(bitSizeMantissa()-1);
                     };
                  };
               }
               else if ((mpParams.isPositiveAdditive()
                        ? (fNegative != mpParams.queryAddSource().fNegative)
                        : (fNegative == mpParams.queryAddSource().fNegative))
                     && scfFlags.isPositiveZeroMAdd())
                  fNegative = !scfFlags.isLowestRound() ? mpParams.isNegativeMult() : !mpParams.isNegativeMult();
            };
            return *this;
         };
         return multAssignND(bdSource, mpParams);
      };

      // biExponent == getInftyExponent()
      if (!biMantissa.isZero()) {
         if (!biMantissa.cbitArray(bitSizeMantissa()-1)) {
            scfFlags.setSNaNOperand();
            biMantissa.setTrueBitArray(bitSizeMantissa()-1);
         }
         else if (bdSource.isSNaN())
            scfFlags.setSNaNOperand();
         if (scfFlags.keepNaNSign())
            fNegative = fOldNegative;
      }
      else if (bdSource.isNaN()) {
         biMantissa = bdSource.biMantissa;
         if (scfFlags.keepNaNSign())
            fNegative = bdSource.fNegative;
         if (!biMantissa.cbitArray(bitSizeMantissa()-1)) {
            scfFlags.setSNaNOperand();
            biMantissa.setTrueBitArray(bitSizeMantissa()-1);
         };
      }
      else if (bdSource.biMantissa.isZero() && bdSource.biExponent.isZero()) {
         scfFlags.setInftyMultZero();
         biMantissa = 0;
         biMantissa.neg();
         biMantissa.setFalseBitArray(bitSizeMantissa()-1);
         if (scfFlags.keepNaNSign())
            fNegative = fOldNegative;
      };
      if (mpParams.hasAdd()) {
         if (mpParams.isPositiveAdditive())
            plusAssign(mpParams.queryAddSource(), scfFlags);
         else
            minusAssign(mpParams.queryAddSource(), scfFlags);
      };
      return *this;
   };
   
   // biExponent == 0
   if (bdSource.biExponent.isZero()) {
      if (!biMantissa.isZero() && !bdSource.biMantissa.isZero())
         return multAssignDD(bdSource, mpParams);
      biMantissa = 0U;
      if (mpParams.hasAdd()) {
         if (!mpParams.queryAddSource().isZero()) {
            biExponent = mpParams.queryAddSource().biExponent;
            fNegative = mpParams.isPositiveAdditive()
               ? mpParams.queryAddSource().fNegative : !mpParams.queryAddSource().fNegative;
            biMantissa = mpParams.queryAddSource().biMantissa;
            if (mpParams.queryAddSource().isNaN()) {
               if (scfFlags.keepNaNSign())
                  fNegative = mpParams.queryAddSource().fNegative;
               if (!biMantissa.cbitArray(bitSizeMantissa()-1)) {
                  scfFlags.setSNaNOperand();
                  biMantissa.setTrueBitArray(bitSizeMantissa()-1);
               };
            };
         }
         else if ((mpParams.isPositiveAdditive()
                  ? (fNegative != mpParams.queryAddSource().fNegative) 
                  : (fNegative == mpParams.queryAddSource().fNegative))
               && scfFlags.isPositiveZeroMAdd())
            fNegative = !scfFlags.isLowestRound() ? mpParams.isNegativeMult() : !mpParams.isNegativeMult();
      };
      return *this;
   };

   // biExponent == 0 && bdSource.biExponent != 0
   if (bdSource.biExponent == getInftyExponent()) {
      biExponent = getInftyExponent();
      if (!bdSource.biMantissa.isZero() || biMantissa.isZero()) {
         if (bdSource.biMantissa.isZero()) {
            scfFlags.setInftyMultZero();
            biMantissa = 0U;
            biMantissa.neg();
            biMantissa.setFalseBitArray(bitSizeMantissa()-1);
         }
         else {
            biMantissa = bdSource.biMantissa;
            if (!biMantissa.cbitArray(bitSizeMantissa()-1)) {
               scfFlags.setSNaNOperand();
               biMantissa.setTrueBitArray(bitSizeMantissa()-1);
            };
            if (scfFlags.keepNaNSign())
               fNegative = bdSource.fNegative;
         };
         if (mpParams.hasAdd()) {
            if (scfFlags.chooseNaNAddBeforeMult() && mpParams.queryAddSource().isNaN()) {
               biExponent = mpParams.queryAddSource().biExponent;
               fNegative = (scfFlags.keepNaNSign() || mpParams.isPositiveAdditive())
                  ? mpParams.queryAddSource().fNegative : !mpParams.queryAddSource().fNegative;
               biMantissa = mpParams.queryAddSource().biMantissa;
               if (!biMantissa.cbitArray(bitSizeMantissa()-1)) {
                  scfFlags.setSNaNOperand();
                  biMantissa.setTrueBitArray(bitSizeMantissa()-1);
               };
               return *this;
            };
            if (mpParams.isPositiveAdditive())
               plusAssign(mpParams.queryAddSource(), scfFlags);
            else
               minusAssign(mpParams.queryAddSource(), scfFlags);
         };
         return *this;
      };
      
      // bdSource.biMantissa == 0 && bdSource.biExponent == getInftyExponent()
      // && biExponent == 0 && biMantissa != 0
      biMantissa = 0U;
      if (mpParams.hasAdd()) {
         if (mpParams.isPositiveAdditive())
            plusAssign(mpParams.queryAddSource(), scfFlags);
         else
            minusAssign(mpParams.queryAddSource(), scfFlags);
      };
      return *this;
   };
   
   // biExponent == 0 && bdSource.biExponent != 0 && bdSource.biExponent != getInftyExponent()
   if (biMantissa.isZero()) {
      if (mpParams.hasAdd()) {
         if (!mpParams.queryAddSource().isZero()) {
            biExponent = mpParams.queryAddSource().biExponent;
            fNegative = mpParams.isPositiveAdditive()
               ? mpParams.queryAddSource().fNegative : !mpParams.queryAddSource().fNegative;
            biMantissa = mpParams.queryAddSource().biMantissa;
            if (mpParams.queryAddSource().isNaN()) {
               if (scfFlags.keepNaNSign())
                  fNegative = mpParams.queryAddSource().fNegative;
               if (!biMantissa.cbitArray(bitSizeMantissa()-1)) {
                  scfFlags.setSNaNOperand();
                  biMantissa.setTrueBitArray(bitSizeMantissa()-1);
               };
            };
         }
         else if ((mpParams.isPositiveAdditive()
                  ? (fNegative != mpParams.queryAddSource().fNegative)
                  : (fNegative == mpParams.queryAddSource().fNegative))
               && scfFlags.isPositiveZeroMAdd())
            fNegative = !scfFlags.isLowestRound() ? mpParams.isNegativeMult() : !mpParams.isNegativeMult();
      };
      return *this;
   };
   if (mpParams.hasAdd() && (mpParams.queryAddSource().biExponent == getInftyExponent())) {
      biExponent = mpParams.queryAddSource().biExponent;
      fNegative = mpParams.isPositiveAdditive()
         ? mpParams.queryAddSource().fNegative : !mpParams.queryAddSource().fNegative;
      biMantissa = mpParams.queryAddSource().biMantissa;
      if (!biMantissa.isZero()) {
         if (scfFlags.keepNaNSign())
            fNegative = mpParams.queryAddSource().fNegative;
         if (!biMantissa.cbitArray(bitSizeMantissa()-1)) {
            scfFlags.setSNaNOperand();
            biMantissa.setTrueBitArray(bitSizeMantissa()-1);
         };
      };
      return *this;
   };
   return multAssignDN(bdSource, mpParams);
}

template <class TypeTraits>
TBuiltDouble<TypeTraits>&
TBuiltDouble<TypeTraits>::divAssign(unsigned int uValue, StatusAndControlFlags& scfFlags) {
   assert((uValue > 0) && (uValue < (1U << (sizeof(unsigned int)*4))));
   if (isNormalized()) {
      typename TypeTraits::ExtendedMantissa emMantissa(biMantissa);
      typename TypeTraits::ExtendedMantissa::AtomicDivisionResult adrResult = emMantissa.divAssign(uValue);
      unsigned int uShift = (bitSizeMantissa()+1 - emMantissa.log_base_2());
      assert((log_base_2(uValue) == (int) uShift) || (log_base_2(uValue) == (int) (uShift-1)));
      unsigned int uLeftQuotient = 0U;
      if ((uShift == (4*sizeof(unsigned int)+1))
            && ((adrResult.remainder() & (1U << 4*sizeof(unsigned int))) != 0U)) {
         adrResult.remainder() <<= 4*sizeof(unsigned int);
         uLeftQuotient = adrResult.remainder() / uValue;
         adrResult.remainder() %= uValue;
         adrResult.remainder() <<= 1;
         uLeftQuotient <<= 1;
         if (adrResult.remainder() >= uValue) {
            ++uLeftQuotient;
            adrResult.remainder() -= uValue;
         };
         assert(uLeftQuotient < (1U << 4*sizeof(unsigned int)));
      }
      else {
         adrResult.remainder() <<= uShift;
         uLeftQuotient = adrResult.remainder() / uValue;
         adrResult.remainder() %= uValue;
      };
      emMantissa <<= uShift;
      emMantissa[0] |= uLeftQuotient;
      if (biExponent.isAtomic() && (biExponent.queryValue() <= uShift)) {
         int uLeftshift = uShift-biExponent.queryValue() + 1;
         bool fAdd = false;
         bool fRoundToEven = false;
         bool fApproximate = false;
         if (!emMantissa.hasZero(uLeftshift) || (adrResult.remainder() != 0)) {
            if (scfFlags.isNearestRound()) {
               fAdd = emMantissa.cbitArray(uLeftshift-1);
               if (fAdd && scfFlags.isRoundToEven()) {
                  fRoundToEven = (adrResult.remainder() == 0U) && emMantissa.hasZero(uLeftshift-1);
                  if (fRoundToEven)
                     scfFlags.setEffectiveRoundToEven();
               };
            }
            else if (scfFlags.isHighestRound())
               fAdd = !fNegative;
            else if (scfFlags.isLowestRound())
               fAdd = fNegative;
            fApproximate = true;
         };
         emMantissa >>= uLeftshift;
         biMantissa = emMantissa;
         biExponent = 0;
         if (fAdd && ((fAdd = (!fRoundToEven || biMantissa.cbitArray(0))) != false)
               && biMantissa.inc().hasCarry())
            biExponent = 1U;
         if (fApproximate) {
            if (fNegative)
               scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
            else
               scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
         };
      }
      else {
         bool fAdd = false;
         biExponent.minusAssign(uShift);
         if (adrResult.remainder() != 0) {
            if (scfFlags.isNearestRound()) {
               fAdd = (2*adrResult.remainder() >= uValue);
               if (fAdd && scfFlags.isRoundToEven())
                  fAdd = (2*adrResult.remainder() != uValue) || biMantissa.cbitArray(0);
            }
            else if (scfFlags.isHighestRound())
               fAdd = !fNegative;
            else if (scfFlags.isLowestRound())
               fAdd = fNegative;
            if (fNegative)
               scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
            else
               scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
         };
         biMantissa = emMantissa;
         if (fAdd && biMantissa.inc().hasCarry()) {
            biMantissa >>= 1;
            biExponent.inc();
         };
      };
   }
   else if (biExponent.isZero()) {
      typename TypeTraits::ExtendedMantissa::AtomicDivisionResult adrResult = biMantissa.divAssign(uValue);
      if (adrResult.remainder() != 0) {
         bool fAdd = false;
         if (scfFlags.isNearestRound()) {
            fAdd = (2*adrResult.remainder() >= uValue);
            if (fAdd && scfFlags.isRoundToEven())
               fAdd = (2*adrResult.remainder() != uValue) || biMantissa.cbitArray(0);
         }
         else if (scfFlags.isHighestRound())
            fAdd = !fNegative;
         else if (scfFlags.isLowestRound())
            fAdd = fNegative;
         if (fAdd)
            biMantissa.inc();
         if (fNegative)
            scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
         else
            scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
      };
   };
   // else { // biExponent == getInftyExponent(); };
   return *this;
}

template <class TypeTraits>
TBuiltDouble<TypeTraits>&
TBuiltDouble<TypeTraits>::divAssignNN(const thisType& bdSource, StatusAndControlFlags& scfFlags) {
   assert(isNormalized() && bdSource.isNormalized());
   typename Mantissa::NormalizedDivisionResult drResult;
   biMantissa.divNormalized(bdSource.biMantissa, drResult);
   assert((drResult.comma() == 0) || (drResult.comma() == -1));
   biMantissa = (const Mantissa&) drResult.quotient();
   bool fAdd = false;
   bool fRoundToEven = false;
   bool fApproximate = false;
   if (!((const Integer::TBigInt<typename Mantissa::RemainderResult>&) drResult.remainder()).isZero()) {
      if (scfFlags.isNearestRound()) {
         fAdd = ((const typename TypeTraits::ExtendedMantissa&) drResult.remainder()).cbitArray(bitSizeMantissa());
         if (!fAdd && ((const typename TypeTraits::ExtendedMantissa&) drResult.remainder()).cbitArray(bitSizeMantissa()-1)) {
            Mantissa biRemainder;
            biRemainder.assertSize(bdSource.biMantissa.querySize());
            for (register int uIndex = 0; uIndex <= bdSource.biMantissa.lastCellIndex(); ++uIndex)
               biRemainder[uIndex] = drResult.remainder()[uIndex];
            fAdd = (biRemainder <<= 1) >= bdSource.biMantissa;
            if (fAdd && scfFlags.isNearestRound()) {
               fRoundToEven = (biRemainder == bdSource.biMantissa);
               if (fRoundToEven)
                  scfFlags.setEffectiveRoundToEven();
            };
         };
      }
      else if (scfFlags.isHighestRound())
         fAdd = !fNegative;
      else if (scfFlags.isLowestRound())
         fAdd = fNegative;
      fApproximate = true;
   };
   if (bdSource.biExponent >= getZeroExponent()) {
      Exponent biSubExponent = bdSource.biExponent;
      biSubExponent -= getZeroExponent();
      biSubExponent.add(-drResult.comma()); // 0 or -1
      if (biExponent <= biSubExponent) {
         biSubExponent -= biExponent;
         biExponent = 0U;
         scfFlags.setUnderflow();
         if (biSubExponent < bitSizeMantissa()) {
            int uShift = biSubExponent.queryValue()+1;
            StatusAndControlFlags rpNewParams(scfFlags);
            if (fApproximate)
               rpNewParams.clearRoundToEven();
            trightShift(biMantissa, uShift, 1, rpNewParams, fNegative, bitSizeMantissa());
            if (!rpNewParams.isApproximate() && fAdd
                  && ((fAdd = !scfFlags.isNearestRound()) != false)) {
               if (biMantissa.inc().hasCarry()) {
                  biExponent.inc();
                  scfFlags.clearUnderflow();
               };
               scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
               return *this;
            }
            else if (rpNewParams.isApproximate()) {
               scfFlags = rpNewParams;
               return *this;
            };
         }
         else {
            fAdd = false;
            if (scfFlags.isNearestRound()) {
               fAdd = (biSubExponent == bitSizeMantissa());
               if (fAdd && scfFlags.isRoundToEven())
                  fAdd = !biMantissa.isZero() && !((const Integer::TBigInt<typename Mantissa::RemainderResult>&)
                     drResult.remainder()).isZero();
            }
            else if (scfFlags.isHighestRound())
               fAdd = !fNegative;
            else if (scfFlags.isLowestRound())
               fAdd = fNegative;
            if (fNegative)
               scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
            else
               scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
            biMantissa = fAdd ? 1U : 0U;
            return *this;
         };
      }
      else
         biExponent -= biSubExponent;
      if (fAdd && ((fAdd = (!fRoundToEven || biMantissa.cbitArray(0))) != false)
            && biMantissa.inc().hasCarry())
         biExponent.inc();
   }
   else { // bdSource.biExponent < getZeroExponent()
      Exponent biSubExponent = getZeroExponent();
      biSubExponent -= bdSource.biExponent;
      biSubExponent.sub(-drResult.comma()); // 0 or -1
      if (biExponent.add(biSubExponent).hasCarry()
            || (biExponent == getInftyExponent())) {
         biExponent = getInftyExponent();
         biMantissa = 0U;
         scfFlags.setOverflow();
         scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
         if (scfFlags.isInftyAvoided() && scfFlags.doesAvoidInfty(fNegative)) {
            fApproximate = false;
            biMantissa.neg();
            biExponent.dec();
            scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
         };
      }
      else if (fAdd && ((fAdd = (!fRoundToEven || biMantissa.cbitArray(0))) != false)
            && biMantissa.inc().hasCarry()) {
         biExponent.inc();
         if (biExponent == getInftyExponent()) {
            scfFlags.setOverflow();
            if (scfFlags.isInftyAvoided() && scfFlags.doesAvoidInfty(fNegative)) {
               fApproximate = false;
               biMantissa.neg();
               biExponent.dec();
               scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
            }
            else if (scfFlags.upApproximateInfty())
               scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
         };
      };
   };
   if (fApproximate) {
      if (fNegative)
         scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
      else
         scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
   };
   return *this;
}

template <class TypeTraits>
TBuiltDouble<TypeTraits>&
TBuiltDouble<TypeTraits>::divAssignND(const thisType& bdSource, StatusAndControlFlags& scfFlags) {
   assert(isNormalized() && bdSource.isDenormalized());
   typename TypeTraits::ExtendedMantissa::DivisionResult drResult;
   typename TypeTraits::ExtendedMantissa emMantissa(biMantissa);
   typename TypeTraits::ExtendedMantissa emSourceMantissa(bdSource.biMantissa);
   emSourceMantissa.setFalseBitArray(bitSizeMantissa());

   emMantissa.div(emSourceMantissa, drResult);
   assert(drResult.comma() >= 0);
   bool fAdd = false;
   bool fRoundToEven = false;
   bool fApproximate = false;
   if (((const typename TypeTraits::ExtendedMantissa&) drResult.quotient()).cbitArray(0)
          || !((const typename TypeTraits::ExtendedMantissa&) drResult.remainder()).isZero()) {
      if (scfFlags.isNearestRound()) {
         fAdd = ((const typename TypeTraits::ExtendedMantissa&) drResult.quotient()).cbitArray(0);
         if (fAdd && scfFlags.isRoundToEven()) {
            fRoundToEven = ((const typename TypeTraits::ExtendedMantissa&) drResult.remainder()).isZero();
            if (fRoundToEven)
               scfFlags.setEffectiveRoundToEven();
         };
      }
      else if (scfFlags.isHighestRound())
         fAdd = !fNegative;
      else if (scfFlags.isLowestRound())
         fAdd = fNegative;
      fApproximate = true;
   };
   ((typename TypeTraits::ExtendedMantissa&) drResult.quotient()) >>= 1;
   for (register int uIndex = 0; uIndex <= biMantissa.lastCellIndex(); ++uIndex)
      biMantissa[uIndex] = drResult.quotient()[uIndex];
   biMantissa.normalizeLastCell();

   if (biExponent.add(getZeroExponent()).hasCarry() || biExponent.add(drResult.comma()-1).hasCarry()
         || (biExponent == getInftyExponent())) {
      biExponent = getInftyExponent();
      biMantissa = 0;
      scfFlags.setOverflow();
      scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
      if (scfFlags.isInftyAvoided() && scfFlags.doesAvoidInfty(fNegative)) {
         fApproximate = false;
         biMantissa.neg();
         biExponent.dec();
         scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
      };
   }
   else if (fAdd && ((fAdd = (!fRoundToEven || biMantissa.cbitArray(0))) != false)
         && biMantissa.inc().hasCarry()) {
      biExponent.inc();
      if (biExponent == getInftyExponent()) {
         scfFlags.setOverflow();
         if (scfFlags.isInftyAvoided() && scfFlags.doesAvoidInfty(fNegative)) {
            fApproximate = false;
            biMantissa.neg();
            biExponent.dec();
            scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
         }
         else if (scfFlags.upApproximateInfty())
            scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
      };
   };
   if (fApproximate) {
      if (fNegative)
         scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
      else
         scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
   };
   return *this;
}

template <class TypeTraits>
TBuiltDouble<TypeTraits>&
TBuiltDouble<TypeTraits>::divAssignDN(const thisType& bdSource, StatusAndControlFlags& scfFlags) {
   assert(isDenormalized() && bdSource.isNormalized());
   typename TypeTraits::ExtendedMantissa::DivisionResult drResult;
   typename TypeTraits::ExtendedMantissa emMantissa(biMantissa);
   emMantissa.setFalseBitArray(bitSizeMantissa());
   typename TypeTraits::ExtendedMantissa emSourceMantissa(bdSource.biMantissa);
   emMantissa.div(emSourceMantissa, drResult);
   assert(drResult.comma() < 0);

   bool fAdd = false;
   bool fRoundToEven = false;
   bool fApproximate = false;
   if (((const typename TypeTraits::ExtendedMantissa&) drResult.quotient()).cbitArray(0)
          || !((const typename TypeTraits::ExtendedMantissa&) drResult.remainder()).isZero()) {
      if (scfFlags.isNearestRound()) {
         fAdd = ((const typename TypeTraits::ExtendedMantissa&) drResult.quotient()).cbitArray(0);
         if (fAdd && scfFlags.isRoundToEven()) {
            fRoundToEven = ((const typename TypeTraits::ExtendedMantissa&) drResult.remainder()).isZero();
            if (fRoundToEven)
               scfFlags.setEffectiveRoundToEven();
         };
      }
      else if (scfFlags.isHighestRound())
         fAdd = !fNegative;
      else if (scfFlags.isLowestRound())
         fAdd = fNegative;
      fApproximate = true;
   };
   ((typename TypeTraits::ExtendedMantissa&) drResult.quotient()) >>= 1;
   for (register int uIndex = 0; uIndex <= biMantissa.lastCellIndex(); ++uIndex)
      biMantissa[uIndex] = drResult.quotient()[uIndex];
   biMantissa.normalizeLastCell();

   if (bdSource.biExponent >= getZeroExponent()) {
      scfFlags.setUnderflow();
      Exponent biSubExponent = bdSource.biExponent;
      biSubExponent -= getZeroExponent();
      biSubExponent.add(-drResult.comma());
      if (biSubExponent <= bitSizeMantissa()) {
         StatusAndControlFlags rpNewParams(scfFlags);
         if (fApproximate)
            rpNewParams.clearRoundToEven();
         trightShift(biMantissa, biSubExponent.queryValue(), 1, rpNewParams, fNegative, bitSizeMantissa());
         if (!rpNewParams.isApproximate() && fAdd
               && ((fAdd = !scfFlags.isNearestRound()) != false)) {
            if (biMantissa.inc().hasCarry()) {
               biExponent.inc();
               scfFlags.clearUnderflow();
            };
            scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
            return *this;
         }
         else if (rpNewParams.isApproximate()) {
            scfFlags = rpNewParams;
            return *this;
         };
      }
      else {
         fAdd = false;
         if (scfFlags.isNearestRound()) {
            fAdd = (biSubExponent == bitSizeMantissa()+1);
            if (fAdd && scfFlags.isRoundToEven())
               fAdd = !biMantissa.isZero() || !((const typename TypeTraits::ExtendedMantissa&) drResult.remainder()).isZero();
         }
         else if (scfFlags.isHighestRound())
            fAdd = !fNegative;
         else if (scfFlags.isLowestRound())
            fAdd = fNegative;
         fApproximate = true;
         biMantissa = fAdd ? 1U : 0U;
      };
   }
   else { // bdSource.biExponent < getZeroExponent()
      Exponent biSubExponent = getZeroExponent();
      biSubExponent -= bdSource.biExponent;
      if (biSubExponent >= -drResult.comma()) {
         biSubExponent.sub(-drResult.comma()-1);
         biExponent = biSubExponent;
      }
      else {
         scfFlags.setUnderflow();
         StatusAndControlFlags rpNewParams(scfFlags);
         if (fApproximate)
            rpNewParams.clearRoundToEven();
         trightShift(biMantissa, -drResult.comma() - biSubExponent.queryValue(), 1, rpNewParams,
            fNegative, bitSizeMantissa());
         if (!rpNewParams.isApproximate() && fAdd
               && ((fAdd = !scfFlags.isNearestRound()) != false)) {
            biMantissa.inc();
            scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
         }
         else if (rpNewParams.isApproximate())
            scfFlags = rpNewParams;
         else if (fApproximate)
            scfFlags.setApproximate(fNegative ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
         return *this;
      };
         
      if (fAdd && ((fAdd = (!fRoundToEven || biMantissa.cbitArray(0))) != false)
            && biMantissa.inc().hasCarry()) {
         scfFlags.clearUnderflow();
         ++biExponent;
      };
   };
   if (fApproximate) {
      if (fNegative)
         scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
      else
         scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
   };
   return *this;
}

template <class TypeTraits>
TBuiltDouble<TypeTraits>&
TBuiltDouble<TypeTraits>::divAssignDD(const thisType& bdSource, StatusAndControlFlags& scfFlags) {
   assert(isDenormalized() && bdSource.isDenormalized());
   typename Mantissa::DivisionResult drResult;
   biMantissa.div(bdSource.biMantissa, drResult);

   for (register int uIndex = 0; uIndex <= biMantissa.lastCellIndex(); ++uIndex)
      biMantissa[uIndex] = drResult.quotient()[uIndex];
   biExponent = getZeroExponent();
   if (drResult.comma() > 0)
      biExponent.add(drResult.comma());
   else if (drResult.comma() < 0)
      biExponent.sub(-drResult.comma());
   
   bool fAdd = false;
   if (!((const Mantissa&) drResult.remainder()).isZero()) {
      if (scfFlags.isNearestRound()) {
         fAdd = ((const Mantissa&) drResult.remainder()).cbitArray(bitSizeMantissa()-1);
         ((Mantissa&) drResult.remainder()) <<= 1;
         if (!fAdd)
            fAdd = (((const Mantissa&) drResult.remainder()) > bdSource.biMantissa);
         if (!fAdd && (!scfFlags.isRoundToEven() || biMantissa.cbitArray(0)))
            fAdd = (((const Mantissa&) drResult.remainder()) == bdSource.biMantissa);
      }
      else if (scfFlags.isHighestRound())
         fAdd = !fNegative;
      else if (scfFlags.isLowestRound())
         fAdd = fNegative;
      if (fNegative)
         scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
      else
         scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
   };

   if (fAdd && biMantissa.inc().hasCarry())
      ++biExponent;
   return *this;
}

template <class TypeTraits>
TBuiltDouble<TypeTraits>&
TBuiltDouble<TypeTraits>::divAssign(const thisType& bdSource, StatusAndControlFlags& scfFlags) {
   bool fOldNegative = fNegative;
   fNegative = fNegative ? !bdSource.fNegative : bdSource.fNegative;
   if (!biExponent.isZero()) {
      if (biExponent != getInftyExponent()) {
         if (!bdSource.biExponent.isZero()) {
            if (bdSource.biExponent != getInftyExponent())
               return divAssignNN(bdSource, scfFlags);
            // bdSource.biExponent == getInftyExponent()
            if (!bdSource.biMantissa.isZero()) {
               biMantissa = bdSource.biMantissa;
               if (!biMantissa.cbitArray(bitSizeMantissa()-1)) {
                  scfFlags.setSNaNOperand();
                  biMantissa.setTrueBitArray(bitSizeMantissa()-1);
               };
               biExponent = getInftyExponent();
               if (scfFlags.keepNaNSign())
                  fNegative = bdSource.fNegative;
            }
            else {
               biMantissa = 0U;
               biExponent = 0U;
            };
            return *this;
         };
         
         // bdSource.biExponent == 0
         if (bdSource.biMantissa.isZero()) {
            scfFlags.setDivisionByZero();
            biMantissa = 0U;
            biExponent = getInftyExponent();
            return *this;
         };
         return divAssignND(bdSource, scfFlags);
      };

      // biExponent == getInftyExponent()
      if (!biMantissa.isZero()) {
         if (!biMantissa.cbitArray(bitSizeMantissa()-1)) {
            scfFlags.setSNaNOperand();
            biMantissa.setTrueBitArray(bitSizeMantissa()-1);
         }
         else if (bdSource.isSNaN())
            scfFlags.setSNaNOperand();
         if (scfFlags.keepNaNSign())
            fNegative = fOldNegative;
      }
      else if (bdSource.biExponent == getInftyExponent()) {
         if (bdSource.biMantissa.isZero()) { // produces a qNaN
            scfFlags.setInftyOnInfty();
            biMantissa.setTrueBitArray(bitSizeMantissa()-1);
            if (scfFlags.produceDivNaNPositive())
               fNegative = false;
         }
         else {
            biMantissa = bdSource.biMantissa;
            if (!biMantissa.cbitArray(bitSizeMantissa()-1)) {
               scfFlags.setSNaNOperand();
               biMantissa.setTrueBitArray(bitSizeMantissa()-1);
            }
            else if (bdSource.isSNaN())
               scfFlags.setSNaNOperand();
            if (scfFlags.keepNaNSign())
               fNegative = bdSource.fNegative;
         };
      };
         
      return *this;
   };
   
   // biExponent == 0
   if (bdSource.biExponent.isZero()) {
      if (!biMantissa.isZero() && !bdSource.biMantissa.isZero())
         return divAssignDD(bdSource, scfFlags);
      if (bdSource.biMantissa.isZero()) {
         biExponent = getInftyExponent();
         if (biMantissa.isZero()) { // produces a qNaN
            scfFlags.setZeroOnZero();
            biMantissa.setTrueBitArray(bitSizeMantissa()-1);
            if (scfFlags.produceDivNaNPositive())
               fNegative = false;
         }
         else {
            biMantissa = 0U;
            scfFlags.setDivisionByZero();
         };
      };
      return *this;
   };

   // biExponent == 0 && bdSource.biExponent != 0
   if (bdSource.biExponent == getInftyExponent()) {
      if (!bdSource.biMantissa.isZero()) {
         biExponent = getInftyExponent();
         biMantissa = bdSource.biMantissa;
         if (!biMantissa.cbitArray(bitSizeMantissa()-1)) {
            scfFlags.setSNaNOperand();
            biMantissa.setTrueBitArray(bitSizeMantissa()-1);
         };
         if (scfFlags.keepNaNSign())
            fNegative = bdSource.fNegative;
         return *this;
      };
      
      // bdSource.biMantissa == 0 && bdSource.biExponent == getInftyExponent() && biExponent == 0
      biMantissa = 0U;
      return *this;
   };
   
   // biExponent == 0 && bdSource.biExponent != 0 && bdSource.biExponent != getInftyExponent()
   if (biMantissa.isZero())
      return *this;
   return divAssignDN(bdSource, scfFlags);
}

template <class TypeTraits>
void
TBuiltDouble<TypeTraits>::read(std::istream& isIn, StatusAndControlFlags& scfFlags) {
   int uReadZero = 0;
   int uRead = isIn.get();
   int uDecimalShiftExponent = 0;
   operator=(thisType());
   bool fReadNegative = false;
   for (; isspace(uRead); uRead = isIn.get());
   if ((uRead == '-') || (uRead == '+')) {
      fReadNegative = (uRead == '-');
      uRead = isIn.get();
      for (; isspace(uRead); uRead = isIn.get());
   };
   for (; uRead == '0'; uRead = isIn.get());
   for (; (uRead >= '0') && (uRead <= '9'); uRead = isIn.get()) {
      if (uRead == '0')
         ++uReadZero;
      else {
         while (uReadZero > 0) {
            --uReadZero;
            multAssign(10U, scfFlags);
         };
         multAssign(10U, scfFlags);
         if (hasNegativeExponent()
               || (queryExponent().queryValue() < (unsigned int) bitSizeMantissa()+4)) // 2^4 > 10
            plusAssign(thisType((unsigned int) (uRead - '0')), scfFlags);
      };
   };

   if (uRead == '.') {
      uRead = isIn.get();
      for (; (uRead >= '0') && (uRead <= '9'); uRead = isIn.get()) {
         if (uRead == '0')
            ++uReadZero;
         else {
            while (uReadZero > 0) {
               --uReadZero;
               multAssign(10U, scfFlags);
            };
            multAssign(10U, scfFlags);
            if (hasNegativeExponent()
               || (queryExponent().queryValue() < (unsigned int) bitSizeMantissa()+4)) // 2^4 > 10
               plusAssign(thisType((unsigned int) (uRead - '0')), scfFlags);
         };
         --uDecimalShiftExponent;
      };
   };

   uDecimalShiftExponent += uReadZero;
   fNegative = fReadNegative;

   if (uRead == 'e') {
      if (isZero()) {
         uRead = isIn.get();
         if ((uRead == '+') || (uRead == '-'))
            uRead = isIn.get();
         for (; (uRead >= '0') && (uRead <= '9'); uRead = isIn.get());
         if (uRead != EOF)
            scfFlags.setPartialRead();
         return;
      };

      uRead = isIn.get();
      bool fNegativeExponent = false;
      if (uRead == '+')
         uRead = isIn.get();
      else if (uRead == '-') {
         uRead = isIn.get();
         fNegativeExponent = true;
      };
      int uDecimalExponent = 0;
      bool fInfty = false;
      for (; (uRead >= '0') && (uRead <= '9'); uRead = isIn.get()) {
         register int uOldDecimalExponent = uDecimalExponent;
         uDecimalExponent *= 10;
         uDecimalExponent += uRead - '0';
         if (uDecimalExponent < uOldDecimalExponent) {
            uDecimalExponent = 0;
            fInfty = true;
            break;
         };
      };
      
      if (uRead != EOF) {
         isIn.putback((char) uRead);
         scfFlags.setPartialRead();
      };

      if (!fInfty) {
         register int uOldDecimalExponent = uDecimalExponent;
         if (fNegativeExponent)
            uDecimalExponent = -uDecimalExponent;

         uDecimalExponent += uDecimalShiftExponent;
         if (((uDecimalShiftExponent > 0) && (uDecimalExponent < uOldDecimalExponent))
               || ((uDecimalShiftExponent < 0) && (uDecimalExponent > uOldDecimalExponent)))
            fInfty = true;
         else if ((fNegativeExponent = (uDecimalExponent < 0)) != false) {
            uDecimalExponent = -uDecimalExponent;
            if (uDecimalExponent < 0)
              fInfty = true;
         };
      };

      if (fInfty) {
         // +0, -0, +oo, -oo
         if (!fNegativeExponent)
            setInfty();
         if (fNegative)
            setSign(false);
         return;
      }
      else if (uDecimalExponent == 0)
         return;

      thisType dExponent(1U);
      unsigned int uSubExponent = 0;
      for (unsigned int uBitIndex = (1U << (log_base_2(uDecimalExponent)-1)); uBitIndex != 0;
            uBitIndex >>= 1) {
         dExponent.multAssign(dExponent, scfFlags);
         uSubExponent <<= 1;
         if (uDecimalExponent & uBitIndex)
            dExponent.multAssign(10U, scfFlags);
         if (fNegativeExponent && !uSubExponent && (dExponent.biExponent > biExponent)) {
            dExponent.biExponent -= getZeroExponent();
            uSubExponent += dExponent.biExponent.queryValue();
            dExponent.biExponent = getZeroExponent();
         };
      };
      if (!fNegativeExponent) {
         multAssign(dExponent, scfFlags);
         return;
      };

      if (!uSubExponent) {
         divAssign(dExponent, scfFlags);
         return;
      };

      divAssign(dExponent, scfFlags);

#ifndef __BORLANDC__
      Exponent biSubExponent = Exponent(typename Exponent::Basic(), uSubExponent);
#else
      Exponent biSubExponent = Exponent(Exponent::Basic(), uSubExponent);
#endif
      if (biExponent > biSubExponent)
         biExponent.minusAssign(biSubExponent);
      else {
         biSubExponent.minusAssign(biExponent);
         if (biSubExponent >= bitSizeMantissa()) {
            register bool fAdd = false;
            if (scfFlags.isNearestRound())
               fAdd = (biSubExponent == bitSizeMantissa());
            else if (scfFlags.isHighestRound())
               fAdd = !fNegative;
            else if (scfFlags.isLowestRound())
               fAdd = fNegative;
            if (fNegative)
               scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Down : StatusAndControlFlags::Up);
            else
               scfFlags.setApproximate(fAdd ? StatusAndControlFlags::Up : StatusAndControlFlags::Down);
            biMantissa = fAdd ? 1U : 0U;
         }
         else
            trightShift(biMantissa, biSubExponent.queryValue()+1, 1, scfFlags, fNegative, bitSizeMantissa());
         biExponent = 0U;
      };
      return;
   };

   // uRead != 'e'
   if (uRead != EOF) {
      isIn.putback((char) uRead);
      scfFlags.setPartialRead();
   };
   bool fNegativeExponent = (uDecimalShiftExponent < 0);
   unsigned int uDecimalExponent = fNegativeExponent ? -uDecimalShiftExponent : uDecimalShiftExponent;
   if (uDecimalExponent == 0)
      return;

   thisType dExponent(1U);
   for (unsigned int uBitIndex = (1U << (log_base_2(uDecimalExponent)-1)); uBitIndex != 0;
         uBitIndex >>= 1) {
      dExponent.multAssign(dExponent, scfFlags);
      if (uDecimalExponent & uBitIndex)
         dExponent.multAssign(10U, scfFlags);
   };
   if (!fNegativeExponent)
      multAssign(dExponent, scfFlags);
   else
      divAssign(dExponent, scfFlags);
}

template <class TypeTraits>
void
TBuiltDouble<TypeTraits>::write(std::ostream& osOut, const WriteParameters& wpParams) const {
   if (wpParams.isBinary()) {
      osOut.put(isPositive() ? '+' : '-').put(' ');
      if (isDenormalized())
         osOut << "0.";
      else if (isNormalized())
         osOut << "1.";
#ifndef __BORLANDC__
      biMantissa.write(osOut, typename Mantissa::FormatParameters().setFullBinary(biMantissa.querySize()));
#else
      biMantissa.write(osOut, Mantissa::FormatParameters().setFullBinary(biMantissa.querySize()));
#endif
      osOut << " 2^ ";
      if (hasNegativeExponent())
         osOut.put('-');
      osOut << (int) queryExponent().queryValue();
   }
   else
      writeDecimal(osOut);
}

template <class TypeTraits>
void
TBuiltDouble<TypeTraits>::writeDecimal(std::ostream& osOut) const {
   if (fNegative)
      osOut.put('-');
   if (isInfty()) {
      osOut << "oo";
      return;
   };
   if (isNaN()) {
      osOut << "NaN";
      return;
   };
   if (isZero()) {
      osOut.put('0');
      return;
   };
   StatusAndControlFlags scfFlags;
   scfFlags.setNearestRound();
   bool fNegativeExponent = biExponent < getZeroExponent();

   thisType dSource = *this;
   if (fNegative)
      dSource.opposite();
   thisType dExponent;
   std::vector<thisType> vSuccessiveExponents;
   unsigned int uAddNegativeDecimalExponent = 0;
   if (fNegativeExponent) {
      dExponent = thisType(10U);
      uAddNegativeDecimalExponent = 1;
      while (thisType(dExponent).multAssign(dSource, scfFlags).queryBasicExponent() < getZeroExponent()) {
         vSuccessiveExponents.push_back(dExponent);
         dExponent.multAssign(dExponent, scfFlags);
         uAddNegativeDecimalExponent *= 2;
      };
      uAddNegativeDecimalExponent /= 2;
   }
   else {
      dExponent = thisType(10U);
      while (dExponent <= dSource) {
         vSuccessiveExponents.push_back(dExponent);
         dExponent.multAssign(dExponent, scfFlags);
      };
   };
   
   unsigned int uDecimalExponent = 0;
   if (fNegativeExponent) {
      if (vSuccessiveExponents.empty()) {
         dSource.multAssign(10U, scfFlags);
         ++uAddNegativeDecimalExponent;
      }
      else {
         dSource.multAssign(vSuccessiveExponents.back(), scfFlags);
         int uExponentAdditional = 1;
         register int uIndexExponent = 0;
         while (thisType(dSource).multAssign(vSuccessiveExponents[uIndexExponent], scfFlags)
               .queryBasicExponent() < getZeroExponent()) {
            if (uIndexExponent < vSuccessiveExponents.size() - 1) {
               uExponentAdditional *= 2;
               ++uIndexExponent;
            }
            else {
               dSource.multAssign(vSuccessiveExponents.back(), scfFlags);
               uAddNegativeDecimalExponent += uExponentAdditional;
            };
         };
         int uCount = vSuccessiveExponents.size();
         while (++uIndexExponent < uCount)
            vSuccessiveExponents.pop_back();

         dSource.multAssign(vSuccessiveExponents.back(), scfFlags);
         uAddNegativeDecimalExponent += uExponentAdditional;
         assert(dSource.queryBasicExponent() >= getZeroExponent());
         vSuccessiveExponents.pop_back();
      };
   };
   while (!vSuccessiveExponents.empty()) {
      uDecimalExponent <<= 1;
      if (dSource >= vSuccessiveExponents.back()) {
         uDecimalExponent |= 1U;
         dSource.divAssign(vSuccessiveExponents.back(), scfFlags);
      };
      vSuccessiveExponents.pop_back();
   };

   assert((dSource >= thisType(1U)) && (dSource < thisType(10U)));
   bool fFirst = true;
   int uSignificantBits = (int) (((double) bitSizeMantissa())*log(2.0)/log(10.0));
   do {
      StatusAndControlFlags scfFlags;
      scfFlags.setNearestRound();
      unsigned int uWriteValue = 0U;
      if (dSource.hasPositiveOrNullExponent()) {
         int uLocalExponent = dSource.queryExponent().queryValue();
         if (uLocalExponent == 0U)
            uWriteValue = 1;
         else if (uLocalExponent == 1U)
            uWriteValue = (dSource.biMantissa.cbitArray(bitSizeMantissa()-1) ? 3 : 2);
         else if (uLocalExponent == 2U)
            uWriteValue = (dSource.biMantissa.cbitArray(bitSizeMantissa()-1)
               ? (dSource.biMantissa.cbitArray(bitSizeMantissa()-2) ? 7 : 6)
               : (dSource.biMantissa.cbitArray(bitSizeMantissa()-2) ? 5 : 4));
         else if (uLocalExponent == 3U) {
            uWriteValue =  (dSource.biMantissa.cbitArray(bitSizeMantissa()-3) ? 9 : 8);
            assert(!dSource.biMantissa.cbitArray(bitSizeMantissa()-2)
               && !dSource.biMantissa.cbitArray(bitSizeMantissa()-1));
         }
         else
            assert(false);
         osOut.put((char) (uWriteValue + '0'));
         dSource.minusAssign(thisType(uWriteValue), scfFlags);
      }
      else
         osOut.put('0');
      if (fFirst) {
         osOut.put('.');
         fFirst = false;
      };
      dSource.multAssign(10U, scfFlags);
   } while (--uSignificantBits >= 0);
   if (fNegativeExponent)
      uDecimalExponent = uAddNegativeDecimalExponent-uDecimalExponent;
   if (uDecimalExponent)
      osOut.put('e').put(fNegativeExponent ? '-' : '+') << (int) uDecimalExponent;
}

#ifndef __BORLANDC__
template <class TypeTraits>
typename TBuiltDouble<TypeTraits>::DiffDouble
TBuiltDouble<TypeTraits>::queryNumberOfFloatsBetween(const thisType& bdSource) const {
   DiffDouble ddResult;
#else
template <class TypeTraits>
void
TBuiltDouble<TypeTraits>::retrieveNumberOfFloatsBetween(const thisType& bdSource, DiffDouble& ddResult) const {
#endif

   if (fNegative == bdSource.fNegative) {
      Mantissa biDiffMantissa;
#ifndef __BORLANDC__
      Exponent biDiffExponent = Exponent(typename Exponent::Min());
#else
      Exponent biDiffExponent = Exponent(Exponent::Min());
#endif
      bool fIncrementThis = false;
      if (fNegative && bdSource.fNegative) {
         biDiffMantissa = biMantissa;
         fIncrementThis = biDiffMantissa.sub(bdSource.biMantissa).hasCarry();
         assert((biExponent > bdSource.biExponent)
                  || ((biExponent == bdSource.biExponent) && !fIncrementThis));
         biDiffExponent = biExponent;
         biDiffExponent -= bdSource.biExponent;
      }
      else {
         biDiffMantissa = bdSource.biMantissa;
         fIncrementThis = biDiffMantissa.sub(biMantissa).hasCarry();
         assert((biExponent < bdSource.biExponent)
                  || ((biExponent == bdSource.biExponent) && !fIncrementThis));
         biDiffExponent = bdSource.biExponent;
         biDiffExponent -= biExponent;
      };
      if (fIncrementThis)
         --biDiffExponent;

      for (register int uIndex = 0; uIndex <= biDiffMantissa.lastCellIndex(); ++uIndex)
         ddResult[uIndex] = biDiffMantissa[uIndex];
      DiffDouble ddDiffExponent;
      for (int uIndex = 0; uIndex <= biDiffExponent.lastCellIndex(); ++uIndex)
         ddDiffExponent[uIndex] = biDiffExponent[uIndex];
      ddResult += (ddDiffExponent <<= bitSizeMantissa());
   }
   else {
      assert(fNegative && !bdSource.fNegative);
      Mantissa biAddMantissa = biMantissa;
      bool fIncrementThis = biAddMantissa.add(bdSource.biMantissa).hasCarry();
      fIncrementThis = biAddMantissa.inc().hasCarry() || fIncrementThis;

      Exponent biAddExponent = Exponent(biExponent);
      bool fCarryExponent = biAddExponent.add(bdSource.biExponent).hasCarry();
      if (fIncrementThis)
         fCarryExponent = biAddExponent.inc().hasCarry() || fCarryExponent;

      for (register int uIndex = 0; uIndex <= biAddMantissa.lastCellIndex(); ++uIndex)
         ddResult[uIndex] = biAddMantissa[uIndex];
      DiffDouble ddAddExponent;
      for (int uIndex = 0; uIndex <= biAddExponent.lastCellIndex(); ++uIndex)
         ddAddExponent[uIndex] = biAddExponent[uIndex];
      ddResult += (ddAddExponent <<= bitSizeMantissa());
      if (fCarryExponent)
        ddResult.setTrueBitArray(bitSizeMantissa()+bitSizeExponent());
   };

   return ddResult;
}

template <class TypeTraits>
#if !defined(__GNUC__) || (GCC_VERSION >= 30000)
TBuiltDouble<TypeTraits>
#ifndef __GNUC__
TBuiltDouble<TypeTraits>::queryNthSuccessor(const DiffDouble& ddDiff) const {
#else
TBuiltDouble<TypeTraits>::queryNthSuccessor(const TBuiltDouble<TypeTraits>::DiffDouble& ddDiff) const {
#endif
#else
void
TBuiltDouble<TypeTraits>::retrieveNthSuccessor(thisType& bdResult, void* pvDiff) const {
   const DiffDouble& ddDiff = *((const DiffDouble*) pvDiff);
#endif

#if !defined(__GNUC__) || (GCC_VERSION >= 30000)
   if ((biExponent == getInftyExponent()) && (!biMantissa.isZero() || !fNegative))
      return *this;
#else
   if ((biExponent == getInftyExponent()) && (!biMantissa.isZero() || !fNegative)) {
      bdResult = *this;
      return;
   };
#endif

   DiffDouble ddThis, ddThisExponent;
   for (register int uIndex = 0; uIndex <= biMantissa.lastCellIndex(); ++uIndex)
      ddThis[uIndex] = biMantissa[uIndex];
   for (register int uIndex = 0; uIndex <= biExponent.lastCellIndex(); ++uIndex)
      ddThisExponent[uIndex] = biExponent[uIndex];
   ddThis |= (ddThisExponent <<= bitSizeMantissa());
#if !defined(__GNUC__) || (GCC_VERSION >= 30000)
   thisType bdResult;
#endif

   if (!fNegative) {
      if (ddThis.add(ddDiff).hasCarry() || ddThis.cbitArray(bitSizeMantissa()+bitSizeExponent())) {
         bdResult.biExponent = getInftyExponent();
#if !defined(__GNUC__) || (GCC_VERSION >= 30000)
         return bdResult;
#else
         return;
#endif
      };
   }
   else {
      if (ddThis.sub(ddDiff).hasCarry()) {
         ddThis.neg();
         if (ddThis.cbitArray(bitSizeMantissa()+bitSizeExponent())) {
            bdResult.biExponent = getInftyExponent();
#if !defined(__GNUC__) || (GCC_VERSION >= 30000)
            return bdResult;
#else
            return;
#endif
         };
      }
      else
         bdResult.fNegative = true;
   };
   for (register int uIndex = 0; uIndex <= bdResult.biMantissa.lastCellIndex(); ++uIndex)
      bdResult.biMantissa[uIndex] = ddThis[uIndex];
   bdResult.biMantissa.normalize();
   ddThis >>= bitSizeMantissa();
   for (register int uIndex = 0; uIndex <= bdResult.biExponent.lastCellIndex(); ++uIndex)
      bdResult.biExponent[uIndex] = ddThis[uIndex];
   if (bdResult.biExponent == getInftyExponent())
      bdResult.biMantissa = 0U;
#if !defined(__GNUC__) || (GCC_VERSION >= 30000)
   return bdResult;
#endif
}

template <class TypeTraits>
#if !defined(__GNUC__) || (GCC_VERSION >= 30000)
TBuiltDouble<TypeTraits>
TBuiltDouble<TypeTraits>::queryNthPredecessor(const DiffDouble& ddDiff) const {
#else
void
TBuiltDouble<TypeTraits>::retrieveNthPredecessor(thisType& bdResult, void* pvDiff) const {
   const DiffDouble& ddDiff = *((const DiffDouble*) pvDiff);
#endif

#if !defined(__GNUC__) || (GCC_VERSION >= 30000)
   if ((biExponent == getInftyExponent()) && (!biMantissa.isZero() || fNegative))
      return *this;
#else
   if ((biExponent == getInftyExponent()) && (!biMantissa.isZero() || fNegative)) {
      bdResult = *this;
      return;
   };
#endif

   DiffDouble ddThis, ddThisExponent;
   for (register int uIndex = 0; uIndex <= biMantissa.lastCellIndex(); ++uIndex)
      ddThis[uIndex] = biMantissa[uIndex];
   for (register int uIndex = 0; uIndex <= biExponent.lastCellIndex(); ++uIndex)
      ddThisExponent[uIndex] = biExponent[uIndex];
   ddThis |= (ddThisExponent <<= bitSizeMantissa());

#if !defined(__GNUC__) || (GCC_VERSION >= 30000)
   thisType bdResult;
#endif

   bdResult.fNegative = true;
   if (fNegative) {
      if (ddThis.add(ddDiff).hasCarry() || ddThis.cbitArray(bitSizeMantissa()+bitSizeExponent())) {
         bdResult.biExponent = getInftyExponent();
#if !defined(__GNUC__) || (GCC_VERSION >= 30000)
         return bdResult;
#else
         return;
#endif
      };
   }
   else {
      if (ddThis.sub(ddDiff).hasCarry()) {
         ddThis.neg();
         if (ddThis.cbitArray(bitSizeMantissa()+bitSizeExponent())) {
            bdResult.biExponent = getInftyExponent();
#if !defined(__GNUC__) || (GCC_VERSION >= 30000)
            return bdResult;
#else
            return;
#endif
         };
      }
      else
         bdResult.fNegative = false;
   };
   for (register int uIndex = 0; uIndex <= bdResult.biMantissa.lastCellIndex(); ++uIndex)
      bdResult.biMantissa[uIndex] = ddThis[uIndex];
   bdResult.biMantissa.normalize();
   ddThis >>= bitSizeMantissa();
   for (register int uIndex = 0; uIndex <= bdResult.biExponent.lastCellIndex(); ++uIndex)
      bdResult.biExponent[uIndex] = ddThis[uIndex];
#if !defined(__GNUC__) || (GCC_VERSION >= 30000)
   return bdResult;
#endif
}

/*******************************************/
/* Implementation - template TFloatingBase */
/*******************************************/

template <class TypeTraits>
void
#if !defined(__GNUC__) || (GCC_VERSION >= 30000)
TFloatingBase<TypeTraits>::fillMantissa(Mantissa& mMantissa) const {
#else
TFloatingBase<TypeTraits>::fillMantissa(void* pvMantissa) const {
   Mantissa& mMantissa = *((Mantissa*) pvMantissa);
#endif
   unsigned int auDouble[UByteSizeImplantation/sizeof(unsigned int)+1];
   memcpy((void*) auDouble, (void*) &dDouble, UByteSizeImplantation);
   unsigned int* puMask = (unsigned int*) auDouble;
   if (TypeTraits::isBigEndian())
      puMask += (UByteSizeImplantation/sizeof(unsigned int)-1);
   for (register int uMantissaIndex = 0;
         uMantissaIndex < mMantissa.lastCellIndex(); ++uMantissaIndex) {
      mMantissa[uMantissaIndex] = *puMask;
      if (TypeTraits::isBigEndian())
         --puMask;
      else
         ++puMask;
   };
   int uShift = TypeTraits::UBitSizeMantissa % (sizeof(unsigned int)*8);
   mMantissa[mMantissa.lastCellIndex()] = (uShift == 0) ? *puMask
      : ((*puMask) & ~((~0U) << uShift));
}

template <class TypeTraits>
void
#if !defined(__GNUC__) || (GCC_VERSION >= 30000)
TFloatingBase<TypeTraits>::setMantissa(const Mantissa& mMantissa) {
#else
TFloatingBase<TypeTraits>::setMantissa(void* pvMantissa) {
   const Mantissa& mMantissa = *((const Mantissa*) pvMantissa);
#endif
   unsigned int auDouble[UByteSizeImplantation/sizeof(unsigned int)+1];
   memcpy((void*) auDouble, (void*) &dDouble, UByteSizeImplantation);
   unsigned int* puMask = (unsigned int*) auDouble;
   if (TypeTraits::isBigEndian())
      puMask += (UByteSizeImplantation/sizeof(unsigned int)-1);
   for (register int uMantissaIndex = 0;
         uMantissaIndex < mMantissa.lastCellIndex(); ++uMantissaIndex) {
      *puMask = mMantissa[uMantissaIndex];
      if (TypeTraits::isBigEndian())
         --puMask;
      else
         ++puMask;
   };
   int uShift = TypeTraits::UBitSizeMantissa % (sizeof(unsigned int)*8);
   if (uShift == 0)
      *puMask = mMantissa[mMantissa.lastCellIndex()];
   else
      *puMask |= (mMantissa[mMantissa.lastCellIndex()] & ~((~0U) << uShift));
   memcpy((void*) &dDouble, (void*) auDouble, UByteSizeImplantation);
}

template <class TypeTraits>
void
TFloatingBase<TypeTraits>::test_double(double dDouble) { std::cout << dDouble << '\n'; }

template <class TypeTraits>
void
TFloatingBase<TypeTraits>::test_int(unsigned int uInt) { std::cout << uInt << '\n'; }

template <class TypeTraits>
void
TFloatingBase<TypeTraits>::test_char(char cChar) { std::cout << cChar << '\n'; }

template <class TypeTraits>
void
TFloatingBase<TypeTraits>::copy_mem(void* pvDestination, void* pvSource, int uBytes)
   {  memcpy(pvDestination, pvSource, uBytes); }

template <class TypeTraits>
#if !defined(__GNUC__) || (GCC_VERSION >= 30000)
TFloatingBase<TypeTraits>&
TFloatingBase<TypeTraits>::operator=(const BuiltDouble& bdSource) {
#else
void
TFloatingBase<TypeTraits>::assign(void* pvSource) {
   const BuiltDouble& bdSource = *((const BuiltDouble*) pvSource);
#endif

   setMantissa(bdSource.queryMantissa());
   setBasicExponent(bdSource.queryBasicExponent().queryValue());
   setSign(bdSource.isPositive());
#if !defined(__GNUC__) || (GCC_VERSION >= 30000)
   return *this;
#endif
}

template <class TypeTraits>
unsigned int
TFloatingBase<TypeTraits>::queryBasicExponent() const {
   unsigned char auDouble[UByteSizeImplantation];
   memcpy((void*) auDouble, (void*) &dDouble, UByteSizeImplantation);
   unsigned char* pcMask = (unsigned char*) auDouble;
   if (TypeTraits::isLittleEndian())
      pcMask += UByteSizeImplantation-1;
   unsigned int uResult = 0;
   int uShift = TypeTraits::UBitSizeExponent-7;
   uResult |= (*pcMask & 0x7f) << uShift;
   while ((uShift -= 8) >= 0) {
      if (TypeTraits::isLittleEndian())
         --pcMask;
      else
         ++pcMask;
      uResult |= *pcMask << uShift;
   };
   if (TypeTraits::isLittleEndian())
      --pcMask;
   else
      ++pcMask;
   if (uShift > -8)
      uResult |= *pcMask >> (-uShift);
   return uResult;
}

template <class TypeTraits>
void
TFloatingBase<TypeTraits>::setBasicExponent(unsigned int uExponent) {
   unsigned char auDouble[UByteSizeImplantation];
   memcpy((void*) auDouble, (void*) &dDouble, UByteSizeImplantation);
   unsigned char* pcMask = (unsigned char*) auDouble;
   if (TypeTraits::isLittleEndian())
      pcMask += UByteSizeImplantation-1;
   int uShift = TypeTraits::UBitSizeExponent-7;
   assert((TypeTraits::UBitSizeExponent == sizeof(unsigned int))
         || (((~0U << TypeTraits::UBitSizeExponent) & uExponent) == 0));
   *pcMask |= uExponent >> uShift;
   while ((uShift -= 8) >= 0) {
      if (TypeTraits::isLittleEndian())
         --pcMask;
      else
         ++pcMask;
      *pcMask |= uExponent >> uShift;
   };
   if (TypeTraits::isLittleEndian())
      --pcMask;
   else
      ++pcMask;
   if (uShift > -8)
      *pcMask |= uExponent << (-uShift);
   memcpy((void*) &dDouble, (void*) auDouble, UByteSizeImplantation);
}

/********************************************/
/* Implementation - template TDoubleElement */
/********************************************/

template <class FloatingBaseTraits>
bool
TDoubleElement<FloatingBaseTraits>::isNaN() const {
   if (!hasInftyExponent())
      return false;
   Mantissa mMantissa;
   fillMantissa(mMantissa);
   return !mMantissa.isZero();
}

template <class FloatingBaseTraits>
bool
TDoubleElement<FloatingBaseTraits>::isInternZero() const {
   if (inherited::queryBasicExponent() != 0)
      return false;
   Mantissa mMantissa;
   fillMantissa(mMantissa);
   return mMantissa.isZero();
}

template <class FloatingBaseTraits>
bool
TDoubleElement<FloatingBaseTraits>::isSNaN() const {
   if (!hasInftyExponent())
      return false;
   Mantissa mMantissa;
   fillMantissa(mMantissa);
   return !mMantissa.isZero() && !mMantissa.cbitArray(UBitSizeMantissa-1);
}

template <class FloatingBaseTraits>
bool
TDoubleElement<FloatingBaseTraits>::isQNaN() const {
   if (!hasInftyExponent())
      return false;
   Mantissa mMantissa;
   fillMantissa(mMantissa);
   return mMantissa.cbitArray(UBitSizeMantissa-1);
}

template <class FloatingBaseTraits>
bool
TDoubleElement<FloatingBaseTraits>::isInfty() const {
   if (!hasInftyExponent())
      return false;
   Mantissa mMantissa;
   fillMantissa(mMantissa);
   return mMantissa.isZero();
}

template <class FloatingBaseTraits>
void
TDoubleElement<FloatingBaseTraits>::write(std::ostream& osOut, const WriteParameters& wpParams) const {
   if (wpParams.isDecimal())
      osOut << inherited::implantation();
   else {
      Mantissa mMantissa;
      fillMantissa(mMantissa);
      osOut.put(inherited::isPositive() ? '+' : '-').put(' ');
#ifndef __BORLANDC__
      mMantissa.write(osOut, typename Mantissa::FormatParameters().setFullBinary(mMantissa.querySize()));
#else
      mMantissa.write(osOut, Mantissa::FormatParameters().setFullBinary(mMantissa.querySize()));
#endif
      osOut << " 2^ " << inherited::queryExponent();
   };
}


template <class FloatingBaseTraits>
bool
TDoubleElement<FloatingBaseTraits>::isPuiss2() const {
   Mantissa mMantissa;
   fillMantissa(mMantissa);
   return mMantissa.isZero();
}

template <class FloatingBaseTraits>
Errors
TDoubleElement<FloatingBaseTraits>::queryErrors() const {
   unsigned int uBasicExponent = inherited::queryBasicExponent();
   if (uBasicExponent == 0) {
      if (inherited::isZero())
         return Errors();
      return inherited::isPositive() ? Errors().setPositiveUnderflow() : Errors().setNegativeUnderflow();
   }
   else if (uBasicExponent == inherited::getMaxExponent()+inherited::getZeroExponent()+1) {
      if (isInfty())
         return inherited::isPositive() ? Errors().setPositiveOverflow() : Errors().setNegativeOverflow();
      return isQNaN() ? Errors().setQNaN() : Errors().setSNaN();
   };
   return Errors();
}

template <class FloatingBaseTraits>
void
TDoubleElement<FloatingBaseTraits>::read(std::istream& isIn, StatusAndControlFlags& scfFlags) {
   BuiltDouble bdDouble;
   bdDouble.read(isIn, scfFlags);
   operator=(bdDouble);
}

}} // end of namespace Numerics::Double

#include "Integer.template"

#endif // Numerics_Double_FloatingTemplate
