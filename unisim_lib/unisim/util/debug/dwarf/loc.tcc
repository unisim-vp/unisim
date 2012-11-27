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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_LOC_TCC__
#define __UNISIM_UTIL_DEBUG_DWARF_LOC_TCC__

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
DWARF_LocListEntry<MEMORY_ADDR>::DWARF_LocListEntry(const DWARF_CompilationUnit<MEMORY_ADDR> *_dw_cu)
	: offset(0xffffffffffffffffULL)
	, dw_cu(_dw_cu)
	, next(0)
	, begin_addr_offset(0)
	, end_addr_offset(0)
	, dw_loc_expr(0)
{
}

template <class MEMORY_ADDR>
DWARF_LocListEntry<MEMORY_ADDR>::~DWARF_LocListEntry()
{
	if(dw_loc_expr)
	{
		delete dw_loc_expr;
	}
}

template <class MEMORY_ADDR>
MEMORY_ADDR DWARF_LocListEntry<MEMORY_ADDR>::GetBeginAddressOffset() const
{
	return begin_addr_offset;
}

template <class MEMORY_ADDR>
MEMORY_ADDR DWARF_LocListEntry<MEMORY_ADDR>::GetEndAddressOffset() const
{
	return end_addr_offset;
}

template <class MEMORY_ADDR>
bool DWARF_LocListEntry<MEMORY_ADDR>::IsBaseAddressSelection() const
{
	return begin_addr_offset > end_addr_offset;
}

template <class MEMORY_ADDR>
bool DWARF_LocListEntry<MEMORY_ADDR>::IsEndOfList() const
{
	return (begin_addr_offset == 0) && (end_addr_offset == 0);
}

template <class MEMORY_ADDR>
uint64_t DWARF_LocListEntry<MEMORY_ADDR>::GetOffset() const
{
	return offset;
}

template <class MEMORY_ADDR>
const DWARF_Expression<MEMORY_ADDR> *DWARF_LocListEntry<MEMORY_ADDR>::GetLocationExpression() const
{
	return dw_loc_expr;
}

template <class MEMORY_ADDR>
void DWARF_LocListEntry<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int _id)
{
	id = _id;
}

template <class MEMORY_ADDR>
std::string DWARF_LocListEntry<MEMORY_ADDR>::GetHREF() const
{
	std::stringstream sstr;
	sstr << "debug_loc/" << (id / debug_range_per_file) << ".html#loc-" << id;
	return sstr.str();
}

template <class MEMORY_ADDR>
unsigned int DWARF_LocListEntry<MEMORY_ADDR>::GetId() const
{
	return id;
}

template <class MEMORY_ADDR>
const DWARF_LocListEntry<MEMORY_ADDR> *DWARF_LocListEntry<MEMORY_ADDR>::GetNext() const
{
	return next;
}

template <class MEMORY_ADDR>
int64_t DWARF_LocListEntry<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size, uint64_t _offset)
{
	offset = _offset;
	int64_t size = 0;
	
	endian_type file_endianness = dw_cu->GetHandler()->GetFileEndianness();
	
	switch(dw_cu->GetAddressSize())
	{
		case 2:
			{
				uint16_t begin_addr_offset16;
				if(max_size < sizeof(begin_addr_offset16)) return -1;
				memcpy(&begin_addr_offset16, rawdata, sizeof(begin_addr_offset16));
				begin_addr_offset16 = Target2Host(file_endianness, begin_addr_offset16);
				rawdata += sizeof(begin_addr_offset16);
				max_size -= sizeof(begin_addr_offset16);
				size += sizeof(begin_addr_offset16);
				begin_addr_offset = (MEMORY_ADDR) begin_addr_offset16;

				uint16_t end_addr_offset16;
				if(max_size < sizeof(end_addr_offset16)) return -1;
				memcpy(&end_addr_offset16, rawdata, sizeof(end_addr_offset16));
				end_addr_offset16 = Target2Host(file_endianness, end_addr_offset16);
				rawdata += sizeof(end_addr_offset16);
				max_size -= sizeof(end_addr_offset16);
				size += sizeof(end_addr_offset16);
				end_addr_offset = (MEMORY_ADDR) end_addr_offset16;
			}
			break;
		case 4:
			{
				uint32_t begin_addr_offset32;
				if(max_size < sizeof(begin_addr_offset32)) return -1;
				memcpy(&begin_addr_offset32, rawdata, sizeof(begin_addr_offset32));
				begin_addr_offset32 = Target2Host(file_endianness, begin_addr_offset32);
				rawdata += sizeof(begin_addr_offset32);
				max_size -= sizeof(begin_addr_offset32);
				size += sizeof(begin_addr_offset32);
				begin_addr_offset = (MEMORY_ADDR) begin_addr_offset32;

				uint32_t end_addr_offset32;
				if(max_size < sizeof(end_addr_offset32)) return -1;
				memcpy(&end_addr_offset32, rawdata, sizeof(end_addr_offset32));
				end_addr_offset32 = Target2Host(file_endianness, end_addr_offset32);
				rawdata += sizeof(end_addr_offset32);
				max_size -= sizeof(end_addr_offset32);
				size += sizeof(end_addr_offset32);
				end_addr_offset = (MEMORY_ADDR) end_addr_offset32;
			}
			break;
		case 8:
			{
				uint64_t begin_addr_offset64;
				if(max_size < sizeof(begin_addr_offset64)) return -1;
				memcpy(&begin_addr_offset64, rawdata, sizeof(begin_addr_offset64));
				begin_addr_offset64 = Target2Host(file_endianness, begin_addr_offset64);
				rawdata += sizeof(begin_addr_offset64);
				max_size -= sizeof(begin_addr_offset64);
				size += sizeof(begin_addr_offset64);
				begin_addr_offset = (MEMORY_ADDR) begin_addr_offset64;

				uint64_t end_addr_offset64;
				if(max_size < sizeof(end_addr_offset64)) return -1;
				memcpy(&end_addr_offset64, rawdata, sizeof(end_addr_offset64));
				end_addr_offset64 = Target2Host(file_endianness, end_addr_offset64);
				rawdata += sizeof(end_addr_offset64);
				max_size -= sizeof(end_addr_offset64);
				size += sizeof(end_addr_offset64);
				end_addr_offset = (MEMORY_ADDR) end_addr_offset64;
			}
			break;
		default:
			return -1;
	}

	if(IsBaseAddressSelection() || IsEndOfList()) return size;
	
	// Warning! whether block_length is a 2-byte unsigned integer or an unsigned LEB128 as I initially expected is undocumented both in DWARF v2 and v3 specifications.
	uint16_t block_length;
	if(max_size < sizeof(block_length)) return -1;
	memcpy(&block_length, rawdata, sizeof(block_length));
	block_length = Target2Host(file_endianness, block_length);
	rawdata += sizeof(block_length);
	max_size -= sizeof(block_length);
	size += sizeof(block_length);

	if(max_size < block_length) return -1;
	dw_loc_expr = new DWARF_Expression<MEMORY_ADDR>(dw_cu, block_length, rawdata);
	rawdata += block_length;
	size += block_length;
	max_size -= block_length;
	
	return size;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_LocListEntry<MEMORY_ADDR>::to_XML(std::ostream& os) const
{
	if(IsEndOfList()) return os << "<DW_EOL id=\"loc-" << id << "\"/>";
	if(IsBaseAddressSelection())
	{
		os << "<DW_BASE_ADDRESS_SELECTION id=\"" << offset << "\" largest_address_offset=\"0x" << std::hex << begin_addr_offset << std::dec << "\" address=\"0x" << std::hex << end_addr_offset << std::dec << "\"/>";
		return os;
	}
	os << "<DW_LOC id=\"loc-" << id << "\" begin_address_offset=\"0x" << std::hex << begin_addr_offset << std::dec << "\" end_address_offset=\"0x" << std::hex << end_addr_offset << std::dec << "\"";
	if(dw_loc_expr)
	{
		os << " location_expression=\"";
		c_string_to_XML(os, dw_loc_expr->to_string().c_str());
		os << "\"";
	}
	os << "/>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_LocListEntry<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	os << "<tr id=\"loc-" << id << "\">" << std::endl;
	if(IsEndOfList())
	{
		os << "<td>End-of-List</td>";
	}
	else if(IsBaseAddressSelection())
	{
		os << "<td>Base address selection</td>";
	}
	else
	{
		os << "<td>Location</td>";
	}
	os << "<td>" << id << "</td>" << std::endl;
	os << "<td>" << offset << "</td>" << std::endl;
	os << "<td>";
	if(next)
	{
		os << "<a href=\"../" << next->GetHREF() << "\">loc-" << next->GetId() << "</a>" << std::endl;
	}
	else
	{
		os << "-";
	}
	os << "</td>";
	if(IsEndOfList())
	{
		os << "<td>-</td>";
	}
	else if(IsBaseAddressSelection())
	{
		os << "<td><table><tr><th>Largest address offset</th><th>Address</th></tr>";
		os << "<tr><td>0x" << std::hex << begin_addr_offset << std::dec << "</td><td>0x" << std::hex << end_addr_offset << std::dec << "</td></tr></table></td>";
	}
	else
	{
		os << "<td><table><tr><th>Beginning address offset</th><th>End Address offset</th><th>Location Expression</th></tr>";
		os << "<tr><td>0x" << std::hex << begin_addr_offset << std::dec << "</td><td>0x" << std::hex << end_addr_offset << std::dec << "</td><td>";
		c_string_to_HTML(os, dw_loc_expr->to_string().c_str());
		os << "</td></tr></table></td>";
	}
	os << "</tr>" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_LocListEntry<MEMORY_ADDR>& dw_loc_list_entry)
{
	if(dw_loc_list_entry.IsEndOfList()) return os << "EOL";
	if(dw_loc_list_entry.IsBaseAddressSelection()) os << "Base";
	os << "[0x" << std::hex << dw_loc_list_entry.begin_addr_offset << "-0x" << dw_loc_list_entry.end_addr_offset;
	if(dw_loc_list_entry.dw_loc_expr)
	{
		os << ":" << *dw_loc_list_entry.dw_loc_expr;
	}
	os << "]";
	return os;
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_DEBUG_DWARF_LOC_TCC__
