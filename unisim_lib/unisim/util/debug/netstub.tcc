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

#ifndef __UNISIM_UTIL_DEBUG_NETSTUB_TCC__
#define __UNISIM_UTIL_DEBUG_NETSTUB_TCC__

#include <iostream>
#include <sstream>
#include <sys/types.h>

#ifdef WIN32
#include <winsock2.h>
#else

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/times.h>
#include <fcntl.h>

#endif

#include <unistd.h>
#include <errno.h>

namespace unisim {
namespace util {
namespace debug {

using namespace std;
	
template <class ADDRESS>
NetStub<ADDRESS>::NetStub(const char *_server_name, unsigned int _tcp_port, bool _is_server) :
	is_server(_is_server),
	server_name(_server_name),
	tcp_port(_tcp_port),
	sock(-1),
	s_command_start("start"),
	s_command_stop("stop"),
	s_command_read("read"),
	s_command_write("write"),
	s_command_write_register("writereg"),
	s_command_read_register("readreg"),
	s_command_run("run"),
	s_command_intr("intr"),
	s_command_setbrk("setbrk"),
	s_command_set_write_watchpoint("setwritew"),
	s_command_set_read_watchpoint("setreadw"),
	s_command_rmbrk("rmbrk"),
	s_command_remove_write_watchpoint("rmwritew"),
	s_command_remove_read_watchpoint("rmreadw"),
	s_command_trap("trap"),
	s_tu_fs("fs"),
	s_tu_ps("ps"),
	s_tu_ns("ns"),
	s_tu_us("us"),
	s_tu_ms("ms"),
	s_tu_s("s"),
	s_true("true"),
	s_false("false"),
	s_cpu_mem("cpu_mem"),
	s_dev_mem("dev_mem"),
	s_dev_io("dev_io"),
	s_trap_breakpoint("breakpoint"),
	s_trap_watchpoint("watchpoint"),
	s_watchpoint_read("read"),
	s_watchpoint_write("write"),
	default_tu(TU_MS)
{
}

template <class ADDRESS>
bool NetStub<ADDRESS>::Initialize()
{
	sock = -1;
	
	if(is_server)
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: Starting as server" << endl;
#endif
		struct sockaddr_in addr;
		int server_sock;
	
		server_sock = socket(AF_INET, SOCK_STREAM, 0);
		
		if(server_sock < 0)
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: socket failed" << endl;
#endif
			return false;
		}
		
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(tcp_port);
		addr.sin_addr.s_addr = INADDR_ANY;
		if(bind(server_sock, (struct sockaddr *) &addr, sizeof(addr)) < 0)
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: bind failed" << endl;
#endif
#ifdef WIN32
			closesocket(server_sock);
#else
			close(server_sock);
#endif
			return false;
		}
	
		if(listen(server_sock, 1))
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: listen failed" << endl;
#endif
#ifdef WIN32
			closesocket(sock);
#else
			close(server_sock);
#endif
			return false;
		}
	
#ifdef WIN32
		int addr_len;
#else
		socklen_t addr_len;
#endif

#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: Waiting for client connection on TCP port " << tcp_port << endl;
#endif
		
		addr_len = sizeof(addr);
		sock = accept(server_sock, (struct sockaddr *) &addr, &addr_len);
		
		if(sock < 0)
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: accept failed" << endl;
#endif
#ifdef WIN32
			closesocket(server_sock);
#else
			close(server_sock);
#endif
			return false;
		}
		
#ifdef WIN32
		closesocket(server_sock);
#else
		close(server_sock);
#endif
	}
	else
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: Starting as client" << endl;
#endif
		bool connected = false;
		struct sockaddr_in sonadr;
		sock = socket(PF_INET, SOCK_STREAM, 0);
		if(sock < 0)
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: Can't create socket for connection to " << server_name << ":" << tcp_port << endl;
#endif
			return false;
		}
		memset((char *) &sonadr, 0, sizeof(sonadr));
		sonadr.sin_family = AF_INET;
		sonadr.sin_port = htons(tcp_port);
		sonadr.sin_addr.s_addr = inet_addr(server_name.c_str());
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: Trying to connect to " << server_name.c_str() << endl;
#endif
		if(sonadr.sin_addr.s_addr != -1)
		{
			//host format is xxx.yyy.zzz.ttt
			connected = connect(sock, (struct sockaddr *) &sonadr, sizeof(sonadr)) != -1;
		}
		else
		{
			//host format is www.whereitis.gnu need to ask dns
			struct hostent *hp;
			int i = 0;
			hp = gethostbyname(server_name.c_str());
			if(!hp)
			{
#ifdef DEBUG_NETSTUB
				cerr << "NETSTUB: Was not able to determine IP address from host name for" << server_name << ":" << tcp_port << endl;
#endif
			}
			else
			{
				while(!connected && hp->h_addr_list[i] != NULL)
				{
					memcpy((char *) &sonadr.sin_addr,
					(char *) hp->h_addr_list[i],
					sizeof(sonadr.sin_addr));
					connected = connect(sock, (struct sockaddr *) &sonadr, sizeof(sonadr)) != -1;
					i++;
				}
			}
		}
		if(!connected)
		{
#ifdef WIN32
			closesocket(sock);
#else
			close(sock);
#endif
			sock = -1;
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: Can't connect to " << server_name << ":" << tcp_port << endl;
#endif
		}
	}
	
	if(sock >= 0)
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: Connection established" << endl;
#endif
		
#ifdef WIN32
		u_long NonBlock = 1;
		if(ioctlsocket(sock, FIONBIO, &NonBlock) != 0)
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: ioctlsocket failed" << endl;
#endif
			closesocket(sock);
			sock = -1;
			return false;
		}
#else // if defined(WIN32)
		int socket_flag = fcntl(sock, F_GETFL, 0);
		
		if(socket_flag < 0)
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: fcntl failed" << endl;
#endif
			close(sock);
			sock = -1;
			return false;
		}
		
		/* Ask for non-blocking reads on socket */
		if(fcntl(sock, F_SETFL, socket_flag | O_NONBLOCK) < 0)
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: fcntl failed" << endl;
#endif
			close(sock);
			sock = -1;
			return false;
		}
#endif // if defined(WIN32)
	
		return true;
	}

	return false;
}

template <class ADDRESS>
NetStub<ADDRESS>::~NetStub()
{
	if(sock >= 0)
	{
#ifdef WIN32
		closesocket(sock);
#else
		close(sock);
#endif
		sock = -1;
	}
}

inline char Nibble2HexChar(uint8_t v)
{
	v = v & 0xf; // keep only 4-bits
	return v < 10 ? '0' + v : 'a' + v - 10;
}

inline uint8_t HexChar2Nibble(char ch)
{
	if(ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
	if(ch >= '0' && ch <= '9') return ch - '0';
	if(ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
	return 0;
}

inline bool IsHexChar(char ch)
{
	if(ch >= 'a' && ch <= 'f') return true;
	if(ch >= '0' && ch <= '9') return true;
	if(ch >= 'A' && ch <= 'F') return true;
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::GetChar(char& c, bool blocking)
{
	do
	{
#ifdef WIN32
		int r = recv(sock, &c, 1, 0);
		if(r == 0 || r == SOCKET_ERROR)
#else
		ssize_t r = read(sock, &c, 1);
		if(r <= 0)
#endif
		{
#ifdef WIN32
			if(r == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK)
#else
			if(r < 0 && errno == EAGAIN)
#endif
			{
				if(blocking)
				{
#ifdef WIN32
					Sleep(10); // sleep for 10ms
#else
					usleep(10000); // sleep for 10ms
#endif
					continue;
				}
				else
				{
					return false;
				}
			}
			
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: Can't read from socket" << endl;
#endif
			return false;
		}
		//cerr << "Get: '" << c << "'" << endl;
		return true;
	} while(1);
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutChar(char c)
{
	//cerr << "Put: '" << c << "'" << endl;
#ifdef WIN32
	int r = send(sock, &c, 1, 0);
	if(r == 0 || r == SOCKET_ERROR)
#else
	ssize_t r = write(sock, &c, 1);
	if(r <= 0)
#endif
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: Can't write on socket" << endl;
#endif
		return false;
	}
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::GetPacket(string& s, bool blocking, uint32_t& device_id, uint32_t& tag)
{
	char c;
	char ch[2];
	ch[1] = 0;
	s.erase();
	device_id = 0;
	tag = 0;
	
	while(1)
	{
		if(!GetChar(c, blocking)) return false;
		if(c == '$') break;
	}
		
	while(1)
	{
		if(!GetChar(c, true)) return false;
		if(c == '#' || c == '$') break;
		ch[0] = c;
		s += ch;
	}
	
	if(c == '#')
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: Packet has probably device_id" << endl;
#endif
		uint8_t nibble;
		
		while(1)
		{
			if(!GetChar(c, true)) return false;
			//cerr << Object::GetName() << ": at " << pos << " got '" << c << "'" << endl;
			if(c == '$' || c == ';') break;
			if(!IsHexChar(c))
			{
#ifdef DEBUG_NETSTUB
				cerr << "NETSTUB: unexpected character '" << c << "'" << endl;
#endif
				return false;
			}
			nibble = HexChar2Nibble(c);
			device_id <<= 4;
			device_id |= nibble;
		}
		
		if(c == ';')
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: Packet has probably tag" << endl;
#endif
			while(1)
			{
				if(!GetChar(c, true)) return false;
				//cerr << Object::GetName() << ": at " << pos << " got '" << c << "'" << endl;
				if(c == '$') break;
				if(!IsHexChar(c))
				{
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: unexpected character '" << c << "'" << endl;
#endif
					return false;
				}
				nibble = HexChar2Nibble(c);
				tag <<= 4;
				tag |= nibble;
			}
		}
	}
	
#ifdef DEBUG_NETSTUB
	cerr << "NETSTUB: received '$" << s << "#" << std::hex << device_id << ";" << tag << std::dec << "$'" << endl;
#endif

	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutHex(uint32_t value)
{
	if(!value)
	{
		return PutChar('0');
	}
	
	int i;
	for(i = 4 * (sizeof(value) - 1) + 4; i >= 0; i -= 4)
	{
		if(value >> i) break;
	}
	for(; i >= 0; i--)
	{
		if(!PutChar(Nibble2HexChar(value >> i))) return false;
	}
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutPacket(const string& s, const uint32_t device_id, const uint32_t tag)
{
	unsigned int pos;
	unsigned int len;
	char c;
	
	if(!PutChar('$')) return false;
	pos = 0;
	len = s.length();
	
	while(pos < len)
	{
		c = s[pos];
		if(!PutChar(c)) return false;
		pos++;
	}
	if(!PutChar('#')) return false;
	if(!PutHex(device_id)) return false;
	if(!PutChar(';')) return false;
	if(!PutHex(tag)) return false;
	if(!PutChar('$')) return false;
	
#ifdef DEBUG_NETSTUB
	cerr << "NETSTUB: sent '$" << s << "#" << std::hex << device_id << ";" << tag << std::dec << "$'" << endl;
#endif

	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseHex(const string& s, unsigned int& pos, uint32_t& value)
{
	unsigned int len = s.length();
	unsigned int n = 0;
	
	value = 0;
	while(pos < len)
	{
		uint8_t nibble;
		char c = s[pos];
		if(!IsHexChar(c)) break;
		//cerr << Object::GetName() << ": at " << pos << " got '" << c << "'" << endl;
		nibble = HexChar2Nibble(c);
		value <<= 4;
		value |= nibble;
		pos++;
		n++;
	}
	//if(n > 0) cerr << Object::GetName() << ": got 0x" << std::hex << value << std::dec << endl;
#ifdef DEBUG_NETSTUB
	if(n > 2 * sizeof(value))
	{
		cerr << "NETSTUB: WARNING! the upper " << ((4 * n) - (8 * sizeof(value)))
			<< " bits have been lost while converting a "
			<< (4 * n) << "-bit hexadecimal integer to a "
			<< (8 * sizeof(value)) << "-bit integer" << endl;
	}
#endif
	return n > 0;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseBool(const string& s, unsigned int& pos, bool& value)
{
	if(s.compare(pos, s_true.length(), s_true) == 0)
	{
		value = true;
		pos += s_true.length();
		return true;
	}
	
	if(s.compare(pos, s_false.length(), s_false) == 0)
	{
		value = false;
		pos += s_false.length();
		return true;
	}

	uint32_t int_value;
	if(ParseHex(s, pos, int_value))
	{
		value = (int_value != 0);
		return true;
	}
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseHex(const string& s, unsigned int& pos, uint64_t& value)
{
	unsigned int len = s.length();
	unsigned int n = 0;
	
	value = 0;
	while(pos < len && n < 2 * sizeof(value))
	{
		uint8_t nibble;
		char c = s[pos];
		if(!IsHexChar(c)) break;
		//cerr << Object::GetName() << ": at " << pos << " got '" << c << "'" << endl;
		nibble = HexChar2Nibble(c);
		value <<= 4;
		value |= nibble;
		pos++;
		n++;
	}
	//if(n > 0) cerr << Object::GetName() << ": got 0x" << std::hex << value << std::dec << endl;
	return n > 0;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseHex(const string& s, unsigned int& pos, uint8_t *buffer, uint32_t size)
{
	unsigned int i;
	uint32_t offset;
	uint32_t l;
	char c;

	l = s.length();
	
	for(offset = 0, i = 0; pos < l && offset < size; i++, pos++)
	{
		c = s[pos];

		//cerr << Object::GetName() << ": at " << pos << " got '" << c << "'" << endl;
		if(!IsHexChar(c))
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: expecting an hexadecimal character (got '" << c << "')" << endl;
#endif
			return false;
		}
		
		if(i & 1)
		{
			buffer[offset] = buffer[offset] | HexChar2Nibble(c);
			//cerr << "buffer[" << offset << "] = 0x" << std::hex << (unsigned int) buffer[offset] << std::dec << endl;
			offset++;
		}
		else
		{
			buffer[offset] = HexChar2Nibble(c) << 4;
		}
	}
	
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseChar(const string& packet, unsigned int& pos, char c)
{	
	if(pos >= packet.length() || packet[pos] != c) return false;
	//cerr << Object::GetName() << ": at " << pos << " got '" << c << "'" << endl;
	pos++;
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseString(const string& packet, unsigned int& pos, char separator, string& s)
{
	unsigned int start_pos = pos;

	while(pos < packet.length())
	{
		if(packet[pos] == separator) break;
		pos++;
	}

	s = packet.substr(start_pos, pos - start_pos);
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseAddress(const string& packet, unsigned int& pos, ADDRESS& addr)
{
	if(ParseChar(packet, pos, '*'))
	{
		if(!ParseHex(packet, pos, addr))
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: expecting an address" << endl;
#endif
			return false;
		}
	}
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseSymbol(const string& packet, unsigned int& pos, ADDRESS& addr, string& symbol_name)
{
	if(ParseChar(packet, pos, '*'))
	{
		if(!ParseHex(packet, pos, addr))
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: expecting an address" << endl;
#endif
			return false;
		}
	}
	else
	{
		if(!ParseString(packet, pos, 0, symbol_name))
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: expecting a symbol name" << endl;
#endif
			return false;
		}
	}
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseCommand(const string& packet, unsigned int& pos, PK_COMMAND& command)
{
	if(packet.compare(pos, s_command_start.length(), s_command_start) == 0)
	{
		command = PKC_START;
		pos += s_command_start.length();
		return true;
	}
	
	if(packet.compare(pos, s_command_stop.length(), s_command_stop) == 0)
	{
		command = PKC_STOP;
		pos += s_command_stop.length();
		return true;
	}

	if(packet.compare(pos, s_command_read_register.length(), s_command_read_register) == 0)
	{
		command = PKC_READREG;
		pos += s_command_read_register.length();
		return true;
	}

	if(packet.compare(pos, s_command_write_register.length(), s_command_write_register) == 0)
	{
		command = PKC_WRITEREG;
		pos += s_command_write_register.length();
		return true;
	}

	if(packet.compare(pos, s_command_read.length(), s_command_read) == 0)
	{
		command = PKC_READ;
		pos += s_command_read.length();
		return true;
	}
	
	if(packet.compare(pos, s_command_write.length(), s_command_write) == 0)
	{
		command = PKC_WRITE;
		pos += s_command_write.length();
		return true;
	}

	if(packet.compare(pos, s_command_intr.length(), s_command_intr) == 0)
	{
		command = PKC_INTR;
		pos += s_command_intr.length();
		return true;
	}
	
	if(packet.compare(pos, s_command_run.length(), s_command_run) == 0)
	{
		command = PKC_RUN;
		pos += s_command_run.length();
		return true;
	}
	
	if(packet.compare(pos, s_command_setbrk.length(), s_command_setbrk) == 0)
	{
		command = PKC_SETBRK;
		pos += s_command_setbrk.length();
		return true;
	}

	if(packet.compare(pos, s_command_set_write_watchpoint.length(), s_command_set_write_watchpoint) == 0)
	{
		command = PKC_SETWRITEW;
		pos += s_command_set_write_watchpoint.length();
		return true;
	}

	if(packet.compare(pos, s_command_set_read_watchpoint.length(), s_command_set_read_watchpoint) == 0)
	{
		command = PKC_SETREADW;
		pos += s_command_set_read_watchpoint.length();
		return true;
	}

	if(packet.compare(pos, s_command_rmbrk.length(), s_command_rmbrk) == 0)
	{
		command = PKC_RMBRK;
		pos += s_command_rmbrk.length();
		return true;
	}

	if(packet.compare(pos, s_command_remove_read_watchpoint.length(), s_command_remove_read_watchpoint) == 0)
	{
		command = PKC_RMREADW;
		pos += s_command_remove_read_watchpoint.length();
		return true;
	}

	if(packet.compare(pos, s_command_remove_write_watchpoint.length(), s_command_remove_write_watchpoint) == 0)
	{
		command = PKC_RMWRITEW;
		pos += s_command_remove_write_watchpoint.length();
		return true;
	}

	if(packet.compare(pos, s_command_trap.length(), s_command_trap) == 0)
	{
		command = PKC_TRAP;
		pos += s_command_trap.length();
		return true;
	}

	uint32_t value;
	if(ParseHex(packet, pos, value))
	{
		switch(value)
		{
			case PKC_START:
			case PKC_STOP:
			case PKC_READ:
			case PKC_WRITE:
			case PKC_READREG:
			case PKC_WRITEREG:
			case PKC_INTR:
			case PKC_RUN:
			case PKC_SETBRK:
			case PKC_SETWRITEW:
			case PKC_SETREADW:
			case PKC_RMBRK:
			case PKC_RMWRITEW:
			case PKC_RMREADW:
			case PKC_TRAP:
				command = (PK_COMMAND) value;
				return true;
		}
	}
		
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseTimeUnit(const string& packet, unsigned int& pos, TIME_UNIT& tu)
{
	if(packet.compare(pos, s_tu_fs.length(), s_tu_fs) == 0)
	{
		tu = TU_FS;
		pos += s_tu_fs.length();
		return true;
	}
	if(packet.compare(pos, s_tu_ps.length(), s_tu_ps) == 0)
	{
		tu = TU_PS;
		pos += s_tu_ps.length();
		return true;
	}
	if(packet.compare(pos, s_tu_ns.length(), s_tu_ns) == 0)
	{
		tu = TU_NS;
		pos += s_tu_ns.length();
		return true;
	}
	if(packet.compare(pos, s_tu_us.length(), s_tu_us) == 0)
	{
		tu = TU_US;
		pos += s_tu_us.length();
		return true;
	}
	if(packet.compare(pos, s_tu_ms.length(), s_tu_ms) == 0)
	{
		tu = TU_MS;
		pos += s_tu_ms.length();
		return true;
	}
	if(packet.compare(pos, s_tu_s.length(), s_tu_s) == 0)
	{
		tu = TU_S;
		pos += s_tu_s.length();
		return true;
	}
	uint32_t value;
	if(ParseHex(packet, pos, value))
	{
		switch(value)
		{
			case TU_FS:
			case TU_PS:
			case TU_NS:
			case TU_US:
			case TU_MS:
			case TU_S:
				tu = (TIME_UNIT) value;
				return true;
		}
	}
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseSpace(const string& packet, unsigned int& pos, SPACE& space)
{
	if(packet.compare(pos, s_cpu_mem.length(), s_cpu_mem) == 0)
	{
		space = SP_CPU_MEM;
		pos += s_cpu_mem.length();
		return true;
	}

	if(packet.compare(pos, s_dev_mem.length(), s_dev_mem) == 0)
	{
		space = SP_DEV_MEM;
		pos += s_dev_mem.length();
		return true;
	}

	if(packet.compare(pos, s_dev_io.length(), s_dev_io) == 0)
	{
		space = SP_DEV_IO;
		pos += s_dev_io.length();
		return true;
	}

	uint32_t value;
	if(ParseHex(packet, pos, value))
	{
		switch(value)
		{
			case SP_CPU_MEM:
			case SP_DEV_MEM:
			case SP_DEV_IO:
				space = (SPACE) value;
				return true;
		}
	}
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseTrapType(const string& packet, unsigned int& pos, TRAP_TYPE& trap_type)
{
	if(packet.compare(pos, s_trap_breakpoint.length(), s_trap_breakpoint) == 0)
	{
		trap_type = TRAP_BREAKPOINT;
		pos += s_trap_breakpoint.length();
		return true;
	}

	if(packet.compare(pos, s_trap_watchpoint.length(), s_trap_watchpoint) == 0)
	{
		trap_type = TRAP_WATCHPOINT;
		pos += s_trap_watchpoint.length();
		return true;
	}

	uint32_t value;
	if(ParseHex(packet, pos, value))
	{
		switch(value)
		{
			case TRAP_BREAKPOINT:
			case TRAP_WATCHPOINT:
				trap_type = (TRAP_TYPE) value;
				return true;
		}
	}
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseWatchpointType(const string& packet, unsigned int& pos, WATCHPOINT_TYPE& watchpoint_type)
{
	if(packet.compare(pos, s_watchpoint_read.length(), s_watchpoint_read) == 0)
	{
		watchpoint_type = WATCHPOINT_READ;
		pos += s_watchpoint_read.length();
		return true;
	}

	if(packet.compare(pos, s_watchpoint_write.length(), s_watchpoint_write) == 0)
	{
		watchpoint_type = WATCHPOINT_WRITE;
		pos += s_watchpoint_write.length();
		return true;
	}

	uint32_t value;
	if(ParseHex(packet, pos, value))
	{
		switch(value)
		{
			case WATCHPOINT_READ:
			case WATCHPOINT_WRITE:
				watchpoint_type = (WATCHPOINT_TYPE) value;
				return true;
		}
	}
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseStart(const string& packet, unsigned int& pos, TIME_UNIT& tu)
{
	if(pos == packet.length()) return true;
	
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';'" << endl;
#endif
		return false;
	}
	
	if(!ParseTimeUnit(packet, pos, tu))
	{
		tu = TU_MS;
	}
	
#ifdef DEBUG_NETSTUB
	if(pos < packet.length())
	{
		cerr << "NETSTUB: ignoring extra characters at pos " << pos << endl;
	}
#endif

	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseStop(const string& packet, unsigned int& pos)
{
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseRead(const string& packet, unsigned int& pos, ADDRESS& addr, uint32_t& size, SPACE& space)
{
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';'" << endl;
#endif
		return false;
	}
	
	if(!ParseAddress(packet, pos, addr))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting an address" << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got address 0x" << std::hex << addr << std::dec << endl;
	
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';'" << endl;
#endif
		return false;
	}

	if(!ParseHex(packet, pos, size))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a size" << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got size 0x" << std::hex << size << std::dec << endl;
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';'" << endl;
#endif
		return false;
	}

	if(!ParseSpace(packet, pos, space))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a space ('cpu_mem', 'dev_mem', 'dev_io')" << endl;
#endif
		return false;
	}
	
	if(pos < packet.length())
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: ignoring extra characters at pos " << pos << endl;
#endif
	}
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseWrite(const string& packet, unsigned int& pos, ADDRESS& addr, uint32_t& size, uint8_t **data, SPACE& space)
{
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';'" << endl;
#endif
		return false;
	}
	
	if(!ParseAddress(packet, pos, addr))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting an address" << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got address 0x" << std::hex << addr << std::dec << endl;
	
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';'" << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got ';'" << endl;
	
	if(!ParseHex(packet, pos, size))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a size" << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got size 0x" << std::hex << size << std::dec << endl;
	
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';'" << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got ';'" << endl;
	
	*data = new uint8_t[size];
	
	if(!ParseHex(packet, pos, *data, size))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting some data" << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got some data" << endl;

	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';'" << endl;
#endif
		return false;
	}

	if(!ParseSpace(packet, pos, space))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a space ('cpu_mem', 'dev_mem', 'dev_io')" << endl;
#endif
		return false;
	}

#ifdef DEBUG_NETSTUB
	if(pos < packet.length())
	{
		cerr << "NETSTUB: ignoring extra characters at pos " << pos << endl;
	}
#endif
	
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseWriteRegister(const string& packet, unsigned int& pos, string& register_name, uint32_t& value)
{
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';'" << endl;
#endif
		return false;
	}

	if(!ParseString(packet, pos, ';', register_name))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a register name" << endl;
#endif
		return false;
	}

	cerr << "got register name = \"" << register_name << "\"" << endl;
	cerr << "currently at pos" << pos << endl;

	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';'" << endl;
#endif
		return false;
	}

	if(!ParseHex(packet, pos, value))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a value" << endl;
#endif
		return false;
	}

#ifdef DEBUG_NETSTUB
	if(pos < packet.length())
	{
		cerr << "NETSTUB: ignoring extra characters at pos " << pos << endl;
	}
#endif

	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseReadRegister(const string& packet, unsigned int& pos, string& register_name)
{
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';'" << endl;
#endif
		return false;
	}

	if(!ParseString(packet, pos, 0, register_name))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a register name" << endl;
#endif
		return false;
	}

#ifdef DEBUG_NETSTUB
	if(pos < packet.length())
	{
		cerr << "NETSTUB: ignoring extra characters at pos " << pos << endl;
	}
#endif

	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseIntr(const string& packet, unsigned int& pos, uint32_t& intr_id, bool& level)
{
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';'" << endl;
#endif
		return false;
	}
	
	if(!ParseHex(packet, pos, intr_id))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting an interrupt id" << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got interrupt id" << std::hex << intr_id << std::dec << endl;

	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';'" << endl;
#endif
		return false;
	}

	if(!ParseBool(packet, pos, level))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting an interrupt level (0/1 or true/false)" << endl;
#endif
		return false;
	}
#ifdef DEBUG_NETSTUB
	if(pos < packet.length())
	{
		cerr << "NETSTUB: ignoring extra characters at pos " << pos << endl;
	}
#endif
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseRun(const string& packet, unsigned int& pos, uint64_t& duration, TIME_UNIT& tu)
{
	if(pos >= packet.length())
	{
		duration = 0;
		return true;
	}

	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';'" << endl;
#endif
		return false;
	}
	
	if(!ParseHex(packet, pos, duration))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a duration" << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got duration " << std::hex << duration << std::dec << endl;
	
	if(pos >= packet.length())
	{
		tu = default_tu;
		return true;
	}
	
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';'" << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got ';'" << endl;
	
	if(!ParseTimeUnit(packet, pos, tu))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a time unit" << endl;
#endif
		return false;
	}
	
#ifdef DEBUG_NETSTUB
	if(pos < packet.length())
	{
		cerr << "NETSTUB: ignoring extra characters at pos " << pos << endl;
	}
#endif
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseSetBreakpoint(const string& packet, unsigned int& pos, ADDRESS& addr, string& symbol_name)
{
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';'" << endl;
#endif
		return false;
	}

	if(!ParseSymbol(packet, pos, addr, symbol_name))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting an address or a symbol name" << endl;
#endif
		return false;
	}

#ifdef DEBUG_NETSTUB
	if(pos < packet.length())
	{
		cerr << "NETSTUB: ignoring extra characters at pos " << pos << endl;
	}
#endif
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseSetReadWatchpoint(const string& packet, unsigned int& pos, ADDRESS& addr, uint32_t& size, SPACE& space)
{
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';'" << endl;
#endif
		return false;
	}

	if(!ParseAddress(packet, pos, addr))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting an address" << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got address 0x" << std::hex << addr << std::dec << endl;
	
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';'" << endl;
#endif
		return false;
	}

	if(!ParseHex(packet, pos, size))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a size" << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got size 0x" << std::hex << size << std::dec << endl;
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';'" << endl;
#endif
		return false;
	}

	if(!ParseSpace(packet, pos, space))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a space ('cpu_mem', 'dev_mem', 'dev_io')" << endl;
#endif
		return false;
	}

#ifdef DEBUG_NETSTUB
	if(pos < packet.length())
	{
		cerr << "NETSTUB: ignoring extra characters at pos " << pos << endl;
	}
#endif
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseSetWriteWatchpoint(const string& packet, unsigned int& pos, ADDRESS& addr, uint32_t& size, SPACE& space)
{
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';'" << endl;
#endif
		return false;
	}

	if(!ParseAddress(packet, pos, addr))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting an address" << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got address 0x" << std::hex << addr << std::dec << endl;
	
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';'" << endl;
#endif
		return false;
	}

	if(!ParseHex(packet, pos, size))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a size" << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got size 0x" << std::hex << size << std::dec << endl;
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';'" << endl;
#endif
		return false;
	}

	if(!ParseSpace(packet, pos, space))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a space ('cpu_mem', 'dev_mem', 'dev_io')" << endl;
#endif
		return false;
	}

#ifdef DEBUG_NETSTUB
	if(pos < packet.length())
	{
		cerr << "NETSTUB: ignoring extra characters at pos " << pos << endl;
	}
#endif
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseRemoveBreakpoint(const string& packet, unsigned int& pos, ADDRESS& addr, string& symbol_name)
{
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';'" << endl;
#endif
		return false;
	}

	if(!ParseSymbol(packet, pos, addr, symbol_name))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting an address or a symbol name" << endl;
#endif
		return false;
	}

#ifdef DEBUG_NETSTUB
	if(pos < packet.length())
	{
		cerr << "NETSTUB: ignoring extra characters at pos " << pos << endl;
	}
#endif
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseRemoveWriteWatchpoint(const string& packet, unsigned int& pos, ADDRESS& addr, uint32_t& size, SPACE& space)
{
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';'" << endl;
#endif
		return false;
	}

	if(!ParseAddress(packet, pos, addr))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting an address" << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got address 0x" << std::hex << addr << std::dec << endl;
	
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';'" << endl;
#endif
		return false;
	}

	if(!ParseHex(packet, pos, size))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a size" << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got size 0x" << std::hex << size << std::dec << endl;
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';'" << endl;
#endif
		return false;
	}

	if(!ParseSpace(packet, pos, space))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a space ('cpu_mem', 'dev_mem', 'dev_io')" << endl;
#endif
		return false;
	}

#ifdef DEBUG_NETSTUB
	if(pos < packet.length())
	{
		cerr << "NETSTUB: ignoring extra characters at pos " << pos << endl;
	}
#endif
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseRemoveReadWatchpoint(const string& packet, unsigned int& pos, ADDRESS& addr, uint32_t& size, SPACE& space)
{
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';'" << endl;
#endif
		return false;
	}

	if(!ParseAddress(packet, pos, addr))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting an address" << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got address 0x" << std::hex << addr << std::dec << endl;
	
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';'" << endl;
#endif
		return false;
	}

	if(!ParseHex(packet, pos, size))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a size" << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got size 0x" << std::hex << size << std::dec << endl;
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';'" << endl;
#endif
		return false;
	}

	if(!ParseSpace(packet, pos, space))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a space ('cpu_mem', 'dev_mem', 'dev_io')" << endl;
#endif
		return false;
	}

#ifdef DEBUG_NETSTUB
	if(pos < packet.length())
	{
		cerr << "NETSTUB: ignoring extra characters at pos " << pos << endl;
	}
#endif
	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ParseTrap(const string& packet, unsigned int& pos, uint64_t& t, TIME_UNIT& tu, list<TRAP>& traps)
{
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';'" << endl;
#endif
		return false;
	}
	
	if(!ParseHex(packet, pos, t))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a time" << endl;
#endif
		return false;
	}
	
	if(!ParseChar(packet, pos, ';'))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting ';'" << endl;
#endif
		return false;
	}
	//cerr << Object::GetName() << ": at " << pos << " got ';'" << endl;
	
	if(!ParseTimeUnit(packet, pos, tu))
	{
#ifdef DEBUG_NETSTUB
		cerr << "NETSTUB: expecting a time unit" << endl;
#endif
		return false;
	}

	traps.clear();

	while(pos < packet.length()) // end of packet ?
	{
		if(!ParseChar(packet, pos, ';'))
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: expecting ';'" << endl;
#endif
			return false;
		}

		TRAP trap;

		if(!ParseTrapType(packet, pos, trap.type))
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: expecting a trap type" << endl;
#endif
		}

		switch(trap.type)
		{
			case TRAP_BREAKPOINT:
				if(!ParseChar(packet, pos, ';'))
				{
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: expecting ';'" << endl;
#endif
					return false;
				}
		
				if(!ParseAddress(packet, pos, trap.breakpoint.addr))
				{
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: expecting an address" << endl;
#endif
					return false;
				}
				break;

			case TRAP_WATCHPOINT:
				if(!ParseChar(packet, pos, ';'))
				{
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: expecting ';'" << endl;
#endif
					return false;
				}
		
				if(!ParseWatchpointType(packet, pos, trap.watchpoint.wtype))
				{
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: expecting a watchpoint type" << endl;
#endif
					return false;
				}

				if(!ParseChar(packet, pos, ';'))
				{
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: expecting ';'" << endl;
#endif
					return false;
				}
		
				if(!ParseAddress(packet, pos, trap.watchpoint.addr))
				{
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: expecting an address" << endl;
#endif
					return false;
				}

				if(!ParseChar(packet, pos, ';'))
				{
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: expecting ';'" << endl;
#endif
					return false;
				}

				if(!ParseHex(packet, pos, trap.watchpoint.size))
				{
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: expecting a size" << endl;
#endif
					return false;
				}

				if(!ParseChar(packet, pos, ';'))
				{
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: expecting ';'" << endl;
#endif
					return false;
				}

				if(!ParseSpace(packet, pos, trap.watchpoint.space))
				{
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: expecting a space ('cpu_mem', 'dev_mem', 'dev_io')" << endl;
#endif
					return false;
				}
				break;
		}

		traps.push_back(trap);
	}

	return true;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutStartPacket(const TIME_UNIT tu, const uint32_t device_id, const uint32_t tag)
{
	stringstream sstr;
	
	sstr << std::hex;
	sstr << s_command_start << ';';
	switch(tu)
	{
		case TU_FS: sstr << s_tu_fs; break;
		case TU_PS: sstr << s_tu_ps; break;
		case TU_NS: sstr << s_tu_ns; break;
		case TU_US: sstr << s_tu_us; break;
		case TU_MS: sstr << s_tu_ms; break;
		case TU_S: sstr << s_tu_s; break;
	}
	return PutPacket(sstr.str(), device_id, tag);
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutStopPacket(const uint32_t device_id, const uint32_t tag)
{
	return PutPacket(s_command_stop, device_id, tag);
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutWritePacket(ADDRESS addr, uint32_t size, uint8_t *data, SPACE space, const uint32_t device_id, const uint32_t tag)
{
	stringstream sstr;
	
	sstr << std::hex;
	sstr << s_command_write << ';';
	sstr << '*' << addr << ';';
	sstr << size << ';';
	sstr << space;
	
	uint32_t i;
		
	for(i = 0; i < size; i++)
	{
		sstr << Nibble2HexChar(data[i] >> 4);
		sstr << Nibble2HexChar(data[i] & 0xf);
	}
	return PutPacket(sstr.str(), device_id, tag);
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutWriteRegisterPacket(const char *name, const uint32_t& value, const uint32_t device_id, const uint32_t tag)
{
	stringstream sstr;
	
	sstr << std::hex;
	sstr << s_command_write_register << ';';
	sstr << name << ';';
	sstr << value;
	return PutPacket(sstr.str(), device_id, tag);
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutReadRegisterPacket(const char *name, const uint32_t device_id, const uint32_t tag)
{
	stringstream sstr;
	
	sstr << std::hex;
	sstr << s_command_write_register << ';';
	sstr << name;
	return PutPacket(sstr.str(), device_id, tag);
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutReadPacket(ADDRESS addr, uint32_t size, SPACE space, const uint32_t device_id, const uint32_t tag)
{
	stringstream sstr;
	
	sstr << std::hex;
	sstr << s_command_read << ';';
	sstr << '*' << addr << ';';
	sstr << size << ';';
	sstr << space;
	return PutPacket(sstr.str(), device_id, tag);
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutIntrPacket(uint32_t intr_id, bool level, const uint32_t device_id, const uint32_t tag)
{
	stringstream sstr;
	
	sstr << std::hex;
	sstr << s_command_intr << ';';
	sstr << intr_id << ';';
	sstr << level;
	return PutPacket(sstr.str(), device_id, tag);
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutRunPacket(uint64_t duration, const uint32_t device_id, const uint32_t tag)
{
	stringstream sstr;
	
	sstr << std::hex;
	sstr << s_command_run << ';';
	sstr << duration;
	return PutPacket(sstr.str(), device_id, tag);
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutRunPacket(uint64_t duration, TIME_UNIT tu, const uint32_t device_id, const uint32_t tag)
{
	stringstream sstr;
	
	sstr << std::hex;
	sstr << s_command_run << ';';
	sstr << duration << ';';
	switch(tu)
	{
		case TU_FS: sstr << s_tu_fs; break;
		case TU_PS: sstr << s_tu_ps; break;
		case TU_NS: sstr << s_tu_ns; break;
		case TU_US: sstr << s_tu_us; break;
		case TU_MS: sstr << s_tu_ms; break;
		case TU_S: sstr << s_tu_s; break;
	}
	return PutPacket(sstr.str(), device_id, tag);
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutSetBreakpointPacket(ADDRESS addr, const uint32_t device_id, const uint32_t tag)
{
	stringstream sstr;
	
	sstr << std::hex;
	sstr << s_command_setbrk << ';';
	sstr << '*' << addr;
	return PutPacket(sstr.str(), device_id, tag);	
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutSetBreakpointPacket(const char *symbol_name, const uint32_t device_id, const uint32_t tag)
{
	stringstream sstr;
	
	sstr << std::hex;
	sstr << s_command_setbrk << ';';
	sstr << symbol_name;
	return PutPacket(sstr.str(), device_id, tag);	
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutRemoveBreakpointPacket(ADDRESS addr, const uint32_t device_id, const uint32_t tag)
{
	stringstream sstr;
	
	sstr << std::hex;
	sstr << s_command_rmbrk << ';';
	sstr << '*' << addr;
	return PutPacket(sstr.str(), device_id, tag);	
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutRemoveBreakpointPacket(const char *symbol_name, const uint32_t device_id, const uint32_t tag)
{
	stringstream sstr;
	
	sstr << std::hex;
	sstr << s_command_rmbrk << ';';
	sstr << symbol_name;
	return PutPacket(sstr.str(), device_id, tag);	
}

template <class ADDRESS>
bool NetStub<ADDRESS>::PutTrapPacket(uint64_t t, TIME_UNIT tu, const list<TRAP>& traps, const uint32_t device_id, const uint32_t tag)
{
	stringstream sstr;
	
	sstr << std::hex;
	sstr << s_command_trap << ';';
	sstr << t << ';';
	sstr << tu;
	
	typename list<TRAP>::const_iterator trap_iter;

	for(trap_iter = traps.begin(); trap_iter != traps.end(); trap_iter++)
	{
		sstr << ';';
		switch(trap_iter->type)
		{
			case TRAP_BREAKPOINT:
				sstr << s_trap_breakpoint << ';' << std::hex << trap_iter->breakpoint.addr;
				break;
			case TRAP_WATCHPOINT:
				sstr << s_trap_watchpoint << ';';

				switch(trap_iter->watchpoint.wtype)
				{
					case WATCHPOINT_READ:
						sstr << s_watchpoint_read;
						break;
					case WATCHPOINT_WRITE:
						sstr << s_watchpoint_write;
						break;
				}

				sstr << ';' << std::hex << trap_iter->watchpoint.addr << ';' << trap_iter->watchpoint.size << ';';
				switch(trap_iter->watchpoint.space)
				{
					case SP_CPU_MEM:
						sstr << s_cpu_mem;
						break;
					case SP_DEV_MEM:
						sstr << s_dev_mem;
						break;
					case SP_DEV_IO:
						sstr << s_dev_io;
						break;
				}
				break;
		}
	}
	return PutPacket(sstr.str(), device_id, tag);	
}

template <class ADDRESS>
void NetStub<ADDRESS>::Step()
{
	if(sock >= 0)
	{
		string packet;
		unsigned int pos;
		PK_COMMAND pk_command;
		uint32_t pk_device_id;
		uint32_t pk_tag;
		
		while(1)
		{
#ifdef DEBUG_NETSTUB
			cerr << "NETSTUB: Reading packet..." << endl;
#endif
			if(!GetPacket(packet, true, pk_device_id, pk_tag))
			{
#ifdef DEBUG_NETSTUB
				cerr << "NETSTUB: failed" << endl;
#endif
				Stop();
				return;
			}
		
			pos = 0;
			if(!ParseCommand(packet, pos, pk_command))
			{
#ifdef DEBUG_NETSTUB
				cerr << "NETSTUB: expecting a command" << endl;
#endif
				return;
			}
			
			switch(pk_command)
			{
				case PKC_START:
				{
					TIME_UNIT pk_tu;
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: parsing command '" << s_command_start << "'" << endl;
#endif
					
					if(!ParseStart(packet, pos, pk_tu))
					{
						Stop();
						return;
					}
					
					default_tu = pk_tu;
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: processing command '" << s_command_start << "'" << endl;
#endif
					Start();
					break;
				}
				case PKC_STOP:
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: processing command '" << s_command_stop << "'" << endl;
#endif
					Stop();
					return;
					
				case PKC_READ:
				{
					ADDRESS pk_addr;
					uint32_t pk_size;
					uint8_t *pk_data;
					SPACE pk_space;
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: parsing command '" << s_command_read << "'" << endl;
#endif
					if(!ParseRead(packet, pos, pk_addr, pk_size, pk_space))
					{
						Stop();
						return;
					}
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: processing command '" << s_command_read << "'" << endl;
#endif
					
					pk_data = new uint8_t[pk_size];
					memset(pk_data, 0, pk_size);
					if(!Read(pk_addr, pk_data, pk_size, pk_space))
					{
#ifdef DEBUG_NETSTUB
						cerr << "NETSTUB: memory read failed" << endl;
#endif
					}
					if(!PutWritePacket(pk_addr, pk_size, pk_data, pk_space, pk_device_id, pk_tag))
					{
						delete[] pk_data;
						Stop();
						return;
					}
					delete[] pk_data;
					break;
				}
				
				case PKC_WRITE:
				{
					ADDRESS pk_addr;
					uint32_t pk_size;
					uint8_t *pk_data;
					SPACE pk_space;
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: parsing command '" << s_command_write << "'" << endl;
#endif
					if(!ParseWrite(packet, pos, pk_addr, pk_size, &pk_data, pk_space))
					{
						Stop();
						return;
					}
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: processing command '" << s_command_write << "'" << endl;
#endif
					
					if(!Write(pk_addr, pk_data, pk_size, pk_space))
					{
#ifdef DEBUG_NETSTUB
						cerr << "NETSTUB: memory write failed" << endl;
#endif
					}
					delete[] pk_data;
					break;
				}

				case PKC_WRITEREG:
				{
					string pk_register_name;
					uint32_t pk_data;

#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: parsing command '" << s_command_write_register << "'" << endl;
#endif
					if(!ParseWriteRegister(packet, pos, pk_register_name, pk_data))
					{
						Stop();
						return;
					}

#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: processing command '" << s_command_write_register << "'" << endl;
#endif
					
					if(!WriteRegister(pk_register_name.c_str(), pk_data))
					{
#ifdef DEBUG_NETSTUB
						cerr << "NETSTUB: register write failed" << endl;
#endif
					}
					break;
				}

				case PKC_READREG:
				{
					string pk_register_name;
					uint32_t pk_data;

#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: parsing command '" << s_command_read_register << "'" << endl;
#endif
					if(!ParseReadRegister(packet, pos, pk_register_name))
					{
						Stop();
						return;
					}

#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: processing command '" << s_command_read_register << "'" << endl;
#endif
					
					if(!ReadRegister(pk_register_name.c_str(), pk_data))
					{
#ifdef DEBUG_NETSTUB
						cerr << "NETSTUB: register read failed" << endl;
#endif
					}

					if(!PutWriteRegisterPacket(pk_register_name.c_str(), pk_data, pk_device_id, pk_tag))
					{
						Stop();
						return;
					}
					break;
				}
				
				case PKC_INTR:
				{
					uint32_t pk_intr_id;
					bool level;
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: parsing command '" << s_command_intr << "'" << endl;
#endif
					if(!ParseIntr(packet, pos, pk_intr_id, level))
					{
						Stop();
						return;
					}
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: processing command '" << s_command_intr << "'" << endl;
#endif
					Intr(pk_intr_id, level);
					break;
				}
				
				case PKC_RUN:
				{
					TIME_UNIT pk_tu;
					uint64_t pk_duration;
					
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: parsing command '" << s_command_run << "'" << endl;
#endif
					if(!ParseRun(packet, pos, pk_duration, pk_tu))
					{
						Stop();
						return;
					}
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: processing command '" << s_command_run << "'" << endl;
#endif
					Run(pk_duration, pk_tu);
					break;
				}

				case PKC_SETBRK:
				{
					ADDRESS pk_addr;
					string pk_symbol_name;
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: parsing command '" << s_command_setbrk << "'" << endl;
#endif
					if(!ParseSetBreakpoint(packet, pos, pk_addr, pk_symbol_name))
					{
						Stop();
						return;
					}
					if(pk_symbol_name.empty() ? !SetBreakpoint(pk_addr) : !SetBreakpoint(pk_symbol_name.c_str()))
					{
#ifdef DEBUG_NETSTUB
						cerr << "NETSTUB: Can't set breakpoint" << endl;
#endif
					}
					break;
				}

				case PKC_SETWRITEW:
				{
					ADDRESS pk_addr;
					uint32_t pk_size;
					SPACE pk_space;
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: parsing command '" << s_command_set_write_watchpoint << "'" << endl;
#endif
					if(!ParseSetWriteWatchpoint(packet, pos, pk_addr, pk_size, pk_space))
					{
						Stop();
						return;
					}
					if(!SetWriteWatchpoint(pk_addr, pk_size, pk_space))
					{
#ifdef DEBUG_NETSTUB
						cerr << "NETSTUB: Can't set write watchpoint" << endl;
#endif
					}
					break;
				}

				case PKC_SETREADW:
				{
					ADDRESS pk_addr;
					uint32_t pk_size;
					SPACE pk_space;
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: parsing command '" << s_command_set_read_watchpoint << "'" << endl;
#endif
					if(!ParseSetReadWatchpoint(packet, pos, pk_addr, pk_size, pk_space))
					{
						Stop();
						return;
					}
					if(!SetReadWatchpoint(pk_addr, pk_size, pk_space))
					{
#ifdef DEBUG_NETSTUB
						cerr << "NETSTUB: Can't set read watchpoint" << endl;
#endif
					}
					break;
				}

				case PKC_RMBRK:
				{
					ADDRESS pk_addr;
					string pk_symbol_name;
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: parsing command '" << s_command_setbrk << "'" << endl;
#endif
					if(!ParseRemoveBreakpoint(packet, pos, pk_addr, pk_symbol_name))
					{
						Stop();
						return;
					}
					cerr << "pk_symbol_name = " << pk_symbol_name << endl;
					if(pk_symbol_name.empty() ? !RemoveBreakpoint(pk_addr) : !RemoveBreakpoint(pk_symbol_name.c_str()))
					{
#ifdef DEBUG_NETSTUB
						cerr << "NETSTUB: Can't remove breakpoint" << endl;
#endif
					}
					break;
				}

				case PKC_RMWRITEW:
				{
					ADDRESS pk_addr;
					uint32_t pk_size;
					SPACE pk_space;
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: parsing command '" << s_command_remove_write_watchpoint << "'" << endl;
#endif
					if(!ParseRemoveWriteWatchpoint(packet, pos, pk_addr, pk_size, pk_space))
					{
						Stop();
						return;
					}
					if(!RemoveWriteWatchpoint(pk_addr, pk_size, pk_space))
					{
#ifdef DEBUG_NETSTUB
						cerr << "NETSTUB: Can't remove write watchpoint" << endl;
#endif
					}
					break;
				}

				case PKC_RMREADW:
				{
					ADDRESS pk_addr;
					uint32_t pk_size;
					SPACE pk_space;
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: parsing command '" << s_command_remove_read_watchpoint << "'" << endl;
#endif
					if(!ParseRemoveReadWatchpoint(packet, pos, pk_addr, pk_size, pk_space))
					{
						Stop();
						return;
					}
					if(!RemoveReadWatchpoint(pk_addr, pk_size, pk_space))
					{
#ifdef DEBUG_NETSTUB
						cerr << "NETSTUB: Can't remove read watchpoint" << endl;
#endif
					}
					break;
				}

				case PKC_TRAP:
#ifdef DEBUG_NETSTUB
					cerr << "NETSTUB: ignoring command '" << s_command_trap << "'" << endl;
#endif
					break;
			}
		}
	}
	else
	{
		Start();
		Run(0xffffffffUL, TU_S); // run forever
		Stop();
	}
}

template <class ADDRESS>
void NetStub<ADDRESS>::Start()
{
}

template <class ADDRESS>
void NetStub<ADDRESS>::Stop()
{
}

template <class ADDRESS>
bool NetStub<ADDRESS>::Read(ADDRESS addr, void *data, uint32_t size, SPACE space)
{
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::Write(ADDRESS addr, const void *data, uint32_t size, SPACE space)
{
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::ReadRegister(const char *name, uint32_t& value)
{
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::WriteRegister(const char *name, uint32_t value)
{
	return false;
}

template <class ADDRESS>
void NetStub<ADDRESS>::Intr(uint32_t intr_id, bool level)
{
}

template <class ADDRESS>
void NetStub<ADDRESS>::Run(uint64_t duration, TIME_UNIT tu)
{
}

template <class ADDRESS>
bool NetStub<ADDRESS>::SetBreakpoint(ADDRESS addr)
{
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::SetBreakpoint(const char *symbol_name)
{
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::SetReadWatchpoint(ADDRESS addr, uint32_t size, SPACE space)
{
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::SetWriteWatchpoint(ADDRESS addr, uint32_t size, SPACE space)
{
	return false;
}


template <class ADDRESS>
bool NetStub<ADDRESS>::RemoveBreakpoint(ADDRESS addr)
{
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::RemoveBreakpoint(const char *symbol_name)
{
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::RemoveReadWatchpoint(ADDRESS addr, uint32_t size, SPACE space)
{
	return false;
}

template <class ADDRESS>
bool NetStub<ADDRESS>::RemoveWriteWatchpoint(ADDRESS addr, uint32_t size, SPACE space)
{
	return false;
}

} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
