/*
 *  Copyright (c) 2019-2022,
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
#include <set>
#include <string>
#include <inttypes.h>

struct AArch64;

struct VIODisk : public unisim::util::virtio::BlockDevice
{
  VIODisk();
  ~VIODisk();

  struct Access : public unisim::util::virtio::Access
  {
    virtual AArch64& GetCore() const = 0;
  };

  struct SyncQMgr
  {
    virtual ~SyncQMgr() {}
    virtual void sync(SnapShot&, unisim::util::virtio::Queue*) const = 0;
    virtual bool is_packed() const = 0;
  };

  struct Delta
  {
    Delta(uint64_t _index) : index(_index >> shift << shift), udat() {}
    Delta(Delta&& pg) : index(pg.index), udat(pg.udat) { pg.udat = 0; }
    Delta(const Delta&) = delete;
    ~Delta() { delete [] udat; }

    bool operator < (Delta const& rhs) const { return index < rhs.index; }

    enum { shift = 12 };
    static constexpr uint64_t fullsize() { return 1<<shift; }

    static uint64_t size_from( uint64_t addr ) { return (~addr % fullsize()) + 1; }

    void uwrite(uint64_t pos, uint64_t size, uint8_t const* src) const;
    
    void uread(uint64_t pos, uint64_t size, uint8_t* dst) const;

    uint64_t    index;
    mutable uint8_t* udat;
  };

  typedef std::set<Delta> Deltas;

  void open(char const* filename);
  void sync(SnapShot& snapshot);

  static uint32_t Vendor() { return 0x70767375; /*usvp*/ }
  uint32_t  ClaimedFeatures();
  bool      UsedFeatures(uint32_t);
  void      SetupQueues(bool is_packed);

  void read(unisim::util::virtio::Access const& sys, uint64_t addr, uint64_t size) override;
  void write(unisim::util::virtio::Access const& sys, uint64_t addr, uint64_t size) override;
  void seek(uint64_t pos) override { diskpos = pos; }

  // Configuration
  SyncQMgr*   sqmgr;
  uint8_t     WriteBack;

  // Storage state
  std::string diskfilename;
  uint64_t    Capacity, diskpos, disksize;
  uint8_t*    storage;
  Deltas      deltas;
};

#endif /* __ARM64VP_VIODISK_HH__ */

