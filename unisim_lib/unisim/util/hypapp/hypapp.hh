#ifndef __HYPAPP_HH__
#define __HYPAPP_HH__

#include <string>
#include <inttypes.h>

struct ClientConnection
{
  ClientConnection(int _socket) : socket(_socket) {}
  
  template <uintptr_t SZ>
  void Send(char const (&text)[SZ]) const { Send( &text[0], SZ-1 ); }
  void Send( std::string const& text ) const { Send( &text[0], text.size() ); }
  void Send(char const* buf, uintptr_t size) const;
  
  int                socket;
};

struct MessageLoop
{
  virtual ~MessageLoop() {}
  
  void Run(ClientConnection const& conn);
  
  enum request_type_t { GET, HEAD, POST };
  
  virtual void NewRequest() {}
  virtual void SetRequestType( request_type_t request_type ) {}
  virtual void SetRequestURI( char const* uri ) {}
  virtual bool SendResponse( ClientConnection const& conn ) = 0;
};

struct HttpServer
{
  HttpServer( int _port, int _maxclients );
  ~HttpServer();

  void StartLoopThread();

  // Content exchange callbacks
  virtual void Serve(ClientConnection const& conn) const = 0;

protected:
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

private:
  void  LoopThread();
};


#endif // __HYPAPPSERV_HH__
