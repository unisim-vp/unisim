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

#include <unisim/service/loader/elf_loader/dwarf.hh>
#include <iostream>
#include <sstream>
#include <string.h>
#include <limits>

namespace unisim {
namespace service {
namespace loader {
namespace elf_loader {

DWARF_LEB128::DWARF_LEB128()
	: leb128(0)
{
}

DWARF_LEB128::DWARF_LEB128(const DWARF_LEB128& _leb128)
	: leb128(0)
{
	leb128 = _leb128.leb128;
/*	if(_leb128.leb128)
	{
		unsigned int byte_size = _leb128.GetByteSize();
		leb128 = new uint8_t[byte_size];
		memcpy(leb128, _leb128.leb128, byte_size);
	}*/
}

DWARF_LEB128::~DWARF_LEB128()
{
/*	if(leb128)
	{
		delete[] leb128;
		leb128 = 0;
	}*/
}

template <typename T>
bool DWARF_LEB128::Fit(const T *t) const
{
	return 8 * sizeof(T) >= GetBitLength();
}

template <typename T>
DWARF_LEB128::operator T() const
{
	T result = 0;
	if(leb128)
	{
		const uint8_t *p = leb128;
		unsigned int shift = 0;
		unsigned int size = 8 * sizeof(T);
		uint8_t byte;
		do
		{
			byte = *(p++);
			result |= (T)(byte & 0x7f) << shift;
			shift += 7;
		}
		while(byte & 0x80);

		if(std::numeric_limits<T>::is_signed)
		{
			if((shift < size) && (byte & 0x40))
			result |= -((T) 1 << shift);
		}
	}
	return result;
}

DWARF_LEB128& DWARF_LEB128::operator = (const DWARF_LEB128& _leb128)
{
	leb128 = _leb128.leb128;
	
/*	if(leb128)
	{
		delete[] leb128;
		leb128 = 0;
	}

	if(_leb128.leb128)
	{
		unsigned int byte_size = _leb128.GetByteSize();
		leb128 = new uint8_t[byte_size];
		memcpy(leb128, _leb128.leb128, byte_size);
	}*/
	
	return *this;
}

bool DWARF_LEB128::operator == (const DWARF_LEB128& _leb128) const
{
	unsigned int byte_index;
	uint8_t byte1, byte2;

	byte_index = 0;
	do
	{
		byte1 = leb128[byte_index];
		byte2 = _leb128.leb128[byte_index];
		if(byte1 != byte2) return false;
		byte_index++;
	} while(byte1 & 0x80);
		
	return true;

}

unsigned int DWARF_LEB128::GetByteSize() const
{
	unsigned int byte_index;

	byte_index = 0;
	do {} while(leb128[byte_index++] & 0x80);
		
	return byte_index;
}

unsigned int DWARF_LEB128::GetBitLength() const
{
	return GetByteSize() * 7;
}

int64_t DWARF_LEB128::Load(const uint8_t *rawdata, uint64_t max_size)
{
// 	if(leb128)
// 	{
// 		delete[] leb128;
// 		leb128 = 0;
// 	}

	unsigned int byte_index;

	byte_index = 0;
	do
	{
		if(byte_index >= max_size) return -1;
	}
	while(rawdata[byte_index++] & 0x80);
	
	unsigned int byte_size = byte_index;

	leb128 = rawdata;
// 	leb128 = new uint8_t[byte_size];
// 	
// 	for(byte_index = 0; byte_index < byte_size; byte_index++)
// 	{
// 		leb128[byte_index] = rawdata[byte_index];
// 	}

	return byte_size;
}

bool DWARF_LEB128::IsZero() const
{
	return !leb128 || (leb128 && ((*leb128) == 0));
}

std::string DWARF_LEB128::to_hex(bool is_signed) const
{
	if(leb128)
	{
		static char hex_chars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
		unsigned int bit_length = GetBitLength();
		unsigned int byte_length = (bit_length + 7) / 8;
		uint8_t data[byte_length];
		memset(data, 0, byte_length);

		unsigned int shift = 0;
		unsigned int i;
		const uint8_t *p = leb128;
		uint8_t byte;
		do
		{
			byte = *(p++);
			for(i = 0; i < 7; i++, shift++)
			{
				data[shift / 8] |= (byte & (1 << i)) << (shift % 8);
			}
		}
		while(byte & 0x80);

		if(is_signed)
		{
			if((shift < bit_length) && (byte & 0x40))
				data[shift / 8] |= -(1 << (shift % 8));
		}
		
		std::string result;
		int j;
		for(j = byte_length - 1; j >= 0; j--)
		{
			result += hex_chars[(data[j] >> 4) & 0xf];
			result += hex_chars[data[j] & 0xf];
		}
		return result;
	}
	return std::string();
}

std::string DWARF_LEB128::to_string(bool is_signed) const
{
	if(leb128)
	{
		if(is_signed)
		{
			if(Fit<int64_t>())
			{
				std::stringstream sstr;
				sstr << (int64_t) *this;
				return sstr.str();
			}
			else
			{
				std::stringstream sstr;
				sstr << "0x" << to_hex(is_signed);
				return sstr.str();
			}
		}
		else
		{
			if(Fit<uint64_t>())
			{
				std::stringstream sstr;
				sstr << (uint64_t) *this;
				return sstr.str();
			}
			else
			{
				std::stringstream sstr;
				sstr << "0x" << to_hex(is_signed);
				return sstr.str();
			}
		}
	}
	return std::string();
}

DWARF_Filename::DWARF_Filename()
	: filename()
	, directory_index()
	, last_modification_time()
	, byte_length()
{
}

DWARF_Filename::DWARF_Filename(const DWARF_Filename& dw_filename)
	: filename(dw_filename.filename)
	, directory_index(dw_filename.directory_index)
	, last_modification_time(dw_filename.last_modification_time)
	, byte_length(dw_filename.byte_length)
{
}

DWARF_Filename::~DWARF_Filename()
{
}

const char *DWARF_Filename::GetFilename() const
{
	return filename;
}

const DWARF_LEB128& DWARF_Filename::GetDirectoryIndex() const
{
	return directory_index;
}

const DWARF_LEB128& DWARF_Filename::GetLastModificationTime() const
{
	return last_modification_time;
}

const DWARF_LEB128& DWARF_Filename::GetByteLength() const
{
	return byte_length;
}

DWARF_Filename& DWARF_Filename::operator = (const DWARF_Filename& dw_filename)
{
	filename = dw_filename.filename;
	directory_index = dw_filename.directory_index;
	last_modification_time = dw_filename.last_modification_time;
	byte_length = dw_filename.byte_length;
	return *this;
}

int64_t DWARF_Filename::Load(const uint8_t *rawdata, uint64_t max_size)
{
	int64_t size = 0;
	int64_t sz;

	if(!max_size) return -1;
	filename = (const char *) rawdata;
	sz = strlen(filename) + 1;
	size += sz;
	rawdata += sz;
	max_size -= sz;
	
	if((sz = directory_index.Load(rawdata, max_size)) < 0) return -1;
	size += sz;
	rawdata += sz;
	max_size -= sz;
	
	if((sz = last_modification_time.Load(rawdata, max_size)) < 0) return -1;
	size += sz;
	rawdata += sz;
	max_size -= sz;

	if((sz = byte_length.Load(rawdata, max_size)) < 0) return -1;
	size += sz;
	rawdata += sz;
	max_size -= sz;

	return size;
}

std::ostream& operator << (std::ostream& os, const DWARF_Filename& dw_filename)
{
	return os << "File: \"" << dw_filename.filename << "\"  Dir: " << dw_filename.directory_index.to_string(false) << "  Time: " << dw_filename.last_modification_time.to_string(false) << "  Size: " << dw_filename.byte_length.to_string(false);
}

DWARF_Abbrev::DWARF_Abbrev(endian_type _endianness)
	: endianness(_endianness)
	, offset(0xffffffffffffffffULL)
	, abbrev_code()
	, dw_tag()
	, dw_children(DW_CHILDREN_no)
	, dw_abbrev_attributes()
{
}

DWARF_Abbrev::~DWARF_Abbrev()
{
	unsigned int num_attrs = dw_abbrev_attributes.size();
	unsigned int i;
	
	for(i = 0; i < num_attrs; i++)
	{
		delete dw_abbrev_attributes[i];
	}
}

int64_t DWARF_Abbrev::Load(const uint8_t *rawdata, uint64_t max_size, uint64_t _offset)
{
	offset = _offset;
	int64_t size = 0;
	int64_t sz;

	if((sz = abbrev_code.Load(rawdata, max_size)) < 0) return -1;
	size += sz;
	rawdata += sz;
	max_size -= sz;

	if(abbrev_code.IsZero()) return size; // Abbreviation code 0 is reserved for null entries
	
	if((sz = dw_tag.Load(rawdata, max_size)) < 0) return -1;
	size += sz;
	rawdata += sz;
	max_size -= sz;

	if(max_size < sizeof(dw_children)) return -1;
	memcpy(&dw_children, rawdata, sizeof(dw_children));
	dw_children = Target2Host(endianness, dw_children);
	rawdata += sizeof(dw_children);
	max_size -= sizeof(dw_children);
	size += sizeof(dw_children);

	do
	{
		if(!max_size) return -1;
		
		DWARF_AbbrevAttribute *dw_abbrev_attribute = new DWARF_AbbrevAttribute();
			
		int64_t sz;
		if((sz = dw_abbrev_attribute->Load(rawdata, max_size)) < 0)
		{
			delete dw_abbrev_attribute;
			return -1;
		}
		size += sz;
		rawdata += sz;
		max_size -= sz;
		
		if(dw_abbrev_attribute->IsNull())
		{
			delete dw_abbrev_attribute;
			break;
		}
		
		dw_abbrev_attributes.push_back(dw_abbrev_attribute);
	} while(1);
	
	return size;
}

bool DWARF_Abbrev::IsNull() const
{
	return abbrev_code.IsZero();
}

const DWARF_LEB128& DWARF_Abbrev::GetAbbrevCode() const
{
	return abbrev_code;
}

uint64_t DWARF_Abbrev::GetOffset() const
{
	return offset;
}

const std::vector<DWARF_AbbrevAttribute *>& DWARF_Abbrev::GetAbbrevAttributes() const
{
	return dw_abbrev_attributes;
}

bool DWARF_Abbrev::HasChildren() const
{
	return dw_children == DW_CHILDREN_yes;
}

std::ostream& operator << (std::ostream& os, const DWARF_Abbrev& dw_abbrev)
{
	os << "Abbreviation:" << std::endl
	   << " - Offset: " << dw_abbrev.offset << std::endl
	   << " - Abbrev code: " << dw_abbrev.abbrev_code.to_string(false) << std::endl
	   << " - Tag: " << DWARF_GetTagName((uint16_t) dw_abbrev.dw_tag) << std::endl
	   << " - Has Children? " << ((dw_abbrev.dw_children == DW_CHILDREN_yes) ? "yes" : ((dw_abbrev.dw_children == DW_CHILDREN_no) ? "no" : "invalid encoding")) << std::endl
	   << " - Attributes:" << std::endl;
	
	unsigned int num_attrs = dw_abbrev.dw_abbrev_attributes.size();
	unsigned int i;
	
	for(i = 0; i < num_attrs; i++)
	{
		os << "   - " << *dw_abbrev.dw_abbrev_attributes[i] << std::endl;
	}
	return os;
}

DWARF_AbbrevAttribute::DWARF_AbbrevAttribute()
	: dw_at()
	, dw_form()
{
}

DWARF_AbbrevAttribute::~DWARF_AbbrevAttribute()
{
}

int64_t DWARF_AbbrevAttribute::Load(const uint8_t *rawdata, uint64_t max_size)
{
	int64_t size = 0;
	int64_t sz;

	if((sz = dw_at.Load(rawdata, max_size)) < 0) return -1;
	size += sz;
	rawdata += sz;
	max_size -= sz;

	if((sz = dw_form.Load(rawdata, max_size)) < 0) return -1;
	size += sz;
	rawdata += sz;
	max_size -= sz;
	
	return size;
}

bool DWARF_AbbrevAttribute::IsNull() const
{
	return dw_at.IsZero() && dw_form.IsZero();
}

const DWARF_LEB128& DWARF_AbbrevAttribute::GetForm() const
{
	return dw_form;
}

const char *DWARF_AbbrevAttribute::GetName() const
{
	return DWARF_GetATName(dw_at);
}

const DWARF_LEB128& DWARF_AbbrevAttribute::GetTag() const
{
	return dw_at;
}

std::ostream& operator << (std::ostream& os, const DWARF_AbbrevAttribute& dw_abbrev_attribute)
{
	return os << "Name: " << DWARF_GetATName(dw_abbrev_attribute.dw_at) << "  Form: " << DWARF_GetFORMName(dw_abbrev_attribute.dw_form);
}

const char *DWARF_GetTagName(uint16_t dw_tag)
{
	static char buf[14];
	
	switch(dw_tag)
	{
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
	}
	std::stringstream sstr;
	sstr << "0x" << std::hex << dw_tag << std::dec << " ("
			// << (((dw_tag >= DW_TAG_lo_user) && (dw_tag <= DW_TAG_hi_user)) ?
			<< (((dw_tag >= DW_TAG_lo_user)) ?
					"user" : "?")
			<< ")";
	strncpy(buf, sstr.str().c_str(), sizeof(buf));
	buf[sizeof(buf) - 1] = 0;
	return (const char *) buf;
}

const char *DWARF_GetATName(uint16_t dw_at)
{
	static char buf[14];
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
	}
	std::stringstream sstr;
	sstr << "0x" << std::hex << dw_at << std::dec << " (" << (((dw_at >= DW_AT_lo_user) && (dw_at <= DW_AT_hi_user)) ? "user" : "?") << ")";
	strncpy(buf, sstr.str().c_str(), sizeof(buf));
	buf[sizeof(buf) - 1] = 0;
	return (const char *) buf;
}

const char *DWARF_GetFORMName(uint16_t dw_form)
{
	static char buf[14];
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
	}
	std::stringstream sstr;
	sstr << "0x" << std::hex << dw_form << std::dec << " (?)";
	strncpy(buf, sstr.str().c_str(), sizeof(buf));
	buf[sizeof(buf) - 1] = 0;
	return (const char *) buf;
}

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

const char *DWARF_GetATEName(uint8_t dw_ate)
{
	static char buf[9];
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
	sstr << "0x" << std::hex << (unsigned int) dw_ate << std::dec << " (?)";
	strncpy(buf, sstr.str().c_str(), sizeof(buf));
	buf[sizeof(buf) - 1] = 0;
	return (const char *) buf;
}

const char *DWARF_GetLANGName(uint16_t dw_lang)
{
	static char buf[14];
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
		case DW_LANG_Mips_Assembler: return "DW_LANG_Mips_Assembler";
		case DW_LANG_Upc: return "DW_LANG_Upc";
	}
	std::stringstream sstr;
	sstr << "0x" << std::hex << (unsigned int) dw_lang << std::dec << " (?)";
	strncpy(buf, sstr.str().c_str(), sizeof(buf));
	buf[sizeof(buf) - 1] = 0;
	return (const char *) buf;
}

const char *DWARF_GetVIRTUALITYName(uint8_t dw_virtuality)
{
	static char buf[9];
	switch(dw_virtuality)
	{
		case DW_VIRTUALITY_none: return "DW_VIRTUALITY_none";
		case DW_VIRTUALITY_virtual: return "DW_VIRTUALITY_virtual";
		case DW_VIRTUALITY_pure_virtual: return "DW_VIRTUALITY_pure_virtual";
	}
	std::stringstream sstr;
	sstr << "0x" << std::hex << (unsigned int) dw_virtuality << std::dec << " (?)";
	strncpy(buf, sstr.str().c_str(), sizeof(buf));
	buf[sizeof(buf) - 1] = 0;
	return (const char *) buf;
}

const char *DWARF_GetVISName(uint8_t dw_vis)
{
	static char buf[9];
	switch(dw_vis)
	{
		case DW_VIS_local: return "DW_VIS_local";
		case DW_VIS_exported: return "DW_VIS_exported";
		case DW_VIS_qualified: return "DW_VIS_qualified";
	}
	std::stringstream sstr;
	sstr << "0x" << std::hex << (unsigned int) dw_vis << std::dec << " (?)";
	strncpy(buf, sstr.str().c_str(), sizeof(buf));
	buf[sizeof(buf) - 1] = 0;
	return (const char *) buf;
}

const char *DWARF_GetACCESSName(uint8_t dw_access)
{
	static char buf[9];
	switch(dw_access)
	{
		case DW_ACCESS_public: return "DW_ACCESS_public";
		case DW_ACCESS_protected: return "DW_ACCESS_protected";
		case DW_ACCESS_private: return "DW_ACCESS_private";
	}
	std::stringstream sstr;
	sstr << "0x" << std::hex << (unsigned int) dw_access << std::dec << " (?)";
	strncpy(buf, sstr.str().c_str(), sizeof(buf));
	buf[sizeof(buf) - 1] = 0;
	return (const char *) buf;
}

const char *DWARF_GetENDName(uint8_t dw_end)
{
	static char buf[9];
	switch(dw_end)
	{
		case DW_END_default: return "DW_END_default";
		case DW_END_big: return "DW_END_big";
		case DW_END_little: return "DW_END_little";
	}
	std::stringstream sstr;
	sstr << "0x" << std::hex << (unsigned int) dw_end << std::dec << " (?)";
	strncpy(buf, sstr.str().c_str(), sizeof(buf));
	buf[sizeof(buf) - 1] = 0;
	return (const char *) buf;
}

const char *DWARF_GetDSName(uint8_t dw_ds)
{
	static char buf[9];
	switch(dw_ds)
	{
		case DW_DS_unsigned: return "DW_DS_unsigned";
		case DW_DS_leading_overpunch: return "DW_DS_leading_overpunch";
		case DW_DS_trailing_overpunch: return "DW_DS_trailing_overpunch";
		case DW_DS_leading_separate: return "DW_DS_leading_separate";
		case DW_DS_trailing_separate: return "DW_DS_trailing_separate";
	}
	std::stringstream sstr;
	sstr << "0x" << std::hex << (unsigned int) dw_ds << std::dec << " (?)";
	strncpy(buf, sstr.str().c_str(), sizeof(buf));
	buf[sizeof(buf) - 1] = 0;
	return (const char *) buf;
}

const char *DWARF_GetIDName(uint8_t dw_id)
{
	static char buf[9];
	switch(dw_id)
	{
		case DW_ID_case_sensitive: return "DW_ID_case_sensitive";
		case DW_ID_up_case: return "DW_ID_up_case";
		case DW_ID_down_case: return "DW_ID_down_case";
		case DW_ID_case_insensitive: return "DW_ID_case_insensitive";
	}
	std::stringstream sstr;
	sstr << "0x" << std::hex << (unsigned int) dw_id << std::dec << " (?)";
	strncpy(buf, sstr.str().c_str(), sizeof(buf));
	buf[sizeof(buf) - 1] = 0;
	return (const char *) buf;
}

const char *DWARF_GetCCName(uint8_t dw_cc)
{
	static char buf[9];
	switch(dw_cc)
	{
		case DW_CC_normal: return "DW_CC_normal";
		case DW_CC_program: return "DW_CC_program";
		case DW_CC_nocall: return "DW_CC_nocall";
	}
	std::stringstream sstr;
	sstr << "0x" << std::hex << (unsigned int) dw_cc << std::dec << " (?)";
	strncpy(buf, sstr.str().c_str(), sizeof(buf));
	buf[sizeof(buf) - 1] = 0;
	return (const char *) buf;
}

const char *DWARF_GetINLName(uint8_t dw_inl)
{
	static char buf[9];
	switch(dw_inl)
	{
		case DW_INL_not_inlined: return "DW_INL_not_inlined";
		case DW_INL_inlined: return "DW_INL_inlined";
		case DW_INL_declared_not_inlined: return "DW_INL_declared_not_inlined";
		case DW_INL_declared_inlined: return "DW_INL_declared_inlined";
	}
	std::stringstream sstr;
	sstr << "0x" << std::hex << (unsigned int) dw_inl << std::dec << " (?)";
	strncpy(buf, sstr.str().c_str(), sizeof(buf));
	buf[sizeof(buf) - 1] = 0;
	return (const char *) buf;
}

const char *DWARF_GetORDName(uint8_t dw_ord)
{
	static char buf[9];
	switch(dw_ord)
	{
		case DW_ORD_row_major: return "DW_ORD_row_major";
		case DW_ORD_col_major: return "DW_ORD_col_major";
	}
	std::stringstream sstr;
	sstr << "0x" << std::hex << (unsigned int) dw_ord << std::dec << " (?)";
	strncpy(buf, sstr.str().c_str(), sizeof(buf));
	buf[sizeof(buf) - 1] = 0;
	return (const char *) buf;
}

const char *DWARF_GetDSCName(uint8_t dw_dsc)
{
	static char buf[9];
	switch(dw_dsc)
	{
		case DW_DSC_label: return "DW_DSC_label";
		case DW_DSC_range: return "DW_DSC_range";
	}
	std::stringstream sstr;
	sstr << "0x" << std::hex << (unsigned int) dw_dsc << std::dec << " (?)";
	strncpy(buf, sstr.str().c_str(), sizeof(buf));
	buf[sizeof(buf) - 1] = 0;
	return (const char *) buf;
}

template bool DWARF_LEB128::Fit<char>(const char *t = 0) const;
template bool DWARF_LEB128::Fit<short>(const short *t = 0) const;
template bool DWARF_LEB128::Fit<int>(const int *t = 0) const;
template bool DWARF_LEB128::Fit<long>(const long *t = 0) const;
template bool DWARF_LEB128::Fit<long long>(const long long *t = 0) const;
template bool DWARF_LEB128::Fit<unsigned char>(const unsigned char *t = 0) const;
template bool DWARF_LEB128::Fit<unsigned short>(const unsigned short *t = 0) const;
template bool DWARF_LEB128::Fit<unsigned int>(const unsigned int *t = 0) const;
template bool DWARF_LEB128::Fit<unsigned long>(const unsigned long *t = 0) const;
template bool DWARF_LEB128::Fit<unsigned long long>(const unsigned long long *t = 0) const;

template DWARF_LEB128::operator char() const;
template DWARF_LEB128::operator short() const;
template DWARF_LEB128::operator int() const;
template DWARF_LEB128::operator long() const;
template DWARF_LEB128::operator long long() const;
template DWARF_LEB128::operator unsigned char() const;
template DWARF_LEB128::operator unsigned short() const;
template DWARF_LEB128::operator unsigned int() const;
template DWARF_LEB128::operator unsigned long() const;
template DWARF_LEB128::operator unsigned long long() const;

} // end of namespace elf_loader
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim
