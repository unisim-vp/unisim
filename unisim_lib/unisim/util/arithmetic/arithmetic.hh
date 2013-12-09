/*
 *  Copyright (c) 2007-2009,
 *  Commissariat a l'Energie Atomique (CEA),
 *  University of Perpignan (UPVD)
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
 *          Reda Nouacer (reda.nouacer@cea.fr)
 *          Sylvain Collange (sylvain.collange@univ-perp.fr)
 */
 
#ifndef __UNISIM_UTIL_ARITHMETIC_ARITHMETIC_HH__
#define __UNISIM_UTIL_ARITHMETIC_ARITHMETIC_HH__

#include <inttypes.h>

namespace unisim {
namespace util {
namespace arithmetic {

#if defined(__GNUC__) && (__GNUC__ >= 3)
inline void SignedAdd8(uint8_t& result, uint8_t& carry_out, uint8_t& overflow, uint8_t& sign, uint8_t x, uint8_t y, uint8_t carry_in) __attribute__((always_inline));
inline void SignedAdd16(uint16_t& result, uint8_t& carry_out, uint8_t& overflow, uint8_t& sign, uint16_t x, uint16_t y, uint8_t carry_in) __attribute__((always_inline));
inline void SignedAdd32(uint32_t& result, uint8_t& carry_out, uint8_t& overflow, uint8_t& sign, uint32_t x, uint32_t y, uint8_t carry_in) __attribute__((always_inline));

inline void UnsignedAdd8(uint8_t& result, uint8_t& carry_out, uint8_t& overflow, uint8_t& sign, uint8_t x, uint8_t y, uint8_t carry_in) __attribute__((always_inline));
inline void UnsignedAdd16(uint16_t& result, uint8_t& carry_out, uint8_t& overflow, uint8_t& sign, uint16_t x, uint16_t y, uint8_t carry_in) __attribute__((always_inline));
inline void UnsignedAdd32(uint32_t& result, uint8_t& carry_out, uint8_t& overflow, uint8_t& sign, uint32_t x, uint32_t y, uint8_t carry_in) __attribute__((always_inline));

inline void SignedSub8(uint8_t& result, uint8_t& carry_out, uint8_t& overflow, uint8_t& sign, uint8_t x, uint8_t y, uint8_t carry_in) __attribute__((always_inline));
inline void SignedSub16(uint16_t& result, uint8_t& carry_out, uint8_t& overflow, uint8_t& sign, uint16_t x, uint16_t y, uint8_t carry_in) __attribute__((always_inline));
inline void SignedSub32(uint32_t& result, uint8_t& carry_out, uint8_t& overflow, uint8_t& sign, uint32_t x, uint32_t y, uint8_t carry_in) __attribute__((always_inline));

inline void SignedSatAdd8(uint8_t& result, uint8_t& borrow_out, uint8_t& overflow, uint8_t& sign, uint8_t x, uint8_t y, uint8_t borrow_in) __attribute__((always_inline));
inline void SignedSatAdd16(uint16_t& result, uint8_t& borrow_out, uint8_t& overflow, uint8_t& sign, uint16_t x, uint16_t y, uint8_t borrow_in) __attribute__((always_inline));
inline void SignedSatAdd32(uint32_t& result, uint8_t& borrow_out, uint8_t& overflow, uint8_t& sign, uint32_t x, uint32_t y, uint8_t borrow_in) __attribute__((always_inline));
inline void SignedSatAdd16(uint32_t& result, uint8_t& does_sat, uint32_t x, uint32_t y) __attribute__((always_inline));
inline void SignedSatAdd32(uint32_t& result, uint8_t& does_sat, uint32_t x, uint32_t y) __attribute__((always_inline));

inline void UnsignedSatAdd8(uint8_t& result, uint8_t& borrow_out, uint8_t& overflow, uint8_t& sign, uint8_t x, uint8_t y, uint8_t borrow_in) __attribute__((always_inline));
inline void UnsignedSatAdd16(uint16_t& result, uint8_t& borrow_out, uint8_t& overflow, uint8_t& sign, uint16_t x, uint16_t y, uint8_t borrow_in) __attribute__((always_inline));
inline void UnsignedSatAdd32(uint32_t& result, uint8_t& borrow_out, uint8_t& overflow, uint8_t& sign, uint32_t x, uint32_t y, uint8_t borrow_in) __attribute__((always_inline));

inline void SignedSatSub8(uint8_t& result, uint8_t& borrow_out, uint8_t& overflow, uint8_t& sign, uint8_t x, uint8_t y, uint8_t borrow_in) __attribute__((always_inline));
inline void SignedSatSub16(uint16_t& result, uint8_t& borrow_out, uint8_t& overflow, uint8_t& sign, uint16_t x, uint16_t y, uint8_t borrow_in) __attribute__((always_inline));
inline void SignedSatSub32(uint32_t& result, uint8_t& borrow_out, uint8_t& overflow, uint8_t& sign, uint32_t x, uint32_t y, uint8_t borrow_in) __attribute__((always_inline));
inline void SignedSatSub32(uint32_t& result, uint8_t& does_sat, uint32_t x, uint32_t y) __attribute__((always_inline));
inline void SignedSatSub16(uint32_t& result, uint8_t& does_sat, uint32_t x, uint32_t y) __attribute__((always_inline));

inline uint32_t ReverseCarryPropagationAdd(uint32_t x, uint32_t y) __attribute__((always_inline));

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

inline uint32_t RotateLeft(uint32_t v, unsigned int n, uint8_t& bit_out) __attribute__((always_inline));
inline uint32_t RotateLeft(uint32_t v, unsigned int n, uint8_t bit_in, uint8_t& bit_out) __attribute__((always_inline));
inline uint32_t RotateRight(uint32_t v, unsigned int n, uint8_t& bit_out) __attribute__((always_inline));
inline uint32_t RotateRight(uint32_t v, unsigned int n, uint8_t bit_in, uint8_t& bit_out) __attribute__((always_inline));
inline uint32_t RotateLeft(uint32_t v, unsigned int n, bool bitin, bool& bitout) __attribute__((always_inline));
inline uint32_t RotateRight(uint32_t v, unsigned int n, bool bitin, bool& bitout) __attribute__((always_inline));

inline uint32_t ShiftLeft(uint32_t v, unsigned int n) __attribute__((always_inline));
inline uint32_t ShiftLeft(uint32_t v, unsigned int n, uint8_t& bit_out) __attribute__((always_inline));
inline uint32_t ShiftRight(uint32_t v, unsigned int n) __attribute__((always_inline));
inline uint32_t ShiftRight(uint32_t v, unsigned int n, uint8_t& bit_out) __attribute__((always_inline));

inline uint32_t ShiftArithmeticRight(uint32_t v, unsigned int n) __attribute__((always_inline));
inline uint32_t ShiftArithmeticRight(uint32_t v, unsigned int n, uint8_t& bit_out) __attribute__((always_inline));

inline bool BitScanForward(unsigned int& n, uint32_t v) __attribute__((always_inline));
inline bool BitScanForward(unsigned int& n, uint64_t v) __attribute__((always_inline));

inline bool BitScanReverse(unsigned int& n, uint32_t v) __attribute__((always_inline));
inline bool BitScanReverse(unsigned int& n, uint64_t v) __attribute__((always_inline));

inline unsigned int CountLeadingZeros(uint32_t v) __attribute__((always_inline));
inline unsigned int CountLeadingZeros(uint64_t v) __attribute__((always_inline));

inline unsigned int Log2(uint32_t v) __attribute__((always_inline));
inline unsigned int Log2(uint64_t v) __attribute__((always_inline));

inline unsigned int CeilLog2(uint32_t v) __attribute__((always_inline));
inline unsigned int CeilLog2(uint64_t v) __attribute__((always_inline));

inline int8_t SignExtend(uint8_t v, unsigned int n) __attribute__((always_inline));
inline int16_t SignExtend(uint16_t v, unsigned int n) __attribute__((always_inline));
inline int32_t SignExtend(uint32_t v, unsigned int n) __attribute__((always_inline));
inline int64_t SignExtend(uint64_t v, unsigned int n) __attribute__((always_inline));

inline void EvenParity(uint8_t v, uint8_t& parity_out) __attribute__((always_inline));
inline void EvenParity(uint16_t v, uint8_t& parity_out) __attribute__((always_inline));
inline void EvenParity(uint32_t v, uint8_t& parity_out) __attribute__((always_inline));

inline void OddParity(uint8_t v, uint8_t& parity_out) __attribute__((always_inline));
inline void OddParity(uint16_t v, uint8_t& parity_out) __attribute__((always_inline));
inline void OddParity(uint32_t v, uint8_t& parity_out) __attribute__((always_inline));

#endif

//=============================================================================
//=                              Full Adders                                  =
//=============================================================================

inline void SignedAdd8(uint8_t& result, uint8_t& carry_out, uint8_t& overflow, uint8_t& sign, uint8_t x, uint8_t y, uint8_t carry_in)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && (defined(__i386) || defined(__x86_64))
	if(carry_in)
	{
		__asm__ ("stc\nadcb %5, %0\nsetc %1\nseto %2\nsets %3" : "=qQ" (result), "=qQm" (carry_out), "=qQm" (overflow), "=qQm" (sign) : "0" (x), "qQm" (y) : "cc");	
	}
	else
	{
		__asm__ ("addb %5, %0\nsetc %1\nseto %2\nsets %3" : "=qQ" (result), "=qQm" (carry_out), "=qQm" (overflow), "=qQm" (sign) : "0" (x), "qQm" (y) : "cc");
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
	sign = (int8_t) res < 0;
#endif
}

inline void SignedAdd16(uint16_t& result, uint8_t& carry_out, uint8_t& overflow, uint8_t& sign, uint16_t x, uint16_t y, uint8_t carry_in)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && (defined(__i386) || defined(__x86_64))
	if(carry_in)
	{
		__asm__ ("stc\nadcw %5, %0\nsetc %1\nseto %2\nsets %3" : "=r" (result), "=qQm" (carry_out), "=qQm" (overflow), "=qQm" (sign) : "0" (x), "rm" (y) : "cc");	
	}
	else
	{
		__asm__ ("addw %5, %0\nsetc %1\nseto %2\nsets %3" : "=r" (result), "=qQm" (carry_out), "=qQm" (overflow), "=qQm" (sign) : "0" (x), "rm" (y) : "cc");
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
	sign = (int16_t) res < 0;
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
inline void SignedAdd32(uint32_t& result, uint8_t& carry_out, uint8_t& overflow, uint8_t& sign, uint32_t x, uint32_t y, uint8_t carry_in)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && (defined(__i386) || defined(__x86_64))
	if(carry_in)
	{
		__asm__ ("stc\nadcl %5, %0\nsetc %1\nseto %2\nsets %3" : "=r" (result), "=qQ" (carry_out), "=qQ" (overflow), "=qQ" (sign) : "0" (x), "rm" (y) : "cc");	
	}
	else
	{
		__asm__ ("addl %5, %0\nsetc %1\nseto %2\nsets %3" : "=r" (result), "=qQ" (carry_out), "=qQ" (overflow), "=qQ" (sign) : "0" (x), "rm" (y) : "cc");
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
	sign = (int32_t) res < 0;
#endif
}

void UnsignedAdd8(uint8_t& result, uint8_t& carry_out, uint8_t& overflow, uint8_t& sign, uint8_t x, uint8_t y, uint8_t carry_in)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && (defined(__i386) || defined(__x86_64))
	if(carry_in)
	{
		__asm__ ("stc\nadcb %4, %0\nsetc %1\nsets %2" : "=qQ" (result), "=qQm" (carry_out), "=qQ" (sign) : "0" (x), "qQm" (y) : "cc");	
	}
	else
	{
		__asm__ ("addb %4, %0\nsetc %1\nsets %2" : "=qQ" (result), "=qQm" (carry_out), "=qQ" (sign) : "0" (x), "qQm" (y) : "cc");
	}
	overflow = carry_out;
#else
	uint8_t res = x + y + carry_in;
	uint8_t x7 = (x >> 7) & 1;
	uint8_t y7 = (y >> 7) & 1;
	uint8_t res7 = (res >> 7) & 1;
	uint8_t c6 = res7 ^ x7 ^ y7;
	uint8_t c7 = (x7 & y7) | (c6 & (x7 | y7));
	overflow = carry_out = c7;
	sign = (int8_t) res < 0;
	result = res;
#endif
}

void UnsignedAdd16(uint16_t& result, uint8_t& carry_out, uint8_t& overflow, uint8_t& sign, uint16_t x, uint16_t y, uint8_t carry_in)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && (defined(__i386) || defined(__x86_64))
	if(carry_in)
	{
		__asm__ ("stc\nadcw %4, %0\nsetc %1\nsets %2" : "=r" (result), "=qQm" (carry_out), "=qQ" (sign) : "0" (x), "rm" (y) : "cc");	
	}
	else
	{
		__asm__ ("addw %4, %0\nsetc %1\nsets %2" : "=r" (result), "=qQm" (carry_out), "=qQ" (sign) : "0" (x), "rm" (y) : "cc");
	}
	overflow = carry_out;
#else
	uint16_t res = x + y + carry_in;
	uint16_t x15 = (x >> 15) & 1;
	uint16_t y15 = (y >> 15) & 1;
	uint16_t res15 = (res >> 15) & 1;
	uint16_t c14 = res15 ^ x15 ^ y15;
	uint16_t c15 = (x15 & y15) | (c14 & (x15 | y15));
	overflow = carry_out = c15;
	sign = (int16_t) res < 0;
	result = res;
#endif
}

void UnsignedAdd32(uint32_t& result, uint8_t& carry_out, uint8_t& overflow, uint8_t& sign, uint32_t x, uint32_t y, uint8_t carry_in)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && (defined(__i386) || defined(__x86_64))
	if(carry_in)
	{
		__asm__ ("stc\nadcl %4, %0\nsetc %1\nsets %2" : "=r" (result), "=qQ" (carry_out), "=qQ" (sign) : "0" (x), "rm" (y) : "cc");	
	}
	else
	{
		__asm__ ("addl %4, %0\nsetc %1\nsets %2" : "=r" (result), "=qQ" (carry_out), "=qQ" (sign) : "0" (x), "rm" (y) : "cc");
	}
	overflow = carry_out;
#else
	uint32_t res = x + y + carry_in;
	uint32_t x31 = (x >> 31) & 1;
	uint32_t y31 = (y >> 31) & 1;
	uint32_t res31 = (res >> 31) & 1;
	uint32_t c30 = res31 ^ x31 ^ y31;
	uint32_t c31 = (x31 & y31) | (c30 & (x31 | y31));
	overflow = carry_out = c31;
	sign = (int32_t) res < 0;
	result = res;
#endif
}

//=============================================================================
//=                           Full Substractors                               =
//=============================================================================

inline void SignedSub8(uint8_t& result, uint8_t& borrow_out, uint8_t& overflow, uint8_t& sign, uint8_t x, uint8_t y, uint8_t borrow_in) {
#if defined(__GNUC__) && (__GNUC__ >= 3) && (defined(__i386) || defined(__x86_64))
	if(borrow_in)
	{
		__asm__ ("stc\nsbbb %5, %0\nsetc %1\nseto %2\nsets %3" : "=qQ" (result), "=qQm" (borrow_out), "=qQm" (overflow), "=qQm" (sign) : "0" (x), "q" (y) : "cc");	
	}
	else
	{
		__asm__ ("subb %5, %0\nsetc %1\nseto %2\nsets %3" : "=qQ" (result), "=qQm" (borrow_out), "=qQm" (overflow), "=qQm" (sign) : "0" (x), "q" (y) : "cc");
	}
#else
	uint8_t res = x - y - borrow_in;
	uint8_t x7 = (x >> 7) & 1;
	uint8_t y7 = (y >> 7) & 1;
	uint8_t res7 = (res >> 7) & 1;
	uint8_t b6 = res7 ^ x7 ^ y7;
	uint8_t b7 = ((~x7 & y7) | (b6 & (~x7 | y7))) & 1;
	overflow = b7 ^ b6;
	borrow_out = b7;
	result = res;
	sign = (int8_t) res < 0;
#endif
}

inline void SignedSub16(uint16_t& result, uint8_t& borrow_out, uint8_t& overflow, uint8_t& sign, uint16_t x, uint16_t y, uint8_t borrow_in) {
#if defined(__GNUC__) && (__GNUC__ >= 3) && (defined(__i386) || defined(__x86_64))
	if(borrow_in)
	{
		__asm__ ("stc\nsbbw %5, %0\nsetc %1\nseto %2\nsets %3" : "=r" (result), "=qQm" (borrow_out), "=qQm" (overflow), "=qQm" (sign) : "0" (x), "rm" (y) : "cc");	
	}
	else
	{
		__asm__ ("subw %5, %0\nsetc %1\nseto %2\nsets %3" : "=r" (result), "=qQm" (borrow_out), "=qQm" (overflow), "=qQm" (sign) : "0" (x), "rm" (y) : "cc");
	}
#else
	uint16_t res = x - y - borrow_in;
	uint16_t x15 = (x >> 15) & 1;
	uint8_t y15 = (y >> 15) & 1;
	uint8_t res15 = (res >> 15) & 1;
	uint8_t b14 = res15 ^ x15 ^ y15;
	uint8_t b15 = ((~x15 & y15) | (b14 & (~x15 | y15))) & 1;
	overflow = b15 ^ b14;
	borrow_out = b15;
	result = res;
	sign = (int16_t) res < 0;
#endif
}

/* The 31th 1-bit full substractor equations are:
   (1) result(31) = x(31) ^ y(31) ^ borrow(30)
   (2) borrow(31) = ~x(31).y(31) + borrow(30).(~x(31) + y(31))
   (3) overflow = borrow(31) ^ borrow(30)
   result is easily computed from x - y - borrow_in
   from (1) we obtain: borrow(30) = result(31) ^ x(31) ^ y(31)
   borrow_out is computed from equation (2), overflow from equation (3)
*/
inline void SignedSub32(uint32_t& result, uint8_t& borrow_out, uint8_t& overflow, uint8_t& sign, uint32_t x, uint32_t y, uint8_t borrow_in) {
#if defined(__GNUC__) && (__GNUC__ >= 3) && (defined(__i386) || defined(__x86_64))
	if(borrow_in)
	{
		__asm__ ("stc\nsbbl %5, %0\nsetc %1\nseto %2\nsets %3" : "=r" (result), "=qQ" (borrow_out), "=qQ" (overflow), "=qQ" (sign) : "0" (x), "rm" (y) : "cc");	
	}
	else
	{
		__asm__ ("subl %5, %0\nsetc %1\nseto %2\nsets %3" : "=r" (result), "=qQ" (borrow_out), "=qQ" (overflow), "=qQ" (sign) : "0" (x), "rm" (y) : "cc");
	}
#else
	uint32_t res = x - y - borrow_in;
	uint32_t x31 = (x >> 31) & 1;
	uint32_t y31 = (y >> 31) & 1;
	uint32_t res31 = (res >> 31) & 1;
	uint32_t b30 = res31 ^ x31 ^ y31;
	uint32_t b31 = ((~x31 & y31) | (b30 & (~x31 | y31))) & 1;
	overflow = b31 ^ b30;
	borrow_out = b31;
	result = res;
	sign = (int32_t) res < 0;
#endif
}

//=============================================================================
//=                    Full Adders with signed saturation                     =
//=============================================================================

inline void SignedSatAdd8(uint8_t& result, uint8_t& carry_out, uint8_t& overflow, uint8_t& sign, uint8_t x, uint8_t y, uint8_t carry_in)
{
	SignedAdd8(result, carry_out, overflow, sign, x, y, carry_in);
	if(overflow)
	{
		sign = !sign;
		result = sign ? 0x7f : 0x80;
	}
}

inline void SignedSatAdd16(uint16_t& result, uint8_t& carry_out, uint8_t& overflow, uint8_t& sign, uint16_t x, uint16_t y, uint8_t carry_in)
{
	SignedAdd16(result, carry_out, overflow, sign, x, y, carry_in);
	if(overflow)
	{
		result = sign ? 0x7fff : 0x8000;
		sign = !sign;
	}
}

inline void SignedSatAdd32(uint32_t& result, uint8_t& carry_out, uint8_t& overflow, uint8_t& sign, uint32_t x, uint32_t y, uint8_t carry_in)
{
	SignedAdd32(result, carry_out, overflow, sign, x, y, carry_in);
	if(overflow)
	{
		result = sign ? 0x7fffffff : 0x80000000;
		sign = !sign;
	}
}

inline void SignedSatAdd16(uint16_t& result, uint8_t& overflow, uint16_t x, uint16_t y) 
{
	uint8_t carry_out;
	uint8_t sign;
	SignedSatAdd16(result, carry_out, overflow, sign, x, y, 0);
}

inline void SignedSatAdd32(uint32_t& result, uint8_t& overflow, uint32_t x, uint32_t y) 
{
	uint8_t carry_out;
	uint8_t sign;
	SignedSatAdd32(result, carry_out, overflow, sign, x, y, 0);
}

//=============================================================================
//=                   Full Adders with unsigned saturation                    =
//=============================================================================

inline void UnsignedSatAdd8(uint8_t& result, uint8_t& carry_out, uint8_t& overflow, uint8_t& sign, uint8_t x, uint8_t y, uint8_t carry_in)
{
	UnsignedAdd8(result, carry_out, overflow, sign, x, y, carry_in);
	if(overflow)
	{
		result = 0xff;
		sign = 1;
	}
}

inline void UnsignedSatAdd16(uint16_t& result, uint8_t& carry_out, uint8_t& overflow, uint8_t& sign, uint16_t x, uint16_t y, uint8_t carry_in)
{
	UnsignedAdd16(result, carry_out, overflow, sign, x, y, carry_in);
	if(overflow)
	{
		result = 0xffff;
		sign = 1;
	}
}

inline void UnsignedSatAdd32(uint32_t& result, uint8_t& carry_out, uint8_t& overflow, uint8_t& sign, uint32_t x, uint32_t y, uint8_t carry_in)
{
	UnsignedAdd32(result, carry_out, overflow, sign, x, y, carry_in);
	if(overflow)
	{
		result = 0xffffffffUL;
		sign = 1;
	}
}

//=============================================================================
//=                 Full Substractors with signed saturation                  =
//=============================================================================

inline void SignedSatSub8(uint8_t& result, uint8_t& borrow_out, uint8_t& overflow, uint8_t& sign, uint8_t x, uint8_t y, uint8_t borrow_in)
{
	SignedSub8(result, borrow_out, overflow, sign, x, y, borrow_in);
	if(overflow)
	{
		result = sign ? 0x7f : 0x80;
		sign = !sign;
	}
}

inline void SignedSatSub16(uint16_t& result, uint8_t& borrow_out, uint8_t& overflow, uint8_t& sign, uint16_t x, uint16_t y, uint8_t borrow_in)
{
	SignedSub16(result, borrow_out, overflow, sign, x, y, borrow_in);
	if(overflow)
	{
		result = sign ? 0x7fff : 0x8000;
		sign = !sign;
	}
}

inline void SignedSatSub32(uint32_t& result, uint8_t& borrow_out, uint8_t& overflow, uint8_t& sign, uint32_t x, uint32_t y, uint8_t borrow_in)
{
	SignedSub32(result, borrow_out, overflow, sign, x, y, borrow_in);
	if(overflow)
	{
		result = sign ? 0x7fffffff : 0x80000000;
		sign = !sign;
	}
}

inline void SignedSatSub16(uint16_t& result, uint8_t& overflow, uint16_t x, uint16_t y) 
{
	uint8_t borrow_out;
	uint8_t sign;
	SignedSatSub16(result, borrow_out, overflow, sign, x, y, 0);
}

inline void SignedSatSub32(uint32_t& result, uint8_t& overflow, uint32_t x, uint32_t y) 
{
	uint8_t borrow_out;
	uint8_t sign;
	SignedSatSub32(result, borrow_out, overflow, sign, x, y, 0);
}

//=============================================================================
//=                          Specific Adders                                  =
//=============================================================================
inline uint32_t ReverseCarryPropagationAdd(uint32_t x, uint32_t y)
{
	// Compute the result as if there were no carry generation and propagation at all
	uint32_t r = x ^ y;

	// Compute local carry generation
	uint32_t x_and_y = x & y;
	unsigned int n;

	// Compute the bit index of the first carry generation to save time
	if(!BitScanReverse(n, x_and_y)) return r;

	uint32_t mask = (1 << n);
	uint32_t c = 0;
	uint32_t x_or_y = x | y;

	// propagate carry through remaining bits
	do
	{
		// Update result with carry in
		r = r ^ (mask & c);

		// Compute carry out: carry_out = (x & y) | (carry_in & (x | y))
		c = ((x_and_y | (c & x_or_y)) & mask) >> 1;

		// Go to next bit
		mask = mask >> 1;
	} while(mask);
	return r;
}

//=============================================================================
//=                                Rotates                                    =
//=============================================================================

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
#if defined(__GNUC__) && (__GNUC__ >= 3) && (defined(__i386) || defined(__x86_64))
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
#if defined(__GNUC__) && (__GNUC__ >= 3) && (defined(__i386) || defined(__x86_64))
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

// WARNING! bit_out value is undefined when rotating of 0 bits
inline uint32_t RotateLeft(uint32_t v, unsigned int n, uint8_t& bit_out)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && (defined(__i386) || defined(__x86_64))
	__asm__ ("rol %%cl, %0\nsetc %1" : "=r" (v), "=qQ" (bit_out) : "0" (v), "c" (n) : "cc");	
	return v;
#else
	n &= 31;
	bit_out = (v >> (32 - n)) & 1;
	return (v << n) | (v >> (32 - n));
#endif
}

inline uint32_t RotateRight(uint32_t v, unsigned int n, uint8_t& bit_out)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && (defined(__i386) || defined(__x86_64))
	__asm__ ("ror %%cl, %0\nsetc %1" : "=r" (v), "=qQ" (bit_out) : "0" (v), "c" (n) : "cc");	
	return v;
#else
	n &= 31;
	bit_out = (v >> (n - 1)) & 1;
	return (v >> n) | (v << (32 - n));
#endif
}

inline uint32_t RotateLeft(uint32_t v, unsigned int n, uint8_t bit_in, uint8_t& bit_out)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && (defined(__i386) || defined(__x86_64))
	if(bit_in)
	{
		__asm__ ("stc\nrcl %%cl, %0\nsetc %1" : "=r" (v), "=qQ" (bit_out) : "0" (v), "c" (n) : "cc");	
	}
	else
	{
		__asm__ ("clc\nrcl %%cl, %0\nsetc %1" : "=r" (v), "=qQ" (bit_out) : "0" (v), "c" (n) : "cc");	
	}
	return v;
#else
	n &= 31;
	bit_out = (v >> (32 - n)) & 1;
	return (uint32_t) (((uint64_t) v << n) | ((uint64_t) bit_in << (n - 1)) | ((uint64_t) v >> (33 - n)));
#endif
}

inline uint32_t RotateRight(uint32_t v, unsigned int n, uint8_t bit_in, uint8_t& bit_out)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && (defined(__i386) || defined(__x86_64))
	if(bit_in)
	{
		__asm__ ("stc\nrcr %%cl, %0\nsetc %1" : "=r" (v), "=qQ" (bit_out) : "0" (v), "c" (n) : "cc");	
	}
	else
	{
		__asm__ ("clc\nrcr %%cl, %0\nsetc %1" : "=r" (v), "=qQ" (bit_out) : "0" (v), "c" (n) : "cc");	
	}
	return v;
#else
	n &= 31;
	bit_out = (v >> (n - 1)) & 1;
	return (uint32_t) (((uint64_t) v >> n) | ((uint64_t) bit_in << (32 - n)) | ((uint64_t) v << (33 - n)));
#endif
}

//=============================================================================
//=                            Logical Shifts                                 =
//=============================================================================

inline uint32_t ShiftLeft(uint32_t v, unsigned int n)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && (defined(__i386) || defined(__x86_64))
	__asm__ ("shl %%cl, %0" : "=r" (v) : "0" (v), "c" (n) : "cc");	
	return v;
#else
	n &= 31;
	return v << n;
#endif
}

inline uint32_t ShiftLeft(uint32_t v, unsigned int n, uint8_t& bit_out)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && (defined(__i386) || defined(__x86_64))
	__asm__ ("shl %%cl, %0\nsetc %1" : "=r" (v), "=qQ" (bit_out): "0" (v), "c" (n) : "cc");	
	return v;
#else
	n &= 31;
	bit_out = (v >> (32 - n)) & 1;
	return v << n;
#endif
}

inline uint32_t ShiftRight(uint32_t v, unsigned int n)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && (defined(__i386) || defined(__x86_64))
	__asm__ ("shr %%cl, %0" : "=r" (v) : "0" (v), "c" (n) : "cc");
	return v;
#else
	n &= 31;
	return v >> n;
#endif
}

inline uint32_t ShiftRight(uint32_t v, unsigned int n, uint8_t& bit_out)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && (defined(__i386) || defined(__x86_64))
	__asm__ ("shr %%cl, %0\nsetc %1" : "=r" (v), "=qQ" (bit_out) : "0" (v), "c" (n) : "cc");	
	return v;
#else
	n &= 31;
	bit_out = (v >> (n - 1)) & 1;
	return v >> n;
#endif
}

//=============================================================================
//=                         Arithmetic Shifts                                 =
//=============================================================================

inline uint32_t ShiftArithmeticRight(uint32_t v, unsigned int n)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && (defined(__i386) || defined(__x86_64))
	__asm__ ("sar %%cl, %0" : "=r" (v) : "0" (v), "c" (n) : "cc");	
	return v;
#else
	n &= 31;
	return (uint32_t)((signed) v >> n);
#endif
}

inline uint32_t ShiftArithmeticRight(uint32_t v, unsigned int n, uint8_t& bit_out)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && (defined(__i386) || defined(__x86_64))
	__asm__ ("sar %%cl, %0\nsetc %1" : "=r" (v), "=qQ" (bit_out) : "0" (v), "c" (n) : "cc");	
	return v;
#else
	n &= 31;
	bit_out = (v >> (n - 1)) & 1;
	return (uint32_t)((signed) v >> n);
#endif
}

//=============================================================================
//=                             Bit Scanning                                  =
//=============================================================================

inline bool BitScanForward(unsigned int& n, uint32_t v)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && (defined(__i386) || defined(__x86_64))
	bool notfound;
	asm("bsf %2, %0\nsetz %1" : "=r" (n), "=qQ" (notfound) : "r" (v) : "cc");
	return !notfound;
#else
	if(!v) return false;

	unsigned int i = 0;
	if(!(v & 1))
	{
		do
		{
			i++;
			v = v >> 1;
		} while(!(v & 1));
	}
	n = i;
	return true;
#endif
}

inline bool BitScanForward(unsigned int& n, uint64_t v)
{
	unsigned int tmp_n;
	if(BitScanForward(tmp_n, (uint32_t) v))
	{
		n = tmp_n;
		return true;
	}
	if(BitScanForward(tmp_n, (uint32_t)(v >> 32)))
	{
		n = tmp_n + 32;
		return true;
	}
	return false;
}

inline bool BitScanReverse(unsigned int& n, uint32_t v)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && (defined(__i386) || defined(__x86_64))
	bool notfound;
	asm("bsr %2, %0\nsetz %1" : "=r" (n), "=qQ" (notfound) : "r" (v) : "cc");
	return !notfound;
#else
	if(!v) return false;

	unsigned int i = 31;
	if(!(v & 0x80000000))
	{
		do
		{
			i--;
			v = v << 1;
		} while(!(v & 0x80000000));
	}
	n = i;
	return true;
#endif
}

inline bool BitScanReverse(unsigned int& n, uint64_t v)
{
	unsigned int tmp_n;
	if(BitScanReverse(tmp_n, (uint32_t)(v >> 32)))
	{
		n = tmp_n + 32;
		return true;
	}
	if(BitScanReverse(tmp_n, (uint32_t) v))
	{
		n = tmp_n;
		return true;
	}
	return false;
}

inline unsigned int CountLeadingZeros(uint32_t v)
{
	unsigned int n;
	return BitScanReverse(n, v) ? 31 - n : 32;
}

inline unsigned int CountLeadingZeros(uint64_t v)
{
	unsigned int n;
	return BitScanReverse(n, v) ? 63 - n : 64;
}

//=============================================================================
//=                              Base 2 Logarithm                             =
//=============================================================================

inline unsigned int Log2(uint32_t v)
{
	unsigned int n;
	return BitScanReverse(n, v) ? n : 0;
}

inline unsigned int Log2(uint64_t v)
{
	unsigned int n;
	return BitScanReverse(n, v) ? n : 0;
}

inline unsigned int CeilLog2(uint32_t v)
{
	unsigned int log2v = Log2(v);
	return (v > (1ULL << log2v)) ? log2v + 1 : log2v;
}

inline unsigned int CeilLog2(uint64_t v)
{
	unsigned int log2v = Log2(v);
	return (v > (1ULL << log2v)) ? log2v + 1 : log2v;
}

//=============================================================================
//=                       2's complement sign Extension                       =
//=============================================================================

inline int8_t SignExtend(uint8_t v, unsigned int n)
{
	unsigned int m = (8 * sizeof(v)) - n;
	return (int8_t) (v << m) >> m; 
}

inline int16_t SignExtend(uint16_t v, unsigned int n)
{
	unsigned int m = (8 * sizeof(v)) - n;
	return (int16_t) (v << m) >> m; 
}

inline int32_t SignExtend(uint32_t v, unsigned int n)
{
	unsigned int m = (8 * sizeof(v)) - n;
	return (int32_t) (v << m) >> m; 
}

inline int64_t SignExtend(uint64_t v, unsigned int n)
{
	unsigned int m = (8 * sizeof(v)) - n;
	return (int64_t) (v << m) >> m; 
}

//=============================================================================
//=                                  Parity                                   =
//=============================================================================

inline void EvenParity(uint8_t v, uint8_t& parity_out)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && (defined(__i386) || defined(__x86_64))
	__asm__ ("orb %1, %1\nsetp %0\n" : "=qQm" (parity_out) : "qQ" (v) : "cc");
#else
	unsigned int n = 8 * sizeof(v);
	uint8_t par = 0;
	do
	{
		par = par ^ v;
	}
	while(v >>= 1,--n);
	parity_out = par & 1;
#endif
}

inline void EvenParity(uint16_t v, uint8_t& parity_out)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && (defined(__i386) || defined(__x86_64))
	__asm__ ("orw %1, %1\nsetp %0\n" : "=qQm" (parity_out) : "r" (v) : "cc");
#else
	unsigned int n = 8 * sizeof(v);
	uint8_t par = 0;
	do
	{
		par = par ^ v;
	}
	while(v >>= 1,--n);
	parity_out = par & 1;
#endif
}

inline void EvenParity(uint32_t v, uint8_t& parity_out)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && (defined(__i386) || defined(__x86_64))
	__asm__ ("orl %1, %1\nsetp %0\n" : "=qQm" (parity_out) : "r" (v) : "cc");
#else
	unsigned int n = 8 * sizeof(v);
	uint8_t par = 0;
	do
	{
		par = par ^ v;
	}
	while(v >>= 1,--n);
	parity_out = par & 1;
#endif
}

inline void OddParity(uint8_t v, uint8_t& parity_out)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && (defined(__i386) || defined(__x86_64))
	__asm__ ("orb %1, %1\nsetnp %0\n" : "=qQm" (parity_out) : "qQ" (v) : "cc");
#else
	unsigned int n = 8 * sizeof(v);
	uint8_t par = 0;
	do
	{
		par = par ^ v;
	}
	while(v >>= 1,--n);
	parity_out = par & 1;
#endif
}

inline void OddParity(uint16_t v, uint8_t& parity_out)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && (defined(__i386) || defined(__x86_64))
	__asm__ ("orw %1, %1\nsetnp %0\n" : "=qQm" (parity_out) : "r" (v) : "cc");
#else
	unsigned int n = 8 * sizeof(v);
	uint8_t par = 0;
	do
	{
		par = par ^ v;
	}
	while(v >>= 1,--n);
	parity_out = par & 1;
#endif
}

inline void OddParity(uint32_t v, uint8_t& parity_out)
{
#if defined(__GNUC__) && (__GNUC__ >= 3) && (defined(__i386) || defined(__x86_64))
	__asm__ ("orl %1, %1\nsetnp %0\n" : "=qQm" (parity_out) : "r" (v) : "cc");
#else
	unsigned int n = 8 * sizeof(v);
	uint8_t par = 0;
	do
	{
		par = par ^ v;
	}
	while(v >>= 1,--n);
	parity_out = par & 1;
#endif
}

} // end of namespace arithmetic
} // end of namespace util
} // end of namespace unisim

#endif
