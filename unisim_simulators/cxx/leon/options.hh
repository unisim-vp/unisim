#ifndef STAR_OPTIONS_HPP
#define STAR_OPTIONS_HPP

#include <inttypes.h>

#include <iosfwd>

namespace Star {
  struct Option {
    char const*                 m_name;
    char const*                 m_value;
    char const*                 m_description;
    
    operator                    uint32_t() const;
    operator                     int64_t() const;
    bool                        defined() const { return m_value; }
    operator                    bool() const;
    operator                    char const*() {  return m_value; }
    enum {
      startdisasm = 0,
      exitpoint,
      maxinsts,
      optionscount
    };
      
  };
  
  struct Options {
    enum Exception_t { Error, Help };
    static Option               table[Option::optionscount];
    static Option&              byname( char const* _name );
    
    static int                  parse( int _argc, char** argv );
    static std::ostream&        dump( std::ostream& _sink );
  };
};

#endif // STAR_OPTIONS_HPP
