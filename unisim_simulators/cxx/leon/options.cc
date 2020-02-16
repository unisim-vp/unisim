#include <options.hh>
#include <iostream>
#include <cstdlib>
#include <cstring>

using namespace std;

namespace Star {
  Option Options::table[Option::optionscount] = {
    {"startdisasm",  0, "instruction address where to start disassembly."},
    {"exitpoint",    0, "instruction address where to stop simulation."},
    {"maxinsts",     0, "instruction count before simulation stops."},
  };

  Option::operator uint32_t() const { return std::strtoul( m_value, 0, 0 ); }
  Option::operator  int64_t() const { return strtoll( m_value, 0, 0 ); }
  Option::operator bool() const {
    if( not m_value ) return false;
    if( not strcasecmp( m_value, "yes" ) or
        not strcasecmp( m_value, "true" ) or
        atol( m_value ) > 0 ) return true;
    return false;
  }
  
  int
  Options::parse( int _argc, char** argv ) {
    for( int aidx = 0; aidx < _argc; ) {
      char const* optname = argv[aidx++];
      if( optname[0] != '-' or optname[1] != '-' ) return aidx - 1;
      if( optname[2] == '\0' ) return aidx;
      if( not strcmp( optname, "--help" ) ) throw Options::Help;
      if( aidx >= _argc ) return aidx - 1;
      Option& opt = byname( optname + 2 );
      opt.m_value = argv[aidx++];
    }
    return 0;
  }
  
  Option&
  Options::byname( char const* _name ) {
    for( int idx = 0; idx < Option::optionscount; ++idx )
      if( not strcmp( _name, table[idx].m_name ) ) return table[idx];
    cerr << "No such option: " << _name << endl;
    throw Options::Error;
  }

  std::ostream&
  Options::dump( std::ostream& _sink ) {
    int ralign = 0;
    for( int idx = 0; idx < Option::optionscount; ++idx ) {
      int count = strlen( table[idx].m_name ) + 6;
      if( count > ralign ) ralign = count;
    }
    for( int idx = 0; idx < Option::optionscount; ++idx ) {
      int count = 0;
      for( char const* ptr = "  --";            *ptr; ptr++, count++ ) _sink << *ptr;
      for( char const* ptr = table[idx].m_name; *ptr; ptr++, count++ ) _sink << *ptr;
      for( char const* ptr = "  ";              *ptr; ptr++, count++ ) _sink << *ptr;
      for( ; count < ralign; count++ ) _sink << ' ';
      _sink << table[idx].m_description << endl;
    }
    
    return _sink;
  }
};
