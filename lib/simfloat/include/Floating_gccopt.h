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
#include "Integer_gccopt.h"

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

#if defined (__GNUC__)
#define DefineGCC_Inline_Double_Details_DTDoubleElement_Access
#include "Floating_gccopt.inline"
#undef DefineGCC_Inline_Double_Details_DTDoubleElement_Access
#endif

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

#if defined (__GNUC__)
#define DefineGCC_Inline_Double_Details_DBuiltDoubleTraits_Access
#include "Floating_gccopt.inline"
#undef DefineGCC_Inline_Double_Details_DBuiltDoubleTraits_Access
#endif

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

#if defined (__GNUC__)
#define DefineGCC_Inline_Double_BuiltDoubleTraits
#include "Floating_gccopt.inline"
#undef DefineGCC_Inline_Double_BuiltDoubleTraits
#endif

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

#if defined (__GNUC__)
#define DefineGCC_Inline_Double_TBuiltDouble
#include "Floating_gccopt.inline"
#undef DefineGCC_Inline_Double_TBuiltDouble
#endif

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

#if defined (__GNUC__)
#define DefineGCC_Inline_Double_TFloatingBase
#include "Floating_gccopt.inline"
#undef DefineGCC_Inline_Double_TFloatingBase
#endif

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

#if defined (__GNUC__)
#define DefineGCC_Inline_Double_TDoubleElement
#include "Floating_gccopt.inline"
#undef DefineGCC_Inline_Double_TDoubleElement
#endif

}} // end of namespace Numerics::Double

#endif // Numerics_Double_FloatingH

