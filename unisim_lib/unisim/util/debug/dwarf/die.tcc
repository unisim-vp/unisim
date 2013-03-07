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

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
DWARF_DIE<MEMORY_ADDR>::DWARF_DIE(DWARF_CompilationUnit<MEMORY_ADDR> *_dw_cu, DWARF_DIE<MEMORY_ADDR> *_dw_parent_die)
	: dw_cu(_dw_cu)
	, dw_parent_die(_dw_parent_die)
	, offset(0xffffffffffffffffULL)
	, abbrev(0)
	, children()
	, attributes()
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
		delete children[i];
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
		std::cerr << "Can't find abbrevation " << dw_abbrev_code.to_string(false) << std::endl;
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
						case DW_AT_const_value:
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
						case DW_AT_const_value:
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
						case DW_AT_const_value:
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
						case DW_AT_const_value:
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
			default:
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
			
			if(!dw_die->IsNull())
			{
				children.push_back(dw_die);
				dw_cu->Register(dw_die);
			}
		}
		while(!dw_die->IsNull());
		
		delete dw_die;
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
void DWARF_DIE<MEMORY_ADDR>::BuildStatementMatrix(std::map<MEMORY_ADDR, const Statement<MEMORY_ADDR> *>& stmt_matrix)
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
				
				DWARF_StatementVM<MEMORY_ADDR> dw_stmt_vm = DWARF_StatementVM<MEMORY_ADDR>();
				if(!dw_stmt_vm.Run(dw_stmt_prog, 0, &stmt_matrix))
				{
					std::cerr << "Invalid DWARF2 statement program. Statement matrix may be incomplete." << std::endl;
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
void DWARF_DIE<MEMORY_ADDR>::GetRanges(std::set<std::pair<MEMORY_ADDR, MEMORY_ADDR> >& ranges) const
{
	MEMORY_ADDR low_pc = 0;
	if(GetLowPC(low_pc))
	{
		MEMORY_ADDR high_pc = 0;
		if(GetHighPC(high_pc))
		{
			ranges.insert(std::pair<MEMORY_ADDR, MEMORY_ADDR>(low_pc, high_pc));
		}
		else
		{
			// single address
			ranges.insert(std::pair<MEMORY_ADDR, MEMORY_ADDR>(low_pc, low_pc + 1));
		}
		return;
	}

	// DW_AT_ranges is only available in DWARF3 and more
	const DWARF_Attribute<MEMORY_ADDR> *dw_at_ranges = FindAttribute(DW_AT_ranges);
	
	if(dw_at_ranges)
	{
		const DWARF_AttributeValue<MEMORY_ADDR> *dw_at_ranges_value = dw_at_ranges->GetValue();
		if(dw_at_ranges_value->GetClass() == DW_CLASS_RANGELISTPTR)
		{
			const DWARF_RangeListPtr<MEMORY_ADDR> *range_list_ptr = (const DWARF_RangeListPtr<MEMORY_ADDR> *) dw_at_ranges_value;
			
			const DWARF_RangeListEntry<MEMORY_ADDR> *range_list_entry = range_list_ptr->GetValue();
			
			bool has_base_addr = false;
			MEMORY_ADDR base_addr = 0;
			do
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
						if(!dw_cu->GetDefaultBaseAddress(base_addr)) return;
						has_base_addr = true;
					}
					ranges.insert(std::pair<MEMORY_ADDR, MEMORY_ADDR>(range_list_entry->GetBegin(), range_list_entry->GetEnd()));
				}
				if(range_list_entry->IsEndOfList()) break;
			}
			while((range_list_entry = range_list_entry->GetNext()) != 0);
		}
		return;
	}
	
	if(dw_parent_die) dw_parent_die->GetRanges(ranges);
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::HasOverlap(MEMORY_ADDR addr, MEMORY_ADDR length) const
{
	MEMORY_ADDR low_pc = 0;
	if(GetLowPC(low_pc))
	{
		MEMORY_ADDR high_pc = 0;
		if(GetHighPC(high_pc))
		{
			return unisim::util::debug::dwarf::HasOverlapEx(low_pc, high_pc, addr, addr + length);
		}
		else
		{
			// single address
			return (addr == low_pc);
		}
		return false;
	}

	// DW_AT_ranges is only available in DWARF3 and more
	const DWARF_Attribute<MEMORY_ADDR> *dw_at_ranges = FindAttribute(DW_AT_ranges);
	
	if(dw_at_ranges)
	{
		const DWARF_AttributeValue<MEMORY_ADDR> *dw_at_ranges_value = dw_at_ranges->GetValue();
		if(dw_at_ranges_value->GetClass() == DW_CLASS_RANGELISTPTR)
		{
			const DWARF_RangeListPtr<MEMORY_ADDR> *range_list_ptr = (const DWARF_RangeListPtr<MEMORY_ADDR> *) dw_at_ranges_value;
			
			const DWARF_RangeListEntry<MEMORY_ADDR> *range_list_entry = range_list_ptr->GetValue();
			
			bool has_base_addr = false;
			MEMORY_ADDR base_addr = 0;
			do
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
						if(!dw_cu->GetDefaultBaseAddress(base_addr)) return false;
						has_base_addr = true;
					}
					if(range_list_entry->HasOverlap(base_addr, addr, length)) return true;
				}
				if(range_list_entry->IsEndOfList()) break;
			}
			while((range_list_entry = range_list_entry->GetNext()) != 0);
		}
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

// 	if((!dw_tag || (GetTag() == dw_tag)) && HasOverlap(addr, length))
// 	{
// 		return dw_found_die ? dw_found_die : this;
// 	}
	
	return dw_found_die ? dw_found_die : (((!dw_tag || (GetTag() == dw_tag)) && HasOverlap(addr, length)) ? this : 0);
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_DIE<MEMORY_ADDR>::FindDataObject(const char *name) const
{
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
						if(strcmp(child_name, name) == 0) return dw_child;
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
					if(child_name)
					{
						if(strcmp(child_name, name) == 0) return dw_child;
					}
				}
				break;
		}
	}
	
	return 0;
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_DIE<MEMORY_ADDR>::FindSubRangeType() const
{
	unsigned int num_children = children.size();
	unsigned int i;
	for(i = 0; i < num_children; i++)
	{
		DWARF_DIE<MEMORY_ADDR> *dw_child = children[i];

		if(dw_child->GetTag() == DW_TAG_subrange_type) return dw_child;
	}
	
	return 0;
}

template <class MEMORY_ADDR>
const char *DWARF_DIE<MEMORY_ADDR>::GetName() const
{
	const DWARF_String<MEMORY_ADDR> *dw_at_name;
	if(GetAttributeValue(DW_AT_name, dw_at_name))
	{
		return dw_at_name->GetValue();
	}
	return 0;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetLowPC(MEMORY_ADDR& low_pc) const
{
	const DWARF_Address<MEMORY_ADDR> *dw_at_low_pc;
	if(GetAttributeValue(DW_AT_low_pc, dw_at_low_pc))
	{
		low_pc = dw_at_low_pc->GetValue();
		return true;
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetHighPC(MEMORY_ADDR& high_pc) const
{
	const DWARF_Address<MEMORY_ADDR> *dw_at_high_pc;
	if(GetAttributeValue(DW_AT_high_pc, dw_at_high_pc))
	{
		high_pc = dw_at_high_pc->GetValue();
		return true;
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetContigousAddressRange(MEMORY_ADDR& low_pc, MEMORY_ADDR& high_pc) const
{
	return GetLowPC(low_pc) && GetHighPC(high_pc);
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
	return 0;
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
	return 0;
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
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetFrameBase(MEMORY_ADDR pc, MEMORY_ADDR& frame_base) const
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
									if(!dw_cu->GetDefaultBaseAddress(base_addr)) return false;
									has_base_addr = true;
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
					DWARF_ExpressionVM<MEMORY_ADDR> loc_expr_vm = DWARF_ExpressionVM<MEMORY_ADDR>(dw_cu->GetHandler());
					DWARF_Location<MEMORY_ADDR> frame_base_loc;
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
								unisim::util::debug::Register *arch_reg = dw_cu->GetHandler()->GetRegisterNumberMapping()->GetArchReg(dw_reg_num);
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
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetAttributeStaticDynamicValue(uint16_t dw_at, MEMORY_ADDR& value) const
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
			DWARF_ExpressionVM<MEMORY_ADDR> dw_at_expr_value_vm = DWARF_ExpressionVM<MEMORY_ADDR>(dw_cu->GetHandler());
			bool dw_at_expr_value_status = dw_at_expr_value_vm.Execute(dw_at_expr_value, value, 0);
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
					DWARF_ExpressionVM<MEMORY_ADDR> dw_dwarf_procedure_compute_vm = DWARF_ExpressionVM<MEMORY_ADDR>(dw_cu->GetHandler());
					bool dw_dwarf_procedure_compute_status = dw_dwarf_procedure_compute_vm.Execute(dw_dwarf_procedure_compute_expr, value, 0);
					return dw_dwarf_procedure_compute_status;
				}
			}
		}
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetLowerBound(MEMORY_ADDR& lower_bound) const
{
	return GetAttributeStaticDynamicValue(DW_AT_lower_bound, lower_bound);
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetUpperBound(MEMORY_ADDR& upper_bound) const
{
	return GetAttributeStaticDynamicValue(DW_AT_upper_bound, upper_bound);
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetCount(MEMORY_ADDR& count) const
{
	if(GetAttributeStaticDynamicValue(DW_AT_count, count)) return true;

	MEMORY_ADDR upper_bound = 0;
	if(GetUpperBound(upper_bound))
	{
		MEMORY_ADDR lower_bound = 0;
		if(!GetLowerBound(lower_bound))
		{
			uint16_t dw_at_language = dw_cu->GetLanguage();
			lower_bound = (dw_at_language == DW_LANG_Fortran77) || (dw_at_language == DW_LANG_Fortran90) ? 1 : 0;
		}
		count = upper_bound - lower_bound + 1;
		
		return true;
	}
	
	const DWARF_Reference<MEMORY_ADDR> *dw_type_ref = 0;
	
	if(GetAttributeValue(DW_AT_type, dw_type_ref))
	{
		const DWARF_DIE<MEMORY_ADDR> *dw_die_type = dw_type_ref->GetValue();
		
		while(dw_die_type->GetTag() == DW_TAG_typedef)
		{
			if(!dw_die_type->GetAttributeValue(DW_AT_type, dw_type_ref)) return false;
			dw_die_type = dw_type_ref->GetValue();
		}
		
		return dw_die_type->GetCount(count);
	}

	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetByteSize(MEMORY_ADDR& byte_size) const
{
	if(GetAttributeStaticDynamicValue(DW_AT_byte_size, byte_size)) return true;
	
	if(GetTag() == DW_TAG_array_type)
	{
		const DWARF_Reference<MEMORY_ADDR> *dw_array_element_type_ref = 0;
		if(!GetAttributeValue(DW_AT_type, dw_array_element_type_ref)) return false;
		
		const DWARF_DIE<MEMORY_ADDR> *dw_array_element_type = dw_array_element_type_ref->GetValue();
		
// 		while(dw_array_element_type->GetTag() == DW_TAG_typedef)
// 		{
// 			if(!dw_array_element_type->GetAttributeValue(DW_AT_type, dw_array_element_type_ref)) return false;
// 			dw_array_element_type = dw_array_element_type_ref->GetValue();
// 		}
		
		MEMORY_ADDR array_element_byte_size;
		if(!dw_array_element_type->GetByteSize(array_element_byte_size)) return false;
		
		const DWARF_DIE<MEMORY_ADDR> *dw_die_subrange_type = GetChild(DW_TAG_subrange_type);
		if(!dw_die_subrange_type) return false;
		
		MEMORY_ADDR count = 0;
		if(!dw_die_subrange_type->GetCount(count)) return false;
		
		byte_size = count * array_element_byte_size;
		
		return true;
	}
	
	const DWARF_Reference<MEMORY_ADDR> *dw_type_ref = 0;
	
	if(GetAttributeValue(DW_AT_type, dw_type_ref))
	{
		const DWARF_DIE<MEMORY_ADDR> *dw_die_type = dw_type_ref->GetValue();
		
		while(dw_die_type->GetTag() == DW_TAG_typedef)
		{
			if(!dw_die_type->GetAttributeValue(DW_AT_type, dw_type_ref)) return false;
			dw_die_type = dw_type_ref->GetValue();
		}
		
		return dw_die_type->GetByteSize(byte_size);
	}
	
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetBitSize(MEMORY_ADDR& bit_size) const
{
	if(GetAttributeStaticDynamicValue(DW_AT_bit_size, bit_size)) return true;

	MEMORY_ADDR byte_size = 0;
	if(GetByteSize(byte_size))
	{
		bit_size = 8 * byte_size;
		return true;
	}

	const DWARF_Reference<MEMORY_ADDR> *dw_type_ref = 0;
	
	if(GetAttributeValue(DW_AT_type, dw_type_ref))
	{
		const DWARF_DIE<MEMORY_ADDR> *dw_die_type = dw_type_ref->GetValue();
		
		while(dw_die_type->GetTag() == DW_TAG_typedef)
		{
			if(!dw_die_type->GetAttributeValue(DW_AT_type, dw_type_ref)) return false;
			dw_die_type = dw_type_ref->GetValue();
		}
		
		return dw_die_type->GetBitSize(bit_size);
	}

	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetBitOffset(MEMORY_ADDR& bit_offset) const
{
	return GetAttributeStaticDynamicValue(DW_AT_bit_offset, bit_offset);
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetDataBitOffset(MEMORY_ADDR& data_bit_offset) const
{
	return GetAttributeStaticDynamicValue(DW_AT_data_bit_offset, data_bit_offset);
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetLocationExpression(uint16_t dw_at, MEMORY_ADDR pc, const DWARF_Expression<MEMORY_ADDR> * & p_dw_loc_expr, std::set<std::pair<MEMORY_ADDR, MEMORY_ADDR> >& ranges) const
{
	if(GetAttributeValue(dw_at, p_dw_loc_expr)) return true;

	const DWARF_LocListPtr<MEMORY_ADDR> *dw_loc_list_ptr = 0;
	if(!GetAttributeValue(dw_at, dw_loc_list_ptr)) return false;
	const DWARF_LocListEntry<MEMORY_ADDR> *dw_loc_list_entry;
	
	bool has_base_addr = false;
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
				if(!dw_cu->GetDefaultBaseAddress(base_addr)) return false;
				has_base_addr = true;
			}
			//std::cerr << "loc_list_entry:" << *dw_loc_list_entry << std::endl;
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
	
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetObjectBitSize(MEMORY_ADDR& bit_size) const
{
	const DWARF_Reference<MEMORY_ADDR> *dw_type_ref = 0;
	
	if(GetAttributeValue(DW_AT_type, dw_type_ref))
	{
		const DWARF_DIE<MEMORY_ADDR> *dw_die_type = dw_type_ref->GetValue();
		
		while(dw_die_type->GetTag() == DW_TAG_typedef)
		{
			if(!dw_die_type->GetAttributeValue(DW_AT_type, dw_type_ref)) return false;
			dw_die_type = dw_type_ref->GetValue();
		}
		
		return dw_die_type->GetBitSize(bit_size);
	}
	return GetBitSize(bit_size);
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetLocation(MEMORY_ADDR pc, DWARF_Location<MEMORY_ADDR>& loc) const
{
	const DWARF_Expression<MEMORY_ADDR> *dw_loc_expr = 0;
	std::set<std::pair<MEMORY_ADDR, MEMORY_ADDR> > ranges;
	if(!GetLocationExpression(DW_AT_location, pc, dw_loc_expr, ranges)) return false;
	//std::cerr << "GetLocation: dw_loc_expr=0x" << std::hex << *dw_loc_expr << std::dec << std::endl;

	MEMORY_ADDR frame_base;
	if(!dw_cu->GetFrameBase(pc, frame_base)) return false;
	//std::cerr << "GetLocation: frame_base=0x" << std::hex << frame_base << std::dec << std::endl;
	
	MEMORY_ADDR addr;
	DWARF_ExpressionVM<MEMORY_ADDR> dw_loc_expr_vm = DWARF_ExpressionVM<MEMORY_ADDR>(dw_cu->GetHandler());
	dw_loc_expr_vm.SetFrameBase(frame_base);
	bool dw_loc_expr_vm_status = dw_loc_expr_vm.Execute(dw_loc_expr, addr, &loc);
 	//std::cerr << "GetLocation: dw_loc_expr_vm_status=" << dw_loc_expr_vm_status << std::endl;
	return dw_loc_expr_vm_status;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::GetDataMemberLocation(MEMORY_ADDR pc, MEMORY_ADDR object_addr, DWARF_Location<MEMORY_ADDR>& loc) const
{
	const DWARF_Expression<MEMORY_ADDR> *dw_loc_expr = 0;
	std::set<std::pair<MEMORY_ADDR, MEMORY_ADDR> > ranges;
	if(!GetLocationExpression(DW_AT_data_member_location, pc, dw_loc_expr, ranges)) return false;
	//std::cerr << "GetDataMemberLocation: dw_loc_expr=0x" << std::hex << *dw_loc_expr << std::dec << std::endl;

	MEMORY_ADDR frame_base;
	if(!dw_cu->GetFrameBase(pc, frame_base)) return false;
	//std::cerr << "GetDataMemberLocation: frame_base=0x" << std::hex << frame_base << std::dec << std::endl;
	
	MEMORY_ADDR addr;
	DWARF_ExpressionVM<MEMORY_ADDR> dw_loc_expr_vm = DWARF_ExpressionVM<MEMORY_ADDR>(dw_cu->GetHandler());
	dw_loc_expr_vm.SetFrameBase(frame_base);
	dw_loc_expr_vm.SetObjectAddress(object_addr);
	dw_loc_expr_vm.Push(object_addr);
	bool dw_loc_expr_vm_status = dw_loc_expr_vm.Execute(dw_loc_expr, addr, &loc);
 	//std::cerr << "GetDataMemberLocation: dw_loc_expr_vm_status=" << dw_loc_expr_vm_status << std::endl;
	return dw_loc_expr_vm_status;
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

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
