/*
 *  Copyright (c) 2019-2020,
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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#include <arch.hh>

typedef Processor<Intel64>  P64;
typedef Processor<Compat32> P32;

typedef ProcessorBase::u64_t U64;
typedef ProcessorBase::s64_t S64;
typedef ProcessorBase::f64_t F64;

void eval_div( P32& arch, U64& hi, U64& lo, U64 const& divisor ) { throw ProcessorBase::Unimplemented(); }
void eval_div( P64& arch, U64& hi, U64& lo, U64 const& divisor ) { throw ProcessorBase::Unimplemented(); }
void eval_div( P32& arch, S64& hi, S64& lo, S64 const& divisor ) { throw ProcessorBase::Unimplemented(); }
void eval_div( P64& arch, S64& hi, S64& lo, S64 const& divisor ) { throw ProcessorBase::Unimplemented(); }
void eval_mul( P32& arch, U64& hi, U64& lo, U64 const& multiplier ) { throw ProcessorBase::Unimplemented(); }
void eval_mul( P64& arch, U64& hi, U64& lo, U64 const& multiplier ) { throw ProcessorBase::Unimplemented(); }
void eval_mul( P32& arch, S64& hi, S64& lo, S64 const& multiplier ) { throw ProcessorBase::Unimplemented(); }
void eval_mul( P64& arch, S64& hi, S64& lo, S64 const& multiplier ) { throw ProcessorBase::Unimplemented(); }

F64 eval_fprem ( P32& arch, F64 const& dividend, F64 const& modulus ) { throw ProcessorBase::Unimplemented(); }
F64 eval_fprem ( P64& arch, F64 const& dividend, F64 const& modulus ) { throw ProcessorBase::Unimplemented(); }
F64 eval_fprem1( P32& arch, F64 const& dividend, F64 const& modulus ) { throw ProcessorBase::Unimplemented(); }
F64 eval_fprem1( P64& arch, F64 const& dividend, F64 const& modulus ) { throw ProcessorBase::Unimplemented(); }

namespace unisim { namespace component { namespace cxx { namespace processor { namespace intel {
          template <typename FPT> FPT firound( FPT const& src, int x87frnd_mode )
          {
            throw ProcessorBase::Unimplemented();
            return FPT();
          }
        } /* namespace unisim */ } /* namespace component */ } /* namespace cxx */ } /* namespace processor */ } /* namespace intel */


#include <unisim/component/cxx/processor/intel/isa/intel.tcc>

typedef unisim::component::cxx::processor::intel::Mode Mode;

template <>
P64::Operation*
P64::Decode(uint64_t address, uint8_t const* bytes)
{
  typedef unisim::component::cxx::processor::intel::InputCode<P64> InputCode;
  return getoperation( InputCode( Mode(1, 0, 1), bytes, OpHeader(address) ) );
}

template <>
P32::Operation*
P32::Decode(uint32_t address, uint8_t const* bytes)
{
  typedef unisim::component::cxx::processor::intel::InputCode<P32> InputCode;
  return getoperation( InputCode( Mode(0, 1, 1), bytes, OpHeader(address) ) );
}
