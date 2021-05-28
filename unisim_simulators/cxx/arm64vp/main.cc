/*
 *  Copyright (c) 2020,
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

#include "architecture.hh"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

struct MMapped : AArch64::Page::Free
{
  MMapped(char const* _filename)
    : filename(_filename), fd(open(_filename,O_RDONLY))
  {
    if (fd < 0)
      {
        std::cerr << "With '" << _filename << "': " << std::flush;
        perror("open error");
      }
  }
  void free (AArch64::Page& page) const override
  {
    munmap((void*)page.data_beg(), page.size());
    munmap((void*)page.udat_beg(), page.size());
    delete this;
  }
  bool get(struct stat& res)
  {
    if (fstat(fd,&res) >= 0)
      return true;
    std::cerr << "With '" << filename << "': " << std::flush;
    perror("fstat error");
    return false;
  }
  bool get(AArch64::Page& page)
  {
    uintptr_t size = page.size();
    uint8_t* data = (uint8_t*)mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (data == (void*)-1) { perror("mmap error"); return false; }
    uint8_t* udat = (uint8_t*)mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (udat == (void*)-1) { perror("mmap error"); return false; }
    page.set_data(data);
    page.set_udat(udat);
    return true;
  }

  void dump_load( std::ostream& sink, char const* reason, AArch64::Page const& page )
  {
    sink << "*** Loading ";
    page.dump_range( sink );
    sink << " with '" << filename << "' ***" << std::endl;
  }

  char const* filename; // For logging purpose
  int fd;

private:
  ~MMapped() { close(fd); }
};

bool
load_linux(AArch64& arch, char const* img_filename, char const* dt_filename)
{
  MMapped* linux_image  = new MMapped( img_filename );

  if (linux_image->fd < 0) return false;

  MMapped* device_tree = new MMapped( dt_filename );

  if (device_tree->fd < 0) return false;

  {
    struct stat f_stat;
    if (not linux_image->get(f_stat))
      return 1;
    uint64_t size = f_stat.st_size;
    // Memory map will be rounded up to page boundary. We round up to
    // AArch64 ZVA boundary which should always be less than host page
    // boundary.
    size = (size + 63) & uint64_t(-64);

    struct
    {
      uint8_t buf[8];
      uint64_t read(int fd)
      {
        uint64_t res = 0;
        if (::read(fd, &buf[0], sizeof (buf)) != sizeof (buf)) throw *this;
        for (unsigned idx = sizeof (buf); idx-- > 0;) res = res << 8 | buf[idx];
        return res;
      }
    } le64;
    lseek(linux_image->fd,8,SEEK_SET);
    uint64_t base = le64.read(linux_image->fd), kernel_size = le64.read(linux_image->fd);
    lseek(linux_image->fd,0,SEEK_SET);
    if (base % 64) { std::cerr << "Bad size: " << std::hex << base << std::endl; return 1; }
    if (size % 64) { std::cerr << "Bad size: " << std::hex << size << std::endl; return 1; }
    AArch64::Page page(base,base+size-1,0,0,linux_image);
    linux_image->dump_load( std::cerr, "linux image", page );
    if (not linux_image->get(page))
      return 1;
    arch.mem_map(std::move(page));
    arch.BranchTo(AArch64::U64(base),arch.B_JMP);
    // If kernel_size is greater than mapped file size, we need to reserve extra space with unitialized data
    uint64_t xbase = base + size, xlast = base + kernel_size - 1;
    if (xbase <= xlast)
      {
        AArch64::Page extra(xbase, xlast);
        std::fill(extra.udat_beg(), extra.udat_end(), -1);
        arch.mem_map(std::move(extra));
      }
    arch.MemDump64(base);
  }

  {
    struct stat f_stat;
    if (not device_tree->get(f_stat))
      return 1;
    uint64_t base = arch.pages.begin()->last + 1, size = f_stat.st_size;
    size = (size + 63) & uint64_t(-64);
    if (base % 64) { std::cerr << "Bad base: " << std::hex << base << std::endl; return 1; }
    // The device tree blob (dtb) must be placed on an 8-byte boundary
    AArch64::Page page(base,base+size-1,0,0,device_tree);
    device_tree->dump_load( std::cerr, "device tree", page );
    if (not device_tree->get(page))
      return 1;
    arch.mem_map(std::move(page));
    /* Primary CPU general-purpose register settings
     * x0 = physical address of device tree blob (dtb) in system RAM.
     * x1 = 0 (reserved for future use)
     * x2 = 0 (reserved for future use)
     * x3 = 0 (reserved for future use)
     */
    arch.SetGSR(0, AArch64::U64(base));
    for (unsigned idx = 1; idx < 4; ++idx)
      arch.SetGSR(idx, 0/*AArch64::U64(0xdeadbeefbadfeed0 + idx,-1)*/);
  }

  return true;
}
// Docs & Notes:
//  [AArch64 boot info] https://www.kernel.org/doc/Documentation/arm64/booting.txt
//  [Yocto dtb generation] runqemu qemuparams="-machine dumpdtb=device_tree_from_qemu.dtb"

int
main(int argc, char *argv[])
{
  char const* disk_filename = "rootfs.ext4";

  // Loading image

  AArch64 arch;

  arch.map_gic(0x48000000);
  arch.map_uart(0x49000000);
  arch.map_rtc(0x49010000);

  for (unsigned int idx = 0; idx < 32; ++idx)
    {
      uint64_t base_addr = 0x4a000000 + 0x200*idx;
      unsigned irq = 32 + 16 + idx;
      switch (idx)
        {
        default: arch.map_virtio_placeholder(idx, base_addr); break;
     // case 30: arch.map_virtio_console(base_addr, irq); break;
        case 31: arch.map_virtio_disk(disk_filename, base_addr, irq); break;
        }
    }

  // if (not load_linux(arch, "Image", "device_tree.dtb"))
  //   return 1;
  arch.load_snapshot("uvp.shot");
  
  // // ffffffc010eb5198 0x20000
  // AArch64::Page const& logbuf = arch.modify_page(0xeb5198);
  // uint8_t const* logbytes = logbuf.data_abs(0xeb5198);
  // std::cerr << "logbuf: " << (void*)logbytes << std::endl;

  std::cerr << "\n*** Run ***" << std::endl;

  try
    {
      arch.run();
      std::cerr << "Executed " << std::dec << arch.insn_counter << " instructions: " << std::endl;
    }
  catch (...)
    {
      std::cerr << "Executed " << std::dec << arch.insn_counter << " instructions: " << std::endl;
      std::ofstream tail("tail");
      for (unsigned idx = 1; idx <= arch.histsize; ++idx)
        if (arch.last_insn(idx).op)
          tail << arch.last_insn(idx) << std::endl;
      throw;
    }


  return 0;
}

void AArch64::breakdance() {}

