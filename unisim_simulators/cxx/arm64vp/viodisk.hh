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

#ifndef __ARM64VP_VIODISK_HH__
#define __ARM64VP_VIODISK_HH__

#include "snapshot.hh"
#include <unisim/util/virtio/virtio.hh>
#include <fstream>
#include <inttypes.h>

struct VIODisk : public unisim::util::virtio::Device
{
  VIODisk();

  enum { BLKSIZE = 512 };

  struct SyncQMgr
  {
    virtual ~SyncQMgr() {}
    virtual void sync(SnapShot&, unisim::util::virtio::Queue*) const = 0;
    virtual bool is_packed() const = 0;
  };

  virtual void seek(uint64_t pos) = 0;
  virtual uint64_t tell() = 0;
  virtual void read(unisim::util::virtio::Access const&, uint64_t addr, uint64_t size) = 0;
  virtual void write(unisim::util::virtio::Access const&, uint64_t addr, uint64_t size) = 0;

  virtual void sync(SnapShot& snapshot);

  static uint32_t Vendor() { return 0x70767375; /*usvp*/ }
  uint32_t  ClaimedFeatures();
  bool      UsedFeatures(uint32_t);
  bool      ReadQueue(unisim::util::virtio::Access const& vioa);
  void      SetupQueues(bool is_packed);

  SyncQMgr *sqmgr;

  // Block Device Config
  uint64_t  Capacity;
  uint8_t   WriteBack;
};

struct VIOConsole
{
  // Generic Config
  static uint32_t Vendor() { return 0x70767375; }
  static uint32_t QueueNumMax() { return 1024; }
  uint32_t ClaimedFeatures();
  bool UsedFeatures(uint32_t);
  bool CheckStatus();
  bool InterruptAck(uint32_t mask) { InterruptStatus &= ~mask; return true; }
  bool ReadQueue(unisim::util::virtio::Access const& vioa);
  bool SetupQueue(unisim::util::virtio::Access const& vioa);

  // Generic Config
  // uint32_t Status, Features, DeviceFeaturesSel, DriverFeaturesSel, ConfigGeneration, InterruptStatus;
  uint32_t DeviceFeaturesSel, DriverFeaturesSel, InterruptStatus;
};

#endif /* __ARM64VP_VIODISK_HH__ */

