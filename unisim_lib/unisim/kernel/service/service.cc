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
 
#include "unisim/kernel/service/service.hh"
#include <sstream>
#include <fstream>
#include <stdlib.h>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/graph/graphviz.hpp>

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
//=                             ParameterBase                                 =
//=============================================================================
	
ParameterBase::ParameterBase(const char *_name, Object *_owner, const char *_description) :
	name(_owner ? _owner->GetName() + string(".") + string(_name) : _name), 
	description(_description ? _description : ""),
	owner(_owner)
{
	ServiceManager::Register(this);
}

ParameterBase::ParameterBase() :
	name(), owner(0), description()
{
}

ParameterBase::~ParameterBase()
{
	if(owner) ServiceManager::Unregister(this);
}

const char *ParameterBase::GetName() const
{
	return name.c_str();
}

const char *ParameterBase::GetDescription() const
{
	return description.c_str();
}

ParameterBase::operator bool () const { return false; }
ParameterBase::operator char () const { return (long long) *this; }
ParameterBase::operator short () const { return (long long) *this; }
ParameterBase::operator int () const { return (long long) *this; }
ParameterBase::operator long () const { return (long long) *this; }
ParameterBase::operator long long () const { return 0LL; }
ParameterBase::operator unsigned char () const { return (unsigned long long) *this; }
ParameterBase::operator unsigned short () const { return (unsigned long long) *this; }
ParameterBase::operator unsigned int () const { return (unsigned long long) *this; }
ParameterBase::operator unsigned long () const { return (unsigned long long) *this; }
ParameterBase::operator unsigned long long () const { return 0ULL; }
ParameterBase::operator float () const { return (double) *this; }
ParameterBase::operator double () const { return 0.0; }
ParameterBase::operator string () const { return string(); }

ParameterBase& ParameterBase::operator = (bool value) { return *this; }
ParameterBase& ParameterBase::operator = (char value) { *this = (long long) value; return *this; }
ParameterBase& ParameterBase::operator = (short value) { *this = (long long) value; return *this; }
ParameterBase& ParameterBase::operator = (int value) { *this = (long long) value; return *this; }
ParameterBase& ParameterBase::operator = (long value) { *this = (long long) value; return *this; }
ParameterBase& ParameterBase::operator = (long long value) { return *this; }
ParameterBase& ParameterBase::operator = (unsigned char value) { *this = (unsigned long long) value; return *this; }
ParameterBase& ParameterBase::operator = (unsigned short value) { *this = (unsigned long long) value; return *this; }
ParameterBase& ParameterBase::operator = (unsigned int value) { *this = (unsigned long long) value; return *this; }
ParameterBase& ParameterBase::operator = (unsigned long value) { *this = (unsigned long long) value; return *this; }
ParameterBase& ParameterBase::operator = (unsigned long long value) { return *this; }
ParameterBase& ParameterBase::operator = (float value) { *this = (double) value; return *this; }
ParameterBase& ParameterBase::operator = (double value) { return *this; }
ParameterBase& ParameterBase::operator = (const char *value) { return *this; }

ParameterBase& ParameterBase::operator [] (unsigned int index)
{
	if(index >= 0)
	{
		cerr << "Subscript out of range" << endl;
		return ServiceManager::void_param;
	}
	return *this;
}

//=============================================================================
//=                            Parameter<TYPE>                                =
//=============================================================================

template <class TYPE>
Parameter<TYPE>::Parameter(const char *_name, Object *_owner, TYPE& _storage, const char *_description) :
	ParameterBase(_name, _owner, _description), storage(&_storage)
{
}

template <class TYPE> Parameter<TYPE>::operator bool () const { return (*storage) ? true : false; }
template <class TYPE> Parameter<TYPE>::operator long long () const { return (long long) *storage; }
template <class TYPE> Parameter<TYPE>::operator unsigned long long () const { return (unsigned long long) *storage; }
template <class TYPE> Parameter<TYPE>::operator double () const { return (double) *storage; }
template <class TYPE> Parameter<TYPE>::operator string () const { stringstream sstr; sstr << "0x" << hex << *storage; return sstr.str(); }

template <class TYPE> ParameterBase& Parameter<TYPE>::operator = (bool value) { *storage = value ? 1 : 0; return *this; }
template <class TYPE> ParameterBase& Parameter<TYPE>::operator = (long long value) { *storage = value;	return *this; }
template <class TYPE> ParameterBase& Parameter<TYPE>::operator = (unsigned long long value) { *storage = value;	return *this; }
template <class TYPE> ParameterBase& Parameter<TYPE>::operator = (double value) { *storage = (TYPE) value; return *this; }

//=============================================================================
//=                           ParameterArray<TYPE>                            =
//=============================================================================

template <class TYPE>
ParameterArray<TYPE>::ParameterArray(const char *_name, Object *_owner, TYPE *_params, unsigned int dim) :
	ParameterBase(_name, _owner),
	params()
{
	unsigned int i;
	for(i = 0; i < dim; i++)
	{
		stringstream sstr;
		
		sstr << _name << "[" << i << "]";
		params.push_back(new Parameter<TYPE>(sstr.str().c_str(), _owner, *(_params + i)));
	}
}


template <class TYPE>
ParameterArray<TYPE>::~ParameterArray()
{
	typename vector<ParameterBase *>::iterator param_iter;
	
	for(param_iter = params.begin(); param_iter != params.end(); param_iter++)
	{
		delete *param_iter;
	}
}

template <class TYPE>
ParameterBase& ParameterArray<TYPE>::operator [] (unsigned int index)
{
	if(index >= params.size())
	{
		cerr << "Subscript out of range" << endl;
		return ServiceManager::void_param;
	}
	return *params[index];
}

//=============================================================================
//=                         specialized Parameter<>                           =
//=============================================================================

template <> ParameterBase& Parameter<bool>::operator = (const char *value) { *storage = strcmp(value, "true") == 0; return *this; }
template <> ParameterBase& Parameter<char>::operator = (const char *value) { *storage = strtoll(value, 0, 0); return *this; }
template <> ParameterBase& Parameter<short>::operator = (const char *value) { *storage = strtoll(value, 0, 0); return *this; }
template <> ParameterBase& Parameter<int>::operator = (const char *value) {	*storage = strtoll(value, 0, 0); return *this; }
template <> ParameterBase& Parameter<long>::operator = (const char *value) { *storage = strtoll(value, 0, 0); return *this; }
template <> ParameterBase& Parameter<long long>::operator = (const char *value) { *storage = strtoll(value, 0, 0); return *this; }
template <> ParameterBase& Parameter<unsigned char>::operator = (const char *value) { *storage = strtoull(value, 0, 0); return *this; }
template <> ParameterBase& Parameter<unsigned short>::operator = (const char *value) { *storage = strtoull(value, 0, 0); return *this; }
template <> ParameterBase& Parameter<unsigned int>::operator = (const char *value) { *storage = strtoull(value, 0, 0); return *this; }
template <> ParameterBase& Parameter<unsigned long>::operator = (const char *value) { *storage = strtoull(value, 0, 0); return *this; }
template <> ParameterBase& Parameter<unsigned long long>::operator = (const char *value) { *storage = strtoull(value, 0, 0); return *this; }
template <> ParameterBase& Parameter<float>::operator = (const char *value) { *storage = strtod(value, 0); return *this; }
template <> ParameterBase& Parameter<double>::operator = (const char *value) { *storage = strtod(value, 0); return *this; }

template <> Parameter<string>::Parameter(const char *_name, Object *_owner, string& _storage, const char *_description) :
	ParameterBase(_name, _owner, _description), storage(&_storage)
{
}

template <> Parameter<string>::operator bool () const { return *storage == string("true"); }
template <> Parameter<string>::operator long long () const { return strtoll(storage->c_str(), 0, 0); }
template <> Parameter<string>::operator unsigned long long () const { return strtoull(storage->c_str(), 0, 0); }
template <> Parameter<string>::operator double () const { return strtod(storage->c_str(), 0); }
template <> Parameter<string>::operator string () const { return *storage; }

template <> ParameterBase& Parameter<string>::operator = (bool value) { *storage = value ? "true" : "false"; return *this; }
template <> ParameterBase& Parameter<string>::operator = (long long value) { stringstream sstr; sstr << "0x" << hex << value; *storage = sstr.str(); return *this; }
template <> ParameterBase& Parameter<string>::operator = (unsigned long long value) { stringstream sstr; sstr << "0x" << hex << value; *storage = sstr.str(); return *this; }
template <> ParameterBase& Parameter<string>::operator = (double value) { stringstream sstr; sstr << value; *storage = sstr.str(); return *this; }
template <> ParameterBase& Parameter<string>::operator = (const char *value) { *storage = value; return *this; }

//=============================================================================
//=                       template instanciations                             =
//=============================================================================

template class Parameter<bool>;
template class Parameter<char>;
template class Parameter<short>;
template class Parameter<int>;
template class Parameter<long>;
template class Parameter<long long>;
template class Parameter<unsigned char>;
template class Parameter<unsigned short>;
template class Parameter<unsigned int>;
template class Parameter<unsigned long>;
template class Parameter<unsigned long long>;
template class Parameter<float>;
template class Parameter<double>;
template class Parameter<string>;

template class ParameterArray<bool>;
template class ParameterArray<char>;
template class ParameterArray<short>;
template class ParameterArray<int>;
template class ParameterArray<long>;
template class ParameterArray<long long>;
template class ParameterArray<unsigned char>;
template class ParameterArray<unsigned short>;
template class ParameterArray<unsigned int>;
template class ParameterArray<unsigned long>;
template class ParameterArray<unsigned long long>;
template class ParameterArray<float>;
template class ParameterArray<double>;
template class ParameterArray<string>;

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

ParameterBase& Object::operator [] (const char *name)
{
	string fullname = GetName() + string(".") + string(name);
	ParameterBase *param = ServiceManager::GetParameter(fullname.c_str());
	return *param;
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
map<const char *, ParameterBase *, ServiceManager::ltstr> ServiceManager::params;
ParameterBase ServiceManager::void_param;

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

void ServiceManager::Register(ParameterBase *param)
{
	if(params.find(param->GetName()) != params.end())
	{
		cerr << "ERROR! Parameter \"" << param->GetName() << "\" already exists" << endl;
		exit(1);
	}

	params[param->GetName()] = param;
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

void ServiceManager::Unregister(ParameterBase *param)
{
	map<const char *, ParameterBase *, ltstr>::iterator param_iter;
	param_iter = params.find(param->GetName());
	if(param_iter != params.end()) params.erase(param_iter);
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
	map<const char *, ParameterBase *, ltstr>::iterator param_iter;
	
	for(param_iter = params.begin(); param_iter != params.end(); param_iter++)
	{
		os << (*param_iter).second->GetName() << " = \"" << ((string) *(*param_iter).second) << "\"" << endl;
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

void ServiceManager::DumpParameters(ostream &os) {
	os << "PARAMETERS:" << endl;
	map<const char *, ParameterBase *, ltstr>::iterator param_iter;

	for(param_iter = params.begin(); param_iter != params.end(); param_iter++)
	{
		os << (*param_iter).second->GetName() << " = \"" << ((string) *(*param_iter).second) << "\"" << endl;
	}
	os << endl;
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

ParameterBase *ServiceManager::GetParameter(const char *name)
{
	map<const char *, ParameterBase *, ltstr>::iterator param_iter;
	
	param_iter = params.find(name);
	
	if(param_iter != params.end()) return (*param_iter).second;
	
	cerr << "ConfigManager: unknown parameter \"" << name << "\"" << endl;
	return &void_param;
}

} // end of namespace service
} // end of namespace kernel
} // end of namespace unisim
