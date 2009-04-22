/*
 *  Copyright (c) 2009,
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
 *          Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#ifndef __UNISIM_SERVICE_OS_TI_C_IO_TI_C_IO_TCC__
#define __UNISIM_SERVICE_OS_TI_C_IO_TI_C_IO_TCC__

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

#ifdef WIN32
#include <windows.h>
#else
#include <sys/times.h>
#endif
#include "unisim/kernel/service/service.hh"
#include "unisim/kernel/logger/logger.hh"
#include "unisim/service/interfaces/os.hh"
#include "unisim/service/interfaces/memory_injection.hh"
#include "unisim/util/endian/endian.hh"

namespace unisim {
namespace service {
namespace os {
namespace ti_c_io {

using std::string;
using std::stringstream;
using std::hex;
using std::dec;
using std::endl;
using std::flush;
using std::map;
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

template <class MEMORY_ADDR>
TI_C_IO<MEMORY_ADDR>::TI_C_IO(const char *name, Object *parent) :
	Object(name, parent),
	Service<unisim::service::interfaces::OS>(name, parent),
	Client<Memory<MEMORY_ADDR> >(name, parent),
	Client<MemoryInjection<MEMORY_ADDR> >(name, parent),
	Client<SymbolTableLookup<MEMORY_ADDR> >(name, parent),
	os_export("os-export", this),
	memory_import("memory-import", this),
	memory_injection_import("memory-injection-import", this),
	symbol_table_lookup_import("symbol-table-lookup-import", this),
	logger(*this),
	max_buffer_byte_length(0),
	buffer(0),
	verbose(false),
	param_verbose("verbose", this, verbose),
	C_IO_BREAKPOINT_SYMBOL_NAME("C$$IO$$"),
	C_IO_BUFFER_SYMBOL_NAME("__CIOBUF_")
{
	Object::SetupDependsOn(memory_import);
	Object::SetupDependsOn(symbol_table_lookup_import);
}

template <class MEMORY_ADDR>
TI_C_IO<MEMORY_ADDR>::~TI_C_IO()
{
	if(buffer) free(buffer);
}

template <class MEMORY_ADDR>
void TI_C_IO<MEMORY_ADDR>::OnDisconnect()
{
}

template <class MEMORY_ADDR>
bool TI_C_IO<MEMORY_ADDR>::Setup()
{
	if(!memory_injection_import)
	{
		logger << DebugError << "Memory injection import is not connected" << EndDebugError;
		return false;
	}

	if(!symbol_table_lookup_import)
	{
		logger << DebugError << "Symbol table lookup import is not connected" << EndDebugError;
		return false;
	}

	const unisim::util::debug::Symbol<MEMORY_ADDR> *c_io_buffer_symbol = symbol_table_lookup_import->FindSymbolByName(C_IO_BUFFER_SYMBOL_NAME);

	if(!c_io_buffer_symbol)
	{
		logger << DebugError << "Undefined symbol " << C_IO_BUFFER_SYMBOL_NAME << EndDebugError;
		return false;
	}

	const unisim::util::debug::Symbol<MEMORY_ADDR> *c_io_breakpoint_symbol = symbol_table_lookup_import->FindSymbolByName(C_IO_BREAKPOINT_SYMBOL_NAME);

	if(!c_io_breakpoint_symbol)
	{
		logger << DebugError << "Undefined symbol " << C_IO_BREAKPOINT_SYMBOL_NAME << EndDebugError;
		return false;
	}

	c_io_buffer_addr = c_io_buffer_symbol->GetAddress();
	MEMORY_ADDR c_io_breakpoint_addr = c_io_breakpoint_symbol->GetAddress();

	uint8_t swi[4] = { 0x00, 0x00, 0x00, 0x66 };
	if(!memory_import->WriteMemory(c_io_breakpoint_addr, swi, sizeof(swi)))
	{
		logger << DebugError << "Cannot install breakpoint at 0x" << hex << c_io_breakpoint_addr << dec << EndDebugError;
		return false;
	}

	target_to_host_fildes.insert(std::pair<int16_t, int>(0, 0));
	target_to_host_fildes.insert(std::pair<int16_t, int>(1, 1));
	target_to_host_fildes.insert(std::pair<int16_t, int>(2, 2));

	return true;
}

template <class MEMORY_ADDR>
void TI_C_IO<MEMORY_ADDR>::GrowBuffer(uint32_t byte_length)
{
	if(buffer && byte_length > max_buffer_byte_length)
		buffer = (uint8_t *) realloc(buffer, byte_length);
	else
		buffer = (uint8_t *) calloc(byte_length, 1);

	max_buffer_byte_length = byte_length;
}


template <class MEMORY_ADDR>
void TI_C_IO<MEMORY_ADDR>::ExecuteSystemCall()
{
	uint32_t i;
	InputMsg input_msg;
	MEMORY_ADDR addr = c_io_buffer_addr;

	if(!memory_injection_import->InjectReadMemory(addr, &input_msg, sizeof(input_msg))) return;

	// Adapt input message to host endianness
	input_msg.length = unisim::util::endian::LittleEndian2Host(input_msg.length);
	input_msg.command = unisim::util::endian::LittleEndian2Host(input_msg.command);
	for(i = 0; i < 4; i++)
	{
		input_msg.parm[i] = unisim::util::endian::LittleEndian2Host(input_msg.parm[i]);
	}

	GrowBuffer(input_msg.length);

	if(input_msg.length)
	{
		addr += sizeof(input_msg);

		if(!memory_injection_import->InjectReadMemory(addr, buffer, input_msg.length)) return;
	}

	if(verbose)
	{
		logger << DebugInfo << "input msg:" << endl << "  - length=" << input_msg.length << endl << "  - command=0x" << hex << input_msg.command << dec << endl;

		for(i = 0; i < 4; i++)
		{
			logger << "  - parm[" << i << "]=0x" << hex << input_msg.parm[i] << dec << endl;
		}
		
		logger << "  - buffer=";

		if(input_msg.length)
		{
			logger << hex;
			for(i = 0; i < input_msg.length; i++)
			{
				logger << "0x" << (unsigned int) buffer[i];
				if(i != input_msg.length - 1) logger << " ";
			}
			logger << dec;
		}
		else
		{
			logger << "<empty>";
		}
		logger << endl << EndDebugInfo;
	}

	OutputMsg output_msg;
	memset(&output_msg, 0, sizeof(output_msg));

	switch(input_msg.command)
	{
		case C_IO_CMD_OPEN:
			output_msg.parm[0] = c_io_open((const char *) buffer, input_msg.parm[1], input_msg.parm[0]);
			break;
		case C_IO_CMD_CLOSE:
			output_msg.parm[0] = c_io_close(input_msg.parm[0]);
			break;
		case C_IO_CMD_READ:
			GrowBuffer(input_msg.parm[1]);
			output_msg.parm[0] = c_io_read(input_msg.parm[0], (char *) buffer, input_msg.parm[1]);
			output_msg.length = (int16_t) output_msg.parm[0] < 0 ? 0 : output_msg.parm[0];
			break;
		case C_IO_CMD_WRITE:
			output_msg.parm[0] = c_io_write(input_msg.parm[0], (const char *) buffer, input_msg.parm[1]);
			break;
		case C_IO_CMD_LSEEK:
			output_msg.parm[0] = c_io_lseek(input_msg.parm[0], (int32_t) input_msg.parm[1] | ((int32_t) input_msg.parm[2] << 16), input_msg.parm[2]);
			break;
		case C_IO_CMD_UNLINK:
			output_msg.parm[0] = c_io_unlink((const char *) buffer);
			break;
		case C_IO_CMD_GETENV:
			{
				const char *value = c_io_getenv((const char *) buffer);
				if(value)
				{
					size_t len = strlen(value);
					output_msg.parm[0] = 1;
					GrowBuffer(len + 1);
					strcpy((char *) buffer, value);
					output_msg.length = len + 1;
				}
				else
				{
					output_msg.parm[0] = 0;
				}
			}
			break;
		case C_IO_CMD_RENAME:
			{
				const char *oldpath = (const char *) buffer;
				const char *newpath = (const char *) buffer + strlen((const char *) buffer) + 1;
				output_msg.parm[0] = c_io_rename(oldpath, newpath);
			}
			break;
		case C_IO_CMD_GETTIME:
			{
				uint32_t t = c_io_gettime();
				output_msg.parm[0] = t;
				output_msg.parm[1] = t >> 16;
			}
			break;
		case C_IO_CMD_GETCLOCK:
			{
				uint32_t clk = c_io_getclock();
				output_msg.parm[0] = clk;
				output_msg.parm[1] = clk >> 16;
			}
			break;
	}

	// Adapt output message to target endianness
	output_msg.length = unisim::util::endian::Host2LittleEndian(output_msg.length);
	for(i = 0; i < 4; i++)
	{
		output_msg.parm[i] = unisim::util::endian::Host2LittleEndian(output_msg.parm[i]);
	}

	if(verbose)
	{
		logger << DebugInfo << "output msg:" << endl << "  - length=" << output_msg.length << endl;
		for(i = 0; i < 4; i++)
		{
			logger << "  - parm[" << i << "]=0x" << hex << output_msg.parm[i] << dec << endl;
		}
		logger << "  - buffer=";

		if(output_msg.length)
		{
			logger << hex;
			for(i = 0; i < output_msg.length; i++)
			{
				logger << "0x" << (unsigned int) buffer[i];
				if(i != output_msg.length - 1) logger << " ";
			}
			logger << dec;
		}
		else
		{
			logger << "<empty>";
		}
		logger << endl << EndDebugInfo;
	}

	addr = c_io_buffer_addr;

	if(!memory_injection_import->InjectWriteMemory(addr, &output_msg, sizeof(output_msg))) return;

	if(output_msg.length)
	{
		addr += sizeof(output_msg);

		if(!memory_injection_import->InjectWriteMemory(addr, buffer, output_msg.length)) return;
	}
}

template <class MEMORY_ADDR>
int16_t TI_C_IO<MEMORY_ADDR>::c_io_open(const char *path, uint16_t c_io_flags, int16_t fno)
{
	if(verbose)
	{
		logger << DebugInfo << "open:" << endl << "  - path=\"" << path << "\"" << endl << "  - flags=0x" << hex
		       << (c_io_flags & ~(C_IO_O_RDONLY | C_IO_O_WRONLY | C_IO_O_RDWR | C_IO_O_APPEND | C_IO_O_CREAT | C_IO_O_TRUNC | C_IO_O_BINARY)) << dec;
		if(c_io_flags & C_IO_O_RDONLY) logger << " | O_RDONLY";
		if(c_io_flags & C_IO_O_WRONLY) logger << " | O_WRONLY";
		if(c_io_flags & C_IO_O_RDWR) logger << " | O_RDWR";
		if(c_io_flags & C_IO_O_APPEND) logger << " | O_APPEND";
		if(c_io_flags & C_IO_O_CREAT) logger << " | O_CREAT";
		if(c_io_flags & C_IO_O_TRUNC) logger << " | O_TRUNC";
		if(c_io_flags & C_IO_O_BINARY) logger << " | O_BINARY";

		logger << endl << "  - fno=" << fno << EndDebugInfo;
	}

	// Return an error if file description is already opened
	map<int16_t, int>::iterator iter = target_to_host_fildes.find(fno);
	if(iter != target_to_host_fildes.end()) return -1;

	int fd;      // the host file descriptor
	int flags;   // the host flags

	// Translate C I/O flags to host flags
	if(c_io_flags & C_IO_O_RDONLY) flags |= O_RDONLY;
	if(c_io_flags & C_IO_O_WRONLY) flags |= O_WRONLY;
	if(c_io_flags & C_IO_O_RDWR) flags |= O_RDWR;
	if(c_io_flags & C_IO_O_APPEND) flags |= O_APPEND;
	if(c_io_flags & C_IO_O_CREAT) flags |= O_CREAT;
	if(c_io_flags & C_IO_O_TRUNC) flags |= O_TRUNC;
#ifdef WIN32
	if(c_io_flags & C_IO_O_BINARY) flags |= O_BINARY;
#endif

	// Open the file on the host machine
	if(flags & O_CREAT)
	{
		mode_t mode = S_IRWXU;
		fd = open(path, flags, mode);
	}
	else
	{
		fd = open(path, flags);
	}

	// if an error occurs, report an error
	if(fd == -1) return -1;

	// else keep relation between the C I/O file descriptor or the host file descriptor
	target_to_host_fildes.insert(std::pair<int16_t, int>(fno, fd));

	// return the C I/O file descriptor
	return fno;
}

template <class MEMORY_ADDR>
int16_t TI_C_IO<MEMORY_ADDR>::c_io_close(int16_t fno)
{
	int fd;      // the host file descriptor

	if(verbose)
	{
		logger << DebugInfo << "close:" << endl << "  - fno=" << fno << EndDebugInfo;
	}

	// Check whether target file descriptor is stdin, stdout or stderr
	if(fno == 0 || fno == 1 || fno == 2)
	{
		return 0;
	}
	else
	{
		// Return an error if file descriptor does not exist
		map<int16_t, int>::iterator iter = target_to_host_fildes.find(fno);
		if(iter == target_to_host_fildes.end()) return -1;

		// Translate the C I/O file descriptor to an host file descriptor
		fd = (*iter).second;

		target_to_host_fildes.erase(iter);
	}

	// Do not close host stdin, stdout or stderr
	if(fd == 0 || fd == 1 || fd == 2) return 0;

	return close(fd);
}

template <class MEMORY_ADDR>
int16_t TI_C_IO<MEMORY_ADDR>::c_io_read(int16_t fno, char *buf, uint16_t count)
{
	int fd;      // the host file descriptor

	if(verbose)
	{
		logger << DebugInfo << "read:" << endl << "  - fno=" << fno << endl << "  - count=" << count << EndDebugInfo;
	}

	// Return an error if file descriptor does not exist
	map<int16_t, int>::iterator iter = target_to_host_fildes.find(fno);
	if(iter == target_to_host_fildes.end()) return -1;

	// Translate the C I/O file descriptor to an host file descriptor
	fd = (*iter).second;

	return read(fd, buf, count);
}

template <class MEMORY_ADDR>
int16_t TI_C_IO<MEMORY_ADDR>::c_io_write(int16_t fno, const char *buf, uint16_t count)
{
	int fd;      // the host file descriptor

	if(verbose)
	{
		logger << DebugInfo << "write:" << endl << "  - fno=" << fno << endl << "  - count=" << count << EndDebugInfo;
	}

	// Return an error if file descriptor does not exist
	map<int16_t, int>::iterator iter = target_to_host_fildes.find(fno);
	if(iter == target_to_host_fildes.end()) return -1;

	// Translate the C I/O file descriptor to an host file descriptor
	fd = (*iter).second;

	return write(fd, buf, count);
}

template <class MEMORY_ADDR>
int16_t TI_C_IO<MEMORY_ADDR>::c_io_lseek(int16_t fno, int32_t offset, int16_t origin)
{
	int fd;      // the host file descriptor

	if(verbose)
	{
		logger << DebugInfo << "lseek:" << endl << "  - fno=" << fno << endl << "  - offset=" << offset << endl << "  - origin=" << origin << EndDebugInfo;
	}

	// Return an error if file descriptor does not exist
	map<int16_t, int>::iterator iter = target_to_host_fildes.find(fno);
	if(iter == target_to_host_fildes.end()) return -1;

	// Translate the C I/O file descriptor to an host file descriptor
	fd = (*iter).second;

	int whence;

	switch(origin)
	{
		case C_IO_SEEK_SET:
			whence = SEEK_SET;
			break;
		case C_IO_SEEK_CUR:
			whence = SEEK_CUR;
			break;
		case C_IO_SEEK_END:
			whence = SEEK_END;
			break;
	}

	return lseek(fd, offset, whence);
}

template <class MEMORY_ADDR>
int16_t TI_C_IO<MEMORY_ADDR>::c_io_unlink(const char *path)
{
	if(verbose)
	{
		logger << DebugInfo << "unlink:" << endl << "  - path=\"" << path << "\"" << EndDebugInfo;
	}

	return unlink(path);
}

template <class MEMORY_ADDR>
const char *TI_C_IO<MEMORY_ADDR>::c_io_getenv(const char *name)
{
	if(verbose)
	{
		logger << DebugInfo << "getenv:" << endl << "  - name=\"" << name << "\"" << EndDebugInfo;
	}

	return getenv(name);
}

template <class MEMORY_ADDR>
int16_t TI_C_IO<MEMORY_ADDR>::c_io_rename(const char *oldpath, const char *newpath)
{
	if(verbose)
	{
		logger << DebugInfo << "rename:" << endl << "  - oldpath=\"" << oldpath << "\"" << endl << "  - newpath=\"" << newpath << "\"" << EndDebugInfo;
	}

	return rename(oldpath, newpath);
}

template <class MEMORY_ADDR>
uint32_t TI_C_IO<MEMORY_ADDR>::c_io_gettime()
{
	if(verbose)
	{
		logger << DebugInfo << "getime" << EndDebugInfo;
	}

	return time(0);
}

template <class MEMORY_ADDR>
uint32_t TI_C_IO<MEMORY_ADDR>::c_io_getclock()
{
	if(verbose)
	{
		logger << DebugInfo << "getclock" << EndDebugInfo;
	}

	return clock();
}

} // end of namespace ti_c_io
} // end of namespace os
} // end of namespace service
} // end of namespace unisim

#endif // __UNISIM_SERVICE_OS_TI_C_IO_TI_C_IO_HH__
