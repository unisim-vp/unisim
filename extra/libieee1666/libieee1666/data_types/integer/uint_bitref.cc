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

#include <data_types/integer/uint_bitref.h>
#include <data_types/integer/uint_base.h>

namespace sc_dt {

///////////////////////////////// definition //////////////////////////////////

// Copy constructor
sc_uint_bitref::sc_uint_bitref(const sc_uint_bitref& a)
	: sc_uint_bitref_r(a)
{
}

// Assignment operators
sc_uint_bitref& sc_uint_bitref::operator = (const sc_uint_bitref_r& b)
{
	return operator = (b.to_bool());
}

sc_uint_bitref& sc_uint_bitref::operator = (const sc_uint_bitref& b)
{
	return operator = ((const sc_uint_bitref_r&) b);
}

sc_uint_bitref& sc_uint_bitref::operator = (bool b)
{
	if(obj)
	{
		uint_type m = mask();
		obj->value = (obj->value & ~m) | ((uint_type) b << bit_pos);
		obj->crop();
	}
	return *this;
}

sc_uint_bitref& sc_uint_bitref::operator &= (bool b)
{
	if(obj)
	{
		uint_type m = mask();
		obj->value &= (((uint_type) b << bit_pos) | ~m);
		obj->crop();
	}
	return *this;
}

sc_uint_bitref& sc_uint_bitref::operator |= (bool b)
{
	if(obj)
	{
		obj->value |= ((uint_type) b << bit_pos);
		obj->crop();
	}
	return *this;
}

sc_uint_bitref& sc_uint_bitref::operator ^= (bool b)
{
	if(obj)
	{
		obj->value ^= ((uint_type) b << bit_pos);
		obj->crop();
	}
	return *this;
}

// Other methods
void sc_uint_bitref::scan(std::istream& is)
{
	bool b;
	if(is >> b)
	{
		operator = (b);
	}
}

sc_uint_bitref::sc_uint_bitref()
	: sc_uint_bitref_r()
{
}

sc_uint_bitref::sc_uint_bitref(sc_uint_base *_obj, int _bit_pos)
	: sc_uint_bitref_r(_obj, _bit_pos)
{
}

} // end of namespace sc_dt
