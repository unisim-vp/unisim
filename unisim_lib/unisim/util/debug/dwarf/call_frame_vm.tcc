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
CFARule<MEMORY_ADDR>::CFARule(unsigned int _type)
	: type(_type)
{
}

template <class MEMORY_ADDR>
CFARule<MEMORY_ADDR>::~CFARule()
{
}

template <class MEMORY_ADDR>
unsigned int CFARule<MEMORY_ADDR>::GetType() const
{
	return type;
}

template <class MEMORY_ADDR>
CFARuleRegisterOffset<MEMORY_ADDR>::CFARuleRegisterOffset(unsigned int _dw_reg_num, int64_t _dw_offset)
	: CFARule<MEMORY_ADDR>(DW_CFA_RULE_REGISTER_OFFSET)
	, dw_reg_num(_dw_reg_num)
	, dw_offset(_dw_offset)
{
}

template <class MEMORY_ADDR>
CFARuleRegisterOffset<MEMORY_ADDR>::CFARuleRegisterOffset(const CFARuleRegisterOffset<MEMORY_ADDR>& cfa_rule_offset)
	: CFARule<MEMORY_ADDR>(DW_CFA_RULE_REGISTER_OFFSET)
	, dw_reg_num(cfa_rule_offset.dw_reg_num)
	, dw_offset(cfa_rule_offset.dw_offset)
{
}

template <class MEMORY_ADDR>
CFARuleRegisterOffset<MEMORY_ADDR>::~CFARuleRegisterOffset()
{
}

template <class MEMORY_ADDR>
void CFARuleRegisterOffset<MEMORY_ADDR>::SetRegisterNumber(unsigned int _dw_reg_num)
{
	dw_reg_num = _dw_reg_num;
}

template <class MEMORY_ADDR>
void CFARuleRegisterOffset<MEMORY_ADDR>::SetOffset(int64_t _dw_offset)
{
	dw_offset = _dw_offset;
}

template <class MEMORY_ADDR>
unsigned int CFARuleRegisterOffset<MEMORY_ADDR>::GetRegisterNumber() const
{
	return dw_reg_num;
}

template <class MEMORY_ADDR>
int64_t CFARuleRegisterOffset<MEMORY_ADDR>::GetOffset() const
{
	return dw_offset;
}

template <class MEMORY_ADDR>
CFARuleExpression<MEMORY_ADDR>::CFARuleExpression(const DWARF_Expression<MEMORY_ADDR> *_dw_expr)
	: CFARule<MEMORY_ADDR>(DW_CFA_RULE_EXPRESSION)
	, dw_expr(_dw_expr)
{
}

template <class MEMORY_ADDR>
CFARuleExpression<MEMORY_ADDR>::CFARuleExpression(const CFARuleExpression<MEMORY_ADDR>& cfa_rule_expression)
	: CFARule<MEMORY_ADDR>(DW_CFA_RULE_EXPRESSION)
	, dw_expr(new DWARF_Expression<MEMORY_ADDR>(*cfa_rule_expression.dw_expr))
{
}

template <class MEMORY_ADDR>
CFARuleExpression<MEMORY_ADDR>::~CFARuleExpression()
{
	if(dw_expr) delete dw_expr;
}

template <class MEMORY_ADDR>
RegisterRule<MEMORY_ADDR>::RegisterRule(unsigned int _dw_rule_type)
	: dw_rule_type(_dw_rule_type)
{
}

template <class MEMORY_ADDR>
RegisterRule<MEMORY_ADDR>::~RegisterRule()
{
}

template <class MEMORY_ADDR>
unsigned int RegisterRule<MEMORY_ADDR>::GetType() const
{
	return dw_rule_type;
}

template <class MEMORY_ADDR>
RegisterRuleUndefined<MEMORY_ADDR>::RegisterRuleUndefined(const RegisterRuleUndefined<MEMORY_ADDR>& rule)
	: RegisterRule<MEMORY_ADDR>(DW_REG_RULE_REGISTER)
{
}

template <class MEMORY_ADDR>
RegisterRuleUndefined<MEMORY_ADDR>::RegisterRuleUndefined()
	: RegisterRule<MEMORY_ADDR>(DW_REG_RULE_REGISTER)
{
}

template <class MEMORY_ADDR>
RegisterRuleUndefined<MEMORY_ADDR>::~RegisterRuleUndefined()
{
}

template <class MEMORY_ADDR>
RegisterRuleSameValue<MEMORY_ADDR>::RegisterRuleSameValue(const RegisterRuleSameValue<MEMORY_ADDR>& rule)
	: RegisterRule<MEMORY_ADDR>(DW_REG_RULE_SAME_VALUE)
{
}

template <class MEMORY_ADDR>
RegisterRuleSameValue<MEMORY_ADDR>::RegisterRuleSameValue()
	: RegisterRule<MEMORY_ADDR>(DW_REG_RULE_SAME_VALUE)
{
}

template <class MEMORY_ADDR>
RegisterRuleSameValue<MEMORY_ADDR>::~RegisterRuleSameValue()
{
}

template <class MEMORY_ADDR>
RegisterRuleOffset<MEMORY_ADDR>::RegisterRuleOffset(const RegisterRuleOffset<MEMORY_ADDR>& rule)
	: RegisterRule<MEMORY_ADDR>(DW_REG_RULE_OFFSET)
	, dw_offset(rule.dw_offset)
{
}

template <class MEMORY_ADDR>
RegisterRuleOffset<MEMORY_ADDR>::RegisterRuleOffset(int64_t _dw_offset)
	: RegisterRule<MEMORY_ADDR>(DW_REG_RULE_OFFSET)
	, dw_offset(_dw_offset)
{
}

template <class MEMORY_ADDR>
RegisterRuleOffset<MEMORY_ADDR>::~RegisterRuleOffset()
{
}

template <class MEMORY_ADDR>
int64_t RegisterRuleOffset<MEMORY_ADDR>::GetOffset() const
{
	return dw_offset;
}

template <class MEMORY_ADDR>
RegisterRuleValOffset<MEMORY_ADDR>::RegisterRuleValOffset(const RegisterRuleValOffset<MEMORY_ADDR>& rule)
	: RegisterRule<MEMORY_ADDR>(DW_REG_RULE_VAL_OFFSET)
	, dw_offset(rule.dw_offset)
{
}

template <class MEMORY_ADDR>
RegisterRuleValOffset<MEMORY_ADDR>::RegisterRuleValOffset(int64_t _dw_offset)
	: RegisterRule<MEMORY_ADDR>(DW_REG_RULE_VAL_OFFSET)
	, dw_offset(_dw_offset)
{
}

template <class MEMORY_ADDR>
RegisterRuleValOffset<MEMORY_ADDR>::~RegisterRuleValOffset()
{
}

template <class MEMORY_ADDR>
int64_t RegisterRuleValOffset<MEMORY_ADDR>::GetOffset() const
{
	return dw_offset;
}

template <class MEMORY_ADDR>
RegisterRuleRegister<MEMORY_ADDR>::RegisterRuleRegister(const RegisterRuleRegister<MEMORY_ADDR>& rule)
	: RegisterRule<MEMORY_ADDR>(DW_REG_RULE_REGISTER)
	, dw_reg_num(rule.dw_reg_num)
{
}

template <class MEMORY_ADDR>
RegisterRuleRegister<MEMORY_ADDR>::RegisterRuleRegister(unsigned int _dw_reg_num)
	: RegisterRule<MEMORY_ADDR>(DW_REG_RULE_REGISTER)
	, dw_reg_num(_dw_reg_num)
{
}

template <class MEMORY_ADDR>
RegisterRuleRegister<MEMORY_ADDR>::~RegisterRuleRegister()
{
}

template <class MEMORY_ADDR>
unsigned int RegisterRuleRegister<MEMORY_ADDR>::GetRegisterNumber() const
{
	return dw_reg_num;
}

template <class MEMORY_ADDR>
RegisterRuleExpression<MEMORY_ADDR>::RegisterRuleExpression(const RegisterRuleExpression<MEMORY_ADDR>& rule)
	: RegisterRule<MEMORY_ADDR>(DW_REG_RULE_EXPRESSION)
	, dw_expr(rule.dw_expr) // FIXME: copy
{
}

template <class MEMORY_ADDR>
RegisterRuleExpression<MEMORY_ADDR>::RegisterRuleExpression(const DWARF_Expression<MEMORY_ADDR> *_dw_expr)
	: RegisterRule<MEMORY_ADDR>(DW_REG_RULE_EXPRESSION)
	, dw_expr(_dw_expr)
{
}

template <class MEMORY_ADDR>
RegisterRuleExpression<MEMORY_ADDR>::~RegisterRuleExpression()
{
}

template <class MEMORY_ADDR>
const DWARF_Expression<MEMORY_ADDR> *RegisterRuleExpression<MEMORY_ADDR>::GetExpression() const
{
	return dw_expr;
}

template <class MEMORY_ADDR>
RegisterRuleValExpression<MEMORY_ADDR>::RegisterRuleValExpression(const RegisterRuleValExpression<MEMORY_ADDR>& rule)
	: RegisterRule<MEMORY_ADDR>(DW_REG_RULE_VAL_EXPRESSION)
	, dw_expr(rule.dw_expr)
{
}

template <class MEMORY_ADDR>
RegisterRuleValExpression<MEMORY_ADDR>::RegisterRuleValExpression(const DWARF_Expression<MEMORY_ADDR> *_dw_expr)
	: RegisterRule<MEMORY_ADDR>(DW_REG_RULE_VAL_EXPRESSION)
	, dw_expr(_dw_expr)
{
}

template <class MEMORY_ADDR>
RegisterRuleValExpression<MEMORY_ADDR>::~RegisterRuleValExpression()
{
}

template <class MEMORY_ADDR>
RuleMatrixRow<MEMORY_ADDR>::RuleMatrixRow(MEMORY_ADDR _location)
	: location(_location)
	, cfa_rule(0)
	, reg_rules()
{
}

template <class MEMORY_ADDR>
RuleMatrixRow<MEMORY_ADDR>::RuleMatrixRow(MEMORY_ADDR _location, const RuleMatrixRow<MEMORY_ADDR>& rule_matrix_row)
	: location(_location)
	, cfa_rule(0)
	, reg_rules()
{
	cfa_rule = new CFARule<MEMORY_ADDR>(*rule_matrix_row.cfa_rule);
	
	unsigned int i;
	unsigned int n = rule_matrix_row.reg_rules.size();
	
	for(i = 0; i < n; i++)
	{
		RegisterRule<MEMORY_ADDR> *rule = rule_matrix_row.reg_rules[i];
		RegisterRule<MEMORY_ADDR> *rule_copy = 0;
		
		switch(rule->GetType())
		{
			case DW_REG_RULE_UNDEFINED:
				rule_copy = new RegisterRuleUndefined<MEMORY_ADDR>(*reinterpret_cast<RegisterRuleUndefined<MEMORY_ADDR> *>(rule));
				break;
			case DW_REG_RULE_SAME_VALUE:
				break;
			case DW_REG_RULE_OFFSET:
				break;
			case DW_REG_RULE_VAL_OFFSET:
				break;
			case DW_REG_RULE_REGISTER:
				break;
			case DW_REG_RULE_EXPRESSION:
				break;
			case DW_REG_RULE_VAL_EXPRESSION:
				break;
		}
		reg_rules.push_back(rule_copy);
	}
}

template <class MEMORY_ADDR>
RuleMatrixRow<MEMORY_ADDR>::~RuleMatrixRow()
{
	if(cfa_rule) delete cfa_rule;
	unsigned int n = reg_rules.size();
	unsigned int i;
	for(i = 0; i < n; i++)
	{
		RegisterRule<MEMORY_ADDR> *reg_rule = reg_rules[i];
		if(reg_rule) delete reg_rule;
	}
}

template <class MEMORY_ADDR>
void RuleMatrixRow<MEMORY_ADDR>::SetCFARule(CFARule<MEMORY_ADDR> *_cfa_rule)
{
	if(cfa_rule) delete cfa_rule;
	cfa_rule = _cfa_rule;
}

template <class MEMORY_ADDR>
void RuleMatrixRow<MEMORY_ADDR>::SetRegisterRule(unsigned int reg_num, RegisterRule<MEMORY_ADDR> *reg_rule)
{
	if(reg_num >= reg_rules.size())
	{
		reg_rules.resize(reg_num + 1);
	}
	
	RegisterRule<MEMORY_ADDR> *old_reg_rule = reg_rules[reg_num];
	
	if(old_reg_rule) delete old_reg_rule;
	reg_rules[reg_num] = reg_rule;
}

template <class MEMORY_ADDR>
MEMORY_ADDR RuleMatrixRow<MEMORY_ADDR>::GetLocation() const
{
	return location;
}

template <class MEMORY_ADDR>
CFARule<MEMORY_ADDR> *RuleMatrixRow<MEMORY_ADDR>::GetCFARule() const
{
	return cfa_rule;
}

template <class MEMORY_ADDR>
RegisterRule<MEMORY_ADDR> *RuleMatrixRow<MEMORY_ADDR>::GetRegisterRule(unsigned int reg_num) const
{
	return (reg_num < reg_rules.size()) ? reg_rules[reg_num] : 0;
}

template <class MEMORY_ADDR>
RuleMatrix<MEMORY_ADDR>::RuleMatrix()
{
}

template <class MEMORY_ADDR>
RuleMatrix<MEMORY_ADDR>::RuleMatrix(const RuleMatrix<MEMORY_ADDR>& rule_matrix)
{
	// FIXME: copy the stack too
	typename std::map<MEMORY_ADDR, RuleMatrixRow<MEMORY_ADDR> *>::const_iterator iter;
	
	for(iter = rule_matrix.rule_matrix_rows.begin(); iter != rule_matrix.rule_matrix_rows.end(); iter++)
	{
		RuleMatrixRow<MEMORY_ADDR> *row = (*iter).second;
		rule_matrix_rows.insert(std::pair<MEMORY_ADDR, RuleMatrixRow<MEMORY_ADDR> *>(row->GetLocation(), new RuleMatrixRow<MEMORY_ADDR>(*row)));
	}
}

template <class MEMORY_ADDR>
RuleMatrix<MEMORY_ADDR>::~RuleMatrix()
{
	while(!state_stack.empty())
	{
		RuleMatrixRow<MEMORY_ADDR> *row = state_stack.top();
		state_stack.pop();
		delete row;
	}

	typename std::map<MEMORY_ADDR, RuleMatrixRow<MEMORY_ADDR> *>::iterator iter;
	
	for(iter = rule_matrix_rows.begin(); iter != rule_matrix_rows.end(); iter++)
	{
		delete (*iter).second;
	}
}

template <class MEMORY_ADDR>
RuleMatrixRow<MEMORY_ADDR> *RuleMatrix<MEMORY_ADDR>::operator[](MEMORY_ADDR loc)
{
	typename std::map<MEMORY_ADDR, RuleMatrixRow<MEMORY_ADDR> *>::iterator iter = rule_matrix_rows.find(loc);
	
	return (iter != rule_matrix_rows.end()) ? (*iter).second : 0;
}

template <class MEMORY_ADDR>
bool RuleMatrix<MEMORY_ADDR>::HasRow(MEMORY_ADDR loc) const
{
	typename std::map<MEMORY_ADDR, RuleMatrixRow<MEMORY_ADDR> *>::iterator iter = rule_matrix_rows.find(loc);
	return (iter != rule_matrix_rows.end());
}

template <class MEMORY_ADDR>
void RuleMatrix<MEMORY_ADDR>::InsertRow(RuleMatrixRow<MEMORY_ADDR> *rule_matrix_row)
{
	typename std::map<MEMORY_ADDR, RuleMatrixRow<MEMORY_ADDR> *>::iterator iter = rule_matrix_rows.find(rule_matrix_row->GetLocation());
	
	if(iter != rule_matrix_rows.end())
	{
		rule_matrix_rows.erase(iter);
	}
	
	rule_matrix_rows.insert(std::pair<MEMORY_ADDR, RuleMatrixRow<MEMORY_ADDR> *>(rule_matrix_row->GetLocation(), rule_matrix_row));
}

template <class MEMORY_ADDR>
void RuleMatrix<MEMORY_ADDR>::CloneRow(MEMORY_ADDR cur_loc, MEMORY_ADDR new_loc)
{
	typename std::map<MEMORY_ADDR, RuleMatrixRow<MEMORY_ADDR> *>::iterator iter = rule_matrix_rows.find(cur_loc);
	
	if(iter != rule_matrix_rows.end())
	{
		RuleMatrixRow<MEMORY_ADDR> *new_row = new RuleMatrixRow<MEMORY_ADDR>(new_loc, *(*iter).second);
		rule_matrix_rows.insert(std::pair<MEMORY_ADDR, RuleMatrixRow<MEMORY_ADDR> *>(new_loc, new_row));
	}
	else
	{
		RuleMatrixRow<MEMORY_ADDR> *new_row = new RuleMatrixRow<MEMORY_ADDR>(new_loc);
		rule_matrix_rows.insert(std::pair<MEMORY_ADDR, RuleMatrixRow<MEMORY_ADDR> *>(new_loc, new_row));
	}
}

template <class MEMORY_ADDR>
void RuleMatrix<MEMORY_ADDR>::RememberState(MEMORY_ADDR loc)
{
	typename std::map<MEMORY_ADDR, RuleMatrixRow<MEMORY_ADDR> *>::iterator iter = rule_matrix_rows.find(loc);
	
	if(iter != rule_matrix_rows.end())
	{
		RuleMatrixRow<MEMORY_ADDR> *row_copy = new RuleMatrixRow<MEMORY_ADDR>(loc, *(*iter).second);
		state_stack.push(row_copy);
	}
}

template <class MEMORY_ADDR>
void RuleMatrix<MEMORY_ADDR>::RestoreState(MEMORY_ADDR loc)
{
	if(!state_stack.empty())
	{
		RuleMatrixRow<MEMORY_ADDR> *row_copy = state_stack.top();
		state_stack.pop();
		InsertRow(row_copy);
	}
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
	return Run(dw_call_frame_prog, &os, 0, 0);
}

template <class MEMORY_ADDR>
bool DWARF_CallFrameVM<MEMORY_ADDR>::Execute(const DWARF_CallFrameProgram<MEMORY_ADDR>& dw_call_frame_prog, MEMORY_ADDR& location, RuleMatrix<MEMORY_ADDR> *rule_matrix)
{
	return Run(dw_call_frame_prog, 0, &location, rule_matrix);
}

template <class MEMORY_ADDR>
bool DWARF_CallFrameVM<MEMORY_ADDR>::Run(const DWARF_CallFrameProgram<MEMORY_ADDR>& dw_call_frame_prog, std::ostream *os, MEMORY_ADDR *_cur_location, RuleMatrix<MEMORY_ADDR> *rule_matrix)
{
	//RuleMatrix<MEMORY_ADDR> *initial_rule_matrix = new RuleMatrix<MEMORY_ADDR>(*rule_matrix);
	
	MEMORY_ADDR cur_location = _cur_location ? *_cur_location : 0;
	const DWARF_CIE<MEMORY_ADDR> *dw_cie = dw_call_frame_prog.GetCIE();
	if(!dw_cie) return false;
	uint64_t program_length = dw_call_frame_prog.length;
	endian_type endianness = dw_call_frame_prog.GetEndianness();
	uint8_t address_size = dw_call_frame_prog.GetAddressSize();
	const uint8_t *program = dw_call_frame_prog.program;
	
	if(program_length)
	{
		do
		{
			uint8_t opcode = *program;
			program++;
			program_length--;
			
			MEMORY_ADDR new_location = cur_location;
			
			switch(opcode & 0xc0) // high 2 bits
			{
				case DW_CFA_advance_loc:
					{
						uint8_t delta = opcode & 0x3f;
						
						uint64_t advance_loc = delta * (uint64_t) dw_cie->GetCodeAlignmentFactor();
						
						new_location += advance_loc;
						if(rule_matrix && (cur_location != new_location))
						{
							rule_matrix->CloneRow(cur_location, new_location);
						}
						if(os) *os << "DW_CFA_advance_loc " << advance_loc;
					}
					break;
				case DW_CFA_offset:
					{
						uint8_t reg_num = opcode & 0x3f;
						DWARF_LEB128 factored_offset;
						int64_t sz;
						
						if((sz = factored_offset.Load(program, program_length)) < 0) return false;
						program += sz;
						program_length -= sz;

						int64_t n = (uint64_t) factored_offset * (int64_t) dw_cie->GetDataAlignmentFactor();

						if(rule_matrix)
						{
							RuleMatrixRow<MEMORY_ADDR> *cur_row = (*rule_matrix)[cur_location];
							RegisterRuleOffset<MEMORY_ADDR> *new_rule = new RegisterRuleOffset<MEMORY_ADDR>(n);
							cur_row->SetRegisterRule(reg_num, new_rule);
						}

						if(os) *os << "DW_CFA_offset " << (uint32_t) reg_num << ", cfa" << (n >= 0 ? "+" : "") << n << " /* r" << (uint32_t) reg_num << " at cfa" << (n >= 0 ? "+" : "") << n << " */";
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
							if(os) *os << "DW_CFA_nop /* padding */";
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
								new_location = addr;
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
								uint64_t advance_loc = delta * (uint64_t) dw_cie->GetCodeAlignmentFactor();
								
								new_location += advance_loc;
								if(rule_matrix && (cur_location != new_location))
								{
									rule_matrix->CloneRow(cur_location, new_location);
								}
								
								if(os) *os << "DW_CFA_advance_loc1 " << advance_loc;
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
								uint64_t advance_loc = delta * (uint64_t) dw_cie->GetCodeAlignmentFactor();
								
								new_location += advance_loc;
								if(rule_matrix && (cur_location != new_location))
								{
									rule_matrix->CloneRow(cur_location, new_location);
								}
								
								if(os) *os << "DW_CFA_advance_loc2 " << advance_loc;
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
								uint64_t advance_loc = delta * (uint64_t) dw_cie->GetCodeAlignmentFactor();
								
								new_location += advance_loc;
								if(rule_matrix && (cur_location != new_location))
								{
									rule_matrix->CloneRow(cur_location, new_location);
								}
								
								if(os) *os << "DW_CFA_advance_loc4 " << advance_loc;
							}
							break;
						case DW_CFA_offset_extended:
							{
								DWARF_LEB128 reg_num;
								DWARF_LEB128 factored_offset;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if((sz = factored_offset.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								
								int64_t n = (uint64_t) factored_offset * (int64_t) dw_cie->GetDataAlignmentFactor();

								if(rule_matrix)
								{
									RuleMatrixRow<MEMORY_ADDR> *cur_row = (*rule_matrix)[cur_location];
									RegisterRuleOffset<MEMORY_ADDR> *new_rule = new RegisterRuleOffset<MEMORY_ADDR>(n);
									cur_row->SetRegisterRule(reg_num, new_rule);
								}

								if(os) *os << "DW_CFA_offset_extended " << reg_num.to_string(false) << ", cfa" << (n >= 0 ? "+" : "") << n << " /* r" << reg_num.to_string(false) << "=cfa" << (n >= 0 ? "+" : "") << n << " */";
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
								
								if(rule_matrix)
								{
									RuleMatrixRow<MEMORY_ADDR> *cur_row = (*rule_matrix)[cur_location];
									RegisterRuleUndefined<MEMORY_ADDR> *new_rule = new RegisterRuleUndefined<MEMORY_ADDR>();
									cur_row->SetRegisterRule(reg_num, new_rule);
								}
								
								if(os) *os << "DW_CFA_undefined " << reg_num.to_string(false) << " /* r" << reg_num.to_string(false) << " not preserved */";
							}
							break;
						case DW_CFA_same_value:
							{
								DWARF_LEB128 reg_num;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								
								if(rule_matrix)
								{
									RuleMatrixRow<MEMORY_ADDR> *cur_row = (*rule_matrix)[cur_location];
									RegisterRuleSameValue<MEMORY_ADDR> *new_rule = new RegisterRuleSameValue<MEMORY_ADDR>();
									cur_row->SetRegisterRule(reg_num, new_rule);
								}
								
								if(os) *os << "DW_CFA_same_value " << reg_num.to_string(false) << " /* r" << reg_num.to_string(false) << " preserved */";
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

								if(rule_matrix)
								{
									RuleMatrixRow<MEMORY_ADDR> *cur_row = (*rule_matrix)[cur_location];
									RegisterRuleRegister<MEMORY_ADDR> *new_rule = new RegisterRuleRegister<MEMORY_ADDR>((unsigned int) reg_num2);
									cur_row->SetRegisterRule(reg_num1, new_rule);
								}

								if(os) *os << "DW_CFA_register " << reg_num1.to_string(false) << ", " << reg_num2.to_string(false) << " /* r" << reg_num1.to_string(false) << " = " << reg_num2.to_string(false) << " */";
							}
							break;
						case DW_CFA_remember_state:
							if(rule_matrix)
							{
								rule_matrix->RememberState(cur_location);
							}
							if(os) *os << "DW_CFA_remember_state";
							break;
						case DW_CFA_restore_state:
							if(rule_matrix)
							{
								rule_matrix->RestoreState(cur_location);
							}
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
								
								if(rule_matrix)
								{
									RuleMatrixRow<MEMORY_ADDR> *cur_row = (*rule_matrix)[cur_location];
									CFARule<MEMORY_ADDR> *new_cfa_rule = new CFARuleRegisterOffset<MEMORY_ADDR>(reg_num, offset);
									cur_row->SetCFARule(new_cfa_rule);
								}
								
								if(os) *os << "DW_CFA_def_cfa " << reg_num.to_string(false) << ", " << offset.to_string(false) << " /* cfa=[r" << reg_num.to_string(false) << "]+" << offset.to_string(false) << " */";
							}
							break;
						case DW_CFA_def_cfa_register:
							{
								DWARF_LEB128 reg_num;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								
								if(rule_matrix)
								{
									RuleMatrixRow<MEMORY_ADDR> *cur_row = (*rule_matrix)[cur_location];
									CFARule<MEMORY_ADDR> *cur_cfa_rule = cur_row->GetCFARule();
									if(cur_cfa_rule->GetType() != DW_CFA_RULE_REGISTER_OFFSET) return false;
									
									reinterpret_cast<CFARuleRegisterOffset<MEMORY_ADDR> *>(cur_cfa_rule)->SetRegisterNumber(reg_num);
								}
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
								if(rule_matrix)
								{
									RuleMatrixRow<MEMORY_ADDR> *cur_row = (*rule_matrix)[cur_location];
									CFARule<MEMORY_ADDR> *cur_cfa_rule = cur_row->GetCFARule();
									if(cur_cfa_rule->GetType() != DW_CFA_RULE_REGISTER_OFFSET) return false;
									
									reinterpret_cast<CFARuleRegisterOffset<MEMORY_ADDR> *>(cur_cfa_rule)->SetOffset((uint64_t) offset);
								}
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

								DWARF_Expression<MEMORY_ADDR> *dw_expr = new DWARF_Expression<MEMORY_ADDR>(&dw_call_frame_prog, block_length, program);
								program += block_length;
								program_length -= block_length;
								if(rule_matrix)
								{
									RuleMatrixRow<MEMORY_ADDR> *cur_row = (*rule_matrix)[cur_location];
									CFARule<MEMORY_ADDR> *new_cfa_rule = new CFARuleExpression<MEMORY_ADDR>(dw_expr);
									cur_row->SetCFARule(new_cfa_rule);
								}
								if(os) *os << "DW_CFA_def_cfa_expression {" << dw_expr->to_string() << "}";
								if(!rule_matrix) delete dw_expr;
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

								DWARF_Expression<MEMORY_ADDR> *dw_expr = new DWARF_Expression<MEMORY_ADDR>(&dw_call_frame_prog, block_length, program);
								program += block_length;
								program_length -= block_length;
								
								if(rule_matrix)
								{
									RuleMatrixRow<MEMORY_ADDR> *cur_row = (*rule_matrix)[cur_location];
									RegisterRuleExpression<MEMORY_ADDR> *new_rule = new RegisterRuleExpression<MEMORY_ADDR>(dw_expr);
									cur_row->SetRegisterRule(reg_num, new_rule);
								}
								
								if(os) *os << "DW_CFA_expression " << reg_num.to_string(false) << " {" << dw_expr->to_string() << "}";
								if(!rule_matrix) delete dw_expr;
							}
							break;
						case DW_CFA_offset_extended_sf:
							{
								DWARF_LEB128 reg_num;
								DWARF_LEB128 signed_factored_offset;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if((sz = signed_factored_offset.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								
								int64_t n = (int64_t) signed_factored_offset * (int64_t) dw_cie->GetDataAlignmentFactor();
								
								if(rule_matrix)
								{
									RuleMatrixRow<MEMORY_ADDR> *cur_row = (*rule_matrix)[cur_location];
									RegisterRuleOffset<MEMORY_ADDR> *new_rule = new RegisterRuleOffset<MEMORY_ADDR>(n);
									cur_row->SetRegisterRule(reg_num, new_rule);
								}
								
								if(os) *os << "DW_CFA_offset_extended_sf " << reg_num.to_string(false) << ", cfa" << (n >= 0 ? "+" : "") << n << " /* r" << reg_num.to_string(false) << " at cfa" << (n >= 0 ? "+" : "") << n << " */";
							}
							break;
						case DW_CFA_def_cfa_sf:
							{
								DWARF_LEB128 reg_num;
								DWARF_LEB128 signed_factored_offset;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if((sz = signed_factored_offset.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								int64_t n = (int64_t) signed_factored_offset * (int64_t) dw_cie->GetDataAlignmentFactor();
								if(os) *os << "DW_CFA_def_cfa_sf " << reg_num.to_string(false) << ", " << signed_factored_offset.to_string(true) << " /* cfa=[" << reg_num.to_string(false) << "]=" << (n > 0 ? "+":"") << n << " */";
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
								DWARF_LEB128 factored_offset;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if((sz = factored_offset.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								
								int64_t n = (uint64_t) factored_offset * (int64_t) dw_cie->GetDataAlignmentFactor();
								
								if(rule_matrix)
								{
									RuleMatrixRow<MEMORY_ADDR> *cur_row = (*rule_matrix)[cur_location];
									RegisterRuleValOffset<MEMORY_ADDR> *new_rule = new RegisterRuleValOffset<MEMORY_ADDR>(n);
									cur_row->SetRegisterRule(reg_num, new_rule);
								}
								
								if(os) *os << "DW_CFA_val_offset " << reg_num.to_string(false) << ", cfa" << (n >= 0 ? "+" : "") << n << " /* r" << reg_num.to_string(false) << "=cfa" << (n >= 0 ? "+" : "") << n << " */";
							}
							break;
						case DW_CFA_val_offset_sf:
							{
								DWARF_LEB128 reg_num;
								DWARF_LEB128 factored_offset;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if((sz = factored_offset.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								
								int64_t n = (int64_t) factored_offset * (int64_t) dw_cie->GetDataAlignmentFactor();

								if(rule_matrix)
								{
									RuleMatrixRow<MEMORY_ADDR> *cur_row = (*rule_matrix)[cur_location];
									RegisterRuleValOffset<MEMORY_ADDR> *new_rule = new RegisterRuleValOffset<MEMORY_ADDR>(n);
									cur_row->SetRegisterRule(reg_num, new_rule);
								}
								
								if(os) *os << "DW_CFA_val_offset_sf " << reg_num.to_string(false) << ", cfa+" << (n >= 0 ? "+" : "") << n << " /* r" << reg_num.to_string(false) << "=cfa" << (n >= 0 ? "+" : "") << n << " */";
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

								DWARF_Expression<MEMORY_ADDR> *dw_expr = new DWARF_Expression<MEMORY_ADDR>(&dw_call_frame_prog, block_length, program);
								program += block_length;
								program_length -= block_length;
								
								if(rule_matrix)
								{
									RuleMatrixRow<MEMORY_ADDR> *cur_row = (*rule_matrix)[cur_location];
									RegisterRuleExpression<MEMORY_ADDR> *new_rule = new RegisterRuleExpression<MEMORY_ADDR>(dw_expr);
									cur_row->SetRegisterRule(reg_num, new_rule);
								}
								
								if(os) *os << "DW_CFA_val_expression " << reg_num.to_string(false) << " {" << dw_expr->to_string() << "}";
								if(!rule_matrix) delete dw_expr;
							}
							break;
						default:
							return false;
					}
					break;
				default:
					return false;
			}
			
			if(os && program_length) *os << std::endl;
		}
		while(program_length);
	}

	return true;
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim
