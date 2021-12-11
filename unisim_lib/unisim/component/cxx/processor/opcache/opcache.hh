/*
 *  Copyright (c) 2021-2021,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific 
 *     prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 *  ARE DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY 
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_OPCACHE_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_OPCACHE_HH__

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace opcache {

struct BaseConfig
{
  static unsigned const DECODE_HASH_TABLE_ENTRIES = 4096;
  static unsigned const OPERATIONS_PER_PAGE = 4096;
};

template <typename DECODER, typename CONFIG = BaseConfig>
struct OpCache : public DECODER
{
  OpCache() : mru_page( 0 ), decode_hash_table() {}
  ~OpCache() { InvalidateDecodingCache(); }

  typedef typename DECODER::operation_type operation_type;
  typedef typename DECODER::address_type   address_type;
  typedef typename DECODER::code_type      code_type;
  enum { alignment = DECODER::alignment % 8 ? 1 : DECODER::alignment / 8 };
  
  struct DecodeMapPage
  {
    DecodeMapPage(address_type _key) : key(_key), next(0), operation() {}
    ~DecodeMapPage() { for (unsigned idx = CONFIG::OPERATIONS_PER_PAGE; idx-- > 0; ) delete operation[idx]; }

    address_type    key;
    DecodeMapPage*  next;
    operation_type* operation[CONFIG::OPERATIONS_PER_PAGE];
  };

  operation_type* Decode(address_type addr, code_type insn);
  void InvalidateDecodingCacheEntry(address_type addr);
  void InvalidateDecodingCache();
  DecodeMapPage* FindPage(address_type page_key);
  
  DecodeMapPage* mru_page;
  DecodeMapPage* decode_hash_table[CONFIG::DECODE_HASH_TABLE_ENTRIES];
};

} // end of opcache
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_HH__
