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

#ifndef UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_TESLA_FLOW_TCC
#define UNISIM_COMPONENT_CXX_PROCESSOR_TESLA_TESLA_FLOW_TCC

#include <unisim/component/cxx/processor/tesla/tesla_flow.hh>
#include <unisim/component/cxx/processor/tesla/cpu.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tesla {

template<class CONFIG>
TeslaFlow<CONFIG>::TeslaFlow(Warp<CONFIG> & warp) :
	warp(warp)
{
}

template<class CONFIG>
void TeslaFlow<CONFIG>::Reset(CPU<CONFIG> * cpu, std::bitset<CONFIG::WARP_SIZE> mask)
{
	this->cpu = cpu;
	current_mask = initial_mask = mask;
	stack = stack_t();
	//stack.push(StackToken(mask, ID_BOTTOM, 0));
}

template<class CONFIG>
void TeslaFlow<CONFIG>::Branch(address_t target_addr, std::bitset<CONFIG::WARP_SIZE> mask)
{
	assert((target_addr & 3) == 0);	// 64-bit-aligned targets
	typename CONFIG::address_t adjpc = warp.pc - CONFIG::CODE_START;

	// Check mask
	// Mask == none -> nop
	// Mask == all  -> jump
	// Mask == other -> argh
	
	typename CONFIG::address_t abs_target = target_addr + CONFIG::CODE_START;
	if(cpu->trace_branch) {
		cerr << " Branch mask = " << mask << endl;
	}
	
	assert((mask & ~(current_mask)).none());	// No zombies allowed

	if((mask).none())
	{
		// Uniformally not taken
		if(cpu->trace_branch) {
			cerr << "  Coherent, not taken." << endl;
		}
		// Nop, just increment PC as usual
	}
	else if(mask == GetCurrentMask())	// Same number of enabled threads as before
	{
		// Uniformally taken
		if(cpu->trace_branch) {
			cerr << "  Coherent, taken." << endl;
		}
		warp.npc = abs_target;
		// current_mask = mask already
	}
	else
	{
		// Start by *taking* the branch ??
	
		// Mixed
		// Push divergence token
		stack.push(StackToken(current_mask & ~mask, ID_DIVERGE, warp.npc));
		if(cpu->trace_branch) {
			cerr << hex;
			cerr << " Conditional non-coherent jump from "
				<< adjpc << " to " << target_addr << endl;
			cerr << dec;
		}
		if(cpu->trace_branch) {
			cerr << "  New mask = " << GetCurrentMask() << endl;
		}

		warp.npc = abs_target;
		current_mask = mask;
	}
}

template<class CONFIG>
void TeslaFlow<CONFIG>::Meet(address_t addr)
{
	// Addr relative address
	// SYNC instruction
	stack.push(StackToken(current_mask, ID_SYNC, addr + CONFIG::CODE_START));	// Does addr mean anything here??
}

template<class CONFIG>
bool TeslaFlow<CONFIG>::Join()
{
	if(stack.empty()) {
		// implicit SYNC token at the bottom of stack?
		current_mask = initial_mask;
		return true;
	}
	StackToken token = stack.top();
	stack.pop();
	
	current_mask = token.mask;
	if(token.id != ID_SYNC)
	{
		// Re-branch to address in token
		warp.npc = token.address;
		
		// Do NOT execute current instruction
		return false;
	}
	return true;
}

template<class CONFIG>
void TeslaFlow<CONFIG>::End()
{
}

template<class CONFIG>
void TeslaFlow<CONFIG>::Kill(std::bitset<CONFIG::WARP_SIZE> mask)
{
	// mask : threads to kill
	// GetCurrentMask() & ~mask : threads alive
}

template <class CONFIG>
void TeslaFlow<CONFIG>::CheckJoin()
{
}

template <class CONFIG>
std::bitset<CONFIG::WARP_SIZE> TeslaFlow<CONFIG>::GetCurrentMask() const
{
	return current_mask;
}

} // end of namespace tesla
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


#endif
