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

#ifndef __UNISIM_UTIL_VCD_GTKWAVE_GTKWAVE_HH__
#define __UNISIM_UTIL_VCD_GTKWAVE_GTKWAVE_HH__

#include <string>
#include <iostream>

namespace unisim {
namespace util {
namespace vcd {
namespace gtkwave {

typedef enum
{
	OPT_GTKWAVE_PATH,  // option for path to GTKWave executable launched at call of Method Start
	OPT_KILL_GTKWAVE,  // option to enable killing GTKWave at call of Method Stop or destructor
	OPT_VERBOSE,       // option to enable minimal verbosity
	OPT_DEBUG          // option to enable debug messages
} Option;

class Stream;

template <unsigned int BUFFER_SIZE>
class RingBuffer
{
public:
	static const unsigned int SIZE = BUFFER_SIZE;
	
	RingBuffer();
	void SetDebugLog(std::ostream& dbg_log);
	void EnableDebug(bool v = true);
	void Initialize(uint8_t *_storage);
	bool PopString();
	bool PushString(const std::string& s);
private:
	uint8_t *storage;
	unsigned int back;
	unsigned int front;
	unsigned int free_space;
	bool debug;
	std::ostream *dbg_log;
};

struct StreamBuffer : std::streambuf
{
	StreamBuffer(Stream& stream, std::size_t max_size);

protected:
	virtual int_type overflow(int_type c);
	virtual std::streamsize xsputn(const char* s, std::streamsize n);
	virtual int sync();
private:
	Stream& stream;
	unsigned int max_size;
	std::string buffer;

	void reserve(std::streamsize n);
};

class Stream : public std::ostream
{
public:
	// the value below shall match the value defined in GTKWave source code File "src/vcd_partial.c"
	static const unsigned int WAVE_PARTIAL_VCD_RING_BUFFER_SIZE = 1024 * 1024;
	
	Stream(std::streamsize io_buffer_size = 8192);
	virtual ~Stream();
	void SetLog(std::ostream& log);
	void SetWarningLog(std::ostream& warn_log);
	void SetErrorLog(std::ostream& err_log);
	void SetOption(Option opt, const char *s);
	void SetOption(Option opt, const std::string& s);
	void SetOption(Option opt, bool flag);
	void GetOption(Option opt, std::string& s) const;
	void GetOption(Option opt, bool& flag) const;
	bool Start();
	bool Stop();
	void Kill();
	bool Killed() const;
private:
	friend class StreamBuffer;
	
	std::ostream *log;
	std::ostream *warn_log;
	std::ostream *err_log;
	std::string gtkwave_path;// option OPT_GTKWAVE_PATH: path to GTKWave executable launched at call of Method Start
	bool kill_gtkwave;       // option OPT_KILL_GTKWAVE: enable killing GTKWave at call of Method Stop or destructor
	bool verbose;            // option OPT_VERBOSE: enable minimal verbosity
	bool debug;              // option OPT_DEBUG: enable debug messages
	uint8_t *shared_buffer;  // pointer to the shared buffer with GTKWave
	void *opaque;            // system dependent data
	int shmid;               // shared memory identifier of the shared buffer
	bool started;            // flag started (not related to whether GTKWave really started or not)
	bool killed;             // flag killed: set by Kill, to avoid blocking
	bool sink;               // flag sink: when it is true, the stream acts as a sink (no peer GTKWave)
	bool alive;              // flag alive: true if GTKWave was alive last time we checked
	RingBuffer<WAVE_PARTIAL_VCD_RING_BUFFER_SIZE> ring_buffer;   // ring buffer in shared memory
	StreamBuffer stream_buffer; // I/O buffer of our stream

	bool Send(const std::string& s);
	std::string GetLastErrorString();
	void WaitTime(unsigned int msec);
	bool PingPeer();
	bool KillPeer(unsigned int timeout_msec);
	std::ostream& Log() const;
	std::ostream& WarnLog() const;
	std::ostream& ErrLog() const;
};

} // end of namespace gtkwave
} // end of namespace vcd
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_VCD_GTKWAVE_GTKWAVE_HH__
