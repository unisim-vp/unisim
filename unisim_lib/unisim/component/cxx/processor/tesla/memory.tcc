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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_MEMORY_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_MEMORY_TCC__

// Declarations in cpu.hh

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {


template <class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::ReadConstant(VectorRegister<CONFIG> const & addr, uint32_t seg)
{
	assert(seg < CONFIG::CONST_SEG_NUM);
	VecReg v;
	Gather32(VecAddr(addr), v, 1, CONFIG::CONST_START + seg * CONFIG::CONST_SEG_SIZE);
	return v;
}

template <class CONFIG>
VectorRegister<CONFIG> CPU<CONFIG>::ReadConstant(int addr, uint32_t seg)
{
	assert(seg < CONFIG::CONST_SEG_NUM);
	VecReg v;
	Broadcast32(addr, v, 1, CONFIG::CONST_START + seg * CONFIG::CONST_SEG_SIZE);
	return v;
}



template <class CONFIG>
VectorAddress<CONFIG> CPU<CONFIG>::EffectiveAddress(uint32_t reg, uint32_t addr_lo, uint32_t addr_hi,
	uint32_t addr_imm, uint32_t shift)
{
	uint32_t addr_reg = (addr_hi << 2) | addr_lo;
	VecAddr offset;
	// [$a#addr_reg + dest]
	if(addr_imm) {
		// Immediate in words
		offset = VecAddr(reg << shift);
	}
	else {
		offset = VecAddr(GetGPR(reg));
	}

	if(addr_reg != 0) {
		offset += GetAddr(addr_reg);
	}
	return offset;
}

template <class CONFIG>
void CPU<CONFIG>::GatherShared(VectorRegister<CONFIG> & output, uint32_t src, uint32_t addr_lo, uint32_t addr_hi, uint32_t addr_imm, std::bitset<CONFIG::WARP_SIZE> mask, SMType type)
{
	uint32_t shift = 0;
	if(type == SM_U16 || type == SM_S16) shift = 1;
	else if(type == SM_U32) shift = 2;
	VecAddr offset = EffectiveAddress(src, addr_lo, addr_hi, addr_imm, shift);

//	if(CONFIG::TRACE)
//		cerr << " GatherShared, EA = " << offset << endl;
	GatherShared(offset, output, mask, type);
}

// For current block
template <class CONFIG>
void CPU<CONFIG>::GatherShared(VectorAddress<CONFIG> const & addr,
	VectorRegister<CONFIG> & data, std::bitset<CONFIG::WARP_SIZE> mask, SMType t)
{
	switch(t)
	{
	case SM_U32:
		Gather32(addr, data, mask, 1, CurrentWarp().GetSMAddress());
		break;
	case SM_U16:
	case SM_S16:
		Gather16(addr, data, mask, 1, CurrentWarp().GetSMAddress());
		break;
	case SM_U8:
		throw "Not implemented!";
	default:
		assert(false);
	}
}

template <class CONFIG>
void CPU<CONFIG>::ScatterShared(VectorRegister<CONFIG> const & output, uint32_t dest,
	uint32_t addr_lo, uint32_t addr_hi, uint32_t addr_imm,
	std::bitset<CONFIG::WARP_SIZE> mask, SMType type)
{

	uint32_t shift = 0;
	if(type == SM_U16 || type == SM_S16) shift = 1;
	else if(type == SM_U32) shift = 2;
	VecAddr offset = EffectiveAddress(dest, addr_lo, addr_hi, addr_imm, shift);

	// TODO
	assert(type == SM_U32);	// b32
	
	// TODO: check memory range
	address_t base = CurrentWarp().GetSMAddress();
	Scatter32(offset, output, mask, 1, base);
}

// For current block
template <class CONFIG>
void CPU<CONFIG>::ReadShared(typename CONFIG::address_t addr, VectorRegister<CONFIG> & data, SMType t)
{
	switch(t)
	{
	case SM_U32:
		//Gather32(addr, data, mask, 4, CurrentWarp().GetSMAddress());
		Broadcast32(addr, data, 4, CurrentWarp().GetSMAddress());
		break;
	case SM_U16:
	case SM_S16:
		//Gather16(addr, data, mask, 1, CurrentWarp().GetSMAddress());
		Broadcast16(addr, data, 2, CurrentWarp().GetSMAddress());
		break;
	case SM_U8:
		throw "Not implemented!";
	default:
		assert(false);
	}
}

template <class CONFIG>
void CPU<CONFIG>::Gather32(VecAddr const & addr, VecReg & data, std::bitset<CONFIG::WARP_SIZE> mask,
	uint32_t factor, address_t offset)
{
	for(int i = 0; i != WARP_SIZE; ++i)
	{
		if(mask[i]) {
			Read32(addr[i], data[i], factor, offset);
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::Scatter32(VecAddr const & addr, VecReg const & data,
	std::bitset<CONFIG::WARP_SIZE> mask,
	uint32_t factor, address_t offset)
{
	// TODO: check overlap and warn
	for(int i = 0; i != WARP_SIZE; ++i)
	{
		if(mask[i]) {
			Write32(addr[i], data[i], factor, offset);
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::Gather16(VecAddr const & addr, VecReg & data, std::bitset<CONFIG::WARP_SIZE> mask,
	uint32_t factor, address_t offset)
{
	for(int i = 0; i != WARP_SIZE; ++i)
	{
		if(mask[i]) {
			Read16(addr[i], data[i], factor, offset);
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::Scatter16(VecAddr const & addr, VecReg const & data,
	std::bitset<CONFIG::WARP_SIZE> mask,
	uint32_t factor, address_t offset)
{
	// TODO: check overlap and warn
	for(int i = 0; i != WARP_SIZE; ++i)
	{
		if(mask[i]) {
			Write16(addr[i], data[i], factor, offset);
		}
	}
}


template <class CONFIG>
void CPU<CONFIG>::Broadcast32(address_t addr, VecReg & data,
	uint32_t factor, address_t offset)
{
	uint32_t val;
	Read32(addr, val, factor, offset);
	data = VecReg(val);
}

// TODO: add mask
template <class CONFIG>
void CPU<CONFIG>::Broadcast16(address_t addr, VecReg & data,
	uint32_t factor, address_t offset)
{
	uint32_t val;
	Read16(addr, val, factor, offset);
	data = VecReg(val);	// mask...
}


template <class CONFIG>
void CPU<CONFIG>::Read32(address_t addr, uint32_t & data,
	uint32_t factor, address_t offset)
{
	if(!ReadMemory(addr * factor + offset, &data, 4)) {
		throw MemoryAccessException<CONFIG>();
	}
	if(CONFIG::TRACE) {
		cerr << " Read32 @" << std::hex << offset << "+" << addr << "*" << factor << ": "
			<< data << std::dec << endl;
	}
}

template <class CONFIG>
void CPU<CONFIG>::Write32(address_t addr, uint32_t data,
	uint32_t factor, address_t offset)
{
	if(CONFIG::TRACE) {
		cerr << " Write32: " << std::hex << data
			<< " @" << offset << "+" << addr << "*" << factor << std::dec << endl;
	}
	if(!WriteMemory(addr * factor + offset, &data, 4)) {
		throw MemoryAccessException<CONFIG>();
	}
}

template <class CONFIG>
void CPU<CONFIG>::Read16(address_t addr, uint32_t & data,
	uint32_t factor, address_t offset)
{
	if(!ReadMemory(addr * factor + offset, &data, 2)) {
		throw MemoryAccessException<CONFIG>();
	}
	if(CONFIG::TRACE) {
		cerr << " Read16 @" << std::hex << offset << "+" << addr << "*" << factor << ": "
			<< data << std::dec << endl;
	}
}

template <class CONFIG>
void CPU<CONFIG>::Write16(address_t addr, uint32_t data,
	uint32_t factor, address_t offset)
{
	if(CONFIG::TRACE) {
		cerr << " Write16: " << std::hex << data
			<< " @" << offset << "+" << addr << "*" << factor << std::dec << endl;
	}
	if(!WriteMemory(addr * factor + offset, &data, 2)) {
		throw MemoryAccessException<CONFIG>();
	}
}


template <class CONFIG>
void CPU<CONFIG>::ScatterGlobal(VecReg output, uint32_t dest, uint32_t addr_lo, uint32_t addr_hi, uint32_t addr_imm, uint32_t segment, std::bitset<CONFIG::WARP_SIZE> mask, DataType dt)
{
	if(dt != DT_U32 && dt != DT_S32) {
		throw "Not implemented!";
	}
	int width = 4;

	uint32_t addr_reg = (addr_hi << 2) | addr_lo;
	// [seg][$a#addr_reg + dest]
	VecAddr offset;
	if(addr_imm) {
		cerr << "Warning: unchecked immediate field!\n";
		// Make sure immediate is in words!
		if(dt == DT_U16 || dt == DT_S16) dest << 1;	// 16-bit
		else if(dt == DT_U32 || dt == DT_S32) dest << 2;	// 32-bit
		else if(dt == DT_U64) dest << 3;	// 32-bit
		else if(dt == DT_U128) dest << 4;

		offset = VecAddr(dest);
	}
	else {
		offset = VecAddr(GetGPR(dest));
	}

	if(addr_reg != 0) {
		offset += GetAddr(addr_reg);
	}
	
	// TODO: segment??
	address_t base = 0;
	Scatter32(offset, output, mask, 1, base);
}

template <class CONFIG>
void CPU<CONFIG>::GatherGlobal(VecReg & output, uint32_t src, uint32_t addr_lo, uint32_t addr_hi, uint32_t addr_imm, uint32_t segment, std::bitset<CONFIG::WARP_SIZE> mask, DataType dt)
{
	if(dt != DT_U32 && dt != DT_S32) {
		throw "Not implemented!";
	}
	int width = 4;

	uint32_t addr_reg = (addr_hi << 2) | addr_lo;
	// [seg][$a#addr_reg + dest]
	if(addr_reg != 0) {
		throw "Not implemented!";
	}
	VecReg offset;
	if(addr_imm) {
		cerr << "Warning: unchecked immediate field!\n";
		offset = VecReg(src);	// TODO: CHECK immediate in words???
	}
	else {
		offset = GetGPR(src);
	}
	
	// TODO: segment??
	address_t base = 0;
	Gather32(offset, output, mask, 1, base);

}


} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
