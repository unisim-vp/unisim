/*
 *  Copyright (c) 2009,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */
 
#include <unisim/component/cxx/processor/tms320/register.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <sstream>
#include <string.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace tms320 {

Register::Register()
	: lo_write_mask(0xffffffff)
	, lo(0)
	, hi(0)
{
}

void Register::SetLoWriteMask(uint32_t _lo_write_mask)
{
	lo_write_mask = _lo_write_mask;
}

std::ostream& operator << (std::ostream& os, const Register& reg)
{
	std::stringstream sstr;

	sstr << "0x" << std::hex;
	sstr.fill('0');
	sstr.width(2);
	sstr << (unsigned int) reg.hi;
	sstr.width(8);
	sstr << reg.lo;
	return os << sstr.str();
}

void Register::SetFromShortFPFormat(uint16_t value)
{
	lo = value & (uint32_t)0x00000fff;
	lo = lo << 20;
	hi = (uint8_t)(((int16_t)value >> 12) & (uint16_t)0x00ff);
}

void Register::SetFromSinglePresicionFPFormat(uint32_t value)
{
	lo = value & (uint32_t)0x00ffffff;
	lo = lo << 8;
	hi = (uint8_t)((value >> 24) & (uint32_t)0x00ff);
}

void Register::Float(uint32_t value)
{
	lo = value;
	hi = 30;
	unsigned int count_nsb = 0; // counter of non significative bits
	bool neg = false;
	if (lo)
	{
		if (lo >= (uint32_t)0x80000000)
		{
			neg = true;
			count_nsb = unisim::util::arithmetic::CountLeadingZeros(~lo) - 1;
		}
		else
		{
			count_nsb = unisim::util::arithmetic::CountLeadingZeros(lo) - 1;
		}
		lo = lo << (count_nsb + 1);
		hi = 30 - count_nsb;
		if (neg)
		{
			lo = lo | (uint32_t)0x80000000;
		}
		else
		{
			lo = lo & ~(uint32_t)0x80000000;
		}
	}
	else
	{
		hi = (uint8_t)0x080;
	}
	
}

uint32_t Register::Fix(bool& overflow)
{
	overflow = false;

	// check for special cases of exponent to signal an overflow
	if ((int8_t)hi > 30)
	{
		overflow = true;
		// check if the sign is positive or negative
		// if positive return the biggest positive number
		// if negative return the biggest negative number
		if (lo & (uint32_t)0x80000000) // the sign is negative
		{
			return (uint32_t)0x80000000;
		}
		else // the sign is positve
		{
			return ~(uint32_t)0x80000000;
		}
	}

	// comput the shift value
	uint32_t shift = 31 - (int8_t)hi;
	// to avoid circular shift trunk it to 32
	if (shift > 32) shift = 32;
	
	// get the mantissa in a 64 bit format 
	// we need that because the mantissa once the sign is implied makes 33 bits
	uint64_t val;
	if (lo & (uint32_t)0x80000000)
	{
		val = (int64_t)-1;
		val = val & (lo & ~(uint32_t)0x80000000);
	}
	else
	{
		val = lo;
		val += (uint32_t)0x80000000;
	}
	
	// perform the shift on the mantissa and return the lowest 32 bits
	val = (int64_t)val >> shift;
	return (uint32_t)val;
}

void Register::Add(const Register& reg, bool& overflow, bool& underflow) 
{
	this->Add(this->hi, this->lo, reg.hi, reg.lo, overflow, underflow);
}

void Register::Add(uint16_t imm, bool& overflow, bool& underflow)
{
	uint8_t hi_b;
	uint32_t lo_b;

	hi_b = (uint8_t)(((int16_t)imm >> 12) & (uint16_t)0xff);
	lo_b = imm & (uint16_t)0x0fff;
	lo_b = lo_b << 20;

	this->Add(this->hi, this->lo, hi_b, lo_b, overflow, underflow);
}

void Register::Add(uint32_t imm, bool& overflow, bool& underflow)
{
	uint8_t hi_b;
	uint32_t lo_b;

	hi_b = imm >> 24;
	lo_b = imm & (uint32_t)0x00ffffff;
	lo_b = lo_b << 8;

	this->Add(this->hi, this->lo, hi_b, lo_b, overflow, underflow);
}

void Register::Add(const Register& reg_a, const Register& reg_b, bool& overflow, bool& underflow)
{
	this->Add(reg_a.hi, reg_a.lo, reg_b.hi, reg_b.lo, overflow, underflow);
}

void Register::Add(const Register& reg, uint32_t imm, bool& overflow, bool& underflow)
{
	uint8_t hi_b;
	uint32_t lo_b;

	hi_b = imm >> 24;
	lo_b = imm & (uint32_t)0x00ffffff;
	lo_b = lo_b << 8;

	this->Add(reg.hi, reg.lo, hi_b, lo_b, overflow, underflow);
}

void Register::Add(uint32_t imm, const Register& reg, bool& overflow, bool& underflow)
{
	this->Add(reg, imm, overflow, underflow);
}

void Register::Add(uint32_t imm_a, uint32_t imm_b, bool& overflow, bool& underflow)
{
	uint8_t hi_a;
	uint32_t lo_a;

	hi_a = imm_a >> 24;
	lo_a = imm_a & (uint32_t)0x00ffffff;
	lo_a = lo_a << 8;
	
	uint8_t hi_b;
	uint32_t lo_b;

	hi_b = imm_b >> 24;
	lo_b = imm_b & (uint32_t)0x00ffffff;
	lo_b = lo_b << 8;

	this->Add(hi_a, lo_a, hi_b, lo_b, overflow, underflow);
}

void Register::Sub(const Register& reg, bool& overflow, bool& underflow) 
{
	this->Sub(this->hi, this->lo, reg.hi, reg.lo, overflow, underflow);
}

void Register::Sub(uint16_t imm, bool& overflow, bool& underflow)
{
	uint8_t hi_b;
	uint32_t lo_b;

	hi_b = (uint8_t)(((int16_t)imm >> 12) & (uint16_t)0xff);
	lo_b = imm & (uint16_t)0x0fff;
	lo_b = lo_b << 20;

	this->Sub(this->hi, this->lo, hi_b, lo_b, overflow, underflow);
}

void Register::Sub(uint32_t imm, bool& overflow, bool& underflow)
{
	uint8_t hi_b;
	uint32_t lo_b;

	hi_b = imm >> 24;
	lo_b = imm & (uint32_t)0x00ffffff;
	lo_b = lo_b << 8;

	this->Sub(this->hi, this->lo, hi_b, lo_b, overflow, underflow);
}

void Register::Sub(const Register& reg_a, const Register& reg_b, bool& overflow, bool& underflow)
{
	this->Sub(reg_a.hi, reg_a.lo, reg_b.hi, reg_b.lo, overflow, underflow);
}

void Register::Sub(const Register& reg, uint32_t imm, bool& overflow, bool& underflow)
{
	uint8_t hi_b;
	uint32_t lo_b;

	hi_b = imm >> 24;
	lo_b = imm & (uint32_t)0x00ffffff;
	lo_b = lo_b << 8;

	this->Sub(reg.hi, reg.lo, hi_b, lo_b, overflow, underflow);
}

void Register::Sub(uint32_t imm, const Register& reg, bool& overflow, bool& underflow)
{
	uint8_t hi_a;
	uint32_t lo_a;

	this->Sub(hi_a, lo_a, reg.hi, reg.lo, overflow, underflow);
}

void Register::Sub(uint32_t imm_a, uint32_t imm_b, bool& overflow, bool& underflow)
{
	uint8_t hi_a;
	uint32_t lo_a;

	hi_a = imm_a >> 24;
	lo_a = imm_a & (uint32_t)0x00ffffff;
	lo_a = lo_a << 8;
	
	uint8_t hi_b;
	uint32_t lo_b;

	hi_b = imm_b >> 24;
	lo_b = imm_b & (uint32_t)0x00ffffff;
	lo_b = lo_b << 8;

	this->Sub(hi_a, lo_a, hi_b, lo_b, overflow, underflow);
}

void Register::Add(uint8_t hi_a, uint32_t lo_a, uint8_t hi_b, uint32_t lo_b, bool& overflow, bool& underflow)
{
	AddSub(true, hi_a, lo_a, hi_b, lo_b, overflow, underflow);
}

void Register::Sub(uint8_t hi_a, uint32_t lo_a, uint8_t hi_b, uint32_t lo_b, bool& overflow, bool& underflow)
{
	AddSub(false, hi_a, lo_a, hi_b, lo_b, overflow, underflow);
}

void Register::AddSub(bool is_add, uint8_t hi_a, uint32_t lo_a, uint8_t hi_b, uint32_t lo_b, bool& overflow, bool& underflow)
{
	int64_t ext_lo_a = 0;
	int64_t ext_lo_b = 0;
	overflow = false;
	underflow = false;

	// convert mantissas to their full representation (33-bit)
	if (lo_a & (uint32_t)0x80000000)
	{
		ext_lo_a = ~ext_lo_a;
		ext_lo_a = ext_lo_a & (uint64_t)(lo_a & ~(uint32_t)0x80000000);
	}
	else
		ext_lo_a = ext_lo_a & (uint64_t)(lo_a | (uint32_t)0x80000000);

	if (lo_b & (uint32_t)0x80000000)
	{
		ext_lo_b = ~ext_lo_b;
		ext_lo_b = ext_lo_b & (uint64_t)(lo_a & ~(uint32_t)0x80000000);
	}
	else
		ext_lo_b = ext_lo_b & (uint64_t)(lo_b | (uint32_t)0x80000000);
		
	// result takes the biggest exponent
	// and compute the difference of the exponent to align mantissas
	uint32_t diff_exp;
	if ((int8_t)hi_a <= (int8_t)hi_b)
	{
		this->hi = hi_b;
		diff_exp = (int8_t)hi_b - (int8_t)hi_a;
		if (diff_exp >= 64)
		{
			if (ext_lo_a < 0) ext_lo_a = -1;
			else ext_lo_a = 0;
		}
		else
			ext_lo_a = ext_lo_a >> diff_exp;
	}
	else
	{
		this->hi = hi_a;
		diff_exp = (int8_t)hi_a - (int8_t)hi_b;
		if (diff_exp >= 64)
		{
			if (ext_lo_b < 0) ext_lo_b = -1;
			else ext_lo_b = 0;
		}
		else
			ext_lo_b = ext_lo_b >> diff_exp;
	}

	// add the mantissas
	int64_t ext_lo_c = ext_lo_a;
	if (is_add) ext_lo_c += ext_lo_b;
	else ext_lo_c -= ext_lo_b;

	// check for special cases of the result mantissa
	// 1 - check mantissa is 0
	if (ext_lo_c == 0)
	{
		this->hi = (uint8_t)0xff;
	}
	else
	{
	// 2 - check for overflow
		if (ext_lo_c > 0 && (ext_lo_c >> 32) != 0)
		{
			// the mantissa is possitive, check how many bits we have to shift it
			//   to be an signed 33bits number
			ext_lo_c = ext_lo_c >> 1;
			if ((int32_t)this->hi + 1 > 127) 
			{
				overflow = true;
				this->hi = (uint8_t)0x7f;
			}
			else
				this->hi = this->hi + 1;
		}
		else
		{
			if (ext_lo_c < 0 && ~(ext_lo_c >> 32) != 1)
			{
				// the mantissa is negative, check how many bits we have to shift it
				//   to be a signed 33bits number
				ext_lo_c = (uint64_t)ext_lo_c >> 1;
				if ((int32_t)this->hi + 1 > 127) 
				{
					overflow = true;
					this->hi = (uint8_t)0x7f;
				}
				else
					this->hi = this->hi + 1;
			}
			else
			{
	// 3 - check if the result needs to be normalized
				if (ext_lo_c > 0 && (ext_lo_c & (uint64_t)0x80000000) == 0)
				{
					uint32_t count = unisim::util::arithmetic::CountLeadingZeros((uint64_t)ext_lo_c);
					count = count - 32;
					ext_lo_c = ext_lo_c << count;
					if ((int32_t)this->hi - count < -128)
					{
						underflow = true;
						this->hi = (uint8_t)0xff;
					}
					else
						this->hi = this->hi - count;
				}
				else
				{
					if (ext_lo_c < 0 && (ext_lo_c & (uint64_t)0x80000000 != 0))
					{
						uint32_t count = unisim::util::arithmetic::CountLeadingZeros(~(uint64_t)ext_lo_c);
						count = count - 32;
						ext_lo_c = ext_lo_c << count;
						if ((int32_t)this->hi - count < -128)
						{
							underflow = true;
							this->hi = (uint8_t)0xff;
						}
						else
							this->hi = this->hi - count;
					}
				}
			}
		}
	}

	// check for special cases of the result exponent
	// 1 - exponent overflow
	if (overflow)
	{
		if (ext_lo_c > 0)
		{
			ext_lo_c = (uint64_t)0xffffffff;
		}
		else // ext_lo_c < 0
		{
			ext_lo_c = ~(uint64_t)0xffffffff;
		}
	}
	else
	{
	// 2 - exponent underflow
		if (underflow)
		{
			ext_lo_c = 0;
		}
		else
		{
	// 3 - exponent in range
			// nothing to do
		}
	}

	// set the mantissa into the register
	this->lo = (uint32_t)((ext_lo_c & ~(uint64_t)0x80000000) | ((ext_lo_c >> 1) & (uint64_t)0x80000000));
}

void Register::Mpy(const Register& reg, bool& overflow, bool& underflow)
{
	this->Mpy(this->hi, this->lo, reg.GetHi(), reg.GetLo(), overflow, underflow);
}

void Register::Mpy(uint16_t imm, bool& overflow, bool& underflow)
{
	uint8_t hi_b;
	uint32_t lo_b;

	hi_b = (uint8_t)(((int16_t)imm >> 12) & (uint16_t)0xff);
	lo_b = imm & (uint16_t)0x0fff;
	lo_b = lo_b << 20;

	this->Mpy(this->hi, this->lo, hi_b, lo_b, overflow, underflow);
}

void Register::Mpy(uint32_t imm, bool& overflow, bool& underflow)
{
	uint8_t hi_b;
	uint32_t lo_b;

	hi_b = imm >> 24;
	lo_b = imm & (uint32_t)0x00ffffff;
	lo_b = lo_b << 8;

	this->Mpy(this->hi, this->lo, hi_b, lo_b, overflow, underflow);
}

void Register::Mpy(const Register& reg_a, const Register& reg_b, bool& overflow, bool& underflow)
{
	this->Mpy(reg_a.hi, reg_a.lo, reg_b.hi, reg_b.lo, overflow, underflow);
}

void Register::Mpy(const Register& reg, uint32_t imm, bool& overflow, bool& underflow)
{
	uint8_t hi_b;
	uint32_t lo_b;

	hi_b = imm >> 24;
	lo_b = imm & (uint32_t)0x00ffffff;
	lo_b = lo_b << 8;

	this->Mpy(reg.hi, reg.lo, hi_b, lo_b, overflow, underflow);
}

void Register::Mpy(uint32_t imm, const Register& reg, bool& overflow, bool& underflow)
{
	this->Mpy(reg, imm, overflow, underflow);
}

void Register::Mpy(uint32_t imm_a, uint32_t imm_b, bool& overflow, bool& underflow)
{
	uint8_t hi_a;
	uint32_t lo_a;

	hi_a = imm_a >> 24;
	lo_a = imm_a & (uint32_t)0x00ffffff;
	lo_a = lo_a << 8;
	
	uint8_t hi_b;
	uint32_t lo_b;

	hi_b = imm_b >> 24;
	lo_b = imm_b & (uint32_t)0x00ffffff;
	lo_b = lo_b << 8;

	this->Mpy(hi_a, lo_a, hi_b, lo_b, overflow, underflow);
}

void Register::Mpy(uint8_t hi_a, uint32_t lo_a, uint8_t hi_b, uint32_t lo_b, bool& overflow, bool& underflow)
{
	int64_t ext_lo_a;
	int64_t ext_lo_b;
	overflow = false;
	underflow = false;

	// convert mantissas to their full representation (33-bit)
	if (lo_a & (uint32_t)0x80000000)
	{
		ext_lo_a = ~ext_lo_a;
		ext_lo_a = ext_lo_a & (uint64_t)(lo_a & ~(uint32_t)0x80000000);
	}
	else
		ext_lo_a = ext_lo_a & (uint64_t)(lo_a | (uint32_t)0x80000000);

	if (lo_b & (uint32_t)0x80000000)
	{
		ext_lo_b = ~ext_lo_b;
		ext_lo_b = ext_lo_b & (uint64_t)(lo_a & ~(uint32_t)0x80000000);
	}
	else
		ext_lo_b = ext_lo_b & (uint64_t)(lo_b | (uint32_t)0x80000000);

	// convert the mantissas to their 25-bit representation
	ext_lo_a = ext_lo_a >> 8;
	ext_lo_b = ext_lo_b >> 8;

	// compute the result exponent
	int32_t exp_c = (int32_t)((int8_t)hi_a) + (int32_t)((int8_t)hi_b);
	
	// multiply the mantissas
	int64_t ext_lo_c = ext_lo_a * ext_lo_b;

	// test for special cases of the mantissa
	// 1 - shift by one or two the result
	int64_t tmp = ext_lo_c;
	unsigned int shift = 0;
	tmp = tmp >> 46;
	if (ext_lo_c < 0) tmp = ~tmp;
	if (tmp != 1) 
	{
		tmp = tmp >> 1;
		shift += 1;
	}
	if (tmp != 1) 
	{
		tmp = tmp >> 1;
		shift += 1;
	}
	ext_lo_c = ext_lo_c >> shift;
	exp_c += shift;
	// 2 - dispose of extra bits
	ext_lo_c = ext_lo_c >> 15;
	// 3 - check that the result is not 0
	if (ext_lo_c == 0)
		exp_c = -128;

	// test for special cases of the result exponent
	// 1 - exponent overflow
	if (exp_c > 127)
	{
		overflow = true;
		exp_c = 127;
		if (ext_lo_c > 0)
		{
			ext_lo_c = (uint64_t)0xffffffff;
		}
		else // ext_lo_c < 0
		{
			ext_lo_c = ~(uint64_t)0xffffffff;
		}
	}
	else
	{
	// 2 - exponent underflow
		if (exp_c < -128)
		{
			underflow = true;
			exp_c = -128;
			ext_lo_c = 0;
		}
		else
		{
	// 3 - exponent in range
			// nothing to do
		}
	}

	// set the mantissa and exponent into the register
	this->lo = (uint32_t)((ext_lo_c & ~(uint64_t)0x80000000) | ((ext_lo_c >> 1) & (uint64_t)0x80000000));
	this->hi = (uint8_t)((int8_t)exp_c);
}

RegisterDebugInterface::RegisterDebugInterface(const char *_name, unisim::component::cxx::processor::tms320::Register *_reg, bool _extended_precision)
	: name(_name)
	, reg(_reg)
	, extended_precision(_extended_precision)
{
}

RegisterDebugInterface::~RegisterDebugInterface()
{
}

const char *RegisterDebugInterface::GetName() const
{
	return name.c_str();
}

void RegisterDebugInterface::GetValue(void *buffer) const
{
	uint32_t lo = reg->GetLo();

	if(extended_precision)
	{
		// Packing lo and hi into the buffer
		uint8_t hi = reg->GetHi();
#if BYTE_ORDER == LITTLE_ENDIAN
		memcpy(buffer, &lo, sizeof(lo));
		memcpy((char *) buffer + sizeof(lo), &hi, sizeof(hi));
#else
		memcpy(buffer, &hi, sizeof(hi));
		memcpy((char *) buffer + sizeof(hi), &lo, sizeof(lo));
#endif
	}
	else
	{
		memcpy(buffer, &lo, sizeof(lo));
	}
}

void RegisterDebugInterface::SetValue(const void *buffer)
{
	uint32_t lo;

	if(extended_precision)
	{
		uint8_t hi;
		// Unpacking the buffer into lo and hi
#if BYTE_ORDER == LITTLE_ENDIAN
		memcpy(&lo, buffer, sizeof(lo));
		memcpy(&hi, (char *) buffer + sizeof(lo), sizeof(hi));
#else
		memcpy(&hi, buffer, sizeof(hi));
		memcpy(&lo, (char *) buffer + sizeof(hi), sizeof(lo));
#endif
		reg->SetHi(hi);
	}
	else
	{
		memcpy(&lo, buffer, sizeof(lo));
	}

	reg->SetLo(lo);
}

int RegisterDebugInterface::GetSize() const
{
	return extended_precision ? sizeof(uint32_t) + sizeof(uint8_t) : sizeof(uint32_t);
}

RegisterBitFieldDebugInterface::RegisterBitFieldDebugInterface(const char *_name, unisim::component::cxx::processor::tms320::Register *_reg, unsigned int _bit_offset, unsigned int _bit_size)
	: name(_name)
	, reg(_reg)
	, bit_offset(_bit_offset)
	, bit_size(_bit_size > 32 ? 32 : _bit_size)
{
}

RegisterBitFieldDebugInterface::~RegisterBitFieldDebugInterface()
{
}

const char *RegisterBitFieldDebugInterface::GetName() const
{
	return name.c_str();
}

void RegisterBitFieldDebugInterface::GetValue(void *buffer) const
{
	uint32_t lo = reg->GetLo();
	uint32_t value = (lo >> bit_offset) & ((1 << bit_size) - 1);

	if(bit_size <= 8) *(uint8_t *) buffer = value; else
	if(bit_size <= 16) *(uint16_t *) buffer = value; else *(uint32_t *) buffer = value;
}

void RegisterBitFieldDebugInterface::SetValue(const void *buffer)
{
	uint32_t value;

	if(bit_size <= 8) value = *(uint8_t *) buffer; else
	if(bit_size <= 16) value = *(uint16_t *) buffer; else value = *(uint32_t *) buffer;

	reg->SetLo((reg->GetLo() & ~(((1 << bit_size) - 1) << bit_offset)) | (value << bit_offset));
}

int RegisterBitFieldDebugInterface::GetSize() const
{
	return (bit_size + 7) / 8;
}

} // end of namespace tms320
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

