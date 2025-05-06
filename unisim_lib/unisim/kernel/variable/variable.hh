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
	IndirectVariableStorage(CONTAINER& _container, KEY _key) : container(_container), key(_key) {}
	
	virtual TYPE& Get() { return container[key]; }
private:
	CONTAINER& container;
	KEY key;
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

//=============================================================================
//=                            Formula<TYPE>                                  =
//=============================================================================

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
class VariableArray : public VariableArrayBase
{
public:
	typedef VariableBase::Type Type;
	VariableArray(const char *name, Object *owner, TYPE *variables, std::size_t dim, Type type, const char *description = NULL);
};

template <class TYPE>
VariableArray<TYPE>::VariableArray(const char *_name, Object *_owner, TYPE *_variables, std::size_t dim, VariableBase::Type type, const char *_description)
	: VariableArrayBase(_name, _owner, _description)
{
	for(std::size_t i = 0; i < dim; ++i)
	{
		std::ostringstream sstr;
		sstr << i;
		this->Add(new Variable<TYPE>(sstr.str().c_str(), *this, new DirectVariableStorage<TYPE>(*(_variables + i)), type, _description));
	}
}

template <class TYPE>
class ParameterArray : public VariableArray<TYPE>
{
public:
	ParameterArray(const char *name, Object *owner, TYPE *parameters, std::size_t dim, const char *description = NULL) : VariableArray<TYPE>(name, owner, parameters, dim, VariableBase::VAR_PARAMETER, description) {}
	using VariableArray<TYPE>::operator =;
};

template <class TYPE>
class StatisticArray : public VariableArray<TYPE>
{
public:
	StatisticArray(const char *name, Object *owner, TYPE *parameters, std::size_t dim, const char *description = NULL) : VariableArray<TYPE>(name, owner, parameters, dim, VariableBase::VAR_STATISTIC, description) {}
	using VariableArray<TYPE>::operator =;
};

template <class TYPE>
class RegisterArray : public VariableArray<TYPE>
{
public:
	RegisterArray(const char *name, Object *owner, TYPE *parameters, std::size_t dim, const char *description = NULL) : VariableArray<TYPE>(name, owner, parameters, dim, VariableBase::VAR_REGISTER, description) {}
	using VariableArray<TYPE>::operator =;
};

template <class TYPE>
class SignalArray : public VariableArray<TYPE>
{
public:
	SignalArray(const char *name, Object *owner, TYPE *parameters, std::size_t dim, const char *description = NULL) : VariableArray<TYPE>(name, owner, parameters, dim, VariableBase::VAR_SIGNAL, description) {}
	using VariableArray<TYPE>::operator =;
};

//=============================================================================
//=                           VariableVector<TYPE>                            =
//=============================================================================

template <class TYPE>
class VariableVector : public VariableArrayBase
{
public:
	typedef VariableBase::Type Type;
	typedef std::vector<TYPE> Storage;
	VariableVector(const char *name, Object *owner, Storage& variables, Type type, const char *description = NULL);
};

template <class TYPE>
VariableVector<TYPE>::VariableVector(const char *_name, Object *_owner, Storage& _variables, VariableBase::Type type, const char *_description)
	: VariableArrayBase(_name, _owner, _description)
{
	for(std::size_t i = 0, dim = _variables.size(); ; ++i)
	{
		std::ostringstream sstr;
		sstr << i;
		
		if(i >= dim) _variables.resize(i + 1);
		Variable<TYPE> *variable = new Variable<TYPE>(sstr.str().c_str(), *this, new IndirectVariableStorage<Storage, std::size_t, TYPE>(_variables, i), type, _description);
		if((i >= dim) && !variable->IsInitialized())
		{
			_variables.pop_back();
			delete variable;
			break;
		}
		this->Add(variable);
	}
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

