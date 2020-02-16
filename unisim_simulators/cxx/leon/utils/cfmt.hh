#ifndef SSV8_UTILS_CFMT_HH
#define SSV8_UTILS_CFMT_HH

#include <inttypes.h>

namespace SSv8 {
  class CFmt {
  private:
    char*               m_storage;
  
  public:
    CFmt( const char* _fmt, ... );
    ~CFmt();
  
    operator            char const*() const { return m_storage; }
  };
};

#endif // SSV8_UTILS_CFMT_HH
