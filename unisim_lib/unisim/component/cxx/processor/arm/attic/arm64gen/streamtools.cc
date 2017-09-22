#include <streamtools.hh>
#include <iostream>
#include <iomanip>
#include <map>
#include <typeinfo>

std::ostream& operator << ( std::ostream& sink, Ostreamable const& dobj )
{
  GetOstreamableUsage( typeid(dobj).name() ) += 1;
  dobj( sink );
  return sink;
}

uintptr_t&
GetOstreamableUsage( std::string const& ostreamable )
{
  static std::map<std::string,uintptr_t> ostreamable_stats;
  return ostreamable_stats[ostreamable];
}

void
Ox32::operator() ( std::ostream& sink ) const
{
  std::ios::fmtflags f( sink.flags() );
  sink << "0x" << std::hex << std::setw(8) << std::setfill('0') << num;
  sink.flags( f );
}

void
Binary::operator() ( std::ostream& sink ) const
{
  sink << "0b";
  for (unsigned bit = bitcount; --bit < bitcount;)
    sink << "01"[(num>>bit)&1];
}

void
OStr::operator() ( std::ostream& sink ) const
{
  sink << '"';
  for (auto&& c : str) {
    if (c == '"')
      sink << "\\\"";
    else if (c == '\\')
      sink << "\\\\";
    else if ((c >= ' ') and (c <= '~'))
      sink << c;
    else {
      switch (c) {
      default:
        std::cerr << "Can't encode character (code: " << int( c ) << ").\n";
        throw 0;
      case '\n': sink << "\\n"; break;
      case '\t': sink << "\\t"; break;
      }
    }
  }
  sink << '"';
}

void
rstrip( std::string& s )
{
  uintptr_t pos = s.find_last_not_of( "\f\n\r\t\v " );
  s.erase( pos == std::string::npos ? 0 : pos + 1 );
}

