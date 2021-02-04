/*
 *  Copyright (c) 2019-2020,
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

#ifndef __ARM64VP_VIODISK_HH__
#define __ARM64VP_VIODISK_HH__

#include <inttypes.h>
#include <fstream>

struct VIOAccess
{
  virtual ~VIOAccess() {}
  struct Slice { uint8_t* bytes; uint64_t size; };
  virtual Slice access(uint64_t addr, uint64_t size, bool is_write) const = 0;
  virtual void dcapture(uint64_t addr, uint64_t size) const = 0;
  virtual void notify() const = 0;
  
  struct Iterator : Slice
  {
    Iterator(uint64_t addr, uint64_t size, bool _write) : slice{0,0}, ptr(addr), left(size), write(_write) {}
    Slice slice;
    uint64_t ptr, left;
    bool write;
    bool next(VIOAccess const& vioa);
  };
  
  uint64_t read(uint64_t addr, unsigned size) const;
  void write(uint64_t addr, unsigned size, uint64_t value) const;
};

struct VIOQueue
{
  struct DescIterator
  {
    DescIterator() : index(0), wrap(1) {}
    unsigned wrap_counter() const { return index >> 15 & 1; }
    uint64_t get_offset() const { return (index & 0x7fff); }
    void next(VIOQueue const& vq) { if (++index < vq.size) return; index = 0; wrap ^= 1;  }
    bool available(uint16_t flags) const { return (flags >> 15 ^ wrap) & ~(flags >> 7 ^ wrap) & 1; }
    uint16_t used(bool write) const { return wrap << 15 | wrap << 7 | int(write) << 1; }
    uint16_t idx() const { return (index & 0x7fff) | (wrap << 15); }
    unsigned index : 16;
    unsigned wrap  :  1;
  };

  //enum { NEXT=1, WRITE=2, INDIRECT=4 };

  VIOQueue() : size(0), ready(0), desc_area(0), driver_area(0), device_area(0), head() {}
  uint64_t desc_addr(DescIterator const& desc) const { return desc_area + 16*desc.index; }

  uint32_t size, ready;
  uint64_t desc_area, driver_area, device_area;
  DescIterator head;
};


struct VIODisk
{
  VIODisk();
  void reset();
  void open(char const* filename);
  
  // Generic Config
  static uint32_t Vendor() { return 0x70767375; }
  static uint32_t QueueNumMax() { return 1024; }
  uint32_t ClaimedFeatures();
  bool UsedFeatures(uint32_t);
  bool CheckFeatures();
  bool CheckStatus();
  bool InterruptAck(uint32_t mask) { InterruptStatus &= ~mask; return true; }
  bool ReadQueue(VIOAccess const& vioa);
  bool SetupQueue(VIOAccess const& vioa);
  
  // Block Device Config
  static uint32_t SegMax() { return 254; }
  static uint32_t BlkSize() { return 512; }
  static uint32_t DiscardSectorAlignment() { return 1; }
  static uint32_t MaxDiscardSectors() { return 0x3fffff; }
  static uint32_t MaxDiscardSeg() { return 1; }
  static uint32_t MaxWriteZeroesSectors() { return 0x3fffff; }
  
  // Generic Config
  uint32_t Status, Features, DeviceFeaturesSel, DriverFeaturesSel, ConfigGeneration, InterruptStatus;

  // Block Device Config
  VIOQueue rq;
  std::fstream storage;
  uint64_t Capacity;
  uint8_t  WriteBack;
};

template <typename FEATTYPE, FEATTYPE FEAT> struct Feature {};

struct CommonFeatures
{
  enum Code { RING_INDIRECT_DESC, RING_EVENT_IDX, VERSION_1, ACCESS_PLATFORM, RING_PACKED, end };
};

#define COMMON_FEATURE(FEAT,BPOS) template <> struct Feature<CommonFeatures::Code,CommonFeatures::FEAT> { enum {BIT=BPOS}; static char const* name() { return #FEAT; } }

COMMON_FEATURE(   RING_INDIRECT_DESC, 28); // Device supports buffer indirection
COMMON_FEATURE(       RING_EVENT_IDX, 29); // used_event and avail_event availables
COMMON_FEATURE(            VERSION_1, 32); // This indicates compliance with this specification
COMMON_FEATURE(      ACCESS_PLATFORM, 33); // Device supports IOMMU
COMMON_FEATURE(          RING_PACKED, 34); // Device supports the packed virtqueue layout
// COMMON_FEATURE(          IN_ORDER, 35); // Device supports in-order use of buffers
// COMMON_FEATURE(    ORDER_PLATFORM, 36); // Device needs platform ordering
// COMMON_FEATURE(            SR_IOV, 37); // Device supports Single Root I/O Virtualization
// COMMON_FEATURE( NOTIFICATION_DATA, 38); // Device support extended notifications

#undef COMMON_FEATURE

struct BlockFeatures
{
  enum Code { SEG_MAX, BLK_SIZE, FLUSH, TOPOLOGY, CONFIG_WCE, DISCARD, WRITE_ZEROES, end };
};

#define BLOCK_FEATURE(FEAT,BPOS) template <> struct Feature<BlockFeatures::Code,BlockFeatures::FEAT> { enum {BIT=BPOS}; static char const* name() { return #FEAT; } }

// BLOCK_FEATURE(            BARRIER,  0); // Device supports request barriers (legacy).
// BLOCK_FEATURE(           SIZE_MAX,  1); // Maximum size of a segment is in size_max.
BLOCK_FEATURE(               SEG_MAX,  2); // Maximum number of segments in a request is in seg_max.
// BLOCK_FEATURE(           GEOMETRY,  4); // Disk-style geometry specified in geometry.
// BLOCK_FEATURE(                 RO,  5); // Device is read-only.
BLOCK_FEATURE(              BLK_SIZE,  6); // Block size of disk is in blk_size.
// BLOCK_FEATURE(               SCSI,  7); // Device supports scsi packet commands (legacy).
BLOCK_FEATURE(                 FLUSH,  9); // Cache flush command support (a.k.a WCE in legacy).
BLOCK_FEATURE(              TOPOLOGY, 10); // Device exports information on optimal I/O alignment.
BLOCK_FEATURE(            CONFIG_WCE, 11); // Device can toggle its cache between writeback and writethrough modes.
BLOCK_FEATURE(               DISCARD, 13); // Support discard command with max_discard_sectors and max_discard_seg limits.
BLOCK_FEATURE(          WRITE_ZEROES, 14); // Support write zeroes command with max_write_zeroes_sectors and max_write_zeroes_seg limits.

#undef BLOCK_FEATURE

#endif /* __ARM64VP_VIODISK_HH__ */

