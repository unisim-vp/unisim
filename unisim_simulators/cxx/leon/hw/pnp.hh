#ifndef SSV8_HW_PNP_HH
#define SSV8_HW_PNP_HH

#include <inttypes.h>
#include <hw/peripheral.hh>

#include <fwd.hh>
#include <iosfwd>

namespace SSv8 {
  struct PNP : public SSv8::Peripheral {
    /* Plug 'n Play Controller */
    uint8_t*                    m_word;
    
    PNP( uint32_t _faddr, uint32_t _laddr ) : SSv8::Peripheral( _faddr, _laddr ) {};
    virtual ~PNP() {};
      

    bool                        read( uint32_t _addr, uint32_t _size, uint8_t* _value ) {
      m_word = _value;
      if( _size > 4 or not getreg( (_addr - m_faddr) / 4 ) ) return false;
      return true;
    }
    
    bool                        write( uint32_t _addr, uint32_t _size, uint8_t const* _value ) { /* Read only */ return false; }
    void                        setreg( uint32_t _value );
    virtual bool                getreg( uint32_t _idx ) = 0;
  };

  struct AHBPNP : public PNP {
    AHBPNP() : PNP( 0xfffff000, 0xffffffff ) {}
    
    bool                        getreg( uint32_t _idx );
  };
  
  struct APBPNP : public PNP {
    APBPNP() : PNP( 0x800ff000, 0x800fffff ) {}
    
    bool                        getreg( uint32_t _idx );
  };
};

#endif // SSV8_HW_PNP_HH
