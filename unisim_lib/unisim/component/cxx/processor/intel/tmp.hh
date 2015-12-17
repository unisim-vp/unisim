#ifndef INTEL_TMP_HH
#define INTEL_TMP_HH

namespace intel
{
  template <unsigned _Value>
  struct SB { enum { begin = SB<((_Value-1) & ~_Value)>::end, end = (1 + SB<(_Value >> 1)>::end) }; };
  template <> struct SB<0u>{ enum { end = 0 }; };
};

#endif // INTEL_TMP_HH
