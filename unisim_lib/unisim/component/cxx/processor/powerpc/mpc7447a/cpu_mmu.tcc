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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_MPC7447A_CPU_MMU_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_MPC7447A_CPU_MMU_TCC__

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
void CPU<CONFIG>::ReconfigureFastBATLookup()
{
	uint32_t i;
	// Update BAT lookup order
	uint32_t num_bats = (GetHID0_HIGH_BAT_EN()) ? CONFIG::NUM_BATS : 4;
	for(i = 0; i < num_bats; i++)
	{
		if(CONFIG::FAST_DBAT_LOOKUP_ENABLE)
		{
			dbat[i].prev = (i > 0) ? &dbat[i - 1] : 0;
			dbat[i].next = (i < (num_bats - 1)) ? &dbat[i + 1] : 0;
		}
		if(CONFIG::FAST_IBAT_LOOKUP_ENABLE)
		{
			ibat[i].prev = (i > 0) ? &ibat[i - 1] : 0;
			ibat[i].next = (i < (num_bats - 1)) ? &ibat[i + 1] : 0;
		}
	}
	if(CONFIG::FAST_DBAT_LOOKUP_ENABLE)
	{
		mru_dbat = &dbat[0];
		last_dbat_miss_bepi = 0;
	}
	if(CONFIG::FAST_IBAT_LOOKUP_ENABLE)
	{
		mru_ibat = &ibat[0];
		last_ibat_miss_bepi = 0;
	}
}

template <class CONFIG>
template <bool DEBUG>
void CPU<CONFIG>::LookupBAT(MMUAccess<CONFIG>& mmu_access)
{
	typename CONFIG::address_t addr = mmu_access.addr;
	typename CONFIG::PrivilegeLevel privilege_level = mmu_access.privilege_level;
	typename CONFIG::MemoryAccessType memory_access_type = mmu_access.memory_access_type;
	typename CONFIG::MemoryType memory_type = mmu_access.memory_type;
	uint32_t bat_num;						// BAT register number
	uint32_t bepi;							// BEPI bit field
/*	uint32_t sr_num;						// Segment register number
	uint32_t virtual_segment_id;			// Virtual segment id
	uint32_t sr_ks;							// Supervisor key of a segment register
	uint32_t sr_kp;							// User key of a segment register
	uint32_t sr_noexecute;					// No-execute bit of a segment register
	virtual_address_t virtual_addr;			// Virtual address
	virtual_address_t base_virtual_addr;	// Base virtual address
	uint32_t tlb_index;						// An index in DTLB/ITLB
	uint32_t h;
	uint32_t page_index;					// page index
	uint32_t api;							// Abreviated page index
	uint32_t hash;							// hash key
	uint32_t pteg_select;
	physical_address_t pte_addr;			// a page table entry address
	uint32_t pte_num;
	bool key;*/
	typename CONFIG::WIMG wimg;
	
	//------------------------------
	//-  Block Address Translation -
	//------------------------------

	// Compute BEPI from the effective address
	mmu_access.bepi = bepi = (addr >> 17) & 0x7fff;

	if(memory_type == CONFIG::MT_INSN)
	{
		num_ibat_accesses++;
		// Memory access is an instruction fetch
		if(CONFIG::FAST_IBAT_LOOKUP_ENABLE)
		{
			if(likely(!(last_ibat_miss_bepi & 0x80000000) || (last_ibat_miss_privilege_level != privilege_level) || ((last_ibat_miss_bepi & 0x7fffffff) != bepi)))
			{
				BAT *bat;
				
				// Scan each IBAT register to find a translation
				for(bat = mru_ibat; bat; bat = bat->next)
				{
					uint32_t batu = bat->u;
					uint32_t batl = bat->l;
					// Check if IBAT register match
					if((bepi & 0x7800) == (GetBATU_BEPI(batu) & 0x7800) // compare the 4 most significative bits of
						// the 15 most significative bits in EA and 15-bit IBAT BEPI field
								&& (bepi & ~GetBATU_BL(batu) & 0x7ff) == (GetBATU_BEPI(batu) & 0x7ff)) // compare the 11 least significative
						// bits of the 15 most significative bits in EA and IBAT BEPI field according to the IBAT block size mask (BL)
					{
						//cerr << "privilege level = " << ((privilege_level == PR_SUPERVISOR) ? "supervisor" : "user") << endl;
						//cerr << "IBATU_VP = " << GetBATU_VP(batu) << endl;
						//cerr << "IBATU_VS = " << GetBATU_BS(batu) << endl;
						if((GetBATU_VP(batu) && (privilege_level == CONFIG::PR_USER)) || (GetBATU_VS(batu) && (privilege_level == CONFIG::PR_SUPERVISOR)))
						{
							//cerr << "IBAT match for 0x" << hex << addr << endl;
							// A BAT match occurs
							mmu_access.protection_boundary = ((GetBATL_BRPN(batl) & 0x7ffUL) + GetBATU_BL(batu) + 1) << 17;
							mmu_access.wimg = wimg = (typename CONFIG::WIMG)(GetBATL_WIMG(batl) & 0x6); // W and G are not defined in IBATs;

							// Check if access is guarded
							if(unlikely(wimg & CONFIG::WIMG_GUARDED_MEMORY))
							{
								if(!DEBUG) throw ISIGuardedMemoryException<CONFIG>(addr);
							}

							// Check access rights
							uint32_t pp = GetBATL_PP(batl);
							if(unlikely(!pp)) // PP=00: No access
							{
								// Raise a protection violation
								if(!DEBUG) throw ISIProtectionViolationException<CONFIG>(addr);
							}
							// Access is allowed
						
							// Compute the physical address
							mmu_access.physical_addr = ((((bepi & 0x7ff) & GetBATU_BL(batu)) | (GetBATL_BRPN(batl) & 0x7ff) | (GetBATL_BRPN(batl) & 0x7800)) << 17) | (addr & 0x1ffff);
							mmu_access.bat_hit = true;
		//					cerr << "IBAT hit at 0x" << std::hex << addr << std::hex << endl;
							if(unlikely(bat->prev != 0))
							{
								if(bat->next)
								{
									bat->next->prev = bat->prev;
								}
								bat->prev->next = bat->next;
								bat->prev = 0;
								bat->next = mru_ibat;
								mru_ibat->prev = bat;
								mru_ibat = bat;
							}
							return;
						}
					}
				}
				last_ibat_miss_bepi = bepi | 0x80000000;
				last_ibat_miss_privilege_level = privilege_level;
			}
		}
		else
		{
			uint32_t num_ibats = (GetHID0_HIGH_BAT_EN()) ? CONFIG::NUM_BATS : 4;

			// Scan each IBAT register to find a translation
			for(bat_num = 0; bat_num < num_ibats; bat_num++)
			{
				// Check if IBAT register match
				if((bepi & 0x7800) == (GetIBATU_BEPI(bat_num) & 0x7800) // compare the 4 most significative bits of
					// the 15 most significative bits in EA and 15-bit IBAT BEPI field
							&& (bepi & ~GetIBATU_BL(bat_num) & 0x7ff) == (GetIBATU_BEPI(bat_num) & 0x7ff)) // compare the 11 least significative
					// bits of the 15 most significative bits in EA and IBAT BEPI field according to the IBAT block size mask (BL)
				{
					//cerr << "privilege level = " << ((privilege_level == PR_SUPERVISOR) ? "supervisor" : "user") << endl;
					//cerr << "IBATU_VP = " << GetIBATU_VP(bat_num) << endl;
					//cerr << "IBATU_VS = " << GetIBATU_VS(bat_num) << endl;
					if((GetIBATU_VP(bat_num) && (privilege_level == CONFIG::PR_USER)) || (GetIBATU_VS(bat_num) && (privilege_level == CONFIG::PR_SUPERVISOR)))
					{
						//cerr << "IBAT match for 0x" << hex << addr << endl;
						// A BAT match occurs
						mmu_access.protection_boundary = ((GetIBATU_BEPI(bat_num) & 0x7ffUL) + GetIBATU_BL(bat_num) + 1) << 17;
						mmu_access.wimg = wimg = (typename CONFIG::WIMG)(GetIBATL_WIMG(bat_num) & 0x6); // W and G are not defined in IBATs;

						// Check if access is guarded
						if(unlikely(wimg & CONFIG::WIMG_GUARDED_MEMORY))
						{
							if(!DEBUG) throw ISIGuardedMemoryException<CONFIG>(addr);
						}

						// Check access rights
						uint32_t pp = GetIBATL_PP(bat_num);
						if(unlikely(!pp)) // PP=00: No access
						{
							// Raise a protection violation
							if(!DEBUG) throw ISIProtectionViolationException<CONFIG>(addr);
						}
						// Access is allowed
					
						// Compute the physical address
						mmu_access.physical_addr = ((((bepi & 0x7ff) & GetIBATU_BL(bat_num)) | (GetIBATL_BRPN(bat_num) & 0x7ff) | (GetIBATL_BRPN(bat_num) & 0x7800)) << 17) | (addr & 0x1ffff);
						mmu_access.bat_hit = true;
	//					cerr << "IBAT hit at 0x" << std::hex << addr << std::hex << endl;
						return;
					}
				}
			}
			//cerr << "IBAT miss at 0x" << std::hex << addr << std::hex << endl;
		}
		num_ibat_misses++;
	}
	else
	{
		num_dbat_accesses++;
		// Memory access is a data memory access
		if(CONFIG::FAST_DBAT_LOOKUP_ENABLE)
		{

			if(likely(!(last_dbat_miss_bepi & 0x80000000) || (last_dbat_miss_privilege_level != privilege_level) || ((last_dbat_miss_bepi & 0x7fffffff) != bepi)))
			{
				BAT *bat;
				
				// Scan each DBAT register to find a translation
				for(bat = mru_dbat; bat; bat = bat->next)
				{
					uint32_t batu = bat->u;
					uint32_t batl = bat->l;
					
					// Check if DBAT register match
					if((bepi & 0x7800) == (GetBATU_BEPI(batu) & 0x7800) // compare the 4 most significative bits of
						// the 15 most significative bits in EA and 15-bit DBAT BEPI field
								&& (bepi & ~GetBATU_BL(batu) & 0x7ff) == (GetBATU_BEPI(batu) & 0x7ff)) // compare the 11 least significative
						// bits of the 15 most significative bits in EA and DBAT BEPI field according to the DBAT block size mask (BL)
					{
						if((GetBATU_VP(batu) && (privilege_level == CONFIG::PR_USER)) || (GetBATU_VS(batu) && (privilege_level == CONFIG::PR_SUPERVISOR)))
						{
							// A DBAT match occurs
							mmu_access.protection_boundary = ((GetBATU_BEPI(batu) & 0x7ffUL) + GetBATU_BL(batu)) << 17;
							mmu_access.wimg = wimg = (typename CONFIG::WIMG) GetBATL_WIMG(batl);
			
							// Check access rights
							uint32_t pp = GetBATL_PP(batl);
							if(unlikely(!pp || // PP=00: No access
										((pp & 1) && (memory_access_type == CONFIG::MAT_WRITE)))) // PP=x1: Read-only
							{
								if(!DEBUG)
								{
									throw DSIProtectionViolationException<CONFIG>(addr, memory_access_type);
								}
							}
							// Access is allowed
							
							// Compute the physical address
							mmu_access.physical_addr = ((((bepi & 0x7ff) & GetBATU_BL(batu)) | (GetBATL_BRPN(batl) & 0x7ff) | (GetBATL_BRPN(batl) & 0x7800)) << 17) | (addr & 0x1ffff);
							mmu_access.bat_hit = true;
		//					cerr << "DBAT hit at 0x" << std::hex << addr << std::hex << endl;
							if(unlikely(bat->prev != 0))
							{
								if(bat->next)
								{
									bat->next->prev = bat->prev;
								}
								bat->prev->next = bat->next;
								bat->prev = 0;
								bat->next = mru_dbat;
								mru_dbat->prev = bat;
								mru_dbat = bat;
							}
							return;
						}
					}
				}
				last_dbat_miss_bepi = bepi | 0x80000000;
				last_dbat_miss_privilege_level = privilege_level;
			}
		}
		else
		{
			uint32_t num_dbats = (GetHID0_HIGH_BAT_EN()) ? CONFIG::NUM_BATS : 4;
			// Scan each DBAT register to find a translation
			for(bat_num = 0; bat_num < num_dbats; bat_num++)
			{
				// Check if DBAT register match
				if((bepi & 0x7800) == (GetDBATU_BEPI(bat_num) & 0x7800) // compare the 4 most significative bits of
					// the 15 most significative bits in EA and 15-bit DBAT BEPI field
							&& (bepi & ~GetDBATU_BL(bat_num) & 0x7ff) == (GetDBATU_BEPI(bat_num) & 0x7ff)) // compare the 11 least significative
					// bits of the 15 most significative bits in EA and DBAT BEPI field according to the DBAT block size mask (BL)
				{
					if((GetDBATU_VP(bat_num) && (privilege_level == CONFIG::PR_USER)) || (GetDBATU_VS(bat_num) && (privilege_level == CONFIG::PR_SUPERVISOR)))
					{
						// A DBAT match occurs
						mmu_access.protection_boundary = ((GetDBATU_BEPI(bat_num) & 0x7ffUL) + GetDBATU_BL(bat_num)) << 17;
						mmu_access.wimg = wimg = (typename CONFIG::WIMG) GetDBATL_WIMG(bat_num);
		
						// Check access rights
						uint32_t pp = GetDBATL_PP(bat_num);
						if(unlikely(!pp || // PP=00: No access
									((pp & 1) && (memory_access_type == CONFIG::MAT_WRITE)))) // PP=x1: Read-only
						{
							if(!DEBUG)
							{
								throw DSIProtectionViolationException<CONFIG>(addr, memory_access_type);
							}
						}
						// Access is allowed
						
						// Compute the physical address
						mmu_access.physical_addr = ((((bepi & 0x7ff) & GetDBATU_BL(bat_num)) | (GetDBATL_BRPN(bat_num) & 0x7ff) | (GetDBATL_BRPN(bat_num) & 0x7800)) << 17) | (addr & 0x1ffff);
						mmu_access.bat_hit = true;
	//					cerr << "DBAT hit at 0x" << std::hex << addr << std::hex << endl;
						return;
					}
				}
			}
			//cerr << "DBAT miss at 0x" << std::hex << addr << std::hex << endl;
		}
		num_dbat_misses++;
	}
		
	// Miss in BAT registers
	mmu_access.bat_hit = false;
}



template <class CONFIG>
void CPU<CONFIG>::InvalidateITLBSet(uint32_t index)
{
	uint32_t way;

	TLBSet<typename CONFIG::ITLB_CONFIG>& itlb_set = itlb[index];

	itlb_set.status.plru_bits = 0;
	
	for(way = 0; way < TLBSet<typename CONFIG::ITLB_CONFIG>::ASSOCIATIVITY; way++)
	{
		TLBEntry<typename CONFIG::ITLB_CONFIG>& itlb_entry = itlb_set[way];

		itlb_entry.status.valid = false;
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
}

template <class CONFIG>
void CPU<CONFIG>::InvalidateITLB()
{
	uint32_t index;
	
	for(index = 0; index < TLB<typename CONFIG::ITLB_CONFIG>::NUM_SETS; index++)
	{
		InvalidateITLBSet(index);
	}
}

template <class CONFIG>
void CPU<CONFIG>::InvalidateDTLBSet(uint32_t index)
{
	uint32_t way;

	TLBSet<typename CONFIG::DTLB_CONFIG>& dtlb_set = dtlb[index];

	dtlb_set.status.plru_bits = 0;
	
	for(way = 0; way < TLBSet<typename CONFIG::DTLB_CONFIG>::ASSOCIATIVITY; way++)
	{
		TLBEntry<typename CONFIG::DTLB_CONFIG>& dtlb_entry = dtlb_set[way];

		dtlb_entry.status.valid = false;
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
}

template <class CONFIG>
void CPU<CONFIG>::InvalidateDTLB()
{
	uint32_t index;
	
	for(index = 0; index < TLB<typename CONFIG::DTLB_CONFIG>::NUM_SETS; index++)
	{
		InvalidateDTLBSet(index);
	}
}

template <class CONFIG>
void CPU<CONFIG>::LookupITLB(MMUAccess<CONFIG>& mmu_access)
{
	typename TLB<typename CONFIG::ITLB_CONFIG>::VIRTUAL_ADDRESS virtual_addr = mmu_access.virtual_addr;
	typename TLB<typename CONFIG::ITLB_CONFIG>::VIRTUAL_ADDRESS base_virtual_addr;
	uint32_t tlb_index;
	TLBSet<typename CONFIG::ITLB_CONFIG> *tlb_set;
	TLBEntry<typename CONFIG::ITLB_CONFIG> *tlb_entry;
	uint32_t tlb_way;

	num_itlb_accesses++;
	itlb.DecodeAddress(virtual_addr, base_virtual_addr, tlb_index);
	mmu_access.base_virtual_addr = base_virtual_addr;
	mmu_access.tlb_index = tlb_index;
	mmu_access.itlb_set = tlb_set = &itlb[tlb_index];

	if(CONFIG::FAST_ITLB_LOOKUP_ENABLE)
	{
		// Fast ITLB lookup
		for(tlb_entry = tlb_set->status.mru_entry; tlb_entry; tlb_entry = tlb_entry->status.next)
		{
			if(tlb_entry->status.valid && tlb_entry->GetBaseVirtualAddr() == base_virtual_addr)
			{
				// ITLB Hit
				// return typename CONFIG::WIMG bits
				mmu_access.tlb_way = tlb_entry->status.way;
				mmu_access.itlb_entry = tlb_entry;
				mmu_access.tlb_hit = true;
				
				if(unlikely(tlb_entry->status.prev != 0))
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
	else
	{
		// Slow ITLB lookup
		for(tlb_way = 0; tlb_way < TLBSet<typename CONFIG::ITLB_CONFIG>::ASSOCIATIVITY; tlb_way++)
		{
			tlb_entry = &(*tlb_set)[tlb_way];
			if(tlb_entry->status.valid && tlb_entry->GetBaseVirtualAddr() == base_virtual_addr)
			{
				// ITLB Hit
				// return typename CONFIG::WIMG bits
				mmu_access.tlb_way = tlb_way;
				mmu_access.itlb_entry = tlb_entry;
				mmu_access.tlb_hit = true;
				return;
			}
		}
	}

	// ITLB Miss
	mmu_access.tlb_hit = false;
	ChooseEntryToEvictITLB(mmu_access);
	if(unlikely(IsVerboseITLB()))
	{
		logger << DebugInfo << "ITLB miss: choosen TLB way=" << mmu_access.tlb_way << endl << EndDebugInfo;
	}
	num_itlb_misses++;
}

template <class CONFIG>
void CPU<CONFIG>::LookupDTLB(MMUAccess<CONFIG>& mmu_access)
{
	typename TLB<typename CONFIG::DTLB_CONFIG>::VIRTUAL_ADDRESS virtual_addr = mmu_access.virtual_addr;
	typename TLB<typename CONFIG::DTLB_CONFIG>::VIRTUAL_ADDRESS base_virtual_addr;
	uint32_t tlb_index;
	TLBSet<typename CONFIG::DTLB_CONFIG> *tlb_set;
	TLBEntry<typename CONFIG::DTLB_CONFIG> *tlb_entry;
	uint32_t tlb_way;

	num_dtlb_accesses++;
	dtlb.DecodeAddress(virtual_addr, base_virtual_addr, tlb_index);
	if(unlikely(IsVerboseDTLB()))
	{
		logger << DebugInfo << "DTLB Lookup at 0x";
		logger << std::hex << virtual_addr << std::dec << " : ";
		logger << "base_virtual_addr=0x" << std::hex << base_virtual_addr << std::dec << ",";
		logger << "tlb_index=0x" << std::hex << tlb_index << std::dec << ",";
		logger << endl << EndDebugInfo;
	}
	mmu_access.base_virtual_addr = base_virtual_addr;
	mmu_access.tlb_index = tlb_index;
	mmu_access.dtlb_set = tlb_set = &dtlb[tlb_index];

	if(CONFIG::FAST_DTLB_LOOKUP_ENABLE)
	{
		// Fast DTLB lookup
		for(tlb_entry = tlb_set->status.mru_entry; tlb_entry; tlb_entry = tlb_entry->status.next)
		{
			if(tlb_entry->status.valid && tlb_entry->GetBaseVirtualAddr() == base_virtual_addr)
			{
				// DTLB Hit
				if(unlikely(IsVerboseDTLB()))
				{
					logger << DebugInfo << "DTLB hit: tlb_way=" << tlb_way << ", base_physical_addr=0x" << std::hex << tlb_entry->pte.base_physical_addr << std::dec << endl << EndDebugInfo;
				}
				mmu_access.tlb_way = tlb_entry->status.way;
				mmu_access.dtlb_entry = tlb_entry;
				mmu_access.tlb_hit = true;

				if(unlikely(tlb_entry->status.prev != 0))
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
	else
	{
		for(tlb_way = 0; tlb_way < TLBSet<typename CONFIG::DTLB_CONFIG>::ASSOCIATIVITY; tlb_way++)
		{
			tlb_entry = &(*tlb_set)[tlb_way];
			if(tlb_entry->status.valid && tlb_entry->GetBaseVirtualAddr() == base_virtual_addr)
			{
				// DTLB Hit
				if(unlikely(IsVerboseDTLB()))
				{
					logger << DebugInfo << "DTLB hit: tlb_way=" << tlb_way << ", base_physical_addr=0x" << std::hex << tlb_entry->pte.base_physical_addr << std::dec << endl << EndDebugInfo;
				}
				mmu_access.tlb_way = tlb_way;
				mmu_access.dtlb_entry = tlb_entry;
				mmu_access.tlb_hit = true;
				return;
			}
		}
	}

	// DTLB Miss
	mmu_access.tlb_hit = false;
	ChooseEntryToEvictDTLB(mmu_access);
	if(unlikely(IsVerboseDTLB()))
	{
		logger << DebugInfo << "DTLB miss: choosen TLB way=" << mmu_access.tlb_way << endl << EndDebugInfo;
	}
	num_dtlb_misses++;
}

template <class CONFIG>
void CPU<CONFIG>::UpdateITLBReplacementPolicy(MMUAccess<CONFIG>& mmu_access)
{
	uint32_t i;
	uint32_t n;
	uint32_t way = mmu_access.tlb_way;
	uint32_t plru_bits = mmu_access.itlb_set->status.plru_bits;

	for(n = CONFIG::ITLB_CONFIG::TLB_LOG_ASSOCIATIVITY, i = 0; n != 0; n--)
	{
		uint32_t bi = (way >> (n - 1)) & 1;
		plru_bits = (plru_bits & ~(1 << i)) | ((1 ^ bi) << i);
		i = (i << 1) + 1 + bi;
	}

	mmu_access.itlb_set->status.plru_bits = plru_bits;
}

template <class CONFIG>
void CPU<CONFIG>::UpdateDTLBReplacementPolicy(MMUAccess<CONFIG>& mmu_access)
{
	uint32_t i;
	uint32_t n;
	uint32_t way = mmu_access.tlb_way;
	uint32_t plru_bits = mmu_access.dtlb_set->status.plru_bits;

	for(n = CONFIG::DTLB_CONFIG::TLB_LOG_ASSOCIATIVITY, i = 0; n != 0; n--)
	{
		uint32_t bi = (way >> (n - 1)) & 1;
		plru_bits = (plru_bits & ~(1 << i)) | ((1 ^ bi) << i);
		i = (i << 1) + 1 + bi;
	}

	mmu_access.dtlb_set->status.plru_bits = plru_bits;
}

template <class CONFIG>
void CPU<CONFIG>::ChooseEntryToEvictITLB(MMUAccess<CONFIG>& mmu_access)
{
	uint32_t way = 0;
	uint32_t i = 0;
	uint32_t n;
	uint32_t plru_bits = mmu_access.itlb_set->status.plru_bits;

	for(n = 0; n < CONFIG::ITLB_CONFIG::TLB_LOG_ASSOCIATIVITY; n++)
	{
		uint32_t bi = (plru_bits >> i) & 1;
		way = (way << 1) | bi;
		i = (i << 1) + 1 + bi;
	}

	mmu_access.tlb_way = way;
	mmu_access.itlb_entry = &(*mmu_access.itlb_set)[way];
}

template <class CONFIG>
void CPU<CONFIG>::ChooseEntryToEvictDTLB(MMUAccess<CONFIG>& mmu_access)
{
	uint32_t way = 0;
	uint32_t i = 0;
	uint32_t n;
	uint32_t plru_bits = mmu_access.dtlb_set->status.plru_bits;

	for(n = 0; n < CONFIG::DTLB_CONFIG::TLB_LOG_ASSOCIATIVITY; n++)
	{
		uint32_t bi = (plru_bits >> i) & 1;
		way = (way << 1) | bi;
		i = (i << 1) + 1 + bi;
	}

	mmu_access.tlb_way = way;
	mmu_access.dtlb_entry = &(*mmu_access.dtlb_set)[way];
}

template <class CONFIG>
template <bool DEBUG>
void CPU<CONFIG>::AccessTLB(MMUAccess<CONFIG>& mmu_access)
{
	typename CONFIG::address_t addr = mmu_access.addr;
	typename CONFIG::PrivilegeLevel privilege_level = mmu_access.privilege_level;
	typename CONFIG::MemoryAccessType memory_access_type = mmu_access.memory_access_type;
	typename CONFIG::MemoryType memory_type = mmu_access.memory_type;
/*	uint32_t bat_num;						// BAT register number
	uint32_t bepi;							// BEPI bit field*/
	uint32_t sr_num;						// Segment register number
	uint32_t virtual_segment_id;			// Virtual segment id
	uint32_t sr_ks;							// Supervisor key of a segment register
	uint32_t sr_kp;							// User key of a segment register
	uint32_t sr_noexecute;					// No-execute bit of a segment register
	typename CONFIG::virtual_address_t virtual_addr;			// Virtual address
	typename CONFIG::virtual_address_t base_virtual_addr;	// Base virtual address
/*	uint32_t tlb_index;						// An index in DTLB/ITLB
	uint32_t h;*/
/*	uint32_t page_index;					// page index
	uint32_t api;							// Abreviated page index*/
/*	uint32_t hash;							// hash key
	uint32_t pteg_select;
	physical_address_t pte_addr;			// a page table entry address
	uint32_t pte_num;*/
	bool key;
	typename CONFIG::WIMG wimg;
// 	TLBSet<typename CONFIG::ITLB_CONFIG> *tlb_set;
// 	TLBEntry<typename CONFIG::ITLB_CONFIG> *tlb_entry;

	// Compute the protection boundary
	mmu_access.protection_boundary = (addr + (CONFIG::MEMORY_PAGE_SIZE)) & ~(CONFIG::MEMORY_PAGE_SIZE - 1);

	//-------------------------------
	//- Compute the virtual address -
	//-------------------------------

	// Get the segment register number from the most significative bits in the effective address
	mmu_access.sr_num = sr_num = (addr >> 28) & 0xf;
	
	// Get the No-execute bit from the segment register
	mmu_access.sr_noexecute = sr_noexecute = GetSR_N(sr_num);

	if(unlikely((memory_type == CONFIG::MT_INSN) && sr_noexecute))
	{
		// Raise an exception because the program is trying to execute instructions within a No-execute segment
		if(!DEBUG) throw ISINoExecuteException<CONFIG>(addr);
	}
	
	if(unlikely(GetSR_T(sr_num) == 1))
	{
		// Raise an exception because the program is trying to access to a direct-store segment (not supported by PPC750)
		if(memory_type == CONFIG::MT_INSN)
		{
			if(!DEBUG) throw ISIDirectStoreException<CONFIG>(addr);
		}
		else
		{
			if(!DEBUG) throw DSIDirectStoreException<CONFIG>(addr, memory_access_type);
		}
	}
	
	// Get the key bits from the segment register
	mmu_access.sr_ks = sr_ks = GetSR_KS(sr_num);
	mmu_access.sr_kp = sr_kp = GetSR_KP(sr_num);
	mmu_access.key = key = (sr_kp && (privilege_level == CONFIG::PR_USER)) || (sr_ks && (privilege_level == CONFIG::PR_SUPERVISOR));
	
	// Get the virtual segment id from the segment register
	mmu_access.virtual_segment_id = virtual_segment_id = GetSR_VSID(sr_num);
	
	// Compute the virtual address from the virtual segment id and the least significant bits of the effective address
	mmu_access.virtual_addr = virtual_addr = ((typename CONFIG::virtual_address_t) virtual_segment_id << 28) | (typename CONFIG::virtual_address_t)(addr & 0x0fffffffUL);
	
	mmu_access.force_page_table_walk = false;
	//------------------------------
	//-    Search in ITLB/DTLB     -
	//------------------------------
	if(memory_type == CONFIG::MT_INSN)
	{
		if(CONFIG::ITLB_CONFIG::ENABLE)
		{
			// Memory access is an instruction fetch
			LookupITLB(mmu_access);
			if(unlikely(itlb_power_estimator_import != 0)) itlb_power_estimator_import->ReportReadAccess();

			if(likely(mmu_access.tlb_hit))
			{
				// ITLB hit
				mmu_access.wimg = wimg = mmu_access.itlb_entry->pte.wimg;
						
				if(unlikely(wimg & CONFIG::WIMG_GUARDED_MEMORY))
				{
					if(!DEBUG) throw ISIGuardedMemoryException<CONFIG>(addr);
				}
			
				// Check access rights
				if(unlikely(key && !mmu_access.itlb_entry->pte.pp))
				{
					if(!DEBUG) throw ISIProtectionViolationException<CONFIG>(addr);
				}
			
				// Update the replacement policy
				if(!DEBUG) UpdateITLBReplacementPolicy(mmu_access);
				
				// compute the physical address
				mmu_access.physical_addr = mmu_access.itlb_entry->pte.base_physical_addr | (addr & 0x00000fffUL);
			}
		
			// ITLB Miss
			return;
		}

		mmu_access.itlb_set = 0;
		mmu_access.itlb_entry = 0;
	}
	else
	{
		if(CONFIG::DTLB_CONFIG::ENABLE)
		{
			// Memory access is a data memory access
			LookupDTLB(mmu_access);
			if(unlikely(dtlb_power_estimator_import != 0)) dtlb_power_estimator_import->ReportReadAccess();

			if(likely(mmu_access.tlb_hit))
			{
				// DTLB Hit
				// return typename CONFIG::WIMG bits
				mmu_access.wimg = wimg = mmu_access.dtlb_entry->pte.wimg;
	
				// Check access rights
				if(unlikely((key && (!mmu_access.dtlb_entry->pte.pp ||
				            ((mmu_access.dtlb_entry->pte.pp & 1) && (memory_access_type == CONFIG::MAT_WRITE)))) ||
                             (!key && (mmu_access.dtlb_entry->pte.pp==3 && (memory_access_type == CONFIG::MAT_WRITE)))))
				{
					// Raise a protection violation
					if(!DEBUG)
					{
						throw DSIProtectionViolationException<CONFIG>(addr, memory_access_type);
					}
				}
				
				mmu_access.force_page_table_walk = memory_access_type == CONFIG::MAT_WRITE && !mmu_access.dtlb_entry->pte.c;

				// Update the replacement policy
				if(likely(!DEBUG && !mmu_access.force_page_table_walk)) UpdateDTLBReplacementPolicy(mmu_access);
					
				// compute the physical address
				mmu_access.physical_addr = mmu_access.dtlb_entry->pte.base_physical_addr | (addr & 0x00000fffUL);
				//cerr << "DTLB hit at 0x" << std::hex << addr << std::dec << endl;
			}
			// DTLB Miss
			return;
		}

		mmu_access.dtlb_set = 0;
		mmu_access.dtlb_entry = 0;
	}
	// Miss in ITLB/DTLB
	mmu_access.tlb_hit = false;
	mmu_access.base_virtual_addr = base_virtual_addr = virtual_addr & ~((typename CONFIG::virtual_address_t) CONFIG::MEMORY_PAGE_SIZE - 1);
}

template <class CONFIG>
template <bool DEBUG>
void CPU<CONFIG>::EmuHardwarePageTableSearch(MMUAccess<CONFIG>& mmu_access)
{
	typename CONFIG::address_t addr = mmu_access.addr;
	typename CONFIG::PrivilegeLevel privilege_level = mmu_access.privilege_level;
	typename CONFIG::MemoryAccessType memory_access_type = mmu_access.memory_access_type;
	typename CONFIG::MemoryType memory_type = mmu_access.memory_type;
	uint32_t sr_num = mmu_access.sr_num;						// Segment register number
	uint32_t virtual_segment_id = mmu_access.virtual_segment_id;			// Virtual segment id
//	virtual_address_t virtual_addr = mmu_access.virtual_addr;			// Virtual address
	typename CONFIG::virtual_address_t base_virtual_addr = mmu_access.base_virtual_addr;	// Base virtual address
//	uint32_t tlb_index = mmu_access.tlb_index;						// An index in DTLB/ITLB
	uint32_t h;
	uint32_t page_index;					// page index
	uint32_t api;							// Abreviated page index
	uint32_t hash;							// hash key
	uint32_t pteg_select;
	typename CONFIG::physical_address_t pte_addr;			// a page table entry address
	uint32_t pte_num;
	bool key = mmu_access.key;
	typename CONFIG::WIMG wimg;

	// Compute the page index from the virtual address
	mmu_access.page_index = page_index = (base_virtual_addr >> 12) & 0xffff;

	// Compute API from the page index
	mmu_access.api = api = page_index >> 10;

	if(unlikely(!DEBUG && GetHID0_STEN()))
	{
		// Software page table search is enabled
		
		/* primary PTEG address */
		hash = (virtual_segment_id & 0x7ffff) ^ page_index;
		pteg_select = (((GetSDR1_HTABORG() & 0x1ff) | (GetSDR1_HTABMASK() & ((hash >> 10) & 0x1ff))) << 10) | (hash & 0x3ff);
		typename CONFIG::physical_address_t primary_pteg = (((GetSDR1_HTABORG() >> 9) & 0x7f) << 25) | (pteg_select << 6);
			
		/* secondary PTEG address */
		hash = (~hash) & 0x7ffff;
		pteg_select = (((GetSDR1_HTABORG() & 0x1ff) | (GetSDR1_HTABMASK() & ((hash >> 10) & 0x1ff))) << 10) | (hash & 0x3ff);
		typename CONFIG::physical_address_t secondary_pteg = (((GetSDR1_HTABORG() >> 9) & 0x7f) << 25) | (pteg_select << 6);
		
		// raise a TLB miss
		throw TLBMissException<CONFIG>(
			memory_access_type,
			memory_type,
			addr,
			mmu_access.tlb_way,
			(privilege_level == CONFIG::PR_USER) ? GetSR_KP(sr_num) : GetSR_KS(sr_num),
			virtual_segment_id,
			api,
			primary_pteg,
			secondary_pteg);
	}
	
	//-------------------------------------
	//- Hardware search in the page table -
	//-------------------------------------

	// search with the primary hash key and if needed with the secondary hash key
	for(h = 0, hash = (virtual_segment_id & 0x7ffff) ^ page_index; h < 2; h++, hash = (~hash) & 0x7ffff)
	{
		// Compute the base address of a group of 8 page table entries
		pteg_select = (((GetSDR1_HTABORG() & 0x1ff) | (GetSDR1_HTABMASK() & ((hash >> 10) & 0x1ff))) << 10) | (hash & 0x3ff);
		pte_addr = (((GetSDR1_HTABORG() >> 9) & 0x7f) << 25) | (pteg_select << 6);

		// Scan each page table entry in the group
		for(pte_num = 0; pte_num < 8; pte_num++, pte_addr += 8)
		{
			uint64_t pte_value;
			uint32_t pte_virtual_segment_id;
			uint32_t pte_api;
			uint32_t pte_valid;
			uint32_t pte_h;
			uint32_t pte_pp;
			typename CONFIG::WIMG pte_wimg;
			uint32_t pte_c;
			typename CONFIG::physical_address_t base_physical_addr;


//			cerr << "pte_addr = 0x" << std::hex << pte_addr << std::dec << endl;
			// Read the page table entry from memory
			if(DEBUG)
			{
				ReadMemory(pte_addr, &pte_value, 8, CONFIG::MT_DATA, false);
			}
			else
			{
				EmuLoad<false>(pte_addr, &pte_value, 8);
			}

			pte_value = BigEndian2Host(pte_value);
//			cerr << "pte_value = 0x" << std::hex << pte_value << std::dec << endl;

			pte_valid = (pte_value >> 63) & 1;
//			cerr << "pte_valid = " << pte_valid << endl;
			if(likely(!pte_valid)) // check valid bit
				continue;
			pte_h = (pte_value >> 38) & 1;
//			cerr << "pte_h = " << pte_h << endl;
			if(likely(pte_h != h)) // good hash key ?
				continue;

			pte_virtual_segment_id = (pte_value >> 39) & 0xffffffUL;
//			cerr << "pte_virtual_segment_id = 0x" << std::hex << pte_virtual_segment_id << std::dec << endl;
			pte_api = (pte_value >> 32) & 0x3f;
			
			if(unlikely(pte_virtual_segment_id == virtual_segment_id && // compare VSIDs
						pte_api == api)) // compare APIs
			{
				// Hit in page table
				pte_wimg = (typename CONFIG::WIMG)((pte_value >> 3) & 0xf);
				mmu_access.wimg = wimg = pte_wimg;
				
				// Check if access is guarded
				if(unlikely((memory_type == CONFIG::MT_INSN) && (wimg & CONFIG::WIMG_GUARDED_MEMORY)))
				{
					if(!DEBUG) throw ISIGuardedMemoryException<CONFIG>(addr);
				}

				pte_pp = (pte_value & 3);
				
				// Check access rights
				if(unlikely((key && (!pte_pp || ((pte_pp & 1) && (memory_access_type == CONFIG::MAT_WRITE)))) || (!key && (pte_pp==3 && (memory_access_type == CONFIG::MAT_WRITE)))))
				{
					// Raise a protection violation
					if(memory_type == CONFIG::MT_INSN)
					{
						if(!DEBUG) throw ISIProtectionViolationException<CONFIG>(addr);
					}
					else
					{
						if(!DEBUG) throw DSIProtectionViolationException<CONFIG>(addr, memory_access_type);
					}
				}
				
				// Update bit R and C in PTE
				pte_value |= 0x100ULL; // set accessed bit (R)
				if((memory_access_type == CONFIG::MAT_WRITE)) pte_value |= 0x80ULL; // set change bit (C)

				base_physical_addr = pte_value & 0xfffff000UL;
//				cerr << "base_physical_addr = 0x" << std::hex << base_physical_addr << std::dec << endl;
				
				// Build an access attribute for the page	
				pte_c = (pte_value >> 7) & 1;

				// Update the page table entry into memory
				pte_value = Host2BigEndian(pte_value);

				if(!DEBUG)
				{
					EmuStore<false>(pte_addr, &pte_value, 8);
				}

				if(memory_type == CONFIG::MT_INSN)
				{
					if(CONFIG::ITLB_CONFIG::ENABLE)
					{
						// ITLB Refill
						if(unlikely(itlb_power_estimator_import != 0)) itlb_power_estimator_import->ReportWriteAccess();
						mmu_access.itlb_entry->status.valid = true;
						mmu_access.itlb_entry->SetBaseVirtualAddr(base_virtual_addr);
						mmu_access.itlb_entry->pte.c = pte_c;
						mmu_access.itlb_entry->pte.wimg = pte_wimg;
						mmu_access.itlb_entry->pte.pp = pte_pp;
						mmu_access.itlb_entry->pte.base_physical_addr = base_physical_addr;
	
						if(!DEBUG)
						{
							UpdateITLBReplacementPolicy(mmu_access);
						}
					}
				}
				else
				{
					if(CONFIG::DTLB_CONFIG::ENABLE)
					{
						// DTLB Refill
						if(unlikely(dtlb_power_estimator_import != 0)) dtlb_power_estimator_import->ReportWriteAccess();
						mmu_access.dtlb_entry->status.valid = true;
						mmu_access.dtlb_entry->SetBaseVirtualAddr(base_virtual_addr);
						mmu_access.dtlb_entry->pte.c = pte_c;
						mmu_access.dtlb_entry->pte.wimg = pte_wimg;
						mmu_access.dtlb_entry->pte.pp = pte_pp;
						mmu_access.dtlb_entry->pte.base_physical_addr = base_physical_addr;
	
						if(!DEBUG)
						{
							UpdateDTLBReplacementPolicy(mmu_access);
						}
					}
				}
			
				// compute the physical address
				mmu_access.physical_addr = base_physical_addr | (addr & 0x00000fffUL);
				return;
			}
		}
	}

	// Miss in the page table
	// Raise a page fault
	//DumpPageTable(cerr);
	
	if(memory_type == CONFIG::MT_INSN)
	{
		if(!DEBUG)
		{
			throw ISIPageFaultException<CONFIG>(addr);
		}
	}
	else
	{
		if(!DEBUG)
		{
			throw DSIPageFaultException<CONFIG>(addr, memory_access_type);
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::DumpPageTable(ostream& os)
{
	uint32_t h;
	uint32_t page_index;					// page index
	uint32_t api;							// Abreviated page index
	uint32_t hash[2];						// hash keys
	typename CONFIG::physical_address_t pte_addr;			// a page table entry address
	uint32_t pte_num;
	uint32_t mask = GetSDR1_HTABMASK();
	uint32_t hash_lo, hash_hi;
	typename CONFIG::physical_address_t pteg_select;
	
	os << "HTABORG = 0x" << std::hex << GetSDR1_HTABORG() << std::dec << endl;
	os << "HTABMASK = 0x" << std::hex << GetSDR1_HTABMASK() << std::dec << endl;
	
	
	for(hash_hi = 0; hash_hi <= mask; hash_hi++)
	{
		for(hash_lo = 0; hash_lo < 1024; hash_lo++)
		{
			hash[0] = (hash_hi << 10) | hash_lo;
			hash[1] = (~hash[0]) & 0x7ffff;
			
			for(h = 0; h < 2; h++)
			{
				pteg_select = (((GetSDR1_HTABORG() & 0x1ff) | (GetSDR1_HTABMASK() & ((hash[h] >> 10) & 0x1ff))) << 10) | (hash[h] & 0x3ff);
				pte_addr = (((GetSDR1_HTABORG() >> 9) & 0x7f) << 25) | (pteg_select << 6);
		
				for(pte_num = 0; pte_num < 8; pte_num++, pte_addr += 8)
				{
					uint64_t pte_value;
					typename CONFIG::virtual_address_t base_virtual_addr;
					uint32_t pte_valid, pte_h, pte_virtual_segment_id, pte_api, pte_pp, pte_r, pte_c, pte_wimg;
					typename CONFIG::address_t base_paddr;

					ReadMemory(pte_addr, &pte_value, 8, CONFIG::MT_DATA, false);
		
					pte_value = BigEndian2Host(pte_value);
		
					pte_valid = (pte_value >> 63) & 1;
					if(!pte_valid) continue;
					pte_h = (pte_value >> 38) & 1;
					if(pte_h != h) continue;
					
					pte_virtual_segment_id = (pte_value >> 39) & 0xffffffUL;
					pte_api = (pte_value >> 32) & 0x3f;
					
					page_index = (pte_virtual_segment_id & 0x7ffff) ^ hash[0];
					api = page_index >> 10; 
					base_virtual_addr = (pte_virtual_segment_id << 28) | (page_index << 12);
					if(pte_valid && pte_api == api)
					{
						pte_r = (pte_value >> 8) & 1;
						pte_c = (pte_value >> 7) & 1;
						pte_pp = (pte_value & 3);
						pte_wimg = (pte_value >> 3) & 0xfUL;
						base_paddr = pte_value & 0xfffff000UL;
						
						os << "0x" << std::hex << pte_addr << ": base_virt_addr=0x" << base_virtual_addr << " hash=0x" << hash << " V=" << pte_valid << " VSID=0x" << pte_virtual_segment_id << " H=" << pte_h << " API=0x" << pte_api << " pp=" << pte_pp << " r=" << pte_r << " c=" << pte_c << " base_paddr=0x" << base_paddr << " wimg=" << pte_wimg << std::dec << endl;
					}
				}
			}
		}
	}
}

template <class CONFIG>
template <bool DEBUG>
void CPU<CONFIG>::EmuTranslateAddress(MMUAccess<CONFIG>& mmu_access)
{
	LookupBAT<DEBUG>(mmu_access);
	if(unlikely(!mmu_access.bat_hit))
	{
		AccessTLB<DEBUG>(mmu_access);

		if(unlikely(!mmu_access.tlb_hit || mmu_access.force_page_table_walk))
		{
			// BAT/TLB Miss
			EmuHardwarePageTableSearch<DEBUG>(mmu_access);
		}
	}
}

/* TLB management */
template <class CONFIG>
void CPU<CONFIG>::Tlbia()
{
	throw IllegalInstructionException<CONFIG>();
}

template <class CONFIG>
void CPU<CONFIG>::Tlbie(typename CONFIG::address_t addr)
{
	if(GetMSR_PR())
	{
		throw PrivilegeViolationException<CONFIG>();
	}
	InvalidateDTLBSet((addr / CONFIG::MEMORY_PAGE_SIZE) & ((CONFIG::DTLB_CONFIG::TLB_NUM_ENTRIES / CONFIG::DTLB_CONFIG::TLB_ASSOCIATIVITY) - 1));
	InvalidateITLBSet((addr / CONFIG::MEMORY_PAGE_SIZE) & ((CONFIG::ITLB_CONFIG::TLB_NUM_ENTRIES / CONFIG::ITLB_CONFIG::TLB_ASSOCIATIVITY) - 1));
}

template <class CONFIG>
void CPU<CONFIG>::Tlbre(unsigned int rd, unsigned int ra, unsigned int ws)
{
	throw IllegalInstructionException<CONFIG>();
}

template <class CONFIG>
void CPU<CONFIG>::Tlbsx(unsigned int rd, typename CONFIG::address_t addr)
{
	throw IllegalInstructionException<CONFIG>();
}

template <class CONFIG>
void CPU<CONFIG>::Tlbwe(unsigned int rs, unsigned int ra, unsigned int ws)
{
	throw IllegalInstructionException<CONFIG>();
}

template <class CONFIG>
void CPU<CONFIG>::LoadITLBEntry(typename CONFIG::address_t addr, uint32_t way, uint32_t pte_hi, uint32_t pte_lo)
{
	if(CONFIG::ITLB_CONFIG::ENABLE)
	{
		uint32_t index = addr / CONFIG::MEMORY_PAGE_SIZE;
		uint32_t virtual_segment_id = (pte_hi >> 7) & 0xffffffUL;
		typename CONFIG::address_t base_physical_addr = pte_lo & ~(CONFIG::MEMORY_PAGE_SIZE - 1);
		typename CONFIG::virtual_address_t base_virtual_addr = ((typename CONFIG::virtual_address_t) virtual_segment_id << 28) | (typename CONFIG::virtual_address_t)(addr & 0x0fffffffUL & ~(CONFIG::MEMORY_PAGE_SIZE - 1));
		
		TLBEntry<typename CONFIG::ITLB_CONFIG>& tlb_entry = itlb[index][way];
	
		tlb_entry.status.valid = ((pte_hi >> 31) & 1) ? true : false;
		tlb_entry.SetBaseVirtualAddr(base_virtual_addr);
		tlb_entry.pte.c = (pte_lo >> 7) & 1;
		tlb_entry.pte.wimg = (typename CONFIG::WIMG)((pte_lo >> 3) & 0xf);
		tlb_entry.pte.pp = (pte_lo & 3);
		tlb_entry.pte.base_physical_addr = base_physical_addr;
	}
}

template <class CONFIG>
void CPU<CONFIG>::LoadDTLBEntry(typename CONFIG::address_t addr, uint32_t way, uint32_t pte_hi, uint32_t pte_lo)
{
	if(CONFIG::DTLB_CONFIG::ENABLE)
	{
		uint32_t index = addr / CONFIG::MEMORY_PAGE_SIZE;
		uint32_t virtual_segment_id = (pte_hi >> 7) & 0xffffffUL;
		typename CONFIG::address_t base_physical_addr = pte_lo & ~(CONFIG::MEMORY_PAGE_SIZE - 1);
		typename CONFIG::virtual_address_t base_virtual_addr = ((typename CONFIG::virtual_address_t) virtual_segment_id << 28) | (typename CONFIG::virtual_address_t)(addr & 0x0fffffffUL & ~(CONFIG::MEMORY_PAGE_SIZE - 1));
		
		TLBEntry<typename CONFIG::DTLB_CONFIG>& tlb_entry = dtlb[index][way];
	
		tlb_entry.SetBaseVirtualAddr(base_virtual_addr);
		tlb_entry.pte.c = (pte_lo >> 7) & 1;
		tlb_entry.pte.wimg = (typename CONFIG::WIMG)((pte_lo >> 3) & 0xf);
		tlb_entry.pte.pp = (pte_lo & 3);
		tlb_entry.pte.base_physical_addr = base_physical_addr;
	}
}

template <class CONFIG>
void CPU<CONFIG>::Tlbld(typename CONFIG::address_t addr)
{
	if(GetMSR_PR())
	{
		throw PrivilegeViolationException<CONFIG>();
	}

	LoadDTLBEntry(addr, addr & 1, GetPTEHI(), GetPTEHI());
}

template <class CONFIG>
void CPU<CONFIG>::Tlbli(typename CONFIG::address_t addr)
{
	if(GetMSR_PR())
	{
		throw PrivilegeViolationException<CONFIG>();
	}

	LoadITLBEntry(addr, addr & 1, GetPTEHI(), GetPTEHI());
}

} // end of namespace mpc7447a
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
