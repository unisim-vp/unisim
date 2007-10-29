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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CACHE_CACHE_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CACHE_CACHE_TCC__

#include "unisim/component/cxx/processor/arm/cache/set.tcc"

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace cache {

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
		CacheInterface<typename CONFIG::address_t> *_next_mem_level,
		Object *parent) :
	Object(name, parent),
	CacheInterfaceWithMemoryService<typename CONFIG::address_t>(name, parent),
	verbose_all(false),
	param_verbose_all("verbose-all", this, verbose_all),
	verbose_pr_read(false),
	param_verbose_pr_read("verbose-pr-read", this, verbose_pr_read),
	verbose_pr_write(false),
	param_verbose_pr_write("verbose-pr-write", this, verbose_pr_write),
	verbose_read_memory(false),
	param_verbose_read_memory("verbose-read-memory", this, verbose_read_memory),
	verbose_write_memory(false),
	param_verbose_write_memory("verbose-write-memory", this, verbose_write_memory),
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
	Object::SetupDependsOn(inherited::logger_import);
	PrReset();
}

template <class CONFIG>
bool 
Cache<CONFIG> ::
Setup() {
	if(verbose_all) {
		verbose_pr_read = true;
		verbose_pr_write = true;
		verbose_read_memory = true;
		verbose_write_memory = true;
	}
	
	if(CONFIG::DEBUG_ENABLE && HasVerbose() && !inherited::logger_import) {
		cerr << "WARNING("
			<< __FUNCTION__ << ":"
			<< __FILE__ << ":"
			<< __LINE__ << "): "
			<< "No Logger exists to generate the output messages" << endl;
	}
	
	if(CONFIG::NSETS != CONFIG::SIZE / CONFIG::LINELEN / CONFIG::ASSOCIATIVITY) {
		if(inherited::logger_import) {
			(*inherited::logger_import) << DebugError << LOCATION
				<< "Invalid configuration (NSETS)"
				<< Endl << EndDebugError;
		}
		return false;
	}
	
	if(next_mem_level == NULL) {
		if(inherited::logger_import) {
			(*inherited::logger_import) << DebugError << LOCATION
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
	if(inherited::logger_import) {
		(*inherited::logger_import) << DebugWarning << LOCATION
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
	if(inherited::logger_import) {
		(*inherited::logger_import) << DebugError << LOCATION
			<< "TODO"
			<< Endl << EndDebugError;
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
	if(inherited::logger_import) {
		(*inherited::logger_import) << DebugError << LOCATION
			<< "TODO"
			<< Endl << EndDebugError;
	}
	exit(-1);
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
	if(inherited::logger_import) {
		(*inherited::logger_import) << DebugError << LOCATION
			<< "TODO"
			<< Endl << EndDebugError;
	}
	exit(-1);
}

//-----------------------------
// invalidate block in cache 
template <class CONFIG>
void 
Cache<CONFIG> :: 
PrInvalidateBlock(address_t addr) {
	if(inherited::logger_import) {
		(*inherited::logger_import) << DebugError << LOCATION
			<< "TODO"
			<< Endl << EndDebugError;
	}
	exit(-1);
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
	if(inherited::logger_import) {
		(*inherited::logger_import) << DebugError << LOCATION
			<< "TODO"
			<< Endl << EndDebugError;
	}
	exit(-1);
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
	if(inherited::logger_import) {
		(*inherited::logger_import) << DebugError << LOCATION
			<< "TODO"
			<< Endl << EndDebugError;
	}
	exit(-1);
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
	if(inherited::logger_import) {
		(*inherited::logger_import) << DebugError << LOCATION
			<< "TODO"
			<< Endl << EndDebugError;
	}
	exit(-1);
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
	if(inherited::logger_import) {
		(*inherited::logger_import) << DebugError << LOCATION
			<< "TODO"
			<< Endl << EndDebugError;
	}
	exit(-1);
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
	if(inherited::logger_import) {
		(*inherited::logger_import) << DebugError << LOCATION
			<< "TODO"
			<< Endl << EndDebugError;
	}
	exit(-1);
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
	if(inherited::logger_import) {
		(*inherited::logger_import) << DebugError << LOCATION
			<< "TODO"
			<< Endl << EndDebugError;
	}
	exit(-1);
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
PrWrite(address_t addr, const uint8_t *data, uint32_t size) {
	uint8_t buffer[CONFIG::LINELEN];
	address_t tag;
	address_t set;
	address_t pos;
	address_t way;
	bool hit;
	
	if(enabled) {
		DecodeAddress(addr, tag, set, pos);
		if(VerbosePrWrite()) {
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "Write on 0x" << Hex << addr << Dec
				<< " of size " << size << Endl
				<< " - write data = " << Hex;
			for(unsigned int i = 0; i < size; i++) 
				(*inherited::logger_import) << (unsigned int)(((uint8_t *)data)[i]) << " ";
			(*inherited::logger_import)
				<< Endl 
				<< " - tag = 0x" << Hex << tag << Dec << Endl
				<< " - set = " << set << Endl
				<< " - pos = " << pos << Endl 
				<< EndDebugInfo;
		}
		if(pos + size > CONFIG::LINELEN) {
			if(CONFIG::DEBUG_ENABLE && inherited::logger_import) 
				(*inherited::logger_import) << DebugError << LOCATION
					<< "Trying to read out of the cache line bounds" << Endl
					<< "- address = 0x" << Hex << addr << Dec << Endl
					<< "- tag = 0x" << Hex << tag << Dec << Endl
					<< "- set = 0x" << Hex << set << Dec << "(" << set << ")" << Endl
					<< "- pos = 0x" << Hex << pos << Dec << "(" << pos << ")" << Endl
					<< "- size = " << size << Endl
					<< "- pos + size > line_lenght (" << pos << " + " << size << " > " << CONFIG::LINELEN << ")" << Endl
					<< EndDebugError;
			else 
				cerr << "ERROR(" << Object::GetName() << ":PrWrite): "
					<< "Trying to read out of the cache line bounds" << endl
					<< "- address = 0x" << hex << addr << dec << endl
					<< "- tag = 0x" << hex << tag << dec << endl
					<< "- set = 0x" << hex << set << dec << "(" << set << ")" << endl
					<< "- pos = 0x" << hex << pos << dec << "(" << pos << ")" << endl
					<< "- size = " << size << endl
					<< "- pos + size > line_lenght"
					<< " (" << pos << " + " << size << " > " << CONFIG::LINELEN << ")" << endl;
			exit(-1);
		}
		cache[set].GetLine(tag, way, hit);
		if(!hit) { // miss
			if(CONFIG::ALLOCATION_POLICY == READ_ALLOCATE) {
				if(VerbosePrWrite())
					(*inherited::logger_import) << DebugInfo << LOCATION
						<< "Write access to 0x" << Hex << addr << Dec
						<< " missed, sending write to the following memory level"
						<< Endl << EndDebugInfo;
				StoreDataInMem(addr, data, size);
				return;
			} else { // AllocationPolicy::WRITE_ALLOCATE
				if(VerbosePrWrite())
					(*inherited::logger_import) << DebugInfo << LOCATION
						<< "Write access to 0x" << Hex << addr << Dec
						<< " missed, refetching cache line"
						<< Endl << EndDebugInfo;
				// get the line that will be replaced
				bool valid;
				bool modified;
				address_t replace_address;
				address_t replace_tag;
				cache[set].GetLineToReplace(way, replace_tag, buffer, valid, modified);
				if(valid && modified) {
					replace_address = CodeAddress(replace_tag, set);
					if(VerbosePrWrite())
						(*inherited::logger_import) << DebugInfo << LOCATION
							<< "Replacing cache line with address 0x"
							<< Hex << replace_address << Dec << " with: " << Endl
							<< " - tag = 0x" << Hex << replace_tag << Dec << Endl
							<< " - set = " << set << Endl
							<< " - way = " << way << Endl
							<< " - valid = " << valid << Endl
							<< " - modified = " << modified
							<< Endl << EndDebugInfo;
					StoreDataInMem(replace_address, buffer, CONFIG::LINELEN);
				} else {
					if(VerbosePrWrite()) {
						replace_address = CodeAddress(replace_tag, set);
						(*inherited::logger_import) << DebugInfo << LOCATION
							<< "Replacing cache line with address 0x"
							<< Hex << replace_address << Dec << " with: " << Endl
							<< " - tag = 0x" << Hex << replace_tag << Dec << Endl
							<< " - set = " << set << Endl
							<< " - way = " << way << Endl
							<< " - valid = " << valid << Endl
							<< " - modified = " << modified
							<< Endl << EndDebugInfo;
					}
				}
				cache[set].InvalidateLine(way);
				
				if(VerbosePrWrite())
					(*inherited::logger_import) << DebugInfo << LOCATION
						<< "Refetching cache line with address 0x"
						<< Hex << addr << Dec
						<< Endl << EndDebugInfo;

				//get a pointer to the data to be written in cache
				LoadDataFromMem(addr & ~(typename CONFIG::address_t)(CONFIG::LINELEN - 1), buffer, CONFIG::LINELEN);
				if(VerbosePrWrite()) {
					(*inherited::logger_import) << DebugInfo << LOCATION
						<< "Fetched cache line:" << Endl
						<< " - addr = 0x" << Hex << addr << Dec << Endl
						<< " - data = " << Hex;
					for(unsigned int i = 0; i < CONFIG::LINELEN; i++) 
						(*inherited::logger_import) << (unsigned int)(buffer[i]) << " ";
					(*inherited::logger_import) << Dec << Endl
						<< EndDebugInfo;
				}

				// allocate block
				cache[set].AllocateLine(way, tag, buffer);
				cache[set].ValidateLine(way);
				cache[set].UpdateReplacementPolicy(lock, lock_index, way); // ???
				
			}
		}
		
		if(VerbosePrWrite()) {
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "Write addr = 0x" << Hex << addr << Dec
				<< " with size = " << size
				<< " (hit = " << hit << ", set = " << set 
				<< ", way = " << way << ", pos = " << pos << ")" << Endl
				<< " - data = " << Hex;
			for(unsigned int i = 0; i < size; i++)
				(*inherited::logger_import) << (unsigned int)(((uint8_t *)data)[i])
					<< " ";
			(*inherited::logger_import) << Dec << Endl;
		}
		// Once the line is ready, the line can be modified
		cache[set].WriteData(way, data, pos, size);
		if(VerbosePrWrite()) {
			uint8_t buffer[CONFIG::LINELEN];
			cache[set].ReadData(way, buffer, 0, CONFIG::LINELEN);
			(*inherited::logger_import) 
				<< " - modified line = " << Hex;
			for(unsigned int i = 0; i < CONFIG::LINELEN; i++)
				(*inherited::logger_import) << (unsigned int)(buffer[i])
					<< " ";
			(*inherited::logger_import) << Dec << Endl
				<< EndDebugInfo;
		}
		cache[set].ModifyLine(way);
		
		if(CONFIG::WRITE_POLICY == WRITE_THROUGH){
			if(VerbosePrWrite()) {
				(*inherited::logger_import) << DebugInfo << LOCATION
					<< "Applying write through policy, sending write to the next"
					<< " memory level (addr = 0x" << Hex << addr << Dec
					<< ", size = " << size << ")" << Endl
					<< EndDebugInfo;
			}
			StoreDataInMem(addr, data, size);
		}
	} else { // cache is disabled
		// store the data into memory
		if(VerbosePrWrite()) {
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "Cache disabled, forwarding "
				<< "write on 0x" << Hex << addr << Dec
				<< " of size " << size
				<< Endl << EndDebugInfo;
		}
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
PrRead(address_t addr, uint8_t *data, uint32_t size) {
	uint8_t buffer[CONFIG::LINELEN];
	address_t tag;
	address_t set;
	address_t pos;
	address_t way;
	bool hit;
	
	if(enabled) {
		DecodeAddress(addr, tag, set, pos);
		if(VerbosePrRead()) {
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "Read on 0x" << Hex << addr << Dec
				<< " of size " << size
				<< Endl 
				<< " - tag = 0x" << Hex << tag << Dec << Endl
				<< " - set = " << set << Endl
				<< " - pos = " << pos << Endl 
				<< EndDebugInfo;
		}
		if(pos + size > CONFIG::LINELEN) {
			if(CONFIG::DEBUG_ENABLE && inherited::logger_import) 
				(*inherited::logger_import) << DebugError << LOCATION
					<< "Trying to read out of the cache line bounds" << Endl
					<< "- address = 0x" << Hex << addr << Dec << Endl
					<< "- tag = 0x" << Hex << tag << Dec << Endl
					<< "- set = 0x" << Hex << set << Dec << "(" << set << ")" << Endl
					<< "- pos = 0x" << Hex << pos << Dec << "(" << pos << ")" << Endl
					<< "- size = " << size << Endl
					<< "- pos + size > line_lenght (" << pos << " + " << size << " > " << CONFIG::LINELEN << ")" << Endl
					<< EndDebugError;
			else
				cerr << "ERROR(" << Object::GetName() << ":PrRead): "
					<< "Trying to read out of the cache line bounds" << endl
					<< "- address = 0x" << hex << addr << dec << endl
					<< "- tag = 0x" << hex << tag << dec << endl
					<< "- set = 0x" << hex << set << dec << "(" << set << ")" << endl
					<< "- pos = 0x" << hex << pos << dec << "(" << pos << ")" << endl
					<< "- size = " << size << endl
					<< "- pos + size > line_lenght"
					<< " (" << pos << " + " << size << " > " << CONFIG::LINELEN << ")" << endl;
			exit(-1);
		}
		cache[set].GetLine(tag, way, hit);
		if(!hit) { // miss
			if(VerbosePrRead())
				(*inherited::logger_import) << DebugInfo << LOCATION
					<< "Read access to 0x" << Hex << addr << Dec
					<< " missed, refetching cache line"
					<< Endl << EndDebugInfo;
			// read operations always allocate
			bool valid;
			bool modified;
			address_t replace_address;
			address_t replace_tag;
			cache[set].GetLineToReplace(way, replace_tag, buffer, valid, modified);
			if(valid && modified) {
				replace_address = CodeAddress(replace_tag, set);
				if(VerbosePrRead())
					(*inherited::logger_import) << DebugInfo << LOCATION
						<< "Replacing cache line with address 0x"
						<< Hex << replace_address << Dec << " with: " << Endl
						<< " - tag = 0x" << Hex << replace_tag << Dec << Endl
						<< " - set = " << set << Endl
						<< " - way = " << way << Endl
						<< " - valid = " << valid << Endl
						<< " - modified = " << modified
						<< Endl << EndDebugInfo;
				StoreDataInMem(replace_address, buffer, CONFIG::LINELEN);
			} else {
				replace_address = CodeAddress(replace_tag, set);
				if(VerbosePrRead()) {
					(*inherited::logger_import) << DebugInfo << LOCATION
						<< "Replacing cache line with address 0x"
						<< Hex << replace_address << Dec << " with: " << Endl
						<< " - tag = 0x" << Hex << replace_tag << Dec << Endl
						<< " - set = " << set << Endl
						<< " - way = " << way << Endl
						<< " - valid = " << valid << Endl
						<< " - modified = " << modified
						<< Endl << EndDebugInfo;
				}
			}
			cache[set].InvalidateLine(way);
			
			if(VerbosePrRead())
				(*inherited::logger_import) << DebugInfo << LOCATION
					<< "Refetching cache line with address 0x"
					<< Hex << (addr & ~(typename CONFIG::address_t)(CONFIG::LINELEN - 1)) << Dec
					<< Endl << EndDebugInfo;
			//get a pointer to the data to be written in cache
			LoadDataFromMem(addr & ~(typename CONFIG::address_t)(CONFIG::LINELEN - 1), buffer, CONFIG::LINELEN);
			if(VerbosePrRead()) {
				(*inherited::logger_import) << DebugInfo << LOCATION
					<< "Fetched cache line:" << Endl
					<< " - addr = 0x" << Hex << addr << Dec << Endl
					<< " - data = " << Hex;
				for(unsigned int i = 0; i < CONFIG::LINELEN; i++) 
					(*inherited::logger_import) << (unsigned int)(buffer[i]) << " ";
				(*inherited::logger_import) << Dec << Endl
					<< EndDebugInfo;
			}
			
			// allocate block
			cache[set].AllocateLine(way, tag, buffer);
			cache[set].ValidateLine(way);
			cache[set].UpdateReplacementPolicy(lock, lock_index, way); // ???
			
			// perform the read
		}
		cache[set].ReadData(way, data, pos, size);
		if(VerbosePrRead()) {
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "Read addr = 0x" << Hex << addr << Dec
				<< " with size = " << size
				<< " (hit = " << hit << ", set = " << set 
				<< ", way = " << way << ", pos = " << pos << ")" << Endl
				<< " - data = " << Hex;
			for(unsigned int i = 0; i < size; i++)
				(*inherited::logger_import) << (unsigned int)(((uint8_t *)data)[i])
					<< " ";
			(*inherited::logger_import) << Dec << Endl
				<< EndDebugInfo;
		}
	} else { // cache is disabled
		if(VerbosePrRead()) {
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "Cache disabled, forwarding "
				<< "read on 0x" << Hex << addr << Dec
				<< " of size " << size
				<< Endl << EndDebugInfo;
		}
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
	if(CONFIG::MULTIPLIER == 2)
		index = (addr / CONFIG::LINELEN) & (CONFIG::NSETS - 1);
	else
		index = (addr / CONFIG::LINELEN) % CONFIG::NSETS;
	// tag = addr & (~(CONFIG::LINELEN - 1));
	tag = addr & (~(CONFIG::LINELEN - 1)); //((addr / CONFIG::LINELEN) / CONFIG::NSETS);
	// offset = addr & (CONFIG::LINELEN - 1);
	offset = addr & (CONFIG::LINELEN - 1);
}

template <class CONFIG>
inline INLINE
typename CONFIG::address_t
Cache<CONFIG> ::
CodeAddress(address_t tag, address_t index) {
	address_t addr = 0;
	addr = tag;
//	addr += index * CONFIG::LINELEN;
	return addr;
}

//-----------------------------
template <class CONFIG>
inline INLINE
void 
Cache<CONFIG> ::
LoadDataFromMem(address_t addr,
		uint8_t* buffer, 
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
StoreDataInMem(address_t addr, const uint8_t* buffer, uint32_t size) {
	next_mem_level->PrWrite(addr, buffer, size);	
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

template <class CONFIG>
void
Cache<CONFIG> ::
Reset() {
}

template <class CONFIG>
bool
Cache<CONFIG> ::
ReadMemory(address_t addr, void *data, uint32_t size) {
	address_t tag;
	address_t set;
	address_t pos;
	address_t way;
	bool hit;

	if(VerboseReadMemory()) {
		DecodeAddress(addr, tag, set, pos);
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "Read memory on address 0x" << Hex << addr << Dec
			<< " of size = " << size << Endl
			<< " - tag = 0x" << Hex << tag << Dec << Endl
			<< " - set = " << set << Endl
			<< " - pos = " << pos << Endl
			<< EndDebugInfo;
	}
	if(enabled) {
		for(uint32_t i = 0; i < size; i++) {
			DecodeAddress(addr + i, tag, set, pos);
			cache[set].GetLine(tag, way, hit);
			if(!hit) { // miss
				if(inherited::memory_import) {
					if(!inherited::memory_import->ReadMemory(addr + i, &(((uint8_t *)data)[i]), 1)) {
						if(VerboseReadMemory()) {
							(*inherited::logger_import) << DebugInfo << LOCATION
								<< "Failed read memory on address 0x" << Hex << addr << Dec
								<< " of size = " << size 
								<< ", because of error when performing ReadMemory on memory_import" << Endl
								<< EndDebugInfo;
						}
						return false;
					}
				} else {
					if(VerboseReadMemory()) {
						(*inherited::logger_import) << DebugInfo << LOCATION
							<< "Read memory on address 0x" << Hex << addr << Dec
							<< " of size = " << size 
							<< ", because memory_import is not connected" << Endl
							<< EndDebugInfo;
					}
					return false;
				}
			} else {
				cache[set].ReadData(way, &(((uint8_t *)data)[i]), pos, 1);
			}
		}
	} else { // cache is disabled
		// load data from memory
		if(inherited::memory_import) {
			if(!inherited::memory_import->ReadMemory(addr, data, size)) {
				if(VerboseReadMemory()) {
					(*inherited::logger_import) << DebugInfo << LOCATION
						<< "Failed read memory on address 0x" << Hex << addr << Dec
						<< " of size = " << size
						<< ", because of error when performing ReadMemory on memory_import" << Endl
						<< EndDebugInfo;
				}
				return false;
			}
		}
	}
	if(VerboseReadMemory()) {
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "Succeded read memory on address 0x" << Hex << addr << Dec
			<< " of size = " << size << " :" << Endl
			<< " - data =" << Hex;
		for(unsigned int i = 0; i < size; i++)
			(*inherited::logger_import) << " " << (unsigned int)(((uint8_t *)data)[i]);
		(*inherited::logger_import) << Dec << Endl << EndDebugInfo;
	}
	return true;
}

template <class CONFIG>
bool 
Cache<CONFIG> ::
WriteMemory(address_t addr, const void *buffer, uint32_t size) {
	address_t tag;
	address_t set;
	address_t pos;
	address_t way;
	bool hit;

	if(VerboseWriteMemory()) {
		DecodeAddress(addr, tag, set, pos);
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "Write memory on address 0x" << Hex << addr << Dec
			<< " of size = " << size << " :" << Endl
			<< " - tag = 0x" << Hex << tag << Dec << Endl
			<< " - set = " << set << Endl
			<< " - pos = " << pos << Endl
			<< " -data =" << Hex;
		for(unsigned int i = 0; i < size; i++)
			(*inherited::logger_import) << " " << (unsigned int)(((uint8_t *)buffer)[i]);
		(*inherited::logger_import) << Dec << Endl << EndDebugInfo;
	}
	if(enabled) {
		for(uint32_t i = 0; i < size; i++) {
			DecodeAddress(addr + i, tag, set, pos);
			cache[set].GetLine(tag, way, hit);
			if(hit)
				cache[set].WriteData(way, &(((uint8_t *)buffer)[i]), pos, 1);
		}
	}
	if(inherited::memory_import) {
		if(!inherited::memory_import->WriteMemory(addr, buffer, size)) {
			if(VerboseWriteMemory())
				(*inherited::logger_import) << DebugInfo << LOCATION
					<< "Failed write memory on address 0x" << Hex << addr << Dec
					<< " of size = " << size
					<< ", because of error when performing WriteMemory on memory_import" << Endl
					<< EndDebugInfo;
			return false;
		}
	} else {
		if(VerboseWriteMemory())
			(*inherited::logger_import) << DebugInfo << LOCATION
				<< "Failed write memory on address 0x" << Hex << addr << Dec
				<< " of size = " << size
				<< ", because memory_import is not connected" << Endl
				<< EndDebugInfo;
		return false;		
	}
	
	if(VerboseWriteMemory()) {
		(*inherited::logger_import) << DebugInfo << LOCATION
			<< "Succeded write memory on address 0x" << Hex << addr << Dec
			<< " of size = " << size << " :" << Endl
			<< " -data =" << Hex;
		for(unsigned int i = 0; i < size; i++)
			(*inherited::logger_import) << " " << (unsigned int)(((uint8_t *)buffer)[i]);
		(*inherited::logger_import) << Dec << Endl << EndDebugInfo;
	}

	return true;
}

/**********************************************************************/
/* Verbose methods                                              START */
/**********************************************************************/

template <class CONFIG>
inline INLINE
bool 
Cache<CONFIG> ::
VerbosePrRead() {
	return CONFIG::DEBUG_ENABLE && verbose_pr_read && inherited::logger_import;
}

template <class CONFIG>
inline INLINE
bool 
Cache<CONFIG> ::
VerbosePrWrite() {
	return CONFIG::DEBUG_ENABLE && verbose_pr_write && inherited::logger_import;
}

template <class CONFIG>
inline INLINE
bool 
Cache<CONFIG> ::
VerboseReadMemory() {
	return CONFIG::DEBUG_ENABLE && verbose_read_memory && inherited::logger_import;
}

template <class CONFIG>
inline INLINE
bool 
Cache<CONFIG> ::
VerboseWriteMemory() {
	return CONFIG::DEBUG_ENABLE && verbose_write_memory && inherited::logger_import;
}

template <class CONFIG>
inline INLINE
bool
Cache<CONFIG> ::
HasVerbose() {
	return verbose_all || verbose_pr_read || verbose_pr_write;
}

/**********************************************************************/
/* Verbose methods                                                END */
/**********************************************************************/

#undef LOCATION
#undef INLINE

} // end of namespace cache
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CACHE_CACHE_TCC__
