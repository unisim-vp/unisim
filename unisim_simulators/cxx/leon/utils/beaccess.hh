#ifndef SSV8_BEACCES_HH
#define SSV8_BEACCES_HH

namespace SSv8 {
  template <class INT_t>
  INT_t be_read( uint8_t const* _buf ) {
    INT_t res = 0;
    for( uintptr_t idx = 0; idx < sizeof( INT_t ); idx += 1 )
      res = (res << 8) | _buf[idx];
    return res;
  }
  
  template <class INT_t>
  void be_write( uint8_t* _buf, INT_t _value ) {
    for( intptr_t idx = sizeof( INT_t ); (--idx) >= 0; ) {
      _buf[idx] = _value % 0x100;
      _value /= 0x100;
    }
  }
}

#endif // SSV8_BEACCES_HH
