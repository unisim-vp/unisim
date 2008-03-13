/*
 *  Copyright (c) 2007,
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
 *          Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#ifndef __UNISIM_UTIL_ARITHMETIC_ARITHMETIC_HH__
#define __UNISIM_UTIL_ARITHMETIC_ARITHMETIC_HH__

namespace unisim {
namespace util {
namespace arithmetic {

#if defined(__GNUC__) && (__GNUC__ >= 3)
inline void Add8(uint8_t& result, uint8_t& carry_out, uint8_t& overflow, uint8_t x, uint8_t y, uint8_t carry_in) __attribute__((always_inline));
inline void Add16(uint16_t& result, uint8_t& carry_out, uint8_t& overflow, uint16_t x, uint16_t y, uint8_t carry_in) __attribute__((always_inline));
inline void Add32(uint32_t& result, uint8_t& carry_out, uint8_t& overflow, uint32_t x, uint32_t y, uint8_t carry_in) __attribute__((always_inline));
inline void Sub8(uint8_t& result, uint8_t& carry_out, uint8_t& overflow, uint8_t x, uint8_t y, uint8_t carry_in) __attribute__((always_inline));
inline void Sub16(uint16_t& result, uint8_t& carry_out, uint8_t& overflow, uint16_t x, uint16_t y, uint8_t carry_in) __attribute__((always_inline));
inline void Sub32(uint32_t& result, uint8_t& carry_out, uint8_t& overflow, uint32_t x, uint32_t y, uint8_t carry_in) __attribute__((always_inline));
inline void SignedSatAdd32(uint32_t& result, uint8_t& does_sat, uint32_t x, uint32_t y) __attribute__((always_inline));
inline void SignedSatSub32(uint32_t& result, uint8_t& does_sat, uint32_t x, uint32_t y) __attribute__((always_inline));

inline uint8_t RotateLeft(uint8_t v, unsigned int n) __attribute__((always_inline));
inline uint16_t RotateLeft(uint16_t v, unsigned int n) __attribute__((always_inline));
inline uint32_t RotateLeft(uint32_t v, unsigned int n) __attribute__((always_inline));
inline uint64_t RotateLeft(uint64_t v, unsigned int n) __attribute__((always_inline));
inline uint8_t RotateRight(uint8_t v, unsigned int n) __attribute__((always_inline));
inline uint16_t RotateRight(uint16_t v, unsigned int n) __attribute__((always_inline));
inline uint32_t RotateRight(uint32_t v, unsigned int n) __attribute__((always_inline));
inline uint64_t RotateRight(uint64_t v, unsigned int n) __attribute__((always_inline));

inline uint32_t RotateLeft(uint32_t v, unsigned int n, bool bitin, bool& bitout) __attribute__((always_inline));
inline uint32_t RotateRight(uint32_t v, unsigned int n, bool bitin, bool& bitout) __attribute__((always_inline));

inline int8_t RotateLeft(int8_t v, unsigned int n) __attribute__((always_inline));
inline int16_t RotateLeft(int16_t v, unsigned int n) __attribute__((always_inline));
inline int32_t RotateLeft(int32_t v, unsigned int n) __attribute__((always_inline));
inline int64_t RotateLeft(int64_t v, unsigned int n) __attribute__((always_inline));
inline int8_t RotateRight(int8_t v, unsigned int n) __attribute__((always_inline));
inline int16_t RotateRight(int16_t v, unsigned int n) __attribute__((always_inline));
inline int32_t RotateRight(int32_t v, unsigned int n) __attribute__((always_inline));
inline int64_t RotateRight(int64_t v, unsigned int n) __attribute__((always_inline));

inline uint32_t ShiftLeft(uint32_t v, unsigned int n) __attribute__((always_inline));
inline uint32_t ShiftLeft(uint32_t v, unsigned int n, uint8_t& bit_out) __attribute__((always_inline));
inline uint32_t ShiftRight(uint32_t v, unsigned int n) __attribute__((always_inline));
inline uint32_t ShiftRight(uint32_t v, unsigned int n, uint8_t& bit_out) __attribute__((always_inline));
inline uint32_t ShiftArithmeticRight(uint32_t v, unsigned int n) __attribute__((always_inline));
inline uint32_t ShiftArithmeticRight(uint32_t v, unsigned int n, uint8_t& bit_out) __attribute__((always_inline));

inline uint32_t RotateLeft(uint32_t v, unsigned int n, uint8_t& bit_out) __attribute__((always_inline));
inline uint32_t RotateRight(uint32_t v, unsigned int n, uint8_t& bit_out) __attribute__((always_inline));

#endif

inline void Add8(uint8_t& result, uint8_t& carry_out, uint8_t& overflow, uint8_t x, uint8_t y, uint8_t carry_in)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && defined(__i386)
	if(carry_in)
	{
		__asm__ ("stc\nadcb %b4, %b0\nsetc %1\nseto %2" : "=qQ" (result), "=qQ" (carry_out), "=qQ" (overflow) : "0" (x), "q" (y) : "cc");	
	}
	else
	{
		__asm__ ("addb %b4, %b0\nsetc %1\nseto %2" : "=qQ" (result), "=qQ" (carry_out), "=qQ" (overflow) : "0" (x), "q" (y) : "cc");
	}
#else
	uint8_t res = x + y + carry_in;
	uint8_t x7 = (x >> 7) & 1;
	uint8_t y7 = (y >> 7) & 1;
	uint8_t res7 = (res >> 7) & 1;
	uint8_t c6 = res7 ^ x7 ^ y7;
	uint8_t c7 = (x7 & y7) | (c6 & (x7 | y7));
	overflow = c7 ^ c6;
	carry_out = c7;
	result = res;
#endif
}

inline void Add16(uint16_t& result, uint8_t& carry_out, uint8_t& overflow, uint16_t x, uint16_t y, uint8_t carry_in)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && defined(__i386)
	if(carry_in)
	{
		__asm__ ("stc\nadcw %4, %w0\nsetc %1\nseto %2" : "=r" (result), "=qQ" (carry_out), "=qQ" (overflow) : "0" (x), "rm" (y) : "cc");	
	}
	else
	{
		__asm__ ("addw %4, %w0\nsetc %1\nseto %2" : "=r" (result), "=qQ" (carry_out), "=qQ" (overflow) : "0" (x), "rm" (y) : "cc");
	}
#else
	uint16_t res = x + y + carry_in;
	uint16_t x15 = (x >> 15) & 1;
	uint16_t y15 = (y >> 15) & 1;
	uint16_t res15 = (res >> 15) & 1;
	uint16_t c14 = res15 ^ x15 ^ y15;
	uint16_t c15 = (x15 & y15) | (c14 & (x15 | y15));
	overflow = c15 ^ c14;
	carry_out = c15;
	result = res;
#endif
}

/* The 31th 1-bit full adder equations are:
   (1) result(31) = x(31) ^ y(31) ^ carry(30)
   (2) carry(31) = x(31).y(31) + carry(30).(x(31) + y(31))
   (3) overflow = carry(31) ^ carry(30)
   result is easily computed from x + y + carry_in
   from (1) we obtain: carry(30) = result(31) ^ x(31) ^ y(31)
   carry_out is computed from equation (2), overflow from equation (3)
*/
inline void Add32(uint32_t& result, uint8_t& carry_out, uint8_t& overflow, uint32_t x, uint32_t y, uint8_t carry_in)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && defined(__i386)
	if(carry_in)
	{
		__asm__ ("stc\nadcl %4, %0\nsetc %1\nseto %2" : "=r" (result), "=qQ" (carry_out), "=qQ" (overflow) : "0" (x), "rm" (y) : "cc");	
	}
	else
	{
		__asm__ ("addl %4, %0\nsetc %1\nseto %2" : "=r" (result), "=qQ" (carry_out), "=qQ" (overflow) : "0" (x), "rm" (y) : "cc");
	}
#else
	uint32_t res = x + y + carry_in;
	uint32_t x31 = (x >> 31) & 1;
	uint32_t y31 = (y >> 31) & 1;
	uint32_t res31 = (res >> 31) & 1;
	uint32_t c30 = res31 ^ x31 ^ y31;
	uint32_t c31 = (x31 & y31) | (c30 & (x31 | y31));
	overflow = c31 ^ c30;
	carry_out = c31;
	result = res;
#endif
}

inline void Sub8(uint8_t& result, uint8_t& carry_out, uint8_t& overflow, uint8_t x, uint8_t y, uint8_t carry_in) {
  Add8(result, carry_out, overflow,
	x,
	-y,
	carry_in);
}

inline void Sub16(uint16_t& result, uint8_t& carry_out, uint8_t& overflow, uint16_t x, uint16_t y, uint8_t carry_in) {
  Add16(result, carry_out, overflow,
	x,
	-y,
	carry_in);
}

inline void Sub32(uint32_t& result, uint8_t& carry_out, uint8_t& overflow, uint32_t x, uint32_t y, uint8_t carry_in) {
  Add32(result, carry_out, overflow,
	x,
	-y,
	carry_in);
}

inline void SignedSatAdd32(uint32_t& result, uint8_t& does_sat, uint32_t x, uint32_t y) 
{
	int32_t sx = x;
	int32_t sy = y;
	int32_t sresult = 0;
	
	sresult = sx + sy;
	result = (uint32_t)sresult;
	does_sat = 0;
	if((x & (0x80000000ULL)) != (y & (0x80000000ULL))) {
		/* no need to saturate */
		return;
	}
	
	if(x & (0x80000000ULL)) {
		if((x & (0x80000000ULL)) != (result & (0x80000000ULL))) {
			does_sat = 1;
			result = 0x80000000ULL;
		}
	} else {
		if((x & (0x80000000ULL)) != (result & (0x80000000ULL))) {
			does_sat = 1; 
			result = 0x7fffffffULL;
		}
	}
}

inline void SignedSatSub32(uint32_t& result, uint8_t& does_sat, uint32_t x, uint32_t y) 
{
	int32_t sx = x;
	int32_t sy = y;
	int32_t sresult = 0;
	
	sresult = sx - sy;
	result = (uint32_t)sresult;
	does_sat = 0;
	if((x & (0x80000000ULL)) == (y & (0x80000000ULL))) {
		/* no need to saturate */
		return;
	}
	
	if(x & (0x80000000ULL)) {
		if((x & (0x80000000ULL)) != (result & (0x80000000ULL))) {
			does_sat = 1;
			result = 0x80000000ULL;
		}
	} else {
		if((x & (0x80000000ULL)) != (result & (0x80000000ULL))) {
			does_sat = 1; 
			result = 0x7fffffffULL;
		}
	}
}

/* Rotate functions: WARNING! only least significative bits of n are used !!!! */
inline uint8_t RotateLeft(uint8_t v, unsigned int n)
{
	n &= 7;
	return (v << n) | (v >> (8 - n));
}

inline uint16_t RotateLeft(uint16_t v, unsigned int n)
{
	n &= 15;
	return (v << n) | (v >> (16 - n));
}

inline uint32_t RotateLeft(uint32_t v, unsigned int n)
{
#if defined(__GNUC__) && defined(__i386)
	__asm__ ("rol %%cl, %0" : "=r" (v) : "0" (v), "c" (n) : "cc");
	return v;
#else
	n &= 31;
	return (v << n) | (v >> (32 - n));
#endif
}

inline uint64_t RotateLeft(uint64_t v, unsigned int n)
{
	n &= 63;
	return (v << n) | (v >> (64 - n));
}

inline uint8_t RotateRight(uint8_t v, unsigned int n)
{
	n &= 7;
	return (v >> n) | (v << (8 - n));
}

inline uint16_t RotateRight(uint16_t v, unsigned int n)
{
	n &= 15;
	return (v >> n) | (v << (16 - n));
}

inline uint32_t RotateRight(uint32_t v, unsigned int n)
{
#if defined(__GNUC__) && defined(__i386)
	__asm__ ("ror %%cl, %0" : "=r" (v) : "0" (v), "c" (n) : "cc");
	return v;
#else
	n &= 31;
	return (v >> n) | (v << (32 - n));
#endif
}

inline uint64_t RotateRight(uint64_t v, unsigned int n)
{
	n &= 63;
	return (v >> n) | (v << (64 - n));
}

inline int8_t RotateLeft(int8_t v, unsigned int n) { return RotateLeft((uint8_t) v, n); }
inline int16_t RotateLeft(int16_t v, unsigned int n) { return RotateLeft((uint16_t) v, n); }
inline int32_t RotateLeft(int32_t v, unsigned int n) { return RotateLeft((uint32_t) v, n); }
inline int64_t RotateLeft(int64_t v, unsigned int n) { return RotateLeft((uint64_t) v, n); }
inline int8_t RotateRight(int8_t v, unsigned int n) { return RotateRight((uint8_t) v, n); }
inline int16_t RotateRight(int16_t v, unsigned int n) { return RotateRight((uint16_t) v, n); }
inline int32_t RotateRight(int32_t v, unsigned int n) { return RotateRight((uint32_t) v, n); }
inline int64_t RotateRight(int64_t v, unsigned int n) { return RotateRight((uint64_t) v, n); }

inline uint32_t ShiftLeft(uint32_t v, unsigned int n)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && defined(__i386)
	__asm__ ("shl %%cl, %0" : "=r" (v) : "0" (v), "c" (n) : "cc");	
	return v;
#else
	n &= 31;
	return v << n;
#endif
}

inline uint32_t ShiftLeft(uint32_t v, unsigned int n, uint8_t& bit_out)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && defined(__i386)
	__asm__ ("shl %%cl, %0\nsetc %1" : "=r" (v), "=qQ" (bit_out): "0" (v), "c" (n) : "cc");	
	return v;
#else
	n &= 31;
	bit_out = (v >> (31 - n)) & 1;
	return v << n;
#endif
}

inline uint32_t ShiftRight(uint32_t v, unsigned int n)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && defined(__i386)
	__asm__ ("shr %%cl, %0" : "=r" (v) : "0" (v), "c" (n) : "cc");
	return v;
#else
	n &= 31;
	return v >> n;
#endif
}

inline uint32_t ShiftRight(uint32_t v, unsigned int n, uint8_t& bit_out)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && defined(__i386)
	__asm__ ("shr %%cl, %0\nsetc %1" : "=r" (v), "=qQ" (bit_out) : "0" (v), "c" (n) : "cc");	
	return v;
#else
	n &= 31;
	bit_out = (v >> (n - 1)) & 1;
	return v >> n;
#endif
}

inline uint32_t ShiftArithmeticRight(uint32_t v, unsigned int n)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && defined(__i386)
	__asm__ ("sar %%cl, %0" : "=r" (v) : "0" (v), "c" (n) : "cc");	
	return v;
#else
	n &= 31;
	return (uint32_t)((signed) v >> n);
#endif
}

inline uint32_t ShiftArithmeticRight(uint32_t v, unsigned int n, uint8_t& bit_out)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && defined(__i386)
	__asm__ ("sar %%cl, %0\nsetc %1" : "=r" (v), "=qQ" (bit_out) : "0" (v), "c" (n) : "cc");	
	return v;
#else
	n &= 31;
	bit_out = (v >> (n - 1)) & 1;
	return (uint32_t)((signed) v >> n);
#endif
}

inline uint32_t RotateLeft(uint32_t v, unsigned int n, uint8_t& bit_out)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && defined(__i386)
	__asm__ ("rol %%cl, %0\nsetc %1" : "=r" (v), "=qQ" (bit_out) : "0" (v), "c" (n) : "cc");	
	return v;
#else
	n &= 31;
	bit_out = (v >> (31 - n)) & 1;
	return (v << n) | (v >> (32 - n));
#endif
}

inline uint32_t RotateRight(uint32_t v, unsigned int n, uint8_t& bit_out)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && defined(__i386)
	__asm__ ("shr %%cl, %0\nsetc %1" : "=r" (v), "=qQ" (bit_out) : "0" (v), "c" (n) : "cc");	
	return v;
#else
	n &= 31;
	bit_out = (v >> (n - 1)) & 1;
	return (v >> n) | (v << (32 - n));
#endif
}


} // end of namespace arithmetic
} // end of namespace util
} // end of namespace unisim

#endif
