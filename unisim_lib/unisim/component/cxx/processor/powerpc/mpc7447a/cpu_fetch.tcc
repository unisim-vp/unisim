/*
 *  Copyright (c) 2007,
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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_MPC7447A_CPU_FETCH_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_MPC7447A_CPU_FETCH_TCC__

#ifdef powerpc
#undef powerpc
#endif

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace mpc7447a {

template <class CONFIG>
void CPU<CONFIG>::FlushSubsequentInstructions()
{
	num_insn_in_prefetch_buffer = 0;
	cur_insn_in_prefetch_buffer = 0;
}

template <class CONFIG>
bool CPU<CONFIG>::EmuFetch(typename CONFIG::address_t addr, void *buffer, uint32_t size)
{
	typename CONFIG::WIMG wimg;
	typename CONFIG::physical_address_t physical_addr;

	if(GetMSR_IR())
	{
		// Address translation
		MMUAccess<CONFIG> mmu_access;
	
		mmu_access.addr = addr;
		mmu_access.privilege_level = GetPrivilegeLevel();
		mmu_access.memory_access_type = CONFIG::MAT_READ;
		mmu_access.memory_type = CONFIG::MT_INSN;
	
		bool status = EmuTranslateAddress<false>(mmu_access);
		if(unlikely(!status)) return false;
	
		wimg = mmu_access.wimg;
		physical_addr = mmu_access.physical_addr;
	}
	else
	{
		// W=0: Write through disabled
		// I=0: Cache not inhibited
		// M=1: Memory coherency enforced
		// G=1: Guarded memory access (speculative fetch forbidden)
		wimg = (typename CONFIG::WIMG)(CONFIG::WIMG_MEMORY_COHERENCY_ENFORCED | CONFIG::WIMG_GUARDED_MEMORY);
		physical_addr = addr;
	}

	// Is IL1 enabled ?
	if(likely(IsInsnCacheEnabled()))
	{
		// IL1 Access
		CacheAccess<class CONFIG::IL1_CONFIG> l1_access;
	
		l1_access.addr = physical_addr;
		LookupIL1(l1_access);

		if(unlikely(!l1_access.line))
		{
			// IL1 line miss
			if(unlikely(IsVerboseIL1()))
			{
				logger << DebugInfo << "IL1: line miss at 0x" << std::hex << l1_access.addr << std::dec << endl << EndDebugInfo;
			}

			ChooseLineToEvictIL1(l1_access);
			if(unlikely(IsVerboseIL1()))
			{
				logger << DebugInfo << "IL1 line miss: choosen way=" << l1_access.way << endl << EndDebugInfo;
			}

			EmuEvictIL1(l1_access);
		}

		if(unlikely(!l1_access.block))
		{
			// IL1 block miss
			if(unlikely(IsVerboseIL1()))
			{
				logger << DebugInfo << "IL1: block miss at 0x" << std::hex << l1_access.addr << std::dec << endl << EndDebugInfo;
			}
			if(unlikely(!EmuFillIL1(l1_access, wimg))) return false;
		}
	
		memcpy(buffer, &(*l1_access.block)[l1_access.offset], size);
		//UpdateReplacementPolicyIL1(l1_access);

		if(unlikely(il1_power_estimator_import != 0)) il1_power_estimator_import->ReportReadAccess();
	}
	else
	{
		// DL1 disabled
		if(unlikely(!BusRead(physical_addr, buffer, size, wimg)))
		{
			SetException(CONFIG::EXC_MACHINE_CHECK_TEA);
			return false;
		}
	}
	
	return true;
}

template <class CONFIG>
bool CPU<CONFIG>::EmuFetch(typename CONFIG::address_t addr, uint32_t& insn)
{
	if(unlikely(requires_memory_access_reporting)) 
	{
		if(unlikely(memory_access_reporting_import != 0))
		{
			memory_access_reporting_import->ReportMemoryAccess(unisim::util::debug::MAT_READ, unisim::util::debug::MT_INSN, addr, 4);
		}
	}

	if(CONFIG::PREFETCH_BUFFER_ENABLE)
	{
		if(unlikely(cur_insn_in_prefetch_buffer == num_insn_in_prefetch_buffer))
		{
			uint32_t size_to_block_boundary = IsInsnCacheEnabled() ? CONFIG::IL1_CONFIG::CACHE_BLOCK_SIZE - (addr & (CONFIG::IL1_CONFIG::CACHE_BLOCK_SIZE - 1)) : CONFIG::FSB_WIDTH - (addr & (CONFIG::FSB_WIDTH - 1));
			uint32_t size_to_prefetch = size_to_block_boundary > (4 * CONFIG::NUM_PREFETCH_BUFFER_ENTRIES) ? CONFIG::NUM_PREFETCH_BUFFER_ENTRIES * 4 : size_to_block_boundary;
			// refill the prefetch buffer with up to one cache line, not much
			if(unlikely(!EmuFetch(addr, prefetch_buffer, size_to_prefetch))) return false;
			num_insn_in_prefetch_buffer = size_to_prefetch / 4;
			cur_insn_in_prefetch_buffer = 0;
		}
		insn = prefetch_buffer[cur_insn_in_prefetch_buffer++];
#if BYTE_ORDER == LITTLE_ENDIAN
		BSwap(insn);
#endif
		return true;
	}
	
	if(unlikely(!EmuFetch(addr, &insn, sizeof(insn)))) return false;
#if BYTE_ORDER == LITTLE_ENDIAN
	BSwap(insn);
#endif
	return true;
}

} // end of namespace mpc7447a
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
