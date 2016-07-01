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
  
  struct FP
  {
    typedef double F64;
    typedef float  F32;
    
    template <typename operT, typename fpscrT> static
    bool
    FlushToZero( operT& op, fpscrT& fpscr )
    {
      int fptype = std::fpclassify(op);
      if ((fptype != FP_SUBNORMAL) or (fptype == FP_ZERO)) return false;
      op = 0.0;
      return true;
    }
    
    template <typename operT, typename fpscrT> static
    void Add( operT& acc, operT op2, fpscrT& fpscr ) { acc += op2; }
  
    template <typename operT, typename fpscrT> static
    void Sub( operT& acc, operT op2, fpscrT& fpscr ) { acc -= op2; }
  
    template <typename operT, typename fpscrT> static
    void Div( operT& acc, operT op2, fpscrT& fpscr ) { acc /= op2; }
    
    template <typename operT, typename fpscrT> static
    void Mul( operT& acc, operT op2, fpscrT& fpscr ) { acc *= op2; }
  
    template <typename SOFTDBL, typename fpscrT> static
    bool IsInvalidMulAdd( SOFTDBL& acc, SOFTDBL const& op1, SOFTDBL const& op2, fpscrT& fpscr ) { return false; }
    
    template <typename operT, typename fpscrT> static
    void MulAdd( operT& acc, operT op1, operT op2, fpscrT& fpscr ) { acc += (op1 * op2); }
  
    template <typename operT, typename fpscrT> static
    void Neg( operT& acc, fpscrT& fpscr ) { acc = -acc; }
  
    template <typename fpT, typename intT, typename fpscrT> static
    void ItoF( fpT& dst, intT src, int fracbits, fpscrT& fpscr ) { dst = fpT( src ) / fpT(1 << fracbits); }
  
    template <typename intT, typename fpT, typename fpscrT> static
    void FtoI( intT& dst, fpT src, int fracbits, fpscrT& fpscr ) { dst = intT( src * fpT(1 << fracbits) ); }
  
    template <typename ofpT, typename ifpT, typename fpscrT> static
    void FtoF( ofpT& dst, ifpT src, fpscrT& fpscr ) { dst = ofpT( src ); }
  
    template <typename fpscrT> static
    void Abs( double& acc, fpscrT& fpscr ) { acc = fabs( acc ); }
  
    template <typename fpscrT> static
    void Abs( float& acc, fpscrT& fpscr ) { acc = fabsf( acc ); }
  
    template <typename operT, typename fpscrT> static
    bool IsSNaN( operT op, fpscrT const& fpscr ) { return std::isnan( op ) and issignaling( op ); }
  
    template <typename operT, typename fpscrT> static
    bool IsQNaN( operT op, fpscrT const& fpscr ) { return std::isnan( op ) and not issignaling( op ); }
  
    template <typename fpscrT> static
    void SetQuietBit( double& op, fpscrT const& fpscr )
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
    void SetQuietBit( float& op, fpscrT const& fpscr )
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
    void SetDefaultNan( double& result, fpscrT const& fpscr )
    { result = (double(0) / double(0)); }
  
    template <typename fpscrT> static
    void SetDefaultNan( float& result, fpscrT const& fpscr )
    { result = (float(0) / float(0)); }
  
    template <typename operT, typename fpscrT> static
    void Sqrt( operT& acc, fpscrT& fpscr ) { acc = sqrt( acc ); }
  
    template <typename operT, typename fpscrT> static
    int Compare( operT op1, operT op2, fpscrT& fpscr )
    { return (op1 == op2) ? 0 : (op1 > op2) ? +1 : -1; }
  };
  
} // end of namespace hostfloat
} // end of namespace arm
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif /* __UNISIM_COMPONENT_CXX_PROCESSOR_ARM_HOSTFLOAT_HH__ */
