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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */
 
#include <unisim/component/cxx/processor/powerpc/ppc440/cpu.hh>
#include <stdlib.h>

#ifdef powerpc
#undef powerpc
#endif

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace ppc440 {

TimeBaseRegisterInterface::TimeBaseRegisterInterface(const char *_name, uint64_t *_value, Type _type) :
	name(_name),
	value(_value),
	type(_type)
{
}

TimeBaseRegisterInterface::~TimeBaseRegisterInterface()
{
}

const char *TimeBaseRegisterInterface::GetName() const
{
	return name.c_str();
}

void TimeBaseRegisterInterface::GetValue(void *buffer) const
{
	*(uint32_t *) buffer = (type == TB_LOW) ? (uint32_t) *value
	                                        : (uint32_t)(*value >> 32);
}

void TimeBaseRegisterInterface::SetValue(const void *buffer)
{
	*value = (type == TB_LOW) ? (*value & 0xffffffff00000000ULL) | (uint64_t)(*(uint32_t *) buffer)
	                          : (*value & 0x00000000ffffffffULL) | ((uint64_t)(*(uint32_t *) buffer) << 32);
}

int TimeBaseRegisterInterface::GetSize() const
{
	return 4;
}

TimeBaseRegisterView::TimeBaseRegisterView(const char *name, unisim::kernel::Object *owner, uint64_t& _storage, TimeBaseRegisterView::Type _type, const char *description)
	: unisim::kernel::VariableBase(name, owner, unisim::kernel::VariableBase::VAR_REGISTER, description)
	, storage(_storage)
	, type(_type)
{
}

TimeBaseRegisterView::~TimeBaseRegisterView()
{
}

const char *TimeBaseRegisterView::GetDataTypeName() const
{
	return "unsigned 64-bit integer";
}

TimeBaseRegisterView::operator bool () const
{
	return (bool) (type == TB_LOW) ? (uint32_t) storage : (uint32_t)(storage >> 32);
}

TimeBaseRegisterView::operator long long () const
{
	return (long long) (type == TB_LOW) ? (uint32_t) storage : (uint32_t)(storage >> 32);
}

TimeBaseRegisterView::operator unsigned long long () const
{
	return (unsigned long long) (type == TB_LOW) ? (uint32_t) storage : (uint32_t)(storage >> 32);
}

TimeBaseRegisterView::operator double () const
{
	return (double) (type == TB_LOW) ? (uint32_t) storage : (uint32_t)(storage >> 32);
}

TimeBaseRegisterView::operator std::string () const
{
	uint32_t value = (type == TB_LOW) ? (uint32_t) storage : (uint32_t)(storage >> 32);
	std::stringstream sstr;
	switch(GetFormat())
	{
		case FMT_DEFAULT:
		case FMT_HEX:
			sstr << "0x" << std::hex << value;
			break;
		case FMT_DEC:
			sstr << std::dec << value;
			break;
	}
	return sstr.str();
}

unisim::kernel::VariableBase& TimeBaseRegisterView::operator = (bool value)
{
	if(IsMutable())
	{
		storage = (type == TB_LOW) ? (storage & 0xffffffff00000000ULL) | (uint64_t) value
								: (storage & 0x00000000ffffffffULL) | ((uint64_t) value << 32);
		NotifyListeners();
	}
	return *this;
}

unisim::kernel::VariableBase& TimeBaseRegisterView::operator = (long long value)
{
	if(IsMutable())
	{
		storage = (type == TB_LOW) ? (storage & 0xffffffff00000000ULL) | (uint64_t) value
								: (storage & 0x00000000ffffffffULL) | ((uint64_t) value << 32);
		NotifyListeners();
	}
	return *this;
}

unisim::kernel::VariableBase& TimeBaseRegisterView::operator = (unsigned long long value)
{
	if(IsMutable())
	{
		storage = (type == TB_LOW) ? (storage & 0xffffffff00000000ULL) | (uint64_t) value
								: (storage & 0x00000000ffffffffULL) | ((uint64_t) value << 32);
		NotifyListeners();
	}
	return *this;
}

unisim::kernel::VariableBase& TimeBaseRegisterView::operator = (double value)
{
	if(IsMutable())
	{
		storage = (type == TB_LOW) ? (storage & 0xffffffff00000000ULL) | (uint64_t) value
								: (storage & 0x00000000ffffffffULL) | ((uint64_t) value << 32);
		NotifyListeners();
	}
	return *this;
}

unisim::kernel::VariableBase& TimeBaseRegisterView::operator = (const char * value)
{
	if(IsMutable())
	{
		uint32_t v = (uint32_t) (strcmp(value, "true") == 0) ? 1 : ((strcmp(value, "false") == 0) ? 0 : strtoull(value, 0, 0));
		storage = (type == TB_LOW) ? (storage & 0xffffffff00000000ULL) | (uint64_t) v
								: (storage & 0x00000000ffffffffULL) | ((uint64_t) v << 32);
		NotifyListeners();
	}
	return *this;
}

} // end of namespace ppc440
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

