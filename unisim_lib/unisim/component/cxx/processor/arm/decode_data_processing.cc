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

#include "unisim/component/cxx/processor/arm/decode_data_processing.hh"
#include "unisim/util/arithmetic/arithmetic.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

using unisim::util::arithmetic::RotateRight;

uint32_t
ShiftOperand32imm(const uint32_t rotate_imm,
		const uint32_t imm)
{
	uint32_t shifter_operand = 0;

	shifter_operand = RotateRight(imm, rotate_imm * 2);

	return shifter_operand;
}

uint32_t
ShiftOperand32imm(const uint32_t rotate_imm,
		const uint32_t imm,
		const bool carry_in,
		bool *shift_carry_out)
{
	uint32_t shifter_operand = 0;

	shifter_operand = RotateRight(imm, rotate_imm * 2);
	if(rotate_imm == 0) {
		*shift_carry_out = carry_in;
	} else {
		*shift_carry_out = (((uint32_t)1) << 31) & shifter_operand;
	}

	return shifter_operand;
}

uint32_t
ShiftOperandImmShift(const uint32_t shift_imm,
		const uint32_t shift,
		const uint32_t val_reg,
		const bool carry_in)
{
	uint32_t shifter_operand = 0;

	if ((shift_imm == 0) && (shift == 0))
	{
		shifter_operand = val_reg;
		return shifter_operand;
	}
	else
	{
		if (shift == 0)
		{
			shifter_operand = val_reg << shift_imm;
			return shifter_operand;
		}
		else
		{
			if (shift == 0x01)
			{
				if (shift_imm == 0)
				{
					shifter_operand = 0;
				}
				else
				{
					shifter_operand = val_reg >> shift_imm;
				}
				return shifter_operand;
			}
			else
			{
				if (shift == 0x02)
				{
					if (shift_imm == 0)
					{
						if (((int32_t)val_reg) > 0)
						{
							shifter_operand = 0;
						}
						else
						{
							shifter_operand = (uint32_t)((int32_t)-1);
						}
					}
					else
					{
						shifter_operand = ((int32_t)val_reg) >> shift_imm;
					}
					return shifter_operand;
				}
				else
				{

					if (shift == 0x03) /* ROR */
					{
						if (shift_imm == 0)
						{
							shifter_operand = 0;
							shifter_operand = ((carry_in?1:0) << 31) |
									(val_reg >> 1);
						}
						else
						{
							shifter_operand = RotateRight(val_reg, shift_imm);
						}
						return shifter_operand;
					}
				}
			}
		}
	}

	return 0;
}

uint32_t
ShiftOperandImmShift(const uint32_t shift_imm,
		const uint32_t shift,
		const uint32_t val_reg,
		const bool carry_in,
		bool *shift_carry_out)
{
	uint32_t shifter_operand = 0;
	uint32_t mask = 1;

	if ((shift_imm == 0) && (shift == 0))
	{
		shifter_operand = val_reg;
		*shift_carry_out = carry_in;
		return shifter_operand;
	}
	else
	{
		if (shift == 0)
		{
			shifter_operand = val_reg << shift_imm;
			//     shifter_operand |= val_reg >> (32 - shift_imm);
			mask = mask << (32 - shift_imm);
			*shift_carry_out = ((mask & val_reg) != 0);
			return shifter_operand;
		}
		else
		{
			if (shift == 0x01)
			{
				if (shift_imm == 0)
				{
					shifter_operand = 0;
					mask = mask << 31;
					*shift_carry_out = ((mask & val_reg) != 0);
				}
				else
				{
					shifter_operand = val_reg >> shift_imm;
					mask = mask << (shift_imm - 1);
					*shift_carry_out = ((mask & val_reg) != 0);
				}
				return shifter_operand;
			}
			else
			{
				if (shift == 0x02)
				{
					if (shift_imm == 0)
					{
						if (((int32_t)val_reg) > 0)
						{
							shifter_operand = 0;
							*shift_carry_out = false;
						}
						else
						{
							shifter_operand = (uint32_t)((int32_t)-1);
							*shift_carry_out = true;
						}
					}
					else
					{
						shifter_operand = ((int32_t)val_reg) >> shift_imm;
						mask = mask << (shift_imm - 1);
						*shift_carry_out = ((mask & val_reg) != 0);
					}
					return shifter_operand;
				}
				else
				{
					if (shift == 0x03) /* ROR */
					{
						if (shift_imm == 0)
						{
							shifter_operand = 0;
							if (carry_in)
							{
								shifter_operand = ((uint32_t)1) << 31;
							}
							shifter_operand |= (val_reg >> 1);
							*shift_carry_out = ((0x01 & val_reg) != 0);
						}
						else
						{
							shifter_operand = RotateRight(val_reg, shift_imm);
							*shift_carry_out =
									(val_reg >> (shift_imm - 1)) & 0x1;
						}
						return shifter_operand;
					}
				}
			}
		}
	}

	return 0;
}

uint32_t
ShiftOperandRegShift(const uint32_t shift_reg,
					 const uint32_t shift,
					 const uint32_t val_reg) {
	uint32_t shifter_operand = 0;
	uint32_t sr8 = (shift_reg & 0x0FF);
	uint32_t sr5 = (shift_reg & 0x01F);

	if (shift == 0x0)
	{
		if (sr8 == 0)
		{
			shifter_operand = val_reg;
		}
		else if (sr8 < 32)
		{
			shifter_operand = val_reg << sr8;
		}
		else if (sr8 == 32)
		{
			shifter_operand = 0;
		}
		else
		{
			shifter_operand = 0;
		}
		return shifter_operand;
	}
	else
	{
		if (shift == 0x01)
		{
			if (sr8 == 0)
			{
				shifter_operand = val_reg;
			}
			else if (sr8 < 32)
			{
				shifter_operand = val_reg >> sr8;
			}
			else if (sr8 == 32)
			{
				shifter_operand = 0;
			}
			else
			{
				shifter_operand = 0;
			}
			return shifter_operand;
		}
		else
		{
			if (shift == 0x02)
			{
				if (sr8 == 0)
				{
					shifter_operand = val_reg;
				}
				else if (sr8 < 32)
				{
					shifter_operand = ((int32_t)val_reg) >> sr8;
				}
				else
				{
					if ((val_reg & ((uint32_t)1 << 31)) == 0)
					{
						shifter_operand = 0;
					}
					else
					{
						shifter_operand = (uint32_t)((int32_t)-1);
					}
				}
				return shifter_operand;
			}
			else
			{
				if (shift == 0x03)
				{
					shifter_operand = RotateRight(val_reg, sr5);
					return shifter_operand;
				}
			}
		}
	}

	return shifter_operand;
}

uint32_t
ShiftOperandRegShift(const uint32_t shift_reg,
					 const uint32_t shift,
					 const uint32_t val_reg,
					 const bool carry_in,
					 bool *shift_carry_out)
{
	uint32_t shifter_operand = 0;
	uint32_t sr8 = (shift_reg & 0x0FF);
	uint32_t sr5 = (shift_reg & 0x01F);

	if (shift == 0x0)
	{
		if (sr8 == 0)
		{
			shifter_operand = val_reg;
			*shift_carry_out = carry_in;
		}
		else if (sr8 < 32)
		{
			shifter_operand = val_reg << sr8;
			*shift_carry_out = ((val_reg & (1 << (32 - sr8))) != 0);
		}
		else if (sr8 == 32)
		{
			shifter_operand = 0;
			*shift_carry_out = ((val_reg & 0x01) != 0);
		}
		else
		{
			shifter_operand = 0;
			*shift_carry_out = false;
		}
		return shifter_operand;
	}
	else
	{
		if (shift == 0x01)
		{
			if (sr8 == 0)
			{
				shifter_operand = val_reg;
				*shift_carry_out = carry_in;
			}
			else if (sr8 < 32)
			{
				shifter_operand = val_reg >> sr8;
				*shift_carry_out = ((val_reg & (1 << (sr8 - 1))) != 0);
			}
			else if (sr8 == 32)
			{
				shifter_operand = 0;
				*shift_carry_out = ((val_reg & (1 << 31)) != 0);
			}
			else
			{
				shifter_operand = 0;
				*shift_carry_out = false;
			}
			return shifter_operand;
		}
		else
		{

			if (shift == 0x02)
			{
				if (sr8 == 0)
				{
					shifter_operand = val_reg;
					*shift_carry_out = carry_in;
				}
				else if (sr8 < 32)
				{
					shifter_operand = ((int32_t)val_reg) >> sr8;
					*shift_carry_out =
							((val_reg & ((uint32_t)1 << (sr8 - 1))) != 0);
				}
				else
				{
					if ((val_reg & ((uint32_t)1 << 31)) == 0)
					{
						shifter_operand = 0;
						*shift_carry_out =
								((val_reg & ((uint32_t)1 << 31)) != 0);
					}
					else
					{
						shifter_operand = (uint32_t)((int32_t)-1);
						*shift_carry_out =
								((val_reg & ((uint32_t)1 << 31)) != 0);
					}
				}
				return shifter_operand;
			}
			else
			{
				if (shift == 0x03)
				{
					shifter_operand = RotateRight(val_reg, sr5);
					if (sr8 == 0)
					{
						*shift_carry_out = carry_in;
					}
					else
					{
						*shift_carry_out =
								(val_reg & (1 << ((sr5 - 1) & 0x01F)));
					}
					return shifter_operand;
				}
			}
		}
	}
	return 0;
}

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
