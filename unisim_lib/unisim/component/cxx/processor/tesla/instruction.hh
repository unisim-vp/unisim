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
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_INSTRUCTION_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_INSTRUCTION_HH__

#include <unisim/component/cxx/processor/tesla/tesla_opcode.hh>
#include <unisim/component/cxx/processor/tesla/tesla_src1.hh>
#include <unisim/component/cxx/processor/tesla/tesla_src2.hh>
#include <unisim/component/cxx/processor/tesla/tesla_src3.hh>
#include <unisim/component/cxx/processor/tesla/tesla_dest.hh>
#include <unisim/component/cxx/processor/tesla/tesla_control.hh>


namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {


template <class CONFIG>
class Instruction
{
public:
	typedef typename isa::opcode::Operation<CONFIG> OpCode;
	typedef isa::dest::Operation<CONFIG> OpDest;
	typedef isa::src1::Operation<CONFIG> OpSrc1;
	typedef isa::src2::Operation<CONFIG> OpSrc2;
	typedef isa::src3::Operation<CONFIG> OpSrc3;
	typedef isa::control::Operation<CONFIG> OpControl;

	void SetOperation(OpCode *operation);
	OpCode * GetOperation() { return operation; }

	void Execute();
	void Disasm(std::ostream & os) const;
	VectorRegister<CONFIG> ReadSrc1(int offset = 0, RegType rt = RT_U32) const;
	VectorRegister<CONFIG> ReadSrc2(int offset = 0, RegType rt = RT_U32) const;
	VectorRegister<CONFIG> ReadSrc3(int offset = 0, RegType rt = RT_U32) const;
	void WriteDest(VectorRegister<CONFIG> const & value, int offset = 0) const;
	void SetPredFP32(VectorRegister<CONFIG> const & value) const;
	void SetPredI32(VectorRegister<CONFIG> const & value, VectorFlags<CONFIG> flags) const;
	void SetPredI32(VectorRegister<CONFIG> const & value) const;
	void SetPred(VectorFlags<CONFIG> flags) const;

	bitset<CONFIG::WARP_SIZE> Mask() const;
	
	void DisasmSrc1(std::ostream & os, RegType rt = RT_U32) const;
	void DisasmSrc2(std::ostream & os, RegType rt = RT_U32) const;
	void DisasmSrc3(std::ostream & os, RegType rt = RT_U32) const;
	void DisasmDest(std::ostream & os) const;
	void DisasmControl(std::ostream & os) const;
	
	bool IsLong() const;
	bool IsEnd() const;
	
	Instruction(CPU<CONFIG> * cpu, typename CONFIG::address_t addr, typename CONFIG::insn_t iw);
	~Instruction();
	
private:
	CPU<CONFIG> * cpu;
	typename CONFIG::address_t addr;
	typename CONFIG::insn_t iw;
	mutable OpCode * operation;
	mutable OpSrc1 * src1;
	mutable OpSrc2 * src2;
	mutable OpSrc3 * src3;
	mutable OpDest * dest;
	mutable OpControl * control;
	
	static isa::opcode::Decoder<CONFIG> op_decoder;
	static isa::src1::Decoder<CONFIG> src1_decoder;
	static isa::src2::Decoder<CONFIG> src2_decoder;
	static isa::src3::Decoder<CONFIG> src3_decoder;
	static isa::dest::Decoder<CONFIG> dest_decoder;
	static isa::control::Decoder<CONFIG> control_decoder;
};

#if 0

enum OperandSource
{
	OPD_NONE,
	OPD_GPR,
	OPD_IMMEDIATE,
	OPD_CONSTANT,
	OPD_SHARED,
	OPD_OUTPUT,
	OPD_LOCAL,
	OPD_GLOBAL
};

// Use polymorphism?
template<class CONFIG>
struct Operand
{
	Operand(uint32_t reg, uint32_t cm, uint32_t sh, uint32_t addr_lo, uint32_t addr_hi, uint32_t addr_imm);
	Operand(uint32_t reg, uint32_t cm, uint32_t sh);
	Operand(uint32_t reg);
	
	VectorRegister<CONFIG> Read() const;
	void Write(VectorRegister<CONFIG> const & v, bitset<CONFIG::WARP_SIZE> mask) const;
	
	void Disasm(ostream & os) const;
	
	OperandSource Type() const;
private:
	OperandSource src;
	//uint32_t address;
	VectorRegister<CONFIG> address;	//??
};

#endif

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif