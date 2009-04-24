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
 * Authors: Gilles Mouchard (gilles.mouchar@cea.fr)
 *          Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

#ifndef __UNISIM_KERNEL_SERVICE_SERVICE_HH__
#define __UNISIM_KERNEL_SERVICE_SERVICE_HH__

#include <iostream>
#include <string>
#include <list>
#include <queue>
#include <map>
#include <vector>
#include <sstream>
#include <string.h>

namespace unisim {
namespace kernel {
namespace service {

using std::cout;
using std::endl;
using std::map;
using std::string;
using std::list;
using std::queue;
using std::cerr;
using std::endl;
using std::stringstream;
using std::ostream;
using std::vector;

class EmptyClientInterface {};
class Object;
class VariableBase;
template <class TYPE> class Variable;
template <class TYPE> class VariableArray;
template <class SERVICE_IF> class Client;
template <class SERVICE_IF> class Service;
class ServiceImportBase;
class ServiceExportBase;
template <class SERVICE_IF> class ServiceImport;
template <class SERVICE_IF> class ServiceExport;

// Service import/export connection operators
template <class SERVICE_IF>
ServiceExport<SERVICE_IF>& operator >> (ServiceImport<SERVICE_IF>& lhs, ServiceExport<SERVICE_IF>& rhs);

template <class SERVICE_IF>
ServiceImport<SERVICE_IF>& operator << (ServiceExport<SERVICE_IF>& lhs, ServiceImport<SERVICE_IF>& rhs);

template <class SERVICE_IF>
ServiceImport<SERVICE_IF>& operator >> (ServiceImport<SERVICE_IF>& lhs, ServiceImport<SERVICE_IF>& rhs);

template <class SERVICE_IF>
ServiceImport<SERVICE_IF>& operator << (ServiceImport<SERVICE_IF>& lhs, ServiceImport<SERVICE_IF>& rhs);

template <class SERVICE_IF>
ServiceExport<SERVICE_IF>& operator >> (ServiceExport<SERVICE_IF>& lhs, ServiceExport<SERVICE_IF>& rhs);

template <class SERVICE_IF>
ServiceExport<SERVICE_IF>& operator << (ServiceExport<SERVICE_IF>& lhs, ServiceExport<SERVICE_IF>& rhs);

//=============================================================================
//=                             VariableBase                                 =
//=============================================================================

class VariableBase
{
public:
	typedef enum { VAR_VOID, VAR_ARRAY, VAR_PARAMETER, VAR_STATISTIC, VAR_REGISTER } Type;

	VariableBase();
	VariableBase(const char *name, Object *owner, Type type, const char *description);
	virtual ~VariableBase();

	const char *GetName() const;
	const char *GetDescription() const;
	Type GetType() const;
	virtual const char *GetDataTypeName() const;
	bool HasEnumeratedValues() const;
	bool HasEnumeratedValue(const char *value) const;
	void GetEnumeratedValues(vector<string> &values) const;
	bool AddEnumeratedValue(const char *value);
	bool RemoveEnumeratedValue(const char *value);

	virtual operator bool () const;
	operator char () const;
	operator short () const;
	operator int () const;
	operator long () const;
	virtual operator long long () const;
	operator unsigned char () const;
	operator unsigned short () const;
	operator unsigned int () const;
	operator unsigned long () const;
	virtual operator unsigned long long () const;
	operator float () const;
	virtual operator double () const;
	virtual operator string () const;
	virtual VariableBase& operator = (bool value);
	VariableBase& operator = (char value);
	VariableBase& operator = (short value);
	VariableBase& operator = (int value);
	VariableBase& operator = (long value);
	virtual VariableBase& operator = (long long value);
	VariableBase& operator = (unsigned char value);
	VariableBase& operator = (unsigned short value);
	VariableBase& operator = (unsigned int value);
	VariableBase& operator = (unsigned long value);
	virtual VariableBase& operator = (unsigned long long value);
	VariableBase& operator = (float value);
	virtual VariableBase& operator = (double value);
	virtual VariableBase& operator = (const char * value);

	virtual VariableBase& operator [] (unsigned int index);
private:
	string name;
	Object *owner;
	string description;
	vector<string> enumerated_values;
	Type type;
};

//=============================================================================
//=                             ServiceManager                                =
//=============================================================================

class ServiceManager
{
public:
	static void Register(Object *object);
	static void Register(ServiceImportBase *srv_import);
	static void Register(ServiceExportBase *srv_export);
	static void Register(VariableBase *variable);

	static void Unregister(Object *object);
	static void Unregister(ServiceImportBase *srv_import);
	static void Unregister(ServiceExportBase *srv_export);
	static void Unregister(VariableBase *variable);

	static void Dump(ostream& os);
	static void DumpVariables(ostream& os, VariableBase::Type filter_type = VariableBase::VAR_VOID);
	static void DumpStatistics(ostream& os);
	static void DumpParameters(ostream& os);
	static void DumpRegisters(ostream& os);

	static bool XmlfyParameters(const char *filename);
	static bool XmlfyStatistics(const char *filename);
	static bool XmlfyRegisters(const char *filename);

	static bool LoadXmlParameters(const char *filename);
	static bool LoadXmlStatistics(const char *filename);
	static bool LoadXmlRegisters(const char *filename);

	static bool XmlfyVariables(const char *filename);
	static bool LoadXmlVariables(const char *filename);

	static bool Setup();

	static VariableBase *GetVariable(const char *name, VariableBase::Type type = VariableBase::VAR_VOID);
	static VariableBase *GetArray(const char *name);
	static VariableBase *GetParameter(const char *name);
	static VariableBase *GetRegister(const char *name);
	static VariableBase *GetStatistic(const char *name);

	static void GetVariables(list<VariableBase *>& lst, VariableBase::Type type = VariableBase::VAR_VOID);
	static void GetArrays(list<VariableBase *>& lst);
	static void GetParameters(list<VariableBase *>& lst);
	static void GetRegisters(list<VariableBase *>& lst);
	static void GetStatistics(list<VariableBase *>& lst);

	static VariableBase void_variable;

	static void GetRootObjects(list<Object *>& lst);

private:
	friend class Object;
	friend class VariableBase;
	friend class XMLHelper;

	struct ltstr
	{
		bool operator() (const char *s1, const char *s2) const
		{
			return strcmp(s1, s2) < 0;
		}
	};

	static map<const char *, Object *, ltstr> objects;
	static map<const char *, ServiceImportBase *, ltstr> imports;
	static map<const char *, ServiceExportBase *, ltstr> exports;
	static map<const char *, VariableBase *, ltstr> variables;

//
//	static void ProcessXmlVariableNode(xmlTextReaderPtr reader);
};

//=============================================================================
//=                            Variable<TYPE>                                =
//=============================================================================

template <class TYPE>
class Variable : public VariableBase
{
public:
	typedef VariableBase::Type Type;
	Variable(const char *name, Object *owner, TYPE& storage, VariableBase::Type type, const char *description = NULL);

	virtual const char *GetDataTypeName() const;
	virtual operator bool () const;
	virtual operator long long () const;
	virtual operator unsigned long long () const;
	virtual operator double () const;
	virtual operator string () const;
	virtual VariableBase& operator = (bool value);
	virtual VariableBase& operator = (long long value);
	virtual VariableBase& operator = (unsigned long long value);
	virtual VariableBase& operator = (double value);
	virtual VariableBase& operator = (const char * value);
private:
	TYPE *storage;
};

template <class TYPE>
class Parameter : public Variable<TYPE>
{
public:
	Parameter(const char *name, Object *owner, TYPE& storage, const char *description = NULL) : Variable<TYPE>(name, owner, storage, VariableBase::VAR_PARAMETER, description) {}
};

template <class TYPE>
class Statistic : public Variable<TYPE>
{
public:
	Statistic(const char *name, Object *owner, TYPE& storage, const char *description = NULL) : Variable<TYPE>(name, owner, storage, VariableBase::VAR_STATISTIC, description) {}
};

template <class TYPE>
class Register : public Register<TYPE>
{
public:
	Register(const char *name, Object *owner, TYPE& storage, const char *description = NULL) : Variable<TYPE>(name, owner, storage, VariableBase::VAR_REGISTER, description) {}
};


//=============================================================================
//=                           VariableArray<TYPE>                            =
//=============================================================================

template <class TYPE>
class VariableArray : public VariableBase
{
public:
	typedef VariableBase::Type Type;
	VariableArray(const char *name, Object *owner, TYPE *variables, unsigned int dim, Type type, const char *description = NULL);
	virtual ~VariableArray();

	virtual VariableBase& operator [] (unsigned int index);
private:
	vector<VariableBase *> variables;
};

template <class TYPE>
VariableArray<TYPE>::VariableArray(const char *_name, Object *_owner, TYPE *_variables, unsigned int dim, VariableBase::Type type, const char *_description) :
	VariableBase(_name, _owner, VariableBase::VAR_ARRAY, _description),
	variables()
{
	unsigned int i;
	for(i = 0; i < dim; i++)
	{
		stringstream sstr;
		
		sstr << _name << "[" << i << "]";
		variables.push_back(new Variable<TYPE>(sstr.str().c_str(), _owner, *(_variables + i), type, _description));
	}
}


template <class TYPE>
VariableArray<TYPE>::~VariableArray()
{
	typename vector<VariableBase *>::iterator variable_iter;
	
	for(variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		delete *variable_iter;
	}
}

template <class TYPE>
VariableBase& VariableArray<TYPE>::operator [] (unsigned int index)
{
	if(index >= variables.size())
	{
		cerr << "Subscript out of range" << endl;
		return ServiceManager::void_variable;
	}
	return *variables[index];
}


template <class TYPE>
class ParameterArray : public VariableArray<TYPE>
{
public:
	ParameterArray(const char *name, Object *owner, TYPE *parameters, unsigned int dim, const char *description = NULL) : VariableArray<TYPE>(name, owner, parameters, dim, VariableBase::VAR_PARAMETER, description) {}
};

template <class TYPE>
class StatisticArray : public VariableArray<TYPE>
{
public:
	StatisticArray(const char *name, Object *owner, TYPE *parameters, unsigned int dim, const char *description = NULL) : VariableArray<TYPE>(name, owner, parameters, dim, VariableBase::VAR_STATISTIC, description) {}
};

template <class TYPE>
class RegisterArray : public RegisterArray<TYPE>
{
public:
	RegisterArray(const char *name, Object *owner, TYPE *parameters, unsigned int dim, const char *description = NULL) : VariableArray<TYPE>(name, owner, parameters, dim, VariableBase::VAR_REGISTER, description) {}
};

//=============================================================================
//=                                 Object                                    =
//=============================================================================

class Object
{
public:
	Object(const char *name, Object *parent = 0);
	virtual ~Object();

	virtual void OnDisconnect();
	virtual bool Setup();

	const char *GetName() const;

	void Add(ServiceImportBase& srv_import);
	void Remove(ServiceImportBase& srv_import);
	void Add(ServiceExportBase& srv_export);
	void Remove(ServiceExportBase& srv_export);
	void Add(Object& object);
	void Remove(Object& object);
	void Add(VariableBase& var);
	void Remove(VariableBase& var);
	const list<ServiceImportBase *>& GetServiceImports() const;
	const list<ServiceExportBase *>& GetServiceExports() const;
	const list<Object *>& GetLeafs() const;
	const list<VariableBase *>& GetVariables() const;
	Object *GetParent() const;
	void Disconnect();
	VariableBase& operator [] (const char *name);
	list<ServiceImportBase *>& GetSetupDependencies();
	void SetupDependsOn(ServiceImportBase& srv_import);
	unsigned int GetID() const;
private:
	unsigned int id;
	static unsigned int next_id;
	string object_name;
	Object *parent;
	list<VariableBase *> variables;
	list<ServiceImportBase *> srv_imports;
	list<ServiceExportBase *> srv_exports;
	list<Object *> leaf_objects;
	list<ServiceImportBase *> setup_dependencies;
	
};

//=============================================================================
//=                            Service<SERVICE_IF>                            =
//=============================================================================

template <class SERVICE_IF>
class Service : public SERVICE_IF, virtual public Object
{
public:
	Service(const char *name, Object *parent = 0);
};

template <class SERVICE_IF>
Service<SERVICE_IF>::Service(const char *_name, Object *_parent) :
	Object(_name, _parent)
{
}

//=============================================================================
//=                           Client<SERVICE_IF>                              =
//=============================================================================

template <class SERVICE_IF>
class Client : virtual public Object
{
public:
	Client(const char *name, Object *parent = 0);
	virtual ~Client();
};

template <class SERVICE_IF>
Client<SERVICE_IF>::Client(const char *_name, Object *_parent) :
	Object(_name, _parent)
{
}

template <class SERVICE_IF>
Client<SERVICE_IF>::~Client()
{
}

//=============================================================================
//=                           ServiceImportBase                               =
//=============================================================================

class ServiceImportBase
{
public:
	ServiceImportBase(const char *name, Object *owner);
	virtual ~ServiceImportBase();
	const char *GetName() const;
	virtual void Disconnect() = 0;
	virtual void DisconnectService() = 0;
	virtual void Dump(ostream& os) const = 0;
	virtual Object *GetService() const = 0;
	virtual ServiceExportBase *GetServiceExport() = 0;
private:
	string name;
	Object *owner;
};

//=============================================================================
//=                           ServiceExportBase                               =
//=============================================================================

class ServiceExportBase
{
public:
	ServiceExportBase(const char *name, Object *owner);
	virtual ~ServiceExportBase();
	const char *GetName() const;
	virtual void Disconnect() = 0;
	virtual void DisconnectClient() = 0;
	virtual void Dump(ostream& os) const = 0;
	virtual Object *GetClient() const = 0;

	friend void operator >> (ServiceExportBase& lhs, ServiceImportBase& rhs);
	friend void operator << (ServiceImportBase& lhs, ServiceExportBase& rhs);
private:
	string name;
	Object *owner;
};

//=============================================================================
//=                       ServiceImport<SERVICE_IF>                           =
//=============================================================================

template <class SERVICE_IF>
class ServiceImport : public ServiceImportBase
{
public:
	ServiceImport(const char *name, Client<SERVICE_IF> *client);
	ServiceImport(const char *name, Object *owner);

 	inline operator SERVICE_IF * () const
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
	;

	inline SERVICE_IF *operator -> () const
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
	;

	// (import -> export) ==> export
	friend ServiceExport<SERVICE_IF>& operator >> <SERVICE_IF>(ServiceImport<SERVICE_IF>& lhs, ServiceExport<SERVICE_IF>& rhs);

	// (export <- import) ==> import
	friend ServiceImport<SERVICE_IF>& operator << <SERVICE_IF>(ServiceExport<SERVICE_IF>& lhs, ServiceImport<SERVICE_IF>& rhs);
	
	// (import1 -> import2) ==> import2
	friend ServiceImport<SERVICE_IF>& operator >> <SERVICE_IF>(ServiceImport<SERVICE_IF>& lhs, ServiceImport<SERVICE_IF>& rhs);
	
	// (import1 <- import2) ==> import2
	friend ServiceImport<SERVICE_IF>& operator << <SERVICE_IF>(ServiceImport<SERVICE_IF>& lhs, ServiceImport<SERVICE_IF>& rhs);

	virtual void Disconnect();

	virtual void DisconnectService();

	void ResolveClient();

	void Unbind(ServiceExport<SERVICE_IF>& srv_export);

	virtual void Dump(ostream& os) const;

	virtual Object *GetService() const;

	virtual ServiceExportBase *GetServiceExport();

private:
	Service<SERVICE_IF> *service;
	ServiceExport<SERVICE_IF> *srv_export;
	ServiceImport<SERVICE_IF> *alias_import;
	list<ServiceImport<SERVICE_IF> *> actual_imports;
	Client<SERVICE_IF> *client;

	Service<SERVICE_IF> *ResolveService(Client<SERVICE_IF> *client);
	ServiceExport<SERVICE_IF> *ResolveServiceExport();
	void UnresolveService();
	void Bind(ServiceExport<SERVICE_IF>& srv_export); // myself -> export
	void Bind(ServiceImport<SERVICE_IF>& alias_import);  // myself -> import
	void Unbind(ServiceImport<SERVICE_IF>& alias_import);
};

template <class SERVICE_IF>
ServiceImport<SERVICE_IF>::ServiceImport(const char *_name, Client<SERVICE_IF> *_client) :
	ServiceImportBase(_name, _client),
	service(0),
	srv_export(0),
	alias_import(0),
	actual_imports(),
	client(_client)
{
}

template <class SERVICE_IF>
ServiceImport<SERVICE_IF>::ServiceImport(const char *_name, Object *_owner) :
	ServiceImportBase(_name, _owner),
	service(0),
	srv_export(0),
	alias_import(0),
	actual_imports(),
	client(0)
{
}

template <class SERVICE_IF>
inline ServiceImport<SERVICE_IF>::operator SERVICE_IF * () const
{
	return service;
}

template <class SERVICE_IF>
inline SERVICE_IF *ServiceImport<SERVICE_IF>::operator -> () const
{
	return service;
}

template <class SERVICE_IF>
void ServiceImport<SERVICE_IF>::Bind(ServiceExport<SERVICE_IF>& srv_export)
{
	if(this->alias_import)
	{
		cerr << "WARNING! Can't connect " << GetName() << " to " << srv_export.GetName() << " because it is already connected to " << this->alias_import->GetName() << endl;
		return;
	}

	if(this->srv_export)
	{
		cerr << "WARNING! Can't connect " << GetName() << " to " << srv_export.GetName() << " because it is already connected to " << this->srv_export->GetName() << endl;
		return;
	}

	//cerr << GetName() << " -> " << srv_export.GetName() << endl;
	this->srv_export = &srv_export;
}

template <class SERVICE_IF>
void ServiceImport<SERVICE_IF>::Bind(ServiceImport<SERVICE_IF>& alias_import)
{
	if(srv_export)
	{
		cerr << "WARNING! Can't connect " << GetName() << " to " << alias_import.GetName() << " because it is already connected to " << srv_export->GetName() << endl;
		return;
	}

	if(this->alias_import)
	{
		cerr << "WARNING! Can't connect " << GetName() << " to " << alias_import.GetName() << " because it is already connected to " << this->alias_import->GetName() << endl;
		return;
	}

	//cerr << GetName() << " -> " << alias_import.GetName() << endl;
	this->alias_import = &alias_import;
	alias_import.actual_imports.push_back(this);
}

template <class SERVICE_IF>
void ServiceImport<SERVICE_IF>::Unbind(ServiceExport<SERVICE_IF>& srv_export)
{
#ifdef DEBUG_SERVICE
	cerr << GetName() << ".Unbind(" << srv_export.GetName() << ")" << endl;
#endif

	if(this->srv_export != &srv_export)
	{
		cerr << "ERROR! Can't disconnect " << GetName() << " from " << srv_export.GetName() << " because connection does not exist" << endl;
		return;
	}

	UnresolveService();

	this->srv_export = 0;
}

template <class SERVICE_IF>
Service<SERVICE_IF> *ServiceImport<SERVICE_IF>::ResolveService(Client<SERVICE_IF> *_client)
{
	if(alias_import)
		return alias_import->ResolveService(_client);
	else
		if(srv_export) return srv_export->ResolveService(_client);

	return 0;
}

template <class SERVICE_IF>
ServiceExport<SERVICE_IF> *ServiceImport<SERVICE_IF>::ResolveServiceExport()
{
	if(alias_import)
		return alias_import->ResolveServiceExport();
	else
		if(srv_export) return srv_export->ResolveServiceExport();

	return 0;
}

template <class SERVICE_IF>
ServiceExportBase *ServiceImport<SERVICE_IF>::GetServiceExport()
{
	return ResolveServiceExport();
}

template <class SERVICE_IF>
void ServiceImport<SERVICE_IF>::ResolveClient()
{
	if(actual_imports.empty())
	{
		if(client && !service)
		{
			service = ResolveService(client);
		}
	}
	else
	{
		typename list<ServiceImport<SERVICE_IF> *>::iterator import_iter;
	
		for(import_iter = actual_imports.begin(); import_iter != actual_imports.end(); import_iter++)
		{
			(*import_iter)->ResolveClient();
		}
	}
}

template <class SERVICE_IF>
void ServiceImport<SERVICE_IF>::UnresolveService()
{
#ifdef DEBUG_SERVICE
	cerr << GetName() << ".UnresolveService()" << endl;
#endif

	if(actual_imports.empty())
	{
		if(service)
		{
			service->OnDisconnect();
#ifdef DEBUG_SERVICE
			cerr << GetName() << ": Unresolving service " << service->GetName() << endl;
#endif
			service = 0;
		}
	}
	else
	{
		typename list<ServiceImport<SERVICE_IF> *>::iterator import_iter;
	
		for(import_iter = actual_imports.begin(); import_iter != actual_imports.end(); import_iter++)
		{
			(*import_iter)->UnresolveService();
		}
	}
}

template <class SERVICE_IF>
void ServiceImport<SERVICE_IF>::Disconnect()
{
#ifdef DEBUG_SERVICE
	cerr << GetName() << ".Disconnect()" << endl;
#endif

	DisconnectService();

	if(alias_import)
	{
		typename list<ServiceImport<SERVICE_IF> *>::iterator import_iter;
	
		for(import_iter = alias_import->actual_imports.begin(); import_iter != alias_import->actual_imports.end(); import_iter++)
		{
			if(*import_iter == this)
			{
				alias_import->actual_imports.erase(import_iter);
				this->alias_import = 0;
			}
		}
	}

	if(srv_export)
	{
		srv_export->Unbind(*this);
		srv_export = 0;
	}
}

template <class SERVICE_IF>
void ServiceImport<SERVICE_IF>::DisconnectService()
{
#ifdef DEBUG_SERVICE
	cerr << GetName() << ".DisconnectService()" << endl;
#endif

	if(alias_import)
	{
		alias_import->DisconnectService();
	}

	if(srv_export)
	{
		UnresolveService();
		srv_export->Unbind(*this);
		srv_export = 0;
	}
}

template <class SERVICE_IF>
void ServiceImport<SERVICE_IF>::Dump(ostream& os) const
{
	if(alias_import)
	{
		os << GetName() << " -> " << alias_import->GetName() << endl;
	}

	typename list<ServiceImport<SERVICE_IF> *>::const_iterator import_iter;

	for(import_iter = actual_imports.begin(); import_iter != actual_imports.end(); import_iter++)
	{
		os << "# " << (*import_iter)->GetName() << " -> " << GetName() << endl;
	}

	if(srv_export)
	{
		os << GetName() << " -> " << srv_export->GetName() << endl;
	}
}

template <class SERVICE_IF>
Object *ServiceImport<SERVICE_IF>::GetService() const
{
	return service;
}

//=============================================================================
//=                          ServiceExport<SERVICE_IF>                        =
//=============================================================================

template <class SERVICE_IF>
class ServiceExport : public ServiceExportBase
{
public:
	ServiceExport(const char *name, Service<SERVICE_IF> *service);
	ServiceExport(const char *name, Object *owner);

	inline bool IsConnected() const
#if defined(__GNUC__) && (__GNUC__ >= 3)
__attribute__((always_inline))
#endif
	;

	// (import -> export) ==> export
	friend ServiceExport<SERVICE_IF>& operator >> <SERVICE_IF>(ServiceImport<SERVICE_IF>& lhs, ServiceExport<SERVICE_IF>& rhs);
	
	// (export <- import) ==> import
	friend ServiceImport<SERVICE_IF>& operator << <SERVICE_IF>(ServiceExport<SERVICE_IF>& lhs, ServiceImport<SERVICE_IF>& rhs);
	
	// (export1 -> export2) ==> export2
	friend ServiceExport<SERVICE_IF>& operator >> <SERVICE_IF>(ServiceExport<SERVICE_IF>& lhs, ServiceExport<SERVICE_IF>& rhs);
	
	// (export1 <- export2) ==> export2
	friend ServiceExport<SERVICE_IF>& operator << <SERVICE_IF>(ServiceExport<SERVICE_IF>& lhs, ServiceExport<SERVICE_IF>& rhs);

	virtual void Disconnect();
	virtual void DisconnectClient();

	void Unbind(ServiceImport<SERVICE_IF>& srv_import);

	Service<SERVICE_IF> *ResolveService(Client<SERVICE_IF> *client);
	ServiceExport<SERVICE_IF> *ResolveServiceExport();

	virtual void Dump(ostream& os) const;

	virtual Object *GetClient() const;

private:
	list<ServiceExport<SERVICE_IF> *> alias_exports;
	ServiceExport<SERVICE_IF> *actual_export;
	Service<SERVICE_IF> *service;
	list<ServiceImport<SERVICE_IF> *> srv_imports;
	Client<SERVICE_IF> *client;

	void UnresolveClient();
	void Bind(ServiceImport<SERVICE_IF>& srv_import);
	void Bind(ServiceExport<SERVICE_IF>& alias_export);
	void ResolveClient();
};

template <class SERVICE_IF>
ServiceExport<SERVICE_IF>::ServiceExport(const char *_name, Service<SERVICE_IF> *_service) :
	ServiceExportBase(_name, _service),
	alias_exports(),
	actual_export(0),
	service(_service),
	srv_imports(),
	client(0)
{
}

template <class SERVICE_IF>
ServiceExport<SERVICE_IF>::ServiceExport(const char *_name, Object *_owner) :
	ServiceExportBase(_name, _owner),
	alias_exports(),
	actual_export(0),
	service(0),
	srv_imports(),
	client(0)
{
}

template <class SERVICE_IF>
inline bool ServiceExport<SERVICE_IF>::IsConnected() const
{
	return client != 0;
}

template <class SERVICE_IF>
void ServiceExport<SERVICE_IF>::Bind(ServiceImport<SERVICE_IF>& srv_import)
{
	typename list<ServiceImport<SERVICE_IF> *>::iterator import_iter;

	for(import_iter = srv_imports.begin(); import_iter != srv_imports.end(); import_iter++)
	{
		if(*import_iter == &srv_import)
		{
			cerr << "WARNING! Can't connect again " << GetName() << " to " << srv_import.GetName() << " because it is already connected" << endl;
			return;
		}
	}

	srv_imports.push_back(&srv_import);
}

template <class SERVICE_IF>
void ServiceExport<SERVICE_IF>::Bind(ServiceExport<SERVICE_IF>& alias_export)
{
	typename list<ServiceExport<SERVICE_IF> *>::iterator export_iter;

	for(export_iter = alias_exports.begin(); export_iter != alias_exports.end(); export_iter++)
	{
		if(*export_iter == &alias_export)
		{
			cerr << "WARNING! Can't connect again " << GetName() << " to " << alias_export.GetName() << " because it is already" << endl;
			return;
		}
	}

	if(alias_export.actual_export)
	{
		cerr << "WARNING! Can't connect " << GetName() << " to " << alias_export.GetName() << " because " << alias_export.GetName() << " is already connected to " << alias_export.actual_export->GetName() << endl;
		return;
	}

	alias_exports.push_back(&alias_export);
	alias_export.actual_export = this;
}

template <class SERVICE_IF>
void ServiceExport<SERVICE_IF>::Unbind(ServiceImport<SERVICE_IF>& srv_import)
{
#ifdef DEBUG_SERVICE
	cerr << GetName() << ".Unbind(" << srv_import.GetName() << ")" << endl;
#endif

	typename list<ServiceImport<SERVICE_IF> *>::iterator import_iter;

	for(import_iter = srv_imports.begin(); import_iter != srv_imports.end(); import_iter++)
	{
		if(*import_iter == &srv_import)
		{
			srv_imports.erase(import_iter);
			UnresolveClient();
			return;
		}
	}

	cerr << "WARNING! Unable to disconnect " << GetName() << " and " << srv_import.GetName() << " because connection does not exist" << endl;
}

template <class SERVICE_IF>
void ServiceExport<SERVICE_IF>::DisconnectClient()
{
#ifdef DEBUG_SERVICE
	cerr << GetName() << ".DisconnectClient()" << endl;
#endif

	typename list<ServiceExport<SERVICE_IF> *>::iterator export_iter;

	for(export_iter = alias_exports.begin(); export_iter != alias_exports.end(); export_iter++)
	{
		(*export_iter)->DisconnectClient();
	}

	typename list<ServiceImport<SERVICE_IF> *>::iterator import_iter;

	if(!srv_imports.empty())
	{
		UnresolveClient();

		for(import_iter = srv_imports.begin(); import_iter != srv_imports.end(); import_iter++)
		{
			(*import_iter)->Unbind(*this);
		}
	
		srv_imports.clear();
	}
}

template <class SERVICE_IF>
void ServiceExport<SERVICE_IF>::Disconnect()
{
#ifdef DEBUG_SERVICE
	cerr << GetName() << ".Disconnect()" << endl;
#endif

	DisconnectClient();

	typename list<ServiceExport<SERVICE_IF> *>::iterator export_iter;

	for(export_iter = alias_exports.begin(); export_iter != alias_exports.end(); export_iter++)
	{
		(*export_iter)->actual_export = 0;
	}

	alias_exports.clear();
}

template <class SERVICE_IF>
void ServiceExport<SERVICE_IF>::ResolveClient()
{
	typename list<ServiceExport<SERVICE_IF> *>::iterator export_iter;

	for(export_iter = alias_exports.begin(); export_iter != alias_exports.end(); export_iter++)
	{
		(*export_iter)->ResolveClient();
	}

	typename list<ServiceImport<SERVICE_IF> *>::iterator import_iter;

	for(import_iter = srv_imports.begin(); import_iter != srv_imports.end(); import_iter++)
	{
		(*import_iter)->ResolveClient();
	}
}

template <class SERVICE_IF>
Service<SERVICE_IF> *ServiceExport<SERVICE_IF>::ResolveService(Client<SERVICE_IF> *_client)
{
	if(actual_export)
	{
		return actual_export->ResolveService(_client);
	}

	if(service)
	{
		client = _client;
		return service;
	}
	return 0;
}

template <class SERVICE_IF>
ServiceExport<SERVICE_IF> *ServiceExport<SERVICE_IF>::ResolveServiceExport()
{
	if(actual_export)
	{
		return actual_export->ResolveServiceExport();
	}

	return this;
}

template <class SERVICE_IF>
void ServiceExport<SERVICE_IF>::UnresolveClient()
{
#ifdef DEBUG_SERVICE
	cerr << GetName() << ".UnresolveClient()" << endl;
#endif

	if(actual_export)
	{
		return actual_export->UnresolveClient();
	}

	if(client)
	{
		client->OnDisconnect();
#ifdef DEBUG_SERVICE
		cerr << GetName() << ": Unresolving client " << client->GetName() << endl;
#endif
		client = 0;
	}
}

template <class SERVICE_IF>
void ServiceExport<SERVICE_IF>::Dump(ostream& os) const
{
	if(actual_export)
	{
		cerr << GetName() << " -> " << actual_export->GetName() << endl;
	}

	typename list<ServiceExport<SERVICE_IF> *>::const_iterator export_iter;

	for(export_iter = alias_exports.begin(); export_iter != alias_exports.end(); export_iter++)
	{
		os << "# " << (*export_iter)->GetName() << " -> " << GetName() << endl;
	}
}

template <class SERVICE_IF>
Object *ServiceExport<SERVICE_IF>::GetClient() const
{
	return client;
}

//=============================================================================
//=                                Operators                                  =
//=============================================================================

// (import -> export) ==> export
template <class SERVICE_IF>
ServiceExport<SERVICE_IF>& operator >> (ServiceImport<SERVICE_IF>& lhs, ServiceExport<SERVICE_IF>& rhs)
{
	lhs.Bind(rhs);
	rhs.Bind(lhs);
	lhs.ResolveClient();
	return rhs;
}

// (export <- import) ==> import
template <class SERVICE_IF>
ServiceImport<SERVICE_IF>& operator << (ServiceExport<SERVICE_IF>& lhs, ServiceImport<SERVICE_IF>& rhs)
{
	rhs.Bind(lhs);
	lhs.Bind(rhs);
	rhs.ResolveClient();
	return rhs;
}

// (import1 -> import2) ==> import2
template <class SERVICE_IF>
ServiceImport<SERVICE_IF>& operator >> (ServiceImport<SERVICE_IF>& lhs, ServiceImport<SERVICE_IF>& rhs)
{
	lhs.Bind(rhs);
	lhs.ResolveClient();
	return rhs;
}

// (import1 <- import2) ==> import2
template <class SERVICE_IF>
ServiceImport<SERVICE_IF>& operator << (ServiceImport<SERVICE_IF>& lhs, ServiceImport<SERVICE_IF>& rhs)
{
	rhs.Bind(lhs);
	rhs.ResolveClient();
	return rhs;
}

// (export1 -> export2) ==> export2
template <class SERVICE_IF>
ServiceExport<SERVICE_IF>& operator >> (ServiceExport<SERVICE_IF>& lhs, ServiceExport<SERVICE_IF>& rhs)
{
	rhs.Bind(lhs);
	lhs.ResolveClient();
	return rhs;
}

// (export1 <- export2) ==> export2
template <class SERVICE_IF>
ServiceExport<SERVICE_IF>& operator << (ServiceExport<SERVICE_IF>& lhs, ServiceExport<SERVICE_IF>& rhs)
{
	lhs.Bind(rhs);
	rhs.ResolveClient();
	return rhs;
}

} // end of namespace service
} // end of namespace kernel
} // end of namespace unisim

#endif
