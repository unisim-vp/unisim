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

#include <data_types/integer/int_subref_r.h>
#include <data_types/integer/int_base.h>
#include <data_types/util.h>

namespace sc_dt {

///////////////////////////////// definition //////////////////////////////////

// Copy constructor
sc_int_subref_r::sc_int_subref_r(const sc_int_subref_r& a)
	: obj(a.obj)
	, left(a.left)
	, right(a.right)
{
}

// Destructor
sc_int_subref_r::~sc_int_subref_r()
{
}

// Capacity
int sc_int_subref_r::length() const
{
	return left - right + 1;
}

// Reduce methods
bool sc_int_subref_r::and_reduce() const
{
	if(obj)
	{
		uint_type m = mask();
		uint_type v = uint_type(obj->value) & m;
		return v == m;
	}
	
	return false;
}

bool sc_int_subref_r::nand_reduce() const
{
	return !and_reduce();
}

bool sc_int_subref_r::or_reduce() const
{
	if(obj)
	{
		uint_type m = mask();
		uint_type v = uint_type(obj->value) & m;
		return v != 0;
	}
	
	return false;
}

bool sc_int_subref_r::nor_reduce() const
{
	return !or_reduce();
}

bool sc_int_subref_r::xor_reduce() const
{
	if(obj)
	{
		// divide and conquer
		uint_type m = mask();
		uint_type v = uint_type(obj->value) & m;
		v ^= v >> 32; // 32-bit xor
		v ^= v >> 16; // 16-bit xor
		v ^= v >> 8;  // 8-bit xor
		v ^= v >> 4;  // 4-bit xor
		v ^= v >> 2;  // 2-bit xor
		v ^= v >> 1;  // 1-bit xor
		return v & 1; // final xor result is in bit 0
	}
	
	return false;
}

bool sc_int_subref_r::xnor_reduce() const
{
	return !xor_reduce();
}

// Implicit conversion to uint_type
sc_int_subref_r::operator uint_type() const
{
	if(obj)
	{
		uint_type m = mask();
		return (uint_type) (obj->value & m) >> right;
	}
	
	return uint_type(0);
}

// Explicit conversions
int sc_int_subref_r::to_int() const
{
	return operator uint_type();
}

unsigned int sc_int_subref_r::to_uint() const
{
	return operator uint_type();
}

long sc_int_subref_r::to_long() const
{
	return operator uint_type();
}

unsigned long sc_int_subref_r::to_ulong() const
{
	return operator uint_type();
}

int64 sc_int_subref_r::to_int64() const
{
	return operator uint_type();
}

uint64 sc_int_subref_r::to_uint64() const
{
	return operator uint_type();
}

double sc_int_subref_r::to_double() const
{
	return operator uint_type();
}

// Explicit conversion to character string
const std::string sc_int_subref_r::to_string(sc_numrep numrep) const
{
	return to_string(numrep, false);
}

const std::string sc_int_subref_r::to_string(sc_numrep numrep, bool w_prefix) const
{
	return int_to_string(operator uint_type(), length(), numrep, w_prefix);
}

// Other methods
void sc_int_subref_r::print(std::ostream& os) const
{
	std::ios_base::fmtflags ff = os.flags();
	sc_numrep numrep =  ((ff & std::ios::basefield) == std::ios::hex) ? SC_HEX :
	                   (((ff & std::ios::basefield) == std::ios::oct) ? SC_OCT : SC_DEC);
	bool w_prefix = ((ff & std::ios::showbase) == std::ios::showbase);
	os << to_string(numrep, w_prefix);
}

sc_int_subref_r::sc_int_subref_r()
	: obj(0)
	, left(0)
	, right(0)
{
}

sc_int_subref_r::sc_int_subref_r(const sc_int_base *_obj, int _left, int _right)
	: obj((sc_int_base *) _obj)
	, left(_left)
	, right(_right)
{
}

uint_type sc_int_subref_r::mask() const
{
	return (~uint_type(0) >> ((sizeof(uint_type) * CHAR_BIT) - 1 - left)) & (~uint_type(0) << right);
}

// Disabled
sc_int_subref_r& sc_int_subref_r::operator = (const sc_int_subref_r&)
{
	return *this;
}

} // end of namespace sc_dt
