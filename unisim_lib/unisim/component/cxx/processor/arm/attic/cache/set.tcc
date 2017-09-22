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
 * Authors: Adriana Carloganu (adriana.carloganu@cea.fr)
 *          Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CACHE_SET_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CACHE_SET_TCC__

#include <stdlib.h>
#include <time.h>
#include "unisim/component/cxx/processor/arm/cache/line.tcc"

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace cache {

#if (defined(__GNUC__) && (__GNUC__ >= 3))
#define INLINE __attribute__((always_inline))
#else
#define INLINE 
#endif

template<class CONFIG>
Set<CONFIG>::Set()
{
  Invalidate();
}

template<class CONFIG>
Set<CONFIG>::~Set()
{

}

/* Arm Replacement Strategy uses the "two typical replacement strategies" described in ARM DDI 0100E/B5-7:
 *   - Random replacement
 *   - Round-robin replacement
 * only if all lines in a set are used (valid)
 * If any line in set invalid, this will be the choice for allocation
 * When a line is freed (invalidated) by freeLine(), it will also be elected for next allocation
*/ 

template<class CONFIG>
void 
Set<CONFIG> ::
UpdateReplacementPolicy(uint32_t lock, uint32_t index,  uint32_t way) {

	if(way > CONFIG::ASSOCIATIVITY){
		return;
	}

	// way <= CONFIG::ASSOCIATIVITY
	if(lock == 1){
		// force next allocation to block specified by index
		next_alloc = index;
		this->base = index;
		return;
	}
  
	// lock == 0
	uint32_t i;
	// if a free block in unlocked area, choose it for next allocation
	if(free_line){
		for(i = base; i < CONFIG::ASSOCIATIVITY; i++){
			if(!line[i].IsValid()){
				next_alloc = i;
				return;
			}
		}
	}
	// no free block in unlocked area, use replacement policy
	uint32_t size = CONFIG::ASSOCIATIVITY - base;
	switch(CONFIG::REPLACEMENT_POLICY){
    case ROUNDROBIN_REPLACEMENT:    // robin-round replacement, ARM application
    	next_alloc = base + ((next_alloc + 1) % size);
    	break;
    default:    // random replacement, ARM application
    	srand((unsigned) time(0));
    	next_alloc = base + (rand() % size);
    	break;
    }
}

template<class CONFIG>
inline INLINE 
void 
Set<CONFIG> ::
Reset() {
	Invalidate();
}

template<class CONFIG>
inline INLINE 
void 
Set<CONFIG> ::
Invalidate() {
	uint32_t i;

	free_line = CONFIG::ASSOCIATIVITY;
	next_alloc = 0;
	head       = 0;
	base       = 0;

	for(i = 0; i < CONFIG::ASSOCIATIVITY; i++){
		line[i].Invalidate();
	}
}

template<class CONFIG>
inline INLINE 
void 
Set<CONFIG> ::
InvalidateLine(uint32_t way) {
	if(way < CONFIG::ASSOCIATIVITY){
		line[way].Invalidate();
		if((way >= base)  && (free_line < CONFIG::ASSOCIATIVITY)){
			free_line++;
		}
	}// else out of array size
}

template<class CONFIG>
inline INLINE 
void 
Set<CONFIG> ::
ReadData(uint32_t way, 
		void* buffer, 
		uint32_t offset, 
		uint32_t size) {
	if(way < CONFIG::ASSOCIATIVITY) {
		line[way].Read(buffer, offset, size);
	}
}

template<class CONFIG>
inline INLINE 
void 
Set<CONFIG> ::
WriteData(uint32_t way, 
		const void* buffer, 
		uint32_t offset, 
		uint32_t size) {
	if(way < CONFIG::ASSOCIATIVITY){
		line[way].Write(buffer, offset, size);
	}
}

template<class CONFIG>
inline INLINE 
void 
Set<CONFIG> ::
AllocateLine(uint32_t way, 
		typename CONFIG::address_t tag, 
		const void* buffer) {
	if(way < CONFIG::ASSOCIATIVITY){ 
		line[way].Allocate(tag, buffer);
		if(free_line > 0){ // allocate block could be done only in unlocked area
			free_line--;
		}
	}
}

template<class CONFIG>
inline INLINE 
void 
Set<CONFIG> ::
GetLine(typename CONFIG::address_t base_addr, 
		uint32_t& way, 
		bool &hit) {
	if((way = FindLine(base_addr)) < CONFIG::ASSOCIATIVITY){
		hit = true;
	} else {
		hit = false;
	}
}

template<class CONFIG>
inline INLINE 
void 
Set<CONFIG> ::
GetLineToReplace(uint32_t& way, 
		typename CONFIG::address_t& tag,
		void* buffer,
		bool &valid,
		bool &modified) {
	way     = next_alloc;
	tag     = line[way].Tag();
	valid = line[way].IsValid();
	modified = line[way].IsModified();
	if(modified) memcpy(buffer, line[way].Data(), CONFIG::LINELEN);
}

template<class CONFIG>
inline INLINE
void
Set<CONFIG> ::
ValidateLine(uint32_t way) {
	if(way < CONFIG::ASSOCIATIVITY) {
		line[way].Validate();
	}
}

template<class CONFIG>
inline INLINE
void
Set<CONFIG> ::
ModifyLine(uint32_t way) {
	if(way < CONFIG::ASSOCIATIVITY) {
		line[way].Modify();
	}
}

template<class CONFIG>
inline INLINE 
bool 
Set<CONFIG> ::
IsModifiedLine(uint32_t way) {
	if(way < CONFIG:: ASSOCIATIVITY){
		return line[way].IsModified();
	}
	return false;
}

template<class CONFIG>
inline INLINE 
bool 
Set<CONFIG> ::
IsValidLine(uint32_t way) {
	if(way < CONFIG:: ASSOCIATIVITY){
		return line[way].IsValid();
	}
	return false;
}

//----------------------------------------------------------------------------
// PRIVATE FUNCTIONS
//----------------------------------------------------------------------------
template<class CONFIG>
inline INLINE 
uint32_t 
Set<CONFIG> ::
FindLine(typename CONFIG::address_t tag) {
	uint32_t i;
	uint32_t way;

	for(i = 0 ; i < CONFIG::ASSOCIATIVITY; i++){
		way = (head + i) % CONFIG::ASSOCIATIVITY;
		if(line[way].IsValid() && (line[way].Tag() == tag)){
			return way;
		}
	}
	return CONFIG::ASSOCIATIVITY;
}

#undef INLINE

} // end of namespace cache
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
    
#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CACHE_SET_TCC__
