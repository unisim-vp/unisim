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

#include "transfer.hh"
#include "architecture.hh"
#include "debug.hh"
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

Transfer::File::File(char const* path)
  : fd(::open(path,O_RDWR)), refcount(1)
{
  if (fd < 0) { perror(__FUNCTION__); return; }
  struct stat info;
  if (fstat(fd, &info) < 0)
    throw info;
  size = info.st_size;
}

Transfer::File::File(char const* path, uint64_t _size)
  : fd(::open(path,O_CREAT | O_EXCL | O_RDWR, 0666)), refcount(1), size(_size)
{
  if (fd < 0) { perror(__FUNCTION__); return; }
  ftruncate(fd, size);
}

Transfer::File::~File() { ::close(fd); }

//extern uint64_t usvp_asm_get(char const* path, uint64_t offset, uint8_t* ptr, uint64_t size);

uint64_t Transfer::get(AArch64& core)
{
  struct Bad {};

  AArch64::U64 path_addr = core.GetGZR(0);
  uint64_t offset = core.untaint( Bad(), core.GetGZR(1) );
  uint64_t ptr = core.untaint( Bad(), core.GetGZR(2) );
  uint64_t size = core.untaint( Bad(), core.GetGZR(3) );

  std::string path;
  for (AArch64::U64 addr = path_addr;; addr += AArch64::U64(1))
    {
      char chr = core.untaint(Bad(), core.MemRead8(addr));
      if (not chr)
        break;
      path += chr;
    }

  // std::cerr << "Host path:     " << path << std::endl;
  // std::cerr << "Host offset:   " << offset << std::endl;
  // std::cerr << "Guest pointer: " << ptr << std::endl;
  // std::cerr << "Guest size:    " << size << std::endl;
  
  bool opening = ptr == 0 and size == 0;

  auto itr = files.lower_bound(path);
  if (itr == files.end() or itr->first > path)
    {
      if (not opening) { std::cerr << "Missing opening command.\n"; return 0; }
      File file(path.c_str());
      if (file.fd < 0) return 0;
      itr = files.emplace_hint(itr, std::piecewise_construct, std::forward_as_tuple(std::move(path)), std::forward_as_tuple(std::move(file)));
    }
  else if (opening)
    itr->second.refcount += 1;

  uint64_t res = itr->second.size;

  if (not opening)
    {
      for (AArch64::SliceIterator slice(ptr + offset, size - offset, AArch64::mem_acc_type::write); slice.vnext(core);)
        {
          lseek(itr->second.fd, offset, SEEK_SET);
          uint64_t done = ::read(itr->second.fd, static_cast<void*>(slice.data), slice.size);
          if (done != slice.size)
            {
              if (done == uint64_t(-1)) perror(__FUNCTION__);
              else std::cerr << itr->first << ": file read abruptly stopped at " << (offset + done) << " bytes.\n";
              return offset;
            }
          std::fill(slice.udat, slice.udat + slice.size, 0);
          offset += slice.size;
          core.SetGZR(1, AArch64::U64(offset));
        }          

      
      if (--(itr->second.refcount) <= 0)
        files.erase(itr);
    }

  return res;
}

//extern uint64_t usvp_asm_put(char const* path, uint64_t offset, uint8_t const* ptr, uint64_t size);

uint64_t Transfer::put(AArch64& core)
{
  struct Bad {};

  AArch64::U64 path_addr = core.GetGZR(0);
  uint64_t offset = core.untaint( Bad(), core.GetGZR(1) );
  uint64_t ptr = core.untaint( Bad(), core.GetGZR(2) );
  uint64_t size = core.untaint( Bad(), core.GetGZR(3) );

  std::string path;
  for (AArch64::U64 addr = path_addr;; addr += AArch64::U64(1))
    {
      char chr = core.untaint(Bad(), core.MemRead8(addr));
      if (not chr)
        break;
      path += chr;
    }

  // std::cerr << "Host path:     " << path << std::endl;
  // std::cerr << "Host offset:   " << offset << std::endl;
  // std::cerr << "Guest pointer: " << ptr << std::endl;
  // std::cerr << "Guest size:    " << size << std::endl;

  auto itr = files.lower_bound(path);
  if (itr == files.end() or itr->first > path)
    {
      File file(path.c_str(), size);
      if (file.fd < 0) return 0;
      itr = files.emplace_hint(itr, std::piecewise_construct, std::forward_as_tuple(std::move(path)), std::forward_as_tuple(std::move(file)));
    }
  
  for (AArch64::SliceIterator slice(ptr + offset, size - offset, AArch64::mem_acc_type::read); slice.vnext(core);)
    {
      lseek(itr->second.fd, offset, SEEK_SET);
      uint64_t done = ::write(itr->second.fd, static_cast<void const*>(slice.data), slice.size);
      if (done != slice.size)
        {
          if (done == uint64_t(-1)) perror(__FUNCTION__);
          else std::cerr << itr->first << ": file write abruptly stopped at " << (offset + done) << " bytes.\n";
          return offset;
        }
      if (std::any_of(slice.udat, slice.udat + slice.size, [](uint8_t b) { return b != 0; } ))
        std::cerr << itr->first << ": writing undefined bits.\n";
      offset += slice.size;
      core.SetGZR(1, AArch64::U64(offset));
    }          

  if (--(itr->second.refcount) <= 0)
    files.erase(itr);

  return size;
}

