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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_PPC440_CPU_MMU_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_PPC440_CPU_MMU_TCC__

#include <unisim/kernel/debug/debug.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace ppc440 {

template <class CONFIG>
void CPU<CONFIG>::InvalidateITLB()
{
	if(IsVerboseITLB())
	{
		logger << DebugInfo << "Invalidating ITLB" << EndDebugInfo;
		logger << DebugInfo << unisim::kernel::debug::BackTrace() << EndDebugInfo;
	}
	
	uint32_t way;

	TLBSet<typename CONFIG::ITLB_CONFIG>& itlb_set = itlb[0];

	for(way = 0; way < TLBSet<typename CONFIG::ITLB_CONFIG>::ASSOCIATIVITY; way++)
	{
		TLBEntry<typename CONFIG::ITLB_CONFIG>& itlb_entry = itlb_set[way];

		itlb_entry.pte.Invalidate();
	}
}

template <class CONFIG>
void CPU<CONFIG>::ResetITLB()
{
	if(IsVerboseITLB())
	{
		logger << DebugInfo << "Resetting ITLB" << EndDebugInfo;
	}
	uint32_t way;

	TLBSet<typename CONFIG::ITLB_CONFIG>& itlb_set = itlb[0];

	itlb_set.status.victim_way = 0;
	for(way = 0; way < TLBSet<typename CONFIG::ITLB_CONFIG>::ASSOCIATIVITY; way++)
	{
		TLBEntry<typename CONFIG::ITLB_CONFIG>& itlb_entry = itlb_set[way];

		itlb_entry.pte.Reset();
		itlb_entry.status.way = way;
		if(CONFIG::FAST_ITLB_LOOKUP_ENABLE)
		{
			itlb_entry.status.prev = (way > 0) ? &itlb_set[way - 1] : 0;
			itlb_entry.status.next = (way < (TLBSet<typename CONFIG::ITLB_CONFIG>::ASSOCIATIVITY - 1)) ? &itlb_set[way + 1] : 0;
		}
	}
	if(CONFIG::FAST_ITLB_LOOKUP_ENABLE)
	{
		itlb_set.status.mru_entry = &itlb_set[0];
		itlb_set.status.lru_entry = &itlb_set[TLBSet<typename CONFIG::ITLB_CONFIG>::ASSOCIATIVITY - 1];
	}

	TLBEntry<typename CONFIG::ITLB_CONFIG>& auto_config_tlb_entry = itlb[0][0];
	auto_config_tlb_entry.pte.Set(
		/* EPN */ 0x3ffffc,
		/*  V  */ 1,
		/* TS  */ CONFIG::AS_SYSTEM,
		/* SIZE */ 1, // 4 KB page
		/* TID */ 0,
		/* RPN */ 0x3ffffc,
		/* ERPN */ GetRSTCFG_ERPN(),
		/* STORAGE ATTR */ (typename CONFIG::STORAGE_ATTR)(CONFIG::SA_I | CONFIG::SA_G | (GetRSTCFG_U0() ? CONFIG::SA_U0 : 0) | (GetRSTCFG_U1() ? CONFIG::SA_U1 : 0) | (GetRSTCFG_U2() ? CONFIG::SA_U2 : 0) | (GetRSTCFG_U3() ? CONFIG::SA_U3 : 0) | (GetRSTCFG_E() ? CONFIG::SA_E : 0)),
		/* ACCESS CTRL */ (typename CONFIG::ACCESS_CTRL)(CONFIG::AC_SX | CONFIG::AC_SR)
	);
}

template <class CONFIG>
void CPU<CONFIG>::InvalidateDTLB()
{
	if(IsVerboseDTLB())
	{
		logger << DebugInfo << "Invalidating DTLB" << EndDebugInfo;
	}
	uint32_t way;

	TLBSet<typename CONFIG::DTLB_CONFIG>& dtlb_set = dtlb[0];

	for(way = 0; way < TLBSet<typename CONFIG::DTLB_CONFIG>::ASSOCIATIVITY; way++)
	{
		TLBEntry<typename CONFIG::DTLB_CONFIG>& dtlb_entry = dtlb_set[way];

		dtlb_entry.pte.Invalidate();
	}
}

template <class CONFIG>
void CPU<CONFIG>::ResetDTLB()
{
	if(IsVerboseDTLB())
	{
		logger << DebugInfo << "Resetting DTLB" << EndDebugInfo;
	}
	uint32_t way;

	TLBSet<typename CONFIG::DTLB_CONFIG>& dtlb_set = dtlb[0];

	dtlb_set.status.victim_way = 0;
	for(way = 0; way < TLBSet<typename CONFIG::DTLB_CONFIG>::ASSOCIATIVITY; way++)
	{
		TLBEntry<typename CONFIG::DTLB_CONFIG>& dtlb_entry = dtlb_set[way];

		dtlb_entry.pte.Reset();
		dtlb_entry.status.way = way;
		if(CONFIG::FAST_DTLB_LOOKUP_ENABLE)
		{
			dtlb_entry.status.prev = (way > 0) ? &dtlb_set[way - 1] : 0;
			dtlb_entry.status.next = (way < (TLBSet<typename CONFIG::DTLB_CONFIG>::ASSOCIATIVITY - 1)) ? &dtlb_set[way + 1] : 0;
		}
	}
	if(CONFIG::FAST_DTLB_LOOKUP_ENABLE)
	{
		dtlb_set.status.mru_entry = &dtlb_set[0];
		dtlb_set.status.lru_entry = &dtlb_set[TLBSet<typename CONFIG::DTLB_CONFIG>::ASSOCIATIVITY - 1];
	}
	
	TLBEntry<typename CONFIG::DTLB_CONFIG>& auto_config_tlb_entry = dtlb[0][0];
	auto_config_tlb_entry.pte.Set(
		/* EPN */ 0x3ffffc,
		/*  V  */ 1,
		/* TS  */ CONFIG::AS_SYSTEM,
		/* SIZE */ 1, // 4 KB page
		/* TID */ 0,
		/* RPN */ 0x3ffffc,
		/* ERPN */ GetRSTCFG_ERPN(),
		/* STORAGE ATTR */ (typename CONFIG::STORAGE_ATTR)(CONFIG::SA_I | CONFIG::SA_G | (GetRSTCFG_U0() ? CONFIG::SA_U0 : 0) | (GetRSTCFG_U1() ? CONFIG::SA_U1 : 0) | (GetRSTCFG_U2() ? CONFIG::SA_U2 : 0) | (GetRSTCFG_U3() ? CONFIG::SA_U3 : 0) | (GetRSTCFG_E() ? CONFIG::SA_E : 0)),
		/* ACCESS CTRL */ (typename CONFIG::ACCESS_CTRL)(CONFIG::AC_SX | CONFIG::AC_SR)
	);
}

template <class CONFIG>
void CPU<CONFIG>::InvalidateUTLB()
{
	if(IsVerboseUTLB())
	{
		logger << DebugInfo << "Invalidating UTLB" << EndDebugInfo;
	}
	uint32_t way;

	TLBSet<typename CONFIG::UTLB_CONFIG>& utlb_set = utlb[0];

	for(way = 0; way < TLBSet<typename CONFIG::UTLB_CONFIG>::ASSOCIATIVITY; way++)
	{
		TLBEntry<typename CONFIG::UTLB_CONFIG>& utlb_entry = utlb_set[way];

		utlb_entry.pte.Invalidate();
	}
}

template <class CONFIG>
void CPU<CONFIG>::ResetUTLB()
{
	if(IsVerboseUTLB())
	{
		logger << DebugInfo << "Resetting UTLB" << EndDebugInfo;
	}
	uint32_t way;

	TLBSet<typename CONFIG::UTLB_CONFIG>& utlb_set = utlb[0];

	for(way = 0; way < TLBSet<typename CONFIG::UTLB_CONFIG>::ASSOCIATIVITY; way++)
	{
		TLBEntry<typename CONFIG::UTLB_CONFIG>& utlb_entry = utlb_set[way];

		utlb_entry.pte.Reset();
		utlb_entry.status.way = way;
		if(CONFIG::FAST_UTLB_LOOKUP_ENABLE)
		{
			utlb_entry.status.prev = (way > 0) ? &utlb_set[way - 1] : 0;
			utlb_entry.status.next = (way < (TLBSet<typename CONFIG::UTLB_CONFIG>::ASSOCIATIVITY - 1)) ? &utlb_set[way + 1] : 0;
		}
	}
	if(CONFIG::FAST_UTLB_LOOKUP_ENABLE)
	{
		utlb_set.status.mru_entry = &utlb_set[0];
		utlb_set.status.lru_entry = &utlb_set[TLBSet<typename CONFIG::UTLB_CONFIG>::ASSOCIATIVITY - 1];
	}
}

template <class CONFIG>
template <bool DEBUG>
void CPU<CONFIG>::LookupITLB(MMUAccess<CONFIG>& mmu_access)
{
	uint32_t tlb_index = 0;
	TLBSet<typename CONFIG::ITLB_CONFIG> *tlb_set;
	TLBEntry<typename CONFIG::ITLB_CONFIG> *tlb_entry;

	if(!DEBUG) num_itlb_accesses++;
	mmu_access.tlb_index = tlb_index;
	mmu_access.itlb_set = tlb_set = &itlb[tlb_index];
	
	if(unlikely(IsVerboseITLB()))
	{
		logger << DebugInfo << (DEBUG ? "(DEBUG) " : "") << "ITLB Lookup at 0x" << std::hex << mmu_access.virtual_addr << "(as=" << mmu_access.as << ", pid=0x" << (unsigned int) mmu_access.pid << ", ea=0x" << mmu_access.addr << ")" << std::dec << EndDebugInfo;
	}
	if(CONFIG::FAST_ITLB_LOOKUP_ENABLE)
	{
		for(tlb_entry = tlb_set->status.mru_entry; tlb_entry; tlb_entry = tlb_entry->status.next)
		{
			if(unlikely(IsVerboseITLB()))
			{
				logger << DebugInfo << (DEBUG ? "(DEBUG) " : "") << "Looking at way #" << tlb_entry->status.way << ": V=" << tlb_entry->pte.GetV() << ", TS=" << tlb_entry->pte.GetTS() << ", TID=0x" << std::hex << (unsigned int) tlb_entry->pte.GetTID() << std::dec << EndDebugInfo;
			}
			if(tlb_entry->pte.GetV() && (tlb_entry->pte.GetTS() == mmu_access.as) && (!tlb_entry->pte.GetTID() || (tlb_entry->pte.GetTID() == mmu_access.pid)))
			{
				typename CONFIG::address_t page_size = 1024 << (2 * tlb_entry->pte.GetSIZE()); // page size in bytes
				if(unlikely(!((1 << tlb_entry->pte.GetSIZE()) & CONFIG::PS_CAP)))
				{
					logger << DebugWarning << (DEBUG ? "(DEBUG) " : "") << "Unsupported page size " << page_size << EndDebugWarning;
					continue;
				}
				typename CONFIG::address_t addr_mask = ~(page_size - 1); // keep only upper bits for the tag comparison
				if(unlikely(IsVerboseITLB()))
				{
					logger << DebugInfo << (DEBUG ? "(DEBUG) " : "") << "page size=" << page_size << ", addr mask=0x" << std::hex << addr_mask << std::dec << ", EPN=0x" << std::hex << tlb_entry->pte.GetEPN() << std::dec << EndDebugInfo;
					logger << DebugInfo << (DEBUG ? "(DEBUG) " : "") << "Comparing 0x" << std::hex << ((tlb_entry->pte.GetEPN() << ((8 * sizeof(typename CONFIG::address_t)) - CONFIG::TLBE0_EPN_BITSIZE)) & addr_mask) << " and 0x" << (mmu_access.addr & addr_mask) << std::dec << EndDebugInfo;
				}
				if(((tlb_entry->pte.GetEPN() << ((8 * sizeof(typename CONFIG::address_t)) - CONFIG::TLBE0_EPN_BITSIZE)) & addr_mask) == (mmu_access.addr & addr_mask))
				{
					// ITLB Hit
					if(unlikely(IsVerboseITLB()))
					{
						logger << DebugInfo << (DEBUG ? "(DEBUG) " : "") << "ITLB hit: tlb_index=" << tlb_index << ", tlb_way=" << tlb_entry->status.way << EndDebugInfo;
					}
					mmu_access.tlb_way = tlb_entry->status.way;
					mmu_access.itlb_entry = tlb_entry;
					mmu_access.pte = &tlb_entry->pte;
					mmu_access.page_size = page_size;
					mmu_access.tlb_hit = true;

					if(unlikely(!DEBUG && (tlb_entry->status.prev != 0)))
					{
						if(tlb_entry->status.next)
						{
							tlb_entry->status.next->status.prev = tlb_entry->status.prev;
						}
						else
						{
							tlb_set->status.lru_entry = tlb_entry->status.prev;
						}
						tlb_entry->status.prev->status.next = tlb_entry->status.next;
						tlb_entry->status.prev = 0;
						tlb_entry->status.next = tlb_set->status.mru_entry;
						tlb_set->status.mru_entry->status.prev = tlb_entry;
						tlb_set->status.mru_entry = tlb_entry;
					}
					
					return;
				}
			}
		}
	}
	else
	{
		uint32_t tlb_way;

		for(tlb_way = 0; tlb_way < TLBSet<typename CONFIG::ITLB_CONFIG>::ASSOCIATIVITY; tlb_way++)
		{
			tlb_entry = &(*tlb_set)[tlb_way];
			
			if(tlb_entry->pte.GetV() && (tlb_entry->pte.GetTS() == mmu_access.as) && (!tlb_entry->pte.GetTID() || (tlb_entry->pte.GetTID() == mmu_access.pid)))
			{
				typename CONFIG::address_t page_size = 1024 << (2 * tlb_entry->pte.GetSIZE()); // page size in bytes
				if(unlikely(!((1 << tlb_entry->pte.GetSIZE()) & CONFIG::PS_CAP)))
				{
					logger << DebugWarning << (DEBUG ? "(DEBUG) " : "") << "Unsupported page size " << page_size << EndDebugWarning;
					continue;
				}
				typename CONFIG::address_t addr_mask = ~(page_size - 1); // keep only upper bits for the tag comparison

				if(((tlb_entry->pte.GetEPN() << ((8 * sizeof(typename CONFIG::address_t)) - CONFIG::TLBE0_EPN_BITSIZE)) & addr_mask) == (mmu_access.addr & addr_mask))
				{
					// ITLB Hit
					if(unlikely(IsVerboseITLB()))
					{
						logger << DebugInfo << (DEBUG ? "(DEBUG) " : "") << "ITLB hit: tlb_index=" << tlb_index << ", tlb_way=" << tlb_way << EndDebugInfo;
					}
					mmu_access.tlb_way = tlb_way;
					mmu_access.itlb_entry = tlb_entry;
					mmu_access.pte = &tlb_entry->pte;
					mmu_access.page_size = page_size;
					mmu_access.tlb_hit = true;
					
					return;
				}
			}
		}
	}

	// ITLB Miss
	mmu_access.tlb_hit = false;
	ChooseEntryToEvictITLB(mmu_access);
	if(unlikely(IsVerboseITLB()))
	{
		logger << DebugInfo << (DEBUG ? "(DEBUG) " : "") << "ITLB miss: choosen TLB way=" << mmu_access.tlb_way << endl << EndDebugInfo;
	}
	if(!DEBUG) num_itlb_misses++;
}

template <class CONFIG>
template <bool DEBUG>
void CPU<CONFIG>::LookupDTLB(MMUAccess<CONFIG>& mmu_access)
{
	uint32_t tlb_index = 0;
	TLBSet<typename CONFIG::DTLB_CONFIG> *tlb_set;
	TLBEntry<typename CONFIG::DTLB_CONFIG> *tlb_entry;

	if(!DEBUG) num_dtlb_accesses++;

	mmu_access.tlb_index = tlb_index;
	mmu_access.dtlb_set = tlb_set = &dtlb[tlb_index];
	
	if(unlikely(IsVerboseDTLB()))
	{
		logger << DebugInfo << (DEBUG ? "(DEBUG) " : "") << "DTLB Lookup at 0x" << std::hex << mmu_access.virtual_addr << "(as=" << mmu_access.as << ", pid=0x" << (unsigned int) mmu_access.pid << ", ea=0x" << mmu_access.addr << ")" << std::dec << EndDebugInfo;
	}
	if(CONFIG::FAST_DTLB_LOOKUP_ENABLE)
	{
		for(tlb_entry = tlb_set->status.mru_entry; tlb_entry; tlb_entry = tlb_entry->status.next)
		{
			if(unlikely(IsVerboseDTLB()))
			{
				logger << DebugInfo << (DEBUG ? "(DEBUG) " : "") << "Looking at way #" << tlb_entry->status.way << ": V=" << tlb_entry->pte.GetV() << ", TS=" << tlb_entry->pte.GetTS() << ", TID=0x" << std::hex << (unsigned int) tlb_entry->pte.GetTID() << std::dec << EndDebugInfo;
			}
			if(tlb_entry->pte.GetV() && (tlb_entry->pte.GetTS() == mmu_access.as) && (!tlb_entry->pte.GetTID() || (tlb_entry->pte.GetTID() == mmu_access.pid)))
			{
				typename CONFIG::address_t page_size = 1024 << (2 * tlb_entry->pte.GetSIZE()); // page size in bytes
				if(unlikely(!((1 << tlb_entry->pte.GetSIZE()) & CONFIG::PS_CAP)))
				{
					logger << DebugWarning << (DEBUG ? "(DEBUG) " : "") << "Unsupported page size " << page_size << EndDebugWarning;
					continue;
				}
				typename CONFIG::address_t addr_mask = ~(page_size - 1); // keep only upper bits for the tag comparison

				if(unlikely(IsVerboseDTLB()))
				{
					logger << DebugInfo << (DEBUG ? "(DEBUG) " : "") << "page size=" << page_size << ", addr mask=0x" << std::hex << addr_mask << std::dec << ", EPN=0x" << std::hex << tlb_entry->pte.GetEPN() << std::dec << EndDebugInfo;
					logger << DebugInfo << (DEBUG ? "(DEBUG) " : "") << "Comparing 0x" << std::hex << ((tlb_entry->pte.GetEPN() << ((8 * sizeof(typename CONFIG::address_t)) - CONFIG::TLBE0_EPN_BITSIZE)) & addr_mask) << " and 0x" << (mmu_access.addr & addr_mask) << std::dec << EndDebugInfo;
				}
				if(((tlb_entry->pte.GetEPN() << ((8 * sizeof(typename CONFIG::address_t)) - CONFIG::TLBE0_EPN_BITSIZE)) & addr_mask) == (mmu_access.addr & addr_mask))
				{
					// DTLB Hit
					if(unlikely(IsVerboseDTLB()))
					{
						logger << DebugInfo << (DEBUG ? "(DEBUG) " : "") << "DTLB hit: tlb_index=" << tlb_index << ", tlb_way=" << tlb_entry->status.way << EndDebugInfo;
					}
					mmu_access.tlb_way = tlb_entry->status.way;
					mmu_access.dtlb_entry = tlb_entry;
					mmu_access.pte = &tlb_entry->pte;
					mmu_access.page_size = page_size;
					mmu_access.tlb_hit = true;

					if(unlikely(!DEBUG && (tlb_entry->status.prev != 0)))
					{
						if(tlb_entry->status.next)
						{
							tlb_entry->status.next->status.prev = tlb_entry->status.prev;
						}
						else
						{
							tlb_set->status.lru_entry = tlb_entry->status.prev;
						}
						tlb_entry->status.prev->status.next = tlb_entry->status.next;
						tlb_entry->status.prev = 0;
						tlb_entry->status.next = tlb_set->status.mru_entry;
						tlb_set->status.mru_entry->status.prev = tlb_entry;
						tlb_set->status.mru_entry = tlb_entry;
					}
					
					return;
				}
			}
		}
	}
	else
	{
		uint32_t tlb_way;

		for(tlb_way = 0; tlb_way < TLBSet<typename CONFIG::DTLB_CONFIG>::ASSOCIATIVITY; tlb_way++)
		{
			tlb_entry = &(*tlb_set)[tlb_way];
			
			if(tlb_entry->pte.GetV() && (tlb_entry->pte.GetTS() == mmu_access.as) && (!tlb_entry->pte.GetTID() || (tlb_entry->pte.GetTID() == mmu_access.pid)))
			{
				typename CONFIG::address_t page_size = 1024 << (2 * tlb_entry->pte.GetSIZE()); // page size in bytes
				if(unlikely(!((1 << tlb_entry->pte.GetSIZE()) & CONFIG::PS_CAP)))
				{
					logger << DebugWarning << (DEBUG ? "(DEBUG) " : "") << "Unsupported page size " << page_size << EndDebugWarning;
					continue;
				}
				typename CONFIG::address_t addr_mask = ~(page_size - 1); // keep only upper bits for the tag comparison

				if(((tlb_entry->pte.GetEPN() << ((8 * sizeof(typename CONFIG::address_t)) - CONFIG::TLBE0_EPN_BITSIZE)) & addr_mask) == (mmu_access.addr & addr_mask))
				{
					// DTLB Hit
					if(unlikely(IsVerboseDTLB()))
					{
						logger << DebugInfo << (DEBUG ? "(DEBUG) " : "") << "DTLB hit: tlb_index=" << tlb_index << ", tlb_way=" << tlb_way << EndDebugInfo;
					}
					mmu_access.tlb_way = tlb_way;
					mmu_access.dtlb_entry = tlb_entry;
					mmu_access.pte = &tlb_entry->pte;
					mmu_access.page_size = page_size;
					mmu_access.tlb_hit = true;

					return;
				}
			}
		}
	}

	// DTLB Miss
	mmu_access.tlb_hit = false;
	ChooseEntryToEvictDTLB(mmu_access);
	if(unlikely(IsVerboseDTLB()))
	{
		logger << DebugInfo << (DEBUG ? "(DEBUG) " : "") << "DTLB miss: choosen TLB way=" << mmu_access.tlb_way << endl << EndDebugInfo;
	}
	if(!DEBUG) num_dtlb_misses++;
}

template <class CONFIG>
template <bool DEBUG>
void CPU<CONFIG>::LookupUTLB(MMUAccess<CONFIG>& mmu_access)
{
	uint32_t tlb_index = 0;
	TLBSet<typename CONFIG::UTLB_CONFIG> *tlb_set;
	TLBEntry<typename CONFIG::UTLB_CONFIG> *tlb_entry;

	if(!DEBUG) num_utlb_accesses++;

	mmu_access.utlb_index = tlb_index;
	mmu_access.utlb_set = tlb_set = &utlb[tlb_index];
	
	if(unlikely(IsVerboseUTLB()))
	{
		logger << DebugInfo << (DEBUG ? "(DEBUG) " : "") << "UTLB Lookup at 0x" << std::hex << mmu_access.virtual_addr << "(as=" << mmu_access.as << ", pid=0x" << (unsigned int) mmu_access.pid << ", ea=0x" << mmu_access.addr << ")" << std::dec << EndDebugInfo;
	}
	if(CONFIG::FAST_UTLB_LOOKUP_ENABLE)
	{
		for(tlb_entry = tlb_set->status.mru_entry; tlb_entry; tlb_entry = tlb_entry->status.next)
		{
			if(unlikely(IsVerboseUTLB()))
			{
				logger << DebugInfo << (DEBUG ? "(DEBUG) " : "") << "Looking at way #" << tlb_entry->status.way << ": V=" << tlb_entry->pte.GetV() << ", TS=" << tlb_entry->pte.GetTS() << ", TID=0x" << std::hex << (unsigned int) tlb_entry->pte.GetTID() << std::dec << EndDebugInfo;
			}
			if(tlb_entry->pte.GetV() && (tlb_entry->pte.GetTS() == mmu_access.as) && (!tlb_entry->pte.GetTID() || (tlb_entry->pte.GetTID() == mmu_access.pid)))
			{
				typename CONFIG::address_t page_size = 1024 << (2 * tlb_entry->pte.GetSIZE()); // page size in bytes
				if(unlikely(!((1 << tlb_entry->pte.GetSIZE()) & CONFIG::PS_CAP)))
				{
					logger << DebugWarning << (DEBUG ? "(DEBUG) " : "") << "Unsupported page size " << page_size << EndDebugWarning;
					continue;
				}
				typename CONFIG::address_t addr_mask = ~(page_size - 1); // keep only upper bits for the tag comparison

				if(unlikely(IsVerboseUTLB()))
				{
					logger << DebugInfo << (DEBUG ? "(DEBUG) " : "") << "page size=" << page_size << ", addr mask=0x" << std::hex << addr_mask << std::dec << ", EPN=0x" << std::hex << tlb_entry->pte.GetEPN() << std::dec << EndDebugInfo;
					logger << DebugInfo << (DEBUG ? "(DEBUG) " : "") << "Comparing 0x" << std::hex << ((tlb_entry->pte.GetEPN() << ((8 * sizeof(typename CONFIG::address_t)) - CONFIG::TLBE0_EPN_BITSIZE)) & addr_mask) << " and 0x" << (mmu_access.addr & addr_mask) << std::dec << EndDebugInfo;
				}
				if(((tlb_entry->pte.GetEPN() << ((8 * sizeof(typename CONFIG::address_t)) - CONFIG::TLBE0_EPN_BITSIZE)) & addr_mask) == (mmu_access.addr & addr_mask))
				{
					// UTLB Hit
					if(unlikely(IsVerboseUTLB()))
					{
						logger << DebugInfo << (DEBUG ? "(DEBUG) " : "") << "UTLB hit: tlb_index=" << tlb_index << ", tlb_way=" << tlb_entry->status.way << EndDebugInfo;
					}
					mmu_access.utlb_way = tlb_entry->status.way;
					mmu_access.utlb_entry = tlb_entry;
					mmu_access.pte = &tlb_entry->pte;
					mmu_access.page_size = page_size;
					mmu_access.utlb_hit = true;

					if(unlikely(!DEBUG && (tlb_entry->status.prev != 0)))
					{
						if(tlb_entry->status.next)
						{
							tlb_entry->status.next->status.prev = tlb_entry->status.prev;
						}
						else
						{
							tlb_set->status.lru_entry = tlb_entry->status.prev;
						}
						tlb_entry->status.prev->status.next = tlb_entry->status.next;
						tlb_entry->status.prev = 0;
						tlb_entry->status.next = tlb_set->status.mru_entry;
						tlb_set->status.mru_entry->status.prev = tlb_entry;
						tlb_set->status.mru_entry = tlb_entry;
					}
					
					return;
				}
			}
		}
	}
	else
	{
		uint32_t tlb_way;

		for(tlb_way = 0; tlb_way < TLBSet<typename CONFIG::UTLB_CONFIG>::ASSOCIATIVITY; tlb_way++)
		{
			tlb_entry = &(*tlb_set)[tlb_way];
			
			if(tlb_entry->pte.GetV() && (tlb_entry->pte.GetTS() == mmu_access.as) && (!tlb_entry->pte.GetTID() || (tlb_entry->pte.GetTID() == mmu_access.pid)))
			{
				typename CONFIG::address_t page_size = 1024 << (2 * tlb_entry->pte.GetSIZE()); // page size in bytes
				if(unlikely(!((1 << tlb_entry->pte.GetSIZE()) & CONFIG::PS_CAP)))
				{
					logger << DebugWarning << (DEBUG ? "(DEBUG) " : "") << "Unsupported page size " << page_size << EndDebugWarning;
					continue;
				}
				typename CONFIG::address_t addr_mask = ~(page_size - 1); // keep only upper bits for the tag comparison

				if(((tlb_entry->pte.GetEPN() << ((8 * sizeof(typename CONFIG::address_t)) - CONFIG::TLBE0_EPN_BITSIZE)) & addr_mask) == (mmu_access.addr & addr_mask))
				{
					// UTLB Hit
					if(unlikely(IsVerboseUTLB()))
					{
						logger << DebugInfo << (DEBUG ? "(DEBUG) " : "") << "UTLB hit: tlb_index=" << tlb_index << ", tlb_way=" << tlb_way << EndDebugInfo;
					}
					mmu_access.utlb_way = tlb_way;
					mmu_access.utlb_entry = tlb_entry;
					mmu_access.utlb_hit = true;

					return;
				}
			}
		}
	}

	// UTLB Miss
	mmu_access.utlb_hit = false;
	if(unlikely(IsVerboseUTLB()))
	{
		logger << DebugInfo << (DEBUG ? "(DEBUG) " : "") << "UTLB miss" << EndDebugInfo;
	}
	if(!DEBUG) num_utlb_misses++;
}

template <class CONFIG>
void CPU<CONFIG>::UpdateITLBReplacementPolicy(MMUAccess<CONFIG>& mmu_access)
{
	// Update victim way in a round-robin manner
	mmu_access.itlb_set->status.victim_way = (mmu_access.itlb_set->status.victim_way + 1) & (TLBSet<typename CONFIG::ITLB_CONFIG>::ASSOCIATIVITY - 1);
}

template <class CONFIG>
void CPU<CONFIG>::UpdateDTLBReplacementPolicy(MMUAccess<CONFIG>& mmu_access)
{
	// Update victim way in a round-robin manner
	mmu_access.dtlb_set->status.victim_way = (mmu_access.dtlb_set->status.victim_way + 1) & (TLBSet<typename CONFIG::DTLB_CONFIG>::ASSOCIATIVITY - 1);
}

template <class CONFIG>
void CPU<CONFIG>::ChooseEntryToEvictITLB(MMUAccess<CONFIG>& mmu_access)
{
	uint32_t way = mmu_access.itlb_set->status.victim_way;
	mmu_access.tlb_way = way;
	mmu_access.itlb_entry = &(*mmu_access.itlb_set)[way];
}

template <class CONFIG>
void CPU<CONFIG>::ChooseEntryToEvictDTLB(MMUAccess<CONFIG>& mmu_access)
{
	uint32_t way = mmu_access.dtlb_set->status.victim_way;
	mmu_access.tlb_way = way;
	mmu_access.dtlb_entry = &(*mmu_access.dtlb_set)[way];
}

template <class CONFIG>
template <bool DEBUG>
void CPU<CONFIG>::EmuTranslateAddress(MMUAccess<CONFIG>& mmu_access)
{
	if(CONFIG::HAS_MMU)
	{
		// Compute the 41-bit virtual address
		mmu_access.virtual_addr = ((typename CONFIG::virtual_address_t) mmu_access.as << CONFIG::VADDR_AS_OFFSET) | ((typename CONFIG::virtual_address_t) mmu_access.pid << CONFIG::VADDR_PID_OFFSET) | ((typename CONFIG::virtual_address_t) mmu_access.addr);
		
		if(mmu_access.memory_type == CONFIG::MT_INSN)
		{
			LookupITLB<DEBUG>(mmu_access);
			
			if(unlikely(!mmu_access.tlb_hit))
			{
				LookupUTLB<DEBUG>(mmu_access);
				if(unlikely(!mmu_access.utlb_hit))
				{
					throw InstructionTLBErrorException<CONFIG>(mmu_access.addr);
				}
				
				if(!DEBUG)
				{
					if(unlikely(IsVerboseITLB()))
					{
						logger << DebugInfo << "Refilling ITLB way #" << mmu_access.itlb_entry->status.way << EndDebugInfo;
					}
					mmu_access.itlb_entry->pte = mmu_access.utlb_entry->pte;
					mmu_access.pte = &mmu_access.itlb_entry->pte;
				}
			}
			
			if(!DEBUG)
			{
				typename CONFIG::ACCESS_CTRL access_ctrl = (mmu_access.privilege_level == CONFIG::PR_USER) ? CONFIG::AC_UX : CONFIG::AC_SX;
				if(unlikely(IsVerboseITLB()))
				{
					logger << DebugInfo << "Comparing access ctrl 0x" << std::hex << (unsigned int) access_ctrl << " and 0x" << (unsigned int) mmu_access.pte->GetAccessCtrl() << EndDebugInfo;
				}

				if(unlikely(!(mmu_access.pte->GetAccessCtrl() & access_ctrl)))
				{
					throw ISIExecuteAccessControlException<CONFIG>(mmu_access.addr);
				}
			}
			
			mmu_access.storage_attr = mmu_access.pte->GetStorageAttr();

			// Compute physical address
			mmu_access.physical_addr = ((typename CONFIG::physical_address_t) mmu_access.pte->GetERPN() << CONFIG::PADDR_ERPN_OFFSET)
			                         | ((typename CONFIG::physical_address_t) mmu_access.pte->GetRPN() << CONFIG::PADDR_RPN_OFFSET)
			                         | ((typename CONFIG::physical_address_t) mmu_access.addr & ((typename CONFIG::physical_address_t) mmu_access.page_size - 1));
			
			if(IsVerboseITLB())
			{
				logger << DebugInfo << (DEBUG ? "(DEBUG) " : "") << "physical addr=0x" << std::hex << mmu_access.physical_addr << std::dec << EndDebugInfo;
			}
		}
		else
		{
			LookupDTLB<DEBUG>(mmu_access);
			
			if(unlikely(!mmu_access.tlb_hit))
			{
				LookupUTLB<DEBUG>(mmu_access);
				if(unlikely(!mmu_access.utlb_hit))
				{
					throw DataTLBErrorException<CONFIG>(mmu_access.addr, mmu_access.memory_access_type);
				}
				
				if(!DEBUG)
				{
					if(unlikely(IsVerboseDTLB()))
					{
						logger << DebugInfo << "Refilling DTLB way #" << mmu_access.itlb_entry->status.way << EndDebugInfo;
					}
					mmu_access.dtlb_entry->pte = mmu_access.utlb_entry->pte;
					mmu_access.pte = &mmu_access.dtlb_entry->pte;
				}
			}

			if(!DEBUG)
			{
				typename CONFIG::ACCESS_CTRL access_ctrl = (mmu_access.privilege_level == CONFIG::PR_USER)
				                                         ? ((mmu_access.memory_access_type == CONFIG::MAT_WRITE) ? CONFIG::AC_UW : CONFIG::AC_UR)
				                                         : ((mmu_access.memory_access_type == CONFIG::MAT_WRITE) ? CONFIG::AC_SW : CONFIG::AC_SR);

				if(unlikely(IsVerboseDTLB()))
				{
					logger << DebugInfo << "Comparing access ctrl 0x" << std::hex << (unsigned int) access_ctrl << " and 0x" << (unsigned int) mmu_access.pte->GetAccessCtrl() << EndDebugInfo;
				}
				if(unlikely(!(mmu_access.pte->GetAccessCtrl() & access_ctrl)))
				{
					if(mmu_access.memory_access_type == CONFIG::MAT_WRITE)
					{
						throw DSIWriteAccessControlException<CONFIG>(mmu_access.addr);
					}
					else
					{
						throw DSIReadAccessControlException<CONFIG>(mmu_access.addr);
					}
				}
			}

			mmu_access.storage_attr = mmu_access.pte->GetStorageAttr();

			// Compute physical address
			mmu_access.physical_addr = ((typename CONFIG::physical_address_t) mmu_access.pte->GetERPN() << CONFIG::PADDR_ERPN_OFFSET)
			                         | ((typename CONFIG::physical_address_t) mmu_access.pte->GetRPN() << CONFIG::PADDR_RPN_OFFSET)
			                         | ((typename CONFIG::physical_address_t) mmu_access.addr & ((typename CONFIG::physical_address_t) mmu_access.page_size - 1));

			if(IsVerboseDTLB())
			{
				logger << DebugInfo << (DEBUG ? "(DEBUG) " : "") << "physical addr=0x" << std::hex << mmu_access.physical_addr << std::dec << EndDebugInfo;
			}
		}

		mmu_access.protection_boundary = mmu_access.physical_addr + mmu_access.page_size;
	}
	else
	{
		mmu_access.physical_addr = mmu_access.addr;
		mmu_access.virtual_addr = mmu_access.addr;
		mmu_access.storage_attr = (typename CONFIG::STORAGE_ATTR)(CONFIG::SA_DEFAULT);
		mmu_access.protection_boundary = 0xffffffffUL;
	}
}

template <class CONFIG>
void CPU<CONFIG>::DumpITLB(std::ostream& os)
{
	uint32_t tlb_index = 0;
	uint32_t tlb_way;
	TLBSet<typename CONFIG::ITLB_CONFIG> *tlb_set = &itlb[tlb_index];
	TLBEntry<typename CONFIG::ITLB_CONFIG> *tlb_entry;

	os << "ITLB:" << std::endl;
	for(tlb_way = 0; tlb_way < TLBSet<typename CONFIG::ITLB_CONFIG>::ASSOCIATIVITY; tlb_way++)
	{
		tlb_entry = &(*tlb_set)[tlb_way];
		
		os << " way #" << tlb_way << ":" << tlb_entry->pte << std::endl;
	}
}

template <class CONFIG>
void CPU<CONFIG>::DumpDTLB(std::ostream& os)
{
	uint32_t tlb_index = 0;
	uint32_t tlb_way;
	TLBSet<typename CONFIG::DTLB_CONFIG> *tlb_set = &dtlb[tlb_index];
	TLBEntry<typename CONFIG::DTLB_CONFIG> *tlb_entry;

	os << "DTLB:" << std::endl;
	for(tlb_way = 0; tlb_way < TLBSet<typename CONFIG::DTLB_CONFIG>::ASSOCIATIVITY; tlb_way++)
	{
		tlb_entry = &(*tlb_set)[tlb_way];
		
		os << " way #" << tlb_way << ":" << tlb_entry->pte << std::endl;
	}
}

template <class CONFIG>
void CPU<CONFIG>::DumpUTLB(std::ostream& os, int highlight_index, int hightligh_way)
{
	uint32_t tlb_index = 0;
	uint32_t tlb_way;
	TLBSet<typename CONFIG::UTLB_CONFIG> *tlb_set = &utlb[tlb_index];
	TLBEntry<typename CONFIG::UTLB_CONFIG> *tlb_entry;

	os << "UTLB:" << std::endl;
	for(tlb_way = 0; tlb_way < TLBSet<typename CONFIG::UTLB_CONFIG>::ASSOCIATIVITY; tlb_way++)
	{
		tlb_entry = &(*tlb_set)[tlb_way];
		
		if(tlb_entry->pte.GetV())
		{
			if((highlight_index >= 0) && (tlb_index == (uint32_t) highlight_index))
			{
				if((hightligh_way < 0) || ((hightligh_way >= 0) && (tlb_way == (uint32_t) hightligh_way)))
				{
					os << ">>>";
				}
			}
			os << " way #" << tlb_way << ":";
			
			typename CONFIG::address_t page_size = 1024 << (2 * tlb_entry->pte.GetSIZE()); // page size in bytes
			typename CONFIG::address_t low = tlb_entry->pte.GetEPN() << ((8 * sizeof(typename CONFIG::address_t)) - CONFIG::TLBE0_EPN_BITSIZE);
			typename CONFIG::address_t high = low + page_size - 1;

			typename CONFIG::physical_address_t plow = ((typename CONFIG::physical_address_t) tlb_entry->pte.GetERPN() << CONFIG::PADDR_ERPN_OFFSET)
										| ((typename CONFIG::physical_address_t) tlb_entry->pte.GetRPN() << CONFIG::PADDR_RPN_OFFSET);
			typename CONFIG::physical_address_t phigh = plow + page_size - 1;

			typename CONFIG::ACCESS_CTRL access_ctrl = tlb_entry->pte.GetAccessCtrl();

			os << "S=";
			os << ((access_ctrl & CONFIG::AC_SR) ? 'r' : '-');
			os << ((access_ctrl & CONFIG::AC_SW) ? 'w' : '-');
			os << ((access_ctrl & CONFIG::AC_SX) ? 'x' : '-');
			os << " U=";
			os << ((access_ctrl & CONFIG::AC_UR) ? 'r' : '-');
			os << ((access_ctrl & CONFIG::AC_UW) ? 'w' : '-');
			os << ((access_ctrl & CONFIG::AC_UX) ? 'x' : '-');
			os << " 0x" << std::hex << low << std::dec << "-0x" << std::hex << high << std::dec;
			os << " -> 0x" << std::hex << plow << std::dec << "-0x" << std::hex << phigh << std::dec << ":";

			os << tlb_entry->pte;

			os << std::endl;
		}
	}
}

/* TLB management */
template <class CONFIG>
void CPU<CONFIG>::Tlbre(unsigned int rd, uint32_t way, uint8_t ws)
{
	if(GetMSR_PR())
	{
		throw PrivilegeViolationException<CONFIG>();
	}
	
	if(ws > 2)
	{
		logger << DebugWarning << "At 0x" << std::hex << GetCIA() << std::dec << ", operand \'ws\' of instruction tlbre is > 2" << EndDebugWarning;
		return;
	}
	
	way = way & (TLBSet<typename CONFIG::UTLB_CONFIG>::ASSOCIATIVITY - 1);
	TLBEntry<typename CONFIG::UTLB_CONFIG>& tlb_entry = utlb[0][way];

	switch(ws)
	{
		case 0:
			SetGPR(rd, tlb_entry.pte.Get(ws) & (GetCCR0_CRPE() ? CONFIG::TLBE0_MASK : CONFIG::TLBE0_TAG_MASK));
			SetMMUCR_STID(tlb_entry.pte.GetTID());
			break;
		case 1:
			SetGPR(rd, tlb_entry.pte.Get(ws) & (GetCCR0_CRPE() ? CONFIG::TLBE1_MASK : CONFIG::TLBE1_DATA_MASK));
			break;
		case 2:
			SetGPR(rd, tlb_entry.pte.Get(ws) & (GetCCR0_CRPE() ? CONFIG::TLBE2_MASK : CONFIG::TLBE2_DATA_MASK));
			break;
	}
}

template <class CONFIG>
void CPU<CONFIG>::Tlbsx(unsigned int rd, typename CONFIG::address_t addr, unsigned int rc)
{
	MMUAccess<CONFIG> mmu_access;
	
	mmu_access.addr = addr;
	mmu_access.pid = (typename CONFIG::process_id_t) GetMMUCR_STID();
	mmu_access.as = (typename CONFIG::address_space_t) GetMMUCR_STS();
	// Compute the 41-bit virtual address
	mmu_access.virtual_addr = ((typename CONFIG::virtual_address_t) mmu_access.as << CONFIG::VADDR_AS_OFFSET) | ((typename CONFIG::virtual_address_t) mmu_access.pid << CONFIG::VADDR_PID_OFFSET) | ((typename CONFIG::virtual_address_t) mmu_access.addr);

	LookupUTLB<false>(mmu_access);
	if(likely(mmu_access.utlb_hit))
	{
		SetGPR(rd, mmu_access.utlb_way);
		if(rc)
		{
			// Generate CR0: CR0[SO]=XER[SO] CR0[LT]=0 CR0[GT]=0 CR0[EQ]=1
			SetCR((GetCR() & ~CONFIG::CR0_MASK) |
			      ((GetXER() & CONFIG::XER_SO_MASK) ? CONFIG::CR0_SO_MASK : 0) |
			      (CONFIG::CR0_EQ_MASK));
		}
	}
	else
	{
		// (rd) is undefined
		if(rc)
		{
			// Generate CR0: CR0[SO]=XER[SO] CR0[LT]=0 CR0[GT]=0 CR0[EQ]=0
			SetCR((GetCR() & ~CONFIG::CR0_MASK) |
				 ((GetXER() & CONFIG::XER_SO_MASK) ? CONFIG::CR0_SO_MASK : 0));
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::Tlbwe(uint32_t s, uint32_t way, uint8_t ws)
{
	if(IsVerboseTlbwe())
	{
		logger << DebugInfo << "At 0x" << std::hex << GetCIA() << ", Tlbwe s=0x" << s << ", way=0x" << way << ", ws=0x" << (unsigned int) ws << std::dec << EndDebugInfo;
	}

	if(GetMSR_PR())
	{
		throw PrivilegeViolationException<CONFIG>();
	}
	
	if(ws > 2)
	{
		logger << DebugWarning << "At 0x" << std::hex << GetCIA() << std::dec << ", operand \'ws\' of instruction tlbwe is > 2" << EndDebugWarning;
		return;
	}

	way = way & (TLBSet<typename CONFIG::UTLB_CONFIG>::ASSOCIATIVITY - 1);
	TLBEntry<typename CONFIG::UTLB_CONFIG>& tlb_entry = utlb[0][way];

	switch(ws)
	{
		case 0:
			tlb_entry.pte.Set(ws, s & CONFIG::TLBE0_TAG_MASK, (typename CONFIG::process_id_t) GetMMUCR_STID());
			break;
		case 1:
			tlb_entry.pte.Set(ws, s & CONFIG::TLBE1_DATA_MASK);
			break;
		case 2:
			tlb_entry.pte.Set(ws, s & CONFIG::TLBE2_DATA_MASK);
			break;
	}
	
	if(IsVerboseTlbwe())
	{
		std::stringstream sstr;
		DumpUTLB(sstr, 0, way);
		logger << DebugInfo << "After tlbwe, UTLB contains:" << std::endl;
		logger << sstr.str();
		logger << EndDebugInfo;
	}
}

} // end of namespace ppc440
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
