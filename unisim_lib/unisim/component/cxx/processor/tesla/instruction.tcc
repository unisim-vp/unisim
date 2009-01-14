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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_INSTRUCTION_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_INSTRUCTION_TCC__

#include <unisim/component/cxx/processor/tesla/instruction.hh>
#include <unisim/component/cxx/processor/tesla/tesla_opcode.tcc>
#include <unisim/component/cxx/processor/tesla/tesla_dest.tcc>
#include <unisim/component/cxx/processor/tesla/tesla_src1.tcc>
#include <unisim/component/cxx/processor/tesla/tesla_src2.tcc>
#include <unisim/component/cxx/processor/tesla/tesla_src3.tcc>


namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {


template <class CONFIG>
isa::opcode::Decoder<CONFIG> Instruction<CONFIG>::op_decoder;

template <class CONFIG>
isa::src1::Decoder<CONFIG> Instruction<CONFIG>::src1_decoder;

template <class CONFIG>
isa::src2::Decoder<CONFIG> Instruction<CONFIG>::src2_decoder;

template <class CONFIG>
isa::src3::Decoder<CONFIG> Instruction<CONFIG>::src3_decoder;

template <class CONFIG>
isa::dest::Decoder<CONFIG> Instruction<CONFIG>::dest_decoder;

template <class CONFIG>
Instruction<CONFIG>::Instruction(CPU<CONFIG> * cpu, typename CONFIG::address_t addr, typename CONFIG::insn_t iw) :
	cpu(cpu), addr(addr), iw(iw), operation(0), src1(0), src2(0), src3(0), dest(0)
{
}

template <class CONFIG>
Instruction<CONFIG>::~Instruction()
{
}

template <class CONFIG>
void Instruction<CONFIG>::Execute()
{
	if(operation == 0) {
		operation = op_decoder.Decode(addr, iw);
	}
	operation->execute(cpu, this);
}

template <class CONFIG>
void Instruction<CONFIG>::Disasm(std::ostream & os) const
{
	if(operation == 0) {
		operation = op_decoder.Decode(addr, iw);
	}
	operation->disasm(cpu, this, os);
}

template <class CONFIG>
VectorRegister<CONFIG> Instruction<CONFIG>::ReadSrc1(int offset) const
{
	if(src1 == 0) {
		src1 = src1_decoder.Decode(addr, iw);
	}
	return src1->read(cpu, offset);
}

template <class CONFIG>
VectorRegister<CONFIG> Instruction<CONFIG>::ReadSrc2(int offset) const
{
	if(src2 == 0) {
		src2 = src2_decoder.Decode(addr, iw);
	}
	return src2->read(cpu, offset);
}

template <class CONFIG>
VectorRegister<CONFIG> Instruction<CONFIG>::ReadSrc3(int offset) const
{
	if(src3 == 0) {
		src3 = src3_decoder.Decode(addr, iw);
	}
	return src3->read(cpu, offset);
}

template <class CONFIG>
void Instruction<CONFIG>::WriteDest(VectorRegister<CONFIG> const & value, bitset<CONFIG::WARP_SIZE> mask, int offset) const
{
	if(dest == 0) {
		dest = dest_decoder.Decode(addr, iw);
	}
	// apply current warp mask
	mask &= cpu->CurrentWarp().mask;
	dest->write(cpu, value, mask, offset);
}

template <class CONFIG>
void Instruction<CONFIG>::SetPredFP32(VectorRegister<CONFIG> const & value, bitset<CONFIG::WARP_SIZE> mask) const
{
}

template <class CONFIG>
void Instruction<CONFIG>::SetPredI32(VectorRegister<CONFIG> const & value, int carry, int ovf, bitset<CONFIG::WARP_SIZE> mask) const
{
}

template <class CONFIG>
bitset<CONFIG::WARP_SIZE> Instruction<CONFIG>::Mask() const
{
}

template <class CONFIG>
void Instruction<CONFIG>::DisasmSrc1(std::ostream & os) const
{
	if(src1 == 0) {
		src1 = src1_decoder.Decode(addr, iw);
	}
	src1->disasm(cpu, this, os);
}

template <class CONFIG>
void Instruction<CONFIG>::DisasmSrc2(std::ostream & os) const
{
	if(src2 == 0) {
		src2 = src2_decoder.Decode(addr, iw);
	}
	src2->disasm(cpu, this, os);
}

template <class CONFIG>
void Instruction<CONFIG>::DisasmSrc3(std::ostream & os) const
{
	if(src3 == 0) {
		src3 = src3_decoder.Decode(addr, iw);
	}
	src3->disasm(cpu, this, os);
}


template <class CONFIG>
void Instruction<CONFIG>::DisasmDest(std::ostream & os) const
{
	if(dest == 0) {
		dest = dest_decoder.Decode(addr, iw);
	}
	dest->disasm(cpu, this, os);
}



} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
