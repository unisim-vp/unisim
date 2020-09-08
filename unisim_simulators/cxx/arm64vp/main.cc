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
  ~MMapped() { close(fd); }
  void free (AArch64::Page& page) const override
  {
    munmap((void*)page.get_data(), page.size());
    munmap((void*)page.get_udat(), page.size());
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
};

int
main(int argc, char *argv[])
{
  char const* img_filename = "Image";
  char const* dt_filename = "device_tree_from_qemu.dtb";
  // Booting an AArch64 system
  //
  // Docs & Notes: 
  //  [arm64 boot info] https://www.kernel.org/doc/Documentation/arm64/booting.txt
  //  [yocto dtb generation] runqemu qemuparams="-machine dumpdtb=device_tree_from_qemu.dtb"
  
  // Loading image

  MMapped linux_image( img_filename );

  if (linux_image.fd < 0) return 1;

  MMapped device_tree( dt_filename );
    
  if (device_tree.fd < 0) return 1;
  
  AArch64 arch;
  
  
  // SETUP the VirtIO Hypervisor Emulator
  // LinuxOS linux32( std::cerr, &cpu, &cpu, &cpu );
  // cpu.SetLinuxOS( &linux32 );
  // linux32.Setup( simargs, envs );
  
  //  arch.m_disasm = false;
  
  {
    struct stat f_stat;
    if (not linux_image.get(f_stat))
      return 1;
    uint64_t base = 0x80000, size = f_stat.st_size;
    size = (size + 63) & uint64_t(-64);
    if (size % 64) { std::cerr << "Bad size: " << std::hex << size << std::endl; return 1; }
    AArch64::Page page(0,base,base+size-1,0,0,&linux_image);
    linux_image.dump_load( std::cerr, "linux image", page );
    if (not linux_image.get(page))
      return 1;
    arch.mem_map(std::move(page));
    arch.BranchTo(AArch64::U64(base),arch.B_JMP);
  }

  {
    struct stat f_stat;
    if (not device_tree.get(f_stat))
      return 1;
    uint64_t base = arch.pages.begin()->last + 1, size = f_stat.st_size;
    size = (size + 63) & uint64_t(-64);
    if (base % 64) { std::cerr << "Bad base: " << std::hex << base << std::endl; return 1; }
    // The device tree blob (dtb) must be placed on an 8-byte boundary
    AArch64::Page page(0,base,base+size-1,0,0,&device_tree);
    device_tree.dump_load( std::cerr, "device tree", page );
    if (not device_tree.get(page))
      return 1;
    arch.mem_map(std::move(page));
    /* Primary CPU general-purpose register settings
     * x0 = physical address of device tree blob (dtb) in system RAM.
     * x1 = 0 (reserved for future use)
     * x2 = 0 (reserved for future use)
     * x3 = 0 (reserved for future use)
     */
    for (unsigned idx = 1; idx < 4; ++idx)
      arch.SetGSR(idx, AArch64::U64(0xdeadbeefbadfeed0 + idx,-1));
    arch.SetGSR(0, AArch64::U64(base));
  }

  
  // ffffffc010eb5198 0x20000
  AArch64::Page const& logbuf = arch.modify_page(0xeb5198);
  uint8_t const* logbytes = logbuf.get_data(0xeb5198);
  std::cerr << "logbuf: " << (void*)logbytes << std::endl;

  std::cerr << "\n*** Run ***" << std::endl;

  for (;;)
    {
      arch.step_instruction();
    }
  
  std::cerr << "WTF" << std::endl;
  
  return 0;
}
