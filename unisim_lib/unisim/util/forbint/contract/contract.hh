/*
 *  Copyright (c) 2007-2020,
 *  Commissariat a l'Energie Atomique (CEA),
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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr), Franck Vedrine (franck.vedrine@cea.fr)
 */

#ifndef __UNISIM_UTIL_FORBINT_CONTRACT_CONTRACT_HH__
#define __UNISIM_UTIL_FORBINT_CONTRACT_CONTRACT_HH__

namespace unisim {
namespace util {
namespace forbint {
namespace contract {


struct Processor;
class MemoryState;
class DomainValue
{
private:
  DomainElement deValue;
  struct _DomainElementFunctions* pfFunctions;
  DomainEvaluationEnvironment* peEnv;

protected:
  friend class MemoryState;
  DomainElement& svalue() { return deValue; }
  bool hasFunctionTable() const { return pfFunctions; }
  struct _DomainElementFunctions& functionTable() const { assert(pfFunctions); return *pfFunctions; }
  DomainEvaluationEnvironment* env() const { return peEnv; }

public:
  const DomainElement& value() const { return deValue; }

public:
  DomainValue() : deValue{ nullptr }, pfFunctions(nullptr), peEnv(nullptr) {}
  class Empty {};
  DomainValue(Empty, const DomainValue& ref)
    : deValue{ nullptr }, pfFunctions(ref.pfFunctions), peEnv(ref.peEnv) {}
  DomainValue(DomainElement&& value, struct _DomainElementFunctions* functions, DomainEvaluationEnvironment* env)
    : deValue(std::move(value)), pfFunctions(functions), peEnv(env) {}
  DomainValue(Processor& processor);
  DomainValue(DomainElement&& value, Processor& processor);
  DomainValue(DomainElement&& value, const DomainValue& source)
    : deValue(value), pfFunctions(source.pfFunctions), peEnv(source.peEnv)
    { value.content = nullptr; }
  DomainValue(DomainValue&& source)
    : deValue(source.deValue), pfFunctions(source.pfFunctions), peEnv(source.peEnv)
    { source.deValue.content = nullptr; }
  DomainValue(const DomainValue& source)
    : deValue{ nullptr }, pfFunctions(source.pfFunctions), peEnv(source.peEnv)
    {
      if (source.deValue.content) {
        assert(pfFunctions);
        deValue = (*pfFunctions->clone)(source.deValue);
      }
    }
  DomainValue& operator=(DomainValue&& source)
    {
      if (this == &source)
        return *this;
      if (deValue.content)
      {
        assert(pfFunctions);
        (*pfFunctions->free)(&deValue);
      }
      pfFunctions = source.pfFunctions;
      peEnv = source.peEnv;
      deValue = source.deValue;
      source.deValue.content = nullptr;
      return *this;
    }
  DomainValue& operator=(const DomainValue& source)
    {
      if (this == &source)
        return *this;
      if (deValue.content)
        (*pfFunctions->free)(&deValue);
      pfFunctions = source.pfFunctions;
      peEnv = source.peEnv;
      if (source.deValue.content)
      {
        assert(pfFunctions);
        deValue = (*pfFunctions->clone)(source.deValue);
      }
      return *this;
    }
  ~DomainValue()
    { if (deValue.content && pfFunctions) (*pfFunctions->free)(&deValue); }

  void clear()
    {
      if (deValue.content)
      {
        assert(pfFunctions);
        (*pfFunctions->free)(&deValue);
      }
    }
  bool isValid() const { return deValue.content; }
  DomainType getType() const
    {
      assert(pfFunctions);
      return (*pfFunctions->get_type)(deValue);
    }
  ZeroResult queryZeroResult() const
    {
      assert(pfFunctions);
      return (*pfFunctions->query_zero_result)(deValue);
    }
  int getSizeInBits() const
    {
      assert(pfFunctions);
      return (*pfFunctions->get_size_in_bits)(deValue);
    }
  bool is_top() const
    {
      assert(pfFunctions);
      return (*pfFunctions->is_top)(deValue);
    }
  friend char* debugPrint(const DomainValue* value);
};

// extern DomainValue getRootDomainValue();

template <typename TypeInt>
class DomainMultiBitValue;
class DomainBitValue : public DomainValue
{
private:
  typedef DomainValue inherited;
  bool fConstant;

protected:
  DomainBitValue(Empty empty, const DomainValue& ref)
    : DomainValue(empty, ref), fConstant(false) {}

public:
  DomainBitValue() : fConstant(false) {}
  DomainBitValue(DomainBitElement&& value, struct _DomainElementFunctions* functions, DomainEvaluationEnvironment* env)
    : DomainValue(std::move(value), functions, env), fConstant(false) {}
  DomainBitValue(Processor& processor);
  DomainBitValue(DomainBitElement&& value, Processor& processor);
  DomainBitValue(DomainBitElement&& value, const DomainValue& source);
  explicit DomainBitValue(bool value, Processor& processor)
    : DomainValue(processor), fConstant(false)
    { svalue() = (*functionTable().bit_create_constant)(value); }
  explicit DomainBitValue( bool value )
    : DomainValue(), fConstant(value) {}

  DomainBitValue(DomainBitValue&& source) = default;
  DomainBitValue(const DomainBitValue& source) = default;
  DomainBitValue& operator=(DomainBitValue&& source) = default;
  DomainBitValue& operator=(const DomainBitValue& source) = default;

  template <typename TypeResult, int size>
  DomainMultiBitValue<TypeResult> castToMultiBit() const;

  DomainBitValue& setToConstant(bool value)
    {
      if (inherited::hasFunctionTable())
        svalue() = (*functionTable().bit_create_constant)(value);
      else
        fConstant = value;
      return *this;
    }

  DomainBitValue& setToUndefined(bool isSymbolic)
    { svalue() = (*functionTable().bit_create_top)(isSymbolic); return *this; }

  DomainBitValue applyUnary(DomainBitUnaryOperation operation,
      std::function<bool(bool)> constantFunction) const
    {
      if (inherited::isValid())
        return DomainBitValue((*functionTable().bit_create_unary_apply)
            (value(), operation, env()), *this);
      else
        return DomainBitValue(constantFunction(fConstant));
    }
  DomainBitValue applyBinary(DomainBitBinaryOperation operation,
      const DomainBitValue& source,
      std::function<bool(bool, bool)> constantFunction) const
    {
      if (inherited::isValid() && source.inherited::isValid())
        return DomainBitValue((*functionTable().bit_create_binary_apply)
            (value(), operation, source.value(), env()), *this);
      else if (inherited::isValid()) // !source.inherited::isValid()
      {
        DomainBitValue alt(Empty(), *this);
        alt.setToConstant(source.fConstant);
        return applyBinary(operation, alt, constantFunction);
      }
      else if (source.inherited::isValid()) // !inherited::isValid()
      {
        DomainBitValue alt(Empty(), source);
        alt.setToConstant(fConstant);
        alt.applyBinaryAssign(operation, source, constantFunction);
        return std::move(alt);
      }
      else
        return DomainBitValue(constantFunction(fConstant, source.fConstant));
    }
  DomainBitValue applyCompare(DomainBitCompareOperation operation,
      const DomainBitValue& source,
      std::function<bool(bool, bool)> constantFunction) const
    {
      if (inherited::isValid() && source.inherited::isValid())
        return DomainBitValue((*functionTable().bit_binary_compare_domain)
            (value(), operation, source.value(), env()), *this);
      else if (inherited::isValid()) // !source.inherited::isValid()
      {
        DomainBitValue alt(Empty(), *this);
        alt.setToConstant(source.fConstant);
        return applyCompare(operation, alt, constantFunction);
      }
      else if (source.inherited::isValid()) // !inherited::isValid()
      {
        DomainBitValue alt(Empty(), source);
        alt.setToConstant(fConstant);
        return alt.applyCompare(operation, source, constantFunction);
      }
      else
        return DomainBitValue(constantFunction(fConstant, source.fConstant));
    }
  DomainBitValue& applyBinaryAssign(DomainBitBinaryOperation operation,
      const DomainBitValue& source,
      std::function<bool(bool, bool)> constantFunction)
    {
      if (inherited::isValid() && source.inherited::isValid())
        (*functionTable().bit_binary_apply_assign)
            (&svalue(), operation, source.value(), env());
      else if (inherited::isValid()) // !source.inherited::isValid()
      {
        DomainBitValue alt(Empty(), *this);
        alt.setToConstant(source.fConstant);
        applyBinaryAssign(operation, alt, constantFunction);
      }
      else if (source.inherited::isValid()) // !inherited::isValid()
      {
        bool thisConstant = fConstant;
        operator=(DomainBitValue(Empty(), source));
        setToConstant(thisConstant);
        applyBinaryAssign(operation, source, constantFunction);
      }
      else
        fConstant = constantFunction(fConstant, source.fConstant);
      return *this;
    }

  DomainBitValue operator~() const
    { return applyUnary(DBUONegate, [](bool val) { return !val; }); }
  DomainBitValue operator!() const
    { return applyUnary(DBUONegate, [](bool val) { return !val; }); }
  DomainBitValue operator|(const DomainBitValue& source) const
    { return applyBinary(DBBOOr, source,
          [](bool fst, bool snd) { return fst | snd; });
    }
  DomainBitValue operator&(const DomainBitValue& source) const
    { return applyBinary(DBBOAnd, source,
          [](bool fst, bool snd) { return fst & snd; });
    }
  DomainBitValue operator^(const DomainBitValue& source) const
    { return applyBinary(DBBOExclusiveOr, source,
          [](bool fst, bool snd) { return fst ^ snd; });
    }
  DomainBitValue operator||(const DomainBitValue& source) const
    { return applyBinary(DBBOOr, source,
          [](bool fst, bool snd) { return fst || snd; });
    }
  DomainBitValue operator&&(const DomainBitValue& source) const
    { return applyBinary(DBBOOr, source,
          [](bool fst, bool snd) { return fst && snd; });
    }
  DomainBitValue& operator|=(const DomainBitValue& source)
    { return applyBinaryAssign(DBBOOr, source,
          [](bool fst, bool snd) { return fst | snd; });
    }
  DomainBitValue& operator&=(const DomainBitValue& source)
    { return applyBinaryAssign(DBBOAnd, source,
          [](bool fst, bool snd) { return fst & snd; });
    }
  DomainBitValue operator^=(const DomainBitValue& source)
    { return applyBinaryAssign(DBBOExclusiveOr, source,
          [](bool fst, bool snd) { return fst ^ snd; });
    }

  DomainBitValue operator==(const DomainBitValue& source) const
    { return applyCompare(DBCOCompareEqual, source,
          [](bool fst, bool snd) { return fst == snd; });
    }
  DomainBitValue operator!=(const DomainBitValue& source) const
    { return applyCompare(DBCOCompareDifferent, source,
          [](bool fst, bool snd) { return fst != snd; });
    }
  DomainBitValue operator<=(const DomainBitValue& source) const
    { return applyCompare(DBCOCompareLessOrEqual, source,
          [](bool fst, bool snd) { return fst <= snd; });
    }
  DomainBitValue operator>=(const DomainBitValue& source) const
    { return applyCompare(DBCOCompareGreaterOrEqual, source,
          [](bool fst, bool snd) { return fst >= snd; });
    }
  DomainBitValue operator<(const DomainBitValue& source) const
    { return applyCompare(DBCOCompareLess, source,
          [](bool fst, bool snd) { return fst < snd; });
    }
  DomainBitValue operator>(const DomainBitValue& source) const
    { return applyCompare(DBCOCompareGreater, source,
          [](bool fst, bool snd) { return fst > snd; });
    }

  bool isConstant(bool* value) const
    {
      if (inherited::isValid())
        return (*functionTable().bit_is_constant_value)(this->value(), value);
      else
      {
        if (value)
          *value = fConstant;
        return true;
      }
    }
};

template <typename VALUE_TYPE>
class DomainMultiFloatValue;

template <typename VALUE_TYPE>
class DomainMultiBitValue : public DomainValue
{
private:
  typedef DomainMultiBitValue<VALUE_TYPE> this_type;
  typedef DomainValue inherited;
  VALUE_TYPE uConstant;
  template <typename SRC_VALUE_TYPE>
  friend class DomainMultiBitValue;
  template <typename SRC_VALUE_TYPE>
  friend class DomainMultiFloatValue;

public:
  typedef VALUE_TYPE value_type;
  
  DomainMultiBitValue() : uConstant(0) {}

  explicit DomainMultiBitValue( value_type value ) : uConstant(value) {}
  DomainMultiBitValue( DomainMultiBitValue&& other ) = default;
  DomainMultiBitValue( Empty empty, const DomainValue& other )
    : inherited(empty, other), uConstant(0) {}
  DomainMultiBitValue( const DomainMultiBitValue& other ) = default;
  DomainMultiBitValue& operator=( DomainMultiBitValue&& other ) = default;
  DomainMultiBitValue& operator=( const DomainMultiBitValue& other ) = default;
  explicit DomainMultiBitValue( const DomainBitValue& other )
    : DomainValue(Empty(), other)
    {
      if (inherited::hasFunctionTable())
        svalue() = (*functionTable().bit_create_cast_multibit)(other.value(),8*sizeof(VALUE_TYPE),env());
      else {
        bool res;
        other.isConstant(&res);
        uConstant = res;
      }
    }
  DomainMultiBitValue(DomainElement&& element, Processor& proc)
    : DomainValue(std::move(element), proc) {}
  DomainMultiBitValue(DomainElement&& element, DomainValue const& value)
    : DomainValue(std::move(element), value) {}
  DomainMultiBitValue(DomainElement&& value, struct _DomainElementFunctions* functions, DomainEvaluationEnvironment* env)
    : DomainValue(std::move(value), functions, env) {}

  template <typename SRC_VALUE_TYPE>
  explicit DomainMultiBitValue( DomainMultiBitValue<SRC_VALUE_TYPE> const& other )
    {
      if (CmpTypes<SRC_VALUE_TYPE,VALUE_TYPE>::same) {
        inherited::operator= ( other );
      } else {
        inherited::operator= ( this_type(Empty(), other) );
        if (inherited::hasFunctionTable())
          svalue() = (*functionTable().multibit_create_cast_multibit)(other.value(),8*sizeof(VALUE_TYPE), std::numeric_limits<VALUE_TYPE>::is_signed, env());
        else
          uConstant = other.uConstant;
      }
    }

private:
  bool isSigned() const { return std::numeric_limits<VALUE_TYPE>::is_signed; }

public:
  DomainElement& svalue() { return DomainValue::svalue(); }
  const DomainElement& value() const { return DomainValue::value(); }

public:
  DomainMultiBitValue& setToConstant(VALUE_TYPE value)
    {
      if (inherited::hasFunctionTable())
        svalue() = (*functionTable().multibit_create_constant)(DomainIntegerConstant{8*sizeof(VALUE_TYPE), std::numeric_limits<VALUE_TYPE>::is_signed, uint64_t(value)});
      else
        uConstant = value;
      return *this;
    }
  DomainMultiBitValue& setToUndefined(int sizeInBits, bool isSymbolic)
    {
      svalue() = (*functionTable().multibit_create_top)(sizeInBits, isSymbolic);
      return *this;
    }
  operator DomainBitValue() const
    {
      if (inherited::isValid()) // [TODO] do not assume it is the first bit
        return DomainBitValue((*functionTable().multibit_create_cast_shift_bit)
            (value(), 0, env()), *this);
      else
        return DomainBitValue((bool) uConstant);
    }
  DomainBitValue castShiftBit(int shift)
    {
      if (inherited::isValid())
        return DomainBitValue((*functionTable().multibit_create_cast_shift_bit)
            (value(), shift, env()), *this);
      else
        return DomainBitValue((bool) (uConstant & ((VALUE_TYPE) 1 << shift)));
    }
  template <class ResultType, int size>
  DomainMultiFloatValue<ResultType> castToMultiFloat() const
    {
      if (inherited::isValid())
        return DomainMultiFloatValue<ResultType>((*functionTable().multibit_create_cast_multifloat)(value(), size, 
            std::numeric_limits<VALUE_TYPE>::is_signed, env()), *this);
      else
        return DomainMultiFloatValue<ResultType>((ResultType) uConstant);
    }

  void reduce(int first, int last)
    {
      if (inherited::isValid())
        (*functionTable().multibit_reduce_apply_assign)(&svalue(),
            DomainMultiBitReduceOperation{first, last}, env());
      else
        uConstant = (uConstant >> first) & ~(~((VALUE_TYPE) 0) << (last-first+1));
    }
  void extendWithZero(int new_size)
    {
      if (inherited::isValid())
        (*functionTable().multibit_extend_apply_assign)(&svalue(),
            DomainMultiBitExtendOperation{DMBEOExtendWithZero, new_size}, env());
    }
  template <typename IntType>
  void bitset(int first, int last, const DomainMultiBitValue<IntType>& source)
    {
      if (inherited::isValid() && source.inherited::isValid())
        (*functionTable().multibit_bitset_apply_assign)(&svalue(),
            DomainMultiBitSetOperation{first, last}, source.value(), env());
      else if (inherited::isValid())
        { // !source.inherited::isValid()
          DomainMultiBitValue<IntType> alt(Empty(), *this);
          alt.setToConstant(source.uConstant);
          bitset(first, last, alt);
        }
      else if (source.inherited::isValid())
        {
          VALUE_TYPE constant = uConstant;
          this->operator=(DomainMultiBitValue<VALUE_TYPE>(Empty(), source));
          setToConstant(constant);
          bitset(first, last, source);
        }
      else
        {
          VALUE_TYPE mask = (~((VALUE_TYPE) 0) << first);
          if ((size_t) (last+1) < sizeof(VALUE_TYPE)*8)
            mask &= ~(~((VALUE_TYPE) 0) << (last+1));
          uConstant &= ~mask;
          uConstant |= ((VALUE_TYPE) source.uConstant) << first;
        }
    }
  this_type applyUnary(DomainMultiBitUnaryOperation operation,
      std::function<VALUE_TYPE(VALUE_TYPE)> constantFunction) const
    {
      if (inherited::isValid())
        return DomainMultiBitValue((*functionTable().multibit_create_unary_apply)
            (value(), operation, env()), *this);
      else
        return DomainMultiBitValue(constantFunction(uConstant));
    }
  this_type applyUnary(DomainMultiBitUnaryOperation signedOperation,
      DomainMultiBitUnaryOperation unsignedOperation,
      std::function<VALUE_TYPE(VALUE_TYPE)> constantFunction) const
    {
      if (inherited::isValid())
        return DomainMultiBitValue((*functionTable().multibit_create_unary_apply)
            (value(), isSigned() ? signedOperation : unsignedOperation, env()), *this);
      else
        return DomainMultiBitValue(constantFunction(uConstant));
    }
  this_type& applyUnaryAssign(DomainMultiBitUnaryOperation signedOperation,
      DomainMultiBitUnaryOperation unsignedOperation,
      std::function<void(VALUE_TYPE&)> constantFunction)
    {
      if (inherited::isValid())
        (*functionTable().multibit_unary_apply_assign)
            (&svalue(), isSigned() ? signedOperation : unsignedOperation, env());
      else
        constantFunction(uConstant);
      return *this;
    }
    
  template <typename IntType>
  this_type applyBinary(DomainMultiBitBinaryOperation signedOperation,
      DomainMultiBitBinaryOperation unsignedOperation,
      const DomainMultiBitValue<IntType>& source,
      std::function<VALUE_TYPE(VALUE_TYPE, IntType)> constantFunction) const
    {
      if (inherited::isValid() && source.inherited::isValid())
        return this_type((*functionTable().multibit_create_binary_apply)
            (value(), isSigned() ? signedOperation : unsignedOperation, source.value(), env()), *this);
      else if (inherited::isValid()) // !source.inherited::isValid()
      {
        DomainMultiBitValue<IntType> alt(Empty(), *this);
        alt.setToConstant(source.uConstant);
        return applyBinary(signedOperation, unsignedOperation, alt, constantFunction);
      }
      else if (source.inherited::isValid()) // !inherited::isValid()
      {
        this_type alt(Empty(), source);
        alt.setToConstant(uConstant);
        alt.applyBinaryAssign(signedOperation, unsignedOperation, source, constantFunction);
        return std::move(alt);
      }
      else
        return this_type(constantFunction(uConstant, source.uConstant));
    }
  template <typename IntType>
  this_type applyBinary(DomainMultiBitBinaryOperation operation,
      const DomainMultiBitValue<IntType>& source,
      std::function<VALUE_TYPE(VALUE_TYPE, IntType)> constantFunction) const
    {
      if (inherited::isValid() && source.inherited::isValid())
        return this_type((*functionTable().multibit_create_binary_apply)
            (value(), operation, source.value(), env()), *this);
      else if (inherited::isValid()) // !source.inherited::isValid()
      {
        DomainMultiBitValue<IntType> alt(Empty(), *this);
        alt.setToConstant(source.uConstant);
        return applyBinary(operation, alt, constantFunction);
      }
      else if (source.inherited::isValid()) // !inherited::isValid()
      {
        this_type alt(Empty(), source);
        alt.setToConstant(uConstant);
        alt.applyBinaryAssign(operation, source, constantFunction);
        return std::move(alt);
      }
      else
        return this_type(constantFunction(uConstant, source.uConstant));
    }
  DomainBitValue applyCompare(DomainMultiBitCompareOperation signedOperation,
      DomainMultiBitCompareOperation unsignedOperation,
      const this_type& source,
      std::function<bool(VALUE_TYPE, VALUE_TYPE)> constantFunction) const
    {
      if (inherited::isValid() && source.inherited::isValid())
        return DomainBitValue((*functionTable().multibit_binary_compare_domain)
            (value(), isSigned() ? signedOperation : unsignedOperation, source.value(), env()), *this);
      else if (inherited::isValid()) // !source.inherited::isValid()
      {
        this_type alt(Empty(), *this);
        alt.setToConstant(source.uConstant);
        return applyCompare(signedOperation, unsignedOperation, alt, constantFunction);
      }
      else if (source.inherited::isValid()) // !inherited::isValid()
      {
        this_type alt(Empty(), source);
        alt.setToConstant(uConstant);
        return alt.applyCompare(signedOperation, unsignedOperation, source, constantFunction);
      }
      else
        return DomainBitValue(constantFunction(uConstant, source.uConstant));
    }
  DomainBitValue applyCompare(DomainMultiBitCompareOperation operation,
      const this_type& source,
      std::function<bool(VALUE_TYPE, VALUE_TYPE)> constantFunction) const
    {
      if (inherited::isValid() && source.inherited::isValid())
        return DomainBitValue((*functionTable().multibit_binary_compare_domain)
            (value(), operation, source.value(), env()), *this);
      else if (inherited::isValid()) // !source.inherited::isValid()
      {
        this_type alt(Empty(), *this);
        alt.setToConstant(source.uConstant);
        return applyCompare(operation, alt, constantFunction);
      }
      else if (source.inherited::isValid()) // !inherited::isValid()
      {
        this_type alt(Empty(), source);
        alt.setToConstant(uConstant);
        return alt.applyCompare(operation, source, constantFunction);
      }
      else
        return DomainBitValue(constantFunction(uConstant, source.uConstant));
    }
  template <typename IntType>
  this_type& applyBinaryAssign(DomainMultiBitBinaryOperation signedOperation,
      DomainMultiBitBinaryOperation unsignedOperation,
      const DomainMultiBitValue<IntType>& source,
      std::function<VALUE_TYPE(VALUE_TYPE, IntType)> constantFunction)
    {
      if (inherited::isValid() && source.inherited::isValid())
        (*functionTable().multibit_binary_apply_assign)
            (&svalue(), isSigned() ? signedOperation : unsignedOperation, source.value(), env());
      else if (inherited::isValid()) // !source.inherited::isValid()
      {
        DomainMultiBitValue<IntType> alt(Empty(), *this);
        alt.setToConstant(source.uConstant);
        applyBinaryAssign(signedOperation, unsignedOperation, alt, constantFunction);
      }
      else if (source.inherited::isValid()) // !inherited::isValid()
      {
        VALUE_TYPE thisConstant = uConstant;
        operator=(this_type(Empty(), source));
        setToConstant(thisConstant);
        applyBinaryAssign(signedOperation, unsignedOperation, source, constantFunction);
      }
      else
        uConstant = constantFunction(uConstant, source.uConstant);
      return *this;
    }
  template <typename IntType>
  this_type& applyBinaryAssign(DomainMultiBitBinaryOperation operation,
      const DomainMultiBitValue<IntType>& source,
      std::function<VALUE_TYPE(VALUE_TYPE, IntType)> constantFunction)
    {
      if (inherited::isValid() && source.inherited::isValid())
        (*functionTable().multibit_binary_apply_assign)
            (&svalue(), operation, source.value(), env());
      else if (inherited::isValid()) // !source.inherited::isValid()
      {
        DomainMultiBitValue<IntType> alt(Empty(), *this);
        alt.setToConstant(source.uConstant);
        applyBinaryAssign(operation, alt, constantFunction);
      }
      else if (source.inherited::isValid()) // !inherited::isValid()
      {
        VALUE_TYPE thisConstant = uConstant;
        operator=(this_type(Empty(), source));
        setToConstant(thisConstant);
        applyBinaryAssign(operation, source, constantFunction);
      }
      else
        uConstant = constantFunction(uConstant, source.uConstant);
      return *this;
    }

  DomainMultiBitValue operator~() const
    { return applyUnary(DMBUOBitNegate, [](VALUE_TYPE val) { return ~val; }); }
  DomainMultiBitValue operator-() const
    { return applyUnary(DMBUOOppositeSigned, [](VALUE_TYPE val) { return -val; }); }
  DomainMultiBitValue& operator++()
    { return applyUnaryAssign(DMBUONextSigned, DMBUONextUnsigned,
        [](VALUE_TYPE& val) { ++val; });
    }
  DomainMultiBitValue& operator--()
    { return applyUnaryAssign(DMBUOPrevSigned, DMBUOPrevUnsigned,
        [](VALUE_TYPE& val) { --val; });
    }
  DomainMultiBitValue operator+(const DomainMultiBitValue& source) const
    { std::function<VALUE_TYPE(VALUE_TYPE, VALUE_TYPE)> fun
        = [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst + snd; };
      return applyBinary(DMBBOPlusSigned, DMBBOPlusUnsigned, source, fun);
    }
  DomainMultiBitValue operator-(const DomainMultiBitValue& source) const
    { std::function<VALUE_TYPE(VALUE_TYPE, VALUE_TYPE)> fun
        = [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst - snd; };
      return applyBinary(DMBBOMinusSigned, DMBBOMinusUnsigned, source, fun);
    }
  DomainMultiBitValue operator*(const DomainMultiBitValue& source) const
    { std::function<VALUE_TYPE(VALUE_TYPE, VALUE_TYPE)> fun
        = [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst * snd; };
      return applyBinary(DMBBOTimesSigned, DMBBOTimesUnsigned, source, fun);
    }
  DomainMultiBitValue operator/(const DomainMultiBitValue& source) const
    { std::function<VALUE_TYPE(VALUE_TYPE, VALUE_TYPE)> fun
        = [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst / snd; };
      return applyBinary(DMBBODivideSigned, DMBBODivideUnsigned, source, fun);
    }
  DomainMultiBitValue operator%(const DomainMultiBitValue& source) const
    { std::function<VALUE_TYPE(VALUE_TYPE, VALUE_TYPE)> fun
        = [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst % snd; };
      return applyBinary(DMBBOModuloSigned, DMBBOModuloUnsigned, source, fun);
    }
  DomainMultiBitValue operator|(const DomainMultiBitValue& source) const
    { std::function<VALUE_TYPE(VALUE_TYPE, VALUE_TYPE)> fun
        = [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst | snd; };
      return applyBinary(DMBBOBitOr, source, fun);
    }
  DomainMultiBitValue operator&(const DomainMultiBitValue& source) const
    { std::function<VALUE_TYPE(VALUE_TYPE, VALUE_TYPE)> fun
        = [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst & snd; };
      return applyBinary(DMBBOBitAnd, source, fun);
    }
  DomainMultiBitValue operator^(const DomainMultiBitValue& source) const
    { std::function<VALUE_TYPE(VALUE_TYPE, VALUE_TYPE)> fun
        = [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst ^ snd; };
      return applyBinary(DMBBOBitExclusiveOr, source, fun);
    }

  DomainMultiBitValue& operator+=(const DomainMultiBitValue& source)
    { std::function<VALUE_TYPE(VALUE_TYPE, VALUE_TYPE)> fun
        = [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst + snd; };
      return applyBinaryAssign(DMBBOPlusSigned, DMBBOPlusUnsigned, source, fun);
    }
  DomainMultiBitValue& operator-=(const DomainMultiBitValue& source)
    { std::function<VALUE_TYPE(VALUE_TYPE, VALUE_TYPE)> fun
        = [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst - snd; };
      return applyBinaryAssign(DMBBOMinusSigned, DMBBOMinusUnsigned, source, fun);
    }
  DomainMultiBitValue& operator*=(const DomainMultiBitValue& source)
    { std::function<VALUE_TYPE(VALUE_TYPE, VALUE_TYPE)> fun
        = [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst * snd; };
      return applyBinaryAssign(DMBBOTimesSigned, DMBBOTimesUnsigned, source, fun);
    }
  DomainMultiBitValue& operator/=(const DomainMultiBitValue& source)
    { std::function<VALUE_TYPE(VALUE_TYPE, VALUE_TYPE)> fun
        = [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst / snd; };
      return applyBinaryAssign(DMBBODivideSigned, DMBBODivideUnsigned, source, fun);
    }
  DomainMultiBitValue& operator%=(const DomainMultiBitValue& source)
    { std::function<VALUE_TYPE(VALUE_TYPE, VALUE_TYPE)> fun
        = [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst % snd; };
      return applyBinaryAssign(DMBBOModuloSigned, DMBBOModuloUnsigned, source, fun);
    }
  DomainMultiBitValue& operator|=(const DomainMultiBitValue& source)
    { std::function<VALUE_TYPE(VALUE_TYPE, VALUE_TYPE)> fun
        = [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst | snd; };
      return applyBinaryAssign(DMBBOBitOr, source, fun);
    }
  DomainMultiBitValue& operator&=(const DomainMultiBitValue& source)
    { std::function<VALUE_TYPE(VALUE_TYPE, VALUE_TYPE)> fun
        = [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst & snd; };
      return applyBinaryAssign(DMBBOBitAnd, source, fun);
    }
  DomainMultiBitValue& operator^=(const DomainMultiBitValue& source)
    { std::function<VALUE_TYPE(VALUE_TYPE, VALUE_TYPE)> fun
        = [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst ^ snd; };
      return applyBinaryAssign(DMBBOBitExclusiveOr, source, fun);
    }

  template <typename SHT>
  DomainMultiBitValue operator << (SHT sht) const
    { std::function<VALUE_TYPE(VALUE_TYPE, SHT)> fun
        = [](VALUE_TYPE fst, SHT snd) { return fst << snd; };
      return applyBinary(DMBBOLeftShift, DomainMultiBitValue<SHT>(sht), fun);
    }
  template <typename SHT>
  DomainMultiBitValue operator << (const DomainMultiBitValue<SHT>& sht) const
    { std::function<VALUE_TYPE(VALUE_TYPE, SHT)> fun
        = [](VALUE_TYPE fst, SHT snd) { return fst << snd; };
      return applyBinary(DMBBOLeftShift, sht, fun);
    }
  template <typename SHT>
  DomainMultiBitValue& operator <<= (SHT sht)
    { std::function<VALUE_TYPE(VALUE_TYPE, SHT)> fun
        = [](VALUE_TYPE fst, SHT snd) { return fst << snd; };
      return applyBinaryAssign(DMBBOLeftShift, DomainMultiBitValue<SHT>(sht), fun);
    }
  template <typename SHT>
  DomainMultiBitValue operator >> (SHT sht) const
    { std::function<VALUE_TYPE(VALUE_TYPE, SHT)> fun
        = [](VALUE_TYPE fst, SHT snd) { return fst >> snd; };
      return applyBinary(DMBBOArithmeticRightShift, DMBBOLogicalRightShift,
        DomainMultiBitValue<SHT>(sht), fun);
    }
  template <typename SHT>
  DomainMultiBitValue operator >> (const DomainMultiBitValue<SHT>& sht) const
    { std::function<VALUE_TYPE(VALUE_TYPE, SHT)> fun
        = [](VALUE_TYPE fst, SHT snd) { return fst >> snd; };
      return applyBinary(DMBBOArithmeticRightShift, DMBBOLogicalRightShift,
        sht, fun);
    }
  template <typename SHT>
  DomainMultiBitValue& operator >>= (SHT sht)
    { std::function<VALUE_TYPE(VALUE_TYPE, SHT)> fun
        = [](VALUE_TYPE fst, SHT snd) { return fst >> snd; };
      return applyBinaryAssign(DMBBOArithmeticRightShift, DMBBOLogicalRightShift,
        DomainMultiBitValue<SHT>(sht), fun);
    }

  DomainBitValue operator==(const DomainMultiBitValue& source) const
    { return applyCompare(DMBCOCompareEqual, source,
        [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst == snd; });
    }
  DomainBitValue operator!=(const DomainMultiBitValue& source) const
    { return applyCompare(DMBCOCompareDifferent, source,
        [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst != snd; });
    }
  DomainBitValue operator<=(const DomainMultiBitValue& source) const
    { return applyCompare(DMBCOCompareLessOrEqualSigned, DMBCOCompareLessOrEqualUnsigned,
        source, [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst <= snd; });
    }
  DomainBitValue operator>=(const DomainMultiBitValue& source) const
    { return applyCompare(DMBCOCompareGreaterOrEqualSigned, DMBCOCompareGreaterOrEqualUnsigned,
        source, [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst >= snd; });
    }
  DomainBitValue operator<(const DomainMultiBitValue& source) const
    { return applyCompare(DMBCOCompareLessSigned, DMBCOCompareLessUnsigned,
        source, [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst < snd; });
    }
  DomainBitValue operator>(const DomainMultiBitValue& source) const
    { return applyCompare(DMBCOCompareGreaterSigned, DMBCOCompareGreaterUnsigned,
        source, [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst > snd; });
    }

  template <typename TypeVal>
  friend this_type RotateRight(const this_type& first, TypeVal second)
    { std::function<VALUE_TYPE(VALUE_TYPE, int32_t)> fun
        = [](VALUE_TYPE fst, int32_t snd)
        { return unisim::util::arithmetic::RotateRight(fst, snd); };
      return first.applyBinary(DMBBORightRotate,
        DomainMultiBitValue<int32_t>(second), fun);
    }
  friend this_type BitScanReverse(const this_type& first)
    { return first.applyUnary(DMBUOBitScanReverse,
        [](VALUE_TYPE fst)
          { return unisim::util::arithmetic::BitScanReverse(fst); });
    }
  friend this_type RotateRight(const this_type& first, const DomainMultiBitValue<int32_t>& second)
    { return first.applyBinary(DMBBORightRotate, second,
        [](VALUE_TYPE fst, int32_t snd)
          { return unisim::util::arithmetic::RotateRight(fst, snd); });
    }
  friend this_type ByteSwap(const this_type& first)
    {
      if (first.inherited::isValid())
      { DomainMultiBitValue result = first;
        for (int index = 0; index < int(sizeof(VALUE_TYPE)/2); ++index) {
          DomainMultiBitValue first_source(first), last_source(first);
          first_source.reduce(index*8+0, index*8+7);
          last_source.reduce((sizeof(VALUE_TYPE)-index-1)*8+0, (sizeof(VALUE_TYPE)-index)*8-1);
          result.bitset(index*8+0, index*8+7, last_source);
          result.bitset((sizeof(VALUE_TYPE)-index-1)*8+0, (sizeof(VALUE_TYPE)-index)*8-1, first_source);
        }
        return result;
      }
      else
        return DomainMultiBitValue(unisim::util::endian::ByteSwap(first.uConstant));
    }
  bool isConstant(VALUE_TYPE* value) const
    {
      if (inherited::isValid()) {
        DomainIntegerConstant res;
        res.sizeInBits = sizeof(VALUE_TYPE);
        res.isSigned = isSigned();
        bool result = (*functionTable().multibit_is_constant_value)(this->value(), &res);
        if (result && value)
           *value = (VALUE_TYPE) res.integerValue;
        return result;
      }
      else
      {
        if (value)
          *value = uConstant;
        return true;
      }
    }
};

template <typename VALUE_TYPE>
struct DomainMultiFloatValue : public DomainValue {
private:
  typedef DomainMultiFloatValue<VALUE_TYPE> this_type;
  typedef DomainValue inherited;
  VALUE_TYPE vtConstant;
  template <typename SRC_VALUE_TYPE>
  friend class DomainMultiBitValue;
  template <typename SRC_VALUE_TYPE>
  friend class DomainMultiFloatValue;

public:
  DomainMultiFloatValue() : vtConstant(0.0) {}
  DomainMultiFloatValue(VALUE_TYPE val) : vtConstant(val) {}
  DomainMultiFloatValue(Empty empty, const DomainValue& ref)
    : DomainValue(empty, ref), vtConstant(0.0) {}
  DomainMultiFloatValue(DomainMultiFloatElement&& value, struct _DomainElementFunctions* functions, DomainEvaluationEnvironment* env)
    : DomainValue(std::move(value), functions, env), vtConstant(0.0) {}
  DomainMultiFloatValue(Processor& processor);

  DomainMultiFloatValue(DomainMultiFloatElement&& value, Processor& processor);
  DomainMultiFloatValue(DomainMultiFloatElement&& value, const DomainValue& source)
    : DomainValue(std::move(value), source) {}
  explicit DomainMultiFloatValue(DomainFloatingPointConstant value, Processor& processor)
    : DomainValue(processor)
    { svalue() = (*functionTable().multifloat_create_constant)(value); }
  DomainMultiFloatValue(DomainMultiFloatValue&& source) = default;
  DomainMultiFloatValue(const DomainMultiFloatValue& source) = default;
  DomainMultiFloatValue& operator=(DomainMultiFloatValue&& source) = default;
  DomainMultiFloatValue& operator=(const DomainMultiFloatValue& source) = default;

  template <class ResultType, int size>
  DomainMultiFloatValue<ResultType> castToMultiFloat() const
    {
      if (inherited::isValid())
        return DomainMultiFloatValue<ResultType>((*functionTable().multifloat_cast_multifloat)(value(), size, env()), *this);
      else
        return DomainMultiFloatValue<ResultType>((ResultType) vtConstant);
    }
  template <class ResultType, int size>
  DomainMultiBitValue<ResultType> castToMultiBit() const
    {
      if (inherited::isValid())
        return DomainMultiBitValue<ResultType>((*functionTable().multifloat_create_cast_multibit)(value(), size, env()), *this);
      else
        return DomainMultiBitValue<ResultType>((ResultType) vtConstant);
    }

  void setToConstant(VALUE_TYPE value)
    {
      if (inherited::hasFunctionTable())
        svalue() = (*functionTable().multifloat_create_constant)(
              DomainFloatingPointConstant{sizeof(value), value});
      else
        vtConstant = value;
    }
  void setToUndefined(int sizeInBits, bool isSymbolic)
    { svalue() = (*functionTable().multifloat_create_top)(sizeInBits, isSymbolic); }

  this_type applyUnary(DomainMultiFloatUnaryOperation operation,
      std::function<VALUE_TYPE(VALUE_TYPE)> constantFunction) const
    {
      if (inherited::isValid())
        return DomainMultiFloatValue((*functionTable().multifloat_create_unary_apply)
            (value(), operation, env()), *this);
      else
        return DomainMultiFloatValue(constantFunction(vtConstant));
    }
  this_type& applyUnaryAssign(DomainMultiFloatUnaryOperation operation,
      std::function<void(VALUE_TYPE&)> constantFunction)
    {
      if (inherited::isValid())
        (*functionTable().multifloat_unary_apply_assign)
            (&svalue(), operation, env());
      else
        constantFunction(vtConstant);
      return *this;
    }
  this_type applyBinary(DomainMultiFloatBinaryOperation operation,
      const DomainMultiFloatValue& source,
      std::function<VALUE_TYPE(VALUE_TYPE, VALUE_TYPE)> constantFunction) const
    {
      if (inherited::isValid() && source.inherited::isValid())
        return this_type((*functionTable().multifloat_create_binary_apply)
            (value(), operation, source.value(), env()), *this);
      else if (inherited::isValid()) // !source.inherited::isValid()
      {
        this_type alt(Empty(), *this);
        alt.setToConstant(source.vtConstant);
        return applyBinary(operation, alt, constantFunction);
      }
      else if (source.inherited::isValid()) // !inherited::isValid()
      {
        this_type alt(Empty(), source);
        alt.setToConstant(vtConstant);
        alt.applyBinaryAssign(operation, source, constantFunction);
        return std::move(alt);
      }
      else
        return this_type(constantFunction(vtConstant, source.vtConstant));
    }
  DomainBitValue applyCompare(DomainMultiFloatCompareOperation operation,
      const this_type& source,
      std::function<bool(VALUE_TYPE, VALUE_TYPE)> constantFunction) const
    {
      if (inherited::isValid() && source.inherited::isValid())
        return DomainBitValue((*functionTable().multifloat_binary_compare_domain)
            (value(), operation, source.value(), env()), *this);
      else if (inherited::isValid()) // !source.inherited::isValid()
      {
        this_type alt(Empty(), *this);
        alt.setToConstant(source.vtConstant);
        return applyCompare(operation, alt, constantFunction);
      }
      else if (source.inherited::isValid()) // !inherited::isValid()
      {
        this_type alt(Empty(), source);
        alt.setToConstant(vtConstant);
        return alt.applyCompare(operation, source, constantFunction);
      }
      else
        return DomainBitValue(constantFunction(vtConstant, source.vtConstant));
    }
  this_type& applyBinaryAssign(DomainMultiFloatBinaryOperation operation,
      const DomainMultiFloatValue& source,
      std::function<VALUE_TYPE(VALUE_TYPE, VALUE_TYPE)> constantFunction)
    {
      if (inherited::isValid() && source.inherited::isValid())
        (*functionTable().multifloat_binary_apply_assign)
            (&svalue(), operation, source.value(), env());
      else if (inherited::isValid()) // !source.inherited::isValid()
      {
        DomainMultiFloatValue alt(Empty(), *this);
        alt.setToConstant(source.vtConstant);
        applyBinaryAssign(operation, alt, constantFunction);
      }
      else if (source.inherited::isValid()) // !inherited::isValid()
      {
        VALUE_TYPE thisConstant = vtConstant;
        operator=(this_type(Empty(), source));
        setToConstant(thisConstant);
        applyBinaryAssign(operation, source, constantFunction);
      }
      else
        vtConstant = constantFunction(vtConstant, source.vtConstant);
      return *this;
    }

  DomainMultiFloatValue operator-() const
    { return applyUnary(DMFUOOpposite, [](VALUE_TYPE val) { return -val; }); }
  DomainMultiFloatValue operator+(const DomainMultiFloatValue& source) const
    { return applyBinary(DMFBOPlus, source,
          [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst + snd; });
    }
  DomainMultiFloatValue operator-(const DomainMultiFloatValue& source) const
    { return applyBinary(DMFBOMinus, source,
          [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst - snd; });
    }
  DomainMultiFloatValue operator*(const DomainMultiFloatValue& source) const
    { return applyBinary(DMFBOTimes, source,
          [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst * snd; });
    }
  DomainMultiFloatValue operator/(const DomainMultiFloatValue& source) const
    { return applyBinary(DMFBODivide, source,
          [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst / snd; });
    }

  DomainMultiFloatValue& operator+=(const DomainMultiFloatValue& source)
    { return applyBinaryAssign(DMFBOPlus, source,
          [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst + snd; });
    }
  DomainMultiFloatValue& operator-=(const DomainMultiFloatValue& source)
    { return applyBinaryAssign(DMFBOMinus, source,
          [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst - snd; });
    }
  DomainMultiFloatValue& operator*=(const DomainMultiFloatValue& source)
    { return applyBinaryAssign(DMFBOTimes, source,
          [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst * snd; });
    }
  DomainMultiFloatValue& operator/=(const DomainMultiFloatValue& source)
    { return applyBinaryAssign(DMFBODivide, source,
          [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst / snd; });
    }

  DomainMultiBitValue<int32_t> compare(const DomainMultiFloatValue& source) const
    {
      if (inherited::isValid() && source.inherited::isValid())
       return DomainMultiBitValue<int32_t>((*functionTable().multifloat_binary_full_compare_domain)
          (value(), source.value(), env()), *this);
      else if (inherited::isValid()) // !source.inherited::isValid()
      {
        this_type alt(Empty(), *this);
        alt.setToConstant(source.vtConstant);
        return compare(alt);
      }
      else if (source.inherited::isValid()) // !inherited::isValid()
      {
        this_type alt(Empty(), source);
        alt.setToConstant(vtConstant);
        return alt.compare(source);
      }
      else
        return DomainMultiBitValue<int32_t>((vtConstant < source.vtConstant)
            ? -1 : ((vtConstant > source.vtConstant) ? +1 : 0)); // also NaN
    }
  DomainBitValue operator==(const DomainMultiFloatValue& source) const
    { return applyCompare(DMFCOCompareEqual, source,
        [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst == snd; });
    }
  DomainBitValue operator!=(const DomainMultiFloatValue& source) const
    { return applyCompare(DMFCOCompareDifferent, source,
        [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst != snd; });
    }
  DomainBitValue operator<=(const DomainMultiFloatValue& source) const
    { return applyCompare(DMFCOCompareLessOrEqual, source,
        [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst <= snd; });
    }
  DomainBitValue operator>=(const DomainMultiFloatValue& source) const
    { return applyCompare(DMFCOCompareGreaterOrEqual, source,
        [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst >= snd; });
    }
  DomainBitValue operator<(const DomainMultiFloatValue& source) const
    { return applyCompare(DMFCOCompareLess, source,
        [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst < snd; });
    }
  DomainBitValue operator>(const DomainMultiFloatValue& source) const
    { return applyCompare(DMFCOCompareGreater, source,
        [](VALUE_TYPE fst, VALUE_TYPE snd) { return fst > snd; });
    }

  bool isConstant(VALUE_TYPE* value) const
    {
      if (inherited::isValid()) {
        DomainFloatingPointConstant res;
        res.sizeInBits = sizeof(VALUE_TYPE);
        bool result = (*functionTable().multifloat_is_constant_value)(this->value(), &res);
        if (result && value)
           *value = (VALUE_TYPE) res.floatValue;
        return result;
      }
      else
      {
        if (value)
          *value = vtConstant;
        return true;
      }
    }

  void setToNaN()
    { (*functionTable().multifloat_unary_apply_assign)(&svalue(), DMFUOSetToNaN, env()); }
  void setQuietBit()
    { (*functionTable().multifloat_unary_apply_assign)(&svalue(), DMFUOSetQuietBit, env()); }
  DomainBitValue setFlushToZero()
    { return DomainBitValue((*functionTable().multifloat_flush_to_zero)(&svalue(), env()), *this); }
  DomainBitValue queryIsSNaN() const
    { DomainMultiBitValue<char> res((*functionTable().multifloat_query_to_multibit)(value(),
             DMFIOIsSNaN, env()), *this);
      return DomainBitValue(res);
    }
  DomainBitValue queryIsQNaN() const
    { DomainMultiBitValue<char> res((*functionTable().multifloat_query_to_multibit)(value(),
             DMFIOIsQNaN, env()), *this);
      return DomainBitValue(res);
    }

  // used with arguments coming from Processor
  void multAssignAndAdd(const this_type& op1, const this_type& op2)
    {  (*functionTable().multifloat_ternary_apply_assign)
          (&svalue(), DMFTOMultAdd, op1.value(), op2.value(), env());
    }
  // used with arguments coming from Processor
  DomainBitValue queryIsInvalidMulAddNode(const this_type& op1, const this_type& op2) const
    {  return DomainBitValue((*functionTable().multifloat_ternary_query)
          (value(), DMFTQIsInvalid, op1.value(), op2.value(), env()), *this);
    }
  void negateAssign()
    { applyUnary(DMFUOOpposite, [](VALUE_TYPE val) { return -val; }); }
  void absAssign()
    { applyUnary(DMFUOAbs, [](VALUE_TYPE val) { return std::abs(val); }); }
  void sqrtAssign()
    { applyUnary(DMFUOSqrt, [](VALUE_TYPE val) { return std::sqrt(val); }); }
};

class MemoryStateOwner;
class MemoryState {
  private:
   MemoryModel* pmModel;
   struct _MemoryModelFunctions* pfFunctions;
   InterpretParameters* pParameters;
   DomainEvaluationEnvironment* peDomainEnv;
   mutable unsigned uErrors; /* set of MemoryEvaluationErrorFlags */
   friend class MemoryStateOwner;

  public:
   MemoryState()
      :  pmModel(nullptr), pfFunctions(nullptr), pParameters(nullptr),
         peDomainEnv(nullptr), uErrors(0U) {}
   MemoryState(MemoryModel* model, struct _MemoryModelFunctions* functions,
         InterpretParameters* parameters)
      :  pmModel(model), pfFunctions(functions), pParameters(parameters),
         peDomainEnv(nullptr), uErrors(0U) {}
   MemoryState(Processor& proc);
   void assign(const MemoryStateOwner& source);

   bool hasError() const { return uErrors; }
   const unsigned& errors() const { return uErrors; }
   void clearErrors() { uErrors = 0U; }
   void mergeWith(MemoryStateOwner& source);

   void setNumberOfRegisters(int number)
      {  (*pfFunctions->set_number_of_registers)(pmModel, number); }
   void setEvaluationEnvironment(DomainEvaluationEnvironment& domainEnvironment)
      {  peDomainEnv = &domainEnvironment; }
   template <typename TypeInt>
   void setRegisterValue(int registerIndex, DomainMultiBitValue<TypeInt>&& value, struct _DomainElementFunctions& functionTable)
      {  if (!((DomainValue&) value).isValid()) {
            TypeInt val;
            value.isConstant(&val);
            value = DomainMultiBitValue<TypeInt>((*functionTable.multibit_create_constant)(
                  DomainIntegerConstant{8*sizeof(TypeInt),
                  std::numeric_limits<TypeInt>::is_signed, (uint64_t) val}),
                  &functionTable, peDomainEnv);
         }
         (*pfFunctions->set_register_value)(pmModel, registerIndex,
            &value.svalue(), pParameters, &uErrors);
         value.svalue() = DomainElement{};
      }

   DomainElement getRegisterValueAsElement(int registerIndex) const
      {  DomainElementFunctions* domainFunctions = nullptr;
         return (*pfFunctions->get_register_value)
               (pmModel, registerIndex, pParameters, &uErrors, &domainFunctions);
      }
   DomainBitValue getRegisterValueAsBit(int registerIndex) const
      {  DomainElementFunctions* domainFunctions = nullptr; 
         DomainElement result = (*pfFunctions->get_register_value)
               (pmModel, registerIndex, pParameters, &uErrors, &domainFunctions);
         DomainMultiBitValue<char> res(std::move(result), domainFunctions, peDomainEnv); 
         return DomainBitValue(std::move(res));
      }
   template <typename TypeInt>
   DomainMultiBitValue<TypeInt> getRegisterValueAsMultiBit(int registerIndex) const
      {  DomainElementFunctions* domainFunctions = nullptr; 
         DomainElement result = (*pfFunctions->get_register_value)
               (pmModel, registerIndex, pParameters, &uErrors, &domainFunctions);
         return DomainMultiBitValue<TypeInt>(std::move(result), domainFunctions, peDomainEnv);
      }
   // DomainMultiFloatValue getRegisterValueAsMultiFloat(int registerIndex) const
   //    {  DomainElementFunctions* domainFunctions = nullptr; 
   //       DomainElement result = (*pfFunctions->get_register_value)
   //             (pmModel, registerIndex, pParameters, &uErrors, &domainFunctions);
   //       return DomainMultiFloatValue(std::move(result), domainFunctions, peDomainEnv);
   //    }

   template <class IntType>
   DomainMultiBitValue<IntType> loadMultiBitValue(
         const DomainMultiBitValue<uint32_t>& indirectAddress) const
      {  DomainElementFunctions* domainFunctions = nullptr; 
         DomainElement result = (*pfFunctions->load_multibit_value)
               (pmModel, indirectAddress.value(), sizeof(IntType), pParameters, &uErrors,
                &domainFunctions);
         return DomainMultiBitValue<IntType>(std::move(result), domainFunctions, peDomainEnv);
      }
   template <class IntType>
   DomainMultiBitValue<IntType> loadMultiBitDisjunctionValue(
         DomainMultiBitValue<uint32_t>&& indirectAddress) const
      {  DomainElementFunctions* domainFunctions = nullptr; 
         DomainElement result = (*pfFunctions->load_multibit_disjunctive_value)
               (pmModel, indirectAddress.value(), sizeof(IntType), pParameters, &uErrors,
                &domainFunctions);
         return DomainMultiBitValue<IntType>(std::move(result), domainFunctions, peDomainEnv);
      }
   template <class FloatType>
   DomainMultiFloatValue<FloatType> loadMultiFloatValue(
         DomainMultiBitValue<uint32_t>&& indirectAddress) const
      {  DomainElementFunctions* domainFunctions = nullptr; 
         DomainElement result = (*pfFunctions->load_multifloat_value)
               (pmModel, indirectAddress.value(), sizeof(FloatType), pParameters, &uErrors,
                &domainFunctions);
         return DomainMultiFloatValue<FloatType>(std::move(result), domainFunctions, peDomainEnv);
      }

   template <class IntType>
   void storeMultiBitValue(const DomainMultiBitValue<uint32_t>& indirectAddress,
         const DomainMultiBitValue<IntType>& value)
      {  (*pfFunctions->store_value)(pmModel, indirectAddress.value(),
               value.value(), pParameters, &uErrors);
      }
   template <class FloatType>
   void storeMultiFloatValue(const DomainMultiBitValue<uint32_t>& indirectAddress,
         const DomainMultiFloatValue<FloatType>& value)
      {  (*pfFunctions->store_value)(pmModel, indirectAddress.value(),
               value.value(), pParameters, &uErrors);
      }
   template <class IntType>
   void constraintStoreValue(const DomainMultiBitValue<uint32_t>& indirectAddress,
         const DomainMultiBitValue<IntType>& value, unsigned indirectRegister)
      {  (*pfFunctions->constraint_store_value)(pmModel, indirectAddress.value(),
            value.value(), indirectRegister, pParameters, &uErrors);
      }
   template <class IntType>
   void constraintAddress(const DomainMultiBitValue<uint32_t>& indirectAddress,
         const DomainMultiBitValue<IntType>& value)
      {  (*pfFunctions->constraint_address)(pmModel, indirectAddress.value(),
            value.value(), pParameters, &uErrors);
      }
};

class MemoryStateOwner {
  private:
   MemoryModel* pmModel;
   struct _MemoryModelFunctions* pfFunctions;
   friend class MemoryState;

  public:
   MemoryStateOwner(const MemoryState& source)
      :  pmModel(nullptr), pfFunctions(source.pfFunctions)
      {  if (source.pmModel)
            pmModel = (*pfFunctions->clone)(source.pmModel);
      }
   ~MemoryStateOwner()
      {  if (pmModel) {
            (*pfFunctions->free)(pmModel);
            pmModel = nullptr;
         }
      }
   void swap(MemoryState& source)
      {  if (pmModel && source.pmModel)
            (*pfFunctions->swap)(pmModel, source.pmModel);
      }
   void mergeWith(MemoryState& source)
      {  if (pmModel && source.pmModel)
            (*pfFunctions->merge)(pmModel, source.pmModel);
      }
};

inline void
MemoryState::assign(const MemoryStateOwner& source) {
   if (pmModel && source.pmModel)
      (*pfFunctions->assign)(pmModel, source.pmModel);
}

inline void
MemoryState::mergeWith(MemoryStateOwner& source) {
   if (pmModel && source.pmModel)
      (*pfFunctions->merge)(pmModel, source.pmModel);
}

  
} /* namespace contract */
} /* namespace forbint */
} /* namespace util */
} /* namespace unisim */

#endif /* __UNISIM_UTIL_FORBINT_CONTRACT_CONTRACT_HH__ */
