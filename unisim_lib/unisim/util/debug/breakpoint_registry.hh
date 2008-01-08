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
 
#ifndef __UNISIM_UTIL_DEBUG_BREAKPOINT_REGISTRY_HH__
#define __UNISIM_UTIL_DEBUG_BREAKPOINT_REGISTRY_HH__

#include <unisim/util/debug/breakpoint.hh>
#include <list>

namespace unisim {
namespace util {
namespace debug {

using std::list;
	
template <class ADDRESS>
class BreakpointMapPage
{
public:
	BreakpointMapPage(ADDRESS addr);
	~BreakpointMapPage();

	void SetBreakpoint(uint32_t offset);
	void RemoveBreakpoint(uint32_t offset);
	bool HasBreakpoint(uint32_t offset);

	static const uint32_t NUM_BREAKPOINTS_PER_PAGE = 256;// MUST BE a power of two !
	ADDRESS base_addr;			/*< base effective address */
	uint32_t *map;			/*< an array of breakpoint masks for 32 consecutive effective addresses */
	BreakpointMapPage *next;	/*< next breakpoint map page with the same hash index */
};

template <class ADDRESS>
class BreakpointRegistry
{
public:
	static const uint32_t NUM_HASH_TABLE_ENTRIES = 32; // MUST BE a power of two !

	BreakpointRegistry();
	virtual ~BreakpointRegistry();

	bool SetBreakpoint(ADDRESS addr);
	bool RemoveBreakpoint(ADDRESS addr);
	bool HasBreakpoint(ADDRESS addr);
	bool HasBreakpoints() const;
	const Breakpoint<ADDRESS> *FindBreakpoint(ADDRESS addr);
	const list<Breakpoint<ADDRESS> >& GetBreakpoints();

private:
	bool has_breakpoints;
	list<Breakpoint<ADDRESS> > breakpoints;
	BreakpointMapPage<ADDRESS> *hash_table[NUM_HASH_TABLE_ENTRIES];

	void AllocatePage(ADDRESS addr);
	BreakpointMapPage<ADDRESS> *GetPage(ADDRESS addr);
	
};

} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
