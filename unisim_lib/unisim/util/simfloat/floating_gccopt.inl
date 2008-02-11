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

#ifdef DefineGCC_Inline_Double_Details_DTDoubleElement_Access

inline
Access::StatusAndControlFlags::StatusAndControlFlags()
   :  fAvoidInfty(false), fKeepNaNSign(false), fProduceDivNaNPositive(false),
      fRoundToEven(false), fPositiveZeroMAdd(false), fUpApproximateInfty(false),
      fUpApproximateInversionForNear(false), fChooseNaNAddBeforeMult(false),
      fConvertNaNNegative(false), fRefuseMinusZero(false), cContext(CMinDown),
      fInverseContext(false), rmRound(RMNearest), fKeepSignalingConversion(false),
      aApproximation(AExact), rrReadResult(RRTotal), fEffectiveRoundToEven(false),
      fSNaNOperand(false), qnrQNaNResult(QNNRUndefined), feExcept(FENoException),
      fDivisionByZero(false) {}

inline
Access::StatusAndControlFlags::StatusAndControlFlags(const StatusAndControlFlags& rpSource)
   :  fAvoidInfty(rpSource.fAvoidInfty), fKeepNaNSign(rpSource.fKeepNaNSign), fProduceDivNaNPositive(rpSource.fProduceDivNaNPositive),
      fRoundToEven(rpSource.fRoundToEven), fPositiveZeroMAdd(rpSource.fPositiveZeroMAdd),
      fUpApproximateInfty(rpSource.fUpApproximateInfty), fUpApproximateInversionForNear(rpSource.fUpApproximateInversionForNear),
      fChooseNaNAddBeforeMult(rpSource.fChooseNaNAddBeforeMult), fConvertNaNNegative(rpSource.fConvertNaNNegative),
      fRefuseMinusZero(rpSource.fRefuseMinusZero), cContext(rpSource.cContext),
      fInverseContext(rpSource.fInverseContext), rmRound(rpSource.rmRound),
      fKeepSignalingConversion(rpSource.fKeepSignalingConversion),
      aApproximation(rpSource.aApproximation), rrReadResult(rpSource.rrReadResult),
      fEffectiveRoundToEven(rpSource.fEffectiveRoundToEven), fSNaNOperand(rpSource.fSNaNOperand), qnrQNaNResult(rpSource.qnrQNaNResult),
      feExcept(rpSource.feExcept), fDivisionByZero(rpSource.fDivisionByZero) {}

inline Access::StatusAndControlFlags&
Access::StatusAndControlFlags::operator=(const StatusAndControlFlags& rpSource)
{  aApproximation = rpSource.aApproximation;
   rrReadResult = rpSource.rrReadResult;
   fEffectiveRoundToEven = rpSource.fEffectiveRoundToEven;
   fSNaNOperand = rpSource.fSNaNOperand;
   qnrQNaNResult = rpSource.qnrQNaNResult;
   feExcept = rpSource.feExcept;
   fDivisionByZero = rpSource.fDivisionByZero;
   return *this;
}

inline Access::StatusAndControlFlags&
Access::StatusAndControlFlags::setRoundToEven()
{  rmRound = RMNearest; fRoundToEven = true; return *this; }

inline Access::StatusAndControlFlags&
Access::StatusAndControlFlags::clearRoundToEven()
{ fRoundToEven = false; return *this; }

inline Access::StatusAndControlFlags&
Access::StatusAndControlFlags::setPositiveZeroMAdd()
{ fPositiveZeroMAdd = true; return *this; }

inline Access::StatusAndControlFlags&
Access::StatusAndControlFlags::avoidInfty()
{ fAvoidInfty = true; return *this; }

inline Access::StatusAndControlFlags&
Access::StatusAndControlFlags::clearAvoidInfty()
{ fAvoidInfty = false; return *this; }

inline Access::StatusAndControlFlags&
Access::StatusAndControlFlags::setKeepNaNSign()
{ fKeepNaNSign = true; return *this; }

inline Access::StatusAndControlFlags&
Access::StatusAndControlFlags::setProduceDivNaNPositive()
{ fProduceDivNaNPositive = true; return *this; }

inline Access::StatusAndControlFlags&
Access::StatusAndControlFlags::setUpApproximateInfty()
{ fUpApproximateInfty = true; return *this; }

inline Access::StatusAndControlFlags&
Access::StatusAndControlFlags::setUpApproximateInversionForNear()
{ fUpApproximateInversionForNear = true; return *this; }

inline Access::StatusAndControlFlags&
Access::StatusAndControlFlags::setChooseNaNAddBeforeMult()
{ fChooseNaNAddBeforeMult = true; return *this; }

inline Access::StatusAndControlFlags&
Access::StatusAndControlFlags::setConvertNaNNegative()
{ fConvertNaNNegative= true; return *this; }

inline Access::StatusAndControlFlags&
Access::StatusAndControlFlags::setAcceptMinusZero()
{ fRefuseMinusZero = false; return *this; }

inline Access::StatusAndControlFlags&
Access::StatusAndControlFlags::setRefuseMinusZero()
{ fRefuseMinusZero = true; return *this; }

inline Access::StatusAndControlFlags&
Access::StatusAndControlFlags::setMinDown()
{ cContext = CMinDown; return *this; }

inline Access::StatusAndControlFlags&
Access::StatusAndControlFlags::setMinUp()
{ cContext = CMinUp; return *this; }

inline Access::StatusAndControlFlags&
Access::StatusAndControlFlags::setMaxDown()
{ cContext = CMaxDown; return *this; }

inline Access::StatusAndControlFlags&
Access::StatusAndControlFlags::setMaxUp()
{ cContext = CMaxUp; return *this; }

inline Access::StatusAndControlFlags&
Access::StatusAndControlFlags::setInverseContext()
{ fInverseContext = true; return *this; }

inline Access::StatusAndControlFlags&
Access::StatusAndControlFlags::clearInverseContext()
{ fInverseContext = false; return *this; }

inline bool
Access::StatusAndControlFlags::isRoundToEven() const
{ return fRoundToEven && isNearestRound(); }

inline bool
Access::StatusAndControlFlags::isPositiveZeroMAdd()
{ return fPositiveZeroMAdd; }

inline bool
Access::StatusAndControlFlags::isInftyAvoided() const
{ return fAvoidInfty; }

inline bool
Access::StatusAndControlFlags::doesAvoidInfty(bool fNegative) const
{  assert(fAvoidInfty);
            return fNegative ? (rmRound >= RMHighest) : (rmRound & RMLowest);
         }

inline bool
Access::StatusAndControlFlags::keepNaNSign() const
{ return fKeepNaNSign; }

inline bool
Access::StatusAndControlFlags::produceDivNaNPositive() const
{ return fProduceDivNaNPositive; }

inline bool
Access::StatusAndControlFlags::upApproximateInfty() const
{ return fUpApproximateInfty; }

inline bool
Access::StatusAndControlFlags::upApproximateInversionForNear() const
{ return fUpApproximateInversionForNear; }

inline bool
Access::StatusAndControlFlags::chooseNaNAddBeforeMult() const
{ return fChooseNaNAddBeforeMult; }

inline bool
Access::StatusAndControlFlags::isConvertNaNNegative() const
{ return fConvertNaNNegative; }

inline bool
Access::StatusAndControlFlags::acceptMinusZero() const
{ return !fRefuseMinusZero; }

inline bool
Access::StatusAndControlFlags::isContextUp() const
{ return cContext & CMinUp; }

inline bool
Access::StatusAndControlFlags::isContextMax() const
{ return cContext & CMaxDown; }

inline bool
Access::StatusAndControlFlags::doesInverseContext() const
{ return fInverseContext; }

inline Access::StatusAndControlFlags&
Access::StatusAndControlFlags::setNearestRound()
{ rmRound = RMNearest; return *this; }

inline Access::StatusAndControlFlags&
Access::StatusAndControlFlags::setHighestRound()
{ rmRound = RMHighest; return *this; }

inline Access::StatusAndControlFlags&
Access::StatusAndControlFlags::setLowestRound()
{ rmRound = RMLowest; return *this; }

inline Access::StatusAndControlFlags&
Access::StatusAndControlFlags::setZeroRound()
{ rmRound = RMZero; return *this; }

inline bool
Access::StatusAndControlFlags::isLowestRound() const
{ return rmRound == RMLowest; }

inline bool
Access::StatusAndControlFlags::isNearestRound() const
{ return rmRound == RMNearest; }

inline bool
Access::StatusAndControlFlags::isHighestRound() const
{ return rmRound == RMHighest; }

inline bool
Access::StatusAndControlFlags::isZeroRound() const
{ return rmRound == RMZero; }

inline Access::StatusAndControlFlags&
Access::StatusAndControlFlags::setKeepSignalingConversion()
{ fKeepSignalingConversion = true; return *this; }

inline Access::StatusAndControlFlags&
Access::StatusAndControlFlags::clearKeepSignalingConversion()
{ fKeepSignalingConversion = false; return *this; }

inline bool
Access::StatusAndControlFlags::keepSignalingConversion() const
{ return fKeepSignalingConversion; }

inline bool
Access::StatusAndControlFlags::isApproximate() const
{ return aApproximation != AExact; }

inline bool
Access::StatusAndControlFlags::isDownApproximate() const
{ return aApproximation == ADownApproximate; }

inline bool
Access::StatusAndControlFlags::isUpApproximate() const
{ return aApproximation == AUpApproximate; }

inline void
Access::StatusAndControlFlags::setDownApproximate()
{ aApproximation = ADownApproximate; }

inline void
Access::StatusAndControlFlags::setUpApproximate()
{ aApproximation = AUpApproximate; }

inline void
Access::StatusAndControlFlags::clearApproximate()
{ aApproximation = AExact; }

inline void
Access::StatusAndControlFlags::setApproximate(Direction dDirection)
{  aApproximation = ((dDirection == Down) ? ADownApproximate : AUpApproximate); }

inline bool
Access::StatusAndControlFlags::isApproximate(Direction dDirection) const
{  return aApproximation == ((dDirection == Down) ? ADownApproximate : AUpApproximate); }

inline bool
Access::StatusAndControlFlags::hasSameApproximation(const StatusAndControlFlags& rpSource) const
{  return aApproximation == rpSource.aApproximation; }

inline bool
Access::StatusAndControlFlags::hasIncrementFraction(bool fNegative) const
{  return fNegative ? isDownApproximate() : isUpApproximate(); }

inline void
Access::StatusAndControlFlags::setEffectiveRoundToEven()
{ fEffectiveRoundToEven = true; }

inline void
Access::StatusAndControlFlags::clearEffectiveRoundToEven()
{ fEffectiveRoundToEven = false; }

inline bool
Access::StatusAndControlFlags::hasEffectiveRoundToEven()
{ return fEffectiveRoundToEven; }

inline void
Access::StatusAndControlFlags::setPartialRead()
{ rrReadResult = RRPartial; }

inline void
Access::StatusAndControlFlags::setTotalRead()
{ rrReadResult = RRTotal; }

inline bool
Access::StatusAndControlFlags::isPartialRead() const
{ return rrReadResult == RRPartial; }

inline bool
Access::StatusAndControlFlags::isTotalRead() const
{ return rrReadResult == RRTotal; }

inline bool
Access::StatusAndControlFlags::hasPartialRead() const
{ return rrReadResult != RRTotal; }

inline void
Access::StatusAndControlFlags::setSNaNOperand()
{ fSNaNOperand = true; }

inline bool
Access::StatusAndControlFlags::hasSNaNOperand() const
{ return fSNaNOperand; }
   
inline void
Access::StatusAndControlFlags::setInftyMinusInfty()
{ assert(!qnrQNaNResult); qnrQNaNResult = QNNRInftyMinusInfty; }

inline void
Access::StatusAndControlFlags::setInftyOnInfty()
{ assert(!qnrQNaNResult); qnrQNaNResult = QNNRInftyOnInfty; }

inline void
Access::StatusAndControlFlags::setZeroOnZero()
{ assert(!qnrQNaNResult); qnrQNaNResult = QNNRZeroOnZero; }

inline void
Access::StatusAndControlFlags::setInftyMultZero()
{ assert(!qnrQNaNResult); qnrQNaNResult = QNNRInftyMultZero; }

inline bool
Access::StatusAndControlFlags::hasQNaNResult() const
{ return qnrQNaNResult; }

inline bool
Access::StatusAndControlFlags::isInftyMinusInfty() const
{ return qnrQNaNResult == QNNRInftyMinusInfty; }

inline bool
Access::StatusAndControlFlags::isInftyOnInfty() const
{ return qnrQNaNResult == QNNRInftyOnInfty; }

inline bool
Access::StatusAndControlFlags::isZeroOnZero() const
{ return qnrQNaNResult == QNNRZeroOnZero; }

inline bool
Access::StatusAndControlFlags::isInftyMultZero() const
{ return qnrQNaNResult == QNNRInftyMultZero; }

inline void
Access::StatusAndControlFlags::clear()
{  aApproximation = AExact;
   rrReadResult = RRTotal;
   fEffectiveRoundToEven = false;
   fSNaNOperand = false;
   qnrQNaNResult = QNNRUndefined;
   feExcept = FENoException;
   fDivisionByZero = false;
}

inline bool
Access::StatusAndControlFlags::isDivisionByZero() const
{ return fDivisionByZero; }

inline void
Access::StatusAndControlFlags::setDivisionByZero()
{ fDivisionByZero = true; }

inline bool
Access::StatusAndControlFlags::hasFlowException() const
{ return feExcept != FENoException; }

inline void
Access::StatusAndControlFlags::clearFlowException()
{ feExcept = FENoException; }

inline void
Access::StatusAndControlFlags::setOverflow()
{ feExcept = FEOverflow; }

inline void
Access::StatusAndControlFlags::setUnderflow()
{ feExcept = FEUnderflow; }

inline bool
Access::StatusAndControlFlags::isOverflow() const
{ return feExcept == FEOverflow; }

inline bool
Access::StatusAndControlFlags::isUnderflow() const
{ return feExcept == FEUnderflow; }

inline void
Access::StatusAndControlFlags::clearUnderflow()
{ feExcept = FENoException; }

inline
Access::WriteParameters::WriteParameters() : tType(TDecimal) {}

inline Access::WriteParameters&
Access::WriteParameters::setDecimal()
{ tType = TDecimal; return *this; }

inline Access::WriteParameters&
Access::WriteParameters::setBinary()
{ tType = TBinary; return *this; }

inline bool
Access::WriteParameters::isDecimal() const
{ return tType == TDecimal; }

inline bool
Access::WriteParameters::isBinary() const
{ return tType == TBinary; }

inline
Access::Carry::Carry() : fCarry(false) {}

inline
Access::Carry::Carry(const Carry& cSource) : fCarry(cSource.fCarry) {}

inline bool&
Access::Carry::carry()
{ return fCarry; }

inline const bool&
Access::Carry::carry() const
{ return fCarry; }

inline bool
Access::Carry::hasCarry() const
{ return fCarry; }

inline bool
Access::isBigEndian()
{
#if defined(__GNUC__) && defined(__LINUX__)
#if BYTE_ORDER == BIG_ENDIAN
   return true;
#else
   return false;
#endif
#else
   int dummy = 0x1234;
   unsigned char chDummy[4];
   memcpy((unsigned char*) chDummy, &dummy, 4);
   return *chDummy == 0x12;
#endif
}

#endif // DefineGCC_Inline_Double_Details_DTDoubleElement_Access

#ifdef DefineGCC_Inline_Double_Details_DBuiltDoubleTraits_Access
template <int USizeMantissa, int USizeExponent>
inline
Access::TFloatConversion<USizeMantissa, USizeExponent>::TFloatConversion()
 : bciMantissa(), uBitsMantissa(0), bciExponent(), uBitsExponent(0), fNegative(false) {}

template <int USizeMantissa, int USizeExponent>
inline
Access::TFloatConversion<USizeMantissa, USizeExponent>::TFloatConversion(const thisType& fcSource)
   :  bciMantissa(fcSource.bciMantissa), uBitsMantissa(fcSource.uBitsMantissa),
      bciExponent(fcSource.bciExponent), uBitsExponent(fcSource.uBitsExponent),
      fNegative(fcSource.fNegative) {}

template <int USizeMantissa, int USizeExponent>
inline bool
Access::TFloatConversion<USizeMantissa, USizeExponent>::isPositive() const
{ return !fNegative; }

template <int USizeMantissa, int USizeExponent>
inline bool
Access::TFloatConversion<USizeMantissa, USizeExponent>::isNegative() const
{ return fNegative; }

template <int USizeMantissa, int USizeExponent>
inline bool
Access::TFloatConversion<USizeMantissa, USizeExponent>::isInftyExponent() const
{ return Exponent(bciExponent).neg(uBitsExponent).hasZero(uBitsExponent); }

template <int USizeMantissa, int USizeExponent>
inline bool
Access::TFloatConversion<USizeMantissa, USizeExponent>::isZeroExponent() const
{ return bciExponent.hasZero(uBitsExponent); }

template <int USizeMantissa, int USizeExponent>
inline bool
Access::TFloatConversion<USizeMantissa, USizeExponent>::isZeroMantissa() const
{ return bciMantissa.hasZero(uBitsMantissa); }

template <int USizeMantissa, int USizeExponent>
inline const int&
Access::TFloatConversion<USizeMantissa, USizeExponent>::querySizeMantissa() const
{ return uBitsMantissa; }

template <int USizeMantissa, int USizeExponent>
inline const int&
Access::TFloatConversion<USizeMantissa, USizeExponent>::querySizeExponent() const
{ return uBitsExponent; }

template <int USizeMantissa, int USizeExponent>
inline Access::TFloatConversion<USizeMantissa, USizeExponent>&
Access::TFloatConversion<USizeMantissa, USizeExponent>::setSizeMantissa(int uSize)
{ uBitsMantissa = uSize; return *this; }

template <int USizeMantissa, int USizeExponent>
inline Access::TFloatConversion<USizeMantissa, USizeExponent>&
Access::TFloatConversion<USizeMantissa, USizeExponent>::setSizeExponent(int uSize)
{ uBitsExponent = uSize; return *this; }

template <int USizeMantissa, int USizeExponent>
inline void
Access::TFloatConversion<USizeMantissa, USizeExponent>::setPositive(bool fPositive)
{ fNegative = !fPositive; }

template <int USizeMantissa, int USizeExponent>
inline void
Access::TFloatConversion<USizeMantissa, USizeExponent>::setNegative(bool fNegativeSource)
{ fNegative = fNegativeSource; }

template <int USizeMantissa, int USizeExponent>
inline const typename Access::TFloatConversion<USizeMantissa, USizeExponent>::Mantissa&
Access::TFloatConversion<USizeMantissa, USizeExponent>::mantissa() const
{ return bciMantissa; }

template <int USizeMantissa, int USizeExponent>
inline typename Access::TFloatConversion<USizeMantissa, USizeExponent>::Mantissa&
Access::TFloatConversion<USizeMantissa, USizeExponent>::mantissa()
{ return bciMantissa; }

template <int USizeMantissa, int USizeExponent>
inline const typename Access::TFloatConversion<USizeMantissa, USizeExponent>::Exponent&
Access::TFloatConversion<USizeMantissa, USizeExponent>::exponent() const
{ return bciExponent; }

template <int USizeMantissa, int USizeExponent>
inline typename Access::TFloatConversion<USizeMantissa, USizeExponent>::Exponent&
Access::TFloatConversion<USizeMantissa, USizeExponent>::exponent()
{ return bciExponent; }

#endif // DefineGCC_Inline_Double_Details_DBuiltDoubleTraits_Access

#ifdef DefineGCC_Inline_Double_BuiltDoubleTraits

template <int BitSizeMantissa, int BitSizeExponent>
inline void
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::setChunkSize(int uChunkSize) const
{ assert(uChunkSize == (BitSizeMantissa+BitSizeExponent+1+7)/8); }

template <int BitSizeMantissa, int BitSizeExponent>
inline void
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::copyChunk(CharChunk& ccChunk, void* pChunk, int uChunkSize) const
{  assert(uChunkSize == (BitSizeMantissa+BitSizeExponent+1+7)/8);
   memcpy((unsigned char*) ccChunk, pChunk, uChunkSize);
}

template <int BitSizeMantissa, int BitSizeExponent>
inline void
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::retrieveChunk(void* pChunk, const CharChunk& ccChunk, int uChunkSize) const
{  assert(uChunkSize == (BitSizeMantissa+BitSizeExponent+1+7)/8);
   memcpy(pChunk, (unsigned char*) ccChunk, uChunkSize);
}

template <int BitSizeMantissa, int BitSizeExponent>
inline void
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::clearChunk(CharChunk& ccChunk, int uChunkSize) const
{  assert(uChunkSize == (BitSizeMantissa+BitSizeExponent+1+7)/8);
   memset((unsigned char*) ccChunk, 0, uChunkSize);
}

template <int BitSizeMantissa, int BitSizeExponent>
inline
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Mantissa::Mantissa()
{}

template <int BitSizeMantissa, int BitSizeExponent>
inline 
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Mantissa::Mantissa(const Mantissa& mSource)
 : inherited(mSource) {}

template <int BitSizeMantissa, int BitSizeExponent>
inline
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Mantissa::Mantissa(const ExtendedMantissa& emSource)
{  for (register int uIndex = 0; uIndex < inherited::uCellSizeMantissa; ++uIndex)
      inherited::array(uIndex) = emSource[uIndex];
}

template <int BitSizeMantissa, int BitSizeExponent>
inline void
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Mantissa::normalizeLastCell()
{ inherited::normalizeLastCell(); }

template <int BitSizeMantissa, int BitSizeExponent>
inline typename BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Mantissa&
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Mantissa::operator=(unsigned int uValue)
{ return (Mantissa&) inherited::operator=(uValue); }

template <int BitSizeMantissa, int BitSizeExponent>
inline typename BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Mantissa&
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Mantissa::operator=(const Mantissa& mSource)
{  return (Mantissa&) inherited::operator=(mSource); }

template <int BitSizeMantissa, int BitSizeExponent>
inline typename BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Mantissa&
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Mantissa::operator=(const ExtendedMantissa& emSource)
{  for (register int uIndex = 0; uIndex <= inherited::lastCellIndex(); ++uIndex)
      inherited::array(uIndex) = emSource[uIndex];
   inherited::normalize();
   return *this;
}

template <int BitSizeMantissa, int BitSizeExponent>
inline
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Exponent::Min::Min() {}

template <int BitSizeMantissa, int BitSizeExponent>
inline
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Exponent::Zero::Zero() {}

template <int BitSizeMantissa, int BitSizeExponent>
inline
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Exponent::One::One() {}

template <int BitSizeMantissa, int BitSizeExponent>
inline
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Exponent::MinusOne::MinusOne() {}

template <int BitSizeMantissa, int BitSizeExponent>
inline
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Exponent::Max::Max() {}

template <int BitSizeMantissa, int BitSizeExponent>
inline
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Exponent::Basic::Basic() {}

template <int BitSizeMantissa, int BitSizeExponent>
inline
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Exponent::Exponent() {}
 
template <int BitSizeMantissa, int BitSizeExponent>
inline
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Exponent::Exponent(Basic, unsigned int uBasicValue)
 : inherited(uBasicValue) {}
 
template <int BitSizeMantissa, int BitSizeExponent>
inline
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Exponent::Exponent(Min)
{}

template <int BitSizeMantissa, int BitSizeExponent>
inline
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Exponent::Exponent(Max)
{ inherited::neg(); }

template <int BitSizeMantissa, int BitSizeExponent>
inline
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Exponent::Exponent(Zero)
{ inherited::neg(); inherited::bitArray(UBitSizeExponent-1) = false; }

template <int BitSizeMantissa, int BitSizeExponent>
inline
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Exponent::Exponent(MinusOne)
{  inherited::neg();
   inherited::bitArray(UBitSizeExponent-1) = false;
   inherited::bitArray(0) = false;
}

template <int BitSizeMantissa, int BitSizeExponent>
inline
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Exponent::Exponent(One)
{ inherited::bitArray(UBitSizeExponent-1) = true; }

template <int BitSizeMantissa, int BitSizeExponent>
inline
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Exponent::Exponent(const Exponent& eSource)
 : inherited(eSource) {}

template <int BitSizeMantissa, int BitSizeExponent>
inline typename BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Exponent&
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Exponent::operator=(const Exponent& eSource)
{ return (Exponent&) inherited::operator=(eSource); }

template <int BitSizeMantissa, int BitSizeExponent>
inline typename BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Exponent&
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Exponent::operator=(unsigned int uSource)
{ return (Exponent&) inherited::operator=(uSource); }

template <int BitSizeMantissa, int BitSizeExponent>
inline typename BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Exponent&
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Exponent::operator-=(const Exponent& eSource)
{ return (Exponent&) inherited::operator-=(eSource); }

template <int BitSizeMantissa, int BitSizeExponent>
inline typename BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Exponent&
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Exponent::operator+=(const Exponent& eSource)
{ return (Exponent&) inherited::operator+=(eSource); }

template <int BitSizeMantissa, int BitSizeExponent>
inline typename BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Exponent&
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Exponent::operator--()
{ return (Exponent&) inherited::operator--(); }

template <int BitSizeMantissa, int BitSizeExponent>
inline typename BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Exponent&
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Exponent::operator++()
{ return (Exponent&) inherited::operator++(); }

template <int BitSizeMantissa, int BitSizeExponent>
inline int
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::bitSizeMantissa(const Mantissa&)
{ return BitSizeMantissa; }

template <int BitSizeMantissa, int BitSizeExponent>
inline int
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::bitSizeExponent(const Exponent&)
{ return BitSizeExponent; }

template <int BitSizeMantissa, int BitSizeExponent>
inline const typename BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Exponent&
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::getZeroExponent(const Exponent&)
{ return eZeroExponent; }

template <int BitSizeMantissa, int BitSizeExponent>
inline typename BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Exponent
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::getBasicExponent(const Exponent&, unsigned int uExponent) const
   {  return Exponent(typename Exponent::Basic(), uExponent); }

template <int BitSizeMantissa, int BitSizeExponent>
inline const typename BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Exponent&
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::getOneExponent(const Exponent&)
{ return eOneExponent; }

template <int BitSizeMantissa, int BitSizeExponent>
inline const typename BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Exponent&
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::getMinusOneExponent(const Exponent&)
{ return eMinusOneExponent; }

template <int BitSizeMantissa, int BitSizeExponent>
inline const typename BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Exponent&
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::getInftyExponent(const Exponent&)
{ return eInftyExponent; }

template <int BitSizeMantissa, int BitSizeExponent>
inline const typename BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Exponent&
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::getMaxExponent(const Exponent&)
{ return eZeroExponent; }

template <int BitSizeMantissa, int BitSizeExponent>
inline 
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::ExtendedMantissa::ExtendedMantissa(const ExtendedMantissa& emSource)
 : inherited(emSource) {}

template <int BitSizeMantissa, int BitSizeExponent>
inline 
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::ExtendedMantissa::ExtendedMantissa(const Mantissa& mSource)
{  register int uIndex = 0;
   for (; uIndex <= mSource.lastCellIndex(); ++uIndex)
      inherited::array(uIndex) = mSource[uIndex];
   inherited::bitArray(UBitSizeMantissa) = true;
}

template <int BitSizeMantissa, int BitSizeExponent>
inline int
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::ExtendedMantissa::queryMultResultCellSize(const EnhancedMultResult& emrResult) const
{  return emrResult.queryCellSize(); }

template <int BitSizeMantissa, int BitSizeExponent>
inline 
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::IntConversion()
 : uResult(0U), fUnsigned(false) {}

template <int BitSizeMantissa, int BitSizeExponent>
inline 
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::IntConversion(const IntConversion& iSource)
 : uResult(iSource.uResult), fUnsigned(iSource.fUnsigned) {}

template <int BitSizeMantissa, int BitSizeExponent>
inline typename BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion&
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::setSigned()
{ fUnsigned = false; return *this; }

template <int BitSizeMantissa, int BitSizeExponent>
inline typename BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion&
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::setUnsigned()
{ fUnsigned = true; return *this; }

template <int BitSizeMantissa, int BitSizeExponent>
inline typename BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion&
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::assign(int uValue)
{ assert(!fUnsigned); uResult = uValue; return *this; }

template <int BitSizeMantissa, int BitSizeExponent>
inline typename BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion&
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::assign(unsigned int uValue)
{ assert(fUnsigned); uResult = uValue; return *this; }

template <int BitSizeMantissa, int BitSizeExponent>
inline int
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::querySize() const
{ return sizeof(unsigned int)*8; }

template <int BitSizeMantissa, int BitSizeExponent>
inline int
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::queryMaxDigits() const
{ return fUnsigned ? sizeof(unsigned int)*8 : (sizeof(unsigned int)*8-1); }

template <int BitSizeMantissa, int BitSizeExponent>
inline bool
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::isUnsigned() const
{ return fUnsigned; }

template <int BitSizeMantissa, int BitSizeExponent>
inline bool
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::isSigned() const
{ return !fUnsigned; }

template <int BitSizeMantissa, int BitSizeExponent>
inline int
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::queryInt() const
{ assert(!fUnsigned); return (int) uResult; }

template <int BitSizeMantissa, int BitSizeExponent>
inline unsigned int
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::queryUnsignedInt() const
{ assert(fUnsigned); return uResult; }

template <int BitSizeMantissa, int BitSizeExponent>
inline unsigned int&
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::sresult()
{ return uResult; }

template <int BitSizeMantissa, int BitSizeExponent>
inline void
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::opposite()
{ assert(!fUnsigned); uResult = (~uResult + 1); }

template <int BitSizeMantissa, int BitSizeExponent>
inline bool
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::isPositive() const
{ return fUnsigned || ((int) uResult >= 0); }

template <int BitSizeMantissa, int BitSizeExponent>
inline bool
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::isNegative() const
{ return !fUnsigned && ((int) uResult < 0); }

template <int BitSizeMantissa, int BitSizeExponent>
inline bool
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::isDifferentZero() const
{ return uResult != 0; }

template <int BitSizeMantissa, int BitSizeExponent>
inline bool
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::log_base_2() const
{ return Integer::Details::Access::log_base_2(uResult); }

template <int BitSizeMantissa, int BitSizeExponent>
inline bool
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::hasZero(int uDigits) const
{ return uResult & ~(~0U << uDigits); }

template <int BitSizeMantissa, int BitSizeExponent>
inline bool
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::cbitArray(int uLocalIndex) const
{ return uResult & (1U << uLocalIndex); }

template <int BitSizeMantissa, int BitSizeExponent>
inline typename BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion&
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::operator>>=(int uShift)
{ uResult >>= uShift; return *this; }

template <int BitSizeMantissa, int BitSizeExponent>
inline typename BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion&
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::operator<<=(int uShift)
{ uResult <<= uShift; return *this; }

template <int BitSizeMantissa, int BitSizeExponent>
inline typename BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion&
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::operator&=(const IntConversion& icSource)
{ uResult &= icSource.uResult; return *this; }

template <int BitSizeMantissa, int BitSizeExponent>
inline typename BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion&
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::neg()
{ uResult = ~uResult; return *this; }

template <int BitSizeMantissa, int BitSizeExponent>
inline typename BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion&
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::inc()
{ ++uResult; return *this; }

template <int BitSizeMantissa, int BitSizeExponent>
inline typename BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion&
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::operator=(const IntConversion& icSource)
{  uResult = icSource.uResult;
   fUnsigned = icSource.fUnsigned;
   return *this;
}

template <int BitSizeMantissa, int BitSizeExponent>
inline typename BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion&
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::operator=(int uValue)
{  if (fUnsigned) {
      assert(uValue >= 0);
      uResult = (unsigned int) uValue;
   }
   else
      uResult = (unsigned int) uValue;
   return *this;
}

template <int BitSizeMantissa, int BitSizeExponent>
inline
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::BitArray::BitArray(IntConversion& icThis, int uIndexSource)
: puResult(&icThis.uResult), uIndex(uIndexSource) {}

template <int BitSizeMantissa, int BitSizeExponent>
inline
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::BitArray::BitArray(const BitArray& baSource)
 : puResult(baSource.puResult), uIndex(baSource.uIndex) {}

template <int BitSizeMantissa, int BitSizeExponent>
inline typename BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::BitArray&
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::BitArray::operator=(const BitArray& baSource)
{  puResult = baSource.puResult;
   uIndex = baSource.uIndex;
   return *this;
}

template <int BitSizeMantissa, int BitSizeExponent>
inline typename BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::BitArray&
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::BitArray::operator=(bool fValue)
{  if (fValue)
      *puResult |= (1U << (uIndex%(sizeof(unsigned int)*8)));
   else
      *puResult &= ~(1U << (uIndex%(sizeof(unsigned int)*8)));
   return *this;
}

template <int BitSizeMantissa, int BitSizeExponent>
inline
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::BitArray::operator bool() const
{  return (*puResult & (1U << (uIndex%(sizeof(unsigned int)*8))))
      ? true : false;
}

template <int BitSizeMantissa, int BitSizeExponent>
inline typename BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::BitArray
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::bitArray(int uIndex)
{  return BitArray(*this, uIndex); }

template <int BitSizeMantissa, int BitSizeExponent>
inline void
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::setBitArray(int uIndex, bool fValue)
{  if (fValue)
      uResult |= (1U << (uIndex%(sizeof(unsigned int)*8)));
   else
      uResult &= ~(1U << (uIndex%(sizeof(unsigned int)*8)));
}

template <int BitSizeMantissa, int BitSizeExponent>
inline void
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::setTrueBitArray(int uIndex)
{  uResult |= (1U << (uIndex%(sizeof(unsigned int)*8))); }

template <int BitSizeMantissa, int BitSizeExponent>
inline void
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::setFalseBitArray(int uIndex)
{  uResult &= ~(1U << (uIndex%(sizeof(unsigned int)*8))); }

template <int BitSizeMantissa, int BitSizeExponent>
inline void
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::setMin()
{ uResult = fUnsigned ? 0U : (1U << (8*sizeof(unsigned int)-1)); }

template <int BitSizeMantissa, int BitSizeExponent>
inline void
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::setMax()
{ uResult = fUnsigned ? ~0U : ~(1U << (8*sizeof(unsigned int)-1)); }

template <int BitSizeMantissa, int BitSizeExponent>
inline unsigned int&
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::operator[](int uIndex)
{ assert(uIndex == 0); return uResult; }

template <int BitSizeMantissa, int BitSizeExponent>
inline const unsigned int&
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::IntConversion::operator[](int uIndex) const
{ assert(uIndex == 0); return uResult; }

#endif // DefineGCC_Inline_Double_BuiltDoubleTraits

#ifdef DefineGCC_Inline_Double_TBuiltDouble

template <class TypeTraits>
inline
TBuiltDouble<TypeTraits>::MultParameters::MultParameters(StatusAndControlFlags& scfFlagsSource)
:  oOperation(OPlusPlus), scfFlags(scfFlagsSource), pbdAdd(NULL) {}

template <class TypeTraits>
inline
TBuiltDouble<TypeTraits>::MultParameters::MultParameters(const MultParameters& mpSource)
:  oOperation(mpSource.oOperation), scfFlags(mpSource.scfFlags), pbdAdd(mpSource.pbdAdd) {}

template <class TypeTraits>
inline typename TBuiltDouble<TypeTraits>::MultParameters&
TBuiltDouble<TypeTraits>::MultParameters::setAdd(const thisType& bdAdd)
{ pbdAdd = &bdAdd; return *this; }

template <class TypeTraits>
inline void
TBuiltDouble<TypeTraits>::MultParameters::clear()
{ pbdAdd = NULL; oOperation = OPlusPlus; }

template <class TypeTraits>
inline typename TBuiltDouble<TypeTraits>::MultParameters&
TBuiltDouble<TypeTraits>::MultParameters::setPositiveMult()
{ oOperation = (Operation) ((int) oOperation & (int) OPlusMinus); return *this; }

template <class TypeTraits>
inline typename TBuiltDouble<TypeTraits>::MultParameters&
TBuiltDouble<TypeTraits>::MultParameters::setNegativeMult()
{  oOperation = (Operation) ((int) oOperation | (int) OMinusPlus); return *this; }

template <class TypeTraits>
inline typename TBuiltDouble<TypeTraits>::MultParameters&
TBuiltDouble<TypeTraits>::MultParameters::setPositiveAdditive()
{  oOperation = (Operation) ((int) oOperation & (int) OMinusPlus); return *this; }

template <class TypeTraits>
inline typename TBuiltDouble<TypeTraits>::MultParameters&
TBuiltDouble<TypeTraits>::MultParameters::setNegativeAdditive()
{ oOperation = (Operation) ((int) oOperation | (int) OPlusMinus); return *this; }

template <class TypeTraits>
inline bool
TBuiltDouble<TypeTraits>::MultParameters::hasAdd() const
{ return pbdAdd != NULL; }

template <class TypeTraits>
inline typename TBuiltDouble<TypeTraits>::StatusAndControlFlags&
TBuiltDouble<TypeTraits>::MultParameters::readParams() const
{ return scfFlags; }

template <class TypeTraits>
inline const TBuiltDouble<TypeTraits>&
TBuiltDouble<TypeTraits>::MultParameters::queryAddSource() const
{ assert(pbdAdd); return *pbdAdd; }

template <class TypeTraits>
inline bool
TBuiltDouble<TypeTraits>::MultParameters::isPositiveAdditive() const
{ return !(oOperation & OPlusMinus); }

template <class TypeTraits>
inline bool
TBuiltDouble<TypeTraits>::MultParameters::isNegativeAdditive() const
{ return (oOperation & OPlusMinus); }

template <class TypeTraits>
inline bool
TBuiltDouble<TypeTraits>::MultParameters::isPositiveMult() const
{ return !(oOperation & OMinusPlus); }

template <class TypeTraits>
inline bool
TBuiltDouble<TypeTraits>::MultParameters::isNegativeMult() const
{ return (oOperation & OMinusPlus); }

template <class TypeTraits>
inline bool
TBuiltDouble<TypeTraits>::MultParameters::hasSameSign(bool fNegative) const
{  assert(pbdAdd);
   register bool fResult = pbdAdd->fNegative == fNegative;
   return ((oOperation == OPlusPlus) || (oOperation == OMinusMinus)) ? fResult : !fResult;
}

template <class TypeTraits>
inline int
TBuiltDouble<TypeTraits>::bitSizeMantissa() const
{ return TypeTraits::bitSizeMantissa(biMantissa); }

template <class TypeTraits>
inline int
TBuiltDouble<TypeTraits>::bitSizeExponent() const
{ return TypeTraits::bitSizeExponent(biExponent); }

template <class TypeTraits>
inline
TBuiltDouble<TypeTraits>::TBuiltDouble()
#ifndef __BORLANDC__
 : biMantissa(), biExponent(typename TypeTraits::Exponent::Min()), fNegative(false) {}
#else
 : biMantissa(), biExponent(TypeTraits::Exponent::Min()), fNegative(false) {}
#endif

template <class TypeTraits>
inline
TBuiltDouble<TypeTraits>::TBuiltDouble(const IntConversion& icValue, StatusAndControlFlags& scfFlags)
:  biMantissa(), biExponent(), fNegative(false)
{  setInteger(icValue, scfFlags); }

template <class TypeTraits>
inline
TBuiltDouble<TypeTraits>::TBuiltDouble(const FloatConversion& fcValue, StatusAndControlFlags& scfFlags)
:  biMantissa(), biExponent(), fNegative(false)
{  setFloat(fcValue, scfFlags); }

template <class TypeTraits>
inline
TBuiltDouble<TypeTraits>::TBuiltDouble(const thisType& bdSource)
:  biMantissa(bdSource.biMantissa), biExponent(bdSource.biExponent),
   fNegative(bdSource.fNegative) {}

template <class TypeTraits>
inline TBuiltDouble<TypeTraits>&
TBuiltDouble<TypeTraits>::operator=(const thisType& bdSource)
{  biMantissa = bdSource.biMantissa;
   biExponent = bdSource.biExponent;
   fNegative = bdSource.fNegative;
   return *this;
}

template <class TypeTraits>
inline void
TBuiltDouble<TypeTraits>::setChunk(void* pChunk)
{ setChunk(pChunk, !Details::DTDoubleElement::Access::isBigEndian()); }

template <class TypeTraits>
inline void
TBuiltDouble<TypeTraits>::fillChunk(void* pChunk)
{ fillChunk(pChunk, !Details::DTDoubleElement::Access::isBigEndian()); }

template <class TypeTraits>
inline typename TBuiltDouble<TypeTraits>::ComparisonResult
TBuiltDouble<TypeTraits>::compare(const thisType& bdSource) const
{  ComparisonResult crResult = CRNaN;
   if (!isNaN() && !bdSource.isNaN()) {
      if (fNegative != bdSource.fNegative) {
         crResult = fNegative ? CRLess : CRGreater;
         if (isZero() && bdSource.isZero())
            crResult = CREqual;
      }
      else {
         register typename Exponent::ComparisonResult crExponentResult = biExponent.compare(bdSource.biExponent);
         crResult = (crExponentResult != Exponent::CREqual)
            ? ((ComparisonResult) (1+crExponentResult))
            : ((ComparisonResult) (1+biMantissa.compare(bdSource.biMantissa)));
         if (fNegative)
            crResult = (ComparisonResult) (CRGreater+1-crResult);
      };
   };
   return crResult;
}

template <class TypeTraits>
inline typename TBuiltDouble<TypeTraits>::ComparisonResult
TBuiltDouble<TypeTraits>::compareValue(const thisType& bdSource) const
{  register ComparisonResult crResult;
   if (fNegative != bdSource.fNegative) {
      if (!isZero() || !bdSource.isZero())
         crResult = fNegative ? CRLess : CRGreater;
      else
         crResult = CREqual;
   }
   else {
      register typename Exponent::ComparisonResult crExponentResult = biExponent.compare(bdSource.biExponent);
      crResult = (crExponentResult != Exponent::CREqual)
         ? ((ComparisonResult) (1+crExponentResult))
         : ((ComparisonResult) (1+biMantissa.compare(bdSource.biMantissa)));
      if (fNegative)
         crResult = (ComparisonResult) (CRGreater+1-crResult);
   };
   return crResult;
}

template <class TypeTraits>
inline bool
TBuiltDouble<TypeTraits>::operator==(const thisType& bdSource) const
{ return compare(bdSource) == CREqual; }

template <class TypeTraits>
inline bool
TBuiltDouble<TypeTraits>::operator!=(const thisType& bdSource) const
{  register ComparisonResult crResult = compare(bdSource);
   return (crResult == CRLess) || (crResult == CRGreater);
}

template <class TypeTraits>
inline bool
TBuiltDouble<TypeTraits>::operator<(const thisType& bdSource) const
{ return compare(bdSource) == CRLess; }

template <class TypeTraits>
inline bool
TBuiltDouble<TypeTraits>::operator>(const thisType& bdSource) const
{ return compare(bdSource) == CRGreater; }

template <class TypeTraits>
inline bool
TBuiltDouble<TypeTraits>::operator<=(const thisType& bdSource) const
{  register ComparisonResult crResult = compare(bdSource);
   return (crResult == CRLess) || (crResult == CREqual);
}

template <class TypeTraits>
inline bool
TBuiltDouble<TypeTraits>::operator>=(const thisType& bdSource) const
{  register ComparisonResult crResult = compare(bdSource);
   return (crResult == CRGreater) || (crResult == CREqual);
}

template <class TypeTraits>
inline typename TypeTraits::Exponent
TBuiltDouble<TypeTraits>::getZeroExponent() const
{ return TypeTraits::getZeroExponent(biExponent); }

template <class TypeTraits>
inline typename TypeTraits::Exponent
TBuiltDouble<TypeTraits>::getBasicExponent(unsigned int uExponent) const
{ return TypeTraits::getBasicExponent(biExponent, uExponent); }

template <class TypeTraits>
inline typename TypeTraits::Exponent
TBuiltDouble<TypeTraits>::getOneExponent() const
{ return TypeTraits::getOneExponent(biExponent); }

template <class TypeTraits>
inline typename TypeTraits::Exponent
TBuiltDouble<TypeTraits>::getMinusOneExponent() const
{ return TypeTraits::getMinusOneExponent(biExponent); }

template <class TypeTraits>
inline typename TypeTraits::Exponent
TBuiltDouble<TypeTraits>::getInftyExponent() const
{ return TypeTraits::getInftyExponent(biExponent); }

template <class TypeTraits>
inline typename TypeTraits::Exponent
TBuiltDouble<TypeTraits>::getMaxExponent() const
{ return TypeTraits::getMaxExponent(biExponent); }

template <class TypeTraits>
inline const typename TypeTraits::Exponent&
TBuiltDouble<TypeTraits>::queryBasicExponent() const
{ return biExponent; }

template <class TypeTraits>
inline typename TypeTraits::Exponent&
TBuiltDouble<TypeTraits>::querySBasicExponent()
{ return biExponent; }

template <class TypeTraits>
inline bool
TBuiltDouble<TypeTraits>::hasPositiveExponent() const
{ return biExponent > getZeroExponent(); }

template <class TypeTraits>
inline bool
TBuiltDouble<TypeTraits>::hasPositiveOrNullExponent() const
{ return biExponent >= getZeroExponent(); }

template <class TypeTraits>
inline bool
TBuiltDouble<TypeTraits>::hasNullExponent() const
{ return biExponent == getZeroExponent(); }

template <class TypeTraits>
inline bool
TBuiltDouble<TypeTraits>::hasNegativeExponent() const
{ return biExponent < getZeroExponent(); }

template <class TypeTraits>
inline bool
TBuiltDouble<TypeTraits>::hasNegativeOrNullExponent() const
{ return biExponent < getZeroExponent(); }

template <class TypeTraits>
inline typename TypeTraits::Exponent
TBuiltDouble<TypeTraits>::queryExponent() const
{  typename TypeTraits::Exponent biResult = getZeroExponent();
   if (biExponent >= biResult) {
      biResult = biExponent;
      biResult -= getZeroExponent();
   }
   else
      biResult -= biExponent;
   return biResult;
}

template <class TypeTraits>
inline void
TBuiltDouble<TypeTraits>::setBasicExponent(const typename TypeTraits::Exponent& biExponentSource)
{ biExponent = biExponentSource; }

template <class TypeTraits>
inline void
TBuiltDouble<TypeTraits>::setInfty()
{ fNegative = false; biExponent = getInftyExponent(); biMantissa = 0U; }

template <class TypeTraits>
inline void
TBuiltDouble<TypeTraits>::setZero()
{ fNegative = false; biExponent = 0U; biMantissa = 0U; }

template <class TypeTraits>
inline void
TBuiltDouble<TypeTraits>::setOne()
{ fNegative = false; biExponent = getZeroExponent(); biMantissa = 0U; }

template <class TypeTraits>
inline void
TBuiltDouble<TypeTraits>::setExponent(const typename TypeTraits::Exponent& biExponentSource, bool fNegative)
{  if (!fNegative) {
      biExponent = biExponentSource;
      biExponent += getZeroExponent();
   }
   else {
      biExponent = getZeroExponent();
      biExponent -= biExponentSource;
   };
}

template <class TypeTraits>
inline bool
TBuiltDouble<TypeTraits>::isPositive() const
{  return !fNegative; }

template <class TypeTraits>
inline bool
TBuiltDouble<TypeTraits>::isNegative() const
{  return fNegative; }

template <class TypeTraits>
inline void
TBuiltDouble<TypeTraits>::setSign(bool fPositive)
{  fNegative = !fPositive; }

template <class TypeTraits>
inline TBuiltDouble<TypeTraits>&
TBuiltDouble<TypeTraits>::opposite()
{ fNegative = !fNegative; return *this; }

template <class TypeTraits>
inline void
TBuiltDouble<TypeTraits>::setPositive()
{ fNegative = false; }

template <class TypeTraits>
inline void
TBuiltDouble<TypeTraits>::setNegative(bool fNegativeSource)
{ fNegative = fNegativeSource; }

template <class TypeTraits>
inline bool
TBuiltDouble<TypeTraits>::isInfty() const
{  return (biExponent == getInftyExponent()) && biMantissa.isZero(); }

template <class TypeTraits>
inline bool
TBuiltDouble<TypeTraits>::isNaN() const
{  return (biExponent == getInftyExponent()) && !biMantissa.isZero(); }

template <class TypeTraits>
inline bool
TBuiltDouble<TypeTraits>::isSNaN() const
{  return (biExponent == getInftyExponent()) && !biMantissa.isZero()
      && !biMantissa.cbitArray(bitSizeMantissa()-1);
}

template <class TypeTraits>
inline bool
TBuiltDouble<TypeTraits>::isQNaN() const
{  return (biExponent == getInftyExponent())
      && biMantissa.cbitArray(bitSizeMantissa()-1);
}

template <class TypeTraits>
inline bool
TBuiltDouble<TypeTraits>::isNormalized() const
{  return (!biExponent.isZero()) && (biExponent != getInftyExponent()); }

template <class TypeTraits>
inline bool
TBuiltDouble<TypeTraits>::isDenormalized() const
{ return biExponent.isZero() && !biMantissa.isZero(); }

template <class TypeTraits>
inline bool
TBuiltDouble<TypeTraits>::isZero() const
{ return biExponent.isZero() && biMantissa.isZero(); }

template <class TypeTraits>
inline void
TBuiltDouble<TypeTraits>::setToEpsilon() {
   if (biExponent > bitSizeMantissa()) {
      Exponent biSub(biExponent);
      biSub.clear();
      biSub[0] = bitSizeMantissa();
      biExponent.sub(biSub);
      biMantissa = 0U;
   }
   else if (biExponent.isZero())
      biMantissa = 1U;
   else {
      biMantissa = 0U;
      biMantissa.bitArray(biExponent.queryValue()-1) = true;
      biExponent = 0U;
   };
}

template <class TypeTraits>
inline TBuiltDouble<TypeTraits>
TBuiltDouble<TypeTraits>::queryEpsilon() const
{  thisType dResult = *this;
   dResult.setToEpsilon();
   return dResult;
}

template <class TypeTraits>
inline const typename TBuiltDouble<TypeTraits>::Mantissa&
TBuiltDouble<TypeTraits>::queryMantissa() const
{ return biMantissa; }

template <class TypeTraits>
inline typename TBuiltDouble<TypeTraits>::Mantissa&
TBuiltDouble<TypeTraits>::querySMantissa()
{ return biMantissa; }

template <class TypeTraits>
inline TBuiltDouble<TypeTraits>&
TBuiltDouble<TypeTraits>::multAssign(const thisType& bdSource, StatusAndControlFlags& scfFlags)
{  return multAssign(bdSource, MultParameters(scfFlags)); }

template <class TypeTraits>
inline TBuiltDouble<TypeTraits>&
TBuiltDouble<TypeTraits>::multAndAddAssign(const thisType& bdMult, const thisType& bdAdd, StatusAndControlFlags& scfFlags)
{  return multAssign(bdMult, MultParameters(scfFlags).setAdd(bdAdd)); }

template <class TypeTraits>
inline TBuiltDouble<TypeTraits>&
TBuiltDouble<TypeTraits>::multAndSubAssign(const thisType& bdMult, const thisType& bdAdd, StatusAndControlFlags& scfFlags)
{  return multAssign(bdMult, MultParameters(scfFlags).setAdd(bdAdd).setNegativeAdditive()); }

template <class TypeTraits>
inline TBuiltDouble<TypeTraits>&
TBuiltDouble<TypeTraits>::negativeMultAndSubAssign(const thisType& bdMult, const thisType& bdAdd, StatusAndControlFlags& scfFlags)
{  return multAssign(bdMult, MultParameters(scfFlags).setAdd(bdAdd).setNegativeMult()); }

template <class TypeTraits>
inline TBuiltDouble<TypeTraits>&
TBuiltDouble<TypeTraits>::negativeMultAndAddAssign(const thisType& bdMult, const thisType& bdAdd, StatusAndControlFlags& scfFlags)
{  return multAssign(bdMult, MultParameters(scfFlags).setAdd(bdAdd).setNegativeMult().setNegativeAdditive()); }

template <class TypeTraits>
inline TBuiltDouble<TypeTraits>&
TBuiltDouble<TypeTraits>::operator+=(const thisType& bdSource)
{  return plusAssign(bdSource, StatusAndControlFlags().setNearestRound()); }

template <class TypeTraits>
inline TBuiltDouble<TypeTraits>
TBuiltDouble<TypeTraits>::operator+(const thisType& bdSource) const
{  thisType bdThis = *this;
   bdThis.plusAssign(bdSource, StatusAndControlFlags().setNearestRound());
   return bdThis;
}

template <class TypeTraits>
inline TBuiltDouble<TypeTraits>&
TBuiltDouble<TypeTraits>::operator-=(const thisType& bdSource)
{  return minusAssign(bdSource, StatusAndControlFlags().setNearestRound()); }

template <class TypeTraits>
inline TBuiltDouble<TypeTraits>
TBuiltDouble<TypeTraits>::operator-(const thisType& bdSource) const
{  thisType bdThis = *this;
   bdThis.minusAssign(bdSource, StatusAndControlFlags().setNearestRound());
   return bdThis;
}

template <class TypeTraits>
inline TBuiltDouble<TypeTraits>&
TBuiltDouble<TypeTraits>::operator*=(const thisType& bdSource)
{  return multAssign(bdSource, StatusAndControlFlags().setNearestRound()); }

template <class TypeTraits>
inline TBuiltDouble<TypeTraits>
TBuiltDouble<TypeTraits>::operator*(const thisType& bdSource) const
{  thisType bdThis = *this;
   bdThis.multAssign(bdSource, StatusAndControlFlags().setNearestRound());
   return bdThis;
}

template <class TypeTraits>
inline TBuiltDouble<TypeTraits>&
TBuiltDouble<TypeTraits>::operator/=(const thisType& bdSource)
{  return divAssign(bdSource, StatusAndControlFlags().setNearestRound()); }

template <class TypeTraits>
inline TBuiltDouble<TypeTraits>
TBuiltDouble<TypeTraits>::operator/(const thisType& bdSource) const
{  thisType bdThis = *this;
   bdThis.divAssign(bdSource, StatusAndControlFlags().setNearestRound());
   return bdThis;
}

template <class TypeTraits>
inline void
TBuiltDouble<TypeTraits>::clear()
{  biMantissa.clear();
   biExponent.clear();
   fNegative = false;
}

template <class TypeTraits>
inline void
TBuiltDouble<TypeTraits>::swap(thisType& bdSource)
{  biMantissa.swap(bdSource.biMantissa);
   biExponent.swap(bdSource.biExponent);
   register bool fTemp = fNegative;
   fNegative = bdSource.fNegative;
   bdSource.fNegative = fTemp;
}

template <class TypeTraits>
inline std::ostream&
operator<<(std::ostream& osOut, const TBuiltDouble<TypeTraits>& bdDouble) {
   typename TBuiltDouble<TypeTraits>::WriteParameters wpParams;
   bdDouble.write(osOut, wpParams);
   return osOut;
}

template <class TypeTraits>
inline std::istream&
operator<<(std::istream& isIn, TBuiltDouble<TypeTraits>& bdDouble) {
   typename TBuiltDouble<TypeTraits>::StatusAndControlFlags scfFlags;
   bdDouble.read(isIn, scfFlags);
   return isIn;
}

#endif // DefineGCC_Inline_Double_TBuiltDouble

#ifdef DefineGCC_Inline_Double_TFloatingBase

inline
Errors::Errors() : uErrors(0) {}

inline
Errors::Errors(const Errors& eSource) : uErrors(eSource.uErrors) {}

inline Errors&
Errors::operator=(const Errors& eSource)
   { uErrors = eSource.uErrors; return *this; }

#define DefineIsError(TypeError) \
inline bool Errors::is##TypeError() const { return (bool) (uErrors & (1U << T##TypeError)); }
   
   DefineIsError(PositiveOverflow)
   DefineIsError(NegativeOverflow)
   DefineIsError(QNaN)
   DefineIsError(SNaN)
   DefineIsError(PositiveUnderflow)
   DefineIsError(NegativeUnderflow)
#undef DefineIsError

inline bool
Errors::isNaN() const { return (bool) (uErrors & (3U << TQNaN)); }

#define DefineSetError(TypeError) \
inline Errors& Errors::set##TypeError() { uErrors |= (1U << T##TypeError); return *this; }
   
   DefineSetError(PositiveOverflow)
   DefineSetError(NegativeOverflow)
   DefineSetError(QNaN)
   DefineSetError(SNaN)
   DefineSetError(PositiveUnderflow)
   DefineSetError(NegativeUnderflow)
#undef DefineSetError

#endif // DefineGCC_Inline_Double_TFloatingBase

