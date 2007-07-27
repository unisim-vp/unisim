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
 
#ifndef __FS_UTILS_DEBUG_WATCHPOINT_REGISTRY_HH__
#define __FS_UTILS_DEBUG_WATCHPOINT_REGISTRY_HH__

#include <plugins/debug/instruction_level_debug_interface.hh>
#include <utils/debug/watchpoint.hh>
#include <list>

namespace full_system {
namespace utils {
namespace debug {

using full_system::plugins::debug::MemoryType;
using full_system::plugins::debug::MemoryAccessType;
using full_system::utils::debug::Watchpoint;
using full_system::plugins::debug::MAT_READ;
using full_system::plugins::debug::MAT_WRITE;
using std::list;

template <class ADDRESS>
class WatchpointMapPage
{
public:
	WatchpointMapPage(ADDRESS addr);
	~WatchpointMapPage();

	void SetWatchpoint(MemoryAccessType mat, uint32_t offset, uint32_t size);
	void RemoveWatchpoint(MemoryAccessType mat, uint32_t offset, uint32_t size);
	bool HasWatchpoint(MemoryAccessType mat, uint32_t offset, uint32_t size);

	static const uint32_t NUM_WATCHPOINTS_PER_PAGE = 4096 * 8;
	ADDRESS base_addr;			/*< base effective address */
	uint32_t *map;			/*< an array of watchpoint masks for 16 consecutive effective addresses */
	WatchpointMapPage *next;	/*< next watchpoint map page with the same hash index */
};

template <class ADDRESS>
class WatchpointRegistry
{
public:
	static const uint32_t NUM_HASH_TABLE_ENTRIES = 4096;

	WatchpointRegistry();
	virtual ~WatchpointRegistry();

	bool SetWatchpoint(MemoryAccessType mat, MemoryType mt, ADDRESS addr, uint32_t size);
	bool RemoveWatchpoint(MemoryAccessType mat, MemoryType mt, ADDRESS addr, uint32_t size);
	bool HasWatchpoint(MemoryAccessType mat, MemoryType mt, ADDRESS addr, uint32_t size);
	const Watchpoint<ADDRESS> *FindWatchpoint(MemoryAccessType mat, MemoryType mt, ADDRESS addr, uint32_t size);
	const list<Watchpoint<ADDRESS> >& GetWatchpoints();

private:
	list<Watchpoint<ADDRESS> > watchpoints;
	WatchpointMapPage<ADDRESS> *hash_table[2][NUM_HASH_TABLE_ENTRIES];

	void AllocatePage(MemoryType mt, ADDRESS addr);
	WatchpointMapPage<ADDRESS> *GetPage(MemoryType mt, ADDRESS addr);
	
};

} // end of namespace debug
} // end of namespace utils
} // end of namespace full_system

#endif
