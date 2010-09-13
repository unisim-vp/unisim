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

/**
 * Methods for the arm processor load/store multiple operand decoding
 **/

#include "unisim/component/cxx/processor/arm/decode_load_store_multiple.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

uint32_t
LSMia(const uint32_t val_reg,
	  const uint32_t reg_list,
	  uint32_t *start_address,
	  uint32_t *end_address,
	  uint32_t *new_val_reg)
{
	uint32_t num_regs;

	*start_address = val_reg;
	num_regs = 0;
	for (uint32_t mask = 0x00008000; mask != 0; mask = mask >> 1)
	{
		if(mask & reg_list) num_regs++;
	}
	*end_address = val_reg + (num_regs * 4) - 4;
	*new_val_reg = val_reg + (num_regs * 4);

	return num_regs;
}

uint32_t
LSMib(const uint32_t val_reg,
	  const uint32_t reg_list,
	  uint32_t *start_address,
	  uint32_t *end_address,
	  uint32_t *new_val_reg)
{
	uint32_t num_regs;

	*start_address = val_reg + 4;
	num_regs = 0;
	for (uint32_t mask = 0x00008000; mask != 0; mask = mask >> 1)
	{
		if (mask & reg_list) num_regs++;
	}
	*end_address = val_reg + (num_regs * 4);
	*new_val_reg = val_reg + (num_regs * 4);

	return num_regs;
}

uint32_t
LSMda(const uint32_t val_reg,
	  const uint32_t reg_list,
	  uint32_t *start_address,
	  uint32_t *end_address,
	  uint32_t *new_val_reg)
{
	uint32_t num_regs;

	num_regs = 0;
	for (uint32_t mask = 0x00008000; mask != 0; mask = mask >> 1)
	{
		if (mask & reg_list) num_regs++;
	}
	*start_address = val_reg - (num_regs * 4) + 4;
	*end_address = val_reg;
	*new_val_reg = val_reg - (num_regs * 4);

	return num_regs;
}

uint32_t
LSMdb(const uint32_t val_reg,
	  const uint32_t reg_list,
	  uint32_t *start_address,
	  uint32_t *end_address,
	  uint32_t *new_val_reg)
{
	uint32_t num_regs;

	num_regs = 0;
	for (uint32_t mask = 0x00008000; mask != 0; mask = mask >> 1)
	{
		if (mask & reg_list) num_regs++;
	}
	*start_address = val_reg - (num_regs * 4);
	*end_address = val_reg - 4;
	*new_val_reg = val_reg - (num_regs * 4);

	return num_regs;
}

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
