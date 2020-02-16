#include <utils/trace.hh>
#include <iostream>

namespace SSv8 {
  Trace::Sink_t::~Sink_t() {
    delete m_stream;
  }
  
  Trace::Sink_t&
  Trace::Sink_t::init( std::streambuf* _sb ) {
    if( not m_stream )
      m_stream = new std::ostream( _sb );
    else
      m_stream->rdbuf( _sb );
    
    return *this;
  }
  
  Trace::Channels_t Trace::s_chans;
  
  std::ostream&
  Trace::defaultchan( char const* _name ) {
    return *s_chans[_name].init( std::cerr.rdbuf() ).m_stream;
  }
  
};
