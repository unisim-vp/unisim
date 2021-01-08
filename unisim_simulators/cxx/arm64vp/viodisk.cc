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
  : Status(0), Features(), DeviceFeaturesSel(), DriverFeaturesSel(), QueueNum(), QueueReady(0)
  , ConfigGeneration(0), QueueDescArea(), QueueDriverArea(), QueueDeviceArea()
  , Capacity(837212), WriteBack(1)
{}

void
VIODisk::reset()
{
  Status = 0;
  DeviceFeaturesSel = 0;
  DriverFeaturesSel = 0;
  QueueReady = 0;
  ConfigGeneration = 0;
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
  uint64_t addr =   vioa.read(QueueDescArea +  0, 8);
  uint32_t len =    vioa.read(QueueDescArea +  8, 4);
  uint16_t id =     vioa.read(QueueDescArea + 12, 2);
  uint16_t flags =  vioa.read(QueueDescArea + 14, 2);

  std::cerr << "pvirtq_desc";
  std::cerr << "{ addr=0x" << std::hex << addr;
  std::cerr << ", len=" << std::dec << len;
  std::cerr << ", id=" << std::dec << id;
  std::cerr << ", flags=0x" << std::hex << flags;
  std::cerr << ", (DescArea): " << std::hex << QueueDescArea;
  std::cerr << ", (DriverArea): " << std::hex << QueueDriverArea;
  std::cerr << ", (DeviceArea): " << std::hex << QueueDeviceArea;
  std::cerr << "}\n";
    
  return false;
}
