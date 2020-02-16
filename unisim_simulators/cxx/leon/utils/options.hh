#ifndef SSV8_OPTIONS_HPP
#define SSV8_OPTIONS_HPP

#include <inttypes.h>
#include <iosfwd>

namespace SSv8 {
  struct Option {
    char const*                 m_name;
    char const*                 m_value;
    char const*                 m_desc;
  private:
    Option*                     m_next;
    
  public:
    enum Exception_t { Help = 0, Error };
    
    Option( char const* _name, char const* _value, char const* _desc )
      : m_name( _name ), m_value( _value ), m_desc( _desc ), m_next( s_optlist )
    { s_optlist = this; }
    
    char const*                 str() const { return m_value; }
    uint32_t                    ui32( uint32_t _default ) const;
    int64_t                     si64( int64_t _default ) const;
    bool                        yesno( bool _default ) const;
    
  private:
    static Option*              s_optlist;

  public:
    static int                  parse( int _args, char** argv );
    static std::ostream&        dump( std::ostream& _sink );
  };
};

#endif // STAR_OPTIONS_HPP
