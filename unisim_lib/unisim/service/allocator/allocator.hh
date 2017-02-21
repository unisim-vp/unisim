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
 
#ifndef UNISIM_SERVICE_ALLOCATOR_ALLOCATOR_HH
#define UNISIM_SERVICE_ALLOCATOR_ALLOCATOR_HH

#include "unisim/service/interfaces/allocator.hh"
#include "unisim/kernel/service/service.hh"
#include <inttypes.h>

namespace unisim {
namespace service {
namespace allocator {

using unisim::kernel::service::Object;
using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::Parameter;

template<class ADDRESS, class SIZE>
struct Allocator : Service<unisim::service::interfaces::Allocator<ADDRESS, SIZE> >
{
	ServiceExport<unisim::service::interfaces::Allocator<ADDRESS, SIZE> > allocator_export;

	Allocator(char const * name, Object * parent = 0);
	virtual ~Allocator();
	
	virtual bool Setup();

	virtual ADDRESS Alloc(SIZE s);
	virtual void Free(ADDRESS a);
	
	virtual ADDRESS GetBase(ADDRESS a);
	virtual SIZE GetSize(ADDRESS a);

private:
	Parameter<ADDRESS> param_base;
	Parameter<ADDRESS> param_limit;
	Parameter<uint32_t> param_alignment;
	
	ADDRESS base;
	ADDRESS limit;
	uint32_t alignment;
};


} // end of namespace allocator
} // end of namespace service
} // end of namespace unisim

#endif
