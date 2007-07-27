/*
 *  Copyright (c) 2007,
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
 
#ifndef __FS_UTILS_DEBUG_WATCHPOINT_HH__
#define __FS_UTILS_DEBUG_WATCHPOINT_HH__

#include <plugins/debug/instruction_level_debug_interface.hh>
#include <inttypes.h>
#include <iostream>

namespace full_system {
namespace utils {
namespace debug {

using full_system::plugins::debug::MemoryType;
using full_system::plugins::debug::MemoryAccessType;
using namespace std;

template <class ADDRESS>
class Watchpoint
{
public:

	Watchpoint(MemoryAccessType mat, MemoryType mt, ADDRESS addr, uint32_t size)
	{
		this->mat = mat;
		this->mt = mt;
		this->addr = addr;
		this->size = size;
	}

	inline MemoryAccessType GetMemoryAccessType() const { return mat; }
	inline MemoryType GetMemoryType() const { return mt; }
	inline ADDRESS GetAddress() const { return addr; }
	inline uint32_t GetSize() const { return size; }
	inline bool HasOverlap(ADDRESS addr, uint32_t size) const 
	{
		ADDRESS a_lo = this->addr;
		ADDRESS a_hi = this->addr + this->size - 1;
		ADDRESS b_lo = addr;
		ADDRESS b_hi = addr + size - 1;
		ADDRESS ovl_lo = a_lo > b_lo ? a_lo : b_lo;
		ADDRESS ovl_hi = a_hi < b_hi ? a_hi : b_hi;
		
		return ovl_lo <= ovl_hi;
	}
private:
	MemoryAccessType mat;
	MemoryType mt;
	ADDRESS addr;
	uint32_t size;
};

} // end of namespace debug
} // end of namespace utils
} // end of namespace full_system

#endif
