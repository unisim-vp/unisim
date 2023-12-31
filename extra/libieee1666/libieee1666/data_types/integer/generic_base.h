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

#ifndef __LIBIEEE1666_DATA_TYPES_INTEGER_GENERIC_BASE_H__
#define __LIBIEEE1666_DATA_TYPES_INTEGER_GENERIC_BASE_H__

#include <data_types/fwd.h>

namespace sc_dt {

//////////////////////////////// declaration //////////////////////////////////

// Note: class sc_generic_base proxy uses curiously recurring template pattern (CRTP) and multiple inheritance
// it provides operator -> as an efficient way for the receipt to statically call functions of derived class without resorting virtual functions.
// non-const -> operator is concretely unused and is here only for completeness purpose.
// An application-defined type should looks like the following:
// class ApplicationDefinedClass : public sc_generic_base<ApplicationDefinedClass>, public sc_value_base
// {
// public:
//     int length() const;
//     uint64 to_uint64() const;
//     int64 to_int64() const;
//     void to_sc_unsigned(sc_unsigned&) const;
//     void to_sc_signed(sc_signed&) const;
// };
template <class T>
class sc_generic_base
{
public:
	inline const T *operator -> () const;
	inline T *operator -> ();
};

///////////////////////////////// definition //////////////////////////////////

template <class T>
inline const T *sc_generic_base<T>::operator -> () const
{
	return (const T *) this;
}

template <class T>
inline T *sc_generic_base<T>::operator -> ()
{
	return (T *) this;
}

} // end of namespace sc_dt

#endif
