#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_HOSTFLOAT_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_HOSTFLOAT_HH__

#include <ieee754.h>
#include <cmath>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace hostfloat {
  
  struct FPSCR
  {
    uint32_t m_value;
    
    FPSCR() : m_value( 0x03000000 ) {}
    
    template <typename RF>
    uint32_t Get( RF const& rf ) const { return rf.Get( m_value ); }
    template <typename RF>
    void     Set( RF const& rf, uint32_t value ) { return rf.Set( m_value, value ); }
  };
  
  template <typename operT, typename fpscrT>
  void
  FloatFlushToZero( operT& op, fpscrT& fpscr )
  {}
  
  template <typename operT, typename fpscrT>
  void FloatAdd( operT& res, operT op1, operT op2, fpscrT& fpscr ) { res = op1 + op2; }
  
  template <typename operT, typename fpscrT>
  void FloatSub( operT& res, operT op1, operT op2, fpscrT& fpscr ) { res = op1 - op2; }
  
  template <typename operT, typename fpscrT>
  void FloatDiv( operT& res, operT op1, operT op2, fpscrT& fpscr ) { res = op1 / op2; }
  
  template <typename operT, typename fpscrT>
  void FloatMul( operT& res, operT op1, operT op2, fpscrT& fpscr ) { res = op1 * op2; }
  
  template <typename operT, typename fpscrT>
  void FloatMulAdd( operT& acc, operT op1, operT op2, fpscrT& fpscr ) { acc = acc + (op1 * op2); }
  
  template <typename operT, typename fpscrT>
  void FloatNeg( operT& res, operT op, fpscrT& fpscr ) { res = -op; }
  
  template <typename fpT, typename intT, typename fpscrT>
  void FloatItoF( fpT& res, intT op, int fracbits, fpscrT& fpscr ) { res = fpT( op ) / fpT(1 << fracbits); }
  
  template <typename intT, typename fpT, typename fpscrT>
  void FloatFtoI( intT& res, fpT op, int fracbits, fpscrT& fpscr ) { res = intT( op * fpT(1 << fracbits) ); }
  
  template <typename ofpT, typename ifpT, typename fpscrT>
  void FloatFtoF( ofpT& res, ifpT op, fpscrT& fpscr ) { res = ofpT( op ); }
  
  template <typename fpscrT>
  void FloatAbs( double& res, double op, fpscrT& fpscr ) { res = fabs( op ); }
  
  template <typename fpscrT>
  void FloatAbs( float& res, float op, fpscrT& fpscr ) { res = fabsf( op ); }
  
  template <typename operT, typename fpscrT>
  bool FloatIsSNaN( operT op, fpscrT const& fpscr ) { return false; }
  
  template <typename operT, typename fpscrT>
  bool FloatIsQNaN( operT op, fpscrT const& fpscr ) { return false; }
  
  template <typename operT, typename fpscrT>
  void FloatSetQuietBit( operT& op, fpscrT const& fpscr ) {}
  
  template <typename fpscrT>
  void FloatSetDefaultNan( double& result, fpscrT const& fpscr )
  { result = (double(0) / double(0)); }
  
  template <typename fpscrT>
  void FloatSetDefaultNan( float& result, fpscrT const& fpscr )
  { result = (float(0) / float(0)); }
  
  template <typename operT, typename fpscrT>
  void FloatSqrt( operT& res, operT const& op, fpscrT& fpscr ) { res = sqrt( op ); }
  
  template <typename operT, typename fpscrT>
  int FloatCompare( operT op1, operT op2, fpscrT& fpscr )
  { return (op1 == op2) ? 0 : (op1 > op2) ? +1 : -1; }

} // end of namespace hostfloat
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif /* __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_HOSTFLOAT_HH__ */
