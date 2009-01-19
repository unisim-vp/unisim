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

using namespace unisim::util::arithmetic;

template <class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::FSMad(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b,
                     VectorRegister<CONFIG> const & c, uint32_t rounding_mode)
{
	throw "Not implemented!";
}

template <class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::FSMul(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b,
                     uint32_t rounding_mode)
{
	throw "Not implemented!";
}

template <class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::FSAdd(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b,
                     uint32_t rounding_mode)
{
	throw "Not implemented!";
}

template <class CONFIG>
void CPU<CONFIG>::FSNegate(VectorRegister<CONFIG> & a)
{
	throw "Not implemented!";
}

template<class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::I32Mad24(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b,
                     VectorRegister<CONFIG> const & c, uint32_t sat, uint32_t ra, uint32_t rb,
                     uint32_t rc)
{
	throw "Not implemented!";
}

template<class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::I16Mad24Lo(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b,
                     VectorRegister<CONFIG> const & c, uint32_t sat, uint32_t ra, uint32_t rb,
                     uint32_t rc)
{
	throw "Not implemented!";
}

template<class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::I32Mul24(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b,
                     uint32_t sat, uint32_t ra, uint32_t rb)
{
	throw "Not implemented!";
}

template<class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::I16Mul(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b,
                     uint32_t sat, uint32_t ra, uint32_t rb)
{
	throw "Not implemented!";
}


// Does NOT update zero and sign flag
template<class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::I32Add(VectorRegister<CONFIG> const & a,
	VectorRegister<CONFIG> const & b,
	VectorFlags<CONFIG> & flags,
	uint32_t sat, uint32_t ra, uint32_t rb)
{
	VecReg rv;
	for(int i = 0; i != WARP_SIZE; ++i)
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
VectorRegister<CONFIG> CPU<CONFIG>::ShiftLeft(VectorRegister<CONFIG> const & a, VectorRegister<CONFIG> const & b)
{
	VecReg rv;
	for(int i = 0; i != WARP_SIZE; ++i)
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
void CPU<CONFIG>::I32Negate(VectorRegister<CONFIG> & a)
{
	throw "Not implemented!";
}

template <class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::Convert(VectorRegister<CONFIG> & a, uint32_t cvt_round, uint32_t cvt_type)
{
	throw "Not implemented!";
}

template <class CONFIG>
void CPU<CONFIG>::ScatterGlobal(VecReg output, uint32_t dest, uint32_t addr_lo, uint32_t addr_hi, uint32_t addr_imm, uint32_t segment, std::bitset<CONFIG::WARP_SIZE> mask, DataType dt)
{
	if(dt != DT_U32 && dt != DT_S32) {
		throw "Not implemented!";
	}
	int width = 4;

	uint32_t addr_reg = (addr_hi << 2) | addr_lo;
	// [seg][$a#addr_reg + dest]
	if(addr_reg != 0) {
		throw "Not implemented!";
	}
	VecReg offset;
	if(addr_imm) {
		offset = VecReg(dest);	// TODO: CHECK immediate in words???
	}
	else {
		offset = GetGPR(dest);
	}
	
	// TODO: segment??
	address_t base = 0;
	Scatter32(offset, output, mask, 1, base);
}

#endif
