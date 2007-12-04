/*
 *  Copyright (c) 2007,
 *  Universitat Politecnica de Catalunya (UPC)
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
 *   - Neither the name of UPC nor the names of its contributors may be used to
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
 * Authors: Paula Casero (pcasero@upc.edu), Alejandro Schenzle (schenzle@upc.edu),
 *          Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PCI_TYPES_TCC__
#define __UNISIM_COMPONENT_CXX_PCI_TYPES_TCC__

#include "unisim/kernel/service/service.hh"

namespace unisim {
namespace kernel {
namespace service {

using std::stringstream;
using namespace unisim::component::cxx::pci;


template <> Parameter<PCISpace>::Parameter(const char *_name, Object *_object, PCISpace& _storage, const char *_description) :
	ParameterBase(_name, _object, "pci space", _description), storage(&_storage)
{
	AddEnumeratedValue("mem");
	AddEnumeratedValue("i/o");
	AddEnumeratedValue("cfg");
}

template <> Parameter<PCISpace>::operator bool () const {
	//this should't be implemented 
	return false; 
}

template <> Parameter<PCISpace>::operator long long () const {
	switch(*storage) {
	case SP_MEM:
		return 1;
	case SP_IO:
		return 2;
	case SP_CONFIG:
		return 3;
	} 
}
    
template <> Parameter<PCISpace>::operator unsigned long long () const {   	
  	switch(*storage) {
	case SP_MEM:
		return 1;
	case SP_IO:
		return 2;
	case SP_CONFIG:
		return 3;
	} 
}

template <> Parameter<PCISpace>::operator double () const { 	
  	switch(*storage) {
  	case SP_MEM:
		return (double)1;
	case SP_IO:
		return (double)2;
	case SP_CONFIG:
		return (double)3;
	} 
}

template <> Parameter<PCISpace>::operator string () const {
	switch(*storage) {
	case SP_MEM:
		return "mem";
	case SP_IO:
		return "i/o";
	case SP_CONFIG:
		return "cfg";
	} 
}
  
template <> ParameterBase& Parameter<PCISpace>::operator = (bool value) { return *this;}

template <> ParameterBase& Parameter<PCISpace>::operator = (long long value) { 
	switch(value) {
	case 1:
		*storage = SP_MEM;
		break;
	case 2:
		*storage = SP_IO; 
		break;
	case 3:
		*storage = SP_CONFIG;
		break;
	} 
	return *this;
}

template <> ParameterBase& Parameter<PCISpace>::operator = (unsigned long long value) {   	
  	switch(value) {
	case 1:
		*storage = SP_MEM;
		break;
	case 2:
		*storage = SP_IO; 
		break;
	case 3:
		*storage = SP_CONFIG;
		break;
	} 
	return *this;
}

template <> ParameterBase& Parameter<PCISpace>::operator = (double value) {   	
  	if (value==1) 
		*storage = SP_MEM;
	if (value==2) 
		*storage = SP_IO; 
	if (value==3) 
		*storage = SP_CONFIG;
 	return *this;
}

template <> ParameterBase& Parameter<PCISpace>::operator = (const char *value) { 
	if (string(value) == string("mem"))
		*storage = SP_MEM;
	if (string(value) == string("i/o"))
		*storage = SP_IO; 
	if (string(value) == string("cfg"))
		*storage = SP_CONFIG;
 	return *this;
}
  
//  template class Parameter<PCISpace>;
  
template <>
ParameterArray<PCISpace>::ParameterArray(const char *_name, Object *_owner, PCISpace *_params, unsigned int dim, const char *_description) :
	ParameterBase(_name, _owner, "pci space", _description),
	params()
{
	unsigned int i;
	for(i = 0; i < dim; i++)
	{
		stringstream sstr;
		
		sstr << _name << "[" << i << "]";
		params.push_back(new Parameter<PCISpace>(sstr.str().c_str(), _owner, *(_params + i)));
	}
}


template <>
ParameterArray<PCISpace>::~ParameterArray()
{
	vector<ParameterBase *>::iterator param_iter;
	
	for(param_iter = params.begin(); param_iter != params.end(); param_iter++)
	{
		delete *param_iter;
	}
}

template <>
ParameterBase& ParameterArray<PCISpace>::operator [] (unsigned int index)
{
	if(index >= params.size())
	{
		cerr << "Subscript out of range" << endl;
		return ServiceManager::void_param;
	}
	return *params[index];
}
  
  
}
}
}


#endif /* __UNISIM_COMPONENT_CXX_PCI_TYPES_TCC__ */
