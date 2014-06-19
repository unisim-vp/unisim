/*
 *  Copyright (c) 2014,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */
 
#ifndef __UNISIM_SERVICE_OS_AVR32_T2H_SYSCALLS_AVR32_T2H_SYSCALLS_TCC__
#define __UNISIM_SERVICE_OS_AVR32_T2H_SYSCALLS_AVR32_T2H_SYSCALLS_TCC__

#include <map>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>

#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/service/interfaces/memory_injection.hh"
#include "unisim/util/endian/endian.hh"
#include "unisim/util/likely/likely.hh"

namespace unisim {
namespace service {
namespace os {
namespace avr32_t2h_syscalls {

using std::string;
using std::stringstream;
using std::hex;
using std::dec;
using std::endl;
using std::flush;
using std::map;
using unisim::util::endian::Host2BigEndian;
using unisim::kernel::service::Service;
using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::Parameter;
using unisim::kernel::logger::Logger;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;
using unisim::service::interfaces::MemoryInjection;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::SymbolTableLookup;

#ifdef WIN32
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

template <class MEMORY_ADDR>
AVR32_T2H_Syscalls<MEMORY_ADDR>::AVR32_T2H_Syscalls(const char *name, Object *parent)
	: Object(name, parent)
	, Service<unisim::service::interfaces::AVR32_T2H_Syscalls>(name, parent)
	, Client<MemoryInjection<MEMORY_ADDR> >(name, parent)
	, Client<Registers>(name, parent)
	, Client<Blob<MEMORY_ADDR> >(name, parent)
	, avr32_t2h_syscalls_export("avr32-t2h-syscalls-export", this)
	, memory_injection_import("memory-injection-import", this)
	, registers_import("registers-import", this)
	, blob_import("blob-import", this)
	, logger(*this)
	, reg_pc(0)
	, reg_sp(0)
	, reg_syscall_num(0)
	, reg_params()
	, reg_return_status(0)
	, reg_errno(0)
	, t2h_syscall_table()
	, terminated(false)
	, exit_status(0)
	, pc_register_name("pc")
	, sp_register_name("sp")
	, syscall_num_register_name("r8")
	, return_status_register_name("r12")
	, errno_register_name("r11")
	, param_register_names()
	, verbose_all(false)
	, verbose_syscalls(false)
	, verbose_setup(false)
	, param_verbose_all("verbose-all", this, verbose_all, "globally enable/disable verbosity")
	, param_verbose_syscalls("verbose-syscalls", this, verbose_syscalls, "enable/disable verbosity while system calls")
	, param_verbose_setup("verbose-setup", this, verbose_setup, "enable/disable verbosity while setup")
	, param_argc("argc", this, argc, "Number of program arguments")
{
	reg_params[0] = 0;
	reg_params[1] = 0;
	reg_params[2] = 0;
	reg_params[3] = 0;
	reg_params[4] = 0;

	param_register_names[0] = "r12";
	param_register_names[1] = "r11";
	param_register_names[2] = "r10";
	param_register_names[3] = "r9";
	param_register_names[4] = "r8";

	t2h_syscall_table[T2H_SYSCALL_OPEN] = &AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_open;
	t2h_syscall_table[T2H_SYSCALL_CLOSE] = &AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_close;
	t2h_syscall_table[T2H_SYSCALL_READ] = &AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_read;
	t2h_syscall_table[T2H_SYSCALL_WRITE] = &AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_write;
	t2h_syscall_table[T2H_SYSCALL_LSEEK] = &AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_lseek;
	t2h_syscall_table[T2H_SYSCALL_RENAME] = &AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_rename;
	t2h_syscall_table[T2H_SYSCALL_UNLINK] = &AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_unlink;
	t2h_syscall_table[T2H_SYSCALL_STAT] = &AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_stat;
	t2h_syscall_table[T2H_SYSCALL_FSTAT] = &AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_fstat;
	t2h_syscall_table[T2H_SYSCALL_GETTIMEOFDAY] = &AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_gettimeofday;
	t2h_syscall_table[T2H_SYSCALL_ISATTY] = &AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_isatty;
	t2h_syscall_table[T2H_SYSCALL_SYSTEM] = &AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_system;
	t2h_syscall_table[T2H_SYSCALL_EXIT] = &AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_exit;
	t2h_syscall_table[T2H_SYSCALL_INIT_ARGV] = &AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_init_argv;
}

template <class MEMORY_ADDR>
AVR32_T2H_Syscalls<MEMORY_ADDR>::~AVR32_T2H_Syscalls()
{
	std::vector<std::string *>::iterator it_argv;

	for(it_argv = argv.begin(); it_argv != argv.end(); it_argv++)
	{
		if(*it_argv) delete (*it_argv);
	}

	std::vector<Parameter<std::string> *>::iterator it_param_argv;

	for(it_param_argv = param_argv.begin(); it_param_argv != param_argv.end(); it_param_argv++)
	{
		if(*it_param_argv) delete (*it_param_argv);
	}
}

template <class MEMORY_ADDR>
void AVR32_T2H_Syscalls<MEMORY_ADDR>::OnDisconnect()
{
}

template <class MEMORY_ADDR>
bool AVR32_T2H_Syscalls<MEMORY_ADDR>::EndSetup()
{
	unsigned int i;

	if(!memory_injection_import)
	{
		logger << DebugError << memory_injection_import.GetName() << " is not connected" << EndDebugError;
		return false;
	}

	if(!registers_import)
	{
		logger << DebugError << registers_import.GetName() << " is not connected" << EndDebugError;
		return false;
	}
	
	if(!blob_import)
	{
		logger << DebugError << blob_import.GetName() << " is not connected" << EndDebugError;
		return false;
	}

	reg_pc = registers_import->GetRegister(pc_register_name.c_str());

	if(!reg_pc)
	{
		logger << DebugError << "Undefined register " << pc_register_name << "." << EndDebugError;
		return false;
	}

	if(reg_pc->GetSize() != 4)
	{
		logger << DebugError << "Register " << pc_register_name << " is not a 32-bit register." << EndDebugError;
		return false;
	}

	reg_sp = registers_import->GetRegister(sp_register_name.c_str());

	if(!reg_sp)
	{
		logger << DebugError << "Undefined register " << sp_register_name << "." << EndDebugError;
		return false;
	}

	if(reg_sp->GetSize() != 4)
	{
		logger << DebugError << "Register " << sp_register_name << " is not a 32-bit register." << EndDebugError;
		return false;
	}
	
	reg_syscall_num = registers_import->GetRegister(syscall_num_register_name.c_str());

	if(!reg_syscall_num)
	{
		logger << DebugError << "Undefined register " << syscall_num_register_name << "." << EndDebugError;
		return false;
	}

	if(reg_syscall_num->GetSize() != 4)
	{
		logger << DebugError << "Register " << syscall_num_register_name << " is not a 32-bit register." << EndDebugError;
		return false;
	}

	for(i = 0; i < 5; i++)
	{
		reg_params[i] = registers_import->GetRegister(param_register_names[i].c_str());
		if(!reg_params[i])
		{
			logger << DebugError << "Undefined register " << param_register_names[i] << "." << EndDebugError;
			return false;
		}

		if(reg_params[i]->GetSize() != 4)
		{
			logger << DebugError << "Register " << param_register_names[i] << " is not a 32-bit register." << EndDebugError;
			return false;
		}
	}

	reg_return_status = registers_import->GetRegister(return_status_register_name.c_str());

	if(!reg_return_status)
	{
		logger << DebugError << "Undefined register " << return_status_register_name << "." << EndDebugError;
		return false;
	}

	if(reg_return_status->GetSize() != 4)
	{
		logger << DebugError << "Register " << return_status_register_name << " is not a 32-bit register." << EndDebugError;
		return false;
	}

	reg_errno = registers_import->GetRegister(errno_register_name.c_str());

	if(!reg_errno)
	{
		logger << DebugError << "Undefined register " << errno_register_name << "." << EndDebugError;
		return false;
	}

	if(reg_errno->GetSize() != 4)
	{
		logger << DebugError << "Register " << errno_register_name << " is not a 32-bit register." << EndDebugError;
		return false;
	}

	for(i = 0; i < argc; i++)
	{
		std::string *arg = new std::string();
		argv.push_back(arg);
		std::stringstream sstr_param_argv_name;
		sstr_param_argv_name << "argv[" << i << "]";
		param_argv.push_back(new Parameter<std::string>(sstr_param_argv_name.str().c_str(), this, *arg, "program argument"));
	}
	
	
	const unisim::util::debug::blob::Blob<MEMORY_ADDR> *blob = blob_import->GetBlob();
	MEMORY_ADDR entry_point = blob->GetEntryPoint();
	reg_pc->SetValue(&entry_point);

	return true;
}

template <class MEMORY_ADDR>
const char *AVR32_T2H_Syscalls<MEMORY_ADDR>::GetSyscallFriendlyName(uint32_t command)
{
	switch(command)
	{
		case T2H_SYSCALL_OPEN: return "open";
		case T2H_SYSCALL_CLOSE: return "close";
		case T2H_SYSCALL_READ: return "read";
		case T2H_SYSCALL_WRITE: return "write";
		case T2H_SYSCALL_LSEEK: return "lseek";
		case T2H_SYSCALL_RENAME: return "rename";
		case T2H_SYSCALL_UNLINK: return "unlink";
		case T2H_SYSCALL_STAT: return "stat";
		case T2H_SYSCALL_FSTAT: return "fstat";
		case T2H_SYSCALL_GETTIMEOFDAY: return "gettimeofday";
		case T2H_SYSCALL_ISATTY: return "isatty";
		case T2H_SYSCALL_SYSTEM: return "system";
		case T2H_SYSCALL_EXIT: return "exit";
		case T2H_SYSCALL_INIT_ARGV: return "init_argv";
	}
	return "?";
}

template <class MEMORY_ADDR>
uint32_t AVR32_T2H_Syscalls<MEMORY_ADDR>::GetSystemCallParam(unsigned int arg_num)
{
	uint32_t value = 0;
	reg_params[arg_num]->GetValue(&value);
	
	return value;
}

template <class MEMORY_ADDR>
void AVR32_T2H_Syscalls<MEMORY_ADDR>::SetSystemCallParam(unsigned int arg_num, uint32_t value)
{
	reg_params[arg_num]->SetValue(&value);
}

template <class MEMORY_ADDR>
void AVR32_T2H_Syscalls<MEMORY_ADDR>::SetSystemCallStatus(int32_t ret)
{
	reg_return_status->SetValue(&ret);
}

template <class MEMORY_ADDR>
void AVR32_T2H_Syscalls<MEMORY_ADDR>::SetErrno(int32_t target_errno)
{
	reg_errno->SetValue(&target_errno);
}

template <class MEMORY_ADDR>
int32_t AVR32_T2H_Syscalls<MEMORY_ADDR>::Host2TargetErrno(int host_errno)
{
	switch(host_errno)
	{
		case EPERM: return T2H_EPERM;
		case ENOENT: return T2H_ENOENT;
		case ESRCH: return T2H_ESRCH;
		case EINTR: return T2H_EINTR;
		case EIO: return T2H_EIO;
		case ENXIO: return T2H_ENXIO;
		case E2BIG: return T2H_E2BIG;
		case ENOEXEC: return T2H_ENOEXEC;
		case EBADF: return T2H_EBADF;
		case ECHILD: return T2H_ECHILD;
		case EAGAIN: return T2H_EAGAIN;
		case ENOMEM: return T2H_ENOMEM;
		case EACCES: return T2H_EACCES;
		case EFAULT: return T2H_EFAULT;
		case ENOTBLK: return T2H_ENOTBLK;
		case EBUSY: return T2H_EBUSY;
		case EEXIST: return T2H_EEXIST;
		case EXDEV: return T2H_EXDEV;
		case ENODEV: return T2H_ENODEV;
		case ENOTDIR: return T2H_ENOTDIR;
		case EISDIR: return T2H_EISDIR;
		case EINVAL: return T2H_EINVAL;
		case ENFILE: return T2H_ENFILE;
		case EMFILE: return T2H_EMFILE;
		case ENOTTY: return T2H_ENOTTY;
		case ETXTBSY: return T2H_ETXTBSY;
		case EFBIG: return T2H_EFBIG;
		case ENOSPC: return T2H_ENOSPC;
		case ESPIPE: return T2H_ESPIPE;
		case EROFS: return T2H_EROFS;
		case EMLINK: return T2H_EMLINK;
		case EPIPE: return T2H_EPIPE;
		case EDOM: return T2H_EDOM;
		case ERANGE: return T2H_ERANGE;
		case EDEADLK: return T2H_EDEADLK;
		case ENAMETOOLONG: return T2H_ENAMETOOLONG;
		case ENOLCK: return T2H_ENOLCK;
		case ENOSYS: return T2H_ENOSYS;
		case ENOTEMPTY: return T2H_ENOTEMPTY;
		case ELOOP: return T2H_ELOOP;
		case ENOMSG: return T2H_ENOMSG;
		case EIDRM: return T2H_EIDRM;
		case ECHRNG: return T2H_ECHRNG;
		case EL2NSYNC: return T2H_EL2NSYNC;
		case EL3HLT: return T2H_EL3HLT;
		case EL3RST: return T2H_EL3RST;
		case ELNRNG: return T2H_ELNRNG;
		case EUNATCH: return T2H_EUNATCH;
		case ENOCSI: return T2H_ENOCSI;
		case EL2HLT: return T2H_EL2HLT;
		case EBADE: return T2H_EBADE;
		case EBADR: return T2H_EBADR;
		case EXFULL: return T2H_EXFULL;
		case ENOANO: return T2H_ENOANO;
		case EBADRQC: return T2H_EBADRQC;
		case EBADSLT: return T2H_EBADSLT;
		case EBFONT: return T2H_EBFONT;
		case ENOSTR: return T2H_ENOSTR;
		case ENODATA: return T2H_ENODATA;
		case ETIME: return T2H_ETIME;
		case ENOSR: return T2H_ENOSR;
		case ENONET: return T2H_ENONET;
		case ENOPKG: return T2H_ENOPKG;
		case EREMOTE: return T2H_EREMOTE;
		case ENOLINK: return T2H_ENOLINK;
		case EADV: return T2H_EADV;
		case ESRMNT: return T2H_ESRMNT;
		case ECOMM: return T2H_ECOMM;
		case EPROTO: return T2H_EPROTO;
		case EMULTIHOP: return T2H_EMULTIHOP;
		case EDOTDOT: return T2H_EDOTDOT;
		case EBADMSG: return T2H_EBADMSG;
		case EOVERFLOW: return T2H_EOVERFLOW;
		case ENOTUNIQ: return T2H_ENOTUNIQ;
		case EBADFD: return T2H_EBADFD;
		case EREMCHG: return T2H_EREMCHG;
		case ELIBACC: return T2H_ELIBACC;
		case ELIBBAD: return T2H_ELIBBAD;
		case ELIBSCN: return T2H_ELIBSCN;
		case ELIBMAX: return T2H_ELIBMAX;
		case ELIBEXEC: return T2H_ELIBEXEC;
		case EILSEQ: return T2H_EILSEQ;
		case EUSERS: return T2H_EUSERS;
		case ENOTSOCK: return T2H_ENOTSOCK;
		case EDESTADDRREQ: return T2H_EDESTADDRREQ;
		case EMSGSIZE: return T2H_EMSGSIZE;
		case EPROTOTYPE: return T2H_EPROTOTYPE;
		case ENOPROTOOPT: return T2H_ENOPROTOOPT;
		case EPROTONOSUPPORT: return T2H_EPROTONOSUPPORT;
		case ESOCKTNOSUPPORT: return T2H_ESOCKTNOSUPPORT;
		case EOPNOTSUPP: return T2H_EOPNOTSUPP;
		case EPFNOSUPPORT: return T2H_EPFNOSUPPORT;
		case EAFNOSUPPORT: return T2H_EAFNOSUPPORT;
		case EADDRINUSE: return T2H_EADDRINUSE;
		case EADDRNOTAVAIL: return T2H_EADDRNOTAVAIL;
		case ENETDOWN: return T2H_ENETDOWN;
		case ENETUNREACH: return T2H_ENETUNREACH;
		case ENETRESET: return T2H_ENETRESET;
		case ECONNABORTED: return T2H_ECONNABORTED;
		case ECONNRESET: return T2H_ECONNRESET;
		case ENOBUFS: return T2H_ENOBUFS;
		case EISCONN: return T2H_EISCONN;
		case ENOTCONN: return T2H_ENOTCONN;
		case ESHUTDOWN: return T2H_ESHUTDOWN;
		case ETOOMANYREFS: return T2H_ETOOMANYREFS;
		case ETIMEDOUT: return T2H_ETIMEDOUT;
		case ECONNREFUSED: return T2H_ECONNREFUSED;
		case EHOSTDOWN: return T2H_EHOSTDOWN;
		case EHOSTUNREACH: return T2H_EHOSTUNREACH;
		case EALREADY: return T2H_EALREADY;
		case EINPROGRESS: return T2H_EINPROGRESS;
		case ESTALE: return T2H_ESTALE;
		case EDQUOT: return T2H_EDQUOT;
		case ENOMEDIUM: return T2H_ENOMEDIUM;
		case ECANCELED: return T2H_ECANCELED;
	}
	
	logger << DebugWarning << "Don't how to convert host errno #" << errno << " to target...Silently setting errno to EINVAL." << EndDebugWarning;
	
	return T2H_EINVAL;
}

template <class MEMORY_ADDR>
bool AVR32_T2H_Syscalls<MEMORY_ADDR>::StringLength(MEMORY_ADDR addr, int &string_length)
{
	string_length = 0;
	char c;

	while(1)
	{
		c = 0;
		if(!memory_injection_import->InjectReadMemory(addr, &c, 1)) return false;
		if(c == 0) break;
		string_length++;
		addr++;
	}
	
	return true;
}

template <class MEMORY_ADDR>
int AVR32_T2H_Syscalls<MEMORY_ADDR>::Fstat(int fd, struct avr32_stat *target_stat)
{
	int ret;
	struct stat host_stat;
	ret = fstat(fd, &host_stat);
	if(ret < 0) return ret;

#if defined(__x86_64) || defined(__amd64) || defined(__LP64__) || defined(_LP64) || defined(__amd64__)
	// 64-bit host
	target_stat->st_dev = Host2BigEndian((uint64_t) host_stat.st_dev);
	target_stat->st_ino = Host2BigEndian((uint64_t) host_stat.st_ino);
	target_stat->st_mode = Host2BigEndian((uint32_t) host_stat.st_mode);
	target_stat->st_nlink = Host2BigEndian((uint64_t) host_stat.st_nlink);
	target_stat->st_uid = Host2BigEndian((uint32_t) host_stat.st_uid);
	target_stat->st_gid = Host2BigEndian((uint32_t) host_stat.st_gid);
	target_stat->st_rdev = Host2BigEndian((int64_t) host_stat.st_rdev);
	target_stat->st_size = Host2BigEndian((int64_t) host_stat.st_size);
#if defined(WIN64) // Windows x64
	target_stat->st_blksize = Host2BigEndian((int32_t) 512);
	target_stat->st_blocks = Host2BigEndian((int64_t)((host_stat.st_size + 511) / 512));
	target_stat->st_atim = Host2BigEndian((int64_t) host_stat.st_atim);
	target_stat->st_mtim = Host2BigEndian((int64_t) host_stat.st_mtim);
	target_stat->st_ctim = Host2BigEndian((int64_t) host_stat.st_ctim);
#elif defined(linux) // Linux x64
	target_stat->st_blksize = Host2BigEndian((int64_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2BigEndian((int64_t) host_stat.st_blocks);
	target_stat->st_atim = Host2BigEndian((int64_t) host_stat.st_atim.tv_sec);
	target_stat->st_mtim = Host2BigEndian((int64_t) host_stat.st_mtim.tv_sec);
	target_stat->st_ctim = Host2BigEndian((int64_t) host_stat.st_ctim.tv_sec);
#elif defined(__APPLE_CC__) // darwin PPC64/x86_64
	target_stat->st_blksize = Host2BigEndian((int64_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2BigEndian((int64_t) host_stat.st_blocks);
	target_stat->st_atim = Host2BigEndian((int64_t) host_stat.st_atimespec.tv_sec);
	target_stat->st_mtim = Host2BigEndian((int64_t) host_stat.st_mtimespec.tv_sec);
	target_stat->st_ctim = Host2BigEndian((int64_t) host_stat.st_ctimespec.tv_sec);
#endif

#else
	// 32-bit host
	target_stat->st_dev = Host2BigEndian((uint64_t) host_stat.st_dev);
	target_stat->st_ino = Host2BigEndian((uint32_t) host_stat.st_ino);
	target_stat->st_mode = Host2BigEndian((uint32_t) host_stat.st_mode);
	target_stat->st_nlink = Host2BigEndian((uint32_t) host_stat.st_nlink);
	target_stat->st_uid = Host2BigEndian((uint32_t) host_stat.st_uid);
	target_stat->st_gid = Host2BigEndian((uint32_t) host_stat.st_gid);
	target_stat->st_rdev = Host2BigEndian((int64_t) host_stat.st_rdev);
	target_stat->st_size = Host2BigEndian((int64_t) host_stat.st_size);
#if defined(WIN32) // Windows 32
	target_stat->st_blksize = Host2BigEndian((int32_t) 512);
	target_stat->st_blocks = Host2BigEndian((int64_t)((host_stat.st_size + 511) / 512));
	target_stat->st_atim = Host2BigEndian((int32_t) host_stat.st_atime);
	target_stat->st_mtim = Host2BigEndian((int32_t) host_stat.st_mtime);
	target_stat->st_ctim = Host2BigEndian((int32_t) host_stat.st_ctime);
#elif defined(linux) // Linux 32
	target_stat->st_blksize = Host2BigEndian((int32_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2BigEndian((int64_t) host_stat.st_blocks);
	target_stat->st_atim = Host2BigEndian((int32_t) host_stat.st_atim.tv_sec);
	target_stat->st_mtim = Host2BigEndian((int32_t) host_stat.st_mtim.tv_sec);
	target_stat->st_ctim = Host2BigEndian((int32_t) host_stat.st_ctim.tv_sec);
#elif defined(__APPLE_CC__) // Darwin PPC32/x86
	target_stat->st_blksize = Host2BigEndian((int32_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2BigEndian((int64_t) host_stat.st_blocks);
	target_stat->st_atim = Host2BigEndian((int32_t) host_stat.st_atimespec.tv_sec);
	target_stat->st_mtim = Host2BigEndian((int32_t) host_stat.st_mtimespec.tv_sec);
	target_stat->st_ctim = Host2BigEndian((int32_t) host_stat.st_ctimespec.tv_sec);
#endif

#endif
	return ret;
}

template <class MEMORY_ADDR>
int AVR32_T2H_Syscalls<MEMORY_ADDR>::Stat(const char *path, struct avr32_stat *target_stat)
{
	int ret;
	struct stat host_stat;
	ret = stat(path, &host_stat);
	if(ret < 0) return ret;

#if defined(__x86_64) || defined(__amd64) || defined(__LP64__) || defined(_LP64) || defined(__amd64__)
	// 64-bit host
	target_stat->st_dev = Host2BigEndian((uint64_t) host_stat.st_dev);
	target_stat->st_ino = Host2BigEndian((uint64_t) host_stat.st_ino);
	target_stat->st_mode = Host2BigEndian((uint32_t) host_stat.st_mode);
	target_stat->st_nlink = Host2BigEndian((uint64_t) host_stat.st_nlink);
	target_stat->st_uid = Host2BigEndian((uint32_t) host_stat.st_uid);
	target_stat->st_gid = Host2BigEndian((uint32_t) host_stat.st_gid);
	target_stat->st_rdev = Host2BigEndian((int64_t) host_stat.st_rdev);
	target_stat->st_size = Host2BigEndian((int64_t) host_stat.st_size);
#if defined(WIN64) // Windows x64
	target_stat->st_blksize = Host2BigEndian((int32_t) 512);
	target_stat->st_blocks = Host2BigEndian((int64_t)((host_stat.st_size + 511) / 512));
	target_stat->st_atim = Host2BigEndian((int64_t) host_stat.st_atim);
	target_stat->st_mtim = Host2BigEndian((int64_t) host_stat.st_mtim);
	target_stat->st_ctim = Host2BigEndian((int64_t) host_stat.st_ctim);
#elif defined(linux) // Linux x64
	target_stat->st_blksize = Host2BigEndian((int64_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2BigEndian((int64_t) host_stat.st_blocks);
	target_stat->st_atim = Host2BigEndian((int64_t) host_stat.st_atim.tv_sec);
	target_stat->st_mtim = Host2BigEndian((int64_t) host_stat.st_mtim.tv_sec);
	target_stat->st_ctim = Host2BigEndian((int64_t) host_stat.st_ctim.tv_sec);
#elif defined(__APPLE_CC__) // darwin PPC64/x86_64
	target_stat->st_blksize = Host2BigEndian((int64_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2BigEndian((int64_t) host_stat.st_blocks);
	target_stat->st_atim = Host2BigEndian((int64_t) host_stat.st_atimespec.tv_sec);
	target_stat->st_mtim = Host2BigEndian((int64_t) host_stat.st_mtimespec.tv_sec);
	target_stat->st_ctim = Host2BigEndian((int64_t) host_stat.st_ctimespec.tv_sec);
#endif

#else
	// 32-bit host
	target_stat->st_dev = Host2BigEndian((uint64_t) host_stat.st_dev);
	target_stat->st_ino = Host2BigEndian((uint32_t) host_stat.st_ino);
	target_stat->st_mode = Host2BigEndian((uint32_t) host_stat.st_mode);
	target_stat->st_nlink = Host2BigEndian((uint32_t) host_stat.st_nlink);
	target_stat->st_uid = Host2BigEndian((uint32_t) host_stat.st_uid);
	target_stat->st_gid = Host2BigEndian((uint32_t) host_stat.st_gid);
	target_stat->st_rdev = Host2BigEndian((int64_t) host_stat.st_rdev);
	target_stat->st_size = Host2BigEndian((int64_t) host_stat.st_size);
#if defined(WIN32) // Windows 32
	target_stat->st_blksize = Host2BigEndian((int32_t) 512);
	target_stat->st_blocks = Host2BigEndian((int64_t)((host_stat.st_size + 511) / 512));
	target_stat->st_atim = Host2BigEndian((int32_t) host_stat.st_atime);
	target_stat->st_mtim = Host2BigEndian((int32_t) host_stat.st_mtime);
	target_stat->st_ctim = Host2BigEndian((int32_t) host_stat.st_ctime);
#elif defined(linux) // Linux 32
	target_stat->st_blksize = Host2BigEndian((int32_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2BigEndian((int64_t) host_stat.st_blocks);
	target_stat->st_atim = Host2BigEndian((int32_t) host_stat.st_atim.tv_sec);
	target_stat->st_mtim = Host2BigEndian((int32_t) host_stat.st_mtim.tv_sec);
	target_stat->st_ctim = Host2BigEndian((int32_t) host_stat.st_ctim.tv_sec);
#elif defined(__APPLE_CC__) // Darwin PPC32/x86
	target_stat->st_blksize = Host2BigEndian((int32_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2BigEndian((int64_t) host_stat.st_blocks);
	target_stat->st_atim = Host2BigEndian((int32_t) host_stat.st_atimespec.tv_sec);
	target_stat->st_mtim = Host2BigEndian((int32_t) host_stat.st_mtimespec.tv_sec);
	target_stat->st_ctim = Host2BigEndian((int32_t) host_stat.st_ctimespec.tv_sec);
#endif

#endif
	return ret;
}

template <class MEMORY_ADDR>
int AVR32_T2H_Syscalls<MEMORY_ADDR>::GetTimeOfDay(struct avr32_timeval *target_timeval, struct avr32_timezone *target_timezone)
{
	int ret;
	struct timeval host_tv;
	struct timezone host_tz;

	ret = gettimeofday(&host_tv, &host_tz);

	if(ret == 0)
	{
		if(target_timeval)
		{
			target_timeval->tv_sec = Host2BigEndian((int32_t) host_tv.tv_sec);
			target_timeval->tv_usec = Host2BigEndian((int32_t) host_tv.tv_usec);
		}
		if(target_timezone)
		{
			target_timezone->tz_minuteswest = Host2BigEndian((int32_t) host_tz.tz_minuteswest);
			target_timezone->tz_dsttime = Host2BigEndian((int32_t) host_tz.tz_dsttime);
		}
	}
	return ret;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::AVR32_T2H_Syscalls::Status AVR32_T2H_Syscalls<MEMORY_ADDR>::HandleEmulatorBreakpoint()
{
	// read PC
	uint32_t pc_value = 0;
	reg_pc->GetValue(&pc_value);

	// read a 6-byte pattern at PC
	uint8_t pattern[6];
	if(!memory_injection_import->InjectReadMemory(pc_value, pattern, 6))
	{
		return unisim::service::interfaces::AVR32_T2H_Syscalls::ERROR;
	}
	
	// try to recognize pattern breakpoint/mov r12,-1/mov r11,...
	if((pattern[0] != 0xd6) || (pattern[1] != 0x73) || // breakpoint
	   (pattern[2] != 0x3f) || (pattern[3] != 0xfc) || // mov r12,-1
	   ((pattern[4] & 0xf0) != 0x30) || ((pattern[5] & 0x0f) != 0x0b)) // mov r11,...
	{
		return unisim::service::interfaces::AVR32_T2H_Syscalls::UNHANDLED;
	}

	uint32_t syscall_num;
	reg_syscall_num->GetValue(&syscall_num);
	
	if((syscall_num >= T2H_SYSCALL_OPEN) && (syscall_num <= T2H_SYSCALL_INIT_ARGV))
	{
		if((this->*t2h_syscall_table[syscall_num])() == unisim::service::interfaces::AVR32_T2H_Syscalls::ERROR)
		{
			logger << DebugWarning << "System call #" << syscall_num << " failure" << EndDebugWarning;
			return unisim::service::interfaces::AVR32_T2H_Syscalls::ERROR;
		}
	}
	else
	{
		logger << DebugWarning << "System call #" << syscall_num << " does not exist" << EndDebugWarning;
		SetSystemCallStatus(-1);
		SetErrno(ENOSYS);
	}

	// skip 2 bytes of breakpoint instruction plus next 4 bytes
	pc_value = pc_value + 6;
	reg_pc->SetValue(&pc_value);
	
	return unisim::service::interfaces::AVR32_T2H_Syscalls::OK;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::AVR32_T2H_Syscalls::Status AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_open()
{
	MEMORY_ADDR addr;
	int pathnamelen;
	char *pathname;
	int flags;
	int ret;
	mode_t mode;

	addr = GetSystemCallParam(0);
	if(!StringLength(addr, pathnamelen)) return unisim::service::interfaces::AVR32_T2H_Syscalls::ERROR;
	pathname = (char *) malloc(pathnamelen + 1);
	
	if(memory_injection_import->InjectReadMemory(addr, pathname, pathnamelen + 1))
	{
		free(pathname);
		return unisim::service::interfaces::AVR32_T2H_Syscalls::ERROR;
	}
	
	flags = GetSystemCallParam(1);
	mode = GetSystemCallParam(2);
	if(((strncmp(pathname, "/dev", 4) == 0) && ((pathname[4] == 0) || (pathname[4] == '/'))) ||
		((strncmp(pathname, "/proc", 5) == 0) && ((pathname[5] == 0) || (pathname[5] == '/'))) ||
		((strncmp(pathname, "/sys", 4) == 0) && ((pathname[4] == 0) || (pathname[4] == '/'))) ||
		((strncmp(pathname, "/var", 4) == 0) && ((pathname[4] == 0) || (pathname[4] == '/'))))
	{
		// deny access to /dev, /proc, /sys, /var
		ret = -EACCES;
	}
	else
	{
		int host_flags = 0;
		int host_mode = 0;
		// open flags encoding may differ between target and host
		if((flags & T2H_O_ACCMODE) == T2H_O_RDONLY) host_flags = (host_flags & ~O_ACCMODE) | O_RDONLY;
		if((flags & T2H_O_ACCMODE) == T2H_O_WRONLY) host_flags = (host_flags & ~O_ACCMODE) | O_WRONLY;
		if((flags & T2H_O_ACCMODE) == T2H_O_RDWR) host_flags = (host_flags & ~O_ACCMODE) | O_RDWR;
		if(flags & T2H_O_CREAT) host_flags |= O_CREAT;
		if(flags & T2H_O_EXCL) host_flags |= O_EXCL;
		if(flags & T2H_O_TRUNC) host_flags |= O_TRUNC;
		if(flags & T2H_O_APPEND) host_flags |= O_APPEND;
#if defined(WIN32) || defined(WIN64)
		host_flags |= O_BINARY; // target opens file as binary files
		host_mode = mode & S_IRWXU; // Windows doesn't have bits for group and others
#else
		if(flags & T2H_O_SYNC) host_flags |= O_SYNC;
		if(flags & T2H_O_NONBLOCK) host_flags |= O_NONBLOCK;
		if(flags & T2H_O_NOCTTY) host_flags |= O_NOCTTY;
		host_mode = mode; // other UNIX systems should have the same bit encoding for protection
#endif
		ret = open(pathname, host_flags, host_mode);
	}
	if(unlikely(verbose_syscalls))
	{
		logger << DebugInfo
			<< "open(pathname=\"" << pathname << "\", flags=0x" << std::hex << flags
			<< ", mode=0x" << mode << std::dec << ") return " << ret
			<< EndDebugInfo;
	}

	free(pathname);
	SetSystemCallStatus(ret);
	if(ret < 0) SetErrno(errno);
	
	return unisim::service::interfaces::AVR32_T2H_Syscalls::OK;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::AVR32_T2H_Syscalls::Status AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_close()
{
	int fd;
	int ret;

	fd = GetSystemCallParam(0);

	switch(fd)
	{
		case 0:
		case 1:
		case 2:
			ret = 0;
			logger << DebugWarning << "attempt to close ";
			switch(fd)
			{
				case 0: logger << "standard input"; break;
				case 1: logger << "standard output"; break;
				case 2: logger << "standard error output"; break;
			}
			logger << EndDebugWarning;
			break;
		default:
			ret = close(fd);
			break;
	}
	if(unlikely(verbose_syscalls))
	{
		logger << DebugInfo << "close(fd=" << fd << ") return " << ret << EndDebugInfo;
	}
	SetSystemCallStatus(ret);
	if(ret < 0) SetErrno(errno);
	return unisim::service::interfaces::AVR32_T2H_Syscalls::OK;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::AVR32_T2H_Syscalls::Status AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_read()
{
	int fd;
	size_t count;
	MEMORY_ADDR buf_addr;
	void *buf;
	size_t ret;

	fd = GetSystemCallParam(0);
	buf_addr = (MEMORY_ADDR) GetSystemCallParam(1);
	count = (size_t) GetSystemCallParam(2);

	buf = malloc(count);

	if(buf)
	{
		ret = read(fd, buf, count);
		if(ret > 0)
		{
			if(!memory_injection_import->InjectWriteMemory(buf_addr, buf, ret))
			{
				free(buf);
				return unisim::service::interfaces::AVR32_T2H_Syscalls::ERROR;
			}
		}
		free(buf);
	}
	else
	{
		ret = (size_t) -1;
	}

	if(unlikely(verbose_syscalls))
	logger << DebugInfo << "read(fd=" << fd << ", buf=0x" << std::hex << buf_addr << std::dec
		<< ", count=" << count << ") return " << ret << EndDebugInfo;

	SetSystemCallStatus(ret);
	if(ret < 0) SetErrno(errno);
	return unisim::service::interfaces::AVR32_T2H_Syscalls::OK;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::AVR32_T2H_Syscalls::Status AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_write()
{
	int fd;
	size_t count;
	void *buf;
	MEMORY_ADDR buf_addr;
	size_t ret;

	fd = GetSystemCallParam(0);
	buf_addr = GetSystemCallParam(1);
	count = (size_t)GetSystemCallParam(2);
	buf = malloc(count);

	ret = (size_t) -1;

	if(buf)
	{
		if(!memory_injection_import->InjectReadMemory(buf_addr, buf, count))
		{
			free(buf);
			return unisim::service::interfaces::AVR32_T2H_Syscalls::ERROR;
		}
		ret = write(fd, buf, count);
		if(unlikely(verbose_syscalls))
		{
			logger << DebugInfo
			<< "write(fd=" << fd << ", buf=0x" << std::hex << buf_addr << std::dec
			<< ", count=" << count << ") return " << ret << std::endl
			<< "buffer =";
			for (size_t i = 0; i < count; i++)
			{
				logger << " " << std::hex << (unsigned int)((uint8_t *) buf)[i] << std::dec;
			}
			logger << EndDebugInfo;
		}
		free(buf);
	}
	else
	{
		ret = (size_t) -1;
	}

	SetSystemCallStatus(ret);
	if(ret < 0) SetErrno(errno);
	return unisim::service::interfaces::AVR32_T2H_Syscalls::OK;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::AVR32_T2H_Syscalls::Status AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_lseek()
{
	int fildes;
	off_t offset;
	int whence;
	off_t ret;

	fildes = GetSystemCallParam(0);
	offset = GetSystemCallParam(1);
	whence = GetSystemCallParam(2);
	ret = lseek(fildes, offset, whence);
	if(unlikely(verbose_syscalls))
	{
		logger << DebugInfo << "lseek(fildes=" << fildes << ", offset=" << offset
			<< ", whence=" << whence << ") return " << ret << EndDebugInfo;
	}

	if(ret == (off_t) -1)
	{
		SetSystemCallStatus(-1);
		SetErrno(errno);
	}
	else
	{
		SetSystemCallStatus(ret);
	}
	return unisim::service::interfaces::AVR32_T2H_Syscalls::OK;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::AVR32_T2H_Syscalls::Status AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_rename()
{
	unisim::service::interfaces::AVR32_T2H_Syscalls::Status status = unisim::service::interfaces::AVR32_T2H_Syscalls::OK;
	MEMORY_ADDR oldpathaddr;
	MEMORY_ADDR newpathaddr;
	int oldpathlen;
	int newpathlen;
	char *oldpath;
	char *newpath;
	int ret;

	oldpathaddr = GetSystemCallParam(0);
	if(!StringLength(oldpathaddr, oldpathlen)) return unisim::service::interfaces::AVR32_T2H_Syscalls::ERROR;
	
	newpathaddr = GetSystemCallParam(1);
	if(!StringLength(newpathaddr, newpathlen)) return unisim::service::interfaces::AVR32_T2H_Syscalls::ERROR;
	
	oldpath = (char *) malloc(oldpathlen + 1);
	if(memory_injection_import->InjectReadMemory(oldpathaddr, oldpath, oldpathlen + 1))
	{
		newpath = (char *) malloc(newpathlen + 1);
		if(memory_injection_import->InjectReadMemory(newpathaddr, newpath, newpathlen + 1))
		{
			ret = rename(oldpath, newpath);
			
			if(unlikely(verbose_syscalls))
			{
				logger << DebugInfo
					<< "rename(oldpath=\"" << oldpath << "\", newpath=\"" << newpath << "\") return " << ret << std::endl
					<< EndDebugInfo;
			}
		}
		else
		{
			status = unisim::service::interfaces::AVR32_T2H_Syscalls::ERROR;
		}
		free(newpath);
	}
	else
	{
		status = unisim::service::interfaces::AVR32_T2H_Syscalls::ERROR;
	}
	free(oldpath);
	
	SetSystemCallStatus(ret);
	if(ret < 0) SetErrno(errno);
	return status;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::AVR32_T2H_Syscalls::Status AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_unlink()
{
	MEMORY_ADDR pathnameaddr;
	int pathnamelen;
	char *pathname;
	int ret;

	pathnameaddr = GetSystemCallParam(0);
	if(!StringLength(pathnameaddr, pathnamelen)) return unisim::service::interfaces::AVR32_T2H_Syscalls::ERROR;
	pathname = (char *) malloc(pathnamelen + 1);
	if(!memory_injection_import->InjectReadMemory(pathnameaddr, pathname, pathnamelen + 1))
	{
		free(pathname);
		return unisim::service::interfaces::AVR32_T2H_Syscalls::ERROR;
	}
	ret = unlink(pathname);
	if(unlikely(verbose_syscalls))
	logger << DebugInfo
		<< "unlink(pathname=\"" << pathname << "\") return " << ret << std::endl
		<< EndDebugInfo;

	free(pathname);
	SetSystemCallStatus(ret);
	if(ret < 0) SetErrno(errno);
	return unisim::service::interfaces::AVR32_T2H_Syscalls::OK;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::AVR32_T2H_Syscalls::Status AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_stat()
{
	MEMORY_ADDR addr;
	int pathnamelen;
	char *pathname;
	MEMORY_ADDR buf_address;
	int ret;

	addr = GetSystemCallParam(0);
	if(!StringLength(addr, pathnamelen)) return unisim::service::interfaces::AVR32_T2H_Syscalls::ERROR;
	pathname = (char *) malloc(pathnamelen + 1);
	if(!memory_injection_import->InjectReadMemory(addr, pathname, pathnamelen + 1))
	{
		free(pathname);
		return unisim::service::interfaces::AVR32_T2H_Syscalls::ERROR;
	}
	buf_address = GetSystemCallParam(1);
	struct avr32_stat target_stat;
	ret = Stat(pathname, &target_stat);
	if(ret == 0) memory_injection_import->InjectWriteMemory(buf_address, &target_stat, sizeof(target_stat));

	if(unlikely(verbose_syscalls))
	logger << DebugInfo
		<< "stat(pathname=\"" << pathname << "\""
		<< ", buf_addr=0x" << std::hex << buf_address << std::dec
		<< ") return " << ret << EndDebugInfo;

	free(pathname);
	SetSystemCallStatus(ret);
	if(ret < 0) SetErrno(errno);
	return unisim::service::interfaces::AVR32_T2H_Syscalls::OK;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::AVR32_T2H_Syscalls::Status AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_fstat()
{
	int ret;
	int fd;
	MEMORY_ADDR buf_address;

	fd = GetSystemCallParam(0);
	buf_address = GetSystemCallParam(1);
	struct avr32_stat target_stat;
	ret = Fstat(fd, &target_stat);
	if(ret == 0)
	{
		if(!memory_injection_import->InjectWriteMemory(buf_address, &target_stat, sizeof(target_stat))) return unisim::service::interfaces::AVR32_T2H_Syscalls::ERROR;
	}

	if(unlikely(verbose_syscalls))
	logger << DebugInfo
		<< "fstat(fd=" << fd
		<< ", buf_addr=0x" << std::hex << buf_address << std::dec
		<< ") return " << ret << EndDebugInfo;

	SetSystemCallStatus(ret);
	if(ret < 0) SetErrno(errno);
	return unisim::service::interfaces::AVR32_T2H_Syscalls::OK;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::AVR32_T2H_Syscalls::Status AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_gettimeofday()
{
	int ret = -1;
	MEMORY_ADDR tv_addr;
	MEMORY_ADDR tz_addr;
	tv_addr = GetSystemCallParam(0);
	tz_addr = GetSystemCallParam(1);

	struct avr32_timeval target_tv;
	struct avr32_timezone target_tz;

	ret = GetTimeOfDay(tv_addr ? &target_tv : 0, tz_addr ? &target_tz : 0);

	if(ret == 0)
	{
		if(tv_addr)
		{
			if(!memory_injection_import->InjectWriteMemory(tv_addr, &target_tv, sizeof(target_tv))) return unisim::service::interfaces::AVR32_T2H_Syscalls::ERROR;
		}
		if(tz_addr)
		{
			if(!memory_injection_import->InjectWriteMemory(tz_addr, &target_tz, sizeof(target_tz))) return unisim::service::interfaces::AVR32_T2H_Syscalls::ERROR;
		}
	}

	if(unlikely(verbose_syscalls))
	logger << DebugInfo
		<< "gettimeofday(tv = 0x" << std::hex << tv_addr << std::dec
		<< ", tz = 0x" << std::hex << tz_addr << std::dec << ") return " << ret
		<< EndDebugInfo;

	SetSystemCallStatus(ret);
	if(ret < 0) SetErrno(errno);
	return unisim::service::interfaces::AVR32_T2H_Syscalls::OK;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::AVR32_T2H_Syscalls::Status AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_isatty()
{
	int fd = GetSystemCallParam(0);
	int ret;

	ret = isatty(fd);
	
	if(unlikely(verbose_syscalls))
	logger << DebugInfo
		<< "isatty() return " << ret
		<< EndDebugInfo;

	SetSystemCallStatus(ret);
	if(ret < 0) SetErrno(errno);
	return unisim::service::interfaces::AVR32_T2H_Syscalls::OK;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::AVR32_T2H_Syscalls::Status AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_system()
{
	MEMORY_ADDR addr;
	int command_len;
	char *command;
	int ret;

	addr = GetSystemCallParam(0);
	if(!StringLength(addr, command_len)) return unisim::service::interfaces::AVR32_T2H_Syscalls::ERROR;
	command = (char *) malloc(command_len + 1);
	if(!memory_injection_import->InjectReadMemory(addr, command, command_len + 1))
	{
		free(command);
		return unisim::service::interfaces::AVR32_T2H_Syscalls::ERROR;
	}
	
	ret = system(command);
	
	if(unlikely(verbose_syscalls))
	{
		logger << DebugInfo
			<< "system(command=\"" << command << "\") return " << ret
			<< EndDebugInfo;
	}

	free(command);
	SetSystemCallStatus(ret);
	if(ret < 0) SetErrno(errno);
	return unisim::service::interfaces::AVR32_T2H_Syscalls::OK;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::AVR32_T2H_Syscalls::Status AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_exit()
{
	int status;

	status = GetSystemCallParam(0);
	
	if(unlikely(verbose_syscalls))
	{
		logger << DebugInfo
			<< "exit(status=" << status << EndDebugInfo;
	}
	
	terminated = true;
	exit_status = status;

	SetSystemCallStatus(0);
	
	Object::Stop(exit_status);
	return unisim::service::interfaces::AVR32_T2H_Syscalls::OK;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::AVR32_T2H_Syscalls::Status AVR32_T2H_Syscalls<MEMORY_ADDR>::t2h_syscall_init_argv()
{
	// Initialize argv, use stack for keeping arguments. 
	// _init_argv() returns argc in r12 and argv in r11
	// and the total size used for the arguments in r10.
	// Signal that we are storing the arguments in a stackwise top down approach (i.e. r11=0). */	
	// If initialization of argv is not handled then _init_argv
        // returns -1 so set argc to 0 and make sure no space is 
	// allocated on the stack. */

	MEMORY_ADDR sp = GetSystemCallParam(0); // high address (next byte address) of argv
	MEMORY_ADDR high_addr = sp;
	
	std::vector<MEMORY_ADDR> argv_ptr;
	int i; // this is signed for a good reason
	int n = argv.size();
	for(i = 0; i < n; i++)
	{
		const std::string& arg = *argv[i];
		sp -= arg.length() + 1;
		sp = sp & ~3; // align to a 32-bit boundary
		
		// Write argument string into memory
		if(!memory_injection_import->InjectWriteMemory(sp, arg.c_str(), arg.length() + 1)) return unisim::service::interfaces::AVR32_T2H_Syscalls::ERROR;
		
		argv_ptr.push_back(sp); // keep string pointer
	}

	argv_ptr.push_back(0);

	for(i = n; i >= 0; i--) // n elements + null terminal pointer
	{
		MEMORY_ADDR arg_addr = Host2BigEndian(argv_ptr[i]);
		sp -= 4;
		
		// Write pointer to argument string
		if(!memory_injection_import->InjectWriteMemory(sp, &arg_addr, 4)) return unisim::service::interfaces::AVR32_T2H_Syscalls::ERROR;
	}

	MEMORY_ADDR low_addr = sp;
	
	SetSystemCallParam(0, argc);                 // r12 = argc
	SetSystemCallParam(1, sp);                   // r11 = argv
	SetSystemCallParam(2, high_addr - low_addr); // r10 = byte size of argv + argc on the stack

	if(unlikely(verbose_syscalls))
	{
		logger << DebugInfo << "init_argv(): argc=" << argc << ", argv=0x" << std::hex << sp << std::dec << ", byte size of argv=" << high_addr - low_addr << EndDebugInfo;
	}

	return unisim::service::interfaces::AVR32_T2H_Syscalls::OK;
}

} // end of namespace avr32_t2h_syscalls
} // end of namespace os
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_OS_AVR32_T2H_SYSCALLS_AVR32_T2H_SYSCALLS_TCC__
