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

#ifndef __UNISIM_UTIL_OS_LINUX_ARM_HH__
#define __UNISIM_UTIL_OS_LINUX_ARM_HH__

#include <unisim/util/likely/likely.hh>

#include <unisim/util/os/linux_os/errno.hh>

#if defined(WIN32) || defined(WIN64)
#include <process.h>
#include <windows.h>
#else
#include <sys/times.h>
#include <sys/time.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <inttypes.h>

namespace unisim {
namespace util {
namespace os {
namespace linux_os {

  using unisim::kernel::logger::DebugInfo;
  using unisim::kernel::logger::DebugWarning;
  using unisim::kernel::logger::DebugError;
  using unisim::kernel::logger::EndDebugInfo;
  using unisim::kernel::logger::EndDebugWarning;
  using unisim::kernel::logger::EndDebugError;

  // Register names
  static char const* const kARM_r0  = "r0"; /* syscall arg#1 */
  static char const* const kARM_r1  = "r1"; /* syscall arg#2, argc */
  static char const* const kARM_r2  = "r2"; /* syscall arg#3, argv */
  static char const* const kARM_r3  = "r3"; /* syscall arg#4 */
  static char const* const kARM_r4  = "r4"; /* syscall arg#5 */
  static char const* const kARM_r5  = "r5"; /* syscall arg#6 */
  static char const* const kARM_r6  = "r6"; /* syscall arg#7 */
  static char const* const kARM_r7  = "r7"; /* syscall NR */
  static char const* const kARM_sp  = "sp"; /* stack pointer */
  static char const* const kARM_pc  = "pc"; /* programm counter */

  template <class LINUX>
  struct ARMTS : public LINUX::TargetSystem
  {
    // HWCAP flags
    static uint32_t const ARM_HWCAP_ARM_SWP       = 1 << 0;
    static uint32_t const ARM_HWCAP_ARM_HALF      = 1 << 1;
    static uint32_t const ARM_HWCAP_ARM_THUMB     = 1 << 2;
    static uint32_t const ARM_HWCAP_ARM_26BIT     = 1 << 3;
    static uint32_t const ARM_HWCAP_ARM_FAST_MULT = 1 << 4;
    static uint32_t const ARM_HWCAP_ARM_FPA       = 1 << 5;
    static uint32_t const ARM_HWCAP_ARM_VFP       = 1 << 6;
    static uint32_t const ARM_HWCAP_ARM_EDSP      = 1 << 7;
    static uint32_t const ARM_HWCAP_ARM_JAVA      = 1 << 8;
    static uint32_t const ARM_HWCAP_ARM_IWMMXT    = 1 << 9;
    static uint32_t const ARM_HWCAP_ARM_CRUNCH    = 1 << 10;
    static uint32_t const ARM_HWCAP_ARM_THUMBEE   = 1 << 11;
    static uint32_t const ARM_HWCAP_ARM_NEON      = 1 << 12;
    static uint32_t const ARM_HWCAP_ARM_VFPv3     = 1 << 13;
    static uint32_t const ARM_HWCAP_ARM_VFPv3D16  = 1 << 14;
    static uint32_t const ARM_HWCAP_ARM_TLS       = 1 << 15;
    static uint32_t const ARM_HWCAP_ARM_VFPv4     = 1 << 16;
    static uint32_t const ARM_HWCAP_ARM_IDIVA     = 1 << 17;
    static uint32_t const ARM_HWCAP_ARM_IDIVT     = 1 << 18;
    
    // Name imports
    typedef typename LINUX::SysCall SysCall;
    typedef typename LINUX::address_type address_type;
    typedef typename LINUX::parameter_type parameter_type;
    typedef typename LINUX::UTSName UTSName;
    using LINUX::TargetSystem::SetRegister;
    using LINUX::TargetSystem::GetRegister;
    using LINUX::TargetSystem::lin;
    using LINUX::TargetSystem::name;
    
    // ARM linux structs
    struct arm_timespec {
      uint32_t tv_sec;  /* Seconds.      (__time_t) */
      uint32_t tv_nsec; /* Nanoseconds.  (long int) */
    };

    struct arm_stat64
    {
      uint64_t st_dev;             /* Device.                           (__dev_t, @0) */
      uint32_t __pad1;
      uint32_t __st_ino;           /* 32bit file serial number.	    (__ino_t, @12) */
      uint32_t st_mode;            /* File mode.                        (__mode_t, @16) */
      uint32_t st_nlink;           /* Link count.                       (__nlink_t, @20) */
      uint32_t st_uid;             /* User ID of the file's owner.	    (__uid_t, @24) */
      uint32_t st_gid;             /* Group ID of the file's group.     (__gid_t, @28) */
      uint64_t st_rdev;            /* Device number, if device.         (__dev_t, @32) */
      uint64_t __pad2;
      int64_t st_size;             /* Size of file, in bytes.           (__off64_t, @48) */
      uint32_t st_blksize;         /* Optimal block size for I/O.       (__blksize_t, @56) */
      uint32_t __pad3;
      uint64_t st_blocks;          /* Number 512-byte blocks allocated. (__blkcnt64_t, @64) */
      struct arm_timespec st_atim; /* Time of last access.              (struct timespec, @72) */
      struct arm_timespec st_mtim; /* Time of last modification.        (struct timespec, @80) */
      struct arm_timespec st_ctim; /* Time of last status change.       (struct timespec, @88) */
      uint64_t st_ino;             /* File serial number.               (__ino64_t, @96) */
    };

    struct arm_tms
    {
      int32_t tms_utime;          /* User CPU time.                    (clock_t) */
      int32_t tms_stime;          /* System CPU time.                  (clock_t) */
      int32_t tms_cutime;         /* User CPU time of dead children.   (clock_t) */
      int32_t tms_cstime;         /* System CPU time of dead children. (clock_t) */
    };

    struct arm_utsname
    {
      char sysname[65];
      char nodename[65];
      char release[65];
      char version[65];
      char machine[65];
    };

    struct arm_timeval {
      int32_t tv_sec;         /* seconds      (__kernel_time_t) */
      int32_t tv_usec;        /* microseconds (__kernel_suseconds_t) */
    };

    struct arm_timezone {
      int32_t tz_minuteswest; /* minutes west of Greenwich (int) */
      int32_t tz_dsttime;     /* type of dst correction    (int) */
    };
    
    ARMTS( std::string _name, LINUX& _lin ) : LINUX::TargetSystem( _name, _lin ) {}
    
    bool GetAT_HWCAP( address_type& hwcap ) const
    {
      uint32_t arm_hwcap = 0;
      std::string hwcap_token;
      std::stringstream sstr(this->GetHWCAP());
      while(sstr >> hwcap_token)
        {
          if(hwcap_token.compare("swp") == 0)             { arm_hwcap |= ARM_HWCAP_ARM_SWP; }
          else if(hwcap_token.compare("half") == 0)       { arm_hwcap |= ARM_HWCAP_ARM_HALF; }
          else if(hwcap_token.compare("thumb") == 0)      { arm_hwcap |= ARM_HWCAP_ARM_THUMB; }
          else if(hwcap_token.compare("26bit") == 0)      { arm_hwcap |= ARM_HWCAP_ARM_26BIT; }
          else if(hwcap_token.compare("fastmult") == 0)   { arm_hwcap |= ARM_HWCAP_ARM_FAST_MULT; }
          else if(hwcap_token.compare("fpa") == 0)        { arm_hwcap |= ARM_HWCAP_ARM_FPA; }
          else if(hwcap_token.compare("vfp") == 0)        { arm_hwcap |= ARM_HWCAP_ARM_VFP; }
          else if(hwcap_token.compare("edsp") == 0)       { arm_hwcap |= ARM_HWCAP_ARM_EDSP; }
          else if(hwcap_token.compare("edsp") == 0)       { arm_hwcap |= ARM_HWCAP_ARM_JAVA; }
          else if(hwcap_token.compare("java") == 0)       { arm_hwcap |= ARM_HWCAP_ARM_JAVA; }
          else if(hwcap_token.compare("iwmmxt") == 0)     { arm_hwcap |= ARM_HWCAP_ARM_IWMMXT; }
          else if(hwcap_token.compare("crunch") == 0)     { arm_hwcap |= ARM_HWCAP_ARM_CRUNCH; }
          else if(hwcap_token.compare("thumbee") == 0)    { arm_hwcap |= ARM_HWCAP_ARM_THUMBEE; }
          else if(hwcap_token.compare("neon") == 0)       { arm_hwcap |= ARM_HWCAP_ARM_NEON; }
          else if(hwcap_token.compare("vfpv3") == 0)      { arm_hwcap |= ARM_HWCAP_ARM_VFPv3; }
          else if(hwcap_token.compare("vfpv3d16") == 0)   { arm_hwcap |= ARM_HWCAP_ARM_VFPv3D16; }
          else if(hwcap_token.compare("tls") == 0)        { arm_hwcap |= ARM_HWCAP_ARM_TLS; }
          else if(hwcap_token.compare("vfpv4") == 0)      { arm_hwcap |= ARM_HWCAP_ARM_VFPv4; }
          else if(hwcap_token.compare("idiva") == 0)      { arm_hwcap |= ARM_HWCAP_ARM_IDIVA; }
          else if(hwcap_token.compare("idivt") == 0)      { arm_hwcap |= ARM_HWCAP_ARM_IDIVT; }
          else { lin.Logger() << DebugWarning << "unknown hardware capability \"" << hwcap_token << "\"" << EndDebugWarning; }
        }
      hwcap = arm_hwcap;
      return true;
    }
    bool SetupTarget() const
    {
      // Reset all target registers
      {
        struct : public unisim::util::debug::RegisterScanner {
          void Append( unisim::util::debug::Register* reg ) { reg->Clear(); }
        } clear_regs;
        this->RegsIF().ScanRegisters( clear_regs );
      }
      // Set PC to the program entry point
      if (not SetRegister(lin, kARM_pc, lin.GetEntryPoint()))
        return false;
      // Set SP to the base of the created stack
      unisim::util::debug::blob::Section<address_type> const * sp_section =
        lin.GetBlob()->FindSection(".unisim.linux_os.stack.stack_pointer");
      if (sp_section == NULL) {
        lin.Logger() << DebugError << "Could not find the stack pointer section." << EndDebugError;
        return false;
      }
      if (not SetRegister(lin, kARM_sp, sp_section->GetAddr()))
        return false;
      address_type par1_addr = sp_section->GetAddr() + 4;
      address_type par2_addr = sp_section->GetAddr() + 8;
      parameter_type par1 = 0;
      parameter_type par2 = 0;
      if (not this->MemIF().ReadMemory(par1_addr, (uint8_t *)&par1, sizeof(par1)) or
          not this->MemIF().ReadMemory(par2_addr, (uint8_t *)&par2, sizeof(par2)) or
          not SetRegister(lin, kARM_r1, Target2Host(lin.GetEndianness(), par1)) or
          not SetRegister(lin, kARM_r2, Target2Host(lin.GetEndianness(), par2)))
        return false;
          
      return true;
    }
    
    static void SetARMSystemCallStatus(LINUX& _lin, int ret, bool error) { SetRegister(_lin, kARM_r0, (parameter_type) ret); }
    
    void SetSystemCallStatus(int ret, bool error) const { SetARMSystemCallStatus( lin, ret, error ); }
    
    static parameter_type GetSystemCallParam( LINUX& _lin, int id )
    {
      parameter_type val = 0;
          
      switch (id) {
      case 0: GetRegister(_lin, kARM_r0, &val); break;
      case 1: GetRegister(_lin, kARM_r1, &val); break;
      case 2: GetRegister(_lin, kARM_r2, &val); break;
      case 3: GetRegister(_lin, kARM_r3, &val); break;
      case 4: GetRegister(_lin, kARM_r4, &val); break;
      case 5: GetRegister(_lin, kARM_r5, &val); break;
      case 6: GetRegister(_lin, kARM_r6, &val); break;
      default: throw id;
        break;
      }
          
      return val;
    }
    
    parameter_type GetSystemCallParam( int id ) const
    {
      try { return GetSystemCallParam( lin, id ); }
      
      catch (int x) {
        lin.Logger() << DebugError << "No syscall argument #" << id << " in " << this->name << " linux" << EndDebugError;
      }
      
      return 0;
    }

    static int GetTimeOfDay(struct arm_timeval *target_timeval, struct arm_timezone *target_timezone, unisim::util::endian::endian_type endianness)
    {
      int ret;
      struct timeval host_tv;
      struct timezone host_tz;

      ret = gettimeofday(&host_tv, &host_tz);

      if(ret == 0)
	{
          if(target_timeval)
            {
              target_timeval->tv_sec = Host2Target(endianness, (int32_t) host_tv.tv_sec);
              target_timeval->tv_usec = Host2Target(endianness, (int32_t) host_tv.tv_usec);
            }
          if(target_timezone)
            {
              target_timezone->tz_minuteswest = Host2Target(endianness, (int32_t) host_tz.tz_minuteswest);
              target_timezone->tz_dsttime = Host2Target(endianness, (int32_t) host_tz.tz_dsttime);
            }
	}
	
      return ret;
    }

    static int Fstat64(int fd, struct arm_stat64 *target_stat, unisim::util::endian::endian_type endianness)
    {
      int ret;
#if defined(WIN32) || defined(WIN64)
      struct _stati64 host_stat;
      ret = _fstati64(fd, &host_stat);
#elif defined(linux) || defined(__linux) || defined(__linux__)
      struct stat64 host_stat;
      ret = fstat64(fd, &host_stat);
#elif defined(__APPLE_CC__)
      struct stat host_stat;
      ret = fstat(fd, &host_stat);
#endif
      if(ret < 0) return ret;

      memset(target_stat, 0, sizeof(struct arm_stat64));
	
#if defined(__x86_64) || defined(__amd64) || defined(__x86_64__) || defined(__amd64__) || defined(__LP64__) || defined(_LP64)
      // 64-bit host
      target_stat->st_dev = Host2Target(endianness, (uint64_t) host_stat.st_dev);
      target_stat->__st_ino = Host2Target(endianness, (uint32_t) host_stat.st_ino);
      target_stat->st_ino = Host2Target(endianness, (uint64_t) host_stat.st_ino);
      target_stat->st_mode = Host2Target(endianness, (uint32_t) host_stat.st_mode);
      target_stat->st_nlink = Host2Target(endianness, (uint64_t) host_stat.st_nlink);
      target_stat->st_uid = Host2Target(endianness, (uint32_t) host_stat.st_uid);
      target_stat->st_gid = Host2Target(endianness, (uint32_t) host_stat.st_gid);
      target_stat->st_rdev = Host2Target(endianness, (uint64_t) host_stat.st_rdev);
      target_stat->st_size = Host2Target(endianness, (int64_t) host_stat.st_size);
#if defined(WIN64) // Windows x64
      target_stat->st_blksize = Host2Target(endianness, (int32_t) 512);
      target_stat->st_blocks = Host2Target(endianness, (uint64_t)((host_stat.st_size + 511) / 512));
      target_stat->st_atim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_atime);
      target_stat->st_atim.tv_nsec = 0;
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_mtime);
      target_stat->st_mtim.tv_nsec = 0;
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_ctime);
      target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) || defined(__linux) || defined(__linux__) // Linux x64
      target_stat->st_blksize = Host2Target(endianness, (uint32_t) host_stat.st_blksize);
      target_stat->st_blocks = Host2Target(endianness, (uint64_t) host_stat.st_blocks);
      target_stat->st_atim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_atim.tv_sec);
      target_stat->st_atim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_atim.tv_nsec);
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_mtim.tv_sec);
      target_stat->st_mtim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_mtim.tv_nsec);
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_ctim.tv_sec);
      target_stat->st_ctim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // darwin PPC64/x86_64
      target_stat->st_blksize = Host2Target(endianness, (uint32_t) host_stat.st_blksize);
      target_stat->st_blocks = Host2Target(endianness, (int64_t) host_stat.st_blocks);
      target_stat->st_atim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_atimespec.tv_sec);
      target_stat->st_atim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_atimespec.tv_nsec);
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_mtimespec.tv_sec);
      target_stat->st_mtim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_mtimespec.tv_nsec);
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_ctimespec.tv_sec);
      target_stat->st_ctim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_ctimespec.tv_nsec);
#endif

#else
      // 32-bit host
      target_stat->st_dev = Host2Target(endianness, (uint64_t) host_stat.st_dev);
      target_stat->__st_ino = Host2Target(endianness, (uint32_t) host_stat.st_ino);
      target_stat->st_ino = Host2Target(endianness, (uint64_t) host_stat.st_ino);
      target_stat->st_mode = Host2Target(endianness, (uint32_t) host_stat.st_mode);
      target_stat->st_nlink = Host2Target(endianness, (uint32_t) host_stat.st_nlink);
      target_stat->st_uid = Host2Target(endianness, (uint32_t) host_stat.st_uid);
      target_stat->st_gid = Host2Target(endianness, (uint32_t) host_stat.st_gid);
      target_stat->st_rdev = Host2Target(endianness, (uint64_t) host_stat.st_rdev);
      target_stat->st_size = Host2Target(endianness, (int64_t) host_stat.st_size);
#if defined(WIN32) // Windows 32
      target_stat->st_blksize = Host2Target(endianness, (int32_t) 512);
      target_stat->st_blocks = Host2Target(endianness, (int64_t)((host_stat.st_size + 511) / 512));
      target_stat->st_atim.tv_sec = Host2Target(endianness, (int32_t) host_stat.st_atime);
      target_stat->st_atim.tv_nsec = 0;
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (int32_t) host_stat.st_mtime);
      target_stat->st_mtim.tv_nsec = 0;
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (int32_t) host_stat.st_ctime);
      target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) || defined(__linux) || defined(__linux__) // Linux 32
      target_stat->st_blksize = Host2Target(endianness, (uint32_t) host_stat.st_blksize);
      target_stat->st_blocks = Host2Target(endianness, (uint64_t) host_stat.st_blocks);
      target_stat->st_atim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_atim.tv_sec);
      target_stat->st_atim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_atim.tv_nsec);
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_mtim.tv_sec);
      target_stat->st_mtim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_mtim.tv_nsec);
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_ctim.tv_sec);
      target_stat->st_ctim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // Darwin PPC32/x86
      target_stat->st_blksize = Host2Target(endianness, (uint32_t) host_stat.st_blksize);
      target_stat->st_blocks = Host2Target(endianness, (uint64_t) host_stat.st_blocks);
      target_stat->st_atim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_atimespec.tv_sec);
      target_stat->st_atim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_atimespec.tv_nsec);
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_mtimespec.tv_sec);
      target_stat->st_mtim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_mtimespec.tv_nsec);
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_ctimespec.tv_sec);
      target_stat->st_ctim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_ctimespec.tv_nsec);
#endif

#endif
      return ret;
    }

    static int Stat64(const char *pathname, struct arm_stat64* target_stat, unisim::util::endian::endian_type endianness)
    {
      int ret;
#if defined(WIN32) || defined(WIN64)
      struct _stati64 host_stat;
      ret = _stati64(pathname, &host_stat);
#elif defined(linux) || defined(__linux) || defined(__linux__)
      struct stat64 host_stat;
      ret = stat64(pathname, &host_stat);
#elif defined(__APPLE_CC__)
      struct stat host_stat;
      ret = stat(pathname, &host_stat);
#endif
      if(ret < 0) return ret;

      memset(target_stat, 0, sizeof(struct arm_stat64));
	
#if defined(__x86_64) || defined(__amd64) || defined(__x86_64__) || defined(__amd64__) || defined(__LP64__) || defined(_LP64)
      // 64-bit host
      target_stat->st_dev = Host2Target(endianness, (uint64_t) host_stat.st_dev);
      target_stat->__st_ino = Host2Target(endianness, (uint32_t) host_stat.st_ino);
      target_stat->st_ino = Host2Target(endianness, (uint64_t) host_stat.st_ino);
      target_stat->st_mode = Host2Target(endianness, (uint32_t) host_stat.st_mode);
      target_stat->st_nlink = Host2Target(endianness, (uint64_t) host_stat.st_nlink);
      target_stat->st_uid = Host2Target(endianness, (uint32_t) host_stat.st_uid);
      target_stat->st_gid = Host2Target(endianness, (uint32_t) host_stat.st_gid);
      target_stat->st_rdev = Host2Target(endianness, (uint64_t) host_stat.st_rdev);
      target_stat->st_size = Host2Target(endianness, (int64_t) host_stat.st_size);
#if defined(WIN64) // Windows x64
      target_stat->st_blksize = Host2Target(endianness, (int32_t) 512);
      target_stat->st_blocks = Host2Target(endianness, (uint64_t)((host_stat.st_size + 511) / 512));
      target_stat->st_atim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_atime);
      target_stat->st_atim.tv_nsec = 0;
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_mtime);
      target_stat->st_mtim.tv_nsec = 0;
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_ctime);
      target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) || defined(__linux) || defined(__linux__) // Linux x64
      target_stat->st_blksize = Host2Target(endianness, (uint32_t) host_stat.st_blksize);
      target_stat->st_blocks = Host2Target(endianness, (uint64_t) host_stat.st_blocks);
      target_stat->st_atim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_atim.tv_sec);
      target_stat->st_atim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_atim.tv_nsec);
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_mtim.tv_sec);
      target_stat->st_mtim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_mtim.tv_nsec);
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_ctim.tv_sec);
      target_stat->st_ctim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // darwin PPC64/x86_64
      target_stat->st_blksize = Host2Target(endianness, (uint32_t) host_stat.st_blksize);
      target_stat->st_blocks = Host2Target(endianness, (int64_t) host_stat.st_blocks);
      target_stat->st_atim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_atimespec.tv_sec);
      target_stat->st_atim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_atimespec.tv_nsec);
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_mtimespec.tv_sec);
      target_stat->st_mtim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_mtimespec.tv_nsec);
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_ctimespec.tv_sec);
      target_stat->st_ctim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_ctimespec.tv_nsec);
#endif

#else
      // 32-bit host
      target_stat->st_dev = Host2Target(endianness, (uint64_t) host_stat.st_dev);
      target_stat->__st_ino = Host2Target(endianness, (uint32_t) host_stat.st_ino);
      target_stat->st_ino = Host2Target(endianness, (uint64_t) host_stat.st_ino);
      target_stat->st_mode = Host2Target(endianness, (uint32_t) host_stat.st_mode);
      target_stat->st_nlink = Host2Target(endianness, (uint32_t) host_stat.st_nlink);
      target_stat->st_uid = Host2Target(endianness, (uint32_t) host_stat.st_uid);
      target_stat->st_gid = Host2Target(endianness, (uint32_t) host_stat.st_gid);
      target_stat->st_rdev = Host2Target(endianness, (uint64_t) host_stat.st_rdev);
      target_stat->st_size = Host2Target(endianness, (int64_t) host_stat.st_size);
#if defined(WIN32) // Windows 32
      target_stat->st_blksize = Host2Target(endianness, (int32_t) 512);
      target_stat->st_blocks = Host2Target(endianness, (int64_t)((host_stat.st_size + 511) / 512));
      target_stat->st_atim.tv_sec = Host2Target(endianness, (int32_t) host_stat.st_atime);
      target_stat->st_atim.tv_nsec = 0;
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (int32_t) host_stat.st_mtime);
      target_stat->st_mtim.tv_nsec = 0;
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (int32_t) host_stat.st_ctime);
      target_stat->st_ctim.tv_nsec = 0;
#elif defined(linux) || defined(__linux) || defined(__linux__) // Linux 32
      target_stat->st_blksize = Host2Target(endianness, (uint32_t) host_stat.st_blksize);
      target_stat->st_blocks = Host2Target(endianness, (uint64_t) host_stat.st_blocks);
      target_stat->st_atim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_atim.tv_sec);
      target_stat->st_atim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_atim.tv_nsec);
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_mtim.tv_sec);
      target_stat->st_mtim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_mtim.tv_nsec);
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_ctim.tv_sec);
      target_stat->st_ctim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_ctim.tv_nsec);
#elif defined(__APPLE_CC__) // Darwin PPC32/x86
      target_stat->st_blksize = Host2Target(endianness, (uint32_t) host_stat.st_blksize);
      target_stat->st_blocks = Host2Target(endianness, (uint64_t) host_stat.st_blocks);
      target_stat->st_atim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_atimespec.tv_sec);
      target_stat->st_atim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_atimespec.tv_nsec);
      target_stat->st_mtim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_mtimespec.tv_sec);
      target_stat->st_mtim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_mtimespec.tv_nsec);
      target_stat->st_ctim.tv_sec = Host2Target(endianness, (uint32_t) host_stat.st_ctimespec.tv_sec);
      target_stat->st_ctim.tv_nsec = Host2Target(endianness, (uint32_t) host_stat.st_ctimespec.tv_nsec);
#endif

#endif
      return ret;
    }


    static int Times(struct arm_tms* target_tms, unisim::util::endian::endian_type endianness)
    {
      int ret;
#if defined(WIN32) || defined(WIN64)
      FILETIME ftCreationTime;
      FILETIME ftExitTime;
      FILETIME ftKernelTime;
      FILETIME ftUserTime;

      if(GetProcessTimes(GetCurrentProcess(), &ftCreationTime, &ftExitTime, &ftKernelTime, &ftUserTime)) return -1;

      target_tms->tms_utime = Host2Target(endianness, (uint32_t) ftUserTime.dwLowDateTime);
      target_tms->tms_stime = Host2Target(endianness, (uint32_t) ftKernelTime.dwLowDateTime);
      target_tms->tms_cutime = 0;   // User CPU time of dead children
      target_tms->tms_cstime = 0;   // System CPU time of dead children
#else
      struct tms host_tms;

      ret = (int) times(&host_tms);
      target_tms->tms_utime = Host2Target(endianness, (int32_t) host_tms.tms_utime);
      target_tms->tms_stime = Host2Target(endianness, (int32_t) host_tms.tms_stime);
      target_tms->tms_cutime = Host2Target(endianness, (int32_t) host_tms.tms_cutime);
      target_tms->tms_cstime = Host2Target(endianness, (int32_t) host_tms.tms_cstime);
#endif
      return ret;
    }

    SysCall* GetSystemCall( int& id ) const
    {
      if (name.compare("arm-eabi") == 0)
        {
          // The arm eabi ignores the supplied id and uses register 7
          parameter_type id_from_reg;
          if (not GetRegister(lin, kARM_r7, &id_from_reg))
            return 0;
          id = int(id_from_reg);
        }
      else if (name.compare("arm") == 0)
        {
          id -= 0x0900000; // Offset translation
        }
          
      // see either arch/arm/include/asm/unistd.h or arch/arm/include/uapi/asm/unistd.h in Linux source
      switch (id) {
      case 0: return this->GetSyscallByName("restart_syscall");
      case 1: return this->GetSyscallByName("exit");
      case 2: return this->GetSyscallByName("fork");
      case 3: return this->GetSyscallByName("read");
      case 4: return this->GetSyscallByName("write");
      case 5: return this->GetSyscallByName("open");
      case 6: return this->GetSyscallByName("close");
        // 7 was "waitpid"
      case 8: return this->GetSyscallByName("creat");
      case 9: return this->GetSyscallByName("link");
      case 10: return this->GetSyscallByName("unlink");
      case 11: return this->GetSyscallByName("execve");
      case 12: return this->GetSyscallByName("chdir");
      case 13: return this->GetSyscallByName("time");
      case 14: return this->GetSyscallByName("mknod");
      case 15: return this->GetSyscallByName("chmod");
      case 16: return this->GetSyscallByName("lchown");
        // 17 was "break"
        // 18 was "stat"
      case 19: return this->GetSyscallByName("lseek");
      case 20: return this->GetSyscallByName("getpid");
      case 21: return this->GetSyscallByName("mount");
      case 22: return this->GetSyscallByName("umount");
      case 23: return this->GetSyscallByName("setuid");
      case 24: return this->GetSyscallByName("getuid");
      case 25: return this->GetSyscallByName("stime");
      case 26: return this->GetSyscallByName("ptrace");
      case 27: return this->GetSyscallByName("alarm");
        // 28 was "fstat"
      case 29: return this->GetSyscallByName("pause");
      case 30: return this->GetSyscallByName("utime");
        // 31 was "stty"
        // 32 was "gtty"
      case 33: return this->GetSyscallByName("access");
      case 34: return this->GetSyscallByName("nice");
        // 35 was "ftime"
      case 36: return this->GetSyscallByName("sync");
      case 37: return this->GetSyscallByName("kill");
      case 38: return this->GetSyscallByName("rename");
      case 39: return this->GetSyscallByName("mkdir");
      case 40: return this->GetSyscallByName("rmdir");
      case 41: return this->GetSyscallByName("dup");
      case 42: return this->GetSyscallByName("pipe");
        // 43: times (see arm specific)
        // 44 was "prof"
      case 45: return this->GetSyscallByName("brk");
      case 46: return this->GetSyscallByName("setgid");
      case 47: return this->GetSyscallByName("getgid");
        // 48 was "signal"
      case 49: return this->GetSyscallByName("geteuid");
      case 50: return this->GetSyscallByName("getegid");
      case 51: return this->GetSyscallByName("acct");
      case 52: return this->GetSyscallByName("umount2");
        // 53 was "lock"
      case 54: return this->GetSyscallByName("ioctl");
      case 55: return this->GetSyscallByName("fcntl");
        // 56 was "mpx"
      case 57: return this->GetSyscallByName("setpgid");
        // 58 was "ulimit"
        // 59 was "olduname"
      case 60: return this->GetSyscallByName("umask");
      case 61: return this->GetSyscallByName("chroot");
      case 62: return this->GetSyscallByName("ustat");
      case 63: return this->GetSyscallByName("dup2");
      case 64: return this->GetSyscallByName("getppid");
      case 65: return this->GetSyscallByName("getpgrp");
      case 66: return this->GetSyscallByName("setsid");
      case 67: return this->GetSyscallByName("sigaction");
        // 68 was "sgetmask"
        // 69 was "ssetmask"
      case 70: return this->GetSyscallByName("setreuid");
      case 71: return this->GetSyscallByName("setregid");
      case 72: return this->GetSyscallByName("sigsuspend");
      case 73: return this->GetSyscallByName("sigpending");
      case 74: return this->GetSyscallByName("sethostname");
      case 75: return this->GetSyscallByName("setrlimit");
      case 76: return this->GetSyscallByName("getrlimit");
      case 77: return this->GetSyscallByName("getrusage");
        // 78: gettimeofday (see arm specific)
      case 79: return this->GetSyscallByName("settimeofday");
      case 80: return this->GetSyscallByName("getgroups");
      case 81: return this->GetSyscallByName("setgroups");
      case 82: return this->GetSyscallByName("select");
      case 83: return this->GetSyscallByName("symlink");
        // 84 was "lstat"
      case 85: return this->GetSyscallByName("readlink");
      case 86: return this->GetSyscallByName("uselib");
      case 87: return this->GetSyscallByName("swapon");
      case 88: return this->GetSyscallByName("reboot");
      case 89: return this->GetSyscallByName("readdir");
      case 90: return this->GetSyscallByName("mmap");
      case 91: return this->GetSyscallByName("munmap");
      case 92: return this->GetSyscallByName("truncate");
      case 93: return this->GetSyscallByName("ftruncate");
      case 94: return this->GetSyscallByName("fchmod");
      case 95: return this->GetSyscallByName("fchown");
      case 96: return this->GetSyscallByName("getpriority");
      case 97: return this->GetSyscallByName("setpriority");
        // 98 was "profile"
      case 99: return this->GetSyscallByName("statfs");
      case 100: return this->GetSyscallByName("fstatfs");
      case 102: return this->GetSyscallByName("socketcall");
      case 103: return this->GetSyscallByName("syslog");
      case 104: return this->GetSyscallByName("setitimer");
      case 105: return this->GetSyscallByName("getitimer");
      case 106: return this->GetSyscallByName("stat");
      case 107: return this->GetSyscallByName("lstat");
        // 108: fstat (see arm specific)
        // 109 was "uname"
        // 110 was "iopl"
      case 111: return this->GetSyscallByName("vhangup");
        // 112 was "idle"
      case 113: return this->GetSyscallByName("syscall");
      case 114: return this->GetSyscallByName("wait4");
      case 115: return this->GetSyscallByName("swapoff");
      case 116: return this->GetSyscallByName("sysinfo");
      case 117: return this->GetSyscallByName("ipc");
      case 118: return this->GetSyscallByName("fsync");
      case 119: return this->GetSyscallByName("sigreturn");
      case 120: return this->GetSyscallByName("clone");
      case 121: return this->GetSyscallByName("setdomainname");
        // 122: uname (see arm specific)
        // 123 was "modify_ldt"
      case 124: return this->GetSyscallByName("adjtimex");
      case 125: return this->GetSyscallByName("mprotect");
      case 126: return this->GetSyscallByName("sigprocmask");
        // 127 was "create_module"
      case 128: return this->GetSyscallByName("init_module");
      case 129: return this->GetSyscallByName("delete_module");
        // 130 was "get_kernel_syms"
      case 131: return this->GetSyscallByName("quotactl");
      case 132: return this->GetSyscallByName("getpgid");
      case 133: return this->GetSyscallByName("fchdir");
      case 134: return this->GetSyscallByName("bdflush");
      case 135: return this->GetSyscallByName("sysfs");
      case 136: return this->GetSyscallByName("personality");
        // 137 was "afs_syscall"
      case 138: return this->GetSyscallByName("setfsuid");
      case 139: return this->GetSyscallByName("setfsgid");
      case 140: return this->GetSyscallByName("_llseek");
      case 141: return this->GetSyscallByName("getdents");
      case 142: return this->GetSyscallByName("_newselect");
      case 143: return this->GetSyscallByName("flock");
      case 144: return this->GetSyscallByName("msync");
      case 145: return this->GetSyscallByName("readv");
      case 146: return this->GetSyscallByName("writev");
      case 147: return this->GetSyscallByName("getsid");
      case 148: return this->GetSyscallByName("fdatasync");
      case 149: return this->GetSyscallByName("_sysctl");
      case 150: return this->GetSyscallByName("mlock");
      case 151: return this->GetSyscallByName("munlock");
      case 152: return this->GetSyscallByName("mlockall");
      case 153: return this->GetSyscallByName("munlockall");
      case 154: return this->GetSyscallByName("sched_setparam");
      case 155: return this->GetSyscallByName("sched_getparam");
      case 156: return this->GetSyscallByName("sched_setscheduler");
      case 157: return this->GetSyscallByName("sched_getscheduler");
      case 158: return this->GetSyscallByName("sched_yield");
      case 159: return this->GetSyscallByName("sched_get_priority_max");
      case 160: return this->GetSyscallByName("sched_get_priority_min");
      case 161: return this->GetSyscallByName("sched_rr_get_interval");
      case 162: return this->GetSyscallByName("nanosleep");
      case 163: return this->GetSyscallByName("mremap");
      case 164: return this->GetSyscallByName("setresuid");
      case 165: return this->GetSyscallByName("getresuid");
        // 166 was "vm86"
        // 167 was "query_module"
      case 168: return this->GetSyscallByName("poll");
      case 169: return this->GetSyscallByName("nfsservctl");
      case 170: return this->GetSyscallByName("setresgid");
      case 171: return this->GetSyscallByName("getresgid");
      case 172: return this->GetSyscallByName("prctl");
      case 173: return this->GetSyscallByName("rt_sigreturn");
      case 174: return this->GetSyscallByName("rt_sigaction");
      case 175: return this->GetSyscallByName("rt_sigprocmask");
      case 176: return this->GetSyscallByName("rt_sigpending");
      case 177: return this->GetSyscallByName("rt_sigtimedwait");
      case 178: return this->GetSyscallByName("rt_sigqueueinfo");
      case 179: return this->GetSyscallByName("rt_sigsuspend");
      case 180: return this->GetSyscallByName("pread64");
      case 181: return this->GetSyscallByName("pwrite64");
      case 182: return this->GetSyscallByName("chown");
      case 183: return this->GetSyscallByName("getcwd");
      case 184: return this->GetSyscallByName("capget");
      case 185: return this->GetSyscallByName("capset");
      case 186: return this->GetSyscallByName("sigaltstack");
      case 187: return this->GetSyscallByName("sendfile");
        // 188 is reserved
        // 189 is reserved
      case 190: return this->GetSyscallByName("vfork");
      case 191: return this->GetSyscallByName("ugetrlimit");
      case 192: return this->GetSyscallByName("mmap2");
      case 193: return this->GetSyscallByName("truncate64");
      case 194: return this->GetSyscallByName("ftruncate64");
        // 195: stat64 (see arm specific)
      case 196: return this->GetSyscallByName("lstat64");
        // 197: fstat64 (see arm specific)  
      case 198: return this->GetSyscallByName("lchown32");
      case 199: return this->GetSyscallByName("getuid32");
      case 200: return this->GetSyscallByName("getgid32");
      case 201: return this->GetSyscallByName("geteuid32");
      case 202: return this->GetSyscallByName("getegid32");
      case 203: return this->GetSyscallByName("setreuid32");
      case 204: return this->GetSyscallByName("setregid32");
      case 205: return this->GetSyscallByName("getgroups32");
      case 206: return this->GetSyscallByName("setgroups32");
      case 207: return this->GetSyscallByName("fchown32");
      case 208: return this->GetSyscallByName("setresuid32");
      case 209: return this->GetSyscallByName("getresuid32");
      case 210: return this->GetSyscallByName("setresgid32");
      case 211: return this->GetSyscallByName("getresgid32");
      case 212: return this->GetSyscallByName("chown32");
      case 213: return this->GetSyscallByName("setuid32");
      case 214: return this->GetSyscallByName("setgid32");
      case 215: return this->GetSyscallByName("setfsuid32");
      case 216: return this->GetSyscallByName("setfsgid32");
      case 217: return this->GetSyscallByName("getdents64");
      case 218: return this->GetSyscallByName("pivot_root");
      case 219: return this->GetSyscallByName("mincore");
      case 220: return this->GetSyscallByName("madvise");
      case 221: return this->GetSyscallByName("fcntl64");
        // 222 is for tux
        // 223 is unused
      case 224: return this->GetSyscallByName("gettid");
      case 225: return this->GetSyscallByName("readahead");
      case 226: return this->GetSyscallByName("setxattr");
      case 227: return this->GetSyscallByName("lsetxattr");
      case 228: return this->GetSyscallByName("fsetxattr");
      case 229: return this->GetSyscallByName("getxattr");
      case 230: return this->GetSyscallByName("lgetxattr");
      case 231: return this->GetSyscallByName("fgetxattr");
      case 232: return this->GetSyscallByName("listxattr");
      case 233: return this->GetSyscallByName("llistxattr");
      case 234: return this->GetSyscallByName("flistxattr");
      case 235: return this->GetSyscallByName("removexattr");
      case 236: return this->GetSyscallByName("lremovexattr");
      case 237: return this->GetSyscallByName("fremovexattr");
      case 238: return this->GetSyscallByName("tkill");
      case 239: return this->GetSyscallByName("sendfile64");
      case 240: return this->GetSyscallByName("futex");
      case 241: return this->GetSyscallByName("sched_setaffinity");
      case 242: return this->GetSyscallByName("sched_getaffinity");
      case 243: return this->GetSyscallByName("io_setup");
      case 244: return this->GetSyscallByName("io_destroy");
      case 245: return this->GetSyscallByName("io_getevents");
      case 246: return this->GetSyscallByName("io_submit");
      case 247: return this->GetSyscallByName("io_cancel");
      case 248: return this->GetSyscallByName("exit_group");
      case 249: return this->GetSyscallByName("lookup_dcookie");
      case 250: return this->GetSyscallByName("epoll_create");
      case 251: return this->GetSyscallByName("epoll_ctl");
      case 252: return this->GetSyscallByName("epoll_wait");
      case 253: return this->GetSyscallByName("remap_file_pages");
        // 254 is for set_thread_area
        // 255 is for get_thread_area
      case 256: return this->GetSyscallByName("set_tid_address");
      case 257: return this->GetSyscallByName("timer_create");
      case 258: return this->GetSyscallByName("timer_settime");
      case 259: return this->GetSyscallByName("timer_gettime");
      case 260: return this->GetSyscallByName("timer_getoverrun");
      case 261: return this->GetSyscallByName("timer_delete");
      case 262: return this->GetSyscallByName("clock_settime");
      case 263: return this->GetSyscallByName("clock_gettime");
      case 264: return this->GetSyscallByName("clock_getres");
      case 265: return this->GetSyscallByName("clock_nanosleep");
      case 266: return this->GetSyscallByName("statfs64");
      case 267: return this->GetSyscallByName("fstatfs64");
      case 268: return this->GetSyscallByName("tgkill");
      case 269: return this->GetSyscallByName("utimes");
      case 270: return this->GetSyscallByName("arm_fadvise64_64");
      case 271: return this->GetSyscallByName("pciconfig_iobase");
      case 272: return this->GetSyscallByName("pciconfig_read");
      case 273: return this->GetSyscallByName("pciconfig_write");
      case 274: return this->GetSyscallByName("mq_open");
      case 275: return this->GetSyscallByName("mq_unlink");
      case 276: return this->GetSyscallByName("mq_timedsend");
      case 277: return this->GetSyscallByName("mq_timedreceive");
      case 278: return this->GetSyscallByName("mq_notify");
      case 279: return this->GetSyscallByName("mq_getsetattr");
      case 280: return this->GetSyscallByName("waitid");
      case 281: return this->GetSyscallByName("socket");
      case 282: return this->GetSyscallByName("bind");
      case 283: return this->GetSyscallByName("connect");
      case 284: return this->GetSyscallByName("listen");
      case 285: return this->GetSyscallByName("accept");
      case 286: return this->GetSyscallByName("getsockname");
      case 287: return this->GetSyscallByName("getpeername");
      case 288: return this->GetSyscallByName("socketpair");
      case 289: return this->GetSyscallByName("send");
      case 290: return this->GetSyscallByName("sendto");
      case 291: return this->GetSyscallByName("recv");
      case 292: return this->GetSyscallByName("recvfrom");
      case 293: return this->GetSyscallByName("shutdown");
      case 294: return this->GetSyscallByName("setsockopt");
      case 295: return this->GetSyscallByName("getsockopt");
      case 296: return this->GetSyscallByName("sendmsg");
      case 297: return this->GetSyscallByName("recvmsg");
      case 298: return this->GetSyscallByName("semop");
      case 299: return this->GetSyscallByName("semget");
      case 300: return this->GetSyscallByName("semctl");
      case 301: return this->GetSyscallByName("msgsnd");
      case 302: return this->GetSyscallByName("msgrcv");
      case 303: return this->GetSyscallByName("msgget");
      case 304: return this->GetSyscallByName("msgctl");
      case 305: return this->GetSyscallByName("shmat");
      case 306: return this->GetSyscallByName("shmdt");
      case 307: return this->GetSyscallByName("shmget");
      case 308: return this->GetSyscallByName("shmctl");
      case 309: return this->GetSyscallByName("add_key");
      case 310: return this->GetSyscallByName("request_key");
      case 311: return this->GetSyscallByName("keyctl");
      case 312: return this->GetSyscallByName("semtimedop");
      case 313: return this->GetSyscallByName("vserver");
      case 314: return this->GetSyscallByName("ioprio_set");
      case 315: return this->GetSyscallByName("ioprio_get");
      case 316: return this->GetSyscallByName("inotify_init");
      case 317: return this->GetSyscallByName("inotify_add_watch");
      case 318: return this->GetSyscallByName("inotify_rm_watch");
      case 319: return this->GetSyscallByName("mbind");
      case 320: return this->GetSyscallByName("get_mempolicy");
      case 321: return this->GetSyscallByName("set_mempolicy");
      case 322: return this->GetSyscallByName("openat");
      case 323: return this->GetSyscallByName("mkdirat");
      case 324: return this->GetSyscallByName("mknodat");
      case 325: return this->GetSyscallByName("fchownat");
      case 326: return this->GetSyscallByName("futimesat");
      case 327: return this->GetSyscallByName("fstatat64");
      case 328: return this->GetSyscallByName("unlinkat");
      case 329: return this->GetSyscallByName("renameat");
      case 330: return this->GetSyscallByName("linkat");
      case 331: return this->GetSyscallByName("symlinkat");
      case 332: return this->GetSyscallByName("readlinkat");
      case 333: return this->GetSyscallByName("fchmodat");
      case 334: return this->GetSyscallByName("faccessat");
      case 335: return this->GetSyscallByName("pselect6");
      case 336: return this->GetSyscallByName("ppoll");
      case 337: return this->GetSyscallByName("unshare");
      case 338: return this->GetSyscallByName("set_robust_list");
      case 339: return this->GetSyscallByName("get_robust_list");
      case 340: return this->GetSyscallByName("splice");
      case 341: return this->GetSyscallByName("sync_file_range2");
      case 342: return this->GetSyscallByName("tee");
      case 343: return this->GetSyscallByName("vmsplice");
      case 344: return this->GetSyscallByName("move_pages");
      case 345: return this->GetSyscallByName("getcpu");
      case 346: return this->GetSyscallByName("epoll_pwait");
      case 347: return this->GetSyscallByName("kexec_load");
      case 348: return this->GetSyscallByName("utimensat");
      case 349: return this->GetSyscallByName("signalfd");
      case 350: return this->GetSyscallByName("timerfd_create");
      case 351: return this->GetSyscallByName("eventfd");
      case 352: return this->GetSyscallByName("fallocate");
      case 353: return this->GetSyscallByName("timerfd_settime");
      case 354: return this->GetSyscallByName("timerfd_gettime");
      case 355: return this->GetSyscallByName("signalfd4");
      case 356: return this->GetSyscallByName("eventfd2");
      case 357: return this->GetSyscallByName("epoll_create1");
      case 358: return this->GetSyscallByName("dup3");
      case 359: return this->GetSyscallByName("pipe2");
      case 360: return this->GetSyscallByName("inotify_init1");
      case 361: return this->GetSyscallByName("preadv");
      case 362: return this->GetSyscallByName("pwritev");
      case 363: return this->GetSyscallByName("rt_tgsigqueueinfo");
      case 364: return this->GetSyscallByName("perf_event_open");
      case 365: return this->GetSyscallByName("recvmmsg");
      case 366: return this->GetSyscallByName("accept4");
      case 367: return this->GetSyscallByName("fanotify_init");
      case 368: return this->GetSyscallByName("fanotify_mark");
      case 369: return this->GetSyscallByName("prlimit64");
      case 370: return this->GetSyscallByName("name_to_handle_at");
      case 371: return this->GetSyscallByName("open_by_handle_at");
      case 372: return this->GetSyscallByName("clock_adjtime");
      case 373: return this->GetSyscallByName("syncfs");
      case 374: return this->GetSyscallByName("sendmmsg");
      case 375: return this->GetSyscallByName("setns");
      case 376: return this->GetSyscallByName("process_vm_readv");
      case 377: return this->GetSyscallByName("process_vm_writev");
        // 378 is for kcmp
	
        // the following are private to the arm
      case 43: /* ARM specific times syscall */
        {
          static struct : public SysCall {
            char const* GetName() const { return "times"; }
            void Execute( LINUX& lin, int syscall_id ) const
            {
              int ret;
              address_type buf_addr;
              int32_t target_errno = 0;
                  
              buf_addr = GetSystemCallParam(lin, 0);
                  
              struct arm_tms target_tms;
              ret = Times(&target_tms, lin.GetEndianness());

              if(ret >= 0)
                {
                  this->WriteMem(lin, buf_addr, (uint8_t *)&target_tms, sizeof(target_tms));
                }
              else
                {
                  target_errno = this->Host2LinuxErrno(lin, errno);
                }
                  
              if(unlikely(lin.GetVerbose()))
                lin.Logger() << DebugInfo << "times(buf=0x" << std::hex << buf_addr << std::dec << ")" << EndDebugInfo;
	
              SetARMSystemCallStatus(lin, (ret == -1) ? -target_errno : ret, (ret == -1));
            }
          } sc;
          return &sc;
        } break;
            
      case 78: /* ARM specific gettimeofday syscall */
        {
          static struct : public SysCall {
            char const* GetName() const { return "gettimeofday"; }
            void Execute( LINUX& lin, int syscall_id ) const
            {
              int ret = -1;
              address_type tv_addr;
              address_type tz_addr;
              int32_t target_errno = 0;
              tv_addr = GetSystemCallParam(lin, 0);
              tz_addr = GetSystemCallParam(lin, 1);

              struct arm_timeval target_tv;
              struct arm_timezone target_tz;

              ret = GetTimeOfDay(tv_addr ? &target_tv : 0, tz_addr ? &target_tz : 0, lin.GetEndianness());
              if(ret == -1) target_errno = this->Host2LinuxErrno(lin, errno);

              if(ret == 0)
                {
                  if(tv_addr)
                    {
                      this->WriteMem(lin, tv_addr, (const uint8_t *) &target_tv, sizeof(target_tv));
                    }
                  if(tz_addr)
                    {
                      this->WriteMem(lin, tz_addr, (const uint8_t *) &target_tz, sizeof(target_tz));
                    }
                }

              if(unlikely(lin.GetVerbose()))
                {
                  lin.Logger() << DebugInfo
                               << "gettimeofday(tv = 0x" << std::hex << tv_addr << std::dec
                               << ", tz = 0x" << std::hex << tz_addr << std::dec << ")"
                               << EndDebugInfo;
                }
	
              SetARMSystemCallStatus(lin, (parameter_type) (ret == -1) ? -target_errno : ret, (ret == -1));
            }
          } sc;
          return &sc;
        } break;

      case 108: /* ARM specific fstat syscall */
        {
          static struct : public SysCall {
            char const* GetName() const { return "fstat"; }
            void Execute( LINUX& lin, int syscall_id ) const
            {
              int ret;
              int32_t target_fd;
              int host_fd;
              address_type buf_address;
              int32_t target_errno = 0;

              target_fd = GetSystemCallParam(lin, 0);
              buf_address = GetSystemCallParam(lin, 1);
	
              host_fd = SysCall::Target2HostFileDescriptor(lin, target_fd);

              if(host_fd == -1)
                {
                  target_errno = LINUX_EBADF;
                  ret = -1;
                }
              else
                {
                  ret = -1;
                  target_errno = LINUX_ENOSYS;
                }

              if(unlikely(lin.GetVerbose()))
                {
                  lin.Logger() << DebugInfo
                               << "fstat(fd=" << target_fd
                               << ", buf_addr=0x" << std::hex << buf_address << std::dec
                               << ")" << EndDebugInfo;
                }
	
              SetARMSystemCallStatus(lin, (ret == -1) ? -target_errno : ret, (ret == -1));
            }
          } sc;
          return &sc;
        } break;

      case 122: /* ARM specific uname syscall */
        {
          static struct : public SysCall {
            char const* GetName() const { return "uname"; }
            void Execute( LINUX& lin, int syscall_id ) const
            {
              int ret;
              int32_t target_errno = 0;
              address_type buf_addr = GetSystemCallParam(lin, 0);
              ret = 0;
	
              struct arm_utsname value;
              memset(&value, 0, sizeof(value));
              UTSName const& utsname = lin.GetUTSName();
              
              strncpy(value.sysname,  utsname.sysname.c_str(), sizeof(value.sysname) - 1);
              strncpy(value.nodename, utsname.nodename.c_str(), sizeof(value.nodename) - 1);
              strncpy(value.release,  utsname.release.c_str(), sizeof(value.release) - 1);
              strncpy(value.version,  utsname.version.c_str(), sizeof(value.version) - 1);
              strncpy(value.machine,  utsname.machine.c_str(), sizeof(value.machine));
              this->WriteMem(lin, buf_addr, (uint8_t *)&value, sizeof(value));
	
              SetARMSystemCallStatus(lin, (ret == -1) ? -target_errno : ret, (ret == -1));
            }
          } sc;
          return &sc;
        } break;

      case 195: /* ARM specific stat64 syscall */
        {
          static struct : public SysCall {
            char const* GetName() const { return "stat64"; }
            void Execute( LINUX& lin, int syscall_id ) const
            {
              int ret;
              int32_t target_errno = 0;

              address_type pathnameaddr = GetSystemCallParam(lin, 0);
              address_type buf_address = GetSystemCallParam(lin, 1);
              std::string pathname;
              if (this->ReadMemString(lin, pathnameaddr, pathname))
                {
                  struct arm_stat64 target_stat;
                  ret = Stat64(pathname.c_str(), &target_stat, lin.GetEndianness());
                  this->WriteMem(lin, buf_address, (uint8_t *)&target_stat, sizeof(target_stat));
                      
                  if(unlikely(lin.GetVerbose()))
                    {
                      lin.Logger() << DebugInfo
                                   << "pathname = \"" << pathname << "\", buf_address = 0x" << std::hex << buf_address << std::dec
                                   << EndDebugInfo;
                    }
                }
              else
                {
                  ret = -1;
                  target_errno = LINUX_ENOMEM;
                }
              
              SetARMSystemCallStatus(lin, (parameter_type) (ret == -1) ? -target_errno : ret, (ret == -1));
            }
          } sc;
          return &sc;
        } break;

      case 197: /* ARM specific fstat64 syscall */
        {
          static struct : public SysCall {
            char const* GetName() const { return "fstat64"; }
            void Execute( LINUX& lin, int syscall_id ) const
            {
              int ret;
              address_type buf_address;
              int32_t target_fd;
              int host_fd;
              int32_t target_errno = 0;

              target_fd = GetSystemCallParam(lin, 0);
              buf_address = GetSystemCallParam(lin, 1);
	
              host_fd = SysCall::Target2HostFileDescriptor(lin, target_fd);
	
              if(host_fd == -1)
                {
                  target_errno = LINUX_EBADF;
                  ret = -1;
                }
              else
                {
                  struct arm_stat64 target_stat;
                  ret = Fstat64(host_fd, &target_stat, lin.GetEndianness());
                  if(ret == -1) target_errno = this->Host2LinuxErrno(lin, errno);
			
                  this->WriteMem(lin, buf_address, (uint8_t *)&target_stat, sizeof(target_stat));
                }
	
              if(unlikely(lin.GetVerbose()))
                {
                  lin.Logger() << DebugInfo
                               << "fd = " << target_fd << ", buf_address = 0x" << std::hex << buf_address << std::dec
                               << EndDebugInfo;
                }
	
              SetARMSystemCallStatus(lin, (parameter_type) (ret == -1) ? -target_errno : ret, (ret == -1));
            }
          } sc;
          return &sc;
        } break;


      case 983041: {
        static struct : public SysCall {
          char const* GetName() const { return "breakpoint"; }
          void Execute( LINUX& lin, int syscall_id ) const { SetARMSystemCallStatus(lin, (parameter_type)(-EINVAL), true); }
        } sc;
        return &sc;
      } break;
            
      case 983042: {
        static struct : public SysCall {
          char const* GetName() const { return "cacheflush"; }
          void Execute( LINUX& lin, int syscall_id ) const { SetARMSystemCallStatus(lin, (parameter_type)(-EINVAL), true); }
        } sc;
        return &sc;
      } break;
            
      case 983043: {
        static struct : public SysCall {
          char const* GetName() const { return "usr26"; }
          void Execute( LINUX& lin, int syscall_id ) const { SetARMSystemCallStatus(lin, (parameter_type)(-EINVAL), true); }
        } sc;
        return &sc;
      } break;
            
      case 983044: {
        static struct : public SysCall {
          char const* GetName() const { return "usr32"; }
          void Execute( LINUX& lin, int syscall_id ) const { SetARMSystemCallStatus(lin, (parameter_type)(-EINVAL), true); }
        } sc;
        return &sc;
      } break;
            
      case 983045: {
        static struct : public SysCall {
          char const* GetName() const { return "set_tls"; }
          void Execute( LINUX& lin, int syscall_id ) const
          {
            uint32_t r0 = Host2Target(lin.GetEndianness(), GetSystemCallParam(lin, 0));
            this->WriteMem(lin, 0xffff0ff0UL, (uint8_t *)&(r0), sizeof(r0));
            SetARMSystemCallStatus(lin, (parameter_type)0, false);
          }
        } sc;
        return &sc;
      } break;
            
      }
          
      return 0;
    }
    
    bool SetSystemBlob( unisim::util::debug::blob::Blob<address_type>* blob ) const
    {
      typedef unisim::util::debug::blob::Section<address_type> Section;
      typedef unisim::util::debug::blob::Segment<address_type> Segment;
      std::string mach = lin.GetUTSName().machine;
      if ((mach.compare("armv5") == 0) or (mach.compare("armv6") == 0) or (mach.compare("armv7") == 0))
        {
          if (blob == NULL) return false;
          // TODO: Check that the program/stack is not in conflict with the
          //   tls and cmpxchg interfaces
          // Set the tls interface, this requires a write into the memory
          //   system
          // The following instructions need to be added to memory:
          // 0xffff0fe0:  e59f0008   ldr r0, [pc, #(16 - 8)] @ TLS stored
          //                                                 @ at 0xffff0ff0
          // 0xffff0fe4:  e1a0f00e   mov pc, lr
          // 0xffff0fe8:  0
          // 0xffff0fec:  0
          // 0xffff0ff0:  0
          // 0xffff0ff4:  0
          // 0xffff0ff8:  0
          address_type tls_base_addr = 0xffff0fe0UL;
          static const uint32_t tls_buf_length = 7;
          static const uint32_t tls_buf[tls_buf_length] =
            {0xe59f0008UL, 0xe1a0f00eUL, 0, 0, 0, 0, 0};
          uint32_t *segment_tls_buf = (uint32_t *) malloc(sizeof(uint32_t) * tls_buf_length);
          uint32_t *section_tls_buf = (uint32_t *) malloc(sizeof(uint32_t) * tls_buf_length);

          for (unsigned int i = 0; i < tls_buf_length; ++i) {
            segment_tls_buf[i] = unisim::util::endian::Host2Target(lin.GetEndianness(), tls_buf[i]);
            section_tls_buf[i] = unisim::util::endian::Host2Target(lin.GetEndianness(), tls_buf[i]);
          }
          Segment *tls_if_segment =
            new Segment(Segment::TY_LOADABLE, Segment::SA_X, 4, tls_base_addr,
                        sizeof(tls_buf), sizeof(tls_buf), segment_tls_buf);
          Section *tls_if_section =
            new Section(Section::TY_UNKNOWN, Section::SA_A,
                        ".unisim.linux_os.interface.tls", 4, 0, tls_base_addr,
                        sizeof(tls_buf), section_tls_buf);

          // Set the cmpxchg (atomic compare and exchange) interface, the
          //   following instructions need to be added to memory:
          // 0xffff0fc0:  e5923000   ldr r3, [r2]
          // 0xffff0fc4:  e0533000   subs r3, r3, r0
          // 0xffff0fc8:  05821000   streq r1, [r2]
          // 0xffff0fcc:  e2730000   rsbs r0, r3, #0	; 0x0
          // 0xffff0fd0:  e1a0f00e   mov pc, lr
          address_type cmpxchg_base_addr = 0xffff0fc0UL;
          static const uint32_t cmpxchg_buf_length = 5;
          static const uint32_t cmpxchg_buf[cmpxchg_buf_length] = {
            0xe5923000UL,
            0xe0533000UL,
            0x05821000UL,
            0xe2730000UL,
            0xe1a0f00eUL
          };
          uint32_t *segment_cmpxchg_buf = (uint32_t *) malloc(sizeof(uint32_t) * cmpxchg_buf_length);
          uint32_t *section_cmpxchg_buf = (uint32_t *) malloc(sizeof(uint32_t) * cmpxchg_buf_length);

          for (unsigned int i = 0; i < cmpxchg_buf_length; ++i) {
            segment_cmpxchg_buf[i] = unisim::util::endian::Host2Target(lin.GetEndianness(), cmpxchg_buf[i]);
            section_cmpxchg_buf[i] = unisim::util::endian::Host2Target(lin.GetEndianness(), cmpxchg_buf[i]);
          }
          Section* cmpxchg_if_section =
            new Section(Section::TY_UNKNOWN, Section::SA_A,
                        ".unisim.linux_os.interface.cmpxchg",
                        4, 0, cmpxchg_base_addr,
                        sizeof(cmpxchg_buf), segment_cmpxchg_buf);
          Segment *cmpxchg_if_segment =
            new Segment(Segment::TY_LOADABLE, Segment::SA_X,
                        4, cmpxchg_base_addr,
                        sizeof(cmpxchg_buf), sizeof(cmpxchg_buf), section_cmpxchg_buf);

          blob->AddSegment(tls_if_segment);
          blob->AddSegment(cmpxchg_if_segment);
          blob->AddSection(tls_if_section);
          blob->AddSection(cmpxchg_if_section);
        }

      return true;
    }
  };

} // end of namespace linux_os
} // end of namespace os
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_OS_LINUX_ARM_HH__
