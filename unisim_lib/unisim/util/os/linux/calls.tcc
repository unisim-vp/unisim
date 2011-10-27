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
 */

#ifndef __UNISIM_UTIL_OS_LINUX_LINUX_TCC__
#define __UNISIM_UTIL_OS_LINUX_LINUX_TCC__

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_exit() {
  string name = "exit";
  int ret;

  ret = GetSystemCallParam(0);
  if (unlikely(verbose))
    logger << DebugInfo
        << "LSC_exit with ret = 0X" << hex << ret << dec
        << LOCATION
        << EndDebugInfo;
  Object::Stop(ret);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_read() {
  string name = "read";
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
    if (ret > 0) WriteMem(buf_addr, buf, ret);
    free(buf);
  } else
    ret = (size_t)-1;

  if (unlikely(verbose))
    logger << DebugInfo
        << "read(fd=" << fd << ", buf=0x" << hex << buf_addr << dec
        << ", count=" << count << ") return " << ret << endl
        << LOCATION
        << EndDebugInfo;

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
    ReadMem(buf_addr, buf, count);
    if ((fd == 1 || fd == 2)) {
      char *tbuf = new char[count + 1];
      memcpy(tbuf, buf, count);
      tbuf[count] = '\0';
      string str(tbuf);
      cout << (str);
      cout << flush;
      ret = count;
      delete[] tbuf;
    } else
      ret = write(fd, buf, count);
    free(buf);
  } else
    ret = (size_t)-1;

  if (unlikely(verbose))
    logger << DebugInfo
        << "write(fd=" << fd << ", buf=0x" << hex << buf_addr << dec
        << ", count=" << count << ") return " << ret << endl
        << LOCATION
        << EndDebugInfo;
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
  ReadMem(addr, pathname, pathnamelen + 1);
  flags = GetSystemCallParam(1);
  mode = GetSystemCallParam(2);
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
  host_mode = mode & S_IRWXU; // Windows doesn't have bits for group and others
#else
  host_mode = mode; // other UNIX systems should have the same bit encoding for protection
#endif
  if (strcmp(pathname, osrelease_filename) == 0) {
    {
      std::ofstream fake_file(fake_osrelease_filename);
      fake_file << utsname_release << std::endl;
    }
    ret = open(fake_osrelease_filename, host_flags, host_mode);
  } else {
    ret = open(pathname, host_flags, host_mode);
  }
#endif

  if (unlikely(verbose))
    logger << DebugInfo
        << "open(pathname=\"" << pathname << "\", flags=0x" << hex << flags
        << ", mode=0x" << mode << dec << ") return " << ret << endl
        << LOCATION
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
  if (unlikely(verbose))
    logger << DebugInfo
        << "close(fd=" << fd << ") return " << ret << endl
        << LOCATION
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
  if (unlikely(verbose))
    logger << DebugInfo
        << "lseek(fildes=" << fildes << ", offset=" << offset
        << ", whence=" << whence << ") return " << ret << endl
        << LOCATION
        << EndDebugInfo;

  if (ret == (off_t)-1)
    SetSystemCallStatus(errno, true);
  else
    SetSystemCallStatus(ret, false);
}


template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_getpid() {
  pid_t ret;

  ret = (pid_t) 1000;
  if (unlikely(verbose))
    logger << DebugInfo
        << "getpid() return " << ret << endl
        << LOCATION
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
  if (unlikely(verbose))
    logger << DebugInfo
        << "getuid() return " << ret << endl
        << LOCATION
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
  ReadMem(addr, pathname, pathnamelen + 1);
  mode = GetSystemCallParam(1);
#if defined(WIN32) || defined(WIN64)
  int win_mode = 0;
  win_mode = mode & S_IRWXU; // Windows doesn't have bits for group and others
  ret = access(pathname, win_mode);
#else
  ret = access(pathname, mode);
#endif
  if (unlikely(verbose))
    logger << DebugInfo
        << "access(pathname=\"" << pathname
        << "\", mode=0x" << hex << mode << dec << ") return " << ret << endl
        << LOCATION
        << EndDebugInfo;
  free(pathname);
  SetSystemCallStatus(ret, ret < 0);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_times() {
  int ret;
  ADDRESS_TYPE buf_addr;
  buf_addr = GetSystemCallParam(0);

  if ( (system == "arm") || (system == "arm-eabi") ) {
    struct arm_tms_t target_tms;
    ret = Times(&target_tms);

    if (ret >= 0) {
      WriteMem(buf_addr, &target_tms, sizeof(target_tms));
    }
  } else if (system == "powerpc") {
    struct powerpc_tms_t target_tms;
    ret = Times(&target_tms);

    if (ret >= 0) {
      WriteMem(buf_addr, &target_tms, sizeof(target_tms));
    }
  } else ret = -1;

  if (unlikely(verbose))
    logger << DebugInfo
        << "times(buf=0x" << hex << buf_addr << dec << ") return " << ret
        << endl
        << LOCATION
        << EndDebugInfo;
  SetSystemCallStatus(ret, ret != -1);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_brk() {
  ADDRESS_TYPE new_brk_point;

  new_brk_point = GetSystemCallParam(0);

  if (new_brk_point > GetBrkPoint())
    SetBrkPoint(new_brk_point);

  if (unlikely(verbose))
    logger << DebugInfo
        << "brk(new_brk_point=0x" << hex << new_brk_point
        << ") return 0x" << GetBrkPoint() << dec << endl
        << LOCATION
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
  if (unlikely(verbose))
    logger << DebugInfo
        << "getgid() return " << ret << endl
        << LOCATION
        << EndDebugInfo;
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
  if (unlikely(verbose))
    logger << DebugInfo
        << "geteuid() return " << ret << endl
        << LOCATION
        << EndDebugInfo;
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
  if (unlikely(verbose))
    logger << DebugInfo
        << "getegid() return " << ret << endl
        << LOCATION
        << EndDebugInfo;
  SetSystemCallStatus(ret,false);
}


template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_munmap() {
  size_t u = (size_t)(GetSystemCallParam(1));

  if (GetMmapBrkPoint() - u > GetMmapBrkPoint()) {
    SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
    if (unlikely(verbose))
      logger << DebugInfo
          << "size = " << u
          << ", ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec
          << endl
          << LOCATION << EndDebugInfo;
    return;
  }

  if (GetMmapBrkPoint() - u < GetMmapBase())
    u = (size_t)(GetMmapBrkPoint() - GetMmapBase());

  if (GetMmapBrkPoint() - u >= GetMmapBrkPoint()) {
    SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
    if (unlikely(verbose))
      logger << DebugInfo
          << "size = " << u
          << ", ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec
          << endl
          << LOCATION << EndDebugInfo;
    return;
  }

  SetSystemCallStatus((PARAMETER_TYPE)0,false);
  if (unlikely(verbose))
    logger << DebugInfo
        << "size = " << u
        << ", ret = 0x" << hex << 0 << dec
        << endl
        << LOCATION << EndDebugInfo;
  SetMmapBrkPoint(GetMmapBrkPoint() - u);
}


template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_stat() {
  logger << DebugWarning
      << "TODO" << endl
      << LOCATION
      << EndDebugWarning;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
int Linux<ADDRESS_TYPE, PARAMETER_TYPE>::Stat(
    int fd,
    struct powerpc_stat_t *target_stat) {
  int ret;
  struct stat host_stat;
  ret = fstat(fd, &host_stat);
  if (ret < 0) return ret;

#if defined(__x86_64) || defined(__amd64) || defined(__LP64__) || defined(_LP64) || defined(__amd64__)
  // 64-bit host
  target_stat->st_dev = Host2Target(endianess, (uint64_t) host_stat.st_dev);
  target_stat->st_ino = Host2Target(endianess, (uint64_t) host_stat.st_ino);
  target_stat->st_mode = Host2Target(endianess, (uint32_t) host_stat.st_mode);
  target_stat->st_nlink = Host2Target(endianess, (uint64_t) host_stat.st_nlink);
  target_stat->st_uid = Host2Target(endianess, (uint32_t) host_stat.st_uid);
  target_stat->st_gid = Host2Target(endianess, (uint32_t) host_stat.st_gid);
  target_stat->st_rdev = Host2Target(endianess, (int64_t) host_stat.st_rdev);
  target_stat->st_size = Host2Target(endianess, (int64_t) host_stat.st_size);
#if defined(WIN64) // Windows x64
  target_stat->st_blksize = Host2Target((int32_t) 512);
  target_stat->st_blocks = 
      Host2Target((int64_t)((host_stat.st_size + 511) / 512));
  target_stat->st_atim.tv_sec =
      Host2Target(endianess, (int64_t) host_stat.st_atim);
  target_stat->st_atim.tv_nsec = 0;
  target_stat->st_mtim.tv_sec = 
      Host2Target(endianess, (int64_t) host_stat.st_mtim);
  target_stat->st_mtim.tv_nsec = 0;
  target_stat->st_ctim.tv_sec = 
      Host2Target(endianess, (int64_t) host_stat.st_ctim);
  target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) // Linux x64
  target_stat->st_blksize = 
      Host2Target(endianess, (int64_t) host_stat.st_blksize);
  target_stat->st_blocks = 
      Host2Target(endianess, (int64_t) host_stat.st_blocks);
  target_stat->st_atim.tv_sec = 
      Host2Target(endianess, (int64_t) host_stat.st_atim.tv_sec);
  target_stat->st_atim.tv_nsec = 
      Host2Target(endianess, (int64_t) host_stat.st_atim.tv_nsec);
  target_stat->st_mtim.tv_sec = 
      Host2Target(endianess, (int64_t) host_stat.st_mtim.tv_sec);
  target_stat->st_mtim.tv_nsec = 
      Host2Target(endianess, (int64_t) host_stat.st_mtim.tv_nsec);
  target_stat->st_ctim.tv_sec = 
      Host2Target(endianess, (int64_t) host_stat.st_ctim.tv_sec);
  target_stat->st_ctim.tv_nsec = 
      Host2Target(endianess, (int64_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // darwin PPC64/x86_64
  target_stat->st_blksize = 
      Host2Target(endianess, (int64_t) host_stat.st_blksize);
  target_stat->st_blocks = 
      Host2Target(endianess, (int64_t) host_stat.st_blocks);
  target_stat->st_atim.tv_sec = 
      Host2Target(endianess, (int64_t) host_stat.st_atimespec.tv_sec);
  target_stat->st_atim.tv_nsec = 
      Host2Target(endianess, (int64_t) host_stat.st_atimespec.tv_nsec);
  target_stat->st_mtim.tv_sec = 
      Host2Target(endianess, (int64_t) host_stat.st_mtimespec.tv_sec);
  target_stat->st_mtim.tv_nsec = 
      Host2Target(endianess, (int64_t) host_stat.st_mtimespec.tv_nsec);
  target_stat->st_ctim.tv_sec = 
      Host2Target(endianess, (int64_t) host_stat.st_ctimespec.tv_sec);
  target_stat->st_ctim.tv_nsec = 
      Host2Target(endianess, (int64_t) host_stat.st_ctimespec.tv_nsec);
#endif

#else
  // 32-bit host
  target_stat->st_dev = Host2Target(endianess, (uint64_t) host_stat.st_dev);
  target_stat->st_ino = Host2Target(endianess, (uint32_t) host_stat.st_ino);
  target_stat->st_mode = Host2Target(endianess, (uint32_t) host_stat.st_mode);
  target_stat->st_nlink = Host2Target(endianess, (uint32_t) host_stat.st_nlink);
  target_stat->st_uid = Host2Target(endianess, (uint32_t) host_stat.st_uid);
  target_stat->st_gid = Host2Target(endianess, (uint32_t) host_stat.st_gid);
  target_stat->st_rdev = Host2Target(endianess, (int64_t) host_stat.st_rdev);
  target_stat->st_size = Host2Target(endianess, (int64_t) host_stat.st_size);
#if defined(WIN32) // Windows 32
  target_stat->st_blksize = Host2Target(endianess, (int32_t) 512);
  target_stat->st_blocks = 
      Host2Target(endianess, (int64_t)((host_stat.st_size + 511) / 512));
  target_stat->st_atim.tv_sec = 
      Host2Target(endianess, (int32_t) host_stat.st_atime);
  target_stat->st_atim.tv_nsec = 0;
  target_stat->st_mtim.tv_sec = 
      Host2Target(endianess, (int32_t) host_stat.st_mtime);
  target_stat->st_mtim.tv_nsec = 0;
  target_stat->st_ctim.tv_sec =
      Host2Target(endianess, (int32_t) host_stat.st_ctime);
  target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) // Linux 32
  target_stat->st_blksize = 
      Host2Target(endianess, (int32_t) host_stat.st_blksize);
  target_stat->st_blocks = 
      Host2Target(endianess, (int64_t) host_stat.st_blocks);
  target_stat->st_atim.tv_sec = 
      Host2Target(endianess, (int32_t) host_stat.st_atim.tv_sec);
  target_stat->st_atim.tv_nsec = 
      Host2Target(endianess, (int32_t) host_stat.st_atim.tv_nsec);
  target_stat->st_mtim.tv_sec = 
      Host2Target(endianess, (int32_t) host_stat.st_mtim.tv_sec);
  target_stat->st_mtim.tv_nsec = 
      Host2Target(endianess, (int32_t) host_stat.st_mtim.tv_nsec);
  target_stat->st_ctim.tv_sec = 
      Host2Target(endianess, (int32_t) host_stat.st_ctim.tv_sec);
  target_stat->st_ctim.tv_nsec = 
      Host2Target(endianess, (int32_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // Darwin PPC32/x86
  target_stat->st_blksize = 
      Host2Target(endianess, (int32_t) host_stat.st_blksize);
  target_stat->st_blocks =
      Host2Target(endianess, (int64_t) host_stat.st_blocks);
  target_stat->st_atim.tv_sec = 
      Host2Target(endianess, (int32_t) host_stat.st_atimespec.tv_sec);
  target_stat->st_atim.tv_nsec = 
      Host2Target(endianess, (int32_t) host_stat.st_atimespec.tv_nsec);
  target_stat->st_mtim.tv_sec =
      Host2Target(endianess, (int32_t) host_stat.st_mtimespec.tv_sec);
  target_stat->st_mtim.tv_nsec = 
      Host2Target(endianess, (int32_t) host_stat.st_mtimespec.tv_nsec);
  target_stat->st_ctim.tv_sec = 
      Host2Target(endianess, (int32_t) host_stat.st_ctimespec.tv_sec);
  target_stat->st_ctim.tv_nsec = 
      Host2Target(endianess, (int32_t) host_stat.st_ctimespec.tv_nsec);
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
    int fd, struct powerpc_stat64_t *target_stat) {
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
  target_stat->st_dev = Host2Target(endianess, (uint64_t) host_stat.st_dev);
  target_stat->st_ino = Host2Target(endianess, (uint64_t) host_stat.st_ino);
  target_stat->st_mode = Host2Target(endianess, (uint32_t) host_stat.st_mode);
  target_stat->st_nlink = Host2Target(endianess, (uint64_t) host_stat.st_nlink);
  target_stat->st_uid = Host2Target(endianess, (uint32_t) host_stat.st_uid);
  target_stat->st_gid = Host2Target(endianess, (uint32_t) host_stat.st_gid);
  target_stat->st_rdev = Host2Target(endianess, (int64_t) host_stat.st_rdev);
  target_stat->st_size = Host2Target(endianess, (int64_t) host_stat.st_size);
#if defined(WIN64) // Windows x64
  target_stat->st_blksize = Host2Target(endianess, (int32_t) 512);
  target_stat->st_blocks = 
      Host2Target(endianess, (int64_t)((host_stat.st_size + 511) / 512));
  target_stat->st_atim.tv_sec = 
      Host2Target(endianess, (int64_t) host_stat.st_atim);
  target_stat->st_atim.tv_nsec = 0;
  target_stat->st_mtim.tv_sec =
      Host2Target(endianess, (int64_t) host_stat.st_mtim);
  target_stat->st_mtim.tv_nsec = 0;
  target_stat->st_ctim.tv_sec = 
      Host2Target(endianess, (int64_t) host_stat.st_ctim);
  target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) // Linux x64
  target_stat->st_blksize = 
      Host2Target(endianess, (int32_t) host_stat.st_blksize);
  target_stat->st_blocks = 
      Host2Target(endianess, (int64_t) host_stat.st_blocks);
  target_stat->st_atim.tv_sec =
      Host2Target(endianess, (int64_t) host_stat.st_atim.tv_sec);
  target_stat->st_atim.tv_nsec = 
      Host2Target(endianess, (int64_t) host_stat.st_atim.tv_nsec);
  target_stat->st_mtim.tv_sec = 
      Host2Target(endianess, (int64_t) host_stat.st_mtim.tv_sec);
  target_stat->st_mtim.tv_nsec = 
      Host2Target(endianess, (int64_t) host_stat.st_mtim.tv_nsec);
  target_stat->st_ctim.tv_sec = 
      Host2Target(endianess, (int64_t) host_stat.st_ctim.tv_sec);
  target_stat->st_ctim.tv_nsec =
      Host2Target(endianess, (int64_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // Darwin PPC64/x86_64
  target_stat->st_blksize = 
      Host2Target(endianess, (int32_t) host_stat.st_blksize);
  target_stat->st_blocks = 
      Host2Target(endianess, (int64_t) host_stat.st_blocks);
  target_stat->st_atim.tv_sec = 
      Host2Target(endianess, (int64_t) host_stat.st_atimespec.tv_sec);
  target_stat->st_atim.tv_nsec = 
      Host2Target(endianess, (int64_t) host_stat.st_atimespec.tv_nsec);
  target_stat->st_mtim.tv_sec = 
      Host2Target(endianess, (int64_t) host_stat.st_mtimespec.tv_sec);
  target_stat->st_mtim.tv_nsec = 
      Host2Target(endianess, (int64_t) host_stat.st_mtimespec.tv_nsec);
  target_stat->st_ctim.tv_sec = 
      Host2Target(endianess, (int64_t) host_stat.st_ctimespec.tv_sec);
  target_stat->st_ctim.tv_nsec = 
      Host2Target(endianess, (int64_t) host_stat.st_ctimespec.tv_nsec);
#endif

#else
  // 32-bit host
  target_stat->st_dev = Host2Target(endianess, (uint64_t) host_stat.st_dev);
  target_stat->st_ino = Host2Target(endianess, (uint32_t) host_stat.st_ino);
  target_stat->st_mode = Host2Target(endianess, (uint32_t) host_stat.st_mode);
  target_stat->st_nlink = Host2Target(endianess, (uint32_t) host_stat.st_nlink);
  target_stat->st_uid = Host2Target(endianess, (uint32_t) host_stat.st_uid);
  target_stat->st_gid = Host2Target(endianess, (uint32_t) host_stat.st_gid);
  target_stat->st_rdev = Host2Target(endianess, (int64_t) host_stat.st_rdev);
  target_stat->st_size = Host2Target(endianess, (int64_t) host_stat.st_size);
#if defined(WIN32) // Windows 32
  target_stat->st_blksize = Host2Target(endianess, (int32_t) 512);
  target_stat->st_blocks = 
      Host2Target(endianess, (int64_t)((host_stat.st_size + 511) / 512));
  target_stat->st_atim.tv_sec = 
      Host2Target(endianess, (int32_t) host_stat.st_atime);
  target_stat->st_atim.tv_nsec = 0;
  target_stat->st_mtim.tv_sec = 
      Host2Target(endianess, (int32_t) host_stat.st_mtime);
  target_stat->st_mtim.tv_nsec = 0;
  target_stat->st_ctim.tv_sec = 
      Host2Target(endianess, (int32_t) host_stat.st_ctime);
  target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) // Linux 32
  target_stat->st_blksize = 
      Host2Target(endianess, (int32_t) host_stat.st_blksize);
  target_stat->st_blocks = 
      Host2Target(endianess, (int64_t) host_stat.st_blocks);
  target_stat->st_atim.tv_sec = 
      Host2Target(endianess, (int32_t) host_stat.st_atim.tv_sec);
  target_stat->st_atim.tv_nsec = 
      Host2Target(endianess, (int32_t) host_stat.st_atim.tv_nsec);
  target_stat->st_mtim.tv_sec = 
      Host2Target(endianess, (int32_t) host_stat.st_mtim.tv_sec);
  target_stat->st_mtim.tv_nsec = 
      Host2Target(endianess, (int32_t) host_stat.st_mtim.tv_nsec);
  target_stat->st_ctim.tv_sec = 
      Host2Target(endianess, (int32_t) host_stat.st_ctim.tv_sec);
  target_stat->st_ctim.tv_nsec = 
      Host2Target(endianess, (int32_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // Darwin PPC32/x86
  target_stat->st_blksize = 
      Host2Target(endianess, (int32_t) host_stat.st_blksize);
  target_stat->st_blocks = 
      Host2Target(endianess, (int64_t) host_stat.st_blocks);
  target_stat->st_atim.tv_sec = 
      Host2Target(endianess, (int32_t) host_stat.st_atimespec.tv_sec);
  target_stat->st_atim.tv_nsec = 
      Host2Target(endianess, (int32_t) host_stat.st_atimespec.tv_nsec);
  target_stat->st_mtim.tv_sec = 
      Host2Target(endianess, (int32_t) host_stat.st_mtimespec.tv_sec);
  target_stat->st_mtim.tv_nsec = 
      Host2Target(endianess, (int32_t) host_stat.st_mtimespec.tv_nsec);
  target_stat->st_ctim.tv_sec = 
      Host2Target(endianess, (int32_t) host_stat.st_ctimespec.tv_sec);
  target_stat->st_ctim.tv_nsec = 
      Host2Target(endianess, (int32_t) host_stat.st_ctimespec.tv_nsec);
#endif

#endif
  target_stat->__pad2 = 0;
  target_stat->__unused4 = 0;
  target_stat->__unused5 = 0;
  return ret;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
int Linux<ADDRESS_TYPE, PARAMETER_TYPE>::Stat64(
    int fd, arm_stat64_t *target_stat) {
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
  target_stat->st_dev = Host2Target(endianess, (uint64_t) host_stat.st_dev);
  target_stat->__st_ino = Host2Target(endianess, (uint32_t) host_stat.st_ino);
  target_stat->st_ino = Host2Target(endianess, (uint64_t) host_stat.st_ino);
  target_stat->st_mode = Host2Target(endianess, (uint32_t) host_stat.st_mode);
  target_stat->st_nlink = Host2Target(endianess, (uint64_t) host_stat.st_nlink);
  target_stat->st_uid = Host2Target(endianess, (uint32_t) host_stat.st_uid);
  target_stat->st_gid = Host2Target(endianess, (uint32_t) host_stat.st_gid);
  target_stat->st_rdev = Host2Target(endianess, (int64_t) host_stat.st_rdev);
  target_stat->st_size = Host2Target(endianess, (int64_t) host_stat.st_size);
#if defined(WIN64) // Windows x64
  target_stat->st_blksize = Host2Target(endianess, (int32_t) 512);
  target_stat->st_blocks = 
      Host2Target(endianess, (int64_t)((host_stat.st_size + 511) / 512));
  target_stat->st_atim.tv_sec = 
      Host2Target(endianess, (int64_t) host_stat.st_atim);
  target_stat->st_atim.tv_nsec = 0;
  target_stat->st_mtim.tv_sec = 
      Host2Target(endianess, (int64_t) host_stat.st_mtim);
  target_stat->st_mtim.tv_nsec = 0;
  target_stat->st_ctim.tv_sec = 
      Host2Target(endianess, (int64_t) host_stat.st_ctim);
  target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) // Linux x64
  target_stat->st_blksize = 
      Host2Target(endianess, (int32_t) host_stat.st_blksize);
  target_stat->st_blocks = 
      Host2Target(endianess, (int64_t) host_stat.st_blocks);
  target_stat->st_atim.tv_sec = 
      Host2Target(endianess, (int64_t) host_stat.st_atim.tv_sec);
  target_stat->st_atim.tv_nsec = 
      Host2Target(endianess, (int64_t) host_stat.st_atim.tv_nsec);
  target_stat->st_mtim.tv_sec = 
      Host2Target(endianess, (int64_t) host_stat.st_mtim.tv_sec);
  target_stat->st_mtim.tv_nsec = 
      Host2Target(endianess, (int64_t) host_stat.st_mtim.tv_nsec);
  target_stat->st_ctim.tv_sec = 
      Host2Target(endianess, (int64_t) host_stat.st_ctim.tv_sec);
  target_stat->st_ctim.tv_nsec = 
      Host2Target(endianess, (int64_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // darwin PPC64/x86_64
  target_stat->st_blksize = 
      Host2Target(endianess, (int32_t) host_stat.st_blksize);
  target_stat->st_blocks = 
      Host2Target(endianess, (int64_t) host_stat.st_blocks);
  target_stat->st_atim.tv_sec = 
      Host2Target(endianess, (int64_t) host_stat.st_atimespec.tv_sec);
  target_stat->st_atim.tv_nsec = 
      Host2Target(endianess, (int64_t) host_stat.st_atimespec.tv_nsec);
  target_stat->st_mtim.tv_sec = 
      Host2Target(endianess, (int64_t) host_stat.st_mtimespec.tv_sec);
  target_stat->st_mtim.tv_nsec = 
      Host2Target(endianess, (int64_t) host_stat.st_mtimespec.tv_nsec);
  target_stat->st_ctim.tv_sec = 
      Host2Target(endianess, (int64_t) host_stat.st_ctimespec.tv_sec);
  target_stat->st_ctim.tv_nsec = 
      Host2Target(endianess, (int64_t) host_stat.st_ctimespec.tv_nsec);
#endif

#else
  // 32-bit host
  target_stat->st_dev = Host2Target(endianess, (uint64_t) host_stat.st_dev);
  target_stat->__st_ino = Host2Target(endianess, (uint32_t) host_stat.st_ino);
  target_stat->st_ino = Host2Target(endianess, (uint32_t) host_stat.st_ino);
  target_stat->st_mode = Host2Target(endianess, (uint32_t) host_stat.st_mode);
  target_stat->st_nlink = Host2Target(endianess, (uint32_t) host_stat.st_nlink);
  target_stat->st_uid = Host2Target(endianess, (uint32_t) host_stat.st_uid);
  target_stat->st_gid = Host2Target(endianess, (uint32_t) host_stat.st_gid);
  target_stat->st_rdev = Host2Target(endianess, (int64_t) host_stat.st_rdev);
  target_stat->st_size = Host2Target(endianess, (int64_t) host_stat.st_size);
#if defined(WIN32) // Windows 32
  target_stat->st_blksize = Host2Target(endianess, (int32_t) 512);
  target_stat->st_blocks = 
      Host2Target(endianess, (int64_t)((host_stat.st_size + 511) / 512));
  target_stat->st_atim.tv_sec = 
      Host2Target(endianess, (int32_t) host_stat.st_atime);
  target_stat->st_atim.tv_nsec = 0;
  target_stat->st_mtim.tv_sec = 
      Host2Target(endianess, (int32_t) host_stat.st_mtime);
  target_stat->st_mtim.tv_nsec = 0;
  target_stat->st_ctim.tv_sec = 
      Host2Target(endianess, (int32_t) host_stat.st_ctime);
  target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) // Linux 32
  target_stat->st_blksize = Host2Target(endianess, (int32_t)
                                        host_stat.st_blksize);
  target_stat->st_blocks = Host2Target(endianess, (int64_t)
                                       host_stat.st_blocks);
  target_stat->st_atim.tv_sec = Host2Target(endianess, (int32_t)
                                            host_stat.st_atim.tv_sec);
  target_stat->st_atim.tv_nsec = Host2Target(endianess, (int32_t)
                                             host_stat.st_atim.tv_nsec);
  target_stat->st_mtim.tv_sec = Host2Target(endianess, (int32_t)
                                            host_stat.st_mtim.tv_sec);
  target_stat->st_mtim.tv_nsec = Host2Target(endianess, (int32_t)
                                             host_stat.st_mtim.tv_nsec);
  target_stat->st_ctim.tv_sec = Host2Target(endianess, (int32_t)
                                            host_stat.st_ctim.tv_sec);
  target_stat->st_ctim.tv_nsec = Host2Target(endianess, (int32_t)
                                             host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // Darwin PPC32/x86
  target_stat->st_blksize = Host2Target(endianess, (int32_t)
                                        host_stat.st_blksize);
  target_stat->st_blocks = Host2Target(endianess, (int64_t)
                                       host_stat.st_blocks);
  target_stat->st_atim.tv_sec = Host2Target(endianess, (int32_t)
                                            host_stat.st_atimespec.tv_sec);
  target_stat->st_atim.tv_nsec = Host2Target(endianess, (int32_t)
                                             host_stat.st_atimespec.tv_nsec);
  target_stat->st_mtim.tv_sec = Host2Target(endianess, (int32_t)
                                            host_stat.st_mtimespec.tv_sec);
  target_stat->st_mtim.tv_nsec = Host2Target(endianess, (int32_t)
                                             host_stat.st_mtimespec.tv_nsec);
  target_stat->st_ctim.tv_sec = Host2Target(endianess, (int32_t)
                                            host_stat.st_ctimespec.tv_sec);
  target_stat->st_ctim.tv_nsec = Host2Target(endianess, (int32_t)
                                             host_stat.st_ctimespec.tv_nsec);
#endif

#endif
  target_stat->__pad1 = 0;
  target_stat->__pad2 = 0;
  return ret;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
int Linux<ADDRESS_TYPE, PARAMETER_TYPE>::Times(
    struct powerpc_tms_t *target_tms) {
  int ret;
#ifdef WIN32
  FILETIME ftCreationTime;
  FILETIME ftExitTime;
  FILETIME ftKernelTime;
  FILETIME ftUserTime;

  if (GetProcessTimes(GetCurrentProcess(), &ftCreationTime, &ftExitTime,
                      &ftKernelTime, &ftUserTime)) return -1;

  target_tms->tms_utime = Host2Target(endianess, (uint32_t)
                                      ftUserTime.dwLowDateTime);
  target_tms->tms_stime = Host2Target(endianess, (uint32_t)
                                      ftKernelTime.dwLowDateTime);
  target_tms->tms_cutime = 0;   // User CPU time of dead children
  target_tms->tms_cstime = 0;   // System CPU time of dead children
#else
  struct tms host_tms;

  ret = (int) times(&host_tms);
  target_tms->tms_utime = Host2Target(endianess, (int32_t) host_tms.tms_utime);
  target_tms->tms_stime = Host2Target(endianess, (int32_t) host_tms.tms_stime);
  target_tms->tms_cutime = Host2Target(endianess, (int32_t)
                                       host_tms.tms_cutime);
  target_tms->tms_cstime = Host2Target(endianess, (int32_t)
                                       host_tms.tms_cstime);
#endif
  return ret;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
int Linux<ADDRESS_TYPE, PARAMETER_TYPE>::Times(struct arm_tms_t *target_tms) {
  int ret;
#ifdef WIN32
  FILETIME ftCreationTime;
  FILETIME ftExitTime;
  FILETIME ftKernelTime;
  FILETIME ftUserTime;

  if (GetProcessTimes(GetCurrentProcess(), &ftCreationTime, &ftExitTime,
                      &ftKernelTime, &ftUserTime)) return -1;

  target_tms->tms_utime = Host2Target(endianess, (uint32_t)
                                      ftUserTime.dwLowDateTime);
  target_tms->tms_stime = Host2Target(endianess, (uint32_t)
                                      ftKernelTime.dwLowDateTime);
  target_tms->tms_cutime = 0;   // User CPU time of dead children
  target_tms->tms_cstime = 0;   // System CPU time of dead children
#else
  struct tms host_tms;

  ret = (int) times(&host_tms);
  target_tms->tms_utime = Host2Target(endianess, (int32_t) host_tms.tms_utime);
  target_tms->tms_stime = Host2Target(endianess, (int32_t) host_tms.tms_stime);
  target_tms->tms_cutime = Host2Target(endianess, (int32_t)
                                       host_tms.tms_cutime);
  target_tms->tms_cstime = Host2Target(endianess, (int32_t)
                                       host_tms.tms_cstime);
#endif
  return ret;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_fstat() {
  int ret;
  int fd;
  ADDRESS_TYPE buf_address;

  fd = GetSystemCallParam(0);
  buf_address = GetSystemCallParam(1);
  if ( (system == "arm") || (system == "arm-eabi") )
    ret = -1;
  else if (system == "powerpc") {
    struct powerpc_stat_t target_stat;
    ret = Stat(fd, &target_stat);
    WriteMem(buf_address, &target_stat, sizeof(target_stat));
  }
  else ret = -1;

  if (unlikely(verbose))
    logger << DebugInfo
        << "stat(fd=" << fd
        << ", buf_addr=0x" << hex << buf_address << dec
        << ") return " << ret << endl
        << LOCATION
        << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)ret,ret < 0);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_uname() {
  int ret;
  ADDRESS_TYPE buf_addr = GetSystemCallParam(0);
  struct utsname_t value;
  memset(&value, 0, sizeof(value));
  memcpy(&(value.sysname),
         utsname_sysname.c_str(), utsname_sysname.length() + 1);
  memcpy(&(value.nodename),
         utsname_nodename.c_str(), utsname_nodename.length() + 1);
  memcpy(&(value.release),
         utsname_release.c_str(), utsname_release.length() + 1);
  memcpy(&(value.version),
         utsname_version.c_str(), utsname_version.length() + 1);
  memcpy(&(value.machine),
         utsname_machine.c_str(), utsname_machine.length() + 1);
  WriteMem(buf_addr, &value, sizeof(value));
  ret = 0;
  SetSystemCallStatus((PARAMETER_TYPE) ret, ret < 0);
}


template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_llseek() {
  int fd;
  uint32_t offset_high;
  uint32_t offset_low;
  PARAMETER_TYPE result_addr;
  uint64_t lseek_ret64;
  off_t lseek_ret;
  int whence;

  fd = GetSystemCallParam(0);
  offset_high = GetSystemCallParam(1);
  offset_low = GetSystemCallParam(2);
  result_addr = GetSystemCallParam(3);
  whence = GetSystemCallParam(4);
  if (unlikely(verbose))
    logger << DebugInfo
        << "llseek(fd=" << fd
        << ", offset=" << (((int64_t) offset_high << 32) | (int64_t) offset_low)
        << ", result_addr=0x" << hex << result_addr << dec
        << ", whence=" << whence << ")" << endl
        << LOCATION << EndDebugInfo;
  if (offset_high == 0) {
    lseek_ret = lseek(fd, offset_low, whence);
    if (lseek_ret >= 0) {
      lseek_ret64 = lseek_ret;
      if (Swap())
        PerformSwap(&lseek_ret64, sizeof(lseek_ret64));
      WriteMem(result_addr, &lseek_ret64, sizeof(lseek_ret64));
      SetSystemCallStatus((PARAMETER_TYPE)lseek_ret, false);
    } else
      SetSystemCallStatus((PARAMETER_TYPE)errno, true);
  } else
    SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_writev() {
  if (unlikely(verbose))
    logger << DebugInfo
        << "ret = 0x"
        << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec << endl
        << LOCATION << EndDebugInfo;

  SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_mmap() {
  SetSystemCallStatus(-1,true); return;
  if (GetSystemCallParam(3) == 0x32) { /* MAP_PRIVATE | MAP_ANONYMOUS */
    SetSystemCallStatus(GetSystemCallParam(0),false);
    if (unlikely(verbose))
      logger << DebugInfo << "map_type = 0x" << hex << GetSystemCallParam(3) <<
          dec << ", size = " << GetSystemCallParam(1) << ", ret = 0x" << hex <<
          ((PARAMETER_TYPE)GetSystemCallParam(0)) << dec << endl << LOCATION <<
          EndDebugInfo;
    return;
  }

  if ((GetSystemCallParam(3)&0xFF) != 0x22) { /* MAP_PRIVATE | MAP_ANONYMOUS */
    SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
    if (unlikely(verbose))
      logger << DebugInfo
          << "map_type = 0x" << hex << GetSystemCallParam(3) << dec
          << ", size = " << GetSystemCallParam(1)
          << ", ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec
          << endl << LOCATION << EndDebugInfo;
    return;
  }
  SetSystemCallStatus(GetMmapBrkPoint(),false);
  if (unlikely(verbose))
    logger << DebugInfo
        << "map_type = 0x" << hex << GetSystemCallParam(3) << dec
        << ", size = " << GetSystemCallParam(1)
        << ", ret = 0x" << hex << GetMmapBrkPoint() << dec
        << endl << LOCATION << EndDebugInfo;
  SetMmapBrkPoint(GetMmapBrkPoint() + GetSystemCallParam(1));
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_mmap2() {
  if (GetSystemCallParam(3) != 0x22) { /* MAP_PRIVATE | MAP_ANONYMOUS */
    SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
    if (unlikely(verbose))
      logger << DebugInfo
          << "map_type = 0x" << hex << GetSystemCallParam(3) << dec
          << ", size = " << GetSystemCallParam(1)
          << ", ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec
          << endl << LOCATION << EndDebugInfo;
    return;
  }

  if (GetMmapBrkPoint() + GetSystemCallParam(1) > GetSystemCallParam(1)) {
    SetSystemCallStatus(GetMmapBrkPoint(),false);
    if (unlikely(verbose))
      logger << DebugInfo
          << "map_type = 0x" << hex << GetSystemCallParam(3) << dec
          << ", size = " << GetSystemCallParam(1)
          << ", ret = 0x" << hex << GetMmapBrkPoint() << dec
          << endl << LOCATION << EndDebugInfo;
    SetMmapBrkPoint(GetMmapBrkPoint() + GetSystemCallParam(1));
  } else {
    if (unlikely(verbose))
      logger << DebugInfo
          << "map_type = 0x" << hex << GetSystemCallParam(3) << dec
          << ", size = " << GetSystemCallParam(1)
          << ", ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec
          << endl << LOCATION << EndDebugInfo;
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
  if ( (system == "arm") || (system == "arm-eabi") ) {
    struct arm_stat64_t target_stat;
    ret = Stat64(fd, &target_stat);
    WriteMem(buf_address, &target_stat, sizeof(target_stat));
  } else if (system == "powerpc") {
    struct powerpc_stat64_t target_stat;
    ret = Stat64(fd, &target_stat);
    WriteMem(buf_address, &target_stat, sizeof(target_stat));
  } else ret = -1;
  if (unlikely(verbose))
    logger << DebugInfo
        << "fd = " << fd << ", buf_address = 0x" << hex << buf_address << dec
        << ", ret = 0x" << hex << ret << dec
        << endl << LOCATION << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)ret,ret < 0);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_fstat64() {
  int ret;
  ADDRESS_TYPE buf_address;
  int fd;

  fd = GetSystemCallParam(0);
  buf_address = GetSystemCallParam(1);
  if ( (system == "arm") || (system == "arm-eabi") ) {
    struct arm_stat64_t target_stat;
    ret = Stat64(fd, &target_stat);
    WriteMem(buf_address, &target_stat, sizeof(target_stat));
  } else if (system == "powerpc") {
    struct powerpc_stat64_t target_stat;
    ret = Stat64(fd, &target_stat);
    WriteMem(buf_address, &target_stat, sizeof(target_stat));
  } else ret = -1;
  if (unlikely(verbose))
    logger << DebugInfo
        << "fd = " << fd << ", buf_address = 0x" << hex << buf_address << dec
        << ", ret = 0x" << hex << ret << dec
        << endl << LOCATION << EndDebugInfo;
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
  if (unlikely(verbose))
    logger << DebugInfo
        << "ret = 0x" << hex << ((PARAMETER_TYPE)ret) << dec
        << endl << LOCATION << EndDebugInfo;
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
  if (unlikely(verbose))
    logger << DebugInfo
        << "ret = 0x" << hex << ((PARAMETER_TYPE)ret) << dec
        << endl << LOCATION << EndDebugInfo;
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
  if (unlikely(verbose))
    logger << DebugInfo
        << "ret = 0x" << hex << ((PARAMETER_TYPE)ret) << dec
        << endl << LOCATION << EndDebugInfo;
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
  if (unlikely(verbose))
    logger << DebugInfo
        << "ret = 0x" << hex << ((PARAMETER_TYPE)ret) << dec
        << endl << LOCATION << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)ret,false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_flistxattr() {
  logger << DebugWarning
      << "TODO"
      << endl << LOCATION << EndDebugWarning;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_exit_group() {
  if (unlikely(verbose))
    logger << DebugInfo
        << "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec
        << endl << LOCATION << EndDebugInfo;
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
  if (unlikely(verbose))
    logger << DebugInfo
        << "ret = " <<  ((PARAMETER_TYPE)ret)
        << endl << LOCATION << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)ret,ret < 0);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_fcntl64() {
  if (unlikely(verbose))
    logger << DebugInfo
        << "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec
        << endl << LOCATION << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_dup() {
  int ret;
  int oldfd = GetSystemCallParam(0);

  ret = dup(oldfd);
  if (unlikely(verbose))
    logger << DebugInfo
        << "oldfd = " << oldfd << ", new fd = " << ((PARAMETER_TYPE)ret)
        << endl << LOCATION << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)ret, ret < 0);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_ioctl() {
  if (unlikely(verbose))
    logger << DebugInfo
        << "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec
        << endl << LOCATION << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_ugetrlimit() {
  if (unlikely(verbose))
    logger << DebugInfo
        << "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec
        << endl << LOCATION << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_getrlimit() {
  if (unlikely(verbose))
    logger << DebugInfo
        << "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec
        << endl << LOCATION << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_setrlimit() {
  if (unlikely(verbose))
    logger << DebugInfo
        << "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec
        << endl << LOCATION << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_rt_sigaction() {
  if (unlikely(verbose))
    logger << DebugInfo
        << "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec
        << endl << LOCATION << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_getrusage() {
  if (unlikely(verbose))
    logger << DebugInfo
        << "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec
        << endl << LOCATION << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_unlink() {
  if (unlikely(verbose))
    logger << DebugInfo
        << "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec
        << endl << LOCATION << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_rename() {
  if (unlikely(verbose))
    logger << DebugInfo
        << "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec
        << endl << LOCATION << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_time() {
  if (unlikely(verbose))
    logger << DebugInfo
        << "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec
        << endl << LOCATION << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_socketcall() {
  if (unlikely(verbose))
    logger << DebugInfo
        << "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec
        << endl << LOCATION << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_rt_sigprocmask() {
  if (unlikely(verbose))
    logger << DebugInfo
        << "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec
        << endl << LOCATION << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_kill() {
  if (unlikely(verbose))
    logger << DebugInfo
        << "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec
        << endl << LOCATION << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)0, false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_ftruncate() {
  int ret;

  ret = ftruncate(GetSystemCallParam(0), GetSystemCallParam(1));

  if (unlikely(verbose))
    logger << DebugInfo
        << "ret = 0x" << hex << ((PARAMETER_TYPE)ret) << dec
        << endl << LOCATION << EndDebugInfo;

  SetSystemCallStatus((PARAMETER_TYPE)ret,ret < 0);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_arm_breakpoint() {
  if (unlikely(verbose))
    logger << DebugInfo
        << "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec
        << endl << LOCATION << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_arm_cacheflush() {
  if (unlikely(verbose))
    logger << DebugInfo
        << "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec
        << endl << LOCATION << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_arm_usr26() {
  if (unlikely(verbose))
    logger << DebugInfo
        << "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec
        << endl << LOCATION << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_arm_usr32() {
  if (unlikely(verbose))
    logger << DebugInfo
        << "ret = 0x" << hex << ((PARAMETER_TYPE)(-EINVAL)) << dec
        << endl << LOCATION << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_arm_set_tls() {
  uint32_t r0 = GetSystemCallParam(0);
  memory_injection_import->InjectWriteMemory(0xffff0ff0UL,
                                             (void *)&(r0), 4);
  if (unlikely(verbose))
    logger << DebugInfo
        << "ret = 0x" << hex << ((PARAMETER_TYPE)0) << dec
        << endl << LOCATION << EndDebugInfo;
  SetSystemCallStatus((PARAMETER_TYPE)0, true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetSyscallNameMap() {
  syscall_name_map[string("unknown")] = &thistype::LSC_unknown;
  syscall_name_map[string("exit")] = &thistype::LSC_exit;
  syscall_name_map[string("read")] = &thistype::LSC_read;
  syscall_name_map[string("write")] = &thistype::LSC_write;
  syscall_name_map[string("open")] = &thistype::LSC_open;
  syscall_name_map[string("close")] = &thistype::LSC_close;
  syscall_name_map[string("lseek")] = &thistype::LSC_lseek;
  syscall_name_map[string("getpid")] = &thistype::LSC_getpid;
  syscall_name_map[string("getuid")] = &thistype::LSC_getuid;
  syscall_name_map[string("access")] = &thistype::LSC_access;
  syscall_name_map[string("times")] = &thistype::LSC_times;
  syscall_name_map[string("brk")] = &thistype::LSC_brk;
  syscall_name_map[string("getgid")] = &thistype::LSC_getgid;
  syscall_name_map[string("geteuid")] = &thistype::LSC_geteuid;
  syscall_name_map[string("getegid")] = &thistype::LSC_getegid;
  syscall_name_map[string("munmap")] = &thistype::LSC_munmap;
  syscall_name_map[string("stat")] = &thistype::LSC_stat;
  syscall_name_map[string("fstat")] = &thistype::LSC_fstat;
  syscall_name_map[string("uname")] = &thistype::LSC_uname;
  syscall_name_map[string("llseek")] = &thistype::LSC_llseek;
  syscall_name_map[string("writev")] = &thistype::LSC_writev;
  syscall_name_map[string("mmap")] = &thistype::LSC_mmap;
  syscall_name_map[string("mmap2")] = &thistype::LSC_mmap2;
  syscall_name_map[string("stat64")] = &thistype::LSC_stat64;
  syscall_name_map[string("fstat64")] = &thistype::LSC_fstat64;
  syscall_name_map[string("getuid32")] = &thistype::LSC_getuid32;
  syscall_name_map[string("getgid32")] = &thistype::LSC_getgid32;
  syscall_name_map[string("geteuid32")] = &thistype::LSC_geteuid32;
  syscall_name_map[string("getegid32")] = &thistype::LSC_getegid32;
  syscall_name_map[string("fcntl64")] = &thistype::LSC_fcntl64;
  syscall_name_map[string("flistxattr")] = &thistype::LSC_flistxattr;
  syscall_name_map[string("exit_group")] = &thistype::LSC_exit_group;
  syscall_name_map[string("fcntl")] = &thistype::LSC_fcntl;
  syscall_name_map[string("dup")] = &thistype::LSC_dup;
  syscall_name_map[string("ioctl")] = &thistype::LSC_ioctl;
  syscall_name_map[string("ugetrlimit")] = &thistype::LSC_ugetrlimit;
  syscall_name_map[string("getrlimit")] = &thistype::LSC_getrlimit;
  syscall_name_map[string("setrlimit")] = &thistype::LSC_setrlimit;
  syscall_name_map[string("rt_sigaction")] = &thistype::LSC_rt_sigaction;
  syscall_name_map[string("getrusage")] = &thistype::LSC_getrusage;
  syscall_name_map[string("unlink")] = &thistype::LSC_unlink;
  syscall_name_map[string("rename")] = &thistype::LSC_rename;
  syscall_name_map[string("time")] = &thistype::LSC_time;
  syscall_name_map[string("socketcall")] = &thistype::LSC_socketcall;
  syscall_name_map[string("rt_sigprocmask")] = &thistype::LSC_rt_sigprocmask;
  syscall_name_map[string("kill")] = &thistype::LSC_kill;
  syscall_name_map[string("ftruncate")] = &thistype::LSC_ftruncate;
  // the following are arm private system calls
  if (utsname_machine.compare("armv5") == 0) {
    syscall_name_map[string("breakpoint")] = &thistype::LSC_arm_breakpoint;
    syscall_name_map[string("cacheflush")] = &thistype::LSC_arm_cacheflush;
    syscall_name_map[string("usr26")] = &thistype::LSC_arm_usr26;
    syscall_name_map[string("usr32")] = &thistype::LSC_arm_usr32;
    syscall_name_map[string("set_tls")] = &thistype::LSC_arm_set_tls;
  }
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetSyscallNumber(int id) {
  if ( system == "arm" )
    return ARMGetSyscallNumber(id);
  else if ( system == "arm-eabi" )
    return ARMEABIGetSyscallNumber(id);
  else
    return PPCGetSyscallNumber(id);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int Linux<ADDRESS_TYPE, PARAMETER_TYPE>::ARMGetSyscallNumber(int id) {
  int translated_id = id - 0x0900000;
  return translated_id;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int Linux<ADDRESS_TYPE, PARAMETER_TYPE>::ARMEABIGetSyscallNumber(int id) {
  // the arm eabi ignores the given id and uses register 7
  //   as the id and translated id
  uint32_t translated_id = 0;
  arm_regs[7]->GetValue(&translated_id);
  return (int)translated_id;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int Linux<ADDRESS_TYPE, PARAMETER_TYPE>::PPCGetSyscallNumber(int id) {
  return id;
}

#endif __UNISIM_UTIL_OS_LINUX_LINUX_TCC__
