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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_ISA_BOOK_I_INTEGER_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_ISA_BOOK_I_INTEGER_HH__

#include <unisim/util/arithmetic/arithmetic.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {

using unisim::util::arithmetic::SignedAdd32;
using unisim::util::arithmetic::UnsignedAdd32;
using unisim::util::arithmetic::RotateLeft;
using unisim::util::arithmetic::CountLeadingZeros;
using unisim::util::arithmetic::SignExtend;
using unisim::util::arithmetic::SignedSatAdd32;
using unisim::util::arithmetic::UnsignedSatAdd32;

inline uint32_t Mask(uint32_t mb, uint32_t me)
{
	return (mb > me) ? ~(((uint32_t) -1 >> mb) ^ ((me >= 31) ? 0 : (uint32_t) -1 >> (me + 1))) : (((uint32_t)-1 >> mb) ^ ((me >= 31) ? 0 : (uint32_t) -1 >> (me + 1)));
}

inline uint32_t Mask(uint32_t n)
{
	return (n < 32) ? (n ? (1 << (32 - n)) - 1 : 0xffffffffUL) : 0;
}

} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_ISA_BOOK_I_INTEGER_HH__