/*
 *  Copyright (c) 2020,
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

#include <unisim/util/vcd/gtkwave/gtkwave.hh>

#include <cstring>
#include <sstream>
#include <cassert>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
#include <windows.h>
#else
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#endif

namespace unisim {
namespace util {
namespace vcd {
namespace gtkwave {

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
static int next_shmid = 0;
#else
static key_t next_shmkey = 0;
#endif

struct Opaque
{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	HANDLE file_mapping_handle;
	PROCESS_INFORMATION gtkwave_process_information;
#else
	pid_t gtkwave_pid;
#endif
};

template <unsigned int BUFFER_SIZE>
const unsigned int RingBuffer<BUFFER_SIZE>::SIZE;

template <unsigned int BUFFER_SIZE>
RingBuffer<BUFFER_SIZE>::RingBuffer()
	: storage(0)
	, back(0)
	, front(0)
	, free_space(BUFFER_SIZE)
	, debug(false)
	, dbg_log(&std::cerr)
{
}

template <unsigned int BUFFER_SIZE>
void RingBuffer<BUFFER_SIZE>::SetDebugLog(std::ostream& _dbg_log)
{
	dbg_log = &_dbg_log;
}

template <unsigned int BUFFER_SIZE>
void RingBuffer<BUFFER_SIZE>::EnableDebug(bool v)
{
	debug = v;
}

template <unsigned int BUFFER_SIZE>
void RingBuffer<BUFFER_SIZE>::Initialize(uint8_t *_storage)
{
	storage = _storage;
	memset(storage, 0, BUFFER_SIZE);
}

template <unsigned int BUFFER_SIZE>
bool RingBuffer<BUFFER_SIZE>::PopString()
{
	if(!storage[back]) // did GTKWave consumed the string?
	{
		// maybe: if string length that follows is zero then it's certain
		unsigned int lookahead = back;
		
		// skip byte
		if(++lookahead >= BUFFER_SIZE) lookahead = 0;

		// read len[31:24]
		uint32_t len = ((uint32_t) storage[lookahead] << 24);
		if(++lookahead >= BUFFER_SIZE) lookahead = 0;

		// read len[23:16]
		len |= ((uint32_t) storage[lookahead] << 16);
		if(++lookahead >= BUFFER_SIZE) lookahead = 0;

		// read len[15:8]
		len |= ((uint32_t) storage[lookahead] << 8);
		if(++lookahead >= BUFFER_SIZE) lookahead = 0;

		// read len[7:0]
		len |= (uint32_t) storage[lookahead];
		
		if(len == 0)
		{
			// zero length string: it's not a string we've put
			return false;
		}

		// it's a valid string we should have put for GTKWave
		
		if(debug)
		{
			(*dbg_log) << "pop: back=" << back << ",len=" << len << " (front=" << front << ")" << std::endl;
		}

		// skip byte
		if(++back >= BUFFER_SIZE) back = 0;
		
		// mr proper: string length and content
		
		// zeroes len[31:24]
		storage[back] = 0;
		if(++back >= BUFFER_SIZE) back = 0;
		
		// zeroes len[23:16]
		storage[back] = 0;
		if(++back >= BUFFER_SIZE) back = 0;

		// zeroes len[15:8]
		storage[back] = 0;
		if(++back >= BUFFER_SIZE) back = 0;
		
		// zeroes len[7:0]
		storage[back] = 0;
		if(++back >= BUFFER_SIZE) back = 0;
		
		// zeroes string content
		std::size_t l = len;
		do
		{
			storage[back] = 0;
			if(++back >= BUFFER_SIZE) back = 0;
		}
		while(--l);
		
		// there's more space free: consumed byte + 4-byte string length + string content bytes
		free_space += 5 + len;

		if(free_space >= BUFFER_SIZE)
		{
			// a string was poped but there are no more strings to pop as we reach maximum free space
			if(free_space != BUFFER_SIZE)
			{
				(*dbg_log) << "free space is " << free_space << " while it is expected to be " << BUFFER_SIZE << std::endl;
			}
			assert(free_space == BUFFER_SIZE);
			return false;
		}
	
		// a string was poped, and there are probably more
		return true;
	}
	
	// no string poped
	return false;
}

template <unsigned int BUFFER_SIZE>
bool RingBuffer<BUFFER_SIZE>::PushString(const std::string& s)
{
	std::size_t len = s.length();
	
	if(!len) return true;
	
	// check that there is enough space for valid (1 byte), len (4 bytes) and string content (len bytes)
	if((len + 5) > free_space)
	{
		if(debug)
		{
			(*dbg_log) << "no enough free space for a new push" << std::endl;
		}
		return false;
	}

	if(debug)
	{
		(*dbg_log) << "push: front=" << front << ",len=" << len << "(back=" << back << "),s=\"";
		for(std::size_t i = 0; i < len; ++i)
		{
			char c = s[i];
			if(c == '\n') (*dbg_log) << "\\n";
			else (*dbg_log) << c;
		}
		(*dbg_log) << "\"" << std::endl;
	}
	
	// skip byte
	unsigned int valid_pos = front;
	if(++front >= BUFFER_SIZE) front = 0;

	// write len[31:24]
	storage[front] = len >> 24;
	if(++front >= BUFFER_SIZE) front = 0;

	// write len[23:16]
	storage[front] = len >> 16;
	if(++front >= BUFFER_SIZE) front = 0;

	// write len[15:8]
	storage[front] = len >> 8;
	if(++front >= BUFFER_SIZE) front = 0;

	// write len[7:0]
	storage[front] = len;
	if(++front >= BUFFER_SIZE) front = 0;

	// write string content
	std::size_t l = len;
	std::size_t pos = 0;
	do
	{
		storage[front] = s[pos];
		if(++front >= BUFFER_SIZE) front = 0;
	}
	while(++pos, --l);
	free_space -= 5 + len;

	// make string valid for GTKWave
	storage[valid_pos] = 1;
	
	return true;
}

const unsigned int Stream::WAVE_PARTIAL_VCD_RING_BUFFER_SIZE;

StreamBuffer::StreamBuffer(Stream& _stream, std::size_t _max_size)
  : stream(_stream)
  , max_size(_max_size)
  , buffer()
{
}

StreamBuffer::int_type StreamBuffer::overflow(int_type c)
{
	buffer.append(1, c);
	if(!stream.Send(buffer))
	{
		return EOF;
	}
	buffer.clear();
	return c;
}

std::streamsize StreamBuffer::xsputn(const char* s, std::streamsize n)
{
	std::streamsize r = 0;
	if(n)
	{
		std::streamsize m;
		
		do
		{
			m = max_size - buffer.length();
			if(!m)
			{
				if(!stream.Send(buffer))
				{
					break;
				}
				buffer.clear();
				m = max_size;
			}
			
			if(m > n) m = n;
			buffer.append(s, m);
		}
		while(s += m, r += m, n -= m);
	}
  
	return r;
}

int StreamBuffer::sync()
{
	if(buffer.empty()) return 0;
	if(!stream.Send(buffer)) return -1;
	buffer.clear();
	return 0;
}

Stream::Stream(std::streamsize io_buffer_size)
	: log(&std::cout)
	, warn_log(&std::cerr)
	, err_log(&std::cerr)
	, gtkwave_path("gtkwave")
	, kill_gtkwave(false)
	, verbose(false)
	, debug()
	, shared_buffer(0)
	, opaque(new Opaque())
	, shmid(0)
	, started(false)
	, killed(false)
	, sink(true)
	, alive(false)
	, ring_buffer()
	, stream_buffer(*this, io_buffer_size)
{
	rdbuf(&stream_buffer);
}

Stream::~Stream()
{
	Stop();
	
	delete static_cast<Opaque *>(opaque);
}

void Stream::SetLog(std::ostream& _log)
{
	log = &_log;
	ring_buffer.SetDebugLog(_log);
}

void Stream::SetWarningLog(std::ostream& _warn_log)
{
	warn_log = &_warn_log;
}

void Stream::SetErrorLog(std::ostream& _err_log)
{
	err_log = &_err_log;
}

void Stream::SetOption(Option opt, const char *s)
{
	switch(opt)
	{
		case OPT_GTKWAVE_PATH: gtkwave_path = s; break;
		default              : break;
	}
}

void Stream::SetOption(Option opt, const std::string& s)
{
	switch(opt)
	{
		case OPT_GTKWAVE_PATH: gtkwave_path = s; break;
		default              : break;
	}
}

void Stream::SetOption(Option opt, bool flag)
{
	switch(opt)
	{
		case OPT_KILL_GTKWAVE: kill_gtkwave = flag; break;
		case OPT_VERBOSE     : verbose = flag; break;
		case OPT_DEBUG       : debug = flag; ring_buffer.EnableDebug(flag); break;
		default              : break;
	}
}

void Stream::GetOption(Option opt, std::string& s) const
{
	switch(opt)
	{
		case OPT_GTKWAVE_PATH: s = gtkwave_path; break;
		default              : break;
	}
}

void Stream::GetOption(Option opt, bool& flag) const
{
	switch(opt)
	{
		case OPT_KILL_GTKWAVE: flag = kill_gtkwave; break;
		case OPT_VERBOSE     : flag = verbose; break;
		case OPT_DEBUG       : flag = debug; break;
		default              : break;
	}
}

bool Stream::Start()
{
	if(started) return true; // already started
	
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	// windows
	HANDLE& file_mapping_handle = static_cast<Opaque *>(opaque)->file_mapping_handle;
	PROCESS_INFORMATION& gtkwave_process_information = static_cast<Opaque *>(opaque)->gtkwave_process_information;
	
	do
	{
		shmid = next_shmid++;
		std::stringstream file_mapping_name_sstr;
		file_mapping_name_sstr << "shmidcat" << shmid;
		std::string file_mapping_name(file_mapping_name_sstr.str());
		
		// Create file mapping
		file_mapping_handle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, ring_buffer.SIZE, file_mapping_name.c_str());
		if(file_mapping_handle == NULL)
		{
			ErrLog() << "CreateFileMapping failed: " << GetLastErrorString() << std::endl;
			return false;
		}
	}
	while(GetLastError() == ERROR_ALREADY_EXISTS); // Check that CreateFileMapping did not reuse an existing file mapping
	
	// Map shared buffer
	shared_buffer = (uint8_t *) MapViewOfFile(file_mapping_handle, FILE_MAP_ALL_ACCESS, 0, 0, ring_buffer.SIZE);
#else
	// unix/posix
	shmid = shmget(next_shmkey++, ring_buffer.SIZE, IPC_CREAT | S_IRUSR | S_IWUSR);

	if(shmid < 0)
	{
		ErrLog() << "shmget failed: " << GetLastErrorString() << std::endl;
		return false;
	}

	shared_buffer = (uint8_t *) shmat(shmid, NULL, 0);
	
	if(shared_buffer == (uint8_t *) -1)
	{
		ErrLog() << "shmat failed: " << GetLastErrorString() << std::endl;
		// mark the segment for destroy
		shmctl(shmid, IPC_RMID, 0); 
		return false;
	}
#endif
	ring_buffer.Initialize(shared_buffer);

#if defined(linux) || defined(__linux) || defined(__linux__)
	// linux specific
	// mark the segment for destroy after the last process detaches it
	shmctl(shmid, IPC_RMID, 0);
#endif
	
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	// windows specific
	STARTUPINFO startup_info;
    ZeroMemory(&startup_info, sizeof(startup_info));
    startup_info.cb = sizeof(startup_info);
    ZeroMemory(&gtkwave_process_information, sizeof(gtkwave_process_information));

	std::stringstream gtkwave_cmdline_sstr;
	gtkwave_cmdline_sstr << gtkwave_path << " -I " << shmid;
	std::string gtkwave_cmdline_str(gtkwave_cmdline_sstr.str());
	
	char gtkwave_cmdline[gtkwave_cmdline_str.length() + 1];
	gtkwave_cmdline_str.copy(gtkwave_cmdline, gtkwave_cmdline_str.length());
 
    // execute GTKWave
    if(!CreateProcess(NULL,                          // No module name (use command line)
                      gtkwave_cmdline,               // Command line
                      NULL,                          // Process handle not inheritable
                      NULL,                          // Thread handle not inheritable
                      TRUE,                          // Set handle inheritance to TRUE
                      0,                             // No creation flags
                      NULL,                          // Use parent's environment block
                      NULL,                          // Use parent's starting directory 
                      &startup_info,                 // Pointer to STARTUPINFO structure
                      &gtkwave_process_information)) // Pointer to PROCESS_INFORMATION structure
    {
		ErrLog() << "CreateProcess failed: " << GetLastErrorString() << std::endl;
		UnmapViewOfFile(shared_buffer);
		CloseHandle(file_mapping_handle);
        return false;
    }
#else
	// unix/posix
	pid_t& gtkwave_pid = static_cast<Opaque *>(opaque)->gtkwave_pid;
	
	// forking a new child process
	gtkwave_pid = fork();

	if(gtkwave_pid == 0)
	{
		// child process
		
		// execute GTKWave
		std::stringstream shmid_sstr;
		shmid_sstr << std::hex << shmid;
		std::string shmid_str(shmid_sstr.str());
		execlp(gtkwave_path.c_str(), "-v", "-I", shmid_str.c_str(), (char *) 0);
		abort();
	}
	else if(gtkwave_pid < 0)
	{
		ErrLog() << "Failed to fork: " << GetLastErrorString() << std::endl;
		// mark the segment for destroy
		shmctl(shmid, IPC_RMID, 0);
		return false;
	}
#endif

	// check that process has started
	if(PingPeer())
	{
		alive = true;
		sink = false;
		started = true;
	}

	if(verbose || debug)
	{
		Log() << "GTKWave has " << (alive ? "started" : "not started") << std::endl;
	}
	
	return started;
}

bool Stream::Stop()
{
	if(!started) return true; // already stopped
	
	if(alive && kill_gtkwave)
	{
		if(verbose || debug)
		{
			Log() << "Killing GTKWave" << std::endl;
		}

		// kill GTKWave if it was alive (last time we checked) and kill is needed
		bool exited = KillPeer(1000); // 1 second timeout for exiting
		
		if(verbose || debug)
		{
			if(exited)
			{
				Log() << "GTKWave has exited" << std::endl;
			}
			else
			{
				Log() << "GTKWave is still running" << std::endl;
			}
		}
	}
	
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	HANDLE& file_mapping_handle = static_cast<Opaque *>(opaque)->file_mapping_handle;

	// Unmap shared buffer and close file mapping
	UnmapViewOfFile(shared_buffer);
	CloseHandle(file_mapping_handle);
#else
	// mark the segment for destroy
	shmctl(shmid, IPC_RMID, 0);
#endif

	alive = false;
	started = false;
	sink = false;
	
	return true;
}

void Stream::Kill()
{
	killed = true;
}

bool Stream::Killed() const
{
	return killed;
}

bool Stream::Send(const std::string& s)
{
	if(s.empty() || sink) return true; // string is empty or we're acting just like a sink
	
	unsigned int timeout = 10; // when pushing a string failed, initial timeout before retrying is 10 ms
	
	while(!Killed())
	{
		// pop as more as more as possible consumed strings
		while(!Killed() && ring_buffer.PopString());

		// try pushing a string
		if(ring_buffer.PushString(s))
		{
			// string was pushed, return a success to the caller
			return true;
		}
		
		// check that GTKWave process is still alive, if not return and signal failure to the caller
		if(!PingPeer())
		{
			if(verbose || debug)
			{
				Log() << "GTKWave has exited" << std::endl;
			}
			sink = true;
			alive = false;
			return false;
		}
		
		if(debug)
		{
			Log() << "GTKWave is alive but unresponsive, waiting " << timeout << " ms before retrying" << std::endl;
		}
		
		WaitTime(timeout);
		
		// timeout between retries starts at 10 ms and increases progressively to 15, 22, 33, 49, 73, 109, 163, 244 ms, up to 366 ms
		if(timeout < 366) timeout = (timeout * 3) / 2;
	}
	
	// we've been killed
	sink = true;
	
	// string was not sent, return a failure to the caller
	return false;
}

std::string Stream::GetLastErrorString()
{
	std::stringstream err_sstr;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	int err_code = GetLastError();
	err_sstr << "Error #" << err_code << ": ";
	char *err_c_str = 0;
	LPTSTR *lptstr_err_str = NULL;
	if(FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
	              NULL,
	              err_code,
	              0,
	              (LPTSTR) &lptstr_err_str,
	              0,
	              NULL) == 0)
	{
		return err_sstr.str();
	}
#if UNICODE
	size_t error_c_str_length = wcstombs(NULL, lptstr_err_str, 0);
    err_c_str = new char[error_c_str_length + 1];
	memset(err_c_str, 0, error_c_str_length);
    if(wcstombs(err_c_str, lptstr_err_str, error_c_str_length + 1) < 0)
	{
		LocalFree(lptstr_err_str);
		delete[] err_c_str;
		return err_sstr.str();
	}
#else
	err_c_str = (char *) lptstr_err_str;
#endif
	
	err_sstr << err_c_str;
	
	LocalFree(lptstr_err_str);
#if UNICODE
	delete[] err_c_str;
#endif
	
#else
	err_sstr << "Error #" << errno << ": " << strerror(errno);
#endif
	return err_sstr.str();
}

void Stream::WaitTime(unsigned int msec)
{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	if(!Killed())
	{
		Sleep(msec);
	}
#else
	struct timespec tim_req, tim_rem;
	tim_req.tv_sec = msec / 1000;
	tim_req.tv_nsec = 1000000 * (msec % 1000);
	
	while(!Killed())
	{
		int status = nanosleep(&tim_req, &tim_rem);
		
		if(status == 0) break;
		
		if(status != -1) break;
		
		if(errno != EINTR) break;

		tim_req.tv_nsec = tim_rem.tv_nsec;
	}
#endif
}

bool Stream::PingPeer()
{
	bool status = true;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	PROCESS_INFORMATION& gtkwave_process_information = static_cast<Opaque *>(opaque)->gtkwave_process_information;
	DWORD exit_code = 0;
	if(!GetExitCodeProcess(gtkwave_process_information.hProcess, &exit_code) || (exit_code != STILL_ACTIVE))
	{
		status = false;
	}
#else
	pid_t& gtkwave_pid = static_cast<Opaque *>(opaque)->gtkwave_pid;
	int wstatus = 0;
	pid_t wpid = waitpid(gtkwave_pid, &wstatus, WNOHANG);
	if(wpid < 0)
	{
		ErrLog() << "waitpid failed: " << GetLastErrorString() << std::endl;
		status = false;
	}
	else if(wpid > 0)
	{
		if(WIFEXITED(wstatus) || WIFSIGNALED(wstatus))
		{
			status = false;
		}
	}
#endif
	if(debug)
	{
		Log() << "GTKWave is " << (status ? "still" : "not") << " alive" << std::endl;
	}
	return status;
}

bool Stream::KillPeer(unsigned int timeout_msec)
{
	bool status = true;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	PROCESS_INFORMATION& gtkwave_process_information = static_cast<Opaque *>(opaque)->gtkwave_process_information;

	UINT gtkwave_exit_code = 0;
	if(!TerminateProcess(gtkwave_process_information.hProcess, gtkwave_exit_code))
	{
		ErrLog() << "TerminateProcess failed: " << GetLastErrorString() << std::endl;
		status = false;
	}
	
    // Close process and thread handles
    CloseHandle(gtkwave_process_information.hProcess);
    CloseHandle(gtkwave_process_information.hThread);
#else
	pid_t& gtkwave_pid = static_cast<Opaque *>(opaque)->gtkwave_pid;
		
	if(gtkwave_pid > 0)
	{
		if(kill(gtkwave_pid, SIGTERM) == -1)
		{
			ErrLog() << "kill failed: " << GetLastErrorString() << std::endl;
			status = false;
		}
	}
#endif
	if(!status) return false;

	unsigned int i, n = timeout_msec / 10;
	for(i = 0; (i < n) && PingPeer(); i++)
	{
		WaitTime(10);
	}
	
	return i < n;
}

std::ostream& Stream::Log() const
{
	return *log;
}

std::ostream& Stream::WarnLog() const
{
	return *warn_log;
}

std::ostream& Stream::ErrLog() const
{
	return *err_log;
}

} // end of namespace gtkwave
} // end of namespace vcd
} // end of namespace util
} // end of namespace unisim
