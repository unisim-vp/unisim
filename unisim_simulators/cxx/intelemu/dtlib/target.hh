#ifndef DTLIB_TARGET_HH
#define DTLIB_TARGET_HH

#include <stdint.h>

namespace dtlib {
  struct Target {
    virtual ~Target() {}
    
    virtual void     write( uint32_t _addr, uint8_t const* _buffer, uint32_t _size ) = 0;
    virtual void     write32( uint32_t addr, uint32_t value ) = 0;
    
    virtual void     read( uint8_t* _buffer, uint32_t addr, uint32_t size ) = 0;
    virtual uint32_t read32( uint32_t addr ) = 0;
    
    virtual void     clear( uint32_t _addr, uint32_t _size ) = 0;
    
    virtual void     segregwrite( uint32_t num, uint16_t value ) = 0;
    
    virtual void     settls( uint32_t addr ) = 0;
    virtual void     dynamic_image() = 0;
    
    virtual uint32_t firstfreepage() = 0;

    virtual uint32_t regread32( uint32_t num ) = 0;
    virtual void     regwrite32( uint32_t num, uint32_t value ) = 0;

    virtual void     setesp( uint32_t esp ) = 0;
    virtual void     seteip( uint32_t eip ) = 0;
    virtual uint32_t geteip() = 0;
    
    virtual void     stop() = 0;
  };
};

#endif // DTLIB_TARGET_HH
