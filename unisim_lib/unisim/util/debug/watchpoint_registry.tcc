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
 
#ifndef __UNISIM_UTIL_DEBUG_WATCHPOINT_REGISTRY_TCC__
#define __UNISIM_UTIL_DEBUG_WATCHPOINT_REGISTRY_TCC__

#include <iostream>
#include <string.h>

namespace unisim {
namespace util {
namespace debug {

using std::cerr;
using std::endl;

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
void WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS>::SetWatchpoint(unisim::util::debug::MemoryAccessType mat, uint32_t offset, uint32_t size, unsigned int front_end_num)
{
	WORD mask = ((mat & unisim::util::debug::MAT_READ ? 0x01UL : 0) | (mat & unisim::util::debug::MAT_WRITE ? 0x02UL : 0)) << (BITS_PER_WATCHPOINT * front_end_num);

	unsigned int i;
	
	for(i = 0; i < size; i++, offset++)
	{
		map[offset / WATCHPOINTS_PER_WORD] |= mask << (BITS_PER_WATCHPOINTS * (offset % WATCHPOINTS_PER_WORD));
	}
}

template <typename ADDRESS, unsigned int MAX_FRONT_ENDS>
void WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS>::RemoveWatchpoint(unisim::util::debug::MemoryAccessType mat, uint32_t offset, uint32_t size, unsigned int front_end_num)
{
	WORD mask = ((mat & unisim::util::debug::MAT_READ ? 0x01UL : 0) | (mat & unisim::util::debug::MAT_WRITE ? 0x02UL : 0)) << (BITS_PER_WATCHPOINT * front_end_num);

	unsigned int i;
	for(i = 0; i < size; i++, offset++)
	{
		map[offset / WATCHPOINTS_PER_WORD] &= ~(mask << (BITS_PER_WATCHPOINT * (offset % WATCHPOINTS_PER_WORD)));
	}
}

template <typename ADDRESS, unsigned int MAX_FRONT_ENDS>
bool WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS>::HasWatchpoint(unisim::util::debug::MemoryAccessType mat, uint32_t offset, uint32_t size, unsigned int front_end_num) const
{
	// TODO: optimize this function for 1, 2, 4, 8 bytes accesses
	WORD mask = ((mat & unisim::util::debug::MAT_READ ? 0x01UL : 0) | (mat & unisim::util::debug::MAT_WRITE ? 0x02UL : 0)) << (BITS_PER_WATCHPOINT * front_end_num);

	unsigned int i;
	for(i = 0; i < size; i++, offset++)
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
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
	{
		mask |= ((mat & unisim::util::debug::MAT_READ ? 0x01UL : 0) | (mat & unisim::util::debug::MAT_WRITE ? 0x02UL : 0)) << (BITS_PER_WATCHPOINT * front_end_num);
	}

	unsigned int i;
	for(i = 0; i < size; i++, offset++)
	{
		if(map[offset / WATCHPOINTS_PER_WORD] & (mask << (BITS_PER_WATCHPOINTS * (offset % WATCHPOINTS_PER_WORD)))) return true;
	}
	
	return false;
}

template <typename ADDRESS, unsigned int NUM_PROCESSORS, unsigned int MAX_FRONT_ENDS>
WatchpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS>::WatchpointRegistry()
	: has_watchpoints()
	, watchpoints()
	, mru_page()
	, hash_table()
{
}

template <typename ADDRESS, unsigned int NUM_PROCESSORS, unsigned int MAX_FRONT_ENDS>
WatchpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS>::~WatchpointRegistry()
{
	Reset();
}

template <typename ADDRESS, unsigned int NUM_PROCESSORS, unsigned int MAX_FRONT_ENDS>
void WatchpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS>::Reset()
{
	unsigned int prc_num;
	unsigned int front_end_num;
	uint32_t mt;
	uint32_t index;
	for(mt = 0; mt < 2; mt++)
	{
		for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
		{
			for(index = 0; index < NUM_HASH_TABLE_ENTRIES; index++)
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
		
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
		{
			typename std::list<Watchpoint<ADDRESS> *>::const_iterator watchpoint_it;

			for(watchpoint_it = watchpoints[prc_num][front_end_num].begin(); watchpoint_it != watchpoints[prc_num][front_end_num].end(); watchpoint_it++)
			{
				Watchpoint<ADDRESS> *watchpoint = *watchpoint_it;
				if(watchpoint)
				{
					delete watchpoint;
				}
			}
		
			has_watchpoints[prc_num][front_end_num] = false;
		}
	}
}

template <typename ADDRESS, unsigned int NUM_PROCESSORS, unsigned int MAX_FRONT_ENDS>
bool WatchpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS>::SetWatchpoint(Watchpoint<ADDRESS> *wp)
{
	unisim::util::debug::MemoryAccessType mat = wp->GetMemoryAccessType();
	unisim::util::debug::MemoryType mt = wp->GetMemoryType();
	ADDRESS addr = wp->GetAddress();
	uint32_t size = wp->GetSize();
	unsigned int prc_num = wp->GetProcessorNumber();
	unsigned int front_end_num = wp->GetFrontEndNumber();
	
	if(prc_num >= NUM_PROCESSORS) return false;
	if(front_end_num >= MAX_FRONT_ENDS) return false;
	
	if(size > 0)
	{
		typename std::list<Watchpoint<ADDRESS> *>::const_iterator watchpoint_it;
	
		for(watchpoint_it = watchpoints[prc_num][front_end_num].begin(); watchpoint_it != watchpoints[prc_num][front_end_num].end(); watchpoint_it++)
		{
			Watchpoint<ADDRESS> *watchpoint = *watchpoint_it;
			if((watchpoint->GetMemoryType() == mt) && (watchpoint->GetMemoryAccessType() == mat) && watchpoint->HasOverlap(addr, size))
			{
//				cout << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__
//					<< ": FALSE mat = " << mat 
//					<< " mt = " << mt
//					<< " addr = 0x" << hex << addr << dec
//					<< " size = " << size << endl;
				return false;
			}
		}
		watchpoints[prc_num][front_end_num].push_back(wp);
		wp->Catch();
		has_watchpoints[prc_num][front_end_num] = true;

		do
		{
			uint32_t size_to_page_boundary = WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_WATCHPOINTS_PER_PAGE - (addr & (WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_WATCHPOINTS_PER_PAGE - 1));
			uint32_t sz = size > size_to_page_boundary ? size_to_page_boundary : size;

			AllocatePage(mt, addr, prc_num);
			WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS> *page = GetPage(mt, addr, prc_num);
			if(!page)
			{
				cerr << "Internal error: watchpoint page has not been allocated" << endl;
				return false;
			}
		
			page->SetWatchpoint(mat, addr & (WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_WATCHPOINTS_PER_PAGE - 1), sz, front_end_num);
		// 	cout << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__
		// 	     << ": TRUE mat = " << mat 
		// 	     << " mt = " << mt
		// 	     << " addr = 0x" << hex << addr << dec
		// 	     << " size = " << size << endl;
			size -= sz;
			addr += sz;
		} while(size > 0);
		return true;
	}
	return false;
}

template <typename ADDRESS, unsigned int NUM_PROCESSORS, unsigned int MAX_FRONT_ENDS>
bool WatchpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS>::RemoveWatchpoint(Watchpoint<ADDRESS> *wp)
{
	unisim::util::debug::MemoryAccessType mat = wp->GetMemoryAccessType();
	unisim::util::debug::MemoryType mt = wp->GetMemoryType();
	ADDRESS addr = wp->GetAddress();
	uint32_t size = wp->GetSize();
	unsigned int prc_num = wp->GetProcessorNumber();
	unsigned int front_end_num = wp->GetFrontEndNumber();

	if(prc_num >= NUM_PROCESSORS) return false;
	if(front_end_num >= MAX_FRONT_ENDS) return false;
	
	bool status = false;

	if(size > 0)
	{
		typename std::list<Watchpoint<ADDRESS> *>::iterator watchpoint_it;

		for(watchpoint_it = watchpoints[prc_num][front_end_num].begin(); watchpoint_it != watchpoints[prc_num][front_end_num].end(); watchpoint_it++)
		{
			Watchpoint<ADDRESS> *watchpoint = *watchpoint_it;
			if((watchpoint->GetAddress() == addr) && (watchpoint->GetSize() == size) && (watchpoint->GetMemoryType() == mt) && (watchpoint->GetMemoryAccessType() == mat))
			{
				watchpoint->Release();
				watchpoints[prc_num][front_end_num].erase(watchpoint_it);
				if(watchpoints[prc_num][front_end_num].empty())
					has_watchpoints[prc_num][front_end_num] = false;
				status = true;
				break;
			}
		}

		do
		{
			uint32_t size_to_page_boundary = WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_WATCHPOINTS_PER_PAGE - (addr & (WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_WATCHPOINTS_PER_PAGE - 1));
			uint32_t sz = size > size_to_page_boundary ? size_to_page_boundary : size;

			WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS> *page = GetPage(mt, addr, prc_num);
			if(!page) return false;

			page->RemoveWatchpoint(mat, addr & (WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS>::NUM_WATCHPOINTS_PER_PAGE - 1), sz, front_end_num);
			size -= sz;
			addr += sz;
		} while(size > 0);

	}

   return status;
}

template <typename ADDRESS, unsigned int NUM_PROCESSORS, unsigned int MAX_FRONT_ENDS>
bool WatchpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS>::HasWatchpoint(Watchpoint<ADDRESS> *wp) const
{
	unisim::util::debug::MemoryAccessType mat = wp->GetMemoryAccessType();
	unisim::util::debug::MemoryType mt = wp->GetMemoryType();
	ADDRESS addr = wp->GetAddress();
	uint32_t size = wp->GetSize();
	unsigned int prc_num = wp->GetProcessorNumber();
	unsigned int front_end_num = wp->GetFrontEndNumber();
	
	return HasWatchpoint(mat, mt, addr, size, prc_num, front_end_num);
}

template <typename ADDRESS, unsigned int NUM_PROCESSORS, unsigned int MAX_FRONT_ENDS>
bool WatchpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS>::HasWatchpoints(unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size, unsigned int prc_num) const
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

template <typename ADDRESS, unsigned int NUM_PROCESSORS, unsigned int MAX_FRONT_ENDS>
bool WatchpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS>::HasWatchpoints(unsigned int prc_num) const
{
	unsigned int front_end_num;
	
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
	{
		if(has_watchpoints[prc_num][front_end_num]) return true;
	}
	
	return false;
}

template <typename ADDRESS, unsigned int NUM_PROCESSORS, unsigned int MAX_FRONT_ENDS>
bool WatchpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS>::HasWatchpoints() const
{
	unsigned int prc_num;
	unsigned int front_end_num;
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
		{
			if(has_watchpoints[prc_num][front_end_num]) return true;
		}
	}
	
	return false;
}

template <typename ADDRESS, unsigned int NUM_PROCESSORS, unsigned int MAX_FRONT_ENDS>
Watchpoint<ADDRESS> *WatchpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS>::FindWatchpoint(unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size, unsigned int prc_num, unsigned int front_end_num) const
{
//   cout << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << ":"
//        << "addr = 0x" << hex << addr << dec << " size = " << size 
//        << " mat = " << mat << " mt = " << mt << endl;
	if(HasWatchpoint(mat, mt, addr, size, prc_num, front_end_num))
	{
		typename std::list<Watchpoint<ADDRESS> *>::const_iterator watchpoint_it;

		for(watchpoint_it = watchpoints[prc_num][front_end_num].begin(); watchpoint_it != watchpoints[prc_num][front_end_num].end(); watchpoint_it++)
		{
			Watchpoint<ADDRESS> *watchpoint = *watchpoint_it;
			if((watchpoint->GetMemoryType() == mt) && (watchpoint->GetMemoryAccessType() == mat) && watchpoint->HasOverlap(addr, size)) return watchpoint;
		}
	}
	return 0;
}

template <typename ADDRESS, unsigned int NUM_PROCESSORS, unsigned int MAX_FRONT_ENDS>
void WatchpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS>::EnumerateWatchpoints(unsigned int prc_num, unsigned int front_end_num, std::list<Event<ADDRESS> *>& lst) const
{
	typename std::list<Watchpoint<ADDRESS> *>::const_iterator watchpoint_it;
	
	for(watchpoint_it = watchpoints[prc_num][front_end_num].begin(); watchpoint_it != watchpoints[prc_num][front_end_num].end(); watchpoint_it++)
	{
		Watchpoint<ADDRESS> *watchpoint = *watchpoint_it;
		
		lst.push_back(watchpoint);
	}
}

template <typename ADDRESS, unsigned int NUM_PROCESSORS, unsigned int MAX_FRONT_ENDS>
void WatchpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS>::EnumerateWatchpoints(unsigned int front_end_num, std::list<Event<ADDRESS> *>& lst) const
{
	unsigned int prc_num;
	
	for(prc_num = 0; prc_num < NUM_PROCESSORS; prc_num++)
	{
		EnumerateWatchpoints(prc_num, front_end_num, lst);
	}
}

template <typename ADDRESS, unsigned int NUM_PROCESSORS, unsigned int MAX_FRONT_ENDS>
void WatchpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS>::EnumerateWatchpoints(std::list<Event<ADDRESS> *>& lst) const
{
	unsigned int front_end_num;
	
	for(front_end_num = 0; front_end_num < MAX_FRONT_ENDS; front_end_num++)
	{
		EnumerateWatchpoints(front_end_num, lst);
	}
}

template <typename ADDRESS, unsigned int NUM_PROCESSORS, unsigned int MAX_FRONT_ENDS>
bool WatchpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS>::HasWatchpoint(unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size, unsigned int prc_num, unsigned int front_end_num) const
{
	if(prc_num >= NUM_PROCESSORS) return false;
	if(front_end_num >= MAX_FRONT_ENDS) return false;
	if(!has_watchpoints[prc_num][front_end_num]) return false;
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

template <typename ADDRESS, unsigned int NUM_PROCESSORS, unsigned int MAX_FRONT_ENDS>
void WatchpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS>::AllocatePage(unisim::util::debug::MemoryType mt, ADDRESS addr, unsigned int prc_num)
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

template <typename ADDRESS, unsigned int NUM_PROCESSORS, unsigned int MAX_FRONT_ENDS>
WatchpointMapPage<ADDRESS, MAX_FRONT_ENDS> *WatchpointRegistry<ADDRESS, NUM_PROCESSORS, MAX_FRONT_ENDS>::GetPage(unisim::util::debug::MemoryType mt, ADDRESS addr, unsigned int prc_num) const
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
