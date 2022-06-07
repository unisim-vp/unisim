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

#include <unisim/util/arithmetic/arithmetic.hh>
#include <viodisk.hh>
#include <debug.hh>
#include <iostream>

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
    throw 0;
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
      throw 0;
    }
}

VIODisk::VIODisk()
  : unisim::util::virtio::Device(), sqmgr(), Capacity(0), WriteBack(1)
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

  struct GetFeatureName
  {
    GetFeatureName(unsigned _bit) : bit(_bit) {} unsigned bit;
    friend std::ostream& operator << (std::ostream& sink, GetFeatureName const& arg)
    {
      typedef unisim::util::virtio::CommonFeatures CommonFeatures;
      typedef unisim::util::virtio::BlockFeatures BlockFeatures;

      if      (char const* name = unisim::util::virtio::NameGetter<CommonFeatures::Code,(int)CommonFeatures::end - 1>::feature(arg.bit)) sink << name;
      else if (char const* name = unisim::util::virtio::NameGetter< BlockFeatures::Code, (int)BlockFeatures::end - 1>::feature(arg.bit)) sink << name;
      else sink << "unknown_feature(" << std::dec << arg.bit << ")";
      return sink;
    }
  };
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
      std::cerr << "VirtIO driver error: feature " << GetFeatureName(32*DriverFeaturesSel + BitScanForward(err)) << " requested by driver but not claimed by device\n";
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
              std::cerr << "VirtIO device error: feature " << GetFeatureName(bit) << " not requested by driver but claimed and requested by device\n";
            }
          break;
        }
    }

  if (has_errors)
    raise( Bad() );

  return true;
}

bool
VIODisk::ReadQueue(unisim::util::virtio::Access const& sys)
{
  struct Request : public unisim::util::virtio::QProc
  {
    Request(unisim::util::virtio::Access const& _sys, VIODisk& _disk) : sys(_sys), disk(_disk), state(Head), type(), status() {}
    unisim::util::virtio::Access const& sys; VIODisk& disk;
    enum { Head, Body, Status } state;
    enum Type { In = 0, Out, Flush = 4, GetID = 8, Discard = 11, WriteZeroes = 13 } type;
    enum { OK, IOERR, UNSUPP } status;

    virtual uint32_t process(uint64_t buf, uint32_t len, uint16_t flags, bool last) override
    {
      uint32_t wlen = 0;
      // bool is_write = VIOQFlags::WRITE.Get(flags);
      bool is_write = flags & 2;
      //std::cerr << std::hex << buf << ',' << len << ',' << (is_write ? 'w' : 'r') << std::endl;


      struct Bad {};

      if (state == Body and last) state = Status;

      switch (state)
        {
        case Head:
          if (is_write or len != 16) { std::cerr << "error: expected 16 byte header\n"; raise( Bad() ); }
          //std::cerr << "VirtIO Block Header: {type=" << sys.read(buf + 0, 4) << ", sector=" << sys.read(buf + 8, 8) << "}\n";
          switch (type = Type(sys.read(buf + 0, 4)))
            {
            default:
              raise( Bad() );
              break;
            case GetID:
              /* TODO: support this command ? */
              break;
            case Flush:
              /* No data, all work should be done here. */
              /* Nothing to do for now */
              break;
            case In: case Out:
              disk.seek(VIODisk::BLKSIZE*sys.read(buf + 8, 8));
              break;
            }

          state = Body;
          break;

        case Body:
          switch (type)
            {
            default:
              status = UNSUPP;
              break;
            case In:
              if (not is_write or len >= 0x100000) { std::cerr << "error: too large buffer\n"; raise( Bad() ); }
              disk.read(sys, buf, len);
              wlen += len;
              break;
            case Out:
              if (is_write or len >= 0x100000) { std::cerr << "error: too large buffer\n"; raise( Bad() ); }
              disk.write(sys, buf, len);
              break;
            case Flush: std::cerr << "TODO: Flush\n"; raise( Bad() );
            case Discard: std::cerr << "TODO: Discard\n"; raise( Bad() );
            case WriteZeroes: std::cerr << "TODO: WriteZeroes\n"; raise( Bad() );
            }
          break;

        case Status:
          if (not is_write or len != 1) { std::cerr << "error: expected a 1 byte footer\n"; raise( Bad() ); }
          sys.write(buf, 1, status);
          wlen += 1;
          state = Head;
          break;
        }
      return wlen;
    }
  } req(sys, *this);

  QueueRead(sys, req);

  return true;
}

void
VIODisk::sync(SnapShot& snapshot)
{
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

  snapshot.sync(WriteBack);
  uint64_t diskpos = tell();
  snapshot.sync(diskpos);
  seek(diskpos);
}

