/*
 *  Copyright (c) 2019,
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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#include <testrun.hh>
#include <ostream>

namespace ut
{
  namespace
  {
    template <typename T>
    void HexStore( std::ostream& sink, T value )
    {
      unsigned const digits = 2*sizeof (T);
      char buf[1 + digits];
      buf[digits] = '\0';
      for (int idx = digits; --idx >= 0;)
        {
          buf[idx] = "0123456789abcdef"[value & 0xf];
          value >>= 4;
        }
      sink << &buf[0];
    }

    template <typename T>
    void HexLoad( T& value, char const*& buf )
    {
      unsigned const digits = 2*sizeof (T);
      T scratch = 0;
      char const* ptr = buf;
      for (char const* end = &buf[digits]; ptr < end; ++ptr)
        {
          char ch = *ptr;
          if      ('0' <= ch and ch <= '9') scratch = (scratch << 4) | T( ch - '0' +  0 );
          else if ('a' <= ch and ch <= 'f') scratch = (scratch << 4) | T( ch - 'a' + 10 );
          else if ('A' <= ch and ch <= 'F') scratch = (scratch << 4) | T( ch - 'A' + 10 );
          else break;
        }
      value = scratch;
      buf = ptr;
    }
  }

  TestbedBase::TestbedBase(char const* seed, uint64_t* elems, uintptr_t count)
  {
    for (unsigned idx = 0, end = 5; idx < end; ++idx)
      HexLoad( rng.State(idx), seed );
    for (unsigned idx = 0, end = count; idx < end; ++idx)
      HexLoad( elems[idx], seed );
    HexLoad( counter, seed );
  }

  void TestbedBase::serial( std::ostream& sink, uint64_t* elems, uintptr_t count ) const
  {
    for (unsigned idx = 0, end = 5; idx < end; ++idx)
      HexStore( sink, rng.State(idx) );
    for (unsigned idx = 0, end = count; idx < end; ++idx)
      HexStore( sink, elems[idx] );
    HexStore( sink, counter );
  }
  
} /* end of namespace ut */
