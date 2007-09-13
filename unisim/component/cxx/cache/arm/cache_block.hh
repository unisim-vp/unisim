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

#ifndef __FS_PROCESSORS_ARM_CACHE_BLOCK_HH__
#define __FS_PROCESSORS_ARM_CACHE_BLOCK_HH__

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
class ArmCacheBlock{
  public:

    ArmCacheBlock();
    virtual ~ArmCacheBlock();

    inline typename CONFIG::ADDRESS Tag()
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
    ;

    inline const void* Data()
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
    ;

    inline DataBlockState State()
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
    ;

    inline bool IsModified()
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
    ;

    inline bool IsValid()
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
    ;

    inline bool IsExclusive()
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
    ;

    inline bool IsShared()
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
    ;

    inline void SetState(DataBlockState s)
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
    ;

    void Invalidate()
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
    ;

    void Read(void* buffer, uint32_t offset, uint32_t size, DataBlockState& cs)
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
    ;

    void Write(const void* buffer, uint32_t offset, uint32_t size, DataBlockState cs);
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
    ;

    void Allocate(typename CONFIG::ADDRESS addr, const void* buffer, DataBlockState cs)
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
    ;

  private:

    DataBlockState         state;
    typename CONFIG::ADDRESS tag;
    uint8_t data[CONFIG::CACHE_BLOCK_SIZE];
};

//----------------------------------------------------------------------------
template<class CONFIG>
ArmCacheBlock<CONFIG>::ArmCacheBlock()
{
  state = ST_INVALID;
  tag   = 0;
  memset(data, 0, CONFIG::CACHE_BLOCK_SIZE);
}

//----------------------------------------------------------------------------
template<class CONFIG>
ArmCacheBlock<CONFIG>::~ArmCacheBlock()
{

}

//----------------------------------------------------------------------------
template<class CONFIG>
inline typename CONFIG::ADDRESS ArmCacheBlock<CONFIG>::Tag()
{
  return tag;
}

//----------------------------------------------------------------------------
template<class CONFIG>
inline const void* ArmCacheBlock<CONFIG>::Data()
{
  return (const void*)data;
}

//----------------------------------------------------------------------------
template<class CONFIG>
inline DataBlockState ArmCacheBlock<CONFIG>::State()
{
  return state;
}
//----------------------------------------------------------------------------
template<class CONFIG>
inline bool ArmCacheBlock<CONFIG>::IsValid()
{
  if((state != ST_INVALID) && (state != ST_UNKNOWN)){
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
template<class CONFIG>
inline bool ArmCacheBlock<CONFIG>::IsModified()
{
  if(state == ST_MODIFIED){
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
template<class CONFIG>
inline bool ArmCacheBlock<CONFIG>::IsExclusive()
{
  if(state == ST_EXCLUSIVE){
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
template<class CONFIG>
inline bool ArmCacheBlock<CONFIG>::IsShared()
{
  if(state == ST_SHARED){
    return true;
  }
  return false;
}

//----------------------
template<class CONFIG>
inline void ArmCacheBlock<CONFIG>::SetState(DataBlockState s)
{
  state = s;
}

//----------------------------------------------------------------------------
template<class CONFIG>
inline void ArmCacheBlock<CONFIG>::Invalidate()
{
  state = ST_INVALID;
}

//----------------------------------------------------------------------------
template<class CONFIG>
inline void ArmCacheBlock<CONFIG>::Read(void* buffer, uint32_t offset, uint32_t size, DataBlockState& st)
{
  if((offset + size) < CONFIG::CACHE_BLOCK_SIZE){
    memcpy(buffer, data + offset, size);
    st = state;
  }// else read crosses cache block boundaries, ignore or message(?)
}

//----------------------------------------------------------------------------
template<class CONFIG>
inline void ArmCacheBlock<CONFIG>::Write(const void* buffer, uint32_t offset, uint32_t size, DataBlockState st)
{
  if((offset + size) < CONFIG::CACHE_BLOCK_SIZE){
    memcpy(data + offset, buffer, size);
    state = st;
  }// else write crosses cache block boundaries, ignore or message(?)
}

//----------------------------------------------------------------------------
template<class CONFIG>
inline void ArmCacheBlock<CONFIG>::Allocate(typename CONFIG::ADDRESS addr, const void* buffer, DataBlockState st)
{
  memcpy(data, buffer, CONFIG::CACHE_BLOCK_SIZE);
  tag   = addr;
  state = st;
}


//----------------------------------------------

} // end of arm namespace
} // end of processors namespace
} // end of full_system namespace

#endif // end of __FS_PROCESSORS_ARM_CACHE_HH__
