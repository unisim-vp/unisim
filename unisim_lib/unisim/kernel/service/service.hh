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

#if defined(__GNUC__) && ((__GNUC__ >= 2 && __GNUC_MINOR__ >= 96) || __GNUC__ >= 3)
#if defined(likely)
#undef likely
#endif

#if defined(unlikely)
#undef unlikely
#endif

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif

#ifdef DEBUG_MEMORY_ALLOCATION
void *operator new(std::size_t size);
void *operator new[](std::size_t size);
void operator delete(void *p, std::size_t size);
void operator delete[](void *p, std::size_t size);
#endif

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
template <class TYPE> class Parameter;
template <class TYPE> class VariableArray;
template <class TYPE> class ParameterArray;
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
	typedef enum { VAR_VOID, VAR_ARRAY, VAR_PARAMETER, VAR_STATISTIC, VAR_REGISTER, VAR_FORMULA } Type;
	typedef enum { FMT_DEFAULT, FMT_HEX, FMT_DEC } Format;

	VariableBase();
	VariableBase(const char *name, Object *owner, Type type, const char *description);
	VariableBase(const char *name, VariableBase *container, Type type, const char *description);
	virtual ~VariableBase();

	Object *GetOwner() const;
	VariableBase *GetContainer() const;
	const char *GetName() const;
	const char *GetVarName() const;
	const char *GetDescription() const;
	Type GetType() const;
	const char *GetTypeName() const;
	Format GetFormat() const;
	virtual const char *GetDataTypeName() const;
	bool HasEnumeratedValues() const;
	bool HasEnumeratedValue(const char *value) const;
	void GetEnumeratedValues(vector<string> &values) const;
	bool AddEnumeratedValue(const char *value);
	bool RemoveEnumeratedValue(const char *value);
	void SetFormat(Format fmt);
	bool IsVoid() const;

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
	virtual const VariableBase& operator [] (unsigned int index) const;
	virtual unsigned int GetLength() const;
	
	virtual VariableBase& operator = (const VariableBase& variable);
	virtual std::string GetSymbolicValue() const;
	void GenerateLatexDocumentation(std::ostream& os) const;
	
	bool IsMutable() const;
	bool IsVisible() const;
	bool IsSerializable() const;
	virtual void SetMutable(bool is_mutable);
	virtual void SetVisible(bool is_visible);
	virtual void SetSerializable(bool is_serializable);

	class Notifiable
	{
	public:
		virtual void VariableNotify(const char *name) = 0;
		virtual ~Notifiable() {};
	};
	void SetNotify(Notifiable *notifiable);
	void RemoveNotify(Notifiable *notifiable);
 	void Notify();

private:
	string name;
	string var_name;
	Object *owner;
	VariableBase *container;
	string description;
	vector<string> enumerated_values;
	Type type;
	Format fmt;
	bool is_mutable;
	bool is_visible;
	bool is_serializable;
	list<Notifiable *> notifiable_list;
};

//=============================================================================
//=                                 Simulator                                 =
//=============================================================================

class Simulator
{
public:
	typedef enum
	{
		ST_OK_TO_START,
		ST_OK_DONT_START,
		ST_WARNING,
		ST_ERROR
	} SetupStatus;
	
	static Simulator *simulator;
	VariableBase *void_variable;

	Simulator(int argc, char **argv, void (*LoadBuiltInConfig)(Simulator *simulator) = 0);
	virtual ~Simulator();
	virtual SetupStatus Setup();
	virtual void Stop(Object *object, int exit_status);

	const VariableBase *FindVariable(const char *name, VariableBase::Type type = VariableBase::VAR_VOID) const;
	VariableBase *FindVariable(const char *name, VariableBase::Type type = VariableBase::VAR_VOID);
	const VariableBase *FindArray(const char *name) const;
	VariableBase *FindArray(const char *name);
	const VariableBase *FindParameter(const char *name) const;
	VariableBase *FindParameter(const char *name);
	const VariableBase *FindRegister(const char *name) const;
	VariableBase *FindRegister(const char *name);
	const VariableBase *FindStatistic(const char *name) const;
	VariableBase *FindStatistic(const char *name);
	const VariableBase *FindFormula(const char *name) const;
	VariableBase *FindFormula(const char *name);

	void GetVariables(list<VariableBase *>& lst, VariableBase::Type type = VariableBase::VAR_VOID);
	void GetArrays(list<VariableBase *>& lst);
	void GetParameters(list<VariableBase *>& lst);
	void GetRegisters(list<VariableBase *>& lst);
	void GetStatistics(list<VariableBase *>& lst);
	void GetFormulas(list<VariableBase *>& lst);

	void Dump(ostream& os);
	void DumpVariables(ostream& os, VariableBase::Type filter_type = VariableBase::VAR_VOID);
	void DumpStatistics(ostream& os);
	void DumpParameters(ostream& os);
	void DumpRegisters(ostream& os);
	void DumpFormulas(ostream& os);

	bool GetExecutablePath(const char *argv0, std::string& out_execute_path) const;
	bool GetBinPath(const char *argv0, std::string& out_bin_dir, std::string& out_bin_program) const;
	bool GetSharePath(const std::string& bin_dir, std::string& out_share_dir) const;
	bool GetSharePath(const std::string& bin_dir, std::string& out_share_dir, const std::string& share_dir_relative_path) const;
	string SearchSharedDataFile(const char *filename) const;

	void GenerateLatexDocumentation(ostream& os) const;
	
	bool IsWarningEnabled() const;
	
private:
	friend class Object;
	friend class VariableBase;
	template <class TYPE> friend class Variable;
	friend class XMLHelper;
	friend class ServiceImportBase;
	friend class ServiceExportBase;

	string shared_data_dir;
	std::map<string, string> set_vars;
	string get_config_filename;
	bool list_parms;
	bool get_config;
	bool generate_doc;
	string generate_doc_filename;
	bool enable_warning;
	bool enable_version;
	bool enable_help;
	bool warn_get_bin_path;
	bool warn_get_share_path;
	bool enable_press_enter_at_exit;
	string bin_dir;
	string program_binary;
	string program_name;
	string authors;
	string copyright;
	string description;
	string version;
	string license;
	Parameter<string> *var_program_name;
	Parameter<string> *var_authors;
	Parameter<string> *var_copyright;
	Parameter<string> *var_description;
	Parameter<string> *var_version;
	Parameter<string> *var_license;
	Parameter<bool> *param_enable_press_enter_at_exit;
	
	void Version(ostream& os) const;
	void Help(ostream& os) const;
	
	void Register(Object *object);
	void Register(ServiceImportBase *srv_import);
	void Register(ServiceExportBase *srv_export);
	void Register(VariableBase *variable);

	void Unregister(Object *object);
	void Unregister(ServiceImportBase *srv_import);
	void Unregister(ServiceExportBase *srv_export);
	void Unregister(VariableBase *variable);
	
	void Initialize(VariableBase *variable);

	bool XmlfyParameters(const char *filename);
	bool XmlfyStatistics(const char *filename);
	bool XmlfyRegisters(const char *filename);
	bool XmlfyFormulas(const char *filename);

	bool LoadXmlParameters(const char *filename);
	bool LoadXmlStatistics(const char *filename);
	bool LoadXmlRegisters(const char *filename);
	bool LoadXmlFormulas(const char *filename);

	bool XmlfyVariables(const char *filename);
	bool LoadXmlVariables(const char *filename);

	void GetRootObjects(list<Object *>& lst);
	
	class CommandLineOption
	{
	public:
		CommandLineOption(char short_name, const char *long_name, const char *opt_description, const char *arg_description = 0);
		char GetShortName() const;
		const char *GetLongName() const;
		bool HasArgument() const;
		const char *GetArgumentDescription() const;
		const char *GetOptionDescription() const;
		int operator == (const char *arg) const;
	private:
		char short_name;
		const char *long_name;
		const char *arg_description;
		const char *opt_description;
	};

	std::vector<CommandLineOption> command_line_options;
	
	struct ltstr
	{
		bool operator() (const char *s1, const char *s2) const
		{
			return strcmp(s1, s2) < 0;
		}
	};

	map<const char *, Object *, ltstr> objects;
	map<const char *, ServiceImportBase *, ltstr> imports;
	map<const char *, ServiceExportBase *, ltstr> exports;
	map<const char *, VariableBase *, ltstr> variables;
	
	string *cmd_args;
	ParameterArray<string> *param_cmd_args;
	
public:
	template <typename T> T GetVariable(const char *variable_name, const T *t = 0) const;
	
	void SetVariable(const char *variable_name, const char *variable_value);
	void SetVariable(const char *variable_name, bool variable_value);
	void SetVariable(const char *variable_name, char variable_value);
	void SetVariable(const char *variable_name, unsigned char variable_value);
	void SetVariable(const char *variable_name, short variable_value);
	void SetVariable(const char *variable_name, unsigned short variable_value);
	void SetVariable(const char *variable_name, int variable_value);
	void SetVariable(const char *variable_name, unsigned int variable_value);
	void SetVariable(const char *variable_name, long variable_value);
	void SetVariable(const char *variable_name, unsigned long variable_value);
	void SetVariable(const char *variable_name, unsigned long long variable_value);
	void SetVariable(const char *variable_name, long long variable_value);
	void SetVariable(const char *variable_name, float variable_value);
	void SetVariable(const char *variable_name, double variable_value);
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
	Statistic(const char *name, Object *owner, TYPE& storage, const char *description = NULL) : Variable<TYPE>(name, owner, storage, VariableBase::VAR_STATISTIC, description) { VariableBase::SetFormat(unisim::kernel::service::VariableBase::FMT_DEC); }
};

template <class TYPE>
class Register : public Variable<TYPE>
{
public:
	Register(const char *name, Object *owner, TYPE& storage, const char *description = NULL) : Variable<TYPE>(name, owner, storage, VariableBase::VAR_REGISTER, description) {}
};

template <class TYPE>
class Formula;

template <class TYPE>
std::ostream& operator << (std::ostream& os, const Formula<TYPE>& formula);

template <class TYPE>
class Formula : public VariableBase
{
public:
	typedef VariableBase::Type Type;
	typedef enum
	{
		OP_ADD,
		OP_SUB,
		OP_MUL,
		OP_DIV,
		OP_LT,
		OP_LTE,
		OP_GT,
		OP_GTE,
		OP_EQ,
		OP_SEL,
		OP_NEG,
		OP_ABS,
		OP_MIN,
		OP_MAX,
		OP_AND,
		OP_OR,
		OP_NEQ,
		OP_NOT
	} Operator;
	
	Formula(const char *name, Object *owner, Operator op, VariableBase *child1, VariableBase *child2, VariableBase *child3, const char *description = 0);
	Formula(const char *name, Object *owner, Operator op, VariableBase *child1, VariableBase *child2, const char *description = 0);
	Formula(const char *name, Object *owner, Operator op, VariableBase *child, const char *description = 0);
	
#if 0
	Formula(const char *name, Object *owner, const char *math_formula, const char *description = 0);
#endif

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
	virtual std::string GetSymbolicValue() const;

private:
	bool IsValid() const;
	TYPE Compute() const;

	Operator op;
	VariableBase *childs[3];

#if 0
	static const unsigned int TOK_EOF   = 256;
	static const unsigned int TOK_ERROR = 257;
	static const unsigned int TOK_LTE   = 258;
	static const unsigned int TOK_GTE   = 259;
	static const unsigned int TOK_ABS   = 260;
	static const unsigned int TOK_MIN   = 261;
	static const unsigned int TOK_MAX   = 262;
	static const unsigned int TOK_IDENT = 263;
	static const unsigned int TOK_EXPR  = 264;

	static const unsigned int PEEK_TOK  = 0;
	static const unsigned int GET_TOK   = 1;
	
	unsigned int look_ahead_token;
	VariableBase *look_ahead_lval;

	const char *GetTokenName(unsigned int token) const;
	unsigned int ReadToken(const char *math_formula, unsigned int& pos, unsigned int mode, VariableBase **lval = 0);
	unsigned int Parse(const char *math_formula, unsigned int& pos, VariableBase **lval);
	void Initialize(Operator op, VariableBase *child1, VariableBase *child2 = 0, VariableBase *child3 = 0);
	Formula(Operator op, VariableBase *child1, VariableBase *child2 = 0, VariableBase *child3 = 0);
#endif
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
	virtual const VariableBase& operator [] (unsigned int index) const;
	void SetFormat(Format fmt);
	virtual unsigned int GetLength() const;
	virtual VariableBase& operator = (const VariableBase& variable);
	virtual const char *GetDataTypeName() const;
	virtual void SetMutable(bool is_mutable);
	virtual void SetVisible(bool is_visible);
	virtual void SetSerializable(bool is_serializable);

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
		return *Simulator::simulator->void_variable;
	}
	return *variables[index];
}

template <class TYPE>
const VariableBase& VariableArray<TYPE>::operator [] (unsigned int index) const
{
	if(index >= variables.size())
	{
		cerr << "Subscript out of range" << endl;
		return *Simulator::simulator->void_variable;
	}
	return *variables[index];
}

template <class TYPE>
unsigned int VariableArray<TYPE>::GetLength() const
{
	return variables.size();
}

template <class TYPE>
VariableBase& VariableArray<TYPE>::operator = (const VariableBase& variable)
{
	unsigned int index;
	unsigned int length = variable.GetLength();
	for(index = 0; index < length && index < variables.size(); index++)
	{
		*variables[index] = variable[index];
	}
	return *this;
}

template <class TYPE>
void VariableArray<TYPE>::SetFormat(Format fmt)
{
	typename vector<VariableBase *>::iterator variable_iter;
	
	for(variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		(*variable_iter)->SetFormat(fmt);
	}
}

template <class TYPE>
const char *VariableArray<TYPE>::GetDataTypeName() const
{
	return "array";
}

template <class TYPE>
void VariableArray<TYPE>::SetMutable(bool _is_mutable)
{
	VariableBase::SetMutable(_is_mutable);
	typename vector<VariableBase *>::iterator variable_iter;
	
	for(variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		(*variable_iter)->SetMutable(_is_mutable);
	}
}

template <class TYPE>
void VariableArray<TYPE>::SetVisible(bool _is_visible)
{
	VariableBase::SetVisible(_is_visible);
	typename vector<VariableBase *>::iterator variable_iter;
	
	for(variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		(*variable_iter)->SetVisible(_is_visible);
	}
}

template <class TYPE>
void VariableArray<TYPE>::SetSerializable(bool _is_serializable)
{
	VariableBase::SetSerializable(_is_serializable);
	typename vector<VariableBase *>::iterator variable_iter;
	
	for(variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		(*variable_iter)->SetSerializable(_is_serializable);
	}
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
class RegisterArray : public VariableArray<TYPE>
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
	Object(const char *name, Object *parent = 0, const char *description = 0);
	virtual ~Object();

	virtual void OnDisconnect();
	virtual bool BeginSetup(); // must not call any import. By contract it is called first.
	virtual bool Setup(ServiceExportBase *service_export); // must setup an export, can call any import it depends, see ServiceExportBase::SetupDependsOn.
	                                                       // By contract, BeginSetup has been called before.
	virtual bool EndSetup(); // can call any import
	                         // By contract, it is called after Setup(ServiceExportBase&)

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
	void GetVariables(list<VariableBase *>& lst) const;
	Object *GetParent() const;
	void Disconnect();
	VariableBase& operator [] (const char *name);
	Simulator *GetSimulator() const;
	void GenerateLatexDocumentation(ostream& os, VariableBase::Type variable_type) const;
	const char *GetDescription() const;
	virtual void Stop(int exit_status);
	void SetDescription(const char *description);
private:
	string object_name;
	string description;
	Object *parent;
	list<VariableBase *> variables;
	list<ServiceImportBase *> srv_imports;
	list<ServiceExportBase *> srv_exports;
	list<Object *> leaf_objects;
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
	virtual Object *GetService() const = 0;
	void SetupDependsOn(ServiceImportBase& srv_import);
	list<ServiceImportBase *>& GetSetupDependencies();

	friend void operator >> (ServiceExportBase& lhs, ServiceImportBase& rhs);
	friend void operator << (ServiceImportBase& lhs, ServiceExportBase& rhs);
private:
	string name;
	Object *owner;
	list<ServiceImportBase *> setup_dependencies;
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
	virtual ~ServiceImport();

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
#ifdef DEBUG_SERVICE
	cerr << GetName() << ".ServiceImport(" << _name << ", client " << _client->GetName() << ")" << endl;
#endif
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
#ifdef DEBUG_SERVICE
	cerr << GetName() << ".ServiceImport(" << _name << ", object " << (_owner ? _owner->GetName() : "?") << ")" << endl;
#endif
}

template <class SERVICE_IF>
ServiceImport<SERVICE_IF>::~ServiceImport()
{
	ServiceImport<SERVICE_IF>::DisconnectService();
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

#ifdef DEBUG_SERVICE
	cerr << GetName() << " -> " << srv_export.GetName() << endl;
#endif
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

#ifdef DEBUG_SERVICE
	cerr << GetName() << " -> " << alias_import.GetName() << endl;
#endif
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

#ifdef DEBUG_SERVICE
	cerr << GetName() << ".ResolveService(" << _client->GetName() << ") failed" << endl;
#endif
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
	service = 0;
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
	virtual ~ServiceExport();

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
	virtual Object *GetService() const;

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
ServiceExport<SERVICE_IF>::~ServiceExport()
{
	ServiceExport<SERVICE_IF>::DisconnectClient();
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
		client = 0;
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

template <class SERVICE_IF>
Object *ServiceExport<SERVICE_IF>::GetService() const
{
	return service;
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
