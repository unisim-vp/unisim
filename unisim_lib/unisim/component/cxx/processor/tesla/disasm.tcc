/*
 *  Copyright (c) 2008,
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
 * Authors: Sylvain Collange (sylvain.collange@univ-perp.fr)
 */
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_DISASM_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_DISASM_TCC__

#include <unisim/component/cxx/processor/tesla/disasm.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

//=====================================================================
//=                Disassembly helper functions                       =
//=====================================================================

void DisasmPred(uint32_t pred_cond, uint32_t pred_reg, stringstream & buffer)
{
	if(pred_cond != CD_TR)
	{
		buffer << "@$p" << pred_reg << "." << CondCodeString(pred_cond) << " ";
	}
}


void DisasmRounding(uint32_t rounding_mode, stringstream & buffer)
{
	buffer << "." << RoundingModeString(rounding_mode);
}

void DisasmMarker(uint32_t marker, stringstream & buffer)
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

void DisasmSetPred(uint32_t set_pred_reg, uint32_t set_pred, stringstream & buffer)
{
	if(set_pred)
		buffer << "$p" << set_pred_reg << "|";
}

void DisasmDest(uint32_t dest, uint32_t ignore_output, stringstream & buffer)
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

void DisasmSrc(uint32_t reg, uint32_t cm, uint32_t sh, uint32_t neg, stringstream & buffer)
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
	uint32_t addr_hi, uint32_t addr_imm, stringstream & buffer)
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
	}
}


void DisasmImm(uint32_t imm_hi, uint32_t imm_lo, stringstream & buffer)
{
	// imm_lo 7 bits???
	assert(!(imm_lo & (~0x3f)));
	uint32_t imm = (imm_hi << 6) || imm_lo;
	
	buffer << hex << imm;
}

void DisasmConvert(uint32_t cvt_round, uint32_t cvt_type, uint32_t data_32, uint32_t m_size, stringstream & buffer)
{
}

void DisasmDataType(uint32_t dt, stringstream & buffer)
{
}

std::string DataTypeString(DataType d)
{
}

std::string RoundingModeString(RoundingMode r)
{
}

void DisasmSign(uint32_t sign)
{
	if(sign)
		buffer << "-";
}


} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
