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
 
#ifndef __UNISIM_UTIL_OS_LINUX_PPC_HH__
#define __UNISIM_UTIL_OS_LINUX_PPC_HH__

#include <inttypes.h>

namespace unisim {
namespace util {
namespace os {
namespace linux_os {

static const int kPPCNumRegs = 31;
static const int kPPCNumSysRegs = 2;
// registers indexes
static const uint32_t kPPC_r0  = 0;
static const uint32_t kPPC_r1  = 1;
static const uint32_t kPPC_r2  = 2;
static const uint32_t kPPC_r3  = 3;
static const uint32_t kPPC_r4  = 4;
static const uint32_t kPPC_r5  = 5;
static const uint32_t kPPC_r6  = 6;
static const uint32_t kPPC_r7  = 7;
static const uint32_t kPPC_r8  = 8;
static const uint32_t kPPC_r9  = 9;
static const uint32_t kPPC_r10 = 10;
static const uint32_t kPPC_r11 = 11;
static const uint32_t kPPC_r12 = 12;
static const uint32_t kPPC_r13 = 13;
static const uint32_t kPPC_r14 = 14;
static const uint32_t kPPC_r15 = 15;
static const uint32_t kPPC_r16 = 16;
static const uint32_t kPPC_r17 = 17;
static const uint32_t kPPC_r18 = 18;
static const uint32_t kPPC_r19 = 19;
static const uint32_t kPPC_r20 = 20;
static const uint32_t kPPC_r21 = 21;
static const uint32_t kPPC_r22 = 22;
static const uint32_t kPPC_r23 = 23;
static const uint32_t kPPC_r24 = 24;
static const uint32_t kPPC_r25 = 25;
static const uint32_t kPPC_r26 = 26;
static const uint32_t kPPC_r27 = 27;
static const uint32_t kPPC_r28 = 28;
static const uint32_t kPPC_r29 = 29;
static const uint32_t kPPC_r30 = 30;
static const uint32_t kPPC_r31 = 31;
static const uint32_t kPPC_cr  = 32;
static const uint32_t kPPC_cia = 33;

// this structure supposes that the timespec was needed (__timespec_defined)
// original structure
// struct timespec
//   {
//     __time_t tv_sec;            /* Seconds.  */
//     long int tv_nsec;           /* Nanoseconds.  */
//   };
struct powerpc_timespec {
  int32_t tv_sec;        /* Seconds.  */
  int32_t tv_nsec;       /* Nanoseconds.  */
};

// this structure supposes that the powerpc file was compiled with
//   the __USE_LARGEFILE64 and __USE_MISC
// original structure
//     # ifdef __USE_LARGEFILE64
//     struct stat64
//     {
//         __dev_t st_dev;			/* Device.  */
//         __ino64_t st_ino;			/* File serial number.	*/
//         __mode_t st_mode;			/* File mode.  */
//         __nlink_t st_nlink;			/* Link count.  */
//         __uid_t st_uid;			/* User ID of the file's owner.	*/
//         __gid_t st_gid;			/* Group ID of the file's group.*/
//         __dev_t st_rdev;			/* Device number, if device.  */
//         unsigned short int __pad2;
//         __off64_t st_size;			/* Size of file, in bytes.  */
//         __blksize_t st_blksize;		/* Optimal block size for I/O.  */
//         __blkcnt64_t st_blocks;		/* Number 512-byte blocks allocated. */
//     #ifdef __USE_MISC
//         /* Nanosecond resolution timestamps are stored in a format
//         equivalent to 'struct timespec'.  This is the type used
//         whenever possible but the Unix namespace rules do not allow the
//         identifier 'timespec' to appear in the <sys/stat.h> header.
//         Therefore we have to handle the use of this header in strictly
//         standard-compliant sources special.  */
//         struct timespec st_atim;		/* Time of last access.  */
//         struct timespec st_mtim;		/* Time of last modification.  */
//         struct timespec st_ctim;		/* Time of last status change.  */
//     # define st_atime st_atim.tv_sec	/* Backward compatibility.  */
//     # define st_mtime st_mtim.tv_sec
//     # define st_ctime st_ctim.tv_sec
//     #else
//         __time_t st_atime;			/* Time of last access.  */
//         unsigned long int st_atimensec;	/* Nscecs of last access.  */
//         __time_t st_mtime;			/* Time of last modification.  */
//         unsigned long int st_mtimensec;	/* Nsecs of last modification.  */
//         __time_t st_ctime;			/* Time of last status change.  */
//         unsigned long int st_ctimensec;	/* Nsecs of last status change.  */
//     #endif
//         unsigned long int __unused4;
//         unsigned long int __unused5;
//     };
//     # endif /* __USE_LARGEFILE64 */

struct powerpc_stat64 {
  uint64_t st_dev;			/* Device.  */
  uint64_t st_ino;			/* File serial number.	*/
  uint32_t st_mode;			/* File mode.  */
  uint32_t st_nlink;			/* Link count.  */
  uint32_t st_uid;			/* User ID of the file's owner.	*/
  uint32_t st_gid;			/* Group ID of the file's group.*/
  uint64_t st_rdev;			/* Device number, if device.  */
  uint32_t __pad2;
  int64_t st_size;			/* Size of file, in bytes.  */
  int32_t st_blksize;		/* Optimal block size for I/O.  */
  int64_t st_blocks;		/* Number 512-byte blocks allocated. */
  /* Nanosecond resolution timestamps are stored in a format
     equivalent to 'struct timespec'.  This is the type used
     whenever possible but the Unix namespace rules do not allow the
     identifier 'timespec' to appear in the <sys/stat.h> header.
     Therefore we have to handle the use of this header in strictly
     standard-compliant sources special.  */
  struct powerpc_timespec st_atim;		/* Time of last access.  */
  struct powerpc_timespec st_mtim;		/* Time of last modification.  */
  struct powerpc_timespec st_ctim;		/* Time of last status change.  */
  uint32_t __unused4;
  uint32_t __unused5;
};

struct powerpc_stat
{
  int64_t st_dev;
  uint32_t __pad1;
  uint32_t st_ino;
  uint32_t st_mode;
  uint32_t st_nlink;
  uint32_t st_uid;
  uint32_t st_gid;
  int64_t st_rdev;
  uint32_t __pad2;
  int32_t st_size;
  int32_t st_blksize;
  int32_t st_blocks;
  struct powerpc_timespec st_atim;
  struct powerpc_timespec st_mtim;
  struct powerpc_timespec st_ctim;
  uint32_t __unused4;
  uint32_t __unused5;
};

// 	original structure
// 	struct tms
// 	{
// 	  clock_t tms_utime;          /* User CPU time.  */
// 	  clock_t tms_stime;          /* System CPU time.  */
// 	  clock_t tms_cutime;         /* User CPU time of dead children.  */
// 	  clock_t tms_cstime;         /* System CPU time of dead children.  */
// 	};
struct powerpc_tms {
  int32_t tms_utime;      /* User CPU time.  */
  int32_t tms_stime;      /* System CPU time.  */
  int32_t tms_cutime;     /* User CPU time of dead children.  */
  int32_t tms_cstime;     /* System CPU time of dead children.  */
};

struct ppc_utsname {
  char sysname[65];
  char nodename[65];
  char release[65];
  char version[65];
  char machine[65];
};

} // end of namespace linux_os
} // end of namespace os
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_OS_LINUX_PPC_HH__
