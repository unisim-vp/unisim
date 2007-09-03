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

#ifndef __UNISIM_UTIL_DEBUG_NETSTUB_HH__
#define __UNISIM_UTIL_DEBUG_NETSTUB_HH__

#include <inttypes.h>
#include <string>

namespace unisim {
namespace util {
namespace debug {

using std::string;

template <class ADDRESS>
class NetStub
{
public:
	typedef enum { PKC_START = 0, PKC_STOP = 1, PKC_WRITE = 2, PKC_READ = 3, PKC_INTR = 4, PKC_RUN = 5, PKC_SETBRK = 6, PKC_RMBRK = 7, PKC_TRAP = 8 } PK_COMMAND;
	typedef enum { TU_FS = 0, TU_PS = 1, TU_NS = 2, TU_US = 3, TU_MS = 4, TU_S = 5 } TIME_UNIT;
	
	NetStub(const char *server_name, unsigned int tcp_port, bool is_server);
	virtual ~NetStub();

	bool Initialize();
	bool GetChar(char& c, bool blocking);
	bool PutChar(char c);
	bool GetPacket(string& s, bool blocking, uint32_t& device_id, uint32_t& tag);
	bool PutPacket(const string& s, const uint32_t device_id = 0, const uint32_t tag = 0);
	bool PutHex(uint32_t value);
	bool PutStartPacket(const TIME_UNIT tu = TU_MS, const uint32_t device_id = 0, const uint32_t tag = 0);
	bool PutStopPacket(const uint32_t device_id = 0, const uint32_t tag = 0);
	bool PutWritePacket(ADDRESS addr, uint32_t size, uint8_t *data, const uint32_t device_id = 0, const uint32_t tag = 0);
	bool PutReadPacket(ADDRESS addr, uint32_t size, const uint32_t device_id = 0, const uint32_t tag = 0);
	bool PutIntrPacket(uint32_t intr_id, const uint32_t device_id = 0, const uint32_t tag = 0);
	bool PutRunPacket(const uint64_t duration = 0, const uint32_t device_id = 0, const uint32_t tag = 0);
	bool PutRunPacket(uint64_t duration, TIME_UNIT tu, const uint32_t device_id = 0, const uint32_t tag = 0);
	bool PutSetBreakpointPacket(ADDRESS addr, const uint32_t device_id = 0, const uint32_t tag = 0);
	bool PutSetBreakpointPacket(const char *symbol_name, const uint32_t device_id = 0, const uint32_t tag = 0);
	bool PutRemoveBreakpointPacket(ADDRESS addr, const uint32_t device_id = 0, const uint32_t tag = 0);
	bool PutRemoveBreakpointPacket(const char *symbol_name, const uint32_t device_id = 0, const uint32_t tag = 0);
	bool PutTrapPacket(uint64_t t, TIME_UNIT tu, const uint32_t device_id = 0, const uint32_t tag = 0);
	bool ParseChar(const string& packet, unsigned int& pos, char c);
	bool ParseString(const string& packer, unsigned int& pos, char separator, string& s);
	bool ParseHex(const string& s, unsigned int& pos, uint32_t& value);
	bool ParseHex(const string& s, unsigned int& pos, uint64_t& value);
	bool ParseHex(const string& s, unsigned int& pos, uint8_t *buffer, uint32_t size);
	bool ParseAddress(const string& packet, unsigned int& pos, ADDRESS& addr);
	bool ParseSymbol(const string& packet, unsigned int& pos, ADDRESS& addr, string& symbol_name);
	bool ParseCommand(const string& packet, unsigned int& pos, PK_COMMAND& command);
	bool ParseTimeUnit(const string& packet, unsigned int& pos, TIME_UNIT& tu);
	bool ParseStart(const string& packet, unsigned int& pos, TIME_UNIT& tu);
	bool ParseStop(const string& packet, unsigned int& pos);
	bool ParseRead(const string& packet, unsigned int& pos, ADDRESS& addr, uint32_t& size);
	bool ParseWrite(const string& packet, unsigned int& pos, ADDRESS& addr, uint32_t& size, uint8_t **data);
	bool ParseIntr(const string& packet, unsigned int& pos, uint32_t& intr_id);
	bool ParseRun(const string& packet, unsigned int& pos, uint64_t& duration, TIME_UNIT& tu);
	bool ParseSetBreakpoint(const string& packet, unsigned int& pos, ADDRESS& addr, string& symbol_name);
	bool ParseRemoveBreakpoint(const string& packet, unsigned int& pos, ADDRESS& addr, string& symbol_name);
	bool ParseTrap(const string& packet, unsigned int& pos, uint64_t& t, TIME_UNIT& tu);
private:
	int sock;
	string s_command_start;
	string s_command_stop;
	string s_command_read;
	string s_command_write;
	string s_command_intr;
	string s_command_run;
	string s_command_setbrk;
	string s_command_rmbrk;
	string s_command_trap;
	string s_tu_fs;
	string s_tu_ps;
	string s_tu_ns;
	string s_tu_us;
	string s_tu_ms;
	string s_tu_s;
	TIME_UNIT default_tu;

protected:
	bool is_server;
	string server_name;
	unsigned int tcp_port;
	
	void Step();
	virtual void Start();
	virtual void Stop();
	virtual bool Read(ADDRESS addr, void *data, uint32_t size);
	virtual bool Read(const char *symbol_name, void *data, uint32_t size);
	virtual bool Write(ADDRESS addr, const void *data, uint32_t size);
	virtual bool Write(const char *symbol_name, const void *data, uint32_t size);
	virtual void Intr(uint32_t intr_id);
	virtual void Run(uint64_t duration, TIME_UNIT tu);
	virtual bool SetBreakpoint(ADDRESS addr);
	virtual bool SetBreakpoint(const char *symbol_name);
	virtual bool RemoveBreakpoint(ADDRESS addr);
	virtual bool RemoveBreakpoint(const char *symbol_name);
};

} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
