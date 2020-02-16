#ifndef SSV8_UTILS_PFXCHAN_HPP
#define SSV8_UTILS_PFXCHAN_HPP

#include <iosfwd>
#include <streambuf>

namespace SSv8 {
  
  /*
   *  PFXChan Pre Formated Lines Channel
   */
  
  struct PFXChan : public std::streambuf {
    typedef std::streambuf::int_type int_type;
    // output info
    char const*                 m_prefix;
    std::ostream&               m_sink;
    // line buffer
    intptr_t                    m_capacity, m_size;
    char*                       m_storage;

    // constructor
    PFXChan( char const* _prefix, std::ostream& _sink );
    // destructor
    ~PFXChan();
    
  protected:
    // write one character
    virtual int_type            overflow( int_type _ch );
//     // write multiple characters
//     virtual std::streamsize     xsputn( const char* _s, std::streamsize _num );
  };

} // end of namespace SSv8

#endif // SSV8_UTILS_PFXCHAN_HPP

