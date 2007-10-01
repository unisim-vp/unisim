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
 *       	Adriana Carloganu (adriana.carloganu@cea.fr)
 *          Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 *------------------------------------------------------------
 * Description: 
 *  Arm Cache: - used two replacement policies that could be selected by software
 *             - could be enabled/disabled by software
 */
 //-----------------------------------------------------------

#ifndef __UNISIM_COMPONENT_CXX_CACHE_ARM_CACHE_TCC__
#define __UNISIM_COMPONENT_CXX_CACHE_ARM_CACHE_TCC__

#include "unisim/component/cxx/cache/arm/set.tcc"

namespace unisim {
namespace component {
namespace cxx {
namespace cache {
namespace arm {

using unisim::kernel::service::Object;
using unisim::service::interfaces::File;
using unisim::service::interfaces::Function;
using unisim::service::interfaces::DebugInfo;
using unisim::service::interfaces::DebugWarning;
using unisim::service::interfaces::DebugError;
using unisim::service::interfaces::EndDebugInfo;
using unisim::service::interfaces::EndDebugWarning;
using unisim::service::interfaces::EndDebugError;
using unisim::service::interfaces::Hex;
using unisim::service::interfaces::Dec;
using unisim::service::interfaces::Endl;

#define LOCATION File << __FILE__ << Function << __FUNCTION__ \
	<< unisim::service::interfaces::Line << __LINE__

#if (defined(__GNUC__) && (__GNUC__ >= 3))
#define INLINE __attribute__((always_inline))
#else
#define INLINE 
#endif

template <class CONFIG>
Cache<CONFIG> ::
Cache(const char *name, 
		CacheInterface<address_t> *_next_mem_level,
		Object *parent) :
	Object(name, parent),
	Service<Memory<address_t> >(name, parent),
	Client<Memory<address_t> >(name, parent),
	Client<Logger>(name, parent),
	memory_export("memory_export", this),
	memory_import("memory_import", this),
	logger_import("logger_import", this),
	verbose_all(false),
	param_verbose_all("verbose-all", this, verbose_all),
	next_mem_level(_next_mem_level),
	lock(0),
	lock_index(0) {
//	NSETS(CONFIG::SIZE / 
//			CONFIG::LINELEN / 
//			CONFIG::ASSOCIATIVITY)
//   Client<CachePowerEstimatorInterface>(name, parent),
//   Client<PowerModeInterface>(name, parent),
//   Service<PowerModeInterface>(name, parent),
//   power_estimator_import("power-estimator-import", this),
//   power_mode_import("power-mode-import", this),
//   power_mode_export("power-mode-export", this),
	SetupDependsOn(logger_import);
	PrReset();
}

template <class CONFIG>
bool 
Cache<CONFIG> ::
Setup() {
	if(CONFIG::DEBUG_ENABLE && verbose_all && !logger_import) {
		cerr << "WARNING("
			<< __FUNCTION__ << ":"
			<< __FILE__ << ":"
			<< __LINE__ << "): "
			<< "No Logger exists to generate the output messages" << endl;
	}
	
	if(CONFIG::NSETS != CONFIG::SIZE / CONFIG::LINELEN / CONFIG::ASSOCIATIVITY) {
		if(logger_import) {
			(*logger_import) << DebugError << LOCATION
				<< "Invalid configuration (NSETS)"
				<< Endl << EndDebugError;
		}
		return false;
	}
	
	if(next_mem_level == NULL) {
		if(logger_import) {
			(*logger_import) << DebugError << LOCATION
				<< "No pointer to the next memory level was given"
				<< Endl << EndDebugError;
		}
		return false;
	}
	return true;
}

template <class CONFIG>
void 
Cache<CONFIG> ::
OnDisconnect() {
	if(logger_import) {
		(*logger_import) << DebugWarning << LOCATION
			<< "TODO"
			<< Endl << EndDebugError;
	}
	// TODO
}

//-----------------------------
template <class CONFIG>
Cache<CONFIG> ::
~Cache() {
}

template <class CONFIG>
void 
Cache<CONFIG> ::
SetLock(uint32_t lock, uint32_t index) {
	uint32_t i;
	if(enabled){
		this->lock       = lock;
		this->lock_index = index;
		for(i = 0; i < CONFIG::NSETS; i++){
			// set all next_alloc to index
			// UpdateReplacementPolicy is called with lock = 1 to force the next replacement,
			//   once the first replacement has happened the lock parameter will be used
			//   to update the replacement policy
			cache[i].UpdateReplacementPolicy(1, index, index);
	    }
	}
}	

//-----------------------------
template <class CONFIG>
inline INLINE
uint32_t 
Cache<CONFIG> ::
GetCacheSize() {
	return CONFIG::SIZE;
}

//-----------------------------
template <class CONFIG>
inline INLINE
uint32_t 
Cache<CONFIG> ::
GetCacheAssociativity() {
	return CONFIG::ASSOCIATIVITY;
}

//-----------------------------
template <class CONFIG>
inline INLINE
uint32_t 
Cache<CONFIG> ::
GetCacheLineSize() {
	return CONFIG::LINELEN;
}

//-----------------------------
template <class CONFIG>
inline INLINE
void 
Cache<CONFIG> ::
Enable() {
	enabled = true;
}

//-----------------------------
template <class CONFIG>
inline INLINE
void 
Cache<CONFIG> ::
Disable() {
	enabled = false;
}

//-----------------------------
template <class CONFIG>
inline INLINE
bool
Cache<CONFIG> ::
IsEnabled() {
	return enabled;
}

//-----------------------------
template <class CONFIG>
void
Cache<CONFIG> ::
PrReset() {
	if(logger_import) {
		(*logger_import) << DebugError << LOCATION
			<< "TODO"
			<< Endl << EndDebugError;
		exit(-1);
	}
//	if(enabled){
//		PrInvalidate();
//		Disable();
//	}
}

//-----------------------------
//invalidate all lines, including the locked-down
template <class CONFIG>
void 
Cache<CONFIG> ::
PrInvalidate() {
	if(logger_import) {
		(*logger_import) << DebugError << LOCATION
			<< "TODO"
			<< Endl << EndDebugError;
		exit(-1);
	}
//	uint32_t i;
//	if(enabled){
//		for(i = 0; i < NSETS; i++){
//			cache[i].Invalidate();
//		}
//		lock = 0;
//		lock_index = 0;
//	}// else not enabled
}

//-----------------------------
template <class CONFIG>
void 
Cache<CONFIG> ::
PrInvalidateSet(uint32_t index) {
	if(logger_import) {
		(*logger_import) << DebugError << LOCATION
			<< "TODO"
			<< Endl << EndDebugError;
		exit(-1);
	}
}

//-----------------------------
// invalidate block in cache 
template <class CONFIG>
void 
Cache<CONFIG> :: 
PrInvalidateBlock(address_t addr) {
	if(logger_import) {
		(*logger_import) << DebugError << LOCATION
			<< "TODO"
			<< Endl << EndDebugError;
		exit(-1);
	}
//	uint8_t*    buffer;
//	uint32_t     index;
//	uint32_t    offset;
//	uint32_t       way;
//	address_t  base_addr;
//
//	if(enabled){
//		//find index for set, and tag for block
//		decodeAddress(addr, base_addr, index, offset);
//		cache[index].GetBlock(base_addr, way, (const void**)&buffer);
//
//		// range of way is checked in cache_set
//		cache[index].InvalidateBlock(way);
//		cache[index].UpdateReplacementPolicy(lock, lock_index, way);
//	}// else cache not enabled, nothing to do
}

//-----------------------------
//used by Arm Cache
template <class CONFIG>
void 
Cache<CONFIG> ::
PrInvalidateBlock(uint32_t index, uint32_t way) {
	if(logger_import) {
		(*logger_import) << DebugError << LOCATION
			<< "TODO"
			<< Endl << EndDebugError;
		exit(-1);
	}
//	if(enabled && (index < NSETS)){
//		// range of way is checked in cache_set
//		cache[index].InvalidateBlock(way);
//		cache[index].UpdateReplacementPolicy(lock, lock_index, way);
//	} // else not enabled or index out of range, nothing to do
}

//-----------------------------
// if data modified, write it into memory, then invalidate block
// For coherence reason with FlushBlock( set, way) function,
// the flush action is not propagate through cache hierarchy
template <class CONFIG>
void 
Cache<CONFIG> ::
PrFlushBlock(address_t addr) {
	if(logger_import) {
		(*logger_import) << DebugError << LOCATION
			<< "TODO"
			<< Endl << EndDebugError;
		exit(-1);
	}
//	uint8_t*    buffer;
//	uint32_t     index;
//	uint32_t    offset;
//	uint32_t       way;
//	address_t  base_addr;
//
//	if(enabled){
//		//find index for set, and tag for block
//		decodeAddress(addr, base_addr, index, offset);
//
//		// get block index in set and data to be written into memory
//		// if block not in cache (miss) way = ASSOCIATIVITY and buffer = NULL
//		cache[index].GetBlock(base_addr, way, (const void**)&buffer);
//
//		if(buffer) { // cache hit
//			if(cache[index].IsModifiedBlock(way)) {
//				// should pass by next_level_cache for virtual addressed caches for address translation
//				storeDataInNextLevel(base_addr, (const void*)(buffer), CONFIG::LINELEN);
//			}// else no data to be written in memory 
//
//			//invalidate block
//			cache[index].InvalidateBlock(way);
//			cache[index].UpdateReplacementPolicy(lock, lock_index, way);
//		}// else miss, nothing to do in this cache
//	}// else cache not enabled
}

//-----------------------------
// if data modified, write it into memory, then invalidate blocks in cache
// Could not invalidate block in next level cache as, if miss 
// there is no information to find block in cache hierarchy (address is not available)

template <class CONFIG>
void 
Cache<CONFIG> ::
PrFlushBlock(uint32_t index, uint32_t way) {
	if(logger_import) {
		(*logger_import) << DebugError << LOCATION
			<< "TODO"
			<< Endl << EndDebugError;
		exit(-1);
	}
//	uint8_t*     buffer;
//	address_t   base_addr;
//	address_t  block_addr;
//
//	if(enabled && (index < NSETS)){
//		// range of way is checked in cache_set
//		if(cache[index].IsModifiedBlock(way)){
//			cache[index].GetBlock(way, block_addr, (const void**)&buffer);
//			// should pass by next_level_cache for virtual addressed caches for address translation
//			storeDataInNextLevel(base_addr, (const void*)(buffer), CONFIG::LINELEN);
//		}
//		cache[index].InvalidateBlock(way);
//		cache[index].UpdateReplacementPolicy(lock, lock_index, way);
//	}// else cache not enabled
//
}

//-----------------------------
// if data modified, write it into memory
template <class CONFIG>
void 
Cache<CONFIG> ::
PrCleanBlock(address_t addr) {
	if(logger_import) {
		(*logger_import) << DebugError << LOCATION
			<< "TODO"
			<< Endl << EndDebugError;
		exit(-1);
	}
//	uint8_t*    buffer;
//	uint32_t     index;
//	uint32_t    offset;
//	uint32_t       way;
//	address_t  base_addr;
//
//	if(enabled){
//		//find index for set, and tag for block
//		decodeAddress(addr, base_addr, index, offset);
//		cache[index].GetBlock(base_addr, way, (const void**)&buffer);
//
//		if(buffer){ // cache hit
//			if(cache[index].IsModifiedBlock(way)){
//				// should pass by next_level_cache for virtual addressed caches for address translation
//				storeDataInNextLevel(base_addr, (const void*)(buffer), CONFIG::LINELEN);
//				cache[index].SetBlockState(way, ST_EXCLUSIVE);
//			}// else no data to be written in memory 
//		}// else miss, nothing to do in this cache
//	} // else cache not enabled
}

//-----------------------------
template <class CONFIG>
void 
Cache<CONFIG> ::
PrCleanBlock(uint32_t index, uint32_t way) {
	if(logger_import) {
		(*logger_import) << DebugError << LOCATION
			<< "TODO"
			<< Endl << EndDebugError;
		exit(-1);
	}
//	uint8_t*     buffer;
//	address_t  block_addr;
//
//	if(enabled && (index < NSETS)){
//		// range of way is checked in cache_set
//		if(cache[index].IsModifiedBlock(way)){
//			cache[index].GetBlock(way, block_addr, (const void**)&buffer);
//			// should pass by next_level_cache for virtual addressed caches for address translation
//			storeDataInNextLevel(block_addr, (const void*)(buffer), CONFIG::LINELEN);
//			cache[index].SetBlockState(way, ST_EXCLUSIVE);
//		} // else not enabled or index out of range, nothing to do
//	}
}

//-----------------------------
// fill with zeros the address addr
template <class CONFIG>
void
Cache<CONFIG> ::
PrZeroBlock(address_t addr) {
	if(logger_import) {
		(*logger_import) << DebugError << LOCATION
			<< "TODO"
			<< Endl << EndDebugError;
		exit(-1);
	}
}

//-----------------------------
// Processor write data from memory
//  A. HIT in cache:
//      - write data in cache
//      - update block state (WB=>modified, WT=>no change)
//      - update replacement policy
//      - cache WriteThrough=> write data into memory
//  B. MISS in cache: 
//      i. Write allocate cache
//        - find a block to replace
//        - if data in block to replace is modified, write data into memory
//        - load new cache block 
//        - write data in cache
//        - update block state(WB=>modified, WT=>no change)
//        - update replacement policy
//        - cache WriteThrough=>write data into memory
//      ii. Read allocate cache
//        - write data into next memory level
template <class CONFIG>
void
Cache<CONFIG> ::
PrWrite(address_t addr, const void *data, uint32_t size) {
	uint8_t buffer[CONFIG::LINELEN];
	address_t tag;
	address_t set;
	address_t pos;
	address_t way;
	bool hit;
	
	if(enabled) {
		DecodeAddress(addr, tag, set, pos);
		if(pos + size > CONFIG::LINELEN) {
			if(CONFIG::DEBUG_ENABLE && logger_import) {
				(*logger_import) << DebugError << LOCATION
					<< "Trying to read out of the cache line bounds" << Endl
					<< "- address = 0x" << Hex << addr << Dec << Endl
					<< "- tag = 0x" << Hex << tag << Dec << Endl
					<< "- set = 0x" << Hex << set << Dec << "(" << set << ")" << Endl
					<< "- pos = 0x" << Hex << pos << Dec << "(" << pos << ")" << Endl
					<< "- size = " << size << Endl
					<< "- pos + size > line_lenght (" << pos << " + " << size << " > " << CONFIG::LINELEN << ")" << Endl
					<< EndDebugError;
				exit(-1);
			}
		}
		cache[set].GetLine(tag, way, hit);
		if(!hit) { // miss
			if(CONFIG::ALLOCATION_POLICY == READ_ALLOCATE) {
				StoreDataInMem(addr, data, size);
			} else { // AllocationPolicy::WRITE_ALLOCATE
				// get the line that will be replaced
				bool modified;
				address_t replace_address;
				cache[set].GetLineToReplace(way, replace_address, buffer, modified);
				if(modified)
					StoreDataInMem(replace_address, buffer, CONFIG::LINELEN);
				cache[set].InvalidateLine(way);
				
				//get a pointer to the data to be written in cache
				LoadDataFromMem(addr, buffer, CONFIG::LINELEN);

				// allocate block
				cache[set].AllocateLine(way, tag, buffer);
				cache[set].UpdateReplacementPolicy(lock, lock_index, way); // ???
				
			}
		}
		
		// Once the line is ready, the line can be modified
		cache[set].WriteData(way, data, pos, size);
		
		if(CONFIG::WRITE_POLICY == WRITE_THROUGH){
			StoreDataInMem(addr, data, size);
		}
	} else { // cache is disabled
		// store the data into memory
		StoreDataInMem(addr, data, size);
	}
}

//-----------------------------
// Processor read data from memory
//  A. HIT in cache:
//      - update replacement policy
//      - data returned from cache
//  B. MISS in cache: 
//      - find a block to replace
//      - if data in block to replace is modified, write data into memory
//      - load new cache block 
//      - update replacement policy
//      - data returned from cache
template <class CONFIG>
void 
Cache<CONFIG> ::
PrRead(address_t addr, void *data, uint32_t size) {
	uint8_t buffer[CONFIG::LINELEN];
	address_t tag;
	address_t set;
	address_t pos;
	address_t way;
	bool hit;
	
	if(enabled) {
		DecodeAddress(addr, tag, set, pos);
		if(pos + size > CONFIG::LINELEN) {
			if(CONFIG::DEBUG_ENABLE && logger_import) {
				(*logger_import) << DebugError << LOCATION
					<< "Trying to read out of the cache line bounds" << Endl
					<< "- address = 0x" << Hex << addr << Dec << Endl
					<< "- tag = 0x" << Hex << tag << Dec << Endl
					<< "- set = 0x" << Hex << set << Dec << "(" << set << ")" << Endl
					<< "- pos = 0x" << Hex << pos << Dec << "(" << pos << ")" << Endl
					<< "- size = " << size << Endl
					<< "- pos + size > line_lenght (" << pos << " + " << size << " > " << CONFIG::LINELEN << ")" << Endl
					<< EndDebugError;
				exit(-1);
			}
		}
		cache[set].ReadData(tag, data, size, hit);
		if(!hit) { // miss
			// read operations always allocate
			bool modified;
			address_t replace_address;
			cache[set].GetLineToReplace(way, replace_address, buffer, modified);
			if(modified)
				StoreDataInMem(replace_address, buffer, CONFIG::LINELEN);
			cache[set].InvalidateLine(way);
			
			//get a pointer to the data to be written in cache
			LoadDataFromMem(addr, buffer, CONFIG::LINELEN);

			// allocate block
			cache[set].AllocateLine(way, tag, buffer);
			cache[set].UpdateReplacementPolicy(lock, lock_index, way); // ???
			
			// perform the read
			cache[set].ReadData(tag, data, size, hit);
		}
	} else { // cache is disabled
		// load data from memory
		LoadDataFromMem(addr, data, size);
	}
}

//-----------------------------
// PRIVATE FUNCTIONS
//-----------------------------
template <class CONFIG>
inline INLINE 
void 
Cache<CONFIG> ::
DecodeAddress(address_t addr, 
		address_t& tag, 
		address_t& index, 
		address_t& offset) {
	// +-----------------+--------+--------+
	// |       tag       |  index | offset |
	// +-----------------+--------+--------+
	// index     = (addr / CONFIG::LINELEN) % NSETS;
	index = (addr / CONFIG::LINELEN) & (CONFIG::NSETS - 1);
	// tag = addr & (~(CONFIG::LINELEN - 1));
	tag = ((addr / CONFIG::LINELEN) / CONFIG::NSETS);
	// offset = addr & (CONFIG::LINELEN - 1);
	offset = addr & (CONFIG::LINELEN - 1);
}

//-----------------------------
template <class CONFIG>
inline INLINE
void 
Cache<CONFIG> ::
LoadDataFromMem(address_t addr,
		void* buffer, 
		uint32_t size) {
	next_mem_level->PrRead(addr, buffer, size);
//	
//	CacheStatus cs;
//	if(next_level_cache) {
//		// Read the cache block from the next level cache
//		next_level_cache->PrRead(addr, buffer, size, st);
//	}else 
//		if(bus_interface) { //no next level cache, Read the data from memory
//			bus_interface->BusRead(addr, buffer, size, BC_NONE, cs);
//		}else{
//			cerr << "WARNING : " << __FUNCTION__ << " : failed, NO CONNECTION" << endl;
//			for(uint32_t i = 0; i < size; i++){
//				((uint8_t*)buffer)[i] = 0xaa;
//			}
//		}
}

//----------------------------------------------

template <class CONFIG>
inline INLINE
void 
Cache<CONFIG> ::
StoreDataInMem(address_t addr, const void* buffer, uint32_t size) {
	next_mem_level->PrWrite(addr, buffer,size);
	
//	if(next_level_cache){
//		// Write into the next level cache
//		next_level_cache->PrWrite(addr, buffer, size);
//	}else 
//		if(bus_interface){
//			// Write into memory
//			bus_interface->BusWrite(addr, buffer, size, BC_NONE);
//		}else{
//			cerr << "WARNING : " << __FUNCTION__ << " : failed, NO CONNECTION" << endl;
//		}
}

#undef INLINE

} // end of namespace arm
} // end of namespace cache
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_CACHE_ARM_CACHE_TCC__
