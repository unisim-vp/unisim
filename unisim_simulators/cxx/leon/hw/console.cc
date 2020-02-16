#include <hw/console.hh>
#include <iostream>
#include <cstring>

namespace SSv8 {
  Console::Console()
    : Peripheral( 0x80000100, 0x8000010f )
  {}
  
  bool
  Console::read( uint32_t _addr, uint32_t _size, uint8_t* _value ) {
    if( (_addr & 0xfffffff0) != 0x80000100 or _size != 4 ) return false;
    uint32_t reg = (_addr / 4) % 4;
    if( reg == 1 ) {
      static const uint8_t status_ready[4] = {0,0,0,5};
      bytes_copy( _value, status_ready, 4 );
      return true;
    }
    // Not implemented
    return false;
  }

  bool
  Console::write( uint32_t _addr, uint32_t _size, uint8_t const* _value ) {
    if( (_addr & 0xfffffff0) != 0x80000100 or _size != 4 ) return false;
    uint32_t reg = (_addr / 4) % 4;
    if( reg == 0 ) {
      std::cout << char( _value[3] );
      return true;
    }
    // Not implemented
    return true;
  };
};
