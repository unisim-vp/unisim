/*
 *  Copyright (c) 2009,
 *  University of Perpignan (UPVD),
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
 *   - Neither the name of UPVD nor the names of its contributors may be used to
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
 * Authors: Sylvain Collange (sylvain.collange@univ-perp.fr)
 */
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_SIMFLOAT_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_SIMFLOAT_HH__

#include <unisim/util/simfloat/floating.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

struct BuiltFloatTraits : unisim::util::simfloat::Numerics::Double::BuiltDoubleTraits<23, 8> {
};

struct SoftFloatIEEE : unisim::util::simfloat::Numerics::Double::TBuiltDouble<BuiltFloatTraits>
{
private:
	typedef unisim::util::simfloat::Numerics::Double::TBuiltDouble<BuiltFloatTraits> inherited;
public:
	SoftFloatIEEE() : inherited() {}
	SoftFloatIEEE(const uint32_t& uFloat) { setChunk((void *) &uFloat, true /* little endian */); }

	SoftFloatIEEE& operator=(const SoftFloatIEEE& sfSource)
	  {  return (SoftFloatIEEE&) inherited::operator=(sfSource); }
	SoftFloatIEEE& assign(const SoftFloatIEEE& sfSource)
	  {  return (SoftFloatIEEE&) inherited::operator=(sfSource); }
	uint32_t queryValue() const
	  {  uint32_t uResult; fillChunk(&uResult, true /* little endian */); return uResult; }
	  
	inline SoftFloatIEEE& saturate();

};

struct HostFloatIEEE
{
private:

public:
	// TODO: rounding mode etc.
};

// Denormal As Zero + Flush To Zero : no denormals
template<class BaseFloat>
struct FloatDAZFTZ : BaseFloat
{
private:
	typedef FloatDAZFTZ<BaseFloat> thisType;
	typedef typename BaseFloat::StatusAndControlFlags Flags;
public:
	FloatDAZFTZ() : BaseFloat() {}
	FloatDAZFTZ(const uint32_t& uFloat) { BaseFloat::setChunk((void *) &uFloat, true /* little endian */); }

	thisType& operator=(const thisType& sfSource)
	  {  return (thisType&) BaseFloat::operator=(sfSource); }
	thisType& assign(const thisType& sfSource)
	  {  return (thisType&) BaseFloat::operator=(sfSource); }
	uint32_t queryValue() const
	  {  uint32_t uResult; BaseFloat::fillChunk(&uResult, true /* little endian */); return uResult; }

	void setZeroPreserveSign() { BaseFloat::querySBasicExponent() = 0U; BaseFloat::querySMantissa() = 0U; }
	void flushDenormals() {
		if(BaseFloat::isDenormalized()) { setZeroPreserveSign(); }
	}

	thisType& plusAssign(const thisType& bdSource, Flags& scfFlags);
	thisType& minusAssign(const thisType& bdSource, Flags& scfFlags);
	thisType& multAssign(const thisType& bdSource, Flags& scfFlags);

	thisType& operator+=(const thisType& bdSource)
		{	return BaseFloat::plusAssign(bdSource, Flags().setNearestRound()); }
	thisType operator+(const thisType& bdSource) const
	{
		thisType bdThis = *this;
		bdThis.plusAssign(bdSource, Flags().setNearestRound());
		return bdThis;
	}
	thisType& operator-=(const thisType& bdSource)
		{  return BaseFloat::minusAssign(bdSource, Flags().setNearestRound()); }
	thisType operator-(const thisType& bdSource) const
	{	thisType bdThis = *this;
		bdThis.minusAssign(bdSource, Flags().setNearestRound());
		return bdThis;
	}
	thisType& operator*=(const thisType& bdSource)
		{  return BaseFloat::multAssign(bdSource, Flags().setNearestRound()); }
	thisType operator*(const thisType& bdSource) const
	{	thisType bdThis = *this;
		bdThis.multAssign(bdSource, Flags().setNearestRound());
		return bdThis;
	}
	
};


// G80-GT200 MAD : multiplication in round toward zero, then addition in current rounding mode
// MAD is *not* a FMA
template<class BaseFloat>
struct FloatMAD : BaseFloat
{
private:
	typedef FloatMAD<BaseFloat> thisType;
	
public:
	thisType& teslaMADAssign(const thisType& bdMult, const thisType& bdAdd, typename BaseFloat::StatusAndControlFlags& scfFlags);
};

typedef FloatMAD<FloatDAZFTZ<SoftFloatIEEE> > SoftFloatTesla;



} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
