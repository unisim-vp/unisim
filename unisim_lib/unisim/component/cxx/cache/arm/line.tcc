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
 
#ifndef __UNISIM_COMPONENT_CXX_CACHE_ARM_LINE_TCC__
#define __UNISIM_COMPONENT_CXX_CACHE_ARM_LINE_TCC__

namespace unisim {
namespace component {
namespace cxx {
namespace cache {
namespace arm {

template<class CONFIG>
Line<CONFIG> ::
Line() :
		valid(false),
		modified(false),
		tag(0) {
	memset(data, 0, CONFIG::CACHE_BLOCK_SIZE);
}

template<class CONFIG>
Line<CONFIG> ::
~Line() {
}

template<class CONFIG>
inline 
typename CONFIG::address_t
Line<CONFIG> ::
Tag() {
	return tag;
}

template<class CONFIG>
inline 
const void* 
Line<CONFIG> ::
Data() {
	return (const void*)data;
}

template<class CONFIG>
inline 
bool 
Line<CONFIG> ::
IsValid() {
	return valid;
}

template<class CONFIG>
inline 
bool 
Line<CONFIG> ::
IsModified() {
	return valid && modifed;
}

template<class CONFIG>
inline 
void 
Line<CONFIG> ::
Invalidate() {
	valid = false;
}

template<class CONFIG>
inline 
void 
Line<CONFIG> ::
Read(void* buffer, typename CONFIG::address_t offset, uint32_t size) {
  if((offset + size) < CONFIG::CACHE_BLOCK_SIZE){
    memcpy(buffer, data + offset, size);
  }// else read crosses cache block boundaries, ignore or message(?)
}

template<class CONFIG>
inline 
void 
Line<CONFIG> ::
Write(const void* buffer, uint32_t offset, uint32_t size) {
  if((offset + size) < CONFIG::CACHE_BLOCK_SIZE) {
    memcpy(data + offset, buffer, size);
  }// else write crosses cache block boundaries, ignore or message(?)
}

template<class CONFIG>
inline 
void 
Line<CONFIG> ::
Allocate(typename CONFIG::ADDRESS addr, const void* buffer) {
  memcpy(data, buffer, CONFIG::CACHE_BLOCK_SIZE);
  tag   = addr;
  state = st;
}


} // end of namespace arm
} // end of namespace cache
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_CACHE_ARM_LINE_TCC__
