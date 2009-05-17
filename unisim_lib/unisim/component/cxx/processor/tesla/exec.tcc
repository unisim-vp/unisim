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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_EXEC_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_EXEC_TCC__

#include <unisim/component/cxx/processor/tesla/exec.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/component/cxx/processor/tesla/simfloat.hh>

#include <cmath>
#include <cfloat>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {


using namespace unisim::util::arithmetic;

template <class CONFIG>
VectorRegister<CONFIG> FSMad(VectorRegister<CONFIG> const & a,
	VectorRegister<CONFIG> const & b,
	VectorRegister<CONFIG> const & c,
	uint32_t neg_a, uint32_t neg_b, uint32_t neg_c,
	uint32_t rounding_mode, uint32_t sat)
{
	typedef VectorRegister<CONFIG> VecReg;
	typedef typename CONFIG::float_t float_t;
	typedef typename float_t::StatusAndControlFlags FPFlags;
	VecReg rv;

	// G80-GT200: first mul always rounded to zero
	FPFlags tempflags;
	tempflags.setZeroRound();	// cannot use return value from setZeroRound(): wrong type!
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		float_t sa = a.ReadSimfloat(i);
		float_t sb = b.ReadSimfloat(i);
		if(neg_a) {
			sa.opposite();
		}
		if(neg_b) {
			sb.opposite();
		}
		
		
		float_t r = sa.multAssign(sb, tempflags);
		rv.WriteSimfloat(r, i);
	}
	
	FPFlags flags;
	if(rounding_mode == RM_RN) {
		flags.setNearestRound();
	}
	else if(rounding_mode == RM_RZ) {
		flags.setZeroRound();
	}
	else {
		assert(false);
	}
	
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		float_t sa = rv.ReadSimfloat(i);
		float_t sc = c.ReadSimfloat(i);
		if(neg_c) {
			sc.opposite();
		}
		
		float_t r = sa.plusAssign(sc, flags);
		if(sat) {
			r.saturate();
		}
		rv.WriteSimfloat(r, i);
	}
	
	rv.SetScalar(a.IsScalar() && b.IsScalar() && c.IsScalar());
	return rv;
}

template <class CONFIG>
VectorRegister<CONFIG> FSMul(VectorRegister<CONFIG> const & a,
	VectorRegister<CONFIG> const & b,
	uint32_t neg_a, uint32_t neg_b,
	uint32_t rounding_mode, uint32_t sat)
{
	typedef typename CONFIG::float_t float_t;
	typedef typename float_t::StatusAndControlFlags FPFlags;
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		float_t sa = a.ReadSimfloat(i);
		float_t sb = b.ReadSimfloat(i);
		if(neg_a) {
			sa.opposite();
		}
		if(neg_b) {
			sb.opposite();
		}
		
		FPFlags flags;
		if(rounding_mode == RM_RN) {
			flags.setNearestRound();
		}
		else if(rounding_mode == RM_RZ) {
			flags.setZeroRound();
		}
		else {
			assert(false);
		}
		
		float_t r = sa.multAssign(sb, flags);
		
		if(sat) {
			r.saturate();
		}
		rv[i] = r.queryValue();
	}
	rv.SetScalar(a.IsScalar() && b.IsScalar());
	return rv;
}

template <class CONFIG>
VectorRegister<CONFIG> FSAdd(VectorRegister<CONFIG> const & a,
	VectorRegister<CONFIG> const & b,
	uint32_t neg_a, uint32_t neg_b,
	uint32_t rounding_mode, uint32_t sat)
{
	typedef typename CONFIG::float_t float_t;
	typedef typename float_t::StatusAndControlFlags FPFlags;
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		float_t sa = a.ReadSimfloat(i);
		float_t sb = b.ReadSimfloat(i);
		if(neg_a) {
			sa.opposite();
		}
		if(neg_b) {
			sb.opposite();
		}
		
		FPFlags flags;
		if(rounding_mode == RM_RN) {
			flags.setNearestRound();
			flags.setRoundToEven();
		}
		else if(rounding_mode == RM_RZ) {
			flags.setZeroRound();
		}
		else {
			assert(false);
		}
		
		float_t r = sa.plusAssign(sb, flags);
		
		if(sat) {
			r.saturate();
		}
		rv[i] = r.queryValue();
	}
	rv.SetScalar(a.IsScalar() && b.IsScalar());
	return rv;
}

template <class CONFIG>
VectorRegister<CONFIG> FSMov(VectorRegister<CONFIG> & a, bool neg)
{
	typedef typename CONFIG::float_t float_t;
	typedef typename float_t::StatusAndControlFlags FPFlags;
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		float_t sa = a.ReadSimfloat(i);
		if(neg) {
			sa.opposite();
		}
		rv.WriteSimfloat(sa, i);
	}
	rv.SetScalar(a.IsScalar());
	return rv;
}


// Does NOT update zero and sign flag
template<class CONFIG>
VectorRegister<CONFIG> IAdd(VectorRegister<CONFIG> const & a,
	VectorRegister<CONFIG> const & b,
	VectorFlags<CONFIG> & flags,
	bool sat, bool ra, bool rb, bool m32)
{
	if(m32) {
		return IAdd32(a, b, flags, sat, ra, rb);
	}
	else {
		return IAdd16(a, b, flags, sat, ra, rb); 
	}
}

template<class CONFIG>
VectorRegister<CONFIG> IAdd32(VectorRegister<CONFIG> const & a,
	VectorRegister<CONFIG> const & b,
	VectorFlags<CONFIG> & flags,
	bool sat, bool ra, bool rb)
{
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		uint32_t sa = a[i];
		uint32_t sb = b[i];
		
		// Cannot flip sign before operation:
		//  carry, ovf would be wrong
		//  Tesla semantics: http://developer.download.nvidia.com/opengl/specs/g80specs.pdf
		// add a -b -> sub a b
		// add -a b -> sub b a
		// add -a -b -> flags undefined (= add -a -b)
		if(ra && rb) {
			sa = -int32_t(sa);
			sb = -int32_t(sb);
		}
		else if(ra) {
			std::swap(sa, sb);
//			std::swap(ra, rb);
		}

		uint32_t r;
		if(sat) {
			uint8_t does_sat;

			if((ra || rb) && !(ra && rb)) {
				SignedSatSub32(r, does_sat, sa, sb);
			}
			else {
				SignedSatAdd32(r, does_sat, sa, sb);
			}
			// G80Specs: ovf not updated
			//flags.SetOvf(int(does_sat), i);
			flags.SetOvf(0, i);
			flags.SetCarry(0, i);
		}
		else {
			uint8_t carry_out, overflow;
			if((ra || rb) && !(ra && rb)) {
				Sub32(r, carry_out, overflow, sa, sb, 0);
			}
			else {
				Add32(r, carry_out, overflow, sa, sb, 0);
			}
			flags.SetOvf(int(overflow), i);
			flags.SetCarry(int(carry_out), i);
		}
		rv[i] = r;
	}
	rv.SetScalar(a.IsScalar() && b.IsScalar());
	rv.SetStrided(!sat && a.IsStrided() && b.IsStrided());
	return rv;
}

template<class CONFIG>
VectorRegister<CONFIG> IAdd16(VectorRegister<CONFIG> const & a,
	VectorRegister<CONFIG> const & b,
	VectorFlags<CONFIG> & flags,
	bool sat, bool ra, bool rb)
{
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		uint16_t sa = a[i];
		uint16_t sb = b[i];
		
		// Cannot flip sign before operation:
		//  carry, ovf would be wrong
		//  Tesla semantics: http://developer.download.nvidia.com/opengl/specs/g80specs.pdf
		// add a -b -> sub a b
		// add -a b -> sub b a
		// add -a -b -> flags undefined (= add -a -b)
		if(ra && rb) {
			sa = -int16_t(sa);
			sb = -int16_t(sb);
			ra = false;
			rb = false;
		}
		else if(ra) {
			std::swap(sa, sb);
			std::swap(ra, rb);
		}

		uint16_t r;
		if(sat) {
			uint8_t does_sat;

			if(rb) {
				SignedSatSub16(r, does_sat, sa, sb);
			}
			else {
				SignedSatAdd16(r, does_sat, sa, sb);
			}
			// G80Specs: ovf not updated
			//flags.SetOvf(int(does_sat), i);
			flags.SetOvf(0, i);
			flags.SetCarry(0, i);
		}
		else {
			uint8_t carry_out, overflow;
			if(rb) {
				Sub16(r, carry_out, overflow, sa, sb, 0);
			}
			else {
				Add16(r, carry_out, overflow, sa, sb, 0);
			}
			flags.SetOvf(int(overflow), i);
			flags.SetCarry(int(carry_out), i);
		}
		rv[i] = r;
	}
	rv.SetScalar16(false, a.IsScalar() && b.IsScalar());
	rv.SetStrided16(false, !sat && a.IsStrided() && b.IsStrided());
	return rv;
}

template<class CONFIG>
VectorRegister<CONFIG> Mul24(VectorRegister<CONFIG> const & a,
	VectorRegister<CONFIG> const & b,
	bool sat, bool ra, bool rb, bool m24,
	bool issigned, bool hi)
{
	typedef VectorRegister<CONFIG> VecReg;
	VecReg rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		uint32_t r;
		if(issigned) {
			int32_t sa = a[i];
			int32_t sb = b[i];
			if(m24) {
				sa = (sa << 8) >> 8;	// sign-extend
				sb = (sb << 8) >> 8;
			}
			else {
				sa = (sa << 16) >> 16;
				sb = (sb << 16) >> 16;
			}

			if(ra) sa = -sa;
			if(rb) sb = -sb;
			r = sa * sb;
			assert(!sat);	// TODO: sat
		}
		else {
			uint32_t sa = a[i];
			uint32_t sb = b[i];
			if(m24) {
				sa = sa & 0x00ffffff;
				sb = sb & 0x00ffffff;
			}
			else {
				sa = sa & 0x0000ffff;
				sb = sb & 0x0000ffff;
			}
			assert(!ra && !rb);
			assert(!sat);
			r = (sa * sb);
		}

		rv[i] = r;
	}
	rv.SetScalar(a.IsScalar() && b.IsScalar());
	return rv;
}


template<class CONFIG>
VectorRegister<CONFIG> Mad24(VectorRegister<CONFIG> const & a,
	VectorRegister<CONFIG> const & b,
	VectorRegister<CONFIG> const & c,
	VectorFlags<CONFIG> & flags,
	bool sat,
	bool src1_neg,
	bool src3_neg,
	bool m24,
	bool issigned,
	bool hi)
{
	VectorRegister<CONFIG> rv;
	
	// TODO: CHECK should only saturate at the end??
	// need to keep all 48 bits of the intermediate result (FMA-like)?
	rv = Mul24(a, b, sat, src1_neg, false, m24, issigned, hi);
	rv = IAdd(rv, c, flags, sat, false, src3_neg, true);
	return rv;
	
}



template<class CONFIG>
VectorRegister<CONFIG> ShiftLeft(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b,
	bool u32, bool issigned)
{
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		int32_t sb = b[i];
		if(sb < 0) {
			if(issigned)
				sb = 0;
			else
				sb = u32 ? 32 : 16;
		}
		if(u32 && sb > 32) {
			sb = 32;
		}
		if(!u32 && sb > 16) {
			sb = 16;
		}
		uint32_t r = a[i] << sb;
		if(!u32)
			r &= 0x0000ffff;
		rv[i] = r;
	}
	rv.SetScalar(a.IsScalar() && b.IsScalar());
	rv.SetStrided(a.IsStrided() && b.IsScalar());
	return rv;
}

template<class CONFIG>
VectorRegister<CONFIG> ShiftLeft(VectorRegister<CONFIG> const & a, uint32_t sb, bool u32)
{
	if(u32 && sb > 32) {
		sb = 32;
	}
	if(!u32 && sb > 16) {
		sb = 16;
	}
	if(sb < 0) {
		sb = 0;
	}
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		uint32_t r = a[i] << sb;
		if(!u32)
			r &= 0x0000ffff;
		rv[i] = r;
	}
	rv.SetScalar(a.IsScalar());
	rv.SetStrided(a.IsStrided());
	return rv;
}


template<class CONFIG>
VectorRegister<CONFIG> ShiftRight(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b,
	bool u32, bool issigned)
{
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		int32_t sb = b[i];
		if(sb < 0) {
			if(issigned)
				sb = 0;
			else
				sb = u32 ? 32 : 16;
		}
		if(u32 && sb > 32) {
			sb = 32;
		}
		if(!u32 && sb > 16) {
			sb = 16;
		}
		if(issigned) {
			if(u32) {
				int32_t sa = a[i];
				rv[i] = sa >> sb;
			}
			else {
				int16_t sa = int32_t(a[i]);
				rv[i] = sa >> int16_t(sb);
			}
		}
		else {
			rv[i] = a[i] >> sb;
		}
	}
	rv.SetScalar(a.IsScalar() && b.IsScalar());
	rv.SetStrided(a.IsStrided() && b.IsScalar());
	return rv;
}

template<class CONFIG>
VectorRegister<CONFIG> ShiftRight(VectorRegister<CONFIG> const & a, uint32_t sb, bool u32, bool issigned)
{
	if(sb < 0) {
		if(issigned)
			sb = 0;
		else
			sb = u32 ? 32 : 16;
	}
	if(u32 && sb > 32) {
		sb = 32;
	}
	if(!u32 && sb > 16) {
		sb = 16;
	}
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		if(issigned) {
			if(u32) {
				int32_t sa = a[i];
				rv[i] = sa >> sb;
			}
			else {
				int16_t sa = int32_t(a[i]);
				rv[i] = sa >> int16_t(sb);
			}
		}
		else {
			rv[i] = a[i] >> sb;
		}
	}
	rv.SetScalar(a.IsScalar());
	rv.SetStrided(a.IsStrided());
	return rv;
}


template <class CONFIG>
VectorRegister<CONFIG> ConvertIntInt(VectorRegister<CONFIG> const & a, uint32_t cvt_round, uint32_t cvt_type, bool b32, AbsSat abssat, bool neg)
{
	// cvt_type = *SOURCE* type
	// b32 = *DEST* type
	assert(abssat == AS_NONE);	// TODO: sat, abs, ssat

	// Unless abs, dest>source means no-op
	if(!neg && (cvt_type == CT_NONE
		|| ((cvt_type == CT_U32 || cvt_type == CT_S32) && b32)))
	{
		// No-op
		return a;
	}
	else if(cvt_type == CT_U16)
	{
		assert(!neg);
		return a.Split(0);	// Extract lower part
	}
	
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		// Also when !b32
		uint32_t r;
		switch(cvt_type)
		{
		case CT_U8:
			// Should not be necessary, just 0-extend (default action)
			r = a[i] & 0x000000ff;
			assert(!neg);
			break;
		case CT_S8:
			r = int32_t(a[i] << 24) >> 24;	// sign-extend
			break;
		case CT_U16:
			r = a[i] & 0x0000ffff;
			assert(!neg);
		case CT_S16:
			r = int32_t(a[i] << 16) >> 16;	// sign-extend
			break;
		case CT_U32:
			assert(!neg);
		case CT_S32:
			r = a[i];
			break;
		default:
			assert(false);
		}
		if(neg) {
			r = -int32_t(r);
		}
		rv[i] = r;
	}
	bool scalar, strided;
	switch(cvt_type) {
	case CT_U16:
	case CT_S16:
		scalar = a.IsScalar16(false);
		strided = a.IsStrided16(false);
		break;
	case CT_U32:
	case CT_S32:
		scalar = a.IsScalar();
		strided = a.IsStrided();
		break;
	default:
		scalar = false;
		strided = false;
	}
	if(b32) {
		rv.SetScalar(scalar);
		rv.SetStrided(strided);
	}
	else {
		rv.SetScalar16(false, scalar);
		rv.SetStrided16(false, strided);
	}
	return rv;
}

// Int to float
template<class CONFIG>
VectorRegister<CONFIG> ConvertFloatInt(VectorRegister<CONFIG> const & a, uint32_t rounding_mode, uint32_t cvt_type, bool b32)
{
	typedef typename CONFIG::float_t float_t;
	typedef typename float_t::StatusAndControlFlags FPFlags;
	// cvt_type = *SOURCE* type
	// b32 = ??? type
	assert(b32);

	FPFlags flags;
	switch(rounding_mode)
	{
	case RM_RN:
		flags.setNearestRound();
		break;
	case RM_RZ:
		flags.setZeroRound();
		break;
	case RM_RD:
		flags.setLowestRound();
		break;
	case RM_RU:
		flags.setHighestRound();
		break;
	}

	// TODO: u32/s32???
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		float_t r;
		switch(cvt_type)
		{
		case CT_U32:
		{
			uint32_t ra = a[i];
			r.setInteger(ra, flags);
			break;
		}
		case CT_U16:
		{
			uint16_t ra = a[i] & 0xffff;
			r.setInteger(ra, flags);
			break;
		}
		case CT_U8:
		{
			uint8_t ra = a[i] & 0xff;
			r.setInteger(ra, flags);
			break;
		}
		case CT_S32:
		{
			int32_t ra = a[i];
			r.setInteger(ra, flags);
			break;
		}
		case CT_S16:
		{
			int16_t ra = a[i] & 0xffff;
			r.setInteger(ra, flags);
			break;
		}
		case CT_S8:
		{
			int8_t ra = a[i] & 0xff;
			r.setInteger(ra, flags);
			break;
		}
		default:
			assert(false);
		}
		rv.WriteSimfloat(r, i);
	}
	rv.SetScalar(a.IsScalar());
	return rv;
}

template<class CONFIG>
void ConvertFloatFloat(VectorRegister<CONFIG> & a, bool dest_32, ConvType srctype,
	RoundingMode cvt_round, bool cvt_int, AbsSat abssat)
{
	typedef typename CONFIG::float_t float_t;
	typedef typename CONFIG::half_t half_t;	
	typedef typename float_t::StatusAndControlFlags FPFlags;

	FPFlags flags;
	switch(cvt_round)
	{
	case RM_RN:
		flags.setNearestRound();
		//flags.setRoundToEven();
		break;
	case RM_RZ:
		flags.setZeroRound();
		break;
	case RM_RD:
		flags.setLowestRound();
		break;
	case RM_RU:
		flags.setHighestRound();
		break;
	}

	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		float_t f;
		if(srctype == CT_U32) {
			f = a.ReadSimfloat(i);
		}
		else {
			// Upconvert to Binary32
			//half_t h = half_t(a[i]);
			//f.assign(h, flags);	// lossless
			assert(false);	// TODO
		}
		
		AbsSaturate<CONFIG>(f, abssat);
		
		
		if(cvt_int /*&& f.queryExponent() < 24*/)
		{
			f.roundToInt(flags);
		}
		
		if(dest_32) {
			a.WriteSimfloat(f, i);
		}
		else {
			// Downconvert to Binary16
			//half_t h;
			//h.assign(f, flags);	// According to rounding mode
								// BUG when rounding from fp32 to fp16, number representable in
								// float but not half and round toward zero?
			assert(false);
		}
		
	}
	a.SetStrided(false);
}

template<class CONFIG>
void AbsSaturate(typename CONFIG::float_t & f, AbsSat abssat)
{
	switch(abssat)
	{
	case AS_ABS:
		f.setPositive();
		break;
	case AS_SAT:
		// Unsigned saturation
		f.saturate();
		break;
	case AS_SSAT:
		// Signed saturation
		f.signedSaturate();
		break;
	case AS_NONE:
		break;
	}
}

// float to int
template<class CONFIG>
void ConvertIntFloat(VectorRegister<CONFIG> & a, bool issigned, bool dest_32, ConvType srctype,
	RoundingMode cvt_round)
{
	typedef typename CONFIG::float_t float_t;
	typedef typename CONFIG::half_t half_t;	
	typedef typename float_t::StatusAndControlFlags FPFlags;
	FPFlags flags;
	switch(cvt_round)
	{
	case RM_RN:
		flags.setNearestRound();
		break;
	case RM_RZ:
		flags.setZeroRound();
		break;
	case RM_RD:
		flags.setLowestRound();
		break;
	case RM_RU:
		flags.setHighestRound();
		break;
	}

	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		float_t f;
		if(srctype == CT_U32) {
			f = a.ReadSimfloat(i);
		}
		else {
			// Upconvert to Binary32
			//half_t h = half_t(a[i]);
			//f.assign(h, flags);	// lossless
			assert(false);
		}

		//typename float_t::IntConversion conv;
		uint32_t result;
		if(issigned) {
			//conv.setSigned();
			int32_t t;
			f.retrieveInteger(t, flags);
			result = t;
		}
		else {
			//conv.setUnsigned();
			f.retrieveInteger(result, flags);
		}
		//f.retrieveInteger(conv, flags);	// Rounded
		if(dest_32) {
			a[i] = result;//conv.queryValue();
		}
		else {
			assert(false);	// TODO: implement... one day
		}
	}
	a.SetStrided(false);
}

template<class CONFIG>
VectorRegister<CONFIG> BinNeg(VectorRegister<CONFIG> const & a)
{
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		rv[i] = ~a[i];
	}
	rv.SetScalar(a.IsScalar());
	return rv;
}

template<class CONFIG>
VectorRegister<CONFIG> BinAnd(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b)
{
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		rv[i] = a[i] & b[i];
	}
	rv.SetScalar(a.IsScalar());
	return rv;
}

template<class CONFIG>
VectorRegister<CONFIG> BinOr(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b)
{
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		rv[i] = a[i] | b[i];
	}
	rv.SetScalar(a.IsScalar());
	return rv;
}

template<class CONFIG>
VectorRegister<CONFIG> BinXor(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b)
{
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		rv[i] = a[i] ^ b[i];
	}
	rv.SetScalar(a.IsScalar());
	return rv;
}


template<class CONFIG>
VectorRegister<CONFIG> Min(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b,
	unsigned int m32, unsigned int issigned)
{
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		if(issigned) {
			int32_t sa = a[i];
			int32_t sb = b[i];
			if(!m32) {
				sa = (sa << 16) >> 16;	// sign-extension
				sb = (sb << 16) >> 16;
			}
			rv[i] = std::min(sa, sb);
		}
		else
		{
			uint32_t sa = a[i];
			uint32_t sb = b[i];
			if(!m32) {
				sa = sa & 0x0000ffff;
				sb = sb & 0x0000ffff;
			}
			rv[i] = std::min(sa, sb);
		}
	}
	rv.SetScalar(a.IsScalar() && b.IsScalar());
	return rv;
}

template<class CONFIG>
VectorRegister<CONFIG> Max(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b,
	unsigned int m32, unsigned int issigned)
{
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		if(issigned) {
			int32_t sa = a[i];
			int32_t sb = b[i];
			if(!m32) {
				sa = (sa << 16) >> 16;	// sign-extension
				sb = (sb << 16) >> 16;
			}
			rv[i] = std::max(sa, sb);
		}
		else
		{
			uint32_t sa = a[i];
			uint32_t sb = b[i];
			if(!m32) {
				sa = sa & 0x0000ffff;
				sb = sb & 0x0000ffff;
			}
			rv[i] = std::max(sa, sb);
		}
	}
	rv.SetScalar(a.IsScalar() && b.IsScalar());
	return rv;
}

template<class CONFIG>
VectorFlags<CONFIG> ComputePredSetFP32(VectorRegister<CONFIG> & output,
	VectorRegister<CONFIG> const & a,
	VectorRegister<CONFIG> const & b,
	SetCond sc,
	bool a_abs)
{
	// TODO: check it only outputs boolean values
	// input:
	// SZ
	// 00  01  10  11
	// +1  0   -1  NaN
	static bool const truth_table[16][4] = {
		{ 0, 0, 0, 0 },	// FL
		{ 0, 0, 1, 0 },	// LT
		{ 0, 1, 0, 0 },	// EQ
		{ 0, 1, 1, 0 },	// LE
		{ 1, 0, 0, 0 },	// GT
		{ 1, 0, 1, 0 },	// NE
		{ 1, 1, 0, 0 },	// GE
		{ 1, 1, 1, 0 },	// NUM
		{ 0, 0, 0, 1 }, // NAN
		{ 0, 0, 1, 1 }, // LTU
		{ 0, 1, 0, 1 }, // EQU
		{ 0, 1, 1, 1 }, // LEU
		{ 1, 0, 0, 1 }, // GTU
		{ 1, 0, 1, 1 }, // NEU
		{ 1, 1, 0, 1 }, // GEU
		{ 1, 1, 1, 1 }  // TRU
	};
	
	VectorFlags<CONFIG> flags;
	
	flags.Reset();
	
	assert(sc < 16);
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		typename CONFIG::float_t fa = a.ReadSimfloat(i);
		if(a_abs) {
			fa.setPositive();
		}
		uint8_t cond = Compare<CONFIG>(fa, b.ReadSimfloat(i));
		// cond in [0,4[
		assert(cond < 4);

		bool r = truth_table[sc][cond];
		output[i] = -r;	// G80Spec -> extend
	
		// TODO: CHECK is this correct (=ComputePredI32)?
		flags.SetZero(output[i] == 0, i);
		flags.SetSign(output[i] < 0, i);
	}
	return flags;
}

template<class CONFIG>
inline uint8_t Compare(typename CONFIG::float_t a, typename CONFIG::float_t b)
{
	switch(a.compare(b))
	{
	case CONFIG::float_t::CRNaN:
		return 3;
	case CONFIG::float_t::CRLess:
		return 2;
	case CONFIG::float_t::CREqual:
		return 1;
	case CONFIG::float_t::CRGreater:
		return 0;
	default:
		assert(false);
	}
}

//////////////////////////////////////////////////////////////////////
// Transcendantals, rcp, rsq
// Currently based on host libm
// Deviates from actual hardware (~round to nearest)
// Assumes host is little-endian!
// TODO: Rewrite from scratch using softfloats and detailed models

template<class CONFIG>
VectorRegister<CONFIG> Rcp(VectorRegister<CONFIG> const & a)
{
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		float r = 1.f / a.ReadFloat(i);
		rv.WriteFloat(r, i);
	}
	rv.SetScalar(a.IsScalar());
	return rv;
}

template<class CONFIG>
VectorRegister<CONFIG> Rsq(VectorRegister<CONFIG> const & a)
{
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		// Intermediate comp. in double
		float r = float(1. / sqrt(double(a.ReadFloat(i))));
		rv.WriteFloat(r, i);
	}
	rv.SetScalar(a.IsScalar());
	return rv;
}

template<class CONFIG>
VectorRegister<CONFIG> Log2(VectorRegister<CONFIG> const & a)
{
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		float r = log2(a.ReadFloat(i));
		rv.WriteFloat(r, i);
	}
	rv.SetScalar(a.IsScalar());
	return rv;
}

template<class CONFIG>
VectorRegister<CONFIG> RRExp2(VectorRegister<CONFIG> const & a)
{
	// Convert to FX sv7.23
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		float f = a.ReadFloat(i);
		rv[i] = FPToFX(f);
	}
	rv.SetScalar(a.IsScalar());
	return rv;
}

template<class CONFIG>
VectorRegister<CONFIG> RRTrig(VectorRegister<CONFIG> const & a)
{
	// Dirty range reduction (like actual hardware)
	// Multiply by 2/Pi
	// Convert to FX sv7.23
	// In which order??
	// 2/Pi on 24 (FP)
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		float f = a.ReadFloat(i) * float(M_2_PI);	// Convert 2/pi to float first
		// Only keep fractional part??
		//f = f - floorf(f);
		rv[i] = FPToFX(f);
	}
	rv.SetScalar(a.IsScalar());
	return rv;
}

template<class CONFIG>
VectorRegister<CONFIG> Exp2(VectorRegister<CONFIG> const & a)
{
	// Input in FX sv7.23
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		uint32_t ia = a[i];
		double r = FXToFP(ia);
		r = exp2(float(r));
		// Flush denormals to zero
		if(r < ldexp(1.0, FLT_MIN_EXP)) {	// r > 0
			r = 0;
		}
		rv.WriteFloat(float(r), i);
		
	}
	rv.SetScalar(a.IsScalar());
	return rv;
}

template<class CONFIG>
VectorRegister<CONFIG> Sin(VectorRegister<CONFIG> const & a)
{
	// Input in FX sv7.23
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		uint32_t ia = a[i];
		double r = FXToFP(ia);
		r = sin(r * .5 * M_PI);
		// Flush denormals to zero
		if(r > -ldexp(1.0, FLT_MIN_EXP) && r < ldexp(1.0, FLT_MIN_EXP)) {
			r = 0;
		}
		rv.WriteFloat(r, i);
		// In double precision
		// (until sinpif and cospif functions are available...)
	}
	rv.SetScalar(a.IsScalar());
	return rv;
}

template<class CONFIG>
VectorRegister<CONFIG> Cos(VectorRegister<CONFIG> const & a)
{
	// Input in FX sv7.23
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		uint32_t ia = a[i];
		double r = FXToFP(ia);
		r = cos(r * .5 * M_PI);
		if(r > -ldexp(1.0, FLT_MIN_EXP) && r < ldexp(1.0, FLT_MIN_EXP)) {
			r = 0;
		}		
		rv.WriteFloat(r, i);
	}
	rv.SetScalar(a.IsScalar());
	return rv;
}

inline uint32_t FPToFX(float f)
{
	uint32_t r = uint32_t(fabs(ldexp(f, 23)));
	if(!(fabs(f) < float(1 << 23))) {
		r |= 0x40000000;
	}
	if(f < 0) {	// or -0?
		r |= 0x80000000;
	}
	// What do we do with NaN???
	return r;
}

inline double FXToFP(uint32_t f)
{
	float r;
	if(f & 0x40000000) {	// ovf
		r = (f & 0x80000000) ? -INFINITY : INFINITY;	// Compatibility??
	}
	else if(f & 0x80000000) {
		r = -ldexp(double(f & ~0x80000000), -23);
	}
	else {
		r = ldexp(double(f), -23);
	}
	return r;
}

// IEEE-754:2008-compliant min and max
template<class CONFIG>
VectorRegister<CONFIG> FSMin(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b,
	AbsSat abs_sat)
{
	typedef typename CONFIG::float_t float_t;
	typedef typename float_t::StatusAndControlFlags FPFlags;
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		float_t sa = a.ReadSimfloat(i);
		float_t sb = b.ReadSimfloat(i);
		AbsSaturate<CONFIG>(sa, abs_sat);	// Only applies to 1st operand (?)
		float_t r;
		if(sb < sa || sa.isNaN()) {
			r = sb;
		} else {
			r = sa;	// Return first argument if both args are +/- zero
		}
		rv.WriteSimfloat(r, i);
	}
	rv.SetScalar(a.IsScalar() && b.IsScalar());
	return rv;
}

template<class CONFIG>
VectorRegister<CONFIG> FSMax(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b,
	AbsSat abs_sat)
{
	typedef typename CONFIG::float_t float_t;
	typedef typename float_t::StatusAndControlFlags FPFlags;
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		float_t sa = a.ReadSimfloat(i);
		float_t sb = b.ReadSimfloat(i);
		AbsSaturate<CONFIG>(sa, abs_sat);	// Only applies to 1st operand (?)
		float_t r;
		if(sb > sa || sa.isNaN()) {
			r = sb;
		} else {
			r = sa;	// Return first argument if both args are +/- zero
		}
		rv.WriteSimfloat(r, i);
	}
	rv.SetScalar(a.IsScalar() && b.IsScalar());
	return rv;
}

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif
