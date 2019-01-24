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
#include <set>
#include <queue>
#include <map>
#include <vector>
#include <sstream>
#include <memory>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#include <windef.h>
#endif

#include <unisim/util/inlining/inlining.hh>

namespace unisim {
namespace kernel {
namespace service {
	class Object;
	class Simulator;
}

namespace logger {
	struct Logger;
}

namespace http_server {
	class HttpRequest;
	class HttpServer;
}

} /* end of naemspace kernel */

namespace util {
namespace hypapp {
	struct ClientConnection;
}
}
} /* end of naemspace unisim */

#ifdef DEBUG_MEMORY_ALLOCATION
void *operator new(std::size_t size);
void *operator new[](std::size_t size);
void operator delete(void *p, std::size_t size);
void operator delete[](void *p, std::size_t size);
#endif

namespace unisim {
namespace kernel {
namespace service {

class VariableBase;
class Simulator;
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
//=                          VariableBaseListener                             =
//=============================================================================

class VariableBaseListener
{
public:
	virtual void VariableBaseNotify(const VariableBase *var) = 0;
	virtual ~VariableBaseListener() {};
};

//=============================================================================
//=                             VariableBase                                  =
//=============================================================================

class VariableBase
{
public:
	typedef enum { VAR_VOID, VAR_ARRAY, VAR_PARAMETER, VAR_STATISTIC, VAR_REGISTER, VAR_FORMULA, VAR_SIGNAL } Type;
	typedef enum { FMT_DEFAULT, FMT_HEX, FMT_DEC } Format;

	VariableBase();
	VariableBase(const char *name, Object *owner, Type type, const char *description = 0);
	VariableBase(const char *name, VariableBase *container, Type type, const char *description = 0);
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
	void GetEnumeratedValues(std::vector<std::string> &values) const;
	bool AddEnumeratedValue(const char *value);
	bool RemoveEnumeratedValue(const char *value);
	void SetDescription(const char *description);
	void SetFormat(Format fmt);
	bool IsVoid() const;

	virtual operator bool () const;
	operator signed char () const;
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
	virtual operator std::string () const;
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
	virtual unsigned int GetBitSize() const;
	
	virtual VariableBase& operator = (const VariableBase& variable);
	virtual std::string GetSymbolicValue() const;
	void GenerateLatexDocumentation(std::ostream& os) const;
	
	bool IsMutable() const;
	bool IsVisible() const;
	bool IsSerializable() const;
	bool IsModified() const;
	virtual void SetMutable(bool is_mutable);
	virtual void SetVisible(bool is_visible);
	virtual void SetSerializable(bool is_serializable);
	virtual void SetModified(bool is_modified);

	void AddListener(VariableBaseListener *listener);
	void RemoveListener(VariableBaseListener *listener);
	void NotifyListeners();

private:
	std::string name;
	std::string var_name;
	Object *owner;
	VariableBase *container;
	std::string description;
	std::vector<std::string> enumerated_values;
	Type type;
	Format fmt;
	bool is_mutable;
	bool is_visible;
	bool is_serializable;
	bool is_modified;
	std::set<VariableBaseListener*> listener_set;
};

//=============================================================================
//=                           ConfigFileHelper                                =
//=============================================================================

class ConfigFileHelper
{
public:
	virtual ~ConfigFileHelper() {}
	virtual const char *GetName() const = 0;
	virtual bool SaveVariables(const char *filename, VariableBase::Type type = VariableBase::VAR_VOID) = 0;
	virtual bool LoadVariables(const char *filename, VariableBase::Type type = VariableBase::VAR_VOID) = 0;
};

//=============================================================================
//=                               Sorting                                     =
//=============================================================================

// 	struct lex_ltstr
// 	{
// 		bool operator () (const std::string& s1, const std::string& s2) const
// 		{
// 			return s1 < s2;
// 		}
// 	};
	
struct nat_ltstr
{
	static bool is_digit(char c)
	{
		return (c >= '0') && (c <= '9');
	}
#if 1

	int	pretty_compare( char const* lhs, char const* rhs) const
	{
		for (;; lhs++, rhs++)
		{
			if (not is_digit(*lhs) and not is_digit(*rhs) && (*lhs == *rhs))
			{
				if (*rhs) continue;
				else break;
			}
			/* *rhs != *lhs */
			if (not is_digit(*lhs) or not is_digit(*rhs))
				return *lhs - *rhs;
			/*compare numerical value*/
			char const* lhb = lhs;
			char const* rhb = rhs;
			while (is_digit(lhs[1])) ++lhs;
			while (is_digit(rhs[1])) ++rhs;
			bool borrow = false, zero = true;
			for (char const *lhp = lhs, *rhp = rhs; (lhp >= lhb) or (rhp >= rhb);)
			{
				unsigned lhd = (lhp >= lhb) ? (*lhp--) - '0' : 0;
				unsigned rhd = (rhp >= rhb) ? (*rhp--) - '0' : 0;
				rhd += unsigned(borrow);
				zero &= (lhd == rhd);
				borrow = lhd < rhd;
			}
			if (not zero)
				return borrow ? -1 : +1;
			if (intptr_t delta = (lhs-lhb)-(rhs-rhb))
				return delta < 0 ? -1 : +1;
		}
		return 0;
	}
	
	bool Less(const std::string& s1, const std::string& s2) const
	{
		return pretty_compare(s1.c_str(), s2.c_str()) < 0;
	}
#else
	enum Token
	{
		STRING,
		NUMBER
	};
	
	static Token Scan(const std::string& s, std::size_t begin, std::size_t& tok_len, unsigned int& val)
	{
		int state = 0;
		unsigned int acc = 0;
		std::size_t len = s.length();
		std::size_t pos;
		
		for(pos = begin; pos < len; pos++)
		{
			char c = s[pos];

			switch(state)
			{
				case 0:
					if(is_digit(c))
					{
						acc = c - '0';
						state = 1;
					}
					else
					{
						state = 2;
					}
					break;
				case 1:
					if(is_digit(c))
					{
						acc *= 10;
						acc += c - '0';
					}
					else
					{
						tok_len = pos - begin;
						val = acc;
						return NUMBER;
					}
					break;
				case 2:
					if(is_digit(c))
					{
						tok_len = pos - begin;
						return STRING;
					}
					break;
			}
		}
		
		tok_len = pos - begin;
		if(state == 1)
		{
			val = acc;
			return NUMBER;
		}
		return STRING;
	}
	
	bool Less(const std::string& s1, const std::string& s2) const
	{
		std::size_t s1_len = s1.length();
		std::size_t s2_len = s2.length();
		std::size_t s1_pos = 0;
		std::size_t s2_pos = 0;
		while((s1_pos < s1_len) && (s2_pos < s2_len))
		{
			std::size_t tok1_len = 0;
			unsigned int val1 = 0;
			Token tok1 = Scan(s1, s1_pos, tok1_len, val1);
			std::size_t tok2_len = 0;
			unsigned int val2 = 0;
			Token tok2 = Scan(s2, s2_pos, tok2_len, val2);
			if((tok1 == NUMBER) && (tok2 == NUMBER))
			{
				if(val1 < val2) return true;
				else if(val1 > val2) return false;
			}
			else
			{
				if(s1.compare(s1_pos, tok1_len, s2, s2_pos, tok2_len) < 0) return true;
				else if(s1.compare(s1_pos, tok1_len, s2, s2_pos, tok2_len) > 0) return false;
			}
			s1_pos += tok1_len;
			s2_pos += tok2_len;
		}
		
		return s1_len < s2_len;
	}
#endif

	bool operator () (const std::string& s1, const std::string& s2) const
	{
		bool less = Less(s1, s2);
		//std::cerr << "\"" << s1 << "\"" << (less ? "<" : ">=") << "\"" << s2 << "\"" << std::endl;
		return less;
	}
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
	
	static inline Simulator *Instance() { return simulator; }
	
	Simulator(int argc, char **argv,
			void (*LoadBuiltInConfig)(Simulator *simulator) = 0);
	virtual ~Simulator();
	virtual SetupStatus Setup();
	virtual void Stop(Object *object, int exit_status, bool asynchronous = false);

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
	Object *FindObject(const char *name) const;

	void GetVariables(std::list<VariableBase *>& lst, VariableBase::Type type = VariableBase::VAR_VOID);
	void GetRootVariables(std::list<VariableBase *>& lst, VariableBase::Type type = VariableBase::VAR_VOID);
	void GetArrays(std::list<VariableBase *>& lst);
	void GetParameters(std::list<VariableBase *>& lst);
	void GetRegisters(std::list<VariableBase *>& lst);
	void GetSignals(std::list<VariableBase *>& lst);
	void GetStatistics(std::list<VariableBase *>& lst);
	void GetFormulas(std::list<VariableBase *>& lst);

	void Dump(std::ostream& os);
	void DumpVariables(std::ostream& os, VariableBase::Type filter_type = VariableBase::VAR_VOID);
	void DumpStatistics(std::ostream& os);
	void DumpParameters(std::ostream& os);
	void DumpRegisters(std::ostream& os);
	void DumpFormulas(std::ostream& os);

	bool GetExecutablePath(const char *argv0, std::string& out_execute_path) const;
	bool GetBinPath(const char *argv0, std::string& out_bin_dir, std::string& out_bin_program) const;
	bool GetSharePath(const std::string& bin_dir, std::string& out_share_dir) const;
	const std::string GetSharedDataDirectory() const;
	std::string SearchSharedDataFile(const char *filename) const;
	std::vector<std::string> const& GetCmdArgs() const;

	void GenerateLatexDocumentation(std::ostream& os) const;
	
	virtual double GetSimTime()	{ return (0);	}
	virtual double GetHostTime()	{ return (0);	}

	// ********** The following methods are added by Reda and used by PIMServer *****
	virtual unsigned long long   GetStructuredAddress(unsigned long long logicalAddress) { return (logicalAddress); }
	virtual unsigned long long   GetPhysicalAddress(unsigned long long logicalAddress) { return (logicalAddress); }
	// *******************************************************************************

	bool IsWarningEnabled() const;

	void Register(ConfigFileHelper *config_file_helper);
private:
	friend class Object;
	friend class VariableBase;
	template <class TYPE> friend class Variable;
	template <class TYPE> friend class VariableArray;
	friend class ServiceImportBase;
	friend class ServiceExportBase;

	static Simulator *simulator;
	VariableBase *void_variable;
	std::string shared_data_dir;
	std::map<std::string, std::string> set_vars;
	std::string get_config_filename;
	std::string input_config_file_format;
	std::string output_config_file_format;
	bool list_parms;
	bool get_config;
	bool generate_doc;
	std::string generate_doc_filename;
	bool enable_warning;
	bool enable_version;
	bool enable_help;
	bool warn_get_bin_path;
	bool warn_get_share_path;
	bool enable_press_enter_at_exit;
	std::string bin_dir;
	std::string program_binary;
	std::string program_name;
	std::string authors;
	std::string copyright;
	std::string description;
	std::string version;
	std::string license;
	std::string schematic;
	Parameter<std::string> *var_program_name;
	Parameter<std::string> *var_authors;
	Parameter<std::string> *var_copyright;
	Parameter<std::string> *var_description;
	Parameter<std::string> *var_version;
	Parameter<std::string> *var_license;
	Parameter<std::string> *var_schematic;
	Parameter<bool> *param_enable_press_enter_at_exit;
	
	void Version(std::ostream& os) const;
	void Help(std::ostream& os) const;
	
	void Register(Object *object);
	void Register(ServiceImportBase *srv_import);
	void Register(ServiceExportBase *srv_export);
	void Register(VariableBase *variable);

	void Unregister(Object *object);
	void Unregister(ServiceImportBase *srv_import);
	void Unregister(ServiceExportBase *srv_export);
	void Unregister(VariableBase *variable);

	void Initialize(VariableBase *variable);

public:
	bool LoadVariables(const char *filename, VariableBase::Type type = VariableBase::VAR_VOID);
	bool SaveVariables(const char *filename, VariableBase::Type type = VariableBase::VAR_VOID);

	void GetObjects(std::list<Object *>& lst) const;
	void GetRootObjects(std::list<Object *>& lst) const;

private:
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

	std::map<std::string, Object *, nat_ltstr /* lex_ltstr*/> objects;
	std::map<std::string, ServiceImportBase *> imports;
	std::map<std::string, ServiceExportBase *> exports;
	std::map<std::string, VariableBase *, nat_ltstr /* lex_ltstr*/> variables;
	std::map<std::string, ConfigFileHelper *> config_file_helpers;
	
	std::vector<std::string> cmd_args;
	ParameterArray<std::string> *param_cmd_args;
	
	unisim::kernel::http_server::HttpServer *http_server;
	
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
private:
#if !defined(WIN32) && !defined(_WIN32) && !defined(WIN64) && !defined(_WIN64)
	static void (*sig_int_handler)(int);
	static void (*prev_sig_int_handler)(int);
#endif
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	static BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType);
#else
	static void SigIntHandler(int signum);
#endif
#if !defined(WIN32) && !defined(_WIN32) && !defined(WIN64) && !defined(_WIN64)
	static void (*sig_pipe_handler)(int);
	static void (*prev_sig_pipe_handler)(int);
	static void SigPipeHandler(int signum);
#endif
	void BroadcastSigInt();
};

//=============================================================================
//=                            Variable<TYPE>                                 =
//=============================================================================

template <class TYPE>
class Variable : public VariableBase
{
public:
	typedef VariableBase::Type Type;
	Variable(const char *name, Object *owner, TYPE& storage, VariableBase::Type type, const char *description = NULL);

	virtual const char *GetDataTypeName() const;
	virtual unsigned int GetBitSize() const;
	virtual operator bool () const;
	virtual operator long long () const;
	virtual operator unsigned long long () const;
	virtual operator double () const;
	virtual operator std::string () const;
	virtual VariableBase& operator = (bool value);
	virtual VariableBase& operator = (long long value);
	virtual VariableBase& operator = (unsigned long long value);
	virtual VariableBase& operator = (double value);
	virtual VariableBase& operator = (const char * value);
	
	virtual void Set( TYPE const& value );
	virtual TYPE Get() const { return *storage; }

private:
	TYPE *storage;
};

template <class TYPE>
void
Variable<TYPE>::Set( TYPE const& value )
{
	SetModified(*storage != value);
	*storage = value;
	NotifyListeners();
}

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

//=============================================================================
//=                  CallBackObject and  TCallBack<TYPE>                      =
//=============================================================================

struct CallBackObject
{
	virtual ~CallBackObject() {}

	virtual bool read(unsigned int offset, const void *buffer, unsigned int data_length) { return false; }

	virtual bool write(unsigned int offset, const void *buffer, unsigned int data_length) {	return false; }

};

template <typename TYPE>
class TCallBack
{
public:
	typedef bool (CallBackObject::*cbwrite)(unsigned int offset, const void*, unsigned int size);
	typedef bool (CallBackObject::*cbread)(unsigned int offset, const void*, unsigned int size);
	
	TCallBack(CallBackObject *owner, unsigned int offset, cbwrite _write, cbread _read)
          : m_owner(owner), m_offset(offset), write(_write), read(_read)
	{}

	bool Write(TYPE const& storage) { return write and (m_owner->*write)(m_offset, &storage, sizeof (TYPE)); }

	bool Read(TYPE& storage) { return read and (m_owner->*read)(m_offset, &storage, sizeof (TYPE)); }
private:
	CallBackObject *m_owner;
	unsigned int m_offset;

	cbwrite write;
	cbread read;
};

template <class TYPE>
class Register : public Variable<TYPE>
{
public:
	Register(const char *name, Object *owner, TYPE& storage, const char *description = NULL)
	  : Variable<TYPE>(name, owner, storage, VariableBase::VAR_REGISTER, description)
	  , m_callback( 0 )
	{}
	
	~Register()
	{
	  delete m_callback;
	}
	
	typedef TCallBack<TYPE> TCB;
	void setCallBack(CallBackObject *owner, unsigned int offset, typename TCB::cbwrite _write, typename TCB::cbread _read)
	{
		if (m_callback) delete m_callback;
		m_callback = new TCB(owner, offset, _write, _read);
	}

	virtual void Set( TYPE const& value ) { if (not WriteBack(value)) Variable<TYPE>::Set( value ); }
	virtual TYPE Get() const { TYPE value; if (not ReadBack(value)) return Variable<TYPE>::Get(); return value; }

//	using Variable<TYPE>::operator=;

	VariableBase& operator = (bool value) { return (Variable<TYPE>::operator = (value)); }
	VariableBase& operator = (long long value) { return (Variable<TYPE>::operator = (value)); }
	VariableBase& operator = (unsigned long long value) { return (Variable<TYPE>::operator = (value)); }
	VariableBase& operator = (double value) { return (Variable<TYPE>::operator = (value)); }
	VariableBase& operator = (const char * value) { return (Variable<TYPE>::operator = (value)); }

protected:
	bool WriteBack(TYPE const& storage)
	{
		bool status = (m_callback and m_callback->Write(storage));
		if (status) this->NotifyListeners();
		return status;
	}

	bool ReadBack(TYPE& storage) const
	{
		return (m_callback and m_callback->Read(storage));
	}

private:
	TCB* m_callback;
};

template <class TYPE>
class Signal : public Variable<TYPE>
{
public:

	Signal(const char *name, Object *owner, TYPE& storage, const char *description = NULL) : Variable<TYPE>(name, owner, storage, VariableBase::VAR_SIGNAL, description) {}

//	using Variable<TYPE>::operator=;

	VariableBase& operator = (bool value) { return (Variable<TYPE>::operator = (value)); }
	VariableBase& operator = (long long value) { return (Variable<TYPE>::operator = (value)); }
	VariableBase& operator = (unsigned long long value) { return (Variable<TYPE>::operator = (value)); }
	VariableBase& operator = (double value) { return (Variable<TYPE>::operator = (value)); }
	VariableBase& operator = (const char * value) { return (Variable<TYPE>::operator = (value)); }

};

class FormulaOperator
{
public:
	typedef enum
	{
		OP_UNKNOWN,
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

	FormulaOperator(const char *name);
	FormulaOperator(Operator op);
	operator Operator() const;
private:
	Operator op;
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
	typedef FormulaOperator Operator;
	
	Formula(const char *name, Object *owner, FormulaOperator op, VariableBase *child1, VariableBase *child2, VariableBase *child3, const char *description = 0);
	Formula(const char *name, Object *owner, FormulaOperator op, VariableBase *child1, VariableBase *child2, const char *description = 0);
	Formula(const char *name, Object *owner, FormulaOperator op, VariableBase *child, const char *description = 0);
	
	virtual const char *GetDataTypeName() const;
	virtual operator bool () const;
	virtual operator long long () const;
	virtual operator unsigned long long () const;
	virtual operator double () const;
	virtual operator std::string () const;
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
	virtual void SetModified(bool is_modified);

private:
	std::vector<VariableBase *> variables;
};

template <class TYPE>
VariableArray<TYPE>::VariableArray(const char *_name, Object *_owner, TYPE *_variables, unsigned int dim, VariableBase::Type type, const char *_description) :
	VariableBase(_name, _owner, VariableBase::VAR_ARRAY, _description),
	variables()
{
	unsigned int i;
	for(i = 0; i < dim; i++)
	{
		std::stringstream sstr;
		
		sstr << _name << "[" << i << "]";
		variables.push_back(new Variable<TYPE>(sstr.str().c_str(), _owner, *(_variables + i), type, _description));
	}
}


template <class TYPE>
VariableArray<TYPE>::~VariableArray()
{
	typename std::vector<VariableBase *>::iterator variable_iter;
	
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
		std::cerr << "Subscript out of range" << std::endl;
		return (*Simulator::Instance()->void_variable);
	}
	return (*variables[index]);
}

template <class TYPE>
const VariableBase& VariableArray<TYPE>::operator [] (unsigned int index) const
{
	if(index >= variables.size())
	{
		std::cerr << "Subscript out of range" << std::endl;
		return (*Simulator::Instance()->void_variable);
	}
	return (*variables[index]);
}

template <class TYPE>
unsigned int VariableArray<TYPE>::GetLength() const
{
	return (variables.size());
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
	return (*this);
}

template <class TYPE>
void VariableArray<TYPE>::SetFormat(Format fmt)
{
	typename std::vector<VariableBase *>::iterator variable_iter;
	
	for(variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		(*variable_iter)->SetFormat(fmt);
	}
}

template <class TYPE>
const char *VariableArray<TYPE>::GetDataTypeName() const
{
	return ("array");
}

template <class TYPE>
void VariableArray<TYPE>::SetMutable(bool _is_mutable)
{
	VariableBase::SetMutable(_is_mutable);
	typename std::vector<VariableBase *>::iterator variable_iter;
	
	for(variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		(*variable_iter)->SetMutable(_is_mutable);
	}
}

template <class TYPE>
void VariableArray<TYPE>::SetVisible(bool _is_visible)
{
	VariableBase::SetVisible(_is_visible);
	typename std::vector<VariableBase *>::iterator variable_iter;
	
	for(variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		(*variable_iter)->SetVisible(_is_visible);
	}
}

template <class TYPE>
void VariableArray<TYPE>::SetSerializable(bool _is_serializable)
{
	VariableBase::SetSerializable(_is_serializable);
	typename std::vector<VariableBase *>::iterator variable_iter;
	
	for(variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		(*variable_iter)->SetSerializable(_is_serializable);
	}
}

template <class TYPE>
void VariableArray<TYPE>::SetModified(bool _is_modified)
{
	// Arrays can't be modified, only there elements
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

template <class TYPE>
class SignalArray : public VariableArray<TYPE>
{
public:
	SignalArray(const char *name, Object *owner, TYPE *parameters, unsigned int dim, const char *description = NULL) : VariableArray<TYPE>(name, owner, parameters, dim, VariableBase::VAR_SIGNAL, description) {}
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
	
	virtual void SigInt();

	const char *GetName() const;
	const char *GetObjectName() const;

	void Add(ServiceImportBase& srv_import);
	void Remove(ServiceImportBase& srv_import);
	void Add(ServiceExportBase& srv_export);
	void Remove(ServiceExportBase& srv_export);
	void Add(Object& object);
	void Remove(Object& object);
	void Add(VariableBase& var);
	void Remove(VariableBase& var);
	const std::list<ServiceImportBase *>& GetServiceImports() const;
	const std::list<ServiceExportBase *>& GetServiceExports() const;
	const std::list<Object *>& GetLeafs() const;
	void GetVariables(std::list<VariableBase *>& lst, VariableBase::Type type = VariableBase::VAR_VOID) const;
	Object *GetParent() const;
	void Disconnect();
	VariableBase& operator [] (const char *name);
	VariableBase& operator [] (const std::string& name);
	Simulator *GetSimulator() const;
	void GenerateLatexDocumentation(std::ostream& os, VariableBase::Type variable_type) const;
	const char *GetDescription() const;
	virtual void Stop(int exit_status, bool asynchronous = false);
	void SetDescription(const char *description);
	
	virtual bool ServeHttpRequest(unisim::kernel::http_server::HttpRequest const& req, unisim::util::hypapp::ClientConnection const& conn);
private:
	std::string object_name;
	std::string object_base_name;
	std::string description;
	Object *parent;
	std::list<VariableBase *> variables;
	std::list<ServiceImportBase *> srv_imports;
	std::list<ServiceExportBase *> srv_exports;
	std::list<Object *> leaf_objects;
};

//=============================================================================
//=                              ServiceInterface                             =
//=============================================================================

class ServiceInterface
{
public:
	virtual ~ServiceInterface() {}
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
	virtual void Dump(std::ostream& os) const = 0;
	virtual Object *GetService() const = 0;
	virtual ServiceExportBase *GetServiceExport() = 0;
protected:
	std::string name;
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
	virtual void Dump(std::ostream& os) const = 0;
	virtual Object *GetClient() const = 0;
	virtual Object *GetService() const = 0;
	void SetupDependsOn(ServiceImportBase& srv_import);
	std::list<ServiceImportBase *>& GetSetupDependencies();

	friend void operator >> (ServiceExportBase& lhs, ServiceImportBase& rhs);
	friend void operator << (ServiceImportBase& lhs, ServiceExportBase& rhs);
protected:
	std::string name;
	Object *owner;
	std::list<ServiceImportBase *> setup_dependencies;
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

 	inline operator SERVICE_IF * () const ALWAYS_INLINE;

	inline SERVICE_IF *operator -> () const ALWAYS_INLINE;

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

	virtual void Dump(std::ostream& os) const;

	virtual Object *GetService() const;

	virtual ServiceExportBase *GetServiceExport();

private:
	Service<SERVICE_IF> *service;
	ServiceExport<SERVICE_IF> *srv_export;
	ServiceImport<SERVICE_IF> *alias_import;
	std::list<ServiceImport<SERVICE_IF> *> actual_imports;
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
	std::cerr << GetName() << ".ServiceImport(" << _name << ", client " << _client->GetName() << ")" << std::endl;
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
	std::cerr << GetName() << ".ServiceImport(" << _name << ", object " << (_owner ? _owner->GetName() : "?") << ")" << std::endl;
#endif
}

template <class SERVICE_IF>
ServiceImport<SERVICE_IF>::~ServiceImport()
{
#ifdef DEBUG_SERVICE
	std::cerr << GetName() << ".~ServiceImport()" << std::endl;
#endif
	//ServiceImport<SERVICE_IF>::DisconnectService();
	ServiceImport<SERVICE_IF>::Disconnect();
}

template <class SERVICE_IF>
inline ServiceImport<SERVICE_IF>::operator SERVICE_IF * () const
{
	return (service);
}

template <class SERVICE_IF>
inline SERVICE_IF *ServiceImport<SERVICE_IF>::operator -> () const
{
#ifdef DEBUG_SERVICE
	if(!service)
	{
		std::cerr << "ERROR! " << GetName() << " interface can't be used because it is not bound to a service." << std::endl;
		owner->Object::Stop(-1);
	}
#endif
	return (service);
}

template <class SERVICE_IF>
void ServiceImport<SERVICE_IF>::Bind(ServiceExport<SERVICE_IF>& srv_export)
{
	if(this->alias_import)
	{
		std::cerr << "WARNING! Can't connect " << GetName() << " to " << srv_export.GetName() << " because it is already connected to " << this->alias_import->GetName() << std::endl;
		return;
	}

	if(this->srv_export)
	{
		std::cerr << "WARNING! Can't connect " << GetName() << " to " << srv_export.GetName() << " because it is already connected to " << this->srv_export->GetName() << std::endl;
		return;
	}

#ifdef DEBUG_SERVICE
	std::cerr << GetName() << " -> " << srv_export.GetName() << std::endl;
#endif
	this->srv_export = &srv_export;
}

template <class SERVICE_IF>
void ServiceImport<SERVICE_IF>::Bind(ServiceImport<SERVICE_IF>& alias_import)
{
	if(srv_export)
	{
		std::cerr << "WARNING! Can't connect " << GetName() << " to " << alias_import.GetName() << " because it is already connected to " << srv_export->GetName() << std::endl;
		return;
	}

	if(this->alias_import)
	{
		std::cerr << "WARNING! Can't connect " << GetName() << " to " << alias_import.GetName() << " because it is already connected to " << this->alias_import->GetName() << std::endl;
		return;
	}

#ifdef DEBUG_SERVICE
	std::cerr << GetName() << " -> " << alias_import.GetName() << std::endl;
#endif
	this->alias_import = &alias_import;
	alias_import.actual_imports.push_back(this);
}

template <class SERVICE_IF>
void ServiceImport<SERVICE_IF>::Unbind(ServiceExport<SERVICE_IF>& srv_export)
{
#ifdef DEBUG_SERVICE
	std::cerr << GetName() << ".Unbind(" << srv_export.GetName() << ")" << std::endl;
#endif

	if(this->srv_export != &srv_export)
	{
		std::cerr << "ERROR! Can't disconnect " << GetName() << " from " << srv_export.GetName() << " because connection does not exist" << std::endl;
		return;
	}

	UnresolveService();

	this->srv_export = 0;
}

template <class SERVICE_IF>
Service<SERVICE_IF> *ServiceImport<SERVICE_IF>::ResolveService(Client<SERVICE_IF> *_client)
{
	if(alias_import)
		return (alias_import->ResolveService(_client));
	else
		if(srv_export) return (srv_export->ResolveService(_client));

#ifdef DEBUG_SERVICE
	std::cerr << GetName() << ".ResolveService(" << _client->GetName() << ") failed" << std::endl;
#endif
	return (0);
}

template <class SERVICE_IF>
ServiceExport<SERVICE_IF> *ServiceImport<SERVICE_IF>::ResolveServiceExport()
{
	if(alias_import)
		return (alias_import->ResolveServiceExport());
	else
		if(srv_export) return (srv_export->ResolveServiceExport());

	return (0);
}

template <class SERVICE_IF>
ServiceExportBase *ServiceImport<SERVICE_IF>::GetServiceExport()
{
	return (ResolveServiceExport());
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
		typename std::list<ServiceImport<SERVICE_IF> *>::iterator import_iter;
	
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
	std::cerr << GetName() << ".UnresolveService()" << std::endl;
#endif

	if(actual_imports.empty())
	{
		if(service)
		{
			//service->OnDisconnect(); // Gilles: That's dangerous
#ifdef DEBUG_SERVICE
			std::cerr << GetName() << ": Unresolving service " << service->GetName() << std::endl;
#endif
		}
	}
	else
	{
		typename std::list<ServiceImport<SERVICE_IF> *>::iterator import_iter;
	
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
	std::cerr << GetName() << ".Disconnect()" << std::endl;
#endif

	DisconnectService();

	if(alias_import)
	{
		typename std::list<ServiceImport<SERVICE_IF> *>::iterator import_iter;
	
		for(import_iter = alias_import->actual_imports.begin(); import_iter != alias_import->actual_imports.end(); import_iter++)
		{
			if(*import_iter == this)
			{
				alias_import->actual_imports.erase(import_iter);
				break;
			}
		}
		alias_import = 0;
	}

	if(!actual_imports.empty())
	{
		typename std::list<ServiceImport<SERVICE_IF> *>::iterator import_iter;
	
		for(import_iter = actual_imports.begin(); import_iter != actual_imports.end(); import_iter++)
		{
			if((*import_iter)->alias_import == this)
			{
				(*import_iter)->alias_import = 0;
			}
		}
		actual_imports.clear();
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
	std::cerr << GetName() << ".DisconnectService()" << std::endl;
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
void ServiceImport<SERVICE_IF>::Dump(std::ostream& os) const
{
	if(alias_import)
	{
		os << GetName() << " -> " << alias_import->GetName() << std::endl;
	}

	typename std::list<ServiceImport<SERVICE_IF> *>::const_iterator import_iter;

	for(import_iter = actual_imports.begin(); import_iter != actual_imports.end(); import_iter++)
	{
		os << "# " << (*import_iter)->GetName() << " -> " << GetName() << std::endl;
	}

	if(srv_export)
	{
		os << GetName() << " -> " << srv_export->GetName() << std::endl;
	}
}

template <class SERVICE_IF>
Object *ServiceImport<SERVICE_IF>::GetService() const
{
	return (service);
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

	inline bool IsConnected() const ALWAYS_INLINE;

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

	virtual void Dump(std::ostream& os) const;

	virtual Object *GetClient() const;
	virtual Object *GetService() const;

private:
	std::list<ServiceExport<SERVICE_IF> *> alias_exports;
	ServiceExport<SERVICE_IF> *actual_export;
	Service<SERVICE_IF> *service;
	std::list<ServiceImport<SERVICE_IF> *> srv_imports;
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
#ifdef DEBUG_SERVICE
	std::cerr << GetName() << ".~ServiceExport()" << std::endl;
#endif
	//ServiceExport<SERVICE_IF>::DisconnectClient();
	ServiceExport<SERVICE_IF>::Disconnect();
}

template <class SERVICE_IF>
inline bool ServiceExport<SERVICE_IF>::IsConnected() const
{
	return (client != 0);
}

template <class SERVICE_IF>
void ServiceExport<SERVICE_IF>::Bind(ServiceImport<SERVICE_IF>& srv_import)
{
	typename std::list<ServiceImport<SERVICE_IF> *>::iterator import_iter;

	for(import_iter = srv_imports.begin(); import_iter != srv_imports.end(); import_iter++)
	{
		if(*import_iter == &srv_import)
		{
			std::cerr << "WARNING! Can't connect again " << GetName() << " to " << srv_import.GetName() << " because it is already connected" << std::endl;
			return;
		}
	}

	srv_imports.push_back(&srv_import);
}

template <class SERVICE_IF>
void ServiceExport<SERVICE_IF>::Bind(ServiceExport<SERVICE_IF>& alias_export)
{
	typename std::list<ServiceExport<SERVICE_IF> *>::iterator export_iter;

	for(export_iter = alias_exports.begin(); export_iter != alias_exports.end(); export_iter++)
	{
		if(*export_iter == &alias_export)
		{
			std::cerr << "WARNING! Can't connect again " << GetName() << " to " << alias_export.GetName() << " because it is already" << std::endl;
			return;
		}
	}

	if(alias_export.actual_export)
	{
		std::cerr << "WARNING! Can't connect " << GetName() << " to " << alias_export.GetName() << " because " << alias_export.GetName() << " is already connected to " << alias_export.actual_export->GetName() << std::endl;
		return;
	}

	alias_exports.push_back(&alias_export);
	alias_export.actual_export = this;
}

template <class SERVICE_IF>
void ServiceExport<SERVICE_IF>::Unbind(ServiceImport<SERVICE_IF>& srv_import)
{
#ifdef DEBUG_SERVICE
	std::cerr << GetName() << ".Unbind(" << srv_import.GetName() << ")" << std::endl;
#endif

	typename std::list<ServiceImport<SERVICE_IF> *>::iterator import_iter;

	for(import_iter = srv_imports.begin(); import_iter != srv_imports.end(); import_iter++)
	{
		if(*import_iter == &srv_import)
		{
			srv_imports.erase(import_iter);
			UnresolveClient();
			return;
		}
	}

	std::cerr << "WARNING! Unable to disconnect " << GetName() << " and " << srv_import.GetName() << " because connection does not exist" << std::endl;
}

template <class SERVICE_IF>
void ServiceExport<SERVICE_IF>::DisconnectClient()
{
#ifdef DEBUG_SERVICE
	std::cerr << GetName() << ".DisconnectClient()" << std::endl;
#endif

	typename std::list<ServiceExport<SERVICE_IF> *>::iterator export_iter;

	for(export_iter = alias_exports.begin(); export_iter != alias_exports.end(); export_iter++)
	{
		(*export_iter)->DisconnectClient();
	}

	typename std::list<ServiceImport<SERVICE_IF> *>::iterator import_iter;

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
	std::cerr << GetName() << ".Disconnect()" << std::endl;
#endif

	DisconnectClient();
	
	if(actual_export)
	{
		typename std::list<ServiceExport<SERVICE_IF> *>::iterator export_iter;
	
		for(export_iter = actual_export->alias_exports.begin(); export_iter != actual_export->alias_exports.end(); export_iter++)
		{
			if(*export_iter == this)
			{
				actual_export->alias_exports.erase(export_iter);
				break;
			}
		}
		actual_export = 0;
	}

	typename std::list<ServiceExport<SERVICE_IF> *>::iterator export_iter;

	for(export_iter = alias_exports.begin(); export_iter != alias_exports.end(); export_iter++)
	{
		(*export_iter)->actual_export = 0;
	}

	alias_exports.clear();
}

template <class SERVICE_IF>
void ServiceExport<SERVICE_IF>::ResolveClient()
{
	typename std::list<ServiceExport<SERVICE_IF> *>::iterator export_iter;

	for(export_iter = alias_exports.begin(); export_iter != alias_exports.end(); export_iter++)
	{
		(*export_iter)->ResolveClient();
	}

	typename std::list<ServiceImport<SERVICE_IF> *>::iterator import_iter;

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
		return (actual_export->ResolveService(_client));
	}

	if(service)
	{
		client = _client;
		return (service);
	}
	return (0);
}

template <class SERVICE_IF>
ServiceExport<SERVICE_IF> *ServiceExport<SERVICE_IF>::ResolveServiceExport()
{
	if(actual_export)
	{
		return (actual_export->ResolveServiceExport());
	}

	return (this);
}

template <class SERVICE_IF>
void ServiceExport<SERVICE_IF>::UnresolveClient()
{
#ifdef DEBUG_SERVICE
	std::cerr << GetName() << ".UnresolveClient()" << std::endl;
#endif

	if(actual_export)
	{
		client = 0;
		return (actual_export->UnresolveClient());
	}

	if(client)
	{
		//client->OnDisconnect(); // Gilles: that's dangerous
#ifdef DEBUG_SERVICE
		std::cerr << GetName() << ": Unresolving client " << client->GetName() << std::endl;
#endif
		client = 0;
	}
}

template <class SERVICE_IF>
void ServiceExport<SERVICE_IF>::Dump(std::ostream& os) const
{
	if(actual_export)
	{
		std::cerr << GetName() << " -> " << actual_export->GetName() << std::endl;
	}

	typename std::list<ServiceExport<SERVICE_IF> *>::const_iterator export_iter;

	for(export_iter = alias_exports.begin(); export_iter != alias_exports.end(); export_iter++)
	{
		os << "# " << (*export_iter)->GetName() << " -> " << GetName() << std::endl;
	}
}

template <class SERVICE_IF>
Object *ServiceExport<SERVICE_IF>::GetClient() const
{
	return (client);
}

template <class SERVICE_IF>
Object *ServiceExport<SERVICE_IF>::GetService() const
{
	return (service);
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
	return (rhs);
}

// (export <- import) ==> import
template <class SERVICE_IF>
ServiceImport<SERVICE_IF>& operator << (ServiceExport<SERVICE_IF>& lhs, ServiceImport<SERVICE_IF>& rhs)
{
	rhs.Bind(lhs);
	lhs.Bind(rhs);
	rhs.ResolveClient();
	return (rhs);
}

// (import1 -> import2) ==> import2
template <class SERVICE_IF>
ServiceImport<SERVICE_IF>& operator >> (ServiceImport<SERVICE_IF>& lhs, ServiceImport<SERVICE_IF>& rhs)
{
	lhs.Bind(rhs);
	lhs.ResolveClient();
	return (rhs);
}

// (import1 <- import2) ==> import2
template <class SERVICE_IF>
ServiceImport<SERVICE_IF>& operator << (ServiceImport<SERVICE_IF>& lhs, ServiceImport<SERVICE_IF>& rhs)
{
	rhs.Bind(lhs);
	rhs.ResolveClient();
	return (rhs);
}

// (export1 -> export2) ==> export2
template <class SERVICE_IF>
ServiceExport<SERVICE_IF>& operator >> (ServiceExport<SERVICE_IF>& lhs, ServiceExport<SERVICE_IF>& rhs)
{
	rhs.Bind(lhs);
	lhs.ResolveClient();
	return (rhs);
}

// (export1 <- export2) ==> export2
template <class SERVICE_IF>
ServiceExport<SERVICE_IF>& operator << (ServiceExport<SERVICE_IF>& lhs, ServiceExport<SERVICE_IF>& rhs)
{
	lhs.Bind(rhs);
	rhs.ResolveClient();
	return (rhs);
}

} // end of namespace service
} // end of namespace kernel
} // end of namespace unisim

#endif // __UNISIM_KERNEL_SERVICE_SERVICE_HH__
