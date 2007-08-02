/*
 *  Copyright (c) 2007,
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
 
#include <memories/am29lv/am29lv.hh>
#include <memories/am29lv/am29lv.tpp>

namespace full_system {
namespace memories {
namespace am29lv {

template class AM29LV<AM29LV800BConfig, 4 * M, 8>; // 4 MB/64 bits
template class AM29LV<AM29LV800BConfig, 8 * M, 8>; // 8 MB/64 bits

} // end of namespace am29lv
} // end of namespace memories
} // end of namespace full_system

namespace full_system {
namespace utils {
namespace services {

using full_system::memories::am29lv::MODE;
using full_system::memories::am29lv::MODE_X8;
using full_system::memories::am29lv::MODE_X16;

template <> Parameter<MODE>::Parameter(const char *_name, Object *_object, MODE& _storage, const char *_description) :
	ParameterBase(_name, _object, _description), storage(&_storage)
{
}

template <> Parameter<MODE>::operator bool () const { return *storage == MODE_X8; }
template <> Parameter<MODE>::operator long long () const { return (*storage == MODE_X8) ? 1: 0; }
template <> Parameter<MODE>::operator unsigned long long () const { return (*storage == MODE_X8) ? 1 : 0; }
template <> Parameter<MODE>::operator double () const { return (double)(*storage == MODE_X8) ? 1 : 0; }
template <> Parameter<MODE>::operator string () const { return (*storage == MODE_X8)?(string("x8")):(string("x16"));}

template <> ParameterBase& Parameter<MODE>::operator = (bool value) { *storage = (value ? MODE_X8 : MODE_X16); return *this;}
template <> ParameterBase& Parameter<MODE>::operator = (long long value) { *storage = value? MODE_X8 : MODE_X16; return *this;}
template <> ParameterBase& Parameter<MODE>::operator = (unsigned long long value) { *storage = value? MODE_X8 : MODE_X16; return *this;}
template <> ParameterBase& Parameter<MODE>::operator = (double value) { *storage = value? MODE_X8 : MODE_X16; return *this;}
template <> ParameterBase& Parameter<MODE>::operator = (const char *value) { *storage = (string(value) == string("x8")) ? MODE_X8 : MODE_X16; return *this; }

template class Parameter<MODE>;

} // end of namespace services
} // end of namespace utils
} // end of namespace full_system
