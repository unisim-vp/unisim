#ifndef SSV8_UTILS_TRACE_HPP
#define SSV8_UTILS_TRACE_HPP

#include <inttypes.h>
#include <map>
#include <iosfwd>

namespace SSv8 {
  struct Trace {
    struct Sink_t {
      std::ostream*             m_stream;
      Sink_t() : m_stream( 0 ) {}
      ~Sink_t();
      Sink_t&                   init( std::streambuf* _sb );
    };
    
    struct Less {
      bool operator()( char const* _s1, char const* _s2 ) const
      { while( *_s1 != '\0' and *_s1 == *_s2 ) ++_s1, ++_s2; return *_s1 < *_s2; }
    };
    
    typedef std::map<char const*, Sink_t, Less> Channels_t;
    static Channels_t           s_chans;
    static std::ostream&        initchan( char const* _name, std::streambuf* _sb = 0 )
    { return *s_chans[_name].init( _sb ).m_stream; }
    static std::ostream&        chan( char const* _name )
    { std::ostream* stream = s_chans[_name].m_stream; return stream ? *stream : defaultchan( _name ); }
    static std::ostream&        defaultchan( char const* _name );
    
    enum Exception_t { KeyError };
  };
  
};

#endif // SSV8_UTILS_TRACE_HPP
