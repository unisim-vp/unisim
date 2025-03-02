/*
 *  Copyright (c) 2017,
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

#include <unisim/component/cxx/processor/intel/dba/arch.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace intel {
namespace dba {

typedef Processor<Intel64>  P64;
typedef Processor<Compat32> P32;

typedef ProcessorBase::rc_t  RC;
typedef ProcessorBase::u64_t U64;
typedef ProcessorBase::s64_t S64;
typedef ProcessorBase::f64_t F64;

template <class I64>
void eval_div_128( P64& arch, I64& hi, I64& lo, I64 const& divisor )
{
  typedef unisim::util::symbolic::Expr Expr;
  typedef unisim::util::symbolic::binsec::BitFilter BitFilter;
  if (arch.Test(divisor == I64(0))) arch._DE();

  bool const sext = hi.is_signed;
  Expr dividend = new unisim::util::symbolic::vector::VCat<P64::u128_t>( std::vector<Expr>{lo.expr, hi.expr}, 8 );
  Expr divisor128 = BitFilter::mksimple( divisor.expr, 64, 0, 64, 128, sext );
  Expr divres = unisim::util::symbolic::make_operation(divisor.is_signed ? "Div" : "Divu", dividend, divisor128);
  Expr modres = unisim::util::symbolic::make_operation(divisor.is_signed ? "Mod" : "Modu", dividend, divisor128);
  lo.expr = BitFilter::mksimple( divres, 128,  0, 64, 64, false );
  hi.expr = BitFilter::mksimple( modres, 128,  0, 64, 64, false );
}

template <class I64>
void eval_mul_128( P64& arch, I64& hi, I64& lo, I64 const& multiplier )
{
  typedef unisim::util::symbolic::Expr Expr;
  typedef unisim::util::symbolic::binsec::BitFilter BitFilter;
  bool const sext = hi.is_signed;
  Expr result = unisim::util::symbolic::make_operation("Mul", BitFilter::mksimple( lo.expr, 64, 0, 64, 128, sext ), BitFilter::mksimple( multiplier.expr, 64, 0, 64, 128, sext ));
  hi.expr = BitFilter::mksimple( result, 128, 64, 64, 64, false );
  lo.expr = BitFilter::mksimple( result, 128,  0, 64, 64, false );
  P64::bit_t ovf( unisim::util::symbolic::make_operation("Tne", BitFilter::mksimple( result, 128, 0, 64, 128, sext ), result) );
  arch.flagwrite( P64::FLAG::OF, ovf );
  arch.flagwrite( P64::FLAG::CF, ovf );
}

void eval_div( P32& arch, U64& hi, U64& lo, U64 const& divisor )    { throw ProcessorBase::Unimplemented(); }
void eval_div( P64& arch, U64& hi, U64& lo, U64 const& divisor )    { eval_div_128(arch, hi, lo, divisor); }
void eval_div( P32& arch, S64& hi, S64& lo, S64 const& divisor )    { throw ProcessorBase::Unimplemented(); }
void eval_div( P64& arch, S64& hi, S64& lo, S64 const& divisor )    { eval_div_128(arch, hi, lo, divisor); }
void eval_mul( P32& arch, U64& hi, U64& lo, U64 const& multiplier ) { throw ProcessorBase::Unimplemented(); }
void eval_mul( P64& arch, U64& hi, U64& lo, U64 const& multiplier ) { eval_mul_128(arch, hi, lo, multiplier); }
void eval_mul( P32& arch, S64& hi, S64& lo, S64 const& multiplier ) { throw ProcessorBase::Unimplemented(); }
void eval_mul( P64& arch, S64& hi, S64& lo, S64 const& multiplier ) { eval_mul_128(arch, hi, lo, multiplier);}

F64 eval_fprem ( P32& arch, F64 const& dividend, F64 const& modulus ) { throw ProcessorBase::Unimplemented(); }
F64 eval_fprem ( P64& arch, F64 const& dividend, F64 const& modulus ) { throw ProcessorBase::Unimplemented(); }
F64 eval_fprem1( P32& arch, F64 const& dividend, F64 const& modulus ) { throw ProcessorBase::Unimplemented(); }
F64 eval_fprem1( P64& arch, F64 const& dividend, F64 const& modulus ) { throw ProcessorBase::Unimplemented(); }

} /* end of namespace dba */

template <typename FPT> FPT firound( FPT const& src, dba::RC x87frnd_mode )
{
  return FPT(unisim::util::symbolic::Expr(new unisim::util::symbolic::binsec::Opaque<FPT>({src.expr, x87frnd_mode.expr})));
}

} /* end of namespace intel */
} /* end of namespace processor */
} /* end of namespace cxx */
} /* end of namespace component */
} /* end of namespace unisim */


#include <unisim/component/cxx/processor/intel/isa/intel.tcc>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace intel {
namespace dba {

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

} /* end of namespace dba */
} /* end of namespace intel */
} /* end of namespace processor */
} /* end of namespace cxx */
} /* end of namespace component */
} /* end of namespace unisim */

