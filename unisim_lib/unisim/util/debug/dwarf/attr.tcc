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

#include <unisim/util/debug/dwarf/attr.hh>
#include <unisim/util/debug/dwarf/encoding.hh>
#include <unisim/util/debug/dwarf/ml.hh>
#include <iostream>
#include <sstream>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
DWARF_AttributeValue<MEMORY_ADDR>::DWARF_AttributeValue(unsigned int _dw_class)
	: dw_class(_dw_class)
{
}

template <class MEMORY_ADDR>
DWARF_AttributeValue<MEMORY_ADDR>::~DWARF_AttributeValue()
{
}

template <class MEMORY_ADDR>
unsigned int DWARF_AttributeValue<MEMORY_ADDR>::DWARF_AttributeValue::GetClass() const
{
	return dw_class;
}

template <class MEMORY_ADDR>
const char *DWARF_AttributeValue<MEMORY_ADDR>::GetClassName() const
{
	return DWARF_GetCLASSName(dw_class);
}

template <class MEMORY_ADDR>
uint64_t DWARF_AttributeValue<MEMORY_ADDR>::to_int() const
{
	return 0;
}

template <class MEMORY_ADDR>
void DWARF_AttributeValue<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler)
{
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_AttributeValue<MEMORY_ADDR>& dw_value)
{
	return os << dw_value.to_string();
}

template <class MEMORY_ADDR>
DWARF_Block<MEMORY_ADDR>::DWARF_Block(uint64_t _length, const uint8_t *_value)
	: DWARF_AttributeValue<MEMORY_ADDR>(DW_CLASS_BLOCK)
	, length(_length)
	, value(_value)
{
}

template <class MEMORY_ADDR>
DWARF_Block<MEMORY_ADDR>::~DWARF_Block()
{
}

template <class MEMORY_ADDR>
uint64_t DWARF_Block<MEMORY_ADDR>::GetLength() const
{
	return length;
}

template <class MEMORY_ADDR>
const uint8_t *DWARF_Block<MEMORY_ADDR>::GetValue() const
{
	return value;
}

template <class MEMORY_ADDR>
std::string DWARF_Block<MEMORY_ADDR>::to_string() const
{
	std::stringstream sstr;
	uint64_t i;
	for(i = 0; i < length; i++)
	{
		if(i != 0) sstr << " ";
		sstr << std::hex << (unsigned int) value[i] << std::dec;
	}
	return std::string(sstr.str());
}

template <class MEMORY_ADDR>
DWARF_UnsignedConstant<MEMORY_ADDR>::DWARF_UnsignedConstant(uint64_t _value)
	: DWARF_AttributeValue<MEMORY_ADDR>(DW_CLASS_UNSIGNED_CONSTANT)
	, value(_value)
{
}

template <class MEMORY_ADDR>
DWARF_UnsignedConstant<MEMORY_ADDR>::~DWARF_UnsignedConstant()
{
}

template <class MEMORY_ADDR>
uint64_t DWARF_UnsignedConstant<MEMORY_ADDR>::GetValue() const
{
	return value;
}

template <class MEMORY_ADDR>
std::string DWARF_UnsignedConstant<MEMORY_ADDR>::to_string() const
{
	std::stringstream sstr;
	sstr << value;
	return std::string(sstr.str());
}

template <class MEMORY_ADDR>
uint64_t DWARF_UnsignedConstant<MEMORY_ADDR>::to_int() const
{
	return value;
}

template <class MEMORY_ADDR>
DWARF_SignedConstant<MEMORY_ADDR>::DWARF_SignedConstant(int64_t _value)
	: DWARF_AttributeValue<MEMORY_ADDR>(DW_CLASS_SIGNED_CONSTANT)
	, value(_value)
{
}

template <class MEMORY_ADDR>
DWARF_SignedConstant<MEMORY_ADDR>::~DWARF_SignedConstant()
{
}

template <class MEMORY_ADDR>
int64_t DWARF_SignedConstant<MEMORY_ADDR>::GetValue() const
{
	return value;
}

template <class MEMORY_ADDR>
std::string DWARF_SignedConstant<MEMORY_ADDR>::to_string() const
{
	std::stringstream sstr;
	sstr << value;
	return std::string(sstr.str());
}

template <class MEMORY_ADDR>
uint64_t DWARF_SignedConstant<MEMORY_ADDR>::to_int() const
{
	return (uint64_t) value;
}

template <class MEMORY_ADDR>
DWARF_UnsignedLEB128Constant<MEMORY_ADDR>::DWARF_UnsignedLEB128Constant(const DWARF_LEB128& leb128)
	: DWARF_AttributeValue<MEMORY_ADDR>(DW_CLASS_UNSIGNED_LEB128_CONSTANT)
	, value(leb128)
{
}

template <class MEMORY_ADDR>
DWARF_UnsignedLEB128Constant<MEMORY_ADDR>::~DWARF_UnsignedLEB128Constant()
{
}

template <class MEMORY_ADDR>
const DWARF_LEB128& DWARF_UnsignedLEB128Constant<MEMORY_ADDR>::GetValue() const
{
	return value;
}

template <class MEMORY_ADDR>
std::string DWARF_UnsignedLEB128Constant<MEMORY_ADDR>::to_string() const
{
	return value.to_string(false);
}

template <class MEMORY_ADDR>
uint64_t DWARF_UnsignedLEB128Constant<MEMORY_ADDR>::to_int() const
{
	return (uint64_t) value;
}

template <class MEMORY_ADDR>
DWARF_SignedLEB128Constant<MEMORY_ADDR>::DWARF_SignedLEB128Constant(const DWARF_LEB128& leb128)
	: DWARF_AttributeValue<MEMORY_ADDR>(DW_CLASS_UNSIGNED_LEB128_CONSTANT)
	, value(leb128)
{
}

template <class MEMORY_ADDR>
DWARF_SignedLEB128Constant<MEMORY_ADDR>::~DWARF_SignedLEB128Constant()
{
}

template <class MEMORY_ADDR>
const DWARF_LEB128& DWARF_SignedLEB128Constant<MEMORY_ADDR>::GetValue() const
{
	return value;
}

template <class MEMORY_ADDR>
std::string DWARF_SignedLEB128Constant<MEMORY_ADDR>::to_string() const
{
	return value.to_string(true);
}

template <class MEMORY_ADDR>
uint64_t DWARF_SignedLEB128Constant<MEMORY_ADDR>::to_int() const
{
	return (uint64_t) (int64_t) value;
}

template <class MEMORY_ADDR>
DWARF_Flag<MEMORY_ADDR>::DWARF_Flag(bool _value)
	: DWARF_AttributeValue<MEMORY_ADDR>(DW_CLASS_FLAG)
	, value(_value)
{
}

template <class MEMORY_ADDR>
DWARF_Flag<MEMORY_ADDR>::~DWARF_Flag()
{
}

template <class MEMORY_ADDR>
bool DWARF_Flag<MEMORY_ADDR>::GetValue() const
{
	return value;
}

template <class MEMORY_ADDR>
std::string DWARF_Flag<MEMORY_ADDR>::to_string() const
{
	return value ? std::string("1") : std::string("0");
}

template <class MEMORY_ADDR>
uint64_t DWARF_Flag<MEMORY_ADDR>::to_int() const
{
	return value ? 1 : 0;
}

template <class MEMORY_ADDR>
DWARF_String<MEMORY_ADDR>::DWARF_String(uint64_t _debug_strp_offset)
	: DWARF_AttributeValue<MEMORY_ADDR>(DW_CLASS_STRING)
	, debug_strp_offset(_debug_strp_offset)
	, value("")
{
}

template <class MEMORY_ADDR>
DWARF_String<MEMORY_ADDR>::DWARF_String(const char *_value)
	: DWARF_AttributeValue<MEMORY_ADDR>(DW_CLASS_STRING)
	, debug_strp_offset(0xffffffffffffffffULL)
	, value(_value)
{
}

template <class MEMORY_ADDR>
DWARF_String<MEMORY_ADDR>::~DWARF_String()
{
}

template <class MEMORY_ADDR>
unsigned int DWARF_String<MEMORY_ADDR>::GetLength() const
{
	return strlen(value);
}

template <class MEMORY_ADDR>
const char *DWARF_String<MEMORY_ADDR>::GetValue() const
{
	return value;
}

template <class MEMORY_ADDR>
std::string DWARF_String<MEMORY_ADDR>::to_string() const
{
	return std::string(value);
}

template <class MEMORY_ADDR>
DWARF_Address<MEMORY_ADDR>::DWARF_Address(MEMORY_ADDR addr)
	: DWARF_AttributeValue<MEMORY_ADDR>(DW_CLASS_ADDRESS)
	, value(addr)
{
}

template <class MEMORY_ADDR>
DWARF_Address<MEMORY_ADDR>::~DWARF_Address()
{
}

template <class MEMORY_ADDR>
MEMORY_ADDR DWARF_Address<MEMORY_ADDR>::GetValue() const
{
	return value;
}

template <class MEMORY_ADDR>
std::string DWARF_Address<MEMORY_ADDR>::to_string() const
{
	std::stringstream sstr;
	sstr << "0x" << std::hex << value << std::dec;
	return std::string(sstr.str());
}

template <class MEMORY_ADDR>
uint64_t DWARF_Address<MEMORY_ADDR>::to_int() const
{
	return (uint64_t) value;
}

template <class MEMORY_ADDR>
DWARF_Reference<MEMORY_ADDR>::DWARF_Reference(uint64_t _debug_info_offset)
	: DWARF_AttributeValue<MEMORY_ADDR>(DW_CLASS_REFERENCE)
	, debug_info_offset(_debug_info_offset)
	, dw_die(0)
{
}

template <class MEMORY_ADDR>
DWARF_Reference<MEMORY_ADDR>::~DWARF_Reference()
{
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_Reference<MEMORY_ADDR>::GetValue() const
{
	return dw_die;
}

template <class MEMORY_ADDR>
uint64_t DWARF_Reference<MEMORY_ADDR>::GetDebugInfoOffset() const
{
	return debug_info_offset;
}

template <class MEMORY_ADDR>
std::string DWARF_Reference<MEMORY_ADDR>::to_string() const
{
	std::stringstream sstr;
	sstr << debug_info_offset;
	return std::string(sstr.str());
}

template <class MEMORY_ADDR>
void DWARF_Reference<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler)
{
	dw_die = dw_handler->FindDIE(debug_info_offset);
	if(!dw_die)
	{
		dw_handler->GetLogger() << DebugWarning << "While resolving [reference attribute value -> DIE] reference, can't find DIE (Debug Information Entry) at offset " << debug_info_offset << EndDebugWarning;
	}
}

template <class MEMORY_ADDR>
DWARF_LinePtr<MEMORY_ADDR>::DWARF_LinePtr(uint64_t _debug_line_offset)
	: DWARF_AttributeValue<MEMORY_ADDR>(DW_CLASS_LINEPTR)
	, debug_line_offset(_debug_line_offset)
	, dw_stmt_prog(0)
{
}

template <class MEMORY_ADDR>
DWARF_LinePtr<MEMORY_ADDR>::~DWARF_LinePtr()
{
}

template <class MEMORY_ADDR>
const DWARF_StatementProgram<MEMORY_ADDR> *DWARF_LinePtr<MEMORY_ADDR>::GetValue() const
{
	return dw_stmt_prog;
}

template <class MEMORY_ADDR>
uint64_t DWARF_LinePtr<MEMORY_ADDR>::GetDebugLineOffset() const
{
	return debug_line_offset;
}

template <class MEMORY_ADDR>
std::string DWARF_LinePtr<MEMORY_ADDR>::to_string() const
{
	std::stringstream sstr;
	sstr << debug_line_offset;
	return std::string(sstr.str());
}

template <class MEMORY_ADDR>
void DWARF_LinePtr<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler)
{
	dw_stmt_prog = dw_handler->FindStatementProgram(debug_line_offset);
	if(!dw_stmt_prog)
	{
		dw_handler->GetLogger() << DebugWarning << "While resolving [line pointer attribute value -> statement program] reference, can't find .debug_line statement program at offset " << debug_line_offset << EndDebugWarning;
	}
}

template <class MEMORY_ADDR>
DWARF_LocListPtr<MEMORY_ADDR>::DWARF_LocListPtr(const DWARF_CompilationUnit<MEMORY_ADDR> *_dw_cu, uint64_t _debug_loc_offset)
	: DWARF_AttributeValue<MEMORY_ADDR>(DW_CLASS_LOCLISTPTR)
	, dw_cu(_dw_cu)
	, debug_loc_offset(_debug_loc_offset)
	, dw_loc_list_entry(0)
{
}

template <class MEMORY_ADDR>
DWARF_LocListPtr<MEMORY_ADDR>::~DWARF_LocListPtr()
{
}

template <class MEMORY_ADDR>
const DWARF_LocListEntry<MEMORY_ADDR> *DWARF_LocListPtr<MEMORY_ADDR>::GetValue() const
{
	return dw_loc_list_entry;
}

template <class MEMORY_ADDR>
uint64_t DWARF_LocListPtr<MEMORY_ADDR>::GetDebugLocOffset() const
{
	return debug_loc_offset;
}

template <class MEMORY_ADDR>
std::string DWARF_LocListPtr<MEMORY_ADDR>::to_string() const
{
	std::stringstream sstr;
	sstr << debug_loc_offset;
	return std::string(sstr.str());
}

template <class MEMORY_ADDR>
void DWARF_LocListPtr<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler)
{
	dw_loc_list_entry = dw_handler->FindLocListEntry(dw_cu, debug_loc_offset);
	if(!dw_loc_list_entry)
	{
		dw_handler->GetLogger() << DebugWarning << "While resolving [location pointer attribute value -> location list entry] reference, can't find location list entry in .debug_loc at offset " << debug_loc_offset << EndDebugWarning;
	}
}


template <class MEMORY_ADDR>
DWARF_MacPtr<MEMORY_ADDR>::DWARF_MacPtr(uint64_t _debug_macinfo_offset)
	: DWARF_AttributeValue<MEMORY_ADDR>(DW_CLASS_MACPTR)
	, debug_macinfo_offset(_debug_macinfo_offset)
{
}

template <class MEMORY_ADDR>
DWARF_MacPtr<MEMORY_ADDR>::~DWARF_MacPtr()
{
}

template <class MEMORY_ADDR>
const DWARF_MacInfoListEntry<MEMORY_ADDR> *DWARF_MacPtr<MEMORY_ADDR>::GetValue() const
{
	return dw_macinfo_list_entry;
}

template <class MEMORY_ADDR>
uint64_t DWARF_MacPtr<MEMORY_ADDR>::GetDebugMacInfoOffset() const
{
	return debug_macinfo_offset;
}

template <class MEMORY_ADDR>
std::string DWARF_MacPtr<MEMORY_ADDR>::to_string() const
{
	std::stringstream sstr;
	sstr << debug_macinfo_offset;
	return std::string(sstr.str());
}

template <class MEMORY_ADDR>
void DWARF_MacPtr<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler)
{
	dw_macinfo_list_entry = dw_handler->FindMacInfoListEntry(debug_macinfo_offset);
	if(!dw_macinfo_list_entry)
	{
		dw_handler->GetLogger() << DebugWarning << "While resolving [macro pointer attribute value -> mac info list entry] reference, can't find mac info list entry in .debug_macinfo at offset " << debug_macinfo_offset << EndDebugWarning;
	}
}

template <class MEMORY_ADDR>
DWARF_RangeListPtr<MEMORY_ADDR>::DWARF_RangeListPtr(const DWARF_CompilationUnit<MEMORY_ADDR> *_dw_cu, uint64_t _debug_ranges_offset)
	: DWARF_AttributeValue<MEMORY_ADDR>(DW_CLASS_RANGELISTPTR)
	, dw_cu(_dw_cu)
	, debug_ranges_offset(_debug_ranges_offset)
	, dw_range_list_entry(0)
{
}

template <class MEMORY_ADDR>
DWARF_RangeListPtr<MEMORY_ADDR>::~DWARF_RangeListPtr()
{
}

template <class MEMORY_ADDR>
const DWARF_RangeListEntry<MEMORY_ADDR> *DWARF_RangeListPtr<MEMORY_ADDR>::GetValue() const
{
	return dw_range_list_entry;
}

template <class MEMORY_ADDR>
uint64_t DWARF_RangeListPtr<MEMORY_ADDR>::GetDebugRangesOffset() const
{
	return debug_ranges_offset;
}

template <class MEMORY_ADDR>
std::string DWARF_RangeListPtr<MEMORY_ADDR>::to_string() const
{
	std::stringstream sstr;
	sstr << debug_ranges_offset;
	return std::string(sstr.str());
}

template <class MEMORY_ADDR>
void DWARF_RangeListPtr<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler)
{
	dw_range_list_entry = dw_handler->FindRangeListEntry(dw_cu, debug_ranges_offset);
	if(!dw_range_list_entry)
	{
		dw_handler->GetLogger() << DebugWarning << "While resolving [range list pointer attribute value -> range list entry] reference, can't find range list entry in .debug_ranges at offset " << debug_ranges_offset << EndDebugWarning;
	}
}

template <class MEMORY_ADDR>
DWARF_Expression<MEMORY_ADDR>::DWARF_Expression(const DWARF_CompilationUnit<MEMORY_ADDR> *_dw_cu, uint64_t _length, const uint8_t *_value)
	: DWARF_AttributeValue<MEMORY_ADDR>(DW_CLASS_EXPRESSION)
	, dw_cu(_dw_cu)
	, dw_cfp(0)
	, length(_length)
	, value(_value)
{
}

template <class MEMORY_ADDR>
DWARF_Expression<MEMORY_ADDR>::DWARF_Expression(const DWARF_CallFrameProgram<MEMORY_ADDR> *_dw_cfp, uint64_t _length, const uint8_t *_value)
	: DWARF_AttributeValue<MEMORY_ADDR>(DW_CLASS_EXPRESSION)
	, dw_cu(0)
	, dw_cfp(_dw_cfp)
	, length(_length)
	, value(_value)
{
}

template <class MEMORY_ADDR>
DWARF_Expression<MEMORY_ADDR>::DWARF_Expression(const DWARF_Expression<MEMORY_ADDR>& dw_exp)
	: DWARF_AttributeValue<MEMORY_ADDR>(DW_CLASS_EXPRESSION)
	, dw_cu(dw_exp.dw_cu)
	, dw_cfp(dw_exp.dw_cfp)
	, length(dw_exp.length)
	, value(dw_exp.value)
{
}

template <class MEMORY_ADDR>
DWARF_Expression<MEMORY_ADDR>::~DWARF_Expression()
{
}

template <class MEMORY_ADDR>
uint64_t DWARF_Expression<MEMORY_ADDR>::GetLength() const
{
	return length;
}

template <class MEMORY_ADDR>
const uint8_t *DWARF_Expression<MEMORY_ADDR>::GetValue() const
{
	return value;
}

template <class MEMORY_ADDR>
std::string DWARF_Expression<MEMORY_ADDR>::to_string() const
{
	std::stringstream sstr;
	
	if(dw_cu)
	{
		DWARF_ExpressionVM<MEMORY_ADDR> expr_vm = DWARF_ExpressionVM<MEMORY_ADDR>(dw_cu);
		return expr_vm.Disasm(sstr, this) ? sstr.str() : std::string();
	}
	
	if(dw_cfp)
	{
		DWARF_ExpressionVM<MEMORY_ADDR> expr_vm = DWARF_ExpressionVM<MEMORY_ADDR>(dw_cfp);
		return expr_vm.Disasm(sstr, this) ? sstr.str() : std::string();
	}
	
	return std::string();
}

template <class MEMORY_ADDR>
DWARF_Attribute<MEMORY_ADDR>::DWARF_Attribute(const DWARF_DIE<MEMORY_ADDR> *_dw_die, DWARF_AbbrevAttribute *_dw_abbrev_attribute, DWARF_AttributeValue<MEMORY_ADDR> *_dw_value)
	: dw_die(_dw_die)
	, dw_abbrev_attribute(_dw_abbrev_attribute)
	, dw_value(_dw_value)
{
}

template <class MEMORY_ADDR>
DWARF_Attribute<MEMORY_ADDR>::~DWARF_Attribute()
{
	if(dw_value)
	{
		delete dw_value;
	}
}

template <class MEMORY_ADDR>
const DWARF_AbbrevAttribute *DWARF_Attribute<MEMORY_ADDR>::GetAbbrevAttribute() const
{
	return dw_abbrev_attribute;
}

template <class MEMORY_ADDR>
const DWARF_AttributeValue<MEMORY_ADDR> *DWARF_Attribute<MEMORY_ADDR>::GetValue() const
{
	return dw_value;
}

template <class MEMORY_ADDR>
void DWARF_Attribute<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler)
{
	uint16_t dw_tag = (uint16_t) dw_abbrev_attribute->GetTag();
	unsigned int dw_class = dw_value->GetClass();
	
	uint64_t ref;
	
	switch(dw_class)
	{
		case DW_CLASS_LINEPTR:
			ref = ((DWARF_LinePtr<MEMORY_ADDR> *) dw_value)->GetDebugLineOffset();
			break;
		case DW_CLASS_LOCLISTPTR:
			ref = ((DWARF_LocListPtr<MEMORY_ADDR> *) dw_value)->GetDebugLocOffset();
			break;
		case DW_CLASS_MACPTR:
			ref = ((DWARF_MacPtr<MEMORY_ADDR> *) dw_value)->GetDebugMacInfoOffset();
			break;
		case DW_CLASS_RANGELISTPTR:
			ref = ((DWARF_RangeListPtr<MEMORY_ADDR> *) dw_value)->GetDebugRangesOffset();
			break;
		case DW_CLASS_REFERENCE:
			ref = ((DWARF_Reference<MEMORY_ADDR> *) dw_value)->GetDebugInfoOffset();
			break;
		default:
			return;
	}
	
	const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu = dw_die->GetCompilationUnit();
	
	switch(dw_tag)
	{
		case DW_AT_sibling:
		case DW_AT_byte_size:
		case DW_AT_bit_offset:
		case DW_AT_bit_size:
		case DW_AT_discr:
		case DW_AT_import:
		case DW_AT_common_reference:
		case DW_AT_containing_type:
		case DW_AT_default_value:
		case DW_AT_lower_bound:
		case DW_AT_upper_bound:
		case DW_AT_abstract_origin:
		case DW_AT_base_types:
		case DW_AT_count:
		case DW_AT_friend:
		case DW_AT_priority:
		case DW_AT_specification:
		case DW_AT_type:
		case DW_AT_allocated:
		case DW_AT_associated:
		case DW_AT_byte_stride:
		case DW_AT_extension:
		case DW_AT_trampoline:
		case DW_AT_small:
		case DW_AT_object_pointer:
			delete dw_value;
			dw_value = new DWARF_Reference<MEMORY_ADDR>(ref);
			break;
		case DW_AT_location:
		case DW_AT_string_length:
		case DW_AT_return_addr:
		case DW_AT_data_member_location:
		case DW_AT_frame_base:
		case DW_AT_segment:
		case DW_AT_static_link:
		case DW_AT_use_location:
		case DW_AT_vtable_elem_location:
			delete dw_value;
			dw_value = new DWARF_LocListPtr<MEMORY_ADDR>(dw_cu, ref);
			break;
		case DW_AT_stmt_list:
			delete dw_value;
			dw_value = new DWARF_LinePtr<MEMORY_ADDR>(ref);
			break;
		case DW_AT_macro_info:
			delete dw_value;
			dw_value = new DWARF_MacPtr<MEMORY_ADDR>(ref);
			break;
		case DW_AT_ranges:
			delete dw_value;
			dw_value = new DWARF_RangeListPtr<MEMORY_ADDR>(dw_cu, ref);
			break;
	}

	dw_value->Fix(dw_handler);
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_Attribute<MEMORY_ADDR>& dw_attribute)
{
	os << dw_attribute.dw_abbrev_attribute->GetName() << ": ";
	
	unsigned int dw_class = dw_attribute.dw_value->GetClass();
	switch(dw_class)
	{
		case DW_CLASS_UNSIGNED_CONSTANT:
		case DW_CLASS_SIGNED_CONSTANT:
		case DW_CLASS_UNSIGNED_LEB128_CONSTANT:
		case DW_CLASS_SIGNED_LEB128_CONSTANT:
			{
				uint64_t int_value = dw_attribute.dw_value->to_int();
				uint16_t dw_at = (uint16_t) dw_attribute.dw_abbrev_attribute->GetTag();
				
				switch(dw_at)
				{
					case DW_AT_encoding:
						os << DWARF_GetATEName(int_value);
						break;
					case DW_AT_language:
						os << DWARF_GetLANGName(int_value);
						break;
					case DW_AT_virtuality:
						os << DWARF_GetVIRTUALITYName(int_value);
						break;
					case DW_AT_visibility:
						os << DWARF_GetVISName(int_value);
						break;
					case DW_AT_accessibility:
						os << DWARF_GetACCESSName(int_value);
						break;
					case DW_AT_endianity:
						os << DWARF_GetENDName(int_value);
						break;
					case DW_AT_decimal_sign:
						os << DWARF_GetDSName(int_value);
						break;
					case DW_AT_identifier_case:
						os << DWARF_GetIDName(int_value);
						break;
					case DW_AT_calling_convention:
						os << DWARF_GetCCName(int_value);
						break;
					case DW_AT_inline:
						os << DWARF_GetINLName(int_value);
						break;
					case DW_AT_ordering:
						os << DWARF_GetORDName(int_value);
						break;
					case DW_AT_discr_list:
						os << DWARF_GetDSCName(int_value);
						break;
					default:
						os << dw_attribute.dw_value->to_string();
						break;
				}
			}
			break;
		default:
			os << dw_attribute.dw_value->to_string();
			break;
	}
	
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_Attribute<MEMORY_ADDR>::to_XML(std::ostream& os)
{
	os << "<DW_AT name=\"";
	c_string_to_XML(os, dw_abbrev_attribute->GetName());
	os << "\" class=\"";
	c_string_to_XML(os, dw_value->GetClassName());
	os << "\" value=\"";

	unsigned int dw_class = dw_value->GetClass();
	switch(dw_class)
	{
		case DW_CLASS_UNSIGNED_CONSTANT:
		case DW_CLASS_SIGNED_CONSTANT:
		case DW_CLASS_UNSIGNED_LEB128_CONSTANT:
		case DW_CLASS_SIGNED_LEB128_CONSTANT:
			{
				uint64_t int_value = dw_value->to_int();
				uint16_t dw_at = (uint16_t) dw_abbrev_attribute->GetTag();
				
				switch(dw_at)
				{
					case DW_AT_encoding:
						c_string_to_XML(os, DWARF_GetATEName(int_value));
						break;
					case DW_AT_language:
						c_string_to_XML(os, DWARF_GetLANGName(int_value));
						break;
					case DW_AT_virtuality:
						c_string_to_XML(os, DWARF_GetVIRTUALITYName(int_value));
						break;
					case DW_AT_visibility:
						c_string_to_XML(os, DWARF_GetVISName(int_value));
						break;
					case DW_AT_accessibility:
						c_string_to_XML(os, DWARF_GetACCESSName(int_value));
						break;
					case DW_AT_endianity:
						c_string_to_XML(os, DWARF_GetENDName(int_value));
						break;
					case DW_AT_decimal_sign:
						c_string_to_XML(os, DWARF_GetDSName(int_value));
						break;
					case DW_AT_identifier_case:
						c_string_to_XML(os, DWARF_GetIDName(int_value));
						break;
					case DW_AT_calling_convention:
						c_string_to_XML(os, DWARF_GetCCName(int_value));
						break;
					case DW_AT_inline:
						c_string_to_XML(os, DWARF_GetINLName(int_value));
						break;
					case DW_AT_ordering:
						c_string_to_XML(os, DWARF_GetORDName(int_value));
						break;
					case DW_AT_discr_list:
						c_string_to_XML(os, DWARF_GetDSCName(int_value));
						break;
					default:
						c_string_to_XML(os, dw_value->to_string().c_str());
						break;
				}
			}
			break;
		default:
			c_string_to_XML(os, dw_value->to_string().c_str());
			break;
	}
	os << "\"/>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_Attribute<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	os << "<tr>" << std::endl;
	os << "<td>" << std::endl;
	c_string_to_HTML(os, dw_abbrev_attribute->GetName());
	os << "</td>" << std::endl;
	os << "<td>" << std::endl;
	c_string_to_HTML(os, dw_value->GetClassName());
	os << "</td>" << std::endl;
	os << "<td>" << std::endl;
	unsigned int dw_class = dw_value->GetClass();
	switch(dw_class)
	{
		case DW_CLASS_REFERENCE:
			{
				DWARF_Reference<MEMORY_ADDR> *dw_ref = (DWARF_Reference<MEMORY_ADDR> *) dw_value;
				const DWARF_DIE<MEMORY_ADDR> *dw_ref_die = dw_ref->GetValue();
				os << "<a href=\"../../" << dw_ref_die->GetHREF() << "\">die-" << dw_ref_die->GetId() << "</a>" << std::endl;
			}
			break;
		case DW_CLASS_MACPTR:
			{
				DWARF_MacPtr<MEMORY_ADDR> *dw_macptr = (DWARF_MacPtr<MEMORY_ADDR> *) dw_value;
				const DWARF_MacInfoListEntry<MEMORY_ADDR> *dw_macinfo_list_entry = dw_macptr->GetValue();
				os << "<a href=\"../../" << dw_macinfo_list_entry->GetHREF() << "\">mac-" << dw_macinfo_list_entry->GetId() << "</a>" << std::endl;
			}
			break;
		case DW_CLASS_LOCLISTPTR:
			{
				DWARF_LocListPtr<MEMORY_ADDR> *dw_loclistptr = (DWARF_LocListPtr<MEMORY_ADDR> *) dw_value;
				const DWARF_LocListEntry<MEMORY_ADDR> *dw_loc_list_entry = dw_loclistptr->GetValue();
				os << "<a href=\"../../" << dw_loc_list_entry->GetHREF() << "\">loc-" << dw_loc_list_entry->GetId() << "</a>" << std::endl;
			}
			break;
		case DW_CLASS_LINEPTR:
			{
				DWARF_LinePtr<MEMORY_ADDR> *dw_lineptr = (DWARF_LinePtr<MEMORY_ADDR> *) dw_value;
				const DWARF_StatementProgram<MEMORY_ADDR> *dw_stmt_prog = dw_lineptr->GetValue();
				os << "<a href=\"../../" << dw_stmt_prog->GetHREF() << "\">stmt-prog-" << dw_stmt_prog->GetId() << "</a>" << std::endl;
			}
			break;
		case DW_CLASS_UNSIGNED_CONSTANT:
		case DW_CLASS_SIGNED_CONSTANT:
		case DW_CLASS_UNSIGNED_LEB128_CONSTANT:
		case DW_CLASS_SIGNED_LEB128_CONSTANT:
			{
				uint64_t int_value = dw_value->to_int();
				uint16_t dw_at = (uint16_t) dw_abbrev_attribute->GetTag();
				
				switch(dw_at)
				{
					case DW_AT_encoding:
						c_string_to_HTML(os, DWARF_GetATEName(int_value));
						break;
					case DW_AT_language:
						c_string_to_HTML(os, DWARF_GetLANGName(int_value));
						break;
					case DW_AT_virtuality:
						c_string_to_HTML(os, DWARF_GetVIRTUALITYName(int_value));
						break;
					case DW_AT_visibility:
						c_string_to_HTML(os, DWARF_GetVISName(int_value));
						break;
					case DW_AT_accessibility:
						c_string_to_HTML(os, DWARF_GetACCESSName(int_value));
						break;
					case DW_AT_endianity:
						c_string_to_HTML(os, DWARF_GetENDName(int_value));
						break;
					case DW_AT_decimal_sign:
						c_string_to_HTML(os, DWARF_GetDSName(int_value));
						break;
					case DW_AT_identifier_case:
						c_string_to_HTML(os, DWARF_GetIDName(int_value));
						break;
					case DW_AT_calling_convention:
						c_string_to_HTML(os, DWARF_GetCCName(int_value));
						break;
					case DW_AT_inline:
						c_string_to_HTML(os, DWARF_GetINLName(int_value));
						break;
					case DW_AT_ordering:
						c_string_to_HTML(os, DWARF_GetORDName(int_value));
						break;
					case DW_AT_discr_list:
						c_string_to_HTML(os, DWARF_GetDSCName(int_value));
						break;
					default:
						c_string_to_HTML(os, dw_value->to_string().c_str());
						break;
				}
			}
			break;
		default:
			c_string_to_HTML(os, dw_value->to_string().c_str());
			break;
	}
	os << "</tr>" << std::endl;

	return os;
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim
