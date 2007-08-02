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
 
#ifndef __UNISIM_COMPONENT_CXX_CACHE_MESI_CACHE_TCC__
#define __UNISIM_COMPONENT_CXX_CACHE_MESI_CACHE_TCC__

#include <unisim/component/c++/cache/cache.tcc>

namespace unisim {
namespace component {
namespace cxx {
namespace cache {
namespace mesi {

template <class PHYSICAL_ADDRESS_T, uint32_t CACHE_BLOCK_SIZE>
CacheBlock<PHYSICAL_ADDRESS_T, CACHE_BLOCK_SIZE>::CacheBlock()
{
	state = ST_INVALID;
	base_physical_addr = 0;
	memset(data, 0, CACHE_BLOCK_SIZE);
	prev = 0;
	next = 0;
}

template <class PHYSICAL_ADDRESS_T, uint32_t CACHE_BLOCK_SIZE>
inline void CacheBlock<PHYSICAL_ADDRESS_T, CACHE_BLOCK_SIZE>::Zero()
{
/*		cerr << "0x" << hex << base_physical_addr << dec << ": ";
	switch(state)
	{
		case ST_INVALID: cerr << "(I)"; break;
		case ST_SHARED: cerr << "(S)"; break;
		case ST_EXCLUSIVE: cerr << "(E)"; break;
		case ST_MODIFIED: cerr << "(M)"; break;
	}
	cerr << "-[Zero]->(M)" << endl;*/
	memset(data, 0, CACHE_BLOCK_SIZE);
}

/* Write into block */
template <class PHYSICAL_ADDRESS_T, uint32_t CACHE_BLOCK_SIZE>
inline void CacheBlock<PHYSICAL_ADDRESS_T, CACHE_BLOCK_SIZE>::Write(uint32_t offset, const void *buffer, uint32_t size)
{
// 		cerr << "0x" << hex << base_physical_addr << dec << ": ";
// 		switch(state)
// 		{
// 			case ST_INVALID: cerr << "(I)"; break;
// 			case ST_SHARED: cerr << "(S)"; break;
// 			case ST_EXCLUSIVE: cerr << "(E)"; break;
// 			case ST_MODIFIED: cerr << "(M)"; break;
// 		}
// 		cerr << "-[Write]->(M)" << endl;

	if(offset + size > CACHE_BLOCK_SIZE) *(int *) 0 = 0;
	memcpy(&data[offset], buffer, size);
}

/* Read from block */
template <class PHYSICAL_ADDRESS_T, uint32_t CACHE_BLOCK_SIZE>
inline void CacheBlock<PHYSICAL_ADDRESS_T, CACHE_BLOCK_SIZE>::Read(uint32_t offset, void *buffer, uint32_t size)
{
	memcpy(buffer, &data[offset], size);
}

template <class PHYSICAL_ADDRESS_T, uint32_t CACHE_BLOCK_SIZE>
inline bool CacheBlock<PHYSICAL_ADDRESS_T, CACHE_BLOCK_SIZE>::IsValid()
{
	return state != ST_INVALID;
}

template <class PHYSICAL_ADDRESS_T, uint32_t CACHE_BLOCK_SIZE>
inline bool CacheBlock<PHYSICAL_ADDRESS_T, CACHE_BLOCK_SIZE>::IsModified()
{
	return state == ST_MODIFIED;
}

template <class PHYSICAL_ADDRESS_T, uint32_t CACHE_BLOCK_SIZE>
inline bool CacheBlock<PHYSICAL_ADDRESS_T, CACHE_BLOCK_SIZE>::IsShared()
{
	return state == ST_SHARED;
}

template <class PHYSICAL_ADDRESS_T, uint32_t CACHE_BLOCK_SIZE>
inline void CacheBlock<PHYSICAL_ADDRESS_T, CACHE_BLOCK_SIZE>::SetModified()
{
	state = ST_MODIFIED;
}

template <class PHYSICAL_ADDRESS_T, uint32_t CACHE_BLOCK_SIZE>
inline void CacheBlock<PHYSICAL_ADDRESS_T, CACHE_BLOCK_SIZE>::Invalidate()
{
/*		if(state != ST_INVALID)
	{
		cerr << "0x" << hex << base_physical_addr << dec << ": ";
		switch(state)
		{
			case ST_INVALID: cerr << "(I)"; break;
			case ST_SHARED: cerr << "(S)"; break;
			case ST_EXCLUSIVE: cerr << "(E)"; break;
			case ST_MODIFIED: cerr << "(M)"; break;
		}
		cerr << "-[Inv]->(I)" << endl;
	}*/
	state = ST_INVALID;
	base_physical_addr = 0;
	memset(data, 0, CACHE_BLOCK_SIZE);
}

template <class PHYSICAL_ADDRESS_T, uint32_t CACHE_BLOCK_SIZE>
inline Action CacheBlock<PHYSICAL_ADDRESS_T, CACHE_BLOCK_SIZE>::GetAction(Event ev)
{
	return actions[ev][state];
}

template <class PHYSICAL_ADDRESS_T, uint32_t CACHE_BLOCK_SIZE>
inline void CacheBlock<PHYSICAL_ADDRESS_T, CACHE_BLOCK_SIZE>::UpdateState(Event ev, bool shared)
{
/*		cerr << "0x" << hex << base_physical_addr << dec << ": ";
	switch(state)
	{
		case ST_INVALID: cerr << "(I)"; break;
		case ST_SHARED: cerr << "(S)"; break;
		case ST_EXCLUSIVE: cerr << "(E)"; break;
		case ST_MODIFIED: cerr << "(M)"; break;
	}
	cerr << "-[";
	switch(ev)
	{
		case EvPrRdHit: cerr << "PrRdHit"; break;
		case EvPrWrHit: cerr << "PrWrHit"; break;
		case EvPrRdMiss: cerr << "PrRdMiss"; break;
		case EvPrWrMiss: cerr << "PrWrMiss"; break;
		case EvBusRdX: cerr << "BusRdX"; break;
		case EvBusRd: cerr << "BusRd"; break;
	}
	switch(actions[ev][state])
	{
		case AcNone: break;
		case AcBusRd: cerr << "/BusRd"; break;
		case AcBusRdX: cerr << "/BusRdX"; break;
		case AcWB: cerr << "/WB"; break;
	}

	if(shared)
		cerr << "(S)";
	else
		cerr << "(!S)";
	cerr << "]->";*/
	state = transitions[shared][ev][state];
/*		switch(state)
	{
		case ST_INVALID: cerr << "(I)"; break;
		case ST_SHARED: cerr << "(S)"; break;
		case ST_EXCLUSIVE: cerr << "(E)"; break;
		case ST_MODIFIED: cerr << "(M)"; break;
		case ST_UNKNOWN: cerr << "(unknown)"; break;
	}
	cerr << endl;*/
/*
	if(state == ST_UNKNOWN)
	{
		cerr << "Unbound result" << endl; cerr.flush();
		*(int *) 0 = 0;
	}*/
}

template <class PHYSICAL_ADDRESS_T, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
Cache<PHYSICAL_ADDRESS_T, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::Cache(const char *name, unisim::component::cxx::cache::CacheInterface<PHYSICAL_ADDRESS_T> *next_level_cache, unisim::component::cxx::cache::BusInterface<PHYSICAL_ADDRESS_T> *bus_interface, Object *parent)
	: Object(name, parent), unisim::component::cxx::cache::Cache<CacheBlock<PHYSICAL_ADDRESS_T, CACHE_BLOCK_SIZE>, PHYSICAL_ADDRESS_T, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>(name, next_level_cache, bus_interface, parent)
{
}

} // end of namespace mesi
} // end of namespace cache
} // end of namespace cxx
} // end of namespace component
} // end of namespace full_system

#endif

