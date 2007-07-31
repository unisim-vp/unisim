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
 
#ifndef __UNISIM_COMPONENT_CXX_CACHE_INSN_CACHE_TCC__
#define __UNISIM_COMPONENT_CXX_CACHE_INSN_CACHE_TCC__

#include <unisim/component/cxx/cache/cache.tcc>

namespace unisim {
namespace component {
namespace cxx {
namespace cache {
namespace insn {

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
	memset(data, 0, CACHE_BLOCK_SIZE);
	state = ST_VALID;
}

/* Write into block */
template <class PHYSICAL_ADDRESS_T, uint32_t CACHE_BLOCK_SIZE>
inline void CacheBlock<PHYSICAL_ADDRESS_T, CACHE_BLOCK_SIZE>::Write(uint32_t offset, const void *buffer, uint32_t size)
{
	memcpy(&data[offset], buffer, size);
	state = ST_VALID;
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
	return false;
}

template <class PHYSICAL_ADDRESS_T, uint32_t CACHE_BLOCK_SIZE>
inline bool CacheBlock<PHYSICAL_ADDRESS_T, CACHE_BLOCK_SIZE>::IsShared()
{
	return false;
}

template <class PHYSICAL_ADDRESS_T, uint32_t CACHE_BLOCK_SIZE>
inline void CacheBlock<PHYSICAL_ADDRESS_T, CACHE_BLOCK_SIZE>::SetModified()
{
}

template <class PHYSICAL_ADDRESS_T, uint32_t CACHE_BLOCK_SIZE>
inline void CacheBlock<PHYSICAL_ADDRESS_T, CACHE_BLOCK_SIZE>::Invalidate()
{
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
	state = transitions[ev][state];
}

template <class PHYSICAL_ADDRESS_T, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
Cache<PHYSICAL_ADDRESS_T, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>::Cache(const char *name, unisim::component::cxx::cache::CacheInterface<PHYSICAL_ADDRESS_T> *next_level_cache, unisim::component::cxx::cache::BusInterface<PHYSICAL_ADDRESS_T> *bus_interface, Object *parent)
	: Object(name, parent), unisim::component::cxx::cache::Cache<CacheBlock<PHYSICAL_ADDRESS_T, CACHE_BLOCK_SIZE>, PHYSICAL_ADDRESS_T, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>(name, next_level_cache, bus_interface, parent)
{
}

} // end of namespace insn
} // end of namespace cache
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif

