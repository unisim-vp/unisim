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

#include <unisim/util/endian/endian.hh>
#include <unisim/util/os/linux_os/linux.hh>
#include <unisim/util/os/linux_os/files_flags.hh>

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
int32_t Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SysCall::HostToLinuxErrno(int host_errno)
{
  switch (host_errno)
    {
    default: return LINUX_EINVAL;
#ifdef EPERM
    case EPERM: return LINUX_EPERM;
#endif
#ifdef ENOENT
    case ENOENT: return LINUX_ENOENT;
#endif
#ifdef ESRCH
    case ESRCH: return LINUX_ESRCH;
#endif
#ifdef EINTR
    case EINTR: return LINUX_EINTR;
#endif
#ifdef EIO
    case EIO: return LINUX_EIO;
#endif
#ifdef ENXIO
    case ENXIO: return LINUX_ENXIO;
#endif
#ifdef E2BIG
    case E2BIG: return LINUX_E2BIG;
#endif
#ifdef ENOEXEC
    case ENOEXEC: return LINUX_ENOEXEC;
#endif
#ifdef EBADF
    case EBADF: return LINUX_EBADF;
#endif
#ifdef ECHILD
    case ECHILD: return LINUX_ECHILD;
#endif
#ifdef EAGAIN
    case EAGAIN: return LINUX_EAGAIN;
#endif
#ifdef ENOMEM
    case ENOMEM: return LINUX_ENOMEM;
#endif
#ifdef EACCES
    case EACCES: return LINUX_EACCES;
#endif
#ifdef EFAULT
    case EFAULT: return LINUX_EFAULT;
#endif
#ifdef ENOTBLK
    case ENOTBLK: return LINUX_ENOTBLK;
#endif
#ifdef EBUSY
    case EBUSY: return LINUX_EBUSY;
#endif
#ifdef EEXIST
    case EEXIST: return LINUX_EEXIST;
#endif
#ifdef EXDEV
    case EXDEV: return LINUX_EXDEV;
#endif
#ifdef ENODEV
    case ENODEV: return LINUX_ENODEV;
#endif
#ifdef ENOTDIR
    case ENOTDIR: return LINUX_ENOTDIR;
#endif
#ifdef EISDIR
    case EISDIR: return LINUX_EISDIR;
#endif
#ifdef EINVAL
    case EINVAL: return LINUX_EINVAL;
#endif
#ifdef ENFILE
    case ENFILE: return LINUX_ENFILE;
#endif
#ifdef EMFILE
    case EMFILE: return LINUX_EMFILE;
#endif
#ifdef ENOTTY
    case ENOTTY: return LINUX_ENOTTY;
#endif
#ifdef ETXTBSY
    case ETXTBSY: return LINUX_ETXTBSY;
#endif
#ifdef EFBIG
    case EFBIG: return LINUX_EFBIG;
#endif
#ifdef ENOSPC
    case ENOSPC: return LINUX_ENOSPC;
#endif
#ifdef ESPIPE
    case ESPIPE: return LINUX_ESPIPE;
#endif
#ifdef EROFS
    case EROFS: return LINUX_EROFS;
#endif
#ifdef EMLINK
    case EMLINK: return LINUX_EMLINK;
#endif
#ifdef EPIPE
    case EPIPE: return LINUX_EPIPE;
#endif
#ifdef EDOM
    case EDOM: return LINUX_EDOM;
#endif
#ifdef ERANGE
    case ERANGE: return LINUX_ERANGE;
#endif
#ifdef EDEADLK
    case EDEADLK: return LINUX_EDEADLK;
#endif
#ifdef ENAMETOOLONG
    case ENAMETOOLONG: return LINUX_ENAMETOOLONG;
#endif
#ifdef ENOLCK
    case ENOLCK: return LINUX_ENOLCK;
#endif
#ifdef ENOSYS
    case ENOSYS: return LINUX_ENOSYS;
#endif
#ifdef ENOTEMPTY
    case ENOTEMPTY: return LINUX_ENOTEMPTY;
#endif
#ifdef ELOOP
    case ELOOP: return LINUX_ELOOP;
#endif
#if defined(EWOULDBLOCK)
# if !defined(EAGAIN) || (EAGAIN != EWOULDBLOCK)
    case EWOULDBLOCK: return LINUX_EWOULDBLOCK;
# endif
#endif
#ifdef ENOMSG
    case ENOMSG: return LINUX_ENOMSG;
#endif
#ifdef EIDRM
    case EIDRM: return LINUX_EIDRM;
#endif
#ifdef ECHRNG
    case ECHRNG: return LINUX_ECHRNG;
#endif
#ifdef EL2NSYNC
    case EL2NSYNC: return LINUX_EL2NSYNC;
#endif
#ifdef EL3HLT
    case EL3HLT: return LINUX_EL3HLT;
#endif
#ifdef EL3RST
    case EL3RST: return LINUX_EL3RST;
#endif
#ifdef ELNRNG
    case ELNRNG: return LINUX_ELNRNG;
#endif
#ifdef EUNATCH
    case EUNATCH: return LINUX_EUNATCH;
#endif
#ifdef ENOCSI
    case ENOCSI: return LINUX_ENOCSI;
#endif
#ifdef EL2HLT
    case EL2HLT: return LINUX_EL2HLT;
#endif
#ifdef EBADE
    case EBADE: return LINUX_EBADE;
#endif
#ifdef EBADR
    case EBADR: return LINUX_EBADR;
#endif
#ifdef EXFULL
    case EXFULL: return LINUX_EXFULL;
#endif
#ifdef ENOANO
    case ENOANO: return LINUX_ENOANO;
#endif
#ifdef EBADRQC
    case EBADRQC: return LINUX_EBADRQC;
#endif
#ifdef EBADSLT
    case EBADSLT: return LINUX_EBADSLT;
#endif
#if defined(EDEADLOCK)
# if !defined(EDEADLK) || (EDEADLK != EDEADLOCK)
    case EDEADLOCK: return LINUX_EDEADLOCK;
# endif
#endif
#ifdef EBFONT
    case EBFONT: return LINUX_EBFONT;
#endif
#ifdef ENOSTR
    case ENOSTR: return LINUX_ENOSTR;
#endif
#ifdef ENODATA
    case ENODATA: return LINUX_ENODATA;
#endif
#ifdef ETIME
    case ETIME: return LINUX_ETIME;
#endif
#ifdef ENOSR
    case ENOSR: return LINUX_ENOSR;
#endif
#ifdef ENONET
    case ENONET: return LINUX_ENONET;
#endif
#ifdef ENOPKG
    case ENOPKG: return LINUX_ENOPKG;
#endif
#ifdef EREMOTE
    case EREMOTE: return LINUX_EREMOTE;
#endif
#ifdef ENOLINK
    case ENOLINK: return LINUX_ENOLINK;
#endif
#ifdef EADV
    case EADV: return LINUX_EADV;
#endif
#ifdef ESRMNT
    case ESRMNT: return LINUX_ESRMNT;
#endif
#ifdef ECOMM
    case ECOMM: return LINUX_ECOMM;
#endif
#ifdef EPROTO
    case EPROTO: return LINUX_EPROTO;
#endif
#ifdef EMULTIHOP
    case EMULTIHOP: return LINUX_EMULTIHOP;
#endif
#ifdef EDOTDOT
    case EDOTDOT: return LINUX_EDOTDOT;
#endif
#ifdef EBADMSG
    case EBADMSG: return LINUX_EBADMSG;
#endif
#ifdef EOVERFLOW
    case EOVERFLOW: return LINUX_EOVERFLOW;
#endif
#ifdef ENOTUNIQ
    case ENOTUNIQ: return LINUX_ENOTUNIQ;
#endif
#ifdef EBADFD
    case EBADFD: return LINUX_EBADFD;
#endif
#ifdef EREMCHG
    case EREMCHG: return LINUX_EREMCHG;
#endif
#ifdef ELIBACC
    case ELIBACC: return LINUX_ELIBACC;
#endif
#ifdef ELIBBAD
    case ELIBBAD: return LINUX_ELIBBAD;
#endif
#ifdef ELIBSCN
    case ELIBSCN: return LINUX_ELIBSCN;
#endif
#ifdef ELIBMAX
    case ELIBMAX: return LINUX_ELIBMAX;
#endif
#ifdef ELIBEXEC
    case ELIBEXEC: return LINUX_ELIBEXEC;
#endif
#ifdef EILSEQ
    case EILSEQ: return LINUX_EILSEQ;
#endif
#ifdef ERESTART
    case ERESTART: return LINUX_ERESTART;
#endif
#ifdef ESTRPIPE
    case ESTRPIPE: return LINUX_ESTRPIPE;
#endif
#ifdef EUSERS
    case EUSERS: return LINUX_EUSERS;
#endif
#ifdef ENOTSOCK
    case ENOTSOCK: return LINUX_ENOTSOCK;
#endif
#ifdef EDESTADDRREQ
    case EDESTADDRREQ: return LINUX_EDESTADDRREQ;
#endif
#ifdef EMSGSIZE
    case EMSGSIZE: return LINUX_EMSGSIZE;
#endif
#ifdef EPROTOTYPE
    case EPROTOTYPE: return LINUX_EPROTOTYPE;
#endif
#ifdef ENOPROTOOPT
    case ENOPROTOOPT: return LINUX_ENOPROTOOPT;
#endif
#ifdef EPROTONOSUPPORT
    case EPROTONOSUPPORT: return LINUX_EPROTONOSUPPORT;
#endif
#ifdef ESOCKTNOSUPPORT
    case ESOCKTNOSUPPORT: return LINUX_ESOCKTNOSUPPORT;
#endif
#ifdef EOPNOTSUPP
    case EOPNOTSUPP: return LINUX_EOPNOTSUPP;
#endif
#ifdef EPFNOSUPPORT
    case EPFNOSUPPORT: return LINUX_EPFNOSUPPORT;
#endif
#ifdef EAFNOSUPPORT
    case EAFNOSUPPORT: return LINUX_EAFNOSUPPORT;
#endif
#ifdef EADDRINUSE
    case EADDRINUSE: return LINUX_EADDRINUSE;
#endif
#ifdef EADDRNOTAVAIL
    case EADDRNOTAVAIL: return LINUX_EADDRNOTAVAIL;
#endif
#ifdef ENETDOWN
    case ENETDOWN: return LINUX_ENETDOWN;
#endif
#ifdef ENETUNREACH
    case ENETUNREACH: return LINUX_ENETUNREACH;
#endif
#ifdef ENETRESET
    case ENETRESET: return LINUX_ENETRESET;
#endif
#ifdef ECONNABORTED
    case ECONNABORTED: return LINUX_ECONNABORTED;
#endif
#ifdef ECONNRESET
    case ECONNRESET: return LINUX_ECONNRESET;
#endif
#ifdef ENOBUFS
    case ENOBUFS: return LINUX_ENOBUFS;
#endif
#ifdef EISCONN
    case EISCONN: return LINUX_EISCONN;
#endif
#ifdef ENOTCONN
    case ENOTCONN: return LINUX_ENOTCONN;
#endif
#ifdef ESHUTDOWN
    case ESHUTDOWN: return LINUX_ESHUTDOWN;
#endif
#ifdef ETOOMANYREFS
    case ETOOMANYREFS: return LINUX_ETOOMANYREFS;
#endif
#ifdef ETIMEDOUT
    case ETIMEDOUT: return LINUX_ETIMEDOUT;
#endif
#ifdef ECONNREFUSED
    case ECONNREFUSED: return LINUX_ECONNREFUSED;
#endif
#ifdef EHOSTDOWN
    case EHOSTDOWN: return LINUX_EHOSTDOWN;
#endif
#ifdef EHOSTUNREACH
    case EHOSTUNREACH: return LINUX_EHOSTUNREACH;
#endif
#ifdef EALREADY
    case EALREADY: return LINUX_EALREADY;
#endif
#ifdef EINPROGRESS
    case EINPROGRESS: return LINUX_EINPROGRESS;
#endif
#ifdef ESTALE
    case ESTALE: return LINUX_ESTALE;
#endif
#ifdef EUCLEAN
    case EUCLEAN: return LINUX_EUCLEAN;
#endif
#ifdef ENOTNAM
    case ENOTNAM: return LINUX_ENOTNAM;
#endif
#ifdef ENAVAIL
    case ENAVAIL: return LINUX_ENAVAIL;
#endif
#ifdef EISNAM
    case EISNAM: return LINUX_EISNAM;
#endif
#ifdef EREMOTEIO
    case EREMOTEIO: return LINUX_EREMOTEIO;
#endif
#ifdef EDQUOT
    case EDQUOT: return LINUX_EDQUOT;
#endif
#ifdef ENOMEDIUM
    case ENOMEDIUM: return LINUX_ENOMEDIUM;
#endif
#ifdef EMEDIUMTYPE
    case EMEDIUMTYPE: return LINUX_EMEDIUMTYPE;
#endif
#ifdef ECANCELED
    case ECANCELED: return LINUX_ECANCELED;
#endif
#ifdef ENOKEY
    case ENOKEY: return LINUX_ENOKEY;
#endif
#ifdef EKEYEXPIRED
    case EKEYEXPIRED: return LINUX_EKEYEXPIRED;
#endif
#ifdef EKEYREVOKED
    case EKEYREVOKED: return LINUX_EKEYREVOKED;
#endif
#ifdef EKEYREJECTED
    case EKEYREJECTED: return LINUX_EKEYREJECTED;
#endif
#ifdef EOWNERDEAD
    case EOWNERDEAD: return LINUX_EOWNERDEAD;
#endif
#ifdef ENOTRECOVERABLE
    case ENOTRECOVERABLE: return LINUX_ENOTRECOVERABLE;
#endif
#ifdef ERFKILL
    case ERFKILL: return LINUX_ERFKILL;
#endif
#ifdef EHWPOISON
    case EHWPOISON: return LINUX_EHWPOISON;
#endif
    }
    return LINUX_EINVAL;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SysCall::Target2HostFileDescriptor(Linux& lin, int32_t fd)
{
  // Return an error if file descriptor does not exist
  typename std::map<int32_t, int>::iterator iter = lin.target_to_host_fildes.find(fd);
  if(iter == lin.target_to_host_fildes.end()) return -1;

  // Translate the target file descriptor to an host file descriptor
  return (*iter).second;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SysCall::SetStatus(Linux& lin, int ret, bool error)
{
  if (unlikely(lin.verbose_))
    lin.logger_ << DebugInfo << (error ? "err" : "ret") << " = 0x" << std::hex << ret << std::dec << EndDebugInfo;
  
  lin.target_system->SetSystemCallStatus(ret, error);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
PARAMETER_TYPE Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SysCall::GetParam(Linux& lin, int id)
{
  return lin.target_system->GetSystemCallParam(id);
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
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetSysCall( std::string _name )
{
  {
    static struct : public SysCall {
      char const* GetName() const { return "exit"; }
      void Execute( Linux& lin, int syscall_id ) const
      {
        int status;

        status = SysCall::GetParam(lin, 0);
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

        target_fd = SysCall::GetParam(lin, 0);
        buf_addr = (ADDRESS_TYPE) SysCall::GetParam(lin, 1);
        count = (size_t) SysCall::GetParam(lin, 2);

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
                if(ret == -1) target_errno = SysCall::HostToLinuxErrno(errno);
                if(ret > 0) SysCall::WriteMem(lin, buf_addr, (uint8_t *)buf, ret);
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

        target_fd = SysCall::GetParam(lin, 0);
        buf_addr = SysCall::GetParam(lin, 1);
        count = (size_t)SysCall::GetParam(lin, 2);
  
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
                SysCall::ReadMem(lin, buf_addr, (uint8_t *)buf, count);
                ret = write(host_fd, buf, count);
                if(ret == -1) target_errno = SysCall::HostToLinuxErrno(errno);
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

        ADDRESS_TYPE addr = SysCall::GetParam(lin, 0);
        std::string pathname;
        if (SysCall::ReadMemString(lin, addr, pathname))
          {
            int flags = SysCall::GetParam(lin, 1);
            mode_t mode = SysCall::GetParam(lin, 2);
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
                if(ret == -1) target_errno = SysCall::HostToLinuxErrno(errno);
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

        target_fd = SysCall::GetParam(lin, 0);
  
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
    
            if(ret == -1) target_errno = SysCall::HostToLinuxErrno(errno);
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

        target_fd = SysCall::GetParam(lin, 0);
        offset = SysCall::GetParam(lin, 1);
        whence = SysCall::GetParam(lin, 2);
  
        host_fd = SysCall::Target2HostFileDescriptor(lin, target_fd);
  
        if(host_fd == -1)
          {
            target_errno = LINUX_EBADF;
            ret = -1;
          }
        else
          {
            ret = lseek(host_fd, offset, whence);
            if(ret == -1) target_errno = SysCall::HostToLinuxErrno(errno);
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

        ADDRESS_TYPE addr = SysCall::GetParam(lin, 0);
        std::string pathname;
        if (SysCall::ReadMemString(lin, addr, pathname))
          {
            mode_t mode = SysCall::GetParam(lin, 1);
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
                if(ret == -1) target_errno = SysCall::HostToLinuxErrno(errno);
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

        new_brk_point = SysCall::GetParam(lin, 0);

        if(new_brk_point > lin.brk_point_)
          {
            lin.brk_point_ = new_brk_point;
          }

        if(unlikely(lin.verbose_))
          lin.logger_ << DebugInfo << "brk(new_brk_point=0x" << std::hex << new_brk_point << ")" << EndDebugInfo;
  
        SysCall::SetStatus(lin, new_brk_point, false);
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
        SysCall::SetStatus(lin, -LINUX_ENOSYS,true); return;
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

        target_fd = SysCall::GetParam(lin, 0);
        offset_high = SysCall::GetParam(lin, 1);
        offset_low = SysCall::GetParam(lin, 2);
        offset = ((uint64_t) offset_high << 32) | (uint64_t) offset_low;
        result_addr = SysCall::GetParam(lin, 3);
        whence = SysCall::GetParam(lin, 4);
  
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
                target_errno = SysCall::HostToLinuxErrno(errno);
              }
            else
              {
                lseek_result64 = unisim::util::endian::Host2Target(lin.endianness_, (uint64_t) lseek_ret);
                SysCall::WriteMem(lin, result_addr, (uint8_t *) &lseek_result64, sizeof(lseek_result64));
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
        int32_t target_fd = SysCall::GetParam(lin, 0);
        uint32_t iovecaddr = SysCall::GetParam(lin, 1);
        int32_t count = SysCall::GetParam(lin, 2);
        int32_t sum = 0;
    
        for (int step = count; (--step) >= 0; iovecaddr += 8) {
          uint32_t iov_base, iov_len;
          SysCall::ReadMem( lin, iovecaddr + 0, (uint8_t*)&iov_base, 4 );
          SysCall::ReadMem( lin, iovecaddr + 4, (uint8_t*)&iov_len, 4 );
          iov_base = unisim::util::endian::Target2Host( lin.endianness_, iov_base );
          iov_len  = unisim::util::endian::Target2Host( lin.endianness_, iov_len );
          assert( iov_len < 0x100000 );
          uint8_t buffer[iov_len];
          SysCall::ReadMem( lin, iov_base, &buffer[0], iov_len );
          int ret = ::write( target_fd, &buffer[0], iov_len );
          if (ret < 0) {
            int32_t target_errno = SysCall::HostToLinuxErrno( errno );
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

        target_fd = SysCall::GetParam(lin, 0);
        cmd = SysCall::GetParam(lin, 1);
        arg = (size_t)SysCall::GetParam(lin, 2);
  
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
            if(ret == -1) target_errno = SysCall::HostToLinuxErrno(errno);
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

        target_fd = SysCall::GetParam(lin, 0);
        cmd = SysCall::GetParam(lin, 1);
  
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
                if(ret == -1) target_errno = SysCall::HostToLinuxErrno(errno);
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
  
        target_oldfd = SysCall::GetParam(lin, 0);

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
                target_errno = SysCall::HostToLinuxErrno(errno);
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

        ADDRESS_TYPE pathnameaddr = SysCall::GetParam(lin, 0);
        std::string pathname;
        if (SysCall::ReadMemString(lin, pathnameaddr, pathname))
          {
            ret = unlink(pathname.c_str());
            if (ret == -1) target_errno = SysCall::HostToLinuxErrno(errno);
    
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

        ADDRESS_TYPE oldpathaddr = SysCall::GetParam(lin, 0);
        ADDRESS_TYPE newpathaddr = SysCall::GetParam(lin, 1);
        std::string oldpath, newpath;
        if (SysCall::ReadMemString(lin, oldpathaddr, oldpath) and SysCall::ReadMemString(lin, newpathaddr, newpath))
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

        target_fd = SysCall::GetParam(lin, 0);
        length = SysCall::GetParam(lin, 1);
  
        host_fd = SysCall::Target2HostFileDescriptor(lin, target_fd);
  
        if(host_fd == -1)
          {
            target_errno = LINUX_EBADF;
            ret = -1;
          }
        else
          {
            ret = ftruncate(host_fd, length);
            if(ret == -1) target_errno = SysCall::HostToLinuxErrno(errno);
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

        mask = SysCall::GetParam(lin, 0);
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
