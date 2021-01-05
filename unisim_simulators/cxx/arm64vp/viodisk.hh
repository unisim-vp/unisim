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

struct VIODisk
{
  VIODisk();
  void reset();
  
  // Generic Config
  static uint32_t Vendor() { return 0x70767375; }
  static uint32_t QueueNumMax() { return 1024; }
  uint32_t ClaimedFeatures();
  bool UsedFeatures(uint32_t);
  bool CheckFeatures();
  bool CheckStatus();
  
  // Block Device Config
  static uint32_t SegMax() { return 254; }
  static uint32_t BlkSize() { return 512; }
  static uint32_t DiscardSectorAlignment() { return 1; }
  static uint32_t MaxDiscardSectors() { return 0x3fffff; }
  static uint32_t MaxDiscardSeg() { return 1; }
  static uint32_t MaxWriteZeroesSectors() { return 0x3fffff; }
  
  // Generic Config
  uint32_t Status, Features, DeviceFeaturesSel, DriverFeaturesSel, QueueNum, QueueReady, ConfigGeneration;
  uint64_t QueueDescArea, QueueDriverArea, QueueDeviceArea;
  
  // Block Device Config
  uint64_t Capacity;
  uint8_t WriteBack;
  
};

#endif /* __ARM64VP_VIODISK_HH__ */

