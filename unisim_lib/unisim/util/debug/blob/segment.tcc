/*
 *  Copyright (c) 2011,
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
 */

#ifndef __UNISIM_UTIL_DEBUG_BLOB_SEGMENT_TCC__
#define __UNISIM_UTIL_DEBUG_BLOB_SEGMENT_TCC__

#include <stdlib.h>
#include <string.h>

namespace unisim {
namespace util {
namespace debug {
namespace blob {

template <class MEMORY_ADDR>
Segment<MEMORY_ADDR>::Segment(Type _type, Attribute _attr, unsigned int _alignment, MEMORY_ADDR _addr, MEMORY_ADDR _size, MEMORY_ADDR _data_size, void *_data)
	: type(_type)
	, attr(_attr)
	, alignment(_alignment)
	, addr(_addr)
	, size(_size)
	, data_size(_data_size)
	, data(_data)
	, refcount(0)
{
	refcount = new unsigned int();
	*refcount = 0;
}

template <class MEMORY_ADDR>
Segment<MEMORY_ADDR>::Segment(const Segment<MEMORY_ADDR>& segment)
	: type(segment.type)
	, attr(segment.attr)
	, alignment(segment.alignment)
	, addr(segment.addr)
	, size(segment.size)
	, data_size(segment.data_size)
	, data(0)
	, refcount(0)
{
	refcount = new unsigned int();
	*refcount = 0;

	if(segment.data)
	{
		data = malloc(size);
		memcpy(data, segment.data, size);
	}
}

template <class MEMORY_ADDR>
Segment<MEMORY_ADDR>::~Segment()
{
	if(data)
	{
		free(data);
	}
	delete refcount;
}

template <class MEMORY_ADDR>
typename Segment<MEMORY_ADDR>::Type Segment<MEMORY_ADDR>::GetType() const
{
	return type;
}

template <class MEMORY_ADDR>
typename Segment<MEMORY_ADDR>::Attribute Segment<MEMORY_ADDR>::GetAttr() const
{
	return attr;
}

template <class MEMORY_ADDR>
unsigned int Segment<MEMORY_ADDR>::GetAlignment() const
{
	return alignment;
}

template <class MEMORY_ADDR>
MEMORY_ADDR Segment<MEMORY_ADDR>::GetAddr() const
{
	return addr;
}

template <class MEMORY_ADDR>
MEMORY_ADDR Segment<MEMORY_ADDR>::GetSize() const
{
	return size;
}

template <class MEMORY_ADDR>
MEMORY_ADDR Segment<MEMORY_ADDR>::GetDataSize() const
{
	return data_size;
}

template <class MEMORY_ADDR>
const void *Segment<MEMORY_ADDR>::GetData() const
{
	return data;
}

template <class MEMORY_ADDR>
void Segment<MEMORY_ADDR>::GetAddrRange(MEMORY_ADDR& min_addr, MEMORY_ADDR& max_addr) const
{
	min_addr = addr;
	max_addr = addr + size - 1;
}

template <class MEMORY_ADDR>
void Segment<MEMORY_ADDR>::Catch() const
{
	(*refcount)++;
}

template <class MEMORY_ADDR>
void Segment<MEMORY_ADDR>::Release() const
{
	if(--(*refcount) == 0)
	{
		delete this;
	}
}

} // end of namespace blob
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
