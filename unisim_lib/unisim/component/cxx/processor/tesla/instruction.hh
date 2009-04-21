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
 
#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_INSTRUCTION_HH
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_INSTRUCTION_HH

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

// Location of operands
enum Domain
{
	DomainNone,	// no operand
	DomainGPR,
	DomainAR,	// address register
	DomainPred,	// pred/flag register
	DomainConst,
	DomainShared,
	DomainGlobal,	// or local
};

enum Operand
{
	OpDest,
	OpSrc1,
	OpSrc2,
	OpSrc3
};

// Instruction:
// Instruction as passed from a pipeline stage to the next.
// Dynamic, allocated during decode stage, discarded on retirement.
// Multiple Instructions in flight for the same Operation are possible.
template <class CONFIG>
class Instruction
{
public:
	// External interface: with CPU
	// Loosely matches pipeline stages
	void Read();
	void Execute();
	void Write();
	void Disasm(std::ostream & os) const;

	// Interface with Operation*
	VectorRegister<CONFIG> const & Temp(unsigned int i) const;
	VectorRegister<CONFIG> & Temp(unsigned int i);
	// Temp(0) = src1 and dest
	// Temp(1) = src1.hi and dest.hi (64-bit)
	// Temp(2) = src2  or src1.hilo (128-bit)
	// Temp(3) = src2.hi (64-bit) or src1.hihi (128-bit)
	// Temp(4) = src3
	// Temp(5) = src3.hi (64-bit)
	enum {
		TempCount = 6,
		TempSrc1 = 0,
		TempSrc2 = 2,
		TempSrc3 = 4,
		TempDest = 0
	};
	
	VectorRegister<CONFIG> & GetSrc1();
	VectorRegister<CONFIG> & GetSrc2();
	VectorRegister<CONFIG> & GetSrc3();
	VectorRegister<CONFIG> & GetDest();
	VectorFlags<CONFIG> & Flags();

	void WriteDest();
	void SetPredI32();
	void SetPredI16();
	void SetPredF32();
	
	void ReadBlock(int reg, DataType dt);
	void WriteBlock(int reg, DataType dt);
	void ReadReg(int reg, int tempbase, RegType rt);
	void WriteReg(int reg, int tempbase, RegType rt,
		std::bitset<CONFIG::WARP_SIZE> mask);

	void ReadSrc1();
	void ReadSrc2();
	void ReadSrc3();

	// Deprecated interface
	typedef typename isa::opcode::Operation<CONFIG> OpCode;
	typedef isa::dest::Operation<CONFIG> OpDest;
	typedef isa::src1::Operation<CONFIG> OpSrc1;
	typedef isa::src2::Operation<CONFIG> OpSrc2;
	typedef isa::src3::Operation<CONFIG> OpSrc3;
	typedef isa::control::Operation<CONFIG> OpControl;
	void SetOperation(OpCode *operation);
	OpCode * GetOperation() { return operation; }

	void SetDest(VectorRegister<CONFIG> const & value);
	void SetPredFP32(VectorRegister<CONFIG> const & value) const;
	void SetPredI32(VectorRegister<CONFIG> const & value, VectorFlags<CONFIG> flags) const;
	void SetPredI32(VectorRegister<CONFIG> const & value) const;
	void SetPred(VectorFlags<CONFIG> flags) const;

	bitset<CONFIG::WARP_SIZE> Mask() const;
	
	void DisasmSrc1(std::ostream & os) const;
	void DisasmSrc2(std::ostream & os) const;
	void DisasmSrc3(std::ostream & os) const;
	void DisasmDest(std::ostream & os) const;
	void DisasmControl(std::ostream & os) const;
	
	bool IsLong() const;
	bool IsEnd() const;
	
	Instruction(CPU<CONFIG> * cpu, typename CONFIG::address_t addr, typename CONFIG::insn_t iw);
	~Instruction();
	
	RegType OperandRegType(Operand op) const;		// 
	DataType OperandDataType(Operand op) const;	// same value, different type (RegType more restricted)
	SMType	OperandSMType(Operand op) const;
	size_t OperandSize(Operand op) const;
	Domain OperandDomain(Operand op) const;
	bool AllowSegment() const;
	bool ForceReg(Operand op) const;
	size_t NumGPRsIn() const;
	VectorAddress<CONFIG> EffectiveAddress(Operand op) const;
	
private:
	CPU<CONFIG> * cpu;
	typename CONFIG::address_t addr;
	typename CONFIG::insn_t iw;
	mutable OpCode * operation;
	
	static isa::opcode::Decoder<CONFIG> op_decoder;

	VectorRegister<CONFIG> temp[TempCount];
	VectorFlags<CONFIG> flags;
	std::bitset<CONFIG::WARP_SIZE> mask;
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
