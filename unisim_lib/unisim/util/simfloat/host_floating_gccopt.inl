/***************************************************************************
                Floating_gccopt.inline  -  inline definitions for gcc
                             -------------------
    first release        : 15 Jul 2005
    copyright            : (C) 2004-2005 CEA
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

#ifdef DefineGCC_Inline_Double_TFloatingBase

inline double
DoubleTraits::max()
{ return DBL_MAX; }

inline double
DoubleTraits::normalizedMin()
{ return DBL_MIN; }

inline double
DoubleTraits::denormalizedMin()
{ return DBL_MIN*DBL_EPSILON; }

inline bool
DoubleTraits::isBigEndian()
{
#if defined(__GNUC__)
#if BYTE_ORDER == BIG_ENDIAN
   return true;
#else
   return false;
#endif
#else
   int dummy = 0x1234;
   return *((unsigned char*) &dummy) == 0x12;
#endif
}

inline bool
DoubleTraits::isLittleEndian()
{ return !isBigEndian(); }

inline unsigned int
DoubleTraits::getMaxExponent()
{ return DBL_MAX_EXP-1; }

inline unsigned int
DoubleTraits::getZeroExponent()
{ return DBL_MAX_EXP-1; }

inline double
DoubleTraits::epsilon()
{ return DBL_EPSILON; }

inline bool
DoubleTraits::isValid()
{ return (UBitSizeExponent <= (int) sizeof(int)*8); }

inline float
FloatTraits::max()
{ return FLT_MAX; }

inline float
FloatTraits::normalizedMin()
{ return FLT_MIN; }

inline float
FloatTraits::denormalizedMin()
{ return FLT_MIN*FLT_EPSILON; }

inline bool
FloatTraits::isBigEndian()
{
#if defined(__GNUC__)
#if BYTE_ORDER == BIG_ENDIAN
   return true;
#else
   return false;
#endif
#else
   int dummy = 0x1234;
   return *((unsigned char*) &dummy) == 0x12;
#endif
}

inline bool
FloatTraits::isLittleEndian()
{ return !isBigEndian(); }

inline unsigned int
FloatTraits::getMaxExponent()
{ return FLT_MAX_EXP-1; }

inline unsigned int
FloatTraits::getZeroExponent()
{ return FLT_MAX_EXP-1; }

inline float
FloatTraits::epsilon()
{ return FLT_EPSILON; }

inline bool
FloatTraits::isValid()
{ return (UBitSizeExponent <= (int) sizeof(int)*8); }

inline long double
LongDoubleTraits::max()
{ return LDBL_MAX; }

inline long double
LongDoubleTraits::normalizedMin()
{ return LDBL_MIN; }

inline long double
LongDoubleTraits::denormalizedMin()
{ return LDBL_MIN*LDBL_EPSILON; }

inline bool
LongDoubleTraits::isBigEndian()
{
#if defined(__GNUC__)
#if BYTE_ORDER == BIG_ENDIAN
   return true;
#else
   return false;
#endif
#else
   int dummy = 0x1234;
   return *((unsigned char*) &dummy) == 0x12;
#endif
}

inline bool
LongDoubleTraits::isLittleEndian()
{ return !isBigEndian(); }

inline unsigned int
LongDoubleTraits::getMaxExponent()
{ return LDBL_MAX_EXP-1; }

inline unsigned int
LongDoubleTraits::getZeroExponent()
{ return LDBL_MAX_EXP-1; }

inline long double
LongDoubleTraits::epsilon()
{ return LDBL_EPSILON; }

inline bool
LongDoubleTraits::isValid()
{ return (UBitSizeExponent <= (int) sizeof(int)*8); }

template <class TypeTraits>
inline
TFloatingBase<TypeTraits>::TFloatingBase() : dDouble(0.0) {}

template <class TypeTraits>
inline
TFloatingBase<TypeTraits>::TFloatingBase(const Implantation& dDoubleSource)
   : dDouble(dDoubleSource) {}

#if !defined (__GNUC__) || (GCC_VERSION >= 30000)
template <class TypeTraits>
inline
TFloatingBase<TypeTraits>::TFloatingBase(const BuiltDouble& bdDouble)
   : dDouble(0.0) { operator=(bdDouble); }
#endif

template <class TypeTraits>
inline
TFloatingBase<TypeTraits>::TFloatingBase(const thisType& sSource)
 : dDouble(sSource.dDouble) {}

template <class TypeTraits>
inline TFloatingBase<TypeTraits>&
TFloatingBase<TypeTraits>::operator=(const thisType& dsSource)
{  dDouble = dsSource.dDouble; return *this; }

#if !defined (__GNUC__) || (GCC_VERSION >= 30000)
template <class TypeTraits>
inline
TFloatingBase<TypeTraits>::operator typename TFloatingBase<TypeTraits>::BuiltDouble() const
{  BuiltDouble bdResult;
   fillMantissa(bdResult.querySMantissa());
#ifndef __BORLANDC__
   bdResult.setBasicExponent(typename BuiltDouble::Exponent(typename BuiltDouble::Exponent::Basic(), queryBasicExponent()));
#else
   bdResult.setBasicExponent(BuiltDouble::Exponent(BuiltDouble::Exponent::Basic(), queryBasicExponent()));
#endif
   bdResult.setSign(isPositive());
   return bdResult;
}
#endif

template <class TypeTraits>
inline typename TFloatingBase<TypeTraits>::Implantation&
TFloatingBase<TypeTraits>::implantation()
{ return dDouble; }

template <class TypeTraits>
inline const typename TFloatingBase<TypeTraits>::Implantation&
TFloatingBase<TypeTraits>::implantation() const
{ return dDouble; }

template <class TypeTraits>
inline TFloatingBase<TypeTraits>
TFloatingBase<TypeTraits>::queryEpsilon() const
{  Implantation dtiResult = implantation()*TypeTraits::epsilon();
   if ((dtiResult == 0.0) && (implantation() != 0.0))
      return thisType(TypeTraits::denormalizedMin());
   return thisType((dtiResult >= 0.0) ? dtiResult : -dtiResult);
}

template <class TypeTraits>
inline bool
TFloatingBase<TypeTraits>::isZero() const
{ return (dDouble == 0.0) || (-dDouble == 0.0); }

template <class TypeTraits>
inline TFloatingBase<TypeTraits>&
TFloatingBase<TypeTraits>::operator-=(const thisType& ttSource)
{  dDouble -= ttSource.dDouble; return *this; }

template <class TypeTraits>
inline TFloatingBase<TypeTraits>&
TFloatingBase<TypeTraits>::operator*=(const thisType& ttSource)
{  dDouble *= ttSource.dDouble; return *this; }

template <class TypeTraits>
inline TFloatingBase<TypeTraits>&
TFloatingBase<TypeTraits>::operator+=(const thisType& ttSource)
{  dDouble += ttSource.dDouble; return *this; }

template <class TypeTraits>
inline TFloatingBase<TypeTraits>&
TFloatingBase<TypeTraits>::operator/=(const thisType& ttSource)
{  dDouble /= ttSource.dDouble; return *this; }

template <class TypeTraits>
inline unsigned int
TFloatingBase<TypeTraits>::getMaxExponent()
{ return TypeTraits::getMaxExponent(); }

template <class TypeTraits>
inline unsigned int
TFloatingBase<TypeTraits>::getZeroExponent()
{ return TypeTraits::getZeroExponent(); }

template <class TypeTraits>
inline TFloatingBase<TypeTraits>&
TFloatingBase<TypeTraits>::opposite()
{ dDouble = -dDouble; return *this; }

template <class TypeTraits>
inline int
TFloatingBase<TypeTraits>::queryExponent() const
{  return ((int) queryBasicExponent()) - getZeroExponent(); }

template <class TypeTraits>
inline void
TFloatingBase<TypeTraits>::setExponent(int uExponent)
{  setBasicExponent(uExponent+getZeroExponent()); }

template <class TypeTraits>
inline bool
TFloatingBase<TypeTraits>::isPositive() const
{  unsigned char auDouble[UByteSizeImplantation];
   memcpy(auDouble, &dDouble, UByteSizeImplantation);
   unsigned char* pcMask = (unsigned char*) auDouble;
   if (TypeTraits::isLittleEndian())
      pcMask += UByteSizeImplantation-1;
   return !((*pcMask) & 0x80);
}

template <class TypeTraits>
inline bool
TFloatingBase<TypeTraits>::isNegative() const
{ return !isPositive(); }

template <class TypeTraits>
inline void
TFloatingBase<TypeTraits>::setSign(bool fPositive)
{  unsigned char auDouble[UByteSizeImplantation];
   memcpy(auDouble, &dDouble, UByteSizeImplantation);
   unsigned char* pcMask = (unsigned char*) auDouble;
   if (TypeTraits::isLittleEndian())
      pcMask += UByteSizeImplantation-1;
   if (fPositive)
      *pcMask &= 0x7f;
   else
      *pcMask |= 0x80;
   memcpy(&dDouble, auDouble, UByteSizeImplantation);
}

template <class TypeTraits>
inline void
TFloatingBase<TypeTraits>::setPositive()
{ setSign(true); }

template <class TypeTraits>
inline void
TFloatingBase<TypeTraits>::setNegative()
{ setSign(false); }

template <class TypeTraits>
inline bool
TFloatingBase<TypeTraits>::isNormalised() const
{  return ((TypeTraits::normalizedMin() <= dDouble) && (dDouble <= TypeTraits::max()))
      || ((-TypeTraits::normalizedMin() >= dDouble) && (dDouble >= -TypeTraits::max()));
}

template <class TypeTraits>
inline bool
TFloatingBase<TypeTraits>::isRanged() const
{  return ((TypeTraits::denormalizedMin() <= dDouble) && (dDouble <= TypeTraits::max()))
      || ((-TypeTraits::denormalizedMin() >= dDouble) && (dDouble >= -TypeTraits::max()));
}

template <class TypeTraits>
inline void
TFloatingBase<TypeTraits>::clear()
{ dDouble = 0.0; }

template <class TypeTraits>
inline void
TFloatingBase<TypeTraits>::swap(thisType& dSource)
{  Implantation dTemp = dDouble;
   dDouble = dSource.dDouble;
   dSource.dDouble = dTemp;
}

template <class TypeTraits>
inline TFloatingBase<TypeTraits>&
TFloatingBase<TypeTraits>::plusAssign(const thisType& ttSource, const StatusAndControlFlags& rpParams) {  
#ifdef DefineComputeParameters
#if defined(_MSC_VER) || defined(__BORLANDC__)
   int uOldState = _controlfp(0, _MCW_RC);
   _controlfp(_RC_DOWN, _MCW_RC);
   double dMin = dDouble+ttSource.dDouble;
   _controlfp(_RC_UP, _MCW_RC);
   double dMax = dDouble+ttSource.dDouble;
   if (rpParams.isLowestRound())
      _controlfp(_RC_DOWN, _MCW_RC);
   else if (rpParams.isHighestRound())
      _controlfp(_RC_UP, _MCW_RC);
   else if (rpParams.isNearestRound())
      _controlfp(_RC_NEAR, _MCW_RC);
   else // (rpParams.isZeroRound())
      _controlfp(_RC_CHOP, _MCW_RC);
   dDouble += ttSource.dDouble;
   _controlfp(uOldState, _MCW_RC);
#else
#ifdef __SUN__
   fpsetround(FP_RM);
   double dMin = dDouble+ttSource.dDouble;
   fpsetround(FP_RP);
   double dMax = dDouble+ttSource.dDouble;
   if (rpParams.isLowestRound())
      fpsetround(FP_RM);
   else if (rpParams.isHighestRound())
      fpsetround(FP_RP);
   else if (rpParams.isNearestRound())
      fpsetround(FP_RN);
   else // (rpParams.isZeroRound())
      fpsetround(FP_RZ);
   dDouble += ttSource.dDouble;
   fpsetround(FP_RN);
#else
#ifndef __CYGWIN__
   fesetround(FE_DOWNWARD);
   double dMin = dDouble+ttSource.dDouble;
   fesetround(FE_UPWARD);
   double dMax = dDouble+ttSource.dDouble;
   if (rpParams.isLowestRound())
      fesetround(FE_DOWNWARD);
   else if (rpParams.isHighestRound())
      fesetround(FE_UPWARD);
   else if (rpParams.isNearestRound())
      fesetround(FE_TONEAREST);
   else // (rpParams.isZeroRound())
      fesetround(FE_TOWARDZERO);
#endif
   dDouble += ttSource.dDouble;
#ifndef __CYGWIN__
   fesetround(FE_TONEAREST);
#endif
#endif
#endif
   if (dMin != dMax) {
      if (dDouble == dMin)
         rpParams.setDownApproximate();
      else {
         assert(dDouble == dMax);
         rpParams.setUpApproximate();
      };
   };
#else
#if defined(_MSC_VER) || defined(__BORLANDC__)
   int uOldState = _controlfp(0, _MCW_RC);
   if (rpParams.isLowestRound())
      _controlfp(_RC_DOWN, _MCW_RC);
   else if (rpParams.isHighestRound())
      _controlfp(_RC_UP, _MCW_RC);
   else if (rpParams.isNearestRound())
      _controlfp(_RC_NEAR, _MCW_RC);
   else // (rpParams.isZeroRound())
      _controlfp(_RC_CHOP, _MCW_RC);
   dDouble += ttSource.dDouble;
   _controlfp(uOldState, _MCW_RC);
#else
#ifdef __SUN__
   if (rpParams.isLowestRound())
      fpsetround(FP_RM);
   else if (rpParams.isHighestRound())
      fpsetround(FP_RP);
   else if (rpParams.isNearestRound())
      fpsetround(FP_RN);
   else // (rpParams.isZeroRound())
      fpsetround(FP_RZ);
   dDouble += ttSource.dDouble;
   fpsetround(FP_RN);
#else
#ifndef __CYGWIN__
   if (rpParams.isLowestRound())
      fesetround(FE_DOWNWARD);
   else if (rpParams.isHighestRound())
      fesetround(FE_UPWARD);
   else if (rpParams.isNearestRound())
      fesetround(FE_TONEAREST);
   else // (rpParams.isZeroRound())
      fesetround(FE_TOWARDZERO);
#endif
   dDouble += ttSource.dDouble;
#ifndef __CYGWIN__
   fesetround(FE_TONEAREST);
#endif
#endif
#endif
#endif // DefineComputeParameters
   return *this;
}

template <class TypeTraits>
inline TFloatingBase<TypeTraits>&
TFloatingBase<TypeTraits>::minusAssign(const thisType& ttSource, const StatusAndControlFlags& rpParams) {  
#ifdef DefineComputeParameters
#if defined(_MSC_VER) || defined(__BORLANDC__)
   int uOldState = _controlfp(0, _MCW_RC);
   _controlfp(_RC_DOWN, _MCW_RC);
   double dMin = dDouble-ttSource.dDouble;
   _controlfp(_RC_UP, _MCW_RC);
   double dMax = dDouble-ttSource.dDouble;
   if (rpParams.isLowestRound())
      _controlfp(_RC_DOWN, _MCW_RC);
   else if (rpParams.isHighestRound())
      _controlfp(_RC_UP, _MCW_RC);
   else if (rpParams.isNearestRound())
      _controlfp(_RC_NEAR, _MCW_RC);
   else // (rpParams.isZeroRound())
      _controlfp(_RC_CHOP, _MCW_RC);
   dDouble -= ttSource.dDouble;
   _controlfp(uOldState, _MCW_RC);
#else
#ifdef __SUN__
   fpsetround(FP_RM);
   double dMin = dDouble-ttSource.dDouble;
   fpsetround(FP_RP);
   double dMax = dDouble-ttSource.dDouble;
   if (rpParams.isLowestRound())
      fpsetround(FP_RM);
   else if (rpParams.isHighestRound())
      fpsetround(FP_RP);
   else if (rpParams.isNearestRound())
      fpsetround(FP_RN);
   else // (rpParams.isZeroRound())
      fpsetround(FP_RZ);
   dDouble -= ttSource.dDouble;
   fpsetround(FP_RN);
#else
#ifndef __CYGWIN__
   fesetround(FE_DOWNWARD);
   double dMin = dDouble-ttSource.dDouble;
   fesetround(FE_UPWARD);
   double dMax = dDouble-ttSource.dDouble;
   if (rpParams.isLowestRound())
      fesetround(FE_DOWNWARD);
   else if (rpParams.isHighestRound())
      fesetround(FE_UPWARD);
   else if (rpParams.isNearestRound())
      fesetround(FE_TONEAREST);
   else // (rpParams.isZeroRound())
      fesetround(FE_TOWARDZERO);
#endif
   dDouble -= ttSource.dDouble;
#ifndef __CYGWIN__
   fesetround(FE_TONEAREST);
#endif
#endif
#endif
   if (dMin != dMax) {
      if (dDouble == dMin)
         rpParams.setDownApproximate();
      else {
         assert(dDouble == dMax);
         rpParams.setUpApproximate();
      };
   };
#else
#if defined(_MSC_VER) || defined(__BORLANDC__)
   int uOldState = _controlfp(0, _MCW_RC);
   if (rpParams.isLowestRound())
      _controlfp(_RC_DOWN, _MCW_RC);
   else if (rpParams.isHighestRound())
      _controlfp(_RC_UP, _MCW_RC);
   else if (rpParams.isNearestRound())
      _controlfp(_RC_NEAR, _MCW_RC);
   else // (rpParams.isZeroRound())
      _controlfp(_RC_CHOP, _MCW_RC);
   dDouble -= ttSource.dDouble;
   _controlfp(uOldState, _MCW_RC);
#else
#ifdef __SUN__
   if (rpParams.isLowestRound())
      fpsetround(FP_RM);
   else if (rpParams.isHighestRound())
      fpsetround(FP_RP);
   else if (rpParams.isNearestRound())
      fpsetround(FP_RN);
   else // (rpParams.isZeroRound())
      fpsetround(FP_RZ);
   dDouble -= ttSource.dDouble;
   fpsetround(FP_RN);
#else
#ifndef __CYGWIN__
   if (rpParams.isLowestRound())
      fesetround(FE_DOWNWARD);
   else if (rpParams.isHighestRound())
      fesetround(FE_UPWARD);
   else if (rpParams.isNearestRound())
      fesetround(FE_TONEAREST);
   else // (rpParams.isZeroRound())
      fesetround(FE_TOWARDZERO);
#endif
   dDouble -= ttSource.dDouble;
#ifndef __CYGWIN__
   fesetround(FE_TONEAREST);
#endif
#endif
#endif
#endif
   return *this;
}

template <class TypeTraits>
inline TFloatingBase<TypeTraits>&
TFloatingBase<TypeTraits>::multAssign(const thisType& ttSource, const StatusAndControlFlags& rpParams) {  
#ifdef DefineComputeParameters
#if defined(_MSC_VER) || defined(__BORLANDC__)
   int uOldState = _controlfp(0, _MCW_RC);
   _controlfp(_RC_DOWN, _MCW_RC);
   double dMin = dDouble*ttSource.dDouble;
   _controlfp(_RC_UP, _MCW_RC);
   double dMax = dDouble*ttSource.dDouble;
   if (rpParams.isLowestRound())
      _controlfp(_RC_DOWN, _MCW_RC);
   else if (rpParams.isHighestRound())
      _controlfp(_RC_UP, _MCW_RC);
   else if (rpParams.isNearestRound())
      _controlfp(_RC_NEAR, _MCW_RC);
   else // (rpParams.isZeroRound())
      _controlfp(_RC_CHOP, _MCW_RC);
   dDouble *= ttSource.dDouble;
   _controlfp(uOldState, _MCW_RC);
#else
#ifdef __SUN__
   fpsetround(FP_RM);
   double dMin = dDouble*ttSource.dDouble;
   fpsetround(FP_RP);
   double dMax = dDouble*ttSource.dDouble;
   if (rpParams.isLowestRound())
      fpsetround(FP_RM);
   else if (rpParams.isHighestRound())
      fpsetround(FP_RP);
   else if (rpParams.isNearestRound())
      fpsetround(FP_RN);
   else // (rpParams.isZeroRound())
      fpsetround(FP_RZ);
   dDouble *= ttSource.dDouble;
   fpsetround(FP_RN);
#else
#ifndef __CYGWIN__
   fesetround(FE_DOWNWARD);
   double dMin = dDouble*ttSource.dDouble;
   fesetround(FE_UPWARD);
   double dMax = dDouble*ttSource.dDouble;
   if (rpParams.isLowestRound())
      fesetround(FE_DOWNWARD);
   else if (rpParams.isHighestRound())
      fesetround(FE_UPWARD);
   else if (rpParams.isNearestRound())
      fesetround(FE_TONEAREST);
   else // (rpParams.isZeroRound())
      fesetround(FE_TOWARDZERO);
#endif
   dDouble *= ttSource.dDouble;
#ifndef __CYGWIN__
   fesetround(FE_TONEAREST);
#endif
#endif
#endif
   if (dMin != dMax) {
      if (dDouble == dMin)
         rpParams.setDownApproximate();
      else {
         assert(dDouble == dMax);
         rpParams.setUpApproximate();
      };
   };
#else
#if defined(_MSC_VER) || defined(__BORLANDC__)
   int uOldState = _controlfp(0, _MCW_RC);
   if (rpParams.isLowestRound())
      _controlfp(_RC_DOWN, _MCW_RC);
   else if (rpParams.isHighestRound())
      _controlfp(_RC_UP, _MCW_RC);
   else if (rpParams.isNearestRound())
      _controlfp(_RC_NEAR, _MCW_RC);
   else // (rpParams.isZeroRound())
      _controlfp(_RC_CHOP, _MCW_RC);
   dDouble *= ttSource.dDouble;
   _controlfp(uOldState, _MCW_RC);
#else
#ifdef __SUN__
   if (rpParams.isLowestRound())
      fpsetround(FP_RM);
   else if (rpParams.isHighestRound())
      fpsetround(FP_RP);
   else if (rpParams.isNearestRound())
      fpsetround(FP_RN);
   else // (rpParams.isZeroRound())
      fpsetround(FP_RZ);
   dDouble *= ttSource.dDouble;
   fpsetround(FP_RN);
#else
#ifndef __CYGWIN__
   if (rpParams.isLowestRound())
      fesetround(FE_DOWNWARD);
   else if (rpParams.isHighestRound())
      fesetround(FE_UPWARD);
   else if (rpParams.isNearestRound())
      fesetround(FE_TONEAREST);
   else // (rpParams.isZeroRound())
      fesetround(FE_TOWARDZERO);
#endif
   dDouble *= ttSource.dDouble;
#ifndef __CYGWIN__
   fesetround(FE_TONEAREST);
#endif
#endif
#endif
#endif
   return *this;
}

template <class TypeTraits>
inline TFloatingBase<TypeTraits>&
TFloatingBase<TypeTraits>::divAssign(const thisType& ttSource, const StatusAndControlFlags& rpParams) {  
#ifdef DefineComputeParameters
#if defined(_MSC_VER) || defined(__BORLANDC__)
   int uOldState = _controlfp(0, _MCW_RC);
   _controlfp(_RC_DOWN, _MCW_RC);
   double dMin = dDouble/ttSource.dDouble;
   _controlfp(_RC_UP, _MCW_RC);
   double dMax = dDouble/ttSource.dDouble;
   if (rpParams.isLowestRound())
      _controlfp(_RC_DOWN, _MCW_RC);
   else if (rpParams.isHighestRound())
      _controlfp(_RC_UP, _MCW_RC);
   else if (rpParams.isNearestRound())
      _controlfp(_RC_NEAR, _MCW_RC);
   else // (rpParams.isZeroRound())
      _controlfp(_RC_CHOP, _MCW_RC);
   dDouble /= ttSource.dDouble;
   _controlfp(uOldState, _MCW_RC);
#else
#ifdef __SUN__
   fpsetround(FP_RM);
   double dMin = dDouble/ttSource.dDouble;
   fpsetround(FP_RP);
   double dMax = dDouble/ttSource.dDouble;
   if (rpParams.isLowestRound())
      fpsetround(FP_RM);
   else if (rpParams.isHighestRound())
      fpsetround(FP_RP);
   else if (rpParams.isNearestRound())
      fpsetround(FP_RN);
   else // (rpParams.isZeroRound())
      fpsetround(FP_RZ);
   dDouble /= ttSource.dDouble;
   fpsetround(FP_RN);
#else
#ifndef __CYGWIN__
   fesetround(FE_DOWNWARD);
   double dMin = dDouble/ttSource.dDouble;
   fesetround(FE_UPWARD);
   double dMax = dDouble/ttSource.dDouble;
   if (rpParams.isLowestRound())
      fesetround(FE_DOWNWARD);
   else if (rpParams.isHighestRound())
      fesetround(FE_UPWARD);
   else if (rpParams.isNearestRound())
      fesetround(FE_TONEAREST);
   else // (rpParams.isZeroRound())
      fesetround(FE_TOWARDZERO);
#endif
   dDouble /= ttSource.dDouble;
#ifndef __CYGWIN__
   fesetround(FE_TONEAREST);
#endif
#endif
#endif
   if (dMin != dMax) {
      if (dDouble == dMin)
         rpParams.setDownApproximate();
      else {
         assert(dDouble == dMax);
         rpParams.setUpApproximate();
      };
   };
#else
#if defined(_MSC_VER) || defined(__BORLANDC__)
   int uOldState = _controlfp(0, _MCW_RC);
   if (rpParams.isLowestRound())
      _controlfp(_RC_DOWN, _MCW_RC);
   else if (rpParams.isHighestRound())
      _controlfp(_RC_UP, _MCW_RC);
   else if (rpParams.isNearestRound())
      _controlfp(_RC_NEAR, _MCW_RC);
   else // (rpParams.isZeroRound())
      _controlfp(_RC_CHOP, _MCW_RC);
   dDouble /= ttSource.dDouble;
   _controlfp(uOldState, _MCW_RC);
#else
#ifdef __SUN__
   if (rpParams.isLowestRound())
      fpsetround(FP_RM);
   else if (rpParams.isHighestRound())
      fpsetround(FP_RP);
   else if (rpParams.isNearestRound())
      fpsetround(FP_RN);
   else // (rpParams.isZeroRound())
      fpsetround(FP_RZ);
   dDouble /= ttSource.dDouble;
   fpsetround(FP_RN);
#else
#ifndef __CYGWIN__
   if (rpParams.isLowestRound())
      fesetround(FE_DOWNWARD);
   else if (rpParams.isHighestRound())
      fesetround(FE_UPWARD);
   else if (rpParams.isNearestRound())
      fesetround(FE_TONEAREST);
   else // (rpParams.isZeroRound())
      fesetround(FE_TOWARDZERO);
#endif
   dDouble /= ttSource.dDouble;
#ifndef __CYGWIN__
   fesetround(FE_TONEAREST);
#endif
#endif
#endif
#endif
   return *this;
}

#endif // DefineGCC_Inline_Double_TFloatingBase

#ifdef DefineGCC_Inline_Double_TDoubleElement

template <class FloatingBaseTraits>
inline
TDoubleElement<FloatingBaseTraits>::TDoubleElement(): inherited() {}

template <class FloatingBaseTraits>
inline
TDoubleElement<FloatingBaseTraits>::TDoubleElement(int iValue, StatusAndControlFlags& scfFlags)
{  typename BuiltDouble::IntConversion icValue;
   icValue.setSigned().assign(iValue);
   BuiltDouble bdDouble(icValue, scfFlags);
   operator=(bdDouble);
}

template <class FloatingBaseTraits>
inline
TDoubleElement<FloatingBaseTraits>::TDoubleElement(typename FloatingBaseTraits::Implantation fbtSource)
   : inherited(fbtSource) {}

template <class FloatingBaseTraits>
inline
TDoubleElement<FloatingBaseTraits>::TDoubleElement(const BuiltDouble& bdDouble)
   : inherited(bdDouble) {}
 
template <class FloatingBaseTraits>
inline
TDoubleElement<FloatingBaseTraits>::TDoubleElement(const thisType& dSource)
   : inherited(dSource) {}

template <class FloatingBaseTraits>
inline int
TDoubleElement<FloatingBaseTraits>::queryInteger(StatusAndControlFlags& scfFlags) const
{  typename BuiltDouble::IntConversion icResult;
   BuiltDouble(*this).retrieveInteger(icResult.setSigned(), scfFlags);
   return icResult.queryInt();
}

template <class FloatingBaseTraits>
inline unsigned int
TDoubleElement<FloatingBaseTraits>::queryUnsignedInteger(StatusAndControlFlags& scfFlags) const
{  typename BuiltDouble::IntConversion icResult;
   BuiltDouble(*this).retrieveInteger(icResult.setUnsigned(), scfFlags);
   return icResult.queryUnsignedInt();
}

template <class FloatingBaseTraits>
inline bool
TDoubleElement<FloatingBaseTraits>::isZero() const
{ return inherited::isZero(); }

template <class FloatingBaseTraits>
inline bool
TDoubleElement<FloatingBaseTraits>::hasInftyExponent() const
{  return inherited::queryBasicExponent() == inherited::getMaxExponent()+inherited::getZeroExponent()+1; }

template <class FloatingBaseTraits>
inline typename TDoubleElement<FloatingBaseTraits>::ComparisonResult
TDoubleElement<FloatingBaseTraits>::compare(const thisType& bdSource) const
{  ComparisonResult crResult = CRNaN;
   if (!isNaN() && !bdSource.isNaN())
      crResult = (inherited::implantation() < bdSource.inherited::implantation()) ? CRLess
         : ((inherited::implantation() > bdSource.inherited::implantation()) ? CRGreater : CREqual);
   return crResult;
}

template <class FloatingBaseTraits>
inline bool
TDoubleElement<FloatingBaseTraits>::operator==(const thisType& bdSource) const
{ return compare(bdSource) == CREqual; }

template <class FloatingBaseTraits>
inline bool
TDoubleElement<FloatingBaseTraits>::operator!=(const thisType& bdSource) const
{  register ComparisonResult crResult = compare(bdSource);
   return (crResult == CRLess) || (crResult == CRGreater);
}

template <class FloatingBaseTraits>
inline bool
TDoubleElement<FloatingBaseTraits>::operator<(const thisType& bdSource) const
{ return compare(bdSource) == CRLess; }

template <class FloatingBaseTraits>
inline bool
TDoubleElement<FloatingBaseTraits>::operator>(const thisType& bdSource) const
{ return compare(bdSource) == CRGreater; }

template <class FloatingBaseTraits>
inline bool
TDoubleElement<FloatingBaseTraits>::operator<=(const thisType& bdSource) const
{  register ComparisonResult crResult = compare(bdSource);
   return (crResult == CRLess) || (crResult == CREqual);
}

template <class FloatingBaseTraits>
inline bool
TDoubleElement<FloatingBaseTraits>::operator>=(const thisType& bdSource) const
{  register ComparisonResult crResult = compare(bdSource);
   return (crResult == CRGreater) || (crResult == CREqual);
}

template <class FloatingBaseTraits>
inline typename TDoubleElement<FloatingBaseTraits>::DiffDouble
TDoubleElement<FloatingBaseTraits>::queryNumberOfFloatsBetween(const thisType& deElement) const
{  BuiltDouble bdThis = *this, bdElement = deElement;
   return bdThis.queryNumberOfFloatsBetween(bdElement);
}

template <class FloatingBaseTraits>
inline TDoubleElement<FloatingBaseTraits>
TDoubleElement<FloatingBaseTraits>::queryNthSuccessor(const DiffDouble& biIntSource) const
{  BuiltDouble bdThis = *this;
   return thisType(bdThis.queryNthSuccessor(biIntSource));
}

template <class FloatingBaseTraits>
inline TDoubleElement<FloatingBaseTraits>
TDoubleElement<FloatingBaseTraits>::queryNthPredecessor(const DiffDouble& biIntSource) const
{  BuiltDouble bdThis = *this;
   return thisType(bdThis.queryNthPredecessor(biIntSource));
}

template <class FloatingBaseTraits>
inline TDoubleElement<FloatingBaseTraits>&
TDoubleElement<FloatingBaseTraits>::opposite()
{ return (thisType&) inherited::opposite(); }

template <class FloatingBaseTraits>
inline TDoubleElement<FloatingBaseTraits>&
TDoubleElement<FloatingBaseTraits>::plusAssign(const thisType& ttSource, StatusAndControlFlags& scfFlags)
{  return (thisType&) inherited::plusAssign(ttSource, scfFlags); }

template <class FloatingBaseTraits>
inline TDoubleElement<FloatingBaseTraits>&
TDoubleElement<FloatingBaseTraits>::minusAssign(const thisType& ttSource, StatusAndControlFlags& scfFlags)
{  return (thisType&) inherited::minusAssign(ttSource, scfFlags); }

template <class FloatingBaseTraits>
inline TDoubleElement<FloatingBaseTraits>&
TDoubleElement<FloatingBaseTraits>::multAssign(const thisType& ttSource, StatusAndControlFlags& scfFlags)
{  return (thisType&) inherited::multAssign(ttSource, scfFlags); }

template <class FloatingBaseTraits>
inline TDoubleElement<FloatingBaseTraits>&
TDoubleElement<FloatingBaseTraits>::divAssign(const thisType& ttSource, StatusAndControlFlags& scfFlags)
{  return (thisType&) inherited::divAssign(ttSource, scfFlags); }

template <class FloatingBaseTraits>
inline TDoubleElement<FloatingBaseTraits>&
TDoubleElement<FloatingBaseTraits>::operator-=(const thisType& ttSource)
{  return (thisType&) inherited::operator-=(ttSource); }

template <class FloatingBaseTraits>
inline TDoubleElement<FloatingBaseTraits>&
TDoubleElement<FloatingBaseTraits>::operator*=(const thisType& ttSource)
{  return (thisType&) inherited::operator*=(ttSource); }

template <class FloatingBaseTraits>
inline TDoubleElement<FloatingBaseTraits>&
TDoubleElement<FloatingBaseTraits>::operator+=(const thisType& ttSource)
{  return (thisType&) inherited::operator+=(ttSource); }

template <class FloatingBaseTraits>
inline TDoubleElement<FloatingBaseTraits>&
TDoubleElement<FloatingBaseTraits>::operator/=(const thisType& ttSource)
{  return (thisType&) inherited::operator/=(ttSource); }

template <class FloatingBaseTraits>
inline TDoubleElement<FloatingBaseTraits>
TDoubleElement<FloatingBaseTraits>::operator-(const thisType& ttSource) const
{  thisType ttResult(*this);
   return (ttResult -= ttSource);
}

template <class FloatingBaseTraits>
inline TDoubleElement<FloatingBaseTraits>
TDoubleElement<FloatingBaseTraits>::operator+(const thisType& ttSource) const
{  thisType ttResult(*this);
   return (ttResult += ttSource);
}

template <class FloatingBaseTraits>
inline TDoubleElement<FloatingBaseTraits>
TDoubleElement<FloatingBaseTraits>::operator*(const thisType& ttSource) const
{  thisType ttResult(*this);
   return (ttResult *= ttSource);
}

template <class FloatingBaseTraits>
inline TDoubleElement<FloatingBaseTraits>
TDoubleElement<FloatingBaseTraits>::operator/(const thisType& ttSource) const
{  thisType ttResult(*this);
   return (ttResult /= ttSource);
}

template <class FloatingBaseTraits>
inline TDoubleElement<FloatingBaseTraits>&
TDoubleElement<FloatingBaseTraits>::setOpposite()
{  inherited::setSign(!inherited::isPositive());
   return *this;
}

template <class FloatingBaseTraits>
inline TDoubleElement<FloatingBaseTraits>&
TDoubleElement<FloatingBaseTraits>::setInftyExponent()
{  inherited::setBasicExponent(-1);
   return *this;
}

template <class FloatingBaseTraits>
inline std::ostream&
operator<<(std::ostream& osOut, const TDoubleElement<FloatingBaseTraits>& deDouble) {
   typename TDoubleElement<FloatingBaseTraits>::WriteParameters wpParams;
   deDouble.write(osOut, wpParams);
   return osOut;
}

template <class FloatingBaseTraits>
inline std::istream&
operator<<(std::istream& isIn, TDoubleElement<FloatingBaseTraits>& deDouble) {
   typename TDoubleElement<FloatingBaseTraits>::StatusAndControlFlags scfFlags;
   deDouble.read(isIn, scfFlags);
   return isIn;
}

#endif // DefineGCC_Inline_Double_TDoubleElement

