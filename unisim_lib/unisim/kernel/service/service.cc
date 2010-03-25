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
#include "unisim/kernel/logger/logger_server.hh"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <stdlib.h>

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
	free(storage);
}

void operator delete[](void *storage, std::size_t size)
{
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
	
VariableBase::VariableBase(const char *_name, Object *_owner, Type _type, const char *_description) :
	name(_owner ? _owner->GetName() + string(".") + string(_name) : _name),
	var_name(_name),
	owner(_owner),
	container(0),
	description(_description ? _description : ""),
	enumerated_values(),
	type(_type),
	fmt(FMT_DEFAULT)
{
	_owner->Add(*this);
	ServiceManager::Register(this);
}

VariableBase::VariableBase(const char *_name, VariableBase *_container, Type _type, const char *_description) :
	name(_container ? _container->GetName() + string(".") + string(_name) : _name), 
	var_name(_container ? _container->GetVarName() + string(".") + string(_name) : _name),
	owner(0),
	container(_container),
	description(_description ? _description : ""),
	enumerated_values(),
	type(_type),
	fmt(FMT_DEFAULT)
{
	ServiceManager::Register(this);
}

VariableBase::VariableBase() :
	name(), owner(0), container(0), description(), type(VAR_VOID), fmt(FMT_DEFAULT)
{
}

VariableBase::~VariableBase()
{
	if(owner) owner->Remove(*this);
	ServiceManager::Unregister(this);
}

Object *VariableBase::GetOwner() const
{
	return owner;
}

VariableBase *VariableBase::GetObject() const
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

template <class TYPE> VariableBase& Variable<TYPE>::operator = (bool value) { *storage = value ? 1 : 0; return *this; }
template <class TYPE> VariableBase& Variable<TYPE>::operator = (long long value) { *storage = value;	return *this; }
template <class TYPE> VariableBase& Variable<TYPE>::operator = (unsigned long long value) { *storage = value;	return *this; }
template <class TYPE> VariableBase& Variable<TYPE>::operator = (double value) { *storage = (TYPE) value; return *this; }

//=============================================================================
//=                           VariableArray<TYPE>                            =
//=============================================================================

//=============================================================================
//=                             Formula<TYPE>                                 =
//=============================================================================

static unsigned int auto_formula_id = 0;
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

#if 0
template <class TYPE>
Formula<TYPE>::Formula(const char *_name, Object *_owner, const char *math_formula, const char *_description)
	: VariableBase(_name, _owner, VAR_FORMULA, _description)
	, op(OP_NOP)
{
	VariableBase *expr = 0;
	unsigned int pos = 0;
	look_ahead_token = 0;
	look_ahead_lval = 0;
	unsigned int tok = Parse(math_formula, pos, &expr);
	if(tok == TOK_EXPR)
		Initialize(OP_EVAL, expr);
}

template <class TYPE>
Formula<TYPE>::Formula(Operator _op, VariableBase *child1, VariableBase *child2, VariableBase *child3)
	: VariableBase(auto_formula_id ? auto_formula_id_string.c_str() : "0", (Object *) 0, VAR_FORMULA, 0)
	, op(_op)
{
	childs[0] = child1;
	childs[1] = child2;
	childs[2] = child3;
	stringstream sstr;
	sstr << (++auto_formula_id);
	auto_formula_id_string = sstr.str();
}

template <class TYPE>
const char *Formula<TYPE>::GetTokenName(unsigned int token) const
{
	static char token_name[2];
	
	switch(token)
	{
		
		TOK_EOF: return "end-of-file";
		TOK_ERROR: return "error";
		TOK_LTE: return "<=";
		TOK_GTE: return ">=";
		TOK_ABS: return "abs";
		TOK_MIN: return "min";
		TOK_MAX: return "max";
		TOK_IDENT: return "identifier";
		TOK_EXPR: return "expression";
		default:
			token_name[0] = token;
			token_name[1] = 0;
			return token_name;
	}
	return 0;
}

template <class TYPE>
unsigned int Formula<TYPE>::ReadToken(const char *math_formula, unsigned int& pos, unsigned int mode, VariableBase **lval)
{
	unsigned int token;
	
	if(look_ahead_token)
	{
		if(mode == GET_TOK)
		{
			*lval = look_ahead_lval;
			token = look_ahead_token;
			look_ahead_token = 0;
			return token;
		}
		if(mode == PEEK_TOK)
		{
			*lval = look_ahead_lval;
			return look_ahead_token;
		}
		return TOK_ERROR;
	}

	do
	{
		switch(math_formula[pos])
		{
			case 0:
				token = TOK_EOF;
				break;
			case '+':
			case '-':
			case '*':
			case '/':
			case '=':
			case '?':
			case ':':
			case '(':
			case ')':
			case '&':
			case '|':
			case '^':
			case '!':
				pos++;
				token = math_formula[pos];
				break;
			case '<':
				if(math_formula[pos + 1] == '=')
				{
					pos += 2;
					token = TOK_LTE;
				}
				else
				{
					pos++;
					token = '<';
				}
				break;
			case '>':
				if(math_formula[pos + 1] == '=')
				{
					pos += 2;
					token = TOK_GTE;
				}
				else
				{
					pos++;
					token = '>';
				}
				break;
			case 'a':
				if(math_formula[pos + 1] == 'b' && math_formula[pos + 2] == 's')
				{
					pos += 3;
					token = TOK_ABS;
				}
				else
				{
					token = TOK_ERROR;
				}
				break;
			case 'm':
				if(math_formula[pos + 1] == 'i' && math_formula[pos + 2] == 'n')
				{
					pos += 3;
					token = TOK_MIN;
				}
				else if(math_formula[pos + 1] == 'a' && math_formula[pos + 2] == 'x')
				{
					pos += 3;
					token = TOK_MAX;
				}
				else
				{
					token = TOK_ERROR;
				}
				break;
			case ' ':
			case '\t':
				break;
			case '$':
				if(math_formula[pos + 1] == '{')
				{
					string ident;
					int i = 2;
					while(math_formula[pos + i] && math_formula[pos + i] != '}')
					{
						ident += math_formula[pos + i];
						i++;
					}
					
					if(math_formula[pos + i] != '}') return TOK_ERROR;
					
					pos += i;
					*lval = &(*GetOwner())[ident.c_str()];
					token = TOK_IDENT;
				}
				break;
			default:
				token = TOK_ERROR;
		}
	} while(!token);
	
	if(mode == PEEK_TOK)
	{
		look_ahead_token = token; // save token
		look_ahead_lval = *lval;
		return token;
	}
	
	if(mode == GET_TOK)
	{
		return token;
	}
}

template <class TYPE>
unsigned int Formula<TYPE>::Parse(const char *math_formula, unsigned int& pos, VariableBase **lval)
{
	VariableBase *expr1 = 0;
	VariableBase *expr2 = 0;
	VariableBase *expr3 = 0;
	VariableBase *expr = 0;
	unsigned int tok = ReadToken(math_formula, pos, GET_TOK, &expr);
	
	switch(tok)
	{
		case TOK_ERROR:
			return TOK_ERROR;
		case '-':
			tok = Parse(math_formula, pos, &expr);
			if(tok != TOK_EXPR)
			{
				std::cerr << "unexpected " << GetTokenName(tok) << std::endl;
				return TOK_ERROR;
			}
			*lval = new Formula<TYPE>(OP_NEG, expr);
			return TOK_EXPR;
		case '!':
			tok = Parse(math_formula, pos, &expr);
			if(tok != TOK_EXPR)
			{
				std::cerr << "unexpected " << GetTokenName(tok) << std::endl;
				return TOK_ERROR;
			}
			*lval = new Formula<TYPE>(OP_NOT, expr);
			return TOK_EXPR;
		case '(':
			tok = Parse(math_formula, pos, &expr);
			if(tok != TOK_EXPR)
			{
				std::cerr << "unexpected " << GetTokenName(tok) << std::endl;
				return TOK_ERROR;
			}
			tok = ReadToken(math_formula, pos, GET_TOK);
			if(tok != ')') return TOK_ERROR; // should free expr
			*lval = expr;
			return TOK_EXPR;
		case TOK_ABS:
			tok = ReadToken(math_formula, pos, GET_TOK);
			if(tok != '(')
			{
				std::cerr << "unexpected " << GetTokenName(tok) << std::endl;
				return TOK_ERROR;
			}
			tok = Parse(math_formula, pos, &expr);
			if(tok != TOK_EXPR)
			{
				std::cerr << "unexpected " << GetTokenName(tok) << std::endl;
				return TOK_ERROR;
			}
			tok = ReadToken(math_formula, pos, GET_TOK);
			if(tok != ')')
			{
				std::cerr << "unexpected " << GetTokenName(tok) << std::endl;
				return TOK_ERROR;
			}
			*lval = new Formula<TYPE>(OP_ABS, expr);
			return TOK_EXPR;
		case TOK_MIN:
			tok = ReadToken(math_formula, pos, GET_TOK);
			if(tok != '(')
			{
				std::cerr << "unexpected " << GetTokenName(tok) << std::endl;
				return TOK_ERROR;
			}
			tok = Parse(math_formula, pos, &expr);
			if(tok != TOK_EXPR)
			{
				std::cerr << "unexpected " << GetTokenName(tok) << std::endl;
				return TOK_ERROR;
			}
			tok = ReadToken(math_formula, pos, GET_TOK);
			if(tok != ')')
			{
				std::cerr << "unexpected " << GetTokenName(tok) << std::endl;
				return TOK_ERROR;
			}
			*lval = new Formula<TYPE>(OP_MIN, expr);
			return TOK_EXPR;
		case TOK_MAX:
			tok = ReadToken(math_formula, pos, GET_TOK);
			if(tok != '(')
			{
				std::cerr << "unexpected " << GetTokenName(tok) << std::endl;
				return TOK_ERROR;
			}
			tok = Parse(math_formula, pos, &expr);
			if(tok != TOK_EXPR)
			{
				std::cerr << "unexpected " << GetTokenName(tok) << std::endl;
				return TOK_ERROR;
			}
			tok = ReadToken(math_formula, pos, GET_TOK);
			if(tok != ')')
			{
				std::cerr << "unexpected " << GetTokenName(tok) << std::endl;
				return TOK_ERROR;
			}
			*lval = new Formula<TYPE>(OP_MAX, expr);
			return TOK_EXPR;
		case TOK_IDENT:
		case TOK_EXPR:
			*lval = expr;
			tok = ReadToken(math_formula, pos, PEEK_TOK, &expr);
			switch(tok)
			{
				case '+':
				case '-':
				case '*':
				case '/':
				case '<':
				case TOK_LTE:
				case '>':
				case TOK_GTE:
				case '?':
				case '&':
				case '|':
				case '^':
					break;
				case TOK_EOF:
					return TOK_EXPR;
				default:
					std::cerr << "unexpected " << GetTokenName(tok) << std::endl;
					return TOK_ERROR;
			}
			return TOK_ERROR;
		default:
			std::cerr << "unexpected " << GetTokenName(tok) << std::endl;
			return TOK_ERROR;
	}
	
	tok = ReadToken(math_formula, pos, GET_TOK, &expr);
	
	Operator op;
	switch(tok)
	{
		case '+':
			op = OP_ADD;
			break;
		case '-':
			op = OP_SUB;
			break;
		case '*':
			op = OP_MUL;
			break;
		case '/':
			op = OP_DIV;
			break;
		case '<':
			op = OP_LT;
			break;
		case TOK_LTE:
			op = OP_LTE;
			break;
		case '>':
			op = OP_GT;
			break;
		case TOK_GTE:
			op = OP_GTE;
			break;
		case '?':
			op = OP_SEL;
			break;
		case '&':
			op = OP_AND;
			break;
		case '|':
			op = OP_OR;
			break;
		case '^':
			op = OP_XOR;
			break;
		default:
			std::cerr << "unexpected " << GetTokenName(tok) << std::endl;
			return TOK_ERROR;
	}
	
	tok = ReadToken(math_formula, pos, GET_TOK, &expr);

	switch(op)
	{
		case OP_ADD:
		case OP_SUB:
		case OP_MUL:
		case OP_DIV:
		case OP_LT:
		case OP_LTE:
		case OP_GT:
		case OP_GTE:
		case OP_EQ:
		case OP_SEL:
		case OP_AND:
		case OP_OR:
		case OP_XOR:
			expr2 = expr;
		default:
			std::cerr << "internal error" << std::endl;
			return TOK_ERROR;
	}

	switch(op)
	{
		case OP_ADD:
			*lval = new Formula(OP_ADD, expr1, expr2);
			return TOK_EXPR;
		case OP_SUB:
			*lval = new Formula(OP_SUB, expr1, expr2);
			return TOK_EXPR;
		case OP_MUL:
			*lval = new Formula(OP_MUL, expr1, expr2);
			return TOK_EXPR;
		case OP_DIV:
			*lval = new Formula(OP_DIV, expr1, expr2);
			return TOK_EXPR;
		case OP_LT:
			*lval = new Formula(OP_LT, expr1, expr2);
			return TOK_EXPR;
		case OP_LTE:
			*lval = new Formula(OP_LTE, expr1, expr2);
			return TOK_EXPR;
		case OP_GT:
			*lval = new Formula(OP_GT, expr1, expr2);
			return TOK_EXPR;
		case OP_GTE:
			*lval = new Formula(OP_GTE, expr1, expr2);
			return TOK_EXPR;
		case OP_EQ:
			*lval = new Formula(OP_EQ, expr1, expr2);
			return TOK_EXPR;
		case OP_SEL:
			break;
		case OP_AND:
			if(tok != TOK_EXPR)
			{
				std::cerr << "unexpected " << GetTokenName(tok) << std::endl;
				return TOK_ERROR;
			}
			*lval = new Formula(OP_AND, expr1, expr2);
			return TOK_EXPR;
		case OP_OR:
			if(tok != TOK_EXPR)
			{
				std::cerr << "unexpected " << GetTokenName(tok) << std::endl;
				return TOK_ERROR;
			}
			*lval = new Formula(OP_OR, expr1, expr2);
			return TOK_EXPR;
		case OP_XOR:
			if(tok != TOK_EXPR)
			{
				std::cerr << "unexpected " << GetTokenName(tok) << std::endl;
				return TOK_ERROR;
			}
			*lval = new Formula(OP_XOR, expr1, expr2);
			return TOK_EXPR;
		default:
			std::cerr << "internal error" << std::endl;
			return TOK_ERROR;
	}

	tok = ReadToken(math_formula, pos, GET_TOK, &expr);
	switch(op)
	{
		case OP_SEL:
			if(tok != ':')
			{
				std::cerr << "unexpected " << GetTokenName(tok) << std::endl;
				return TOK_ERROR;
			}
			break;
		default:
			std::cerr << "unexpected " << GetTokenName(tok) << std::endl;
			return TOK_ERROR;
	}

	tok = ReadToken(math_formula, pos, GET_TOK, &expr);
	switch(op)
	{
		case OP_SEL:
			if(tok != TOK_EXPR)
			{
				std::cerr << "unexpected " << GetTokenName(tok) << std::endl;
				return TOK_ERROR;
			}
			expr3 = expr;
			break;
		default:
			std::cerr << "unexpected " << GetTokenName(tok) << std::endl;
			return TOK_ERROR;
	}

	switch(op)
	{
		case OP_SEL:
			*lval = new Formula(OP_SEL, expr1, expr2, expr3);
			return TOK_EXPR;
		default:
			std::cerr << "internal error" << std::endl;
			return TOK_ERROR;
	}
	
	std::cerr << "internal error" << std::endl;
	return TOK_ERROR;
}
#endif

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
{}

template <>
const char *Variable<bool>::GetDataTypeName() const
{
	return "boolean";
}

template <>
Variable<char>::Variable(const char *_name, Object *_owner, char& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{}

template <>
const char *Variable<char>::GetDataTypeName() const
{
	return GetSignedDataTypeName<char>(); //"char";
}

template <>
Variable<short>::Variable(const char *_name, Object *_owner, short& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{}

template <>
const char *Variable<short>::GetDataTypeName() const
{
	return GetSignedDataTypeName<short>(); //"short";
}

template <>
Variable<int>::Variable(const char *_name, Object *_owner, int& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{}

template <>
const char *Variable<int>::GetDataTypeName() const
{
	return GetSignedDataTypeName<int>(); //"int";
}

template <>
Variable<long>::Variable(const char *_name, Object *_owner, long& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{}

template <>
const char *Variable<long>::GetDataTypeName() const
{
	return GetSignedDataTypeName<long>(); //"long";
}

template <>
Variable<long long>::Variable(const char *_name, Object *_owner, long long& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{}

template <>
const char *Variable<long long>::GetDataTypeName() const
{
	return GetSignedDataTypeName<long long>(); //"long long";
}


template <>
Variable<unsigned char>::Variable(const char *_name, Object *_owner, unsigned char& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{}

template <>
const char *Variable<unsigned char>::GetDataTypeName() const
{
	return GetUnsignedDataTypeName<unsigned char>(); //"unsigned char";
}

template <>
Variable<unsigned short>::Variable(const char *_name, Object *_owner, unsigned short& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{}

template <>
const char *Variable<unsigned short>::GetDataTypeName() const
{
	return GetUnsignedDataTypeName<unsigned short>(); //"unsigned short";
}

template <>
Variable<unsigned int>::Variable(const char *_name, Object *_owner, unsigned int& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{}

template <>
const char *Variable<unsigned int>::GetDataTypeName() const
{
	return GetUnsignedDataTypeName<unsigned int>(); //"unsigned int";
}

template <>
Variable<unsigned long>::Variable(const char *_name, Object *_owner, unsigned long& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{}

template <>
const char *Variable<unsigned long>::GetDataTypeName() const
{
	return GetUnsignedDataTypeName<unsigned long>(); //"unsigned long";
}

template <>
Variable<unsigned long long>::Variable(const char *_name, Object *_owner, unsigned long long& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{}

template <>
const char *Variable<unsigned long long>::GetDataTypeName() const
{
	return GetUnsignedDataTypeName<unsigned long long>(); //"unsigned long long";
}

template <> 
Variable<double>::Variable(const char *_name, Object *_owner, double& _storage, Type type, const char *_description) :
	VariableBase(_name, _owner, type, _description), storage(&_storage)
{}

template <>
const char *Variable<double>::GetDataTypeName() const
{
	return "double precision floating-point";
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
{}

template <>
const char *Variable<float>::GetDataTypeName() const
{
	return "single precision floating-point";
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
{}

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

template <> VariableBase& Variable<bool>::operator = (const char *value) { *storage = (strcmp(value, "true") == 0) || (strcmp(value, "0x1") == 0) || (strcmp(value, "1") == 0); return *this; }
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

Object::Object(const char *_name, Object *_parent) :
	object_name(_parent ? (string(_parent->GetName()) + "." + _name) : _name),
	parent(_parent),
	srv_imports(),
	srv_exports(),
	leaf_objects()
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

const list<VariableBase *>& Object::GetVariables() const
{
	return variables;
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

void ServiceManager::DumpVariables(ostream &os, VariableBase::Type filter_type) {
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
		if(filter_type == VariableBase::VAR_VOID || var_type == filter_type)
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
		if(filter_type == VariableBase::VAR_VOID || var_type == filter_type)
		{
			std::stringstream sstr_name;
			std::stringstream sstr_value;
			const char *name = var->GetName();
			string value = (string) *var;
			const char *dt = var->GetDataTypeName();
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

void ServiceManager::DumpStatistics(ostream &os)
{
	DumpVariables(os, VariableBase::VAR_STATISTIC);
}

void ServiceManager::DumpParameters(ostream &os)
{
	DumpVariables(os, VariableBase::VAR_PARAMETER);
}

void ServiceManager::DumpRegisters(ostream &os)
{
	DumpVariables(os, VariableBase::VAR_REGISTER);
}

void ServiceManager::DumpFormulas(ostream &os)
{
	DumpVariables(os, VariableBase::VAR_FORMULA);
}

bool ServiceManager::XmlfyVariables(const char *filename)
{
	XMLHelper xml_helper;
	
	return xml_helper.XmlfyVariables(filename);
}

bool ServiceManager::LoadXmlVariables(const char *filename)
{
	XMLHelper xml_helper;
	
	return xml_helper.LoadXmlVariables(filename);
}

bool ServiceManager::XmlfyParameters(const char *filename)
{
	XMLHelper xml_helper;
	
	return xml_helper.XmlfyVariables(filename, VariableBase::VAR_PARAMETER);
}

bool ServiceManager::LoadXmlParameters(const char *filename)
{
	XMLHelper xml_helper;
	
	return xml_helper.LoadXmlVariables(filename, VariableBase::VAR_PARAMETER);
}

bool ServiceManager::XmlfyStatistics(const char *filename)
{
	XMLHelper xml_helper;
	
	return xml_helper.XmlfyVariables(filename, VariableBase::VAR_STATISTIC);
}

bool ServiceManager::LoadXmlStatistics(const char *filename)
{
	XMLHelper xml_helper;
	
	return xml_helper.LoadXmlVariables(filename, VariableBase::VAR_STATISTIC);
}

bool ServiceManager::XmlfyRegisters(const char *filename)
{
	XMLHelper xml_helper;
	
	return xml_helper.XmlfyVariables(filename, VariableBase::VAR_REGISTER);
}

bool ServiceManager::LoadXmlRegisters(const char *filename)
{
	XMLHelper xml_helper;
	
	return xml_helper.LoadXmlVariables(filename, VariableBase::VAR_REGISTER);
}

bool ServiceManager::XmlfyFormulas(const char *filename)
{
	XMLHelper xml_helper;
	
	return xml_helper.XmlfyVariables(filename, VariableBase::VAR_FORMULA);
}

bool ServiceManager::LoadXmlFormulas(const char *filename)
{
	XMLHelper xml_helper;
	
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

bool ServiceManager::Setup()
{
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

	unisim::kernel::logger::LoggerServer::GetInstanceWithoutCountingReference()->Setup();
	
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

VariableBase *ServiceManager::GetVariable(const char *name, VariableBase::Type type)
{
	map<const char *, VariableBase *, ltstr>::iterator variable_iter;
	
	variable_iter = variables.find(name);
	
	if(variable_iter != variables.end() && (type == VariableBase::VAR_VOID || (*variable_iter).second->GetType() == type)) return (*variable_iter).second;
	
	// cerr << "ConfigManager: unknown variable \"" << name << "\"" << endl;
	return &void_variable;
}

VariableBase *ServiceManager::GetArray(const char *name)
{
	return GetVariable(name, VariableBase::VAR_ARRAY);
}

VariableBase *ServiceManager::GetParameter(const char *name)
{
	return GetVariable(name, VariableBase::VAR_PARAMETER);
}

VariableBase *ServiceManager::GetRegister(const char *name)
{
	return GetVariable(name, VariableBase::VAR_REGISTER);
}

VariableBase *ServiceManager::GetStatistic(const char *name)
{
	return GetVariable(name, VariableBase::VAR_STATISTIC);
}

VariableBase *ServiceManager::GetFormula(const char *name)
{
	return GetVariable(name, VariableBase::VAR_FORMULA);
}

void ServiceManager::GetVariables(list<VariableBase *>& lst, VariableBase::Type type)
{
	map<const char *, VariableBase *, ltstr>::iterator variable_iter;

	lst.clear();
	
	for(variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		if(type == VariableBase::VAR_VOID || (*variable_iter).second->GetType() == type)
		{
			lst.push_back((*variable_iter).second);
		}
	}
}

void ServiceManager::GetArrays(list<VariableBase *>& lst)
{
	GetVariables(lst, VariableBase::VAR_ARRAY);
}

void ServiceManager::GetParameters(list<VariableBase *>& lst)
{
	GetVariables(lst, VariableBase::VAR_PARAMETER);
}

void ServiceManager::GetRegisters(list<VariableBase *>& lst)
{
	GetVariables(lst, VariableBase::VAR_REGISTER);
}

void ServiceManager::GetStatistics(list<VariableBase *>& lst)
{
	GetVariables(lst, VariableBase::VAR_STATISTIC);
}

void ServiceManager::GetFormulas(list<VariableBase *>& lst)
{
	GetVariables(lst, VariableBase::VAR_FORMULA);
}

void ServiceManager::GetRootObjects(list<Object *>& lst)
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


} // end of namespace service
} // end of namespace kernel
} // end of namespace unisim
