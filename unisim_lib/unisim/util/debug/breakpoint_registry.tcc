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
 
#ifndef __UNISIM_UTIL_DEBUG_BREAKPOINT_REGISTRY_TCC__
#define __UNISIM_UTIL_DEBUG_BREAKPOINT_REGISTRY_TCC__

#include <iostream>

namespace unisim {
namespace util {
namespace debug {

using std::cerr;
using std::endl;

template <class ADDRESS>
BreakpointMapPage<ADDRESS>::BreakpointMapPage(ADDRESS addr)
{
	this->base_addr = addr & ~(NUM_BREAKPOINTS_PER_PAGE - 1);
	map = new uint32_t[NUM_BREAKPOINTS_PER_PAGE / 32];
	memset(map, 0, NUM_BREAKPOINTS_PER_PAGE / 8);
	next = 0;
}

template <class ADDRESS>
BreakpointMapPage<ADDRESS>::~BreakpointMapPage()
{
	delete[] map;
}

template <class ADDRESS>
void BreakpointMapPage<ADDRESS>::SetBreakpoint(uint32_t offset)
{
  map[offset / 32] |= 1 << (offset % 32);
}

template <class ADDRESS>
void BreakpointMapPage<ADDRESS>::RemoveBreakpoint(uint32_t offset)
{
  map[offset / 32] &= ~(1 << (offset % 32));
}

template <class ADDRESS>
bool BreakpointMapPage<ADDRESS>::HasBreakpoint(uint32_t offset)
{
  return (map[offset / 32] & (1 << (offset % 32))) ? true : false;
}


template <class ADDRESS>
BreakpointRegistry<ADDRESS>::BreakpointRegistry()
{
	memset(hash_table, 0, sizeof(hash_table));
}

template <class ADDRESS>
BreakpointRegistry<ADDRESS>::~BreakpointRegistry()
{
	uint32_t index;
	for(index = 0; index < NUM_HASH_TABLE_ENTRIES; index++)
	{
		BreakpointMapPage<ADDRESS> *page, *nextpage;
		
		page = hash_table[index];
		if(page)
		{
			do
			{
				nextpage = page->next;
				delete page;
				page = nextpage;
			} while(page);
			hash_table[index] = 0;
		}
	}
}

template <class ADDRESS>
bool BreakpointRegistry<ADDRESS>::SetBreakpoint(ADDRESS addr)
{
	if(!HasBreakpoint(addr))
	{
		AllocatePage(addr);
		BreakpointMapPage<ADDRESS> *page = GetPage(addr);
		if(!page)
		{
			cerr << "Internal error: breakpoint page has not been allocated" << endl;
			return false;
		}
		page->SetBreakpoint(addr & (BreakpointMapPage<ADDRESS>::NUM_BREAKPOINTS_PER_PAGE - 1));
		breakpoints.push_back(Breakpoint<ADDRESS>(addr));
		return true;
	}
	return false;
}

template <class ADDRESS>
bool BreakpointRegistry<ADDRESS>::RemoveBreakpoint(ADDRESS addr)
{
	BreakpointMapPage<ADDRESS> *page = GetPage(addr);
	if(!page) return false;
	page->RemoveBreakpoint(addr & (BreakpointMapPage<ADDRESS>::NUM_BREAKPOINTS_PER_PAGE - 1));

	typename list<Breakpoint<ADDRESS> >::iterator breakpoint;

	for(breakpoint = breakpoints.begin(); breakpoint != breakpoints.end(); breakpoint++)
	{
		if(breakpoint->GetAddress() == addr)
		{
			breakpoints.erase(breakpoint);
			return true;
		}
	}

	return false;
}

template <class ADDRESS>
const Breakpoint<ADDRESS> *BreakpointRegistry<ADDRESS>::FindBreakpoint(ADDRESS addr)
{
	if(HasBreakpoint(addr))
	{
		typename list<Breakpoint<ADDRESS> >::const_iterator breakpoint;

		for(breakpoint = breakpoints.begin(); breakpoint != breakpoints.end(); breakpoint++)
		{
			if(breakpoint->GetAddress() == addr) return &(*breakpoint);
		}
	}
	return 0;
}

template <class ADDRESS>
bool BreakpointRegistry<ADDRESS>::HasBreakpoint(ADDRESS addr)
{
  BreakpointMapPage<ADDRESS> *page = GetPage(addr);
  if(!page) return false;
  return page->HasBreakpoint(addr & (BreakpointMapPage<ADDRESS>::NUM_BREAKPOINTS_PER_PAGE - 1));
}

template <class ADDRESS>
const list<Breakpoint<ADDRESS> >& BreakpointRegistry<ADDRESS>::GetBreakpoints()
{
	return breakpoints;
}

template <class ADDRESS>
void BreakpointRegistry<ADDRESS>::AllocatePage(ADDRESS addr)
{
	BreakpointMapPage<ADDRESS> *page;
	ADDRESS base_addr = addr & ~(BreakpointMapPage<ADDRESS>::NUM_BREAKPOINTS_PER_PAGE - 1);
	uint32_t index = (base_addr / BreakpointMapPage<ADDRESS>::NUM_BREAKPOINTS_PER_PAGE) & (NUM_HASH_TABLE_ENTRIES - 1);
	page = hash_table[index];
	if(page)
	{
		do
		{
			if(page->base_addr == base_addr)
			{
				return;
			}
		} while((page = page->next) != 0);
	}
	
	/* page not found: we must allocate a new one */
	
	page = new BreakpointMapPage<ADDRESS>(base_addr);
	page->next = hash_table[index];
	hash_table[index] = page;
}


template <class ADDRESS>
BreakpointMapPage<ADDRESS> *BreakpointRegistry<ADDRESS>::GetPage(ADDRESS addr)
{
	BreakpointMapPage<ADDRESS> *prev, *page;
	ADDRESS base_addr = addr & ~(BreakpointMapPage<ADDRESS>::NUM_BREAKPOINTS_PER_PAGE - 1);
	uint32_t index = (base_addr / BreakpointMapPage<ADDRESS>::NUM_BREAKPOINTS_PER_PAGE) & (NUM_HASH_TABLE_ENTRIES - 1);
	page = hash_table[index];
	if(page)
	{
		if(page->base_addr == base_addr) return page;
		prev = page;
		page = page->next;
		if(page)
		{
		do
		{
			if(page->base_addr == base_addr)
			{
				prev->next = page->next;
				page->next= hash_table[index];
				hash_table[index] = page;
				return page;
			}
			prev = page;
		} while((page = page->next) != 0);
		}
	}
	return page;
}


} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
