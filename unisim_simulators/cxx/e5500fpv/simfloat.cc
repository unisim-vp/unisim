/*
 *  Copyright (c) 2018,
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

#include <simfloat.hh>
#include <unisim/util/simfloat/integer.hh>
#include <unisim/util/simfloat/integer.tcc>
#include <unisim/util/simfloat/floating.hh>
#include <unisim/util/simfloat/floating.tcc>
#include <unisim/util/likely/likely.hh>

SoftFloat&
SoftFloat::fromRawBitsAssign(uint32_t raw_bits)
{
  impl.setChunk((void*) &raw_bits, unisim::util::endian::IsHostLittleEndian());

  return *this;
}

SoftDouble&
SoftDouble::fromRawBitsAssign(uint64_t raw_bits)
{
  impl.setChunk((void*) &raw_bits, unisim::util::endian::IsHostLittleEndian());

  return *this;
}

SoftFloat&
SoftFloat::convertAssign(const SoftDouble& op1, Flags& flags)
{
  impl_type::FloatConversion fcConversion;
  
  fcConversion.setSizeMantissa(52).setSizeExponent(11);
  fcConversion.setNegative(op1.isNegative());
  fcConversion.exponent()[0] = op1.impl.queryBasicExponent()[0];
  fcConversion.mantissa()[0] = op1.impl.queryMantissa()[0];
  fcConversion.mantissa()[1] = op1.impl.queryMantissa()[1];

  impl = impl_type(fcConversion, flags);

  return *this;
}

SoftDouble&
SoftDouble::convertAssign(const SoftFloat& op1, Flags& flags)
{
  impl_type::FloatConversion fcConversion;
  
  fcConversion.setSizeMantissa(23).setSizeExponent(8);
  fcConversion.setNegative(op1.isNegative());
  fcConversion.exponent()[0] = op1.impl.queryBasicExponent()[0];
  fcConversion.mantissa()[0] = op1.impl.queryMantissa()[0];
   
  impl = impl_type(fcConversion, flags);
   
  return *this;
}

SoftFloat&  SoftFloat::  operator= (const SoftFloat& sfSource) { impl = sfSource.impl; return *this; }
SoftDouble& SoftDouble:: operator= (const SoftDouble& sdSource) { impl = sdSource.impl; return *this; }

SoftFloat&  SoftFloat:: assign(const SoftFloat& sfSource) { impl = sfSource.impl; return *this; }
SoftDouble& SoftDouble::assign(const SoftDouble& sdSource) { impl = sdSource.impl; return *this; }

uint32_t
SoftFloat::queryValue() const
{
  uint32_t uResult;
  impl.fillChunk(&uResult, unisim::util::endian::IsHostLittleEndian());
  
  return uResult;
}

uint64_t
SoftDouble::queryValue() const
{
  uint64_t uResult;
  impl.fillChunk( &uResult, unisim::util::endian::IsHostLittleEndian() );
  
  return uResult;
}

bool SoftFloat::isNegative() const { return impl.isNegative(); }
bool SoftDouble::isNegative() const { return impl.isNegative(); }
bool SoftFloat::isPositive() const { return impl.isPositive(); }
bool SoftDouble::isPositive() const { return impl.isPositive(); }
bool SoftFloat::isZero() const { return impl.isZero(); }
bool SoftDouble::isZero() const { return impl.isZero(); }
bool SoftFloat::isInfty() const { return impl.isInfty(); }
bool SoftDouble::isInfty() const { return impl.isInfty(); }

void SoftFloat::opposite() { impl.opposite(); }
void SoftDouble::opposite() { impl.opposite(); }

void SoftFloat::plusAssign(const SoftFloat& op1, Flags& flags) { impl.plusAssign(op1.impl,flags); }
void SoftDouble::plusAssign(const SoftDouble& op1, Flags& flags) { impl.plusAssign(op1.impl,flags); }
void SoftFloat::minusAssign(const SoftFloat& op1, Flags& flags) { impl.minusAssign(op1.impl,flags); }
void SoftDouble::minusAssign(const SoftDouble& op1, Flags& flags) { impl.minusAssign(op1.impl,flags); }
void SoftFloat::multAssign(const SoftFloat& op1, Flags& flags) { impl.multAssign(op1.impl,flags); }
void SoftDouble::multAssign(const SoftDouble& op1, Flags& flags) { impl.multAssign(op1.impl,flags); }
void SoftFloat::divAssign(const SoftFloat& op1, Flags& flags) { impl.divAssign(op1.impl,flags); }
void SoftDouble::divAssign(const SoftDouble& op1, Flags& flags) { impl.divAssign(op1.impl,flags); }

void SoftFloat::multAndAddAssign(const SoftFloat& a, const SoftFloat& b, Flags& flags) { impl.multAndAddAssign(a.impl, b.impl, flags); }
void SoftDouble::multAndAddAssign(const SoftDouble& a, const SoftDouble& b, Flags& flags) { impl.multAndAddAssign(a.impl, b.impl, flags); }

void SoftFloat::multAndSubAssign(const SoftFloat& a, const SoftFloat& b, Flags& flags) { impl.multAndSubAssign(a.impl, b.impl, flags); }
void SoftDouble::multAndSubAssign(const SoftDouble& a, const SoftDouble& b, Flags& flags) { impl.multAndSubAssign(a.impl, b.impl, flags); }

SoftFloat::ComparisonResult SoftFloat::compare( SoftFloat const& op1 ) const { return (SoftFloat::ComparisonResult)impl.compare( op1.impl ); }
SoftDouble::ComparisonResult SoftDouble::compare( SoftDouble const& op1 ) const { return (SoftDouble::ComparisonResult)impl.compare( op1.impl ); }

bool SoftFloat::isSNaN() const { return impl.isSNaN(); }
bool SoftDouble::isSNaN() const { return impl.isSNaN(); }
bool SoftFloat::isQNaN() const { return impl.isQNaN(); }
bool SoftDouble::isQNaN() const { return impl.isQNaN(); }
bool SoftFloat::isNaN() const { return impl.isNaN(); }
bool SoftDouble::isNaN() const { return impl.isNaN(); }
bool SoftFloat::isDenormalized() const { return impl.isDenormalized(); }
bool SoftDouble::isDenormalized() const { return impl.isDenormalized(); }

void SoftFloat::retrieveInteger(IntConversion& result, Flags& flags) const { impl.retrieveInteger(result, flags); }
void SoftDouble::retrieveInteger(IntConversion& result, Flags& flags) const { impl.retrieveInteger(result, flags); }


void SoftFloat::setQuiet() { impl.querySMantissa().setBitArray(impl.bitSizeMantissa()-1,1); }
void SoftDouble::setQuiet() { impl.querySMantissa().setBitArray(impl.bitSizeMantissa()-1,1); }
