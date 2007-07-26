/***************************************************************************
                Floating_gccopt.h  -  Template for various types of floating point computations
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

#ifndef Numerics_Double_FloatingH
#define Numerics_Double_FloatingH

#ifdef WIN32
#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN    4321
#define LITTLE_ENDIAN __LITTLE_ENDIAN
#define BIG_ENDIAN __BIG_ENDIAN
#define ENDIAN LITTLE_ENDIAN
#else
#include <endian.h>
#endif

#include <iostream>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>
#if defined(__GNUC__)
#ifdef __SUN__
#include <ieeefp.h>
#else
#ifndef __CYGWIN__
#include <fenv.h>
#endif
#endif
#endif
#include "lib/simfloat/integer_gccopt.hh"

namespace unisim {
namespace lib {
namespace simfloat {

namespace Numerics { namespace Double {

namespace Details { namespace DTDoubleElement {

class Access : public Integer::Details::Access {
  public:
   class StatusAndControlFlags {
     protected:
      enum RoundMode { RMNearest, RMLowest, RMHighest, RMZero };
      enum Approximation { AExact, ADownApproximate, AUpApproximate };
      enum ReadResult { RRTotal, RRPartial };
      enum QNaNResult
         {  QNNRUndefined, QNNRInftyMinusInfty, QNNRInftyOnInfty, QNNRZeroOnZero,
            QNNRInftyMultZero
         };
      enum FlowException { FENoException, FEOverflow, FEUnderflow, FEEnd };
      
     private:
      bool fAvoidInfty;
      bool fKeepNaNSign;
      bool fProduceDivNaNPositive;
      bool fRoundToEven;
      bool fPositiveZeroMAdd;
      bool fUpApproximateInfty;
      bool fUpApproximateInversionForNear;
      bool fChooseNaNAddBeforeMult;
      bool fConvertNaNNegative;
      bool fRefuseMinusZero;

      RoundMode rmRound;
      bool fKeepSignalingConversion;

      Approximation aApproximation;
      ReadResult rrReadResult;
      bool fEffectiveRoundToEven;
      bool fSNaNOperand;
      QNaNResult qnrQNaNResult;
      FlowException feExcept;
      bool fDivisionByZero;

     public:
      inline StatusAndControlFlags() GCC_INLINE;
      inline StatusAndControlFlags(const StatusAndControlFlags& rpSource) GCC_INLINE;
      inline StatusAndControlFlags& operator=(const StatusAndControlFlags& rpSource) GCC_INLINE;
         
      // Control parameters
      inline StatusAndControlFlags& setRoundToEven() GCC_INLINE;
      inline StatusAndControlFlags& clearRoundToEven() GCC_INLINE; 
      inline StatusAndControlFlags& setPositiveZeroMAdd() GCC_INLINE; 
      inline StatusAndControlFlags& avoidInfty() GCC_INLINE;     
      inline StatusAndControlFlags& setKeepNaNSign() GCC_INLINE; 
      inline StatusAndControlFlags& setProduceDivNaNPositive() GCC_INLINE; 
      inline StatusAndControlFlags& setUpApproximateInfty() GCC_INLINE; 
      inline StatusAndControlFlags& setUpApproximateInversionForNear() GCC_INLINE; 
      inline StatusAndControlFlags& setChooseNaNAddBeforeMult() GCC_INLINE; 
      inline StatusAndControlFlags& setConvertNaNNegative() GCC_INLINE; 
      inline StatusAndControlFlags& setAcceptMinusZero() GCC_INLINE; 
      inline StatusAndControlFlags& setRefuseMinusZero() GCC_INLINE; 

      inline bool isRoundToEven() const GCC_INLINE; 
      inline bool isPositiveZeroMAdd() GCC_INLINE; 
      inline bool isInftyAvoided() const GCC_INLINE; 
      inline bool doesAvoidInfty(bool fNegative) const GCC_INLINE;
      inline bool keepNaNSign() const GCC_INLINE; 
      inline bool produceDivNaNPositive() const GCC_INLINE; 
      inline bool upApproximateInfty() const GCC_INLINE; 
      inline bool upApproximateInversionForNear() const GCC_INLINE; 
      inline bool chooseNaNAddBeforeMult() const GCC_INLINE; 
      inline bool isConvertNaNNegative() const GCC_INLINE; 
      inline bool acceptMinusZero() const GCC_INLINE; 

      // status/intern control : internal write
      inline StatusAndControlFlags& setNearestRound() GCC_INLINE;   
      inline StatusAndControlFlags& setHighestRound() GCC_INLINE;   
      inline StatusAndControlFlags& setLowestRound() GCC_INLINE;    
      inline StatusAndControlFlags& setZeroRound() GCC_INLINE;      

      inline bool isLowestRound() const GCC_INLINE; 
      inline bool isNearestRound() const GCC_INLINE; 
      inline bool isHighestRound() const GCC_INLINE; 
      inline bool isZeroRound() const GCC_INLINE; 

      inline StatusAndControlFlags& setKeepSignalingConversion() GCC_INLINE; 
      inline StatusAndControlFlags& clearKeepSignalingConversion() GCC_INLINE; 
      inline bool keepSignalingConversion() const GCC_INLINE; 

      // status
      inline bool isApproximate() const GCC_INLINE; 
      inline bool isDownApproximate() const GCC_INLINE; 
      inline bool isUpApproximate() const GCC_INLINE; 
      inline void setDownApproximate() GCC_INLINE; 
      inline void setUpApproximate() GCC_INLINE; 
      inline void clearApproximate() GCC_INLINE; 
      enum Direction { Down, Up };
      inline void setApproximate(Direction dDirection) GCC_INLINE;
         
      inline bool isApproximate(Direction dDirection) const GCC_INLINE;
      inline bool hasSameApproximation(const StatusAndControlFlags& rpSource) const GCC_INLINE;
      inline bool hasIncrementFraction(bool fNegative) const GCC_INLINE;

      inline void setEffectiveRoundToEven() GCC_INLINE; 
      inline void clearEffectiveRoundToEven() GCC_INLINE; 
      inline bool hasEffectiveRoundToEven() GCC_INLINE; 

      inline void setPartialRead() GCC_INLINE; 
      inline void setTotalRead() GCC_INLINE; 
      inline bool isPartialRead() const GCC_INLINE; 
      inline bool isTotalRead() const GCC_INLINE; 
      inline bool hasPartialRead() const GCC_INLINE; 

      inline void setSNaNOperand() GCC_INLINE; 
      inline bool hasSNaNOperand() const GCC_INLINE; 
      
      inline void setInftyMinusInfty() GCC_INLINE; 
      inline void setInftyOnInfty() GCC_INLINE; 
      inline void setZeroOnZero() GCC_INLINE; 
      inline void setInftyMultZero() GCC_INLINE; 
      inline bool hasQNaNResult() const GCC_INLINE; 
      inline bool isInftyMinusInfty() const GCC_INLINE; 
      inline bool isInftyOnInfty() const GCC_INLINE; 
      inline bool isZeroOnZero() const GCC_INLINE; 
      inline bool isInftyMultZero() const GCC_INLINE; 

      inline void clear() GCC_INLINE;

      inline bool isDivisionByZero() const GCC_INLINE; 
      inline void setDivisionByZero() GCC_INLINE; 
      inline bool hasFlowException() const GCC_INLINE; 
      inline void clearFlowException() GCC_INLINE; 
      inline void setOverflow() GCC_INLINE; 
      inline void setUnderflow() GCC_INLINE; 
      inline bool isOverflow() const GCC_INLINE; 
      inline bool isUnderflow() const GCC_INLINE; 
      inline void clearUnderflow() GCC_INLINE; 
   };
   class WriteParameters {
     private:
      enum Type { TDecimal, TBinary };
      Type tType;
         
     public:
      inline WriteParameters() GCC_INLINE;
      inline WriteParameters& setDecimal() GCC_INLINE; 
      inline WriteParameters& setBinary() GCC_INLINE; 
      inline bool isDecimal() const GCC_INLINE; 
      inline bool isBinary() const GCC_INLINE; 
   };

   class Carry {
     private:
      bool fCarry;

     public:
      inline Carry() GCC_INLINE;
      inline Carry(const Carry& cSource) GCC_INLINE;
      inline bool& carry() GCC_INLINE; 
      inline const bool& carry() const GCC_INLINE; 
      inline bool hasCarry() const GCC_INLINE; 
   };
   
   template <class TypeMantissa, class TypeStatusAndControlFlags>
   static Carry trightShift(TypeMantissa& mMantissa, int uShift,
         unsigned int uValue, TypeStatusAndControlFlags& scfFlags, bool fNegative, int uBitSizeMantissa);
};

inline
Access::StatusAndControlFlags::StatusAndControlFlags()
   :  fAvoidInfty(false), fKeepNaNSign(false), fProduceDivNaNPositive(false),
      fRoundToEven(false), fPositiveZeroMAdd(false), fUpApproximateInfty(false),
      fUpApproximateInversionForNear(false), fChooseNaNAddBeforeMult(false),
      fConvertNaNNegative(false), fRefuseMinusZero(false), rmRound(RMNearest),
      fKeepSignalingConversion(false), aApproximation(AExact), rrReadResult(RRTotal),
      fEffectiveRoundToEven(false), fSNaNOperand(false), qnrQNaNResult(QNNRUndefined),
      feExcept(FENoException), fDivisionByZero(false) {}

inline
Access::StatusAndControlFlags::StatusAndControlFlags(const StatusAndControlFlags& rpSource)
   :  fAvoidInfty(rpSource.fAvoidInfty), fKeepNaNSign(rpSource.fKeepNaNSign), fProduceDivNaNPositive(rpSource.fProduceDivNaNPositive),
      fRoundToEven(rpSource.fRoundToEven), fPositiveZeroMAdd(rpSource.fPositiveZeroMAdd),
      fUpApproximateInfty(rpSource.fUpApproximateInfty),
      fUpApproximateInversionForNear(rpSource.fUpApproximateInversionForNear),
      fChooseNaNAddBeforeMult(rpSource.fChooseNaNAddBeforeMult),
      fConvertNaNNegative(rpSource.fConvertNaNNegative), fRefuseMinusZero(rpSource.fRefuseMinusZero),
      rmRound(rpSource.rmRound), fKeepSignalingConversion(rpSource.fKeepSignalingConversion),
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

} // end of namespace DTDoubleElement

namespace DBuiltDoubleTraits {

class Access {
  public:
   template <int USizeMantissa, int USizeExponent>
   class TFloatConversion {
     public:
      typedef Integer::TBigCellInt<Integer::Details::TCellIntegerTraits<USizeMantissa> > Mantissa;
      typedef Integer::TBigCellInt<Integer::Details::TCellIntegerTraits<USizeExponent> > Exponent;

     private:
      typedef TFloatConversion<USizeMantissa, USizeExponent> thisType;

      Mantissa bciMantissa;
      int uBitsMantissa;
      Exponent bciExponent;
      int uBitsExponent;
      bool fNegative;

     public:
      inline TFloatConversion() GCC_INLINE;
      inline TFloatConversion(const thisType& fcSource) GCC_INLINE;

      inline bool isPositive() const GCC_INLINE; 
      inline bool isNegative() const GCC_INLINE; 
      inline bool isInftyExponent() const GCC_INLINE;
      inline bool isZeroExponent() const GCC_INLINE; 
      inline bool isZeroMantissa() const GCC_INLINE; 
      inline const int& querySizeMantissa() const GCC_INLINE; 
      inline const int& querySizeExponent() const GCC_INLINE; 

      inline thisType& setSizeMantissa(int uSize) GCC_INLINE; 
      inline thisType& setSizeExponent(int uSize) GCC_INLINE; 

      inline void setPositive(bool fPositive) GCC_INLINE; 
      inline void setNegative(bool fNegativeSource) GCC_INLINE; 
      inline const Mantissa& mantissa() const GCC_INLINE; 
      inline Mantissa& mantissa() GCC_INLINE; 
      inline const Exponent& exponent() const GCC_INLINE; 
      inline Exponent& exponent() GCC_INLINE; 
   };
};

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

}} // end of namespace Details::DBuiltDoubleTraits

template <int BitSizeMantissa, int BitSizeExponent>
class BuiltDoubleTraits : public Details::DBuiltDoubleTraits::Access {
  private:
   typedef Details::DBuiltDoubleTraits::Access inherited;

  public:
   static const int UBitSizeMantissa = BitSizeMantissa;
   static const int UBitSizeExponent = BitSizeExponent;
   inline static int bitSizeMantissa() GCC_INLINE; 
   inline static int bitSizeExponent() GCC_INLINE; 

   class ExtendedMantissa;
   class Mantissa : public Integer::TBigInt<Integer::Details::TIntegerTraits<BitSizeMantissa> > {
     private:
      typedef Integer::TBigInt<Integer::Details::TIntegerTraits<BitSizeMantissa> > inherited;

     public:
      inline Mantissa() GCC_INLINE; 
      inline Mantissa(unsigned int uValue) GCC_INLINE;
      inline Mantissa(const Mantissa& mSource) GCC_INLINE;
      inline Mantissa(const ExtendedMantissa& emSource) GCC_INLINE;
         
      inline void normalizeLastCell() GCC_INLINE; 
      inline Mantissa& operator=(unsigned int uValue) GCC_INLINE; 
      inline Mantissa& operator=(const Mantissa& mSource) GCC_INLINE;
      inline Mantissa& operator=(const ExtendedMantissa& emSource) GCC_INLINE;
   };
   
   class Exponent : public Integer::TBigInt<Integer::Details::TIntegerTraits<BitSizeExponent> > {
     private:
      typedef Integer::TBigInt<Integer::Details::TIntegerTraits<BitSizeExponent> > inherited;
      
     public:
      class Min { public : inline Min() GCC_INLINE; };
      class Zero { public : inline Zero() GCC_INLINE; };
      class One { public : inline One() GCC_INLINE; };
      class MinusOne { public : inline MinusOne() GCC_INLINE; };
      class Max { public : inline Max() GCC_INLINE; };
      class Basic { public : inline Basic() GCC_INLINE; };
      
      inline Exponent(Basic, unsigned int uBasicValue) GCC_INLINE;
      inline Exponent(Min) GCC_INLINE;  
      inline Exponent(Max) GCC_INLINE;  
      inline Exponent(Zero) GCC_INLINE; 
      inline Exponent(MinusOne) GCC_INLINE;
      inline Exponent(One) GCC_INLINE;  
      inline Exponent(const Exponent& eSource) GCC_INLINE;

      inline Exponent& operator=(const Exponent& eSource) GCC_INLINE; 
      inline Exponent& operator=(unsigned int uSource) GCC_INLINE; 
      inline Exponent& operator-=(const Exponent& eSource) GCC_INLINE; 
      inline Exponent& operator+=(const Exponent& eSource) GCC_INLINE; 
      inline Exponent& operator--() GCC_INLINE; 
      inline Exponent& operator++() GCC_INLINE; 
   };
   
   class ExtendedMantissa : public Integer::TBigInt<Integer::Details::TIntegerTraits<BitSizeMantissa+1> > {
     private:
      typedef Integer::TBigInt<Integer::Details::TIntegerTraits<BitSizeMantissa+1> > inherited;

     public:
      inline ExtendedMantissa(const ExtendedMantissa& emSource) GCC_INLINE;
      inline ExtendedMantissa(const Mantissa& mSource) GCC_INLINE;
   };
   
   class IntConversion {
     private:
      unsigned int uResult;
      bool fUnsigned;
     
     public:
      inline IntConversion() GCC_INLINE;
      inline IntConversion(const IntConversion& iSource) GCC_INLINE;

      inline IntConversion& setSigned() GCC_INLINE; 
      inline IntConversion& setUnsigned() GCC_INLINE; 
      inline IntConversion& assign(int uValue) GCC_INLINE; 
      inline IntConversion& assign(unsigned int uValue) GCC_INLINE; 

      inline int querySize() const GCC_INLINE;   
      inline int queryMaxDigits() const GCC_INLINE;
      inline bool isUnsigned() const GCC_INLINE; 
      inline bool isSigned() const GCC_INLINE;   
      inline int queryInt() const GCC_INLINE; 
      inline unsigned int queryUnsignedInt() const GCC_INLINE; 
      inline unsigned int& sresult() GCC_INLINE; 
      inline void opposite() GCC_INLINE; 
      inline bool isPositive() const GCC_INLINE; 
      inline bool isNegative() const GCC_INLINE; 
      inline bool isDifferentZero() const GCC_INLINE; 
      inline bool log_base_2() const GCC_INLINE; 
      inline bool hasZero(int uDigits) const GCC_INLINE; 
      inline bool cbitArray(int uLocalIndex) const GCC_INLINE; 
      inline IntConversion& operator>>=(int uShift) GCC_INLINE; 
      inline IntConversion& operator<<=(int uShift) GCC_INLINE; 
      inline IntConversion& operator&=(const IntConversion& icSource) GCC_INLINE; 
      inline IntConversion& neg() GCC_INLINE; 
      inline IntConversion& inc() GCC_INLINE; 

      inline IntConversion& operator=(const IntConversion& icSource) GCC_INLINE;
      inline IntConversion& operator=(int uValue) GCC_INLINE;
         
      class BitArray {
        private:
         unsigned int* puResult;
         int uIndex;

        public:
         inline BitArray(IntConversion& icThis, int uIndexSource) GCC_INLINE;
         inline BitArray(const BitArray& baSource) GCC_INLINE;
         inline BitArray& operator=(const BitArray& baSource) GCC_INLINE;
         inline BitArray& operator=(bool fValue) GCC_INLINE;
            
         inline operator bool() const GCC_INLINE;
            
      };
      friend class BitArray;
      inline BitArray bitArray(int uIndex) GCC_INLINE; 

      inline void setMin() GCC_INLINE; 
      inline void setMax() GCC_INLINE; 
      inline unsigned int& operator[](int uIndex) GCC_INLINE; 
      inline const unsigned int& operator[](int uIndex) const GCC_INLINE; 
   };

   typedef typename inherited::TFloatConversion<2, 1> FloatConversion;

   typedef Integer::TBigInt<Numerics::Integer::Details::TIntegerTraits<BitSizeMantissa+BitSizeExponent+1> >
      DiffDouble;
   typedef BuiltDoubleTraits<2*BitSizeMantissa+1, BitSizeExponent+1> MultExtension;
   typedef Details::DTDoubleElement::Access::StatusAndControlFlags StatusAndControlFlags;
   typedef Details::DTDoubleElement::Access::WriteParameters WriteParameters;
};

template <int BitSizeMantissa, int BitSizeExponent>
inline int
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::bitSizeMantissa()
{ return BitSizeMantissa; }

template <int BitSizeMantissa, int BitSizeExponent>
inline int
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::bitSizeExponent()
{ return BitSizeExponent; }

template <int BitSizeMantissa, int BitSizeExponent>
inline
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Mantissa::Mantissa()
{}

template <int BitSizeMantissa, int BitSizeExponent>
inline
BuiltDoubleTraits<BitSizeMantissa, BitSizeExponent>::Mantissa::Mantissa(unsigned int uValue)
 : inherited(uValue) {}

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

template <class TypeTraits>
class TBuiltDouble : protected Details::DTDoubleElement::Access, private TypeTraits {
  private:
   typedef TBuiltDouble<TypeTraits> thisType;

  public:
   typedef typename TypeTraits::StatusAndControlFlags StatusAndControlFlags;
   typedef typename TypeTraits::WriteParameters WriteParameters;
   class MultParameters {
     private:
      enum Operation { OPlusPlus=0, OPlusMinus=1, OMinusPlus=2, OMinusMinus=3 };
      Operation oOperation;
      StatusAndControlFlags& scfFlags;
      const thisType* pbdAdd;

     public:
      inline MultParameters(StatusAndControlFlags& scfFlagsSource) GCC_INLINE;
      inline MultParameters(const MultParameters& mpSource) GCC_INLINE;

      inline MultParameters& setAdd(const thisType& bdAdd) GCC_INLINE; 
      inline void clear() GCC_INLINE; 
      inline MultParameters& setPositiveMult() GCC_INLINE;
      inline MultParameters& setNegativeMult() GCC_INLINE;
      inline MultParameters& setPositiveAdditive() GCC_INLINE;
      inline MultParameters& setNegativeAdditive() GCC_INLINE;
         
      inline bool hasAdd() const GCC_INLINE; 
      inline StatusAndControlFlags& readParams() const GCC_INLINE; 
      inline const thisType& queryAddSource() const GCC_INLINE; 
      inline bool isPositiveAdditive() const GCC_INLINE; 
      inline bool isNegativeAdditive() const GCC_INLINE; 
      inline bool isPositiveMult() const GCC_INLINE; 
      inline bool isNegativeMult() const GCC_INLINE; 
      inline bool hasSameSign(bool fNegative) const GCC_INLINE;
   };
   friend class MultParameters;
   
  private:
   inline int bitSizeMantissa() const GCC_INLINE; 
   inline int bitSizeExponent() const GCC_INLINE; 

   typename TypeTraits::Mantissa biMantissa;
   typename TypeTraits::Exponent biExponent;
   bool fNegative;

   void addExtension(const thisType& bdSource,
         typename Integer::TBigInt<typename TypeTraits::ExtendedMantissa::MultResult>& mprResult,
         StatusAndControlFlags& scfFlags, bool fPositiveAdd, int& uLogResult,
         bool& fExponentHasCarry, bool& fResultPositiveExponent, bool& fAddExponent);

  public:
   thisType& plusAssignSureNN(const thisType& bdSource, StatusAndControlFlags& scfFlags);
   thisType& plusAssignSureND(const thisType& bdSource, StatusAndControlFlags& scfFlags);
   thisType& plusAssignSureDN(const thisType& bdSource, StatusAndControlFlags& scfFlags);
   thisType& plusAssignSureDD(const thisType& bdSource, StatusAndControlFlags& scfFlags);
   thisType& minusAssignSureNN(const thisType& bdSource, StatusAndControlFlags& scfFlags);
   thisType& minusAssignSureND(const thisType& bdSource, StatusAndControlFlags& scfFlags);
   thisType& minusAssignSureDD(const thisType& bdSource, StatusAndControlFlags& scfFlags);
   thisType& multAssignNN(const thisType& bdSource, const MultParameters& mpParams);
   thisType& multAssignND(const thisType& bdSource, const MultParameters& mpParams);
   thisType& multAssignDN(const thisType& bdSource, const MultParameters& mpParams);
   thisType& multAssignDD(const thisType& bdSource, const MultParameters& mpParams);
   thisType& divAssignNN(const thisType& bdSource, StatusAndControlFlags& scfFlags);
   thisType& divAssignND(const thisType& bdSource, StatusAndControlFlags& scfFlags);
   thisType& divAssignDN(const thisType& bdSource, StatusAndControlFlags& scfFlags);
   thisType& divAssignDD(const thisType& bdSource, StatusAndControlFlags& scfFlags);

  public:
   inline TBuiltDouble() GCC_INLINE;
   TBuiltDouble(unsigned int uValue);
   typedef typename TypeTraits::IntConversion IntConversion;
   typedef typename TypeTraits::FloatConversion FloatConversion;
   TBuiltDouble(const IntConversion& icValue, StatusAndControlFlags& scfFlags);
   TBuiltDouble(const FloatConversion& fcValue, StatusAndControlFlags& scfFlags);
   inline TBuiltDouble(const thisType& bdSource) GCC_INLINE;
   inline thisType& operator=(const thisType& bdSource) GCC_INLINE;

   void retrieveInteger(IntConversion& icResult, StatusAndControlFlags& scfFlags) const;
   enum ComparisonResult { CRNaN=0, CRLess=1, CREqual=2, CRGreater=3 };
   inline ComparisonResult compare(const thisType& bdSource) const GCC_INLINE;
   inline bool operator==(const thisType& bdSource) const GCC_INLINE; 
   inline bool operator!=(const thisType& bdSource) const GCC_INLINE;
   inline bool operator<(const thisType& bdSource) const GCC_INLINE; 
   inline bool operator>(const thisType& bdSource) const GCC_INLINE; 
   inline bool operator<=(const thisType& bdSource) const GCC_INLINE;
   inline bool operator>=(const thisType& bdSource) const GCC_INLINE;

   static typename TypeTraits::Exponent eZeroExponent;
   static typename TypeTraits::Exponent eOneExponent;
   static typename TypeTraits::Exponent eMinusOneExponent;
   static typename TypeTraits::Exponent eInftyExponent;
   
   inline static const typename TypeTraits::Exponent& getZeroExponent() GCC_INLINE; 
   inline static const typename TypeTraits::Exponent& getOneExponent() GCC_INLINE; 
   inline static const typename TypeTraits::Exponent& getMinusOneExponent() GCC_INLINE; 
   inline static const typename TypeTraits::Exponent& getInftyExponent() GCC_INLINE; 
   inline static const typename TypeTraits::Exponent& getMaxExponent() GCC_INLINE; 

   inline const typename TypeTraits::Exponent& queryBasicExponent() const GCC_INLINE; 
   inline typename TypeTraits::Exponent& querySBasicExponent() GCC_INLINE; 
   inline bool hasPositiveExponent() const GCC_INLINE; 
   inline bool hasPositiveOrNullExponent() const GCC_INLINE; 
   inline bool hasNullExponent() const GCC_INLINE; 
   inline bool hasNegativeExponent() const GCC_INLINE; 
   inline bool hasNegativeOrNullExponent() const GCC_INLINE; 

   inline typename TypeTraits::Exponent queryExponent() const GCC_INLINE;
   inline void setBasicExponent(const typename TypeTraits::Exponent& biExponentSource) GCC_INLINE; 
   inline void setInfty() GCC_INLINE; 
   inline void setZero() GCC_INLINE; 
   inline void setOne() GCC_INLINE; 
   inline void setExponent(const typename TypeTraits::Exponent& biExponentSource, bool fNegative = false) GCC_INLINE;
      
   inline bool isPositive() const GCC_INLINE; 
   inline bool isNegative() const GCC_INLINE; 
   inline void setSign(bool fPositive) GCC_INLINE; 
   inline thisType& opposite() GCC_INLINE; 
   inline void setPositive() GCC_INLINE; 
   inline void setNegative() GCC_INLINE; 

   inline bool isInfty() const GCC_INLINE;
   inline bool isNaN() const GCC_INLINE;
   inline bool isSNaN() const GCC_INLINE;
   inline bool isQNaN() const GCC_INLINE;
   inline bool isNormalized() const GCC_INLINE;
   inline bool isDenormalized() const GCC_INLINE; 
   inline bool isZero() const GCC_INLINE; 
   inline thisType queryEpsilon() const GCC_INLINE;

   typedef typename TypeTraits::Exponent Exponent;
   typedef typename TypeTraits::Mantissa Mantissa;
   inline const Mantissa& queryMantissa() const GCC_INLINE; 
   inline Mantissa& querySMantissa() GCC_INLINE; 

   typedef typename TypeTraits::DiffDouble DiffDouble;
#ifndef __BORLANDC__
   inline DiffDouble queryNumberOfFloatsBetween(const thisType& bdSource) const GCC_INLINE;
#else
   void retrieveNumberOfFloatsBetween(const thisType& bdSource, DiffDouble& ddResult) const;
   inline DiffDouble queryNumberOfFloatsBetween(const thisType& bdSource) const GCC_INLINE;
#endif

#if !defined(__GNUC__) || (GCC_VERSION >= 30000)
   thisType queryNthSuccessor(const DiffDouble& biIntSource) const;
   thisType queryNthPredecessor(const DiffDouble& biIntSource) const;
#else
  private:
   void retrieveNthSuccessor(thisType& bdResult, void* pvDiff) const;

  public:
   thisType queryNthSuccessor(const DiffDouble& ddDiff) const
      {  thisType bdResult;
         retrieveNthSuccessor(bdResult, const_cast<DiffDouble*>(&ddDiff));
         return bdResult;
      }
     
  private:
   void retrieveNthPredecessor(thisType& bdResult, void* pvDiff) const;

  public:
   thisType queryNthPredecessor(const DiffDouble& ddDiff) const
      {  thisType bdResult;
         retrieveNthPredecessor(bdResult, const_cast<DiffDouble*>(&ddDiff));
         return bdResult;
      }
#endif

   thisType& plusAssign(const thisType& bdSource, StatusAndControlFlags& scfFlags);
   thisType& minusAssign(const thisType& bdSource, StatusAndControlFlags& scfFlags);
   thisType& multAssign(unsigned int uValue, StatusAndControlFlags& scfFlags);
   inline thisType& multAssign(const thisType& bdSource, StatusAndControlFlags& scfFlags) GCC_INLINE;
   inline thisType& multAndAddAssign(const thisType& bdMult, const thisType& bdAdd, StatusAndControlFlags& scfFlags) GCC_INLINE;
   inline thisType& multAndSubAssign(const thisType& bdMult, const thisType& bdAdd, StatusAndControlFlags& scfFlags) GCC_INLINE;
   inline thisType& negativeMultAndSubAssign(const thisType& bdMult, const thisType& bdAdd, StatusAndControlFlags& scfFlags) GCC_INLINE;
   inline thisType& negativeMultAndAddAssign(const thisType& bdMult, const thisType& bdAdd, StatusAndControlFlags& scfFlags) GCC_INLINE;
   thisType& multAssign(const thisType& bdSource, const MultParameters& mpParams);
   thisType& divAssign(unsigned int uValue, StatusAndControlFlags& scfFlags);
   thisType& divAssign(const thisType& bdSource, StatusAndControlFlags& scfFlags);

   inline thisType& operator+=(const thisType& bdSource) GCC_INLINE;
   inline thisType operator+(const thisType& bdSource) const GCC_INLINE;
   inline thisType& operator-=(const thisType& bdSource) GCC_INLINE;
   inline thisType operator-(const thisType& bdSource) const GCC_INLINE;
   inline thisType& operator*=(const thisType& bdSource) GCC_INLINE;
   inline thisType operator*(const thisType& bdSource) const GCC_INLINE;
   inline thisType& operator/=(const thisType& bdSource) GCC_INLINE;
   inline thisType operator/(const thisType& bdSource) const GCC_INLINE;
      
   void read(std::istream& isIn, StatusAndControlFlags& scfFlags);
   void write(std::ostream& osOut, const WriteParameters& wpParams) const;
   void writeDecimal(std::ostream& osOut) const;

   inline void clear() GCC_INLINE;
   inline void swap(thisType& bdSource) GCC_INLINE;
};

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
{ return TypeTraits::bitSizeMantissa(); }

template <class TypeTraits>
inline int
TBuiltDouble<TypeTraits>::bitSizeExponent() const
{ return TypeTraits::bitSizeExponent(); }

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
inline const typename TypeTraits::Exponent&
TBuiltDouble<TypeTraits>::getZeroExponent()
{ return eZeroExponent; }

template <class TypeTraits>
inline const typename TypeTraits::Exponent&
TBuiltDouble<TypeTraits>::getOneExponent()
{ return eOneExponent; }

template <class TypeTraits>
inline const typename TypeTraits::Exponent&
TBuiltDouble<TypeTraits>::getMinusOneExponent()
{ return eMinusOneExponent; }

template <class TypeTraits>
inline const typename TypeTraits::Exponent&
TBuiltDouble<TypeTraits>::getInftyExponent()
{ return eInftyExponent; }

template <class TypeTraits>
inline const typename TypeTraits::Exponent&
TBuiltDouble<TypeTraits>::getMaxExponent()
{ return eZeroExponent; }

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
TBuiltDouble<TypeTraits>::setNegative()
{ fNegative = true; }

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
inline TBuiltDouble<TypeTraits>
TBuiltDouble<TypeTraits>::queryEpsilon() const
{  thisType dResult = *this;
   if (dResult.biExponent > bitSizeMantissa()) {
      dResult.biExponent.sub(bitSizeMantissa());
      dResult.biMantissa = 0U;
   }
   else if (dResult.biExponent.isZero())
      dResult.biMantissa = 1U;
   else {
      dResult.biMantissa = 0U;
      dResult.biMantissa.bitArray(dResult.biExponent.queryValue()-1) = true;
      dResult.biExponent = 0U;
   };
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

/*****************************/
/* Definition - class Errors */
/*****************************/

class Errors {
  private:
   unsigned int uErrors;
   enum Type { TPositiveOverflow, TNegativeOverflow, TQNaN, TSNaN, TPositiveUnderflow, TNegativeUnderflow };

  public:
   inline Errors() GCC_INLINE;
   inline Errors(const Errors& eSource) GCC_INLINE;
   inline Errors& operator=(const Errors& eSource) GCC_INLINE;

#define DefineIsError(TypeError) \
   inline bool is##TypeError() const GCC_INLINE;
   
   DefineIsError(PositiveOverflow)
   DefineIsError(NegativeOverflow)
   DefineIsError(QNaN)
   DefineIsError(SNaN)
   DefineIsError(PositiveUnderflow)
   DefineIsError(NegativeUnderflow)
#undef DefineIsError
   inline bool isNaN() const GCC_INLINE;

#define DefineSetError(TypeError) \
   inline Errors& set##TypeError() GCC_INLINE;
   
   DefineSetError(PositiveOverflow)
   DefineSetError(NegativeOverflow)
   DefineSetError(QNaN)
   DefineSetError(SNaN)
   DefineSetError(PositiveUnderflow)
   DefineSetError(NegativeUnderflow)
#undef DefineSetError
};

/***************************************/
/* Definition - template TFloatingBase */
/***************************************/

class DoubleTraits {
  public:
   typedef double TypeFloat;
   //typedef unsigned long int TypeIntegerConversion; // sizeof(TypeFloat) == sizeof(TypeIntegerConversion)
   inline static double max() GCC_INLINE;              
   inline static double normalizedMin() GCC_INLINE;    
   inline static double denormalizedMin() GCC_INLINE;  
   inline static bool isBigEndian() GCC_INLINE;
   inline static bool isLittleEndian() GCC_INLINE; 

   static const int UBitSizeMantissa = DBL_MANT_DIG-1;
   static const int UByteSizeImplantation = sizeof(TypeFloat);
   static const int UBitSizeExponent = UByteSizeImplantation*8-DBL_MANT_DIG;
   inline static unsigned int getMaxExponent() GCC_INLINE; 
   inline static unsigned int getZeroExponent() GCC_INLINE; 
   inline static double epsilon() GCC_INLINE; 
   inline static bool isValid() GCC_INLINE; 
};

class FloatTraits {
  public:
   typedef float TypeFloat;
   //typedef unsigned int TypeIntegerConversion; // sizeof(TypeFloat) == sizeof(TypeIntegerConversion)
   inline static float max() GCC_INLINE;              
   inline static float normalizedMin() GCC_INLINE;    
   inline static float denormalizedMin() GCC_INLINE;  
   inline static bool isBigEndian() GCC_INLINE;
   inline static bool isLittleEndian() GCC_INLINE; 

   static const int UBitSizeMantissa = FLT_MANT_DIG-1;
   static const int UByteSizeImplantation = sizeof(TypeFloat);
   static const int UBitSizeExponent = UByteSizeImplantation*8-FLT_MANT_DIG;
   inline static unsigned int getMaxExponent() GCC_INLINE; 
   inline static unsigned int getZeroExponent() GCC_INLINE; 
   inline static float epsilon() GCC_INLINE; 
   inline static bool isValid() GCC_INLINE; 
};

template <class TypeTraits>
class TFloatingBase {
  public:
   typedef TBuiltDouble<BuiltDoubleTraits<TypeTraits::UBitSizeMantissa, TypeTraits::UBitSizeExponent> >
      BuiltDouble;

  protected:
   typedef typename TypeTraits::TypeFloat Implantation;
   typedef TFloatingBase<TypeTraits> thisType;

  private:
   Implantation dDouble;
   static void test_double(double dDouble);
   static void test_int(unsigned int uInt);
   static void test_char(char cChar);
   static void copy_mem(void* pvDestination, void* pvSource, int uBytes);

  public:
   inline TFloatingBase() GCC_INLINE;
   inline TFloatingBase(const Implantation& dDoubleSource) GCC_INLINE;
#if defined (__GNUC__) && (GCC_VERSION >= 30000)
   inline TFloatingBase(const BuiltDouble& bdDouble) GCC_INLINE;
#else
   inline TFloatingBase(const BuiltDouble& bdDouble) : dDouble(0.0) { operator=(bdDouble); }
#endif
   inline TFloatingBase(const thisType& sSource) GCC_INLINE;
   inline thisType& operator=(const thisType& dsSource) GCC_INLINE;
      
#if !defined(__GNUC__) || (GCC_VERSION >= 30000)
   thisType& operator=(const BuiltDouble& bdDouble);
#else
  private:
   void assign(void* pvBuiltDouble);

  public:
   thisType& operator=(const BuiltDouble& bdSource)
      {  assign(const_cast<BuiltDouble*>(&bdSource));
         return *this;
      }
#endif

#if !defined (__GNUC__) || (GCC_VERSION >= 30000)
   inline operator BuiltDouble() const GCC_INLINE;
#else
   inline operator BuiltDouble() const
	{  BuiltDouble bdResult;
		fillMantissa(bdResult.querySMantissa());
		bdResult.setBasicExponent(typename BuiltDouble::Exponent(typename BuiltDouble::Exponent::Basic(), queryBasicExponent()));
		bdResult.setSign(isPositive());
		return bdResult;
	}
#endif
   inline Implantation& implantation() GCC_INLINE;  
   inline const Implantation& implantation() const GCC_INLINE;  
   inline thisType queryEpsilon() const GCC_INLINE;
   inline bool isZero() const GCC_INLINE; 
   
   typedef Details::DTDoubleElement::Access::StatusAndControlFlags StatusAndControlFlags;
   inline thisType& plusAssign(const thisType& ttSource, const StatusAndControlFlags& scfFlags) GCC_INLINE;
   inline thisType& minusAssign(const thisType& ttSource, const StatusAndControlFlags& scfFlags) GCC_INLINE;
   inline thisType& multAssign(const thisType& ttSource, const StatusAndControlFlags& scfFlags) GCC_INLINE;
   inline thisType& divAssign(const thisType& ttSource, const StatusAndControlFlags& scfFlags) GCC_INLINE;

   inline thisType& operator-=(const thisType& ttSource) GCC_INLINE;
   inline thisType& operator*=(const thisType& ttSource) GCC_INLINE;
   inline thisType& operator+=(const thisType& ttSource) GCC_INLINE;
   inline thisType& operator/=(const thisType& ttSource) GCC_INLINE;

   static const int UByteSizeImplantation = TypeTraits::UByteSizeImplantation;
   static const int UBitSizeMantissa = TypeTraits::UBitSizeMantissa;
   static const int UBitSizeExponent = TypeTraits::UBitSizeExponent;
   typedef typename BuiltDouble::Mantissa Mantissa;
#if !defined(__GNUC__) || (GCC_VERSION >= 30000)
   void fillMantissa(Mantissa& mMantissa) const;
   void setMantissa(const Mantissa& mMantissa);
#else
  private:
   void fillMantissa(void* pvMantissa) const;
   void setMantissa(void* pvMantissa);

  public:
   void fillMantissa(Mantissa& mMantissa) const { fillMantissa((void*) &mMantissa); }
   void setMantissa(const Mantissa& mMantissa)
      {  setMantissa((void*) const_cast<Mantissa*>(&mMantissa)); }
#endif

   inline static unsigned int getMaxExponent() GCC_INLINE; 
   inline static unsigned int getZeroExponent() GCC_INLINE; 
   inline thisType& opposite() GCC_INLINE; 
 
   unsigned int queryBasicExponent() const;
   inline int queryExponent() const GCC_INLINE;
   void setBasicExponent(unsigned int uExponent);
   inline void setExponent(int uExponent) GCC_INLINE;
      
   inline bool isPositive() const GCC_INLINE;
   inline bool isNegative() const GCC_INLINE; 
   inline void setSign(bool fPositive) GCC_INLINE;
   inline void setPositive() GCC_INLINE; 
   inline void setNegative() GCC_INLINE; 
   inline bool isNormalised() const GCC_INLINE;
   inline bool isRanged() const GCC_INLINE;
   inline void clear() GCC_INLINE; 
   inline void swap(thisType& dSource) GCC_INLINE;
};

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

template <class FloatingBaseTraits>
class TDoubleElement : public FloatingBaseTraits, public Details::DTDoubleElement::Access {
  private:
   typedef FloatingBaseTraits inherited;
   typedef TDoubleElement<FloatingBaseTraits> thisType;

   typedef typename FloatingBaseTraits::Mantissa Mantissa;

  public:
   typedef typename inherited::BuiltDouble BuiltDouble;
   typedef Details::DTDoubleElement::Access::StatusAndControlFlags StatusAndControlFlags;
   inline TDoubleElement() GCC_INLINE;
   inline TDoubleElement(int iValue, StatusAndControlFlags& scfFlags) GCC_INLINE;
      
   inline TDoubleElement(typename FloatingBaseTraits::Implantation fbtSource) GCC_INLINE;
   inline TDoubleElement(const BuiltDouble& bdDouble) GCC_INLINE;
   inline TDoubleElement(const thisType& dSource) GCC_INLINE;
   inline int queryInteger(StatusAndControlFlags& scfFlags) const GCC_INLINE;
   inline unsigned int queryUnsignedInteger(StatusAndControlFlags& scfFlags) const GCC_INLINE;

   bool isInternZero() const;
   inline bool isZero() const GCC_INLINE; 
   bool isNaN() const;
   bool isQNaN() const;
   bool isSNaN() const;
   bool isInfty() const;
   inline bool hasInftyExponent() const GCC_INLINE;

   enum ComparisonResult { CRNaN, CRLess, CREqual, CRGreater };
   inline ComparisonResult compare(const thisType& bdSource) const GCC_INLINE;
   inline bool operator==(const thisType& bdSource) const GCC_INLINE; 
   inline bool operator!=(const thisType& bdSource) const GCC_INLINE;
   inline bool operator<(const thisType& bdSource) const GCC_INLINE; 
   inline bool operator>(const thisType& bdSource) const GCC_INLINE; 
   inline bool operator<=(const thisType& bdSource) const GCC_INLINE;
   inline bool operator>=(const thisType& bdSource) const GCC_INLINE;

   static const int UByteSizeImplantation = inherited::UByteSizeImplantation;
   static const int UBitSizeMantissa = inherited::UBitSizeMantissa;
   static const int UBitSizeExponent = inherited::UBitSizeExponent;

   typedef typename BuiltDouble::DiffDouble DiffDouble;
   inline DiffDouble queryNumberOfFloatsBetween(const thisType& deElement) const GCC_INLINE;
   inline thisType queryNthSuccessor(const DiffDouble& biIntSource) const GCC_INLINE;
   inline thisType queryNthPredecessor(const DiffDouble& biIntSource) const GCC_INLINE;
   bool isPuiss2() const;
   Errors queryErrors() const;

   inline thisType& opposite() GCC_INLINE; 
   inline thisType& plusAssign(const thisType& ttSource, StatusAndControlFlags& scfFlags) GCC_INLINE;
   inline thisType& minusAssign(const thisType& ttSource, StatusAndControlFlags& scfFlags) GCC_INLINE;
   inline thisType& multAssign(const thisType& ttSource, StatusAndControlFlags& scfFlags) GCC_INLINE;
   inline thisType& divAssign(const thisType& ttSource, StatusAndControlFlags& scfFlags) GCC_INLINE;

   inline thisType& operator-=(const thisType& ttSource) GCC_INLINE;
   inline thisType& operator*=(const thisType& ttSource) GCC_INLINE;
   inline thisType& operator+=(const thisType& ttSource) GCC_INLINE;
   inline thisType& operator/=(const thisType& ttSource) GCC_INLINE;

   inline thisType operator-(const thisType& ttSource) const GCC_INLINE;
   inline thisType operator+(const thisType& ttSource) const GCC_INLINE;
   inline thisType operator*(const thisType& ttSource) const GCC_INLINE;
   inline thisType operator/(const thisType& ttSource) const GCC_INLINE;

   inline thisType& setOpposite() GCC_INLINE;
   inline thisType& setInftyExponent() GCC_INLINE;
   void write(std::ostream& osOut, const WriteParameters& wpParams) const;
   void read(std::istream& isIn, StatusAndControlFlags& scfFlags);
};

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

}} // end of namespace Numerics::Double

} // end of namespace simfloat
} // end of namespace lib
} // end of namespace unisim

#endif // Numerics_Double_FloatingH

