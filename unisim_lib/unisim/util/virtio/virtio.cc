/*
 *  Copyright (c) 2007-2022,
 *  Commissariat a l'Energie Atomique (CEA),
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

#include <unisim/util/virtio/virtio.hh>
#include <iostream>

namespace unisim {
namespace util {
namespace virtio {
  bool
  PackedQueue::Read(Access const& vioa, QProc& req)
  {
    uint32_t pqes = vioa.read(driver_area, 4);

    bool notification = false;
    for (;;)
      {
        uint64_t cur_desc_addr = desc_addr(head);
        uint16_t flags = vioa.read(cur_desc_addr + 14, 2);
        if (not head.available(flags))
          break;
        bool last = not (flags & 1);

        DescIterator tail(head);
        uint32_t wlen = 0;

        for (;;)
          {
            uint64_t buf_addr =   vioa.read(cur_desc_addr + 0, 8);
            uint32_t buf_len =    vioa.read(cur_desc_addr + 8, 4);
            //if (QFlags::INDIRECT.Get(flags))
            if (flags & 4)
              {
                uint64_t cur_desc_addr = buf_addr, buf_end = buf_addr + buf_len;
                for (bool cont = true; cont; )
                  {
                    uint64_t buf_addr =  vioa.read(cur_desc_addr +  0, 8);
                    uint32_t buf_len =   vioa.read(cur_desc_addr +  8, 4);
                    uint16_t buf_flags = vioa.read(cur_desc_addr + 14, 2);

                    cur_desc_addr += 16;
                    cont = cur_desc_addr < buf_end;

                    wlen += req.process(buf_addr, buf_len, buf_flags, last and not cont);
                  }
              }
            else
              {
                wlen += req.process(buf_addr, buf_len, flags, last);
              }

            head.next(*this);
            // if (QFlags::NEXT.Get(flags));
            if (last)
              break;

            cur_desc_addr = desc_addr(head);
            flags =  vioa.read(cur_desc_addr + 14, 2);
          }

        uint16_t id = vioa.read(cur_desc_addr + 12, 2);
        // std::cerr << "ID:" << id << std::endl;

        uint64_t tail_addr = desc_addr(tail);
        vioa.write(tail_addr +  8, 4, wlen);
        vioa.write(tail_addr + 12, 2, id);
        vioa.write(tail_addr + 14, 2, tail.used(wlen > 0));

        if (not notification)
          {
            switch (pqes >> 16 & 3)
              {
              default: /*    ??? */ std::cerr << "virtio error: bad flag value.\n"; notification = true; break;
              case 0: /*  ENABLE */ notification = true; break;
              case 1: /* DISABLE */ break;
              case 2: /*    DESC */ notification = tail.idx() == (pqes & 0xffff); break;
              }
          }
      }

    return notification;
  }

  bool
  PackedQueue::Start(Access const& vioa)
  {
    // Setup device event suppresion mechanisms
    // Packed VirtQ : (reserved[14] = 0: desc_event_flags[2] = 0 /*RING_EVENT_FLAGS_ENABLE*/: desc_event_wrap[1] = 0, desc_event_off[15] = 0)
    vioa.write(device_area, 4, 0);
    return true;
  }

  bool SplitQueue::Read(Access const& vioa, QProc& proc)
  {
    throw 0;
    return false;
  }

  bool
  SplitQueue::Start(Access const& vioa)
  {
    // Setup device event suppresion mechanisms
    // Split  VirtQ : (used_event[16] = 0: reserved[15] : flags[1] = 0/*not VIRTQ_USED_F_NO_NOTIFY*/)
    vioa.write(device_area, 4, 0);
    return true;
  }

  Device::Device()
    : Status(0), DeviceFeaturesSel(), DriverFeaturesSel(), ConfigGeneration(0), qmgr()
  {}

  // void
  // Device::reset()
  // {
  //   Status = 0;
  //   Features = 0;
  //   DeviceFeaturesSel = 0;
  //   DriverFeaturesSel = 0;
  //   ConfigGeneration = 0;
  //   rq = Queue();
  //   WriteBack = 1;
  // }

  bool
  Device::CheckStatus()
  {
    uint32_t status = Status;

    static struct { uint32_t mask; char const* name; }
      const fields[] =
      {
        {1,"acknowledged"},
        {2,"driver found"}, {128,"failed"}, {8,"features ok"}, {4,"driver ok"}, {64,"driver needs reset"}
      };

    uint32_t handled = 0;
    std::cerr << "virtio::Device status: {";
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

    if (status & 128)
      {
        std::cerr << "unhandled <failed> status bit.\n";
        return false;
      }

    if (status & ~handled)
      {
        std::cerr << "| unhandled status bits: " << std::hex << status << ", Status=" << Status << "\n";
        return false;
      }

    return true;
  }
} /* end of namespace virtio */
} /* end of namespace util */
} /* end of namespace unisim */

