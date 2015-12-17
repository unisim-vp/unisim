#include <dtlib/misc.hh>

#include <ostream>
#include <stdexcept>
#include <cstdarg>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <unistd.h>

namespace dtlib {
  std::ostream&
  osprintf( std::ostream& _out, char const* _fmt, ... )
  {
    va_list ap;
  
    for( intptr_t capacity = 128, size; true; capacity = (size > -1) ? size + 1 : capacity * 2 ) {
      /* allocation */
      char storage[capacity];
      /* Try to print in the allocated space. */
      va_start( ap, _fmt );
      size = vsnprintf( storage, capacity, _fmt, ap );
      va_end( ap );
      /* If it worked, return */
      if( size >= 0 and size < capacity ) {
        _out.write( storage, size );
        break;
      }
    }
    return _out;
  }

  std::string stringf( char const* _fmt, ... ) {
    std::string str;
    va_list ap;

    for( intptr_t capacity = 128, size; true; capacity = (size > -1) ? size + 1 : capacity * 2 ) {
      /* allocation */
      char storage[capacity];
      /* Try to print in the allocated space. */
      va_start( ap, _fmt );
      size = vsnprintf( storage, capacity, _fmt, ap );
      va_end( ap );
      /* If it worked, return */
      if( size >= 0 and size < capacity ) {
        str.append( storage, size );
        break;
      }
    }
    return str;
  }
}
