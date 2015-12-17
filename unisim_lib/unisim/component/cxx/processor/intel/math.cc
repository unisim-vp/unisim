#include <unisim/component/cxx/processor/intel/math.hh>
#include <cmath>
#include <fenv.h>

namespace intel
{
  double
  x87frnd( double value, x87frnd_mode_t mode )
  {
    int parent_mode = fegetround(), status;
    switch (parent_mode) {
    case FE_TONEAREST: case FE_UPWARD: case FE_DOWNWARD: case FE_TOWARDZERO: break;
    default: throw x87frnd_error;
    }
    switch (mode) {
    case x87frnd_nearest: status = fesetround( FE_TONEAREST ); break;
    case x87frnd_down:    status = fesetround( FE_DOWNWARD ); break;
    case x87frnd_up:      status = fesetround( FE_UPWARD ); break;
    case x87frnd_toward0: status = fesetround( FE_TOWARDZERO ); break;
    default: throw x87frnd_error;
    }
    double result = nearbyint( value );
    status = fesetround( parent_mode );
    if (status != 0) throw x87frnd_error;
    return result;
  }

}; // end of namespace intel
