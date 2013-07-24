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

/**************************************************************/
/* Disassembling methods                                      */
/**************************************************************/

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_DISASM_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_DISASM_HH__

#include <inttypes.h>
#include <iostream>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

/* Condition opcode bytes disassembling method */
char const* DisasmCondition(uint32_t cond);

void
DisasmConditionFieldsMask(const uint32_t mask,
		std::stringstream &buffer);

/* Register disassembling method */
char const* DisasmRegister(uint32_t r);

/* Data processing operand disassembling methods */
void
DisasmShiftOperand32Imm(const uint32_t rotate_imm,
		const uint32_t imm,
		std::stringstream &buffer);
void
DisasmShiftOperandImmShift(const uint32_t shift_imm,
		const uint32_t shift,
		const uint32_t rm,
		std::stringstream &buffer);
void
DisasmShiftOperandRegShift(const uint32_t rs,
		const uint32_t shift,
		const uint32_t rm,
		std::stringstream &buffer);

/* Load/store operand disassembling methods */
void
DisasmLSWUBImmOffset_post(const uint32_t u,
		const uint32_t rn,
		const uint32_t offset,
		std::stringstream &buffer);
void
DisasmLSWUBImmOffset_offset(const uint32_t u,
		const uint32_t rn,
		const uint32_t offset,
		std::stringstream &buffer);
void
DisasmLSWUBImmOffset_pre(const uint32_t u,
		const uint32_t rn,
		const uint32_t offset,
		std::stringstream &buffer);
void
DisasmLSWUBReg_post(const uint32_t u,
		const uint32_t rn,
		const uint32_t shift_imm,
		const uint32_t shift,
		const uint32_t rm,
		std::stringstream &buffer);
void
DisasmLSWUBReg_offset(const uint32_t u,
		const uint32_t rn,
		const uint32_t shift_imm,
		const uint32_t shift,
		const uint32_t rm,
		std::stringstream &buffer);
void
DisasmLSWUBReg_pre(const uint32_t u,
		const uint32_t rn,
		const uint32_t shift_imm,
		const uint32_t shift,
		const uint32_t rm,
		std::stringstream &buffer);

/* Miscellaneous load/store operand disassembling methods */
void
DisasmMLSImmOffset_post(const uint32_t u,
		const uint32_t rn,
		const uint32_t immedH,
		const uint32_t immedL,
		std::stringstream &buffer);
void
DisasmMLSImmOffset_offset(const uint32_t u,
		const uint32_t rn,
		const uint32_t immedH,
		const uint32_t immedL,
		std::stringstream &buffer);
void
DisasmMLSImmOffset_pre(const uint32_t u,
		const uint32_t rn,
		const uint32_t immedH,
		const uint32_t immedL,
		std::stringstream &buffer);
void
DisasmMLSReg_post(const uint32_t u,
		const uint32_t rn,
		const uint32_t rm,
		std::stringstream &buffer);
void
DisasmMLSReg_offset(const uint32_t u,
		const uint32_t rn,
		const uint32_t rm,
		std::stringstream &buffer);
void
DisasmMLSReg_pre(const uint32_t u,
		const uint32_t rn,
		const uint32_t rm,
		std::stringstream &buffer);

/* Coprocessor load/store operand disassembling methods */
void
DisasmCLSImm_post(const uint32_t u,
		const uint32_t rn,
		const uint32_t offset,
		std::stringstream &buffer);
void
DisasmCLSImm_offset(const uint32_t u,
		const uint32_t rn,
		const uint32_t offset,
		std::stringstream &buffer);
void
DisasmCLSImm_pre(const uint32_t u,
		const uint32_t rn,
		const uint32_t offset,
		std::stringstream &buffer);
void
DisasmCLSUnindexed(const uint32_t rn,
		const uint32_t option,
		std::stringstream &buffer);

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif /* __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_DISASM_HH__ */
