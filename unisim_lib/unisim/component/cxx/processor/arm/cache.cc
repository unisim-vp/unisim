/*
 *  Copyright (c) 2009-2015,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */


#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include "unisim/component/cxx/processor/arm/cache.hh"
#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger.hh"

// #define ARM_CACHE_DEBUG

#ifdef ARM_CACHE_DEBUG
#include <iostream>
using namespace std;
#endif

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

  using unisim::service::interfaces::CachePowerEstimator;
  using unisim::service::interfaces::PowerMode;
  using unisim::kernel::logger::DebugError;
  using unisim::kernel::logger::EndDebugError;

  Cache::Cache(const char *name, unisim::kernel::service::Object *parent) 
    : unisim::kernel::service::Object(name, parent)
    , unisim::kernel::service::Client<CachePowerEstimator>(name,  parent)
    , unisim::kernel::service::Client<PowerMode>(name,  parent)
    , power_estimator_import("power-estimator-import", this)
    , power_mode_import("power-mode-import", this)
    , accesses(0)
    , read_accesses(0)
    , write_accesses(0)
    , prefetch_accesses(0)
    , hits(0)
    , read_hits(0)
    , write_hits(0)
    , prefetch_hits(0)
    , logger(*this)
    , rand(1, -1, rand.Max, rand.Min)
    , m_is_ok(false)
    , m_size(0)
    , m_round_robin_replacement_policy(false)
    , parm_size("size", this,
                m_size,
                "Size of the cache in bytes. Avalaible sizes are 4KB, 8KB, 16KB,"
                " 32KB, 64KB and 128KB."
                " The cache can be deactivated setting this value to 0.")
    , stat_read_accesses("read-accesses", this,
                         read_accesses,
                         "Number of read accesses to the cache.")
    , stat_write_accesses("write-accesses", this,
                          write_accesses,
                          "Number of write accesses to the cache.")
    , stat_prefetch_accesses("prefetch-accesses", this,
                             prefetch_accesses,
                             "Number of prefetch accesses to the cache.")
    , form_accesses("accesses", this,
                    unisim::kernel::service::Formula<uint32_t>::OP_ADD,
                    &stat_read_accesses, &stat_write_accesses, &stat_prefetch_accesses,
                    "Number of accesses to the cache.")
    , stat_read_hits("read-hits", this,
                     read_hits,
                     "Number of read hit accesses to the cache.")
    , stat_write_hits("write-hits", this,
                      write_hits,
                      "Number of write hit accesses to the cache.")
    , stat_prefetch_hits("prefetch-hits", this,
                         prefetch_hits,
                         "Number of prefetch hit accesses to the cache.")
    , form_hits("hits", this,
                unisim::kernel::service::Formula<uint32_t>::OP_ADD,
                &stat_read_hits, &stat_write_hits, &stat_prefetch_hits,
                "Number of hit accesses to the cache.")
    , form_hit_rate("hit-rate", this,
                    unisim::kernel::service::Formula<double>::OP_DIV,
                    &form_hits, &form_accesses, 0,
                    "Cache hit rate.")
  {
    for (uint32_t set = 0; set < m_sets_; set++)
      {
        for (uint32_t way = 0; way < m_associativity_; way++)
          {
            m_tag[set][way] = 0;
            memset(m_data[set][way], 0, m_line_size_);
            m_valid[set][way] = false;
            m_dirty[set][way] = false;
          }
        m_replacement_history[set] = 0;
      }
    parm_size.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
    stat_read_accesses.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
    stat_write_accesses.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
    stat_prefetch_accesses.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
    form_accesses.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
    stat_read_hits.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
    stat_write_hits.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
    stat_prefetch_hits.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
    form_hits.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
  }

  Cache::~Cache()
  {
    // nothing to do
  }

  bool
  Cache::BeginSetup()
  {
    SetSize(m_size);

    if ( !m_is_ok )
      logger << DebugError
             << "Invalid cache size ("
             << (unsigned int)m_size
             << "). Avalaible values are 4KB, 8KB, 16KB, 32KB, 64KB and 128KB."
             << EndDebugError;

    return m_is_ok;
  }

  bool 
  Cache::SetSize(uint32_t size)
  {
    m_is_ok = false;
    // test that the size parameter is a valid value
    if ( size == 0 )
      m_is_ok = true;
    else if ( size > 0 )
      {
        // ARM caches can only be between 4KB (0x01000) and 128KB (0x020000)
        //   and must be 2^x = size
#ifdef ARM_CACHE_DEBUG
        cerr << "Cache size = 0x" << hex << size << dec
             << "(" << (unsigned int)size << ")" << std::endl;
#endif
        m_is_ok = true;
        switch ( size )
          {
          case 0x01000: // 4KB
            m_set_mask = (uint32_t)0x01f << 5;
            m_tag_mask = ~(m_set_mask | (uint32_t)0x01f);
            m_tag_shift = 10;
            break;
          case 0x02000: // 8KB
            m_set_mask = (uint32_t)0x03f << 5;
            m_tag_mask = ~(m_set_mask | (uint32_t)0x01f);
            m_tag_shift = 11;
            break;
          case 0x04000: // 16KB
            m_set_mask = (uint32_t)0x07f << 5;
            m_tag_mask = ~(m_set_mask | (uint32_t)0x01f);
            m_tag_shift = 12;
            break;
          case 0x08000: // 32KB
            m_set_mask = (uint32_t)0x0ff << 5;
            m_tag_mask = ~(m_set_mask | (uint32_t)0x01f);
            m_tag_shift = 13;
            break;
          case 0x010000: // 64KB
            m_set_mask = (uint32_t)0x01ff << 5;
            m_tag_mask = ~(m_set_mask | (uint32_t)0x01f);
            m_tag_shift = 14;
            break;
          case 0x020000: // 128KB
            m_set_mask = (uint32_t)0x03ff << 5;
            m_tag_mask = ~(m_set_mask | (uint32_t)0x01f);
            m_tag_shift = 15;
            break;
          default:
            // given size is not supported
            m_is_ok = false;
            break;
          }

#ifdef ARM_CACHE_DEBUG
        if ( m_is_ok )
          {	
            std::cerr << "m_set_mask   = 0x" << std::hex << m_set_mask << std::dec << std::endl;
            std::cerr << "m_set_shift_ = " << m_set_shift_ << std::endl;
            std::cerr << "m_tag_mask   = 0x" << std::hex << m_tag_mask << std::dec << std::endl;
            std::cerr << "m_tag_shift  = " << m_tag_shift << std::endl;
          }
#endif
      }

    return m_is_ok;
  }

  uint32_t
  Cache::GetNewWay(uint32_t set)
  {
    // apply the ARM replacement policy
    if ( m_round_robin_replacement_policy )
      {
        // current replacement policy is round robin
        m_replacement_history[set] = (m_replacement_history[set] + 1) %
          m_associativity_;
      }
    else
      {
        // current replacement policy is random
        m_replacement_history[set] =
          rand.Generate(m_associativity_/2) +
          (m_associativity_/2);
      }
    return m_replacement_history[set];
  }

  uint32_t
  Cache::GetBaseAddress(uint32_t set, uint32_t way)
    const
  {
    uint32_t tag = m_tag[set][way];
    uint32_t addr = 0;
    addr = (tag << m_tag_shift) |
      (set << m_set_shift_);
    return addr;
  }

  uint32_t
  Cache::GetBaseAddressFromAddress(uint32_t addr)
    const
  {
#ifdef ARM_CACHE_DEBUG
    cerr << "GettingAddressFromAddress" << std::endl
         << " - addr       = 0x" << hex << (unsigned long int)addr << std::endl
         << " - m_set_mask = 0x" << (unsigned long int)m_set_mask << dec
         << " (shift = " << (unsigned int)m_set_shift_ << ")" << std::endl
         << " - m_tag_mask = 0x" << hex << (unsigned long int)m_tag_mask << dec
         << " (shift = " << (unsigned int)m_tag_shift << ")" << std::endl
         << " - new_addr   = 0x" << hex
         << (unsigned long int)(addr & (m_set_mask | m_tag_mask)) << dec
         << std::endl;
#endif
    addr = addr & (m_set_mask | m_tag_mask);
    return addr;
  }

  uint32_t
  Cache::GetData(uint32_t set, uint32_t way, uint8_t **data)
  {
    *data = m_data[set][way];
    return m_line_size_;
  }

  uint32_t
  Cache::GetData(uint32_t set, uint32_t way, uint32_t index, uint32_t size, uint8_t **data)
  {
    uint32_t max_size = m_line_size_ - index;
    *data = &(m_data[set][way][index]);
    return (max_size < size) ? max_size : size;
  }

  uint32_t
  Cache::GetDataCopy(uint32_t set, uint32_t way, uint8_t *data)
    const
  {
    memcpy(data, m_data[set][way], m_line_size_);
    return m_line_size_;
  }

  uint32_t
  Cache::GetDataCopy(uint32_t set, uint32_t way, uint32_t index, uint32_t size, uint8_t *data)
    const
  {
    uint32_t max_size = m_line_size_ - index;
    size = (max_size < size) ? max_size : size;
    memcpy(data, &(m_data[set][way][index]), size);
    return size;
  }

  uint32_t
  Cache::SetData(uint32_t set, uint32_t way, uint8_t *data)
  {
    memcpy(m_data[set][way], data, m_line_size_);
    return m_line_size_;
  }

  uint32_t
  Cache::SetData(uint32_t set, uint32_t way, uint32_t index, uint32_t size, uint8_t *data)
  {
    uint32_t max_size = m_line_size_ - index;
    size = (max_size < size) ? max_size : size;
    memcpy(&(m_data[set][way][index]), data, size);
    return size;
  }
  
  void
  Cache::Invalidate()
  {
    memset(m_valid, 0, sizeof(uint8_t) * m_sets_ * m_associativity_);
  }

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim


