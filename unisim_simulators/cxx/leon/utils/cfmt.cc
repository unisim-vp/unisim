#include <utils/cfmt.hh>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <cassert>

using namespace std;

namespace SSv8 {
  CFmt::CFmt( const char * _fmt, ... )
    : m_storage( 0 )
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
        m_storage = new char[size + 1];
        strcpy( m_storage, storage );
        return;
      }
    }
    assert( false );
  }
  
  CFmt::~CFmt() { delete [] m_storage; }
};

