/*
 *  Copyright (c) 2016-2021,
 *  Commissariat a l'Energie Atomique (CEA),
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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */
 
#ifndef __UNISIM_UTIL_ARRAY_ARRAY_HH__
#define __UNISIM_UTIL_ARRAY_ARRAY_HH__

#include <inttypes.h>

#if __cplusplus >= 201103L

namespace details
{
  // A bit of template metaprog to construct static initializers for Objects, ServiceExports and ServiceImports
  template <std::size_t... Indices> struct indices { using next = indices<Indices..., sizeof...(Indices)>; };
  template <std::size_t N>  struct build_indices { using type = typename build_indices<N-1>::type::next; };
  template <>  struct build_indices<0> { using type = indices<>; };

  // Internal overload with indices tag
  template <std::size_t... I, typename Generator>
  std::array<typename Generator::Item, sizeof...(I)>
  make_array(Generator&& generator, indices<I...>)
  {
    return std::array<typename Generator::Item, sizeof...(I)> { { (generator.make_item(I))... } };
  }    
}

/// Construct statically initialized std::array using generator object.
template <std::size_t N, typename Generator>
std::array<typename Generator::Item, N>
make_array(Generator&& generator)
{
  return details::make_array(std::move(generator), details::build_indices<N>::type {});
}

#endif // __cplusplus >= 201103L

#endif // __UNISIM_UTIL_ARRAY_ARRAY_HH__

