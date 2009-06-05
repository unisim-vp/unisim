/*
 *  Copyright (c) 2009,
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
 
#ifndef __UNISIM_UTIL_DEBUG_PROFILE_TCC__
#define __UNISIM_UTIL_DEBUG_PROFILE_TCC__

#include <iostream>

namespace unisim {
namespace util {
namespace debug {

template <class ADDRESS, uint32_t PAGE_SIZE>
ProfilePage<ADDRESS, PAGE_SIZE>::ProfilePage(ADDRESS _key) :
	key(_key),
	next(0)
{
	weights = new uint64_t[PAGE_SIZE];
	memset(weights, 0, sizeof(uint64_t) * PAGE_SIZE);
}

template <class ADDRESS, uint32_t PAGE_SIZE>
ProfilePage<ADDRESS, PAGE_SIZE>::~ProfilePage()
{
	delete[] weights;
}

template <class ADDRESS, uint32_t PAGE_SIZE>
std::ostream& operator << (std::ostream& os, const ProfilePage<ADDRESS, PAGE_SIZE>& page)
{
	ADDRESS base_addr = page.key * PAGE_SIZE;
	ADDRESS offset;
	bool first = true;

	for(offset = 0; offset < PAGE_SIZE; offset++)
	{
		uint64_t weight = page.weights[offset];
		if(weight > 0)
		{
			if(!first)
			{
				os << std::endl;
			}
			else
			{
				first = false;
			}
			os << "0x" << std::hex << (base_addr + offset) << std::dec << ":" << weight;
		}
	}
	return os;
}

template <class ADDRESS, uint32_t PAGE_SIZE>
Profile<ADDRESS, PAGE_SIZE>::Profile()
{
}

template <class ADDRESS, uint32_t PAGE_SIZE>
Profile<ADDRESS, PAGE_SIZE>::~Profile()
{
}

template <class ADDRESS, uint32_t PAGE_SIZE>
void Profile<ADDRESS, PAGE_SIZE>::Accumulate(ADDRESS addr, uint64_t weight)
{
	ProfilePage<ADDRESS, PAGE_SIZE> *page;

	ADDRESS key = addr / PAGE_SIZE;

	page = hash_table.Find(key);
	if(!page)
	{
		page = new ProfilePage<ADDRESS, PAGE_SIZE>(key);
		hash_table.Insert(page);
	}

	page->weights[addr & (PAGE_SIZE - 1)] += weight;
}

template <class ADDRESS, uint32_t PAGE_SIZE>
std::ostream& operator << (std::ostream& os, const Profile<ADDRESS, PAGE_SIZE>& prof)
{
	bool first = true;
	typename std::map<ADDRESS, ProfilePage<ADDRESS, PAGE_SIZE> *>::map map = prof.hash_table;
	typename std::map<ADDRESS, ProfilePage<ADDRESS, PAGE_SIZE> *>::const_iterator iter;

	for(iter = map.begin(); iter != map.end(); iter++)
	{
		if(!first)
		{
			os << std::endl;
		}
		else
		{
			first = false;
		}
		os << *(*iter).second;
	}

	return os;
}

template <class ADDRESS, uint32_t PAGE_SIZE>
Profile<ADDRESS, PAGE_SIZE>::operator std::map<ADDRESS, uint64_t>() const
{
	std::map<ADDRESS, uint64_t> map;

	typename std::map<ADDRESS, ProfilePage<ADDRESS, PAGE_SIZE> *>::map page_map = hash_table;
	typename std::map<ADDRESS, ProfilePage<ADDRESS, PAGE_SIZE> *>::const_iterator iter;

	for(iter = page_map.begin(); iter != page_map.end(); iter++)
	{
		ProfilePage<ADDRESS, PAGE_SIZE> *page = (*iter).second;

		ADDRESS base_addr = page->key * PAGE_SIZE;
		ADDRESS offset;

		for(offset = 0; offset < PAGE_SIZE; offset++)
		{
			uint64_t weight = page->weights[offset];
			if(weight > 0)
			{
				map.insert(std::pair<ADDRESS, uint64_t>(base_addr + offset, weight));
			}
		}
	}
	return map;
}

} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
