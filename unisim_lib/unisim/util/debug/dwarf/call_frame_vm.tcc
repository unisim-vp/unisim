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

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
Rule<MEMORY_ADDR>::Rule(unsigned int _dw_rule_type)
	: dw_rule_type(_dw_rule_type)
{
}

template <class MEMORY_ADDR>
Rule<MEMORY_ADDR>::~Rule()
{
}

template <class MEMORY_ADDR>
unsigned int Rule<MEMORY_ADDR>::GetType() const
{
	return dw_rule_type;
}

template <class MEMORY_ADDR>
CFARule<MEMORY_ADDR>::CFARule(const DWARF_CIE<MEMORY_ADDR> *dw_cie)
	: Rule<MEMORY_ADDR>(DW_RULE_CFA)
	, dw_reg_num()
	, dw_offset_sf()
{
}

template <class MEMORY_ADDR>
CFARule<MEMORY_ADDR>::~CFARule()
{
}

template <class MEMORY_ADDR>
void CFARule<MEMORY_ADDR>::SetRegister(const DWARF_LEB128& _dw_reg_num)
{
	dw_reg_num = _dw_reg_num;
}

template <class MEMORY_ADDR>
void CFARule<MEMORY_ADDR>::SetFactoredOffset(const DWARF_LEB128& _dw_offset_sf)
{
	dw_offset_sf = _dw_offset_sf;
}

template <class MEMORY_ADDR>
void CFARule<MEMORY_ADDR>::SetOffset(const DWARF_LEB128& _dw_offset)
{
	// TODO
}

template <class MEMORY_ADDR>
const DWARF_LEB128& CFARule<MEMORY_ADDR>::GetRegisterNumber() const
{
	return dw_reg_num;
}

template <class MEMORY_ADDR>
int64_t CFARule<MEMORY_ADDR>::GetOffset() const
{
	return (int64_t) dw_offset_sf;
}

template <class MEMORY_ADDR>
OffsetRule<MEMORY_ADDR>::OffsetRule(const Rule<MEMORY_ADDR> *_dw_cfa_rule, const DWARF_LEB128& _dw_offset)
	: Rule<MEMORY_ADDR>(DW_RULE_OFFSET)
	, dw_cfa_rule(_dw_cfa_rule)
	, dw_offset(_dw_offset)
{
}

template <class MEMORY_ADDR>
OffsetRule<MEMORY_ADDR>::~OffsetRule()
{
}

template <class MEMORY_ADDR>
MEMORY_ADDR OffsetRule<MEMORY_ADDR>::GetValue() const
{
	// TODO
	return 0;
}

template <class MEMORY_ADDR>
ValOffsetRule<MEMORY_ADDR>::ValOffsetRule(const Rule<MEMORY_ADDR> *_dw_cfa_rule, const DWARF_LEB128& _dw_offset)
	: Rule<MEMORY_ADDR>(DW_RULE_VAL_OFFSET)
	, dw_cfa_rule(_dw_cfa_rule)
	, dw_offset(_dw_offset)
{
}

template <class MEMORY_ADDR>
ValOffsetRule<MEMORY_ADDR>::~ValOffsetRule()
{
}

template <class MEMORY_ADDR>
MEMORY_ADDR ValOffsetRule<MEMORY_ADDR>::GetValue() const
{
	// TODO
	return 0;
}

template <class MEMORY_ADDR>
RegisterRule<MEMORY_ADDR>::RegisterRule(const DWARF_LEB128& _dw_reg_num)
	: Rule<MEMORY_ADDR>(DW_RULE_REGISTER)
	, dw_reg_num(_dw_reg_num)
{
}

template <class MEMORY_ADDR>
RegisterRule<MEMORY_ADDR>::~RegisterRule()
{
}

template <class MEMORY_ADDR>
const DWARF_LEB128& RegisterRule<MEMORY_ADDR>::GetValue() const
{
	return dw_reg_num;
}

template <class MEMORY_ADDR>
ExpressionRule<MEMORY_ADDR>::ExpressionRule(const DWARF_Expression<MEMORY_ADDR> *_dw_expr)
	: Rule<MEMORY_ADDR>(DW_RULE_EXPRESSION)
	, dw_expr(_dw_expr)
{
}

template <class MEMORY_ADDR>
ExpressionRule<MEMORY_ADDR>::~ExpressionRule()
{
}

template <class MEMORY_ADDR>
MEMORY_ADDR ExpressionRule<MEMORY_ADDR>::GetValue() const
{
	// TODO
	return 0;
}

template <class MEMORY_ADDR>
ValExpressionRule<MEMORY_ADDR>::ValExpressionRule(const DWARF_Expression<MEMORY_ADDR> *_dw_expr)
	: Rule<MEMORY_ADDR>(DW_RULE_VAL_EXPRESSION)
	, dw_expr(_dw_expr)
{
}

template <class MEMORY_ADDR>
ValExpressionRule<MEMORY_ADDR>::~ValExpressionRule()
{
}

template <class MEMORY_ADDR>
DWARF_CallFrameVM<MEMORY_ADDR>::DWARF_CallFrameVM()
{
}

template <class MEMORY_ADDR>
DWARF_CallFrameVM<MEMORY_ADDR>::~DWARF_CallFrameVM()
{
}

template <class MEMORY_ADDR>
bool DWARF_CallFrameVM<MEMORY_ADDR>::Disasm(std::ostream& os, const DWARF_CallFrameProgram<MEMORY_ADDR>& dw_call_frame_prog)
{
	return Run(dw_call_frame_prog, &os, 0);
}

template <class MEMORY_ADDR>
bool DWARF_CallFrameVM<MEMORY_ADDR>::Execute(const DWARF_CallFrameProgram<MEMORY_ADDR>& dw_call_frame_prog, std::map<MEMORY_ADDR, RuleMatrixRow<MEMORY_ADDR> *>& rule_matrix)
{
	return Run(dw_call_frame_prog, 0, &rule_matrix);
}

template <class MEMORY_ADDR>
bool DWARF_CallFrameVM<MEMORY_ADDR>::Run(const DWARF_CallFrameProgram<MEMORY_ADDR>& dw_call_frame_prog, std::ostream *os, std::map<MEMORY_ADDR, RuleMatrixRow<MEMORY_ADDR> *> *rule_matrix)
{
	uint64_t program_length = dw_call_frame_prog.length;
	endian_type endianness = dw_call_frame_prog.GetEndianness();
	uint8_t address_size = dw_call_frame_prog.GetAddressSize();
	const uint8_t *program = dw_call_frame_prog.program;
	
	if(program_length)
	{
		do
		{
			uint8_t opcode = *program;
			program_length--;
			
			switch(opcode & 0xc0) // high 2 bits
			{
				case DW_CFA_advance_loc:
					{
						uint8_t delta = opcode & 0x3f;
						
						if(os) *os << "DW_CFA_advance_loc " << (uint32_t) delta;
					}
					break;
				case DW_CFA_offset:
					{
						uint8_t reg_num = opcode & 0x3f;
						DWARF_LEB128 offset;
						int64_t sz;
						
						if((sz = offset.Load(program, program_length)) < 0) return false;
						program += sz;
						program_length -= sz;
						if(os) *os << "DW_CFA_offset " << (uint32_t) reg_num << ", " << offset.to_string(false);
					}
					break;
				case DW_CFA_restore:
					{
						uint8_t reg_num = opcode & 0x3f;
						if(os) *os << "DW_CFA_restore " << (uint32_t) reg_num;
					}
					break;
				case 0x00:
					switch(opcode)
					{
						case DW_CFA_nop:
							if(os) *os << "DW_CFA_nop";
							break;
						case DW_CFA_set_loc:
							{
								MEMORY_ADDR addr;
								
								switch(address_size)
								{
									case sizeof(uint16_t):
										{
											if(program_length < sizeof(uint16_t)) return false;
											uint16_t value;
											memcpy(&value, program, sizeof(uint16_t));
											addr = Target2Host(endianness, value);
											program += sizeof(uint16_t);
											program_length -= sizeof(uint16_t);
										}
										break;
									case sizeof(uint32_t):
										{
											if(program_length < sizeof(uint32_t)) return false;
											uint32_t value;
											memcpy(&value, program, sizeof(uint32_t));
											addr = Target2Host(endianness, value);
											program += sizeof(uint32_t);
											program_length -= sizeof(uint32_t);
										}
										break;
									case sizeof(uint64_t):
										{
											if(program_length < sizeof(uint64_t)) return false;
											uint64_t value;
											memcpy(&value, program, sizeof(uint64_t));
											addr = Target2Host(endianness, value);
											program += sizeof(uint64_t);
											program_length -= sizeof(uint64_t);
										}
										break;
									default:
										return false;
								}
								if(os) *os << "DW_CFA_set_loc 0x" << std::hex << addr << std::dec;
							}
							break;
						case DW_CFA_advance_loc1:
							{
								uint8_t delta;
								
								if(program_length < sizeof(delta)) return false;
								memcpy(&delta, program, sizeof(delta));
								delta = Target2Host(dw_call_frame_prog.GetEndianness(), delta);
								program += sizeof(delta);
								program_length -= sizeof(delta);
								if(os) *os << "DW_CFA_advance_loc1 " << std::hex << (uint32_t) delta << std::dec;
							}
							break;
						case DW_CFA_advance_loc2:
							{
								uint16_t delta;
								
								if(program_length < sizeof(delta)) return false;
								memcpy(&delta, program, sizeof(delta));
								delta = Target2Host(dw_call_frame_prog.GetEndianness(), delta);
								program += sizeof(delta);
								program_length -= sizeof(delta);
								if(os) *os << "DW_CFA_advance_loc2 " << std::hex << delta << std::dec;
							}
							break;
						case DW_CFA_advance_loc4:
							{
								uint32_t delta;
								
								if(program_length < sizeof(delta)) return false;
								memcpy(&delta, program, sizeof(delta));
								delta = Target2Host(dw_call_frame_prog.GetEndianness(), delta);
								program += sizeof(delta);
								program_length -= sizeof(delta);
								if(os) *os << "DW_CFA_advance_loc4 " << std::hex << delta << std::dec;
							}
							break;
						case DW_CFA_offset_extended:
							{
								DWARF_LEB128 reg_num;
								DWARF_LEB128 offset;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if((sz = offset.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if(os) *os << "DW_CFA_offset_extended " << reg_num.to_string(false) << ", " << offset.to_string(false);
							}
							break;
						case DW_CFA_restore_extended:
							{
								DWARF_LEB128 reg_num;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if(os) *os << "DW_CFA_restore_extended " << reg_num.to_string(false);
							}
							break;
						case DW_CFA_undefined:
							{
								DWARF_LEB128 reg_num;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if(os) *os << "DW_CFA_undefined " << reg_num.to_string(false);
							}
							break;
						case DW_CFA_same_value:
							{
								DWARF_LEB128 reg_num;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if(os) *os << "DW_CFA_same_value " << reg_num.to_string(false);
							}
							break;
						case DW_CFA_register:
							{
								DWARF_LEB128 reg_num1;
								DWARF_LEB128 reg_num2;
								int64_t sz;
								
								if((sz = reg_num1.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if((sz = reg_num2.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if(os) *os << "DW_CFA_register " << reg_num1.to_string(false) << ", " << reg_num2.to_string(false);
							}
							break;
						case DW_CFA_remember_state:
							if(os) *os << "DW_CFA_remember_state";
							break;
						case DW_CFA_restore_state:
							if(os) *os << "DW_CFA_restore_state";
							break;
						case DW_CFA_def_cfa:
							{
								DWARF_LEB128 reg_num;
								DWARF_LEB128 offset;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if((sz = offset.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if(os) *os << "DW_CFA_def_cfa " << reg_num.to_string(false) << ", " << offset.to_string(false);
							}
							break;
						case DW_CFA_def_cfa_register:
							{
								DWARF_LEB128 reg_num;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if(os) *os << "DW_CFA_def_cfa_register " << reg_num.to_string(false);
							}
							break;
						case DW_CFA_def_cfa_offset:
							{
								DWARF_LEB128 offset;
								int64_t sz;
								
								if((sz = offset.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if(os) *os << "DW_CFA_def_cfa_offset " << offset.to_string(false);
							}
							break;
						case DW_CFA_def_cfa_expression:
							{
								DWARF_LEB128 leb128_block_length;
								int64_t sz;
								if((sz = leb128_block_length.Load(program, program_length)) < 0) return -1;
								program += sz;
								program_length -= sz;

								uint64_t block_length = (uint64_t) leb128_block_length;
								if(program_length < block_length) return -1;

								DWARF_Expression<MEMORY_ADDR> dw_expr = DWARF_Expression<MEMORY_ADDR>(&dw_call_frame_prog, block_length, program);
								program += block_length;
								program_length -= block_length;
								if(os) *os << "DW_CFA_def_cfa_expression {" << dw_expr.to_string() << "}";
							}
							break;
						case DW_CFA_expression:
							{
								DWARF_LEB128 reg_num;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;

								DWARF_LEB128 leb128_block_length;
								if((sz = leb128_block_length.Load(program, program_length)) < 0) return -1;
								program += sz;
								program_length -= sz;

								uint64_t block_length = (uint64_t) leb128_block_length;
								if(program_length < block_length) return -1;

								DWARF_Expression<MEMORY_ADDR> dw_expr = DWARF_Expression<MEMORY_ADDR>(&dw_call_frame_prog, block_length, program);
								program += block_length;
								program_length -= block_length;
								if(os) *os << "DW_CFA_expression " << reg_num.to_string(false) << " {" << dw_expr.to_string() << "}";
							}
							break;
						case DW_CFA_offset_extended_sf:
							{
								DWARF_LEB128 reg_num;
								DWARF_LEB128 offset;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if((sz = offset.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if(os) *os << "DW_CFA_offset_extended_sf " << reg_num.to_string(false) << ", " << offset.to_string(true);
							}
							break;
						case DW_CFA_def_cfa_sf:
							{
								DWARF_LEB128 reg_num;
								DWARF_LEB128 offset;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if((sz = offset.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if(os) *os << "DW_CFA_def_cfa_sf " << reg_num.to_string(false) << ", " << offset.to_string(true);
							}
							break;
						case DW_CFA_def_cfa_offset_sf:
							{
								DWARF_LEB128 offset;
								int64_t sz;
								
								if((sz = offset.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if(os) *os << "DW_CFA_def_cfa_offset_sf " << offset.to_string(true);
							}
							break;
						case DW_CFA_val_offset:
							{
								DWARF_LEB128 reg_num;
								DWARF_LEB128 offset;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if((sz = offset.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if(os) *os << "DW_CFA_val_offset " << reg_num.to_string(false) << ", " << offset.to_string(false);
							}
							break;
						case DW_CFA_val_offset_sf:
							{
								DWARF_LEB128 reg_num;
								DWARF_LEB128 offset;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if((sz = offset.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if(os) *os << "DW_CFA_val_offset_sf " << reg_num.to_string(false) << ", " << offset.to_string(true);
							}
							break;
						case DW_CFA_val_expression:
							{
								DWARF_LEB128 reg_num;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;

								DWARF_LEB128 leb128_block_length;
								if((sz = leb128_block_length.Load(program, program_length)) < 0) return -1;
								program += sz;
								program_length -= sz;

								uint64_t block_length = (uint64_t) leb128_block_length;
								if(program_length < block_length) return -1;

								DWARF_Expression<MEMORY_ADDR> dw_expr = DWARF_Expression<MEMORY_ADDR>(&dw_call_frame_prog, block_length, program);
								program += block_length;
								program_length -= block_length;
								if(os) *os << "DW_CFA_val_expression " << reg_num.to_string(false) << " {" << dw_expr.to_string() << "}";
							}
							break;
						default:
							return false;
					}
					break;
				default:
					return false;
			}
			if(os && program_length) *os << "; ";
		}
		while(program_length);
	}

	return true;
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim