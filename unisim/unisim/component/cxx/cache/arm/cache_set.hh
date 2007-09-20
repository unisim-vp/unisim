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
 */
 
/****************************************************************
            CACHE set for ARM processors
*****************************************************************/

/* History:
 * 12/07/2007 Adriana: Creation.
 *
 */
// !!!!!
// ***** the use of this class assumes that user check and insure the validity of function parameters (expecially way and buffer size)
// !!!!!
#ifndef __FS_PROCESSORS_ARM_CACHE_SET_HH__
#define __FS_PROCESSORS_ARM_CACHE_SET_HH__

#include <cstdlib>
#include <math.h>
#include <string>

#include <inttypes.h>
#include <iostream>

#include <generic/cache/cache_config.hh>

namespace full_system {
namespace processors {
namespace arm {

using namespace full_system::generic::cache;
//-------------------------------------------------------------


template <class CONFIG>
class ArmCacheSet{
  public:
    ArmCacheSet();
    virtual ~ArmCacheSet();

    virtual void UpdateReplacementPolicy(uint32_t lock, uint32_t base, uint32_t way);

    inline void Reset()
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
    ;

    inline void Invalidate()
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
    ;

    inline void InvalidateBlock(uint32_t way)
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
    ;

    inline void ReadData(uint32_t way, void* buffer, uint32_t offset, uint32_t size, DataBlockState& st)
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
    ;

    inline void WriteData(uint32_t way, const void* buffer, uint32_t offset, uint32_t size, DataBlockState st)
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
    ;

    inline void AllocateBlock(uint32_t way, typename CONFIG::ADDRESS tag, const void* buffer, DataBlockState st)
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
    ;

    inline void GetBlock(typename CONFIG::ADDRESS base_addr, uint32_t& way, const void** buffer)
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
    ;

    inline void GetBlockToReplace(uint32_t& way, typename CONFIG::ADDRESS& tag, const void** buffer)
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
    ;

    inline bool IsValidBlock(uint32_t way)
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
    ;

    inline bool IsModifiedBlock(uint32_t way)
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
    ;

    inline void SetBlockState(uint32_t way, DataBlockState st)
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
    ;

    inline void GetBlockState(uint32_t way, DataBlockState& st)
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
    ;

  private:
    uint32_t  base;
    uint32_t  free_block;
    uint32_t  next_alloc;
    uint32_t  head;
 
    typename CONFIG::CACHE_BLOCK block[CONFIG::CACHE_ASSOCIATIVITY];

    inline uint32_t findBlock(typename CONFIG::ADDRESS tag)
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
    ;
};

//----------------------------------------------------------------------------
template<class CONFIG>
ArmCacheSet<CONFIG>::ArmCacheSet()
{
  Invalidate();
}

//----------------------------------------------------------------------------
template<class CONFIG>
ArmCacheSet<CONFIG>::~ArmCacheSet()
{

}

//----------------------------------------------------------------------------
/* Arm Replacement Strategy uses the "two typical replacement strategies" described in ARM DDI 0100E/B5-7:
 *   - Random replacement
 *   - Round-robin replacement
 * only if all lines in a set are used (valid)
 * If any line in set invalid, this will be the choice for allocation
 * When a line is freed (invalidated) by freeLine(), it will also be elected for next allocation
*/ 
template<class CONFIG>
void ArmCacheSet<CONFIG>::UpdateReplacementPolicy(uint32_t lock, uint32_t base,  uint32_t way)
{

  if(way > CONFIG::CACHE_ASSOCIATIVITY){
    return;
  }

  // way <= CONFIG::CACHE_ASSOCIATIVITY){
  if(lock == 1){
    // force next allocation to block specified by base
    next_alloc = base;
    this->base = base;
    return;
  }
  
  // lock == 0
  uint32_t i;
  // if a free block in unlocked area, choose it for next allocation
  if(free_block){
    for(i = base; i < CONFIG::CACHE_ASSOCIATIVITY; i++){
      if(!block[i].IsValid()){
        next_alloc = i;
        return;
      }
    }
  }
  // no free block in unlocked area, use relpacement policy
  uint32_t size = CONFIG::CACHE_ASSOCIATIVITY - base;
  switch(CONFIG::CACHE_REPLACEMENT_POLICY){
    case RP_RR:    // robin-round replacement, ARM application
      next_alloc = base + ((next_alloc + 1) % size);
      break;
    default:    // random replacement, ARM application
      srand((unsigned) time(0));
      next_alloc = base + (rand() % size);
      break;
    }
}

//----------------------------------------------------------------------------
template<class CONFIG>
inline void ArmCacheSet<CONFIG>::Reset()
{
  Invalidate();
}

//----------------------------------------------------------------------------
template<class CONFIG>
inline void ArmCacheSet<CONFIG>::Invalidate()
{
  uint32_t i;

  free_block = CONFIG::CACHE_ASSOCIATIVITY;
  next_alloc = 0;
  head       = 0;
  base       = 0;

  for(i = 0; i < CONFIG::CACHE_ASSOCIATIVITY; i++){
    block[i].Invalidate();
  }
}

//----------------------------------------------------------------------------
template<class CONFIG>
inline void ArmCacheSet<CONFIG>::InvalidateBlock(uint32_t way)
{
  if(way < CONFIG::CACHE_ASSOCIATIVITY){
    block[way].Invalidate();
    if((way >= base)  && (free_block < CONFIG::CACHE_ASSOCIATIVITY)){
      free_block++;
    }
  }// else out of array size
}

//----------------------------------------------------------------------------
template<class CONFIG>
inline void ArmCacheSet<CONFIG>::ReadData(uint32_t way, void* buffer, uint32_t offset, uint32_t size, DataBlockState& st)
{
  if(way < CONFIG::CACHE_ASSOCIATIVITY){
    block[way].Read(buffer, offset, size, st);
  }
}

//----------------------------------------------------------------------------
template<class CONFIG>
inline void ArmCacheSet<CONFIG>::WriteData(uint32_t way, const void* buffer, uint32_t offset, uint32_t size, DataBlockState st)
{
  if(way < CONFIG::CACHE_ASSOCIATIVITY){
    block[way].Write(buffer, offset, size, st);
  }
}

//----------------------------------------------------------------------------
template<class CONFIG>
inline void ArmCacheSet<CONFIG>::AllocateBlock(uint32_t way, typename CONFIG::ADDRESS tag, const void* buffer, DataBlockState st)
{
  if(way < CONFIG::CACHE_ASSOCIATIVITY){ 
    block[way].Allocate(tag, buffer, st);
    if(free_block > 0){ // allocate block could be done only in unlocked area
      free_block--;
    }
  }
}
//----------------------------------------------------------------------------

template<class CONFIG>
inline void ArmCacheSet<CONFIG>::GetBlock(typename CONFIG::ADDRESS base_addr, uint32_t& way, const void** buffer)
{
  if((way = findBlock(base_addr)) < CONFIG::CACHE_ASSOCIATIVITY){
    *buffer = block[way].Data();
  }else{
    *buffer = NULL;
  }
}

//----------------------------------------------------------------------------
template<class CONFIG>
inline void ArmCacheSet<CONFIG>::GetBlockToReplace(uint32_t& way, typename CONFIG::ADDRESS& tag, const void** buffer)
{
  way     = next_alloc;
  tag     = block[way].Tag();
  *buffer = block[way].Data();
}

//----------------------------------------------------------------------------
template<class CONFIG>
inline bool ArmCacheSet<CONFIG>::IsModifiedBlock(uint32_t way)
{
  if(way < CONFIG:: CACHE_ASSOCIATIVITY){
    return block[way].IsModified();
  }
  return false;
}
//----------------------------------------------------------------------------
template<class CONFIG>
inline bool ArmCacheSet<CONFIG>::IsValidBlock(uint32_t way)
{
  if(way < CONFIG:: CACHE_ASSOCIATIVITY){
    return block[way].IsValid();
  }
  return false;
}

//----------------------------------------------------------------------------
template<class CONFIG>
inline void ArmCacheSet<CONFIG>::SetBlockState(uint32_t way, DataBlockState st)
{
  if(way < CONFIG:: CACHE_ASSOCIATIVITY){
     block[way].SetState(st);
  }
}

//----------------------------------------------------------------------------
template<class CONFIG>
inline void ArmCacheSet<CONFIG>::GetBlockState(uint32_t way, DataBlockState& st)
{
  if(way < CONFIG:: CACHE_ASSOCIATIVITY){
     st = block[way].State();
  }
}
//----------------------------------------------------------------------------
// PRIVATE FUNCTIONS
//----------------------------------------------------------------------------
template<class CONFIG>
inline uint32_t ArmCacheSet<CONFIG>::findBlock(typename CONFIG::ADDRESS tag)
{
  uint32_t i;
  uint32_t way;

  for(i = 0 ; i < CONFIG::CACHE_ASSOCIATIVITY; i++){
    way = (head + i) % CONFIG::CACHE_ASSOCIATIVITY;
    if(block[way].IsValid() && (block[way].Tag() == tag)){
      return way;
    }
  }
  return CONFIG::CACHE_ASSOCIATIVITY;
}
//----------------------------------------------------------------------------


// template <class CACHE_HWParams_t, class VIRTUAL_ADDRESS>
// void ArmCache<CACHE_HWParams_t, VIRTUAL_ADDRESS>::accessAlignment(uint32_t& offset, uint32_t& nb_bytes)
// {
// 
//   switch(nb_bytes){
//     case 1:
//      // any offset is accepted
//      break;
//     case 2:
//      // offset bit 0 should be 0 
//      offset &= 0xfffffffe;
//      break;
//     case 3:
//      // limit and aline to half-word access
//      offset &= 0xfffffffe;
//      nb_bytes = 2;
//      break;
//     case 4:
//      // offset bit0 and bit 1 should be 0
//      offset &= 0xfffffffc;
//      break;
//     default:
//      // limit and align to word access 
//      offset &= 0xfffffffc;
//      nb_bytes = 4;
//      break;
//   }
// #if DEBUG_ARM_CACHE == 1
//   cout << "\t" << __FUNCTION__ << "(offset = " << offset << ", nb_bytes = " << nb_bytes << ") : ";
//   cout << "offset = " << offset << ", nb_bytes = " << nb_bytes << endl;
// #endif
// 
// }
//----------------------------------------------


//----------------------------------------------

} // end of arm namespace
} // end of processors namespace
} // end of full_system namespace

#endif // end of __FS_PROCESSORS_ARM_CACHE_HH__
