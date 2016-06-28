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
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_FLOATING_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_FLOATING_HH__

#include <unisim/util/simfloat/floating.hh>
#include <unisim/component/cxx/processor/arm/extregbank.hh>
#include <unisim/component/cxx/processor/arm/psr.hh>
#include <unisim/kernel/service/service.hh>
#include <unisim/service/interfaces/register.hh>

#include <inttypes.h>
#include <string>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

  struct VFPExpandImm;

namespace simfloat {
  struct SoftFloat;
  struct SoftDouble;
  
  struct Flags
  {
    Flags()
      : fRoundToEven(true), fUpApproximateInfty(false), rmRound(RMNearest)
      , fKeepSignalingConversion(true), aApproximation(AExact), rrReadResult(RRTotal)
      , fEffectiveRoundToEven(false), fSNaNOperand(false), qnrQNaNResult(QNNRUndefined)
      , feExcept(FENoException), fDivisionByZero(false)
    {}
    Flags(const Flags& rpSource)
      : fRoundToEven(rpSource.fRoundToEven), fUpApproximateInfty(rpSource.fUpApproximateInfty)
      , rmRound(rpSource.rmRound), fKeepSignalingConversion(rpSource.fKeepSignalingConversion)
      , aApproximation(rpSource.aApproximation), rrReadResult(rpSource.rrReadResult)
      , fEffectiveRoundToEven(rpSource.fEffectiveRoundToEven), fSNaNOperand(rpSource.fSNaNOperand)
      , qnrQNaNResult(rpSource.qnrQNaNResult), feExcept(rpSource.feExcept)
      , fDivisionByZero(rpSource.fDivisionByZero)
    {}

    Flags& operator=(const Flags& rpSource)
    {
      aApproximation = rpSource.aApproximation;
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
    bool doesAvoidInfty(bool fNegative) const {  return fNegative ? (rmRound >= RMHighest) : (rmRound & RMLowest); }
    bool keepNaNSign() const { return true; }
    bool produceMultNaNPositive() const { return true; }
    bool produceDivNaNPositive() const { return true; }
    bool produceAddNaNPositive() const { return true; }
    bool produceSubNaNPositive() const { return true; }
    bool upApproximateInfty() const { return fUpApproximateInfty; }
    bool upApproximateInversionForNear() const { return true; }
    bool chooseNaNAddBeforeMult() const { return true; }
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
    {
      aApproximation = AExact;
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
    void mergeException(const Flags& source) { if (feExcept == FENoException) feExcept = source.feExcept; }
      
    void setRoundingMode(unsigned int rn_mode)
    {
      switch(rn_mode)
        {
        case 0b00: /* Round to Nearest (RN) mode */
          setNearestRound();
          break;
        case 0b01: /* Round towards Plus Infinity (RP) mode */
          setHighestRound();
          break;
        case 0b10: /* Round towards Minus Infinity (RM) mode */
          setLowestRound();
          break;
        case 0b11: /* Round towards Zero (RZ) mode. */
          setZeroRound();
          break;
        }
    }
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

  };

  struct BuiltDoubleTraits : public unisim::util::simfloat::Numerics::Double::BuiltDoubleTraits<52, 11>
  {
    typedef Flags StatusAndControlFlags;
    
    struct MultExtension : public unisim::util::simfloat::Numerics::Double::BuiltDoubleTraits<52, 11>::MultExtension
    {
      typedef Flags StatusAndControlFlags;
    };
  };

  struct SoftDouble : public unisim::util::simfloat::Numerics::Double::TBuiltDouble<BuiltDoubleTraits>
  {
    SoftDouble() : inherited() {}
    SoftDouble(const SoftFloat& sfFloat, Flags& rpParams);
    SoftDouble(const uint64_t& uDouble) { setChunk((void *) &uDouble, unisim::util::endian::IsHostLittleEndian()); }
    SoftDouble(VFPExpandImm const& imm);
    SoftDouble& operator=(const SoftDouble& sdSource) { return (SoftDouble&) inherited::operator=(sdSource); }
    SoftDouble& assign(const SoftDouble& sdSource) { return (SoftDouble&) inherited::operator=(sdSource); }
    SoftDouble& assign(const SoftFloat& sfFloat, Flags& rpParams);
    uint64_t queryValue() const
    { uint64_t uResult; fillChunk(&uResult, unisim::util::endian::IsHostLittleEndian()); return uResult; }
    void ToBytes( uint8_t* bytes ) const;
    void FromBytes( uint8_t const* bytes );
    
  private:
    typedef unisim::util::simfloat::Numerics::Double::TBuiltDouble<BuiltDoubleTraits> inherited;
  };

  struct BuiltFloatTraits : public unisim::util::simfloat::Numerics::Double::BuiltDoubleTraits<23, 8>
  {
    typedef Flags StatusAndControlFlags;
    struct MultExtension : public unisim::util::simfloat::Numerics::Double::BuiltDoubleTraits<23, 8>::MultExtension
    {
      typedef Flags StatusAndControlFlags;
    };
  };

  struct SoftFloat : public unisim::util::simfloat::Numerics::Double::TBuiltDouble<BuiltFloatTraits>
  {
    SoftFloat() : inherited() {}
    SoftFloat(const SoftDouble& sdDouble, Flags& rpParams);
    //SoftFloat(const uint32_t& uFloat) { setChunk((void *) &uFloat, unisim::util::endian::IsHostLittleEndian()); }
    SoftFloat(VFPExpandImm const& imm);

    SoftFloat& operator=(const SoftFloat& sfSource)
    {  return (SoftFloat&) inherited::operator=(sfSource); }
    SoftFloat& assign(const SoftFloat& sfSource)
    {  return (SoftFloat&) inherited::operator=(sfSource); }
    SoftFloat& assign(const SoftDouble& sdDouble, Flags& rpParams);
    uint32_t queryValue() const
    {  uint32_t uResult; fillChunk(&uResult, unisim::util::endian::IsHostLittleEndian()); return uResult; }
    void ToBytes( uint8_t* bytes ) const;
    void FromBytes( uint8_t const* bytes );
  private:
    typedef unisim::util::simfloat::Numerics::Double::TBuiltDouble<BuiltFloatTraits> inherited;
  };

  inline SoftDouble&
  SoftDouble::assign(const SoftFloat& sfFloat, Flags& rpParams) {
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
  { assign(sfFloat, rpParams); }

  inline SoftFloat&
  SoftFloat::assign(const SoftDouble& sdDouble, Flags& rpParams) {
    FloatConversion fcConversion;
    fcConversion.setSizeMantissa(52).setSizeExponent(11);
    fcConversion.setNegative(sdDouble.isNegative());
    fcConversion.exponent()[0] = sdDouble.queryBasicExponent()[0];
    fcConversion.mantissa()[0] = sdDouble.queryMantissa()[0];
    fcConversion.mantissa()[1] = sdDouble.queryMantissa()[1];
    return (SoftFloat&) inherited::operator=(inherited(fcConversion, rpParams));
  }

  inline
  SoftFloat::SoftFloat(const SoftDouble& sdDouble, Flags& rpParams)
  { assign(sdDouble, rpParams); }

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

  struct FP
  {
    typedef SoftDouble F64;
    typedef SoftFloat  F32;
    
    template <typename SOFTDBL, typename fpscrT> static
    void SetDefaultNan( SOFTDBL& result, fpscrT const& fpscr )
    {
      result.setNegative(false);
      result.querySBasicExponent() = result.getInftyExponent();
      result.querySMantissa().clear();
      result.querySMantissa().setTrueBitArray(result.bitSizeMantissa()-1);
    }
    
    template <typename SOFTDBL, typename fpscrT> static
    void SetQuietBit( SOFTDBL& op, fpscrT const& fpscr )
    {
      op.querySMantissa().setTrueBitArray(op.bitSizeMantissa()-1);
    } 

    template <typename fpscrT> static
    void Sqrt( SoftDouble& acc, fpscrT& fpscr )
    {
      throw std::logic_error("TODO: sqrt");
    }

    template <typename fpscrT> static
    void Sqrt( SoftFloat& acc, fpscrT& fpscr )
    {
      throw std::logic_error("TODO: sqrt");
    }

    template <typename SOFTDBL, typename fpscrT> static
    int Compare( SOFTDBL const& op1, SOFTDBL const& op2, fpscrT& fpscr )
    {
      typename SOFTDBL::ComparisonResult cmp = op1.compare( op2 );
      if      (cmp == SOFTDBL::CRLess)    return -1;
      else if (cmp == SOFTDBL::CRGreater) return +1;
      return 0;
    }
    
    template <typename SOFTDBL, typename fpscrT> static
    bool IsSNaN( SOFTDBL const& op, fpscrT const& fpscr )
    {
      return op.isSNaN();
    }

    template <typename SOFTDBL, typename fpscrT> static
    bool IsQNaN( SOFTDBL const& op, fpscrT const& fpscr )
    {
      return op.isQNaN();
    }

    template <typename SOFTDBL, typename fpscrT> static
    bool FlushToZero( SOFTDBL& op, fpscrT& fpscr )
    {
      if (op.isDenormalized()) {
        op.querySMantissa().clear();
        return true;
      }
      return false;
    }

    template <typename SOFTDBL, typename fpscrT> static
    void Add( SOFTDBL& acc, SOFTDBL const& op2, fpscrT& fpscr )
    {
      Flags flags;
      flags.setRoundingMode( fpscr.Get( RMode ) );
      acc.plusAssign(op2, flags);
      // Process exceptions (Underflow, Overflow, InvalidOp, Inexact)
      if (flags.isApproximate()) {
        if (flags.hasFlowException()) {
          if      (flags.isOverflow())     fpscr.ProcessException( OFC );
          else if (flags.isUnderflow())    fpscr.ProcessException( UFC );
        }
        fpscr.ProcessException( IXC );
      }
    }
    
    template <typename SOFTDBL, typename fpscrT> static
    void Sub( SOFTDBL& acc, SOFTDBL const& op2, fpscrT& fpscr )
    {
      Flags flags;
      flags.setRoundingMode( fpscr.Get( RMode ) );
      acc.minusAssign(op2, flags);
      // Process exceptions (Underflow, Overflow, InvalidOp, Inexact)
      if (flags.isApproximate()) {
        if (flags.hasFlowException()) {
          if      (flags.isOverflow())     fpscr.ProcessException( OFC );
          else if (flags.isUnderflow())    fpscr.ProcessException( UFC );
        }
        fpscr.ProcessException( IXC );
      }
    }

    template <typename SOFTDBL, typename fpscrT> static
    void Div( SOFTDBL& acc, SOFTDBL const& op2, fpscrT& fpscr )
    {
      Flags flags;
      flags.setRoundingMode( fpscr.Get( RMode ) );
      acc.divAssign(op2, flags);
      // Process exceptions (Underflow, Overflow, InvalidOp, Inexact)
      if (flags.isApproximate()) {
        if (flags.hasFlowException()) {
          if      (flags.isOverflow())     fpscr.ProcessException( OFC );
          else if (flags.isUnderflow())    fpscr.ProcessException( UFC );
        }
        fpscr.ProcessException( IXC );
      }
    }

    template <typename SOFTDBL, typename fpscrT> static
    void Mul( SOFTDBL& acc, SOFTDBL const& op2, fpscrT& fpscr )
    {
      Flags flags;
      flags.setRoundingMode( fpscr.Get( RMode ) );
      acc.multAssign(op2, flags);
      // Process exceptions (Underflow, Overflow, InvalidOp, Inexact)
      if (flags.isApproximate()) {
        if (flags.hasFlowException()) {
          if      (flags.isOverflow())     fpscr.ProcessException( OFC );
          else if (flags.isUnderflow())    fpscr.ProcessException( UFC );
        }
        fpscr.ProcessException( IXC );
      }
    }

    template <typename SOFTDBL, typename fpscrT> static
    void MulAdd( SOFTDBL& acc, SOFTDBL const& op1, SOFTDBL const& op2, fpscrT& fpscr )
    {
      Flags flags;
      flags.setRoundingMode( fpscr.Get( RMode ) );
      
      SOFTDBL res( op1 );
      res.multAndAddAssign(op2, acc, flags);
      acc = res;
      // Process exceptions (Underflow, Overflow, InvalidOp, Inexact)
      if (flags.isApproximate()) {
        if (flags.hasFlowException()) {
          if      (flags.isOverflow())     fpscr.ProcessException( OFC );
          else if (flags.isUnderflow())    fpscr.ProcessException( UFC );
        }
        fpscr.ProcessException( IXC );
      }
    }
    
    template <typename SOFTDBL, typename fpscrT> static
    void Abs( SOFTDBL& acc, fpscrT& fpscr )
    {
      acc.setNegative(false);
    }
    
    template <typename SOFTDBL, typename fpscrT> static
    void Neg( SOFTDBL& acc, fpscrT& fpscr )
    {
      acc.opposite();
    }

    template <typename fpscrT> static
    void FtoF( SoftDouble& dst, SoftFloat const& src, fpscrT& fpscr )
    {
      Flags flags_conv;
      flags_conv.setRoundingMode( fpscr.Get( RMode ) );
      flags_conv.clearKeepSignalingConversion();
      flags_conv.setUpApproximateInfty();
      
      dst.assign(src, flags_conv);
    }

    template <typename fpscrT> static
    void FtoF( SoftFloat& dst, SoftDouble const& src, fpscrT& fpscr )
    {
      Flags flags_conv;
      flags_conv.setRoundingMode( fpscr.Get( RMode ) );
      flags_conv.clearKeepSignalingConversion();
      flags_conv.setUpApproximateInfty();
      
      dst.assign(src, flags_conv);
    }

    
    template <typename INT, typename fpscrT> static
    void FtoI( INT& dst, SoftDouble const& src, int fracbits, fpscrT& fpscr )
    {
      throw std::logic_error("TODO: ftoi");
    }

    template <typename INT, typename fpscrT> static
    void FtoI( INT& dst, SoftFloat const& src, int fracbits, fpscrT& fpscr )
    {
      throw std::logic_error("TODO: ftoi");
    }

    template <typename INT, typename fpscrT> static
    void ItoF( SoftDouble& dst, INT const& src, int fracbits, fpscrT& fpscr )
    { 
      throw std::logic_error("TODO: itof");
    }
    
    template <typename INT, typename fpscrT> static
    void ItoF( SoftFloat& dst, INT const& src, int fracbits, fpscrT& fpscr )
    {
      throw std::logic_error("TODO: itof");
    }
  };

} // end of namespace simfloat
  
  template <> struct ExtTypeInfo<simfloat::SoftFloat>
  {
    typedef simfloat::SoftFloat float_type;
    enum bytecount_t { bytecount = 4 };
    static void ToBytes( uint8_t* dst, float_type const& src ) { src.ToBytes( dst ); }
    static void FromBytes( float_type& dst, uint8_t const* src ) { dst.FromBytes( src ); }
  };

  template <> struct ExtTypeInfo<simfloat::SoftDouble>
  {
    typedef simfloat::SoftDouble float_type;
    enum bytecount_t { bytecount = 8 };
    static void ToBytes( uint8_t* dst, float_type const& src ) { src.ToBytes( dst ); }
    static void FromBytes( float_type& dst, uint8_t const* src ) { dst.FromBytes( src ); }
  };
  

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif /*  __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_FLOATING_HH__ */
