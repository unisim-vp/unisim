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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#include <data_types/integer/int_base.h>
#include <data_types/integer/int_bitref.h>
#include <data_types/integer/int_subref.h>
#include <data_types/util.h>

namespace sc_dt {

///////////////////////////////// definition //////////////////////////////////

// Constructors
sc_int_base::sc_int_base(int w)
	: value(0)
	, len(w)
{
}

sc_int_base::sc_int_base(int_type v, int w)
	: value(v)
	, len(w)
{
}

sc_int_base::sc_int_base(const sc_int_base& a)
	: value(a.value)
	, len(a.len)
{
}

sc_int_base::sc_int_base(const sc_int_subref_r& a)
{
}

sc_int_base::sc_int_base(const sc_signed& a)
{
}

sc_int_base::sc_int_base(const sc_unsigned& a)
{
}

sc_int_base::sc_int_base(const sc_bv_base& v)
{
}

sc_int_base::sc_int_base(const sc_lv_base& v)
{
}

sc_int_base::sc_int_base(const sc_uint_subref_r& v)
{
}

sc_int_base::sc_int_base(const sc_signed_subref_r& v)
{
}

sc_int_base::sc_int_base(const sc_unsigned_subref_r& v)
{
}

// Destructor
sc_int_base::~sc_int_base()
{
}


// Assignment operators
sc_int_base& sc_int_base::operator = (int_type v)
{
	value = v;
	sign_extend();
	return *this;
}

sc_int_base& sc_int_base::operator = (const sc_int_base& a)
{
	value = a.value;
	len = a.len;
	return *this;
}

sc_int_base& sc_int_base::operator = (const sc_int_subref_r & a)
{
}

sc_int_base& sc_int_base::operator = (const sc_signed& a)
{
}

sc_int_base& sc_int_base::operator = (const sc_unsigned& a)
{
}

sc_int_base& sc_int_base::operator = (const sc_fxval& a)
{
}

sc_int_base& sc_int_base::operator = (const sc_fxval_fast& a)
{
}

sc_int_base& sc_int_base::operator = (const sc_fxnum& a)
{
}

sc_int_base& sc_int_base::operator = (const sc_fxnum_fast& a)
{
}

sc_int_base& sc_int_base::operator = (const sc_bv_base& a)
{
}

sc_int_base& sc_int_base::operator = (const sc_lv_base& a)
{
}

sc_int_base& sc_int_base::operator = (const char *a)
{
	string_to_int(a, value);
	return *this;
}

sc_int_base& sc_int_base::operator = (unsigned long a)
{
	value = a;
	sign_extend();
	return *this;
}

sc_int_base& sc_int_base::operator = (long a)
{
	value = a;
	sign_extend();
	return *this;
}

sc_int_base& sc_int_base::operator = (unsigned int a)
{
	value = a;
	sign_extend();
	return *this;
}

sc_int_base& sc_int_base::operator = (int a)
{
	value = a;
	sign_extend();
	return *this;
}

sc_int_base& sc_int_base::operator = (uint64 a)
{
	value = a;
	sign_extend();
	return *this;
}

sc_int_base& sc_int_base::operator = (double a)
{
	value = a;
	sign_extend();
	return *this;
}


// Prefix and postfix increment and decrement operators
sc_int_base& sc_int_base::operator ++ ()          // Prefix
{
	++value;
	sign_extend();
	return *this;
}

const sc_int_base sc_int_base::operator ++ (int)  // Postfix
{
	sc_int_base ret(*this);
	++value;
	sign_extend();
	return ret;
}

sc_int_base& sc_int_base::operator -- ()          // Prefix
{
	--value;
	sign_extend();
	return *this;
}

const sc_int_base sc_int_base::operator -- (int)  // Postfix
{
	sc_int_base ret(*this);
	--value;
	sign_extend();
	return ret;
}

// Bit selection
sc_int_bitref sc_int_base::operator [] (int i)
{
	return check_bit_select(i) ? sc_int_bitref(this, i) : sc_int_bitref();
}

sc_int_bitref_r sc_int_base::operator [] (int i) const
{
	return check_bit_select(i) ? sc_int_bitref_r(this, i) : sc_int_bitref_r();
}

// Part selection
sc_int_subref sc_int_base::operator () (int left, int right)
{
	return range(left, right);
}

sc_int_subref_r sc_int_base::operator () (int left, int right) const
{
	return range(left, right);
}

sc_int_subref sc_int_base::range(int left, int right)
{
	return check_part_select(left, right) ? sc_int_subref(this, left, right) : sc_int_subref();
}

sc_int_subref_r sc_int_base::range(int left, int right) const
{
	return check_part_select(left, right) ? sc_int_subref_r(this, left, right) : sc_int_subref_r();
}

// Capacity
int sc_int_base::length() const
{
	return len;
}

// Reduce methods
bool sc_int_base::and_reduce() const
{
	return value == ~int_type(0);
}

bool sc_int_base::nand_reduce() const
{
	return !and_reduce();
}

bool sc_int_base::or_reduce() const
{
	return value != int_type(0);
}

bool sc_int_base::nor_reduce() const
{
	return !or_reduce();
}

bool sc_int_base::xor_reduce() const
{
	// divide and conquer
	uint_type m = ~uint_type(0) >> ((sizeof(uint_type) * CHAR_BIT) - len);
	uint_type v = uint_type(value) & m; // mask unused bits: beware value is signed extended
	v ^= v >> 32; // 32-bit xor
	v ^= v >> 16; // 16-bit xor
	v ^= v >> 8;  // 8-bit xor
	v ^= v >> 4;  // 4-bit xor
	v ^= v >> 2;  // 2-bit xor
	v ^= v >> 1;  // 1-bit xor
	return v & 1; // final xor result is in bit 0	
}

bool sc_int_base::xnor_reduce() const
{
	return !xor_reduce();
}

// Implicit conversion to int_type
sc_int_base::operator int_type() const
{
	return value;
}

// Explicit conversions
int sc_int_base::to_int() const
{
	return value;
}

unsigned int sc_int_base::to_uint() const
{
	return value;
}

long sc_int_base::to_long() const
{
	return value;
}

unsigned long sc_int_base::to_ulong() const
{
	return value;
}

int64 sc_int_base::to_int64() const
{
	return value;
}

uint64 sc_int_base::to_uint64() const
{
	return value;
}

double sc_int_base::to_double() const
{
	return value;
}

// Explicit conversion to character string
const std::string sc_int_base::to_string(sc_numrep numrep) const
{
	return to_string(SC_DEC, false);
}

const std::string sc_int_base::to_string(sc_numrep numrep, bool w_prefix) const
{
	return int_to_string(value, len, numrep, w_prefix);
}

// Other methods
void sc_int_base::print(std::ostream& os) const
{
	std::ios_base::fmtflags ff = os.flags();
	sc_numrep numrep =  ((ff & std::ios::basefield) == std::ios::hex) ? SC_HEX :
	                   (((ff & std::ios::basefield) == std::ios::oct) ? SC_OCT : SC_DEC);
	bool w_prefix = ((ff & std::ios::showbase) == std::ios::showbase);
	os << to_string(numrep, w_prefix);
}

void sc_int_base::scan(std::istream& is)
{
	std::string s;
	if(is >> s)
	{
		operator = (s.c_str());
	}
}

// Arithmetic perators
sc_int_base& sc_int_base::operator += (int_type v)
{
	value += v;
	sign_extend();
	return *this;
}

sc_int_base& sc_int_base::operator -= (int_type v)
{
	value -= v;
	sign_extend();
	return *this;
}

sc_int_base& sc_int_base::operator *= (int_type v)
{
	value *= v;
	sign_extend();
	return *this;
}

sc_int_base& sc_int_base::operator /= (int_type v)
{
	value /= v;
	sign_extend();
	return *this;
}

sc_int_base& sc_int_base::operator %= (int_type v)
{
	value %= v;
	sign_extend();
	return *this;
}

// Bitwise operators
sc_int_base& sc_int_base::operator &= (int_type v)
{
	value &= v;
	sign_extend();
	return *this;
}

sc_int_base& sc_int_base::operator |= (int_type v)
{
	value |= v;
	sign_extend();
	return *this;
}

sc_int_base& sc_int_base::operator ^= (int_type v)
{
	value ^= v;
	sign_extend();
	return *this;
}

sc_int_base& sc_int_base::operator <<= (int_type v)
{
	value <<= v;
	return *this;
}

sc_int_base& sc_int_base::operator >>= (int_type v)
{
	value >>= v;
	return *this;
}

void sc_int_base::sign_extend()
{
	const unsigned int sh = (sizeof(uint_type) * CHAR_BIT) - len;
	value = (value << sh) >> sh;
}

bool sc_int_base::check_bit_select(int bit_pos) const
{
	if((bit_pos < 0) || (bit_pos >= len))
	{
		throw std::runtime_error("out of range bit select of sc_int_base");
		return false;
	}
	
	return true;
}

bool sc_int_base::check_part_select(int left, int right) const
{
	if(left < right)
	{
		throw std::runtime_error("ill-formed part select of sc_int_base");
		return false;
	}
	
	if((left < 0) || (left >= len) || (right < 0) || (right >= len))
	{
		throw std::runtime_error("out of range part select of sc_int_base");
		return false;
	}
	
	return true;
}

// Comparison operators
bool operator == (const sc_int_base& a, const sc_int_base& b)
{
	return a.value == b.value;
}

bool operator != (const sc_int_base& a, const sc_int_base& b)
{
	return a.value != b.value;
}

bool operator < (const sc_int_base& a, const sc_int_base& b)
{
	return a.value < b.value;
}

bool operator <= (const sc_int_base& a, const sc_int_base& b)
{
	return a.value <= b.value;
}

bool operator > (const sc_int_base& a, const sc_int_base& b)
{
	return a.value > b.value;
}

bool operator >= (const sc_int_base& a, const sc_int_base& b)
{
	return a.value >= b.value;
}

} // end of namespace sc_dt
