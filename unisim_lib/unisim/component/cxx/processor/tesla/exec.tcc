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
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
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

#if 0
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
#endif

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
			ra = false;
			rb = false;
		}
		else if(ra) {
			std::swap(sa, sb);
			std::swap(ra, rb);
		}

		uint32_t r;
		if(sat) {
			uint8_t does_sat;

			if(rb) {
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
			if(rb) {
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
#if 0
	// unsigned, ignore neg...?
	assert(!src1_neg);
	assert(!src3_neg);
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		if(issigned) {
		}
		else {
			uint32_t sa = a[i];
			uint32_t sb = b[i];
			if(m24) {
				sa = (sa & 0x00ffffff);	// 24x24 mul
				sb = (sb & 0x00ffffff);
			}
			else {
				sa = (sa & 0x0000ffff);	// 16x16 mul
				sb = (sb & 0x0000ffff);
			}
			uint32_t sc = c[i];
			uint32_t r;

			uint8_t carry_out, overflow;
	//		cerr << "mac " << sa << " * " << sb << " + " << sc << std::endl;
			r = sa * sb;
			Add32(r, carry_out, overflow, r, sc, 0);
		
		}
		// TODO: CHECK which op updates flags...
		flags.SetOvf(int(overflow), i);
		flags.SetCarry(int(carry_out), i);

		rv[i] = r;
	}
#endif
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
	
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		// Also when !b32
		if(cvt_type == CT_U8)
		{
			// Should not be necessary, just 0-extend (default action)
			rv[i] = a[i] & 0x000000ff;
		}
		else if(cvt_type == CT_S8)
		{
			rv[i] = (a[i] << 24) >> 24;	// sign-extend
		}
		else if(cvt_type == CT_S16)
		{
			rv[i] = (a[i] << 16) >> 16;	// sign-extend
		}
		else {
			assert(false);
		}
	}
	return rv;
}



template<class CONFIG>
VectorRegister<CONFIG> BinNeg(VectorRegister<CONFIG> const & a)
{
	VectorRegister<CONFIG> rv;
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		rv[i] = ~a[i];
	}
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
	case CT_U8:	// hack!
	case CT_S8:
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
	return rv;
}

template<class CONFIG>
VectorRegister<CONFIG> Max(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b,
	unsigned int m32, unsigned int issigned)
{
	assert(false);
}


} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif
