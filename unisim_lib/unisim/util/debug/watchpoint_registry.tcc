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
 *          Yves Lhuillier (yves.lhuillier@cea.fr)
 */
 
#ifndef __UNISIM_UTIL_DEBUG_WATCHPOINT_REGISTRY_TCC__
#define __UNISIM_UTIL_DEBUG_WATCHPOINT_REGISTRY_TCC__

#include <iostream>
#include <string.h>
#include <set>

namespace unisim {
namespace util {
namespace debug {

template <typename ADDRESS, unsigned int MAX_FRONT_ENDS>
WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS>::WatchpointMapPage(ADDRESS addr)
{
	this->base_addr = addr & ~(NUM_WATCHPOINTS_PER_PAGE - 1);
	map = new WORD[(NUM_WATCHPOINTS_PER_PAGE + WATCHPOINTS_PER_WORD - 1)/ WATCHPOINTS_PER_WORD];
	memset(map, 0, ((NUM_WATCHPOINTS_PER_PAGE + WATCHPOINTS_PER_WORD - 1)/ WATCHPOINTS_PER_WORD) * sizeof(WORD));
	next = 0;
}

template <typename ADDRESS, unsigned int MAX_FRONT_ENDS>
WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS>::~WatchpointMapPage()
{
	delete[] map;
}

template <typename ADDRESS, unsigned int MAX_FRONT_ENDS>
unsigned int WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS>::SetWatchpoint(unisim::util::debug::MemoryAccessType mat, uint32_t offset, uint32_t size, unsigned int front_end_num)
{
	unsigned int watchpoint_count_increment = 0;
	WORD mask = ((mat & unisim::util::debug::MAT_READ ? 0x01UL : 0) | (mat & unisim::util::debug::MAT_WRITE ? 0x02UL : 0)) << (BITS_PER_WATCHPOINT * front_end_num);

	unsigned int i;
	
	for(i = 0; i < size; ++i, ++offset)
	{
		WORD& w = map[offset / WATCHPOINTS_PER_WORD];
		WORD m = mask << (BITS_PER_WATCHPOINTS * (offset % WATCHPOINTS_PER_WORD));
		if(!(w & m))
		{
			++watchpoint_count_increment;
			w |= m;
		}
	}
	
	return watchpoint_count_increment;
}

template <typename ADDRESS, unsigned int MAX_FRONT_ENDS>
unsigned int WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS>::RemoveWatchpoint(unisim::util::debug::MemoryAccessType mat, uint32_t offset, uint32_t size, unsigned int front_end_num)
{
	unsigned int watchpoint_count_decrement = 0;
	WORD mask = ((mat & unisim::util::debug::MAT_READ ? 0x01UL : 0) | (mat & unisim::util::debug::MAT_WRITE ? 0x02UL : 0)) << (BITS_PER_WATCHPOINT * front_end_num);

	unsigned int i;
	for(i = 0; i < size; ++i, ++offset)
	{
		WORD& w = map[offset / WATCHPOINTS_PER_WORD];
		WORD m = mask << (BITS_PER_WATCHPOINTS * (offset % WATCHPOINTS_PER_WORD));
		if(w & m)
		{
			++watchpoint_count_decrement;
			w &= ~m;
		}
	}
	return watchpoint_count_decrement;
}

template <typename ADDRESS, unsigned int MAX_FRONT_ENDS>
bool WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS>::HasWatchpoint(unisim::util::debug::MemoryAccessType mat, uint32_t offset, uint32_t size, unsigned int front_end_num) const
{
	// TODO: optimize this function for 1, 2, 4, 8 bytes accesses
	WORD mask = ((mat & unisim::util::debug::MAT_READ ? 0x01UL : 0) | (mat & unisim::util::debug::MAT_WRITE ? 0x02UL : 0)) << (BITS_PER_WATCHPOINT * front_end_num);

	unsigned int i;
	for(i = 0; i < size; ++i, ++offset)
	{
		if(map[offset / WATCHPOINTS_PER_WORD] & (mask << (BITS_PER_WATCHPOINTS * (offset % WATCHPOINTS_PER_WORD)))) return true;
	}
	
	return false;
}

template <typename ADDRESS, unsigned int MAX_FRONT_ENDS>
bool WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS>::HasWatchpoints(unisim::util::debug::MemoryAccessType mat, uint32_t offset, uint32_t size) const
{
	// TODO: optimize this function for 1, 2, 4, 8 bytes accesses
	WORD mask = 0;
	
	unsigned int front_end_num;
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; ++front_end_num)
	{
		mask |= ((mat & unisim::util::debug::MAT_READ ? 0x01UL : 0) | (mat & unisim::util::debug::MAT_WRITE ? 0x02UL : 0)) << (BITS_PER_WATCHPOINT * front_end_num);
	}

	unsigned int i;
	for(i = 0; i < size; ++i, ++offset)
	{
		if(map[offset / WATCHPOINTS_PER_WORD] & (mask << (BITS_PER_WATCHPOINTS * (offset % WATCHPOINTS_PER_WORD)))) return true;
	}
	
	return false;
}

template <typename CONFIG>
WatchpointRegistry<CONFIG>::WatchpointRegistry()
	: watchpoints()
	, watchpoint_count()
	, mru_page()
	, hash_table()
{
}

template <typename CONFIG>
WatchpointRegistry<CONFIG>::~WatchpointRegistry()
{
	Reset();
}

template <typename CONFIG>
void WatchpointRegistry<CONFIG>::Reset()
{
	unsigned int prc_num;
	unsigned int front_end_num;
	uint32_t mt;
	uint32_t index;
	for(mt = 0; mt < 2; ++mt)
	{
		for(prc_num = 0; prc_num < NUM_PROCESSORS; ++prc_num)
		{
			for(index = 0; index < NUM_HASH_TABLE_ENTRIES; ++index)
			{
				WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS> *page, *nextpage;
		
				page = hash_table[mt][prc_num][index];
				if(page)
				{
					do
					{
						nextpage = page->next;
						delete page;
						page = nextpage;
					} while(page);
					hash_table[mt][prc_num][index] = 0;
				}
			}
			
			mru_page[mt][prc_num] = 0;
		}
	}
		
	for(prc_num = 0; prc_num < NUM_PROCESSORS; ++prc_num)
	{
		watchpoint_count[prc_num] = 0;
		
		for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; ++front_end_num)
		{
			for(typename Watchpoints::iterator watchpoint_it = watchpoints[prc_num][front_end_num].begin(); watchpoint_it != watchpoints[prc_num][front_end_num].end(); ++watchpoint_it)
			{
				WATCHPOINT *watchpoint = (*watchpoint_it).second;
				watchpoint->Release();
			}
			
			watchpoints[prc_num][front_end_num].clear();
		}
	}
}

template <typename CONFIG>
void WatchpointRegistry<CONFIG>::Clear(unsigned int front_end_num)
{
	unsigned int prc_num;
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; ++prc_num)
	{
		for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; ++front_end_num)
		{
			typename Watchpoints::iterator watchpoint_it = watchpoints[prc_num][front_end_num].begin();
			
			do
			{
				WATCHPOINT *watchpoint = (*watchpoint_it).second;
				if(!RemoveWatchpoint(watchpoint))
				{
					std::cerr << "watchpoint was not removed" << std::endl;
				}
				watchpoint_it = watchpoints[prc_num][front_end_num].begin();
			}
			while(watchpoint_it != watchpoints[prc_num][front_end_num].end());
		}
	}
}

template <typename CONFIG>
bool WatchpointRegistry<CONFIG>::SetWatchpointIntoMap(unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size, unsigned int prc_num, unsigned int front_end_num)
{
	if(prc_num >= NUM_PROCESSORS) return false;
	if(front_end_num >= MAX_FRONT_ENDS) return false;
	
	if(size > 0)
	{
		do
		{
			uint32_t size_to_page_boundary = WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_WATCHPOINTS_PER_PAGE - (addr & (WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_WATCHPOINTS_PER_PAGE - 1));
			uint32_t sz = size > size_to_page_boundary ? size_to_page_boundary : size;

			AllocatePage(mt, addr, prc_num);
			WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS> *page = GetPage(mt, addr, prc_num);
			if(!page)
			{
				std::cerr << "Internal error: watchpoint page has not been allocated" << std::endl;
				return false;
			}
		
			watchpoint_count[prc_num] += page->SetWatchpoint(mat, addr & (WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_WATCHPOINTS_PER_PAGE - 1), sz, front_end_num);
			size -= sz;
			addr += sz;
		} while(size > 0);
	}
	
	return true;
}

template <typename CONFIG>
bool WatchpointRegistry<CONFIG>::RemoveWatchpointFromMap(unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size, unsigned int prc_num, unsigned int front_end_num)
{
	if(prc_num >= NUM_PROCESSORS) return false;
	if(front_end_num >= MAX_FRONT_ENDS) return false;
	
	if(size > 0)
	{
		do
		{
			uint32_t size_to_page_boundary = WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_WATCHPOINTS_PER_PAGE - (addr & (WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_WATCHPOINTS_PER_PAGE - 1));
			uint32_t sz = size > size_to_page_boundary ? size_to_page_boundary : size;

			WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS> *page = GetPage(mt, addr, prc_num);
			if(!page) return false;

			watchpoint_count[prc_num] -= page->RemoveWatchpoint(mat, addr & (WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_WATCHPOINTS_PER_PAGE - 1), sz, front_end_num);
			size -= sz;
			addr += sz;
		} while(size > 0);
	}

	return true;
}

template <typename CONFIG>
typename CONFIG::WATCHPOINT *WatchpointRegistry<CONFIG>::SetWatchpoint(unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size, bool overlook, unsigned int prc_num, unsigned int front_end_num)
{
	if(prc_num >= NUM_PROCESSORS) return 0;
	if(front_end_num >= MAX_FRONT_ENDS) return 0;
	
	if(!SetWatchpointIntoMap(mat, mt, addr, size, prc_num, front_end_num)) return 0;

	WATCHPOINT *watchpoint = 0;
	std::pair<typename Watchpoints::const_iterator, typename Watchpoints::const_iterator> range = watchpoints[prc_num][front_end_num].equal_range(addr);

	for(typename Watchpoints::const_iterator watchpoint_it = range.first; watchpoint_it != range.second; ++watchpoint_it)
	{
		WATCHPOINT *wp = (*watchpoint_it).second;
		
		if(wp->Equals(mat, mt, addr, size, overlook))
		{
			watchpoint = wp;
			break;
		}
	}
	
	if(!watchpoint)
	{
		watchpoint = new WATCHPOINT(prc_num, front_end_num, mat, mt, addr, size, overlook);
		
		do
		{
			watchpoints[prc_num][front_end_num].insert(typename Watchpoints::value_type(addr, watchpoint));
			watchpoint->Catch();
			--size;
			++addr;
		}
		while(size > 0);
	}
	
	return watchpoint;
}

template <typename CONFIG>
bool WatchpointRegistry<CONFIG>::RemoveWatchpoint(unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size, unsigned int prc_num, unsigned int front_end_num)
{
	if(prc_num >= NUM_PROCESSORS) return false;
	if(front_end_num >= MAX_FRONT_ENDS) return false;
	
	if(size > 0)
	{
		do
		{
			// invalidate watchpoints events that match the address, memory type, and memory access type
			typename std::pair<typename Watchpoints::iterator, typename Watchpoints::iterator> range = watchpoints[prc_num][front_end_num].equal_range(addr);

			typename Watchpoints::iterator watchpoint_it = range.first;
			
			while(watchpoint_it != range.second)
			{
				typename Watchpoints::iterator next_watchpoint_it = watchpoint_it;
				++next_watchpoint_it;
				
				WATCHPOINT *watchpoint = (*watchpoint_it).second;
				
				if((watchpoint->GetMemoryType() == mt) && (watchpoint->GetMemoryAccessType() == mat) && watchpoint->HasOverlap(addr))
				{
					if(!RemoveWatchpoint(watchpoint)) return false;
				}
				
				watchpoint_it = next_watchpoint_it;
			}
			
			--size;
			++addr;
		}
		while(size > 0);
	}

	return true;
}

template <typename CONFIG>
bool WatchpointRegistry<CONFIG>::SetWatchpoint(WATCHPOINT *wp)
{
	unisim::util::debug::MemoryAccessType mat = wp->GetMemoryAccessType();
	unisim::util::debug::MemoryType mt = wp->GetMemoryType();
	ADDRESS addr = wp->GetAddress();
	uint32_t size = wp->GetSize();
	unsigned int prc_num = wp->GetProcessorNumber();
	unsigned int front_end_num = wp->GetFrontEndNumber();

	if(prc_num >= NUM_PROCESSORS) return false;
	if(front_end_num >= MAX_FRONT_ENDS) return false;
	
	if(HasWatchpoint(wp)) return true; // that watchpoint is already set
	
	if(!SetWatchpointIntoMap(mat, mt, addr, size, prc_num, front_end_num)) return false;
	
	if(size > 0)
	{
		do
		{
			bool found = false;
			std::pair<typename Watchpoints::const_iterator, typename Watchpoints::const_iterator> range = watchpoints[prc_num][front_end_num].equal_range(addr);

			for(typename Watchpoints::const_iterator watchpoint_it = range.first; watchpoint_it != range.second; ++watchpoint_it)
			{
				WATCHPOINT *watchpoint = (*watchpoint_it).second;
				if(watchpoint == wp)
				{
					found = true;
					break;
				}
			}
			if(!found)
			{
				watchpoints[prc_num][front_end_num].insert(typename Watchpoints::value_type(addr, wp));
				wp->Catch();
			}
			--size;
			++addr;
		}
		while(size > 0);
	}
	
	return true;
}

template <typename CONFIG>
bool WatchpointRegistry<CONFIG>::RemoveWatchpoint(WATCHPOINT *wp)
{
	unisim::util::debug::MemoryAccessType mat = wp->GetMemoryAccessType();
	unisim::util::debug::MemoryType mt = wp->GetMemoryType();
	ADDRESS addr = wp->GetAddress();
	uint32_t size = wp->GetSize();
	unsigned int prc_num = wp->GetProcessorNumber();
	unsigned int front_end_num = wp->GetFrontEndNumber();

	if(prc_num >= NUM_PROCESSORS) return false;
	if(front_end_num >= MAX_FRONT_ENDS) return false;
	
	if(!HasWatchpoint(wp)) return false; // that watchpoint is not set
	
	if(size > 0)
	{
		do
		{
			typename std::pair<typename Watchpoints::iterator, typename Watchpoints::iterator> range = watchpoints[prc_num][front_end_num].equal_range(addr);
			
			typename Watchpoints::const_iterator found_watchpoint_it;
			unsigned int count = 0;
			bool found = false;
			
			for(typename Watchpoints::const_iterator watchpoint_it = range.first; watchpoint_it != range.second; ++watchpoint_it)
			{
				WATCHPOINT *watchpoint = (*watchpoint_it).second;
				if((watchpoint->GetMemoryType() == mt) && (watchpoint->GetMemoryAccessType() == mat) && watchpoint->HasOverlap(addr))
				{
					++count;
				}
				
				if(watchpoint == wp)
				{
					found = true;
					found_watchpoint_it = watchpoint_it;
				}
			}
			
			if(found)
			{
				// invalidate
				wp->Release();
				watchpoints[prc_num][front_end_num].erase(found_watchpoint_it);
				
				// if that watchpoint was the only one set at that address, we update the watchpoint map
				if(count == 1)
				{
					if(!RemoveWatchpointFromMap(mat, mt, addr, /* size= */ 1, prc_num, front_end_num)) return false;
				}
			}
			
			--size;
			++addr;
		}
		while(size > 0);
	}
	
	return true;
}

template <typename CONFIG>
bool WatchpointRegistry<CONFIG>::HasWatchpoint(WATCHPOINT *wp) const
{
	ADDRESS addr = wp->GetAddress();
	unsigned int prc_num = wp->GetProcessorNumber();
	unsigned int front_end_num = wp->GetFrontEndNumber();
	
	if(prc_num >= NUM_PROCESSORS) return false;
	if(front_end_num >= MAX_FRONT_ENDS) return false;

	std::pair<typename Watchpoints::const_iterator, typename Watchpoints::const_iterator> range = watchpoints[prc_num][front_end_num].equal_range(addr);

	if(range.first != range.second)
	{
		// at least one watchpoint event is registered for this address, processor and front-end
		for(typename Watchpoints::const_iterator watchpoint_it = range.first; watchpoint_it != range.second; ++watchpoint_it)
		{
			WATCHPOINT *watchpoint = (*watchpoint_it).second;
			
			if(watchpoint == wp) return true;
		}
	}

	return false;
}

template <typename CONFIG>
bool WatchpointRegistry<CONFIG>::HasWatchpoints(unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size, unsigned int prc_num) const
{
	if(prc_num >= NUM_PROCESSORS) return false;
	if(size > 0)
	{
		do
		{
			uint32_t size_to_page_boundary = WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_WATCHPOINTS_PER_PAGE - (addr & (WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_WATCHPOINTS_PER_PAGE - 1));
			uint32_t sz = size > size_to_page_boundary ? size_to_page_boundary : size;

			WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS> *page = GetPage(mt, addr, prc_num);
			if(!page) return false;
			if(page->HasWatchpoints(mat, addr & (WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_WATCHPOINTS_PER_PAGE - 1), sz)) return true;

			size -= sz;
			addr += sz;
		} while(size > 0);
	}
	return false;
}

template <typename CONFIG>
bool WatchpointRegistry<CONFIG>::HasWatchpoints(unsigned int prc_num) const
{
	return watchpoint_count[prc_num] != 0;
}

template <typename CONFIG>
bool WatchpointRegistry<CONFIG>::HasWatchpoints() const
{
	unsigned int prc_num;
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; ++prc_num)
	{
		if(watchpoint_count[prc_num] != 0) return true;
	}
	
	return false;
}

/* struct Visitor { void Visit(WATCHPOINT *) {} }; */
template <typename CONFIG>
template <class VISITOR>
bool WatchpointRegistry<CONFIG>::FindWatchpoints(unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size, unsigned int prc_num, unsigned int front_end_num, VISITOR& visitor) const
{
	if(!HasWatchpoints(mat, mt, addr, size, prc_num, front_end_num)) return false;
	
	bool found = false;
	
	if(size > 0)
	{
		// beware of reentrancy
		
		typedef std::set<WATCHPOINT *> FoundWatchpoints;
		FoundWatchpoints found_watchpoints;
		
		ADDRESS _addr = addr;
		uint32_t _size = size;
		do
		{
			std::pair<typename Watchpoints::const_iterator, typename Watchpoints::const_iterator> range = watchpoints[prc_num][front_end_num].equal_range(_addr);
	
			if(range.first != range.second)
			{
				// at least one watchpoint event is registered for this address, processor and front-end
				for(typename Watchpoints::const_iterator watchpoint_it = range.first; watchpoint_it != range.second; ++watchpoint_it)
				{
					found = true;
					WATCHPOINT *watchpoint = (*watchpoint_it).second;
					
					if((watchpoint->GetMemoryAccessType() == mat) && (watchpoint->GetMemoryType() == mt))
					{
						watchpoint->Catch();
						found_watchpoints.insert(watchpoint);
					}
				}
			}
			--_size;
			++_addr;
		}
		while(_size > 0);
		
		for(typename FoundWatchpoints::const_iterator found_watchpoint_it = found_watchpoints.begin(); found_watchpoint_it != found_watchpoints.end(); ++found_watchpoint_it)
		{
			WATCHPOINT *watchpoint = *found_watchpoint_it;
			visitor.Visit(watchpoint);
			watchpoint->Release();
		}
	}

	return found;
}

template <typename CONFIG>
template <typename SCANNER>
void WatchpointRegistry<CONFIG>::ScanWatchpoints(unsigned int prc_num, unsigned int front_end_num, SCANNER& scanner) const
{
	// beware of reentrancy
	typedef std::set<WATCHPOINT *> FoundWatchpoints;
	FoundWatchpoints found_watchpoints;
	
	for(typename Watchpoints::const_iterator watchpoint_it = watchpoints[prc_num][front_end_num].begin(); watchpoint_it != watchpoints[prc_num][front_end_num].end(); ++watchpoint_it)
	{
		WATCHPOINT *watchpoint = (*watchpoint_it).second;
		found_watchpoints.insert(watchpoint);
	}
	
	for(typename FoundWatchpoints::const_iterator found_watchpoint_it = found_watchpoints.begin(); found_watchpoint_it != found_watchpoints.end(); ++found_watchpoint_it)
	{
		WATCHPOINT *watchpoint = *found_watchpoint_it;
		scanner.Append(watchpoint);
	}
}

template <typename CONFIG>
template <typename SCANNER>
void WatchpointRegistry<CONFIG>::ScanWatchpoints(unsigned int front_end_num, SCANNER& scanner) const
{
	unsigned int prc_num;
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; ++prc_num)
	{
		ScanWatchpoints(prc_num, front_end_num, scanner);
	}
}

template <typename CONFIG>
template <typename SCANNER>
void WatchpointRegistry<CONFIG>::ScanWatchpoints(SCANNER& scanner) const
{
	unsigned int front_end_num;
	
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; ++front_end_num)
	{
		ScanWatchpoints(front_end_num, scanner);
	}
}

template <typename CONFIG>
bool WatchpointRegistry<CONFIG>::HasWatchpoints(unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size, unsigned int prc_num, unsigned int front_end_num) const
{
	if(prc_num >= NUM_PROCESSORS) return false;
	if(front_end_num >= MAX_FRONT_ENDS) return false;
	if(!HasWatchpoints(prc_num)) return false;
	if(size > 0)
	{
		do
		{
			uint32_t size_to_page_boundary = WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_WATCHPOINTS_PER_PAGE - (addr & (WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_WATCHPOINTS_PER_PAGE - 1));
			uint32_t sz = size > size_to_page_boundary ? size_to_page_boundary : size;

			const WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS> *page = GetPage(mt, addr, prc_num);
			if(!page) return false;
			if(page->HasWatchpoint(mat, addr & (WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_WATCHPOINTS_PER_PAGE - 1), sz, front_end_num)) return true;

			size -= sz;
			addr += sz;
		} while(size > 0);
	}
	return false;
}

template <typename CONFIG>
void WatchpointRegistry<CONFIG>::AllocatePage(unisim::util::debug::MemoryType mt, ADDRESS addr, unsigned int prc_num)
{
	if(prc_num >= NUM_PROCESSORS) return;
	ADDRESS base_addr = addr & ~(WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_WATCHPOINTS_PER_PAGE - 1);
	if(mru_page[mt][prc_num] && (mru_page[mt][prc_num]->base_addr == base_addr)) return;
	WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS> *page;
	uint32_t index = (base_addr / WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_WATCHPOINTS_PER_PAGE) & (NUM_HASH_TABLE_ENTRIES - 1);
	page = hash_table[mt][prc_num][index];
	if(page)
	{
		do
		{
			if(page->base_addr == base_addr)
			{
				mru_page[mt][prc_num] = page;
				return;
			}
		} while((page = page->next) != 0);
	}
	
	/* page not found: we must allocate a new one */
	
	page = new WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS>(base_addr);
	page->next = hash_table[mt][prc_num][index];
	hash_table[mt][prc_num][index] = page;
	mru_page[mt][prc_num] = page;
}

template <typename CONFIG>
WatchpointMapPage<typename CONFIG::ADDRESS, CONFIG::MAX_FRONT_ENDS> *WatchpointRegistry<CONFIG>::GetPage(unisim::util::debug::MemoryType mt, ADDRESS addr, unsigned int prc_num) const
{
	if(prc_num >= NUM_PROCESSORS) return 0;
	ADDRESS base_addr = addr & ~(WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_WATCHPOINTS_PER_PAGE - 1);
	if(mru_page[mt][prc_num] && (mru_page[mt][prc_num]->base_addr == base_addr)) return mru_page[mt][prc_num];
	WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS> *prev, *page;
	uint32_t index = (base_addr / WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_WATCHPOINTS_PER_PAGE) & (NUM_HASH_TABLE_ENTRIES - 1);
	page = hash_table[mt][prc_num][index];
	if(page)
	{
		if(page->base_addr == base_addr)
		{
			mru_page[mt][prc_num] = page;
			return page;
		}
		prev = page;
		page = page->next;
		if(page)
		{
			do
			{
				if(page->base_addr == base_addr)
				{
					prev->next = page->next;
					page->next= hash_table[mt][prc_num][index];
					hash_table[mt][prc_num][index] = page;
					mru_page[mt][prc_num] = page;
					return page;
				}
				prev = page;
			} while((page = page->next) != 0);
		}
	}
	return 0;
}


} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
