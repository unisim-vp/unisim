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

  std::ostream&
  operator << ( std::ostream& sink, DisasmObject const& dobj )
  {
    dobj( sink );
    return sink;
  }

  static char const* const  conds_dis[] =
    {"eq", "ne", "cs", "cc", "mi", "pl", "vs", "vc", "hi", "ls", "ge", "lt", "gt", "le", "al", "<und>"};
  
  /* Condition opcode bytes disassembling method */
  void DisasmCondition::operator() ( std::ostream& sink ) const
  {
    if (m_cond < 14)
      sink << conds_dis[m_cond];
    else if (m_cond == 14)
      return;
    
    sink << "?";
    std::cerr << "ERROR(" << __FUNCTION__ << "): "
              << "unknown condition code (" << m_cond << ")" << endl;
    
  }

  void DisasmShImm::operator() ( std::ostream& sink ) const
  {
    if (m_offset) {
      switch (m_shift) {
      case 0: sink << ", lsl #"; break;
      case 1: sink << ", lsr #"; break;
      case 2: sink << ", asr #"; break;
      case 3: sink << ", ror #"; break;
      }
      sink << m_offset;
    } else {
      switch (m_shift) {
      case 0: break;
      case 1: sink << ", lsr #32"; break;
      case 2: sink << ", asr #32"; break;
      case 3: sink << ", rrx"; break;
      }
    }
  }
  
  static char const* const register_dis[] =
    {"r0","r1","r2","r3","r4","r5","r6","r7", "r8","r9","sl","fp","ip","sp","lr","pc"};

  static char const* const sornames[] = { "lsl", "lsr", "asr", "ror" };
  
  void DisasmShReg::operator() ( std::ostream& sink ) const
  {
    sink << sornames[m_shift] << " " << register_dis[m_reg];
  }

  /* Register disassembling method */
  void DisasmRegister::operator() ( std::ostream& sink ) const
  {
    sink << register_dis[m_reg];
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
