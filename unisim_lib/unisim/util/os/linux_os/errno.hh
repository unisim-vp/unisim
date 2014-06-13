/*
 *  Copyright (c) 2014 Commissariat a l'Energie Atomique (CEA) All rights
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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_UTIL_OS_LINUX_OS_ERRNO_HH__
#define __UNISIM_UTIL_OS_LINUX_OS_ERRNO_HH__

#include <inttypes.h>

namespace unisim {
namespace util {
namespace os {
namespace linux_os {

static const uint32_t LINUX_EPERM           = 1;   // Operation not permitted
static const uint32_t LINUX_ENOENT          = 2;   // No such file or directory
static const uint32_t LINUX_ESRCH           = 3;   // No such process
static const uint32_t LINUX_EINTR           = 4;   // Interrupted system call
static const uint32_t LINUX_EIO             = 5;   // I/O error
static const uint32_t LINUX_ENXIO           = 6;   // No such device or address
static const uint32_t LINUX_E2BIG           = 7;   // Argument list too long
static const uint32_t LINUX_ENOEXEC         = 8;   // Exec format error
static const uint32_t LINUX_EBADF           = 9;   // Bad file number
static const uint32_t LINUX_ECHILD          = 10;  // No child processes
static const uint32_t LINUX_EAGAIN          = 11;  // Try again
static const uint32_t LINUX_ENOMEM          = 12;  // Out of memory
static const uint32_t LINUX_EACCES          = 13;  // Permission denied
static const uint32_t LINUX_EFAULT          = 14;  // Bad address
static const uint32_t LINUX_ENOTBLK         = 15;  // Block device required
static const uint32_t LINUX_EBUSY           = 16;  // Device or resource busy
static const uint32_t LINUX_EEXIST          = 17;  // File exists
static const uint32_t LINUX_EXDEV           = 18;  // Cross-device link
static const uint32_t LINUX_ENODEV          = 19;  // No such device
static const uint32_t LINUX_ENOTDIR         = 20;  // Not a directory
static const uint32_t LINUX_EISDIR          = 21;  // Is a directory
static const uint32_t LINUX_EINVAL          = 22;  // Invalid argument
static const uint32_t LINUX_ENFILE          = 23;  // File table overflow
static const uint32_t LINUX_EMFILE          = 24;  // Too many open files
static const uint32_t LINUX_ENOTTY          = 25;  // Not a typewriter
static const uint32_t LINUX_ETXTBSY         = 26;  // Text file busy
static const uint32_t LINUX_EFBIG           = 27;  // File too large
static const uint32_t LINUX_ENOSPC          = 28;  // No space left on device
static const uint32_t LINUX_ESPIPE          = 29;  // Illegal seek
static const uint32_t LINUX_EROFS           = 30;  // Read-only file system
static const uint32_t LINUX_EMLINK          = 31;  // Too many links
static const uint32_t LINUX_EPIPE           = 32;  // Broken pipe
static const uint32_t LINUX_EDOM            = 33;  // Math argument out of domain of func
static const uint32_t LINUX_ERANGE          = 34;  // Math result not representable
static const uint32_t LINUX_EDEADLK         = 35;  // Resource deadlock would occur
static const uint32_t LINUX_ENAMETOOLONG    = 36;  // File name too long
static const uint32_t LINUX_ENOLCK          = 37;  // No record locks available
static const uint32_t LINUX_ENOSYS          = 38;  // Function not implemented
static const uint32_t LINUX_ENOTEMPTY       = 39;  // Directory not empty
static const uint32_t LINUX_ELOOP           = 40;  // Too many symbolic links encountered
static const uint32_t LINUX_EWOULDBLOCK     = LINUX_EAGAIN; // Operation would block
static const uint32_t LINUX_ENOMSG          = 42;  // No message of desired type
static const uint32_t LINUX_EIDRM           = 43;  // Identifier removed
static const uint32_t LINUX_ECHRNG          = 44;  // Channel number out of range
static const uint32_t LINUX_EL2NSYNC        = 45;  // Level 2 not synchronized
static const uint32_t LINUX_EL3HLT          = 46;  // Level 3 halted
static const uint32_t LINUX_EL3RST          = 47;  // Level 3 reset
static const uint32_t LINUX_ELNRNG          = 48;  // Link number out of range
static const uint32_t LINUX_EUNATCH         = 49;  // Protocol driver not attached
static const uint32_t LINUX_ENOCSI          = 50;  // No CSI structure available
static const uint32_t LINUX_EL2HLT          = 51;  // Level 2 halted
static const uint32_t LINUX_EBADE           = 52;  // Invalid exchange
static const uint32_t LINUX_EBADR           = 53;  // Invalid request descriptor
static const uint32_t LINUX_EXFULL          = 54;  // Exchange full
static const uint32_t LINUX_ENOANO          = 55;  // No anode
static const uint32_t LINUX_EBADRQC         = 56;  // Invalid request code
static const uint32_t LINUX_EBADSLT         = 57;  // Invalid slot
static const uint32_t LINUX_EDEADLOCK       = LINUX_EDEADLK;
static const uint32_t LINUX_EBFONT          = 59;  // Bad font file format
static const uint32_t LINUX_ENOSTR          = 60;  // Device not a stream
static const uint32_t LINUX_ENODATA         = 61;  // No data available
static const uint32_t LINUX_ETIME           = 62;  // Timer expired
static const uint32_t LINUX_ENOSR           = 63;  // Out of streams resources
static const uint32_t LINUX_ENONET          = 64;  // Machine is not on the network
static const uint32_t LINUX_ENOPKG          = 65;  // Package not installed
static const uint32_t LINUX_EREMOTE         = 66;  // Object is remote
static const uint32_t LINUX_ENOLINK         = 67;  // Link has been severed
static const uint32_t LINUX_EADV            = 68;  // Advertise error
static const uint32_t LINUX_ESRMNT          = 69;  // Srmount error
static const uint32_t LINUX_ECOMM           = 70;  // Communication error on send
static const uint32_t LINUX_EPROTO          = 71;  // Protocol error
static const uint32_t LINUX_EMULTIHOP       = 72;  // Multihop attempted
static const uint32_t LINUX_EDOTDOT         = 73;  // RFS specific error
static const uint32_t LINUX_EBADMSG         = 74;  // Not a data message
static const uint32_t LINUX_EOVERFLOW       = 75;  // Value too large for defined data type
static const uint32_t LINUX_ENOTUNIQ        = 76;  // Name not unique on network
static const uint32_t LINUX_EBADFD          = 77;  // File descriptor in bad state
static const uint32_t LINUX_EREMCHG         = 78;  // Remote address changed
static const uint32_t LINUX_ELIBACC         = 79;  // Can not access a needed shared library
static const uint32_t LINUX_ELIBBAD         = 80;  // Accessing a corrupted shared library
static const uint32_t LINUX_ELIBSCN         = 81;  // .lib section in a.out corrupted
static const uint32_t LINUX_ELIBMAX         = 82;  // Attempting to link in too many shared libraries
static const uint32_t LINUX_ELIBEXEC        = 83;  // Cannot exec a shared library directly
static const uint32_t LINUX_EILSEQ          = 84;  // Illegal byte sequence
static const uint32_t LINUX_ERESTART        = 85;  // Interrupted system call should be restarted
static const uint32_t LINUX_ESTRPIPE        = 86;  // Streams pipe error
static const uint32_t LINUX_EUSERS          = 87;  // Too many users
static const uint32_t LINUX_ENOTSOCK        = 88;  // Socket operation on non-socket
static const uint32_t LINUX_EDESTADDRREQ    = 89;  // Destination address required
static const uint32_t LINUX_EMSGSIZE        = 90;  // Message too long
static const uint32_t LINUX_EPROTOTYPE      = 91;  // Protocol wrong type for socket
static const uint32_t LINUX_ENOPROTOOPT     = 92;  // Protocol not available
static const uint32_t LINUX_EPROTONOSUPPORT = 93;  // Protocol not supported
static const uint32_t LINUX_ESOCKTNOSUPPORT = 94;  // Socket type not supported
static const uint32_t LINUX_EOPNOTSUPP      = 95;  // Operation not supported on transport endpoint
static const uint32_t LINUX_EPFNOSUPPORT    = 96;  // Protocol family not supported
static const uint32_t LINUX_EAFNOSUPPORT    = 97;  // Address family not supported by protocol
static const uint32_t LINUX_EADDRINUSE      = 98;  // Address already in use
static const uint32_t LINUX_EADDRNOTAVAIL   = 99;  // Cannot assign requested address
static const uint32_t LINUX_ENETDOWN        = 100; // Network is down
static const uint32_t LINUX_ENETUNREACH     = 101; // Network is unreachable
static const uint32_t LINUX_ENETRESET       = 102; // Network dropped connection because of reset
static const uint32_t LINUX_ECONNABORTED    = 103; // Software caused connection abort
static const uint32_t LINUX_ECONNRESET      = 104; // Connection reset by peer
static const uint32_t LINUX_ENOBUFS         = 105; // No buffer space available
static const uint32_t LINUX_EISCONN         = 106; // Transport endpoint is already connected
static const uint32_t LINUX_ENOTCONN        = 107; // Transport endpoint is not connected
static const uint32_t LINUX_ESHUTDOWN       = 108; // Cannot send after transport endpoint shutdown
static const uint32_t LINUX_ETOOMANYREFS    = 109; // Too many references: cannot splice
static const uint32_t LINUX_ETIMEDOUT       = 110; // Connection timed out
static const uint32_t LINUX_ECONNREFUSED    = 111; // Connection refused
static const uint32_t LINUX_EHOSTDOWN       = 112; // Host is down
static const uint32_t LINUX_EHOSTUNREACH    = 113; // No route to host
static const uint32_t LINUX_EALREADY        = 114; // Operation already in progress
static const uint32_t LINUX_EINPROGRESS     = 115; // Operation now in progress
static const uint32_t LINUX_ESTALE          = 116; // Stale NFS file handle
static const uint32_t LINUX_EUCLEAN         = 117; // Structure needs cleaning
static const uint32_t LINUX_ENOTNAM         = 118; // Not a XENIX named type file
static const uint32_t LINUX_ENAVAIL         = 119; // No XENIX semaphores available
static const uint32_t LINUX_EISNAM          = 120; // Is a named type file
static const uint32_t LINUX_EREMOTEIO       = 121; // Remote I/O error
static const uint32_t LINUX_EDQUOT          = 122; // Quota exceeded
static const uint32_t LINUX_ENOMEDIUM       = 123; // No medium found
static const uint32_t LINUX_EMEDIUMTYPE     = 124; // Wrong medium type
static const uint32_t LINUX_ECANCELED       = 125; // Operation Canceled
static const uint32_t LINUX_ENOKEY          = 126; // Required key not available
static const uint32_t LINUX_EKEYEXPIRED     = 127; // Key has expired
static const uint32_t LINUX_EKEYREVOKED     = 128; // Key has been revoked
static const uint32_t LINUX_EKEYREJECTED    = 129; // Key was rejected by service
static const uint32_t LINUX_EOWNERDEAD      = 130; // Owner died
static const uint32_t LINUX_ENOTRECOVERABLE = 131; // State not recoverable
static const uint32_t LINUX_ERFKILL         = 132; // Operation not possible due to RF-kill
static const uint32_t LINUX_EHWPOISON       = 133; // Memory page has hardware error


} // end of namespace linux_os
} // end of namespace os
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_OS_LINUX_OS_ERRNO_HH__
