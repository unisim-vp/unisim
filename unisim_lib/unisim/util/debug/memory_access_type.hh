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
 *          Yves Lhuillier (yves.lhuillier@cea.fr)
 */
 
#ifndef __UNISIM_UTIL_DEBUG_MEMORY_ACCESS_TYPE_HH__
#define __UNISIM_UTIL_DEBUG_MEMORY_ACCESS_TYPE_HH__

#include <iostream>

namespace unisim {
namespace util {
namespace debug {

typedef enum { MAT_READ = 1, MAT_WRITE = 2, MAT_READ_WRITE = MAT_READ | MAT_WRITE } MemoryAccessType;
typedef enum { MT_DATA = 0, MT_INSN = 1 } MemoryType;

} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

inline std::ostream& operator << (std::ostream& stream, unisim::util::debug::MemoryAccessType mat)
{
	switch(mat)
	{
		case unisim::util::debug::MAT_READ: stream << "read"; break;
		case unisim::util::debug::MAT_WRITE: stream << "write"; break;
		case unisim::util::debug::MAT_READ_WRITE: stream << "read-write"; break;
	}
	return stream;
}

inline std::ostream& operator << (std::ostream& stream, unisim::util::debug::MemoryType mt)
{
	switch(mt)
	{
		case unisim::util::debug::MT_DATA: stream << "data"; break;
		case unisim::util::debug::MT_INSN: stream << "insn"; break;
	}
	return stream;
}

#endif // __UNISIM_UTIL_DEBUG_MEMORY_ACCESS_TYPE_HH__
