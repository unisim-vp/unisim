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

#include <unisim/util/hypapp/hypapp.hh>
#include <iostream>
#include <vector>

#include <cstring>
#include <cstdlib>

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
#include <poll.h>
#include <ctype.h>

#endif

namespace unisim {
namespace util {
namespace hypapp {

namespace 
{
  char const*
  GetHeaderEnd( char const* buf, uintptr_t size )
  {
    for (char const* end = &buf[size-3]; buf < end;)
      {
        if       (buf[3] == '\r')                          buf += 1;
        else if ((buf[2] == '\r') and (buf[3] == '\n'))
          {
            if  ((buf[0] == '\r') and (buf[1] == '\n'))    return buf;
            else                                           buf += 2;
          }
        else                                               buf += 4;
      }
    
    return 0;
  }
  
  char const*
  streat( char const* ptr, char const* from )
  {
    while (*ptr and tolower(*ptr) == tolower(*from))
      ++ptr, ++from;
    return *ptr ? 0 : from;
  }
  
  void sleep(unsigned int msec)
  {
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
    Sleep(msec);
#else
    struct timespec tim_req, tim_rem;
    tim_req.tv_sec = msec / 1000;
    tim_req.tv_nsec = 1000000 * (msec % 1000);
    
    for(;;)
    {
      int status = nanosleep(&tim_req, &tim_rem);
      
      if(status == 0) break;
      
      if(status != -1) break;
      
      if(errno != EINTR) break;
      
      tim_req.tv_nsec = tim_rem.tv_nsec;
    }
#endif
  }
}

struct IPAddrRepr
{
  IPAddrRepr( uint32_t _ipaddr ) : addr(ntohl(_ipaddr)) {} uint32_t addr;
  friend std::ostream& operator << (std::ostream& sink, IPAddrRepr const& ip)
  {
    sink << unsigned((ip.addr << 3*8) >> 24) << "."
         << unsigned((ip.addr << 2*8) >> 24) << "."
         << unsigned((ip.addr << 1*8) >> 24) << "."
         << unsigned((ip.addr << 0*8) >> 24);
    return sink;
  }
};

void MessageLoop::Run(ClientConnection const& conn)
{
  std::vector<char> ibuf;
  ibuf.reserve(4096);
    
  for (;;)
    {
      char const* ptr = &ibuf[0];
#if 1
      {
        uintptr_t skip = 0, size = ibuf.size();
        while ((size-skip) >= 2 and ptr[skip] == '\r' and ptr[skip+1] == '\n')
          skip += 2;
        if (skip)
          ibuf.erase( ibuf.begin(), ibuf.begin() + skip );
        ptr = &ibuf[0];
      }
#endif

      char const* soh = ptr;
      char const* eoh = GetHeaderEnd(ptr, ibuf.size());
        
      if (not eoh)
        {
          /* Partial header */
          if(ibuf.size() == ibuf.capacity()) ibuf.reserve(ibuf.capacity() + 4096);
          uintptr_t resume = ibuf.size(), end = ibuf.capacity();
          ibuf.resize(end);
          if(end == resume) throw 0;
          if(http_server.Killed()) return;
          if(http_server.Verbose())
          {
            log << "trying to receive " << (end - resume) << " bytes" << std::endl;
          }
          intptr_t bytes = recv(conn.socket, &ibuf[resume], end - resume, 0);
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
          if((bytes == 0) || (bytes == SOCKET_ERROR))
#else
          if(bytes <= 0)
#endif
          {
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
            if(bytes == SOCKET_ERROR)
#else
            if(bytes < 0)
#endif
            {
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
              if(WSAGetLastError() == WSAEWOULDBLOCK)
#else
              if((errno == EAGAIN) || (errno == EWOULDBLOCK))
#endif
              {
                sleep(1); // try again later
              }
              else
              {
                err_log << "[" << conn.socket << "] recv error\n"; return; 
              }
            }
            else
            {
              if(resume)
              {
                err_log << "[" << conn.socket << "] header short read (got " << resume << " bytes)\n";
              }  
              return; 
            }
            ibuf.resize(resume);
          }
          else
          {
            ibuf.resize(resume + bytes);
            if(http_server.Verbose())
            {
              log << "received " << bytes << " bytes [";
              for(intptr_t i = 0; i < bytes; i++)
              {
                if(i) log << ' ';
                char c = ibuf[resume + i];
                if((c >= 32) && (c <= 126))
                {
                  log << '\'' << c << '\'';
                }
                else
                {
                  log << '\'' << "0x" << std::hex << (unsigned int) c << '\'' << std::dec;
                }
              }
              log << "]" << std::endl;
            }
          }
          continue;
        }
        
      eoh += 2; // first "\r\n" is inside header
      uintptr_t request_size = eoh - ptr;
      ibuf[request_size] = '\0';
      request_size += 2; // second "\r\n" before next part

      Request request;
      if      (char const* p = streat("GET ", ptr))  { ptr = p; request.request_type = Request::GET; }
      else if (char const* p = streat("HEAD ", ptr)) { ptr = p; request.request_type = Request::HEAD; }
      else if (char const* p = streat("POST ", ptr)) { ptr = p; request.request_type = Request::POST; }
      else {
        if (char const* p = strchr(ptr, '\r'))
          ibuf[p-soh] = '\0';
        err_log << "unknown method in: " << ptr << std::endl;
        return;
      }
      
      {
        char const* uri = ptr;
        if (not (ptr = strchr(uri, ' '))) { err_log << "[" << conn.socket << "] URI parse error\n"; return; }
        ibuf[ptr++-soh] = '\0';
        if(http_server.Verbose())
        {
          log << "[" << conn.socket << "] URI: " << uri << std::endl;
        }
        request.uri = uri - &ibuf[0];
      }
      
      if (not (ptr = streat("HTTP/1.", ptr))) { err_log << "[" << conn.socket << "] HTTP version parse error\n"; return; }
      
      {
        unsigned http_version = 0;
        for (unsigned digit; *ptr != '\r'; ++ptr) {
          if ((digit = (*ptr - '0')) >= 10) { err_log << "[" << conn.socket << "] HTTP version parse error\n"; return; }
          http_version = 10*http_version+digit;
        }
        if (http_version != 1)
          { err_log << "[" << conn.socket << "] HTTP/1." << http_version <<  " != 1 makes me nervous\n"; return; }
      }
      if (not (ptr = streat("\r\n", ptr))) { err_log << "[" << conn.socket << "] HTTP request parse error\n"; return; }
        
      bool keep_alive = false;
      int cseq = 0;
	  unsigned content_length = 0;
	  bool var_content = false;
        
      while (ptr < eoh)
        {
          char const* key = ptr;
          if (not (ptr = strchr(ptr, ':'))) { err_log << "[" << conn.socket << "] HTTP headers parse error\n"; return; }
          ibuf[ptr++-soh] = '\0';
          while (isblank(*ptr)) ptr += 1;
          char const* val = ptr;
          if (not (ptr = strchr(ptr, '\r'))) { err_log << "[" << conn.socket << "] HTTP headers parse error\n"; return; }
          for (char const* p = ptr; (--p > val) and isspace(*p);) ibuf[p-soh] = '\0';
          ibuf[ptr++-soh] = '\0';
          if (*ptr++ != '\n') { err_log << "[" << conn.socket << "] HTTP headers parse error\n"; return; }
            
          // header field parsed
            
          if      (streat("Connection", key))
            {
              keep_alive = streat("Keep-Alive", val);
              if(http_server.Verbose())
              {
                log << "[" << conn.socket << "] keep alive: " << keep_alive << "\n";
              }
            }
          else if (streat("Content-Type", key))
            {
              if (char const* b = streat("multipart/form-data; boundary=",val))
                { err_log << "[" << conn.socket << "] MP boundary:" << b << "\n"; return; }
              else { if(http_server.Verbose()) { log << "[" << conn.socket << "] " << key << ": " << val << "\n"; request.content_type = val - &ibuf[0]; } }
            }
          else if (streat("CSeq", key))
            { cseq = strtoul( val, 0, 0 ); if(http_server.Verbose()) { log << "[" << conn.socket << "] cseq:" << cseq << "\n"; } }
          else if (streat("Host", key) or
                   streat("User-Agent", key) or
                   streat("Referer", key) or
                   streat("Accept", key) or
                   streat("Upgrade-Insecure-Requests",key) or
                   streat("If-Modified-Since", key) or
                   streat("Cache-Control", key) or
                   streat("DNT", key) or
                   streat("Origin", key))
            { if(http_server.Verbose()) { log << "[" << conn.socket << "] " << key << ": " << val << "\n"; } }
          else if (streat("Range", key))
            { err_log << "[" << conn.socket << "] " << key << ": " << val << "\n"; return; }
          else if (streat("Content-Length", key))
            { if(http_server.Verbose()) { log << "[" << conn.socket << "] " << key << ": " << val << "\n"; }
              content_length = 0;
              for (unsigned digit; *val != 0; ++val) {
                if ((digit = (*val - '0')) >= 10) { err_log << "[" << conn.socket << "] Content length parse error\n"; return; }
                content_length = 10*content_length+digit;
              }
              if(content_length) request.content_length = content_length;
              var_content = (content_length == 0);
              if(var_content) keep_alive = false;
            }
          else
            { err_log << "[" << conn.socket << "] error: {key:" << key << ", val:" << val << "}\n"; return; }
        }
        
      if(http_server.Verbose())
      {
        log << "Header (size: " << request_size << ") received and processed.\n";
        switch (request.request_type) {
        case Request::GET: log << "Get request." << std::endl; break;
        case Request::HEAD: log << "Head request." << std::endl; break;
        case Request::POST: log << "Post request." << std::endl; break;
        }
      }
      
      if(var_content || (content_length != 0))
      {
        if(content_length != 0)
        {
          ibuf.reserve(request_size + content_length); // header + content
        }
        
        unsigned int rem_content_length = content_length;

        unsigned int lookead_size = ibuf.size() - request_size;
        
        if(lookead_size != 0)
        {
          if(var_content)
          {
            content_length = lookead_size;
          }
          else
          {
            rem_content_length -= lookead_size;
          }
        }
      
        while(var_content || (rem_content_length != 0))
        {
          if(var_content && (ibuf.size() == ibuf.capacity())) ibuf.reserve(ibuf.capacity() + 4096);
          uintptr_t resume = ibuf.size(), end = var_content ? (ibuf.capacity() /*- 1*/): (request_size + content_length);
          ibuf.resize(end);
          if(http_server.Killed()) return;
          int bytes = recv(conn.socket, &ibuf[resume], end - resume, 0);
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
          if((bytes == 0) || (bytes == SOCKET_ERROR))
#else
          if(bytes <= 0)
#endif
          {
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
            if(bytes == SOCKET_ERROR)
#else
            if(bytes < 0)
#endif
            {
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
              if(WSAGetLastError() == WSAEWOULDBLOCK)
#else
              if((errno == EAGAIN) || (errno == EWOULDBLOCK))
#endif
              {
                sleep(1); // try again later
                ibuf.resize(resume);
                continue;
              }
              else
              {
                err_log << "[" << conn.socket << "] recv error\n"; return; 
              }
            }
            
            // connection closed by peer
            if(var_content) break;
            if(rem_content_length)
            {
              err_log << "[" << conn.socket << "] content short read (" << (content_length - rem_content_length) << " instead of " << content_length << " bytes)\n";
            }
            return; 
          }
          
          ibuf.resize(resume + bytes);

          if(var_content) content_length += bytes; else rem_content_length -= bytes;
        }
        
        if(http_server.Verbose())
        {
          log << "[" << conn.socket << "] content " << ":" << "\n=-=-=-=-=-=-=-=-=-=-=\n" << std::string(&ibuf[request_size], content_length) << "\n=-=-=-=-=-=-=-=-=-=-=\n";
        }
        
        request.content_length = content_length;
        request.content = request_size;
      }
  
      request.ibuf = &ibuf[0];
      if (not SendResponse(request, conn) or not keep_alive) return;

      ibuf.erase(ibuf.begin(), ibuf.begin() + request_size + content_length);
    }
}

HttpServer::HttpServer()
  : port(0)
  , maxclients(0)
  , mutex()
  , clients()
  , socket()
  , killed( false )
  , running( false )
  , verbose( false )
  , log(&std::cout)
  , warn_log(&std::cout)
  , err_log(&std::cerr)
{
  Init();
}

HttpServer::HttpServer( int _port, int _maxclients)
  : port(_port)
  , maxclients(_maxclients)
  , mutex()
  , clients()
  , socket()
  , killed( false )
  , running( false )
  , verbose( false )
  , log(&std::cout)
  , warn_log(&std::cout)
  , err_log(&std::cerr)
{
  Init();
}

void HttpServer::Init()
{
#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
  // Loads the winsock2 dll
  WORD wVersionRequested = MAKEWORD( 2, 2 );
  WSADATA wsaData;
  if(WSAStartup(wVersionRequested, &wsaData) != 0)
  {
    throw std::runtime_error("WSAStartup failed: Windows sockets not available");
  }
#endif
  pthread_mutex_init( &mutex, 0 );
}

HttpServer::~HttpServer()
{
  pthread_mutex_destroy(&mutex);
  
#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
  //releases the winsock2 resources
  WSACleanup();
#endif
}

void HttpServer::SetTCPPort(int _port)
{
	port = _port;
}

void HttpServer::SetMaxClients(int _maxclients)
{
	maxclients = _maxclients;
}

void HttpServer::SetLog(std::ostream& os)
{
	log = &os;
}

void HttpServer::SetWarnLog(std::ostream& os)
{
	warn_log = &os;
}

void HttpServer::SetErrLog(std::ostream& os)
{
	err_log = &os;
}

void HttpServer::StartLoopThread()
{
  if (this->running)
    {
      (*err_log) << "error: server instance already running\n";
      return;
    }
  
  running = true;
  killed = false;

  struct launcher { static void* routine(void* obj) { static_cast<HttpServer*>(obj)->LoopThread(); return 0; } };
  if (int err = pthread_create(&tid, 0, launcher::routine, static_cast<void*>(this)))
    {
      (*err_log) << "error in thread creation (" << err << ")\n";
      running = false;
    }
}

void HttpServer::Kill()
{
	killed = true;
}

void HttpServer::JoinLoopThread()
{
  if ( !this->running)
    {
      (*err_log) << "error: server instance is not running\n";
      return;
    }
    
  if (int err = pthread_join(tid, 0))
    {
      (*err_log) << "error while joining with thread (" << err << ")\n";
    }
}

bool HttpServer::Killed() const
{
	return killed;
}

bool HttpServer::Verbose() const
{
  return verbose;
}

bool ClientConnection::Send( char const* buf, uintptr_t size ) const
{
  for (intptr_t done; size > 0; size -= done)
    {
      if(http_server.Killed()) return false;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
      done = ::send( socket, buf, size, 0);
#else
      done = ::write( socket, buf, size );
#endif
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
      if((done == 0) || (done == SOCKET_ERROR))
#else
      if(done <= 0)
#endif
      {
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
        if(done == SOCKET_ERROR)
#else
        if(done < 0)
#endif
        {
           done = 0;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
           if(WSAGetLastError() == WSAEWOULDBLOCK)
#else
           if((errno == EAGAIN) || (errno == EWOULDBLOCK))
#endif
           {
             sleep(1); // try again later
             continue;
           }
           return false;
        }
      }
      buf += done;
    }
  return true;
}

void
HttpServer::LoopThread()
{
  struct LoopGuard
  {
    LoopGuard( HttpServer& _hs ) : hs(_hs) {}
    ~LoopGuard() { hs.running = false; hs.killed = false; }
    HttpServer& hs;
  } loopGuard(*this);

  if (this->maxclients == 0)
    {
      (*err_log) << "error: maximum clients not configured\n";
      return;
    }
  
  // create a new socket
  if ((socket = ::socket(AF_INET,SOCK_STREAM,0)) <= 0)
    {
      (*err_log) << "Error creating socket\n";
      return;
    }

  if (this->ALLOW_REUSE_PORT_NUMBER)
    {
      int data = 1;
      int status = setsockopt(socket,SOL_SOCKET,SO_REUSEADDR, (char*)&data,sizeof(data));
      if (status < 0) {
        (*err_log) << "Error configuring reusable socket\n";
        return;
      }
    }
  
  // bind it to the http port
  {
    struct sockaddr_in sinAddress;
    memset( &sinAddress, 0, sizeof(struct sockaddr_in) );
    
    sinAddress.sin_family = AF_INET;
    sinAddress.sin_addr.s_addr = htonl(0); // INADDR_ANY is 0
    sinAddress.sin_port = htons(this->port); // http port
    
    int status = bind(socket,(struct sockaddr*)&sinAddress, sizeof(struct sockaddr_in));
    
    if (status < 0)
      {
        (*err_log) << "Error binding on port " << this->port << "\n";
        return;
      }
  }

  // Set listening socket to non-blocking in order to avoid lockout
  // issue (see Section 15.6 in Unix network programming book)
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
  u_long non_blocking_flag = 1;
  ioctlsocket(socket, FIONBIO, &non_blocking_flag);
#else
  fcntl(socket, F_SETFL, fcntl(socket, F_GETFL, 0) | O_NONBLOCK);
#endif
  
  // listen on the socket for incoming calls
  if (listen(socket, this->maxclients-1))
    {
      (*err_log) << "Error: can't listen on port " << this->port << " with socket " << socket << "\n";
      return;
    }
  (*log) << "Listening on port " << this->port << " with socket " << socket << "\n";

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
  WSAPOLLFD listenpoll;
#else
  struct pollfd listenpoll;
#endif
  listenpoll.fd = socket;
  listenpoll.events = POLLIN;
  
  std::vector<pthread_t> client_thread_ids;
  
  // Entering main processing loop
  for (this->killed = false; not this->killed;)
    {
      //(*log) << "MainLoopIteration\n";
      if (this->clients >= maxclients)
        {
          sleep(1);
          continue;
        }
      
      listenpoll.revents = 0;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	  if (WSAPoll( &listenpoll, 1, 1000) <= 0)
#else
      if (poll( &listenpoll, 1, 1000 ) <= 0)
#endif
        continue;
      
      struct Shuttle {
        static void* routine(void* obj)
        {
          Shuttle shuttle( *static_cast<Shuttle*>(obj) );
          
          /* Client's Connection Data are Saved, release the caller */
          pthread_mutex_unlock( &shuttle.server.mutex );
          (*shuttle.server.log) << "[" << shuttle.socket << "] Client Connection from IP: " << IPAddrRepr(shuttle.ipaddr) << "\n";
          
          /* Finish servicing client */
          ClientConnection conn(shuttle.server, shuttle.socket);
          shuttle.server.Serve(conn);
          
          (*shuttle.server.log) << "[" << shuttle.socket << "] closing connection\n";
          
          pthread_mutex_lock(&shuttle.server.mutex);
          shuttle.server.clients -= 1;
          pthread_mutex_unlock(&shuttle.server.mutex);
          (*shuttle.server.log) << "Connected clients: " << shuttle.server.clients << "\n";
          
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
          closesocket( shuttle.socket );
#else
          close( shuttle.socket );
#endif
          
          return 0;
        }
        Shuttle( HttpServer& _server )
          : server(_server)
        {
          // accept connection socket
          struct sockaddr_in addr;
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
          int addrlen = sizeof (addr);
#else
          socklen_t addrlen = sizeof (addr);
#endif
          socket = accept(server.socket, (struct sockaddr*)&addr, &addrlen);
          if (socket < 0) return;
          ipaddr = addr.sin_addr.s_addr;
#if 0
          /* set short latency */
          int opt_tcp_nodelay = 1;
          setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (char *) &opt_tcp_nodelay, sizeof(opt_tcp_nodelay));
#endif
          // set to non-blocking to allow partial read ()
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
          u_long non_blocking_flag = 1;
          ioctlsocket(socket, FIONBIO, &non_blocking_flag);
#else
          fcntl(socket, F_SETFL, fcntl(socket, F_GETFL, 0) | O_NONBLOCK);
#endif
          (*server.log) << "[" << socket << "] connection accepted\n";
      
          pthread_mutex_lock(&server.mutex);
          server.clients += 1;
          pthread_mutex_unlock(&server.mutex);
      
          (*server.log) << "Connected clients: " << server.clients << "\n";
      
        }
        HttpServer&        server;
        int                socket;
        uint32_t           ipaddr;
      } launcher(*this);
      
      if (launcher.socket < 0)
        {
          /* FIXME: should investigate the reason... */
          (*err_log) << "Error accepting connection\n";
          continue;
        }
      
      /* Launch the service thread */
      pthread_t client_thread_id;
      pthread_mutex_lock(&mutex);
      if (int err = pthread_create(&client_thread_id, 0, Shuttle::routine, static_cast<void*>(&launcher)))
        {
          (*err_log) << "error in thread creation (" << err << ")\n";
          throw 0;
        }
      client_thread_ids.push_back(client_thread_id);
        
      /* Wait for thread full initialization */
      pthread_mutex_lock(&mutex);
      pthread_mutex_unlock(&mutex);
    }
    
    for(std::vector<pthread_t>::const_iterator it = client_thread_ids.begin(); it != client_thread_ids.end(); it++)
    {
      pthread_join(*it, 0);
    }
  
  // Leaving main processing loop
 (*log) << "Cleaning up...\n";
  
  if (this->socket)
  {
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
    closesocket(this->socket);
#else
    close(this->socket);
#endif
  }
  this->socket = 0;
}

} // end of namespace hypapp
} // end of namespace util
} // end of namespace unisim
