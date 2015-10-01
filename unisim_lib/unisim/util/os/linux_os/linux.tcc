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

#ifndef __UNISIM_UTIL_OS_LINUX_LINUX_TCC__
#define __UNISIM_UTIL_OS_LINUX_LINUX_TCC__

#if !defined(linux) && !defined(__linux) && !defined(__linux) && !defined(__APPLE_CC__) && !defined(WIN32) && !defined(WIN64)
#error "Unsupported host machine for Linux system call translation !"
#endif

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>

#if defined(WIN32) || defined(WIN64)
#include <stdlib.h>
#endif

#include "unisim/util/likely/likely.hh"
#include "unisim/util/debug/blob/blob.hh"
#include "unisim/util/endian/endian.hh"
#include "unisim/util/loader/elf_loader/elf32_loader.hh"

#include "unisim/util/os/linux_os/arm.hh"
#include "unisim/util/os/linux_os/ppc.hh"
#include "unisim/util/os/linux_os/environment.hh"
#include "unisim/util/os/linux_os/aux_table.hh"
#include "unisim/util/os/linux_os/errno.hh"

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

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
const int Linux<ADDRESS_TYPE, PARAMETER_TYPE>::kNumSupportedSystemTypes = 3;

const char * tmp_supported_system_types[] = {"arm", "arm-eabi", "ppc"};
template <class ADDRESS_TYPE, class PARAMETER_TYPE>
const std::vector<std::string> Linux<ADDRESS_TYPE, PARAMETER_TYPE>::
supported_system_types_(tmp_supported_system_types, 
                        tmp_supported_system_types + 
                        sizeof(tmp_supported_system_types) / sizeof(char *));

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::Linux(unisim::kernel::logger::Logger& logger,
      unisim::service::interfaces::Registers *regs_if, unisim::service::interfaces::Memory<ADDRESS_TYPE> *mem_if,
      unisim::service::interfaces::MemoryInjection<ADDRESS_TYPE> *mem_inject_if)
	: is_load_(false)
	, system_type_("arm-eabi")
	, endianness_(unisim::util::endian::E_LITTLE_ENDIAN)
	, load_files_()
	, entry_point_(0)
	, load_addr_set_(false)
	, load_addr_(0)
	, start_code_(0)
	, end_code_(0)
	, start_data_(0)
	, end_data_(0)
	, elf_stack_(0)
	, elf_brk_(0)
	, num_segments_(0)
	, stack_base_(0)
	, memory_page_size_(0)
	, mmap_base_(0xd4000000)
	, mmap_brk_point_(0)
	, brk_point_(0)
	, argc_(0)
	, argv_()
	, envc_(0)
	, envp_()
	, apply_host_environnement_(false)
	, target_envp_()
	, utsname_sysname_()
	, utsname_nodename_()
	, utsname_release_()
	, utsname_version_()
	, utsname_machine_()
	, utsname_domainname_()
	, blob_(NULL)
	, regs_if_(regs_if)
	, mem_if_(mem_if)
	, mem_inject_if_(mem_inject_if)
	, syscall_name_map_()
	, syscall_name_assoc_map_()
	, syscall_impl_assoc_map_()
	, current_syscall_id_(0)
	, current_syscall_name_("(NONE)")
	, verbose_(false)
	, parse_dwarf_(false)
	, debug_dwarf_(false)
	, dwarf_to_html_output_directory_()
	, dwarf_to_xml_output_filename_()
	, logger_(logger)
	, terminated_(false)
	, return_status_(0)
	, stdin_pipe_filename()
	, stdout_pipe_filename()
	, stderr_pipe_filename()
	, stdin_pipe_fd(-1)
	, stdout_pipe_fd(-1)
	, stderr_pipe_fd(-1)
{

#if defined(WIN32) || defined(WIN64)
	_setmode(0, _O_BINARY); // force binary mode for the standard input file descriptor (0)
	_setmode(1, _O_BINARY); // force binary mode for the standard output file descriptor (1)
	_setmode(2, _O_BINARY); // force binary mode for the standard error file descriptor (2)
#endif

#ifdef EPERM
	host2linux_errno[EPERM] = LINUX_EPERM;
#endif
#ifdef ENOENT
	host2linux_errno[ENOENT] = LINUX_ENOENT;
#endif
#ifdef ESRCH
	host2linux_errno[ESRCH] = LINUX_ESRCH;
#endif
#ifdef EINTR
	host2linux_errno[EINTR] = LINUX_EINTR;
#endif
#ifdef EIO
	host2linux_errno[EIO] = LINUX_EIO;
#endif
#ifdef ENXIO
	host2linux_errno[ENXIO] = LINUX_ENXIO;
#endif
#ifdef E2BIG
	host2linux_errno[E2BIG] = LINUX_E2BIG;
#endif
#ifdef ENOEXEC
	host2linux_errno[ENOEXEC] = LINUX_ENOEXEC;
#endif
#ifdef EBADF
	host2linux_errno[EBADF] = LINUX_EBADF;
#endif
#ifdef ECHILD
	host2linux_errno[ECHILD] = LINUX_ECHILD;
#endif
#ifdef EAGAIN
	host2linux_errno[EAGAIN] = LINUX_EAGAIN;
#endif
#ifdef ENOMEM
	host2linux_errno[ENOMEM] = LINUX_ENOMEM;
#endif
#ifdef EACCES
	host2linux_errno[EACCES] = LINUX_EACCES;
#endif
#ifdef EFAULT
	host2linux_errno[EFAULT] = LINUX_EFAULT;
#endif
#ifdef ENOTBLK
	host2linux_errno[ENOTBLK] = LINUX_ENOTBLK;
#endif
#ifdef EBUSY
	host2linux_errno[EBUSY] = LINUX_EBUSY;
#endif
#ifdef EEXIST
	host2linux_errno[EEXIST] = LINUX_EEXIST;
#endif
#ifdef EXDEV
	host2linux_errno[EXDEV] = LINUX_EXDEV;
#endif
#ifdef ENODEV
	host2linux_errno[ENODEV] = LINUX_ENODEV;
#endif
#ifdef ENOTDIR
	host2linux_errno[ENOTDIR] = LINUX_ENOTDIR;
#endif
#ifdef EISDIR
	host2linux_errno[EISDIR] = LINUX_EISDIR;
#endif
#ifdef EINVAL
	host2linux_errno[EINVAL] = LINUX_EINVAL;
#endif
#ifdef ENFILE
	host2linux_errno[ENFILE] = LINUX_ENFILE;
#endif
#ifdef EMFILE
	host2linux_errno[EMFILE] = LINUX_EMFILE;
#endif
#ifdef ENOTTY
	host2linux_errno[ENOTTY] = LINUX_ENOTTY;
#endif
#ifdef ETXTBSY
	host2linux_errno[ETXTBSY] = LINUX_ETXTBSY;
#endif
#ifdef EFBIG
	host2linux_errno[EFBIG] = LINUX_EFBIG;
#endif
#ifdef ENOSPC
	host2linux_errno[ENOSPC] = LINUX_ENOSPC;
#endif
#ifdef ESPIPE
	host2linux_errno[ESPIPE] = LINUX_ESPIPE;
#endif
#ifdef EROFS
	host2linux_errno[EROFS] = LINUX_EROFS;
#endif
#ifdef EMLINK
	host2linux_errno[EMLINK] = LINUX_EMLINK;
#endif
#ifdef EPIPE
	host2linux_errno[EPIPE] = LINUX_EPIPE;
#endif
#ifdef EDOM
	host2linux_errno[EDOM] = LINUX_EDOM;
#endif
#ifdef ERANGE
	host2linux_errno[ERANGE] = LINUX_ERANGE;
#endif
#ifdef EDEADLK
	host2linux_errno[EDEADLK] = LINUX_EDEADLK;
#endif
#ifdef ENAMETOOLONG
	host2linux_errno[ENAMETOOLONG] = LINUX_ENAMETOOLONG;
#endif
#ifdef ENOLCK
	host2linux_errno[ENOLCK] = LINUX_ENOLCK;
#endif
#ifdef ENOSYS
	host2linux_errno[ENOSYS] = LINUX_ENOSYS;
#endif
#ifdef ENOTEMPTY
	host2linux_errno[ENOTEMPTY] = LINUX_ENOTEMPTY;
#endif
#ifdef ELOOP
	host2linux_errno[ELOOP] = LINUX_ELOOP;
#endif
#ifdef EWOULDBLOCK
	host2linux_errno[EWOULDBLOCK] = LINUX_EWOULDBLOCK;
#endif
#ifdef ENOMSG
	host2linux_errno[ENOMSG] = LINUX_ENOMSG;
#endif
#ifdef EIDRM
	host2linux_errno[EIDRM] = LINUX_EIDRM;
#endif
#ifdef ECHRNG
	host2linux_errno[ECHRNG] = LINUX_ECHRNG;
#endif
#ifdef EL2NSYNC
	host2linux_errno[EL2NSYNC] = LINUX_EL2NSYNC;
#endif
#ifdef EL3HLT
	host2linux_errno[EL3HLT] = LINUX_EL3HLT;
#endif
#ifdef EL3RST
	host2linux_errno[EL3RST] = LINUX_EL3RST;
#endif
#ifdef ELNRNG
	host2linux_errno[ELNRNG] = LINUX_ELNRNG;
#endif
#ifdef EUNATCH
	host2linux_errno[EUNATCH] = LINUX_EUNATCH;
#endif
#ifdef ENOCSI
	host2linux_errno[ENOCSI] = LINUX_ENOCSI;
#endif
#ifdef EL2HLT
	host2linux_errno[EL2HLT] = LINUX_EL2HLT;
#endif
#ifdef EBADE
	host2linux_errno[EBADE] = LINUX_EBADE;
#endif
#ifdef EBADR
	host2linux_errno[EBADR] = LINUX_EBADR;
#endif
#ifdef EXFULL
	host2linux_errno[EXFULL] = LINUX_EXFULL;
#endif
#ifdef ENOANO
	host2linux_errno[ENOANO] = LINUX_ENOANO;
#endif
#ifdef EBADRQC
	host2linux_errno[EBADRQC] = LINUX_EBADRQC;
#endif
#ifdef EBADSLT
	host2linux_errno[EBADSLT] = LINUX_EBADSLT;
#endif
#ifdef EDEADLOCK
	host2linux_errno[EDEADLOCK] = LINUX_EDEADLOCK;
#endif
#ifdef EBFONT
	host2linux_errno[EBFONT] = LINUX_EBFONT;
#endif
#ifdef ENOSTR
	host2linux_errno[ENOSTR] = LINUX_ENOSTR;
#endif
#ifdef ENODATA
	host2linux_errno[ENODATA] = LINUX_ENODATA;
#endif
#ifdef ETIME
	host2linux_errno[ETIME] = LINUX_ETIME;
#endif
#ifdef ENOSR
	host2linux_errno[ENOSR] = LINUX_ENOSR;
#endif
#ifdef ENONET
	host2linux_errno[ENONET] = LINUX_ENONET;
#endif
#ifdef ENOPKG
	host2linux_errno[ENOPKG] = LINUX_ENOPKG;
#endif
#ifdef EREMOTE
	host2linux_errno[EREMOTE] = LINUX_EREMOTE;
#endif
#ifdef ENOLINK
	host2linux_errno[ENOLINK] = LINUX_ENOLINK;
#endif
#ifdef EADV
	host2linux_errno[EADV] = LINUX_EADV;
#endif
#ifdef ESRMNT
	host2linux_errno[ESRMNT] = LINUX_ESRMNT;
#endif
#ifdef ECOMM
	host2linux_errno[ECOMM] = LINUX_ECOMM;
#endif
#ifdef EPROTO
	host2linux_errno[EPROTO] = LINUX_EPROTO;
#endif
#ifdef EMULTIHOP
	host2linux_errno[EMULTIHOP] = LINUX_EMULTIHOP;
#endif
#ifdef EDOTDOT
	host2linux_errno[EDOTDOT] = LINUX_EDOTDOT;
#endif
#ifdef EBADMSG
	host2linux_errno[EBADMSG] = LINUX_EBADMSG;
#endif
#ifdef EOVERFLOW
	host2linux_errno[EOVERFLOW] = LINUX_EOVERFLOW;
#endif
#ifdef ENOTUNIQ
	host2linux_errno[ENOTUNIQ] = LINUX_ENOTUNIQ;
#endif
#ifdef EBADFD
	host2linux_errno[EBADFD] = LINUX_EBADFD;
#endif
#ifdef EREMCHG
	host2linux_errno[EREMCHG] = LINUX_EREMCHG;
#endif
#ifdef ELIBACC
	host2linux_errno[ELIBACC] = LINUX_ELIBACC;
#endif
#ifdef ELIBBAD
	host2linux_errno[ELIBBAD] = LINUX_ELIBBAD;
#endif
#ifdef ELIBSCN
	host2linux_errno[ELIBSCN] = LINUX_ELIBSCN;
#endif
#ifdef ELIBMAX
	host2linux_errno[ELIBMAX] = LINUX_ELIBMAX;
#endif
#ifdef ELIBEXEC
	host2linux_errno[ELIBEXEC] = LINUX_ELIBEXEC;
#endif
#ifdef EILSEQ
	host2linux_errno[EILSEQ] = LINUX_EILSEQ;
#endif
#ifdef ERESTART
	host2linux_errno[ERESTART] = LINUX_ERESTART;
#endif
#ifdef ESTRPIPE
	host2linux_errno[ESTRPIPE] = LINUX_ESTRPIPE;
#endif
#ifdef EUSERS
	host2linux_errno[EUSERS] = LINUX_EUSERS;
#endif
#ifdef ENOTSOCK
	host2linux_errno[ENOTSOCK] = LINUX_ENOTSOCK;
#endif
#ifdef EDESTADDRREQ
	host2linux_errno[EDESTADDRREQ] = LINUX_EDESTADDRREQ;
#endif
#ifdef EMSGSIZE
	host2linux_errno[EMSGSIZE] = LINUX_EMSGSIZE;
#endif
#ifdef EPROTOTYPE
	host2linux_errno[EPROTOTYPE] = LINUX_EPROTOTYPE;
#endif
#ifdef ENOPROTOOPT
	host2linux_errno[ENOPROTOOPT] = LINUX_ENOPROTOOPT;
#endif
#ifdef EPROTONOSUPPORT
	host2linux_errno[EPROTONOSUPPORT] = LINUX_EPROTONOSUPPORT;
#endif
#ifdef ESOCKTNOSUPPORT
	host2linux_errno[ESOCKTNOSUPPORT] = LINUX_ESOCKTNOSUPPORT;
#endif
#ifdef EOPNOTSUPP
	host2linux_errno[EOPNOTSUPP] = LINUX_EOPNOTSUPP;
#endif
#ifdef EPFNOSUPPORT
	host2linux_errno[EPFNOSUPPORT] = LINUX_EPFNOSUPPORT;
#endif
#ifdef EAFNOSUPPORT
	host2linux_errno[EAFNOSUPPORT] = LINUX_EAFNOSUPPORT;
#endif
#ifdef EADDRINUSE
	host2linux_errno[EADDRINUSE] = LINUX_EADDRINUSE;
#endif
#ifdef EADDRNOTAVAIL
	host2linux_errno[EADDRNOTAVAIL] = LINUX_EADDRNOTAVAIL;
#endif
#ifdef ENETDOWN
	host2linux_errno[ENETDOWN] = LINUX_ENETDOWN;
#endif
#ifdef ENETUNREACH
	host2linux_errno[ENETUNREACH] = LINUX_ENETUNREACH;
#endif
#ifdef ENETRESET
	host2linux_errno[ENETRESET] = LINUX_ENETRESET;
#endif
#ifdef ECONNABORTED
	host2linux_errno[ECONNABORTED] = LINUX_ECONNABORTED;
#endif
#ifdef ECONNRESET
	host2linux_errno[ECONNRESET] = LINUX_ECONNRESET;
#endif
#ifdef ENOBUFS
	host2linux_errno[ENOBUFS] = LINUX_ENOBUFS;
#endif
#ifdef EISCONN
	host2linux_errno[EISCONN] = LINUX_EISCONN;
#endif
#ifdef ENOTCONN
	host2linux_errno[ENOTCONN] = LINUX_ENOTCONN;
#endif
#ifdef ESHUTDOWN
	host2linux_errno[ESHUTDOWN] = LINUX_ESHUTDOWN;
#endif
#ifdef ETOOMANYREFS
	host2linux_errno[ETOOMANYREFS] = LINUX_ETOOMANYREFS;
#endif
#ifdef ETIMEDOUT
	host2linux_errno[ETIMEDOUT] = LINUX_ETIMEDOUT;
#endif
#ifdef ECONNREFUSED
	host2linux_errno[ECONNREFUSED] = LINUX_ECONNREFUSED;
#endif
#ifdef EHOSTDOWN
	host2linux_errno[EHOSTDOWN] = LINUX_EHOSTDOWN;
#endif
#ifdef EHOSTUNREACH
	host2linux_errno[EHOSTUNREACH] = LINUX_EHOSTUNREACH;
#endif
#ifdef EALREADY
	host2linux_errno[EALREADY] = LINUX_EALREADY;
#endif
#ifdef EINPROGRESS
	host2linux_errno[EINPROGRESS] = LINUX_EINPROGRESS;
#endif
#ifdef ESTALE
	host2linux_errno[ESTALE] = LINUX_ESTALE;
#endif
#ifdef EUCLEAN
	host2linux_errno[EUCLEAN] = LINUX_EUCLEAN;
#endif
#ifdef ENOTNAM
	host2linux_errno[ENOTNAM] = LINUX_ENOTNAM;
#endif
#ifdef ENAVAIL
	host2linux_errno[ENAVAIL] = LINUX_ENAVAIL;
#endif
#ifdef EISNAM
	host2linux_errno[EISNAM] = LINUX_EISNAM;
#endif
#ifdef EREMOTEIO
	host2linux_errno[EREMOTEIO] = LINUX_EREMOTEIO;
#endif
#ifdef EDQUOT
	host2linux_errno[EDQUOT] = LINUX_EDQUOT;
#endif
#ifdef ENOMEDIUM
	host2linux_errno[ENOMEDIUM] = LINUX_ENOMEDIUM;
#endif
#ifdef EMEDIUMTYPE
	host2linux_errno[EMEDIUMTYPE] = LINUX_EMEDIUMTYPE;
#endif
#ifdef ECANCELED
	host2linux_errno[ECANCELED] = LINUX_ECANCELED;
#endif
#ifdef ENOKEY
	host2linux_errno[ENOKEY] = LINUX_ENOKEY;
#endif
#ifdef EKEYEXPIRED
	host2linux_errno[EKEYEXPIRED] = LINUX_EKEYEXPIRED;
#endif
#ifdef EKEYREVOKED
	host2linux_errno[EKEYREVOKED] = LINUX_EKEYREVOKED;
#endif
#ifdef EKEYREJECTED
	host2linux_errno[EKEYREJECTED] = LINUX_EKEYREJECTED;
#endif
#ifdef EOWNERDEAD
	host2linux_errno[EOWNERDEAD] = LINUX_EOWNERDEAD;
#endif
#ifdef ENOTRECOVERABLE
	host2linux_errno[ENOTRECOVERABLE] = LINUX_ENOTRECOVERABLE;
#endif
#ifdef ERFKILL
	host2linux_errno[ERFKILL] = LINUX_ERFKILL;
#endif
#ifdef EHWPOISON
	host2linux_errno[EHWPOISON] = LINUX_EHWPOISON;
#endif
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::~Linux()
{
	typename std::map<std::string, unisim::util::debug::blob::Blob<ADDRESS_TYPE> const *>::const_iterator it;
	for(it = load_files_.begin(); it != load_files_.end(); it++)
	{
		const unisim::util::debug::blob::Blob<ADDRESS_TYPE> *blob = (*it).second;
		blob->Release();
	}
	
	load_files_.clear();
	
	if(blob_)
	{
		blob_->Release();
		blob_ = 0;
	}
	
	if(stdin_pipe_fd != -1) close(stdin_pipe_fd);
	
	if(stdout_pipe_fd != -1) close(stdout_pipe_fd);
	
	if(stderr_pipe_fd != -1) close(stderr_pipe_fd);
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetVerbose(bool verbose) {
  verbose_ = verbose;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetParseDWARF(bool parse_dwarf) {
  parse_dwarf_ = parse_dwarf;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetDebugDWARF(bool debug_dwarf) {
  debug_dwarf_ = debug_dwarf;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetDWARFToHTMLOutputDirectory(const char *dwarf_to_html_output_directory)
{
	dwarf_to_html_output_directory_ = dwarf_to_html_output_directory;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetDWARFToXMLOutputFilename(const char *dwarf_to_xml_output_filename)
{
	dwarf_to_xml_output_filename_ = dwarf_to_xml_output_filename;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetCommandLine(
    std::vector<std::string> const &cmd) {
  argc_ = cmd.size();
  argv_ = cmd;

  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
std::vector<std::string> Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetCommandLine() {
  return argv_;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetEnvironment(
    std::vector<std::string> const &env) {
  envc_ = env.size();
  envp_ = env;

  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
std::vector<std::string> Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetEnvironment() {
  return envp_;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetApplyHostEnvironment(
    bool use_host_environment) {
  apply_host_environnement_ = use_host_environment;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetApplyHostEnvironment() {
  return apply_host_environnement_;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::AddLoadFile(
    char const * const filename) {
  // NOTE: for the moment we only support one file to load, if this
  // method is called more than once it will return false.
  if (load_files_.size() != 0) return false;

  // check that filename points to something
  if (filename == NULL) {
    logger_ << DebugWarning << "calling AddLoadFile "
        << "without handling file." << EndDebugWarning;
    return false;
  }

  // check that the file exists and that the elf loader can create a blob from it
  unisim::util::loader::elf_loader::Elf32Loader<ADDRESS_TYPE> *loader =
      new unisim::util::loader::elf_loader::Elf32Loader<ADDRESS_TYPE>(
          logger_, regs_if_, mem_if_);

  if (loader == NULL) {
    logger_ << DebugError
        << "Could not create an elf loader."
        << EndDebugError;
    return false;
  }

  loader->SetOption(unisim::util::loader::elf_loader::OPT_VERBOSE, verbose_);
  loader->SetOption(unisim::util::loader::elf_loader::OPT_FILENAME, filename);
  loader->SetOption(unisim::util::loader::elf_loader::OPT_PARSE_DWARF, parse_dwarf_);
  loader->SetOption(unisim::util::loader::elf_loader::OPT_DEBUG_DWARF, debug_dwarf_);
  loader->SetOption(unisim::util::loader::elf_loader::OPT_DWARF_TO_HTML_OUTPUT_DIRECTORY, dwarf_to_html_output_directory_.c_str());
  loader->SetOption(unisim::util::loader::elf_loader::OPT_DWARF_TO_XML_OUTPUT_FILENAME, dwarf_to_xml_output_filename_.c_str());

  if (!loader->Load()) {
    logger_ << DebugError
        << "Could not load the given file "
        << "\"" << filename << "\"" << EndDebugError;
    delete loader;
    return false;
  }

  unisim::util::debug::blob::Blob<ADDRESS_TYPE> const * const blob =
      loader->GetBlob();
  if (blob == NULL) {
    logger_ << DebugError << "Could not create blob from"
        << " requested file (" << filename << ")." << EndDebugError;
    delete loader;
    return false;
  }

  blob->Catch();
  std::string filename_str(filename);
  if(load_files_.find(filename_str) != load_files_.end()) load_files_[filename_str]->Release();
  load_files_[filename_str] = blob;
  delete loader;
  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetSystemType(
    char const * const system_type) {
  bool supported = false;
  for (int i = 0; (!supported) && (i < kNumSupportedSystemTypes); ++i) {
    supported = (supported_system_types_[i].compare(system_type) == 0);
  }
  if (!supported) return false;
  system_type_ = system_type;
  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetEndianness(
    unisim::util::endian::endian_type endianness) {
  endianness_ = endianness;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetStackBase(
    ADDRESS_TYPE stack_base) {
  stack_base_ = stack_base;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetMemoryPageSize(
    ADDRESS_TYPE memory_page_size) {
  memory_page_size_ = memory_page_size;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetUname(
    char const * const utsname_sysname,
    char const * const utsname_nodename,
    char const * const utsname_release,
    char const * const utsname_version,
    char const * const utsname_machine,
    char const * const utsname_domainname) {
  utsname_sysname_    = utsname_sysname;
  utsname_nodename_   = utsname_nodename;
  utsname_release_    = utsname_release;
  utsname_version_    = utsname_version;
  utsname_machine_    = utsname_machine;
  utsname_domainname_ = utsname_domainname;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetHWCap(const char *hwcap)
{
  hwcap_ = hwcap;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetStdinPipeFilename(const char *filename)
{
	stdin_pipe_filename = filename;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetStdoutPipeFilename(const char *filename)
{
	stdout_pipe_filename = filename;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetStderrPipeFilename(const char *filename)
{
	stderr_pipe_filename = filename;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
unisim::util::debug::Register * Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetRegisterFromId(uint32_t id) {
  if (!regs_if_) return NULL;
  char const * reg_name = 0;
  if ((system_type_.compare("arm") == 0) || (system_type_.compare("arm-eabi") == 0)) {
    switch(id) {
      case unisim::util::os::linux_os::kARM_r0: reg_name = "r0"; break;
      case unisim::util::os::linux_os::kARM_r1: reg_name = "r1"; break;
      case unisim::util::os::linux_os::kARM_r2: reg_name = "r2"; break;
      case unisim::util::os::linux_os::kARM_r3: reg_name = "r3"; break;
      case unisim::util::os::linux_os::kARM_r4: reg_name = "r4"; break;
      case unisim::util::os::linux_os::kARM_r5: reg_name = "r5"; break;
      case unisim::util::os::linux_os::kARM_r6: reg_name = "r6"; break;
      case unisim::util::os::linux_os::kARM_r7: reg_name = "r7"; break;
      case unisim::util::os::linux_os::kARM_r8: reg_name = "r8"; break;
      case unisim::util::os::linux_os::kARM_r9: reg_name = "r9"; break;
      case unisim::util::os::linux_os::kARM_r10: reg_name = "r10"; break;
      case unisim::util::os::linux_os::kARM_r11: reg_name = "r11"; break;
      case unisim::util::os::linux_os::kARM_r12: reg_name = "r12"; break;
      case unisim::util::os::linux_os::kARM_sp: reg_name = "sp"; break;
      case unisim::util::os::linux_os::kARM_lr: reg_name = "lr"; break;
      case unisim::util::os::linux_os::kARM_pc: reg_name = "pc"; break;
      default: return NULL;
    }
  } else if (system_type_.compare("ppc") == 0) {
    switch(id) {
      case unisim::util::os::linux_os::kPPC_r0: reg_name = "r0"; break;
      case unisim::util::os::linux_os::kPPC_r1: reg_name = "r1"; break;
      case unisim::util::os::linux_os::kPPC_r2: reg_name = "r2"; break;
      case unisim::util::os::linux_os::kPPC_r3: reg_name = "r3"; break;
      case unisim::util::os::linux_os::kPPC_r4: reg_name = "r4"; break;
      case unisim::util::os::linux_os::kPPC_r5: reg_name = "r5"; break;
      case unisim::util::os::linux_os::kPPC_r6: reg_name = "r6"; break;
      case unisim::util::os::linux_os::kPPC_r7: reg_name = "r7"; break;
      case unisim::util::os::linux_os::kPPC_r8: reg_name = "r8"; break;
      case unisim::util::os::linux_os::kPPC_r9: reg_name = "r9"; break;
      case unisim::util::os::linux_os::kPPC_r10: reg_name = "r10"; break;
      case unisim::util::os::linux_os::kPPC_r11: reg_name = "r11"; break;
      case unisim::util::os::linux_os::kPPC_r12: reg_name = "r12"; break;
      case unisim::util::os::linux_os::kPPC_r13: reg_name = "r13"; break;
      case unisim::util::os::linux_os::kPPC_r14: reg_name = "r14"; break;
      case unisim::util::os::linux_os::kPPC_r15: reg_name = "r15"; break;
      case unisim::util::os::linux_os::kPPC_r16: reg_name = "r16"; break;
      case unisim::util::os::linux_os::kPPC_r17: reg_name = "r17"; break;
      case unisim::util::os::linux_os::kPPC_r18: reg_name = "r18"; break;
      case unisim::util::os::linux_os::kPPC_r19: reg_name = "r19"; break;
      case unisim::util::os::linux_os::kPPC_r20: reg_name = "r20"; break;
      case unisim::util::os::linux_os::kPPC_r21: reg_name = "r21"; break;
      case unisim::util::os::linux_os::kPPC_r22: reg_name = "r22"; break;
      case unisim::util::os::linux_os::kPPC_r23: reg_name = "r23"; break;
      case unisim::util::os::linux_os::kPPC_r24: reg_name = "r24"; break;
      case unisim::util::os::linux_os::kPPC_r25: reg_name = "r25"; break;
      case unisim::util::os::linux_os::kPPC_r26: reg_name = "r26"; break;
      case unisim::util::os::linux_os::kPPC_r27: reg_name = "r27"; break;
      case unisim::util::os::linux_os::kPPC_r28: reg_name = "r28"; break;
      case unisim::util::os::linux_os::kPPC_r29: reg_name = "r29"; break;
      case unisim::util::os::linux_os::kPPC_r30: reg_name = "r30"; break;
      case unisim::util::os::linux_os::kPPC_r31: reg_name = "r31"; break;
      case unisim::util::os::linux_os::kPPC_cr: reg_name = "cr"; break;
      case unisim::util::os::linux_os::kPPC_cia: reg_name = "cia"; break;
      default: return NULL;
    }
  }
  return regs_if_->GetRegister(reg_name);
}


template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetRegister(uint32_t id,
    PARAMETER_TYPE * const value) {
  unisim::util::debug::Register *reg = 0;
  reg = GetRegisterFromId(id);
  if (reg == NULL) return false;
  if (reg->GetSize() != sizeof(PARAMETER_TYPE)) return false;
  reg->GetValue(value);
  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetRegister(uint32_t id,
    PARAMETER_TYPE value) {
  unisim::util::debug::Register *reg = 0;
  reg = GetRegisterFromId(id);
  if (reg == NULL) return false;
  if (reg->GetSize() != sizeof(PARAMETER_TYPE)) return false;
  reg->SetValue(&value);
  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::Load()
{
	// set the environment to be used
	if (apply_host_environnement_) GetHostEnvironment(envp_, &target_envp_);
	else target_envp_ = envp_;

	// create the root blob that will be used to store the image that will be
	// loaded
	unisim::util::debug::blob::Blob<ADDRESS_TYPE>* blob =
	new unisim::util::debug::blob::Blob<ADDRESS_TYPE>();
	blob->Catch();
	blob->SetFileFormat(unisim::util::debug::blob::FFMT_ELF32);
	
	// load and add files to the blob
	if(verbose_)
	{
		logger_ << DebugInfo
			<< "Loading elf files." << EndDebugInfo;
	}
	if(!LoadFiles(blob))
	{
		logger_ << DebugError
			<< "Could not load elf files." << EndDebugError;
		blob->Release();
		return false;
	}

	// create the stack footprint and add it to the blob
	uint64_t stack_size = 0;
	if(verbose_)
	{
		logger_ << DebugInfo
			<< "Creating the Linux software stack." << EndDebugInfo;
	}
	if (!CreateStack(blob, stack_size))
	{
		// TODO
		// Remove non finished state (i.e., unfinished blob, reset values, ...)
		logger_ << DebugError
			<< "Could not create the Linux software stack." << EndDebugError;
		blob->Release();
		return false;
	}

	// finish the state of the memory image depending on the system we are running
	// on
	if(verbose_)
	{
		logger_ << DebugInfo
			<< "Setting the system blob." << EndDebugInfo;
	}
	if (!SetSystemBlob(blob))
	{
		// TODO
		// Remove non finished state (i.e., unfinished blob, reset values, ...)
		logger_ << DebugError
			<< "Could not set the system blob." << EndDebugError;
		blob->Release();
		return false;
	}

	// Set the linux calls maps
	syscall_impl_assoc_map_.clear();
	SetSyscallNameMap();
	if ((system_type_.compare("arm") == 0) ||
	(system_type_.compare("arm-eabi") == 0))
	{
		if(!SetupLinuxOSARM())
		{
			logger_ << DebugError
				<< "Error while trying to setup the linux os arm"
				<< EndDebugError;
			blob->Release();
			return false;
		}
	}
	
	if(system_type_.compare("ppc") == 0)
	{
		if(!SetupLinuxOSPPC())
		{
			logger_ << DebugError
				<< "Error while trying to setup the linux os ppc"
				<< EndDebugError;
			blob->Release();
			return false;
		}
	}

	// Set mmap_brk_point and brk_point
	mmap_brk_point_ = mmap_base_;

	ADDRESS_TYPE top_addr = stack_base_ + stack_size - 1;
	if(verbose_)
	{
		logger_ << DebugInfo << "=== top_addr = 0x" << std::hex << top_addr << std::dec
		<< EndDebugInfo;
	}

	brk_point_ = top_addr +
	(memory_page_size_ - (top_addr % memory_page_size_));

	if(verbose_)
	{
		logger_ << DebugInfo
			<< "=== stack_size_ = 0x" << std::hex << stack_size << " (" << std::dec << stack_size << ")" << std::endl
			<< "=== brk_point_ = 0x" << std::hex << brk_point_ << std::endl
			<< "=== mmap_brk_point_ = 0x" << mmap_brk_point_ << std::endl
			<< "=== mmap_base_ = 0x" << mmap_base_ << std::endl
			<< "=== memory_page_size_ = 0x" << memory_page_size_ << " ("
			<< std::dec << memory_page_size_ << ")" << EndDebugInfo;
	}

	if(blob_) blob_->Release();
	blob_ = blob;
	is_load_ = true;

	if(!stdin_pipe_filename.empty())
	{
		int stdin_pipe_flags = O_RDONLY;
#if defined(WIN32) || defined(WIN64)
		stdin_pipe_flags |= O_BINARY;
#endif
		stdin_pipe_fd = open(stdin_pipe_filename.c_str(), stdin_pipe_flags);
		if(stdin_pipe_fd == -1)
		{
			logger_ << DebugError << "Can't open \"" << stdin_pipe_filename << "\"" << EndDebugError;
			return false;
		}
	}
	
	if(!stdout_pipe_filename.empty())
	{
		int stdout_pipe_flags = O_WRONLY | O_CREAT | O_TRUNC;
		mode_t stdout_pipe_mode = S_IRWXU;
#if defined(WIN32) || defined(WIN64)
		stdout_pipe_flags |= O_BINARY;
#else
		stdout_pipe_mode |= S_IRWXG | S_IRWXO;
#endif
		stdout_pipe_fd = open(stdout_pipe_filename.c_str(), stdout_pipe_flags, stdout_pipe_mode);
		if(stdout_pipe_fd == -1)
		{
			logger_ << DebugError << "Can't open \"" << stdout_pipe_filename << "\"" << EndDebugError;
			return false;
		}
	}
	
	if(!stderr_pipe_filename.empty())
	{
		int stderr_pipe_flags = O_WRONLY | O_CREAT | O_TRUNC;
		mode_t stderr_pipe_mode = S_IRWXU;
#if defined(WIN32) || defined(WIN64)
		stderr_pipe_flags |= O_BINARY;
#else
		stderr_pipe_mode |= S_IRWXG | S_IRWXO;
#endif
		stderr_pipe_fd = open(stderr_pipe_filename.c_str(), stderr_pipe_flags, stderr_pipe_mode);
		if(stderr_pipe_fd == -1)
		{
			logger_ << DebugError << "Can't open \"" << stderr_pipe_filename << "\"" << EndDebugError;
			return false;
		}
	}

	MapTargetToHostFileDescriptor(0, (stdin_pipe_fd == -1) ? 0 : stdin_pipe_fd); // map target stdin file descriptor to either host stdin file descriptor or host input file descriptor
	MapTargetToHostFileDescriptor(1, (stdout_pipe_fd == -1) ? 1 : stdout_pipe_fd); // map target stdout file descriptor to either host stdout file descriptor or host output file descriptor
	MapTargetToHostFileDescriptor(2, (stderr_pipe_fd == -1) ? 2 : stderr_pipe_fd); // map target stdout file descriptor to either host stderr file descriptor or host output file descriptor

	return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::IsLoad() {
  return blob_ != NULL;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetupTarget() {
  if (mem_if_ == NULL
      || mem_inject_if_ == NULL
      || regs_if_ == NULL) {
    logger_ << DebugError
        << "The linux system interfaces (memory/register) were not"
        << " assigned" << EndDebugError;
    return false;
  }

  if (blob_ == NULL) {
    logger_ << DebugError
        << "The linux system was not loaded." << EndDebugError;
    return false;
  }

  if ((system_type_.compare("arm") == 0) || 
      (system_type_.compare("arm-eabi") == 0))
    return SetupARMTarget();
  else if (system_type_.compare("ppc") == 0)
    return SetupPPCTarget();

  logger_ << DebugError
      << "Unknown system to setup (" << system_type_ << ")." << EndDebugError;
  return false;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetupARMTarget() {
  typedef unisim::util::debug::blob::Segment<ADDRESS_TYPE> Segment;
  typedef std::vector<const Segment*> SegmentVector;
  typedef typename SegmentVector::const_iterator SegmentVectorIterator;

  bool success = true;
  SegmentVector segments = blob_->GetSegments();
  for (SegmentVectorIterator it = segments.begin();
       success && (it != segments.end()); it++) {
    if ((*it)->GetType() == Segment::TY_LOADABLE) {
      uint8_t const * data = (uint8_t const *)(*it)->GetData();
      ADDRESS_TYPE start, end;
      (*it)->GetAddrRange(start, end);

      if(verbose_) {
        logger_ << DebugInfo << "--> writing memory segment start = 0x"
            << std::hex << start << " end = 0x" << end << std::dec
            << EndDebugInfo;
      }

      success = mem_if_->WriteMemory(start, data, end - start + 1);
    }
  }

  if (!success) {
    logger_ << DebugError
        << "Error while writing the segments into the target memory."
        << EndDebugError;
    return false;
  }

  // Reset all the target registers
  unsigned int reg_id = 0;
  PARAMETER_TYPE null_param = 0;
  for (reg_id = 0; success && (reg_id < kARMNumRegs); reg_id++) {
    success = SetRegister(reg_id, null_param);
  }
  if (!success) {
    logger_ << DebugError
        << "Error while setting register '" << (reg_id - 1) << "'" << EndDebugError;
    return false;
  }
  // Set PC to the program entry point
  success = SetRegister(kARM_pc, entry_point_);
  if (!success) {
    logger_ << DebugError
        << "Error while setting pc register (" << kARM_pc << ")" << EndDebugError;
    return false;
  }
  // Set SP to the base of the created stack
  unisim::util::debug::blob::Section<ADDRESS_TYPE> const * sp_section =
      blob_->FindSection(".unisim.linux_os.stack.stack_pointer");
  if (sp_section == NULL) {
    logger_ << DebugError
        << "Could not find the stack pointer section." << EndDebugError;
    return false;
  }
  success = SetRegister(kARM_sp, sp_section->GetAddr());
  if (!success) {
    logger_ << DebugError
        << "Error while setting sp register (" << kARM_sp << ")" << EndDebugError;
    return false;
  }
  ADDRESS_TYPE par1_addr = sp_section->GetAddr() + 4;
  ADDRESS_TYPE par2_addr = sp_section->GetAddr() + 8;
  PARAMETER_TYPE par1 = 0;
  PARAMETER_TYPE par2 = 0;
  success = mem_if_->ReadMemory(par1_addr, (uint8_t *)&par1, sizeof(par1)); 
  success = mem_if_->ReadMemory(par2_addr, (uint8_t *)&par2, sizeof(par2));
  SetRegister(kARM_r1, Target2Host(endianness_, par1));
  SetRegister(kARM_r2, Target2Host(endianness_, par2));


  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetupPPCTarget() {
  typedef unisim::util::debug::blob::Segment<ADDRESS_TYPE> Segment;
  typedef std::vector<const Segment*> SegmentVector;
  typedef typename SegmentVector::const_iterator SegmentVectorIterator;

  bool success = true;
  SegmentVector segments = blob_->GetSegments();
  for (SegmentVectorIterator it = segments.begin();
       success && (it != segments.end()); it++) {
    if ((*it)->GetType() == Segment::TY_LOADABLE) {
      uint8_t const * data = (uint8_t const *)(*it)->GetData();
      ADDRESS_TYPE start, end;
      (*it)->GetAddrRange(start, end);

      if(verbose_) {
         logger_ << DebugInfo << "--> writing memory segment start = 0x"
            << std::hex << start << " end = 0x" << end << std::dec
            << EndDebugInfo;
      }

      success = mem_if_->WriteMemory(start, data, end - start + 1);
    }
  }

  if (!success) {
    logger_ << DebugError
        << "Error while writing the segments into the target memory."
        << EndDebugError;
    return false;
  }

  // Reset all the target registers
  unsigned int reg_id = 0;
  PARAMETER_TYPE null_param = 0;
  for (reg_id = 0; success && (reg_id < kPPCNumRegs); reg_id++) {
    success = SetRegister(reg_id, null_param);
  }
  if (!success) {
    logger_ << DebugError
        << "Error while setting register '" << (reg_id - 1) << "'" << EndDebugError;
    return false;
  }
  // Set PC to the program entry point
  success = SetRegister(kPPC_cia, entry_point_);
  if (!success) {
    logger_ << DebugError
        << "Error while setting cia register (" << kPPC_cia << ")" << EndDebugError;
    return false;
  }
  // Set SP to the base of the created stack
  unisim::util::debug::blob::Section<ADDRESS_TYPE> const * sp_section =
      blob_->FindSection(".unisim.linux_os.stack.stack_pointer");
  if (sp_section == NULL) {
    logger_ << DebugError
        << "Could not find the stack pointer section." << EndDebugError;
    return false;
  }
  success = SetRegister(kPPC_sp, sp_section->GetAddr());
  if (!success) {
    logger_ << DebugError
        << "Error while setting sp register (" << kPPC_sp << ")" << DebugError;
    return false;
  }
  ADDRESS_TYPE par1_addr = sp_section->GetAddr() + 4;
  ADDRESS_TYPE par2_addr = sp_section->GetAddr() + 8;
  PARAMETER_TYPE par1 = 0;
  PARAMETER_TYPE par2 = 0;
  success = mem_if_->ReadMemory(par1_addr, (uint8_t *)&par1, sizeof(par1)); 
  success = mem_if_->ReadMemory(par2_addr, (uint8_t *)&par2, sizeof(par2));
  SetRegister(kPPC_r3, Target2Host(endianness_, par1));
  SetRegister(kPPC_r4, Target2Host(endianness_, par2));

  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
unisim::util::debug::blob::Blob<ADDRESS_TYPE> const * const Linux<ADDRESS_TYPE,
    PARAMETER_TYPE>::GetBlob() const { return blob_; }

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::ExecuteSystemCall(int id, bool& terminated, int& return_status) {
	if(!is_load_)
	{
		logger_
			<< DebugError
			<< "unisim::util::os::linux_os::Linux.ExecuteSystemCall: "
			<< "Trying to execute system call with id " << id << " while the linux "
			<< "emulation has not been correctly loaded."
			<< EndDebugError;
		return;
	}

	int translated_id = GetSyscallNumber(id);

	current_syscall_id_ = translated_id;
	current_syscall_name_ = GetSyscallName(translated_id);
	
	if(unlikely(verbose_))
	{
		logger_
			<< DebugInfo
			<< "Executing syscall(name = " << current_syscall_name_ << ";"
			<< " id = " << translated_id << ";"
			<< " unstranslated id = " << id << ")"
			<< EndDebugInfo;
	}
	
	syscall_t y = GetSyscallImpl(translated_id);
	(this->*y)();

	if(terminated_)
	{
		terminated = true;
		return_status = return_status_;
	}
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LoadFiles(
    unisim::util::debug::blob::Blob<ADDRESS_TYPE> *blob) {
  // Get the main executable blob
  // NOTE: current implementation of the linux os library only supports one
  // blob, the main executable blob
  unisim::util::debug::blob::Blob<ADDRESS_TYPE> const * const main_blob =
      GetMainBlob();

  if (main_blob == NULL) return false;

  if(main_blob->GetCapability() & unisim::util::debug::blob::CAP_FILENAME) {
    blob->SetFilename(main_blob->GetFilename());
  }
  if(main_blob->GetCapability() & unisim::util::debug::blob::CAP_ENTRY_POINT) {
    blob->SetEntryPoint(main_blob->GetEntryPoint());
  }
  if(main_blob->GetCapability() & unisim::util::debug::blob::CAP_ARCHITECTURE) {
    blob->SetArchitecture(main_blob->GetArchitecture());
  }
  if(main_blob->GetCapability() & unisim::util::debug::blob::CAP_STACK_BASE) {
    blob->SetStackBase(main_blob->GetStackBase());
  }
  if(main_blob->GetCapability() & unisim::util::debug::blob::CAP_ENDIAN) {
    blob->SetEndian(main_blob->GetEndian());
  }
  if(main_blob->GetCapability() & unisim::util::debug::blob::CAP_FILE_ENDIAN) {
    blob->SetFileEndian(main_blob->GetFileEndian());
  }
  if(main_blob->GetCapability() & unisim::util::debug::blob::CAP_ADDRESS_SIZE) {
    blob->SetAddressSize(main_blob->GetAddressSize());
  }
  if(main_blob->GetCapability() & unisim::util::debug::blob::CAP_ELF_PHOFF) {
    blob->SetELF_PHOFF(main_blob->GetELF_PHOFF());
  }
  if(main_blob->GetCapability() & unisim::util::debug::blob::CAP_ELF_PHENT) {
    blob->SetELF_PHENT(main_blob->GetELF_PHENT());
  }

  // compute the different structure addresses from the given blob
  if (!ComputeStructuralAddresses(*main_blob))
    return false;

  if (!FillBlobWithFileBlob(*main_blob, blob))
    return false;

  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
unisim::util::debug::blob::Blob<ADDRESS_TYPE> const * const Linux<ADDRESS_TYPE,
    PARAMETER_TYPE>:: GetMainBlob() const {
  /* NOTE only one file is supported, so we just get the first one if any */
  typename std::map<std::string,
      unisim::util::debug::blob::Blob<ADDRESS_TYPE> const *>::const_iterator
      it = load_files_.begin();
  if (it == load_files_.end()) return NULL;
  else return it->second;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::ComputeStructuralAddresses(
    unisim::util::debug::blob::Blob<ADDRESS_TYPE> const &blob) {
  int loaded_segments = 0;
  typedef unisim::util::debug::blob::Segment<ADDRESS_TYPE> Segment;
  typedef std::vector<const Segment *> SegmentVector;
  typedef typename SegmentVector::const_iterator SegmentVectorIterator;
  const SegmentVector &segments = blob.GetSegments();

  // Reset application structure addresses
  entry_point_ = blob.GetEntryPoint();
  load_addr_ = ~((ADDRESS_TYPE)0);
  start_code_ = ~((ADDRESS_TYPE)0);
  end_code_ = 0;
  start_data_ = 0;
  end_data_ = 0;
  elf_stack_ = ~((ADDRESS_TYPE)0);
  elf_brk_ = 0;
  num_segments_ = 0;

  for (SegmentVectorIterator it = segments.begin(); it != segments.end(); it++) {
    typename unisim::util::debug::blob::Segment<ADDRESS_TYPE>::Type type =
        (*it)->GetType();
    // ignore the segment if it is not a loadable
    if (type != unisim::util::debug::blob::Segment<ADDRESS_TYPE>::TY_LOADABLE)
      continue;

    loaded_segments++;

    ADDRESS_TYPE segment_addr = (*it)->GetAddr();
    ADDRESS_TYPE segment_end_addr = segment_addr + (*it)->GetSize();
    if (load_addr_ > segment_addr) {
      // TODO
      // WARNING
      // We are not considering the offset in the elf file. It would be better
      // to set load_addr = segment_addr - segment_offset
      load_addr_ = segment_addr;
    }
    if (segment_addr < start_code_)
      start_code_ = segment_addr;
    if (start_data_ < segment_addr)
      start_data_ = segment_addr;
    if ((*it)->GetAttr() & Segment::SA_X) {
      if (end_code_ < segment_end_addr)
        end_code_ = segment_end_addr;
    }
    if (end_data_ < segment_end_addr)
      end_data_ = segment_end_addr;
    if (segment_end_addr > elf_brk_)
      elf_brk_ = segment_end_addr;
  }

  if (loaded_segments == 0) return false;
  num_segments_ = segments.size();
  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::FillBlobWithFileBlob(
    unisim::util::debug::blob::Blob<ADDRESS_TYPE> const &file_blob,
    unisim::util::debug::blob::Blob<ADDRESS_TYPE> *blob) {
  typedef unisim::util::debug::blob::Segment<ADDRESS_TYPE> Segment;
  typedef std::vector<const Segment*> SegmentVector;
  typedef typename SegmentVector::const_iterator SegmentVectorIterator;
  typedef unisim::util::debug::blob::Section<ADDRESS_TYPE> Section;
  typedef std::vector<const Section*> SectionVector;
  typedef typename SectionVector::const_iterator SectionVectorIterator;
  const SegmentVector &file_segments = file_blob.GetSegments();
  const SectionVector &file_sections = file_blob.GetSections();

  // copy only segments that have data to be loaded
  for (SegmentVectorIterator it = file_segments.begin();
       it != file_segments.end(); it++) {
    typename unisim::util::debug::blob::Segment<ADDRESS_TYPE>::Type type =
        (*it)->GetType();
    // ignore the segment if it is not a loadable
    if (type != unisim::util::debug::blob::Segment<ADDRESS_TYPE>::TY_LOADABLE)
      continue;

    blob->AddSegment((*it));
  }

  // copy all the sections
  for (SectionVectorIterator it = file_sections.begin();
       it != file_sections.end(); it++) {
    blob->AddSection((*it));
  }
  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::CreateStack(
    unisim::util::debug::blob::Blob<ADDRESS_TYPE>* blob, uint64_t& stack_size) const {
  typedef std::vector<ADDRESS_TYPE> AddressVector;
  typedef typename AddressVector::iterator AddressVectorIterator;
  typedef typename AddressVector::reverse_iterator AddressVectorReverseIterator;
  typedef unisim::util::debug::blob::Section<ADDRESS_TYPE> Section;
  typedef unisim::util::debug::blob::Segment<ADDRESS_TYPE> Segment;

  // make sure a blob is being handled
  if (blob == NULL) {
    logger_ << DebugError
        << "no blob handled to method." << EndDebugError;
    return false;
  }
  // make sure argv has been defined, at least for the application to execute
  if ((argc_ == 0) || (argv_.size() == 0)) {
    logger_ << DebugError
        << "argc and/or size(argv) is/are 0." << EndDebugError;
    return false;
  }
  
  ADDRESS_TYPE aux_table_size = 0;
  SetAuxTable(0, aux_table_size);
  aux_table_size = -aux_table_size;
  
  int pass;
  stack_size = 0;
  Section* env_section = 0;
  Section* argv_section = 0;
  Section* aux_table_section = 0;
  Section* envp_section = 0;
  Section* argvp_section = 0;
  Section* argc_section = 0;
  Section* stack_pointer_section = 0;
  uint8_t *stack_data = 0;
  // First pass for computing the size of the initialized stack
  // Second pass for actually initializing the stack
  for(pass = 1; pass <= 2; pass++)
  {
    // Create the stack
    ADDRESS_TYPE sp = (pass == 2) ? stack_size : 0;
    
    stack_data = (pass == 2) ? (uint8_t *) calloc(stack_size, 1) : 0;
    
    // Fill the stack
    ADDRESS_TYPE cur_length;
    // - copy envp
    // variable to keep the stack environment entries addresses
    std::vector<ADDRESS_TYPE> sp_envp;
    ADDRESS_TYPE env_top = sp;
    for (std::vector<std::string>::const_iterator it = target_envp_.begin();
        it != target_envp_.end(); it++) {
      cur_length = (*it).length() + 1;
      sp = sp - cur_length;
      sp_envp.push_back(sp);
      if(pass == 2) memcpy(stack_data + sp, (*it).c_str(), cur_length);
    }
    ADDRESS_TYPE env_bottom = sp;
    if(pass == 2) {
      uint8_t *env_section_data = (uint8_t *)malloc(env_top - env_bottom);
      memcpy(env_section_data, stack_data + env_bottom, env_top - env_bottom);
      env_section =
          new Section(Section::TY_PROGBITS, Section::SA_AW,
                      ".unisim.linux_os.stack.env", 1, 0, stack_base_ + env_bottom,
                      env_top - env_bottom,env_section_data);
    }
    // - copy argv
    // variable to keep the stack argument entries addresses
    std::vector<ADDRESS_TYPE> sp_argv;
    ADDRESS_TYPE argv_top = sp;
    for (std::vector<std::string>::const_iterator it = argv_.begin();
        it != argv_.end(); it++) {
      cur_length = (*it).length() + 1;
      sp = sp - cur_length;
      sp_argv.push_back(sp);
      if(pass == 2) memcpy(stack_data + sp, (*it).c_str(), cur_length);
    }
    ADDRESS_TYPE argv_bottom = sp;
    if(pass == 2) {
      uint8_t *argv_section_data = (uint8_t *)malloc(argv_top - argv_bottom);
      memcpy(argv_section_data, stack_data + argv_bottom, argv_top - argv_bottom);
      argv_section =
          new Section(Section::TY_PROGBITS, Section::SA_AW,
                      ".unisim.linux_os.stack.argv", 1, 0,
                      stack_base_ + argv_bottom, argv_top - argv_bottom,
                      argv_section_data);
    }
    // force 16 byte alignment
    sp = sp & ~(ADDRESS_TYPE)0x0f;
    // compute the required stack size for the aux table entries and the pointers
    // to the different environment and argument values
    ADDRESS_TYPE sp_content_size = 0;
    // aux table including AT_NULL entry
    sp_content_size += aux_table_size;
    // number of environment entries
    sp_content_size += target_envp_.size() * sizeof(ADDRESS_TYPE);
    //   and its termination
    sp_content_size += sizeof(ADDRESS_TYPE);
    // number of argument entries (argc)
    sp_content_size += argv_.size() * sizeof(ADDRESS_TYPE);
    //   and its termination
    sp_content_size += sizeof(ADDRESS_TYPE);
    // entry for argc itself
    sp_content_size += sizeof(ADDRESS_TYPE);
    if ( sp_content_size & 0x0f ) // force aligned start
      sp = sp - (16 - (sp_content_size & 0x0f));

    ADDRESS_TYPE aux_table_top = sp;
    SetAuxTable((pass == 2) ? stack_data : 0, sp);
    ADDRESS_TYPE aux_table_bottom = sp;
    if(pass == 2) {
      uint8_t *aux_table_section_data = (uint8_t *)malloc(aux_table_top - aux_table_bottom);
      memcpy(aux_table_section_data, stack_data + aux_table_bottom, aux_table_top - aux_table_bottom);
      aux_table_section =
          new Section(Section::TY_PROGBITS, Section::SA_AW,
                      ".unisim.linux_os.stack.aux_table", 1, 0,
                      stack_base_ + aux_table_bottom,
                      aux_table_top - aux_table_bottom,
                      aux_table_section_data);
    }
    // Now we have to put the pointers to the different argv and envp
    ADDRESS_TYPE envp_top = sp;
    ADDRESS_TYPE envp_value = 0;
    sp -= sizeof(envp_value);
    if(pass == 2) memcpy(stack_data + sp, &envp_value, sizeof(envp_value));
    for (AddressVectorIterator it = sp_envp.begin();
        it != sp_envp.end();
        ++it) {
      sp -= sizeof(envp_value);
      envp_value = Host2Target(endianness_, (*it) + stack_base_);
      if(pass == 2) memcpy(stack_data + sp, &envp_value, sizeof(envp_value));
    }
    ADDRESS_TYPE envp_bottom = sp;
    if(pass == 2) {
      uint8_t *envp_section_data = (uint8_t *)malloc(envp_top - envp_bottom);
      memcpy(envp_section_data, stack_data + envp_bottom, envp_top - envp_bottom);
      envp_section =
          new Section(Section::TY_PROGBITS, Section::SA_AW,
                      ".unisim.linux_os.stack.environment_pointers",
                      1, 0, stack_base_ + envp_bottom,
                      envp_top - envp_bottom,
                      envp_section_data);
    }
    
    ADDRESS_TYPE argvp_top = sp;
    ADDRESS_TYPE argvp_value = 0;
    sp -= sizeof(argvp_value);
    if(pass == 2) memcpy(stack_data + sp, &argvp_value, sizeof(argvp_value));
    for (AddressVectorReverseIterator it = sp_argv.rbegin();
        it != sp_argv.rend();
        ++it) {
      sp -= sizeof(argvp_value);
      argvp_value = Host2Target(endianness_, (*it) + stack_base_);
      if(pass == 2) memcpy(stack_data + sp, &argvp_value, sizeof(argvp_value));
    }
    ADDRESS_TYPE argvp_bottom = sp;
    if(pass == 2) {
      uint8_t *argvp_section_data = (uint8_t *)malloc(argvp_top - argvp_bottom);
      memcpy(argvp_section_data, stack_data + argvp_bottom, argvp_top - argvp_bottom);
      argvp_section =
          new Section(Section::TY_PROGBITS, Section::SA_AW,
                      ".unisim.linux_os.stack.argument_pointers",
                      1, 0, stack_base_ + argvp_bottom,
                      argvp_top - argvp_bottom,
                      argvp_section_data);
    }
    // and finally we put argc into the stack
    ADDRESS_TYPE argc_top = sp;
    ADDRESS_TYPE argc_value = Host2Target(endianness_, argc_);
    sp -= sizeof(argc_value);
    if(pass == 2) memcpy(stack_data + sp, &argc_value, sizeof(argc_value));
    ADDRESS_TYPE argc_bottom = sp;
    if(pass == 2) {
      uint8_t *argc_section_data = (uint8_t *)malloc(argc_top - argc_bottom);
      memcpy(argc_section_data, stack_data + argc_bottom, argc_top - argc_bottom);
      argc_section =
          new Section(Section::TY_PROGBITS, Section::SA_AW,
                      ".unisim.linux_os.stack.argument_counter",
                      1, 0, stack_base_ + argc_bottom,
                      argc_top - argc_bottom,
                      argc_section_data);
    }
    // create an empty section to keep the stack pointer
    if(pass == 2) {
      stack_pointer_section =
          new Section(Section::TY_NULL, Section::SA_NULL,
                      ".unisim.linux_os.stack.stack_pointer",
                      0, 0, stack_base_ + sp,
                      0, NULL);
    }
    
    if(pass == 1) stack_size = -sp;
  }
  
  // create a segment for the stack
  Segment* stack_segment =
      new Segment(Segment::TY_LOADABLE, Segment::SA_RW,
                  4, stack_base_, stack_size, stack_size, stack_data);

  // add the stack segment and the different sections to the blob
  blob->AddSegment(stack_segment);
  blob->AddSection(env_section);
  blob->AddSection(argv_section);
  blob->AddSection(aux_table_section);
  blob->AddSection(envp_section);
  blob->AddSection(argvp_section);
  blob->AddSection(argc_section);
  blob->AddSection(stack_pointer_section);

  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetAuxTable(
    uint8_t* stack_data, ADDRESS_TYPE &sp) const {

  ADDRESS_TYPE aux_table_symbol;
  ADDRESS_TYPE aux_table_value;
  unisim::util::debug::blob::Blob<ADDRESS_TYPE> const * const main_blob =
      GetMainBlob();

  aux_table_symbol = AT_NULL;
  aux_table_value = 0;
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_PHDR;
  /* TODO
   * WARNING
   * load_addr does not consider the segments offsets in the elf file
   * (see previous warning).
   * The elf library should provide information on the size of the elf header.
   */
  
  aux_table_value = load_addr_ + main_blob->GetELF_PHOFF(); // start address of ELF program headers
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_PHENT;
  /* TODO
   * WARNING
   * The elf library should provide information on the size of the program
   * header.
   */
  aux_table_value = main_blob->GetELF_PHENT(); // size of the program header
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_PHNUM;
  aux_table_value = num_segments_;
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_PAGESZ;
  aux_table_value = memory_page_size_;
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  /* TODO
   * Add support for interpreter.
   */
  aux_table_symbol = AT_BASE;
  aux_table_value = 0; // 0 to signal no interpreter
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_FLAGS;
  aux_table_value = 0;
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_ENTRY;
  aux_table_value = entry_point_;
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_UID;
#if defined(WIN32) || defined(WIN64)
  aux_table_value = (ADDRESS_TYPE) 1000;
#else
  aux_table_value = (ADDRESS_TYPE)getuid();
#endif
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_EUID;
#if defined(WIN32) || defined(WIN64)
  aux_table_value = (ADDRESS_TYPE) 1000;
#else
  aux_table_value = (ADDRESS_TYPE)geteuid();
#endif
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_GID;
#if defined(WIN32) || defined(WIN64)
  aux_table_value = (ADDRESS_TYPE) 1000;
#else
  aux_table_value = (ADDRESS_TYPE)getgid();
#endif
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  aux_table_symbol = AT_EGID;
#if defined(WIN32) || defined(WIN64)
  aux_table_value = (ADDRESS_TYPE) 1000;
#else
  aux_table_value = (ADDRESS_TYPE)getegid();
#endif
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);

  if ((system_type_.compare("arm") == 0) ||
        (system_type_.compare("arm-eabi") == 0)) {
    uint32_t arm_hwcap = 0;
    std::string hwcap_token;
    std::stringstream sstr(hwcap_);
    while(sstr >> hwcap_token)
    {
      if(hwcap_token.compare("swp") == 0)
      {
        arm_hwcap |= ARM_HWCAP_ARM_SWP;
      }
      else if(hwcap_token.compare("half") == 0)
      {
        arm_hwcap |= ARM_HWCAP_ARM_HALF;
      }
      else if(hwcap_token.compare("thumb") == 0)
      {
        arm_hwcap |= ARM_HWCAP_ARM_THUMB;
      }
      else if(hwcap_token.compare("26bit") == 0)
      {
        arm_hwcap |= ARM_HWCAP_ARM_26BIT;
      }
      else if(hwcap_token.compare("fastmult") == 0)
      {
        arm_hwcap |= ARM_HWCAP_ARM_FAST_MULT;
      }
      else if(hwcap_token.compare("fpa") == 0)
      {
        arm_hwcap |= ARM_HWCAP_ARM_FPA;
      }
      else if(hwcap_token.compare("vfp") == 0)
      {
        arm_hwcap |= ARM_HWCAP_ARM_VFP;
      }
      else if(hwcap_token.compare("edsp") == 0)
      {
        arm_hwcap |= ARM_HWCAP_ARM_EDSP;
      }
      else if(hwcap_token.compare("edsp") == 0)
      {
        arm_hwcap |= ARM_HWCAP_ARM_JAVA;
      }
      else if(hwcap_token.compare("java") == 0)
      {
        arm_hwcap |= ARM_HWCAP_ARM_JAVA;
      }
      else if(hwcap_token.compare("iwmmxt") == 0)
      {
        arm_hwcap |= ARM_HWCAP_ARM_IWMMXT;
      }
      else if(hwcap_token.compare("crunch") == 0)
      {
        arm_hwcap |= ARM_HWCAP_ARM_CRUNCH;
      }
      else if(hwcap_token.compare("thumbee") == 0)
      {
        arm_hwcap |= ARM_HWCAP_ARM_THUMBEE;
      }
      else if(hwcap_token.compare("neon") == 0)
      {
        arm_hwcap |= ARM_HWCAP_ARM_NEON;
      }
      else if(hwcap_token.compare("vfpv3") == 0)
      {
        arm_hwcap |= ARM_HWCAP_ARM_VFPv3;
      }
      else if(hwcap_token.compare("vfpv3d16") == 0)
      {
        arm_hwcap |= ARM_HWCAP_ARM_VFPv3D16;
      }
      else if(hwcap_token.compare("tls") == 0)
      {
        arm_hwcap |= ARM_HWCAP_ARM_TLS;
      }
      else if(hwcap_token.compare("vfpv4") == 0)
      {
        arm_hwcap |= ARM_HWCAP_ARM_VFPv4;
      }
      else if(hwcap_token.compare("idiva") == 0)
      {
        arm_hwcap |= ARM_HWCAP_ARM_IDIVA;
      }
      else if(hwcap_token.compare("idivt") == 0)
      {
        arm_hwcap |= ARM_HWCAP_ARM_IDIVT;
      }
      else
      {
        logger_ << DebugWarning << "unknown hardware capability \"" << hwcap_token << "\"" << EndDebugWarning;
      }
    }
    aux_table_symbol = AT_HWCAP;
    aux_table_value = arm_hwcap;
    sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);
  }

  aux_table_symbol = AT_CLKTCK;
#if defined(WIN32) || defined(WIN64)
  aux_table_value = (ADDRESS_TYPE) 250;
#else
  aux_table_value = (ADDRESS_TYPE)sysconf(_SC_CLK_TCK);
#endif
  sp = SetAuxTableEntry(stack_data, sp, aux_table_symbol, aux_table_value);
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
ADDRESS_TYPE Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetAuxTableEntry(
    uint8_t * stack_data, ADDRESS_TYPE sp,
    ADDRESS_TYPE entry, ADDRESS_TYPE value) const {
  ADDRESS_TYPE target_entry = Host2Target(endianness_, entry);
  ADDRESS_TYPE target_value = Host2Target(endianness_, value);
  sp = sp - sizeof(target_value);
  uint8_t *addr;
  if(stack_data) {
    addr = stack_data + sp;
    memcpy(addr, &target_value, sizeof(target_value));
  }
  sp = sp - sizeof(target_entry);
  if(stack_data) {
    addr = stack_data + sp;
    memcpy(addr, &target_entry, sizeof(target_entry));
  }
  return sp;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetSystemBlob(
    unisim::util::debug::blob::Blob<ADDRESS_TYPE> *blob) const {
  if ((system_type_.compare("arm") == 0) ||
      (system_type_.compare("arm-eabi") == 0))
    return SetArmBlob(blob);
  else if (system_type_.compare("ppc") == 0)
    return SetPPCBlob(blob);

  logger_ << DebugError
      << "Unknown system type (" << system_type_ << ")" << EndDebugError;
  return false;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetArmBlob(
    unisim::util::debug::blob::Blob<ADDRESS_TYPE> *blob) const {
  typedef unisim::util::debug::blob::Section<ADDRESS_TYPE> Section;
  typedef unisim::util::debug::blob::Segment<ADDRESS_TYPE> Segment;
  if (utsname_machine_.compare("armv5") == 0) {
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
    ADDRESS_TYPE tls_base_addr = 0xffff0fe0UL;
    static const uint32_t tls_buf_length = 7;
    static const uint32_t tls_buf[tls_buf_length] =
    {0xe59f0008UL, 0xe1a0f00eUL, 0, 0, 0, 0, 0};
    uint32_t *segment_tls_buf = (uint32_t *) malloc(sizeof(uint32_t) * tls_buf_length);
    uint32_t *section_tls_buf = (uint32_t *) malloc(sizeof(uint32_t) * tls_buf_length);

    for (unsigned int i = 0; i < tls_buf_length; ++i) {
      segment_tls_buf[i] = unisim::util::endian::Host2Target(endianness_, tls_buf[i]);
      section_tls_buf[i] = unisim::util::endian::Host2Target(endianness_, tls_buf[i]);
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
    ADDRESS_TYPE cmpxchg_base_addr = 0xffff0fc0UL;
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
        segment_cmpxchg_buf[i] = unisim::util::endian::Host2Target(endianness_, cmpxchg_buf[i]);
        section_cmpxchg_buf[i] = unisim::util::endian::Host2Target(endianness_, cmpxchg_buf[i]);
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

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetPPCBlob(
    unisim::util::debug::blob::Blob<ADDRESS_TYPE> *blob) const {
  // Nothing to do but that may change in the future if necessary
  return true;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetupLinuxOSARM() {
	// see either arch/arm/include/asm/unistd.h or arch/arm/include/uapi/asm/unistd.h in Linux source
	SetSyscallId("restart_syscall", 0);
	SetSyscallId("exit", 1);
	SetSyscallId("fork", 2);
	SetSyscallId("read", 3);
	SetSyscallId("write", 4);
	SetSyscallId("open", 5);
	SetSyscallId("close", 6);
	// 7 was "waitpid"
	SetSyscallId("creat", 8);
	SetSyscallId("link", 9);
	SetSyscallId("unlink", 10);
	SetSyscallId("execve", 11);
	SetSyscallId("chdir", 12);
	SetSyscallId("time", 13);
	SetSyscallId("mknod", 14);
	SetSyscallId("chmod", 15);
	SetSyscallId("lchown", 16);
	// 17 was "break"
	// 18 was "stat"
	SetSyscallId("lseek", 19);
	SetSyscallId("getpid", 20);
	SetSyscallId("mount", 21);
	SetSyscallId("umount", 22);
	SetSyscallId("setuid", 23);
	SetSyscallId("getuid", 24);
	SetSyscallId("stime", 25);
	SetSyscallId("ptrace", 26);
	SetSyscallId("alarm", 27);
	// 28 was "fstat"
	SetSyscallId("pause", 29);
	SetSyscallId("utime", 30);
	// 31 was "stty"
	// 32 was "gtty"
	SetSyscallId("access", 33);
	SetSyscallId("nice", 34);
	// 35 was "ftime"
	SetSyscallId("sync", 36);
	SetSyscallId("kill", 37);
	SetSyscallId("rename", 38);
	SetSyscallId("mkdir", 39);
	SetSyscallId("rmdir", 40);
	SetSyscallId("dup", 41);
	SetSyscallId("pipe", 42);
	SetSyscallId("times", 43);
	// 44 was "prof"
	SetSyscallId("brk", 45);
	SetSyscallId("setgid", 46);
	SetSyscallId("getgid", 47);
	// 48 was "signal"
	SetSyscallId("geteuid", 49);
	SetSyscallId("getegid", 50);
	SetSyscallId("acct", 51);
	SetSyscallId("umount2", 52);
	// 53 was "lock"
	SetSyscallId("ioctl", 54);
	SetSyscallId("fcntl", 55);
	// 56 was "mpx"
	SetSyscallId("setpgid", 57);
	// 58 was "ulimit"
	// 59 was "olduname"
	SetSyscallId("umask", 60);
	SetSyscallId("chroot", 61);
	SetSyscallId("ustat", 62);
	SetSyscallId("dup2", 63);
	SetSyscallId("getppid", 64);
	SetSyscallId("getpgrp", 65);
	SetSyscallId("setsid", 66);
	SetSyscallId("sigaction", 67);
	// 68 was "sgetmask"
	// 69 was "ssetmask"
	SetSyscallId("setreuid", 70);
	SetSyscallId("setregid", 71);
	SetSyscallId("sigsuspend", 72);
	SetSyscallId("sigpending", 73);
	SetSyscallId("sethostname", 74);
	SetSyscallId("setrlimit", 75);
	SetSyscallId("getrlimit", 76);
	SetSyscallId("getrusage", 77);
	SetSyscallId("gettimeofday", 78);
	SetSyscallId("settimeofday", 79);
	SetSyscallId("getgroups", 80);
	SetSyscallId("setgroups", 81);
	SetSyscallId("select", 82);
	SetSyscallId("symlink", 83);
	// 84 was "lstat"
	SetSyscallId("readlink", 85);
	SetSyscallId("uselib", 86);
	SetSyscallId("swapon", 87);
	SetSyscallId("reboot", 88);
	SetSyscallId("readdir", 89);
	SetSyscallId("mmap", 90);
	SetSyscallId("munmap", 91);
	SetSyscallId("truncate", 92);
	SetSyscallId("ftruncate", 93);
	SetSyscallId("fchmod", 94);
	SetSyscallId("fchown", 95);
	SetSyscallId("getpriority", 96);
	SetSyscallId("setpriority", 97);
	// 98 was "profile"
	SetSyscallId("statfs", 99);
	SetSyscallId("fstatfs", 100);
	SetSyscallId("socketcall", 102);
	SetSyscallId("syslog", 103);
	SetSyscallId("setitimer", 104);
	SetSyscallId("getitimer", 105);
	SetSyscallId("stat", 106);
	SetSyscallId("lstat", 107);
	SetSyscallId("fstat", 108);
	// 109 was "uname"
	// 110 was "iopl"
	SetSyscallId("vhangup", 111);
	// 112 was "idle"
	SetSyscallId("syscall", 113);
	SetSyscallId("wait4", 114);
	SetSyscallId("swapoff", 115);
	SetSyscallId("sysinfo", 116);
	SetSyscallId("ipc", 117);
	SetSyscallId("fsync", 118);
	SetSyscallId("sigreturn", 119);
	SetSyscallId("clone", 120);
	SetSyscallId("setdomainname", 121);
	SetSyscallId("uname", 122);
	// 123 was "modify_ldt"
	SetSyscallId("adjtimex", 124);
	SetSyscallId("mprotect", 125);
	SetSyscallId("sigprocmask", 126);
	// 127 was "create_module"
	SetSyscallId("init_module", 128);
	SetSyscallId("delete_module", 129);
	// 130 was "get_kernel_syms"
	SetSyscallId("quotactl", 131);
	SetSyscallId("getpgid", 132);
	SetSyscallId("fchdir", 133);
	SetSyscallId("bdflush", 134);
	SetSyscallId("sysfs", 135);
	SetSyscallId("personality", 136);
	// 137 was "afs_syscall"
	SetSyscallId("setfsuid", 138);
	SetSyscallId("setfsgid", 139);
	SetSyscallId("_llseek", 140);
	SetSyscallId("getdents", 141);
	SetSyscallId("_newselect", 142);
	SetSyscallId("flock", 143);
	SetSyscallId("msync", 144);
	SetSyscallId("readv", 145);
	SetSyscallId("writev", 146);
	SetSyscallId("getsid", 147);
	SetSyscallId("fdatasync", 148);
	SetSyscallId("_sysctl", 149);
	SetSyscallId("mlock", 150);
	SetSyscallId("munlock", 151);
	SetSyscallId("mlockall", 152);
	SetSyscallId("munlockall", 153);
	SetSyscallId("sched_setparam", 154);
	SetSyscallId("sched_getparam", 155);
	SetSyscallId("sched_setscheduler", 156);
	SetSyscallId("sched_getscheduler", 157);
	SetSyscallId("sched_yield", 158);
	SetSyscallId("sched_get_priority_max", 159);
	SetSyscallId("sched_get_priority_min", 160);
	SetSyscallId("sched_rr_get_interval", 161);
	SetSyscallId("nanosleep", 162);
	SetSyscallId("mremap", 163);
	SetSyscallId("setresuid", 164);
	SetSyscallId("getresuid", 165);
	// 166 was "vm86"
	// 167 was "query_module"
	SetSyscallId("poll", 168);
	SetSyscallId("nfsservctl", 169);
	SetSyscallId("setresgid", 170);
	SetSyscallId("getresgid", 171);
	SetSyscallId("prctl", 172);
	SetSyscallId("rt_sigreturn", 173);
	SetSyscallId("rt_sigaction", 174);
	SetSyscallId("rt_sigprocmask", 175);
	SetSyscallId("rt_sigpending", 176);
	SetSyscallId("rt_sigtimedwait", 177);
	SetSyscallId("rt_sigqueueinfo", 178);
	SetSyscallId("rt_sigsuspend", 179);
	SetSyscallId("pread64", 180);
	SetSyscallId("pwrite64", 181);
	SetSyscallId("chown", 182);
	SetSyscallId("getcwd", 183);
	SetSyscallId("capget", 184);
	SetSyscallId("capset", 185);
	SetSyscallId("sigaltstack", 186);
	SetSyscallId("sendfile", 187);
	// 188 is reserved
	// 189 is reserved
	SetSyscallId("vfork", 190);
	SetSyscallId("ugetrlimit", 191);
	SetSyscallId("mmap2", 192);
	SetSyscallId("truncate64", 193);
	SetSyscallId("ftruncate64", 194);
	SetSyscallId("stat64", 195);
	SetSyscallId("lstat64", 196);
	SetSyscallId("fstat64", 197);
	SetSyscallId("lchown32", 198);
	SetSyscallId("getuid32", 199);
	SetSyscallId("getgid32", 200);
	SetSyscallId("geteuid32", 201);
	SetSyscallId("getegid32", 202);
	SetSyscallId("setreuid32", 203);
	SetSyscallId("setregid32", 204);
	SetSyscallId("getgroups32", 205);
	SetSyscallId("setgroups32", 206);
	SetSyscallId("fchown32", 207);
	SetSyscallId("setresuid32", 208);
	SetSyscallId("getresuid32", 209);
	SetSyscallId("setresgid32", 210);
	SetSyscallId("getresgid32", 211);
	SetSyscallId("chown32", 212);
	SetSyscallId("setuid32", 213);
	SetSyscallId("setgid32", 214);
	SetSyscallId("setfsuid32", 215);
	SetSyscallId("setfsgid32", 216);
	SetSyscallId("getdents64", 217);
	SetSyscallId("pivot_root", 218);
	SetSyscallId("mincore", 219);
	SetSyscallId("madvise", 220);
	SetSyscallId("fcntl64", 221);
	// 222 is for tux
	// 223 is unused
	SetSyscallId("gettid", 224);
	SetSyscallId("readahead", 225);
	SetSyscallId("setxattr", 226);
	SetSyscallId("lsetxattr", 227);
	SetSyscallId("fsetxattr", 228);
	SetSyscallId("getxattr", 229);
	SetSyscallId("lgetxattr", 230);
	SetSyscallId("fgetxattr", 231);
	SetSyscallId("listxattr", 232);
	SetSyscallId("llistxattr", 233);
	SetSyscallId("flistxattr", 234);
	SetSyscallId("removexattr", 235);
	SetSyscallId("lremovexattr", 236);
	SetSyscallId("fremovexattr", 237);
	SetSyscallId("tkill", 238);
	SetSyscallId("sendfile64", 239);
	SetSyscallId("futex", 240);
	SetSyscallId("sched_setaffinity", 241);
	SetSyscallId("sched_getaffinity", 242);
	SetSyscallId("io_setup", 243);
	SetSyscallId("io_destroy", 244);
	SetSyscallId("io_getevents", 245);
	SetSyscallId("io_submit", 246);
	SetSyscallId("io_cancel", 247);
	SetSyscallId("exit_group", 248);
	SetSyscallId("lookup_dcookie", 249);
	SetSyscallId("epoll_create", 250);
	SetSyscallId("epoll_ctl", 251);
	SetSyscallId("epoll_wait", 252);
	SetSyscallId("remap_file_pages", 253);
	// 254 is for set_thread_area
	// 255 is for get_thread_area
	SetSyscallId("set_tid_address", 256);
	SetSyscallId("timer_create", 257);
	SetSyscallId("timer_settime", 258);
	SetSyscallId("timer_gettime", 259);
	SetSyscallId("timer_getoverrun", 260);
	SetSyscallId("timer_delete", 261);
	SetSyscallId("clock_settime", 262);
	SetSyscallId("clock_gettime", 263);
	SetSyscallId("clock_getres", 264);
	SetSyscallId("clock_nanosleep", 265);
	SetSyscallId("statfs64", 266);
	SetSyscallId("fstatfs64", 267);
	SetSyscallId("tgkill", 268);
	SetSyscallId("utimes", 269);
	SetSyscallId("arm_fadvise64_64", 270);
	SetSyscallId("pciconfig_iobase", 271);
	SetSyscallId("pciconfig_read", 272);
	SetSyscallId("pciconfig_write", 273);
	SetSyscallId("mq_open", 274);
	SetSyscallId("mq_unlink", 275);
	SetSyscallId("mq_timedsend", 276);
	SetSyscallId("mq_timedreceive", 277);
	SetSyscallId("mq_notify", 278);
	SetSyscallId("mq_getsetattr", 279);
	SetSyscallId("waitid", 280);
	SetSyscallId("socket", 281);
	SetSyscallId("bind", 282);
	SetSyscallId("connect", 283);
	SetSyscallId("listen", 284);
	SetSyscallId("accept", 285);
	SetSyscallId("getsockname", 286);
	SetSyscallId("getpeername", 287);
	SetSyscallId("socketpair", 288);
	SetSyscallId("send", 289);
	SetSyscallId("sendto", 290);
	SetSyscallId("recv", 291);
	SetSyscallId("recvfrom", 292);
	SetSyscallId("shutdown", 293);
	SetSyscallId("setsockopt", 294);
	SetSyscallId("getsockopt", 295);
	SetSyscallId("sendmsg", 296);
	SetSyscallId("recvmsg", 297);
	SetSyscallId("semop", 298);
	SetSyscallId("semget", 299);
	SetSyscallId("semctl", 300);
	SetSyscallId("msgsnd", 301);
	SetSyscallId("msgrcv", 302);
	SetSyscallId("msgget", 303);
	SetSyscallId("msgctl", 304);
	SetSyscallId("shmat", 305);
	SetSyscallId("shmdt", 306);
	SetSyscallId("shmget", 307);
	SetSyscallId("shmctl", 308);
	SetSyscallId("add_key", 309);
	SetSyscallId("request_key", 310);
	SetSyscallId("keyctl", 311);
	SetSyscallId("semtimedop", 312);
	SetSyscallId("vserver", 313);
	SetSyscallId("ioprio_set", 314);
	SetSyscallId("ioprio_get", 315);
	SetSyscallId("inotify_init", 316);
	SetSyscallId("inotify_add_watch", 317);
	SetSyscallId("inotify_rm_watch", 318);
	SetSyscallId("mbind", 319);
	SetSyscallId("get_mempolicy", 320);
	SetSyscallId("set_mempolicy", 321);
	SetSyscallId("openat", 322);
	SetSyscallId("mkdirat", 323);
	SetSyscallId("mknodat", 324);
	SetSyscallId("fchownat", 325);
	SetSyscallId("futimesat", 326);
	SetSyscallId("fstatat64", 327);
	SetSyscallId("unlinkat", 328);
	SetSyscallId("renameat", 329);
	SetSyscallId("linkat", 330);
	SetSyscallId("symlinkat", 331);
	SetSyscallId("readlinkat", 332);
	SetSyscallId("fchmodat", 333);
	SetSyscallId("faccessat", 334);
	SetSyscallId("pselect6", 335);
	SetSyscallId("ppoll", 336);
	SetSyscallId("unshare", 337);
	SetSyscallId("set_robust_list", 338);
	SetSyscallId("get_robust_list", 339);
	SetSyscallId("splice", 340);
	SetSyscallId("sync_file_range2", 341);
	SetSyscallId("tee", 342);
	SetSyscallId("vmsplice", 343);
	SetSyscallId("move_pages", 344);
	SetSyscallId("getcpu", 345);
	SetSyscallId("epoll_pwait", 346);
	SetSyscallId("kexec_load", 347);
	SetSyscallId("utimensat", 348);
	SetSyscallId("signalfd", 349);
	SetSyscallId("timerfd_create", 350);
	SetSyscallId("eventfd", 351);
	SetSyscallId("fallocate", 352);
	SetSyscallId("timerfd_settime", 353);
	SetSyscallId("timerfd_gettime", 354);
	SetSyscallId("signalfd4", 355);
	SetSyscallId("eventfd2", 356);
	SetSyscallId("epoll_create1", 357);
	SetSyscallId("dup3", 358);
	SetSyscallId("pipe2", 359);
	SetSyscallId("inotify_init1", 360);
	SetSyscallId("preadv", 361);
	SetSyscallId("pwritev", 362);
	SetSyscallId("rt_tgsigqueueinfo", 363);
	SetSyscallId("perf_event_open", 364);
	SetSyscallId("recvmmsg", 365);
	SetSyscallId("accept4", 366);
	SetSyscallId("fanotify_init", 367);
	SetSyscallId("fanotify_mark", 368);
	SetSyscallId("prlimit64", 369);
	SetSyscallId("name_to_handle_at", 370);
	SetSyscallId("open_by_handle_at", 371);
	SetSyscallId("clock_adjtime", 372);
	SetSyscallId("syncfs", 373);
	SetSyscallId("sendmmsg", 374);
	SetSyscallId("setns", 375);
	SetSyscallId("process_vm_readv", 376);
	SetSyscallId("process_vm_writev", 377);
	// 378 is for kcmp
	
	// the following are private to the arm
	SetSyscallId("breakpoint", 983041);
	SetSyscallId("cacheflush", 983042);
	SetSyscallId("usr26", 983043);
	SetSyscallId("usr32", 983044);
	SetSyscallId("set_tls", 983045);

	return true;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetupLinuxOSPPC()
{
	// see either arch/powerpc/include/asm/unistd.h or arch/powerpc/include/uapi/asm/unistd.h in Linux source
	SetSyscallId("restart_syscall", 0);
	SetSyscallId("exit", 1);
	SetSyscallId("fork", 2);
	SetSyscallId("read", 3);
	SetSyscallId("write", 4);
	SetSyscallId("open", 5);
	SetSyscallId("close", 6);
	SetSyscallId("waitpid", 7);
	SetSyscallId("creat", 8);
	SetSyscallId("link", 9);
	SetSyscallId("unlink", 10);
	SetSyscallId("execve", 11);
	SetSyscallId("chdir", 12);
	SetSyscallId("time", 13);
	SetSyscallId("mknod", 14);
	SetSyscallId("chmod", 15);
	SetSyscallId("lchown", 16);
	SetSyscallId("break", 17);
	SetSyscallId("oldstat", 18);
	SetSyscallId("lseek", 19);
	SetSyscallId("getpid", 20);
	SetSyscallId("mount", 21);
	SetSyscallId("umount", 22);
	SetSyscallId("setuid", 23);
	SetSyscallId("getuid", 24);
	SetSyscallId("stime", 25);
	SetSyscallId("ptrace", 26);
	SetSyscallId("alarm", 27);
	SetSyscallId("oldfstat", 28);
	SetSyscallId("pause", 29);
	SetSyscallId("utime", 30);
	SetSyscallId("stty", 31);
	SetSyscallId("gtty", 32);
	SetSyscallId("access", 33);
	SetSyscallId("nice", 34);
	SetSyscallId("ftime", 35);
	SetSyscallId("sync", 36);
	SetSyscallId("kill", 37);
	SetSyscallId("rename", 38);
	SetSyscallId("mkdir", 39);
	SetSyscallId("rmdir", 40);
	SetSyscallId("dup", 41);
	SetSyscallId("pipe", 42);
	SetSyscallId("times", 43);
	SetSyscallId("prof", 44);
	SetSyscallId("brk", 45);
	SetSyscallId("setgid", 46);
	SetSyscallId("getgid", 47);
	SetSyscallId("signal", 48);
	SetSyscallId("geteuid", 49);
	SetSyscallId("getegid", 50);
	SetSyscallId("acct", 51);
	SetSyscallId("umount2", 52);
	SetSyscallId("lock", 53);
	SetSyscallId("ioctl", 54);
	SetSyscallId("fcntl", 55);
	SetSyscallId("mpx", 56);
	SetSyscallId("setpgid", 57);
	SetSyscallId("ulimit", 58);
	SetSyscallId("oldolduname", 59);
	SetSyscallId("umask", 60);
	SetSyscallId("chroot", 61);
	SetSyscallId("ustat", 62);
	SetSyscallId("dup2", 63);
	SetSyscallId("getppid", 64);
	SetSyscallId("getpgrp", 65);
	SetSyscallId("setsid", 66);
	SetSyscallId("sigaction", 67);
	SetSyscallId("sgetmask", 68);
	SetSyscallId("ssetmask", 69);
	SetSyscallId("setreuid", 70);
	SetSyscallId("setregid", 71);
	SetSyscallId("sigsuspend", 72);
	SetSyscallId("sigpending", 73);
	SetSyscallId("sethostname", 74);
	SetSyscallId("setrlimit", 75);
	SetSyscallId("getrlimit", 76);
	SetSyscallId("getrusage", 77);
	SetSyscallId("gettimeofday", 78);
	SetSyscallId("settimeofday", 79);
	SetSyscallId("getgroups", 80);
	SetSyscallId("setgroups", 81);
	SetSyscallId("select", 82);
	SetSyscallId("symlink", 83);
	SetSyscallId("oldlstat", 84);
	SetSyscallId("readlink", 85);
	SetSyscallId("uselib", 86);
	SetSyscallId("swapon", 87);
	SetSyscallId("reboot", 88);
	SetSyscallId("readdir", 89);
	SetSyscallId("mmap", 90);
	SetSyscallId("munmap", 91);
	SetSyscallId("truncate", 92);
	SetSyscallId("ftruncate", 93);
	SetSyscallId("fchmod", 94);
	SetSyscallId("fchown", 95);
	SetSyscallId("getpriority", 96);
	SetSyscallId("setpriority", 97);
	SetSyscallId("profil", 98);
	SetSyscallId("statfs", 99);
	SetSyscallId("fstatfs", 100);
	SetSyscallId("ioperm", 101);
	SetSyscallId("socketcall", 102);
	SetSyscallId("syslog", 103);
	SetSyscallId("setitimer", 104);
	SetSyscallId("getitimer", 105);
	SetSyscallId("stat", 106);
	SetSyscallId("lstat", 107);
	SetSyscallId("fstat", 108);
	SetSyscallId("olduname", 109);
	SetSyscallId("iopl", 110);
	SetSyscallId("vhangup", 111);
	SetSyscallId("idle", 112);
	SetSyscallId("vm86", 113);
	SetSyscallId("wait4", 114);
	SetSyscallId("swapoff", 115);
	SetSyscallId("sysinfo", 116);
	SetSyscallId("ipc", 117);
	SetSyscallId("fsync", 118);
	SetSyscallId("sigreturn", 119);
	SetSyscallId("clone", 120);
	SetSyscallId("setdomainname", 121);
	SetSyscallId("uname", 122);
	SetSyscallId("modify_ldt", 123);
	SetSyscallId("adjtimex", 124);
	SetSyscallId("mprotect", 125);
	SetSyscallId("sigprocmask", 126);
	SetSyscallId("create_module", 127);
	SetSyscallId("init_module", 128);
	SetSyscallId("delete_module", 129);
	SetSyscallId("get_kernel_syms", 130);
	SetSyscallId("quotactl", 131);
	SetSyscallId("getpgid", 132);
	SetSyscallId("fchdir", 133);
	SetSyscallId("bdflush", 134);
	SetSyscallId("sysfs", 135);
	SetSyscallId("personality", 136);
	SetSyscallId("afs_syscall", 137);
	SetSyscallId("setfsuid", 138);
	SetSyscallId("setfsgid", 139);
	SetSyscallId("_llseek", 140);
	SetSyscallId("getdents", 141);
	SetSyscallId("_newselect", 142);
	SetSyscallId("flock", 143);
	SetSyscallId("msync", 144);
	SetSyscallId("readv", 145);
	SetSyscallId("writev", 146);
	SetSyscallId("getsid", 147);
	SetSyscallId("fdatasync", 148);
	SetSyscallId("_sysctl", 149);
	SetSyscallId("mlock", 150);
	SetSyscallId("munlock", 151);
	SetSyscallId("mlockall", 152);
	SetSyscallId("munlockall", 153);
	SetSyscallId("sched_setparam", 154);
	SetSyscallId("sched_getparam", 155);
	SetSyscallId("sched_get_priority_max", 159);
	SetSyscallId("sched_set_priority_max", 160);
	SetSyscallId("sched_rr_get_interval", 161);
	SetSyscallId("nanosleep", 162);
	SetSyscallId("mremap", 163);
	SetSyscallId("setresuid", 164);
	SetSyscallId("getresuid", 165);
	SetSyscallId("query_module", 166);
	SetSyscallId("poll", 167);
	SetSyscallId("nfsservctl", 168);
	SetSyscallId("setresgid", 169);
	SetSyscallId("getresgid", 170);
	SetSyscallId("prctl", 171);
	SetSyscallId("rt_sigreturn", 172);
	SetSyscallId("rt_sigaction", 173);
	SetSyscallId("rt_sigprocmask", 174);
	SetSyscallId("rt_sigpending", 175);
	SetSyscallId("rt_sigtimedwait", 176);
	SetSyscallId("rt_sigqueueinfo", 177);
	SetSyscallId("ry_sigsuspend", 178);
	SetSyscallId("pread64", 179);
	SetSyscallId("pwrite64", 180);
	SetSyscallId("chown", 181);
	SetSyscallId("getcwd", 182);
	SetSyscallId("capget", 183);
	SetSyscallId("capset", 184);
	SetSyscallId("sigaltstack", 185);
	SetSyscallId("sendfile", 186);
	SetSyscallId("getpmsg", 187);
	SetSyscallId("putpmsg", 188);
	SetSyscallId("vfork", 189);
	SetSyscallId("ugetrlimit", 190);
	SetSyscallId("readahead", 191);
	SetSyscallId("mmap2", 192);
	SetSyscallId("truncate64", 193);
	SetSyscallId("ftruncate64", 194);
	SetSyscallId("stat64", 195);
	SetSyscallId("lstat64", 196);
	SetSyscallId("fstat64", 197);
	SetSyscallId("pciconfig_read", 198);
	SetSyscallId("pciconfig_write", 199);
	SetSyscallId("pciconfig_iobase", 200);
	SetSyscallId("multiplexer", 201);
	SetSyscallId("getdents64", 202);
	SetSyscallId("pivot_root", 203);
	SetSyscallId("fcntl64", 204);
	SetSyscallId("madvise", 205);
	SetSyscallId("mincore", 206);
	SetSyscallId("getpid", 207); // in reality gettid: assuming that in a mono-thread environment pid=tid
	SetSyscallId("tkill", 208);
	SetSyscallId("setxattr", 209);
	SetSyscallId("lsetxattr", 210);
	SetSyscallId("fsetxattr", 211);
	SetSyscallId("getxattr", 212);
	SetSyscallId("lgetxattr", 213);
	SetSyscallId("fgetxattr", 214);
	SetSyscallId("listxattr", 215);
	SetSyscallId("llistxattr", 216);
	SetSyscallId("flistxattr", 217);
	SetSyscallId("removexattr", 218);
	SetSyscallId("lremovexattr", 219);
	SetSyscallId("fremovexattr", 220);
	SetSyscallId("futex", 221);
	SetSyscallId("sched_setaffinity", 222);
	SetSyscallId("sched_getaffinity", 223);
	// 224 currently unused
	SetSyscallId("tuxcall", 225);
	SetSyscallId("sendfile64", 226);
	SetSyscallId("io_setup", 227);
	SetSyscallId("io_destroy", 228);
	SetSyscallId("io_getevents", 229);
	SetSyscallId("io_submit", 230);
	SetSyscallId("io_cancel", 231);
	SetSyscallId("set_tid_address", 232);
	SetSyscallId("fadvise64", 233);
	SetSyscallId("exit_group", 234);
	SetSyscallId("lookup_dcookie", 235);
	SetSyscallId("epoll_create", 236);
	SetSyscallId("epoll_ctl", 237);
	SetSyscallId("epoll_wait", 238);
	SetSyscallId("remap_file_pages", 239);
	SetSyscallId("timer_create", 240);
	SetSyscallId("timer_settime", 241);
	SetSyscallId("timer_gettime", 242);
	SetSyscallId("timer_getoverrun", 243);
	SetSyscallId("timer_delete", 244);
	SetSyscallId("clock_settime", 245);
	SetSyscallId("clock_gettime", 246);
	SetSyscallId("clock_getres", 247);
	SetSyscallId("clock_nanosleep", 248);
	SetSyscallId("swapcontext", 249);
	SetSyscallId("tgkill", 250);
	SetSyscallId("utimes", 251);
	SetSyscallId("statfs64", 252);
	SetSyscallId("fstatfs64", 253);
	SetSyscallId("fadvise64_64", 254);
	SetSyscallId("rtas", 255);
	SetSyscallId("sys_debug_setcontext", 256);
	// 257 is reserved for vserver
	SetSyscallId("migrate_pages", 258);
	SetSyscallId("mbind", 259);
	SetSyscallId("get_mempolicy", 260);
	SetSyscallId("set_mempolicy", 261);
	SetSyscallId("mq_open", 262);
	SetSyscallId("mq_unlink", 263);
	SetSyscallId("mq_timedsend", 264);
	SetSyscallId("mq_timedreceive", 265);
	SetSyscallId("mq_notify", 266);
	SetSyscallId("mq_getsetattr", 267);
	SetSyscallId("kexec_load", 268);
	SetSyscallId("add_key", 269);
	SetSyscallId("request_key", 270);
	SetSyscallId("keyctl", 271);
	SetSyscallId("waitid", 272);
	SetSyscallId("ioprio_set", 273);
	SetSyscallId("ioprio_get", 274);
	SetSyscallId("inotify_init", 275);
	SetSyscallId("inotify_add_watch", 276);
	SetSyscallId("inotify_rm_watch", 277);
	SetSyscallId("spu_run", 278);
	SetSyscallId("spu_create", 279);
	SetSyscallId("pselect6", 280);
	SetSyscallId("ppoll", 281);
	SetSyscallId("unshare", 282);
	SetSyscallId("splice", 283);
	SetSyscallId("tee", 284);
	SetSyscallId("vmsplice", 285);
	SetSyscallId("openat", 286);
	SetSyscallId("mkdirat", 287);
	SetSyscallId("mknodat", 288);
	SetSyscallId("fchownat", 289);
	SetSyscallId("futimesat", 290);
	SetSyscallId("fstatat64", 291);
	SetSyscallId("unlinkat", 292);
	SetSyscallId("renameat", 293);
	SetSyscallId("linkat", 294);
	SetSyscallId("symlinkat", 295);
	SetSyscallId("readlinkat", 296);
	SetSyscallId("fchmodat", 297);
	SetSyscallId("faccessat", 298);
	SetSyscallId("get_robust_list", 299);
	SetSyscallId("set_robust_list", 300);
	SetSyscallId("move_pages", 301);
	SetSyscallId("getcpu", 302);
	SetSyscallId("epoll_wait", 303);
	SetSyscallId("utimensat", 304);
	SetSyscallId("signalfd", 305);
	SetSyscallId("timerfd_create", 306);
	SetSyscallId("eventfd", 307);
	SetSyscallId("sync_file_range2", 308);
	SetSyscallId("fallocate", 309);
	SetSyscallId("subpage_prot", 310);
	SetSyscallId("timerfd_settime", 311);
	SetSyscallId("timerfd_gettime", 312);
	SetSyscallId("signalfd4", 313);
	SetSyscallId("eventfd2", 314);
	SetSyscallId("epoll_create1", 315);
	SetSyscallId("dup3", 316);
	SetSyscallId("pipe2", 317);
	SetSyscallId("inotify_init1", 318);
	SetSyscallId("perf_event_open", 319);
	SetSyscallId("preadv", 320);
	SetSyscallId("pwritev", 321);
	SetSyscallId("rt_tsigqueueinfo", 322);
	SetSyscallId("fanotify_init", 323);
	SetSyscallId("fanotify_mark", 324);
	SetSyscallId("prlimit64", 325);
	SetSyscallId("socket", 326);
	SetSyscallId("bind", 327);
	SetSyscallId("connect", 328);
	SetSyscallId("listen", 329);
	SetSyscallId("accept", 330);
	SetSyscallId("getsockname", 331);
	SetSyscallId("getpeername", 332);
	SetSyscallId("socketpair", 333);
	SetSyscallId("send", 334);
	SetSyscallId("sendto", 335);
	SetSyscallId("recv", 336);
	SetSyscallId("recvfrom", 337);
	SetSyscallId("shutdown", 338);
	SetSyscallId("setsockopt", 339);
	SetSyscallId("getsockopt", 340);
	SetSyscallId("sendmsg", 341);
	SetSyscallId("recvmsg", 342);
	SetSyscallId("recvmmsg", 343);
	SetSyscallId("accept4", 344);
	SetSyscallId("name_to_handle_at", 345);
	SetSyscallId("open_by_handle_at", 346);
	SetSyscallId("clock_adjtime", 347);
	SetSyscallId("syncfs", 348);
	SetSyscallId("sendmmsg", 349);
	SetSyscallId("setns", 350);
	SetSyscallId("process_vm_readv", 351);
	SetSyscallId("process_vm_writev", 352);

  return true;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetSyscallId(
    const char *syscall_name, int syscall_id)
{
	if(unlikely(verbose_))
	logger_
		<< DebugInfo
		<< "Associating syscall_name \"" << syscall_name << "\""
		<< "to syscall_id number " << syscall_id
		<< EndDebugInfo;
		
	if(HasSyscall(syscall_id))
	{
		std::stringstream s;
		s << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << std::endl;
		s << "syscall_id already associated to syscall \""
			<< syscall_name_assoc_map_[syscall_id] << "\"" << std::endl;
		s << "  you wanted to associate it to " << syscall_name << std::endl;
		throw std::runtime_error(s.str().c_str());
	}

	syscall_name_assoc_map_[syscall_id] = syscall_name;
	syscall_impl_assoc_map_[syscall_id] = GetSyscallImpl(syscall_name);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetSyscallNameMap()
{
	syscall_name_map_["exit"] = &thistype::LSC_exit;
	syscall_name_map_["read"] = &thistype::LSC_read;
	syscall_name_map_["write"] = &thistype::LSC_write;
	syscall_name_map_["open"] = &thistype::LSC_open;
	syscall_name_map_["close"] = &thistype::LSC_close;
	syscall_name_map_["lseek"] = &thistype::LSC_lseek;
	syscall_name_map_["getpid"] = &thistype::LSC_getpid;
	syscall_name_map_["gettid"] = &thistype::LSC_gettid;
	syscall_name_map_["getuid"] = &thistype::LSC_getuid;
	syscall_name_map_["access"] = &thistype::LSC_access;
	syscall_name_map_["times"] = &thistype::LSC_times;
	syscall_name_map_["brk"] = &thistype::LSC_brk;
	syscall_name_map_["getgid"] = &thistype::LSC_getgid;
	syscall_name_map_["geteuid"] = &thistype::LSC_geteuid;
	syscall_name_map_["getegid"] = &thistype::LSC_getegid;
	syscall_name_map_["munmap"] = &thistype::LSC_munmap;
	syscall_name_map_["stat"] = &thistype::LSC_stat;
	syscall_name_map_["fstat"] = &thistype::LSC_fstat;
	syscall_name_map_["uname"] = &thistype::LSC_uname;
	syscall_name_map_["_llseek"] = &thistype::LSC__llseek;
	syscall_name_map_["writev"] = &thistype::LSC_writev;
	syscall_name_map_["mmap"] = &thistype::LSC_mmap;
	syscall_name_map_["mmap2"] = &thistype::LSC_mmap2;
	syscall_name_map_["stat64"] = &thistype::LSC_stat64;
	syscall_name_map_["fstat64"] = &thistype::LSC_fstat64;
	syscall_name_map_["getuid32"] = &thistype::LSC_getuid32;
	syscall_name_map_["getgid32"] = &thistype::LSC_getgid32;
	syscall_name_map_["geteuid32"] = &thistype::LSC_geteuid32;
	syscall_name_map_["getegid32"] = &thistype::LSC_getegid32;
	syscall_name_map_["fcntl64"] = &thistype::LSC_fcntl64;
	syscall_name_map_["flistxattr"] = &thistype::LSC_flistxattr;
	syscall_name_map_["exit_group"] = &thistype::LSC_exit_group;
	syscall_name_map_["fcntl"] = &thistype::LSC_fcntl;
	syscall_name_map_["dup"] = &thistype::LSC_dup;
	syscall_name_map_["ioctl"] = &thistype::LSC_ioctl;
	syscall_name_map_["ugetrlimit"] = &thistype::LSC_ugetrlimit;
	syscall_name_map_["getrlimit"] = &thistype::LSC_getrlimit;
	syscall_name_map_["setrlimit"] = &thistype::LSC_setrlimit;
	syscall_name_map_["rt_sigaction"] = &thistype::LSC_rt_sigaction;
	syscall_name_map_["getrusage"] = &thistype::LSC_getrusage;
	syscall_name_map_["unlink"] = &thistype::LSC_unlink;
	syscall_name_map_["rename"] = &thistype::LSC_rename;
	syscall_name_map_["time"] = &thistype::LSC_time;
	syscall_name_map_["socketcall"] = &thistype::LSC_socketcall;
	syscall_name_map_["rt_sigprocmask"] = &thistype::LSC_rt_sigprocmask;
	syscall_name_map_["kill"] = &thistype::LSC_kill;
	syscall_name_map_["tkill"] = &thistype::LSC_tkill;
	syscall_name_map_["tgkill"] = &thistype::LSC_tgkill;
	syscall_name_map_["ftruncate"] = &thistype::LSC_ftruncate;
	syscall_name_map_["umask"] = &thistype::LSC_umask;
	syscall_name_map_["gettimeofday"] = &thistype::LSC_gettimeofday;
	syscall_name_map_["statfs"] = &thistype::LSC_statfs;
	
	// the following are arm private system calls
	if (utsname_machine_.compare("armv5") == 0)
	{
		syscall_name_map_["breakpoint"] = &thistype::LSC_arm_breakpoint;
		syscall_name_map_["cacheflush"] = &thistype::LSC_arm_cacheflush;
		syscall_name_map_["usr26"] = &thistype::LSC_arm_usr26;
		syscall_name_map_["usr32"] = &thistype::LSC_arm_usr32;
		syscall_name_map_["set_tls"] = &thistype::LSC_arm_set_tls;
	}
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::HasSyscall(const char *syscall_name)
{
	return syscall_name_map_.find(syscall_name) != syscall_name_map_.end();
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::HasSyscall(int syscall_id)
{
	return syscall_impl_assoc_map_.find(syscall_id) != 
	  syscall_impl_assoc_map_.end();
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
typename Linux<ADDRESS_TYPE, PARAMETER_TYPE>::syscall_t Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetSyscallImpl(const char *syscall_name)
{
	typename std::map<std::string, syscall_t>::const_iterator it = syscall_name_map_.find(syscall_name);
	return (it != syscall_name_map_.end()) ? (*it).second : &thistype::LSC_unimplemented;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
typename Linux<ADDRESS_TYPE, PARAMETER_TYPE>::syscall_t Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetSyscallImpl(int syscall_id)
{
	typename std::map<int, syscall_t>::const_iterator it = syscall_impl_assoc_map_.find(syscall_id);
	return (it != syscall_impl_assoc_map_.end()) ? (*it).second : &thistype::LSC_unknown;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
const char *Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetSyscallName(int syscall_id)
{
	std::map<int, std::string>::const_iterator it = syscall_name_assoc_map_.find(syscall_id);
	return (it != syscall_name_assoc_map_.end()) ? (*it).second.c_str() : "unknown";
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetSyscallNumber(int id)
{
	if ( system_type_.compare("arm") == 0 )
		return ARMGetSyscallNumber(id);
	else if ( system_type_.compare("arm-eabi") == 0 )
		return ARMEABIGetSyscallNumber(id);
	else if ( system_type_.compare("ppc") == 0 )
		return PPCGetSyscallNumber(id);

	logger_ << DebugError
		<< "Could not translate syscall number "
		<< id << " (" << std::hex << id << std::dec << ") for system \""
		<< system_type_ << "\". Returning untranslated id."
		<< EndDebugError;

	return id;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int Linux<ADDRESS_TYPE, PARAMETER_TYPE>::ARMGetSyscallNumber(int id)
{
	int translated_id = id - 0x0900000;
	return translated_id;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int Linux<ADDRESS_TYPE, PARAMETER_TYPE>::ARMEABIGetSyscallNumber(int id)
{
	// the arm eabi ignores the given id and uses register 7
	//   as the id and translated id
	PARAMETER_TYPE translated_id = 0;
	if (regs_if_ == NULL)
	{
		logger_ << DebugError
			<< "no register interface available." << EndDebugError;
		return 0;
	}

	if (GetRegister(kARMEABISyscallNumberReg,
	                &translated_id))
	{
		return (int)translated_id;
	}
	
	return 0;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int Linux<ADDRESS_TYPE, PARAMETER_TYPE>::PPCGetSyscallNumber(int id)
{
	return id;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int32_t Linux<ADDRESS_TYPE, PARAMETER_TYPE>::Host2LinuxErrno(int host_errno) const
{
	typename std::map<int, int32_t>::const_iterator it = host2linux_errno.find(host_errno);
	
	return (it != host2linux_errno.end()) ? (*it).second : LINUX_EINVAL;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int Linux<ADDRESS_TYPE, PARAMETER_TYPE>::Target2HostFileDescriptor(int32_t fd)
{
	// Return an error if file descriptor does not exist
	typename std::map<int32_t, int>::iterator iter = target_to_host_fildes.find(fd);
	if(iter == target_to_host_fildes.end()) return -1;

	// Translate the target file descriptor to an host file descriptor
	return (*iter).second;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int32_t Linux<ADDRESS_TYPE, PARAMETER_TYPE>::AllocateFileDescriptor()
{
	if(!target_fildes_free_list.empty())
	{
		int32_t fildes = target_fildes_free_list.front();
		target_fildes_free_list.pop();
		return fildes;
	}
	
	std::map<int32_t, int>::reverse_iterator iter = target_to_host_fildes.rbegin();
	
	if(iter != target_to_host_fildes.rend())
	{
		return (*iter).first + 1;
	}
	
	return 0;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::FreeFileDescriptor(int32_t fd)
{
	target_fildes_free_list.push(fd);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::MapTargetToHostFileDescriptor(int32_t target_fd, int host_fd)
{
	target_to_host_fildes.insert(std::pair<int32_t, int>(target_fd, host_fd));
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::UnmapTargetToHostFileDescriptor(int32_t target_fd)
{
	std::map<int32_t, int>::iterator iter = target_to_host_fildes.find(target_fd);
	
	if(iter != target_to_host_fildes.end())
	{
		target_to_host_fildes.erase(iter);
	}
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::ReadMem(
    ADDRESS_TYPE addr, uint8_t * buffer, uint32_t size)
{
	if (mem_inject_if_ == NULL) return false;
	
	if (mem_inject_if_->InjectReadMemory(addr, buffer, size))
	{
		if (unlikely(verbose_))
		{
			logger_
				<< DebugInfo
				<< "OS read memory:" << std::endl
				<< "\taddr = 0x" << std::hex << addr << std::dec << std::endl
				<< "\tsize = " << size << std::endl
				<< "\tdata =" << std::hex;
				
			for (unsigned int i = 0; i < size; i++)
			{
				logger_ << " " << (unsigned int)buffer[i];
			}
			
			logger_ << std::dec
					<< EndDebugInfo;
		}
		return true;
	}
	else
	{
		logger_
			<< DebugWarning
			<< "failed OS read memory:" << std::endl
			<< "\taddr = 0x" << std::hex << addr << std::dec << std::endl
			<< "\tsize = " << size
			<< EndDebugWarning;
		return false;
	}
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::WriteMem(
    ADDRESS_TYPE addr, uint8_t const * const buffer, uint32_t size)
{
	if (mem_inject_if_ == NULL) return false;
	
	if (unlikely(verbose_))
	{
		logger_ << DebugInfo
		        << "OS write memory:" << std::endl
		        << "\taddr = 0x" << std::hex << addr << std::dec << std::endl
		        << "\tsize = " << size << std::endl
		        << "\tdata =" << std::hex;
			
		for (unsigned int i = 0; i < size; i++)
		{
			logger_ << " " << (unsigned int)buffer[i];
		}
		
		logger_ << std::dec
		        << EndDebugInfo;
	}
	return mem_inject_if_->InjectWriteMemory(addr, buffer, size);
}

} // end of namespace linux
} // end of namespace os
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_OS_LINUX_LINUX_TCC__
