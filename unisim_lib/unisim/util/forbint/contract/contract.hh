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

#include <unisim/util/forbint/contract/domsec_callback.h>
#include <unisim/util/forbint/contract/decsec_callback.h>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/util/endian/endian.hh>
#include <utility>
#include <functional>
#include <vector>
#include <limits>
#include <cmath>
#include <cassert>
  

namespace unisim {
namespace util {
namespace forbint {
namespace contract {

class DomainValue
{
private:
  DomainElement deValue;
  struct _DomainElementFunctions* pfFunctions;
  DomainEvaluationEnvironment* peEnv;

public:
  friend class MemoryState;
  DomainElement& svalue() { return deValue; }
  bool hasFunctionTable() const { return pfFunctions; }
  struct _DomainElementFunctions& functionTable() const { assert(pfFunctions); return *pfFunctions; }
  struct _DomainElementFunctions& funcs() const { assert(pfFunctions); return *pfFunctions; }
  DomainEvaluationEnvironment* env() const { return peEnv; }
  const DomainElement& value() const { return deValue; }

public:
  DomainValue() : deValue{ nullptr }, pfFunctions(nullptr), peEnv(nullptr) {}
  class Empty {};
  DomainValue(Empty, const DomainValue& ref)
    : deValue{ nullptr }, pfFunctions(ref.pfFunctions), peEnv(ref.peEnv) {}
  DomainValue(Empty, struct _DomainElementFunctions* functions, DomainEvaluationEnvironment* env)
    : deValue{ nullptr }, pfFunctions(functions), peEnv(env) {}
  DomainValue(DomainElement&& value, struct _DomainElementFunctions* functions, DomainEvaluationEnvironment* env)
    : deValue(std::move(value)), pfFunctions(functions), peEnv(env) {}
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
// inline
  DomainValue& assign(DomainElement&& value)
    {
      if (deValue.content)
        (*pfFunctions->free)(&deValue);
      deValue = value;
      value.content = 0;
      return *this;
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

  template <typename T>
  bool is_constant( T& value )
  {
    switch (getType())
      {
      case DTBit:
        {
          bool res = false;
          if (not (*pfFunctions->bit_is_constant_value)(deValue, &res))
            return false;
          value = T(res);
          return true;
        }
      case DTInteger:
        {
          DomainIntegerConstant divalue;
          if (not (*pfFunctions->multibit_is_constant_value)(deValue, &divalue))
            return false;
          if (std::numeric_limits<T>::is_signed != divalue.isSigned)
            { struct SignError {}; throw SignError(); }
          value = T(divalue.integerValue);
          struct EncodingError {};
          if (divalue.isSigned)
            { if ((int64_t)divalue.integerValue != value) throw EncodingError(); }
          else
            { if (         divalue.integerValue != value) throw EncodingError(); }
          return true;
        }
      case DTFloating:
      case DTUndefined:
        break;
      }
    return false;
  }

  char* write() const;
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
  //MemoryState(Processor& proc);
   void assign(const MemoryStateOwner& source);

   bool hasError() const { return uErrors; }
   const unsigned& errors() const { return uErrors; }
   void clearErrors() { uErrors = 0U; }
   void mergeWith(MemoryStateOwner& source);

   void setNumberOfRegisters(int number)
      {  (*pfFunctions->set_number_of_registers)(pmModel, number); }
   void setEvaluationEnvironment(DomainEvaluationEnvironment& domainEnvironment)
      {  peDomainEnv = &domainEnvironment; }

   DomainEvaluationEnvironment* env() const { return peDomainEnv; }

   void setRegisterValue(int registerIndex, DomainValue& value) const
      {
         assert(value.isValid());
         (*pfFunctions->set_register_value)(pmModel, registerIndex, &value.svalue(), pParameters, &uErrors);
         value.svalue() = DomainElement{};
      }
   DomainElement getRegisterValueAsElement(int registerIndex) const
      {  DomainElementFunctions* domainFunctions = nullptr;
         return (*pfFunctions->get_register_value)
               (pmModel, registerIndex, pParameters, &uErrors, &domainFunctions);
      }
   // DomainValue getRegisterValueAsBit(int registerIndex) const
   //    {  DomainElementFunctions* domainFunctions = nullptr;
   //       DomainElement result = (*pfFunctions->get_register_value)
   //             (pmModel, registerIndex, pParameters, &uErrors, &domainFunctions);
   //       DomainMultiBitValue<char> res(std::move(result), domainFunctions, peDomainEnv); 
   //       return DomainBitValue(std::move(res));
   //    }
   // template <typename TypeInt>
   // DomainMultiBitValue<TypeInt> getRegisterValueAsMultiBit(int registerIndex) const
   //    {  DomainElementFunctions* domainFunctions = nullptr; 
   //       DomainElement result = (*pfFunctions->get_register_value)
   //             (pmModel, registerIndex, pParameters, &uErrors, &domainFunctions);
   //       return DomainMultiBitValue<TypeInt>(std::move(result), domainFunctions, peDomainEnv);
   //    }
   // DomainMultiFloatValue getRegisterValueAsMultiFloat(int registerIndex) const
   //    {  DomainElementFunctions* domainFunctions = nullptr; 
   //       DomainElement result = (*pfFunctions->get_register_value)
   //             (pmModel, registerIndex, pParameters, &uErrors, &domainFunctions);
   //       return DomainMultiFloatValue(std::move(result), domainFunctions, peDomainEnv);
   //    }

  DomainElement loadMultiBit(DomainValue const& address, unsigned size) const
   {
      DomainElementFunctions* domainFunctions = nullptr; 
      return (*pfFunctions->load_multibit_value)
        (pmModel, address.value(), size, pParameters, &uErrors, &domainFunctions);
   }
   // template <class IntType>
   // DomainMultiBitValue<IntType> loadMultiBitValue(
   //       const DomainMultiBitValue<uint32_t>& indirectAddress) const
   //    {  DomainElementFunctions* domainFunctions = nullptr; 
   //       DomainElement result = (*pfFunctions->load_multibit_value)
   //             (pmModel, indirectAddress.value(), sizeof(IntType), pParameters, &uErrors,
   //              &domainFunctions);
   //       return DomainMultiBitValue<IntType>(std::move(result), domainFunctions, peDomainEnv);
   //    }
  DomainElement loadMultiBitDisjunction(DomainValue const& address, unsigned size) const
   {
      DomainElementFunctions* domainFunctions = nullptr; 
      return (*pfFunctions->load_multibit_disjunctive_value)
        (pmModel, address.value(), size, pParameters, &uErrors, &domainFunctions);
   }
   // template <class IntType>
   // DomainMultiBitValue<IntType> loadMultiBitDisjunctionValue(
   //       DomainMultiBitValue<uint32_t>&& indirectAddress) const
   //    {  DomainElementFunctions* domainFunctions = nullptr; 
   //       DomainElement result = (*pfFunctions->load_multibit_disjunctive_value)
   //             (pmModel, indirectAddress.value(), sizeof(IntType), pParameters, &uErrors,
   //              &domainFunctions);
   //       return DomainMultiBitValue<IntType>(std::move(result), domainFunctions, peDomainEnv);
   //    }
  DomainElement loadMultiFloat(DomainValue const& address, unsigned size) const
   {
      DomainElementFunctions* domainFunctions = nullptr; 
      return (*pfFunctions->load_multifloat_value)
        (pmModel, address.value(), size, pParameters, &uErrors, &domainFunctions);
   }
   // template <class FloatType>
   // DomainMultiFloatValue<FloatType> loadMultiFloatValue(
   //       DomainMultiBitValue<uint32_t>&& indirectAddress) const
   //    {  DomainElementFunctions* domainFunctions = nullptr; 
   //       DomainElement result = (*pfFunctions->load_multifloat_value)
   //             (pmModel, indirectAddress.value(), sizeof(FloatType), pParameters, &uErrors,
   //              &domainFunctions);
   //       return DomainMultiFloatValue<FloatType>(std::move(result), domainFunctions, peDomainEnv);
   //    }

   void valueStore(DomainValue const& address, DomainValue const& value) const
      {
         (*pfFunctions->store_value)(pmModel, address.value(), value.value(), pParameters, &uErrors);
      }
   // template <class IntType>
   // void storeMultiBitValue(const DomainMultiBitValue<uint32_t>& indirectAddress,
   //       const DomainMultiBitValue<IntType>& value)
   //    {  (*pfFunctions->store_value)(pmModel, indirectAddress.value(),
   //             value.value(), pParameters, &uErrors);
   //    }
   // template <class FloatType>
   // void storeMultiFloatValue(const DomainMultiBitValue<uint32_t>& indirectAddress,
   //       const DomainMultiFloatValue<FloatType>& value)
   //    {  (*pfFunctions->store_value)(pmModel, indirectAddress.value(),
   //             value.value(), pParameters, &uErrors);
   //    }
   void constraintStore(DomainValue const& address, DomainValue const& value, unsigned indirectRegister) const
      {
         (*pfFunctions->constraint_store_value)(pmModel, address.value(), value.value(), indirectRegister, pParameters, &uErrors);
      }
   // template <class IntType>
   // void constraintStoreValue(const DomainMultiBitValue<uint32_t>& indirectAddress,
   //       const DomainMultiBitValue<IntType>& value, unsigned indirectRegister)
   //    {  (*pfFunctions->constraint_store_value)(pmModel, indirectAddress.value(),
   //          value.value(), indirectRegister, pParameters, &uErrors);
   //    }
   void constraintAddress(DomainValue const& address, DomainValue const& value) const
      {
         (*pfFunctions->constraint_address)(pmModel, address.value(), value.value(), pParameters, &uErrors);
      }
   // template <class IntType>
   // void constraintAddress(const DomainMultiBitValue<uint32_t>& indirectAddress,
   //       const DomainMultiBitValue<IntType>& value)
   //    {  (*pfFunctions->constraint_address)(pmModel, indirectAddress.value(),
   //          value.value(), pParameters, &uErrors);
   //    }
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
