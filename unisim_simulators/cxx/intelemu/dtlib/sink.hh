#ifndef DTLIB_SINK_HH
#define DTLIB_SINK_HH

#include <iosfwd>
#include <stdint.h>

namespace dtlib {
  struct Sink {
    std::ostream*             m_stream;
    
    Sink();
    
    void                      redirect( std::ostream& _stream );
    void                      close();
    bool                      is_null() const;
    
    std::ostream&             operator() () { return *m_stream; }
    
    static std::ostream&      nullstream();
  };
};

#endif // DTLIB_SINK_HH
