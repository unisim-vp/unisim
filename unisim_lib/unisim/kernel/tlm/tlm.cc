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

#include "unisim/kernel/tlm/tlm.hh"
#include "unisim/kernel/service/service.hh"
#include <stdlib.h>
#include <string.h>


namespace unisim {
namespace kernel {
namespace service {

template <> 
Variable<sc_time>::Variable(const char *_name, Object *_object, sc_time& _storage, Type type, const char *_description)
	: VariableBase(_name, _object, type, _description), storage(&_storage)
{
}

template <>
const char *Variable<sc_time>::GetDataTypeName() const
{
	return "sc_time";
}

template <> 
Variable<sc_time>::operator bool () const
{ 
	return *storage != SC_ZERO_TIME; 
}

template <> 
Variable<sc_time>::operator long long () const
{
	return (long long) storage->to_seconds();
}

template <> 
Variable<sc_time>::operator unsigned long long () const
{
	return (unsigned long long) storage->to_seconds();
}

template <> 
Variable<sc_time>::operator double () const
{
	return storage->to_seconds();
}

template <> 
Variable<sc_time>::operator string () const
{
	return storage->to_string();
}

template <> 
VariableBase& Variable<sc_time>::operator = (bool value)
{ 
	*storage = sc_time(value ? 1.0 : 0.0, SC_SEC);
	return *this;
}

template <> 
VariableBase& Variable<sc_time>::operator = (long long value)
{
	*storage = sc_time((double) value, SC_SEC);
	return *this;
}

template <> 
VariableBase& Variable<sc_time>::operator = (unsigned long long value)
{
	*storage = sc_time((double) value, SC_SEC);
	return *this;
}

template <> 
VariableBase& Variable<sc_time>::operator = (double value)
{
	*storage = sc_time(value, SC_SEC);
	return *this;
}

template <> 
VariableBase& Variable<sc_time>::operator = (const char *value)
{
	double v = 0.0;
	sc_time_unit unit = SC_SEC;
	
	char *end;
	
	v = strtod(value, &end);
	
	if(end != value)
	{
		// skip spaces
		while(*end == ' ') end++;
		
		if(strncmp(end, "fs", 2) == 0) unit = SC_FS; else
		if(strncmp(end, "ps", 2) == 0) unit = SC_PS; else
		if(strncmp(end, "ns", 2) == 0) unit = SC_NS; else
		if(strncmp(end, "us", 2) == 0) unit = SC_US; else
		if(strncmp(end, "ms", 2) == 0) unit = SC_MS; else
		if(strncmp(end, "s", 1) == 0) unit = SC_SEC;
	}
	
	*storage = sc_time(v, unit);
	return *this; 
}

template class Variable<sc_time>;


} // end of namespace service
} // end of namespace kernel
} // end of namespace unisim
