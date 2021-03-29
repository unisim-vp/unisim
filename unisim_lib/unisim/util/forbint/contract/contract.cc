/*
 *  Copyright (c) 2007-2020,
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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr), Franck Vedrine (franck.vedrine@cea.fr)
 */

#include <unisim/util/forbint/contract/contract.hh>
#include <vector>

namespace unisim {
namespace util {
namespace forbint {
namespace contract {

char*
DomainValue::write() const
{
  if (not pfFunctions)
    return 0;
  
  static std::vector<char> buffer;
  buffer.clear();
  
  struct ibs
  {
    static char* increase_vector_char_buffer_size(char* buffer, int old_length, int new_length, void* awriter)
    {
      std::vector<char>* writer = reinterpret_cast<std::vector<char>*>(awriter);
      assert(&(*writer)[0] == buffer && int(writer->size()) == old_length);
      writer->insert(writer->end(), new_length-old_length, '\0');
      return &(*writer)[0];
    }
  };

  buffer.insert(buffer.begin(), 40, '\0');
  int buffer_size = 40, length = 0;
  (*pfFunctions->write)(deValue, &buffer[0], buffer_size, &length, &buffer, &ibs::increase_vector_char_buffer_size);
  buffer.resize(length+1);
  return &buffer[0];
}

} /* namespace contract */
} /* namespace forbint */
} /* namespace util */
} /* namespace unisim */

