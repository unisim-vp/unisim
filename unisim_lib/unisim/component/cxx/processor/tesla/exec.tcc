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

#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_EXEC_TCC
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_EXEC_TCC

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
		case CT_U32U8:
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

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif
