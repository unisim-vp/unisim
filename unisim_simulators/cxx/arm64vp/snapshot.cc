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

#include <snapshot.hh>
#include <zlib.h>

void
SnapShot::sync( uint8_t* buf, uintptr_t size )
{
  if (is_load())
    load(buf,size);
  else
    save(buf,size);
}


std::unique_ptr<SnapShot> SnapShot::gzsave(char const* filename)
{
  struct SaveSnapShot : public SnapShot
  {
    SaveSnapShot( char const* filename ) : SnapShot(false), gz_file( gzopen( filename, "w" ) ) {}
    ~SaveSnapShot() { gzclose(gz_file); }
    struct Bad {};
    void load(uint8_t*, uintptr_t size) override { throw Bad(); }
    void save(uint8_t const* bytes, uintptr_t size) override
    {
      if (uintptr_t(gzwrite( gz_file, bytes, size)) != size)
        throw Bad();
    }
    gzFile gz_file;
  };
  
  return std::make_unique<SaveSnapShot>( filename );
}

std::unique_ptr<SnapShot> SnapShot::gzload(char const* filename)
{
  struct LoadSnapShot : public SnapShot
  {
    LoadSnapShot( char const* filename ) : SnapShot(true), gz_file( gzopen( filename, "r" ) ) {}
    ~LoadSnapShot() { gzclose(gz_file); }
    struct Bad {};
    void load(uint8_t* bytes, uintptr_t size) override
    {
      if (uintptr_t(gzread( gz_file, bytes, size )) != size)
        throw Bad();
    }
    void save(uint8_t const*, uintptr_t size) override { throw Bad(); }
    gzFile gz_file;
  };

  return std::make_unique<LoadSnapShot>( filename );
}
