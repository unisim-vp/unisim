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

#if !defined(linux) && !defined(__linux) && !defined(__linux__) && !defined(__APPLE_CC__) && !defined(WIN32) && !defined(WIN64)
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
#include "unisim/util/os/linux_os/i386.hh"
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
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::Linux(unisim::kernel::logger::Logger& logger,
      unisim::service::interfaces::Registers *regs_if, unisim::service::interfaces::Memory<ADDRESS_TYPE> *mem_if,
      unisim::service::interfaces::MemoryInjection<ADDRESS_TYPE> *mem_inject_if)
	: is_load_(false)
	, target_system(0)
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
        
	struct UndefinedTS : public TargetSystem
        {
          UndefinedTS( Linux& _lin ) : TargetSystem("", _lin) {}
          virtual char const* GetRegisterName( unsigned id ) const { perr(); return 0; };
          virtual bool SetupTarget() const { perr(); return false; }
          virtual bool GetAT_HWCAP( ADDRESS_TYPE& hwcap ) const { perr(); return false; };
          virtual bool SetSystemBlob( unisim::util::debug::blob::Blob<ADDRESS_TYPE>* blob ) const { perr(); return false; };
          virtual SysCall*  GetSystemCall( int& id ) const { perr(); return 0; }
          virtual PARAMETER_TYPE GetSystemCallParam(int id) const { perr(); return 0; };
          virtual void SetSystemCallStatus(int ret, bool error) const { perr(); };
          void perr() const { TargetSystem::lin.logger_ << DebugError << "Undefined Target system." << EndDebugError; }
        };
        target_system = new UndefinedTS( *this );
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::~Linux()
{
	delete target_system;
	
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
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetCommandLine(std::vector<std::string> const &cmd)
{
  argc_ = cmd.size();
  argv_ = cmd;

  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
std::vector<std::string> Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetCommandLine()
{
  return argv_;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetEnvironment(std::vector<std::string> const &env)
{
  envc_ = env.size();
  envp_ = env;

  return true;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
std::vector<std::string> Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetEnvironment()
{
  return envp_;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetApplyHostEnvironment(bool use_host_environment)
{
  apply_host_environnement_ = use_host_environment;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetApplyHostEnvironment()
{
  return apply_host_environnement_;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::AddLoadFile(char const * const filename)
{
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
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetSystemType(std::string system_type_name)
{
  if (target_system->name.size()) {
    logger_ << DebugError << "System Type already assigned (" << target_system->name << ").\n" << EndDebugError;
    return false;
  }
  
  if ((system_type_name.compare( "arm" ) == 0) or (system_type_name.compare( "arm-eabi" ) == 0))
    {
      struct ARMTS : public TargetSystem
      {
        ARMTS( std::string _name, Linux& _lin ) : TargetSystem( _name, _lin ) {}
        using TargetSystem::lin;
        bool GetAT_HWCAP( ADDRESS_TYPE& hwcap ) const
        {
          uint32_t arm_hwcap = 0;
          std::string hwcap_token;
          std::stringstream sstr(lin.hwcap_);
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
              else { lin.logger_ << DebugWarning << "unknown hardware capability \"" << hwcap_token << "\"" << EndDebugWarning; }
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
            lin.regs_if_->ScanRegisters( clear_regs );
          }
          // Set PC to the program entry point
          if (not lin.SetRegister(kARM_pc, lin.entry_point_))
            return false;
          // Set SP to the base of the created stack
          unisim::util::debug::blob::Section<ADDRESS_TYPE> const * sp_section =
            lin.blob_->FindSection(".unisim.linux_os.stack.stack_pointer");
          if (sp_section == NULL) {
            lin.logger_ << DebugError << "Could not find the stack pointer section." << EndDebugError;
            return false;
          }
          if (not lin.SetRegister(kARM_sp, sp_section->GetAddr()))
            return false;
          ADDRESS_TYPE par1_addr = sp_section->GetAddr() + 4;
          ADDRESS_TYPE par2_addr = sp_section->GetAddr() + 8;
          PARAMETER_TYPE par1 = 0;
          PARAMETER_TYPE par2 = 0;
          if (not lin.mem_if_->ReadMemory(par1_addr, (uint8_t *)&par1, sizeof(par1)) or
              not lin.mem_if_->ReadMemory(par2_addr, (uint8_t *)&par2, sizeof(par2)) or
              not lin.SetRegister(kARM_r1, Target2Host(lin.endianness_, par1)) or
              not lin.SetRegister(kARM_r2, Target2Host(lin.endianness_, par2)))
            return false;
          
          return true;
        }

        void SetSystemCallStatus(int ret, bool error) const { lin.SetRegister(kARM_r0, (PARAMETER_TYPE) ret); }
        
        SysCall* GetSystemCall( int& id ) const
        {
          if (TargetSystem::name.compare("arm-eabi") == 0)
            {
              // The arm eabi ignores the supplied id and uses register 7
              PARAMETER_TYPE id_from_reg;
              if (not lin.GetRegister(kARM_r7, &id_from_reg))
                return 0;
              id = int(id_from_reg);
            }
          else if (TargetSystem::name.compare("arm") == 0)
            {
              id -= 0x0900000; // Offset translation
            }
          
          // see either arch/arm/include/asm/unistd.h or arch/arm/include/uapi/asm/unistd.h in Linux source
          switch (id) {
          case 0: return lin.GetSyscallByName("restart_syscall");
          case 1: return lin.GetSyscallByName("exit");
          case 2: return lin.GetSyscallByName("fork");
          case 3: return lin.GetSyscallByName("read");
          case 4: return lin.GetSyscallByName("write");
          case 5: return lin.GetSyscallByName("open");
          case 6: return lin.GetSyscallByName("close");
          // 7 was "waitpid"
          case 8: return lin.GetSyscallByName("creat");
          case 9: return lin.GetSyscallByName("link");
          case 10: return lin.GetSyscallByName("unlink");
          case 11: return lin.GetSyscallByName("execve");
          case 12: return lin.GetSyscallByName("chdir");
          case 13: return lin.GetSyscallByName("time");
          case 14: return lin.GetSyscallByName("mknod");
          case 15: return lin.GetSyscallByName("chmod");
          case 16: return lin.GetSyscallByName("lchown");
          // 17 was "break"
          // 18 was "stat"
          case 19: return lin.GetSyscallByName("lseek");
          case 20: return lin.GetSyscallByName("getpid");
          case 21: return lin.GetSyscallByName("mount");
          case 22: return lin.GetSyscallByName("umount");
          case 23: return lin.GetSyscallByName("setuid");
          case 24: return lin.GetSyscallByName("getuid");
          case 25: return lin.GetSyscallByName("stime");
          case 26: return lin.GetSyscallByName("ptrace");
          case 27: return lin.GetSyscallByName("alarm");
          // 28 was "fstat"
          case 29: return lin.GetSyscallByName("pause");
          case 30: return lin.GetSyscallByName("utime");
          // 31 was "stty"
          // 32 was "gtty"
          case 33: return lin.GetSyscallByName("access");
          case 34: return lin.GetSyscallByName("nice");
          // 35 was "ftime"
          case 36: return lin.GetSyscallByName("sync");
          case 37: return lin.GetSyscallByName("kill");
          case 38: return lin.GetSyscallByName("rename");
          case 39: return lin.GetSyscallByName("mkdir");
          case 40: return lin.GetSyscallByName("rmdir");
          case 41: return lin.GetSyscallByName("dup");
          case 42: return lin.GetSyscallByName("pipe");
          // 43: times (see arm specific)
          // 44 was "prof"
          case 45: return lin.GetSyscallByName("brk");
          case 46: return lin.GetSyscallByName("setgid");
          case 47: return lin.GetSyscallByName("getgid");
          // 48 was "signal"
          case 49: return lin.GetSyscallByName("geteuid");
          case 50: return lin.GetSyscallByName("getegid");
          case 51: return lin.GetSyscallByName("acct");
          case 52: return lin.GetSyscallByName("umount2");
          // 53 was "lock"
          case 54: return lin.GetSyscallByName("ioctl");
          case 55: return lin.GetSyscallByName("fcntl");
          // 56 was "mpx"
          case 57: return lin.GetSyscallByName("setpgid");
          // 58 was "ulimit"
          // 59 was "olduname"
          case 60: return lin.GetSyscallByName("umask");
          case 61: return lin.GetSyscallByName("chroot");
          case 62: return lin.GetSyscallByName("ustat");
          case 63: return lin.GetSyscallByName("dup2");
          case 64: return lin.GetSyscallByName("getppid");
          case 65: return lin.GetSyscallByName("getpgrp");
          case 66: return lin.GetSyscallByName("setsid");
          case 67: return lin.GetSyscallByName("sigaction");
          // 68 was "sgetmask"
          // 69 was "ssetmask"
          case 70: return lin.GetSyscallByName("setreuid");
          case 71: return lin.GetSyscallByName("setregid");
          case 72: return lin.GetSyscallByName("sigsuspend");
          case 73: return lin.GetSyscallByName("sigpending");
          case 74: return lin.GetSyscallByName("sethostname");
          case 75: return lin.GetSyscallByName("setrlimit");
          case 76: return lin.GetSyscallByName("getrlimit");
          case 77: return lin.GetSyscallByName("getrusage");
            // 78: gettimeofday (see arm specific)
          case 79: return lin.GetSyscallByName("settimeofday");
          case 80: return lin.GetSyscallByName("getgroups");
          case 81: return lin.GetSyscallByName("setgroups");
          case 82: return lin.GetSyscallByName("select");
          case 83: return lin.GetSyscallByName("symlink");
          // 84 was "lstat"
          case 85: return lin.GetSyscallByName("readlink");
          case 86: return lin.GetSyscallByName("uselib");
          case 87: return lin.GetSyscallByName("swapon");
          case 88: return lin.GetSyscallByName("reboot");
          case 89: return lin.GetSyscallByName("readdir");
          case 90: return lin.GetSyscallByName("mmap");
          case 91: return lin.GetSyscallByName("munmap");
          case 92: return lin.GetSyscallByName("truncate");
          case 93: return lin.GetSyscallByName("ftruncate");
          case 94: return lin.GetSyscallByName("fchmod");
          case 95: return lin.GetSyscallByName("fchown");
          case 96: return lin.GetSyscallByName("getpriority");
          case 97: return lin.GetSyscallByName("setpriority");
          // 98 was "profile"
          case 99: return lin.GetSyscallByName("statfs");
          case 100: return lin.GetSyscallByName("fstatfs");
          case 102: return lin.GetSyscallByName("socketcall");
          case 103: return lin.GetSyscallByName("syslog");
          case 104: return lin.GetSyscallByName("setitimer");
          case 105: return lin.GetSyscallByName("getitimer");
          case 106: return lin.GetSyscallByName("stat");
          case 107: return lin.GetSyscallByName("lstat");
          // 108: fstat (see arm specific)
          // 109 was "uname"
          // 110 was "iopl"
          case 111: return lin.GetSyscallByName("vhangup");
          // 112 was "idle"
          case 113: return lin.GetSyscallByName("syscall");
          case 114: return lin.GetSyscallByName("wait4");
          case 115: return lin.GetSyscallByName("swapoff");
          case 116: return lin.GetSyscallByName("sysinfo");
          case 117: return lin.GetSyscallByName("ipc");
          case 118: return lin.GetSyscallByName("fsync");
          case 119: return lin.GetSyscallByName("sigreturn");
          case 120: return lin.GetSyscallByName("clone");
          case 121: return lin.GetSyscallByName("setdomainname");
            // 122: uname (see arm specific)
          // 123 was "modify_ldt"
          case 124: return lin.GetSyscallByName("adjtimex");
          case 125: return lin.GetSyscallByName("mprotect");
          case 126: return lin.GetSyscallByName("sigprocmask");
          // 127 was "create_module"
          case 128: return lin.GetSyscallByName("init_module");
          case 129: return lin.GetSyscallByName("delete_module");
          // 130 was "get_kernel_syms"
          case 131: return lin.GetSyscallByName("quotactl");
          case 132: return lin.GetSyscallByName("getpgid");
          case 133: return lin.GetSyscallByName("fchdir");
          case 134: return lin.GetSyscallByName("bdflush");
          case 135: return lin.GetSyscallByName("sysfs");
          case 136: return lin.GetSyscallByName("personality");
          // 137 was "afs_syscall"
          case 138: return lin.GetSyscallByName("setfsuid");
          case 139: return lin.GetSyscallByName("setfsgid");
          case 140: return lin.GetSyscallByName("_llseek");
          case 141: return lin.GetSyscallByName("getdents");
          case 142: return lin.GetSyscallByName("_newselect");
          case 143: return lin.GetSyscallByName("flock");
          case 144: return lin.GetSyscallByName("msync");
          case 145: return lin.GetSyscallByName("readv");
          case 146: return lin.GetSyscallByName("writev");
          case 147: return lin.GetSyscallByName("getsid");
          case 148: return lin.GetSyscallByName("fdatasync");
          case 149: return lin.GetSyscallByName("_sysctl");
          case 150: return lin.GetSyscallByName("mlock");
          case 151: return lin.GetSyscallByName("munlock");
          case 152: return lin.GetSyscallByName("mlockall");
          case 153: return lin.GetSyscallByName("munlockall");
          case 154: return lin.GetSyscallByName("sched_setparam");
          case 155: return lin.GetSyscallByName("sched_getparam");
          case 156: return lin.GetSyscallByName("sched_setscheduler");
          case 157: return lin.GetSyscallByName("sched_getscheduler");
          case 158: return lin.GetSyscallByName("sched_yield");
          case 159: return lin.GetSyscallByName("sched_get_priority_max");
          case 160: return lin.GetSyscallByName("sched_get_priority_min");
          case 161: return lin.GetSyscallByName("sched_rr_get_interval");
          case 162: return lin.GetSyscallByName("nanosleep");
          case 163: return lin.GetSyscallByName("mremap");
          case 164: return lin.GetSyscallByName("setresuid");
          case 165: return lin.GetSyscallByName("getresuid");
          // 166 was "vm86"
          // 167 was "query_module"
          case 168: return lin.GetSyscallByName("poll");
          case 169: return lin.GetSyscallByName("nfsservctl");
          case 170: return lin.GetSyscallByName("setresgid");
          case 171: return lin.GetSyscallByName("getresgid");
          case 172: return lin.GetSyscallByName("prctl");
          case 173: return lin.GetSyscallByName("rt_sigreturn");
          case 174: return lin.GetSyscallByName("rt_sigaction");
          case 175: return lin.GetSyscallByName("rt_sigprocmask");
          case 176: return lin.GetSyscallByName("rt_sigpending");
          case 177: return lin.GetSyscallByName("rt_sigtimedwait");
          case 178: return lin.GetSyscallByName("rt_sigqueueinfo");
          case 179: return lin.GetSyscallByName("rt_sigsuspend");
          case 180: return lin.GetSyscallByName("pread64");
          case 181: return lin.GetSyscallByName("pwrite64");
          case 182: return lin.GetSyscallByName("chown");
          case 183: return lin.GetSyscallByName("getcwd");
          case 184: return lin.GetSyscallByName("capget");
          case 185: return lin.GetSyscallByName("capset");
          case 186: return lin.GetSyscallByName("sigaltstack");
          case 187: return lin.GetSyscallByName("sendfile");
          // 188 is reserved
          // 189 is reserved
          case 190: return lin.GetSyscallByName("vfork");
          case 191: return lin.GetSyscallByName("ugetrlimit");
          case 192: return lin.GetSyscallByName("mmap2");
          case 193: return lin.GetSyscallByName("truncate64");
          case 194: return lin.GetSyscallByName("ftruncate64");
            // 195: stat64 (see arm specific)
          case 196: return lin.GetSyscallByName("lstat64");
            // 197: fstat64 (see arm specific)  
          case 198: return lin.GetSyscallByName("lchown32");
          case 199: return lin.GetSyscallByName("getuid32");
          case 200: return lin.GetSyscallByName("getgid32");
          case 201: return lin.GetSyscallByName("geteuid32");
          case 202: return lin.GetSyscallByName("getegid32");
          case 203: return lin.GetSyscallByName("setreuid32");
          case 204: return lin.GetSyscallByName("setregid32");
          case 205: return lin.GetSyscallByName("getgroups32");
          case 206: return lin.GetSyscallByName("setgroups32");
          case 207: return lin.GetSyscallByName("fchown32");
          case 208: return lin.GetSyscallByName("setresuid32");
          case 209: return lin.GetSyscallByName("getresuid32");
          case 210: return lin.GetSyscallByName("setresgid32");
          case 211: return lin.GetSyscallByName("getresgid32");
          case 212: return lin.GetSyscallByName("chown32");
          case 213: return lin.GetSyscallByName("setuid32");
          case 214: return lin.GetSyscallByName("setgid32");
          case 215: return lin.GetSyscallByName("setfsuid32");
          case 216: return lin.GetSyscallByName("setfsgid32");
          case 217: return lin.GetSyscallByName("getdents64");
          case 218: return lin.GetSyscallByName("pivot_root");
          case 219: return lin.GetSyscallByName("mincore");
          case 220: return lin.GetSyscallByName("madvise");
          case 221: return lin.GetSyscallByName("fcntl64");
          // 222 is for tux
          // 223 is unused
          case 224: return lin.GetSyscallByName("gettid");
          case 225: return lin.GetSyscallByName("readahead");
          case 226: return lin.GetSyscallByName("setxattr");
          case 227: return lin.GetSyscallByName("lsetxattr");
          case 228: return lin.GetSyscallByName("fsetxattr");
          case 229: return lin.GetSyscallByName("getxattr");
          case 230: return lin.GetSyscallByName("lgetxattr");
          case 231: return lin.GetSyscallByName("fgetxattr");
          case 232: return lin.GetSyscallByName("listxattr");
          case 233: return lin.GetSyscallByName("llistxattr");
          case 234: return lin.GetSyscallByName("flistxattr");
          case 235: return lin.GetSyscallByName("removexattr");
          case 236: return lin.GetSyscallByName("lremovexattr");
          case 237: return lin.GetSyscallByName("fremovexattr");
          case 238: return lin.GetSyscallByName("tkill");
          case 239: return lin.GetSyscallByName("sendfile64");
          case 240: return lin.GetSyscallByName("futex");
          case 241: return lin.GetSyscallByName("sched_setaffinity");
          case 242: return lin.GetSyscallByName("sched_getaffinity");
          case 243: return lin.GetSyscallByName("io_setup");
          case 244: return lin.GetSyscallByName("io_destroy");
          case 245: return lin.GetSyscallByName("io_getevents");
          case 246: return lin.GetSyscallByName("io_submit");
          case 247: return lin.GetSyscallByName("io_cancel");
          case 248: return lin.GetSyscallByName("exit_group");
          case 249: return lin.GetSyscallByName("lookup_dcookie");
          case 250: return lin.GetSyscallByName("epoll_create");
          case 251: return lin.GetSyscallByName("epoll_ctl");
          case 252: return lin.GetSyscallByName("epoll_wait");
          case 253: return lin.GetSyscallByName("remap_file_pages");
          // 254 is for set_thread_area
          // 255 is for get_thread_area
          case 256: return lin.GetSyscallByName("set_tid_address");
          case 257: return lin.GetSyscallByName("timer_create");
          case 258: return lin.GetSyscallByName("timer_settime");
          case 259: return lin.GetSyscallByName("timer_gettime");
          case 260: return lin.GetSyscallByName("timer_getoverrun");
          case 261: return lin.GetSyscallByName("timer_delete");
          case 262: return lin.GetSyscallByName("clock_settime");
          case 263: return lin.GetSyscallByName("clock_gettime");
          case 264: return lin.GetSyscallByName("clock_getres");
          case 265: return lin.GetSyscallByName("clock_nanosleep");
          case 266: return lin.GetSyscallByName("statfs64");
          case 267: return lin.GetSyscallByName("fstatfs64");
          case 268: return lin.GetSyscallByName("tgkill");
          case 269: return lin.GetSyscallByName("utimes");
          case 270: return lin.GetSyscallByName("arm_fadvise64_64");
          case 271: return lin.GetSyscallByName("pciconfig_iobase");
          case 272: return lin.GetSyscallByName("pciconfig_read");
          case 273: return lin.GetSyscallByName("pciconfig_write");
          case 274: return lin.GetSyscallByName("mq_open");
          case 275: return lin.GetSyscallByName("mq_unlink");
          case 276: return lin.GetSyscallByName("mq_timedsend");
          case 277: return lin.GetSyscallByName("mq_timedreceive");
          case 278: return lin.GetSyscallByName("mq_notify");
          case 279: return lin.GetSyscallByName("mq_getsetattr");
          case 280: return lin.GetSyscallByName("waitid");
          case 281: return lin.GetSyscallByName("socket");
          case 282: return lin.GetSyscallByName("bind");
          case 283: return lin.GetSyscallByName("connect");
          case 284: return lin.GetSyscallByName("listen");
          case 285: return lin.GetSyscallByName("accept");
          case 286: return lin.GetSyscallByName("getsockname");
          case 287: return lin.GetSyscallByName("getpeername");
          case 288: return lin.GetSyscallByName("socketpair");
          case 289: return lin.GetSyscallByName("send");
          case 290: return lin.GetSyscallByName("sendto");
          case 291: return lin.GetSyscallByName("recv");
          case 292: return lin.GetSyscallByName("recvfrom");
          case 293: return lin.GetSyscallByName("shutdown");
          case 294: return lin.GetSyscallByName("setsockopt");
          case 295: return lin.GetSyscallByName("getsockopt");
          case 296: return lin.GetSyscallByName("sendmsg");
          case 297: return lin.GetSyscallByName("recvmsg");
          case 298: return lin.GetSyscallByName("semop");
          case 299: return lin.GetSyscallByName("semget");
          case 300: return lin.GetSyscallByName("semctl");
          case 301: return lin.GetSyscallByName("msgsnd");
          case 302: return lin.GetSyscallByName("msgrcv");
          case 303: return lin.GetSyscallByName("msgget");
          case 304: return lin.GetSyscallByName("msgctl");
          case 305: return lin.GetSyscallByName("shmat");
          case 306: return lin.GetSyscallByName("shmdt");
          case 307: return lin.GetSyscallByName("shmget");
          case 308: return lin.GetSyscallByName("shmctl");
          case 309: return lin.GetSyscallByName("add_key");
          case 310: return lin.GetSyscallByName("request_key");
          case 311: return lin.GetSyscallByName("keyctl");
          case 312: return lin.GetSyscallByName("semtimedop");
          case 313: return lin.GetSyscallByName("vserver");
          case 314: return lin.GetSyscallByName("ioprio_set");
          case 315: return lin.GetSyscallByName("ioprio_get");
          case 316: return lin.GetSyscallByName("inotify_init");
          case 317: return lin.GetSyscallByName("inotify_add_watch");
          case 318: return lin.GetSyscallByName("inotify_rm_watch");
          case 319: return lin.GetSyscallByName("mbind");
          case 320: return lin.GetSyscallByName("get_mempolicy");
          case 321: return lin.GetSyscallByName("set_mempolicy");
          case 322: return lin.GetSyscallByName("openat");
          case 323: return lin.GetSyscallByName("mkdirat");
          case 324: return lin.GetSyscallByName("mknodat");
          case 325: return lin.GetSyscallByName("fchownat");
          case 326: return lin.GetSyscallByName("futimesat");
          case 327: return lin.GetSyscallByName("fstatat64");
          case 328: return lin.GetSyscallByName("unlinkat");
          case 329: return lin.GetSyscallByName("renameat");
          case 330: return lin.GetSyscallByName("linkat");
          case 331: return lin.GetSyscallByName("symlinkat");
          case 332: return lin.GetSyscallByName("readlinkat");
          case 333: return lin.GetSyscallByName("fchmodat");
          case 334: return lin.GetSyscallByName("faccessat");
          case 335: return lin.GetSyscallByName("pselect6");
          case 336: return lin.GetSyscallByName("ppoll");
          case 337: return lin.GetSyscallByName("unshare");
          case 338: return lin.GetSyscallByName("set_robust_list");
          case 339: return lin.GetSyscallByName("get_robust_list");
          case 340: return lin.GetSyscallByName("splice");
          case 341: return lin.GetSyscallByName("sync_file_range2");
          case 342: return lin.GetSyscallByName("tee");
          case 343: return lin.GetSyscallByName("vmsplice");
          case 344: return lin.GetSyscallByName("move_pages");
          case 345: return lin.GetSyscallByName("getcpu");
          case 346: return lin.GetSyscallByName("epoll_pwait");
          case 347: return lin.GetSyscallByName("kexec_load");
          case 348: return lin.GetSyscallByName("utimensat");
          case 349: return lin.GetSyscallByName("signalfd");
          case 350: return lin.GetSyscallByName("timerfd_create");
          case 351: return lin.GetSyscallByName("eventfd");
          case 352: return lin.GetSyscallByName("fallocate");
          case 353: return lin.GetSyscallByName("timerfd_settime");
          case 354: return lin.GetSyscallByName("timerfd_gettime");
          case 355: return lin.GetSyscallByName("signalfd4");
          case 356: return lin.GetSyscallByName("eventfd2");
          case 357: return lin.GetSyscallByName("epoll_create1");
          case 358: return lin.GetSyscallByName("dup3");
          case 359: return lin.GetSyscallByName("pipe2");
          case 360: return lin.GetSyscallByName("inotify_init1");
          case 361: return lin.GetSyscallByName("preadv");
          case 362: return lin.GetSyscallByName("pwritev");
          case 363: return lin.GetSyscallByName("rt_tgsigqueueinfo");
          case 364: return lin.GetSyscallByName("perf_event_open");
          case 365: return lin.GetSyscallByName("recvmmsg");
          case 366: return lin.GetSyscallByName("accept4");
          case 367: return lin.GetSyscallByName("fanotify_init");
          case 368: return lin.GetSyscallByName("fanotify_mark");
          case 369: return lin.GetSyscallByName("prlimit64");
          case 370: return lin.GetSyscallByName("name_to_handle_at");
          case 371: return lin.GetSyscallByName("open_by_handle_at");
          case 372: return lin.GetSyscallByName("clock_adjtime");
          case 373: return lin.GetSyscallByName("syncfs");
          case 374: return lin.GetSyscallByName("sendmmsg");
          case 375: return lin.GetSyscallByName("setns");
          case 376: return lin.GetSyscallByName("process_vm_readv");
          case 377: return lin.GetSyscallByName("process_vm_writev");
          // 378 is for kcmp
	
          // the following are private to the arm
          case 43: /* ARM specific times syscall */
            {
              static struct : public SysCall {
                char const* GetName() const { return "times"; }
                void Execute( Linux& lin, int syscall_id ) const
                {
                  int ret;
                  ADDRESS_TYPE buf_addr;
                  int32_t target_errno = 0;
                  
                  buf_addr = lin.target_system->GetSystemCallParam(0);
                  
                  struct arm_tms target_tms;
                  ret = lin.Times(&target_tms);

                  if(ret >= 0)
                    {
                      lin.WriteMem(buf_addr, (uint8_t *)&target_tms, sizeof(target_tms));
                    }
                  else
                    {
                      target_errno = lin.Host2LinuxErrno(errno);
                    }
                  
                  if(unlikely(lin.verbose_))
                      lin.logger_ << DebugInfo << "times(buf=0x" << std::hex << buf_addr << std::dec << ")" << EndDebugInfo;
	
                  lin.SetSystemCallStatus((ret == -1) ? -target_errno : ret, (ret == -1));
                }
              } sc;
              return &sc;
            } break;
            
          case 78: /* ARM specific gettimeofday syscall */
            {
              static struct : public SysCall {
                char const* GetName() const { return "gettimeofday"; }
                void Execute( Linux& lin, int syscall_id ) const
                {
                  int ret = -1;
                  ADDRESS_TYPE tv_addr;
                  ADDRESS_TYPE tz_addr;
                  int32_t target_errno = 0;
                  tv_addr = lin.target_system->GetSystemCallParam(0);
                  tz_addr = lin.target_system->GetSystemCallParam(1);

                  struct arm_timeval target_tv;
                  struct arm_timezone target_tz;

                  ret = lin.GetTimeOfDay(tv_addr ? &target_tv : 0, tz_addr ? &target_tz : 0);
                  if(ret == -1) target_errno = lin.Host2LinuxErrno(errno);

                  if(ret == 0)
                    {
                      if(tv_addr)
                        {
                          lin.WriteMem(tv_addr, (const uint8_t *) &target_tv, sizeof(target_tv));
                        }
                      if(tz_addr)
                        {
                          lin.WriteMem(tz_addr, (const uint8_t *) &target_tz, sizeof(target_tz));
                        }
                    }

                  if(unlikely(lin.verbose_))
                    {
                      lin.logger_ << DebugInfo
                                  << "gettimeofday(tv = 0x" << std::hex << tv_addr << std::dec
                                  << ", tz = 0x" << std::hex << tz_addr << std::dec << ")"
                                  << EndDebugInfo;
                    }
	
                  lin.SetSystemCallStatus((PARAMETER_TYPE) (ret == -1) ? -target_errno : ret, (ret == -1));
                }
              } sc;
              return &sc;
            } break;

          case 108: /* ARM specific fstat syscall */
            {
              static struct : public SysCall {
                char const* GetName() const { return "fstat"; }
                void Execute( Linux& lin, int syscall_id ) const
                {
                  int ret;
                  int32_t target_fd;
                  int host_fd;
                  ADDRESS_TYPE buf_address;
                  int32_t target_errno = 0;

                  target_fd = lin.target_system->GetSystemCallParam(0);
                  buf_address = lin.target_system->GetSystemCallParam(1);
	
                  host_fd = lin.Target2HostFileDescriptor(target_fd);

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

                  if(unlikely(lin.verbose_))
                    {
                      lin.logger_ << DebugInfo
                                  << "fstat(fd=" << target_fd
                                  << ", buf_addr=0x" << std::hex << buf_address << std::dec
                                  << ")" << EndDebugInfo;
                    }
	
                  lin.SetSystemCallStatus((ret == -1) ? -target_errno : ret, (ret == -1));
                }
              } sc;
              return &sc;
            } break;

          case 122: /* ARM specific uname syscall */
            {
              static struct : public SysCall {
                char const* GetName() const { return "uname"; }
                void Execute( Linux& lin, int syscall_id ) const
                {
                  int ret;
                  int32_t target_errno = 0;
                  ADDRESS_TYPE buf_addr = lin.target_system->GetSystemCallParam(0);
                  ret = 0;
	
                  struct arm_utsname value;
                  memset(&value, 0, sizeof(value));
                  strncpy(value.sysname,  lin.utsname_sysname_.c_str(), sizeof(value.sysname) - 1);
                  strncpy(value.nodename, lin.utsname_nodename_.c_str(), sizeof(value.nodename) - 1);
                  strncpy(value.release,  lin.utsname_release_.c_str(), sizeof(value.release) - 1);
                  strncpy(value.version,  lin.utsname_version_.c_str(), sizeof(value.version) - 1);
                  strncpy(value.machine,  lin.utsname_machine_.c_str(), sizeof(value.machine));
                  lin.WriteMem(buf_addr, (uint8_t *)&value, sizeof(value));
	
                  lin.SetSystemCallStatus((ret == -1) ? -target_errno : ret, (ret == -1));
                }
              } sc;
              return &sc;
            } break;

          case 195: /* ARM specific stat64 syscall */
            {
              static struct : public SysCall {
                char const* GetName() const { return "stat64"; }
                void Execute( Linux& lin, int syscall_id ) const
                {
                  int ret;
                  ADDRESS_TYPE pathnameaddr;
                  ADDRESS_TYPE buf_address;
                  unsigned int pathnamelen;
                  char *pathname;
                  int32_t target_errno = 0;

                  pathnameaddr = lin.target_system->GetSystemCallParam(0);
                  buf_address = lin.target_system->GetSystemCallParam(1);
                  pathnamelen = lin.StringLength(pathnameaddr);
                  pathname = (char *) malloc(pathnamelen + 1);
	
                  if(pathname)
                    {
                      lin.ReadMem(pathnameaddr, (uint8_t *) pathname, pathnamelen + 1);
		
                      struct arm_stat64 target_stat;
                      ret = lin.Stat64(pathname, &target_stat);
                      lin.WriteMem(buf_address, (uint8_t *)&target_stat, sizeof(target_stat));
                      
                      if(unlikely(lin.verbose_))
                        {
                          lin.logger_ << DebugInfo
                                      << "pathname = \"" << pathname << "\", buf_address = 0x" << std::hex << buf_address << std::dec
                                      << EndDebugInfo;
                        }
		
                      free(pathname);
                    }
                  else
                    {
                      ret = -1;
                      target_errno = LINUX_ENOMEM;
                    }
	
                  lin.SetSystemCallStatus((PARAMETER_TYPE) (ret == -1) ? -target_errno : ret, (ret == -1));
                }
              } sc;
              return &sc;
            } break;

          case 197: /* ARM specific fstat64 syscall */
            {
              static struct : public SysCall {
                char const* GetName() const { return "fstat64"; }
                void Execute( Linux& lin, int syscall_id ) const
                {
                  int ret;
                  ADDRESS_TYPE buf_address;
                  int32_t target_fd;
                  int host_fd;
                  int32_t target_errno = 0;

                  target_fd = lin.target_system->GetSystemCallParam(0);
                  buf_address = lin.target_system->GetSystemCallParam(1);
	
                  host_fd = lin.Target2HostFileDescriptor(target_fd);
	
                  if(host_fd == -1)
                    {
                      target_errno = LINUX_EBADF;
                      ret = -1;
                    }
                  else
                    {
                      struct arm_stat64 target_stat;
                      ret = lin.Fstat64(host_fd, &target_stat);
                      if(ret == -1) target_errno = lin.Host2LinuxErrno(errno);
			
                      lin.WriteMem(buf_address, (uint8_t *)&target_stat, sizeof(target_stat));
                    }
	
                  if(unlikely(lin.verbose_))
                    {
                      lin.logger_ << DebugInfo
                                  << "fd = " << target_fd << ", buf_address = 0x" << std::hex << buf_address << std::dec
                                  << EndDebugInfo;
                    }
	
                  lin.SetSystemCallStatus((PARAMETER_TYPE) (ret == -1) ? -target_errno : ret, (ret == -1));
                }
              } sc;
              return &sc;
            } break;


          case 983041: {
            static struct : public SysCall {
              char const* GetName() const { return "breakpoint"; }
              void Execute( Linux& lin, int syscall_id ) const { lin.SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true); }
            } sc;
            return &sc;
          } break;
            
          case 983042: {
            static struct : public SysCall {
              char const* GetName() const { return "cacheflush"; }
              void Execute( Linux& lin, int syscall_id ) const { lin.SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true); }
            } sc;
            return &sc;
          } break;
            
          case 983043: {
            static struct : public SysCall {
              char const* GetName() const { return "usr26"; }
              void Execute( Linux& lin, int syscall_id ) const { lin.SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true); }
            } sc;
            return &sc;
          } break;
            
          case 983044: {
            static struct : public SysCall {
              char const* GetName() const { return "usr32"; }
              void Execute( Linux& lin, int syscall_id ) const { lin.SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true); }
            } sc;
            return &sc;
          } break;
            
          case 983045: {
            static struct : public SysCall {
              char const* GetName() const { return "set_tls"; }
              void Execute( Linux& lin, int syscall_id ) const
              {
                uint32_t r0 = Host2Target(lin.endianness_, lin.target_system->GetSystemCallParam(0));
                lin.WriteMem(0xffff0ff0UL, (uint8_t *)&(r0), sizeof(r0));
                lin.SetSystemCallStatus((PARAMETER_TYPE)0, false);
              }
            } sc;
            return &sc;
          } break;
            
          }
          
          return 0;
        }
        PARAMETER_TYPE GetSystemCallParam(int id) const
        {
          PARAMETER_TYPE val = 0;
          
          switch (id) {
          case 0: lin.GetRegister(kARM_r0, &val); break;
          case 1: lin.GetRegister(kARM_r1, &val); break;
          case 2: lin.GetRegister(kARM_r2, &val); break;
          case 3: lin.GetRegister(kARM_r3, &val); break;
          case 4: lin.GetRegister(kARM_r4, &val); break;
          case 5: lin.GetRegister(kARM_r5, &val); break;
          case 6: lin.GetRegister(kARM_r6, &val); break;
          default:
            lin.logger_ << DebugError << "No syscall argument #" << id << " in " << this->name << " linux" << EndDebugError;
            break;
          }
          
          return val;
        }

        bool SetSystemBlob( unisim::util::debug::blob::Blob<ADDRESS_TYPE>* blob ) const
        {
          typedef unisim::util::debug::blob::Section<ADDRESS_TYPE> Section;
          typedef unisim::util::debug::blob::Segment<ADDRESS_TYPE> Segment;
          std::string mach = lin.utsname_machine_;
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
              ADDRESS_TYPE tls_base_addr = 0xffff0fe0UL;
              static const uint32_t tls_buf_length = 7;
              static const uint32_t tls_buf[tls_buf_length] =
                {0xe59f0008UL, 0xe1a0f00eUL, 0, 0, 0, 0, 0};
              uint32_t *segment_tls_buf = (uint32_t *) malloc(sizeof(uint32_t) * tls_buf_length);
              uint32_t *section_tls_buf = (uint32_t *) malloc(sizeof(uint32_t) * tls_buf_length);

              for (unsigned int i = 0; i < tls_buf_length; ++i) {
                segment_tls_buf[i] = unisim::util::endian::Host2Target(lin.endianness_, tls_buf[i]);
                section_tls_buf[i] = unisim::util::endian::Host2Target(lin.endianness_, tls_buf[i]);
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
                segment_cmpxchg_buf[i] = unisim::util::endian::Host2Target(lin.endianness_, cmpxchg_buf[i]);
                section_cmpxchg_buf[i] = unisim::util::endian::Host2Target(lin.endianness_, cmpxchg_buf[i]);
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
      delete target_system;
      target_system = new ARMTS( system_type_name, *this );
      return true;
    }
  
  if (system_type_name.compare("ppc") == 0)
    {
      struct PPCTS : public TargetSystem
      {
        PPCTS( Linux& _lin ) : TargetSystem( "ppc", _lin ) {}
        using TargetSystem::lin;
        
        bool GetAT_HWCAP( ADDRESS_TYPE& hwcap ) const { return false; }
        
        bool SetupTarget() const
        {
          // Reset all target registers
          {
            struct : public unisim::util::debug::RegisterScanner {
              void Append( unisim::util::debug::Register* reg ) { reg->Clear(); }
            } clear_regs;
            lin.regs_if_->ScanRegisters( clear_regs );
          }
          // Set PC to the program entry point
          if (not lin.SetRegister(kPPC_cia, lin.entry_point_))
            return false;
          // Set SP to the base of the created stack
          unisim::util::debug::blob::Section<ADDRESS_TYPE> const * sp_section =
            lin.blob_->FindSection(".unisim.linux_os.stack.stack_pointer");
          if (sp_section == NULL)
            {
              lin.logger_ << DebugError << "Could not find the stack pointer section." << EndDebugError;
              return false;
            }
          if (not lin.SetRegister(kPPC_sp, sp_section->GetAddr()))
            return false;
          ADDRESS_TYPE par1_addr = sp_section->GetAddr() + 4;
          ADDRESS_TYPE par2_addr = sp_section->GetAddr() + 8;
          PARAMETER_TYPE par1 = 0;
          PARAMETER_TYPE par2 = 0;
          if (not lin.mem_if_->ReadMemory(par1_addr, (uint8_t *)&par1, sizeof(par1)) or
              not lin.mem_if_->ReadMemory(par2_addr, (uint8_t *)&par2, sizeof(par2)) or
              not lin.SetRegister(kPPC_r3, Target2Host(lin.endianness_, par1)) or
              not lin.SetRegister(kPPC_r4, Target2Host(lin.endianness_, par2)))
            return false;

          return true;
        }

        void SetSystemCallStatus(int ret, bool error) const
        {
          PARAMETER_TYPE val;
	
          if(error)
            {
              if (not lin.GetRegister(kPPC_cr, &val))
                return;
		
              val |= (1 << 28); // CR0[SO] <- 1
		
              if (not lin.SetRegister(kPPC_cr, val))
                return;
            }
          else
            {
              if (not lin.GetRegister(kPPC_cr, &val))
                return;
		
              val &= ~(1 << 28); // CR0[SO] <- 0
		
              if (not lin.SetRegister(kPPC_cr, val))
                return;
            }
	
          val = (PARAMETER_TYPE)ret;
	
          if (not lin.SetRegister(kPPC_r3, val))
            return;
        }


        SysCall* GetSystemCall( int& id ) const
        {
          // see either arch/powerpc/include/asm/unistd.h or arch/powerpc/include/uapi/asm/unistd.h in Linux source
          switch (id) {
          case 0: return lin.GetSyscallByName("restart_syscall");
          case 1: return lin.GetSyscallByName("exit");
          case 2: return lin.GetSyscallByName("fork");
          case 3: return lin.GetSyscallByName("read");
          case 4: return lin.GetSyscallByName("write");
          case 5: return lin.GetSyscallByName("open");
          case 6: return lin.GetSyscallByName("close");
          case 7: return lin.GetSyscallByName("waitpid");
          case 8: return lin.GetSyscallByName("creat");
          case 9: return lin.GetSyscallByName("link");
          case 10: return lin.GetSyscallByName("unlink");
          case 11: return lin.GetSyscallByName("execve");
          case 12: return lin.GetSyscallByName("chdir");
          case 13: return lin.GetSyscallByName("time");
          case 14: return lin.GetSyscallByName("mknod");
          case 15: return lin.GetSyscallByName("chmod");
          case 16: return lin.GetSyscallByName("lchown");
          case 17: return lin.GetSyscallByName("break");
          case 18: return lin.GetSyscallByName("oldstat");
          case 19: return lin.GetSyscallByName("lseek");
          case 20: return lin.GetSyscallByName("getpid");
          case 21: return lin.GetSyscallByName("mount");
          case 22: return lin.GetSyscallByName("umount");
          case 23: return lin.GetSyscallByName("setuid");
          case 24: return lin.GetSyscallByName("getuid");
          case 25: return lin.GetSyscallByName("stime");
          case 26: return lin.GetSyscallByName("ptrace");
          case 27: return lin.GetSyscallByName("alarm");
          case 28: return lin.GetSyscallByName("oldfstat");
          case 29: return lin.GetSyscallByName("pause");
          case 30: return lin.GetSyscallByName("utime");
          case 31: return lin.GetSyscallByName("stty");
          case 32: return lin.GetSyscallByName("gtty");
          case 33: return lin.GetSyscallByName("access");
          case 34: return lin.GetSyscallByName("nice");
          case 35: return lin.GetSyscallByName("ftime");
          case 36: return lin.GetSyscallByName("sync");
          case 37: return lin.GetSyscallByName("kill");
          case 38: return lin.GetSyscallByName("rename");
          case 39: return lin.GetSyscallByName("mkdir");
          case 40: return lin.GetSyscallByName("rmdir");
          case 41: return lin.GetSyscallByName("dup");
          case 42: return lin.GetSyscallByName("pipe");
            // 43: times (see ppc specific)
          case 44: return lin.GetSyscallByName("prof");
          case 45: return lin.GetSyscallByName("brk");
          case 46: return lin.GetSyscallByName("setgid");
          case 47: return lin.GetSyscallByName("getgid");
          case 48: return lin.GetSyscallByName("signal");
          case 49: return lin.GetSyscallByName("geteuid");
          case 50: return lin.GetSyscallByName("getegid");
          case 51: return lin.GetSyscallByName("acct");
          case 52: return lin.GetSyscallByName("umount2");
          case 53: return lin.GetSyscallByName("lock");
          case 54: return lin.GetSyscallByName("ioctl");
          case 55: return lin.GetSyscallByName("fcntl");
          case 56: return lin.GetSyscallByName("mpx");
          case 57: return lin.GetSyscallByName("setpgid");
          case 58: return lin.GetSyscallByName("ulimit");
          case 59: return lin.GetSyscallByName("oldolduname");
          case 60: return lin.GetSyscallByName("umask");
          case 61: return lin.GetSyscallByName("chroot");
          case 62: return lin.GetSyscallByName("ustat");
          case 63: return lin.GetSyscallByName("dup2");
          case 64: return lin.GetSyscallByName("getppid");
          case 65: return lin.GetSyscallByName("getpgrp");
          case 66: return lin.GetSyscallByName("setsid");
          case 67: return lin.GetSyscallByName("sigaction");
          case 68: return lin.GetSyscallByName("sgetmask");
          case 69: return lin.GetSyscallByName("ssetmask");
          case 70: return lin.GetSyscallByName("setreuid");
          case 71: return lin.GetSyscallByName("setregid");
          case 72: return lin.GetSyscallByName("sigsuspend");
          case 73: return lin.GetSyscallByName("sigpending");
          case 74: return lin.GetSyscallByName("sethostname");
          case 75: return lin.GetSyscallByName("setrlimit");
          case 76: return lin.GetSyscallByName("getrlimit");
          case 77: return lin.GetSyscallByName("getrusage");
            // 78: gettimeofday (see ppc specific)
          case 79: return lin.GetSyscallByName("settimeofday");
          case 80: return lin.GetSyscallByName("getgroups");
          case 81: return lin.GetSyscallByName("setgroups");
          case 82: return lin.GetSyscallByName("select");
          case 83: return lin.GetSyscallByName("symlink");
          case 84: return lin.GetSyscallByName("oldlstat");
          case 85: return lin.GetSyscallByName("readlink");
          case 86: return lin.GetSyscallByName("uselib");
          case 87: return lin.GetSyscallByName("swapon");
          case 88: return lin.GetSyscallByName("reboot");
          case 89: return lin.GetSyscallByName("readdir");
          case 90: return lin.GetSyscallByName("mmap");
          case 91: return lin.GetSyscallByName("munmap");
          case 92: return lin.GetSyscallByName("truncate");
          case 93: return lin.GetSyscallByName("ftruncate");
          case 94: return lin.GetSyscallByName("fchmod");
          case 95: return lin.GetSyscallByName("fchown");
          case 96: return lin.GetSyscallByName("getpriority");
          case 97: return lin.GetSyscallByName("setpriority");
          case 98: return lin.GetSyscallByName("profil");
          case 99: return lin.GetSyscallByName("statfs");
          case 100: return lin.GetSyscallByName("fstatfs");
          case 101: return lin.GetSyscallByName("ioperm");
          case 102: return lin.GetSyscallByName("socketcall");
          case 103: return lin.GetSyscallByName("syslog");
          case 104: return lin.GetSyscallByName("setitimer");
          case 105: return lin.GetSyscallByName("getitimer");
          case 106: return lin.GetSyscallByName("stat");
          case 107: return lin.GetSyscallByName("lstat");
            // 108: fstat (see ppc specific)
          case 109: return lin.GetSyscallByName("olduname");
          case 110: return lin.GetSyscallByName("iopl");
          case 111: return lin.GetSyscallByName("vhangup");
          case 112: return lin.GetSyscallByName("idle");
          case 113: return lin.GetSyscallByName("vm86");
          case 114: return lin.GetSyscallByName("wait4");
          case 115: return lin.GetSyscallByName("swapoff");
          case 116: return lin.GetSyscallByName("sysinfo");
          case 117: return lin.GetSyscallByName("ipc");
          case 118: return lin.GetSyscallByName("fsync");
          case 119: return lin.GetSyscallByName("sigreturn");
          case 120: return lin.GetSyscallByName("clone");
          case 121: return lin.GetSyscallByName("setdomainname");
            // 122: uname (see ppc specific)
          case 123: return lin.GetSyscallByName("modify_ldt");
          case 124: return lin.GetSyscallByName("adjtimex");
          case 125: return lin.GetSyscallByName("mprotect");
          case 126: return lin.GetSyscallByName("sigprocmask");
          case 127: return lin.GetSyscallByName("create_module");
          case 128: return lin.GetSyscallByName("init_module");
          case 129: return lin.GetSyscallByName("delete_module");
          case 130: return lin.GetSyscallByName("get_kernel_syms");
          case 131: return lin.GetSyscallByName("quotactl");
          case 132: return lin.GetSyscallByName("getpgid");
          case 133: return lin.GetSyscallByName("fchdir");
          case 134: return lin.GetSyscallByName("bdflush");
          case 135: return lin.GetSyscallByName("sysfs");
          case 136: return lin.GetSyscallByName("personality");
          case 137: return lin.GetSyscallByName("afs_syscall");
          case 138: return lin.GetSyscallByName("setfsuid");
          case 139: return lin.GetSyscallByName("setfsgid");
          case 140: return lin.GetSyscallByName("_llseek");
          case 141: return lin.GetSyscallByName("getdents");
          case 142: return lin.GetSyscallByName("_newselect");
          case 143: return lin.GetSyscallByName("flock");
          case 144: return lin.GetSyscallByName("msync");
          case 145: return lin.GetSyscallByName("readv");
          case 146: return lin.GetSyscallByName("writev");
          case 147: return lin.GetSyscallByName("getsid");
          case 148: return lin.GetSyscallByName("fdatasync");
          case 149: return lin.GetSyscallByName("_sysctl");
          case 150: return lin.GetSyscallByName("mlock");
          case 151: return lin.GetSyscallByName("munlock");
          case 152: return lin.GetSyscallByName("mlockall");
          case 153: return lin.GetSyscallByName("munlockall");
          case 154: return lin.GetSyscallByName("sched_setparam");
          case 155: return lin.GetSyscallByName("sched_getparam");
          case 159: return lin.GetSyscallByName("sched_get_priority_max");
          case 160: return lin.GetSyscallByName("sched_set_priority_max");
          case 161: return lin.GetSyscallByName("sched_rr_get_interval");
          case 162: return lin.GetSyscallByName("nanosleep");
          case 163: return lin.GetSyscallByName("mremap");
          case 164: return lin.GetSyscallByName("setresuid");
          case 165: return lin.GetSyscallByName("getresuid");
          case 166: return lin.GetSyscallByName("query_module");
          case 167: return lin.GetSyscallByName("poll");
          case 168: return lin.GetSyscallByName("nfsservctl");
          case 169: return lin.GetSyscallByName("setresgid");
          case 170: return lin.GetSyscallByName("getresgid");
          case 171: return lin.GetSyscallByName("prctl");
          case 172: return lin.GetSyscallByName("rt_sigreturn");
          case 173: return lin.GetSyscallByName("rt_sigaction");
          case 174: return lin.GetSyscallByName("rt_sigprocmask");
          case 175: return lin.GetSyscallByName("rt_sigpending");
          case 176: return lin.GetSyscallByName("rt_sigtimedwait");
          case 177: return lin.GetSyscallByName("rt_sigqueueinfo");
          case 178: return lin.GetSyscallByName("ry_sigsuspend");
          case 179: return lin.GetSyscallByName("pread64");
          case 180: return lin.GetSyscallByName("pwrite64");
          case 181: return lin.GetSyscallByName("chown");
          case 182: return lin.GetSyscallByName("getcwd");
          case 183: return lin.GetSyscallByName("capget");
          case 184: return lin.GetSyscallByName("capset");
          case 185: return lin.GetSyscallByName("sigaltstack");
          case 186: return lin.GetSyscallByName("sendfile");
          case 187: return lin.GetSyscallByName("getpmsg");
          case 188: return lin.GetSyscallByName("putpmsg");
          case 189: return lin.GetSyscallByName("vfork");
          case 190: return lin.GetSyscallByName("ugetrlimit");
          case 191: return lin.GetSyscallByName("readahead");
          case 192: return lin.GetSyscallByName("mmap2");
          case 193: return lin.GetSyscallByName("truncate64");
          case 194: return lin.GetSyscallByName("ftruncate64");
            // 195: stat64 (see ppc specific)
          case 196: return lin.GetSyscallByName("lstat64");
            // 197: fstat64 (see ppc specific)
          case 198: return lin.GetSyscallByName("pciconfig_read");
          case 199: return lin.GetSyscallByName("pciconfig_write");
          case 200: return lin.GetSyscallByName("pciconfig_iobase");
          case 201: return lin.GetSyscallByName("multiplexer");
          case 202: return lin.GetSyscallByName("getdents64");
          case 203: return lin.GetSyscallByName("pivot_root");
          case 204: return lin.GetSyscallByName("fcntl64");
          case 205: return lin.GetSyscallByName("madvise");
          case 206: return lin.GetSyscallByName("mincore");
          case 207: return lin.GetSyscallByName("getpid"); // in reality gettid: assuming that in a mono-thread environment pid=tid
          case 208: return lin.GetSyscallByName("tkill");
          case 209: return lin.GetSyscallByName("setxattr");
          case 210: return lin.GetSyscallByName("lsetxattr");
          case 211: return lin.GetSyscallByName("fsetxattr");
          case 212: return lin.GetSyscallByName("getxattr");
          case 213: return lin.GetSyscallByName("lgetxattr");
          case 214: return lin.GetSyscallByName("fgetxattr");
          case 215: return lin.GetSyscallByName("listxattr");
          case 216: return lin.GetSyscallByName("llistxattr");
          case 217: return lin.GetSyscallByName("flistxattr");
          case 218: return lin.GetSyscallByName("removexattr");
          case 219: return lin.GetSyscallByName("lremovexattr");
          case 220: return lin.GetSyscallByName("fremovexattr");
          case 221: return lin.GetSyscallByName("futex");
          case 222: return lin.GetSyscallByName("sched_setaffinity");
          case 223: return lin.GetSyscallByName("sched_getaffinity");
          // 224 currently unused
          case 225: return lin.GetSyscallByName("tuxcall");
          case 226: return lin.GetSyscallByName("sendfile64");
          case 227: return lin.GetSyscallByName("io_setup");
          case 228: return lin.GetSyscallByName("io_destroy");
          case 229: return lin.GetSyscallByName("io_getevents");
          case 230: return lin.GetSyscallByName("io_submit");
          case 231: return lin.GetSyscallByName("io_cancel");
          case 232: return lin.GetSyscallByName("set_tid_address");
          case 233: return lin.GetSyscallByName("fadvise64");
          case 234: return lin.GetSyscallByName("exit_group");
          case 235: return lin.GetSyscallByName("lookup_dcookie");
          case 236: return lin.GetSyscallByName("epoll_create");
          case 237: return lin.GetSyscallByName("epoll_ctl");
          case 238: return lin.GetSyscallByName("epoll_wait");
          case 239: return lin.GetSyscallByName("remap_file_pages");
          case 240: return lin.GetSyscallByName("timer_create");
          case 241: return lin.GetSyscallByName("timer_settime");
          case 242: return lin.GetSyscallByName("timer_gettime");
          case 243: return lin.GetSyscallByName("timer_getoverrun");
          case 244: return lin.GetSyscallByName("timer_delete");
          case 245: return lin.GetSyscallByName("clock_settime");
          case 246: return lin.GetSyscallByName("clock_gettime");
          case 247: return lin.GetSyscallByName("clock_getres");
          case 248: return lin.GetSyscallByName("clock_nanosleep");
          case 249: return lin.GetSyscallByName("swapcontext");
          case 250: return lin.GetSyscallByName("tgkill");
          case 251: return lin.GetSyscallByName("utimes");
          case 252: return lin.GetSyscallByName("statfs64");
          case 253: return lin.GetSyscallByName("fstatfs64");
          case 254: return lin.GetSyscallByName("fadvise64_64");
          case 255: return lin.GetSyscallByName("rtas");
          case 256: return lin.GetSyscallByName("sys_debug_setcontext");
          // 257 is reserved for vserver
          case 258: return lin.GetSyscallByName("migrate_pages");
          case 259: return lin.GetSyscallByName("mbind");
          case 260: return lin.GetSyscallByName("get_mempolicy");
          case 261: return lin.GetSyscallByName("set_mempolicy");
          case 262: return lin.GetSyscallByName("mq_open");
          case 263: return lin.GetSyscallByName("mq_unlink");
          case 264: return lin.GetSyscallByName("mq_timedsend");
          case 265: return lin.GetSyscallByName("mq_timedreceive");
          case 266: return lin.GetSyscallByName("mq_notify");
          case 267: return lin.GetSyscallByName("mq_getsetattr");
          case 268: return lin.GetSyscallByName("kexec_load");
          case 269: return lin.GetSyscallByName("add_key");
          case 270: return lin.GetSyscallByName("request_key");
          case 271: return lin.GetSyscallByName("keyctl");
          case 272: return lin.GetSyscallByName("waitid");
          case 273: return lin.GetSyscallByName("ioprio_set");
          case 274: return lin.GetSyscallByName("ioprio_get");
          case 275: return lin.GetSyscallByName("inotify_init");
          case 276: return lin.GetSyscallByName("inotify_add_watch");
          case 277: return lin.GetSyscallByName("inotify_rm_watch");
          case 278: return lin.GetSyscallByName("spu_run");
          case 279: return lin.GetSyscallByName("spu_create");
          case 280: return lin.GetSyscallByName("pselect6");
          case 281: return lin.GetSyscallByName("ppoll");
          case 282: return lin.GetSyscallByName("unshare");
          case 283: return lin.GetSyscallByName("splice");
          case 284: return lin.GetSyscallByName("tee");
          case 285: return lin.GetSyscallByName("vmsplice");
          case 286: return lin.GetSyscallByName("openat");
          case 287: return lin.GetSyscallByName("mkdirat");
          case 288: return lin.GetSyscallByName("mknodat");
          case 289: return lin.GetSyscallByName("fchownat");
          case 290: return lin.GetSyscallByName("futimesat");
          case 291: return lin.GetSyscallByName("fstatat64");
          case 292: return lin.GetSyscallByName("unlinkat");
          case 293: return lin.GetSyscallByName("renameat");
          case 294: return lin.GetSyscallByName("linkat");
          case 295: return lin.GetSyscallByName("symlinkat");
          case 296: return lin.GetSyscallByName("readlinkat");
          case 297: return lin.GetSyscallByName("fchmodat");
          case 298: return lin.GetSyscallByName("faccessat");
          case 299: return lin.GetSyscallByName("get_robust_list");
          case 300: return lin.GetSyscallByName("set_robust_list");
          case 301: return lin.GetSyscallByName("move_pages");
          case 302: return lin.GetSyscallByName("getcpu");
          case 303: return lin.GetSyscallByName("epoll_wait");
          case 304: return lin.GetSyscallByName("utimensat");
          case 305: return lin.GetSyscallByName("signalfd");
          case 306: return lin.GetSyscallByName("timerfd_create");
          case 307: return lin.GetSyscallByName("eventfd");
          case 308: return lin.GetSyscallByName("sync_file_range2");
          case 309: return lin.GetSyscallByName("fallocate");
          case 310: return lin.GetSyscallByName("subpage_prot");
          case 311: return lin.GetSyscallByName("timerfd_settime");
          case 312: return lin.GetSyscallByName("timerfd_gettime");
          case 313: return lin.GetSyscallByName("signalfd4");
          case 314: return lin.GetSyscallByName("eventfd2");
          case 315: return lin.GetSyscallByName("epoll_create1");
          case 316: return lin.GetSyscallByName("dup3");
          case 317: return lin.GetSyscallByName("pipe2");
          case 318: return lin.GetSyscallByName("inotify_init1");
          case 319: return lin.GetSyscallByName("perf_event_open");
          case 320: return lin.GetSyscallByName("preadv");
          case 321: return lin.GetSyscallByName("pwritev");
          case 322: return lin.GetSyscallByName("rt_tsigqueueinfo");
          case 323: return lin.GetSyscallByName("fanotify_init");
          case 324: return lin.GetSyscallByName("fanotify_mark");
          case 325: return lin.GetSyscallByName("prlimit64");
          case 326: return lin.GetSyscallByName("socket");
          case 327: return lin.GetSyscallByName("bind");
          case 328: return lin.GetSyscallByName("connect");
          case 329: return lin.GetSyscallByName("listen");
          case 330: return lin.GetSyscallByName("accept");
          case 331: return lin.GetSyscallByName("getsockname");
          case 332: return lin.GetSyscallByName("getpeername");
          case 333: return lin.GetSyscallByName("socketpair");
          case 334: return lin.GetSyscallByName("send");
          case 335: return lin.GetSyscallByName("sendto");
          case 336: return lin.GetSyscallByName("recv");
          case 337: return lin.GetSyscallByName("recvfrom");
          case 338: return lin.GetSyscallByName("shutdown");
          case 339: return lin.GetSyscallByName("setsockopt");
          case 340: return lin.GetSyscallByName("getsockopt");
          case 341: return lin.GetSyscallByName("sendmsg");
          case 342: return lin.GetSyscallByName("recvmsg");
          case 343: return lin.GetSyscallByName("recvmmsg");
          case 344: return lin.GetSyscallByName("accept4");
          case 345: return lin.GetSyscallByName("name_to_handle_at");
          case 346: return lin.GetSyscallByName("open_by_handle_at");
          case 347: return lin.GetSyscallByName("clock_adjtime");
          case 348: return lin.GetSyscallByName("syncfs");
          case 349: return lin.GetSyscallByName("sendmmsg");
          case 350: return lin.GetSyscallByName("setns");
          case 351: return lin.GetSyscallByName("process_vm_readv");
          case 352: return lin.GetSyscallByName("process_vm_writev");
            
          // Following is specific to PowerPC
          case 43: /* PowerPC specific times syscall */
            {
              static struct : public SysCall {
                char const* GetName() const { return "times"; }
                void Execute( Linux& lin, int syscall_id ) const
                {
                  int ret;
                  ADDRESS_TYPE buf_addr;
                  int32_t target_errno = 0;
	
                  buf_addr = lin.target_system->GetSystemCallParam(0);

                  struct powerpc_tms target_tms;
                  ret = lin.Times(&target_tms);

                  if(ret >= 0)
                    lin.WriteMem(buf_addr, (uint8_t *)&target_tms, sizeof(target_tms));
                  else
                    target_errno = lin.Host2LinuxErrno(errno);

                  if(unlikely(lin.verbose_))
                    lin.logger_ << DebugInfo << "times(buf=0x" << std::hex << buf_addr << std::dec << ")" << EndDebugInfo;
	
                  lin.SetSystemCallStatus((ret == -1) ? -target_errno : ret, (ret == -1));
                }
              } sc;
              return &sc;
            }

          case 78: /* PowerPC specific gettimeofday syscall */
            {
              static struct : public SysCall {
                char const* GetName() const { return "gettimeofday"; }
                void Execute( Linux& lin, int syscall_id ) const
                {
                  int ret = -1;
                  ADDRESS_TYPE tv_addr;
                  ADDRESS_TYPE tz_addr;
                  int32_t target_errno = 0;
                  tv_addr = lin.target_system->GetSystemCallParam(0);
                  tz_addr = lin.target_system->GetSystemCallParam(1);

                  struct powerpc_timeval target_tv;
                  struct powerpc_timezone target_tz;

                  ret = lin.GetTimeOfDay(tv_addr ? &target_tv : 0, tz_addr ? &target_tz : 0);
                  if(ret == -1) target_errno = lin.Host2LinuxErrno(errno);

                  if(ret == 0)
                    {
                      if(tv_addr)
                        {
                          lin.WriteMem(tv_addr, (const uint8_t *) &target_tv, sizeof(target_tv));
                        }
                      if(tz_addr)
                        {
                          lin.WriteMem(tz_addr, (const uint8_t *) &target_tz, sizeof(target_tz));
                        }
                    }

                  if(unlikely(lin.verbose_))
                    {
                      lin.logger_ << DebugInfo
                                  << "gettimeofday(tv = 0x" << std::hex << tv_addr << std::dec
                                  << ", tz = 0x" << std::hex << tz_addr << std::dec << ")"
                                  << EndDebugInfo;
                    }
	
                  lin.SetSystemCallStatus((PARAMETER_TYPE) (ret == -1) ? -target_errno : ret, (ret == -1));
                }
              } sc;
              return &sc;
            } break;

          case 108: /* PowerPC specific fstat syscall */
            {
              static struct : public SysCall {
                char const* GetName() const { return "fstat"; }
                void Execute( Linux& lin, int syscall_id ) const
                {
                  int ret;
                  int32_t target_fd;
                  int host_fd;
                  ADDRESS_TYPE buf_address;
                  int32_t target_errno = 0;

                  target_fd = lin.target_system->GetSystemCallParam(0);
                  buf_address = lin.target_system->GetSystemCallParam(1);
	
                  host_fd = lin.Target2HostFileDescriptor(target_fd);

                  if(host_fd == -1)
                    {
                      target_errno = LINUX_EBADF;
                      ret = -1;
                    }
                  else
                    {
                      struct powerpc_stat target_stat;
                      ret = lin.Stat(host_fd, &target_stat);
                      if(ret == -1) target_errno = lin.Host2LinuxErrno(errno);
                      if(ret >= 0) lin.WriteMem(buf_address, (uint8_t *)&target_stat, sizeof(target_stat));
                    }

                  if(unlikely(lin.verbose_))
                    {
                      lin.logger_ << DebugInfo
                                  << "fstat(fd=" << target_fd
                                  << ", buf_addr=0x" << std::hex << buf_address << std::dec
                                  << ")" << EndDebugInfo;
                    }
	
                  lin.SetSystemCallStatus((ret == -1) ? -target_errno : ret, (ret == -1));
                }
              } sc;
              return &sc;
            } break;

          case 122: /* PowerPC specific uname syscall */
            {
              static struct : public SysCall {
                char const* GetName() const { return "uname"; }
                void Execute( Linux& lin, int syscall_id ) const
                {
                  int ret;
                  int32_t target_errno = 0;
                  ADDRESS_TYPE buf_addr = lin.target_system->GetSystemCallParam(0);
                  ret = 0;
                  
                  struct ppc_utsname value;
                  memset(&value, 0, sizeof(value));
                  strncpy(value.sysname,     lin.utsname_sysname_.c_str(), sizeof(value.sysname) - 1);
                  strncpy(value.nodename,    lin.utsname_nodename_.c_str(), sizeof(value.nodename) - 1);
                  strncpy(value.release,     lin.utsname_release_.c_str(), sizeof(value.release) - 1);
                  strncpy(value.version,     lin.utsname_version_.c_str(), sizeof(value.version) - 1);
                  strncpy(value.machine,     lin.utsname_machine_.c_str(), sizeof(value.machine) - 1);
                  strncpy(value.domainname,  lin.utsname_domainname_.c_str(), sizeof(value.domainname) - 1);
                  lin.WriteMem(buf_addr, (uint8_t *)&value, sizeof(value));
	
                  lin.SetSystemCallStatus((ret == -1) ? -target_errno : ret, (ret == -1));
                }
              } sc;
              return &sc;
            } break;

          case 195: /* PowerPC specific stat64 syscall */
            {
              static struct : public SysCall {
                char const* GetName() const { return "stat64"; }
                void Execute( Linux& lin, int syscall_id ) const
                {
                  int ret;
                  ADDRESS_TYPE pathnameaddr;
                  ADDRESS_TYPE buf_address;
                  unsigned int pathnamelen;
                  char *pathname;
                  int32_t target_errno = 0;

                  pathnameaddr = lin.target_system->GetSystemCallParam(0);
                  buf_address = lin.target_system->GetSystemCallParam(1);
                  pathnamelen = lin.StringLength(pathnameaddr);
                  pathname = (char *) malloc(pathnamelen + 1);
	
                  if(pathname)
                    {
                      lin.ReadMem(pathnameaddr, (uint8_t *) pathname, pathnamelen + 1);
		
                      struct powerpc_stat64 target_stat;
                      ret = lin.Stat64(pathname, &target_stat);
                      lin.WriteMem(buf_address, (uint8_t *)&target_stat, sizeof(target_stat));
		
                      if(unlikely(lin.verbose_))
                        {
                          lin.logger_ << DebugInfo
                                      << "pathname = \"" << pathname << "\", buf_address = 0x" << std::hex << buf_address << std::dec
                                      << EndDebugInfo;
                        }
		
                      free(pathname);
                    }
                  else
                    {
                      ret = -1;
                      target_errno = LINUX_ENOMEM;
                    }
	
                  lin.SetSystemCallStatus((PARAMETER_TYPE) (ret == -1) ? -target_errno : ret, (ret == -1));
                }
              } sc;
              return &sc;
            } break;

          case 197: /* PowerPC specific fstat64 syscall */
            {
              static struct : public SysCall {
                char const* GetName() const { return "fstat64"; }
                void Execute( Linux& lin, int syscall_id ) const
                {
                  int ret;
                  ADDRESS_TYPE buf_address;
                  int32_t target_fd;
                  int host_fd;
                  int32_t target_errno = 0;

                  target_fd = lin.target_system->GetSystemCallParam(0);
                  buf_address = lin.target_system->GetSystemCallParam(1);
	
                  host_fd = lin.Target2HostFileDescriptor(target_fd);
	
                  if(host_fd == -1)
                    {
                      target_errno = LINUX_EBADF;
                      ret = -1;
                    }
                  else
                    {
                      struct powerpc_stat64 target_stat;
                      ret = lin.Fstat64(host_fd, &target_stat);
                      if(ret == -1) target_errno = lin.Host2LinuxErrno(errno);
			
                      lin.WriteMem(buf_address, (uint8_t *)&target_stat, sizeof(target_stat));
                    }
	
                  if(unlikely(lin.verbose_))
                    {
                      lin.logger_ << DebugInfo
                                  << "fd = " << target_fd << ", buf_address = 0x" << std::hex << buf_address << std::dec
                                  << EndDebugInfo;
                    }
	
                  lin.SetSystemCallStatus((PARAMETER_TYPE) (ret == -1) ? -target_errno : ret, (ret == -1));
                }
              } sc;
              return &sc;
            } break;
          }
          
          return 0;
        }
        PARAMETER_TYPE GetSystemCallParam(int id) const
        {
          PARAMETER_TYPE val = 0;
          
          switch (id) {
          case 0: lin.GetRegister(kPPC_r3, &val); break;
          case 1: lin.GetRegister(kPPC_r4, &val); break;
          case 2: lin.GetRegister(kPPC_r5, &val); break;
          case 3: lin.GetRegister(kPPC_r6, &val); break;
          case 4: lin.GetRegister(kPPC_r7, &val); break;
          default:
            lin.logger_ << DebugError << "No syscall argument #" << id << " in " << this->name << " linux" << EndDebugError;
            break;
          }
          
          return val;
        }

        bool SetSystemBlob( unisim::util::debug::blob::Blob<ADDRESS_TYPE>* blob ) const { return true; }
      };
      delete target_system;
      target_system = new PPCTS( *this );
      return true;
    }
  
  if (system_type_name.compare("i386") == 0)
    {
      struct I386TS : public TargetSystem
      {
        I386TS( Linux& _lin ) : TargetSystem( "i386", _lin ) {}
        using TargetSystem::lin;
        
        bool GetAT_HWCAP( ADDRESS_TYPE& hwcap ) const { return false; }
        
        void WriteSegmentSelector( char const* segname, uint16_t value ) const
        {
          if (not lin.regs_if_)
            throw 0;
          if (unisim::util::debug::Register* reg = lin.regs_if_->GetRegister(segname))
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
            lin.regs_if_->ScanRegisters( clear_regs );
          }
          // Set EIP to the program entry point
          if (not lin.SetRegister(kI386_eip, lin.entry_point_))
            return false;
          // Set ESP to the base of the created stack
          unisim::util::debug::blob::Section<ADDRESS_TYPE> const * esp_section =
            lin.blob_->FindSection(".unisim.linux_os.stack.stack_pointer");
          if (esp_section == NULL)
            {
              lin.logger_ << DebugError << "Could not find the stack pointer section." << EndDebugError;
              return false;
            }
          if (not lin.SetRegister(kI386_esp, esp_section->GetAddr()))
            return false;
          
          // Pseudo GDT initialization  (flat memory + early TLS)
          lin.SetRegister("@gdt[1].base", 0 ); // For code segment
          lin.SetRegister("@gdt[2].base", 0 ); // For data segments
          unisim::util::debug::blob::Section<ADDRESS_TYPE> const* etls_section =
            lin.blob_->FindSection(".unisim.linux_os.etls.middle_pointer");
          if (not etls_section)
            {
              lin.logger_ << DebugError << "Could not find the early TLS section." << EndDebugError;
              return false;
            }
          lin.SetRegister("@gdt[3].base", etls_section->GetAddr() ); // for early TLS kludge
          
          this->WriteSegmentSelector(kI386_cs,(1<<3) | (0<<2) | 3); // code
          this->WriteSegmentSelector(kI386_ss,(2<<3) | (0<<2) | 3); // data
          this->WriteSegmentSelector(kI386_ds,(2<<3) | (0<<2) | 3); // data
          this->WriteSegmentSelector(kI386_es,(2<<3) | (0<<2) | 3); // data
          this->WriteSegmentSelector(kI386_fs,(2<<3) | (0<<2) | 3); // data
          this->WriteSegmentSelector(kI386_gs,(3<<3) | (0<<2) | 3); // tls
          
          return true;
        }
        
        void SetSystemCallStatus(int ret, bool error) const
        {
          throw "TODO:";
        }
        
        SysCall* GetSystemCall( int& id ) const
        {
          // TODO: fill syscall map
          // see either arch/i386/include/asm/unistd.h or arch/i386/include/uapi/asm/unistd.h in Linux source
          switch (id) {
          case 0: return lin.GetSyscallByName("restart_syscall");
          case 1: return lin.GetSyscallByName("exit");
          case 2: return lin.GetSyscallByName("fork");
          case 3: return lin.GetSyscallByName("read");
          case 4: return lin.GetSyscallByName("write");
          case 5: return lin.GetSyscallByName("open");
          case 6: return lin.GetSyscallByName("close");
          case 7: return lin.GetSyscallByName("waitpid");
          case 8: return lin.GetSyscallByName("creat");
          case 9: return lin.GetSyscallByName("link");
          case 10: return lin.GetSyscallByName("unlink");
          case 11: return lin.GetSyscallByName("execve");
          case 12: return lin.GetSyscallByName("chdir");
          case 13: return lin.GetSyscallByName("time");
          case 14: return lin.GetSyscallByName("mknod");
          case 15: return lin.GetSyscallByName("chmod");
          case 16: return lin.GetSyscallByName("lchown");
          case 17: return lin.GetSyscallByName("break");
          case 18: return lin.GetSyscallByName("oldstat");
          case 19: return lin.GetSyscallByName("lseek");
          case 20: return lin.GetSyscallByName("getpid");
          case 21: return lin.GetSyscallByName("mount");
          case 22: return lin.GetSyscallByName("umount");
          case 23: return lin.GetSyscallByName("setuid");
          case 24: return lin.GetSyscallByName("getuid");
          case 25: return lin.GetSyscallByName("stime");
          case 26: return lin.GetSyscallByName("ptrace");
          case 27: return lin.GetSyscallByName("alarm");
          case 28: return lin.GetSyscallByName("oldfstat");
          case 29: return lin.GetSyscallByName("pause");
          case 30: return lin.GetSyscallByName("utime");
          case 31: return lin.GetSyscallByName("stty");
          case 32: return lin.GetSyscallByName("gtty");
          case 33: return lin.GetSyscallByName("access");
          case 34: return lin.GetSyscallByName("nice");
          case 35: return lin.GetSyscallByName("ftime");
          case 36: return lin.GetSyscallByName("sync");
          case 37: return lin.GetSyscallByName("kill");
          case 38: return lin.GetSyscallByName("rename");
          case 39: return lin.GetSyscallByName("mkdir");
          case 40: return lin.GetSyscallByName("rmdir");
          case 41: return lin.GetSyscallByName("dup");
          case 42: return lin.GetSyscallByName("pipe");
            // // 43: times (see i386 specific)
          case 44: return lin.GetSyscallByName("prof");
          case 45: return lin.GetSyscallByName("brk");
          case 46: return lin.GetSyscallByName("setgid");
          case 47: return lin.GetSyscallByName("getgid");
          case 48: return lin.GetSyscallByName("signal");
          case 49: return lin.GetSyscallByName("geteuid");
          case 50: return lin.GetSyscallByName("getegid");
          case 51: return lin.GetSyscallByName("acct");
          case 52: return lin.GetSyscallByName("umount2");
          case 53: return lin.GetSyscallByName("lock");
          case 54: return lin.GetSyscallByName("ioctl");
          case 55: return lin.GetSyscallByName("fcntl");
          case 56: return lin.GetSyscallByName("mpx");
          case 57: return lin.GetSyscallByName("setpgid");
          case 58: return lin.GetSyscallByName("ulimit");
          case 59: return lin.GetSyscallByName("oldolduname");
          case 60: return lin.GetSyscallByName("umask");
          case 61: return lin.GetSyscallByName("chroot");
          case 62: return lin.GetSyscallByName("ustat");
          case 63: return lin.GetSyscallByName("dup2");
          case 64: return lin.GetSyscallByName("getppid");
          case 65: return lin.GetSyscallByName("getpgrp");
          case 66: return lin.GetSyscallByName("setsid");
          case 67: return lin.GetSyscallByName("sigaction");
          case 68: return lin.GetSyscallByName("sgetmask");
          case 69: return lin.GetSyscallByName("ssetmask");
          case 70: return lin.GetSyscallByName("setreuid");
          case 71: return lin.GetSyscallByName("setregid");
          case 72: return lin.GetSyscallByName("sigsuspend");
          case 73: return lin.GetSyscallByName("sigpending");
          case 74: return lin.GetSyscallByName("sethostname");
          case 75: return lin.GetSyscallByName("setrlimit");
          case 76: return lin.GetSyscallByName("getrlimit");
          case 77: return lin.GetSyscallByName("getrusage");
            // 78: gettimeofday (see i386 specific)
          case 79: return lin.GetSyscallByName("settimeofday");
          case 80: return lin.GetSyscallByName("getgroups");
          case 81: return lin.GetSyscallByName("setgroups");
          case 82: return lin.GetSyscallByName("select");
          case 83: return lin.GetSyscallByName("symlink");
          case 84: return lin.GetSyscallByName("oldlstat");
          case 85: return lin.GetSyscallByName("readlink");
          case 86: return lin.GetSyscallByName("uselib");
          case 87: return lin.GetSyscallByName("swapon");
          case 88: return lin.GetSyscallByName("reboot");
          case 89: return lin.GetSyscallByName("readdir");
          case 90: return lin.GetSyscallByName("mmap");
          case 91: return lin.GetSyscallByName("munmap");
          case 92: return lin.GetSyscallByName("truncate");
          case 93: return lin.GetSyscallByName("ftruncate");
          case 94: return lin.GetSyscallByName("fchmod");
          case 95: return lin.GetSyscallByName("fchown");
          case 96: return lin.GetSyscallByName("getpriority");
          case 97: return lin.GetSyscallByName("setpriority");
          case 98: return lin.GetSyscallByName("profil");
          case 99: return lin.GetSyscallByName("statfs");
          case 100: return lin.GetSyscallByName("fstatfs");
          case 101: return lin.GetSyscallByName("ioperm");
          case 102: return lin.GetSyscallByName("socketcall");
          case 103: return lin.GetSyscallByName("syslog");
          case 104: return lin.GetSyscallByName("setitimer");
          case 105: return lin.GetSyscallByName("getitimer");
          case 106: return lin.GetSyscallByName("stat");
          case 107: return lin.GetSyscallByName("lstat");
            // 108 fstat (see i386 specific)
          case 109: return lin.GetSyscallByName("olduname");
          case 110: return lin.GetSyscallByName("iopl");
          case 111: return lin.GetSyscallByName("vhangup");
          case 112: return lin.GetSyscallByName("idle");
          case 113: return lin.GetSyscallByName("vm86old");
          case 114: return lin.GetSyscallByName("wait4");
          case 115: return lin.GetSyscallByName("swapoff");
          case 116: return lin.GetSyscallByName("sysinfo");
          case 117: return lin.GetSyscallByName("ipc");
          case 118: return lin.GetSyscallByName("fsync");
          case 119: return lin.GetSyscallByName("sigreturn");
          case 120: return lin.GetSyscallByName("clone");
          case 121: return lin.GetSyscallByName("setdomainname");
            // 122: uname (see i386 specific)
          case 123: return lin.GetSyscallByName("modify_ldt");
          case 124: return lin.GetSyscallByName("adjtimex");
          case 125: return lin.GetSyscallByName("mprotect");
          case 126: return lin.GetSyscallByName("sigprocmask");
          case 127: return lin.GetSyscallByName("create_module");
          case 128: return lin.GetSyscallByName("init_module");
          case 129: return lin.GetSyscallByName("delete_module");
          case 130: return lin.GetSyscallByName("get_kernel_syms");
          case 131: return lin.GetSyscallByName("quotactl");
          case 132: return lin.GetSyscallByName("getpgid");
          case 133: return lin.GetSyscallByName("fchdir");
          case 134: return lin.GetSyscallByName("bdflush");
          case 135: return lin.GetSyscallByName("sysfs");
          case 136: return lin.GetSyscallByName("personality");
          case 137: return lin.GetSyscallByName("afs_syscall");
          case 138: return lin.GetSyscallByName("setfsuid");
          case 139: return lin.GetSyscallByName("setfsgid");
          case 140: return lin.GetSyscallByName("_llseek");
          case 141: return lin.GetSyscallByName("getdents");
          case 142: return lin.GetSyscallByName("_newselect");
          case 143: return lin.GetSyscallByName("flock");
          case 144: return lin.GetSyscallByName("msync");
          case 145: return lin.GetSyscallByName("readv");
          case 146: return lin.GetSyscallByName("writev");
          case 147: return lin.GetSyscallByName("getsid");
          case 148: return lin.GetSyscallByName("fdatasync");
          case 149: return lin.GetSyscallByName("_sysctl");
          case 150: return lin.GetSyscallByName("mlock");
          case 151: return lin.GetSyscallByName("munlock");
          case 152: return lin.GetSyscallByName("mlockall");
          case 153: return lin.GetSyscallByName("munlockall");
          case 154: return lin.GetSyscallByName("sched_setparam");
          case 155: return lin.GetSyscallByName("sched_getparam");
          case 156: return lin.GetSyscallByName("sched_setscheduler");
          case 157: return lin.GetSyscallByName("sched_getscheduler");
          case 158: return lin.GetSyscallByName("sched_yield");
          case 159: return lin.GetSyscallByName("sched_get_priority_max");
          case 160: return lin.GetSyscallByName("sched_get_priority_min");
          case 161: return lin.GetSyscallByName("sched_rr_get_interval");
          case 162: return lin.GetSyscallByName("nanosleep");
          case 163: return lin.GetSyscallByName("mremap");
          case 164: return lin.GetSyscallByName("setresuid");
          case 165: return lin.GetSyscallByName("getresuid");
          case 166: return lin.GetSyscallByName("vm86");
          case 167: return lin.GetSyscallByName("query_module");
          case 168: return lin.GetSyscallByName("poll");
          case 169: return lin.GetSyscallByName("nfsservctl");
          case 170: return lin.GetSyscallByName("setresgid");
          case 171: return lin.GetSyscallByName("getresgid");
          case 172: return lin.GetSyscallByName("prctl");
          case 173: return lin.GetSyscallByName("rt_sigreturn");
          case 174: return lin.GetSyscallByName("rt_sigaction");
          case 175: return lin.GetSyscallByName("rt_sigprocmask");
          case 176: return lin.GetSyscallByName("rt_sigpending");
          case 177: return lin.GetSyscallByName("rt_sigtimedwait");
          case 178: return lin.GetSyscallByName("rt_sigqueueinfo");
          case 179: return lin.GetSyscallByName("rt_sigsuspend");
          case 180: return lin.GetSyscallByName("pread64");
          case 181: return lin.GetSyscallByName("pwrite64");
          case 182: return lin.GetSyscallByName("chown");
          case 183: return lin.GetSyscallByName("getcwd");
          case 184: return lin.GetSyscallByName("capget");
          case 185: return lin.GetSyscallByName("capset");
          case 186: return lin.GetSyscallByName("sigaltstack");
          case 187: return lin.GetSyscallByName("sendfile");
          case 188: return lin.GetSyscallByName("getpmsg");
          case 189: return lin.GetSyscallByName("putpmsg");
          case 190: return lin.GetSyscallByName("vfork");
          case 191: return lin.GetSyscallByName("ugetrlimit");
          case 192: return lin.GetSyscallByName("mmap2");
          case 193: return lin.GetSyscallByName("truncate64");
          case 194: return lin.GetSyscallByName("ftruncate64");
            // 195 stat64 (see i386 specific)
          case 196: return lin.GetSyscallByName("lstat64");
            // 197 fstat64 (see i386 specific)
          case 198: return lin.GetSyscallByName("lchown32");
          case 199: return lin.GetSyscallByName("getuid32");
          case 200: return lin.GetSyscallByName("getgid32");
          case 201: return lin.GetSyscallByName("geteuid32");
          case 202: return lin.GetSyscallByName("getegid32");
          case 203: return lin.GetSyscallByName("setreuid32");
          case 204: return lin.GetSyscallByName("setregid32");
          case 205: return lin.GetSyscallByName("getgroups32");
          case 206: return lin.GetSyscallByName("setgroups32");
          case 207: return lin.GetSyscallByName("fchown32");
          case 208: return lin.GetSyscallByName("setresuid32");
          case 209: return lin.GetSyscallByName("getresuid32");
          case 210: return lin.GetSyscallByName("setresgid32");
          case 211: return lin.GetSyscallByName("getresgid32");
          case 212: return lin.GetSyscallByName("chown32");
          case 213: return lin.GetSyscallByName("setuid32");
          case 214: return lin.GetSyscallByName("setgid32");
          case 215: return lin.GetSyscallByName("setfsuid32");
          case 216: return lin.GetSyscallByName("setfsgid32");
          case 217: return lin.GetSyscallByName("pivot_root");
          case 218: return lin.GetSyscallByName("mincore");
          case 219: return lin.GetSyscallByName("madvise");
          case 220: return lin.GetSyscallByName("getdents64");
          case 221: return lin.GetSyscallByName("fcntl64");
          case 224: return lin.GetSyscallByName("gettid");
          case 225: return lin.GetSyscallByName("readahead");
          case 226: return lin.GetSyscallByName("setxattr");
          case 227: return lin.GetSyscallByName("lsetxattr");
          case 228: return lin.GetSyscallByName("fsetxattr");
          case 229: return lin.GetSyscallByName("getxattr");
          case 230: return lin.GetSyscallByName("lgetxattr");
          case 231: return lin.GetSyscallByName("fgetxattr");
          case 232: return lin.GetSyscallByName("listxattr");
          case 233: return lin.GetSyscallByName("llistxattr");
          case 234: return lin.GetSyscallByName("flistxattr");
          case 235: return lin.GetSyscallByName("removexattr");
          case 236: return lin.GetSyscallByName("lremovexattr");
          case 237: return lin.GetSyscallByName("fremovexattr");
          case 238: return lin.GetSyscallByName("tkill");
          case 239: return lin.GetSyscallByName("sendfile64");
          case 240: return lin.GetSyscallByName("futex");
          case 241: return lin.GetSyscallByName("sched_setaffinity");
          case 242: return lin.GetSyscallByName("sched_getaffinity");
          case 243: return lin.GetSyscallByName("set_thread_area");
          case 244: return lin.GetSyscallByName("get_thread_area");
          case 245: return lin.GetSyscallByName("io_setup");
          case 246: return lin.GetSyscallByName("io_destroy");
          case 247: return lin.GetSyscallByName("io_getevents");
          case 248: return lin.GetSyscallByName("io_submit");
          case 249: return lin.GetSyscallByName("io_cancel");
          case 250: return lin.GetSyscallByName("fadvise64");
          case 252: return lin.GetSyscallByName("exit_group");
          case 253: return lin.GetSyscallByName("lookup_dcookie");
          case 254: return lin.GetSyscallByName("epoll_create");
          case 255: return lin.GetSyscallByName("epoll_ctl");
          case 256: return lin.GetSyscallByName("epoll_wait");
          case 257: return lin.GetSyscallByName("remap_file_pages");
          case 258: return lin.GetSyscallByName("set_tid_address");
          case 259: return lin.GetSyscallByName("timer_create");
          case 260: return lin.GetSyscallByName("timer_settime");
          case 261: return lin.GetSyscallByName("timer_gettime");
          case 262: return lin.GetSyscallByName("timer_getoverrun");
          case 263: return lin.GetSyscallByName("timer_delete");
          case 264: return lin.GetSyscallByName("clock_settime");
          case 265: return lin.GetSyscallByName("clock_gettime");
          case 266: return lin.GetSyscallByName("clock_getres");
          case 267: return lin.GetSyscallByName("clock_nanosleep");
          case 268: return lin.GetSyscallByName("statfs64");
          case 269: return lin.GetSyscallByName("fstatfs64");
          case 270: return lin.GetSyscallByName("tgkill");
          case 271: return lin.GetSyscallByName("utimes");
          case 272: return lin.GetSyscallByName("fadvise64_64");
          case 273: return lin.GetSyscallByName("vserver");
          case 274: return lin.GetSyscallByName("mbind");
          case 275: return lin.GetSyscallByName("get_mempolicy");
          case 276: return lin.GetSyscallByName("set_mempolicy");
          case 277: return lin.GetSyscallByName("mq_open");
          case 278: return lin.GetSyscallByName("mq_unlink");
          case 279: return lin.GetSyscallByName("mq_timedsend");
          case 280: return lin.GetSyscallByName("mq_timedreceive");
          case 281: return lin.GetSyscallByName("mq_notify");
          case 282: return lin.GetSyscallByName("mq_getsetattr");
          case 283: return lin.GetSyscallByName("kexec_load");
          case 284: return lin.GetSyscallByName("waitid");
          case 286: return lin.GetSyscallByName("add_key");
          case 287: return lin.GetSyscallByName("request_key");
          case 288: return lin.GetSyscallByName("keyctl");
          case 289: return lin.GetSyscallByName("ioprio_set");
          case 290: return lin.GetSyscallByName("ioprio_get");
          case 291: return lin.GetSyscallByName("inotify_init");
          case 292: return lin.GetSyscallByName("inotify_add_watch");
          case 293: return lin.GetSyscallByName("inotify_rm_watch");
          case 294: return lin.GetSyscallByName("migrate_pages");
          case 295: return lin.GetSyscallByName("openat");
          case 296: return lin.GetSyscallByName("mkdirat");
          case 297: return lin.GetSyscallByName("mknodat");
          case 298: return lin.GetSyscallByName("fchownat");
          case 299: return lin.GetSyscallByName("futimesat");
          case 300: return lin.GetSyscallByName("fstatat64");
          case 301: return lin.GetSyscallByName("unlinkat");
          case 302: return lin.GetSyscallByName("renameat");
          case 303: return lin.GetSyscallByName("linkat");
          case 304: return lin.GetSyscallByName("symlinkat");
          case 305: return lin.GetSyscallByName("readlinkat");
          case 306: return lin.GetSyscallByName("fchmodat");
          case 307: return lin.GetSyscallByName("faccessat");
          case 308: return lin.GetSyscallByName("pselect6");
          case 309: return lin.GetSyscallByName("ppoll");
          case 310: return lin.GetSyscallByName("unshare");
          case 311: return lin.GetSyscallByName("set_robust_list");
          case 312: return lin.GetSyscallByName("get_robust_list");
          case 313: return lin.GetSyscallByName("splice");
          case 314: return lin.GetSyscallByName("sync_file_range");
          case 315: return lin.GetSyscallByName("tee");
          case 316: return lin.GetSyscallByName("vmsplice");
          case 317: return lin.GetSyscallByName("move_pages");
          case 318: return lin.GetSyscallByName("getcpu");
          case 319: return lin.GetSyscallByName("epoll_pwait");
          case 320: return lin.GetSyscallByName("utimensat");
          case 321: return lin.GetSyscallByName("signalfd");
          case 322: return lin.GetSyscallByName("timerfd_create");
          case 323: return lin.GetSyscallByName("eventfd");
          case 324: return lin.GetSyscallByName("fallocate");
          case 325: return lin.GetSyscallByName("timerfd_settime");
          case 326: return lin.GetSyscallByName("timerfd_gettime");
          case 327: return lin.GetSyscallByName("signalfd4");
          case 328: return lin.GetSyscallByName("eventfd2");
          case 329: return lin.GetSyscallByName("epoll_create1");
          case 330: return lin.GetSyscallByName("dup3");
          case 331: return lin.GetSyscallByName("pipe2");
          case 332: return lin.GetSyscallByName("inotify_init1");
          case 333: return lin.GetSyscallByName("preadv");
          case 334: return lin.GetSyscallByName("pwritev");
          case 335: return lin.GetSyscallByName("rt_tgsigqueueinfo");
          case 336: return lin.GetSyscallByName("perf_event_open");
          case 337: return lin.GetSyscallByName("recvmmsg");
          case 338: return lin.GetSyscallByName("fanotify_init");
          case 339: return lin.GetSyscallByName("fanotify_mark");
          case 340: return lin.GetSyscallByName("prlimit64");
          case 341: return lin.GetSyscallByName("name_to_handle_at");
          case 342: return lin.GetSyscallByName("open_by_handle_at");
          case 343: return lin.GetSyscallByName("clock_adjtime");
          case 344: return lin.GetSyscallByName("syncfs");
          case 345: return lin.GetSyscallByName("sendmmsg");
          case 346: return lin.GetSyscallByName("setns");
          case 347: return lin.GetSyscallByName("process_vm_readv");
          case 348: return lin.GetSyscallByName("process_vm_writev");
          case 349: return lin.GetSyscallByName("kcmp");
          case 350: return lin.GetSyscallByName("finit_module");
          case 351: return lin.GetSyscallByName("sched_setattr");
          case 352: return lin.GetSyscallByName("sched_getattr");

          case 122: /* i386 specific uname syscall */
            {
              static struct : public SysCall {
                char const* GetName() const { return "uname"; }
                void Execute( Linux& lin, int syscall_id ) const
                {
                  int ret;
                  int32_t target_errno = 0;
                  ADDRESS_TYPE buf_addr = lin.target_system->GetSystemCallParam(0);
                  ret = 0;
	
                  struct i386_utsname value;
                  memset(&value, 0, sizeof(value));
                  strncpy(&value.sysname[0],    lin.utsname_sysname_.c_str(), sizeof(value.sysname) - 1);
                  strncpy(&value.nodename[0],   lin.utsname_nodename_.c_str(), sizeof(value.nodename) - 1);
                  strncpy(&value.release[0],    lin.utsname_release_.c_str(), sizeof(value.release) - 1);
                  strncpy(&value.version[0],    lin.utsname_version_.c_str(), sizeof(value.version) - 1);
                  strncpy(&value.machine[0],    lin.utsname_machine_.c_str(), sizeof(value.machine));
                  strncpy(&value.domainname[0], lin.utsname_domainname_.c_str(), sizeof(value.domainname) - 1);
                  lin.WriteMem(buf_addr, (uint8_t *)&value, sizeof(value));
	
                  lin.SetSystemCallStatus((ret == -1) ? -target_errno : ret, (ret == -1));
                }
              } sc;
              return &sc;
            } break;

          }
          
          return 0;
        }
        PARAMETER_TYPE GetSystemCallParam(int id) const
        {
          PARAMETER_TYPE val = 0;
          
          switch (id) {
          case 0: lin.GetRegister(kI386_ebx, &val); break;
          case 1: lin.GetRegister(kI386_ecx, &val); break;
          case 2: lin.GetRegister(kI386_edx, &val); break;
          case 3: lin.GetRegister(kI386_esi, &val); break;
          case 4: lin.GetRegister(kI386_edi, &val); break;
          case 5: lin.GetRegister(kI386_ebp, &val); break;
          default:
            lin.logger_ << DebugError << "No syscall argument #" << id << " in " << this->name << " linux" << EndDebugError;
            break;
          }
          
          return val;
        }
        bool SetSystemBlob( unisim::util::debug::blob::Blob<ADDRESS_TYPE>* blob ) const
        {
          typedef unisim::util::debug::blob::Section<ADDRESS_TYPE> Section;
          typedef unisim::util::debug::blob::Segment<ADDRESS_TYPE> Segment;
          
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
          ADDRESS_TYPE const etls_addr = 0xf0000000UL;
          ADDRESS_TYPE const etls_size = 0x00001000UL;
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
      delete target_system;
      target_system = new I386TS( *this );
      return true;
    }
  
  return false;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetEndianness(unisim::util::endian::endian_type endianness)
{
  endianness_ = endianness;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetStackBase(ADDRESS_TYPE stack_base)
{
  stack_base_ = stack_base;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetMemoryPageSize(ADDRESS_TYPE memory_page_size)
{
  memory_page_size_ = memory_page_size;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetUname(
    char const * const utsname_sysname,
    char const * const utsname_nodename,
    char const * const utsname_release,
    char const * const utsname_version,
    char const * const utsname_machine,
    char const * const utsname_domainname)
{
  utsname_sysname_    = utsname_sysname;
  utsname_nodename_   = utsname_nodename;
  utsname_release_    = utsname_release;
  utsname_version_    = utsname_version;
  utsname_machine_    = utsname_machine;
  utsname_domainname_ = utsname_domainname;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetHWCap(const char *hwcap)
{
  hwcap_ = hwcap;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetStdinPipeFilename(const char *filename)
{
  stdin_pipe_filename = filename;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetStdoutPipeFilename(const char *filename)
{
  stdout_pipe_filename = filename;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetStderrPipeFilename(const char *filename)
{
  stderr_pipe_filename = filename;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
unisim::util::debug::Register*
Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetDebugRegister( char const* regname )
{
  if (not regname) return 0;
  
  if (not regs_if_) {
    logger_ << DebugError << "No register interface is available" << EndDebugError;
    return 0;
  }
  
  unisim::util::debug::Register* reg = regs_if_->GetRegister(regname);
  if (not reg) {
    logger_ << DebugError << "Can't access register " << regname << EndDebugError;
    return 0;
  }
  
  if (reg->GetSize() != sizeof(PARAMETER_TYPE)) {
    logger_ << DebugError << "Bad register size for " << regname << EndDebugError;
    return 0;
  }
  
  return reg;
}


template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::GetRegister(char const* regname, PARAMETER_TYPE * const value )
{
  if (unisim::util::debug::Register* reg = GetDebugRegister(regname)) {
    reg->GetValue(value);
    return true;
  }
  return false;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetRegister( char const* regname, PARAMETER_TYPE value )
{
  if (unisim::util::debug::Register* reg = GetDebugRegister(regname)) {
    reg->SetValue(&value);
    return true;
  }
  return false;
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
	if (not target_system->SetSystemBlob(blob))
	{
		// TODO: Remove non finished state (i.e., unfinished
		//       blob, reset values, ...)
		logger_ << DebugError
			<< "Could not set the system blob." << EndDebugError;
		blob->Release();
		return false;
	}

	// Set mmap_brk_point and brk_point
	mmap_brk_point_ = mmap_base_;

	ADDRESS_TYPE top_addr = stack_base_ + stack_size - 1;
	if (verbose_)
	{
		logger_ << DebugInfo << "=== top_addr = 0x" << std::hex << top_addr << std::dec
		<< EndDebugInfo;
	}

	brk_point_ = top_addr +	(memory_page_size_ - (top_addr % memory_page_size_));

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

	if (blob_) blob_->Release();
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
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetupTarget()
{
  if ((mem_if_ == NULL) or (mem_inject_if_ == NULL) or (regs_if_ == NULL))
    {
      logger_ << DebugError << "The linux system interfaces (memory/register) were not assigned" << EndDebugError;
      return false;
    }

  if (blob_ == NULL)
    {
      logger_ << DebugError << "The linux system was not loaded." << EndDebugError;
      return false;
    }
  
  typedef unisim::util::debug::blob::Segment<ADDRESS_TYPE> Segment;
  typedef std::vector<const Segment*> SegmentVector;
  typedef typename SegmentVector::const_iterator SegmentVectorIterator;
  
  SegmentVector segments = blob_->GetSegments();
  for (SegmentVectorIterator it = segments.begin(); (it != segments.end()); it++)
    {
      if ((*it)->GetType() != Segment::TY_LOADABLE) continue;
      
      uint8_t const * data = (uint8_t const *)(*it)->GetData();
      ADDRESS_TYPE start, end;
      (*it)->GetAddrRange(start, end);
      
      if(verbose_) {
        logger_ << DebugInfo << "--> writing memory segment start = 0x"
                << std::hex << start << " end = 0x" << end << std::dec
                << EndDebugInfo;
      }
      
      if (not mem_if_->WriteMemory(start, data, end - start + 1))
        {
          logger_ << DebugError << "Error while writing the segments into the target memory." << EndDebugError;
          return false;
        }
    }

  return target_system->SetupTarget();
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
unisim::util::debug::blob::Blob<ADDRESS_TYPE> const * const Linux<ADDRESS_TYPE,
    PARAMETER_TYPE>::GetBlob() const { return blob_; }

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
void Linux<ADDRESS_TYPE, PARAMETER_TYPE>::ExecuteSystemCall(int id, bool& terminated, int& return_status)
{
  if (not is_load_)
    {
      logger_
        << DebugError
        << "unisim::util::os::linux_os::Linux.ExecuteSystemCall: "
        << "Trying to execute system call with id " << id << " while the linux "
        << "emulation has not been correctly loaded."
        << EndDebugError;
      return;
    }
        
  int translated_id = id;
  
  SysCall* sc = target_system->GetSystemCall( translated_id );
  
  if (not sc) {
    logger_ << DebugError << "Unknown syscall(id = " << translated_id << ", untranslated id = " << id << ")" << EndDebugError;
    // FIXME: shouldn't we end the simulation (terminated = true) ?
    SetSystemCallStatus(-LINUX_ENOSYS, true);
    return;
  }
        
  if (unlikely(verbose_))
    {
      logger_
        << DebugInfo
        << "Executing syscall(name = " << sc->GetName() << ", "
        << "id = " << translated_id << ", " << "unstranslated id = " << id << ")"
        << EndDebugInfo;
    }
  
  try {
    sc->Execute( *this, translated_id );
  } catch (LSCExit const scx) {
    terminated = true;
    return_status = scx.status;
  }
  sc->Release();
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::LoadFiles( unisim::util::debug::blob::Blob<ADDRESS_TYPE> *blob )
{
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
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::CreateStack(unisim::util::debug::blob::Blob<ADDRESS_TYPE>* blob, uint64_t& stack_size) const
{
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
  
  if (target_system->GetAT_HWCAP( aux_table_value ))
    
  {
    aux_table_symbol = AT_HWCAP;
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
ADDRESS_TYPE Linux<ADDRESS_TYPE, PARAMETER_TYPE>::SetAuxTableEntry( uint8_t * stack_data, ADDRESS_TYPE sp, ADDRESS_TYPE entry, ADDRESS_TYPE value) const
{
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
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::ReadMem( ADDRESS_TYPE addr, uint8_t * buffer, uint32_t size )
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
bool Linux<ADDRESS_TYPE, PARAMETER_TYPE>::WriteMem( ADDRESS_TYPE addr, uint8_t const * const buffer, uint32_t size)
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
