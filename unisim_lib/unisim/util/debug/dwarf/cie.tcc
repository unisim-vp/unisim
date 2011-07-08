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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_CIE_TCC__
#define __UNISIM_UTIL_DEBUG_DWARF_CIE_TCC__

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
DWARF_CIE<MEMORY_ADDR>::DWARF_CIE(DWARF_Handler<MEMORY_ADDR> *_dw_handler)
	: dw_handler(_dw_handler)
	, dw_fmt(FMT_DWARF32)
	, offset(0)
	, id(0)
	, length(0)
	, cie_id(0)
	, version(0)
	, augmentation(0)
	, code_alignment_factor()
	, data_alignment_factor()
	, dw2_return_address_register(0)
	, dw3_return_address_register()
	, dw_initial_call_frame_prog(0)
{
}

template <class MEMORY_ADDR>
DWARF_CIE<MEMORY_ADDR>::~DWARF_CIE()
{
	if(dw_initial_call_frame_prog)
	{
		delete dw_initial_call_frame_prog;
	}
}

template <class MEMORY_ADDR>
int64_t DWARF_CIE<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size, uint64_t _offset)
{
	offset = _offset;
	endian_type endianness = dw_handler->GetEndianness();
	uint32_t length32;
	
	uint64_t size = 0;
	int64_t sz;
	if(max_size < sizeof(length32)) return -1;
	memcpy(&length32, rawdata, sizeof(length32));
	length32 = Target2Host(endianness, length32);
	rawdata += sizeof(length32);
	max_size -= sizeof(length32);
	size += sizeof(length32);
	
	dw_fmt = (length32 == 0xffffffffUL) ? FMT_DWARF64 : FMT_DWARF32;
	
	if(dw_fmt == FMT_DWARF64)
	{
		uint64_t length64;
		if(max_size < sizeof(length64)) return -1;
		memcpy(&length64, rawdata, sizeof(length64));
		length64 = Target2Host(endianness, length64);
		rawdata += sizeof(length64);
		max_size -= sizeof(length64);
		size += sizeof(length64);
		length = length64;
	}
	else
	{
		length = length32;
	}
	
	if(dw_fmt == FMT_DWARF64)
	{
		uint64_t cie_id64;
		if(max_size < sizeof(cie_id64)) return -1;
		memcpy(&cie_id64, rawdata, sizeof(cie_id64));
		cie_id64 = Target2Host(endianness, cie_id64);
		rawdata += sizeof(cie_id64);
		max_size -= sizeof(cie_id64);
		size += sizeof(cie_id64);
		if(cie_id64 != 0xffffffffffffffffULL) return -1;
		cie_id = cie_id64;
	}
	else
	{
		uint32_t cie_id32;
		if(max_size < sizeof(cie_id32)) return -1;
		memcpy(&cie_id32, rawdata, sizeof(cie_id32));
		cie_id32 = Target2Host(endianness, cie_id32);
		rawdata += sizeof(cie_id32);
		max_size -= sizeof(cie_id32);
		size += sizeof(cie_id32);
		if(cie_id32 != 0xffffffffULL) return -1;
		cie_id = cie_id32;
	}

	if(max_size < sizeof(version)) return -1;
	memcpy(&version, rawdata, sizeof(version));
	version = Target2Host(endianness, version);
	rawdata += sizeof(version);
	max_size -= sizeof(version);
	size += sizeof(version);

	if((version != 1) && (version != 3)) return -1;
	
	augmentation = (const char *) rawdata;
	unsigned int augmentation_length = strlen(augmentation);
	if(max_size < (augmentation_length + 1)) return -1;
	size += augmentation_length + 1;
	rawdata += augmentation_length + 1;
	max_size -= augmentation_length + 1;
	
	if((sz = code_alignment_factor.Load(rawdata, max_size)) < 0) return -1;
	rawdata += sz;
	max_size -= sz;
	size += sz;
	
	if((sz = data_alignment_factor.Load(rawdata, max_size)) < 0) return -1;
	rawdata += sz;
	max_size -= sz;
	size += sz;
	
	if(version == 1)
	{
		// DWARF v2
		if(max_size < sizeof(dw2_return_address_register)) return -1;
		memcpy(&dw2_return_address_register, rawdata, sizeof(dw2_return_address_register));
		dw2_return_address_register = Target2Host(endianness, dw2_return_address_register);
		rawdata += sizeof(dw2_return_address_register);
		max_size -= sizeof(dw2_return_address_register);
		size += sizeof(dw2_return_address_register);
	}
	else
	{
		// DWARF v3
		if((sz = dw3_return_address_register.Load(rawdata, max_size)) < 0) return -1;
		rawdata += sz;
		max_size -= sz;
		size += sz;
	}
	
	uint64_t initial_instructions_length;
	if(dw_fmt == FMT_DWARF64)
	{
		if(size > length + sizeof(uint32_t) + sizeof(uint64_t)) return -1;
		initial_instructions_length = length + sizeof(uint32_t) + sizeof(uint64_t) - size; 
	}
	else
	{
		if(size > length + sizeof(uint32_t)) return -1;
		initial_instructions_length = length + sizeof(uint32_t) - size; 
	}
	
	if(initial_instructions_length > max_size) return -1;
	dw_initial_call_frame_prog = new DWARF_CallFrameProgram<MEMORY_ADDR>(dw_handler, initial_instructions_length, rawdata, DW_CFP_INITIAL_INSTRUCTIONS);
	dw_initial_call_frame_prog->SetCIE(this);
	size += initial_instructions_length;

	return size;
}

template <class MEMORY_ADDR>
void DWARF_CIE<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int _id)
{
	id = _id;
}

template <class MEMORY_ADDR>
std::string DWARF_CIE<MEMORY_ADDR>::GetHREF() const
{
	std::stringstream sstr;
	sstr << "debug_frame/cies/" << (id / cies_per_file) << ".html#cie-" << id;
	return sstr.str();
}

template <class MEMORY_ADDR>
unsigned int DWARF_CIE<MEMORY_ADDR>::GetId() const
{
	return id;
}

template <class MEMORY_ADDR>
const DWARF_LEB128& DWARF_CIE<MEMORY_ADDR>::GetCodeAlignmentFactor() const
{
	return code_alignment_factor;
}

template <class MEMORY_ADDR>
const DWARF_LEB128& DWARF_CIE<MEMORY_ADDR>::GetDataAlignmentFactor() const
{
	return data_alignment_factor;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_CIE<MEMORY_ADDR>::to_XML(std::ostream& os) const
{
	os << "<DW_CIE offset=\"" << offset << "\" version=\"" << (uint32_t) version << "\" augmentation=\"";
	c_string_to_XML(os, augmentation);
	os << "\" code_alignment_factor=\"" << code_alignment_factor.to_string(false) << "\" data_alignment_factor=\"" << data_alignment_factor.to_string(true) << "\" return_address_register=\"";
	if(version == 1)
	{
		os << (uint32_t) dw2_return_address_register;
	}
	else
	{
		os << dw3_return_address_register.to_string(false);
	}
	os << "\" initial_call_frame_program=\"";
	std::stringstream sstr;
	sstr << *dw_initial_call_frame_prog;
	c_string_to_XML(os, sstr.str().c_str());
	os << "\"/>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_CIE<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	os << "<tr id=\"cie-" << id << "\">" << std::endl;
	os << "<td>" << id << "</td><td>" << offset << "</td><td>" << (uint32_t) version << "</td><td>";
	c_string_to_HTML(os, augmentation);
	os << "</td><td>" << code_alignment_factor.to_string(false) << "</td><td>" << data_alignment_factor.to_string(true) << "</td><td>";
	if(version == 1)
	{
		os << (uint32_t) dw2_return_address_register;
	}
	else
	{
		os << dw3_return_address_register.to_string(false);
	}
	os << "</td><td>";
	std::stringstream sstr;
	sstr << *dw_initial_call_frame_prog;
	c_string_to_HTML(os, sstr.str().c_str());
	os << "</td></tr>" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_CIE<MEMORY_ADDR>& dw_cie)
{
	os << "Common Information Entry:" << std::endl;
	os << " - Offset: " << dw_cie.offset << std::endl;
	os << " - Version: " << (uint32_t) dw_cie.version << std::endl;          // Independent of DWARF version number. DWARF v2/version=1; DWARF v3/version=3
	os << " - Augmentation: " << dw_cie.augmentation << std::endl;
	os << " - Code alignment factor: " << dw_cie.code_alignment_factor.to_string(false) << std::endl;
	os << " - Data alignment factor: " << dw_cie.data_alignment_factor.to_string(true) << std::endl;
	os << " - Return address register: ";
	if(dw_cie.version == 1)
	{
		os << (uint32_t) dw_cie.dw2_return_address_register;
	}
	else
	{
		os << dw_cie.dw3_return_address_register.to_string(false);
	}
	os << std::endl;
	os << " - Initial instructions (" << dw_cie.dw_initial_call_frame_prog->GetLength() << " bytes): " << *dw_cie.dw_initial_call_frame_prog << std::endl;
	return os;
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
