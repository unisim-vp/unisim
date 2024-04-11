/*
 *  Copyright (c) 2012,
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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_FRAME_TCC__
#define __UNISIM_UTIL_DEBUG_DWARF_FRAME_TCC__

#include <iostream>
#include <cassert>
#include <unisim/util/debug/simple_register.hh>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

using unisim::util::endian::E_BIG_ENDIAN;
using unisim::util::endian::E_LITTLE_ENDIAN;

template <class MEMORY_ADDR>
DWARF_RegisterRef<MEMORY_ADDR>::DWARF_RegisterRef(const DWARF_Frame<MEMORY_ADDR> *_dw_frame, unsigned int _dw_reg_num)
	: dw_frame(_dw_frame)
	, dw_reg_num(_dw_reg_num)
{
}

template <class MEMORY_ADDR>
DWARF_Register<MEMORY_ADDR> *DWARF_RegisterRef<MEMORY_ADDR>::GetRegister() const
{
	return dw_frame->GetInnerFrame()->GetRegister(dw_reg_num);
}

template <class MEMORY_ADDR>
DWARF_Register<MEMORY_ADDR>::DWARF_Register(unisim::service::interfaces::Register *_reg)
	: reg(_reg)
	, snapshot()
{
}

template <class MEMORY_ADDR>
DWARF_Register<MEMORY_ADDR>::~DWARF_Register()
{
}

template <class MEMORY_ADDR>
const char *DWARF_Register<MEMORY_ADDR>::GetName() const
{
	return reg ? reg->GetName() : "unnamed";
}

template <class MEMORY_ADDR>
int DWARF_Register<MEMORY_ADDR>::GetSize() const
{
	return reg ? reg->GetSize() : 0;
}

template <class MEMORY_ADDR>
bool DWARF_Register<MEMORY_ADDR>::GetValue(void *buffer) const
{
	if(!reg) return false;
	reg->GetValue(buffer);
	return true;
}

template <class MEMORY_ADDR>
bool DWARF_Register<MEMORY_ADDR>::SetValue(const void *buffer)
{
	if(!reg) return false;
	reg->SetValue(buffer);
	return true;
}

template <class MEMORY_ADDR>
bool DWARF_Register<MEMORY_ADDR>::GetValue(uint8_t& val) const
{
	return GetTypedValue(val);
}

template <class MEMORY_ADDR>
bool DWARF_Register<MEMORY_ADDR>::SetValue(const uint8_t& val)
{
	return SetTypedValue(val);
}

template <class MEMORY_ADDR>
bool DWARF_Register<MEMORY_ADDR>::GetValue(uint16_t& val) const
{
	return GetTypedValue(val);
}

template <class MEMORY_ADDR>
bool DWARF_Register<MEMORY_ADDR>::SetValue(const uint16_t& val)
{
	return SetTypedValue(val);
}

template <class MEMORY_ADDR>
bool DWARF_Register<MEMORY_ADDR>::GetValue(uint32_t& val) const
{
	return GetTypedValue(val);
}

template <class MEMORY_ADDR>
bool DWARF_Register<MEMORY_ADDR>::SetValue(const uint32_t& val)
{
	return SetTypedValue(val);
}

template <class MEMORY_ADDR>
bool DWARF_Register<MEMORY_ADDR>::GetValue(uint64_t& val) const
{
	return GetTypedValue(val);
}

template <class MEMORY_ADDR>
bool DWARF_Register<MEMORY_ADDR>::SetValue(const uint64_t& val)
{
	return SetTypedValue(val);
}

template <class MEMORY_ADDR>
void DWARF_Register<MEMORY_ADDR>::Print(std::ostream& os) const
{
	switch(this->GetSize())
	{
		case 1:
			{
				uint8_t val8;
				this->GetValue(&val8);
				os << +val8;
			}
			break;
		case 2:
			{
				uint16_t val16;
				this->GetValue(&val16);
				os << val16;
			}
			break;
		case 4:
			{
				uint32_t val32;
				this->GetValue(&val32);
				os << val32;
			}
			break;
		case 8:
			{
				uint64_t val64;
				this->GetValue(&val64);
				os << val64;
			}
			break;
	}
}

template <class MEMORY_ADDR>
bool DWARF_Register<MEMORY_ADDR>::Snap()
{
	snapshot.resize(GetSize());
	
	return GetValue(&snapshot[0]);
}

template <class MEMORY_ADDR>
bool DWARF_Register<MEMORY_ADDR>::Commit()
{
	if(!reg) return true;
	if(snapshot.size() == (unsigned int) reg->GetSize())
	{
// 		std::cerr << reg->GetName() << " <-";
// 		for(unsigned int i = 0; i < snapshot.size(); ++i)
// 		{
// 			std::cerr << " 0x" << std::hex << +snapshot[i] << std::dec;
// 		}
// 		std::cerr << std::endl;
		reg->SetValue(&snapshot[0]);
		return true;
	}
	return false;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_Register<MEMORY_ADDR>& reg)
{
	reg.Print(os);
	return os;
}

template <class MEMORY_ADDR>
template <typename T> bool DWARF_Register<MEMORY_ADDR>::GetTypedValue(T& val) const
{
	switch(GetSize())
	{
		case 1: { uint8_t val8;   if(!this->GetValue(&val8))  return false; val = val8;  return true; }
		case 2: { uint16_t val16; if(!this->GetValue(&val16)) return false; val = val16; return true; }
		case 4: { uint32_t val32; if(!this->GetValue(&val32)) return false; val = val32; return true; }
		case 8: { uint64_t val64; if(!this->GetValue(&val64)) return false; val = val64; return true; }
	}
	return false;
}

template <class MEMORY_ADDR>
template <typename T> bool DWARF_Register<MEMORY_ADDR>::SetTypedValue(T& val) const
{
	switch(GetSize())
	{
		case 1: { uint8_t val8  = val; if(!this->SetValue(&val8)) return false; return true; }
		case 2: { uint8_t val16 = val; if(!this->SetValue(&val16)) return false; return true; }
		case 4: { uint8_t val32 = val; if(!this->SetValue(&val32)) return false; return true; }
		case 8: { uint8_t val64 = val; if(!this->SetValue(&val64)) return false; return true; }
	}
	return false;
}

template <class MEMORY_ADDR>
DWARF_RenameRegister<MEMORY_ADDR>::DWARF_RenameRegister(unisim::service::interfaces::Register *_reg, const DWARF_RegisterRef<MEMORY_ADDR>& _dw_reg_ref)
	: DWARF_Register<MEMORY_ADDR>(_reg)
	, dw_reg_ref(_dw_reg_ref)
{
}

template <class MEMORY_ADDR>
int DWARF_RenameRegister<MEMORY_ADDR>::GetSize() const
{
	return dw_reg_ref.GetRegister()->GetSize();
}

template <class MEMORY_ADDR>
bool DWARF_RenameRegister<MEMORY_ADDR>::GetValue(void *buffer) const
{
	return dw_reg_ref.GetRegister()->GetValue(buffer);
}

template <class MEMORY_ADDR>
bool DWARF_RenameRegister<MEMORY_ADDR>::SetValue(const void *buffer)
{
	return dw_reg_ref.GetRegister()->SetValue(buffer);
}

template <class MEMORY_ADDR>
DWARF_ValueRegister<MEMORY_ADDR>::DWARF_ValueRegister(unisim::service::interfaces::Register *_reg, int _size, const uint64_t& _value)
	: DWARF_Register<MEMORY_ADDR>(_reg)
	, size(_size)
	, value(_value)
{
}

template <class MEMORY_ADDR>
int DWARF_ValueRegister<MEMORY_ADDR>::GetSize() const
{
	return size;
}

template <class MEMORY_ADDR>
bool DWARF_ValueRegister<MEMORY_ADDR>::GetValue(void *buffer) const
{
	switch(GetSize())
	{
		case 1: *(uint8_t *) buffer = value; return true;
		case 2: *(uint16_t *) buffer = value; return true;
		case 3:
		{
			unisim::util::endian::endian_type host_endianness = unisim::util::endian::GetHostEndian();
			((uint8_t *) buffer)[0] = (host_endianness == E_BIG_ENDIAN) ? (value >> 16) : value;
			((uint8_t *) buffer)[1] = value >> 8;
			((uint8_t *) buffer)[2] = (host_endianness == E_BIG_ENDIAN) ? value : (value >> 16);
			return true;
		}
		case 4: *(uint32_t *) buffer = value; return true;
		case 5:
		{
			unisim::util::endian::endian_type host_endianness = unisim::util::endian::GetHostEndian();
			((uint8_t *) buffer)[0] = (host_endianness == E_BIG_ENDIAN) ? (value >> 32) : value;
			((uint8_t *) buffer)[1] = (host_endianness == E_BIG_ENDIAN) ? (value >> 24) : (value >> 8);
			((uint8_t *) buffer)[2] = value >> 16;
			((uint8_t *) buffer)[3] = (host_endianness == E_BIG_ENDIAN) ? (value >> 8)  : (value >> 24);
			((uint8_t *) buffer)[4] = (host_endianness == E_BIG_ENDIAN) ? value         : (value >> 32);
			return true;
		}
		case 6:
		{
			unisim::util::endian::endian_type host_endianness = unisim::util::endian::GetHostEndian();
			((uint8_t *) buffer)[0] = (host_endianness == E_BIG_ENDIAN) ? (value >> 40) : value;
			((uint8_t *) buffer)[1] = (host_endianness == E_BIG_ENDIAN) ? (value >> 32) : (value >> 8);
			((uint8_t *) buffer)[2] = (host_endianness == E_BIG_ENDIAN) ? (value >> 24) : (value >> 16);
			((uint8_t *) buffer)[3] = (host_endianness == E_BIG_ENDIAN) ? (value >> 16) : (value >> 24);
			((uint8_t *) buffer)[4] = (host_endianness == E_BIG_ENDIAN) ? (value >> 8)  : (value >> 32);
			((uint8_t *) buffer)[5] = (host_endianness == E_BIG_ENDIAN) ? value         : (value >> 40);
			return true;
		}
		case 7:
		{
			unisim::util::endian::endian_type host_endianness = unisim::util::endian::GetHostEndian();
			((uint8_t *) buffer)[0] = (host_endianness == E_BIG_ENDIAN) ? (value >> 48) : value;
			((uint8_t *) buffer)[1] = (host_endianness == E_BIG_ENDIAN) ? (value >> 40) : (value >> 8);
			((uint8_t *) buffer)[2] = (host_endianness == E_BIG_ENDIAN) ? (value >> 32) : (value >> 16);
			((uint8_t *) buffer)[3] = value >> 24;
			((uint8_t *) buffer)[4] = (host_endianness == E_BIG_ENDIAN) ? (value >> 16) : (value >> 32);
			((uint8_t *) buffer)[5] = (host_endianness == E_BIG_ENDIAN) ? (value >> 8)  : (value >> 40);
			((uint8_t *) buffer)[6] = (host_endianness == E_BIG_ENDIAN) ? value         : (value >> 48);
			return true;
		}
		case 8: *(uint64_t *) buffer = value; return true;
	}
	return false;
}


template <class MEMORY_ADDR>
bool DWARF_ValueRegister<MEMORY_ADDR>::SetValue(const void *buffer)
{
	return false; // read-only
}

template <class MEMORY_ADDR>
DWARF_SpilledRegister<MEMORY_ADDR>::DWARF_SpilledRegister(unisim::service::interfaces::Register *_reg, unisim::service::interfaces::Memory<MEMORY_ADDR> *_mem_if, MEMORY_ADDR _addr, unsigned int _size, unisim::util::endian::endian_type _endianness)
	: DWARF_Register<MEMORY_ADDR>(_reg)
	, mem_if(_mem_if)
	, addr(_addr)
	, size(_size)
	, endianness(_endianness)
{
	assert(this->reg);
	unsigned int reg_size = this->reg->GetSize();
	if(!size || (size > reg_size)) size = reg_size;
}

template <class MEMORY_ADDR>
bool DWARF_SpilledRegister<MEMORY_ADDR>::GetValue(void *buffer) const
{
	uint8_t buf[size];
  ::memset(&buf[0], 0, sizeof(buf));
	if(!mem_if->ReadMemory(addr, &buf[0], size)) return false;
	unsigned int reg_size = this->GetSize();
	unisim::util::endian::endian_type host_endianness = unisim::util::endian::GetHostEndian();
	unsigned int i, j;
	for(i = 0, j = 0; (i < reg_size) && (j < size); ++i, ++j)
	{
		((uint8_t *) buffer)[(host_endianness == E_BIG_ENDIAN) ? (reg_size - i - 1) : i] = buf[(endianness == E_BIG_ENDIAN) ? (size - j - 1) : j];
	}
	while(i < reg_size)
	{
		((uint8_t *) buffer)[(host_endianness == E_BIG_ENDIAN) ? (reg_size - i - 1) : i] = 0;
	}
	return true;
}

template <class MEMORY_ADDR>
bool DWARF_SpilledRegister<MEMORY_ADDR>::SetValue(const void *buffer)
{
	uint8_t buf[size];
  ::memset(&buf[0], 0, sizeof(buf));
	unsigned int reg_size = this->GetSize();
	unisim::util::endian::endian_type host_endianness = unisim::util::endian::GetHostEndian();
	unsigned int i, j;
	for(i = 0, j = 0; (i < reg_size) && (j < size); ++i, ++j)
	{
		buf[(endianness == E_BIG_ENDIAN) ? (size - j - 1) : j] = ((uint8_t *) buffer)[(host_endianness == E_BIG_ENDIAN) ? (reg_size - i - 1) : i];
	}
	return mem_if->WriteMemory(addr, &buf[0], size);
}

template <class MEMORY_ADDR>
DWARF_RegSet<MEMORY_ADDR>::DWARF_RegSet(const DWARF_MachineState<MEMORY_ADDR> *dw_mach_state, unsigned int prc_num)
	: dw_reg_num_mapping(dw_mach_state->GetRegisterNumberMapping(prc_num))
	, reg_set()
	, program_counter(0)
{
}

template <class MEMORY_ADDR>
DWARF_RegSet<MEMORY_ADDR>::~DWARF_RegSet()
{
	for(typename RegSet::iterator it = reg_set.begin(); it != reg_set.end(); ++it)
	{
		DWARF_Register<MEMORY_ADDR> *dw_reg = (*it).second;
		delete dw_reg;
	}
	if(program_counter) delete program_counter;
}

template <class MEMORY_ADDR>
bool DWARF_RegSet<MEMORY_ADDR>::LoadArchRegs()
{
	if(!dw_reg_num_mapping) return false;
	
	std::set<unsigned int> reg_num_set;
	
	dw_reg_num_mapping->EnumRegisterNumbers(reg_num_set);
	
	std::set<unsigned int>::const_iterator iter;
	
	for(iter = reg_num_set.begin(); iter != reg_num_set.end(); iter++)
	{
		unsigned int dw_reg_num = *iter;
		unisim::service::interfaces::Register *arch_reg = dw_reg_num_mapping->GetRegister(dw_reg_num);
		
		if(!arch_reg)
		{
			throw std::runtime_error("Internal error");
			return false;
		}
		
		DefRegister(dw_reg_num, new DWARF_Register<MEMORY_ADDR>(arch_reg));
	}
	
	unisim::service::interfaces::Register *pc_reg = dw_reg_num_mapping->GetProgramCounterRegister();
	DefProgramCounterRegister(new DWARF_Register<MEMORY_ADDR>(pc_reg));
	
	return true;
}

template <class MEMORY_ADDR>
bool DWARF_RegSet<MEMORY_ADDR>::Commit()
{
	for(typename RegSet::const_iterator it = reg_set.begin(); it != reg_set.end(); ++it)
	{
		DWARF_Register<MEMORY_ADDR> *dw_reg = (*it).second;
		if(!dw_reg->Snap()) return false;
	}
	if(!program_counter) return false;
	if(!program_counter->Snap()) return false;
	for(typename RegSet::const_iterator it = reg_set.begin(); it != reg_set.end(); ++it)
	{
		DWARF_Register<MEMORY_ADDR> *dw_reg = (*it).second;
		if(!dw_reg->Commit()) return false;
	}
	return program_counter->Commit();
}

template <class MEMORY_ADDR>
void DWARF_RegSet<MEMORY_ADDR>::DefRegister(unsigned int dw_reg_num, DWARF_Register<MEMORY_ADDR> *dw_reg)
{
	typename RegSet::iterator it = reg_set.find(dw_reg_num);
	if(it != reg_set.end())
	{
		DWARF_Register<MEMORY_ADDR> *old_dw_reg = (*it).second;
		delete old_dw_reg;
		(*it).second = dw_reg;
	}
	else
	{
		reg_set.insert(typename RegSet::value_type(dw_reg_num, dw_reg));
	}
}

template <class MEMORY_ADDR>
void DWARF_RegSet<MEMORY_ADDR>::UndefRegister(unsigned int dw_reg_num)
{
	typename RegSet::iterator it = reg_set.find(dw_reg_num);
	if(it != reg_set.end())
	{
		DWARF_Register<MEMORY_ADDR> *dw_reg = (*it).second;
		delete dw_reg;
		reg_set.erase(it);
	}
}

template <class MEMORY_ADDR>
template <typename T>
bool DWARF_RegSet<MEMORY_ADDR>::ReadRegister(unsigned int dw_reg_num, T& reg_value) const
{
	DWARF_Register<MEMORY_ADDR> *dw_reg = GetRegister(dw_reg_num);
	return dw_reg && dw_reg->GetValue(reg_value);
}

template <class MEMORY_ADDR>
bool DWARF_RegSet<MEMORY_ADDR>::ReadProgramCounterRegister(MEMORY_ADDR& pc) const
{
	DWARF_Register<MEMORY_ADDR> *pc_reg = GetProgramCounterRegister();
	if(!pc_reg) return false;
	pc_reg->GetValue(pc);
	return true;
}

template <class MEMORY_ADDR>
template <typename T>
bool DWARF_RegSet<MEMORY_ADDR>::WriteRegister(unsigned int dw_reg_num, T value)
{
	DWARF_Register<MEMORY_ADDR> *dw_reg = GetRegister(dw_reg_num);
	if(!dw_reg) return false;
	dw_reg->SetValue(value);
	
	return true;
}

template <class MEMORY_ADDR>
void DWARF_RegSet<MEMORY_ADDR>::DefProgramCounterRegister(DWARF_Register<MEMORY_ADDR> *pc_reg)
{
	if(program_counter) delete program_counter;
	program_counter = pc_reg;
}

template <class MEMORY_ADDR>
DWARF_Register<MEMORY_ADDR> *DWARF_RegSet<MEMORY_ADDR>::GetRegister(unsigned int reg_num) const
{
	typename RegSet::const_iterator it = reg_set.find(reg_num);
	return (it != reg_set.end()) ? (*it).second : 0;
}

template <class MEMORY_ADDR>
DWARF_Register<MEMORY_ADDR> *DWARF_RegSet<MEMORY_ADDR>::GetProgramCounterRegister() const
{
	return program_counter;
}

template <class MEMORY_ADDR>
void DWARF_RegSet<MEMORY_ADDR>::Print(std::ostream& os) const
{
	for(typename RegSet::const_iterator it = reg_set.begin(); it != reg_set.end(); it++)
	{
		unsigned int reg_num = (*it).first;
		DWARF_Register<MEMORY_ADDR> *dw_reg = (*it).second;
		
		os << "<r" << reg_num << "=0x" << std::hex << (*dw_reg) << std::dec << "> ";
	}
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_RegSet<MEMORY_ADDR>& dw_reg_set)
{
	dw_reg_set.Print(os);
	return os;
}

template <class MEMORY_ADDR>
DWARF_Frame<MEMORY_ADDR>::DWARF_Frame(const DWARF_MachineState<MEMORY_ADDR> *_dw_mach_state, unsigned int _prc_num)
	: dw_mach_state(_dw_mach_state)
	, prc_num(_prc_num)
	, mem_if(_dw_mach_state->GetMemoryInterface(prc_num))
	, cfa(0)
	, dw_reg_set(_dw_mach_state, _prc_num)
	, dw_handler(0)
	, dw_reg_num_mapping(_dw_mach_state->GetRegisterNumberMapping(_prc_num))
	, cfi_row(0)
	, dw_ret_addr_reg_num((unsigned int) -1)
	, has_sp_reg_num(dw_reg_num_mapping && dw_reg_num_mapping->GetStackPointerRegisterNumber(sp_reg_num))
{
}

template <class MEMORY_ADDR>
DWARF_Frame<MEMORY_ADDR>::DWARF_Frame(const DWARF_MachineState<MEMORY_ADDR> *_dw_mach_state, unsigned int _prc_num, const DWARF_Handler<MEMORY_ADDR> *_dw_handler, const DWARF_CFIRow<MEMORY_ADDR> *_cfi_row, unsigned int _dw_ret_addr_reg_num)
	: dw_mach_state(_dw_mach_state)
	, prc_num(_prc_num)
	, mem_if(_dw_mach_state->GetMemoryInterface(prc_num))
	, cfa(0)
	, dw_reg_set(_dw_mach_state, _prc_num)
	, dw_handler(_dw_handler)
	, dw_reg_num_mapping(_dw_mach_state->GetRegisterNumberMapping(_prc_num))
	, cfi_row(_cfi_row)
	, dw_ret_addr_reg_num(_dw_ret_addr_reg_num)
	, has_sp_reg_num(dw_reg_num_mapping && dw_reg_num_mapping->GetStackPointerRegisterNumber(sp_reg_num))
{
}

template <class MEMORY_ADDR>
DWARF_Frame<MEMORY_ADDR>::~DWARF_Frame()
{
}

template <class MEMORY_ADDR>
MEMORY_ADDR DWARF_Frame<MEMORY_ADDR>::ReadCFA() const
{
	return cfa;
}

template <class MEMORY_ADDR>
const DWARF_RegSet<MEMORY_ADDR>& DWARF_Frame<MEMORY_ADDR>::GetRegSet() const
{
	return dw_reg_set;
}

template <class MEMORY_ADDR>
DWARF_Register<MEMORY_ADDR> *DWARF_Frame<MEMORY_ADDR>::GetRegister(unsigned int dw_reg_num) const
{
// 	return dw_reg_set.GetRegister(reg_num);
	DWARF_Register<MEMORY_ADDR> *dw_reg = dw_reg_set.GetRegister(dw_reg_num);
	if(dw_reg) return dw_reg;
	
	if(dw_reg_num == sp_reg_num)
	{
		if(!has_sp_reg_num) return 0;
		if(!dw_reg_num_mapping) return 0;
		unisim::service::interfaces::Register *arch_sp_reg = dw_reg_num_mapping->GetRegister(sp_reg_num);
		if(!arch_sp_reg) return 0;
		
		// architectural rule: SP = CFA (standard) or CFA + call context size (non-standard)
		DW_CFA_Specification dw_cfa_spec = dw_handler->GetCFA_Specification();
		MEMORY_ADDR sp_value = 0;
		switch(dw_cfa_spec)
		{
			case DW_CFA_IS_SP_AT_THE_CALL_SITE_IN_THE_PREVIOUS_FRAME: // standard
				sp_value = cfa;
				break;
			case DW_CFA_IS_SP_VALUE_ON_ENTRY_TO_THE_CURRENT_FRAME: // non-standard
				sp_value = cfa + dw_handler->GetReturnAddressSize(cfi_row->GetLocation());
				break;
		}
		DWARF_Register<MEMORY_ADDR> *dw_reg = new DWARF_ValueRegister<MEMORY_ADDR>(arch_sp_reg, arch_sp_reg->GetSize(), sp_value);
		dw_reg_set.DefRegister(sp_reg_num, dw_reg);
		return dw_reg;
	}
	
	if(cfi_row)
	{
		DWARF_RegisterRule<MEMORY_ADDR> *reg_rule = cfi_row->GetRegisterRule(dw_reg_num);
		
		if(reg_rule)
		{
			DW_CFA_RegRuleOffsetSpecification dw_cfa_reg_rule_offset_spec = dw_handler->GetCFA_RegRuleOffsetSpecification();
			
			switch(reg_rule->GetType())
			{
				case DW_REG_RULE_UNDEFINED:
					dw_reg_set.UndefRegister(dw_reg_num);
					return 0;
				case DW_REG_RULE_SAME_VALUE:
					{
						if(!dw_reg_num_mapping) return 0;
						unisim::service::interfaces::Register *arch_reg = dw_reg_num_mapping->GetRegister(dw_reg_num);
						DWARF_Register<MEMORY_ADDR> *dw_reg = new DWARF_RenameRegister<MEMORY_ADDR>(arch_reg, DWARF_RegisterRef<MEMORY_ADDR>(this, dw_reg_num));
						dw_reg_set.DefRegister(dw_reg_num, dw_reg);
						return dw_reg;
					}
					break;
				case DW_REG_RULE_OFFSET:
					{
						DWARF_RegisterRuleOffset<MEMORY_ADDR> *reg_rule_offset = reinterpret_cast<DWARF_RegisterRuleOffset<MEMORY_ADDR> *>(reg_rule);
						int64_t offset = reg_rule_offset->GetOffset();
						MEMORY_ADDR addr = 0;
						switch(dw_cfa_reg_rule_offset_spec)
						{
							case DW_CFA_REG_RULE_OFFSET_IS_CFA_RELATIVE: // standard
								addr = cfa + offset;
								break;
							case DW_CFA_REG_RULE_OFFSET_IS_SP_RELATIVE: // non-standard
							{
								MEMORY_ADDR sp = 0;
								if(!has_sp_reg_num || !GetInnerFrame()->ReadRegister(sp_reg_num, sp)) return 0;
								addr = sp + offset;
								break;
							}
						}
						if(!dw_reg_num_mapping) return 0;
						unisim::service::interfaces::Register *arch_reg = dw_reg_num_mapping->GetRegister(dw_reg_num);
						if(!arch_reg) return 0;
						unsigned int size = (dw_reg_num == dw_ret_addr_reg_num) ? dw_handler->GetReturnAddressSize(cfi_row->GetLocation()) : 0;
						unisim::util::endian::endian_type endianness = dw_handler->GetArchEndianness();
						DWARF_Register<MEMORY_ADDR> *dw_reg = new DWARF_SpilledRegister<MEMORY_ADDR>(arch_reg, mem_if, addr, size, endianness);
						dw_reg_set.DefRegister(dw_reg_num, dw_reg);
						return dw_reg;
					}
					break;
				case DW_REG_RULE_VAL_OFFSET:
					{
						DWARF_RegisterRuleValOffset<MEMORY_ADDR> *reg_rule_val_offset = reinterpret_cast<DWARF_RegisterRuleValOffset<MEMORY_ADDR> *>(reg_rule);
						int64_t offset = reg_rule_val_offset->GetOffset();
						MEMORY_ADDR prev_reg_value = 0;
						switch(dw_cfa_reg_rule_offset_spec)
						{
							case DW_CFA_REG_RULE_OFFSET_IS_CFA_RELATIVE: // standard
								prev_reg_value = cfa + offset;
								break;
							case DW_CFA_REG_RULE_OFFSET_IS_SP_RELATIVE: // non-standard
							{
								MEMORY_ADDR sp = 0;
								if(!has_sp_reg_num || !GetInnerFrame()->ReadRegister(sp_reg_num, sp)) return 0;
								prev_reg_value = sp + offset;
								break;
							}
						}
						if(!dw_reg_num_mapping) return 0;
						unisim::service::interfaces::Register *arch_reg = dw_reg_num_mapping->GetRegister(dw_reg_num);
						DWARF_Register<MEMORY_ADDR> *dw_reg = new DWARF_ValueRegister<MEMORY_ADDR>(arch_reg, arch_reg ? arch_reg->GetSize() : sizeof(prev_reg_value), prev_reg_value);
						dw_reg_set.DefRegister(dw_reg_num, dw_reg);
						return dw_reg;
					}
					break;
				case DW_REG_RULE_REGISTER:
					{
						DWARF_RegisterRuleRegister<MEMORY_ADDR> *reg_rule_reg = reinterpret_cast<DWARF_RegisterRuleRegister<MEMORY_ADDR> *>(reg_rule);
						unsigned int dw_prev_reg_num = reg_rule_reg->GetRegisterNumber();
						if(!dw_reg_num_mapping) return 0;
						unisim::service::interfaces::Register *arch_reg = dw_reg_num_mapping->GetRegister(dw_reg_num);
						DWARF_Register<MEMORY_ADDR> *dw_reg = new DWARF_RenameRegister<MEMORY_ADDR>(arch_reg, DWARF_RegisterRef<MEMORY_ADDR>(this, dw_prev_reg_num));
						dw_reg_set.DefRegister(dw_reg_num, dw_reg);
						return dw_reg;
					}
					break;
				case DW_REG_RULE_EXPRESSION:
					{
						DWARF_RegisterRuleExpression<MEMORY_ADDR> *reg_rule_expr = reinterpret_cast<DWARF_RegisterRuleExpression<MEMORY_ADDR> *>(reg_rule);
						DWARF_ExpressionVM<MEMORY_ADDR> reg_rule_expr_vm = DWARF_ExpressionVM<MEMORY_ADDR>(dw_handler, GetInnerFrame());
						MEMORY_ADDR prev_reg_value_addr = 0;
						bool reg_rule_expr_status = reg_rule_expr_vm.Execute(reg_rule_expr->GetExpression(), prev_reg_value_addr, 0);
						if(!reg_rule_expr_status) return 0;
						if(!dw_reg_num_mapping) return 0;
						unisim::service::interfaces::Register *arch_reg = dw_reg_num_mapping->GetRegister(dw_reg_num);
						if(!arch_reg) return 0;
						unsigned int size = (dw_reg_num == dw_ret_addr_reg_num) ? dw_handler->GetReturnAddressSize(cfi_row->GetLocation()) : 0;
						unisim::util::endian::endian_type endianness = dw_handler->GetArchEndianness();
						DWARF_Register<MEMORY_ADDR> *dw_reg = new DWARF_SpilledRegister<MEMORY_ADDR>(arch_reg, mem_if, prev_reg_value_addr, size, endianness);
						dw_reg_set.DefRegister(dw_reg_num, dw_reg);
						return dw_reg;
					}
					break;
				case DW_REG_RULE_VAL_EXPRESSION:
					{
						DWARF_RegisterRuleValExpression<MEMORY_ADDR> *reg_rule_val_expr = reinterpret_cast<DWARF_RegisterRuleValExpression<MEMORY_ADDR> *>(reg_rule);
						DWARF_ExpressionVM<MEMORY_ADDR> reg_rule_val_expr_vm = DWARF_ExpressionVM<MEMORY_ADDR>(dw_handler, GetInnerFrame());
						MEMORY_ADDR prev_reg_value = 0;
						bool reg_rule_expr_status = reg_rule_val_expr_vm.Execute(reg_rule_val_expr->GetExpression(), prev_reg_value, 0);
						if(!reg_rule_expr_status) return 0;
						if(!dw_reg_num_mapping) return 0;
						unisim::service::interfaces::Register *arch_reg = dw_reg_num_mapping->GetRegister(dw_reg_num);
						DWARF_Register<MEMORY_ADDR> *dw_reg = new DWARF_ValueRegister<MEMORY_ADDR>(arch_reg, arch_reg ? arch_reg->GetSize() : sizeof(prev_reg_value), prev_reg_value);
						dw_reg_set.DefRegister(dw_reg_num, dw_reg);
						return dw_reg;
					}
					break;
			}
		}
	}
	
	dw_reg = GetInnerFrame()->GetRegister(dw_reg_num);
	if(dw_reg)
	{
		if(!dw_reg_num_mapping) return 0;
		unisim::service::interfaces::Register *arch_reg = dw_reg_num_mapping->GetRegister(dw_reg_num);
		dw_reg_set.DefRegister(dw_reg_num, new DWARF_RenameRegister<MEMORY_ADDR>(arch_reg, DWARF_RegisterRef<MEMORY_ADDR>(this, dw_reg_num)));
	}
	
	return dw_reg;
}

template <class MEMORY_ADDR>
DWARF_Register<MEMORY_ADDR> *DWARF_Frame<MEMORY_ADDR>::GetProgramCounterRegister() const
{
// 	return dw_reg_set.GetProgramCounterRegister();
	DWARF_Register<MEMORY_ADDR> *dw_reg = dw_reg_set.GetProgramCounterRegister();
	
	if(dw_reg) return dw_reg;
	
	// return address of next frame
	MEMORY_ADDR ret_addr_value = 0;
	if(!ReadRegister(dw_ret_addr_reg_num, ret_addr_value)) return 0;
	if(!dw_reg_num_mapping) return 0;
	unisim::service::interfaces::Register *pc_reg = dw_reg_num_mapping->GetProgramCounterRegister();
	if(!pc_reg) return 0;
	dw_reg = new DWARF_ValueRegister<MEMORY_ADDR>(pc_reg, pc_reg->GetSize(), ret_addr_value);
	dw_reg_set.DefProgramCounterRegister(dw_reg);
	
	return dw_reg;
}

template <class MEMORY_ADDR>
template <typename T>
bool DWARF_Frame<MEMORY_ADDR>::ReadRegister(unsigned int reg_num, T& reg_value) const
{
//	return dw_reg_set.ReadRegister(reg_num, reg_value);
	DWARF_Register<MEMORY_ADDR> *dw_reg = GetRegister(reg_num);
	return dw_reg && dw_reg->GetValue(reg_value);
}

template <class MEMORY_ADDR>
template <typename T>
bool DWARF_Frame<MEMORY_ADDR>::WriteRegister(unsigned int dw_reg_num, T value)
{
	DWARF_Register<MEMORY_ADDR> *dw_reg = GetRegister(dw_reg_num);
	if(!dw_reg) return false;
	dw_reg->SetValue(value);
	
	return true;
}

template <class MEMORY_ADDR>
unsigned int DWARF_Frame<MEMORY_ADDR>::GetRegisterSize(unsigned int dw_reg_num) const
{
	DWARF_Register<MEMORY_ADDR> *dw_reg = GetRegister(dw_reg_num);
	if(!dw_reg) return 0;
	
	return dw_reg->GetSize();
}

template <class MEMORY_ADDR>
const char *DWARF_Frame<MEMORY_ADDR>::GetRegisterName(unsigned int dw_reg_num) const
{
	DWARF_Register<MEMORY_ADDR> *dw_reg = GetRegister(dw_reg_num);
	if(!dw_reg) return 0;
	
	return dw_reg->GetName();
}

template <class MEMORY_ADDR>
bool DWARF_Frame<MEMORY_ADDR>::ReadProgramCounterRegister(MEMORY_ADDR& pc) const
{
// 	return dw_reg_set.ReadProgramCounterRegister(pc);
	DWARF_Register<MEMORY_ADDR> *pc_reg = GetProgramCounterRegister();
	if(!pc_reg) return false;
	pc_reg->GetValue(pc);
	return true;
}

template <class MEMORY_ADDR>
bool DWARF_Frame<MEMORY_ADDR>::LoadArchRegs()
{
	return dw_reg_set.LoadArchRegs();
}

template <class MEMORY_ADDR>
bool DWARF_Frame<MEMORY_ADDR>::Unwind()
{
	DWARF_CFARule<MEMORY_ADDR> *cfa_rule = cfi_row->GetCFARule();
	
	switch(cfa_rule->GetType())
	{
		case DW_CFA_RULE_REGISTER_OFFSET:
			{
				DWARF_CFARuleRegisterOffset<MEMORY_ADDR> *cfa_rule_reg_ofs = reinterpret_cast<DWARF_CFARuleRegisterOffset<MEMORY_ADDR> *>(cfa_rule);
				unsigned int dw_reg_num = cfa_rule_reg_ofs->GetRegisterNumber();
				int64_t offset = cfa_rule_reg_ofs->GetOffset();
				MEMORY_ADDR reg_value = 0;
				if(!GetInnerFrame()->ReadRegister(dw_reg_num, reg_value)) return false;
				cfa = reg_value + offset;
			}
			break;
		case DW_CFA_RULE_EXPRESSION:
			{
				DWARF_CFARuleExpression<MEMORY_ADDR> *cfa_rule_expr = reinterpret_cast<DWARF_CFARuleExpression<MEMORY_ADDR> *>(cfa_rule);
				DWARF_ExpressionVM<MEMORY_ADDR> cfa_rule_expr_vm = DWARF_ExpressionVM<MEMORY_ADDR>(dw_handler, GetInnerFrame());
				bool cfa_rule_expr_status = cfa_rule_expr_vm.Execute(cfa_rule_expr->GetExpression(), cfa, 0);
				if(!cfa_rule_expr_status) return false;
			}
			break;
	}
	
	return true;
}

template <class MEMORY_ADDR>
const DWARF_MachineState<MEMORY_ADDR> *DWARF_Frame<MEMORY_ADDR>::GetMachineState() const
{
	return dw_mach_state;
}

template <class MEMORY_ADDR>
unsigned int DWARF_Frame<MEMORY_ADDR>::GetProcessorNumber() const
{
	return prc_num;
}

template <class MEMORY_ADDR>
const DWARF_Frame<MEMORY_ADDR> *DWARF_Frame<MEMORY_ADDR>::GetInnerFrame() const
{
	return dw_mach_state->GetInnerFrame(this);
}

template <class MEMORY_ADDR>
template <class VALUE_TYPE>
bool DWARF_Frame<MEMORY_ADDR>::Load(MEMORY_ADDR addr, VALUE_TYPE& value, unsigned int read_size, unisim::util::endian::endian_type endianness, unsigned int addr_space) const
{
	// FIXME: addr_space is currently ignored in our implementation
	switch(read_size)
	{
		case 1:
			{
				uint8_t read_value = 0;
				if(!mem_if->ReadMemory(addr, &read_value, read_size)) return false;
				value = read_value;
			}
			break;
		case 2:
			{
				uint16_t read_value = 0;
				if(!mem_if->ReadMemory(addr, &read_value, read_size)) return false;
				value = unisim::util::endian::Target2Host(endianness, read_value);
			}
			break;
		case 3:
			{
				uint8_t buf[3] = { 0, 0, 0 };
				if(!mem_if->ReadMemory(addr, buf, read_size)) return false;
				switch(endianness)
				{
					case E_BIG_ENDIAN:
						value = ((uint32_t) buf[0] << 16) | ((uint32_t) buf[1] << 8) | (uint32_t) buf[2];
						break;
					case E_LITTLE_ENDIAN:
						value = (uint32_t) buf[0] | ((uint32_t) buf[1] << 8) | ((uint32_t) buf[2] << 16);
						break;
					default:
						value = 0;
						break;
				}
			}
			break;
		case 4:
			{
				uint32_t read_value = 0;
				if(!mem_if->ReadMemory(addr, &read_value, read_size)) return false;
				value = unisim::util::endian::Target2Host(endianness, read_value);
			}
			break;
		case 5:
			{
				uint8_t buf[5] = { 0, 0, 0, 0, 0 };
				if(!mem_if->ReadMemory(addr, buf, read_size)) return false;
				switch(endianness)
				{
					case E_BIG_ENDIAN:
						value = ((uint64_t) buf[0] << 32) | ((uint64_t) buf[1] << 24) | ((uint64_t) buf[2] << 16) | ((uint64_t) buf[3] << 8) | (uint64_t) buf[4];
						break;
					case E_LITTLE_ENDIAN:
						value = (uint64_t) buf[0] | ((uint64_t) buf[1] << 8) | ((uint64_t) buf[2] << 16) | ((uint64_t) buf[3] << 24) | ((uint64_t) buf[4] << 32);
						break;
					default:
						value = 0;
						break;
				}
			}
			break;
		case 6:
			{
				uint8_t buf[6] = { 0, 0, 0, 0, 0, 0 };
				if(!mem_if->ReadMemory(addr, buf, read_size)) return false;
				switch(endianness)
				{
					case E_BIG_ENDIAN:
						value = ((uint64_t) buf[0] << 40) | ((uint64_t) buf[1] << 32) | ((uint64_t) buf[2] << 24) | ((uint64_t) buf[3] << 16) | ((uint64_t) buf[4] << 8) | (uint64_t) buf[5];
						break;
					case E_LITTLE_ENDIAN:
						value = (uint64_t) buf[0] | ((uint64_t) buf[1] << 8) | ((uint64_t) buf[2] << 16) | ((uint64_t) buf[3] << 24) | ((uint64_t) buf[4] << 32) | ((uint64_t) buf[4] << 40);
						break;
					default:
						value = 0;
						break;
				}
			}
			break;
		case 7:
			{
				uint8_t buf[7] = { 0, 0, 0, 0, 0, 0, 0 };
				if(!mem_if->ReadMemory(addr, buf, read_size)) return false;
				switch(endianness)
				{
					case E_BIG_ENDIAN:
						value = ((uint64_t) buf[0] << 48) | ((uint64_t) buf[1] << 40) | ((uint64_t) buf[2] << 32) | ((uint64_t) buf[3] << 24) | ((uint64_t) buf[4] << 16) | ((uint64_t) buf[5] << 8) | (uint64_t) buf[6];
						break;
					case E_LITTLE_ENDIAN:
						value = (uint64_t) buf[0] | ((uint64_t) buf[1] << 8) | ((uint64_t) buf[2] << 16) | ((uint64_t) buf[3] << 24) | ((uint64_t) buf[4] << 32) | ((uint64_t) buf[4] << 40) | ((uint64_t) buf[5] << 48);
						break;
					default:
						value = 0;
						break;
				}
			}
			break;
		case 8:
			{
				uint32_t read_value = 0;
				if(!mem_if->ReadMemory(addr, &read_value, read_size)) return false;
				value = unisim::util::endian::Target2Host(endianness, read_value);
			}
			break;
		default:
			return false;
	}
	return true;
}

template <class MEMORY_ADDR>
bool DWARF_Frame<MEMORY_ADDR>::Commit()
{
	return dw_reg_set.Commit();
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
