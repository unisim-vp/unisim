#ifndef SSV8_HW_PERIPHERAL_HH
#define SSV8_HW_PERIPHERAL_HH

#include <inttypes.h>

namespace SSv8 {
  struct Peripheral {
    enum Exception_t { RangeError };
    uint32_t                    m_faddr;
    uint32_t                    m_laddr;
    
    Peripheral( uint32_t _faddr, uint32_t _laddr )
      : m_faddr( _faddr ), m_laddr( _laddr ) { if( _laddr < _faddr ) throw RangeError; }
    
    virtual ~Peripheral() {};
    virtual bool                read( uint32_t _addr, uint32_t _size, uint8_t* _value ) = 0;
    virtual bool                write( uint32_t _addr, uint32_t _size, uint8_t const* _value ) = 0;
    
    static void                 bytes_copy( uint8_t* _dst, uint8_t const* _src, uintptr_t _size ) {
      if( _size == 1 )      *_dst = *_src;
      else if( _size == 2 ) *((uint16_t*)( _dst )) = *((uint16_t const*)( _src ));
      else if( _size == 4 ) *((uint32_t*)( _dst )) = *((uint32_t const*)( _src ));
      else if( _size == 8 ) *((uint64_t*)( _dst )) = *((uint64_t const*)( _src ));
      else { for( uint32_t idx = 0; idx < _size; ++ idx ) _dst[idx] = _src[idx]; }
    }
  };
};

#endif // SSV8_HW_PERIPHERAL_HH
