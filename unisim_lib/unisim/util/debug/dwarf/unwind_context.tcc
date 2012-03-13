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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_UNWIND_CONTEXT_TCC__
#define __UNISIM_UTIL_DEBUG_DWARF_UNWIND_CONTEXT_TCC__

#include <iostream>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
DWARF_UnwindContext<MEMORY_ADDR>::DWARF_UnwindContext(unisim::util::endian::endian_type _endianness, unsigned int _address_size, unisim::service::interfaces::Memory<MEMORY_ADDR> *_mem_if)
	: endianness(_endianness)
	, address_size(_address_size)
	, mem_if(_mem_if)
{
}

template <class MEMORY_ADDR>
DWARF_UnwindContext<MEMORY_ADDR>::~DWARF_UnwindContext()
{
}
	
template <class MEMORY_ADDR>
MEMORY_ADDR DWARF_UnwindContext<MEMORY_ADDR>::ReadRegister(unsigned int reg_num) const
{
	typename std::map<unsigned int, MEMORY_ADDR>::const_iterator iter = reg_set.find(reg_num);
	return (iter != reg_set.end()) ? (*iter).second : 0;
}

template <class MEMORY_ADDR>
void DWARF_UnwindContext<MEMORY_ADDR>::WriteRegister(unsigned int reg_num, MEMORY_ADDR value)
{
	reg_set[reg_num] = value;
}

template <class MEMORY_ADDR>
bool DWARF_UnwindContext<MEMORY_ADDR>::ReadAddrFromMemory(MEMORY_ADDR addr, MEMORY_ADDR& read_addr) const
{
	switch(address_size)
	{
		case sizeof(uint8_t):
			{
				uint8_t value = 0;
				if(!mem_if->ReadMemory(addr, &value, address_size)) return false;
				read_addr = value;
			}
			break;
		case sizeof(uint16_t):
			{
				uint16_t value = 0;
				if(!mem_if->ReadMemory(addr, &value, address_size)) return false;
				read_addr = unisim::util::endian::Target2Host(endianness, value);
			}
			break;
		case sizeof(uint32_t):
			{
				uint32_t value = 0;
				if(!mem_if->ReadMemory(addr, &value, address_size)) return false;
				read_addr = unisim::util::endian::Target2Host(endianness, value);
			}
			break;
		case sizeof(uint64_t):
			{
				uint32_t value = 0;
				if(!mem_if->ReadMemory(addr, &value, address_size)) return false;
				read_addr = unisim::util::endian::Target2Host(endianness, value);
			}
			break;
		default:
			return false;
	}
	return true;
}

template <class MEMORY_ADDR>
bool DWARF_UnwindContext<MEMORY_ADDR>::Load(const DWARF_RegisterNumberMapping *reg_num_mapping)
{
	typename std::set<unsigned int> reg_num_set;
	
	reg_num_mapping->EnumRegisterNumbers(reg_num_set);
	
	std::set<unsigned int>::const_iterator iter;
	
	for(iter = reg_num_set.begin(); iter != reg_num_set.end(); iter++)
	{
		unsigned int dw_reg_num = *iter;
		
		const unisim::util::debug::Register *arch_reg = reg_num_mapping->GetArchReg(dw_reg_num);
		
		if(!arch_reg)
		{
			throw std::runtime_error("Internal error");
			return false;
		}
		
		unsigned int arch_reg_size = arch_reg->GetSize();
		MEMORY_ADDR reg_value = 0;
		
		switch(arch_reg_size)
		{
			case 1:
				{
					uint8_t arch_reg_value;
					arch_reg->GetValue(&arch_reg_value);
					reg_value = arch_reg_value;
				}
				break;
			case 2:
				{
					uint16_t arch_reg_value;
					arch_reg->GetValue(&arch_reg_value);
					reg_value = arch_reg_value;
				}
				break;
			case 4:
				{
					uint32_t arch_reg_value;
					arch_reg->GetValue(&arch_reg_value);
					reg_value = arch_reg_value;
				}
				break;
			case 8:
				{
					uint64_t arch_reg_value;
					arch_reg->GetValue(&arch_reg_value);
					reg_value = arch_reg_value;
				}
				break;
			default:
				throw std::runtime_error("Internal error");
				return false;
		}
		
		WriteRegister(dw_reg_num, reg_value);
	}
	
	return true;
}

template <class MEMORY_ADDR>
bool DWARF_UnwindContext<MEMORY_ADDR>::Compute(const DWARF_CFIRow<MEMORY_ADDR> *cfi_row)
{
	DWARF_CFARule<MEMORY_ADDR> *cfa_rule = cfi_row->GetCFARule();
	
	switch(cfa_rule->GetType())
	{
		case DW_CFA_RULE_REGISTER_OFFSET:
			{
				DWARF_CFARuleRegisterOffset<MEMORY_ADDR> *cfa_rule_reg_ofs = reinterpret_cast<DWARF_CFARuleRegisterOffset<MEMORY_ADDR> *>(cfa_rule);
				unsigned int dw_reg_num = cfa_rule_reg_ofs->GetRegisterNumber();
				int64_t offset = cfa_rule_reg_ofs->GetOffset();
				cfa = ReadRegister(dw_reg_num) + offset;
			}
			break;
		case DW_CFA_RULE_EXPRESSION:
			throw std::runtime_error("DWARF expressions are not yet implemented");
			return false;
	}
	
	typename std::map<unsigned int, MEMORY_ADDR>::iterator iter;
	
	for(iter = reg_set.begin(); iter != reg_set.end(); iter++)
	{
		unsigned int dw_reg_num = (*iter).first;
		
		DWARF_RegisterRule<MEMORY_ADDR> *reg_rule = cfi_row->GetRegisterRule(dw_reg_num);
		
		if(reg_rule)
		{
			switch(reg_rule->GetType())
			{
				case DW_REG_RULE_UNDEFINED:
					break;
				case DW_REG_RULE_SAME_VALUE:
					break;
				case DW_REG_RULE_OFFSET:
					{
						DWARF_RegisterRuleOffset<MEMORY_ADDR> *reg_rule_offset = reinterpret_cast<DWARF_RegisterRuleOffset<MEMORY_ADDR> *>(reg_rule);
						int64_t offset = reg_rule_offset->GetOffset();
						MEMORY_ADDR addr = cfa + offset;
						MEMORY_ADDR prev_reg_value;
						if(!ReadAddrFromMemory(addr, prev_reg_value)) return false;
						WriteRegister(dw_reg_num, prev_reg_value);
					}
					break;
				case DW_REG_RULE_VAL_OFFSET:
					{
						DWARF_RegisterRuleValOffset<MEMORY_ADDR> *reg_rule_val_offset = reinterpret_cast<DWARF_RegisterRuleValOffset<MEMORY_ADDR> *>(reg_rule);
						int64_t offset = reg_rule_val_offset->GetOffset();
						MEMORY_ADDR prev_reg_value = cfa + offset;
						WriteRegister(dw_reg_num, prev_reg_value);
					}
					break;
				case DW_REG_RULE_REGISTER:
					{
						DWARF_RegisterRuleRegister<MEMORY_ADDR> *reg_rule_reg = reinterpret_cast<DWARF_RegisterRuleRegister<MEMORY_ADDR> *>(reg_rule);
						unsigned int dw_prev_reg_num = reg_rule_reg->GetRegisterNumber();
						MEMORY_ADDR prev_reg_value = ReadRegister(dw_prev_reg_num);
						WriteRegister(dw_reg_num, prev_reg_value);
					}
					break;
				case DW_REG_RULE_EXPRESSION:
					throw std::runtime_error("DWARF expressions are not yet implemented");
					break;
				case DW_REG_RULE_VAL_EXPRESSION:
					throw std::runtime_error("DWARF expressions are not yet implemented");
					break;
			}
		}
	}
	
	return true;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_UnwindContext<MEMORY_ADDR>& dw_reg_set)
{
	typename std::map<unsigned int, MEMORY_ADDR>::const_iterator iter;
	
	for(iter = dw_reg_set.reg_set.begin(); iter != dw_reg_set.reg_set.end(); iter++)
	{
		unsigned int reg_num = (*iter).first;
		MEMORY_ADDR reg_value = (*iter).second;
		
		os << "<r" << reg_num << "=0x" << std::hex << reg_value << std::dec << "> ";
	}
	
	return os;
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
