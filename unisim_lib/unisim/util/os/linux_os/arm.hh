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

#include <inttypes.h>

namespace unisim {
namespace util {
namespace os {
namespace linux_os {

static const uint32_t kARMNumRegs = 16;
static const uint32_t kARMNumSysRegs = 0;
// registers indexes
static const uint32_t kARM_r0 = 0;
static const uint32_t kARM_r1 = 1;
static const uint32_t kARM_r2 = 2;
static const uint32_t kARM_r3 = 3;
static const uint32_t kARM_r4 = 4;
static const uint32_t kARM_r5 = 5;
static const uint32_t kARM_r6 = 6;
static const uint32_t kARM_r7 = 7;
static const uint32_t kARM_r8 = 8;
static const uint32_t kARM_r9 = 9;
static const uint32_t kARM_r10 = 10;
static const uint32_t kARM_r11 = 11;
static const uint32_t kARM_r12 = 12;
static const uint32_t kARM_r13 = 13;
static const uint32_t kARM_r14 = 14;
static const uint32_t kARM_r15 = 15;
static const uint32_t kARM_sp  = 13;
static const uint32_t kARM_lr  = 14;
static const uint32_t kARM_pc  = 15;
static const uint32_t kARMSyscallStatusReg = 0;
static const uint32_t kARMEABISyscallStatusReg = 0;
static const uint32_t kARMEABISyscallNumberReg = 7;

// this structure supposes that the timespec was needed (__timespec_defined)
// original structure
// struct timespec
//   {
//     __time_t tv_sec;            /* Seconds.  */
//     long int tv_nsec;           /* Nanoseconds.  */
//   };
struct arm_timespec {
  uint32_t tv_sec;		/* Seconds.  */
  uint32_t tv_nsec;		/* Nanoseconds.  */
};

// this structure supposes that the arm file was compiled with
//   the __USE_LARGEFILE64 and __USE_MISC
// original structure
// #ifdef __USE_LARGEFILE64
// struct stat64
//   {
//     __dev_t st_dev;			/* Device.  */
//     unsigned int __pad1;
//     __ino_t __st_ino;			/* 32bit file serial number.	*/
//     __mode_t st_mode;			/* File mode.  */
//     __nlink_t st_nlink;			/* Link count.  */
//     __uid_t st_uid;			/* User ID of the file's owner.	*/
//     __gid_t st_gid;			/* Group ID of the file's group.*/
//     __dev_t st_rdev;			/* Device number, if device.  */
//     unsigned int __pad2;
//     __off64_t st_size;			/* Size of file, in bytes.  */
//     __blksize_t st_blksize;		/* Optimal block size for I/O.  */
//     __blkcnt64_t st_blocks;		/* Number 512-byte blocks allocated. */
// #ifdef __USE_MISC
//     /* Nanosecond resolution timestamps are stored in a format
//        equivalent to 'struct timespec'.  This is the type used
//        whenever possible but the Unix namespace rules do not allow the
//        identifier 'timespec' to appear in the <sys/stat.h> header.
//        Therefore we have to handle the use of this header in strictly
//        standard-compliant sources special.  */
//     struct timespec st_atim;		/* Time of last access.  */
//     struct timespec st_mtim;		/* Time of last modification.  */
//     struct timespec st_ctim;		/* Time of last status change.  */
// #else
//     __time_t st_atime;			/* Time of last access.  */
//     unsigned long int st_atimensec;	/* Nscecs of last access.  */
//     __time_t st_mtime;			/* Time of last modification.  */
//     unsigned long int st_mtimensec;	/* Nsecs of last modification.  */
//     __time_t st_ctime;			/* Time of last status change.  */
//     unsigned long int st_ctimensec;	/* Nsecs of last status change.  */
// #endif
//     __ino64_t st_ino;			/* File serial number.		*/
//   };
// #endif

// sizeof(stat64)=104
// offset(st_dev)=0, sizeof(st_dev)=8
// offset(st_ino)=96, sizeof(st_ino)=8
// offset(st_mode)=16, sizeof(st_mode)=4
// offset(st_nlink)=20, sizeof(st_nlink)=4
// offset(st_uid)=24, sizeof(st_uid)=4
// offset(st_gid)=28, sizeof(st_gid)=4
// offset(st_rdev)=32, sizeof(st_rdev)=8
// offset(__pad2)=40, sizeof(__pad2)=4
// offset(st_size)=48, sizeof(st_size)=8
// offset(st_blksize)=56, sizeof(st_blksize)=4
// offset(st_blocks)=64, sizeof(st_blocks)=8
// offset(st_atim)=72, sizeof(st_atim)=8
// offset(st_mtim)=80, sizeof(st_mtim)=8
// offset(st_ctim)=88, sizeof(st_ctim)=8

struct arm_stat64 {
  uint64_t st_dev;			/* Device.  */
  uint32_t __pad1;
  uint32_t __st_ino;			/* 32bit file serial number.	*/
  uint32_t st_mode;			/* File mode.  */
  uint32_t st_nlink;			/* Link count.  */
  uint32_t st_uid;			/* User ID of the file's owner.	*/
  uint32_t st_gid;			/* Group ID of the file's group.*/
  uint64_t st_rdev;			/* Device number, if device.  */
  uint64_t __pad2;
  int64_t st_size;			/* Size of file, in bytes.  */
  uint32_t st_blksize;		/* Optimal block size for I/O.  */
  uint32_t __pad3;
  uint64_t st_blocks;		/* Number 512-byte blocks allocated. */
  struct arm_timespec st_atim;		/* Time of last access.  */
  struct arm_timespec st_mtim;		/* Time of last modification.  */
  struct arm_timespec st_ctim;		/* Time of last status change.  */
  uint64_t st_ino;			/* File serial number.		*/
};

// 	original structure
// 	struct tms
// 	{
// 	  clock_t tms_utime;          /* User CPU time.  */
// 	  clock_t tms_stime;          /* System CPU time.  */
// 	  clock_t tms_cutime;         /* User CPU time of dead children.  */
// 	  clock_t tms_cstime;         /* System CPU time of dead children.  */
// 	};
struct arm_tms {
  int32_t tms_utime;          /* User CPU time.  */
  int32_t tms_stime;          /* System CPU time.  */
  int32_t tms_cutime;         /* User CPU time of dead children.  */
  int32_t tms_cstime;         /* System CPU time of dead children.  */
};

struct arm_utsname {
  char sysname[65];
  char nodename[65];
  char release[65];
  char version[65];
  char machine[65];
};

// 	original structure
//struct timeval {
//        __kernel_time_t         tv_sec;         /* seconds */
//        __kernel_suseconds_t    tv_usec;        /* microseconds */
//};

struct arm_timeval {
  int32_t tv_sec;         /* seconds */
  int32_t tv_usec;        /* microseconds */
};

// 	original structure
//struct timezone {
//        int     tz_minuteswest; /* minutes west of Greenwich */
//        int     tz_dsttime;     /* type of dst correction */
//};

struct arm_timezone {
  int32_t tz_minuteswest; /* minutes west of Greenwich */
  int32_t tz_dsttime;     /* type of dst correction */
};

} // end of namespace linux_os
} // end of namespace os
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_OS_LINUX_ARM_HH__