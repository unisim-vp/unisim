/*
 *  Copyright (c) 2024,
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
 
#ifndef __UNISIM_SERVICE_INTERFACES_INSTRUCTION_COLLECTING_HH__
#define __UNISIM_SERVICE_INTERFACES_INSTRUCTION_COLLECTING_HH__

#include <unisim/service/interfaces/interface.hh>
#include <cstdint>

namespace unisim {
namespace service {
namespace interfaces {

struct InstructionInfoBase
{
	// Instruction types
	enum
	{
		STANDARD = 0, // flows to the next instruction
		JUMP     = 1, // jump(@target, mode: (direct | indirect)): unconditionally jumps to @target address, either directly or indirectly
		BRANCH   = 2, // branch(@target, @fallthrough, mode: (direct | indirect)): conditionally branches to @target or @fallthrough address, either directly or indirectly
		CALL     = 3, // call(@target, @fallthrough, mode: (direct | indirect)): invokes the function stored at the @target address, either directly or indirectly
		RETURN   = 4  // transfers control back to caller
	};

	// Control flow instruction modes
	enum
	{
		DIRECT   = 0, // direct
		INDIRECT = 1  // indirect
	};
	
	unsigned       type;   // one of { STANDARD, JUMP, BRANCH, CALL, RETURN }
	unsigned       size;   // size in bytes
	unsigned       mode;   // one of { DIRECT, INDIRECT }
	const uint8_t *opcode; // opcode
	
	InstructionInfoBase() : type(STANDARD), size(0), mode(DIRECT), opcode(0) {}
};

template <typename ADDRESS>
struct InstructionInfo : InstructionInfoBase
{
	ADDRESS        addr;        // @addr
	ADDRESS        target;      // @target
	ADDRESS        fallthrough; // @fallthrough
	
	InstructionInfo() : InstructionInfoBase(), addr(~ADDRESS(0)), target(~ADDRESS(0)), fallthrough(~ADDRESS(0)) {}
	
	void Standard(ADDRESS _addr, unsigned _size, const uint8_t *_opcode);
	void Jump(ADDRESS _addr, unsigned _size, const uint8_t *_opcode, ADDRESS _target, unsigned _mode);
	void Branch(ADDRESS _addr, unsigned _size, const uint8_t *_opcode, ADDRESS _target, ADDRESS _fallthrough, unsigned _mode);
	void Call(ADDRESS _addr, unsigned _size, const uint8_t *_opcode, ADDRESS _target, ADDRESS _fallthrough, unsigned _mode);
	void Return(ADDRESS _addr, unsigned _size, const uint8_t *_opcode);
};

template <typename ADDRESS>
struct InstructionCollecting : ServiceInterface
{
	virtual void CollectInstruction(const InstructionInfo<ADDRESS>& instr_info) = 0;
};

template <typename ADDRESS>
void InstructionInfo<ADDRESS>::Standard(ADDRESS _addr, unsigned _size, const uint8_t *_opcode)
{
	type = STANDARD;
	size = _size;
	mode = DIRECT;
	opcode = _opcode;
	addr = _addr;
	target = ~ADDRESS(0);
	fallthrough = ~ADDRESS(0);
}

template <typename ADDRESS>
void InstructionInfo<ADDRESS>::Jump(ADDRESS _addr, unsigned _size, const uint8_t *_opcode, ADDRESS _target, unsigned _mode)
{
	type = JUMP;
	size = _size;
	mode = _mode;
	opcode = _opcode;
	addr = _addr;
	target = _target;
	fallthrough = ~ADDRESS(0);
}

template <typename ADDRESS>
void InstructionInfo<ADDRESS>::Branch(ADDRESS _addr, unsigned _size, const uint8_t *_opcode, ADDRESS _target, ADDRESS _fallthrough, unsigned _mode)
{
	type = BRANCH;
	size = _size;
	mode = _mode;
	opcode = _opcode;
	addr = _addr;
	target = _target;
	fallthrough = _fallthrough;
}

template <typename ADDRESS>
void InstructionInfo<ADDRESS>::Call(ADDRESS _addr, unsigned _size, const uint8_t *_opcode, ADDRESS _target, ADDRESS _fallthrough, unsigned _mode)
{
	type = CALL;
	size = _size;
	mode = _mode;
	opcode = _opcode;
	addr = _addr;
	target = _target;
	fallthrough = _fallthrough;
}

template <typename ADDRESS>
void InstructionInfo<ADDRESS>::Return(ADDRESS _addr, unsigned _size, const uint8_t *_opcode)
{
	type = RETURN;
	size = _size;
	mode = DIRECT;
	opcode = _opcode;
	addr = _addr;
	target = ~ADDRESS(0);
	fallthrough = ~ADDRESS(0);
}

} // end of namespace interfaces
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_INTERFACES_INSTRUCTION_COLLECTING_HH__
