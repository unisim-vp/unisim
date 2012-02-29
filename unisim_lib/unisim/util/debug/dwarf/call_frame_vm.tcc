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

#include <stdexcept>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
DWARF_CFARule<MEMORY_ADDR>::DWARF_CFARule(unsigned int _type)
	: type(_type)
{
}

template <class MEMORY_ADDR>
DWARF_CFARule<MEMORY_ADDR>::~DWARF_CFARule()
{
}

template <class MEMORY_ADDR>
unsigned int DWARF_CFARule<MEMORY_ADDR>::GetType() const
{
	return type;
}

template <class MEMORY_ADDR>
DWARF_CFARule<MEMORY_ADDR> *DWARF_CFARule<MEMORY_ADDR>::Clone()
{
	switch(type)
	{
		case DW_CFA_RULE_REGISTER_OFFSET:
			return new DWARF_CFARuleRegisterOffset<MEMORY_ADDR>(*reinterpret_cast<const DWARF_CFARuleRegisterOffset<MEMORY_ADDR> *>(this));
		case DW_CFA_RULE_EXPRESSION:
			return new DWARF_CFARuleExpression<MEMORY_ADDR>(*reinterpret_cast<const DWARF_CFARuleExpression<MEMORY_ADDR> *>(this));
	}
	return 0;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_CFARule<MEMORY_ADDR>& cfa_rule)
{
	return cfa_rule.Print(os);
}

template <class MEMORY_ADDR>
DWARF_CFARuleRegisterOffset<MEMORY_ADDR>::DWARF_CFARuleRegisterOffset(unsigned int _dw_reg_num, int64_t _dw_offset)
	: DWARF_CFARule<MEMORY_ADDR>(DW_CFA_RULE_REGISTER_OFFSET)
	, dw_reg_num(_dw_reg_num)
	, dw_offset(_dw_offset)
{
}

template <class MEMORY_ADDR>
DWARF_CFARuleRegisterOffset<MEMORY_ADDR>::DWARF_CFARuleRegisterOffset(const DWARF_CFARuleRegisterOffset<MEMORY_ADDR>& cfa_rule_offset)
	: DWARF_CFARule<MEMORY_ADDR>(DW_CFA_RULE_REGISTER_OFFSET)
	, dw_reg_num(cfa_rule_offset.dw_reg_num)
	, dw_offset(cfa_rule_offset.dw_offset)
{
}

template <class MEMORY_ADDR>
DWARF_CFARuleRegisterOffset<MEMORY_ADDR>::~DWARF_CFARuleRegisterOffset()
{
}

template <class MEMORY_ADDR>
void DWARF_CFARuleRegisterOffset<MEMORY_ADDR>::SetRegisterNumber(unsigned int _dw_reg_num)
{
	dw_reg_num = _dw_reg_num;
}

template <class MEMORY_ADDR>
void DWARF_CFARuleRegisterOffset<MEMORY_ADDR>::SetOffset(int64_t _dw_offset)
{
	dw_offset = _dw_offset;
}

template <class MEMORY_ADDR>
unsigned int DWARF_CFARuleRegisterOffset<MEMORY_ADDR>::GetRegisterNumber() const
{
	return dw_reg_num;
}

template <class MEMORY_ADDR>
int64_t DWARF_CFARuleRegisterOffset<MEMORY_ADDR>::GetOffset() const
{
	return dw_offset;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_CFARuleRegisterOffset<MEMORY_ADDR>::Print(std::ostream& os) const
{
	return os << "cfa=[r" << dw_reg_num << "]+" << dw_offset;
}

template <class MEMORY_ADDR>
DWARF_CFARuleExpression<MEMORY_ADDR>::DWARF_CFARuleExpression(const DWARF_Expression<MEMORY_ADDR> *_dw_expr)
	: DWARF_CFARule<MEMORY_ADDR>(DW_CFA_RULE_EXPRESSION)
	, dw_expr(_dw_expr)
{
}

template <class MEMORY_ADDR>
DWARF_CFARuleExpression<MEMORY_ADDR>::DWARF_CFARuleExpression(const DWARF_CFARuleExpression<MEMORY_ADDR>& cfa_rule_expression)
	: DWARF_CFARule<MEMORY_ADDR>(DW_CFA_RULE_EXPRESSION)
	, dw_expr(new DWARF_Expression<MEMORY_ADDR>(*cfa_rule_expression.dw_expr))
{
}

template <class MEMORY_ADDR>
DWARF_CFARuleExpression<MEMORY_ADDR>::~DWARF_CFARuleExpression()
{
	if(dw_expr) delete dw_expr;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_CFARuleExpression<MEMORY_ADDR>::Print(std::ostream& os) const
{
	return os << "cfa={" << *dw_expr << "}";
}

template <class MEMORY_ADDR>
DWARF_RegisterRule<MEMORY_ADDR>::DWARF_RegisterRule(unsigned int _dw_rule_type)
	: dw_rule_type(_dw_rule_type)
{
}

template <class MEMORY_ADDR>
DWARF_RegisterRule<MEMORY_ADDR>::~DWARF_RegisterRule()
{
}

template <class MEMORY_ADDR>
unsigned int DWARF_RegisterRule<MEMORY_ADDR>::GetType() const
{
	return dw_rule_type;
}

template <class MEMORY_ADDR>
DWARF_RegisterRule<MEMORY_ADDR> *DWARF_RegisterRule<MEMORY_ADDR>::Clone()
{
	switch(dw_rule_type)
	{
		case DW_REG_RULE_UNDEFINED:
			return new DWARF_RegisterRuleUndefined<MEMORY_ADDR>(*reinterpret_cast<const DWARF_RegisterRuleUndefined<MEMORY_ADDR> *>(this));
		case DW_REG_RULE_SAME_VALUE:
			return new DWARF_RegisterRuleSameValue<MEMORY_ADDR>(*reinterpret_cast<const DWARF_RegisterRuleSameValue<MEMORY_ADDR> *>(this));
		case DW_REG_RULE_OFFSET:
			return new DWARF_RegisterRuleOffset<MEMORY_ADDR>(*reinterpret_cast<const DWARF_RegisterRuleOffset<MEMORY_ADDR> *>(this));
		case DW_REG_RULE_VAL_OFFSET:
			return new DWARF_RegisterRuleValOffset<MEMORY_ADDR>(*reinterpret_cast<const DWARF_RegisterRuleValOffset<MEMORY_ADDR> *>(this));
		case DW_REG_RULE_REGISTER:
			return new DWARF_RegisterRuleRegister<MEMORY_ADDR>(*reinterpret_cast<const DWARF_RegisterRuleRegister<MEMORY_ADDR> *>(this));
		case DW_REG_RULE_EXPRESSION:
			return new DWARF_RegisterRuleExpression<MEMORY_ADDR>(*reinterpret_cast<const DWARF_RegisterRuleExpression<MEMORY_ADDR> *>(this));
		case DW_REG_RULE_VAL_EXPRESSION:
			return new DWARF_RegisterRuleValExpression<MEMORY_ADDR>(*reinterpret_cast<const DWARF_RegisterRuleValExpression<MEMORY_ADDR> *>(this));
	}
	return 0;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_RegisterRule<MEMORY_ADDR>& register_rule)
{
	return register_rule.Print(os);
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleUndefined<MEMORY_ADDR>::DWARF_RegisterRuleUndefined(const DWARF_RegisterRuleUndefined<MEMORY_ADDR>& rule)
	: DWARF_RegisterRule<MEMORY_ADDR>(DW_REG_RULE_REGISTER)
{
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleUndefined<MEMORY_ADDR>::DWARF_RegisterRuleUndefined()
	: DWARF_RegisterRule<MEMORY_ADDR>(DW_REG_RULE_REGISTER)
{
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleUndefined<MEMORY_ADDR>::~DWARF_RegisterRuleUndefined()
{
}

template <class MEMORY_ADDR>
std::ostream& DWARF_RegisterRuleUndefined<MEMORY_ADDR>::Print(std::ostream& os) const
{
	return os << "undefined";
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleSameValue<MEMORY_ADDR>::DWARF_RegisterRuleSameValue(const DWARF_RegisterRuleSameValue<MEMORY_ADDR>& rule)
	: DWARF_RegisterRule<MEMORY_ADDR>(DW_REG_RULE_SAME_VALUE)
{
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleSameValue<MEMORY_ADDR>::DWARF_RegisterRuleSameValue()
	: DWARF_RegisterRule<MEMORY_ADDR>(DW_REG_RULE_SAME_VALUE)
{
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleSameValue<MEMORY_ADDR>::~DWARF_RegisterRuleSameValue()
{
}

template <class MEMORY_ADDR>
std::ostream& DWARF_RegisterRuleSameValue<MEMORY_ADDR>::Print(std::ostream& os) const
{
	return os << "preserved";
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleOffset<MEMORY_ADDR>::DWARF_RegisterRuleOffset(const DWARF_RegisterRuleOffset<MEMORY_ADDR>& rule)
	: DWARF_RegisterRule<MEMORY_ADDR>(DW_REG_RULE_OFFSET)
	, dw_offset(rule.dw_offset)
{
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleOffset<MEMORY_ADDR>::DWARF_RegisterRuleOffset(int64_t _dw_offset)
	: DWARF_RegisterRule<MEMORY_ADDR>(DW_REG_RULE_OFFSET)
	, dw_offset(_dw_offset)
{
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleOffset<MEMORY_ADDR>::~DWARF_RegisterRuleOffset()
{
}

template <class MEMORY_ADDR>
int64_t DWARF_RegisterRuleOffset<MEMORY_ADDR>::GetOffset() const
{
	return dw_offset;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_RegisterRuleOffset<MEMORY_ADDR>::Print(std::ostream& os) const
{
	return os << "[cfa" << ((dw_offset < 0) ? "-" : "+") << ((dw_offset < 0) ? -dw_offset : dw_offset) << "]";
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleValOffset<MEMORY_ADDR>::DWARF_RegisterRuleValOffset(const DWARF_RegisterRuleValOffset<MEMORY_ADDR>& rule)
	: DWARF_RegisterRule<MEMORY_ADDR>(DW_REG_RULE_VAL_OFFSET)
	, dw_offset(rule.dw_offset)
{
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleValOffset<MEMORY_ADDR>::DWARF_RegisterRuleValOffset(int64_t _dw_offset)
	: DWARF_RegisterRule<MEMORY_ADDR>(DW_REG_RULE_VAL_OFFSET)
	, dw_offset(_dw_offset)
{
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleValOffset<MEMORY_ADDR>::~DWARF_RegisterRuleValOffset()
{
}

template <class MEMORY_ADDR>
int64_t DWARF_RegisterRuleValOffset<MEMORY_ADDR>::GetOffset() const
{
	return dw_offset;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_RegisterRuleValOffset<MEMORY_ADDR>::Print(std::ostream& os) const
{
	return os << "cfa+" << dw_offset;
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleRegister<MEMORY_ADDR>::DWARF_RegisterRuleRegister(const DWARF_RegisterRuleRegister<MEMORY_ADDR>& rule)
	: DWARF_RegisterRule<MEMORY_ADDR>(DW_REG_RULE_REGISTER)
	, dw_reg_num(rule.dw_reg_num)
{
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleRegister<MEMORY_ADDR>::DWARF_RegisterRuleRegister(unsigned int _dw_reg_num)
	: DWARF_RegisterRule<MEMORY_ADDR>(DW_REG_RULE_REGISTER)
	, dw_reg_num(_dw_reg_num)
{
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleRegister<MEMORY_ADDR>::~DWARF_RegisterRuleRegister()
{
}

template <class MEMORY_ADDR>
unsigned int DWARF_RegisterRuleRegister<MEMORY_ADDR>::GetRegisterNumber() const
{
	return dw_reg_num;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_RegisterRuleRegister<MEMORY_ADDR>::Print(std::ostream& os) const
{
	return os << "r" << dw_reg_num;
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleExpression<MEMORY_ADDR>::DWARF_RegisterRuleExpression(const DWARF_RegisterRuleExpression<MEMORY_ADDR>& rule)
	: DWARF_RegisterRule<MEMORY_ADDR>(DW_REG_RULE_EXPRESSION)
	, dw_expr(new DWARF_Expression<MEMORY_ADDR>(*rule.dw_expr))
{
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleExpression<MEMORY_ADDR>::DWARF_RegisterRuleExpression(const DWARF_Expression<MEMORY_ADDR> *_dw_expr)
	: DWARF_RegisterRule<MEMORY_ADDR>(DW_REG_RULE_EXPRESSION)
	, dw_expr(new DWARF_Expression<MEMORY_ADDR>(*_dw_expr))
{
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleExpression<MEMORY_ADDR>::~DWARF_RegisterRuleExpression()
{
}

template <class MEMORY_ADDR>
const DWARF_Expression<MEMORY_ADDR> *DWARF_RegisterRuleExpression<MEMORY_ADDR>::GetExpression() const
{
	return dw_expr;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_RegisterRuleExpression<MEMORY_ADDR>::Print(std::ostream& os) const
{
	return os << "[{" << *dw_expr << "}]";
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleValExpression<MEMORY_ADDR>::DWARF_RegisterRuleValExpression(const DWARF_RegisterRuleValExpression<MEMORY_ADDR>& rule)
	: DWARF_RegisterRule<MEMORY_ADDR>(DW_REG_RULE_VAL_EXPRESSION)
	, dw_expr(new DWARF_Expression<MEMORY_ADDR>(*rule.dw_expr))
{
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleValExpression<MEMORY_ADDR>::DWARF_RegisterRuleValExpression(const DWARF_Expression<MEMORY_ADDR> *_dw_expr)
	: DWARF_RegisterRule<MEMORY_ADDR>(DW_REG_RULE_VAL_EXPRESSION)
	, dw_expr(new DWARF_Expression<MEMORY_ADDR>(*_dw_expr))
{
}

template <class MEMORY_ADDR>
DWARF_RegisterRuleValExpression<MEMORY_ADDR>::~DWARF_RegisterRuleValExpression()
{
}

template <class MEMORY_ADDR>
std::ostream& DWARF_RegisterRuleValExpression<MEMORY_ADDR>::Print(std::ostream& os) const
{
	return os << "{" << *dw_expr << "}";
}

template <class MEMORY_ADDR>
DWARF_RuleMatrixRow<MEMORY_ADDR>::DWARF_RuleMatrixRow()
	: location(0)
	, cfa_rule(0)
	, reg_rules()
{
}

template <class MEMORY_ADDR>
DWARF_RuleMatrixRow<MEMORY_ADDR>::DWARF_RuleMatrixRow(MEMORY_ADDR _location)
	: location(_location)
	, cfa_rule(0)
	, reg_rules()
{
}

template <class MEMORY_ADDR>
DWARF_RuleMatrixRow<MEMORY_ADDR>::DWARF_RuleMatrixRow(const DWARF_RuleMatrixRow<MEMORY_ADDR>& rule_matrix_row)
	: location(rule_matrix_row.location)
	, cfa_rule(0)
	, reg_rules()
{
	DWARF_CFARule<MEMORY_ADDR> *orig_cfa_rule = rule_matrix_row.GetCFARule();
	
	if(orig_cfa_rule)
	{
		cfa_rule = orig_cfa_rule->Clone();
	}
	
	typename std::map<unsigned int, DWARF_RegisterRule<MEMORY_ADDR> *>::const_iterator it;
	
	for(it = rule_matrix_row.reg_rules.begin(); it != rule_matrix_row.reg_rules.end(); it++)
	{
		unsigned int reg_num = (*it).first;
		DWARF_RegisterRule<MEMORY_ADDR> *orig_rule = (*it).second;
		DWARF_RegisterRule<MEMORY_ADDR> *rule_copy = orig_rule ? orig_rule->Clone() : 0;
		reg_rules.insert(std::pair<unsigned int, DWARF_RegisterRule<MEMORY_ADDR> *>(reg_num, rule_copy));
	}
}

template <class MEMORY_ADDR>
DWARF_RuleMatrixRow<MEMORY_ADDR>::DWARF_RuleMatrixRow(MEMORY_ADDR _location, const DWARF_RuleMatrixRow<MEMORY_ADDR>& rule_matrix_row)
	: location(_location)
	, cfa_rule(0)
	, reg_rules()
{
	DWARF_CFARule<MEMORY_ADDR> *orig_cfa_rule = rule_matrix_row.GetCFARule();
	
	if(orig_cfa_rule)
	{
		cfa_rule = orig_cfa_rule->Clone();
	}
		
	typename std::map<unsigned int, DWARF_RegisterRule<MEMORY_ADDR> *>::const_iterator it;
	
	for(it = rule_matrix_row.reg_rules.begin(); it != rule_matrix_row.reg_rules.end(); it++)
	{
		unsigned int reg_num = (*it).first;
		DWARF_RegisterRule<MEMORY_ADDR> *orig_rule = (*it).second;
		DWARF_RegisterRule<MEMORY_ADDR> *rule_copy = orig_rule ? orig_rule->Clone() : 0;
		reg_rules.insert(std::pair<unsigned int, DWARF_RegisterRule<MEMORY_ADDR> *>(reg_num, rule_copy));
	}
}

template <class MEMORY_ADDR>
DWARF_RuleMatrixRow<MEMORY_ADDR>::~DWARF_RuleMatrixRow()
{
	if(cfa_rule) delete cfa_rule;

	typename std::map<unsigned int, DWARF_RegisterRule<MEMORY_ADDR> *>::const_iterator it;
	
	for(it = reg_rules.begin(); it != reg_rules.end(); it++)
	{
		DWARF_RegisterRule<MEMORY_ADDR> *reg_rule = (*it).second;
		if(reg_rule) delete reg_rule;
	}
}

template <class MEMORY_ADDR>
void DWARF_RuleMatrixRow<MEMORY_ADDR>::SetCFARule(DWARF_CFARule<MEMORY_ADDR> *_cfa_rule)
{
	if(cfa_rule) delete cfa_rule;
	cfa_rule = _cfa_rule;
}

template <class MEMORY_ADDR>
void DWARF_RuleMatrixRow<MEMORY_ADDR>::SetRegisterRule(unsigned int reg_num, DWARF_RegisterRule<MEMORY_ADDR> *reg_rule)
{
	typename std::map<unsigned int, DWARF_RegisterRule<MEMORY_ADDR> *>::iterator it = reg_rules.find(reg_num);
	
	if(it != reg_rules.end())
	{
		DWARF_RegisterRule<MEMORY_ADDR> *old_reg_rule = (*it).second;
		if(old_reg_rule) delete old_reg_rule;
		(*it).second = reg_rule;
	}
	else
	{
		reg_rules.insert(std::pair<unsigned int, DWARF_RegisterRule<MEMORY_ADDR> *>(reg_num, reg_rule));
	}
}

template <class MEMORY_ADDR>
MEMORY_ADDR DWARF_RuleMatrixRow<MEMORY_ADDR>::GetLocation() const
{
	return location;
}

template <class MEMORY_ADDR>
DWARF_CFARule<MEMORY_ADDR> *DWARF_RuleMatrixRow<MEMORY_ADDR>::GetCFARule() const
{
	return cfa_rule;
}

template <class MEMORY_ADDR>
DWARF_RegisterRule<MEMORY_ADDR> *DWARF_RuleMatrixRow<MEMORY_ADDR>::GetRegisterRule(unsigned int reg_num) const
{
	typename std::map<unsigned int, DWARF_RegisterRule<MEMORY_ADDR> *>::const_iterator it = reg_rules.find(reg_num);
	return (it != reg_rules.end()) ? (*it).second : 0;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_RuleMatrixRow<MEMORY_ADDR>& rule_matrix_row)
{
	os << std::hex << "pc=0x" << rule_matrix_row.location << std::dec << "  <";
	if(rule_matrix_row.cfa_rule)
	{
		os << *rule_matrix_row.cfa_rule;
	}
	else
	{
		os << "cfa=?";
	}
	os << ">";
	typename std::map<unsigned int, DWARF_RegisterRule<MEMORY_ADDR> *>::const_iterator it;
		
	for(it = rule_matrix_row.reg_rules.begin(); it != rule_matrix_row.reg_rules.end(); it++)
	{
		os << " <";
		DWARF_RegisterRule<MEMORY_ADDR> *reg_rule = (*it).second;
		unsigned int reg_num = (*it).first;
		if(reg_rule)
		{
			os << "r" << reg_num << "=" << *reg_rule;
		}
		else
		{
			os << "r" << reg_num << "=u";
		}
		os << ">";
	}
	return os;
}

template <class MEMORY_ADDR>
DWARF_RuleMatrix<MEMORY_ADDR>::DWARF_RuleMatrix()
	: initial_row(new DWARF_RuleMatrixRow<MEMORY_ADDR>())
{
}

template <class MEMORY_ADDR>
DWARF_RuleMatrix<MEMORY_ADDR>::DWARF_RuleMatrix(const DWARF_RuleMatrix<MEMORY_ADDR>& rule_matrix)
	: initial_row(0)
{
	typename std::map<MEMORY_ADDR, DWARF_RuleMatrixRow<MEMORY_ADDR> *>::const_iterator iter;
	
	for(iter = rule_matrix.rule_matrix_rows.begin(); iter != rule_matrix.rule_matrix_rows.end(); iter++)
	{
		DWARF_RuleMatrixRow<MEMORY_ADDR> *row = (*iter).second;
		DWARF_RuleMatrixRow<MEMORY_ADDR> *new_row = new DWARF_RuleMatrixRow<MEMORY_ADDR>(*row);
		rule_matrix_rows.insert(std::pair<MEMORY_ADDR, DWARF_RuleMatrixRow<MEMORY_ADDR> *>(new_row->GetLocation(), new_row));
	}
	
	initial_row = new DWARF_RuleMatrixRow<MEMORY_ADDR>(*rule_matrix.GetInitialRow());
}

template <class MEMORY_ADDR>
DWARF_RuleMatrix<MEMORY_ADDR>::~DWARF_RuleMatrix()
{
	typename std::map<MEMORY_ADDR, DWARF_RuleMatrixRow<MEMORY_ADDR> *>::iterator iter;
	
	for(iter = rule_matrix_rows.begin(); iter != rule_matrix_rows.end(); iter++)
	{
		delete (*iter).second;
	}
	
	if(initial_row) delete initial_row;
}

template <class MEMORY_ADDR>
DWARF_RuleMatrixRow<MEMORY_ADDR> *DWARF_RuleMatrix<MEMORY_ADDR>::operator[](MEMORY_ADDR loc) const
{
	typename std::map<MEMORY_ADDR, DWARF_RuleMatrixRow<MEMORY_ADDR> *>::const_iterator iter = rule_matrix_rows.find(loc);
	
	return (iter != rule_matrix_rows.end()) ? (*iter).second : 0;
}

template <class MEMORY_ADDR>
DWARF_RuleMatrixRow<MEMORY_ADDR> *DWARF_RuleMatrix<MEMORY_ADDR>::GetRow(MEMORY_ADDR loc) const
{
	typename std::map<MEMORY_ADDR, DWARF_RuleMatrixRow<MEMORY_ADDR> *>::const_iterator iter = rule_matrix_rows.find(loc);
	
	return (iter != rule_matrix_rows.end()) ? (*iter).second : 0;
}

template <class MEMORY_ADDR>
DWARF_RuleMatrixRow<MEMORY_ADDR> *DWARF_RuleMatrix<MEMORY_ADDR>::GetInitialRow() const
{
	return initial_row;
}

template <class MEMORY_ADDR>
bool DWARF_RuleMatrix<MEMORY_ADDR>::HasRow(MEMORY_ADDR loc) const
{
	typename std::map<MEMORY_ADDR, DWARF_RuleMatrixRow<MEMORY_ADDR> *>::const_iterator iter = rule_matrix_rows.find(loc);
	return (iter != rule_matrix_rows.end());
}

template <class MEMORY_ADDR>
void DWARF_RuleMatrix<MEMORY_ADDR>::InsertRow(DWARF_RuleMatrixRow<MEMORY_ADDR> *rule_matrix_row)
{
	typename std::map<MEMORY_ADDR, DWARF_RuleMatrixRow<MEMORY_ADDR> *>::iterator iter = rule_matrix_rows.find(rule_matrix_row->GetLocation());
	
	if(iter != rule_matrix_rows.end())
	{
		rule_matrix_rows.erase(iter);
	}
	
	if(rule_matrix_row)
	{
		rule_matrix_rows.insert(std::pair<MEMORY_ADDR, DWARF_RuleMatrixRow<MEMORY_ADDR> *>(rule_matrix_row->GetLocation(), rule_matrix_row));
	}
}

template <class MEMORY_ADDR>
void DWARF_RuleMatrix<MEMORY_ADDR>::CloneRow(MEMORY_ADDR cur_loc, MEMORY_ADDR new_loc)
{
	typename std::map<MEMORY_ADDR, DWARF_RuleMatrixRow<MEMORY_ADDR> *>::iterator iter = rule_matrix_rows.find(cur_loc);
	
	if(iter != rule_matrix_rows.end())
	{
		DWARF_RuleMatrixRow<MEMORY_ADDR> *new_row = new DWARF_RuleMatrixRow<MEMORY_ADDR>(new_loc, *(*iter).second);
		rule_matrix_rows.insert(std::pair<MEMORY_ADDR, DWARF_RuleMatrixRow<MEMORY_ADDR> *>(new_loc, new_row));
	}
	else
	{
		DWARF_RuleMatrixRow<MEMORY_ADDR> *new_row = new DWARF_RuleMatrixRow<MEMORY_ADDR>(new_loc, *initial_row);
		rule_matrix_rows.insert(std::pair<MEMORY_ADDR, DWARF_RuleMatrixRow<MEMORY_ADDR> *>(new_loc, new_row));
	}
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_RuleMatrix<MEMORY_ADDR>& rule_matrix)
{
	typename std::map<MEMORY_ADDR, DWARF_RuleMatrixRow<MEMORY_ADDR> *>::const_iterator iter;
	
	for(iter = rule_matrix.rule_matrix_rows.begin(); iter != rule_matrix.rule_matrix_rows.end(); iter++)
	{
		DWARF_RuleMatrixRow<MEMORY_ADDR> *row = (*iter).second;
		os << *row << std::endl;
	}
	return os;
}

template <class MEMORY_ADDR>
DWARF_CallFrameVM<MEMORY_ADDR>::DWARF_CallFrameVM()
{
}

template <class MEMORY_ADDR>
DWARF_CallFrameVM<MEMORY_ADDR>::~DWARF_CallFrameVM()
{
	ResetState();
}

template <class MEMORY_ADDR>
bool DWARF_CallFrameVM<MEMORY_ADDR>::Disasm(std::ostream& os, const DWARF_CallFrameProgram<MEMORY_ADDR>& dw_call_frame_prog)
{
	return Run(dw_call_frame_prog, &os, 0, 0);
}

template <class MEMORY_ADDR>
bool DWARF_CallFrameVM<MEMORY_ADDR>::Execute(const DWARF_CallFrameProgram<MEMORY_ADDR>& dw_call_frame_prog, MEMORY_ADDR& location, DWARF_RuleMatrix<MEMORY_ADDR> *rule_matrix)
{
	return Run(dw_call_frame_prog, 0, &location, rule_matrix);
}

template <class MEMORY_ADDR>
bool DWARF_CallFrameVM<MEMORY_ADDR>::RememberState(DWARF_RuleMatrix<MEMORY_ADDR> *rule_matrix, MEMORY_ADDR loc)
{
	DWARF_RuleMatrixRow<MEMORY_ADDR> *row = rule_matrix->GetRow(loc);
	
	if(!row) return false;

	DWARF_RuleMatrixRow<MEMORY_ADDR> *row_copy = new DWARF_RuleMatrixRow<MEMORY_ADDR>(loc, *row);
	row_stack.push(row_copy);
	return true;
}

template <class MEMORY_ADDR>
bool DWARF_CallFrameVM<MEMORY_ADDR>::RestoreState(DWARF_RuleMatrix<MEMORY_ADDR> *rule_matrix, MEMORY_ADDR loc)
{
	if(row_stack.empty()) return false;

	DWARF_RuleMatrixRow<MEMORY_ADDR> *row_copy = row_stack.top();
	row_stack.pop();
	rule_matrix->InsertRow(row_copy);
	return true;
}

template <class MEMORY_ADDR>
void DWARF_CallFrameVM<MEMORY_ADDR>::ResetState()
{
	while(!row_stack.empty())
	{
		DWARF_RuleMatrixRow<MEMORY_ADDR> *row = row_stack.top();
		row_stack.pop();
		delete row;
	}
}

template <class MEMORY_ADDR>
bool DWARF_CallFrameVM<MEMORY_ADDR>::Run(const DWARF_CallFrameProgram<MEMORY_ADDR>& dw_call_frame_prog, std::ostream *os, MEMORY_ADDR *_cur_location, DWARF_RuleMatrix<MEMORY_ADDR> *rule_matrix)
{
	const DWARF_CIE<MEMORY_ADDR> *dw_cie = dw_call_frame_prog.GetCIE();
	const DWARF_FDE<MEMORY_ADDR> *dw_fde = dw_call_frame_prog.GetFDE();
	if(!dw_cie) return false;
	
	unsigned int dw_cfp_type = dw_call_frame_prog.GetType();
	MEMORY_ADDR cur_location = _cur_location ? *_cur_location : 0;
	uint64_t program_length = dw_call_frame_prog.length;
	endian_type endianness = dw_call_frame_prog.GetEndianness();
	uint8_t address_size = dw_call_frame_prog.GetAddressSize();
	const uint8_t *program = dw_call_frame_prog.program;
	bool status = true;
	
	if(dw_cfp_type == DW_CFP_INSTRUCTIONS)
	{
		if(rule_matrix)
		{
			if(!rule_matrix->HasRow(cur_location))
			{
				DWARF_RuleMatrixRow<MEMORY_ADDR> *cur_row = new DWARF_RuleMatrixRow<MEMORY_ADDR>(cur_location, *rule_matrix->GetInitialRow());
				rule_matrix->InsertRow(cur_row);
			}
		}
	}

	if(program_length)
	{
		DWARF_RuleMatrix<MEMORY_ADDR> *initial_rule_matrix = rule_matrix ? new DWARF_RuleMatrix<MEMORY_ADDR>(*rule_matrix) : 0;
		
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
						/* The DW_CFA_advance instruction takes a single operand (encoded with the opcode) that
						represents a constant delta. The required action is to create a new table row with a location
						value that is computed by taking the current entry’s location value and adding the value of
						delta * code_alignment_factor. All other values in the new row are initially identical
						to the current row. */

						uint8_t delta = opcode & 0x3f;
						
						uint64_t advance_loc = delta * (uint64_t) dw_cie->GetCodeAlignmentFactor();

						if(os) *os << "DW_CFA_advance_loc(" << advance_loc << ")";
						
						if(dw_cfp_type == DW_CFP_INSTRUCTIONS)
						{
							new_location += advance_loc;
							
							if(rule_matrix && (cur_location != new_location))
							{
								rule_matrix->CloneRow(cur_location, new_location);
							}
						}
					}
					break;
				case DW_CFA_offset:
					{
						/* The DW_CFA_offset instruction takes two operands: a register number (encoded with the
						opcode) and an unsigned LEB128 constant representing a factored offset. The required action
						is to change the rule for the register indicated by the register number to be an offset(N) rule
						with a value of (N = factored offset * data_alignment_factor). */

						uint8_t reg_num = opcode & 0x3f;
						DWARF_LEB128 factored_offset;
						int64_t sz;
						
						if((sz = factored_offset.Load(program, program_length)) < 0)
						{
							status = false;
							break;
						}
						program += sz;
						program_length -= sz;

						int64_t n = (uint64_t) factored_offset * (int64_t) dw_cie->GetDataAlignmentFactor();

						if(os) *os << "DW_CFA_offset(" << (uint32_t) reg_num << "," << factored_offset.to_string(false) << ") /* r" << (uint32_t) reg_num << " at cfa" << (n >= 0 ? "+" : "") << n << " */";

						if(rule_matrix)
						{
							DWARF_RuleMatrixRow<MEMORY_ADDR> *cur_row = (dw_cfp_type == DW_CFP_INITIAL_INSTRUCTIONS) ? rule_matrix->GetInitialRow() : rule_matrix->GetRow(cur_location);
							
							if(!cur_row)
							{
								status = false;
								break;
							}
							
							DWARF_RegisterRuleOffset<MEMORY_ADDR> *new_rule = new DWARF_RegisterRuleOffset<MEMORY_ADDR>(n);
							
							cur_row->SetRegisterRule(reg_num, new_rule);
						}
					}
					break;
				case DW_CFA_restore:
					{
						/* The DW_CFA_restore instruction takes a single operand (encoded with the opcode) that
						represents a register number. The required action is to change the rule for the indicated
						register to the rule assigned it by the initial_instructions in the CIE. */
						uint8_t reg_num = opcode & 0x3f;
						
						if(os) *os << "DW_CFA_restore(" << (uint32_t) reg_num << ")";

						if(dw_cfp_type == DW_CFP_INSTRUCTIONS)
						{
							if(rule_matrix)
							{
								DWARF_RuleMatrixRow<MEMORY_ADDR> *initial_row = initial_rule_matrix->GetRow(cur_location);
								
								if(!initial_row)
								{
									status = false;
									break;
								}

								DWARF_RuleMatrixRow<MEMORY_ADDR> *cur_row = rule_matrix->GetRow(cur_location);
								
								if(!cur_row)
								{
									status = false;
									break;
								}
								
								DWARF_RegisterRule<MEMORY_ADDR> *initial_reg_rule = initial_row->GetRegisterRule(reg_num);
								
								DWARF_RegisterRule<MEMORY_ADDR> *reg_rule_copy = initial_reg_rule ? initial_reg_rule->Clone() : 0;
								
								cur_row->SetRegisterRule(reg_num, reg_rule_copy);
							}
						}
					}
					break;
				case 0x00:
					switch(opcode)
					{
						case DW_CFA_nop:
							/* The DW_CFA_nop instruction has no operands and no required actions. It is used as padding
							to make a CIE or FDE an appropriate size. */
							if(os) *os << "DW_CFA_nop /* padding */";
							break;
						case DW_CFA_set_loc:
							{
								/* The DW_CFA_set_loc instruction takes a single operand that represents an address. The
								required action is to create a new table row using the specified address as the location. All
								other values in the new row are initially identical to the current row. The new location value
								should always be greater than the current one. */

								MEMORY_ADDR addr;
								
								switch(address_size)
								{
									case sizeof(uint16_t):
										{
											if(program_length < sizeof(uint16_t))
											{
												status = false;
												break;
											}
											uint16_t value;
											memcpy(&value, program, sizeof(uint16_t));
											addr = Target2Host(endianness, value);
											program += sizeof(uint16_t);
											program_length -= sizeof(uint16_t);
										}
										break;
									case sizeof(uint32_t):
										{
											if(program_length < sizeof(uint32_t))
											{
												status = false;
												break;
											}
											uint32_t value;
											memcpy(&value, program, sizeof(uint32_t));
											addr = Target2Host(endianness, value);
											program += sizeof(uint32_t);
											program_length -= sizeof(uint32_t);
										}
										break;
									case sizeof(uint64_t):
										{
											if(program_length < sizeof(uint64_t))
											{
												status = false;
												break;
											}
											uint64_t value;
											memcpy(&value, program, sizeof(uint64_t));
											addr = Target2Host(endianness, value);
											program += sizeof(uint64_t);
											program_length -= sizeof(uint64_t);
										}
										break;
									default:
										status = false;
										break;
								}
								if(!status) break;
								if(os) *os << "DW_CFA_set_loc(0x" << std::hex << addr << std::dec << ")";
								if(dw_cfp_type == DW_CFP_INSTRUCTIONS)
								{
									new_location = addr;
								}
							}
							break;
						case DW_CFA_advance_loc1:
							{
								/* The DW_CFA_advance_loc1 instruction takes a single ubyte operand that represents a
								constant delta. This instruction is identical to DW_CFA_advance_loc except for the encoding
								and size of the delta operand. */
								uint8_t delta;
								
								if(program_length < sizeof(delta))
								{
									status = false;
									break;
								}
								memcpy(&delta, program, sizeof(delta));
								delta = Target2Host(dw_call_frame_prog.GetEndianness(), delta);
								program += sizeof(delta);
								program_length -= sizeof(delta);
								uint64_t advance_loc = delta * (uint64_t) dw_cie->GetCodeAlignmentFactor();
								
								if(os) *os << "DW_CFA_advance_loc1(" << advance_loc << ")";
								
								if(dw_cfp_type == DW_CFP_INSTRUCTIONS)
								{
									new_location += advance_loc;
									if(rule_matrix && (cur_location != new_location))
									{
										rule_matrix->CloneRow(cur_location, new_location);
									}
								}
							}
							break;
						case DW_CFA_advance_loc2:
							{
								/* The DW_CFA_advance_loc2 instruction takes a single uhalf operand that represents a
								constant delta. This instruction is identical to DW_CFA_advance_loc except for the encoding
								and size of the delta operand. */
								uint16_t delta;
								
								if(program_length < sizeof(delta))
								{
									status = false;
									break;
								}
								memcpy(&delta, program, sizeof(delta));
								delta = Target2Host(dw_call_frame_prog.GetEndianness(), delta);
								program += sizeof(delta);
								program_length -= sizeof(delta);
								uint64_t advance_loc = delta * (uint64_t) dw_cie->GetCodeAlignmentFactor();

								if(os) *os << "DW_CFA_advance_loc2(" << advance_loc << ")";

								if(dw_cfp_type == DW_CFP_INSTRUCTIONS)
								{
									new_location += advance_loc;
									if(rule_matrix && (cur_location != new_location))
									{
										rule_matrix->CloneRow(cur_location, new_location);
									}
								}
							}
							break;
						case DW_CFA_advance_loc4:
							{
								/* The DW_CFA_advance_loc4 instruction takes a single uword operand that represents a
								constant delta. This instruction is identical to DW_CFA_advance_loc except for the encoding
								and size of the delta operand. */
								uint32_t delta;
								
								if(program_length < sizeof(delta))
								{
									status = false;
									break;
								}
								memcpy(&delta, program, sizeof(delta));
								delta = Target2Host(dw_call_frame_prog.GetEndianness(), delta);
								program += sizeof(delta);
								program_length -= sizeof(delta);
								uint64_t advance_loc = delta * (uint64_t) dw_cie->GetCodeAlignmentFactor();

								if(os) *os << "DW_CFA_advance_loc4(" << advance_loc << ")";

								if(dw_cfp_type == DW_CFP_INSTRUCTIONS)
								{
									new_location += advance_loc;
									if(rule_matrix && (cur_location != new_location))
									{
										rule_matrix->CloneRow(cur_location, new_location);
									}
								}
							}
							break;
						case DW_CFA_offset_extended:
							{
								/* The DW_CFA_offset_extended instruction takes two unsigned LEB128 operands
								representing a register number and a factored offset. This instruction is identical to
								DW_CFA_offset except for the encoding and size of the register operand. */
								DWARF_LEB128 reg_num;
								DWARF_LEB128 factored_offset;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0)
								{
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								if((sz = factored_offset.Load(program, program_length)) < 0)
								{
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								
								int64_t n = (uint64_t) factored_offset * (int64_t) dw_cie->GetDataAlignmentFactor();

								if(os) *os << "DW_CFA_offset_extended(" << reg_num.to_string(false) << "," << factored_offset.to_string(false) << ") /* r" << reg_num.to_string(false) << " at cfa" << (n >= 0 ? "+" : "") << n << " */";

								if(rule_matrix)
								{
									DWARF_RuleMatrixRow<MEMORY_ADDR> *cur_row = (dw_cfp_type == DW_CFP_INITIAL_INSTRUCTIONS) ? rule_matrix->GetInitialRow() : rule_matrix->GetRow(cur_location);
									
									if(!cur_row)
									{
										status = false;
										break;
									}
									
									DWARF_RegisterRuleOffset<MEMORY_ADDR> *new_rule = new DWARF_RegisterRuleOffset<MEMORY_ADDR>(n);
									cur_row->SetRegisterRule(reg_num, new_rule);
								}
							}
							break;
						case DW_CFA_restore_extended:
							{
								/* The DW_CFA_restore_extended instruction takes a single unsigned LEB128 operand that
								represents a register number. This instruction is identical to DW_CFA_restore except for the
								encoding and size of the register operand. */
								DWARF_LEB128 reg_num;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0)
								{
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								
								if(os) *os << "DW_CFA_restore_extended(" << reg_num.to_string(false) << ")";

								if(dw_cfp_type == DW_CFP_INSTRUCTIONS)
								{
									if(rule_matrix)
									{
										DWARF_RuleMatrixRow<MEMORY_ADDR> *initial_row = initial_rule_matrix->GetRow(cur_location);
										
										if(!initial_row)
										{
											status = false;
											break;
										}
										
										DWARF_RuleMatrixRow<MEMORY_ADDR> *cur_row = rule_matrix->GetRow(cur_location);
										
										if(!cur_row)
										{
											status = false;
											break;
										}
										
										DWARF_RegisterRule<MEMORY_ADDR> *initial_reg_rule = initial_row->GetRegisterRule(reg_num);
										
										DWARF_RegisterRule<MEMORY_ADDR> *reg_rule_copy = initial_reg_rule ? initial_reg_rule->Clone() : 0;
										
										cur_row->SetRegisterRule(reg_num, reg_rule_copy);
									}
								}
							}
							break;
						case DW_CFA_undefined:
							{
								/* The DW_CFA_undefined instruction takes a single unsigned LEB128 operand that
								represents a register number. The required action is to set the rule for the specified register to
								"undefined." */
								DWARF_LEB128 reg_num;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0)
								{
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								
								if(os) *os << "DW_CFA_undefined(" << reg_num.to_string(false) << ") /* r" << reg_num.to_string(false) << " not preserved */";

								if(rule_matrix)
								{
									DWARF_RuleMatrixRow<MEMORY_ADDR> *cur_row = (dw_cfp_type == DW_CFP_INITIAL_INSTRUCTIONS) ? rule_matrix->GetInitialRow() : rule_matrix->GetRow(cur_location);
									
									if(!cur_row)
									{
										status = false;
										break;
									}
									
									DWARF_RegisterRuleUndefined<MEMORY_ADDR> *new_rule = new DWARF_RegisterRuleUndefined<MEMORY_ADDR>();
									cur_row->SetRegisterRule(reg_num, new_rule);
								}
							}
							break;
						case DW_CFA_same_value:
							{
								/* The DW_CFA_same_value instruction takes a single unsigned LEB128 operand that
								represents a register number. The required action is to set the rule for the specified register to
								"same value." */
								DWARF_LEB128 reg_num;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0)
								{
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								
								if(os) *os << "DW_CFA_same_value(" << reg_num.to_string(false) << ") /* r" << reg_num.to_string(false) << " preserved */";

								if(rule_matrix)
								{
									DWARF_RuleMatrixRow<MEMORY_ADDR> *cur_row = (dw_cfp_type == DW_CFP_INITIAL_INSTRUCTIONS) ? rule_matrix->GetInitialRow() : rule_matrix->GetRow(cur_location);
									
									if(!cur_row)
									{
										status = false;
										break;
									}
									
									DWARF_RegisterRuleSameValue<MEMORY_ADDR> *new_rule = new DWARF_RegisterRuleSameValue<MEMORY_ADDR>();
									cur_row->SetRegisterRule(reg_num, new_rule);
								}
							}
							break;
						case DW_CFA_register:
							{
								/* The DW_CFA_register instruction takes two unsigned LEB128 operands representing
								register numbers. The required action is to set the rule for the first register to be the second
								register. */
								DWARF_LEB128 reg_num1;
								DWARF_LEB128 reg_num2;
								int64_t sz;
								
								if((sz = reg_num1.Load(program, program_length)) < 0)
								{
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								if((sz = reg_num2.Load(program, program_length)) < 0)
								{
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;

								if(os) *os << "DW_CFA_register(" << reg_num1.to_string(false) << ", " << reg_num2.to_string(false) << ") /* r" << reg_num1.to_string(false) << "=r" << reg_num2.to_string(false) << " */";
								
								if(rule_matrix)
								{
									DWARF_RuleMatrixRow<MEMORY_ADDR> *cur_row = (dw_cfp_type == DW_CFP_INITIAL_INSTRUCTIONS) ? rule_matrix->GetInitialRow() : rule_matrix->GetRow(cur_location);
									
									if(!cur_row)
									{
										status = false;
										break;
									}
									
									DWARF_RegisterRuleRegister<MEMORY_ADDR> *new_rule = new DWARF_RegisterRuleRegister<MEMORY_ADDR>((unsigned int) reg_num2);
									cur_row->SetRegisterRule(reg_num1, new_rule);
								}
							}
							break;
						case DW_CFA_remember_state:
							/* The DW_CFA_remember_state instruction takes no operands. The required action is to push
							the set of rules for every register onto an implicit stack. */
							if(os) *os << "DW_CFA_remember_state";
							if(dw_cfp_type == DW_CFP_INSTRUCTIONS)
							{
								if(rule_matrix)
								{
									if(!RememberState(rule_matrix, cur_location))
									{
										status = false;
										break;
									}
								}
							}
							break;
						case DW_CFA_restore_state:
							/* The DW_CFA_restore_state instruction takes no operands. The required action is to pop the
							set of rules off the implicit stack and place them in the current row. */
							if(os) *os << "DW_CFA_restore_state";
							if(dw_cfp_type == DW_CFP_INSTRUCTIONS)
							{
								if(rule_matrix)
								{
									if(!RestoreState(rule_matrix, cur_location))
									{
										status = false;
										break;
									}
								}
							}
							break;
						case DW_CFA_def_cfa:
							{
								/* The DW_CFA_def_cfa instruction takes two unsigned LEB128 operands representing a
								register number and a (non-factored) offset. The required action is to define the current CFA
								rule to use the provided register and offset. */
								DWARF_LEB128 reg_num;
								DWARF_LEB128 offset;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0)
								{
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								if((sz = offset.Load(program, program_length)) < 0)
								{
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								
								if(os) *os << "DW_CFA_def_cfa(" << reg_num.to_string(false) << ", " << offset.to_string(false) << ") /* cfa=[r" << reg_num.to_string(false) << "]+" << offset.to_string(false) << " */";
								
								if(rule_matrix)
								{
									DWARF_RuleMatrixRow<MEMORY_ADDR> *cur_row = (dw_cfp_type == DW_CFP_INITIAL_INSTRUCTIONS) ? rule_matrix->GetInitialRow() : rule_matrix->GetRow(cur_location);
									
									if((dw_cfp_type == DW_CFP_INSTRUCTIONS) && !cur_row)
									{
										cur_row = new DWARF_RuleMatrixRow<MEMORY_ADDR>(cur_location, *rule_matrix->GetInitialRow());
										rule_matrix->InsertRow(cur_row);
									}
									
									DWARF_CFARule<MEMORY_ADDR> *new_cfa_rule = new DWARF_CFARuleRegisterOffset<MEMORY_ADDR>(reg_num, offset);
									cur_row->SetCFARule(new_cfa_rule);
								}
							}
							break;
						case DW_CFA_def_cfa_register:
							{
								/* The DW_CFA_def_cfa_register instruction takes a single unsigned LEB128 operand
								representing a register number. The required action is to define the current CFA rule to use
								the provided register (but to keep the old offset). */
								DWARF_LEB128 reg_num;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0)
								{
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								
								if(os) *os << "DW_CFA_def_cfa_register(" << reg_num.to_string(false) << ")";
								
								if(rule_matrix)
								{
									DWARF_RuleMatrixRow<MEMORY_ADDR> *cur_row = (dw_cfp_type == DW_CFP_INITIAL_INSTRUCTIONS) ? rule_matrix->GetInitialRow() : rule_matrix->GetRow(cur_location);
									
									if((dw_cfp_type == DW_CFP_INSTRUCTIONS) && !cur_row)
									{
										cur_row = new DWARF_RuleMatrixRow<MEMORY_ADDR>(cur_location, *rule_matrix->GetInitialRow());
										rule_matrix->InsertRow(cur_row);
									}
									
									DWARF_CFARule<MEMORY_ADDR> *cur_cfa_rule = cur_row->GetCFARule();
									
									if(!cur_cfa_rule || (cur_cfa_rule->GetType() != DW_CFA_RULE_REGISTER_OFFSET))
									{
										status = false;
										break;
									}
									
									reinterpret_cast<DWARF_CFARuleRegisterOffset<MEMORY_ADDR> *>(cur_cfa_rule)->SetRegisterNumber(reg_num);
								}
							}
							break;
						case DW_CFA_def_cfa_offset:
							{
								/* The DW_CFA_def_cfa_offset instruction takes a single unsigned LEB128 operand
								representing a (non-factored) offset. The required action is to define the current CFA rule to
								use the provided offset (but to keep the old register). */
								DWARF_LEB128 offset;
								int64_t sz;
								
								if((sz = offset.Load(program, program_length)) < 0)
								{
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								
								if(os) *os << "DW_CFA_def_cfa_offset(" << offset.to_string(false) << ")";
								
								if(rule_matrix)
								{
									DWARF_RuleMatrixRow<MEMORY_ADDR> *cur_row = (dw_cfp_type == DW_CFP_INITIAL_INSTRUCTIONS) ? rule_matrix->GetInitialRow() : rule_matrix->GetRow(cur_location);
									
									if((dw_cfp_type == DW_CFP_INSTRUCTIONS) && !cur_row)
									{
										cur_row = new DWARF_RuleMatrixRow<MEMORY_ADDR>(cur_location, *rule_matrix->GetInitialRow());
										rule_matrix->InsertRow(cur_row);
									}
									
									DWARF_CFARule<MEMORY_ADDR> *cur_cfa_rule = cur_row->GetCFARule();
									
									if(!cur_cfa_rule || (cur_cfa_rule->GetType() != DW_CFA_RULE_REGISTER_OFFSET))
									{
										status = false;
										break;
									}
									
									reinterpret_cast<DWARF_CFARuleRegisterOffset<MEMORY_ADDR> *>(cur_cfa_rule)->SetOffset((uint64_t) offset);
								}
							}
							break;
						case DW_CFA_def_cfa_expression:
							{
								/* The DW_CFA_def_cfa_expression instruction takes a single operand encoded as a
								DW_FORM_block value representing a DWARF expression. The required action is to
								establish that expression as the means by which the current CFA is computed.
								The DW_OP_call2, DW_OP_call4, DW_OP_call_ref DW_OP_push_object_address and
								DW_OP_call_frame_cfa DWARF operators (see Section 2.5.1) cannot be used in such a
								DWARF expression. */
								DWARF_LEB128 leb128_block_length;
								int64_t sz;
								if((sz = leb128_block_length.Load(program, program_length)) < 0)
								{
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;

								uint64_t block_length = (uint64_t) leb128_block_length;
								if(program_length < block_length)
								{
									status = false;
									break;
								}

								DWARF_Expression<MEMORY_ADDR> *dw_expr = new DWARF_Expression<MEMORY_ADDR>(&dw_call_frame_prog, block_length, program);
								program += block_length;
								program_length -= block_length;
								
								if(os) *os << "DW_CFA_def_cfa_expression({" << dw_expr->to_string() << "})";
								
								if(rule_matrix)
								{
									DWARF_RuleMatrixRow<MEMORY_ADDR> *cur_row = (dw_cfp_type == DW_CFP_INITIAL_INSTRUCTIONS) ? rule_matrix->GetInitialRow() : rule_matrix->GetRow(cur_location);
									
									if((dw_cfp_type == DW_CFP_INSTRUCTIONS) && !cur_row)
									{
										cur_row = new DWARF_RuleMatrixRow<MEMORY_ADDR>(cur_location, *rule_matrix->GetInitialRow());
										rule_matrix->InsertRow(cur_row);
									}
									
									DWARF_CFARule<MEMORY_ADDR> *new_cfa_rule = new DWARF_CFARuleExpression<MEMORY_ADDR>(dw_expr);
									
									cur_row->SetCFARule(new_cfa_rule);
								}
								if(!rule_matrix) delete dw_expr;
							}
							break;
						case DW_CFA_expression:
							{
								/* The DW_CFA_expression instruction takes two operands: an unsigned LEB128 value
								representing a register number, and a DW_FORM_block value representing a DWARF
								expression. The required action is to change the rule for the register indicated by the register
								number to be an expression(E) rule where E is the DWARF expression. That is, the DWARF
								expression computes the address. The value of the CFA is pushed on the DWARF evaluation
								stack prior to execution of the DWARF expression.
								The DW_OP_call2, DW_OP_call4, DW_OP_call_ref, DW_OP_push_object_address and
								DW_OP_call_frame_cfa DWARF operators (see Section 2.5.1) cannot be used in such a
								DWARF expression. */
								DWARF_LEB128 reg_num;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0)
								{
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;

								DWARF_LEB128 leb128_block_length;
								if((sz = leb128_block_length.Load(program, program_length)) < 0)
								{
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;

								uint64_t block_length = (uint64_t) leb128_block_length;
								if(program_length < block_length)
								{
									status = false;
									break;
								}

								DWARF_Expression<MEMORY_ADDR> *dw_expr = new DWARF_Expression<MEMORY_ADDR>(&dw_call_frame_prog, block_length, program);
								program += block_length;
								program_length -= block_length;
								
								if(os) *os << "DW_CFA_expression(" << reg_num.to_string(false) << ", {" << dw_expr->to_string() << "})";
								
								if(rule_matrix)
								{
									DWARF_RuleMatrixRow<MEMORY_ADDR> *cur_row = (dw_cfp_type == DW_CFP_INITIAL_INSTRUCTIONS) ? rule_matrix->GetInitialRow() : rule_matrix->GetRow(cur_location);
									
									if(!cur_row)
									{
										status = false;
										break;
									}
									
									DWARF_RegisterRuleExpression<MEMORY_ADDR> *new_rule = new DWARF_RegisterRuleExpression<MEMORY_ADDR>(dw_expr);
									cur_row->SetRegisterRule(reg_num, new_rule);
								}
								
								if(!rule_matrix) delete dw_expr;
							}
							break;
						case DW_CFA_offset_extended_sf:
							{
								/* The DW_CFA_offset_extended_sf instruction takes two operands: an unsigned LEB128
								value representing a register number and a signed LEB128 factored offset. This instruction is
								identical to DW_CFA_offset_extended except that the second operand is signed and
								factored. */
								DWARF_LEB128 reg_num;
								DWARF_LEB128 signed_factored_offset;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0)
								{
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								if((sz = signed_factored_offset.Load(program, program_length)) < 0)
								{
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								
								int64_t n = (int64_t) signed_factored_offset * (int64_t) dw_cie->GetDataAlignmentFactor();
								
								if(os) *os << "DW_CFA_offset_extended_sf(" << reg_num.to_string(false) << "," << signed_factored_offset.to_string(true) << ") /* r" << reg_num.to_string(false) << " at cfa" << (n >= 0 ? "+" : "") << n << " */";
								
								if(rule_matrix)
								{
									DWARF_RuleMatrixRow<MEMORY_ADDR> *cur_row = (dw_cfp_type == DW_CFP_INITIAL_INSTRUCTIONS) ? rule_matrix->GetInitialRow() : rule_matrix->GetRow(cur_location);
									
									if(!cur_row)
									{
										status = false;
										break;
									}
									
									DWARF_RegisterRuleOffset<MEMORY_ADDR> *new_rule = new DWARF_RegisterRuleOffset<MEMORY_ADDR>(n);
									cur_row->SetRegisterRule(reg_num, new_rule);
								}
							}
							break;
						case DW_CFA_def_cfa_sf:
							{
								/* The DW_CFA_def_cfa_sf instruction takes two operands: an unsigned LEB128 value
								representing a register number and a signed LEB128 factored offset. This instruction is
								identical to DW_CFA_def_cfa except that the second operand is signed and factored. */
								DWARF_LEB128 reg_num;
								DWARF_LEB128 signed_factored_offset;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0)
								{
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								if((sz = signed_factored_offset.Load(program, program_length)) < 0)
								{
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								int64_t n = (int64_t) signed_factored_offset * (int64_t) dw_cie->GetDataAlignmentFactor();
								if(os) *os << "DW_CFA_def_cfa_sf(" << reg_num.to_string(false) << ", " << signed_factored_offset.to_string(true) << ") /* cfa=[" << reg_num.to_string(false) << "]+" << (n > 0 ? "+":"") << n << " */";

								if(rule_matrix)
								{
									DWARF_RuleMatrixRow<MEMORY_ADDR> *cur_row = (dw_cfp_type == DW_CFP_INITIAL_INSTRUCTIONS) ? rule_matrix->GetInitialRow() : rule_matrix->GetRow(cur_location);
									
									if((dw_cfp_type == DW_CFP_INSTRUCTIONS) && !cur_row)
									{
										cur_row = new DWARF_RuleMatrixRow<MEMORY_ADDR>(cur_location, *rule_matrix->GetInitialRow());
										rule_matrix->InsertRow(cur_row);
									}
									
									DWARF_CFARule<MEMORY_ADDR> *new_cfa_rule = new DWARF_CFARuleRegisterOffset<MEMORY_ADDR>(reg_num, n);
									cur_row->SetCFARule(new_cfa_rule);
								}
							}
							break;
						case DW_CFA_def_cfa_offset_sf:
							{
								/* The DW_CFA_def_cfa_offset_sf instruction takes a signed LEB128 operand representing a
								factored offset. This instruction is identical to DW_CFA_def_cfa_offset except that the
								operand is signed and factored. */
								DWARF_LEB128 signed_factored_offset;
								int64_t sz;
								
								if((sz = signed_factored_offset.Load(program, program_length)) < 0)
								{
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								if(os) *os << "DW_CFA_def_cfa_offset_sf(" << signed_factored_offset.to_string(true) << ")";
								int64_t n = (int64_t) signed_factored_offset * (int64_t) dw_cie->GetDataAlignmentFactor();

								if(rule_matrix)
								{
									DWARF_RuleMatrixRow<MEMORY_ADDR> *cur_row = (dw_cfp_type == DW_CFP_INITIAL_INSTRUCTIONS) ? rule_matrix->GetInitialRow() : rule_matrix->GetRow(cur_location);
									
									if((dw_cfp_type == DW_CFP_INSTRUCTIONS) && !cur_row)
									{
										cur_row = new DWARF_RuleMatrixRow<MEMORY_ADDR>(cur_location, *rule_matrix->GetInitialRow());
										rule_matrix->InsertRow(cur_row);
									}
									
									DWARF_CFARule<MEMORY_ADDR> *cur_cfa_rule = cur_row->GetCFARule();
									
									if(!cur_cfa_rule || (cur_cfa_rule->GetType() != DW_CFA_RULE_REGISTER_OFFSET))
									{
										status = false;
										break;
									}
									
									reinterpret_cast<DWARF_CFARuleRegisterOffset<MEMORY_ADDR> *>(cur_cfa_rule)->SetOffset(n);
								}
							}
							break;
						case DW_CFA_val_offset:
							{
								/* The DW_CFA_val_offset instruction takes two unsigned LEB128 operands representing a
								register number and a factored offset. The required action is to change the rule for the
								register indicated by the register number to be a val_offset(N) rule with a value of (N =
								factored offset * data_alignment_factor). */
								DWARF_LEB128 reg_num;
								DWARF_LEB128 factored_offset;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0)
								{
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								if((sz = factored_offset.Load(program, program_length)) < 0)
								{
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								
								int64_t n = (uint64_t) factored_offset * (int64_t) dw_cie->GetDataAlignmentFactor();
								if(os) *os << "DW_CFA_val_offset(" << reg_num.to_string(false) << "," << factored_offset.to_string(false) << ") /* r" << reg_num.to_string(false) << "=cfa" << (n >= 0 ? "+" : "") << n << " */";

								if(rule_matrix)
								{
									DWARF_RuleMatrixRow<MEMORY_ADDR> *cur_row = (dw_cfp_type == DW_CFP_INITIAL_INSTRUCTIONS) ? rule_matrix->GetInitialRow() : rule_matrix->GetRow(cur_location);
									
									if(!cur_row)
									{
										status = false;
										break;
									}
									
									DWARF_RegisterRuleValOffset<MEMORY_ADDR> *new_rule = new DWARF_RegisterRuleValOffset<MEMORY_ADDR>(n);
									cur_row->SetRegisterRule(reg_num, new_rule);
								}
							}
							break;
						case DW_CFA_val_offset_sf:
							{
								/* The DW_CFA_val_offset_sf instruction takes two operands: an unsigned LEB128 value
								representing a register number and a signed LEB128 factored offset. This instruction is
								identical to DW_CFA_val_offset except that the second operand is signed and factored. */
								DWARF_LEB128 reg_num;
								DWARF_LEB128 signed_factored_offset;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0)
								{
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								if((sz = signed_factored_offset.Load(program, program_length)) < 0)
								{
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;
								
								int64_t n = (int64_t) signed_factored_offset * (int64_t) dw_cie->GetDataAlignmentFactor();

								if(os) *os << "DW_CFA_val_offset_sf(" << reg_num.to_string(false) << "," << signed_factored_offset.to_string(true) << ") /* r" << reg_num.to_string(false) << "=cfa" << (n >= 0 ? "+" : "") << n << " */";

								if(rule_matrix)
								{
									DWARF_RuleMatrixRow<MEMORY_ADDR> *cur_row = (dw_cfp_type == DW_CFP_INITIAL_INSTRUCTIONS) ? rule_matrix->GetInitialRow() : rule_matrix->GetRow(cur_location);
									
									if(!cur_row)
									{
										status = false;
										break;
									}
									
									DWARF_RegisterRuleValOffset<MEMORY_ADDR> *new_rule = new DWARF_RegisterRuleValOffset<MEMORY_ADDR>(n);
									cur_row->SetRegisterRule(reg_num, new_rule);
								}
							}
							break;
						case DW_CFA_val_expression:
							{
								/* The DW_CFA_val_expression instruction takes two operands: an unsigned LEB128 value
								representing a register number, and a DW_FORM_block value representing a DWARF
								expression. The required action is to change the rule for the register indicated by the register
								number to be a val_expression(E) rule where E is the DWARF expression. That is, the
								DWARF expression computes the value of the given register. The value of the CFA is
								pushed on the DWARF evaluation stack prior to execution of the DWARF expression.
								The DW_OP_call2, DW_OP_call4, DW_OP_call_ref and DW_OP_push_object_address
								DWARF operators (see Section 2.5.1) cannot be used in such a DWARF expression. */
								DWARF_LEB128 reg_num;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0)
								{
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;

								DWARF_LEB128 leb128_block_length;
								if((sz = leb128_block_length.Load(program, program_length)) < 0)
								{
									status = false;
									break;
								}
								program += sz;
								program_length -= sz;

								uint64_t block_length = (uint64_t) leb128_block_length;
								if(program_length < block_length)
								{
									status = false;
									break;
								}

								DWARF_Expression<MEMORY_ADDR> *dw_expr = new DWARF_Expression<MEMORY_ADDR>(&dw_call_frame_prog, block_length, program);
								program += block_length;
								program_length -= block_length;
								
								if(os) *os << "DW_CFA_val_expression(" << reg_num.to_string(false) << ", {" << dw_expr->to_string() << "})";

								if(rule_matrix)
								{
									DWARF_RuleMatrixRow<MEMORY_ADDR> *cur_row = (dw_cfp_type == DW_CFP_INITIAL_INSTRUCTIONS) ? rule_matrix->GetInitialRow() : rule_matrix->GetRow(cur_location);
									
									if(!cur_row)
									{
										status = false;
										break;
									}
									
									DWARF_RegisterRuleExpression<MEMORY_ADDR> *new_rule = new DWARF_RegisterRuleExpression<MEMORY_ADDR>(dw_expr);
									cur_row->SetRegisterRule(reg_num, new_rule);
								}
								
								if(!rule_matrix) delete dw_expr;
							}
							break;
						default:
							status = false;
							break;
					}
					break;
				default:
					status = false;
					break;
			}
			
			cur_location = new_location;
			if(os && program_length) *os << std::endl;
		}
		while(status && program_length);
		
		if(initial_rule_matrix) delete initial_rule_matrix;
	}

	return status;
}

template <class MEMORY_ADDR>
const DWARF_RuleMatrix<MEMORY_ADDR> *DWARF_CallFrameVM<MEMORY_ADDR>::ComputeRuleMatrix(const DWARF_FDE<MEMORY_ADDR> *dw_fde)
{
	MEMORY_ADDR initial_location = dw_fde->GetInitialLocation();
	MEMORY_ADDR address_range = dw_fde->GetAddressRange();
	const DWARF_CIE<MEMORY_ADDR> *dw_cie = dw_fde->GetCIE();
	MEMORY_ADDR code_alignment_factor = (MEMORY_ADDR) dw_cie->GetCodeAlignmentFactor();
		
	DWARF_RuleMatrix<MEMORY_ADDR> *rule_matrix = new DWARF_RuleMatrix<MEMORY_ADDR>();
			
	const DWARF_CallFrameProgram<MEMORY_ADDR> *initial_instructions = dw_cie->GetInitialInstructions();
			
	MEMORY_ADDR location = 0;
	
	if(!Execute(*initial_instructions, location, rule_matrix))
	{
		delete rule_matrix;
		return 0;
	}
	location = initial_location;
	const DWARF_CallFrameProgram<MEMORY_ADDR> *instructions = dw_fde->GetInstructions();
	if(!Execute(*instructions, location, rule_matrix))
	{
		delete rule_matrix;
		return 0;
	}
	return rule_matrix;
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim
