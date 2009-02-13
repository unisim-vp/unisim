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
	for(int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		float_t sa = a.ReadSimfloat(i);
		float_t sb = b.ReadSimfloat(i);
		float_t sc = c.ReadSimfloat(i);
		if(neg_a) {
			sa.opposite();
		}
		if(neg_b) {
			sb.opposite();
		}
		if(neg_c) {
			sc.opposite();
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
		
		// G80-GT200: first mul always rounded to zero
		sa.multAssign(sb, FPFlags().setZeroRound());
		float_t r = sa.plusAssign(sc, flags);
		
		if(sat) {
			r.saturate();
		}
		rv[i] = r.queryValue();
	}
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
	for(int i = 0; i != CONFIG::WARP_SIZE; ++i)
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
	for(int i = 0; i != CONFIG::WARP_SIZE; ++i)
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
		
		float_t r = sa.plusAssign(sb, flags);
		
		if(sat) {
			r.saturate();
		}
		rv[i] = r.queryValue();
	}
	return rv;
}

template <class CONFIG>
void FSNegate(VectorRegister<CONFIG> & a)
{
	throw "Not implemented!";
}

template<class CONFIG>
VectorRegister<CONFIG> I32Mad24(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b,
                     VectorRegister<CONFIG> const & c, uint32_t sat, uint32_t ra, uint32_t rb,
                     uint32_t rc)
{
	throw "Not implemented!";
}

template<class CONFIG>
VectorRegister<CONFIG> I16Mad24Lo(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b,
                     VectorRegister<CONFIG> const & c, uint32_t sat, uint32_t ra, uint32_t rb,
                     uint32_t rc)
{
	throw "Not implemented!";
}

template<class CONFIG>
VectorRegister<CONFIG> I32Mul24(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b,
                     uint32_t sat, uint32_t ra, uint32_t rb)
{
	throw "Not implemented!";
}

template<class CONFIG>
VectorRegister<CONFIG> I16Mul(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b,
                     uint32_t sat, uint32_t ra, uint32_t rb)
{
	throw "Not implemented!";
}


// Does NOT update zero and sign flag
template<class CONFIG>
VectorRegister<CONFIG> I32Add(VectorRegister<CONFIG> const & a,
	VectorRegister<CONFIG> const & b,
	VectorFlags<CONFIG> & flags,
	uint32_t sat, uint32_t ra, uint32_t rb)
{
	VectorRegister<CONFIG> rv;
	for(int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		int32_t sa = int32_t(a[i]);
		int32_t sb = int32_t(b[i]);
		if(ra) {
			sa = -sa;
		}
		if(rb) {
			sb = -sb;
		}
		uint32_t r;
		if(sat) {
			uint8_t does_sat;
			SignedSatAdd32(r, does_sat, uint32_t(sa), uint32_t(sb));
			// TODO: CHECK: ovf = does_sat??
			flags.SetOvf(int(does_sat), i);
			flags.SetCarry(0, i);
		}
		else {
			uint8_t carry_out, overflow;
			Add32(r, carry_out, overflow, uint32_t(sa), uint32_t(sb), 0);
			flags.SetOvf(int(overflow), i);
			flags.SetCarry(int(carry_out), i);
		}
		rv[i] = r;
	}
	return rv;
}

template<class CONFIG>
VectorRegister<CONFIG> UMad24(VectorRegister<CONFIG> const & a,
	VectorRegister<CONFIG> const & b,
	VectorRegister<CONFIG> const & c,
	VectorFlags<CONFIG> & flags,
	uint32_t src1_neg,
	uint32_t src3_neg)
{
	VectorRegister<CONFIG> rv;
	// unsigned, ignore neg...?
	assert(!src1_neg);
	assert(!src3_neg);
	for(int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		uint32_t sa = (a[i] & 0x00ffffff);	// 24x24 mul
		uint32_t sb = (b[i] & 0x00ffffff);
		uint32_t sc = c[i];
		uint32_t r;

		uint8_t carry_out, overflow;
//		cerr << "mac " << sa << " * " << sb << " + " << sc << std::endl;
		r = sa * sb;
		Add32(r, carry_out, overflow, r, sc, 0);
		// TODO: CHECK which op updates flags...
		flags.SetOvf(int(overflow), i);
		flags.SetCarry(int(carry_out), i);

		rv[i] = r;
	}
	return rv;
}

template<class CONFIG>
VectorRegister<CONFIG> ShiftLeft(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b)
{
	VectorRegister<CONFIG> rv;
	for(int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		uint32_t sb = b[i];
		if(sb > 32) {
			sb = 32;
		}
		if(sb < 0) {
			sb = 0;
		}
		rv[i] = a[i] << sb;
	}
	return rv;
}


template <class CONFIG>
void I32Negate(VectorRegister<CONFIG> & a)
{
	throw "Not implemented!";
}

template <class CONFIG>
VectorRegister<CONFIG> ConvertIntInt(VectorRegister<CONFIG> & a, uint32_t cvt_round, uint32_t cvt_type, bool b32, AbsSat abssat)
{
	// cvt_type = *SOURCE* type
	// b32 = *DEST* type
	assert(abssat == AS_NONE);	// TODO: sat, abs, ssat

	// Unless abs, dest>source means no-op
	if(cvt_type == CT_NONE
		|| (cvt_type == CT_U32 || cvt_type == CT_S32) && b32)
	{
		// No-op
		return a;
	}
	else if(cvt_type == CT_U16)
	{
		return a.Split(0);	// Extract lower part
	}
	
	// No rounding mode for int->int conversion (always truncation?)
	assert(false);	// TODO
	return a;
}



template<class CONFIG>
VectorRegister<CONFIG> BinNeg(VectorRegister<CONFIG> const & a)
{
	VectorRegister<CONFIG> rv;
	for(int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		rv[i] = ~a[i];
	}
	return rv;
}

template<class CONFIG>
VectorRegister<CONFIG> BinAnd(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b)
{
	VectorRegister<CONFIG> rv;
	for(int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		rv[i] = a[i] & b[i];
	}
	return rv;
}

template<class CONFIG>
VectorRegister<CONFIG> BinOr(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b)
{
	VectorRegister<CONFIG> rv;
	for(int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		rv[i] = a[i] | b[i];
	}
	return rv;
}

template<class CONFIG>
VectorRegister<CONFIG> BinXor(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b)
{
	VectorRegister<CONFIG> rv;
	for(int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		rv[i] = a[i] ^ b[i];
	}
	return rv;
}

template<class CONFIG>
VectorRegister<CONFIG> UMul24Lo(VectorRegister<CONFIG> const & a,
	VectorRegister<CONFIG> const & b,
	uint32_t sat, uint32_t ra, uint32_t rb, uint32_t u16)
{
	typedef VectorRegister<CONFIG> VecReg;
	VecReg rv;
	for(int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		uint32_t sa = a[i];
		uint32_t sb = b[i];
		if(u16) {
			sa = sa & 0x0000ffff;
			sb = sb & 0x0000ffff;
		}
		else {
			sa = sa & 0x00ffffff;
			sb = sb & 0x00ffffff;
		}
		assert(!ra && !rb);
		uint32_t r;
		assert(!sat);
		uint8_t carry_out, overflow;
		r = (sa * sb);

		rv[i] = r;
	}
	return rv;
}

template<class CONFIG>
VectorRegister<CONFIG> ShiftLeft(VectorRegister<CONFIG> const & a, uint32_t sb)
{
	if(sb > 32) {
		sb = 32;
	}
	if(sb < 0) {
		sb = 0;
	}
	VectorRegister<CONFIG> rv;
	for(int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		rv[i] = a[i] << sb;
	}
	return rv;
}

inline SMType MvSizeToSMType(uint32_t mv_size)
{
	switch(mv_size) {
	case 0:
		return SM_U16;
	case 1:
		return SM_U32;
	case 2:
		return SM_U8;
	default:
		assert(false);
		throw "";
	}
}

inline RegType CvtTypeToRT(ConvType ct)
{
	switch(ct) {
	case CT_U16:
	case CT_S16:
		return RT_U16;
	case CT_U32:
	case CT_S32:
	case CT_NONE:
		return RT_U32;
	default:
		assert(false);
		throw "";
	}
}


} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif
