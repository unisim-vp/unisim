/*
 *  Copyright (c) 2017,
 *  Commissariat a l'Energie Atomique (CEA)
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
 *   - Neither the name of CEA nor the names of its contributors may be used to
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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200Z710N3_TYPES_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200Z710N3_TYPES_HH__

#include <unisim/component/cxx/processor/powerpc/cpu.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace e200z710n3 {

class CPU;

typedef uint32_t ADDRESS;
typedef uint32_t PHYSICAL_ADDRESS;
typedef unsigned int STORAGE_ATTR;
const unsigned int FSB_WIDTH = 8;
const bool DEBUG_ENABLE = false;

struct CONFIG
{
	typedef CPU STATE;

	static const Model MODEL = E200Z710N3;
	static const unsigned int NUM_EXCEPTIONS = 32;
	static const unsigned int FSB_WIDTH = unisim::component::cxx::processor::powerpc::e200z710n3::FSB_WIDTH;
	static const bool DEBUG_ENABLE = unisim::component::cxx::processor::powerpc::e200z710n3::DEBUG_ENABLE;
	
	typedef unisim::component::cxx::processor::powerpc::e200z710n3::ADDRESS ADDRESS;
	typedef unisim::component::cxx::processor::powerpc::e200z710n3::PHYSICAL_ADDRESS PHYSICAL_ADDRESS;
	typedef unisim::component::cxx::processor::powerpc::e200z710n3::STORAGE_ATTR STORAGE_ATTR;
};

struct MSS_TYPES
{
	typedef CONFIG::PHYSICAL_ADDRESS ADDRESS;
	typedef CONFIG::STORAGE_ATTR STORAGE_ATTR;
};

} // end of namespace e200z710n3
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_E200Z710N3_TYPES_HH__
