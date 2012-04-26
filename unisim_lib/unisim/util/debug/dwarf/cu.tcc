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
 * Authors: Gilles Mouchard (gilles.mouch.fr)
 */

#ifndef __UNISIM_UTIL_DEBUG_DWARF_CU_TCC__
#define __UNISIM_UTIL_DEBUG_DWARF_CU_TCC__

#include <unisim/util/debug/dwarf/cu.hh>
#include <unisim/util/debug/dwarf/ml.hh>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
DWARF_CompilationUnit<MEMORY_ADDR>::DWARF_CompilationUnit(DWARF_Handler<MEMORY_ADDR> *_dw_handler)
	: dw_handler(_dw_handler)
	, offset(0xffffffffffffffffULL)
	, unit_length(0)
	, version(0)
	, debug_abbrev_offset(0)
	, address_size(0)
{
}

template <class MEMORY_ADDR>
DWARF_CompilationUnit<MEMORY_ADDR>::~DWARF_CompilationUnit()
{
	unsigned int num_debug_info_entries = debug_info_entries.size();
	unsigned int i;
	for(i = 0; i < num_debug_info_entries; i++)
	{
		delete debug_info_entries[i];
	}
}

template <class MEMORY_ADDR>
endian_type DWARF_CompilationUnit<MEMORY_ADDR>::GetEndianness() const
{
	return dw_handler->GetEndianness();
}

template <class MEMORY_ADDR>
DWARF_Format DWARF_CompilationUnit<MEMORY_ADDR>::GetFormat() const
{
	return dw_fmt;
}

template <class MEMORY_ADDR>
uint8_t DWARF_CompilationUnit<MEMORY_ADDR>::GetAddressSize() const
{
	return address_size;
}

template <class MEMORY_ADDR>
uint16_t DWARF_CompilationUnit<MEMORY_ADDR>::GetVersion() const
{
	return version;
}

template <class MEMORY_ADDR>
uint8_t DWARF_CompilationUnit<MEMORY_ADDR>::GetOffsetSize() const
{
	return (version == 3) ? ((dw_fmt == FMT_DWARF64) ? 8 : 4) : address_size;
}

template <class MEMORY_ADDR>
uint64_t DWARF_CompilationUnit<MEMORY_ADDR>::GetOffset() const
{
	return offset;
}

template <class MEMORY_ADDR>
DWARF_Handler<MEMORY_ADDR> *DWARF_CompilationUnit<MEMORY_ADDR>::GetHandler() const
{
	return dw_handler;
}

template <class MEMORY_ADDR>
const char *DWARF_CompilationUnit<MEMORY_ADDR>::GetString(uint64_t debug_str_offset) const
{
	return dw_handler->GetString(debug_str_offset);
}

template <class MEMORY_ADDR>
const DWARF_Abbrev *DWARF_CompilationUnit<MEMORY_ADDR>::FindAbbrev(const DWARF_LEB128& dw_abbrev_code) const
{
	return dw_handler->FindAbbrev(debug_abbrev_offset, dw_abbrev_code);
}

template <class MEMORY_ADDR>
int64_t DWARF_CompilationUnit<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size, uint64_t _offset)
{
	offset = _offset;
	endian_type endianness = dw_handler->GetEndianness();
	uint32_t unit_length32;
	
	uint64_t size = 0;
	if(max_size < sizeof(unit_length32)) return -1;
	memcpy(&unit_length32, rawdata, sizeof(unit_length32));
	unit_length32 = Target2Host(endianness, unit_length32);
	rawdata += sizeof(unit_length32);
	max_size -= sizeof(unit_length32);
	size += sizeof(unit_length32);
	
	dw_fmt = (unit_length32 == 0xffffffffUL) ? FMT_DWARF64 : FMT_DWARF32;
	
	if(dw_fmt == FMT_DWARF64)
	{
		uint64_t unit_length64;
		if(max_size < sizeof(unit_length64)) return -1;
		memcpy(&unit_length64, rawdata, sizeof(unit_length64));
		unit_length64 = Target2Host(endianness, unit_length64);
		rawdata += sizeof(unit_length64);
		max_size -= sizeof(unit_length64);
		size += sizeof(unit_length64);
		unit_length = unit_length64;
	}
	else
	{
		unit_length = unit_length32;
	}
		
	if(max_size < unit_length) return -1;
	
	if(max_size < sizeof(version)) return -1;
	memcpy(&version, rawdata, sizeof(version));
	version = Target2Host(endianness, version);
	rawdata += sizeof(version);
	max_size -= sizeof(version);
	size += sizeof(version);

	if(version != 2 && version != 3) return -1;
	
	if(dw_fmt == FMT_DWARF64)
	{
		uint64_t debug_abbrev_offset64;
	
		if(max_size < sizeof(debug_abbrev_offset64)) return -1;
		memcpy(&debug_abbrev_offset64, rawdata, sizeof(debug_abbrev_offset64));
		debug_abbrev_offset64 = Target2Host(endianness, debug_abbrev_offset64);
		rawdata += sizeof(debug_abbrev_offset64);
		max_size -= sizeof(debug_abbrev_offset64);
		size += sizeof(debug_abbrev_offset64);
		debug_abbrev_offset = debug_abbrev_offset64;
	}
	else
	{
		uint32_t debug_abbrev_offset32;
	
		if(max_size < sizeof(debug_abbrev_offset32)) return -1;
		memcpy(&debug_abbrev_offset32, rawdata, sizeof(debug_abbrev_offset32));
		debug_abbrev_offset32 = Target2Host(endianness, debug_abbrev_offset32);
		rawdata += sizeof(debug_abbrev_offset32);
		max_size -= sizeof(debug_abbrev_offset32);
		size += sizeof(debug_abbrev_offset32);
		debug_abbrev_offset = debug_abbrev_offset32;
	}

	if(max_size < sizeof(address_size)) return -1;
	memcpy(&address_size, rawdata, sizeof(address_size));
	address_size = Target2Host(endianness, address_size);
	rawdata += sizeof(address_size);
	max_size -= sizeof(address_size);
	size += sizeof(address_size);
	
 	do
 	{
		DWARF_DIE<MEMORY_ADDR> *dw_die = new DWARF_DIE<MEMORY_ADDR>(this);
		
		int64_t sz;
		if((sz = dw_die->Load(rawdata, max_size, offset + size)) < 0)
		{
			delete dw_die;
			return -1;
		}
		rawdata += sz;
		max_size -= sz;
		size += sz;
		
		debug_info_entries.push_back(dw_die);
		Register(dw_die);
 	}
 	while(size < ((dw_fmt == FMT_DWARF64) ? (unit_length + sizeof(uint32_t) + sizeof(uint64_t)) : (unit_length + sizeof(uint32_t))));
	return size;
}

template <class MEMORY_ADDR>
void DWARF_CompilationUnit<MEMORY_ADDR>::Register(DWARF_DIE<MEMORY_ADDR> *dw_die)
{
	dw_handler->Register(dw_die);
}

template <class MEMORY_ADDR>
void DWARF_CompilationUnit<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int _id)
{
	id = _id;
}

template <class MEMORY_ADDR>
std::string DWARF_CompilationUnit<MEMORY_ADDR>::GetHREF() const
{
	std::stringstream sstr;
	sstr << "debug_info/cus/" << (id / cus_per_file) << ".html#cu-" << id;
	return sstr.str();
}

template <class MEMORY_ADDR>
unsigned int DWARF_CompilationUnit<MEMORY_ADDR>::GetId() const
{
	return id;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_CompilationUnit<MEMORY_ADDR>::to_XML(std::ostream& os)
{
	os << "<DW_CU offset=\"" << offset
	   << "\" unit_length=\"" << unit_length
	   << "\" version=\"" << version
	   << "\" debug_abbrev_offset=\"" << debug_abbrev_offset
	   << "\" address_size=\"" << (uint32_t) address_size
	   << "\">" << std::endl;
	unsigned int num_debug_info_entries = debug_info_entries.size();
	unsigned int i;
	for(i = 0; i < num_debug_info_entries; i++)
	{
		debug_info_entries[i]->to_XML(os) << std::endl;
	}
	os << "</DW_CU>" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_CompilationUnit<MEMORY_ADDR>::to_HTML(std::ostream& os)
{
	os << "<tr id=\"cu-" << id << "\">" << std::endl;
	os << "<td>" << id << "</td><td>" << offset << "</td><td>" << (uint32_t) version << "</td><td>" << (uint32_t) address_size << "</td>" << std::endl;
	os << "<td>";
	unsigned int num_debug_info_entries = debug_info_entries.size();
	unsigned int i;
	for(i = 0; i < num_debug_info_entries; i++)
	{
		if(i != 0) os << "&nbsp;";
		os << "<a href=\"../../" << debug_info_entries[i]->GetHREF() << "\">die #" << debug_info_entries[i]->GetId() << "</a>";
	}
	os << "</td>" << std::endl;
	os << "</tr>" << std::endl;

	return os;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_CompilationUnit<MEMORY_ADDR>& dw_cu)
{
	os << "Compilation Unit Header:" << std::endl
	   << " - Offset: " << dw_cu.offset << std::endl
	   << " - Unit Length: " << dw_cu.unit_length << std::endl
	   << " - DWARF Version: " << dw_cu.version << std::endl
	   << " - Offset in .debug_abbrev: " << dw_cu.debug_abbrev_offset << std::endl
	   << " - Address Size: " << (uint32_t) dw_cu.address_size << std::endl
	   << " - Debug Info Entries:" << std::endl;
	unsigned int num_debug_info_entries = dw_cu.debug_info_entries.size();
	unsigned int i;
	for(i = 0; i < num_debug_info_entries; i++)
	{
		os << *dw_cu.debug_info_entries[i] << std::endl;
	}
	return os;
}

template <class MEMORY_ADDR>
void DWARF_CompilationUnit<MEMORY_ADDR>::BuildStatementMatrix(std::map<MEMORY_ADDR, const Statement<MEMORY_ADDR> *>& stmt_matrix)
{
	unsigned int num_debug_info_entries = debug_info_entries.size();
	unsigned int i;
	for(i = 0; i < num_debug_info_entries; i++)
	{
		DWARF_DIE<MEMORY_ADDR> *dw_die = debug_info_entries[i];
		
		dw_die->BuildStatementMatrix(stmt_matrix);
	}
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
