/*
 *  Copyright (c) 2009,
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

#include <unisim/component/cxx/processor/tms320/cpu.hh>
#include <unisim/util/endian/endian.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tms320 {

Register::Register()
	: lo_write_mask(0xffffffff)
	, lo(0)
	, hi(0)
{
}

void Register::SetLoWriteMask(uint32_t _lo_write_mask)
{
	lo_write_mask = _lo_write_mask;
}

RegisterDebugInterface::RegisterDebugInterface(const char *_name, unisim::component::cxx::processor::tms320::Register *_reg, bool _extended_precision)
	: name(_name)
	, reg(_reg)
	, extended_precision(_extended_precision)
{
}

RegisterDebugInterface::~RegisterDebugInterface()
{
}

const char *RegisterDebugInterface::GetName() const
{
	return name.c_str();
}

void RegisterDebugInterface::GetValue(void *buffer) const
{
	uint32_t lo = reg->GetLo();

	if(extended_precision)
	{
		// Packing lo and hi into the buffer
		uint8_t hi = reg->GetHi();
#if BYTE_ORDER == LITTLE_ENDIAN
		memcpy(buffer, &lo, sizeof(lo));
		memcpy((char *) buffer + sizeof(lo), &hi, sizeof(hi));
#else
		memcpy(buffer, &hi, sizeof(hi));
		memcpy((char *) buffer + sizeof(hi), &lo, sizeof(lo));
#endif
	}
	else
	{
		memcpy(buffer, &lo, sizeof(lo));
	}
}

void RegisterDebugInterface::SetValue(const void *buffer)
{
	uint32_t lo;

	if(extended_precision)
	{
		uint8_t hi;
		// Unpacking the buffer into lo and hi
#if BYTE_ORDER == LITTLE_ENDIAN
		memcpy(&lo, buffer, sizeof(lo));
		memcpy(&hi, (char *) buffer + sizeof(lo), sizeof(hi));
#else
		memcpy(&hi, buffer, sizeof(hi));
		memcpy(&lo, (char *) buffer + sizeof(hi), sizeof(lo));
#endif
		reg->SetHi(hi);
	}
	else
	{
		memcpy(&lo, buffer, sizeof(lo));
	}

	reg->SetLo(lo);
}

int RegisterDebugInterface::GetSize() const
{
	return extended_precision ? sizeof(uint32_t) + sizeof(uint8_t) : sizeof(uint32_t);
}

RegisterBitFieldDebugInterface::RegisterBitFieldDebugInterface(const char *_name, unisim::component::cxx::processor::tms320::Register *_reg, unsigned int _bit_offset, unsigned int _bit_size)
	: name(_name)
	, reg(_reg)
	, bit_offset(_bit_offset)
	, bit_size(_bit_size > 32 ? 32 : _bit_size)
{
}

RegisterBitFieldDebugInterface::~RegisterBitFieldDebugInterface()
{
}

const char *RegisterBitFieldDebugInterface::GetName() const
{
	return name.c_str();
}

void RegisterBitFieldDebugInterface::GetValue(void *buffer) const
{
	uint32_t lo = reg->GetLo();
	uint32_t value = (lo >> bit_offset) & ((1 << bit_size) - 1);

	if(bit_size <= 8) *(uint8_t *) buffer = value; else
	if(bit_size <= 16) *(uint16_t *) buffer = value; else *(uint32_t *) buffer = value;
}

void RegisterBitFieldDebugInterface::SetValue(const void *buffer)
{
	uint32_t value;

	if(bit_size <= 8) value = *(uint8_t *) buffer; else
	if(bit_size <= 16) value = *(uint16_t *) buffer; else value = *(uint32_t *) buffer;

	reg->SetLo((reg->GetLo() & ~(((1 << bit_size) - 1) << bit_offset)) | (value << bit_offset));
}

int RegisterBitFieldDebugInterface::GetSize() const
{
	return (bit_size + 7) / 8;
}


} // end of namespace tms320
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
