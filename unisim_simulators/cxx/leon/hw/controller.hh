#ifndef SSV8_HW_CONTROLLER_HH
#define SSV8_HW_CONTROLLER_HH

#include <inttypes.h>
#include <unisim/component/cxx/processor/sparc/isa_sv8.hh>

namespace SSv8 {
  
  template <class t_Arch_t>
  struct Controller : public SSv8::Decoder<t_Arch_t> {
    t_Arch_t                    m_arch;
    
    bool                        m_disasm;
    Operation<t_Arch_t>*        m_lastoperation;
    uint32_t                    m_lastpc;
    //uint8_t const*              m_laststorage;
    
    Controller();
    void                        Fetch( void *_buffer, uint32_t _addr, uint32_t _size );
    void                        step();

    void                        dumptrap( std::ostream& _sink );
  };

} // end of namespace SSv8

#endif // SSV8_HW_CONTROLLER_HH
