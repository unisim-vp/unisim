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

int
main(int argc, char *argv[])
{
  // Loading image
  char const* img_filename = "Image";
  std::cerr << "*** Loading linux image: " << img_filename << " ***" << std::endl;

  struct MMapped : AArch64::Page::Free
  {
    MMapped(char const* filename)
      : fd(open(filename,O_RDONLY))
    {
      if (fd < 0)
        {
            std::cerr << "With '" << filename << "': " << std::flush;
            perror("open error");
        }
    }
    ~MMapped() { close(fd); }
    void free (AArch64::Page& page) const override
    {
      munmap((void*)page.get_data(), page.size());
      munmap((void*)page.get_udat(), page.size());
    }
    
    int fd;
  } linux_image( img_filename );

  if (linux_image.fd < 0)
    return 1;
    
  AArch64 arch;
  
  // SETUP the VirtIO Hypervisor Emulator
  // LinuxOS linux32( std::cerr, &cpu, &cpu, &cpu );
  // cpu.SetLinuxOS( &linux32 );
  // linux32.Setup( simargs, envs );
  
  //  arch.m_disasm = false;
  
  {
    struct stat f_stat;
    if (fstat(linux_image.fd,&f_stat) < 0)
      {
        std::cerr << "With '" << img_filename << "': " << std::flush;
        perror("fstat error");
        return 1;
      }
    uint64_t base = 0x80000, size = f_stat.st_size;
    uint8_t* data = (uint8_t*)mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, linux_image.fd, 0);
    if (data == (void*)-1) { perror("mmap error"); return 1; }
    uint8_t* udat = (uint8_t*)mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (udat == (void*)-1) { perror("mmap error"); return 1; }
    arch.mem_map(AArch64::Page(0,base,base+size-1,data,udat,&linux_image));
    arch.BranchTo(AArch64::U64(base),arch.B_JMP);
  }
  
  std::cerr << "\n*** Run ***" << std::endl;

  for (;;)
    {
      arch.step_instruction();
    }
  
  std::cerr << "WTF" << std::endl;
  
  return 0;
}
