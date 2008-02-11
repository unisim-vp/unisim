/***************************************************************************
                Floating.template  -  Template for various types of floating point computations
                             -------------------
    first release        : 15 Jul 2005
    copyright �          : (C) 2004-2005 CEA
    authors              : Franck V�drine, Bruno Marre, Benjamin Blanc, Gilles Mouchard
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

#ifndef Numerics_Double_HostFloatingTemplate
#define Numerics_Double_HostFloatingTemplate
#include <unisim/util/simfloat/host_floating.hh>

namespace unisim {
namespace util {
namespace simfloat {

namespace Numerics {} // for precompiled headers

} // end of namespace simfloat
} // end of namespace util
} // end of namespace unisim

#include <unisim/util/simfloat/floating.tcc>

namespace unisim {
namespace util {
namespace simfloat {

namespace Numerics { namespace Double {

/*******************************************/
/* Implementation - template TFloatingBase */
/*******************************************/

template <class TypeTraits>
void
TFloatingBase<TypeTraits>::fillMantissa(Mantissa& mMantissa) const {
   unsigned int auDouble[UByteSizeImplantation/sizeof(unsigned int)+1];
   memcpy(auDouble, &dDouble, UByteSizeImplantation);
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
TFloatingBase<TypeTraits>::setMantissa(const Mantissa& mMantissa) {
   unsigned int auDouble[UByteSizeImplantation/sizeof(unsigned int)+1];
   memcpy(auDouble, &dDouble, UByteSizeImplantation);
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
   memcpy(&dDouble, auDouble, UByteSizeImplantation);
}

template <class TypeTraits>
TFloatingBase<TypeTraits>&
TFloatingBase<TypeTraits>::operator=(const BuiltDouble& bdSource) {
   setMantissa(bdSource.queryMantissa());
   setBasicExponent(bdSource.queryBasicExponent().queryValue());
   setSign(bdSource.isPositive());
   return *this;
}

template <class TypeTraits>
unsigned int
TFloatingBase<TypeTraits>::queryBasicExponent() const {
   unsigned char auDouble[UByteSizeImplantation];
   memcpy(auDouble, &dDouble, UByteSizeImplantation);
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
   memcpy(auDouble, &dDouble, UByteSizeImplantation);
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
   memcpy(&dDouble, auDouble, UByteSizeImplantation);
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
TDoubleElement<FloatingBaseTraits>::write(std::ostream& osOut, const WriteParameters& wpFlags) const {
   if (wpFlags.isDecimal())
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

} // end of namespace simfloat
} // end of namespace util
} // end of namespace unisim

#endif // Numerics_Double_HostFloatingTemplate
