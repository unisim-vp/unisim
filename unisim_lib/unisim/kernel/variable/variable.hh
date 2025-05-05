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
 *          Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#ifndef __UNISIM_KERNEL_VARIABLE_VARIABLE_HH__
#define __UNISIM_KERNEL_VARIABLE_VARIABLE_HH__

#include <unisim/kernel/kernel.hh>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace unisim {
namespace kernel {
namespace variable {

//=============================================================================
//=                            Variable<TYPE>                                 =
//=============================================================================

template <class TYPE>
class VariableStorage
{
public:
	virtual ~VariableStorage() {}
	virtual TYPE& Get() = 0;
};

template <class TYPE>
class DirectVariableStorage : public VariableStorage<TYPE>
{
public:
	DirectVariableStorage(TYPE& _storage) : storage(_storage) {}
	
	virtual TYPE& Get() { return storage; }
private:
	TYPE& storage;
};

template <class CONTAINER, class KEY, class TYPE>
class IndirectVariableStorage : public VariableStorage<TYPE>
{
public:
	IndirectVariableStorage(CONTAINER& _container, KEY _index) : container(_container), index(_index) {}
	
	virtual TYPE& Get() { return container[index]; }
private:
	CONTAINER& container;
	KEY index;
};

template <class TYPE>
class Variable : public VariableBase
{
public:
	typedef VariableBase::Type Type;
	Variable(const char *name, Object *owner, TYPE& storage, VariableBase::Type type, const char *description = NULL);
	Variable(const char *name, VariableBase& container, VariableStorage<TYPE> *storage, VariableBase::Type type, const char *description = NULL);
	virtual ~Variable();

	virtual const char *GetDataTypeName() const;
	virtual DataType GetDataType() const;
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
	virtual const TYPE& Get() const { return storage->Get(); }

protected:
	VariableStorage<TYPE> *storage;
};

template <class TYPE>
Variable<TYPE>::Variable(const char *_name, Object *_owner, TYPE& _storage, Type _type, const char *_description) :
	VariableBase(_name, _owner, _type, _description), storage(new DirectVariableStorage<TYPE>(_storage))
{
	Initialize();
}

template <class TYPE>
Variable<TYPE>::Variable(const char *_name, VariableBase& _container, VariableStorage<TYPE> *_storage, VariableBase::Type _type, const char *_description) :
	VariableBase(_name, _container, _type, _description), storage(_storage)
{
	Initialize();
}

template <class TYPE>
Variable<TYPE>::~Variable()
{
	if(storage) delete storage;
}

template <class TYPE>
void Variable<TYPE>::Set( TYPE const& value )
{
	if(IsMutable())
	{
		TYPE& curr_value = storage->Get();
		SetModified(curr_value != value);
		curr_value = value;
		NotifyListeners();
	}
}

template <class TYPE>
class Parameter : public Variable<TYPE>
{
public:
	Parameter(const char *name, Object *owner, TYPE& storage, const char *description = NULL) : Variable<TYPE>(name, owner, storage, VariableBase::VAR_PARAMETER, description) {}
	Parameter(const char *name, VariableBase& container, VariableStorage<TYPE> *storage, const char *description = NULL) : Variable<TYPE>(name, container, storage, VariableBase::VAR_PARAMETER, description) {}
	using Variable<TYPE>::operator=;
};

template <class TYPE>
class Statistic : public Variable<TYPE>
{
public:
	Statistic(const char *name, Object *owner, TYPE& storage, const char *description = NULL) : Variable<TYPE>(name, owner, storage, VariableBase::VAR_STATISTIC, description) { VariableBase::SetFormat(unisim::kernel::VariableBase::FMT_DEC); }
	Statistic(const char *name, VariableBase& container, VariableStorage<TYPE> *storage, const char *description = NULL) : Variable<TYPE>(name, container, storage, VariableBase::VAR_STATISTIC, description) { VariableBase::SetFormat(unisim::kernel::VariableBase::FMT_DEC); }
	using Variable<TYPE>::operator=;
};

template <class TYPE>
class Register : public Variable<TYPE>
{
public:
	Register(const char *name, Object *owner, TYPE& storage, const char *description = NULL) : Variable<TYPE>(name, owner, storage, VariableBase::VAR_REGISTER, description) {}
	Register(const char *name, VariableBase& container, VariableStorage<TYPE> *storage, const char *description = NULL) : Variable<TYPE>(name, container, storage, VariableBase::VAR_REGISTER, description) {}
	using Variable<TYPE>::operator=;
};

template <class TYPE>
class Signal : public Variable<TYPE>
{
public:
	Signal(const char *name, Object *owner, TYPE& storage, const char *description = NULL) : Variable<TYPE>(name, owner, storage, VariableBase::VAR_SIGNAL, description) {}
	Signal(const char *name, VariableBase& container, VariableStorage<TYPE> *storage, const char *description = NULL) : Variable<TYPE>(name, container, storage, VariableBase::VAR_SIGNAL, description) {}
	using Variable<TYPE>::operator=;
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
	typedef FormulaOperator::Operator Operator;
	
	Formula(const char *name, Object *owner, VariableBase::Type type, FormulaOperator op, VariableBase *child1, VariableBase *child2, VariableBase *child3, const char *description = 0);
	Formula(const char *name, Object *owner, VariableBase::Type type, FormulaOperator op, VariableBase *child1, VariableBase *child2, const char *description = 0);
	Formula(const char *name, Object *owner, VariableBase::Type type, FormulaOperator op, VariableBase *child, const char *description = 0);
	
	virtual const char *GetDataTypeName() const;
	virtual DataType GetDataType() const;
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

template <class TYPE>
class StatisticFormula : public Formula<TYPE>
{
public:
	StatisticFormula(const char *name, Object *owner, FormulaOperator op, VariableBase *child1, VariableBase *child2, VariableBase *child3, const char *description = 0);
	StatisticFormula(const char *name, Object *owner, FormulaOperator op, VariableBase *child1, VariableBase *child2, const char *description = 0);
	StatisticFormula(const char *name, Object *owner, FormulaOperator op, VariableBase *child, const char *description = 0);
};

//=============================================================================
//=                           VariableArray<TYPE>                            =
//=============================================================================

template <class TYPE>
class VariableArray : public VariableBase
{
public:
	typedef VariableBase::Type Type;
	VariableArray(const char *name, Object *owner, TYPE *variables, uint64_t dim, Type type, const char *description = NULL);
	virtual ~VariableArray();

	virtual VariableBase& operator [] (uint64_t index);
	virtual const VariableBase& operator [] (uint64_t index) const;
	void SetFormat(Format fmt);
	virtual uint64_t GetLength() const;
	virtual VariableBase& operator = (const VariableBase& variable);
	virtual void SetMutable(bool is_mutable);
	virtual void SetVisible(bool is_visible);
	virtual void SetSerializable(bool is_serializable);
	virtual void SetModified(bool is_modified);

private:
	typedef std::vector<VariableBase *> Variables;
	Variables variables;
};

template <class TYPE>
VariableArray<TYPE>::VariableArray(const char *_name, Object *_owner, TYPE *_variables, uint64_t dim, VariableBase::Type type, const char *_description) :
	VariableBase(_name, _owner, VariableBase::VAR_ARRAY, _description),
	variables()
{
	for(uint64_t i = 0; i < dim; ++i)
	{
		std::ostringstream sstr;
		sstr << i;
		variables.push_back(new Variable<TYPE>(sstr.str().c_str(), *this, new DirectVariableStorage<TYPE>(*(_variables + i)), type, _description));
	}
}

template <class TYPE>
VariableArray<TYPE>::~VariableArray()
{
	for(typename Variables::iterator variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		delete *variable_iter;
	}
}

template <class TYPE>
VariableBase& VariableArray<TYPE>::operator [] (uint64_t index)
{
	if(index >= variables.size())
	{
		std::cerr << "Subscript out of range" << std::endl;
		return GetVoidVariable();
	}
	return (*variables[index]);
}

template <class TYPE>
const VariableBase& VariableArray<TYPE>::operator [] (uint64_t index) const
{
	if(index >= variables.size())
	{
		std::cerr << "Subscript out of range" << std::endl;
		return GetVoidVariable();
	}
	return (*variables[index]);
}

template <class TYPE>
uint64_t VariableArray<TYPE>::GetLength() const
{
	return (variables.size());
}

template <class TYPE>
VariableBase& VariableArray<TYPE>::operator = (const VariableBase& variable)
{
	for(uint64_t index = 0, length = variable.GetLength(); (index < length) && (index < variables.size()); ++index)
	{
		*variables[index] = variable[index];
	}
	return (*this);
}

template <class TYPE>
void VariableArray<TYPE>::SetFormat(Format fmt)
{
	for(typename Variables::iterator variable_iter = variables.begin(); variable_iter != variables.end(); ++variable_iter)
	{
		(*variable_iter)->SetFormat(fmt);
	}
}

template <class TYPE>
void VariableArray<TYPE>::SetMutable(bool _is_mutable)
{
	VariableBase::SetMutable(_is_mutable);
	for(typename Variables::iterator variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		(*variable_iter)->SetMutable(_is_mutable);
	}
}

template <class TYPE>
void VariableArray<TYPE>::SetVisible(bool _is_visible)
{
	VariableBase::SetVisible(_is_visible);
	for(typename Variables::iterator variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		(*variable_iter)->SetVisible(_is_visible);
	}
}

template <class TYPE>
void VariableArray<TYPE>::SetSerializable(bool _is_serializable)
{
	VariableBase::SetSerializable(_is_serializable);
	for(typename Variables::iterator variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		(*variable_iter)->SetSerializable(_is_serializable);
	}
}

template <class TYPE>
void VariableArray<TYPE>::SetModified(bool _is_modified)
{
	// Arrays can't be modified, only their elements
}

template <class TYPE>
class ParameterArray : public VariableArray<TYPE>
{
public:
	ParameterArray(const char *name, Object *owner, TYPE *parameters, uint64_t dim, const char *description = NULL) : VariableArray<TYPE>(name, owner, parameters, dim, VariableBase::VAR_PARAMETER, description) {}
	using VariableArray<TYPE>::operator =;
};

template <class TYPE>
class StatisticArray : public VariableArray<TYPE>
{
public:
	StatisticArray(const char *name, Object *owner, TYPE *parameters, uint64_t dim, const char *description = NULL) : VariableArray<TYPE>(name, owner, parameters, dim, VariableBase::VAR_STATISTIC, description) {}
	using VariableArray<TYPE>::operator =;
};

template <class TYPE>
class RegisterArray : public VariableArray<TYPE>
{
public:
	RegisterArray(const char *name, Object *owner, TYPE *parameters, uint64_t dim, const char *description = NULL) : VariableArray<TYPE>(name, owner, parameters, dim, VariableBase::VAR_REGISTER, description) {}
	using VariableArray<TYPE>::operator =;
};

template <class TYPE>
class SignalArray : public VariableArray<TYPE>
{
public:
	SignalArray(const char *name, Object *owner, TYPE *parameters, uint64_t dim, const char *description = NULL) : VariableArray<TYPE>(name, owner, parameters, dim, VariableBase::VAR_SIGNAL, description) {}
	using VariableArray<TYPE>::operator =;
};

//=============================================================================
//=                           VariableVector<TYPE>                            =
//=============================================================================

template <class TYPE>
class VariableVector : public VariableBase
{
public:
	typedef VariableBase::Type Type;
	VariableVector(const char *name, Object *owner, std::vector<TYPE>& variables, Type type, const char *description = NULL);
	virtual ~VariableVector();

	virtual VariableBase& operator [] (uint64_t index);
	virtual const VariableBase& operator [] (uint64_t index) const;
	void SetFormat(Format fmt);
	virtual uint64_t GetLength() const;
	virtual VariableBase& operator = (const VariableBase& variable);
	virtual void SetMutable(bool is_mutable);
	virtual void SetVisible(bool is_visible);
	virtual void SetSerializable(bool is_serializable);
	virtual void SetModified(bool is_modified);

private:
	typedef std::vector<VariableBase *> Variables;
	Variables variables;
};

template <class TYPE>
VariableVector<TYPE>::VariableVector(const char *_name, Object *_owner, std::vector<TYPE>& _variables, VariableBase::Type type, const char *_description) :
	VariableBase(_name, _owner, VariableBase::VAR_ARRAY, _description),
	variables()
{
	for(uint64_t i = 0, dim = _variables.size(); ; ++i)
	{
		std::ostringstream sstr;
		sstr << i;
		
		if(i >= dim) _variables.resize(i + 1);
		Variable<TYPE> *variable = new Variable<TYPE>(sstr.str().c_str(), *this, new IndirectVariableStorage<std::vector<TYPE>, uint64_t, TYPE>(_variables, i), type, _description);
		if((i >= dim) && !variable->IsInitialized())
		{
			_variables.pop_back();
			delete variable;
			break;
		}
		variables.push_back(variable);
	};
}

template <class TYPE>
VariableVector<TYPE>::~VariableVector()
{
	for(typename Variables::iterator variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		delete *variable_iter;
	}
}

template <class TYPE>
VariableBase& VariableVector<TYPE>::operator [] (uint64_t index)
{
	if(index >= variables.size())
	{
		std::cerr << "Subscript out of range" << std::endl;
		return GetVoidVariable();
	}
	return (*variables[index]);
}

template <class TYPE>
const VariableBase& VariableVector<TYPE>::operator [] (uint64_t index) const
{
	if(index >= variables.size())
	{
		std::cerr << "Subscript out of range" << std::endl;
		return GetVoidVariable();
	}
	return (*variables[index]);
}

template <class TYPE>
uint64_t VariableVector<TYPE>::GetLength() const
{
	return (variables.size());
}

template <class TYPE>
VariableBase& VariableVector<TYPE>::operator = (const VariableBase& variable)
{
	for(uint64_t index = 0, length = variable.GetLength(); (index < length) && (index < variables.size()); ++index)
	{
		*variables[index] = variable[index];
	}
	return (*this);
}

template <class TYPE>
void VariableVector<TYPE>::SetFormat(Format fmt)
{
	for(typename Variables::iterator variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		(*variable_iter)->SetFormat(fmt);
	}
}

template <class TYPE>
void VariableVector<TYPE>::SetMutable(bool _is_mutable)
{
	VariableBase::SetMutable(_is_mutable);
	for(typename Variables::iterator variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		(*variable_iter)->SetMutable(_is_mutable);
	}
}

template <class TYPE>
void VariableVector<TYPE>::SetVisible(bool _is_visible)
{
	VariableBase::SetVisible(_is_visible);
	for(typename Variables::iterator variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		(*variable_iter)->SetVisible(_is_visible);
	}
}

template <class TYPE>
void VariableVector<TYPE>::SetSerializable(bool _is_serializable)
{
	VariableBase::SetSerializable(_is_serializable);
	for(typename Variables::iterator variable_iter = variables.begin(); variable_iter != variables.end(); variable_iter++)
	{
		(*variable_iter)->SetSerializable(_is_serializable);
	}
}

template <class TYPE>
void VariableVector<TYPE>::SetModified(bool _is_modified)
{
	// Vectors can't be modified, only their elements
}

template <class TYPE>
class ParameterVector : public VariableVector<TYPE>
{
public:
	ParameterVector(const char *name, Object *owner, std::vector<TYPE>& parameters, const char *description = NULL) : VariableVector<TYPE>(name, owner, parameters, VariableBase::VAR_PARAMETER, description) {}
	using VariableVector<TYPE>::operator =;
};

template <class TYPE>
class StatisticVector : public VariableVector<TYPE>
{
public:
	StatisticVector(const char *name, Object *owner, std::vector<TYPE>& parameters, const char *description = NULL) : VariableVector<TYPE>(name, owner, parameters, VariableBase::VAR_STATISTIC, description) {}
	using VariableVector<TYPE>::operator =;
};

template <class TYPE>
class RegisterVector : public VariableVector<TYPE>
{
public:
	RegisterVector(const char *name, Object *owner, std::vector<TYPE>& parameters, const char *description = NULL) : VariableVector<TYPE>(name, owner, parameters, VariableBase::VAR_REGISTER, description) {}
	using VariableVector<TYPE>::operator =;
};

template <class TYPE>
class SignalVector : public VariableVector<TYPE>
{
public:
	SignalVector(const char *name, Object *owner, std::vector<TYPE>& parameters, const char *description = NULL) : VariableVector<TYPE>(name, owner, parameters, VariableBase::VAR_SIGNAL, description) {}
	using VariableVector<TYPE>::operator =;
};

} // end of namespace variable
} // end of namespace kernel
} // end of namespace unisim

#endif // __UNISIM_KERNEL_VARIABLE_VARIABLE_HH__

