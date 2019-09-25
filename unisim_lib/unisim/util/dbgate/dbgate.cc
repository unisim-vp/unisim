/*
 *  Copyright (c) 2019-2020,
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
 
#include <unisim/util/dbgate/dbgate.hh>
#include <limits>
#include <iostream>

namespace unisim {
namespace util {
namespace dbgate {
  

  DBGated::DBGated(int _port, char const* _root)
    : port(_port)
    , root()
  {
    if (_root)
      root.assign(_root);
    else
      { char tmpdirbuf[] = "/tmp/dbgateXXXXXX"; root = mkdtemp( tmpdirbuf ); }
    if (not _port)
      port = 12345;
  }
  
  void
  DBGated::write(int fd, char const* buffer, uintptr_t size)
  {
    auto itr = ostreams.find(fd);
    if (itr != ostreams.end())
      itr->second.write(buffer,size);
  }
  
  int
  DBGated::open(char const* path)
  {
    std::string fullpath = root + "/" + path;
    
    int rfd = 0;
    auto itr = ostreams.end();
    
    if (ostreams.size())
      {
        rfd = std::numeric_limits<int>::max();
        for (auto end = itr; --itr != end;)
          {
            if (itr->first < rfd) { rfd = itr->first+1; break; }
            else                  { rfd = itr->first-1; continue; }
          }
      }
    
    if (rfd < 0)
      return rfd;
    
    ostreams.emplace_hint( itr, std::piecewise_construct, std::forward_as_tuple( rfd ), std::forward_as_tuple( fullpath ) );

    return rfd;
  }

  void
  DBGated::close(int fd)
  {
    auto itr = ostreams.find(fd);
    if (itr != ostreams.end())
      ostreams.erase(itr);
  }
  
} /* end of namespace dbgate */
} /* end of namespace util */
} /* end of namespace unisim */

