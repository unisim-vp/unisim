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
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#include <unisim/util/virtio/virtio.hh>
#include <iostream>

namespace unisim {
namespace util {
namespace virtio {
  bool
  PackedQueue::Read(QMgr const&, Access const& vioa, QProc& proc)
  {
    /* Pre-compute notification conditions */
    uint32_t event_notification = vioa.read(driver_area, 4);
    bool notification = false;

    for (;;)
      {
        /* Compute first descriptor address and availability */
        uint64_t cur_desc_addr = desc_addr(head);
        uint16_t flags = vioa.read(cur_desc_addr + 14, 2);
        if (not head.available(flags))
          break;

        DescIterator first(head);
        uint32_t wlen = 0;

        for (;;)
          {
            uint64_t buf_addr =   vioa.read(cur_desc_addr + 0, 8);
            uint32_t buf_len =    vioa.read(cur_desc_addr + 8, 4);
            bool last = not (flags & 1);

            if (flags & 4)
              {
                /* Indirect descriptor table */
                uint64_t cur_desc_addr = buf_addr, table_end = buf_addr + buf_len;
                for (bool cont = true; cont; )
                  {
                    uint64_t buf_addr =  vioa.read(cur_desc_addr +  0, 8);
                    uint32_t buf_len =   vioa.read(cur_desc_addr +  8, 4);
                    uint16_t flags = vioa.read(cur_desc_addr + 14, 2);

                    cur_desc_addr += 16;
                    cont = cur_desc_addr < table_end;

                    wlen += proc.process(buf_addr, buf_len, flags & 2, last and not cont);
                  }
              }
            else
              {
                wlen += proc.process(buf_addr, buf_len, flags & 2, last);
              }

            head.next(*this);

            if (last)
              break;

            cur_desc_addr = desc_addr(head);
            flags =  vioa.read(cur_desc_addr + 14, 2);
          }

        /* Dispose chain */
        uint16_t id = vioa.read(cur_desc_addr + 12, 2);
        // std::cerr << "ID:" << id << std::endl;

        uint64_t first_addr = desc_addr(first);
        vioa.write(first_addr +  8, 4, wlen);
        vioa.write(first_addr + 12, 2, id);
        vioa.write(first_addr + 14, 2, first.used(wlen > 0));

        if (not notification)
          {
            switch (event_notification >> 16 & 3)
              {
              default: /*    ??? */ std::cerr << "virtio error: bad notification flag value.\n"; notification = true; break;
              case 0: /*  ENABLE */ notification = true; break;
              case 1: /* DISABLE */ break;
              case 2: /*    DESC */ notification = first.idx() == (event_notification & 0xffff); break;
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

  bool
  SplitQueue::Read(QMgr const& qmgr, Access const& vioa, QProc& proc)
  {
    /* Pre-compute notification conditions */
    uint16_t event_notification = vioa.read(driver_area + 4 + 2*size, 2);
    bool notification = false;

    for (;;)
      {
        /* Compute first descriptor address and availability */
        if (avail_idx == vioa.read(driver_area + 2, 2))
          break;
        uint16_t first = vioa.read(driver_area + 4 + 2*(avail_idx%size), 2);
        avail_idx += 1;
        uint64_t cur_desc_addr = desc_area + 16*first;
        uint16_t flags = vioa.read(cur_desc_addr + 12, 2);
        
        uint32_t wlen = 0;

        for (;;)
          {
            uint64_t buf_addr =   vioa.read(cur_desc_addr + 0, 8);
            uint32_t buf_len =    vioa.read(cur_desc_addr + 8, 4);
            bool last = not (flags & 1);

            if (flags & 4)
              {
                /* Indirect descriptor table */
                uint64_t cur_desc_addr = buf_addr, table_end = buf_addr + buf_len;
                for (bool cont = true; cont; )
                  {
                    uint64_t buf_addr =  vioa.read(cur_desc_addr +  0, 8);
                    uint32_t buf_len =   vioa.read(cur_desc_addr +  8, 4);
                    uint16_t flags = vioa.read(cur_desc_addr + 12, 2);

                    cur_desc_addr += 16;
                    cont = cur_desc_addr < table_end;

                    wlen += proc.process(buf_addr, buf_len, flags & 2, last and not cont);
                  }
              }
            else
              {
                wlen += proc.process(buf_addr, buf_len, flags & 2, last);
              }

            if (last)
              break;

            cur_desc_addr = desc_area + 16*vioa.read(cur_desc_addr + 14, 2);
            flags =  vioa.read(cur_desc_addr + 12, 2);
          }

        /* Dispose chain */
        uint64_t used_entry = device_area + 4 + 8*(used_idx%size);
        vioa.write(used_entry + 0, 4, first);
        vioa.write(used_entry + 4, 4, wlen);
        notification = notification or (event_notification == used_idx);
        used_idx += 1;
      }

    vioa.write(device_area + 2, 2, used_idx); // Update upper bound of the ring
    if (qmgr.event_idx) // Update avail_event notification if needed
      vioa.write(device_area + 4 + 8*size, 2, avail_idx);
    
    return notification;
  }

  bool
  SplitQueue::Start(Access const& vioa)
  {
    // Setup device event suppresion mechanisms
    vioa.write(device_area, 2, /*not VIRTQ_USED_F_NO_NOTIFY*/ 0);
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
  
  void
  GetFeatureNameBase::Print(std::ostream& sink, char const* name) const
  {
    typedef unisim::util::virtio::CommonFeatures CommonFeatures;
    if (name) sink << name;
    else if ((name = unisim::util::virtio::NameGetter<CommonFeatures::Code,(int)CommonFeatures::end - 1>::feature(bit))) sink << name;
    else sink << "unknown_feature(" << std::dec << bit << ")";
  }

  bool
  BlockDevice::ReadQueue(unisim::util::virtio::Access const& sys)
  {
    struct Request : public QProc
    {
      Request(unisim::util::virtio::Access const& _sys, BlockDevice& _disk) : sys(_sys), disk(_disk), state(Head), type(), status() {}
      unisim::util::virtio::Access const& sys; BlockDevice& disk;
      enum { Head, Body, Status } state;
      enum Type { In = 0, Out, Flush = 4, GetID = 8, Discard = 11, WriteZeroes = 13 } type;
      enum { OK, IOERR, UNSUPP } status;

      virtual uint32_t process(uint64_t buf, uint32_t len, bool is_write, bool last) override
      {
        uint32_t wlen = 0;
        //std::cerr << std::hex << buf << ',' << len << ',' << (is_write ? 'w' : 'r') << std::endl;


        if (state == Body and last) state = Status;

        switch (state)
          {
          case Head:
            if (is_write or len != 16) { struct UnexpectedHeader {}; throw UnexpectedHeader (); }
            //std::cerr << "VirtIO Block Header: {type=" << sys.read(buf + 0, 4) << ", sector=" << sys.read(buf + 8, 8) << "}\n";
            switch (type = Type(sys.read(buf + 0, 4)))
              {
              default: { struct UnexpectedCommandType {}; throw UnexpectedCommandType(); }

              case GetID:
                /* TODO: support this command ? */
                break;
              case Flush:
                /* No data, all work should be done here. */
                disk.flush(sys);
                break;
              case Discard:
              case WriteZeroes:
                break;
              case In: case Out:
                disk.seek(BlockDevice::BLKSIZE*sys.read(buf + 8, 8));
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
                
                struct UnexpectedBody {};
              case In:
                if (not is_write or (len % BlockDevice::BLKSIZE) != 0) throw UnexpectedBody();
                disk.read(sys, buf, len);
                wlen += len;
                break;
              case Out:
                if (is_write or (len % BlockDevice::BLKSIZE) != 0) throw UnexpectedBody();
                disk.write(sys, buf, len);
                break;
              case Flush:
               throw UnexpectedBody();
               break;
              case Discard:
              case WriteZeroes:
                if (len)
                {
                  if (len % 16) { struct UnexpectedBody {}; throw UnexpectedBody (); }
                  for(uint64_t end = buf + len; buf < end; buf += 16)
                  {
                    if (type == WriteZeroes)
                      disk.write_zeroes(sys, BlockDevice::BLKSIZE*sys.read(buf, 8), BlockDevice::BLKSIZE*sys.read(buf + 8, 4), sys.read(buf + 12, 4));
                    else
                      disk.discard(sys, BlockDevice::BLKSIZE*sys.read(buf, 8), BlockDevice::BLKSIZE*sys.read(buf + 8, 4));
                  }
                }
                break;
              }
            break;

          case Status:
            if (not is_write or len != 1) { struct UnexpectedFooter {}; throw UnexpectedFooter (); }
            sys.write(buf, 1, status);
            wlen += 1;
            state = Head;
            break;
          }
        return wlen;
      }
    } req(sys, *this);

    QueueRead(sys, req);

    if (req.state != req.Head)
      { struct UnexpectedEndOfRequest {}; throw UnexpectedEndOfRequest(); }
    
    return true;
  }

} /* end of namespace virtio */
} /* end of namespace util */
} /* end of namespace unisim */

