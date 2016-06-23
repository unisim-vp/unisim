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
 
#include <unisim/component/cxx/processor/arm/simfloat.hh>
#include <unisim/util/simfloat/floating.tcc>
#include <stdlib.h>
#include <sstream>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace simfloat {

FloatingPointRegisterInterface::FloatingPointRegisterInterface(const char *_name, SoftDouble *_value)
  : name(_name)
  , value(_value)
{
}

FloatingPointRegisterInterface::~FloatingPointRegisterInterface()
{
}

const char *FloatingPointRegisterInterface::GetName() const
{
	return name.c_str();
}

void FloatingPointRegisterInterface::GetValue(void *buffer) const
{
	*(uint64_t *) buffer = value->queryValue();
}

void FloatingPointRegisterInterface::SetValue(const void *buffer)
{
	*value = SoftDouble(*(uint64_t *) buffer);
}

int FloatingPointRegisterInterface::GetSize() const
{
	return 8;
}

FloatingPointRegisterView::FloatingPointRegisterView(const char *name, unisim::kernel::service::Object *owner, SoftDouble& _storage, const char *description)
	: unisim::kernel::service::VariableBase(name, owner, unisim::kernel::service::VariableBase::VAR_REGISTER, description)
	, storage(_storage)
{
}

FloatingPointRegisterView::~FloatingPointRegisterView()
{
}

const char *FloatingPointRegisterView::GetDataTypeName() const
{
	return "64-bit floating-point register";
}

FloatingPointRegisterView::operator bool () const
{
	return (bool) storage.queryValue();
}

FloatingPointRegisterView::operator long long () const
{
	return (long long) storage.queryValue();
}

FloatingPointRegisterView::operator unsigned long long () const
{
	return (unsigned long long) storage.queryValue();
}

FloatingPointRegisterView::operator double () const
{
	return (double) storage.queryValue();
}

FloatingPointRegisterView::operator std::string () const
{
	std::stringstream sstr;
	sstr << "0x" << std::hex;
	sstr.fill('0');
	sstr.width(8);
	sstr << (uint64_t) storage.queryValue();
	return sstr.str();
}

unisim::kernel::service::VariableBase& FloatingPointRegisterView::operator = (bool value)
{
	if(IsMutable())
	{
		storage = SoftDouble((uint64_t) value);
		NotifyListeners();
	}
	return *this;
}

unisim::kernel::service::VariableBase& FloatingPointRegisterView::operator = (long long value)
{
	if(IsMutable())
	{
		storage = SoftDouble((int64_t) value);
		NotifyListeners();
	}
	return *this;
}

unisim::kernel::service::VariableBase& FloatingPointRegisterView::operator = (unsigned long long value)
{
	if(IsMutable())
	{
		storage = SoftDouble((uint64_t) value);
		NotifyListeners();
	}
	return *this;
}

unisim::kernel::service::VariableBase& FloatingPointRegisterView::operator = (double value)
{
	if(IsMutable())
	{
		storage = SoftDouble((int64_t) value);
		NotifyListeners();
	}
	return *this;
}

unisim::kernel::service::VariableBase& FloatingPointRegisterView::operator = (const char * value)
{
	if(IsMutable())
	{
		storage = SoftDouble((uint64_t) (strcmp(value, "true") == 0) ? 1 : ((strcmp(value, "false") == 0) ? 0 : strtoull(value, 0, 0)));
		NotifyListeners();
	}
	return *this;
}

} // end of namespace simfloat
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
