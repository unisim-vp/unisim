/*
 *  Copyright (c) 2016,
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

#ifndef __UNISIM_COMPONENT_TLM2_MEMORY_FLASH_N25Q_REGISTER_HH__
#define __UNISIM_COMPONENT_TLM2_MEMORY_FLASH_N25Q_REGISTER_HH__

#include <string>
#include <iostream>
#include <inttypes.h>

namespace unisim {
namespace component {
namespace tlm2 {
namespace memory {
namespace flash {
namespace n25q {

enum BitOrdering
{
	BO_BIG_ENDIAN,
	BO_LITTLE_ENDIAN
};

enum Access
{
	ACC_NONE = 0,
	ACC_RO   = 1,
	ACC_WO   = 2,
	ACC_RW   = 3
};

template <int BIT_SIZE>
struct Storage
{
};

template <>
struct Storage<2>
{
	typedef uint8_t TYPE;
	static const TYPE MASK = 0x3;
	TYPE value;
	
	Storage() : value() {}
	Storage(TYPE initial_value) : value(initial_value & MASK) {}
};

template <>
struct Storage<8>
{
	typedef uint8_t TYPE;
	static const TYPE MASK = 0xff;
	TYPE value;
	
	Storage() : value() {}
	Storage(TYPE initial_value) : value(initial_value & MASK) {}
};

template <>
struct Storage<16>
{
	typedef uint16_t TYPE;
	static const TYPE MASK = 0xffff;
	TYPE value;

	Storage() : value() {}
	Storage(TYPE initial_value) : value(initial_value & MASK) {}
};

template <>
struct Storage<32>
{
	typedef uint32_t TYPE;
	static const TYPE MASK = 0xffffffffUL;
	TYPE value;

	Storage() : value() {}
	Storage(TYPE initial_value) : value(initial_value & MASK) {}
};

template <>
struct Storage<64>
{
	typedef uint64_t TYPE;
	static const TYPE MASK = 0xffffffffffffffffULL;
	TYPE value;

	Storage() : value() {}
	Storage(TYPE initial_value) : value(initial_value & MASK) {}
};

class RegisterBase
{
public:
	RegisterBase(const std::string& name, const std::string& long_name = std::string());
	const std::string& GetName() const;
	const std::string& GetLongName() const;
private:
	std::string name;
	std::string long_name;
};

RegisterBase::RegisterBase(const std::string& _name, const std::string& _long_name)
	: name(_name)
	, long_name(_long_name)
{
}

const std::string& RegisterBase::GetName() const
{
	return name;
}

const std::string& RegisterBase::GetLongName() const
{
	return long_name;
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, BitOrdering B_ORDER>
class Register;

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, BitOrdering B_ORDER>
std::ostream& operator << (std::ostream& os, const Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER>& reg);

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, BitOrdering B_ORDER>
class Register : public RegisterBase
{
public:
	typedef typename Storage<B_SIZE>::TYPE STORAGE_TYPE;
	static const int BIT_SIZE = B_SIZE;
	static const BitOrdering BIT_ORDERING = B_ORDER;
	Register(const std::string& name, const std::string& long_name = std::string());
	Register(const std::string& name, const STORAGE_TYPE& reset_value);
	Register(const std::string& name, const std::string& long_name, const STORAGE_TYPE& reset_value);
	virtual ~Register();
	
	void SetHW_WriteMask(STORAGE_TYPE mask);
	void SetHW_ReadMask(STORAGE_TYPE mask);
	void SetSW_WriteMask(STORAGE_TYPE mask);
	void SetSW_ReadMask(STORAGE_TYPE mask);
	STORAGE_TYPE GetHW_WriteMask() const;
	STORAGE_TYPE GetHW_ReadMask() const;
	STORAGE_TYPE GetSW_WriteMask() const;
	STORAGE_TYPE GetSW_ReadMask() const;
	void SetResetValue(STORAGE_TYPE value, STORAGE_TYPE mask);
	void Reset();
	operator const STORAGE_TYPE() const;
	Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER>& operator = (const STORAGE_TYPE& value);
	bool Write(const STORAGE_TYPE& value);
	bool Read(STORAGE_TYPE& value) const;
	
	friend std::ostream& operator << <B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER>(std::ostream& os, const Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER>& reg);
private:
	Storage<B_SIZE> storage;
	Storage<B_SIZE> reset_storage;
	Storage<B_SIZE> hw_write_mask_storage;
	Storage<B_SIZE> hw_read_mask_storage;
	Storage<B_SIZE> sw_write_mask_storage;
	Storage<B_SIZE> sw_read_mask_storage;
};

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, BitOrdering B_ORDER>
std::ostream& operator << (std::ostream& os, const Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER>& reg)
{
	return os << (uint64_t) reg.storage.value;
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, BitOrdering B_ORDER>
Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER>::Register(const std::string& name, const std::string& long_name)
	: RegisterBase(name, long_name)
	, storage()
	, reset_storage()
	, hw_write_mask_storage(0)
	, hw_read_mask_storage(0)
	, sw_write_mask_storage(0)
	, sw_read_mask_storage(0)
{
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, BitOrdering B_ORDER>
Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER>::Register(const std::string& name, const std::string& long_name, const STORAGE_TYPE& reset_value)
	: RegisterBase(name, long_name)
	, storage(reset_value)
	, reset_storage(reset_value)
	, hw_write_mask_storage(0)
	, hw_read_mask_storage(0)
	, sw_write_mask_storage(0)
	, sw_read_mask_storage(0)
{
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, BitOrdering B_ORDER>
Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER>::Register(const std::string& name, const STORAGE_TYPE& reset_value)
	: RegisterBase(name)
	, storage(reset_value)
	, reset_storage(reset_value)
	, hw_write_mask_storage(0)
	, hw_read_mask_storage(0)
	, sw_write_mask_storage(0)
	, sw_read_mask_storage(0)
{
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, BitOrdering B_ORDER>
Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER>::~Register()
{
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, BitOrdering B_ORDER>
void Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER>::SetHW_WriteMask(STORAGE_TYPE mask)
{
	hw_write_mask_storage.value |= mask;
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, BitOrdering B_ORDER>
void Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER>::SetHW_ReadMask(STORAGE_TYPE mask)
{
	hw_read_mask_storage.value |= mask;
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, BitOrdering B_ORDER>
void Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER>::SetSW_WriteMask(STORAGE_TYPE mask)
{
	sw_write_mask_storage.value |= mask;
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, BitOrdering B_ORDER>
void Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER>::SetSW_ReadMask(STORAGE_TYPE mask)
{
	sw_read_mask_storage.value |= mask;
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, BitOrdering B_ORDER>
typename Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER>::STORAGE_TYPE Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER>::GetHW_WriteMask() const
{
	return hw_write_mask_storage.value;
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, BitOrdering B_ORDER>
typename Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER>::STORAGE_TYPE Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER>::GetHW_ReadMask() const
{
	return hw_read_mask_storage.value;
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, BitOrdering B_ORDER>
typename Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER>::STORAGE_TYPE Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER>::GetSW_WriteMask() const
{
	return sw_write_mask_storage.value;
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, BitOrdering B_ORDER>
typename Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER>::STORAGE_TYPE Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER>::GetSW_ReadMask() const
{
	return sw_read_mask_storage.value;
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, BitOrdering B_ORDER>
void Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER>::SetResetValue(STORAGE_TYPE value, STORAGE_TYPE mask)
{
	reset_storage.value = ((reset_storage.value & ~mask) | (value & mask)) & reset_storage.MASK;
	Reset();
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, BitOrdering B_ORDER>
void Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER>::Reset()
{
	storage.value = reset_storage.value;
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, BitOrdering B_ORDER>
Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER>::operator const STORAGE_TYPE() const
{
	return ((reset_storage.value & ~hw_read_mask_storage.value) | (storage.value & hw_read_mask_storage.value)) & storage.MASK;
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, BitOrdering B_ORDER>
Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER>& Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER>::operator = (const STORAGE_TYPE& value)
{
	if(((reset_storage.value & hw_write_mask_storage.value) | (value & ~hw_write_mask_storage.value & storage.MASK)) != reset_storage.value)
	{
		// Attempt to modify reserved or read-only bits
		std::cerr << "Hardware attempt to modify reserved bits in Register \"" << GetName() << "\"" << std::endl;
	}

	storage.value = ((storage.value & ~hw_write_mask_storage.value) | (value & hw_write_mask_storage.value)) & storage.MASK;
	return *this;
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, BitOrdering B_ORDER>
bool Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER>::Write(const STORAGE_TYPE& value)
{
	if(SW_ACCESS & ACC_WO)
	{
		if((value & ~sw_write_mask_storage.value) != (reset_storage.value & ~sw_write_mask_storage.value)) // is write would modify reserved bits ?
		{
			// yes: attempt to modify reserved bits
			std::cerr << "Software attempt to modify reserved bits in Register \"" << GetName() << "\"" << std::endl;
		}

		STORAGE_TYPE old_value = this->operator const STORAGE_TYPE();
		STORAGE_TYPE new_value = (old_value & ~sw_write_mask_storage.value) | (value & sw_write_mask_storage.value);
		this->operator = (new_value);
		return true;
	}
	
	return false;
}

template <int B_SIZE, Access HW_ACCESS, Access SW_ACCESS, BitOrdering B_ORDER>
bool Register<B_SIZE, HW_ACCESS, SW_ACCESS, B_ORDER>::Read(STORAGE_TYPE& value) const
{
	if(SW_ACCESS & ACC_RO)
	{
		STORAGE_TYPE reg_value = this->operator const STORAGE_TYPE();
		value = reg_value & sw_read_mask_storage.value;
		return true;
	}
	
	return false;
}

class BitFieldBase
{
public:
	BitFieldBase(const std::string& name, RegisterBase *reg);
	virtual ~BitFieldBase();
	
	const std::string& GetName() const;
private:
	std::string name;
};

BitFieldBase::BitFieldBase(const std::string& _name, RegisterBase *reg)
	: name(reg->GetName() + '.' + _name)
{
}

BitFieldBase::~BitFieldBase()
{
}

const std::string& BitFieldBase::GetName() const
{
	return name;
}

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2>
class BitField;

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2>
std::ostream& operator << (std::ostream& os, const BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>& bf);

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2 = -1>
class BitField : public BitFieldBase
{
public:
	typedef typename REGISTER::STORAGE_TYPE STORAGE_TYPE;
	static const BitOrdering BIT_ORDERING = REGISTER::BIT_ORDERING;

	BitField(const std::string& name, REGISTER *reg);
	BitField(const std::string& name, REGISTER *reg, const STORAGE_TYPE& reset_value);
	virtual ~BitField();
	
	operator const STORAGE_TYPE () const;
	BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>& operator = (const STORAGE_TYPE& value);
	unsigned int Shift() const;
	unsigned int BitSize() const;
	STORAGE_TYPE Mask() const;
	STORAGE_TYPE HW_WriteMask() const;
	STORAGE_TYPE HW_ReadMask() const;
	STORAGE_TYPE SW_WriteMask() const;
	STORAGE_TYPE SW_ReadMask() const;
private:
	REGISTER *reg;
	
	friend std::ostream& operator << <REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>(std::ostream& os, const BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>& bf);
};

template <bool COND>
struct CompileTimeAssert
{
};

template <>
struct CompileTimeAssert<true>
{
	static void _() {}
};

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2>
std::ostream& operator << (std::ostream& os, const BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>& bf)
{
	return os << (uint64_t) bf.operator const typename BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::STORAGE_TYPE();
}

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2>
BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::BitField(const std::string& _name, REGISTER *_reg)
	: BitFieldBase(_name, _reg)
	, reg(_reg)
{
	CompileTimeAssert<(OFFSET1 >= 0) && (OFFSET1 < (int) REGISTER::BIT_SIZE)>::_();
	CompileTimeAssert<OFFSET2 < REGISTER::BIT_SIZE>::_();
	
	reg->SetHW_WriteMask(HW_WriteMask());
	reg->SetHW_ReadMask(HW_ReadMask());
	reg->SetSW_WriteMask(SW_WriteMask());
	reg->SetSW_ReadMask(SW_ReadMask());
}

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2>
BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::BitField(const std::string& _name, REGISTER *_reg, const STORAGE_TYPE& reset_value)
	: BitFieldBase(_name, _reg)
	, reg(_reg)
{
	CompileTimeAssert<(OFFSET1 >= 0) && (OFFSET1 < (int) REGISTER::BIT_SIZE)>::_();
	CompileTimeAssert<OFFSET2 < REGISTER::BIT_SIZE>::_();
	
	reg->SetHW_WriteMask(HW_WriteMask());
	reg->SetHW_ReadMask(HW_ReadMask());
	reg->SetSW_WriteMask(SW_WriteMask());
	reg->SetSW_ReadMask(SW_ReadMask());
	reg->SetResetValue(reset_value << Shift(), Mask());
}

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2>
BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::~BitField()
{
}

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2>
BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::operator const STORAGE_TYPE () const
{
	STORAGE_TYPE reg_value = reg->operator const STORAGE_TYPE();
	
	return (reg_value & HW_ReadMask()) >> Shift();
}

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2>
BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>& BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::operator = (const STORAGE_TYPE& value)
{
	STORAGE_TYPE hw_write_mask = HW_WriteMask();
	STORAGE_TYPE reg_value = reg->operator const STORAGE_TYPE();
	STORAGE_TYPE ins_value = value << Shift();
	reg->operator = ((reg_value & ~hw_write_mask) | (ins_value & hw_write_mask));
	return *this;
}

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2>
unsigned int BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::Shift() const
{
	if(OFFSET2 >= 0)
	{
		if(OFFSET1 >= OFFSET2)
		{
			switch(REGISTER::BIT_ORDERING)
			{
				case BO_LITTLE_ENDIAN:
					return OFFSET2;
				case BO_BIG_ENDIAN:
					return REGISTER::BIT_SIZE - 1 - OFFSET2;
			}
		}
		else
		{
			switch(REGISTER::BIT_ORDERING)
			{
				case BO_LITTLE_ENDIAN:
					return OFFSET1;
				case BO_BIG_ENDIAN:
					return REGISTER::BIT_SIZE - 1 - OFFSET1;
			}
		}
	}
	
	return OFFSET1;
}

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2>
unsigned int BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::BitSize() const
{
	if(OFFSET2 >= 0)
	{
		if(OFFSET1 >= OFFSET2)
		{
			return OFFSET1 - OFFSET2 + 1;
		}
		else
		{
			return OFFSET2 - OFFSET1 + 1;
		}
	}
	
	return 1;
}

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2>
typename BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::STORAGE_TYPE BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::Mask() const
{
	return ((STORAGE_TYPE(1) << BitSize()) - 1) << Shift();
}

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2>
typename BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::STORAGE_TYPE BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::HW_WriteMask() const
{
	return (HW_ACCESS & ACC_WO) ? ((STORAGE_TYPE(1) << BitSize()) - 1) << Shift() : 0;
}

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2>
typename BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::STORAGE_TYPE BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::HW_ReadMask() const
{
	return (HW_ACCESS & ACC_RO) ? ((STORAGE_TYPE(1) << BitSize()) - 1) << Shift() : 0;
}

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2>
typename BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::STORAGE_TYPE BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::SW_WriteMask() const
{
	return (SW_ACCESS & ACC_WO) ? ((STORAGE_TYPE(1) << BitSize()) - 1) << Shift() : 0;
}

template <typename REGISTER, Access HW_ACCESS, Access SW_ACCESS, int OFFSET1, int OFFSET2>
typename BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::STORAGE_TYPE BitField<REGISTER, HW_ACCESS, SW_ACCESS, OFFSET1, OFFSET2>::SW_ReadMask() const
{
	return (SW_ACCESS & ACC_RO) ? ((STORAGE_TYPE(1) << BitSize()) - 1) << Shift() : 0;
}

} // end of namespace n25q
} // end of namespace flash
} // end of namespace memory
} // end of namespace tlm2
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_TLM2_MEMORY_FLASH_N25Q_REGISTER_HH__
