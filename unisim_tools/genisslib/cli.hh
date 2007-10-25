#ifndef CLI_HH
#define CLI_HH

#include <inttypes.h>

struct CLI {
  char const*            m_displayname;
  char const*            m_callname;
  intptr_t               m_opt_tabstop;
  intptr_t               m_optc;
  
  enum Appearances_t { AtMostOnce, Unlimited, Once };
  
  struct Exit_t { int m_value; Exit_t( int _value ) : m_value( _value ) {} };
  
  struct Args_t {
    virtual ~Args_t() {};
    virtual bool         match( Appearances_t _appearances, ... ) = 0;
    virtual char const*  pop();
  };
  
  CLI();
  virtual ~CLI() {}
  
  void                   set( char const* _displayname, char const* _callname );
  
  void                   process( intptr_t _argc, char** _argv );

  virtual void           help();
  virtual void           prototype();
  virtual void           options();

  virtual void           parse( Args_t& _args ) = 0;
  virtual void           description() = 0;
};

#endif // CLI_HH
