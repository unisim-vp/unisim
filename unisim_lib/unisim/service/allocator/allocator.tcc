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
 
#ifndef UNISIM_SERVICE_ALLOCATOR_ALLOCATOR_TCC
#define UNISIM_SERVICE_ALLOCATOR_ALLOCATOR_TCC

#include <unisim/service/allocator/allocator.hh>
#include <limits>
#include <cassert>

namespace unisim {
namespace service {
namespace allocator {

template<class ADDRESS, class SIZE>
Allocator<ADDRESS, SIZE>::Allocator(char const * name, Object * parent) :
	Object(name, parent),
	param_base("base", this, base),
	param_limit("limit", this, limit),
	param_alignment("alignment", this, alignment),
	base(0),
	limit(std::numeric_limits<ADDRESS>::max),
	alignment(128)
{
}

template<class ADDRESS, class SIZE>
Allocator<ADDRESS, SIZE>::~Allocator()
{
}

template<class ADDRESS, class SIZE>
bool Allocator<ADDRESS, SIZE>::Setup()
{
	if(limit < base
}

template<class ADDRESS, class SIZE>
ADDRESS Allocator<ADDRESS, SIZE>::Alloc(SIZE s)
{
	assert(!(limit & (alignment - 1)));
	size = (size + (alignment - 1)) & ~(alignment - 1);	// align
	if(limit + size > base + max_size)
	{
		throw CudaException(CUDA_ERROR_OUT_OF_MEMORY);
	}
	typename CONFIG::address_t addr = limit;
	limit += size;
	return addr;
}

template<class ADDRESS, class SIZE>
void Allocator<ADDRESS, SIZE>::Free(ADDRESS a)
{
}

template<class ADDRESS, class SIZE>
ADDRESS Allocator<ADDRESS, SIZE>::GetBase(ADDRESS a)
{
	assert(false);
}

template<class ADDRESS, class SIZE>
SIZE Allocator<ADDRESS, SIZE>::GetSize(ADDRESS a)
{
	assert(false);
}


} // end of namespace allocator
} // end of namespace service
} // end of namespace unisim

#endif
