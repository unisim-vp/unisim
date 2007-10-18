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
#include <list>

namespace unisim {
namespace util {
namespace debug {

using std::string;
using std::list;

template <class ADDRESS>
class NetStub
{
public:
	typedef enum
	{
		PKC_MIN         =  0,
		PKC_START       =  0, // start
		PKC_STOP        =  1, // stop
		PKC_WRITEREG    =  2, // write CPU register
		PKC_READREG     =  3, // read CPU register
		PKC_WRITE       =  4, // write
		PKC_READ        =  5, // read
		PKC_INTR        =  6, // set interrupt level
		PKC_RUN         =  7, // run
		PKC_SETBRK      =  8, // set breakpoint
		PKC_SETWRITEW   =  9, // set write watchpoint
		PKC_SETREADW    = 10, // set read watchpoint
		PKC_RMBRK       = 11, // remove breakpoint
		PKC_RMWRITEW    = 12, // remove write watchpoint
		PKC_RMREADW     = 13, // remove read watchpoint
		PKC_TRAP        = 14, // trap on breakpoint/watchpoint
		PKC_MAX         = 14
	} PK_COMMAND;

	typedef enum
	{
		TU_MIN = 0,
		TU_FS = 0, // 1e-15 s
		TU_PS = 1, // 1e-12 s
		TU_NS = 2, // 1e-9 s
		TU_US = 3, // 1e-6 s
		TU_MS = 4, // 1e-3 s
		TU_S  = 5, // 1 s
		TU_MAX = 5
	} TIME_UNIT;

	typedef enum
	{
		SP_MIN = 0,
		SP_CPU_MEM = 0, // CPU view of memory.
		                // WARNING! Be aware that MMU may be activated so that effective addresses
		                // are not physical addresses
		SP_DEV_MEM = 1, // Local device memory
		SP_DEV_IO  = 2, // local device I/O space
		SP_MAX = 2
	} SPACE;

	typedef enum
	{
		TRAP_MIN = 0,
		TRAP_BREAKPOINT = 0,
		TRAP_WATCHPOINT = 1,
		NUM_TRAP_TYPES  = 2,
		TRAP_MAX = 2
	} TRAP_TYPE;

	typedef enum
	{
		WATCHPOINT_MIN = 0,
		WATCHPOINT_READ      = 0,
		WATCHPOINT_WRITE     = 1,
		WATCHPOINT_MAX = 1
	} WATCHPOINT_TYPE;

	typedef union
	{
		TRAP_TYPE type;
		struct
		{
			TRAP_TYPE type;
			ADDRESS addr;
		} breakpoint;
		struct
		{
			TRAP_TYPE type;
			WATCHPOINT_TYPE wtype;
			ADDRESS addr;
			uint32_t size;
			SPACE space;
		} watchpoint;
	} TRAP;
	
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
	bool PutWritePacket(ADDRESS addr, uint32_t size, uint8_t *data, SPACE space, const uint32_t device_id = 0, const uint32_t tag = 0);
	bool PutReadPacket(ADDRESS addr, uint32_t size, SPACE space, const uint32_t device_id = 0, const uint32_t tag = 0);
	bool PutWriteRegisterPacket(const char *name, const uint32_t& value, const uint32_t device_id = 0, const uint32_t tag = 0);
	bool PutReadRegisterPacket(const char *name, const uint32_t device_id = 0, const uint32_t tag = 0);
	bool PutIntrPacket(uint32_t intr_id, bool level, const uint32_t device_id = 0, const uint32_t tag = 0);
	bool PutRunPacket(const uint64_t duration = 0, const uint32_t device_id = 0, const uint32_t tag = 0);
	bool PutRunPacket(uint64_t duration, TIME_UNIT tu, const uint32_t device_id = 0, const uint32_t tag = 0);
	bool PutSetBreakpointPacket(ADDRESS addr, const uint32_t device_id = 0, const uint32_t tag = 0);
	bool PutSetBreakpointPacket(const char *symbol_name, const uint32_t device_id = 0, const uint32_t tag = 0);
	bool PutSetReadWatchpointPacket(ADDRESS addr, uint32_t size, SPACE space, const uint32_t device_id = 0, const uint32_t tag = 0);
	bool PutSetWriteWatchpointPacket(ADDRESS addr, uint32_t size, SPACE space, const uint32_t device_id = 0, const uint32_t tag = 0);
	bool PutRemoveBreakpointPacket(ADDRESS addr, const uint32_t device_id = 0, const uint32_t tag = 0);
	bool PutRemoveBreakpointPacket(const char *symbol_name, const uint32_t device_id = 0, const uint32_t tag = 0);
	bool PutRemoveReadWatchpointPacket(ADDRESS addr, uint32_t size, SPACE space, const uint32_t device_id = 0, const uint32_t tag = 0);
	bool PutRemoveWriteWatchpointPacket(ADDRESS addr, uint32_t size, SPACE space, const uint32_t device_id = 0, const uint32_t tag = 0);
	bool PutTrapPacket(uint64_t t, TIME_UNIT tu, const list<TRAP>& traps, const uint32_t device_id = 0, const uint32_t tag = 0);
	bool ParseChar(const string& packet, unsigned int& pos, char c);
	bool ParseString(const string& packet, unsigned int& pos, char separator, string& s);
	bool ParseHex(const string& s, unsigned int& pos, uint32_t& value);
	bool ParseHex(const string& s, unsigned int& pos, uint64_t& value);
	bool ParseHex(const string& s, unsigned int& pos, uint8_t *buffer, uint32_t size);
	bool ParseBool(const string& s, unsigned int& pos, bool& value);
	bool ParseAddress(const string& packet, unsigned int& pos, ADDRESS& addr);
	bool ParseSymbol(const string& packet, unsigned int& pos, ADDRESS& addr, string& symbol_name);
	bool ParseCommand(const string& packet, unsigned int& pos, PK_COMMAND& command);
	bool ParseTimeUnit(const string& packet, unsigned int& pos, TIME_UNIT& tu);
	bool ParseSpace(const string& packet, unsigned int& pos, SPACE& space);
	bool ParseTrapType(const string& packet, unsigned int& pos, TRAP_TYPE& trap_type);
	bool ParseWatchpointType(const string& packet, unsigned int& pos, WATCHPOINT_TYPE& watchpoint_type);
	bool ParseStart(const string& packet, unsigned int& pos, TIME_UNIT& tu);
	bool ParseStop(const string& packet, unsigned int& pos);
	bool ParseRead(const string& packet, unsigned int& pos, ADDRESS& addr, uint32_t& size, SPACE& space);
	bool ParseWrite(const string& packet, unsigned int& pos, ADDRESS& addr, uint32_t& size, uint8_t **data, SPACE& space);
	bool ParseWriteRegister(const string& packet, unsigned int& pos, string& register_name, uint32_t& value);
	bool ParseReadRegister(const string& packet, unsigned int& pos, string& register_name);
	bool ParseIntr(const string& packet, unsigned int& pos, uint32_t& intr_id, bool& level);
	bool ParseRun(const string& packet, unsigned int& pos, uint64_t& duration, TIME_UNIT& tu);
	bool ParseSetBreakpoint(const string& packet, unsigned int& pos, ADDRESS& addr, string& symbol_name);
	bool ParseSetReadWatchpoint(const string& packet, unsigned int& pos, ADDRESS& addr, uint32_t& size, SPACE& space);
	bool ParseSetWriteWatchpoint(const string& packet, unsigned int& pos, ADDRESS& addr, uint32_t& size, SPACE& space);
	bool ParseRemoveBreakpoint(const string& packet, unsigned int& pos, ADDRESS& addr, string& symbol_name);
	bool ParseRemoveReadWatchpoint(const string& packet, unsigned int& pos, ADDRESS& addr, uint32_t& size, SPACE& space);
	bool ParseRemoveWriteWatchpoint(const string& packet, unsigned int& pos, ADDRESS& addr, uint32_t& size, SPACE& space);
	bool ParseTrap(const string& packet, unsigned int& pos, uint64_t& t, TIME_UNIT& tu, list<TRAP>& traps);
private:
	int sock;
	string s_command[PKC_MAX - PKC_MIN + 1];
	string s_tu[TU_MAX - TU_MIN + 1];
	string s_bool[2];
	string s_space[SP_MAX - SP_MIN + 1];
	string s_trap_type[TRAP_MAX - TRAP_MIN + 1];
	string s_watchpoint_type[WATCHPOINT_MAX - WATCHPOINT_MIN + 1];
	TIME_UNIT default_tu;

protected:
	bool is_server;
	string server_name;
	unsigned int tcp_port;
	
	void Step();
	virtual void Start();
	virtual void Stop();
	virtual bool Read(ADDRESS addr, void *data, uint32_t size, SPACE space);
	virtual bool Write(ADDRESS addr, const void *data, uint32_t size, SPACE space);
	virtual bool ReadRegister(const char *name, uint32_t& value);
	virtual bool WriteRegister(const char *name, uint32_t value);
	virtual void Intr(uint32_t intr_id, bool level);
	virtual void Run(uint64_t duration, TIME_UNIT tu);
	virtual bool SetBreakpoint(ADDRESS addr);
	virtual bool SetBreakpoint(const char *symbol_name);
	virtual bool SetReadWatchpoint(ADDRESS addr, uint32_t size, SPACE space);
	virtual bool SetWriteWatchpoint(ADDRESS addr, uint32_t size, SPACE space);
	virtual bool RemoveBreakpoint(ADDRESS addr);
	virtual bool RemoveBreakpoint(const char *symbol_name);
	virtual bool RemoveReadWatchpoint(ADDRESS addr, uint32_t size, SPACE space);
	virtual bool RemoveWriteWatchpoint(ADDRESS addr, uint32_t size, SPACE space);
};

} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
