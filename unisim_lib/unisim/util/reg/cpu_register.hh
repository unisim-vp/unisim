/*
 *  Copyright (c) 2017,
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
 *          Yves Lhuillier (yves.lhuillier@cea.fr)
 */

/* Example of use:
 * 
 * struct MR : Register<32>
 * {
 *     typedef BitField<31, 2> A;
 *     typedef BitField<26>    B; // implicitely one bit length bitfield
 *     typedef BitField<11, 3> C;
 *     typedef BitField<0, 5>  D;
 * 
 *     typedef BitFieldSet<A, B, C, D> ALL;
 * };
 * 
 * MR mr(0);
 * 
 * mr.Set<MR::B>(1);
 * 
 * MR::TYPE mr_val = mr.Get<MR::ALL>();
 */

#ifndef __UNISIM_UTIL_REG_CPU_REGISTER_HH__
#define __UNISIM_UTIL_REG_CPU_REGISTER_HH__

#include <iostream>
#include <inttypes.h>

namespace unisim {
namespace util {
namespace reg {

template <int BYTE_SIZE> struct TypeForByteSize {};
template <> struct TypeForByteSize<1> { typedef uint8_t TYPE; };
template <> struct TypeForByteSize<2> { typedef uint16_t TYPE; };
template <> struct TypeForByteSize<3> { typedef uint32_t TYPE; };
template <> struct TypeForByteSize<4> { typedef uint32_t TYPE; };
template <> struct TypeForByteSize<5> { typedef uint64_t TYPE; };
template <> struct TypeForByteSize<6> { typedef uint64_t TYPE; };
template <> struct TypeForByteSize<7> { typedef uint64_t TYPE; };
template <> struct TypeForByteSize<8> { typedef uint64_t TYPE; };

template <int BITSIZE> struct TypeForBitSize
{
	static const int BYTE_SIZE = (BITSIZE + 7) / 8;
	typedef typename TypeForByteSize<BYTE_SIZE>::TYPE TYPE;
	static const TYPE MASK = (BITSIZE == (8 * BYTE_SIZE)) ? (~TYPE(0)) : ((TYPE(1) << BITSIZE) - 1);
};

template <unsigned int _BITSIZE>
class Register;

template <unsigned int _BITSIZE>
std::ostream& operator << (std::ostream& os, const Register<_BITSIZE>& reg);

template <unsigned int _BITSIZE>
class Register
{
public:
	typedef typename TypeForBitSize<_BITSIZE>::TYPE TYPE;
	static const TYPE MASK = TypeForBitSize<_BITSIZE>::MASK;
	static const unsigned int BITSIZE = _BITSIZE;
	
	template <typename BITFIELD> TYPE Get() const;
	template <typename BITFIELD> void Set(TYPE bitfield_value);
	
	Register();
	Register(TYPE value);
	Register(const Register<_BITSIZE>& reg);
	Register<_BITSIZE>& operator = (const TYPE& value);
	Register<_BITSIZE>& operator = (const Register<_BITSIZE>& reg);
	operator TYPE () const;
	
	friend std::ostream& operator << <_BITSIZE>(std::ostream& os, const Register<_BITSIZE>& reg);
	
private:
	TYPE value;
};

template <unsigned int _BITSIZE>
template <typename BITFIELD> typename Register<_BITSIZE>::TYPE Register<_BITSIZE>::Get() const
{
	return BITFIELD::Get(value);
}

template <unsigned int _BITSIZE>
template <typename BITFIELD> void Register<_BITSIZE>::Set(TYPE bitfield_value)
{
	BITFIELD::Set(value, bitfield_value);	
}

template <unsigned int _BITSIZE>
Register<_BITSIZE>::Register()
	: value(0)
{
}

template <unsigned int _BITSIZE>
Register<_BITSIZE>::Register(TYPE _value)
	: value(_value & MASK)
{
}

template <unsigned int _BITSIZE>
Register<_BITSIZE>::Register(const Register<_BITSIZE>& reg)
	: value(reg.value & MASK)
{
}

template <unsigned int _BITSIZE>
Register<_BITSIZE>& Register<_BITSIZE>::operator = (const TYPE& _value)
{
	value = _value & MASK;
	return *this;
}

template <unsigned int _BITSIZE>
Register<_BITSIZE>& Register<_BITSIZE>::operator = (const Register<_BITSIZE>& reg)
{
	value = reg.value & MASK;
	return *this;
}

template <unsigned int _BITSIZE>
Register<_BITSIZE>::operator TYPE () const
{
	return value;
}

template <unsigned int _BITSIZE>
std::ostream& operator << (std::ostream& os, const Register<_BITSIZE>& reg)
{
	return os << reg.value;
}

template <unsigned int OFFSET, unsigned int BITSIZE = 1>
class BitField
{
public:
	static const unsigned int BITSIZE_MINUS_1 = BITSIZE - 1;

	template <typename T> static T GetMask();
	template <typename T> static T Mask(const T& storage);
	template <typename T> static T Get(const T& storage);
	template <typename T> static void Set(T& storage, const T& bitfied_value);
};

template <unsigned int OFFSET, unsigned int BITSIZE>
template <typename T> T BitField<OFFSET, BITSIZE>::GetMask()
{
	return ((T(2) << (BITSIZE_MINUS_1)) - T(1)) << OFFSET;
}

template <unsigned int OFFSET, unsigned int BITSIZE>
template <typename T> T BitField<OFFSET, BITSIZE>::Mask(const T& storage)
{
	return storage & GetMask<T>();
}

template <unsigned int OFFSET, unsigned int BITSIZE>
template <typename T>
T BitField<OFFSET, BITSIZE>::Get(const T& storage)
{
	return (storage & GetMask<T>()) >> OFFSET;
}

template <unsigned int OFFSET, unsigned int BITSIZE>
template <typename T>
void BitField<OFFSET, BITSIZE>::Set(T& storage, const T& bitfied_value)
{
	storage = (storage & ~GetMask<T>()) | ((bitfied_value << OFFSET) & GetMask<T>());
}

struct NullBitField
{
	template <typename T> static T GetMask();
};

template <typename T>
T NullBitField::GetMask()
{
	return 0;
}

template < typename  BF0               , typename  BF1 = NullBitField, typename  BF2 = NullBitField, typename  BF3 = NullBitField
         , typename  BF4 = NullBitField, typename  BF5 = NullBitField, typename  BF6 = NullBitField, typename  BF7 = NullBitField
         , typename  BF8 = NullBitField, typename  BF9 = NullBitField, typename BF10 = NullBitField, typename BF11 = NullBitField
         , typename BF12 = NullBitField, typename BF13 = NullBitField, typename BF14 = NullBitField, typename BF15 = NullBitField
         , typename BF16 = NullBitField, typename BF17 = NullBitField, typename BF18 = NullBitField, typename BF19 = NullBitField
         , typename BF20 = NullBitField, typename BF21 = NullBitField, typename BF22 = NullBitField, typename BF23 = NullBitField
         , typename BF24 = NullBitField, typename BF25 = NullBitField, typename BF26 = NullBitField, typename BF27 = NullBitField
         , typename BF28 = NullBitField, typename BF29 = NullBitField, typename BF30 = NullBitField, typename BF31 = NullBitField
         , typename BF32 = NullBitField, typename BF33 = NullBitField, typename BF34 = NullBitField, typename BF35 = NullBitField
         , typename BF36 = NullBitField, typename BF37 = NullBitField, typename BF38 = NullBitField, typename BF39 = NullBitField
         , typename BF40 = NullBitField, typename BF41 = NullBitField, typename BF42 = NullBitField, typename BF43 = NullBitField
         , typename BF44 = NullBitField, typename BF45 = NullBitField, typename BF46 = NullBitField, typename BF47 = NullBitField
         , typename BF48 = NullBitField, typename BF49 = NullBitField, typename BF50 = NullBitField, typename BF51 = NullBitField
         , typename BF52 = NullBitField, typename BF53 = NullBitField, typename BF54 = NullBitField, typename BF55 = NullBitField
         , typename BF56 = NullBitField, typename BF57 = NullBitField, typename BF58 = NullBitField, typename BF59 = NullBitField
         , typename BF60 = NullBitField, typename BF61 = NullBitField, typename BF62 = NullBitField, typename BF63 = NullBitField>
struct BitFieldSet
{
	template <typename T> static T GetMask();
	template <typename T> static T Get(const T& storage);
	template <typename T> static void Set(T& storage, const T& bitfied_value);
};

template < typename  BF0, typename  BF1, typename  BF2, typename  BF3 
         , typename  BF4, typename  BF5, typename  BF6, typename  BF7 
         , typename  BF8, typename  BF9, typename BF10, typename BF11
         , typename BF12, typename BF13, typename BF14, typename BF15
         , typename BF16, typename BF17, typename BF18, typename BF19
         , typename BF20, typename BF21, typename BF22, typename BF23
         , typename BF24, typename BF25, typename BF26, typename BF27
         , typename BF28, typename BF29, typename BF30, typename BF31
         , typename BF32, typename BF33, typename BF34, typename BF35
         , typename BF36, typename BF37, typename BF38, typename BF39
         , typename BF40, typename BF41, typename BF42, typename BF43
         , typename BF44, typename BF45, typename BF46, typename BF47
         , typename BF48, typename BF49, typename BF50, typename BF51
         , typename BF52, typename BF53, typename BF54, typename BF55
         , typename BF56, typename BF57, typename BF58, typename BF59
         , typename BF60, typename BF61, typename BF62, typename BF63>
template <typename T> T BitFieldSet<  BF0,  BF1,  BF2,  BF3,  BF4,  BF5,  BF6,  BF7
                                   ,  BF8,  BF9, BF10, BF11, BF12, BF13, BF14, BF15
                                   , BF16, BF17, BF18, BF19, BF20, BF21, BF22, BF23
                                   , BF24, BF25, BF26, BF27, BF28, BF29, BF30, BF31
                                   , BF32, BF33, BF34, BF35, BF36, BF37, BF38, BF39
                                   , BF40, BF41, BF42, BF43, BF44, BF45, BF46, BF47
                                   , BF48, BF49, BF50, BF51, BF52, BF53, BF54, BF55
                                   , BF56, BF57, BF58, BF59, BF60, BF61, BF62, BF63>::GetMask()
{
	return  BF0::template GetMask<T>() |  BF1::template GetMask<T>() |  BF2::template GetMask<T>() |  BF3::template GetMask<T>() |  BF4::template GetMask<T>() |  BF5::template GetMask<T>() |  BF6::template GetMask<T>() |  BF7::template GetMask<T>()
	     |  BF8::template GetMask<T>() |  BF9::template GetMask<T>() | BF10::template GetMask<T>() | BF11::template GetMask<T>() | BF12::template GetMask<T>() | BF13::template GetMask<T>() | BF14::template GetMask<T>() | BF15::template GetMask<T>()
		 | BF16::template GetMask<T>() | BF17::template GetMask<T>() | BF18::template GetMask<T>() | BF19::template GetMask<T>() | BF20::template GetMask<T>() | BF21::template GetMask<T>() | BF22::template GetMask<T>() | BF23::template GetMask<T>()
		 | BF24::template GetMask<T>() | BF25::template GetMask<T>() | BF26::template GetMask<T>() | BF27::template GetMask<T>() | BF28::template GetMask<T>() | BF29::template GetMask<T>() | BF30::template GetMask<T>() | BF31::template GetMask<T>()
		 | BF32::template GetMask<T>() | BF33::template GetMask<T>() | BF34::template GetMask<T>() | BF35::template GetMask<T>() | BF36::template GetMask<T>() | BF37::template GetMask<T>() | BF38::template GetMask<T>() | BF39::template GetMask<T>()
		 | BF40::template GetMask<T>() | BF41::template GetMask<T>() | BF42::template GetMask<T>() | BF43::template GetMask<T>() | BF44::template GetMask<T>() | BF45::template GetMask<T>() | BF46::template GetMask<T>() | BF47::template GetMask<T>()
		 | BF48::template GetMask<T>() | BF49::template GetMask<T>() | BF50::template GetMask<T>() | BF51::template GetMask<T>() | BF52::template GetMask<T>() | BF53::template GetMask<T>() | BF54::template GetMask<T>() | BF55::template GetMask<T>()
		 | BF56::template GetMask<T>() | BF57::template GetMask<T>() | BF58::template GetMask<T>() | BF59::template GetMask<T>() | BF60::template GetMask<T>() | BF61::template GetMask<T>() | BF62::template GetMask<T>() | BF63::template GetMask<T>();
}

template < typename  BF0, typename  BF1, typename  BF2, typename  BF3 
         , typename  BF4, typename  BF5, typename  BF6, typename  BF7 
         , typename  BF8, typename  BF9, typename BF10, typename BF11
         , typename BF12, typename BF13, typename BF14, typename BF15
         , typename BF16, typename BF17, typename BF18, typename BF19
         , typename BF20, typename BF21, typename BF22, typename BF23
         , typename BF24, typename BF25, typename BF26, typename BF27
         , typename BF28, typename BF29, typename BF30, typename BF31
         , typename BF32, typename BF33, typename BF34, typename BF35
         , typename BF36, typename BF37, typename BF38, typename BF39
         , typename BF40, typename BF41, typename BF42, typename BF43
         , typename BF44, typename BF45, typename BF46, typename BF47
         , typename BF48, typename BF49, typename BF50, typename BF51
         , typename BF52, typename BF53, typename BF54, typename BF55
         , typename BF56, typename BF57, typename BF58, typename BF59
         , typename BF60, typename BF61, typename BF62, typename BF63>
template <typename T> T BitFieldSet<  BF0,  BF1,  BF2,  BF3,  BF4,  BF5,  BF6,  BF7
                                   ,  BF8,  BF9, BF10, BF11, BF12, BF13, BF14, BF15
                                   , BF16, BF17, BF18, BF19, BF20, BF21, BF22, BF23
                                   , BF24, BF25, BF26, BF27, BF28, BF29, BF30, BF31
                                   , BF32, BF33, BF34, BF35, BF36, BF37, BF38, BF39
                                   , BF40, BF41, BF42, BF43, BF44, BF45, BF46, BF47
                                   , BF48, BF49, BF50, BF51, BF52, BF53, BF54, BF55
                                   , BF56, BF57, BF58, BF59, BF60, BF61, BF62, BF63>::Get(const T& storage)
{
	return storage & GetMask<T>();
}

template < typename  BF0, typename  BF1, typename  BF2, typename  BF3 
         , typename  BF4, typename  BF5, typename  BF6, typename  BF7 
         , typename  BF8, typename  BF9, typename BF10, typename BF11
         , typename BF12, typename BF13, typename BF14, typename BF15
         , typename BF16, typename BF17, typename BF18, typename BF19
         , typename BF20, typename BF21, typename BF22, typename BF23
         , typename BF24, typename BF25, typename BF26, typename BF27
         , typename BF28, typename BF29, typename BF30, typename BF31
         , typename BF32, typename BF33, typename BF34, typename BF35
         , typename BF36, typename BF37, typename BF38, typename BF39
         , typename BF40, typename BF41, typename BF42, typename BF43
         , typename BF44, typename BF45, typename BF46, typename BF47
         , typename BF48, typename BF49, typename BF50, typename BF51
         , typename BF52, typename BF53, typename BF54, typename BF55
         , typename BF56, typename BF57, typename BF58, typename BF59
         , typename BF60, typename BF61, typename BF62, typename BF63>
template <typename T> void BitFieldSet<  BF0,  BF1,  BF2,  BF3,  BF4,  BF5,  BF6,  BF7
                                      ,  BF8,  BF9, BF10, BF11, BF12, BF13, BF14, BF15
                                      , BF16, BF17, BF18, BF19, BF20, BF21, BF22, BF23
                                      , BF24, BF25, BF26, BF27, BF28, BF29, BF30, BF31
                                      , BF32, BF33, BF34, BF35, BF36, BF37, BF38, BF39
                                      , BF40, BF41, BF42, BF43, BF44, BF45, BF46, BF47
                                      , BF48, BF49, BF50, BF51, BF52, BF53, BF54, BF55
                                      , BF56, BF57, BF58, BF59, BF60, BF61, BF62, BF63>::Set(T& storage, const T& bitfiedset_value)
{
	storage = (storage & ~GetMask<T>()) | (bitfiedset_value & GetMask<T>());
}

} // end of namespace reg
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_REG_CPU_REGISTER_HH__
