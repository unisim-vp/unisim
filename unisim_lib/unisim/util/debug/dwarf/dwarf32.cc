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

#include <unisim/util/debug/dwarf/dwarf.hh>
#include <unisim/util/debug/dwarf/dwarf.tcc>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template class DWARF_StatementProgram<uint32_t>;
template class DWARF_StatementVM<uint32_t>;
template class DWARF_AttributeValue<uint32_t>;
template class DWARF_Address<uint32_t>;
template class DWARF_Block<uint32_t>;
template class DWARF_UnsignedConstant<uint32_t>;
template class DWARF_SignedConstant<uint32_t>;
template class DWARF_UnsignedLEB128Constant<uint32_t>;
template class DWARF_SignedLEB128Constant<uint32_t>;
template class DWARF_Flag<uint32_t>;
template class DWARF_LinePtr<uint32_t>;
template class DWARF_LocListPtr<uint32_t>;
template class DWARF_MacPtr<uint32_t>;
template class DWARF_RangeListPtr<uint32_t>;
template class DWARF_Reference<uint32_t>;
template class DWARF_String<uint32_t>;
template class DWARF_Expression<uint32_t>;
template class DWARF_Attribute<uint32_t>;
template class DWARF_DIE<uint32_t>;
template class DWARF_CompilationUnit<uint32_t>;
template class DWARF_ExpressionVM<uint32_t>;
template class DWARF_CallFrameProgram<uint32_t>;
template class DWARF_CIE<uint32_t>;
template class DWARF_FDE<uint32_t>;
template class DWARF_RegisterRule<uint32_t>;
template class DWARF_CFARule<uint32_t>;
template class DWARF_RegisterRuleUndefined<uint32_t>;
template class DWARF_RegisterRuleSameValue<uint32_t>;
template class DWARF_RegisterRuleOffset<uint32_t>;
template class DWARF_RegisterRuleValOffset<uint32_t>;
template class DWARF_RegisterRuleRegister<uint32_t>;
template class DWARF_RegisterRuleExpression<uint32_t>;
template class DWARF_RegisterRuleValExpression<uint32_t>;
template class DWARF_CFIRow<uint32_t>;
template class DWARF_CFI<uint32_t>;
template class DWARF_CallFrameVM<uint32_t>;
template class DWARF_Frame<uint32_t>;
template class DWARF_RangeListEntry<uint32_t>;
template class DWARF_MacInfoListEntry<uint32_t>;
template class DWARF_MacInfoListEntryDefine<uint32_t>;
template class DWARF_MacInfoListEntryUndef<uint32_t>;
template class DWARF_MacInfoListEntryStartFile<uint32_t>;
template class DWARF_MacInfoListEntryEndFile<uint32_t>;
template class DWARF_MacInfoListEntryVendorExtension<uint32_t>;
template class DWARF_AddressRangeDescriptor<uint32_t>;
template class DWARF_AddressRanges<uint32_t>;
template class DWARF_Pub<uint32_t>;
template class DWARF_Pubs<uint32_t>;
template class DWARF_LocListEntry<uint32_t>;
template class DWARF_Handler<uint32_t>;
template class DWARF_DataObject<uint32_t>;
template class DWARF_MachineState<uint32_t>;

template std::ostream& operator << <uint32_t>(std::ostream&, const DWARF_AddressRangeDescriptor<uint32_t>&);
template std::ostream& operator << <uint32_t>(std::ostream&, const DWARF_AddressRanges<uint32_t>&);
template std::ostream& operator << <uint32_t>(std::ostream&, const DWARF_AttributeValue<uint32_t>&);
template std::ostream& operator << <uint32_t>(std::ostream&, const DWARF_Attribute<uint32_t>&);
template std::ostream& operator << <uint32_t>(std::ostream&, const DWARF_CallFrameProgram<uint32_t>&);
template std::ostream& operator << <uint32_t>(std::ostream&, const DWARF_CFARule<uint32_t>&);
template std::ostream& operator << <uint32_t>(std::ostream&, const DWARF_RegisterRule<uint32_t>&);
template std::ostream& operator << <uint32_t>(std::ostream&, const DWARF_CFIRow<uint32_t>&);
template std::ostream& operator << <uint32_t>(std::ostream&, const DWARF_CFI<uint32_t>&);
template std::ostream& operator << <uint32_t>(std::ostream&, const DWARF_CIE<uint32_t>&);
template std::ostream& operator << <uint32_t>(std::ostream&, const DWARF_CompilationUnit<uint32_t>&);
template std::ostream& operator << <uint32_t>(std::ostream&, const DWARF_DIE<uint32_t>&);
template std::ostream& operator << <uint32_t>(std::ostream&, const DWARF_FDE<uint32_t>&);
template std::ostream& operator << <uint32_t>(std::ostream&, const DWARF_RegSet<uint32_t>&);
template std::ostream& operator << <uint32_t>(std::ostream&, const DWARF_LocListEntry<uint32_t>&);
template std::ostream& operator << <uint32_t>(std::ostream&, const DWARF_MacInfoListEntry<uint32_t>&);
template std::ostream& operator << <uint32_t>(std::ostream&, const DWARF_Pub<uint32_t>&);
template std::ostream& operator << <uint32_t>(std::ostream&, const DWARF_Pubs<uint32_t>&);
template std::ostream& operator << <uint32_t>(std::ostream&, const DWARF_RangeListEntry<uint32_t>&);
template std::ostream& operator << <uint32_t>(std::ostream&, const DWARF_StatementProgram<uint32_t>&);

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim
