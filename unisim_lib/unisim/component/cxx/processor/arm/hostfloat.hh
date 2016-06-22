#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_HOSTFLOAT_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_HOSTFLOAT_HH__

#if !defined(WIN32) && !defined(_WIN32) && !defined(WIN64) && !defined(_WIN64)
#include <ieee754.h>
#endif

#include <cmath>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
inline int issignaling(double d)
{
	uint64_t i;
	memcpy(&i, &d, 8);
	return ((i & ((1ULL << 51) - 1)) != 0) && (i & (1ULL << 51) == 0);
}

inline int issignaling(float f)
{
	uint32_t i;
	memcpy(&i, &f, 4);
	return ((i & ((1UL << 22) - 1)) != 0) && (i & (1ULL << 22) == 0);
}
#endif

#include <unisim/component/cxx/processor/arm/register_field.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace arm {
namespace hostfloat {
  
  struct FPU
  {
    template <typename operT, typename fpscrT> static
    bool
    FloatFlushToZero( operT& op, fpscrT& fpscr )
    {
      int fptype = std::fpclassify(op);
      if ((fptype != FP_SUBNORMAL) or (fptype == FP_ZERO)) return false;
      op = 0.0;
      return true;
    }
    
    template <typename operT, typename fpscrT> static
    void FloatAdd( operT& res, operT op1, operT op2, fpscrT& fpscr ) { res = op1 + op2; }
  
    template <typename operT, typename fpscrT> static
    void FloatSub( operT& res, operT op1, operT op2, fpscrT& fpscr ) { res = op1 - op2; }
  
    template <typename operT, typename fpscrT> static
    void FloatDiv( operT& res, operT op1, operT op2, fpscrT& fpscr ) { res = op1 / op2; }
    
    template <typename operT, typename fpscrT> static
    void FloatMul( operT& res, operT op1, operT op2, fpscrT& fpscr ) { res = op1 * op2; }
  
    template <typename operT, typename fpscrT> static
    void FloatMulAdd( operT& acc, operT op1, operT op2, fpscrT& fpscr ) { acc = acc + (op1 * op2); }
  
    template <typename operT, typename fpscrT> static
    void FloatNeg( operT& res, operT op, fpscrT& fpscr ) { res = -op; }
  
    template <typename fpT, typename intT, typename fpscrT> static
    void FloatItoF( fpT& res, intT op, int fracbits, fpscrT& fpscr ) { res = fpT( op ) / fpT(1 << fracbits); }
  
    template <typename intT, typename fpT, typename fpscrT> static
    void FloatFtoI( intT& res, fpT op, int fracbits, fpscrT& fpscr ) { res = intT( op * fpT(1 << fracbits) ); }
  
    template <typename ofpT, typename ifpT, typename fpscrT> static
    void FloatFtoF( ofpT& res, ifpT op, fpscrT& fpscr ) { res = ofpT( op ); }
  
    template <typename fpscrT> static
    void FloatAbs( double& res, double op, fpscrT& fpscr ) { res = fabs( op ); }
  
    template <typename fpscrT> static
    void FloatAbs( float& res, float op, fpscrT& fpscr ) { res = fabsf( op ); }
  
    template <typename operT, typename fpscrT> static
    bool FloatIsSNaN( operT op, fpscrT const& fpscr ) { return std::isnan( op ) and issignaling( op ); }
  
    template <typename operT, typename fpscrT> static
    bool FloatIsQNaN( operT op, fpscrT const& fpscr ) { return std::isnan( op ) and not issignaling( op ); }
  
    template <typename fpscrT> static
    void FloatSetQuietBit( double& op, fpscrT const& fpscr )
    {
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
      uint64_t d;
      memcpy(&d, &op, 8);
      d = d | (1ULL << 51);
      memcpy(&op, &d, 8);
#else
      ieee754_double ud;
      ud.d = op;
      ud.ieee_nan.quiet_nan = 1;
      op = ud.d;
#endif
    }
  
    template <typename fpscrT> static
    void FloatSetQuietBit( float& op, fpscrT const& fpscr )
    {
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
      uint32_t f;
      memcpy(&f, &op, 4);
      f = f | (1UL << 22);
      memcpy(&op, &f, 4);
#else
      ieee754_float uf;
      uf.f = op;
      uf.ieee_nan.quiet_nan = 1;
      op = uf.f;
#endif
    }
  
    template <typename fpscrT> static
    void FloatSetDefaultNan( double& result, fpscrT const& fpscr )
    { result = (double(0) / double(0)); }
  
    template <typename fpscrT> static
    void FloatSetDefaultNan( float& result, fpscrT const& fpscr )
    { result = (float(0) / float(0)); }
  
    template <typename operT, typename fpscrT> static
    void FloatSqrt( operT& res, operT const& op, fpscrT& fpscr ) { res = sqrt( op ); }
  
    template <typename operT, typename fpscrT> static
    int FloatCompare( operT op1, operT op2, fpscrT& fpscr )
    { return (op1 == op2) ? 0 : (op1 > op2) ? +1 : -1; }
  };
  
} // end of namespace hostfloat
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif /* __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_HOSTFLOAT_HH__ */
