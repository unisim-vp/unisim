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

// Features:
//  BLOCK specific
//   BARRIER = 0, // Device supports request barriers (legacy).
//   SIZE_MAX  = 1, // Maximum size of any single segment is in size_max.
//   SEG_MAX = 2, // Maximum number of segments in a request is in seg_max.
//   GEOMETRY = 4, // Disk-style geometry specified in geometry.
//   RO = 5, // Device is read-only.
//   BLK_SIZE = 6, // Block size of disk is in blk_size.
//   SCSI = 7, // Device supports scsi packet commands (legacy).
//   FLUSH = 9, // Cache flush command support (a.k.a WCE in legacy).
//   TOPOLOGY = 10, // Device exports information on optimal I/O alignment.
//   CONFIG_WCE = 11, // Device can toggle its cache between writeback and writethrough modes.
//   DISCARD = 13, // Support discard command with max_discard_sectors and max_discard_seg limits.
//   WRITE_ZEROES = 14, // Support write zeroes command with max_write_zeroes_sectors and max_write_zeroes_seg limits.
//  Common
//   VIRTIO_F_RING_INDIRECT_DESC (28) // Device supports with the VIRTQ_DESC_F_INDIRECT flag set
//   VIRTIO_F_RING_EVENT_IDX (29) // This feature enables the used_event and the avail_event fields
//   VIRTIO_F_VERSION_1(32) // This indicates compliance with this specification
//   VIRTIO_F_ACCESS_PLATFORM(33) // Device supports IOMMU
//   VIRTIO_F_RING_PACKED(34) // Device supports the packed virtqueue layout
//   VIRTIO_F_IN_ORDER(35) // Device supports in-order use of buffers
//   VIRTIO_F_ORDER_PLATFORM(36) // Device needs platform ordering
//   VIRTIO_F_SR_IOV(37) // Device supports Single Root I/O Virtualization
//   VIRTIO_F_NOTIFICATION_DATA(38) // Device support extended notifications
// 
uint32_t
VIODisk::ClaimedFeatures()
{
  switch (DeviceFeaturesSel)
    {
    case 0: return 0b0011 << 28 | 0b110111001000100;
    case 1: return 0b1001111;
    }
  
  struct Bad {};
  throw Bad ();
  
  return 0;
}

bool
VIODisk::UsedFeatures(uint32_t feat)
{
  std::cerr << "UsedFeatures: ";
  char const* sep = "";
  for (unsigned bit = 32; bit-->0;)
    {
      if (not (feat >> bit & 1))
        continue;

      std::cerr << sep; sep = ", ";
      unsigned fnum = bit+32*DriverFeaturesSel;
      switch (fnum)
        {
          //        case 0:  std::cerr << "BARRIER"; break; // Device supports barrier commands (legacy).
        case 1:  std::cerr << "SIZE_MAX"; break; // Maximum size of any single segment is in size_max.
        case 2:  std::cerr << "SEG_MAX"; break; // Maximum number of segments in a request is in seg_max.
        case 4:  std::cerr << "GEOMETRY"; break; // Disk-style geometry specified in geometry.
        case 5:  std::cerr << "RO"; break; // Device is read-only.
          //        case 7:  std::cerr << "SCSI"; break; // Device supports scsi packet commands (legacy).
        case 6:  std::cerr << "BLK_SIZE"; break; // Block size of disk is in blk_size.
        case 9:  std::cerr << "FLUSH"; break; // Cache flush command support (a.k.a WCE in legacy).
        case 10: std::cerr << "TOPOLOGY"; break; // Device exports information on optimal I/O alignment.
        case 11: std::cerr << "CONFIG_WCE"; break; // Device can toggle its cache between writeback and writethrough modes.
        case 13: std::cerr << "DISCARD"; break; // Support discard command with max_discard_sectors and max_discard_seg limits.
        case 14: std::cerr << "WRITE_ZEROES"; break; // Support write zeroes command (max_write_zeroes_sectors, max_write_zeroes_seg)
        case 28: std::cerr << "RING_INDIRECT_DESC "; break; // Device supports with the VIRTQ_DESC_F_INDIRECT flag set
        case 29: std::cerr << "RING_EVENT_IDX "; break; // This feature enables the used_event and the avail_event fields
        case 32: std::cerr << "VERSION_1"; break; // This indicates compliance with this specification
        case 33: std::cerr << "ACCESS_PLATFORM"; break; // Device supports IOMMU
        case 34: std::cerr << "RING_PACKED"; break; // Device supports the packed virtqueue layout
        case 35: std::cerr << "IN_ORDER"; break; // Device supports in-order use of buffers
        case 36: std::cerr << "ORDER_PLATFORM"; break; // Device needs platform ordering
        case 37: std::cerr << "SR_IOV"; break; // Device supports Single Root I/O Virtualization
        case 38: std::cerr << "NOTIFICATION_DATA"; break; // Device support extended notifications
        default: std::cerr << "Unknown feature (" << fnum << ")";
        }
    }
  std::cerr << "\n";
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

