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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#include "unisim/util/time/time.hh"
#include "unisim/kernel/service/service.hh"

namespace unisim {
namespace kernel {
namespace service {

using unisim::util::time::time_unit_type;
using unisim::util::time::TU_FS;
using unisim::util::time::TU_PS;
using unisim::util::time::TU_NS;
using unisim::util::time::TU_US;
using unisim::util::time::TU_MS;
using unisim::util::time::TU_SEC;

template <> 
Parameter<time_unit_type>::
Parameter(const char *_name, Object *_object, time_unit_type& _storage, const char *_description) :
	ParameterBase(_name, _object, "time-unit", _description), storage(&_storage) {
	AddEnumeratedValue("TU_FS");
	AddEnumeratedValue("TU_PS");
	AddEnumeratedValue("TU_NS");
	AddEnumeratedValue("TU_US");
	AddEnumeratedValue("TU_MS");
	AddEnumeratedValue("TU_SEC");
}

template <> 
Parameter<time_unit_type>::
operator bool () const { 
	return *storage == TU_FS; 
}

template <> 
Parameter<time_unit_type>::
operator long long () const {
	switch(*storage) {
	case TU_FS: return 0;
	case TU_PS: return 1;
	case TU_NS: return 2;
	case TU_US: return 3;
	case TU_MS: return 4;
	case TU_SEC: return 5;
	}
}

template <> 
Parameter<time_unit_type>::
operator unsigned long long () const { 
	switch(*storage) {
	case TU_FS: return 0;
	case TU_PS: return 1;
	case TU_NS: return 2;
	case TU_US: return 3;
	case TU_MS: return 4;
	case TU_SEC: return 5;
	}
}

template <> 
Parameter<time_unit_type>::
operator double () const { 
	switch(*storage) {
	case TU_FS: return 0;
	case TU_PS: return 1;
	case TU_NS: return 2;
	case TU_US: return 3;
	case TU_MS: return 4;
	case TU_SEC: return 5;
	}
}

template <> 
Parameter<time_unit_type>::
operator string () const { 
	switch(*storage) {
	case TU_FS: return string("TU_FS");
	case TU_PS: return string("TU_PS");
	case TU_NS: return string("TU_NS");
	case TU_US: return string("TU_US");
	case TU_MS: return string("TU_MS");
	case TU_SEC: return string("TU_SEC");
	}
}

template <> 
ParameterBase& Parameter<time_unit_type>::
operator = (bool value) { 
	*storage = (value?TU_PS:TU_FS); 
	return *this;
}

template <> 
ParameterBase& Parameter<time_unit_type>::
operator = (long long value) { 
	switch(value) {
	case 0:
		*storage = TU_FS;
		break;
	case 1:
		*storage = TU_PS;
		break;
	case 2:
		*storage = TU_NS;
		break;
	case 3:
		*storage = TU_US;
		break;
	case 4:
		*storage = TU_MS;
		break;
	case 5:
		*storage = TU_SEC;
		break;
	default:
		*storage = TU_FS;
		break;
	}
	return *this;
}

template <> 
ParameterBase& Parameter<time_unit_type>::
operator = (unsigned long long value) { 
	switch(value) {
	case 0:
		*storage = TU_FS;
		break;
	case 1:
		*storage = TU_PS;
		break;
	case 2:
		*storage = TU_NS;
		break;
	case 3:
		*storage = TU_US;
		break;
	case 4:
		*storage = TU_MS;
		break;
	case 5:
		*storage = TU_SEC;
		break;
	default:
		*storage = TU_FS;
		break;
	}
	return *this;
}

template <> 
ParameterBase& Parameter<time_unit_type>::
operator = (double value) {
	unsigned long long int ivalue = (unsigned long long int)value;
	switch(ivalue) {
	case 0:
		*storage = TU_FS;
		break;
	case 1:
		*storage = TU_PS;
		break;
	case 2:
		*storage = TU_NS;
		break;
	case 3:
		*storage = TU_US;
		break;
	case 4:
		*storage = TU_MS;
		break;
	case 5:
		*storage = TU_SEC;
		break;
	default:
		*storage = TU_FS;
		break;
	}
	return *this;
}

template <> 
ParameterBase& Parameter<time_unit_type>::
operator = (const char *value) {
	if(string(value) == string("TU_FS")) {
		*storage = TU_FS;
		return *this;
	}
	if(string(value) == string("TU_PS")) {
		*storage = TU_PS;
		return *this;
	}
	if(string(value) == string("TU_NS")) {
		*storage = TU_NS;
		return *this;
	}
	if(string(value) == string("TU_US")) {
		*storage = TU_US;
		return *this;
	}
	if(string(value) == string("TU_MS")) {
		*storage = TU_MS;
		return *this;
	}
	if(string(value) == string("TU_SEC")) {
		*storage = TU_SEC;
		return *this;
	}
	*storage = TU_FS;
	return *this; 
}

template class Parameter<time_unit_type>;


} // end of namespace service
} // end of namespace kernel
} // end of namespace unisim

