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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#ifndef __UNISIM_UTIL_HYPAPP_HYPAPP_HH__
#define __UNISIM_UTIL_HYPAPP_HYPAPP_HH__

#include <string>
#include <iostream>
#include <inttypes.h>

namespace unisim {
namespace util {
namespace hypapp {

struct HttpServer;

struct ClientConnection
{
  ClientConnection(HttpServer& _http_server, int _socket) : http_server(_http_server), socket(_socket) {}
  
  template <uintptr_t SZ>
  bool Send(char const (&text)[SZ]) const { return Send( &text[0], SZ-1 ); }
  bool Send( std::string const& text ) const { return Send( &text[0], text.size() ); }
  bool Send(char const* buf, uintptr_t size) const;
  
  HttpServer& http_server;
  int         socket;
};

struct MessageLoop;

struct Request
{
	enum request_type_t { GET, HEAD, POST };
	
	request_type_t GetRequestType() const { return request_type; }
	char const* GetRequestURI() const { return uri ? (ibuf + uri) : 0; }
	char const* GetHost() const { return host ? (ibuf + host) : 0; }
	char const* GetContentType() const { return content_type ? (ibuf + content_type) : 0; }
	unsigned int GetContentLength() const { return content_length; }
	char const* GetContent() const { return content ? (ibuf + content) : 0; }
	
private:
	Request() {}
	Request(Request const &) {}
	Request& operator = (Request const&) { return *this; }
	
	friend struct MessageLoop;
	
	char const *ibuf;
	request_type_t request_type;
	unsigned int uri;
	unsigned int host;
	unsigned int content_type;
	unsigned int content_length;
	unsigned int content;
};

std::ostream& operator << (std::ostream& os, const Request::request_type_t& request_type);
std::ostream& operator << (std::ostream& os, const Request& req);

struct MessageLoop
{
  MessageLoop(HttpServer& _http_server, std::ostream& _log = std::cout, std::ostream& _warn_log = std::cout, std::ostream& _err_log = std::cerr)
    : http_server(_http_server), log(_log), warn_log(_warn_log), err_log(_err_log) {}
  virtual ~MessageLoop() {}
  
  void Run(ClientConnection const& conn);
  
  virtual bool SendResponse(Request const& request, ClientConnection const& conn ) = 0;
protected:
  HttpServer& http_server;
  // Log
  std::ostream& log;
  std::ostream& warn_log;
  std::ostream& err_log;
};

struct Form_URL_Encoded_Decoder
{
	bool Decode(const std::string& s, std::ostream& err_log = std::cerr);
	virtual bool FormAssign(const std::string& name, const std::string& value) = 0;
};

struct Encoder
{
	static std::string Encode(const std::string& s);
};

struct URL_AbsolutePathDecoder
{
	static bool Decode(const std::string& url, std::string& abs_path, std::ostream& err_log = std::cerr);
};

struct HttpServer
{
  HttpServer();
  HttpServer( int _port, int _maxclients);
  virtual ~HttpServer();

  void SetTCPPort(int _port);
  void SetMaxClients(int _maxclients);
  void SetLog(std::ostream& os);
  void SetWarnLog(std::ostream& os);
  void SetErrLog(std::ostream& os);
  void StartLoopThread();
  void Kill();
  void JoinLoopThread();
  bool Killed() const;
  bool Verbose() const;

  // Content exchange callbacks
  virtual void Serve(ClientConnection const& conn) = 0;

protected:
  // Loop Thread
  pthread_t tid;
  
  // Configuration
  static bool const ALLOW_REUSE_PORT_NUMBER = true;
  int             port;
  int             maxclients;
  
  // State
  pthread_mutex_t mutex;
  int             clients;
  int             socket;
  bool            killed;
  bool            running;
  
  // Log
  bool verbose;
  std::ostream* log;
  std::ostream* warn_log;
  std::ostream* err_log;
  
private:
  void  Init();
  void  LoopThread();
};

} // end of namespace hypapp
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_HYPAPP_HYPAPP_HH__
