#include <utils/pfxchan.hh>
#include <ostream>
#include <cstdio>

namespace SSv8 {
  // Constructor
  PFXChan::PFXChan( char const* _prefix, std::ostream& _sink )
    : m_prefix( _prefix ), m_sink( _sink ), m_capacity( 0 ), m_size( 0 ), m_storage( 0 )
  {}
  
  // destructor
  PFXChan::~PFXChan() {
    delete [] m_storage;
  }
  
  // write one character
  PFXChan::int_type
  PFXChan::overflow( PFXChan::int_type _ch ) {
    if( _ch == EOF or _ch == 0 ) return _ch;
    
    if( (m_size+1) >= m_capacity ) {
      m_capacity = std::max<intptr_t>( 32, m_capacity*2 );
      char* nstorage = new char [m_capacity];
      for( intptr_t idx = m_size; (--idx) >= 0; ) nstorage[idx] = m_storage[idx];
      delete [] m_storage;
      m_storage = nstorage;
    }
    
    m_storage[m_size++] = _ch;
    if( _ch == '\n' ) {
      m_storage[m_size] = '\0';
      m_sink << m_prefix << m_storage << "\x1b[0m";
      m_size = 0;
    }
    return _ch;
  }

} // end of namespace SSv8
