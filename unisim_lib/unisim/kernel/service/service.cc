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
 *          Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif
#if defined(HAVE_UNISIM__KERNEL__SERVICE__CONFIG_HH)
#include "unisim/kernel/service/config.hh"
#endif

#ifndef BIN_TO_SHARED_DATA_PATH
#error "BIN_TO_SHARED_DATA_PATH is undefined"
#endif

#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger_server.hh"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#if defined(__APPLE_CC__) || defined (linux) 
#include <dlfcn.h>
#endif

#if defined(WIN32)
#include <windows.h>
#endif

// #if defined(__APPLE_CC__)
// #include <mach-o/dyld.h>
// #endif

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/graph/graphviz.hpp>

#include "unisim/kernel/service/xml_helper.hh"
#include <unisim/kernel/debug/debug.hh>

#ifdef DEBUG_MEMORY_ALLOCATION
void *operator new(std::size_t size)
{
	std::cerr << "malloc(" << size << ")" << std::endl;
	std::cerr << unisim::kernel::debug::BackTrace() << std::endl;
	return malloc(size);
}

void *operator new[](std::size_t size)
{
	std::cerr << "malloc(" << size << ")" << std::endl;
	std::cerr << unisim::kernel::debug::BackTrace() << std::endl;
	return malloc(size);
}

void operator delete(void *storage, std::size_t size)
{
	std::cerr << "free(" << size << ")" << std::endl;
	std::cerr << unisim::kernel::debug::BackTrace() << std::endl;
	free(storage);
}

void operator delete[](void *storage, std::size_t size)
{
	std::cerr << "free(" << size << ")" << std::endl;
	std::cerr << unisim::kernel::debug::BackTrace() << std::endl;
	free(storage);
}
#endif

namespace unisim {
namespace kernel {
namespace service {

using std::hex;
using std::dec;
using std::cerr;
using std::endl;
using std::stringstream;
using std::ofstream;
using namespace boost;

//=============================================================================
//=                             VariableBase                                 =
//=============================================================================
	
VariableBase::
VariableBase(const char *_name, Object *_owner, Type _type, const char *_description)
	: name(_owner ? string(_owner->GetName()) + string(".") + string(_name) : string(_name) )
	, var_name(string(_name))
	, owner(_owner)
	, container(0)
	, description(_description ? string(_description) : string(""))
	, enumerated_values()
	, type(_type)
	, fmt(FMT_DEFAULT)
	, is_mutable(true)
	, is_visible(true)
	, is_serializable(true)
	, notifiable_list()
{
	if(_owner)
	{
		_owner->Add(*this);
	}
	Simulator::simulator->Register(this);
}

VariableBase::
VariableBase(const char *_name, VariableBase *_container, Type _type, const char *_description)
	: name(_container ? string(_container->GetName()) + string(".") + string(_name) : string(_name))
	, var_name(_container ? string(_container->GetVarName()) + string(".") + string(_name) : string(_name))
	, owner(0)
	, container(_container)
	, description(_description ? string(_description) : string(""))
	, enumerated_values()
	, type(_type)
	, fmt(FMT_DEFAULT)
	, is_mutable(true)
	, is_visible(true)
	, is_serializable(true)
	, notifiable_list()
{
	Simulator::simulator->Register(this);
}

VariableBase::
VariableBase()
	: name()
	, owner(0)
	, container(0)
	, description()
	, type(VAR_VOID)
	, fmt(FMT_DEFAULT)
	, notifiable_list()
{
}

VariableBase::~VariableBase()
{
	if(owner) owner->Remove(*this);
	Simulator::simulator->Unregister(this);
}

Object *VariableBase::GetOwner() const
{
	return owner;
}

VariableBase *VariableBase::GetContainer() const
{
	return container;
}

const char *VariableBase::GetName() const
{
	return name.c_str();
}

const char *VariableBase::GetVarName() const
{
	return var_name.c_str();
}
	
const char *VariableBase::GetDescription() const
{
	return description.c_str();
}

VariableBase::Type VariableBase::GetType() const
{
	return type;
}

VariableBase::Format VariableBase::GetFormat() const
{
	return fmt;
}

const char *VariableBase::GetDataTypeName() const
{
	return "";
}

bool VariableBase::HasEnumeratedValues() const {
	return !enumerated_values.empty();
}

bool VariableBase::HasEnumeratedValue(const char * value) const {
	vector<string>::const_iterator iter;

	for(iter = enumerated_values.begin(); iter != enumerated_values.end(); iter++) {
		if(iter->compare(value) == 0) {
			return true;
		}
	}
	return false;
}

void VariableBase::GetEnumeratedValues(vector<string> &values) const {
	if(!HasEnumeratedValues()) return;
	for(unsigned int i = 0; i < enumerated_values.size(); i++) {
		values.push_back(enumerated_values[i]);
	}
}

bool VariableBase::AddEnumeratedValue(const char *value) {
	if(HasEnumeratedValue(value)) return false;
	enumerated_values.push_back(string(value));
	return true;
}

bool VariableBase::RemoveEnumeratedValue(const char *value) {
	vector<string>::iterator it;
	for(it = enumerated_values.begin(); it != enumerated_values.end(); it++) {
		if(it->compare(value) == 0) {
			enumerated_values.erase(it);
			return true;
		}
	}
	return false;
}

void VariableBase::SetFormat(Format _fmt)
{
	fmt = _fmt;
}

bool VariableBase::IsVoid() const
{
	return this == Simulator::simulator->void_variable;
}

bool VariableBase::IsMutable() const
{
	return is_mutable;
}

bool VariableBase::IsVisible() const
{
	return is_visible;
}

bool VariableBase::IsSerializable() const
{
	return is_serializable;
}

void VariableBase::SetMutable(bool _is_mutable)
{
	is_mutable = _is_mutable;
}

void VariableBase::SetVisible(bool _is_visible)
{
	is_visible = _is_visible;
}

void VariableBase::SetSerializable(bool _is_serializable)
{
	is_serializable = _is_serializable;
}

void VariableBase::SetNotify(VariableBase::Notifiable *notifiable)
{
	notifiable_list.push_back(notifiable);
	notifiable_list.unique(); // remove doubles
}

void VariableBase::RemoveNotify(VariableBase::Notifiable *notifiable)
{
	notifiable_list.remove(notifiable);
}

void VariableBase::Notify()
{
	list<VariableBase::Notifiable *>::iterator iter;
	if ( IsMutable() )
		for ( iter = notifiable_list.begin();
				iter != notifiable_list.end();
				iter++)
			(*iter)->VariableNotify(name.c_str());
}

VariableBase::operator bool () const { return false; }
VariableBase::operator char () const { return (long long) *this; }
VariableBase::operator short () const { return (long long) *this; }
VariableBase::operator int () const { return (long long) *this; }
VariableBase::operator long () const { return (long long) *this; }
VariableBase::operator long long () const { return 0LL; }
VariableBase::operator unsigned char () const { return (unsigned long long) *this; }
VariableBase::operator unsigned short () const { return (unsigned long long) *this; }
VariableBase::operator unsigned int () const { return (unsigned long long) *this; }
VariableBase::operator unsigned long () const { return (unsigned long long) *this; }
VariableBase::operator unsigned long long () const { return 0ULL; }
VariableBase::operator float () const { return (double) *this; }
VariableBase::operator double () const { return 0.0; }
VariableBase::operator string () const { return string(); }

VariableBase& VariableBase::operator = (bool value) { Notify(); return *this; }
VariableBase& VariableBase::operator = (char value) { *this = (long long) value; return *this; }
VariableBase& VariableBase::operator = (short value) { *this = (long long) value; return *this; }
VariableBase& VariableBase::operator = (int value) { *this = (long long) value; return *this; }
VariableBase& VariableBase::operator = (long value) { *this = (long long) value; return *this; }
VariableBase& VariableBase::operator = (long long value) { Notify(); return *this; }
VariableBase& VariableBase::operator = (unsigned char value) { *this = (unsigned long long) value; return *this; }
VariableBase& VariableBase::operator = (unsigned short value) { *this = (unsigned long long) value; return *this; }
VariableBase& VariableBase::operator = (unsigned int value) { *this = (unsigned long long) value; return *this; }
VariableBase& VariableBase::operator = (unsigned long value) { *this = (unsigned long long) value; return *this; }
VariableBase& VariableBase::operator = (unsigned long long value) { return *this; }
VariableBase& VariableBase::operator = (float value) { *this = (double) value; return *this; }
VariableBase& VariableBase::operator = (double value) { Notify(); return *this; }
VariableBase& VariableBase::operator = (const char *value) { Notify(); return *this; }

VariableBase& VariableBase::operator [] (unsigned int index)
{
	if(index >= 0)
	{
		cerr << "Subscript out of range" << endl;
		return *Simulator::simulator->void_variable;
	}
	return *this;
}

const VariableBase& VariableBase::operator [] (unsigned int index) const
{
	if(index >= 0)
	{
		cerr << "Subscript out of range" << endl;
		return *Simulator::simulator->void_variable;
	}
	return *this;
}

unsigned int VariableBase::GetLength() const
{
	return 0;
}

VariableBase& VariableBase::operator = (const VariableBase& variable)
{
	string variable_value = (string) variable;
	return *this = variable_value.c_str();
}

//=============================================================================
//=                            Variable<TYPE>                                =
//=============================================================================

template <class TYPE>
Variable<TYPE>::Variable(const char *_name, Object *_owner, TYPE& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{
	Simulator::simulator->Initialize(this);
}

template <class TYPE> Variable<TYPE>::operator bool () const { return (*storage) ? true : false; }
template <class TYPE> Variable<TYPE>::operator long long () const { return (long long) *storage; }
template <class TYPE> Variable<TYPE>::operator unsigned long long () const { return (unsigned long long) *storage; }
template <class TYPE> Variable<TYPE>::operator double () const { return (double) *storage; }
template <class TYPE> Variable<TYPE>::operator string () const
{
	stringstream sstr;
	switch(GetFormat())
	{
		case FMT_DEFAULT:
		case FMT_HEX:
			sstr << "0x" << hex << (unsigned long long) *storage;
			break;
		case FMT_DEC:
			sstr << dec << (unsigned long long) *storage;
			break;
	}
	return sstr.str();
}

template <class TYPE> VariableBase& Variable<TYPE>::operator = (bool value)
{
	if ( IsMutable() )
		*storage = value ? 1 : 0;
	Notify();
	return *this;
}

template <class TYPE> VariableBase& Variable<TYPE>::operator = (long long value)
{
	if ( IsMutable() )
		*storage = value;
	Notify();
	return *this;
}

template <class TYPE> VariableBase& Variable<TYPE>::operator = (unsigned long long value)
{
	if ( IsMutable() )
		*storage = value;
	Notify();
	return *this;
}

template <class TYPE> VariableBase& Variable<TYPE>::operator = (double value)
{
	if ( IsMutable() )
		*storage = (TYPE) value;
	Notify();
	return *this;
}

//=============================================================================
//=                           VariableArray<TYPE>                            =
//=============================================================================

//=============================================================================
//=                             Formula<TYPE>                                 =
//=============================================================================

// static unsigned int auto_formula_id = 0;
static string auto_formula_id_string;

template <class TYPE>
Formula<TYPE>::Formula(const char *_name, Object *_owner, Operator _op, VariableBase *child1, VariableBase *child2, VariableBase *child3, const char *_description)
	: VariableBase(_name, _owner, VAR_FORMULA, _description)
	, op(_op)
{
	childs[0] = child1;
	childs[1] = child2;
	childs[2] = child3;
}

template <class TYPE> Formula<TYPE>::operator bool () const { return Compute() ? true : false; }
template <class TYPE> Formula<TYPE>::operator long long () const { return (long long) Compute(); }
template <class TYPE> Formula<TYPE>::operator unsigned long long () const { return (unsigned long long) Compute(); }
template <class TYPE> Formula<TYPE>::operator double () const { return (double) Compute(); }
template <class TYPE> Formula<TYPE>::operator string () const
{
	stringstream sstr;
	switch(GetFormat())
	{
		case FMT_DEFAULT:
		case FMT_HEX:
			sstr << "0x" << hex << (unsigned long long) Compute();
			break;
		case FMT_DEC:
			sstr << dec << (unsigned long long) Compute();
			break;
	}
	return sstr.str();
}

/* Note: assigning a formula has no effect */
template <class TYPE> VariableBase& Formula<TYPE>::operator = (bool value) { return *this; }
template <class TYPE> VariableBase& Formula<TYPE>::operator = (long long value) { return *this; }
template <class TYPE> VariableBase& Formula<TYPE>::operator = (unsigned long long value) { return *this; }
template <class TYPE> VariableBase& Formula<TYPE>::operator = (double value) { return *this; }
template <class TYPE> VariableBase& Formula<TYPE>::operator = (const char *value) { return *this; }

template <class TYPE>
TYPE Formula<TYPE>::Compute() const
{
	switch(op)
	{
		case OP_ADD: return (TYPE)(*childs[0]) + (TYPE)(*childs[1]);
		case OP_SUB: return (TYPE)(*childs[0]) - (TYPE)(*childs[1]);
		case OP_MUL: return (TYPE)(*childs[0]) * (TYPE)(*childs[1]);
		case OP_DIV: return (TYPE)(*childs[0]) / (TYPE)(*childs[1]);
		case OP_LT: return (TYPE)(*childs[0]) < (TYPE)(*childs[1]);
		case OP_LTE: return (TYPE)(*childs[0]) <= (TYPE)(*childs[1]);
		case OP_GT: return (TYPE)(*childs[0]) > (TYPE)(*childs[1]);
		case OP_GTE: return (TYPE)(*childs[0]) >= (TYPE)(*childs[1]);
		case OP_EQ: return (TYPE)(*childs[0]) == (TYPE)(*childs[1]);
		case OP_SEL: return (TYPE)(*childs[0]) ? (TYPE)(*childs[1]) : (TYPE)(*childs[1]);
		case OP_NEG: return -(TYPE)(*childs[0]);
		case OP_ABS: return (TYPE)(*childs[0]) >= 0 ? (TYPE)(*childs[0]) : -(TYPE)(*childs[0]);
		case OP_MIN: return (TYPE)(*childs[0]) < (TYPE)(*childs[1]) ? (TYPE)(*childs[0]) : (TYPE)(*childs[1]);
		case OP_MAX: return (TYPE)(*childs[0]) > (TYPE)(*childs[1]) ? (TYPE)(*childs[0]) : (TYPE)(*childs[1]);
		case OP_AND: return (TYPE)(*childs[0]) && (TYPE)(*childs[1]);
		case OP_OR: return (TYPE)(*childs[0]) || (TYPE)(*childs[1]);
		case OP_NOT: return !(TYPE)(*childs[0]);
		case OP_EVAL: return (TYPE)(*childs[0]);
		default: return 0;
	}
	return 0;
}

//=============================================================================
//=                         specialized Variable<>                           =
//=============================================================================

template <typename T>
static const char *GetSignedDataTypeName(const T *p = 0)
{
	switch(sizeof(T))
	{
		case 1: return "signed 8-bit integer";
		case 2: return "signed 16-bit integer";
		case 4: return "signed 32-bit integer";
		case 8: return "signed 64-bit integer";
	}
	return "?";
}

template <typename T>
static const char *GetUnsignedDataTypeName(const T *p = 0)
{
	switch(sizeof(T))
	{
		case 1: return "unsigned 8-bit integer";
		case 2: return "unsigned 16-bit integer";
		case 4: return "unsigned 32-bit integer";
		case 8: return "unsigned 64-bit integer";
	}
	return "?";
}

template <>
Variable<bool>::Variable(const char *_name, Object *_owner, bool& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type,  _description), storage(&_storage)
{
	AddEnumeratedValue("true");
	AddEnumeratedValue("false");
	Simulator::simulator->Initialize(this);
}

template <>
const char *Variable<bool>::GetDataTypeName() const
{
	return "boolean";
}

template <>
Variable<char>::Variable(const char *_name, Object *_owner, char& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{
	Simulator::simulator->Initialize(this);
}

template <>
const char *Variable<char>::GetDataTypeName() const
{
	return GetSignedDataTypeName<char>(); //"char";
}

template <>
Variable<short>::Variable(const char *_name, Object *_owner, short& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{
	Simulator::simulator->Initialize(this);
}

template <>
const char *Variable<short>::GetDataTypeName() const
{
	return GetSignedDataTypeName<short>(); //"short";
}

template <>
Variable<int>::Variable(const char *_name, Object *_owner, int& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{
	Simulator::simulator->Initialize(this);
}

template <>
const char *Variable<int>::GetDataTypeName() const
{
	return GetSignedDataTypeName<int>(); //"int";
}

template <>
Variable<long>::Variable(const char *_name, Object *_owner, long& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{
	Simulator::simulator->Initialize(this);
}

template <>
const char *Variable<long>::GetDataTypeName() const
{
	return GetSignedDataTypeName<long>(); //"long";
}

template <>
Variable<long long>::Variable(const char *_name, Object *_owner, long long& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{
	Simulator::simulator->Initialize(this);
}

template <>
const char *Variable<long long>::GetDataTypeName() const
{
	return GetSignedDataTypeName<long long>(); //"long long";
}


template <>
Variable<unsigned char>::Variable(const char *_name, Object *_owner, unsigned char& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{
	Simulator::simulator->Initialize(this);
}

template <>
const char *Variable<unsigned char>::GetDataTypeName() const
{
	return GetUnsignedDataTypeName<unsigned char>(); //"unsigned char";
}

template <>
Variable<unsigned short>::Variable(const char *_name, Object *_owner, unsigned short& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{
	Simulator::simulator->Initialize(this);
}

template <>
const char *Variable<unsigned short>::GetDataTypeName() const
{
	return GetUnsignedDataTypeName<unsigned short>(); //"unsigned short";
}

template <>
Variable<unsigned int>::Variable(const char *_name, Object *_owner, unsigned int& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{
	Simulator::simulator->Initialize(this);
}

template <>
const char *Variable<unsigned int>::GetDataTypeName() const
{
	return GetUnsignedDataTypeName<unsigned int>(); //"unsigned int";
}

template <>
Variable<unsigned long>::Variable(const char *_name, Object *_owner, unsigned long& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{
	Simulator::simulator->Initialize(this);
}

template <>
const char *Variable<unsigned long>::GetDataTypeName() const
{
	return GetUnsignedDataTypeName<unsigned long>(); //"unsigned long";
}

template <>
Variable<unsigned long long>::Variable(const char *_name, Object *_owner, unsigned long long& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{
	Simulator::simulator->Initialize(this);
}

template <>
const char *Variable<unsigned long long>::GetDataTypeName() const
{
	return GetUnsignedDataTypeName<unsigned long long>(); //"unsigned long long";
}

template <> 
Variable<double>::Variable(const char *_name, Object *_owner, double& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{
	Simulator::simulator->Initialize(this);
}

template <>
const char *Variable<double>::GetDataTypeName() const
{
	return "64-bit floating-point";
}

template <>
Variable<double>::operator string () const
{
	stringstream sstr;
	sstr << *storage;
	return sstr.str();
}

template <> 
Variable<float>::Variable(const char *_name, Object *_owner, float& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{
	Simulator::simulator->Initialize(this);
}

template <>
const char *Variable<float>::GetDataTypeName() const
{
	return "32-bit floating-point";
}

template <>
Variable<float>::operator string () const
{
	stringstream sstr;
	sstr << *storage;
	return sstr.str();
}

template <> 
Variable<string>::Variable(const char *_name, Object *_owner, string& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{
	Simulator::simulator->Initialize(this);
}

template <>
const char *Variable<string>::GetDataTypeName() const
{
	return "string";
}

template <> Variable<bool>::operator string () const
{
	stringstream sstr;
	switch(GetFormat())
	{
		case FMT_DEFAULT:
			sstr << (*storage ? "true" : "false");
			break;
		case FMT_HEX:
			sstr << (*storage ? "0x1" : "0x0");
			break;
		case FMT_DEC:
			sstr << (*storage ? "1" : "0");
			break;
	}
	return sstr.str();
}

template <> VariableBase& Variable<bool>::operator = (const char *value)
{
	if ( IsMutable() )
		*storage = (strcmp(value, "true") == 0) || (strcmp(value, "0x1") == 0) || (strcmp(value, "1") == 0);
	Notify();
	return *this;
}
template <> VariableBase& Variable<char>::operator = (const char *value)
{
	if ( IsMutable() )
		*storage = strtoll(value, 0, 0);
	Notify();
	return *this;
}
template <> VariableBase& Variable<short>::operator = (const char *value)
{
	if ( IsMutable() )
		*storage = strtoll(value, 0, 0);
	Notify();
	return *this;
}
template <> VariableBase& Variable<int>::operator = (const char *value)
{
	if ( IsMutable() )
		*storage = strtoll(value, 0, 0);
	Notify();
	return *this;
}
template <> VariableBase& Variable<long>::operator = (const char *value)
{
	if ( IsMutable() )
		*storage = strtoll(value, 0, 0);
	Notify();
	return *this;
}
template <> VariableBase& Variable<long long>::operator = (const char *value)
{
	if ( IsMutable() )
		*storage = strtoll(value, 0, 0);
	Notify();
	return *this;
}
template <> VariableBase& Variable<unsigned char>::operator = (const char *value)
{
	if ( IsMutable() )
		*storage = strtoull(value, 0, 0);
	Notify();
	return *this;
}
template <> VariableBase& Variable<unsigned short>::operator = (const char *value)
{
	if ( IsMutable() )
		*storage = strtoull(value, 0, 0);
	Notify();
	return *this;
}
template <> VariableBase& Variable<unsigned int>::operator = (const char *value)
{
	if ( IsMutable() )
		*storage = strtoull(value, 0, 0);
	Notify();
	return *this;
}
template <> VariableBase& Variable<unsigned long>::operator = (const char *value)
{
	if ( IsMutable() )
		*storage = strtoull(value, 0, 0);
	Notify();
	return *this;
}
template <> VariableBase& Variable<unsigned long long>::operator = (const char *value)
{
	if ( IsMutable() )
		*storage = strtoull(value, 0, 0);
	Notify();
	return *this;
}
template <> VariableBase& Variable<float>::operator = (const char *value)
{
	if ( IsMutable() )
		*storage = strtod(value, 0);
	Notify();
	return *this;
}
template <> VariableBase& Variable<double>::operator = (const char *value)
{
	if ( IsMutable() )
		*storage = strtod(value, 0);
	Notify();
	return *this;
}

template <> Variable<string>::operator bool () const { return *storage == string("true"); }
template <> Variable<string>::operator long long () const { return strtoll(storage->c_str(), 0, 0); }
template <> Variable<string>::operator unsigned long long () const { return strtoull(storage->c_str(), 0, 0); }
template <> Variable<string>::operator double () const { return strtod(storage->c_str(), 0); }
template <> Variable<string>::operator string () const { return *storage; }

template <> VariableBase& Variable<string>::operator = (bool value)
{
	if ( IsMutable() )
		*storage = value ? "true" : "false";
	Notify();
	return *this;
}
template <> VariableBase& Variable<string>::operator = (long long value)
{
	if(IsMutable())
	{
		stringstream sstr;
		sstr << "0x" << hex << value;
		*storage = sstr.str();
	}
	Notify();
	return *this;
}
template <> VariableBase& Variable<string>::operator = (unsigned long long value)
{
	if ( IsMutable() )
	{
		stringstream sstr;
		sstr << "0x" << hex << value;
		*storage = sstr.str();
	}
	Notify();
	return *this;
}
template <> VariableBase& Variable<string>::operator = (double value)
{
	if ( IsMutable() )
	{
		stringstream sstr;
		sstr << value;
		*storage = sstr.str();
	}
	Notify();
	return *this;
}
template <> VariableBase& Variable<string>::operator = (const char *value)
{
	if ( IsMutable() )
		*storage = value;
	Notify();
	return *this;
}

//=============================================================================
//=                         specialized Formula<>                             =
//=============================================================================

template <>
Formula<double>::operator string () const
{
	stringstream sstr;
	sstr << Compute();
	return sstr.str();
}

template <>
const char *Formula<bool>::GetDataTypeName() const
{
	return "boolean";
}

template <>
const char *Formula<char>::GetDataTypeName() const
{
	return GetSignedDataTypeName<char>();
}

template <>
const char *Formula<unsigned char>::GetDataTypeName() const
{
	return GetSignedDataTypeName<unsigned char>();
}

template <>
const char *Formula<short>::GetDataTypeName() const
{
	return GetSignedDataTypeName<short>();
}

template <>
const char *Formula<unsigned short>::GetDataTypeName() const
{
	return GetSignedDataTypeName<unsigned short>();
}

template <>
const char *Formula<int>::GetDataTypeName() const
{
	return GetSignedDataTypeName<int>();
}

template <>
const char *Formula<unsigned int>::GetDataTypeName() const
{
	return GetSignedDataTypeName<unsigned int>();
}

template <>
const char *Formula<long>::GetDataTypeName() const
{
	return GetSignedDataTypeName<long>();
}

template <>
const char *Formula<unsigned long>::GetDataTypeName() const
{
	return GetSignedDataTypeName<unsigned long>();
}

template <>
const char *Formula<long long>::GetDataTypeName() const
{
	return GetSignedDataTypeName<long long>();
}

template <>
const char *Formula<unsigned long long>::GetDataTypeName() const
{
	return GetSignedDataTypeName<unsigned long long>();
}

template <>
const char *Formula<float>::GetDataTypeName() const
{
	return "single precision floating-point";
}

template <>
const char *Formula<double>::GetDataTypeName() const
{
	return "double precision floating-point";
}

//=============================================================================
//=                       template instanciations                             =
//=============================================================================

template class Variable<bool>;
template class Variable<char>;
template class Variable<short>;
template class Variable<int>;
template class Variable<long>;
template class Variable<long long>;
template class Variable<unsigned char>;
template class Variable<unsigned short>;
template class Variable<unsigned int>;
template class Variable<unsigned long>;
template class Variable<unsigned long long>;
template class Variable<float>;
template class Variable<double>;
template class Variable<string>;

template class VariableArray<bool>;
template class VariableArray<char>;
template class VariableArray<short>;
template class VariableArray<int>;
template class VariableArray<long>;
template class VariableArray<long long>;
template class VariableArray<unsigned char>;
template class VariableArray<unsigned short>;
template class VariableArray<unsigned int>;
template class VariableArray<unsigned long>;
template class VariableArray<unsigned long long>;
template class VariableArray<float>;
template class VariableArray<double>;
template class VariableArray<string>;

template class Formula<bool>;
template class Formula<char>;
template class Formula<short>;
template class Formula<int>;
template class Formula<long>;
template class Formula<long long>;
template class Formula<unsigned char>;
template class Formula<unsigned short>;
template class Formula<unsigned int>;
template class Formula<unsigned long>;
template class Formula<unsigned long long>;
template class Formula<float>;
template class Formula<double>;

//=============================================================================
//=                                 Object                                    =
//=============================================================================

Object::Object(const char *_name, Object *_parent, const char *_description)
	: object_name(_parent ? (string(_parent->GetName()) + string(".") + string(_name)) : string(_name))
	, description(_description ? string(_description) : string(""))
	, parent(_parent)
	, srv_imports()
	, srv_exports()
	, leaf_objects()
	, setup_dependencies()
{
	if(_parent) _parent->Add(*this);
	Simulator::simulator->Register(this);
}

Object::~Object()
{
	if(parent) parent->Remove(*this);
	Simulator::simulator->Unregister(this);
}

const char *Object::GetName() const
{
	return object_name.c_str();
}

Object *Object::GetParent() const
{
	return parent;
}

const list<ServiceExportBase *>& Object::GetServiceExports() const
{
	return srv_exports;
}

void Object::Add(ServiceExportBase& srv_export)
{
	srv_exports.push_back(&srv_export);
}

void Object::Remove(ServiceExportBase& srv_export)
{
	list<ServiceExportBase *>::iterator export_iter;

	for(export_iter = srv_exports.begin(); export_iter != srv_exports.end(); export_iter++)
	{
		if(*export_iter == &srv_export)
		{
			srv_exports.erase(export_iter);
			return;
		}
	}
}

const list<ServiceImportBase *>& Object::GetServiceImports() const
{
	return srv_imports;
}

void Object::Add(ServiceImportBase& srv_import)
{
	srv_imports.push_back(&srv_import);
}

void Object::Remove(ServiceImportBase& srv_import)
{
	list<ServiceImportBase *>::iterator import_iter;

	for(import_iter = srv_imports.begin(); import_iter != srv_imports.end(); import_iter++)
	{
		if(*import_iter == &srv_import)
		{
			srv_imports.erase(import_iter);
			return;
		}
	}
}

void Object::Add(Object& object)
{
	leaf_objects.push_back(&object);
}

void Object::Remove(Object& object)
{
	list<Object *>::iterator object_iter;

	for(object_iter = leaf_objects.begin(); object_iter != leaf_objects.end(); object_iter++)
	{
		if(*object_iter == &object)
		{
			leaf_objects.erase(object_iter);
			return;
		}
	}
}

void Object::Add(VariableBase& var)
{
	variables.push_back(&var);
}

void Object::Remove(VariableBase& var)
{
	list<VariableBase *>::iterator var_iter;

	for(var_iter = variables.begin(); var_iter != variables.end(); var_iter++)
	{
		if(*var_iter == &var)
		{
			variables.erase(var_iter);
			return;
		}
	}
}

void Object::GetVariables(list<VariableBase *>& lst) const
{
	list<VariableBase *>::const_iterator variable_iter;

	lst.clear();
	
	for(variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		if((*variable_iter)->IsVisible())
		{
			lst.push_back(*variable_iter);
		}
	}
}

const list<Object *>& Object::GetLeafs() const
{
	return leaf_objects;
}

void Object::Disconnect()
{
	list<ServiceImportBase *>::iterator import_iter;

	for(import_iter = srv_imports.begin(); import_iter != srv_imports.end(); import_iter++)
	{
#ifdef DEBUG_SERVICE
		cerr << (*import_iter)->GetName() << "->DisconnectService()" << endl;
#endif
		(*import_iter)->DisconnectService();
	}

	list<ServiceExportBase *>::iterator export_iter;

	for(export_iter = srv_exports.begin(); export_iter != srv_exports.end(); export_iter++)
	{
#ifdef DEBUG_SERVICE
		cerr << (*export_iter)->GetName() << "->DisconnectClient()" << endl;
#endif
		(*export_iter)->DisconnectClient();
	}
}

bool Object::Setup()
{
	return true;
}

void Object::OnDisconnect()
{
//	cerr << "WARNING! Using default OnDisconnect for " << GetName() << endl;
}

VariableBase& Object::operator [] (const char *name)
{
	string fullname = GetName() + string(".") + string(name);
	VariableBase *variable = Simulator::simulator->FindVariable(fullname.c_str());
	return *variable;
}

void Object::SetupDependsOn(ServiceImportBase& srv_import)
{
	return setup_dependencies.push_back(&srv_import);
}

list<ServiceImportBase *>& Object::GetSetupDependencies()
{
	return setup_dependencies;
}

Simulator *Object::GetSimulator() const
{
	return Simulator::simulator;
}

const char *Object::GetDescription() const
{
	return description.c_str();
}

typedef struct
{
	const char *searchFor;
	const char *replaceBy;
} Translation;

Translation conversion_table[] = {
		{"\n"}, {"\\\\\n"},
		{"é"}, {"\\'e"},
		{"è"}, {"\\`e"},
		{"ê"}, {"\\^e"},
		{"à"}, {"\\`a"},
		{"#"}, {"\\#"},
//	" ", "~",
		{"_"}, {"\\_"},
		{"\t"}, {"~~"},
		{"{"}, {"$\\{$"},
		{"}"}, {"$\\}$"},
		{"&"}, {"\\&"},
		{"--"}, {"{-}{-}"},
		{"<<"}, {"<}\\texttt{<"},
		{">>"}, {">}\\texttt{>"},
		{"<"}, {"$<$"},
		{">"}, {"$>$"},
		{"%"}, {"\\%"},
		{"//(1)"}, {"\\ding{202}"},
		{"//(2)"}, {"\\ding{203}"},
		{"//(3)"}, {"\\ding{204}"},
		{"//(4)"}, {"\\ding{205}"},
		{"//(5)"}, {"\\ding{206}"},
		{"//(6)"}, {"\\ding{207}"},
		{"//(7)"}, {"\\ding{208}"},
		{"//(8)"}, {"\\ding{209}"},
		{"//(9)"}, {"\\ding{210}"},
		{"/*(1)*/"}, {"\\ding{202}"},
		{"/*(2)*/"}, {"\\ding{203}"},
		{"/*(3)*/"}, {"\\ding{204}"},
		{"/*(4)*/"}, {"\\ding{205}"},
		{"/*(5)*/"}, {"\\ding{206}"},
		{"/*(6)*/"}, {"\\ding{207}"},
		{"/*(7)*/"}, {"\\ding{208}"},
		{"/*(8)*/"}, {"\\ding{209}"},
		{"/*(9)*/"}, {"\\ding{210}"}
};

std::string string_to_latex(const char *s, unsigned int cut = 0, const char *style = 0)
{
	std::string out;
	unsigned int col = 1;
	
	if(style)
	{
		out += "\\";
		out += style;
		out += "{";
	}
	
	while(*s)
	{
		unsigned int i;
		bool found = false;
		
		bool can_cut = (*s == ' ') || (strncmp(s, "::", 2) == 0) || (*s == '/') || (*s == '_') || (*s == '-') || (*s == '.') || (*s == '[');
		
		for(i = 0; i < sizeof(conversion_table)/sizeof(conversion_table[0]); i++)
		{
			int length = strlen(conversion_table[i].searchFor);
			if(strncmp(s, conversion_table[i].searchFor, length) == 0)
			{
				out += conversion_table[i].replaceBy;
				s += length;
				col += length;

				if(cut)
				{
					if(col > cut && can_cut)
					{
						if(style) out += "}";
						out += " \\newline$\\hookrightarrow$";
						if(style)
						{
							out += "\\";
							out += style;
							out += "{";
						}
						col = 1;
					}
				}
				found = true;
			}
		}
		
		if(!found)
		{
			out += *s;
			s++;
			col++;
			if(cut)
			{
				if(col > cut && can_cut)
				{
					if(style) out += "}";
					out += " \\newline$\\hookrightarrow$";
					if(style)
					{
						out += "\\";
						out += style;
						out += "{";
					}
					col = 1;
				}
			}
		}
	}
	
	if(style)
	{
		out += "}";
	}
	return out;
}

void Object::GenerateLatexDocumentation(ostream& os) const
{
	os << "\\multicolumn{2}{|l|}{\\textbf{\\Large " << string_to_latex(GetName()) << "}}\\\\" << std::endl;
	os << "\\hline" << std::endl;
	if(!description.empty())
	{
		os << "\\multicolumn{2}{|l|}{\\textbf{Description:} " << string_to_latex(GetDescription()) << "}\\\\" << std::endl;
		os << "\\hline" << std::endl;
	}
	
	list<VariableBase *>::const_iterator variable_iter;
	
	for(variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		if((*variable_iter)->IsVisible())
		{
			os << "\\multicolumn{1}{|p{7.5cm}}{\\textbf{Name:} " << string_to_latex((*variable_iter)->GetName(), 26, "texttt") << "} & \\multicolumn{1}{p{7.5cm}|}{\\textbf{Type:} " << string_to_latex((*variable_iter)->GetDataTypeName(), 36, "texttt") << "}\\\\" << std::endl;
			os << "\\multicolumn{1}{|p{7.5cm}}{\\textbf{Default:} " << string_to_latex(((string) *(*variable_iter)).c_str(), 24, "texttt") << "} &";
			
			if((*variable_iter)->HasEnumeratedValues())
			{
				os << " \\multicolumn{1}{p{7.5cm}|}{\\textbf{Valid:} ";
				
				std::vector<string> enumerated_values;
				(*variable_iter)->GetEnumeratedValues(enumerated_values);
				std::vector<string>::const_iterator enumerated_values_iter;
				for(enumerated_values_iter = enumerated_values.begin(); enumerated_values_iter != enumerated_values.end(); enumerated_values_iter++)
				{
					if(enumerated_values_iter != enumerated_values.begin())
					{
						os << ",~";
					}
					os << string_to_latex((*enumerated_values_iter).c_str(), 0, "texttt");
				}
				os << "}\\\\" << std::endl;
			}
			else
			{
				os << " \\multicolumn{1}{p{7.5cm}|}{}\\\\" << std::endl;
			}
			
			os << "\\multicolumn{2}{|l|}{}\\\\" << std::endl;
			os << "\\multicolumn{2}{|p{15cm}|}{\\textbf{Description:} \\newline " << string_to_latex((*variable_iter)->GetDescription()) << ".}\\\\" << std::endl;
			os << "\\hline" << std::endl;
		}
	}
}

void Object::Stop(int exit_status)
{
	Simulator::simulator->Stop(this, exit_status);
}

//=============================================================================
//=                           ServiceImportBase                               =
//=============================================================================

ServiceImportBase::ServiceImportBase(const char *_name, Object *_owner) :
	name(string(_owner->GetName()) + string(".") + string(_name)),
	owner(_owner)
{
	_owner->Add(*this);
	Simulator::simulator->Register(this);
}

ServiceImportBase::~ServiceImportBase()
{
	if(owner) owner->Remove(*this);
	Simulator::simulator->Unregister(this);
}

const char *ServiceImportBase::GetName() const
{
	return name.c_str();
}

//=============================================================================
//=                           ServiceExportBase                               =
//=============================================================================

ServiceExportBase::ServiceExportBase(const char *_name, Object *_owner) :
	name(string(_owner->GetName()) + string(".") + string(_name)),
	owner(_owner)
{
	_owner->Add(*this);
	Simulator::simulator->Register(this);
}

ServiceExportBase::~ServiceExportBase()
{
	if(owner) owner->Remove(*this);
	Simulator::simulator->Unregister(this);
}

const char *ServiceExportBase::GetName() const
{
	return name.c_str();
}

//=============================================================================
//=                                Simulator                                  =
//=============================================================================

Simulator *Simulator::simulator = 0;

Simulator::CommandLineOption::CommandLineOption(char _short_name, const char *_long_name, const char *_opt_description, const char *_arg_description)
	: short_name(_short_name)
	, long_name(_long_name)
	, arg_description(_arg_description)
	, opt_description(_opt_description)
{
}

char Simulator::CommandLineOption::GetShortName() const
{
	return short_name;
}

const char *Simulator::CommandLineOption::GetLongName() const
{
	return long_name;
}

bool Simulator::CommandLineOption::HasArgument() const
{
	return arg_description != 0;
}

const char *Simulator::CommandLineOption::GetArgumentDescription() const
{
	return arg_description;
}

const char *Simulator::CommandLineOption::GetOptionDescription() const
{
	return opt_description;
}

int Simulator::CommandLineOption::operator == (const char *arg) const
{
	if(arg[0] == '-')
	{
		if(arg[1] == '-')
		{
			return strcmp(arg + 2, long_name) == 0;
		}
		else
		{
			return (arg[2] == 0) && (arg[1] == short_name);
		}
	}
	return 0;
}

Simulator::Simulator(int argc, char **argv, void (*LoadBuiltInConfig)(Simulator *))
	: void_variable(0)
	, shared_data_dir()
	, set_vars()
	, get_config_filename()
	, list_parms(false)
	, get_config(false)
	, generate_doc(false)
	, generate_doc_filename()
	, enable_warning(false)
	, enable_version(false)
	, enable_help(false)
	, warn_get_bin_path(false)
	, warn_get_share_path(false)
	, enable_press_enter_at_exit(false)
    , bin_dir()
	, program_binary()
	, program_name()
	, authors()
	, copyright()
	, description()
	, version()
    , license()
	, param_get_config(0)
	, var_program_name(0)
	, var_authors(0)
	, var_copyright(0)
	, var_description(0)
	, var_version(0)
	, var_license(0)
	, param_enable_press_enter_at_exit(0)
	, command_line_options()
	, objects()
	, imports()
	, exports()
	, variables()
	, cmd_args(0)
	, param_cmd_args(0)
{
	command_line_options.push_back(CommandLineOption('s', "set", "set value of parameter 'param' to 'value'", "param=value"));
	command_line_options.push_back(CommandLineOption('c', "config", "configures the simulator with the given XML configuration file", "XML file"));
	command_line_options.push_back(CommandLineOption('g', "get-config", "get the simulator configuration XML file (you can use it to create your own configuration. This option can be combined with -c to get a new configuration file with existing variables from another file", "XML file"));
	command_line_options.push_back(CommandLineOption('l', "list", "lists all available parameters, their type, and their current value"));
	command_line_options.push_back(CommandLineOption('w', "warn", "enable printing of kernel warnings"));
	command_line_options.push_back(CommandLineOption('d', "doc", "enable printing a latex documentation", "Latex file"));
	command_line_options.push_back(CommandLineOption('v', "version", "displays the program version information"));
	command_line_options.push_back(CommandLineOption('h', "help", "displays this help"));
	
	if(LoadBuiltInConfig)
	{
		LoadBuiltInConfig(this);
	}
	
	simulator = this;
	void_variable = new VariableBase("void", (Object *) 0, VariableBase::VAR_VOID, "unknown variable");
	param_get_config = new Parameter<bool>("get-config", 0, get_config, "Enable/Disable saving configuration at setup");

	var_authors = new Parameter<string>("authors", 0, authors, "Authors");
	var_authors->SetMutable(false);
	var_authors->SetVisible(false);
	var_authors->SetSerializable(false);

	var_program_name = new Parameter<string>("program-name", 0, program_name, "Program name");
	var_program_name->SetMutable(false);
	var_program_name->SetVisible(false);
	var_program_name->SetSerializable(false);

	var_copyright = new Parameter<string>("copyright", 0, copyright, "Copyright");
	var_copyright->SetMutable(false);
	var_copyright->SetVisible(false);
	var_copyright->SetSerializable(false);

	var_version = new Parameter<string>("version", 0, version, "Version");
	var_version->SetMutable(false);
	var_version->SetVisible(false);
	var_version->SetSerializable(false);

	var_description = new Parameter<string>("description", 0, description, "Description");
	var_description->SetMutable(false);
	var_description->SetVisible(false);
	var_description->SetSerializable(false);

	var_license = new Parameter<string>("license", 0, license, "License");
	var_license->SetMutable(false);
	var_license->SetVisible(false);
	var_license->SetSerializable(false);

	param_enable_press_enter_at_exit = new Parameter<bool>("enable-press-enter-at-exit", 0, enable_press_enter_at_exit, "Enable/Disable pressing key enter at exit");
	
	if(GetBinPath(argv[0], bin_dir, program_binary))
	{
		//std::cerr << "bin_dir=\"" << bin_dir << "\"" << std::endl;
		//std::cerr << "program_binary=\"" << program_binary << "\"" << std::endl;

		if(GetSharePath(bin_dir, shared_data_dir))
		{
			//std::cerr << "shared_data_dir=\"" << shared_data_dir << "\"" << std::endl;
		}
		else
		{
			warn_get_share_path = true;
		}
	}
	else
	{
		warn_get_bin_path = true;
		warn_get_share_path = true;
	}
	
	// parse command line arguments
	int state = 0;
	char **arg;
	for(arg = argv + 1; *arg != 0 && state != -1;)
	{
		switch(state)
		{
			case 0:
				{
					std::vector<CommandLineOption>::const_iterator cmd_opt_iter;
					bool match = false;
					for(cmd_opt_iter = command_line_options.begin(); !match && cmd_opt_iter != command_line_options.end(); cmd_opt_iter++)
					{
						if(*cmd_opt_iter == *arg)
						{
							// match
							match=true;
							switch(cmd_opt_iter->GetShortName())
							{
								case 's':
									arg++;
									state = 1;
									break;
								case 'c':
									arg++;
									state = 2;
									break;
								case 'g':
									arg++;
									state = 3;
									break;
								case 'l':
									arg++;
									list_parms = true;
									break;
								case 'v':
									arg++;
									enable_version = true;
									break;
								case 'h':
									arg++;
									enable_help = true;
									break;
								case 'w':
									arg++;
									enable_warning = true;
									if(!LoadBuiltInConfig)
									{
										cerr << "WARNING! No built-in parameters set loaded" << endl;
									}
									if(warn_get_bin_path)
									{
										cerr << "WARNING! Can't determine binary directory" << endl;
									}
									if(warn_get_share_path)
									{
										cerr << "WARNING! Can't determine share directory" << endl;
									}
									break;
								case 'd':
									arg++;
									state = 4;
									break;
								default:
									state = -1;
									break;
							}
						}
					}
					if(!match)
					{
						state = -1;
					}
				}
				break;
			case 1:
				{
					string variable_name;
					
					char *p;
					for(p = *arg; *p != 0 && *p != '='; p++)
					{
						variable_name += *p;
					}
					if(*p == '=')
					{
						char *variable_value = ++p;
						
						SetVariable(variable_name.c_str(), variable_value);
					}
					else if(enable_warning)
					{
						cerr << "WARNING! Ignoring " << *arg << endl;
					}
					arg++;
				}
				state = 0;
				break;
			case 2:
				if(LoadXmlParameters(*arg))
				{
					cerr << "Parameters set using file \"" << (*arg) << "\"" << endl;
				}
				else if(enable_warning)
				{
					cerr << "WARNING! Loading parameters set from file \"" << (*arg) << "\" failed" << endl;
				}
				arg++;
				state = 0;
				break;
			case 3:
				get_config = true;
				get_config_filename = *arg;
				arg++;
				state = 0;
				break;
			case 4:
				generate_doc = true;
				generate_doc_filename = *arg;
				arg++;
				state = 0;
				break;
			default:
				cerr << "Internal error while parsing command line arguments" << endl;
				state = -1;
		}
	}
	
	// create on the fly parameters cmd-args[*] that are the remaining parameters
	int cmd_args_dim = argc - (arg - argv);
	if(cmd_args_dim > 0)
	{
		cmd_args = new string[cmd_args_dim];
		param_cmd_args = new ParameterArray<string>("cmd-args", 0, cmd_args, cmd_args_dim);
		int i;
		for(i = 0; *arg != 0; arg++, i++)
		{
			(*param_cmd_args)[i] = *arg;
		}
	}
	else
	{
		param_cmd_args = new ParameterArray<string>("cmd-args", 0, 0, 0);
	}
}

Simulator::~Simulator()
{
	if(enable_press_enter_at_exit)
	{
		std::cout << "Press Enter to exit..." << std::endl;
		getc(stdin);
	}
	
	if(void_variable)
	{
		delete void_variable;
	}
	
	if(param_get_config)
	{
		delete param_get_config;
	}
	
	if(param_cmd_args)
	{
		delete param_cmd_args;
	}
	
	if(var_program_name)
	{
		delete var_program_name;
	}
	
	if(var_authors)
	{
		delete var_authors;
	}
	
	if(var_copyright)
	{
		delete var_copyright;
	}
	
	if(var_version)
	{
		delete var_version;
	}
	
	if(var_license)
	{
		delete var_license;
	}
	
	if(var_description)
	{
		delete var_description;
	}

	if(param_enable_press_enter_at_exit)
	{
		delete param_enable_press_enter_at_exit;
	}
	
	if(cmd_args)
	{
		delete[] cmd_args;
	}
}

void Simulator::Version(ostream& os) const
{
	os << program_name << " " << version << endl;
	os << copyright << endl;
	os << "License: " << license << endl;
	os << "Authors: " << authors << endl;
}

void Simulator::Help(ostream& os) const
{
	os << description << endl << endl;
	os << "Usage: " << program_binary << " [<options>] [...]" << endl << endl;
	os << "Options:" << endl;

	std::vector<CommandLineOption>::const_iterator cmd_opt_iter;
	for(cmd_opt_iter = command_line_options.begin(); cmd_opt_iter != command_line_options.end(); cmd_opt_iter++)
	{
		os << std::endl;
		os << " --" << cmd_opt_iter->GetLongName();
		if(cmd_opt_iter->HasArgument())
		{
			os << " <" << cmd_opt_iter->GetArgumentDescription() << ">";
		}
		os << std::endl;
		os << " -" << cmd_opt_iter->GetShortName();
		if(cmd_opt_iter->HasArgument())
		{
			os << " <" << cmd_opt_iter->GetArgumentDescription() << ">";
		}
		os << std::endl;
		os << "            " << cmd_opt_iter->GetOptionDescription();
		os << std::endl;
	}
}

void Simulator::Register(Object *object)
{
	if(objects.find(object->GetName()) != objects.end())
	{
		cerr << "ERROR! Object \"" << object->GetName() << "\" already exists" << endl;
		Dump(cerr);
		exit(1);
	}
	
	objects[object->GetName()] = object;
}

void Simulator::Register(VariableBase *variable)
{
	if(variables.find(variable->GetName()) != variables.end())
	{
		cerr << "ERROR! Variable \"" << variable->GetName() << "\" already exists" << endl;
		exit(1);
	}

	variables[variable->GetName()] = variable;
}

void Simulator::Initialize(VariableBase *variable)
{
	// initialize variable from command line
	std::map<string, string>::iterator set_var_iter = set_vars.find(variable->GetName());
	
	if(set_var_iter != set_vars.end())
	{
		*variable = (*set_var_iter).second.c_str();
		set_vars.erase(set_var_iter);
	}
}

void Simulator::Register(ServiceImportBase *srv_import)
{
	if(imports.find(srv_import->GetName()) != imports.end())
	{
		cerr << "ERROR! Import \"" << srv_import->GetName() << "\" already exists" << endl;
		exit(1);
	}

	imports[srv_import->GetName()] = srv_import;
}

void Simulator::Register(ServiceExportBase *srv_export)
{
	if(exports.find(srv_export->GetName()) != exports.end())
	{
		cerr << "ERROR! Export \"" << srv_export->GetName() << "\" already exists" << endl;
		exit(1);
	}

	exports[srv_export->GetName()] = srv_export;
}

void Simulator::Unregister(Object *object)
{
	map<const char *, Object *, ltstr>::iterator object_iter;
	object_iter = objects.find(object->GetName());
	if(object_iter != objects.end())
	{
		objects.erase(object_iter);
	}
}

void Simulator::Unregister(VariableBase *variable)
{
	map<const char *, VariableBase *, ltstr>::iterator variable_iter;
	variable_iter = variables.find(variable->GetName());
	if(variable_iter != variables.end()) variables.erase(variable_iter);
}

void Simulator::Unregister(ServiceImportBase *srv_import)
{
	map<const char *, ServiceImportBase *, ltstr>::iterator import_iter;
	import_iter = imports.find(srv_import->GetName());
	if(import_iter != imports.end()) imports.erase(import_iter);
}

void Simulator::Unregister(ServiceExportBase *srv_export)
{
	map<const char *, ServiceExportBase *, ltstr>::iterator export_iter;
	export_iter = exports.find(srv_export->GetName());
	if(export_iter != exports.end()) exports.erase(export_iter);
}

void Simulator::Dump(ostream& os)
{
	os << "OBJECTS:" << endl;

	map<const char *, Object *, ltstr>::iterator object_iter;

	for(object_iter = objects.begin(); object_iter != objects.end(); object_iter++)
	{
		os << (*object_iter).second->GetName() << endl;
	}
	os << endl;

	os << "PARAMETERS:" << endl;
	map<const char *, VariableBase *, ltstr>::iterator variable_iter;
	
	for(variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		os << (*variable_iter).second->GetName() << " = \"" << ((string) *(*variable_iter).second) << "\"" << endl;
	}
	os << endl;

	os << "IMPORTS:" << endl;

	map<const char *, ServiceImportBase *, ltstr>::iterator import_iter;

	for(import_iter = imports.begin(); import_iter != imports.end(); import_iter++)
	{
		Object *service = (*import_iter).second->GetService();
		os << (*import_iter).second->GetName();
		if(service) os << " (to " << service->GetName() << ")";
		os << endl;
	}
	os << endl;

	cerr << "EXPORTS:" << endl;

	map<const char *, ServiceExportBase *, ltstr>::iterator export_iter;

	for(export_iter = exports.begin(); export_iter != exports.end(); export_iter++)
	{
		Object *client = (*export_iter).second->GetClient();
		os << (*export_iter).second->GetName();
		if(client) os << " (from " << client->GetName() << ")";
		os << endl;
	}
	os << endl;

	cerr << "CONNECTIONS:" << endl;
	for(import_iter = imports.begin(); import_iter != imports.end(); import_iter++)
	{
		(*import_iter).second->Dump(os);
	}
	for(export_iter = exports.begin(); export_iter != exports.end(); export_iter++)
	{
		(*export_iter).second->Dump(os);
	}
}

void Simulator::DumpVariables(ostream &os, VariableBase::Type filter_type) {
/*	switch(filter_type)
	{
		case VariableBase::VAR_VOID:
			os << "Variables:" << endl;
			break;
		case VariableBase::VAR_ARRAY:
			os << "Arrays of variables:" << endl;
			break;
		case VariableBase::VAR_PARAMETER:
			os << "Parameters:" << endl;
			break;
		case VariableBase::VAR_STATISTIC:
			os << "Statistics:" << endl;
			break;
		case VariableBase::VAR_REGISTER:
			os << "Registers:" << endl;
			break;
	}*/
	
	unsigned int max_variable_name_length = 0;
	unsigned int max_variable_value_length = 0;
	
	map<const char *, VariableBase *, ltstr>::iterator variable_iter;

	for(variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		VariableBase *var = (*variable_iter).second;
		VariableBase::Type var_type = var->GetType();
		if(var->IsVisible() && (filter_type == VariableBase::VAR_VOID || var_type == filter_type))
		{
			const char *name = var->GetName();
			string value = (string) *var;
			
			unsigned int variable_name_length = strlen(name);
			unsigned int variable_value_length = value.length();
			
			if(variable_name_length > max_variable_name_length) max_variable_name_length = variable_name_length;
			if(variable_value_length > max_variable_value_length) max_variable_value_length = variable_value_length;
		}
	}

	for(variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		VariableBase *var = (*variable_iter).second;
		VariableBase::Type var_type = var->GetType();
		if(var->IsVisible() && (filter_type == VariableBase::VAR_VOID || var_type == filter_type))
		{
			std::stringstream sstr_name;
			std::stringstream sstr_value;
			const char *name = var->GetName();
			string value = (string) *var;
//			const char *dt = var->GetDataTypeName();
			const char *desc = (*variable_iter).second->GetDescription();
			
			sstr_name.width(max_variable_name_length);
			sstr_name.setf(std::ios::left);
			sstr_name << name;
			sstr_value.width(max_variable_value_length);
			sstr_value.setf(std::ios::left);
			sstr_value << value;
			os << sstr_name.str() << " " << sstr_value.str();
			if(strlen(desc) != 0)
			{
				os << " # " << (*variable_iter).second->GetDescription();
			}
// 			if(strlen(dt) != 0)
// 			{
// 				os << dt;
// 			}
			
			os << endl;
		}
	}
}

void Simulator::DumpStatistics(ostream &os)
{
	DumpVariables(os, VariableBase::VAR_STATISTIC);
}

void Simulator::DumpParameters(ostream &os)
{
	DumpVariables(os, VariableBase::VAR_PARAMETER);
}

void Simulator::DumpRegisters(ostream &os)
{
	DumpVariables(os, VariableBase::VAR_REGISTER);
}

void Simulator::DumpFormulas(ostream &os)
{
	DumpVariables(os, VariableBase::VAR_FORMULA);
}

bool Simulator::XmlfyVariables(const char *filename)
{
	XMLHelper xml_helper(this);
	
	return xml_helper.XmlfyVariables(filename);
}

bool Simulator::LoadXmlVariables(const char *filename)
{
	XMLHelper xml_helper(this);
	
	return xml_helper.LoadXmlVariables(filename);
}

bool Simulator::XmlfyParameters(const char *filename)
{
	XMLHelper xml_helper(this);
	
	return xml_helper.XmlfyVariables(filename, VariableBase::VAR_PARAMETER);
}

bool Simulator::LoadXmlParameters(const char *filename)
{
	XMLHelper xml_helper(this);
	
	return xml_helper.LoadXmlVariables(filename, VariableBase::VAR_PARAMETER);
}

bool Simulator::XmlfyStatistics(const char *filename)
{
	XMLHelper xml_helper(this);
	
	return xml_helper.XmlfyVariables(filename, VariableBase::VAR_STATISTIC);
}

bool Simulator::LoadXmlStatistics(const char *filename)
{
	XMLHelper xml_helper(this);
	
	return xml_helper.LoadXmlVariables(filename, VariableBase::VAR_STATISTIC);
}

bool Simulator::XmlfyRegisters(const char *filename)
{
	XMLHelper xml_helper(this);
	
	return xml_helper.XmlfyVariables(filename, VariableBase::VAR_REGISTER);
}

bool Simulator::LoadXmlRegisters(const char *filename)
{
	XMLHelper xml_helper(this);
	
	return xml_helper.LoadXmlVariables(filename, VariableBase::VAR_REGISTER);
}

bool Simulator::XmlfyFormulas(const char *filename)
{
	XMLHelper xml_helper(this);
	
	return xml_helper.XmlfyVariables(filename, VariableBase::VAR_FORMULA);
}

bool Simulator::LoadXmlFormulas(const char *filename)
{
	XMLHelper xml_helper(this);
	
	return xml_helper.LoadXmlVariables(filename, VariableBase::VAR_FORMULA);
}

struct MyVertexProperty
{
	Object *obj;
};

typedef adjacency_list<vecS, vecS, directedS, MyVertexProperty> DependencyGraph;
typedef graph_traits<DependencyGraph>::vertex_descriptor Vertex;

struct CycleDetector : public dfs_visitor<>
{
	CycleDetector(bool& _has_cycle) : has_cycle(_has_cycle)
	{
	}
	
	template <class Edge, class Graph>
	void back_edge(Edge, Graph&)
	{
		has_cycle = true;
	}
protected:
	bool& has_cycle;
};

Simulator::SetupStatus Simulator::Setup()
{
	if(generate_doc)
	{
		if(generate_doc_filename.empty())
		{
			GenerateLatexDocumentation(std::cerr);
		}
		else
		{
			cerr << "Latex documentation generation using file \"" << generate_doc_filename << "\"" << endl;
			ofstream stream(generate_doc_filename.c_str(), std::ofstream::out);
			GenerateLatexDocumentation(stream);
		}
		return ST_OK_DONT_START;
	}
	if(enable_version)
	{
		Version(cerr);
		return ST_OK_DONT_START;
	}
	if(enable_help)
	{
		Help(cerr);
		return ST_OK_DONT_START;
	}
	
	if(list_parms)
	{
		cerr << "Listing parameters..." << endl;
		DumpVariables(cerr, unisim::kernel::service::VariableBase::VAR_PARAMETER);
		cerr << "Aborting simulation" << endl;
		return ST_OK_DONT_START;
	}

	if(!get_config_filename.empty())
	{
		XmlfyParameters(get_config_filename.c_str());
		cerr << "Parameters saved on file \"" << get_config_filename << "\"" << endl;
		cerr << "Aborting simulation" << endl;
		return ST_OK_DONT_START;
	}

	// Build a dependency graph of methods "Setup"
	map<const char *, Object *, ltstr>::iterator object_iter;
	map<Object *, unsigned int> id_lookup;
	DependencyGraph dependency_graph(objects.size());
	
	unsigned int id = 0;

	for(object_iter = objects.begin(); object_iter != objects.end(); object_iter++, id++)
	{
//		cerr << "Object: " << (*object_iter).second->GetName() << endl;
		dependency_graph[id].obj = (*object_iter).second;
		id_lookup[(*object_iter).second] = id;
	}

	for(object_iter = objects.begin(); object_iter != objects.end(); object_iter++)
	{
		list<ServiceImportBase *>& setup_dependencies = (*object_iter).second->GetSetupDependencies();
		list<ServiceImportBase *>::iterator import_iter;

		for(import_iter = setup_dependencies.begin(); import_iter != setup_dependencies.end(); import_iter++)
		{
			Object *peer_object = (*import_iter)->GetService();
			if(peer_object)
			{
				add_edge(id_lookup[peer_object], id_lookup[(*object_iter).second], dependency_graph);
//				cerr << peer_object->GetID() << ":" << peer_object->GetName() << "->" 
//					<< (*object_iter).second->GetID() << ":" << (*object_iter).second->GetName() << endl;
			}
		}

	}

// 	ofstream of("deps.dot");
// 	boost::write_graphviz(of, dependency_graph);

	// Detect cycles in dependency graph
	bool has_cycle = false;
	CycleDetector vis(has_cycle);
	boost::depth_first_search(dependency_graph, visitor(vis));

	if(has_cycle)
	{
		cerr << "Simulator: ERROR! cyclic setup dependency graph" << endl;
		return ST_ERROR;
	}

	// Compute a topological order of methods "Setup"
	typedef list<Vertex> SetupOrder;
	SetupOrder setup_order;
	topological_sort(dependency_graph, std::front_inserter(setup_order));

	// Call methods "Setup" in a topological order
	unisim::kernel::logger::LoggerServer::GetInstanceWithoutCountingReference()->Setup();
	
	SetupStatus status = ST_OK_TO_START;
	list<Vertex>::iterator vertex_iter;
	for(vertex_iter = setup_order.begin(); vertex_iter != setup_order.end(); vertex_iter++)
	{
//		cerr << "Simulator:" <<  dependency_graph[*vertex_iter].obj->GetName() << "::Setup()" << endl;
		if(!dependency_graph[*vertex_iter].obj->Setup())
		{
			cerr << "Simulator: " << dependency_graph[*vertex_iter].obj->GetName() << " setup failed" << endl;
			status = ST_ERROR;
			break;
		}
	}

	if(enable_warning)
	{
		// display a warning if some variable values are unused
		std::map<string, string>::iterator set_var_iter;
		
		for(set_var_iter = set_vars.begin(); set_var_iter != set_vars.end(); set_var_iter++)
		{
			cerr << "WARNING! value \"" << (*set_var_iter).second << "\" for variable \"" << (*set_var_iter).first << "\" is unused." << endl;
		}
	}

	return status;
}

void Simulator::Stop(Object *object, int exit_status)
{
}

const VariableBase *Simulator::FindVariable(const char *name, VariableBase::Type type) const
{
	map<const char *, VariableBase *, ltstr>::const_iterator variable_iter;
	
	variable_iter = variables.find(name);
	
	if(variable_iter != variables.end() && (type == VariableBase::VAR_VOID || (*variable_iter).second->GetType() == type)) return (*variable_iter).second;
	
	return void_variable;
}

VariableBase *Simulator::FindVariable(const char *name, VariableBase::Type type)
{
	map<const char *, VariableBase *, ltstr>::iterator variable_iter;
	
	variable_iter = variables.find(name);
	
	if(variable_iter != variables.end() && (type == VariableBase::VAR_VOID || (*variable_iter).second->GetType() == type)) return (*variable_iter).second;
	
	return void_variable;
}

const VariableBase *Simulator::FindArray(const char *name) const
{
	return FindVariable(name, VariableBase::VAR_ARRAY);
}

VariableBase *Simulator::FindArray(const char *name)
{
	return FindVariable(name, VariableBase::VAR_ARRAY);
}

const VariableBase *Simulator::FindParameter(const char *name) const
{
	return FindVariable(name, VariableBase::VAR_PARAMETER);
}

VariableBase *Simulator::FindParameter(const char *name)
{
	return FindVariable(name, VariableBase::VAR_PARAMETER);
}

const VariableBase *Simulator::FindRegister(const char *name) const
{
	return FindVariable(name, VariableBase::VAR_REGISTER);
}

VariableBase *Simulator::FindRegister(const char *name)
{
	return FindVariable(name, VariableBase::VAR_REGISTER);
}

const VariableBase *Simulator::FindStatistic(const char *name) const
{
	return FindVariable(name, VariableBase::VAR_STATISTIC);
}

VariableBase *Simulator::FindStatistic(const char *name)
{
	return FindVariable(name, VariableBase::VAR_STATISTIC);
}

const VariableBase *Simulator::FindFormula(const char *name) const
{
	return FindVariable(name, VariableBase::VAR_FORMULA);
}

VariableBase *Simulator::FindFormula(const char *name)
{
	return FindVariable(name, VariableBase::VAR_FORMULA);
}

void Simulator::GetVariables(list<VariableBase *>& lst, VariableBase::Type type)
{
	map<const char *, VariableBase *, ltstr>::iterator variable_iter;

	lst.clear();
	
	for(variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		if((*variable_iter).second->IsVisible() && (type == VariableBase::VAR_VOID || (*variable_iter).second->GetType() == type))
		{
			lst.push_back((*variable_iter).second);
		}
	}
}

void Simulator::GetArrays(list<VariableBase *>& lst)
{
	GetVariables(lst, VariableBase::VAR_ARRAY);
}

void Simulator::GetParameters(list<VariableBase *>& lst)
{
	GetVariables(lst, VariableBase::VAR_PARAMETER);
}

void Simulator::GetRegisters(list<VariableBase *>& lst)
{
	GetVariables(lst, VariableBase::VAR_REGISTER);
}

void Simulator::GetStatistics(list<VariableBase *>& lst)
{
	GetVariables(lst, VariableBase::VAR_STATISTIC);
}

void Simulator::GetFormulas(list<VariableBase *>& lst)
{
	GetVariables(lst, VariableBase::VAR_FORMULA);
}

void Simulator::GetRootObjects(list<Object *>& lst)
{
	map<const char *, Object *, ltstr>::iterator object_iter;

	for(object_iter = objects.begin(); object_iter != objects.end(); object_iter++)
	{
		if((*object_iter).second)
		{
			if(!(*object_iter).second->GetParent())
			{
				// cerr << "root: " << (*object_iter).second->GetName() << endl;
				lst.push_back((*object_iter).second);
			}
		}
	}
}

#if defined(__APPLE_CC__) || defined(linux)
void FindMyself()
{
	// stupid method to find the path to the executable/library using the dladdr
	//   function under apple and linux
}
#endif

bool Simulator::GetExecutablePath(const char *argv0, std::string& out_executable_path) const
{
#if defined(linux)
	Dl_info info;
	if ( dladdr((void *)unisim::kernel::service::FindMyself, &info) != 0 )
	{
		char bin_path_buf[PATH_MAX + 1];
		ssize_t bin_path_length;
		char *bin_path_pointer = realpath(info.dli_fname, bin_path_buf);
		if(bin_path_pointer == bin_path_buf)
		{
			out_executable_path = std::string(bin_path_buf);
			return true;
		}
	}
#elif defined(WIN32)
	char bin_path_buf[PATH_MAX + 1];
	DWORD bin_path_length;
	bin_path_length = GetModuleFileName(NULL, bin_path_buf, sizeof(bin_path_buf));
	if(bin_path_length > 0)
	{
		bin_path_buf[bin_path_length] = 0;
		out_executable_path = std::string(bin_path_buf);
		return true;
	}
#elif defined(__APPLE_CC__)
	Dl_info info;
	if ( dladdr((void *)unisim::kernel::service::FindMyself, &info) != 0 )
	{
		char bin_path_buf[PATH_MAX + 1];
//		ssize_t bin_path_length;
		char *bin_path_pointer = realpath(info.dli_fname, bin_path_buf);
		if(bin_path_pointer == bin_path_buf)
		{
			out_executable_path = std::string(bin_path_buf);
			return true;
		}
	}
//	uint32_t bin_path_buf_size = 0;
//	_NSGetExecutablePath(0, &bin_path_buf_size);
//	char bin_path_buf[bin_path_buf_size];
//	if(_NSGetExecutablePath(bin_path_buf, &bin_path_buf_size) == 0)
//	{
//		out_executable_path = std::string(bin_path_buf);
//		return true;
//	}
#endif
	char *path_buf = getenv("PATH");
	if(path_buf)
	{
		// environment variable PATH is set
		std::string path(path_buf);
		
		size_t pos = 0;
		size_t next_pos = 0;
		do
		{
			next_pos = path.find(':', pos);
			if(next_pos != std::string::npos)
			{
				if(pos != next_pos)
				{
					std::string dir(path.substr(pos, next_pos - pos));
					std::string filename(dir + "/" + argv0);
					if(access(filename.c_str(), X_OK) == 0)
					{
						out_executable_path = filename;
						return true;
					}
				}
				pos = next_pos + 1;
			}
		} while(next_pos != std::string::npos);
		
		char cwd_path_buf[PATH_MAX];
		if(getcwd(cwd_path_buf, sizeof(cwd_path_buf)))
		{
			out_executable_path = std::string(cwd_path_buf) + "/" + argv0;
			return true;
		}
	}
	if(access(argv0, X_OK) == 0)
	{
		out_executable_path = std::string(argv0);
		return true;
	}
	return false;
}

bool Simulator::GetBinPath(const char *argv0, std::string& out_bin_dir, std::string& out_bin_program) const
{
	std::string executable_path;
	
	if(!GetExecutablePath(argv0, executable_path)) return false;
	//std::cerr << "executable_path=\"" << executable_path << "\"" << std::endl;
	// compute bin dirname
	const char *start = executable_path.c_str();
	const char *end = start + executable_path.length() - 1;
	while(end != (start - 1) && 
#ifdef WIN32
	      (*end != '\\') &&
#endif
	      (*end != '/'))
	{
		end--;
	}
	
	if(end != (start - 1))
	{
		out_bin_dir.clear();
		const char *p = start;
		while(p != end)
		{
			out_bin_dir += *p;
			p++;
		}
		
		out_bin_program = end + 1;
		return true;
	}
	return false;
}

bool Simulator::GetSharePath(const std::string& bin_dir, std::string& out_share_dir) const
{
	// append path of shared data relative to bin directory
	std::string unresolved_shared_data_dir = bin_dir;
	unresolved_shared_data_dir += '/';
	unresolved_shared_data_dir += BIN_TO_SHARED_DATA_PATH;
	char resolved_shared_data_dir_buf[PATH_MAX + 1];
		
#if defined(linux) || defined(__APPLE_CC__)
	if(realpath(unresolved_shared_data_dir.c_str(), resolved_shared_data_dir_buf))
	{
		out_share_dir = resolved_shared_data_dir_buf;
		return true;
	}
#elif defined(WIN32)
	DWORD length = GetFullPathName(unresolved_shared_data_dir.c_str(), PATH_MAX + 1, resolved_shared_data_dir_buf, 0);
	if(length > 0)
	{
		resolved_shared_data_dir_buf[length] = 0;
		out_share_dir = resolved_shared_data_dir_buf;
		return true;
	}
#endif
	return false;
}

string Simulator::SearchSharedDataFile(const char *filename) const
{
	string s(filename);
	if(access(s.c_str(), F_OK) == 0)
	{
		return s;
	}

	stringstream sstr;
	sstr << shared_data_dir << "/" << filename;
	s = sstr.str();
	if(access(s.c_str(), F_OK) == 0)
	{
		return s;
	}
	
	return string(filename);
}

template <typename T>
T Simulator::GetVariable(const char *variable_name, const T *t) const
{
	return (T) *FindVariable(variable_name);
}

template bool Simulator::GetVariable(const char *, const bool *) const;
template char Simulator::GetVariable(const char *, const char *) const;
template short Simulator::GetVariable(const char *, const short *) const;
template int Simulator::GetVariable(const char *, const int *) const;
template long Simulator::GetVariable(const char *, const long *) const;
template long long Simulator::GetVariable(const char *, const long long *) const;
template unsigned char Simulator::GetVariable(const char *, const unsigned char *) const;
template unsigned short Simulator::GetVariable(const char *, const unsigned short *) const;
template unsigned int Simulator::GetVariable(const char *, const unsigned int *) const;
template unsigned long Simulator::GetVariable(const char *, const unsigned long *) const;
template float Simulator::GetVariable(const char *, const float *) const;
template double Simulator::GetVariable(const char *, const double *) const;
template string Simulator::GetVariable(const char *, const string *) const;

void Simulator::SetVariable(const char *variable_name, const char *variable_value)
{
	VariableBase *variable = FindVariable(variable_name);
	if(variable == void_variable)
	{
		std::map<string, string>::iterator set_vars_it = set_vars.find(string(variable_name));
		
		if(set_vars_it != set_vars.end())
		{
			set_vars.erase(set_vars_it);
		}
		set_vars.insert(std::pair<string, string>(string(variable_name), string(variable_value)));
	}
	else
	{
		*variable = variable_value;
	}
}

void Simulator::SetVariable(const char *variable_name, bool variable_value)
{
	SetVariable(variable_name, variable_value ? "true" : "false");
}

void Simulator::SetVariable(const char *variable_name, char variable_value)
{
	SetVariable(variable_name, (long long) variable_value);
}

void Simulator::SetVariable(const char *variable_name, unsigned char variable_value)
{
	SetVariable(variable_name, (unsigned long long) variable_value);
}

void Simulator::SetVariable(const char *variable_name, short variable_value)
{
	SetVariable(variable_name, (long long) variable_value);
}

void Simulator::SetVariable(const char *variable_name, unsigned short variable_value)
{
	SetVariable(variable_name, (unsigned long long) variable_value);
}

void Simulator::SetVariable(const char *variable_name, int variable_value)
{
	SetVariable(variable_name, (long long) variable_value);
}

void Simulator::SetVariable(const char *variable_name, unsigned int variable_value)
{
	SetVariable(variable_name, (unsigned long long) variable_value);
}

void Simulator::SetVariable(const char *variable_name, long variable_value)
{
	SetVariable(variable_name, (long long) variable_value);
}

void Simulator::SetVariable(const char *variable_name, unsigned long variable_value)
{
	SetVariable(variable_name, (unsigned long long) variable_value);
}

void Simulator::SetVariable(const char *variable_name, unsigned long long variable_value)
{
	stringstream sstr;
	sstr << variable_value;
	SetVariable(variable_name, sstr.str().c_str());
}

void Simulator::SetVariable(const char *variable_name, long long variable_value)
{
	stringstream sstr;
	sstr << variable_value;
	SetVariable(variable_name, sstr.str().c_str());
}

void Simulator::SetVariable(const char *variable_name, float variable_value)
{
	SetVariable(variable_name, (double) variable_value);
}

void Simulator::SetVariable(const char *variable_name, double variable_value)
{
	stringstream sstr;
	sstr << variable_value;
	SetVariable(variable_name, sstr.str().c_str());
}

void Simulator::GenerateLatexDocumentation(ostream& os) const
{
	map<const char *, Object *, ltstr>::const_iterator object_iter;

	os << "\\section{Introduction}" << endl;
	os << string_to_latex(description.c_str()) << endl;

	os << "\\section{Licensing}" << endl;
	os << string_to_latex(program_name.c_str()) << " " << version << "\\\\" << endl;
	os << string_to_latex(copyright.c_str()) << "\\\\" << endl;
	os << "License: " << string_to_latex(license.c_str()) << "\\\\" << endl;
	os << "Authors: " << string_to_latex(authors.c_str()) << "\\\\" << endl;
	
	os << "\\section{Simulated configuration}" << endl;
	
	os << "\\noindent The simulator is composed of the following modules and services:" << endl;
	os << "\\begin{itemize}\\addtolength{\\itemsep}{-0.40\\baselineskip}" << endl;
	for(object_iter = objects.begin(); object_iter != objects.end(); object_iter++)
	{
		const char *obj_description = (*object_iter).second->GetDescription();
		os << "\\item " << string_to_latex((*object_iter).second->GetName());
		if(*obj_description != 0) // not empty description
		{
			os << ": " << string_to_latex(obj_description);
		}
		os << std::endl;
	}
	os << "\\end{itemize}" << endl;

	os << "\\section{Using " << program_name << "}" << endl;
	os << "\\noindent Usage: \\texttt{" << program_binary << " [<options>] [...]}" << endl << endl;
	os << "\\noindent Options:" << endl;
	os << "\\begin{itemize}" << endl;

	std::vector<CommandLineOption>::const_iterator cmd_opt_iter;
	for(cmd_opt_iter = command_line_options.begin(); cmd_opt_iter != command_line_options.end(); cmd_opt_iter++)
	{
		os << "\\item \\texttt{";
		os << "--" << string_to_latex(cmd_opt_iter->GetLongName());
		if(cmd_opt_iter->HasArgument())
		{
			os << " $<$" << string_to_latex(cmd_opt_iter->GetArgumentDescription()) << "$>$";
		}
		os << " or -" << cmd_opt_iter->GetShortName();
		if(cmd_opt_iter->HasArgument())
		{
			os << " $<$" << string_to_latex(cmd_opt_iter->GetArgumentDescription()) << "$>$";
		}
		os << "}: " << string_to_latex(cmd_opt_iter->GetOptionDescription());
		os << "" << std::endl;
	}
	os << "\\end{itemize}" << endl;

	
	// 	std::stringstream sstr_version;
// 	Version(sstr_version);
// 	os << "\\section{Version}" << std::endl;
// 	os << string_to_latex(sstr_version.str().c_str()) << std::endl;
// 	
// 	std::stringstream sstr_help;
// 	Help(sstr_help);
// 	os << "\\section{Usage}" << std::endl;
// 	os << string_to_latex(sstr_help.str().c_str(), 80, "texttt") << std::endl;

	os << "\\section{Configuration}" << std::endl;
	os << "\\tablehead{\\hline}" << std::endl;
	os << "\\tabletail{\\hline}" << std::endl;
	os << "\\begin{supertabular}{|p{7.5cm}|p{7.5cm}|}" << std::endl;
	os << "\\hline" << std::endl;

	for(object_iter = objects.begin(); object_iter != objects.end(); object_iter++)
	{
		(*object_iter).second->GenerateLatexDocumentation(os);
		os << "\\hline" << std::endl;
	}

	os << "\\hline" << std::endl;
	os << "\\end{supertabular}" << std::endl;
}

} // end of namespace service
} // end of namespace kernel
} // end of namespace unisim
