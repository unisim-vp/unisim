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
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CACHE_SET_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CACHE_SET_HH__

#include "unisim/component/cxx/processor/arm/cache/line.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace cache {

template <class CONFIG>
class Set{
  public:
    Set();
    virtual ~Set();

    virtual void UpdateReplacementPolicy(uint32_t lock, 
    		uint32_t index, 
    		uint32_t way);

    void Reset();
    void Invalidate();

    void InvalidateLine(uint32_t way);

    void ReadData(uint32_t way, 
    		void* buffer, 
    		uint32_t offset, 
    		uint32_t size);

    void WriteData(uint32_t way, 
    		const void* buffer, 
    		uint32_t offset, 
    		uint32_t size);

    void AllocateLine(uint32_t way, 
    		typename CONFIG::address_t tag, 
    		const void* buffer);

    void GetLine(typename CONFIG::address_t base_addr, 
    		uint32_t& way, 
    		bool &hit);
    void GetLineToReplace(uint32_t& way, 
    		typename CONFIG::address_t& tag, 
    		void* buffer,
    		bool &modified);

    bool IsValidLine(uint32_t way);

    bool IsModifiedLine(uint32_t way);

  private:
    uint32_t  base;
    uint32_t  free_line;
    uint32_t  next_alloc;
    uint32_t  head;
 
    Line<CONFIG> line[CONFIG::ASSOCIATIVITY];

    uint32_t FindLine(typename CONFIG::address_t tag);
};

} // end of namespace cache
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
    
#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CACHE_SET_HH__
