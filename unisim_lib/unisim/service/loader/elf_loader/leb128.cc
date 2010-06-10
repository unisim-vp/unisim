/*
 *  Copyright (c) 2010,
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
 
#include <unisim/service/loader/elf_loader/leb128.hh>
#include <iostream>
#include <limits>
#include <vector>
#include <string.h>

namespace unisim {
namespace service {
namespace loader {
namespace elf_loader {

//static unsigned int GetByteSize(const std::vector<uint8_t>& leb128)
//{
//	unsigned int byte_size;
//	for(byte_size = 1; (byte_size < leb128.size()) && (leb128[byte_size] & 0x80); byte_size++);
//	return byte_size;
//}

static unsigned int GetByteSize(const uint8_t *leb128)
{
	unsigned int byte_size;
	for(byte_size = 1; leb128[byte_size] & 0x80; byte_size++);
	return byte_size;
}

static unsigned int GetBitLength(const uint8_t *leb128)
{
	return GetByteSize(leb128) * 7;
}

//DWARF_LEB128::DWARF_LEB128()
//	: leb128(0)
//{
//}

//DWARF_LEB128::DWARF_LEB128(const std::vector<uint8_t>& _leb128)
//	: leb128(0)
//{
//	if(_leb128)
//	{
//		unsigned int byte_size = GetByteSize(_leb128);
//		leb128 = new uint8_t[byte_size];
//		unsigned int byte_index;
//		for(byte_index = 0; byte_index < byte_size; byte_index++)
//		{
//			leb128[byte_index] = _leb128[byte_index];
//		}
//	}
//}

DWARF_LEB128::DWARF_LEB128(const uint8_t *_leb128)
	: leb128(0)
{
	if(_leb128)
	{
		unsigned int byte_size = unisim::service::loader::elf_loader::GetByteSize(_leb128);
		leb128 = new uint8_t[byte_size];
		memcpy(leb128, _leb128, byte_size);
	}
}

DWARF_LEB128::DWARF_LEB128(const DWARF_LEB128& _leb128)
	: leb128(0)
{
	if(_leb128.leb128)
	{
		unsigned int byte_size = _leb128.GetByteSize();
		leb128 = new uint8_t[byte_size];
		memcpy(leb128, _leb128.leb128, byte_size);
	}
}

DWARF_LEB128::~DWARF_LEB128()
{
	if(leb128)
	{
		delete[] leb128;
		leb128 = 0;
	}
}

DWARF_LEB128& DWARF_LEB128::operator = (const uint8_t *_leb128)
{
	if(leb128)
	{
		delete[] leb128;
		leb128 = 0;
	}

	if(_leb128)
	{
		unsigned int byte_size = unisim::service::loader::elf_loader::GetByteSize(_leb128);
		leb128 = new uint8_t[byte_size];
		memcpy(leb128, _leb128, byte_size);
	}
	
	return *this;
}

DWARF_LEB128& DWARF_LEB128::operator = (const DWARF_LEB128& _leb128)
{
	if(leb128)
	{
		delete[] leb128;
		leb128 = 0;
	}

	if(_leb128.leb128)
	{
		unsigned int byte_size = _leb128.GetByteSize();
		leb128 = new uint8_t[byte_size];
		memcpy(leb128, _leb128.leb128, byte_size);
	}
	
	return *this;
}

unsigned int DWARF_LEB128::GetByteSize() const
{
	return unisim::service::loader::elf_loader::GetByteSize(leb128);
}

unsigned int DWARF_LEB128::GetBitLength() const
{
	return unisim::service::loader::elf_loader::GetBitLength(leb128);
}

template <typename T>
bool DWARF_LEB128::Fit(const T *t) const
{
	return 8 * sizeof(T) >= GetBitLength();
}

template <typename T>
DWARF_LEB128::operator T() const
{
	T result = 0;
	unsigned int shift = 0;
	unsigned int size = 8 * sizeof(T);
	uint8_t *p = leb128;
	uint8_t byte;
	do
	{
		byte = *p++;
		result |= (byte & 0x7f) << shift;
		shift += 7;
	}
	while(byte & 0x80);

	if(std::numeric_limits<T>::is_signed)
	{
		if((shift < size) && (byte & 0x40))
		result |= -(1 << shift);
	}
	return result;
}

} // end of namespace elf_loader
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim

