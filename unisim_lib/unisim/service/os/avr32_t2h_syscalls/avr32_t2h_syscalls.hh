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
 
#ifndef __UNISIM_SERVICE_OS_AVR32_T2H_SYSCALLS_AVR32_T2H_SYSCALLS_HH__
#define __UNISIM_SERVICE_OS_AVR32_T2H_SYSCALLS_AVR32_T2H_SYSCALLS_HH__

#include <inttypes.h>
#include <map>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <fstream>
#include <sstream>

#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/service/interfaces/avr32_t2h_syscalls.hh"
#include "unisim/service/interfaces/memory.hh"
#include "unisim/service/interfaces/memory_injection.hh"
#include "unisim/service/interfaces/symbol_table_lookup.hh"
#include "unisim/service/interfaces/registers.hh"
#include "unisim/service/interfaces/blob.hh"

#if defined(__GNUC__) && (__GNUC__ >= 3)
#define PACKED __attribute__ ((packed))
#else
#define PACKED
#endif

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
using unisim::kernel::service::Service;
using unisim::kernel::service::Client;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::Parameter;
using unisim::kernel::logger::Logger;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::MemoryInjection;
using unisim::service::interfaces::SymbolTableLookup;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::Blob;

struct avr32_stat
{
	uint32_t st_dev;      /* device */
	uint32_t st_ino;      /* inode */
	int32_t  st_mode;     /* protection */
	uint32_t st_nlink;    /* number of hard links */
	uint32_t st_uid;      /* user ID of owner */
	uint32_t st_gid;      /* group ID of owner */
	uint32_t st_rdev;     /* device type (if inode device) */
	uint64_t st_size;     /* total size, in bytes */
	uint64_t st_blksize;  /* blocksize for filesystem I/O */
	uint64_t st_blocks;   /* number of blocks allocated */
	int32_t  st_atim;     /* time of last access */
	int32_t  st_mtim;     /* time of last modification */
	int32_t  st_ctim;     /* time of last change */
} PACKED;

struct avr32_timeval
{
	int32_t tv_sec;
	int64_t tv_usec;
} PACKED;

struct avr32_timezone
{
  int32_t tz_minuteswest;
  int32_t tz_dsttime;
};

template <class MEMORY_ADDR>
class AVR32_T2H_Syscalls :
	public Service<unisim::service::interfaces::AVR32_T2H_Syscalls>,
	public Client<MemoryInjection<MEMORY_ADDR> >,
	public Client<Registers>
{
public:
	ServiceExport<unisim::service::interfaces::AVR32_T2H_Syscalls> avr32_t2h_syscalls_export;

	ServiceImport<MemoryInjection<MEMORY_ADDR> > memory_injection_import;
	ServiceImport<Registers> registers_import;

    AVR32_T2H_Syscalls(const char *name, Object *parent = 0);
    virtual ~AVR32_T2H_Syscalls();

    virtual void OnDisconnect();
	virtual bool EndSetup();

    virtual unisim::service::interfaces::AVR32_T2H_Syscalls::Status HandleEmulatorBreakpoint();
private:
	// AVR32's target to host syscall numbers
	static const uint32_t T2H_SYSCALL_OPEN = 1;
	static const uint32_t T2H_SYSCALL_CLOSE = 2;
	static const uint32_t T2H_SYSCALL_READ = 3;
	static const uint32_t T2H_SYSCALL_WRITE = 4;
	static const uint32_t T2H_SYSCALL_LSEEK = 5;
	static const uint32_t T2H_SYSCALL_RENAME = 6;
	static const uint32_t T2H_SYSCALL_UNLINK = 7;
	static const uint32_t T2H_SYSCALL_STAT = 8;
	static const uint32_t T2H_SYSCALL_FSTAT = 9;
	static const uint32_t T2H_SYSCALL_GETTIMEOFDAY = 10;
	static const uint32_t T2H_SYSCALL_ISATTY = 11;
	static const uint32_t T2H_SYSCALL_SYSTEM = 12;
	static const uint32_t T2H_SYSCALL_EXIT = 13;
	static const uint32_t T2H_SYSCALL_INIT_ARGV = 14;

	static const uint32_t NUM_SYSCALLS = T2H_SYSCALL_INIT_ARGV + 1;
	// the kernel logger
	unisim::kernel::logger::Logger logger;

	// register access interfaces
	unisim::util::debug::Register *reg_sp;
	unisim::util::debug::Register *reg_syscall_num;
	unisim::util::debug::Register *reg_params[5];
	unisim::util::debug::Register *reg_return_status;

	// system call table
	unisim::service::interfaces::AVR32_T2H_Syscalls::Status (unisim::service::os::avr32_t2h_syscalls::AVR32_T2H_Syscalls<MEMORY_ADDR>::*t2h_syscall_table[NUM_SYSCALLS])();

	// program termination and return status
	bool terminated;
	int exit_status;

	// register names
	std::string sp_register_name;
	std::string syscall_num_register_name;
	std::string return_status_register_name;
	std::string param_register_names[5];
	
	// program arguments
	int argc;
	std::vector<std::string> argv;

	bool verbose_all;
	bool verbose_syscalls;
	bool verbose_setup;
	Parameter<bool> param_verbose_all;
	Parameter<bool> param_verbose_syscalls;
	Parameter<bool> param_verbose_setup;
	Parameter<int> param_argc;
	std::vector<Parameter<std::string> *> param_argv;

	const char *GetSyscallFriendlyName(uint32_t syscall_num);

	uint32_t GetSystemCallParam(unsigned int arg_num);
	void SetSystemCallParam(unsigned int arg_num, uint32_t value);
	bool StringLength(MEMORY_ADDR addr, int &string_length);
	int Fstat(int fd, struct avr32_stat *target_stat);
	int Stat(const char *path, struct avr32_stat *target_stat);
	int GetTimeOfDay(struct avr32_timeval *target_timeval, struct avr32_timezone *target_timezone);
	void SetSystemCallStatus(int32_t ret);
	
	unisim::service::interfaces::AVR32_T2H_Syscalls::Status t2h_syscall_open();
	unisim::service::interfaces::AVR32_T2H_Syscalls::Status t2h_syscall_close();
	unisim::service::interfaces::AVR32_T2H_Syscalls::Status t2h_syscall_read();
	unisim::service::interfaces::AVR32_T2H_Syscalls::Status t2h_syscall_write();
	unisim::service::interfaces::AVR32_T2H_Syscalls::Status t2h_syscall_lseek();
	unisim::service::interfaces::AVR32_T2H_Syscalls::Status t2h_syscall_rename();
	unisim::service::interfaces::AVR32_T2H_Syscalls::Status t2h_syscall_unlink();
	unisim::service::interfaces::AVR32_T2H_Syscalls::Status t2h_syscall_stat();
	unisim::service::interfaces::AVR32_T2H_Syscalls::Status t2h_syscall_fstat();
	unisim::service::interfaces::AVR32_T2H_Syscalls::Status t2h_syscall_gettimeofday();
	unisim::service::interfaces::AVR32_T2H_Syscalls::Status t2h_syscall_isatty();
	unisim::service::interfaces::AVR32_T2H_Syscalls::Status t2h_syscall_system();
	unisim::service::interfaces::AVR32_T2H_Syscalls::Status t2h_syscall_exit();
	unisim::service::interfaces::AVR32_T2H_Syscalls::Status t2h_syscall_init_argv();
};

} // end of namespace ti_c_io
} // end of namespace os
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_OS_AVR32_T2H_SYSCALLS_HH__
