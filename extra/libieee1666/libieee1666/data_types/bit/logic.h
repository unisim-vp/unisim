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

#ifndef __LIBIEEE1666_DATA_TYPES_BIT_LOGIC_H__
#define __LIBIEEE1666_DATA_TYPES_BIT_LOGIC_H__

#include <data_types/fwd.h>
#include <iostream>

namespace sc_dt {

//////////////////////////////// declaration //////////////////////////////////

class sc_logic
{
public:
	// Constructors
	sc_logic();
	sc_logic(const sc_logic& a);
	sc_logic(sc_logic_value_t v);
	explicit sc_logic(bool a);
	explicit sc_logic(char a);
	explicit sc_logic(int a);

	// Destructor
	~sc_logic();

	// Assignment operators
	sc_logic& operator = (const sc_logic& a);
	sc_logic& operator = (sc_logic_value_t v);
	sc_logic& operator = (bool a);
	sc_logic& operator = (char a);
	sc_logic& operator = (int a);

	// Explicit conversions
	sc_logic_value_t value() const;
	char to_char() const;
	bool to_bool() const;
	bool is_01() const;
	
	// Other methods
	void print(std::ostream& os = std::cout) const;
	void scan(std::istream& is = std::cin);

	// Bitwise operators
	const sc_logic operator ~() const;

	friend const sc_logic operator & (const sc_logic& a, const sc_logic& b);
	friend const sc_logic operator & (const sc_logic& a, sc_logic_value_t b);
	friend const sc_logic operator & (const sc_logic& a, bool b);
	friend const sc_logic operator & (const sc_logic& a, char b);
	friend const sc_logic operator & (const sc_logic& a, int b);
	friend const sc_logic operator & (sc_logic_value_t a, const sc_logic& b);
	friend const sc_logic operator & (bool a, const sc_logic& b);
	friend const sc_logic operator & (char a, const sc_logic& b);
	friend const sc_logic operator & (int a, const sc_logic& b);

	sc_logic& operator &= (const sc_logic& b);
	sc_logic& operator &= (sc_logic_value_t b);
	sc_logic& operator &= (bool b);
	sc_logic& operator &= (char b);
	sc_logic& operator &= (int b);

	friend const sc_logic operator | (const sc_logic& a, const sc_logic& b);
	friend const sc_logic operator | (const sc_logic& a, sc_logic_value_t b);
	friend const sc_logic operator | (const sc_logic& a, bool b);
	friend const sc_logic operator | (const sc_logic& a, char b);
	friend const sc_logic operator | (const sc_logic& a, int b);
	friend const sc_logic operator | (sc_logic_value_t a, const sc_logic& b);
	friend const sc_logic operator | (bool a, const sc_logic& b);
	friend const sc_logic operator | (char a, const sc_logic& b);
	friend const sc_logic operator | (int a, const sc_logic& b);

	sc_logic& operator |= (const sc_logic& b);
	sc_logic& operator |= (sc_logic_value_t b);
	sc_logic& operator |= (bool b);
	sc_logic& operator |= (char b);
	sc_logic& operator |= (int b);

	friend const sc_logic operator ^ (const sc_logic& a, const sc_logic& b);
	friend const sc_logic operator ^ (const sc_logic& a, sc_logic_value_t b);
	friend const sc_logic operator ^ (const sc_logic& a, bool b);
	friend const sc_logic operator ^ (const sc_logic& a, char b);
	friend const sc_logic operator ^ (const sc_logic& a, int b);
	friend const sc_logic operator ^ (sc_logic_value_t a, const sc_logic& b);
	friend const sc_logic operator ^ (bool a, const sc_logic& b);
	friend const sc_logic operator ^ (char a, const sc_logic& b);
	friend const sc_logic operator ^ (int a, const sc_logic& b);

	sc_logic& operator ^= (const sc_logic& b);
	sc_logic& operator ^= (sc_logic_value_t b);
	sc_logic& operator ^= (bool b);
	sc_logic& operator ^= (char b);
	sc_logic& operator ^= (int b);

	// Comparison operators
	friend bool operator == (const sc_logic& a, const sc_logic& b);
	friend bool operator == (const sc_logic& a, sc_logic_value_t b);
	friend bool operator == (const sc_logic& a, bool b);
	friend bool operator == (const sc_logic& a, char b);
	friend bool operator == (const sc_logic& a, int b);
	friend bool operator == (sc_logic_value_t a, const sc_logic& b);
	friend bool operator == (bool a, const sc_logic& b);
	friend bool operator == (char a, const sc_logic& b);
	friend bool operator == (int a, const sc_logic& b);

	friend bool operator != (const sc_logic& a, const sc_logic& b);
	friend bool operator != (const sc_logic& a, sc_logic_value_t b);
	friend bool operator != (const sc_logic& a, bool b);
	friend bool operator != (const sc_logic& a, char b);
	friend bool operator != (const sc_logic& a, int b);
	friend bool operator != (sc_logic_value_t a, const sc_logic& b);
	friend bool operator != (bool a, const sc_logic& b);
	friend bool operator != (char a, const sc_logic& b);
	friend bool operator != (int a, const sc_logic& b);

private:
	// Disabled
	explicit sc_logic(const char *);
	sc_logic& operator = (const char *);

	static sc_logic_value_t bool2value(bool a);
	static sc_logic_value_t char2value(char a);
	static sc_logic_value_t int2value(int a);
	static sc_logic_value_t and_op(sc_logic_value_t a, sc_logic_value_t b);
	static sc_logic_value_t or_op(sc_logic_value_t a, sc_logic_value_t b);
	static sc_logic_value_t xor_op(sc_logic_value_t a, sc_logic_value_t b);
	static sc_logic_value_t not_op(sc_logic_value_t a);

	sc_logic_value_t logic_value;
};

const sc_logic SC_LOGIC_0(Log_0);
const sc_logic SC_LOGIC_1(Log_1);
const sc_logic SC_LOGIC_Z(Log_Z);
const sc_logic SC_LOGIC_X(Log_X);

} // end of namespace sc_dt

#endif
