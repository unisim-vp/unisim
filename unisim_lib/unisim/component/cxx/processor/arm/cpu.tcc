/*
 *  Copyright (c) 2007-2015,
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
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 *  SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 *  SUCH DAMAGE.
 *
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr), Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */

 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_TCC__

#include <unisim/component/cxx/processor/arm/cpu.hh>
#include <unisim/component/cxx/processor/arm/models.hh>
#include <unisim/util/likely/likely.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/util/endian/endian.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {

/** Constructor.
 * Initializes CPU
 *
 * @param name the name that will be used by the UNISIM service 
 *   infrastructure and will identify this object
 * @param parent the parent object of this object
 */
  
template <class _CONFIG_>
CPU<_CONFIG_>::CPU(const char *name, Object *parent)
  : unisim::kernel::service::Object(name, parent)
  , unisim::component::cxx::processor::arm::CP15Interface()
  , dcache("dcache", this)
  , exception(0)
  , cp15(this, "cp15", this)
{
  // Initialize general purpose registers
  for(unsigned int i = 0; i < num_log_gprs; i++)
    gpr[i] = 0;
  this->current_pc = 0;
  this->next_pc = 0;
  
  // Initialize ARM Modes (TODO: modes should be conditionnaly selected based on CONFIG)
  modes[0b10000] = new Mode( "usr" ); // User mode (No banked regs, using main regs)
  modes[0b10001] = new BankedMode<0b0111111100000000>( "fiq" ); // FIQ mode
  modes[0b10010] = new BankedMode<0b0110000000000000>( "irq" ); // IRQ mode
  modes[0b10011] = new BankedMode<0b0110000000000000>( "svc" ); // Supervisor mode
  modes[0b10110] = new BankedMode<0b0110000000000000>( "mon" ); // Monitor mode
  modes[0b10111] = new BankedMode<0b0110000000000000>( "abt" ); // Abort mode
  modes[0b11010] = new BankedMode<0b0110000000000000>( "hyp" ); // Hyp mode
  modes[0b11011] = new BankedMode<0b0110000000000000>( "und" ); // Undefined mode
  modes[0b11111] = new Mode( "sys" ); // System mode (No banked regs, using main regs)

  // TODO: Provide access to Banked Registers
  // for (unsigned int i = 0; i < num_phys_gprs; i++)
  //   {
  //     std::stringstream ss, ss_desc;
  //     ss << "PHYS_GPR[" << i << "]";
  //     ss_desc << "Physical register " << i;
  //     reg_phys_gpr[i] =  
  //       new unisim::kernel::service::Register<uint32_t>( ss.str().c_str(), this, phys_gpr[i], ss_desc.str().c_str() );
  //   }
  // TODO: provide unisim registers for SPSRs
  // for (unsigned int i = 0; i < num_phys_spsrs; i++)
  //   {
  //     std::stringstream ss, ss_desc;
  //     ss << "SPSR[" << i << "]";
  //     ss_desc << "SPSR[" << i << "] register";
  //     reg_spsr[i] =
  //       new unisim::kernel::service::Register<uint32_t>(ss.str().c_str(), this, spsr[i].m_value, ss_desc.str().c_str());
  //   }

}

/** Performs a prefetch access.
 *
 * @param addr the address of the memory prefetch access
 */
template <class _CONFIG_>
void 
CPU<_CONFIG_>::PerformPrefetchAccess( uint32_t addr )
{
  if ( likely(dcache.GetSize()) )
    {
      dcache.prefetch_accesses++;
      uint32_t cache_tag = dcache.GetTag(addr);
      uint32_t cache_set = dcache.GetSet(addr);
      uint32_t cache_way;
      bool cache_hit = false;
      if ( dcache.GetWay(cache_tag, cache_set, &cache_way) )
        {
          if ( dcache.GetValid(cache_set, cache_way) )
            {
              // the access is a hit, nothing needs to be done
              cache_hit = true;
            }
        }
      // if the access was a miss, data needs to be fetched from main
      //   memory and placed into the cache
      if ( likely(!cache_hit) )
        {
          // get a way to replace
          cache_way = dcache.GetNewWay(cache_set);
          // get the valid and dirty bits from the way to replace
          bool cache_valid = dcache.GetValid(cache_set,
                                             cache_way);
          bool cache_dirty = dcache.GetDirty(cache_set,
                                             cache_way);
          if ( cache_valid & cache_dirty )
            {
              // the cache line to replace is valid and dirty so it needs
              //   to be sent to the main memory
              uint8_t *rep_cache_data = 0;
              uint32_t rep_cache_address =
                dcache.GetBaseAddress(cache_set,
                                      cache_way);
              dcache.GetData(cache_set, cache_way,
                             &rep_cache_data);
              PrWrite(rep_cache_address, rep_cache_data,
                      dcache.LINE_SIZE);
            }
          // the new data can be requested
          uint8_t *cache_data = 0;
          uint32_t cache_address =
            dcache.GetBaseAddressFromAddress(addr);
          // when getting the data we get the pointer to the cache line
          //   containing the data, so no need to write the cache
          //   afterwards
          uint32_t cache_line_size = dcache.GetData(cache_set,
                                                    cache_way, &cache_data);
          PrRead(cache_address, cache_data,
                 cache_line_size);
          dcache.SetTag(cache_set, cache_way, cache_tag);
          dcache.SetValid(cache_set, cache_way, 1);
          dcache.SetDirty(cache_set, cache_way, 0);
        }
      else
        {
          dcache.prefetch_hits++;
        }
      if ( unlikely(dcache.power_estimator_import != 0) )
        dcache.power_estimator_import->ReportReadAccess();
    }
  else
    {
      /* it is just a cache prefetch, ignore the request if there is 
       * no cache */
    }
  /* CHECK: should we report a memory access for a prefetch???? */
}

/** Performs a write access.
 * @param addr the address of the memory write access
 * @param size the size of the memory write access
 * @param value the value of the memory write access
 */
template <class _CONFIG_>
void 
CPU<_CONFIG_>::PerformWriteAccess( uint32_t addr, uint32_t size, uint32_t value )
{
  uint32_t write_addr = addr;
  uint8_t data[4];

  if (size > 4) throw 0; // should never happen

  if (GetEndianness() == unisim::util::endian::E_BIG_ENDIAN) {
    // fix the write address according to request size when big endian
    write_addr ^= ((-size) & 3);
    // TODO: need to check that because it seems astonishingly incorrect
    uint32_t shifter = value;
    for (int byte = size; --byte >= 0;)
      { data[byte] = shifter; shifter >>= 8; }
  } else {
    uint32_t shifter = value;
    for (int byte = 0; byte < int( size ); ++byte)
      { data[byte] = shifter; shifter >>= 8; }
  }

  if ( likely(dcache.GetSize()) )
    {
      dcache.write_accesses++;
      uint32_t cache_tag = dcache.GetTag(write_addr);
      uint32_t cache_set = dcache.GetSet(write_addr);
      uint32_t cache_way;
      bool cache_hit = false;
      if ( dcache.GetWay(cache_tag, cache_set, &cache_way) )
        {
          if ( dcache.GetValid(cache_set, cache_way) != 0 )
            {
              // the access is a hit
              cache_hit = true;
            }
        }
      // if the access was a hit the data needs to be written into
      //   the cache, if the access was a miss the data needs to be
      //   written into memory, but the cache doesn't need to be updated
      if ( likely(cache_hit) )
        {
          dcache.write_hits++;
          uint32_t cache_index = dcache.GetIndex(write_addr);
          dcache.SetData(cache_set, cache_way, cache_index,
                         size, data);
          dcache.SetDirty(cache_set, cache_way, 1);
        }
      else
        {
          PrWrite(write_addr, data, size);
        }

      if ( unlikely(dcache.power_estimator_import != 0) )
        dcache.power_estimator_import->ReportWriteAccess();
    }
  else // there is no data cache
    {
      // there is no data cache, so just send the request to the
      //   memory interface
      PrWrite(write_addr, data, size);
    }

  /* report read memory access if necessary */
  ReportMemoryAccess(unisim::util::debug::MAT_WRITE, unisim::util::debug::MT_DATA, addr, size);
}

/** Performs a read access.
 * @param addr the address of the memory read access
 * @param size the size of the memory read access
 * @param _signed the nature of the memory read access (signed or unsigned)
 */
template <class _CONFIG_>
uint32_t
CPU<_CONFIG_>::PerformReadAccess(	uint32_t addr, uint32_t size, bool _signed )
{
  uint32_t read_addr = addr & ~(uint32_t)(size - 1);
  uint8_t data[4];

  if (size > 4) throw 0;
  uint32_t misalignment = addr & (size-1);

  // fix the read address depending on the request size and endianess
  if (GetEndianness() == unisim::util::endian::E_BIG_ENDIAN) {
    // TODO: need to check that because it seems astonishingly
    // incorrect to perform address masking and endianness conversion
    read_addr ^= ((-size) & 3);
  }

  if ( likely(dcache.GetSize()) )
    {
      dcache.read_accesses++;
      uint32_t cache_tag = dcache.GetTag(read_addr);
      uint32_t cache_set = dcache.GetSet(read_addr);
      uint32_t cache_way;
      bool cache_hit = false;
      if ( dcache.GetWay(cache_tag, cache_set, &cache_way) )
        {
          if ( dcache.GetValid(cache_set, cache_way) )
            {
              // the access is a hit, nothing needs to be done
              cache_hit = true;
            }
        }
      // if the access was a miss, data needs to be fetched from main
      //   memory and placed into the cache
      if ( unlikely(!cache_hit) )
        {
          // get a way to replace
          cache_way = dcache.GetNewWay(cache_set);
          // get the valid and dirty bits from the way to replace
          uint8_t cache_valid = dcache.GetValid(cache_set,
                                                cache_way);
          uint8_t cache_dirty = dcache.GetDirty(cache_set,
                                                cache_way);

          if ( (cache_valid != 0) & (cache_dirty != 0) )
            {
              // the cache line to replace is valid and dirty so it needs
              //   to be sent to the main memory
              uint8_t *rep_cache_data = 0;
              uint32_t rep_cache_address =
                dcache.GetBaseAddress(cache_set, cache_way);
              dcache.GetData(cache_set, cache_way, &rep_cache_data);
              PrWrite(rep_cache_address, rep_cache_data,
                      dcache.LINE_SIZE);
            }
          // the new data can be requested
          uint8_t *cache_data = 0;
          uint32_t cache_address =
            dcache.GetBaseAddressFromAddress(read_addr);
          // when getting the data we get the pointer to the cache line
          //   containing the data, so no need to write the cache
          //   afterwards
          uint32_t cache_line_size = dcache.GetData(cache_set,
                                                    cache_way, &cache_data);
          PrRead(cache_address, cache_data,
                 cache_line_size);
          dcache.SetTag(cache_set, cache_way, cache_tag);
          dcache.SetValid(cache_set, cache_way, 1);
          dcache.SetDirty(cache_set, cache_way, 0);
        }
      else
        {
          // cache hit
          dcache.read_hits++;
        }

      // at this point the data is in the cache, we can read it from the
      //   cache
      uint32_t cache_index = dcache.GetIndex(read_addr);
      uint8_t* ptr;
      (void)dcache.GetData(cache_set, cache_way, cache_index, size, &ptr);
      memcpy( &data[0], ptr, size );
    }
  else // there is no data cache
    {
      // just read the data from the memory system
      PrRead(read_addr, &data[0], size);
    }

  uint32_t value;
  if (GetEndianness() == unisim::util::endian::E_LITTLE_ENDIAN) {
    uint32_t shifter = 0;
    for (int byte = size; --byte >= 0;)
      { shifter = (shifter << 8) | uint32_t( data[byte] ); }
    if ((size == 4) and (CONFIG::MODEL < ARMV6) and misalignment)
      shifter = unisim::util::arithmetic::RotateLeft( shifter, misalignment*8 );
    value = shifter;
  } else {
    uint32_t shifter = 0;
    for (int byte = 0; byte < int( size ); ++byte)
      { shifter = (shifter << 8) | uint32_t( data[byte] ); }
    if ((size == 4) and (CONFIG::MODEL < ARMV6) and misalignment)
      shifter = unisim::util::arithmetic::RotateRight( shifter, misalignment*8 );
    value = shifter;
  }

  if ( likely(dcache.GetSize()) )
    if ( unlikely(dcache.power_estimator_import != 0) )
      dcache.power_estimator_import->ReportReadAccess();

  /* report read memory access if necessary */
  ReportMemoryAccess(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, addr, size);
  // if (requires_memory_access_reporting and memory_access_reporting_import)
  //   memory_access_reporting_import->
  //     ReportMemoryAccess(unisim::util::debug::MAT_READ, unisim::util::debug::MT_DATA, addr, size);
      
  return value;
}


/** Get caches info
 *
 */
template <class _CONFIG_>
void
CPU<_CONFIG_>::GetCacheInfo(bool &unified, 
                  uint32_t &isize, uint32_t &iassoc, uint32_t &ilen,
                  uint32_t &dsize, uint32_t &dassoc, uint32_t &dlen)
{
  unified = false;
  isize = icache.GetSize();
  iassoc = icache.GetNumWays();
  ilen = icache.LINE_SIZE;
  dsize = icache.GetSize();
  dassoc = dcache.GetNumWays();
  dlen = dcache.LINE_SIZE;
}

/** Drain write buffer.
 * Perform a memory barrier by draining the write buffer.
 */
template <class _CONFIG_>
void
CPU<_CONFIG_>::DrainWriteBuffer()
{
  logger << DebugWarning
         << "TODO: Drain write buffer once implemented"
         << EndDebugWarning;
}

/** Invalidate ICache single entry using MVA
 *
 * Perform an invalidation of a single entry in the ICache using the
 *   given address in MVA format.
 *
 * @param mva the address to invalidate
 */
template <class _CONFIG_>
void 
CPU<_CONFIG_>::InvalidateICacheSingleEntryWithMVA(uint32_t init_mva)
{
  uint32_t mva = init_mva & ~(uint32_t)(dcache.LINE_SIZE - 1);

  if ( unlikely(verbose & 0x02) )
    {
      logger << DebugInfo
             << "Invalidating ICache single entry with MVA:" << std::endl
             << " - mva               = 0x" << std::hex << init_mva << std::endl
             << " - cache aligned mva = 0x" << mva << std::dec
             << EndDebugInfo;
    }

  if ( likely(cp15.IsICacheEnabled() && icache.GetSize()) )
    {
      uint32_t cache_tag = icache.GetTag(mva);
      uint32_t cache_set = icache.GetSet(mva);
      uint32_t cache_way = 0;
      bool cache_hit = false;
      if ( likely(icache.GetWay(cache_tag, cache_set, &cache_way)) )
        cache_hit = true;

      if ( likely(cache_hit) )
        {
          if ( unlikely(verbose & 0x02) )
            logger << DebugInfo
                   << "ICache hit (set = "
                   << cache_set << ", way = "
                   << cache_way << "), invalidating it"
                   << EndDebugInfo;
          icache.SetValid(cache_set, cache_way, 0);
        }
      else
        {
          if ( unlikely(verbose & 0x02) )
            logger << DebugInfo
                   << "ICache miss (set = "
                   << cache_set
                   << ")"
                   << EndDebugInfo;
        }
    }
}

/** Clean DCache single entry using MVA
 *
 * Perform a clean of a single entry in the DCache using the given
 *   address in MVA format.
 *
 * @param mva the address to clean
 * @param invalidate true if the line needs to be also invalidated
 */
template <class _CONFIG_>
void 
CPU<_CONFIG_>::CleanDCacheSingleEntryWithMVA(uint32_t init_mva, bool invalidate)
{
  uint32_t mva = init_mva & ~(uint32_t)(dcache.LINE_SIZE - 1);
  uint32_t pa = mva;

  if ( unlikely(verbose & 0x02) )
    {
      logger << DebugInfo
             << "Cleaning DCache single entry with MVA:" << std::endl
             << " - mva               = 0x" << std::hex << init_mva << std::endl
             << " - cache aligned mva = 0x" << mva << std::dec
             << EndDebugInfo;
    }

  if ( likely(cp15.IsDCacheEnabled() && dcache.GetSize()) )
    {
      uint32_t cache_tag = dcache.GetTag(mva);
      uint32_t cache_set = dcache.GetSet(mva);
      uint32_t cache_way;
      bool cache_hit = false;
      if ( likely(dcache.GetWay(cache_tag, cache_set, &cache_way)) )
        cache_hit = true;

      if ( likely(cache_hit) )
        {
          if ( unlikely(verbose & 0x02) )
            logger << DebugInfo
                   << "Cache hit (set = "
                   << cache_set << ", way = "
                   << cache_way << ")"
                   << EndDebugInfo;
          uint8_t cache_dirty = dcache.GetDirty(cache_set, cache_way);

          if ( cache_dirty != 0 )
			{
              if ( unlikely(verbose & 0x02) )
                logger << DebugInfo
                       << "Line is dirty, performing a cleaning"
                       << EndDebugInfo;
              uint8_t *data = 0;
              // translate the address
              // if ( likely(cp15.IsMMUEnabled()) )
              //   TranslateMVA(mva, pa);
              dcache.GetData(cache_set, cache_way, &data);
              if ( unlikely(verbose & 0x02) )
                {
                  logger << DebugInfo
                         << "Cleaning cache line:" << std::endl
                         << " - mva = 0x" << std::hex << mva << std::endl
                         << " - pa  = 0x" << pa << std::endl
                         << " - tag = 0x" << cache_tag << std::endl
                         << " - set = " << std::dec << cache_set << std::endl
                         << " - way = " << cache_way << std::endl
                         << " - data =" << std::hex;
                  for ( unsigned int i = 0; i < dcache.LINE_SIZE; i++ )
                    logger << " " << (unsigned int)data[i];
                  logger << std::dec
                         << EndDebugInfo;
                }
              PrWrite(pa, data, dcache.LINE_SIZE);
              dcache.SetDirty(cache_set, cache_way, 0);
			}
          else
			{
              if ( unlikely(verbose & 0x02) )
                logger << DebugInfo
                       << "Line is already cleaned, doing nothing"
                       << EndDebugInfo;
			}
          if ( invalidate )
            dcache.SetValid(cache_set, cache_way, 0);
        }
      else
        {
          if ( unlikely(verbose & 0x02) )
            logger << DebugInfo
                   << "Cache miss (set = "
                   << cache_set
                   << ")"
                   << EndDebugInfo;
        }
    }
}

/** Invalidate the caches.
 * Perform a complete invalidation of the instruction cache and/or the 
 *   data cache.
 *
 * @param insn_cache whether or not the instruction cache should be 
 *   invalidated
 * @param data_cache whether or not the data cache should be invalidated
 */
template <class _CONFIG_>
void 
CPU<_CONFIG_>::InvalidateCache(bool insn_cache, bool data_cache)
{
  if ( insn_cache )
    {
      icache.Invalidate();
    }
  if ( data_cache )
    {
      dcache.Invalidate();
    }
}

/** Invalidate the TLBs.
 * Perform a complete invalidation of the instruction TLB and/or the 
 *   data TLB.
 *
 * @param insn_tlb whether or not the instruction tlb should be invalidated
 * @param data_tlb whether or not the data tlb should be invalidated
 */
template <class _CONFIG_>
void 
CPU<_CONFIG_>::InvalidateTLB()
{
  // only the tlb needs to be invalidated, do not touch the lockdown tlb
  tlb.Invalidate();
}

/** Test and clean DCache.
 * Perform a test and clean operation of the DCache.
 *
 * @return return true if the complete cache is clean, false otherwise
 */
template <class _CONFIG_>
bool 
CPU<_CONFIG_>::TestAndCleanDCache()
{
  bool cleaned = true;
  uint32_t num_sets = dcache.GetNumSets();
  uint32_t num_ways = dcache.GetNumWays();
  uint32_t set_index = 0;
  uint32_t way_index = 0;

  uint32_t dirty_set = 0;
  uint32_t dirty_way = 0;

  for ( set_index = 0; 
        cleaned && (set_index < num_sets);
        set_index++ )
    {
      for ( way_index = 0;
            cleaned && (way_index < num_ways); 
            way_index++ )
        {
          if ( dcache.GetValid(set_index, way_index) != 0 )
			{
              if ( dcache.GetDirty(set_index, way_index) != 0 )
                {
                  cleaned = false;
                  dirty_set = set_index;
                  dirty_way = way_index;
                }
			}
        }
    }

  if ( !cleaned )
    {
      /* Get the address and data of the dirty line */
      uint32_t mva = dcache.GetBaseAddress(dirty_set, dirty_way);
      uint32_t pa = mva;
      // if ( likely(cp15.IsMMUEnabled()) )
      //   TranslateMVA(mva, pa);
      uint8_t *data = 0;
      dcache.GetData(dirty_set, dirty_way, &data);

      /* Write the data into memory */
      PrWrite(pa, data, dcache.LINE_SIZE);
      /* Set the line as clean */
      dcache.SetDirty(dirty_set, dirty_way, 0);

      /* Check if there are still dirty lines */
      cleaned = true;
      for ( set_index = dirty_set;
            cleaned && (set_index < num_sets);
            set_index++ )
        {
          for ( way_index = 0; // we might be doing some useless work
                cleaned && (way_index < num_ways);
                way_index++ )
			{
              if ( (dcache.GetValid(set_index, way_index) != 0)
                   && (dcache.GetDirty(set_index, way_index) != 0) )
                {
                  cleaned = false;
                }
			}
        }
    }

  return cleaned;
}

/** Test, clean and invalidate DCache.
 * Perform a test and clean operation of the DCache, and invalidate the
 *   complete cache if it is clean.
 *
 * @return return true if the complete cache is clean, false otherwise
 */
template <class _CONFIG_>
bool 
CPU<_CONFIG_>::TestCleanAndInvalidateDCache()
{
  bool cleaned = TestAndCleanDCache();

  if ( cleaned ) dcache.Invalidate();

  return cleaned;
}

} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_CPU_TCC__
