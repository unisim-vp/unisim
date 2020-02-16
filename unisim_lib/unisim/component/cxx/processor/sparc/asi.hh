#ifndef SSV8_HW_ASI_HH
#define SSV8_HW_ASI_HH

namespace SSv8 {
  // AddressSpaceIdentifier
  enum ASI_t {
    user_instruction       = 0x8,
    supervisor_instruction = 0x9,
    user_data              = 0xa,
    supervisor_data        = 0xb
  };
};

#endif // SSV8_HW_ASI_HH
