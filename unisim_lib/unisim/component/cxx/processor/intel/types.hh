#ifndef INTEL_TYPES_HH
#define INTEL_TYPES_HH

namespace intel {
  typedef uint8_t     u8_t;
  typedef uint16_t    u16_t;
  typedef uint32_t    u32_t;
  typedef uint64_t    u64_t;
  typedef int8_t      s8_t;
  typedef int16_t     s16_t;
  typedef int32_t     s32_t;
  typedef int64_t     s64_t;
  typedef bool        bit_t;
  typedef float       f32_t;
  typedef double      f64_t;
  typedef long double f80_t;
  typedef bool     mkbool;
  typedef int      mkint;

  template <typename T> struct tpinfo {};
  template <> struct tpinfo< u8_t> { typedef  s8_t stype; typedef  u8_t utype; typedef u16_t twice; enum { is_signed = 0, bitsize =  8 }; };
  template <> struct tpinfo< s8_t> { typedef  s8_t stype; typedef  u8_t utype; typedef s16_t twice; enum { is_signed = 1, bitsize =  8 }; };
  template <> struct tpinfo<u16_t> { typedef s16_t stype; typedef u16_t utype; typedef u32_t twice; enum { is_signed = 0, bitsize = 16 }; };
  template <> struct tpinfo<s16_t> { typedef s16_t stype; typedef u16_t utype; typedef s32_t twice; enum { is_signed = 1, bitsize = 16 }; };
  template <> struct tpinfo<u32_t> { typedef s32_t stype; typedef u32_t utype; typedef u64_t twice; enum { is_signed = 0, bitsize = 32 }; };
  template <> struct tpinfo<s32_t> { typedef s32_t stype; typedef u32_t utype; typedef s64_t twice; enum { is_signed = 1, bitsize = 32 }; };
  template <> struct tpinfo<u64_t> { typedef s64_t stype; typedef u64_t utype;                      enum { is_signed = 0, bitsize = 64 }; };
  template <> struct tpinfo<s64_t> { typedef s64_t stype; typedef u64_t utype;                      enum { is_signed = 1, bitsize = 64 }; };

  template <unsigned OPSIZE> struct TypeFor {};
  template <> struct TypeFor<8> { typedef int8_t s; typedef uint8_t u; };
  template <> struct TypeFor<16> { typedef int16_t s; typedef uint16_t u; };
  template <> struct TypeFor<32> { typedef int32_t s; typedef uint32_t u; typedef float f; };
  template <> struct TypeFor<64> { typedef int64_t s; typedef uint64_t u; typedef double f; };
  template <> struct TypeFor<80> { typedef long double f; };

  template <unsigned SIZE> struct CTypeFor {};
  template <> struct CTypeFor<8> { typedef int8_t s; typedef uint8_t u; };
  template <> struct CTypeFor<16> { typedef int16_t s; typedef uint16_t u; };
  template <> struct CTypeFor<32> { typedef int32_t s; typedef uint32_t u; typedef float f; };
  template <> struct CTypeFor<64> { typedef int64_t s; typedef uint64_t u; typedef double f; };
  template <> struct CTypeFor<80> { typedef double f; };
  
};

#endif // INTEL_TYPES_HH
