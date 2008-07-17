/*
 *  Copyright (c) 2008,
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
 
#include "unisim/kernel/service/service.hh"
#include "unisim/component/tlm2/bus/simple_router/router.hh"
#include <sstream>
#include <string>

namespace unisim {
namespace kernel {
namespace service {

template <> Variable<unisim::component::tlm2::bus::simple_router::mapping_type>::Variable(const char *_name, Object *_object, unisim::component::tlm2::bus::simple_router::mapping_type &_storage, Type type, const char *_description) :
	VariableBase(_name, _object, type, _description), storage(&_storage) {
}

template <> Variable<unisim::component::tlm2::bus::simple_router::mapping_type>::operator bool () const { return false; }
template <> Variable<unisim::component::tlm2::bus::simple_router::mapping_type>::operator long long () const { return 0; }
template <> Variable<unisim::component::tlm2::bus::simple_router::mapping_type>::operator unsigned long long () const { return 0; }
template <> Variable<unisim::component::tlm2::bus::simple_router::mapping_type>::operator double () const { return 0; }
template <> Variable<unisim::component::tlm2::bus::simple_router::mapping_type>::operator string () const { 
	std::stringstream buf;
	buf << "<unisim::component::tlm2::bus::simple_router::mapping range_start=0x" << std::hex << storage->range_start << std::dec
		<< " range_end=0x" << std::hex << storage->range_end << std::dec
		<< " output_port=" << storage->output_port << "/>";
	return buf.str();
}

template <> VariableBase& Variable<unisim::component::tlm2::bus::simple_router::mapping_type>::operator = (bool value) { return *this;}
template <> VariableBase& Variable<unisim::component::tlm2::bus::simple_router::mapping_type>::operator = (long long value) { return *this;}
template <> VariableBase& Variable<unisim::component::tlm2::bus::simple_router::mapping_type>::operator = (unsigned long long value) { return *this;}
template <> VariableBase& Variable<unisim::component::tlm2::bus::simple_router::mapping_type>::operator = (double value) { return *this;}
template <> VariableBase& Variable<unisim::component::tlm2::bus::simple_router::mapping_type>::operator = (const char *value) { 
	uint64_t range_start;
	uint64_t range_end;
	unsigned int output_port;

	std::stringstream buf(value);
	std::string str(buf.str());
	std::string str_rest;
	unsigned int pos;
	pos = str.find('"');
	str_rest = str.substr(pos + 1);
	str = str_rest;
	pos = str.find('"');
	str_rest = str.substr(pos + 1);
	str = str.substr(0, pos);
	stringstream range_start_str;
	range_start_str << str;
	range_start_str >> std::hex >> range_start >> std::dec;
	str = str_rest;
	pos = str.find('"');
	str = str.substr(pos + 1);
	pos = str.find('"');
	str_rest = str.substr(pos + 1);
	str = str.substr(0, pos);
	stringstream range_end_str;
	range_end_str << str;
	range_end_str >> std::hex >> range_end >> std::dec;
	str = str_rest;
	pos = str.find('"');
	str = str.substr(pos + 1);
	pos = str.find('"');
	str = str.substr(0, pos);
	stringstream output_port_str;
	output_port_str << str;
	output_port_str >> output_port;
	storage->range_start = range_start;
	storage->range_end = range_end;
	storage->output_port = output_port;
	return *this;
}

template class Variable<unisim::component::tlm2::bus::simple_router::mapping_type>;

} // end of namespace unisim
} // end of namespace kernel
} // namespace service


namespace unisim {
namespace component {
namespace tlm2 {
namespace bus {
namespace simple_router {

template
class Router<>;

} // end of namespace simple_router
} // end of namespace bus
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

