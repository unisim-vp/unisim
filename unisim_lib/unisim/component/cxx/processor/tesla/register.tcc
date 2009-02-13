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
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_REGISTER_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_REGISTER_TCC__

#include <unisim/component/cxx/processor/tesla/register.hh>
#include <cassert>
#include <ostream>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {


template <class CONFIG>
void VectorRegister<CONFIG>::NegateFP32()
{
}

template <class CONFIG>
VectorRegister<CONFIG>::VectorRegister()
{
}

template <class CONFIG>
VectorRegister<CONFIG>::VectorRegister(uint32_t val)
{
	std::fill(v, v + WARP_SIZE, val);
}

template <class CONFIG>
void VectorRegister<CONFIG>::Write(VectorRegister<CONFIG> const & vec, bitset<CONFIG::WARP_SIZE> mask)
{
	for(int i = 0; i != WARP_SIZE; ++i)
	{
		if(mask[i]) {
			WriteLane(vec[i], i);
		}
	}
}

template <class CONFIG>
void VectorRegister<CONFIG>::Write16(VectorRegister<CONFIG> const & vec,
	bitset<CONFIG::WARP_SIZE> mask, int hi)
{
	throw "Not implemented!";
}

template <class CONFIG>
void VectorRegister<CONFIG>::WriteLane(uint32_t val, int lane)
{
	assert(lane >= 0 && lane < WARP_SIZE);
	v[lane] = val;
}

template <class CONFIG>
uint32_t VectorRegister<CONFIG>::ReadLane(int lane) const
{
	assert(lane >= 0 && lane < WARP_SIZE);
	return v[lane];
}

template <class CONFIG>
void VectorRegister<CONFIG>::WriteFloat(float val, int lane)
{
	// TODO: endianness
	union { float f; uint32_t u; } caster;
	caster.f = val;
	WriteLane(caster.u, lane);
}

template <class CONFIG>
float VectorRegister<CONFIG>::ReadFloat(int lane) const
{
	// TODO: endianness
	union { float f; uint32_t u; } caster;
	caster.u = ReadLane(lane);
	return caster.f;
}

template <class CONFIG>
void VectorRegister<CONFIG>::WriteSimfloat(typename CONFIG::float_t val, int lane)
{
	WriteLane(val.queryValue(), lane);
}

template <class CONFIG>
typename CONFIG::float_t VectorRegister<CONFIG>::ReadSimfloat(int lane) const
{
	return typename CONFIG::float_t(ReadLane(lane));
}

template <class CONFIG>
VectorRegister<CONFIG> VectorRegister<CONFIG>::Split(int hilo) const
{
	VectorRegister<CONFIG> vr;
	for(int i = 0; i != WARP_SIZE; ++i)
	{
		if(hilo) {
			// high part
			vr[i] = (v[i] << 16);
		}
		else {
			// low part
			vr[i] = (v[i] & 0x0000ffff);
		}
	}
	return vr;
}

template <class CONFIG>
uint32_t VectorRegister<CONFIG>::operator[] (int lane) const
{
	assert(lane >= 0 && lane < WARP_SIZE);
	return v[lane];
}

template <class CONFIG>
uint32_t & VectorRegister<CONFIG>::operator[] (int lane)
{
	assert(lane >= 0 && lane < WARP_SIZE);
	return v[lane];
}

template <class CONFIG>
std::ostream & operator << (std::ostream & os, VectorRegister<CONFIG> const & r)
{
	os << "(";
	os << std::hex;
	for(int i = 0; i != CONFIG::WARP_SIZE-1; ++i)
	{
		os << r[i] << ", ";
	}
	os << r[CONFIG::WARP_SIZE-1];
	os << std::dec;
	os << ")";
}

template <class CONFIG>
VectorAddress<CONFIG>::VectorAddress()
{
}

template <class CONFIG>
VectorAddress<CONFIG>::VectorAddress(VectorAddress<CONFIG>::address_t addr)
{
	std::fill(v, v + WARP_SIZE, addr);
}

template <class CONFIG>
VectorAddress<CONFIG>::VectorAddress(VectorRegister<CONFIG> const & vr)
{
	for(int i = 0; i != CONFIG::WARP_SIZE; ++i) {
		v[i] = vr[i];
	}
}

template <class CONFIG>
void VectorAddress<CONFIG>::Write(VectorAddress<CONFIG> const & vec, std::bitset<CONFIG::WARP_SIZE> mask)
{
	for(int i = 0; i != CONFIG::WARP_SIZE; ++i) {
		if(mask[i])
			v[i] = vec[i];
	}
}

template <class CONFIG>
void VectorAddress<CONFIG>::Reset()
{
	std::fill(v, v + WARP_SIZE, 0);
}

template <class CONFIG>
typename VectorAddress<CONFIG>::address_t VectorAddress<CONFIG>::operator[] (int lane) const
{
	assert(lane >= 0 && lane < WARP_SIZE);
	return v[lane];
}

template <class CONFIG>
typename VectorAddress<CONFIG>::address_t & VectorAddress<CONFIG>::operator[] (int lane)
{
	assert(lane >= 0 && lane < WARP_SIZE);
	return v[lane];
}

template <class CONFIG>
VectorAddress<CONFIG> & VectorAddress<CONFIG>::operator+=(VectorAddress<CONFIG> const & other)
{
	for(int i = 0; i != CONFIG::WARP_SIZE; ++i) {
		v[i] += other[i];
	}
	return *this;
}


} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
