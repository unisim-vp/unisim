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

#if defined(WIN32) || defined(WIN64)
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

namespace unisim {
namespace util {
namespace os {
namespace linux_os {

#if defined(WIN32) || defined(WIN64)
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
ADDRESS_TYPE Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetMmapBase() const
{
	return mmap_base_;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetMmapBase(ADDRESS_TYPE base)
{
	mmap_base_ = base;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
ADDRESS_TYPE Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetMmapBrkPoint() const
{
	return mmap_brk_point_;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetMmapBrkPoint(ADDRESS_TYPE brk_point)
{
	mmap_brk_point_ = brk_point;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
ADDRESS_TYPE Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetBrkPoint() const
{
	return brk_point_;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetBrkPoint(ADDRESS_TYPE brk_point)
{
	brk_point_ = brk_point;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SysCall::SetStatus(Linux& lin, int ret, bool error)
{
  if (unlikely(lin.verbose_))
    lin.logger_ << DebugInfo << (error ? "err" : "ret") << " = 0x" << std::hex << ret << std::dec << EndDebugInfo;
  
  lin.target_system->SetSystemCallStatus(ret, error);
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SysCall::ReadMem( Linux& lin, ADDRESS_TYPE addr, uint8_t * buffer, uint32_t size )
{
  if (lin.mem_inject_if_ == NULL) return false;
       
  if (lin.mem_inject_if_->InjectReadMemory(addr, buffer, size))
    {
      if (unlikely(lin.verbose_))
        {
          lin.logger_
            << DebugInfo
            << "OS read memory:" << std::endl
            << "\taddr = 0x" << std::hex << addr << std::dec << std::endl
            << "\tsize = " << size << std::endl
            << "\tdata =" << std::hex;
                               
          for (unsigned int i = 0; i < size; i++)
            {
              lin.logger_ << " " << (unsigned int)buffer[i];
            }
                       
          lin.logger_ << std::dec << EndDebugInfo;
        }
      return true;
    }
  else
    {
      lin.logger_
        << DebugWarning
        << "failed OS read memory:" << std::endl
        << "\taddr = 0x" << std::hex << addr << std::dec << std::endl
        << "\tsize = " << size
        << EndDebugWarning;
      return false;
    }
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SysCall::WriteMem( Linux& lin, ADDRESS_TYPE addr, uint8_t const * const buffer, uint32_t size)
{
  if (lin.mem_inject_if_ == NULL) return false;
       
  if (unlikely(lin.verbose_))
    {
      lin.logger_ << DebugInfo
                  << "OS write memory:" << std::endl
                  << "\taddr = 0x" << std::hex << addr << std::dec << std::endl
                  << "\tsize = " << size << std::endl
                  << "\tdata =" << std::hex;
                       
      for (unsigned int i = 0; i < size; i++)
        {
          lin.logger_ << " " << (unsigned int)buffer[i];
        }
               
      lin.logger_ << std::dec << EndDebugInfo;
    }
  return lin.mem_inject_if_->InjectWriteMemory(addr, buffer, size);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SysCall::ReadMemString(Linux& lin, ADDRESS_TYPE addr, std::string& str)
{
  // Two pass string retrieval
  int len = 0;
  for (ADDRESS_TYPE tail = addr; ; len += 1, tail += 1)
    {
      if (len >= 0x100000) {
        lin.Logger() << DebugError << "Huge string: bailing out" << EndDebugError;
        return false;
      }
      uint8_t buffer;
      if (not ReadMem(lin, addr, &buffer, 1)) return false;
    }
  
  str.resize( len, '*' );
  
  return ReadMem(lin, addr, (uint8_t*)&str[0], len);
}

namespace {
  template <uintptr_t N>
  bool
  SubPathOf( std::string str, char const (&ref)[N] )
  {
    for (uintptr_t idx = 0; idx < (N-1); ++idx)
      if (ref[idx] != str[idx]) return false;
    return (str[N-1] == '\0') or (str[N-1] == '/');
  }
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
typename Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SysCall*
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetSyscallByName( std::string _name )
{
  {
    static struct : public SysCall {
      char const* GetName() const { return "exit"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        int status;

        status = lin.target_system->GetSystemCallParam(0);
        if (unlikely(lin.verbose_))
          lin.logger_ << DebugInfo << "exit(status=" << status << EndDebugInfo;
        throw LSCExit( status );
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "read"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        int32_t target_fd;
        int host_fd;
        size_t count;
        ADDRESS_TYPE buf_addr;
        void *buf;
        ssize_t ret;
        int32_t target_errno = 0;

        target_fd = lin.target_system->GetSystemCallParam(0);
        buf_addr = (ADDRESS_TYPE) lin.target_system->GetSystemCallParam(1);
        count = (size_t) lin.target_system->GetSystemCallParam(2);

        host_fd = SysCall::Target2HostFileDescriptor(lin, target_fd);
	
        if(host_fd == -1)
          {
            target_errno = LINUX_EBADF;
            ret = -1;
          }
        else
          {
            buf = malloc(count);

            if(buf)
              {
                ret = read(host_fd, buf, count);
                if(ret == -1) target_errno = this->Host2LinuxErrno(lin, errno);
                if(ret > 0) this->WriteMem(lin, buf_addr, (uint8_t *)buf, ret);
                free(buf);
              }
            else
              {
                ret = (ssize_t) -1;
                target_errno = LINUX_ENOMEM;
                lin.logger_ << DebugWarning << "Out of memory" << EndDebugWarning;
              }
          }

        if(unlikely(lin.verbose_))
          lin.logger_ << DebugInfo << "read(fd=" << target_fd << ", buf=0x" << std::hex << buf_addr << std::dec
                  << ", count=" << count << ")" << EndDebugInfo;

        SysCall::SetStatus(lin, (ret == -1) ? -target_errno : ret, (ret == -1));
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "write"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        int32_t target_fd;
        int host_fd;
        size_t count;
        void *buf;
        ADDRESS_TYPE buf_addr;
        ssize_t ret;
        int32_t target_errno = 0;

        target_fd = lin.target_system->GetSystemCallParam(0);
        buf_addr = lin.target_system->GetSystemCallParam(1);
        count = (size_t)lin.target_system->GetSystemCallParam(2);
	
        host_fd = SysCall::Target2HostFileDescriptor(lin, target_fd);
	
        if(host_fd == -1)
          {
            target_errno = LINUX_EBADF;
            ret = -1;
          }
        else
          {
            buf = malloc(count);

            if(buf)
              {
                this->ReadMem(lin, buf_addr, (uint8_t *)buf, count);
                ret = write(host_fd, buf, count);
                if(ret == -1) target_errno = this->Host2LinuxErrno(lin, errno);
                if(unlikely(lin.verbose_))
                  {
                    lin.logger_ << DebugInfo
                            << "write(fd=" << target_fd << ", buf=0x" << std::hex << buf_addr << std::dec
                            << ", count=" << count << ")" << std::endl
                            << "buffer =";
                    for (size_t i = 0; i < count; i++)
                      {
                        lin.logger_ << " " << std::hex << (unsigned int)((uint8_t *)buf)[i] << std::dec;
                      }
                    lin.logger_ << EndDebugInfo;
                  }
                free(buf);
              }
            else
              {
                ret = (ssize_t) -1;
                target_errno = LINUX_ENOMEM;
                lin.logger_ << DebugWarning << "Out of memory" << EndDebugWarning;
              }
          }

        SysCall::SetStatus(lin, (ret == -1) ? -target_errno : ret, (ret == -1));
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "open"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        int64_t ret;
        int32_t target_errno = 0;
        int32_t target_fd = -1;

        ADDRESS_TYPE addr = lin.target_system->GetSystemCallParam(0);
        std::string pathname;
        if (this->ReadMemString(lin, addr, pathname))
          {
            int flags = lin.target_system->GetSystemCallParam(1);
            mode_t mode = lin.target_system->GetSystemCallParam(2);
            if (SubPathOf(pathname, "/dev") or SubPathOf(pathname, "/proc") or
                SubPathOf(pathname, "/sys") or SubPathOf(pathname, "/var"))
              {
                // deny access to /dev, /proc, /sys, /var
                ret = -1;
                target_errno = LINUX_EACCES;
              }
            else
              {
#if defined(linux) || defined(__linux) || defined(__linux__)
                ret = open(pathname.c_str(), flags, mode);
#else
                int host_flags = 0;
                int host_mode = 0;
                // non-Linux open flags encoding may differ from a true Linux host
                if((flags & LINUX_O_ACCMODE) == LINUX_O_RDONLY) host_flags = (host_flags & ~O_ACCMODE) | O_RDONLY;
                if((flags & LINUX_O_ACCMODE) == LINUX_O_WRONLY) host_flags = (host_flags & ~O_ACCMODE) | O_WRONLY;
                if((flags & LINUX_O_ACCMODE) == LINUX_O_RDWR) host_flags = (host_flags & ~O_ACCMODE) | O_RDWR;
                if(flags & LINUX_O_CREAT) host_flags |= O_CREAT;
                if(flags & LINUX_O_EXCL) host_flags |= O_EXCL;
                if(flags & LINUX_O_TRUNC) host_flags |= O_TRUNC;
                if(flags & LINUX_O_APPEND) host_flags |= O_APPEND;
#if defined(WIN32) || defined(WIN64)
                host_flags |= O_BINARY; // Linux opens file as binary files
                host_mode = mode & S_IRWXU; // Windows doesn't have bits for group and others
#else
                host_mode = mode; // other UNIX systems should have the same bit encoding for protection
#endif
                ret = open(pathname.c_str(), host_flags, host_mode);
#endif
                if(ret == -1) target_errno = this->Host2LinuxErrno(lin, errno);
              }
		
            if(ret != -1)
              {
                int host_fd = ret;
                target_fd = lin.AllocateFileDescriptor();
                // keep relation between the target file descriptor and the host file descriptor
                lin.MapTargetToHostFileDescriptor(target_fd, host_fd);
              }
		
            if(unlikely(lin.verbose_))
              {
                lin.logger_ << DebugInfo
                        << "open(pathname=\"" << pathname << "\", flags=0x" << std::hex << flags
                        << ", mode=0x" << mode << std::dec << ")"
                        << EndDebugInfo;
              }

          }
        else
          {
            ret = -1;
            target_errno = LINUX_ENOMEM;
            lin.logger_ << DebugWarning << "Out of memory" << EndDebugWarning;
          }
	
        SysCall::SetStatus(lin, (ret == -1) ? -target_errno : target_fd, (ret == -1));
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "close"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        int host_fd;
        int32_t target_fd;
        int ret;
        int32_t target_errno = 0;

        target_fd = lin.target_system->GetSystemCallParam(0);
	
        host_fd = SysCall::Target2HostFileDescriptor(lin, target_fd);
	
        if(host_fd == -1)
          {
            target_errno = LINUX_EBADF;
            ret = -1;
          }
        else
          {
            switch(host_fd)
              {
              case 0:
              case 1:
              case 2:
                lin.UnmapTargetToHostFileDescriptor(target_fd);
                lin.FreeFileDescriptor(target_fd);
                ret = 0;
                break;
              default:
                ret = close(host_fd);
                if(ret == 0)
                  {
                    lin.UnmapTargetToHostFileDescriptor(target_fd);
                    lin.FreeFileDescriptor(target_fd);
                  }
              }
		
            if(ret == -1) target_errno = this->Host2LinuxErrno(lin, errno);
          }

        if(unlikely(lin.verbose_))
          lin.logger_ << DebugInfo
                  << "close(fd=" << target_fd << ")"
                  << EndDebugInfo;
        SysCall::SetStatus(lin, (ret == -1) ? -target_errno : ret, (ret == -1));
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "lseek"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        int32_t target_fd;
        int host_fd;
        off_t offset;
        int whence;
        off_t ret;
        int32_t target_errno = 0;

        target_fd = lin.target_system->GetSystemCallParam(0);
        offset = lin.target_system->GetSystemCallParam(1);
        whence = lin.target_system->GetSystemCallParam(2);
	
        host_fd = SysCall::Target2HostFileDescriptor(lin, target_fd);
	
        if(host_fd == -1)
          {
            target_errno = LINUX_EBADF;
            ret = -1;
          }
        else
          {
            ret = lseek(host_fd, offset, whence);
            if(ret == -1) target_errno = this->Host2LinuxErrno(lin, errno);
          }
	
        if(unlikely(lin.verbose_))
          {
            lin.logger_ << DebugInfo
                    << "lseek(fildes=" << target_fd << ", offset=" << offset
                    << ", whence=" << whence << ")"
                    << EndDebugInfo;
          }

        SysCall::SetStatus(lin, (ret == -1) ? -target_errno : ret, (ret == -1));
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "getpid"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        pid_t ret;

        ret = (pid_t) getpid();
        if(unlikely(lin.verbose_))
          lin.logger_ << DebugInfo << "getpid()" << EndDebugInfo;
        SysCall::SetStatus(lin, ret, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "gettid"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        // Note: in a single threaded environment, the thread ID is equal to the process ID (PID, as returned by getpid)
        pid_t ret;

        ret = (pid_t) getpid();
        if(unlikely(lin.verbose_))
          lin.logger_ << DebugInfo << "gettid()" << EndDebugInfo;
        SysCall::SetStatus(lin, ret, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }
  
  {
    static struct : public SysCall {
      char const* GetName() const { return "getuid"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
#if defined(WIN32) || defined(WIN64)
        uint32_t ret = 0;
#else
        uid_t ret;

        ret = getuid();
#endif
        if(unlikely(lin.verbose_))
          lin.logger_ << DebugInfo << "getuid()" << EndDebugInfo;
        SysCall::SetStatus(lin, ret, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "access"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        int ret;
        int32_t target_errno = 0;

        ADDRESS_TYPE addr = lin.target_system->GetSystemCallParam(0);
        std::string pathname;
        if (this->ReadMemString(lin, addr, pathname))
          {
            mode_t mode = lin.target_system->GetSystemCallParam(1);
            if (SubPathOf(pathname, "/dev") or SubPathOf(pathname, "/proc") or
                SubPathOf(pathname, "/sys") or SubPathOf(pathname, "/var"))
              {
                // deny access to /dev, /proc, /sys, /var
                ret = -1;
                target_errno = LINUX_EACCES;
              }
            else
              {
#if defined(WIN32) || defined(WIN64)
                int win_mode = 0;
                win_mode = mode & S_IRWXU; // Windows doesn't have bits for group and others
                ret = access(pathname.c_str(), win_mode);
#else
                ret = access(pathname.c_str(), mode);
#endif
                if(ret == -1) target_errno = this->Host2LinuxErrno(lin, errno);
              }
            if(unlikely(lin.verbose_))
              {
                lin.logger_ << DebugInfo
                        << "access(pathname=\"" << pathname
                        << "\", mode=0x" << std::hex << mode << std::dec << ")"
                        << EndDebugInfo;
              }
          }
        else
          {
            ret = -1;
            target_errno = LINUX_ENOMEM;
            lin.logger_ << DebugWarning << "Out of memory" << EndDebugWarning;
          }
        SysCall::SetStatus(lin, (ret == -1) ? -target_errno : ret, (ret == -1));
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "brk"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        ADDRESS_TYPE new_brk_point;

        new_brk_point = lin.target_system->GetSystemCallParam(0);

        if(new_brk_point > lin.GetBrkPoint())
          {
            lin.SetBrkPoint(new_brk_point);
          }

        if(unlikely(lin.verbose_))
            lin.logger_ << DebugInfo << "brk(new_brk_point=0x" << std::hex << new_brk_point << ")" << EndDebugInfo;
	
        SysCall::SetStatus(lin, lin.GetBrkPoint(), false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "getgid"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
#if defined(WIN32) || defined(WIN64)
        uint32_t ret = 0;
#else
        gid_t ret;

        ret = getgid();
#endif
        if(unlikely(lin.verbose_))
          lin.logger_ << DebugInfo << "getgid()" << EndDebugInfo;
        
        SysCall::SetStatus(lin, ret, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "geteuid"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
#if defined(WIN32) || defined(WIN64)
        uint32_t ret = 0;
#else
        uid_t ret;

        ret = geteuid();
#endif
        if(unlikely(lin.verbose_))
          lin.logger_ << DebugInfo << "geteuid()" << EndDebugInfo;
	
        SysCall::SetStatus(lin, ret, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "getegid"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
#if defined(WIN32) || defined(WIN64)
        uint32_t ret = 0;
#else
        gid_t ret;

        ret = getegid();
#endif
        if(unlikely(lin.verbose_))
          lin.logger_ << DebugInfo << "getegid()" << EndDebugInfo;
	
        SysCall::SetStatus(lin, ret, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "munmap"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        // Note: currently disabled
        SysCall::SetStatus(lin, -LINUX_ENOSYS, false); return;
	
        size_t u = (size_t)(lin.target_system->GetSystemCallParam(1));

        if(lin.GetMmapBrkPoint() - u > lin.GetMmapBrkPoint())
          {
            SysCall::SetStatus(lin, (PARAMETER_TYPE)(-LINUX_EINVAL), true);
            if(unlikely(lin.verbose_))
              lin.logger_ << DebugInfo << "size = " << u << EndDebugInfo;
            return;
          }

        if(lin.GetMmapBrkPoint() - u < lin.GetMmapBase())
          u = (size_t)(lin.GetMmapBrkPoint() - lin.GetMmapBase());

        if(lin.GetMmapBrkPoint() - u >= lin.GetMmapBrkPoint())
          {
            SysCall::SetStatus(lin, (PARAMETER_TYPE)(-LINUX_EINVAL), true);
            if(unlikely(lin.verbose_))
              lin.logger_ << DebugInfo << "size = " << u << EndDebugInfo;
            return;
          }

        SysCall::SetStatus(lin, (PARAMETER_TYPE) 0, false);
	
        if(unlikely(lin.verbose_))
          lin.logger_ << DebugInfo << "size = " << u << EndDebugInfo;
	
        lin.SetMmapBrkPoint(lin.GetMmapBrkPoint() - u);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "stat"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        // TODO write LSC_stat
        lin.logger_ << DebugWarning << "TODO LSC_stat" << EndDebugWarning;
        SysCall::SetStatus(lin, -LINUX_ENOSYS, true);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "_llseek"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        int32_t target_fd;
        int host_fd;
        uint32_t offset_high;
        uint32_t offset_low;
        off64_t offset;
        PARAMETER_TYPE result_addr;
        uint64_t lseek_result64;
        off64_t lseek_ret;
        int whence;
        int32_t target_errno = 0;

        target_fd = lin.target_system->GetSystemCallParam(0);
        offset_high = lin.target_system->GetSystemCallParam(1);
        offset_low = lin.target_system->GetSystemCallParam(2);
        offset = ((uint64_t) offset_high << 32) | (uint64_t) offset_low;
        result_addr = lin.target_system->GetSystemCallParam(3);
        whence = lin.target_system->GetSystemCallParam(4);
	
        host_fd = SysCall::Target2HostFileDescriptor(lin, target_fd);

        if(host_fd == -1)
          {
            target_errno = LINUX_EBADF;
            lseek_ret = -1;
          }
        else
          {
            lseek_ret = lseek64(host_fd, offset, whence);
			
            if(lseek_ret == -1)
              {
                target_errno = this->Host2LinuxErrno(lin, errno);
              }
            else
              {
                lseek_result64 = unisim::util::endian::Host2Target(lin.endianness_, (uint64_t) lseek_ret);
                this->WriteMem(lin, result_addr, (uint8_t *) &lseek_result64, sizeof(lseek_result64));
              }
          }

        if(unlikely(lin.verbose_))
          {
            lin.logger_ << DebugInfo
                    << "_llseek(fd=" << target_fd
                    << ", offset_high=" << offset_high
                    << ", offset_low=" << offset_low
                    << ", result_addr=0x" << std::hex << result_addr << std::dec
                    << ", whence=" << whence << ")" << EndDebugInfo;
          }
	
        SysCall::SetStatus(lin, (PARAMETER_TYPE) (lseek_ret == -1) ? -target_errno : lseek_ret, (lseek_ret == -1));
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "writev"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        int32_t target_fd = lin.target_system->GetSystemCallParam(0);
        uint32_t iovecaddr = lin.target_system->GetSystemCallParam(1);
        int32_t count = lin.target_system->GetSystemCallParam(2);
        int32_t sum = 0;
    
        for (int step = count; (--step) >= 0; iovecaddr += 8) {
          uint32_t iov_base, iov_len;
          this->ReadMem( lin, iovecaddr + 0, (uint8_t*)&iov_base, 4 );
          this->ReadMem( lin, iovecaddr + 4, (uint8_t*)&iov_len, 4 );
          iov_base = unisim::util::endian::Target2Host( lin.endianness_, iov_base );
          iov_len  = unisim::util::endian::Target2Host( lin.endianness_, iov_len );
          assert( iov_len < 0x100000 );
          uint8_t buffer[iov_len];
          this->ReadMem( lin, iov_base, &buffer[0], iov_len );
          int ret = ::write( target_fd, &buffer[0], iov_len );
          if (ret < 0) {
            int32_t target_errno = this->Host2LinuxErrno(lin,  errno );
            SysCall::SetStatus(lin,  -target_errno, true );
            return;
          }
          sum += ret;
        }

        SysCall::SetStatus(lin,  sum, false );
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "mmap"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        // Note: currently disabled
        SysCall::SetStatus(lin, -LINUX_ENOSYS,true); return;
	
        if(lin.target_system->GetSystemCallParam(3) == 0x32)
          {
            /* MAP_PRIVATE | MAP_ANONYMOUS */
            SysCall::SetStatus(lin, lin.target_system->GetSystemCallParam(0),false);
            if(unlikely(lin.verbose_))
              {
                lin.logger_  << DebugInfo << "map_type = 0x" << std::hex << lin.target_system->GetSystemCallParam(3)
                         << std::dec << ", size = " << lin.target_system->GetSystemCallParam(1)
                         << EndDebugInfo;
              }
            return;
          }

        if((lin.target_system->GetSystemCallParam(3)&0xFF) != 0x22)
          {
            /* MAP_PRIVATE | MAP_ANONYMOUS */
            SysCall::SetStatus(lin, (PARAMETER_TYPE)(-LINUX_EINVAL),true);
            if(unlikely(lin.verbose_))
              {
                lin.logger_ << DebugInfo
                        << "map_type = 0x" << std::hex << lin.target_system->GetSystemCallParam(3) << std::dec
                        << ", size = " << lin.target_system->GetSystemCallParam(1)
                        << EndDebugInfo;
              }
            return;
          }
	
        SysCall::SetStatus(lin, lin.GetMmapBrkPoint(),false);
	
        if(unlikely(lin.verbose_))
          {
            lin.logger_ << DebugInfo
                    << "map_type = 0x" << std::hex << lin.target_system->GetSystemCallParam(3) << std::dec
                    << ", size = " << lin.target_system->GetSystemCallParam(1)
                    << EndDebugInfo;
          }
	
        lin.SetMmapBrkPoint(lin.GetMmapBrkPoint() + lin.target_system->GetSystemCallParam(1));
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "mmap2"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        // Note: currently disabled
        SysCall::SetStatus(lin, -LINUX_ENOSYS,true); return;

        if(lin.target_system->GetSystemCallParam(3) != 0x22)
          {
            /* MAP_PRIVATE | MAP_ANONYMOUS */
            SysCall::SetStatus(lin, (PARAMETER_TYPE)(-LINUX_EINVAL),true);
            if(unlikely(lin.verbose_))
              lin.logger_ << DebugInfo
                      << "map_type = 0x" << std::hex << lin.target_system->GetSystemCallParam(3) << std::dec
                      << ", size = " << lin.target_system->GetSystemCallParam(1)
                      << EndDebugInfo;
            return;
          }

        if(lin.GetMmapBrkPoint() + lin.target_system->GetSystemCallParam(1) > lin.target_system->GetSystemCallParam(1))
          {
            SysCall::SetStatus(lin, lin.GetMmapBrkPoint(),false);
		
            if(unlikely(lin.verbose_))
              {
                lin.logger_ << DebugInfo
                        << "map_type = 0x" << std::hex << lin.target_system->GetSystemCallParam(3) << std::dec
                        << ", size = " << lin.target_system->GetSystemCallParam(1)
                        << EndDebugInfo;
              }
		
            lin.SetMmapBrkPoint(lin.GetMmapBrkPoint() + lin.target_system->GetSystemCallParam(1));
          }
        else
          {
            if(unlikely(lin.verbose_))
              {
                lin.logger_ << DebugInfo
                        << "map_type = 0x" << std::hex << lin.target_system->GetSystemCallParam(3) << std::dec
                        << ", size = " << lin.target_system->GetSystemCallParam(1)
                        << EndDebugInfo;
              }
		
            SysCall::SetStatus(lin, (PARAMETER_TYPE)(-LINUX_EINVAL),true);
          }
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "getuid32"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
#if defined(WIN32) || defined(WIN64)
        uint32_t ret = 0;
#else
        uid_t ret;

        ret = getuid();
#endif
        SysCall::SetStatus(lin, (PARAMETER_TYPE) ret, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "getgid32"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
#if defined(WIN32) || defined(WIN64)
        uint32_t ret = 0;
#else
        gid_t ret;

        ret = getgid();
#endif
        SysCall::SetStatus(lin, (PARAMETER_TYPE) ret, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "geteuid32"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
#if defined(WIN32) || defined(WIN64)
        uint32_t ret = 0;
#else
        uid_t ret;

        ret = geteuid();
#endif
        SysCall::SetStatus(lin, (PARAMETER_TYPE) ret, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "getegid32"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
#if defined(WIN32) || defined(WIN64)
        uint32_t ret = 0;
#else
        gid_t ret;

        ret = getegid();
#endif
        SysCall::SetStatus(lin, (PARAMETER_TYPE) ret, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "flistxattr"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        // TODO implement LSC_flistxattr
        lin.logger_ << DebugWarning
                << "TODO LSC_flistxattr"
                << EndDebugWarning;
        SysCall::SetStatus(lin, -LINUX_ENOSYS, true);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "exit_group"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        SysCall::SetStatus(lin, (PARAMETER_TYPE)(-LINUX_EINVAL),true);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "fcntl"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        int32_t target_fd;
        int host_fd;
        int32_t cmd;
        int32_t arg;
        int64_t ret;
        int32_t target_errno = 0;

        target_fd = lin.target_system->GetSystemCallParam(0);
        cmd = lin.target_system->GetSystemCallParam(1);
        arg = (size_t)lin.target_system->GetSystemCallParam(2);
	
        host_fd = SysCall::Target2HostFileDescriptor(lin, target_fd);
	
        if(host_fd == -1)
          {
            target_errno = LINUX_EBADF;
            ret = -1;
          }
        else
          {
#if defined(WIN32) || defined(WIN64)
            ret = -1;
            SysCall::SetStatus(lin, (PARAMETER_TYPE) -LINUX_ENOSYS, true);
#else
            ret = fcntl(host_fd, cmd, arg);
            if(ret == -1) target_errno = this->Host2LinuxErrno(lin, errno);
#endif
          }
	
        if(unlikely(lin.verbose_))
          lin.logger_ << DebugInfo << "fd=" << target_fd << ",cmd=" << cmd << ",arg=" << arg << EndDebugInfo;
	
        SysCall::SetStatus(lin, (PARAMETER_TYPE) (ret == -1) ? -target_errno : ret, (ret == -1));
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "fcntl64"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        int32_t target_fd;
        int host_fd;
        int32_t cmd;
        int64_t ret;
        int32_t target_errno = 0;

        target_fd = lin.target_system->GetSystemCallParam(0);
        cmd = lin.target_system->GetSystemCallParam(1);
	
        host_fd = SysCall::Target2HostFileDescriptor(lin, target_fd);
	
        if(host_fd == -1)
          {
            target_errno = LINUX_EBADF;
            ret = -1;
          }
        else
          {
#if defined(WIN32) || defined(WIN64)
            ret = -1;
            target_errno = LINUX_ENOSYS;
#else
            switch(cmd)
              {
              case F_DUPFD:
                //    case F_DUPFD_CLOEXEC:
              case F_GETFD:
              case F_SETFD:
              case F_GETFL:
              case F_SETFL:
                ret = fcntl(host_fd, cmd);
                if(ret == -1) target_errno = this->Host2LinuxErrno(lin, errno);
                break;
              case F_GETLK:
              case F_SETLK:
              case F_SETLKW:
              default:
                ret = -1;
                target_errno = LINUX_EINVAL;
                break;
              }
		
#endif
          }
	
        if(ret == -1)
          {
            SysCall::SetStatus(lin, (PARAMETER_TYPE) -target_errno, true);
          }
        else
          {
            SysCall::SetStatus(lin, (PARAMETER_TYPE) ret, false);
          }
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "dup"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        int32_t target_oldfd;
        int host_oldfd;
        int32_t target_newfd = -1;
        int ret;
        int32_t target_errno = 0;
	
        target_oldfd = lin.target_system->GetSystemCallParam(0);

        host_oldfd = SysCall::Target2HostFileDescriptor(lin, target_oldfd);
	
        if(host_oldfd == -1)
          {
            target_errno = LINUX_EBADF;
            ret = -1;
          }
        else
          {
            ret = dup(host_oldfd);
            if(ret == -1)
              {
                target_errno = this->Host2LinuxErrno(lin, errno);
              }
            else
              {
                int host_newfd = ret;
                target_newfd = lin.AllocateFileDescriptor();
                // keep relation between the target file descriptor and the host file descriptor
                lin.MapTargetToHostFileDescriptor(target_newfd, host_newfd);
              }
          }
	
        if(unlikely(lin.verbose_))
          {
            lin.logger_ << DebugInfo
                    << "oldfd = " << target_oldfd << ", new fd = " << ((PARAMETER_TYPE)target_newfd)
                    << EndDebugInfo;
          }
	
        SysCall::SetStatus(lin, (PARAMETER_TYPE) (ret == -1) ? -target_errno : target_newfd, (ret == -1));
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "ioctl"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        SysCall::SetStatus(lin, (PARAMETER_TYPE)(-EINVAL),true);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "ugetrlimit"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        SysCall::SetStatus(lin, (PARAMETER_TYPE)(-EINVAL),true);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "getrlimit"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        SysCall::SetStatus(lin, (PARAMETER_TYPE)(-EINVAL),true);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "setrlimit"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        SysCall::SetStatus(lin, (PARAMETER_TYPE)(-EINVAL),true);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "rt_sigaction"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        SysCall::SetStatus(lin, (PARAMETER_TYPE)(-EINVAL),true);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "getrusage"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        SysCall::SetStatus(lin, (PARAMETER_TYPE)(-EINVAL),true);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "unlink"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        int ret;
        int32_t target_errno = 0;

        ADDRESS_TYPE pathnameaddr = lin.target_system->GetSystemCallParam(0);
        std::string pathname;
        if (this->ReadMemString(lin, pathnameaddr, pathname))
          {
            ret = unlink(pathname.c_str());
            if (ret == -1) target_errno = this->Host2LinuxErrno(lin, errno);
		
            if(unlikely(lin.verbose_))
              {
                lin.logger_ << DebugInfo
                        << "unlink(pathname=\"" << pathname << "\")" << std::endl
                        << EndDebugInfo;
              }
          }
        else
          {
            ret = -1;
            target_errno = LINUX_ENOMEM;
          }
	
        SysCall::SetStatus(lin, (PARAMETER_TYPE) (ret == -1) ? -target_errno : ret, (ret == -1));
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "rename"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        int ret;
        int32_t target_errno = 0;

        ADDRESS_TYPE oldpathaddr = lin.target_system->GetSystemCallParam(0);
        ADDRESS_TYPE newpathaddr = lin.target_system->GetSystemCallParam(1);
        std::string oldpath, newpath;
        if (this->ReadMemString(lin, oldpathaddr, oldpath) and this->ReadMemString(lin, newpathaddr, newpath))
          {
            ret = rename(oldpath.c_str(), newpath.c_str());
			
            if (unlikely(lin.verbose_))
              {
                lin.logger_ << DebugInfo
                            << "rename(oldpath=\"" << oldpath << "\", newpath=\"" << newpath << "\")" << std::endl
                            << EndDebugInfo;
              }
          }
        else
          {
            ret = -1;
            target_errno = LINUX_ENOMEM;
          }
	
        SysCall::SetStatus(lin, (PARAMETER_TYPE) (ret == -1) ? -target_errno : ret, (ret == -1));
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "time"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        SysCall::SetStatus(lin, (PARAMETER_TYPE)(-EINVAL), true);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "socketcall"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        SysCall::SetStatus(lin, (PARAMETER_TYPE)(-EINVAL), true);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "rt_sigprocmask"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        SysCall::SetStatus(lin, (PARAMETER_TYPE)(-EINVAL), true);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "kill"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        SysCall::SetStatus(lin, (PARAMETER_TYPE) 0, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "tkill"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        SysCall::SetStatus(lin, (PARAMETER_TYPE) 0, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "tgkill"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        SysCall::SetStatus(lin, (PARAMETER_TYPE) 0, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "ftruncate"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        int host_fd;
        int32_t target_fd;
        int32_t length;
        int ret;
        int32_t target_errno = 0;

        target_fd = lin.target_system->GetSystemCallParam(0);
        length = lin.target_system->GetSystemCallParam(1);
	
        host_fd = SysCall::Target2HostFileDescriptor(lin, target_fd);
	
        if(host_fd == -1)
          {
            target_errno = LINUX_EBADF;
            ret = -1;
          }
        else
          {
            ret = ftruncate(host_fd, length);
            if(ret == -1) target_errno = this->Host2LinuxErrno(lin, errno);
          }
	
        SysCall::SetStatus(lin, (PARAMETER_TYPE) (ret == -1) ? -target_errno : ret, (ret == -1));
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "umask"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        mode_t mask;
        int ret;

        mask = lin.target_system->GetSystemCallParam(0);
        ret = umask(mask);
        if(unlikely(lin.verbose_))
          {
            lin.logger_ << DebugInfo
                    << "umask(mask=0" << std::oct << mask << std::dec << ")"
                    << EndDebugInfo;
          }

        SysCall::SetStatus(lin, ret, false);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }

  {
    static struct : public SysCall {
      char const* GetName() const { return "statfs"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        SysCall::SetStatus(lin, (PARAMETER_TYPE)(-EINVAL), true);
      }
    } sc;
    if (_name.compare( sc.GetName() ) == 0) return &sc;
  }
  
  {
    struct UnimplementedSC : public SysCall
    {
      UnimplementedSC( std::string _name ) : name( _name ) {} std::string name;
      char const* GetName() const { return name.c_str(); }
      void Execute( Linux& lin, int syscall_id ) const
      {
        if(unlikely(lin.verbose_))
          lin.logger_ << DebugInfo << "Unimplemented system call '" << name << "', #" << syscall_id << EndDebugInfo;
        SysCall::SetStatus(lin, -LINUX_ENOSYS, true);
      }
      void Release() { delete this; }
    };
  
    return new UnimplementedSC( _name );
  }
}

} // end of namespace unisim
} // end of namespace util
} // end of namespace os
} // end of namespace linux_os

#endif // __UNISIM_UTIL_OS_LINUX_LINUX_TCC__
