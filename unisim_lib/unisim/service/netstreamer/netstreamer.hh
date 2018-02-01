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
 
#ifndef __UNISIM_SERVICE_NETSTREAMER_NETSTREAMER_HH__
#define __UNISIM_SERVICE_NETSTREAMER_NETSTREAMER_HH__

#include <unisim/service/interfaces/char_io.hh>
#include <unisim/kernel/service/service.hh>
#include <unisim/kernel/logger/logger.hh>
#include <string>
#include <vector>
#include <inttypes.h>
#include <pthread.h>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)

#include <winsock2.h>

#endif

namespace unisim {
namespace service {
namespace netstreamer {

enum NetStreamerProtocol
{
	NETSTREAMER_PROTOCOL_RAW,    // convenient for use with netcat
	NETSTREAMER_PROTOCOL_TELNET, // convenient for use with telnet
};

std::ostream& operator << (std::ostream& os, const NetStreamerProtocol& nsp);

class NetStreamer : public unisim::kernel::service::Service<unisim::service::interfaces::CharIO>
{
public:
	static const unsigned int AUTO_FLUSH_OUTPUT_SIZE = 256;
	static const uint64_t CONNECTION_POLL_PERIOD_MS = 1000  /* ms */;  // every 1000 ms
	
	unisim::kernel::service::ServiceExport<unisim::service::interfaces::CharIO> char_io_export;
	
	NetStreamer(const char *name, unisim::kernel::service::Object *parent = 0);
	virtual ~NetStreamer();

	virtual bool EndSetup();
	virtual void Reset();
	virtual bool GetChar(char& c);
	virtual void PutChar(char c);
	virtual void FlushChars();
private:
	unisim::kernel::logger::Logger logger;
	bool verbose;
	bool debug;
	bool is_server;
	std::string server_name;
	NetStreamerProtocol protocol;
	bool filter_null_character;
	bool filter_line_feed;
	bool enable_telnet_binary;
	bool enable_telnet_echo;
	bool enable_telnet_suppress_go_ahead;
	bool enable_telnet_linemode;

	int tcp_port;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	SOCKET sock;
#else
	int sock;
#endif
	int state;
	uint8_t telnet_sb_opt;
	std::vector<uint8_t> telnet_sb_params;
	bool thrd_conn_alive;
	bool killed;
	pthread_t thrd_conn;
	pthread_mutex_t thrd_conn_create_mutex;
	pthread_cond_t thrd_conn_create_cond;
	
	pthread_mutex_t thrd_conn_close_mutex;
	pthread_cond_t thrd_conn_close_cond;
	bool connection_closed;
	bool connection_established;
	
	pthread_mutex_t mutex;

	unisim::kernel::service::Parameter<bool> param_verbose;
	unisim::kernel::service::Parameter<bool> param_debug;
	unisim::kernel::service::Parameter<int> param_tcp_port;
	unisim::kernel::service::Parameter<bool> param_is_server;
	unisim::kernel::service::Parameter<std::string> param_server_name;
	unisim::kernel::service::Parameter<NetStreamerProtocol> param_protocol;
	unisim::kernel::service::Parameter<bool> param_filter_null_character;
	unisim::kernel::service::Parameter<bool> param_filter_line_feed;
	unisim::kernel::service::Parameter<bool> param_enable_telnet_binary;
	unisim::kernel::service::Parameter<bool> param_enable_telnet_echo;
	unisim::kernel::service::Parameter<bool> param_enable_telnet_suppress_go_ahead;
	unisim::kernel::service::Parameter<bool> param_enable_telnet_linemode;


	unsigned int input_buffer_size;
	unsigned int input_buffer_index;
	uint8_t input_buffer[256];

	std::vector<uint8_t> output_buffer;
	
	bool StartConnThread();
	bool JoinConnThread();
	static void *ConnThrdEntryPoint(void *_self);
	void Lock();
	void Unlock();
	void WaitForConnectionClose();
	void CloseConnection();
	void Kill();
	void ConnThrd();
	void WaitTime(unsigned int msec);
	std::string GetLastErrorString();
	bool Connect();
	void FlushOutput();
	void EarlyPut(uint8_t v);
	void Put(uint8_t v);
	bool Get(uint8_t& v);
	const char *TelnetGetOptName(uint8_t opt) const;
	void TelnetDont(uint8_t opt);
	void TelnetDo(uint8_t opt);
	void TelnetWont(uint8_t opt);
	void TelnetWill(uint8_t opt);
	void TelnetSubNegociation(uint8_t opt, uint8_t *params, unsigned int num_params);
};

} // end of namespace netstreamer
} // end of namespace service
} // end of namespace unisim


#endif
