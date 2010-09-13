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

#include <inttypes.h>
#include <sstream>

#include "unisim/component/cxx/processor/arm/disasm.hh"
#include "unisim/component/cxx/processor/arm/masks.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

using namespace std;

/* Condition opcode bytes disassembling method */
void
DisasmCondition(const uint32_t cond, stringstream &buffer)
{
	switch (cond)
	{
	case COND_EQ:
		buffer << "eq";
		break;
	case COND_NE:
		buffer << "ne";
		break;
	case COND_CS_HS:
		buffer << "cs/hs";
		break;
	case COND_CC_LO:
		buffer << "cc/lo";
		break;
	case COND_MI:
		buffer << "mi";
		break;
	case COND_PL:
		buffer << "pl";
		break;
	case COND_VS:
		buffer << "vs";
		break;
	case COND_VC:
		buffer << "vc";
		break;
	case COND_HI:
		buffer << "hi";
		break;
	case COND_LS:
		buffer << "ls";
		break;
	case COND_GE:
		buffer << "ge";
		break;
	case COND_LT:
		buffer << "lt";
		break;
	case COND_GT:
		buffer << "gt";
		break;
	case COND_LE:
		buffer << "le";
		break;
	case COND_AL:
		// buffer << "al";
		break;
	default:
		cerr << "ERROR(" << __FUNCTION__ << "): "
			<< "unknown condition code (" << cond << ")" << endl;
		break;
	}
}

void
DisasmConditionFieldsMask(const uint32_t mask,
		stringstream &buffer)
{
	if ((mask & 0x01) == 0x01) buffer << "c";
	if ((mask & 0x02) == 0x02) buffer << "x";
	if ((mask & 0x04) == 0x04) buffer << "s";
	if ((mask & 0x08) == 0x08) buffer << "f";
}

/* Data processing operand disassembling methods */
void
DisasmShiftOperand32Imm(const uint32_t rotate_imm,
		const uint32_t imm,
		stringstream &buffer)
{
	uint32_t imm_r, imm_l; // immediate right and left rotated
	uint32_t imm_f; // final immediate

	imm_r = imm >> (rotate_imm * 2);
	imm_l = imm << (32 - (rotate_imm * 2));

	imm_f = imm_r | imm_l;
	buffer << "#" << dec << imm_f;
}

void
DisasmShiftOperandImmShift(const uint32_t shift_imm,
		const uint32_t shift,
		const uint32_t rm,
		stringstream &buffer) {
	buffer << "r" << rm;
	if ((shift_imm == 0) && (shift == 0))
	{
		return;
	}

	if ((shift_imm == 0) && (shift == 0x01))
	{
		buffer << ", rrx";
		return;
	}

	buffer << ", ";
	switch (shift)
	{
	case 0x00:
		buffer << "lsl";
		break;
	case 0x01:
		buffer << "lsr";
		break;
	case 0x02:
		buffer << "asr";
		break;
	case 0x03:
		buffer << "ror";
		break;
	default:
		buffer << "(?)";
		cerr << "ERROR(" << __FUNCTION__ << "): ";
		cerr << "unexpected case found disassembling (shift val = "
			<< dec << shift << ")." << endl;
		break;
	}

	buffer << " #" << dec << shift_imm;
}

void
DisasmShiftOperandRegShift(const uint32_t rs,
		const uint32_t shift,
		const uint32_t rm,
		stringstream &buffer)
{
	buffer << "r" << rm;
	buffer << ", ";
	switch (shift)
	{
	case 0x00:
		buffer << "lsl";
		break;
	case 0x01:
		buffer << "lsr";
		break;
	case 0x02:
		buffer << "asr";
		break;
	case 0x03:
		buffer << "ror";
		break;
	default:
		buffer << "(?)";
		cerr << "ERROR(" << __FUNCTION__ << "): ";
		cerr << "unexpected case found disassembling (shift val = "
			<< hex << shift << dec << ")." << endl;
		break;
	}
	buffer << " r" << rs;
}

/* Load/store operand disassembling methods */
void
DisasmLSWUBImmOffset_post(const uint32_t u,
		const uint32_t rn,
		const uint32_t offset,
		stringstream &buffer)
{
	buffer << "[r" << rn << "], "
			<< "#" << (u == 0 ? "-" : "") << dec << offset;
}

void
DisasmLSWUBImmOffset_offset(const uint32_t u,
		const uint32_t rn,
		const uint32_t offset,
		stringstream &buffer)
{
	buffer << "[r" << rn << ", "
			<< "#" << (u == 0 ? "-" : "") << dec << offset << "]";
}

void
DisasmLSWUBImmOffset_pre(const uint32_t u,
		const uint32_t rn,
		const uint32_t offset,
		stringstream &buffer)
{
	buffer << "[r" << rn << ", "
			<< "#" << (u == 0 ? "-" : "") << dec << offset << "]!";
}

void
DisasmLSWUBReg_post(const uint32_t u,
		const uint32_t rn,
		const uint32_t shift_imm,
		const uint32_t shift,
		const uint32_t rm,
		stringstream &buffer)
{
	buffer << "[r" << rn << "], "
			<< (u == 0 ? "-" : "") << "r" << rm;
	if (!((shift_imm == 0) && (shift == 0)))
	{
		buffer << ", ";
		switch (shift)
		{
		case 0:
			buffer << "lsl";
			break;
		case 1:
			buffer << "lsr";
			break;
		case 2:
			buffer << "asr";
			break;
		case 3:
			if (shift_imm == 0) buffer << "rrx";
			else buffer << "ror";
			break;
		default:
			buffer << "(?)";
			cerr << "ERROR(" << __FUNCTION__ << "): "
				<< "unknown shift value (" << dec << shift << ")" << endl;
			break;
		}
		if ((shift != 3) && (shift_imm != 0))
			buffer << " #" << shift_imm;
	}
}

void
DisasmLSWUBReg_offset(const uint32_t u,
		const uint32_t rn,
		const uint32_t shift_imm,
		const uint32_t shift,
		const uint32_t rm,
		stringstream &buffer)
{
	buffer << "[r" << rn << ", "
			<< (u == 0 ? "-" : "") << "r" << rm;
	if (!((shift_imm == 0) && (shift == 0)))
	{
		buffer << ", ";
		switch (shift)
		{
		case 0:
			buffer << "lsl";
			break;
		case 1:
			buffer << "lsr";
			break;
		case 2:
			buffer << "asr";
			break;
		case 3:
			if (shift_imm == 0) buffer << "rrx";
			else buffer << "ror";
			break;
		default:
			buffer << "(?)";
			cerr << "ERROR(" << __FUNCTION__ << "): "
				<< "unknown shift value (" << dec << shift << ")" << endl;
			break;
		}
		if (shift != 3 && shift_imm != 0)
			buffer << " #" << shift_imm;
	}
	buffer << "]";
}

void
DisasmLSWUBReg_pre(const uint32_t u,
		const uint32_t rn,
		const uint32_t shift_imm,
		const uint32_t shift,
		const uint32_t rm,
		stringstream &buffer)
{
	buffer << "[r" << rn << ", "
			<< (u == 0 ? "-" : "") << "r" << rm;
	if (!((shift_imm == 0) && (shift == 0)))
	{
		buffer << ", ";
		switch (shift)
		{
		case 0:
			buffer << "lsl";
			break;
		case 1:
			buffer << "lsr";
			break;
		case 2:
			buffer << "asr";
			break;
		case 3:
			if (shift_imm == 0) buffer << "rrx";
			else buffer << "ror";
			break;
		default:
			buffer << "(?)";
			cerr << "ERROR(" << __FUNCTION__ << "): "
				<< "unknown shift value (" << dec << shift << ")" << endl;
			break;
		}
		if ((shift != 3) && (shift_imm != 0))
			buffer << " #" << shift_imm;
	}
	buffer << "]!";
}

/* Miscellaneous load/store operand disassembling methods */
void
DisasmMLSImmOffset_post(const uint32_t u,
		const uint32_t rn,
		const uint32_t immedH,
		const uint32_t immedL,
		stringstream &buffer)
{
	buffer << "[r" << rn << "], "
			<< "#" << ((u == 1) ? "" : "-")
			<< (immedH << 4) + immedL;
}

void
DisasmMLSImmOffset_offset(const uint32_t u,
		const uint32_t rn,
		const uint32_t immedH,
		const uint32_t immedL,
		stringstream &buffer)
{
	buffer << "[r" << rn << ", "
			<< "#" << ((u == 1) ? "" : "-")
			<< (immedH << 4) + immedL << "]";
}

void
DisasmMLSImmOffset_pre(const uint32_t u,
		const uint32_t rn,
		const uint32_t immedH,
		const uint32_t immedL,
		stringstream &buffer)
{
	buffer << "[r" << rn << ", "
			<< "#" << ((u == 1) ? "" : "-")
			<< (immedH << 4) + immedL << "]!";
}

void
DisasmMLSReg_post(const uint32_t u,
		const uint32_t rn,
		const uint32_t rm,
		stringstream &buffer)
{
	buffer << "[r" << rn << "], "
			<< ((u == 1) ? "" : "-") << "r" << rm;
}

void
DisasmMLSReg_offset(const uint32_t u,
		const uint32_t rn,
		const uint32_t rm,
		stringstream &buffer)
{
	buffer << "[r" << rn << ", "
			<< ((u == 1) ? "" : "-") << "r" << rm << "]";
}

void
DisasmMLSReg_pre(const uint32_t u,
		const uint32_t rn,
		const uint32_t rm,
		stringstream &buffer)
{
	buffer << "[r" << rn << ", "
			<< ((u == 1) ? "" : "-") << "r" << rm << "]!";
}

/* Coprocessor load/store operand disassembling methods */
void
DisasmCLSImm_post(const uint32_t u,
		const uint32_t rn,
		const uint32_t offset,
		stringstream &buffer)
{
	buffer << "[r" << rn << "], #"
			<< ((u == 1) ? "" : "-") << offset * 4;
}

void
DisasmCLSImm_offset(const uint32_t u,
		const uint32_t rn,
		const uint32_t offset,
		stringstream &buffer)
{
	buffer << "[r" << rn << ", #"
			<< ((u == 1) ? "" : "-") << offset * 4 << "]";
}

void
DisasmCLSImm_pre(const uint32_t u,
		const uint32_t rn,
		const uint32_t offset,
		stringstream &buffer)
{
	buffer << "[r" << rn << ", #"
			<< ((u == 1) ? "" : "-") << offset * 4 << "]!";
}

void
DisasmCLSUnindexed(const uint32_t rn,
		const uint32_t option,
		stringstream &buffer)
{
	buffer << "[r" << rn << "], "
			<< "{" << option << "}";
}

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
