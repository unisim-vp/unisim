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
 
#include "unisim/kernel/service/service.hh"
#include <sstream>
#include <fstream>
#include <stdlib.h>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/graph/graphviz.hpp>

#include "unisim/kernel/service/xml_helper.hh"

namespace unisim {
namespace kernel {
namespace service {

using std::hex;
using std::cerr;
using std::endl;
using std::stringstream;
using std::ofstream;
using namespace boost;

//=============================================================================
//=                             VariableBase                                 =
//=============================================================================
	
VariableBase::VariableBase(const char *_name, Object *_owner, Type _type, const char *_description) :
	name(_owner ? _owner->GetName() + string(".") + string(_name) : _name), 
	description(_description ? _description : ""),
	owner(_owner),
	type(_type),
	enumerated_values()
{
	ServiceManager::Register(this);
}

VariableBase::VariableBase() :
	name(), owner(0), description()
{
}

VariableBase::~VariableBase()
{
	if(owner) ServiceManager::Unregister(this);
}

const char *VariableBase::GetName() const
{
	return name.c_str();
}

const char *VariableBase::GetDescription() const
{
	return description.c_str();
}

VariableBase::Type VariableBase::GetType() const
{
	return type;
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
	for(int i = 0; i < enumerated_values.size(); i++) {
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

VariableBase& VariableBase::operator = (bool value) { return *this; }
VariableBase& VariableBase::operator = (char value) { *this = (long long) value; return *this; }
VariableBase& VariableBase::operator = (short value) { *this = (long long) value; return *this; }
VariableBase& VariableBase::operator = (int value) { *this = (long long) value; return *this; }
VariableBase& VariableBase::operator = (long value) { *this = (long long) value; return *this; }
VariableBase& VariableBase::operator = (long long value) { return *this; }
VariableBase& VariableBase::operator = (unsigned char value) { *this = (unsigned long long) value; return *this; }
VariableBase& VariableBase::operator = (unsigned short value) { *this = (unsigned long long) value; return *this; }
VariableBase& VariableBase::operator = (unsigned int value) { *this = (unsigned long long) value; return *this; }
VariableBase& VariableBase::operator = (unsigned long value) { *this = (unsigned long long) value; return *this; }
VariableBase& VariableBase::operator = (unsigned long long value) { return *this; }
VariableBase& VariableBase::operator = (float value) { *this = (double) value; return *this; }
VariableBase& VariableBase::operator = (double value) { return *this; }
VariableBase& VariableBase::operator = (const char *value) { return *this; }

VariableBase& VariableBase::operator [] (unsigned int index)
{
	if(index >= 0)
	{
		cerr << "Subscript out of range" << endl;
		return ServiceManager::void_variable;
	}
	return *this;
}

//=============================================================================
//=                            Variable<TYPE>                                =
//=============================================================================

template <class TYPE>
Variable<TYPE>::Variable(const char *_name, Object *_owner, TYPE& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, "string", type, _description), storage(&_storage)
{}

template <class TYPE> Variable<TYPE>::operator bool () const { return (*storage) ? true : false; }
template <class TYPE> Variable<TYPE>::operator long long () const { return (long long) *storage; }
template <class TYPE> Variable<TYPE>::operator unsigned long long () const { return (unsigned long long) *storage; }
template <class TYPE> Variable<TYPE>::operator double () const { return (double) *storage; }
template <class TYPE> Variable<TYPE>::operator string () const { stringstream sstr; sstr << "0x" << hex << *storage; return sstr.str(); }

template <class TYPE> VariableBase& Variable<TYPE>::operator = (bool value) { *storage = value ? 1 : 0; return *this; }
template <class TYPE> VariableBase& Variable<TYPE>::operator = (long long value) { *storage = value;	return *this; }
template <class TYPE> VariableBase& Variable<TYPE>::operator = (unsigned long long value) { *storage = value;	return *this; }
template <class TYPE> VariableBase& Variable<TYPE>::operator = (double value) { *storage = (TYPE) value; return *this; }

//=============================================================================
//=                           VariableArray<TYPE>                            =
//=============================================================================


//=============================================================================
//=                         specialized Variable<>                           =
//=============================================================================

template <>
Variable<bool>::Variable(const char *_name, Object *_owner, bool& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type,  _description), storage(&_storage)
{}

template <>
const char *Variable<bool>::GetDataTypeName() const
{
	return "bool";
}

template <>
Variable<char>::Variable(const char *_name, Object *_owner, char& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{}

template <>
const char *Variable<char>::GetDataTypeName() const
{
	return "char";
}

template <>
Variable<short>::Variable(const char *_name, Object *_owner, short& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{}

template <>
const char *Variable<short>::GetDataTypeName() const
{
	return "short";
}

template <>
Variable<int>::Variable(const char *_name, Object *_owner, int& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{}

template <>
const char *Variable<int>::GetDataTypeName() const
{
	return "int";
}

template <>
Variable<long>::Variable(const char *_name, Object *_owner, long& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{}

template <>
const char *Variable<long>::GetDataTypeName() const
{
	return "long";
}

template <>
Variable<long long>::Variable(const char *_name, Object *_owner, long long& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{}

template <>
const char *Variable<long long>::GetDataTypeName() const
{
	return "long long";
}


template <>
Variable<unsigned char>::Variable(const char *_name, Object *_owner, unsigned char& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{}

template <>
const char *Variable<unsigned char>::GetDataTypeName() const
{
	return "unsigned char";
}

template <>
Variable<unsigned short>::Variable(const char *_name, Object *_owner, unsigned short& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{}

template <>
const char *Variable<unsigned short>::GetDataTypeName() const
{
	return "unsigned short";
}

template <>
Variable<unsigned int>::Variable(const char *_name, Object *_owner, unsigned int& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{}

template <>
const char *Variable<unsigned int>::GetDataTypeName() const
{
	return "unsigned int";
}

template <>
Variable<unsigned long>::Variable(const char *_name, Object *_owner, unsigned long& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{}

template <>
const char *Variable<unsigned long>::GetDataTypeName() const
{
	return "unsigned long";
}

template <>
Variable<unsigned long long>::Variable(const char *_name, Object *_owner, unsigned long long& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{}

template <>
const char *Variable<unsigned long long>::GetDataTypeName() const
{
	return "unsigned long long";
}

template <> 
Variable<double>::Variable(const char *_name, Object *_owner, double& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{}

template <>
const char *Variable<double>::GetDataTypeName() const
{
	return "double";
}

template <> 
Variable<float>::Variable(const char *_name, Object *_owner, float& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{}

template <>
const char *Variable<float>::GetDataTypeName() const
{
	return "float";
}

template <> 
Variable<string>::Variable(const char *_name, Object *_owner, string& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{}

template <>
const char *Variable<string>::GetDataTypeName() const
{
	return "string";
}

template <> Variable<bool>::operator string () const { stringstream sstr; sstr << (*storage?"true":"false"); return sstr.str(); }

template <> VariableBase& Variable<bool>::operator = (const char *value) { *storage = strcmp(value, "true") == 0; return *this; }
template <> VariableBase& Variable<char>::operator = (const char *value) { *storage = strtoll(value, 0, 0); return *this; }
template <> VariableBase& Variable<short>::operator = (const char *value) { *storage = strtoll(value, 0, 0); return *this; }
template <> VariableBase& Variable<int>::operator = (const char *value) {	*storage = strtoll(value, 0, 0); return *this; }
template <> VariableBase& Variable<long>::operator = (const char *value) { *storage = strtoll(value, 0, 0); return *this; }
template <> VariableBase& Variable<long long>::operator = (const char *value) { *storage = strtoll(value, 0, 0); return *this; }
template <> VariableBase& Variable<unsigned char>::operator = (const char *value) { *storage = strtoull(value, 0, 0); return *this; }
template <> VariableBase& Variable<unsigned short>::operator = (const char *value) { *storage = strtoull(value, 0, 0); return *this; }
template <> VariableBase& Variable<unsigned int>::operator = (const char *value) { *storage = strtoull(value, 0, 0); return *this; }
template <> VariableBase& Variable<unsigned long>::operator = (const char *value) { *storage = strtoull(value, 0, 0); return *this; }
template <> VariableBase& Variable<unsigned long long>::operator = (const char *value) { *storage = strtoull(value, 0, 0); return *this; }
template <> VariableBase& Variable<float>::operator = (const char *value) { *storage = strtod(value, 0); return *this; }
template <> VariableBase& Variable<double>::operator = (const char *value) { *storage = strtod(value, 0); return *this; }

template <> Variable<string>::operator bool () const { return *storage == string("true"); }
template <> Variable<string>::operator long long () const { return strtoll(storage->c_str(), 0, 0); }
template <> Variable<string>::operator unsigned long long () const { return strtoull(storage->c_str(), 0, 0); }
template <> Variable<string>::operator double () const { return strtod(storage->c_str(), 0); }
template <> Variable<string>::operator string () const { return *storage; }

template <> VariableBase& Variable<string>::operator = (bool value) { *storage = value ? "true" : "false"; return *this; }
template <> VariableBase& Variable<string>::operator = (long long value) { stringstream sstr; sstr << "0x" << hex << value; *storage = sstr.str(); return *this; }
template <> VariableBase& Variable<string>::operator = (unsigned long long value) { stringstream sstr; sstr << "0x" << hex << value; *storage = sstr.str(); return *this; }
template <> VariableBase& Variable<string>::operator = (double value) { stringstream sstr; sstr << value; *storage = sstr.str(); return *this; }
template <> VariableBase& Variable<string>::operator = (const char *value) { *storage = value; return *this; }

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

//=============================================================================
//=                                 Object                                    =
//=============================================================================

unsigned int Object::next_id = 0;

Object::Object(const char *_name, Object *_parent) :
	object_name(_parent ? (string(_parent->GetName()) + "." + _name) : _name),
	parent(_parent),
	srv_imports(),
	srv_exports(),
	leaf_objects(),
	id(next_id++)
{
	if(_parent) _parent->Add(*this);
	ServiceManager::Register(this);
}

Object::~Object()
{
	if(parent) parent->Remove(*this);
	ServiceManager::Unregister(this);
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

const list<Object *>& Object::GetLeafs() const
{
	return leaf_objects;
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

void Object::Disconnect()
{
	list<ServiceImportBase *>::iterator import_iter;

	for(import_iter = srv_imports.begin(); import_iter != srv_imports.end(); import_iter++)
	{
		(*import_iter)->DisconnectService();
	}

	list<ServiceExportBase *>::iterator export_iter;

	for(export_iter = srv_exports.begin(); export_iter != srv_exports.end(); export_iter++)
	{
		(*export_iter)->DisconnectClient();
	}
	//cerr << "After Disconnecting " << GetName() << endl;
	//ServiceManager::Dump(cerr);
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
	VariableBase *variable = ServiceManager::GetVariable(fullname.c_str());
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

unsigned int Object::GetID() const
{
	return id;
}

//=============================================================================
//=                           ServiceImportBase                               =
//=============================================================================

ServiceImportBase::ServiceImportBase(const char *_name, Object *_owner) :
	name(string(_owner->GetName()) + "." + _name),
	owner(_owner)
{
	_owner->Add(*this);
	ServiceManager::Register(this);
}

ServiceImportBase::~ServiceImportBase()
{
	if(owner) owner->Remove(*this);
	ServiceManager::Unregister(this);
}

const char *ServiceImportBase::GetName() const
{
	return name.c_str();
}

//=============================================================================
//=                           ServiceExportBase                               =
//=============================================================================

ServiceExportBase::ServiceExportBase(const char *_name, Object *_owner) :
	name(string(_owner->GetName()) + "." + _name),
	owner(_owner)
{
	_owner->Add(*this);
	ServiceManager::Register(this);
}

ServiceExportBase::~ServiceExportBase()
{
	if(owner) owner->Remove(*this);
	ServiceManager::Unregister(this);
}

const char *ServiceExportBase::GetName() const
{
	return name.c_str();
}

//=============================================================================
//=                             ServiceManager                                =
//=============================================================================

map<const char *, Object *, ServiceManager::ltstr> ServiceManager::objects;
map<const char *, ServiceImportBase *, ServiceManager::ltstr> ServiceManager::imports;
map<const char *, ServiceExportBase *, ServiceManager::ltstr> ServiceManager::exports;
map<const char *, VariableBase *, ServiceManager::ltstr> ServiceManager::variables;
VariableBase ServiceManager::void_variable;

void ServiceManager::Register(Object *object)
{
	if(objects.find(object->GetName()) != objects.end())
	{
		cerr << "ERROR! Object \"" << object->GetName() << "\" already exists" << endl;
		Dump(cerr);
		exit(1);
	}
	
	objects[object->GetName()] = object;
}

void ServiceManager::Register(VariableBase *variable)
{
	if(variables.find(variable->GetName()) != variables.end())
	{
		cerr << "ERROR! Variable \"" << variable->GetName() << "\" already exists" << endl;
		exit(1);
	}

	variables[variable->GetName()] = variable;
}

void ServiceManager::Register(ServiceImportBase *srv_import)
{
	if(imports.find(srv_import->GetName()) != imports.end())
	{
		cerr << "ERROR! Import \"" << srv_import->GetName() << "\" already exists" << endl;
		exit(1);
	}

	imports[srv_import->GetName()] = srv_import;
}

void ServiceManager::Register(ServiceExportBase *srv_export)
{
	if(exports.find(srv_export->GetName()) != exports.end())
	{
		cerr << "ERROR! Export \"" << srv_export->GetName() << "\" already exists" << endl;
		exit(1);
	}

	exports[srv_export->GetName()] = srv_export;
}

void ServiceManager::Unregister(Object *object)
{
	map<const char *, Object *, ltstr>::iterator object_iter;
	object_iter = objects.find(object->GetName());
	if(object_iter != objects.end())
	{
		objects.erase(object_iter);
	}
}

void ServiceManager::Unregister(VariableBase *variable)
{
	map<const char *, VariableBase *, ltstr>::iterator variable_iter;
	variable_iter = variables.find(variable->GetName());
	if(variable_iter != variables.end()) variables.erase(variable_iter);
}

void ServiceManager::Unregister(ServiceImportBase *srv_import)
{
	map<const char *, ServiceImportBase *, ltstr>::iterator import_iter;
	import_iter = imports.find(srv_import->GetName());
	if(import_iter != imports.end()) imports.erase(import_iter);
}

void ServiceManager::Unregister(ServiceExportBase *srv_export)
{
	map<const char *, ServiceExportBase *, ltstr>::iterator export_iter;
	export_iter = exports.find(srv_export->GetName());
	if(export_iter != exports.end()) exports.erase(export_iter);
}

void ServiceManager::Dump(ostream& os)
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

void ServiceManager::DumpVariables(ostream &os) {
	os << "PARAMETERS:" << endl;
	map<const char *, VariableBase *, ltstr>::iterator variable_iter;

	for(variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		os << (*variable_iter).second->GetName() << " = \"" << ((string) *(*variable_iter).second) << "\"" << endl;
	}
	os << endl;
}

bool ServiceManager::XmlfyParameters(const char *filename) {
	XMLHelper xml_helper;
	
	return xml_helper.XmlfyParameters(filename);
}

bool ServiceManager::LoadXmlParameters(const char *filename) {
	XMLHelper xml_helper;
	
	return xml_helper.LoadXmlParameters(filename);
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

bool ServiceManager::Setup()
{
	map<const char *, Object *, ltstr>::iterator object_iter;
	DependencyGraph dependency_graph(objects.size());
	
	for(object_iter = objects.begin(); object_iter != objects.end(); object_iter++)
	{
		dependency_graph[(*object_iter).second->GetID()].obj = (*object_iter).second;
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
				add_edge(peer_object->GetID(), (*object_iter).second->GetID(), dependency_graph);
//				cerr << peer_object->GetID() << ":" << peer_object->GetName() << "->" 
//					<< (*object_iter).second->GetName() << ":" << (*object_iter).second->GetName() << endl;
			}
		}

	}

// 	ofstream of("deps.dot");
// 	boost::write_graphviz(of, dependency_graph);

	bool has_cycle = false;
	CycleDetector vis(has_cycle);
	boost::depth_first_search(dependency_graph, visitor(vis));

	if(has_cycle)
	{
		cerr << "ServiceManager: ERROR! cyclic setup dependency graph" << endl;
		return false;
	}

	typedef list<Vertex> SetupOrder;
	SetupOrder setup_order;
	topological_sort(dependency_graph, std::front_inserter(setup_order));

	list<Vertex>::iterator vertex_iter;
	for(vertex_iter = setup_order.begin(); vertex_iter != setup_order.end(); vertex_iter++)
	{
//		cerr << "ServiceManager:" <<  dependency_graph[*vertex_iter].obj->GetName() << "::Setup()" << endl;
		if(!dependency_graph[*vertex_iter].obj->Setup())
		{
			cerr << "ServiceManager: " << dependency_graph[*vertex_iter].obj->GetName() << " setup failed" << endl;
			return false;
		}
	}

	return true;
}

VariableBase *ServiceManager::GetVariable(const char *name)
{
	map<const char *, VariableBase *, ltstr>::iterator variable_iter;
	
	variable_iter = variables.find(name);
	
	if(variable_iter != variables.end()) return (*variable_iter).second;
	
	cerr << "ConfigManager: unknown variable \"" << name << "\"" << endl;
	return &void_variable;
}

} // end of namespace service
} // end of namespace kernel
} // end of namespace unisim
