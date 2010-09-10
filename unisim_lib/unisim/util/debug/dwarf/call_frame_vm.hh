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

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

const unsigned int DW_RULE_CFA = 0;
const unsigned int DW_RULE_UNDEFINED = 1;
const unsigned int DW_RULE_SAME_VALUE = 2;
const unsigned int DW_RULE_OFFSET = 3;
const unsigned int DW_RULE_VAL_OFFSET = 4;
const unsigned int DW_RULE_REGISTER = 5;
const unsigned int DW_RULE_EXPRESSION = 6;
const unsigned int DW_RULE_VAL_EXPRESSION = 7;
const unsigned int DW_RULE_ARCHITECTURAL = 8;

template <class MEMORY_ADDR>
class Rule
{
public:
	Rule(unsigned int dw_rule_type);
	virtual ~Rule();
	unsigned int GetType() const;
private:
	unsigned int dw_rule_type;
};

template <class MEMORY_ADDR>
class CFARule : public Rule<MEMORY_ADDR>
{
public:
	CFARule(const DWARF_CIE<MEMORY_ADDR> *dw_cie);
	~CFARule();
	void SetRegister(const DWARF_LEB128& dw_reg_num);
	void SetFactoredOffset(const DWARF_LEB128& dw_offset_sf);
	void SetOffset(const DWARF_LEB128& dw_offset);
	const DWARF_LEB128& GetRegisterNumber() const;
	int64_t GetOffset() const;
private:
	DWARF_LEB128 dw_reg_num;
	int64_t dw_offset_sf;
};

template <class MEMORY_ADDR>
class OffsetRule : public Rule<MEMORY_ADDR>
{
public:
	OffsetRule(const Rule<MEMORY_ADDR> *dw_cfa_rule, const DWARF_LEB128& dw_offset);
	virtual ~OffsetRule();
	MEMORY_ADDR GetValue() const;
private:
	const Rule<MEMORY_ADDR> *dw_cfa_rule;
	DWARF_LEB128 dw_offset; // unsigned
};

template <class MEMORY_ADDR>
class ValOffsetRule : public Rule<MEMORY_ADDR>
{
public:
	ValOffsetRule(const Rule<MEMORY_ADDR> *dw_cfa_rule, const DWARF_LEB128& dw_offset);
	virtual ~ValOffsetRule();
	MEMORY_ADDR GetValue() const;
private:
	const Rule<MEMORY_ADDR> *dw_cfa_rule;
	DWARF_LEB128 dw_offset; // unsigned
};

template <class MEMORY_ADDR>
class RegisterRule : public Rule<MEMORY_ADDR>
{
public:
	RegisterRule(const DWARF_LEB128& dw_reg_num);
	virtual ~RegisterRule();
	const DWARF_LEB128& GetValue() const;
private:
	DWARF_LEB128 dw_reg_num;
};

template <class MEMORY_ADDR>
class ExpressionRule : public Rule<MEMORY_ADDR>
{
public:
	ExpressionRule(const DWARF_Expression<MEMORY_ADDR> *dw_expr);
	virtual ~ExpressionRule();
	MEMORY_ADDR GetValue() const;
private:
	const DWARF_Expression<MEMORY_ADDR> *dw_expr;
};

template <class MEMORY_ADDR>
class ValExpressionRule : public Rule<MEMORY_ADDR>
{
public:
	ValExpressionRule(const DWARF_Expression<MEMORY_ADDR> *dw_expr);
	virtual ~ValExpressionRule();
private:
	const DWARF_Expression<MEMORY_ADDR> *dw_expr;
};

template <class MEMORY_ADDR>
class RuleMatrixRow : public Rule<MEMORY_ADDR>
{
public:
private:
	CFARule<MEMORY_ADDR> *cfa_rule;
	std::vector<RegisterRule<MEMORY_ADDR> *> reg_rules;
};

template <class MEMORY_ADDR>
class DWARF_CallFrameVM
{
public:
	DWARF_CallFrameVM();
	~DWARF_CallFrameVM();
	bool Disasm(std::ostream& os, const DWARF_CallFrameProgram<MEMORY_ADDR>& dw_call_frame_prog);
	bool Execute(const DWARF_CallFrameProgram<MEMORY_ADDR>& dw_call_frame_prog, std::map<MEMORY_ADDR, RuleMatrixRow<MEMORY_ADDR> *>& rule_matrix);
private:
	bool Run(const DWARF_CallFrameProgram<MEMORY_ADDR>& dw_call_frame_prog, std::ostream *os, std::map<MEMORY_ADDR, RuleMatrixRow<MEMORY_ADDR> *> *rule_matrix);
	
};

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
