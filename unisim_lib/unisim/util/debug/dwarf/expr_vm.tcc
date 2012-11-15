/*
 *  Copyright (c) 2010,
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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_EXPR_VM_TCC__
#define __UNISIM_UTIL_DEBUG_DWARF_EXPR_VM_TCC__

#include <unisim/util/debug/dwarf/expr_vm.hh>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/util/endian/endian.hh>
#include <stack>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

using unisim::util::arithmetic::SignExtend;
using unisim::util::endian::E_BIG_ENDIAN;
using unisim::util::endian::E_LITTLE_ENDIAN;
using unisim::util::endian::Target2Host;

template <class MEMORY_ADDR>
DWARF_LocationPiece<MEMORY_ADDR>::DWARF_LocationPiece(unsigned int _dw_loc_type)
	: dw_loc_type(_dw_loc_type)
{
}

template <class MEMORY_ADDR>
DWARF_LocationPiece<MEMORY_ADDR>::~DWARF_LocationPiece()
{
}

template <class MEMORY_ADDR>
unsigned int DWARF_LocationPiece<MEMORY_ADDR>::GetType() const
{
	return dw_loc_type;
}

template <class MEMORY_ADDR>
DWARF_MemoryLocationPiece<MEMORY_ADDR>::DWARF_MemoryLocationPiece(MEMORY_ADDR _dw_addr)
	: DWARF_LocationPiece<MEMORY_ADDR>(DW_LOC_MEMORY)
	, dw_addr(_dw_addr)
	, dw_bit_offset(0)
	, dw_bit_size(0)
{
}

template <class MEMORY_ADDR>
DWARF_MemoryLocationPiece<MEMORY_ADDR>::DWARF_MemoryLocationPiece(MEMORY_ADDR _dw_addr, unsigned int _dw_bit_offset, unsigned int _dw_bit_size)
	: DWARF_LocationPiece<MEMORY_ADDR>(DW_LOC_MEMORY)
	, dw_addr(_dw_addr)
	, dw_bit_offset(_dw_bit_offset)
	, dw_bit_size(_dw_bit_size)
{
}

template <class MEMORY_ADDR>
DWARF_MemoryLocationPiece<MEMORY_ADDR>::~DWARF_MemoryLocationPiece()
{
}

template <class MEMORY_ADDR>
MEMORY_ADDR DWARF_MemoryLocationPiece<MEMORY_ADDR>::GetAddress() const
{
	return dw_addr;
}

template <class MEMORY_ADDR>
unsigned int DWARF_MemoryLocationPiece<MEMORY_ADDR>::GetBitOffset() const
{
	return dw_bit_offset;
}

template <class MEMORY_ADDR>
unsigned int DWARF_MemoryLocationPiece<MEMORY_ADDR>::GetBitSize() const
{
	return dw_bit_size;
}

template <class MEMORY_ADDR>
DWARF_RegisterLocationPiece<MEMORY_ADDR>::DWARF_RegisterLocationPiece(unsigned int _dw_reg_num)
	: DWARF_LocationPiece<MEMORY_ADDR>(DW_LOC_REGISTER)
	, dw_reg_num(_dw_reg_num)
	, dw_bit_offset(0)
	, dw_bit_size(0)
{
}

template <class MEMORY_ADDR>
DWARF_RegisterLocationPiece<MEMORY_ADDR>::DWARF_RegisterLocationPiece(unsigned int _dw_reg_num, unsigned int _dw_bit_offset, unsigned int _dw_bit_size)
	: DWARF_LocationPiece<MEMORY_ADDR>(DW_LOC_REGISTER)
	, dw_reg_num(_dw_reg_num)
	, dw_bit_offset(_dw_bit_offset)
	, dw_bit_size(_dw_bit_size)
{
}

template <class MEMORY_ADDR>
DWARF_RegisterLocationPiece<MEMORY_ADDR>::~DWARF_RegisterLocationPiece()
{
}

template <class MEMORY_ADDR>
unsigned int DWARF_RegisterLocationPiece<MEMORY_ADDR>::GetRegisterNumber() const
{
	return dw_reg_num;
}

template <class MEMORY_ADDR>
unsigned int DWARF_RegisterLocationPiece<MEMORY_ADDR>::GetBitOffset() const
{
	return dw_bit_offset;
}

template <class MEMORY_ADDR>
unsigned int DWARF_RegisterLocationPiece<MEMORY_ADDR>::GetBitSize() const
{
	return dw_bit_size;
}

template <class MEMORY_ADDR>
DWARF_Location<MEMORY_ADDR>::DWARF_Location()
{
}

template <class MEMORY_ADDR>
DWARF_Location<MEMORY_ADDR>::~DWARF_Location()
{
}

template <class MEMORY_ADDR>
void DWARF_Location<MEMORY_ADDR>::Add(DWARF_LocationPiece<MEMORY_ADDR> *dw_loc_piece)
{
	dw_location_pieces.push_back(dw_loc_piece);
}

template <class MEMORY_ADDR>
const std::vector<DWARF_LocationPiece<MEMORY_ADDR> *>& DWARF_Location<MEMORY_ADDR>::GetLocationPieces() const
{
	return dw_location_pieces;
}

template <class MEMORY_ADDR>
DWARF_ExpressionVM<MEMORY_ADDR>::DWARF_ExpressionVM(const DWARF_Handler<MEMORY_ADDR> *_dw_handler)
	: dw_handler(_dw_handler)
	, reg_num_mapping(_dw_handler->GetRegisterNumberMapping())
	, mem_if(_dw_handler->GetMemoryInterface())
	, file_endianness(_dw_handler->GetFileEndianness())
	, arch_endianness(_dw_handler->GetArchEndianness())
	, file_address_size(_dw_handler->GetFileAddressSize())
	, arch_address_size(_dw_handler->GetArchAddressSize())
{
}

template <class MEMORY_ADDR>
DWARF_ExpressionVM<MEMORY_ADDR>::~DWARF_ExpressionVM()
{
}

template <class MEMORY_ADDR>
bool DWARF_ExpressionVM<MEMORY_ADDR>::Disasm(std::ostream& os, const DWARF_Expression<MEMORY_ADDR> *dw_expr)
{
	return Run(dw_expr, &os, 0, 0);
}

template <class MEMORY_ADDR>
bool DWARF_ExpressionVM<MEMORY_ADDR>::Execute(const DWARF_Expression<MEMORY_ADDR> *dw_expr, MEMORY_ADDR& result_addr, DWARF_Location<MEMORY_ADDR> *dw_location)
{
	return Run(dw_expr, 0, &result_addr, dw_location);
}

template <class MEMORY_ADDR>
bool DWARF_ExpressionVM<MEMORY_ADDR>::Run(const DWARF_Expression<MEMORY_ADDR> *dw_expr, std::ostream *os, MEMORY_ADDR *result_addr, DWARF_Location<MEMORY_ADDR> *dw_location)
{
	bool executing = (result_addr != 0) || (dw_location != 0);
	const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu = dw_expr->GetCompilationUnit();
	const DWARF_CallFrameProgram<MEMORY_ADDR> *dw_cfp = dw_expr->GetCallFrameProgram();
	
	uint8_t file_address_size = dw_cu ? dw_cu->GetHandler()->GetFileAddressSize() : dw_cfp->GetHandler()->GetFileAddressSize();
	endian_type file_endianness = dw_cu ? dw_cu->GetHandler()->GetFileEndianness() : dw_cfp->GetHandler()->GetFileEndianness();
//	uint8_t offset_size = dw_cu ? dw_cu->GetOffsetSize() : 0; // offsets are unused for expressions in a call frame program
	
	uint64_t expr_length = dw_expr->GetLength();
	const uint8_t *expr = dw_expr->GetValue();
	
	uint8_t opcode;
	
	if(expr_length)
	{
		uint64_t expr_pos = 0;
		
		do
		{
			opcode = expr[expr_pos];
			expr_pos++;
			
			switch(opcode)
			{
				case DW_OP_lit0:
				case DW_OP_lit1:
				case DW_OP_lit2:
				case DW_OP_lit3:
				case DW_OP_lit4:
				case DW_OP_lit5:
				case DW_OP_lit6:
				case DW_OP_lit7:
				case DW_OP_lit8:
				case DW_OP_lit9:
				case DW_OP_lit10:
				case DW_OP_lit11:
				case DW_OP_lit12:
				case DW_OP_lit13:
				case DW_OP_lit14:
				case DW_OP_lit15:
				case DW_OP_lit16:
				case DW_OP_lit17:
				case DW_OP_lit18:
				case DW_OP_lit19:
				case DW_OP_lit20:
				case DW_OP_lit21:
				case DW_OP_lit22:
				case DW_OP_lit23:
				case DW_OP_lit24:
				case DW_OP_lit25:
				case DW_OP_lit26:
				case DW_OP_lit27:
				case DW_OP_lit28:
				case DW_OP_lit29:
				case DW_OP_lit30:
				case DW_OP_lit31:
					{
						MEMORY_ADDR literal_value = (opcode - DW_OP_lit0);
						if(executing)
						{
							if(dw_stack.empty()) return false;
							dw_stack.push_back(literal_value);
						}
						if(os) *os << "DW_OP_lit" << literal_value;
					}
					break;
				case DW_OP_addr:
					{
						MEMORY_ADDR addr;
						if((expr_pos + file_address_size) > expr_length) return false;
						switch(file_address_size)
						{
							case 1:
								{
									uint8_t addr8;
									memcpy(&addr8, expr, file_address_size);
									addr8 = Target2Host(file_endianness, addr8);
									addr = addr8;
								}
								break;
							case 2:
								{
									uint16_t addr16;
									memcpy(&addr16, expr, file_address_size);
									addr16 = Target2Host(file_endianness, addr16);
									addr = addr16;
								}
								break;
							case 4:
								{
									uint32_t addr32;
									memcpy(&addr32, expr, file_address_size);
									addr32 = Target2Host(file_endianness, addr32);
									addr = addr32;
								}
								break;
							case 8:
								{
									uint64_t addr64;
									memcpy(&addr64, expr, file_address_size);
									addr64 = Target2Host(file_endianness, addr64);
									addr = addr64;
								}
								break;
							default:
								return -1;
						}
						expr_pos += file_address_size;
						if(executing) dw_stack.push_back(addr);
						if(os) *os << "DW_OP_addr 0x" << std::hex << addr << std::dec;
					}
					break;
				case DW_OP_const1u:
					{
						uint8_t dw_const;
						if((expr_pos + sizeof(dw_const)) > expr_length) return -1;
						memcpy(&dw_const, expr, sizeof(dw_const));
						dw_const = Target2Host(file_endianness, dw_const);
						expr_pos += sizeof(dw_const);
						MEMORY_ADDR value = dw_const;
						if(executing) dw_stack.push_back(value);
						if(os) *os << "DW_OP_const1u " << (uint32_t) dw_const;
					}
					break;
				case DW_OP_const1s:
					{
						uint8_t dw_const;
						if((expr_pos + sizeof(dw_const)) > expr_length) return -1;
						memcpy(&dw_const, expr, sizeof(dw_const));
						dw_const = Target2Host(file_endianness, dw_const);
						expr_pos += sizeof(dw_const);
						MEMORY_ADDR value = SignExtend((MEMORY_ADDR) dw_const, 8 * sizeof(dw_const));
						if(executing) dw_stack.push_back(value);
						if(os) *os << "DW_OP_const1s " << (int64_t)(int8_t) dw_const;
					}
					break;
				case DW_OP_const2u:
					{
						uint16_t dw_const;
						if((expr_pos + sizeof(dw_const)) > expr_length) return -1;
						memcpy(&dw_const, expr, sizeof(dw_const));
						dw_const = Target2Host(file_endianness, dw_const);
						expr_pos += sizeof(dw_const);
						MEMORY_ADDR value = dw_const;
						if(executing) dw_stack.push_back(value);
						if(os) *os << "DW_OP_const2u " << (uint32_t) dw_const;
					}
					break;
				case DW_OP_const2s:
					{
						uint16_t dw_const;
						if((expr_pos + sizeof(dw_const)) > expr_length) return -1;
						memcpy(&dw_const, expr, sizeof(dw_const));
						dw_const = Target2Host(file_endianness, dw_const);
						expr_pos += sizeof(dw_const);
						MEMORY_ADDR value = SignExtend((MEMORY_ADDR) dw_const, 8 * sizeof(dw_const));
						if(executing) dw_stack.push_back(value);
						if(os) *os << "DW_OP_const2s " << (int64_t)(int16_t) dw_const;
					}
					break;
				case DW_OP_const4u:
					{
						uint32_t dw_const;
						if((expr_pos + sizeof(dw_const)) > expr_length) return -1;
						memcpy(&dw_const, expr, sizeof(dw_const));
						dw_const = Target2Host(file_endianness, dw_const);
						expr_pos += sizeof(dw_const);
						MEMORY_ADDR value = dw_const;
						if(executing) dw_stack.push_back(value);
						if(os) *os << "DW_OP_const4u " << dw_const;
					}
					break;
				case DW_OP_const4s:
					{
						uint32_t dw_const;
						if((expr_pos + sizeof(dw_const)) > expr_length) return -1;
						memcpy(&dw_const, expr, sizeof(dw_const));
						dw_const = Target2Host(file_endianness, dw_const);
						expr_pos += sizeof(dw_const);
						MEMORY_ADDR value = SignExtend((MEMORY_ADDR) dw_const, 8 * sizeof(dw_const));
						if(executing) dw_stack.push_back(value);
						if(os) *os << "DW_OP_const4s " << (int64_t)(int32_t) dw_const;
					}
					break;
				case DW_OP_const8u:
					{
						uint64_t dw_const;
						if((expr_pos + sizeof(dw_const)) > expr_length) return -1;
						memcpy(&dw_const, expr, sizeof(dw_const));
						dw_const = Target2Host(file_endianness, dw_const);
						expr_pos += sizeof(dw_const);
						MEMORY_ADDR value = dw_const;
						if(executing) dw_stack.push_back(value);
						if(os) *os << "DW_OP_const8u " << dw_const;
					}
					break;
				case DW_OP_const8s:
					{
						uint64_t dw_const;
						if((expr_pos + sizeof(dw_const)) > expr_length) return -1;
						memcpy(&dw_const, expr, sizeof(dw_const));
						dw_const = Target2Host(file_endianness, dw_const);
						expr_pos += sizeof(dw_const);
						MEMORY_ADDR value = SignExtend((MEMORY_ADDR) dw_const, 8 * sizeof(dw_const));
						if(executing) dw_stack.push_back(value);
						if(os) *os << "DW_OP_const8s " << (int64_t) dw_const;
					}
					break;
				case DW_OP_constu:
					{
						DWARF_LEB128 dw_const_leb128;
						int64_t sz;
						if((sz = dw_const_leb128.Load(expr, expr_length - expr_pos)) < 0) return -1;
						expr_pos += sz;
						
						uint64_t value = (uint64_t) dw_const_leb128;
						MEMORY_ADDR addr = value;
						if(executing) dw_stack.push_back(addr);
						if(os) *os << "DW_OP_constu " << value;
					}
					break;
				case DW_OP_consts:
					{
						DWARF_LEB128 dw_const_leb128;
						int64_t sz;
						if((sz = dw_const_leb128.Load(expr, expr_length - expr_pos)) < 0) return -1;
						expr_pos += sz;
						
						int64_t value = (int64_t) dw_const_leb128;
						MEMORY_ADDR addr = value;
						if(executing) dw_stack.push_back(addr);
						if(os) *os << "DW_OP_consts " << value;
					}
					break;
				case DW_OP_fbreg:
					{
						DWARF_LEB128 dw_offset_leb128;
						int64_t sz;
						if((sz = dw_offset_leb128.Load(expr, expr_length - expr_pos)) < 0) return -1;
						expr_pos += sz;
						
						int64_t offset = (int64_t) dw_offset_leb128;
						if(executing)
						{
							// TODO: push onto the stack frame pointer + offset
						}
						if(os) *os << "DW_OP_fbreg " << offset;
					}
					break;
				case DW_OP_breg0:
				case DW_OP_breg1:
				case DW_OP_breg2:
				case DW_OP_breg3:
				case DW_OP_breg4:
				case DW_OP_breg5:
				case DW_OP_breg6:
				case DW_OP_breg7:
				case DW_OP_breg8:
				case DW_OP_breg9:
				case DW_OP_breg10:
				case DW_OP_breg11:
				case DW_OP_breg12:
				case DW_OP_breg13:
				case DW_OP_breg14:
				case DW_OP_breg15:
				case DW_OP_breg16:
				case DW_OP_breg17:
				case DW_OP_breg18:
				case DW_OP_breg19:
				case DW_OP_breg20:
				case DW_OP_breg21:
				case DW_OP_breg22:
				case DW_OP_breg23:
				case DW_OP_breg24:
				case DW_OP_breg25:
				case DW_OP_breg26:
				case DW_OP_breg27:
				case DW_OP_breg28:
				case DW_OP_breg29:
				case DW_OP_breg30:
				case DW_OP_breg31:
					{
						uint8_t dw_reg_num = opcode - DW_OP_breg0;
						DWARF_LEB128 dw_offset_leb128;
						int64_t sz;
						if((sz = dw_offset_leb128.Load(expr, expr_length - expr_pos)) < 0) return -1;
						expr_pos += sz;
						
						int64_t offset = (int64_t) dw_offset_leb128;
						if(executing)
						{
							MEMORY_ADDR reg_value = ReadRegister(dw_reg_num);
							// push onto the stack reg value + offset
							dw_stack.push_back(reg_value + offset);
						}
						if(os) *os << "DW_OP_breg" << (unsigned int) dw_reg_num << " " << offset;
					}
					break;
				case DW_OP_bregx:
					{
						DWARF_LEB128 dw_reg_num_leb128;
						int64_t sz;
						if((sz = dw_reg_num_leb128.Load(expr, expr_length - expr_pos)) < 0) return -1;
						expr_pos += sz;
						
						uint64_t dw_reg_num = (uint64_t) dw_reg_num_leb128;

						DWARF_LEB128 dw_offset_leb128;
						if((sz = dw_offset_leb128.Load(expr, expr_length - expr_pos)) < 0) return -1;
						expr_pos += sz;
						
						int64_t offset = (int64_t) dw_offset_leb128;
						
						if(executing)
						{
							MEMORY_ADDR reg_value = ReadRegister(dw_reg_num);
							// push onto the stack reg value + offset
							dw_stack.push_back(reg_value + offset);
						}
						if(os) *os << "DW_OP_bregx " << dw_reg_num;
					}
					break;
				case DW_OP_dup:
					if(executing)
					{
						if(dw_stack.empty()) return false;
						MEMORY_ADDR addr = dw_stack.back();
						dw_stack.push_back(addr);
					}
					if(os) *os << "DW_OP_dup";
					break;
				case DW_OP_drop:
					if(executing)
					{
						if(dw_stack.empty()) return false;
						dw_stack.pop_back();
					}
					if(os) *os << "DW_OP_drop";
					break;
				case DW_OP_pick:
					{
						uint8_t dw_index;
						if((expr_pos + sizeof(dw_index)) > expr_length) return -1;
						memcpy(&dw_index, expr, sizeof(dw_index));
						dw_index = Target2Host(file_endianness, dw_index);
						expr_pos += sizeof(dw_index);
						if(executing)
						{
							unsigned int dw_stack_size = dw_stack.size();
							if(dw_index >= dw_stack_size) return false;
							MEMORY_ADDR pick_value = dw_stack[dw_stack_size - 1 - dw_index];
							dw_stack.push_back(pick_value);
						}
						if(os) *os << "DW_OP_pick " << (uint32_t) dw_index;
					}
					break;
				case DW_OP_over:
					if(executing)
					{
						unsigned int dw_index = 1; // same as DW_OP_pick 1
						unsigned int dw_stack_size = dw_stack.size();
						if(dw_index >= dw_stack_size) return false;
						MEMORY_ADDR pick_value = dw_stack[dw_stack_size - 1 - dw_index];
						dw_stack.push_back(pick_value);
					}
					if(os) *os << "DW_OP_over";
					break;
				case DW_OP_swap:
					if(executing)
					{
						unsigned int dw_stack_size = dw_stack.size();
						if(dw_stack_size < 2) return false;
						MEMORY_ADDR second_value = dw_stack[dw_stack_size - 2];
						MEMORY_ADDR first_value = dw_stack[dw_stack_size - 1];
						// swap the top two stack entries
						dw_stack[dw_stack_size - 2] = first_value;
						dw_stack[dw_stack_size - 1] = second_value;
					}
					if(os) *os << "DW_OP_swap";
					break;
				case DW_OP_rot:
					if(executing)
					{
						unsigned int dw_stack_size = dw_stack.size();
						if(dw_stack_size < 3) return false;
						MEMORY_ADDR third_value = dw_stack[dw_stack_size - 3];
						MEMORY_ADDR second_value = dw_stack[dw_stack_size - 2];
						MEMORY_ADDR first_value = dw_stack[dw_stack_size - 1];
						// rotates the first three stack entries
						dw_stack[dw_stack_size - 3] = second_value;
						dw_stack[dw_stack_size - 2] = first_value;
						dw_stack[dw_stack_size - 1] = third_value;
					}
					if(os) *os << "DW_OP_rot";
					break;
				case DW_OP_deref:
					if(executing)
					{
						if(dw_stack.empty()) return false;
						MEMORY_ADDR addr = dw_stack.back();
						dw_stack.pop_back();
						MEMORY_ADDR read_addr = 0;
						
						if(!ReadAddrFromMemory(addr, read_addr)) return false;
						dw_stack.push_back(read_addr);
					}
					if(os) *os << "DW_OP_deref";
					break;
				case DW_OP_deref_size:
					{
						uint8_t dw_size;
						if((expr_pos + sizeof(dw_size)) > expr_length) return -1;
						memcpy(&dw_size, expr, sizeof(dw_size));
						dw_size = Target2Host(file_endianness, dw_size);
						expr_pos += sizeof(dw_size);
						if(executing)
						{
							if(dw_stack.empty()) return false;
							MEMORY_ADDR addr = dw_stack.back();
							dw_stack.pop_back();
							MEMORY_ADDR read_addr = 0;
							
							if(!ReadAddrFromMemory(addr, read_addr, dw_size)) return false;
							dw_stack.push_back(read_addr);
						}
						if(os) *os << "DW_OP_deref_size " << (uint32_t) dw_size;
					}
					break;
				case DW_OP_xderef:
					if(os) *os << "DW_OP_xderef";
					if(executing)
					{
						if(dw_stack.empty()) return false;
						MEMORY_ADDR addr = dw_stack.back();
						dw_stack.pop_back();
						MEMORY_ADDR addr_space = dw_stack.back();
						dw_stack.pop_back();
						MEMORY_ADDR read_addr = 0;
						
						if(!ReadAddrFromMemory(addr, read_addr, 0, addr_space)) return false;
						dw_stack.push_back(read_addr);
					}
					break;
				case DW_OP_xderef_size:
					{
						uint8_t dw_size;
						if((expr_pos + sizeof(dw_size)) > expr_length) return -1;
						memcpy(&dw_size, expr, sizeof(dw_size));
						dw_size = Target2Host(file_endianness, dw_size);
						expr_pos += sizeof(dw_size);
						if(executing)
						{
							if(dw_stack.empty()) return false;
							MEMORY_ADDR addr = dw_stack.back();
							dw_stack.pop_back();
							MEMORY_ADDR addr_space = dw_stack.back();
							dw_stack.pop_back();
							MEMORY_ADDR read_addr = 0;
							
							if(!ReadAddrFromMemory(addr, read_addr, dw_size, addr_space)) return false;
							dw_stack.push_back(read_addr);
						}
						if(os) *os << "DW_OP_xderef_size " << (uint32_t) dw_size;
					}
					break;
				case DW_OP_push_object_address:
					if(executing)
					{
						// DW_OP_push_object_address is not meaningful in an operand of these instructions because
						// there is no object context to provide a value to push.
						if(dw_cfp) return false;

						if(!dw_cu) return false;
						
						// Currently unimplemented.
						return false;
					}
					if(os) *os << "DW_OP_push_object_address";
					break;
				case DW_OP_form_tls_address:
					if(executing)
					{
						// Currently unimplemented.
						return false;
					}
					if(os) *os << "DW_OP_form_tls_address";
					break;
				case DW_OP_call_frame_cfa:
					if(executing)
					{
						// DW_OP_call_frame_cfa is not meaningful in an operand of these instructions because its use
						// would be circular.
						if(dw_cfp) return false;
						
						// Currently unimplemented. I don't understand the specification !
						return false;
					}
					if(os) *os << "DW_OP_call_frame_cfa";
					break;
				case DW_OP_abs:
					if(executing)
					{
						if(dw_stack.empty()) return false;
						MEMORY_ADDR op1 = dw_stack.back();
						if(op1 & (1ULL << ((8 * sizeof(MEMORY_ADDR)) - 1))) // 2's complement negative value
						{
							dw_stack.back() = -op1;
						}
					}
					if(os) *os << "DW_OP_abs";
					break;
				case DW_OP_and:
					if(executing)
					{
						if(dw_stack.size() < 2) return false;
						MEMORY_ADDR op1 = dw_stack.back();
						dw_stack.pop_back();
						MEMORY_ADDR op2 = dw_stack.back();
						dw_stack.pop_back();
						dw_stack.push_back(op1 & op2);
					}
					if(os) *os << "DW_OP_and";
					break;
				case DW_OP_div:
					if(executing)
					{
						if(dw_stack.size() < 2) return false;
						int64_t op1 = dw_stack.back();
						dw_stack.pop_back();
						int64_t op2 = dw_stack.back();
						dw_stack.pop_back();
						dw_stack.push_back(op1 ? (op2 / op1) : 0); // Note: division by zero is unspecified
					}
					if(os) *os << "DW_OP_div";
					break;
				case DW_OP_minus:
					if(executing)
					{
						if(dw_stack.size() < 2) return false;
						MEMORY_ADDR op1 = dw_stack.back();
						dw_stack.pop_back();
						MEMORY_ADDR op2 = dw_stack.back();
						dw_stack.pop_back();
						dw_stack.push_back(op2 - op1);
					}
					if(os) *os << "DW_OP_minus";
					break;
				case DW_OP_mod:
					if(executing)
					{
						if(dw_stack.size() < 2) return false;
						MEMORY_ADDR op1 = dw_stack.back();
						dw_stack.pop_back();
						MEMORY_ADDR op2 = dw_stack.back();
						dw_stack.pop_back();
						dw_stack.push_back(op1 ? (op2 % op1) : 0); // Note1: modulo zero is unspecified
						                                           // Note2: unsigned modulus
					}
					if(os) *os << "DW_OP_mod";
					break;
				case DW_OP_mul:
					if(executing)
					{
						if(dw_stack.size() < 2) return false;
						MEMORY_ADDR op1 = dw_stack.back();
						dw_stack.pop_back();
						MEMORY_ADDR op2 = dw_stack.back();
						dw_stack.pop_back();
						dw_stack.push_back(op2 * op1);
					}
					if(os) *os << "DW_OP_mul";
					break;
				case DW_OP_neg:
					if(executing)
					{
						if(dw_stack.empty()) return false;
						MEMORY_ADDR op1 = dw_stack.back();
						dw_stack.back() = -op1;
					}
					if(os) *os << "DW_OP_neg";
					break;
				case DW_OP_not:
					if(executing)
					{
						if(dw_stack.empty()) return false;
						MEMORY_ADDR op1 = dw_stack.back();
						dw_stack.back() = ~op1;
					}
					if(os) *os << "DW_OP_not";
					break;
				case DW_OP_or:
					if(executing)
					{
						if(dw_stack.size() < 2) return false;
						MEMORY_ADDR op1 = dw_stack.back();
						dw_stack.pop_back();
						MEMORY_ADDR op2 = dw_stack.back();
						dw_stack.pop_back();
						dw_stack.push_back(op2 | op1);
					}
					if(os) *os << "DW_OP_or";
					break;
				case DW_OP_plus:
					if(executing)
					{
						if(dw_stack.size() < 2) return false;
						MEMORY_ADDR op1 = dw_stack.back();
						dw_stack.pop_back();
						MEMORY_ADDR op2 = dw_stack.back();
						dw_stack.pop_back();
						dw_stack.push_back(op2 + op1);
					}
					if(os) *os << "DW_OP_plus";
					break;
				case DW_OP_plus_uconst:
					{
						DWARF_LEB128 dw_uconst_leb128;
						int64_t sz;
						if((sz = dw_uconst_leb128.Load(expr, expr_length - expr_pos)) < 0) return -1;
						expr_pos += sz;
						
						MEMORY_ADDR dw_uconst = (MEMORY_ADDR) dw_uconst_leb128;
						if(executing)
						{
							if(dw_stack.empty()) return false;
							MEMORY_ADDR op1 = dw_stack.back();
							dw_stack.pop_back();
							dw_stack.push_back(dw_uconst + op1);
						}
						if(os) *os << "DW_OP_plus_uconst " << dw_uconst;
					}
					break;
				case DW_OP_shl:
					if(executing)
					{
						if(dw_stack.size() < 2) return false;
						MEMORY_ADDR op1 = dw_stack.back();
						dw_stack.pop_back();
						MEMORY_ADDR op2 = dw_stack.back();
						dw_stack.pop_back();
						dw_stack.push_back(op2 << op1);
					}
					if(os) *os << "DW_OP_shl";
					break;
				case DW_OP_shr:
					if(executing)
					{
						if(dw_stack.size() < 2) return false;
						MEMORY_ADDR op1 = dw_stack.back();
						dw_stack.pop_back();
						MEMORY_ADDR op2 = dw_stack.back();
						dw_stack.pop_back();
						dw_stack.push_back(op2 >> op1);
					}
					if(os) *os << "DW_OP_shr";
					break;
				case DW_OP_shra:
					if(executing)
					{
						if(dw_stack.size() < 2) return false;
						MEMORY_ADDR op1 = dw_stack.back();
						dw_stack.pop_back();
						MEMORY_ADDR op2 = dw_stack.back();
						dw_stack.pop_back();
						dw_stack.push_back((signed) op2 >> op1);
					}
					if(os) *os << "DW_OP_shra";
					break;
				case DW_OP_xor:
					if(executing)
					{
						if(dw_stack.size() < 2) return false;
						MEMORY_ADDR op1 = dw_stack.back();
						dw_stack.pop_back();
						MEMORY_ADDR op2 = dw_stack.back();
						dw_stack.pop_back();
						dw_stack.push_back(op2 ^ op1);
					}
					if(os) *os << "DW_OP_xor";
					break;
				case DW_OP_le:
					if(executing)
					{
						if(dw_stack.size() < 2) return false;
						MEMORY_ADDR op1 = dw_stack.back();
						dw_stack.pop_back();
						MEMORY_ADDR op2 = dw_stack.back();
						dw_stack.pop_back();
						dw_stack.push_back(((signed) op2 <= (signed) op1) ? 1 : 0);
					}
					if(os) *os << "DW_OP_le";
					break;
				case DW_OP_ge:
					if(executing)
					{
						if(dw_stack.size() < 2) return false;
						MEMORY_ADDR op1 = dw_stack.back();
						dw_stack.pop_back();
						MEMORY_ADDR op2 = dw_stack.back();
						dw_stack.pop_back();
						dw_stack.push_back(((signed) op2 >= (signed) op1) ? 1 : 0);
					}
					if(os) *os << "DW_OP_ge";
					break;
				case DW_OP_eq:
					if(executing)
					{
						if(dw_stack.size() < 2) return false;
						MEMORY_ADDR op1 = dw_stack.back();
						dw_stack.pop_back();
						MEMORY_ADDR op2 = dw_stack.back();
						dw_stack.pop_back();
						dw_stack.push_back((op2 == op1) ? 1 : 0);
					}
					if(os) *os << "DW_OP_eq";
					break;
				case DW_OP_lt:
					if(executing)
					{
						if(dw_stack.size() < 2) return false;
						MEMORY_ADDR op1 = dw_stack.back();
						dw_stack.pop_back();
						MEMORY_ADDR op2 = dw_stack.back();
						dw_stack.pop_back();
						dw_stack.push_back(((signed) op2 < (signed) op1) ? 1 : 0);
					}
					if(os) *os << "DW_OP_lt";
					break;
				case DW_OP_gt:
					if(executing)
					{
						if(dw_stack.size() < 2) return false;
						MEMORY_ADDR op1 = dw_stack.back();
						dw_stack.pop_back();
						MEMORY_ADDR op2 = dw_stack.back();
						dw_stack.pop_back();
						dw_stack.push_back(((signed) op2 > (signed) op1) ? 1 : 0);
					}
					if(os) *os << "DW_OP_gt";
					break;
				case DW_OP_ne:
					if(executing)
					{
						if(dw_stack.size() < 2) return false;
						MEMORY_ADDR op1 = dw_stack.back();
						dw_stack.pop_back();
						MEMORY_ADDR op2 = dw_stack.back();
						dw_stack.pop_back();
						dw_stack.push_back((op2 != op1) ? 1 : 0);
					}
					if(os) *os << "DW_OP_ne";
					break;
				case DW_OP_skip:
					{
						int16_t dw_skip_amount;
						if((expr_pos + sizeof(dw_skip_amount)) > expr_length) return -1;
						memcpy(&dw_skip_amount, expr, sizeof(dw_skip_amount));
						dw_skip_amount = Target2Host(file_endianness, dw_skip_amount);
						expr_pos += sizeof(dw_skip_amount);
						if(executing)
						{
							if(dw_skip_amount > 0)
							{
								if(expr_pos + dw_skip_amount > expr_length) return false;
							}
							else
							{
								if(expr_pos < (uint64_t) -dw_skip_amount) return false;
							}
							
							expr_pos += dw_skip_amount;
						}
						if(os) *os << "DW_OP_skip " << dw_skip_amount;
					}
					break;
				case DW_OP_bra:
					{
						int16_t dw_skip_amount;
						if((expr_pos + sizeof(dw_skip_amount)) > expr_length) return -1;
						memcpy(&dw_skip_amount, expr, sizeof(dw_skip_amount));
						dw_skip_amount = Target2Host(file_endianness, dw_skip_amount);
						expr_pos += sizeof(dw_skip_amount);
						if(executing)
						{
							if(dw_skip_amount > 0)
							{
								if(expr_pos + dw_skip_amount > expr_length) return false;
							}
							else
							{
								if(expr_pos < (uint64_t) -dw_skip_amount) return false;
							}
							
							if(dw_stack.empty()) return false;
							MEMORY_ADDR cond = dw_stack.back();
							dw_stack.pop_back();
							if(cond) expr_pos += dw_skip_amount;
						}
						if(os) *os << "DW_OP_bra " << dw_skip_amount;
					}
					break;
				case DW_OP_call2:
					{
						// DW_OP_call2, DW_OP_call4 and DW_OP_call_ref operators are not meaningful in an
						// operand of these instructions because there is no mapping from call frame information to any
						// corresponding debugging compilation unit information, thus there is no way to interpret the call
						// offset.
						if(dw_cfp) return false;
						
						uint16_t debug_info_offset16;
						if((expr_pos + sizeof(debug_info_offset16)) > expr_length) return -1;
						memcpy(&debug_info_offset16, expr, sizeof(debug_info_offset16));
						debug_info_offset16 = Target2Host(file_endianness, debug_info_offset16);
						expr_pos += sizeof(debug_info_offset16);
						if(executing)
						{
							const DWARF_DIE<MEMORY_ADDR> *dw_die = dw_handler->FindDIE(debug_info_offset16);
							if(!dw_die) return false;
							const DWARF_Attribute<MEMORY_ADDR> *dw_at_location = dw_die->FindAttribute(DW_AT_location);
							if(dw_at_location)
							{
								const DWARF_AttributeValue<MEMORY_ADDR> *dw_at_location_value = dw_at_location->GetValue();
								if(dw_at_location_value->GetClass() != DW_CLASS_EXPRESSION) return false;
								const DWARF_Expression<MEMORY_ADDR> *dw_at_location_expr = (const DWARF_Expression<MEMORY_ADDR> *) dw_at_location_value;
								MEMORY_ADDR call_result_addr;
								DWARF_Location<MEMORY_ADDR> dw_call_location;
								bool call_status = Run(dw_at_location_expr, os, &call_result_addr, &dw_call_location);
								if(!call_status) return false;
							}
						}
						if(os) *os << "DW_OP_call2 " << debug_info_offset16;
					}
					break;
				case DW_OP_call4:
					{
						// DW_OP_call2, DW_OP_call4 and DW_OP_call_ref operators are not meaningful in an
						// operand of these instructions because there is no mapping from call frame information to any
						// corresponding debugging compilation unit information, thus there is no way to interpret the call
						// offset.
						if(dw_cfp) return false;
						
						uint32_t debug_info_offset32;
						if((expr_pos + sizeof(debug_info_offset32)) > expr_length) return -1;
						memcpy(&debug_info_offset32, expr, sizeof(debug_info_offset32));
						debug_info_offset32 = Target2Host(file_endianness, debug_info_offset32);
						expr_pos += sizeof(debug_info_offset32);
						if(executing)
						{
							const DWARF_DIE<MEMORY_ADDR> *dw_die = dw_handler->FindDIE(debug_info_offset32);
							if(!dw_die) return false;
							const DWARF_Attribute<MEMORY_ADDR> *dw_at_location = dw_die->FindAttribute(DW_AT_location);
							if(dw_at_location)
							{
								const DWARF_AttributeValue<MEMORY_ADDR> *dw_at_location_value = dw_at_location->GetValue();
								if(dw_at_location_value->GetClass() != DW_CLASS_EXPRESSION) return false;
								const DWARF_Expression<MEMORY_ADDR> *dw_at_location_expr = (const DWARF_Expression<MEMORY_ADDR> *) dw_at_location_value;
								MEMORY_ADDR call_result_addr;
								DWARF_Location<MEMORY_ADDR> dw_call_location;
								bool call_status = Run(dw_at_location_expr, os, &call_result_addr, &dw_call_location);
								if(!call_status) return false;
							}
						}
						if(os) *os << "DW_OP_call4 " << debug_info_offset32;
					}
					break;
				case DW_OP_call_ref:
					{
						// DW_OP_call2, DW_OP_call4 and DW_OP_call_ref operators are not meaningful in an
						// operand of these instructions because there is no mapping from call frame information to any
						// corresponding debugging compilation unit information, thus there is no way to interpret the call
						// offset.
						if(dw_cfp) return false;
						
						if(!dw_cu) return false;
						uint64_t debug_info_offset;
						switch(dw_cu->GetOffsetSize())
						{
							case 2:
								{
									uint16_t debug_info_offset16;
								
									if((expr_pos + sizeof(debug_info_offset16)) > expr_length) return -1;
									memcpy(&debug_info_offset16, expr, sizeof(debug_info_offset16));
									debug_info_offset16 = Target2Host(file_endianness, debug_info_offset16);
									expr_pos += sizeof(debug_info_offset16);
									debug_info_offset = debug_info_offset16;
								}
								break;
							case 4:
								{
									uint32_t debug_info_offset32;
								
									if((expr_pos + sizeof(debug_info_offset32)) > expr_length) return -1;
									memcpy(&debug_info_offset32, expr, sizeof(debug_info_offset32));
									debug_info_offset32 = Target2Host(file_endianness, debug_info_offset32);
									expr_pos += sizeof(debug_info_offset32);
									debug_info_offset = debug_info_offset32;
								}
								break;
							case 8:
								{
									uint64_t debug_info_offset64;
								
									if((expr_pos + sizeof(debug_info_offset64)) > expr_length) return -1;
									memcpy(&debug_info_offset64, expr, sizeof(debug_info_offset64));
									debug_info_offset64 = Target2Host(file_endianness, debug_info_offset64);
									expr_pos += sizeof(debug_info_offset64);
									debug_info_offset = debug_info_offset64;
								}
								break;
							default:
								return -1;
						}
						if(executing)
						{
							const DWARF_DIE<MEMORY_ADDR> *dw_die = dw_handler->FindDIE(debug_info_offset);
							if(!dw_die) return false;
							const DWARF_Attribute<MEMORY_ADDR> *dw_at_location = dw_die->FindAttribute(DW_AT_location);
							if(dw_at_location)
							{
								const DWARF_AttributeValue<MEMORY_ADDR> *dw_at_location_value = dw_at_location->GetValue();
								if(dw_at_location_value->GetClass() != DW_CLASS_EXPRESSION) return false;
								const DWARF_Expression<MEMORY_ADDR> *dw_at_location_expr = (const DWARF_Expression<MEMORY_ADDR> *) dw_at_location_value;
								MEMORY_ADDR call_result_addr;
								DWARF_Location<MEMORY_ADDR> dw_call_location;
								bool call_status = Run(dw_at_location_expr, os, &call_result_addr, &dw_call_location);
								if(!call_status) return false;
							}
						}
						if(os) *os << "DW_OP_call_ref " << debug_info_offset;
					}
					break;
				case DW_OP_nop:
					if(os) *os << "DW_OP_nop";
					break;
				case DW_OP_reg0:
				case DW_OP_reg1:
				case DW_OP_reg2:
				case DW_OP_reg3:
				case DW_OP_reg4:
				case DW_OP_reg5:
				case DW_OP_reg6:
				case DW_OP_reg7:
				case DW_OP_reg8:
				case DW_OP_reg9:
				case DW_OP_reg10:
				case DW_OP_reg11:
				case DW_OP_reg12:
				case DW_OP_reg13:
				case DW_OP_reg14:
				case DW_OP_reg15:
				case DW_OP_reg16:
				case DW_OP_reg17:
				case DW_OP_reg18:
				case DW_OP_reg19:
				case DW_OP_reg20:
				case DW_OP_reg21:
				case DW_OP_reg22:
				case DW_OP_reg23:
				case DW_OP_reg24:
				case DW_OP_reg25:
				case DW_OP_reg26:
				case DW_OP_reg27:
				case DW_OP_reg28:
				case DW_OP_reg29:
				case DW_OP_reg30:
				case DW_OP_reg31:
					{
						uint8_t dw_reg_num = opcode - DW_OP_reg0;
						if(executing)
						{
							if(!dw_location) return false; // DW_OP_reg* are only allowed in location expressions
							
							// Each register name operator must be used alone (as a DWARF expression
							// consisting of just that one operation).
							if((expr_pos != expr_length) && (expr[expr_pos] != DW_OP_piece)) return false;
							
							in_dw_op_reg = true; // indicate that we started a DW_OP_reg* and that the register number is at top of the stack
							dw_stack.push_back(dw_reg_num);
						}
						if(os) *os << "DW_OP_reg" << (unsigned int) dw_reg_num;
					}
					break;
				case DW_OP_regx:
					{
						DWARF_LEB128 dw_reg_num_leb128;
						int64_t sz;
						if((sz = dw_reg_num_leb128.Load(expr, expr_length - expr_pos)) < 0) return -1;
						expr_pos += sz;
						
						MEMORY_ADDR dw_reg_num = (MEMORY_ADDR) dw_reg_num_leb128;
						if(executing)
						{
							if(!dw_location) return false; // DW_OP_reg* are only allowed in location expressions

							// Each register name operator must be used alone (as a DWARF expression
							// consisting of just that one operation).
							if((expr_pos != expr_length) && (expr[expr_pos] != DW_OP_piece)) return false;
							
							in_dw_op_reg = true; // indicate that we started a DW_OP_reg* and that the register number is at top of the stack
							dw_stack.push_back(dw_reg_num);
						}
						if(os) *os << "DW_OP_regx " << dw_reg_num;
					}
					break;
				case DW_OP_piece:
					{
						DWARF_LEB128 dw_byte_size_leb128;
						int64_t sz;
						if((sz = dw_byte_size_leb128.Load(expr, expr_length - expr_pos)) < 0) return -1;
						expr_pos += sz;
						
						uint64_t dw_byte_size = (uint64_t) dw_byte_size_leb128;
						if(executing)
						{
							if(!dw_location) return false; // DW_OP_piece is only allowed in location expressions

							if(dw_stack.empty()) return false; // DW_OP_piece must be preceeded by a register name operator or an address operation that have pushed a register number or an address on the stack
							
							DWARF_LocationPiece<MEMORY_ADDR> *dw_loc_piece;
							if(in_dw_op_reg)
							{
								// register location
								MEMORY_ADDR dw_reg_num = dw_stack.back();
								dw_stack.pop_back();
								dw_loc_piece = new DWARF_RegisterLocationPiece<MEMORY_ADDR>(dw_reg_num);
								in_dw_op_reg = false;
							}
							else
							{
								// memory location
								MEMORY_ADDR addr = dw_stack.back();
								dw_stack.pop_back();
								dw_loc_piece = new DWARF_MemoryLocationPiece<MEMORY_ADDR>(addr);
							}
							
							dw_location->Add(dw_loc_piece);
							
							// Each simple location expression describes the location of one piece of the object;
							// each composition operator describes which part of the object is located there. Each simple
							// location expression that is a DWARF expression is evaluated independently of any others (as
							// though on its own separate stack).
							dw_stack.clear();
						}
						if(os) *os << "DW_OP_piece " << dw_byte_size;
					}
					break;
				case DW_OP_bit_piece:
					{
						DWARF_LEB128 dw_bit_size_leb128;
						int64_t sz;
						if((sz = dw_bit_size_leb128.Load(expr, expr_length - expr_pos)) < 0) return -1;
						expr_pos += sz;
						
						uint64_t dw_bit_size = (uint64_t) dw_bit_size_leb128;

						DWARF_LEB128 dw_bit_offset_leb128;
						if((sz = dw_bit_offset_leb128.Load(expr, expr_length - expr_pos)) < 0) return -1;
						expr_pos += sz;
						
						uint64_t dw_bit_offset = (uint64_t) dw_bit_offset_leb128;
						
						if(executing)
						{
							if(!dw_location) return false; // DW_OP_piece is only allowed in location expressions
							
							DWARF_LocationPiece<MEMORY_ADDR> *dw_loc_piece;
							if(in_dw_op_reg)
							{
								// register location
								MEMORY_ADDR dw_reg_num = dw_stack.back();
								dw_stack.pop_back();
								dw_loc_piece = new DWARF_RegisterLocationPiece<MEMORY_ADDR>(dw_reg_num, dw_bit_offset, dw_bit_size); // whole register
								in_dw_op_reg = false;
							}
							else
							{
								// memory location
								MEMORY_ADDR addr = dw_stack.back();
								dw_stack.pop_back();
								dw_loc_piece = new DWARF_MemoryLocationPiece<MEMORY_ADDR>(addr, dw_bit_offset, dw_bit_size);
							}
							
							dw_location->Add(dw_loc_piece);
						}
						if(os) *os << "DW_OP_bit_piece " << dw_bit_size << ", " << dw_bit_offset;
					}
					break;
				default:
					return false;
			}
			
			if(os && (expr_pos < expr_length))
			{
				*os << "; "; // add a separator
			}
		}
		while(expr_pos < expr_length);
	}
	
	if(result_addr) *result_addr = dw_stack.back();
	if(dw_location && !dw_stack.empty())
	{
		DWARF_LocationPiece<MEMORY_ADDR> *dw_loc_piece;
		
		if(in_dw_op_reg)
		{
			// register location
			MEMORY_ADDR dw_reg_num = dw_stack.back();
			dw_loc_piece = new DWARF_RegisterLocationPiece<MEMORY_ADDR>(dw_reg_num);
		}
		else
		{
			// memory location
			MEMORY_ADDR addr = dw_stack.back();
			dw_loc_piece = new DWARF_MemoryLocationPiece<MEMORY_ADDR>(addr);
		}
		
		dw_location->Add(dw_loc_piece);
	}

	return true;
}

template <class MEMORY_ADDR>
MEMORY_ADDR DWARF_ExpressionVM<MEMORY_ADDR>::ReadRegister(unsigned int dw_reg_num) const
{
	MEMORY_ADDR reg_value = 0;
	const unisim::util::debug::Register *arch_reg = reg_num_mapping->GetArchReg(dw_reg_num);
	
	if(arch_reg)
	{
		switch(arch_reg->GetSize())
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
	}
	return reg_value;
}

template <class MEMORY_ADDR>
bool DWARF_ExpressionVM<MEMORY_ADDR>::ReadAddrFromMemory(MEMORY_ADDR addr, MEMORY_ADDR& read_addr, unsigned int read_size, MEMORY_ADDR addr_space) const
{
	// FIXME: addr_space is currently ignored in our implementation
	if(read_size > arch_address_size) return false;
	if(!read_size) read_size = arch_address_size;
	
	switch(read_size)
	{
		case sizeof(uint8_t):
			{
				uint8_t value = 0;
				if(!mem_if->ReadMemory(addr, &value, read_size)) return false;
				read_addr = value;
			}
			break;
		case sizeof(uint16_t):
			{
				uint16_t value = 0;
				if(!mem_if->ReadMemory(addr, &value, read_size)) return false;
				read_addr = unisim::util::endian::Target2Host(arch_endianness, value);
			}
			break;
		case 3:
			{
				uint8_t buf[3] = { 0, 0, 0 };
				if(!mem_if->ReadMemory(addr, buf, read_size)) return false;
				switch(arch_endianness)
				{
					case E_BIG_ENDIAN:
						read_addr = ((uint32_t) buf[0] << 16) | ((uint32_t) buf[1] << 8) | (uint32_t) buf[2];
						break;
					case E_LITTLE_ENDIAN:
						read_addr = (uint32_t) buf[0] | ((uint32_t) buf[1] << 8) | ((uint32_t) buf[2] << 16);
						break;
					default:
						read_addr = 0;
						break;
				}
			}
			break;
		case sizeof(uint32_t):
			{
				uint32_t value = 0;
				if(!mem_if->ReadMemory(addr, &value, read_size)) return false;
				read_addr = unisim::util::endian::Target2Host(arch_endianness, value);
			}
			break;
		case 5:
			{
				uint8_t buf[5] = { 0, 0, 0, 0, 0 };
				if(!mem_if->ReadMemory(addr, buf, read_size)) return false;
				switch(arch_endianness)
				{
					case E_BIG_ENDIAN:
						read_addr = ((uint64_t) buf[0] << 32) | ((uint64_t) buf[1] << 24) | ((uint64_t) buf[2] << 16) | ((uint64_t) buf[3] << 8) | (uint64_t) buf[4];
						break;
					case E_LITTLE_ENDIAN:
						read_addr = (uint64_t) buf[0] | ((uint64_t) buf[1] << 8) | ((uint64_t) buf[2] << 16) | ((uint64_t) buf[3] << 24) | ((uint64_t) buf[4] << 32);
						break;
					default:
						read_addr = 0;
						break;
				}
			}
			break;
		case 6:
			{
				uint8_t buf[6] = { 0, 0, 0, 0, 0, 0 };
				if(!mem_if->ReadMemory(addr, buf, read_size)) return false;
				switch(arch_endianness)
				{
					case E_BIG_ENDIAN:
						read_addr = ((uint64_t) buf[0] << 40) | ((uint64_t) buf[1] << 32) | ((uint64_t) buf[2] << 24) | ((uint64_t) buf[3] << 16) | ((uint64_t) buf[4] << 8) | (uint64_t) buf[5];
						break;
					case E_LITTLE_ENDIAN:
						read_addr = (uint64_t) buf[0] | ((uint64_t) buf[1] << 8) | ((uint64_t) buf[2] << 16) | ((uint64_t) buf[3] << 24) | ((uint64_t) buf[4] << 32) | ((uint64_t) buf[4] << 40);
						break;
					default:
						read_addr = 0;
						break;
				}
			}
			break;
		case 7:
			{
				uint8_t buf[7] = { 0, 0, 0, 0, 0, 0, 0 };
				if(!mem_if->ReadMemory(addr, buf, read_size)) return false;
				switch(arch_endianness)
				{
					case E_BIG_ENDIAN:
						read_addr = ((uint64_t) buf[0] << 48) | ((uint64_t) buf[1] << 40) | ((uint64_t) buf[2] << 32) | ((uint64_t) buf[3] << 24) | ((uint64_t) buf[4] << 16) | ((uint64_t) buf[5] << 8) | (uint64_t) buf[6];
						break;
					case E_LITTLE_ENDIAN:
						read_addr = (uint64_t) buf[0] | ((uint64_t) buf[1] << 8) | ((uint64_t) buf[2] << 16) | ((uint64_t) buf[3] << 24) | ((uint64_t) buf[4] << 32) | ((uint64_t) buf[4] << 40) | ((uint64_t) buf[5] << 48);
						break;
					default:
						read_addr = 0;
						break;
				}
			}
			break;
		case sizeof(uint64_t):
			{
				uint32_t value = 0;
				if(!mem_if->ReadMemory(addr, &value, read_size)) return false;
				read_addr = unisim::util::endian::Target2Host(arch_endianness, value);
			}
			break;
		default:
			return false;
	}
	return true;
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
