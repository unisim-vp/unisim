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

VIODisk::VIODisk()
  : Status(0), Features(), DeviceFeaturesSel(), DriverFeaturesSel(), ConfigGeneration(0)
  , rq(), Capacity(0), WriteBack(1), ring_packed(true)
{}

void
VIODisk::reset()
{
  Status = 0;
  Features = 0;
  DeviceFeaturesSel = 0;
  DriverFeaturesSel = 0;
  ConfigGeneration = 0;
  rq = VIOQueue();
  WriteBack = 1;
}

template <unsigned POS, typename FEATTYPE, int FEAT>
struct Claimed
{
  enum { BIT = int(Feature<FEATTYPE,FEATTYPE(FEAT)>::BIT) - POS };
  static uint32_t const bit = (BIT < 0) ? 0 : (BIT >= 32) ? 0 : 1 << BIT;
  static uint32_t const mask = bit | Claimed<POS, FEATTYPE, FEAT - 1>::mask;
};

template <unsigned POS, typename FEATTYPE>
struct Claimed<POS, FEATTYPE, -1>
{
  static uint32_t const mask = 0;
};

template <unsigned POS>
struct BlockClaimed
{
  typedef CommonFeatures::Code CF;
  typedef BlockFeatures::Code BF;
  static uint32_t const mask = Claimed<POS, CF, (int)CommonFeatures::end - 1>::mask | Claimed<POS, BF, (int)BlockFeatures::end - 1>::mask;
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

namespace
{
  template <typename FEATTYPE, int FEAT>
  struct NameGetter
  {
    typedef Feature<FEATTYPE,FEATTYPE(FEAT)> Feat;
    static char const* feature(unsigned bit)
    {
      if (bit == Feat::BIT) return Feat::name();
      return NameGetter<FEATTYPE, FEAT - 1>::feature(bit);
    }
  };

  template <typename FEATTYPE> struct NameGetter<FEATTYPE, -1> { static char const* feature(unsigned bit) { return 0; } };

  struct GetFeatureName
  {
    GetFeatureName(unsigned _bit) : bit(_bit) {} unsigned bit;
    friend std::ostream& operator << (std::ostream& sink, GetFeatureName const& arg)
    {
      if      (char const* name = NameGetter<CommonFeatures::Code,(int)CommonFeatures::end - 1>::feature(arg.bit)) sink << name;
      else if (char const* name = NameGetter< BlockFeatures::Code, (int)BlockFeatures::end - 1>::feature(arg.bit)) sink << name;
      else sink << "unknown_feature(" << std::dec << arg.bit << ")";
      return sink;
    }
  };
}

bool
VIODisk::UsedFeatures(uint32_t requested)
{
  struct Bad {};

  if (not ring_packed)
    {
      std::cerr << "should support split queues.\n";
      raise(Bad());
    }
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
        case Feature<CommonFeatures::Code,CommonFeatures::RING_PACKED>::BIT:
          ring_packed = feature_requested;
          break;

        case Feature<BlockFeatures::Code,BlockFeatures::DISCARD>::BIT:
          // We won't receive DISCARD commands
          break;

        case Feature<BlockFeatures::Code,BlockFeatures::WRITE_ZEROES>::BIT:
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
VIODisk::CheckStatus()
{
  uint32_t status = Status;

  static struct { uint32_t mask; char const* name; }
  const fields[] =
    {
     {1,"acknowledged"},
     {2,"driver found"}, {128,"failed"}, {8,"features ok"}, {4,"driver ok"}, {64,"driver needs reset"}
    };

  uint32_t handled = 0;
  std::cerr << "VIODisk status: {";
  char const* sep = "";
  for (unsigned idx = sizeof (fields) / sizeof (fields[0]); idx-- > 0;)
    {
      if (status & fields[idx].mask)
        {
          std::cerr << sep << fields[idx].name;
          sep = ", ";
        }
      handled |= fields[idx].mask;
    }
  std::cerr << "}\n";
  
  struct Bad {};
  
  if (status & 128)
    {
      std::cerr << "unhandled <failed> status bit.\n";
      raise( Bad() );
    }
  
  if (status & ~handled)
    {
      std::cerr << "| unhandled status bits: " << std::hex << status << ", Status=" << Status << "\n";
      raise( Bad() );
    }

  return true;
}

bool
VIODisk::SetupQueue(VIOAccess const& vioa)
{
  /* {desc = x, flags = enable} */
  vioa.write(rq.device_area, 4, 0);
  return true;
}

bool
VIODisk::ReadQueue(VIOAccess const& vioa)
{
  if (not rq.ready)
    return true;
  uint32_t pqes = vioa.read(rq.driver_area, 4);
  
  struct Request
  {
    Request(VIOAccess const& _vioa, VIODisk& _disk) : state(Head), vioa(_vioa), disk(_disk), type(), status() {}
    enum { Head, Body, Status } state; VIOAccess const& vioa; VIODisk& disk;
    enum Type { In = 0, Out, Flush = 4, GetID = 8, Discard = 11, WriteZeroes = 13 } type;
    enum { OK, IOERR, UNSUPP } status;

    uint32_t process(uint64_t buf, uint32_t len, uint16_t flags, bool last)
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
          //std::cerr << "VirtIO Block Header: {type=" << vioa.read(buf + 0, 4) << ", sector=" << vioa.read(buf + 8, 8) << "}\n";
          switch (type = Type(vioa.read(buf + 0, 4)))
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
              disk.seek(VIODisk::BLKSIZE*vioa.read(buf + 8, 8));
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
              disk.read(vioa, buf, len);
              wlen += len;
              break;
            case Out:
              if (is_write or len >= 0x100000) { std::cerr << "error: too large buffer\n"; raise( Bad() ); }
              disk.write(vioa, buf, len);
              break;
            case Flush: std::cerr << "TODO: Flush\n"; raise( Bad() );
            case Discard: std::cerr << "TODO: Discard\n"; raise( Bad() );
            case WriteZeroes: std::cerr << "TODO: WriteZeroes\n"; raise( Bad() );
            }
          break;

        case Status:
          if (not is_write or len != 1) { std::cerr << "error: expected a 1 byte footer\n"; raise( Bad() ); }
          vioa.write(buf, 1, status);
          wlen += 1;
          state = Head;
          break;
        }
      return wlen;
    }
  } req(vioa, *this);

  bool notification = false;
  for (;;)
    {
      uint64_t desc_addr = rq.desc_addr(rq.head);
      uint16_t flags = vioa.read(desc_addr + 14, 2);
      if (not rq.head.available(flags))
        break;
      bool last = not (flags & 1);
      
      VIOQueue::DescIterator tail(rq.head);
      uint32_t wlen = 0;

      for (;;)
        {
          uint64_t buf_addr =   vioa.read(desc_addr + 0, 8);
          uint32_t buf_len =    vioa.read(desc_addr + 8, 4);
          //if (VIOQFlags::INDIRECT.Get(flags))
          if (flags & 4)
            {
              uint64_t desc_addr = buf_addr, buf_end = buf_addr + buf_len;
              for (bool cont = true; cont; )
                {
                  uint64_t buf_addr =  vioa.read(desc_addr +  0, 8);
                  uint32_t buf_len =   vioa.read(desc_addr +  8, 4);
                  uint16_t buf_flags = vioa.read(desc_addr + 14, 2);

                  desc_addr += 16;
                  cont = desc_addr < buf_end;
              
                  wlen += req.process(buf_addr, buf_len, buf_flags, last and not cont);
                }
            }
          else
            {
              wlen += req.process(buf_addr, buf_len, flags, last);
            }
      
          rq.head.next(rq);
          // if (VIOQFlags::NEXT.Get(flags));
          if (last)
            break;
          
          desc_addr = rq.desc_addr(rq.head);
          flags =  vioa.read(desc_addr + 14, 2);
        }

      uint16_t id = vioa.read(desc_addr + 12, 2);
      // std::cerr << "ID:" << id << std::endl;

      uint64_t tail_addr = rq.desc_addr(tail);
      vioa.write(tail_addr +  8, 4, wlen);
      vioa.write(tail_addr + 12, 2, id);
      vioa.write(tail_addr + 14, 2, tail.used(wlen > 0));

      if (not notification)
        {
          switch (pqes >> 16 & 3)
            {
            default: { std::cerr << "error: bad flag value.\n"; struct Bad {}; raise( Bad() ); }
            case 0: /*  ENABLE */ notification = true; break;
            case 1: /* DISABLE */ break;
            case 2: /*    DESC */ notification = tail.idx() == (pqes & 0xffff); break;
            }
        }
    }

  if (notification)
    {
      InterruptStatus |= 1;
      vioa.notify();
    }
  return true;
}

void
VIOQueue::sync(SnapShot& snapshot)
{
  snapshot.sync(size);
  snapshot.sync(ready);
  for (auto reg : {&desc_area, &driver_area, &device_area})
    snapshot.sync(*reg);
  head.sync(snapshot);
}

void
VIODisk::sync(SnapShot& snapshot)
{
  for (auto reg : {&Status, &Features, &DeviceFeaturesSel, &DriverFeaturesSel, &ConfigGeneration, &InterruptStatus})
    snapshot.sync(*reg);
  rq.sync(snapshot);
  snapshot.sync(WriteBack);
  uint64_t diskpos = tell();
  snapshot.sync(diskpos);
  seek(diskpos);
}

void
VIOQueue::DescIterator::sync(SnapShot& snapshot)
{
  snapshot.sync(index);
  snapshot.sync(wrap);
}

