/*
 *  Copyright (c) 2019-2021,
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

#ifndef __ARM64VP_SNAPSHOT_HH__
#define __ARM64VP_SNAPSHOT_HH__

#include <memory>
#include <inttypes.h>

struct SnapShot
{
  SnapShot(bool isld) : is_load_p(isld) {}
  virtual ~SnapShot() {}
  
  bool is_load() { return is_load_p; }
  bool is_save() { return not is_load_p; }

  virtual void load(uint8_t*, uintptr_t) = 0;
  virtual void save(uint8_t const*, uintptr_t) = 0;

  static std::unique_ptr<SnapShot> gzload(char const* filename);
  static std::unique_ptr<SnapShot> gzsave(char const* filename);
  
  void sync( uint8_t* buf, uintptr_t size );

  template <typename T> void save( T value  ) { sync(value); }
  
  template <typename T>
  void sync( T& value )
  {
    unsigned const size = sizeof (T);
    uint8_t buf[size];
    T tmp = 0;
    if (is_load())
      {
        load(&buf[0], size);
        for (unsigned idx = size; idx-- > 0;)
          tmp = (tmp << 8) | T(buf[idx]);
        value = tmp;
      }
    else
      {
        tmp = value;
        for (unsigned idx = 0; idx < size; ++idx)
          { buf[idx] = tmp; tmp >>= 8; }
        save(&buf[0], size);
      }
  }
  
  bool is_load_p;
};

#endif /* __ARM64VP_SNAPSHOT_HH__ */
