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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_DWARF_TCC__
#define __UNISIM_UTIL_DEBUG_DWARF_DWARF_TCC__

#include <unisim/util/debug/dwarf/dwarf.hh>
#include <unisim/util/debug/dwarf/ml.hh>

#include <stdlib.h>

#if defined(WIN32)
#include <io.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include <iostream>
#include <fstream>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugError;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugInfo;
	
template <class MEMORY_ADDR>
DWARF_Handler<MEMORY_ADDR>::DWARF_Handler(const unisim::util::debug::blob::Blob<MEMORY_ADDR> *_blob, unisim::kernel::logger::Logger& _logger)
	: endianness(_blob->GetEndian())
	, address_size(_blob->GetAddressSize())
	, debug_line_section(_blob->FindSection(".debug_line"))
	, debug_info_section(_blob->FindSection(".debug_info"))
	, debug_abbrev_section(_blob->FindSection(".debug_abbrev"))
	, debug_aranges_section(_blob->FindSection(".debug_aranges"))
	, debug_pubnames_section(_blob->FindSection(".debug_pubnames"))
	, debug_pubtypes_section(_blob->FindSection(".debug_pubtypes"))
	, debug_macinfo_section(_blob->FindSection(".debug_macinfo"))
	, debug_frame_section(_blob->FindSection(".debug_frame"))
	, debug_str_section(_blob->FindSection(".debug_str"))
	, debug_loc_section(_blob->FindSection(".debug_loc"))
	, debug_ranges_section(_blob->FindSection(".debug_ranges"))
	, logger(_logger)
	, blob(_blob)
{
	if(blob) blob->Catch();
	if(debug_line_section) debug_line_section->Catch();
	if(debug_info_section) debug_info_section->Catch();
	if(debug_abbrev_section) debug_abbrev_section->Catch();
	if(debug_aranges_section) debug_aranges_section->Catch();
	if(debug_pubnames_section) debug_pubnames_section->Catch();
	if(debug_pubtypes_section) debug_pubtypes_section->Catch();
	if(debug_macinfo_section) debug_macinfo_section->Catch();
	if(debug_frame_section) debug_frame_section->Catch();
	if(debug_str_section) debug_str_section->Catch();
	if(debug_loc_section) debug_loc_section->Catch();
	if(debug_ranges_section) debug_ranges_section->Catch();
}

template <class MEMORY_ADDR>
DWARF_Handler<MEMORY_ADDR>::~DWARF_Handler()
{
	unsigned int i;

	typename std::map<MEMORY_ADDR, Statement<MEMORY_ADDR> *>::const_iterator stmt_iter;

	for(stmt_iter = stmt_matrix.begin(); stmt_iter != stmt_matrix.end(); stmt_iter++)
	{
		if((*stmt_iter).second)
		{
			delete (*stmt_iter).second;
		}
	}

	typename std::map<uint64_t, DWARF_StatementProgram<MEMORY_ADDR> *>::iterator dw_stmt_prog_iter;
	for(dw_stmt_prog_iter = dw_stmt_progs.begin(); dw_stmt_prog_iter != dw_stmt_progs.end(); dw_stmt_prog_iter++)
	{
		if((*dw_stmt_prog_iter).second)
		{
			delete (*dw_stmt_prog_iter).second;
		}
	}

	typename std::map<uint64_t, DWARF_Abbrev *>::iterator dw_abbrev_iter;
	for(dw_abbrev_iter = dw_abbrevs.begin(); dw_abbrev_iter != dw_abbrevs.end(); dw_abbrev_iter++)
	{
		if((*dw_abbrev_iter).second)
		{
			delete (*dw_abbrev_iter).second;
		}
	}

	typename std::map<uint64_t, DWARF_CompilationUnit<MEMORY_ADDR> *>::iterator dw_cu_iter;
	for(dw_cu_iter = dw_cus.begin(); dw_cu_iter != dw_cus.end(); dw_cu_iter++)
	{
		if((*dw_cu_iter).second)
		{
			delete (*dw_cu_iter).second;
		}
	}

	unsigned int num_fdes = dw_fdes.size();
	for(i = 0; i < num_fdes; i++)
	{
		delete dw_fdes[i];
	}

	typename std::map<uint64_t, DWARF_CIE<MEMORY_ADDR> *>::iterator dw_cie_iter;
	for(dw_cie_iter = dw_cies.begin(); dw_cie_iter != dw_cies.end(); dw_cie_iter++)
	{
		if((*dw_cie_iter).second)
		{
			delete (*dw_cie_iter).second;
		}
	}

	typename std::map<uint64_t, DWARF_RangeListEntry<MEMORY_ADDR> *>::iterator dw_range_list_entry_iter;
	for(dw_range_list_entry_iter = dw_range_list.begin(); dw_range_list_entry_iter != dw_range_list.end(); dw_range_list_entry_iter++)
	{
		if((*dw_range_list_entry_iter).second)
		{
			delete (*dw_range_list_entry_iter).second;
		}
	}

	typename std::map<uint64_t, DWARF_MacInfoListEntry<MEMORY_ADDR> *>::iterator dw_macinfo_list_entry_iter;
	for(dw_macinfo_list_entry_iter = dw_macinfo_list.begin(); dw_macinfo_list_entry_iter != dw_macinfo_list.end(); dw_macinfo_list_entry_iter++)
	{
		if((*dw_macinfo_list_entry_iter).second)
		{
			delete (*dw_macinfo_list_entry_iter).second;
		}
	}

	unsigned int num_aranges = dw_aranges.size();
	for(i = 0; i < num_aranges; i++)
	{
		delete dw_aranges[i];
	}

	unsigned int num_pubnames = dw_pubnames.size();
	for(i = 0; i < num_pubnames; i++)
	{
		delete dw_pubnames[i];
	}

	unsigned int num_pubtypes = dw_pubtypes.size();
	for(unsigned int i = 0; i < num_pubtypes; i++)
	{
		delete dw_pubtypes[i];
	}

	typename std::map<uint64_t, DWARF_LocListEntry<MEMORY_ADDR> *>::iterator dw_loc_list_entry_iter;
	for(dw_loc_list_entry_iter = dw_loc_list.begin(); dw_loc_list_entry_iter != dw_loc_list.end(); dw_loc_list_entry_iter++)
	{
		if((*dw_loc_list_entry_iter).second)
		{
			delete (*dw_loc_list_entry_iter).second;
		}
	}

	if(debug_line_section) debug_line_section->Release();
	if(debug_info_section) debug_info_section->Release();
	if(debug_abbrev_section) debug_abbrev_section->Release();
	if(debug_aranges_section) debug_aranges_section->Release();
	if(debug_pubnames_section) debug_pubnames_section->Release();
	if(debug_pubtypes_section) debug_pubtypes_section->Release();
	if(debug_macinfo_section) debug_macinfo_section->Release();
	if(debug_frame_section) debug_frame_section->Release();
	if(debug_str_section) debug_str_section->Release();
	if(debug_loc_section) debug_loc_section->Release();
	if(debug_ranges_section) debug_ranges_section->Release();
	
	if(blob) blob->Release();
}

template <class MEMORY_ADDR>
const DWARF_Abbrev *DWARF_Handler<MEMORY_ADDR>::FindAbbrev(uint64_t debug_abbrev_offset, const DWARF_LEB128& dw_abbrev_code) const
{
	std::map<uint64_t, DWARF_Abbrev *>::const_iterator abbrev_iter;

	for(abbrev_iter = dw_abbrevs.find(debug_abbrev_offset); abbrev_iter != dw_abbrevs.end(); abbrev_iter++)
	{
		const DWARF_Abbrev *dw_abbrev = (*abbrev_iter).second;
		
		if(dw_abbrev->GetAbbrevCode() == dw_abbrev_code)
		{
			return dw_abbrev;
		}
	}
	return 0;
}

template <class MEMORY_ADDR>
const char *DWARF_Handler<MEMORY_ADDR>::GetString(uint64_t debug_str_offset) const
{
	return (debug_str_section && (debug_str_offset < debug_str_section->GetSize())) ? (const char *) debug_str_section->GetData() + debug_str_offset : 0;
}

template <class MEMORY_ADDR>
void DWARF_Handler<MEMORY_ADDR>::Parse()
{
	unsigned int i;
	
	if(debug_abbrev_section)
	{
		uint64_t debug_abbrev_offset = 0;
		do
		{
			DWARF_Abbrev *dw_abbrev = new DWARF_Abbrev(endianness);
			int64_t sz;
			if((sz = dw_abbrev->Load((const uint8_t *) debug_abbrev_section->GetData() + debug_abbrev_offset, debug_abbrev_section->GetSize() - debug_abbrev_offset, debug_abbrev_offset)) < 0)
			{
				logger << DebugWarning << "In File \"" << blob->GetFilename() << "\", invalid DWARF v2/v3 abbreviation informations at offset 0x" << std::hex << debug_abbrev_offset << std::dec << EndDebugWarning;
				delete dw_abbrev;
				break;
			}
			else
			{
				if(dw_abbrev->IsNull()) // Null entries are for padding to align section on some architectures
				{
					delete dw_abbrev;
				}
				else
				{
					//std::cerr << *dw_abbrev << std::endl;
					dw_abbrevs.insert(std::pair<uint64_t, DWARF_Abbrev *>(debug_abbrev_offset, dw_abbrev));
				}
				debug_abbrev_offset += sz;
			}
		}
		while(debug_abbrev_offset < debug_abbrev_section->GetSize());
	}
	else
	{
		logger << DebugWarning << "In File \"" << blob->GetFilename() << "\", no DWARF v2/v3 .debug_abbrev section found" << EndDebugWarning;
	}

	if(debug_info_section)
	{
		uint64_t debug_info_offset = 0;
		do
		{
			DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu = new DWARF_CompilationUnit<MEMORY_ADDR>(this);
			int64_t sz;
			if((sz = dw_cu->Load((const uint8_t *) debug_info_section->GetData() + debug_info_offset, debug_info_section->GetSize() - debug_info_offset, debug_info_offset)) < 0)
			{
				logger << DebugWarning << "In File \"" << blob->GetFilename() << "\", invalid DWARF v2/v3 debug informations at offset 0x" << std::hex << debug_info_offset << std::dec << EndDebugWarning;
				delete dw_cu;
				break;
			}
			else
			{
				//std::cerr << *dw_cu << std::endl;
				dw_cus.insert(std::pair<uint64_t, DWARF_CompilationUnit<MEMORY_ADDR> *>(dw_cu->GetOffset(), dw_cu));
				debug_info_offset += sz;
			}
		}
		while(debug_info_offset < debug_info_section->GetSize());
	}
	else
	{
		logger << DebugWarning << "In File \"" << blob->GetFilename() << "\", no DWARF v2/v3 .debug_info section found" << EndDebugWarning;
	}

	if(debug_frame_section)
	{
		uint64_t debug_frame_offset = 0;
		int64_t sz;
		do
		{
			DWARF_FDE<MEMORY_ADDR> *dw_fde = new DWARF_FDE<MEMORY_ADDR>(this);
			
			if((sz = dw_fde->Load((const uint8_t *) debug_frame_section->GetData() + debug_frame_offset, debug_frame_section->GetSize() - debug_frame_offset, debug_frame_offset)) < 0)
			{
				delete dw_fde;
				DWARF_CIE<MEMORY_ADDR> *dw_cie = new DWARF_CIE<MEMORY_ADDR>(this);
			
				if((sz = dw_cie->Load((const uint8_t *) debug_frame_section->GetData() + debug_frame_offset, debug_frame_section->GetSize() - debug_frame_offset, debug_frame_offset)) < 0)
				{
					delete dw_cie;
					
					logger << DebugWarning << "In File \"" << blob->GetFilename() << "\", invalid DWARF v2/v3 debug frame at offset 0x" << std::hex << debug_frame_offset << std::dec << EndDebugWarning;
					break;
				}
				
				//std::cerr << *dw_cie << std::endl;
				dw_cies.insert(std::pair<uint64_t, DWARF_CIE<MEMORY_ADDR> *>(debug_frame_offset, dw_cie));
			}
			else
			{
				//std::cerr << *dw_fde << std::endl;
				dw_fdes.push_back(dw_fde);
			}
			
			debug_frame_offset += sz;
		}
		while(debug_frame_offset < debug_frame_section->GetSize());
	}
	else
	{
		logger << DebugWarning << "In File \"" << blob->GetFilename() << "\", no DWARF v2/v3 .debug_frame section found" << EndDebugWarning;
	}

	if(debug_aranges_section)
	{
		uint64_t debug_aranges_offset = 0;
		do
		{
			DWARF_AddressRanges<MEMORY_ADDR> *dw_address_ranges = new DWARF_AddressRanges<MEMORY_ADDR>(this);
			int64_t sz;
			if((sz = dw_address_ranges->Load((const uint8_t *) debug_aranges_section->GetData() + debug_aranges_offset, debug_aranges_section->GetSize() - debug_aranges_offset)) < 0)
			{
				logger << DebugWarning << "In File \"" << blob->GetFilename() << "\", invalid DWARF v2/v3 debug aranges at offset 0x" << std::hex << debug_aranges_offset << std::dec << EndDebugWarning;
				delete dw_address_ranges;
				break;
			}
			else
			{
				//std::cerr << *dw_address_ranges << std::endl;
				dw_aranges.push_back(dw_address_ranges);
				debug_aranges_offset += sz;
			}
		}
		while(debug_aranges_offset < debug_aranges_section->GetSize());
	}
	else
	{
		logger << DebugWarning << "In File \"" << blob->GetFilename() << "\", no DWARF v2/v3 .debug_aranges section found" << EndDebugWarning;
	}

	if(debug_pubnames_section)
	{
		uint64_t debug_pubnames_offset = 0;
		do
		{
			DWARF_Pubs<MEMORY_ADDR> *dw_public_names = new DWARF_Pubs<MEMORY_ADDR>(this);
			int64_t sz;
			if((sz = dw_public_names->Load((const uint8_t *) debug_pubnames_section->GetData() + debug_pubnames_offset, debug_pubnames_section->GetSize() - debug_pubnames_offset)) < 0)
			{
				logger << DebugWarning << "In File \"" << blob->GetFilename() << "\", invalid DWARF v2/v3 debug pubnames at offset 0x" << std::hex << debug_pubnames_offset << std::dec << EndDebugWarning;
				delete dw_public_names;
				break;
			}
			else
			{
				//std::cerr << *dw_public_names << std::endl;
				dw_pubnames.push_back(dw_public_names);
				debug_pubnames_offset += sz;
			}
		}
		while(debug_pubnames_offset < debug_pubnames_section->GetSize());
	}
	else
	{
		logger << DebugWarning << "In File \"" << blob->GetFilename() << "\", no DWARF v2/v3 .debug_pubnames section found" << EndDebugWarning;
	}

	if(debug_pubtypes_section)
	{
		uint64_t debug_pubtypes_offset = 0;
		do
		{
			DWARF_Pubs<MEMORY_ADDR> *dw_public_types = new DWARF_Pubs<MEMORY_ADDR>(this);
			int64_t sz;
			if((sz = dw_public_types->Load((const uint8_t *) debug_pubtypes_section->GetData() + debug_pubtypes_offset, debug_pubtypes_section->GetSize() - debug_pubtypes_offset)) < 0)
			{
				logger << DebugWarning << "In File \"" << blob->GetFilename() << "\", invalid DWARF v2/v3 debug pubtypes at offset 0x" << std::hex << debug_pubtypes_offset << std::dec << EndDebugWarning;
				delete dw_public_types;
				break;
			}
			else
			{
				//std::cerr << *dw_public_types << std::endl;
				dw_pubtypes.push_back(dw_public_types);
				debug_pubtypes_offset += sz;
			}
		}
		while(debug_pubtypes_offset < debug_pubtypes_section->GetSize());
	}

	// Fix all pointer cross-references
	typename std::map<uint64_t, DWARF_CompilationUnit<MEMORY_ADDR> *>::const_iterator dw_cu_iter;
	
	unsigned int dw_cu_id = 0;
	for(dw_cu_iter = dw_cus.begin(); dw_cu_iter != dw_cus.end(); dw_cu_iter++, dw_cu_id++)
	{
		DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu = (*dw_cu_iter).second;
		dw_cu->Fix(this, dw_cu_id);
	}

	typename std::map<uint64_t, DWARF_DIE<MEMORY_ADDR> *>::const_iterator dw_die_iter;
	
	unsigned int dw_die_id = 0;
	for(dw_die_iter = dw_dies.begin(); dw_die_iter != dw_dies.end(); dw_die_iter++, dw_die_id++)
	{
		DWARF_DIE<MEMORY_ADDR> *dw_die = (*dw_die_iter).second;
		dw_die->Fix(this, dw_die_id);
	}

	typename std::map<uint64_t, DWARF_StatementProgram<MEMORY_ADDR> *>::const_iterator dw_stmt_prog_iter;
	
	unsigned int dw_stmt_prog_id = 0;
	for(dw_stmt_prog_iter = dw_stmt_progs.begin(); dw_stmt_prog_iter != dw_stmt_progs.end(); dw_stmt_prog_iter++, dw_stmt_prog_id++)
	{
		DWARF_StatementProgram<MEMORY_ADDR> *dw_stmt_prog = (*dw_stmt_prog_iter).second;
		dw_stmt_prog->Fix(this, dw_stmt_prog_id);
	}

	typename std::map<uint64_t, DWARF_CIE<MEMORY_ADDR> *>::const_iterator dw_cie_iter;
	
	unsigned int dw_cie_id = 0;
	for(dw_cie_iter = dw_cies.begin(); dw_cie_iter != dw_cies.end(); dw_cie_iter++, dw_cie_id++)
	{
		DWARF_CIE<MEMORY_ADDR> *dw_cie = (*dw_cie_iter).second;
		dw_cie->Fix(this, dw_cie_id);
	}

	unsigned int num_fdes = dw_fdes.size();
	for(i = 0; i < num_fdes; i++)
	{
		dw_fdes[i]->Fix(this);
	}

	unsigned int num_aranges = dw_aranges.size();
	for(i = 0; i < num_aranges; i++)
	{
		dw_aranges[i]->Fix(this);
	}

	unsigned int num_pubnames = dw_pubnames.size();
	for(i = 0; i < num_pubnames; i++)
	{
		dw_pubnames[i]->Fix(this);
	}

	unsigned int num_pubtypes = dw_pubtypes.size();
	for(i = 0; i < num_pubtypes; i++)
	{
		dw_pubtypes[i]->Fix(this);
	}
	
	typename std::map<uint64_t, DWARF_RangeListEntry<MEMORY_ADDR> *>::iterator dw_range_list_entry_iter;
	unsigned int dw_range_list_entry_id = 0;
	DWARF_RangeListEntry<MEMORY_ADDR> *dw_prev_range_list_entry = 0;
	for(dw_range_list_entry_iter = dw_range_list.begin(); dw_range_list_entry_iter != dw_range_list.end(); dw_range_list_entry_iter++, dw_range_list_entry_id++)
	{
		DWARF_RangeListEntry<MEMORY_ADDR> *dw_range_list_entry = (*dw_range_list_entry_iter).second;
		
		if(dw_prev_range_list_entry)
		{
			dw_prev_range_list_entry->next = dw_range_list_entry;
		}
		
		dw_range_list_entry->Fix(this, dw_range_list_entry_id);
		
		dw_prev_range_list_entry = dw_range_list_entry;
	}

	typename std::map<uint64_t, DWARF_MacInfoListEntry<MEMORY_ADDR> *>::iterator dw_macinfo_list_entry_iter;
	unsigned int dw_macinfo_list_entry_id = 0;
	DWARF_MacInfoListEntry<MEMORY_ADDR> *dw_prev_macinfo_list_entry = 0;
	for(dw_macinfo_list_entry_iter = dw_macinfo_list.begin(); dw_macinfo_list_entry_iter != dw_macinfo_list.end(); dw_macinfo_list_entry_iter++, dw_macinfo_list_entry_id++)
	{
		DWARF_MacInfoListEntry<MEMORY_ADDR> *dw_macinfo_list_entry = (*dw_macinfo_list_entry_iter).second;
		
		if(dw_prev_macinfo_list_entry)
		{
			dw_prev_macinfo_list_entry->next = dw_macinfo_list_entry;
		}
		
		dw_macinfo_list_entry->Fix(this, dw_macinfo_list_entry_id);
		
		dw_prev_macinfo_list_entry = dw_macinfo_list_entry;
	}

	typename std::map<uint64_t, DWARF_LocListEntry<MEMORY_ADDR> *>::iterator dw_loc_list_entry_iter;
	unsigned int dw_loc_list_entry_id = 0;
	DWARF_LocListEntry<MEMORY_ADDR> *dw_prev_loc_list_entry = 0;
	for(dw_loc_list_entry_iter = dw_loc_list.begin(); dw_loc_list_entry_iter != dw_loc_list.end(); dw_loc_list_entry_iter++, dw_loc_list_entry_id++)
	{
		DWARF_LocListEntry<MEMORY_ADDR> *dw_loc_list_entry = (*dw_loc_list_entry_iter).second;
		
		if(dw_prev_loc_list_entry)
		{
			dw_prev_loc_list_entry->next = dw_loc_list_entry;
		}
		
		dw_loc_list_entry->Fix(this, dw_loc_list_entry_id);

		dw_prev_loc_list_entry = dw_loc_list_entry;
	}
	
	BuildStatementMatrix();
	//std::ofstream f("out.xml", std::ios::out);
	//to_XML(f);
}

template <class MEMORY_ADDR>
void DWARF_Handler<MEMORY_ADDR>::to_XML(std::ostream& os)
{
	unsigned int i;
	
	os << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
	os << "<DWARF>" << std::endl;
	os << "<DW_DEBUG_PUBNAMES>" << std::endl;
	unsigned int num_pubnames = dw_pubnames.size();
	for(i = 0; i < num_pubnames; i++)
	{
		dw_pubnames[i]->to_XML(os) << std::endl;
	}
	os << "</DW_DEBUG_PUBNAMES>" << std::endl;
	os << "<DW_DEBUG_PUBTYPES>" << std::endl;
	unsigned int num_pubtypes = dw_pubtypes.size();
	for(i = 0; i < num_pubtypes; i++)
	{
		dw_pubtypes[i]->to_XML(os) << std::endl;
	}
	os << "</DW_DEBUG_PUBTYPESS>" << std::endl;
	os << "<DW_DEBUG_ARANGES>" << std::endl;
	unsigned int num_aranges = dw_aranges.size();
	for(i = 0; i < num_aranges; i++)
	{
		dw_aranges[i]->to_XML(os) << std::endl;
	}
	os << "</DW_DEBUG_ARANGESS>" << std::endl;
	os << "<DW_DEBUG_ABBREV>" << std::endl;
	typename std::map<uint64_t, DWARF_Abbrev *>::iterator dw_abbrev_iter;
	for(dw_abbrev_iter = dw_abbrevs.begin(); dw_abbrev_iter != dw_abbrevs.end(); dw_abbrev_iter++)
	{
		DWARF_Abbrev *dw_abbrev = (*dw_abbrev_iter).second;
		
		dw_abbrev->to_XML(os) << std::endl;
	}
	os << "</DW_DEBUG_ABBREV>" << std::endl;
	os << "<DW_DEBUG_INFO>" << std::endl;
	typename std::map<uint64_t, DWARF_CompilationUnit<MEMORY_ADDR> *>::iterator dw_cu_iter;
	for(dw_cu_iter = dw_cus.begin(); dw_cu_iter != dw_cus.end(); dw_cu_iter++)
	{
		DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu = (*dw_cu_iter).second;
		dw_cu->to_XML(os) << std::endl;
	}
	os << "</DW_DEBUG_INFO>" << std::endl;
	os << "<DW_DEBUG_MACINFO>" << std::endl;
	typename std::map<uint64_t, DWARF_MacInfoListEntry<MEMORY_ADDR> *>::iterator dw_macinfo_list_entry_iter;
	for(dw_macinfo_list_entry_iter = dw_macinfo_list.begin(); dw_macinfo_list_entry_iter != dw_macinfo_list.end(); dw_macinfo_list_entry_iter++)
	{
		DWARF_MacInfoListEntry<MEMORY_ADDR> *dw_macinfo_list_entry = (*dw_macinfo_list_entry_iter).second;
		dw_macinfo_list_entry->to_XML(os) << std::endl;
	}
	os << "</DW_DEBUG_MACINFO>" << std::endl;
	os << "<DW_DEBUG_RANGES>" << std::endl;
	typename std::map<uint64_t, DWARF_RangeListEntry<MEMORY_ADDR> *>::iterator dw_range_list_entry_iter;
	for(dw_range_list_entry_iter = dw_range_list.begin(); dw_range_list_entry_iter != dw_range_list.end(); dw_range_list_entry_iter++)
	{
		DWARF_RangeListEntry<MEMORY_ADDR> *dw_range_list_entry = (*dw_range_list_entry_iter).second;
		dw_range_list_entry->to_XML(os) << std::endl;
	}
	os << "</DW_DEBUG_RANGES>" << std::endl;
	os << "<DW_DEBUG_LOC>" << std::endl;
	typename std::map<uint64_t, DWARF_LocListEntry<MEMORY_ADDR> *>::iterator dw_loc_list_entry_iter;
	for(dw_loc_list_entry_iter = dw_loc_list.begin(); dw_loc_list_entry_iter != dw_loc_list.end(); dw_loc_list_entry_iter++)
	{
		DWARF_LocListEntry<MEMORY_ADDR> *dw_loc_list_entry = (*dw_loc_list_entry_iter).second;
		dw_loc_list_entry->to_XML(os) << std::endl;
	}
	os << "</DW_DEBUG_LOC>" << std::endl;
	os << "<DW_DEBUG_FRAME>" << std::endl;
	typename std::map<uint64_t, DWARF_CIE<MEMORY_ADDR> *>::iterator dw_cie_iter;
	for(dw_cie_iter = dw_cies.begin(); dw_cie_iter != dw_cies.end(); dw_cie_iter++)
	{
		DWARF_CIE<MEMORY_ADDR> *dw_cie = (*dw_cie_iter).second;
		
		dw_cie->to_XML(os) << std::endl;
	}

	unsigned int num_fdes = dw_fdes.size();
	for(i = 0; i < num_fdes; i++)
	{
		dw_fdes[i]->to_XML(os) << std::endl;
	}
	os << "</DW_DEBUG_FRAME>" << std::endl;
	os << "<DW_DEBUG_LINE>" << std::endl;
	typename std::map<uint64_t, DWARF_StatementProgram<MEMORY_ADDR> *>::iterator dw_stmt_prog_iter;
	for(dw_stmt_prog_iter = dw_stmt_progs.begin(); dw_stmt_prog_iter != dw_stmt_progs.end(); dw_stmt_prog_iter++)
	{
		DWARF_StatementProgram<MEMORY_ADDR> *dw_stmt_prog = (*dw_stmt_prog_iter).second;
		
		dw_stmt_prog->to_XML(os) << std::endl;
	}
	os << "</DW_DEBUG_LINE>" << std::endl;
	os << "</DWARF>" << std::endl;
}

template <class MEMORY_ADDR>
void DWARF_Handler<MEMORY_ADDR>::to_HTML(const char *output_dir)
{
#if defined(WIN32)
	mkdir(output_dir);
#else
	mkdir(output_dir, S_IRWXU);
#endif
	std::string index_filename(std::string(output_dir) + "/index.html");
	std::ofstream index(index_filename.c_str(), std::ios::out);
	
	index << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">" << std::endl;
	index << "<html>" << std::endl;
	index << "<head>" << std::endl;
	index << "<meta name=\"description\" content=\"DWARF v2/v3 navigation\">" << std::endl;
	index << "<meta name=\"keywords\" content=\"DWARF\">" << std::endl;
	index << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
	index << "<title>DWARF v2/v3 navigation</title>" << std::endl;
	index << "<style type=\"text/css\">" << std::endl;
	index << "<!--" << std::endl;
	index << "table th { text-align:center; }" << std::endl;
	index << "table th { font-weight:bold; }" << std::endl;
	index << "table td { text-align:left; }" << std::endl;
	index << "table { border-style:solid; }" << std::endl;
	index << "table { border-width:1px; }" << std::endl;
	index << "th, td { border-style:solid; }" << std::endl;
	index << "th, td { border-width:1px; }" << std::endl;
	index << "th, td { border-width:1px; }" << std::endl;
	index << "-->" << std::endl;
	index << "</style>" << std::endl;
	index << "</head>" << std::endl;
	index << "<body>" << std::endl;
	index << "<h1><a href=\"http://dwarfstd.org\"/>DWARF</a> v2/v3 navigation</h1>" << std::endl;
	index << "<table>" << std::endl;
	index << "<tr><th>Section</th><th>Description</th></tr>" << std::endl;
	index << "<tr>" << std::endl;
	index << "<td><a href=\"debug_abbrev/0.html\">.debug_abbrev</a></td>" << std::endl;
	index << "<td>Abbreviations Tables</td>" << std::endl;
	index << "</tr>" << std::endl;
	index << "<tr>" << std::endl;
	index << "<td><a href=\"debug_info/cus/0.html\">.debug_info</a></td>" << std::endl;
	index << "<td>Debugging Information</td>" << std::endl;
	index << "</tr>" << std::endl;
	index << "<tr>" << std::endl;
	index << "<td><a href=\"debug_pubnames/0.html\">.debug_pubnames</a>&nbsp;<a href=\"debug_pubtypes/0.html\">.debug_pubtypes</a></td>" << std::endl;
	index << "<td>Lookup by Name</td>" << std::endl;
	index << "</tr>" << std::endl;
	index << "<tr>" << std::endl;
	index << "<td><a href=\"debug_aranges/0.html\">.debug_aranges</a></td>" << std::endl;
	index << "<td>Lookup by Address</td>" << std::endl;
	index << "</tr>" << std::endl;
	index << "<tr>" << std::endl;
	index << "<td><a href=\"debug_ranges/0.html\">.debug_ranges</a></td>" << std::endl;
	index << "<td> Non-Contiguous Address Ranges</td>" << std::endl;
	index << "</tr>" << std::endl;
	index << "<tr>" << std::endl;
	index << "<td><a href=\"debug_frame/fdes/0.html\">.debug_frame</a></td>" << std::endl;
	index << "<td>Call Frame Information</td>" << std::endl;
	index << "</tr>" << std::endl;
	index << "<tr>" << std::endl;
	index << "<td><a href=\"debug_line/0.html\">.debug_line</a></td>" << std::endl;
	index << "<td>Line Number Information</td>" << std::endl;
	index << "</tr>" << std::endl;
	index << "<tr>" << std::endl;
	index << "<td><a href=\"debug_macinfo/0.html\">.debug_macinfo</a></td>" << std::endl;
	index << "<td>Macro Information</td>" << std::endl;
	index << "</tr>" << std::endl;
	index << "<tr>" << std::endl;
	index << "<td><a href=\"debug_loc/0.html\">.debug_loc</a></td>" << std::endl;
	index << "<td> Location Lists</td>" << std::endl;
	index << "</tr>" << std::endl;
	index << "</table>" << std::endl;
	index << "</body>" << std::endl;
	index << "</html>" << std::endl;

	// Abbreviations Table
	std::stringstream debug_abbrev_output_dir_sstr;
	debug_abbrev_output_dir_sstr << output_dir << "/debug_abbrev";
	std::string debug_abbrev_output_dir(debug_abbrev_output_dir_sstr.str());
#if defined(WIN32)
	mkdir(debug_abbrev_output_dir.c_str());
#else
	mkdir(debug_abbrev_output_dir.c_str(), S_IRWXU);
#endif
	unsigned int debug_abbrev_filename_idx = 0;
	unsigned int debug_abbrev_per_file = 512;
	unsigned int num_debug_abbrevs = dw_abbrevs.size();
	unsigned int num_debug_abbrev_filenames = ((num_debug_abbrevs + debug_abbrev_per_file - 1) / debug_abbrev_per_file);
	typename std::map<uint64_t, DWARF_Abbrev *>::iterator dw_abbrev_iter = dw_abbrevs.begin();
	do
	{
		std::stringstream debug_abbrev_filename_sstr;
		debug_abbrev_filename_sstr << debug_abbrev_output_dir << "/" << debug_abbrev_filename_idx << ".html";
		std::string debug_abbrev_filename = debug_abbrev_filename_sstr.str().c_str();
		std::ofstream debug_abbrev(debug_abbrev_filename.c_str(), std::ios::out);
		debug_abbrev << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">" << std::endl;
		debug_abbrev << "<html>" << std::endl;
		debug_abbrev << "<head>" << std::endl;
		debug_abbrev << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		debug_abbrev << "<style type=\"text/css\">" << std::endl;
		debug_abbrev << "table th { text-align:center; }" << std::endl;
		debug_abbrev << "table th { font-weight:bold; }" << std::endl;
		debug_abbrev << "table td { text-align:left; }" << std::endl;
		debug_abbrev << "table { border-style:solid; }" << std::endl;
		debug_abbrev << "table { border-width:1px; }" << std::endl;
		debug_abbrev << "th, td { border-style:solid; }" << std::endl;
		debug_abbrev << "th, td { border-width:1px; }" << std::endl;
		debug_abbrev << "th, td { border-width:1px; }" << std::endl;
		debug_abbrev << "table.abbrev_attr { border-width:0px; }" << std::endl;
		debug_abbrev << "table.abbrev_attr th { border-width:0px; }" << std::endl;
		debug_abbrev << "table.abbrev_attr td { border-width:0px; }" << std::endl;
		debug_abbrev << "</style>" << std::endl;
		debug_abbrev << "</head>" << std::endl;
		debug_abbrev << "<body>" << std::endl;
		debug_abbrev << "<h1>Abbreviations Tables (.debug_abbrev)</h1>" << std::endl;
		debug_abbrev << "<a href=\"../index.html\">Index</a>&nbsp;" << std::endl;
		unsigned int i;
		for(i = 0; i < num_debug_abbrev_filenames; i++)
		{
			if(i != 0) debug_abbrev << "&nbsp;";
			if(i == debug_abbrev_filename_idx)
			{
				debug_abbrev << i << std::endl;
			}
			else
			{
				std::stringstream sstr;
				sstr << i << ".html";
				debug_abbrev << "<a href=\"" << sstr.str() << "\">" << i << "</a>" << std::endl;
			}
		}
		debug_abbrev << "<table>" << std::endl;
		debug_abbrev << "<tr>" << std::endl;
		debug_abbrev << "<th>Offset</th>" << std::endl;
		debug_abbrev << "<th>Code</th>" << std::endl;
		debug_abbrev << "<th>Tag</th>" << std::endl;
		debug_abbrev << "<th>Children</th>" << std::endl;
		debug_abbrev << "<th>Attributes</th>" << std::endl;
		debug_abbrev << "</tr>" << std::endl;
		unsigned int count = 0;
		while(count < debug_abbrev_per_file && dw_abbrev_iter != dw_abbrevs.end())
		{
			DWARF_Abbrev *dw_abbrev = (*dw_abbrev_iter).second;
			dw_abbrev->to_HTML(debug_abbrev) << std::endl;
			
			dw_abbrev_iter++;
			count++;
		}
		debug_abbrev << "</table>" << std::endl;
		debug_abbrev << "</body>" << std::endl;
		debug_abbrev << "</html>" << std::endl;
		debug_abbrev_filename_idx++;
	}
	while(dw_abbrev_iter != dw_abbrevs.end());
	
	// Macro Information
	std::stringstream debug_macinfo_ouput_dir_sstr;
	debug_macinfo_ouput_dir_sstr << output_dir << "/debug_macinfo";
	std::string debug_macinfo_output_dir(debug_macinfo_ouput_dir_sstr.str());
#if defined(WIN32)
	mkdir(debug_macinfo_output_dir.c_str());
#else
	mkdir(debug_macinfo_output_dir.c_str(), S_IRWXU);
#endif
	
	unsigned int debug_macinfo_filename_idx = 0;
	unsigned int debug_macinfo_per_file = 2048;
	unsigned int num_debug_macinfos = dw_macinfo_list.size();
	unsigned int num_debug_macinfo_filenames = ((num_debug_macinfos + debug_macinfo_per_file - 1) / debug_macinfo_per_file);
	typename std::map<uint64_t, DWARF_MacInfoListEntry<MEMORY_ADDR> *>::iterator dw_macinfo_list_iter = dw_macinfo_list.begin();
	do
	{
		std::stringstream debug_macinfo_filename_sstr;
		debug_macinfo_filename_sstr << debug_macinfo_output_dir << "/" << debug_macinfo_filename_idx << ".html";
		std::string debug_macinfo_filename = debug_macinfo_filename_sstr.str().c_str();
		std::ofstream debug_macinfo(debug_macinfo_filename.c_str(), std::ios::out);
		debug_macinfo << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">" << std::endl;
		debug_macinfo << "<html>" << std::endl;
		debug_macinfo << "<head>" << std::endl;
		debug_macinfo << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		debug_macinfo << "<style type=\"text/css\">" << std::endl;
		debug_macinfo << "<!--" << std::endl;
		debug_macinfo << "table th { text-align:center; }" << std::endl;
		debug_macinfo << "table th { font-weight:bold; }" << std::endl;
		debug_macinfo << "table td { text-align:left; }" << std::endl;
		debug_macinfo << "table { border-style:solid; }" << std::endl;
		debug_macinfo << "table { border-width:1px; }" << std::endl;
		debug_macinfo << "th, td { border-style:solid; }" << std::endl;
		debug_macinfo << "th, td { border-width:1px; }" << std::endl;
		debug_macinfo << "th, td { border-width:1px; }" << std::endl;
		debug_macinfo << "table.macinfo { border-width:0px; }" << std::endl;
		debug_macinfo << "table.macinfo th { border-width:0px; }" << std::endl;
		debug_macinfo << "table.macinfo td { border-width:0px; }" << std::endl;
		debug_macinfo << "-->" << std::endl;
		debug_macinfo << "</style>" << std::endl;
		debug_macinfo << "</head>" << std::endl;
		debug_macinfo << "<body>" << std::endl;
		debug_macinfo << "<h1>Macro Information (.debug_macinfo)</h1>" << std::endl;
		debug_macinfo << "<a href=\"../index.html\">Index</a>&nbsp;" << std::endl;
		unsigned int i;
		for(i = 0; i < num_debug_macinfo_filenames; i++)
		{
			if(i != 0) debug_macinfo << "&nbsp;";
			if(i == debug_macinfo_filename_idx)
			{
				debug_macinfo << i << std::endl;
			}
			else
			{
				std::stringstream sstr;
				sstr << i << ".html";
				debug_macinfo << "<a href=\"" << sstr.str() << "\">" << i << "</a>" << std::endl;
			}
		}
		debug_macinfo << "<table>" << std::endl;
		debug_macinfo << "<tr>" << std::endl;
		debug_macinfo << "<th>Entry type</th>" << std::endl;
		debug_macinfo << "<th>Id</th>" << std::endl;
		debug_macinfo << "<th>Offset</th>" << std::endl;
		debug_macinfo << "<th>Next</th>" << std::endl;
		debug_macinfo << "<th>Entry Information</th>" << std::endl;
		debug_macinfo << "</tr>" << std::endl;
		unsigned int count = 0;
		while(count < debug_macinfo_per_file && dw_macinfo_list_iter != dw_macinfo_list.end())
		{
			DWARF_MacInfoListEntry<MEMORY_ADDR> *dw_macinfo = (*dw_macinfo_list_iter).second;

			dw_macinfo->to_HTML(debug_macinfo) << std::endl;
			
			dw_macinfo_list_iter++;
			count++;
		}
		debug_macinfo << "</table>" << std::endl;
		debug_macinfo << "</body>" << std::endl;
		debug_macinfo << "</html>" << std::endl;
		debug_macinfo_filename_idx++;
	}
	while(dw_macinfo_list_iter != dw_macinfo_list.end());

	
	// Line Number Information
	std::stringstream debug_line_ouput_dir_sstr;
	debug_line_ouput_dir_sstr << output_dir << "/debug_line";
	std::string debug_line_output_dir(debug_line_ouput_dir_sstr.str());
#if defined(WIN32)
	mkdir(debug_line_output_dir.c_str());
#else
	mkdir(debug_line_output_dir.c_str(), S_IRWXU);
#endif

	
	unsigned int debug_line_filename_idx = 0;
	unsigned int num_debug_line = dw_stmt_progs.size();
	unsigned int num_debug_line_filenames = ((num_debug_line + debug_line_per_file - 1) / debug_line_per_file);
	typename std::map<uint64_t, DWARF_StatementProgram<MEMORY_ADDR> *>::iterator dw_stmt_prog_iter = dw_stmt_progs.begin();
	do
	{
		std::stringstream debug_line_filename_sstr;
		debug_line_filename_sstr << debug_line_output_dir << "/" << debug_line_filename_idx << ".html";
		std::string debug_line_filename = debug_line_filename_sstr.str().c_str();
		std::ofstream debug_line(debug_line_filename.c_str(), std::ios::out);
		debug_line << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">" << std::endl;
		debug_line << "<html>" << std::endl;
		debug_line << "<head>" << std::endl;
		debug_line << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		debug_line << "<style type=\"text/css\">" << std::endl;
		debug_line << "<!--" << std::endl;
		debug_line << "table th { text-align:center; }" << std::endl;
		debug_line << "table th { font-weight:bold; }" << std::endl;
		debug_line << "table td { text-align:left; }" << std::endl;
		debug_line << "table { border-style:solid; }" << std::endl;
		debug_line << "table { border-width:1px; }" << std::endl;
		debug_line << "th, td { border-style:solid; }" << std::endl;
		debug_line << "th, td { border-width:1px; }" << std::endl;
		debug_line << "th, td { border-width:1px; }" << std::endl;
		debug_line << "table.range { border-width:0px; }" << std::endl;
		debug_line << "table.range th { border-width:0px; }" << std::endl;
		debug_line << "table.range td { border-width:0px; }" << std::endl;
		debug_line << "-->" << std::endl;
		debug_line << "</style>" << std::endl;
		debug_line << "</head>" << std::endl;
		debug_line << "<body>" << std::endl;
		debug_line << "<h1>Line number information (.debug_line)</h1>" << std::endl;
		debug_line << "<a href=\"../index.html\">Index</a>&nbsp;" << std::endl;
		unsigned int i;
		for(i = 0; i < num_debug_line_filenames; i++)
		{
			if(i != 0) debug_line << "&nbsp;";
			if(i == debug_line_filename_idx)
			{
				debug_line << i << std::endl;
			}
			else
			{
				std::stringstream sstr;
				sstr << i << ".html";
				debug_line << "<a href=\"" << sstr.str() << "\">" << i << "</a>" << std::endl;
			}
		}
		debug_line << "<table>" << std::endl;
		debug_line << "<tr>" << std::endl;
		debug_line << "<th>Statement Program</th>" << std::endl;
		debug_line << "</tr>" << std::endl;
		unsigned int count = 0;
		while(count < debug_line_per_file && dw_stmt_prog_iter != dw_stmt_progs.end())
		{
			DWARF_StatementProgram<MEMORY_ADDR> *dw_stmt_prog = (*dw_stmt_prog_iter).second;

			dw_stmt_prog->to_HTML(debug_line) << std::endl;
			
			dw_stmt_prog_iter++;
			count++;
		}
		debug_line << "</table>" << std::endl;
		debug_line << "</body>" << std::endl;
		debug_line << "</html>" << std::endl;
		debug_line_filename_idx++;
	}
	while(dw_stmt_prog_iter != dw_stmt_progs.end());

	// Lookup by Address
	std::stringstream debug_aranges_ouput_dir_sstr;
	debug_aranges_ouput_dir_sstr << output_dir << "/debug_aranges";
	std::string debug_aranges_output_dir(debug_aranges_ouput_dir_sstr.str());
#if defined(WIN32)
	mkdir(debug_aranges_output_dir.c_str());
#else
	mkdir(debug_aranges_output_dir.c_str(), S_IRWXU);
#endif

	
	unsigned int debug_aranges_filename_idx = 0;
	unsigned int debug_aranges_per_file = 2048;
	unsigned int num_debug_aranges = dw_aranges.size();
	unsigned int num_debug_aranges_filenames = ((num_debug_aranges + debug_aranges_per_file - 1) / debug_aranges_per_file);
	typename std::vector<DWARF_AddressRanges<MEMORY_ADDR> *>::iterator dw_aranges_iter = dw_aranges.begin();
	do
	{
		std::stringstream debug_aranges_filename_sstr;
		debug_aranges_filename_sstr << debug_aranges_output_dir << "/" << debug_aranges_filename_idx << ".html";
		std::string debug_aranges_filename = debug_aranges_filename_sstr.str().c_str();
		std::ofstream debug_aranges(debug_aranges_filename.c_str(), std::ios::out);
		debug_aranges << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">" << std::endl;
		debug_aranges << "<html>" << std::endl;
		debug_aranges << "<head>" << std::endl;
		debug_aranges << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		debug_aranges << "<style type=\"text/css\">" << std::endl;
		debug_aranges << "<!--" << std::endl;
		debug_aranges << "table th { text-align:center; }" << std::endl;
		debug_aranges << "table th { font-weight:bold; }" << std::endl;
		debug_aranges << "table td { text-align:left; }" << std::endl;
		debug_aranges << "table { border-style:solid; }" << std::endl;
		debug_aranges << "table { border-width:1px; }" << std::endl;
		debug_aranges << "th, td { border-style:solid; }" << std::endl;
		debug_aranges << "th, td { border-width:1px; }" << std::endl;
		debug_aranges << "th, td { border-width:1px; }" << std::endl;
		debug_aranges << "table.range { border-width:0px; }" << std::endl;
		debug_aranges << "table.range th { border-width:0px; }" << std::endl;
		debug_aranges << "table.range td { border-width:0px; }" << std::endl;
		debug_aranges << "-->" << std::endl;
		debug_aranges << "</style>" << std::endl;
		debug_aranges << "</head>" << std::endl;
		debug_aranges << "<body>" << std::endl;
		debug_aranges << "<h1>Lookup by Address (.debug_aranges)</h1>" << std::endl;
		debug_aranges << "<a href=\"../index.html\">Index</a>&nbsp;" << std::endl;
		unsigned int i;
		for(i = 0; i < num_debug_aranges_filenames; i++)
		{
			if(i != 0) debug_aranges << "&nbsp;";
			if(i == debug_aranges_filename_idx)
			{
				debug_aranges << i << std::endl;
			}
			else
			{
				std::stringstream sstr;
				sstr << i << ".html";
				debug_aranges << "<a href=\"" << sstr.str() << "\">" << i << "</a>" << std::endl;
			}
		}
		debug_aranges << "<table>" << std::endl;
		debug_aranges << "<tr>" << std::endl;
		debug_aranges << "<th>Version</th>" << std::endl;
		debug_aranges << "<th>Compilation unit</th>" << std::endl;
		debug_aranges << "<th>Address size</th>" << std::endl;
		debug_aranges << "<th>Segment size</th>" << std::endl;
		debug_aranges << "<th>Address descriptors</th>" << std::endl;
		debug_aranges << "</tr>" << std::endl;
		unsigned int count = 0;
		while(count < debug_aranges_per_file && dw_aranges_iter != dw_aranges.end())
		{
			DWARF_AddressRanges<MEMORY_ADDR> *dw_arange = (*dw_aranges_iter);

			dw_arange->to_HTML(debug_aranges) << std::endl;
			
			dw_aranges_iter++;
			count++;
		}
		debug_aranges << "</table>" << std::endl;
		debug_aranges << "</body>" << std::endl;
		debug_aranges << "</html>" << std::endl;
		debug_aranges_filename_idx++;
	}
	while(dw_aranges_iter != dw_aranges.end());

	// Lookup by name (.debug_pubnames)
	std::stringstream debug_pubnames_ouput_dir_sstr;
	debug_pubnames_ouput_dir_sstr << output_dir << "/debug_pubnames";
	std::string debug_pubnames_output_dir(debug_pubnames_ouput_dir_sstr.str());
#if defined(WIN32)
	mkdir(debug_pubnames_output_dir.c_str());
#else
	mkdir(debug_pubnames_output_dir.c_str(), S_IRWXU);
#endif

	
	unsigned int debug_pubnames_filename_idx = 0;
	unsigned int debug_pubnames_per_file = 2048;
	unsigned int num_debug_pubnames = dw_pubnames.size();
	unsigned int num_debug_pubnames_filenames = ((num_debug_pubnames + debug_pubnames_per_file - 1) / debug_pubnames_per_file);
	typename std::vector<DWARF_Pubs<MEMORY_ADDR> *>::iterator dw_pubnames_iter = dw_pubnames.begin();
	do
	{
		std::stringstream debug_pubnames_filename_sstr;
		debug_pubnames_filename_sstr << debug_pubnames_output_dir << "/" << debug_pubnames_filename_idx << ".html";
		std::string debug_pubnames_filename = debug_pubnames_filename_sstr.str().c_str();
		std::ofstream debug_pubnames(debug_pubnames_filename.c_str(), std::ios::out);
		debug_pubnames << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">" << std::endl;
		debug_pubnames << "<html>" << std::endl;
		debug_pubnames << "<head>" << std::endl;
		debug_pubnames << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		debug_pubnames << "<style type=\"text/css\">" << std::endl;
		debug_pubnames << "<!--" << std::endl;
		debug_pubnames << "table th { text-align:center; }" << std::endl;
		debug_pubnames << "table th { font-weight:bold; }" << std::endl;
		debug_pubnames << "table td { text-align:left; }" << std::endl;
		debug_pubnames << "table { border-style:solid; }" << std::endl;
		debug_pubnames << "table { border-width:1px; }" << std::endl;
		debug_pubnames << "th, td { border-style:solid; }" << std::endl;
		debug_pubnames << "th, td { border-width:1px; }" << std::endl;
		debug_pubnames << "th, td { border-width:1px; }" << std::endl;
		debug_pubnames << "table.range { border-width:0px; }" << std::endl;
		debug_pubnames << "table.range th { border-width:0px; }" << std::endl;
		debug_pubnames << "table.range td { border-width:0px; }" << std::endl;
		debug_pubnames << "-->" << std::endl;
		debug_pubnames << "</style>" << std::endl;
		debug_pubnames << "</head>" << std::endl;
		debug_pubnames << "<body>" << std::endl;
		debug_pubnames << "<h1>Lookup by name (.debug_pubnames)</h1>" << std::endl;
		debug_pubnames << "<a href=\"../index.html\">Index</a>&nbsp;" << std::endl;
		unsigned int i;
		for(i = 0; i < num_debug_pubnames_filenames; i++)
		{
			if(i != 0) debug_pubnames << "&nbsp;";
			if(i == debug_pubnames_filename_idx)
			{
				debug_pubnames << i << std::endl;
			}
			else
			{
				std::stringstream sstr;
				sstr << i << ".html";
				debug_pubnames << "<a href=\"" << sstr.str() << "\">" << i << "</a>" << std::endl;
			}
		}
		debug_pubnames << "<table>" << std::endl;
		debug_pubnames << "<tr>" << std::endl;
		debug_pubnames << "<th>Version</th>" << std::endl;
		debug_pubnames << "<th>Compilation unit</th>" << std::endl;
		debug_pubnames << "<th>Public names</th>" << std::endl;
		debug_pubnames << "</tr>" << std::endl;
		unsigned int count = 0;
		while(count < debug_pubnames_per_file && dw_pubnames_iter != dw_pubnames.end())
		{
			DWARF_Pubs<MEMORY_ADDR> *dw_pubname = (*dw_pubnames_iter);

			dw_pubname->to_HTML(debug_pubnames) << std::endl;
			
			dw_pubnames_iter++;
			count++;
		}
		debug_pubnames << "</table>" << std::endl;
		debug_pubnames << "</body>" << std::endl;
		debug_pubnames << "</html>" << std::endl;
		debug_pubnames_filename_idx++;
	}
	while(dw_pubnames_iter != dw_pubnames.end());

	// Lookup by name (.debug_pubtypes)
	std::stringstream debug_pubtypes_ouput_dir_sstr;
	debug_pubtypes_ouput_dir_sstr << output_dir << "/debug_pubtypes";
	std::string debug_pubtypes_output_dir(debug_pubtypes_ouput_dir_sstr.str());
#if defined(WIN32)
	mkdir(debug_pubtypes_output_dir.c_str());
#else
	mkdir(debug_pubtypes_output_dir.c_str(), S_IRWXU);
#endif

	
	unsigned int debug_pubtypes_filename_idx = 0;
	unsigned int debug_pubtypes_per_file = 2048;
	unsigned int num_debug_pubtypes = dw_pubtypes.size();
	unsigned int num_debug_pubtypes_filenames = ((num_debug_pubtypes + debug_pubtypes_per_file - 1) / debug_pubtypes_per_file);
	typename std::vector<DWARF_Pubs<MEMORY_ADDR> *>::iterator dw_pubtypes_iter = dw_pubtypes.begin();
	do
	{
		std::stringstream debug_pubtypes_filename_sstr;
		debug_pubtypes_filename_sstr << debug_pubtypes_output_dir << "/" << debug_pubtypes_filename_idx << ".html";
		std::string debug_pubtypes_filename = debug_pubtypes_filename_sstr.str().c_str();
		std::ofstream debug_pubtypes(debug_pubtypes_filename.c_str(), std::ios::out);
		debug_pubtypes << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">" << std::endl;
		debug_pubtypes << "<html>" << std::endl;
		debug_pubtypes << "<head>" << std::endl;
		debug_pubtypes << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		debug_pubtypes << "<style type=\"text/css\">" << std::endl;
		debug_pubtypes << "<!--" << std::endl;
		debug_pubtypes << "table th { text-align:center; }" << std::endl;
		debug_pubtypes << "table th { font-weight:bold; }" << std::endl;
		debug_pubtypes << "table td { text-align:left; }" << std::endl;
		debug_pubtypes << "table { border-style:solid; }" << std::endl;
		debug_pubtypes << "table { border-width:1px; }" << std::endl;
		debug_pubtypes << "th, td { border-style:solid; }" << std::endl;
		debug_pubtypes << "th, td { border-width:1px; }" << std::endl;
		debug_pubtypes << "th, td { border-width:1px; }" << std::endl;
		debug_pubtypes << "table.range { border-width:0px; }" << std::endl;
		debug_pubtypes << "table.range th { border-width:0px; }" << std::endl;
		debug_pubtypes << "table.range td { border-width:0px; }" << std::endl;
		debug_pubtypes << "-->" << std::endl;
		debug_pubtypes << "</style>" << std::endl;
		debug_pubtypes << "</head>" << std::endl;
		debug_pubtypes << "<body>" << std::endl;
		debug_pubtypes << "<h1>Lookup by name (.debug_pubtypes)</h1>" << std::endl;
		debug_pubtypes << "<a href=\"../index.html\">Index</a>&nbsp;" << std::endl;
		unsigned int i;
		for(i = 0; i < num_debug_pubtypes_filenames; i++)
		{
			if(i != 0) debug_pubtypes << "&nbsp;";
			if(i == debug_pubtypes_filename_idx)
			{
				debug_pubtypes << i << std::endl;
			}
			else
			{
				std::stringstream sstr;
				sstr << i << ".html";
				debug_pubtypes << "<a href=\"" << sstr.str() << "\">" << i << "</a>" << std::endl;
			}
		}
		debug_pubtypes << "<table>" << std::endl;
		debug_pubtypes << "<tr>" << std::endl;
		debug_pubtypes << "<th>Version</th>" << std::endl;
		debug_pubtypes << "<th>Compilation unit</th>" << std::endl;
		debug_pubtypes << "<th>Public names</th>" << std::endl;
		debug_pubtypes << "</tr>" << std::endl;
		unsigned int count = 0;
		while(count < debug_pubtypes_per_file && dw_pubtypes_iter != dw_pubtypes.end())
		{
			DWARF_Pubs<MEMORY_ADDR> *dw_pubtype = (*dw_pubtypes_iter);

			dw_pubtype->to_HTML(debug_pubtypes) << std::endl;
			
			dw_pubtypes_iter++;
			count++;
		}
		debug_pubtypes << "</table>" << std::endl;
		debug_pubtypes << "</body>" << std::endl;
		debug_pubtypes << "</html>" << std::endl;
		debug_pubtypes_filename_idx++;
	}
	while(dw_pubtypes_iter != dw_pubtypes.end());

	// Non-Contiguous Address Ranges
	
	std::stringstream debug_range_ouput_dir_sstr;
	debug_range_ouput_dir_sstr << output_dir << "/debug_ranges";
	std::string debug_range_output_dir(debug_range_ouput_dir_sstr.str());
#if defined(WIN32)
	mkdir(debug_range_output_dir.c_str());
#else
	mkdir(debug_range_output_dir.c_str(), S_IRWXU);
#endif
	
	unsigned int debug_range_filename_idx = 0;
	unsigned int debug_range_per_file = 2048;
	unsigned int num_debug_ranges = dw_range_list.size();
	unsigned int num_debug_range_filenames = ((num_debug_ranges + debug_range_per_file - 1) / debug_range_per_file);
	typename std::map<uint64_t, DWARF_RangeListEntry<MEMORY_ADDR> *>::iterator dw_range_list_iter = dw_range_list.begin();
	do
	{
		std::stringstream debug_range_filename_sstr;
		debug_range_filename_sstr << debug_range_output_dir << "/" << debug_range_filename_idx << ".html";
		std::string debug_range_filename = debug_range_filename_sstr.str().c_str();
		std::ofstream debug_range(debug_range_filename.c_str(), std::ios::out);
		debug_range << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">" << std::endl;
		debug_range << "<html>" << std::endl;
		debug_range << "<head>" << std::endl;
		debug_range << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		debug_range << "<style type=\"text/css\">" << std::endl;
		debug_range << "<!--" << std::endl;
		debug_range << "table th { text-align:center; }" << std::endl;
		debug_range << "table th { font-weight:bold; }" << std::endl;
		debug_range << "table td { text-align:left; }" << std::endl;
		debug_range << "table { border-style:solid; }" << std::endl;
		debug_range << "table { border-width:1px; }" << std::endl;
		debug_range << "th, td { border-style:solid; }" << std::endl;
		debug_range << "th, td { border-width:1px; }" << std::endl;
		debug_range << "th, td { border-width:1px; }" << std::endl;
		debug_range << "table.range { border-width:0px; }" << std::endl;
		debug_range << "table.range th { border-width:0px; }" << std::endl;
		debug_range << "table.range td { border-width:0px; }" << std::endl;
		debug_range << "-->" << std::endl;
		debug_range << "</style>" << std::endl;
		debug_range << "</head>" << std::endl;
		debug_range << "<body>" << std::endl;
		debug_range << "<h1>Non-Contiguous Address Ranges (.debug_ranges)</h1>" << std::endl;
		debug_range << "<a href=\"../index.html\">Index</a>&nbsp;" << std::endl;
		unsigned int i;
		for(i = 0; i < num_debug_range_filenames; i++)
		{
			if(i != 0) debug_range << "&nbsp;";
			if(i == debug_range_filename_idx)
			{
				debug_range << i << std::endl;
			}
			else
			{
				std::stringstream sstr;
				sstr << i << ".html";
				debug_range << "<a href=\"" << sstr.str() << "\">" << i << "</a>" << std::endl;
			}
		}
		debug_range << "<table>" << std::endl;
		debug_range << "<tr>" << std::endl;
		debug_range << "<th>Entry type</th>" << std::endl;
		debug_range << "<th>Id</th>" << std::endl;
		debug_range << "<th>Offset</th>" << std::endl;
		debug_range << "<th>Next</th>" << std::endl;
		debug_range << "<th>Entry Information</th>" << std::endl;
		debug_range << "</tr>" << std::endl;
		unsigned int count = 0;
		while(count < debug_range_per_file && dw_range_list_iter != dw_range_list.end())
		{
			DWARF_RangeListEntry<MEMORY_ADDR> *dw_range = (*dw_range_list_iter).second;

			dw_range->to_HTML(debug_range) << std::endl;
			
			dw_range_list_iter++;
			count++;
		}
		debug_range << "</table>" << std::endl;
		debug_range << "</body>" << std::endl;
		debug_range << "</html>" << std::endl;
		debug_range_filename_idx++;
	}
	while(dw_range_list_iter != dw_range_list.end());

	// Location Lists
	
	std::stringstream debug_loc_ouput_dir_sstr;
	debug_loc_ouput_dir_sstr << output_dir << "/debug_loc";
	std::string debug_loc_output_dir(debug_loc_ouput_dir_sstr.str());
#if defined(WIN32)
	mkdir(debug_loc_output_dir.c_str());
#else
	mkdir(debug_loc_output_dir.c_str(), S_IRWXU);
#endif
	
	unsigned int debug_loc_filename_idx = 0;
	unsigned int debug_loc_per_file = 2048;
	unsigned int num_debug_locs = dw_loc_list.size();
	unsigned int num_debug_loc_filenames = ((num_debug_locs + debug_loc_per_file - 1) / debug_loc_per_file);
	typename std::map<uint64_t, DWARF_LocListEntry<MEMORY_ADDR> *>::iterator dw_loc_list_iter = dw_loc_list.begin();
	do
	{
		std::stringstream debug_loc_filename_sstr;
		debug_loc_filename_sstr << debug_loc_output_dir << "/" << debug_loc_filename_idx << ".html";
		std::string debug_loc_filename = debug_loc_filename_sstr.str().c_str();
		std::ofstream debug_loc(debug_loc_filename.c_str(), std::ios::out);
		debug_loc << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">" << std::endl;
		debug_loc << "<html>" << std::endl;
		debug_loc << "<head>" << std::endl;
		debug_loc << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		debug_loc << "<style type=\"text/css\">" << std::endl;
		debug_loc << "<!--" << std::endl;
		debug_loc << "table th { text-align:center; }" << std::endl;
		debug_loc << "table th { font-weight:bold; }" << std::endl;
		debug_loc << "table td { text-align:left; }" << std::endl;
		debug_loc << "table { border-style:solid; }" << std::endl;
		debug_loc << "table { border-width:1px; }" << std::endl;
		debug_loc << "th, td { border-style:solid; }" << std::endl;
		debug_loc << "th, td { border-width:1px; }" << std::endl;
		debug_loc << "th, td { border-width:1px; }" << std::endl;
		debug_loc << "table.loc { border-width:0px; }" << std::endl;
		debug_loc << "table.loc th { border-width:0px; }" << std::endl;
		debug_loc << "table.loc td { border-width:0px; }" << std::endl;
		debug_loc << "-->" << std::endl;
		debug_loc << "</style>" << std::endl;
		debug_loc << "</head>" << std::endl;
		debug_loc << "<body>" << std::endl;
		debug_loc << "<h1>Non-Contiguous Address Ranges (.debug_locs)</h1>" << std::endl;
		debug_loc << "<a href=\"../index.html\">Index</a>&nbsp;" << std::endl;
		unsigned int i;
		for(i = 0; i < num_debug_loc_filenames; i++)
		{
			if(i != 0) debug_loc << "&nbsp;";
			if(i == debug_loc_filename_idx)
			{
				debug_loc << i << std::endl;
			}
			else
			{
				std::stringstream sstr;
				sstr << i << ".html";
				debug_loc << "<a href=\"" << sstr.str() << "\">" << i << "</a>" << std::endl;
			}
		}
		debug_loc << "<table>" << std::endl;
		debug_loc << "<tr>" << std::endl;
		debug_loc << "<th>Entry type</th>" << std::endl;
		debug_loc << "<th>Id</th>" << std::endl;
		debug_loc << "<th>Offset</th>" << std::endl;
		debug_loc << "<th>Next</th>" << std::endl;
		debug_loc << "<th>Entry Information</th>" << std::endl;
		debug_loc << "</tr>" << std::endl;
		unsigned int count = 0;
		while(count < debug_loc_per_file && dw_loc_list_iter != dw_loc_list.end())
		{
			DWARF_LocListEntry<MEMORY_ADDR> *dw_loc = (*dw_loc_list_iter).second;

			dw_loc->to_HTML(debug_loc) << std::endl;
			
			dw_loc_list_iter++;
			count++;
		}
		debug_loc << "</table>" << std::endl;
		debug_loc << "</body>" << std::endl;
		debug_loc << "</html>" << std::endl;
		debug_loc_filename_idx++;
	}
	while(dw_loc_list_iter != dw_loc_list.end());

	// Call Frame Information
	std::stringstream debug_frame_ouput_dir_sstr;
	debug_frame_ouput_dir_sstr << output_dir << "/debug_frame";
	std::string debug_frame_output_dir(debug_frame_ouput_dir_sstr.str());
#if defined(WIN32)
	mkdir(debug_frame_output_dir.c_str());
#else
	mkdir(debug_frame_output_dir.c_str(), S_IRWXU);
#endif

	std::stringstream debug_frame_cies_ouput_dir_sstr;
	debug_frame_cies_ouput_dir_sstr << debug_frame_output_dir << "/cies";
	std::string debug_frame_cies_output_dir(debug_frame_cies_ouput_dir_sstr.str());
#if defined(WIN32)
	mkdir(debug_frame_cies_output_dir.c_str());
#else
	mkdir(debug_frame_cies_output_dir.c_str(), S_IRWXU);
#endif

	std::stringstream debug_frame_fdes_ouput_dir_sstr;
	debug_frame_fdes_ouput_dir_sstr << debug_frame_output_dir << "/fdes";
	std::string debug_frame_fdes_output_dir(debug_frame_fdes_ouput_dir_sstr.str());
#if defined(WIN32)
	mkdir(debug_frame_fdes_output_dir.c_str());
#else
	mkdir(debug_frame_fdes_output_dir.c_str(), S_IRWXU);
#endif

	unsigned int debug_frame_cies_filename_idx = 0;
	unsigned int cies_per_file = 2048;
	unsigned int num_debug_cies = dw_cies.size();
	unsigned int num_debug_frame_cies_filenames = ((num_debug_cies + cies_per_file - 1) /cies_per_file);
	typename std::map<uint64_t, DWARF_CIE<MEMORY_ADDR> *>::iterator dw_cie_iter = dw_cies.begin();
	do
	{
		std::stringstream debug_frame_cies_filename_sstr;
		debug_frame_cies_filename_sstr << debug_frame_cies_output_dir << "/" << debug_frame_cies_filename_idx << ".html";
		std::string debug_frame_cies_filename = debug_frame_cies_filename_sstr.str().c_str();
		std::ofstream debug_frame_cies(debug_frame_cies_filename.c_str(), std::ios::out);
		debug_frame_cies << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">" << std::endl;
		debug_frame_cies << "<html>" << std::endl;
		debug_frame_cies << "<head>" << std::endl;
		debug_frame_cies << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		debug_frame_cies << "<style type=\"text/css\">" << std::endl;
		debug_frame_cies << "<!--" << std::endl;
		debug_frame_cies << "table th { text-align:center; }" << std::endl;
		debug_frame_cies << "table th { font-weight:bold; }" << std::endl;
		debug_frame_cies << "table td { text-align:left; }" << std::endl;
		debug_frame_cies << "table { border-style:solid; }" << std::endl;
		debug_frame_cies << "table { border-width:1px; }" << std::endl;
		debug_frame_cies << "th, td { border-style:solid; }" << std::endl;
		debug_frame_cies << "th, td { border-width:1px; }" << std::endl;
		debug_frame_cies << "th, td { border-width:1px; }" << std::endl;
		debug_frame_cies << "-->" << std::endl;
		debug_frame_cies << "</style>" << std::endl;
		debug_frame_cies << "</head>" << std::endl;
		debug_frame_cies << "<body>" << std::endl;
		debug_frame_cies << "<h1>Call Frame Information/Common Information Entries (CIE) (.debug_frame)</h1>" << std::endl;
		debug_frame_cies << "<a href=\"../../index.html\">Index</a>&nbsp;" << std::endl;
		unsigned int i;
		for(i = 0; i < num_debug_frame_cies_filenames; i++)
		{
			if(i != 0) debug_frame_cies << "&nbsp;";
			if(i == debug_frame_cies_filename_idx)
			{
				debug_frame_cies << i << std::endl;
			}
			else
			{
				std::stringstream sstr;
				sstr << i << ".html";
				debug_frame_cies << "<a href=\"" << sstr.str() << "\">" << i << "</a>" << std::endl;
			}
		}
		debug_frame_cies << "<hr>" << std::endl;
		debug_frame_cies << "<table>" << std::endl;
		debug_frame_cies << "<tr>" << std::endl;
		debug_frame_cies << "<th>Id</th>" << std::endl;
		debug_frame_cies << "<th>Offset</th>" << std::endl;
		debug_frame_cies << "<th>Version</th>" << std::endl;
		debug_frame_cies << "<th>Augmentation</th>" << std::endl;
		debug_frame_cies << "<th>Code alignment factor</th>" << std::endl;
		debug_frame_cies << "<th>Data alignment factor</th>" << std::endl;
		debug_frame_cies << "<th>Return address register</th>" << std::endl;
		debug_frame_cies << "<th>Initial call frame program</th>" << std::endl;
		debug_frame_cies << "</tr>" << std::endl;
		unsigned int count = 0;
		while(count < cies_per_file && dw_cie_iter != dw_cies.end())
		{
			DWARF_CIE<MEMORY_ADDR> *dw_cie = (*dw_cie_iter).second;

			dw_cie->to_HTML(debug_frame_cies) << std::endl;
			
			dw_cie_iter++;
			count++;
		}
		debug_frame_cies << "</table>" << std::endl;
		debug_frame_cies << "<hr>" << std::endl;
		for(i = 0; i < num_debug_frame_cies_filenames; i++)
		{
			if(i != 0) debug_frame_cies << "&nbsp;";
			if(i == debug_frame_cies_filename_idx)
			{
				debug_frame_cies << i << std::endl;
			}
			else
			{
				std::stringstream sstr;
				sstr << i << ".html";
				debug_frame_cies << "<a href=\"" << sstr.str() << "\">" << i << "</a>" << std::endl;
			}
		}
		debug_frame_cies << "</body>" << std::endl;
		debug_frame_cies << "</html>" << std::endl;
		debug_frame_cies_filename_idx++;
	}
	while(dw_cie_iter != dw_cies.end());
	
	unsigned int debug_frame_fdes_filename_idx = 0;
	unsigned int fdes_per_file = 2048;
	unsigned int num_debug_fdes = dw_fdes.size();
	unsigned int num_debug_frame_fdes_filenames = ((num_debug_fdes + fdes_per_file - 1) /fdes_per_file);
	typename std::vector<DWARF_FDE<MEMORY_ADDR> *>::iterator dw_fde_iter = dw_fdes.begin();
	do
	{
		std::stringstream debug_frame_fdes_filename_sstr;
		debug_frame_fdes_filename_sstr << debug_frame_fdes_output_dir << "/" << debug_frame_fdes_filename_idx << ".html";
		std::string debug_frame_fdes_filename = debug_frame_fdes_filename_sstr.str().c_str();
		std::ofstream debug_frame_fdes(debug_frame_fdes_filename.c_str(), std::ios::out);
		debug_frame_fdes << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">" << std::endl;
		debug_frame_fdes << "<html>" << std::endl;
		debug_frame_fdes << "<head>" << std::endl;
		debug_frame_fdes << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		debug_frame_fdes << "<style type=\"text/css\">" << std::endl;
		debug_frame_fdes << "<!--" << std::endl;
		debug_frame_fdes << "table th { text-align:center; }" << std::endl;
		debug_frame_fdes << "table th { font-weight:bold; }" << std::endl;
		debug_frame_fdes << "table td { text-align:left; }" << std::endl;
		debug_frame_fdes << "table { border-style:solid; }" << std::endl;
		debug_frame_fdes << "table { border-width:1px; }" << std::endl;
		debug_frame_fdes << "th, td { border-style:solid; }" << std::endl;
		debug_frame_fdes << "th, td { border-width:1px; }" << std::endl;
		debug_frame_fdes << "th, td { border-width:1px; }" << std::endl;
		debug_frame_fdes << "-->" << std::endl;
		debug_frame_fdes << "</style>" << std::endl;
		debug_frame_fdes << "</head>" << std::endl;
		debug_frame_fdes << "<body>" << std::endl;
		debug_frame_fdes << "<h1>Call Frame Information/Frame Description Entries (FDE) (.debug_frame)</h1>" << std::endl;
		debug_frame_fdes << "<a href=\"../../index.html\">Index</a>&nbsp;" << std::endl;
		unsigned int i;
		for(i = 0; i < num_debug_frame_fdes_filenames; i++)
		{
			if(i != 0) debug_frame_fdes << "&nbsp;";
			if(i == debug_frame_fdes_filename_idx)
			{
				debug_frame_fdes << i << std::endl;
			}
			else
			{
				std::stringstream sstr;
				sstr << i << ".html";
				debug_frame_fdes << "<a href=\"" << sstr.str() << "\">" << i << "</a>" << std::endl;
			}
		}
		debug_frame_fdes << "<hr>" << std::endl;
		debug_frame_fdes << "<table>" << std::endl;
		debug_frame_fdes << "<tr>" << std::endl;
		debug_frame_fdes << "<th>Offset</th>" << std::endl;
		debug_frame_fdes << "<th>CIE</th>" << std::endl;
		debug_frame_fdes << "<th>Initial location</th>" << std::endl;
		debug_frame_fdes << "<th>Address range</th>" << std::endl;
		debug_frame_fdes << "<th>Call frame program</th>" << std::endl;
		debug_frame_fdes << "</tr>" << std::endl;
		unsigned int count = 0;
		while(count < fdes_per_file && dw_fde_iter != dw_fdes.end())
		{
			DWARF_FDE<MEMORY_ADDR> *dw_fde = (*dw_fde_iter);

			dw_fde->to_HTML(debug_frame_fdes) << std::endl;
			
			dw_fde_iter++;
			count++;
		}
		debug_frame_fdes << "</table>" << std::endl;
		debug_frame_fdes << "<hr>" << std::endl;
		for(i = 0; i < num_debug_frame_fdes_filenames; i++)
		{
			if(i != 0) debug_frame_fdes << "&nbsp;";
			if(i == debug_frame_fdes_filename_idx)
			{
				debug_frame_fdes << i << std::endl;
			}
			else
			{
				std::stringstream sstr;
				sstr << i << ".html";
				debug_frame_fdes << "<a href=\"" << sstr.str() << "\">" << i << "</a>" << std::endl;
			}
		}
		debug_frame_fdes << "</body>" << std::endl;
		debug_frame_fdes << "</html>" << std::endl;
		debug_frame_fdes_filename_idx++;
	}
	while(dw_fde_iter != dw_fdes.end());
	
	// Debugging Information
	std::stringstream debug_info_ouput_dir_sstr;
	debug_info_ouput_dir_sstr << output_dir << "/debug_info";
	std::string debug_info_output_dir(debug_info_ouput_dir_sstr.str());
#if defined(WIN32)
	mkdir(debug_info_output_dir.c_str());
#else
	mkdir(debug_info_output_dir.c_str(), S_IRWXU);
#endif

	std::stringstream debug_info_cus_ouput_dir_sstr;
	debug_info_cus_ouput_dir_sstr << debug_info_output_dir << "/cus";
	std::string debug_info_cus_output_dir(debug_info_cus_ouput_dir_sstr.str());
#if defined(WIN32)
	mkdir(debug_info_cus_output_dir.c_str());
#else
	mkdir(debug_info_cus_output_dir.c_str(), S_IRWXU);
#endif

	std::stringstream debug_info_dies_ouput_dir_sstr;
	debug_info_dies_ouput_dir_sstr << debug_info_output_dir << "/dies";
	std::string debug_info_dies_output_dir(debug_info_dies_ouput_dir_sstr.str());
#if defined(WIN32)
	mkdir(debug_info_dies_output_dir.c_str());
#else
	mkdir(debug_info_dies_output_dir.c_str(), S_IRWXU);
#endif

	unsigned int debug_info_cus_filename_idx = 0;
	unsigned int num_debug_cus = dw_cus.size();
	unsigned int num_debug_info_cus_filenames = ((num_debug_cus + cus_per_file - 1) / cus_per_file);
	typename std::map<uint64_t, DWARF_CompilationUnit<MEMORY_ADDR> *>::iterator dw_cu_iter = dw_cus.begin();
	do
	{
		std::stringstream debug_info_cus_filename_sstr;
		debug_info_cus_filename_sstr << debug_info_cus_output_dir << "/" << debug_info_cus_filename_idx << ".html";
		std::string debug_info_cus_filename = debug_info_cus_filename_sstr.str().c_str();
		std::ofstream debug_info_cus(debug_info_cus_filename.c_str(), std::ios::out);
		debug_info_cus << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">" << std::endl;
		debug_info_cus << "<html>" << std::endl;
		debug_info_cus << "<head>" << std::endl;
		debug_info_cus << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		debug_info_cus << "<style type=\"text/css\">" << std::endl;
		debug_info_cus << "<!--" << std::endl;
		debug_info_cus << "table th { text-align:center; }" << std::endl;
		debug_info_cus << "table th { font-weight:bold; }" << std::endl;
		debug_info_cus << "table td { text-align:left; }" << std::endl;
		debug_info_cus << "table { border-style:solid; }" << std::endl;
		debug_info_cus << "table { border-width:1px; }" << std::endl;
		debug_info_cus << "th, td { border-style:solid; }" << std::endl;
		debug_info_cus << "th, td { border-width:1px; }" << std::endl;
		debug_info_cus << "th, td { border-width:1px; }" << std::endl;
		debug_info_cus << "-->" << std::endl;
		debug_info_cus << "</style>" << std::endl;
		debug_info_cus << "</head>" << std::endl;
		debug_info_cus << "<body>" << std::endl;
		debug_info_cus << "<h1>Debug Information/Compilation Units (.debug_info)</h1>" << std::endl;
		debug_info_cus << "<a href=\"../../index.html\">Index</a>&nbsp;" << std::endl;
		unsigned int i;
		for(i = 0; i < num_debug_info_cus_filenames; i++)
		{
			if(i != 0) debug_info_cus << "&nbsp;";
			if(i == debug_info_cus_filename_idx)
			{
				debug_info_cus << i << std::endl;
			}
			else
			{
				std::stringstream sstr;
				sstr << i << ".html";
				debug_info_cus << "<a href=\"" << sstr.str() << "\">" << i << "</a>" << std::endl;
			}
		}
		debug_info_cus << "<table>" << std::endl;
		debug_info_cus << "<tr>" << std::endl;
		debug_info_cus << "<th>Id</th>" << std::endl;
		debug_info_cus << "<th>Offset</th>" << std::endl;
		debug_info_cus << "<th>Version</th>" << std::endl;
		debug_info_cus << "<th>Address size</th>" << std::endl;
		debug_info_cus << "<th>DIEs</th>" << std::endl;
		debug_info_cus << "</tr>" << std::endl;
		unsigned int count = 0;
		while(count < cus_per_file && dw_cu_iter != dw_cus.end())
		{
			DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu = (*dw_cu_iter).second;

			dw_cu->to_HTML(debug_info_cus) << std::endl;
			
			dw_cu_iter++;
			count++;
		}
		debug_info_cus << "</table>" << std::endl;
		debug_info_cus << "</body>" << std::endl;
		debug_info_cus << "</html>" << std::endl;
		debug_info_cus_filename_idx++;
	}
	while(dw_cu_iter != dw_cus.end());
	
	unsigned int debug_info_dies_filename_idx = 0;
	unsigned int dies_per_file = 2048;
	unsigned int num_debug_dies = dw_dies.size();
	unsigned int num_debug_info_dies_filenames = ((num_debug_dies + dies_per_file - 1) /dies_per_file);
	typename std::map<uint64_t, DWARF_DIE<MEMORY_ADDR> *>::iterator dw_die_iter = dw_dies.begin();
	do
	{
		std::stringstream debug_info_dies_filename_sstr;
		debug_info_dies_filename_sstr << debug_info_dies_output_dir << "/" << debug_info_dies_filename_idx << ".html";
		std::string debug_info_dies_filename = debug_info_dies_filename_sstr.str().c_str();
		std::ofstream debug_info_dies(debug_info_dies_filename.c_str(), std::ios::out);
		debug_info_dies << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">" << std::endl;
		debug_info_dies << "<html>" << std::endl;
		debug_info_dies << "<head>" << std::endl;
		debug_info_dies << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		debug_info_dies << "<style type=\"text/css\">" << std::endl;
		debug_info_dies << "<!--" << std::endl;
		debug_info_dies << "table th { text-align:center; }" << std::endl;
		debug_info_dies << "table th { font-weight:bold; }" << std::endl;
		debug_info_dies << "table td { text-align:left; }" << std::endl;
		debug_info_dies << "table { border-style:solid; }" << std::endl;
		debug_info_dies << "table { border-width:1px; }" << std::endl;
		debug_info_dies << "th, td { border-style:solid; }" << std::endl;
		debug_info_dies << "th, td { border-width:1px; }" << std::endl;
		debug_info_dies << "th, td { border-width:1px; }" << std::endl;
		debug_info_dies << "-->" << std::endl;
		debug_info_dies << "</style>" << std::endl;
		debug_info_dies << "</head>" << std::endl;
		debug_info_dies << "<body>" << std::endl;
		debug_info_dies << "<h1>Debug Information/Debug Information Entries (.debug_info)</h1>" << std::endl;
		debug_info_dies << "<a href=\"../../index.html\">Index</a>&nbsp;" << std::endl;
		unsigned int i;
		for(i = 0; i < num_debug_info_dies_filenames; i++)
		{
			if(i != 0) debug_info_dies << "&nbsp;";
			if(i == debug_info_dies_filename_idx)
			{
				debug_info_dies << i << std::endl;
			}
			else
			{
				std::stringstream sstr;
				sstr << i << ".html";
				debug_info_dies << "<a href=\"" << sstr.str() << "\">" << i << "</a>" << std::endl;
			}
		}
		debug_info_dies << "<hr>" << std::endl;
		debug_info_dies << "<table>" << std::endl;
		debug_info_dies << "<tr>" << std::endl;
		debug_info_dies << "<th>Id</th>" << std::endl;
		debug_info_dies << "<th>Offset</th>" << std::endl;
		debug_info_dies << "<th>Compilation Unit</th>" << std::endl;
		debug_info_dies << "<th>Parent DIE</th>" << std::endl;
		debug_info_dies << "<th>Children</th>" << std::endl;
		debug_info_dies << "<th>Entry Information</th>" << std::endl;
		debug_info_dies << "</tr>" << std::endl;
		unsigned int count = 0;
		while(count < dies_per_file && dw_die_iter != dw_dies.end())
		{
			DWARF_DIE<MEMORY_ADDR> *dw_die = (*dw_die_iter).second;

			dw_die->to_HTML(debug_info_dies) << std::endl;
			
			dw_die_iter++;
			count++;
		}
		debug_info_dies << "</table>" << std::endl;
		debug_info_dies << "<hr>" << std::endl;
		for(i = 0; i < num_debug_info_dies_filenames; i++)
		{
			if(i != 0) debug_info_dies << "&nbsp;";
			if(i == debug_info_dies_filename_idx)
			{
				debug_info_dies << i << std::endl;
			}
			else
			{
				std::stringstream sstr;
				sstr << i << ".html";
				debug_info_dies << "<a href=\"" << sstr.str() << "\">" << i << "</a>" << std::endl;
			}
		}
		debug_info_dies << "</body>" << std::endl;
		debug_info_dies << "</html>" << std::endl;
		debug_info_dies_filename_idx++;
	}
	while(dw_die_iter != dw_dies.end());
}

template <class MEMORY_ADDR>
void DWARF_Handler<MEMORY_ADDR>::Register(DWARF_StatementProgram<MEMORY_ADDR> *dw_stmt_prog)
{
	dw_stmt_progs.insert(std::pair<uint64_t, DWARF_StatementProgram<MEMORY_ADDR> *>(dw_stmt_prog->GetOffset(), dw_stmt_prog));
}

template <class MEMORY_ADDR>
void DWARF_Handler<MEMORY_ADDR>::Register(DWARF_DIE<MEMORY_ADDR> *dw_die)
{
	//std::cerr << "Registering " << dw_die << " at offset " << dw_die->GetOffset() << std::endl;
	dw_dies.insert(std::pair<uint64_t, DWARF_DIE<MEMORY_ADDR> *>(dw_die->GetOffset(), dw_die));
}

template <class MEMORY_ADDR>
void DWARF_Handler<MEMORY_ADDR>::Register(DWARF_RangeListEntry<MEMORY_ADDR> *dw_range_list_entry)
{
	dw_range_list.insert(std::pair<uint64_t, DWARF_RangeListEntry<MEMORY_ADDR> *>(dw_range_list_entry->GetOffset(), dw_range_list_entry));
}

template <class MEMORY_ADDR>
void DWARF_Handler<MEMORY_ADDR>::Register(DWARF_MacInfoListEntry<MEMORY_ADDR> *dw_macinfo_list_entry)
{
	dw_macinfo_list.insert(std::pair<uint64_t, DWARF_MacInfoListEntry<MEMORY_ADDR> *>(dw_macinfo_list_entry->GetOffset(), dw_macinfo_list_entry));
}

template <class MEMORY_ADDR>
void DWARF_Handler<MEMORY_ADDR>::Register(DWARF_LocListEntry<MEMORY_ADDR> *dw_loc_list_entry)
{
	dw_loc_list.insert(std::pair<uint64_t, DWARF_LocListEntry<MEMORY_ADDR> *>(dw_loc_list_entry->GetOffset(), dw_loc_list_entry));
}

template <class MEMORY_ADDR>
void DWARF_Handler<MEMORY_ADDR>::DumpStatementMatrix()
{
	typename std::map<MEMORY_ADDR, Statement<MEMORY_ADDR> *>::const_iterator stmt_iter;

	for(stmt_iter = stmt_matrix.begin(); stmt_iter != stmt_matrix.end(); stmt_iter++)
	{
		if((*stmt_iter).second)
		{
			std::cout << *(*stmt_iter).second << std::endl;
		}
	}
}

template <class MEMORY_ADDR>
const unisim::util::debug::Statement<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindStatement(MEMORY_ADDR addr) const
{
	typename std::map<MEMORY_ADDR, Statement<MEMORY_ADDR> *>::const_iterator stmt_iter = stmt_matrix.find(addr);
	
	return (stmt_iter != stmt_matrix.end()) ? (*stmt_iter).second : 0;
}

template <class MEMORY_ADDR>
const unisim::util::debug::Statement<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindStatement(const char *filename, unsigned int lineno, unsigned int colno) const
{
	bool requested_filename_is_absolute = IsAbsolutePath(filename);
	std::vector<std::string> hierarchical_requested_filename;
	
	std::string s;
	const char *p = filename;
	do
	{
		if(*p == 0 || *p == '/' || *p == '\\')
		{
			hierarchical_requested_filename.push_back(s);
			s.clear();
		}
		else
		{
			s += *p;
		}
	} while(*(p++));
	int hierarchical_requested_filename_depth = hierarchical_requested_filename.size();

	typename std::map<MEMORY_ADDR, Statement<MEMORY_ADDR> *>::const_iterator stmt_iter;
	
	for(stmt_iter = stmt_matrix.begin(); stmt_iter != stmt_matrix.end(); stmt_iter++)
	{
		Statement<MEMORY_ADDR> *stmt = (*stmt_iter).second;
		
		if(stmt)
		{
			if(stmt->GetLineNo() == lineno && (!colno || (stmt->GetColNo() == colno)))
			{
				std::string source_path;
				const char *source_filename = stmt->GetSourceFilename();
				if(source_filename)
				{
					const char *source_dirname = stmt->GetSourceDirname();
					if(source_dirname)
					{
						source_path += source_dirname;
						source_path += '/';
					}
					source_path += source_filename;

					std::vector<std::string> hierarchical_source_path;
					
					s.clear();
					p = source_path.c_str();
					do
					{
						if(*p == 0 || *p == '/' || *p == '\\')
						{
							hierarchical_source_path.push_back(s);
							s.clear();
						}
						else
						{
							s += *p;
						}
					} while(*(p++));

					int hierarchical_source_path_depth = hierarchical_source_path.size();
					
					if((!requested_filename_is_absolute && hierarchical_source_path_depth >= hierarchical_requested_filename_depth) ||
					   (requested_filename_is_absolute && hierarchical_source_path_depth == hierarchical_requested_filename_depth))
					{
						int i;
						bool match = true;
						
						for(i = 0; i < hierarchical_requested_filename_depth; i++)
						{
							if(hierarchical_source_path[hierarchical_source_path_depth - 1 - i] != hierarchical_requested_filename[hierarchical_requested_filename_depth - 1 - i])
							{
								match = false;
								break;
							}
						}
						
						if(match) return stmt;
					}
				}
			}
		}
	}
	return 0;
}

template <class MEMORY_ADDR>
bool DWARF_Handler<MEMORY_ADDR>::IsAbsolutePath(const char *filename) const
{
	// filename starts '/' or 'drive letter':\ or 'driver letter':/
	return (((filename[0] >= 'a' && filename[0] <= 'z') || (filename[0] >= 'A' && filename[0] <= 'Z')) && (filename[1] == ':') && ((filename[2] == '\\') || (filename[2] == '/'))) || (*filename == '/');
}

template <class MEMORY_ADDR>
endian_type DWARF_Handler<MEMORY_ADDR>::GetEndianness() const
{
	return endianness;
}

template <class MEMORY_ADDR>
uint8_t DWARF_Handler<MEMORY_ADDR>::GetAddressSize() const
{
	return address_size;
}

template <class MEMORY_ADDR>
const DWARF_StatementProgram<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindStatementProgram(uint64_t debug_line_offset)
{
	if(!debug_line_section || debug_line_offset >= debug_line_section->GetSize()) return 0;
	
	typename std::map<uint64_t, DWARF_StatementProgram<MEMORY_ADDR> *>::const_iterator dw_stmt_prog_iter = dw_stmt_progs.find(debug_line_offset);
		
	if(dw_stmt_prog_iter != dw_stmt_progs.end())
	{
		return (*dw_stmt_prog_iter).second;
	}

	DWARF_StatementProgram<MEMORY_ADDR> *dw_stmt_prog = new DWARF_StatementProgram<MEMORY_ADDR>(this);
	int64_t sz;
	if((sz = dw_stmt_prog->Load((const uint8_t *) debug_line_section->GetData() + debug_line_offset, debug_line_section->GetSize() - debug_line_offset, debug_line_offset)) < 0)
	{
		logger << DebugWarning << "Invalid DWARF v2/v3 statement program prologue at offset 0x" << std::hex << debug_line_offset << std::dec << EndDebugWarning;
		delete dw_stmt_prog;
		return 0;
	}

	Register(dw_stmt_prog);
	//std::cerr << *dw_stmt_prog << std::endl;
	return dw_stmt_prog;
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindDIE(uint64_t debug_info_offset) const
{
	typename std::map<uint64_t, DWARF_DIE<MEMORY_ADDR> *>::const_iterator dw_die_iter = dw_dies.find(debug_info_offset);
	
	return dw_die_iter != dw_dies.end() ? (*dw_die_iter).second : 0;
}

template <class MEMORY_ADDR>
const DWARF_RangeListEntry<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindRangeListEntry(const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu, uint64_t debug_ranges_offset)
{
	if(!debug_ranges_section || debug_ranges_offset >= debug_ranges_section->GetSize()) return 0;
	
	DWARF_RangeListEntry<MEMORY_ADDR> *head = 0;
	do
	{
		typename std::map<uint64_t, DWARF_RangeListEntry<MEMORY_ADDR> *>::const_iterator dw_range_list_iter = dw_range_list.find(debug_ranges_offset);
		
		if(dw_range_list_iter != dw_range_list.end())
		{
			return head ? head : (*dw_range_list_iter).second;
		}
		
		DWARF_RangeListEntry<MEMORY_ADDR> *dw_range_list_entry = new DWARF_RangeListEntry<MEMORY_ADDR>(dw_cu);
		int64_t sz;
			
		if((sz = dw_range_list_entry->Load((const uint8_t *) debug_ranges_section->GetData() + debug_ranges_offset, debug_ranges_section->GetSize() - debug_ranges_offset, debug_ranges_offset)) < 0)
		{
			logger << DebugWarning << "Invalid DWARF v2/v3 debug ranges at offset 0x" << std::hex << debug_ranges_offset << std::dec << EndDebugWarning;
			delete dw_range_list_entry;
			return head;
		}

		//std::cerr << *dw_range_list_entry << std::endl;
		Register(dw_range_list_entry);
		debug_ranges_offset += sz;
		if(!head) head = dw_range_list_entry;
		if(dw_range_list_entry->IsEndOfList()) break; // End of list
	} while(debug_ranges_offset < debug_ranges_section->GetSize());
	
	return head;
}

template <class MEMORY_ADDR>
const DWARF_MacInfoListEntry<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindMacInfoListEntry(uint64_t debug_macinfo_offset)
{
	if(!debug_macinfo_section || debug_macinfo_offset >= debug_macinfo_section->GetSize()) return 0;

	DWARF_MacInfoListEntry<MEMORY_ADDR> *head = 0;
	do
	{
		typename std::map<uint64_t, DWARF_MacInfoListEntry<MEMORY_ADDR> *>::const_iterator dw_macinfo_list_iter = dw_macinfo_list.find(debug_macinfo_offset);
		
		if(dw_macinfo_list_iter != dw_macinfo_list.end())
		{
			return head ? head : (*dw_macinfo_list_iter).second;
		}
		
		uint8_t dw_mac_info_type = *((const uint8_t *) debug_macinfo_section->GetData() + debug_macinfo_offset);

		DWARF_MacInfoListEntry<MEMORY_ADDR> *dw_macinfo_list_entry = 0;

		switch(dw_mac_info_type)
		{
			case 0: // Null entry
				dw_macinfo_list_entry = new DWARF_MacInfoListEntryNull<MEMORY_ADDR>();
				break;
			case DW_MACINFO_define:
				dw_macinfo_list_entry = new DWARF_MacInfoListEntryDefine<MEMORY_ADDR>();
				break;
			case DW_MACINFO_undef:
				dw_macinfo_list_entry = new DWARF_MacInfoListEntryUndef<MEMORY_ADDR>();
				break;
			case DW_MACINFO_start_file:
				dw_macinfo_list_entry = new DWARF_MacInfoListEntryStartFile<MEMORY_ADDR>();
				break;
			case DW_MACINFO_end_file:
				dw_macinfo_list_entry = new DWARF_MacInfoListEntryEndFile<MEMORY_ADDR>();
				break;
			case DW_MACINFO_vendor_ext:
				dw_macinfo_list_entry = new DWARF_MacInfoListEntryVendorExtension<MEMORY_ADDR>();
				break;
			default:
				logger << DebugWarning << "Invalid DWARF v2/v3 debug macinfo at offset 0x" << std::hex << debug_macinfo_offset << std::dec << " (unknown type " << ((unsigned int) dw_mac_info_type) << ")" << EndDebugWarning;
				return 0;
		}
			
		if(!dw_macinfo_list_entry) return head;

		int64_t sz;
			
		if((sz = dw_macinfo_list_entry->Load((const uint8_t *) debug_macinfo_section->GetData() + debug_macinfo_offset, debug_macinfo_section->GetSize() - debug_macinfo_offset, debug_macinfo_offset)) < 0)
		{
			logger << DebugWarning << "Invalid DWARF v2/v3 debug macinfo at offset 0x" << std::hex << debug_macinfo_offset << std::dec << " (type " << ((unsigned int) dw_mac_info_type) << ")" << EndDebugWarning;
			delete dw_macinfo_list_entry;
			return head;
		}

		//std::cerr << *dw_macinfo_list_entry << std::endl;
		Register(dw_macinfo_list_entry);
		debug_macinfo_offset += sz;
		if(!head) head = dw_macinfo_list_entry;
		if(dw_macinfo_list_entry->GetType() == 0) break; // Null entry, i.e. End of list
	} while(debug_macinfo_offset < debug_macinfo_section->GetSize());
	
	return head;
}

template <class MEMORY_ADDR>
const DWARF_CompilationUnit<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindCompilationUnit(uint64_t debug_info_offset) const
{
	typename std::map<uint64_t, DWARF_CompilationUnit<MEMORY_ADDR> *>::const_iterator dw_cu_iter = dw_cus.find(debug_info_offset);
	
	return dw_cu_iter != dw_cus.end() ? (*dw_cu_iter).second : 0;
}

template <class MEMORY_ADDR>
const DWARF_LocListEntry<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindLocListEntry(const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu, uint64_t debug_loc_offset)
{
	if(!debug_loc_section || debug_loc_offset >= debug_loc_section->GetSize()) return 0;
	
	DWARF_LocListEntry<MEMORY_ADDR> *head = 0;
	
	do
	{
		typename std::map<uint64_t, DWARF_LocListEntry<MEMORY_ADDR> *>::const_iterator dw_loc_list_iter = dw_loc_list.find(debug_loc_offset);
			
		if(dw_loc_list_iter != dw_loc_list.end())
		{
			return head ? head : (*dw_loc_list_iter).second;
		}
			
		DWARF_LocListEntry<MEMORY_ADDR> *dw_loc_list_entry = new DWARF_LocListEntry<MEMORY_ADDR>(dw_cu);
		int64_t sz;
			
		if((sz = dw_loc_list_entry->Load((const uint8_t *) debug_loc_section->GetData() + debug_loc_offset, debug_loc_section->GetSize() - debug_loc_offset, debug_loc_offset)) < 0)
		{
			logger << DebugWarning << "Invalid DWARF v2/v3 debug loc at offset 0x" << std::hex << debug_loc_offset << std::dec << EndDebugWarning;
			delete dw_loc_list_entry;
			return head;
		}

		//std::cerr << *dw_loc_list_entry << std::endl;
		Register(dw_loc_list_entry);
		debug_loc_offset += sz;
		if(!head) head = dw_loc_list_entry;
		if(dw_loc_list_entry->IsEndOfList())
		{
			break; // End of list
		}
	} while(debug_loc_offset < debug_loc_section->GetSize());
	
	return head;
}

template <class MEMORY_ADDR>
const DWARF_CIE<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindCIE(uint64_t debug_frame_offset) const
{
	typename std::map<uint64_t, DWARF_CIE<MEMORY_ADDR> *>::const_iterator dw_cie_iter = dw_cies.find(debug_frame_offset);
	
	return dw_cie_iter != dw_cies.end() ? (*dw_cie_iter).second : 0;
}

template <class MEMORY_ADDR>
void DWARF_Handler<MEMORY_ADDR>::BuildStatementMatrix()
{
	typename std::map<uint64_t, DWARF_CompilationUnit<MEMORY_ADDR> *>::iterator dw_cu_iter;
	
	for(dw_cu_iter = dw_cus.begin(); dw_cu_iter != dw_cus.end(); dw_cu_iter++)
	{
		DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu = (*dw_cu_iter).second;

		dw_cu->BuildStatementMatrix(stmt_matrix);
	}
}

/*
template <class MEMORY_ADDR>
bool DWARF_Handler<MEMORY_ADDR>::GetCFI(MEMORY_ADDR pc, DWARF_UnwindContext<MEMORY_ADDR>& unwind_context, std::vector<DWARF_RuleMatrixRow<MEMORY_ADDR> *>& backtrace) const
{
	unsigned int i;
	unsigned int num_fdes = dw_fdes.size();
	for(i = 0; i < num_fdes; i++)
	{
		DWARF_FDE<MEMORY_ADDR> *dw_fde = dw_fdes[i];
		MEMORY_ADDR initial_location = dw_fde->GetInitialLocation();
		MEMORY_ADDR address_range = dw_fde->GetAddressRange();
		
		if((pc >= initial_location) && (pc < (initial_location + address_range)))
		{
			// found FDE
			
			DWARF_RuleMatrix<MEMORY_ADDR> rule_matrix;
			
			const DWARF_CIE<MEMORY_ADDR> *dw_cie = dw_fde->GetCIE();
			
			const DWARF_CallFrameProgram<MEMORY_ADDR> *initial_instructions = dw_cie->GetInitialInstructions();
			
			MEMORY_ADDR location = initial_location;
			
			DWARF_CallFrameVM<MEMORY_ADDR> dw_call_frame_vm;
			if(!dw_call_frame_vm.Execute(*initial_instructions, location, rule_matrix))
			{
				return false;
			}
			
			const DWARF_CallFrameProgram<MEMORY_ADDR> *instructions = dw_fde->GetInstructions();
			if(!dw_call_frame_vm.Execute(*instructions, location, rule_matrix))
			{
				return false;
			}
			
			DWARF_RuleMatrixRow<MEMORY_ADDR> *rule_matrix_row = rule_matrix->GetRow(pc);
			
			if(!rule_matrix_row) return false;
			
			unsigned int return_address_register = dw_cie->GetReturnAddressRegister();
			
			DWARF_RegisterRule<MEMORY_ADDR> *return_address_register_rule = rule_matrix_row->GetRegisterRule(return_address_register);
			
			if(!return_address_register_rule) return false;
			
			
		}
	}
	return false;
}
*/
template <class MEMORY_ADDR>
std::vector<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::GetBackTrace(MEMORY_ADDR pc) const
{
	std::vector<MEMORY_ADDR> *backtrace = new std::vector<MEMORY_ADDR>();
	
	//GetBackTrace(pc, *back_trace);

	return backtrace;
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
