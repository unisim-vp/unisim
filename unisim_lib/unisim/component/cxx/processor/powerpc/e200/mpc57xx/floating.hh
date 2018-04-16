/*
 *  Copyright (c) 2007,
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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_FLOATING_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_FLOATING_HH__

#include <unisim/kernel/service/service.hh>
#include <unisim/util/simfloat/floating.hh>
#include <unisim/service/interfaces/register.hh>

#include <inttypes.h>
#include <string>

#ifdef powerpc
#undef powerpc
#endif

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace e200 {
namespace mpc57xx {

static const unsigned int RN_NEAREST = 0;
static const unsigned int RN_ZERO = 1;
static const unsigned int RN_UP = 2;
static const unsigned int RN_DOWN = 3;

class Flags
{
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
  bool fRoundToEven;
  bool fUpApproximateInfty;
      
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
  Flags()
    :  fRoundToEven(true), fUpApproximateInfty(false), rmRound(RMNearest),
       fKeepSignalingConversion(true), aApproximation(AExact), rrReadResult(RRTotal),
       fEffectiveRoundToEven(false), fSNaNOperand(false), qnrQNaNResult(QNNRUndefined),
       feExcept(FENoException), fDivisionByZero(false) {}
  Flags(const Flags& rpSource)
    :  fRoundToEven(rpSource.fRoundToEven), fUpApproximateInfty(rpSource.fUpApproximateInfty),
       rmRound(rpSource.rmRound), fKeepSignalingConversion(rpSource.fKeepSignalingConversion),
       aApproximation(rpSource.aApproximation), rrReadResult(rpSource.rrReadResult),
       fEffectiveRoundToEven(rpSource.fEffectiveRoundToEven), fSNaNOperand(rpSource.fSNaNOperand), qnrQNaNResult(rpSource.qnrQNaNResult),
       feExcept(rpSource.feExcept), fDivisionByZero(rpSource.fDivisionByZero) {}

  struct RoundingMode { RoundingMode(unsigned int rm) : mode(rm) {} unsigned int mode; };
      
  Flags( RoundingMode const& rm )
    :  fRoundToEven(true), fUpApproximateInfty(false), rmRound(RMNearest),
       fKeepSignalingConversion(true), aApproximation(AExact), rrReadResult(RRTotal),
       fEffectiveRoundToEven(false), fSNaNOperand(false), qnrQNaNResult(QNNRUndefined),
       feExcept(FENoException), fDivisionByZero(false)
  { setRoundingMode( rm.mode ); }
      
  Flags& operator=(const Flags& rpSource)
  {  aApproximation = rpSource.aApproximation;
    rrReadResult = rpSource.rrReadResult;
    fEffectiveRoundToEven = rpSource.fEffectiveRoundToEven;
    fSNaNOperand = rpSource.fSNaNOperand;
    qnrQNaNResult = rpSource.qnrQNaNResult;
    feExcept = rpSource.feExcept;
    fDivisionByZero = rpSource.fDivisionByZero;
    return *this;
  }
  bool isRoundToEven() const { return fRoundToEven && isNearestRound(); }
  bool isPositiveZeroMAdd() { return true; }
  bool isInftyAvoided() const { return true; }
  bool isAllInftyAvoided() const { return true; }
  bool doesAvoidInfty(bool fNegative) const { return true; }
  bool isDenormalizedAvoided() const { return true; }
  bool keepNaNSign() const { return false; }
  bool produceMultNaNPositive() const { return false; }
  bool produceDivNaNPositive() const { return false; }
  bool produceAddNaNPositive() const { return true; }
  bool produceSubNaNPositive() const { return true; }
  bool upApproximateInfty() const { return fUpApproximateInfty; }
  bool upApproximateInversionForNear() const { return true; }
  bool chooseNaNAddBeforeMult() const { return false; }
  bool isConvertNaNNegative() const { return true; }
  bool acceptMinusZero() const { return true; }

  void setRoundToEven() { fRoundToEven = true; }
  void clearRoundToEven() { fRoundToEven = false; }
  void setUpApproximateInfty() { fUpApproximateInfty = true; }
  void clearUpApproximateInfty() { fUpApproximateInfty = false; }

  // dynamic read parameters
  Flags& setNearestRound()   { rmRound = RMNearest; return *this; }
  Flags& setHighestRound()   { rmRound = RMHighest; return *this; }
  Flags& setLowestRound()    { rmRound = RMLowest; return *this; }
  Flags& setZeroRound()      { rmRound = RMZero; return *this; }

  bool isLowestRound() const { return rmRound == RMLowest; }
  bool isNearestRound() const { return rmRound == RMNearest; }
  bool isHighestRound() const { return rmRound == RMHighest; }
  bool isZeroRound() const { return rmRound == RMZero; }

  Flags& setKeepSignalingConversion() { fKeepSignalingConversion = true; return *this; }
  Flags& clearKeepSignalingConversion() { fKeepSignalingConversion = false; return *this; }
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
  bool hasSameApproximation(const Flags& rpSource) const
  {  return aApproximation == rpSource.aApproximation; }
  bool hasIncrementFraction(bool fNegative) const
  {  return fNegative ? isDownApproximate() : isUpApproximate(); }

  void setEffectiveRoundToEven() { fEffectiveRoundToEven = true; }
  void clearEffectiveRoundToEven() { fEffectiveRoundToEven = false; }
  bool hasEffectiveRoundToEven() const { return fEffectiveRoundToEven; }

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
  void clearFlowException() { feExcept = FENoException; }
  void setOverflow() { feExcept = FEOverflow; }
  void setUnderflow() { feExcept = FEUnderflow; }
  bool isOverflow() const { return feExcept == FEOverflow; }
  bool isUnderflow() const { return feExcept == FEUnderflow; }
      
  void setRoundingMode(unsigned int rn_mode)
  {  switch(rn_mode)
      {
      case RN_NEAREST:
        setNearestRound();
        break;
      case RN_ZERO:
        setZeroRound();
        break;
      case RN_UP:
        setHighestRound();
        break;
      case RN_DOWN:
        setLowestRound();
        break;
      }
  }
};

class IntConversion
{
public:
  typedef unisim::util::simfloat::Numerics::Integer::TBigCellInt<unisim::util::simfloat::Numerics::Integer::Details::TCellIntegerTraits<(64 + 1)/(8*sizeof(unsigned))> > BigInt;

private:
  BigInt biResult;
  int uSize;
  bool fUnsigned;

  void normalize()
  {  if ((uSize % (8*sizeof(unsigned))) > 0)
      biResult.array((uSize+1)/(8*sizeof(unsigned)))
        &= ~(~0U << (uSize % (8*sizeof(unsigned))));
  }

public:
  IntConversion() : uSize(32), fUnsigned(true) {}
  IntConversion(const IntConversion& source)
    :  biResult(source.biResult), uSize(source.uSize), fUnsigned(source.fUnsigned) {}

  IntConversion& setSigned() { fUnsigned = false; return *this; }
  IntConversion& setUnsigned() { fUnsigned = true; return *this; }
  void setSize(int bitSize) { assert(bitSize <= 64); uSize = bitSize; }
  IntConversion& assignSigned(int64_t value)
  {  assert(!fUnsigned); uSize = 64; memcpy(&biResult.array(0), &value, 8); return *this; }
  IntConversion& assignUnsigned(uint64_t value)
  {  assert(fUnsigned); uSize = 64; memcpy(&biResult.array(0), &value, 8); return *this; }
  IntConversion& assignSigned(int32_t value)
  {  assert(!fUnsigned); uSize = 32; memcpy(&biResult.array(0), &value, 4); return *this; }
  IntConversion& assignUnsigned(uint32_t value)
  {  assert(fUnsigned); uSize = 32; memcpy(&biResult.array(0), &value, 4); return *this; }
  IntConversion& assignSigned(int16_t value)
  {  assert(!fUnsigned); uSize = 32; memcpy(&biResult.array(0), &value, 4); return *this; }
  IntConversion& assignUnsigned(uint16_t value)
  {  assert(fUnsigned); uSize = 16; memcpy(&biResult.array(0), &value, 4); return *this; }
  IntConversion& assign(unsigned int value)
  {  assert(fUnsigned); biResult.clear(); biResult.array(0) = value; return *this; }
  IntConversion& assign(int value)
  {  assert(!fUnsigned); biResult.array(1) = -(value < 0); biResult.array(0) = value; return *this; }

  int querySize() const { return uSize; }
  int queryMaxDigits() const { return fUnsigned ? uSize : (uSize-1); }
  bool isUnsigned() const { return fUnsigned; }
  bool isSigned() const   { return !fUnsigned; }

  int64_t asInt64() const
  {  int64_t result; memcpy(&result, &const_cast<BigInt&>(biResult).array(0), 8); return result; }
  int64_t asUInt64() const
  {  uint64_t result; memcpy(&result, &const_cast<BigInt&>(biResult).array(0), 8); return result; }
  int32_t asInt32() const
  {  int32_t result; memcpy(&result, &const_cast<BigInt&>(biResult).array(0), 4); return result; }
  int32_t asUInt32() const
  {  uint32_t result; memcpy(&result, &const_cast<BigInt&>(biResult).array(0), 4); return result; }
  int16_t asInt16() const
  {  int16_t result; memcpy(&result, &const_cast<BigInt&>(biResult).array(0), 2); return result; }
  int16_t asUInt16() const
  {  uint16_t result; memcpy(&result, &const_cast<BigInt&>(biResult).array(0), 2); return result; }

  void opposite()
  {  assert(!fUnsigned);
    if (biResult.neg(uSize).inc().hasCarry())
      normalize();
  }
  bool isPositive() const { return fUnsigned || !biResult.cbitArray(uSize-1); }
  bool isNegative() const { return !fUnsigned && biResult.cbitArray(uSize-1); }
  bool isDifferentZero() const { return !biResult.isZero(); }
  int log_base_2() const { return biResult.log_base_2(); }
  bool hasZero(int digits) const { return biResult.hasZero(digits); }
  bool cbitArray(int localIndex) const { return biResult.cbitArray(localIndex); }
  IntConversion& operator>>=(int shift)
  {  assert(isPositive()); biResult >>= shift; return *this; }
  IntConversion& operator<<=(int shift)
  {  assert(isPositive()); biResult <<= shift; return *this; }
  IntConversion& operator&=(const IntConversion& source)
  {  assert(isPositive() && source.isPositive());
    biResult &= source.biResult;
    return *this;
  }
  IntConversion& neg() { biResult.neg(uSize); return *this; }
  IntConversion& inc()
  {  if (biResult.inc().hasCarry())
      normalize();
    return *this;
  }

  IntConversion& operator=(const IntConversion& source)
  {  biResult = source.biResult;
    fUnsigned = source.fUnsigned;
    return *this;
  }
  typedef BigInt::BitArray BitArray;
  BitArray bitArray(int index) { assert(isPositive()); return biResult.bitArray(index); }
  void setBitArray(int index, bool fValue)
  {  assert(isPositive()); biResult.setBitArray(index, fValue); }
  void setTrueBitArray(int index)
  {  assert(isPositive()); biResult.setTrueBitArray(index); }
  void setFalseBitArray(int index)
  {  assert(isPositive()); biResult.setFalseBitArray(index); }

  void setMin()
  {  biResult.clear();
    if (!fUnsigned)
      biResult.setTrueBitArray(uSize-1);
  }
  void setMax()
  {  biResult.clear(); 
    if (fUnsigned)
      biResult.neg(uSize);
    else
      biResult.neg(uSize-1);
  }
  unsigned int& operator[](int index) { return biResult[index]; }
  unsigned int operator[](int index) const { return biResult[index]; }
};

class BuiltDoubleTraits : public unisim::util::simfloat::Numerics::Double::BuiltDoubleTraits<52, 11>
{
  public:
   typedef Flags StatusAndControlFlags;
   class MultExtension : public unisim::util::simfloat::Numerics::Double::BuiltDoubleTraits<52, 11>::MultExtension {
     public:
      typedef Flags StatusAndControlFlags;
   };
   typedef mpc57xx::IntConversion IntConversion;
};

class SoftHalfFloat;
class SoftFloat;

class SoftDouble : public unisim::util::simfloat::Numerics::Double::TBuiltDouble<BuiltDoubleTraits>
{
private:
  typedef unisim::util::simfloat::Numerics::Double::TBuiltDouble<BuiltDoubleTraits> inherited;

public:
  SoftDouble() : inherited() {}
  SoftDouble(const SoftFloat& sfFloat, Flags& rpParams);
  SoftDouble(const SoftDouble& source) : inherited(source) {}
  enum __FromRawBits__ { FromRawBits };
  SoftDouble(__FromRawBits__, uint64_t const& source) { setChunk((void *) &source, unisim::util::endian::IsHostLittleEndian()); }
  SoftDouble& operator=(const SoftDouble& sdSource)
  { return (SoftDouble&) inherited::operator=(sdSource); }
  SoftDouble& assign(const SoftDouble& sdSource)
  { return (SoftDouble&) inherited::operator=(sdSource); }
  SoftDouble& assign(const SoftFloat& sfFloat, Flags& rpParams);

  uint64_t queryValue() const
  { uint64_t uResult; fillChunk(&uResult, unisim::util::endian::IsHostLittleEndian()); return uResult; }
};

class BuiltFloatTraits : public unisim::util::simfloat::Numerics::Double::BuiltDoubleTraits<23, 8> {
  public:
   typedef Flags StatusAndControlFlags;
   class MultExtension : public unisim::util::simfloat::Numerics::Double::BuiltDoubleTraits<23, 8>::MultExtension {
     public:
      typedef Flags StatusAndControlFlags;
   };
   typedef mpc57xx::IntConversion IntConversion;
};

class SoftFloat : public unisim::util::simfloat::Numerics::Double::TBuiltDouble<BuiltFloatTraits>
{
private:
  typedef unisim::util::simfloat::Numerics::Double::TBuiltDouble<BuiltFloatTraits> inherited;

public:
  SoftFloat() : inherited() {}
  SoftFloat(const SoftHalfFloat& sdDouble, Flags& rpParams);
  SoftFloat(const SoftDouble& sdDouble, Flags& rpParams);
  enum __FromFraction__ { FromFraction };
  SoftFloat(__FromFraction__,  int32_t source, Flags& flags);
  SoftFloat(__FromFraction__, uint32_t source, Flags& flags);
  SoftFloat( int32_t source, Flags& flags);
  SoftFloat(uint32_t source, Flags& flags);
  SoftFloat(const SoftFloat& source) : inherited(source) {}
  enum __FromRawBits__ { FromRawBits };
  SoftFloat(__FromRawBits__, uint32_t const& source) { setChunk((void *) &source, unisim::util::endian::IsHostLittleEndian()); }

  SoftFloat& operator=(const SoftFloat& sfSource)
  {  return (SoftFloat&) inherited::operator=(sfSource); }
  SoftFloat& assign(const SoftFloat& sfSource)
  {  return (SoftFloat&) inherited::operator=(sfSource); }
  SoftFloat& assign(const SoftDouble& sdDouble, Flags& rpParams);
  SoftFloat& assign(const SoftHalfFloat& sdDouble, Flags& rpParams);
  uint32_t queryValue() const
  {  uint32_t uResult; fillChunk(&uResult, unisim::util::endian::IsHostLittleEndian()); return uResult; }
};

class BuiltHalfFloatTraits : public unisim::util::simfloat::Numerics::Double::BuiltDoubleTraits<10, 5>
{
  public:
   typedef Flags StatusAndControlFlags;
   class MultExtension : public unisim::util::simfloat::Numerics::Double::BuiltDoubleTraits<10, 5>::MultExtension {
     public:
      typedef Flags StatusAndControlFlags;
   };
   typedef mpc57xx::IntConversion IntConversion;
};

class SoftHalfFloat : public unisim::util::simfloat::Numerics::Double::TBuiltDouble<BuiltHalfFloatTraits>
{
private:
  typedef unisim::util::simfloat::Numerics::Double::TBuiltDouble<BuiltHalfFloatTraits> inherited;

public:
  SoftHalfFloat() : inherited() {}
  SoftHalfFloat(const IntConversion& intConversion, Flags& params) : inherited(intConversion, params) {}
  SoftHalfFloat(const SoftHalfFloat& source) : inherited(source) {}
  SoftHalfFloat(const SoftFloat& sdDouble, Flags& rpParams);
  SoftHalfFloat(const SoftDouble& sdDouble, Flags& rpParams);
  SoftHalfFloat(const uint16_t& uHalfFloat) { setChunk((void *) &uHalfFloat, unisim::util::endian::IsHostLittleEndian()); }

  SoftHalfFloat& operator=(const SoftHalfFloat& sfSource)
  { return (SoftHalfFloat&) inherited::operator=(sfSource); }
  SoftHalfFloat& assign(const SoftHalfFloat& sfSource)
  { return (SoftHalfFloat&) inherited::operator=(sfSource); }
  SoftHalfFloat& assign(const SoftFloat& sdDouble, Flags& rpParams);
  uint16_t queryValue() const
  { uint16_t uResult; fillChunk(&uResult, unisim::util::endian::IsHostLittleEndian()); return uResult; }
};

inline SoftDouble&
SoftDouble::assign(const SoftFloat& sfFloat, Flags& rpParams)
{
   FloatConversion fcConversion;
   fcConversion.setSizeMantissa(23).setSizeExponent(8);
   fcConversion.setNegative(sfFloat.isNegative());
   fcConversion.exponent()[0] = sfFloat.queryBasicExponent()[0];
   fcConversion.mantissa()[0] = sfFloat.queryMantissa()[0];
   inherited source(fcConversion, rpParams);
   return (SoftDouble&) inherited::operator=(source);
}

inline
SoftDouble::SoftDouble(const SoftFloat& sfFloat, Flags& rpParams)
{
  assign(sfFloat, rpParams);
}

inline
SoftFloat::SoftFloat(int32_t source, Flags& flags)
{
  SoftFloat::IntConversion conversion;
  setInteger(conversion.setSigned().assignSigned(source), flags);
}

inline
SoftFloat::SoftFloat(uint32_t source, Flags& flags)
{
  SoftFloat::IntConversion conversion;
  setInteger(conversion.setUnsigned().assignUnsigned(source), flags);
}

inline
SoftFloat::SoftFloat(__FromFraction__, int32_t source, Flags& flags)
  : inherited()
{
  if (source == 0)
    return;
  SoftFloat::IntConversion conversion;
  setInteger(conversion.setSigned().assignSigned(source), flags);
  querySBasicExponent().sub(31);
}

inline
SoftFloat::SoftFloat(__FromFraction__, uint32_t source, Flags& flags)
{
  if (source == 0)
    return;
  SoftFloat::IntConversion conversion;
  setInteger(conversion.setUnsigned().assignUnsigned(source), flags);
  querySBasicExponent().sub(32);
}

inline SoftFloat&
SoftFloat::assign(const SoftDouble& sdDouble, Flags& rpParams)
{
  FloatConversion fcConversion;
  fcConversion.setSizeMantissa(52).setSizeExponent(11);
  fcConversion.setNegative(sdDouble.isNegative());
  fcConversion.exponent()[0] = sdDouble.queryBasicExponent()[0];
  fcConversion.mantissa()[0] = sdDouble.queryMantissa()[0];
  fcConversion.mantissa()[1] = sdDouble.queryMantissa()[1];
  return (SoftFloat&) inherited::operator=(inherited(fcConversion, rpParams));
}

inline SoftFloat&
SoftFloat::assign(const SoftHalfFloat& sdDouble, Flags& rpParams)
{
  FloatConversion fcConversion;
  fcConversion.setSizeMantissa(10).setSizeExponent(5);
  fcConversion.setNegative(sdDouble.isNegative());
  fcConversion.exponent()[0] = sdDouble.queryBasicExponent()[0];
  fcConversion.mantissa()[0] = sdDouble.queryMantissa()[0];
  return (SoftFloat&) inherited::operator=(inherited(fcConversion, rpParams));
}

inline
SoftFloat::SoftFloat(const SoftDouble& sdDouble, Flags& rpParams)
   { assign(sdDouble, rpParams); }

inline
SoftFloat::SoftFloat(const SoftHalfFloat& sdDouble, Flags& rpParams)
   { assign(sdDouble, rpParams); }

inline SoftHalfFloat&
SoftHalfFloat::assign(const SoftFloat& sdDouble, Flags& rpParams)
{
  FloatConversion fcConversion;
  fcConversion.setSizeMantissa(23).setSizeExponent(8);
  fcConversion.setNegative(sdDouble.isNegative());
  fcConversion.exponent()[0] = sdDouble.queryBasicExponent()[0];
  fcConversion.mantissa()[0] = sdDouble.queryMantissa()[0];
  return (SoftHalfFloat&) inherited::operator=(inherited(fcConversion, rpParams));
}

inline
SoftHalfFloat::SoftHalfFloat(const SoftFloat& sdDouble, Flags& rpParams)
{
  assign(sdDouble, rpParams);
}

class FloatingPointRegisterInterface : public unisim::service::interfaces::Register
{
public:
  FloatingPointRegisterInterface(const char *name, SoftDouble *value);
  virtual ~FloatingPointRegisterInterface();
  virtual const char *GetName() const;
  virtual void GetValue(void *buffer) const;
  virtual void SetValue(const void *buffer);
  virtual int GetSize() const;
private:
  std::string name;
  SoftDouble *value;
};

class FloatingPointRegisterView : public unisim::kernel::service::VariableBase
{
public:
  FloatingPointRegisterView(const char *name, unisim::kernel::service::Object *owner, SoftDouble& storage, const char *description);
  virtual ~FloatingPointRegisterView();
  virtual const char *GetDataTypeName() const;
  virtual operator bool () const;
  virtual operator long long () const;
  virtual operator unsigned long long () const;
  virtual operator double () const;
  virtual operator std::string () const;
  virtual unisim::kernel::service::VariableBase& operator = (bool value);
  virtual unisim::kernel::service::VariableBase& operator = (long long value);
  virtual unisim::kernel::service::VariableBase& operator = (unsigned long long value);
  virtual unisim::kernel::service::VariableBase& operator = (double value);
  virtual unisim::kernel::service::VariableBase& operator = (const char * value);
private:
  SoftDouble& storage;
};

} // end of namespace mpc57xx
} // end of namespace e200
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
