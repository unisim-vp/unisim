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
 
#ifndef __UNISIM_UTIL_DEBUG_BREAKPOINT_REGISTRY_TCC__
#define __UNISIM_UTIL_DEBUG_BREAKPOINT_REGISTRY_TCC__

#include <iostream>
#include <string.h>

namespace unisim {
namespace util {
namespace debug {

template <typename ADDRESS, unsigned int MAX_FRONT_ENDS>
BreakpointMapPage<ADDRESS, MAX_FRONT_ENDS>::BreakpointMapPage(ADDRESS addr)
{
	this->base_addr = addr & ~(NUM_BREAKPOINTS_PER_PAGE - 1);
	map = new WORD[(NUM_BREAKPOINTS_PER_PAGE + BREAKPOINTS_PER_WORD - 1)/ BREAKPOINTS_PER_WORD];
	memset(map, 0, ((NUM_BREAKPOINTS_PER_PAGE + BREAKPOINTS_PER_WORD - 1)/ BREAKPOINTS_PER_WORD) * sizeof(WORD));
	next = 0;
}

template <typename ADDRESS, unsigned int MAX_FRONT_ENDS>
BreakpointMapPage<ADDRESS, MAX_FRONT_ENDS>::~BreakpointMapPage()
{
	delete[] map;
}

template <typename ADDRESS, unsigned int MAX_FRONT_ENDS>
unsigned int BreakpointMapPage<ADDRESS, MAX_FRONT_ENDS>::SetBreakpoint(uint32_t offset, unsigned int front_end_num)
{
	unsigned int breakpoint_count_increment = HasBreakpoint(offset, front_end_num) ? 0 : 1;
	WORD mask = 1ULL << front_end_num;
	map[offset / BREAKPOINTS_PER_WORD] |= mask << (offset % BREAKPOINTS_PER_WORD);
	return breakpoint_count_increment;
}

template <typename ADDRESS, unsigned int MAX_FRONT_ENDS>
unsigned int BreakpointMapPage<ADDRESS, MAX_FRONT_ENDS>::RemoveBreakpoint(uint32_t offset, unsigned int front_end_num)
{
	unsigned int breakpoint_count_decrement = HasBreakpoint(offset, front_end_num) ? 1 : 0;
	WORD mask = 1ULL << front_end_num;
	map[offset / BREAKPOINTS_PER_WORD] &= ~(mask << (offset % BREAKPOINTS_PER_WORD));
	return breakpoint_count_decrement;
}

template <typename ADDRESS, unsigned int MAX_FRONT_ENDS>
bool BreakpointMapPage<ADDRESS, MAX_FRONT_ENDS>::HasBreakpoint(uint32_t offset, unsigned int front_end_num) const
{
	WORD mask = 1ULL << front_end_num;
	return (map[offset / BREAKPOINTS_PER_WORD] & (mask << (offset % BREAKPOINTS_PER_WORD))) ? true : false;
}

template <typename ADDRESS, unsigned int MAX_FRONT_ENDS>
bool BreakpointMapPage<ADDRESS, MAX_FRONT_ENDS>::HasBreakpoints(uint32_t offset) const
{
	WORD mask = 0;
	
	unsigned int front_end_num;
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; ++front_end_num)
	{
		mask |= 1ULL << front_end_num;
	}
	
	return (map[offset / BREAKPOINTS_PER_WORD] & (mask << (offset % BREAKPOINTS_PER_WORD))) ? true : false;
}

template <typename CONFIG>
BreakpointRegistry<CONFIG>::BreakpointRegistry()
	: breakpoints()
	, breakpoint_count()
	, mru_page()
	, hash_table()
{
}

template <typename CONFIG>
BreakpointRegistry<CONFIG>::~BreakpointRegistry()
{
	Reset();
}

template <typename CONFIG>
void BreakpointRegistry<CONFIG>::Reset()
{
	unsigned int prc_num;
	unsigned int front_end_num;
	for(prc_num = 0; prc_num < NUM_PROCESSORS; ++prc_num)
	{
		uint32_t index;
		for(index = 0; index < NUM_HASH_TABLE_ENTRIES; ++index)
		{
			BreakpointMapPage<ADDRESS, MAX_FRONT_ENDS> *page, *nextpage;
			
			page = hash_table[prc_num][index];
			if(page)
			{
				do
				{
					nextpage = page->next;
					delete page;
					page = nextpage;
				} while(page);
				hash_table[prc_num][index] = 0;
			}
		}
			
		mru_page[prc_num] = 0;
		
		breakpoint_count[prc_num] = 0;
		
		for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; ++front_end_num)
		{
			for(typename Breakpoints::iterator breakpoint_it = breakpoints[prc_num][front_end_num].begin(); breakpoint_it != breakpoints[prc_num][front_end_num].end(); ++breakpoint_it)
			{
				BREAKPOINT *breakpoint = (*breakpoint_it).second;
				breakpoint->Release();
			}
			
			breakpoints[prc_num][front_end_num].clear();
		}
	}
}

template <typename CONFIG>
void BreakpointRegistry<CONFIG>::Clear(unsigned int front_end_num)
{
	unsigned int prc_num;
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; ++prc_num)
	{
		for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; ++front_end_num)
		{
			typename Breakpoints::iterator breakpoint_it = breakpoints[prc_num][front_end_num].begin();
			
			do
			{
				BREAKPOINT *breakpoint = (*breakpoint_it).second;
				if(!RemoveBreakpoint(breakpoint))
				{
					std::cerr << "breakpoint was not removed" << std::endl;
				}
				breakpoint_it = breakpoints[prc_num][front_end_num].begin();
			}
			while(breakpoint_it != breakpoints[prc_num][front_end_num].end());
		}
	}
}

template <typename CONFIG>
bool BreakpointRegistry<CONFIG>::SetBreakpointIntoMap(ADDRESS addr, unsigned int prc_num, unsigned int front_end_num)
{
	if(prc_num >= NUM_PROCESSORS) return false;
	if(front_end_num >= MAX_FRONT_ENDS) return false;
	
	AllocatePage(addr, prc_num);
	BreakpointMapPage<ADDRESS, MAX_FRONT_ENDS> *page = GetPage(addr, prc_num);
	if(!page)
	{
		std::cerr << "Internal error: breakpoint page has not been allocated" << std::endl;
		return false;
	}
	breakpoint_count[prc_num] += page->SetBreakpoint(addr & (BreakpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_BREAKPOINTS_PER_PAGE - 1), front_end_num);
	
	return true;
}

template <typename CONFIG>
bool BreakpointRegistry<CONFIG>::RemoveBreakpointFromMap(ADDRESS addr, unsigned int prc_num, unsigned int front_end_num)
{
	if(prc_num >= NUM_PROCESSORS) return false;
	if(front_end_num >= MAX_FRONT_ENDS) return false;

	BreakpointMapPage<ADDRESS, MAX_FRONT_ENDS> *page = GetPage(addr, prc_num);
	if(!page) return false;
	breakpoint_count[prc_num] -= page->RemoveBreakpoint(addr & (BreakpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_BREAKPOINTS_PER_PAGE - 1), front_end_num);

	return true;
}

template <typename CONFIG>
typename CONFIG::BREAKPOINT *BreakpointRegistry<CONFIG>::SetBreakpoint(ADDRESS addr, unsigned int prc_num, unsigned int front_end_num)
{
	if(prc_num >= NUM_PROCESSORS) return 0;
	if(front_end_num >= MAX_FRONT_ENDS) return 0;
	
	if(!SetBreakpointIntoMap(addr, prc_num, front_end_num)) return 0;
	
	typename Breakpoints::iterator breakpoint_it = breakpoints[prc_num][front_end_num].find(addr);
	
	BREAKPOINT *breakpoint = (breakpoint_it != breakpoints[prc_num][front_end_num].end()) ? (*breakpoint_it).second : 0;
	
	if(!breakpoint)
	{
		breakpoint = new BREAKPOINT(prc_num, front_end_num, addr);
		breakpoint->Catch();
		breakpoints[prc_num][front_end_num].insert(typename Breakpoints::value_type(addr, breakpoint));
	}
	
	return breakpoint;
}

template <typename CONFIG>
bool BreakpointRegistry<CONFIG>::RemoveBreakpoint(ADDRESS addr, unsigned int prc_num, unsigned int front_end_num)
{
	if(prc_num >= NUM_PROCESSORS) return false;
	if(front_end_num >= MAX_FRONT_ENDS) return false;

	// invalidate breakpoint events that match the address
	typename std::pair<typename Breakpoints::iterator, typename Breakpoints::iterator> range = breakpoints[prc_num][front_end_num].equal_range(addr);
	
	if(range.first != range.second)
	{
		// at least one breakpoint event is registered for this address, processor and front-end
		typename Breakpoints::iterator breakpoint_it = range.first;
		
		while(breakpoint_it != range.second)
		{
			typename Breakpoints::iterator next_breakpoint_it = breakpoint_it;
			++next_breakpoint_it;
			
			BREAKPOINT *breakpoint = (*breakpoint_it).second;

			// invalidate
			breakpoint->Release();
			breakpoints[prc_num][front_end_num].erase(breakpoint_it);
			
			breakpoint_it = next_breakpoint_it;
		}
	}

	return RemoveBreakpointFromMap(addr, prc_num, front_end_num);
}

template <typename CONFIG>
bool BreakpointRegistry<CONFIG>::SetBreakpoint(BREAKPOINT *brkp)
{
	ADDRESS addr = brkp->GetAddress();
	unsigned int prc_num = brkp->GetProcessorNumber();
	unsigned int front_end_num = brkp->GetFrontEndNumber();
	
	if(prc_num >= NUM_PROCESSORS) return false;
	if(front_end_num >= MAX_FRONT_ENDS) return false;

	if(HasBreakpoint(brkp)) return true; // that breakpoint is already set

	if(!SetBreakpointIntoMap(addr, prc_num, front_end_num)) return false;

	breakpoints[prc_num][front_end_num].insert(typename Breakpoints::value_type(addr, brkp));
	brkp->Catch();
	
	return true;
}

template <typename CONFIG>
bool BreakpointRegistry<CONFIG>::RemoveBreakpoint(BREAKPOINT *brkp)
{
	ADDRESS addr = brkp->GetAddress();
	unsigned int prc_num = brkp->GetProcessorNumber();
	unsigned int front_end_num = brkp->GetFrontEndNumber();
	
	if(prc_num >= NUM_PROCESSORS) return false;
	if(front_end_num >= MAX_FRONT_ENDS) return false;

	// search breakpoint events that match the address
	typename std::pair<typename Breakpoints::iterator, typename Breakpoints::iterator> range = breakpoints[prc_num][front_end_num].equal_range(addr);
	
	typename Breakpoints::iterator found_breakpoint_it;
	unsigned int count = 0;
	bool found = false;
	
	for(typename Breakpoints::iterator breakpoint_it = range.first; breakpoint_it != range.second; ++breakpoint_it)
	{
		++count;
		BREAKPOINT *breakpoint = (*breakpoint_it).second;
		
		if(breakpoint == brkp)
		{
			found = true;
			found_breakpoint_it = breakpoint_it;
		}
	}
	
	if(!found) return false; // not found !
	
	// invalidate
	brkp->Release();
	breakpoints[prc_num][front_end_num].erase(found_breakpoint_it);
	
	// if that breakpoint was the only one set at that address, we update the breakpoint map
	return (count > 1) || RemoveBreakpointFromMap(addr, prc_num, front_end_num);
}

template <typename CONFIG>
bool BreakpointRegistry<CONFIG>::HasBreakpoint(BREAKPOINT *brkp) const
{
	ADDRESS addr = brkp->GetAddress();
	unsigned int prc_num = brkp->GetProcessorNumber();
	unsigned int front_end_num = brkp->GetFrontEndNumber();
	
	if(prc_num >= NUM_PROCESSORS) return false;
	if(front_end_num >= MAX_FRONT_ENDS) return false;
	
	typename std::pair<typename Breakpoints::const_iterator, typename Breakpoints::const_iterator> range = breakpoints[prc_num][front_end_num].equal_range(addr);
	
	if(range.first != range.second)
	{
		// at least one breakpoint event is registered for this address, processor and front-end
		typename Breakpoints::const_iterator breakpoint_it;
		
		for(breakpoint_it = range.first; breakpoint_it != range.second; ++breakpoint_it)
		{
			BREAKPOINT *breakpoint = (*breakpoint_it).second;
			
			if(breakpoint == brkp) return true;
		}
	}
	
	return false;
}

template <typename CONFIG>
bool BreakpointRegistry<CONFIG>::HasBreakpoints(ADDRESS addr, unsigned int prc_num) const
{
	if(prc_num >= NUM_PROCESSORS) return false;
	BreakpointMapPage<ADDRESS, MAX_FRONT_ENDS> *page = GetPage(addr, prc_num);
	if(!page) return false;
	return page->HasBreakpoints(addr & (BreakpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_BREAKPOINTS_PER_PAGE - 1));
}

template <typename CONFIG>
bool BreakpointRegistry<CONFIG>::HasBreakpoints(unsigned int prc_num) const
{
	return breakpoint_count[prc_num] != 0;
}

template <typename CONFIG>
bool BreakpointRegistry<CONFIG>::HasBreakpoints() const
{
	unsigned int prc_num;
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; ++prc_num)
	{
		if(breakpoint_count[prc_num] != 0) return true;
	}
	
	return false;
}

template <typename CONFIG>
template <typename SCANNER>
void BreakpointRegistry<CONFIG>::ScanBreakpoints(unsigned int prc_num, unsigned int front_end_num, SCANNER& scanner) const
{
	// beware of reentrancy
	typename Breakpoints::size_type i = 0, n = breakpoints[prc_num][front_end_num].size();
	BREAKPOINT *found_breakpoints[n];
	for(typename Breakpoints::const_iterator breakpoint_it = breakpoints[prc_num][front_end_num].begin(); breakpoint_it != breakpoints[prc_num][front_end_num].end(); ++i, ++breakpoint_it)
	{
		BREAKPOINT *breakpoint = (*breakpoint_it).second;
		breakpoint->Catch();
		found_breakpoints[i] = breakpoint;
	}
	for(i = 0; i < n; ++i)
	{
		BREAKPOINT *breakpoint = found_breakpoints[i];
		scanner.Append(breakpoint);
		breakpoint->Release();
	}
}

template <typename CONFIG>
template <typename SCANNER>
void BreakpointRegistry<CONFIG>::ScanBreakpoints(unsigned int front_end_num, SCANNER& scanner) const
{
	unsigned int prc_num;
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; ++prc_num)
	{
		ScanBreakpoints(prc_num, front_end_num, scanner);
	}
}

template <typename CONFIG>
template <typename SCANNER>
void BreakpointRegistry<CONFIG>::ScanBreakpoints(SCANNER& scanner) const
{
	unsigned int front_end_num;
	
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; ++front_end_num)
	{
		ScanBreakpoints(front_end_num, scanner);
	}
}

template <typename CONFIG>
template <class VISITOR>
bool BreakpointRegistry<CONFIG>::FindBreakpoints(ADDRESS addr, unsigned int prc_num, unsigned int front_end_num, VISITOR& visitor)
{
	if(!HasBreakpoints(addr, prc_num, front_end_num)) return false;
	
	typename std::pair<typename Breakpoints::iterator, typename Breakpoints::iterator> range = breakpoints[prc_num][front_end_num].equal_range(addr);
	
	if(range.first != range.second)
	{
		// at least one breakpoint event is registered for this address, processor and front-end
		
		// beware of reentrancy
		typename Breakpoints::size_type i = 0, n = 0;
		for(typename Breakpoints::iterator breakpoint_it = range.first; breakpoint_it != range.second; ++n, ++breakpoint_it);
		BREAKPOINT *found_breakpoints[n];
		for(typename Breakpoints::iterator breakpoint_it = range.first; breakpoint_it != range.second; ++i, ++breakpoint_it)
		{
			BREAKPOINT *breakpoint = (*breakpoint_it).second;
			breakpoint->Catch();
			found_breakpoints[i] = breakpoint;
		}
		for(i = 0; i < n; ++i)
		{
			BREAKPOINT *breakpoint = found_breakpoints[i];
			visitor.Visit(breakpoint);
			breakpoint->Release();
		}
		
		return true;
	}

	return false;
}

template <typename CONFIG>
bool BreakpointRegistry<CONFIG>::HasBreakpoints(ADDRESS addr, unsigned int prc_num, unsigned int front_end_num) const
{
	if(prc_num >= NUM_PROCESSORS) return false;
	if(front_end_num >= MAX_FRONT_ENDS) return false;
	if(!HasBreakpoints(prc_num)) return false;
	const BreakpointMapPage<ADDRESS, MAX_FRONT_ENDS> *page = GetPage(addr, prc_num);
	if(!page) return false;
	return page->HasBreakpoint(addr & (BreakpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_BREAKPOINTS_PER_PAGE - 1), front_end_num);
}

template <typename CONFIG>
void BreakpointRegistry<CONFIG>::AllocatePage(ADDRESS addr, unsigned int prc_num)
{
	if(prc_num >= NUM_PROCESSORS) return;
	ADDRESS base_addr = addr & ~(BreakpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_BREAKPOINTS_PER_PAGE - 1);
	if(mru_page[prc_num] && (mru_page[prc_num]->base_addr == base_addr)) return;
	BreakpointMapPage<ADDRESS, MAX_FRONT_ENDS> *page;
	uint32_t index = (base_addr / BreakpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_BREAKPOINTS_PER_PAGE) & (NUM_HASH_TABLE_ENTRIES - 1);
	page = hash_table[prc_num][index];
	if(page)
	{
		do
		{
			if(page->base_addr == base_addr)
			{
				mru_page[prc_num] = page;
				return;
			}
		} while((page = page->next) != 0);
	}
	
	/* page not found: we must allocate a new one */
	
	page = new BreakpointMapPage<ADDRESS, MAX_FRONT_ENDS>(base_addr);
	page->next = hash_table[prc_num][index];
	hash_table[prc_num][index] = page;
	mru_page[prc_num] = page;
}

template <typename CONFIG>
BreakpointMapPage<typename CONFIG::ADDRESS, CONFIG::MAX_FRONT_ENDS> *BreakpointRegistry<CONFIG>::GetPage(ADDRESS addr, unsigned int prc_num) const
{
	if(prc_num >= NUM_PROCESSORS) return 0;
	ADDRESS base_addr = addr & ~(BreakpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_BREAKPOINTS_PER_PAGE - 1);
	if(mru_page[prc_num] && (mru_page[prc_num]->base_addr == base_addr)) return mru_page[prc_num];
	BreakpointMapPage<ADDRESS, MAX_FRONT_ENDS> *prev, *page;
	uint32_t index = (base_addr / BreakpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_BREAKPOINTS_PER_PAGE) & (NUM_HASH_TABLE_ENTRIES - 1);
	page = hash_table[prc_num][index];
	if(page)
	{
		if(page->base_addr == base_addr)
		{
			mru_page[prc_num] = page;
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
					page->next= hash_table[prc_num][index];
					hash_table[prc_num][index] = page;
					mru_page[prc_num] = page;
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
