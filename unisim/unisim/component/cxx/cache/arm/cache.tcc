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

namespace unisim {
namespace component {
namespace cxx {
namespace cache {
namespace arm {

template <class CONFIG>
Cache<CONFIG>::Cache(const char *name, ArmCacheInterface<ADDRESS> *_next_level_cache, BusInterface<ADDRESS> *_bus_interface, Object *parent) :
//   Object(name, parent),
//   Service<MemoryInterface<ADDRESS> >(name, parent),
//   Client<MemoryInterface<ADDRESS> >(name, parent),
//   Client<CachePowerEstimatorInterface>(name, parent),
//   Client<PowerModeInterface>(name, parent),
//   Service<PowerModeInterface>(name, parent),
//   to_mem("to-mem", this),
//   to_cpu("to-cpu", this),
//   power_estimator_import("power-estimator-import", this),
//   power_mode_import("power-mode-import", this),
//   power_mode_export("power-mode-export", this),
  next_level_cache(_next_level_cache),
  bus_interface(_bus_interface)
{
  NB_SETS = CONFIG::CACHE_SIZE / CONFIG::CACHE_BLOCK_SIZE / CONFIG::CACHE_ASSOCIATIVITY;
  PrReset();
}
//-----------------------------
template <class CONFIG>
Cache<CONFIG>::~Cache()
{
}

//-----------------------------
template <class CONFIG>
uint32_t Cache<CONFIG>::GetCacheSize()
{
  return CONFIG::CACHE_SIZE;
}

//-----------------------------
template <class CONFIG>
uint32_t Cache<CONFIG>::GetCacheAssociativity()
{
  return CONFIG::CACHE_ASSOCIATIVITY;
}

//-----------------------------
template <class CONFIG>
uint32_t Cache<CONFIG>::GetCacheBlockSize()
{
  return CONFIG::CACHE_BLOCK_SIZE;
}

//-----------------------------
template <class CONFIG>
void Cache<CONFIG>::Enable()
{
  enabled = true;
}

//-----------------------------
template <class CONFIG>
void Cache<CONFIG>::Disable()
{
  enabled = false;
}

//-----------------------------
template <class CONFIG>
bool Cache<CONFIG>::IsEnabled()
{
  return enabled;
}

//-----------------------------
template <class CONFIG>
void Cache<CONFIG>::PrReset()
{
  if(enabled){
    PrInvalidate();
    Disable();
  }
}
//-----------------------------
template <class CONFIG>
void Cache<CONFIG>::SetLock(uint32_t lock, uint32_t index)
{
  uint32_t i; 
  if(enabled){
    this->lock       = lock;
    this->lock_index = index;
    for(i = 0; i < NB_SETS; i++){
     // set all next_alloc to index 
     cache[i].UpdateReplacementPolicy(1 , index, index);
    }
  }
}

//-----------------------------
//invalidate all lines, including the locked-down
template <class CONFIG>
void Cache<CONFIG>::PrInvalidate()
{
  uint32_t i;
  if(enabled){
    for(i = 0; i < NB_SETS; i++){
      cache[i].Invalidate();
    }
    lock = 0;
    lock_index = 0;
  }// else not enabled
}

//-----------------------------
template <class CONFIG>
void Cache<CONFIG>::PrInvalidateSet(uint32_t index)
{
  cerr << "WARNING : " << __FILE__ << " : " << __FUNCTION__ << " : Not implemented" << endl;
}

//-----------------------------
// invalidate block in cache 
template <class CONFIG>
void Cache<CONFIG>::PrInvalidateBlock(ADDRESS addr)
{
  uint8_t*    buffer;
  uint32_t     index;
  uint32_t    offset;
  uint32_t       way;
  ADDRESS  base_addr;

  if(enabled){
    //find index for set, and tag for block
    decodeAddress(addr, base_addr, index, offset);
    cache[index].GetBlock(base_addr, way, (const void**)&buffer);

    // range of way is checked in cache_set
    cache[index].InvalidateBlock(way);
    cache[index].UpdateReplacementPolicy(lock, lock_index, way);
  }// else cache not enabled, nothing to do
}

//-----------------------------
//used by Arm Cache
template <class CONFIG>
void Cache<CONFIG>::PrInvalidateBlock(uint32_t index, uint32_t way)
{
  if(enabled && (index < NB_SETS)){
    // range of way is checked in cache_set
    cache[index].InvalidateBlock(way);
    cache[index].UpdateReplacementPolicy(lock, lock_index, way);
  } // else not enabled or index out of range, nothing to do
}

//-----------------------------
// if data modified, write it into memory, then invalidate block
// For coherence reason with FlushBlock( set, way) function,
// the flush action is not propagate through cache hierarchy
template <class CONFIG>
void Cache<CONFIG>::PrFlushBlock(ADDRESS addr)
{
  uint8_t*    buffer;
  uint32_t     index;
  uint32_t    offset;
  uint32_t       way;
  ADDRESS  base_addr;

  if(enabled){
    //find index for set, and tag for block
    decodeAddress(addr, base_addr, index, offset);

    // get block index in set and data to be written into memory
    // if block not in cache (miss) way = CACHE_ASSOCIATIVITY and buffer = NULL
    cache[index].GetBlock(base_addr, way, (const void**)&buffer);

    if(buffer){ // cache hit
      if(cache[index].IsModifiedBlock(way)){
        // should pass by next_level_cache for virtual addressed caches for address translation
        storeDataInNextLevel(base_addr, (const void*)(buffer), CONFIG::CACHE_BLOCK_SIZE);
      }// else no data to be written in memory 

      //invalidate block
      cache[index].InvalidateBlock(way);
      cache[index].UpdateReplacementPolicy(lock, lock_index, way);
    }// else miss, nothing to do in this cache
  }// else cache not enabled
}
//-----------------------------
// if data modified, write it into memory, then invalidate blocks in cache
// Could not invalidate block in next level cache as, if miss 
// there is no information to find block in cache hierarchy (address is not available)

template <class CONFIG>
void Cache<CONFIG>::PrFlushBlock(uint32_t index, uint32_t way)
{
  uint8_t*     buffer;
  ADDRESS   base_addr;
  ADDRESS  block_addr;

  if(enabled && (index < NB_SETS)){
    // range of way is checked in cache_set
    if(cache[index].IsModifiedBlock(way)){
      cache[index].GetBlock(way, block_addr, (const void**)&buffer);
      // should pass by next_level_cache for virtual addressed caches for address translation
      storeDataInNextLevel(base_addr, (const void*)(buffer), CONFIG::CACHE_BLOCK_SIZE);
    }
    cache[index].InvalidateBlock(way);
    cache[index].UpdateReplacementPolicy(lock, lock_index, way);
  }// else cache not enabled

}

//-----------------------------
// if data modified, write it into memory
template <class CONFIG>
void Cache<CONFIG>::PrCleanBlock(ADDRESS addr)
{
  uint8_t*    buffer;
  uint32_t     index;
  uint32_t    offset;
  uint32_t       way;
  ADDRESS  base_addr;

  if(enabled){
    //find index for set, and tag for block
    decodeAddress(addr, base_addr, index, offset);
    cache[index].GetBlock(base_addr, way, (const void**)&buffer);

    if(buffer){ // cache hit
      if(cache[index].IsModifiedBlock(way)){
         // should pass by next_level_cache for virtual addressed caches for address translation
         storeDataInNextLevel(base_addr, (const void*)(buffer), CONFIG::CACHE_BLOCK_SIZE);
         cache[index].SetBlockState(way, ST_EXCLUSIVE);
      }// else no data to be written in memory 
    }// else miss, nothing to do in this cache
  } // else cache not enabled
}
//-----------------------------
template <class CONFIG>
void Cache<CONFIG>::PrCleanBlock(uint32_t index, uint32_t way)
{
  uint8_t*     buffer;
  ADDRESS  block_addr;

  if(enabled && (index < NB_SETS)){
    // range of way is checked in cache_set
    if(cache[index].IsModifiedBlock(way)){
      cache[index].GetBlock(way, block_addr, (const void**)&buffer);
      // should pass by next_level_cache for virtual addressed caches for address translation
      storeDataInNextLevel(block_addr, (const void*)(buffer), CONFIG::CACHE_BLOCK_SIZE);
      cache[index].SetBlockState(way, ST_EXCLUSIVE);
    } // else not enabled or index out of range, nothing to do
  }
}

//-----------------------------
// fill with zeros the address addr
template <class CONFIG>
void Cache<CONFIG>::PrZeroBlock(ADDRESS addr)
{
  cerr << "WARNING : " << __FILE__ << " : " << __FUNCTION__ << " : Not implemented" << endl;
}

//-----------------------------
// Processor write data from memory
// Two behaviours: cache with no support for coherence, cache supporting snooping coherence
// 1. NO_COHERENCE support
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
// 2. SNOOP_COHERENCE support
//  A. HIT in cache:
//      - write data in cache
//      - update block state(WB = modified, WT = no change)
//      - update replacement policy
//      - broadcast block state modified (all other copies will be invalidated)
//      - cache WriteThrough=> write data into memory
//  B. MISS in cache:
//      i. Write allocate cache
//        - find a block to replace
//        - if block modified eq unique copy of data, write block into memory
//        - load new cache block and block state (Unknown = from main memory, shared = several copies, exclusiv = one other copy)
//        - write data in cache
//        - update block state (WB => modified, WT => exclusif)
//        - update replacement policy
//        - broadcast block state modified (all other copies will be invalidated)
//        - cache WriteThrough=> write data into memory
//     ii. Read allocate cache
//        - write data into memory
//        - broadcast blockstate modified (all copies must be invalidated)
// this function implements only not snooping cache (Arm like)
template <class CONFIG>
void Cache<CONFIG>::PrWrite(ADDRESS addr, const void *data, uint32_t size)
{
  uint8_t* buffer;
  uint32_t  index;
  uint32_t offset;
  uint32_t    way;

  ADDRESS           base_addr;
  ADDRESS           block_tag;
  DataBlockState  block_state;

  if(enabled){
    decodeAddress(addr, base_addr, index, offset);
    cache[index].GetBlock(base_addr, way, (const void**)&buffer);

    if(!buffer){ // miss
      if(CONFIG::CACHE_ALLOCATION_POLICY == READ_ALLOCATE){
        // do not allocate cache block for write miss, send write to next level
        storeDataInNextLevel(addr, data, size);
        //if(CONFIG::CACHE_COHERENCE == SNOOP_COHERENCE){// force invalidation of other possible copies
        //  broadcastBlockState(addr, ST_MOFIFIED);
        //}
        return;
      }else{ //(CONFIG::CACHE_ALLOCATION_POLICY == WRITE_ALLOCATE)

        // find block to replace
        cache[index].GetBlockToReplace(way, block_tag, (const void**)&buffer);
        if(cache[index].IsModifiedBlock(way)){
          // need to save data before block replacement
          storeDataInNextLevel(block_tag, (const void*)(buffer), CONFIG::CACHE_BLOCK_SIZE);
        }
        cache[index].InvalidateBlock(way);

        //get a pointer to the data to be written in cache
        uint8_t line[CONFIG::CACHE_BLOCK_SIZE];
        loadDataFromNextLevel(base_addr, line, CONFIG::CACHE_BLOCK_SIZE, block_state);

        // allocate block
        cache[index].AllocateBlock(way, base_addr, line, block_state);
        cache[index].UpdateReplacementPolicy(lock, lock_index, way);
      }
    }// else hit

    block_state = selectNextBlockState(block_state, WRITE_DATA);

    // block in cache, either hit or allocate on miss
    cache[index].WriteData(way, data, offset, size, block_state);
    cache[index].SetBlockState(way, block_state); // modified or exclusive

    if(CONFIG::CACHE_WRITE_POLICY == WRITE_THROUGH){
      storeDataInNextLevel(addr, data, size);
    }

//     if(CONFIG::CACHE_COHERENCE == SNOOP_COHERENCE){
//        broadcastBlockState(addr, ST_MODIFIED); // force invalidation of other possible copies
//     }

  }else{ // cache disable, send write to next level
    storeDataInNextLevel(addr, data, size);
  }
}

//-----------------------------
// Processor read data from memory
// Two behaviours: cache with no support for coherence, cache supporting snooping coherence
// 1. NO_COHERENCE support
//  A. HIT in cache:
//      - update replacement policy
//      - data returned from cache
//  B. MISS in cache: 
//      - find a block to replace
//      - if data in block to replace is modified, write data into memory
//      - load new cache block 
//      - update replacement policy
//      - data returned from cache
// 2. SNOOP_COHERENCE support
//  A. HIT in cache:
//      - update replacement policy
//      - data returned from cache
//  B. MISS in cache:
//      - find a block to replace
//      - if block modified eq unique copy of data, write block into memory
//      - load new cache block and block state (Unknown = from main memory, shared = several copies, exclusif = one other copy)
//      - update replacement policy
//      - update and broadcast new block state(Unknown=>exclusif, exclusif=>shared, shared=>shared)
//      - data returned from cache
// this function implements only not snooping cache (Arm like)
template <class CONFIG>
void Cache<CONFIG>::PrRead(ADDRESS addr, void *data, uint32_t size, DataBlockState& st)
{
  uint8_t* buffer;
  uint32_t  index;
  uint32_t offset;
  uint32_t    way;

  ADDRESS           base_addr;
  ADDRESS           block_tag;
  DataBlockState  block_state;
 
  if(enabled){
    // search for block in cache
    decodeAddress(addr, base_addr, index, offset);
    cache[index].GetBlock(base_addr, way, (const void**)&buffer);

    if(!buffer){ // miss, allways READ_ALLOCATE
      // find block to replace
      cache[index].GetBlockToReplace(way, block_tag, (const void**)&buffer);

      if(cache[index].IsModifiedBlock(way)){
        // need to save data before block replacement
        storeDataInNextLevel(block_tag, (const void*)(buffer), CONFIG::CACHE_BLOCK_SIZE);
      }
      cache[index].InvalidateBlock(way);

      //get data block to be written in cache and it state from next memory level
      uint8_t line[CONFIG::CACHE_BLOCK_SIZE];
      loadDataFromNextLevel(base_addr, line, CONFIG::CACHE_BLOCK_SIZE, st);

      // allocate block and update state
      cache[index].AllocateBlock(way, base_addr, line, st);
      cache[index].UpdateReplacementPolicy(lock, lock_index, way);

    }//else hit
    // here block is in cache anyway

    // select new block state, depending on cache write policy ans cache coherence support
    block_state = selectNextBlockState(st, READ_DATA);

    cache[index].SetBlockState(way, block_state);
    cache[index].ReadData(way, data, offset, size, st);

    // broadcast new block state eq exclusive or shared
    //if(CONFIG::CACHE_COHERENCE == SNOOP_COHERENCE){
    //    broadcastBlockState(addr, block_state);
    //}// no broadcast if no coherence support

  }else{ // cache disable, read from next level
    loadDataFromNextLevel(addr, data, size, st);
  }
}

//-----------------------------
template <class CONFIG>
void Cache<CONFIG>::PrReadX(ADDRESS addr, void *buffer, uint32_t size, DataBlockState& st)
{



}
//-----------------------------

template <class CONFIG>
void Cache<CONFIG>::BusInvalidateBlock(ADDRESS addr)
{
  cerr << "WARNING : " << __FILE__ << " : " << __FUNCTION__ << " : Not implemented" << endl;
}
//-----------------------------

template <class CONFIG>
void Cache<CONFIG>::BusFlushBlock(ADDRESS addr)
{
  cerr << "WARNING : " << __FILE__ << " : " << __FUNCTION__ << " : Not implemented" << endl;
}
//-----------------------------

// this function is used when snooping is supported, other wise a cache does not receive demands from the bus
// used in loadDataFromNextLevel when a block miss occurs in cache
template <class CONFIG>
void Cache<CONFIG>::BusRead(ADDRESS addr, void *data, uint32_t size, BusControl bc, DataBlockState& st)
{
//   uint8_t* buffer;
//   ADDRESS base_addr;
//   ADDRESS block_tag;
//   uint32_t  index;
//   uint32_t offset;
//   uint32_t    way;
//   CacheBlockState block_state;
// 
//   if(enabled){
//     decodeAddress(addr, base_addr, index, offset);
//     buffer = cache[index]->GetBlockDataIndex(base_addr, way);
// 
//     if(buffer){ // hit in this level
//       if(cache[index]->IsModifiedBlock(way){
//         storeDataInNextLevel(base_addr, buffer, CONFIG::CACHE_BLOCK_SIZE);
//         cache[index]->SetState(ST_EXCLUSIVE);
//       }
//       cache[index]->Read(way, data, size, st);
//     }else{ // miss
//       if(next_level_cache){
//         next_level_cache->BusRead(addr, data, size, bc, st);
//       }else{ // no other level and miss
//         st = ST_INVALID;
//       }
//     }
//   }else{ // cache disable
//     if(next_level_cache){
//       next_level_cache->BusRead(addr, data, size, bc, st);
//     }else{ // no other level 
//       st = ST_INVALID;
//     }
//   }
  cerr << "WARNING : " << __FILE__ << " : " << __FUNCTION__ << " : Not implemented" << endl;
}
//-----------------------------

template <class CONFIG>
void Cache<CONFIG>::BusReadX(ADDRESS addr, void *buffer, uint32_t size, BusControl bc, DataBlockState& st)
{
 cerr << "WARNING : " << __FILE__ << " : " << __FUNCTION__ << " : Not implemented" << endl;
}
//-----------------------------

template <class CONFIG>
void Cache<CONFIG>::BusWrite(ADDRESS addr, const void *buffer, uint32_t size)
{
  cerr << "WARNING : " << __FILE__ << " : " << __FUNCTION__ << " : Not implemented" << endl;
}

//-----------------------------
// PRIVATE FUNCTIONS
//-----------------------------
template <class CONFIG>
inline void Cache<CONFIG>::decodeAddress(ADDRESS addr, ADDRESS& base_addr, uint32_t& index, uint32_t& offset)
{
  // +-----------------+--------+--------+
  // |       tag       |  index | offset |
  // +-----------------+--------+--------+
  index     = (addr / CONFIG::CACHE_BLOCK_SIZE) % NB_SETS;
  base_addr = addr & (~(CONFIG::CACHE_BLOCK_SIZE - 1));
  offset    = addr & (CONFIG::CACHE_BLOCK_SIZE - 1);
}

//-----------------------------
template <class CONFIG>
void Cache<CONFIG>::loadDataFromNextLevel(ADDRESS addr, void* buffer, uint32_t size, DataBlockState& st)
{
  CacheStatus cs;
  if(next_level_cache){
    // Read the cache block from the next level cache
    next_level_cache->PrRead(addr, buffer, size, st);
  }else if(bus_interface){ //no next level cache, Read the data from memory
    bus_interface->BusRead(addr, buffer, size, BC_NONE, cs);
  }else{
    cerr << "WARNING : " << __FUNCTION__ << " : failed, NO CONNECTION" << endl;
    for(uint32_t i = 0; i < size; i++){
      ((uint8_t*)buffer)[i] = 0xaa;
    }
  }
}

//----------------------------------------------

template <class CONFIG>
void Cache<CONFIG>::storeDataInNextLevel(ADDRESS addr, const void* buffer, uint32_t size)
{
  if(next_level_cache){
    // Write into the next level cache
    next_level_cache->PrWrite(addr, buffer, size);
  }else if(bus_interface){
    // Write into memory
    bus_interface->BusWrite(addr, buffer, size, BC_NONE);
  }else{
    cerr << "WARNING : " << __FUNCTION__ << " : failed, NO CONNECTION" << endl;
  }
}

//----------------------------------------------
template <class CONFIG>
DataBlockState Cache<CONFIG>::selectNextBlockState(DataBlockState st, uint8_t op)
{
  DataBlockState next_state = st;

  switch(st){
   case ST_SHARED:
    if(op == WRITE_DATA){
      if(CONFIG::CACHE_WRITE_POLICY == WRITE_BACK){
        next_state = ST_MODIFIED;
      }else{ // write through, only one valid copie after modification
        next_state = ST_EXCLUSIVE;
      }
    }else{// else read data
      next_state = ST_SHARED;
    }
    break;
   case ST_EXCLUSIVE:
    if(op == WRITE_DATA){
      if(CONFIG::CACHE_WRITE_POLICY == WRITE_BACK){
        next_state = ST_MODIFIED;
      }else{ // write through
        next_state = ST_EXCLUSIVE;
      }
    }else{// else read data
      next_state = ST_SHARED;
    }
    break;
   case ST_UNKNOWN:
    if(op == WRITE_DATA){
      if(CONFIG::CACHE_WRITE_POLICY == WRITE_BACK){
        next_state = ST_MODIFIED;
      }else{ // write through
        next_state = ST_EXCLUSIVE;
      }
    }else{// read data
      next_state = ST_EXCLUSIVE;
    }
   default: // return same state
    break;
  }
  return next_state;
}

//-----------------------------
// template <class CONFIG>
// void Cache<CONFIG>::broadcastBlockState(ADDRESS addr, DataBlockState block_state)
// {
// 
// }
//------------------------------

} // end of namespace arm
} // end of namespace cache
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_CACHE_ARM_CACHE_TCC__
