/*
 *  Copyright (c) 2023,
 *  Commissariat a l'Energie Atomique (CEA)
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, this
 *     list of conditions and the following disclaimer.
 *
 *   - Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   - Neither the name of CEA nor the names of its contributors may be used to
 *     endorse or promote products derived from this software without specific prior
 *     written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED.
 *  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Gilles Mouchard (gilles.mouchard@cea.fr)
 */

#ifndef __UNISIM_UTIL_FLOATING_POINT_SOFTFLOAT_EMU_SOFTFLOAT_EMU_HH__
#define __UNISIM_UTIL_FLOATING_POINT_SOFTFLOAT_EMU_SOFTFLOAT_EMU_HH__

#if HAVE_CONFIG_H
#include "config.h"
#endif

#if HAVE_SOFTFLOAT_EMU

#include <unisim/util/floating_point/floating_point.hh>
#include <softfloat_emu/softfloat_emu.h>
#include <cstring>
#include <iostream>

// This C++ header provides support for the following emulated floating-point types:
//   * unisim::util::floating_point::softfloat_emu::x86::Half             (C++ type for IEEE 754 half-precision floating-point format emulating behavior of x86 architecture)
//   * unisim::util::floating_point::softfloat_emu::x86::Single           (C++ type for IEEE 754 single-precision floating-point format emulating behavior of x86 architecture)
//   * unisim::util::floating_point::softfloat_emu::x86::Double           (C++ type for IEEE 754 double-precision floating-point format emulating behavior of x86 architecture)
//   * unisim::util::floating_point::softfloat_emu::x86_sse::Half         (C++ type for IEEE 754 half-precision floating-point format emulating behavior of x86 SSE architecture)
//   * unisim::util::floating_point::softfloat_emu::x86_sse::Single       (C++ type for IEEE 754 single-precision floating-point format emulating behavior of x86 SSE architecture)
//   * unisim::util::floating_point::softfloat_emu::x86_sse::Double       (C++ type for IEEE 754 double-precision floating-point format emulating behavior of x86 SSE architecture)
//   * unisim::util::floating_point::softfloat_emu::arm_vfpv2::Half       (C++ type for IEEE 754 half-precision floating-point format emulating behavior of ARM vfpv2 architecture)
//   * unisim::util::floating_point::softfloat_emu::arm_vfpv2::Single     (C++ type for IEEE 754 single-precision floating-point format emulating behavior of ARM vfpv2 architecture)
//   * unisim::util::floating_point::softfloat_emu::arm_vfpv2::Double     (C++ type for IEEE 754 double-precision floating-point format emulating behavior of ARM vfpv2 architecture)
//   * unisim::util::floating_point::softfloat_emu::arm_vfpv2_dn::Half    (C++ type for IEEE 754 half-precision floating-point format emulating behavior of ARM vfpv2 architecture with use of default NaNs while NaN propagation)
//   * unisim::util::floating_point::softfloat_emu::arm_vfpv2_dn::Single  (C++ type for IEEE 754 single-precision floating-point format emulating behavior of ARM vfpv2 architecture with use of default NaNs while NaN propagation)
//   * unisim::util::floating_point::softfloat_emu::arm_vfpv2_dn::Double  (C++ type for IEEE 754 double-precision floating-point format emulating behavior of ARM vfpv2 architecture with use of default NaNs while NaN propagation)
//   * unisim::util::floating_point::softfloat_emu::riscv::Half           (C++ type for IEEE 754 half-precision floating-point format emulating behavior of architecture RISCV)
//   * unisim::util::floating_point::softfloat_emu::riscv::Single         (C++ type for IEEE 754 single-precision floating-point format emulating behavior of architecture RISCV)
//   * unisim::util::floating_point::softfloat_emu::riscv::Double         (C++ type for IEEE 754 double-precision floating-point format emulating behavior of architecture RISCV)
//   * unisim::util::floating_point::softfloat_emu::arm_vfpv2_ddn::Half   (C++ type for IEEE 754 half-precision floating-point format emulating behavior of ARM vfpv2 architecture with programmable use of default NaNs while NaN propagation)
//   * unisim::util::floating_point::softfloat_emu::arm_vfpv2_ddn::Single (C++ type for IEEE 754 single-precision floating-point format emulating behavior of ARM vfpv2 architecture with programmable use of default NaNs while NaN propagation)
//   * unisim::util::floating_point::softfloat_emu::arm_vfpv2_ddn::Double (C++ type for IEEE 754 double-precision floating-point format emulating behavior of ARM vfpv2 architecture with programmable use of default NaNs while NaN propagation)

//==============================================================================
//                            Forward declarations
//==============================================================================

namespace unisim {
namespace util {
namespace floating_point {

// Description of float16_t type
template <> struct FloatingPointTypeInfo<float16_t>
{
	typedef uint16_t packed_type;
	typedef float16_t float_type;
	static const packed_type sign_mask  = 0x8000;
	static const packed_type  exp_mask  = 0x7c00;
	static const unsigned      exp_pos  = 10;
	static const unsigned     exp_bias  = 15;
	static const packed_type quiet_mask = 0x0200;
	static const packed_type frac_mask  = 0x03ff;
	static __UNISIM_FP_CONSTEXPR float_type FromPacked(packed_type packed) { float_type v = { packed }; return v; }
	static __UNISIM_FP_CONSTEXPR packed_type ToPacked(float_type v) { return v.v; }
};

// Description of float32_t type
template <> struct FloatingPointTypeInfo<float32_t>
{
	typedef uint32_t packed_type;
	typedef float32_t float_type;
	static const packed_type sign_mask  = 0x80000000;
	static const packed_type  exp_mask  = 0x7f800000;
	static const unsigned      exp_pos  = 23;
	static const unsigned     exp_bias  = 127;
	static const packed_type quiet_mask = 0x00400000;
	static const packed_type frac_mask  = 0x007fffff;
	static __UNISIM_FP_CONSTEXPR float_type FromPacked(packed_type packed) { float_type v = { packed }; return v; }
	static __UNISIM_FP_CONSTEXPR packed_type ToPacked(float_type v) { return v.v; }
};

// Description of float64_t type
template <> struct FloatingPointTypeInfo<float64_t>
{
	typedef uint64_t packed_type;
	typedef float64_t float_type;
	static const packed_type sign_mask  = 0x8000000000000000ULL;
	static const packed_type  exp_mask  = 0x7ff0000000000000ULL;
	static const unsigned      exp_pos  = 52;
	static const unsigned     exp_bias  = 1023;
	static const packed_type quiet_mask = 0x0008000000000000ULL;
	static const packed_type frac_mask  = 0x000fffffffffffffULL;
	static __UNISIM_FP_CONSTEXPR float_type FromPacked(packed_type packed) { float_type v = { packed }; return v; }
	static __UNISIM_FP_CONSTEXPR packed_type ToPacked(float_type v) { return v.v; }
};

namespace softfloat_emu {

// Generic Floating-point type (C++ wrapper class for float16_t, float32_t, and float64_t types).
template <typename IF, typename T> struct Float;

// Generic helper functions dependent from FloatingPointTypeInfo<> trait availability
template <typename PACKED, typename FLOAT> PACKED _ToPacked( FLOAT a );
template <typename FLOAT> void _ToUnpacked( UnpackedFloat& unpacked, FLOAT a );
template <typename FLOAT> bool _IsNaN( FLOAT a );
template <typename FLOAT> bool _IsDenormal( FLOAT a );
template <typename FLOAT> bool _IsZero( FLOAT a );
template <typename FLOAT> bool _IsNeg( FLOAT a );
template <typename FLOAT> bool _IsInf( FLOAT a );
template <typename INTEGER, typename FLOAT> INTEGER _ToInt( FLOAT a, uint_fast8_t rmode, bool exact );
template <typename INTEGER, typename FLOAT> INTEGER _ToIntRoundMinMag( FLOAT a, bool exact );
template <typename FLOAT> FLOAT _RoundToInt( FLOAT a, uint_fast8_t rmode, bool exact );
template <typename FLOAT> FLOAT _FMulAdd( FLOAT a, FLOAT b, FLOAT c);
template <typename FLOAT> FLOAT _FRem( FLOAT a, FLOAT b );
template <typename FLOAT> FLOAT _FSqrt( FLOAT a );
template <typename FLOAT> FLOAT _FAbs( FLOAT a );
template <typename FLOAT> FLOAT _FNeg( FLOAT a );
template <typename FLOAT> FLOAT _FMin( FLOAT a, FLOAT b );
template <typename FLOAT> FLOAT _FMinNumber( FLOAT a, FLOAT b );
template <typename FLOAT> FLOAT _FMax( FLOAT a, FLOAT b );
template <typename FLOAT> FLOAT _FMaxNumber( FLOAT a, FLOAT b );
template <typename FLOAT> bool _IsSignaling( FLOAT a );
template <typename FLOAT> FLOAT _DefaultNaN();
template <typename FLOAT> FLOAT _ClearSignaling( FLOAT a );
template <typename FLOAT> FLOAT _Zeroes( FLOAT a );
template <typename FLOAT> FLOAT _DefaultNaN();
template <typename STREAM, typename FLOAT> void _Print( STREAM& stream, FLOAT const& value );
template <typename STREAM, typename FLOAT> void _Scan( STREAM& stream, FLOAT& value );

} // end of namespace softfloat_emu
} // end of namespace floating_point
} // end of namespace util
} // end of namespace unisim

//==============================================================================
//                              Definitions
//==============================================================================

namespace unisim {
namespace util {
namespace floating_point {
namespace softfloat_emu {

// Control/Status representation conversion functions

static inline uint_fast8_t ToDetectTininess( uint_fast8_t dt )
{
	switch( dt )
	{
		case softfloat_emu_tininess_beforeRounding: return tininess_beforeRounding;
		case softfloat_emu_tininess_afterRounding : return tininess_afterRounding;
	}
	struct Bad {};
	throw Bad();
	return tininess_beforeRounding;
}

static inline uint_fast8_t FromDetectTininess( uint_fast8_t dt )
{
	switch( dt )
	{
		case tininess_beforeRounding: return softfloat_emu_tininess_beforeRounding;
		case tininess_afterRounding : return softfloat_emu_tininess_afterRounding;
	}
	struct Bad {};
	throw Bad();
	return softfloat_emu_tininess_beforeRounding;
}

static inline uint_fast8_t ToRoundingMode( uint_fast8_t rmode )
{
	switch( rmode )
	{
		case softfloat_emu_round_near_even  : return round_near_even;
		case softfloat_emu_round_minMag     : return round_minMag;
		case softfloat_emu_round_min        : return round_min;
		case softfloat_emu_round_max        : return round_max;
		case softfloat_emu_round_near_maxMag: return round_near_maxMag;
		case softfloat_emu_round_odd        : return round_odd;
	}
	struct Bad {};
	throw Bad();
	return softfloat_emu_round_near_even;
}

static inline uint_fast8_t FromRoundingMode( uint_fast8_t rmode )
{
	switch( rmode )
	{
		case round_near_even  : return softfloat_emu_round_near_even;
		case round_minMag     : return softfloat_emu_round_minMag;
		case round_min        : return softfloat_emu_round_min;
		case round_max        : return softfloat_emu_round_max;
		case round_near_maxMag: return softfloat_emu_round_near_maxMag;
		case round_odd        : return softfloat_emu_round_odd;
	}
	
	struct Bad {};
	throw Bad();
	return round_near_even;
}

static inline uint_fast8_t FromExceptionFlags( uint_fast8_t eflags )
{
	return ( ( ( eflags & flag_inexact   ) != 0 ) ? softfloat_emu_flag_inexact   : 0)
		   | ( ( ( eflags & flag_underflow ) != 0 ) ? softfloat_emu_flag_underflow : 0)
		   | ( ( ( eflags & flag_overflow  ) != 0 ) ? softfloat_emu_flag_overflow  : 0)
		   | ( ( ( eflags & flag_infinite  ) != 0 ) ? softfloat_emu_flag_infinite  : 0)
		   | ( ( ( eflags & flag_invalid   ) != 0 ) ? softfloat_emu_flag_invalid   : 0);
}

static inline uint_fast8_t ToExceptionFlags( uint_fast8_t eflags )
{
	return ( ( ( eflags & softfloat_emu_flag_inexact   ) != 0 ) ? flag_inexact   : 0)
		   | ( ( ( eflags & softfloat_emu_flag_underflow ) != 0 ) ? flag_underflow : 0)
		   | ( ( ( eflags & softfloat_emu_flag_overflow  ) != 0 ) ? flag_overflow  : 0)
		   | ( ( ( eflags & softfloat_emu_flag_infinite  ) != 0 ) ? flag_infinite  : 0)
		   | ( ( ( eflags & softfloat_emu_flag_invalid   ) != 0 ) ? flag_invalid   : 0);
}

static inline uint_fast8_t ToFlushToZero( uint_fast8_t ftz )
{
	switch( ftz )
	{
		case softfloat_emu_ftz_never         : return ftz_never;
		case softfloat_emu_ftz_beforeRounding: return ftz_beforeRounding;
		case softfloat_emu_ftz_afterRounding : return ftz_afterRounding;
	}
	struct Bad {};
	throw Bad();
	return ftz_never;
}

static inline uint_fast8_t FromFlushToZero( uint_fast8_t ftz )
{
	switch( ftz )
	{
		case ftz_never         : return softfloat_emu_ftz_never;
		case ftz_beforeRounding: return softfloat_emu_ftz_beforeRounding;
		case ftz_afterRounding : return softfloat_emu_ftz_afterRounding;
	}
	struct Bad {};
	throw Bad();
	return softfloat_emu_ftz_never;
}

// Interfaces with softfloat-emu library functions

#undef SOFTFLOAT_EMU_DEF_ARCH_IF_FUNCTIONS
#define SOFTFLOAT_EMU_DEF_ARCH_IF_FUNCTIONS( ARCH_NAME )                                                                                                                                            \
	/*----------------------------------------------------------------------------                                                                                                                    \
	| Routine to raise any or all of the software floating-point exception flags.                                                                                                                     \
	*----------------------------------------------------------------------------*/                                                                                                                   \
	static void raiseFlags( uint_fast8_t eflags ) { softfloat_emu_ ## ARCH_NAME ## _raiseFlags( FromExceptionFlags( eflags ) ); }                                                                     \
	                                                                                                                                                                                                  \
	/*----------------------------------------------------------------------------                                                                                                                    \
	| Integer-to-floating-point conversion routines.                                                                                                                                                  \
	*----------------------------------------------------------------------------*/                                                                                                                   \
	static float16_t ui32_to_f16( uint32_t a) { return ARCH_NAME ## _ui32_to_f16( a ); }                                                                                                              \
	static float32_t ui32_to_f32( uint32_t a ) { return ARCH_NAME ## _ui32_to_f32( a ); }                                                                                                             \
	static float64_t ui32_to_f64( uint32_t a ) { return ARCH_NAME ## _ui32_to_f64( a ); }                                                                                                             \
	static void ui32_to_extF80M( uint32_t a, extFloat80_t *z ) { ARCH_NAME ## _ui32_to_extF80M( a, z ); }                                                                                             \
	static void ui32_to_f128M( uint32_t a, float128_t *z ) { ARCH_NAME ## _ui32_to_f128M( a, z ); }                                                                                                   \
	static float16_t ui64_to_f16( uint64_t a ) { return ARCH_NAME ## _ui64_to_f16( a ); }                                                                                                             \
	static float32_t ui64_to_f32( uint64_t a ) { return ARCH_NAME ## _ui64_to_f32( a ); }                                                                                                             \
	static float64_t ui64_to_f64( uint64_t a ) { return ARCH_NAME ## _ui64_to_f64( a ); }                                                                                                             \
	static void ui64_to_extF80M( uint64_t a, extFloat80_t *z ) { ARCH_NAME ## _ui64_to_extF80M( a, z ); }                                                                                             \
	static void ui64_to_f128M( uint64_t a, float128_t *z ) { ARCH_NAME ## _ui64_to_f128M( a, z ); }                                                                                                   \
	static float16_t i32_to_f16( int32_t a ) { return ARCH_NAME ## _i32_to_f16( a ); }                                                                                                                \
	static float32_t i32_to_f32( int32_t a ) { return ARCH_NAME ## _i32_to_f32( a ); }                                                                                                                \
	static float64_t i32_to_f64( int32_t a ) { return ARCH_NAME ## _i32_to_f64( a ); }                                                                                                                \
	static void i32_to_extF80M( int32_t a, extFloat80_t *z ) { return ARCH_NAME ## _i32_to_extF80M( a, z ); }                                                                                         \
	static void i32_to_f128M( int32_t a, float128_t *z ) { ARCH_NAME ## _i32_to_f128M( a, z ); }                                                                                                      \
	static float16_t i64_to_f16( int64_t a ) { return ARCH_NAME ## _i64_to_f16( a ); }                                                                                                                \
	static float32_t i64_to_f32( int64_t a) { return ARCH_NAME ## _i64_to_f32( a ); }                                                                                                                 \
	static float64_t i64_to_f64( int64_t a ) { return ARCH_NAME ## _i64_to_f64( a ); }                                                                                                                \
	static void i64_to_extF80M( int64_t a, extFloat80_t *z ) { ARCH_NAME ## _i64_to_extF80M( a, z ); }                                                                                                \
	static void i64_to_f128M( int64_t a, float128_t *z ) { ARCH_NAME ## _i64_to_f128M( a, z ); }                                                                                                      \
                                                                                                                                                                                                    \
	/*----------------------------------------------------------------------------                                                                                                                    \
	| 16-bit (half-precision) floating-point operations.                                                                                                                                              \
	*----------------------------------------------------------------------------*/                                                                                                                   \
	static uint_fast32_t f16_to_ui32( float16_t a, uint_fast8_t rmode, bool exact ) { return ARCH_NAME ## _f16_to_ui32( a, FromRoundingMode( rmode ), exact ); }                                      \
	static uint_fast64_t f16_to_ui64( float16_t a, uint_fast8_t rmode, bool exact ) { return ARCH_NAME ## _f16_to_ui64( a, FromRoundingMode( rmode ), exact ); }                                      \
	static int_fast32_t f16_to_i32( float16_t a, uint_fast8_t rmode, bool exact ) { return ARCH_NAME ## _f16_to_i32( a, FromRoundingMode( rmode ), exact ); }                                         \
	static int_fast64_t f16_to_i64( float16_t a, uint_fast8_t rmode, bool exact ) { return ARCH_NAME ## _f16_to_i64( a, FromRoundingMode( rmode ), exact ); }                                         \
	static uint_fast32_t f16_to_ui32_r_minMag( float16_t a, bool exact ) { return ARCH_NAME ## _f16_to_ui32_r_minMag( a, exact ); }                                                                   \
	static uint_fast64_t f16_to_ui64_r_minMag( float16_t a, bool exact ) { return ARCH_NAME ## _f16_to_ui64_r_minMag( a, exact ); }                                                                   \
	static int_fast32_t f16_to_i32_r_minMag( float16_t a, bool exact ) { return ARCH_NAME ## _f16_to_i32_r_minMag( a, exact ); }                                                                      \
	static int_fast64_t f16_to_i64_r_minMag( float16_t a, bool exact) { return ARCH_NAME ## _f16_to_i64_r_minMag( a, exact ); }                                                                       \
	static float32_t f16_to_f32( float16_t a ) { return ARCH_NAME ## _f16_to_f32( a ); }                                                                                                              \
	static float64_t f16_to_f64( float16_t a ) { return ARCH_NAME ## _f16_to_f64( a ); }                                                                                                              \
	static void f16_to_extF80M( float16_t a, extFloat80_t *z ) { ARCH_NAME ## _f16_to_extF80M( a, z ); }                                                                                              \
	static void f16_to_f128M( float16_t a, float128_t *z ) { ARCH_NAME ## _f16_to_f128M( a, z ); }                                                                                                    \
	static float16_t f16_roundToInt( float16_t a, uint_fast8_t rmode, bool exact) { return ARCH_NAME ## _f16_roundToInt( a, FromRoundingMode( rmode ), exact ); }                                     \
	static float16_t f16_add( float16_t a, float16_t b ) { return ARCH_NAME ## _f16_add( a, b ); }                                                                                                    \
	static float16_t f16_sub( float16_t a, float16_t b ) { return ARCH_NAME ## _f16_sub( a, b ); }                                                                                                    \
	static float16_t f16_mul( float16_t a, float16_t b ) { return ARCH_NAME ## _f16_mul( a, b ); }                                                                                                    \
	static float16_t f16_mulAdd( float16_t a, float16_t b, float16_t c) { return ARCH_NAME ## _f16_mulAdd(a, b, c); }                                                                                 \
	static float16_t f16_div( float16_t a, float16_t b ) { return ARCH_NAME ## _f16_div( a, b ); }                                                                                                    \
	static float16_t f16_rem( float16_t a, float16_t b ) { return ARCH_NAME ## _f16_rem( a, b ); }                                                                                                    \
	static float16_t f16_sqrt( float16_t a ) { return ARCH_NAME ## _f16_sqrt( a ); }                                                                                                                  \
	static bool f16_eq( float16_t a, float16_t b ) { return ARCH_NAME ## _f16_eq( a, b ); }                                                                                                           \
	static bool f16_le( float16_t a, float16_t b ) { return ARCH_NAME ## _f16_le( a, b ); }                                                                                                           \
	static bool f16_lt( float16_t a, float16_t b ) { return ARCH_NAME ## _f16_lt( a, b ); }                                                                                                           \
	static bool f16_eq_signaling( float16_t a, float16_t b ) { return ARCH_NAME ## _f16_eq_signaling( a, b ); }                                                                                       \
	static bool f16_le_quiet( float16_t a, float16_t b ) { return ARCH_NAME ## _f16_le_quiet( a, b ); }                                                                                               \
	static bool f16_lt_quiet( float16_t a, float16_t b ) { return ARCH_NAME ## _f16_lt_quiet( a, b ); }                                                                                               \
	static bool f16_isSignalingNaN( float16_t a ) { return ARCH_NAME ## _f16_isSignalingNaN( a ); }                                                                                                   \
                                                                                                                                                                                                    \
	/*----------------------------------------------------------------------------                                                                                                                    \
	| 32-bit (single-precision) floating-point operations.                                                                                                                                            \
	*----------------------------------------------------------------------------*/                                                                                                                   \
	static uint_fast32_t f32_to_ui32( float32_t a, uint_fast8_t rmode, bool exact ) { return ARCH_NAME ## _f32_to_ui32( a, FromRoundingMode( rmode ), exact ); }                                      \
	static uint_fast64_t f32_to_ui64( float32_t a, uint_fast8_t rmode, bool exact ) { return ARCH_NAME ## _f32_to_ui64( a, FromRoundingMode( rmode ), exact ); }                                      \
	static int_fast32_t f32_to_i32( float32_t a, uint_fast8_t rmode, bool exact ) { return ARCH_NAME ## _f32_to_i32( a, FromRoundingMode( rmode ), exact ); }                                         \
	static int_fast64_t f32_to_i64( float32_t a, uint_fast8_t rmode, bool exact ) { return ARCH_NAME ## _f32_to_i64( a, FromRoundingMode( rmode ), exact ); }                                         \
	static uint_fast32_t f32_to_ui32_r_minMag( float32_t a, bool exact ) { return ARCH_NAME ## _f32_to_ui32_r_minMag( a, exact ); }                                                                   \
	static uint_fast64_t f32_to_ui64_r_minMag( float32_t a, bool exact ) { return ARCH_NAME ## _f32_to_ui64_r_minMag( a, exact ); }                                                                   \
	static int_fast32_t f32_to_i32_r_minMag( float32_t a, bool exact ) { return ARCH_NAME ## _f32_to_i32_r_minMag( a, exact ); }                                                                      \
	static int_fast64_t f32_to_i64_r_minMag( float32_t a, bool exact ) { return ARCH_NAME ## _f32_to_i64_r_minMag( a, exact ); }                                                                      \
	static float16_t f32_to_f16( float32_t a ) { return ARCH_NAME ## _f32_to_f16( a ); }                                                                                                              \
	static float64_t f32_to_f64( float32_t a ) { return ARCH_NAME ## _f32_to_f64( a ); }                                                                                                              \
	static void f32_to_extF80M( float32_t a, extFloat80_t *z ) { ARCH_NAME ## _f32_to_extF80M( a, z ); }                                                                                              \
	static void f32_to_f128M( float32_t a, float128_t *z ) { ARCH_NAME ## _f32_to_f128M( a, z ); }                                                                                                    \
	static float32_t f32_roundToInt( float32_t a, uint_fast8_t rmode, bool exact ) { return ARCH_NAME ## _f32_roundToInt( a, FromRoundingMode( rmode ), exact ); }                                    \
	static float32_t f32_add( float32_t a, float32_t b ) { return ARCH_NAME ## _f32_add( a, b ); }                                                                                                    \
	static float32_t f32_sub( float32_t a, float32_t b ) { return ARCH_NAME ## _f32_sub( a, b ); }                                                                                                    \
	static float32_t f32_mul( float32_t a, float32_t b ) { return ARCH_NAME ## _f32_mul( a, b ); }                                                                                                    \
	static float32_t f32_mulAdd( float32_t a, float32_t b, float32_t c ) { return ARCH_NAME ## _f32_mulAdd(a, b, c); }                                                                                \
	static float32_t f32_div( float32_t a, float32_t b ) { return ARCH_NAME ## _f32_div( a, b ); }                                                                                                    \
	static float32_t f32_rem( float32_t a, float32_t b ) { return ARCH_NAME ## _f32_rem( a, b ); }                                                                                                    \
	static float32_t f32_sqrt( float32_t a ) { return ARCH_NAME ## _f32_sqrt( a ); }                                                                                                                  \
	static bool f32_eq( float32_t a, float32_t b ) { return ARCH_NAME ## _f32_eq( a, b ); }                                                                                                           \
	static bool f32_le( float32_t a, float32_t b ) { return ARCH_NAME ## _f32_le( a, b ); }                                                                                                           \
	static bool f32_lt( float32_t a, float32_t b ) { return ARCH_NAME ## _f32_lt( a, b ); }                                                                                                           \
	static bool f32_eq_signaling( float32_t a, float32_t b ) { return ARCH_NAME ## _f32_eq_signaling( a, b ); }                                                                                       \
	static bool f32_le_quiet( float32_t a, float32_t b ) { return ARCH_NAME ## _f32_le_quiet( a, b ); }                                                                                               \
	static bool f32_lt_quiet( float32_t a, float32_t b ) { return ARCH_NAME ## _f32_lt_quiet( a, b ); }                                                                                               \
	static bool f32_isSignalingNaN( float32_t a ) { return ARCH_NAME ## _f32_isSignalingNaN( a ); }                                                                                                   \
                                                                                                                                                                                                    \
	/*----------------------------------------------------------------------------                                                                                                                    \
	| 64-bit (double-precision) floating-point operations.                                                                                                                                            \
	*----------------------------------------------------------------------------*/                                                                                                                   \
	static uint_fast32_t f64_to_ui32( float64_t a, uint_fast8_t rmode, bool exact ) { return ARCH_NAME ## _f64_to_ui32( a, FromRoundingMode( rmode ), exact ); }                                      \
	static uint_fast64_t f64_to_ui64( float64_t a, uint_fast8_t rmode, bool  exact) { return ARCH_NAME ## _f64_to_ui64( a, FromRoundingMode( rmode ), exact ); }                                      \
	static int_fast32_t f64_to_i32( float64_t a, uint_fast8_t rmode, bool exact ) { return ARCH_NAME ## _f64_to_i32( a, FromRoundingMode( rmode ), exact ); }                                         \
	static int_fast64_t f64_to_i64( float64_t a, uint_fast8_t rmode, bool exact ) { return ARCH_NAME ## _f64_to_i64( a, FromRoundingMode( rmode ), exact ); }                                         \
	static uint_fast32_t f64_to_ui32_r_minMag( float64_t a, bool exact ) { return ARCH_NAME ## _f64_to_ui32_r_minMag( a, exact ); }                                                                   \
	static uint_fast64_t f64_to_ui64_r_minMag( float64_t a, bool exact ) { return ARCH_NAME ## _f64_to_ui64_r_minMag( a, exact ); }                                                                   \
	static int_fast32_t f64_to_i32_r_minMag( float64_t a, bool exact ) { return ARCH_NAME ## _f64_to_i32_r_minMag( a, exact ); }                                                                      \
	static int_fast64_t f64_to_i64_r_minMag( float64_t a, bool exact ) { return ARCH_NAME ## _f64_to_i64_r_minMag( a, exact ); }                                                                      \
	static float16_t f64_to_f16( float64_t a ) { return ARCH_NAME ## _f64_to_f16( a ); }                                                                                                              \
	static float32_t f64_to_f32( float64_t a ) { return ARCH_NAME ## _f64_to_f32( a ); }                                                                                                              \
	static void f64_to_extF80M( float64_t a, extFloat80_t *z ) { ARCH_NAME ## _f64_to_extF80M( a, z ); }                                                                                              \
	static void f64_to_f128M( float64_t a, float128_t *z ) { ARCH_NAME ## _f64_to_f128M( a, z ); }                                                                                                    \
	static float64_t f64_roundToInt( float64_t a, uint_fast8_t rmode, bool exact ) { return ARCH_NAME ## _f64_roundToInt( a, FromRoundingMode( rmode ), exact ); }                                    \
	static float64_t f64_add( float64_t a, float64_t b ) { return ARCH_NAME ## _f64_add( a, b ); }                                                                                                    \
	static float64_t f64_sub( float64_t a, float64_t b ) { return ARCH_NAME ## _f64_sub( a, b ); }                                                                                                    \
	static float64_t f64_mul( float64_t a, float64_t b ) { return ARCH_NAME ## _f64_mul( a, b ); }                                                                                                    \
	static float64_t f64_mulAdd( float64_t a, float64_t b, float64_t c) { return ARCH_NAME ## _f64_mulAdd(a, b, c); }                                                                                 \
	static float64_t f64_div( float64_t a, float64_t b) { return ARCH_NAME ## _f64_div( a, b ); }                                                                                                     \
	static float64_t f64_rem( float64_t a, float64_t b) { return ARCH_NAME ## _f64_rem( a, b ); }                                                                                                     \
	static float64_t f64_sqrt( float64_t a ) { return ARCH_NAME ## _f64_sqrt( a ); }                                                                                                                  \
	static bool f64_eq( float64_t a, float64_t b) { return ARCH_NAME ## _f64_eq( a, b ); }                                                                                                            \
	static bool f64_le( float64_t a, float64_t b) { return ARCH_NAME ## _f64_le( a, b ); }                                                                                                            \
	static bool f64_lt( float64_t a, float64_t b) { return ARCH_NAME ## _f64_lt( a, b ); }                                                                                                            \
	static bool f64_eq_signaling( float64_t a, float64_t b) { return ARCH_NAME ## _f64_eq_signaling( a, b ); }                                                                                        \
	static bool f64_le_quiet( float64_t a, float64_t b) { return ARCH_NAME ## _f64_le_quiet( a, b ); }                                                                                                \
	static bool f64_lt_quiet( float64_t a, float64_t b) { return ARCH_NAME ## _f64_lt_quiet( a, b ); }                                                                                                \
	static bool f64_isSignalingNaN( float64_t a ) { return ARCH_NAME ## _f64_isSignalingNaN( a ); }                                                                                                   \
                                                                                                                                                                                                    \
	/*----------------------------------------------------------------------------                                                                                                                    \
	| 80-bit extended double-precision floating-point operations.                                                                                                                                     \
	*----------------------------------------------------------------------------*/                                                                                                                   \
	static uint_fast32_t extF80M_to_ui32( const extFloat80_t *a, uint_fast8_t rmode, bool exact ) { return ARCH_NAME ## _extF80M_to_ui32( a, FromRoundingMode( rmode ), exact ); }                    \
	static uint_fast64_t extF80M_to_ui64( const extFloat80_t *a, uint_fast8_t rmode, bool exact ) { return ARCH_NAME ## _extF80M_to_ui64( a, FromRoundingMode( rmode ), exact ); }                    \
	static int_fast32_t extF80M_to_i32( const extFloat80_t *a, uint_fast8_t rmode, bool exact ) { return ARCH_NAME ## _extF80M_to_i32( a, FromRoundingMode( rmode ), exact ); }                       \
	static int_fast64_t extF80M_to_i64( const extFloat80_t *a, uint_fast8_t rmode, bool exact ) { return ARCH_NAME ## _extF80M_to_i64( a, FromRoundingMode( rmode ), exact ); }                       \
	static uint_fast32_t extF80M_to_ui32_r_minMag( const extFloat80_t *a, bool exact ) { return ARCH_NAME ## _extF80M_to_ui32_r_minMag( a, exact ); }                                                 \
	static uint_fast64_t extF80M_to_ui64_r_minMag( const extFloat80_t *a, bool exact ) { return ARCH_NAME ## _extF80M_to_ui64_r_minMag( a, exact ); }                                                 \
	static int_fast32_t extF80M_to_i32_r_minMag( const extFloat80_t *a, bool exact ) { return ARCH_NAME ## _extF80M_to_i32_r_minMag( a, exact ); }                                                    \
	static int_fast64_t extF80M_to_i64_r_minMag( const extFloat80_t *a, bool exact ) { return ARCH_NAME ## _extF80M_to_i64_r_minMag( a, exact ); }                                                    \
	static float16_t extF80M_to_f16( const extFloat80_t *a) { return ARCH_NAME ## _extF80M_to_f16( a ); }                                                                                             \
	static float32_t extF80M_to_f32( const extFloat80_t *a) { return ARCH_NAME ## _extF80M_to_f32( a ); }                                                                                             \
	static float64_t extF80M_to_f64( const extFloat80_t *a) { return ARCH_NAME ## _extF80M_to_f64( a ); }                                                                                             \
	static void extF80M_to_f128M( const extFloat80_t *a, float128_t * z ) { return ARCH_NAME ## _extF80M_to_f128M( a, z ); }                                                                          \
	static void extF80M_roundToInt(const extFloat80_t *a, uint_fast8_t rmode, bool exact, extFloat80_t *z ) { return ARCH_NAME ## _extF80M_roundToInt( a, FromRoundingMode( rmode ), exact, z ); }    \
	static void extF80M_add( const extFloat80_t *a, const extFloat80_t *b, extFloat80_t *z ) { return ARCH_NAME ## _extF80M_add( a, b, z ); }                                                         \
	static void extF80M_sub( const extFloat80_t *a, const extFloat80_t *b, extFloat80_t *z ) { return ARCH_NAME ## _extF80M_sub( a, b, z ); }                                                         \
	static void extF80M_mul( const extFloat80_t *a, const extFloat80_t *b, extFloat80_t *z ) { return ARCH_NAME ## _extF80M_mul( a, b, z ); }                                                         \
	static void extF80M_div( const extFloat80_t *a, const extFloat80_t *b, extFloat80_t *z ) { return ARCH_NAME ## _extF80M_div( a, b, z ); }                                                         \
	static void extF80M_rem( const extFloat80_t *a, const extFloat80_t *b, extFloat80_t *z ) { return ARCH_NAME ## _extF80M_rem( a, b, z ); }                                                         \
	static void extF80M_sqrt( const extFloat80_t *a, extFloat80_t *z ) { return ARCH_NAME ## _extF80M_sqrt( a, z ); }                                                                                 \
	static bool extF80M_eq( const extFloat80_t *a, const extFloat80_t *b ) { return ARCH_NAME ## _extF80M_eq( a, b ); }                                                                               \
	static bool extF80M_le( const extFloat80_t *a, const extFloat80_t *b ) { return ARCH_NAME ## _extF80M_le( a, b ); }                                                                               \
	static bool extF80M_lt( const extFloat80_t *a, const extFloat80_t *b ) { return ARCH_NAME ## _extF80M_lt( a, b ); }                                                                               \
	static bool extF80M_eq_signaling( const extFloat80_t *a, const extFloat80_t *b ) { return ARCH_NAME ## _extF80M_eq_signaling( a, b ); }                                                           \
	static bool extF80M_le_quiet( const extFloat80_t *a, const extFloat80_t *b ) { return ARCH_NAME ## _extF80M_le_quiet( a, b ); }                                                                   \
	static bool extF80M_lt_quiet( const extFloat80_t *a, const extFloat80_t *b ) { return ARCH_NAME ## _extF80M_lt_quiet( a, b ); }                                                                   \
	static bool extF80M_isSignalingNaN( const extFloat80_t *a ) { return ARCH_NAME ## _extF80M_isSignalingNaN( a ); }                                                                                 \
                                                                                                                                                                                                    \
	/*----------------------------------------------------------------------------                                                                                                                    \
	| 128-bit (quadruple-precision) floating-point operations.                                                                                                                                        \
	*----------------------------------------------------------------------------*/                                                                                                                   \
	static uint_fast32_t f128M_to_ui32( const float128_t *a, uint_fast8_t rmode, bool exact ) { return ARCH_NAME ## _f128M_to_ui32( a, FromRoundingMode( rmode ), exact ); }                          \
	static uint_fast64_t f128M_to_ui64( const float128_t *a, uint_fast8_t rmode, bool exact) { return ARCH_NAME ## _f128M_to_ui64( a, FromRoundingMode( rmode ), exact ); }                           \
	static int_fast32_t f128M_to_i32( const float128_t *a, uint_fast8_t rmode, bool exact ) { return ARCH_NAME ## _f128M_to_i32( a, FromRoundingMode( rmode ), exact ); }                             \
	static int_fast64_t f128M_to_i64( const float128_t *a, uint_fast8_t rmode, bool exact ) { return ARCH_NAME ## _f128M_to_i64( a, FromRoundingMode( rmode ), exact ); }                             \
	static uint_fast32_t f128M_to_ui32_r_minMag( const float128_t *a, bool exact ) { return ARCH_NAME ## _f128M_to_ui32_r_minMag( a, exact ); }                                                       \
	static uint_fast64_t f128M_to_ui64_r_minMag( const float128_t *a, bool exact ) { return ARCH_NAME ## _f128M_to_ui64_r_minMag( a, exact ); }                                                       \
	static int_fast32_t f128M_to_i32_r_minMag( const float128_t *a, bool exact ) { return ARCH_NAME ## _f128M_to_i32_r_minMag( a, exact ); }                                                          \
	static int_fast64_t f128M_to_i64_r_minMag( const float128_t *a, bool exact ) { return ARCH_NAME ## _f128M_to_i64_r_minMag( a, exact ); }                                                          \
	static float16_t f128M_to_f16( const float128_t *a ) { return ARCH_NAME ## _f128M_to_f16( a ); }                                                                                                  \
	static float32_t f128M_to_f32( const float128_t *a ) { return ARCH_NAME ## _f128M_to_f32( a ); }                                                                                                  \
	static float64_t f128M_to_f64( const float128_t *a ) { return ARCH_NAME ## _f128M_to_f64( a ); }                                                                                                  \
	static void f128M_to_extF80M( const float128_t *a, extFloat80_t * z) { return ARCH_NAME ## _f128M_to_extF80M( a, z ); }                                                                           \
	static void f128M_roundToInt( const float128_t *a, uint_fast8_t rmode, bool exact, float128_t *z ) { return ARCH_NAME ## _f128M_roundToInt( a, FromRoundingMode( rmode ), exact, z ); }           \
	static void f128M_add( const float128_t *a, const float128_t *b, float128_t *z ) { return ARCH_NAME ## _f128M_add( a, b , z ); }                                                                  \
	static void f128M_sub( const float128_t *a, const float128_t *b, float128_t *z ) { return ARCH_NAME ## _f128M_sub( a, b , z ); }                                                                  \
	static void f128M_mul( const float128_t *a, const float128_t *b, float128_t *z ) { return ARCH_NAME ## _f128M_mul( a, b , z ); }                                                                  \
	static void f128M_mulAdd(const float128_t *a, const float128_t *b, const float128_t *c, float128_t *z) { return ARCH_NAME ## _f128M_mulAdd( a, b, c, z ); }                                       \
	static void f128M_div( const float128_t *a, const float128_t *b, float128_t *z ) { return ARCH_NAME ## _f128M_div( a, b , z ); }                                                                  \
	static void f128M_rem( const float128_t *a, const float128_t *b, float128_t *z ) { return ARCH_NAME ## _f128M_rem( a, b , z ); }                                                                  \
	static void f128M_sqrt( const float128_t *a, float128_t * z) { return ARCH_NAME ## _f128M_sqrt( a, z ); }                                                                                         \
	static bool f128M_eq( const float128_t *a, const float128_t *b ) { return ARCH_NAME ## _f128M_eq( a, b ); }                                                                                       \
	static bool f128M_le( const float128_t *a, const float128_t *b ) { return ARCH_NAME ## _f128M_le( a, b ); }                                                                                       \
	static bool f128M_lt( const float128_t *a, const float128_t *b ) { return ARCH_NAME ## _f128M_lt( a, b ); }                                                                                       \
	static bool f128M_eq_signaling( const float128_t *a, const float128_t *b ) { return ARCH_NAME ## _f128M_eq_signaling( a, b ); }                                                                   \
	static bool f128M_le_quiet( const float128_t *a, const float128_t *b ) { return ARCH_NAME ## _f128M_le_quiet( a, b ); }                                                                           \
	static bool f128M_lt_quiet( const float128_t *a, const float128_t *b ) { return ARCH_NAME ## _f128M_lt_quiet( a, b ); }                                                                           \
	static bool f128M_isSignalingNaN( const float128_t *a ) { return ARCH_NAME ## _f128M_isSignalingNaN( a ); }

#undef SOFTFLOAT_EMU_DEF_ARCH_IF_FUNCTIONS_FAST_INT64
#if SOFTFLOAT_EMU_FAST_INT64
#define SOFTFLOAT_EMU_DEF_ARCH_IF_FUNCTIONS_FAST_INT64( ARCH_NAME )                                                                                                                                 \
	/*----------------------------------------------------------------------------                                                                                                                    \
	| Integer-to-floating-point conversion routines.                                                                                                                                                  \
	*----------------------------------------------------------------------------*/                                                                                                                   \
	static extFloat80_t ui32_to_extF80( uint32_t a ) { return ARCH_NAME ## _ui32_to_extF80( a ); }                                                                                                    \
	static float128_t ui32_to_f128( uint32_t a ) { return ARCH_NAME ## _ui32_to_f128( a ); }                                                                                                          \
	static extFloat80_t ui64_to_extF80( uint64_t a ) { return ARCH_NAME ## _ui64_to_extF80( a ); }                                                                                                    \
	static float128_t ui64_to_f128( uint64_t a ) { return ARCH_NAME ## _ui64_to_f128( a ); }                                                                                                          \
	static extFloat80_t i32_to_extF80( int32_t a ) { return ARCH_NAME ## _i32_to_extF80( a ); }                                                                                                       \
	static float128_t i32_to_f128( int32_t a ) { return ARCH_NAME ## _i32_to_f128( a ); }                                                                                                             \
	static extFloat80_t i64_to_extF80( int64_t a ) { return ARCH_NAME ## _i64_to_extF80( a ); }                                                                                                       \
	static float128_t i64_to_f128( int64_t a ) { return ARCH_NAME ## _i64_to_f128( a ); }                                                                                                             \
                                                                                                                                                                                                    \
	/*----------------------------------------------------------------------------                                                                                                                    \
	| 16-bit (half-precision) floating-point operations.                                                                                                                                              \
	*----------------------------------------------------------------------------*/                                                                                                                   \
	static extFloat80_t f16_to_extF80( float16_t a ) { return ARCH_NAME ## _f16_to_extF80( a ); }                                                                                                     \
	static float128_t f16_to_f128( float16_t a ) { return ARCH_NAME ## _f16_to_f128( a ); }                                                                                                           \
                                                                                                                                                                                                    \
	/*----------------------------------------------------------------------------                                                                                                                    \
	| 32-bit (single-precision) floating-point operations.                                                                                                                                            \
	*----------------------------------------------------------------------------*/                                                                                                                   \
	static extFloat80_t f32_to_extF80( float32_t a ) { return ARCH_NAME ## _f32_to_extF80( a ); }                                                                                                     \
	static float128_t f32_to_f128( float32_t a ) { return ARCH_NAME ## _f32_to_f128( a ); }                                                                                                           \
                                                                                                                                                                                                    \
	/*----------------------------------------------------------------------------                                                                                                                    \
	| 64-bit (double-precision) floating-point operations.                                                                                                                                            \
	*----------------------------------------------------------------------------*/                                                                                                                   \
	static extFloat80_t f64_to_extF80( float64_t a ) { return ARCH_NAME ## _f64_to_extF80( a ); }                                                                                                     \
	static float128_t f64_to_f128( float64_t a ) { return ARCH_NAME ## _f64_to_f128( a ); }                                                                                                           \
                                                                                                                                                                                                    \
	/*----------------------------------------------------------------------------                                                                                                                    \
	| 80-bit extended double-precision floating-point operations.                                                                                                                                     \
	*----------------------------------------------------------------------------*/                                                                                                                   \
	static uint_fast32_t extF80_to_ui32( extFloat80_t a, uint_fast8_t rmode , bool exact ) { return ARCH_NAME ## _extF80_to_ui32( a, FromRoundingMode( rmode ), exact ); }                            \
	static uint_fast64_t extF80_to_ui64( extFloat80_t a, uint_fast8_t rmode , bool exact ) { return ARCH_NAME ## _extF80_to_ui64( a, FromRoundingMode( rmode ), exact ); }                            \
	static int_fast32_t extF80_to_i32( extFloat80_t a, uint_fast8_t rmode, bool exact ) { return ARCH_NAME ## _extF80_to_i32( a, FromRoundingMode( rmode ), exact ); }                                \
	static int_fast64_t extF80_to_i64( extFloat80_t a, uint_fast8_t rmode, bool exact ) { return ARCH_NAME ## _extF80_to_i64( a, FromRoundingMode( rmode ), exact ); }                                \
	static uint_fast32_t extF80_to_ui32_r_minMag( extFloat80_t a, bool exact ) { return ARCH_NAME ## _extF80_to_ui32_r_minMag( a, exact ); }                                                          \
	static uint_fast64_t extF80_to_ui64_r_minMag( extFloat80_t a, bool exact ) { return ARCH_NAME ## _extF80_to_ui64_r_minMag( a, exact ); }                                                          \
	static int_fast32_t extF80_to_i32_r_minMag( extFloat80_t a, bool exact ) { return ARCH_NAME ## _extF80_to_i32_r_minMag( a, exact ); }                                                             \
	static int_fast64_t extF80_to_i64_r_minMag( extFloat80_t a, bool exact ) { return ARCH_NAME ## _extF80_to_i64_r_minMag( a, exact ); }                                                             \
	static float16_t extF80_to_f16( extFloat80_t a ) { return ARCH_NAME ## _extF80_to_f16( a ); }                                                                                                     \
	static float32_t extF80_to_f32( extFloat80_t a ) { return ARCH_NAME ## _extF80_to_f32( a ); }                                                                                                     \
	static float64_t extF80_to_f64( extFloat80_t a ) { return ARCH_NAME ## _extF80_to_f64( a ); }                                                                                                     \
	static float128_t extF80_to_f128( extFloat80_t a ) { return ARCH_NAME ## _extF80_to_f128( a ); }                                                                                                  \
	static extFloat80_t extF80_roundToInt( extFloat80_t a, uint_fast8_t rmode, bool exact ) { return ARCH_NAME ## _extF80_roundToInt( a, FromRoundingMode( rmode ), exact ); }                        \
	static extFloat80_t extF80_add( extFloat80_t a, extFloat80_t b ) { return ARCH_NAME ## _extF80_add( a, b ); }                                                                                     \
	static extFloat80_t extF80_sub( extFloat80_t a, extFloat80_t b ) { return ARCH_NAME ## _extF80_sub( a, b ); }                                                                                     \
	static extFloat80_t extF80_mul( extFloat80_t a, extFloat80_t b ) { return ARCH_NAME ## _extF80_mul( a, b ); }                                                                                     \
	static extFloat80_t extF80_div( extFloat80_t a, extFloat80_t b ) { return ARCH_NAME ## _extF80_div( a, b ); }                                                                                     \
	static extFloat80_t extF80_rem( extFloat80_t a, extFloat80_t b ) { return ARCH_NAME ## _extF80_rem( a, b ); }                                                                                     \
	static extFloat80_t extF80_sqrt( extFloat80_t a ) { return ARCH_NAME ## _extF80_sqrt( a ); }                                                                                                      \
	static bool extF80_eq( extFloat80_t a, extFloat80_t b ) { return ARCH_NAME ## _extF80_eq( a, b ); }                                                                                               \
	static bool extF80_le( extFloat80_t a, extFloat80_t b ) { return ARCH_NAME ## _extF80_le( a, b ); }                                                                                               \
	static bool extF80_lt( extFloat80_t a, extFloat80_t b ) { return ARCH_NAME ## _extF80_lt( a, b ); }                                                                                               \
	static bool extF80_eq_signaling( extFloat80_t a, extFloat80_t b ) { return ARCH_NAME ## _extF80_eq_signaling( a, b ); }                                                                           \
	static bool extF80_le_quiet( extFloat80_t a, extFloat80_t b ) { return ARCH_NAME ## _extF80_le_quiet( a, b ); }                                                                                   \
	static bool extF80_lt_quiet( extFloat80_t a, extFloat80_t b ) { return ARCH_NAME ## _extF80_lt_quiet( a, b ); }                                                                                   \
	static bool extF80_isSignalingNaN( extFloat80_t a ) { return ARCH_NAME ## _extF80_isSignalingNaN( a ); }                                                                                          \
                                                                                                                                                                                                    \
	/*----------------------------------------------------------------------------                                                                                                                    \
	| 128-bit (quadruple-precision) floating-point operations.                                                                                                                                        \
	*----------------------------------------------------------------------------*/                                                                                                                   \
	static uint_fast32_t f128_to_ui32( float128_t a, uint_fast8_t rmode, bool exact ) { return ARCH_NAME ## _f128_to_ui32( a, FromRoundingMode( rmode ), exact ); }                                   \
	static uint_fast64_t f128_to_ui64( float128_t a, uint_fast8_t rmode, bool exact ) { return ARCH_NAME ## _f128_to_ui64( a, FromRoundingMode( rmode ), exact ); }                                   \
	static int_fast32_t f128_to_i32( float128_t a, uint_fast8_t rmode, bool exact ) { return ARCH_NAME ## _f128_to_i32( a, FromRoundingMode( rmode ), exact ); }                                      \
	static int_fast64_t f128_to_i64( float128_t a, uint_fast8_t rmode, bool exact ) { return ARCH_NAME ## _f128_to_i64( a, FromRoundingMode( rmode ), exact ); }                                      \
	static uint_fast32_t f128_to_ui32_r_minMag( float128_t a, bool exact ) { return ARCH_NAME ## _f128_to_ui32_r_minMag( a, exact ); }                                                                \
	static uint_fast64_t f128_to_ui64_r_minMag( float128_t a, bool exact ) { return ARCH_NAME ## _f128_to_ui64_r_minMag( a, exact ); }                                                                \
	static int_fast32_t f128_to_i32_r_minMag( float128_t a, bool exact ) { return ARCH_NAME ## _f128_to_i32_r_minMag( a, exact ); }                                                                   \
	static int_fast64_t f128_to_i64_r_minMag( float128_t a, bool exact ) { return ARCH_NAME ## _f128_to_i64_r_minMag( a, exact ); }                                                                   \
	static float16_t f128_to_f16( float128_t a ) { return ARCH_NAME ## _f128_to_f16( a ); }                                                                                                           \
	static float32_t f128_to_f32( float128_t a ) { return ARCH_NAME ## _f128_to_f32( a ); }                                                                                                           \
	static float64_t f128_to_f64( float128_t a ) { return ARCH_NAME ## _f128_to_f64( a ); }                                                                                                           \
	static extFloat80_t f128_to_extF80( float128_t a ) { return ARCH_NAME ## _f128_to_extF80( a ); }                                                                                                  \
	static float128_t f128_roundToInt( float128_t a, uint_fast8_t rmode, bool exact ) { return ARCH_NAME ## _f128_roundToInt( a, FromRoundingMode( rmode ), exact ); }                                \
	static float128_t f128_add( float128_t a, float128_t b ) { return ARCH_NAME ## _f128_add( a, b ); }                                                                                               \
	static float128_t f128_sub( float128_t a, float128_t b ) { return ARCH_NAME ## _f128_sub( a, b ); }                                                                                               \
	static float128_t f128_mul( float128_t a, float128_t b ) { return ARCH_NAME ## _f128_mul( a, b ); }                                                                                               \
	static float128_t f128_mulAdd( float128_t a, float128_t b, float128_t c) { return ARCH_NAME ## _f128_mulAdd(a, b, c); }                                                                           \
	static float128_t f128_div( float128_t a, float128_t b ) { return ARCH_NAME ## _f128_div( a, b ); }                                                                                               \
	static float128_t f128_rem( float128_t a, float128_t b ) { return ARCH_NAME ## _f128_rem( a, b ); }                                                                                               \
	static float128_t f128_sqrt( float128_t a  ) { return ARCH_NAME ## _f128_sqrt( a ); }                                                                                                             \
	static bool f128_eq( float128_t a, float128_t b ) { return ARCH_NAME ## _f128_eq( a, b ); }                                                                                                       \
	static bool f128_le( float128_t a, float128_t b ) { return ARCH_NAME ## _f128_le( a, b ); }                                                                                                       \
	static bool f128_lt( float128_t a, float128_t b ) { return ARCH_NAME ## _f128_lt( a, b ); }                                                                                                       \
	static bool f128_eq_signaling( float128_t a, float128_t b ) { return ARCH_NAME ## _f128_eq_signaling( a, b ); }                                                                                   \
	static bool f128_le_quiet( float128_t a, float128_t b ) { return ARCH_NAME ## _f128_le_quiet( a, b ); }                                                                                           \
	static bool f128_lt_quiet( float128_t a, float128_t b ) { return ARCH_NAME ## _f128_lt_quiet( a, b ); }                                                                                           \
	static bool f128_isSignalingNaN( float128_t a ) { return ARCH_NAME ## _f128_isSignalingNaN( a ); }
#else
#define SOFTFLOAT_EMU_DEF_ARCH_IF_FUNCTIONS_FAST_INT64
#endif

#undef SOFTFLOAT_EMU_DEF_ARCH_IF
#define SOFTFLOAT_EMU_DEF_ARCH_IF( IF_NAME, ARCH_NAME )                                                                                                                                             \
struct IF_NAME                                                                                                                                                                                      \
{                                                                                                                                                                                                   \
	/*----------------------------------------------------------------------------                                                                                                                    \
	| Software floating-point underflow tininess-detection mode.                                                                                                                                      \
	*----------------------------------------------------------------------------*/                                                                                                                   \
	                                                                                                                                                                                                  \
	static void detectTininess( uint_fast8_t dt ) { softfloat_emu_ ## ARCH_NAME ## _detectTininess = FromDetectTininess( dt ); }                                                                      \
	static uint_fast8_t detectTininess() { return ToDetectTininess( softfloat_emu_ ## ARCH_NAME ## _detectTininess ); }                                                                               \
	                                                                                                                                                                                                  \
	/*----------------------------------------------------------------------------                                                                                                                    \
	| Software floating-point rounding mode.  (Mode "odd" is supported only if                                                                                                                        \
	| SoftFloat is compiled with macro 'SOFTFLOAT_ROUND_ODD' defined.)                                                                                                                                \
	*----------------------------------------------------------------------------*/                                                                                                                   \
	                                                                                                                                                                                                  \
	static void roundingMode( uint_fast8_t rmode ) { softfloat_emu_ ## ARCH_NAME ## _roundingMode = FromRoundingMode( rmode ); }                                                                      \
	static uint_fast8_t roundingMode() { return ToRoundingMode( softfloat_emu_ ## ARCH_NAME ## _roundingMode ); }                                                                                     \
	                                                                                                                                                                                                  \
	/*----------------------------------------------------------------------------                                                                                                                    \
	| Software floating-point exception flags.                                                                                                                                                        \
	*----------------------------------------------------------------------------*/                                                                                                                   \
	                                                                                                                                                                                                  \
	static void exceptionFlags( uint_fast8_t eflags ) { softfloat_emu_ ## ARCH_NAME ## _exceptionFlags = FromExceptionFlags( eflags ); }                                                              \
	static uint_fast8_t exceptionFlags() { return ToExceptionFlags( softfloat_emu_ ## ARCH_NAME ## _exceptionFlags ); }                                                                               \
	                                                                                                                                                                                                  \
	/*----------------------------------------------------------------------------                                                                                                                    \
	| Rounding precision for 80-bit extended double-precision floating-point.                                                                                                                         \
	| Valid values are 32, 64, and 80.                                                                                                                                                                \
	*----------------------------------------------------------------------------*/                                                                                                                   \
	                                                                                                                                                                                                  \
	static void extF80_roundingPrecision( uint_fast8_t ext80_rp ) { ARCH_NAME ## _extF80_roundingPrecision = ext80_rp; }                                                                              \
	static uint_fast8_t extF80_roundingPrecision() { return ARCH_NAME ## _extF80_roundingPrecision; }                                                                                                 \
	                                                                                                                                                                                                  \
	/*----------------------------------------------------------------------------                                                                                                                    \
	| Software floating-point flush-to-zero mode.                                                                                                                                                     \
	*----------------------------------------------------------------------------*/                                                                                                                   \
	                                                                                                                                                                                                  \
	static void flushToZero( uint_fast8_t ftz ) { softfloat_emu_ ## ARCH_NAME ## _flushToZero = FromFlushToZero( ftz ); }                                                                             \
	static uint_fast8_t flushToZero() { return ToFlushToZero( softfloat_emu_ ## ARCH_NAME ## _flushToZero ); }                                                                                        \
	                                                                                                                                                                                                  \
	/*----------------------------------------------------------------------------                                                                                                                    \
	| C++ friendly overloaded routines.                                                                                                                                                               \
	*----------------------------------------------------------------------------*/                                                                                                                   \
	                                                                                                                                                                                                  \
	static float16_t f_construct( uint32_t a, float16_t const * ) { return ui32_to_f16( a ); }                                                                                                        \
	static float32_t f_construct( uint32_t a, float32_t const * ) { return ui32_to_f32( a ); }                                                                                                        \
	static float64_t f_construct( uint32_t a, float64_t const * ) { return ui32_to_f64( a ); }                                                                                                        \
	static float16_t f_construct( uint64_t a, float16_t const * ) { return ui64_to_f16( a ); }                                                                                                        \
	static float32_t f_construct( uint64_t a, float32_t const * ) { return ui64_to_f32( a ); }                                                                                                        \
	static float64_t f_construct( uint64_t a, float64_t const * ) { return ui64_to_f64( a ); }                                                                                                        \
	static float16_t f_construct( int32_t a, float16_t const * ) { return i32_to_f16( a ); }                                                                                                          \
	static float32_t f_construct( int32_t a, float32_t const * ) { return i32_to_f32( a ); }                                                                                                          \
	static float64_t f_construct( int32_t a, float64_t const * ) { return i32_to_f64( a ); }                                                                                                          \
	static float16_t f_construct( int64_t a, float16_t const * ) { return i64_to_f16( a ); }                                                                                                          \
	static float32_t f_construct( int64_t a, float32_t const * ) { return i64_to_f32( a ); }                                                                                                          \
	static float64_t f_construct( int64_t a, float64_t const * ) { return i64_to_f64( a ); }                                                                                                          \
	static float16_t f_construct( float16_t a, float16_t const * ) { return a; }                                                                                                                      \
	static float32_t f_construct( float16_t a, float32_t const * ) { return f16_to_f32( a ); }                                                                                                        \
	static float64_t f_construct( float16_t a, float64_t const * ) { return f16_to_f64( a ); }                                                                                                        \
	static float16_t f_construct( float32_t a, float16_t const * ) { return f32_to_f16( a ); }                                                                                                        \
	static float32_t f_construct( float32_t a, float32_t const * ) { return a; }                                                                                                                      \
	static float64_t f_construct( float32_t a, float64_t const * ) { return f32_to_f64( a ); }                                                                                                        \
	static float16_t f_construct( float64_t a, float16_t const * ) { return f64_to_f16( a ); }                                                                                                        \
	static float32_t f_construct( float64_t a, float32_t const * ) { return f64_to_f32( a ); }                                                                                                        \
	static float64_t f_construct( float64_t a, float64_t const * ) { return a; }                                                                                                                      \
	static uint16_t f_toInt( float16_t a, uint_fast8_t rmode, bool exact, uint16_t const * ) { uint32_t ui = f16_to_ui32( a, FromRoundingMode( rmode ), exact );                                      \
		if( ui > std::numeric_limits<uint16_t>::max() ) { ui = std::numeric_limits<uint16_t>::max(); raiseFlags(flag_invalid); } return ui; }                                                           \
	static uint16_t f_toInt( float32_t a, uint_fast8_t rmode, bool exact, uint16_t const * ) { uint32_t ui = f32_to_ui32( a, FromRoundingMode( rmode ), exact );                                      \
		if( ui > std::numeric_limits<uint16_t>::max() ) { ui = std::numeric_limits<uint16_t>::max(); raiseFlags(flag_invalid); } return ui; }                                                           \
	static uint16_t f_toInt( float64_t a, uint_fast8_t rmode, bool exact, uint16_t const * ) { uint32_t ui = f64_to_ui32( a, FromRoundingMode( rmode ), exact );                                      \
		if( ui > std::numeric_limits<uint16_t>::max() ) { ui = std::numeric_limits<uint16_t>::max(); raiseFlags(flag_invalid); } return ui; }                                                           \
	static uint32_t f_toInt( float16_t a, uint_fast8_t rmode, bool exact, uint32_t const * ) { return f16_to_ui32( a, FromRoundingMode( rmode ), exact ); }                                           \
	static uint32_t f_toInt( float32_t a, uint_fast8_t rmode, bool exact, uint32_t const * ) { return f32_to_ui32( a, FromRoundingMode( rmode ), exact ); }                                           \
	static uint32_t f_toInt( float64_t a, uint_fast8_t rmode, bool exact, uint32_t const * ) { return f64_to_ui32( a, FromRoundingMode( rmode ), exact ); }                                           \
	static uint64_t f_toInt( float16_t a, uint_fast8_t rmode, bool exact, uint64_t const * ) { return f16_to_ui64( a, FromRoundingMode( rmode ), exact ); }                                           \
	static uint64_t f_toInt( float32_t a, uint_fast8_t rmode, bool exact, uint64_t const * ) { return f32_to_ui64( a, FromRoundingMode( rmode ), exact ); }                                           \
	static uint64_t f_toInt( float64_t a, uint_fast8_t rmode, bool exact, uint64_t const * ) { return f64_to_ui64( a, FromRoundingMode( rmode ), exact ); }                                           \
	static int16_t f_toInt( float16_t a, uint_fast8_t rmode, bool exact, int16_t const * ) { int32_t i = f16_to_i32( a, FromRoundingMode( rmode ), exact );                                           \
		{ if( i > std::numeric_limits<int16_t>::max() ) { i = std::numeric_limits<int16_t>::max(); raiseFlags(flag_invalid); }                                                                          \
		else if( i < std::numeric_limits<int16_t>::min() ) { i = std::numeric_limits<int16_t>::min(); raiseFlags(flag_invalid); } } return i; }                                                        \
	static int16_t f_toInt( float32_t a, uint_fast8_t rmode, bool exact, int16_t const * ) { int32_t i = f32_to_i32( a, FromRoundingMode( rmode ), exact );                                           \
		{ if( i > std::numeric_limits<int16_t>::max() ) { i = std::numeric_limits<int16_t>::max(); raiseFlags(flag_invalid); }                                                                          \
		else if( i < std::numeric_limits<int16_t>::min() ) { i = std::numeric_limits<int16_t>::min(); raiseFlags(flag_invalid); } } return i; }                                                        \
	static int16_t f_toInt( float64_t a, uint_fast8_t rmode, bool exact, int16_t const * ) { int32_t i = f64_to_i32( a, FromRoundingMode( rmode ), exact );                                           \
		{ if( i > std::numeric_limits<int16_t>::max() ) { i = std::numeric_limits<int16_t>::max(); raiseFlags(flag_invalid); }                                                                          \
		else if( i < std::numeric_limits<int16_t>::min() ) { i = std::numeric_limits<int16_t>::min(); raiseFlags(flag_invalid); } } return i; }                                                        \
	static int32_t f_toInt( float16_t a, uint_fast8_t rmode, bool exact, int32_t const * ) { return f16_to_i32( a, FromRoundingMode( rmode ), exact ); }                                              \
	static int32_t f_toInt( float32_t a, uint_fast8_t rmode, bool exact, int32_t const * ) { return f32_to_i32( a, FromRoundingMode( rmode ), exact ); }                                              \
	static int32_t f_toInt( float64_t a, uint_fast8_t rmode, bool exact, int32_t const * ) { return f64_to_i32( a, FromRoundingMode( rmode ), exact ); }                                              \
	static int64_t f_toInt( float16_t a, uint_fast8_t rmode, bool exact, int64_t const * ) { return f16_to_i64( a, FromRoundingMode( rmode ), exact ); }                                              \
	static int64_t f_toInt( float32_t a, uint_fast8_t rmode, bool exact, int64_t const * ) { return f32_to_i64( a, FromRoundingMode( rmode ), exact ); }                                              \
	static int64_t f_toInt( float64_t a, uint_fast8_t rmode, bool exact, int64_t const * ) { return f64_to_i64( a, FromRoundingMode( rmode ), exact ); }                                              \
	static uint16_t f_toInt_r_minMag( float16_t a, bool exact, uint16_t const * ) { uint32_t ui = f16_to_ui32_r_minMag( a, exact );                                                                   \
		if( ui > std::numeric_limits<uint16_t>::max() ) { ui = std::numeric_limits<uint16_t>::max(); raiseFlags(flag_invalid); } return ui; }                                                           \
	static uint16_t f_toInt_r_minMag( float32_t a, bool exact, uint16_t const * ) { uint32_t ui = f32_to_ui32_r_minMag( a, exact );                                                                   \
		if( ui > std::numeric_limits<uint16_t>::max() ) { ui = std::numeric_limits<uint16_t>::max(); raiseFlags(flag_invalid); } return ui; }                                                           \
	static uint16_t f_toInt_r_minMag( float64_t a, bool exact, uint16_t const * ) { uint32_t ui = f64_to_ui32_r_minMag( a, exact );                                                                   \
		if( ui > std::numeric_limits<uint16_t>::max() ) { ui = std::numeric_limits<uint16_t>::max(); raiseFlags(flag_invalid); } return ui; }                                                           \
	static uint32_t f_toInt_r_minMag( float16_t a, bool exact, uint32_t const * ) { return f16_to_ui32_r_minMag( a, exact ); }                                                                        \
	static uint32_t f_toInt_r_minMag( float32_t a, bool exact, uint32_t const * ) { return f32_to_ui32_r_minMag( a, exact ); }                                                                        \
	static uint32_t f_toInt_r_minMag( float64_t a, bool exact, uint32_t const * ) { return f64_to_ui32_r_minMag( a, exact ); }                                                                        \
	static uint64_t f_toInt_r_minMag( float16_t a, bool exact, uint64_t const * ) { return f16_to_ui64_r_minMag( a, exact ); }                                                                        \
	static uint64_t f_toInt_r_minMag( float32_t a, bool exact, uint64_t const * ) { return f32_to_ui64_r_minMag( a, exact ); }                                                                        \
	static uint64_t f_toInt_r_minMag( float64_t a, bool exact, uint64_t const * ) { return f64_to_ui64_r_minMag( a, exact ); }                                                                        \
	static int16_t f_toInt_r_minMag( float16_t a, bool exact, int16_t const * ) { int32_t i = f16_to_i32_r_minMag( a, exact );                                                                        \
		{ if( i > std::numeric_limits<int16_t>::max() ) { i = std::numeric_limits<int16_t>::max(); raiseFlags(flag_invalid); }                                                                          \
		else if( i < std::numeric_limits<int16_t>::min() ) { i = std::numeric_limits<int16_t>::min(); raiseFlags(flag_invalid); } } return i; }                                                        \
	static int16_t f_toInt_r_minMag( float32_t a, bool exact, int16_t const * ) { int32_t i = f32_to_i32_r_minMag( a, exact );                                                                        \
		{ if( i > std::numeric_limits<int16_t>::max() ) { i = std::numeric_limits<int16_t>::max(); raiseFlags(flag_invalid); }                                                                          \
		else if( i < std::numeric_limits<int16_t>::min() ) { i = std::numeric_limits<int16_t>::min(); raiseFlags(flag_invalid); } } return i; }                                                        \
	static int16_t f_toInt_r_minMag( float64_t a, bool exact, int16_t const * ) { int32_t i = f64_to_i32_r_minMag( a, exact );                                                                        \
		{ if( i > std::numeric_limits<int16_t>::max() ) { i = std::numeric_limits<int16_t>::max(); raiseFlags(flag_invalid); }                                                                          \
		else if( i < std::numeric_limits<int16_t>::min() ) { i = std::numeric_limits<int16_t>::min(); raiseFlags(flag_invalid); } } return i; }                                                        \
	static int32_t f_toInt_r_minMag( float16_t a, bool exact, int32_t const * ) { return f16_to_i32_r_minMag( a, exact ); }                                                                           \
	static int32_t f_toInt_r_minMag( float32_t a, bool exact, int32_t const * ) { return f32_to_i32_r_minMag( a, exact ); }                                                                           \
	static int32_t f_toInt_r_minMag( float64_t a, bool exact, int32_t const * ) { return f64_to_i32_r_minMag( a, exact ); }                                                                           \
	static int64_t f_toInt_r_minMag( float16_t a, bool exact, int64_t const * ) { return f16_to_i64_r_minMag( a, exact ); }                                                                           \
	static int64_t f_toInt_r_minMag( float32_t a, bool exact, int64_t const * ) { return f32_to_i64_r_minMag( a, exact ); }                                                                           \
	static int64_t f_toInt_r_minMag( float64_t a, bool exact, int64_t const * ) { return f64_to_i64_r_minMag( a, exact ); }                                                                           \
	static float16_t f_roundToInt( float16_t a, uint_fast8_t rmode, bool exact) { return f16_roundToInt( a, FromRoundingMode( rmode ), exact ); }                                                     \
	static float32_t f_roundToInt( float32_t a, uint_fast8_t rmode, bool exact) { return f32_roundToInt( a, FromRoundingMode( rmode ), exact ); }                                                     \
	static float64_t f_roundToInt( float64_t a, uint_fast8_t rmode, bool exact) { return f64_roundToInt( a, FromRoundingMode( rmode ), exact ); }                                                     \
	static float16_t f_add( float16_t a, float16_t b ) { return f16_add( a, b ); }                                                                                                                    \
	static float32_t f_add( float32_t a, float32_t b ) { return f32_add( a, b ); }                                                                                                                    \
	static float64_t f_add( float64_t a, float64_t b ) { return f64_add( a, b ); }                                                                                                                    \
	static float16_t f_sub( float16_t a, float16_t b ) { return f16_sub( a, b ); }                                                                                                                    \
	static float32_t f_sub( float32_t a, float32_t b ) { return f32_sub( a, b ); }                                                                                                                    \
	static float64_t f_sub( float64_t a, float64_t b ) { return f64_sub( a, b ); }                                                                                                                    \
	static float16_t f_mul( float16_t a, float16_t b ) { return f16_mul( a, b ); }                                                                                                                    \
	static float32_t f_mul( float32_t a, float32_t b ) { return f32_mul( a, b ); }                                                                                                                    \
	static float64_t f_mul( float64_t a, float64_t b ) { return f64_mul( a, b ); }                                                                                                                    \
	static float16_t f_mulAdd( float16_t a, float16_t b, float16_t c ) { return f16_mulAdd( a, b, c ); }                                                                                              \
	static float32_t f_mulAdd( float32_t a, float32_t b, float32_t c ) { return f32_mulAdd( a, b, c ); }                                                                                              \
	static float64_t f_mulAdd( float64_t a, float64_t b, float64_t c ) { return f64_mulAdd( a, b, c ); }                                                                                              \
	static float16_t f_div( float16_t a, float16_t b ) { return f16_div( a, b ); }                                                                                                                    \
	static float32_t f_div( float32_t a, float32_t b ) { return f32_div( a, b ); }                                                                                                                    \
	static float64_t f_div( float64_t a, float64_t b ) { return f64_div( a, b ); }                                                                                                                    \
	static float16_t f_rem( float16_t a, float16_t b ) { return f16_rem( a, b ); }                                                                                                                    \
	static float32_t f_rem( float32_t a, float32_t b ) { return f32_rem( a, b ); }                                                                                                                    \
	static float64_t f_rem( float64_t a, float64_t b ) { return f64_rem( a, b ); }                                                                                                                    \
	static float32_t f_sqrt( float32_t a ) { return f32_sqrt( a ); }                                                                                                                                  \
	static float64_t f_sqrt( float64_t a ) { return f64_sqrt( a ); }                                                                                                                                  \
	static bool f_lt( float16_t a, float16_t b) { return f16_lt( a, b ); }                                                                                                                            \
	static bool f_le( float16_t a, float16_t b) { return f16_le( a, b ); }                                                                                                                            \
	static bool f_eq( float16_t a, float16_t b) { return f16_eq( a, b ); }                                                                                                                            \
	static bool f_lt_quiet( float16_t a, float16_t b) { return f16_lt_quiet( a, b ); }                                                                                                                \
	static bool f_le_quiet( float16_t a, float16_t b) { return f16_le_quiet( a, b ); }                                                                                                                \
	static bool f_eq_signaling( float16_t a, float16_t b ) { return f16_eq_signaling( a, b ); }                                                                                                       \
	static bool f_isSignalingNaN( float16_t a ) { return f16_isSignalingNaN( a ); }                                                                                                                   \
	static bool f_lt( float32_t a, float32_t b) { return f32_lt( a, b ); }                                                                                                                            \
	static bool f_le( float32_t a, float32_t b) { return f32_le( a, b ); }                                                                                                                            \
	static bool f_eq( float32_t a, float32_t b) { return f32_eq( a, b ); }                                                                                                                            \
	static bool f_lt_quiet( float32_t a, float32_t b) { return f32_lt_quiet( a, b ); }                                                                                                                \
	static bool f_le_quiet( float32_t a, float32_t b) { return f32_le_quiet( a, b ); }                                                                                                                \
	static bool f_eq_signaling( float32_t a, float32_t b ) { return f32_eq_signaling( a, b ); }                                                                                                       \
	static bool f_isSignalingNaN( float32_t a ) { return f32_isSignalingNaN( a ); }                                                                                                                   \
	static bool f_lt( float64_t a, float64_t b) { return f64_lt( a, b ); }                                                                                                                            \
	static bool f_le( float64_t a, float64_t b) { return f64_le( a, b ); }                                                                                                                            \
	static bool f_eq( float64_t a, float64_t b) { return f64_eq( a, b ); }                                                                                                                            \
	static bool f_lt_quiet( float64_t a, float64_t b) { return f64_lt_quiet( a, b ); }                                                                                                                \
	static bool f_le_quiet( float64_t a, float64_t b) { return f64_le_quiet( a, b ); }                                                                                                                \
	static bool f_eq_signaling( float64_t a, float64_t b ) { return f64_eq_signaling( a, b ); }                                                                                                       \
	static bool f_isSignalingNaN( float64_t a ) { return f64_isSignalingNaN( a ); }                                                                                                                   \
	                                                                                                                                                                                                  \
protected:                                                                                                                                                                                          \
	SOFTFLOAT_EMU_DEF_ARCH_IF_FUNCTIONS(ARCH_NAME)                                                                                                                                                    \
	SOFTFLOAT_EMU_DEF_ARCH_IF_FUNCTIONS_FAST_INT64(ARCH_NAME)                                                                                                                                         \
};

SOFTFLOAT_EMU_DEF_ARCH_IF( x86_base_if         , x86          )
SOFTFLOAT_EMU_DEF_ARCH_IF( x86_sse_base_if     , x86_sse      )
SOFTFLOAT_EMU_DEF_ARCH_IF( arm_vfpv2_base_if   , arm_vfpv2    )
SOFTFLOAT_EMU_DEF_ARCH_IF( arm_vfpv2_dn_base_if, arm_vfpv2_dn )
SOFTFLOAT_EMU_DEF_ARCH_IF( riscv_base_if       , riscv        )

#undef SOFTFLOAT_EMU_DEF_ARCH_EXT_IF
#define SOFTFLOAT_EMU_DEF_ARCH_EXT_IF( IF_NAME, BASE_IF_NAME, DEFAULT_NAN )                                                                                                                                      \
struct IF_NAME : BASE_IF_NAME                                                                                                                                                                       \
{                                                                                                                                                                                                   \
	/*----------------------------------------------------------------------------                                                                                                                    \
	| Default NaN switching                                                                                                                                                                           \
	*----------------------------------------------------------------------------*/                                                                                                                   \
                                                                                                                                                                                                    \
	static void defaultNaN( bool dn ) { }                                                                                                                                                             \
	static bool defaultNaN() { return DEFAULT_NAN; }                                                                                                                                                  \
	                                                                                                                                                                                                  \
	using BASE_IF_NAME::f_construct;                                                                                                                                                                  \
	using BASE_IF_NAME::f_sqrt;                                                                                                                                                                       \
	static float16_t f_sqrt( float16_t a ) { return f32_to_f16( f32_sqrt( f16_to_f32( a ) ) ); }                                                                                                      \
	static bool f_ne( float16_t a, float16_t b) { return !f16_eq( a, b ); }                                                                                                                           \
	static bool f_ge( float16_t a, float16_t b) { return !f16_lt( a, b ); }                                                                                                                           \
	static bool f_gt( float16_t a, float16_t b) { return !f16_le( a, b ); }                                                                                                                           \
	static bool f_ge_quiet( float16_t a, float16_t b) { return !f16_lt_quiet( a, b ); }                                                                                                               \
	static bool f_gt_quiet( float16_t a, float16_t b) { return !f16_le_quiet( a, b ); }                                                                                                               \
	static float16_t f_clearSignaling( float16_t a ) { float16_t z = { uint16_t(a.v | FloatingPointTypeInfo<float16_t>::quiet_mask) }; return z; }                                                    \
	static float16_t f_defaultNaN( float16_t const * ) { float16_t z = { uint16_t( FloatingPointTypeInfo<float16_t>::exp_mask | FloatingPointTypeInfo<float16_t>::quiet_mask ) }; return z; }         \
	static bool f_ne( float32_t a, float32_t b) { return !f32_eq( a, b ); }                                                                                                                           \
	static bool f_ge( float32_t a, float32_t b) { return !f32_lt( a, b ); }                                                                                                                           \
	static bool f_gt( float32_t a, float32_t b) { return !f32_le( a, b ); }                                                                                                                           \
	static bool f_ge_quiet( float32_t a, float32_t b) { return !f32_lt_quiet( a, b ); }                                                                                                               \
	static bool f_gt_quiet( float32_t a, float32_t b) { return !f32_le_quiet( a, b ); }                                                                                                               \
	static float32_t f_clearSignaling( float32_t a ) { float32_t z = { uint32_t( a.v | FloatingPointTypeInfo<float32_t>::quiet_mask ) }; return z; }                                                  \
	static float32_t f_defaultNaN( float32_t const * ) { float32_t z = { uint32_t( FloatingPointTypeInfo<float32_t>::exp_mask | FloatingPointTypeInfo<float32_t>::quiet_mask ) }; return z; }         \
	static bool f_ne( float64_t a, float64_t b) { return !f64_eq( a, b ); }                                                                                                                           \
	static bool f_ge( float64_t a, float64_t b) { return !f64_lt( a, b ); }                                                                                                                           \
	static bool f_gt( float64_t a, float64_t b) { return !f64_le( a, b ); }                                                                                                                           \
	static bool f_ge_quiet( float64_t a, float64_t b) { return !f64_lt_quiet( a, b ); }                                                                                                               \
	static bool f_gt_quiet( float64_t a, float64_t b) { return !f64_le_quiet( a, b ); }                                                                                                               \
	static float64_t f_clearSignaling( float64_t a ) { float64_t z = { uint64_t(a.v | FloatingPointTypeInfo<float64_t>::quiet_mask) }; return z; }                                                    \
	static float64_t f_defaultNaN( float64_t const * ) { float64_t z = { uint64_t( FloatingPointTypeInfo<float64_t>::exp_mask | FloatingPointTypeInfo<float64_t>::quiet_mask ) }; return z; }         \
};

SOFTFLOAT_EMU_DEF_ARCH_EXT_IF( x86_ext_if     , x86_base_if         , false )
SOFTFLOAT_EMU_DEF_ARCH_EXT_IF( x86_sse_ext_if , x86_sse_base_if     , false )
SOFTFLOAT_EMU_DEF_ARCH_EXT_IF( arm_vfpv2_if   , arm_vfpv2_base_if   , false )
SOFTFLOAT_EMU_DEF_ARCH_EXT_IF( arm_vfpv2_dn_if, arm_vfpv2_dn_base_if, true  )
SOFTFLOAT_EMU_DEF_ARCH_EXT_IF( riscv_if       , riscv_base_if       , false )

struct x86_if : x86_ext_if
{
	static float16_t f_defaultNaN( float16_t const * ) { float16_t z = { uint16_t(FloatingPointTypeInfo<float16_t>::sign_mask | FloatingPointTypeInfo<float16_t>::exp_mask | FloatingPointTypeInfo<float16_t>::quiet_mask) }; return z; }
	static float32_t f_defaultNaN( float32_t const * ) { float32_t z = { uint32_t(FloatingPointTypeInfo<float32_t>::sign_mask | FloatingPointTypeInfo<float32_t>::exp_mask | FloatingPointTypeInfo<float32_t>::quiet_mask) }; return z; }
	static float64_t f_defaultNaN( float64_t const * ) { float64_t z = { uint64_t(FloatingPointTypeInfo<float64_t>::sign_mask | FloatingPointTypeInfo<float64_t>::exp_mask | FloatingPointTypeInfo<float64_t>::quiet_mask) }; return z; }
};

struct x86_sse_if : x86_sse_ext_if
{
	static float16_t f_defaultNaN( float16_t const * ) { float16_t z = { uint16_t(FloatingPointTypeInfo<float16_t>::sign_mask | FloatingPointTypeInfo<float16_t>::exp_mask | FloatingPointTypeInfo<float16_t>::quiet_mask) }; return z; }
	static float32_t f_defaultNaN( float32_t const * ) { float32_t z = { uint32_t(FloatingPointTypeInfo<float32_t>::sign_mask | FloatingPointTypeInfo<float32_t>::exp_mask | FloatingPointTypeInfo<float32_t>::quiet_mask) }; return z; }
	static float64_t f_defaultNaN( float64_t const * ) { float64_t z = { uint64_t(FloatingPointTypeInfo<float64_t>::sign_mask | FloatingPointTypeInfo<float64_t>::exp_mask | FloatingPointTypeInfo<float64_t>::quiet_mask) }; return z; }
};

template <typename IF1, typename IF2>
struct DynamicDefaultNaNArchIf
{
	/*----------------------------------------------------------------------------
	| Software floating-point underflow tininess-detection mode.
	*----------------------------------------------------------------------------*/
	
	static void detectTininess( uint_fast8_t dt ) { if( default_nan ) IF2::detectTininess( dt ); else IF1::detectTininess( dt ); }
	static uint_fast8_t detectTininess() { return default_nan ? IF2::detectTininess() : IF1::detectTininess(); }
	
	/*----------------------------------------------------------------------------
	| Software floating-point rounding mode.  (Mode "odd" is supported only if
	| SoftFloat is compiled with macro 'SOFTFLOAT_ROUND_ODD' defined.)
	*----------------------------------------------------------------------------*/
	
	static void roundingMode( uint_fast8_t rmode ) { if( default_nan ) IF2::roundingMode( rmode ); else IF1::roundingMode( rmode ); }
	static uint_fast8_t roundingMode() { return default_nan ? IF2::roundingMode() : IF1::roundingMode(); }
	
	/*----------------------------------------------------------------------------
	| Software floating-point exception flags.
	*----------------------------------------------------------------------------*/
	
	static void exceptionFlags( uint_fast8_t eflags ) { if( default_nan ) IF2::exceptionFlags( eflags ); else IF1::exceptionFlags( eflags ); }
	static uint_fast8_t exceptionFlags() { return default_nan ? IF2::exceptionFlags() : IF1::exceptionFlags(); }
	
	/*----------------------------------------------------------------------------
	| Rounding precision for 80-bit extended double-precision floating-point.
	| Valid values are 32, 64, and 80.
	*----------------------------------------------------------------------------*/
	
	static void extF80_roundingPrecision( uint_fast8_t ext80_rp ) { if( default_nan ) IF2::extF80_roundingPrecision( ext80_rp ); else IF1::extF80_roundingPrecision( ext80_rp ); }
	static uint_fast8_t extF80_roundingPrecision() { return default_nan ? IF2::extF80_roundingPrecision() : IF1::extF80_roundingPrecision(); }
	
	/*----------------------------------------------------------------------------
	| Software floating-point flush-to-zero mode.
	*----------------------------------------------------------------------------*/
	
	static void flushToZero( uint_fast8_t ftz ) { if( default_nan ) IF2::flushToZero( ftz ); else IF1::flushToZero( ftz ); }
	static uint_fast8_t flushToZero() { return default_nan ? IF2::flushToZero() : IF1::flushToZero(); }
	
	/*----------------------------------------------------------------------------
	| Default NaN switching
	*----------------------------------------------------------------------------*/
  
	static void defaultNaN( bool dn ) { default_nan = dn; }
	static bool defaultNaN() { return default_nan; }
	
	/*----------------------------------------------------------------------------
	| C++ friendly overloaded routines.
	*----------------------------------------------------------------------------*/
	
	static float16_t f_construct( uint32_t a, float16_t const * ) { return default_nan ? IF2::f_construct( a, (float16_t const *) 0 ) : IF1::f_construct( a, (float16_t const *) 0 ); }
	static float32_t f_construct( uint32_t a, float32_t const * ) { return default_nan ? IF2::f_construct( a, (float32_t const *) 0 ) : IF1::f_construct( a, (float32_t const *) 0 ); }
	static float64_t f_construct( uint32_t a, float64_t const * ) { return default_nan ? IF2::f_construct( a, (float64_t const *) 0 ) : IF1::f_construct( a, (float64_t const *) 0 ); }
	static float16_t f_construct( uint64_t a, float16_t const * ) { return default_nan ? IF2::f_construct( a, (float16_t const *) 0 ) : IF1::f_construct( a, (float16_t const *) 0 ); }
	static float32_t f_construct( uint64_t a, float32_t const * ) { return default_nan ? IF2::f_construct( a, (float32_t const *) 0 ) : IF1::f_construct( a, (float32_t const *) 0 ); }
	static float64_t f_construct( uint64_t a, float64_t const * ) { return default_nan ? IF2::f_construct( a, (float64_t const *) 0 ) : IF1::f_construct( a, (float64_t const *) 0 ); }
	static float16_t f_construct( int32_t a, float16_t const * ) { return default_nan ? IF2::f_construct( a, (float16_t const *) 0 ) : IF1::f_construct( a, (float16_t const *) 0 ); }
	static float32_t f_construct( int32_t a, float32_t const * ) { return default_nan ? IF2::f_construct( a, (float32_t const *) 0 ) : IF1::f_construct( a, (float32_t const *) 0 ); }
	static float64_t f_construct( int32_t a, float64_t const * ) { return default_nan ? IF2::f_construct( a, (float64_t const *) 0 ) : IF1::f_construct( a, (float64_t const *) 0 ); }
	static float16_t f_construct( int64_t a, float16_t const * ) { return default_nan ? IF2::f_construct( a, (float16_t const *) 0 ) : IF1::f_construct( a, (float16_t const *) 0 ); }
	static float32_t f_construct( int64_t a, float32_t const * ) { return default_nan ? IF2::f_construct( a, (float32_t const *) 0 ) : IF1::f_construct( a, (float32_t const *) 0 ); }
	static float64_t f_construct( int64_t a, float64_t const * ) { return default_nan ? IF2::f_construct( a, (float64_t const *) 0 ) : IF1::f_construct( a, (float64_t const *) 0 ); }
	static float16_t f_construct( float16_t a, float16_t const * ) { return a; }
	static float32_t f_construct( float16_t a, float32_t const * ) { return default_nan ? IF2::f_construct( a, (float32_t const *) 0 ) : IF1::f_construct( a, (float32_t const *) 0 ); }
	static float64_t f_construct( float16_t a, float64_t const * ) { return default_nan ? IF2::f_construct( a, (float64_t const *) 0 ) : IF1::f_construct( a, (float64_t const *) 0 ); }
	static float16_t f_construct( float32_t a, float16_t const * ) { return default_nan ? IF2::f_construct( a, (float16_t const *) 0 ) : IF1::f_construct( a, (float16_t const *) 0 ); }
	static float32_t f_construct( float32_t a, float32_t const * ) { return a; }
	static float64_t f_construct( float32_t a, float64_t const * ) { return default_nan ? IF2::f_construct( a, (float64_t const *) 0 ) : IF1::f_construct( a, (float64_t const *) 0 ); }
	static float16_t f_construct( float64_t a, float16_t const * ) { return default_nan ? IF2::f_construct( a, (float16_t const *) 0 ) : IF1::f_construct( a, (float16_t const *) 0 ); }
	static float32_t f_construct( float64_t a, float32_t const * ) { return default_nan ? IF2::f_construct( a, (float32_t const *) 0 ) : IF1::f_construct( a, (float32_t const *) 0 ); }
	static float64_t f_construct( float64_t a, float64_t const * ) { return a; }
	static uint16_t f_toInt( float16_t a, uint_fast8_t rmode, bool exact, uint16_t const * ) { return default_nan ? IF2::f_toInt( a, FromRoundingMode( rmode ), exact, (uint16_t const *) 0 ) : IF1::f_toInt( a, FromRoundingMode( rmode ), exact, (uint16_t const *) 0 ); }
	static uint16_t f_toInt( float32_t a, uint_fast8_t rmode, bool exact, uint16_t const * ) { return default_nan ? IF2::f_toInt( a, FromRoundingMode( rmode ), exact, (uint16_t const *) 0 ) : IF1::f_toInt( a, FromRoundingMode( rmode ), exact, (uint16_t const *) 0 ); }
	static uint16_t f_toInt( float64_t a, uint_fast8_t rmode, bool exact, uint16_t const * ) { return default_nan ? IF2::f_toInt( a, FromRoundingMode( rmode ), exact, (uint16_t const *) 0 ) : IF1::f_toInt( a, FromRoundingMode( rmode ), exact, (uint16_t const *) 0 ); }
	static uint32_t f_toInt( float16_t a, uint_fast8_t rmode, bool exact, uint32_t const * ) { return default_nan ? IF2::f_toInt( a, FromRoundingMode( rmode ), exact, (uint32_t const *) 0 ) : IF1::f_toInt( a, FromRoundingMode( rmode ), exact, (uint32_t const *) 0 ); }
	static uint32_t f_toInt( float32_t a, uint_fast8_t rmode, bool exact, uint32_t const * ) { return default_nan ? IF2::f_toInt( a, FromRoundingMode( rmode ), exact, (uint32_t const *) 0 ) : IF1::f_toInt( a, FromRoundingMode( rmode ), exact, (uint32_t const *) 0 ); }
	static uint32_t f_toInt( float64_t a, uint_fast8_t rmode, bool exact, uint32_t const * ) { return default_nan ? IF2::f_toInt( a, FromRoundingMode( rmode ), exact, (uint32_t const *) 0 ) : IF1::f_toInt( a, FromRoundingMode( rmode ), exact, (uint32_t const *) 0 ); }
	static uint64_t f_toInt( float16_t a, uint_fast8_t rmode, bool exact, uint64_t const * ) { return default_nan ? IF2::f_toInt( a, FromRoundingMode( rmode ), exact, (uint64_t const *) 0 ) : IF1::f_toInt( a, FromRoundingMode( rmode ), exact, (uint64_t const *) 0 ); }
	static uint64_t f_toInt( float32_t a, uint_fast8_t rmode, bool exact, uint64_t const * ) { return default_nan ? IF2::f_toInt( a, FromRoundingMode( rmode ), exact, (uint64_t const *) 0 ) : IF1::f_toInt( a, FromRoundingMode( rmode ), exact, (uint64_t const *) 0 ); }
	static uint64_t f_toInt( float64_t a, uint_fast8_t rmode, bool exact, uint64_t const * ) { return default_nan ? IF2::f_toInt( a, FromRoundingMode( rmode ), exact, (uint64_t const *) 0 ) : IF1::f_toInt( a, FromRoundingMode( rmode ), exact, (uint64_t const *) 0 ); }
	static int16_t f_toInt( float16_t a, uint_fast8_t rmode, bool exact, int16_t const * ) { return default_nan ? IF2::f_toInt( a, FromRoundingMode( rmode ), exact, (int16_t const *) 0 ) : IF1::f_toInt( a, FromRoundingMode( rmode ), exact, (int16_t const *) 0 ); }
	static int16_t f_toInt( float32_t a, uint_fast8_t rmode, bool exact, int16_t const * ) { return default_nan ? IF2::f_toInt( a, FromRoundingMode( rmode ), exact, (int16_t const *) 0 ) : IF1::f_toInt( a, FromRoundingMode( rmode ), exact, (int16_t const *) 0 ); }
	static int16_t f_toInt( float64_t a, uint_fast8_t rmode, bool exact, int16_t const * ) { return default_nan ? IF2::f_toInt( a, FromRoundingMode( rmode ), exact, (int16_t const *) 0 ) : IF1::f_toInt( a, FromRoundingMode( rmode ), exact, (int16_t const *) 0 ); }
	static int32_t f_toInt( float16_t a, uint_fast8_t rmode, bool exact, int32_t const * ) { return default_nan ? IF2::f_toInt( a, FromRoundingMode( rmode ), exact, (int32_t const *) 0 ) : IF1::f_toInt( a, FromRoundingMode( rmode ), exact, (int32_t const *) 0 ); }
	static int32_t f_toInt( float32_t a, uint_fast8_t rmode, bool exact, int32_t const * ) { return default_nan ? IF2::f_toInt( a, FromRoundingMode( rmode ), exact, (int32_t const *) 0 ) : IF1::f_toInt( a, FromRoundingMode( rmode ), exact, (int32_t const *) 0 ); }
	static int32_t f_toInt( float64_t a, uint_fast8_t rmode, bool exact, int32_t const * ) { return default_nan ? IF2::f_toInt( a, FromRoundingMode( rmode ), exact, (int32_t const *) 0 ) : IF1::f_toInt( a, FromRoundingMode( rmode ), exact, (int32_t const *) 0 ); }
	static int64_t f_toInt( float16_t a, uint_fast8_t rmode, bool exact, int64_t const * ) { return default_nan ? IF2::f_toInt( a, FromRoundingMode( rmode ), exact, (int64_t const *) 0 ) : IF1::f_toInt( a, FromRoundingMode( rmode ), exact, (int64_t const *) 0 ); }
	static int64_t f_toInt( float32_t a, uint_fast8_t rmode, bool exact, int64_t const * ) { return default_nan ? IF2::f_toInt( a, FromRoundingMode( rmode ), exact, (int64_t const *) 0 ) : IF1::f_toInt( a, FromRoundingMode( rmode ), exact, (int64_t const *) 0 ); }
	static int64_t f_toInt( float64_t a, uint_fast8_t rmode, bool exact, int64_t const * ) { return default_nan ? IF2::f_toInt( a, FromRoundingMode( rmode ), exact, (int64_t const *) 0 ) : IF1::f_toInt( a, FromRoundingMode( rmode ), exact, (int64_t const *) 0 ); }
	static uint16_t f_toInt_r_minMag( float16_t a, bool exact, uint16_t const * ) { return default_nan ? IF2::f_toInt_r_minMag( a, exact, (uint16_t const *) 0 ) : IF1::f_toInt_r_minMag( a, exact, (uint16_t const *) 0 ); }
	static uint16_t f_toInt_r_minMag( float32_t a, bool exact, uint16_t const * ) { return default_nan ? IF2::f_toInt_r_minMag( a, exact, (uint16_t const *) 0 ) : IF1::f_toInt_r_minMag( a, exact, (uint16_t const *) 0 ); }
	static uint16_t f_toInt_r_minMag( float64_t a, bool exact, uint16_t const * ) { return default_nan ? IF2::f_toInt_r_minMag( a, exact, (uint16_t const *) 0 ) : IF1::f_toInt_r_minMag( a, exact, (uint16_t const *) 0 ); }
	static uint32_t f_toInt_r_minMag( float16_t a, bool exact, uint32_t const * ) { return default_nan ? IF2::f_toInt_r_minMag( a, exact, (uint32_t const *) 0 ) : IF1::f_toInt_r_minMag( a, exact, (uint32_t const *) 0 ); }
	static uint32_t f_toInt_r_minMag( float32_t a, bool exact, uint32_t const * ) { return default_nan ? IF2::f_toInt_r_minMag( a, exact, (uint32_t const *) 0 ) : IF1::f_toInt_r_minMag( a, exact, (uint32_t const *) 0 ); }
	static uint32_t f_toInt_r_minMag( float64_t a, bool exact, uint32_t const * ) { return default_nan ? IF2::f_toInt_r_minMag( a, exact, (uint32_t const *) 0 ) : IF1::f_toInt_r_minMag( a, exact, (uint32_t const *) 0 ); }
	static uint64_t f_toInt_r_minMag( float16_t a, bool exact, uint64_t const * ) { return default_nan ? IF2::f_toInt_r_minMag( a, exact, (uint64_t const *) 0 ) : IF1::f_toInt_r_minMag( a, exact, (uint64_t const *) 0 ); }
	static uint64_t f_toInt_r_minMag( float32_t a, bool exact, uint64_t const * ) { return default_nan ? IF2::f_toInt_r_minMag( a, exact, (uint64_t const *) 0 ) : IF1::f_toInt_r_minMag( a, exact, (uint64_t const *) 0 ); }
	static uint64_t f_toInt_r_minMag( float64_t a, bool exact, uint64_t const * ) { return default_nan ? IF2::f_toInt_r_minMag( a, exact, (uint64_t const *) 0 ) : IF1::f_toInt_r_minMag( a, exact, (uint64_t const *) 0 ); }
	static int16_t f_toInt_r_minMag( float16_t a, bool exact, int16_t const * ) { return default_nan ? IF2::f_toInt_r_minMag( a, exact, (int16_t const *) 0 ) : IF1::f_toInt_r_minMag( a, exact, (int16_t const *) 0 ); }
	static int16_t f_toInt_r_minMag( float32_t a, bool exact, int16_t const * ) { return default_nan ? IF2::f_toInt_r_minMag( a, exact, (int16_t const *) 0 ) : IF1::f_toInt_r_minMag( a, exact, (int16_t const *) 0 ); }
	static int16_t f_toInt_r_minMag( float64_t a, bool exact, int16_t const * ) { return default_nan ? IF2::f_toInt_r_minMag( a, exact, (int16_t const *) 0 ) : IF1::f_toInt_r_minMag( a, exact, (int16_t const *) 0 ); }
	static int32_t f_toInt_r_minMag( float16_t a, bool exact, int32_t const * ) { return default_nan ? IF2::f_toInt_r_minMag( a, exact, (int32_t const *) 0 ) : IF1::f_toInt_r_minMag( a, exact, (int32_t const *) 0 ); }
	static int32_t f_toInt_r_minMag( float32_t a, bool exact, int32_t const * ) { return default_nan ? IF2::f_toInt_r_minMag( a, exact, (int32_t const *) 0 ) : IF1::f_toInt_r_minMag( a, exact, (int32_t const *) 0 ); }
	static int32_t f_toInt_r_minMag( float64_t a, bool exact, int32_t const * ) { return default_nan ? IF2::f_toInt_r_minMag( a, exact, (int32_t const *) 0 ) : IF1::f_toInt_r_minMag( a, exact, (int32_t const *) 0 ); }
	static int64_t f_toInt_r_minMag( float16_t a, bool exact, int64_t const * ) { return default_nan ? IF2::f_toInt_r_minMag( a, exact, (int64_t const *) 0 ) : IF1::f_toInt_r_minMag( a, exact, (int64_t const *) 0 ); }
	static int64_t f_toInt_r_minMag( float32_t a, bool exact, int64_t const * ) { return default_nan ? IF2::f_toInt_r_minMag( a, exact, (int64_t const *) 0 ) : IF1::f_toInt_r_minMag( a, exact, (int64_t const *) 0 ); }
	static int64_t f_toInt_r_minMag( float64_t a, bool exact, int64_t const * ) { return default_nan ? IF2::f_toInt_r_minMag( a, exact, (int64_t const *) 0 ) : IF1::f_toInt_r_minMag( a, exact, (int64_t const *) 0 ); }
	static float16_t f_roundToInt( float16_t a, uint_fast8_t rmode, bool exact) { return default_nan ? IF2::f_roundToInt( a, FromRoundingMode( rmode ), exact ) : IF1::f_roundToInt( a, FromRoundingMode( rmode ), exact ); }
	static float32_t f_roundToInt( float32_t a, uint_fast8_t rmode, bool exact) { return default_nan ? IF2::f_roundToInt( a, FromRoundingMode( rmode ), exact ) : IF1::f_roundToInt( a, FromRoundingMode( rmode ), exact ); }
	static float64_t f_roundToInt( float64_t a, uint_fast8_t rmode, bool exact) { return default_nan ? IF2::f_roundToInt( a, FromRoundingMode( rmode ), exact ) : IF1::f_roundToInt( a, FromRoundingMode( rmode ), exact ); }
	static float16_t f_add( float16_t a, float16_t b ) { return default_nan ? IF2::f_add( a, b ) : IF1::f_add( a, b ); }
	static float32_t f_add( float32_t a, float32_t b ) { return default_nan ? IF2::f_add( a, b ) : IF1::f_add( a, b ); }
	static float64_t f_add( float64_t a, float64_t b ) { return default_nan ? IF2::f_add( a, b ) : IF1::f_add( a, b ); }
	static float16_t f_sub( float16_t a, float16_t b ) { return default_nan ? IF2::f_sub( a, b ) : IF1::f_sub( a, b ); }
	static float32_t f_sub( float32_t a, float32_t b ) { return default_nan ? IF2::f_sub( a, b ) : IF1::f_sub( a, b ); }
	static float64_t f_sub( float64_t a, float64_t b ) { return default_nan ? IF2::f_sub( a, b ) : IF1::f_sub( a, b ); }
	static float16_t f_mul( float16_t a, float16_t b ) { return default_nan ? IF2::f_mul( a, b ) : IF1::f_mul( a, b ); }
	static float32_t f_mul( float32_t a, float32_t b ) { return default_nan ? IF2::f_mul( a, b ) : IF1::f_mul( a, b ); }
	static float64_t f_mul( float64_t a, float64_t b ) { return default_nan ? IF2::f_mul( a, b ) : IF1::f_mul( a, b ); }
	static float16_t f_mulAdd( float16_t a, float16_t b, float16_t c ) { return default_nan ? IF2::f_mulAdd( a, b, c ) : IF1::f_mulAdd( a, b, c ); }
	static float32_t f_mulAdd( float32_t a, float32_t b, float32_t c ) { return default_nan ? IF2::f_mulAdd( a, b, c ) : IF1::f_mulAdd( a, b, c ); }
	static float64_t f_mulAdd( float64_t a, float64_t b, float64_t c ) { return default_nan ? IF2::f_mulAdd( a, b, c ) : IF1::f_mulAdd( a, b, c ); }
	static float16_t f_div( float16_t a, float16_t b ) { return default_nan ? IF2::f_div( a, b ) : IF1::f_div( a, b ); }
	static float32_t f_div( float32_t a, float32_t b ) { return default_nan ? IF2::f_div( a, b ) : IF1::f_div( a, b ); }
	static float64_t f_div( float64_t a, float64_t b ) { return default_nan ? IF2::f_div( a, b ) : IF1::f_div( a, b ); }
	static float16_t f_rem( float16_t a, float16_t b ) { return default_nan ? IF2::f_rem( a, b ) : IF1::f_rem( a, b ); }
	static float32_t f_rem( float32_t a, float32_t b ) { return default_nan ? IF2::f_rem( a, b ) : IF1::f_rem( a, b ); }
	static float64_t f_rem( float64_t a, float64_t b ) { return default_nan ? IF2::f_rem( a, b ) : IF1::f_rem( a, b ); }
	static float16_t f_sqrt( float16_t a ) { return default_nan ? IF2::f_sqrt( a ) : IF1::f_sqrt( a ); }
	static float32_t f_sqrt( float32_t a ) { return default_nan ? IF2::f_sqrt( a ) : IF1::f_sqrt( a ); }
	static float64_t f_sqrt( float64_t a ) { return default_nan ? IF2::f_sqrt( a ) : IF1::f_sqrt( a ); }
	static bool f_lt( float16_t a, float16_t b) { return default_nan ? IF2::f_lt( a, b ) : IF1::f_lt( a, b ); }
	static bool f_le( float16_t a, float16_t b) { return default_nan ? IF2::f_le( a, b ) : IF1::f_le( a, b ); }
	static bool f_eq( float16_t a, float16_t b) { return default_nan ? IF2::f_eq( a, b ) : IF1::f_eq( a, b ); }
	static bool f_ne( float16_t a, float16_t b) { return default_nan ? IF2::f_ne( a, b ) : IF1::f_ne( a, b ); }
	static bool f_ge( float16_t a, float16_t b) { return default_nan ? IF2::f_ge( a, b ) : IF1::f_ge( a, b ); }
	static bool f_gt( float16_t a, float16_t b) { return default_nan ? IF2::f_gt( a, b ) : IF1::f_gt( a, b ); }
	static bool f_lt_quiet( float16_t a, float16_t b) { return default_nan ? IF2::f_lt_quiet( a, b ) : IF1::f_lt_quiet( a, b ); }
	static bool f_le_quiet( float16_t a, float16_t b) { return default_nan ? IF2::f_le_quiet( a, b ) : IF1::f_le_quiet( a, b ); }
	static bool f_ge_quiet( float16_t a, float16_t b) { return default_nan ? IF2::f_ge_quiet( a, b ) : IF1::f_ge_quiet( a, b ); }
	static bool f_gt_quiet( float16_t a, float16_t b) { return default_nan ? IF2::f_gt_quiet( a, b ) : IF1::f_gt_quiet( a, b ); }
	static bool f_eq_signaling( float16_t a, float16_t b ) { return default_nan ? IF2::f_eq_signaling( a, b ) : IF1::f_eq_signaling( a, b ); }
	static bool f_isSignalingNaN( float16_t a ) { return default_nan ? IF2::f_isSignalingNaN( a ) : IF1::f_isSignalingNaN( a ); }
	static float16_t f_clearSignaling( float16_t a ) { return default_nan ? IF2::f_clearSignaling( a ) : IF1::f_clearSignaling( a ); }
	static float16_t f_defaultNaN( float16_t const * ) { return default_nan ? IF2::f_defaultNaN( (float16_t const *) 0 ) : IF1::f_defaultNaN( (float16_t const *) 0 ); }
	static bool f_lt( float32_t a, float32_t b) { return default_nan ? IF2::f_lt( a, b ) : IF1::f_lt( a, b ); }
	static bool f_le( float32_t a, float32_t b) { return default_nan ? IF2::f_le( a, b ) : IF1::f_le( a, b ); }
	static bool f_eq( float32_t a, float32_t b) { return default_nan ? IF2::f_eq( a, b ) : IF1::f_eq( a, b ); }
	static bool f_ne( float32_t a, float32_t b) { return default_nan ? IF2::f_ne( a, b ) : IF1::f_ne( a, b ); }
	static bool f_ge( float32_t a, float32_t b) { return default_nan ? IF2::f_ge( a, b ) : IF1::f_ge( a, b ); }
	static bool f_gt( float32_t a, float32_t b) { return default_nan ? IF2::f_gt( a, b ) : IF1::f_gt( a, b ); }
	static bool f_lt_quiet( float32_t a, float32_t b) { return default_nan ? IF2::f_lt_quiet( a, b ) : IF1::f_lt_quiet( a, b ); }
	static bool f_le_quiet( float32_t a, float32_t b) { return default_nan ? IF2::f_le_quiet( a, b ) : IF1::f_le_quiet( a, b ); }
	static bool f_ge_quiet( float32_t a, float32_t b) { return default_nan ? IF2::f_ge_quiet( a, b ) : IF1::f_ge_quiet( a, b ); }
	static bool f_gt_quiet( float32_t a, float32_t b) { return default_nan ? IF2::f_gt_quiet( a, b ) : IF1::f_gt_quiet( a, b ); }
	static bool f_eq_signaling( float32_t a, float32_t b ) { return default_nan ? IF2::f_eq_signaling( a, b ) : IF1::f_eq_signaling( a, b ); }
	static bool f_isSignalingNaN( float32_t a ) { return default_nan ? IF2::f_isSignalingNaN( a ) : IF1::f_isSignalingNaN( a ); }
	static float32_t f_clearSignaling( float32_t a ) { return default_nan ? IF2::f_clearSignaling( a ) : IF1::f_clearSignaling( a ); }
	static float32_t f_defaultNaN( float32_t const * ) { return default_nan ? IF2::f_defaultNaN( (float32_t const *) 0 ) : IF1::f_defaultNaN( (float32_t const *) 0 ); }
	static bool f_lt( float64_t a, float64_t b) { return default_nan ? IF2::f_lt( a, b ) : IF1::f_lt( a, b ); }
	static bool f_le( float64_t a, float64_t b) { return default_nan ? IF2::f_le( a, b ) : IF1::f_le( a, b ); }
	static bool f_eq( float64_t a, float64_t b) { return default_nan ? IF2::f_eq( a, b ) : IF1::f_eq( a, b ); }
	static bool f_ne( float64_t a, float64_t b) { return default_nan ? IF2::f_ne( a, b ) : IF1::f_ne( a, b ); }
	static bool f_ge( float64_t a, float64_t b) { return default_nan ? IF2::f_ge( a, b ) : IF1::f_ge( a, b ); }
	static bool f_gt( float64_t a, float64_t b) { return default_nan ? IF2::f_gt( a, b ) : IF1::f_gt( a, b ); }
	static bool f_lt_quiet( float64_t a, float64_t b) { return default_nan ? IF2::f_lt_quiet( a, b ) : IF1::f_lt_quiet( a, b ); }
	static bool f_le_quiet( float64_t a, float64_t b) { return default_nan ? IF2::f_le_quiet( a, b ) : IF1::f_le_quiet( a, b ); }
	static bool f_ge_quiet( float64_t a, float64_t b) { return default_nan ? IF2::f_ge_quiet( a, b ) : IF1::f_ge_quiet( a, b ); }
	static bool f_gt_quiet( float64_t a, float64_t b) { return default_nan ? IF2::f_gt_quiet( a, b ) : IF1::f_gt_quiet( a, b ); }
	static bool f_eq_signaling( float64_t a, float64_t b ) { return default_nan ? IF2::f_eq_signaling( a, b ) : IF1::f_eq_signaling( a, b ); }
	static bool f_isSignalingNaN( float64_t a ) { return default_nan ? IF2::f_isSignalingNaN( a ) : IF1::f_isSignalingNaN( a ); }
	static float64_t f_clearSignaling( float64_t a ) { return default_nan ? IF2::f_clearSignaling( a ) : IF1::f_clearSignaling( a ); }
	static float64_t f_defaultNaN( float64_t const * ) { return default_nan ? IF2::f_defaultNaN( (float64_t const *) 0 ) : IF1::f_defaultNaN( (float64_t const *) 0 ); }
	
private:
	static bool default_nan;
};

template <typename IF1, typename IF2>
bool DynamicDefaultNaNArchIf<IF1, IF2>::default_nan = false;

typedef DynamicDefaultNaNArchIf<arm_vfpv2_if, arm_vfpv2_dn_if> arm_vfpv2_ddn_if;

// Generic Floating-point type (C++ wrapper class for float16_t, float32_t, and float64_t types).
template <typename IF, typename T>
struct Float
{
	typedef Float<IF, T> this_type;
	typedef T value_type;
	typedef IF interface_type;
	
	__UNISIM_FP_CONSTEXPR Float() : value() {}
	template <typename OTHER_T> explicit __UNISIM_FP_CONSTEXPR Float( Float<IF, OTHER_T> const& other ) : value( IF::f_construct( other.value, (value_type const *) 0 ) ) {}
	Float(uint16_t other) : value( IF::f_construct( other, (value_type const *) 0 ) ) {}
	Float(uint32_t other) : value( IF::f_construct( other, (value_type const *) 0 ) ) {}
	Float(uint64_t other) : value( IF::f_construct( other, (value_type const *) 0 ) ) {}
	Float(int16_t other) : value( IF::f_construct( other, (value_type const *) 0 ) ) {}
	Float(int32_t other) : value( IF::f_construct( other, (value_type const *) 0 ) ) {}
	Float(int64_t other) : value( IF::f_construct( other, (value_type const *) 0 ) ) {}
#if HAVE_FLOAT16
	Float(_Float16 other) : value( IF::f_construct( _FromPacked<float16_t>( ToPacked( other ) ), (value_type const *) 0 ) ) {}
#endif
	Float(float other) : value( IF::f_construct( _FromPacked<float32_t>( ToPacked( other ) ), (value_type const *) 0 ) ) {}
	Float(double other) : value( IF::f_construct( _FromPacked<float64_t>( ToPacked( other ) ), (value_type const *) 0 ) ) {}
	
	template <typename OTHER_T> this_type& operator = ( Float<IF, OTHER_T> const& other ) { value = IF::f_construct( other.value, (value_type const *) 0 ); return *this; }
	this_type& operator = ( this_type const& other ) { value = other.value; return *this; }
	
	this_type& operator += ( this_type const& other ) { value = IF::f_add( value, other.value ); return *this; }
	this_type& operator -= ( this_type const& other ) { value = IF::f_sub( value, other.value ); return *this; }
	this_type& operator *= ( this_type const& other ) { value = IF::f_mul( value, other.value ); return *this; }
	this_type& operator /= ( this_type const& other ) { value = IF::f_div( value, other.value ); return *this; }
	
	this_type operator - () const { return unisim::util::floating_point::softfloat_emu::_FNeg<this_type>( *this ); }
	this_type operator + ( this_type const& other ) const { return this_type( IF::f_add( value, other.value ) ); }
	this_type operator - ( this_type const& other ) const { return this_type( IF::f_sub( value, other.value ) ); }
	this_type operator * ( this_type const& other ) const { return this_type( IF::f_mul( value, other.value ) ); }
	this_type operator / ( this_type const& other ) const { return this_type( IF::f_div( value, other.value ) ); }
	
	bool operator <  (this_type const& other) const { return IF::f_lt( value, other.value ); }
	bool operator <= (this_type const& other) const { return IF::f_le( value, other.value ); }
	bool operator == (this_type const& other) const { return IF::f_eq( value, other.value ); }
	bool operator != (this_type const& other) const { return IF::f_ne( value, other.value ); }
	bool operator >= (this_type const& other) const { return IF::f_ge( value, other.value ); }
	bool operator >  (this_type const& other) const { return IF::f_gt( value, other.value ); }
	
	template <typename FLOAT> friend FLOAT _FromUnpacked( UnpackedFloat const& unpacked );
	template <typename FLOAT, typename PACKED> friend __UNISIM_FP_CONSTEXPR FLOAT _FromPacked( PACKED a );
	template <typename PACKED, typename FLOAT> friend PACKED _ToPacked( FLOAT a );
	template <typename FLOAT> friend void _ToUnpacked( UnpackedFloat& unpacked, FLOAT a );
	template <typename DST, typename SRC> friend DST _FConvert( SRC a );
	template <typename INTEGER, typename FLOAT> friend INTEGER _ToInt( FLOAT a, uint_fast8_t rmode, bool exact );
	template <typename INTEGER, typename FLOAT> friend INTEGER _ToIntRoundMinMag( FLOAT a, bool exact );
	friend this_type _RoundToInt<this_type>( this_type a, uint_fast8_t rmode, bool exact );
	friend this_type _FMulAdd<this_type>( this_type a, this_type b, this_type c);
	friend this_type _FRem<this_type>( this_type a, this_type b );
	friend this_type _FSqrt<this_type>( this_type a );
	friend this_type _FAbs<this_type>( this_type a );
	friend this_type _FNeg<this_type>( this_type a );
	friend this_type _FMin<this_type>( this_type a, this_type b );
	friend this_type _FMinNumber<this_type>( this_type a, this_type b );
	friend this_type _FMax<this_type>( this_type a, this_type b );
	friend this_type _FMaxNumber<this_type>( this_type a, this_type b );
	friend bool _IsNaN<this_type>( this_type a );
	friend bool _IsDenormal<this_type>( this_type a );
	friend bool _IsSignaling<this_type>( this_type a );
	friend this_type _ClearSignaling<this_type>( this_type a );
	friend this_type _DefaultNaN<this_type>();
	friend this_type _Zeroes<this_type>( this_type a );
	friend bool _IsZero<this_type>( this_type a );
	friend bool _IsNeg<this_type>( this_type a );
	friend bool _IsInf<this_type>( this_type a );
	template <typename STREAM, typename FLOAT> friend void _Print( STREAM& stream, FLOAT const& value );
	template <typename STREAM, typename FLOAT> friend void _Scan( STREAM& stream, FLOAT& value );
	friend struct FloatingPointTypeInfo<this_type>;

private:
	template <typename TRAIT, typename OTHER_T> friend struct Float;
	
	value_type value;
	explicit __UNISIM_FP_CONSTEXPR Float(value_type _value) : value(_value) {}
};

// Generic helper functions dependent from FloatingPointTypeInfo<> trait availability

template <typename FLOAT> FLOAT _FromUnpacked( UnpackedFloat const& unpacked )
{
	return FLOAT( unisim::util::floating_point::_FromUnpacked<typename FLOAT::value_type>( unpacked ) );
}

template <typename FLOAT, typename PACKED> __UNISIM_FP_CONSTEXPR FLOAT _FromPacked( PACKED a )
{
	return FLOAT( unisim::util::floating_point::_FromPacked<typename FLOAT::value_type>( a ) );
}

template <typename PACKED, typename FLOAT> PACKED _ToPacked( FLOAT a )
{
	return unisim::util::floating_point::_ToPacked( a.value );
}

template <typename FLOAT> void _ToUnpacked( UnpackedFloat& unpacked, FLOAT a )
{
	unisim::util::floating_point::_ToUnpacked( unpacked, a.value );
}

template <typename FLOAT> bool _IsNaN( FLOAT a ) { return unisim::util::floating_point::_IsNaN<typename FLOAT::value_type>( a.value ); }
template <typename FLOAT> bool _IsDenormal( FLOAT a ) { return unisim::util::floating_point::_IsDenormal<typename FLOAT::value_type>( a.value ); }
template <typename FLOAT> bool _IsZero( FLOAT a ) { return unisim::util::floating_point::_IsZero<typename FLOAT::value_type>( a.value ); }
template <typename FLOAT> bool _IsNeg( FLOAT a ) { return unisim::util::floating_point::_IsNeg<typename FLOAT::value_type>( a.value ); }
template <typename FLOAT> bool _IsInf( FLOAT a ) { return unisim::util::floating_point::_IsInf<typename FLOAT::value_type>( a.value ); }
template <typename FLOAT> bool _IsSignaling( FLOAT a ) { return FLOAT::interface_type::f_isSignalingNaN( a.value ); }

template <typename INTEGER, typename FLOAT> INTEGER _ToInt( FLOAT a, uint_fast8_t rmode, bool exact )
{
	return FLOAT::interface_type::f_toInt( a.value, FromRoundingMode( rmode ), exact, (INTEGER const *) 0 );
}

template <typename INTEGER, typename FLOAT> INTEGER _ToIntRoundMinMag( FLOAT a, bool exact )
{
	return FLOAT::interface_type::f_toInt_r_minMag( a.value, exact, (INTEGER const *) 0 );
}

template <typename FLOAT>
FLOAT _RoundToInt( FLOAT a, uint_fast8_t rmode, bool exact )
{
	return FLOAT( FLOAT::interface_type::f_roundToInt( a.value, FromRoundingMode( rmode ), exact ) );
}

template <typename FLOAT>
FLOAT _FMulAdd( FLOAT a, FLOAT b, FLOAT c)
{
	return FLOAT( FLOAT::interface_type::f_mulAdd( a.value, b.value, c.value ) );
}

template <typename FLOAT>
FLOAT _FRem( FLOAT a, FLOAT b )
{
	return FLOAT( FLOAT::interface_type::f_rem( a.value, b.value ) );
}

template <typename FLOAT>
FLOAT _FSqrt( FLOAT a )
{
	return FLOAT( FLOAT::interface_type::f_sqrt( a.value ) );
}

template <typename FLOAT> FLOAT _FAbs( FLOAT a )
{
	return FLOAT( unisim::util::floating_point::_FAbs( a.value ) );
}

template <typename FLOAT> FLOAT _FNeg( FLOAT a )
{
	return FLOAT( unisim::util::floating_point::_FNeg( a.value ) );
}

template <typename FLOAT>
FLOAT _FMin( FLOAT a, FLOAT b )
{
	return (IsZero(a) && IsZero(b)) ? (IsNeg(a) ? a : b) : ((a < b) ? a : b);
}

template <typename FLOAT>
FLOAT _FMinNumber( FLOAT a, FLOAT b )
{
	FLOAT z = IsNaN( a ) ? ( IsNaN( b ) ? a : b)
	                     : ( IsNaN( b ) ? a
	                                    : ( ( IsZero( a ) && IsZero( b ) ) ? ( IsNeg( a ) ? a : b )
	                                                                       : (FLOAT::interface_type::f_lt_quiet(a.value, b.value) ? a : b) ) );
	return ( IsNaN( z ) && FLOAT::interface_type::defaultNaN() && !IsSignaling( z ) ) ? DefaultNaN<FLOAT>() : z;
}

template <typename FLOAT>
FLOAT _FMax( FLOAT a, FLOAT b )
{
	return (IsZero(a) && IsZero(b)) ? (IsNeg(a) ? b : a) : ((a < b) ? b : a);
}

template <typename FLOAT>
FLOAT _FMaxNumber( FLOAT a, FLOAT b )
{
	FLOAT z = IsNaN( a ) ? ( IsNaN( b ) ? a : b)
	                     : ( IsNaN( b ) ? a
	                                    : ( ( IsZero( a ) && IsZero( b ) ) ? ( IsNeg( a ) ? b : a )
	                                                                       : (FLOAT::interface_type::f_lt_quiet(a.value, b.value) ? b : a) ) );
	return ( IsNaN( z ) && FLOAT::interface_type::defaultNaN() && !IsSignaling( z ) ) ? DefaultNaN<FLOAT>() : z;
}

template <typename FLOAT> FLOAT _ClearSignaling( FLOAT a ) { return FLOAT( FLOAT::interface_type::f_clearSignaling( a.value ) ); }
template <typename FLOAT> FLOAT _Zeroes( FLOAT a ) { return FLOAT( unisim::util::floating_point::_Zeroes( a.value ) ); }
template <typename FLOAT> FLOAT _DefaultNaN() { return FLOAT( FLOAT::interface_type::f_defaultNaN( (typename FLOAT::value_type const *) 0 ) ); }

template <typename STREAM, typename FLOAT>
void _Print(STREAM& stream, FLOAT const& value)
{
	stream << FromPacked<double>( FLOAT::interface_type::f_construct( value.value, (float64_t const *) 0).v );
}

template <typename STREAM, typename FLOAT>
void _Scan(STREAM& stream, FLOAT& value)
{
	double d;
	if(stream >> d)
	{
		value = FLOAT( FLOAT::interface_type::f_construct( FLOAT::interface_type::f_construct( ToPacked( d ), (float64_t const *) 0 ), (typename FLOAT::value_type const *) 0) );
	}
}

// All available floating-point types

namespace x86 {
	typedef Float<x86_if, float16_t> Half;
	typedef Float<x86_if, float32_t> Single;
	typedef Float<x86_if, float64_t> Double;
}

namespace x86_sse {
	typedef Float<x86_sse_if, float16_t> Half;
	typedef Float<x86_sse_if, float32_t> Single;
	typedef Float<x86_sse_if, float64_t> Double;
}

namespace arm_vfpv2 {
	typedef Float<arm_vfpv2_if, float16_t> Half;
	typedef Float<arm_vfpv2_if, float32_t> Single;
	typedef Float<arm_vfpv2_if, float64_t> Double;
}

namespace arm_vfpv2_dn {
	typedef Float<arm_vfpv2_dn_if, float16_t> Half;
	typedef Float<arm_vfpv2_dn_if, float32_t> Single;
	typedef Float<arm_vfpv2_dn_if, float64_t> Double;
}

namespace riscv {
	typedef Float<riscv_if, float16_t> Half;
	typedef Float<riscv_if, float32_t> Single;
	typedef Float<riscv_if, float64_t> Double;
}

namespace arm_vfpv2_ddn {
	typedef Float<arm_vfpv2_ddn_if, float16_t> Half;
	typedef Float<arm_vfpv2_ddn_if, float32_t> Single;
	typedef Float<arm_vfpv2_ddn_if, float64_t> Double;
}

} // end of namespace softfloat_emu
} // end of namespace floating_point
} // end of namespace util
} // end of namespace unisim

// Functions and traits for all emulated floating-point types

#undef SOFTFLOAT_EMU_DEF_FUNCTIONS
#define SOFTFLOAT_EMU_DEF_FUNCTIONS( NAMESPACE, FLOAT, PACKED )                                                                                                                     \
template <> inline FLOAT FromUnpacked<FLOAT>( UnpackedFloat const& unpacked )                 { return NAMESPACE::_FromUnpacked<FLOAT>( unpacked ); }                               \
template <> inline __UNISIM_FP_CONSTEXPR FLOAT FromPacked<FLOAT, PACKED>( PACKED a )          { return NAMESPACE::_FromPacked<FLOAT, PACKED>( a ); }                                \
inline PACKED ToPacked( FLOAT a )                                                             { return NAMESPACE::_ToPacked<PACKED, FLOAT>( a ); }                                  \
inline void ToUnpacked( UnpackedFloat& unpacked, FLOAT a )                                    { return NAMESPACE::_ToUnpacked<FLOAT>( unpacked, a ); }                              \
template <> inline uint16_t ToInt<uint16_t, FLOAT>( FLOAT a, uint_fast8_t rmode, bool exact ) { return NAMESPACE::_ToInt<uint16_t, FLOAT>( a, rmode, exact ); }                     \
template <> inline uint32_t ToInt<uint32_t, FLOAT>( FLOAT a, uint_fast8_t rmode, bool exact ) { return NAMESPACE::_ToInt<uint32_t, FLOAT>( a, rmode, exact ); }                     \
template <> inline uint64_t ToInt<uint64_t, FLOAT>( FLOAT a, uint_fast8_t rmode, bool exact ) { return NAMESPACE::_ToInt<uint64_t, FLOAT>( a, rmode, exact ); }                     \
template <> inline int16_t  ToInt<int16_t , FLOAT>( FLOAT a, uint_fast8_t rmode, bool exact ) { return NAMESPACE::_ToInt<int16_t , FLOAT>( a, rmode, exact ); }                     \
template <> inline int32_t  ToInt<int32_t , FLOAT>( FLOAT a, uint_fast8_t rmode, bool exact ) { return NAMESPACE::_ToInt<int32_t , FLOAT>( a, rmode, exact ); }                     \
template <> inline int64_t  ToInt<int64_t , FLOAT>( FLOAT a, uint_fast8_t rmode, bool exact ) { return NAMESPACE::_ToInt<int64_t , FLOAT>( a, rmode, exact ); }                     \
template <> inline uint16_t ToIntRoundMinMag<uint16_t, FLOAT>( FLOAT a, bool exact )          { return NAMESPACE::_ToIntRoundMinMag<uint16_t, FLOAT>( a, exact ); }                 \
template <> inline uint32_t ToIntRoundMinMag<uint32_t, FLOAT>( FLOAT a, bool exact )          { return NAMESPACE::_ToIntRoundMinMag<uint32_t, FLOAT>( a, exact ); }                 \
template <> inline uint64_t ToIntRoundMinMag<uint64_t, FLOAT>( FLOAT a, bool exact )          { return NAMESPACE::_ToIntRoundMinMag<uint64_t, FLOAT>( a, exact ); }                 \
template <> inline int16_t  ToIntRoundMinMag<int16_t , FLOAT>( FLOAT a, bool exact )          { return NAMESPACE::_ToIntRoundMinMag<int16_t , FLOAT>( a, exact ); }                 \
template <> inline int32_t  ToIntRoundMinMag<int32_t , FLOAT>( FLOAT a, bool exact )          { return NAMESPACE::_ToIntRoundMinMag<int32_t , FLOAT>( a, exact ); }                 \
template <> inline int64_t  ToIntRoundMinMag<int64_t , FLOAT>( FLOAT a, bool exact )          { return NAMESPACE::_ToIntRoundMinMag<int64_t , FLOAT>( a, exact ); }                 \
inline FLOAT RoundToInt( FLOAT a, uint_fast8_t rmode, bool exact )                            { return NAMESPACE::_RoundToInt<FLOAT>( a, rmode, exact ); }                          \
inline FLOAT FMulAdd( FLOAT a, FLOAT b, FLOAT c)                                              { return NAMESPACE::_FMulAdd<FLOAT>( a, b, c ); }                                     \
inline FLOAT FRem( FLOAT a, FLOAT b )                                                         { return NAMESPACE::_FRem<FLOAT>( a, b ); }                                           \
inline FLOAT FSqrt( FLOAT a )                                                                 { return NAMESPACE::_FSqrt<FLOAT>( a ); }                                             \
inline FLOAT FAbs( FLOAT a )                                                                  { return NAMESPACE::_FAbs<FLOAT>( a ); }                                              \
inline FLOAT FMin( FLOAT a, FLOAT b )                                                         { return NAMESPACE::_FMin<FLOAT>( a, b ); }                                           \
inline FLOAT FMinNumber( FLOAT a, FLOAT b )                                                   { return NAMESPACE::_FMinNumber<FLOAT>( a, b ); }                                     \
inline FLOAT FMax( FLOAT a, FLOAT b )                                                         { return NAMESPACE::_FMax<FLOAT>( a, b ); }                                           \
inline FLOAT FMaxNumber( FLOAT a, FLOAT b )                                                   { return NAMESPACE::_FMaxNumber<FLOAT>( a, b ); }                                     \
inline bool IsNaN( FLOAT a )                                                                  { return NAMESPACE::_IsNaN<FLOAT>( a ); }                                             \
inline bool IsDenormal( FLOAT a )                                                             { return NAMESPACE::_IsDenormal<FLOAT>( a ); }                                        \
inline bool IsSignaling( FLOAT a )                                                            { return NAMESPACE::_IsSignaling<FLOAT>( a ); }                                       \
inline FLOAT ClearSignaling( FLOAT a )                                                        { return NAMESPACE::_ClearSignaling<FLOAT>( a ); }                                    \
template <> inline FLOAT DefaultNaN<FLOAT>()                                                  { return NAMESPACE::_DefaultNaN<FLOAT>(); }                                           \
inline FLOAT Zeroes( FLOAT a )                                                                { return NAMESPACE::_Zeroes<FLOAT>( a ); }                                            \
inline bool IsZero( FLOAT a )                                                                 { return NAMESPACE::_IsZero<FLOAT>( a ); }                                            \
inline bool IsNeg( FLOAT a )                                                                  { return NAMESPACE::_IsNeg<FLOAT>( a ); }                                             \
inline bool IsInf( FLOAT a )                                                                  { return NAMESPACE::_IsInf<FLOAT>( a ); }                                             \
inline std::ostream& operator << (std::ostream& stream, FLOAT const& value)                   { NAMESPACE::_Print<std::ostream, FLOAT> (stream, value); return stream; }            \
inline std::wostream& operator << (std::wostream& stream, FLOAT const& value)                 { NAMESPACE::_Print<std::wostream, FLOAT>(stream, value); return stream; }            \
inline std::istream& operator >> (std::istream& stream, FLOAT& value)                         { NAMESPACE::_Scan<std::istream, FLOAT>  (stream, value); return stream; }            \
inline std::wistream& operator >> (std::wistream& stream, FLOAT& value)                       { NAMESPACE::_Scan<std::wistream, FLOAT> (stream, value); return stream; }

#undef SOFTFLOAT_EMU_DEF_CONVERSION_FUNCTIONS
#define SOFTFLOAT_EMU_DEF_CONVERSION_FUNCTIONS( NAMESPACE, HALF, SINGLE, DOUBLE )      \
template <> inline HALF   FConvert<HALF  , HALF  >( HALF   a ) { return a; }           \
template <> inline HALF   FConvert<HALF  , SINGLE>( SINGLE a ) { return HALF( a ); }   \
template <> inline HALF   FConvert<HALF  , DOUBLE>( DOUBLE a ) { return HALF( a ); }   \
template <> inline SINGLE FConvert<SINGLE, HALF  >( HALF   a ) { return SINGLE( a ); } \
template <> inline SINGLE FConvert<SINGLE, SINGLE>( SINGLE a ) { return a; }           \
template <> inline SINGLE FConvert<SINGLE, DOUBLE>( DOUBLE a ) { return SINGLE( a ); } \
template <> inline DOUBLE FConvert<DOUBLE, HALF  >( HALF   a ) { return DOUBLE( a ); } \
template <> inline DOUBLE FConvert<DOUBLE, SINGLE>( SINGLE a ) { return DOUBLE( a ); } \
template <> inline DOUBLE FConvert<DOUBLE, DOUBLE>( DOUBLE a ) { return a; }

SOFTFLOAT_EMU_DEF_FUNCTIONS( unisim::util::floating_point::softfloat_emu, unisim::util::floating_point::softfloat_emu::x86::Half            , uint16_t )
SOFTFLOAT_EMU_DEF_FUNCTIONS( unisim::util::floating_point::softfloat_emu, unisim::util::floating_point::softfloat_emu::x86::Single          , uint32_t )
SOFTFLOAT_EMU_DEF_FUNCTIONS( unisim::util::floating_point::softfloat_emu, unisim::util::floating_point::softfloat_emu::x86::Double          , uint64_t )
SOFTFLOAT_EMU_DEF_FUNCTIONS( unisim::util::floating_point::softfloat_emu, unisim::util::floating_point::softfloat_emu::x86_sse::Half        , uint16_t )
SOFTFLOAT_EMU_DEF_FUNCTIONS( unisim::util::floating_point::softfloat_emu, unisim::util::floating_point::softfloat_emu::x86_sse::Single      , uint32_t )
SOFTFLOAT_EMU_DEF_FUNCTIONS( unisim::util::floating_point::softfloat_emu, unisim::util::floating_point::softfloat_emu::x86_sse::Double      , uint64_t )
SOFTFLOAT_EMU_DEF_FUNCTIONS( unisim::util::floating_point::softfloat_emu, unisim::util::floating_point::softfloat_emu::arm_vfpv2::Half      , uint16_t )
SOFTFLOAT_EMU_DEF_FUNCTIONS( unisim::util::floating_point::softfloat_emu, unisim::util::floating_point::softfloat_emu::arm_vfpv2::Single    , uint32_t )
SOFTFLOAT_EMU_DEF_FUNCTIONS( unisim::util::floating_point::softfloat_emu, unisim::util::floating_point::softfloat_emu::arm_vfpv2::Double    , uint64_t )
SOFTFLOAT_EMU_DEF_FUNCTIONS( unisim::util::floating_point::softfloat_emu, unisim::util::floating_point::softfloat_emu::arm_vfpv2_dn::Half   , uint16_t )
SOFTFLOAT_EMU_DEF_FUNCTIONS( unisim::util::floating_point::softfloat_emu, unisim::util::floating_point::softfloat_emu::arm_vfpv2_dn::Single , uint32_t )
SOFTFLOAT_EMU_DEF_FUNCTIONS( unisim::util::floating_point::softfloat_emu, unisim::util::floating_point::softfloat_emu::arm_vfpv2_dn::Double , uint64_t )
SOFTFLOAT_EMU_DEF_FUNCTIONS( unisim::util::floating_point::softfloat_emu, unisim::util::floating_point::softfloat_emu::riscv::Half          , uint16_t )
SOFTFLOAT_EMU_DEF_FUNCTIONS( unisim::util::floating_point::softfloat_emu, unisim::util::floating_point::softfloat_emu::riscv::Single        , uint32_t )
SOFTFLOAT_EMU_DEF_FUNCTIONS( unisim::util::floating_point::softfloat_emu, unisim::util::floating_point::softfloat_emu::riscv::Double        , uint64_t )
SOFTFLOAT_EMU_DEF_FUNCTIONS( unisim::util::floating_point::softfloat_emu, unisim::util::floating_point::softfloat_emu::arm_vfpv2_ddn::Half  , uint16_t )
SOFTFLOAT_EMU_DEF_FUNCTIONS( unisim::util::floating_point::softfloat_emu, unisim::util::floating_point::softfloat_emu::arm_vfpv2_ddn::Single, uint32_t )
SOFTFLOAT_EMU_DEF_FUNCTIONS( unisim::util::floating_point::softfloat_emu, unisim::util::floating_point::softfloat_emu::arm_vfpv2_ddn::Double, uint64_t )
SOFTFLOAT_EMU_DEF_CONVERSION_FUNCTIONS( unisim::util::floating_point::softfloat_emu,
                                        unisim::util::floating_point::softfloat_emu::x86::Half,
                                        unisim::util::floating_point::softfloat_emu::x86::Single,
                                        unisim::util::floating_point::softfloat_emu::x86::Double )
SOFTFLOAT_EMU_DEF_CONVERSION_FUNCTIONS( unisim::util::floating_point::softfloat_emu,
                                        unisim::util::floating_point::softfloat_emu::x86_sse::Half,
                                        unisim::util::floating_point::softfloat_emu::x86_sse::Single,
                                        unisim::util::floating_point::softfloat_emu::x86_sse::Double )
SOFTFLOAT_EMU_DEF_CONVERSION_FUNCTIONS( unisim::util::floating_point::softfloat_emu,
                                        unisim::util::floating_point::softfloat_emu::arm_vfpv2::Half,
                                        unisim::util::floating_point::softfloat_emu::arm_vfpv2::Single,
                                        unisim::util::floating_point::softfloat_emu::arm_vfpv2::Double )
SOFTFLOAT_EMU_DEF_CONVERSION_FUNCTIONS( unisim::util::floating_point::softfloat_emu,
                                        unisim::util::floating_point::softfloat_emu::arm_vfpv2_dn::Half,
                                        unisim::util::floating_point::softfloat_emu::arm_vfpv2_dn::Single,
                                        unisim::util::floating_point::softfloat_emu::arm_vfpv2_dn::Double )
SOFTFLOAT_EMU_DEF_CONVERSION_FUNCTIONS( unisim::util::floating_point::softfloat_emu,
                                        unisim::util::floating_point::softfloat_emu::riscv::Half,
                                        unisim::util::floating_point::softfloat_emu::riscv::Single,
                                        unisim::util::floating_point::softfloat_emu::riscv::Double )
SOFTFLOAT_EMU_DEF_CONVERSION_FUNCTIONS( unisim::util::floating_point::softfloat_emu,
                                        unisim::util::floating_point::softfloat_emu::arm_vfpv2_ddn::Half,
                                        unisim::util::floating_point::softfloat_emu::arm_vfpv2_ddn::Single,
                                        unisim::util::floating_point::softfloat_emu::arm_vfpv2_ddn::Double )

#undef SOFTFLOAT_EMU_DEF_FPSC
#define SOFTFLOAT_EMU_DEF_FPSC( FLOAT )                                                                                          \
template <>                                                                                                                      \
struct FloatingPointStatusAndControl<FLOAT>                                                                                      \
{                                                                                                                                \
	typedef unisim::util::floating_point::Context<FloatingPointStatusAndControl<FLOAT> > Context;                                  \
                                                                                                                                 \
	static void defaultNaN( bool dn ) { FLOAT::interface_type::defaultNaN( dn );}                                                  \
	static bool defaultNaN() { return FLOAT::interface_type::defaultNaN(); }                                                       \
	static void detectTininess( uint_fast8_t dt ) { FLOAT::interface_type::detectTininess( dt ); }                                 \
	static uint_fast8_t detectTininess() { return FLOAT::interface_type::detectTininess(); }                                       \
	static void roundingMode( uint_fast8_t rmode ) { FLOAT::interface_type::roundingMode( rmode ); }                               \
	static uint_fast8_t roundingMode() { return FLOAT::interface_type::roundingMode(); }                                           \
	static void exceptionFlags( uint_fast8_t eflags ) { FLOAT::interface_type::exceptionFlags( eflags ); }                         \
	static uint_fast8_t exceptionFlags() { return FLOAT::interface_type::exceptionFlags(); }                                       \
	static void extF80_roundingPrecision( uint_fast8_t ext80_rp ) { FLOAT::interface_type::extF80_roundingPrecision( ext80_rp ); } \
	static uint_fast8_t extF80_roundingPrecision() { return FLOAT::interface_type::extF80_roundingPrecision(); }                   \
	static void flushToZero( uint_fast8_t ftz ) { FLOAT::interface_type::flushToZero( ftz ); }                                     \
	static uint_fast8_t flushToZero() { return FLOAT::interface_type::flushToZero(); }                                             \
};

SOFTFLOAT_EMU_DEF_FPSC( unisim::util::floating_point::softfloat_emu::x86          ::Half   )
SOFTFLOAT_EMU_DEF_FPSC( unisim::util::floating_point::softfloat_emu::x86          ::Single )
SOFTFLOAT_EMU_DEF_FPSC( unisim::util::floating_point::softfloat_emu::x86          ::Double )
SOFTFLOAT_EMU_DEF_FPSC( unisim::util::floating_point::softfloat_emu::x86_sse      ::Half   )
SOFTFLOAT_EMU_DEF_FPSC( unisim::util::floating_point::softfloat_emu::x86_sse      ::Single )
SOFTFLOAT_EMU_DEF_FPSC( unisim::util::floating_point::softfloat_emu::x86_sse      ::Double )
SOFTFLOAT_EMU_DEF_FPSC( unisim::util::floating_point::softfloat_emu::arm_vfpv2    ::Half   )
SOFTFLOAT_EMU_DEF_FPSC( unisim::util::floating_point::softfloat_emu::arm_vfpv2    ::Single )
SOFTFLOAT_EMU_DEF_FPSC( unisim::util::floating_point::softfloat_emu::arm_vfpv2    ::Double )
SOFTFLOAT_EMU_DEF_FPSC( unisim::util::floating_point::softfloat_emu::arm_vfpv2_dn ::Half   )
SOFTFLOAT_EMU_DEF_FPSC( unisim::util::floating_point::softfloat_emu::arm_vfpv2_dn ::Single )
SOFTFLOAT_EMU_DEF_FPSC( unisim::util::floating_point::softfloat_emu::arm_vfpv2_dn ::Double )
SOFTFLOAT_EMU_DEF_FPSC( unisim::util::floating_point::softfloat_emu::riscv        ::Half   )
SOFTFLOAT_EMU_DEF_FPSC( unisim::util::floating_point::softfloat_emu::riscv        ::Single )
SOFTFLOAT_EMU_DEF_FPSC( unisim::util::floating_point::softfloat_emu::riscv        ::Double )
SOFTFLOAT_EMU_DEF_FPSC( unisim::util::floating_point::softfloat_emu::arm_vfpv2_ddn::Half   )
SOFTFLOAT_EMU_DEF_FPSC( unisim::util::floating_point::softfloat_emu::arm_vfpv2_ddn::Single )
SOFTFLOAT_EMU_DEF_FPSC( unisim::util::floating_point::softfloat_emu::arm_vfpv2_ddn::Double )

#undef SOFTFLOAT_EMU_DEF_NUMERIC_LIMITS_FLOAT16
#define SOFTFLOAT_EMU_DEF_NUMERIC_LIMITS_FLOAT16( FLOAT )                                                       \
template <>                                                                                                     \
class std::numeric_limits<FLOAT>                                                                                \
{                                                                                                               \
public:                                                                                                         \
	static __UNISIM_FP_CONSTEXPR bool is_specialized = true;                                                      \
	static __UNISIM_FP_CONSTEXPR bool is_signed = true;                                                           \
	static __UNISIM_FP_CONSTEXPR bool is_integer = false;                                                         \
	static __UNISIM_FP_CONSTEXPR bool is_exact = false;                                                           \
	static __UNISIM_FP_CONSTEXPR bool has_infinity = true;                                                        \
	static __UNISIM_FP_CONSTEXPR bool has_quiet_NaN = true;                                                       \
	static __UNISIM_FP_CONSTEXPR bool has_signaling_NaN = true;                                                   \
	static __UNISIM_FP_CONSTEXPR std::float_denorm_style has_denorm = std::denorm_present;                        \
	static __UNISIM_FP_CONSTEXPR bool has_denorm_loss = false;                                                    \
	static __UNISIM_FP_CONSTEXPR std::float_round_style round_style = std::round_to_nearest;                      \
	static __UNISIM_FP_CONSTEXPR bool is_iec559 = true;                                                           \
	static __UNISIM_FP_CONSTEXPR bool is_bounded = true;                                                          \
	static __UNISIM_FP_CONSTEXPR bool is_modulo = false;                                                          \
	static __UNISIM_FP_CONSTEXPR int digits = 11;                                                                 \
	static __UNISIM_FP_CONSTEXPR int digits10 = 3;                                                                \
	static __UNISIM_FP_CONSTEXPR int max_digits10 = 5;                                                            \
	static __UNISIM_FP_CONSTEXPR int radix = 2;                                                                   \
	static __UNISIM_FP_CONSTEXPR int min_exponent = -13;                                                          \
	static __UNISIM_FP_CONSTEXPR int min_exponent10 = -4 ;                                                        \
	static __UNISIM_FP_CONSTEXPR int max_exponent = 16;                                                           \
	static __UNISIM_FP_CONSTEXPR int max_exponent10 = 4 ;                                                         \
	static __UNISIM_FP_CONSTEXPR bool traps = false;                                                              \
	static __UNISIM_FP_CONSTEXPR int tinyness_before = false;                                                     \
	                                                                                                              \
	static __UNISIM_FP_CONSTEXPR FLOAT min()           noexcept { return FromPacked<FLOAT>( uint16_t(0x0400) ); } \
	static __UNISIM_FP_CONSTEXPR FLOAT lowest()        noexcept { return FromPacked<FLOAT>( uint16_t(0xfbff) ); } \
	static __UNISIM_FP_CONSTEXPR FLOAT max()           noexcept { return FromPacked<FLOAT>( uint16_t(0x7bff) ); } \
	static __UNISIM_FP_CONSTEXPR FLOAT epsilon()       noexcept { return FromPacked<FLOAT>( uint16_t(0x1400) ); } \
	static __UNISIM_FP_CONSTEXPR FLOAT round_error()   noexcept { return FromPacked<FLOAT>( uint16_t(0x3800) ); } \
	static __UNISIM_FP_CONSTEXPR FLOAT infinity()      noexcept { return FromPacked<FLOAT>( uint16_t(0x7c00) ); } \
	static __UNISIM_FP_CONSTEXPR FLOAT quiet_NaN()     noexcept { return FromPacked<FLOAT>( uint16_t(0x7e00) ); } \
	static __UNISIM_FP_CONSTEXPR FLOAT signaling_NaN() noexcept { return FromPacked<FLOAT>( uint16_t(0x7d00) ); } \
	static __UNISIM_FP_CONSTEXPR FLOAT denorm_min()    noexcept { return FromPacked<FLOAT>( uint16_t(0x0001) ); } \
};

#undef SOFTFLOAT_EMU_DEF_NUMERIC_LIMITS_FLOAT32
#define SOFTFLOAT_EMU_DEF_NUMERIC_LIMITS_FLOAT32( FLOAT )                                                           \
template <>                                                                                                         \
class std::numeric_limits<FLOAT>                                                                                    \
{                                                                                                                   \
public:                                                                                                             \
	static __UNISIM_FP_CONSTEXPR bool is_specialized = true;                                                          \
	static __UNISIM_FP_CONSTEXPR bool is_signed = true;                                                               \
	static __UNISIM_FP_CONSTEXPR bool is_integer = false;                                                             \
	static __UNISIM_FP_CONSTEXPR bool is_exact = false;                                                               \
	static __UNISIM_FP_CONSTEXPR bool has_infinity = true;                                                            \
	static __UNISIM_FP_CONSTEXPR bool has_quiet_NaN = true;                                                           \
	static __UNISIM_FP_CONSTEXPR bool has_signaling_NaN = true;                                                       \
	static __UNISIM_FP_CONSTEXPR std::float_denorm_style has_denorm = std::denorm_present;                            \
	static __UNISIM_FP_CONSTEXPR bool has_denorm_loss = false;                                                        \
	static __UNISIM_FP_CONSTEXPR std::float_round_style round_style = std::round_to_nearest;                          \
	static __UNISIM_FP_CONSTEXPR bool is_iec559 = true;                                                               \
	static __UNISIM_FP_CONSTEXPR bool is_bounded = true;                                                              \
	static __UNISIM_FP_CONSTEXPR bool is_modulo = false;                                                              \
	static __UNISIM_FP_CONSTEXPR int digits = 24;                                                                     \
	static __UNISIM_FP_CONSTEXPR int digits10 = 6;                                                                    \
	static __UNISIM_FP_CONSTEXPR int max_digits10 = 10;                                                               \
	static __UNISIM_FP_CONSTEXPR int radix = 2;                                                                       \
	static __UNISIM_FP_CONSTEXPR int min_exponent = -125;                                                             \
	static __UNISIM_FP_CONSTEXPR int min_exponent10 = -37;                                                            \
	static __UNISIM_FP_CONSTEXPR int max_exponent = 128;                                                              \
	static __UNISIM_FP_CONSTEXPR int max_exponent10 = 38;                                                             \
	static __UNISIM_FP_CONSTEXPR bool traps = false;                                                                  \
	static __UNISIM_FP_CONSTEXPR int tinyness_before = false;                                                         \
	                                                                                                                  \
	static __UNISIM_FP_CONSTEXPR FLOAT min()           noexcept { return FromPacked<FLOAT>( uint32_t(0x00800000) ); } \
	static __UNISIM_FP_CONSTEXPR FLOAT lowest()        noexcept { return FromPacked<FLOAT>( uint32_t(0xff7fffff) ); } \
	static __UNISIM_FP_CONSTEXPR FLOAT max()           noexcept { return FromPacked<FLOAT>( uint32_t(0x7f7fffff) ); } \
	static __UNISIM_FP_CONSTEXPR FLOAT epsilon()       noexcept { return FromPacked<FLOAT>( uint32_t(0x34000000) ); } \
	static __UNISIM_FP_CONSTEXPR FLOAT round_error()   noexcept { return FromPacked<FLOAT>( uint32_t(0x3f000000) ); } \
	static __UNISIM_FP_CONSTEXPR FLOAT infinity()      noexcept { return FromPacked<FLOAT>( uint32_t(0x7f800000) ); } \
	static __UNISIM_FP_CONSTEXPR FLOAT quiet_NaN()     noexcept { return FromPacked<FLOAT>( uint32_t(0x7fc00000) ); } \
	static __UNISIM_FP_CONSTEXPR FLOAT signaling_NaN() noexcept { return FromPacked<FLOAT>( uint32_t(0x7fa00000) ); } \
	static __UNISIM_FP_CONSTEXPR FLOAT denorm_min()    noexcept { return FromPacked<FLOAT>( uint32_t(0x00000001) ); } \
};

#undef SOFTFLOAT_EMU_DEF_NUMERIC_LIMITS_FLOAT64
#define SOFTFLOAT_EMU_DEF_NUMERIC_LIMITS_FLOAT64( FLOAT )                                                                      \
template <>                                                                                                                    \
class std::numeric_limits<FLOAT>                                                                                               \
{                                                                                                                              \
public:                                                                                                                        \
	static __UNISIM_FP_CONSTEXPR bool is_specialized = true;                                                                     \
	static __UNISIM_FP_CONSTEXPR bool is_signed = true;                                                                          \
	static __UNISIM_FP_CONSTEXPR bool is_integer = false;                                                                        \
	static __UNISIM_FP_CONSTEXPR bool is_exact = false;                                                                          \
	static __UNISIM_FP_CONSTEXPR bool has_infinity = true;                                                                       \
	static __UNISIM_FP_CONSTEXPR bool has_quiet_NaN = true;                                                                      \
	static __UNISIM_FP_CONSTEXPR bool has_signaling_NaN = true;                                                                  \
	static __UNISIM_FP_CONSTEXPR std::float_denorm_style has_denorm = std::denorm_present;                                       \
	static __UNISIM_FP_CONSTEXPR bool has_denorm_loss = false;                                                                   \
	static __UNISIM_FP_CONSTEXPR std::float_round_style round_style = std::round_to_nearest;                                     \
	static __UNISIM_FP_CONSTEXPR bool is_iec559 = true;                                                                          \
	static __UNISIM_FP_CONSTEXPR bool is_bounded = true;                                                                         \
	static __UNISIM_FP_CONSTEXPR bool is_modulo = false;                                                                         \
	static __UNISIM_FP_CONSTEXPR int digits = 53;                                                                                \
	static __UNISIM_FP_CONSTEXPR int digits10 = 15;                                                                              \
	static __UNISIM_FP_CONSTEXPR int max_digits10 = 17;                                                                          \
	static __UNISIM_FP_CONSTEXPR int radix = 2;                                                                                  \
	static __UNISIM_FP_CONSTEXPR int min_exponent = -1021;                                                                       \
	static __UNISIM_FP_CONSTEXPR int min_exponent10 = -307;                                                                      \
	static __UNISIM_FP_CONSTEXPR int max_exponent = 1024;                                                                        \
	static __UNISIM_FP_CONSTEXPR int max_exponent10 = 308;                                                                       \
	static __UNISIM_FP_CONSTEXPR bool traps = false;                                                                             \
	static __UNISIM_FP_CONSTEXPR int tinyness_before = false;                                                                    \
	                                                                                                                             \
	static __UNISIM_FP_CONSTEXPR FLOAT min()           noexcept { return FromPacked<FLOAT>( uint64_t(0x0010000000000000ULL) ); } \
	static __UNISIM_FP_CONSTEXPR FLOAT lowest()        noexcept { return FromPacked<FLOAT>( uint64_t(0xffefffffffffffffULL) ); } \
	static __UNISIM_FP_CONSTEXPR FLOAT max()           noexcept { return FromPacked<FLOAT>( uint64_t(0x7fefffffffffffffULL) ); } \
	static __UNISIM_FP_CONSTEXPR FLOAT epsilon()       noexcept { return FromPacked<FLOAT>( uint64_t(0x3cb0000000000000ULL) ); } \
	static __UNISIM_FP_CONSTEXPR FLOAT round_error()   noexcept { return FromPacked<FLOAT>( uint64_t(0x3fe0000000000000ULL) ); } \
	static __UNISIM_FP_CONSTEXPR FLOAT infinity()      noexcept { return FromPacked<FLOAT>( uint64_t(0x7ff0000000000000ULL) ); } \
	static __UNISIM_FP_CONSTEXPR FLOAT quiet_NaN()     noexcept { return FromPacked<FLOAT>( uint64_t(0x7ff8000000000000ULL) ); } \
	static __UNISIM_FP_CONSTEXPR FLOAT signaling_NaN() noexcept { return FromPacked<FLOAT>( uint64_t(0x7ff4000000000000ULL) ); } \
	static __UNISIM_FP_CONSTEXPR FLOAT denorm_min()    noexcept { return FromPacked<FLOAT>( uint64_t(0x0000000000000001ULL) ); } \
};

SOFTFLOAT_EMU_DEF_NUMERIC_LIMITS_FLOAT16( unisim::util::floating_point::softfloat_emu::x86::Half             );
SOFTFLOAT_EMU_DEF_NUMERIC_LIMITS_FLOAT16( unisim::util::floating_point::softfloat_emu::x86_sse::Half         );
SOFTFLOAT_EMU_DEF_NUMERIC_LIMITS_FLOAT16( unisim::util::floating_point::softfloat_emu::arm_vfpv2::Half       );
SOFTFLOAT_EMU_DEF_NUMERIC_LIMITS_FLOAT16( unisim::util::floating_point::softfloat_emu::arm_vfpv2_dn::Half    );
SOFTFLOAT_EMU_DEF_NUMERIC_LIMITS_FLOAT16( unisim::util::floating_point::softfloat_emu::riscv::Half           );
SOFTFLOAT_EMU_DEF_NUMERIC_LIMITS_FLOAT16( unisim::util::floating_point::softfloat_emu::arm_vfpv2_ddn::Half   );

SOFTFLOAT_EMU_DEF_NUMERIC_LIMITS_FLOAT32( unisim::util::floating_point::softfloat_emu::x86::Single           );
SOFTFLOAT_EMU_DEF_NUMERIC_LIMITS_FLOAT32( unisim::util::floating_point::softfloat_emu::x86_sse::Single       );
SOFTFLOAT_EMU_DEF_NUMERIC_LIMITS_FLOAT32( unisim::util::floating_point::softfloat_emu::arm_vfpv2::Single     );
SOFTFLOAT_EMU_DEF_NUMERIC_LIMITS_FLOAT32( unisim::util::floating_point::softfloat_emu::arm_vfpv2_dn::Single  );
SOFTFLOAT_EMU_DEF_NUMERIC_LIMITS_FLOAT32( unisim::util::floating_point::softfloat_emu::riscv::Single         );
SOFTFLOAT_EMU_DEF_NUMERIC_LIMITS_FLOAT32( unisim::util::floating_point::softfloat_emu::arm_vfpv2_ddn::Single );

SOFTFLOAT_EMU_DEF_NUMERIC_LIMITS_FLOAT64( unisim::util::floating_point::softfloat_emu::x86::Double           );
SOFTFLOAT_EMU_DEF_NUMERIC_LIMITS_FLOAT64( unisim::util::floating_point::softfloat_emu::x86_sse::Double       );
SOFTFLOAT_EMU_DEF_NUMERIC_LIMITS_FLOAT64( unisim::util::floating_point::softfloat_emu::arm_vfpv2::Double     );
SOFTFLOAT_EMU_DEF_NUMERIC_LIMITS_FLOAT64( unisim::util::floating_point::softfloat_emu::arm_vfpv2_dn::Double  );
SOFTFLOAT_EMU_DEF_NUMERIC_LIMITS_FLOAT64( unisim::util::floating_point::softfloat_emu::riscv::Double         );
SOFTFLOAT_EMU_DEF_NUMERIC_LIMITS_FLOAT64( unisim::util::floating_point::softfloat_emu::arm_vfpv2_ddn::Double );

#undef SOFTFLOAT_EMU_DEF_ARCH_IF_FUNCTIONS
#undef SOFTFLOAT_EMU_DEF_ARCH_IF_FUNCTIONS_FAST_INT64
#undef SOFTFLOAT_EMU_DEF_ARCH_IF
#undef SOFTFLOAT_EMU_DEF_ARCH_EXT_IF
#undef SOFTFLOAT_EMU_DEF_FUNCTIONS
#undef SOFTFLOAT_EMU_DEF_CONVERSION_FUNCTIONS
#undef SOFTFLOAT_EMU_DEF_FPSC
#undef SOFTFLOAT_EMU_DEF_NUMERIC_LIMITS_FLOAT16
#undef SOFTFLOAT_EMU_DEF_NUMERIC_LIMITS_FLOAT32
#undef SOFTFLOAT_EMU_DEF_NUMERIC_LIMITS_FLOAT64

#endif // HAVE_SOFTFLOAT_EMU

#endif // __UNISIM_UTIL_FLOATING_POINT_SOFTFLOAT_EMU_SOFTFLOAT_EMU_HH__
