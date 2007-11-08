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
 
#include "unisim/util/endian/endian.hh"
#include "unisim/kernel/service/service.hh"

namespace unisim {
namespace kernel {
namespace service {

using unisim::util::endian::endian_type;
using unisim::util::endian::E_LITTLE_ENDIAN;
using unisim::util::endian::E_BIG_ENDIAN;
//  namespace full_system::memories::endian_type;

//   template <> Parameter<endian_type>::Parameter(const char *_name, Configurable *_configurable, endian_type& _storage) :
//     ParameterBase(_name, _configurable), storage(&_storage)
//   {
//   }

  template <> Parameter<endian_type>::Parameter(const char *_name, Object *_object, endian_type& _storage, const char *_description) :
    ParameterBase(_name, _object, "endianess", _description), storage(&_storage)
  {
  }

  template <> Parameter<endian_type>::operator bool () const { return *storage == E_LITTLE_ENDIAN; }
  template <> Parameter<endian_type>::operator long long () const { return (*storage == E_LITTLE_ENDIAN)?1:0; }
  template <> Parameter<endian_type>::operator unsigned long long () const { return (*storage == E_LITTLE_ENDIAN)?1:0; }
  template <> Parameter<endian_type>::operator double () const { return (double)(*storage == E_LITTLE_ENDIAN)?1:0; }
  template <> Parameter<endian_type>::operator string () const { return (*storage == E_LITTLE_ENDIAN)?(string("little-endian")):(string("big-endian"));}
  
  template <> ParameterBase& Parameter<endian_type>::operator = (bool value) { *storage = (value?E_LITTLE_ENDIAN:E_BIG_ENDIAN); return *this;}
  template <> ParameterBase& Parameter<endian_type>::operator = (long long value) { *storage = value?E_LITTLE_ENDIAN:E_BIG_ENDIAN; return *this;}
  template <> ParameterBase& Parameter<endian_type>::operator = (unsigned long long value) { *storage = value?E_LITTLE_ENDIAN:E_BIG_ENDIAN; return *this;}
  template <> ParameterBase& Parameter<endian_type>::operator = (double value) { *storage = value?E_LITTLE_ENDIAN:E_BIG_ENDIAN; return *this;}
  template <> ParameterBase& Parameter<endian_type>::operator = (const char *value) { *storage = (string(value) == string("little-endian"))?E_LITTLE_ENDIAN:E_BIG_ENDIAN; return *this; }
  
  template class Parameter<endian_type>;
}
}
}
