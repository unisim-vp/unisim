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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#ifndef __UNISIM_UTIL_OS_LINUX_I386_HH__
#define __UNISIM_UTIL_OS_LINUX_I386_HH__

#include <inttypes.h>

namespace unisim {
namespace util {
namespace os {
namespace linux_os {

// Register names
static char const* const kI386_eax = "%eax";
static char const* const kI386_ecx = "%ecx";
static char const* const kI386_edx = "%edx";
static char const* const kI386_ebx = "%ebx";
static char const* const kI386_esp = "%esp";
static char const* const kI386_ebp = "%ebp";
static char const* const kI386_esi = "%esi";
static char const* const kI386_edi = "%edi";
static char const* const kI386_eip = "%eip";

static char const* const kI386_es = "%es";
static char const* const kI386_cs = "%cs";
static char const* const kI386_ss = "%ss";
static char const* const kI386_ds = "%ds";
static char const* const kI386_fs = "%fs";
static char const* const kI386_gs = "%gs";

  template <class LINUX>
  struct I386TS : public LINUX::TargetSystem
  {
    // Name imports
    typedef typename LINUX::SysCall SysCall;
    typedef typename LINUX::address_type address_type;
    typedef typename LINUX::parameter_type parameter_type;
    typedef typename LINUX::UTSName UTSName;
    using LINUX::TargetSystem::SetRegister;
    using LINUX::TargetSystem::GetRegister;
    using LINUX::TargetSystem::lin;
    using LINUX::TargetSystem::name;
    
    // I386 linux structs
    struct i386_timespec
    {
      int32_t tv_sec;     /* Seconds.     (__time_t) */
      int32_t tv_nsec;    /* Nanoseconds. (long int) */
    };

    struct i386_stat64
    {
      uint64_t st_dev;            // 00-08 0b0000xxx
      uint32_t __pad1;            // 08-0c 0b00010xx
      uint32_t __st_ino;          // 0c-10 0b00011xx
      uint32_t st_mode;           // 10-14 0b00100xx
      uint32_t st_nlink;          // 14-18 0b00101xx
      uint32_t st_uid;            // 18-1c 0b00110xx
      uint32_t st_gid;            // 1c-20 0b00111xx
      uint64_t st_rdev;           // 20-28 0b0100xxx
      uint32_t __pad2;            // 28-2c 0b01010xx
      int32_t  st_size_low;       // 2c-30 0b01011xx ... need to split st_size to solve
      int32_t  st_size_high;      // 30-34 0b01100xx ... alignment issues on 64bits platforms
      int32_t  st_blksize;        // 34-38 0b01101xx
      int64_t  st_blocks;         // 38-40 0b0111xxx
      int32_t  st_atim_tv_sec;    // 40-44 0b10000xx
      int32_t  st_atim_tv_nsec;   // 44-48 0b10001xx
      int32_t  st_mtim_tv_sec;    // 48-4c 0b10010xx
      int32_t  st_mtim_tv_nsec;   // 4c-50 0b10011xx
      int32_t  st_ctim_tv_sec;    // 50-54 0b10100xx
      int32_t  st_ctim_tv_nsec;   // 54-58 0b10101xx
      uint64_t st_ino;            // 58-60 0b1011xxx
    };

    struct i386_tms
    {
      int32_t tms_utime;          /* User CPU time.                    (clock_t) */
      int32_t tms_stime;          /* System CPU time.                  (clock_t) */
      int32_t tms_cutime;         /* User CPU time of dead children.   (clock_t) */
      int32_t tms_cstime;         /* System CPU time of dead children. (clock_t) */
    };

    struct i386_utsname
    {
      char sysname[65];
      char nodename[65];
      char release[65];
      char version[65];
      char machine[65];
      char domainname[65];
    };

    struct i386_timeval
    {
      int32_t tv_sec;         /* seconds      (__kernel_time_t) */
      int32_t tv_usec;        /* microseconds (__kernel_suseconds_t) */
    };

    struct i386_timezone
    {
      int32_t tz_minuteswest; /* minutes west of Greenwich (int) */
      int32_t tz_dsttime;     /* type of dst correction    (int) */
    };
    
    I386TS( LINUX& _lin ) : LINUX::TargetSystem( "i386", _lin ) {}
    
    bool GetAT_HWCAP( address_type& hwcap ) const { return false; }
        
    void WriteSegmentSelector( char const* segname, uint16_t value ) const
    {
      if (unisim::util::debug::Register* reg = this->RegsIF().GetRegister(segname))
        reg->SetValue(&value);
      else
        throw 0;
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
      // Set EIP to the program entry point
      if (not SetRegister(lin, kI386_eip, lin.GetEntryPoint()))
        return false;
      // Set ESP to the base of the created stack
      unisim::util::debug::blob::Section<address_type> const * esp_section =
        lin.GetBlob()->FindSection(".unisim.linux_os.stack.stack_pointer");
      if (esp_section == NULL)
        {
          lin.Logger() << DebugError << "Could not find the stack pointer section." << EndDebugError;
          return false;
        }
      if (not SetRegister(lin, kI386_esp, esp_section->GetAddr()))
        return false;
          
      // Pseudo GDT initialization  (flat memory + early TLS)
      SetRegister(lin, "@gdt[1].base", 0 ); // For code segment
      SetRegister(lin, "@gdt[2].base", 0 ); // For data segments
      unisim::util::debug::blob::Section<address_type> const* etls_section =
        lin.GetBlob()->FindSection(".unisim.linux_os.etls.middle_pointer");
      if (not etls_section)
        {
          lin.Logger() << DebugError << "Could not find the early TLS section." << EndDebugError;
          return false;
        }
      SetRegister(lin, "@gdt[3].base", etls_section->GetAddr() ); // for early TLS kludge
          
      this->WriteSegmentSelector(kI386_cs,(1<<3) | (0<<2) | 3); // code
      this->WriteSegmentSelector(kI386_ss,(2<<3) | (0<<2) | 3); // data
      this->WriteSegmentSelector(kI386_ds,(2<<3) | (0<<2) | 3); // data
      this->WriteSegmentSelector(kI386_es,(2<<3) | (0<<2) | 3); // data
      this->WriteSegmentSelector(kI386_fs,(2<<3) | (0<<2) | 3); // data
      this->WriteSegmentSelector(kI386_gs,(3<<3) | (0<<2) | 3); // tls
          
      return true;
    }
        
    static void set_tls_base( LINUX& lin, address_type base )
    {
      SetRegister( lin, "@gdt[3].base", base ); // pseudo allocation of a tls descriptor
    }

    static void SetSystemCallStatus(LINUX& lin, int ret, bool error) { SetRegister(lin, kI386_eax, (parameter_type) ret); }
    void SetSystemCallStatus(int ret, bool error) const { SetRegister(lin, kI386_eax, (parameter_type) ret); }
        
    static parameter_type GetSystemCallParam(LINUX& lin, int id)
    {
      parameter_type val = 0;
          
      switch (id) {
      case 0: GetRegister(lin, kI386_ebx, &val); break;
      case 1: GetRegister(lin, kI386_ecx, &val); break;
      case 2: GetRegister(lin, kI386_edx, &val); break;
      case 3: GetRegister(lin, kI386_esi, &val); break;
      case 4: GetRegister(lin, kI386_edi, &val); break;
      case 5: GetRegister(lin, kI386_ebp, &val); break;
      default: throw id;
      }
          
      return val;
    }
    parameter_type GetSystemCallParam(int id) const
    {
      try { return GetSystemCallParam( lin, id ); }
      
      catch (int x) {
        lin.Logger() << DebugError << "No syscall argument #" << id << " in " << this->name << " linux" << EndDebugError;
      }
      
      return 0;
    }
      
    SysCall* GetSystemCall( int& id ) const
    {
      // TODO: fill syscall map
      // see either arch/i386/include/asm/unistd.h or arch/i386/include/uapi/asm/unistd.h in LINUX source
      switch (id) {
      case 0: return this->GetSyscallByName("restart_syscall");
      case 1: return this->GetSyscallByName("exit");
      case 2: return this->GetSyscallByName("fork");
      case 3: return this->GetSyscallByName("read");
      case 4: return this->GetSyscallByName("write");
      case 5: return this->GetSyscallByName("open");
      case 6: return this->GetSyscallByName("close");
      case 7: return this->GetSyscallByName("waitpid");
      case 8: return this->GetSyscallByName("creat");
      case 9: return this->GetSyscallByName("link");
      case 10: return this->GetSyscallByName("unlink");
      case 11: return this->GetSyscallByName("execve");
      case 12: return this->GetSyscallByName("chdir");
      case 13: return this->GetSyscallByName("time");
      case 14: return this->GetSyscallByName("mknod");
      case 15: return this->GetSyscallByName("chmod");
      case 16: return this->GetSyscallByName("lchown");
      case 17: return this->GetSyscallByName("break");
      case 18: return this->GetSyscallByName("oldstat");
      case 19: return this->GetSyscallByName("lseek");
      case 20: return this->GetSyscallByName("getpid");
      case 21: return this->GetSyscallByName("mount");
      case 22: return this->GetSyscallByName("umount");
      case 23: return this->GetSyscallByName("setuid");
      case 24: return this->GetSyscallByName("getuid");
      case 25: return this->GetSyscallByName("stime");
      case 26: return this->GetSyscallByName("ptrace");
      case 27: return this->GetSyscallByName("alarm");
      case 28: return this->GetSyscallByName("oldfstat");
      case 29: return this->GetSyscallByName("pause");
      case 30: return this->GetSyscallByName("utime");
      case 31: return this->GetSyscallByName("stty");
      case 32: return this->GetSyscallByName("gtty");
      case 33: return this->GetSyscallByName("access");
      case 34: return this->GetSyscallByName("nice");
      case 35: return this->GetSyscallByName("ftime");
      case 36: return this->GetSyscallByName("sync");
      case 37: return this->GetSyscallByName("kill");
      case 38: return this->GetSyscallByName("rename");
      case 39: return this->GetSyscallByName("mkdir");
      case 40: return this->GetSyscallByName("rmdir");
      case 41: return this->GetSyscallByName("dup");
      case 42: return this->GetSyscallByName("pipe");
        // // 43: times (see i386 specific)
      case 44: return this->GetSyscallByName("prof");
      case 45: return this->GetSyscallByName("brk");
      case 46: return this->GetSyscallByName("setgid");
      case 47: return this->GetSyscallByName("getgid");
      case 48: return this->GetSyscallByName("signal");
      case 49: return this->GetSyscallByName("geteuid");
      case 50: return this->GetSyscallByName("getegid");
      case 51: return this->GetSyscallByName("acct");
      case 52: return this->GetSyscallByName("umount2");
      case 53: return this->GetSyscallByName("lock");
      case 54: return this->GetSyscallByName("ioctl");
      case 55: return this->GetSyscallByName("fcntl");
      case 56: return this->GetSyscallByName("mpx");
      case 57: return this->GetSyscallByName("setpgid");
      case 58: return this->GetSyscallByName("ulimit");
      case 59: return this->GetSyscallByName("oldolduname");
      case 60: return this->GetSyscallByName("umask");
      case 61: return this->GetSyscallByName("chroot");
      case 62: return this->GetSyscallByName("ustat");
      case 63: return this->GetSyscallByName("dup2");
      case 64: return this->GetSyscallByName("getppid");
      case 65: return this->GetSyscallByName("getpgrp");
      case 66: return this->GetSyscallByName("setsid");
      case 67: return this->GetSyscallByName("sigaction");
      case 68: return this->GetSyscallByName("sgetmask");
      case 69: return this->GetSyscallByName("ssetmask");
      case 70: return this->GetSyscallByName("setreuid");
      case 71: return this->GetSyscallByName("setregid");
      case 72: return this->GetSyscallByName("sigsuspend");
      case 73: return this->GetSyscallByName("sigpending");
      case 74: return this->GetSyscallByName("sethostname");
      case 75: return this->GetSyscallByName("setrlimit");
      case 76: return this->GetSyscallByName("getrlimit");
      case 77: return this->GetSyscallByName("getrusage");
        // 78: gettimeofday (see i386 specific)
      case 79: return this->GetSyscallByName("settimeofday");
      case 80: return this->GetSyscallByName("getgroups");
      case 81: return this->GetSyscallByName("setgroups");
      case 82: return this->GetSyscallByName("select");
      case 83: return this->GetSyscallByName("symlink");
      case 84: return this->GetSyscallByName("oldlstat");
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
      case 98: return this->GetSyscallByName("profil");
      case 99: return this->GetSyscallByName("statfs");
      case 100: return this->GetSyscallByName("fstatfs");
      case 101: return this->GetSyscallByName("ioperm");
      case 102: return this->GetSyscallByName("socketcall");
      case 103: return this->GetSyscallByName("syslog");
      case 104: return this->GetSyscallByName("setitimer");
      case 105: return this->GetSyscallByName("getitimer");
      case 106: return this->GetSyscallByName("stat");
      case 107: return this->GetSyscallByName("lstat");
        // 108 fstat (see i386 specific)
      case 109: return this->GetSyscallByName("olduname");
      case 110: return this->GetSyscallByName("iopl");
      case 111: return this->GetSyscallByName("vhangup");
      case 112: return this->GetSyscallByName("idle");
      case 113: return this->GetSyscallByName("vm86old");
      case 114: return this->GetSyscallByName("wait4");
      case 115: return this->GetSyscallByName("swapoff");
      case 116: return this->GetSyscallByName("sysinfo");
      case 117: return this->GetSyscallByName("ipc");
      case 118: return this->GetSyscallByName("fsync");
      case 119: return this->GetSyscallByName("sigreturn");
      case 120: return this->GetSyscallByName("clone");
      case 121: return this->GetSyscallByName("setdomainname");
        // 122: uname (see i386 specific)
      case 123: return this->GetSyscallByName("modify_ldt");
      case 124: return this->GetSyscallByName("adjtimex");
      case 125: return this->GetSyscallByName("mprotect");
      case 126: return this->GetSyscallByName("sigprocmask");
      case 127: return this->GetSyscallByName("create_module");
      case 128: return this->GetSyscallByName("init_module");
      case 129: return this->GetSyscallByName("delete_module");
      case 130: return this->GetSyscallByName("get_kernel_syms");
      case 131: return this->GetSyscallByName("quotactl");
      case 132: return this->GetSyscallByName("getpgid");
      case 133: return this->GetSyscallByName("fchdir");
      case 134: return this->GetSyscallByName("bdflush");
      case 135: return this->GetSyscallByName("sysfs");
      case 136: return this->GetSyscallByName("personality");
      case 137: return this->GetSyscallByName("afs_syscall");
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
      case 166: return this->GetSyscallByName("vm86");
      case 167: return this->GetSyscallByName("query_module");
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
      case 188: return this->GetSyscallByName("getpmsg");
      case 189: return this->GetSyscallByName("putpmsg");
      case 190: return this->GetSyscallByName("vfork");
      case 191: return this->GetSyscallByName("ugetrlimit");
      case 192: return this->GetSyscallByName("mmap2");
      case 193: return this->GetSyscallByName("truncate64");
      case 194: return this->GetSyscallByName("ftruncate64");
        // 195 stat64 (see i386 specific)
      case 196: return this->GetSyscallByName("lstat64");
        // 197 fstat64 (see i386 specific)
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
      case 217: return this->GetSyscallByName("pivot_root");
      case 218: return this->GetSyscallByName("mincore");
      case 219: return this->GetSyscallByName("madvise");
      case 220: return this->GetSyscallByName("getdents64");
      case 221: return this->GetSyscallByName("fcntl64");
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
        // 243: set_thread_area (see i386 specific)
      case 244: return this->GetSyscallByName("get_thread_area");
      case 245: return this->GetSyscallByName("io_setup");
      case 246: return this->GetSyscallByName("io_destroy");
      case 247: return this->GetSyscallByName("io_getevents");
      case 248: return this->GetSyscallByName("io_submit");
      case 249: return this->GetSyscallByName("io_cancel");
      case 250: return this->GetSyscallByName("fadvise64");
      case 252: return this->GetSyscallByName("exit_group");
      case 253: return this->GetSyscallByName("lookup_dcookie");
      case 254: return this->GetSyscallByName("epoll_create");
      case 255: return this->GetSyscallByName("epoll_ctl");
      case 256: return this->GetSyscallByName("epoll_wait");
      case 257: return this->GetSyscallByName("remap_file_pages");
      case 258: return this->GetSyscallByName("set_tid_address");
      case 259: return this->GetSyscallByName("timer_create");
      case 260: return this->GetSyscallByName("timer_settime");
      case 261: return this->GetSyscallByName("timer_gettime");
      case 262: return this->GetSyscallByName("timer_getoverrun");
      case 263: return this->GetSyscallByName("timer_delete");
      case 264: return this->GetSyscallByName("clock_settime");
      case 265: return this->GetSyscallByName("clock_gettime");
      case 266: return this->GetSyscallByName("clock_getres");
      case 267: return this->GetSyscallByName("clock_nanosleep");
      case 268: return this->GetSyscallByName("statfs64");
      case 269: return this->GetSyscallByName("fstatfs64");
      case 270: return this->GetSyscallByName("tgkill");
      case 271: return this->GetSyscallByName("utimes");
      case 272: return this->GetSyscallByName("fadvise64_64");
      case 273: return this->GetSyscallByName("vserver");
      case 274: return this->GetSyscallByName("mbind");
      case 275: return this->GetSyscallByName("get_mempolicy");
      case 276: return this->GetSyscallByName("set_mempolicy");
      case 277: return this->GetSyscallByName("mq_open");
      case 278: return this->GetSyscallByName("mq_unlink");
      case 279: return this->GetSyscallByName("mq_timedsend");
      case 280: return this->GetSyscallByName("mq_timedreceive");
      case 281: return this->GetSyscallByName("mq_notify");
      case 282: return this->GetSyscallByName("mq_getsetattr");
      case 283: return this->GetSyscallByName("kexec_load");
      case 284: return this->GetSyscallByName("waitid");
      case 286: return this->GetSyscallByName("add_key");
      case 287: return this->GetSyscallByName("request_key");
      case 288: return this->GetSyscallByName("keyctl");
      case 289: return this->GetSyscallByName("ioprio_set");
      case 290: return this->GetSyscallByName("ioprio_get");
      case 291: return this->GetSyscallByName("inotify_init");
      case 292: return this->GetSyscallByName("inotify_add_watch");
      case 293: return this->GetSyscallByName("inotify_rm_watch");
      case 294: return this->GetSyscallByName("migrate_pages");
      case 295: return this->GetSyscallByName("openat");
      case 296: return this->GetSyscallByName("mkdirat");
      case 297: return this->GetSyscallByName("mknodat");
      case 298: return this->GetSyscallByName("fchownat");
      case 299: return this->GetSyscallByName("futimesat");
      case 300: return this->GetSyscallByName("fstatat64");
      case 301: return this->GetSyscallByName("unlinkat");
      case 302: return this->GetSyscallByName("renameat");
      case 303: return this->GetSyscallByName("linkat");
      case 304: return this->GetSyscallByName("symlinkat");
      case 305: return this->GetSyscallByName("readlinkat");
      case 306: return this->GetSyscallByName("fchmodat");
      case 307: return this->GetSyscallByName("faccessat");
      case 308: return this->GetSyscallByName("pselect6");
      case 309: return this->GetSyscallByName("ppoll");
      case 310: return this->GetSyscallByName("unshare");
      case 311: return this->GetSyscallByName("set_robust_list");
      case 312: return this->GetSyscallByName("get_robust_list");
      case 313: return this->GetSyscallByName("splice");
      case 314: return this->GetSyscallByName("sync_file_range");
      case 315: return this->GetSyscallByName("tee");
      case 316: return this->GetSyscallByName("vmsplice");
      case 317: return this->GetSyscallByName("move_pages");
      case 318: return this->GetSyscallByName("getcpu");
      case 319: return this->GetSyscallByName("epoll_pwait");
      case 320: return this->GetSyscallByName("utimensat");
      case 321: return this->GetSyscallByName("signalfd");
      case 322: return this->GetSyscallByName("timerfd_create");
      case 323: return this->GetSyscallByName("eventfd");
      case 324: return this->GetSyscallByName("fallocate");
      case 325: return this->GetSyscallByName("timerfd_settime");
      case 326: return this->GetSyscallByName("timerfd_gettime");
      case 327: return this->GetSyscallByName("signalfd4");
      case 328: return this->GetSyscallByName("eventfd2");
      case 329: return this->GetSyscallByName("epoll_create1");
      case 330: return this->GetSyscallByName("dup3");
      case 331: return this->GetSyscallByName("pipe2");
      case 332: return this->GetSyscallByName("inotify_init1");
      case 333: return this->GetSyscallByName("preadv");
      case 334: return this->GetSyscallByName("pwritev");
      case 335: return this->GetSyscallByName("rt_tgsigqueueinfo");
      case 336: return this->GetSyscallByName("perf_event_open");
      case 337: return this->GetSyscallByName("recvmmsg");
      case 338: return this->GetSyscallByName("fanotify_init");
      case 339: return this->GetSyscallByName("fanotify_mark");
      case 340: return this->GetSyscallByName("prlimit64");
      case 341: return this->GetSyscallByName("name_to_handle_at");
      case 342: return this->GetSyscallByName("open_by_handle_at");
      case 343: return this->GetSyscallByName("clock_adjtime");
      case 344: return this->GetSyscallByName("syncfs");
      case 345: return this->GetSyscallByName("sendmmsg");
      case 346: return this->GetSyscallByName("setns");
      case 347: return this->GetSyscallByName("process_vm_readv");
      case 348: return this->GetSyscallByName("process_vm_writev");
      case 349: return this->GetSyscallByName("kcmp");
      case 350: return this->GetSyscallByName("finit_module");
      case 351: return this->GetSyscallByName("sched_setattr");
      case 352: return this->GetSyscallByName("sched_getattr");

      case 122: /* i386 specific uname syscall */
        {
          static struct : public SysCall {
            char const* GetName() const { return "uname"; }
            void Execute( LINUX& lin, int syscall_id ) const
            {
              int ret;
              int32_t target_errno = 0;
              address_type buf_addr = GetSystemCallParam(lin, 0);
              ret = 0;
  
              struct i386_utsname value;
              memset(&value, 0, sizeof(value));
              UTSName const& utsname = lin.GetUTSName();
              strncpy(&value.sysname[0],    utsname.sysname.c_str(), sizeof(value.sysname) - 1);
              strncpy(&value.nodename[0],   utsname.nodename.c_str(), sizeof(value.nodename) - 1);
              strncpy(&value.release[0],    utsname.release.c_str(), sizeof(value.release) - 1);
              strncpy(&value.version[0],    utsname.version.c_str(), sizeof(value.version) - 1);
              strncpy(&value.machine[0],    utsname.machine.c_str(), sizeof(value.machine));
              strncpy(&value.domainname[0], utsname.domainname.c_str(), sizeof(value.domainname) - 1);
              this->WriteMem( lin, buf_addr, (uint8_t *)&value, sizeof(value));
  
              SetSystemCallStatus(lin, (ret == -1) ? -target_errno : ret, (ret == -1));
            }
          } sc;
          return &sc;
        } break;
            
      case 243: /* i386 specific set_thread_area syscall */
        {
          static struct : public SysCall {
            char const* GetName() const { return "set_thread_area"; }
            void Execute( LINUX& lin, int syscall_id ) const
            {
              uint32_t user_desc_ptr = GetSystemCallParam(lin, 0);
                  
              int32_t  entry_number;
              uint32_t base_addr, limit, attributes;
              this->ReadMem( lin, user_desc_ptr + 0x0, (uint8_t*)&entry_number, 4 );
              this->ReadMem( lin, user_desc_ptr + 0x4, (uint8_t*)&base_addr, 4 );
              this->ReadMem( lin, user_desc_ptr + 0x8, (uint8_t*)&limit, 4 );
              this->ReadMem( lin, user_desc_ptr + 0xc, (uint8_t*)&attributes, 4 );
              entry_number = unisim::util::endian::Target2Host( lin.GetEndianness(), entry_number );
              base_addr    = unisim::util::endian::Target2Host( lin.GetEndianness(), base_addr );
              limit        = unisim::util::endian::Target2Host( lin.GetEndianness(), limit );
              attributes   = unisim::util::endian::Target2Host( lin.GetEndianness(), attributes );
                  
              if (entry_number != -1)
                throw 0;
                  
              set_tls_base( lin, base_addr );
                  
              entry_number = 3;
              entry_number = unisim::util::endian::Host2Target( lin.GetEndianness(), entry_number );
              this->WriteMem( lin, user_desc_ptr + 0x0, (uint8_t*)&entry_number, 4 );

              SetSystemCallStatus( lin, 0, false );
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
          
      // The following code is clearly a hack since linux doesn't
      // allocate any TLS. Allocating the TLS is the libc's
      // responsability. Nevertheless, because syscalls emulation
      // is incomplete, errno (located in TLS) may be used before
      // any libc attempt to initialize it.  Thus, we need to
      // introduce a pseudo TLS are for these early accesses to
      // errno. 
      if (not blob) return false;
          
      // TODO: Check that the existing segments/sections are not
      //   in conflict with the early TLS area. Once libc have
      //   initialized TLS this area becomes useless. We
      //   additionnaly need to ensure that early mallocs won't
      //   come in the way. 
      //
      // In practice a [0xf0000000-0xf0000fff] zero-filled area seems safe...
      address_type const etls_addr = 0xf0000000UL;
      address_type const etls_size = 0x00001000UL;
      uint8_t* etls_seg_data = (uint8_t*)calloc(1,etls_size);
      // uint8_t* etls_sec_data = (uint8_t*)calloc(1,etls_size);
          
      Segment* etls_segment =
        new Segment(Segment::TY_LOADABLE, Segment::SA_RW, etls_size, etls_addr, etls_size, etls_size, etls_seg_data);
      // Section* etls_section =
      //   new Section(Section::TY_UNKNOWN, Section::SA_A, ".unisim.linux_os.interface.etls", etls_size, 0, etls_addr, etls_size, etls_sec_data);

      blob->AddSegment(etls_segment);
      blob->AddSection(new Section(Section::TY_NULL, Section::SA_NULL,".unisim.linux_os.etls.middle_pointer", 0, 0, (etls_addr + (etls_size/2)), 0, NULL));

      return true;
    }
  };

} // end of namespace linux_os
} // end of namespace os
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_OS_LINUX_I386_HH__
