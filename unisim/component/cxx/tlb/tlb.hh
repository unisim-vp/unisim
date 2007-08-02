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
 
#ifndef __UNISIM_COMPONENT_CXX_TLB_TLB_HH__
#define __UNISIM_COMPONENT_CXX_TLB_TLB_HH__

#include <unisim/service/interfaces/cache_power_estimator.hh>
#include <unisim/service/interfaces/power_mode.hh>
#include <iostream>

namespace unisim {
namespace component {
namespace cxx {
namespace tlb {

using unisim::kernel::service::Service;
using unisim::kernel::service::Client;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::Object;
using unisim::kernel::service::Parameter;
using unisim::kernel::service::ParameterArray;
using unisim::service::interfaces::CachePowerEstimator;
using unisim::service::interfaces::PowerMode;
using namespace std;

typedef enum { RP_LRU, RP_PLRU } ReplacementPolicy;

#if 0
class MemoryPageTableEntry
{
public:
	bool valid;									/*< true if entry is valid, false otherwise */
	virtual_address_t base_virtual_addr;		/*< 52-bit base virtual address */
	uint32_t way;								/*< TLB way */
	uint32_t lru_bits;							/*< computed only when saving the TLBs, and restored when restoring the TLBs, */
												/*< these bits are not updated during simulation. Instead next and prev pointers */
												/*< are used to sort PTE according LRU replacement policy */
	physical_address_t base_physical_addr;		/*< 32-bit base physical address */
	MemoryPageTableEntry *prev; 			/*< previous entry (unused if TLB associativity is 1) */
	MemoryPageTableEntry *next; 			/*< next entry (unused if TLB associativity is 1) */
};
#endif

template <class MEMORY_PAGE_TABLE_ENTRY_T, class VIRTUAL_ADDRESS_T, uint32_t NUM_TLB_ENTRIES, uint32_t MEMORY_PAGE_SIZE, uint32_t TLB_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
class TLB :
	public Service<PowerMode>,
	public Client<CachePowerEstimator>,
	public Client<PowerMode>
{
public:
	ServiceExport<PowerMode> power_mode_export;
	ServiceImport<PowerMode> power_mode_import;
	ServiceImport<CachePowerEstimator> power_estimator_import;
	
	TLB(const char *name, Object *parent = 0);
	virtual ~TLB();

	// Power mode interface
	virtual void SetPowerMode(unsigned int frequency, unsigned int voltage);
	virtual unsigned int GetMaxFrequency(); // in Mhz
	virtual unsigned int GetDefaultVoltage(); // in mV

	void Invalidate();
	void InvalidateEntry(VIRTUAL_ADDRESS_T virtual_addr);
	void InvalidateSet(uint32_t index);
	MEMORY_PAGE_TABLE_ENTRY_T *GetEntry(uint32_t index, uint32_t way);
	
	inline MEMORY_PAGE_TABLE_ENTRY_T *SearchEntry(uint32_t index, VIRTUAL_ADDRESS_T base_virtual_addr)
#if defined(__GNUC__) && (__GNUC__ >= 3)
	__attribute__((always_inline))
#endif
	;

	MEMORY_PAGE_TABLE_ENTRY_T *ChooseEntryToReplace(uint32_t index);
	
	inline void DecodeAddress(VIRTUAL_ADDRESS_T virtual_addr, VIRTUAL_ADDRESS_T& base_virtual_addr, uint32_t& index)
#if defined(__GNUC__) && (__GNUC__ >= 3)
			__attribute__((always_inline))
#endif
	;

	inline void UpdateReplacementPolicy(MEMORY_PAGE_TABLE_ENTRY_T *pte, uint32_t index)
#if defined(__GNUC__) && (__GNUC__ >= 3)
			__attribute__((always_inline))
#endif
	;

	void Refill(MEMORY_PAGE_TABLE_ENTRY_T *pte, uint32_t index);
private:
	MEMORY_PAGE_TABLE_ENTRY_T entries[NUM_TLB_ENTRIES / TLB_ASSOCIATIVITY][TLB_ASSOCIATIVITY];
	MEMORY_PAGE_TABLE_ENTRY_T *mru_pte[NUM_TLB_ENTRIES / TLB_ASSOCIATIVITY];
	MEMORY_PAGE_TABLE_ENTRY_T *lru_pte[NUM_TLB_ENTRIES / TLB_ASSOCIATIVITY];
};

template <class MEMORY_PAGE_TABLE_ENTRY_T, class VIRTUAL_ADDRESS_T, uint32_t NUM_TLB_ENTRIES, uint32_t MEMORY_PAGE_SIZE, uint32_t TLB_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
TLB<MEMORY_PAGE_TABLE_ENTRY_T, VIRTUAL_ADDRESS_T, NUM_TLB_ENTRIES, MEMORY_PAGE_SIZE, TLB_ASSOCIATIVITY, REPLACEMENT_POLICY>::TLB(const char *name, Object *parent) :
	Object(name, parent),
	Client<PowerMode>(name, parent),
	Client<CachePowerEstimator>(name, parent),
	Service<PowerMode>(name, parent),
	power_mode_import("power-mode-import", this),
	power_mode_export("power-mode-export", this),
	power_estimator_import("power-estimator-import", this)
{
	Invalidate();
}

template <class MEMORY_PAGE_TABLE_ENTRY_T, class VIRTUAL_ADDRESS_T, uint32_t NUM_TLB_ENTRIES, uint32_t MEMORY_PAGE_SIZE, uint32_t TLB_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
TLB<MEMORY_PAGE_TABLE_ENTRY_T, VIRTUAL_ADDRESS_T, NUM_TLB_ENTRIES, MEMORY_PAGE_SIZE, TLB_ASSOCIATIVITY, REPLACEMENT_POLICY>::~TLB()
{
}

template <class MEMORY_PAGE_TABLE_ENTRY_T, class VIRTUAL_ADDRESS_T, uint32_t NUM_TLB_ENTRIES, uint32_t MEMORY_PAGE_SIZE, uint32_t TLB_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
void TLB<MEMORY_PAGE_TABLE_ENTRY_T, VIRTUAL_ADDRESS_T, NUM_TLB_ENTRIES, MEMORY_PAGE_SIZE, TLB_ASSOCIATIVITY, REPLACEMENT_POLICY>::Invalidate()
{
	uint32_t index;
	uint32_t way;
	
	for(index = 0; index < NUM_TLB_ENTRIES / TLB_ASSOCIATIVITY; index++)
	{
		InvalidateSet(index);
	}
}

template <class MEMORY_PAGE_TABLE_ENTRY_T, class VIRTUAL_ADDRESS_T, uint32_t NUM_TLB_ENTRIES, uint32_t MEMORY_PAGE_SIZE, uint32_t TLB_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
void TLB<MEMORY_PAGE_TABLE_ENTRY_T, VIRTUAL_ADDRESS_T, NUM_TLB_ENTRIES, MEMORY_PAGE_SIZE, TLB_ASSOCIATIVITY, REPLACEMENT_POLICY>::SetPowerMode(unsigned int frequency, unsigned int voltage)
{
	if(power_mode_import) power_mode_import->SetPowerMode(frequency, voltage);
}
	
template <class MEMORY_PAGE_TABLE_ENTRY_T, class VIRTUAL_ADDRESS_T, uint32_t NUM_TLB_ENTRIES, uint32_t MEMORY_PAGE_SIZE, uint32_t TLB_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
unsigned int TLB<MEMORY_PAGE_TABLE_ENTRY_T, VIRTUAL_ADDRESS_T, NUM_TLB_ENTRIES, MEMORY_PAGE_SIZE, TLB_ASSOCIATIVITY, REPLACEMENT_POLICY>::GetMaxFrequency()
{
	return power_mode_import ? power_mode_import->GetMaxFrequency() : 0;
}

template <class MEMORY_PAGE_TABLE_ENTRY_T, class VIRTUAL_ADDRESS_T, uint32_t NUM_TLB_ENTRIES, uint32_t MEMORY_PAGE_SIZE, uint32_t TLB_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
unsigned int TLB<MEMORY_PAGE_TABLE_ENTRY_T, VIRTUAL_ADDRESS_T, NUM_TLB_ENTRIES, MEMORY_PAGE_SIZE, TLB_ASSOCIATIVITY, REPLACEMENT_POLICY>::GetDefaultVoltage()
{
	return power_mode_import ? power_mode_import->GetDefaultVoltage() : 0;
}

template <class MEMORY_PAGE_TABLE_ENTRY_T, class VIRTUAL_ADDRESS_T, uint32_t NUM_TLB_ENTRIES, uint32_t MEMORY_PAGE_SIZE, uint32_t TLB_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
void TLB<MEMORY_PAGE_TABLE_ENTRY_T, VIRTUAL_ADDRESS_T, NUM_TLB_ENTRIES, MEMORY_PAGE_SIZE, TLB_ASSOCIATIVITY, REPLACEMENT_POLICY>::InvalidateEntry(VIRTUAL_ADDRESS_T virtual_addr)
{
	uint32_t index;
	VIRTUAL_ADDRESS_T base_virtual_addr;
	MEMORY_PAGE_TABLE_ENTRY_T *pte;

	DecodeAddress(virtual_addr, base_virtual_addr, index);

	pte = SearchEntry(index, base_virtual_addr);

	if(pte)
	{
		pte->valid = false;
	}
}

template <class MEMORY_PAGE_TABLE_ENTRY_T, class VIRTUAL_ADDRESS_T, uint32_t NUM_TLB_ENTRIES, uint32_t MEMORY_PAGE_SIZE, uint32_t TLB_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
void TLB<MEMORY_PAGE_TABLE_ENTRY_T, VIRTUAL_ADDRESS_T, NUM_TLB_ENTRIES, MEMORY_PAGE_SIZE, TLB_ASSOCIATIVITY, REPLACEMENT_POLICY>::InvalidateSet(uint32_t index)
{
	uint32_t way;
	
	index &= ((NUM_TLB_ENTRIES / TLB_ASSOCIATIVITY) - 1);

	for(way = 0; way < TLB_ASSOCIATIVITY; way++)
	{
		MEMORY_PAGE_TABLE_ENTRY_T *pte = &entries[index][way];
		
		//cerr << Object::GetName() << ":invalidating set #" << index << " : " << *pte << endl;
		
		pte->valid = false; // invalidate entry
		pte->base_virtual_addr = 0;
		pte->way = way;
		pte->lru_bits = (TLB_ASSOCIATIVITY - 1) - way;
		
		if(TLB_ASSOCIATIVITY > 1)
		{
			switch(REPLACEMENT_POLICY)
			{
				case RP_LRU:
					if(way > 0)
					{
						pte->next = &entries[index][way - 1];
					}
					else
					{
						pte->next = 0;
						lru_pte[index] = pte;
					}
					
					if(way < TLB_ASSOCIATIVITY - 1)
					{
						pte->prev = &entries[index][way + 1];
					}
					else
					{
						mru_pte[index] = pte;
						pte->prev = 0;
					}
					break;

				case RP_PLRU:
					break;
			}
		}
	}
}

template <class MEMORY_PAGE_TABLE_ENTRY_T, class VIRTUAL_ADDRESS_T, uint32_t NUM_TLB_ENTRIES, uint32_t MEMORY_PAGE_SIZE, uint32_t TLB_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
MEMORY_PAGE_TABLE_ENTRY_T *TLB<MEMORY_PAGE_TABLE_ENTRY_T, VIRTUAL_ADDRESS_T, NUM_TLB_ENTRIES, MEMORY_PAGE_SIZE, TLB_ASSOCIATIVITY, REPLACEMENT_POLICY>::GetEntry(uint32_t index, uint32_t way)
{
	return &entries[index & (NUM_TLB_ENTRIES - 1)][way & (TLB_ASSOCIATIVITY - 1)];
}

template <class MEMORY_PAGE_TABLE_ENTRY_T, class VIRTUAL_ADDRESS_T, uint32_t NUM_TLB_ENTRIES, uint32_t MEMORY_PAGE_SIZE, uint32_t TLB_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
inline MEMORY_PAGE_TABLE_ENTRY_T *TLB<MEMORY_PAGE_TABLE_ENTRY_T, VIRTUAL_ADDRESS_T, NUM_TLB_ENTRIES, MEMORY_PAGE_SIZE, TLB_ASSOCIATIVITY, REPLACEMENT_POLICY>::SearchEntry(uint32_t index, VIRTUAL_ADDRESS_T base_virtual_addr)
{
	MEMORY_PAGE_TABLE_ENTRY_T *pte;

	// Report a TLB lookup to the power estimator
	if(power_estimator_import) power_estimator_import->ReportReadAccess();
	
	for(pte = mru_pte[index]; pte; pte = pte->next)
	{
		if(pte->valid)
		{
			if(pte->base_virtual_addr == base_virtual_addr)
			{
				/* TLB Hit */
// 				UpdateReplacementPolicy(pte, index);
				return pte;
			}
		}
	}
	return 0;
}

template <class MEMORY_PAGE_TABLE_ENTRY_T, class VIRTUAL_ADDRESS_T, uint32_t NUM_TLB_ENTRIES, uint32_t MEMORY_PAGE_SIZE, uint32_t TLB_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
MEMORY_PAGE_TABLE_ENTRY_T *TLB<MEMORY_PAGE_TABLE_ENTRY_T, VIRTUAL_ADDRESS_T, NUM_TLB_ENTRIES, MEMORY_PAGE_SIZE, TLB_ASSOCIATIVITY, REPLACEMENT_POLICY>::ChooseEntryToReplace(uint32_t index)
{
	if(TLB_ASSOCIATIVITY > 1)
	{
		switch(REPLACEMENT_POLICY)
		{
			case RP_LRU:
				return lru_pte[index];
				break;

			case RP_PLRU:
				break;
		}
	}
	return 0;
}

template <class MEMORY_PAGE_TABLE_ENTRY_T, class VIRTUAL_ADDRESS_T, uint32_t NUM_TLB_ENTRIES, uint32_t MEMORY_PAGE_SIZE, uint32_t TLB_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
inline void TLB<MEMORY_PAGE_TABLE_ENTRY_T, VIRTUAL_ADDRESS_T, NUM_TLB_ENTRIES, MEMORY_PAGE_SIZE, TLB_ASSOCIATIVITY, REPLACEMENT_POLICY>::DecodeAddress(VIRTUAL_ADDRESS_T virtual_addr, VIRTUAL_ADDRESS_T& base_virtual_addr, uint32_t& index)
{
	base_virtual_addr = virtual_addr & ~((VIRTUAL_ADDRESS_T) MEMORY_PAGE_SIZE - 1);
	index = (base_virtual_addr / MEMORY_PAGE_SIZE) & ((NUM_TLB_ENTRIES / TLB_ASSOCIATIVITY) - 1);
}

template <class MEMORY_PAGE_TABLE_ENTRY_T, class VIRTUAL_ADDRESS_T, uint32_t NUM_TLB_ENTRIES, uint32_t MEMORY_PAGE_SIZE, uint32_t TLB_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
void TLB<MEMORY_PAGE_TABLE_ENTRY_T, VIRTUAL_ADDRESS_T, NUM_TLB_ENTRIES, MEMORY_PAGE_SIZE, TLB_ASSOCIATIVITY, REPLACEMENT_POLICY>::Refill(MEMORY_PAGE_TABLE_ENTRY_T *pte, uint32_t index)
{
	//cerr << Object::GetName() << ":refilling set #" << index << " : " << *pte << endl;
	// Report a TLB refill to the power estimator
	if(power_estimator_import) power_estimator_import->ReportWriteAccess();
}

template <class MEMORY_PAGE_TABLE_ENTRY_T, class VIRTUAL_ADDRESS_T, uint32_t NUM_TLB_ENTRIES, uint32_t MEMORY_PAGE_SIZE, uint32_t TLB_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
inline void TLB<MEMORY_PAGE_TABLE_ENTRY_T, VIRTUAL_ADDRESS_T, NUM_TLB_ENTRIES, MEMORY_PAGE_SIZE, TLB_ASSOCIATIVITY, REPLACEMENT_POLICY>::UpdateReplacementPolicy(MEMORY_PAGE_TABLE_ENTRY_T *pte, uint32_t index)
{
	if(TLB_ASSOCIATIVITY > 1)
	{
		switch(REPLACEMENT_POLICY)
		{
			case RP_LRU:
				if(pte->prev)
				{
					// Update LRU replacement policy
					if(pte->next)
					{
						pte->next->prev = pte->prev;
					}
					else
					{
						lru_pte[index] = pte->prev;
					}
						
					pte->prev->next = pte->next;
					pte->next = mru_pte[index];
					pte->prev = 0;
					mru_pte[index]->prev = pte;
					mru_pte[index] = pte;
				}
				break;

			case RP_PLRU:
				break;
		}
	}
}

} // end of namespace tlb
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
