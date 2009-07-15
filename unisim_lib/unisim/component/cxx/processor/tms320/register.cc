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
#include <iostream>
#include <string.h>

//#define __DEBUG_TMS320C3X_REGISTER__

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
		sstr << " ";
		sstr.width(8);
		sstr << reg.lo;
		return os << sstr.str();
	}
	
	uint32_t Register::GetLo(uint16_t value)
	{
		if (value & (uint16_t)0xf000 == (uint16_t)0x8000)
			return 0;
		return (((uint32_t)value) & (uint32_t)0x00000fff) << 20;
	}
	
	uint8_t Register::GetHi(uint16_t value)
	{
		if (value & (uint16_t)0xf000 == (uint16_t)0x8000)
			return (uint8_t)0x80;
//		if (value == (uint16_t)0x8000)
//			return (uint8_t)0x80;
		return (uint8_t)(((int16_t)value >> 12) & (uint16_t)0x00ff);
	}
	
	void Register::SetFromShortFPFormat(uint16_t value)
	{
		this->SetLo(GetLo(value));
		this->SetHi(GetHi(value));
		// lo = value & (uint32_t)0x00000fff;
		// lo = lo << 20;
		// hi = (uint8_t)(((int16_t)value >> 12) & (uint16_t)0x00ff);
	}
	
	uint32_t Register::GetLo(uint32_t value)
	{
		if (value & (uint32_t)0xff000000 == (uint32_t)0x80000000) return 0;
		return ((uint32_t)value & (uint32_t)0x00ffffff) << 8;
	}
	
	uint8_t Register::GetHi(uint32_t value)
	{
		return (uint8_t)((value >> 24) & (uint32_t)0x00ff);
	}
	
	void Register::SetFromSinglePresicionFPFormat(uint32_t value)
	{
		// the 0 value is converted seamlessly from single presicion to extended
		//   so no need to check the input value
		this->SetLo(GetLo(value));
		this->SetHi(GetHi(value));
		// lo = value & (uint32_t)0x00ffffff;
		// lo = lo << 8;
		// hi = (uint8_t)((value >> 24) & (uint32_t)0x00ff);
	}
	
	uint32_t Register::GetSinglePrecisionFPFormat() 
	{
		uint32_t t_hi;
		uint32_t t_lo;
		
		t_hi = GetHi();
		t_hi = t_hi << 24;
		t_lo = GetLo();
		t_lo = t_lo & ~((uint32_t)0x00ff);
		t_lo = t_lo >> 8;
		return t_hi + t_lo;
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
		
		int64_t ext_lo;
		// convert mantissas to their full representation (33-bit)
		if (lo & (uint32_t)0x80000000)
			ext_lo = (int64_t)((int32_t)lo) & ~(uint64_t)0x80000000;
		else
			ext_lo = (uint64_t)(lo | (uint32_t)0x80000000);
		
		// perform the shift on the mantissa and return the lowest 32 bits
		ext_lo = (int64_t)ext_lo >> shift;
		return (uint32_t)ext_lo;
	}
	
	void Register::Abs(const Register& reg, bool& overflow)
	{
		this->Abs(reg.GetHi(), reg.GetLo(), overflow);
	}
	
	void Register::Abs(uint16_t imm, bool& overflow)
	{
		this->Abs(GetHi(imm), GetLo(imm), overflow);
	}
	
	void Register::Abs(uint32_t imm, bool& overflow)
	{
		this->Abs(GetHi(imm), GetLo(imm), overflow);
	}
	
	void Register::Add(const Register& reg, bool& overflow, bool& underflow) 
	{
		this->Add(this->GetHi(), this->GetLo(), reg.GetHi(), reg.GetLo(), overflow, underflow);
	}
	
	void Register::Add(uint16_t imm, bool& overflow, bool& underflow)
	{
		this->Add(this->GetHi(), this->GetLo(), GetHi(imm), GetLo(imm), overflow, underflow);
	}
	
	void Register::Add(uint32_t imm, bool& overflow, bool& underflow)
	{
		this->Add(this->hi, this->lo, GetHi(imm), GetLo(imm), overflow, underflow);
	}
	
	void Register::Add(const Register& reg_a, const Register& reg_b, bool& overflow, bool& underflow)
	{
		this->Add(reg_a.GetHi(), reg_a.GetLo(), reg_b.GetHi(), reg_b.GetLo(), overflow, underflow);
	}
	
	void Register::Add(const Register& reg, uint32_t imm, bool& overflow, bool& underflow)
	{
		this->Add(reg.GetHi(), reg.GetLo(), GetHi(), GetLo(), overflow, underflow);
	}
	
	void Register::Add(uint32_t imm, const Register& reg, bool& overflow, bool& underflow)
	{
		this->Add(reg, imm, overflow, underflow);
	}
	
	void Register::Add(uint32_t imm_a, uint32_t imm_b, bool& overflow, bool& underflow)
	{
		this->Add(GetHi(imm_a), GetLo(imm_a), GetHi(imm_b), GetLo(imm_b), overflow, underflow);
	}
	
	void Register::Sub(const Register& reg, bool& overflow, bool& underflow) 
	{
		this->Sub(this->GetHi(), this->GetLo(), reg.hi, reg.lo, overflow, underflow);
	}
	
	void Register::Sub(uint16_t imm, bool& overflow, bool& underflow)
	{
		this->Sub(this->GetHi(), this->GetLo(), GetHi(imm), GetLo(imm), overflow, underflow);
	}
	
	void Register::Sub(uint32_t imm, bool& overflow, bool& underflow)
	{
		this->Sub(this->GetHi(), this->GetLo(), GetHi(imm), GetLo(imm), overflow, underflow);
	}
	
	void Register::Sub(const Register& reg_a, const Register& reg_b, bool& overflow, bool& underflow)
	{
		this->Sub(reg_a.GetHi(), reg_a.GetLo(), reg_b.GetHi(), reg_b.GetLo(), overflow, underflow);
	}
	
	void Register::Sub(const Register& reg, uint16_t imm, bool& overflow, bool& underflow)
	{
		this->Sub(reg.GetHi(), reg.GetLo(), GetHi(imm), GetLo(imm), overflow, underflow);
	}
	
	void Register::Sub(const Register& reg, uint32_t imm, bool& overflow, bool& underflow)
	{
		this->Sub(reg.GetHi(), reg.GetLo(), GetHi(imm), GetLo(imm), overflow, underflow);
	}
	
	void Register::Sub(uint16_t imm, const Register& reg, bool& overflow, bool& underflow)
	{
		this->Sub(GetHi(imm), GetLo(imm), reg.GetHi(), reg.GetLo(), overflow, underflow);
	}
	
	void Register::Sub(uint32_t imm, const Register& reg, bool& overflow, bool& underflow)
	{
		this->Sub(GetHi(imm), GetLo(imm), reg.GetHi(), reg.GetLo(), overflow, underflow);
	}
	
	void Register::Sub(uint32_t imm_a, uint32_t imm_b, bool& overflow, bool& underflow)
	{
		this->Sub(GetHi(imm_a), GetLo(imm_a), GetHi(imm_b), GetLo(imm_b), overflow, underflow);
	}
	
	void Register::Neg(const Register& reg, bool& overflow, bool& underflow)
	{
		this->Sub((uint8_t)0x80, 0, reg.GetHi(), reg.GetLo(), overflow, underflow);
	}
	
	void Register::Neg(uint16_t imm, bool& overflow, bool& underflow)
	{
		this->Sub((uint8_t)0x80, 0, GetHi(imm), GetLo(imm), overflow, underflow);
	}
	
	void Register::Neg(uint32_t imm, bool& overflow, bool& underflow)
	{
		this->Sub((uint8_t)0x80, 0, GetHi(imm), GetLo(imm), overflow, underflow);
	}
	
	void Register::Norm(const Register& reg, bool& underflow)
	{
		this->Norm(reg.GetHi(), reg.GetLo(), underflow);
	}
	
	void Register::Norm(uint16_t imm, bool& underflow)
	{
		this->Norm(GetHi(imm), GetLo(imm), underflow);
	}
	
	void Register::Norm(uint32_t imm, bool& underflow)
	{
		this->Norm(GetHi(imm), GetLo(imm), underflow);
	}
	
	void Register::Mpy(const Register& reg, bool& overflow, bool& underflow)
	{
		this->Mpy(this->hi, this->lo, reg.GetHi(), reg.GetLo(), overflow, underflow);
	}
	
	void Register::Mpy(uint16_t imm, bool& overflow, bool& underflow)
	{
		this->Mpy(this->hi, this->lo, GetHi(imm), GetLo(imm), overflow, underflow);
	}
	
	void Register::Mpy(uint32_t imm, bool& overflow, bool& underflow)
	{
		this->Mpy(this->hi, this->lo, GetHi(imm), GetLo(imm), overflow, underflow);
	}
	
	void Register::Mpy(const Register& reg_a, const Register& reg_b, bool& overflow, bool& underflow)
	{
		this->Mpy(reg_a.hi, reg_a.lo, reg_b.hi, reg_b.lo, overflow, underflow);
	}
	
	void Register::Mpy(const Register& reg, uint32_t imm, bool& overflow, bool& underflow)
	{
		this->Mpy(reg.hi, reg.lo, GetHi(imm), GetLo(imm), overflow, underflow);
	}
	
	void Register::Mpy(uint32_t imm, const Register& reg, bool& overflow, bool& underflow)
	{
		this->Mpy(reg, imm, overflow, underflow);
	}
	
	void Register::Mpy(uint32_t imm_a, uint32_t imm_b, bool& overflow, bool& underflow)
	{
		this->Mpy(GetHi(imm_a), GetLo(imm_a), GetHi(imm_b), GetHi(imm_b), overflow, underflow);
	}
	
	void Register::Rnd(const Register& reg, bool& overflow, bool& underflow)
	{
		this->Rnd(reg.GetHi(), reg.GetLo(), overflow, underflow);
	}
	
	void Register::Rnd(uint16_t imm, bool& overflow, bool& underflow)
	{
		this->Rnd(GetHi(imm), GetLo(imm), overflow, underflow);
	}
	
	void Register::Rnd(uint32_t imm, bool& overflow, bool& underflow)
	{
		this->Rnd(GetHi(imm), GetLo(imm), overflow, underflow);
	}

	void Register::Abs(uint8_t hi_a, uint32_t lo_a, bool& overflow)
	{
		// if the exponent value is -128, then the mantissa should be 0
		//   to represent the 0.0 value
		if (hi_a == (uint8_t)0x80)
		{
			this->SetHi((uint8_t)0x80);
			this->SetLo(0);
			return;
		}
		overflow = false;
		if (hi_a == (uint8_t)0x7f && lo_a == (uint32_t)0x80000000)
		{
			this->SetHi((uint8_t)0x7f);
			this->SetLo((uint32_t)0x7fffffff);
			overflow = true;
			return;
		}
		// if the value is negative, then the abs operation can be performed
		if ((int32_t)lo_a < 0)
		{
			this->SetHi(hi_a);
			this->SetLo((~lo_a) + 1); // absolute value of the mantissa
			return;
		}
		// the value is positive, simply copy exponent and mantissa
		this->SetHi(hi_a);
		this->SetLo(lo_a);
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
		
#ifdef __DEBUG_TMS320C3X_REGISTER__
		std::cerr 
		<< "1- add = " << is_add << std::endl
		<< "   hi_a = 0x" << std::hex << (unsigned int)hi_a << "\t lo_a = 0x" << (unsigned int)lo_a << std::endl
		<< "   hi_b = 0x"             << (unsigned int)hi_b << "\t lo_b = 0x" << (unsigned int)lo_b << std::dec << std::endl;
#endif

		// convert mantissas to their full representation (33-bit)
		// when the exponent is 0x80 (-128) the mantissa should be considered to contain 0, whatever its real value is
		if (hi_a == (uint8_t)0x80)
			ext_lo_a = 0;
		else
			if (lo_a & (uint32_t)0x80000000)
				ext_lo_a = (int64_t)((int32_t)lo_a) & ~(uint64_t)0x80000000;
			else
				ext_lo_a = (uint64_t)(lo_a | (uint32_t)0x80000000);
		
		if (hi_b == (uint8_t)0x80)
			ext_lo_b = 0;
		else
			if (lo_b & (uint32_t)0x80000000)
				ext_lo_b = (int64_t)((int32_t)lo_b) & ~(uint64_t)0x80000000;
			else
				ext_lo_b = (uint64_t)(lo_b | (uint32_t)0x80000000);
		
#ifdef __DEBUG_TMS320C3X_REGISTER__
	std::cerr << std::hex
		<< "2- hi_a = 0x" << (unsigned int)hi_a << "\t ext_lo_a = 0x" << (unsigned long long int)ext_lo_a << std::endl
		<< "   hi_b = 0x" << (unsigned int)hi_b << "\t ext_lo_b = 0x" << (unsigned long long int)ext_lo_b << std::dec << std::endl;
#endif										
		// result takes the biggest exponent
		// and compute the difference of the exponent to align mantissas
		uint32_t diff_exp;
		if ((int8_t)hi_a <= (int8_t)hi_b)
		{
			this->hi = hi_b;
			diff_exp = (int32_t)(int8_t)hi_b - (int32_t)(int8_t)hi_a;
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
			diff_exp = (int32_t)(int8_t)hi_a - (int32_t)(int8_t)hi_b;
			if (diff_exp >= 64)
			{
				if (ext_lo_b < 0) ext_lo_b = -1;
				else ext_lo_b = 0;
			}
			else
				ext_lo_b = ext_lo_b >> diff_exp;
		}
		
#ifdef __DEBUG_TMS320C3X_REGISTER__
	std::cerr
		<< "3- diff_exp = " << (int)diff_exp << std::endl << std::hex
		<< "   hi_a = 0x" << (unsigned int)hi_a << "\t ext_lo_a = 0x" << (unsigned long long int)ext_lo_a << std::endl
		<< "   hi_b = 0x" << (unsigned int)hi_b << "\t ext_lo_b = 0x" << (unsigned long long int)ext_lo_b << std::endl
		<< "   hi   = 0x" << (unsigned int)this->GetHi() << std::dec << std::endl;
#endif								
		// add the mantissas
		int64_t ext_lo_c = ext_lo_a;
		if (is_add) ext_lo_c += ext_lo_b;
		else ext_lo_c -= ext_lo_b;
		
#ifdef __DEBUG_TMS320C3X_REGISTER__
		std::cerr
		<< "4- diff_exp = " << (int)diff_exp << std::endl << std::hex
		<< "   hi_a = 0x" << (unsigned int)hi_a          << "\t ext_lo_a = 0x" << (unsigned long long int)ext_lo_a << std::endl
		<< "   hi_b = 0x" << (unsigned int)hi_b          << "\t ext_lo_b = 0x" << (unsigned long long int)ext_lo_b << std::endl
		<< "   hi   = 0x" << (unsigned int)this->GetHi() << "\t ext_lo_c = 0x" << (unsigned long long int)ext_lo_c << std::dec << std::endl;
#endif										
		// check for special cases of the result mantissa
		// 1 - check mantissa is 0
		if (ext_lo_c == 0)
		{
			this->SetHi((uint8_t)0x80);
		}
		else
		{
#ifdef __DEBUG_TMS320C3X_REGISTER__
			std::cerr
			<< "5- ext_lo_c = 0x" << (unsigned long long int)ext_lo_c << std::endl
			<< "   ext_lo_c > 0 --> " << (ext_lo_c > 0) << std::endl
			<< "   ext_lo_c >> 32 --> 0x" << (unsigned long long int)(ext_lo_c >> 32) << std::endl;
#endif
			// 2 - check for overflow
			if ((ext_lo_c > 0) && ((ext_lo_c >> 32) != 0))
			{
#ifdef __DEBUG_TMS320C3X_REGISTER__
				std::cerr
				<< "6- overflow checking" << std::endl;
#endif
				// the mantissa is positive, check how many bits we have to shift it
				//   to be an signed 33bits number
				ext_lo_c = ext_lo_c >> 1;
				if (((int32_t)(int8_t)this->GetHi()) + 1 > 127) 
				{
					overflow = true;
					this->SetHi((uint8_t)0x7f);
				}
				else
					this->SetHi(this->GetHi() + 1);
			}
			else
			{
				if ((ext_lo_c < 0) && (~(ext_lo_c >> 32) != 0))
				{
					// the mantissa is negative, check how many bits we have to shift it
					//   to be a signed 33bits number
					ext_lo_c = (uint64_t)ext_lo_c >> 1;
					if ((int32_t)(int8_t)this->GetHi() + 1 > 127) 
					{
						overflow = true;
						this->SetHi((uint8_t)0x7f);
					}
					else
						this->SetHi(this->GetHi() + 1);
				}
				else
				{
#ifdef __DEBUG_TMS320C3X_REGISTER__
				std::cerr << "5- normalizing" << std::endl;
#endif					
					// 3 - check if the result needs to be normalized
					if (ext_lo_c > 0 && (ext_lo_c & (uint64_t)0x80000000) == 0)
					{
						uint32_t count = unisim::util::arithmetic::CountLeadingZeros((uint64_t)ext_lo_c);
						count = count - 32;
						ext_lo_c = ext_lo_c << count;
						if ((int32_t)((int32_t)this->GetHi() - count) < -128)
						{
							underflow = true;
							this->SetHi((uint8_t)0x80);
						}
						else
							this->SetHi(this->GetHi() - count);
					}
					else
					{
#ifdef __DEBUG_TMS320C3X_REGISTER__
						std::cerr 
						<< "5.1- normalizing neg" << std::endl
						<< "   hi   = 0x" << std::hex << (unsigned int)this->GetHi() << "\t ext_lo_c = 0x" << (unsigned long long)ext_lo_c << std::dec << std::endl
						<< "   -> 0x" << std::hex << (ext_lo_c & (uint64_t)0x80000000) << std::dec
						<< std::endl;
#endif						
						if (ext_lo_c < 0 && ((ext_lo_c & (uint64_t)0x80000000) != 0))
						{
							uint32_t count = unisim::util::arithmetic::CountLeadingZeros(~(uint64_t)ext_lo_c);

							count = count - 32;
							ext_lo_c = ext_lo_c << count;
							if ((int32_t)((int32_t)this->hi - count) < -128)
							{
								underflow = true;
								this->SetHi((uint8_t)0xff);
							}
							else
								this->SetHi(this->GetHi() - count);
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
		this->SetLo((uint32_t)((ext_lo_c & ~(uint64_t)0x80000000) 
							   | ((ext_lo_c >> 1) & 
								  (uint64_t)0x80000000)));
	}
	
	void Register::Mpy(uint8_t hi_a, uint32_t lo_a, uint8_t hi_b, uint32_t lo_b, bool& overflow, bool& underflow)
	{
		int64_t ext_lo_a = 0;
		int64_t ext_lo_b = 0;
		overflow = false;
		underflow = false;
		
		// convert mantissas to their full representation (33-bit)
		// when the exponent is 0x80 (-128) the mantissa should be considered to contain 0, whatever its real value is
		if (hi_a == (uint8_t)0x80)
			ext_lo_a = 0;
		else
			if (lo_a & (uint32_t)0x80000000)
				ext_lo_a = (int64_t)((int32_t)lo_a) & ~(uint64_t)0x80000000;
			else
				ext_lo_a = (uint64_t)(lo_a | (uint32_t)0x80000000);
		
		if (hi_b == (uint8_t)0x80)
			ext_lo_b = 0;
		else
			if (lo_b & (uint32_t)0x80000000)
				ext_lo_b = (int64_t)((int32_t)lo_b) & ~(uint64_t)0x80000000;
			else
				ext_lo_b = (uint64_t)(lo_b | (uint32_t)0x80000000);
		
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
	
	void Register::Norm(uint8_t hi_a, uint32_t lo_a, bool& underflow)
	{
		underflow = false;
		
		if (lo_a == 0)
		{
			this->SetLo(0);
			this->SetHi((uint8_t)0xff);
			return;
		}
		
		uint32_t man = lo_a; // sign extend the mantissa
		int32_t exp = (int8_t)hi_a; // sign extend the exponent
		uint32_t count = 0;
		
		if ((int32_t)lo_a < 0)
			count = unisim::util::arithmetic::CountLeadingZeros(~lo_a);
		else
			count = unisim::util::arithmetic::CountLeadingZeros(lo_a);
		
		man = man << count;
		exp = exp - count;
		if (exp < -128)
		{
			exp = -128;
			underflow = true;
		}
		this->SetHi((uint32_t)exp);
		this->SetLo(man ^ (uint32_t)0x80000000);
	}
	
	void Register::Rnd(uint8_t hi_a, uint32_t lo_a, bool& overflow, bool& underflow)
	{
		overflow = false;
		underflow = false;
		
		// check when src = 0
		if (lo_a == 0 && hi_a == (uint8_t)0x80)
		{
			underflow = true;
			this->SetHi(hi_a);
			this->SetLo(lo_a);
			return;
		}
		
		// create a fake register (hi_b and ext_lo_b) to perform an addition of the input
		//   with:
		//           signed(hi_a) - 24
		//     1 x 2
		int64_t ext_lo_a = 0;
		int64_t ext_lo_b = (uint32_t)0x80000000;
		ext_lo_b = ext_lo_b >> 24;
		// convert mantissas to their full representation (33-bit)
		if (lo_a & (uint32_t)0x80000000)
			ext_lo_a = (int64_t)((int32_t)lo_a) & ~(uint64_t)0x80000000;
		else
			ext_lo_a = (uint64_t)(lo_a | (uint32_t)0x80000000);
		
		
		// add the mantissas
		int64_t ext_lo_c = ext_lo_a + ext_lo_b;
		
		// set the exponent result
		this->SetHi(hi_a);

		// check for special cases of the result mantissa
		// 1 - mantissa is 0
		if (ext_lo_c == 0)
		{
			ext_lo_c = 0;
			this->SetHi((uint8_t)0x80);
		}
		else
		{
			// 2 - mantissa overflow
			if (ext_lo_c > 0 && (ext_lo_c >> 32) != 0)
			{
				// the mantissa is possitive, check how many bits we have to shift it
				//   to be a signed 33bits number
				ext_lo_c = ext_lo_c >> 1;
				if ((int32_t)this->GetHi() + 1 > 127) 
				{
					overflow = true;
					this->SetHi((uint8_t)0x7f);
				}
				else
					this->SetHi(hi_a + 1);
			}
			else
			{
				if (ext_lo_c < 0 && ~(ext_lo_c >> 32) != 1)
				{
					// the mantissa is negative, check how many bits we have to shift it
					//   to be a signed 33bits number
					ext_lo_c = (uint64_t)ext_lo_c >> 1;
					if ((int32_t)this->GetHi() + 1 > 127) 
					{
						overflow = true;
						this->SetHi((uint8_t)0x7f);
					}
					else
						this->SetHi(hi_a + 1);
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
		
		// set the mantissa into the register
		this->SetLo((uint32_t)((ext_lo_c & ~(uint64_t)0x80000000) 
							   | ((ext_lo_c >> 1) & 
								  (uint64_t)0x80000000)));
		this->SetLo(this->GetLo() & (uint32_t)0xffffff00);
	}
	
	void Register::LoadExp(const Register& reg)
	{
		if (reg.GetHi() == 0x80)
			this->SetLo(0);
		this->SetHi(reg.GetHi());
	}
	
	void Register::LoadExp(uint32_t value)
	{
		if (GetHi(value) == 0x80)
			this->SetLo(0);
		this->SetHi(GetHi(value));
	}
	
	void Register::LoadExp(uint16_t value)
	{
		if (GetHi(value) == 0x80)
			this->SetLo(0);
		this->SetHi(GetHi(value));
	}
	
	void Register::LoadMan(const Register& reg)
	{
		this->SetLo(reg.GetLo());
	}
	
	void Register::LoadMan(uint32_t value)
	{
		// TOCHECK: the documentation states that the memory value
		//   doesn't need to be interpreted as a float, but that all
		//   the value is loaded into the register mantissa (see LDM instruction)
		this->SetLo(value);
	}
	
	void Register::LoadMan(uint16_t value)
	{
		this->SetLo(GetLo(value));
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
		
		if(bit_size <= 8) *(uint8_t *) buffer = value; 
		else
			if(bit_size <= 16) *(uint16_t *) buffer = value; 
			else *(uint32_t *) buffer = value;
	}
	
	void RegisterBitFieldDebugInterface::SetValue(const void *buffer)
	{
		uint32_t value;
		
		if(bit_size <= 8) value = *(uint8_t *) buffer; 
		else
			if(bit_size <= 16) value = *(uint16_t *) buffer; 
			else value = *(uint32_t *) buffer;
		
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

