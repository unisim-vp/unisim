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
 
/****************************************************************
            CACHE set for ARM processors
*****************************************************************/

// !!!!!
// ***** the use of this class assumes that user check and insure the validity of function parameters (expecially way and buffer size)
// !!!!!
#ifndef __UNISIM_COMPONENT_CXX_CACHE_ARM_SET_HH__
#define __UNISIM_COMPONENT_CXX_CACHE_ARM_SET_HH__

namespace unisim {
namespace component {
namespace cxx {
namespace cache {
namespace arm {

#define INLINE \ 
	#if defined(__GNUC__) && (__GNUC__ >= 3) \
		__attribute__((always_inline)) \
	#endif

template <class CONFIG>
class Set{
  public:
    Set();
    virtual ~Set();

    virtual void UpdateReplacementPolicy(uint32_t lock, 
    		uint32_t base, 
    		uint32_t way);

    inline void Reset() INLINE;
    inline void Invalidate() INLINE;

    inline void InvalidateBlock(uint32_t way) INLINE;

    inline void ReadData(uint32_t way, 
    		void* buffer, 
    		uint32_t offset, 
    		uint32_t size,
    		DataBlockState& st) INLINE;

    inline void WriteData(uint32_t way, 
    		const void* buffer, 
    		uint32_t offset, 
    		uint32_t size, 
    		DataBlockState st) INLINE;

    inline void AllocateBlock(uint32_t way, 
    		typename CONFIG::address_t tag, 
    		const void* buffer, 
    		DataBlockState st) INLINE;

    inline void GetBlock(typename CONFIG::address_t base_addr, 
    		uint32_t& way, 
    		const void** buffer) INLINE;
    inline void GetBlockToReplace(uint32_t& way, 
    		typename CONFIG::address_t& tag, 
    		const void** buffer) INLINE;

    inline bool IsValidBlock(uint32_t way) INLINE;

    inline bool IsModifiedBlock(uint32_t way) INLINE;

    inline void SetBlockState(uint32_t way, 
    		DataBlockState st) INLINE;

    inline void GetBlockState(uint32_t way, 
    		DataBlockState& st) INLINE;

  private:
    uint32_t  base;
    uint32_t  free_line;
    uint32_t  next_alloc;
    uint32_t  head;
 
    typename CONFIG::CACHE_LINE block[CONFIG::CACHE_ASSOCIATIVITY];

    inline uint32_t findLine(typename CONFIG::address_t tag) INLINE;
};

#undef INLINE

} // end of namespace arm
} // end of namespace cache
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
    
#endif // __UNISIM_COMPONENT_CXX_CACHE_ARM_SET_HH__
