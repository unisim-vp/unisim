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

#include <data_types/bit/logic.h>

namespace sc_dt {

///////////////////////////////// definition //////////////////////////////////

const sc_logic SC_LOGIC_0(Log_0);
const sc_logic SC_LOGIC_1(Log_1);
const sc_logic SC_LOGIC_Z(Log_Z);
const sc_logic SC_LOGIC_X(Log_X);

sc_logic_value_t sc_logic::bool2value(bool a)
{
	return a ? Log_1 : Log_0;
}

sc_logic_value_t sc_logic::char2value(char a)
{
	return (a == '0') ? Log_0 :
	       ((a == '1') ? Log_1 :
	       (((a == 'Z') || (a == 'z')) ? Log_Z : Log_X));
}

sc_logic_value_t sc_logic::int2value(int a)
{
	switch(a)
	{
		case Log_0: return Log_0;
		case Log_1: return Log_1;
		case Log_Z: return Log_Z;
		case Log_X: return Log_X;
	}

	throw std::runtime_error("invalid sc_logic value");
	return Log_X;
}

sc_logic_value_t sc_logic::and_op(sc_logic_value_t a, sc_logic_value_t b)
{
	return ((a == Log_0) || (b == Log_0)) ? Log_0 :
	       ((a == Log_1) && (b == Log_1)) ? Log_1 : Log_X;
}

sc_logic_value_t sc_logic::or_op(sc_logic_value_t a, sc_logic_value_t b)
{
	return ((a == Log_1) || (b == Log_1)) ? Log_1 :
	       ((a == Log_0) && (b == Log_0)) ? Log_0 : Log_X;
}

sc_logic_value_t sc_logic::xor_op(sc_logic_value_t a, sc_logic_value_t b)
{
	return ((a == Log_0) && (b == Log_0)) ? Log_0 :
	       (((a == Log_0) && (b == Log_1)) ? Log_1 :
	       ((((a == Log_1) && (b == Log_0)) ? Log_1 :
	       (((((a == Log_1) && (b == Log_1)) ? Log_0 : Log_X))))));
}

sc_logic_value_t sc_logic::not_op(sc_logic_value_t a)
{
	return (a == Log_0) ? Log_1 :
	       (a == Log_1) ? Log_0 : Log_X;
}

// Constructors
sc_logic::sc_logic()
	: logic_value(Log_X)
{
}

sc_logic::sc_logic(const sc_logic& a)
	: logic_value(a.logic_value)
{
}

sc_logic::sc_logic(sc_logic_value_t v)
	: logic_value(v)
{
}

sc_logic::sc_logic(bool a)
	: logic_value(a ? Log_1 : Log_0)
{
}

sc_logic::sc_logic(char a)
	: logic_value(char2value(a))
{
}

sc_logic::sc_logic(int a)
	: logic_value(int2value(a))
{
}

// Destructor
sc_logic::~sc_logic()
{
}

// Assignment operators
sc_logic& sc_logic::operator = (const sc_logic& a)
{
	logic_value = a.logic_value;
	return *this;
}

sc_logic& sc_logic::operator = (sc_logic_value_t v)
{
	logic_value = v;
	return *this;
}

sc_logic& sc_logic::operator = (bool a)
{
	logic_value = bool2value(a);;
	return *this;
}

sc_logic& sc_logic::operator = (char a)
{
	logic_value = char2value(a);
	return *this;
}

sc_logic& sc_logic::operator = (int a)
{
	logic_value = int2value(a);
	return *this;
}

// Explicit conversions
sc_logic_value_t sc_logic::value() const
{
	return logic_value;
}

char sc_logic::to_char() const
{
	switch(logic_value)
	{
		case Log_0: return '0';
		case Log_1: return '1';
		case Log_Z: return 'Z';
		case Log_X: return 'X';
	}
	return 'X';
}

bool sc_logic::to_bool() const
{
	return logic_value != Log_0;
}

bool sc_logic::is_01() const
{
	return (logic_value == Log_0) || (logic_value == Log_1);
}

// Other methods
void sc_logic::print(std::ostream& os) const
{
	os << to_char();
}

void sc_logic::scan(std::istream& is)
{
	char c;
	if(is >> c)
	{
		operator = (c);
	}
}

// Disabled
sc_logic::sc_logic(const char *)
{
}

sc_logic& sc_logic::operator = (const char *)
{
	return *this;
}

// Bitwise operators
const sc_logic sc_logic::operator ~() const
{
	return sc_logic(sc_logic::not_op(logic_value));
}

sc_logic& sc_logic::operator &= (const sc_logic& b)
{
	logic_value = and_op(logic_value, b.logic_value);
	return *this;
}

sc_logic& sc_logic::operator &= (sc_logic_value_t b)
{
	logic_value = and_op(logic_value, b);
	return *this;
}

sc_logic& sc_logic::operator &= (bool b)
{
	logic_value = and_op(logic_value, bool2value(b));
	return *this;
}

sc_logic& sc_logic::operator &= (char b)
{
	logic_value = and_op(logic_value, char2value(b));
	return *this;
}

sc_logic& sc_logic::operator &= (int b)
{
	logic_value = and_op(logic_value, int2value(b));
	return *this;
}

sc_logic& sc_logic::operator |= (const sc_logic& b)
{
	logic_value = or_op(logic_value, b.logic_value);
	return *this;
}

sc_logic& sc_logic::operator |= (sc_logic_value_t b)
{
	logic_value = or_op(logic_value, b);
	return *this;
}

sc_logic& sc_logic::operator |= (bool b)
{
	logic_value = or_op(logic_value, bool2value(b));
	return *this;
}

sc_logic& sc_logic::operator |= (char b)
{
	logic_value = or_op(logic_value, char2value(b));
	return *this;
}

sc_logic& sc_logic::operator |= (int b)
{
	logic_value = or_op(logic_value, int2value(b));
	return *this;
}

sc_logic& sc_logic::operator ^= (const sc_logic& b)
{
	logic_value = xor_op(logic_value, b.logic_value);
	return *this;
}

sc_logic& sc_logic::operator ^= (sc_logic_value_t b)
{
	logic_value = xor_op(logic_value, b);
	return *this;
}

sc_logic& sc_logic::operator ^= (bool b)
{
	logic_value = xor_op(logic_value, b ? Log_1 : Log_0);
	return *this;
}

sc_logic& sc_logic::operator ^= (char b)
{
	logic_value = xor_op(logic_value, char2value(b));
	return *this;
}

sc_logic& sc_logic::operator ^= (int b)
{
	logic_value = xor_op(logic_value, int2value(b));
	return *this;
}

const sc_logic operator & (const sc_logic& a, const sc_logic& b)
{
	return sc_logic(sc_logic::and_op(a.logic_value, b.logic_value));
}

const sc_logic operator & (const sc_logic& a, sc_logic_value_t b)
{
	return sc_logic(sc_logic::and_op(a.logic_value, b));
}

const sc_logic operator & (const sc_logic& a, bool b)
{
	return sc_logic(sc_logic::and_op(a.logic_value, sc_logic::bool2value(b)));
}

const sc_logic operator & (const sc_logic& a, char b)
{
	return sc_logic(sc_logic::and_op(a.logic_value, sc_logic::char2value(b)));
}

const sc_logic operator & (const sc_logic& a, int b)
{
	return sc_logic(sc_logic::and_op(a.logic_value, sc_logic::int2value(b)));
}

const sc_logic operator & (sc_logic_value_t a, const sc_logic& b)
{
	return sc_logic(sc_logic::and_op(a, b.logic_value));
}

const sc_logic operator & (bool a, const sc_logic& b)
{
	return sc_logic(sc_logic::and_op(sc_logic::bool2value(a), b.logic_value));
}

const sc_logic operator & (char a, const sc_logic& b)
{
	return sc_logic(sc_logic::and_op(sc_logic::char2value(a), b.logic_value));
}

const sc_logic operator & (int a, const sc_logic& b)
{
	return sc_logic(sc_logic::and_op(sc_logic::int2value(a), b.logic_value));
}

const sc_logic operator | (const sc_logic& a, const sc_logic& b)
{
	return sc_logic(sc_logic::or_op(a.logic_value, b.logic_value));
}

const sc_logic operator | (const sc_logic& a, sc_logic_value_t b)
{
	return sc_logic(sc_logic::or_op(a.logic_value, b));
}

const sc_logic operator | (const sc_logic& a, bool b)
{
	return sc_logic(sc_logic::or_op(a.logic_value, sc_logic::bool2value(b)));
}

const sc_logic operator | (const sc_logic& a, char b)
{
	return sc_logic(sc_logic::or_op(a.logic_value, sc_logic::char2value(b)));
}

const sc_logic operator | (const sc_logic& a, int b)
{
	return sc_logic(sc_logic::or_op(a.logic_value, sc_logic::int2value(b)));
}

const sc_logic operator | (sc_logic_value_t a, const sc_logic& b)
{
	return sc_logic(sc_logic::or_op(a, b.logic_value));
}

const sc_logic operator | (bool a, const sc_logic& b)
{
	return sc_logic(sc_logic::or_op(sc_logic::bool2value(a), b.logic_value));
}

const sc_logic operator | (char a, const sc_logic& b)
{
	return sc_logic(sc_logic::or_op(sc_logic::char2value(a), b.logic_value));
}

const sc_logic operator | (int a, const sc_logic& b)
{
	return sc_logic(sc_logic::or_op(sc_logic::int2value(a), b.logic_value));
}


const sc_logic operator ^ (const sc_logic& a, const sc_logic& b)
{
	return sc_logic(sc_logic::xor_op(a.logic_value, b.logic_value));
}

const sc_logic operator ^ (const sc_logic& a, sc_logic_value_t b)
{
	return sc_logic(sc_logic::xor_op(a.logic_value, b));
}

const sc_logic operator ^ (const sc_logic& a, bool b)
{
	return sc_logic(sc_logic::xor_op(a.logic_value, sc_logic::bool2value(b)));
}

const sc_logic operator ^ (const sc_logic& a, char b)
{
	return sc_logic(sc_logic::xor_op(a.logic_value, sc_logic::char2value(b)));
}

const sc_logic operator ^ (const sc_logic& a, int b)
{
	return sc_logic(sc_logic::xor_op(a.logic_value, sc_logic::int2value(b)));
}

const sc_logic operator ^ (sc_logic_value_t a, const sc_logic& b)
{
	return sc_logic(sc_logic::xor_op(a, b.logic_value));
}

const sc_logic operator ^ (bool a, const sc_logic& b)
{
	return sc_logic(sc_logic::xor_op(sc_logic::bool2value(a), b.logic_value));
}

const sc_logic operator ^ (char a, const sc_logic& b)
{
	return sc_logic(sc_logic::xor_op(sc_logic::char2value(a), b.logic_value));
}

const sc_logic operator ^ (int a, const sc_logic& b)
{
	return sc_logic(sc_logic::xor_op(sc_logic::int2value(a), b.logic_value));
}

// Comparison operators
bool operator == (const sc_logic& a, const sc_logic& b)
{
	return a.value() == b.value();
}

bool operator == (const sc_logic& a, sc_logic_value_t b)
{
	return a.value() == b;
}

bool operator == (const sc_logic& a, bool b)
{
	return a.to_bool() == b;
}

bool operator == (const sc_logic& a, char b)
{
	return a.to_char() == b;
}

bool operator == (const sc_logic& a, int b)
{
	return a.value() == sc_logic_value_t(b);
}

bool operator == (sc_logic_value_t a, const sc_logic& b)
{
	return a == b.value();
}

bool operator == (bool a, const sc_logic& b)
{
	return a == b.to_bool();
}

bool operator == (char a, const sc_logic& b)
{
	return a == b.to_char();
}

bool operator == (int a, const sc_logic& b)
{
	return sc_logic_value_t(a) == b.value();
}


bool operator != (const sc_logic& a, const sc_logic& b)
{
	return a.value() != b.value();
}

bool operator != (const sc_logic& a, sc_logic_value_t b)
{
	return a.value() != b;
}

bool operator != (const sc_logic& a, bool b)
{
	return a.to_bool() != b;
}

bool operator != (const sc_logic& a, char b)
{
	return a.to_char() != b;
}

bool operator != (const sc_logic& a, int b)
{
	return a.value() != sc_logic_value_t(b);
}

bool operator != (sc_logic_value_t a, const sc_logic& b)
{
	return a != b.value();
}

bool operator != (bool a, const sc_logic& b)
{
	return a != b.to_bool();
}

bool operator != (char a, const sc_logic& b)
{
	return a != b.to_char();
}

bool operator != (int a, const sc_logic& b)
{
	return sc_logic_value_t(a) != b.value();
}

} // end of namespace sc_dt
