#ifndef SSV8_HW_CONSOLE_HH
#define SSV8_HW_CONSOLE_HH

#include <inttypes.h>
#include <hw/peripheral.hh>

namespace SSv8 {
  struct Console : public Peripheral {
    /* uart = { data:32, status:32, control:32, scaler:32 } */
    
    Console();
    
    bool                        read( uint32_t _addr, uint32_t _size, uint8_t* _value );
    bool                        write( uint32_t _addr, uint32_t _size, uint8_t const* _value );
  };
};

#endif // SSV8_HW_CONSOLE_HH
