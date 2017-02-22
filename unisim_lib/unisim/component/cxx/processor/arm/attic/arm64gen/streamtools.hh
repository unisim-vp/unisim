#ifndef __STREAMTOOLS_HH__
#define __STREAMTOOLS_HH__

#include <string>
#include <inttypes.h>
#include <iosfwd>

struct Ostreamable
{
  virtual void operator() ( std::ostream& sink ) const = 0;
  virtual ~Ostreamable() {};
};

uintptr_t& GetOstreamableUsage( std::string const& ostreamable );

std::ostream& operator << ( std::ostream& sink, Ostreamable const& dobj );

struct Ox32 : public Ostreamable
{
  Ox32( uint32_t _num ) : num(_num) {} uint32_t num;
  void operator() ( std::ostream& sink ) const;
};

struct Binary : public Ostreamable
{
  Binary( uint32_t _num, unsigned _bitcount ) : num(_num), bitcount(_bitcount) {} uint32_t num; unsigned bitcount;
  void operator() ( std::ostream& sink ) const;
};

struct OStr : public Ostreamable
{
  OStr( std::string _str ) : str(_str) {} std::string str;
  void operator() ( std::ostream& sink ) const;
};

void rstrip( std::string& s );

template <uintptr_t OLDSZ, uintptr_t NEWSZ>
bool
query_replace( std::string& text, char const (&olds)[OLDSZ], char const (&news)[NEWSZ] )
{
  uintptr_t pos = text.find( olds, 0, OLDSZ-1 );
  if (pos == std::string::npos)
    return false;
  text.replace( pos, OLDSZ-1, news, NEWSZ-1 );
  return true;
}

#endif // __STREAMTOOLS_HH__
