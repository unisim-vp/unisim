#ifndef INTEL_X87_HH
#define INTEL_X87_HH

namespace intel
{
  enum x87frnd_mode_t { x87frnd_error = -1, x87frnd_nearest = 0, x87frnd_down = 1, x87frnd_up = 2, x87frnd_toward0 = 3 };

  double x87frnd( double value, x87frnd_mode_t mode );
}; // end of namespace intel

#endif // INTEL_X87_HH
    
