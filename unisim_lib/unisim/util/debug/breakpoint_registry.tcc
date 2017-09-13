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
 *          Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#ifndef __UNISIM_UTIL_DEBUG_BREAKPOINT_REGISTRY_TCC__
#define __UNISIM_UTIL_DEBUG_BREAKPOINT_REGISTRY_TCC__

#include <iostream>
#include <string.h>

namespace unisim {
namespace util {
namespace debug {

using std::cerr;
using std::endl;

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
void BreakpointMapPage<ADDRESS, MAX_FRONT_ENDS>::SetBreakpoint(uint32_t offset, unsigned int front_end_num)
{
	WORD mask = 1ULL << front_end_num;
	map[offset / BREAKPOINTS_PER_WORD] |= mask << (offset % BREAKPOINTS_PER_WORD);
}

template <typename ADDRESS, unsigned int MAX_FRONT_ENDS>
void BreakpointMapPage<ADDRESS, MAX_FRONT_ENDS>::RemoveBreakpoint(uint32_t offset, unsigned int front_end_num)
{
	WORD mask = 1ULL << front_end_num;
	map[offset / BREAKPOINTS_PER_WORD] &= ~(mask << (offset % BREAKPOINTS_PER_WORD));
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
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
	{
		mask |= 1ULL << front_end_num;
	}
	
	return (map[offset / BREAKPOINTS_PER_WORD] & (mask << (offset % BREAKPOINTS_PER_WORD))) ? true : false;
}

template <typename ADDRESS, unsigned int NUM_PROCESSORS, unsigned int MAX_FRONT_ENDS>
BreakpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS>::BreakpointRegistry()
	: has_breakpoints()
	, breakpoints()
	, mru_page()
	, hash_table()
{
}

template <typename ADDRESS, unsigned int NUM_PROCESSORS, unsigned int MAX_FRONT_ENDS>
BreakpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS>::~BreakpointRegistry()
{
	Reset();
}

template <typename ADDRESS, unsigned int NUM_PROCESSORS, unsigned int MAX_FRONT_ENDS>
void BreakpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS>::Reset()
{
	unsigned int prc_num;
	unsigned int front_end_num;
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		uint32_t index;
		for(index = 0; index < NUM_HASH_TABLE_ENTRIES; index++)
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
		
		for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
		{
			typename std::list<Breakpoint<ADDRESS> *>::const_iterator breakpoint_it;

			for(breakpoint_it = breakpoints[prc_num][front_end_num].begin(); breakpoint_it != breakpoints[prc_num][front_end_num].end(); breakpoint_it++)
			{
				Breakpoint<ADDRESS> *breakpoint = *breakpoint_it;
				if(breakpoint)
				{
					delete breakpoint;
				}
			}

			has_breakpoints[prc_num][front_end_num] = false;
		}
	}
}

template <typename ADDRESS, unsigned int NUM_PROCESSORS, unsigned int MAX_FRONT_ENDS>
bool BreakpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS>::SetBreakpoint(Breakpoint<ADDRESS> *brkp)
{
	ADDRESS addr = brkp->GetAddress();
	unsigned int prc_num = brkp->GetProcessorNumber();
	unsigned int front_end_num = brkp->GetFrontEndNumber();
	
	if(prc_num >= NUM_PROCESSORS) return false;
	if(front_end_num >= MAX_FRONT_ENDS) return false;
	
	if(!HasBreakpoint(addr, prc_num, front_end_num))
	{
		AllocatePage(addr, prc_num);
		BreakpointMapPage<ADDRESS, MAX_FRONT_ENDS> *page = GetPage(addr, prc_num);
		if(!page)
		{
			cerr << "Internal error: breakpoint page has not been allocated" << endl;
			return false;
		}
		page->SetBreakpoint(addr & (BreakpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_BREAKPOINTS_PER_PAGE - 1), front_end_num);
		breakpoints[prc_num][front_end_num].push_back(brkp);
		brkp->Catch();
		has_breakpoints[prc_num][front_end_num] = true;
		return true;
	}
	return false;
}

template <typename ADDRESS, unsigned int NUM_PROCESSORS, unsigned int MAX_FRONT_ENDS>
bool BreakpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS>::RemoveBreakpoint(Breakpoint<ADDRESS> *brkp)
{
	ADDRESS addr = brkp->GetAddress();
	unsigned int prc_num = brkp->GetProcessorNumber();
	unsigned int front_end_num = brkp->GetFrontEndNumber();
	
	if(prc_num >= NUM_PROCESSORS) return false;
	if(front_end_num >= MAX_FRONT_ENDS) return false;

	BreakpointMapPage<ADDRESS, MAX_FRONT_ENDS> *page = GetPage(addr, prc_num);
	if(!page) return false;
	page->RemoveBreakpoint(addr & (BreakpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_BREAKPOINTS_PER_PAGE - 1), front_end_num);

	typename std::list<Breakpoint<ADDRESS> *>::iterator breakpoint_it;

	for(breakpoint_it = breakpoints[prc_num][front_end_num].begin(); breakpoint_it != breakpoints[prc_num][front_end_num].end(); breakpoint_it++)
	{
		Breakpoint<ADDRESS> *breakpoint = *breakpoint_it;
		if(breakpoint->GetAddress() == addr)
		{
			breakpoint->Release();
			breakpoints[prc_num][front_end_num].erase(breakpoint_it);
			if(breakpoints[prc_num][front_end_num].empty())
				has_breakpoints[prc_num][front_end_num] = false;
			return true;
		}
	}

	return false;
}

template <typename ADDRESS, unsigned int NUM_PROCESSORS, unsigned int MAX_FRONT_ENDS>
bool BreakpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS>::HasBreakpoint(Breakpoint<ADDRESS> *brkp) const
{
	ADDRESS addr = brkp->GetAddress();
	unsigned int prc_num = brkp->GetProcessorNumber();
	unsigned int front_end_num = brkp->GetFrontEndNumber();
	
	if(prc_num >= NUM_PROCESSORS) return false;
	if(front_end_num >= MAX_FRONT_ENDS) return false;
	
	return HasBreakpoint(addr, prc_num, front_end_num);
}

template <typename ADDRESS, unsigned int NUM_PROCESSORS, unsigned int MAX_FRONT_ENDS>
bool BreakpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS>::HasBreakpoints(ADDRESS addr, unsigned int prc_num) const
{
	if(prc_num >= NUM_PROCESSORS) return false;
	BreakpointMapPage<ADDRESS, MAX_FRONT_ENDS> *page = GetPage(addr, prc_num);
	if(!page) return false;
	return page->HasBreakpoints(addr & (BreakpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_BREAKPOINTS_PER_PAGE - 1));
}

template <typename ADDRESS, unsigned int NUM_PROCESSORS, unsigned int MAX_FRONT_ENDS>
bool BreakpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS>::HasBreakpoints(unsigned int prc_num) const
{
	unsigned int front_end_num;
	
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
	{
		if(has_breakpoints[prc_num][front_end_num]) return true;
	}
	
	return false;
}

template <typename ADDRESS, unsigned int NUM_PROCESSORS, unsigned int MAX_FRONT_ENDS>
bool BreakpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS>::HasBreakpoints() const
{
	unsigned int prc_num;
	unsigned int front_end_num;
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
		{
			if(has_breakpoints[prc_num][front_end_num]) return true;
		}
	}
	
	return false;
}

template <typename ADDRESS, unsigned int NUM_PROCESSORS, unsigned int MAX_FRONT_ENDS>
Breakpoint<ADDRESS> *BreakpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS>::FindBreakpoint(ADDRESS addr, unsigned int prc_num, unsigned int front_end_num) const
{
	if(HasBreakpoint(addr, prc_num, front_end_num))
	{
		typename std::list<Breakpoint<ADDRESS> *>::const_iterator breakpoint_it;

		for(breakpoint_it = breakpoints[prc_num][front_end_num].begin(); breakpoint_it != breakpoints[prc_num][front_end_num].end(); breakpoint_it++)
		{
			Breakpoint<ADDRESS> *breakpoint = *breakpoint_it;
			if(breakpoint->GetAddress() == addr) return breakpoint;
		}
	}
	return 0;
}

template <typename ADDRESS, unsigned int NUM_PROCESSORS, unsigned int MAX_FRONT_ENDS>
void BreakpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS>::EnumerateBreakpoints(unsigned int prc_num, unsigned int front_end_num, std::list<Event<ADDRESS> *>& lst) const
{
	typename std::list<Breakpoint<ADDRESS> *>::const_iterator breakpoint_it;
	
	for(breakpoint_it = breakpoints[prc_num][front_end_num].begin(); breakpoint_it != breakpoints[prc_num][front_end_num].end(); breakpoint_it++)
	{
		Breakpoint<ADDRESS> *breakpoint = *breakpoint_it;
		
		lst.push_back(breakpoint);
	}
}

template <typename ADDRESS, unsigned int NUM_PROCESSORS, unsigned int MAX_FRONT_ENDS>
void BreakpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS>::EnumerateBreakpoints(unsigned int front_end_num, std::list<Event<ADDRESS> *>& lst) const
{
	unsigned int prc_num;
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		EnumerateBreakpoints(prc_num, front_end_num, lst);
	}
}

template <typename ADDRESS, unsigned int NUM_PROCESSORS, unsigned int MAX_FRONT_ENDS>
void BreakpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS>::EnumerateBreakpoints(std::list<Event<ADDRESS> *>& lst) const
{
	unsigned int front_end_num;
	
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
	{
		EnumerateBreakpoints(front_end_num, lst);
	}
}

template <typename ADDRESS, unsigned int NUM_PROCESSORS, unsigned int MAX_FRONT_ENDS>
bool BreakpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS>::HasBreakpoint(ADDRESS addr, unsigned int prc_num, unsigned int front_end_num) const
{
	if(prc_num >= NUM_PROCESSORS) return false;
	if(front_end_num >= MAX_FRONT_ENDS) return false;
	if(!has_breakpoints[prc_num][front_end_num]) return false;
	const BreakpointMapPage<ADDRESS, MAX_FRONT_ENDS> *page = GetPage(addr, prc_num);
	if(!page) return false;
	return page->HasBreakpoint(addr & (BreakpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_BREAKPOINTS_PER_PAGE - 1), front_end_num);
}

template <typename ADDRESS, unsigned int NUM_PROCESSORS, unsigned int MAX_FRONT_ENDS>
void BreakpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS>::AllocatePage(ADDRESS addr, unsigned int prc_num)
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

template <typename ADDRESS, unsigned int NUM_PROCESSORS, unsigned int MAX_FRONT_ENDS>
BreakpointMapPage<ADDRESS, MAX_FRONT_ENDS> *BreakpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS>::GetPage(ADDRESS addr, unsigned int prc_num) const
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
