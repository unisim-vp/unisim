#include <unisim/util/hypapp/hypapp.hh>
#include <iostream>
#include <vector>

#include <cstring>
#include <cstdlib>

#include <arpa/inet.h>

#include <poll.h>
#include <fcntl.h>
#include <unistd.h>

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
    while (*ptr and *ptr == *from)
      ++ptr, ++from;
    return *ptr ? 0 : from;
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
      {
        uintptr_t skip = 0, size = ibuf.size();
        while ((size-skip) >= 2 and ptr[skip] == '\r' and ptr[skip+1] == '\n')
          skip += 2;
        if (skip)
          ibuf.erase( ibuf.begin(), ibuf.begin() + skip );
        ptr = &ibuf[0];
      }

      char const* soh = ptr;
      char const* eoh = GetHeaderEnd(ptr, ibuf.size());
        
      if (not eoh)
        {
          /* Partial header */
          uintptr_t resume = ibuf.size(), end = ibuf.capacity();
          ibuf.resize(end);
          intptr_t bytes = recv(conn.socket, &ibuf[resume], end - resume, 0);
          if (bytes < 0)
            return;
          else if (bytes > 0)
            ibuf.resize(resume + bytes);
          else
            sleep(1);
          continue;
        }
        
      eoh += 2; // first "\r\n" is inside header
      uintptr_t request_size = eoh - ptr;
      ibuf[request_size] = '\0';
      request_size += 2; // second "\r\n" before next part

      NewRequest();
      
      request_type_t request_type;
      if      (char const* p = streat("GET ", ptr))  { ptr = p; request_type = GET; }
      else if (char const* p = streat("HEAD ", ptr)) { ptr = p; request_type = HEAD; }
      else if (char const* p = streat("POST ", ptr)) { ptr = p; request_type = POST; }
      else {
        if (char const* p = strchr(ptr, '\r'))
          ibuf[p-soh] = '\0';
        std::cerr << "unknown method in: " << ptr << std::endl;
        return;
      }
      SetRequestType( request_type );
      
      {
        char const* uri = ptr;
        if (not (ptr = strchr(uri, ' '))) { std::cerr << "[" << conn.socket << "] URI parse error\n"; return; }
        ibuf[ptr++-soh] = '\0';
        std::cout << "[" << conn.socket << "] URI: " << uri << std::endl;
        SetRequestURI( uri );
      }
      
      if (not (ptr = streat("HTTP/1.", ptr))) { std::cerr << "[" << conn.socket << "] HTTP version parse error\n"; return; }
      
      {
        unsigned http_version = 0;
        for (unsigned digit; *ptr != '\r'; ++ptr) {
          if ((digit = (*ptr - '0')) >= 10) { std::cerr << "[" << conn.socket << "] HTTP version parse error\n"; return; }
          http_version = 10*http_version+digit;
        }
        if (http_version != 1)
          { std::cerr << "[" << conn.socket << "] HTTP/1." << http_version <<  " != 1 makes me nervous\n"; return; }
      }
      if (not (ptr = streat("\r\n", ptr))) { std::cerr << "[" << conn.socket << "] HTTP request parse error\n"; return; }
        
      bool keep_alive = false;
      int cseq = 0;
        
      while (ptr < eoh)
        {
          char const* key = ptr;
          if (not (ptr = strchr(ptr, ':'))) { std::cerr << "[" << conn.socket << "] HTTP headers parse error\n"; return; }
          ibuf[ptr++-soh] = '\0';
          while (isblank(*ptr)) ptr += 1;
          char const* val = ptr;
          if (not (ptr = strchr(ptr, '\r'))) { std::cerr << "[" << conn.socket << "] HTTP headers parse error\n"; return; }
          for (char const* p = ptr; (--p > val) and isspace(*p);) ibuf[p-soh] = '\0';
          ibuf[ptr++-soh] = '\0';
          if (*ptr++ != '\n') { std::cerr << "[" << conn.socket << "] HTTP headers parse error\n"; return; }
            
          // header field parsed
            
          if      (streat("Connection", key))
            {
              keep_alive = streat("Keep-Alive", val);
              std::cout << "[" << conn.socket << "] keep alive: " << keep_alive << "\n";
            }
          else if (streat("Content-Type", key))
            {
              if (char const* b = streat("multipart/form-data; boundary=",val))
                { std::cerr << "[" << conn.socket << "] MP boundary:" << b << "\n"; return; }
              else { std::cerr << "[" << conn.socket << "] unknown content type; " << val << "\n"; return; }
            }
          else if (streat("CSeq", key))
            { cseq = strtoul( val, 0, 0 ); std::cout << "[" << conn.socket << "] cseq:" << cseq << "\n"; }
          else if (streat("Host", key) or
                   streat("User-Agent", key) or
                   streat("Referer", key) or
                   streat("Accept", key) or
                   streat("Upgrade-Insecure-Requests",key) or
                   streat("If-Modified-Since", key) or
                   streat("Cache-Control", key))
            { std::cout << "[" << conn.socket << "] " << key << ": " << val << "\n"; }
          else if (streat("Range", key) or
                   streat("Content-Length", key))
            { std::cerr << "[" << conn.socket << "] " << key << ": " << val << "\n"; return; }
          else
            { std::cerr << "[" << conn.socket << "] error: {key:" << key << ", val:" << val << "}\n"; return; }
        }
        
      std::cout << "Header (size: " << request_size << ") received and processed.\n";
      switch (request_type) {
      case GET: std::cout << "Get request." << std::endl; break;
      case HEAD: std::cout << "Head request." << std::endl; return;
      case POST: std::cout << "Post request." << std::endl; return;
      }
        
      if (not SendResponse(conn) or not keep_alive) return;
      
      ibuf.erase(ibuf.begin(), ibuf.begin()+request_size);
    }
}

HttpServer::HttpServer( int _port, int _maxclients)
  : port(_port)
  , maxclients(_maxclients)
  , mutex()
  , clients()
  , socket()
  , killed( false )
  , running( false )
{
  pthread_mutex_init( &mutex, 0 );
}

HttpServer::~HttpServer()
{
  pthread_mutex_destroy(&mutex);
}

void
HttpServer::StartLoopThread()
{
  if (this->running)
    {
      std::cerr << "error: server instance already running\n";
      return;
    }
  
  running = true;
  killed = false;

  struct launcher { static void* routine(void* obj) { static_cast<HttpServer*>(obj)->LoopThread(); return 0; } };
  pthread_t tid;
  if (int err = pthread_create(&tid, 0, launcher::routine, static_cast<void*>(this)))
    {
      std::cerr << "error in thread creation (" << err << ")\n";
      throw 0;
    }
  
  killed = true;
  while (running)
    sleep(1);
}

void
ClientConnection::Send( char const* buf, uintptr_t size ) const
{
  for (uintptr_t done; size > 0; size -= done)
    {
      done = ::write( socket, buf, size );
      if (done > size) {
        // TODO: should investigate the reason and bail out properly
        throw 0;
      }
    }
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
      std::cerr << "error: maximum clients not configured\n";
      return;
    }
  
  // create a new socket
  if ((socket = ::socket(AF_INET,SOCK_STREAM,0)) <= 0)
    {
      std::cerr << "Error creating socket\n";
      return;
    }

  if (this->ALLOW_REUSE_PORT_NUMBER)
    {
      int data = 1;
      int status = setsockopt(socket,SOL_SOCKET,SO_REUSEADDR, (char*)&data,sizeof(data));
      if (status < 0) {
        std::cerr << "Error configuring reusable socket\n";
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
        std::cerr << "Error binding on port " << this->port << "\n";
        return;
      }
  }

  // Set listening socket to non-blocking in order to avoid lockout
  // issue (see Section 15.6 in Unix network programming book)
  fcntl(socket, F_SETFL, fcntl(socket, F_GETFL, 0) | O_NONBLOCK);
  
  // listen on the socket for incoming calls
  if (listen(socket, this->maxclients-1))
    {
      std::cerr << "Error: can't listen on port " << this->port << " with socket " << socket << "\n";
      return;
    }
  std::cout << "Listening on port " << this->port << " with socket " << socket << "\n";

  struct pollfd listenpoll;
  listenpoll.fd = socket;
  listenpoll.events = POLLIN;
  
  // Entering main processing loop
  for (this->killed = false; not this->killed;)
    {
      //std::cout << "MainLoopIteration\n";
      if (this->clients >= maxclients)
        {
          sleep(1);
          continue;
        }
      
      listenpoll.revents = 0;
      if (poll( &listenpoll, 1, 1000 ) <= 0)
        continue;
      
      struct Shuttle {
        static void* routine(void* obj)
        {
          Shuttle shuttle( *static_cast<Shuttle*>(obj) );
          
          /* Client's Connection Data are Saved, release the caller */
          pthread_mutex_unlock( &shuttle.server.mutex );
          std::cout << "[" << shuttle.socket << "] Client Connection from IP: " << IPAddrRepr(shuttle.ipaddr) << "\n";
          
          /* Finish servicing client */
          ClientConnection conn(shuttle.socket);
          shuttle.server.Serve(conn);
          
          std::cout << "[" << shuttle.socket << "] closing connection\n";
          
          pthread_mutex_lock(&shuttle.server.mutex);
          shuttle.server.clients -= 1;
          pthread_mutex_unlock(&shuttle.server.mutex);
          std::cout << "Connected clients: " << shuttle.server.clients << "\n";
          
          close( shuttle.socket );
          
          return 0;
        }
        Shuttle( HttpServer& _server )
          : server(_server)
        {
          // accept connection socket
          struct sockaddr_in addr;
          socklen_t addrlen = sizeof (addr);
          socket = accept(server.socket, (struct sockaddr*)&addr, &addrlen);
          if (socket < 0) return;
          ipaddr = addr.sin_addr.s_addr;
          // set to non-blocking to allow partial read ()
          fcntl(socket, F_SETFL, fcntl(socket, F_GETFL, 0) | O_NONBLOCK);
          std::cout << "[" << socket << "] connection accepted\n";
      
          pthread_mutex_lock(&server.mutex);
          server.clients += 1;
          pthread_mutex_unlock(&server.mutex);
      
          std::cout << "Connected clients: " << server.clients << "\n";
      
        }
        HttpServer&        server;
        int                socket;
        uint32_t           ipaddr;
      } launcher(*this);
      
      if (launcher.socket < 0)
        {
          /* FIXME: should investigate the reason... */
          std::cerr << "Error accepting connection\n";
          continue;
        }
      
      /* Launch the service thread */
      pthread_t client_thread_id;
      pthread_mutex_lock(&mutex);
      if (int err = pthread_create(&client_thread_id, 0, Shuttle::routine, static_cast<void*>(&launcher)))
        {
          std::cerr << "error in thread creation (" << err << ")\n";
          throw 0;
        }

      /* Wait for thread full initialization */
      pthread_mutex_lock(&mutex);
      pthread_mutex_unlock(&mutex);
    }
  
  // Leaving main processing loop
  std::cout << "Cleaning up...\n";
  
  if (this->socket)
    close(this->socket);
  this->socket = 0;
}

