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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_OPCACHE_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_OPCACHE_TCC__

#include <unisim/component/cxx/processor/opcache/opcache.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace opcache {

template <typename DECODER, class CONFIG>
void
OpCache<DECODER, CONFIG>::InvalidateDecodingCache()
{
  address_type index;
  mru_page = 0;
  for (index = 0; index < CONFIG::DECODE_HASH_TABLE_ENTRIES; index++)
    {
      DecodeMapPage *page, *next_page;
      page = decode_hash_table[index];
      if(page)
        {
          do
            {
              next_page = page->next;
              delete page;
              page = next_page;
            } while(page);
          decode_hash_table[index] = 0;
        }
    }
}
 
template <typename DECODER, class CONFIG>
void
OpCache<DECODER, CONFIG>::InvalidateDecodingCacheEntry(address_type addr)
{
  address_type word_addr = addr / alignment;
  address_type page_key = word_addr / CONFIG::OPERATIONS_PER_PAGE;
  if (mru_page && mru_page->key == page_key) mru_page = 0;
  address_type index = page_key % CONFIG::DECODE_HASH_TABLE_ENTRIES; // hash the key
  DecodeMapPage *prev, *cur;
  cur = decode_hash_table[index];
  if(cur)
    {
      if(cur->key == page_key)
        {
          decode_hash_table[index] = cur->next;
          delete cur;
          return;
        }
      prev = cur;
      cur = cur->next;
      if(cur)
        {
          do
            {
              if(cur->key == page_key)
                {
                  prev->next = cur->next;
                  cur->next = 0;
                  delete cur;
                  return;
                }
              prev = cur;
            } while((cur = cur->next) != 0);
        }
    }
}

template <typename DECODER, class CONFIG>
typename OpCache<DECODER, CONFIG>::DecodeMapPage*
OpCache<DECODER, CONFIG>::FindPage(address_type page_key)
{
  if(mru_page && mru_page->key == page_key) return mru_page;
  address_type index = page_key % CONFIG::DECODE_HASH_TABLE_ENTRIES; // hash the key
  DecodeMapPage *prev, *cur;
  cur = decode_hash_table[index];
  if(cur)
    {
      if(cur->key == page_key)
        {
          mru_page = cur;
          return cur;
        }
      prev = cur;
      cur = cur->next;
      if(cur)
        {
          do
            {
              if(cur->key == page_key)
                {
                  prev->next = cur->next;
                  cur->next= decode_hash_table[index];
                  decode_hash_table[index] = cur;
                  mru_page = cur;
                  return cur;
                }
              prev = cur;
            } while((cur = cur->next) != 0);
        }
    }
  return 0;
}

template <typename DECODER, class CONFIG>
typename DECODER::operation_type*
OpCache<DECODER, CONFIG>::Decode(address_type addr, code_type insn)
{
  address_type const operations_per_page( CONFIG::OPERATIONS_PER_PAGE );
  address_type const decode_hash_table_entries( CONFIG::DECODE_HASH_TABLE_ENTRIES );
  address_type word_addr = addr / alignment;
  address_type page_key = word_addr / operations_per_page;

  DecodeMapPage* page = FindPage(page_key);
  if (not page)
    {
      page = new DecodeMapPage (page_key);
      address_type index = page_key % decode_hash_table_entries; // hash the key
      page->next = decode_hash_table[index];
      decode_hash_table[index] = page;
      mru_page = page;
    }

  operation_type* operation = page->operation[word_addr % operations_per_page];
  if (operation)
    {
      if (operation->Match(addr, insn))
        return operation;
      delete operation;
    }

  operation = this->NCDecode(addr, insn);
  page->operation[word_addr % operations_per_page] = operation;

  return operation;
}

} // end of opcache
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_HH__
