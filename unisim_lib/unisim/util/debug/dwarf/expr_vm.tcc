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
#include <stack>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

using unisim::util::arithmetic::SignExtend;

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
DWARF_MemoryLocationPiece<MEMORY_ADDR>::DWARF_MemoryLocationPiece(MEMORY_ADDR _dw_addr, unsigned int _dw_bit_offset, unsigned int _dw_bit_size)
	: dw_addr(_dw_addr)
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
DWARF_RegisterLocationPiece<MEMORY_ADDR>::DWARF_RegisterLocationPiece(unisim::util::debug::Register *_dw_reg, unsigned int _dw_bit_offset, unsigned int _dw_bit_size)
	: dw_reg(_dw_reg)
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
	return dw_reg;
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
DWARF_ExpressionVM<MEMORY_ADDR>::DWARF_ExpressionVM(const DWARF_CompilationUnit<MEMORY_ADDR> *_dw_cu)
	: dw_cu(_dw_cu)
	, dw_cfp(0)
{
	//memcpy(registers, _registers, sizeof(registers));
}

template <class MEMORY_ADDR>
DWARF_ExpressionVM<MEMORY_ADDR>::DWARF_ExpressionVM(const DWARF_CallFrameProgram<MEMORY_ADDR> *_dw_cfp)
	: dw_cu(0)
	, dw_cfp(_dw_cfp)
{
	//memcpy(registers, _registers, sizeof(registers));
}

template <class MEMORY_ADDR>
DWARF_ExpressionVM<MEMORY_ADDR>::~DWARF_ExpressionVM()
{
}


template <class MEMORY_ADDR>
bool DWARF_ExpressionVM<MEMORY_ADDR>::Disasm(std::ostream& os, const DWARF_Expression<MEMORY_ADDR> *dw_expr)
{
	return Run(dw_expr, &os, 0);
}

template <class MEMORY_ADDR>
bool DWARF_ExpressionVM<MEMORY_ADDR>::Execute(const DWARF_Expression<MEMORY_ADDR> *dw_expr, DWARF_Location<MEMORY_ADDR> *dw_location)
{
	return Run(dw_expr, 0, dw_location);
}

template <class MEMORY_ADDR>
bool DWARF_ExpressionVM<MEMORY_ADDR>::Run(const DWARF_Expression<MEMORY_ADDR> *dw_expr, std::ostream *os, DWARF_Location<MEMORY_ADDR> *dw_location)
{
	std::stack<MEMORY_ADDR> dw_stack;
	uint8_t address_size = dw_cu ? dw_cu->GetAddressSize() : dw_cfp->GetAddressSize();
	endian_type endianness = dw_cu ? dw_cu->GetEndianness() : dw_cfp->GetEndianness();
//	uint8_t offset_size = dw_cu ? dw_cu->GetOffsetSize() : 0; // offsets are unused for expressions in a call frame program
	
	uint64_t expr_length = dw_expr->GetLength();
	const uint8_t *expr = dw_expr->GetValue();
	
	uint8_t opcode;
	
	if(expr_length)
	{
		do
		{
			opcode = *expr;
			expr++;
			expr_length--;
			
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
						dw_stack.push(literal_value);
						if(os) *os << "DW_OP_lit" << literal_value;
					}
					break;
				case DW_OP_addr:
					{
						MEMORY_ADDR addr;
						if(expr_length < address_size) return false;
						switch(address_size)
						{
							case 1:
								{
									uint8_t addr8;
									memcpy(&addr8, expr, address_size);
									addr8 = Target2Host(endianness, addr8);
									addr = addr8;
								}
								break;
							case 2:
								{
									// *** REDA *** replace uint8_t by uint16_t
									uint16_t addr16;
									memcpy(&addr16, expr, address_size);
									addr16 = Target2Host(endianness, addr16);
									addr = addr16;
								}
								break;
							case 4:
								{
									// *** REDA *** replace uint8_t by uint32_t
									uint32_t addr32;
									memcpy(&addr32, expr, address_size);
									addr32 = Target2Host(endianness, addr32);
									addr = addr32;
								}
								break;
							case 8:
								{
									uint64_t addr64;
									memcpy(&addr64, expr, address_size);
									addr64 = Target2Host(endianness, addr64);
									addr = addr64;
								}
								break;
							default:
								return -1;
						}
						expr += address_size;
						expr_length -= address_size;
						dw_stack.push(addr);
						if(os) *os << "DW_OP_addr 0x" << std::hex << addr << std::dec;
					}
					break;
				case DW_OP_const1u:
					{
						uint8_t dw_const;
						if(expr_length < sizeof(dw_const)) return -1;
						memcpy(&dw_const, expr, sizeof(dw_const));
						dw_const = Target2Host(endianness, dw_const);
						expr += sizeof(dw_const);
						expr_length -= sizeof(dw_const);
						MEMORY_ADDR value = dw_const;
						dw_stack.push(value);
						if(os) *os << "DW_OP_const1u " << (uint32_t) dw_const;
					}
					break;
				case DW_OP_const1s:
					{
						uint8_t dw_const;
						if(expr_length < sizeof(dw_const)) return -1;
						memcpy(&dw_const, expr, sizeof(dw_const));
						dw_const = Target2Host(endianness, dw_const);
						expr += sizeof(dw_const);
						expr_length -= sizeof(dw_const);
						MEMORY_ADDR value = SignExtend((MEMORY_ADDR) dw_const, 8 * sizeof(dw_const));
						dw_stack.push(value);
						if(os) *os << "DW_OP_const1s " << (int64_t)(int8_t) dw_const;
					}
					break;
				case DW_OP_const2u:
					{
						uint16_t dw_const;
						if(expr_length < sizeof(dw_const)) return -1;
						memcpy(&dw_const, expr, sizeof(dw_const));
						dw_const = Target2Host(endianness, dw_const);
						expr += sizeof(dw_const);
						expr_length -= sizeof(dw_const);
						MEMORY_ADDR value = dw_const;
						dw_stack.push(value);
						if(os) *os << "DW_OP_const2u " << (uint32_t) dw_const;
					}
					break;
				case DW_OP_const2s:
					{
						uint16_t dw_const;
						if(expr_length < sizeof(dw_const)) return -1;
						memcpy(&dw_const, expr, sizeof(dw_const));
						dw_const = Target2Host(endianness, dw_const);
						expr += sizeof(dw_const);
						expr_length -= sizeof(dw_const);
						MEMORY_ADDR value = SignExtend((MEMORY_ADDR) dw_const, 8 * sizeof(dw_const));
						dw_stack.push(value);
						if(os) *os << "DW_OP_const2s " << (int64_t)(int16_t) dw_const;
					}
					break;
				case DW_OP_const4u:
					{
						uint32_t dw_const;
						if(expr_length < sizeof(dw_const)) return -1;
						memcpy(&dw_const, expr, sizeof(dw_const));
						dw_const = Target2Host(endianness, dw_const);
						expr += sizeof(dw_const);
						expr_length -= sizeof(dw_const);
						MEMORY_ADDR value = dw_const;
						dw_stack.push(value);
						if(os) *os << "DW_OP_const4u " << dw_const;
					}
					break;
				case DW_OP_const4s:
					{
						uint32_t dw_const;
						if(expr_length < sizeof(dw_const)) return -1;
						memcpy(&dw_const, expr, sizeof(dw_const));
						dw_const = Target2Host(endianness, dw_const);
						expr += sizeof(dw_const);
						expr_length -= sizeof(dw_const);
						MEMORY_ADDR value = SignExtend((MEMORY_ADDR) dw_const, 8 * sizeof(dw_const));
						dw_stack.push(value);
						if(os) *os << "DW_OP_const4s " << (int64_t)(int32_t) dw_const;
					}
					break;
				case DW_OP_const8u:
					{
						uint64_t dw_const;
						if(expr_length < sizeof(dw_const)) return -1;
						memcpy(&dw_const, expr, sizeof(dw_const));
						dw_const = Target2Host(endianness, dw_const);
						expr += sizeof(dw_const);
						expr_length -= sizeof(dw_const);
						MEMORY_ADDR value = dw_const;
						dw_stack.push(value);
						if(os) *os << "DW_OP_const8u " << dw_const;
					}
					break;
				case DW_OP_const8s:
					{
						uint64_t dw_const;
						if(expr_length < sizeof(dw_const)) return -1;
						memcpy(&dw_const, expr, sizeof(dw_const));
						dw_const = Target2Host(endianness, dw_const);
						expr += sizeof(dw_const);
						expr_length -= sizeof(dw_const);
						MEMORY_ADDR value = SignExtend((MEMORY_ADDR) dw_const, 8 * sizeof(dw_const));
						dw_stack.push(value);
						if(os) *os << "DW_OP_const8s " << (int64_t) dw_const;
					}
					break;
				case DW_OP_constu:
					{
						DWARF_LEB128 dw_const_leb128;
						int64_t sz;
						if((sz = dw_const_leb128.Load(expr, expr_length)) < 0) return -1;
						expr += sz;
						expr_length -= sz;
						
						uint64_t value = (uint64_t) dw_const_leb128;
						MEMORY_ADDR addr = value;
						dw_stack.push(addr);
						if(os) *os << "DW_OP_constu " << value;
					}
					break;
				case DW_OP_consts:
					{
						DWARF_LEB128 dw_const_leb128;
						int64_t sz;
						if((sz = dw_const_leb128.Load(expr, expr_length)) < 0) return -1;
						expr += sz;
						expr_length -= sz;
						
						int64_t value = (int64_t) dw_const_leb128;
						MEMORY_ADDR addr = value;
						dw_stack.push(addr);
						if(os) *os << "DW_OP_consts " << value;
					}
					break;
				case DW_OP_fbreg:
					{
						DWARF_LEB128 dw_offset_leb128;
						int64_t sz;
						if((sz = dw_offset_leb128.Load(expr, expr_length)) < 0) return -1;
						expr += sz;
						expr_length -= sz;
						
						int64_t offset = (int64_t) dw_offset_leb128;
						// TODO: push onto the stack frame pointer + offset
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
						uint8_t reg_num = opcode - DW_OP_breg0;
						DWARF_LEB128 dw_offset_leb128;
						int64_t sz;
						if((sz = dw_offset_leb128.Load(expr, expr_length)) < 0) return -1;
						expr += sz;
						expr_length -= sz;
						
						int64_t offset = (int64_t) dw_offset_leb128;
						// TODO: push onto the stack reg value + offset
						if(os) *os << "DW_OP_breg" << (unsigned int) reg_num << " " << offset;
					}
					break;
				case DW_OP_dup:
					if(os) *os << "DW_OP_dup";
					break;
				case DW_OP_drop:
					if(os) *os << "DW_OP_drop";
					break;
				case DW_OP_pick:
					{
						uint8_t dw_index;
						if(expr_length < sizeof(dw_index)) return -1;
						memcpy(&dw_index, expr, sizeof(dw_index));
						dw_index = Target2Host(endianness, dw_index);
						expr += sizeof(dw_index);
						expr_length -= sizeof(dw_index);
						if(os) *os << "DW_OP_pick " << (uint32_t) dw_index;
					}
					break;
				case DW_OP_over:
					if(os) *os << "DW_OP_over";
					break;
				case DW_OP_swap:
					if(os) *os << "DW_OP_swap";
					break;
				case DW_OP_rot:
					if(os) *os << "DW_OP_rot";
					break;
				case DW_OP_deref:
					if(os) *os << "DW_OP_deref";
					break;
				case DW_OP_deref_size:
					{
						uint8_t dw_size;
						if(expr_length < sizeof(dw_size)) return -1;
						memcpy(&dw_size, expr, sizeof(dw_size));
						dw_size = Target2Host(endianness, dw_size);
						expr += sizeof(dw_size);
						expr_length -= sizeof(dw_size);
						if(os) *os << "DW_OP_deref_size " << (uint32_t) dw_size;
					}
					break;
				case DW_OP_xderef:
					if(os) *os << "DW_OP_xderef";
					break;
				case DW_OP_xderef_size:
					{
						uint8_t dw_size;
						if(expr_length < sizeof(dw_size)) return -1;
						memcpy(&dw_size, expr, sizeof(dw_size));
						dw_size = Target2Host(endianness, dw_size);
						expr += sizeof(dw_size);
						expr_length -= sizeof(dw_size);
						if(os) *os << "DW_OP_xderef_size " << (uint32_t) dw_size;
					}
					break;
				case DW_OP_push_object_address:
					if(!dw_cu) return false;
					if(os) *os << "DW_OP_push_object_address";
					break;
				case DW_OP_form_tls_address:
					if(os) *os << "DW_OP_form_tls_address";
					break;
				case DW_OP_call_frame_cfa:
					if(os) *os << "DW_OP_call_frame_cfa";
					break;
				case DW_OP_abs:
					if(os) *os << "DW_OP_abs";
					break;
				case DW_OP_and:
					if(os) *os << "DW_OP_and";
					break;
				case DW_OP_div:
					if(os) *os << "DW_OP_div";
					break;
				case DW_OP_minus:
					if(os) *os << "DW_OP_minus";
					break;
				case DW_OP_mod:
					if(os) *os << "DW_OP_mod";
					break;
				case DW_OP_mul:
					if(os) *os << "DW_OP_mul";
					break;
				case DW_OP_neg:
					if(os) *os << "DW_OP_neg";
					break;
				case DW_OP_not:
					if(os) *os << "DW_OP_not";
					break;
				case DW_OP_or:
					if(os) *os << "DW_OP_or";
					break;
				case DW_OP_plus:
					if(os) *os << "DW_OP_plus";
					break;
				case DW_OP_plus_uconst:
					{
						DWARF_LEB128 dw_uconst_leb128;
						int64_t sz;
						if((sz = dw_uconst_leb128.Load(expr, expr_length)) < 0) return -1;
						expr += sz;
						expr_length -= sz;
						
						uint64_t dw_uconst = (uint64_t) dw_uconst_leb128;
						if(os) *os << "DW_OP_plus_uconst " << dw_uconst;
					}
					break;
				case DW_OP_shl:
					if(os) *os << "DW_OP_shl";
					break;
				case DW_OP_shr:
					if(os) *os << "DW_OP_shr";
					break;
				case DW_OP_shra:
					if(os) *os << "DW_OP_shra";
					break;
				case DW_OP_xor:
					if(os) *os << "DW_OP_xor";
					break;
				case DW_OP_le:
					if(os) *os << "DW_OP_le";
					break;
				case DW_OP_ge:
					if(os) *os << "DW_OP_ge";
					break;
				case DW_OP_eq:
					if(os) *os << "DW_OP_eq";
					break;
				case DW_OP_lt:
					if(os) *os << "DW_OP_lt";
					break;
				case DW_OP_gt:
					if(os) *os << "DW_OP_gt";
					break;
				case DW_OP_ne:
					if(os) *os << "DW_OP_ne";
					break;
				case DW_OP_skip:
					{
						int16_t dw_skip_amount;
						if(expr_length < sizeof(dw_skip_amount)) return -1;
						memcpy(&dw_skip_amount, expr, sizeof(dw_skip_amount));
						dw_skip_amount = Target2Host(endianness, dw_skip_amount);
						expr += sizeof(dw_skip_amount);
						expr_length -= sizeof(dw_skip_amount);
						if(os) *os << "DW_OP_skip " << dw_skip_amount;
					}
					break;
				case DW_OP_bra:
					{
						int16_t dw_skip_amount;
						if(expr_length < sizeof(dw_skip_amount)) return -1;
						memcpy(&dw_skip_amount, expr, sizeof(dw_skip_amount));
						dw_skip_amount = Target2Host(endianness, dw_skip_amount);
						expr += sizeof(dw_skip_amount);
						expr_length -= sizeof(dw_skip_amount);
						if(os) *os << "DW_OP_bra " << dw_skip_amount;
					}
					break;
				case DW_OP_call2:
					{
						if(!dw_cu) return false;
						uint16_t cu_offset;
						if(expr_length < sizeof(cu_offset)) return -1;
						memcpy(&cu_offset, expr, sizeof(cu_offset));
						cu_offset = Target2Host(endianness, cu_offset);
						expr += sizeof(cu_offset);
						expr_length -= sizeof(cu_offset);
						uint64_t debug_info_offset = dw_cu->GetOffset() + cu_offset;
						if(os) *os << "DW_OP_call2 " << debug_info_offset;
					}
					break;
				case DW_OP_call4:
					{
						if(!dw_cu) return false;
						uint32_t cu_offset;
						if(expr_length < sizeof(cu_offset)) return -1;
						memcpy(&cu_offset, expr, sizeof(cu_offset));
						cu_offset = Target2Host(endianness, cu_offset);
						expr += sizeof(cu_offset);
						expr_length -= sizeof(cu_offset);
						uint64_t debug_info_offset = dw_cu->GetOffset() + cu_offset;
						if(os) *os << "DW_OP_call4 " << debug_info_offset;
					}
					break;
				case DW_OP_call_ref:
					{
						if(!dw_cu) return false;
						uint64_t debug_info_offset;
						switch(dw_cu->GetOffsetSize())
						{
							case 2:
								{
									uint16_t debug_info_offset16;
								
									if(expr_length < sizeof(debug_info_offset16)) return -1;
									memcpy(&debug_info_offset16, expr, sizeof(debug_info_offset16));
									debug_info_offset16 = Target2Host(endianness, debug_info_offset16);
									expr += sizeof(debug_info_offset16);
									expr_length -= sizeof(debug_info_offset16);
									debug_info_offset = debug_info_offset16;
								}
								break;
							case 4:
								{
									uint32_t debug_info_offset32;
								
									if(expr_length < sizeof(debug_info_offset32)) return -1;
									memcpy(&debug_info_offset32, expr, sizeof(debug_info_offset32));
									debug_info_offset32 = Target2Host(endianness, debug_info_offset32);
									expr += sizeof(debug_info_offset32);
									expr_length -= sizeof(debug_info_offset32);
									debug_info_offset = debug_info_offset32;
								}
								break;
							case 8:
								{
									uint64_t debug_info_offset64;
								
									if(expr_length < sizeof(debug_info_offset64)) return -1;
									memcpy(&debug_info_offset, expr, sizeof(debug_info_offset64));
									debug_info_offset64 = Target2Host(endianness, debug_info_offset64);
									expr += sizeof(debug_info_offset64);
									expr_length -= sizeof(debug_info_offset64);
									debug_info_offset = debug_info_offset64;
								}
								break;
							default:
								return -1;
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
						uint8_t reg_num = opcode - DW_OP_reg0;
						if(os) *os << "DW_OP_reg" << (unsigned int) reg_num;
					}
					break;
				case DW_OP_regx:
					{
						DWARF_LEB128 dw_reg_num_leb128;
						int64_t sz;
						if((sz = dw_reg_num_leb128.Load(expr, expr_length)) < 0) return -1;
						expr += sz;
						expr_length -= sz;
						
						unsigned int dw_reg_num = (unsigned int) dw_reg_num_leb128;
						if(os) *os << "DW_OP_regx " << dw_reg_num;
					}
					break;
				case DW_OP_piece:
					{
						DWARF_LEB128 dw_byte_size_leb128;
						int64_t sz;
						if((sz = dw_byte_size_leb128.Load(expr, expr_length)) < 0) return -1;
						expr += sz;
						expr_length -= sz;
						
						uint64_t dw_byte_size = (uint64_t) dw_byte_size_leb128;
						if(os) *os << "DW_OP_piece " << dw_byte_size;
					}
					break;
				case DW_OP_bit_piece:
					{
						DWARF_LEB128 dw_bit_size_leb128;
						int64_t sz;
						if((sz = dw_bit_size_leb128.Load(expr, expr_length)) < 0) return -1;
						expr += sz;
						expr_length -= sz;
						
						uint64_t dw_bit_size = (uint64_t) dw_bit_size_leb128;
						if(os) *os << "DW_OP_bit_piece " << dw_bit_size;
					}
					break;
				default:
					return false;
			}
			
			if(os && expr_length)
			{
				*os << "; "; // add a separator
			}
		}
		while(expr_length);
	}
	
	return true;
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
