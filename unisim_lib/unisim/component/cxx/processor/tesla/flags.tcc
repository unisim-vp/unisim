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

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

using namespace std;

template <class CONFIG>
bitset<CONFIG::WARP_SIZE> IsPredSet(uint32_t cond, VectorFlags<CONFIG> flags)
{
	throw "Not implemented!";
}

template <class CONFIG>
VectorFlags<CONFIG> ComputePred(VectorRegister<CONFIG> const & output)
{
	throw "Not implemented!";
}

template <class CONFIG>
void VectorFlags<CONFIG>::Write(VectorFlags<CONFIG> const & f, bitset<CONFIG::WARP_SIZE> mask)
{
}

template <class CONFIG>
void VectorFlags<CONFIG>::Reset()
{
	std::fill(v, v + CONFIG::WARP_SIZE, 0);
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

