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
#include "unisim/kernel/logger/logger.hh"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
//#include <limits.h>
//#include <limits>
#if defined(__APPLE_CC__) || defined(linux) || defined(__linux) || defined(__linux__)
#include <dlfcn.h>
#endif

#if !defined(WIN32) && !defined(_WIN32) && !defined(WIN64) && !defined(_WIN64)
#include <signal.h>
#endif

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#include <windows.h>
#endif

// #if defined(__APPLE_CC__)
// #include <mach-o/dyld.h>
// #endif

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/graph/graphviz.hpp>

#include "unisim/kernel/config/xml_config_file_helper.hh"
#include "unisim/util/backtrace/backtrace.hh"
#include "unisim/util/likely/likely.hh"

#include <unisim/kernel/config/ini_config_file_helper.hh>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#include <fcntl.h>
//Note: this is to force opening console and files in binary mode on Windows as on UNIX
int _CRT_fmode = _O_BINARY;
#endif

#ifdef DEBUG_MEMORY_ALLOCATION
void *operator new(std::size_t size)
{
	std::cerr << "malloc(" << size << ")" << std::endl;
	std::cerr << unisim::util::backtrace::BackTrace() << std::endl;
	return malloc(size);
}

void *operator new[](std::size_t size)
{
	std::cerr << "malloc(" << size << ")" << std::endl;
	std::cerr << unisim::util::backtrace::BackTrace() << std::endl;
	return malloc(size);
}

void operator delete(void *storage, std::size_t size)
{
	std::cerr << "free(" << size << ")" << std::endl;
	std::cerr << unisim::util::backtrace::BackTrace() << std::endl;
	free(storage);
}

void operator delete[](void *storage, std::size_t size)
{
	std::cerr << "free(" << size << ")" << std::endl;
	std::cerr << unisim::util::backtrace::BackTrace() << std::endl;
	free(storage);
}
#endif

namespace unisim {
namespace kernel {
namespace service {

typedef struct
{
	const char *searchFor;
	const char *replaceBy;
} Translation;

static Translation conversion_table[] = {
	{"\n", "\\\\\n"},
	{"é", "\\'e"},
	{"è", "\\`e"},
	{"ê", "\\^e"},
	{"à", "\\`a"},
	{"#", "\\#"},
	{"_", "\\_"},
	{"\t", "~~"},
	{"{", "$\\{$"},
	{"}", "$\\}$"},
	{"&", "\\&"},
	{"--", "{-}{-}"},
	{"->", "$\\rightarrow$"},
	{"<<", "<}\\texttt{<"},
	{">>", ">}\\texttt{>"},
	{"<", "$<$"},
	{">", "$>$"},
	{"%", "\\%"},
	{"$", "\\$"},
	{"//(1)", "\\ding{202}"},
	{"//(2)", "\\ding{203}"},
	{"//(3)", "\\ding{204}"},
	{"//(4)", "\\ding{205}"},
	{"//(5)", "\\ding{206}"},
	{"//(6)", "\\ding{207}"},
	{"//(7)", "\\ding{208}"},
	{"//(8)", "\\ding{209}"},
	{"//(9)", "\\ding{210}"},
	{"/*(1)*/", "\\ding{202}"},
	{"/*(2)*/", "\\ding{203}"},
	{"/*(3)*/", "\\ding{204}"},
	{"/*(4)*/", "\\ding{205}"},
	{"/*(5)*/", "\\ding{206}"},
	{"/*(6)*/", "\\ding{207}"},
	{"/*(7)*/", "\\ding{208}"},
	{"/*(8)*/", "\\ding{209}"},
	{"/*(9)*/", "\\ding{210}"}
};

static std::string string_to_latex(const char *s, unsigned int cut = 0, const char *style = 0)
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

bool ResolvePath(const std::string& prefix_dir,
		const std::string& suffix_dir,
		std::string& out_dir) 
{
	std::string unresolved_dir = prefix_dir;
	unresolved_dir += '/';
	unresolved_dir += suffix_dir;
//	char resolved_dir_buf[PATH_MAX + 1];
	char resolved_dir_buf[FILENAME_MAX + 1];

#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__APPLE_CC__)
	if ( realpath(unresolved_dir.c_str(), 
				resolved_dir_buf) )
	{
		out_dir = resolved_dir_buf;
		return true;
	}
#elif defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	DWORD length = GetFullPathName(unresolved_dir.c_str(), 
			PATH_MAX + 1, 
			resolved_dir_buf, 
			0);
	if(length > 0)
	{
		resolved_dir_buf[length] = 0;
		out_dir = resolved_dir_buf;
		return true;
	}
#endif
	return false;
}

//=============================================================================
//=                             VariableBase                                 =
//=============================================================================
	
VariableBase::
VariableBase(const char *_name, Object *_owner, Type _type, const char *_description)
	: name(_owner ? std::string(_owner->GetName()) + std::string(".") + std::string(_name) : std::string(_name) )
	, var_name(std::string(_name))
	, owner(_owner)
	, container(0)
	, description(_description ? std::string(_description) : std::string(""))
	, enumerated_values()
	, type(_type)
	, fmt(FMT_DEFAULT)
	, is_mutable(true)
	, is_visible(true)
	, is_serializable(true)
	, is_modified(false)
	, listener_set()
{
	if(_owner)
	{
		_owner->Add(*this);
	}
	Simulator::Instance()->Register(this);
}

VariableBase::
VariableBase(const char *_name, VariableBase *_container, Type _type, const char *_description)
	: name(_container ? std::string(_container->GetName()) + std::string(".") + std::string(_name) : std::string(_name))
	, var_name(_container ? std::string(_container->GetVarName()) + std::string(".") + std::string(_name) : std::string(_name))
	, owner(0)
	, container(_container)
	, description(_description ? std::string(_description) : std::string(""))
	, enumerated_values()
	, type(_type)
	, fmt(FMT_DEFAULT)
	, is_mutable(true)
	, is_visible(true)
	, is_serializable(true)
	, is_modified(false)
	, listener_set()
{
	Simulator::Instance()->Register(this);
}

VariableBase::
VariableBase()
	: name()
	, owner(0)
	, container(0)
	, description()
	, type(VAR_VOID)
	, fmt(FMT_DEFAULT)
	, is_mutable(false)
	, is_visible(false)
	, is_serializable(false)
	, is_modified(false)
	, listener_set()
{
}

VariableBase::~VariableBase()
{
	if(owner) owner->Remove(*this);
	Simulator::Instance()->Unregister(this);
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

const char *VariableBase::GetTypeName() const
{
	switch(type)
	{
		case VAR_VOID: return "void";
		case VAR_ARRAY: return "array";
		case VAR_PARAMETER: return "parameter";
		case VAR_STATISTIC: return "statistic";
		case VAR_REGISTER: return "register";
		case VAR_FORMULA: return "formula";
		case VAR_SIGNAL: return "signal";
	}
	return "invalid";
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
	std::vector<std::string>::const_iterator iter;

	for(iter = enumerated_values.begin(); iter != enumerated_values.end(); iter++) {
		if(iter->compare(value) == 0) {
			return true;
		}
	}
	return false;
}

void VariableBase::GetEnumeratedValues(std::vector<std::string> &values) const {
	if(!HasEnumeratedValues()) return;
	for(unsigned int i = 0; i < enumerated_values.size(); i++) {
		values.push_back(enumerated_values[i]);
	}
}

bool VariableBase::AddEnumeratedValue(const char *value) {
	if(HasEnumeratedValue(value)) return false;
	enumerated_values.push_back(std::string(value));
	return true;
}

bool VariableBase::RemoveEnumeratedValue(const char *value) {
	std::vector<std::string>::iterator it;
	for(it = enumerated_values.begin(); it != enumerated_values.end(); it++) {
		if(it->compare(value) == 0) {
			enumerated_values.erase(it);
			return true;
		}
	}
	return false;
}

void VariableBase::SetDescription(const char *_description)
{
	description = _description;
}

void VariableBase::SetFormat(Format _fmt)
{
	fmt = _fmt;
}

bool VariableBase::IsVoid() const
{
	return this == Simulator::Instance()->void_variable;
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

bool VariableBase::IsModified() const
{
	return is_modified;
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

void VariableBase::SetModified(bool _is_modified)
{
	is_modified = _is_modified;
}

void
VariableBase::AddListener( VariableBaseListener* listener )
{
  listener_set.insert( listener );
}

void
VariableBase::RemoveListener( VariableBaseListener* listener )
{
  listener_set.erase( listener );
}

void
VariableBase::NotifyListeners()
{
  typedef std::set<VariableBaseListener*>::iterator listener_iterator;
  for (listener_iterator itr = listener_set.begin(), end = listener_set.end(); itr != end; ++itr)
    {
      (*itr)->VariableBaseNotify( this );
    }
}

VariableBase::operator bool () const { return false; }
VariableBase::operator signed char () const { return (long long) *this; }
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
VariableBase::operator std::string () const { return std::string(); }

VariableBase& VariableBase::operator = (bool value) { NotifyListeners(); return *this; }
VariableBase& VariableBase::operator = (char value) { *this = (long long) value; return *this; }
VariableBase& VariableBase::operator = (short value) { *this = (long long) value; return *this; }
VariableBase& VariableBase::operator = (int value) { *this = (long long) value; return *this; }
VariableBase& VariableBase::operator = (long value) { *this = (long long) value; return *this; }
VariableBase& VariableBase::operator = (long long value) { NotifyListeners(); return *this; }
VariableBase& VariableBase::operator = (unsigned char value) { *this = (unsigned long long) value; return *this; }
VariableBase& VariableBase::operator = (unsigned short value) { *this = (unsigned long long) value; return *this; }
VariableBase& VariableBase::operator = (unsigned int value) { *this = (unsigned long long) value; return *this; }
VariableBase& VariableBase::operator = (unsigned long value) { *this = (unsigned long long) value; return *this; }
VariableBase& VariableBase::operator = (unsigned long long value) { NotifyListeners(); return *this; }
VariableBase& VariableBase::operator = (float value) { *this = (double) value; return *this; }
VariableBase& VariableBase::operator = (double value) { NotifyListeners(); return *this; }
VariableBase& VariableBase::operator = (const char *value) { NotifyListeners(); return *this; }

VariableBase& VariableBase::operator [] (unsigned int index)
{
	if(index >= 0)
	{
		std::cerr << "Subscript out of range" << std::endl;
		return *Simulator::Instance()->void_variable;
	}
	return *this;
}

const VariableBase& VariableBase::operator [] (unsigned int index) const
{
	if(index >= 0)
	{
		std::cerr << "Subscript out of range" << std::endl;
		return *Simulator::Instance()->void_variable;
	}
	return *this;
}

unsigned int VariableBase::GetLength() const
{
	return 0;
}

unsigned int VariableBase::GetBitSize() const { return 0; }

VariableBase& VariableBase::operator = (const VariableBase& variable)
{
	std::string variable_value = (std::string) variable;
	return *this = variable_value.c_str();
}

std::string VariableBase::GetSymbolicValue() const
{
	return IsVisible() ? name : (std::string) *this;
}

void VariableBase::GenerateLatexDocumentation(std::ostream& os) const
{
	os << "\\multicolumn{1}{|p{7.5cm}}{\\textbf{Name:} " << string_to_latex(name.c_str(), 26, "texttt") << "} & \\multicolumn{1}{p{7.5cm}|}{\\textbf{Type:} " << string_to_latex(GetTypeName(), 36, "texttt") << "}\\\\" << std::endl;
	
	if(type == VAR_FORMULA)
	{
		// symbolically print formula 
		os << "\\multicolumn{1}{|p{7.5cm}}{\\textbf{Formula:} ";
		os << string_to_latex(GetSymbolicValue().c_str(), 24, "texttt");
	}
	else if(type == VAR_STATISTIC)
	{
		os << "\\multicolumn{1}{|p{7.5cm}}{";
	}
	else
	{
		// print variable current value
		os << "\\multicolumn{1}{|p{7.5cm}}{\\textbf{Default:} ";
		os << string_to_latex(((std::string) *this).c_str(), 24, "texttt");
	}
	os << "} & \\multicolumn{1}{p{7.5cm}|}{\\textbf{Data type:} " << string_to_latex(GetDataTypeName(), 36, "texttt") << "}\\\\" << std::endl;
	
	if(HasEnumeratedValues())
	{
		os << "\\multicolumn{2}{|p{15cm}|}{\\textbf{Valid:} ";
		
		std::vector<std::string>::const_iterator enumerated_values_iter;
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
		os << "\\multicolumn{2}{|l|}{}\\\\" << std::endl;
	}
	
	if(!description.empty())
	{
		os << "\\multicolumn{2}{|l|}{}\\\\" << std::endl;
		os << "\\multicolumn{2}{|p{15cm}|}{\\textbf{Description:} \\newline " << string_to_latex(description.c_str()) << ".}\\\\" << std::endl;
	}
	os << "\\hline" << std::endl;
}


//=============================================================================
//=                            Variable<TYPE>                                =
//=============================================================================

template <class TYPE>
Variable<TYPE>::Variable(const char *_name, Object *_owner, TYPE& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{
	Simulator::Instance()->Initialize(this);
}

template <class TYPE>
unsigned int Variable<TYPE>::GetBitSize() const { return sizeof(TYPE) * 8; }

template <class TYPE> Variable<TYPE>::operator bool () const
{
	return bool( Get() );
}

template <class TYPE> Variable<TYPE>::operator long long () const
{
	return (long long)( Get() );
}

template <class TYPE> Variable<TYPE>::operator unsigned long long () const
{
	return (unsigned long long)( Get() );
}

template <class TYPE> Variable<TYPE>::operator double () const
{
	return double( Get() );
}

template <class TYPE> Variable<TYPE>::operator std::string () const
{
	TYPE value = Get();

	std::stringstream sstr;
	switch(GetFormat())
	{
		case FMT_DEFAULT:
		case FMT_HEX:
			sstr << "0x" << std::hex;
			sstr.fill('0');
			sstr.width(2 * sizeof(TYPE));
			sstr << (unsigned long long)( value );
			break;
		case FMT_DEC:
			sstr << std::dec;
			if(std::numeric_limits<TYPE>::is_signed)
				sstr << (long long)( value );
			else
				sstr << (unsigned long long)( value );
			break;
	}
	return sstr.str();
}

template <class TYPE> VariableBase& Variable<TYPE>::operator = (bool value)
{
	if (IsMutable()) {
		Set( value );
	}
	return *this;
}

template <class TYPE> VariableBase& Variable<TYPE>::operator = (long long value)
{
	if (IsMutable()) {
		Set( value );
	}
	return *this;
}

template <class TYPE> VariableBase& Variable<TYPE>::operator = (unsigned long long value)
{
	if (IsMutable()) {
		Set( value );
	}
	return *this;
}

template <class TYPE> VariableBase& Variable<TYPE>::operator = (double value)
{
	if (IsMutable()) {
		Set( value );
	}
	return *this;
}

//=============================================================================
//=                           FormulaOperator                                 =
//=============================================================================

FormulaOperator::FormulaOperator(const char *name)
{
	if((strcmp(name, "+") == 0) || (strcmp(name, "add") == 0) || (strcmp(name, "ADD") == 0)) op = OP_ADD;
	else if((strcmp(name, "-") == 0) || (strcmp(name, "sub") == 0) || (strcmp(name, "SUB") == 0)) op = OP_SUB;
	else if((strcmp(name, "*") == 0) || (strcmp(name, "mul") == 0) || (strcmp(name, "MUL") == 0)) op = OP_MUL;
	else if((strcmp(name, "/") == 0) || (strcmp(name, "div") == 0) || (strcmp(name, "DIV") == 0)) op = OP_DIV;
	else if((strcmp(name, "<") == 0) || (strcmp(name, "lt") == 0) || (strcmp(name, "LT") == 0)) op = OP_LT;
	else if((strcmp(name, "<=") == 0) || (strcmp(name, "=<") == 0) || (strcmp(name, "lte") == 0) || (strcmp(name, "LTE") == 0)) op = OP_LTE;
	else if((strcmp(name, ">") == 0) || (strcmp(name, "gt") == 0) || (strcmp(name, "GT") == 0)) op = OP_GT;
	else if((strcmp(name, ">=") == 0) || (strcmp(name, "=>") == 0) || (strcmp(name, "gte") == 0)) op = OP_GTE;
	else if((strcmp(name, "==") == 0) || (strcmp(name, "=") == 0) || (strcmp(name, "eq") == 0) || (strcmp(name, "EQ") == 0)) op = OP_EQ;
	else if((strcmp(name, "?") == 0) || (strcmp(name, "sel") == 0) || (strcmp(name, "SEL") == 0)) op = OP_SEL;
	else if((strcmp(name, "neg") == 0) || (strcmp(name, "NEG") == 0)) op = OP_NEG;
	else if((strcmp(name, "abs") == 0) || (strcmp(name, "ABS") == 0)) op = OP_ABS;
	else if((strcmp(name, "min") == 0) || (strcmp(name, "MIN") == 0)) op = OP_MIN;
	else if((strcmp(name, "max") == 0) || (strcmp(name, "MAX") == 0)) op = OP_MAX;
	else if((strcmp(name, "&") == 0) || (strcmp(name, "and") == 0) || (strcmp(name, "AND") == 0)) op = OP_AND;
	else if((strcmp(name, "|") == 0) || (strcmp(name, "or") == 0) || (strcmp(name, "OR") == 0)) op = OP_OR;
	else if((strcmp(name, "!=") == 0) || (strcmp(name, "<>") == 0) || (strcmp(name, "ne") == 0) || (strcmp(name, "NE") == 0)) op = OP_NEQ;
	else if((strcmp(name, "!") == 0) || (strcmp(name, "~") == 0) || (strcmp(name, "not") == 0) || (strcmp(name, "NOT") == 0)) op = OP_NOT;
	else op = OP_UNKNOWN;
}

FormulaOperator::FormulaOperator(Operator _op)
	: op(_op)
{
}

FormulaOperator::operator FormulaOperator::Operator() const
{
	return op;
}

//=============================================================================
//=                             Formula<TYPE>                                 =
//=============================================================================

template <class TYPE>
Formula<TYPE>::Formula(const char *_name, Object *_owner, FormulaOperator _formula_op, VariableBase *child1, VariableBase *child2, VariableBase *child3, const char *_description)
	: VariableBase(_name, _owner, VAR_FORMULA, _description)
	, op(_formula_op)
{
	VariableBase::SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	childs[0] = child1;
	childs[1] = child2;
	childs[2] = child3;
	if(!IsValid() && Simulator::Instance()->IsWarningEnabled())
	{
		std::cerr << "WARNING! " << VariableBase::GetName() << " is an invalid formula" << std::endl;
	}
}

template <class TYPE>
Formula<TYPE>::Formula(const char *_name, Object *_owner, FormulaOperator _formula_op, VariableBase *child1, VariableBase *child2, const char *_description)
	: VariableBase(_name, _owner, VAR_FORMULA, _description)
	, op(_formula_op)
{
	VariableBase::SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	childs[0] = child1;
	childs[1] = child2;
	childs[2] = 0;
	if(!IsValid() && Simulator::Instance()->IsWarningEnabled())
	{
		std::cerr << "WARNING! " << VariableBase::GetName() << " is an invalid formula" << std::endl;
	}
}

template <class TYPE>
Formula<TYPE>::Formula(const char *_name, Object *_owner, FormulaOperator _formula_op, VariableBase *child, const char *_description)
	: VariableBase(_name, _owner, VAR_FORMULA, _description)
	, op(_formula_op)
{
	VariableBase::SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	childs[0] = child;
	childs[1] = 0;
	childs[2] = 0;
	if(!IsValid() && Simulator::Instance()->IsWarningEnabled())
	{
		std::cerr << "WARNING! " << VariableBase::GetName() << " is an invalid formula" << std::endl;
	}
}

template <class TYPE>
bool Formula<TYPE>::IsValid() const
{
	switch(op)
	{
		case FormulaOperator::OP_UNKNOWN:
			return false;
		case FormulaOperator::OP_NEG:
		case FormulaOperator::OP_ABS:
		case FormulaOperator::OP_NOT:
			return childs[0] != 0;
		case FormulaOperator::OP_ADD:
		case FormulaOperator::OP_SUB:
		case FormulaOperator::OP_MUL:
		case FormulaOperator::OP_DIV:
		case FormulaOperator::OP_LT:
		case FormulaOperator::OP_LTE:
		case FormulaOperator::OP_GT:
		case FormulaOperator::OP_GTE:
		case FormulaOperator::OP_EQ:
		case FormulaOperator::OP_MIN:
		case FormulaOperator::OP_MAX:
		case FormulaOperator::OP_AND:
		case FormulaOperator::OP_OR:
		case FormulaOperator::OP_NEQ:
			return (childs[0] != 0) && (childs[1] != 0);
		case FormulaOperator::OP_SEL:
			return (childs[0] != 0) && (childs[1] != 0) && (childs[2] != 0);
	}
	return false;
}

template <class TYPE> Formula<TYPE>::operator bool () const { return Compute() ? true : false; }
template <class TYPE> Formula<TYPE>::operator long long () const { return (long long) Compute(); }
template <class TYPE> Formula<TYPE>::operator unsigned long long () const { return (unsigned long long) Compute(); }
template <class TYPE> Formula<TYPE>::operator double () const { return (double) Compute(); }
template <class TYPE> Formula<TYPE>::operator std::string () const
{
	std::stringstream sstr;
	switch(GetFormat())
	{
		case FMT_DEFAULT:
		case FMT_HEX:
			sstr << "0x" << std::hex << (unsigned long long) Compute();
			break;
		case FMT_DEC:
			sstr << std::dec << (unsigned long long) Compute();
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
		case FormulaOperator::OP_UNKNOWN: return TYPE();
		case FormulaOperator::OP_ADD: return (TYPE)(childs[0] ? *childs[0] : TYPE()) + (TYPE)(childs[1] ? *childs[1] : TYPE());
		case FormulaOperator::OP_SUB: return (TYPE)(childs[0] ? *childs[0] : TYPE()) - (TYPE)(childs[1] ? *childs[1] : TYPE());
		case FormulaOperator::OP_MUL: return (TYPE)(childs[0] ? *childs[0] : TYPE()) * (TYPE)(childs[1] ? *childs[1] : TYPE());
		case FormulaOperator::OP_DIV: return (TYPE)(childs[0] ? *childs[0] : TYPE()) / (TYPE)(childs[1] ? *childs[1] : TYPE());
		case FormulaOperator::OP_LT: return (TYPE)(childs[0] ? *childs[0] : TYPE()) < (TYPE)(childs[1] ? *childs[1] : TYPE());
		case FormulaOperator::OP_LTE: return (TYPE)(childs[0] ? *childs[0] : TYPE()) <= (TYPE)(childs[1] ? *childs[1] : TYPE());
		case FormulaOperator::OP_GT: return (TYPE)(childs[0] ? *childs[0] : TYPE()) > (TYPE)(childs[1] ? *childs[1] : TYPE());
		case FormulaOperator::OP_GTE: return (TYPE)(childs[0] ? *childs[0] : TYPE()) >= (TYPE)(childs[1] ? *childs[1] : TYPE());
		case FormulaOperator::OP_EQ: return (TYPE)(childs[0] ? *childs[0] : TYPE()) == (TYPE)(childs[1] ? *childs[1] : TYPE());
		case FormulaOperator::OP_SEL: return (TYPE)(childs[0] ? *childs[0] : TYPE()) ? (TYPE)(childs[1] ? *childs[1] : TYPE()) : (TYPE)(childs[2] ? *childs[2] : TYPE());
		case FormulaOperator::OP_NEG: return -(TYPE)(childs[0] ? *childs[0] : TYPE());
		case FormulaOperator::OP_ABS: return (TYPE)(childs[0] ? *childs[0] : TYPE()) >= TYPE() ? (TYPE)(childs[0] ? *childs[0] : TYPE()) : -(TYPE)(childs[0] ? *childs[0] : TYPE());
		case FormulaOperator::OP_MIN: return (TYPE)(childs[0] ? *childs[0] : TYPE()) < (TYPE)(childs[1] ? *childs[1] : TYPE()) ? (TYPE)(childs[0] ? *childs[0] : TYPE()) : (TYPE)(childs[1] ? *childs[1] : TYPE());
		case FormulaOperator::OP_MAX: return (TYPE)(childs[0] ? *childs[0] : TYPE()) > (TYPE)(childs[1] ? *childs[1] : TYPE()) ? (TYPE)(childs[0] ? *childs[0] : TYPE()) : (TYPE)(childs[1] ? *childs[1] : TYPE());
		case FormulaOperator::OP_AND: return (TYPE)(childs[0] ? *childs[0] : TYPE()) && (TYPE)(childs[1] ? *childs[1] : TYPE());
		case FormulaOperator::OP_OR: return (TYPE)(childs[0] ? *childs[0] : TYPE()) || (TYPE)(childs[1] ? *childs[1] : TYPE());
		case FormulaOperator::OP_NEQ: return (TYPE)(childs[0] ? *childs[0] : TYPE()) != (TYPE)(childs[1] ? *childs[1] : TYPE());
		case FormulaOperator::OP_NOT: return !(TYPE)(childs[0] ? *childs[0] : TYPE());
	}
	return 0;
}

template <class TYPE>
std::string Formula<TYPE>::GetSymbolicValue() const
{
	std::stringstream sstr;
	switch(op)
	{
		case FormulaOperator::OP_UNKNOWN:
			sstr << "unknown";
			break;
		case FormulaOperator::OP_NEG:
			sstr << "-";
			if(childs[0])
				sstr << childs[0]->GetSymbolicValue();
			else
				sstr << TYPE();
			break;
		case FormulaOperator::OP_ABS:
			sstr << "ABS(";
			if(childs[0])
				sstr << childs[0]->GetSymbolicValue();
			else
				sstr << TYPE();
			sstr << ")";
			break;
		case FormulaOperator::OP_NOT:
			sstr << "!";
			if(childs[0])
				sstr << childs[0]->GetSymbolicValue();
			else
				sstr << TYPE();
			break;
		case FormulaOperator::OP_ADD:
			if(childs[0])
				sstr << childs[0]->GetSymbolicValue();
			else
				sstr << TYPE();
			sstr << " + ";
			if(childs[1])
				sstr << childs[1]->GetSymbolicValue();
			else
				sstr << TYPE();
			break;
		case FormulaOperator::OP_SUB:
			if(childs[0])
				sstr << childs[0]->GetSymbolicValue();
			else
				sstr << TYPE();
			sstr << " - ";
			if(childs[1])
				sstr << childs[1]->GetSymbolicValue();
			else
				sstr << TYPE();
			break;
		case FormulaOperator::OP_MUL:
			if(childs[0])
				sstr << childs[0]->GetSymbolicValue();
			else
				sstr << TYPE();
			sstr << " * ";
			if(childs[1])
				sstr << childs[1]->GetSymbolicValue();
			else
				sstr << TYPE();
			break;
		case FormulaOperator::OP_DIV:
			if(childs[0])
				sstr << childs[0]->GetSymbolicValue();
			else
				sstr << TYPE();
			sstr << " / ";
			if(childs[1])
				sstr << childs[1]->GetSymbolicValue();
			else
				sstr << TYPE();
			break;
		case FormulaOperator::OP_LT:
			if(childs[0])
				sstr << childs[0]->GetSymbolicValue();
			else
				sstr << TYPE();
			sstr << " < ";
			if(childs[1])
				sstr << childs[1]->GetSymbolicValue();
			else
				sstr << TYPE();
			break;
		case FormulaOperator::OP_LTE:
			if(childs[0])
				sstr << childs[0]->GetSymbolicValue();
			else
				sstr << TYPE();
			sstr << " <= ";
			if(childs[1])
				sstr << childs[1]->GetSymbolicValue();
			else
				sstr << TYPE();
			break;
		case FormulaOperator::OP_GT:
			if(childs[0])
				sstr << childs[0]->GetSymbolicValue();
			else
				sstr << TYPE();
			sstr << " > ";
			if(childs[1])
				sstr << childs[1]->GetSymbolicValue();
			else
				sstr << TYPE();
			break;
		case FormulaOperator::OP_GTE:
			if(childs[0])
				sstr << childs[0]->GetSymbolicValue();
			else
				sstr << TYPE();
			sstr << " >= ";
			if(childs[1])
				sstr << childs[1]->GetSymbolicValue();
			else
				sstr << TYPE();
			break;
		case FormulaOperator::OP_EQ:
			if(childs[0])
				sstr << childs[0]->GetSymbolicValue();
			else
				sstr << TYPE();
			sstr << " == ";
			if(childs[1])
				sstr << childs[1]->GetSymbolicValue();
			else
				sstr << TYPE();
			break;
		case FormulaOperator::OP_MIN:
			sstr << "MIN(";
			if(childs[0])
				sstr << childs[0]->GetSymbolicValue();
			else
				sstr << TYPE();
			sstr << ", ";
			if(childs[1])
				sstr << childs[1]->GetSymbolicValue();
			else
				sstr << TYPE();
			sstr << ")";
			break;
		case FormulaOperator::OP_MAX:
			sstr << "MAX(";
			if(childs[0])
				sstr << childs[0]->GetSymbolicValue();
			else
				sstr << TYPE();
			sstr << ", ";
			if(childs[1])
				sstr << childs[1]->GetSymbolicValue();
			else
				sstr << TYPE();
			sstr << ")";
			break;
		case FormulaOperator::OP_AND:
			if(childs[0])
				sstr << childs[0]->GetSymbolicValue();
			else
				sstr << TYPE();
			sstr << " & ";
			if(childs[1])
				sstr << childs[1]->GetSymbolicValue();
			else
				sstr << TYPE();
			break;
		case FormulaOperator::OP_OR:
			if(childs[0])
				sstr << childs[0]->GetSymbolicValue();
			else
				sstr << TYPE();
			sstr << " | ";
			if(childs[1])
				sstr << childs[1]->GetSymbolicValue();
			else
				sstr << TYPE();
			break;
		case FormulaOperator::OP_NEQ:
			if(childs[0])
				sstr << childs[0]->GetSymbolicValue();
			else
				sstr << TYPE();
			sstr << " != ";
			if(childs[1])
				sstr << childs[1]->GetSymbolicValue();
			else
				sstr << TYPE();
			break;
		case FormulaOperator::OP_SEL:
			if(childs[0])
				sstr << childs[0]->GetSymbolicValue();
			else
				sstr << TYPE();
			sstr << " ? ";
			if(childs[1])
				sstr << childs[1]->GetSymbolicValue();
			else
				sstr << TYPE();
			sstr << " : ";
			if(childs[2])
				sstr << childs[2]->GetSymbolicValue();
			else
				sstr << TYPE();
			break;
	}
	return sstr.str();
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
	Simulator::Instance()->Initialize(this);
}

template <>
const char *Variable<bool>::GetDataTypeName() const
{
	return "boolean";
}

template <>
Variable<signed char>::Variable(const char *_name, Object *_owner, signed char& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{
	Simulator::Instance()->Initialize(this);
}

template <>
const char *Variable<signed char>::GetDataTypeName() const
{
	return GetSignedDataTypeName<signed char>(); //"char";
}

template <>
Variable<short>::Variable(const char *_name, Object *_owner, short& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{
	Simulator::Instance()->Initialize(this);
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
	Simulator::Instance()->Initialize(this);
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
	Simulator::Instance()->Initialize(this);
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
	Simulator::Instance()->Initialize(this);
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
	Simulator::Instance()->Initialize(this);
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
	Simulator::Instance()->Initialize(this);
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
	Simulator::Instance()->Initialize(this);
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
	Simulator::Instance()->Initialize(this);
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
	Simulator::Instance()->Initialize(this);
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
	Simulator::Instance()->Initialize(this);
}

template <>
const char *Variable<double>::GetDataTypeName() const
{
	return "double precision floating-point";
}

template <>
Variable<double>::operator std::string () const
{
	std::stringstream sstr;
	sstr << Get();
	return sstr.str();
}

template <> 
Variable<float>::Variable(const char *_name, Object *_owner, float& _storage, Type type, const char *_description)
  : VariableBase(_name, _owner, type, _description), storage(&_storage)
{
	Simulator::Instance()->Initialize(this);
}

template <>
const char *Variable<float>::GetDataTypeName() const
{
	return "single precision floating-point";
}

template <>
Variable<float>::operator std::string () const
{
	std::stringstream sstr;
	sstr << Get();
	return sstr.str();
}

template <> 
Variable<std::string>::Variable(const char *_name, Object *_owner, std::string& _storage, Type type, const char *_description)
  : VariableBase(_name, _owner, type, _description), storage(&_storage)
{
	Simulator::Instance()->Initialize(this);
}

template <>
const char *Variable<std::string>::GetDataTypeName() const
{
	return "string";
}

template <> Variable<bool>::operator std::string () const
{
	bool value = Get();
	std::stringstream sstr;
	switch(GetFormat())
	{
		case FMT_DEFAULT:
			sstr << (value ? "true" : "false");
			break;
		case FMT_HEX:
			sstr << (value ? "0x1" : "0x0");
			break;
		case FMT_DEC:
			sstr << (value ? "1" : "0");
			break;
	}
	return sstr.str();
}

namespace
{
  long long ParseSigned( char const* value )
  {
    return (strcmp(value, "true") == 0) ? 1 : ((strcmp(value, "false") == 0) ? 0 : strtoll(value, 0, 0));
  }
  unsigned long long ParseUnsigned( char const* value )
  {
    return (strcmp(value, "true") == 0) ? 1 : ((strcmp(value, "false") == 0) ? 0 : strtoull(value, 0, 0));
  }
  double ParseDouble( char const* value )
  {
    return (strcmp(value, "true") == 0) ? 1.0 : ((strcmp(value, "false") == 0) ? 0.0 : strtod(value, 0));
  }
  
}

template <> VariableBase& Variable<bool>::operator = (const char *value)
{
	if (IsMutable()) {
		Set( ParseSigned( value ) );
	}
	return *this;
}
template <> VariableBase& Variable<signed char>::operator = (const char *value)
{
	if (IsMutable()) {
		Set( ParseSigned( value ) );
	}
	return *this;
}
template <> VariableBase& Variable<short>::operator = (const char *value)
{
	if (IsMutable()) {
		Set( ParseSigned( value ) );
	}
	return *this;
}
template <> VariableBase& Variable<int>::operator = (const char *value)
{
	if (IsMutable()) {
		Set( ParseSigned( value ) );
	}
	return *this;
}
template <> VariableBase& Variable<long>::operator = (const char *value)
{
	if (IsMutable()) {
		Set( ParseSigned( value ) );
	}
	return *this;
}
template <> VariableBase& Variable<long long>::operator = (const char *value)
{
	if (IsMutable()) {
		Set( ParseSigned( value ) );
	}
	return *this;
}
template <> VariableBase& Variable<unsigned char>::operator = (const char *value)
{
	if (IsMutable()) {
		Set( ParseUnsigned( value ) );
	}
	return *this;
}
template <> VariableBase& Variable<unsigned short>::operator = (const char *value)
{
	if (IsMutable()) {
		Set( ParseUnsigned( value ) );
	}
	return *this;
}
template <> VariableBase& Variable<unsigned int>::operator = (const char *value)
{
	if (IsMutable()) {
		Set( ParseUnsigned( value ) );
	}
	return *this;
}
template <> VariableBase& Variable<unsigned long>::operator = (const char *value)
{
	if (IsMutable()) {
		Set( ParseUnsigned( value ) );
	}
	return *this;
}
template <> VariableBase& Variable<unsigned long long>::operator = (const char *value)
{
	if (IsMutable()) {
		Set( ParseUnsigned( value ) );
	}
	return *this;
}
template <> VariableBase& Variable<float>::operator = (const char *value)
{
	if (IsMutable()) {
		Set( ParseDouble( value ) );
	}
	return *this;
}
template <> VariableBase& Variable<double>::operator = (const char *value)
{
	if (IsMutable()) {
		Set( ParseDouble( value ) );
	}
	return *this;
}

template <> Variable<std::string>::operator bool () const { return ParseSigned(storage->c_str()); }
template <> Variable<std::string>::operator long long () const { return ParseSigned(storage->c_str()); }
template <> Variable<std::string>::operator unsigned long long () const { return ParseSigned(storage->c_str()); }
template <> Variable<std::string>::operator double () const { return ParseDouble(storage->c_str()); }
template <> Variable<std::string>::operator std::string () const { return *storage; }

template <> VariableBase& Variable<std::string>::operator = (bool value)
{
	if (IsMutable()) {
		Set( value ? "true" : "false" );
	}
	return *this;
}
template <> VariableBase& Variable<std::string>::operator = (long long value)
{
	if(IsMutable())
	{
		std::stringstream sstr;
		sstr << "0x" << std::hex << value;
		std::string tmp = sstr.str();
		Set( tmp );
	}
	return *this;
}
template <> VariableBase& Variable<std::string>::operator = (unsigned long long value)
{
	if (IsMutable())
	{
		std::stringstream sstr;
		sstr << "0x" << std::hex << value;
		std::string tmp = sstr.str();
		Set( tmp );
	}
	return *this;
}
template <> VariableBase& Variable<std::string>::operator = (double value)
{
	if (IsMutable())
	{
		std::stringstream sstr;
		sstr << value;
		std::string tmp = sstr.str();
		Set( tmp );
	}
	return *this;
}
template <> VariableBase& Variable<std::string>::operator = (const char *value)
{
	if (IsMutable())
	{
		Set( value );
	}
	return *this;
}

//=============================================================================
//=                         specialized Formula<>                             =
//=============================================================================

template <>
Formula<double>::operator std::string () const
{
	std::stringstream sstr;
	sstr << Compute();
	return sstr.str();
}

template <>
const char *Formula<bool>::GetDataTypeName() const
{
	return "boolean";
}

template <>
const char *Formula<signed char>::GetDataTypeName() const
{
	return GetSignedDataTypeName<signed char>();
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
template class Variable<signed char>;
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
template class Variable<std::string>;

template class VariableArray<bool>;
template class VariableArray<signed char>;
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
template class VariableArray<std::string>;

template class Formula<bool>;
template class Formula<signed char>;
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
	: object_name(_parent ? (std::string(_parent->GetName()) + std::string(".") + std::string(_name)) : std::string(_name))
	, object_base_name(std::string(_name))
	, description(_description ? std::string(_description) : std::string(""))
	, parent(_parent)
	, srv_imports()
	, srv_exports()
	, leaf_objects()
{
	if(_parent) _parent->Add(*this);
	Simulator::Instance()->Register(this);
}

Object::~Object()
{
	if(parent) parent->Remove(*this);
	Simulator::Instance()->Unregister(this);
}

const char *Object::GetName() const
{
	return object_name.c_str();
}

const char *Object::GetObjectName() const
{
	return object_base_name.c_str();
}

Object *Object::GetParent() const
{
	return parent;
}

const std::list<ServiceExportBase *>& Object::GetServiceExports() const
{
	return srv_exports;
}

void Object::Add(ServiceExportBase& srv_export)
{
	srv_exports.push_back(&srv_export);
}

void Object::Remove(ServiceExportBase& srv_export)
{
	std::list<ServiceExportBase *>::iterator export_iter;

	for(export_iter = srv_exports.begin(); export_iter != srv_exports.end(); export_iter++)
	{
		if(*export_iter == &srv_export)
		{
			srv_exports.erase(export_iter);
			return;
		}
	}
}

const std::list<ServiceImportBase *>& Object::GetServiceImports() const
{
	return srv_imports;
}

void Object::Add(ServiceImportBase& srv_import)
{
	srv_imports.push_back(&srv_import);
}

void Object::Remove(ServiceImportBase& srv_import)
{
	std::list<ServiceImportBase *>::iterator import_iter;

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
	std::list<Object *>::iterator object_iter;
	for(object_iter = leaf_objects.begin(); object_iter != leaf_objects.end(); object_iter++)
	{
		if(unisim::util::nat_sort::nat_ltstr::Less(object.GetName(), (*object_iter)->GetName()))
		{
			leaf_objects.insert(object_iter, &object);
			return;
		}
	}
	leaf_objects.push_back(&object);
}

void Object::Remove(Object& object)
{
	std::list<Object *>::iterator object_iter;

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
	std::list<VariableBase *>::iterator variable_iter;
	for(variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		if(unisim::util::nat_sort::nat_ltstr::Less(var.GetName(), (*variable_iter)->GetName()))
		{
			variables.insert(variable_iter, &var);
			return;
		}
	}
	variables.push_back(&var);
}

void Object::Remove(VariableBase& var)
{
	std::list<VariableBase *>::iterator var_iter;

	for(var_iter = variables.begin(); var_iter != variables.end(); var_iter++)
	{
		if(*var_iter == &var)
		{
			variables.erase(var_iter);
			return;
		}
	}
}

void Object::GetVariables(std::list<VariableBase *>& lst, VariableBase::Type type) const
{
	std::list<VariableBase *>::const_iterator variable_iter;

	lst.clear();
	
	for(variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		VariableBase *variable = *variable_iter;
		if(((type == VariableBase::VAR_VOID) || (variable->GetType() == type)) && variable->IsVisible())
		{
			lst.push_back(*variable_iter);
		}
	}
}

const std::list<Object *>& Object::GetLeafs() const
{
	return leaf_objects;
}

void Object::Disconnect()
{
	std::list<ServiceImportBase *>::iterator import_iter;

	for(import_iter = srv_imports.begin(); import_iter != srv_imports.end(); import_iter++)
	{
#ifdef DEBUG_SERVICE
		cerr << (*import_iter)->GetName() << "->DisconnectService()" << std::endl;
#endif
		(*import_iter)->DisconnectService();
	}

	std::list<ServiceExportBase *>::iterator export_iter;

	for(export_iter = srv_exports.begin(); export_iter != srv_exports.end(); export_iter++)
	{
#ifdef DEBUG_SERVICE
		cerr << (*export_iter)->GetName() << "->DisconnectClient()" << std::endl;
#endif
		(*export_iter)->DisconnectClient();
	}
}

bool Object::BeginSetup()
{
	return true;
}

bool Object::Setup(ServiceExportBase *srv_export)
{
	return true;
}

bool Object::EndSetup()
{
	return true;
}

void Object::SigInt()
{
}

void Object::OnDisconnect()
{
//	cerr << "WARNING! Using default OnDisconnect for " << GetName() << std::endl;
}

VariableBase& Object::operator [] (const char *name)
{
	std::string fullname = GetName() + std::string(".") + std::string(name);
	VariableBase *variable = Simulator::Instance()->FindVariable(fullname.c_str());
	return *variable;
}

VariableBase& Object::operator [] (const std::string& name)
{
	std::string fullname = GetName() + std::string(".") + name;
	VariableBase *variable = Simulator::Instance()->FindVariable(fullname.c_str());
	return *variable;
}

Simulator *Object::GetSimulator() const
{
	return Simulator::Instance();
}

const char *Object::GetDescription() const
{
	return description.c_str();
}

void Object::GenerateLatexDocumentation(std::ostream& os, VariableBase::Type variable_type) const
{
	bool header_printed = false;
	
	std::list<VariableBase *>::const_iterator variable_iter;
	
	for(variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		VariableBase *variable = *variable_iter;
		
		if((variable->GetType() == variable_type) && variable->IsVisible())
		{
			if(!header_printed)
			{
				os << "\\multicolumn{2}{|l|}{\\textbf{\\Large " << string_to_latex(GetName()) << "}}\\\\" << std::endl;
				os << "\\hline" << std::endl;
				header_printed = true;
			}
			variable->GenerateLatexDocumentation(os);
		}
	}
	if(header_printed) os << "\\hline" << std::endl;
}

void Object::Stop(int exit_status, bool asynchronous)
{
	Simulator::Instance()->Stop(this, exit_status, asynchronous);
}

void Object::SetDescription(const char *_description)
{
	description = _description;
}

//=============================================================================
//=                           ServiceImportBase                               =
//=============================================================================

ServiceImportBase::ServiceImportBase(const char *_name, Object *_owner) :
	name(std::string(_owner->GetName()) + std::string(".") + std::string(_name)),
	owner(_owner)
{
	_owner->Add(*this);
	Simulator::Instance()->Register(this);
}

ServiceImportBase::~ServiceImportBase()
{
	if(owner) owner->Remove(*this);
	Simulator::Instance()->Unregister(this);
}

const char *ServiceImportBase::GetName() const
{
	return name.c_str();
}

//=============================================================================
//=                           ServiceExportBase                               =
//=============================================================================

ServiceExportBase::ServiceExportBase(const char *_name, Object *_owner) :
	name(std::string(_owner->GetName()) + std::string(".") + std::string(_name)),
	owner(_owner),
	setup_dependencies()
{
	_owner->Add(*this);
	Simulator::Instance()->Register(this);
}

ServiceExportBase::~ServiceExportBase()
{
	if(owner) owner->Remove(*this);
	Simulator::Instance()->Unregister(this);
}

const char *ServiceExportBase::GetName() const
{
	return name.c_str();
}

void ServiceExportBase::SetupDependsOn(ServiceImportBase& srv_import)
{
	setup_dependencies.push_back(&srv_import);
}

std::list<ServiceImportBase *>& ServiceExportBase::GetSetupDependencies()
{
	return setup_dependencies;
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

#if !defined(WIN32) && !defined(_WIN32) && !defined(WIN64) && !defined(_WIN64)
void (*Simulator::sig_pipe_handler)(int) = 0;
void (*Simulator::prev_sig_pipe_handler)(int) = 0;
void (*Simulator::sig_int_handler)(int) = 0;
void (*Simulator::prev_sig_int_handler)(int) = 0;
#endif

Simulator::Simulator(int argc, char **argv, void (*LoadBuiltInConfig)(Simulator *))
	: void_variable(0)
	, shared_data_dir()
	, set_vars()
	, get_config_filename()
	, input_config_file_format("XML")
	, output_config_file_format("XML")
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
	, schematic()
	, var_program_name(0)
	, var_authors(0)
	, var_copyright(0)
	, var_description(0)
	, var_version(0)
	, var_license(0)
	, var_schematic(0)
	, param_enable_press_enter_at_exit(0)
	, command_line_options()
	, objects()
	, imports()
	, exports()
	, variables()
	, cmd_args()
	, param_cmd_args(0)
{
	new unisim::kernel::config::XMLConfigFileHelper(this);
	new unisim::kernel::config::INIConfigFileHelper(this);
	
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	SetConsoleCtrlHandler(&Simulator::ConsoleCtrlHandler, TRUE);
#else
	sig_int_handler = &Simulator::SigIntHandler;
	prev_sig_int_handler = signal(SIGINT, sig_int_handler);
#endif

#if !defined(WIN32) && !defined(_WIN32) && !defined(WIN64) && !defined(_WIN64)
	sig_pipe_handler = &Simulator::SigPipeHandler;
	prev_sig_pipe_handler = signal(SIGPIPE, sig_pipe_handler);
#endif

	bool has_share_data_dir_hint = false;
	std::string shared_data_dir_hint;

	command_line_options.push_back(CommandLineOption('s', "set", "set value of parameter 'param' to 'value'", "param=value"));
	command_line_options.push_back(CommandLineOption('c', "config", "configures the simulator with the given configuration file", "file"));
	command_line_options.push_back(CommandLineOption('g', "get-config", "get the simulator configuration file (you can use it to create your own configuration. This option can be combined with -c to get a new configuration file with existing variables from another file", "file"));
	command_line_options.push_back(CommandLineOption('f', "input-config-file-format", "set the simulator input configuration file format", "XML | INI"));
	command_line_options.push_back(CommandLineOption('F', "output-config-file-format", "set the simulator output configuration file format", "XML | INI"));
	command_line_options.push_back(CommandLineOption('l', "list", "lists all available parameters, their type, and their current value"));
	command_line_options.push_back(CommandLineOption('w', "warn", "enable printing of kernel warnings"));
	command_line_options.push_back(CommandLineOption('d', "doc", "enable printing a latex documentation", "Latex file"));
	command_line_options.push_back(CommandLineOption('v', "version", "displays the program version information"));
	command_line_options.push_back(CommandLineOption('p', "share-path", "the path that should be used for the share directory (absolute path)", "path"));
	command_line_options.push_back(CommandLineOption('h', "help", "displays this help"));
	
	if(LoadBuiltInConfig)
	{
		LoadBuiltInConfig(this);
	}
	
	if(simulator)
	{
		throw std::runtime_error("No more than one instance of unisim::kernel::service::Simulator is allowed");
	}
	
	simulator = this;
	void_variable = new VariableBase("void", (Object *) 0, VariableBase::VAR_VOID, "unknown variable");
	void_variable->SetMutable(false);
	void_variable->SetVisible(false);
	void_variable->SetSerializable(false);

	var_authors = new Parameter<std::string>("authors", 0, authors, "Authors");
	var_authors->SetMutable(false);
	var_authors->SetVisible(false);
	var_authors->SetSerializable(false);

	var_program_name = new Parameter<std::string>("program-name", 0, program_name, "Program name");
	var_program_name->SetMutable(false);
	var_program_name->SetVisible(false);
	var_program_name->SetSerializable(false);

	var_copyright = new Parameter<std::string>("copyright", 0, copyright, "Copyright");
	var_copyright->SetMutable(false);
	var_copyright->SetVisible(false);
	var_copyright->SetSerializable(false);

	var_version = new Parameter<std::string>("version", 0, version, "Version");
	var_version->SetMutable(false);
	var_version->SetVisible(false);
	var_version->SetSerializable(false);

	var_description = new Parameter<std::string>("description", 0, description, "Description");
	var_description->SetMutable(false);
	var_description->SetVisible(false);
	var_description->SetSerializable(false);

	var_license = new Parameter<std::string>("license", 0, license, "License");
	var_license->SetMutable(false);
	var_license->SetVisible(false);
	var_license->SetSerializable(false);

	var_schematic = new Parameter<std::string>("schematic", 0, schematic, "path to simulator schematic");
	var_schematic->SetMutable(false);
	var_schematic->SetVisible(false);
	var_schematic->SetSerializable(false);

	param_enable_press_enter_at_exit = new Parameter<bool>("enable-press-enter-at-exit", 0, enable_press_enter_at_exit, "Enable/Disable pressing key enter at exit");
	
	// parse command line arguments (first pass)
	char **arg_end = argv + argc;
	
	int state = 0;
	for(char **arg = argv + 1; arg < arg_end; arg++)
	{
		switch(state)
		{
			case 0:
				// Unless there's an acceptable option entry assume options parsing ends here
				state = -1;
				
				// "--" unconditionally ends option parsing afterwards
				if (strcmp(*arg, "--") == 0)
				{
					arg++;
					break;
				}
				
				for (std::vector<CommandLineOption>::const_iterator cmd_opt_itr = command_line_options.begin(),
				                                                    cmd_opt_end = command_line_options.end();
				     cmd_opt_itr != cmd_opt_end; ++cmd_opt_itr)
				{
					if(*cmd_opt_itr == *arg)
					{
						switch(cmd_opt_itr->GetShortName())
						{
							case 's':
								state = 1;
								break;
							case 'c':
								state = 2;
								break;
							case 'f':
								state = 6;
								break;
							case 'F':
								state = 7;
								break;
							case 'g':
								state = 3;
								break;
							case 'l':
								list_parms = true;
								state = 0;
								break;
							case 'v':
								enable_version = true;
								state = 0;
								break;
							case 'h':
								enable_help = true;
								state = 0;
								break;
							case 'w':
								enable_warning = true;
								state = 0;
								break;
							case 'd':
								state = 4;
								break;
							case 'p':
								has_share_data_dir_hint = true;
								state = 5;
								break;
							default:
								break;
						}
						break;
					}
				}
				break;
			
			case 1:
				// skipping set variable
				state = 0;
				break;
			case 2:
				// skipping loading variables
				state = 0;
				break;
			case 3:
				// skipping get config
				state = 0;
				break;
			case 4:
				// skipping generate doc
				state = 0;
				break;
			case 5:
				// getting the share data path
				shared_data_dir_hint = *arg;
				state = 0;
				break;
			case 6:
				// getting the input config file format
				input_config_file_format = *arg;
				state = 0;
				break;
			case 7:
				// getting the output config file format
				output_config_file_format = *arg;
				state = 0;
				break;
			default:
				std::cerr << "Internal error while parsing command line arguments" << std::endl;
				state = -1;
				break;
		}
		if (state == -1)
                {
			arg_end = arg;
                }
	}

	if ( !has_share_data_dir_hint )
	{
		if(GetBinPath(argv[0], bin_dir, program_binary))
		{
// 			 std::cerr << "bin_dir=\"" << bin_dir << "\"" << std::endl;
// 			 std::cerr << "program_binary=\"" << program_binary << "\"" << std::endl;

			if ( GetSharePath(bin_dir, shared_data_dir) )
			{
// 				std::cerr << "shared_data_dir=\"" << shared_data_dir << "\"" << std::endl;
			}
			else
			{
// 				std::cerr << "Could not resolve share data dir path" << std::endl;
				warn_get_share_path = true;
			}
		}
		else
		{
// 			std::cerr << "Could not resolve bin and share data dir paths" << std::endl;
			warn_get_bin_path = true;
			warn_get_share_path = true;
		}
	}
	else
	{
		if ( !ResolvePath(shared_data_dir_hint, std::string(), shared_data_dir) )
		{
// 			std::cerr << "Could not resolve share data dir path" << std::endl;
			warn_get_share_path = true;
		}
		else
		{
// 			std::cerr << "Resolved data dir path: " << shared_data_dir
// 			 	<< std::endl;
		}
	}

	// parse command line arguments (second pass)
	state = 0;
	for(char **arg = argv + 1; arg < arg_end; arg++)
	{
		switch(state)
		{
			case 0:
				// Unless there's an acceptable option entry assume options parsing ends here
				state = -1;
				
				// "--" unconditionally ends option parsing afterwards
				if (strcmp(*arg, "--") == 0)
				{
					arg++;
					break;
				}
				
				for (std::vector<CommandLineOption>::const_iterator cmd_opt_itr = command_line_options.begin(),
				                                                    cmd_opt_end = command_line_options.end();
				     cmd_opt_itr != cmd_opt_end; ++cmd_opt_itr)
				{
					if(*cmd_opt_itr == *arg)
					{
						switch(cmd_opt_itr->GetShortName())
						{
							case 's':
								state = 1;
								break;
							case 'c':
								state = 2;
								break;
							case 'f':
								state = 6;
								break;
							case 'F':
								state = 7;
								break;
							case 'g':
								state = 3;
								break;
							case 'l':
								list_parms = true;
								state = 0;
								break;
							case 'v':
								enable_version = true;
								state = 0;
								break;
							case 'h':
								enable_help = true;
								state = 0;
								break;
							case 'w':
								enable_warning = true;
								if(!LoadBuiltInConfig)
								{
									std::cerr << "WARNING! No built-in parameters set loaded" << std::endl;
								}
								if(warn_get_bin_path)
								{
									std::cerr << "WARNING! Can't determine binary directory" << std::endl;
								}
								if(warn_get_share_path)
								{
									std::cerr << "WARNING! Can't determine share directory" << std::endl;
									std::cerr << "         Program binary is '" << program_binary << "'" << std::endl;
									std::cerr << "         Binary dir is     '" << bin_dir << "'" << std::endl;
								}
								state = 0;
								break;
							case 'd':
								state = 4;
								break;
							case 'p':
								state = 5;
								break;
							default:
								state = -1;
								break;
						}
					}
				}
				break;
			
			case 1:
				{
					std::string variable_name;
					
					char *p;
					for(p = *arg; *p != 0 && *p != '='; p++)
					{
						variable_name += *p;
					}
					if(*p == '=')
					{
						const char *variable_value = ++p;
						
						SetVariable(variable_name.c_str(), variable_value);
					}
					else if(enable_warning)
					{
						std::cerr << "WARNING! Ignoring " << *arg << std::endl;
					}
				}
				state = 0;
				break;
			case 2:
				if(LoadVariables(*arg, VariableBase::VAR_PARAMETER))
				{
					std::cerr << "Parameters set using file \"" << (*arg) << "\"" << std::endl;
				}
				else if(enable_warning)
				{
					std::cerr << "WARNING! Loading parameters set from file \"" << (*arg) << "\" failed" << std::endl;
				}
// 				if(LoadXmlParameters(*arg))
// 				{
// 					std::cerr << "Parameters set using file \"" << (*arg) << "\"" << std::endl;
// 				}
// 				else if(enable_warning)
// 				{
// 					std::cerr << "WARNING! Loading parameters set from file \"" << (*arg) << "\" failed" << std::endl;
// 				}
				state = 0;
				break;
			case 3:
				get_config = true;
				get_config_filename = *arg;
				state = 0;
				break;
			case 4:
				generate_doc = true;
				generate_doc_filename = *arg;
				state = 0;
				break;
			case 5:
				state = 0;
				break;
			case 6:
				state = 0;
				break;
			case 7:
				state = 0;
				break;
			default:
				std::cerr << "Internal error while parsing command line arguments" << std::endl;
				state = -1;
				break;
		}
		if (state == -1)
                {
			arg_end = arg;
                }
	}
	
	// create on the fly parameters cmd-args[*] that are the remaining parameters
	int cmd_args_dim = argv + argc - arg_end;
        cmd_args.resize(cmd_args_dim);
	param_cmd_args = new ParameterArray<std::string>("cmd-args", 0, &cmd_args[0], cmd_args_dim, "command line arguments");
	for(int i = 0; i < cmd_args_dim; i++)
	{
		(*param_cmd_args)[i] = arg_end[i];
	}
	param_cmd_args->SetVisible(false);
	param_cmd_args->SetMutable(false);
	param_cmd_args->SetSerializable(false);
	
	// Setup logger server
	unisim::kernel::logger::LoggerServer *logserv = unisim::kernel::logger::Logger::StaticServerInstance();
	logserv->Setup();
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
	
	if(var_schematic)
	{
		delete var_schematic;
	}

	if(var_description)
	{
		delete var_description;
	}

	if(param_enable_press_enter_at_exit)
	{
		delete param_enable_press_enter_at_exit;
	}
	
	std::map<std::string, ConfigFileHelper *>::iterator config_file_helper_iter;
	for(config_file_helper_iter = config_file_helpers.begin(); config_file_helper_iter != config_file_helpers.end(); config_file_helper_iter++)
	{
		ConfigFileHelper *config_file_helper = (*config_file_helper_iter).second;
		delete config_file_helper;
	}
	
#if !defined(WIN32) && !defined(_WIN32) && !defined(WIN64) && !defined(_WIN64)
	if(sig_pipe_handler)
	{
		signal(SIGPIPE, prev_sig_pipe_handler);
		sig_pipe_handler = 0;
	}
#endif
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	SetConsoleCtrlHandler(&Simulator::ConsoleCtrlHandler, FALSE);
#else
	signal(SIGINT, prev_sig_int_handler);
	sig_int_handler = 0;
#endif
	
	unisim::kernel::logger::Logger::ReleaseStaticServiceInstance();
}

void Simulator::Version(std::ostream& os) const
{
	os << program_name << " " << version << std::endl;
	os << copyright << std::endl;
	os << "License: " << license << std::endl;
	os << "Authors: " << authors << std::endl;
}

void Simulator::Help(std::ostream& os) const
{
	os << description << std::endl << std::endl;
	os << "Usage: " << program_binary << " [<options>] [...]" << std::endl << std::endl;
	os << "Options:" << std::endl;

	std::vector<CommandLineOption>::const_iterator cmd_opt_itr;
	for(cmd_opt_itr = command_line_options.begin(); cmd_opt_itr != command_line_options.end(); cmd_opt_itr++)
	{
		os << std::endl;
		os << " --" << cmd_opt_itr->GetLongName();
		if(cmd_opt_itr->HasArgument())
		{
			os << " <" << cmd_opt_itr->GetArgumentDescription() << ">";
		}
		os << std::endl;
		os << " -" << cmd_opt_itr->GetShortName();
		if(cmd_opt_itr->HasArgument())
		{
			os << " <" << cmd_opt_itr->GetArgumentDescription() << ">";
		}
		os << std::endl;
		os << "            " << cmd_opt_itr->GetOptionDescription();
		os << std::endl;
	}
}

void Simulator::Register(Object *object)
{
	if(objects.find(object->GetName()) != objects.end())
	{
		std::cerr << "ERROR! Object \"" << object->GetName() << "\" already exists" << std::endl;
		Dump(std::cerr);
		exit(1);
	}
	
	objects[object->GetName()] = object;
}

void Simulator::Register(VariableBase *variable)
{
	if(variables.find(variable->GetName()) != variables.end())
	{
		std::cerr << "ERROR! Variable \"" << variable->GetName() << "\" already exists" << std::endl;
		exit(1);
	}

	variables[variable->GetName()] = variable;
}

void Simulator::Initialize(VariableBase *variable)
{
	// initialize variable from command line
	std::map<std::string, std::string>::iterator set_var_iter = set_vars.find(variable->GetName());
	
	if(set_var_iter != set_vars.end())
	{
		const char *value = (*set_var_iter).second.c_str();
#ifdef DEBUG_VARIABLES
		std::cerr << variable->GetName() << " <- \"" << value << "\"" << std::endl;
#endif
		*variable = value;
		variable->SetModified(false); // cancel modification flag
		set_vars.erase(set_var_iter);
	}
}

void Simulator::Register(ServiceImportBase *srv_import)
{
	if(imports.find(srv_import->GetName()) != imports.end())
	{
		std::cerr << "ERROR! Import \"" << srv_import->GetName() << "\" already exists" << std::endl;
		exit(1);
	}

	imports[srv_import->GetName()] = srv_import;
}

void Simulator::Register(ServiceExportBase *srv_export)
{
	if(exports.find(srv_export->GetName()) != exports.end())
	{
		std::cerr << "ERROR! Export \"" << srv_export->GetName() << "\" already exists" << std::endl;
		exit(1);
	}

	exports[srv_export->GetName()] = srv_export;
}

void Simulator::Register(ConfigFileHelper *config_file_helper)
{
	if(config_file_helpers.find(config_file_helper->GetName()) != config_file_helpers.end())
	{
		std::cerr << "ERROR! Configuration file helper \"" << config_file_helper->GetName() << "\" already exists" << std::endl;
		exit(1);
	}

	config_file_helpers[config_file_helper->GetName()] = config_file_helper;
}

void Simulator::Unregister(Object *object)
{
	std::map<std::string, Object *>::iterator object_iter;
	object_iter = objects.find(object->GetName());
	if(object_iter != objects.end())
	{
		objects.erase(object_iter);
	}
}

void Simulator::Unregister(VariableBase *variable)
{
	std::map<std::string, VariableBase *>::iterator variable_iter;
	variable_iter = variables.find(variable->GetName());
	if(variable_iter != variables.end()) variables.erase(variable_iter);
}

void Simulator::Unregister(ServiceImportBase *srv_import)
{
	std::map<std::string, ServiceImportBase *>::iterator import_iter;
	import_iter = imports.find(srv_import->GetName());
	if(import_iter != imports.end()) imports.erase(import_iter);
}

void Simulator::Unregister(ServiceExportBase *srv_export)
{
	std::map<std::string, ServiceExportBase *>::iterator export_iter;
	export_iter = exports.find(srv_export->GetName());
	if(export_iter != exports.end()) exports.erase(export_iter);
}

void Simulator::Dump(std::ostream& os)
{
	os << "OBJECTS:" << std::endl;

	std::map<std::string, Object *>::iterator object_iter;

	for(object_iter = objects.begin(); object_iter != objects.end(); object_iter++)
	{
		os << (*object_iter).second->GetName() << std::endl;
	}
	os << std::endl;

	os << "PARAMETERS:" << std::endl;
	std::map<std::string, VariableBase *>::iterator variable_iter;
	
	for(variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		os << (*variable_iter).second->GetName() << " = \"" << ((std::string) *(*variable_iter).second) << "\"" << std::endl;
	}
	os << std::endl;

	os << "IMPORTS:" << std::endl;

	std::map<std::string, ServiceImportBase *>::iterator import_iter;

	for(import_iter = imports.begin(); import_iter != imports.end(); import_iter++)
	{
		Object *service = (*import_iter).second->GetService();
		os << (*import_iter).second->GetName();
		if(service) os << " (to " << service->GetName() << ")";
		os << std::endl;
	}
	os << std::endl;

	std::cerr << "EXPORTS:" << std::endl;

	std::map<std::string, ServiceExportBase *>::iterator export_iter;

	for(export_iter = exports.begin(); export_iter != exports.end(); export_iter++)
	{
		Object *client = (*export_iter).second->GetClient();
		os << (*export_iter).second->GetName();
		if(client) os << " (from " << client->GetName() << ")";
		os << std::endl;
	}
	os << std::endl;

	std::cerr << "CONNECTIONS:" << std::endl;
	for(import_iter = imports.begin(); import_iter != imports.end(); import_iter++)
	{
		(*import_iter).second->Dump(os);
	}
	for(export_iter = exports.begin(); export_iter != exports.end(); export_iter++)
	{
		(*export_iter).second->Dump(os);
	}
}

void Simulator::DumpVariables(std::ostream &os, VariableBase::Type filter_type) {
/*	switch(filter_type)
	{
		case VariableBase::VAR_VOID:
			os << "Variables:" << std::endl;
			break;
		case VariableBase::VAR_ARRAY:
			os << "Arrays of variables:" << std::endl;
			break;
		case VariableBase::VAR_PARAMETER:
			os << "Parameters:" << std::endl;
			break;
		case VariableBase::VAR_STATISTIC:
			os << "Statistics:" << std::endl;
			break;
		case VariableBase::VAR_REGISTER:
			os << "Registers:" << std::endl;
			break;
	}*/
	
	unsigned int max_name_column_width = 64;
	unsigned int max_value_column_width = 32;
	unsigned int max_variable_name_length = 0;
	unsigned int max_variable_value_length = 0;
	
	std::map<std::string, VariableBase *>::iterator variable_iter;

	for(variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		VariableBase *var = (*variable_iter).second;
		VariableBase::Type var_type = var->GetType();
		if(var->IsVisible() && (filter_type == VariableBase::VAR_VOID || var_type == filter_type))
		{
			const char *name = var->GetName();
			std::string value = (std::string) *var;
			
			unsigned int variable_name_length = strlen(name);
			unsigned int variable_value_length = value.length();
			
			if(variable_name_length > max_variable_name_length) max_variable_name_length = variable_name_length;
			if(variable_value_length > max_variable_value_length) max_variable_value_length = variable_value_length;
		}
	}
	
	if(max_variable_name_length < max_name_column_width) max_name_column_width = max_variable_name_length;
	if(max_variable_value_length < max_value_column_width) max_value_column_width = max_variable_value_length;

	for(variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		VariableBase *var = (*variable_iter).second;
		VariableBase::Type var_type = var->GetType();
		if(var->IsVisible() && (filter_type == VariableBase::VAR_VOID || var_type == filter_type))
		{
			std::stringstream sstr_name;
			std::stringstream sstr_value;
			const char *name = var->GetName();
			std::string value = (std::string) *var;
//			const char *dt = var->GetDataTypeName();
			const char *desc = (*variable_iter).second->GetDescription();
			
			if(strlen(name) <= max_name_column_width)
			{
				sstr_name.width(max_name_column_width);
			}
			sstr_name.setf(std::ios::left);
			sstr_name << name;
			if(value.length() <= max_value_column_width)
			{
				sstr_value.width(max_value_column_width);
			}
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
			
			os << std::endl;
		}
	}
}

void Simulator::DumpStatistics(std::ostream &os)
{
	DumpVariables(os, VariableBase::VAR_STATISTIC);
}

void Simulator::DumpParameters(std::ostream &os)
{
	DumpVariables(os, VariableBase::VAR_PARAMETER);
}

void Simulator::DumpRegisters(std::ostream &os)
{
	DumpVariables(os, VariableBase::VAR_REGISTER);
}

void Simulator::DumpFormulas(std::ostream &os)
{
	DumpVariables(os, VariableBase::VAR_FORMULA);
}

bool Simulator::LoadVariables(const char *filename, VariableBase::Type type)
{
	std::map<std::string, ConfigFileHelper *>::iterator config_file_helper_it = config_file_helpers.find(input_config_file_format);
	
	if(config_file_helper_it != config_file_helpers.end())
	{
		 ConfigFileHelper *config_file_helper = (*config_file_helper_it).second;
		 
		 return config_file_helper->LoadVariables(filename, type);
	}
	
	return false;
}

bool Simulator::SaveVariables(const char *filename, VariableBase::Type type)
{
	std::map<std::string, ConfigFileHelper *>::iterator config_file_helper_it = config_file_helpers.find(output_config_file_format);
	
	if(config_file_helper_it != config_file_helpers.end())
	{
		 ConfigFileHelper *config_file_helper = (*config_file_helper_it).second;
		 
		 return config_file_helper->SaveVariables(filename, type);
	}
	
	return false;
}

struct MyVertexProperty
{
	ServiceExportBase *srv_export;
};

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, MyVertexProperty> DependencyGraph;
typedef boost::graph_traits<DependencyGraph>::vertex_descriptor Vertex;

struct CycleDetector : public boost::dfs_visitor<>
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
			std::cerr << "Latex documentation generation using file \"" << generate_doc_filename << "\"" << std::endl;
			std::ofstream stream(generate_doc_filename.c_str(), std::ofstream::out);
			GenerateLatexDocumentation(stream);
		}
		return ST_OK_DONT_START;
	}
	if(enable_version)
	{
		Version(std::cerr);
		return ST_OK_DONT_START;
	}
	if(enable_help)
	{
		Help(std::cerr);
		return ST_OK_DONT_START;
	}
	
	if(list_parms)
	{
		std::cerr << "Listing parameters..." << std::endl;
		DumpVariables(std::cerr, unisim::kernel::service::VariableBase::VAR_PARAMETER);
		std::cerr << "Aborting simulation" << std::endl;
		return ST_OK_DONT_START;
	}

	if(!get_config_filename.empty())
	{
		if(SaveVariables(get_config_filename.c_str(), VariableBase::VAR_PARAMETER))
		{
			std::cerr << "Parameters saved on file \"" << get_config_filename << "\"" << std::endl;
		}
		else
		{
			std::cerr << "WARNING! Saving parameters set to file \"" << get_config_filename << "\" failed" << std::endl;
		}
		std::cerr << "Aborting simulation" << std::endl;
		return ST_OK_DONT_START;
	}

	std::map<std::string, ServiceExportBase *>::iterator export_iter;
	if(enable_warning)
	{
		for(export_iter = exports.begin(); export_iter != exports.end(); export_iter++)
		{
			ServiceExportBase *srv_export = (*export_iter).second;
			if(!srv_export->GetClient())
			{
				std::cerr << "WARNING! " << srv_export->GetName() << " is unused" << std::endl;
			}
		}
	}

	// Build a dependency graph of exports
	std::map<ServiceExportBase *, unsigned int> id_lookup;
	DependencyGraph dependency_graph(exports.size());
	
	unsigned int id = 0;

	for(export_iter = exports.begin(); export_iter != exports.end(); export_iter++, id++)
	{
//		std::cerr << "Export: " << (*export_iter).second->GetName() << std::endl;
		dependency_graph[id].srv_export = (*export_iter).second;
		id_lookup[(*export_iter).second] = id;
	}

	for(export_iter = exports.begin(); export_iter != exports.end(); export_iter++, id++)
	{
		ServiceExportBase *srv_export = (*export_iter).second;
		std::list<ServiceImportBase *>& setup_dependencies = srv_export->GetSetupDependencies();
		std::list<ServiceImportBase *>::iterator import_iter;

		for(import_iter = setup_dependencies.begin(); import_iter != setup_dependencies.end(); import_iter++)
		{
			ServiceExportBase *peer_export = (*import_iter)->GetServiceExport();
			if(peer_export)
			{
				add_edge(id_lookup[peer_export], id_lookup[(*export_iter).second], dependency_graph);
//				std::cerr << peer_export->GetID() << ":" << peer_export->GetName() << "->" 
//					<< (*export_iter).second->GetID() << ":" << (*export_iter).second->GetName() << std::endl;
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
		std::cerr << "Simulator: ERROR! cyclic setup dependency graph" << std::endl;
		return ST_ERROR;
	}

	// Compute a topological order of methods "Setup"
	typedef std::list<Vertex> SetupOrder;
	SetupOrder setup_order;
	topological_sort(dependency_graph, std::front_inserter(setup_order));

	SetupStatus status = ST_OK_TO_START;
	
	// Call all methods "BeginSetup()"
	std::map<std::string, Object *>::iterator object_iter;
	for(object_iter = objects.begin(); object_iter != objects.end(); object_iter++)
	{
		Object *object = (*object_iter).second;
		if(!object->BeginSetup())
		{
			std::cerr << "Simulator: " << object->GetName() << " beginning of setup failed" << std::endl;
			status = ST_ERROR;
			break;
		}
	}
	
	if(status != ST_ERROR)
	{
		// Call methods "Setup(export)" in a topological order
		std::list<Vertex>::iterator vertex_iter;
		for(vertex_iter = setup_order.begin(); vertex_iter != setup_order.end(); vertex_iter++)
		{
			ServiceExportBase *srv_export = dependency_graph[*vertex_iter].srv_export;
			Object *object = srv_export->GetService();

			if(object)
			{
//				std::cerr << "Simulator:" << object->GetName() << "::Setup(" << srv_export->GetName() << ")" << std::endl;
				if(!object->Setup(srv_export))
				{
					std::cerr << "Simulator: " << srv_export->GetName() << " setup failed" << std::endl;
					status = ST_ERROR;
					break;
				}
			}
		}
	}

	if(status != ST_ERROR)
	{
		// Call all methods "EndSetup()"
		for(object_iter = objects.begin(); object_iter != objects.end(); object_iter++)
		{
			Object *object = (*object_iter).second;
			if(!object->EndSetup())
			{
				std::cerr << "Simulator: " << object->GetName() << " end of setup failed" << std::endl;
				status = ST_ERROR;
				break;
			}
		}
	}

	if(enable_warning)
	{
		// display a warning if some variable values are unused
		std::map<std::string, std::string>::iterator set_var_iter;
		
		for(set_var_iter = set_vars.begin(); set_var_iter != set_vars.end(); set_var_iter++)
		{
			std::cerr << "WARNING! value \"" << (*set_var_iter).second << "\" for variable \"" << (*set_var_iter).first << "\" is unused." << std::endl;
		}
	}

	return status;
}

void Simulator::Stop(Object *object, int exit_status, bool asynchronous)
{
	std::cerr << "WARNING! Stop is not cleanly implemented" << std::endl;
	exit(exit_status);
}

const VariableBase *Simulator::FindVariable(const char *name, VariableBase::Type type) const
{
	std::map<std::string, VariableBase *>::const_iterator variable_iter;
	
	variable_iter = variables.find(name);
	
	if(variable_iter != variables.end() && (type == VariableBase::VAR_VOID || (*variable_iter).second->GetType() == type)) return (*variable_iter).second;
	
	return void_variable;
}

VariableBase *Simulator::FindVariable(const char *name, VariableBase::Type type)
{
	std::map<std::string, VariableBase *>::iterator variable_iter;
	
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

void Simulator::GetVariables(std::list<VariableBase *>& lst, VariableBase::Type type)
{
	std::map<std::string, VariableBase *>::iterator variable_iter;

	lst.clear();
	
	for(variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		if((*variable_iter).second->IsVisible() && (type == VariableBase::VAR_VOID || (*variable_iter).second->GetType() == type))
		{
			lst.push_back((*variable_iter).second);
		}
	}
}

void Simulator::GetRootVariables(std::list<VariableBase *>& lst, VariableBase::Type type)
{
	std::map<std::string, VariableBase *>::iterator variable_iter;

	lst.clear();
	
	for(variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		VariableBase *var = (*variable_iter).second;
		if(!var->GetOwner() && var->IsVisible() && (type == VariableBase::VAR_VOID || var->GetType() == type))
		{
			lst.push_back((*variable_iter).second);
		}
	}
}

void Simulator::GetArrays(std::list<VariableBase *>& lst)
{
	GetVariables(lst, VariableBase::VAR_ARRAY);
}

void Simulator::GetParameters(std::list<VariableBase *>& lst)
{
	GetVariables(lst, VariableBase::VAR_PARAMETER);
}

void Simulator::GetRegisters(std::list<VariableBase *>& lst)
{
	GetVariables(lst, VariableBase::VAR_REGISTER);
}

void Simulator::GetSignals(std::list<VariableBase *>& lst)
{
	GetVariables(lst, VariableBase::VAR_SIGNAL);
}

void Simulator::GetStatistics(std::list<VariableBase *>& lst)
{
	GetVariables(lst, VariableBase::VAR_STATISTIC);
}

void Simulator::GetFormulas(std::list<VariableBase *>& lst)
{
	GetVariables(lst, VariableBase::VAR_FORMULA);
}

void Simulator::GetObjects(std::list<Object *>& lst) const
{
	std::map<std::string, Object *>::const_iterator object_iter;

	for(object_iter = objects.begin(); object_iter != objects.end(); object_iter++)
	{
		if((*object_iter).second)
		{
			lst.push_back((*object_iter).second);
		}
	}
}

void Simulator::GetRootObjects(std::list<Object *>& lst) const
{
	std::map<std::string, Object *>::const_iterator object_iter;

	for(object_iter = objects.begin(); object_iter != objects.end(); object_iter++)
	{
		if((*object_iter).second)
		{
			if(!(*object_iter).second->GetParent())
			{
				// std::cerr << "root: " << (*object_iter).second->GetName() << std::endl;
				lst.push_back((*object_iter).second);
			}
		}
	}
}

Object *Simulator::FindObject(const char *name) const
{
	std::map<std::string, Object *>::const_iterator object_iter = objects.find(name);
	return (object_iter != objects.end()) ? (*object_iter).second : 0;
}

#if defined(__APPLE_CC__) || defined(linux) || defined(__linux) || defined(__linux__)
void FindMyself()
{
	// stupid method to find the path to the executable/library using the dladdr
	//   function under apple and linux
}
#endif

bool Simulator::GetExecutablePath(const char *argv0, std::string& out_executable_path) const
{
#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__APPLE_CC__)
	Dl_info info;
	if ( dladdr((void *)unisim::kernel::service::FindMyself, &info) != 0 )
	{
//		char bin_path_buf[PATH_MAX + 1];
		char bin_path_buf[FILENAME_MAX + 1];
		char *bin_path_pointer = realpath(info.dli_fname, bin_path_buf);
		if(bin_path_pointer == bin_path_buf)
		{
			out_executable_path = std::string(bin_path_buf);
			return true;
		}
	}
#elif defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	char bin_path_buf[PATH_MAX + 1];
	DWORD bin_path_length;
	bin_path_length = GetModuleFileName(NULL, bin_path_buf, sizeof(bin_path_buf));
	if(bin_path_length > 0)
	{
		bin_path_buf[bin_path_length] = 0;
		out_executable_path = std::string(bin_path_buf);
		return true;
	}
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
		
//		char cwd_path_buf[PATH_MAX];
		char cwd_path_buf[FILENAME_MAX];
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
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
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

//#define DEBUG_SEARCH_SHARED_DATA_FILE

bool Simulator::GetSharePath(const std::string& bin_dir, std::string& out_share_dir) const
{
	return ResolvePath(bin_dir, std::string(BIN_TO_SHARED_DATA_PATH), out_share_dir);
}

const std::string Simulator::GetSharedDataDirectory() const
{
	return shared_data_dir;
}

std::string Simulator::SearchSharedDataFile(const char *filename) const
{
	std::string s(filename);
	if(!s.empty())
	{
#ifdef DEBUG_SEARCH_SHARED_DATA_FILE
		std::cerr << "SearchSharedDataFile: Trying \"" << s << "\"";
#endif
		if(access(s.c_str(), F_OK) == 0)
		{
#ifdef DEBUG_SEARCH_SHARED_DATA_FILE
			std::cerr << "...found" << std::endl;
#endif
			return s;
		}
#ifdef DEBUG_SEARCH_SHARED_DATA_FILE
		std::cerr << "...not found" << std::endl;
#endif

		std::stringstream sstr;
		sstr << shared_data_dir << "/" << filename;
		s = sstr.str();
#ifdef DEBUG_SEARCH_SHARED_DATA_FILE
		std::cerr << "SearchSharedDataFile: Trying \"" << s << "\"";
#endif
		if(access(s.c_str(), F_OK) == 0)
		{
#ifdef DEBUG_SEARCH_SHARED_DATA_FILE
			std::cerr << "...found" << std::endl;
#endif
			return s;
		}
		
#ifdef DEBUG_SEARCH_SHARED_DATA_FILE
		std::cerr << "...not found" << std::endl;
#endif
	}
	return std::string(filename);
}

std::vector<std::string> const&
Simulator::GetCmdArgs() const
{
	return cmd_args;
}

template <typename T>
T Simulator::GetVariable(const char *variable_name, const T *t) const
{
	return (T) *FindVariable(variable_name);
}

template bool Simulator::GetVariable(const char *, const bool *) const;
template signed char Simulator::GetVariable(const char *, const signed char *) const;
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
template std::string Simulator::GetVariable(const char *, const std::string *) const;

void Simulator::SetVariable(const char *variable_name, const char *variable_value)
{
	VariableBase *variable = FindVariable(variable_name);
	if(variable == void_variable)
	{
		std::map<std::string, std::string>::iterator set_vars_it = set_vars.find(std::string(variable_name));
		
		if(set_vars_it != set_vars.end())
		{
			set_vars.erase(set_vars_it);
		}
		set_vars.insert(std::pair<std::string, std::string>(std::string(variable_name), std::string(variable_value)));
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
	std::stringstream sstr;
	sstr << variable_value;
	SetVariable(variable_name, sstr.str().c_str());
}

void Simulator::SetVariable(const char *variable_name, long long variable_value)
{
	std::stringstream sstr;
	sstr << variable_value;
	SetVariable(variable_name, sstr.str().c_str());
}

void Simulator::SetVariable(const char *variable_name, float variable_value)
{
	SetVariable(variable_name, double(variable_value));
}

void Simulator::SetVariable(const char *variable_name, double variable_value)
{
	std::stringstream sstr;
	sstr << variable_value;
	SetVariable(variable_name, sstr.str().c_str());
}

void Simulator::GenerateLatexDocumentation(std::ostream& os) const
{
	std::map<std::string, Object *>::const_iterator object_iter;
	
	os << "This documentation has been automatically generated from the simulator \\texttt{" << string_to_latex(program_name.c_str()) << "} version " << string_to_latex(version.c_str()) << " on " << string_to_latex(__DATE__) << "." << std::endl;

	os << "\\subsection{Introduction}" << std::endl;
	os << string_to_latex(description.c_str()) << ".\\\\" << std::endl;
	os << "Section \\ref{" << program_name << "_licensing} gives licensing informations about the simulator." << std::endl;
	os << "Section \\ref{" << program_name << "_simulated_configuration} shows the set of modules and services that compose the simulator." << std::endl;
	os << "Section \\ref{" << program_name << "_using} shows how to invoke the simulator at the command line prompt." << std::endl;
	os << "Section \\ref{" << program_name << "_configuration} gives the simulator parameters." << std::endl;
	os << "Section \\ref{" << program_name << "_statistics} gives the simulator statistic counters." << std::endl;
	os << "Section \\ref{" << program_name << "_formulas} gives the simulator statistic formulas." << std::endl;

	os << "\\subsection{Licensing}" << std::endl;
	os << "\\label{" << program_name << "_licensing}" << std::endl;
	os << string_to_latex(program_name.c_str()) << " " << string_to_latex(version.c_str()) << "\\\\" << std::endl;
	os << string_to_latex(copyright.c_str()) << "\\\\" << std::endl;
	os << "License: " << string_to_latex(license.c_str()) << "\\\\" << std::endl;
	os << "Authors: " << string_to_latex(authors.c_str()) << "\\\\" << std::endl;
	
	os << "\\subsection{Simulated configuration}" << std::endl;
	os << "\\label{" << program_name << "_simulated_configuration}" << std::endl;
	
	if(!schematic.empty())
	{
		os << "\\begin{figure}[!ht]" << std::endl;
		os << "\t\\begin{center}" << std::endl;
		os << "\t\t\\includegraphics[width=\\textwidth]{" << schematic << "}" << std::endl;
		os << "\t\\end{center}" << std::endl;
		os << "\t\\caption{" << program_name << " simulator schematic.}" << std::endl;
		os << "\\end{figure}" << std::endl;
	}
	
	os << "\\noindent The " << string_to_latex(program_name.c_str()) << " simulator is composed of the following modules and services:" << std::endl;
	os << "\\begin{itemize}\\addtolength{\\itemsep}{-0.40\\baselineskip}" << std::endl;
	for(object_iter = objects.begin(); object_iter != objects.end(); object_iter++)
	{
		const char *obj_description = (*object_iter).second->GetDescription();
		os << "\\item \\textbf{" << string_to_latex((*object_iter).second->GetName()) << "}";
		if(*obj_description != 0) // not empty description
		{
			os << ": " << string_to_latex(obj_description);
		}
		os << std::endl;
	}
	os << "\\end{itemize}" << std::endl;

	os << "\\subsection{Using the " << string_to_latex(program_name.c_str()) << " simulator}" << std::endl;
	os << "\\label{" << program_name << "_using}" << std::endl;
	os << "The " << string_to_latex(program_name.c_str()) << " simulator has the following command line options:\\\\" << std::endl;
	os << "~\\\\" << std::endl;
	os << "\\noindent Usage: \\texttt{" << program_binary << " [<options>] [...]}" << std::endl << std::endl;
	os << "\\noindent Options:" << std::endl;
	os << "\\begin{itemize}" << std::endl;

	std::vector<CommandLineOption>::const_iterator cmd_opt_itr;
	for(cmd_opt_itr = command_line_options.begin(); cmd_opt_itr != command_line_options.end(); cmd_opt_itr++)
	{
		os << "\\item \\texttt{";
		os << "--" << string_to_latex(cmd_opt_itr->GetLongName());
		if(cmd_opt_itr->HasArgument())
		{
			os << " $<$" << string_to_latex(cmd_opt_itr->GetArgumentDescription()) << "$>$";
		}
		os << " or -" << cmd_opt_itr->GetShortName();
		if(cmd_opt_itr->HasArgument())
		{
			os << " $<$" << string_to_latex(cmd_opt_itr->GetArgumentDescription()) << "$>$";
		}
		os << "}: " << string_to_latex(cmd_opt_itr->GetOptionDescription());
		os << "" << std::endl;
	}
	os << "\\end{itemize}" << std::endl;

	
	// 	std::stringstream sstr_version;
// 	Version(sstr_version);
// 	os << "\\subsection{Version}" << std::endl;
// 	os << string_to_latex(sstr_version.str().c_str()) << std::endl;
// 	
// 	std::stringstream sstr_help;
// 	Help(sstr_help);
// 	os << "\\subsection{Usage}" << std::endl;
// 	os << string_to_latex(sstr_help.str().c_str(), 80, "texttt") << std::endl;

	std::map<std::string, VariableBase *>::const_iterator variable_iter;
	bool header_printed = false;
	
	//----------------------- Configuration -----------------------
	os << "\\subsection{Configuration}" << std::endl;
	os << "\\label{" << program_name << "_configuration}" << std::endl;
	os << "Simulator configuration (see below) can be modified using command line Options \\texttt{--set $<$param=value$>$} or \\texttt{--config $<$config file$>$}.\\\\" << std::endl;
	os << "~\\\\" << std::endl;
	
	os << "\\tablehead{\\hline}" << std::endl;
	os << "\\tabletail{\\hline}" << std::endl;
	os << "\\begin{supertabular}{|p{7.5cm}|p{7.5cm}|}" << std::endl;
	//os << "\\hline" << std::endl;

	header_printed = false;
	for(variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		VariableBase *variable = (*variable_iter).second;
		if((variable->GetType() == VariableBase::VAR_PARAMETER) && !variable->GetOwner() && variable->IsVisible())
		{
			if(!header_printed)
			{
				os << "\\multicolumn{2}{|l|}{\\textbf{\\Large Global}}\\\\" << std::endl;
				os << "\\hline" << std::endl;
				header_printed = true;
			}
			variable->GenerateLatexDocumentation(os);
		}
	}

	if(header_printed) os << "\\hline" << std::endl;

	for(object_iter = objects.begin(); object_iter != objects.end(); object_iter++)
	{
		(*object_iter).second->GenerateLatexDocumentation(os, VariableBase::VAR_PARAMETER);
	}

	//os << "\\hline" << std::endl;
	os << "\\end{supertabular}" << std::endl;

	//----------------------- Statistics -----------------------
	os << "\\subsection{Statistics}" << std::endl;
	os << "\\label{" << program_name << "_statistics}" << std::endl;
	os << "Simulation statistic counters are listed below:\\\\" << std::endl;
	os << "~\\\\" << std::endl;
	os << "\\tablehead{\\hline}" << std::endl;
	os << "\\tabletail{\\hline}" << std::endl;
	os << "\\begin{supertabular}{|p{7.5cm}|p{7.5cm}|}" << std::endl;
	//os << "\\hline" << std::endl;

	header_printed = false;
	// Global statistics
	
	for(variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		VariableBase *variable = (*variable_iter).second;
		if((variable->GetType() == VariableBase::VAR_STATISTIC) && !variable->GetOwner() && variable->IsVisible())
		{
			if(!header_printed)
			{
				os << "\\multicolumn{2}{|l|}{\\textbf{\\Large Global}}\\\\" << std::endl;
				os << "\\hline" << std::endl;
				header_printed = true;
			}
			variable->GenerateLatexDocumentation(os);
		}
	}

	if(header_printed) os << "\\hline" << std::endl;

	for(object_iter = objects.begin(); object_iter != objects.end(); object_iter++)
	{
		(*object_iter).second->GenerateLatexDocumentation(os, VariableBase::VAR_STATISTIC);
	}

	//os << "\\hline" << std::endl;
	os << "\\end{supertabular}" << std::endl;

	//----------------------- Formulas -----------------------
	os << "\\subsection{Formulas}" << std::endl;
	os << "\\label{" << program_name << "_formulas}" << std::endl;
	os << "Simulation statistic formulas are listed below:\\\\" << std::endl;
	os << "~\\\\" << std::endl;
	os << "\\tablehead{\\hline}" << std::endl;
	os << "\\tabletail{\\hline}" << std::endl;
	os << "\\begin{supertabular}{|p{7.5cm}|p{7.5cm}|}" << std::endl;
	//os << "\\hline" << std::endl;

	// Global formulas
	header_printed = false;
	
	for(variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		VariableBase *variable = (*variable_iter).second;
		if((variable->GetType() == VariableBase::VAR_FORMULA) && !variable->GetOwner() && variable->IsVisible())
		{
			if(!header_printed)
			{
				os << "\\multicolumn{2}{|l|}{\\textbf{\\Large Global}}\\\\" << std::endl;
				os << "\\hline" << std::endl;
				header_printed = true;
			}
			variable->GenerateLatexDocumentation(os);
		}
	}

	if(header_printed) os << "\\hline" << std::endl;

	for(object_iter = objects.begin(); object_iter != objects.end(); object_iter++)
	{
		(*object_iter).second->GenerateLatexDocumentation(os, VariableBase::VAR_FORMULA);
	}

	//os << "\\hline" << std::endl;
	os << "\\end{supertabular}" << std::endl;
}

bool Simulator::IsWarningEnabled() const
{
	return enable_warning;
}

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
BOOL WINAPI Simulator::ConsoleCtrlHandler(DWORD dwCtrlType)
{
	bool interrupted = false;
	switch(dwCtrlType)
	{
		case CTRL_C_EVENT:
			std::cerr << "Interrupted by Ctrl-C" << std::endl;
			interrupted = true;
			break;
		case CTRL_BREAK_EVENT:
			std::cerr << "Interrupted by Ctrl-Break" << std::endl;
			interrupted = true;
			break;
		case CTRL_CLOSE_EVENT:
			std::cerr << "Interrupted by a console close" << std::endl;
			interrupted = true;
			break;
		case CTRL_LOGOFF_EVENT:
			std::cerr << "Interrupted because of logoff" << std::endl;
			interrupted = true;
			break;
		case CTRL_SHUTDOWN_EVENT:
			std::cerr << "Interrupted because of shutdown" << std::endl;
			interrupted = true;
			break;
	}
	
	if(interrupted)
	{
		simulator->BroadcastSigInt();
	}
	
	return interrupted ? TRUE : FALSE;
}
#else
void Simulator::SigIntHandler(int signum)
{
	std::cerr << "Interrupted by Ctrl-C or SIGINT signal" << std::endl;
	simulator->BroadcastSigInt();
}
#endif

#if !defined(WIN32) && !defined(_WIN32) && !defined(WIN64) && !defined(_WIN64)
void Simulator::SigPipeHandler(int signum)
{
	/* do nothing */
}
#endif

void Simulator::BroadcastSigInt()
{
	std::map<std::string, Object *>::iterator object_iter;
	for(object_iter = objects.begin(); object_iter != objects.end(); object_iter++)
	{
		Object *object = (*object_iter).second;
		object->SigInt();
	}
}


} // end of namespace service
} // end of namespace kernel
} // end of namespace unisim
