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

template <class ADDRESS>
WatchpointMapPage<ADDRESS>::WatchpointMapPage(ADDRESS addr)
{
	this->base_addr = addr & ~(NUM_WATCHPOINTS_PER_PAGE - 1);
	map = new uint32_t[(NUM_WATCHPOINTS_PER_PAGE + 15)/ 16];
	memset(map, 0, ((NUM_WATCHPOINTS_PER_PAGE + 15)/ 16) * sizeof(uint32_t));
	next = 0;
}

template <class ADDRESS>
WatchpointMapPage<ADDRESS>::~WatchpointMapPage()
{
	delete[] map;
}

template <class ADDRESS>
void WatchpointMapPage<ADDRESS>::SetWatchpoint(unisim::util::debug::MemoryAccessType mat, uint32_t offset, uint32_t size)
{
	uint32_t i;
	for(i = 0; i < size; i++, offset++)
		map[offset / 16] |= ((mat & unisim::util::debug::MAT_READ ? 0x01UL : 0) | (mat & unisim::util::debug::MAT_WRITE ? 0x02UL : 0)) << (2 * (offset % 16));
}

template <class ADDRESS>
void WatchpointMapPage<ADDRESS>::RemoveWatchpoint(unisim::util::debug::MemoryAccessType mat, uint32_t offset, uint32_t size)
{
	uint32_t i;
	for(i = 0; i < size; i++, offset++)
		map[offset / 16] &= ~(((mat & unisim::util::debug::MAT_READ ? 0x01UL : 0) | (mat & unisim::util::debug::MAT_WRITE ? 0x02UL : 0)) << (2 * (offset % 16)));
}

template <class ADDRESS>
bool WatchpointMapPage<ADDRESS>::HasWatchpoint(unisim::util::debug::MemoryAccessType mat, uint32_t offset, uint32_t size) const
{
	uint32_t offset_mod_16 = offset % 16;
	uint32_t offset_div_16 = offset / 16;
	
	switch(size)
	{
		case 1:
			return (map[offset_div_16] & (((mat & unisim::util::debug::MAT_READ ? 0x1UL : 0) | (mat & unisim::util::debug::MAT_WRITE ? 0x2UL : 0)) << (2 * offset_mod_16))) ? true : false;
		case 2:
			if(offset_mod_16 <= 14)
			{
				return (map[offset_div_16] & (((mat & unisim::util::debug::MAT_READ ? 0x5UL : 0) | (mat & unisim::util::debug::MAT_WRITE ? 0xaUL : 0)) << (2 * offset_mod_16))) ? true : false;
			}
			else
			{
				uint64_t mask = (uint64_t) ((mat & unisim::util::debug::MAT_READ ? 0x5ULL : 0) | (mat & unisim::util::debug::MAT_WRITE ? 0xaULL : 0)) << (2 * offset_mod_16);
				return (map[offset_div_16] & (uint32_t) mask) || (map[offset_div_16 + 1] & (uint32_t)(mask >> 32)) ? true : false;
			}
			break;
		case 4:
			if(offset_mod_16 <= 12)
			{
				return (map[offset_div_16] & (((mat & unisim::util::debug::MAT_READ ? 0x55UL : 0) | (mat & unisim::util::debug::MAT_WRITE ? 0xaaUL : 0)) << (2 * offset_mod_16))) ? true : false;
			}
			else
			{
				uint64_t mask = (uint64_t) ((mat & unisim::util::debug::MAT_READ ? 0x55ULL : 0) | (mat & unisim::util::debug::MAT_WRITE ? 0xaaULL : 0)) << (2 * offset_mod_16);
				return (map[offset_div_16] & (uint32_t) mask) || (map[offset_div_16 + 1] & (uint32_t)(mask >> 32)) ? true : false;
			}
			break;
		case 8:
			if(offset_mod_16 <= 8)
			{
				return (map[offset_div_16] & (((mat & unisim::util::debug::MAT_READ ? 0x5555UL : 0) | (mat & unisim::util::debug::MAT_WRITE ? 0xaaaaUL : 0)) << (2 * offset_mod_16))) ? true : false;
			}
			else
			{
				uint64_t mask = (uint64_t) ((mat & unisim::util::debug::MAT_READ ? 0x5555ULL : 0) | (mat & unisim::util::debug::MAT_WRITE ? 0xaaaaULL : 0)) << (2 * offset_mod_16);
				return (map[offset_div_16] & (uint32_t) mask) || (map[offset_div_16 + 1] & (uint32_t)(mask >> 32)) ? true : false;
			}
			break;
		case 16:
			if(offset_mod_16 == 0)
			{
				return (map[offset_div_16] & (((mat & unisim::util::debug::MAT_READ ? 0x55555555UL : 0) | (mat & unisim::util::debug::MAT_WRITE ? 0xaaaaaaaaUL : 0)) << (2 * offset_mod_16))) ? true : false;
			}
			else
			{
				uint64_t mask = (uint64_t) ((mat & unisim::util::debug::MAT_READ ? 0x55555555ULL : 0) | (mat & unisim::util::debug::MAT_WRITE ? 0xaaaaaaaaULL : 0)) << (2 * offset_mod_16);
				return (map[offset_div_16] & (uint32_t) mask) || (map[offset_div_16 + 1] & (uint32_t)(mask >> 32)) ? true : false;
			}
			break;

		default:
			// access is not 1, 2, 4, 8, or 16 bytes long
			if(size)
			{
				uint32_t mask = (mat & unisim::util::debug::MAT_READ ? 0x01UL : 0) | (mat & unisim::util::debug::MAT_WRITE ? 0x02UL : 0);
				do
				{
					if(map[offset / 16] & (mask << (2 * (offset % 16)))) return true;
				} while(++offset, --size);
			}
	}

	return false;
}

template <class ADDRESS>
WatchpointRegistry<ADDRESS>::WatchpointRegistry() :
	has_watchpoints(false)
{
	memset(hash_table, 0, sizeof(hash_table));
}

template <class ADDRESS>
WatchpointRegistry<ADDRESS>::~WatchpointRegistry()
{
	Reset();
}

template <class ADDRESS>
void WatchpointRegistry<ADDRESS>::Reset()
{
	uint32_t mt;
	uint32_t index;
	for(mt = 0; mt < 2; mt++)
	{
		for(index = 0; index < NUM_HASH_TABLE_ENTRIES; index++)
		{
			WatchpointMapPage<ADDRESS> *page, *nextpage;
			
			page = hash_table[mt][index];
			if(page)
			{
				do
				{
					nextpage = page->next;
					delete page;
					page = nextpage;
				} while(page);
				hash_table[mt][index] = 0;
			}
		}
	}
}

template <class ADDRESS>
bool WatchpointRegistry<ADDRESS>::SetWatchpoint(unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size)
{
	if(size > 0)
	{
		typename list<Watchpoint<ADDRESS> >::const_iterator watchpoint;
	
		for(watchpoint = watchpoints.begin(); watchpoint != watchpoints.end(); watchpoint++)
		{
			if(watchpoint->GetMemoryType() == mt && watchpoint->GetMemoryAccessType() == mat && watchpoint->HasOverlap(addr, size))
			{
	// 		  cout << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__
	// 		       << ": FALSE mat = " << mat 
	// 		       << " mt = " << mt
	// 		       << " addr = 0x" << hex << addr << dec
	// 		       << " size = " << size << endl;
				return false;
			}
		}
		watchpoints.push_back(Watchpoint<ADDRESS>(mat, mt, addr, size));
		has_watchpoints = true;

		do
		{
			uint32_t size_to_page_boundary = WatchpointMapPage<ADDRESS>::NUM_WATCHPOINTS_PER_PAGE - (addr & (WatchpointMapPage<ADDRESS>::NUM_WATCHPOINTS_PER_PAGE - 1));
			uint32_t sz = size > size_to_page_boundary ? size_to_page_boundary : size;

			AllocatePage(mt, addr);
			WatchpointMapPage<ADDRESS> *page = GetPage(mt, addr);
			if(!page)
			{
				cerr << "Internal error: watchpoint page has not been allocated" << endl;
				return false;
			}
		
			page->SetWatchpoint(mat, addr & (WatchpointMapPage<ADDRESS>::NUM_WATCHPOINTS_PER_PAGE - 1), sz);
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

template <class ADDRESS>
bool WatchpointRegistry<ADDRESS>::SetWatchpoint(const Watchpoint<ADDRESS>& wp)
{
	unisim::util::debug::MemoryAccessType mat = wp.GetMemoryAccessType();
	unisim::util::debug::MemoryType mt = wp.GetMemoryType();
	ADDRESS addr = wp.GetAddress();
	uint32_t size = wp.GetSize();
	
	return SetWatchpoint(mat, mt, addr, size);
}

template <class ADDRESS>
bool WatchpointRegistry<ADDRESS>::RemoveWatchpoint(unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size)
{

	bool status = false;

	if(size > 0)
	{
		typename list<Watchpoint<ADDRESS> >::iterator watchpoint;

		for(watchpoint = watchpoints.begin(); watchpoint != watchpoints.end(); watchpoint++)
		{
			if(watchpoint->GetAddress() == addr && watchpoint->GetSize() == size && watchpoint->GetMemoryType() == mt && watchpoint->GetMemoryAccessType() == mat)
			{
				watchpoints.erase(watchpoint);
				if(watchpoints.empty())
					has_watchpoints = false;
				status = true;
				break;
			}
		}

		do
		{
			uint32_t size_to_page_boundary = WatchpointMapPage<ADDRESS>::NUM_WATCHPOINTS_PER_PAGE - (addr & (WatchpointMapPage<ADDRESS>::NUM_WATCHPOINTS_PER_PAGE - 1));
			uint32_t sz = size > size_to_page_boundary ? size_to_page_boundary : size;

			WatchpointMapPage<ADDRESS> *page = GetPage(mt, addr);
			if(!page) return false;

			page->RemoveWatchpoint(mat, addr & (WatchpointMapPage<ADDRESS>::NUM_WATCHPOINTS_PER_PAGE - 1), sz);
			size -= sz;
			addr += sz;
		} while(size > 0);

	}

   return status;

}

template <class ADDRESS>
bool WatchpointRegistry<ADDRESS>::RemoveWatchpoint(const Watchpoint<ADDRESS>& wp)
{
	unisim::util::debug::MemoryAccessType mat = wp.GetMemoryAccessType();
	unisim::util::debug::MemoryType mt = wp.GetMemoryType();
	ADDRESS addr = wp.GetAddress();
	uint32_t size = wp.GetSize();
	
	return RemoveWatchpoint(mat, mt, addr, size);
}

template <class ADDRESS>
const Watchpoint<ADDRESS> *WatchpointRegistry<ADDRESS>::FindWatchpoint(unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size) const
{
//   cout << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << ":"
//        << "addr = 0x" << hex << addr << dec << " size = " << size 
//        << " mat = " << mat << " mt = " << mt << endl;
	if(HasWatchpoint(mat, mt, addr, size))
	{
		typename list<Watchpoint<ADDRESS> >::const_iterator watchpoint;

		for(watchpoint = watchpoints.begin(); watchpoint != watchpoints.end(); watchpoint++)
		{
			if(watchpoint->GetMemoryType() == mt && watchpoint->GetMemoryAccessType() == mat && watchpoint->HasOverlap(addr, size)) return &(*watchpoint);
		}
	}
	return 0;
}

template <class ADDRESS>
bool WatchpointRegistry<ADDRESS>::HasWatchpoint(unisim::util::debug::MemoryAccessType mat, unisim::util::debug::MemoryType mt, ADDRESS addr, uint32_t size) const
{
	if(!has_watchpoints) return false;
	if(size > 0)
	{
		do
		{
			uint32_t size_to_page_boundary = WatchpointMapPage<ADDRESS>::NUM_WATCHPOINTS_PER_PAGE - (addr & (WatchpointMapPage<ADDRESS>::NUM_WATCHPOINTS_PER_PAGE - 1));
			uint32_t sz = size > size_to_page_boundary ? size_to_page_boundary : size;

			const WatchpointMapPage<ADDRESS> *page = GetPage(mt, addr);
			if(!page) return false;
			if(page->HasWatchpoint(mat, addr & (WatchpointMapPage<ADDRESS>::NUM_WATCHPOINTS_PER_PAGE - 1), sz)) return true;

			size -= sz;
			addr += sz;
		} while(size > 0);
	}
	return false;
}

template <class ADDRESS>
bool WatchpointRegistry<ADDRESS>::HasWatchpoint(const Watchpoint<ADDRESS>& wp) const
{
	unisim::util::debug::MemoryAccessType mat = wp.GetMemoryAccessType();
	unisim::util::debug::MemoryType mt = wp.GetMemoryType();
	ADDRESS addr = wp.GetAddress();
	uint32_t size = wp.GetSize();
	
	return HasWatchpoint(mat, mt, addr, size);
}

template <class ADDRESS>
bool WatchpointRegistry<ADDRESS>::HasWatchpoints() const {
	return has_watchpoints;
}

template <class ADDRESS>
const list<Watchpoint<ADDRESS> >& WatchpointRegistry<ADDRESS>::GetWatchpoints() const
{
	return watchpoints;
}

template <class ADDRESS>
void WatchpointRegistry<ADDRESS>::AllocatePage(unisim::util::debug::MemoryType mt, ADDRESS addr)
{
	WatchpointMapPage<ADDRESS> *page;
	ADDRESS base_addr = addr & ~(WatchpointMapPage<ADDRESS>::NUM_WATCHPOINTS_PER_PAGE - 1);
	uint32_t index = (base_addr / WatchpointMapPage<ADDRESS>::NUM_WATCHPOINTS_PER_PAGE) & (NUM_HASH_TABLE_ENTRIES - 1);
	page = hash_table[mt][index];
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
	
	page = new WatchpointMapPage<ADDRESS>(base_addr);
	page->next = hash_table[mt][index];
	hash_table[mt][index] = page;
}


template <class ADDRESS>
const WatchpointMapPage<ADDRESS> *WatchpointRegistry<ADDRESS>::GetPage(unisim::util::debug::MemoryType mt, ADDRESS addr) const
{
	WatchpointMapPage<ADDRESS> *page;
	ADDRESS base_addr = addr & ~(WatchpointMapPage<ADDRESS>::NUM_WATCHPOINTS_PER_PAGE - 1);
	uint32_t index = (base_addr / WatchpointMapPage<ADDRESS>::NUM_WATCHPOINTS_PER_PAGE) & (NUM_HASH_TABLE_ENTRIES - 1);
	page = hash_table[mt][index];
	if(page)
	{
		if(page->base_addr == base_addr) return page;
		page = page->next;
		if(page)
		{
			do
			{
				if(page->base_addr == base_addr)
				{
					return page;
				}
			} while((page = page->next) != 0);
		}
	}
	return page;
}

template <class ADDRESS>
WatchpointMapPage<ADDRESS> *WatchpointRegistry<ADDRESS>::GetPage(unisim::util::debug::MemoryType mt, ADDRESS addr)
{
	WatchpointMapPage<ADDRESS> *prev, *page;
	ADDRESS base_addr = addr & ~(WatchpointMapPage<ADDRESS>::NUM_WATCHPOINTS_PER_PAGE - 1);
	uint32_t index = (base_addr / WatchpointMapPage<ADDRESS>::NUM_WATCHPOINTS_PER_PAGE) & (NUM_HASH_TABLE_ENTRIES - 1);
	page = hash_table[mt][index];
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
					page->next= hash_table[mt][index];
					hash_table[mt][index] = page;
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
