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

#include <unisim/service/loader/multiloader/multiloader.hh>
#include <unisim/service/loader/multiloader/multiloader.tcc>
#include <inttypes.h>
#include <limits>

namespace unisim {
namespace service {
namespace loader {
namespace multiloader {

template class MultiLoader<uint32_t>;

} // end of namespace multiloader
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim

namespace unisim {
namespace kernel {
namespace service {

typedef unisim::service::loader::multiloader::AddressRange<uint32_t> AddressRange32;
	
template <> Variable<AddressRange32 >::Variable(const char *_name, Object *_object, AddressRange32 & _storage, Type type, const char *_description) :
	VariableBase(_name, _object, type, _description), storage(&_storage)
{
	Simulator::simulator->Initialize(this);
}

template <>
const char *Variable<AddressRange32 >::GetDataTypeName() const
{
	return "32-bit address range";
}

template <> Variable<AddressRange32 >::operator bool () const
{
	return !storage->IsEmpty();
}

template <> Variable<AddressRange32 >::operator long long () const
{
	return storage->GetAmplitude();
}

template <> Variable<AddressRange32 >::operator unsigned long long () const
{
	return storage->GetAmplitude();
}

template <> Variable<AddressRange32 >::operator double () const
{
	return (double) storage->GetAmplitude();
}

template <> Variable<AddressRange32 >::operator string () const
{
	return storage->ToString();
}

template <> VariableBase& Variable<AddressRange32 >::operator = (bool value)
{
	if(IsMutable())
	{
		if(value)
		{
			storage->low = std::numeric_limits<uint32_t>::min();
			storage->high = std::numeric_limits<uint32_t>::max();
		}
		else
		{
			storage->low = std::numeric_limits<uint32_t>::max();
			storage->high = std::numeric_limits<uint32_t>::min();
		}
	}
	return *this;
}

template <> VariableBase& Variable<AddressRange32 >::operator = (long long value)
{
	if(IsMutable())
	{
		if(value > 0)
		{
			storage->low = 0;
			storage->high = value - 1;
		}
		else
		{
			storage->low = std::numeric_limits<uint32_t>::max();
			storage->high = std::numeric_limits<uint32_t>::min();
		}
	}
	return *this;
}

template <> VariableBase& Variable<AddressRange32 >::operator = (unsigned long long value)
{
	if(IsMutable())
	{
		if(value > 0)
		{
			storage->low = 0;
			storage->high = value - 1;
		}
		else
		{
			storage->low = std::numeric_limits<uint32_t>::max();
			storage->high = std::numeric_limits<uint32_t>::min();
		}
	}
	return *this;
}

template <> VariableBase& Variable<AddressRange32 >::operator = (double value)
{
	if(IsMutable())
	{
		if(value > 0.0)
		{
			storage->low = 0;
			storage->high = value - 1;
		}
		else
		{
			storage->low = std::numeric_limits<uint32_t>::max();
			storage->high = std::numeric_limits<uint32_t>::min();
		}
	}
	return *this;
}

template <> VariableBase& Variable<AddressRange32 >::operator = (const char *value)
{
	if(IsMutable())
	{
		storage->low = std::numeric_limits<uint32_t>::max();
		storage->high = std::numeric_limits<uint32_t>::min();

		if(strlen(value) > 0)
		{
			std::string str(value);

			size_t pos;
			pos = str.find('-');
			std::string str_rest = str.substr(pos + 1);
			str = str.substr(0, pos);
			
			stringstream range_low_str;
			range_low_str << str;
			range_low_str >> std::hex >> storage->low >> std::dec;
			
			stringstream range_high_str;
			range_high_str << str_rest;
			range_high_str >> std::hex >> storage->high >> std::dec;
		}
	}
	return *this;
}

} // end of namespace service
} // end of namespace kernel
} // end of namespace unisim
