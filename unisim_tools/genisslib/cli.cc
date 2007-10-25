#include <cli.hh>
#include <cassert>
#include <cstdarg>
#include <strtools.hh>
#include <conststr.hh>
#include <iostream>

using namespace std;

CLI::CLI()
  : m_callname( 0 ), m_opt_tabstop( 0 ), m_optc( 0 )
{}

struct InfoArgs_t : public CLI::Args_t {
  CLI&                   m_cli;

  InfoArgs_t( CLI& _cli ) : m_cli( _cli ) {}
  
  bool match( CLI::Appearances_t _appearances, ... ) {
    // Gathering information about optionnal and mandatory arguments
    m_cli.m_optc++;
    if( _appearances == CLI::Once ) return false;
    va_list args;
    va_start( args, _appearances );
    intptr_t value = 0;
    for( char const* optname; (optname = va_arg( args, char const* )); ) { value += 2 + strlen( optname ); }
    value += 1 + strlen( va_arg( args, char const* ) ); // synopsis
    va_end( args );
    if( value > m_cli.m_opt_tabstop ) m_cli.m_opt_tabstop = value;
    return false;
  }
};

void
CLI::set( char const* _displayname, char const* _callname ) {
  m_displayname = _displayname;
  m_callname = _callname;
  InfoArgs_t args( *this );
  parse( args );
}

struct Proto_t : public CLI::Args_t {
  bool match( CLI::Appearances_t _appearances, ... ) {
    if( _appearances != CLI::Once ) return false;
    
    va_list args;
    va_start( args, _appearances );
    for( char const* optname; (optname = va_arg( args, char const* )); ) {}
    char const* opt_synopsis = va_arg( args, char const* );
    va_end( args );
    
    cerr << ' ' << opt_synopsis;
    return false;
  }
};

void
CLI::prototype() {
  cerr << "Usage: " << m_callname;
  Proto_t proto;
  parse( proto );
  cerr << endl;
}

struct Screen_t {
  intptr_t  m_tabstop, m_width;
  Str::Buf  m_buffer;
  intptr_t  m_lastblank;
  bool      m_lastwasblank;
  
  Screen_t( intptr_t _tabstop, intptr_t _width )
    : m_tabstop( _tabstop ), m_width( _width ), m_lastblank( -1 ), m_lastwasblank( true ) {}
  
  Screen_t& tab() {
    if( m_buffer.m_index >= m_tabstop) return write( '\n' );
    while( m_buffer.m_index < m_tabstop ) m_buffer.write( " " );
    return *this;
  }
  
  Screen_t& reset() {
    m_buffer.clear();
    m_lastblank = -1;
    m_lastwasblank = true;
    return *this;
  }
  
  Screen_t& flush() {
    for( char const* ptr = m_buffer.m_storage; *ptr; ++ptr )
      if( *ptr > ' ' ) return write( '\n' );
    return *this;
  }
  
  Screen_t& write( char ch ) {
    intptr_t index = m_buffer.m_index;
    m_buffer.write( 1, &ch );
    if( ch == '\n' ) {
      cerr << m_buffer.m_storage;
      reset();
      return tab();
    } else if( ch <= ' ' ) { // considered as blank
      if( not m_lastwasblank ) { m_lastblank = index; m_lastwasblank = true; }
    } else {
      m_lastwasblank = false;
    }
    
    if( m_buffer.m_index > m_width and m_lastblank > 0 and not m_lastwasblank ) {
      char flushbuf[m_buffer.m_index + 1];
      memcpy( flushbuf, m_buffer.m_storage, m_lastblank );
      flushbuf[m_lastblank] = '\0';
      cerr << flushbuf << endl;
      char const* remainder = m_buffer.m_storage + m_lastblank;
      while( *remainder != '\0' and *remainder <= ' ' ) remainder++;
      strcpy( flushbuf, remainder );
      reset();
      tab();
      write( flushbuf );
    }
    return *this;
  }
  
  Screen_t& write( char const* _str ) { while( *_str ) write( *_str++ ); return *this; }
  
};
  
struct Opts_t : public CLI::Args_t {
  intptr_t m_align;
  
  Opts_t( intptr_t _align ) : m_align( _align + 2 ) {}
  
  bool match( CLI::Appearances_t _appearances, ... ) {
    if( _appearances == CLI::Once ) return false;
    Screen_t screen( m_align, 120 );
    
    va_list args;
    va_start( args, _appearances );
    char const* sep = "  ";
    for( char const* optname; (optname = va_arg( args, char const* )); ) {
      screen.write( sep ).write( optname );
      sep = ", ";
    }
    char const* opt_synopsis = va_arg( args, char const* );
    char const* opt_description = va_arg( args, char const* );
    va_end( args );
    
    screen.write( " " ).write( opt_synopsis ).tab().write( opt_description );
    screen.flush();
    return false;
  }
};

void
CLI::options() {
  cerr << "options:\n";
  Opts_t opts( m_opt_tabstop );
  parse( opts );
}

void
CLI::help() {
  prototype();
  description();
  options();
}

struct ValueArgs_t : public CLI::Args_t {
  intptr_t  m_argidx, m_argc;
  char**    m_argvals;
  intptr_t  m_optidx, m_optc;
  intptr_t* m_optoccs;
  bool      m_hasmatch;
  
  ValueArgs_t( intptr_t _argc, char** _argvals,
               intptr_t _optc, intptr_t* _optoccs )
    : m_argidx( 0 ), m_argc( _argc ), m_argvals( _argvals ),
      m_optidx( 0 ), m_optc( _optc ), m_optoccs( _optoccs ),
      m_hasmatch( false )
  {}
  
  bool match( CLI::Appearances_t _appearances, ... ) {
    assert( m_optidx < m_optc );
    intptr_t&   optocc = m_optoccs[m_optidx++];
    assert( m_argidx < m_argc );
    char const* argval = m_argvals[m_argidx];
    
    va_list args;
    va_start( args, _appearances );
    bool nomatch = true;
    bool alternatives = false;
    for( char const* optname; (optname = va_arg( args, char const* )); )
      { nomatch = nomatch and (strcmp( optname, argval ) != 0); alternatives = true; }
    nomatch = nomatch and alternatives;
    va_end( args );
    
    if( nomatch ) return false;
    
    switch( _appearances ) {
    case CLI::AtMostOnce:
      if( optocc >= 1 )
        throw Str::fmt( "error: `%s' appears more than once on command line.\n", argval );
      break;
    case CLI::Once:
      if( optocc >= 1 ) return false;
      break;
    default: break;
    };
    optocc += 1;
    if( alternatives ) m_argidx += 1;
    m_hasmatch = true;
    return true;
  }
  
  char const* pop() { return (m_argidx < m_argc) ? m_argvals[m_argidx++] : 0; }
  bool        next() { m_optidx = 0; m_hasmatch = false; return m_argidx < m_argc; }
};

void
CLI::process( intptr_t _argc, char** _argv ) {
  for( intptr_t idx = _argc; (--idx) >= 0; ) {
    if( strcmp( _argv[idx], "--help" ) == 0 or  strcmp( _argv[idx], "-h" ) == 0 ) {
      help();
      throw Exit_t( 0 );
    }
  }

  intptr_t optoccs[m_optc];
  for( intptr_t idx = m_optc; (--idx) >= 0; ) optoccs[idx] = 0;
  
  try {
    for( ValueArgs_t args( _argc, _argv, m_optc, optoccs ); args.next(); ) {
      parse( args );
      if( args.m_hasmatch ) continue;
      cerr << m_displayname << ": unexpected argument: " << args.pop() << "\n";
      throw Exit_t( 1 );
    }
  } catch( ConstStr_t _error ) {
    cerr << m_displayname << ": " << _error.str();
    help();
    throw Exit_t( 1 );
  }
}

char const* CLI::Args_t::pop() { assert( false ); }

