/*
 *  Copyright (c) 2007,
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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#ifndef __UNISIM_SERVICE_OS_LINUX_OS_LINUX_OS_TCC__
#define __UNISIM_SERVICE_OS_LINUX_OS_LINUX_OS_TCC__

#include <map>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <fstream>
#include <sstream>
#include <iostream>

#ifdef WIN32
#include <process.h>
#else
#include <sys/times.h>
#endif

#include "unisim/kernel/service/service.hh"
#include "unisim/service/interfaces/linux_os.hh"
#include "unisim/service/interfaces/cpu_linux_os.hh"
#include "unisim/service/interfaces/loader.hh"
#include "unisim/service/interfaces/memory.hh"
#include "unisim/service/interfaces/memory_injection.hh"
#include "unisim/service/interfaces/registers.hh"
#include "unisim/service/interfaces/logger.hh"
#include "unisim/service/os/linux_os/linux_os_exception.hh"
#include "unisim/util/endian/endian.hh"
#include "unisim/util/debug/register.hh"

#define LOCATION Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__

namespace unisim {
namespace service {
namespace os {
namespace linux_os {

using std::runtime_error;
using std::string;
using std::stringstream;
using std::hex;
using std::dec;
using std::endl;
using std::flush;
using std::cout;
using std::cerr;
using unisim::kernel::service::Service;
using unisim::kernel::service::Client;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::Parameter;
using unisim::service::interfaces::Loader;
using unisim::service::interfaces::Memory;
using unisim::service::interfaces::MemoryInjection;
using unisim::service::interfaces::Registers;
using unisim::service::interfaces::Logger;
//using unisim::service::interfaces::operator<<;
using unisim::service::interfaces::DebugInfo;
using unisim::service::interfaces::EndDebugInfo;
using unisim::service::interfaces::DebugWarning;
using unisim::service::interfaces::EndDebugWarning;
using unisim::service::interfaces::DebugError;
using unisim::service::interfaces::EndDebugError;
using unisim::service::interfaces::Hex;
using unisim::service::interfaces::Dec;
using unisim::service::interfaces::Function;
using unisim::service::interfaces::File;
using unisim::service::interfaces::Line;
using unisim::service::interfaces::Endl;
using unisim::util::endian::endian_type;
using unisim::util::debug::Register;
using unisim::util::endian::E_BIG_ENDIAN;
using unisim::util::endian::E_LITTLE_ENDIAN;
using unisim::util::endian::Host2BigEndian;
using unisim::util::endian::Host2LittleEndian;
using unisim::util::endian::Host2Target;

/** Constructor. */
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LinuxOS(const char *name, Object *parent) :
    Object(name, parent),
	Service<unisim::service::interfaces::LinuxOS>(name, parent),
	Client<CPULinuxOS>(name, parent),
	Client<Memory<ADDRESS_TYPE> >(name, parent),
	Client<MemoryInjection<ADDRESS_TYPE> >(name, parent),
	Client<Registers>(name, parent),
	Client<Loader<ADDRESS_TYPE> >(name, parent),
	Client<Logger>(name, parent),
	linux_os_export("linux-os-export", this),
	memory_import("memory-import", this),
	memory_injection_import("memory-injection-import", this),
	registers_import("registers-import", this),
	logger_import("logger-import", this),
	loader_import("loader-import", this),
	cpu_linux_os_import("cpu-linux-os-import", this),
	system(""),
	param_system("system", this, system),
    endianess(E_LITTLE_ENDIAN),
    param_endian("endianess", this, endianess),
    verbose(false),
    param_verbose("verbose", this, verbose),
	memory_page_size(4096),
	param_memory_page_size("memory-page-size", this, memory_page_size),
	mmap_base(0xd4000000),
	current_syscall_id(0),
    current_syscall_name(""),
	osrelease_filename("/proc/sys/kernel/osrelease"),
	fake_osrelease_filename("osrelease"),
	fake_osrelease("2.6.8")
	
{
	SetSyscallNameMap();

	Object::SetupDependsOn(registers_import);
	Object::SetupDependsOn(loader_import);
	Object::SetupDependsOn(logger_import);
 }

/** Destructor. */
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
~LinuxOS() {
}

/** Method to execute when the LinuxOS is disconnected from its client. */
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
OnDisconnect() {
	if(logger_import)
		(*logger_import) << DebugWarning << LOCATION 
			<< LOCATION
			<< "TODO"
			<< Endl << EndDebugWarning;
}

/** Method to setup the service */
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
Setup() {
	if(!cpu_linux_os_import) {
		if(logger_import) {
			(*logger_import) << DebugError << LOCATION
				<< cpu_linux_os_import.GetName() << " is not connected" << Endl
				<< EndDebugError;
		}
		return false;
	}
	if(!memory_import) {
		if(logger_import) {
			(*logger_import) << DebugError << LOCATION
				<< memory_import.GetName() << " is not connected" << Endl
				<< EndDebugError;
		}
		return false;
	}
	if(!memory_injection_import) {
		if(logger_import) {
			(*logger_import) << DebugError << LOCATION
				<< memory_injection_import.GetName() << " is not connected" << Endl
				<< EndDebugError;
		}
		return false;
	}
	
	if(!registers_import) {
		if(logger_import) {
			(*logger_import) << DebugError << LOCATION
				<< registers_import.GetName() << " is not connected" << Endl
				<< EndDebugError;
		}
		return false;
	}
	
	// check that the given system is supported
	if(system != "arm" && system != "powerpc") {
		if(logger_import)
			(*logger_import) << DebugError << LOCATION
				<< "Unsupported system (" << system << "), this service only supports"
				<< " arm and ppc systems" << Endl
				<< EndDebugError;
		return false;
	}
	// Call the system dependent setup
	if(system == "arm") {
		if(!ARMSetup()) return false;
	}
	if(system == "powerpc") {
		if(!PPCSetup()) return false;
	}
	if(logger_import) 
		(*logger_import) << DebugInfo << LOCATION
			<< "Setup finished with success"
			<< Endl << EndDebugInfo;
	return true;
}

/**
 * Linux ARM dependent setup
 * 
 * @return true if setup succeeds
 */
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
ARMSetup() {
	// Set the system calls mappings
	SetSyscallId(string("exit"), 1);
	SetSyscallId(string("read"), 3);
	SetSyscallId(string("write"), 4);
	SetSyscallId(string("open"), 5);
	SetSyscallId(string("close"), 6);
	SetSyscallId(string("unlink"), 10);
	SetSyscallId(string("time"), 13);
	SetSyscallId(string("lseek"), 19);
	SetSyscallId(string("getpid"), 20);
	SetSyscallId(string("getuid"), 24);
	SetSyscallId(string("access"), 33);
	SetSyscallId(string("kill"), 37);
	SetSyscallId(string("rename"), 38);
	SetSyscallId(string("times"), 43);
	SetSyscallId(string("brk"), 45);
	SetSyscallId(string("getgid"), 47);
	SetSyscallId(string("geteuid"), 49);
	SetSyscallId(string("getegid"), 50);
	SetSyscallId(string("ioctl"), 54);
	SetSyscallId(string("setrlimit"), 75);
	SetSyscallId(string("getrusage"), 77);
	SetSyscallId(string("munmap"), 91);
	SetSyscallId(string("socketcall"), 102);
	SetSyscallId(string("stat"), 106);
	SetSyscallId(string("fstat"), 108);
	SetSyscallId(string("uname"), 122);
	SetSyscallId(string("llseek"), 140);
	SetSyscallId(string("writev"), 146);
	SetSyscallId(string("rt_sigaction"), 174);
	SetSyscallId(string("rt_sigprocmask"), 175);
	SetSyscallId(string("ugetrlimit"), 191);
	SetSyscallId(string("mmap2"), 192);
	SetSyscallId(string("fstat64"), 197);
	SetSyscallId(string("getuid32"), 199);
	SetSyscallId(string("getgid32"), 200);
	SetSyscallId(string("geteuid32"), 201);
	SetSyscallId(string("getegid32"), 202);
	SetSyscallId(string("flistxattr"), 234);
	SetSyscallId(string("exit_group"), 248);

	// Set mmap_brk_point and brk_point
	mmap_brk_point = mmap_base;
	brk_point = loader_import->GetTopAddr() +
    	(memory_page_size - (loader_import->GetTopAddr() % memory_page_size));

	for(unsigned int i = 0; i < 13; i++) {
		stringstream buf;
		buf << "r" << i;
		arm_regs[i] = registers_import->GetRegister(buf.str().c_str());
		if(!arm_regs[i]) {
			if(logger_import)
				(*logger_import) << DebugError << LOCATION
					<< "CPU has no register named " << buf.str()
					<< Endl << EndDebugError;
			return false;
		}
	}
	arm_regs[13] = registers_import->GetRegister("sp");
	if(!arm_regs[13]) {
		if(logger_import)
			(*logger_import) << DebugError << LOCATION
				<< "CPU has no register named sp"
				<< Endl << EndDebugError;
		return false;
	}
	arm_regs[14] = registers_import->GetRegister("lr");
	if(!arm_regs[14]) {
		if(logger_import)
			(*logger_import) << DebugError << LOCATION
				<< "CPU has no register named lr"
				<< Endl << EndDebugError;
		return false;
	}
	arm_regs[15] = registers_import->GetRegister("pc");
	if(!arm_regs[15]) {
		if(logger_import)
			(*logger_import) << DebugError << LOCATION
				<< "CPU has no register named pc"
				<< Endl << EndDebugError;
		return false;
	}
	for(unsigned int i = 0; i < 16; i++) {
		if(arm_regs[i]->GetSize() != sizeof(PARAMETER_TYPE)) {
			if(logger_import)
				(*logger_import) << DebugError << LOCATION
					<< "Unexpected register size for register " << i
					<< Endl << EndDebugError;
			return false;
		}
	}

	// Set initial CPU registers
	PARAMETER_TYPE pc = loader_import->GetEntryPoint();
	if(logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "Setting register \"" << arm_regs[15]->GetName() << "\""
			<< " to value 0x" << Hex << pc << Dec
			<< Endl << EndDebugInfo;
	arm_regs[15]->SetValue(&pc);
	PARAMETER_TYPE st = loader_import->GetStackBase();
	if(logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "Setting register \"" << arm_regs[13]->GetName() << "\""
			<< " to value 0x" << Hex << st << Dec
			<< Endl << EndDebugInfo;
	arm_regs[13]->SetValue(&st);
	
	return true;
}

/**
 * Linux PPC dependent setup
 * 
 * @return true if setup succeeds
 */
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
PPCSetup() {
    // Set the system calls mappings
    SetSyscallId(string("exit"), 1);
    SetSyscallId(string("read"), 3);
    SetSyscallId(string("write"), 4);
    SetSyscallId(string("open"), 5);
    SetSyscallId(string("close"), 6);
    SetSyscallId(string("unlink"), 10);
    SetSyscallId(string("time"), 13);
    SetSyscallId(string("lseek"), 19);
    SetSyscallId(string("getpid"), 20);
    SetSyscallId(string("getuid"), 24);
    SetSyscallId(string("access"), 33);
    SetSyscallId(string("kill"), 37);
    SetSyscallId(string("rename"), 38);
    SetSyscallId(string("times"), 43);
    SetSyscallId(string("brk"), 45);
    SetSyscallId(string("getgid"), 47);
    SetSyscallId(string("geteuid"), 49);
    SetSyscallId(string("getegid"), 50);
    SetSyscallId(string("ioctl"), 54);
    SetSyscallId(string("setrlimit"), 75);
    SetSyscallId(string("getrlimit"), 76);
    SetSyscallId(string("getrusage"), 77);
    SetSyscallId(string("mmap"), 90);
    SetSyscallId(string("munmap"), 91);
    SetSyscallId(string("socketcall"), 102);
    SetSyscallId(string("stat"), 106);
    SetSyscallId(string("fstat"), 108);
    SetSyscallId(string("uname"), 122);
    SetSyscallId(string("llseek"), 140);
    SetSyscallId(string("writev"), 146);
    SetSyscallId(string("rt_sigaction"), 173);
    SetSyscallId(string("rt_sigprocmask"), 174);
    SetSyscallId(string("ugetrlimit"), 190);
    SetSyscallId(string("mmap2"), 192);
    SetSyscallId(string("fstat64"), 197);
    SetSyscallId(string("fcntl64"), 204);
    SetSyscallId(string("flistxattr"), 217);
    SetSyscallId(string("exit_group"), 234);

    // Set mmap_brk_point and brk_point
    mmap_brk_point = mmap_base;
    brk_point = loader_import->GetTopAddr() +
	            (memory_page_size - (loader_import->GetTopAddr() % memory_page_size));

    ppc_cr = registers_import->GetRegister("cr");
    if(!ppc_cr) {
		if(logger_import) {
			(*logger_import) << DebugError;
			(*logger_import) << "CPU has no register named \"cr\"" << Endl;
			(*logger_import) << EndDebugError;
		}
		return false;
	}

    for(unsigned int i = 0; i < 31; i++) {
		stringstream buf;
		buf << "r" << i;
		ppc_regs[i] = registers_import->GetRegister(buf.str().c_str());
		if(!ppc_regs[i]) {
			if(logger_import) {
				(*logger_import) << DebugError;
				(*logger_import) << "CPU has no register named \"" << buf.str() << "\"" << Endl;
				(*logger_import) << EndDebugError;
			}
			return false;
		}
    }

    // Set initial CPU registers
    PARAMETER_TYPE pc = loader_import->GetEntryPoint();
    Register *ppc_cia = registers_import->GetRegister("cia");
    if(!ppc_cia) {
		if(logger_import) {
			(*logger_import) << DebugError;
			(*logger_import) << "CPU has no register named \"cia\"" << Endl;
			(*logger_import) << EndDebugError;
		}
		return false;
	}
    ppc_cia->SetValue(&pc);
    PARAMETER_TYPE st = loader_import->GetStackBase();
    ppc_regs[1]->SetValue(&st);

    return true;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
ReadMem(ADDRESS_TYPE addr, void *buffer, uint32_t size) {
	memory_injection_import->InjectReadMemory(addr, buffer, size);
	if(verbose && logger_import) {
		(*logger_import) << DebugInfo << LOCATION
			<< "OS read memory: " << Endl
			<< " - addr = 0x" << Hex << addr << Dec << Endl
			<< " - size = " << size << Endl
			<< " - data =" << Hex;
		for(unsigned int i = 0; i < size; i++) {
			(*logger_import) << " " << (unsigned int)(((uint8_t *)buffer)[i]);
		}
		(*logger_import) << Dec << Endl
			<< EndDebugInfo;
	}
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
WriteMem(ADDRESS_TYPE addr, const void *buffer, uint32_t size) {
	if(verbose && logger_import) {
		(*logger_import) << DebugInfo << LOCATION
			<< "OS write memory: " << Endl
			<< " - addr = 0x" << Hex << addr << Dec << Endl
			<< " - size = " << size << Endl
			<< " - data =" << Hex;
		for(unsigned int i = 0; i < size; i++) {
			(*logger_import) << " " << (unsigned int)(((uint8_t *)buffer)[i]);
		}
		(*logger_import) << Dec << Endl
			<< EndDebugInfo;
	}
	memory_injection_import->InjectWriteMemory(addr, buffer, size);
}

/**
 * Checks that an implementation exists for a syscall name.
 *
 * @param syscall_name the name of the syscall to check.
 * @return True if the syscall exists.
 */
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
HasSyscall(const string &syscall_name) {
	return syscall_name_map.find(syscall_name) != syscall_name_map.end();
}

/**
 * Checks that an id has been associated to a system call implementation.
 *
 * @param syscall_id id to check. 
 * @return True if syscall_id has been associated to an implementation.
 */
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
HasSyscall(int syscall_id) {
	return syscall_impl_assoc_map.find(syscall_id) != syscall_impl_assoc_map.end();
}

/**
 * Associates an id (integer) to a the implementation of the syscall specified by syscall_name.
 *
 * @param syscall_name
 * @param syscall_id
 */
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
SetSyscallId(const string &syscall_name, int syscall_id) {
    syscall_t syscall_impl;

    if(logger_import)
    	(*logger_import) << DebugInfo << LOCATION
    		<< "Associating syscall_name \"" << syscall_name << "\""
    		<< "to syscall_id number " << syscall_id
    		<< Endl << EndDebugInfo;
    if(HasSyscall(syscall_name)) {
    	if(HasSyscall(syscall_id)) {
    		stringstream s;
    		s << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << endl;
    		s << "syscall_id already associated to syscall \"" << syscall_name_assoc_map[syscall_id] << "\"" << endl;
    		s << "  you wanted to associate it to " << syscall_name << endl;
    		throw std::runtime_error(s.str().c_str());
    	}
    	syscall_name_assoc_map[syscall_id] = syscall_name;
    	syscall_impl_assoc_map[syscall_id] = syscall_name_map[syscall_name];
    } else {
    	stringstream s;
    	s << __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << endl;
    	s << "Unimplemented system call (" << syscall_name << ")";
    	throw std::runtime_error(s.str().c_str());      
    }
}

/**
 * Gets from the cpu calling this method the syscall id to execute and emulates it if it exists.
 */
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
ExecuteSystemCall(int id) {
	int translated_id = GetSyscallNumber(id);

	if (HasSyscall(translated_id)) {
		current_syscall_id = translated_id;
		current_syscall_name = syscall_name_assoc_map[translated_id];
		if(logger_import)
			(*logger_import) << DebugInfo << LOCATION
				<< "Executing syscall(name = " << current_syscall_name << ";"
				<< " id = " << translated_id << ";"
				<< " unstranslated id = " << id << ")" << Endl
				<< EndDebugInfo;
		syscall_t y = syscall_impl_assoc_map[translated_id];
		(this->*y)();
    } else
    	throw UnimplementedSystemCall(__FUNCTION__,
				    __FILE__,
				    __LINE__,
				    translated_id);
}

/**
 * Checks if the endianess of the host and the target system differs.
 */
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
bool 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
Swap() {
#if BYTE_ORDER == BIG_ENDIAN
	if(GetEndianess() == E_BIG_ENDIAN) return false;
	else return true;
#else
    if(GetEndianess() == E_BIG_ENDIAN) return true;
    else return false;
#endif
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
PerformSwap(void *buf,
		int count) {
	int size = count;
    
    if(count > 0) {
    	char *dst_base, *dst;
    	char *src;

    	dst = (char *)malloc(sizeof(char) * count);
    	dst_base = dst;
    	src = (char *)buf + count - 1;
      
    	do {
    		*dst = *src;
        } while(src--, dst++, --count);
    	memcpy(buf, dst_base, size);
    	free(dst_base);
    }
}
  
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
StringLength(ADDRESS_TYPE addr) {
    int len = 0;
    char buffer;
    
    while(1) {
    	ReadMem(addr, &buffer, 1);
    	if(buffer == 0) return len;
    	len++;
    	addr += 1;
    }
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::LSC_unknown() {
	if(verbose && logger_import) {
		(*logger_import) << DebugInfo << LOCATION
			<< "Unimplemented system call #" << current_syscall_id
			<< Endl << EndDebugInfo;
	}
	SetSystemCallStatus(-1, true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_exit() {
	string name = "exit";
	int ret;
    
	ret = GetSystemCallParam(0);
	if(verbose && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "LSC_exit with ret = 0X" << Hex << ret << Dec
			<< Endl << EndDebugInfo;
	cpu_linux_os_import->PerformExit(ret);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_read() {
	string name = "read";
	int fd;
	size_t count;
	ADDRESS_TYPE buf_addr;
	void *buf;
	size_t ret;
   
	fd = GetSystemCallParam(0);
	buf_addr = (ADDRESS_TYPE) GetSystemCallParam(1);
	count = (size_t) GetSystemCallParam(2);
	
	buf = malloc(count);
   
	if(buf) {
		ret = read(fd, buf, count);
		if(ret > 0) WriteMem(buf_addr, buf, ret);
		free(buf);
	} else {
		ret = (size_t)-1;
	}
		
	if(verbose && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "read(fd=" << fd << ", buf=0x" << Hex << buf_addr << Dec 
			<< ", count=" << count << ") return " << ret 
			<< Endl << EndDebugInfo;
		
	SetSystemCallStatus(ret, ret < 0);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_write() {
	int fd; 
	size_t count;
	void *buf;
	ADDRESS_TYPE buf_addr;
	size_t ret;
   
	fd = GetSystemCallParam(0);
	buf_addr = GetSystemCallParam(1);
	count = (size_t)GetSystemCallParam(2);
	buf = malloc(count);
   
	ret = (size_t)-1;
   
	if(buf) {
		ReadMem(buf_addr, buf, count);
		if((fd == 1 || fd == 2)) {
			char *tbuf = new char[count + 1];
			memcpy(tbuf, buf, count);
			tbuf[count] = '\0';
			string str(tbuf);
			cout << (str);
			cout << flush;
			ret = count;
			delete[] tbuf;
		} else 
			ret = write(fd, buf, count);
		free(buf);
	} else 
		ret = (size_t)-1;
		
	if(verbose && logger_import) 
		(*logger_import) << DebugInfo << LOCATION
			<< "write(fd=" << fd << ", buf=0x" << Hex << buf_addr << Dec 
			<< ", count=" << count << ") return " << ret 
			<< Endl << EndDebugInfo;
	SetSystemCallStatus(ret, ret < 0);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_open() {
	ADDRESS_TYPE addr;
	int pathnamelen;
	char *pathname;
	int flags;
	int64_t ret;
	mode_t mode;
    
	addr = GetSystemCallParam(0);
	pathnamelen = StringLength(addr);
	pathname = (char *) malloc(pathnamelen + 1);
	ReadMem(addr, pathname, pathnamelen + 1);
	flags = GetSystemCallParam(1);
	mode = GetSystemCallParam(2);
#if defined(WIN32) || defined(WIN64)
	int win_flags = 0;
	int win_mode = 0;
	// Windows and Linux open flags encoding differ
	if((flags & LINUX_O_ACCMODE) == LINUX_O_RDONLY) win_flags = (win_flags & ~O_ACCMODE) | O_RDONLY;
	if((flags & LINUX_O_ACCMODE) == LINUX_O_WRONLY) win_flags = (win_flags & ~O_ACCMODE) | O_WRONLY;
	if((flags & LINUX_O_ACCMODE) == LINUX_O_RDWR) win_flags = (win_flags & ~O_ACCMODE) | O_RDWR;
	if(flags & LINUX_O_CREAT) win_flags |= O_CREAT;
	if(flags & LINUX_O_EXCL) win_flags |= O_EXCL;
	if(flags & LINUX_O_TRUNC) win_flags |= O_TRUNC;
	if(flags & LINUX_O_APPEND) win_flags |= O_APPEND;
	win_mode = mode & S_IRWXU; // Windows doesn't have bits for group and others
	if(strcmp(pathname, osrelease_filename) == 0)
	{
		{
			std::ofstream fake_file(fake_osrelease_filename);
			fake_file << fake_osrelease << std::endl;
		}
		ret = open(fake_osrelease_filename, win_flags, win_mode);
	}
	else
		ret = open(pathname, win_flags, win_mode);
#else
	ret = open(pathname, flags, mode);
#endif
	
	if(verbose && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "open(pathname=\"" << pathname << "\", flags=0x" << Hex << flags 
			<< ", mode=0x" << mode << Dec << ") return " << ret 
			<< Endl << EndDebugInfo;
		
	free(pathname);
	SetSystemCallStatus(ret, (ret < 0));
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_close() {
	int fd;
	int ret;
   
	fd = GetSystemCallParam(0);
	ret = close(fd);
	if(verbose && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "close(fd=" << fd << ") return " << ret 
			<< Endl << EndDebugInfo;
	SetSystemCallStatus(ret, ret < 0);
}
	

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_lseek() {
	int fildes;
	off_t offset;
	int whence;
	off_t ret;
   
	fildes = GetSystemCallParam(0);
	offset = GetSystemCallParam(1);
	whence = GetSystemCallParam(2);
	ret = lseek(fildes, offset, whence);
	if(verbose && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "lseek(fildes=" << fildes << ", offset=" << offset 
			<< ", whence=" << whence << ") return " << ret 
			<< Endl << EndDebugInfo;
  
	if (ret == (off_t)-1) 
		SetSystemCallStatus(errno, true);
	else
		SetSystemCallStatus(ret, false);	
}


template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_getpid() {
	pid_t ret;
    
	ret = (pid_t) 1000;
	if(verbose && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "getpid() return " << ret 
			<< Endl << EndDebugInfo;
	SetSystemCallStatus(ret,false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE> 
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_getuid() {
#ifdef WIN32
	uint32_t ret = 0;
#else
	uid_t ret;
    
	ret = getuid();
#endif
	if(verbose && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "getuid() return " << ret 
			<< Endl << EndDebugInfo;
	SetSystemCallStatus(ret,false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_access() {
	ADDRESS_TYPE addr;
	int pathnamelen;
	char *pathname;
	mode_t mode;
	int ret;
    
	addr = GetSystemCallParam(0);
	pathnamelen = StringLength(addr);
	pathname = (char *) malloc(pathnamelen + 1);
	ReadMem(addr, pathname, pathnamelen + 1);
	mode = GetSystemCallParam(1);
#if defined(WIN32) || defined(WIN64)
	int win_mode = 0;
	win_mode = mode & S_IRWXU; // Windows doesn't have bits for group and others
	ret = access(pathname, win_mode);
#else
	ret = access(pathname, mode);
#endif
	if(verbose && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "access(pathname=\"" << pathname 
			<< "\", mode=0x" << Hex << mode << Dec << ") return " << ret 
			<< Endl << EndDebugInfo;
	free(pathname);
	SetSystemCallStatus(ret, ret < 0);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE> 
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_times() {
	int ret;
	ADDRESS_TYPE buf_addr;
	buf_addr = GetSystemCallParam(0);

	if(system == "arm")
	{
		struct arm_tms_t target_tms;
		ret = Times(&target_tms);

		if(ret >= 0)
		{
			WriteMem(buf_addr, &target_tms, sizeof(target_tms));
		}
	}
	else if(system == "powerpc")
	{
		struct powerpc_tms_t target_tms;
		ret = Times(&target_tms);

		if(ret >= 0)
		{
			WriteMem(buf_addr, &target_tms, sizeof(target_tms));
		}
	}
	else ret = -1;

	if(verbose && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "times(buf=0x" << Hex << buf_addr << Dec << ") return " << ret 
			<< Endl << EndDebugInfo;
	SetSystemCallStatus(ret, ret != -1);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_brk() {
	ADDRESS_TYPE new_brk_point;
    
	new_brk_point = GetSystemCallParam(0);
    
	if(new_brk_point > GetBrkPoint())
		SetBrkPoint(new_brk_point);      
		
	if(verbose && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "brk(new_brk_point=0x" << Hex << new_brk_point 
			<< ") return 0x" << GetBrkPoint() << Dec 
			<< Endl << EndDebugInfo;
	SetSystemCallStatus(GetBrkPoint(),false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE> 
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_getgid() {
#ifdef WIN32
	uint32_t ret = 0;
#else
	gid_t ret;
    
	ret = getgid();
#endif
	if(verbose && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "getgid() return " << ret 
			<< Endl << EndDebugInfo;
	SetSystemCallStatus(ret,false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_geteuid() {
#ifdef WIN32
	uint32_t ret = 0;
#else
	uid_t ret;
    
	ret = geteuid();
#endif
	if(verbose && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "geteuid() return " << ret 
			<< Endl << EndDebugInfo;
	SetSystemCallStatus(ret,false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE> 
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_getegid() {
#ifdef WIN32
	uint32_t ret = 0;
#else
	gid_t ret;
    
	ret = getegid();
#endif
	if(verbose && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "getegid() return " << ret 
			<< Endl << EndDebugInfo;
	SetSystemCallStatus(ret,false);
}


template<class ADDRESS_TYPE, class PARAMETER_TYPE> 
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_munmap() {
	size_t u = (size_t)(GetSystemCallParam(1));
   
	if(GetMmapBrkPoint() - u > GetMmapBrkPoint()) {
		SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
		if(verbose && logger_import)
			(*logger_import) << DebugInfo << LOCATION
				<< "size = " << u 
				<< ", ret = 0x" << Hex << ((PARAMETER_TYPE)(-EINVAL)) << Dec 
				<< Endl << EndDebugInfo;
		return;
	}
    
	if(GetMmapBrkPoint() - u < GetMmapBase())
		u = (size_t)(GetMmapBrkPoint() - GetMmapBase());
   
	if(GetMmapBrkPoint() - u >= GetMmapBrkPoint()) {
		SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
		if(verbose && logger_import)
			(*logger_import) << DebugInfo << LOCATION
				<< "size = " << u 
				<< ", ret = 0x" << Hex << ((PARAMETER_TYPE)(-EINVAL)) << Dec 
				<< Endl << EndDebugInfo;
		return;
	}

	SetSystemCallStatus((PARAMETER_TYPE)0,false);
	if(verbose && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "size = " << u 
			<< ", ret = 0x" << Hex << 0 << Dec 
			<< Endl << EndDebugInfo;
	SetMmapBrkPoint(GetMmapBrkPoint() - u);
}
	
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_stat() {
	if(logger_import) 
		(*logger_import) << DebugWarning << LOCATION
			<< "TODO" 
			<< Endl << EndDebugWarning;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
int LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
Stat(int fd, struct powerpc_stat_t *target_stat)
{
	int ret;
	struct stat host_stat;
	ret = fstat(fd, &host_stat);
	if(ret < 0) return ret;

#if defined(__x86_64)
	// Linux x86_64 host
	target_stat->st_dev = Host2Target(endianess, (uint64_t) host_stat.st_dev);
	target_stat->st_ino = Host2Target(endianess, (uint64_t) host_stat.st_ino);
	target_stat->st_mode = Host2Target(endianess, (uint32_t) host_stat.st_mode);
	target_stat->st_nlink = Host2Target(endianess, (uint64_t) host_stat.st_nlink);
	target_stat->st_uid = Host2Target(endianess, (uint32_t) host_stat.st_uid);
	target_stat->st_gid = Host2Target(endianess, (uint32_t) host_stat.st_gid);
	target_stat->st_rdev = Host2Target(endianess, (int64_t) host_stat.st_rdev);
	target_stat->__pad2 = 0;
	target_stat->st_size = Host2Target(endianess, (int64_t) host_stat.st_size);
#if defined(WIN64)
	// Windows 64
	target_stat->st_blksize = Host2Target((int32_t) 512);
	target_stat->st_blocks = Host2Target((int64_t)((host_stat.st_size + 511) / 512));
	target_stat->st_atim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_atim);
	target_stat->st_atim.tv_nsec = 0;
	target_stat->st_mtim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_mtim);
	target_stat->st_mtim.tv_nsec = 0;
	target_stat->st_ctim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_ctim);
	target_stat->st_ctim.tv_nsec = 0;
#else // WIN64
	// Linux 64
	target_stat->st_blksize = Host2Target(endianess, (int64_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2Target(endianess, (int64_t) host_stat.st_blocks);
	target_stat->st_atim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_atim.tv_sec);
	target_stat->st_atim.tv_nsec = Host2Target(endianess, (int64_t) host_stat.st_atim.tv_nsec);
	target_stat->st_mtim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_mtim.tv_sec);
	target_stat->st_mtim.tv_nsec = Host2Target(endianess, (int64_t) host_stat.st_mtim.tv_nsec);
	target_stat->st_ctim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_ctim.tv_sec);
	target_stat->st_ctim.tv_nsec = Host2Target(endianess, (int64_t) host_stat.st_ctim.tv_nsec);
#endif // !WIN64
#else // __x86_64
	// Linux x86 host
	target_stat->st_dev = Host2Target(endianess, (uint64_t) host_stat.st_dev);
	target_stat->st_ino = Host2Target(endianess, (uint32_t) host_stat.st_ino);
	target_stat->st_mode = Host2Target(endianess, (uint32_t) host_stat.st_mode);
	target_stat->st_nlink = Host2Target(endianess, (uint32_t) host_stat.st_nlink);
	target_stat->st_uid = Host2Target(endianess, (uint32_t) host_stat.st_uid);
	target_stat->st_gid = Host2Target(endianess, (uint32_t) host_stat.st_gid);
	target_stat->st_rdev = Host2Target(endianess, (int64_t) host_stat.st_rdev);
	target_stat->__pad2 = 0;
	target_stat->st_size = Host2Target(endianess, (int64_t) host_stat.st_size);
#if defined(WIN32)
	target_stat->st_blksize = Host2Target(endianess, (int32_t) 512);
	target_stat->st_blocks = Host2Target(endianess, (int64_t)((host_stat.st_size + 511) / 512));
	target_stat->st_atim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_atime);
	target_stat->st_atim.tv_nsec = 0;
	target_stat->st_mtim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_mtime);
	target_stat->st_mtim.tv_nsec = 0;
	target_stat->st_ctim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_ctime);
	target_stat->st_ctim.tv_nsec = 0;
#else // WIN32
	target_stat->st_blksize = Host2Target(endianess, (int32_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2Target(endianess, (int64_t) host_stat.st_blocks);
	target_stat->st_atim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_atim.tv_sec);
	target_stat->st_atim.tv_nsec = Host2Target(endianess, (int32_t) host_stat.st_atim.tv_nsec);
	target_stat->st_mtim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_mtim.tv_sec);
	target_stat->st_mtim.tv_nsec = Host2Target(endianess, (int32_t) host_stat.st_mtim.tv_nsec);
	target_stat->st_ctim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_ctim.tv_sec);
	target_stat->st_ctim.tv_nsec = Host2Target(endianess, (int32_t) host_stat.st_ctim.tv_nsec);
#endif // !WIN32
#endif // !__x86_64
    target_stat->__unused4 = 0;
    target_stat->__unused5 = 0;
	return ret;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
int LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
Stat64(int fd, struct powerpc_stat64_t *target_stat)
{
	int ret;
#if defined(WIN32) || defined(WIN64)
	struct _stati64 host_stat;
	ret = _fstati64(fd, &host_stat);
#else
	struct stat64 host_stat;
	ret = fstat64(fd, &host_stat);
#endif
	if(ret < 0) return ret;

#if defined(__x86_64)
	// x86_64 host
	target_stat->st_dev = Host2Target(endianess, (uint64_t) host_stat.st_dev);
	target_stat->st_ino = Host2Target(endianess, (uint64_t) host_stat.st_ino);
	target_stat->st_mode = Host2Target(endianess, (uint32_t) host_stat.st_mode);
	target_stat->st_nlink = Host2Target(endianess, (uint64_t) host_stat.st_nlink);
	target_stat->st_uid = Host2Target(endianess, (uint32_t) host_stat.st_uid);
	target_stat->st_gid = Host2Target(endianess, (uint32_t) host_stat.st_gid);
	target_stat->st_rdev = Host2Target(endianess, (int64_t) host_stat.st_rdev);
	target_stat->__pad2 = 0;
	target_stat->st_size = Host2Target(endianess, (int64_t) host_stat.st_size);
#if defined(WIN64)
	target_stat->st_blksize = Host2Target(endianess, (int32_t) 512);
	target_stat->st_blocks = Host2Target(endianess, (int64_t)((host_stat.st_size + 511) / 512));
	target_stat->st_atim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_atim);
	target_stat->st_atim.tv_nsec = 0;
	target_stat->st_mtim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_mtim);
	target_stat->st_mtim.tv_nsec = 0;
	target_stat->st_ctim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_ctim);
	target_stat->st_ctim.tv_nsec = 0;
#else
	target_stat->st_blksize = Host2Target(endianess, (int32_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2Target(endianess, (int64_t) host_stat.st_blocks);
	target_stat->st_atim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_atim.tv_sec);
	target_stat->st_atim.tv_nsec = Host2Target(endianess, (int64_t) host_stat.st_atim.tv_nsec);
	target_stat->st_mtim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_mtim.tv_sec);
	target_stat->st_mtim.tv_nsec = Host2Target(endianess, (int64_t) host_stat.st_mtim.tv_nsec);
	target_stat->st_ctim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_ctim.tv_sec);
	target_stat->st_ctim.tv_nsec = Host2Target(endianess, (int64_t) host_stat.st_ctim.tv_nsec);
#endif
#else
	target_stat->st_dev = Host2Target(endianess, (uint64_t) host_stat.st_dev);
	target_stat->st_ino = Host2Target(endianess, (uint32_t) host_stat.st_ino);
	target_stat->st_mode = Host2Target(endianess, (uint32_t) host_stat.st_mode);
	target_stat->st_nlink = Host2Target(endianess, (uint32_t) host_stat.st_nlink);
	target_stat->st_uid = Host2Target(endianess, (uint32_t) host_stat.st_uid);
	target_stat->st_gid = Host2Target(endianess, (uint32_t) host_stat.st_gid);
	target_stat->st_rdev = Host2Target(endianess, (int64_t) host_stat.st_rdev);
	target_stat->__pad2 = 0;
	target_stat->st_size = Host2Target(endianess, (int64_t) host_stat.st_size);
#if defined(WIN32)
	target_stat->st_blksize = Host2Target(endianess, (int32_t) 512);
	target_stat->st_blocks = Host2Target(endianess, (int64_t)((host_stat.st_size + 511) / 512));
	target_stat->st_atim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_atime);
	target_stat->st_atim.tv_nsec = 0;
	target_stat->st_mtim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_mtime);
	target_stat->st_mtim.tv_nsec = 0;
	target_stat->st_ctim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_ctime);
	target_stat->st_ctim.tv_nsec = 0;
#else
	target_stat->st_blksize = Host2Target(endianess, (int32_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2Target(endianess, (int64_t) host_stat.st_blocks);
	target_stat->st_atim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_atim.tv_sec);
	target_stat->st_atim.tv_nsec = Host2Target(endianess, (int32_t) host_stat.st_atim.tv_nsec);
	target_stat->st_mtim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_mtim.tv_sec);
	target_stat->st_mtim.tv_nsec = Host2Target(endianess, (int32_t) host_stat.st_mtim.tv_nsec);
	target_stat->st_ctim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_ctim.tv_sec);
	target_stat->st_ctim.tv_nsec = Host2Target(endianess, (int32_t) host_stat.st_ctim.tv_nsec);
#endif
#endif
    target_stat->__unused4 = 0;
    target_stat->__unused5 = 0;
	return ret;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
int LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
Stat64(int fd, arm_stat64_t *target_stat)
{
	int ret;
#if defined(WIN32) || defined(WIN64)
	struct _stati64 host_stat;
	ret = _fstati64(fd, &host_stat);
#else
	struct stat64 host_stat;
	ret = fstat64(fd, &host_stat);
#endif
	if(ret < 0) return ret;

#if defined(__x86_64)
	// x86_64 host
	target_stat->st_dev = Host2Target(endianess, (uint64_t) host_stat.st_dev);
	target_stat->st_ino = Host2Target(endianess, (uint64_t) host_stat.st_ino);
	target_stat->st_mode = Host2Target(endianess, (uint32_t) host_stat.st_mode);
	target_stat->st_nlink = Host2Target(endianess, (uint64_t) host_stat.st_nlink);
	target_stat->st_uid = Host2Target(endianess, (uint32_t) host_stat.st_uid);
	target_stat->st_gid = Host2Target(endianess, (uint32_t) host_stat.st_gid);
	target_stat->st_rdev = Host2Target(endianess, (int64_t) host_stat.st_rdev);
	target_stat->__pad2 = 0;
	target_stat->st_size = Host2Target(endianess, (int64_t) host_stat.st_size);
#if defined(WIN64)
	target_stat->st_blksize = Host2Target(endianess, (int32_t) 512);
	target_stat->st_blocks = Host2Target(endianess, (int64_t)((host_stat.st_size + 511) / 512));
	target_stat->st_atim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_atim);
	target_stat->st_atim.tv_nsec = 0;
	target_stat->st_mtim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_mtim);
	target_stat->st_mtim.tv_nsec = 0;
	target_stat->st_ctim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_ctim);
	target_stat->st_ctim.tv_nsec = 0;
#else
	target_stat->st_blksize = Host2Target(endianess, (int32_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2Target(endianess, (int64_t) host_stat.st_blocks);
	target_stat->st_atim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_atim.tv_sec);
	target_stat->st_atim.tv_nsec = Host2Target(endianess, (int64_t) host_stat.st_atim.tv_nsec);
	target_stat->st_mtim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_mtim.tv_sec);
	target_stat->st_mtim.tv_nsec = Host2Target(endianess, (int64_t) host_stat.st_mtim.tv_nsec);
	target_stat->st_ctim.tv_sec = Host2Target(endianess, (int64_t) host_stat.st_ctim.tv_sec);
	target_stat->st_ctim.tv_nsec = Host2Target(endianess, (int64_t) host_stat.st_ctim.tv_nsec);
#endif
#else
	target_stat->st_dev = Host2Target(endianess, (uint64_t) host_stat.st_dev);
	target_stat->st_ino = Host2Target(endianess, (uint32_t) host_stat.st_ino);
	target_stat->st_mode = Host2Target(endianess, (uint32_t) host_stat.st_mode);
	target_stat->st_nlink = Host2Target(endianess, (uint32_t) host_stat.st_nlink);
	target_stat->st_uid = Host2Target(endianess, (uint32_t) host_stat.st_uid);
	target_stat->st_gid = Host2Target(endianess, (uint32_t) host_stat.st_gid);
	target_stat->st_rdev = Host2Target(endianess, (int64_t) host_stat.st_rdev);
	target_stat->__pad2 = 0;
	target_stat->st_size = Host2Target(endianess, (int64_t) host_stat.st_size);
#if defined(WIN32)
	target_stat->st_blksize = Host2Target(endianess, (int32_t) 512);
	target_stat->st_blocks = Host2Target(endianess, (int64_t)((host_stat.st_size + 511) / 512));
	target_stat->st_atim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_atime);
	target_stat->st_atim.tv_nsec = 0;
	target_stat->st_mtim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_mtime);
	target_stat->st_mtim.tv_nsec = 0;
	target_stat->st_ctim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_ctime);
	target_stat->st_ctim.tv_nsec = 0;
#else
	target_stat->st_blksize = Host2Target(endianess, (int32_t) host_stat.st_blksize);
	target_stat->st_blocks = Host2Target(endianess, (int64_t) host_stat.st_blocks);
	target_stat->st_atim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_atim.tv_sec);
	target_stat->st_atim.tv_nsec = Host2Target(endianess, (int32_t) host_stat.st_atim.tv_nsec);
	target_stat->st_mtim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_mtim.tv_sec);
	target_stat->st_mtim.tv_nsec = Host2Target(endianess, (int32_t) host_stat.st_mtim.tv_nsec);
	target_stat->st_ctim.tv_sec = Host2Target(endianess, (int32_t) host_stat.st_ctim.tv_sec);
	target_stat->st_ctim.tv_nsec = Host2Target(endianess, (int32_t) host_stat.st_ctim.tv_nsec);
#endif
#endif
	return ret;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
int LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
Times(struct powerpc_tms_t *target_tms)
{
	int ret;
#ifdef WIN32
	FILETIME ftCreationTime;
	FILETIME ftExitTime;
	FILETIME ftKernelTime;
	FILETIME ftUserTime;

	if(GetProcessTimes(GetCurrentProcess(), &ftCreationTime, &ftExitTime, &ftKernelTime, &ftUserTime)) return -1;

	target_tms->tms_utime = Host2Target(endianess, (uint32_t) ftUserTime.dwLowDateTime);
	target_tms->tms_stime = Host2Target(endianess, (uint32_t) ftKernelTime.dwLowDateTime);
	target_tms->tms_cutime = 0;   // User CPU time of dead children
	target_tms->tms_cstime = 0;   // System CPU time of dead children
#else
	struct tms host_tms;

	ret = (int) times(&host_tms);
	target_tms->tms_utime = Host2Target(endianess, (int32_t) host_tms.tms_utime);
	target_tms->tms_stime = Host2Target(endianess, (int32_t) host_tms.tms_stime);
	target_tms->tms_cutime = Host2Target(endianess, (int32_t) host_tms.tms_cutime);
	target_tms->tms_cstime = Host2Target(endianess, (int32_t) host_tms.tms_cstime);
#endif
	return ret;
}

template <class ADDRESS_TYPE, class PARAMETER_TYPE>
int LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
Times(struct arm_tms_t *target_tms)
{
	int ret;
#ifdef WIN32
	FILETIME ftCreationTime;
	FILETIME ftExitTime;
	FILETIME ftKernelTime;
	FILETIME ftUserTime;

	if(GetProcessTimes(GetCurrentProcess(), &ftCreationTime, &ftExitTime, &ftKernelTime, &ftUserTime)) return -1;

	target_tms->tms_utime = Host2Target(endianess, (uint32_t) ftUserTime.dwLowDateTime);
	target_tms->tms_stime = Host2Target(endianess, (uint32_t) ftKernelTime.dwLowDateTime);
	target_tms->tms_cutime = 0;   // User CPU time of dead children
	target_tms->tms_cstime = 0;   // System CPU time of dead children
#else
	struct tms host_tms;

	ret = (int) times(&host_tms);
	target_tms->tms_utime = Host2Target(endianess, (int32_t) host_tms.tms_utime);
	target_tms->tms_stime = Host2Target(endianess, (int32_t) host_tms.tms_stime);
	target_tms->tms_cutime = Host2Target(endianess, (int32_t) host_tms.tms_cutime);
	target_tms->tms_cstime = Host2Target(endianess, (int32_t) host_tms.tms_cstime);
#endif
	return ret;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_fstat() {
	int ret;
	int fd;
	ADDRESS_TYPE buf_address;
	struct stat *buf;
    
	fd = GetSystemCallParam(0);
	buf_address = GetSystemCallParam(1);
	if(system == "arm")
	{
		ret = -1;
	}
	else if(system == "powerpc")
	{
		struct powerpc_stat_t target_stat;
		ret = Stat(fd, &target_stat);
		WriteMem(buf_address, &target_stat, sizeof(target_stat));
	}
	else ret = -1;

	if(verbose && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "stat(fd=" << fd 
			<< ", buf_addr=0x" << Hex << buf_address << Dec 
			<< ") return " << ret 
			<< Endl << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)ret,ret < 0);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_uname() {
	int ret;
// 	static const char sysname[] = "Linux\0localhost\0""2.6.8\0#1 SMP Tue Feb 12 07:42:25 UTC 2008\0armv5teb\0(none)\0";
// 	ADDRESS_TYPE buf_addr = GetSystemCallParam(0);
// 	WriteMem(buf_addr, sysname, sizeof(sysname));
// 	ret = 0;
	ret = -1;
	if(verbose && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "uname() return " << ret 
			<< Endl << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE) ret, ret < 0);
}
	
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_llseek() {
	int fd;
	uint32_t offset_high;
	uint32_t offset_low;
	PARAMETER_TYPE result_addr;
	uint64_t lseek_ret64;
	off_t lseek_ret;
	int whence;
	int ret;
		
	fd = GetSystemCallParam(0);
	offset_high = GetSystemCallParam(1);
	offset_low = GetSystemCallParam(2);
	result_addr = GetSystemCallParam(3);
	whence = GetSystemCallParam(4);
	if(verbose && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "llseek(fd=" << fd 
			<< ", offset=" << (((int64_t) offset_high << 32) | (int64_t) offset_low)
			<< ", result_addr=0x" << Hex << result_addr << Dec 
			<< ", whence=" << whence << ")" 
			<< Endl << EndDebugInfo;
	if(offset_high == 0) {
		lseek_ret = lseek(fd, offset_low, whence);
		if(lseek_ret >= 0) {
			lseek_ret64 = lseek_ret;
			if(Swap())
				PerformSwap(&lseek_ret64, sizeof(lseek_ret64));
			WriteMem(result_addr, &lseek_ret64, sizeof(lseek_ret64));
			SetSystemCallStatus((PARAMETER_TYPE)lseek_ret, false);
		} else 
			SetSystemCallStatus((PARAMETER_TYPE)errno, true);
	} else 
		SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}


template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_writev() {
	if(verbose && logger_import) 
		(*logger_import) << DebugInfo << LOCATION
			<< "ret = 0x" 
			<< Hex << ((PARAMETER_TYPE)(-EINVAL)) << Dec 
			<< Endl << EndDebugInfo;

	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_mmap() {
	SetSystemCallStatus(-1,true); return;
	if(GetSystemCallParam(3) == 0x32) { /* MAP_PRIVATE | MAP_ANONYMOUS */
		SetSystemCallStatus(GetSystemCallParam(0),false);
		if(verbose && logger_import) 
			(*logger_import) << DebugInfo << LOCATION
				<< "map_type = 0x" << Hex << GetSystemCallParam(3) << Dec 
				<< ", size = " << GetSystemCallParam(1)
				<< ", ret = 0x" << Hex << ((PARAMETER_TYPE)GetSystemCallParam(0)) << Dec 
				<< Endl << EndDebugInfo;
		return;
	}
		
	if((GetSystemCallParam(3)&0xFF) != 0x22) { /* MAP_PRIVATE | MAP_ANONYMOUS */
		SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
		if(verbose && logger_import) 
			(*logger_import) << DebugInfo << LOCATION
				<< "map_type = 0x" << Hex << GetSystemCallParam(3) << Dec 
				<< ", size = " << GetSystemCallParam(1)
				<< ", ret = 0x" << Hex << ((PARAMETER_TYPE)(-EINVAL)) << Dec 
				<< Endl << EndDebugInfo;
		return;
	}
	SetSystemCallStatus(GetMmapBrkPoint(),false);
	if(verbose && logger_import) 
		(*logger_import) << DebugInfo << LOCATION
			<< "map_type = 0x" << Hex << GetSystemCallParam(3) << Dec 
			<< ", size = " << GetSystemCallParam(1)
			<< ", ret = 0x" << Hex << GetMmapBrkPoint() << Dec 
			<< Endl << EndDebugInfo;
	SetMmapBrkPoint(GetMmapBrkPoint() + GetSystemCallParam(1));
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_mmap2() {
	if(GetSystemCallParam(3) != 0x22) { /* MAP_PRIVATE | MAP_ANONYMOUS */
		SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
		if(verbose && logger_import)    
			(*logger_import) << DebugInfo << LOCATION 
				<< "map_type = 0x" << Hex << GetSystemCallParam(3) << Dec 
				<< ", size = " << GetSystemCallParam(1)
				<< ", ret = 0x" << Hex << ((PARAMETER_TYPE)(-EINVAL)) << Dec 
				<< Endl << EndDebugInfo;
		return;
	}
    
	if(GetMmapBrkPoint() + GetSystemCallParam(1) > GetSystemCallParam(1)) {
		SetSystemCallStatus(GetMmapBrkPoint(),false);
		if(verbose && logger_import) 
			(*logger_import) << DebugInfo << LOCATION
				<< "map_type = 0x" << Hex << GetSystemCallParam(3) << Dec 
				<< ", size = " << GetSystemCallParam(1)
				<< ", ret = 0x" << Hex << GetMmapBrkPoint() << Dec 
				<< Endl << EndDebugInfo;
		SetMmapBrkPoint(GetMmapBrkPoint() + GetSystemCallParam(1));
	} else {
		if(verbose && logger_import) 
			(*logger_import) << DebugInfo << LOCATION
				<< "map_type = 0x" << Hex << GetSystemCallParam(3) << Dec 
				<< ", size = " << GetSystemCallParam(1)
				<< ", ret = 0x" << Hex << ((PARAMETER_TYPE)(-EINVAL)) << Dec 
				<< Endl << EndDebugInfo;
		SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
	}
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_fstat64() { 
	int ret;
	ADDRESS_TYPE buf_address;
	int fd;
	struct stat64 *buf;

	fd = GetSystemCallParam(0);
	buf_address = GetSystemCallParam(1);
	if(system == "arm")
	{
		struct arm_stat64_t target_stat;
		ret = Stat64(fd, &target_stat);
		WriteMem(buf_address, &target_stat, sizeof(target_stat));
	}
	else if(system == "powerpc")
	{
		struct powerpc_stat64_t target_stat;
		ret = Stat64(fd, &target_stat);
		WriteMem(buf_address, &target_stat, sizeof(target_stat));
	}
	else ret = -1;
	if(verbose && logger_import)     
		(*logger_import) << DebugInfo << LOCATION
			<< "fd = " << fd << ", buf_address = 0x" << Hex << buf_address << Dec 
			<< ", ret = 0x" << Hex << ret << Dec 
			<< Endl << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)ret,ret < 0);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_getuid32() {
#ifdef WIN32
	uint32_t ret = 0;
#else
	uid_t ret;

	ret = getuid();
#endif
	if(verbose && logger_import) 
		(*logger_import) << DebugInfo << LOCATION
			<< "ret = 0x" << Hex << ((PARAMETER_TYPE)ret) << Dec 
			<< Endl << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)ret,false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_getgid32() {
#ifdef WIN32
	uint32_t ret = 0;
#else
	gid_t ret;
    
	ret = getgid();
#endif
	if(verbose && logger_import) 
		(*logger_import) << DebugInfo << LOCATION
			<< "ret = 0x" << Hex << ((PARAMETER_TYPE)ret) << Dec 
			<< Endl << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)ret,false);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_geteuid32() {
#ifdef WIN32
	uint32_t ret = 0;
#else
	uid_t ret;
    
	ret = geteuid();
#endif
	if(verbose && logger_import) 
		(*logger_import) << DebugInfo << LOCATION
			<< "ret = 0x" << Hex << ((PARAMETER_TYPE)ret) << Dec 
			<< Endl << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)ret,false);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_getegid32() {
#ifdef WIN32
	uint32_t ret = 0;
#else
	gid_t ret;
    
	ret = getegid();
#endif
	if(verbose && logger_import) 
		(*logger_import) << DebugInfo << LOCATION
			<< "ret = 0x" << Hex << ((PARAMETER_TYPE)ret) << Dec 
			<< Endl << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)ret,false);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_flistxattr() {
	if(logger_import)
		(*logger_import) << DebugWarning << LOCATION
			<< "TODO" 
			<< Endl << EndDebugWarning;
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_exit_group() { 
	if(verbose && logger_import)  
		(*logger_import) << DebugInfo << LOCATION
			<< "ret = 0x" << Hex << ((PARAMETER_TYPE)(-EINVAL)) << Dec 
			<< Endl << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_fcntl() { 
	int64_t ret;
    
#if defined(WIN32) || defined(WIN64)
	ret = -1;
#else
	ret = fcntl(GetSystemCallParam(0),
			GetSystemCallParam(1),
			GetSystemCallParam(2));
#endif
	if(verbose && logger_import) 
		(*logger_import) << DebugInfo << LOCATION
			<< "ret = " <<  ((PARAMETER_TYPE)ret)  
			<< Endl << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)ret,ret < 0);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_fcntl64() {
	if(verbose && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "ret = 0x" << Hex << ((PARAMETER_TYPE)(-EINVAL)) << Dec 
			<< Endl << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_dup() {
	int ret;
	int oldfd = GetSystemCallParam(0);
    
	ret = dup(oldfd);
	if(verbose && logger_import) 
		(*logger_import) << DebugInfo << LOCATION
			<< "oldfd = " << oldfd << ", new fd = " << ((PARAMETER_TYPE)ret) 
			<< Endl << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)ret, ret < 0);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_ioctl() {
	if(verbose && logger_import)  
		(*logger_import) << DebugInfo << LOCATION
			<< "ret = 0x" << Hex << ((PARAMETER_TYPE)(-EINVAL)) << Dec 
			<< Endl << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_ugetrlimit() {
	if(verbose && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "ret = 0x" << Hex << ((PARAMETER_TYPE)(-EINVAL)) << Dec 
			<< Endl << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_getrlimit() {
	if(verbose && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "ret = 0x" << Hex << ((PARAMETER_TYPE)(-EINVAL)) << Dec 
			<< Endl << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_setrlimit() {
	if(verbose && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "ret = 0x" << Hex << ((PARAMETER_TYPE)(-EINVAL)) << Dec 
			<< Endl << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_rt_sigaction() {
	if(verbose && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "ret = 0x" << Hex << ((PARAMETER_TYPE)(-EINVAL)) << Dec 
			<< Endl << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_getrusage() {
	if(verbose && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "ret = 0x" << Hex << ((PARAMETER_TYPE)(-EINVAL)) << Dec 
			<< Endl << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_unlink() {
	if(verbose && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "ret = 0x" << Hex << ((PARAMETER_TYPE)(-EINVAL)) << Dec 
			<< Endl << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_rename() {
	if(verbose && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "ret = 0x" << Hex << ((PARAMETER_TYPE)(-EINVAL)) << Dec 
			<< Endl << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL),true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_time() {
	if(verbose && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "ret = 0x" << Hex << ((PARAMETER_TYPE)(-EINVAL)) << Dec 
			<< Endl << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_socketcall() {
	if(verbose && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "ret = 0x" << Hex << ((PARAMETER_TYPE)(-EINVAL)) << Dec 
			<< Endl << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_rt_sigprocmask() {
	if(verbose && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "ret = 0x" << Hex << ((PARAMETER_TYPE)(-EINVAL)) << Dec 
			<< Endl << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)(-EINVAL), true);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
LSC_kill() {
	if(verbose && logger_import)
		(*logger_import) << DebugInfo << LOCATION
			<< "ret = 0x" << Hex << ((PARAMETER_TYPE)(-EINVAL)) << Dec 
			<< Endl << EndDebugInfo;
	SetSystemCallStatus((PARAMETER_TYPE)0, false);
}
	
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
SetSyscallNameMap() {
	syscall_name_map[string("unknown")] = &thistype::LSC_unknown;
	syscall_name_map[string("exit")] = &thistype::LSC_exit;
	syscall_name_map[string("read")] = &thistype::LSC_read;
	syscall_name_map[string("write")] = &thistype::LSC_write;
	syscall_name_map[string("open")] = &thistype::LSC_open;
	syscall_name_map[string("close")] = &thistype::LSC_close;
	syscall_name_map[string("lseek")] = &thistype::LSC_lseek;
	syscall_name_map[string("getpid")] = &thistype::LSC_getpid;
	syscall_name_map[string("getuid")] = &thistype::LSC_getuid;
	syscall_name_map[string("access")] = &thistype::LSC_access;
	syscall_name_map[string("times")] = &thistype::LSC_times;
	syscall_name_map[string("brk")] = &thistype::LSC_brk;
	syscall_name_map[string("getgid")] = &thistype::LSC_getgid;
	syscall_name_map[string("geteuid")] = &thistype::LSC_geteuid;
	syscall_name_map[string("getegid")] = &thistype::LSC_getegid;
	syscall_name_map[string("munmap")] = &thistype::LSC_munmap;
	syscall_name_map[string("stat")] = &thistype::LSC_stat;
	syscall_name_map[string("fstat")] = &thistype::LSC_fstat;
	syscall_name_map[string("uname")] = &thistype::LSC_uname;
	syscall_name_map[string("llseek")] = &thistype::LSC_llseek;
	syscall_name_map[string("writev")] = &thistype::LSC_writev;
	syscall_name_map[string("mmap")] = &thistype::LSC_mmap;
	syscall_name_map[string("mmap2")] = &thistype::LSC_mmap2;
	syscall_name_map[string("fstat64")] = &thistype::LSC_fstat64;
	syscall_name_map[string("getuid32")] = &thistype::LSC_getuid32;
	syscall_name_map[string("getgid32")] = &thistype::LSC_getgid32;
	syscall_name_map[string("geteuid32")] = &thistype::LSC_geteuid32;
	syscall_name_map[string("getegid32")] = &thistype::LSC_getegid32;
	syscall_name_map[string("fcntl64")] = &thistype::LSC_fcntl64;
	syscall_name_map[string("flistxattr")] = &thistype::LSC_flistxattr;
	syscall_name_map[string("exit_group")] = &thistype::LSC_exit_group;
	syscall_name_map[string("fcntl")] = &thistype::LSC_fcntl;
	syscall_name_map[string("dup")] = &thistype::LSC_dup;
	syscall_name_map[string("ioctl")] = &thistype::LSC_ioctl;
	syscall_name_map[string("ugetrlimit")] = &thistype::LSC_ugetrlimit;
	syscall_name_map[string("getrlimit")] = &thistype::LSC_getrlimit;
	syscall_name_map[string("setrlimit")] = &thistype::LSC_setrlimit;
	syscall_name_map[string("rt_sigaction")] = &thistype::LSC_rt_sigaction;
	syscall_name_map[string("getrusage")] = &thistype::LSC_getrusage;
	syscall_name_map[string("unlink")] = &thistype::LSC_unlink;
	syscall_name_map[string("rename")] = &thistype::LSC_rename;
	syscall_name_map[string("time")] = &thistype::LSC_time;
	syscall_name_map[string("socketcall")] = &thistype::LSC_socketcall;
	syscall_name_map[string("rt_sigprocmask")] = &thistype::LSC_rt_sigprocmask;
	syscall_name_map[string("kill")] = &thistype::LSC_kill;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
GetSyscallNumber(int id) {
	if(system == "arm")
		return ARMGetSyscallNumber(id);
	else
		return PPCGetSyscallNumber(id);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
ARMGetSyscallNumber(int id) {
	int translated_id = id - 0x0900000;
	return translated_id;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
int
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
PPCGetSyscallNumber(int id) {
	return id;
}

// template<class ADDRESS_TYPE, class PARAMETER_TYPE>
// void *
// LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
// PPCConvertStat(struct stat *s) {
//     // this implementation supposes that the host is a x86 with __USE_LARGEFILE64
//     //   and __USE_MISC
//     struct powerpc_stat_t *res;
//       
//     res = (powerpc_stat_t *)malloc(sizeof(powerpc_stat_t));
//     if(GetEndianess() == E_BIG_ENDIAN) {
// #if defined(__x86_64)
// 		res->st_dev = Host2Target(endianess, (uint64_t)s->st_dev);
//         res->st_ino = Host2Target(endianess, (uint64_t)s->st_ino);
//         res->st_mode = Host2Target(endianess, (uint32_t)s->st_mode);
//         res->st_nlink = Host2Target(endianess, (uint64_t)s->st_nlink);
//         res->st_uid = Host2Target(endianess, (uint32_t)s->st_uid);
//         res->st_gid = Host2Target(endianess, (uint32_t)s->st_gid);
//         res->st_rdev = Host2Target(endianess, (int64_t)s->st_rdev);
//         res->__pad2 = 0;
//         res->st_size = Host2Target(endianess, (int64_t)s->st_size);
//         res->st_blksize = Host2Target(endianess, (int64_t)s->st_blksize);
//         res->st_blocks = Host2Target(endianess, (int64_t)s->st_blocks);
//         res->st_atim.tv_sec = Host2Target(endianess, (int64_t)s->st_atim.tv_sec);
//         res->st_atim.tv_nsec = Host2Target(endianess, (int64_t)s->st_atim.tv_nsec);
//         res->st_mtim.tv_sec = Host2Target(endianess, (int64_t)s->st_mtim.tv_sec);
//         res->st_mtim.tv_nsec = Host2Target(endianess, (int64_t)s->st_mtim.tv_nsec);
//         res->st_ctim.tv_sec = Host2Target(endianess, (int64_t)s->st_ctim.tv_sec);
//         res->st_ctim.tv_nsec = Host2Target(endianess, (int64_t)s->st_ctim.tv_nsec);
// #else
//         res->st_dev = Host2Target(endianess, (uint64_t)s->st_dev);
//         res->st_ino = Host2Target(endianess, (uint32_t)s->st_ino);
//         res->st_mode = Host2Target(endianess, (uint32_t)s->st_mode);
//         res->st_nlink = Host2Target(endianess, (uint32_t)s->st_nlink);
//         res->st_uid = Host2Target(endianess, (uint32_t)s->st_uid);
//         res->st_gid = Host2Target(endianess, (uint32_t)s->st_gid);
//         res->st_rdev = Host2Target(endianess, (int64_t)s->st_rdev);
//         res->__pad2 = 0;
//         res->st_size = Host2Target(endianess, (int64_t)s->st_size);
// #ifdef WIN32
//         res->st_blksize = 512;
//         res->st_blocks = s->st_size / 512;
// #else
//         res->st_blksize = Host2Target(endianess, (int32_t)s->st_blksize);
//         res->st_blocks = Host2Target(endianess, (int64_t)s->st_blocks);
// #endif
//         res->st_atim.tv_sec = Host2Target(endianess, (int32_t)s->st_atim);
//         res->st_atim.tv_nsec = 0;
//         res->st_mtim.tv_sec = Host2Target(endianess, (int32_t)s->st_mtim.tv_sec);
//         res->st_mtim.tv_nsec = 0;
//         res->st_ctim.tv_sec = Host2Target(endianess, (int32_t)s->st_ctim.tv_sec);
//         res->st_ctim.tv_nsec = 0;
// #endif
// 	} else {
// #if defined(__x86_64)
//         res->st_dev = Host2LittleEndian((uint64_t)s->st_dev);
//         res->st_ino = Host2LittleEndian((uint64_t)s->st_ino);
//         res->st_mode = Host2LittleEndian((uint32_t)s->st_mode);
//         res->st_nlink = Host2LittleEndian((uint64_t)s->st_nlink);
//         res->st_uid = Host2LittleEndian((uint32_t)s->st_uid);
//         res->st_gid = Host2LittleEndian((uint32_t)s->st_gid);
//         res->st_rdev = Host2LittleEndian((int64_t)s->st_rdev);
//         res->__pad2 = 0;
//         res->st_size = Host2LittleEndian((int64_t)s->st_size);
//         res->st_blksize = Host2LittleEndian((int64_t)s->st_blksize);
//         res->st_blocks = Host2LittleEndian((int64_t)s->st_blocks);
//         res->st_atim.tv_sec = Host2LittleEndian((int64_t)s->st_atim.tv_sec);
//         res->st_atim.tv_nsec = Host2LittleEndian((int64_t)s->st_atim.tv_nsec);
//         res->st_mtim.tv_sec = Host2LittleEndian((int64_t)s->st_mtim.tv_sec);
//         res->st_mtim.tv_nsec = Host2LittleEndian((int64_t)s->st_mtim.tv_nsec);
//         res->st_ctim.tv_sec = Host2LittleEndian((int64_t)s->st_ctim.tv_sec);
//         res->st_ctim.tv_nsec = Host2LittleEndian((int64_t)s->st_ctim.tv_nsec);
// #else
//         res->st_dev = Host2LittleEndian((uint64_t)s->st_dev);
//         res->st_ino = Host2LittleEndian((uint32_t)s->st_ino);
//         res->st_mode = Host2LittleEndian((uint32_t)s->st_mode);
//         res->st_nlink = Host2LittleEndian((uint32_t)s->st_nlink);
//         res->st_uid = Host2LittleEndian((uint32_t)s->st_uid);
//         res->st_gid = Host2LittleEndian((uint32_t)s->st_gid);
//         res->st_rdev = Host2LittleEndian((int64_t)s->st_rdev);
//         res->__pad2 = 0;
//         res->st_size = Host2LittleEndian((int64_t)s->st_size);
//         res->st_blksize = Host2LittleEndian((int32_t)s->st_blksize);
//         res->st_blocks = Host2LittleEndian((int64_t)s->st_blocks);
//         res->st_atim.tv_sec = Host2LittleEndian((int32_t)s->st_atim.tv_sec);
//         res->st_atim.tv_nsec = Host2LittleEndian((int32_t)s->st_atim.tv_nsec);
//         res->st_mtim.tv_sec = Host2LittleEndian((int32_t)s->st_mtim.tv_sec);
//         res->st_mtim.tv_nsec = Host2LittleEndian((int32_t)s->st_mtim.tv_nsec);
//         res->st_ctim.tv_sec = Host2LittleEndian((int32_t)s->st_ctim.tv_sec);
//         res->st_ctim.tv_nsec = Host2LittleEndian((int32_t)s->st_ctim.tv_nsec);
// #endif
//     }
//     res->__unused4 = 0;
//     res->__unused5 = 0;
//     return (void *)res;
// }
/*
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void *
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
ARMConvertStat64(struct stat64 *s) {
	// this implementation supposes that the host is a x86 with __USE_LARGEFILE64
	//   and __USE_MISC
  
	struct arm_stat64_t *res;
  
	res = (arm_stat64_t *)malloc(sizeof(arm_stat64_t));
	if(GetEndianess() == E_BIG_ENDIAN) {
#if defined(__x86_64)
		res->st_dev = Host2Target(endianess, (uint64_t)s->st_dev);
		res->__pad1 = 0;
	    res->__st_ino = Host2Target(endianess, (uint64_t)s->st_ino);
	    res->st_mode = Host2Target(endianess, (uint32_t)s->st_mode);
	    res->st_nlink = Host2Target(endianess, (uint64_t)s->st_nlink);
	    res->st_uid = Host2Target(endianess, (uint32_t)s->st_uid);
	    res->st_gid = Host2Target(endianess, (uint32_t)s->st_gid);
	    res->st_rdev = Host2Target(endianess, (uint64_t)s->st_rdev);
	    res->__pad2 = 0;
	    res->st_size = Host2Target(endianess, (uint64_t)s->st_size);
	    res->st_blksize = Host2Target(endianess, (uint32_t)s->st_blksize);
	    res->st_blocks = Host2Target(endianess, (uint64_t)s->st_blocks);
	    res->st_atim.tv_sec = Host2Target(endianess, (uint64_t)s->st_atim.tv_sec);
	    res->st_atim.tv_nsec = Host2Target(endianess, (uint64_t)s->st_atim.tv_nsec);
	    res->st_mtim.tv_sec = Host2Target(endianess, (uint64_t)s->st_mtim.tv_sec);
	    res->st_mtim.tv_nsec = Host2Target(endianess, (uint64_t)s->st_mtim.tv_nsec);
	    res->st_ctim.tv_sec = Host2Target(endianess, (uint64_t)s->st_ctim.tv_sec);
	    res->st_ctim.tv_nsec = Host2Target(endianess, (uint64_t)s->st_ctim.tv_nsec);
	    res->st_ino = Host2Target(endianess, (uint64_t)s->st_ino);
#else
	    res->st_dev = Host2Target(endianess, (uint64_t)s->st_dev);
	    res->__pad1 = 0;
	    res->__st_ino = Host2Target(endianess, (uint32_t)s->__st_ino);
	    res->st_mode = Host2Target(endianess, (uint32_t)s->st_mode);
	    res->st_nlink = Host2Target(endianess, (uint32_t)s->st_nlink);
	    res->st_uid = Host2Target(endianess, (uint32_t)s->st_uid);
	    res->st_gid = Host2Target(endianess, (uint32_t)s->st_gid);
	    res->st_rdev = Host2Target(endianess, (uint64_t)s->st_rdev);
	    res->__pad2 = 0;
	    res->st_size = Host2Target(endianess, (uint64_t)s->st_size);
	    res->st_blksize = Host2Target(endianess, (uint32_t)s->st_blksize);
	    res->st_blocks = Host2Target(endianess, (uint64_t)s->st_blocks);
	    res->st_atim.tv_sec = Host2Target(endianess, (uint32_t)s->st_atim.tv_sec);
	    res->st_atim.tv_nsec = Host2Target(endianess, (uint32_t)s->st_atim.tv_nsec);
	    res->st_mtim.tv_sec = Host2Target(endianess, (uint32_t)s->st_mtim.tv_sec);
	    res->st_mtim.tv_nsec = Host2Target(endianess, (uint32_t)s->st_mtim.tv_nsec);
	    res->st_ctim.tv_sec = Host2Target(endianess, (uint32_t)s->st_ctim.tv_sec);
	    res->st_ctim.tv_nsec = Host2Target(endianess, (uint32_t)s->st_ctim.tv_nsec);
	    res->st_ino = Host2Target(endianess, (uint64_t)s->st_ino);
#endif
	} else {
#if defined(__x86_64)
	    res->st_dev = Host2LittleEndian((uint64_t)s->st_dev);
	    res->__pad1 = 0;
	    res->__st_ino = Host2LittleEndian((uint64_t)s->st_ino);
	    res->st_mode = Host2LittleEndian((uint32_t)s->st_mode);
	    res->st_nlink = Host2LittleEndian((uint64_t)s->st_nlink);
	    res->st_uid = Host2LittleEndian((uint32_t)s->st_uid);
	    res->st_gid = Host2LittleEndian((uint32_t)s->st_gid);
	    res->st_rdev = Host2LittleEndian((uint64_t)s->st_rdev);
	    res->__pad2 = 0;
	    res->st_size = Host2LittleEndian((uint64_t)s->st_size);
	    res->st_blksize = Host2LittleEndian((uint32_t)s->st_blksize);
	    res->st_blocks = Host2LittleEndian((uint64_t)s->st_blocks);
	    res->st_atim.tv_sec = Host2LittleEndian((uint64_t)s->st_atim.tv_sec);
	    res->st_atim.tv_nsec = Host2LittleEndian((uint64_t)s->st_atim.tv_nsec);
	    res->st_mtim.tv_sec = Host2LittleEndian((uint64_t)s->st_mtim.tv_sec);
	    res->st_mtim.tv_nsec = Host2LittleEndian((uint64_t)s->st_mtim.tv_nsec);
	    res->st_ctim.tv_sec = Host2LittleEndian((uint64_t)s->st_ctim.tv_sec);
	    res->st_ctim.tv_nsec = Host2LittleEndian((uint64_t)s->st_ctim.tv_nsec);
	    res->st_ino = Host2LittleEndian((uint64_t)s->st_ino);
#else
	    res->st_dev = Host2LittleEndian((uint64_t)s->st_dev);
	    res->__pad1 = 0;
	    res->__st_ino = Host2LittleEndian((uint32_t)s->__st_ino);
	    res->st_mode = Host2LittleEndian((uint32_t)s->st_mode);
	    res->st_nlink = Host2LittleEndian((uint32_t)s->st_nlink);
	    res->st_uid = Host2LittleEndian((uint32_t)s->st_uid);
	    res->st_gid = Host2LittleEndian((uint32_t)s->st_gid);
	    res->st_rdev = Host2LittleEndian((uint64_t)s->st_rdev);
	    res->__pad2 = 0;
	    res->st_size = Host2LittleEndian((uint64_t)s->st_size);
	    res->st_blksize = Host2LittleEndian((uint32_t)s->st_blksize);
	    res->st_blocks = Host2LittleEndian((uint64_t)s->st_blocks);
	    res->st_atim.tv_sec = Host2LittleEndian((uint32_t)s->st_atim.tv_sec);
	    res->st_atim.tv_nsec = Host2LittleEndian((uint32_t)s->st_atim.tv_nsec);
	    res->st_mtim.tv_sec = Host2LittleEndian((uint32_t)s->st_mtim.tv_sec);
	    res->st_mtim.tv_nsec = Host2LittleEndian((uint32_t)s->st_mtim.tv_nsec);
	    res->st_ctim.tv_sec = Host2LittleEndian((uint32_t)s->st_ctim.tv_sec);
	    res->st_ctim.tv_nsec = Host2LittleEndian((uint32_t)s->st_ctim.tv_nsec);
	    res->st_ino = Host2LittleEndian((uint64_t)s->st_ino);
#endif
	}
	return (void *)res;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void *
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
PPCConvertStat64(struct stat64 *s) {
    // this implementation supposes that the host is a x86 with __USE_LARGEFILE64
    //   and __USE_MISC
    
	struct powerpc_stat64_t *res;
    
    res = (powerpc_stat64_t *)malloc(sizeof(powerpc_stat64_t));
    if(GetEndianess() == E_BIG_ENDIAN) {
#if defined(__x86_64)
    	res->st_dev = Host2Target(endianess, (uint64_t)s->st_dev);
      	res->st_ino = Host2Target(endianess, (uint64_t)s->st_ino);
      	res->st_mode = Host2Target(endianess, (uint32_t)s->st_mode);
      	res->st_nlink = Host2Target(endianess, (uint64_t)s->st_nlink);
      	res->st_uid = Host2Target(endianess, (uint32_t)s->st_uid);
      	res->st_gid = Host2Target(endianess, (uint32_t)s->st_gid);
      	res->st_rdev = Host2Target(endianess, (int64_t)s->st_rdev);
      	res->__pad2 = 0;
      	res->st_size = Host2Target(endianess, (int64_t)s->st_size);
      	res->st_blksize = Host2Target(endianess, (int32_t)s->st_blksize);
      	res->st_blocks = Host2Target(endianess, (int64_t)s->st_blocks);
      	res->st_atim.tv_sec = Host2Target(endianess, (int64_t)s->st_atim.tv_sec);
      	res->st_atim.tv_nsec = Host2Target(endianess, (int64_t)s->st_atim.tv_nsec);
      	res->st_mtim.tv_sec = Host2Target(endianess, (int64_t)s->st_mtim.tv_sec);
		res->st_mtim.tv_nsec = Host2Target(endianess, (int64_t)s->st_mtim.tv_nsec);
		res->st_ctim.tv_sec = Host2Target(endianess, (int64_t)s->st_ctim.tv_sec);
		res->st_ctim.tv_nsec = Host2Target(endianess, (int64_t)s->st_ctim.tv_nsec);
#else
		res->st_dev = Host2Target(endianess, (uint64_t)s->st_dev);
      	res->st_ino = Host2Target(endianess, (uint32_t)s->__st_ino);
      	res->st_mode = Host2Target(endianess, (uint32_t)s->st_mode);
      	res->st_nlink = Host2Target(endianess, (uint32_t)s->st_nlink);
      	res->st_uid = Host2Target(endianess, (uint32_t)s->st_uid);
      	res->st_gid = Host2Target(endianess, (uint32_t)s->st_gid);
      	res->st_rdev = Host2Target(endianess, (int64_t)s->st_rdev);
      	res->__pad2 = 0;
      	res->st_size = Host2Target(endianess, (int64_t)s->st_size);
      	res->st_blksize = Host2Target(endianess, (int32_t)s->st_blksize);
      	res->st_blocks = Host2Target(endianess, (int64_t)s->st_blocks);
      	res->st_atim.tv_sec = Host2Target(endianess, (int32_t)s->st_atim.tv_sec);
      	res->st_atim.tv_nsec = Host2Target(endianess, (int32_t)s->st_atim.tv_nsec);
      	res->st_mtim.tv_sec = Host2Target(endianess, (int32_t)s->st_mtim.tv_sec);
      	res->st_mtim.tv_nsec = Host2Target(endianess, (int32_t)s->st_mtim.tv_nsec);
      	res->st_ctim.tv_sec = Host2Target(endianess, (int32_t)s->st_ctim.tv_sec);
      	res->st_ctim.tv_nsec = Host2Target(endianess, (int32_t)s->st_ctim.tv_nsec);
#endif
    } else {
#if defined(__x86_64)
    	res->st_dev = Host2LittleEndian((uint64_t)s->st_dev);
      	res->st_ino = Host2LittleEndian((uint64_t)s->st_ino);
      	res->st_mode = Host2LittleEndian((uint32_t)s->st_mode);
      	res->st_nlink = Host2LittleEndian((uint64_t)s->st_nlink);
      	res->st_uid = Host2LittleEndian((uint32_t)s->st_uid);
      	res->st_gid = Host2LittleEndian((uint32_t)s->st_gid);
      	res->st_rdev = Host2LittleEndian((int64_t)s->st_rdev);
      	res->__pad2 = 0;
      	res->st_size = Host2LittleEndian((int64_t)s->st_size);
      	res->st_blksize = Host2LittleEndian((int32_t)s->st_blksize);
      	res->st_blocks = Host2LittleEndian((int64_t)s->st_blocks);
      	res->st_atim.tv_sec = Host2LittleEndian((int64_t)s->st_atim.tv_sec);
		res->st_atim.tv_nsec = Host2LittleEndian((int64_t)s->st_atim.tv_nsec);
		res->st_mtim.tv_sec = Host2LittleEndian((int64_t)s->st_mtim.tv_sec);
		res->st_mtim.tv_nsec = Host2LittleEndian((int64_t)s->st_mtim.tv_nsec);
		res->st_ctim.tv_sec = Host2LittleEndian((int64_t)s->st_ctim.tv_sec);
		res->st_ctim.tv_nsec = Host2LittleEndian((int64_t)s->st_ctim.tv_nsec);
#else
		res->st_dev = Host2LittleEndian((uint64_t)s->st_dev);
		res->st_ino = Host2LittleEndian((uint32_t)s->__st_ino);
		res->st_mode = Host2LittleEndian((uint32_t)s->st_mode);
		res->st_nlink = Host2LittleEndian((uint32_t)s->st_nlink);
		res->st_uid = Host2LittleEndian((uint32_t)s->st_uid);
		res->st_gid = Host2LittleEndian((uint32_t)s->st_gid);
		res->st_rdev = Host2LittleEndian((int64_t)s->st_rdev);
		res->__pad2 = 0;
		res->st_size = Host2LittleEndian((int64_t)s->st_size);
		res->st_blksize = Host2LittleEndian((int32_t)s->st_blksize);
		res->st_blocks = Host2LittleEndian((int64_t)s->st_blocks);
		res->st_atim.tv_sec = Host2LittleEndian((int32_t)s->st_atim.tv_sec);
		res->st_atim.tv_nsec = Host2LittleEndian((int32_t)s->st_atim.tv_nsec);
		res->st_mtim.tv_sec = Host2LittleEndian((int32_t)s->st_mtim.tv_sec);
		res->st_mtim.tv_nsec = Host2LittleEndian((int32_t)s->st_mtim.tv_nsec);
		res->st_ctim.tv_sec = Host2LittleEndian((int32_t)s->st_ctim.tv_sec);
		res->st_ctim.tv_nsec = Host2LittleEndian((int32_t)s->st_ctim.tv_nsec);
#endif
    }
    res->__unused4 = 0;
    res->__unused5 = 0;
    return (void *)res;
}
  
template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void *
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
ARMConvertTms(struct tms *t) {
	struct arm_tms_t *res;
  
	res = (arm_tms_t *)malloc(sizeof(arm_tms_t));
	if(GetEndianess() == E_BIG_ENDIAN) {
		res->tms_utime = Host2Target(endianess, (uint32_t)t->tms_utime);
		res->tms_stime = Host2Target(endianess, (uint32_t)t->tms_stime);
		res->tms_cutime = Host2Target(endianess, (uint32_t)t->tms_cutime);
		res->tms_cstime = Host2Target(endianess, (uint32_t)t->tms_cstime);
	} else {
		res->tms_utime = Host2LittleEndian((uint32_t)t->tms_utime);
		res->tms_stime = Host2LittleEndian((uint32_t)t->tms_stime);
		res->tms_cutime = Host2LittleEndian((uint32_t)t->tms_cutime);
		res->tms_cstime = Host2LittleEndian((uint32_t)t->tms_cstime);
	}
  
	return (void *)res;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void *
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
PPCConvertTms(struct tms *t) {
    struct powerpc_tms_t *res;
    
    res = (powerpc_tms_t *)malloc(sizeof(powerpc_tms_t));
    if(GetEndianess() == E_BIG_ENDIAN) {
		res->tms_utime = Host2Target(endianess, (int32_t)t->tms_utime);
		res->tms_stime = Host2Target(endianess, (int32_t)t->tms_stime);
		res->tms_cutime = Host2Target(endianess, (int32_t)t->tms_cutime);
		res->tms_cstime = Host2Target(endianess, (int32_t)t->tms_cstime);
    } else {
		res->tms_utime = Host2LittleEndian((int32_t)t->tms_utime);
		res->tms_stime = Host2LittleEndian((int32_t)t->tms_stime);
		res->tms_cutime = Host2LittleEndian((int32_t)t->tms_cutime);
		res->tms_cstime = Host2LittleEndian((int32_t)t->tms_cstime);
    }
    
    return (void *)res;
}*/
  

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
ADDRESS_TYPE 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
GetMmapBase() const {
	return mmap_base;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
SetMmapBase(ADDRESS_TYPE base) {
	mmap_base = base;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
ADDRESS_TYPE 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
GetMmapBrkPoint() const {
	return mmap_brk_point;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
SetMmapBrkPoint(ADDRESS_TYPE brk_point) {
	mmap_brk_point = brk_point;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
ADDRESS_TYPE 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
GetBrkPoint() const {
	return brk_point;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
SetBrkPoint(ADDRESS_TYPE brk_point) {
	this->brk_point = brk_point;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
PARAMETER_TYPE
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
GetSystemCallParam(int id) {
	if(system == "arm")
		return ARMGetSystemCallParam(id);
	else
		return PPCGetSystemCallParam(id);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
PARAMETER_TYPE
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
ARMGetSystemCallParam(int id) {
	PARAMETER_TYPE val;
	arm_regs[id]->GetValue(&val);
	return val;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
PARAMETER_TYPE
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
PPCGetSystemCallParam(int id) {
    PARAMETER_TYPE val;
    ppc_regs[id+3]->GetValue(&val);
    return val;
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
SetSystemCallStatus(int ret, bool error) {
	if(system == "arm")
		ARMSetSystemCallStatus(ret, error);
	else
		PPCSetSystemCallStatus(ret, error);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
ARMSetSystemCallStatus(int ret, bool error) {
	PARAMETER_TYPE val = (PARAMETER_TYPE)ret;
	arm_regs[0]->SetValue(&val);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
void 
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
PPCSetSystemCallStatus(int ret, bool error) {
    PARAMETER_TYPE val;
    if(error) {
		ppc_cr->GetValue(&val);
		val |= (1 << 28); // CR0[SO] <- 1
      	ppc_cr->SetValue(&val);
    } else {
		ppc_cr->GetValue(&val);
		val &= ~(1 << 28); // CR0[SO] <- 0
		ppc_cr->SetValue(&val);
    }
    val = (PARAMETER_TYPE)ret;
    ppc_regs[3]->SetValue(&val);
}

template<class ADDRESS_TYPE, class PARAMETER_TYPE>
endian_type
LinuxOS<ADDRESS_TYPE, PARAMETER_TYPE>::
GetEndianess() {
	return endianess;
}
//
//void 
//ARMLinuxOS::Reset() {
//	return cpu_arm_linux_os_import->Reset();
//}
//
//bool 
//ARMLinuxOS::ReadMemory(ADDRESS_TYPE addr, void *buffer, uint32_t size) {
//	return cpu_arm_linux_os_import->ReadMemory(addr, buffer, size);
//}
//
//bool 
//ARMLinuxOS::WriteMemory(ADDRESS_TYPE addr, const void *buffer, uint32_t size) {
//	return cpu_arm_linux_os_import->WriteMemory(addr, buffer, size);
//}
//
} // end of linux_os namespace
} // end of os namespace
} // end of service namespace
} // end of unisim namespace

#undef LOCATION

#endif // __UNISIM_SERVICE_OS_LINUX_OS_LINUX_OS_TCC__
