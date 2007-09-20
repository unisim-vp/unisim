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
 */
 
#ifndef __UNISIM_UTIL_ARITHMETIC_ARITHMETIC_HH__
#define __UNISIM_UTIL_ARITHMETIC_ARITHMETIC_HH__

namespace unisim {
namespace util {
namespace arithmetic {

#if defined(__GNUC__) && (__GNUC__ >= 3)
inline void Add32(uint32_t& result, uint8_t& carry_out, uint8_t& overflow, uint32_t x, uint32_t y, uint8_t carry_in) __attribute__((always_inline));
inline void Sub32(uint32_t& result, uint8_t& carry_out, uint8_t& overflow, uint32_t x, uint32_t y, uint8_t carry_in) __attribute__((always_inline));

inline uint8_t RotateLeft(uint8_t v, unsigned int n) __attribute__((always_inline));
inline uint16_t RotateLeft(uint16_t v, unsigned int n) __attribute__((always_inline));
inline uint32_t RotateLeft(uint32_t v, unsigned int n) __attribute__((always_inline));
inline uint64_t RotateLeft(uint64_t v, unsigned int n) __attribute__((always_inline));
inline uint8_t RotateRight(uint8_t v, unsigned int n) __attribute__((always_inline));
inline uint16_t RotateRight(uint16_t v, unsigned int n) __attribute__((always_inline));
inline uint32_t RotateRight(uint32_t v, unsigned int n) __attribute__((always_inline));
inline uint64_t RotateRight(uint64_t v, unsigned int n) __attribute__((always_inline));

inline int8_t RotateLeft(int8_t v, unsigned int n) __attribute__((always_inline));
inline int16_t RotateLeft(int16_t v, unsigned int n) __attribute__((always_inline));
inline int32_t RotateLeft(int32_t v, unsigned int n) __attribute__((always_inline));
inline int64_t RotateLeft(int64_t v, unsigned int n) __attribute__((always_inline));
inline int8_t RotateRight(int8_t v, unsigned int n) __attribute__((always_inline));
inline int16_t RotateRight(int16_t v, unsigned int n) __attribute__((always_inline));
inline int32_t RotateRight(int32_t v, unsigned int n) __attribute__((always_inline));
inline int64_t RotateRight(int64_t v, unsigned int n) __attribute__((always_inline));

#endif

inline void Add32(uint32_t& result, uint8_t& carry_out, uint8_t& overflow, uint32_t x, uint32_t y, uint8_t carry_in)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && defined(__i386)
	if(carry_in)
	{
		__asm__ ("stc\nadc %4, %0\nsetc %1\nseto %2" : "=r" (result), "=Q" (carry_out), "=Q" (overflow) : "0" (x), "rm" (y) : "cc");	
	}
	else
	{
		__asm__ ("add %4, %0\nsetc %1\nseto %2" : "=r" (result), "=Q" (carry_out), "=Q" (overflow) : "0" (x), "rm" (y) : "cc");
	}
#else
	uint32_t result_tmp = x + y + carry_in;
	uint32_t carry = 0;
	if(((~((uint32_t) carry_in << 31))) | ((~(x ^ y)) & ((result_tmp !=0 ) << 31)))
	{
		carry = ((x & y) | (x & (result_tmp ^ x ^y)) | (y & (result_tmp ^ x ^y)));
	}
	else
	{
		carry = 0xffffffff;
	}
	overflow = (((result_tmp ^ x ^ y) ^ carry) >> 31) & 1;
	carry_out = (carry >> 31) & 1;
	result = result_tmp;
#endif
}

inline void Sub32(uint32_t& result, uint8_t& carry_out, uint8_t& overflow, uint32_t x, uint32_t y, uint8_t carry_in) {
  Add32(result, carry_out, overflow,
	x,
	-y,
	carry_in);
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

} // end of namespace arithmetic
} // end of namespace util
} // end of namespace unisim

#endif
