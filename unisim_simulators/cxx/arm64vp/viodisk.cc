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

#include <viodisk.hh>
#include <iostream>

VIODisk::VIODisk()
  : Status(0), Features(), DeviceFeaturesSel(), DriverFeaturesSel(), ConfigGeneration(0)
  , rq(), storage(), Capacity(837212), WriteBack(1)
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
    throw Bad ();
  
    return 0;
  }
}

uint32_t
VIODisk::ClaimedFeatures()
{
  return claimed_features(DeviceFeaturesSel);
}

template <typename FEATTYPE, int FEAT>
struct GetName
{
  typedef Feature<FEATTYPE,FEATTYPE(FEAT)> Feat;
  static char const* feature(unsigned bit)
  {
    if (bit == Feat::BIT) return Feat::name();
    return GetName<FEATTYPE, FEAT - 1>::feature(bit);
  }
};

template <typename FEATTYPE> struct GetName<FEATTYPE, -1> { static char const* feature(unsigned bit) { return 0; } };

namespace
{
  void feat_list_msg(std::ostream& sink, unsigned pos, uint32_t mask, char const* pre)
  {
    for (unsigned bit = 32; bit-->0;)
      if (mask >> bit & 1)
        {
          unsigned fbit = pos+bit;
          sink << pre; pre = ", ";
          if (char const* name = GetName<CommonFeatures::Code,(int)CommonFeatures::end - 1>::feature(fbit)) sink << name;
          else if (char const* name = GetName<BlockFeatures::Code,(int)BlockFeatures::end - 1>::feature(fbit)) sink << name;
          else sink << "unknown(" << fbit << ")";
        }
  }
}

bool
VIODisk::UsedFeatures(uint32_t requested)
{
  struct Bad {};
  uint32_t claimed = claimed_features(DriverFeaturesSel);
  
  if (uint32_t err = requested & ~claimed)
    {
      feat_list_msg(std::cerr, 32*DriverFeaturesSel, err, "Erroneous feature(s) activation: ");
      std::cerr << std::endl;
      throw Bad();
    }
  if (uint32_t err = claimed & ~requested)
    {
      feat_list_msg(std::cerr, 32*DriverFeaturesSel, err, "Unsupported feature(s) deactivation: ");
      std::cerr << std::endl;
      throw Bad();
    }
  
  return true;
}


bool
VIODisk::CheckStatus()
{
  uint32_t status = Status;
  std::cerr << "VIODisk is ";
  if      (status & 1)
    { std::cerr << "|ack"; status &= ~1; }
  if (status & 2)
    { std::cerr << "|don"; status &= ~2; }
  if (status & 4)
    { std::cerr << "|dok"; status &= ~4; }
  if (status & 8)
    { std::cerr << "|fok"; status &= ~8; }
  if (status & 128)
    {
      std::cerr << "| <FAILED>\n";
      return false;
    }
  if (status)
    {
      std::cerr << "| unhandled status bits: " << std::hex << status << ", Status=" << Status << "\n";
      return false;
    }
  
  std::cerr << "|\n";
  return true;
}

bool
VIODisk::ReadQueue(VIOAccess const& vioa)
{
  if (not rq.ready)
    return true;
  
  struct Request
  {
    Request(VIOAccess const& _vioa, VIODisk& _disk) : state(Head), vioa(_vioa), disk(_disk), type() {}
    enum { Head, Body, Status } state; VIOAccess const& vioa; VIODisk& disk;
    enum Type { In = 0, Out, Flush = 4, Discard = 11, WriteZeroes = 13 } type;
    uint64_t sector;
    enum { OK, IOERR, UNSUPP } status = OK;
    uint32_t process(uint64_t buf, uint32_t len, uint16_t flags)
    {
      uint32_t wlen = 0;
      //bool is_write = VIOQFlags::WRITE.Get(flags);
      bool is_write = flags & 2;
      std::cerr << std::hex << buf << ',' << len << ',' << (is_write ? 'w' : 'r') << std::endl;
      struct Bad {};
      switch (state)
        {
        case Head:
          if (is_write or len != 16) throw Bad();
          type = Type(vioa.read(buf + 0, 4));
          sector = vioa.read(buf + 8, 8);
          state = Body;
          break;
          
        case Body:
          switch (type)
            {
            default:
              status = UNSUPP;
              break;
            case In:
              if (not is_write or len >= 0x100000) throw Bad();
              disk.storage.seekg(512*sector);
              for (VIOAccess::Iterator itr(buf, len, true); itr.next(vioa);)
                disk.storage.read((char*)itr.slice.bytes, itr.slice.size);
              wlen += len;
              break;
            case Out: throw Bad();
            case Flush: throw Bad();
            case Discard: throw Bad();
            case WriteZeroes: throw Bad();
            }
          state = Status;
          break;

        case Status:
          if (not is_write or len != 1) throw Bad();
          vioa.write(buf, 1, status);
          wlen += 1;
          state = Head;
          break;
        }
      return wlen;
    }
  } req(vioa, *this);

  bool used = false;
  for (;;)
    {
      uint64_t desc_addr = rq.desc_addr(rq.head);
      uint16_t flags = vioa.read(desc_addr + 14, 2);
      if (not rq.head.available(flags))
        break;
      
      VIOQueue::DescIterator tail(rq.head);
      uint32_t wlen = 0;

      for (;;)
        {
          uint64_t buf_addr =   vioa.read(desc_addr + 0, 8);
          uint32_t buf_len =    vioa.read(desc_addr + 8, 4);
          //if (VIOQFlags::INDIRECT.Get(flags))
          if (flags & 4)
            {
              for (uint64_t desc_addr = buf_addr, buf_end = buf_addr + buf_len; desc_addr < buf_end; desc_addr += 16)
                {
                  uint64_t buf_addr =  vioa.read(desc_addr +  0, 8);
                  uint32_t buf_len =   vioa.read(desc_addr +  8, 4);
                  uint16_t buf_flags = vioa.read(desc_addr + 14, 2);
              
                  wlen += req.process(buf_addr, buf_len, buf_flags);
                }
            }
          else
            {
              wlen += req.process(buf_addr, buf_len, flags);
            }
      
          rq.head.next(rq);
          // if (VIOQFlags::NEXT.Get(flags));
          if (flags & 1)
            {
              desc_addr = rq.desc_addr(rq.head);
              flags =  vioa.read(desc_addr + 14, 2);
            }
          else
            break;
        }

      uint16_t id = vioa.read(desc_addr + 12, 2);
      std::cerr << "ID:" << id << std::endl;

      uint64_t tail_addr = rq.desc_addr(tail);
      vioa.write(tail_addr +  8, 4, wlen);
      vioa.write(tail_addr + 12, 2, id);
      vioa.write(tail_addr + 14, 2, tail.used(wlen > 0));
      used = true;
    }

  if (used)
    vioa.notify();
  return true;
}

void
VIODisk::open(char const* filename)
{
  storage.open(filename);
  if (not storage or not storage.is_open()) { struct Bad {}; throw Bad (); }
  Capacity = storage.seekg( 0, std::ios::end ).tellg();
}

uint64_t
VIOAccess::read(uint64_t addr, unsigned size) const
{
  uint64_t res = 0, wbyte = 0;

  for (Iterator itr(addr, size, false); itr.next(*this);)
    {
      for (uint64_t rbyte = 0; rbyte < itr.slice.size; ++rbyte, ++wbyte)
        res |= uint64_t(itr.slice.bytes[rbyte]) << 8*wbyte;
    }
  
  return res;
}

void
VIOAccess::write(uint64_t addr, unsigned size, uint64_t value) const
{
  uint64_t rbyte = 0;
  
  for (Iterator itr(addr, size, true); itr.next(*this);)
    {
      for (uint64_t wbyte = 0; wbyte < itr.slice.size; ++rbyte, ++wbyte)
        itr.slice.bytes[wbyte] = value >> 8*rbyte;
    }
}

bool
VIOAccess::Iterator::next(VIOAccess const& vioa)
{
  if (uint64_t nleft = left - slice.size)
    { left = nleft; ptr += slice.size; }
  else
    return false;
  slice = vioa.access(ptr, left, write);
  return true;
}
