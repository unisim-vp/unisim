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

#include "viodisk.hh"
#include "architecture.hh"
#include "debug.hh"
#include <unisim/util/arithmetic/arithmetic.hh>
#include <iostream>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

struct SyncPackedQueue : public unisim::util::virtio::PackedQueue
{
  SyncPackedQueue() : unisim::util::virtio::PackedQueue() {}
  void sync(SnapShot& snapshot)
  {
    for (auto reg : {&size, &ready})
      snapshot.sync(*reg);
    for (auto reg : {&desc_area, &driver_area, &device_area})
      snapshot.sync(*reg);
    uint16_t idx = head.idx();
    snapshot.sync(idx);
    head.set_idx(idx);
  }
};

struct SyncSplitQueue : public unisim::util::virtio::SplitQueue
{
  SyncSplitQueue() : unisim::util::virtio::SplitQueue() {}
  void sync(SnapShot& snapshot)
  {
    struct Bad {}; raise( Bad() );
  }
};

void
VIODisk::SetupQueues(bool is_packed)
{
  delete qmgr;
  if (is_packed)
    {
      static struct : public SyncQMgr
      {
        virtual void sync(SnapShot& snapshot, unisim::util::virtio::Queue* q) const override { static_cast<SyncPackedQueue*>(q)->sync(snapshot); }
        virtual bool is_packed() const override { return true; }
      } _;
      sqmgr = &_;
      qmgr = new unisim::util::virtio::QMgrImpl<SyncPackedQueue,1>();
    }
  else
    {
      static struct : public SyncQMgr
      {
        virtual void sync(SnapShot& snapshot, unisim::util::virtio::Queue* q) const override { static_cast<SyncSplitQueue*>(q)->sync(snapshot); }
        virtual bool is_packed() const override { return false; }
      } _;
      sqmgr = &_;
      qmgr = new unisim::util::virtio::QMgrImpl<SyncSplitQueue,1>();
    }
}

VIODisk::VIODisk()
  : unisim::util::virtio::BlockDevice(), sqmgr(), WriteBack(1), diskfilename(), Capacity(0), diskpos(), disksize(), storage(), deltas()
{}

template <unsigned POS>
struct BlockClaimed
{
  typedef unisim::util::virtio::CommonFeatures CommonFeatures;
  typedef unisim::util::virtio::BlockFeatures BlockFeatures;
  typedef CommonFeatures::Code CF;
  typedef BlockFeatures::Code BF;
  static uint32_t const mask =
    unisim::util::virtio::Claimed<POS, CF, (int)CommonFeatures::end - 1>::mask |
    unisim::util::virtio::Claimed<POS, BF, (int)BlockFeatures::end - 1>::mask;
};

namespace
{
  uint32_t claimed_features(unsigned sel)
  {
    switch (sel)
      {
      case 0: return BlockClaimed< 0>::mask; // should be: 0x30006e44, 0b0011 << 28 | 0b110111001000100
      case 1: return BlockClaimed<32>::mask; // should be: 0x7, 0b111
      }
    struct Bad {};
    raise( Bad() );

    return 0;
  }
}

uint32_t
VIODisk::ClaimedFeatures()
{
  return claimed_features(DeviceFeaturesSel);
}

bool
VIODisk::UsedFeatures(uint32_t requested)
{
  struct Bad {};
  typedef unisim::util::virtio::CommonFeatures CommonFeatures;
  typedef unisim::util::virtio::BlockFeatures BlockFeatures;

  uint32_t claimed = claimed_features(DriverFeaturesSel);

  using unisim::util::arithmetic::BitScanForward;

  bool has_errors = false;
  for (uint32_t err = requested & ~claimed; err; err &= err - 1)
    {
      /* Iterating over all requested but not claimed features */
      has_errors = true;
      std::cerr << "VirtIO driver error: feature "
                << unisim::util::virtio::GetFeatureName<unisim::util::virtio::BlockFeatures>(32*DriverFeaturesSel + BitScanForward(err))
                << " requested by driver but not claimed by device\n";
    }

  for (uint32_t rem = claimed; rem; rem &= rem - 1)
    {
      /* Iterating over all claimed bits */
      unsigned wbit = BitScanForward(rem), bit = 32*DriverFeaturesSel + wbit;
      bool feature_requested = requested >> wbit & 1;
      switch (bit)
        {
        case unisim::util::virtio::Feature<CommonFeatures::Code,CommonFeatures::RING_PACKED>::BIT:
          SetupQueues(feature_requested);
          break;

        case unisim::util::virtio::Feature<BlockFeatures::Code,BlockFeatures::DISCARD>::BIT:
          // We won't receive DISCARD commands
          break;

        case unisim::util::virtio::Feature<BlockFeatures::Code,BlockFeatures::WRITE_ZEROES>::BIT:
          // We won't receive WRITE_ZEROES commands
          break;

        default:
          if (not feature_requested)
            {
              has_errors = true;
              std::cerr << "VirtIO device error: feature "
                        << unisim::util::virtio::GetFeatureName<unisim::util::virtio::BlockFeatures>(bit)
                        << " not requested by driver but claimed and requested by device\n";
            }
          break;
        }
    }

  if (has_errors)
    raise( Bad() );

  return true;
}

void
VIODisk::sync(SnapShot& snapshot)
{
  // Snapshoting unisim::util::virtio::BlockDevice
  for (auto reg : {&Status, &DeviceFeaturesSel, &DriverFeaturesSel, &ConfigGeneration, &InterruptStatus})
    snapshot.sync(*reg);

  uint8_t qtype = snapshot.is_load() or not qmgr ? 0 : sqmgr->is_packed() ? 2 : 1;
  snapshot.sync(qtype);
  if (qtype)
    {
      if (snapshot.is_load())
        SetupQueues(qtype == 2);
      sqmgr->sync(snapshot, qmgr->active());
    }

  // Snapshoting VIODisk
  snapshot.sync(WriteBack);
  snapshot.sync(diskpos);

  if (snapshot.is_save())
    {
      std::cerr << "Saving VirtIO storage " << diskfilename << "\n";
      int fd = ::open(diskfilename.c_str(), O_WRONLY);
      bool uloss = false;
      for (auto const& delta : deltas)
        {
          uint64_t base = delta.index, size = delta.fullsize();
          lseek(fd, base, SEEK_SET);
          if (::write(fd, (void const*)&storage[base], size) != int64_t(size))
            { struct Bad {}; raise(Bad()); }
          uloss |= bool(delta.udat);
        }
      if (uloss)
        std::cerr << "Losing storage tainted data...\n";
    }
}

void
VIODisk::open(char const* filename)
{
  struct Error {};
  int fd = ::open(filename,O_RDONLY);
  struct stat info;
  if (fstat(fd, &info) < 0)
    throw Error();
  Capacity = (info.st_size + BLKSIZE - 1) / BLKSIZE;
  disksize = Capacity * BLKSIZE;
  storage = (uint8_t*)mmap(0, disksize, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  ::close(fd);
  if (storage == (uint8_t*)-1)
    throw Error();
  diskfilename = filename;
}

VIODisk::~VIODisk()
{
  munmap(storage, disksize);
}

void
VIODisk::Delta::uread(uint64_t pos, uint64_t size, uint8_t* dst) const
{
  if (not udat)
  {
    std::fill(&dst[0], &dst[size], 0);
    return;
  }
  uint8_t const* src = &udat[pos % fullsize()];
  std::copy( &src[0], &src[size], dst );
}

void
VIODisk::read(unisim::util::virtio::Access const& vioa, uint64_t addr, uint64_t size)
{
  AArch64& core = dynamic_cast<Access const&>(vioa).GetCore();
  for (AArch64::SliceIterator slice(addr, size, AArch64::mem_acc_type::read); slice.pnext(core);)
    {
      {
        uint8_t* udat = slice.udat;
        auto itr = deltas.lower_bound(Delta(diskpos));

        for (uint64_t pos = diskpos, left = slice.size, psize = 0; left; left -= psize, udat += psize, pos += psize)
          {
            psize = std::min(Delta::size_from(pos), left);

            if (itr == deltas.end() or itr->index > pos)
              std::fill(&udat[0], &udat[psize], 0);
            else
              {
                itr->uread(pos, psize, udat);
                ++itr;
              }
          }
      }
      uint8_t const* src = &storage[diskpos];
      std::copy(src, src + slice.size, slice.data);
      diskpos += slice.size;
    }
}

void
VIODisk::Delta::uwrite(uint64_t pos, uint64_t size, uint8_t const* src) const
{
  bool partial = pos == index and size == fullsize();
  
  if ((partial and udat) or std::any_of(&src[0], &src[size], [](uint8_t b) { return b != 0; } ))
    {
      if (not udat)
        {
          udat = new uint8_t[fullsize()];
          if (partial)
            std::fill(&udat[0], &udat[fullsize()], 0);
        }
      std::copy( &src[0], &src[size], &udat[pos % fullsize()] );
      if (not partial or not std::any_of(&udat[0], &udat[fullsize()], [](uint8_t b) { return b != 0; } ))
        return;
    }
  
  delete [] udat;
  udat = 0;
}

void
VIODisk::write(unisim::util::virtio::Access const& vioa, uint64_t addr, uint64_t size)
{
  AArch64& core = dynamic_cast<Access const&>(vioa).GetCore();
  for (AArch64::SliceIterator slice(addr, size, AArch64::mem_acc_type::write); slice.pnext(core);)
    {
      {
        uint8_t* udat = slice.udat;
        auto itr = deltas.lower_bound(Delta(diskpos));

        for (uint64_t pos = diskpos, left = slice.size, psize = 0; left; left -= psize, udat += psize, pos += psize)
          {
            psize = std::min(Delta::size_from(pos), left);
      
            if (itr == deltas.end() or itr->index > pos)
              itr = deltas.insert(itr, Delta(pos));

            itr->uwrite(pos, psize, udat);
            ++itr;
          }
      }
      std::copy(slice.data, slice.data + slice.size, &storage[diskpos]);
      diskpos += slice.size;
    }
}

void
VIODisk::flush(unisim::util::virtio::Access const& vioa)
{
	::msync(storage, disksize, MS_SYNC);
}

void
VIODisk::discard(unisim::util::virtio::Access const& vioa, uint64_t pos, uint64_t size)
{
}

void
VIODisk::write_zeroes(unisim::util::virtio::Access const& vioa, uint64_t pos, uint64_t size, uint32_t flags)
{
	::memset(storage + pos, 0, size);
}
