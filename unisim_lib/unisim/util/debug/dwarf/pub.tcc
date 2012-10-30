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

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
DWARF_Pub<MEMORY_ADDR>::DWARF_Pub(const DWARF_Pubs<MEMORY_ADDR> *_dw_pubs)
	: dw_pubs(_dw_pubs)
	, dw_die(0)
	, debug_info_offset(0xffffffffffffffffULL)
	, name(0)
{
}

template <class MEMORY_ADDR>
DWARF_Pub<MEMORY_ADDR>::~DWARF_Pub()
{
}

template <class MEMORY_ADDR>
int64_t DWARF_Pub<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size)
{
	DWARF_Format dw_fmt = dw_pubs->GetFormat();
	endian_type endianness = dw_pubs->GetEndianness();
	int64_t size = 0;
	
	if(dw_fmt == FMT_DWARF64)
	{
		uint64_t debug_info_offset64;
	
		if(max_size < sizeof(debug_info_offset64)) return -1;
		memcpy(&debug_info_offset64, rawdata, sizeof(debug_info_offset64));
		debug_info_offset64 = Target2Host(endianness, debug_info_offset64);
		rawdata += sizeof(debug_info_offset64);
		max_size -= sizeof(debug_info_offset64);
		size += sizeof(debug_info_offset64);
		debug_info_offset = debug_info_offset64;
	}
	else
	{
		uint32_t debug_info_offset32;
	
		if(max_size < sizeof(debug_info_offset32)) return -1;
		memcpy(&debug_info_offset32, rawdata, sizeof(debug_info_offset32));
		debug_info_offset32 = Target2Host(endianness, debug_info_offset32);
		rawdata += sizeof(debug_info_offset32);
		max_size -= sizeof(debug_info_offset32);
		size += sizeof(debug_info_offset32);
		debug_info_offset = debug_info_offset32;
	}

	if(debug_info_offset == 0) return size;
	
	name = (const char *) rawdata;
	uint64_t sz = strlen(name) + 1;
	if(max_size < sz) return -1;
	size += sz;
	rawdata += sz;
	max_size -= sz;

	return size;
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_Pub<MEMORY_ADDR>::GetDIE() const
{
	return dw_die;
}

template <class MEMORY_ADDR>
bool DWARF_Pub<MEMORY_ADDR>::IsNull() const
{
	return debug_info_offset == 0;
}

template <class MEMORY_ADDR>
void DWARF_Pub<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler)
{
	uint64_t cu_debug_info_offset = dw_pubs->GetDebugInfoOffset();
	dw_die = dw_handler->FindDIE(cu_debug_info_offset + debug_info_offset);
	if(!dw_die)
	{
		dw_handler->GetLogger() << DebugWarning << "While resolving [pub entry -> DIE] reference, can't find DIE (Debugging Information Entry) in .debug_info at offset " << (cu_debug_info_offset + debug_info_offset) << EndDebugWarning;
	}
}

template <class MEMORY_ADDR>
std::ostream& DWARF_Pub<MEMORY_ADDR>::to_XML(std::ostream& os) const
{
	uint64_t cu_debug_info_offset = dw_pubs->GetDebugInfoOffset();
	os << "<DW_PUB abs_debug_info_offset=\"" << (cu_debug_info_offset + debug_info_offset) << "\" debug_info_offset=\"" << debug_info_offset << "\" name=\"" << name << "\"/>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_Pub<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	os << "<tr><td>" << name << "</td><td><a href=\"../" << dw_die->GetHREF() << "\">die-" << dw_die->GetId() << "</a></td></tr>" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_Pub<MEMORY_ADDR>& dw_pub)
{
	os << "   - Offset in .debug_info (rel. to CU): " << dw_pub.debug_info_offset << std::endl;
	os << "   - Name: \"" << dw_pub.name << "\"";
	return os;
}

template <class MEMORY_ADDR>
DWARF_Pubs<MEMORY_ADDR>::DWARF_Pubs(DWARF_Handler<MEMORY_ADDR> *_dw_handler)
	: dw_handler(_dw_handler)
	, dw_fmt(FMT_DWARF32)
	, offset(0xffffffffffffffffULL)
	, dw_cu(0)
	, unit_length(0)
	, version(0)
	, debug_info_offset(0xffffffffffffffffULL)
	, debug_info_length(0)
	, dw_pubs()
{
}

template <class MEMORY_ADDR>
DWARF_Pubs<MEMORY_ADDR>::~DWARF_Pubs()
{
	unsigned int num_pubs = dw_pubs.size();
	
	unsigned int i;
	for(i = 0; i < num_pubs; i++)
	{
		delete dw_pubs[i];
	}
}

template <class MEMORY_ADDR>
uint64_t DWARF_Pubs<MEMORY_ADDR>::GetDebugInfoOffset() const
{
	return debug_info_offset;
}

template <class MEMORY_ADDR>
endian_type DWARF_Pubs<MEMORY_ADDR>::GetEndianness() const
{
	return dw_handler->GetEndianness();
}

template <class MEMORY_ADDR>
DWARF_Format DWARF_Pubs<MEMORY_ADDR>::GetFormat() const
{
	return dw_fmt;
}

template <class MEMORY_ADDR>
int64_t DWARF_Pubs<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size)
{
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

	if(version != 2) return -1; // 2=(DWARF2 or DWARF3)
	
	if(dw_fmt == FMT_DWARF64)
	{
		uint64_t debug_info_offset64;
	
		if(max_size < sizeof(debug_info_offset64)) return -1;
		memcpy(&debug_info_offset64, rawdata, sizeof(debug_info_offset64));
		debug_info_offset64 = Target2Host(endianness, debug_info_offset64);
		rawdata += sizeof(debug_info_offset64);
		max_size -= sizeof(debug_info_offset64);
		size += sizeof(debug_info_offset64);
		debug_info_offset = debug_info_offset64;

		uint64_t debug_info_length64;
	
		if(max_size < sizeof(debug_info_length64)) return -1;
		memcpy(&debug_info_length64, rawdata, sizeof(debug_info_length64));
		debug_info_length64 = Target2Host(endianness, debug_info_length64);
		rawdata += sizeof(debug_info_length64);
		max_size -= sizeof(debug_info_length64);
		size += sizeof(debug_info_length64);
		debug_info_length = debug_info_length64;
	}
	else
	{
		uint32_t debug_info_offset32;
	
		if(max_size < sizeof(debug_info_offset32)) return -1;
		memcpy(&debug_info_offset32, rawdata, sizeof(debug_info_offset32));
		debug_info_offset32 = Target2Host(endianness, debug_info_offset32);
		rawdata += sizeof(debug_info_offset32);
		max_size -= sizeof(debug_info_offset32);
		size += sizeof(debug_info_offset32);
		debug_info_offset = debug_info_offset32;

		uint32_t debug_info_length32;
	
		if(max_size < sizeof(debug_info_length32)) return -1;
		memcpy(&debug_info_length32, rawdata, sizeof(debug_info_length32));
		debug_info_length32 = Target2Host(endianness, debug_info_length32);
		rawdata += sizeof(debug_info_length32);
		max_size -= sizeof(debug_info_length32);
		size += sizeof(debug_info_length32);
		debug_info_length = debug_info_length32;
	}

	do
	{
		DWARF_Pub<MEMORY_ADDR> *dw_pub = new DWARF_Pub<MEMORY_ADDR>(this);
		
		int64_t sz;
		if((sz = dw_pub->Load(rawdata, max_size)) < 0)
		{
			delete dw_pub;
			return -1;
		}
		rawdata += sz;
		max_size -= sz;
		size += sz;
		
		if(dw_pub->IsNull())
		{
			delete dw_pub;
			break;
		}
		
		dw_pubs.push_back(dw_pub);
	}
	while(size < ((dw_fmt == FMT_DWARF64) ? (unit_length + sizeof(uint32_t) + sizeof(uint64_t)) : (unit_length + sizeof(uint32_t))));
	
	size = (dw_fmt == FMT_DWARF64) ? (unit_length + sizeof(uint32_t) + sizeof(uint64_t)) : (unit_length + sizeof(uint32_t));
	
	return size;
}

template <class MEMORY_ADDR>
void DWARF_Pubs<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler)
{
	dw_cu = dw_handler->FindCompilationUnit(debug_info_offset);
	if(!dw_cu)
	{
		dw_handler->GetLogger() << DebugWarning << "While resolving [pubs -> CU] reference, can't find CU (Compilation Unit) in .debug_info at offset " << debug_info_offset << EndDebugWarning;
	}
	
	unsigned int num_pubs = dw_pubs.size();
	
	unsigned int i;
	for(i = 0; i < num_pubs; i++)
	{
		dw_pubs[i]->Fix(dw_handler);
	}
}

template <class MEMORY_ADDR>
std::ostream& DWARF_Pubs<MEMORY_ADDR>::to_XML(std::ostream& os) const
{
	os << "<DW_PUBS unit_length=\"" << unit_length << "\" version=\"" << version << "\" debug_info_offset=\"" << debug_info_offset << "\" debug_info_length=\"" << debug_info_length << "\">" << std::endl;
	unsigned int num_pubs = dw_pubs.size();
	
	unsigned int i;
	for(i = 0; i < num_pubs; i++)
	{
		dw_pubs[i]->to_XML(os);
		os << std::endl;
	}
	os << "</DW_PUBS>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_Pubs<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	os << "<tr><td>" << version << "</td><td><a href=\"../" << dw_cu->GetHREF() << "\">cu-" << dw_cu->GetId() << "</a></td><td><table>";
	unsigned int num_pubs = dw_pubs.size();
	
	unsigned int i;
	for(i = 0; i < num_pubs; i++)
	{
		dw_pubs[i]->to_HTML(os);
		os << std::endl;
	}
	os << "</table></td></tr>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_Pubs<MEMORY_ADDR>& dw_pubs)
{
	os << "Pubs:" << std::endl;
	os << " - Unit length: " << dw_pubs.unit_length << std::endl;
	os << " - Version: " << dw_pubs.version << std::endl;
	os << " - Offset in .debug_info: " << dw_pubs.debug_info_offset << std::endl;
	os << " - Length in .debug_info: " << dw_pubs.debug_info_length << std::endl;
	unsigned int num_pubs = dw_pubs.dw_pubs.size();
	
	unsigned int i;
	for(i = 0; i < num_pubs; i++)
	{
		os << *dw_pubs.dw_pubs[i] << std::endl;
	}
	return os;
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim
