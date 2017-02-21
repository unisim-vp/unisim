#include <dtlib/sink.hh>
#include <fstream>
#include <iostream>

namespace dtlib {
  Sink::Sink()
    : m_stream( &(Sink::nullstream()) )
  {}
  
  void           Sink::redirect( std::ostream& _stream ) { m_stream = &_stream; }
  
  void           Sink::close() { m_stream = &Sink::nullstream(); }
  
  bool           Sink::is_null() const { return (m_stream == &Sink::nullstream()); }
  
  std::ostream&  Sink::nullstream()
  {
    static std::ostream res( 0 );
    return res;
  }
};
