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

#include <unisim/util/debug/dwarf/encoding.hh>
#include <string.h>
#include <sstream>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

const char *DWARF_GetTagName(uint16_t dw_tag)
{
	static char buf[23];
	
	switch(dw_tag)
	{
		case 0: return "DW_TAG_NULL";
		case DW_TAG_array_type: return "DW_TAG_array_type";
		case DW_TAG_class_type: return "DW_TAG_class_type";
		case DW_TAG_entry_point: return "DW_TAG_entry_point";
		case DW_TAG_enumeration_type: return "DW_TAG_enumeration_type";
		case DW_TAG_formal_parameter: return "DW_TAG_formal_parameter";
		case DW_TAG_imported_declaration: return "DW_TAG_imported_declaration";
		case DW_TAG_label: return "DW_TAG_label";
		case DW_TAG_lexical_block: return "DW_TAG_lexical_block";
		case DW_TAG_member: return "DW_TAG_member";
		case DW_TAG_pointer_type: return "DW_TAG_pointer_type";
		case DW_TAG_reference_type: return "DW_TAG_reference_type";
		case DW_TAG_compile_unit: return "DW_TAG_compile_unit";
		case DW_TAG_string_type: return "DW_TAG_string_type";
		case DW_TAG_structure_type: return "DW_TAG_structure_type";
		case DW_TAG_subroutine_type: return "DW_TAG_subroutine_type";
		case DW_TAG_typedef: return "DW_TAG_typedef";
		case DW_TAG_union_type: return "DW_TAG_union_type";
		case DW_TAG_unspecified_parameters: return "DW_TAG_unspecified_parameters";
		case DW_TAG_variant: return "DW_TAG_variant";
		case DW_TAG_common_block: return "DW_TAG_common_block";
		case DW_TAG_common_inclusion: return "DW_TAG_common_inclusion";
		case DW_TAG_inheritance: return "DW_TAG_inheritance";
		case DW_TAG_inlined_subroutine: return "DW_TAG_inlined_subroutine";
		case DW_TAG_module: return "DW_TAG_module";
		case DW_TAG_ptr_to_member_type: return "DW_TAG_ptr_to_member_type";
		case DW_TAG_set_type: return "DW_TAG_set_type";
		case DW_TAG_subrange_type: return "DW_TAG_subrange_type";
		case DW_TAG_with_stmt: return "DW_TAG_with_stmt";
		case DW_TAG_access_declaration: return "DW_TAG_access_declaration";
		case DW_TAG_base_type: return "DW_TAG_base_type";
		case DW_TAG_catch_block: return "DW_TAG_catch_block";
		case DW_TAG_const_type: return "DW_TAG_const_type";
		case DW_TAG_constant: return "DW_TAG_constant";
		case DW_TAG_enumerator: return "DW_TAG_enumerator";
		case DW_TAG_file_type: return "DW_TAG_file_type";
		case DW_TAG_friend: return "DW_TAG_friend";
		case DW_TAG_namelist: return "DW_TAG_namelist";
		case DW_TAG_namelist_item: return "DW_TAG_namelist_item";
		case DW_TAG_packed_type: return "DW_TAG_packed_type";
		case DW_TAG_subprogram: return "DW_TAG_subprogram";
		case DW_TAG_template_type_param: return "DW_TAG_template_type_param";
		case DW_TAG_template_value_param: return "DW_TAG_template_value_param";
		case DW_TAG_thrown_type: return "DW_TAG_thrown_type";
		case DW_TAG_try_block: return "DW_TAG_try_block";
		case DW_TAG_variant_part: return "DW_TAG_variant_part";
		case DW_TAG_variable: return "DW_TAG_variable";
		case DW_TAG_volatile_type: return "DW_TAG_volatile_type";
		case DW_TAG_dwarf_procedure: return "DW_TAG_dwarf_procedure";
		case DW_TAG_restrict_type: return "DW_TAG_restrict_type";
		case DW_TAG_interface_type: return "DW_TAG_interface_type";
		case DW_TAG_namespace: return "DW_TAG_namespace";
		case DW_TAG_imported_module: return "DW_TAG_imported_module";
		case DW_TAG_unspecified_type: return "DW_TAG_unspecified_type";
		case DW_TAG_partial_unit: return "DW_TAG_partial_unit";
		case DW_TAG_imported_unit: return "DW_TAG_imported_unit";
		case DW_TAG_condition: return "DW_TAG_condition";
		case DW_TAG_shared_type: return "DW_TAG_shared_type";
		case DW_TAG_type_unit: return "DW_TAG_type_unit";
		case DW_TAG_rvalue_reference_type: return "DW_TAG_rvalue_reference_type";
		case DW_TAG_template_alias: return "DW_TAG_template_alias";
	}
	std::stringstream sstr;
	sstr << "DW_TAG_" << (((dw_tag >= DW_TAG_lo_user) && (dw_tag <= DW_TAG_hi_user)) ? "user" : "unknown") << "_0x" << std::hex << dw_tag << std::dec;
	strcpy(buf, sstr.str().c_str());
	return (const char *) buf;
}

const char *DWARF_GetATName(uint16_t dw_at)
{
	static char buf[21];
	switch(dw_at)
	{
		case DW_AT_sibling: return "DW_AT_sibling";
		case DW_AT_location: return "DW_AT_location";
		case DW_AT_name: return "DW_AT_name";
		case DW_AT_ordering: return "DW_AT_ordering";
		case DW_AT_byte_size: return "DW_AT_byte_size";
		case DW_AT_bit_offset: return "DW_AT_bit_offset";
		case DW_AT_bit_size: return "DW_AT_bit_size";
		case DW_AT_stmt_list: return "DW_AT_stmt_list";
		case DW_AT_low_pc: return "DW_AT_low_pc";
		case DW_AT_high_pc: return "DW_AT_high_pc";
		case DW_AT_language: return "DW_AT_language";
		case DW_AT_discr: return "DW_AT_discr";
		case DW_AT_discr_value: return "DW_AT_discr_value";
		case DW_AT_visibility: return "DW_AT_visibility";
		case DW_AT_import: return "DW_AT_import";
		case DW_AT_string_length: return "DW_AT_string_length";
		case DW_AT_common_reference: return "DW_AT_common_reference";
		case DW_AT_comp_dir: return "DW_AT_comp_dir";
		case DW_AT_const_value: return "DW_AT_const_value";
		case DW_AT_containing_type: return "DW_AT_containing_type";
		case DW_AT_default_value: return "DW_AT_default_value";
		case DW_AT_inline: return "DW_AT_inline";
		case DW_AT_is_optional: return "DW_AT_is_optional";
		case DW_AT_lower_bound: return "DW_AT_lower_bound";
		case DW_AT_producer: return "DW_AT_producer";
		case DW_AT_prototyped: return "DW_AT_prototyped";
		case DW_AT_return_addr: return "DW_AT_return_addr";
		case DW_AT_start_scope: return "DW_AT_start_scope";
		case DW_AT_stride_size: return "DW_AT_stride_size";
		case DW_AT_upper_bound: return "DW_AT_upper_bound";
		case DW_AT_abstract_origin: return "DW_AT_abstract_origin";
		case DW_AT_accessibility: return "DW_AT_accessibility";
		case DW_AT_address_class: return "DW_AT_address_class";
		case DW_AT_artificial: return "DW_AT_artificial";
		case DW_AT_base_types: return "DW_AT_base_types";
		case DW_AT_calling_convention: return "DW_AT_calling_convention";
		case DW_AT_count: return "DW_AT_count";
		case DW_AT_data_member_location: return "DW_AT_data_member_location";
		case DW_AT_decl_column: return "DW_AT_decl_column";
		case DW_AT_decl_file: return "DW_AT_decl_file";
		case DW_AT_decl_line: return "DW_AT_decl_line";
		case DW_AT_declaration: return "DW_AT_declaration";
		case DW_AT_discr_list: return "DW_AT_discr_list";
		case DW_AT_encoding: return "DW_AT_encoding";
		case DW_AT_external: return "DW_AT_external";
		case DW_AT_frame_base: return "DW_AT_frame_base";
		case DW_AT_friend: return "DW_AT_friend";
		case DW_AT_identifier_case: return "DW_AT_identifier_case";
		case DW_AT_macro_info: return "DW_AT_macro_info";
		case DW_AT_namelist_item: return "DW_AT_namelist_item";
		case DW_AT_priority: return "DW_AT_priority";
		case DW_AT_segment: return "DW_AT_segment";
		case DW_AT_specification: return "DW_AT_specification";
		case DW_AT_static_link: return "DW_AT_static_link";
		case DW_AT_type: return "DW_AT_type";
		case DW_AT_use_location: return "DW_AT_use_location";
		case DW_AT_variable_parameter: return "DW_AT_variable_parameter";
		case DW_AT_virtuality: return "DW_AT_virtuality";
		case DW_AT_vtable_elem_location: return "DW_AT_vtable_elem_location";
		case DW_AT_allocated: return "DW_AT_allocated";
		case DW_AT_associated: return "DW_AT_associated";
		case DW_AT_data_location: return "DW_AT_data_location";
		case DW_AT_byte_stride: return "DW_AT_byte_stride";
		case DW_AT_entry_pc: return "DW_AT_entry_pc";
		case DW_AT_use_UTF8: return "DW_AT_use_UTF8";
		case DW_AT_extension: return "DW_AT_extension";
		case DW_AT_ranges: return "DW_AT_ranges";
		case DW_AT_trampoline: return "DW_AT_trampoline";
		case DW_AT_call_column: return "DW_AT_call_column";
		case DW_AT_call_file: return "DW_AT_call_file";
		case DW_AT_call_line: return "DW_AT_call_line";
		case DW_AT_description: return "DW_AT_description";
		case DW_AT_binary_scale: return "DW_AT_binary_scale";
		case DW_AT_decimal_scale: return "DW_AT_decimal_scale";
		case DW_AT_small: return "DW_AT_small";
		case DW_AT_decimal_sign: return "DW_AT_decimal_sign";
		case DW_AT_digit_count: return "DW_AT_digit_count";
		case DW_AT_picture_string: return "DW_AT_picture_string";
		case DW_AT_mutable: return "DW_AT_mutable";
		case DW_AT_threads_scaled: return "DW_AT_threads_scaled";
		case DW_AT_explicit: return "DW_AT_explicit";
		case DW_AT_object_pointer: return "DW_AT_object_pointer";
		case DW_AT_endianity: return "DW_AT_endianity";
		case DW_AT_elemental: return "DW_AT_elemental";
		case DW_AT_pure: return "DW_AT_pure";
		case DW_AT_recursive: return "DW_AT_recursive";
		case DW_AT_signature: return "DW_AT_signature";
		case DW_AT_main_subprogram: return "DW_AT_main_subprogram";
		case DW_AT_data_bit_offset: return "DW_AT_data_bit_offset";
		case DW_AT_const_expr: return "DW_AT_const_expr";
		case DW_AT_enum_class: return "DW_AT_enum_class";
		case DW_AT_linkage_name: return "DW_AT_linkage_name";
	}
	std::stringstream sstr;
	sstr << "DW_AT_" << (((dw_at >= DW_AT_lo_user) && (dw_at <= DW_AT_hi_user)) ? "user" : "unknown") << "_0x" << std::hex << dw_at << std::dec;
	strcpy(buf, sstr.str().c_str());
	return (const char *) buf;
}

const char *DWARF_GetFORMName(uint16_t dw_form)
{
	static char buf[23];
	switch(dw_form)
	{
		case DW_FORM_addr: return "DW_FORM_addr";
		case DW_FORM_block2: return "DW_FORM_block2";
		case DW_FORM_block4: return "DW_FORM_block4";
		case DW_FORM_data2: return "DW_FORM_data2";
		case DW_FORM_data4: return "DW_FORM_data4";
		case DW_FORM_data8: return "DW_FORM_data8";
		case DW_FORM_string: return "DW_FORM_string";
		case DW_FORM_block: return "DW_FORM_block";
		case DW_FORM_block1: return "DW_FORM_block1";
		case DW_FORM_data1: return "DW_FORM_data1";
		case DW_FORM_flag: return "DW_FORM_flag";
		case DW_FORM_sdata: return "DW_FORM_sdata";
		case DW_FORM_strp: return "DW_FORM_strp";
		case DW_FORM_udata: return "DW_FORM_udata";
		case DW_FORM_ref_addr: return "DW_FORM_ref_addr";
		case DW_FORM_ref1: return "DW_FORM_ref1";
		case DW_FORM_ref2: return "DW_FORM_ref2";
		case DW_FORM_ref4: return "DW_FORM_ref4";
		case DW_FORM_ref8: return "DW_FORM_ref8";
		case DW_FORM_ref_udata: return "DW_FORM_ref_udata";
		case DW_FORM_indirect: return "DW_FORM_indirect";
		case DW_FORM_sec_offset: return "DW_FORM_sec_offset";
	}
	std::stringstream sstr;
	sstr << "DW_FORM_unknown_0x" << std::hex << dw_form << std::dec;
	strcpy(buf, sstr.str().c_str());
	return (const char *) buf;
}

const char *DWARF_GetATEName(uint8_t dw_ate)
{
	static char buf[22];
	switch(dw_ate)
	{
		case DW_ATE_address: return "DW_ATE_address";
		case DW_ATE_boolean: return "DW_ATE_boolean";
		case DW_ATE_complex_float: return "DW_ATE_complex_float";
		case DW_ATE_float: return "DW_ATE_float";
		case DW_ATE_signed: return "DW_ATE_signed";
		case DW_ATE_signed_char: return "DW_ATE_signed_char";
		case DW_ATE_unsigned: return "DW_ATE_unsigned";
		case DW_ATE_unsigned_char: return "DW_ATE_unsigned_char";
		case DW_ATE_imaginary_float: return "DW_ATE_imaginary_float";
		case DW_ATE_packed_decimal: return "DW_ATE_packed_decimal";
		case DW_ATE_numeric_string: return "DW_ATE_numeric_string";
		case DW_ATE_edited: return "DW_ATE_edited";
		case DW_ATE_signed_fixed: return "DW_ATE_signed_fixed";
		case DW_ATE_unsigned_fixed: return "DW_ATE_unsigned_fixed";
		case DW_ATE_decimal_float: return "DW_ATE_decimal_float";
	}
	std::stringstream sstr;
	sstr << "DW_ATE_unknown_0x" << std::hex << (unsigned int) dw_ate << std::dec;
	strcpy(buf, sstr.str().c_str());
	return (const char *) buf;
}

const char *DWARF_GetLANGName(uint16_t dw_lang)
{
	static char buf[23];
	switch(dw_lang)
	{
		case DW_LANG_C89: return "DW_LANG_C89";
		case DW_LANG_C: return "DW_LANG_C";
		case DW_LANG_Ada83: return "DW_LANG_Ada83";
		case DW_LANG_C_plus_plus: return "DW_LANG_C_plus_plus";
		case DW_LANG_Cobol74: return "DW_LANG_Cobol74";
		case DW_LANG_Cobol85: return "DW_LANG_Cobol85";
		case DW_LANG_Fortran77: return "DW_LANG_Fortran77";
		case DW_LANG_Fortran90: return "DW_LANG_Fortran90";
		case DW_LANG_Pascal83: return "DW_LANG_Pascal83";
		case DW_LANG_Modula2: return "DW_LANG_Modula2";
		case DW_LANG_Java: return "DW_LANG_Java";
		case DW_LANG_C99: return "DW_LANG_C99";
		case DW_LANG_Ada95: return "DW_LANG_Ada95";
		case DW_LANG_Fortran95: return "DW_LANG_Fortran95";
		case DW_LANG_PLI: return "DW_LANG_PLI";
		case DW_LANG_ObjC: return "DW_LANG_ObjC";
		case DW_LANG_ObjC_plus_plus: return "DW_LANG_ObjC_plus_plus";
		case DW_LANG_UPC: return "DW_LANG_UPC";
		case DW_LANG_D: return "DW_LANG_D";
		case DW_LANG_python: return "DW_LANG_python";
		case DW_LANG_Mips_Assembler: return "DW_LANG_Mips_Assembler";
		case DW_LANG_Upc: return "DW_LANG_Upc";
	}
	std::stringstream sstr;
	sstr << "DW_LANG_unknown_0x" << std::hex << (unsigned int) dw_lang << std::dec;
	strcpy(buf, sstr.str().c_str());
	return (const char *) buf;
}

const char *DWARF_GetFriendlyLANGName(uint16_t dw_lang)
{
	switch(dw_lang)
	{
		case DW_LANG_C89: return "ISO C:1989";
		case DW_LANG_C: return "Non-standardized C, such as K&R";
		case DW_LANG_Ada83: return "ISO Ada:1983";
		case DW_LANG_C_plus_plus: return "ISO C++:1998";
		case DW_LANG_Cobol74: return "ISO Cobol:1974";
		case DW_LANG_Cobol85: return "ISO Cobol:1985";
		case DW_LANG_Fortran77: return "ISO FORTRAN 77";
		case DW_LANG_Fortran90: return "ISO Fortran 90";
		case DW_LANG_Pascal83: return "DW_LANG_Pascal83";
		case DW_LANG_Modula2: return "ISO Modula-2:1996";
		case DW_LANG_Java: return "Java";
		case DW_LANG_C99: return "ISO C:1999";
		case DW_LANG_Ada95: return "ISO Ada:1995";
		case DW_LANG_Fortran95: return "ISO Fortran 95";
		case DW_LANG_PLI: return "ANSI PL/I:1976";
		case DW_LANG_ObjC: return "Objective-C";
		case DW_LANG_ObjC_plus_plus: return "Objective-C++";
		case DW_LANG_UPC: return "Unified Parallel C";
		case DW_LANG_D: return "D";
		case DW_LANG_python: return "DW_LANG_python";
		case DW_LANG_Mips_Assembler: return "MIPS assembler";
		case DW_LANG_Upc: return "Unified Parallel C";
	}
	return "an unknown programming language";
}

const char *DWARF_GetVIRTUALITYName(uint8_t dw_virtuality)
{
	static char buf[27];
	switch(dw_virtuality)
	{
		case DW_VIRTUALITY_none: return "DW_VIRTUALITY_none";
		case DW_VIRTUALITY_virtual: return "DW_VIRTUALITY_virtual";
		case DW_VIRTUALITY_pure_virtual: return "DW_VIRTUALITY_pure_virtual";
	}
	std::stringstream sstr;
	sstr << "DW_VIRTUALITY_unknown_0x" << std::hex << (unsigned int) dw_virtuality << std::dec;
	strcpy(buf, sstr.str().c_str());
	return (const char *) buf;
}

const char *DWARF_GetVISName(uint8_t dw_vis)
{
	static char buf[20];
	switch(dw_vis)
	{
		case DW_VIS_local: return "DW_VIS_local";
		case DW_VIS_exported: return "DW_VIS_exported";
		case DW_VIS_qualified: return "DW_VIS_qualified";
	}
	std::stringstream sstr;
	sstr << "DW_VIS_unknown_0x" << std::hex << (unsigned int) dw_vis << std::dec;
	strcpy(buf, sstr.str().c_str());
	return (const char *) buf;
}

const char *DWARF_GetACCESSName(uint8_t dw_access)
{
	static char buf[23];
	switch(dw_access)
	{
		case DW_ACCESS_public: return "DW_ACCESS_public";
		case DW_ACCESS_protected: return "DW_ACCESS_protected";
		case DW_ACCESS_private: return "DW_ACCESS_private";
	}
	std::stringstream sstr;
	sstr << "DW_ACCESS_unknown_0x" << std::hex << (unsigned int) dw_access << std::dec;
	strcpy(buf, sstr.str().c_str());
	return (const char *) buf;
}

const char *DWARF_GetENDName(uint8_t dw_end)
{
	static char buf[20];
	switch(dw_end)
	{
		case DW_END_default: return "DW_END_default";
		case DW_END_big: return "DW_END_big";
		case DW_END_little: return "DW_END_little";
	}
	std::stringstream sstr;
	sstr << "DW_END_unknown_0x" << std::hex << (unsigned int) dw_end << std::dec;
	strcpy(buf, sstr.str().c_str());
	return (const char *) buf;
}

const char *DWARF_GetDSName(uint8_t dw_ds)
{
	static char buf[19];
	switch(dw_ds)
	{
		case DW_DS_unsigned: return "DW_DS_unsigned";
		case DW_DS_leading_overpunch: return "DW_DS_leading_overpunch";
		case DW_DS_trailing_overpunch: return "DW_DS_trailing_overpunch";
		case DW_DS_leading_separate: return "DW_DS_leading_separate";
		case DW_DS_trailing_separate: return "DW_DS_trailing_separate";
	}
	std::stringstream sstr;
	sstr << "DW_DS_unknown_0x" << std::hex << (unsigned int) dw_ds << std::dec;
	strcpy(buf, sstr.str().c_str());
	return (const char *) buf;
}

const char *DWARF_GetIDName(uint8_t dw_id)
{
	static char buf[19];
	switch(dw_id)
	{
		case DW_ID_case_sensitive: return "DW_ID_case_sensitive";
		case DW_ID_up_case: return "DW_ID_up_case";
		case DW_ID_down_case: return "DW_ID_down_case";
		case DW_ID_case_insensitive: return "DW_ID_case_insensitive";
	}
	std::stringstream sstr;
	sstr << "DW_ID_unknown_0x" << std::hex << (unsigned int) dw_id << std::dec;
	strcpy(buf, sstr.str().c_str());
	return (const char *) buf;
}

const char *DWARF_GetCCName(uint8_t dw_cc)
{
	static char buf[19];
	switch(dw_cc)
	{
		case DW_CC_normal: return "DW_CC_normal";
		case DW_CC_program: return "DW_CC_program";
		case DW_CC_nocall: return "DW_CC_nocall";
	}
	std::stringstream sstr;
	sstr << "DW_CC_unknown_0x" << std::hex << (unsigned int) dw_cc << std::dec;
	strcpy(buf, sstr.str().c_str());
	return (const char *) buf;
}

const char *DWARF_GetINLName(uint8_t dw_inl)
{
	static char buf[20];
	switch(dw_inl)
	{
		case DW_INL_not_inlined: return "DW_INL_not_inlined";
		case DW_INL_inlined: return "DW_INL_inlined";
		case DW_INL_declared_not_inlined: return "DW_INL_declared_not_inlined";
		case DW_INL_declared_inlined: return "DW_INL_declared_inlined";
	}
	std::stringstream sstr;
	sstr << "DW_INL_unknown_0x" << std::hex << (unsigned int) dw_inl << std::dec;
	strcpy(buf, sstr.str().c_str());
	return (const char *) buf;
}

const char *DWARF_GetORDName(uint8_t dw_ord)
{
	static char buf[20];
	switch(dw_ord)
	{
		case DW_ORD_row_major: return "DW_ORD_row_major";
		case DW_ORD_col_major: return "DW_ORD_col_major";
	}
	std::stringstream sstr;
	sstr << "DW_ORD_unknown_0x" << std::hex << (unsigned int) dw_ord << std::dec;
	strcpy(buf, sstr.str().c_str());
	return (const char *) buf;
}

const char *DWARF_GetDSCName(uint8_t dw_dsc)
{
	static char buf[20];
	switch(dw_dsc)
	{
		case DW_DSC_label: return "DW_DSC_label";
		case DW_DSC_range: return "DW_DSC_range";
	}
	std::stringstream sstr;
	sstr << "DW_DSC_unknown_0x" << std::hex << (unsigned int) dw_dsc << std::dec;
	strcpy(buf, sstr.str().c_str());
	return (const char *) buf;
}

const char *DWARF_GetCHILDRENName(uint8_t dw_children)
{
	static char buf[25];
	switch(dw_children)
	{
		case DW_CHILDREN_no: return "DW_CHILDREN_no";
		case DW_CHILDREN_yes: return "DW_CHILDREN_yes";
	}
	std::stringstream sstr;
	sstr << "DW_CHILDREN_unknown_0x" << std::hex << (unsigned int) dw_children << std::dec;
	strcpy(buf, sstr.str().c_str());
	return (const char *) buf;
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim
