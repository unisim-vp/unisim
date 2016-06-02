#ifndef DTLIB_MISC_HH
#define DTLIB_MISC_HH

#include <iosfwd>
#include <string>
#include <stdint.h>

namespace dtlib {
  std::ostream& osprintf( std::ostream& _out, char const* _fmt, ... );
  std::string stringf( char const* _fmt, ... );

  template <class tInt>
  char*
  intexport( char* buf, tInt value )
  {
    for (uintptr_t idx = 0; idx < sizeof (tInt); ++idx) {
      *buf++ = value;
      value >>= 8;
    }
    return buf;
  }
  
  template <class tInt>
  char const*
  intimport( char const* buf, tInt& value )
  {
    tInt tmp = 0;
    for (uintptr_t idx = 0; idx < sizeof (tInt); ++idx) {
      tmp |= (tInt( static_cast<unsigned char>( buf[idx] ) ) << (idx*8));
    }
    value = tmp;
    return &buf[sizeof (tInt)];
  }

  template <class T>
  struct NoDefault {
    operator T() const { if (not m_valid) throw Error(); return m_value; }
    NoDefault() : m_valid( false ) {};
    NoDefault( T const& value ) : m_value( value ), m_valid( true ) {}
    T const& value() const { if (not m_valid) throw Error(); return m_value; }
    T&       value() { if (not m_valid) throw Error(); return m_value; }
    bool     valid() const { return m_valid; }

    struct Error {};
  private:
    T        m_value;
    bool     m_valid;
  };

};

#endif /* DTLIB_MISC_HH */
