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
 
#include <unisim/component/cxx/processor/powerpc/mpc7447a/cpu.hh>
#include <stdlib.h>
#include <stdexcept>

#ifdef powerpc
#undef powerpc
#endif

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace mpc7447a {

VectorRegisterView::VectorRegisterView(const char *name, unisim::kernel::Object *owner, vr_t& _storage, const char *description)
	: unisim::kernel::VariableBase(name, owner, unisim::kernel::VariableBase::VAR_REGISTER, description)
	, storage(_storage)
{
}

VectorRegisterView::~VectorRegisterView()
{
}

const char *VectorRegisterView::GetDataTypeName() const
{
	return "128-bit vector register";
}

VectorRegisterView::operator bool () const
{
	return (storage.w[0] || storage.w[1] || storage.w[2] || storage.w[3]);
}

VectorRegisterView::operator long long () const
{
	switch(sizeof(long long))
	{
		case 1: return (int8_t)(uint8_t) storage.w[3];
		case 2: return (int16_t)(uint16_t) storage.w[3];
		case 4: return (int32_t) storage.w[3];
		case 8: return (int64_t) ((uint64_t) storage.w[2] << 32) | ((uint64_t) storage.w[3]);
		default: throw std::runtime_error("Internal error");
	}
	return 0;
}

VectorRegisterView::operator unsigned long long () const
{
	switch(sizeof(unsigned long long))
	{
		case 1: return (int8_t)(uint8_t) storage.w[3];
		case 2: return (int16_t)(uint16_t) storage.w[3];
		case 4: return (int32_t) storage.w[3];
		case 8: return (int64_t) ((uint64_t) storage.w[2] << 32) | ((uint64_t) storage.w[3]);
		default: throw std::runtime_error("Internal error");
	}
	return 0;
}

VectorRegisterView::operator double () const
{
	return (double) (((uint64_t) storage.w[2] << 32) | ((uint64_t) storage.w[3]));

}

VectorRegisterView::operator std::string () const
{
	std::stringstream sstr;
	sstr << "0x" << std::hex;
	sstr.fill('0');
	unsigned int i;
	for(i = 0; i < 4; i++)
	{
		sstr.width(8);
		sstr << storage.w[i];
	}
	return sstr.str();
}

unisim::kernel::VariableBase& VectorRegisterView::operator = (bool value)
{
	storage.w[0] = 0;
	storage.w[1] = 0;
	storage.w[2] = 0;
	storage.w[3] = (value ? 1 : 0);
	return *this;
}

unisim::kernel::VariableBase& VectorRegisterView::operator = (long long value)
{
	switch(sizeof(value))
	{
		case 1:
			storage.w[0] = (value < 0) ? 0xffffffffUL : 0;
			storage.w[1] = (value < 0) ? 0xffffffffUL : 0;
			storage.w[2] = (value < 0) ? 0xffffffffUL : 0;
			storage.w[3] = (int32_t)(int8_t) value;
			break;
		case 2:
			storage.w[0] = (value < 0) ? 0xffffffffUL : 0;
			storage.w[1] = (value < 0) ? 0xffffffffUL : 0;
			storage.w[2] = (value < 0) ? 0xffffffffUL : 0;
			storage.w[3] = (int32_t)(int16_t) value;
			break;
		case 4:
			storage.w[0] = (value < 0) ? 0xffffffffUL : 0;
			storage.w[1] = (value < 0) ? 0xffffffffUL : 0;
			storage.w[2] = (value < 0) ? 0xffffffffUL : 0;
			storage.w[3] = value;
			break;
		case 8:
			storage.w[0] = (value < 0) ? 0xffffffffUL : 0;
			storage.w[1] = (value < 0) ? 0xffffffffUL : 0;
			storage.w[2] = value >> 32;
			storage.w[3] = value;
			break;
		default: throw std::runtime_error("Internal error");
	}
	return *this;
}

unisim::kernel::VariableBase& VectorRegisterView::operator = (unsigned long long value)
{
	switch(sizeof(value))
	{
		case 1:
		case 2:
		case 4:
			storage.w[0] = 0;
			storage.w[1] = 0;
			storage.w[2] = 0;
			storage.w[3] = value;
			break;
		case 8: 
			storage.w[0] = 0;
			storage.w[1] = 0;
			storage.w[2] = value >> 32;
			storage.w[3] = value;
			break;
		default: throw std::runtime_error("Internal error");
	}
	return *this;
}

unisim::kernel::VariableBase& VectorRegisterView::operator = (double value)
{
	storage.w[0] = 0;
	storage.w[1] = 0;
	storage.w[2] = (uint32_t)((int64_t) value >> 32);
	storage.w[3] = (uint32_t) (int64_t) value;
	return *this;
}

unisim::kernel::VariableBase& VectorRegisterView::operator = (const char * value)
{
	// skip space, tabs
	char c;
	do
	{
		c = *(value++);
		if(!c) return *this;
	}
	while((c == ' ') || (c == '\t') || (c == '\n'));

	if(c != '0') return *this;
	
	c = *(value++);
	
	if((c != 'x') && (c != 'X')) return *this;
	
	unsigned int n = 0;
	do
	{
		c = *value;
		if((c < '0' || c > '9') && (c < 'a' || c > 'f') && (c < 'A' || c > 'F')) break; // not an hex digit
		n++;
		value++;
	}
	while(1);
	
	if(n > 32) n = 32; // keep only 32 low significant bits
	
	unsigned int i;
	for(i = 0; i < n; i++)
	{
		c = *(--value);
		uint32_t nibble = 0;
		if((c >= '0') && (c <= '9')) nibble = (c - '0');
		else if((c >= 'a') && (c <= 'z')) nibble = 10 + (c - 'a');
		else if((c >= 'A') && (c <= 'Z')) nibble = 10 + (c - 'A');
		unsigned int sh = 4 * (i % 8);
		unsigned int j = 3 - (i / 8);
		storage.w[j] = (storage.w[j] & ~(0xf << sh)) | (nibble << sh);
	}

	return *this;
}

} // end of namespace mpc7447a
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

