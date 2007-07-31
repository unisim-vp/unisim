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
 
#ifndef __UNISIM_COMPONENT_CXX_CACHE_MESI_CACHE_HH__
#define __UNISIM_COMPONENT_CXX_CACHE_MESI_CACHE_HH__

#include <unisim/component/c++/cache/cache.hh>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace cache {
namespace mesi {

using unisim::kernel::service::Object;

// States of the MESI cache coherence protocol
typedef enum
{
	ST_INVALID = 0,		// Invalid   (I)
	ST_SHARED = 1,		// Shared    (S)
	ST_EXCLUSIVE = 2,	// Exclusive (E)
	ST_MODIFIED = 3,	// Modified  (M)
	ST_UNKNOWN = 4
} CacheBlockState;

extern Action actions[5][4];
extern CacheBlockState transitions[2][5][4];

template <class PHYSICAL_ADDRESS_T, uint32_t CACHE_BLOCK_SIZE>
class CacheBlock
{
public:
	CacheBlockState state;
	PHYSICAL_ADDRESS_T base_physical_addr;
	CacheBlock *prev;			/*< previous block (unused if cache associativity is 1) */
	CacheBlock *next;			/*< next entry (unused if cache associativity is 1) */
	uint8_t data[CACHE_BLOCK_SIZE];	/*< cache block content */

	CacheBlock();

	inline void Zero()
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
	;

	inline void Write(uint32_t offset, const void *buffer, uint32_t size)
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
	;

	inline void Read(uint32_t offset, void *buffer, uint32_t size)
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
	;

	inline bool IsValid()
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
	;

	inline bool IsModified()
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
	;

	inline bool IsShared()
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
	;

	inline void SetModified()
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
	;

	inline void Invalidate()
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
	;

	inline Action GetAction(Event ev)
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
	;

	inline void UpdateState(Event ev, bool shared)
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
	;

};

template <class PHYSICAL_ADDRESS_T, uint32_t CACHE_SIZE, uint32_t CACHE_BLOCK_SIZE, uint32_t CACHE_ASSOCIATIVITY, ReplacementPolicy REPLACEMENT_POLICY>
class Cache : public unisim::component::cxx::cache::Cache<CacheBlock<PHYSICAL_ADDRESS_T, CACHE_BLOCK_SIZE>, PHYSICAL_ADDRESS_T, CACHE_SIZE, CACHE_BLOCK_SIZE, CACHE_ASSOCIATIVITY, REPLACEMENT_POLICY>
{
public:
	Cache(const char *name, unisim::component::cxx::cache::CacheInterface<PHYSICAL_ADDRESS_T> *next_level_cache, unisim::component::cxx::cache::BusInterface<PHYSICAL_ADDRESS_T> *bus_interface, Object *parent = 0);
};

} // end of namespace mesi
} // end of namespace cache
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif

