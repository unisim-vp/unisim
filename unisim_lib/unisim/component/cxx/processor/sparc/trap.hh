#ifndef SSV8_HW_TRAP_HPP
#define SSV8_HW_TRAP_HPP

#include <inttypes.h>

namespace SSv8 {
  struct Trap_t {
    char const* m_name;
    uint8_t     m_priority;
    uint8_t     m_traptype;
    
    Trap_t&     operator=( Trap_t const& _trap ) { m_name = _trap.m_name; m_priority = _trap.m_priority; m_traptype = _trap.m_traptype; return *this; }
    void        clear() { m_name = 0; m_priority = 0xff; m_traptype = 0; }
    operator    bool() const { return bool( m_name ); }
    
    enum TrapType_t {
      data_store_error = 0,
      instruction_access_MMU_miss,
      instruction_access_error,
      r_register_access_error,
      instruction_access_exception,
      privileged_instruction,
      illegal_instruction,
      fp_disabled,
      cp_disabled,
      unimplemented_FLUSH,
      watchpoint_detected,
      window_overflow,
      window_underflow,
      mem_address_not_aligned,
      fp_exception,
      cp_exception,
      data_access_error,
      data_access_MMU_miss,
      data_access_exception,
      tag_overflow,
      division_by_zero,
      trap_instruction,
      interrupt_level_15,
      interrupt_level_14,
      interrupt_level_13,
      interrupt_level_12,
      interrupt_level_11,
      interrupt_level_10,
      interrupt_level_9,
      interrupt_level_8,
      interrupt_level_7,
      interrupt_level_6,
      interrupt_level_5,
      interrupt_level_4,
      interrupt_level_3,
      interrupt_level_2,
      interrupt_level_1,
      trapcount
    };
    
    static Trap_t s_hardware[];
  };
}

#endif // SSV8_HW_TRAP_HPP
