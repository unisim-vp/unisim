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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_CALL_FRAME_VM_HH__
#define __UNISIM_UTIL_DEBUG_DWARF_CALL_FRAME_VM_HH__

#include <unisim/util/debug/dwarf/fwd.hh>
#include <map>
#include <vector>
#include <stack>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

const unsigned int DW_CFA_RULE_REGISTER_OFFSET = 0;
const unsigned int DW_CFA_RULE_EXPRESSION = 1;
	
const unsigned int DW_REG_RULE_UNDEFINED = 0;
const unsigned int DW_REG_RULE_SAME_VALUE = 1;
const unsigned int DW_REG_RULE_OFFSET = 2;
const unsigned int DW_REG_RULE_VAL_OFFSET = 3;
const unsigned int DW_REG_RULE_REGISTER = 4;
const unsigned int DW_REG_RULE_EXPRESSION = 5;
const unsigned int DW_REG_RULE_VAL_EXPRESSION = 6;

template <class MEMORY_ADDR>
class CFARule
{
public:
	CFARule(unsigned int type);
	virtual ~CFARule();
	unsigned int GetType() const;
private:
	unsigned int type;
};

template <class MEMORY_ADDR>
class CFARuleRegisterOffset : public CFARule<MEMORY_ADDR>
{
public:
	CFARuleRegisterOffset(unsigned int dw_reg_num, int64_t dw_offset);
	CFARuleRegisterOffset(const CFARuleRegisterOffset<MEMORY_ADDR>& cfa_rule_offset);
	virtual ~CFARuleRegisterOffset();
	void SetRegisterNumber(unsigned int dw_reg_num);
	void SetOffset(int64_t dw_offset);
	unsigned int GetRegisterNumber() const;
	int64_t GetOffset() const;
private:
	unsigned int dw_reg_num;
	int64_t dw_offset;
};

template <class MEMORY_ADDR>
class CFARuleExpression : public CFARule<MEMORY_ADDR>
{
public:
	CFARuleExpression(const DWARF_Expression<MEMORY_ADDR> *dw_expr);
	CFARuleExpression(const CFARuleExpression<MEMORY_ADDR>& cfa_rule_expression);
	virtual ~CFARuleExpression();
private:
	const DWARF_Expression<MEMORY_ADDR> *dw_expr;
};

template <class MEMORY_ADDR>
class RegisterRule
{
public:
	RegisterRule(unsigned int dw_rule_type);
	virtual ~RegisterRule();
	unsigned int GetType() const;
private:
	unsigned int dw_rule_type;
};

template <class MEMORY_ADDR>
class RegisterRuleUndefined : public RegisterRule<MEMORY_ADDR>
{
public:
	RegisterRuleUndefined(const RegisterRuleUndefined<MEMORY_ADDR>& rule);
	RegisterRuleUndefined();
	virtual ~RegisterRuleUndefined();
};

template <class MEMORY_ADDR>
class RegisterRuleSameValue : public RegisterRule<MEMORY_ADDR>
{
public:
	RegisterRuleSameValue(const RegisterRuleSameValue<MEMORY_ADDR>& rule);
	RegisterRuleSameValue();
	virtual ~RegisterRuleSameValue();
};

template <class MEMORY_ADDR>
class RegisterRuleOffset : public RegisterRule<MEMORY_ADDR>
{
public:
	RegisterRuleOffset(const RegisterRuleOffset<MEMORY_ADDR>& rule);
	RegisterRuleOffset(int64_t dw_offset);
	virtual ~RegisterRuleOffset();
	int64_t GetOffset() const;
private:
	int64_t dw_offset;
};

template <class MEMORY_ADDR>
class RegisterRuleValOffset : public RegisterRule<MEMORY_ADDR>
{
public:
	RegisterRuleValOffset(const RegisterRuleValOffset<MEMORY_ADDR>& rule);
	RegisterRuleValOffset(int64_t dw_offset);
	virtual ~RegisterRuleValOffset();
	int64_t GetOffset() const;
private:
	int64_t dw_offset;
};

template <class MEMORY_ADDR>
class RegisterRuleRegister : public RegisterRule<MEMORY_ADDR>
{
public:
	RegisterRuleRegister(const RegisterRuleRegister<MEMORY_ADDR>& rule);
	RegisterRuleRegister(unsigned int dw_reg_num);
	virtual ~RegisterRuleRegister();
	unsigned int GetRegisterNumber() const;
private:
	unsigned int dw_reg_num;
};

template <class MEMORY_ADDR>
class RegisterRuleExpression : public RegisterRule<MEMORY_ADDR>
{
public:
	RegisterRuleExpression(const RegisterRuleExpression<MEMORY_ADDR>& rule);
	RegisterRuleExpression(const DWARF_Expression<MEMORY_ADDR> *dw_expr);
	virtual ~RegisterRuleExpression();
	const DWARF_Expression<MEMORY_ADDR> *GetExpression() const;
private:
	const DWARF_Expression<MEMORY_ADDR> *dw_expr;
};

template <class MEMORY_ADDR>
class RegisterRuleValExpression : public RegisterRule<MEMORY_ADDR>
{
public:
	RegisterRuleValExpression(const RegisterRuleValExpression<MEMORY_ADDR>& rule);
	RegisterRuleValExpression(const DWARF_Expression<MEMORY_ADDR> *dw_expr);
	virtual ~RegisterRuleValExpression();
	const DWARF_Expression<MEMORY_ADDR> *GetExpression() const;
private:
	const DWARF_Expression<MEMORY_ADDR> *dw_expr;
};

template <class MEMORY_ADDR>
class RuleMatrixRow
{
public:
	RuleMatrixRow(MEMORY_ADDR location);
	RuleMatrixRow(MEMORY_ADDR location, const RuleMatrixRow<MEMORY_ADDR>& rule_matrix_row);
	~RuleMatrixRow();
	void SetCFARule(CFARule<MEMORY_ADDR> *cfa_rule);
	void SetRegisterRule(unsigned int reg_num, RegisterRule<MEMORY_ADDR> *reg_rule);
	void RestoreRegisterRule(unsigned int reg_num);
	MEMORY_ADDR GetLocation() const;
	CFARule<MEMORY_ADDR> *GetCFARule() const;
	RegisterRule<MEMORY_ADDR> *GetRegisterRule(unsigned int reg_num) const;
private:
	MEMORY_ADDR location;
	CFARule<MEMORY_ADDR> *cfa_rule;
	std::vector<RegisterRule<MEMORY_ADDR> *> reg_rules;
};

template <class MEMORY_ADDR>
class RuleMatrix
{
public:
	RuleMatrix();
	RuleMatrix(const RuleMatrix<MEMORY_ADDR>& rule_matrix);
	~RuleMatrix();
	
	RuleMatrixRow<MEMORY_ADDR> *operator[](MEMORY_ADDR loc);
	bool HasRow(MEMORY_ADDR loc) const;
	void InsertRow(RuleMatrixRow<MEMORY_ADDR> *rule_matrix_row);
	void CloneRow(MEMORY_ADDR cur_loc, MEMORY_ADDR new_loc);
	void RememberState(MEMORY_ADDR loc);
	void RestoreState(MEMORY_ADDR loc);
private:
	std::map<MEMORY_ADDR, RuleMatrixRow<MEMORY_ADDR> *> rule_matrix_rows;
	std::stack<RuleMatrixRow<MEMORY_ADDR> *> state_stack;
};

template <class MEMORY_ADDR>
class DWARF_CallFrameVM
{
public:
	DWARF_CallFrameVM();
	~DWARF_CallFrameVM();
	bool Disasm(std::ostream& os, const DWARF_CallFrameProgram<MEMORY_ADDR>& dw_call_frame_prog);
	bool Execute(const DWARF_CallFrameProgram<MEMORY_ADDR>& dw_call_frame_prog, MEMORY_ADDR& location, RuleMatrix<MEMORY_ADDR> *rule_matrix);
private:
	bool Run(const DWARF_CallFrameProgram<MEMORY_ADDR>& dw_call_frame_prog, std::ostream *os, MEMORY_ADDR *location, RuleMatrix<MEMORY_ADDR> *rule_matrix);
	
};

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
