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
 
#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_SAMPLER_HH
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_SAMPLER_HH

#include <unisim/component/cxx/processor/tesla/register.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

enum ArrayFormat
{
    AF_U8  = 0x01,
    AF_U16 = 0x02,
    AF_U32 = 0x03,
    AF_S8 = 0x08,
    AF_S16 = 0x09,
    AF_S32 = 0x0a,
    AF_F16 = 0x10,
    AF_F32 = 0x20
};

enum AddressMode
{
    AM_WRAP = 0,
    AM_CLAMP = 1,
    AM_MIRROR = 2
};

enum FilterMode
{
    FM_POINT = 0,
    FM_LINEAR = 1
};

enum TextureFlags
{
	TF_READ_AS_INTEGER = 0x01,
	TF_NORMALIZED_COORDINATES = 0x02
};

inline int ArrayFormatSize(ArrayFormat af);

template<class CONFIG>
struct Sampler
{
	void Sample1DS32(VectorRegister<CONFIG> dest[],
		VectorRegister<CONFIG> const src[],
		uint32_t destBitfield);

	void Reset(CPU<CONFIG> * cpu);

	typename CONFIG::address_t baseAddress;
	int ndims;
	int numPackedComponents;
	uint32_t size[3];
	ArrayFormat format;
	AddressMode addressMode[3];
	FilterMode filterMode;
	uint32_t flags;
	
	CPU<CONFIG> * cpu;

private:
	void Fetch(VectorRegister<CONFIG> dest[],
		VectorAddress<CONFIG> const & addr,
		uint32_t destBitfield);
	uint32_t Unpack(uint32_t rawval);
};

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
