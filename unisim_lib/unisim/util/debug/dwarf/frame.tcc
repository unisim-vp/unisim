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
DWARF_Register<MEMORY_ADDR>::DWARF_Register(unisim::service::interfaces::Register *_reg, unisim::util::endian::endian_type _endianness)
	: reg(_reg)
	, endianness(_endianness)
{
}

template <class MEMORY_ADDR>
const char *DWARF_Register<MEMORY_ADDR>::GetName() const
{
	return reg->GetName();
}

template <class MEMORY_ADDR>
const char *DWARF_Register<MEMORY_ADDR>::GetDescription() const
{
	return reg->GetDescription();
}

template <class MEMORY_ADDR>
int DWARF_Register<MEMORY_ADDR>::GetSize() const
{
	return reg->GetSize();
}

template <class MEMORY_ADDR, class VALUE_TYPE>
DWARF_CopyRegister<MEMORY_ADDR, VALUE_TYPE>::DWARF_CopyRegister(unisim::service::interfaces::Register *_reg, unisim::util::endian::endian_type _endianness)
	: DWARF_Register<MEMORY_ADDR>(_reg, _endianness)
	, value()
{
	assert(this->reg->GetSize() == sizeof(VALUE_TYPE));
	this->reg->GetValue(value);
}

template <class MEMORY_ADDR, class VALUE_TYPE>
void DWARF_CopyRegister<MEMORY_ADDR, VALUE_TYPE>::GetValue(void *buffer) const
{
	*(VALUE_TYPE *) buffer = value;
}


template <class MEMORY_ADDR, class VALUE_TYPE>
void DWARF_CopyRegister<MEMORY_ADDR, VALUE_TYPE>::SetValue(const void *buffer)
{
	value = *(VALUE_TYPE *) buffer;
}

template <class MEMORY_ADDR, class VALUE_TYPE>
void DWARF_CopyRegister<MEMORY_ADDR, VALUE_TYPE>::Copy(const unisim::service::interfaces::Register *from)
{
	from->GetValue(&value);
}

template <class MEMORY_ADDR, class VALUE_TYPE>
bool DWARF_CopyRegister<MEMORY_ADDR, VALUE_TYPE>::Load(unisim::service::interfaces::Memory<MEMORY_ADDR> *mem_if, MEMORY_ADDR addr, unsigned int read_size)
{
	unsigned int reg_size = this->reg->GetSize();
	if(!read_size || (read_size > reg_size)) read_size = this->reg->GetSize();
	switch(read_size)
	{
		case sizeof(uint8_t):
			{
				uint8_t read_value = 0;
				if(!mem_if->ReadMemory(addr, &read_value, read_size)) return false;
				value = read_value;
			}
			break;
		case sizeof(uint16_t):
			{
				uint16_t read_value = 0;
				if(!mem_if->ReadMemory(addr, &read_value, read_size)) return false;
				value = unisim::util::endian::Target2Host(this->endianness, read_value);
			}
			break;
		case 3:
			{
				uint8_t buf[3] = { 0, 0, 0 };
				if(!mem_if->ReadMemory(addr, buf, read_size)) return false;
				switch(this->endianness)
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
		case sizeof(uint32_t):
			{
				uint32_t read_value = 0;
				if(!mem_if->ReadMemory(addr, &read_value, read_size)) return false;
				value = unisim::util::endian::Target2Host(this->endianness, read_value);
			}
			break;
		case sizeof(uint64_t):
			{
				uint32_t read_value = 0;
				if(!mem_if->ReadMemory(addr, &read_value, read_size)) return false;
				value = unisim::util::endian::Target2Host(this->endianness, read_value);
			}
			break;
		default:
			return false;
	}
//	std::cerr << "ReadFromMemory(0x" << std::hex << addr << ", 0x" << +value << ", " << std::dec << read_size << ");" << std::endl;
	return true;
}

template <class MEMORY_ADDR>
DWARF_PassThroughRegister<MEMORY_ADDR>::DWARF_PassThroughRegister(unisim::service::interfaces::Register *_reg, unisim::util::endian::endian_type _endianness)
	:	DWARF_Register<MEMORY_ADDR>(_reg, _endianness)
{
}

template <class MEMORY_ADDR>
void DWARF_PassThroughRegister<MEMORY_ADDR>::GetValue(void *buffer) const
{
	this->reg->GetValue(buffer);
}

template <class MEMORY_ADDR>
void DWARF_PassThroughRegister<MEMORY_ADDR>::SetValue(const void *buffer)
{
	this->reg->SetValue(buffer);
}

template <class MEMORY_ADDR>
void DWARF_PassThroughRegister<MEMORY_ADDR>::Copy(const unisim::service::interfaces::Register *from)
{
	// NOP
}

template <class MEMORY_ADDR>
bool DWARF_PassThroughRegister<MEMORY_ADDR>::Load(unisim::service::interfaces::Memory<MEMORY_ADDR> *mem_if, MEMORY_ADDR addr, unsigned int read_size)
{
	unsigned int reg_size = this->reg->GetSize();
	if(!read_size || (read_size > reg_size)) read_size = this->reg->GetSize();
	switch(read_size)
	{
		case sizeof(uint8_t):
			{
				uint8_t read_value = 0;
				if(!mem_if->ReadMemory(addr, &read_value, read_size)) return false;
				this->reg->SetValue(read_value);
			}
			break;
		case sizeof(uint16_t):
			{
				uint16_t read_value = 0;
				if(!mem_if->ReadMemory(addr, &read_value, read_size)) return false;
				read_value = unisim::util::endian::Target2Host(this->endianness, read_value);
				this->reg->SetValue(read_value);
			}
			break;
		case 3:
			{
				uint8_t buf[3] = { 0, 0, 0 };
				if(!mem_if->ReadMemory(addr, buf, read_size)) return false;
				if(unisim::util::endian::GetHostEndian() != this->endianness)
				{
					uint8_t tmp = buf[0];
					buf[0] = buf[2];
					buf[2] = tmp;
				}
				this->reg->SetValue(buf);
			}
			break;
		case sizeof(uint32_t):
			{
				uint32_t read_value = 0;
				if(!mem_if->ReadMemory(addr, &read_value, read_size)) return false;
				read_value = unisim::util::endian::Target2Host(this->endianness, read_value);
				this->reg->SetValue(read_value);
			}
			break;
		case sizeof(uint64_t):
			{
				uint32_t read_value = 0;
				if(!mem_if->ReadMemory(addr, &read_value, read_size)) return false;
				read_value = unisim::util::endian::Target2Host(this->endianness, read_value);
				this->reg->SetValue(read_value);
			}
			break;
		default:
			return false;
	}
//	std::cerr << "ReadFromMemory(0x" << std::hex << addr << ", 0x" << +value << ", " << std::dec << read_size << ");" << std::endl;
	return true;
}

template <class MEMORY_ADDR>
DWARF_RegSet<MEMORY_ADDR>::DWARF_RegSet(const DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int prc_num)
	: reg_num_mapping(dw_handler->GetRegisterNumberMapping(prc_num))
	, endianness(dw_handler->GetArchEndianness())
	, reg_set()
	, program_counter(0)
{
}

template <class MEMORY_ADDR>
DWARF_RegSet<MEMORY_ADDR>::~DWARF_RegSet()
{
	for(typename RegSet::iterator it = reg_set.begin(); it != reg_set.end(); ++it)
	{
		unisim::service::interfaces::Register *reg = (*it).second;
		delete reg;
	}
	if(program_counter) delete program_counter;
}

template <class MEMORY_ADDR>
bool DWARF_RegSet<MEMORY_ADDR>::LoadArchRegs()
{
	std::set<unsigned int> reg_num_set;
	
	reg_num_mapping->EnumRegisterNumbers(reg_num_set);
	
	std::set<unsigned int>::const_iterator iter;
	
	for(iter = reg_num_set.begin(); iter != reg_num_set.end(); iter++)
	{
		unsigned int dw_reg_num = *iter;
		unisim::service::interfaces::Register *arch_reg = reg_num_mapping->GetRegister(dw_reg_num);
		
		if(!arch_reg)
		{
			throw std::runtime_error("Internal error");
			return false;
		}
		
		DefPassThroughRegister(dw_reg_num, arch_reg);
	}
	
	return true;
}

template <class MEMORY_ADDR>
void DWARF_RegSet<MEMORY_ADDR>::Copy(const DWARF_RegSet& o)
{
	for(typename RegSet::const_iterator it = o.reg_set.begin(); it != o.reg_set.end(); ++it)
	{
		unsigned int dw_reg_num = (*it).first;
		unisim::service::interfaces::Register *reg = (*it).second;
		DefRegister(dw_reg_num, reg);
	}
}

template <class MEMORY_ADDR>
DWARF_Register<MEMORY_ADDR> *DWARF_RegSet<MEMORY_ADDR>::DefRegister(unsigned int reg_num, unisim::service::interfaces::Register *reg)
{
	DWARF_Register<MEMORY_ADDR> *reg_copy = 0;
	switch(reg->GetSize())
	{
		case sizeof(uint8_t): reg_copy = new DWARF_CopyRegister<MEMORY_ADDR, uint8_t>(reg, endianness); break;
		case sizeof(uint16_t): reg_copy = new DWARF_CopyRegister<MEMORY_ADDR, uint16_t>(reg, endianness); break;
		case sizeof(uint32_t): reg_copy = new DWARF_CopyRegister<MEMORY_ADDR, uint32_t>(reg, endianness); break;
		case sizeof(uint64_t): reg_copy = new DWARF_CopyRegister<MEMORY_ADDR, uint64_t>(reg, endianness); break;
	}
	
	typename RegSet::iterator it = reg_set.find(reg_num);
	if(it != reg_set.end())
	{
		unisim::service::interfaces::Register *old_reg = (*it).second;
		delete old_reg;
		(*it).second = reg_copy;
	}
	else
	{
		reg_set.insert(typename RegSet::value_type(reg_num, reg_copy));
	}
	
	return reg_copy;
}

template <class MEMORY_ADDR>
DWARF_Register<MEMORY_ADDR> *DWARF_RegSet<MEMORY_ADDR>::DefPassThroughRegister(unsigned int reg_num, unisim::service::interfaces::Register *reg)
{
	DWARF_Register<MEMORY_ADDR> *passthrough_reg = 0;
	switch(reg->GetSize())
	{
		case sizeof(uint8_t): passthrough_reg = new DWARF_PassThroughRegister<MEMORY_ADDR>(reg, endianness); break;
		case sizeof(uint16_t): passthrough_reg = new DWARF_PassThroughRegister<MEMORY_ADDR>(reg, endianness); break;
		case sizeof(uint32_t): passthrough_reg = new DWARF_PassThroughRegister<MEMORY_ADDR>(reg, endianness); break;
		case sizeof(uint64_t): passthrough_reg = new DWARF_PassThroughRegister<MEMORY_ADDR>(reg, endianness); break;
	}
	
	typename RegSet::iterator it = reg_set.find(reg_num);
	if(it != reg_set.end())
	{
		unisim::service::interfaces::Register *old_reg = (*it).second;
		delete old_reg;
		(*it).second = passthrough_reg;
	}
	else
	{
		reg_set.insert(typename RegSet::value_type(reg_num, passthrough_reg));
	}
	
	return passthrough_reg;
}

template <class MEMORY_ADDR>
void DWARF_RegSet<MEMORY_ADDR>::UndefRegister(unsigned int reg_num)
{
	typename RegSet::iterator it = reg_set.find(reg_num);
	if(it != reg_set.end())
	{
		unisim::service::interfaces::Register *reg = (*it).second;
		delete reg;
		reg_set.erase(it);
	}
}

template <class MEMORY_ADDR>
template <typename T>
bool DWARF_RegSet<MEMORY_ADDR>::ReadRegister(unsigned int reg_num, T& reg_value) const
{
	typename RegSet::const_iterator it = reg_set.find(reg_num);
	if(it != reg_set.end())
	{
		unisim::service::interfaces::Register *reg = (*it).second;
		reg->GetValue(reg_value);
		return true;
	}
	
	return false;
}

template <class MEMORY_ADDR>
template <typename T>
bool DWARF_RegSet<MEMORY_ADDR>::WriteRegister(unsigned int reg_num, T value)
{
// 	std::cerr << "r" << reg_num << "<-0x" << std::hex << value << std::dec << std::endl;
	unisim::service::interfaces::Register *reg = 0;
	
	typename RegSet::const_iterator it = reg_set.find(reg_num);
	if(it != reg_set.end())
	{
		reg = (*it).second;
	}
	else
	{
		unisim::service::interfaces::Register *arch_reg = reg_num_mapping->GetRegister(reg_num);
		
		if(!arch_reg) return false;
		
		reg = DefRegister(reg_num, arch_reg);
	}
	
	reg->SetValue(value);
	
	return true;
}

template <class MEMORY_ADDR>
void DWARF_RegSet<MEMORY_ADDR>::DefProgramCounterRegister(DWARF_Register<MEMORY_ADDR> *ret_addr_reg)
{
	if(program_counter) delete program_counter;
	program_counter = 0;
	
	switch(ret_addr_reg->GetSize())
	{
		case sizeof(uint8_t): program_counter = new DWARF_CopyRegister<MEMORY_ADDR, uint8_t>(ret_addr_reg, endianness); break;
		case sizeof(uint16_t): program_counter = new DWARF_CopyRegister<MEMORY_ADDR, uint16_t>(ret_addr_reg, endianness); break;
		case sizeof(uint32_t): program_counter = new DWARF_CopyRegister<MEMORY_ADDR, uint32_t>(ret_addr_reg, endianness); break;
		case sizeof(uint64_t): program_counter = new DWARF_CopyRegister<MEMORY_ADDR, uint64_t>(ret_addr_reg, endianness); break;
	}
}

template <class MEMORY_ADDR>
DWARF_Register<MEMORY_ADDR> *DWARF_RegSet<MEMORY_ADDR>::GetRegister(unsigned int reg_num) const
{
	typename RegSet::const_iterator it = reg_set.find(reg_num);
	return (it != reg_set.end()) ? (*it).second : 0;
}

template <class MEMORY_ADDR>
unisim::service::interfaces::Register *DWARF_RegSet<MEMORY_ADDR>::GetProgramCounterRegister() const
{
	return program_counter ? program_counter : reg_num_mapping->GetProgramCounterRegister();
}

template <class MEMORY_ADDR>
bool DWARF_RegSet<MEMORY_ADDR>::LoadRegister(unsigned int reg_num, unisim::service::interfaces::Memory<MEMORY_ADDR> *mem_if, MEMORY_ADDR addr, unsigned int read_size)
{
	DWARF_Register<MEMORY_ADDR> *reg = 0;
	
	typename RegSet::const_iterator it = reg_set.find(reg_num);
	if(it != reg_set.end())
	{
		reg = (*it).second;
	}
	else
	{
		unisim::service::interfaces::Register *arch_reg = reg_num_mapping->GetRegister(reg_num);
		
		if(!arch_reg) return false;
		
		reg = DefRegister(reg_num, arch_reg);
	}
	
	return reg->Load(mem_if, addr, read_size);
}

template <class MEMORY_ADDR>
void DWARF_RegSet<MEMORY_ADDR>::Print(std::ostream& os) const
{
	for(typename RegSet::const_iterator it = reg_set.begin(); it != reg_set.end(); it++)
	{
		unsigned int reg_num = (*it).first;
		unisim::service::interfaces::Register *reg = (*it).second;
		
		os << "<r" << reg_num << "=0x" << std::hex << (*reg) << std::dec << "> ";
	}
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_RegSet<MEMORY_ADDR>& dw_reg_set)
{
	dw_reg_set.Print(os);
	return os;
}

template <class MEMORY_ADDR>
DWARF_Frame<MEMORY_ADDR>::DWARF_Frame(const DWARF_Handler<MEMORY_ADDR> *_dw_handler, unsigned int _prc_num)
	: dw_handler(_dw_handler)
	, prc_num(_prc_num)
	, has_sp_reg_num(false)
	, sp_reg_num(0)
	, endianness(_dw_handler->GetArchEndianness())
	, address_size(_dw_handler->GetArchAddressSize())
	, dw_cfa_spec(_dw_handler->GetCFA_Specification())
	, dw_cfa_reg_rule_offset_spec(_dw_handler->GetCFA_RegRuleOffsetSpecification())
	, mem_if(_dw_handler->GetMemoryInterface(prc_num))
	, cfa(0)
	, dw_reg_set(_dw_handler, _prc_num)
{
	has_sp_reg_num = _dw_handler->GetRegisterNumberMapping(_prc_num)->GetStackPointerRegisterNumber(sp_reg_num);
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
unisim::service::interfaces::Register *DWARF_Frame<MEMORY_ADDR>::GetRegister(unsigned int reg_num) const
{
	return dw_reg_set.GetRegister(reg_num);
}

template <class MEMORY_ADDR>
unisim::service::interfaces::Register *DWARF_Frame<MEMORY_ADDR>::GetProgramCounterRegister() const
{
	return dw_reg_set.GetProgramCounterRegister();
}

template <class MEMORY_ADDR>
template <typename T>
bool DWARF_Frame<MEMORY_ADDR>::ReadRegister(unsigned int reg_num, T& reg_value) const
{
	return dw_reg_set.ReadRegister(reg_num, reg_value);
}

template <class MEMORY_ADDR>
bool DWARF_Frame<MEMORY_ADDR>::LoadArchRegs()
{
	return dw_reg_set.LoadArchRegs();
}

template <class MEMORY_ADDR>
bool DWARF_Frame<MEMORY_ADDR>::ComputeCFA(const DWARF_CFIRow<MEMORY_ADDR> *cfi_row, const DWARF_Frame<MEMORY_ADDR> *next_frame)
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
				if(!next_frame->ReadRegister(dw_reg_num, reg_value)) return false;
				cfa = reg_value + offset;
			}
			break;
		case DW_CFA_RULE_EXPRESSION:
			{
				DWARF_CFARuleExpression<MEMORY_ADDR> *cfa_rule_expr = reinterpret_cast<DWARF_CFARuleExpression<MEMORY_ADDR> *>(cfa_rule);
				DWARF_ExpressionVM<MEMORY_ADDR> cfa_rule_expr_vm = DWARF_ExpressionVM<MEMORY_ADDR>(dw_handler, this);
				bool cfa_rule_expr_status = cfa_rule_expr_vm.Execute(cfa_rule_expr->GetExpression(), cfa, 0);
				if(!cfa_rule_expr_status) return false;
			}
			break;
	}
	
	return true;
}

template <class MEMORY_ADDR>
bool DWARF_Frame<MEMORY_ADDR>::Unwind(const DWARF_CFIRow<MEMORY_ADDR> *cfi_row, const DWARF_Frame<MEMORY_ADDR> *next_frame, unsigned int dw_ret_addr_reg_num)
{
	dw_reg_set.Copy(next_frame->GetRegSet());
	
	DWARF_CFARule<MEMORY_ADDR> *cfa_rule = cfi_row->GetCFARule();
	
	switch(cfa_rule->GetType())
	{
		case DW_CFA_RULE_REGISTER_OFFSET:
			{
				DWARF_CFARuleRegisterOffset<MEMORY_ADDR> *cfa_rule_reg_ofs = reinterpret_cast<DWARF_CFARuleRegisterOffset<MEMORY_ADDR> *>(cfa_rule);
				unsigned int dw_reg_num = cfa_rule_reg_ofs->GetRegisterNumber();
				int64_t offset = cfa_rule_reg_ofs->GetOffset();
				MEMORY_ADDR reg_value = 0;
				if(!next_frame->ReadRegister(dw_reg_num, reg_value)) return false;
				cfa = reg_value + offset;
			}
			break;
		case DW_CFA_RULE_EXPRESSION:
			{
				DWARF_CFARuleExpression<MEMORY_ADDR> *cfa_rule_expr = reinterpret_cast<DWARF_CFARuleExpression<MEMORY_ADDR> *>(cfa_rule);
				DWARF_ExpressionVM<MEMORY_ADDR> cfa_rule_expr_vm = DWARF_ExpressionVM<MEMORY_ADDR>(dw_handler, this);
				bool cfa_rule_expr_status = cfa_rule_expr_vm.Execute(cfa_rule_expr->GetExpression(), cfa, 0);
				if(!cfa_rule_expr_status) return false;
			}
			break;
	}
	
	typename std::set<unsigned int> dw_reg_nums;
	
	cfi_row->GetRegisterRulesNumbers(dw_reg_nums); // complete register set with "virtual" register rules

	typename std::set<unsigned int>::iterator dw_reg_nums_iter;
	for(dw_reg_nums_iter = dw_reg_nums.begin(); dw_reg_nums_iter != dw_reg_nums.end(); dw_reg_nums_iter++)
	{
		unsigned int dw_reg_num = *dw_reg_nums_iter;
		DWARF_RegisterRule<MEMORY_ADDR> *reg_rule = cfi_row->GetRegisterRule(dw_reg_num);
		
		if(reg_rule)
		{
			switch(reg_rule->GetType())
			{
				case DW_REG_RULE_UNDEFINED:
					dw_reg_set.UndefRegister(dw_reg_num);
					break;
				case DW_REG_RULE_SAME_VALUE:
					{
						unisim::service::interfaces::Register *reg = next_frame->GetRegister(dw_reg_num);
						if(!reg) return false;
						dw_reg_set.DefRegister(dw_reg_num, reg);
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
								if(!has_sp_reg_num || !next_frame->ReadRegister(sp_reg_num, sp)) return false;
								addr = sp + offset;
								break;
							}
						}
						if(!dw_reg_set.LoadRegister(dw_reg_num, mem_if, addr, (dw_reg_num == dw_ret_addr_reg_num) ? dw_handler->GetReturnAddressSize(cfi_row->GetLocation()) : 0)) return false;
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
								if(!has_sp_reg_num || !next_frame->ReadRegister(sp_reg_num, sp)) return false;
								prev_reg_value = sp + offset;
								break;
							}
						}
						if(!dw_reg_set.WriteRegister(dw_reg_num, prev_reg_value)) return false;
					}
					break;
				case DW_REG_RULE_REGISTER:
					{
						DWARF_RegisterRuleRegister<MEMORY_ADDR> *reg_rule_reg = reinterpret_cast<DWARF_RegisterRuleRegister<MEMORY_ADDR> *>(reg_rule);
						unsigned int dw_prev_reg_num = reg_rule_reg->GetRegisterNumber();
						const unisim::service::interfaces::Register *prev_reg = next_frame->GetRegister(dw_prev_reg_num);
						if(!prev_reg) return false;
						DWARF_Register<MEMORY_ADDR> *reg = dw_reg_set.GetRegister(dw_reg_num);
						if(!reg) return false;
						reg->Copy(prev_reg);
					}
					break;
				case DW_REG_RULE_EXPRESSION:
					{
						DWARF_RegisterRuleExpression<MEMORY_ADDR> *reg_rule_expr = reinterpret_cast<DWARF_RegisterRuleExpression<MEMORY_ADDR> *>(reg_rule);
						DWARF_ExpressionVM<MEMORY_ADDR> reg_rule_expr_vm = DWARF_ExpressionVM<MEMORY_ADDR>(dw_handler, this);
						MEMORY_ADDR prev_reg_value_addr = 0;
						bool reg_rule_expr_status = reg_rule_expr_vm.Execute(reg_rule_expr->GetExpression(), prev_reg_value_addr, 0);
						if(!reg_rule_expr_status) return false;
						if(!dw_reg_set.LoadRegister(dw_reg_num, mem_if, prev_reg_value_addr, (dw_reg_num == dw_ret_addr_reg_num) ? dw_handler->GetReturnAddressSize(cfi_row->GetLocation()) : 0)) return false;
					}
					break;
				case DW_REG_RULE_VAL_EXPRESSION:
					{
						DWARF_RegisterRuleValExpression<MEMORY_ADDR> *reg_rule_val_expr = reinterpret_cast<DWARF_RegisterRuleValExpression<MEMORY_ADDR> *>(reg_rule);
						DWARF_ExpressionVM<MEMORY_ADDR> reg_rule_val_expr_vm = DWARF_ExpressionVM<MEMORY_ADDR>(dw_handler, this);
						MEMORY_ADDR prev_reg_value = 0;
						bool reg_rule_expr_status = reg_rule_val_expr_vm.Execute(reg_rule_val_expr->GetExpression(), prev_reg_value, 0);
						if(!reg_rule_expr_status) return false;
						if(!dw_reg_set.WriteRegister(dw_reg_num, prev_reg_value)) return false;
					}
					break;
			}
		}
	}

	// architectural rule: SP = CFA (standard) or CFA + call context size (non-standard)
	switch(dw_cfa_spec)
	{
		case DW_CFA_IS_SP_AT_THE_CALL_SITE_IN_THE_PREVIOUS_FRAME:
			if(!has_sp_reg_num || !dw_reg_set.WriteRegister(sp_reg_num, cfa)) return false;
			break;
		case DW_CFA_IS_SP_VALUE_ON_ENTRY_TO_THE_CURRENT_FRAME:
			if(!has_sp_reg_num || !dw_reg_set.WriteRegister(sp_reg_num, cfa + dw_handler->GetReturnAddressSize(cfi_row->GetLocation()))) return false;
			break;
	}

	// return address of next frame
	dw_reg_set.DefProgramCounterRegister(dw_reg_set.GetRegister(dw_ret_addr_reg_num));
	
	return true;
}

template <class MEMORY_ADDR>
unsigned int DWARF_Frame<MEMORY_ADDR>::GetProcessorNumber() const
{
	return prc_num;
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
