/*
 *  Copyright (c) 2010,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#include <assert.h>
#include "unisim/component/cxx/processor/arm/decode_load_store.hh"
#include "unisim/util/arithmetic/arithmetic.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

using unisim::util::arithmetic::RotateRight;

uint32_t
LSWUBImmOffset(const uint32_t u,
		const uint32_t val_reg,
		const uint32_t offset)
{
	return val_reg + (((u << 1) - 1) * offset);
}

uint32_t
LSWUBReg(const uint32_t u,
		const uint32_t val_rn,
		const uint32_t val_rd,
		const uint32_t shift_imm,
		const uint32_t shift,
		const uint32_t val_rm,
		const bool carry_in)
{

	if ((shift_imm == 0) && (shift == 0))
	{
		return val_rn + ((( u << 1) - 1) * val_rm);
	}

	uint32_t index = 0;
	switch (shift)
	{
	case 0:
		index = val_rm << shift_imm;
		break;
	case 1:
		if (shift_imm == 0)
			index = 0;
		else
		{
			index = val_rm >> shift_imm;
		}
		break;
	case 2:
		if (shift_imm == 0)
		{
			if ((val_rm & (1 << ((sizeof(uint32_t) * 8) - 1))) != 0)
			{
				index = (uint32_t)((int32_t)-1);
			}
			else
			{
				index = 0;
			}
		}
		else
		{
//			index = ((uint32_t)val_rm) >> shift_imm;   // Gilles: It's an arithmetic shift not a logical shift !!!!!
			index = ((int32_t)val_rm) >> shift_imm;
		}
		break;
	case 3:
		if (shift_imm == 0)
		{
			if (carry_in)
				index = 1 << ((sizeof(uint32_t) * 8) - 1);
			index |= val_rm >> 1;
		}
		else
		{
			index = RotateRight(val_rm, shift_imm);
		}
		break;
	default:
		// this should never happen
		assert(shift < 4);
		break;
	}
	return val_rn + (((u << 1) - 1) * index);
}

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
