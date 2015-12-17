#ifndef INTEL_SEGMENTS_HH
#define INTEL_SEGMENTS_HH

#include <iosfwd>
#include <stdint.h>

/* TODO:
 *  implement real segment descriptor/selector
 */

namespace intel {
  enum segment_t { ES = 0, CS = 1, SS = 2, DS = 3, FS = 4, GS = 5 };
  
  struct SegmentReg
  {
    uint16_t m_value; uint32_t m_base;
    SegmentReg() : m_value( 0 ), m_base( 0 ) {}
    void                      update( uint16_t _id, uint16_t _ti, uint16_t _pl, uint32_t _base )
    {
      m_value = ((_id & 0x1fff) << 3) | ((_ti & 0x1) << 2) | ((_pl & 0x3) << 0);
      m_base = _base;
    }
  };
};

#endif // INTEL_SEGMENTS_HH
