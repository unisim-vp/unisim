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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_FLAGS_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_FLAGS_TCC__

#include <unisim/component/cxx/processor/tesla/flags.hh>

#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cmath>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

using namespace std;

template <class CONFIG>
bitset<CONFIG::WARP_SIZE> IsPredSet(uint32_t cond, VectorFlags<CONFIG> flags)
{
	if(cond == CD_TR) {
		return bitset<CONFIG::WARP_SIZE>().set();	// fast path
	}
	
	// TODO: Lookup in a big truth table
	throw "Not implemented!";
}

template <class CONFIG>
VectorFlags<CONFIG> ComputePredFP32(VectorRegister<CONFIG> const & output)
{
	// As tested on G80 and G86:
	//  normal -> 0
	//  zero   -> 1
	//  -inf   -> 2
	//  +inf   -> 0	(sic!)
	//  NaN    -> 3
	VectorFlags<CONFIG> flags;
	flags.Reset();
	for(int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		float f = output.ReadFloat(i);
		switch(fpclassify(f))
		{
		case FP_NORMAL:
			break;
		case FP_ZERO:
			flags.v[i] = 1;
			break;
		case FP_INFINITE:
			if(f < 0) {
				flags.v[i] = 2;
			}
			break;
		case FP_NAN:
			flags.v[i] = 3;
			break;
		case FP_SUBNORMAL:
			flags.v[i] = 1;
			cerr << "Warning: unexpected FP32 subnormal!\n";
			break;
		}
	}
	return flags;
}

template<class CONFIG>
VectorFlags<CONFIG> ComputePredI32(VectorRegister<CONFIG> const & output, VectorFlags<CONFIG> flags)
{
	for(int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		flags.SetZero((output[i] == 0), i);
		flags.SetSign((output[i] < 0), i);
	}
	return flags;
}

template <class CONFIG>
void VectorFlags<CONFIG>::Write(VectorFlags<CONFIG> const & f, bitset<CONFIG::WARP_SIZE> mask)
{
	for(int i = 0; i != CONFIG::WARP_SIZE; ++i)
	{
		if(mask[i])
		{
			v[i] = f.v[i];
		}
	}
}

template <class CONFIG>
VectorFlags<CONFIG> VectorFlags<CONFIG>::Reset()
{
	std::fill(v, v + CONFIG::WARP_SIZE, 0);
	return *this;
}

template <class CONFIG>
void VectorFlags<CONFIG>::SetZero(int z, int lane)
{
	v[lane][0] = z;
}

template <class CONFIG>
int VectorFlags<CONFIG>::GetZero(int lane)
{
	return int(v[lane][0]);
}
template <class CONFIG>
void VectorFlags<CONFIG>::SetSign(int s, int lane)
{
	v[lane][1] = s;
}

template <class CONFIG>
int VectorFlags<CONFIG>::GetSign(int lane)
{
	return int(v[lane][1]);
}


template <class CONFIG>
void VectorFlags<CONFIG>::SetCarry(int c, int lane)
{
	v[lane][2] = c;
}

template <class CONFIG>
int VectorFlags<CONFIG>::GetCarry(int lane)
{
	return int(v[lane][2]);
}

template <class CONFIG>
void VectorFlags<CONFIG>::SetOvf(int o, int lane)
{
	v[lane][3] = o;
}
template <class CONFIG>
int VectorFlags<CONFIG>::GetOvf(int lane)
{
	return int(v[lane][3]);
}

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif

