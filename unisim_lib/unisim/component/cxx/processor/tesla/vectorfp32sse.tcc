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

#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_VECTORFP32SSE_TCC
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_VECTORFP32SSE_TCC

#include <unisim/component/cxx/processor/tesla/vectorfp32.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/component/cxx/processor/tesla/simfloat.hh>

#include <cmath>
#include <cfloat>
#include <xmmintrin.h>
#include <emmintrin.h>	// For DAZ mode

// Missing from emmintrin.h...
#define MM_DAZ_MASK	0x0040
#define MM_DAZ_ON	0x0040
#define MM_DAZ_OFF	0x0000

inline unsigned int MM_GET_DAZ_MODE ()
{
	return _mm_getcsr() & MM_DAZ_MASK;
}

inline void MM_SET_DAZ_MODE (unsigned int mode)
{
	_mm_setcsr((_mm_getcsr() & ~MM_DAZ_MASK) | mode);
}

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

template <class CONFIG>
void VectorFP32SSE<CONFIG>::Init()
{
	// Set FTZ and DAZ globally?
	//_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
	//MM_SET_DAZ_MODE(MM_DAZ_ON);
}

inline __m128 saturate(__m128 x)
{
	_mm_max_ps(x, (__m128)_mm_setzero_si128());
	_mm_min_ps(x, _mm_set1_ps(1.0f));
	return x;
}

template <class CONFIG>
VectorRegister<CONFIG> & VectorFP32SSE<CONFIG>::Mad(VectorRegister<CONFIG> & a,
	VectorRegister<CONFIG> const & b,
	VectorRegister<CONFIG> const & c,
	uint32_t neg_a, uint32_t neg_b, uint32_t neg_c,
	uint32_t rounding_mode, uint32_t sat)
{
	assert(CONFIG::WARP_SIZE % 4 == 0);
	assert(!neg_b);
	typedef VectorRegister<CONFIG> VecReg;

	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
	MM_SET_DAZ_MODE(MM_DAZ_ON);

	// G80-GT200: first mul always rounded to zero
	_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);
	
	__m128 sign_mask = (__m128)_mm_set1_epi32(0x80000000);
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; i += 4)
	{
		// TODO: force alignment of VectorRegister (or just switch to Nehalem...)
		__m128 sa = _mm_loadu_ps((float*)a.v + i);
		__m128 sb = _mm_loadu_ps((float*)b.v + i);
		if(neg_a) {
			sa = _mm_xor_ps(sa, sign_mask);
		}
		//if(neg_b) {
		//	sb = _mm_xor_ps(sb, sign_mask);
		//}
		__m128 r = _mm_mul_ps(sa, sb);
		_mm_storeu_ps((float*)a.v + i, r);
	}
	
	if(rounding_mode == RM_RN) {
		_MM_SET_ROUNDING_MODE(_MM_ROUND_NEAREST);
	}
	else if(rounding_mode == RM_RZ) {
		// keep current rounding mode (TZ)
	}
	else {
		assert(false);
	}
	
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; i += 4)
	{
		__m128 sa = _mm_loadu_ps((float*)a.v + i);
		__m128 sc = _mm_loadu_ps((float*)c.v + i);
		if(neg_c) {
			sc = _mm_xor_ps(sc, sign_mask);
		}
		
		__m128 r = _mm_add_ps(sa, sc);
		if(sat) {
			r = saturate(r);
		}
		_mm_storeu_ps((float*)a.v + i, r);
	}
	
	if(rounding_mode != RM_RN) {
		// Restore RM
		_MM_SET_ROUNDING_MODE(_MM_ROUND_NEAREST);
	}
	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_OFF);
	MM_SET_DAZ_MODE(MM_DAZ_OFF);
	
	a.SetScalar(a.IsScalar() && b.IsScalar() && c.IsScalar());
	return a;
}

template <class CONFIG>
VectorRegister<CONFIG> & VectorFP32SSE<CONFIG>::Mul(VectorRegister<CONFIG> & a,
	VectorRegister<CONFIG> const & b,
	uint32_t neg_a, uint32_t neg_b,
	uint32_t rounding_mode, uint32_t sat)
{
	assert(CONFIG::WARP_SIZE % 4 == 0);

	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
	MM_SET_DAZ_MODE(MM_DAZ_ON);
	
	__m128 sign_mask = (__m128)_mm_set1_epi32(0x80000000);

	if(rounding_mode == RM_RZ) {
		_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);
	}
	else {
		assert(rounding_mode == RM_RN);
	}

	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; i += 4)
	{
		__m128 sa = _mm_loadu_ps((float*)a.v + i);
		__m128 sb = _mm_loadu_ps((float*)b.v + i);

		if(neg_a) {
			sa = _mm_xor_ps(sa, sign_mask);
		}
		if(neg_b) {
			sb = _mm_xor_ps(sb, sign_mask);
		}
		__m128 r = _mm_mul_ps(sa, sb);
		if(sat) {
			r = saturate(r);
		}
		_mm_storeu_ps((float*)a.v + i, r);
	}
	if(rounding_mode != RM_RN) {
		// Restore RM
		_MM_SET_ROUNDING_MODE(_MM_ROUND_NEAREST);
	}
	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_OFF);
	MM_SET_DAZ_MODE(MM_DAZ_OFF);
	a.SetScalar(a.IsScalar() && b.IsScalar());
	return a;
}

template <class CONFIG>
VectorRegister<CONFIG> & VectorFP32SSE<CONFIG>::Add(VectorRegister<CONFIG> & a,
	VectorRegister<CONFIG> const & b,
	uint32_t neg_a, uint32_t neg_b,
	uint32_t rounding_mode, uint32_t sat)
{
	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
	MM_SET_DAZ_MODE(MM_DAZ_ON);

	__m128 sign_mask = (__m128)_mm_set1_epi32(0x80000000);
	if(rounding_mode == RM_RZ) {
		_MM_SET_ROUNDING_MODE(_MM_ROUND_TOWARD_ZERO);
	}
	else {
		assert(rounding_mode == RM_RN);
	}
	for(unsigned int i = 0; i != CONFIG::WARP_SIZE; i += 4)
	{
		__m128 sa = _mm_loadu_ps((float*)a.v + i);
		__m128 sb = _mm_loadu_ps((float*)b.v + i);

		if(neg_a) {
			sa = _mm_xor_ps(sa, sign_mask);
		}
		if(neg_b) {
			sb = _mm_xor_ps(sb, sign_mask);
		}
		__m128 r = _mm_add_ps(sa, sb);
		if(sat) {
			r = saturate(r);
		}
		_mm_storeu_ps((float*)a.v + i, r);
	}
	if(rounding_mode != RM_RN) {
		// Restore RM
		_MM_SET_ROUNDING_MODE(_MM_ROUND_NEAREST);
	}
	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_OFF);
	MM_SET_DAZ_MODE(MM_DAZ_OFF);
	a.SetScalar(a.IsScalar() && b.IsScalar());
	return a;
}

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif
