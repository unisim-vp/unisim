/***************************************************************************
                Floating.h  -  Template for various types of floating point computations
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

#include <endian.h>
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
#include <unisim/lib/simfloat/integer.hh>

namespace unisim {
namespace util {
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
      StatusAndControlFlags()
         :  fAvoidInfty(false), fKeepNaNSign(false), fProduceDivNaNPositive(false),
            fRoundToEven(false), fPositiveZeroMAdd(false), fUpApproximateInfty(false),
            fUpApproximateInversionForNear(false), fChooseNaNAddBeforeMult(false),
            fConvertNaNNegative(false), fRefuseMinusZero(false), rmRound(RMNearest),
            fKeepSignalingConversion(false), aApproximation(AExact), rrReadResult(RRTotal),
            fEffectiveRoundToEven(false), fSNaNOperand(false), qnrQNaNResult(QNNRUndefined),
            feExcept(FENoException), fDivisionByZero(false) {}
      StatusAndControlFlags(const StatusAndControlFlags& rpSource)
         :  fAvoidInfty(rpSource.fAvoidInfty), fKeepNaNSign(rpSource.fKeepNaNSign), fProduceDivNaNPositive(rpSource.fProduceDivNaNPositive),
            fRoundToEven(rpSource.fRoundToEven), fPositiveZeroMAdd(rpSource.fPositiveZeroMAdd),
            fUpApproximateInfty(rpSource.fUpApproximateInfty), fUpApproximateInversionForNear(rpSource.fUpApproximateInversionForNear),
            fChooseNaNAddBeforeMult(rpSource.fChooseNaNAddBeforeMult), fConvertNaNNegative(rpSource.fConvertNaNNegative),
            fRefuseMinusZero(rpSource.fRefuseMinusZero), rmRound(rpSource.rmRound),
            fKeepSignalingConversion(rpSource.fKeepSignalingConversion),
            aApproximation(rpSource.aApproximation), rrReadResult(rpSource.rrReadResult),
            fEffectiveRoundToEven(rpSource.fEffectiveRoundToEven), fSNaNOperand(rpSource.fSNaNOperand), qnrQNaNResult(rpSource.qnrQNaNResult),
            feExcept(rpSource.feExcept), fDivisionByZero(rpSource.fDivisionByZero) {}

      StatusAndControlFlags& operator=(const StatusAndControlFlags& rpSource)
         {  aApproximation = rpSource.aApproximation;
            rrReadResult = rpSource.rrReadResult;
            fEffectiveRoundToEven = rpSource.fEffectiveRoundToEven;
            fSNaNOperand = rpSource.fSNaNOperand;
            qnrQNaNResult = rpSource.qnrQNaNResult;
            feExcept = rpSource.feExcept;
            fDivisionByZero = rpSource.fDivisionByZero;
            return *this;
         }

      // Static parameters
      StatusAndControlFlags& setRoundToEven()
         {  rmRound = RMNearest; fRoundToEven = true; return *this; }
      StatusAndControlFlags& clearRoundToEven() { fRoundToEven = false; return *this; }
      StatusAndControlFlags& setPositiveZeroMAdd() { fPositiveZeroMAdd = true; return *this; }
      StatusAndControlFlags& avoidInfty()     { fAvoidInfty = true; return *this; }
      StatusAndControlFlags& setKeepNaNSign() { fKeepNaNSign = true; return *this; }
      StatusAndControlFlags& setProduceDivNaNPositive() { fProduceDivNaNPositive = true; return *this; }
      StatusAndControlFlags& setUpApproximateInfty() { fUpApproximateInfty = true; return *this; }
      StatusAndControlFlags& setUpApproximateInversionForNear() { fUpApproximateInversionForNear = true; return *this; }
      StatusAndControlFlags& setChooseNaNAddBeforeMult() { fChooseNaNAddBeforeMult = true; return *this; }
      StatusAndControlFlags& setConvertNaNNegative() { fConvertNaNNegative= true; return *this; }
      StatusAndControlFlags& setAcceptMinusZero() { fRefuseMinusZero = false; return *this; }
      StatusAndControlFlags& setRefuseMinusZero() { fRefuseMinusZero = true; return *this; }

      bool isRoundToEven() const { return fRoundToEven && isNearestRound(); }
      bool isPositiveZeroMAdd() { return fPositiveZeroMAdd; }
      bool isInftyAvoided() const { return fAvoidInfty; }
      bool doesAvoidInfty(bool fNegative) const
         {  assert(fAvoidInfty);
            return fNegative ? (rmRound >= RMHighest) : (rmRound & RMLowest);
         }
      bool keepNaNSign() const { return fKeepNaNSign; }
      bool produceDivNaNPositive() const { return fProduceDivNaNPositive; }
      bool upApproximateInfty() const { return fUpApproximateInfty; }
      bool upApproximateInversionForNear() const { return fUpApproximateInversionForNear; }
      bool chooseNaNAddBeforeMult() const { return fChooseNaNAddBeforeMult; }
      bool isConvertNaNNegative() const { return fConvertNaNNegative; }
      bool acceptMinusZero() const { return !fRefuseMinusZero; }

      // dynamic read parameters
      StatusAndControlFlags& setNearestRound()   { rmRound = RMNearest; return *this; }
      StatusAndControlFlags& setHighestRound()   { rmRound = RMHighest; return *this; }
      StatusAndControlFlags& setLowestRound()    { rmRound = RMLowest; return *this; }
      StatusAndControlFlags& setZeroRound()      { rmRound = RMZero; return *this; }

      bool isLowestRound() const { return rmRound == RMLowest; }
      bool isNearestRound() const { return rmRound == RMNearest; }
      bool isHighestRound() const { return rmRound == RMHighest; }
      bool isZeroRound() const { return rmRound == RMZero; }

      StatusAndControlFlags& setKeepSignalingConversion() { fKeepSignalingConversion = true; return *this; }
      StatusAndControlFlags& clearKeepSignalingConversion() { fKeepSignalingConversion = false; return *this; }
      bool keepSignalingConversion() const { return fKeepSignalingConversion; }

      // dynamic write parameters
      bool isApproximate() const { return aApproximation != AExact; }
      bool isDownApproximate() const { return aApproximation == ADownApproximate; }
      bool isUpApproximate() const { return aApproximation == AUpApproximate; }
      void setDownApproximate() { aApproximation = ADownApproximate; }
      void setUpApproximate() { aApproximation = AUpApproximate; }
      void clearApproximate() { aApproximation = AExact; }
      enum Direction { Down, Up };
      void setApproximate(Direction dDirection)
         {  aApproximation = ((dDirection == Down) ? ADownApproximate : AUpApproximate); }
      bool isApproximate(Direction dDirection) const
         {  return aApproximation == ((dDirection == Down) ? ADownApproximate : AUpApproximate); }
      bool hasSameApproximation(const StatusAndControlFlags& rpSource) const
         {  return aApproximation == rpSource.aApproximation; }
      bool hasIncrementFraction(bool fNegative) const
         {  return fNegative ? isDownApproximate() : isUpApproximate(); }

      void setEffectiveRoundToEven() { fEffectiveRoundToEven = true; }
      void clearEffectiveRoundToEven() { fEffectiveRoundToEven = false; }
      bool hasEffectiveRoundToEven() { return fEffectiveRoundToEven; }

      void setPartialRead() { rrReadResult = RRPartial; }
      void setTotalRead() { rrReadResult = RRTotal; }
      bool isPartialRead() const { return rrReadResult == RRPartial; }
      bool isTotalRead() const { return rrReadResult == RRTotal; }
      bool hasPartialRead() const { return rrReadResult != RRTotal; }

      void setSNaNOperand() { fSNaNOperand = true; }
      bool hasSNaNOperand() const { return fSNaNOperand; }
      
      void setInftyMinusInfty() { assert(!qnrQNaNResult); qnrQNaNResult = QNNRInftyMinusInfty; }
      void setInftyOnInfty() { assert(!qnrQNaNResult); qnrQNaNResult = QNNRInftyOnInfty; }
      void setZeroOnZero() { assert(!qnrQNaNResult); qnrQNaNResult = QNNRZeroOnZero; }
      void setInftyMultZero() { assert(!qnrQNaNResult); qnrQNaNResult = QNNRInftyMultZero; }
      bool hasQNaNResult() const { return qnrQNaNResult; }
      bool isInftyMinusInfty() const { return qnrQNaNResult == QNNRInftyMinusInfty; }
      bool isInftyOnInfty() const { return qnrQNaNResult == QNNRInftyOnInfty; }
      bool isZeroOnZero() const { return qnrQNaNResult == QNNRZeroOnZero; }
      bool isInftyMultZero() const { return qnrQNaNResult == QNNRInftyMultZero; }

      void clear()
         {  aApproximation = AExact;
            rrReadResult = RRTotal;
            fEffectiveRoundToEven = false;
            fSNaNOperand = false;
            qnrQNaNResult = QNNRUndefined;
            feExcept = FENoException;
            fDivisionByZero = false;
         }

      bool isDivisionByZero() const { return fDivisionByZero; }
      void setDivisionByZero() { fDivisionByZero = true; }
      bool hasFlowException() const { return feExcept != FENoException; }
      void clearFlowException() { feExcept = FENoException; }
      void setOverflow() { feExcept = FEOverflow; }
      void setUnderflow() { feExcept = FEUnderflow; }
      bool isOverflow() const { return feExcept == FEOverflow; }
      bool isUnderflow() const { return feExcept == FEUnderflow; }
      void clearUnderflow() { feExcept = FENoException; }
   };
   class WriteParameters {
     private:
      enum Type { TDecimal, TBinary };
      Type tType;
         
     public:
      WriteParameters() : tType(TDecimal) {}

      WriteParameters& setDecimal() { tType = TDecimal; return *this; }
      WriteParameters& setBinary() { tType = TBinary; return *this; }

      bool isDecimal() const { return tType == TDecimal; }
      bool isBinary() const { return tType == TBinary; }
   };

   class Carry {
     private:
      bool fCarry;

     public:
      Carry() : fCarry(false) {}
      Carry(const Carry& cSource) : fCarry(cSource.fCarry) {}
      bool& carry() { return fCarry; }
      const bool& carry() const { return fCarry; }
      bool hasCarry() const { return fCarry; }
   };
   
   template <class TypeMantissa, class TypeStatusAndControlFlags>
   static Carry trightShift(TypeMantissa& mMantissa, int uShift,
         unsigned int uValue, TypeStatusAndControlFlags& scfFlags, bool fNegative, int uBitSizeMantissa);
};

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
      TFloatConversion() : bciMantissa(), uBitsMantissa(0), bciExponent(), uBitsExponent(0), fNegative(false) {}
      TFloatConversion(const thisType& fcSource)
         :  bciMantissa(fcSource.bciMantissa), uBitsMantissa(fcSource.uBitsMantissa),
            bciExponent(fcSource.bciExponent), uBitsExponent(fcSource.uBitsExponent),
            fNegative(fcSource.fNegative) {}

      bool isPositive() const { return !fNegative; }
      bool isNegative() const { return fNegative; }
      bool isInftyExponent() const
         { return Exponent(bciExponent).neg(uBitsExponent).hasZero(uBitsExponent); }
      bool isZeroExponent() const { return bciExponent.hasZero(uBitsExponent); }
      bool isZeroMantissa() const { return bciMantissa.hasZero(uBitsMantissa); }
      const int& querySizeMantissa() const { return uBitsMantissa; }
      const int& querySizeExponent() const { return uBitsExponent; }

      thisType& setSizeMantissa(int uSize) { uBitsMantissa = uSize; return *this; }
      thisType& setSizeExponent(int uSize) { uBitsExponent = uSize; return *this; }

      void setPositive(bool fPositive) { fNegative = !fPositive; }
      void setNegative(bool fNegativeSource) { fNegative = fNegativeSource; }
      const Mantissa& mantissa() const { return bciMantissa; }
      Mantissa& mantissa() { return bciMantissa; }
      const Exponent& exponent() const { return bciExponent; }
      Exponent& exponent() { return bciExponent; }
   };
};

}} // end of namespace Details::DBuiltDoubleTraits

template <int BitSizeMantissa, int BitSizeExponent>
class BuiltDoubleTraits : public Details::DBuiltDoubleTraits::Access {
  private:
   typedef Details::DBuiltDoubleTraits::Access inherited;

  public:
   static const int UBitSizeMantissa = BitSizeMantissa;
   static const int UBitSizeExponent = BitSizeExponent;
   static int bitSizeMantissa() { return BitSizeMantissa; }
   static int bitSizeExponent() { return BitSizeExponent; }

   class ExtendedMantissa;
   class Mantissa : public Integer::TBigInt<Integer::Details::TIntegerTraits<BitSizeMantissa> > {
     private:
      typedef Integer::TBigInt<Integer::Details::TIntegerTraits<BitSizeMantissa> > inherited;

     public:
      Mantissa() {}
      Mantissa(unsigned int uValue) : inherited(uValue) {}
      Mantissa(const Mantissa& mSource) : inherited(mSource) {}
      Mantissa(const ExtendedMantissa& emSource)
         {  for (register int uIndex = 0; uIndex < inherited::uCellSizeMantissa; ++uIndex)
               inherited::array(uIndex) = emSource[uIndex];
         }
      void normalizeLastCell() { inherited::normalizeLastCell(); }

      Mantissa& operator=(unsigned int uValue) { return (Mantissa&) inherited::operator=(uValue); }
      Mantissa& operator=(const Mantissa& mSource)
         {  return (Mantissa&) inherited::operator=(mSource); }
      Mantissa& operator=(const ExtendedMantissa& emSource)
         {  for (register int uIndex = 0; uIndex <= inherited::lastCellIndex(); ++uIndex)
               inherited::array(uIndex) = emSource[uIndex];
            inherited::normalize();
            return *this;
         }
   };
   
   class Exponent : public Integer::TBigInt<Integer::Details::TIntegerTraits<BitSizeExponent> > {
     private:
      typedef Integer::TBigInt<Integer::Details::TIntegerTraits<BitSizeExponent> > inherited;
      
     public:
      class Min { public : Min() {} };
      class Zero { public : Zero() {} };
      class One { public : One() {} };
      class MinusOne { public : MinusOne() {} };
      class Max { public : Max() {} };
      class Basic { public : Basic() {} };
      
      Exponent(Basic, unsigned int uBasicValue) : inherited(uBasicValue) {}
      Exponent(Min)  {}
      Exponent(Max)  { inherited::neg(); }
      Exponent(Zero) { inherited::neg(); inherited::bitArray(UBitSizeExponent-1) = false; }
      Exponent(MinusOne)
         {  inherited::neg();
            inherited::bitArray(UBitSizeExponent-1) = false;
            inherited::bitArray(0) = false;
         }
      Exponent(One)  { inherited::bitArray(UBitSizeExponent-1) = true; }
      Exponent(const Exponent& eSource) : inherited(eSource) {}

      Exponent& operator=(const Exponent& eSource) { return (Exponent&) inherited::operator=(eSource); }
      Exponent& operator=(unsigned int uSource) { return (Exponent&) inherited::operator=(uSource); }
      Exponent& operator-=(const Exponent& eSource) { return (Exponent&) inherited::operator-=(eSource); }
      Exponent& operator+=(const Exponent& eSource) { return (Exponent&) inherited::operator+=(eSource); }
      Exponent& operator--() { return (Exponent&) inherited::operator--(); }
      Exponent& operator++() { return (Exponent&) inherited::operator++(); }
   };
   
   class ExtendedMantissa : public Integer::TBigInt<Integer::Details::TIntegerTraits<BitSizeMantissa+1> > {
     private:
      typedef Integer::TBigInt<Integer::Details::TIntegerTraits<BitSizeMantissa+1> > inherited;

     public:
      ExtendedMantissa(const ExtendedMantissa& emSource) : inherited(emSource) {}
      ExtendedMantissa(const Mantissa& mSource)
         {  register int uIndex = 0;
            for (; uIndex <= mSource.lastCellIndex(); ++uIndex)
               inherited::array(uIndex) = mSource[uIndex];
            inherited::bitArray(UBitSizeMantissa) = true;
         }
   };
   
   class IntConversion {
     private:
      unsigned int uResult;
      bool fUnsigned;

     public:
      IntConversion() : uResult(0U), fUnsigned(false) {}
      IntConversion(const IntConversion& iSource) : uResult(iSource.uResult), fUnsigned(iSource.fUnsigned) {}

      IntConversion& setSigned() { fUnsigned = false; return *this; }
      IntConversion& setUnsigned() { fUnsigned = true; return *this; }
      IntConversion& assign(int uValue) { assert(!fUnsigned); uResult = uValue; return *this; }
      IntConversion& assign(unsigned int uValue) { assert(fUnsigned); uResult = uValue; return *this; }

      int querySize() const   { return sizeof(unsigned int)*8; }
      int queryMaxDigits() const
         { return fUnsigned ? sizeof(unsigned int)*8 : (sizeof(unsigned int)*8-1); }
      bool isUnsigned() const { return fUnsigned; }
      bool isSigned() const   { return !fUnsigned; }
      int queryInt() const { assert(!fUnsigned); return (int) uResult; }
      unsigned int queryUnsignedInt() const { assert(fUnsigned); return uResult; }
      unsigned int& sresult() { return uResult; }
      void opposite() { assert(!fUnsigned); uResult = (~uResult + 1); }
      bool isPositive() const { return fUnsigned || ((int) uResult >= 0); }
      bool isNegative() const { return !fUnsigned && ((int) uResult < 0); }
      bool isDifferentZero() const { return uResult != 0; }
      bool log_base_2() const { return Integer::Details::Access::log_base_2(uResult); }
      bool hasZero(int uDigits) const { return uResult & ~(~0U << uDigits); }
      bool cbitArray(int uLocalIndex) const { return uResult & (1U << uLocalIndex); }
      IntConversion& operator>>=(int uShift) { uResult >>= uShift; return *this; }
      IntConversion& operator<<=(int uShift) { uResult <<= uShift; return *this; }
      IntConversion& operator&=(const IntConversion& icSource) { uResult &= icSource.uResult; return *this; }
      IntConversion& neg() { uResult = ~uResult; return *this; }
      IntConversion& inc() { ++uResult; return *this; }

      IntConversion& operator=(const IntConversion& icSource)
         {  uResult = icSource.uResult;
            fUnsigned = icSource.fUnsigned;
            return *this;
         }
      IntConversion& operator=(int uValue)
         {  if (fUnsigned) {
               assert(uValue >= 0);
               uResult = (unsigned int) uValue;
            }
            else
               uResult = (unsigned int) uValue;
            return *this;
         }
      class BitArray {
        private:
         unsigned int* puResult;
         int uIndex;

        public:
         BitArray(IntConversion& icThis, int uIndexSource)
            : puResult(&icThis.uResult), uIndex(uIndexSource) {}
         BitArray(const BitArray& baSource) : puResult(baSource.puResult), uIndex(baSource.uIndex) {}
         BitArray& operator=(const BitArray& baSource)
            {  puResult = baSource.puResult;
               uIndex = baSource.uIndex;
               return *this;
            }
         BitArray& operator=(bool fValue)
            {  if (fValue)
                  *puResult |= (1U << (uIndex%(sizeof(unsigned int)*8)));
               else
                  *puResult &= ~(1U << (uIndex%(sizeof(unsigned int)*8)));
               return *this;
            }
         operator bool() const
            {  return (*puResult & (1U << (uIndex%(sizeof(unsigned int)*8))))
                  ? true : false;
            }
      };
      friend class BitArray;
      BitArray bitArray(int uIndex) {  return BitArray(*this, uIndex); }
      void setBitArray(int uIndex, bool fValue)
         {  if (fValue)
               uResult |= (1U << (uIndex%(sizeof(unsigned int)*8)));
            else
               uResult &= ~(1U << (uIndex%(sizeof(unsigned int)*8)));
         }
      void setTrueBitArray(int uIndex)
         {  uResult |= (1U << (uIndex%(sizeof(unsigned int)*8))); }
      void setFalseBitArray(int uIndex)
         {  uResult &= ~(1U << (uIndex%(sizeof(unsigned int)*8))); }

      void setMin() { uResult = fUnsigned ? 0U : (1U << (8*sizeof(unsigned int)-1)); }
      void setMax() { uResult = fUnsigned ? ~0U : ~(1U << (8*sizeof(unsigned int)-1)); }
      unsigned int& operator[](int uIndex) { assert(uIndex == 0); return uResult; }
      const unsigned int& operator[](int uIndex) const { assert(uIndex == 0); return uResult; }
   };

   typedef typename inherited::TFloatConversion<2, 1> FloatConversion;

   typedef Integer::TBigInt<Numerics::Integer::Details::TIntegerTraits<BitSizeMantissa+BitSizeExponent+1> >
      DiffDouble;
   typedef BuiltDoubleTraits<2*BitSizeMantissa+1, BitSizeExponent+1> MultExtension;
   typedef Details::DTDoubleElement::Access::StatusAndControlFlags StatusAndControlFlags;
   typedef Details::DTDoubleElement::Access::WriteParameters WriteParameters;
};

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
      MultParameters(StatusAndControlFlags& scfFlagsSource)
         :  oOperation(OPlusPlus), scfFlags(scfFlagsSource), pbdAdd(NULL) {}
      MultParameters(const MultParameters& mpSource)
         :  oOperation(mpSource.oOperation), scfFlags(mpSource.scfFlags), pbdAdd(mpSource.pbdAdd) {}

      MultParameters& setAdd(const thisType& bdAdd) { pbdAdd = &bdAdd; return *this; }
      void clear() { pbdAdd = NULL; oOperation = OPlusPlus; }
      MultParameters& setPositiveMult()
         { oOperation = (Operation) ((int) oOperation & (int) OPlusMinus); return *this; }
      MultParameters& setNegativeMult()
         {  oOperation = (Operation) ((int) oOperation | (int) OMinusPlus); return *this; }
      MultParameters& setPositiveAdditive()
         {  oOperation = (Operation) ((int) oOperation & (int) OMinusPlus); return *this; }
      MultParameters& setNegativeAdditive()
         { oOperation = (Operation) ((int) oOperation | (int) OPlusMinus); return *this; }
      bool hasAdd() const { return pbdAdd != NULL; }
      StatusAndControlFlags& readParams() const { return scfFlags; }
      const thisType& queryAddSource() const { assert(pbdAdd); return *pbdAdd; }
      bool isPositiveAdditive() const { return !(oOperation & OPlusMinus); }
      bool isNegativeAdditive() const { return (oOperation & OPlusMinus); }
      bool isPositiveMult() const { return !(oOperation & OMinusPlus); }
      bool isNegativeMult() const { return (oOperation & OMinusPlus); }
      bool hasSameSign(bool fNegative) const
         {  assert(pbdAdd);
            register bool fResult = pbdAdd->fNegative == fNegative;
            return ((oOperation == OPlusPlus) || (oOperation == OMinusMinus)) ? fResult : !fResult;
         }
   };
   
  private:
   int bitSizeMantissa() const { return TypeTraits::bitSizeMantissa(); }
   int bitSizeExponent() const { return TypeTraits::bitSizeExponent(); }

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
#ifndef __BORLANDC__
   TBuiltDouble() : biMantissa(), biExponent(typename TypeTraits::Exponent::Min()), fNegative(false) {}
#else
   TBuiltDouble() : biMantissa(), biExponent(TypeTraits::Exponent::Min()), fNegative(false) {}
#endif
   TBuiltDouble(unsigned int uValue);
   typedef typename TypeTraits::IntConversion IntConversion;
   typedef typename TypeTraits::FloatConversion FloatConversion;
   TBuiltDouble(const IntConversion& icValue, StatusAndControlFlags& scfFlags);
   TBuiltDouble(const FloatConversion& fcValue, StatusAndControlFlags& scfFlags);
   TBuiltDouble(const thisType& bdSource)
      :  biMantissa(bdSource.biMantissa), biExponent(bdSource.biExponent),
         fNegative(bdSource.fNegative) {}
   thisType& operator=(const thisType& bdSource)
      {  biMantissa = bdSource.biMantissa;
         biExponent = bdSource.biExponent;
         fNegative = bdSource.fNegative;
         return *this;
      }

   void retrieveInteger(IntConversion& icResult, StatusAndControlFlags& scfFlags) const;
   enum ComparisonResult { CRNaN=0, CRLess=1, CREqual=2, CRGreater=3 };
   ComparisonResult compare(const thisType& bdSource) const
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
   bool operator==(const thisType& bdSource) const { return compare(bdSource) == CREqual; }
   bool operator!=(const thisType& bdSource) const
      {  register ComparisonResult crResult = compare(bdSource);
         return (crResult == CRLess) || (crResult == CRGreater);
      }
   bool operator<(const thisType& bdSource) const { return compare(bdSource) == CRLess; }
   bool operator>(const thisType& bdSource) const { return compare(bdSource) == CRGreater; }
   bool operator<=(const thisType& bdSource) const
      {  register ComparisonResult crResult = compare(bdSource);
         return (crResult == CRLess) || (crResult == CREqual);
      }
   bool operator>=(const thisType& bdSource) const
      {  register ComparisonResult crResult = compare(bdSource);
         return (crResult == CRGreater) || (crResult == CREqual);
      }

   static typename TypeTraits::Exponent eZeroExponent;
   static typename TypeTraits::Exponent eOneExponent;
   static typename TypeTraits::Exponent eMinusOneExponent;
   static typename TypeTraits::Exponent eInftyExponent;
   
   static const typename TypeTraits::Exponent& getZeroExponent() { return eZeroExponent; }
   static const typename TypeTraits::Exponent& getOneExponent() { return eOneExponent; }
   static const typename TypeTraits::Exponent& getMinusOneExponent() { return eMinusOneExponent; }
   static const typename TypeTraits::Exponent& getInftyExponent() { return eInftyExponent; }
   static const typename TypeTraits::Exponent& getMaxExponent() { return eZeroExponent; }

   const typename TypeTraits::Exponent& queryBasicExponent() const { return biExponent; }
   typename TypeTraits::Exponent& querySBasicExponent() { return biExponent; }
   bool hasPositiveExponent() const { return biExponent > getZeroExponent(); }
   bool hasPositiveOrNullExponent() const { return biExponent >= getZeroExponent(); }
   bool hasNullExponent() const { return biExponent == getZeroExponent(); }
   bool hasNegativeExponent() const { return biExponent < getZeroExponent(); }
   bool hasNegativeOrNullExponent() const { return biExponent < getZeroExponent(); }
   
   typename TypeTraits::Exponent queryExponent() const
      {  typename TypeTraits::Exponent biResult = getZeroExponent();
         if (biExponent >= biResult) {
            biResult = biExponent;
            biResult -= getZeroExponent();
         }
         else
            biResult -= biExponent;
         return biResult;
      }
   void setBasicExponent(const typename TypeTraits::Exponent& biExponentSource) { biExponent = biExponentSource; }
   void setInfty() { fNegative = false; biExponent = getInftyExponent(); biMantissa = 0U; }
   void setZero() { fNegative = false; biExponent = 0U; biMantissa = 0U; }
   void setOne() { fNegative = false; biExponent = getZeroExponent(); biMantissa = 0U; }
   void setExponent(const typename TypeTraits::Exponent& biExponentSource, bool fNegative = false)
      {  if (!fNegative) {
            biExponent = biExponentSource;
            biExponent += getZeroExponent();
         }
         else {
            biExponent = getZeroExponent();
            biExponent -= biExponentSource;
         };
      }
   bool isPositive() const {  return !fNegative; }
   bool isNegative() const {  return fNegative; }
   void setSign(bool fPositive) {  fNegative = !fPositive; }
   thisType& opposite() { fNegative = !fNegative; return *this; }
   void setPositive() { fNegative = false; }
   void setNegative() { fNegative = true; }

   bool isInfty() const
      {  return (biExponent == getInftyExponent()) && biMantissa.isZero(); }
   bool isNaN() const
      {  return (biExponent == getInftyExponent()) && !biMantissa.isZero(); }
   bool isSNaN() const
      {  return (biExponent == getInftyExponent()) && !biMantissa.isZero()
            && !biMantissa.cbitArray(bitSizeMantissa()-1);
      }
   bool isQNaN() const
      {  return (biExponent == getInftyExponent())
            && biMantissa.cbitArray(bitSizeMantissa()-1);
      }
   bool isNormalized() const
      {  return (!biExponent.isZero()) && (biExponent != getInftyExponent()); }
   bool isDenormalized() const { return biExponent.isZero() && !biMantissa.isZero(); }
   bool isZero() const { return biExponent.isZero() && biMantissa.isZero(); }

   thisType queryEpsilon() const
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

   typedef typename TypeTraits::Exponent Exponent;
   typedef typename TypeTraits::Mantissa Mantissa;
   const Mantissa& queryMantissa() const { return biMantissa; }
   Mantissa& querySMantissa() { return biMantissa; }

   typedef typename TypeTraits::DiffDouble DiffDouble;
#ifndef __BORLANDC__
   DiffDouble queryNumberOfFloatsBetween(const thisType& bdSource) const;
#else
   void retrieveNumberOfFloatsBetween(const thisType& bdSource, DiffDouble& ddResult) const;
   DiffDouble queryNumberOfFloatsBetween(const thisType& bdSource) const
      {  DiffDouble ddResult;
         retrieveNumberOfFloatsBetween(bdSource, ddResult);
         return ddResult;
      }
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
   thisType& multAssign(const thisType& bdSource, StatusAndControlFlags& scfFlags)
      {  return multAssign(bdSource, MultParameters(scfFlags)); }
   thisType& multAndAddAssign(const thisType& bdMult, const thisType& bdAdd, StatusAndControlFlags& scfFlags)
      {  return multAssign(bdMult, MultParameters(scfFlags).setAdd(bdAdd)); }
   thisType& multAndSubAssign(const thisType& bdMult, const thisType& bdAdd, StatusAndControlFlags& scfFlags)
      {  return multAssign(bdMult, MultParameters(scfFlags).setAdd(bdAdd).setNegativeAdditive()); }
   thisType& multNegativeAndAddAssign(const thisType& bdMult, const thisType& bdAdd, StatusAndControlFlags& scfFlags)
      {  return multAssign(bdMult, MultParameters(scfFlags).setAdd(bdAdd).setNegativeMult()); }
   thisType& multNegativeAndSubAssign(const thisType& bdMult, const thisType& bdAdd, StatusAndControlFlags& scfFlags)
      {  return multAssign(bdMult, MultParameters(scfFlags).setAdd(bdAdd).setNegativeMult().setNegativeAdditive()); }
   thisType& multAssign(const thisType& bdSource, const MultParameters& mpParams);
   thisType& divAssign(unsigned int uValue, StatusAndControlFlags& scfFlags);
   thisType& divAssign(const thisType& bdSource, StatusAndControlFlags& scfFlags);

   thisType& operator+=(const thisType& bdSource)
      {  return plusAssign(bdSource, StatusAndControlFlags().setNearestRound()); }
   thisType operator+(const thisType& bdSource) const
      {  thisType bdThis = *this;
         bdThis.plusAssign(bdSource, StatusAndControlFlags().setNearestRound());
         return bdThis;
      }
   thisType& operator-=(const thisType& bdSource)
      {  return minusAssign(bdSource, StatusAndControlFlags().setNearestRound()); }
   thisType operator-(const thisType& bdSource) const
      {  thisType bdThis = *this;
         bdThis.minusAssign(bdSource, StatusAndControlFlags().setNearestRound());
         return bdThis;
      }
   thisType& operator*=(const thisType& bdSource)
      {  return multAssign(bdSource, StatusAndControlFlags().setNearestRound()); }
   thisType operator*(const thisType& bdSource) const
      {  thisType bdThis = *this;
         bdThis.multAssign(bdSource, StatusAndControlFlags().setNearestRound());
         return bdThis;
      }
   thisType& operator/=(const thisType& bdSource)
      {  return divAssign(bdSource, StatusAndControlFlags().setNearestRound()); }
   thisType operator/(const thisType& bdSource) const
      {  thisType bdThis = *this;
         bdThis.divAssign(bdSource, StatusAndControlFlags().setNearestRound());
         return bdThis;
      }
   void read(std::istream& isIn, StatusAndControlFlags& scfFlags);
   void write(std::ostream& osOut, const WriteParameters& wpParams) const;
   void writeDecimal(std::ostream& osOut) const;

   void clear()
      {  biMantissa.clear();
         biExponent.clear();
         fNegative = false;
      }
   void swap(thisType& bdSource)
      {  biMantissa.swap(bdSource.biMantissa);
         biExponent.swap(bdSource.biExponent);
         register bool fTemp = fNegative;
         fNegative = bdSource.fNegative;
         bdSource.fNegative = fTemp;
      }
};

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
   Errors() : uErrors(0) {}
   Errors(const Errors& eSource) : uErrors(eSource.uErrors) {}
   Errors& operator=(const Errors& eSource) { uErrors = eSource.uErrors; return *this; }

#define DefineIsError(TypeError) \
   bool is##TypeError() const { return (bool) (uErrors & (1U << T##TypeError)); }
   
   DefineIsError(PositiveOverflow)
   DefineIsError(NegativeOverflow)
   DefineIsError(QNaN)
   DefineIsError(SNaN)
   DefineIsError(PositiveUnderflow)
   DefineIsError(NegativeUnderflow)
#undef DefineIsError
   bool isNaN() const { return (bool) (uErrors & (3U << TQNaN)); }

#define DefineSetError(TypeError) \
   Errors& set##TypeError() { uErrors |= (1U << T##TypeError); return *this; }
   
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
   static double max()              { return DBL_MAX; }
   static double normalizedMin()    { return DBL_MIN; }
   static double denormalizedMin()  { return DBL_MIN*DBL_EPSILON; }
   static bool isBigEndian()
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
   static bool isLittleEndian() { return !isBigEndian(); }

   static const int UBitSizeMantissa = DBL_MANT_DIG-1;
   static const int UByteSizeImplantation = sizeof(TypeFloat);
   static const int UBitSizeExponent = UByteSizeImplantation*8-DBL_MANT_DIG;
   static unsigned int getMaxExponent() { return DBL_MAX_EXP-1; }
   static unsigned int getZeroExponent() { return DBL_MAX_EXP-1; }
   static double epsilon() { return DBL_EPSILON; }
   static bool isValid() { return (UBitSizeExponent <= (int) sizeof(int)*8); }
};

class FloatTraits {
  public:
   typedef float TypeFloat;
   static float max()              { return FLT_MAX; }
   static float normalizedMin()    { return FLT_MIN; }
   static float denormalizedMin()  { return FLT_MIN*FLT_EPSILON; }
   static bool isBigEndian()
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

   inline static bool isLittleEndian() { return !isBigEndian(); }

   static const int UBitSizeMantissa = FLT_MANT_DIG-1;
   static const int UByteSizeImplantation = sizeof(TypeFloat);
   static const int UBitSizeExponent = UByteSizeImplantation*8-FLT_MANT_DIG;
   inline static unsigned int getMaxExponent() { return FLT_MAX_EXP-1; }
   inline static unsigned int getZeroExponent() { return FLT_MAX_EXP-1; }
   inline static float epsilon() { return FLT_EPSILON; }
   inline static bool isValid() { return (UBitSizeExponent <= (int) sizeof(int)*8); }
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
   TFloatingBase() : dDouble(0.0) {}
   TFloatingBase(const Implantation& dDoubleSource) : dDouble(dDoubleSource) {}
   TFloatingBase(const BuiltDouble& bdDouble) : dDouble(0.0) { operator=(bdDouble); }
   TFloatingBase(const thisType& sSource) : dDouble(sSource.dDouble) {}
   thisType& operator=(const thisType& dsSource)
      {  dDouble = dsSource.dDouble; return *this; }
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

   operator BuiltDouble() const
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
   
   Implantation& implantation() { return dDouble; } 
   const Implantation& implantation() const { return dDouble; } 
   thisType queryEpsilon() const
      {  Implantation dtiResult = implantation()*TypeTraits::epsilon();
         if ((dtiResult == 0.0) && (implantation() != 0.0))
            return thisType(TypeTraits::denormalizedMin());
         return thisType((dtiResult >= 0.0) ? dtiResult : -dtiResult);
      }
   
   bool isZero() const { return (dDouble == 0.0) || (-dDouble == 0.0); }
   
   typedef Details::DTDoubleElement::Access::StatusAndControlFlags StatusAndControlFlags;
   thisType& plusAssign(const thisType& ttSource, const StatusAndControlFlags& scfFlags);
   thisType& minusAssign(const thisType& ttSource, const StatusAndControlFlags& scfFlags);
   thisType& multAssign(const thisType& ttSource, const StatusAndControlFlags& scfFlags);
   thisType& divAssign(const thisType& ttSource, const StatusAndControlFlags& scfFlags);

   thisType& operator-=(const thisType& ttSource)
      {  dDouble -= ttSource.dDouble; return *this; }
   thisType& operator*=(const thisType& ttSource)
      {  dDouble *= ttSource.dDouble; return *this; }
   thisType& operator+=(const thisType& ttSource)
      {  dDouble += ttSource.dDouble; return *this; }
   thisType& operator/=(const thisType& ttSource)
      {  dDouble /= ttSource.dDouble; return *this; }

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

   static unsigned int getMaxExponent() { return TypeTraits::getMaxExponent(); }
   static unsigned int getZeroExponent() { return TypeTraits::getZeroExponent(); }
   thisType& opposite() { dDouble = -dDouble; return *this; }
 
   unsigned int queryBasicExponent() const;
   int queryExponent() const
      {  return ((int) queryBasicExponent()) - getZeroExponent(); }
   void setBasicExponent(unsigned int uExponent);
   void setExponent(int uExponent)
      {  setBasicExponent(uExponent+getZeroExponent()); }
   bool isPositive() const
      {  unsigned char auDouble[UByteSizeImplantation];
         memcpy(auDouble, &dDouble, UByteSizeImplantation);
         unsigned char* pcMask = (unsigned char*) auDouble;
         if (TypeTraits::isLittleEndian())
            pcMask += UByteSizeImplantation-1;
         return !((*pcMask) & 0x80);
      }
   bool isNegative() const { return !isPositive(); }
   void setSign(bool fPositive)
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
   void setPositive() { setSign(true); }
   void setNegative() { setSign(false); }
   bool isNormalised() const
      {  return ((TypeTraits::normalizedMin() <= dDouble) && (dDouble <= TypeTraits::max()))
            || ((-TypeTraits::normalizedMin() >= dDouble) && (dDouble >= -TypeTraits::max()));
      }
   bool isRanged() const
      {  return ((TypeTraits::denormalizedMin() <= dDouble) && (dDouble <= TypeTraits::max()))
            || ((-TypeTraits::denormalizedMin() >= dDouble) && (dDouble >= -TypeTraits::max()));
      }
   void clear() { dDouble = 0.0; }
   void swap(thisType& dSource)
      {  Implantation dTemp = dDouble;
         dDouble = dSource.dDouble;
         dSource.dDouble = dTemp;
      }
};

template <class TypeTraits>
inline TFloatingBase<TypeTraits>&
TFloatingBase<TypeTraits>::plusAssign(const thisType& ttSource, const StatusAndControlFlags& scfFlags) {  
#ifdef DefineComputeParameters
#if defined(_MSC_VER) || defined(__BORLANDC__)
   int uOldState = _controlfp(0, _MCW_RC);
   _controlfp(_RC_DOWN, _MCW_RC);
   double dMin = dDouble+ttSource.dDouble;
   _controlfp(_RC_UP, _MCW_RC);
   double dMax = dDouble+ttSource.dDouble;
   if (scfFlags.isLowestRound())
      _controlfp(_RC_DOWN, _MCW_RC);
   else if (scfFlags.isHighestRound())
      _controlfp(_RC_UP, _MCW_RC);
   else if (scfFlags.isNearestRound())
      _controlfp(_RC_NEAR, _MCW_RC);
   else // (scfFlags.isZeroRound())
      _controlfp(_RC_CHOP, _MCW_RC);
   dDouble += ttSource.dDouble;
   _controlfp(uOldState, _MCW_RC);
#else
#ifdef __SUN__
   fpsetround(FP_RM);
   double dMin = dDouble+ttSource.dDouble;
   fpsetround(FP_RP);
   double dMax = dDouble+ttSource.dDouble;
   if (scfFlags.isLowestRound())
      fpsetround(FP_RM);
   else if (scfFlags.isHighestRound())
      fpsetround(FP_RP);
   else if (scfFlags.isNearestRound())
      fpsetround(FP_RN);
   else // (scfFlags.isZeroRound())
      fpsetround(FP_RZ);
   dDouble += ttSource.dDouble;
   fpsetround(FP_RN);
#else
#ifndef __CYGWIN__
   fesetround(FE_DOWNWARD);
   double dMin = dDouble+ttSource.dDouble;
   fesetround(FE_UPWARD);
   double dMax = dDouble+ttSource.dDouble;
   if (scfFlags.isLowestRound())
      fesetround(FE_DOWNWARD);
   else if (scfFlags.isHighestRound())
      fesetround(FE_UPWARD);
   else if (scfFlags.isNearestRound())
      fesetround(FE_TONEAREST);
   else // (scfFlags.isZeroRound())
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
         scfFlags.setDownApproximate();
      else {
         assert(dDouble == dMax);
         scfFlags.setUpApproximate();
      };
   };
#else
#if defined(_MSC_VER) || defined(__BORLANDC__)
   int uOldState = _controlfp(0, _MCW_RC);
   if (scfFlags.isLowestRound())
      _controlfp(_RC_DOWN, _MCW_RC);
   else if (scfFlags.isHighestRound())
      _controlfp(_RC_UP, _MCW_RC);
   else if (scfFlags.isNearestRound())
      _controlfp(_RC_NEAR, _MCW_RC);
   else // (scfFlags.isZeroRound())
      _controlfp(_RC_CHOP, _MCW_RC);
   dDouble += ttSource.dDouble;
   _controlfp(uOldState, _MCW_RC);
#else
#ifdef __SUN__
   if (scfFlags.isLowestRound())
      fpsetround(FP_RM);
   else if (scfFlags.isHighestRound())
      fpsetround(FP_RP);
   else if (scfFlags.isNearestRound())
      fpsetround(FP_RN);
   else // (scfFlags.isZeroRound())
      fpsetround(FP_RZ);
   dDouble += ttSource.dDouble;
   fpsetround(FP_RN);
#else
#ifndef __CYGWIN__
   if (scfFlags.isLowestRound())
      fesetround(FE_DOWNWARD);
   else if (scfFlags.isHighestRound())
      fesetround(FE_UPWARD);
   else if (scfFlags.isNearestRound())
      fesetround(FE_TONEAREST);
   else // (scfFlags.isZeroRound())
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
TFloatingBase<TypeTraits>::minusAssign(const thisType& ttSource, const StatusAndControlFlags& scfFlags) {  
#ifdef DefineComputeParameters
#if defined(_MSC_VER) || defined(__BORLANDC__)
   int uOldState = _controlfp(0, _MCW_RC);
   _controlfp(_RC_DOWN, _MCW_RC);
   double dMin = dDouble-ttSource.dDouble;
   _controlfp(_RC_UP, _MCW_RC);
   double dMax = dDouble-ttSource.dDouble;
   if (scfFlags.isLowestRound())
      _controlfp(_RC_DOWN, _MCW_RC);
   else if (scfFlags.isHighestRound())
      _controlfp(_RC_UP, _MCW_RC);
   else if (scfFlags.isNearestRound())
      _controlfp(_RC_NEAR, _MCW_RC);
   else // (scfFlags.isZeroRound())
      _controlfp(_RC_CHOP, _MCW_RC);
   dDouble -= ttSource.dDouble;
   _controlfp(uOldState, _MCW_RC);
#else
#ifdef __SUN__
   fpsetround(FP_RM);
   double dMin = dDouble-ttSource.dDouble;
   fpsetround(FP_RP);
   double dMax = dDouble-ttSource.dDouble;
   if (scfFlags.isLowestRound())
      fpsetround(FP_RM);
   else if (scfFlags.isHighestRound())
      fpsetround(FP_RP);
   else if (scfFlags.isNearestRound())
      fpsetround(FP_RN);
   else // (scfFlags.isZeroRound())
      fpsetround(FP_RZ);
   dDouble -= ttSource.dDouble;
   fpsetround(FP_RN);
#else
#ifndef __CYGWIN__
   fesetround(FE_DOWNWARD);
   double dMin = dDouble-ttSource.dDouble;
   fesetround(FE_UPWARD);
   double dMax = dDouble-ttSource.dDouble;
   if (scfFlags.isLowestRound())
      fesetround(FE_DOWNWARD);
   else if (scfFlags.isHighestRound())
      fesetround(FE_UPWARD);
   else if (scfFlags.isNearestRound())
      fesetround(FE_TONEAREST);
   else // (scfFlags.isZeroRound())
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
         scfFlags.setDownApproximate();
      else {
         assert(dDouble == dMax);
         scfFlags.setUpApproximate();
      };
   };
#else
#if defined(_MSC_VER) || defined(__BORLANDC__)
   int uOldState = _controlfp(0, _MCW_RC);
   if (scfFlags.isLowestRound())
      _controlfp(_RC_DOWN, _MCW_RC);
   else if (scfFlags.isHighestRound())
      _controlfp(_RC_UP, _MCW_RC);
   else if (scfFlags.isNearestRound())
      _controlfp(_RC_NEAR, _MCW_RC);
   else // (scfFlags.isZeroRound())
      _controlfp(_RC_CHOP, _MCW_RC);
   dDouble -= ttSource.dDouble;
   _controlfp(uOldState, _MCW_RC);
#else
#ifdef __SUN__
   if (scfFlags.isLowestRound())
      fpsetround(FP_RM);
   else if (scfFlags.isHighestRound())
      fpsetround(FP_RP);
   else if (scfFlags.isNearestRound())
      fpsetround(FP_RN);
   else // (scfFlags.isZeroRound())
      fpsetround(FP_RZ);
   dDouble -= ttSource.dDouble;
   fpsetround(FP_RN);
#else
#ifndef __CYGWIN__
   if (scfFlags.isLowestRound())
      fesetround(FE_DOWNWARD);
   else if (scfFlags.isHighestRound())
      fesetround(FE_UPWARD);
   else if (scfFlags.isNearestRound())
      fesetround(FE_TONEAREST);
   else // (scfFlags.isZeroRound())
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
TFloatingBase<TypeTraits>::multAssign(const thisType& ttSource, const StatusAndControlFlags& scfFlags) {  
#ifdef DefineComputeParameters
#if defined(_MSC_VER) || defined(__BORLANDC__)
   int uOldState = _controlfp(0, _MCW_RC);
   _controlfp(_RC_DOWN, _MCW_RC);
   double dMin = dDouble*ttSource.dDouble;
   _controlfp(_RC_UP, _MCW_RC);
   double dMax = dDouble*ttSource.dDouble;
   if (scfFlags.isLowestRound())
      _controlfp(_RC_DOWN, _MCW_RC);
   else if (scfFlags.isHighestRound())
      _controlfp(_RC_UP, _MCW_RC);
   else if (scfFlags.isNearestRound())
      _controlfp(_RC_NEAR, _MCW_RC);
   else // (scfFlags.isZeroRound())
      _controlfp(_RC_CHOP, _MCW_RC);
   dDouble *= ttSource.dDouble;
   _controlfp(uOldState, _MCW_RC);
#else
#ifdef __SUN__
   fpsetround(FP_RM);
   double dMin = dDouble*ttSource.dDouble;
   fpsetround(FP_RP);
   double dMax = dDouble*ttSource.dDouble;
   if (scfFlags.isLowestRound())
      fpsetround(FP_RM);
   else if (scfFlags.isHighestRound())
      fpsetround(FP_RP);
   else if (scfFlags.isNearestRound())
      fpsetround(FP_RN);
   else // (scfFlags.isZeroRound())
      fpsetround(FP_RZ);
   dDouble *= ttSource.dDouble;
   fpsetround(FP_RN);
#else
#ifndef __CYGWIN__
   fesetround(FE_DOWNWARD);
   double dMin = dDouble*ttSource.dDouble;
   fesetround(FE_UPWARD);
   double dMax = dDouble*ttSource.dDouble;
   if (scfFlags.isLowestRound())
      fesetround(FE_DOWNWARD);
   else if (scfFlags.isHighestRound())
      fesetround(FE_UPWARD);
   else if (scfFlags.isNearestRound())
      fesetround(FE_TONEAREST);
   else // (scfFlags.isZeroRound())
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
         scfFlags.setDownApproximate();
      else {
         assert(dDouble == dMax);
         scfFlags.setUpApproximate();
      };
   };
#else
#if defined(_MSC_VER) || defined(__BORLANDC__)
   int uOldState = _controlfp(0, _MCW_RC);
   if (scfFlags.isLowestRound())
      _controlfp(_RC_DOWN, _MCW_RC);
   else if (scfFlags.isHighestRound())
      _controlfp(_RC_UP, _MCW_RC);
   else if (scfFlags.isNearestRound())
      _controlfp(_RC_NEAR, _MCW_RC);
   else // (scfFlags.isZeroRound())
      _controlfp(_RC_CHOP, _MCW_RC);
   dDouble *= ttSource.dDouble;
   _controlfp(uOldState, _MCW_RC);
#else
#ifdef __SUN__
   if (scfFlags.isLowestRound())
      fpsetround(FP_RM);
   else if (scfFlags.isHighestRound())
      fpsetround(FP_RP);
   else if (scfFlags.isNearestRound())
      fpsetround(FP_RN);
   else // (scfFlags.isZeroRound())
      fpsetround(FP_RZ);
   dDouble *= ttSource.dDouble;
   fpsetround(FP_RN);
#else
#ifndef __CYGWIN__
   if (scfFlags.isLowestRound())
      fesetround(FE_DOWNWARD);
   else if (scfFlags.isHighestRound())
      fesetround(FE_UPWARD);
   else if (scfFlags.isNearestRound())
      fesetround(FE_TONEAREST);
   else // (scfFlags.isZeroRound())
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
TFloatingBase<TypeTraits>::divAssign(const thisType& ttSource, const StatusAndControlFlags& scfFlags) {  
#ifdef DefineComputeParameters
#if defined(_MSC_VER) || defined(__BORLANDC__)
   int uOldState = _controlfp(0, _MCW_RC);
   _controlfp(_RC_DOWN, _MCW_RC);
   double dMin = dDouble/ttSource.dDouble;
   _controlfp(_RC_UP, _MCW_RC);
   double dMax = dDouble/ttSource.dDouble;
   if (scfFlags.isLowestRound())
      _controlfp(_RC_DOWN, _MCW_RC);
   else if (scfFlags.isHighestRound())
      _controlfp(_RC_UP, _MCW_RC);
   else if (scfFlags.isNearestRound())
      _controlfp(_RC_NEAR, _MCW_RC);
   else // (scfFlags.isZeroRound())
      _controlfp(_RC_CHOP, _MCW_RC);
   dDouble /= ttSource.dDouble;
   _controlfp(uOldState, _MCW_RC);
#else
#ifdef __SUN__
   fpsetround(FP_RM);
   double dMin = dDouble/ttSource.dDouble;
   fpsetround(FP_RP);
   double dMax = dDouble/ttSource.dDouble;
   if (scfFlags.isLowestRound())
      fpsetround(FP_RM);
   else if (scfFlags.isHighestRound())
      fpsetround(FP_RP);
   else if (scfFlags.isNearestRound())
      fpsetround(FP_RN);
   else // (scfFlags.isZeroRound())
      fpsetround(FP_RZ);
   dDouble /= ttSource.dDouble;
   fpsetround(FP_RN);
#else
#ifndef __CYGWIN__
   fesetround(FE_DOWNWARD);
   double dMin = dDouble/ttSource.dDouble;
   fesetround(FE_UPWARD);
   double dMax = dDouble/ttSource.dDouble;
   if (scfFlags.isLowestRound())
      fesetround(FE_DOWNWARD);
   else if (scfFlags.isHighestRound())
      fesetround(FE_UPWARD);
   else if (scfFlags.isNearestRound())
      fesetround(FE_TONEAREST);
   else // (scfFlags.isZeroRound())
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
         scfFlags.setDownApproximate();
      else {
         assert(dDouble == dMax);
         scfFlags.setUpApproximate();
      };
   };
#else
#if defined(_MSC_VER) || defined(__BORLANDC__)
   int uOldState = _controlfp(0, _MCW_RC);
   if (scfFlags.isLowestRound())
      _controlfp(_RC_DOWN, _MCW_RC);
   else if (scfFlags.isHighestRound())
      _controlfp(_RC_UP, _MCW_RC);
   else if (scfFlags.isNearestRound())
      _controlfp(_RC_NEAR, _MCW_RC);
   else // (scfFlags.isZeroRound())
      _controlfp(_RC_CHOP, _MCW_RC);
   dDouble /= ttSource.dDouble;
   _controlfp(uOldState, _MCW_RC);
#else
#ifdef __SUN__
   if (scfFlags.isLowestRound())
      fpsetround(FP_RM);
   else if (scfFlags.isHighestRound())
      fpsetround(FP_RP);
   else if (scfFlags.isNearestRound())
      fpsetround(FP_RN);
   else // (scfFlags.isZeroRound())
      fpsetround(FP_RZ);
   dDouble /= ttSource.dDouble;
   fpsetround(FP_RN);
#else
#ifndef __CYGWIN__
   if (scfFlags.isLowestRound())
      fesetround(FE_DOWNWARD);
   else if (scfFlags.isHighestRound())
      fesetround(FE_UPWARD);
   else if (scfFlags.isNearestRound())
      fesetround(FE_TONEAREST);
   else // (scfFlags.isZeroRound())
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
   TDoubleElement() : inherited() {}
   TDoubleElement(int iValue, StatusAndControlFlags& scfFlags)
      {  typename BuiltDouble::IntConversion icValue;
         icValue.setSigned().assign(iValue);
         BuiltDouble bdDouble(icValue, scfFlags);
         operator=(bdDouble);
      }
   TDoubleElement(typename FloatingBaseTraits::Implantation fbtSource) : inherited(fbtSource) {}
   TDoubleElement(const BuiltDouble& bdDouble) : inherited(bdDouble) {}
   TDoubleElement(const thisType& dSource) : inherited(dSource) {}

   int queryInteger(StatusAndControlFlags& scfFlags) const
      {  typename BuiltDouble::IntConversion icResult;
         BuiltDouble(*this).retrieveInteger(icResult.setSigned(), scfFlags);
         return icResult.queryInt();
      }
   unsigned int queryUnsignedInteger(StatusAndControlFlags& scfFlags) const
      {  typename BuiltDouble::IntConversion icResult;
         BuiltDouble(*this).retrieveInteger(icResult.setUnsigned(), scfFlags);
         return icResult.queryUnsignedInt();
      }

   bool isInternZero() const;
   bool isZero() const { return inherited::isZero(); }
   bool isNaN() const;
   bool isQNaN() const;
   bool isSNaN() const;
   bool isInfty() const;
   bool hasInftyExponent() const
      {  return inherited::queryBasicExponent() == inherited::getMaxExponent()+inherited::getZeroExponent()+1; }

   enum ComparisonResult { CRNaN, CRLess, CREqual, CRGreater };
   ComparisonResult compare(const thisType& bdSource) const
      {  ComparisonResult crResult = CRNaN;
         if (!isNaN() && !bdSource.isNaN())
            crResult = (inherited::implantation() < bdSource.inherited::implantation()) ? CRLess
               : ((inherited::implantation() > bdSource.inherited::implantation()) ? CRGreater : CREqual);
         return crResult;
      }
   bool operator==(const thisType& bdSource) const { return compare(bdSource) == CREqual; }
   bool operator!=(const thisType& bdSource) const
      {  register ComparisonResult crResult = compare(bdSource);
         return (crResult == CRLess) || (crResult == CRGreater);
      }
   bool operator<(const thisType& bdSource) const { return compare(bdSource) == CRLess; }
   bool operator>(const thisType& bdSource) const { return compare(bdSource) == CRGreater; }
   bool operator<=(const thisType& bdSource) const
      {  register ComparisonResult crResult = compare(bdSource);
         return (crResult == CRLess) || (crResult == CREqual);
      }
   bool operator>=(const thisType& bdSource) const
      {  register ComparisonResult crResult = compare(bdSource);
         return (crResult == CRGreater) || (crResult == CREqual);
      }

   static const int UByteSizeImplantation = inherited::UByteSizeImplantation;
   static const int UBitSizeMantissa = inherited::UBitSizeMantissa;
   static const int UBitSizeExponent = inherited::UBitSizeExponent;

   typedef typename BuiltDouble::DiffDouble DiffDouble;
   DiffDouble queryNumberOfFloatsBetween(const thisType& deElement) const
      {  BuiltDouble bdThis = *this, bdElement = deElement;
         return bdThis.queryNumberOfFloatsBetween(bdElement);
      }
   thisType queryNthSuccessor(const DiffDouble& biIntSource) const
      {  BuiltDouble bdThis = *this;
         return thisType(bdThis.queryNthSuccessor(biIntSource));
      }
   thisType queryNthPredecessor(const DiffDouble& biIntSource) const
      {  BuiltDouble bdThis = *this;
         return thisType(bdThis.queryNthPredecessor(biIntSource));
      }
   bool isPuiss2() const;
   Errors queryErrors() const;

   thisType& opposite() { return (thisType&) inherited::opposite(); }
   thisType& plusAssign(const thisType& ttSource, StatusAndControlFlags& scfFlags)
      {  return (thisType&) inherited::plusAssign(ttSource, scfFlags); }
   thisType& minusAssign(const thisType& ttSource, StatusAndControlFlags& scfFlags)
      {  return (thisType&) inherited::minusAssign(ttSource, scfFlags); }
   thisType& multAssign(const thisType& ttSource, StatusAndControlFlags& scfFlags)
      {  return (thisType&) inherited::multAssign(ttSource, scfFlags); }
   thisType& divAssign(const thisType& ttSource, StatusAndControlFlags& scfFlags)
      {  return (thisType&) inherited::divAssign(ttSource, scfFlags); }

   thisType& operator-=(const thisType& ttSource)
      {  return (thisType&) inherited::operator-=(ttSource); }
   thisType& operator*=(const thisType& ttSource)
      {  return (thisType&) inherited::operator*=(ttSource); }
   thisType& operator+=(const thisType& ttSource)
      {  return (thisType&) inherited::operator+=(ttSource); }
   thisType& operator/=(const thisType& ttSource)
      {  return (thisType&) inherited::operator/=(ttSource); }

   thisType operator-(const thisType& ttSource) const
      {  thisType ttResult(*this);
         return (ttResult -= ttSource);
      }
   thisType operator+(const thisType& ttSource) const
      {  thisType ttResult(*this);
         return (ttResult += ttSource);
      }
   thisType operator*(const thisType& ttSource) const
      {  thisType ttResult(*this);
         return (ttResult *= ttSource);
      }
   thisType operator/(const thisType& ttSource) const
      {  thisType ttResult(*this);
         return (ttResult /= ttSource);
      }

   thisType& setOpposite()
      {  inherited::setSign(!inherited::isPositive());
         return *this;
      }
   thisType& setInftyExponent()
      {  inherited::setBasicExponent(-1);
         return *this;
      }

   void write(std::ostream& osOut, const WriteParameters& wpParams) const;
   void read(std::istream& isIn, StatusAndControlFlags& scfFlags);
};

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
} // end of namespace util
} // end of namespace unisim

#endif // Numerics_Double_FloatingH

