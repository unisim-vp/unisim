/*
 *  Copyright (c) 2017,
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
 
#include <unisim/service/netstreamer/netstreamer.hh>
#include <unisim/util/likely/likely.hh>

#include <errno.h>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)

#include <winsock2.h>

#else

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <fcntl.h>

#endif

namespace unisim {
namespace service {
namespace netstreamer {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

const uint64_t NetStreamer::CONNECTION_POLL_PERIOD_MS;

// Telnet commands
enum TelnetCommand
{
	TELNET_CMD_IAC  = 255,
	TELNET_CMD_DONT = 254,
	TELNET_CMD_DO   = 253,
	TELNET_CMD_WONT = 252,
	TELNET_CMD_WILL = 251,
	TELNET_CMD_SB   = 250,
	TELNET_CMD_SE   = 240
};

// Telnet options
enum TelnetOption
{
	TELNET_OPT_BINARY            = 0,
	TELNET_OPT_ECHO              = 1,
	TELNET_OPT_SUPPRESS_GO_AHEAD = 3,
	TELNET_OPT_LINEMODE          = 34
};

std::ostream& operator << (std::ostream& os, const NetStreamerProtocol& nsp)
{
	switch(nsp)
	{
		case NETSTREAMER_PROTOCOL_RAW   : os << "raw protocol"; break;
		case NETSTREAMER_PROTOCOL_TELNET: os << "telnet protocol"; break;
		default                         : os << "unknown protocol"; break;
	}
	
	return os;
}

NetStreamer::NetStreamer(const char *name, unisim::kernel::service::Object *parent)
	: Object(name, parent, "This service provides character I/O over TCP/IP")
	, unisim::kernel::service::Service<CharIO>(name, parent)
	, char_io_export("char-io-export", this)
	, logger(*this)
	, verbose(false)
	, debug(false)
	, is_server(true)
	, server_name("localhost")
	, protocol(NETSTREAMER_PROTOCOL_RAW)
	, filter_null_character(false)
	, filter_line_feed(false)
	, enable_telnet_binary(true)
	, enable_telnet_echo(true)
	, enable_telnet_suppress_go_ahead(true)
	, enable_telnet_linemode(false)
	, tcp_port(0)
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	, sock(INVALID_SOCKET)
#else
	, sock(-1)
#endif
	, state(0)
	, telnet_sb_opt(0)
	, telnet_sb_params()
	, thrd_conn_alive(false)
	, killed(false)
	, thrd_conn()
	, thrd_conn_create_mutex()
	, thrd_conn_create_cond()
	, thrd_conn_close_mutex()
	, thrd_conn_close_cond()
	, connection_closed()
	, connection_established()
	, mutex()
	, param_verbose("verbose", this, verbose, "Enable/Disable verbosity")
	, param_debug("debug", this, debug, "Enable/Disable debug (intended for developper)")
	, param_tcp_port("tcp-port", this, tcp_port, "TCP/IP port")
	, param_is_server("is-server", this, is_server, "if 1, act as a server, otherwise act as a client")
	, param_server_name("server-name", this, server_name, "if acting as a client, name of server to connect to")
	, param_protocol("protocol", this, protocol, "network protocol (raw or telnet)")
	, param_filter_null_character("filter-null-character", this, filter_null_character, "Whether to filter null character")
	, param_filter_line_feed("filter-line-feed", this, filter_line_feed, "Whether to filter line feed")
	, param_enable_telnet_binary("enable-telnet-binary", this, enable_telnet_binary, "enable/disable telnet binary option")
	, param_enable_telnet_echo("enable-telnet-echo", this, enable_telnet_echo, "enable/disable telnet echo option")
	, param_enable_telnet_suppress_go_ahead("enable-telnet-suppress-go-ahead", this, enable_telnet_suppress_go_ahead, "enable/disable telnet suppress go ahead option")
	, param_enable_telnet_linemode("enable-telnet-linemode", this, enable_telnet_linemode, "enable/disable telnet linemode option")
	, input_buffer_size(0)
	, input_buffer_index(0)
	, input_buffer()
	, output_buffer()
{
	param_tcp_port.SetFormat(unisim::kernel::service::VariableBase::FMT_DEC);
	
#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
	// Loads the winsock2 dll
	WORD wVersionRequested = MAKEWORD( 2, 2 );
	WSADATA wsaData;
	if(WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		throw std::runtime_error("WSAStartup failed: Windows sockets not available");
	}
#endif

	pthread_mutex_init(&thrd_conn_create_mutex, NULL);
	pthread_mutex_init(&thrd_conn_close_mutex, NULL);
	pthread_mutex_init(&mutex, NULL);
	
	pthread_cond_init(&thrd_conn_create_cond, NULL);
	pthread_cond_init(&thrd_conn_close_cond, NULL);
}

NetStreamer::~NetStreamer()
{
	Kill();
	
	FlushOutput();
	
	CloseConnection();
	
	if(!JoinConnThread())
	{
		logger << DebugError << "Simulation thread: Can't join thread that handle connection" << EndDebugError;
	}

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
	//releases the winsock2 resources
	WSACleanup();
#endif

	pthread_mutex_destroy(&thrd_conn_create_mutex);
	pthread_mutex_destroy(&thrd_conn_close_mutex);
	pthread_mutex_destroy(&mutex);
	
	pthread_cond_destroy(&thrd_conn_create_cond);
	pthread_cond_destroy(&thrd_conn_close_cond);
}

bool NetStreamer::StartConnThread()
{
	bool status = true;
	
	if(thrd_conn_alive)
	{
		logger << DebugError << "Simulation thread: thread that handle connection has already started" << EndDebugError;
		status = false;
	}
	else
	{
		pthread_attr_t thrd_attr;
		pthread_attr_init(&thrd_attr);
		
// 		if(stack_size)
// 		{
// 			pthread_attr_setstacksize(&thrd_attr, stack_size);
// 		}
		
		pthread_mutex_lock(&thrd_conn_create_mutex);

		thrd_conn_alive = false;

		// Create a communication thread 
		if(unlikely(debug))
		{
			logger << DebugInfo << "Simulation thread: creating thread that handle connection" << EndDebugInfo;
		}
		if(pthread_create(&thrd_conn, &thrd_attr, &NetStreamer::ConnThrdEntryPoint, this) == 0)
		{
			// wait for creation of thread that handle connection
			do
			{
				pthread_cond_wait(&thrd_conn_create_cond, &thrd_conn_create_mutex);
			}
			while(!thrd_conn_alive);
			
			if(unlikely(debug))
			{
				logger << DebugInfo << "Simulation thread: thread that handle connection has started" << EndDebugInfo;
			}
		}
		else
		{
			// can't create thread that handle connection
			if(unlikely(debug))
			{
				logger << DebugInfo << "Simulation thread: can't create thread that handle connection" << EndDebugInfo;
			}
			status = false;
		}
		
		pthread_mutex_unlock(&thrd_conn_create_mutex);

		pthread_attr_destroy(&thrd_attr);
	}
	
	return status;
}

bool NetStreamer::JoinConnThread()
{
	bool status = true;
	
	if(thrd_conn_alive)
	{
		if(unlikely(debug))
		{
			logger << DebugInfo << "Simulation thread: joining thread that handle connection" << EndDebugInfo;
		}
		
		if(pthread_join(thrd_conn, NULL) == 0)
		{
			// thread that handle connection has gracefully exited
			if(unlikely(debug))
			{
				logger << DebugInfo << "Simulation thread: thread that handle connection has gracefully exited" << EndDebugInfo;
			}
			thrd_conn_alive = false;
		}
		else
		{
			// can't join thread that handle connection
			if(unlikely(debug))
			{
				logger << DebugInfo << "Simulation thread: can't join thread that handle connection" << EndDebugInfo;
			}
			status = false;
		}
	}
	
	return status;
}

void *NetStreamer::ConnThrdEntryPoint(void *_self)
{
	NetStreamer *self = static_cast<NetStreamer *>(_self);
	
	pthread_mutex_lock(&self->thrd_conn_create_mutex);
	
	self->thrd_conn_alive = true;
	pthread_cond_signal(&self->thrd_conn_create_cond);
	pthread_mutex_unlock(&self->thrd_conn_create_mutex);
	
	self->ConnThrd();
	
	return 0;
}

void NetStreamer::Lock()
{
	if(unlikely(debug))
	{
		logger << DebugInfo << "Locking socket" << EndDebugInfo;
	}
	pthread_mutex_lock(&mutex);
}

void NetStreamer::Unlock()
{
	if(unlikely(debug))
	{
		logger << DebugInfo << "Unlocking socket" << EndDebugInfo;
	}
	pthread_mutex_unlock(&mutex);
}

void NetStreamer::WaitForConnectionClose()
{
	if(unlikely(debug))
	{
		logger << DebugInfo << "Connection thread: waiting for connection close (start)" << EndDebugInfo;
	}
	if(!killed && !connection_closed)
	{
		pthread_mutex_lock(&thrd_conn_close_mutex);
		
		do
		{
			pthread_cond_wait(&thrd_conn_close_cond, &thrd_conn_close_mutex);
		}
		while(!connection_closed);
		
		pthread_mutex_unlock(&thrd_conn_close_mutex);
	}
	if(unlikely(debug))
	{
		logger << DebugInfo << "Connection thread: waiting for connection close (end)" << EndDebugInfo;
	}
}

void NetStreamer::CloseConnection()
{
	logger << DebugInfo << "Closing connection" << EndDebugInfo;
	
	if(unlikely(debug))
	{
		logger << DebugInfo << "Simulation thread: locking socket" << std::endl;
	}
	
	Lock();
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	if(sock != INVALID_SOCKET)
#else
	if(sock >= 0)
#endif
	{
		if(unlikely(debug))
		{
			logger << DebugInfo << "Simulation thread: closing connection" << EndDebugInfo;
		}
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		closesocket(sock);
		sock = INVALID_SOCKET;
#else
		close(sock);
		sock = -1;
#endif
		if(unlikely(debug))
		{
			logger << DebugInfo << "Simulation thread: let connection thread establish another connection" << EndDebugInfo;
		}
		pthread_mutex_lock(&thrd_conn_close_mutex);
		connection_closed = true;
		connection_established = false;
		pthread_cond_signal(&thrd_conn_close_cond);
		pthread_mutex_unlock(&thrd_conn_close_mutex);
	}
	if(unlikely(debug))
	{
		logger << DebugInfo << "Simulation thread: unlocking socket" << std::endl;
	}
	Unlock();
}

void NetStreamer::Kill()
{
	killed = true;
}

void NetStreamer::ConnThrd()
{
	while(!killed)
	{
		if(unlikely(debug))
		{
			logger << DebugInfo << "connection thread: locking socket" << EndDebugInfo;
		}
		Lock();
		connection_closed = false;
		connection_established = Connect();
		if(unlikely(debug))
		{
			logger << DebugInfo << "connection thread: unlocking socket" << EndDebugInfo;
		}
		Unlock();
		
		if(!connection_established)
		{
			break;
		}
		
		WaitForConnectionClose();
	}
}

void NetStreamer::WaitTime(unsigned int msec)
{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	if(!killed)
	{
		Sleep(msec);
	}
#else
	struct timespec tim_req, tim_rem;
	tim_req.tv_sec = msec / 1000;
	tim_req.tv_nsec = 1000000 * (msec % 1000);
	
	while(!killed)
	{
		int status = nanosleep(&tim_req, &tim_rem);
		
		if(status == 0) break;
		
		if(status != -1) break;
		
		if(errno != EINTR) break;

		tim_req.tv_nsec = tim_rem.tv_nsec;
	}
#endif
}

std::string NetStreamer::GetLastErrorString()
{
	std::stringstream err_sstr;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	int err_code = WSAGetLastError();
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

bool NetStreamer::Connect()
{
	logger << DebugInfo << "Using " << protocol << EndDebugInfo;
	
	if(is_server)
	{
		// Starting a server
		
		struct sockaddr_in addr;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		SOCKET server_sock;
#else
		int server_sock;
#endif

		server_sock = socket(AF_INET, SOCK_STREAM, 0);

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		if(server_sock == INVALID_SOCKET)
#else
		if(server_sock < 0)
#endif
		{
			logger << DebugError << "socket failed" << EndDebugError;
			return false;
		}

		/* Ask for non-blocking accept on server socket */
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		u_long ServerSocketNonBlock = 1;
		if(ioctlsocket(server_sock, FIONBIO, &ServerSocketNonBlock) != 0)
		{
			logger << DebugError << "ioctlsocket(FIONBIO) on server socked failed (" << GetLastErrorString() << ")" << EndDebugError;
			closesocket(server_sock);
			return false;
		}
#else
		int server_socket_flag = fcntl(server_sock, F_GETFL, 0);

		if(server_socket_flag < 0)
		{
			logger << DebugError << "fcntl(F_GETFL) on server socket failed (" << GetLastErrorString() << ")" << EndDebugError;
			close(server_sock);
			return false;
		}

		if(fcntl(server_sock, F_SETFL, server_socket_flag | O_NONBLOCK) < 0)
		{
			logger << DebugError << "fcntl(F_SETFL, O_NONBLOCK) on server socket failed (" << GetLastErrorString() << ")" << EndDebugError;
			close(server_sock);
			return false;
		}
#endif
		/* ask for reusing TCP port */
		int opt = 1;
		if(setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt)) < 0)
		{
			if(verbose)
			{
				logger << DebugWarning << "setsockopt failed requesting port reuse" << EndDebugWarning;
			}
		}

		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(tcp_port);
		addr.sin_addr.s_addr = INADDR_ANY;
		if(bind(server_sock, (struct sockaddr *) &addr, sizeof(addr)) < 0)
		{
			logger << DebugError << "Bind failed. TCP Port #" << tcp_port << " may be already in use. Please specify another port in " << param_tcp_port.GetName() << EndDebugError;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
			closesocket(server_sock);
#else
			close(server_sock);
#endif
			return false;
		}

		if(listen(server_sock, 1))
		{
			logger << DebugError << "Listen failed" << EndDebugError;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
			closesocket(server_sock);
#else
			close(server_sock);
#endif
			return false;
		}

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		int addr_len;
#else
		socklen_t addr_len;
#endif

		logger << DebugInfo << "Listening on TCP port " << tcp_port << EndDebugInfo;
		addr_len = sizeof(addr);
		while(!killed)
		{
			sock = accept(server_sock, (struct sockaddr *) &addr, &addr_len);

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
			if(sock != INVALID_SOCKET)
#else
			if(sock >= 0)
#endif
			{
				logger << DebugInfo << "Connection with client established" << EndDebugInfo;
				break;
			}
			
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
			if(WSAGetLastError() != WSAEWOULDBLOCK)
#else
			if((errno != EAGAIN) && (errno != EWOULDBLOCK))
#endif
			{
				logger << DebugError << "accept failed (" << GetLastErrorString() << ")" << EndDebugError;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
				closesocket(server_sock);
#else
				close(server_sock);
#endif
				return false;
			}
				
			WaitTime(CONNECTION_POLL_PERIOD_MS); // retry later
		}

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		closesocket(server_sock);
#else
		close(server_sock);
#endif
		
		if(killed)
		{
			logger << DebugInfo << "Server shuts down" << EndDebugInfo;
			return false;
		}
	}
	else
	{
		// Acting as a client
		
		bool connected = false;
		struct sockaddr_in sonadr;

		sock = socket(PF_INET, SOCK_STREAM, 0);

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		if(sock == INVALID_SOCKET)
#else
		if(sock < 0)
#endif
		{
			logger << DebugError << "Can't create socket for connection to " << server_name << ":" << tcp_port << EndDebugError;
			return false;
		}
		memset((char *) &sonadr, 0, sizeof(sonadr));
		sonadr.sin_family = AF_INET;
		sonadr.sin_port = htons(tcp_port);
		sonadr.sin_addr.s_addr = inet_addr(server_name.c_str());

		logger << DebugInfo << "Trying to connect to " << server_name << EndDebugInfo;

		while(!killed && !connected)
		{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
			u_long addr_none = INADDR_NONE;
#else
			in_addr_t addr_none = INADDR_NONE;
#endif
			if(memcmp(&sonadr.sin_addr.s_addr, &addr_none, sizeof(addr_none)) != 0)
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
					logger << DebugError << "Can't determine IP address from host name for" << server_name << ":" << tcp_port << EndDebugError;
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
				logger << DebugWarning << "Can't connect to " << server_name << ":" << tcp_port << EndDebugWarning;
			}
			
			WaitTime(CONNECTION_POLL_PERIOD_MS); // retry later
		}

		if(killed)
		{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
			closesocket(sock);
#else
			close(sock);
#endif
			logger << DebugInfo << "Client abandons his connection attempt" << EndDebugInfo;
			return false;
		}
		
		logger << DebugInfo << "Connection with " << server_name << " established" << EndDebugInfo;
	}
	
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	u_long NonBlock = 1;
	if(ioctlsocket(sock, FIONBIO, &NonBlock) != 0)
	{
		logger << DebugError << "ioctlsocket failed" << EndDebugError;
		closesocket(sock);
		sock = -1;
		return false;
	}
#else
	int socket_flag = fcntl(sock, F_GETFL, 0);

	if(socket_flag < 0)
	{
		logger << DebugError << "fcntl failed" << EndDebugError;
		close(sock);
		sock = -1;
		return false;
	}

	/* Ask for non-blocking reads on socket */
	if(fcntl(sock, F_SETFL, socket_flag | O_NONBLOCK) < 0)
	{
		logger << DebugError << "fcntl failed" << EndDebugError;
		close(sock);
		sock = -1;
		return false;
	}
#endif

	if(protocol == NETSTREAMER_PROTOCOL_TELNET)
	{
		EarlyPut(TELNET_OPT_LINEMODE);
		EarlyPut(enable_telnet_linemode ? TELNET_CMD_WILL : TELNET_CMD_WONT);
		EarlyPut(TELNET_CMD_IAC);

		EarlyPut(TELNET_OPT_SUPPRESS_GO_AHEAD);
		EarlyPut(enable_telnet_suppress_go_ahead ? TELNET_CMD_WILL : TELNET_CMD_WONT);
		EarlyPut(TELNET_CMD_IAC);

		EarlyPut(TELNET_OPT_ECHO);
		EarlyPut(enable_telnet_echo ? TELNET_CMD_WILL : TELNET_CMD_WONT);
		EarlyPut(TELNET_CMD_IAC);

		EarlyPut(TELNET_OPT_BINARY);
		EarlyPut(enable_telnet_binary ? TELNET_CMD_WILL : TELNET_CMD_WONT);
		EarlyPut(TELNET_CMD_IAC);
		
		FlushOutput();
	}

	return true;
}

bool NetStreamer::EndSetup()
{
	if(!StartConnThread()) return false;
	
	return true;
}

void NetStreamer::FlushOutput()
{
	if(!connection_established) return;
	
	if(unlikely(debug))
	{
		logger << DebugInfo << "simulation thread: locking socket" << EndDebugInfo;
	}
	Lock();
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	if(sock != INVALID_SOCKET)
#else
	if(sock >= 0)
#endif
	{
		unsigned int output_buffer_size = output_buffer.size();
	
		if(output_buffer_size > 0)
		{
			if(verbose)
			{
				logger << DebugInfo << "sending " << output_buffer_size << " chars" << EndDebugInfo;
			}
			unsigned int index = 0;
			do
			{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
				int r = send(sock, (const char *) &output_buffer[index], output_buffer_size, 0);
				if((r == 0) || (r == SOCKET_ERROR))
#else
				ssize_t r = write(sock, &output_buffer[index], output_buffer_size);
				if(r <= 0)
#endif
				{
					logger << DebugWarning << "can't write into socket" << EndDebugWarning;
					if(unlikely(debug))
					{
						logger << DebugInfo << "simulation thread: unlocking socket" << EndDebugInfo;
					}
					Unlock();
					CloseConnection();
					return;
				}

				index += r;
				output_buffer_size -= r;
			}
			while(output_buffer_size > 0);
			
			output_buffer.clear();
		}
	}
	if(unlikely(debug))
	{
		logger << DebugInfo << "simulation thread: unlocking socket" << EndDebugInfo;
	}
	Unlock();
}

void NetStreamer::EarlyPut(uint8_t v)
{
	if(output_buffer.empty())
	{
		output_buffer.push_back(v);
	}
	else
	{
		output_buffer.insert(output_buffer.begin(), v);
	}
}

void NetStreamer::Put(uint8_t v)
{
	unsigned int output_buffer_size = output_buffer.size();

	if(output_buffer_size >= AUTO_FLUSH_OUTPUT_SIZE)
	{
		FlushOutput();
	}

	output_buffer.push_back(v);
}

bool NetStreamer::Get(uint8_t& v)
{
	if(!connection_established) return false;
	
	if(unlikely(debug))
	{
		logger << DebugInfo << "simulation thread: locking socket" << EndDebugInfo;
	}
	Lock();
	
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	if(sock != INVALID_SOCKET)
#else
	if(sock >= 0)
#endif
	{
		if(input_buffer_size == 0)
		{
			do
			{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
				int r = recv(sock, (char *) input_buffer, sizeof(input_buffer), 0);
				if(r == 0 || r == SOCKET_ERROR)
#else
				ssize_t r = read(sock, input_buffer, sizeof(input_buffer));
				if(r <= 0)
#endif
				{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
					if((r == SOCKET_ERROR) && (WSAGetLastError() == WSAEWOULDBLOCK))
#else
					if(r < 0 && errno == EAGAIN)
#endif
					{
						if(unlikely(debug))
						{
							logger << DebugInfo << "simulation thread: unlocking socket" << EndDebugInfo;
						}
						Unlock();
						return false;
					}

					logger << DebugWarning << "can't read from socket" << EndDebugWarning;
					Unlock();
					CloseConnection();
					return false;
				}
				input_buffer_index = 0;
				input_buffer_size = r;
				break;
			} while(1);
		}
	}

	v = input_buffer[input_buffer_index++];
	input_buffer_size--;
	
	if(unlikely(debug))
	{
		logger << DebugInfo << "simulation thread: unlocking socket" << EndDebugInfo;
	}
	Unlock();
	
	return true;
}

void NetStreamer::Reset()
{
	input_buffer_size = 0;
	input_buffer_index = 0;
	output_buffer.clear();
}

bool NetStreamer::GetChar(char& c)
{
	uint8_t v;

	do
	{
		if(!Get(v)) return false;

		switch(state)
		{
			case 0:
				if((protocol == NETSTREAMER_PROTOCOL_TELNET) && (v == TELNET_CMD_IAC))
				{
					state = 1;
					break;
				}
				if((v == 0) && filter_null_character) break; // filter null character
				if((v == 10) && filter_line_feed) break; // filter line feed
				c = (char) v;
				if(unlikely(verbose))
				{
					logger << DebugInfo << "Getting character ";
					if((v >= 32) && (v < 128))
						logger << "'" << c << "'";
					else
						logger << "0x" << std::hex << (unsigned int) v << std::dec;
					logger << EndDebugInfo;
				}
				return true;
				
			case 1: // got an IAC
				switch(v)
				{
					case TELNET_CMD_IAC:
						c = (char) v;
						return true;
					case TELNET_CMD_DONT:
						state = 2;
						break;
					case TELNET_CMD_DO:
						state = 3;
						break;
					case TELNET_CMD_WONT:
						state = 4;
						break;
					case TELNET_CMD_WILL:
						state = 5;
						break;
					case TELNET_CMD_SB:
						state = 6;
						break;
					default:
						state = 0;
						break;
				}
				break;
			case 2: // got a DONT
				TelnetDont(v);
				state = 0;
				break;
			case 3: // got a DO
				TelnetDo(v);
				state = 0;
				break;
			case 4: // got a WONT
				TelnetWont(v);
				state = 0;
				break;
			case 5: // got a WILL
				TelnetWill(v);
				state = 0;
				break;
			case 6: // got a SB
				telnet_sb_opt = v;
				telnet_sb_params.clear();
				state = 7;
				break;
			case 7:
				if(v == TELNET_CMD_SE)
				{
					unsigned int num_params = telnet_sb_params.size();
					uint8_t sb_params[num_params];
					unsigned int i;
					for(i = 0; i < num_params; i++)
					{
						sb_params[i] = telnet_sb_params[i];
					}
					TelnetSubNegociation(telnet_sb_opt, sb_params, num_params);
					telnet_sb_params.clear();
					state = 0;
					break;
				}
				else
				{
					telnet_sb_params.push_back(v);
				}
				break;
		}
	}
	while(1);

	return false;
}

void NetStreamer::PutChar(char c)
{
	uint8_t v = (uint8_t) c;
	
	if(verbose)
	{
		logger << DebugInfo << "Putting character ";
		if(v >= 32)
			logger << "'" << c << "'";
		else
			logger << "0x" << std::hex << (unsigned int) v << std::dec;
		logger << EndDebugInfo;
	}
	Put(v);
	if((protocol == NETSTREAMER_PROTOCOL_TELNET) && (v == TELNET_CMD_IAC)) Put(v);
}

void NetStreamer::FlushChars()
{
	FlushOutput();
}

const char *NetStreamer::TelnetGetOptName(uint8_t opt) const
{
	switch(opt)
	{
		case TELNET_OPT_ECHO: return "ECHO";
		case TELNET_OPT_BINARY: return "BINARY";
		case TELNET_OPT_LINEMODE: return "LINEMODE";
		case TELNET_OPT_SUPPRESS_GO_AHEAD: return "SUPPRESS_GO_AHEAD";
	}
	return 0;
}

void NetStreamer::TelnetDont(uint8_t opt)
{
	if(unlikely(verbose))
	{
		const char *opt_name = TelnetGetOptName(opt);
		logger << DebugInfo << "DONT ";
		if(opt_name)
		{
			logger << opt_name;
		}
		else
		{
			logger << (unsigned int) opt;
		}
		logger << EndDebugInfo;
	}
}

void NetStreamer::TelnetDo(uint8_t opt)
{
	if(unlikely(verbose))
	{
		const char *opt_name = TelnetGetOptName(opt);
		logger << DebugInfo << "DO ";
		if(opt_name)
		{
			logger << opt_name;
		}
		else
		{
			logger << (unsigned int) opt;
		}
		logger << EndDebugInfo;
	}
}

void NetStreamer::TelnetWont(uint8_t opt)
{
	if(unlikely(verbose))
	{
		const char *opt_name = TelnetGetOptName(opt);
		logger << DebugInfo << "WONT ";
		if(opt_name)
		{
			logger << opt_name;
		}
		else
		{
			logger << (unsigned int) opt;
		}
		logger << EndDebugInfo;
	}
}

void NetStreamer::TelnetWill(uint8_t opt)
{
	if(unlikely(verbose))
	{
		const char *opt_name = TelnetGetOptName(opt);
		logger << DebugInfo << "WILL ";
		if(opt_name)
		{
			logger << opt_name;
		}
		else
		{
			logger << (unsigned int) opt;
		}
		logger << EndDebugInfo;
	}
}

void NetStreamer::TelnetSubNegociation(uint8_t opt, uint8_t *params, unsigned int num_params)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo << "SB " << TelnetGetOptName(opt);
		unsigned int i;
		for(i = 0; i < num_params; i++)
		{
			logger << (unsigned int) params[i];
			if(i != (num_params - 1)) logger << " ";
		}
		logger << EndDebugInfo;
	}
}

} // end of namespace netstreamer
} // end of namespace service
} // end of namespace unisim

namespace unisim {
namespace kernel {
namespace service {

using unisim::service::netstreamer::NetStreamerProtocol;
using unisim::service::netstreamer::NETSTREAMER_PROTOCOL_RAW;
using unisim::service::netstreamer::NETSTREAMER_PROTOCOL_TELNET;

template <> Variable<NetStreamerProtocol>::Variable(const char *_name, Object *_object, NetStreamerProtocol& _storage, Type type, const char *_description) :
	VariableBase(_name, _object, type, _description), storage(&_storage)
{
	Simulator::simulator->Initialize(this);
	AddEnumeratedValue("raw");
	AddEnumeratedValue("telnet");
}

template <>
const char *Variable<NetStreamerProtocol>::GetDataTypeName() const
{
	return "netstreamer-protocol";
}

template <>
unsigned int Variable<NetStreamerProtocol>::GetBitSize() const
{
	return 1;
}

template <> Variable<NetStreamerProtocol>::operator bool () const { return *storage != NETSTREAMER_PROTOCOL_TELNET; }
template <> Variable<NetStreamerProtocol>::operator long long () const { return *storage; }
template <> Variable<NetStreamerProtocol>::operator unsigned long long () const { return *storage; }
template <> Variable<NetStreamerProtocol>::operator double () const { return (double)(*storage); }
template <> Variable<NetStreamerProtocol>::operator std::string () const
{
	switch(*storage)
	{
		case NETSTREAMER_PROTOCOL_TELNET: return std::string("telnet");
		case NETSTREAMER_PROTOCOL_RAW: return std::string("raw");
	}
	return std::string("?");
}

template <> VariableBase& Variable<NetStreamerProtocol>::operator = (bool value)
{
	if(IsMutable())
	{
		NetStreamerProtocol tmp = *storage;
		switch((unsigned int) value)
		{
			case NETSTREAMER_PROTOCOL_TELNET:
			case NETSTREAMER_PROTOCOL_RAW:
				tmp = (NetStreamerProtocol)(unsigned int) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<NetStreamerProtocol>::operator = (long long value)
{
	if(IsMutable())
	{
		NetStreamerProtocol tmp = *storage;
		switch(value)
		{
			case NETSTREAMER_PROTOCOL_TELNET:
			case NETSTREAMER_PROTOCOL_RAW:
				tmp = (NetStreamerProtocol) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<NetStreamerProtocol>::operator = (unsigned long long value)
{
	if(IsMutable())
	{
		NetStreamerProtocol tmp = *storage;
		switch(value)
		{
			case NETSTREAMER_PROTOCOL_TELNET:
			case NETSTREAMER_PROTOCOL_RAW:
				tmp = (NetStreamerProtocol) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<NetStreamerProtocol>::operator = (double value)
{
	if(IsMutable())
	{
		NetStreamerProtocol tmp = *storage;
		switch((unsigned int) value)
		{
			case NETSTREAMER_PROTOCOL_TELNET:
			case NETSTREAMER_PROTOCOL_RAW:
				tmp = (NetStreamerProtocol)(unsigned int) value;
				break;
		}
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template <> VariableBase& Variable<NetStreamerProtocol>::operator = (const char *value)
{
	if(IsMutable())
	{
		NetStreamerProtocol tmp = *storage;
		if(std::string(value) == std::string("telnet")) tmp = NETSTREAMER_PROTOCOL_TELNET;
		else if(std::string(value) == std::string("raw")) tmp = NETSTREAMER_PROTOCOL_RAW;
		SetModified(*storage != tmp);
		*storage = tmp;
	}
	return *this;
}

template class Variable<NetStreamerProtocol>;

} // end of service namespace
} // end of kernel namespace
} // end of unisim namespace
