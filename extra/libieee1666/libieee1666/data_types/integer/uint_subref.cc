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

#include <data_types/integer/uint_subref.h>
#include <data_types/integer/uint_base.h>
#include <data_types/util.h>

namespace sc_dt {

///////////////////////////////// definition //////////////////////////////////

// Copy constructor
sc_uint_subref::sc_uint_subref(const sc_uint_subref& a)
	: sc_uint_subref_r(a)
{
}

// Assignment operators
sc_uint_subref& sc_uint_subref::operator = (uint_type v)
{
	uint_type m = mask();
	obj->value = (obj->value & ~m) | (((uint_type) v << right) & m);
	return *this;
}

sc_uint_subref& sc_uint_subref::operator = (const sc_uint_base& a)
{
	return operator = (a.value);
}

sc_uint_subref& sc_uint_subref::operator = (const sc_uint_subref_r& a)
{
	return operator = ((uint_type) a);
}

sc_uint_subref& sc_uint_subref::operator = (const sc_uint_subref& a)
{
	return operator = ((const sc_uint_subref_r&) a);
}

sc_uint_subref& sc_uint_subref::operator = (const char *a)
{
	uint_type v;
	if(string_to_int(a, v))
	{
		operator = (v);
	}
	return *this;
}

sc_uint_subref& sc_uint_subref::operator = (unsigned long a)
{
	return operator = ((int_type) a);
}

sc_uint_subref& sc_uint_subref::operator = (long a)
{
	return operator = ((int_type) a);
}

sc_uint_subref& sc_uint_subref::operator = (unsigned int a)
{
	return operator = ((int_type) a);
}

sc_uint_subref& sc_uint_subref::operator = (int a)
{
	return operator = ((int_type) a);
}

sc_uint_subref& sc_uint_subref::operator = (int64 a)
{
	return operator = ((int_type) a);
}

sc_uint_subref& sc_uint_subref::operator = (double a)
{
	return operator = ((int_type) a);
}

sc_uint_subref& sc_uint_subref::operator = (const sc_signed&)
{
}

sc_uint_subref& sc_uint_subref::operator = (const sc_unsigned&)
{
}

sc_uint_subref& sc_uint_subref::operator = (const sc_bv_base&)
{
}

sc_uint_subref& sc_uint_subref::operator = (const sc_lv_base&)
{
}

// Other methods
void sc_uint_subref::scan(std::istream& is)
{
	std::string s;
	if(is >> s)
	{
		operator = (s.c_str());
	}
}

sc_uint_subref::sc_uint_subref()
{
}

sc_uint_subref::sc_uint_subref(sc_uint_base *_obj, int _left, int _right)
	: sc_uint_subref_r(_obj, _left, _right)
{
}

} // end of namespace sc_dt
