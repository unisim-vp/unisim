/*
 *  Copyright (c) 2011 Commissariat a l'Energie Atomique (CEA) All rights
 *  reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *   endorse or promote products derived from this software without specific
 *   prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 *          Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_UTIL_OS_LINUX_CALLS_TCC__
#define __UNISIM_UTIL_OS_LINUX_CALLS_TCC__

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

#ifdef WIN32
#include <process.h>
#include <windows.h>
#else
#include <sys/times.h>
#include <sys/time.h>
#endif

#include <string.h>
#include <fstream>
#include <stdexcept>

#include "unisim/util/endian/endian.hh"
#include "unisim/util/os/linux_os/linux.hh"
#include "unisim/util/os/linux_os/files_flags.hh"
#include "unisim/util/os/linux_os/arm.hh"
#include "unisim/util/os/linux_os/ppc.hh"

namespace unisim {
namespace util {
namespace os {
namespace linux_os {

#ifdef WIN32
// see http://mathieuturcotte.ca/textes/windows-gettimeofday
struct timezone {
  int tz_minuteswest;     /* minutes west of Greenwich */
  int tz_dsttime;         /* type of DST correction */
};
int gettimeofday(struct timeval* p, struct timezone* tz) {
  ULARGE_INTEGER ul; // As specified on MSDN.
  FILETIME ft;

  // Returns a 64-bit value representing the number of
  // 100-nanosecond intervals since January 1, 1601 (UTC).
  GetSystemTimeAsFileTime(&ft);

  // Fill ULARGE_INTEGER low and high parts.
  ul.LowPart = ft.dwLowDateTime;
  ul.HighPart = ft.dwHighDateTime;
  // Convert to microseconds.
  ul.QuadPart /= 10ULL;
  // Remove Windows to UNIX Epoch delta.
  ul.QuadPart -= 11644473600000000ULL;
  // Modulo to retrieve the microseconds.
  p->tv_usec = (long) (ul.QuadPart % 1000000LL);
  // Divide to retrieve the seconds.
  p->tv_sec = (long) (ul.QuadPart / 1000000LL);

  tz->tz_minuteswest = 0;
  tz->tz_dsttime = 0;

  return 0;
}
#endif

using unisim::util::endian::Host2Target;
using unisim::util::endian::Target2Host;

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_unknown()
{
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "Unimplemented system call #" << current_syscall_id_ << EndDebugInfo;
  SetSystemCallStatus(-1, true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_exit() {
  std::string name = "exit";
  int ret;

  ret = GetSystemCallParam(0);
  if (unlikely(verbose_))
    logger_ << DebugInfo << "LSC_exit with ret = 0X" << std::hex << ret << std::dec
        << EndDebugInfo;
  terminated_=true;
  return_status_=ret;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_read() {
  std::string name = "read";
  int fd;
  size_t count;
  ADDRESS_TYPE buf_addr;
  void *buf;
  size_t ret;

  fd = GetSystemCallParam(0);
  buf_addr = (ADDRESS_TYPE) GetSystemCallParam(1);
  count = (size_t) GetSystemCallParam(2);

  buf = malloc(count);

  if (buf) {
    ret = read(fd, buf, count);
    if (ret > 0) WriteMem(buf_addr, (uint8_t *)buf, ret);
    free(buf);
  } else
    ret = (size_t)-1;

  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "read(fd=" << fd << ", buf=0x" << std::hex << buf_addr << std::dec
        << ", count=" << count << ") return " << ret << EndDebugInfo;

  SetSystemCallStatus(ret, ret < 0);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_write() {
  int fd;
  size_t count;
  void *buf;
  ADDRESS_TYPE buf_addr;
  size_t ret;

  fd = GetSystemCallParam(0);
  buf_addr = GetSystemCallParam(1);
  count = (size_t)GetSystemCallParam(2);
  buf = malloc(count);

  ret = (size_t)-1;

  if (buf) {
    ReadMem(buf_addr, (uint8_t *)buf, count);
    ret = write(fd, buf, count);
    if (unlikely(verbose_)) {
      logger_ << DebugInfo
        << "write(fd=" << fd << ", buf=0x" << std::hex << buf_addr << std::dec
        << ", count=" << count << ") return " << ret << std::endl
        << "buffer =";
      for (size_t i = 0; i < count; i++)
        logger_
            << " " << std::hex << (unsigned int)((uint8_t *)buf)[i] << std::dec;
      logger_
        << EndDebugInfo;
    }
    free(buf);
  } else
    ret = (size_t)-1;

  SetSystemCallStatus(ret, ret < 0);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_open() {
  ADDRESS_TYPE addr;
  int pathnamelen;
  char *pathname;
  int flags;
  int64_t ret;
  mode_t mode;

  addr = GetSystemCallParam(0);
  pathnamelen = StringLength(addr);
  pathname = (char *) malloc(pathnamelen + 1);
  ReadMem(addr, (uint8_t *)pathname, pathnamelen + 1);
  flags = GetSystemCallParam(1);
  mode = GetSystemCallParam(2);
  if(((strncmp(pathname, "/dev", 4) == 0) && ((pathname[4] == 0) || (pathname[4] == '/'))) ||
     ((strncmp(pathname, "/proc", 5) == 0) && ((pathname[5] == 0) || (pathname[5] == '/'))) ||
     ((strncmp(pathname, "/sys", 4) == 0) && ((pathname[4] == 0) || (pathname[4] == '/'))) ||
     ((strncmp(pathname, "/var", 4) == 0) && ((pathname[4] == 0) || (pathname[4] == '/'))))
  {
    // deny access to /dev, /proc, /sys, /var
    ret = -EACCES;
  }
  else
  {
#if defined(linux)
    ret = open(pathname, flags, mode);
#else
    int host_flags = 0;
    int host_mode = 0;
    // non-Linux open flags encoding may differ from a true Linux host
    if ((flags & LINUX_O_ACCMODE) == LINUX_O_RDONLY) host_flags = (host_flags & ~O_ACCMODE) | O_RDONLY;
    if ((flags & LINUX_O_ACCMODE) == LINUX_O_WRONLY) host_flags = (host_flags & ~O_ACCMODE) | O_WRONLY;
    if ((flags & LINUX_O_ACCMODE) == LINUX_O_RDWR) host_flags = (host_flags & ~O_ACCMODE) | O_RDWR;
    if (flags & LINUX_O_CREAT) host_flags |= O_CREAT;
    if (flags & LINUX_O_EXCL) host_flags |= O_EXCL;
    if (flags & LINUX_O_TRUNC) host_flags |= O_TRUNC;
    if (flags & LINUX_O_APPEND) host_flags |= O_APPEND;
#if defined(WIN32) || defined(WIN64)
    host_flags |= O_BINARY; // Linux opens file as binary files
    host_mode = mode & S_IRWXU; // Windows doesn't have bits for group and others
#else
    host_mode = mode; // other UNIX systems should have the same bit encoding for protection
#endif
    ret = open(pathname, host_flags, host_mode);
#endif
  }
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "open(pathname=\"" << pathname << "\", flags=0x" << std::hex << flags
        << ", mode=0x" << mode << std::dec << ") return " << ret
        << EndDebugInfo;

  free(pathname);
  SetSystemCallStatus(ret, (ret < 0));
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_close() {
  int fd;
  int ret;

  fd = GetSystemCallParam(0);
  ret = close(fd);
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "close(fd=" << fd << ") return " << ret
        << EndDebugInfo;
  SetSystemCallStatus(ret, ret < 0);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_lseek() {
  int fildes;
  off_t offset;
  int whence;
  off_t ret;

  fildes = GetSystemCallParam(0);
  offset = GetSystemCallParam(1);
  whence = GetSystemCallParam(2);
  ret = lseek(fildes, offset, whence);
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "lseek(fildes=" << fildes << ", offset=" << offset
        << ", whence=" << whence << ") return " << ret
        << EndDebugInfo;

  if (ret == (off_t)-1)
    SetSystemCallStatus(errno, true);
  else
    SetSystemCallStatus(ret, false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_getpid() {
  pid_t ret;

  ret = (pid_t) getpid();
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "getpid() return " << ret
        << EndDebugInfo;
  SetSystemCallStatus(ret,false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_getuid() {
#ifdef WIN32
  uint32_t ret = 0;
#else
  uid_t ret;

  ret = getuid();
#endif
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "getuid() return " << ret
        << EndDebugInfo;
  SetSystemCallStatus(ret,false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_access() {
  ADDRESS_TYPE addr;
  int pathnamelen;
  char *pathname;
  mode_t mode;
  int ret;

  addr = GetSystemCallParam(0);
  pathnamelen = StringLength(addr);
  pathname = (char *) malloc(pathnamelen + 1);
  ReadMem(addr, (uint8_t *)pathname, pathnamelen + 1);
  mode = GetSystemCallParam(1);
  if(((strncmp(pathname, "/dev", 4) == 0) && ((pathname[4] == 0) || (pathname[4] == '/'))) ||
     ((strncmp(pathname, "/proc", 5) == 0) && ((pathname[5] == 0) || (pathname[5] == '/'))) ||
     ((strncmp(pathname, "/sys", 4) == 0) && ((pathname[4] == 0) || (pathname[4] == '/'))) ||
     ((strncmp(pathname, "/var", 4) == 0) && ((pathname[4] == 0) || (pathname[4] == '/'))))
  {
    // deny access to /dev, /proc, /sys, /var
    ret = -EACCES;
  }
  else
	{
#if defined(WIN32) || defined(WIN64)
    int win_mode = 0;
    win_mode = mode & S_IRWXU; // Windows doesn't have bits for group and others
    ret = access(pathname, win_mode);
#else
    ret = access(pathname, mode);
#endif
  }
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "access(pathname=\"" << pathname
        << "\", mode=0x" << std::hex << mode << std::dec << ") return " << ret
        << EndDebugInfo;
  free(pathname);
  SetSystemCallStatus(ret, ret < 0);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_times() {
  int ret;
  ADDRESS_TYPE buf_addr;
  buf_addr = GetSystemCallParam(0);

  if ((system_type_.compare("arm") == 0) ||
      (system_type_.compare("arm-eabi") == 0)) {
    struct arm_tms target_tms;
    ret = Times(&target_tms);

    if (ret >= 0) {
      WriteMem(buf_addr, (uint8_t *)&target_tms, sizeof(target_tms));
    }
  } else if (system_type_.compare("ppc") == 0) {
    struct powerpc_tms target_tms;
    ret = Times(&target_tms);

    if (ret >= 0) {
      WriteMem(buf_addr, (uint8_t *)&target_tms, sizeof(target_tms));
    }
  } else ret = -1;

  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "times(buf=0x" << std::hex << buf_addr << std::dec << ") return " << ret
        << EndDebugInfo;
  SetSystemCallStatus(ret, ret != -1);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_brk() {
  ADDRESS_TYPE new_brk_point;

  new_brk_point = GetSystemCallParam(0);

  if (new_brk_point > GetBrkPoint())
    SetBrkPoint(new_brk_point);

  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "brk(new_brk_point=0x" << std::hex << new_brk_point
        << ") return 0x" << GetBrkPoint() << std::dec
        << EndDebugInfo;
  SetSystemCallStatus(GetBrkPoint(),false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_getgid() {
#ifdef WIN32
  uint32_t ret = 0;
#else
  gid_t ret;

  ret = getgid();
#endif
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "getgid() return " << ret << EndDebugInfo;
  SetSystemCallStatus(ret,false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_geteuid() {
#ifdef WIN32
  uint32_t ret = 0;
#else
  uid_t ret;

  ret = geteuid();
#endif
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "geteuid() return " << ret << EndDebugInfo;
  SetSystemCallStatus(ret,false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_getegid() {
#ifdef WIN32
  uint32_t ret = 0;
#else
  gid_t ret;

  ret = getegid();
#endif
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "getegid() return " << ret << EndDebugInfo;
  SetSystemCallStatus(ret,false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_munmap() {
  // Note: currently disabled
  SetSystemCallStatus(-1,false); return;
  size_t u = (size_t)(GetSystemCallParam(1));

  if (GetMmapBrkPoint() - u > GetMmapBrkPoint()) {
    SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
    if (unlikely(verbose_))
      logger_ << DebugInfo
          << "size = " << u
          << ", ret = 0x" << std::hex << ((PARAMETER_TYPE)(-EINVAL)) << std::dec
          << EndDebugInfo;
    return;
  }

  if (GetMmapBrkPoint() - u < GetMmapBase())
    u = (size_t)(GetMmapBrkPoint() - GetMmapBase());

  if (GetMmapBrkPoint() - u >= GetMmapBrkPoint()) {
    SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
    if (unlikely(verbose_))
      logger_ << DebugInfo
          << "size = " << u
          << ", ret = 0x" << std::hex << ((PARAMETER_TYPE)(-EINVAL)) << std::dec
          << EndDebugInfo;
    return;
  }

  SetSystemCallStatus((PARAMETER_TYPE)0,false);
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "size = " << u
        << ", ret = 0x" << std::hex << 0 << std::dec
        << EndDebugInfo;
  SetMmapBrkPoint(GetMmapBrkPoint() - u);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_stat() {
  // TODO write LSC_stat
  logger_ << DebugWarning
      << "TODO LSC_stat" << EndDebugWarning;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
int Linux<ADDRESS_TYPE, PARAMETER_TYPE>::Stat(
    int fd,
    struct powerpc_stat *target_stat) {
  int ret;
  struct stat host_stat;
  ret = fstat(fd, &host_stat);
  if (ret < 0) return ret;

#if defined(__x86_64) || defined(__amd64) || defined(__LP64__) || defined(_LP64) || defined(__amd64__)
  // 64-bit host
  target_stat->st_dev = Host2Target(endianness_, (uint64_t) host_stat.st_dev);
  target_stat->st_ino = Host2Target(endianness_, (uint64_t) host_stat.st_ino);
  target_stat->st_mode = Host2Target(endianness_, (uint32_t) host_stat.st_mode);
  target_stat->st_nlink = Host2Target(endianness_, (uint64_t) host_stat.st_nlink);
  target_stat->st_uid = Host2Target(endianness_, (uint32_t) host_stat.st_uid);
  target_stat->st_gid = Host2Target(endianness_, (uint32_t) host_stat.st_gid);
  target_stat->st_rdev = Host2Target(endianness_, (int64_t) host_stat.st_rdev);
  target_stat->st_size = Host2Target(endianness_, (int64_t) host_stat.st_size);
#if defined(WIN64) // Windows x64
  target_stat->st_blksize = Host2Target((int32_t) 512);
  target_stat->st_blocks =
      Host2Target((int64_t)((host_stat.st_size + 511) / 512));
  target_stat->st_atim.tv_sec =
      Host2Target(endianness_, (int64_t) host_stat.st_atim);
  target_stat->st_atim.tv_nsec = 0;
  target_stat->st_mtim.tv_sec =
      Host2Target(endianness_, (int64_t) host_stat.st_mtim);
  target_stat->st_mtim.tv_nsec = 0;
  target_stat->st_ctim.tv_sec =
      Host2Target(endianness_, (int64_t) host_stat.st_ctim);
  target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) // Linux x64
  target_stat->st_blksize =
      Host2Target(endianness_, (int64_t) host_stat.st_blksize);
  target_stat->st_blocks =
      Host2Target(endianness_, (int64_t) host_stat.st_blocks);
  target_stat->st_atim.tv_sec =
      Host2Target(endianness_, (int64_t) host_stat.st_atim.tv_sec);
  target_stat->st_atim.tv_nsec =
      Host2Target(endianness_, (int64_t) host_stat.st_atim.tv_nsec);
  target_stat->st_mtim.tv_sec =
      Host2Target(endianness_, (int64_t) host_stat.st_mtim.tv_sec);
  target_stat->st_mtim.tv_nsec =
      Host2Target(endianness_, (int64_t) host_stat.st_mtim.tv_nsec);
  target_stat->st_ctim.tv_sec =
      Host2Target(endianness_, (int64_t) host_stat.st_ctim.tv_sec);
  target_stat->st_ctim.tv_nsec =
      Host2Target(endianness_, (int64_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // darwin PPC64/x86_64
  target_stat->st_blksize =
      Host2Target(endianness_, (int64_t) host_stat.st_blksize);
  target_stat->st_blocks =
      Host2Target(endianness_, (int64_t) host_stat.st_blocks);
  target_stat->st_atim.tv_sec =
      Host2Target(endianness_, (int64_t) host_stat.st_atimespec.tv_sec);
  target_stat->st_atim.tv_nsec =
      Host2Target(endianness_, (int64_t) host_stat.st_atimespec.tv_nsec);
  target_stat->st_mtim.tv_sec =
      Host2Target(endianness_, (int64_t) host_stat.st_mtimespec.tv_sec);
  target_stat->st_mtim.tv_nsec =
      Host2Target(endianness_, (int64_t) host_stat.st_mtimespec.tv_nsec);
  target_stat->st_ctim.tv_sec =
      Host2Target(endianness_, (int64_t) host_stat.st_ctimespec.tv_sec);
  target_stat->st_ctim.tv_nsec =
      Host2Target(endianness_, (int64_t) host_stat.st_ctimespec.tv_nsec);
#endif

#else
  // 32-bit host
  target_stat->st_dev = Host2Target(endianness_, (uint64_t) host_stat.st_dev);
  target_stat->st_ino = Host2Target(endianness_, (uint32_t) host_stat.st_ino);
  target_stat->st_mode = Host2Target(endianness_, (uint32_t) host_stat.st_mode);
  target_stat->st_nlink = Host2Target(endianness_, (uint32_t) host_stat.st_nlink);
  target_stat->st_uid = Host2Target(endianness_, (uint32_t) host_stat.st_uid);
  target_stat->st_gid = Host2Target(endianness_, (uint32_t) host_stat.st_gid);
  target_stat->st_rdev = Host2Target(endianness_, (int64_t) host_stat.st_rdev);
  target_stat->st_size = Host2Target(endianness_, (int64_t) host_stat.st_size);
#if defined(WIN32) // Windows 32
  target_stat->st_blksize = Host2Target(endianness_, (int32_t) 512);
  target_stat->st_blocks =
      Host2Target(endianness_, (int64_t)((host_stat.st_size + 511) / 512));
  target_stat->st_atim.tv_sec =
      Host2Target(endianness_, (int32_t) host_stat.st_atime);
  target_stat->st_atim.tv_nsec = 0;
  target_stat->st_mtim.tv_sec =
      Host2Target(endianness_, (int32_t) host_stat.st_mtime);
  target_stat->st_mtim.tv_nsec = 0;
  target_stat->st_ctim.tv_sec =
      Host2Target(endianness_, (int32_t) host_stat.st_ctime);
  target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) // Linux 32
  target_stat->st_blksize =
      Host2Target(endianness_, (int32_t) host_stat.st_blksize);
  target_stat->st_blocks =
      Host2Target(endianness_, (int64_t) host_stat.st_blocks);
  target_stat->st_atim.tv_sec =
      Host2Target(endianness_, (int32_t) host_stat.st_atim.tv_sec);
  target_stat->st_atim.tv_nsec =
      Host2Target(endianness_, (int32_t) host_stat.st_atim.tv_nsec);
  target_stat->st_mtim.tv_sec =
      Host2Target(endianness_, (int32_t) host_stat.st_mtim.tv_sec);
  target_stat->st_mtim.tv_nsec =
      Host2Target(endianness_, (int32_t) host_stat.st_mtim.tv_nsec);
  target_stat->st_ctim.tv_sec =
      Host2Target(endianness_, (int32_t) host_stat.st_ctim.tv_sec);
  target_stat->st_ctim.tv_nsec =
      Host2Target(endianness_, (int32_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // Darwin PPC32/x86
  target_stat->st_blksize =
      Host2Target(endianness_, (int32_t) host_stat.st_blksize);
  target_stat->st_blocks =
      Host2Target(endianness_, (int64_t) host_stat.st_blocks);
  target_stat->st_atim.tv_sec =
      Host2Target(endianness_, (int32_t) host_stat.st_atimespec.tv_sec);
  target_stat->st_atim.tv_nsec =
      Host2Target(endianness_, (int32_t) host_stat.st_atimespec.tv_nsec);
  target_stat->st_mtim.tv_sec =
      Host2Target(endianness_, (int32_t) host_stat.st_mtimespec.tv_sec);
  target_stat->st_mtim.tv_nsec =
      Host2Target(endianness_, (int32_t) host_stat.st_mtimespec.tv_nsec);
  target_stat->st_ctim.tv_sec =
      Host2Target(endianness_, (int32_t) host_stat.st_ctimespec.tv_sec);
  target_stat->st_ctim.tv_nsec =
      Host2Target(endianness_, (int32_t) host_stat.st_ctimespec.tv_nsec);
#endif

#endif
  target_stat->__pad1 = 0;
  target_stat->__pad2 = 0;
  target_stat->__unused4 = 0;
  target_stat->__unused5 = 0;
  return ret;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
int Linux<ADDRESS_TYPE, PARAMETER_TYPE>::Stat64(
    int fd, struct powerpc_stat64 *target_stat) {
  int ret;
#if defined(WIN32) || defined(WIN64)
  struct _stati64 host_stat;
  ret = _fstati64(fd, &host_stat);
#elif defined(linux)
  struct stat64 host_stat;
  ret = fstat64(fd, &host_stat);
#elif defined(__APPLE_CC__)
  struct stat host_stat;
  ret = fstat(fd, &host_stat);
#endif

  if (ret < 0) return ret;

#if defined(__x86_64) || defined(__amd64) || defined(__x86_64__) || defined(__amd64__) || defined(__LP64__) || defined(_LP64)
  // 64-bit host
  target_stat->st_dev = Host2Target(endianness_, (uint64_t) host_stat.st_dev);
  target_stat->st_ino = Host2Target(endianness_, (uint64_t) host_stat.st_ino);
  target_stat->st_mode = Host2Target(endianness_, (uint32_t) host_stat.st_mode);
  target_stat->st_nlink = Host2Target(endianness_, (uint64_t) host_stat.st_nlink);
  target_stat->st_uid = Host2Target(endianness_, (uint32_t) host_stat.st_uid);
  target_stat->st_gid = Host2Target(endianness_, (uint32_t) host_stat.st_gid);
  target_stat->st_rdev = Host2Target(endianness_, (int64_t) host_stat.st_rdev);
  target_stat->st_size = Host2Target(endianness_, (int64_t) host_stat.st_size);
#if defined(WIN64) // Windows x64
  target_stat->st_blksize = Host2Target(endianness_, (int32_t) 512);
  target_stat->st_blocks =
      Host2Target(endianness_, (int64_t)((host_stat.st_size + 511) / 512));
  target_stat->st_atim.tv_sec =
      Host2Target(endianness_, (int64_t) host_stat.st_atim);
  target_stat->st_atim.tv_nsec = 0;
  target_stat->st_mtim.tv_sec =
      Host2Target(endianness_, (int64_t) host_stat.st_mtim);
  target_stat->st_mtim.tv_nsec = 0;
  target_stat->st_ctim.tv_sec =
      Host2Target(endianness_, (int64_t) host_stat.st_ctim);
  target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) // Linux x64
  target_stat->st_blksize =
      Host2Target(endianness_, (int32_t) host_stat.st_blksize);
  target_stat->st_blocks =
      Host2Target(endianness_, (int64_t) host_stat.st_blocks);
  target_stat->st_atim.tv_sec =
      Host2Target(endianness_, (int64_t) host_stat.st_atim.tv_sec);
  target_stat->st_atim.tv_nsec =
      Host2Target(endianness_, (int64_t) host_stat.st_atim.tv_nsec);
  target_stat->st_mtim.tv_sec =
      Host2Target(endianness_, (int64_t) host_stat.st_mtim.tv_sec);
  target_stat->st_mtim.tv_nsec =
      Host2Target(endianness_, (int64_t) host_stat.st_mtim.tv_nsec);
  target_stat->st_ctim.tv_sec =
      Host2Target(endianness_, (int64_t) host_stat.st_ctim.tv_sec);
  target_stat->st_ctim.tv_nsec =
      Host2Target(endianness_, (int64_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // Darwin PPC64/x86_64
  target_stat->st_blksize =
      Host2Target(endianness_, (int32_t) host_stat.st_blksize);
  target_stat->st_blocks =
      Host2Target(endianness_, (int64_t) host_stat.st_blocks);
  target_stat->st_atim.tv_sec =
      Host2Target(endianness_, (int64_t) host_stat.st_atimespec.tv_sec);
  target_stat->st_atim.tv_nsec =
      Host2Target(endianness_, (int64_t) host_stat.st_atimespec.tv_nsec);
  target_stat->st_mtim.tv_sec =
      Host2Target(endianness_, (int64_t) host_stat.st_mtimespec.tv_sec);
  target_stat->st_mtim.tv_nsec =
      Host2Target(endianness_, (int64_t) host_stat.st_mtimespec.tv_nsec);
  target_stat->st_ctim.tv_sec =
      Host2Target(endianness_, (int64_t) host_stat.st_ctimespec.tv_sec);
  target_stat->st_ctim.tv_nsec =
      Host2Target(endianness_, (int64_t) host_stat.st_ctimespec.tv_nsec);
#endif

#else
  // 32-bit host
  target_stat->st_dev = Host2Target(endianness_, (uint64_t) host_stat.st_dev);
  target_stat->st_ino = Host2Target(endianness_, (uint64_t) host_stat.st_ino);
  target_stat->st_mode = Host2Target(endianness_, (uint32_t) host_stat.st_mode);
  target_stat->st_nlink = Host2Target(endianness_, (uint32_t) host_stat.st_nlink);
  target_stat->st_uid = Host2Target(endianness_, (uint32_t) host_stat.st_uid);
  target_stat->st_gid = Host2Target(endianness_, (uint32_t) host_stat.st_gid);
  target_stat->st_rdev = Host2Target(endianness_, (int64_t) host_stat.st_rdev);
  target_stat->st_size = Host2Target(endianness_, (int64_t) host_stat.st_size);
#if defined(WIN32) // Windows 32
  target_stat->st_blksize = Host2Target(endianness_, (int32_t) 512);
  target_stat->st_blocks =
      Host2Target(endianness_, (int64_t)((host_stat.st_size + 511) / 512));
  target_stat->st_atim.tv_sec =
      Host2Target(endianness_, (int32_t) host_stat.st_atime);
  target_stat->st_atim.tv_nsec = 0;
  target_stat->st_mtim.tv_sec =
      Host2Target(endianness_, (int32_t) host_stat.st_mtime);
  target_stat->st_mtim.tv_nsec = 0;
  target_stat->st_ctim.tv_sec =
      Host2Target(endianness_, (int32_t) host_stat.st_ctime);
  target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) // Linux 32
  target_stat->st_blksize =
      Host2Target(endianness_, (int32_t) host_stat.st_blksize);
  target_stat->st_blocks =
      Host2Target(endianness_, (int64_t) host_stat.st_blocks);
  target_stat->st_atim.tv_sec =
      Host2Target(endianness_, (int32_t) host_stat.st_atim.tv_sec);
  target_stat->st_atim.tv_nsec =
      Host2Target(endianness_, (int32_t) host_stat.st_atim.tv_nsec);
  target_stat->st_mtim.tv_sec =
      Host2Target(endianness_, (int32_t) host_stat.st_mtim.tv_sec);
  target_stat->st_mtim.tv_nsec =
      Host2Target(endianness_, (int32_t) host_stat.st_mtim.tv_nsec);
  target_stat->st_ctim.tv_sec =
      Host2Target(endianness_, (int32_t) host_stat.st_ctim.tv_sec);
  target_stat->st_ctim.tv_nsec =
      Host2Target(endianness_, (int32_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // Darwin PPC32/x86
  target_stat->st_blksize =
      Host2Target(endianness_, (int32_t) host_stat.st_blksize);
  target_stat->st_blocks =
      Host2Target(endianness_, (int64_t) host_stat.st_blocks);
  target_stat->st_atim.tv_sec =
      Host2Target(endianness_, (int32_t) host_stat.st_atimespec.tv_sec);
  target_stat->st_atim.tv_nsec =
      Host2Target(endianness_, (int32_t) host_stat.st_atimespec.tv_nsec);
  target_stat->st_mtim.tv_sec =
      Host2Target(endianness_, (int32_t) host_stat.st_mtimespec.tv_sec);
  target_stat->st_mtim.tv_nsec =
      Host2Target(endianness_, (int32_t) host_stat.st_mtimespec.tv_nsec);
  target_stat->st_ctim.tv_sec =
      Host2Target(endianness_, (int32_t) host_stat.st_ctimespec.tv_sec);
  target_stat->st_ctim.tv_nsec =
      Host2Target(endianness_, (int32_t) host_stat.st_ctimespec.tv_nsec);
#endif

#endif
  target_stat->__pad2 = 0;
  target_stat->__pad3 = 0;
  return ret;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
int Linux<ADDRESS_TYPE, PARAMETER_TYPE>::Stat64(
    int fd, struct arm_stat64 *target_stat) {
  int ret;
#if defined(WIN32) || defined(WIN64)
  struct _stati64 host_stat;
  ret = _fstati64(fd, &host_stat);
#elif defined(linux)
  struct stat64 host_stat;
  ret = fstat64(fd, &host_stat);
#elif defined(__APPLE_CC__)
  struct stat host_stat;
  ret = fstat(fd, &host_stat);
#endif
  if (ret < 0) return ret;

#if defined(__x86_64) || defined(__amd64) || defined(__x86_64__) || defined(__amd64__) || defined(__LP64__) || defined(_LP64)
  // 64-bit host
  target_stat->st_dev = Host2Target(endianness_, (uint64_t) host_stat.st_dev);
  target_stat->__st_ino = Host2Target(endianness_, (uint32_t) host_stat.st_ino);
  target_stat->st_ino = Host2Target(endianness_, (uint64_t) host_stat.st_ino);
  target_stat->st_mode = Host2Target(endianness_, (uint32_t) host_stat.st_mode);
  target_stat->st_nlink = Host2Target(endianness_, (uint64_t) host_stat.st_nlink);
  target_stat->st_uid = Host2Target(endianness_, (uint32_t) host_stat.st_uid);
  target_stat->st_gid = Host2Target(endianness_, (uint32_t) host_stat.st_gid);
  target_stat->st_rdev = Host2Target(endianness_, (uint64_t) host_stat.st_rdev);
  target_stat->st_size = Host2Target(endianness_, (int64_t) host_stat.st_size);
#if defined(WIN64) // Windows x64
  target_stat->st_blksize = Host2Target(endianness_, (int32_t) 512);
  target_stat->st_blocks =
      Host2Target(endianness_, (uint64_t)((host_stat.st_size + 511) / 512));
  target_stat->st_atim.tv_sec =
      Host2Target(endianness_, (uint32_t) host_stat.st_atim);
  target_stat->st_atim.tv_nsec = 0;
  target_stat->st_mtim.tv_sec =
      Host2Target(endianness_, (uint32_t) host_stat.st_mtim);
  target_stat->st_mtim.tv_nsec = 0;
  target_stat->st_ctim.tv_sec =
      Host2Target(endianness_, (uint32_t) host_stat.st_ctim);
  target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) // Linux x64
  target_stat->st_blksize =
      Host2Target(endianness_, (uint32_t) host_stat.st_blksize);
  target_stat->st_blocks =
      Host2Target(endianness_, (uint64_t) host_stat.st_blocks);
  target_stat->st_atim.tv_sec =
      Host2Target(endianness_, (uint32_t) host_stat.st_atim.tv_sec);
  target_stat->st_atim.tv_nsec =
      Host2Target(endianness_, (uint32_t) host_stat.st_atim.tv_nsec);
  target_stat->st_mtim.tv_sec =
      Host2Target(endianness_, (uint32_t) host_stat.st_mtim.tv_sec);
  target_stat->st_mtim.tv_nsec =
      Host2Target(endianness_, (uint32_t) host_stat.st_mtim.tv_nsec);
  target_stat->st_ctim.tv_sec =
      Host2Target(endianness_, (uint32_t) host_stat.st_ctim.tv_sec);
  target_stat->st_ctim.tv_nsec =
      Host2Target(endianness_, (uint32_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // darwin PPC64/x86_64
  target_stat->st_blksize =
      Host2Target(endianness_, (uint32_t) host_stat.st_blksize);
  target_stat->st_blocks =
      Host2Target(endianness_, (int64_t) host_stat.st_blocks);
  target_stat->st_atim.tv_sec =
      Host2Target(endianness_, (uint32_t) host_stat.st_atimespec.tv_sec);
  target_stat->st_atim.tv_nsec =
      Host2Target(endianness_, (uint32_t) host_stat.st_atimespec.tv_nsec);
  target_stat->st_mtim.tv_sec =
      Host2Target(endianness_, (uint32_t) host_stat.st_mtimespec.tv_sec);
  target_stat->st_mtim.tv_nsec =
      Host2Target(endianness_, (uint32_t) host_stat.st_mtimespec.tv_nsec);
  target_stat->st_ctim.tv_sec =
      Host2Target(endianness_, (uint32_t) host_stat.st_ctimespec.tv_sec);
  target_stat->st_ctim.tv_nsec =
      Host2Target(endianness_, (uint32_t) host_stat.st_ctimespec.tv_nsec);
#endif

#else
  // 32-bit host
  target_stat->st_dev = Host2Target(endianness_, (uint64_t) host_stat.st_dev);
  target_stat->__st_ino = Host2Target(endianness_, (uint32_t) host_stat.st_ino);
  target_stat->st_ino = Host2Target(endianness_, (uint64_t) host_stat.st_ino);
  target_stat->st_mode = Host2Target(endianness_, (uint32_t) host_stat.st_mode);
  target_stat->st_nlink = Host2Target(endianness_, (uint32_t) host_stat.st_nlink);
  target_stat->st_uid = Host2Target(endianness_, (uint32_t) host_stat.st_uid);
  target_stat->st_gid = Host2Target(endianness_, (uint32_t) host_stat.st_gid);
  target_stat->st_rdev = Host2Target(endianness_, (uint64_t) host_stat.st_rdev);
  target_stat->st_size = Host2Target(endianness_, (int64_t) host_stat.st_size);
#if defined(WIN32) // Windows 32
  target_stat->st_blksize = Host2Target(endianness_, (int32_t) 512);
  target_stat->st_blocks =
      Host2Target(endianness_, (int64_t)((host_stat.st_size + 511) / 512));
  target_stat->st_atim.tv_sec =
      Host2Target(endianness_, (int32_t) host_stat.st_atime);
  target_stat->st_atim.tv_nsec = 0;
  target_stat->st_mtim.tv_sec =
      Host2Target(endianness_, (int32_t) host_stat.st_mtime);
  target_stat->st_mtim.tv_nsec = 0;
  target_stat->st_ctim.tv_sec =
      Host2Target(endianness_, (int32_t) host_stat.st_ctime);
  target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) // Linux 32
  target_stat->st_blksize = Host2Target(endianness_, (uint32_t)
                                        host_stat.st_blksize);
  target_stat->st_blocks = Host2Target(endianness_, (uint64_t)
                                       host_stat.st_blocks);
  target_stat->st_atim.tv_sec = Host2Target(endianness_, (uint32_t)
                                            host_stat.st_atim.tv_sec);
  target_stat->st_atim.tv_nsec = Host2Target(endianness_, (uint32_t)
                                             host_stat.st_atim.tv_nsec);
  target_stat->st_mtim.tv_sec = Host2Target(endianness_, (uint32_t)
                                            host_stat.st_mtim.tv_sec);
  target_stat->st_mtim.tv_nsec = Host2Target(endianness_, (uint32_t)
                                             host_stat.st_mtim.tv_nsec);
  target_stat->st_ctim.tv_sec = Host2Target(endianness_, (uint32_t)
                                            host_stat.st_ctim.tv_sec);
  target_stat->st_ctim.tv_nsec = Host2Target(endianness_, (uint32_t)
                                             host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // Darwin PPC32/x86
  target_stat->st_blksize = Host2Target(endianness_, (uint32_t)
                                        host_stat.st_blksize);
  target_stat->st_blocks = Host2Target(endianness_, (uint64_t)
                                       host_stat.st_blocks);
  target_stat->st_atim.tv_sec = Host2Target(endianness_, (uint32_t)
                                            host_stat.st_atimespec.tv_sec);
  target_stat->st_atim.tv_nsec = Host2Target(endianness_, (uint32_t)
                                             host_stat.st_atimespec.tv_nsec);
  target_stat->st_mtim.tv_sec = Host2Target(endianness_, (uint32_t)
                                            host_stat.st_mtimespec.tv_sec);
  target_stat->st_mtim.tv_nsec = Host2Target(endianness_, (uint32_t)
                                             host_stat.st_mtimespec.tv_nsec);
  target_stat->st_ctim.tv_sec = Host2Target(endianness_, (uint32_t)
                                            host_stat.st_ctimespec.tv_sec);
  target_stat->st_ctim.tv_nsec = Host2Target(endianness_, (uint32_t)
                                             host_stat.st_ctimespec.tv_nsec);
#endif

#endif
  target_stat->__pad1 = 0;
  target_stat->__pad2 = 0;
  target_stat->__pad3 = 0;
  return ret;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
int Linux<ADDRESS_TYPE, PARAMETER_TYPE>::Times(
    struct powerpc_tms *target_tms) {
  int ret;
#ifdef WIN32
  FILETIME ftCreationTime;
  FILETIME ftExitTime;
  FILETIME ftKernelTime;
  FILETIME ftUserTime;

  if (GetProcessTimes(GetCurrentProcess(), &ftCreationTime, &ftExitTime,
                      &ftKernelTime, &ftUserTime)) return -1;

  target_tms->tms_utime = Host2Target(endianness_, (uint32_t)
                                      ftUserTime.dwLowDateTime);
  target_tms->tms_stime = Host2Target(endianness_, (uint32_t)
                                      ftKernelTime.dwLowDateTime);
  target_tms->tms_cutime = 0;   // User CPU time of dead children
  target_tms->tms_cstime = 0;   // System CPU time of dead children
#else
  struct tms host_tms;

  ret = (int) times(&host_tms);
  target_tms->tms_utime = Host2Target(endianness_, (int32_t) host_tms.tms_utime);
  target_tms->tms_stime = Host2Target(endianness_, (int32_t) host_tms.tms_stime);
  target_tms->tms_cutime = Host2Target(endianness_, (int32_t)
                                       host_tms.tms_cutime);
  target_tms->tms_cstime = Host2Target(endianness_, (int32_t)
                                       host_tms.tms_cstime);
#endif
  return ret;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
int Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetTimeOfDay(
    struct powerpc_timeval *target_timeval,
    struct powerpc_timezone *target_timezone) {
  int ret;
  struct timeval host_tv;
  struct timezone host_tz;
  
  ret = gettimeofday(&host_tv, &host_tz);
  
  if(ret == 0)
  {
    if(target_timeval) {
      target_timeval->tv_sec = Host2Target(endianness_, (int32_t) host_tv.tv_sec);
      target_timeval->tv_usec = Host2Target(endianness_, (int32_t) host_tv.tv_usec);
    }
    if(target_timezone) {
      target_timezone->tz_minuteswest = Host2Target(endianness_, (int32_t) host_tz.tz_minuteswest);
      target_timezone->tz_dsttime = Host2Target(endianness_, (int32_t) host_tz.tz_dsttime);
    }
  }
  return ret;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
int Linux<ADDRESS_TYPE, PARAMETER_TYPE>::Times(struct arm_tms *target_tms) {
  int ret;
#ifdef WIN32
  FILETIME ftCreationTime;
  FILETIME ftExitTime;
  FILETIME ftKernelTime;
  FILETIME ftUserTime;

  if (GetProcessTimes(GetCurrentProcess(), &ftCreationTime, &ftExitTime,
                      &ftKernelTime, &ftUserTime)) return -1;

  target_tms->tms_utime = Host2Target(endianness_, (uint32_t)
                                      ftUserTime.dwLowDateTime);
  target_tms->tms_stime = Host2Target(endianness_, (uint32_t)
                                      ftKernelTime.dwLowDateTime);
  target_tms->tms_cutime = 0;   // User CPU time of dead children
  target_tms->tms_cstime = 0;   // System CPU time of dead children
#else
  struct tms host_tms;

  ret = (int) times(&host_tms);
  target_tms->tms_utime = Host2Target(endianness_, (int32_t) host_tms.tms_utime);
  target_tms->tms_stime = Host2Target(endianness_, (int32_t) host_tms.tms_stime);
  target_tms->tms_cutime = Host2Target(endianness_, (int32_t)
                                       host_tms.tms_cutime);
  target_tms->tms_cstime = Host2Target(endianness_, (int32_t)
                                       host_tms.tms_cstime);
#endif
  return ret;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
int Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetTimeOfDay(
    struct arm_timeval *target_timeval,
    struct arm_timezone *target_timezone) {
  int ret;
  struct timeval host_tv;
  struct timezone host_tz;
  
  ret = gettimeofday(&host_tv, &host_tz);
  
  if(ret == 0)
  {
    if(target_timeval) {
      target_timeval->tv_sec = Host2Target(endianness_, (int32_t) host_tv.tv_sec);
      target_timeval->tv_usec = Host2Target(endianness_, (int32_t) host_tv.tv_usec);
    }
    if(target_timezone) {
      target_timezone->tz_minuteswest = Host2Target(endianness_, (int32_t) host_tz.tz_minuteswest);
      target_timezone->tz_dsttime = Host2Target(endianness_, (int32_t) host_tz.tz_dsttime);
    }
  }
  return ret;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_fstat() {
  int ret;
  int fd;
  ADDRESS_TYPE buf_address;

  fd = GetSystemCallParam(0);
  buf_address = GetSystemCallParam(1);
  if ((system_type_.compare("arm") == 0) ||
      (system_type_.compare("arm-eabi") == 0))
    ret = -1;
  else if (system_type_.compare("ppc") == 0) {
    struct powerpc_stat target_stat;
    ret = Stat(fd, &target_stat);
    WriteMem(buf_address, (uint8_t *)&target_stat, sizeof(target_stat));
  }
  else ret = -1;

  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "stat(fd=" << fd
        << ", buf_addr=0x" << std::hex << buf_address << std::dec
        << ") return " << ret << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)ret,ret < 0);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_uname() {
  int ret;
  ADDRESS_TYPE buf_addr = GetSystemCallParam(0);
  ret = 0;
  if ((system_type_.compare("arm") == 0) ||
      (system_type_.compare("arm-eabi") == 0)) {
    struct arm_utsname value;
    memset(&value, 0, sizeof(value));
    strncpy(value.sysname,
           utsname_sysname_.c_str(), sizeof(value.sysname) - 1);
    strncpy(value.nodename,
           utsname_nodename_.c_str(), sizeof(value.nodename) - 1);
    strncpy(value.release,
           utsname_release_.c_str(), sizeof(value.release) - 1);
    strncpy(value.version,
           utsname_version_.c_str(), sizeof(value.version) - 1);
    strncpy(value.machine,
           utsname_machine_.c_str(), sizeof(value.machine));
    WriteMem(buf_addr, (uint8_t *)&value, sizeof(value));
  } else if (system_type_.compare("ppc") == 0) {
    struct ppc_utsname value;
    memset(&value, 0, sizeof(value));
    strncpy(value.sysname,
           utsname_sysname_.c_str(), sizeof(value.sysname) - 1);
    strncpy(value.nodename,
           utsname_nodename_.c_str(), sizeof(value.nodename) - 1);
    strncpy(value.release,
           utsname_release_.c_str(), sizeof(value.release) - 1);
    strncpy(value.version,
           utsname_version_.c_str(), sizeof(value.version) - 1);
    strncpy(value.machine,
           utsname_machine_.c_str(), sizeof(value.machine) - 1);
    strncpy(value.domainname,
           utsname_domainname_.c_str(), sizeof(value.domainname) - 1);
    WriteMem(buf_addr, (uint8_t *)&value, sizeof(value));
  } else
    ret = -1;
  SetSystemCallStatus((PARAMETER_TYPE) ret, ret < 0);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_llseek() {
  int fd;
  uint32_t offset_high;
  uint32_t offset_low;
  uint64_t offset;
  PARAMETER_TYPE result_addr;
  uint64_t lseek_ret64;
  off_t lseek_ret;
  int whence;

  fd = GetSystemCallParam(0);
  offset_high = GetSystemCallParam(1);
  offset_low = GetSystemCallParam(2);
  offset = ((uint64_t) offset_high << 32) | (uint64_t) offset_low;
  result_addr = GetSystemCallParam(3);
  whence = GetSystemCallParam(4);
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "llseek(fd=" << fd
        << ", offset=" << offset
        << ", result_addr=0x" << std::hex << result_addr << std::dec
        << ", whence=" << whence << ")" << EndDebugInfo;
  if (offset_high == 0) {
    lseek_ret = lseek64(fd, offset, whence);
    if (lseek_ret >= 0) {
      lseek_ret64 = unisim::util::endian::Host2Target(endianness_, (uint64_t) lseek_ret);
      WriteMem(result_addr, (uint8_t *)&lseek_ret64, sizeof(lseek_ret64));
      SetSystemCallStatus((PARAMETER_TYPE)lseek_ret, false);
    } else
      SetSystemCallStatus((PARAMETER_TYPE)-errno, true);
  } else
    SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_writev() {
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "ret = 0x"
        << std::hex << ((PARAMETER_TYPE)(-EINVAL)) << std::dec << EndDebugInfo;

  SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_mmap() {
  // Note: currently disabled
  SetSystemCallStatus(-1,false); return;
  if (GetSystemCallParam(3) == 0x32) { /* MAP_PRIVATE | MAP_ANONYMOUS */
    SetSystemCallStatus(GetSystemCallParam(0),false);
    if (unlikely(verbose_))
      logger_  << DebugInfo << "map_type = 0x" << std::hex << GetSystemCallParam(3)
          << std::dec << ", size = " << GetSystemCallParam(1)
          << ", ret = 0x"
          << std::hex << ((PARAMETER_TYPE)GetSystemCallParam(0)) << std::dec
          << EndDebugInfo;
    return;
  }

  if ((GetSystemCallParam(3)&0xFF) != 0x22) { /* MAP_PRIVATE | MAP_ANONYMOUS */
    SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
    if (unlikely(verbose_))
      logger_ << DebugInfo
          << "map_type = 0x" << std::hex << GetSystemCallParam(3) << std::dec
          << ", size = " << GetSystemCallParam(1)
          << ", ret = 0x" << std::hex << ((PARAMETER_TYPE)(-EINVAL)) << std::dec
          << EndDebugInfo;
    return;
  }
  SetSystemCallStatus(GetMmapBrkPoint(),false);
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "map_type = 0x" << std::hex << GetSystemCallParam(3) << std::dec
        << ", size = " << GetSystemCallParam(1)
        << ", ret = 0x" << std::hex << GetMmapBrkPoint() << std::dec
        << EndDebugInfo;
  SetMmapBrkPoint(GetMmapBrkPoint() + GetSystemCallParam(1));
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_mmap2() {
  // Note: currently disabled
  SetSystemCallStatus(-1,false); return;
  if (GetSystemCallParam(3) != 0x22) { /* MAP_PRIVATE | MAP_ANONYMOUS */
    SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
    if (unlikely(verbose_))
      logger_ << DebugInfo
          << "map_type = 0x" << std::hex << GetSystemCallParam(3) << std::dec
          << ", size = " << GetSystemCallParam(1)
          << ", ret = 0x" << std::hex << ((PARAMETER_TYPE)(-EINVAL)) << std::dec
          << EndDebugInfo;
    return;
  }

  if (GetMmapBrkPoint() + GetSystemCallParam(1) > GetSystemCallParam(1)) {
    SetSystemCallStatus(GetMmapBrkPoint(),false);
    if (unlikely(verbose_))
      logger_ << DebugInfo
          << "map_type = 0x" << std::hex << GetSystemCallParam(3) << std::dec
          << ", size = " << GetSystemCallParam(1)
          << ", ret = 0x" << std::hex << GetMmapBrkPoint() << std::dec
          << EndDebugInfo;
    SetMmapBrkPoint(GetMmapBrkPoint() + GetSystemCallParam(1));
  } else {
    if (unlikely(verbose_))
      logger_ << DebugInfo
          << "map_type = 0x" << std::hex << GetSystemCallParam(3) << std::dec
          << ", size = " << GetSystemCallParam(1)
          << ", ret = 0x" << std::hex << ((PARAMETER_TYPE)(-EINVAL)) << std::dec
          << EndDebugInfo;
    SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
  }
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_stat64() {
  int ret;
  ADDRESS_TYPE buf_address;
  int fd;

  fd = GetSystemCallParam(0);
  buf_address = GetSystemCallParam(1);
  if ((system_type_.compare("arm") == 0) ||
      (system_type_.compare("arm-eabi") == 0)) {
    struct arm_stat64 target_stat;
    ret = Stat64(fd, &target_stat);
    WriteMem(buf_address, (uint8_t *)&target_stat, sizeof(target_stat));
  } else if (system_type_.compare("ppc") == 0) {
    struct powerpc_stat64 target_stat;
    ret = Stat64(fd, &target_stat);
    WriteMem(buf_address, (uint8_t *)&target_stat, sizeof(target_stat));
  } else ret = -1;
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "fd = " << fd << ", buf_address = 0x" << std::hex << buf_address << std::dec
        << ", ret = 0x" << std::hex << ret << std::dec
        << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)ret,ret < 0);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_fstat64() {
  int ret;
  ADDRESS_TYPE buf_address;
  int fd;

  fd = GetSystemCallParam(0);
  buf_address = GetSystemCallParam(1);
  if ((system_type_.compare("arm") == 0) ||
      (system_type_.compare("arm-eabi") == 0)) {
    struct arm_stat64 target_stat;
    ret = Stat64(fd, &target_stat);
    WriteMem(buf_address, (uint8_t *)&target_stat, sizeof(target_stat));
  } else if (system_type_.compare("ppc") == 0) {
    struct powerpc_stat64 target_stat;
    ret = Stat64(fd, &target_stat);
    WriteMem(buf_address, (uint8_t *)&target_stat, sizeof(target_stat));
  } else ret = -1;
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "fd = " << fd << ", buf_address = 0x" << std::hex << buf_address << std::dec
        << ", ret = 0x" << std::hex << ret << std::dec
        << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)ret,ret < 0);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_getuid32() {
#ifdef WIN32
  uint32_t ret = 0;
#else
  uid_t ret;

  ret = getuid();
#endif
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "ret = 0x" << std::hex << ((PARAMETER_TYPE)ret) << std::dec
        << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)ret,false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_getgid32() {
#ifdef WIN32
  uint32_t ret = 0;
#else
  gid_t ret;

  ret = getgid();
#endif
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "ret = 0x" << std::hex << ((PARAMETER_TYPE)ret) << std::dec
        << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)ret,false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_geteuid32() {
#ifdef WIN32
  uint32_t ret = 0;
#else
  uid_t ret;

  ret = geteuid();
#endif
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "ret = 0x" << std::hex << ((PARAMETER_TYPE)ret) << std::dec
        << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)ret,false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_getegid32() {
#ifdef WIN32
  uint32_t ret = 0;
#else
  gid_t ret;

  ret = getegid();
#endif
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "ret = 0x" << std::hex << ((PARAMETER_TYPE)ret) << std::dec
        << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)ret,false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_flistxattr() {
  // TODO implement LSC_flistxattr
  logger_ << DebugWarning
      << "TODO LSC_flistxattr"
      << EndDebugWarning;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_exit_group() {
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "ret = 0x" << std::hex << ((PARAMETER_TYPE)(-EINVAL)) << std::dec
        << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_fcntl() {
  int64_t ret;

#if defined(WIN32) || defined(WIN64)
  ret = -1;
#else
  ret = fcntl(GetSystemCallParam(0),
              GetSystemCallParam(1),
              GetSystemCallParam(2));
#endif
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "ret = " <<  ((PARAMETER_TYPE)ret)
        << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)ret,ret < 0);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_fcntl64() {
  int64_t ret;
#if defined(WIN32) || defined(WIN64)
  ret = -1;
#else
  int fd = GetSystemCallParam(0);
  int cmd = GetSystemCallParam(1);
  
  switch(cmd)
  {
    case F_DUPFD:
//    case F_DUPFD_CLOEXEC:
    case F_GETFD:
    case F_SETFD:
    case F_GETFL:
    case F_SETFL:
      ret = fcntl(fd, cmd);
      break;
    case F_GETLK:
    case F_SETLK:
    case F_SETLKW:
    default:
      ret = -1;
      break;
  }
#endif
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "ret = 0x" << std::hex << ret << std::dec
        << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)ret,ret < 0);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_dup() {
  int ret;
  int oldfd = GetSystemCallParam(0);

  ret = dup(oldfd);
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "oldfd = " << oldfd << ", new fd = " << ((PARAMETER_TYPE)ret)
        << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)ret, ret < 0);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_ioctl() {
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "ret = 0x" << std::hex << ((PARAMETER_TYPE)(-EINVAL)) << std::dec
        << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_ugetrlimit() {
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "ret = 0x" << std::hex << ((PARAMETER_TYPE)(-EINVAL)) << std::dec
        << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_getrlimit() {
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "ret = 0x" << std::hex << ((PARAMETER_TYPE)(-EINVAL)) << std::dec
        << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_setrlimit() {
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "ret = 0x" << std::hex << ((PARAMETER_TYPE)(-EINVAL)) << std::dec
        << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_rt_sigaction() {
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "ret = 0x" << std::hex << ((PARAMETER_TYPE)(-EINVAL)) << std::dec
        << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_getrusage() {
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "ret = 0x" << std::hex << ((PARAMETER_TYPE)(-EINVAL)) << std::dec
        << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_unlink() {
  ADDRESS_TYPE pathnameaddr;
  int pathnamelen;
  char *pathname;
  int ret;
  
  pathnameaddr = GetSystemCallParam(0);
  pathnamelen = StringLength(pathnameaddr);
  pathname = (char *) malloc(pathnamelen + 1);
  ReadMem(pathnameaddr, (uint8_t *) pathname, pathnamelen + 1);
  ret = unlink(pathname);
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "unlink(pathname=\"" << pathname << "\") return " << ret << std::endl
        << EndDebugInfo;

  free(pathname);
  SetSystemCallStatus(ret, (ret < 0));
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_rename() {
  ADDRESS_TYPE oldpathaddr;
  ADDRESS_TYPE newpathaddr;
  int oldpathlen;
  int newpathlen;
  char *oldpath;
  char *newpath;
  int ret;

  oldpathaddr = GetSystemCallParam(0);
  newpathaddr = GetSystemCallParam(1);
  oldpathlen = StringLength(oldpathaddr);
  newpathlen = StringLength(newpathaddr);
  oldpath = (char *) malloc(oldpathlen + 1);
  newpath = (char *) malloc(newpathlen + 1);
  ReadMem(oldpathaddr, (uint8_t *)oldpath, oldpathlen + 1);
  ReadMem(newpathaddr, (uint8_t *)newpath, newpathlen + 1);
  ret = rename(oldpath, newpath);
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "rename(oldpath=\"" << oldpath << "\", newpath=\"" << newpath << "\") return " << ret << std::endl
        << EndDebugInfo;

  free(oldpath);
  free(newpath);
  SetSystemCallStatus(ret, (ret < 0));
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_time() {
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "ret = 0x" << std::hex << ((PARAMETER_TYPE)(-EINVAL)) << std::dec
        << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_socketcall() {
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "ret = 0x" << std::hex << ((PARAMETER_TYPE)(-EINVAL)) << std::dec
        << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_rt_sigprocmask() {
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "ret = 0x" << std::hex << ((PARAMETER_TYPE)(-EINVAL)) << std::dec
        << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_kill() {
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "ret = 0x" << std::hex << ((PARAMETER_TYPE)(-EINVAL)) << std::dec
        << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)0, false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_tkill() {
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "ret = 0x" << std::hex << ((PARAMETER_TYPE)(-EINVAL)) << std::dec
        << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)0, false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_tgkill() {
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "ret = 0x" << std::hex << ((PARAMETER_TYPE)(-EINVAL)) << std::dec
        << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)0, false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_ftruncate() {
  int ret;

  ret = ftruncate(GetSystemCallParam(0), GetSystemCallParam(1));

  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "ret = 0x" << std::hex << ((PARAMETER_TYPE)ret) << std::dec
        << EndDebugInfo;

  SetSystemCallStatus((PARAMETER_TYPE)ret,ret < 0);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_umask() {
  mode_t mask;
  int ret;
  
  mask = GetSystemCallParam(0);
  ret = umask(mask);
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "umask(mask=0" << std::oct << mask << std::dec << ") return " << ret
        << EndDebugInfo;

  SetSystemCallStatus(ret, (ret < 0));
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_gettimeofday() {
  int ret = -1;
  ADDRESS_TYPE tv_addr;
  ADDRESS_TYPE tz_addr;
  tv_addr = GetSystemCallParam(0);
  tz_addr = GetSystemCallParam(1);
  
  if(system_type_.compare("ppc") == 0) {
    struct powerpc_timeval target_tv;
    struct powerpc_timezone target_tz;
  
    ret = GetTimeOfDay(tv_addr ? &target_tv : 0, tz_addr ? &target_tz : 0);
  
    if(ret == 0)
    {
      if(tv_addr) {
        WriteMem(tv_addr, (const uint8_t *) &target_tv, sizeof(target_tv));
      }
      if(tz_addr) {
        WriteMem(tz_addr, (const uint8_t *) &target_tz, sizeof(target_tz));
      }
    }
  }
  else if((system_type_.compare("arm") == 0) ||
      (system_type_.compare("arm-eabi") == 0)) {
    struct arm_timeval target_tv;
    struct arm_timezone target_tz;
  
    ret = GetTimeOfDay(tv_addr ? &target_tv : 0, tz_addr ? &target_tz : 0);
  
    if(ret == 0)
    {
      if(tv_addr) {
        WriteMem(tv_addr, (const uint8_t *) &target_tv, sizeof(target_tv));
      }
      if(tz_addr) {
        WriteMem(tz_addr, (const uint8_t *) &target_tz, sizeof(target_tz));
      }
    }
  }

  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "gettimeofday(tv = 0x" << std::hex << tv_addr << std::dec
        << ", tz = 0x" << std::hex << tz_addr << std::dec << ") return " << ret
        << EndDebugInfo;
  SetSystemCallStatus(ret, (ret < 0));
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_statfs() {
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "ret = 0x" << std::hex << ((PARAMETER_TYPE)(-EINVAL)) << std::dec
        << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_arm_breakpoint() {
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "ret = 0x" << std::hex << ((PARAMETER_TYPE)(-EINVAL)) << std::dec
        << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_arm_cacheflush() {
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "ret = 0x" << std::hex << ((PARAMETER_TYPE)(-EINVAL)) << std::dec
        << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_arm_usr26() {
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "ret = 0x" << std::hex << ((PARAMETER_TYPE)(-EINVAL)) << std::dec
        << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_arm_usr32() {
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "ret = 0x" << std::hex << ((PARAMETER_TYPE)(-EINVAL)) << std::dec
        << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_arm_set_tls() {
  uint32_t r0 = Host2Target(endianness_, GetSystemCallParam(0));
  WriteMem(0xffff0ff0UL, (uint8_t *)&(r0), sizeof(r0));
  if (unlikely(verbose_))
    logger_ << DebugInfo
        << "ret = 0x" << std::hex << ((PARAMETER_TYPE)0) << std::dec
        << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)0, true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
ADDRESS_TYPE Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetMmapBase() const {
  return mmap_base_;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetMmapBase(ADDRESS_TYPE base) {
  mmap_base_ = base;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
ADDRESS_TYPE Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetMmapBrkPoint() const {
  return mmap_brk_point_;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetMmapBrkPoint(
    ADDRESS_TYPE brk_point) {
  mmap_brk_point_ = brk_point;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
ADDRESS_TYPE Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetBrkPoint() const {
  return brk_point_;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetBrkPoint(
    ADDRESS_TYPE brk_point) {
  brk_point_ = brk_point;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
PARAMETER_TYPE Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetSystemCallParam(
    int id) {
  if (system_type_.compare("arm") == 0)
    return ARMGetSystemCallParam(id);
  else if (system_type_.compare("arm-eabi") == 0)
    return ARMEABIGetSystemCallParam(id);
  else
    return PPCGetSystemCallParam(id);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
PARAMETER_TYPE Linux<ADDRESS_TYPE, PARAMETER_TYPE>::ARMGetSystemCallParam(
    int id) {
  PARAMETER_TYPE val = 0;
  // TODO warning if regs_if_ has not been defined
  if (regs_if_ == NULL) return 0;
  if (!GetRegister(id, &val))
    // TODO warning if GetRegister fails
    return val;
  return val;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
PARAMETER_TYPE Linux<ADDRESS_TYPE, PARAMETER_TYPE>::ARMEABIGetSystemCallParam(
    int id) {
  PARAMETER_TYPE val = 0;
  // TODO warning if regs_if_ has not been defined
  if (regs_if_ == NULL) return 0;
  if (!GetRegister(id, &val))
    // TODO warning if GetRegister fails
    return val;
  return val;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
PARAMETER_TYPE Linux<ADDRESS_TYPE, PARAMETER_TYPE>::PPCGetSystemCallParam(
    int id) {
  PARAMETER_TYPE val = 0;
  // TODO warning if regs_if_ has not been defined
  if (regs_if_ == NULL) return 0;
  if (!GetRegister(id+3, &val))
    // TODO warning if GetRegister fails
    return val;
  return val;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetSystemCallStatus(
    int ret, bool error) {
  if (system_type_.compare("arm") == 0)
    ARMSetSystemCallStatus(ret, error);
  else if (system_type_.compare("arm-eabi") == 0)
    ARMEABISetSystemCallStatus(ret, error);
  else
    PPCSetSystemCallStatus(ret, error);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::ARMSetSystemCallStatus(
    int ret, bool error) {
  if (regs_if_ == NULL)
    // TODO warning if regs_if_ not present
    return;
  if (!SetRegister(kARMSyscallStatusReg,
                                        (PARAMETER_TYPE)ret))
    // TODO warning if SetRegister fails
    return;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::ARMEABISetSystemCallStatus(
    int ret, bool error) {
  if (regs_if_ == NULL)
    // TODO warning if register_interface is not present
    return;
  if (!SetRegister(kARMEABISyscallStatusReg,
                                        (PARAMETER_TYPE)ret))
    // TODO warning if SetRegister fails
    return;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::PPCSetSystemCallStatus(
    int ret, bool error) {
  PARAMETER_TYPE val;
  if (regs_if_ == NULL) {
    // TODO warning if regs_if_ is not present
    logger_ << DebugWarning
        << "register interface not available." << EndDebugWarning;
    return;
  }
  if (error) {
    if (!GetRegister(kPPC_cr, &val))
      // TODO warning if GetRegister fails
      return;
    val |= (1 << 28); // CR0[SO] <- 1
    if (!SetRegister(kPPC_cr, val))
      // TODO warnign if SetRegister fails
      return;
  }
  else
  {
    if (!GetRegister(kPPC_cr, &val))
      // TODO warning if GetRegister fails
      return;
    val &= ~(1 << 28); // CR0[SO] <- 0
    if (!SetRegister(kPPC_cr, val))
      // TODO warnign if SetRegister fails
      return;
  }
  val = (PARAMETER_TYPE)ret;
  if (!SetRegister(kPPC_r3, val))
    // TODO warning if SetRegister fails
    return;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int Linux<ADDRESS_TYPE, PARAMETER_TYPE>::StringLength(ADDRESS_TYPE addr) {
  int len = 0;
  char buffer;

  while (1)
  {
    ReadMem(addr, (uint8_t *)&buffer, 1);
    if (buffer == 0) return len;
    len++;
    addr += 1;
  }
}

} // end of namespace unisim
} // end of namespace util
} // end of namespace os
} // end of namespace linux_os

#endif // __UNISIM_UTIL_OS_LINUX_LINUX_TCC__