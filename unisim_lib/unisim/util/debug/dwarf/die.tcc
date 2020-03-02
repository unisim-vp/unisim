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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_DIE_TCC__
#define __UNISIM_UTIL_DEBUG_DWARF_DIE_TCC__

#include <unisim/util/debug/dwarf/util.hh>
#include <unisim/util/debug/dwarf/option.hh>
#include <unisim/util/arithmetic/arithmetic.hh>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
DWARF_DIE<MEMORY_ADDR>::DWARF_DIE(DWARF_CompilationUnit<MEMORY_ADDR> *_dw_cu, DWARF_DIE<MEMORY_ADDR> *_dw_parent_die)
	: dw_handler(_dw_cu->GetHandler())
	, dw_cu(_dw_cu)
	, dw_parent_die(_dw_parent_die)
	, debug_info_stream(dw_handler->GetDebugInfoStream())
	, debug_warning_stream(dw_handler->GetDebugWarningStream())
	, debug_error_stream(dw_handler->GetDebugErrorStream())
	, debug(dw_handler->GetOptionFlag(OPT_DEBUG))
	, offset(0xffffffffffffffffULL)
	, abbrev(0)
	, children()
	, attributes()
	, cached_name()
	, cached_declaration_flag()
	, cached_external_flag()
	, cached_specification()
	, cached_dw_abstract_origin()
	, cached_dw_type_die()
	, cached_dw_defining_die()
	, cached_low_pc()
	, cached_high_pc()
	, cached_high_pc_offset()
	, cached_ranges()
	, type_cache()
	, cached_subprogram()
	, cached_variable()
{
}

template <class MEMORY_ADDR>
DWARF_DIE<MEMORY_ADDR>::~DWARF_DIE()
{
	unsigned int num_attributes = attributes.size();
	unsigned int i;
	for(i = 0; i < num_attributes; i++)
	{
		delete attributes[i];
	}
	unsigned int num_children = children.size();
	for(i = 0; i < num_children; i++)
	{
		dw_cu->UnRegister(children[i]);
		delete children[i];
	}
	
	for(typename TypeCache::const_iterator it = type_cache.begin(); it != type_cache.end(); ++it)
	{
		unisim::util::debug::Type const *cached_type = *it;
		if(cached_type) cached_type->Release();
	}
	
	if(cached_subprogram.Valid())
	{
		(*cached_subprogram).Release();
	}
	
	if(cached_variable.Valid())
	{
		(*cached_variable).Release();
	}
}

template <class MEMORY_ADDR>
const DWARF_CompilationUnit<MEMORY_ADDR> *DWARF_DIE<MEMORY_ADDR>::GetCompilationUnit() const
{
	return dw_cu;
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_DIE<MEMORY_ADDR>::GetParentDIE() const
{
	return dw_parent_die;
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_DIE<MEMORY_ADDR>::GetRootDIE() const
{
	return dw_parent_die ? dw_parent_die->GetRootDIE() : this;
}

template <class MEMORY_ADDR>
const DWARF_Abbrev *DWARF_DIE<MEMORY_ADDR>::GetAbbrev() const
{
	return abbrev;
}

template <class MEMORY_ADDR>
const std::vector<DWARF_DIE<MEMORY_ADDR> *>& DWARF_DIE<MEMORY_ADDR>::GetChildren() const
{
	return children;
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_DIE<MEMORY_ADDR>::GetChild(uint16_t _dw_tag) const
{
	unsigned int num_children = children.size();
	unsigned int i;
	for(i = 0; i < num_children; i++)
	{
		DWARF_DIE<MEMORY_ADDR> *dw_child = children[i];
		if(dw_child->GetTag() == _dw_tag) return dw_child;
	}
	return 0;
}

template <class MEMORY_ADDR>
const std::vector<DWARF_Attribute<MEMORY_ADDR> *>& DWARF_DIE<MEMORY_ADDR>::GetAttributes() const
{
	return attributes;
}

template <class MEMORY_ADDR>
const DWARF_Attribute<MEMORY_ADDR> *DWARF_DIE<MEMORY_ADDR>::FindAttribute(uint16_t dw_at) const
{
	unsigned int num_attributes = attributes.size();
	unsigned int i;

	for(i = 0; i < num_attributes; i++)
	{
		const DWARF_Attribute<MEMORY_ADDR> *dw_attr = attributes[i];
		if((uint16_t) dw_attr->GetAbbrevAttribute()->GetTag() == dw_at) return dw_attr;
	}
	return 0;
}

template <class MEMORY_ADDR>
uint16_t DWARF_DIE<MEMORY_ADDR>::GetTag() const
{
	return abbrev ? (uint16_t) abbrev->GetTag() : 0;
}

template <class MEMORY_ADDR>
uint64_t DWARF_DIE<MEMORY_ADDR>::GetOffset() const
{
	return offset;
}

template <class MEMORY_ADDR>
void DWARF_DIE<MEMORY_ADDR>::Add(DWARF_Attribute<MEMORY_ADDR> *dw_attribute)
{
	attributes.push_back(dw_attribute);
}

template <class MEMORY_ADDR>
void DWARF_DIE<MEMORY_ADDR>::Add(DWARF_DIE<MEMORY_ADDR> *child)
{
	children.push_back(child);
}

template <class MEMORY_ADDR>
int64_t DWARF_DIE<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size, uint64_t _offset)
{
	offset = _offset;
	uint8_t address_size = dw_cu->GetAddressSize();
	uint8_t offset_size = dw_cu->GetOffsetSize();
	endian_type file_endianness = dw_cu->GetHandler()->GetFileEndianness();
	DWARF_Version dw_ver = dw_cu->GetDWARFVersion();
	uint64_t size = 0;
	int64_t sz;
	DWARF_LEB128 dw_abbrev_code;
	if((sz = dw_abbrev_code.Load(rawdata, max_size)) < 0) return -1;
	rawdata += sz;
	max_size -= sz;
	size += sz;
	if(dw_abbrev_code.IsZero())
	{
		abbrev = 0;
		return size;
	}

	//std::cerr << ".debug_info: at " << size << " got " << dw_abbrev_code.to_string(false) << std::endl;

	
	//std::cerr << "Looking for abbrevation " << dw_abbrev_code.to_string(false) << " starting offset " << debug_abbrev_offset << std::endl;
	
	const DWARF_Abbrev *dw_abbrev = dw_cu->FindAbbrev(dw_abbrev_code);
	
	if(!dw_abbrev)
	{
		debug_warning_stream << "Can't find abbrevation " << dw_abbrev_code.to_string(false) << std::endl;
		return -1;
	}
	
	const std::vector<DWARF_AbbrevAttribute *>& dw_abbrev_attributes = dw_abbrev->GetAbbrevAttributes();
	std::vector<DWARF_AbbrevAttribute *>::const_iterator dw_abbrev_attribute_iter;
			
	for(dw_abbrev_attribute_iter = dw_abbrev_attributes.begin(); dw_abbrev_attribute_iter != dw_abbrev_attributes.end(); dw_abbrev_attribute_iter++)
	{
		DWARF_AbbrevAttribute *dw_abbrev_attribute = *dw_abbrev_attribute_iter;
		
		//std::cerr << *dw_abbrev_attribute << std::endl;
		
		const DWARF_LEB128& dw_at_leb128 = dw_abbrev_attribute->GetTag();
		const DWARF_LEB128& dw_form_leb128 = dw_abbrev_attribute->GetForm();
		
		
		DWARF_Attribute<MEMORY_ADDR> *dw_attribute = 0;
		
		uint16_t dw_form = (uint16_t) dw_form_leb128;
		uint16_t dw_at = (uint16_t) dw_at_leb128;
		
		if(dw_form == DW_FORM_indirect)
		{
			DWARF_LEB128 dw_actual_form_leb128;
			int64_t sz;
			if((sz = dw_actual_form_leb128.Load(rawdata, max_size)) < 0) return -1;
			size += sz;
			rawdata += sz;
			max_size -= sz;
			
			dw_form = (uint16_t) dw_actual_form_leb128;
		}
		
		switch(dw_form)
		{
			case DW_FORM_addr:
				{
					MEMORY_ADDR addr;
					
					switch(address_size)
					{
						case sizeof(uint8_t):
							{
								if(max_size < sizeof(uint8_t)) return -1;
								uint8_t value;
								memcpy(&value, rawdata, sizeof(uint8_t));
								addr = Target2Host(file_endianness, value);
								size += sizeof(uint8_t);
								rawdata += sizeof(uint8_t);
								max_size -= sizeof(uint8_t);
							}
							break;
						case sizeof(uint16_t):
							{
								if(max_size < sizeof(uint16_t)) return -1;
								uint16_t value;
								memcpy(&value, rawdata, sizeof(uint16_t));
								addr = Target2Host(file_endianness, value);
								size += sizeof(uint16_t);
								rawdata += sizeof(uint16_t);
								max_size -= sizeof(uint16_t);
							}
							break;
						case sizeof(uint32_t):
							{
								if(max_size < sizeof(uint32_t)) return -1;
								uint32_t value;
								memcpy(&value, rawdata, sizeof(uint32_t));
								addr = Target2Host(file_endianness, value);
								size += sizeof(uint32_t);
								rawdata += sizeof(uint32_t);
								max_size -= sizeof(uint32_t);
							}
							break;
						case sizeof(uint64_t):
							{
								if(max_size < sizeof(uint64_t)) return -1;
								uint64_t value;
								memcpy(&value, rawdata, sizeof(uint64_t));
								addr = Target2Host(file_endianness, value);
								size += sizeof(uint64_t);
								rawdata += sizeof(uint64_t);
								max_size -= sizeof(uint64_t);
							}
							break;
						default:
							return -1;
					}
					
					DWARF_Address<MEMORY_ADDR> *dw_address = new DWARF_Address<MEMORY_ADDR>(addr);
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_address);
				}
				break;
			case DW_FORM_block2:
				{
					if(max_size < sizeof(uint16_t)) return -1;
					uint16_t block_length;
					memcpy(&block_length, rawdata, sizeof(block_length));
					block_length = Target2Host(file_endianness, block_length);
					size += sizeof(block_length);
					rawdata += sizeof(block_length);
					max_size -= sizeof(block_length);
					if(max_size < block_length) return -1;
					
					DWARF_AttributeValue<MEMORY_ADDR> *dw_value;
					
					switch(dw_at)
					{
						case DW_AT_location:
						case DW_AT_byte_size:
						case DW_AT_bit_offset:
						case DW_AT_bit_size:
						case DW_AT_string_length:
						case DW_AT_lower_bound:
						case DW_AT_return_addr:
						case DW_AT_upper_bound:
						case DW_AT_count:
						case DW_AT_data_member_location:
						case DW_AT_frame_base:
						case DW_AT_segment:
						case DW_AT_static_link:
						case DW_AT_use_location:
						case DW_AT_vtable_elem_location:
						case DW_AT_allocated:
						case DW_AT_associated:
						case DW_AT_data_location:
						case DW_AT_byte_stride:
							dw_value = new DWARF_Expression<MEMORY_ADDR>(dw_cu, block_length, rawdata);
							break;
						default:
							dw_value = new DWARF_Block<MEMORY_ADDR>(block_length, rawdata);
							break;
					}
					
					rawdata += block_length;
					size += block_length;
					max_size -= block_length;
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_value);
				}
				break;
			case DW_FORM_block4:
				{
					if(max_size < sizeof(uint32_t)) return -1;
					uint32_t block_length;
					memcpy(&block_length, rawdata, sizeof(block_length));
					block_length = Target2Host(file_endianness, block_length);
					size += sizeof(block_length);
					rawdata += sizeof(block_length);
					max_size -= sizeof(block_length);
					if(max_size < block_length) return -1;
					
					DWARF_AttributeValue<MEMORY_ADDR> *dw_value;
					
					switch(dw_at)
					{
						case DW_AT_location:
						case DW_AT_byte_size:
						case DW_AT_bit_offset:
						case DW_AT_bit_size:
						case DW_AT_string_length:
						case DW_AT_lower_bound:
						case DW_AT_return_addr:
						case DW_AT_upper_bound:
						case DW_AT_count:
						case DW_AT_data_member_location:
						case DW_AT_frame_base:
						case DW_AT_segment:
						case DW_AT_static_link:
						case DW_AT_use_location:
						case DW_AT_vtable_elem_location:
						case DW_AT_allocated:
						case DW_AT_associated:
						case DW_AT_data_location:
						case DW_AT_byte_stride:
							dw_value = new DWARF_Expression<MEMORY_ADDR>(dw_cu, block_length, rawdata);
							break;
						default:
							dw_value = new DWARF_Block<MEMORY_ADDR>(block_length, rawdata);
							break;
					}
					
					rawdata += block_length;
					size += block_length;
					max_size -= block_length;
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_value);
				}
				break;
			case DW_FORM_data2:
				{
					if(max_size < sizeof(uint16_t)) return -1;
					uint16_t value;
					memcpy(&value, rawdata, sizeof(uint16_t));
					value = Target2Host(file_endianness, value);
					size += sizeof(uint16_t);
					rawdata += sizeof(uint16_t);
					max_size -= sizeof(uint16_t);
					DWARF_UnsignedConstant<MEMORY_ADDR> *dw_const = new DWARF_UnsignedConstant<MEMORY_ADDR>((uint64_t) value);
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_const);
				}
				break;
			case DW_FORM_data4:
				{
					if(max_size < sizeof(uint32_t)) return -1;
					uint32_t value;
					memcpy(&value, rawdata, sizeof(uint32_t));
					value = Target2Host(file_endianness, value);
					size += sizeof(uint32_t);
					rawdata += sizeof(uint32_t);
					max_size -= sizeof(uint32_t);
					
					DWARF_AttributeValue<MEMORY_ADDR> *dw_value;
					
					if(dw_ver < DW_VER4)
					{
						switch(dw_at)
						{
							case DW_AT_stmt_list:
								dw_value = new DWARF_LinePtr<MEMORY_ADDR>((uint64_t) value);
								break;
							case DW_AT_ranges:
								dw_value = new DWARF_RangeListPtr<MEMORY_ADDR>(dw_cu, (uint64_t) value);
								break;
							case DW_AT_macro_info:
								dw_value = new DWARF_MacPtr<MEMORY_ADDR>((uint64_t) value);
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
								dw_value = new DWARF_LocListPtr<MEMORY_ADDR>(dw_cu, (uint64_t) value);
								break;
							default:
								dw_value = new DWARF_UnsignedConstant<MEMORY_ADDR>((uint64_t) value);
								break;
						}
					}
					else
					{
						dw_value = new DWARF_UnsignedConstant<MEMORY_ADDR>((uint64_t) value);
					}
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_value);
				}
				break;
			case DW_FORM_data8:
				{
					if(max_size < sizeof(uint64_t)) return -1;
					uint64_t value;
					memcpy(&value, rawdata, sizeof(uint64_t));
					value = Target2Host(file_endianness, value);
					size += sizeof(uint64_t);
					rawdata += sizeof(uint64_t);
					max_size -= sizeof(uint64_t);

					DWARF_AttributeValue<MEMORY_ADDR> *dw_value;
					
					if(dw_ver < DW_VER4)
					{
						switch(dw_at)
						{
							case DW_AT_stmt_list:
								dw_value = new DWARF_LinePtr<MEMORY_ADDR>((uint64_t) value);
								break;
							case DW_AT_ranges:
								dw_value = new DWARF_RangeListPtr<MEMORY_ADDR>(dw_cu, (uint64_t) value);
								break;
							case DW_AT_macro_info:
								dw_value = new DWARF_MacPtr<MEMORY_ADDR>((uint64_t) value);
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
								dw_value = new DWARF_LocListPtr<MEMORY_ADDR>(dw_cu, (uint64_t) value);
								break;
							default:
								dw_value = new DWARF_UnsignedConstant<MEMORY_ADDR>((uint64_t) value);
								break;
						}
					}
					else
					{
						dw_value = new DWARF_UnsignedConstant<MEMORY_ADDR>((uint64_t) value);
					}
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_value);
				}
				break;
			case DW_FORM_string:
				{
					const char *c_string = (const char *) rawdata;
					unsigned int string_length = strlen(c_string);
					if(max_size < (string_length + 1)) return -1;
					size += string_length + 1;
					rawdata += string_length + 1;
					max_size -= string_length + 1;
					DWARF_String<MEMORY_ADDR> *dw_string = new DWARF_String<MEMORY_ADDR>(c_string);
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_string);
				}
				break;
			case DW_FORM_block:
				{
					DWARF_LEB128 leb128_block_length;
					int64_t sz;
					if((sz = leb128_block_length.Load(rawdata, max_size)) < 0) return -1;
					rawdata += sz;
					size += sz;
					max_size -= sz;

					uint64_t block_length = (uint64_t) leb128_block_length;
					if(max_size < block_length) return -1;

					DWARF_AttributeValue<MEMORY_ADDR> *dw_value;
					
					switch(dw_at)
					{
						case DW_AT_location:
						case DW_AT_byte_size:
						case DW_AT_bit_offset:
						case DW_AT_bit_size:
						case DW_AT_string_length:
						case DW_AT_lower_bound:
						case DW_AT_return_addr:
						case DW_AT_upper_bound:
						case DW_AT_count:
						case DW_AT_data_member_location:
						case DW_AT_frame_base:
						case DW_AT_segment:
						case DW_AT_static_link:
						case DW_AT_use_location:
						case DW_AT_vtable_elem_location:
						case DW_AT_allocated:
						case DW_AT_associated:
						case DW_AT_data_location:
						case DW_AT_byte_stride:
							dw_value = new DWARF_Expression<MEMORY_ADDR>(dw_cu, block_length, rawdata);
							break;
						default:
							dw_value = new DWARF_Block<MEMORY_ADDR>(block_length, rawdata);
							break;
					}
					
					rawdata += block_length;
					size += block_length;
					max_size -= block_length;
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_value);
				}
				break;
			case DW_FORM_block1:
				{
					if(max_size < sizeof(uint8_t)) return -1;
					uint8_t block_length;
					memcpy(&block_length, rawdata, sizeof(block_length));
					block_length = Target2Host(file_endianness, block_length);
					size += sizeof(block_length);
					rawdata += sizeof(block_length);
					max_size -= sizeof(block_length);
					
					if(max_size < block_length) return -1;

					DWARF_AttributeValue<MEMORY_ADDR> *dw_value;
					
					switch(dw_at)
					{
						case DW_AT_location:
						case DW_AT_byte_size:
						case DW_AT_bit_offset:
						case DW_AT_bit_size:
						case DW_AT_string_length:
						case DW_AT_lower_bound:
						case DW_AT_return_addr:
						case DW_AT_upper_bound:
						case DW_AT_count:
						case DW_AT_data_member_location:
						case DW_AT_frame_base:
						case DW_AT_segment:
						case DW_AT_static_link:
						case DW_AT_use_location:
						case DW_AT_vtable_elem_location:
						case DW_AT_allocated:
						case DW_AT_associated:
						case DW_AT_data_location:
						case DW_AT_byte_stride:
							dw_value = new DWARF_Expression<MEMORY_ADDR>(dw_cu, block_length, rawdata);
							break;
						default:
							dw_value = new DWARF_Block<MEMORY_ADDR>(block_length, rawdata);
							break;
					}
					
					rawdata += block_length;
					size += block_length;
					max_size -= block_length;
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_value);
				}
				break;
			case DW_FORM_data1:
				{
					if(max_size < sizeof(uint8_t)) return -1;
					uint8_t value;
					memcpy(&value, rawdata, sizeof(uint8_t));
					value = Target2Host(file_endianness, value);
					size += sizeof(uint8_t);
					rawdata += sizeof(uint8_t);
					max_size -= sizeof(uint8_t);
					DWARF_UnsignedConstant<MEMORY_ADDR> *dw_const = new DWARF_UnsignedConstant<MEMORY_ADDR>((uint64_t) value);
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_const);
				}
				break;
			case DW_FORM_flag:
				{
					if(max_size < sizeof(uint8_t)) return -1;
					uint8_t value;
					memcpy(&value, rawdata, sizeof(uint8_t));
					size += sizeof(uint8_t);
					rawdata += sizeof(uint8_t);
					max_size -= sizeof(uint8_t);
					
					DWARF_Flag<MEMORY_ADDR> *dw_flag = new DWARF_Flag<MEMORY_ADDR>(value ? true : false);
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_flag);
				}
				break;
			case DW_FORM_sdata:
				{
					DWARF_LEB128 leb128;
					int64_t sz;
					if((sz = leb128.Load(rawdata, max_size)) < 0) return -1;
					size += sz;
					rawdata += sz;
					max_size -= sz;
					
					DWARF_SignedLEB128Constant<MEMORY_ADDR> *dw_const = new DWARF_SignedLEB128Constant<MEMORY_ADDR>(leb128);
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_const);
				}
				break;
			case DW_FORM_strp:
				{
					uint64_t debug_str_offset;
					switch(offset_size)
					{
						case 2:
							{
								uint16_t debug_str_offset16;
							
								if(max_size < sizeof(debug_str_offset16)) return -1;
								memcpy(&debug_str_offset16, rawdata, sizeof(debug_str_offset16));
								debug_str_offset16 = Target2Host(file_endianness, debug_str_offset16);
								rawdata += sizeof(debug_str_offset16);
								max_size -= sizeof(debug_str_offset16);
								size += sizeof(debug_str_offset16);
								debug_str_offset = debug_str_offset16;
							}
							break;
						case 4:
							{
								uint32_t debug_str_offset32;
							
								if(max_size < sizeof(debug_str_offset32)) return -1;
								memcpy(&debug_str_offset32, rawdata, sizeof(debug_str_offset32));
								debug_str_offset32 = Target2Host(file_endianness, debug_str_offset32);
								rawdata += sizeof(debug_str_offset32);
								max_size -= sizeof(debug_str_offset32);
								size += sizeof(debug_str_offset32);
								debug_str_offset = debug_str_offset32;
							}
							break;
						case 8:
							{
								uint64_t debug_str_offset64;
							
								if(max_size < sizeof(debug_str_offset64)) return -1;
								memcpy(&debug_str_offset64, rawdata, sizeof(debug_str_offset64));
								debug_str_offset64 = Target2Host(file_endianness, debug_str_offset64);
								rawdata += sizeof(debug_str_offset64);
								max_size -= sizeof(debug_str_offset64);
								size += sizeof(debug_str_offset64);
								debug_str_offset = debug_str_offset64;
							}
							break;
						default:
							return -1;
					}
					const char *c_str = dw_cu->GetString(debug_str_offset);
					if(!c_str) return -1;
					DWARF_String<MEMORY_ADDR> *dw_string = new DWARF_String<MEMORY_ADDR>(c_str);
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_string);
				}
				break;
			case DW_FORM_udata:
				{
					DWARF_LEB128 leb128;
					int64_t sz;
					if((sz = leb128.Load(rawdata, max_size)) < 0) return -1;
					size += sz;
					rawdata += sz;
					max_size -= sz;
					
					DWARF_UnsignedLEB128Constant<MEMORY_ADDR> *dw_const = new DWARF_UnsignedLEB128Constant<MEMORY_ADDR>(leb128);
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_const);
				}
				break;
			case DW_FORM_ref_addr:
				{
					uint64_t debug_info_offset;
					switch(offset_size)
					{
						case 2:
							{
								uint16_t debug_info_offset16;
							
								if(max_size < sizeof(debug_info_offset16)) return -1;
								memcpy(&debug_info_offset16, rawdata, sizeof(debug_info_offset16));
								debug_info_offset16 = Target2Host(file_endianness, debug_info_offset16);
								rawdata += sizeof(debug_info_offset16);
								max_size -= sizeof(debug_info_offset16);
								size += sizeof(debug_info_offset16);
								debug_info_offset = debug_info_offset16;
							}
							break;
						case 4:
							{
								uint32_t debug_info_offset32;
							
								if(max_size < sizeof(debug_info_offset32)) return -1;
								memcpy(&debug_info_offset32, rawdata, sizeof(debug_info_offset32));
								debug_info_offset32 = Target2Host(file_endianness, debug_info_offset32);
								rawdata += sizeof(debug_info_offset32);
								max_size -= sizeof(debug_info_offset32);
								size += sizeof(debug_info_offset32);
								debug_info_offset = debug_info_offset32;
							}
							break;
						case 8:
							{
								uint64_t debug_info_offset64;
							
								if(max_size < sizeof(debug_info_offset64)) return -1;
								memcpy(&debug_info_offset64, rawdata, sizeof(debug_info_offset64));
								debug_info_offset64 = Target2Host(file_endianness, debug_info_offset64);
								rawdata += sizeof(debug_info_offset64);
								max_size -= sizeof(debug_info_offset64);
								size += sizeof(debug_info_offset64);
								debug_info_offset = debug_info_offset64;
							}
							break;
						default:
							return -1;
					}
					DWARF_Reference<MEMORY_ADDR> *dw_ref = new DWARF_Reference<MEMORY_ADDR>(debug_info_offset);
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_ref);
				}
				break;
			case DW_FORM_ref1:
				{
					uint8_t debug_info_offset;
					
					if(max_size < sizeof(debug_info_offset)) return -1;
					memcpy(&debug_info_offset, rawdata, sizeof(debug_info_offset));
					debug_info_offset = Target2Host(file_endianness, debug_info_offset);
					rawdata += sizeof(debug_info_offset);
					max_size -= sizeof(debug_info_offset);
					size += sizeof(debug_info_offset);

					DWARF_Reference<MEMORY_ADDR> *dw_ref = new DWARF_Reference<MEMORY_ADDR>(dw_cu->GetOffset() + (uint64_t) debug_info_offset);
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_ref);
				}
				break;
			case DW_FORM_ref2:
				{
					uint16_t debug_info_offset;
					
					if(max_size < sizeof(debug_info_offset)) return -1;
					memcpy(&debug_info_offset, rawdata, sizeof(debug_info_offset));
					debug_info_offset = Target2Host(file_endianness, debug_info_offset);
					rawdata += sizeof(debug_info_offset);
					max_size -= sizeof(debug_info_offset);
					size += sizeof(debug_info_offset);

					DWARF_Reference<MEMORY_ADDR> *dw_ref = new DWARF_Reference<MEMORY_ADDR>(dw_cu->GetOffset() + (uint64_t) debug_info_offset);
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_ref);
				}
				break;
			case DW_FORM_ref4:
				{
					uint32_t debug_info_offset;
					
					if(max_size < sizeof(debug_info_offset)) return -1;
					memcpy(&debug_info_offset, rawdata, sizeof(debug_info_offset));
					debug_info_offset = Target2Host(file_endianness, debug_info_offset);
					rawdata += sizeof(debug_info_offset);
					max_size -= sizeof(debug_info_offset);
					size += sizeof(debug_info_offset);

					DWARF_Reference<MEMORY_ADDR> *dw_ref = new DWARF_Reference<MEMORY_ADDR>(dw_cu->GetOffset() + (uint64_t) debug_info_offset);
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_ref);
				}
				break;
			case DW_FORM_ref8:
				{
					uint64_t debug_info_offset;
					
					if(max_size < sizeof(debug_info_offset)) return -1;
					memcpy(&debug_info_offset, rawdata, sizeof(debug_info_offset));
					debug_info_offset = Target2Host(file_endianness, debug_info_offset);
					rawdata += sizeof(debug_info_offset);
					max_size -= sizeof(debug_info_offset);
					size += sizeof(debug_info_offset);

					DWARF_Reference<MEMORY_ADDR> *dw_ref = new DWARF_Reference<MEMORY_ADDR>(dw_cu->GetOffset() + debug_info_offset);
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_ref);
				}
				break;
			case DW_FORM_ref_udata:
				{
					DWARF_LEB128 debug_info_offset;
					int64_t sz;
					if((sz = debug_info_offset.Load(rawdata, max_size)) < 0) return -1;
					size += sz;
					rawdata += sz;
					max_size -= sz;

					DWARF_Reference<MEMORY_ADDR> *dw_ref = new DWARF_Reference<MEMORY_ADDR>(dw_cu->GetOffset() + (uint64_t) debug_info_offset);
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_ref);
				}
				break;
			case DW_FORM_sec_offset:
				switch(dw_cu->GetFormat())
				{
					case FMT_DWARF32:
						{
							if(max_size < sizeof(uint32_t)) return -1;
							uint32_t value;
							memcpy(&value, rawdata, sizeof(uint32_t));
							value = Target2Host(file_endianness, value);
							size += sizeof(uint32_t);
							rawdata += sizeof(uint32_t);
							max_size -= sizeof(uint32_t);
							
							DWARF_AttributeValue<MEMORY_ADDR> *dw_value;
							
							switch(dw_at)
							{
								case DW_AT_stmt_list:
									dw_value = new DWARF_LinePtr<MEMORY_ADDR>((uint64_t) value);
									break;
								case DW_AT_ranges:
									dw_value = new DWARF_RangeListPtr<MEMORY_ADDR>(dw_cu, (uint64_t) value);
									break;
								case DW_AT_macro_info:
									dw_value = new DWARF_MacPtr<MEMORY_ADDR>((uint64_t) value);
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
									dw_value = new DWARF_LocListPtr<MEMORY_ADDR>(dw_cu, (uint64_t) value);
									break;
								default:
									dw_value = new DWARF_UnsignedConstant<MEMORY_ADDR>((uint64_t) value);
									break;
							}
							dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_value);
						}
						
						break;
					case FMT_DWARF64:
						{
							if(max_size < sizeof(uint64_t)) return -1;
							uint64_t value;
							memcpy(&value, rawdata, sizeof(uint64_t));
							value = Target2Host(file_endianness, value);
							size += sizeof(uint64_t);
							rawdata += sizeof(uint64_t);
							max_size -= sizeof(uint64_t);

							DWARF_AttributeValue<MEMORY_ADDR> *dw_value;
							
							switch(dw_at)
							{
								case DW_AT_stmt_list:
									dw_value = new DWARF_LinePtr<MEMORY_ADDR>((uint64_t) value);
									break;
								case DW_AT_ranges:
									dw_value = new DWARF_RangeListPtr<MEMORY_ADDR>(dw_cu, (uint64_t) value);
									break;
								case DW_AT_macro_info:
									dw_value = new DWARF_MacPtr<MEMORY_ADDR>((uint64_t) value);
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
									dw_value = new DWARF_LocListPtr<MEMORY_ADDR>(dw_cu, (uint64_t) value);
									break;
								default:
									dw_value = new DWARF_UnsignedConstant<MEMORY_ADDR>((uint64_t) value);
									break;
							}
							dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_value);
						}
						break;
					default:
						return -1;
				}
				break;
				
			case DW_FORM_expr_loc:
				{
					DWARF_LEB128 leb128_info_bytes_length;
					int64_t sz;
					if((sz = leb128_info_bytes_length.Load(rawdata, max_size)) < 0) return -1;
					rawdata += sz;
					size += sz;
					max_size -= sz;

					uint64_t info_bytes_length = (uint64_t) leb128_info_bytes_length;
					if(max_size < info_bytes_length) return -1;

					DWARF_AttributeValue<MEMORY_ADDR> *dw_value;
					
					dw_value = new DWARF_Expression<MEMORY_ADDR>(dw_cu, info_bytes_length, rawdata);
					
					rawdata += info_bytes_length;
					size += info_bytes_length;
					max_size -= info_bytes_length;
					
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_value);
				}
				break;
				
			case DW_FORM_flag_present:
				{
					DWARF_Flag<MEMORY_ADDR> *dw_flag = new DWARF_Flag<MEMORY_ADDR>(true);
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_flag);
				}
				break;
				
			case DW_FORM_ref_sig8:
				debug_warning_stream << "unimplemented form DW_FORM_ref_sig8" << std::endl;
				return -1;
				break;
				
			default:
				debug_warning_stream << "Unknown form 0x" << std::hex << dw_form << std::dec << std::endl;
				return -1;
		}
		
		if(dw_attribute)
		{
			Add(dw_attribute);
		}
	}
	
	if(dw_abbrev->HasChildren())
	{
		DWARF_DIE<MEMORY_ADDR> *dw_die = 0;
		do
		{
			dw_die = new DWARF_DIE<MEMORY_ADDR>(dw_cu, this);
			
			if((sz = dw_die->Load(rawdata, max_size, offset + size)) < 0)
			{
				delete dw_die;
				return -1;
			}
			rawdata += sz;
			max_size -= sz;
			size += sz;
			
			children.push_back(dw_die);
			dw_cu->Register(dw_die);
		}
		while(!dw_die->IsNull());
	}
	
	abbrev = dw_abbrev;
	
	return size;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::IsNull() const
{
	return abbrev == 0;
}

template <class MEMORY_ADDR>
void DWARF_DIE<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int _id)
{
	id = _id;
	unsigned int num_attributes = attributes.size();
	unsigned int i;

	for(i = 0; i < num_attributes; i++)
	{
		attributes[i]->Fix(dw_handler);
	}
}

template <class MEMORY_ADDR>
std::string DWARF_DIE<MEMORY_ADDR>::GetHREF() const
{
	std::stringstream sstr;
	sstr << "debug_info/dies/" << (id / dies_per_file) << ".html#die-" << id;
	return sstr.str();
}

template <class MEMORY_ADDR>
unsigned int DWARF_DIE<MEMORY_ADDR>::GetId() const
{
	return id;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_DIE<MEMORY_ADDR>::to_XML(std::ostream& os)
{
	unsigned int i;

	//os << "<DW_DIE id=\"die-" << id << "\" dw_tag=\"" << DWARF_GetTagName(GetTag()) << "\">" << std::endl;
	os << "<" << DWARF_GetTagName(GetTag()) << " id=\"die-" << id << "\">" << std::endl;
	unsigned int num_attributes = attributes.size();
	for(i = 0; i < num_attributes; i++)
	{
		attributes[i]->to_XML(os) << std::endl;
	}
	unsigned int num_children = children.size();
	for(i = 0; i < num_children; i++)
	{
		children[i]->to_XML(os) << std::endl;
	}
	os << "</" << DWARF_GetTagName(GetTag()) << ">" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_DIE<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	unsigned int i;

	os << "<tr id=\"die-" << id << "\">" << std::endl;
	os << "<td>" << id << "</td><td>" << offset << "</td><td><a href=\"../../" << dw_cu->GetHREF() << "\">cu-" << dw_cu->GetId() << "</a></td><td>";
	if(dw_parent_die)
	{
		os << "<a href=\"../../" << dw_parent_die->GetHREF() << "\">die-" << dw_parent_die->GetId() << "</a>";
	}
	else
	{
		os << "-";
	}
	os << std::endl;
	os << "<td>" << std::endl;
	unsigned int num_children = children.size();
	for(i = 0; i < num_children; i++)
	{
		if(i != 0) os << "&nbsp;";//"<br>";
		os << "<a href=\"../../" << children[i]->GetHREF() << "\">die-" << children[i]->GetId() << "</a>" << std::endl;
	}
	os << "</td>" << std::endl;
	os << "<td>" << std::endl;
	os << DWARF_GetTagName(GetTag()) << "<br>" << std::endl;
	os << "<table>" << std::endl;
	os << "<tr><th>Name</th><th>Class</th><th>Value</th></tr>" << std::endl;
	unsigned int num_attributes = attributes.size();
	for(i = 0; i < num_attributes; i++)
	{
		attributes[i]->to_HTML(os) << std::endl;
	}
	os << "</table>" << std::endl;
	os << "</td>" << std::endl;
	os << "</tr>" << std::endl;
	return os;
}


template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_DIE<MEMORY_ADDR>& dw_die)
{
	unsigned int i;

	os << "   - DIE:" << std::endl;
	os << "     - Offset: " << dw_die.offset;
	unsigned int num_attributes = dw_die.attributes.size();
	if(num_attributes)
	{
		os << std::endl << "     - Attributes:";
		for(i = 0; i < num_attributes; i++)
		{
			os << std::endl << "       - " << *dw_die.attributes[i];
		}
	}
	unsigned int num_children = dw_die.children.size();
	if(num_children)
	{
		os << std::endl << "     - Children DIE:";
		for(i = 0; i < num_children; i++)
		{
			os << std::endl << "  " << *dw_die.children[i];
		}
	}
	return os;
}

template <class MEMORY_ADDR>
void DWARF_DIE<MEMORY_ADDR>::BuildStatementMatrix(std::multimap<MEMORY_ADDR, const Statement<MEMORY_ADDR> *>& stmt_matrix)
{
	unsigned int i;
	unsigned int num_attributes = attributes.size();
	for(i = 0; i < num_attributes; i++)
	{
		DWARF_Attribute<MEMORY_ADDR> *dw_attr = attributes[i];
		
		const DWARF_AbbrevAttribute *dw_abbrev_attr = dw_attr->GetAbbrevAttribute();
		
		uint16_t dw_tag = (uint16_t) dw_abbrev_attr->GetTag();
		if(dw_tag == DW_AT_stmt_list)
		{
			const DWARF_AttributeValue<MEMORY_ADDR> *dw_attr_value = dw_attr->GetValue();
			
			unsigned int dw_class = dw_attr_value->GetClass();
			
			if(dw_class == DW_CLASS_LINEPTR)
			{
				DWARF_LinePtr<MEMORY_ADDR> *dw_line_ptr = (DWARF_LinePtr<MEMORY_ADDR> *) dw_attr_value;
				
				const DWARF_StatementProgram<MEMORY_ADDR> *dw_stmt_prog = dw_line_ptr->GetValue();
				
				DWARF_StatementVM<MEMORY_ADDR> dw_stmt_vm = DWARF_StatementVM<MEMORY_ADDR>(dw_cu->GetHandler());
				if(!dw_stmt_vm.Run(dw_stmt_prog, 0, &stmt_matrix, dw_cu))
				{
					debug_warning_stream << "Invalid DWARF2 statement program. Statement matrix may be incomplete." << std::endl;
				}
			}
		}
	}

	unsigned int num_children = children.size();
	for(i = 0; i < num_children; i++)
	{
		DWARF_DIE<MEMORY_ADDR> *dw_child = children[i];
		
		dw_child->BuildStatementMatrix(stmt_matrix);
	}
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::HasRanges() const
{
	const DWARF_RangeListEntry<MEMORY_ADDR> *range_list_entry = 0;
	if(GetRanges(range_list_entry) && range_list_entry) return true;
	MEMORY_ADDR low_pc = 0;
	if(GetLowPC(low_pc)) return true;
	return false;
// 	return FindAttribute(DW_AT_ranges) || FindAttribute(DW_AT_low_pc);
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetRanges(const DWARF_RangeListEntry<MEMORY_ADDR> *& range_list_entry) const
{
	if(cached_ranges.Valid())
	{
		if(!cached_ranges.Present()) return false;
		range_list_entry = cached_ranges;
		return true;
	}
	
	// DW_AT_ranges is only available in DWARF3 and more
	const DWARF_Attribute<MEMORY_ADDR> *dw_at_ranges = FindAttribute(DW_AT_ranges);
	
	if(dw_at_ranges)
	{
		const DWARF_AttributeValue<MEMORY_ADDR> *dw_at_ranges_value = dw_at_ranges->GetValue();
		if(dw_at_ranges_value->GetClass() == DW_CLASS_RANGELISTPTR)
		{
			const DWARF_RangeListPtr<MEMORY_ADDR> *range_list_ptr = (const DWARF_RangeListPtr<MEMORY_ADDR> *) dw_at_ranges_value;
			
			cached_ranges = range_list_entry = range_list_ptr->GetValue();
			return true;
		}
		
		cached_ranges = 0; // DW_AT_ranges is present but has an unexpected form
		return true;
	}
	
	cached_ranges.Present(false);
	return false;
}

template <class MEMORY_ADDR>
void DWARF_DIE<MEMORY_ADDR>::GetRanges(std::set<std::pair<MEMORY_ADDR, MEMORY_ADDR> >& ranges) const
{
	const DWARF_RangeListEntry<MEMORY_ADDR> *range_list_entry = 0;
	if(GetRanges(range_list_entry))
	{
		if(range_list_entry)
		{
			bool has_base_addr = false;
			bool has_queried_cu_default_base_address = false;
			MEMORY_ADDR base_addr = 0;
			while(range_list_entry && !range_list_entry->IsEndOfList())
			{
				if(range_list_entry->IsBaseAddressSelection())
				{
					base_addr = range_list_entry->GetBaseAddress();
					has_base_addr = true;
				}
				else
				{
					if(!has_base_addr)
					{
						if(!has_queried_cu_default_base_address)
						{
							has_base_addr = dw_cu->GetDefaultBaseAddress(base_addr);
							has_queried_cu_default_base_address = true;
						}
					}
					ranges.insert(std::pair<MEMORY_ADDR, MEMORY_ADDR>(base_addr + range_list_entry->GetBegin(), base_addr + range_list_entry->GetEnd()));
				}
				range_list_entry = range_list_entry->GetNext();
			}
		}
		return;
	}

	MEMORY_ADDR low_pc = 0;
	if(GetLowPC(low_pc))
	{
		MEMORY_ADDR high_pc = 0;
		bool high_pc_is_offset = false;
		if(GetHighPC(high_pc, high_pc_is_offset))
		{
			ranges.insert(std::pair<MEMORY_ADDR, MEMORY_ADDR>(low_pc, high_pc_is_offset ? low_pc + high_pc : high_pc));
		}
		else
		{
			// single address
			ranges.insert(std::pair<MEMORY_ADDR, MEMORY_ADDR>(low_pc, low_pc + 1));
		}
		return;
	}

	if(dw_parent_die) dw_parent_die->GetRanges(ranges);
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::HasOverlap(MEMORY_ADDR addr, MEMORY_ADDR length) const
{
	const DWARF_RangeListEntry<MEMORY_ADDR> *range_list_entry = 0;
	if(GetRanges(range_list_entry))
	{
		bool has_base_addr = false;
		bool has_queried_cu_default_base_address = false;
		MEMORY_ADDR base_addr = 0;
		while(range_list_entry && !range_list_entry->IsEndOfList())
		{
			if(range_list_entry->IsBaseAddressSelection())
			{
				base_addr = range_list_entry->GetBaseAddress();
				has_base_addr = true;
			}
			else
			{
				if(!has_base_addr)
				{
					if(!has_queried_cu_default_base_address)
					{
						has_base_addr = dw_cu->GetDefaultBaseAddress(base_addr);
						has_queried_cu_default_base_address = true;
					}
				}
				if(range_list_entry->HasOverlap(base_addr, addr, length)) return true;
			}
			range_list_entry = range_list_entry->GetNext();
		}
	}

	MEMORY_ADDR low_pc = 0;
	if(GetLowPC(low_pc))
	{
		MEMORY_ADDR high_pc = 0;
		bool high_pc_is_offset = false;
		if(GetHighPC(high_pc, high_pc_is_offset))
		{
			return unisim::util::debug::dwarf::HasOverlapEx(low_pc, high_pc_is_offset ? low_pc + high_pc : high_pc, addr, addr + length);
		}
		else
		{
			// single address
			return (addr == low_pc);
		}
		return false;
	}

	return false;
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_DIE<MEMORY_ADDR>::FindDIEByAddrRange(unsigned int dw_tag, MEMORY_ADDR addr, MEMORY_ADDR length) const
{
	const DWARF_DIE<MEMORY_ADDR> *dw_found_die = 0;
	unsigned int num_children = children.size();
	unsigned int i;
	for(i = 0; i < num_children; i++)
	{
		DWARF_DIE<MEMORY_ADDR> *dw_child = children[i];
		
		dw_found_die = dw_child->FindDIEByAddrRange(dw_tag, addr, length);
		if(dw_found_die) break;
	}

	return dw_found_die ? dw_found_die : (((!dw_tag || (GetTag() == dw_tag)) && HasOverlap(addr, length)) ? this : 0);
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_DIE<MEMORY_ADDR>::FindDIEByName(unsigned int dw_tag, const char *name, bool external) const
{
	if(!dw_tag || (GetTag() == dw_tag))
	{
		bool declaration_flag = false;
		if(GetDeclarationFlag(declaration_flag) && declaration_flag)
		{
			return 0;
		}
		
		bool external_flag;
		if(!external || (GetExternalFlag(external_flag) && external_flag))
		{
			const char *die_name = GetName();
			if(die_name && (strcmp(die_name, name) == 0)) return this;
		}
	}
	
	const DWARF_DIE<MEMORY_ADDR> *dw_found_die = 0;
	unsigned int num_children = children.size();
	unsigned int i;
	for(i = 0; i < num_children; i++)
	{
		DWARF_DIE<MEMORY_ADDR> *dw_child = children[i];
		
		dw_found_die = dw_child->FindDIEByName(dw_tag, name, external);
		if(dw_found_die) return dw_found_die;
	}
	
	return 0;
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_DIE<MEMORY_ADDR>::FindDataObject(const char *name) const
{
	if(GetTag() == DW_TAG_GNU_call_site)
	{
		const DWARF_DIE<MEMORY_ADDR> *dw_at_abstract_origin = GetAbstractOrigin();
		
		if(dw_at_abstract_origin)
		{
			return dw_at_abstract_origin->FindDataObject(name);
		}
		
		if(debug)
		{
			debug_info_stream << "In File \"" << dw_handler->GetFilename() << "\", DIE #" << id << " which is a GNU call site doesn't have an abstract origin" << std::endl;
		}
		
		return 0;
	}
	else if(GetTag() == DW_TAG_inlined_subroutine)
	{
		if(debug)
		{
			debug_info_stream << "In File \"" << dw_handler->GetFilename() << "\", DIE #" << id << " is an inlined subroutine" << std::endl;
		}

		const DWARF_DIE<MEMORY_ADDR> *dw_at_abstract_origin = GetAbstractOrigin();
		
		if(dw_at_abstract_origin)
		{
			return dw_at_abstract_origin->FindDataObject(name);
		}
		
		if(debug)
		{
			debug_info_stream << "In File \"" << dw_handler->GetFilename() << "\", DIE #" << id << " which represents an inlined subroutine doesn't have an abstract origin" << std::endl;
		}
		
		return 0;
	}
	
	unsigned int num_children = children.size();
	unsigned int i;
	for(i = 0; i < num_children; i++)
	{
		DWARF_DIE<MEMORY_ADDR> *dw_child = children[i];

		switch(dw_child->GetTag())
		{
			case DW_TAG_variable:
			case DW_TAG_formal_parameter:
			case DW_TAG_constant:
				{
					const char *child_name = dw_child->GetName();
					if(child_name)
					{
						if(strcmp(child_name, name) == 0)
						{
							if(debug)
							{
								debug_info_stream << "In File \"" << dw_handler->GetFilename() << "\", found DIE #" << dw_child->GetId() << " for data Object \"" << name << "\"" << std::endl;
							}
							return dw_child;
						}
					}
				}
				break;
		}
	}
	
	return 0;
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_DIE<MEMORY_ADDR>::FindDataMember(const char *name) const
{
	unsigned int num_children = children.size();
	unsigned int i;
	for(i = 0; i < num_children; i++)
	{
		DWARF_DIE<MEMORY_ADDR> *dw_child = children[i];

		switch(dw_child->GetTag())
		{
			case DW_TAG_member:
				{
					const char *child_name = dw_child->GetName();
					if(child_name && (*child_name != 0))
					{
						if(strcmp(child_name, name) == 0) return dw_child;
					}
					else
					{
						switch(dw_cu->GetLanguage())
						{
							case DW_LANG_C89:
							case DW_LANG_C:
							case DW_LANG_C99:
							{
								// member is unnamed
								const DWARF_DIE<MEMORY_ADDR> *dw_child_die_type = dw_child->GetTypeDIE();
								if(dw_child_die_type &&
								   ((dw_child_die_type->GetTag() == DW_TAG_structure_type) ||
								   (dw_child_die_type->GetTag() == DW_TAG_union_type)))
								{
									// member type is struct or union
									const char *child_type_name = dw_child_die_type->GetName();
									if(!child_type_name || (*child_type_name == 0)) // struct/union has no name (anonymous)
									{
										// behave as if members of anonymous struct are members of container
										return dw_child_die_type->FindDataMember(name);
									}
								}
								break;
							}
						}
					}
				}
				break;
		}
	}
	
	return 0;
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_DIE<MEMORY_ADDR>::FindSubProgram(const char *name) const
{
	unsigned int num_children = children.size();
	unsigned int i;
	for(i = 0; i < num_children; i++)
	{
		DWARF_DIE<MEMORY_ADDR> *dw_child = children[i];

		switch(dw_child->GetTag())
		{
			case DW_TAG_subprogram:
				{
					if(dw_child->HasRanges())
					{
						const char *child_name = dw_child->GetName();
						if(child_name)
						{
							if(strcmp(child_name, name) == 0) return dw_child;
						}
					}
				}
				break;
		}
	}
	
	return 0;
}

template <class MEMORY_ADDR>
void DWARF_DIE<MEMORY_ADDR>::EnumerateDataObjectNames(std::set<std::string>& name_set) const
{
	if(GetTag() == DW_TAG_inlined_subroutine)
	{
		if(debug)
		{
			debug_info_stream << "In File \"" << dw_handler->GetFilename() << "\", DIE #" << id << " is an inlined subroutine" << std::endl;
		}

		const DWARF_DIE<MEMORY_ADDR> *dw_at_abstract_origin = GetAbstractOrigin();
		
		if(dw_at_abstract_origin)
		{
// 			if(debug)
// 			{
// 				debug_info_stream << "In File \"" << dw_handler->GetFilename() << "\", abstract origin of DIE #" << id << " is DIE #" << dw_at_abstract_origin->GetId() << std::endl;
// 			}

			dw_at_abstract_origin->EnumerateDataObjectNames(name_set);
		}
		
		return;
	}
	
	unsigned int num_children = children.size();
	unsigned int i;
	for(i = 0; i < num_children; i++)
	{
		DWARF_DIE<MEMORY_ADDR> *dw_child = children[i];

		switch(dw_child->GetTag())
		{
			case DW_TAG_variable:
			case DW_TAG_formal_parameter:
			case DW_TAG_constant:
				{
					const char *child_name = dw_child->GetName();
					if(child_name)
					{
						name_set.insert(child_name);
					}
				}
				break;
		}
	}
}

template <class MEMORY_ADDR>
const char *DWARF_DIE<MEMORY_ADDR>::GetName() const
{
	if(cached_name.Valid()) return cached_name;
	
	const DWARF_String<MEMORY_ADDR> *dw_at_name;
	if(GetAttributeValue(DW_AT_name, dw_at_name))
	{
		return cached_name = dw_at_name->GetValue();
	}
	
	bool declaration_flag = false;
	if(GetDeclarationFlag(declaration_flag) && declaration_flag)
	{
		const DWARF_DIE<MEMORY_ADDR> *dw_die_specification = GetSpecification();
		if(dw_die_specification) return cached_name = dw_die_specification->GetName();
	}
	
	const DWARF_DIE<MEMORY_ADDR> *dw_at_abstract_origin = GetAbstractOrigin();
	
	return cached_name = dw_at_abstract_origin ? dw_at_abstract_origin->GetName() : 0;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetLowPC(MEMORY_ADDR& low_pc) const
{
	if(cached_low_pc.Valid())
	{
		if(!cached_low_pc.Present()) return false;
		low_pc = cached_low_pc;
		return true;
	}
	
	const DWARF_Address<MEMORY_ADDR> *dw_at_low_pc;
	if(GetAttributeValue(DW_AT_low_pc, dw_at_low_pc))
	{
		cached_low_pc = low_pc = dw_at_low_pc->GetValue();
		return true;
	}
	
	const DWARF_DIE<MEMORY_ADDR> *dw_at_abstract_origin = GetAbstractOrigin();
	
	if(dw_at_abstract_origin)
	{
		if(dw_at_abstract_origin->GetLowPC(low_pc))
		{
			cached_low_pc = low_pc;
			return true;
		}
	}
	
	cached_low_pc.Present(false);
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetHighPC(MEMORY_ADDR& high_pc, bool& is_offset) const
{
	if(cached_high_pc.Valid())
	{
		if(!cached_high_pc.Present()) return false;
		
		high_pc = cached_high_pc;
		is_offset = false;
		return true;
	}
	
	if(cached_high_pc_offset.Valid())
	{
		if(!cached_high_pc_offset.Present()) return false;

		high_pc = cached_high_pc_offset;
		is_offset = true;
		return true;
	}
	
	const DWARF_Address<MEMORY_ADDR> *dw_at_high_pc_addr;
	if(GetAttributeValue(DW_AT_high_pc, dw_at_high_pc_addr))
	{
		cached_high_pc = high_pc = dw_at_high_pc_addr->GetValue();
		is_offset = false;
		return true;
	}
	
	const DWARF_Constant<MEMORY_ADDR> *dw_at_high_pc_offset;
	if(GetAttributeValue(DW_AT_high_pc, dw_at_high_pc_offset))
	{
		cached_high_pc_offset = high_pc = dw_at_high_pc_offset->to_uint();
		is_offset = true;
		return true;
	}
	
	const DWARF_DIE<MEMORY_ADDR> *dw_at_abstract_origin = GetAbstractOrigin();
	
	if(dw_at_abstract_origin)
	{
		if(dw_at_abstract_origin->GetHighPC(high_pc, is_offset))
		{
			if(is_offset)
			{
				cached_high_pc = high_pc;
			}
			else
			{
				cached_high_pc_offset = high_pc;
			}
			return true;
		}
	}
	
	cached_high_pc.Present(false);
	cached_high_pc_offset.Present(false);
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetContigousAddressRange(MEMORY_ADDR& low_pc, MEMORY_ADDR& high_pc) const
{
	if(!GetLowPC(low_pc)) return false;
	
	bool high_pc_is_offset = false;
	if(!GetHighPC(high_pc, high_pc_is_offset)) return false;
	
	if(high_pc_is_offset) high_pc += low_pc;
	return true;
}

template <class MEMORY_ADDR>
const DWARF_RangeListEntry<MEMORY_ADDR> *DWARF_DIE<MEMORY_ADDR>::GetNonContigousAddressRange() const
{
	// DW_AT_ranges is only available in DWARF3 and more
	const DWARF_RangeListPtr<MEMORY_ADDR> *dw_at_ranges;
	if(GetAttributeValue(DW_AT_ranges, dw_at_ranges))
	{
		return dw_at_ranges->GetValue();
	}
	
	const DWARF_DIE<MEMORY_ADDR> *dw_at_abstract_origin = GetAbstractOrigin();
	
	return dw_at_abstract_origin ? dw_at_abstract_origin->GetNonContigousAddressRange() : 0;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetDefaultBaseAddress(MEMORY_ADDR& base_addr) const
{
	// the default base address for a compilation unit is only available in DWARF3 and more
	return GetLowPC(base_addr);
}

template <class MEMORY_ADDR>
const DWARF_Expression<MEMORY_ADDR> *DWARF_DIE<MEMORY_ADDR>::GetSegment() const
{
	const DWARF_Attribute<MEMORY_ADDR> *dw_at_segment = FindAttribute(DW_AT_segment);
	
	if(dw_at_segment)
	{
		const DWARF_AttributeValue<MEMORY_ADDR> *dw_at_segment_value = dw_at_segment->GetValue();
		if(dw_at_segment_value->GetClass() == DW_CLASS_EXPRESSION)
		{
			return (const DWARF_Expression<MEMORY_ADDR> *) dw_at_segment_value;
		}
	}
	
	const DWARF_DIE<MEMORY_ADDR> *dw_at_abstract_origin = GetAbstractOrigin();
	
	return dw_at_abstract_origin ? dw_at_abstract_origin->GetSegment() : 0;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetCallingConvention(uint8_t& calling_convention) const
{
	const DWARF_Attribute<MEMORY_ADDR> *dw_at_calling_convention = FindAttribute(DW_AT_calling_convention);
	
	if(dw_at_calling_convention)
	{
		const DWARF_AttributeValue<MEMORY_ADDR> *dw_at_calling_convention_value = dw_at_calling_convention->GetValue();
		if(dw_at_calling_convention_value->GetClass() & DW_CLASS_CONSTANT)
		{
			calling_convention = (uint8_t) ((const DWARF_Constant<MEMORY_ADDR> *) dw_at_calling_convention_value)->to_uint();
			return true;
		}
	}
	
	const DWARF_DIE<MEMORY_ADDR> *dw_at_abstract_origin = GetAbstractOrigin();
	
	return dw_at_abstract_origin ? dw_at_abstract_origin->GetCallingConvention(calling_convention) : false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetFrameBase(unsigned int prc_num, MEMORY_ADDR pc, MEMORY_ADDR& frame_base) const
{
	const DWARF_Expression<MEMORY_ADDR> *dw_at_frame_base_loc_expr = 0;
	
	switch(GetTag())
	{
		case DW_TAG_subprogram:
		case DW_TAG_entry_point:
			{
				if(!GetAttributeValue(DW_AT_frame_base, dw_at_frame_base_loc_expr))
				{
					const DWARF_LocListPtr<MEMORY_ADDR> *dw_at_frame_base_loclistptr;
					if(GetAttributeValue(DW_AT_frame_base, dw_at_frame_base_loclistptr))
					{
						const DWARF_LocListEntry<MEMORY_ADDR> *loc_list_entry = dw_at_frame_base_loclistptr->GetValue();
						bool has_base_addr = false;
						bool has_queried_cu_default_base_address = false;
						MEMORY_ADDR base_addr = 0;
						do
						{
							if(loc_list_entry->IsBaseAddressSelection())
							{
								base_addr = loc_list_entry->GetBaseAddress();
								has_base_addr = true;
							}
							else
							{
								if(!has_base_addr)
								{
									if(!has_queried_cu_default_base_address)
									{
										has_base_addr = dw_cu->GetDefaultBaseAddress(base_addr);
										has_queried_cu_default_base_address = true;
									}
								}
								if(loc_list_entry->HasOverlap(base_addr, pc, 1))
								{
									 // found the entry we're looking for
									 dw_at_frame_base_loc_expr = loc_list_entry->GetLocationExpression();
									break;
								}
							}
							if(loc_list_entry->IsEndOfList()) break;
						}
						while((loc_list_entry = loc_list_entry->GetNext()) != 0);
					}
				}
				
				if(dw_at_frame_base_loc_expr)
				{
					DWARF_ExpressionVM<MEMORY_ADDR> loc_expr_vm = DWARF_ExpressionVM<MEMORY_ADDR>(dw_cu->GetHandler(), prc_num);
					DWARF_Location<MEMORY_ADDR> frame_base_loc;
					loc_expr_vm.SetPC(pc);
					bool loc_expr_vm_status = loc_expr_vm.Execute(dw_at_frame_base_loc_expr, frame_base, &frame_base_loc);
					if(!loc_expr_vm_status) return false;
					
					switch(frame_base_loc.GetType())
					{
						case DW_LOC_NULL:
							break;
						case DW_LOC_SIMPLE_MEMORY:
							frame_base = frame_base_loc.GetAddress();
							return true;
						case DW_LOC_SIMPLE_REGISTER:
							{
								unsigned int dw_reg_num = frame_base_loc.GetRegisterNumber();
								unisim::service::interfaces::Register *arch_reg = dw_cu->GetHandler()->GetRegisterNumberMapping(prc_num)->GetArchReg(dw_reg_num);
								MEMORY_ADDR reg_value = 0;
								switch(arch_reg->GetSize())
								{
									case 1:
										{
											uint8_t value = 0;
											arch_reg->GetValue(&value);
											reg_value = value;
										}
										break;
									case 2:
										{
											uint16_t value = 0;
											arch_reg->GetValue(&value);
											reg_value = value;
										}
										break;
									case 4:
										{
											uint32_t value = 0;
											arch_reg->GetValue(&value);
											reg_value = value;
										}
										break;
									case 8:
										{
											uint64_t value = 0;
											arch_reg->GetValue(&value);
											reg_value = value;
										}
										break;
								}
								frame_base = reg_value;
							}
							return true;
						case DW_LOC_COMPOSITE:
							return false;
					}
				}
			}
			break;
	}
	
	return dw_parent_die ? dw_parent_die->GetFrameBase(prc_num, pc, frame_base) : false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetAttributeStaticDynamicValue(unsigned int prc_num, uint16_t dw_at, uint64_t& value) const
{
	const DWARF_Constant<MEMORY_ADDR> *dw_at_const_value = 0;
	if(GetAttributeValue(dw_at, dw_at_const_value))
	{
		value = dw_at_const_value->to_uint();
		return true;
	}
	else
	{
		const DWARF_Expression<MEMORY_ADDR> *dw_at_expr_value = 0;
		if(GetAttributeValue(dw_at, dw_at_expr_value))
		{
			DWARF_ExpressionVM<MEMORY_ADDR> dw_at_expr_value_vm = DWARF_ExpressionVM<MEMORY_ADDR>(dw_cu->GetHandler(), prc_num);
			MEMORY_ADDR address_size_unsigned_value = 0;
			bool dw_at_expr_value_status = dw_at_expr_value_vm.Execute(dw_at_expr_value, address_size_unsigned_value, 0);
			if(dw_at_expr_value_status) value = address_size_unsigned_value;
			return dw_at_expr_value_status;
		}
		else
		{
			const DWARF_Reference<MEMORY_ADDR> *dw_at_ref_value = 0;
			if(GetAttributeValue(dw_at, dw_at_ref_value))
			{
				// For a reference, the value is a reference to another entity which specifies the value of the
				// attribute.
				// This may be interpreted as, "For a reference, the value is a DWARF procedure that computes the attribute value".
				const DWARF_DIE<MEMORY_ADDR> *dw_die_dwarf_procedure = dw_at_ref_value->GetValue();
				
				const DWARF_Expression<MEMORY_ADDR> *dw_dwarf_procedure_compute_expr = 0;
				if(dw_die_dwarf_procedure->GetAttributeValue(DW_AT_location, dw_dwarf_procedure_compute_expr))
				{
					DWARF_ExpressionVM<MEMORY_ADDR> dw_dwarf_procedure_compute_vm = DWARF_ExpressionVM<MEMORY_ADDR>(dw_cu->GetHandler(), prc_num);
					MEMORY_ADDR address_size_unsigned_value = 0;
					bool dw_dwarf_procedure_compute_status = dw_dwarf_procedure_compute_vm.Execute(dw_dwarf_procedure_compute_expr, address_size_unsigned_value, 0);
					if(dw_dwarf_procedure_compute_status) value = address_size_unsigned_value;
					return dw_dwarf_procedure_compute_status;
				}
			}
		}
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetAttributeStaticDynamicValue(unsigned int prc_num, uint16_t dw_at, int64_t& value) const
{
	const DWARF_Constant<MEMORY_ADDR> *dw_at_const_value = 0;
	if(GetAttributeValue(dw_at, dw_at_const_value))
	{
		value = dw_at_const_value->to_int();
		return true;
	}
	else
	{
		const DWARF_Expression<MEMORY_ADDR> *dw_at_expr_value = 0;
		if(GetAttributeValue(dw_at, dw_at_expr_value))
		{
			DWARF_ExpressionVM<MEMORY_ADDR> dw_at_expr_value_vm = DWARF_ExpressionVM<MEMORY_ADDR>(dw_cu->GetHandler(), prc_num);
			MEMORY_ADDR address_size_unsigned_value = 0;
			bool dw_at_expr_value_status = dw_at_expr_value_vm.Execute(dw_at_expr_value, address_size_unsigned_value, 0);
			if(dw_at_expr_value_status)
			{
				value = unisim::util::arithmetic::SignExtend((uint64_t) address_size_unsigned_value, 8 * sizeof(MEMORY_ADDR));
			}
			return dw_at_expr_value_status;
		}
		else
		{
			const DWARF_Reference<MEMORY_ADDR> *dw_at_ref_value = 0;
			if(GetAttributeValue(dw_at, dw_at_ref_value))
			{
				// For a reference, the value is a reference to another entity which specifies the value of the
				// attribute.
				// This may be interpreted as, "For a reference, the value is a DWARF procedure that computes the attribute value".
				const DWARF_DIE<MEMORY_ADDR> *dw_die_dwarf_procedure = dw_at_ref_value->GetValue();
				
				const DWARF_Expression<MEMORY_ADDR> *dw_dwarf_procedure_compute_expr = 0;
				if(dw_die_dwarf_procedure->GetAttributeValue(DW_AT_location, dw_dwarf_procedure_compute_expr))
				{
					DWARF_ExpressionVM<MEMORY_ADDR> dw_dwarf_procedure_compute_vm = DWARF_ExpressionVM<MEMORY_ADDR>(dw_cu->GetHandler(), prc_num);
					MEMORY_ADDR address_size_unsigned_value = 0;
					bool dw_dwarf_procedure_compute_status = dw_dwarf_procedure_compute_vm.Execute(dw_dwarf_procedure_compute_expr, address_size_unsigned_value, 0);
					if(dw_dwarf_procedure_compute_status)
					{
						value = unisim::util::arithmetic::SignExtend((uint64_t) address_size_unsigned_value, 8 * sizeof(MEMORY_ADDR));
					}
					return dw_dwarf_procedure_compute_status;
				}
			}
		}
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetLowerBound(unsigned int prc_num, int64_t& lower_bound) const
{
	if(GetAttributeStaticDynamicValue(prc_num, DW_AT_lower_bound, lower_bound))
	{
		return true;
	}

	const DWARF_DIE<MEMORY_ADDR> *dw_at_abstract_origin = GetAbstractOrigin();
	
	if(dw_at_abstract_origin)
	{
// 		if(debug)
// 		{
// 			debug_info_stream << "In File \"" << dw_handler->GetFilename() << "\", abstract origin of DIE #" << id << " is DIE #" << dw_at_abstract_origin->GetId() << std::endl;
// 		}

		return dw_at_abstract_origin->GetLowerBound(prc_num, lower_bound);
	}
	
	return dw_cu->GetDefaultLowerBound(lower_bound);
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetUpperBound(unsigned int prc_num, int64_t& upper_bound) const
{
	if(GetAttributeStaticDynamicValue(prc_num, DW_AT_upper_bound, upper_bound))
	{
		return true;
	}
	
	const DWARF_DIE<MEMORY_ADDR> *dw_at_abstract_origin = GetAbstractOrigin();
	
	if(dw_at_abstract_origin)
	{
// 		if(debug)
// 		{
// 			debug_info_stream << "In File \"" << dw_handler->GetFilename() << "\", abstract origin of DIE #" << id << " is DIE #" << dw_at_abstract_origin->GetId() << std::endl;
// 		}

		return dw_at_abstract_origin->GetUpperBound(prc_num, upper_bound);
	}
	
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetCount(unsigned int prc_num, uint64_t& count) const
{
	if(GetAttributeStaticDynamicValue(prc_num, DW_AT_count, count)) return true;

	const DWARF_DIE<MEMORY_ADDR> *dw_at_abstract_origin = GetAbstractOrigin();
	
	if(dw_at_abstract_origin)
	{
// 		if(debug)
// 		{
// 			debug_info_stream << "In File \"" << dw_handler->GetFilename() << "\", abstract origin of DIE #" << id << " is DIE #" << dw_at_abstract_origin->GetId() << std::endl;
// 		}
		
		if(dw_at_abstract_origin->GetCount(prc_num, count))
		{
			return true;
		}
	}

	int64_t upper_bound = 0;
	if(GetUpperBound(prc_num, upper_bound))
	{
		int64_t lower_bound = 0;
		if(!GetLowerBound(prc_num, lower_bound))
		{
			if(debug)
			{
				debug_warning_stream << "In File \"" << dw_handler->GetFilename() << "\", can't determine count because can't determine lower bound of DIE #" << id << std::endl;
			}
			return false;
		}
		count = upper_bound - lower_bound + 1;
		
		return true;
	}
	
	const DWARF_DIE<MEMORY_ADDR> *dw_die_type = GetTypeDIE();
	
	if(dw_die_type)
	{
		while(dw_die_type && (dw_die_type->GetTag() == DW_TAG_typedef))
		{
			dw_die_type = dw_die_type->GetTypeDIE();
		}
		
		if(dw_die_type)
		{
			return dw_die_type->GetCount(prc_num, count);
		}
	}
	
	if(debug)
	{
		debug_warning_stream << "In File \"" << dw_handler->GetFilename() << "\", can't determine count of DIE #" << id << std::endl;
	}
	
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetArrayElementCount(unsigned int prc_num, unsigned int dim, uint64_t& count) const
{
	unsigned int num_children = children.size();
	
	if(num_children)
	{
		count = 1;

		uint8_t dw_array_ordering = DW_ORD_row_major;
		if(!GetOrdering(dw_array_ordering))
		{
			dw_array_ordering = dw_cu->GetDefaultOrdering();
		}
		unsigned int i = 0;
		unsigned int subrange_num = 0;
		
		switch(dw_array_ordering)
		{
			case DW_ORD_row_major:
				i = 0;
				break;
			case DW_ORD_col_major:
				i = num_children - 1;
				break;
		}
		do
		{
			const DWARF_DIE<MEMORY_ADDR> *child = children[i];
			
			if(child->GetTag() == DW_TAG_subrange_type)
			{
				if(subrange_num >= dim)
				{
					uint64_t subrange_count = 0;
					if(!child->GetCount(prc_num, subrange_count)) return false;
					
					count *= subrange_count;
				}
				subrange_num++;
			}
			
			switch(dw_array_ordering)
			{
				case DW_ORD_row_major:
					if(++i < num_children) continue;
					break;
				case DW_ORD_col_major:
					if(i-- > 0) continue;
					break;
			}
			break;
		}
		while(1);
		
		return true;
	}
	
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetByteSize(unsigned int prc_num, uint64_t& byte_size) const
{
	if(GetAttributeStaticDynamicValue(prc_num, DW_AT_byte_size, byte_size)) return true;
	
	const DWARF_DIE<MEMORY_ADDR> *dw_at_abstract_origin = GetAbstractOrigin();
	
	if(dw_at_abstract_origin)
	{
		if(dw_at_abstract_origin->GetByteSize(prc_num, byte_size))
		{
			return true;
		}
	}
	
	// the case of the arrays
	switch(GetTag())
	{
		case DW_TAG_array_type:
		case DW_TAG_enumeration_type:
			{
				uint64_t count = 0;
				if(GetArrayElementCount(prc_num, 0, count))
				{
					uint64_t array_element_bit_size = 0;
					if(GetArrayElementPaddedBitSize(prc_num, array_element_bit_size))
					{
						byte_size = ((count * array_element_bit_size) + 7) / 8;
						return true;
					}
				}
			}
			return false;
	}
	
	// follow typedefs
	const DWARF_DIE<MEMORY_ADDR> *dw_die_type = GetTypeDIE();
	
	if(dw_die_type)
	{
		while(dw_die_type && (dw_die_type->GetTag() == DW_TAG_typedef))
		{
			dw_die_type = dw_die_type->GetTypeDIE();
		}
		
		if(dw_die_type)
		{
			return dw_die_type->GetByteSize(prc_num, byte_size);
		}
	}

	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetBitSize(unsigned int prc_num, uint64_t& bit_size) const
{
	if(GetAttributeStaticDynamicValue(prc_num, DW_AT_bit_size, bit_size)) return true;

	const DWARF_DIE<MEMORY_ADDR> *dw_at_abstract_origin = GetAbstractOrigin();
	
	if(dw_at_abstract_origin)
	{
		if(dw_at_abstract_origin->GetBitSize(prc_num, bit_size))
		{
			return true;
		}
	}
	
	switch(GetTag())
	{
		case DW_TAG_array_type:
			{
				uint64_t count = 0;
				if(GetArrayElementCount(prc_num, 0, count))
				{
					uint64_t array_element_bit_size = 0;
					if(GetArrayElementPaddedBitSize(prc_num, array_element_bit_size))
					{
						bit_size = count * array_element_bit_size;
						return true;
					}
				}
			}
			return false;
		case DW_TAG_pointer_type:
			break;
		default:
			{
				// follow typedefs (but pointer)
				const DWARF_DIE<MEMORY_ADDR> *dw_die_type = GetTypeDIE();
				
				if(dw_die_type)
				{
					while(dw_die_type && (dw_die_type->GetTag() == DW_TAG_typedef))
					{
						dw_die_type = dw_die_type->GetTypeDIE();
					}
					
					if(dw_die_type)
					{
						return dw_die_type->GetBitSize(prc_num, bit_size);
					}
				}
			}
			break;
	}

	// fallback
	uint64_t byte_size = 0;
	if(GetByteSize(prc_num, byte_size))
	{
		bit_size = 8 * byte_size;
		return true;
	}

	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetArrayElementPaddedBitSize(unsigned int prc_num, uint64_t& array_element_bit_size) const
{
	if(GetBitStride(prc_num, array_element_bit_size))
	{
		return true;
	}
	
	const DWARF_DIE<MEMORY_ADDR> *dw_at_abstract_origin = GetAbstractOrigin();
	
	if(dw_at_abstract_origin)
	{
		return dw_at_abstract_origin->GetArrayElementPaddedBitSize(prc_num, array_element_bit_size);
	}
	
	const DWARF_DIE<MEMORY_ADDR> *dw_array_element_type = GetTypeDIE();
	
	if(dw_array_element_type)
	{
		uint64_t array_element_byte_size = 0;
		
		if(dw_array_element_type->GetByteSize(prc_num, array_element_byte_size))
		{
			array_element_bit_size = 8 * array_element_byte_size;
			return true;
		}
	}
	
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetArrayElementEncoding(uint8_t& encoding) const
{
	const DWARF_DIE<MEMORY_ADDR> *dw_array_element_type = GetTypeDIE();
	
	return dw_array_element_type ? dw_array_element_type->GetEncoding(encoding) : false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetBitOffset(unsigned int prc_num, int64_t& bit_offset) const
{
	if(GetAttributeStaticDynamicValue(prc_num, DW_AT_bit_offset, bit_offset)) return true;
	
	const DWARF_DIE<MEMORY_ADDR> *dw_at_abstract_origin = GetAbstractOrigin();
	
	return dw_at_abstract_origin ? dw_at_abstract_origin->GetBitOffset(prc_num, bit_offset) : false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetBitStride(unsigned int prc_num, uint64_t& bit_stride) const
{
	if(GetAttributeStaticDynamicValue(prc_num, DW_AT_bit_stride /* = DW_AT_stride_size */, bit_stride)) return true;
	uint64_t byte_stride = 0;
	if(GetAttributeStaticDynamicValue(prc_num, DW_AT_byte_stride, byte_stride))
	{
		bit_stride = byte_stride * 8;
		return true;
	}
	
	const DWARF_DIE<MEMORY_ADDR> *dw_at_abstract_origin = GetAbstractOrigin();
	
	return dw_at_abstract_origin ? dw_at_abstract_origin->GetBitStride(prc_num, bit_stride) : false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetDataBitOffset(unsigned int prc_num, int64_t& data_bit_offset) const
{
	if(GetAttributeStaticDynamicValue(prc_num, DW_AT_data_bit_offset, data_bit_offset)) return true;
	
	const DWARF_DIE<MEMORY_ADDR> *dw_at_abstract_origin = GetAbstractOrigin();
	
	return dw_at_abstract_origin ? dw_at_abstract_origin->GetDataBitOffset(prc_num, data_bit_offset) : false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetLocationExpression(uint16_t dw_at, MEMORY_ADDR pc, const DWARF_Expression<MEMORY_ADDR> * & p_dw_loc_expr, std::set<std::pair<MEMORY_ADDR, MEMORY_ADDR> >& ranges) const
{
	if(GetAttributeValue(dw_at, p_dw_loc_expr))
	{
		ranges.clear();
		GetRanges(ranges);
		return true;
	}

	const DWARF_LocListPtr<MEMORY_ADDR> *dw_loc_list_ptr = 0;
	if(!GetAttributeValue(dw_at, dw_loc_list_ptr))
	{
		if(debug)
		{
			debug_info_stream << "In File \"" << dw_handler->GetFilename() << "\", can't find either a simple location expression or location list of DIE #" << id << std::endl; 
		}
		
		const DWARF_DIE<MEMORY_ADDR> *dw_at_abstract_origin = GetAbstractOrigin();
		
		return dw_at_abstract_origin ? dw_at_abstract_origin->GetLocationExpression(dw_at, pc, p_dw_loc_expr, ranges) : false;
	}
	const DWARF_LocListEntry<MEMORY_ADDR> *dw_loc_list_entry;
	
	bool has_base_addr = false;
	bool has_queried_cu_default_base_address = false;
	MEMORY_ADDR base_addr = 0;
	for(dw_loc_list_entry = dw_loc_list_ptr->GetValue(); dw_loc_list_entry && !dw_loc_list_entry->IsEndOfList(); dw_loc_list_entry = dw_loc_list_entry->GetNext())
	{
		if(dw_loc_list_entry->IsBaseAddressSelection())
		{
			base_addr = dw_loc_list_entry->GetBaseAddress();
			has_base_addr = true;
		}
		else
		{
			if(!has_base_addr)
			{
				if(!has_queried_cu_default_base_address)
				{
					has_base_addr = dw_cu->GetDefaultBaseAddress(base_addr);
					has_queried_cu_default_base_address = true;
				}
			}
			if(dw_loc_list_entry->HasOverlap(base_addr, pc, 1))
			{
				// found DWARF location expression that applies at PC
				p_dw_loc_expr = dw_loc_list_entry->GetLocationExpression();
				
				// restrict ranges
				ranges.clear();
				ranges.insert(std::pair<MEMORY_ADDR, MEMORY_ADDR>(base_addr + dw_loc_list_entry->GetBegin(), base_addr + dw_loc_list_entry->GetEnd()));
				return true;
			}
		}
	}
	
	if(debug)
	{
		debug_info_stream << "In File \"" << dw_handler->GetFilename() << "\", no location expression from DIE #" << id << " location list match PC=0x" << std::hex << pc << std::dec << std::endl; 
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetLocation(unsigned int prc_num, MEMORY_ADDR pc, bool has_frame_base, MEMORY_ADDR frame_base, DWARF_Location<MEMORY_ADDR>& loc) const
{
	const DWARF_Expression<MEMORY_ADDR> *dw_loc_expr = 0;
	
	bool declaration_flag = false;
	if(!GetDeclarationFlag(declaration_flag))
	{
		declaration_flag = false;
	}
	if(declaration_flag)
	{
		// non-defining DIE
		const char *name = GetName();
		
		if(!name)
		{
			if(debug)
			{
				debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", non-defining DIE #" << id << " don't have a name" << std::endl;
			}
			return false;
		}
		
		const DWARF_DIE<MEMORY_ADDR> *dw_defining_die = 0;
		if(cached_dw_defining_die.Valid())
		{
			dw_defining_die = cached_dw_defining_die;
		}
		else
		{
			dw_defining_die = dw_cu->FindDIEByName(GetTag(), name, false);
		
			if(!dw_defining_die) dw_defining_die = dw_handler->FindDIEByName(GetTag(), name, true);
			
			cached_dw_defining_die = dw_defining_die;
		}
		
		if(!dw_defining_die)
		{
			if(debug)
			{
				debug_warning_stream << "In File \"" << dw_handler->GetFilename() << "\", can't get defining DIE of DIE #" << id << " for PC=0x" << std::hex << pc << std::dec << std::endl;
			}
			
			bool external_flag = false;
			if((GetTag() == DW_TAG_variable) && (GetExternalFlag(external_flag) && external_flag))
			{
				const unisim::util::debug::SymbolTable<MEMORY_ADDR> *symbol_table = dw_handler->GetSymbolTable();
				if(symbol_table)
				{
					const unisim::util::debug::Symbol<MEMORY_ADDR> *symbol = symbol_table->FindSymbolByName(name, unisim::util::debug::Symbol<MEMORY_ADDR>::SYM_OBJECT);
					if(symbol)
					{
						if(debug)
						{
							debug_info_stream << "In File \"" << dw_handler->GetFilename() << "\", found Symbol \"" << symbol->GetName() << "\" corresponding to DIE #" << id << std::endl;
						}
						loc.Clear();
						loc.SetAddress(symbol->GetAddress());
						loc.SetByteSize(symbol->GetSize());
						GetRanges(loc.GetRanges());
						return true;
					}
					
					debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", symbol \"" << name << "\" not found" << std::endl;
					return false;
				}

				debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", no symbol table" << std::endl;
				return false;
			}
			
			return false;
		}
		
		if(debug)
		{
			debug_info_stream << "In File \"" << dw_handler->GetFilename() << "\", defining DIE #" << dw_defining_die->GetId() << " for non-defining DIE #" << id << std::endl;
		}
		
		return dw_defining_die->GetLocation(prc_num, pc, has_frame_base, frame_base, loc);
	}

	if(!GetLocationExpression(DW_AT_location, pc, dw_loc_expr, loc.GetRanges()))
	{
		// optimized out
		loc.Clear();

		if(debug)
		{
			debug_info_stream << "In File \"" << dw_handler->GetFilename() << "\", can't get location expression of DIE #" << id << " for PC=0x" << std::hex << pc << std::dec << std::endl;
		}
		return true;
	}

	if(dw_loc_expr->IsEmpty())
	{
		loc.Clear();
		return true; // optimized out
	}
	
	MEMORY_ADDR addr;
	DWARF_ExpressionVM<MEMORY_ADDR> dw_loc_expr_vm = DWARF_ExpressionVM<MEMORY_ADDR>(dw_cu->GetHandler(), prc_num);
	if(has_frame_base) dw_loc_expr_vm.SetFrameBase(frame_base);
	dw_loc_expr_vm.SetPC(pc);
	bool dw_loc_expr_vm_status = dw_loc_expr_vm.Execute(dw_loc_expr, addr, &loc);
	if(!dw_loc_expr_vm_status)
	{
		if(debug)
		{
			debug_info_stream << "In File \"" << dw_handler->GetFilename() << "\", evaluation of DIE #" << id << " location expression failed" << std::endl;
		}
	}
	
	// Determine the size in bytes (including padding bits)
	uint64_t dw_byte_size;
	if(!GetByteSize(prc_num, dw_byte_size))
	{
		if(debug)
		{
			debug_info_stream << "In File \"" << dw_handler->GetFilename() << "\", can't determine byte size (with padding) of data object for DIE #" << id << std::endl;
		}
	}
	loc.SetByteSize(dw_byte_size);
	
	// Determine the actual size in bits (excluding padding bits)
	uint64_t dw_bit_size = 0;
	if(!GetBitSize(prc_num, dw_bit_size))
	{
		if(debug)
		{
			debug_info_stream << "In File \"" << dw_handler->GetFilename() << "\", can't determine bit size of data object for DIE #" << id << std::endl;
		}
	}
	loc.SetBitSize(dw_bit_size);

	const DWARF_DIE<MEMORY_ADDR> *dw_die_type = GetTypeDIE();
	uint8_t dw_at_encoding = 0;
	
	if(!dw_die_type || !dw_die_type->GetEncoding(dw_at_encoding))
	{
		dw_at_encoding = 0;
	}
	loc.SetEncoding(dw_at_encoding);

	return true;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetDataMemberLocation(unsigned int prc_num, MEMORY_ADDR pc, bool has_frame_base, MEMORY_ADDR frame_base, DWARF_Location<MEMORY_ADDR>& loc) const
{
	uint64_t dw_byte_size = 0;
	if(!GetByteSize(prc_num, dw_byte_size))
	{
		if(debug)
		{
			debug_info_stream << "In File \"" << dw_handler->GetFilename() << "\", can't determine byte size (with padding) of data member for DIE #" << id << std::endl;
		}
	}

	uint64_t dw_bit_size = 0;
	if(!GetBitSize(prc_num, dw_bit_size))
	{
		if(debug)
		{
			debug_info_stream << "In File \"" << dw_handler->GetFilename() << "\", can't determine bit size of data member for DIE #" << id << std::endl;
		}
	}
	
	const DWARF_Constant<MEMORY_ADDR> *dw_at_const_value = 0;
	if(GetAttributeValue(DW_AT_data_member_location, dw_at_const_value))
	{
		// constant offset from beginning of data object
		MEMORY_ADDR offset = dw_at_const_value->to_uint();
		loc.IncBitOffset(offset * 8);
	}
	else
	{
		// location expression
		const DWARF_Expression<MEMORY_ADDR> *dw_loc_expr = 0;
		if(GetLocationExpression(DW_AT_data_member_location, pc, dw_loc_expr, loc.GetRanges()))
		{
			if(dw_loc_expr->IsEmpty())
			{
				// optimized out
				loc.Clear();
				return true;
			}
			
			MEMORY_ADDR addr;
			DWARF_ExpressionVM<MEMORY_ADDR> dw_loc_expr_vm = DWARF_ExpressionVM<MEMORY_ADDR>(dw_cu->GetHandler(), prc_num);
			if(has_frame_base) dw_loc_expr_vm.SetFrameBase(frame_base);
			dw_loc_expr_vm.SetPC(pc);
			if(loc.GetType() == DW_LOC_SIMPLE_MEMORY)
			{
				MEMORY_ADDR object_addr = loc.GetAddress();
				dw_loc_expr_vm.SetObjectAddress(object_addr);
				dw_loc_expr_vm.Push(object_addr);
			}
			loc.Clear();
			bool dw_loc_expr_vm_status = dw_loc_expr_vm.Execute(dw_loc_expr, addr, &loc);
			if(!dw_loc_expr_vm_status)
			{
				if(debug)
				{
					debug_error_stream << "In File \"" << dw_handler->GetFilename() << "\", evaluation of data member DIE #" << id << " location expression failed" << std::endl;
				}
				return false;
			}
		}
		else
		{
			int64_t dw_bit_offset = 0;
			int64_t dw_data_member_bit_offset = 0;
			if(GetBitOffset(prc_num, dw_data_member_bit_offset))
			{
				dw_bit_offset = (dw_handler->GetArchEndianness() == unisim::util::endian::E_BIG_ENDIAN) ? dw_data_member_bit_offset : (8 * dw_byte_size) - dw_bit_size - dw_data_member_bit_offset;
			}
			else if(!GetDataBitOffset(prc_num, dw_bit_offset))
			{
				dw_bit_offset = 0;
			}
			loc.IncBitOffset(dw_bit_offset);
		}
	}
	
	loc.SetByteSize(dw_byte_size);
	loc.SetBitSize(dw_bit_size);
	
	const DWARF_DIE<MEMORY_ADDR> *dw_die_type = GetTypeDIE();
	uint8_t dw_at_encoding = 0;
	
	if(!dw_die_type || !dw_die_type->GetEncoding(dw_at_encoding))
	{
		dw_at_encoding = 0;
	}
	loc.SetEncoding(dw_at_encoding);

	return true;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetExternalFlag(bool& external_flag) const
{
	if(cached_external_flag.Valid())
	{
		if(!cached_external_flag.Present()) return false;
		
		external_flag = cached_external_flag;
		return true;
	}
	
	const DWARF_Flag<MEMORY_ADDR> *dw_at_external_value = 0;
	if(GetAttributeValue(DW_AT_external, dw_at_external_value))
	{
		cached_external_flag = external_flag = dw_at_external_value->GetValue();
		return true;
	}
	
	const DWARF_DIE<MEMORY_ADDR> *dw_at_abstract_origin = GetAbstractOrigin();
	
	if(dw_at_abstract_origin)
	{
		if(dw_at_abstract_origin->GetExternalFlag(external_flag))
		{
			cached_external_flag = external_flag;
			return true;
		}
	}
	
	cached_external_flag.Present(false);
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetDeclarationFlag(bool& declaration_flag) const
{
	if(cached_declaration_flag.Valid())
	{
		if(!cached_declaration_flag.Present()) return false;
		
		declaration_flag = cached_declaration_flag;
		return true;
	}
	
	const DWARF_Flag<MEMORY_ADDR> *dw_at_declaration_value = 0;
	if(GetAttributeValue(DW_AT_declaration, dw_at_declaration_value))
	{
		cached_declaration_flag = declaration_flag = dw_at_declaration_value->GetValue();
		return true;
	}
	
	const DWARF_DIE<MEMORY_ADDR> *dw_at_abstract_origin = GetAbstractOrigin();
	
	if(dw_at_abstract_origin)
	{
		if(dw_at_abstract_origin->GetDeclarationFlag(declaration_flag))
		{
			cached_declaration_flag = declaration_flag;
			return true;
		}
	}
	
	cached_declaration_flag.Present(false);
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetOrdering(uint8_t& ordering) const
{
	const DWARF_Constant<MEMORY_ADDR> *dw_at_ordering_value = 0;
	if(GetAttributeValue(DW_AT_ordering, dw_at_ordering_value))
	{
		ordering = dw_at_ordering_value->to_uint();
		return true;
	}
	
	const DWARF_DIE<MEMORY_ADDR> *dw_at_abstract_origin = GetAbstractOrigin();
	
	return dw_at_abstract_origin ? dw_at_abstract_origin->GetOrdering(ordering) : false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetEncoding(uint8_t& encoding) const
{
	if(GetTag() == DW_TAG_base_type)
	{
		const DWARF_Constant<MEMORY_ADDR> *dw_at_encoding_value = 0;
		if(GetAttributeValue(DW_AT_encoding, dw_at_encoding_value))
		{
			encoding = dw_at_encoding_value->to_uint();
			return true;
		}
	}
	
	const DWARF_DIE<MEMORY_ADDR> *dw_at_abstract_origin = GetAbstractOrigin();
	
	if(dw_at_abstract_origin)
	{
		if(dw_at_abstract_origin->GetEncoding(encoding))
		{
			return true;
		}
	}
	
	const DWARF_DIE<MEMORY_ADDR> *dw_die_type = GetTypeDIE();
	
	if(dw_die_type)
	{
		while(dw_die_type && (dw_die_type->GetTag() == DW_TAG_typedef))
		{
			dw_die_type = dw_die_type->GetTypeDIE();
		}
		
		if(dw_die_type)
		{
			return dw_die_type->GetEncoding(encoding);
		}
	}
	
	return false;
}

template <class MEMORY_ADDR>
unsigned int DWARF_DIE<MEMORY_ADDR>::GetSubRangeCount() const
{
	unsigned int subrange_count = 0;
	unsigned int num_children = children.size();
	unsigned int i;
	for(i = 0; i < num_children; i++)
	{
		if(children[i]->GetTag() == DW_TAG_subrange_type) subrange_count++;
	}
	
	return subrange_count;
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_DIE<MEMORY_ADDR>::GetAbstractOrigin() const
{
	if(cached_dw_abstract_origin.Valid()) return cached_dw_abstract_origin;
	
	const DWARF_Reference<MEMORY_ADDR> *dw_at_abstract_origin_ref = 0;
	if(!GetAttributeValue(DW_AT_abstract_origin, dw_at_abstract_origin_ref)) return cached_dw_abstract_origin = 0;
	
	return cached_dw_abstract_origin = dw_at_abstract_origin_ref->GetValue();
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_DIE<MEMORY_ADDR>::GetSpecification() const
{
	if(cached_specification.Valid()) return cached_specification;
	
	const DWARF_Reference<MEMORY_ADDR> *dw_at_specification_ref = 0;
	if(GetAttributeValue(DW_AT_specification, dw_at_specification_ref))
	{
		const DWARF_DIE<MEMORY_ADDR> *dw_at_specification = dw_at_specification_ref->GetValue();
		
		if(debug)
		{
			debug_info_stream << "In File \"" << dw_handler->GetFilename() << "\", specification of DIE #" << id << " is DIE #" << dw_at_specification->GetId() << std::endl;
		}
		
		return cached_specification = dw_at_specification;
	}
	
	const DWARF_DIE<MEMORY_ADDR> *dw_at_abstract_origin = GetAbstractOrigin();
	
	return cached_specification = dw_at_abstract_origin ? dw_at_abstract_origin->GetSpecification() : 0;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetConstValue(int64_t& const_value) const
{
	const DWARF_Constant<MEMORY_ADDR> *dw_at_const_value = 0;
	if(GetAttributeValue(DW_AT_const_value, dw_at_const_value) && dw_at_const_value->IsSigned())
	{
		const_value = dw_at_const_value->to_int();
		return true;
	}
	
	const DWARF_DIE<MEMORY_ADDR> *dw_at_abstract_origin = GetAbstractOrigin();
	
	return dw_at_abstract_origin ? dw_at_abstract_origin->GetConstValue(const_value) : 0;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetConstValue(uint64_t& const_value) const
{
	const DWARF_Constant<MEMORY_ADDR> *dw_at_const_value = 0;
	if(GetAttributeValue(DW_AT_const_value, dw_at_const_value) && !dw_at_const_value->IsSigned())
	{
		const_value = dw_at_const_value->to_int();
		return true;
	}
	
	const DWARF_DIE<MEMORY_ADDR> *dw_at_abstract_origin = GetAbstractOrigin();
	
	return dw_at_abstract_origin ? dw_at_abstract_origin->GetConstValue(const_value) : 0;
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_DIE<MEMORY_ADDR>::GetTypeDIE() const
{
	if(cached_dw_type_die.Valid()) return cached_dw_type_die;
	
	const DWARF_Reference<MEMORY_ADDR> *dw_type_ref = 0;
	if(GetAttributeValue(DW_AT_type, dw_type_ref))
	{
		return cached_dw_type_die = dw_type_ref->GetValue();
	}
	
	const DWARF_DIE<MEMORY_ADDR> *dw_at_abstract_origin = GetAbstractOrigin();
	
	return cached_dw_type_die = dw_at_abstract_origin ? dw_at_abstract_origin->GetTypeDIE() : 0;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetInlineCode(uint8_t& inline_code) const
{
	const DWARF_UnsignedConstant<MEMORY_ADDR> *dw_at_inline = 0;
	if(GetAttributeValue(DW_AT_inline, dw_at_inline))
	{
		inline_code = dw_at_inline->to_uint();
		return true;
	}
	
	const DWARF_DIE<MEMORY_ADDR> *dw_at_abstract_origin = GetAbstractOrigin();
	
	return dw_at_abstract_origin ? dw_at_abstract_origin->GetInlineCode(inline_code) : 0;
}

template <class MEMORY_ADDR>
const unisim::util::debug::Type *DWARF_DIE<MEMORY_ADDR>::GetType(unsigned int prc_num, bool following_pointer, unsigned int array_dim) const
{
	uint16_t dw_tag = GetTag();
	switch(dw_tag)
	{
		case DW_TAG_base_type:
		{
			uint8_t dw_at_encoding = 0;
			if(!GetEncoding(dw_at_encoding))
			{
				dw_at_encoding = 0;
			}
			
			uint64_t bit_size = 0;
			if(!GetBitSize(prc_num, bit_size))
			{
				bit_size = 0;
			}
			
			for(typename TypeCache::const_iterator it = type_cache.begin(); it != type_cache.end(); ++it)
			{
				unisim::util::debug::BaseType const *cached_type = dynamic_cast<unisim::util::debug::BaseType const *>(*it);
				if(cached_type && (cached_type->GetBitSize() == bit_size)) return cached_type;
			}
			
			unisim::util::debug::BaseType const *base_type = 0;
			
			switch(dw_at_encoding)
			{
				case DW_ATE_boolean:
					base_type = new unisim::util::debug::BooleanType(GetName(), bit_size);
					break;
					
				case DW_ATE_float:
					base_type = new unisim::util::debug::FloatingPointType(GetName(), bit_size);
					break;
					
				case DW_ATE_signed:
					base_type = new unisim::util::debug::IntegerType(GetName(), bit_size, true);
					break;
					
				case DW_ATE_signed_char:
					base_type =  new unisim::util::debug::CharType(GetName(), bit_size, true);
					break;
					
				case DW_ATE_unsigned:
					base_type =  new unisim::util::debug::IntegerType(GetName(), bit_size, false);
					break;
					
				case DW_ATE_unsigned_char:
					base_type =  new unisim::util::debug::CharType(GetName(), bit_size, false);
					break;
					
				default:
					base_type = new unisim::util::debug::BaseType(GetName(), bit_size);
					break;
			}
			
			type_cache.push_back(base_type);
			base_type->Catch();
			return base_type;
		}
		
		case DW_TAG_structure_type:
		case DW_TAG_union_type:
		case DW_TAG_class_type:
		case DW_TAG_interface_type:
			{
				uint64_t byte_size = 0;
				bool declaration_flag = false;
				bool incomplete = (following_pointer && GetName()) || (!GetByteSize(prc_num, byte_size) && GetDeclarationFlag(declaration_flag) && declaration_flag);
				
				for(typename TypeCache::const_iterator it = type_cache.begin(); it != type_cache.end(); ++it)
				{
					unisim::util::debug::CompositeType const *cached_type = dynamic_cast<unisim::util::debug::CompositeType const *>(*it);
					if(cached_type && (cached_type->IsIncomplete() == incomplete)) return cached_type;
				}
				
				unisim::util::debug::CompositeType *comp_type = 0;

				switch(dw_tag)
				{
					case DW_TAG_structure_type:
						comp_type = new unisim::util::debug::StructureType(GetName(), incomplete);
						break;
					case DW_TAG_union_type:
						comp_type = new unisim::util::debug::UnionType(GetName(), incomplete);
						break;
					case DW_TAG_class_type:
						comp_type = new unisim::util::debug::ClassType(GetName(), incomplete);
						break;
					case DW_TAG_interface_type:
						comp_type = new unisim::util::debug::InterfaceType(GetName(), incomplete);
						break;
				}
				
				if(!incomplete)
				{
					unsigned int num_children = children.size();
					unsigned int i;
					for(i = 0; i < num_children; i++)
					{
						DWARF_DIE<MEMORY_ADDR> *dw_child = children[i];

						switch(dw_child->GetTag())
						{
							case DW_TAG_member:
								{
									const DWARF_DIE<MEMORY_ADDR> *dw_die_member = dw_child;
									const char *member_name = dw_die_member->GetName();
// 									if(member_name)
// 									{
										const DWARF_DIE<MEMORY_ADDR> *dw_die_member_type = dw_die_member->GetTypeDIE();
										if(dw_die_member_type)
										{
											uint64_t bit_size = 0;
											if(!dw_die_member_type->GetAttributeStaticDynamicValue(prc_num, DW_AT_bit_size, bit_size))
											{
												const DWARF_DIE<MEMORY_ADDR> *dw_die_member_type_at_abstract_origin = dw_die_member_type->GetAbstractOrigin();
												
												while(dw_die_member_type_at_abstract_origin && !dw_die_member_type_at_abstract_origin->GetAttributeStaticDynamicValue(prc_num, DW_AT_bit_size, bit_size))
												{
													dw_die_member_type_at_abstract_origin = dw_die_member_type_at_abstract_origin->GetAbstractOrigin();
												}
											}
											
											comp_type->Add(new unisim::util::debug::Member(member_name, dw_die_member_type ? dw_die_member_type->GetType(prc_num) : new unisim::util::debug::Type(), bit_size));
										}
// 									}
								}
								break;
						}
					}
				}
				
				type_cache.push_back(comp_type);
				comp_type->Catch();
				return comp_type;
			}
			break;
		case DW_TAG_array_type:
			{
				const DWARF_DIE<MEMORY_ADDR> *dw_die_element_type = GetTypeDIE();
				
				if(dw_die_element_type)
				{
					unisim::util::debug::ArrayType const *array_type = 0;
				
					uint8_t dw_array_ordering = DW_ORD_row_major;
					if(!GetOrdering(dw_array_ordering))
					{
						dw_array_ordering = dw_cu->GetDefaultOrdering();
					}
					
					unsigned int subrange_count = GetSubRangeCount();

					unsigned int num_children = children.size();
					unsigned int dim = subrange_count - 1;
					unsigned int i = 0;
					
					switch(dw_array_ordering)
					{
						case DW_ORD_row_major:
							i = num_children - 1;
							break;
						case DW_ORD_col_major:
							i = 0;
							break;
					}
					do
					{
						DWARF_DIE<MEMORY_ADDR> *dw_child = children[i];

						switch(dw_child->GetTag())
						{
							case DW_TAG_subrange_type:
								{
									if(dim >= array_dim)
									{
										const DWARF_DIE<MEMORY_ADDR> *dw_die_subrange_type = dw_child;
										int64_t lower_bound = 0;
										if(!dw_die_subrange_type->GetLowerBound(prc_num, lower_bound))
										{
											lower_bound = 0;
										}
										
										int64_t upper_bound = 0;
										uint64_t count = 0;
										if(!dw_die_subrange_type->GetUpperBound(prc_num, upper_bound))
										{
											if(dw_die_subrange_type->GetCount(prc_num, count))
											{
												upper_bound = lower_bound + count - 1;
											}
											else
											{
												upper_bound = lower_bound - 1; // null array
											}
										}
										
										if(array_type)
										{
											bool cached = false;
											for(typename TypeCache::const_iterator it = type_cache.begin(); it != type_cache.end(); ++it)
											{
												unisim::util::debug::ArrayType const *cached_type = dynamic_cast<unisim::util::debug::ArrayType const *>(*it);
												if((cached_type->GetTypeOfElement() == array_type) && (cached_type->GetOrder() == i) && (cached_type->GetLowerBound() == lower_bound) && (cached_type->GetUpperBound() == upper_bound))
												{
													array_type = cached_type;
													cached = true;
													break;
												}
											}
											
											if(!cached)
											{
												array_type = new unisim::util::debug::ArrayType(array_type, i, lower_bound, upper_bound);
												array_type->Catch();
												type_cache.push_back(array_type);
											}
										}
										else
										{
											Type const *type_of_element = dw_die_element_type->GetType(prc_num);
											
											bool cached = false;
											for(typename TypeCache::const_iterator it = type_cache.begin(); it != type_cache.end(); ++it)
											{
												unisim::util::debug::ArrayType const *cached_type = dynamic_cast<unisim::util::debug::ArrayType const *>(*it);
												if((cached_type->GetTypeOfElement() == type_of_element) && (cached_type->GetOrder() == i) && (cached_type->GetLowerBound() == lower_bound) && (cached_type->GetUpperBound() == upper_bound))
												{
													array_type = cached_type;
													cached = true;
													break;
												}
											}
											
											if(!cached)
											{
												array_type = new unisim::util::debug::ArrayType(type_of_element, i, lower_bound, upper_bound);
												array_type->Catch();
												type_cache.push_back(array_type);
											}
										}
									}
									dim--;
								}
								break;
						}
						
						switch(dw_array_ordering)
						{
							case DW_ORD_row_major:
								if(i-- > 0) continue;
								break;
							case DW_ORD_col_major:
								if(++i < num_children) continue;
								break;
						}
						break;
					}
					while(1);
					
					return array_type ? array_type : dw_die_element_type->GetType(prc_num);
				}
			}
			break;
		case DW_TAG_pointer_type:
			{
				const DWARF_DIE<MEMORY_ADDR> *dw_die_dereferenced_object_type = GetTypeDIE();
				
				unisim::util::debug::Type const *dereferenced_object_type = 0;
				if(dw_die_dereferenced_object_type)
				{
					dereferenced_object_type = dw_die_dereferenced_object_type->GetType(prc_num, true);
				}
				else
				{
					for(typename TypeCache::const_iterator it = type_cache.begin(); it != type_cache.end(); ++it)
					{
						unisim::util::debug::UnspecifiedType const *cached_type = dynamic_cast<unisim::util::debug::UnspecifiedType const *>(*it);
						if(cached_type)
						{
							dereferenced_object_type = cached_type;
							break;
						}
					}
					
					if(!dereferenced_object_type)
					{
						dereferenced_object_type = new unisim::util::debug::UnspecifiedType();
						dereferenced_object_type->Catch();
						type_cache.push_back(dereferenced_object_type);
					}
				}
					
				for(typename TypeCache::const_iterator it = type_cache.begin(); it != type_cache.end(); ++it)
				{
					unisim::util::debug::PointerType const *cached_type = dynamic_cast<unisim::util::debug::PointerType const *>(*it);
					if(cached_type && (cached_type->GetTypeOfDereferencedObject() == dereferenced_object_type)) return cached_type;
				}
				
				unisim::util::debug::PointerType const *pointer_type = new unisim::util::debug::PointerType(dereferenced_object_type);
				pointer_type->Catch();
				type_cache.push_back(pointer_type);
				return pointer_type;
			}
			break;
		case DW_TAG_typedef:
			{
				const DWARF_DIE<MEMORY_ADDR> *dw_die_type = GetTypeDIE();
				
				const char *typedef_name = GetName();
				
				unisim::util::debug::Type const *type = 0;
				if(dw_die_type)
				{
					type = dw_die_type->GetType(prc_num, following_pointer);
				}
				else
				{
					for(typename TypeCache::const_iterator it = type_cache.begin(); it != type_cache.end(); ++it)
					{
						unisim::util::debug::UnspecifiedType const *cached_type = dynamic_cast<unisim::util::debug::UnspecifiedType const *>(*it);
						if(cached_type)
						{
							type = cached_type;
							break;
						}
					}
				}
				
				for(typename TypeCache::const_iterator it = type_cache.begin(); it != type_cache.end(); ++it)
				{
					unisim::util::debug::Typedef const *cached_type = dynamic_cast<unisim::util::debug::Typedef const *>(*it);
					if(cached_type && (cached_type->GetType() == type)) return cached_type;
				}
				
				unisim::util::debug::Typedef const *typedef_type = new unisim::util::debug::Typedef(type, typedef_name);
				type_cache.push_back(typedef_type);
				typedef_type->Catch();
				return typedef_type;
			}
			break;
		case DW_TAG_subroutine_type:
			{
				for(typename TypeCache::const_iterator it = type_cache.begin(); it != type_cache.end(); ++it)
				{
					unisim::util::debug::FunctionType const *cached_type = dynamic_cast<unisim::util::debug::FunctionType const *>(*it);
					if(cached_type) return cached_type;
				}
				
				const DWARF_DIE<MEMORY_ADDR> *dw_die_return_type = GetTypeDIE();
				
				unisim::util::debug::FunctionType *func_type = new unisim::util::debug::FunctionType(dw_die_return_type ? dw_die_return_type->GetType(prc_num) : new unisim::util::debug::UnspecifiedType());
				
				const DWARF_DIE<MEMORY_ADDR> *dw_at_abstract_origin = GetAbstractOrigin();
				const std::vector<DWARF_DIE<MEMORY_ADDR> *>& dw_children = dw_at_abstract_origin ? dw_at_abstract_origin->GetChildren() : children;
				unsigned int num_children = dw_children.size();
				unsigned int i;
				for(i = 0; i < num_children; i++)
				{
					const DWARF_DIE<MEMORY_ADDR> *child = dw_children[i];
					
					if(child->GetTag() == DW_TAG_formal_parameter)
					{
						const char *formal_param_name = child->GetName();
						const DWARF_DIE<MEMORY_ADDR> *dw_die_formal_param_type = child->GetTypeDIE();
						
						func_type->Add(new unisim::util::debug::FormalParameter(formal_param_name ? formal_param_name :  "", dw_die_formal_param_type ? dw_die_formal_param_type->GetType(prc_num) : new unisim::util::debug::UnspecifiedType()));
					}
				}
				
				func_type->Catch();
				type_cache.push_back(func_type);
				return func_type;
			}
			break;
		case DW_TAG_const_type:
			{
				const DWARF_DIE<MEMORY_ADDR> *dw_die_type = GetTypeDIE();
				
				unisim::util::debug::Type const *type = 0;
				if(dw_die_type)
				{
					type = dw_die_type->GetType(prc_num, following_pointer);
				}
				else
				{
					for(typename TypeCache::const_iterator it = type_cache.begin(); it != type_cache.end(); ++it)
					{
						unisim::util::debug::UnspecifiedType const *cached_type = dynamic_cast<unisim::util::debug::UnspecifiedType const *>(*it);
						if(cached_type)
						{
							type = cached_type;
							break;
						}
					}
					
					if(!type)
					{
						type = new unisim::util::debug::UnspecifiedType();
						type->Catch();
						type_cache.push_back(type);
					}
				}
				
				for(typename TypeCache::const_iterator it = type_cache.begin(); it != type_cache.end(); ++it)
				{
					unisim::util::debug::ConstType const *cached_type = dynamic_cast<unisim::util::debug::ConstType const *>(*it);
					if(cached_type && (cached_type->GetType() == type)) return cached_type;
				}
				
				unisim::util::debug::ConstType const *const_type = new unisim::util::debug::ConstType(type);
				const_type->Catch();
				type_cache.push_back(const_type);
				return const_type;
			}
			break;
		case DW_TAG_enumeration_type:
			{
				for(typename TypeCache::const_iterator it = type_cache.begin(); it != type_cache.end(); ++it)
				{
					unisim::util::debug::EnumType const *cached_type = dynamic_cast<unisim::util::debug::EnumType const *>(*it);
					if(cached_type) return cached_type;
				}
				
				const char *enum_name = GetName();
				unisim::util::debug::EnumType *enum_type = new unisim::util::debug::EnumType(enum_name);
				
				unsigned int num_children = children.size();
				unsigned int i;
				for(i = 0; i < num_children; i++)
				{
					const DWARF_DIE<MEMORY_ADDR> *child = children[i];
					
					if(child->GetTag() == DW_TAG_enumerator)
					{
						const char *enumerator_name = child->GetName();
						
						if(enumerator_name)
						{
							uint64_t unsigned_const_value;
							if(child->GetConstValue(unsigned_const_value))
							{
								enum_type->Add(new unisim::util::debug::Enumerator(enumerator_name, unsigned_const_value));
							}
							else
							{
								int64_t signed_const_value = 0;
								if(child->GetConstValue(signed_const_value))
								{
									enum_type->Add(new unisim::util::debug::Enumerator(enumerator_name, signed_const_value));
								}
							}
						}
					}
				}
				
				enum_type->Catch();
				type_cache.push_back(enum_type);
				return enum_type;
			}
			break;
		case DW_TAG_unspecified_type:
		{
			for(typename TypeCache::const_iterator it = type_cache.begin(); it != type_cache.end(); ++it)
			{
				unisim::util::debug::UnspecifiedType const *cached_type = dynamic_cast<unisim::util::debug::UnspecifiedType const *>(*it);
				if(cached_type) return cached_type;
			}
			
			unisim::util::debug::UnspecifiedType const *unspecified_type = new unisim::util::debug::UnspecifiedType();
			unspecified_type->Catch();
			type_cache.push_back(unspecified_type);
			return unspecified_type;
		}
		
		case DW_TAG_volatile_type:
			{
				const DWARF_DIE<MEMORY_ADDR> *dw_die_type = GetTypeDIE();
				
				unisim::util::debug::Type const *type = 0;
				if(dw_die_type)
				{
					type = dw_die_type->GetType(prc_num, following_pointer);
				}
				else
				{
					for(typename TypeCache::const_iterator it = type_cache.begin(); it != type_cache.end(); ++it)
					{
						unisim::util::debug::UnspecifiedType const *cached_type = dynamic_cast<unisim::util::debug::UnspecifiedType const *>(*it);
						if(cached_type)
						{
							type = cached_type;
							break;
						}
					}
					
					if(!type)
					{
						type = new unisim::util::debug::UnspecifiedType();
						type->Catch();
						type_cache.push_back(type);
					}
				}
				
				for(typename TypeCache::const_iterator it = type_cache.begin(); it != type_cache.end(); ++it)
				{
					unisim::util::debug::VolatileType const *cached_type = dynamic_cast<unisim::util::debug::VolatileType const *>(*it);
					if(cached_type && (cached_type->GetType() == type)) return cached_type;
				}
				
				unisim::util::debug::VolatileType const *volatile_type = new unisim::util::debug::VolatileType(type);
				volatile_type->Catch();
				type_cache.push_back(volatile_type);
				return volatile_type;
			}
			break;
		default:
			debug_warning_stream << "Don't know how to handle tag " << DWARF_GetTagName(dw_tag) << std::endl;
			break;
	}
	
	for(typename TypeCache::const_iterator it = type_cache.begin(); it != type_cache.end(); ++it)
	{
		unisim::util::debug::Type const *cached_type = *it;
		if(cached_type) return cached_type;
	}
	
	unisim::util::debug::Type const *type = new unisim::util::debug::Type();
	type->Catch();
	type_cache.push_back(type);
	return type;
}

template <class MEMORY_ADDR>
const unisim::util::debug::Type *DWARF_DIE<MEMORY_ADDR>::GetTypeOf(unsigned int prc_num) const
{
	const DWARF_DIE<MEMORY_ADDR> *dw_die_type = GetTypeDIE();
	if(!dw_die_type)
	{
		if(debug)
		{
			debug_info_stream << "In File \"" << dw_handler->GetFilename() << "\", can't determine type of DIE #" << id << std::endl;
		}
		return new unisim::util::debug::Type();
	}
	
	return dw_die_type->GetType(prc_num);
}

template <class MEMORY_ADDR>
const unisim::util::debug::SubProgram<MEMORY_ADDR> *DWARF_DIE<MEMORY_ADDR>::GetSubProgram(unsigned int prc_num) const
{
	if(cached_subprogram.Valid()) return cached_subprogram;
	
	bool external_flag = false;
	bool declaration_flag = false;
	uint8_t inline_code = 0;
	GetInlineCode(inline_code);
	const DWARF_DIE<MEMORY_ADDR> *dw_die_return_type = GetTypeDIE();
	
	DWARF_SubProgram<MEMORY_ADDR> *dw_subprogram = new DWARF_SubProgram<MEMORY_ADDR>(
		GetName(),
		(GetExternalFlag(external_flag) && external_flag),
		(GetDeclarationFlag(declaration_flag) && declaration_flag),
		inline_code,
		dw_die_return_type ? dw_die_return_type->GetType(prc_num) : new unisim::util::debug::UnspecifiedType()
	);
	
	const DWARF_DIE<MEMORY_ADDR> *dw_at_abstract_origin = GetAbstractOrigin();
	
	const std::vector<DWARF_DIE<MEMORY_ADDR> *>& dw_children = dw_at_abstract_origin ? dw_at_abstract_origin->GetChildren() : children;
	unsigned int num_children = dw_children.size();
	unsigned int i;
	for(i = 0; i < num_children; i++)
	{
		const DWARF_DIE<MEMORY_ADDR> *child = dw_children[i];
		
		if(child->GetTag() == DW_TAG_formal_parameter)
		{
			const char *formal_param_name = child->GetName();
			const DWARF_DIE<MEMORY_ADDR> *dw_die_formal_param_type = child->GetTypeDIE();
			
			dw_subprogram->AddFormalParameter(new unisim::util::debug::FormalParameter(formal_param_name ? formal_param_name :  "", dw_die_formal_param_type ? dw_die_formal_param_type->GetType(prc_num) : new unisim::util::debug::UnspecifiedType()));
		}
	}
	
	dw_subprogram->Catch();
	return cached_subprogram = dw_subprogram;
}

template <class MEMORY_ADDR>
const unisim::util::debug::Variable *DWARF_DIE<MEMORY_ADDR>::GetVariable(unsigned int prc_num) const
{
	if(cached_variable.Valid()) return cached_variable;
	
	bool external_flag = false;
	bool declaration_flag = false;
	const DWARF_DIE<MEMORY_ADDR> *dw_variable_type = GetTypeDIE();
	
	DWARF_Variable *dw_variable = new DWARF_Variable(
		GetName(),
		(GetExternalFlag(external_flag) && external_flag),
		(GetDeclarationFlag(declaration_flag) && declaration_flag),
		dw_variable_type ? dw_variable_type->GetType(prc_num) : new unisim::util::debug::UnspecifiedType()
	);
	
	dw_variable->Catch();
	return cached_variable = dw_variable;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetAttributeValue(uint16_t dw_at, const DWARF_Address<MEMORY_ADDR> * & p_dw_addr_attr) const
{
	const DWARF_Attribute<MEMORY_ADDR> *dw_attr = FindAttribute(dw_at);
	
	if(dw_attr)
	{
		const DWARF_AttributeValue<MEMORY_ADDR> *dw_attr_value = dw_attr->GetValue();
		if(dw_attr_value->GetClass() == DW_CLASS_ADDRESS)
		{
			p_dw_addr_attr = (const DWARF_Address<MEMORY_ADDR> *) dw_attr_value;
			return true;
		}
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetAttributeValue(uint16_t dw_at, const DWARF_Block<MEMORY_ADDR> * & p_dw_block_attr) const
{
	const DWARF_Attribute<MEMORY_ADDR> *dw_attr = FindAttribute(dw_at);
	
	if(dw_attr)
	{
		const DWARF_AttributeValue<MEMORY_ADDR> *dw_attr_value = dw_attr->GetValue();
		if(dw_attr_value->GetClass() == DW_CLASS_BLOCK)
		{
			p_dw_block_attr = (const DWARF_Block<MEMORY_ADDR> *) dw_attr_value;
			return true;
		}
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetAttributeValue(uint16_t dw_at, const DWARF_Constant<MEMORY_ADDR> * & p_dw_const_attr) const
{
	const DWARF_Attribute<MEMORY_ADDR> *dw_attr = FindAttribute(dw_at);
	
	if(dw_attr)
	{
		const DWARF_AttributeValue<MEMORY_ADDR> *dw_attr_value = dw_attr->GetValue();
		if(dw_attr_value->GetClass() & DW_CLASS_CONSTANT)
		{
			p_dw_const_attr = (const DWARF_Constant<MEMORY_ADDR> *) dw_attr_value;
			return true;
		}
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetAttributeValue(uint16_t dw_at, const DWARF_UnsignedConstant<MEMORY_ADDR> * & p_dw_uconst_attr) const
{
	const DWARF_Attribute<MEMORY_ADDR> *dw_attr = FindAttribute(dw_at);
	
	if(dw_attr)
	{
		const DWARF_AttributeValue<MEMORY_ADDR> *dw_attr_value = dw_attr->GetValue();
		if(dw_attr_value->GetClass() == DW_CLASS_UNSIGNED_CONSTANT)
		{
			p_dw_uconst_attr = (const DWARF_UnsignedConstant<MEMORY_ADDR> *) dw_attr_value;
			return true;
		}
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetAttributeValue(uint16_t dw_at, const DWARF_SignedConstant<MEMORY_ADDR> * & p_dw_sconst_attr) const
{
	const DWARF_Attribute<MEMORY_ADDR> *dw_attr = FindAttribute(dw_at);
	
	if(dw_attr)
	{
		const DWARF_AttributeValue<MEMORY_ADDR> *dw_attr_value = dw_attr->GetValue();
		if(dw_attr_value->GetClass() == DW_CLASS_SIGNED_CONSTANT)
		{
			p_dw_sconst_attr = (const DWARF_SignedConstant<MEMORY_ADDR> *) dw_attr_value;
			return true;
		}
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetAttributeValue(uint16_t dw_at, const DWARF_UnsignedLEB128Constant<MEMORY_ADDR> * & p_dw_uconst_attr) const
{
	const DWARF_Attribute<MEMORY_ADDR> *dw_attr = FindAttribute(dw_at);
	
	if(dw_attr)
	{
		const DWARF_AttributeValue<MEMORY_ADDR> *dw_attr_value = dw_attr->GetValue();
		if(dw_attr_value->GetClass() == DW_CLASS_UNSIGNED_LEB128_CONSTANT)
		{
			p_dw_uconst_attr = (const DWARF_UnsignedLEB128Constant<MEMORY_ADDR> *) dw_attr_value;
			return true;
		}
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetAttributeValue(uint16_t dw_at, const DWARF_SignedLEB128Constant<MEMORY_ADDR> * & p_dw_sconst_attr) const
{
	const DWARF_Attribute<MEMORY_ADDR> *dw_attr = FindAttribute(dw_at);
	
	if(dw_attr)
	{
		const DWARF_AttributeValue<MEMORY_ADDR> *dw_attr_value = dw_attr->GetValue();
		if(dw_attr_value->GetClass() == DW_CLASS_SIGNED_LEB128_CONSTANT)
		{
			p_dw_sconst_attr = (const DWARF_SignedLEB128Constant<MEMORY_ADDR> *) dw_attr_value;
			return true;
		}
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetAttributeValue(uint16_t dw_at, const DWARF_Flag<MEMORY_ADDR> * & p_dw_flag_attr) const
{
	const DWARF_Attribute<MEMORY_ADDR> *dw_attr = FindAttribute(dw_at);
	
	if(dw_attr)
	{
		const DWARF_AttributeValue<MEMORY_ADDR> *dw_attr_value = dw_attr->GetValue();
		if(dw_attr_value->GetClass() == DW_CLASS_FLAG)
		{
			p_dw_flag_attr = (const DWARF_Flag<MEMORY_ADDR> *) dw_attr_value;
			return true;
		}
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetAttributeValue(uint16_t dw_at, const DWARF_LinePtr<MEMORY_ADDR> * & p_dw_lineptr_attr) const
{
	const DWARF_Attribute<MEMORY_ADDR> *dw_attr = FindAttribute(dw_at);
	
	if(dw_attr)
	{
		const DWARF_AttributeValue<MEMORY_ADDR> *dw_attr_value = dw_attr->GetValue();
		if(dw_attr_value->GetClass() == DW_CLASS_LINEPTR)
		{
			p_dw_lineptr_attr = (const DWARF_LinePtr<MEMORY_ADDR> *) dw_attr_value;
			return true;
		}
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetAttributeValue(uint16_t dw_at, const DWARF_LocListPtr<MEMORY_ADDR> * & p_dw_loclist_attr) const
{
	const DWARF_Attribute<MEMORY_ADDR> *dw_attr = FindAttribute(dw_at);
	
	if(dw_attr)
	{
		const DWARF_AttributeValue<MEMORY_ADDR> *dw_attr_value = dw_attr->GetValue();
		if(dw_attr_value->GetClass() == DW_CLASS_LOCLISTPTR)
		{
			p_dw_loclist_attr = (const DWARF_LocListPtr<MEMORY_ADDR> *) dw_attr_value;
			return true;
		}
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetAttributeValue(uint16_t dw_at, const DWARF_MacPtr<MEMORY_ADDR> * & p_dw_macptr_attr) const
{
	const DWARF_Attribute<MEMORY_ADDR> *dw_attr = FindAttribute(dw_at);
	
	if(dw_attr)
	{
		const DWARF_AttributeValue<MEMORY_ADDR> *dw_attr_value = dw_attr->GetValue();
		if(dw_attr_value->GetClass() == DW_CLASS_MACPTR)
		{
			p_dw_macptr_attr = (const DWARF_MacPtr<MEMORY_ADDR> *) dw_attr_value;
			return true;
		}
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetAttributeValue(uint16_t dw_at, const DWARF_RangeListPtr<MEMORY_ADDR> * & p_dw_ranges_attr) const
{
	const DWARF_Attribute<MEMORY_ADDR> *dw_attr = FindAttribute(dw_at);
	
	if(dw_attr)
	{
		const DWARF_AttributeValue<MEMORY_ADDR> *dw_attr_value = dw_attr->GetValue();
		if(dw_attr_value->GetClass() == DW_CLASS_RANGELISTPTR)
		{
			p_dw_ranges_attr = (const DWARF_RangeListPtr<MEMORY_ADDR> *) dw_attr_value;
			return true;
		}
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetAttributeValue(uint16_t dw_at, const DWARF_Reference<MEMORY_ADDR> * & p_dw_ref_attr) const
{
	const DWARF_Attribute<MEMORY_ADDR> *dw_attr = FindAttribute(dw_at);
	
	if(dw_attr)
	{
		const DWARF_AttributeValue<MEMORY_ADDR> *dw_attr_value = dw_attr->GetValue();
		if(dw_attr_value->GetClass() == DW_CLASS_REFERENCE)
		{
			p_dw_ref_attr = (const DWARF_Reference<MEMORY_ADDR> *) dw_attr_value;
			return true;
		}
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetAttributeValue(uint16_t dw_at, const DWARF_String<MEMORY_ADDR> * & p_dw_str_attr) const
{
	const DWARF_Attribute<MEMORY_ADDR> *dw_attr = FindAttribute(dw_at);
	
	if(dw_attr)
	{
		const DWARF_AttributeValue<MEMORY_ADDR> *dw_attr_value = dw_attr->GetValue();
		if(dw_attr_value->GetClass() == DW_CLASS_STRING)
		{
			p_dw_str_attr = (const DWARF_String<MEMORY_ADDR> *) dw_attr_value;
			return true;
		}
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetAttributeValue(uint16_t dw_at, const DWARF_Expression<MEMORY_ADDR> * & p_dw_expr_attr) const
{
	const DWARF_Attribute<MEMORY_ADDR> *dw_attr = FindAttribute(dw_at);
	
	if(dw_attr)
	{
		const DWARF_AttributeValue<MEMORY_ADDR> *dw_attr_value = dw_attr->GetValue();
		if(dw_attr_value->GetClass() == DW_CLASS_EXPRESSION)
		{
			p_dw_expr_attr = (const DWARF_Expression<MEMORY_ADDR> *) dw_attr_value;
			return true;
		}
	}
	return false;
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_DIE<MEMORY_ADDR>::FindParentDIE(unsigned int dw_tag) const
{
	return dw_parent_die ? ((dw_parent_die->GetTag() == dw_tag) ? dw_parent_die : dw_parent_die->FindParentDIE(dw_tag)) : 0;
}

template <class MEMORY_ADDR>
template <typename VISITOR>
void DWARF_DIE<MEMORY_ADDR>::Scan(VISITOR& visitor) const
{
	unsigned int num_children = children.size();
	unsigned int i;
	for(i = 0; i < num_children; i++)
	{
		DWARF_DIE<MEMORY_ADDR> const *dw_child = children[i];
		if(!visitor.Visit(dw_child)) return;
	}
}

template <class MEMORY_ADDR>
template <typename VISITOR>
void DWARF_DIE<MEMORY_ADDR>::ScanType(VISITOR& visitor) const
{
	const DWARF_DIE<MEMORY_ADDR> *dw_die_type = GetTypeDIE();
	if(dw_die_type)
	{
		if(!visitor.VisitType(dw_die_type)) return;
	}
	unsigned int num_children = children.size();
	unsigned int i;
	for(i = 0; i < num_children; i++)
	{
		DWARF_DIE<MEMORY_ADDR> const *dw_child = children[i];
		if(!visitor.VisitType(dw_child)) return;
	}
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
