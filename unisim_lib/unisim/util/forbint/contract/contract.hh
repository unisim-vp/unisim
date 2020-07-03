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

protected:
  //  friend class MemoryState;
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

  char* write() const;
};

  
} /* namespace contract */
} /* namespace forbint */
} /* namespace util */
} /* namespace unisim */

#endif /* __UNISIM_UTIL_FORBINT_CONTRACT_CONTRACT_HH__ */
