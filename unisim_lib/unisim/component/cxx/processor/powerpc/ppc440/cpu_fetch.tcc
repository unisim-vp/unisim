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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_PPC440_CPU_FETCH_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_PPC440_CPU_FETCH_TCC__

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace ppc440 {

template <class CONFIG>
void CPU<CONFIG>::FlushSubsequentInstructions()
{
	cur_insn_in_prefetch_buffer = num_insn_in_prefetch_buffer - 1;
}

template <class CONFIG>
void CPU<CONFIG>::FillPrefetchBuffer(uint32_t insn)
{
	prefetch_buffer[0] = insn;
	num_insn_in_prefetch_buffer = 1;
	cur_insn_in_prefetch_buffer = 0;
}

template <class CONFIG>
bool CPU<CONFIG>::NeedFillingPrefetchBuffer() const
{
	return cur_insn_in_prefetch_buffer == num_insn_in_prefetch_buffer;
}

template <class CONFIG>
void CPU<CONFIG>::EmuFetch(typename CONFIG::address_t addr, void *buffer, uint32_t size)
{
	// Address translation
	MMUAccess<CONFIG> mmu_access;

	mmu_access.addr = addr;
	mmu_access.pid = GetProcessID();
	mmu_access.as = GetDataAddressSpace();
	mmu_access.privilege_level = GetPrivilegeLevel();
	mmu_access.memory_access_type = CONFIG::MAT_READ;
	mmu_access.memory_type = CONFIG::MT_INSN;

	EmuTranslateAddress<false>(mmu_access);

	if(likely(!(mmu_access.storage_attr & CONFIG::SA_I) && IsInsnCacheEnabled())) // cache inhibited ?
	{
		// IL1 Access
		CacheAccess<class CONFIG::IL1_CONFIG> l1_access;
	
		l1_access.addr = mmu_access.virtual_addr;
		l1_access.storage_attr = mmu_access.storage_attr;
		LookupIL1<false>(l1_access);

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
			EmuFillIL1(l1_access, mmu_access);
		}
	
		memcpy(buffer, &(*l1_access.block)[l1_access.offset], size);

		if(unlikely(il1_power_estimator_import != 0)) il1_power_estimator_import->ReportReadAccess();
	}
	else
	{
		// DL1 disabled
		if(unlikely(!BusRead(mmu_access.physical_addr, buffer, size, mmu_access.storage_attr)))
		{
			throw InstructionAsynchronousMachineCheckException<CONFIG>();
		}
	}
	
	if(mmu_access.storage_attr & CONFIG::SA_E) // little-endian ?
	{
#if BYTE_ORDER == BIG_ENDIAN
		uint32_t *insn = (uint32_t *) buffer;
		do
		{
			BSwap(*insn);
		}
		while(++insn, size -= 4);
#endif
	}
	else
	{
#if BYTE_ORDER == LITTLE_ENDIAN
		uint32_t *insn = (uint32_t *) buffer;
		do
		{
			BSwap(*insn);
		}
		while(++insn, size -= 4);
#endif
	}
}

} // end of namespace ppc440
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif