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
 
#include <unisim/component/cxx/processor/tesla/disasm.hh>
#include <ostream>
#include <cassert>
#include <iomanip>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

using namespace std;

//=====================================================================
//=                Disassembly helper functions                       =
//=====================================================================

void DisasmPred(uint32_t pred_cond, uint32_t pred_reg, ostream & buffer)
{
	if(pred_cond != CD_TR)
	{
		buffer << "@$p" << pred_reg << "." << CondCodeString(Cond(pred_cond)) << " ";
	}
}


void DisasmRounding(uint32_t rounding_mode, ostream & buffer)
{
	buffer << RoundingModeString(RoundingMode(rounding_mode));
}

void DisasmMarker(uint32_t marker, ostream & buffer)
{
	switch(marker)
	{
	case 0:
		break;
	case 1:
		buffer << ".end";
		break;
	case 2:
		buffer << ".join";
		break;
	case 3:
		assert(false);
		break;
	}
}

void DisasmSetPred(uint32_t set_pred_reg, uint32_t set_pred, ostream & buffer)
{
	if(set_pred)
		buffer << "$p" << set_pred_reg << "|";
}

void DisasmDest(uint32_t dest, uint32_t ignore_output, ostream & buffer)
{
	if(!ignore_output)
	{
		buffer << "$r" << dest;
	}
	else
	{
		buffer << "_";
	}
}

void DisasmSrc(uint32_t reg, uint32_t cm, uint32_t sh, uint32_t neg, ostream & buffer)
{
	// TODO: segment??
	// TODO: handle 16-bit operands (hi, lo)
	assert(!(cm && sh));
	if(cm)
		buffer << "c[" << reg * 4 << "]";
	else if(sh)
		buffer << "s[" << reg * 4 << "]";
	else
		buffer << "$r" << reg;
}

void DisasmSrc(uint32_t reg, uint32_t cm, uint32_t sh, uint32_t neg, uint32_t addr_lo,
	uint32_t addr_hi, uint32_t addr_imm, ostream & buffer)
{
	uint32_t addr_reg = (addr_hi << 2) | addr_lo;

	// addr_imm = ???	
	if(addr_imm == 0)
	{
		DisasmSrc(reg, cm, sh, neg, buffer);
	}
	else
	{
		// Indirect access:
		// [$a#addr_reg + reg * 4]
		buffer << "???";	// TODO
	}
}


void DisasmImm(uint32_t imm_hi, uint32_t imm_lo, ostream & buffer)
{
	// imm_lo 7 bits???
	assert(!(imm_lo & (~0x3f)));
	uint32_t imm = (imm_hi << 6) || imm_lo;
	
	buffer << hex << imm;
}

void DisasmConvert(uint32_t cvt_round, uint32_t cvt_type, uint32_t data_32, ostream & buffer)
{
	if(data_32 == 0)
		buffer << ".b16";
	buffer << ConvTypeString(ConvType(cvt_type)) << RoundingModeString(RoundingMode(cvt_round));
}

void DisasmDataType(uint32_t dt, ostream & buffer)
{
	buffer << DataTypeString(DataType(dt));
}

std::string DataTypeString(DataType d)
{
	static char const * dt_desc[] = {
		".u8",	// 0
		".s8",
		".u16",
		".s16",
		".u64",
		".u128",
		".u32",
		".s32"	// 7
	};
	assert(d >= 0 && d < 8);
	return string(dt_desc[d]);
}

std::string RoundingModeString(RoundingMode r)
{
	static char const * rm_desc[] = {
		".rn",	// 0
		".rd",
		".ru",
		".rz"	// 3
	};
	assert(r >= 0 && r < 4);
	return string(rm_desc[r]);
}

std::string ConvTypeString(ConvType t)
{
	static char const * ct_desc[] = {
		".u16",	// 0
		".u32",
		".u8",
		".??",
		".s16",
		".s32",
		".s8",
		""	// 7
	};
	assert(t >= 0 && t < 8);
	return string(ct_desc[t]);
}

std::string SMTypeString(SMType t)
{
	static char const * sm_desc[] = {
		".u8",	// 0
		".u16",
		".s16",
		".u32"	// 3
	};
	assert(t >= 0 && t < 4);
	return string(sm_desc[t]);
}

void DisasmSign(uint32_t sign, ostream & buffer)
{
	if(sign)
		buffer << "-";
}

void DisasmGlobal(uint32_t dest, uint32_t addr_lo, uint32_t addr_hi, uint32_t addr_imm, uint32_t segment, ostream & buffer)
{
	uint32_t addr_reg = (addr_hi << 2) | addr_lo;
	buffer << "g" << segment << "[";
	if(addr_reg == 0)
	{
		if(addr_imm)
		{
			// g_segment[dest]
			buffer << "0x" << std::setfill('0') << std::setw(4) << std::hex << dest << dec;
		}
		else
		{
			// g_segment[r_dest]
			buffer << "r" << dest;
		}
	}
	else
	{
		buffer << "a??";	// TODO
	}
	
	buffer << "]";
}

void DisasmShared(uint32_t addr, uint32_t type, ostream & buffer)
{
	static int sm_size[] = {
		1, 2, 2, 4
	};
	SMType smt = SMType(type);
	assert(smt < 4);
	addr *= sm_size[smt];
	buffer << "s" << SMTypeString(smt)
		<< "[" << "0x" << std::setfill('0') << std::setw(4) << std::hex << addr << "]";
	buffer << std::dec;
}

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
