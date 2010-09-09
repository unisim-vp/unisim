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
#include <string.h>
#include <sstream>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

const char *DWARF_GetCLASSName(unsigned int dw_class)
{
	switch(dw_class)
	{
		case DW_CLASS_ADDRESS: return "address";
		case DW_CLASS_BLOCK: return "block";
		case DW_CLASS_EXPRESSION: return "expression";
		case DW_CLASS_UNSIGNED_CONSTANT:
		case DW_CLASS_UNSIGNED_LEB128_CONSTANT: return "unsigned constant";
		case DW_CLASS_SIGNED_CONSTANT:
		case DW_CLASS_SIGNED_LEB128_CONSTANT:  return "signed constant";
		case DW_CLASS_FLAG: return "flag";
		case DW_CLASS_LINEPTR: return "lineptr";
		case DW_CLASS_LOCLISTPTR: return "loclistptr";
		case DW_CLASS_MACPTR: return "macptr";
		case DW_CLASS_RANGELISTPTR: return "rangelistptr";
		case DW_CLASS_REFERENCE: return "reference";
		case DW_CLASS_STRING: return "string";
	}
	return "";
}

// const char *DWARF_GetATEName(uint8_t dw_ate)
// {
// 	static char buf[9];
// 	switch(dw_ate)
// 	{
// 		case DW_ATE_address: return "DW_ATE_address";
// 		case DW_ATE_boolean: return "DW_ATE_boolean";
// 		case DW_ATE_complex_float: return "DW_ATE_complex_float";
// 		case DW_ATE_float: return "DW_ATE_float";
// 		case DW_ATE_signed: return "DW_ATE_signed";
// 		case DW_ATE_signed_char: return "DW_ATE_signed_char";
// 		case DW_ATE_unsigned: return "DW_ATE_unsigned";
// 		case DW_ATE_unsigned_char: return "DW_ATE_unsigned_char";
// 		case DW_ATE_imaginary_float: return "DW_ATE_imaginary_float";
// 		case DW_ATE_packed_decimal: return "DW_ATE_packed_decimal";
// 		case DW_ATE_numeric_string: return "DW_ATE_numeric_string";
// 		case DW_ATE_edited: return "DW_ATE_edited";
// 		case DW_ATE_signed_fixed: return "DW_ATE_signed_fixed";
// 		case DW_ATE_unsigned_fixed: return "DW_ATE_unsigned_fixed";
// 		case DW_ATE_decimal_float: return "DW_ATE_decimal_float";
// 	}
// 	std::stringstream sstr;
// 	sstr << "0x" << std::hex << (unsigned int) dw_ate << std::dec << " (?)";
// 	strncpy(buf, sstr.str().c_str(), sizeof(buf));
// 	buf[sizeof(buf) - 1] = 0;
// 	return (const char *) buf;
// }
// 
// const char *DWARF_GetLANGName(uint16_t dw_lang)
// {
// 	static char buf[14];
// 	switch(dw_lang)
// 	{
// 		case DW_LANG_C89: return "DW_LANG_C89";
// 		case DW_LANG_C: return "DW_LANG_C";
// 		case DW_LANG_Ada83: return "DW_LANG_Ada83";
// 		case DW_LANG_C_plus_plus: return "DW_LANG_C_plus_plus";
// 		case DW_LANG_Cobol74: return "DW_LANG_Cobol74";
// 		case DW_LANG_Cobol85: return "DW_LANG_Cobol85";
// 		case DW_LANG_Fortran77: return "DW_LANG_Fortran77";
// 		case DW_LANG_Fortran90: return "DW_LANG_Fortran90";
// 		case DW_LANG_Pascal83: return "DW_LANG_Pascal83";
// 		case DW_LANG_Modula2: return "DW_LANG_Modula2";
// 		case DW_LANG_Java: return "DW_LANG_Java";
// 		case DW_LANG_C99: return "DW_LANG_C99";
// 		case DW_LANG_Ada95: return "DW_LANG_Ada95";
// 		case DW_LANG_Fortran95: return "DW_LANG_Fortran95";
// 		case DW_LANG_PLI: return "DW_LANG_PLI";
// 		case DW_LANG_ObjC: return "DW_LANG_ObjC";
// 		case DW_LANG_ObjC_plus_plus: return "DW_LANG_ObjC_plus_plus";
// 		case DW_LANG_UPC: return "DW_LANG_UPC";
// 		case DW_LANG_D: return "DW_LANG_D";
// 		case DW_LANG_Mips_Assembler: return "DW_LANG_Mips_Assembler";
// 		case DW_LANG_Upc: return "DW_LANG_Upc";
// 	}
// 	std::stringstream sstr;
// 	sstr << "0x" << std::hex << (unsigned int) dw_lang << std::dec << " (?)";
// 	strncpy(buf, sstr.str().c_str(), sizeof(buf));
// 	buf[sizeof(buf) - 1] = 0;
// 	return (const char *) buf;
// }
// 
// const char *DWARF_GetVIRTUALITYName(uint8_t dw_virtuality)
// {
// 	static char buf[9];
// 	switch(dw_virtuality)
// 	{
// 		case DW_VIRTUALITY_none: return "DW_VIRTUALITY_none";
// 		case DW_VIRTUALITY_virtual: return "DW_VIRTUALITY_virtual";
// 		case DW_VIRTUALITY_pure_virtual: return "DW_VIRTUALITY_pure_virtual";
// 	}
// 	std::stringstream sstr;
// 	sstr << "0x" << std::hex << (unsigned int) dw_virtuality << std::dec << " (?)";
// 	strncpy(buf, sstr.str().c_str(), sizeof(buf));
// 	buf[sizeof(buf) - 1] = 0;
// 	return (const char *) buf;
// }
// 
// const char *DWARF_GetVISName(uint8_t dw_vis)
// {
// 	static char buf[9];
// 	switch(dw_vis)
// 	{
// 		case DW_VIS_local: return "DW_VIS_local";
// 		case DW_VIS_exported: return "DW_VIS_exported";
// 		case DW_VIS_qualified: return "DW_VIS_qualified";
// 	}
// 	std::stringstream sstr;
// 	sstr << "0x" << std::hex << (unsigned int) dw_vis << std::dec << " (?)";
// 	strncpy(buf, sstr.str().c_str(), sizeof(buf));
// 	buf[sizeof(buf) - 1] = 0;
// 	return (const char *) buf;
// }
// 
// const char *DWARF_GetACCESSName(uint8_t dw_access)
// {
// 	static char buf[9];
// 	switch(dw_access)
// 	{
// 		case DW_ACCESS_public: return "DW_ACCESS_public";
// 		case DW_ACCESS_protected: return "DW_ACCESS_protected";
// 		case DW_ACCESS_private: return "DW_ACCESS_private";
// 	}
// 	std::stringstream sstr;
// 	sstr << "0x" << std::hex << (unsigned int) dw_access << std::dec << " (?)";
// 	strncpy(buf, sstr.str().c_str(), sizeof(buf));
// 	buf[sizeof(buf) - 1] = 0;
// 	return (const char *) buf;
// }
// 
// const char *DWARF_GetENDName(uint8_t dw_end)
// {
// 	static char buf[9];
// 	switch(dw_end)
// 	{
// 		case DW_END_default: return "DW_END_default";
// 		case DW_END_big: return "DW_END_big";
// 		case DW_END_little: return "DW_END_little";
// 	}
// 	std::stringstream sstr;
// 	sstr << "0x" << std::hex << (unsigned int) dw_end << std::dec << " (?)";
// 	strncpy(buf, sstr.str().c_str(), sizeof(buf));
// 	buf[sizeof(buf) - 1] = 0;
// 	return (const char *) buf;
// }
// 
// const char *DWARF_GetDSName(uint8_t dw_ds)
// {
// 	static char buf[9];
// 	switch(dw_ds)
// 	{
// 		case DW_DS_unsigned: return "DW_DS_unsigned";
// 		case DW_DS_leading_overpunch: return "DW_DS_leading_overpunch";
// 		case DW_DS_trailing_overpunch: return "DW_DS_trailing_overpunch";
// 		case DW_DS_leading_separate: return "DW_DS_leading_separate";
// 		case DW_DS_trailing_separate: return "DW_DS_trailing_separate";
// 	}
// 	std::stringstream sstr;
// 	sstr << "0x" << std::hex << (unsigned int) dw_ds << std::dec << " (?)";
// 	strncpy(buf, sstr.str().c_str(), sizeof(buf));
// 	buf[sizeof(buf) - 1] = 0;
// 	return (const char *) buf;
// }
// 
// const char *DWARF_GetIDName(uint8_t dw_id)
// {
// 	static char buf[9];
// 	switch(dw_id)
// 	{
// 		case DW_ID_case_sensitive: return "DW_ID_case_sensitive";
// 		case DW_ID_up_case: return "DW_ID_up_case";
// 		case DW_ID_down_case: return "DW_ID_down_case";
// 		case DW_ID_case_insensitive: return "DW_ID_case_insensitive";
// 	}
// 	std::stringstream sstr;
// 	sstr << "0x" << std::hex << (unsigned int) dw_id << std::dec << " (?)";
// 	strncpy(buf, sstr.str().c_str(), sizeof(buf));
// 	buf[sizeof(buf) - 1] = 0;
// 	return (const char *) buf;
// }
// 
// const char *DWARF_GetCCName(uint8_t dw_cc)
// {
// 	static char buf[9];
// 	switch(dw_cc)
// 	{
// 		case DW_CC_normal: return "DW_CC_normal";
// 		case DW_CC_program: return "DW_CC_program";
// 		case DW_CC_nocall: return "DW_CC_nocall";
// 	}
// 	std::stringstream sstr;
// 	sstr << "0x" << std::hex << (unsigned int) dw_cc << std::dec << " (?)";
// 	strncpy(buf, sstr.str().c_str(), sizeof(buf));
// 	buf[sizeof(buf) - 1] = 0;
// 	return (const char *) buf;
// }
// 
// const char *DWARF_GetINLName(uint8_t dw_inl)
// {
// 	static char buf[9];
// 	switch(dw_inl)
// 	{
// 		case DW_INL_not_inlined: return "DW_INL_not_inlined";
// 		case DW_INL_inlined: return "DW_INL_inlined";
// 		case DW_INL_declared_not_inlined: return "DW_INL_declared_not_inlined";
// 		case DW_INL_declared_inlined: return "DW_INL_declared_inlined";
// 	}
// 	std::stringstream sstr;
// 	sstr << "0x" << std::hex << (unsigned int) dw_inl << std::dec << " (?)";
// 	strncpy(buf, sstr.str().c_str(), sizeof(buf));
// 	buf[sizeof(buf) - 1] = 0;
// 	return (const char *) buf;
// }
// 
// const char *DWARF_GetORDName(uint8_t dw_ord)
// {
// 	static char buf[9];
// 	switch(dw_ord)
// 	{
// 		case DW_ORD_row_major: return "DW_ORD_row_major";
// 		case DW_ORD_col_major: return "DW_ORD_col_major";
// 	}
// 	std::stringstream sstr;
// 	sstr << "0x" << std::hex << (unsigned int) dw_ord << std::dec << " (?)";
// 	strncpy(buf, sstr.str().c_str(), sizeof(buf));
// 	buf[sizeof(buf) - 1] = 0;
// 	return (const char *) buf;
// }
// 
// const char *DWARF_GetDSCName(uint8_t dw_dsc)
// {
// 	static char buf[9];
// 	switch(dw_dsc)
// 	{
// 		case DW_DSC_label: return "DW_DSC_label";
// 		case DW_DSC_range: return "DW_DSC_range";
// 	}
// 	std::stringstream sstr;
// 	sstr << "0x" << std::hex << (unsigned int) dw_dsc << std::dec << " (?)";
// 	strncpy(buf, sstr.str().c_str(), sizeof(buf));
// 	buf[sizeof(buf) - 1] = 0;
// 	return (const char *) buf;
// }
// 
// const char *DWARF_GetCHILDRENName(uint8_t dw_children)
// {
// 	static char buf[9];
// 	switch(dw_children)
// 	{
// 		case DW_CHILDREN_no: return "DW_CHILDREN_no";
// 		case DW_CHILDREN_yes: return "DW_CHILDREN_yes";
// 	}
// 	std::stringstream sstr;
// 	sstr << "0x" << std::hex << (unsigned int) dw_children << std::dec << " (?)";
// 	strncpy(buf, sstr.str().c_str(), sizeof(buf));
// 	buf[sizeof(buf) - 1] = 0;
// 	return (const char *) buf;
// }

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim
