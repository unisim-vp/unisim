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
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_PPC440_EXCEPTION_HANDLING_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_PPC440_EXCEPTION_HANDLING_TCC__

#include <sstream>
#include <unisim/component/cxx/processor/powerpc/exception.tcc>

#ifdef powerpc
#undef powerpc
#endif

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {
namespace ppc440 {

template <class CONFIG>
int CPU<CONFIG>::StringLength(typename CONFIG::address_t addr)
{
	int len = 0;
	char buffer;

	while(1)
	{
		buffer = 0;
		ReadMemory(addr, &buffer, 1);
		if(buffer == 0) return len;
		len++;
		addr += 1;
	}
}

template <class CONFIG>
std::string CPU<CONFIG>::ReadString(typename CONFIG::address_t addr, unsigned int count)
{
	if(!count)
	{
		count = StringLength(addr);
	}
	
	char *buf = (char *) calloc(count + 1, 1);
	ReadMemory(addr, buf, count);

	std::stringstream sstr;

	bool open_string = false;
	unsigned int i;
	for(i = 0; i <= count; i++)
	{
		char c = buf[i];
		if(c < ' ') // control character
		{
			switch(c)
			{
				case '\n':
				case '\r':
				case '\e':
				case '\a':
				case '\f':
				case '\t':
				case '\v':
				case '\'':
				case '\\':
					if(!open_string)
					{
						sstr << "\"";
						open_string = true;
					}
					switch(c)
					{
						case '\n': sstr << "\\n"; break;
						case '\r': sstr << "\\r"; break;
						case '\e': sstr << "\\e"; break;
						case '\a': sstr << "\\a"; break;
						case '\f': sstr << "\\f"; break;
						case '\t': sstr << "\\t"; break;
						case '\v': sstr << "\\v"; break;
						case '\'': sstr << "\\'"; break;
						case '\\': sstr << "\\\\"; break;
					}
					if(i == count) sstr << "\"";
					break;
				default:
					if(open_string) sstr << "\"";
					sstr << " \\0x" << std::hex << (unsigned int) c << std::dec;
					if((i != count) && open_string) sstr << "\"";
					break;
			}
		}
		else
		{
			if(!open_string)
			{
				sstr << "\"";
				open_string = true;
			}
			sstr << c;
			if(i == count) sstr << "\"";
		}
	}
		
	free(buf);
	
	return sstr.str();
}

/* System call exception */
template <class CONFIG>
void CPU<CONFIG>::HandleException(const SystemCallException<CONFIG>& exc, unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation)
{
	num_interrupts++;
	
	if(unlikely(IsVerboseException() || enable_trap_on_exception || linux_os_import))
	{
		std::stringstream sstr;
		sstr << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) sstr << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		sstr << ", got a " << exc.what();

		if(unlikely(enable_linux_syscall_snooping))
		{
			sstr << ": ";
			
			switch(GetGPR(0))
			{
				case 1:
					sstr << "exit(0x" << std::hex << GetGPR(3) << std::dec << ");";
					break;
					
				case 2:
					sstr << "fork()";
					break;
			
				case 3:
					sstr << "read(fd=" << (signed) GetGPR(3) << ",buf=@0x" << std::hex << GetGPR(4) << std::dec
							<< ",count=" << GetGPR(5) << ");";
					break;
			
				case 4:
					{
						int fd; 
		
						fd = GetGPR(3);
						
						sstr << "write(fd=" << (signed) GetGPR(3) << ",buf=@0x" << std::hex << GetGPR(4) << std::dec;
						if((fd == 1) || (fd == 2))
						{
							std::string str = ReadString(GetGPR(4), GetGPR(5));
							sstr << " " << str;
						}
						
						sstr << ",count=" << GetGPR(5) << ");";
					}
					break;
					
				case 5:
					sstr << "open(pathname=" << ReadString(GetGPR(3)) << ",flags=0x" << std::hex << GetGPR(4) 
								<< ",mode=0x" << GetGPR(5) << std::dec << ");";
					break;
				
				case 6:
					sstr << "close(fd=" << (signed) GetGPR(3) << ");";
					break;
					
				case 7:
					sstr << "waitpid(pid=" << (int32_t) GetGPR(3) << ",status=@0x" << std::hex << GetGPR(4) << std::dec << ",options=0x" << std::hex << GetGPR(5) << std::dec << ");";
					break;
					
				case 8:
					sstr << "creat(filename=" << ReadString(GetGPR(3)) << ",mode=0x" << std::hex << GetGPR(4) << std::dec << ");";
					break;
					
				case 9:
					sstr << "link(old_path=" << ReadString(GetGPR(3)) << ",new_path=" << ReadString(GetGPR(4)) << ");";
					break;
					
				case 10:
					sstr << "unlink(path=" << ReadString(GetGPR(3)) << ");";
					break;

				case 11:
					sstr << "execve(filename=" << ReadString(GetGPR(3)) << ",argv=@0x" << std::hex << GetGPR(4) << ",envp=@0x" << GetGPR(5) << std::dec << ");";
					break;
					
				case 12:
					sstr << "chdir(path=" << ReadString(GetGPR(3)) << ");";
					break;
					
				case 13:
					sstr << "time(t=@0x" << std::hex << GetGPR(3) << std::dec << ");";
					break;

				case 14:
					sstr << "mknod(pathname=" << ReadString(GetGPR(3)) << ",mode=0x" << std::hex << GetGPR(4) << ",dev=0x" << GetGPR(5) << std::dec << ");";
					break;
					
				case 15:
					sstr << "chmod(pathname=" << ReadString(GetGPR(3)) << ",mode=0x" << std::hex << GetGPR(4) << std::dec << ");";
					break;
				
				case 16:
					sstr << "lchown(path=" << ReadString(GetGPR(3)) << ",owner=" << GetGPR(4) << ",group=" << GetGPR(5) << ");";
					break;
					
				case 18:
					sstr << "oldstat(path=" << ReadString(GetGPR(3)) << ",buf=@0x" << std::hex << GetGPR(4) << std::dec << ");";
					break;
					
				case 19:
					sstr << "lseek(fd=" << (signed) GetGPR(3) << ",offset=" << (signed) GetGPR(4) << ",whence=" << (signed) GetGPR(5) << ");";
					break;
					
				case 20:
					sstr << "getpid();";
					break;
					
				case 21:
					sstr << "mount(source=" << ReadString(GetGPR(3)) << ",target=" << ReadString(GetGPR(4)) << ",filesystemtype="
							<< ReadString(GetGPR(5)) << ",mountflags=0x" << std::hex << GetGPR(6) << ",data=@0x"
							<< GetGPR(7) << std::dec << ");";
					break;
				
				case 22:
					sstr << "umount(target=" << ReadString(GetGPR(4)) << ");";
					break;
					
				case 23:
					sstr << "setuid(uid=" << GetGPR(3) << ");";
					break;
					
				case 24:
					sstr << "getuid();";
					break;
					
				case 25:
					sstr << "stime(t=@" << std::hex << GetGPR(3) << std::dec << ");";
					break;
				
				case 26:
					sstr << "ptrace(request=" << GetGPR(3) << ",pid=" << GetGPR(4) << ",addr=@" << std::hex << GetGPR(5) << std::dec << ",data=@" << std::hex << GetGPR(6) << std::dec << ");";
					break;

				case 27:
					sstr << "alarm(seconds=" << GetGPR(3) << ");";
					break;

				case 28:
					sstr << "oldfstat(fd=" << (signed) GetGPR(3) << ",buf=@0x" << std::hex << GetGPR(4) << std::dec << ");";
					break;

				case 29:
					sstr << "pause();";
					break;
					
				case 30:
					sstr << "utime(filename=" << ReadString(GetGPR(3)) << ",times=@0x" << std::hex << GetGPR(4) << std::dec << ");";
					break;

				case 33:
					sstr << "access(pathname=" << ReadString(GetGPR(3))
						<< ",mode=0x" << std::hex << GetGPR(4) << std::dec << ");";
					break;
					
				case 34:
					sstr << "nice(inc=" << (signed) GetGPR(3) << ");";
					break;

				case 35:
					sstr << "ftime(tp=@" << std::hex << GetGPR(3) << std::dec << ");";
					break;
					
				case 36:
					sstr << "sync();";
					break;
					
				case 37:
					sstr << "kill(pid=" << GetGPR(3) << ",sig=" << (signed) GetGPR(4) << ");";
					break;
					
				case 38:
					sstr << "rename(oldpath=" << ReadString(GetGPR(3)) << ",newpath=" << ReadString(GetGPR(4)) << ");";
					break;

				case 39:
					sstr << "mkdir(pathname=" << ReadString(GetGPR(3)) << ",mode=0x" << std::hex << GetGPR(4) << std::dec << ");";
					break;
					
				case 40:
					sstr << "rmdir(pathname=" << ReadString(GetGPR(3)) << ");";
					break;
					
				case 41:
					sstr << "dup(oldfd=" << (signed) GetGPR(3) << ");";
					break;
				
				case 42:
					sstr << "pipe(pipefd=@" << std::hex << GetGPR(3) << std::dec << ");";
					break;
					
				case 43:
					sstr << "times(buf=@" << std::hex << GetGPR(3) << std::dec << ");";
					break;
					
				case 45:
					sstr << "brk(end_data_segment=@0x" << std::hex << GetGPR(3) << std::dec << ");";
					break;
					
				case 46:
					sstr << "setgid(gid=" << GetGPR(3) << ");";
					break;
				
				case 47:
					sstr << "getgid();";
					break;
					
				case 48:
					sstr << "signal(signum=" << (signed) GetGPR(3) << ",handler=@" << std::hex << GetGPR(4) << std::dec << ");";
					break;
					
				case 49:
					sstr << "geteuid();";
					break;
					
				case 50:
					sstr << "getegid();";
					break;
					
				case 51:
					sstr << "acct(filename=" << ReadString(GetGPR(3)) << ");";
					break;
					
				case 52:
					sstr << "umount2(target=" << ReadString(GetGPR(3)) << ",flags=" << (signed) GetGPR(4) << ");";
					break;
					
				case 54:
					sstr << "ioctl(d=" << (signed) GetGPR(3) << ",request=" << (signed) GetGPR(4) << ",argp=@" << std::hex << GetGPR(5) << std::dec << ");";
					break;
				
				case 55:
					sstr << "fcntl(fd=" << (signed) GetGPR(3) << ",cmd=0x" << std::hex << GetGPR(4) << std::dec << ",arg=@0x" << std::hex << GetGPR(5) << std::dec << ");";
					break;
					
				case 57:
					sstr << "setpgid(pid=" << GetGPR(3) << ",pgid=" << GetGPR(4) << ");";
					break;
					
				case 58:
					sstr << "ulimit(cmd=" << (signed) GetGPR(3) << ",newlimit=" << (signed) GetGPR(4) << ");";
					break;

				case 59:
					sstr << "oldolduname(buf=@0x" << std::hex << GetGPR(3) << std::dec << ");";
					break;

				case 60:
					sstr << "umask(mask=0" << std::oct << GetGPR(3) << std::dec << ");";
					break;
					
				case 61:
					sstr << "chroot(path=" << ReadString(GetGPR(3)) << ");";
					break;
					
				case 62:
					sstr << "ustat(dev=" << GetGPR(3) << ",ubuf=@" << GetGPR(4) << ");";
					break;
					
				case 63:
					sstr << "dup2(old=" << (signed) GetGPR(3) << ",new=" << (signed) GetGPR(4) << ");";
					break;
					
				case 64:
					sstr << "getppid();";
					break;
				
				case 65:
					sstr << "getpgrp();";
					break;
					
				case 66:
					sstr << "setsid();";
					break;
					
				case 78:
					sstr << "gettimeofday(tv=@" << GetGPR(3) << ",tz=@" << GetGPR(4) << ");";
					break;
					
				case 88:
					sstr << "reboot(magic=" << (signed) GetGPR(3) << ",magic2=" << (signed) GetGPR(4) << ",cmd=" << (signed) GetGPR(5) << ",arg=@0x" << std::hex << GetGPR(6) << std::dec << ");";
					break;
					
				case 90:
					sstr << "mmap(addr=@0x" << std::hex << GetGPR(3) << std::dec << ",length=" << GetGPR(4) << ",prot=0x"
							<< std::hex << GetGPR(5) << ",flags=0x" << GetGPR(6) << std::dec << ",fd=" << (signed) GetGPR(7)
							<< ",offset=" << GetGPR(8) << ");";
					break;
					
				case 91:
					sstr << "munmap(addr=@0x" << std::hex << GetGPR(3) << std::dec << ",length=" << GetGPR(4) << ");";
					break;
					
				case 102:
					sstr << "socketcall(call=" << (signed) GetGPR(3) << ",args=@0x" << GetGPR(4) << ");";
					break;
					
				case 106:
					sstr << "stat(path=" << ReadString(GetGPR(3)) << ",buf=@0x" << std::hex << GetGPR(4) << std::dec << ");";
					break;
					
				case 114:
					sstr << "wait4(pid=" << GetGPR(3) << ",status=@0x" << std::hex << GetGPR(4) << ",options=0x" << GetGPR(5) << ",rusage=@0x" << GetGPR(6) << std::dec << ");";
					break;
				
				case 120:
					sstr << "clone(fn=0x" << std::hex << GetGPR(3) << ",child_stack=0x" << GetGPR(4) << ",flags=0x" << GetGPR(5) << ",arg=@0x" << GetGPR(6) << std::dec << ");";
					break;
					
				case 122:
					sstr << "uname(buf=@0x" << std::hex << GetGPR(3) << std::dec << ");";
					break;
					
				case 140:
					sstr << "llseek(fd=" << (signed) GetGPR(3) << ",offset_high=" << GetGPR(4) << ",offset_low=" << GetGPR(5) << ",result=@0x" << GetGPR(6) << ",whence=" << GetGPR(7) << ");";
					break;
					
				case 141:
					sstr << "getdents(fd=" << (signed) GetGPR(3) << ",dirp=@0x" << std::hex << GetGPR(4) << std::dec << ",count=" << GetGPR(5) << ");";
					break;
					
				case 146:
					sstr << "writev(fd=" << (signed) GetGPR(3) << ",iov=@0x" << std::hex << GetGPR(4) << std::dec << ",iovcnt=" << (signed) GetGPR(5) << ");";
					break;
					
				case 162:
					sstr << "nanosleep(req=@" << std::hex << GetGPR(3) << std::dec << ",rem=@" << std::hex << GetGPR(4) << std::dec << ");";
					break;
					
				case 167:
					sstr << "poll(fds=@" << std::hex << GetGPR(3) << std::dec << ",nfds=" << GetGPR(4) << ",timeout=" << (signed) GetGPR(5) << ");";
					break;
					
				case 173:
					sstr << "rt_sigaction(signum=" << (signed) GetGPR(3) << ",act=@0x" << std::hex << GetGPR(4) << ",oldact=@0x" << GetGPR(5) << std::dec << ",sigsetsize=" << GetGPR(6) << ");";
					break;
					
				case 174:
					sstr << "rt_sigprocmask(how=" << (signed) GetGPR(3) << ",set=@0x" << std::hex << GetGPR(4) << ",oldset=@0x" << GetGPR(5) << std::dec << ",sigsetsize=" << GetGPR(6) << ");";
					break;
					
				case 182:
					sstr << "getcwd(buf=@0x" << std::hex << GetGPR(3) << std::dec << ",size=" << GetGPR(4) << ");";
					break;
					
				case 189:
					sstr << "vfork();";
					break;
					
				case 195:
					sstr << "stat64(path=" << ReadString(GetGPR(3)) << ",buf=@0x" << std::hex << GetGPR(4) << std::dec << ");";
					break;
					
				case 196:
					sstr << "lstat64(path=" << ReadString(GetGPR(3)) << ",buf=@0x" << std::hex << GetGPR(4) << std::dec << ");";
					break;
					
				case 197:
					sstr << "fstat64(fd=" << (signed) GetGPR(3) << ",buf=@0x" << std::hex << GetGPR(4) << std::dec << ");";
					break;
					
				case 202:
					sstr << "getdents64(fd=" << (signed) GetGPR(3) << ",dirp=@0x" << std::hex << GetGPR(4) << std::dec << ",count=" << GetGPR(5) << ");";
					break;
					
				case 204:
					sstr << "fcntl64(fd=" << (signed) GetGPR(3) << ",cmd=0x" << std::hex << GetGPR(4) << std::dec << ",arg=@0x" << std::hex << GetGPR(5) << std::dec << ");";
					break;
					
				case 207:
					sstr << "gettid();";
					break;
					
				case 208:
					sstr << "tkill(tid=" << (signed) GetGPR(3) << ",sig=" << (signed) GetGPR(4) << ");";
					break;

				case 234:
					sstr << "exit_group(" << (signed) GetGPR(3) << ");";
					break;
					
				case 250:
					sstr << "tgkill(tgid=" << (signed) GetGPR(3) << ",tid=" << (signed) GetGPR(4) << ",sig=" << (signed) GetGPR(5) << ");";
					break;
					
				default:
					sstr << "unknown_syscall_" << GetGPR(0) << "();";
					break;
			}
		}
		
		std::string msg = sstr.str();
		
		if(IsVerboseException()) logger << DebugInfo << msg << EndDebugInfo;
		//if(enable_trap_on_exception && trap_reporting_import) trap_reporting_import->ReportTrap(*this, msg.c_str());
	}

	if(linux_os_import)
	{
		// Do Linux ABI translation
		linux_os_import->ExecuteSystemCall(GetGPR(0));
	}
	else
	{
		// Invalidate shadow TLBs
		InvalidateITLB();
		InvalidateDTLB();
		
		SetSRR0(GetNIA()); // effective address of the instruction after the system call instruction
		
		SetSRR1(GetMSR()); // content of MSR
		
		SetMSR(GetMSR() & (CONFIG::MSR_CE_MASK | CONFIG::MSR_DE_MASK | CONFIG::MSR_ME_MASK)); // MSR[CE], MSR[DE] and MSR[ME] unchanged, all other MSR bits set to 0
		
		Branch(GetIVPR() | GetIVOR(CONFIG::IVOR_SYSTEM_CALL));
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const MachineCheckException<CONFIG>& exc, unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation)
{
	num_interrupts++;

	// Invalidate shadow TLBs
	InvalidateITLB();
	InvalidateDTLB();

	switch(exc.GetType())
	{
		case MachineCheckException<CONFIG>::MCP_INSTRUCTION_SYNCHRONOUS:
			break;
			SetMCSRR0(GetCIA()); // effective address of the instruction presenting the exception
		case MachineCheckException<CONFIG>::MCP_INSTRUCTION_ASYNCHRONOUS:
		case MachineCheckException<CONFIG>::MCP_DATA_ASYNCHRONOUS:
		case MachineCheckException<CONFIG>::MCP_TLB_ASYNCHRONOUS:
			SetMCSRR0(GetNIA()); // effective address of the next instruction to be executed
			break;
	}
	
	SetMCSRR1(GetMSR()); // content of MSR
	
	SetMSR(0); //  all MSR bits set to 0
	
	Branch(GetIVPR() | GetIVOR(CONFIG::IVOR_MACHINE_CHECK));

	if(unlikely(IsVerboseException() || enable_trap_on_exception || linux_os_import))
	{
		std::stringstream sstr;
		sstr << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) sstr << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		sstr << ", got a " << exc.what();
		std::string msg = sstr.str();

		if(IsVerboseException()) logger << DebugInfo << msg << EndDebugInfo;
		if(enable_trap_on_exception && trap_reporting_import)
		{
			trap_reporting_import->ReportTrap(*this, msg.c_str());
		}
		else if(linux_os_import)
		{
			logger << DebugError << msg << EndDebugError;
			Object::Stop(-1);
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const DecrementerInterruptException<CONFIG>& exc, unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation)
{
	num_interrupts++;

	// Invalidate shadow TLBs
	InvalidateITLB();
	InvalidateDTLB();

	SetSRR0(GetNIA()); // effective address of the next instruction to be executed
	
	SetSRR1(GetMSR()); // content of MSR
	
	SetMSR(GetMSR() & (CONFIG::MSR_CE_MASK | CONFIG::MSR_DE_MASK | CONFIG::MSR_ME_MASK)); // MSR[CE], MSR[DE] and MSR[ME] unchanged, all other MSR bits set to 0
	
	Branch(GetIVPR() | GetIVOR(CONFIG::IVOR_DECREMENTER));
	
	ResetIRQ(CONFIG::IRQ_DECREMENTER_INTERRUPT);

	if(unlikely(IsVerboseException() || enable_trap_on_exception || linux_os_import))
	{
		std::stringstream sstr;
		sstr << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) sstr << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		sstr << ", got a " << exc.what();
		std::string msg = sstr.str();

		if(IsVerboseException()) logger << DebugInfo << msg << EndDebugInfo;
		if(enable_trap_on_exception && trap_reporting_import)
		{
			trap_reporting_import->ReportTrap(*this, msg.c_str());
		}
		else if(linux_os_import)
		{
			logger << DebugError << msg << EndDebugError;
			Object::Stop(-1);
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const ExternalInputInterruptException<CONFIG>& exc, unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation)
{
	num_interrupts++;

	// Invalidate shadow TLBs
	InvalidateITLB();
	InvalidateDTLB();

	SetSRR0(GetNIA()); // effective address of the next instruction to be executed
	
	SetSRR1(GetMSR()); // content of MSR
	
	SetMSR(GetMSR() & (CONFIG::MSR_CE_MASK | CONFIG::MSR_DE_MASK | CONFIG::MSR_ME_MASK)); // MSR[CE], MSR[DE] and MSR[ME] unchanged, all other MSR bits set to 0
	
	Branch(GetIVPR() | GetIVOR(CONFIG::IVOR_EXTERNAL_INPUT));

	//ResetIRQ(CONFIG::IRQ_EXTERNAL_INPUT_INTERRUPT);

	if(unlikely(IsVerboseException() || enable_trap_on_exception || linux_os_import))
	{
		std::stringstream sstr;
		sstr << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) sstr << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		sstr << ", got a " << exc.what();
		std::string msg = sstr.str();

		if(IsVerboseException()) logger << DebugInfo << msg << EndDebugInfo;
		if(enable_trap_on_exception && trap_reporting_import)
		{
			trap_reporting_import->ReportTrap(*this, msg.c_str());
		}
		else if(linux_os_import)
		{
			logger << DebugError << msg << EndDebugError;
			Object::Stop(-1);
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const CriticalInputInterruptException<CONFIG>& exc, unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation)
{
	num_interrupts++;

	// Invalidate shadow TLBs
	InvalidateITLB();
	InvalidateDTLB();

	SetCSRR0(GetNIA()); // effective address of the next instruction to be executed
	
	SetCSRR1(GetMSR()); // content of MSR
	
	SetMSR(GetMSR() & CONFIG::MSR_ME_MASK); // MSR[ME] unchanged, all other MSR bits set to 0
	
	Branch(GetIVPR() | GetIVOR(CONFIG::IVOR_CRITICAL_INPUT));

	//ResetIRQ(CONFIG::IRQ_CRITICAL_INPUT_INTERRUPT);

	if(unlikely(IsVerboseException() || enable_trap_on_exception || linux_os_import))
	{
		std::stringstream sstr;
		sstr << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) sstr << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		sstr << ", got a " << exc.what();
		std::string msg = sstr.str();

		if(IsVerboseException()) logger << DebugInfo << msg << EndDebugInfo;
		if(enable_trap_on_exception && trap_reporting_import)
		{
			trap_reporting_import->ReportTrap(*this, msg.c_str());
		}
		else if(linux_os_import)
		{
			logger << DebugError << msg << EndDebugError;
			Object::Stop(-1);
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const DSIException<CONFIG>& exc, unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation)
{
	num_interrupts++;

	// Invalidate shadow TLBs
	InvalidateITLB();
	InvalidateDTLB();

	SetSRR0(GetCIA()); // effective address of the instruction causing the exception
	
	SetSRR1(GetMSR()); // content of MSR
	
	SetMSR(GetMSR() & (CONFIG::MSR_CE_MASK | CONFIG::MSR_DE_MASK | CONFIG::MSR_ME_MASK)); // MSR[CE], MSR[DE] and MSR[ME] unchanged, all other MSR bits set to 0
	
	SetDEAR(exc.GetAddress()); // effective address causing the esception

	// leave ESR[MCI] unmodified
	// alter ESR[FP], ESR[ST], ESR[DLK], ESR[AP] and ESR[BO]
	uint32_t esr_value = (GetESR() & CONFIG::ESR_MCI_MASK) | 
		(operation->get_esr() & (CONFIG::ESR_FP_MASK | CONFIG::ESR_ST_MASK | CONFIG::ESR_DLK_MASK | CONFIG::ESR_AP_MASK | CONFIG::ESR_BO_MASK));
		
	SetESR(esr_value);

	Branch(GetIVPR() | GetIVOR(CONFIG::IVOR_DATA_STORAGE));

	if(unlikely(IsVerboseException() || enable_trap_on_exception || linux_os_import))
	{
		std::stringstream sstr;
		sstr << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *func_symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(func_symbol) sstr << " (" << func_symbol->GetFriendlyName(GetCIA()) << ")";
		sstr << ", got a " << exc.what() << " while accessing data at 0x" << std::hex << exc.GetAddress() << std::dec;
		const Symbol<typename CONFIG::address_t> *obj_symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_OBJECT) : 0;
		if(obj_symbol) sstr << " (" << obj_symbol->GetFriendlyName(exc.GetAddress()) << ")";
		std::string msg = sstr.str();

		if(IsVerboseException()) logger << DebugInfo << msg << EndDebugInfo;
		if(enable_trap_on_exception && trap_reporting_import)
		{
			trap_reporting_import->ReportTrap(*this, msg.c_str());
		}
		else if(linux_os_import)
		{
			logger << DebugError << msg << EndDebugError;
			Object::Stop(-1);
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const ISIException<CONFIG>& exc, unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation)
{
	num_interrupts++;

	// Invalidate shadow TLBs
	InvalidateITLB();
	InvalidateDTLB();

	SetSRR0(GetCIA()); // effective address of the instruction causing the exception
	
	SetSRR1(GetMSR()); // content of MSR
	
	SetMSR(GetMSR() & (CONFIG::MSR_CE_MASK | CONFIG::MSR_DE_MASK | CONFIG::MSR_ME_MASK)); // MSR[CE], MSR[DE] and MSR[ME] unchanged, all other MSR bits set to 0
	
	SetESR(GetESR() & CONFIG::ESR_MCI_MASK); // ESR[MCI] unchanged, all other bits are set to 0
	
	Branch(GetIVPR() | GetIVOR(CONFIG::IVOR_INSTRUCTION_STORAGE));

	if(unlikely(IsVerboseException() || enable_trap_on_exception || linux_os_import))
	{
		std::stringstream sstr;
		sstr << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) sstr << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		sstr << ", got a " << exc.what();
		std::string msg = sstr.str();

		if(IsVerboseException()) logger << DebugInfo << msg << EndDebugInfo;
		if(enable_trap_on_exception && trap_reporting_import)
		{
			trap_reporting_import->ReportTrap(*this, msg.c_str());
		}
		else if(linux_os_import)
		{
			logger << DebugError << msg << EndDebugError;
			Object::Stop(-1);
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const DataTLBErrorException<CONFIG>& exc, unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation)
{
	num_interrupts++;

	// Invalidate shadow TLBs
	InvalidateITLB();
	InvalidateDTLB();

	SetSRR0(GetCIA()); // effective address of the instruction causing the exception
	
	SetSRR1(GetMSR()); // content of MSR
	
	SetMSR(GetMSR() & (CONFIG::MSR_CE_MASK | CONFIG::MSR_DE_MASK | CONFIG::MSR_ME_MASK)); // MSR[CE], MSR[DE] and MSR[ME] unchanged, all other MSR bits set to 0
	
	SetDEAR(exc.GetAddress()); // effective address causing the esception

	// leave ESR[MCI] unmodified
	// alter ESR[FP], ESR[ST], ESR[AP]
	uint32_t esr_value = (GetESR() & CONFIG::ESR_MCI_MASK) | (operation->get_esr() & (CONFIG::ESR_FP_MASK | CONFIG::ESR_ST_MASK | CONFIG::ESR_AP_MASK));
	SetESR(esr_value);
	
	Branch(GetIVPR() | GetIVOR(CONFIG::IVOR_DATA_TLB_ERROR));

	if(unlikely(IsVerboseException() || enable_trap_on_exception || linux_os_import))
	{
		std::stringstream sstr;
		sstr << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *func_symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(func_symbol) sstr << " (" << func_symbol->GetFriendlyName(GetCIA()) << ")";
		sstr << ", got a " << exc.what() << " while accessing data at 0x" << std::hex << exc.GetAddress() << std::dec;
		const Symbol<typename CONFIG::address_t> *obj_symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_OBJECT) : 0;
		if(obj_symbol) sstr << " (" << obj_symbol->GetFriendlyName(exc.GetAddress()) << ")";
		std::string msg = sstr.str();

		if(IsVerboseException()) logger << DebugInfo << msg << EndDebugInfo;
		if(enable_trap_on_exception && trap_reporting_import)
		{
			trap_reporting_import->ReportTrap(*this, msg.c_str());
		}
		else if(linux_os_import)
		{
			logger << DebugError << msg << EndDebugError;
			Object::Stop(-1);
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const InstructionTLBErrorException<CONFIG>& exc, unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation)
{
	num_interrupts++;

	// Invalidate shadow TLBs
	InvalidateITLB();
	InvalidateDTLB();

	SetSRR0(GetCIA()); // effective address of the instruction causing the exception
	
	SetSRR1(GetMSR()); // content of MSR
	
	SetMSR(GetMSR() & (CONFIG::MSR_CE_MASK | CONFIG::MSR_DE_MASK | CONFIG::MSR_ME_MASK)); // MSR[CE], MSR[DE] and MSR[ME] unchanged, all other MSR bits set to 0
	
	Branch(GetIVPR() | GetIVOR(CONFIG::IVOR_INSTRUCTION_TLB_ERROR));

	if(unlikely(IsVerboseException() || enable_trap_on_exception || linux_os_import))
	{
		std::stringstream sstr;
		sstr << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) sstr << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		sstr << ", got a " << exc.what();
		std::string msg = sstr.str();

		if(IsVerboseException()) logger << DebugInfo << msg << EndDebugInfo;
		if(enable_trap_on_exception && trap_reporting_import)
		{
			trap_reporting_import->ReportTrap(*this, msg.c_str());
		}
		else if(linux_os_import)
		{
			logger << DebugError << msg << EndDebugError;
			Object::Stop(-1);
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const AlignmentException<CONFIG>& exc, unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation)
{
	num_interrupts++;

	// Invalidate shadow TLBs
	InvalidateITLB();
	InvalidateDTLB();

	SetSRR0(GetCIA()); // effective address of the instruction causing the exception
	
	SetSRR1(GetMSR()); // content of MSR
	
	SetMSR(GetMSR() & (CONFIG::MSR_CE_MASK | CONFIG::MSR_DE_MASK | CONFIG::MSR_ME_MASK)); // MSR[CE], MSR[DE] and MSR[ME] unchanged, all other MSR bits set to 0
	
	SetDEAR(exc.GetAddress()); // effective address causing the esception

	uint32_t esr_value = operation->get_esr() & (CONFIG::ESR_FP_MASK | CONFIG::ESR_ST_MASK | CONFIG::ESR_AP_MASK);
	SetESR(esr_value);
	
	Branch(GetIVPR() | GetIVOR(CONFIG::IVOR_ALIGNMENT));

	if(unlikely(IsVerboseException() || enable_trap_on_exception || linux_os_import))
	{
		std::stringstream sstr;
		sstr << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) sstr << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		sstr << ", got a " << exc.what();
		std::string msg = sstr.str();

		if(IsVerboseException()) logger << DebugInfo << msg << EndDebugInfo;
		if(enable_trap_on_exception && trap_reporting_import)
		{
			trap_reporting_import->ReportTrap(*this, msg.c_str());
		}
		else if(linux_os_import)
		{
			logger << DebugError << msg << EndDebugError;
			Object::Stop(-1);
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const ProgramException<CONFIG>& exc, unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation)
{
	num_interrupts++;

	// Invalidate shadow TLBs
	InvalidateITLB();
	InvalidateDTLB();

	SetSRR0(GetCIA()); // effective address of the instruction causing the exception
	
	SetSRR1(GetMSR()); // content of MSR
	
	SetMSR(GetMSR() & (CONFIG::MSR_CE_MASK | CONFIG::MSR_DE_MASK | CONFIG::MSR_ME_MASK)); // MSR[CE], MSR[DE] and MSR[ME] unchanged, all other MSR bits set to 0
	
	uint32_t esr_value = operation->get_esr() & (CONFIG::ESR_PIL_MASK | CONFIG::ESR_PPR_MASK | CONFIG::ESR_PTR_MASK | CONFIG::ESR_PUO_MASK
	                                           | CONFIG::ESR_FP_MASK | CONFIG::ESR_AP_MASK | CONFIG::ESR_PIE_MASK | CONFIG::ESR_PCRE_MASK
	                                           | CONFIG::ESR_PCMP_MASK | CONFIG::ESR_PCRF_MASK);
	switch(exc.GetType())
	{
		case ProgramException<CONFIG>::PX_ILLEGAL_INSTRUCTION:
			esr_value |= CONFIG::ESR_PIL_MASK;
			break;
		case ProgramException<CONFIG>::PX_PRIVILEGE_VIOLATION:
			esr_value |= CONFIG::ESR_PPR_MASK;
			break;
		case ProgramException<CONFIG>::PX_TRAP:
			esr_value |= CONFIG::ESR_PTR_MASK;
			break;
		case ProgramException<CONFIG>::PX_FLOATING_POINT:
			break;
		case ProgramException<CONFIG>::PX_UNIMPLEMENTED_INSTRUCTION:
			esr_value |= CONFIG::ESR_PUO_MASK;
			break;
	}
	
	SetESR(esr_value);
	
	Branch(GetIVPR() | GetIVOR(CONFIG::IVOR_PROGRAM));
	
	if(unlikely(IsVerboseException() || enable_trap_on_exception || linux_os_import))
	{
		std::stringstream sstr;
		sstr << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) sstr << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		sstr << ", got a " << exc.what();
		std::string msg = sstr.str();

		if(IsVerboseException()) logger << DebugInfo << msg << EndDebugInfo;
		if(enable_trap_on_exception && trap_reporting_import)
		{
			trap_reporting_import->ReportTrap(*this, msg.c_str());
		}
		else if(linux_os_import)
		{
			logger << DebugError << msg << EndDebugError;
			Object::Stop(-1);
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const FloatingPointUnavailableException<CONFIG>& exc, unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation)
{
	num_interrupts++;

	// Invalidate shadow TLBs
	InvalidateITLB();
	InvalidateDTLB();

	SetSRR0(GetCIA()); // effective address of the instruction causing the exception
	
	SetSRR1(GetMSR()); // content of MSR
	
	SetMSR(GetMSR() & (CONFIG::MSR_CE_MASK | CONFIG::MSR_DE_MASK | CONFIG::MSR_ME_MASK)); // MSR[CE], MSR[DE] and MSR[ME] unchanged, all other MSR bits set to 0
	
	Branch(GetIVPR() | GetIVOR(CONFIG::IVOR_FLOATING_POINT_UNAVAILABLE));

	if(unlikely(IsVerboseException() || enable_trap_on_exception || linux_os_import))
	{
		std::stringstream sstr;
		sstr << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) sstr << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		sstr << ", got a " << exc.what();
		std::string msg = sstr.str();

		if(IsVerboseException()) logger << DebugInfo << msg << EndDebugInfo;
		if(enable_trap_on_exception && trap_reporting_import)
		{
			trap_reporting_import->ReportTrap(*this, msg.c_str());
		}
		else if(linux_os_import)
		{
			logger << DebugError << msg << EndDebugError;
			Object::Stop(-1);
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const AuxiliaryProcessorUnavailableException<CONFIG>& exc, unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation)
{
	num_interrupts++;

	// Invalidate shadow TLBs
	InvalidateITLB();
	InvalidateDTLB();

	SetSRR0(GetCIA()); // effective address of the instruction causing the exception
	
	SetSRR1(GetMSR()); // content of MSR
	
	SetMSR(GetMSR() & (CONFIG::MSR_CE_MASK | CONFIG::MSR_DE_MASK | CONFIG::MSR_ME_MASK)); // MSR[CE], MSR[DE] and MSR[ME] unchanged, all other MSR bits set to 0
	
	Branch(GetIVPR() | GetIVOR(CONFIG::IVOR_AUXILIARY_PROCESSOR_UNAVAILABLE));

	if(unlikely(IsVerboseException() || enable_trap_on_exception || linux_os_import))
	{
		std::stringstream sstr;
		sstr << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) sstr << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		sstr << ", got a " << exc.what();
		std::string msg = sstr.str();

		if(IsVerboseException()) logger << DebugInfo << msg << EndDebugInfo;
		if(enable_trap_on_exception && trap_reporting_import)
		{
			trap_reporting_import->ReportTrap(*this, msg.c_str());
		}
		else if(linux_os_import)
		{
			logger << DebugError << msg << EndDebugError;
			Object::Stop(-1);
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const FixedIntervalTimerInterruptException<CONFIG>& exc, unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation)
{
	num_interrupts++;

	// Invalidate shadow TLBs
	InvalidateITLB();
	InvalidateDTLB();

	SetSRR0(GetNIA()); // effective address of the next instruction to be executed
	
	SetSRR1(GetMSR()); // content of MSR
	
	SetMSR(GetMSR() & (CONFIG::MSR_CE_MASK | CONFIG::MSR_DE_MASK | CONFIG::MSR_ME_MASK)); // MSR[CE], MSR[DE] and MSR[ME] unchanged, all other MSR bits set to 0
	
	Branch(GetIVPR() | GetIVOR(CONFIG::IVOR_FIXED_INTERVAL_TIMER));

	ResetIRQ(CONFIG::IRQ_FIXED_INTERVAL_TIMER_INTERRUPT);

	if(unlikely(IsVerboseException() || enable_trap_on_exception || linux_os_import))
	{
		std::stringstream sstr;
		sstr << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) sstr << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		sstr << ", got a " << exc.what();
		std::string msg = sstr.str();

		if(IsVerboseException()) logger << DebugInfo << msg << EndDebugInfo;
		if(enable_trap_on_exception && trap_reporting_import)
		{
			trap_reporting_import->ReportTrap(*this, msg.c_str());
		}
		else if(linux_os_import)
		{
			logger << DebugError << msg << EndDebugError;
			Object::Stop(-1);
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const WatchDogTimerInterruptException<CONFIG>& exc, unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation)
{
	num_interrupts++;

	// Invalidate shadow TLBs
	InvalidateITLB();
	InvalidateDTLB();

	SetCSRR0(GetNIA()); // effective address of the next instruction to be executed
	
	SetCSRR1(GetMSR()); // content of MSR
	
	SetMSR(GetMSR() & CONFIG::MSR_ME_MASK); // MSR[ME] unchanged, all other MSR bits set to 0
	
	Branch(GetIVPR() | GetIVOR(CONFIG::IVOR_WATCHDOG_TIMER));

	ResetIRQ(CONFIG::IRQ_WATCHDOG_TIMER_INTERRUPT);

	if(unlikely(IsVerboseException() || enable_trap_on_exception || linux_os_import))
	{
		std::stringstream sstr;
		sstr << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) sstr << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		sstr << ", got a " << exc.what();
		std::string msg = sstr.str();

		if(IsVerboseException()) logger << DebugInfo << msg << EndDebugInfo;
		if(enable_trap_on_exception && trap_reporting_import)
		{
			trap_reporting_import->ReportTrap(*this, msg.c_str());
		}
		else if(linux_os_import)
		{
			logger << DebugError << msg << EndDebugError;
			Object::Stop(-1);
		}
	}
}

template <class CONFIG>
void CPU<CONFIG>::HandleException(const DebugInterruptException<CONFIG>& exc, unisim::component::cxx::processor::powerpc::ppc440::Operation<CONFIG> *operation)
{
	num_interrupts++;

	// Invalidate shadow TLBs
	InvalidateITLB();
	InvalidateDTLB();

	SetCSRR0(GetCIA()); // CHECKME
	
	SetCSRR1(GetMSR()); // content of MSR
	
	SetMSR(GetMSR() & CONFIG::MSR_ME_MASK); // MSR[ME] unchanged, all other MSR bits set to 0
	
	Branch(GetIVPR() | GetIVOR(CONFIG::IVOR_DEBUG));

	if(unlikely(IsVerboseException() || enable_trap_on_exception || linux_os_import))
	{
		std::stringstream sstr;
		sstr << "At 0x" << std::hex << GetCIA() << std::dec;
		const Symbol<typename CONFIG::address_t> *symbol = symbol_table_lookup_import ? symbol_table_lookup_import->FindSymbolByAddr(GetCIA(), Symbol<typename CONFIG::address_t>::SYM_FUNC) : 0;
		if(symbol) sstr << " (" << symbol->GetFriendlyName(GetCIA()) << ")";
		sstr << ", got a " << exc.what();
		std::string msg = sstr.str();

		if(IsVerboseException()) logger << DebugInfo << msg << EndDebugInfo;
		if(enable_trap_on_exception && trap_reporting_import)
		{
			trap_reporting_import->ReportTrap(*this, msg.c_str());
		}
		else if(linux_os_import)
		{
			logger << DebugError << msg << EndDebugError;
			Object::Stop(-1);
		}
	}
}

} // end of namespace ppc440
} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
