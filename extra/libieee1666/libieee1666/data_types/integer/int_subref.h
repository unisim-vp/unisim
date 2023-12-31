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

#ifndef __LIBIEEE1666_DATA_TYPES_INTEGER_INT_SUBREF_H__
#define __LIBIEEE1666_DATA_TYPES_INTEGER_INT_SUBREF_H__

#include <data_types/fwd.h>
#include <data_types/integer/int_subref_r.h>
#include <iostream>

namespace sc_dt {

//////////////////////////////// declaration //////////////////////////////////

class sc_int_subref : public sc_int_subref_r
{
	friend class sc_int_base;
	
public:
	// Copy constructor
	sc_int_subref(const sc_int_subref& a);

	// Assignment operators
	sc_int_subref& operator = (int_type v);
	sc_int_subref& operator = (const sc_int_base& a);
	sc_int_subref& operator = (const sc_int_subref_r & a);
	sc_int_subref& operator = (const sc_int_subref& a);
	template <class T> sc_int_subref& operator = (const sc_generic_base<T>& a);
	sc_int_subref& operator = (const char *a);
	sc_int_subref& operator = (unsigned long a);
	sc_int_subref& operator = (long a);
	sc_int_subref& operator = (unsigned int a);
	sc_int_subref& operator = (int a);
	sc_int_subref& operator = (uint64 a);
	sc_int_subref& operator = (double a);
	sc_int_subref& operator = (const sc_signed&);
	sc_int_subref& operator = (const sc_unsigned&);
	sc_int_subref& operator = (const sc_bv_base&);
	sc_int_subref& operator = (const sc_lv_base&);

	// Other methods
	void scan(std::istream& is = std::cin);

protected:
	sc_int_subref();
	sc_int_subref(sc_int_base *obj, int left, int right);	
};

///////////////////////////////// definition //////////////////////////////////

// Assignment operators
template <class T> sc_int_subref& sc_int_subref::operator = (const sc_generic_base<T>& a)
{
}

} // end of namespace sc_dt

#endif
