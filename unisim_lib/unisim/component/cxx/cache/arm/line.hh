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
 * 			Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
/****************************************************************
            CACHE Line for ARM processors
*****************************************************************/

#ifndef __UNISIM_COMPONENT_CXX_CACHE_ARM_LINE_HH__
#define __UNISIM_COMPONENT_CXX_CACHE_ARM_LINE_HH__

#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace cache {
namespace arm {

template <class CONFIG>
class Line{
  public:
    Line();
    virtual ~Line();

    typename CONFIG::address_t Tag();
    const void* Data();
    bool IsModified();
    bool IsValid();
    void Invalidate();
    void Read(void* buffer, typename CONFIG::address_t offset, uint32_t size);
    void Write(const void* buffer, typename CONFIG::address_t offset, uint32_t size);
    void Allocate(typename CONFIG::address_t addr, const void* buffer);
private:
	bool modified;
	bool valid;
    typename CONFIG::address_t tag;
    uint8_t data[CONFIG::LINELEN];
};

} // end of namespace arm
} // end of namespace cache
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_CACHE_ARM_LINE_HH__

