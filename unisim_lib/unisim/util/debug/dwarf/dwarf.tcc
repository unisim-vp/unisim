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
#include <unisim/util/debug/dwarf/register_number_mapping.hh>
#include <unisim/util/debug/dwarf/frame.hh>
#include <unisim/util/debug/dwarf/data_object.hh>
#include <unisim/util/debug/dwarf/c_loc_expr_parser.hh>

#include <unisim/util/debug/dwarf/addr_range.tcc>
#include <unisim/util/debug/dwarf/attr.tcc>
#include <unisim/util/debug/dwarf/call_frame_prog.tcc>
#include <unisim/util/debug/dwarf/call_frame_vm.tcc>
#include <unisim/util/debug/dwarf/cie.tcc>
#include <unisim/util/debug/dwarf/cu.tcc>
#include <unisim/util/debug/dwarf/die.tcc>
#include <unisim/util/debug/dwarf/expr_vm.tcc>
#include <unisim/util/debug/dwarf/fde.tcc>
#include <unisim/util/debug/dwarf/loc.tcc>
#include <unisim/util/debug/dwarf/macinfo.tcc>
#include <unisim/util/debug/dwarf/pub.tcc>
#include <unisim/util/debug/dwarf/range.tcc>
#include <unisim/util/debug/dwarf/stmt_prog.tcc>
#include <unisim/util/debug/dwarf/stmt_vm.tcc>
#include <unisim/util/debug/dwarf/frame.tcc>
#include <unisim/util/debug/dwarf/data_object.tcc>
#include <unisim/util/debug/dwarf/subprogram.tcc>
#include <unisim/util/debug/dwarf/machine_state.tcc>

#include <cstdlib>
#include <cstring>

#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
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

template <class MEMORY_ADDR>
DWARF_Handler<MEMORY_ADDR>::DWARF_Handler(const unisim::util::blob::Blob<MEMORY_ADDR> *_blob, const unisim::util::debug::SymbolTable<MEMORY_ADDR> *_symbol_table)
	: file_endianness(_blob->GetEndian())
	, arch_endianness(_blob->GetEndian())
	, arch_address_size(_blob->GetAddressSize())
	, inclusive_fde_addr_range(strcmp(_blob->GetArchitecture(), "68hc12") == 0)
	, debug_line_section(_blob->FindSection(".debug_line", false))
	, debug_info_section(_blob->FindSection(".debug_info", false))
	, debug_abbrev_section(_blob->FindSection(".debug_abbrev", false))
	, debug_aranges_section(_blob->FindSection(".debug_aranges", false))
	, debug_pubnames_section(_blob->FindSection(".debug_pubnames", false))
	, debug_pubtypes_section(_blob->FindSection(".debug_pubtypes", false))
	, debug_macinfo_section(_blob->FindSection(".debug_macinfo", false))
	, debug_frame_section(_blob->FindSection(".debug_frame", false))
	, eh_frame_section(_blob->FindSection(".eh_frame", false))
	, debug_str_section(_blob->FindSection(".debug_str", false))
	, debug_loc_section(_blob->FindSection(".debug_loc", false))
	, debug_ranges_section(_blob->FindSection(".debug_ranges", false))
	, symbol_table(_symbol_table)
	, blob(_blob)
	, subprogram_die_cache()
	, variable_die_cache()
	, debug_info_stream(&std::cout)
	, debug_warning_stream(&std::cerr)
	, debug_error_stream(&std::cerr)
	, reg_num_mapping_filename()
	, verbose(false)
	, debug(false)
	, auto_dw_mach_state()
	, c_loc_expr_parser()
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
	if(eh_frame_section) eh_frame_section->Catch();
	if(debug_str_section) debug_str_section->Catch();
	if(debug_loc_section) debug_loc_section->Catch();
	if(debug_ranges_section) debug_ranges_section->Catch();
	
	auto_dw_mach_state.Register(this);
}

template <class MEMORY_ADDR>
DWARF_Handler<MEMORY_ADDR>::~DWARF_Handler()
{
	unsigned int i;
	unsigned int j;
	
	for(typename CFIs::iterator dw_cfi_iter = dw_cfis.begin(); dw_cfi_iter != dw_cfis.end(); ++dw_cfi_iter)
	{
		const DWARF_CFI<MEMORY_ADDR> *dw_cfi = *dw_cfi_iter;
		delete dw_cfi;
	}

	typename std::multimap<MEMORY_ADDR, const Statement<MEMORY_ADDR> *>::const_iterator stmt_iter;

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

	for(j = FST_DEBUG_FRAME; j <= FST_EH_FRAME; j++)
	{
		unsigned int num_fdes = dw_fdes[j].size();
		for(i = 0; i < num_fdes; i++)
		{
			delete dw_fdes[j][i];
		}
	}

	for(j = FST_DEBUG_FRAME; j <= FST_EH_FRAME; j++)
	{
		typename std::map<uint64_t, DWARF_CIE<MEMORY_ADDR> *>::iterator dw_cie_iter;
		for(dw_cie_iter = dw_cies[j].begin(); dw_cie_iter != dw_cies[j].end(); dw_cie_iter++)
		{
			if((*dw_cie_iter).second)
			{
				delete (*dw_cie_iter).second;
			}
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
	if(eh_frame_section) eh_frame_section->Release();
	if(debug_str_section) debug_str_section->Release();
	if(debug_loc_section) debug_loc_section->Release();
	if(debug_ranges_section) debug_ranges_section->Release();
	
	if(blob) blob->Release();
}

template <class MEMORY_ADDR>
void DWARF_Handler<MEMORY_ADDR>::SetDebugInfoStream(std::ostream& _debug_info_stream)
{
	debug_info_stream = &_debug_info_stream;
	c_loc_expr_parser.SetDebugInfoStream(_debug_info_stream);
}

template <class MEMORY_ADDR>
void DWARF_Handler<MEMORY_ADDR>::SetDebugWarningStream(std::ostream& _debug_warning_stream)
{
	debug_warning_stream = &_debug_warning_stream;
	c_loc_expr_parser.SetDebugWarningStream(_debug_warning_stream);
}

template <class MEMORY_ADDR>
void DWARF_Handler<MEMORY_ADDR>::SetDebugErrorStream(std::ostream& _debug_error_stream)
{
	debug_error_stream = &_debug_error_stream;
	c_loc_expr_parser.SetDebugErrorStream(_debug_error_stream);
}

template <class MEMORY_ADDR>
void DWARF_Handler<MEMORY_ADDR>::SetRegistersInterface(unsigned int prc_num, unisim::service::interfaces::Registers *regs_if)
{
	auto_dw_mach_state.SetArchitecture(prc_num, blob->GetArchitecture());
	auto_dw_mach_state.SetRegistersInterface(prc_num, regs_if);
}

template <class MEMORY_ADDR>
void DWARF_Handler<MEMORY_ADDR>::SetMemoryInterface(unsigned int prc_num, unisim::service::interfaces::Memory<MEMORY_ADDR> *mem_if)
{
	auto_dw_mach_state.SetArchitecture(prc_num, blob->GetArchitecture());
	auto_dw_mach_state.SetMemoryInterface(prc_num, mem_if);
}

template <class MEMORY_ADDR>
bool DWARF_Handler<MEMORY_ADDR>::SetOption(Option opt, const char *s)
{
	switch(opt)
	{
		case OPT_REG_NUM_MAPPING_FILENAME:
			return auto_dw_mach_state.SetOption(opt, s);
		default:
			break;
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_Handler<MEMORY_ADDR>::SetOption(Option opt, bool flag)
{
	switch(opt)
	{
		case OPT_VERBOSE:
			if(auto_dw_mach_state.SetOption(opt, flag))
			{
				verbose = flag;
				return true;
			}
		case OPT_DEBUG:
			if(auto_dw_mach_state.SetOption(opt, flag))
			{
				debug = flag;
				c_loc_expr_parser.SetDebug(flag);
				return true;
			}
		default:
			break;
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_Handler<MEMORY_ADDR>::SetOption(Option opt, int value)
{
	switch(opt)
	{
		case OPT_MAX_STACK_FRAMES:
			return auto_dw_mach_state.SetOption(opt, value);
		default:
			break;
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_Handler<MEMORY_ADDR>::GetOption(Option opt, std::string& s) const
{
	switch(opt)
	{
		case OPT_REG_NUM_MAPPING_FILENAME:
			return auto_dw_mach_state.GetOption(opt, s);
		default:
			break;
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_Handler<MEMORY_ADDR>::GetOption(Option opt, bool& flag) const
{
	switch(opt)
	{
		case OPT_VERBOSE:
			flag = verbose;
			return true;
		case OPT_DEBUG:
			flag = debug;
			return true;
		default:
			break;
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_Handler<MEMORY_ADDR>::GetOption(Option opt, int& value) const
{
	switch(opt)
	{
		case OPT_MAX_STACK_FRAMES:
			return auto_dw_mach_state.GetOption(opt, value);
		default:
			break;
	}
	return false;
}

template <class MEMORY_ADDR>
const bool& DWARF_Handler<MEMORY_ADDR>::GetOptionFlag(Option opt) const
{
	switch(opt)
	{
		case OPT_VERBOSE:
			return verbose;
		case OPT_DEBUG:
			return debug;
		default:
			break;
	}
	static bool dummy_flag = false;
	return dummy_flag;
}

template <class MEMORY_ADDR>
const std::string& DWARF_Handler<MEMORY_ADDR>::GetOptionString(Option opt) const
{
	switch(opt)
	{
		case OPT_REG_NUM_MAPPING_FILENAME:
			return auto_dw_mach_state.GetOptionString(opt);
		default:
			break;
	}
	static std::string dummy_string;
	return dummy_string;
}

template <class MEMORY_ADDR>
const int& DWARF_Handler<MEMORY_ADDR>::GetOptionInt(Option opt) const
{
	switch(opt)
	{
		case OPT_MAX_STACK_FRAMES:
			return auto_dw_mach_state.GetOptionInt(opt);
		default:
			break;
	}
	static int dummy_int = 0;
	return dummy_int;
}

template <class MEMORY_ADDR>
bool DWARF_Handler<MEMORY_ADDR>::HasDebugInfo() const
{
	return !dw_cus.empty();
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
	unsigned int j;
	
	if(debug_abbrev_section && debug_abbrev_section->GetSize())
	{
		if(verbose)
		{
			GetDebugInfoStream() << "Parsing Section .debug_abbrev" << std::endl;
		}
		uint64_t debug_abbrev_offset = 0;
		do
		{
			DWARF_Abbrev *dw_abbrev = new DWARF_Abbrev(file_endianness);
			int64_t sz;
			if((sz = dw_abbrev->Load((const uint8_t *) debug_abbrev_section->GetData() + debug_abbrev_offset, debug_abbrev_section->GetSize() - debug_abbrev_offset, debug_abbrev_offset)) < 0)
			{
				GetDebugWarningStream() << "invalid DWARF v2/v3 abbreviation informations at offset 0x" << std::hex << debug_abbrev_offset << std::dec << std::endl;
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
		if(debug)
		{
			GetDebugInfoStream() << "no DWARF v2/v3 .debug_abbrev section found" << std::endl;
		}
	}

	if(debug_info_section && debug_info_section->GetSize())
	{
		if(verbose)
		{
			GetDebugInfoStream() << "Parsing Section .debug_info" << std::endl;
		}
		uint64_t debug_info_offset = 0;
		do
		{
			DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu = new DWARF_CompilationUnit<MEMORY_ADDR>(this);
			int64_t sz;
			if((sz = dw_cu->Load((const uint8_t *) debug_info_section->GetData() + debug_info_offset, debug_info_section->GetSize() - debug_info_offset, debug_info_offset)) < 0)
			{
				GetDebugWarningStream() << "invalid DWARF v2/v3 debug informations at offset 0x" << std::hex << debug_info_offset << std::dec << std::endl;
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
		if(debug)
		{
			GetDebugInfoStream() << "no DWARF v2/v3 .debug_info section found" << std::endl;
		}
	}

	if(debug_frame_section && debug_frame_section->GetSize())
	{
		if(verbose)
		{
			GetDebugInfoStream() << "Parsing Section .debug_frame" << std::endl;
		}
		uint64_t debug_frame_offset = 0;
		int64_t sz;
		do
		{
			DWARF_FDE<MEMORY_ADDR> *dw_fde = new DWARF_FDE<MEMORY_ADDR>(this, FST_DEBUG_FRAME);
			
			if((sz = dw_fde->Load((const uint8_t *) debug_frame_section->GetData() + debug_frame_offset, debug_frame_section->GetSize() - debug_frame_offset, debug_frame_section->GetAddr(), debug_frame_offset)) < 0)
			{
				delete dw_fde;
				DWARF_CIE<MEMORY_ADDR> *dw_cie = new DWARF_CIE<MEMORY_ADDR>(this, FST_DEBUG_FRAME);
			
				if((sz = dw_cie->Load((const uint8_t *) debug_frame_section->GetData() + debug_frame_offset, debug_frame_section->GetSize() - debug_frame_offset, debug_frame_offset)) < 0)
				{
					delete dw_cie;
					
					GetDebugWarningStream() << "invalid DWARF v2/v3 debug frame at offset 0x" << std::hex << debug_frame_offset << std::dec << std::endl;
					break;
				}
				
				//std::cerr << *dw_cie << std::endl;
				dw_cies[FST_DEBUG_FRAME].insert(std::pair<uint64_t, DWARF_CIE<MEMORY_ADDR> *>(debug_frame_offset, dw_cie));
			}
			else
			{
				//std::cerr << *dw_fde << std::endl;
				dw_fdes[FST_DEBUG_FRAME].push_back(dw_fde);
			}
			
			debug_frame_offset += sz;
		}
		while(debug_frame_offset < debug_frame_section->GetSize());
	}
	else
	{
		if(debug)
		{
			GetDebugInfoStream() << "no DWARF v2/v3 .debug_frame section found" << std::endl;
		}
	}

	if(eh_frame_section && eh_frame_section->GetSize())
	{
		if(verbose)
		{
			GetDebugInfoStream() << "Parsing Section .eh_frame" << std::endl;
		}
		uint64_t eh_frame_offset = 0;
		int64_t sz;
		do
		{
			DWARF_FDE<MEMORY_ADDR> *dw_fde = new DWARF_FDE<MEMORY_ADDR>(this, FST_EH_FRAME);
			
			if((sz = dw_fde->Load((const uint8_t *) eh_frame_section->GetData() + eh_frame_offset, eh_frame_section->GetSize() - eh_frame_offset, eh_frame_section->GetAddr(), eh_frame_offset)) < 0)
			{
				delete dw_fde;
				DWARF_CIE<MEMORY_ADDR> *dw_cie = new DWARF_CIE<MEMORY_ADDR>(this, FST_EH_FRAME);
			
				if((sz = dw_cie->Load((const uint8_t *) eh_frame_section->GetData() + eh_frame_offset, eh_frame_section->GetSize() - eh_frame_offset, eh_frame_offset)) < 0)
				{
					delete dw_cie;
					
					GetDebugWarningStream() << "invalid DWARF v2/v3 eh frame at offset 0x" << std::hex << eh_frame_offset << std::dec << std::endl;
					break;
				}
				
				if(dw_cie->IsTerminator())
				{
					delete dw_cie;
					break;
				}
				//std::cerr << *dw_cie << std::endl;
				dw_cies[FST_EH_FRAME].insert(std::pair<uint64_t, DWARF_CIE<MEMORY_ADDR> *>(eh_frame_offset, dw_cie));
			}
			else
			{
				if(dw_fde->IsTerminator())
				{
					delete dw_fde;
					break;
				}
				//std::cerr << *dw_fde << std::endl;
				dw_fdes[FST_EH_FRAME].push_back(dw_fde);
			}
			
			eh_frame_offset += sz;
		}
		while(eh_frame_offset < eh_frame_section->GetSize());
	}

	if(debug_aranges_section && debug_aranges_section->GetSize())
	{
		if(verbose)
		{
			GetDebugInfoStream() << "Parsing Section .debug_aranges" << std::endl;
		}
		uint64_t debug_aranges_offset = 0;
		do
		{
			DWARF_AddressRanges<MEMORY_ADDR> *dw_address_ranges = new DWARF_AddressRanges<MEMORY_ADDR>(this);
			int64_t sz;
			if((sz = dw_address_ranges->Load((const uint8_t *) debug_aranges_section->GetData() + debug_aranges_offset, debug_aranges_section->GetSize() - debug_aranges_offset)) < 0)
			{
				GetDebugWarningStream() << "invalid DWARF v2/v3 debug aranges at offset 0x" << std::hex << debug_aranges_offset << std::dec << std::endl;
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
		if(debug)
		{
			GetDebugInfoStream() << "no DWARF v2/v3 .debug_aranges section found" << std::endl;
		}
	}

	if(debug_pubnames_section && debug_pubnames_section->GetSize())
	{
		if(verbose)
		{
			GetDebugInfoStream() << "Parsing Section .debug_pubnames" << std::endl;
		}
		uint64_t debug_pubnames_offset = 0;
		do
		{
			DWARF_Pubs<MEMORY_ADDR> *dw_public_names = new DWARF_Pubs<MEMORY_ADDR>(this);
			int64_t sz;
			if((sz = dw_public_names->Load((const uint8_t *) debug_pubnames_section->GetData() + debug_pubnames_offset, debug_pubnames_section->GetSize() - debug_pubnames_offset)) < 0)
			{
				GetDebugWarningStream() << "invalid DWARF v2/v3 debug pubnames at offset 0x" << std::hex << debug_pubnames_offset << std::dec << std::endl;
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
		if(debug)
		{
			GetDebugInfoStream() << "no DWARF v2/v3 .debug_pubnames section found" << std::endl;
		}
	}

	if(debug_pubtypes_section && debug_pubtypes_section->GetSize())
	{
		if(verbose)
		{
			GetDebugInfoStream() << "Parsing Section .debug_pubtypes" << std::endl;
		}
		uint64_t debug_pubtypes_offset = 0;
		do
		{
			DWARF_Pubs<MEMORY_ADDR> *dw_public_types = new DWARF_Pubs<MEMORY_ADDR>(this);
			int64_t sz;
			if((sz = dw_public_types->Load((const uint8_t *) debug_pubtypes_section->GetData() + debug_pubtypes_offset, debug_pubtypes_section->GetSize() - debug_pubtypes_offset)) < 0)
			{
				GetDebugWarningStream() << "invalid DWARF v3 debug pubtypes at offset 0x" << std::hex << debug_pubtypes_offset << std::dec << std::endl;
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
	else
	{
		if(debug)
		{
			GetDebugInfoStream() << "no DWARF v3 .debug_pubtypes section found" << std::endl;
		}
	}

	if(!debug_ranges_section)
	{
		if(debug)
		{
			GetDebugInfoStream() << "no DWARF v3 .debug_ranges section found" << std::endl;
		}
	}
	
	// Fix all pointer cross-references
	if(verbose)
	{
		GetDebugInfoStream() << "Fixing DWARF cross-references" << std::endl;
	}
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
	for(j = FST_DEBUG_FRAME; j <= FST_EH_FRAME; j++)
	{
		for(dw_cie_iter = dw_cies[j].begin(); dw_cie_iter != dw_cies[j].end(); dw_cie_iter++, dw_cie_id++)
		{
			DWARF_CIE<MEMORY_ADDR> *dw_cie = (*dw_cie_iter).second;
			dw_cie->Fix(this, dw_cie_id);
		}
	}

	unsigned int dw_fde_id = 0;
	for(j = FST_DEBUG_FRAME; j <= FST_EH_FRAME; j++)
	{
		unsigned int num_fdes = dw_fdes[j].size();
		for(i = 0; i < num_fdes; i++, dw_fde_id++)
		{
			dw_fdes[j][i]->Fix(this, dw_fde_id);
		}
	}

	unsigned int num_aranges = dw_aranges.size();
	for(i = 0; i < num_aranges; i++)
	{
		dw_aranges[i]->Fix(this, i);
	}

	unsigned int num_pubnames = dw_pubnames.size();
	for(i = 0; i < num_pubnames; i++)
	{
		dw_pubnames[i]->Fix(this, i);
	}

	unsigned int num_pubtypes = dw_pubtypes.size();
	for(i = 0; i < num_pubtypes; i++)
	{
		dw_pubtypes[i]->Fix(this, i);
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
	//DumpStatementMatrix();
	
	auto_dw_mach_state.Initialize();
}

template <class MEMORY_ADDR>
void DWARF_Handler<MEMORY_ADDR>::to_XML(const char *output_filename)
{
	unsigned int i;
	unsigned int j;
	std::ofstream of(output_filename, std::ios::out);
	
	of << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
	of << "<DWARF>" << std::endl;
	of << "<DW_DEBUG_PUBNAMES>" << std::endl;
	unsigned int num_pubnames = dw_pubnames.size();
	for(i = 0; i < num_pubnames; i++)
	{
		dw_pubnames[i]->to_XML(of) << std::endl;
	}
	of << "</DW_DEBUG_PUBNAMES>" << std::endl;
	of << "<DW_DEBUG_PUBTYPES>" << std::endl;
	unsigned int num_pubtypes = dw_pubtypes.size();
	for(i = 0; i < num_pubtypes; i++)
	{
		dw_pubtypes[i]->to_XML(of) << std::endl;
	}
	of << "</DW_DEBUG_PUBTYPES>" << std::endl;
	of << "<DW_DEBUG_ARANGES>" << std::endl;
	unsigned int num_aranges = dw_aranges.size();
	for(i = 0; i < num_aranges; i++)
	{
		dw_aranges[i]->to_XML(of) << std::endl;
	}
	of << "</DW_DEBUG_ARANGES>" << std::endl;
	of << "<DW_DEBUG_ABBREV>" << std::endl;
	typename std::map<uint64_t, DWARF_Abbrev *>::iterator dw_abbrev_iter;
	for(dw_abbrev_iter = dw_abbrevs.begin(); dw_abbrev_iter != dw_abbrevs.end(); dw_abbrev_iter++)
	{
		DWARF_Abbrev *dw_abbrev = (*dw_abbrev_iter).second;
		
		dw_abbrev->to_XML(of) << std::endl;
	}
	of << "</DW_DEBUG_ABBREV>" << std::endl;
	of << "<DW_DEBUG_INFO>" << std::endl;
	typename std::map<uint64_t, DWARF_CompilationUnit<MEMORY_ADDR> *>::iterator dw_cu_iter;
	for(dw_cu_iter = dw_cus.begin(); dw_cu_iter != dw_cus.end(); dw_cu_iter++)
	{
		DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu = (*dw_cu_iter).second;
		dw_cu->to_XML(of) << std::endl;
	}
	of << "</DW_DEBUG_INFO>" << std::endl;
	of << "<DW_DEBUG_MACINFO>" << std::endl;
	typename std::map<uint64_t, DWARF_MacInfoListEntry<MEMORY_ADDR> *>::iterator dw_macinfo_list_entry_iter;
	for(dw_macinfo_list_entry_iter = dw_macinfo_list.begin(); dw_macinfo_list_entry_iter != dw_macinfo_list.end(); dw_macinfo_list_entry_iter++)
	{
		DWARF_MacInfoListEntry<MEMORY_ADDR> *dw_macinfo_list_entry = (*dw_macinfo_list_entry_iter).second;
		dw_macinfo_list_entry->to_XML(of) << std::endl;
	}
	of << "</DW_DEBUG_MACINFO>" << std::endl;
	of << "<DW_DEBUG_RANGES>" << std::endl;
	typename std::map<uint64_t, DWARF_RangeListEntry<MEMORY_ADDR> *>::iterator dw_range_list_entry_iter;
	for(dw_range_list_entry_iter = dw_range_list.begin(); dw_range_list_entry_iter != dw_range_list.end(); dw_range_list_entry_iter++)
	{
		DWARF_RangeListEntry<MEMORY_ADDR> *dw_range_list_entry = (*dw_range_list_entry_iter).second;
		dw_range_list_entry->to_XML(of) << std::endl;
	}
	of << "</DW_DEBUG_RANGES>" << std::endl;
	of << "<DW_DEBUG_LOC>" << std::endl;
	typename std::map<uint64_t, DWARF_LocListEntry<MEMORY_ADDR> *>::iterator dw_loc_list_entry_iter;
	for(dw_loc_list_entry_iter = dw_loc_list.begin(); dw_loc_list_entry_iter != dw_loc_list.end(); dw_loc_list_entry_iter++)
	{
		DWARF_LocListEntry<MEMORY_ADDR> *dw_loc_list_entry = (*dw_loc_list_entry_iter).second;
		dw_loc_list_entry->to_XML(of) << std::endl;
	}
	of << "</DW_DEBUG_LOC>" << std::endl;
	for(j = FST_DEBUG_FRAME; j <= FST_EH_FRAME; j++)
	{
		switch(j)
		{
			case FST_DEBUG_FRAME:
				of << "<DW_DEBUG_FRAME>" << std::endl;
				break;
			case FST_EH_FRAME:
				of << "<GNU_EH_FRAME>" << std::endl;
				break;
		}
		typename std::map<uint64_t, DWARF_CIE<MEMORY_ADDR> *>::iterator dw_cie_iter;
		for(dw_cie_iter = dw_cies[j].begin(); dw_cie_iter != dw_cies[j].end(); dw_cie_iter++)
		{
			DWARF_CIE<MEMORY_ADDR> *dw_cie = (*dw_cie_iter).second;
			
			dw_cie->to_XML(of) << std::endl;
		}

		unsigned int num_fdes = dw_fdes[j].size();
		for(i = 0; i < num_fdes; i++)
		{
			dw_fdes[j][i]->to_XML(of) << std::endl;
		}
		switch(j)
		{
			case FST_DEBUG_FRAME:
				of << "</DW_DEBUG_FRAME>" << std::endl;
				break;
			case FST_EH_FRAME:
				of << "</GNU_EH_FRAME>" << std::endl;
				break;
		}
	}
	of << "<DW_DEBUG_LINE>" << std::endl;
	typename std::map<uint64_t, DWARF_StatementProgram<MEMORY_ADDR> *>::iterator dw_stmt_prog_iter;
	for(dw_stmt_prog_iter = dw_stmt_progs.begin(); dw_stmt_prog_iter != dw_stmt_progs.end(); dw_stmt_prog_iter++)
	{
		DWARF_StatementProgram<MEMORY_ADDR> *dw_stmt_prog = (*dw_stmt_prog_iter).second;
		
		dw_stmt_prog->to_XML(of) << std::endl;
	}
	of << "</DW_DEBUG_LINE>" << std::endl;
	of << "</DWARF>" << std::endl;
}

template <class MEMORY_ADDR>
void DWARF_Handler<MEMORY_ADDR>::to_HTML(const char *output_dir)
{
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	mkdir(output_dir);
#else
	mkdir(output_dir, S_IRWXU);
#endif
	std::string index_filename(std::string(output_dir) + "/index.html");
	std::ofstream index(index_filename.c_str(), std::ios::out);
	
	index << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">" << std::endl;
	index << "<html>" << std::endl;
	index << "<head>" << std::endl;
	index << "<meta name=\"description\" content=\"DWARF v2+ navigation\">" << std::endl;
	index << "<meta name=\"keywords\" content=\"DWARF\">" << std::endl;
	index << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
	index << "<title>DWARF v2+ navigation</title>" << std::endl;
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
	index << "<h1><a href=\"http://dwarfstd.org\"/>DWARF</a> v2+ navigation</h1>" << std::endl;
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
	index << "<td><a href=\"debug_frame/fdes/0.html\">.debug_frame</a>&nbsp;<a href=\"eh_frame/fdes/0.html\">.eh_frame</a></td>" << std::endl;
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
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
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
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
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
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
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
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
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
		debug_aranges << "<th>Segment selector size</th>" << std::endl;
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
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
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
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
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
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
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
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
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
	unsigned int j;
	for(j = FST_DEBUG_FRAME; j <= FST_EH_FRAME; j++)
	{
		std::stringstream debug_frame_ouput_dir_sstr;
		debug_frame_ouput_dir_sstr << output_dir;
		switch(j)
		{
			case FST_DEBUG_FRAME:
				debug_frame_ouput_dir_sstr << "/debug_frame";
				break;
			case FST_EH_FRAME:
				debug_frame_ouput_dir_sstr << "/eh_frame";
				break;
		}
		std::string debug_frame_output_dir(debug_frame_ouput_dir_sstr.str());
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		mkdir(debug_frame_output_dir.c_str());
#else
		mkdir(debug_frame_output_dir.c_str(), S_IRWXU);
#endif

		std::stringstream debug_frame_cies_ouput_dir_sstr;
		debug_frame_cies_ouput_dir_sstr << debug_frame_output_dir << "/cies";
		std::string debug_frame_cies_output_dir(debug_frame_cies_ouput_dir_sstr.str());
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		mkdir(debug_frame_cies_output_dir.c_str());
#else
		mkdir(debug_frame_cies_output_dir.c_str(), S_IRWXU);
#endif

		std::stringstream debug_frame_fdes_ouput_dir_sstr;
		debug_frame_fdes_ouput_dir_sstr << debug_frame_output_dir << "/fdes";
		std::string debug_frame_fdes_output_dir(debug_frame_fdes_ouput_dir_sstr.str());
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
		mkdir(debug_frame_fdes_output_dir.c_str());
#else
		mkdir(debug_frame_fdes_output_dir.c_str(), S_IRWXU);
#endif

		unsigned int debug_frame_cies_filename_idx = 0;
		unsigned int cies_per_file = 2048;
		unsigned int num_debug_cies = dw_cies[j].size();
		unsigned int num_debug_frame_cies_filenames = ((num_debug_cies + cies_per_file - 1) /cies_per_file);
		typename std::map<uint64_t, DWARF_CIE<MEMORY_ADDR> *>::iterator dw_cie_iter = dw_cies[j].begin();
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
			debug_frame_cies << "<h1>Call Frame Information/Common Information Entries (CIE) (";
			switch(j)
			{
				case FST_DEBUG_FRAME:
					debug_frame_cies << ".debug_frame";
					break;
				case FST_EH_FRAME:
					debug_frame_cies << ".eh_frame";
					break;
			}
			debug_frame_cies << ")</h1>" << std::endl;
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
			if(j == FST_EH_FRAME)
			{
				debug_frame_cies << "<th>EH Data (Optional)</th>" << std::endl;
			}
			debug_frame_cies << "<th>Address Size</th>" << std::endl;
			debug_frame_cies << "<th>Segment selector Size</th>" << std::endl;
			debug_frame_cies << "<th>Code alignment factor</th>" << std::endl;
			debug_frame_cies << "<th>Data alignment factor</th>" << std::endl;
			debug_frame_cies << "<th>Return address register</th>" << std::endl;
			if(j == FST_EH_FRAME)
			{
				debug_frame_cies << "<th>Augmentation Data (Optional)</th>" << std::endl;
			}
			debug_frame_cies << "<th>Initial call frame program</th>" << std::endl;
			debug_frame_cies << "</tr>" << std::endl;
			unsigned int count = 0;
			while(count < cies_per_file && dw_cie_iter != dw_cies[j].end())
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
		while(dw_cie_iter != dw_cies[j].end());
		
		unsigned int debug_frame_fdes_filename_idx = 0;
		unsigned int fdes_per_file = 2048;
		unsigned int num_debug_fdes = dw_fdes[j].size();
		unsigned int num_debug_frame_fdes_filenames = ((num_debug_fdes + fdes_per_file - 1) /fdes_per_file);
		typename std::vector<DWARF_FDE<MEMORY_ADDR> *>::iterator dw_fde_iter = dw_fdes[j].begin();
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
			debug_frame_fdes << "<h1>Call Frame Information/Frame Description Entries (FDE) (";
			switch(j)
			{
				case FST_DEBUG_FRAME:
					debug_frame_fdes << ".debug_frame";
					break;
				case FST_EH_FRAME:
					debug_frame_fdes << ".eh_frame";
					break;
			}
			debug_frame_fdes << ")</h1>" << std::endl;
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
			if(j == FST_EH_FRAME)
			{
				debug_frame_fdes << "<th>Augmentation Data (Optional)</th>" << std::endl;
			}
			debug_frame_fdes << "<th>Call frame program/Computed call frame information</th>" << std::endl;
			debug_frame_fdes << "</tr>" << std::endl;
			unsigned int count = 0;
			while(count < fdes_per_file && dw_fde_iter != dw_fdes[j].end())
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
		while(dw_fde_iter != dw_fdes[j].end());
	}
	// Debugging Information
	std::stringstream debug_info_ouput_dir_sstr;
	debug_info_ouput_dir_sstr << output_dir << "/debug_info";
	std::string debug_info_output_dir(debug_info_ouput_dir_sstr.str());
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	mkdir(debug_info_output_dir.c_str());
#else
	mkdir(debug_info_output_dir.c_str(), S_IRWXU);
#endif

	std::stringstream debug_info_cus_ouput_dir_sstr;
	debug_info_cus_ouput_dir_sstr << debug_info_output_dir << "/cus";
	std::string debug_info_cus_output_dir(debug_info_cus_ouput_dir_sstr.str());
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
	mkdir(debug_info_cus_output_dir.c_str());
#else
	mkdir(debug_info_cus_output_dir.c_str(), S_IRWXU);
#endif

	std::stringstream debug_info_dies_ouput_dir_sstr;
	debug_info_dies_ouput_dir_sstr << debug_info_output_dir << "/dies";
	std::string debug_info_dies_output_dir(debug_info_dies_ouput_dir_sstr.str());
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
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
		debug_info_cus << "<th>Name</th>" << std::endl;
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
void DWARF_Handler<MEMORY_ADDR>::Register(DWARF_CIE<MEMORY_ADDR> *dw_cie)
{
	dw_cies[dw_cie->GetFrameSectionType()].insert(std::pair<uint64_t, DWARF_CIE<MEMORY_ADDR> *>(dw_cie->GetOffset(), dw_cie));
}

template <class MEMORY_ADDR>
void DWARF_Handler<MEMORY_ADDR>::UnRegister(DWARF_DIE<MEMORY_ADDR> *dw_die)
{
	typename std::map<uint64_t, DWARF_DIE<MEMORY_ADDR> *>::iterator dw_die_iter = dw_dies.find(dw_die->GetOffset());
	if(dw_die_iter != dw_dies.end())
	{
		dw_dies.erase(dw_die_iter);
	}
}

template <class MEMORY_ADDR>
void DWARF_Handler<MEMORY_ADDR>::DumpStatementMatrix()
{
	typename std::multimap<MEMORY_ADDR, const Statement<MEMORY_ADDR> *>::const_iterator stmt_iter;

	for(stmt_iter = stmt_matrix.begin(); stmt_iter != stmt_matrix.end(); stmt_iter++)
	{
		if((*stmt_iter).second)
		{
			std::cout << *(*stmt_iter).second << std::endl;
		}
	}
}

template <class MEMORY_ADDR>
void DWARF_Handler<MEMORY_ADDR>::ScanStatements(unisim::service::interfaces::StatementScanner<MEMORY_ADDR>& scanner) const
{
	typename std::multimap<MEMORY_ADDR, const Statement<MEMORY_ADDR> *>::const_iterator stmt_iter;

	for(stmt_iter = stmt_matrix.begin(); stmt_iter != stmt_matrix.end(); stmt_iter++)
	{
		if((*stmt_iter).second)
		{
			scanner.Append((*stmt_iter).second);
		}
	}
}

template <class MEMORY_ADDR>
const unisim::util::debug::Statement<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindStatements(unisim::service::interfaces::StatementScanner<MEMORY_ADDR> *scanner, MEMORY_ADDR addr, typename unisim::service::interfaces::StatementLookup<MEMORY_ADDR>::Scope scope) const
{
	if(stmt_matrix.empty()) return 0;

	const unisim::util::debug::Statement<MEMORY_ADDR> *ret = 0;
	MEMORY_ADDR func_start_addr = addr;
	MEMORY_ADDR func_end_addr = addr;
	
	if(((scope == unisim::service::interfaces::StatementLookup<MEMORY_ADDR>::SCOPE_NEAREST_LOWER_OR_EQUAL_STMT_WITHIN_FUNCTION) ||
	    (scope == unisim::service::interfaces::StatementLookup<MEMORY_ADDR>::SCOPE_NEXT_STMT_WITHIN_FUNCTION)) &&
	   symbol_table)
	{
		const unisim::util::debug::Symbol<MEMORY_ADDR> *symbol = symbol_table->FindSymbolByAddr(addr);
		if(symbol)
		{
			MEMORY_ADDR symbol_addr = symbol->GetAddress();
			MEMORY_ADDR symbol_size = symbol->GetSize();
			typename unisim::util::debug::SymbolBase::Type symbol_type = symbol->GetType();
			if((symbol_size != 0) && (symbol_type == unisim::util::debug::SymbolBase::SYM_FUNC))
			{
				func_start_addr = symbol_addr;
				func_end_addr = symbol_addr + symbol_size - 1;
			}
		}
	}
	
	typename std::multimap<MEMORY_ADDR, const Statement<MEMORY_ADDR> *>::const_iterator stmt_iter;
	switch(scope)
	{
		case unisim::service::interfaces::StatementLookup<MEMORY_ADDR>::SCOPE_NEAREST_LOWER_OR_EQUAL_STMT:
		case unisim::service::interfaces::StatementLookup<MEMORY_ADDR>::SCOPE_NEAREST_LOWER_OR_EQUAL_STMT_WITHIN_FUNCTION:
		{
			// check whether addr is not below
			stmt_iter = stmt_matrix.begin();
			if(stmt_iter != stmt_matrix.end())
			{
				if((*stmt_iter).first > addr) break;
			}
			
			stmt_iter = stmt_matrix.lower_bound(addr);
			if(stmt_iter == stmt_matrix.end()) break; // check whether addr is above
			if((*stmt_iter).first > addr)
			{
				stmt_iter--;
			}
			
			addr = (*stmt_iter).first; // addr is valid and lower or equal to the requested address
			std::pair<typename std::multimap<MEMORY_ADDR, const Statement<MEMORY_ADDR> *>::const_iterator, typename std::multimap<MEMORY_ADDR, const Statement<MEMORY_ADDR> *>::const_iterator> range = stmt_matrix.equal_range(addr);
			for(stmt_iter = range.first; stmt_iter != range.second; stmt_iter++)
			{
				const unisim::util::debug::Statement<MEMORY_ADDR> *stmt = (*stmt_iter).second;
				if((scope == unisim::service::interfaces::StatementLookup<MEMORY_ADDR>::SCOPE_NEAREST_LOWER_OR_EQUAL_STMT) || ((stmt->GetAddress() >= func_start_addr) && (stmt->GetAddress() <= func_end_addr)))
				{
					if(!scanner) return stmt;
					if(!ret) ret = stmt;
					scanner->Append(stmt);
				}
			}
			break;
		}
		
		case unisim::service::interfaces::StatementLookup<MEMORY_ADDR>::SCOPE_EXACT_STMT:
		{
			std::pair<typename std::multimap<MEMORY_ADDR, const Statement<MEMORY_ADDR> *>::const_iterator, typename std::multimap<MEMORY_ADDR, const Statement<MEMORY_ADDR> *>::const_iterator> range = stmt_matrix.equal_range(addr);
			for(stmt_iter = range.first; stmt_iter != range.second; stmt_iter++)
			{
				const unisim::util::debug::Statement<MEMORY_ADDR> *stmt = (*stmt_iter).second;
				if(!scanner) return stmt;
				if(!ret) ret = stmt;
				scanner->Append(stmt);
			}
			break;
		}
		
		case unisim::service::interfaces::StatementLookup<MEMORY_ADDR>::SCOPE_NEXT_STMT:
		case unisim::service::interfaces::StatementLookup<MEMORY_ADDR>::SCOPE_NEXT_STMT_WITHIN_FUNCTION:
		{
			stmt_iter = stmt_matrix.upper_bound(addr);
			if(stmt_iter == stmt_matrix.end()) break;
			
			addr = (*stmt_iter).first;
			std::pair<typename std::multimap<MEMORY_ADDR, const Statement<MEMORY_ADDR> *>::const_iterator, typename std::multimap<MEMORY_ADDR, const Statement<MEMORY_ADDR> *>::const_iterator> range = stmt_matrix.equal_range(addr);
			for(stmt_iter = range.first; stmt_iter != range.second; stmt_iter++)
			{
				const unisim::util::debug::Statement<MEMORY_ADDR> *stmt = (*stmt_iter).second;
				if((scope != unisim::service::interfaces::StatementLookup<MEMORY_ADDR>::SCOPE_NEXT_STMT_WITHIN_FUNCTION) || ((stmt->GetAddress() >= func_start_addr) && (stmt->GetAddress() <= func_end_addr)))
				{
					if(!scanner) return stmt;
					if(!ret) ret = stmt;
					scanner->Append(stmt);
				}
			}
			break;
		}
	}
	
	return ret;
}

template <class MEMORY_ADDR>
const unisim::util::debug::Statement<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindStatements(unisim::service::interfaces::StatementScanner<MEMORY_ADDR>& scanner, MEMORY_ADDR addr, typename unisim::service::interfaces::StatementLookup<MEMORY_ADDR>::Scope scope) const
{
	return FindStatements(&scanner, addr, scope);
}

template <class MEMORY_ADDR>
const unisim::util::debug::Statement<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindStatement(MEMORY_ADDR addr, typename unisim::service::interfaces::StatementLookup<MEMORY_ADDR>::Scope scope) const
{
	return FindStatements((unisim::service::interfaces::StatementScanner<MEMORY_ADDR> *) 0, addr, scope);
}

template <class MEMORY_ADDR>
const unisim::util::debug::Statement<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindStatements(unisim::service::interfaces::StatementScanner<MEMORY_ADDR> *scanner, const unisim::util::debug::SourceCodeLocation& source_code_location) const
{
	const std::string& filename = source_code_location.GetSourceCodeFilename();
	unsigned int lineno = source_code_location.GetLineNo();
	unsigned int colno = source_code_location.GetColNo();
	const unisim::util::debug::Statement<MEMORY_ADDR> *ret = 0;
	bool requested_filename_is_absolute = IsAbsolutePath(filename.c_str());
	std::vector<std::string> hierarchical_requested_filename;
	
	std::string s;
	const char *p = filename.c_str();
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

	typename std::multimap<MEMORY_ADDR, const Statement<MEMORY_ADDR> *>::const_iterator stmt_iter;
	
	for(stmt_iter = stmt_matrix.begin(); stmt_iter != stmt_matrix.end(); stmt_iter++)
	{
		const Statement<MEMORY_ADDR> *stmt = (*stmt_iter).second;
		
		if(stmt)
		{
			if(stmt->IsBeginningOfSourceStatement())
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
							
							if(match)
							{
								if(!scanner) return stmt;
								if(!ret) ret = stmt;
								scanner->Append(stmt);
							}
						}
					}
				}
			}
		}
	}
	return ret;
}

template <class MEMORY_ADDR>
const unisim::util::debug::Statement<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindStatement(const unisim::util::debug::SourceCodeLocation& source_code_location) const
{
	return FindStatements((unisim::service::interfaces::StatementScanner<MEMORY_ADDR> *) 0, source_code_location);
}

template <class MEMORY_ADDR>
const unisim::util::debug::Statement<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindStatements(unisim::service::interfaces::StatementScanner<MEMORY_ADDR>& scanner, const unisim::util::debug::SourceCodeLocation& source_code_location) const
{
	return FindStatements(&scanner, source_code_location);
}

template <class MEMORY_ADDR>
bool DWARF_Handler<MEMORY_ADDR>::IsAbsolutePath(const char *filename) const
{
	// filename starts '/' or 'drive letter':\ or 'driver letter':/
	return (((filename[0] >= 'a' && filename[0] <= 'z') || (filename[0] >= 'A' && filename[0] <= 'Z')) && (filename[1] == ':') && ((filename[2] == '\\') || (filename[2] == '/'))) || (*filename == '/');
}

template <class MEMORY_ADDR>
endian_type DWARF_Handler<MEMORY_ADDR>::GetFileEndianness() const
{
	return file_endianness;
}

template <class MEMORY_ADDR>
endian_type DWARF_Handler<MEMORY_ADDR>::GetArchEndianness() const
{
	return arch_endianness;
}

template <class MEMORY_ADDR>
uint8_t DWARF_Handler<MEMORY_ADDR>::GetFileAddressSize() const
{
	switch(blob->GetFileFormat())
	{
		case unisim::util::blob::FFMT_ELF32: return 4;
		case unisim::util::blob::FFMT_ELF64: return 8;
		default: return 0;
	}
	return 0;
}

template <class MEMORY_ADDR>
uint8_t DWARF_Handler<MEMORY_ADDR>::GetArchAddressSize() const
{
	return arch_address_size;
}

template <class MEMORY_ADDR>
bool DWARF_Handler<MEMORY_ADDR>::HasFilename() const
{
	return (blob->GetCapability() & unisim::util::blob::CAP_FILENAME) != 0;
}

template <class MEMORY_ADDR>
const char *DWARF_Handler<MEMORY_ADDR>::GetFilename() const
{
	return HasFilename() ? blob->GetFilename() : "<anonymous>";
}

template <class MEMORY_ADDR>
const char *DWARF_Handler<MEMORY_ADDR>::GetArchitecture() const
{
	return blob->GetArchitecture();
}

template <class MEMORY_ADDR>
unisim::service::interfaces::Memory<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::GetMemoryInterface(unsigned int prc_num) const
{
	return auto_dw_mach_state.GetMemoryInterface(prc_num);
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
		GetDebugWarningStream() << "Invalid DWARF v2/v3 statement program prologue at offset 0x" << std::hex << debug_line_offset << std::dec << std::endl;
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
			GetDebugWarningStream() << "Invalid DWARF v3 debug ranges at offset 0x" << std::hex << debug_ranges_offset << std::dec << std::endl;
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
				GetDebugWarningStream() << "Invalid DWARF v2/v3 debug macinfo at offset 0x" << std::hex << debug_macinfo_offset << std::dec << " (unknown type " << ((unsigned int) dw_mac_info_type) << ")" << std::endl;
				return 0;
		}
			
		if(!dw_macinfo_list_entry) return head;

		int64_t sz;
			
		if((sz = dw_macinfo_list_entry->Load((const uint8_t *) debug_macinfo_section->GetData() + debug_macinfo_offset, debug_macinfo_section->GetSize() - debug_macinfo_offset, debug_macinfo_offset)) < 0)
		{
			GetDebugWarningStream() << "Invalid DWARF v2/v3 debug macinfo at offset 0x" << std::hex << debug_macinfo_offset << std::dec << " (type " << ((unsigned int) dw_mac_info_type) << ")" << std::endl;
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
			GetDebugWarningStream() << "Invalid DWARF v2/v3 debug loc at offset 0x" << std::hex << debug_loc_offset << std::dec << std::endl;
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
const DWARF_CIE<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindCIE(uint64_t cie_offset, DWARF_FrameSectionType dw_fst)
{
	switch(dw_fst)
	{
		case FST_DEBUG_FRAME:
			if(!debug_frame_section || (cie_offset >= debug_frame_section->GetSize())) return 0;
			break;
		case FST_EH_FRAME:
			if(!eh_frame_section || (cie_offset >= eh_frame_section->GetSize())) return 0;
			break;
	}
	
	typename std::map<uint64_t, DWARF_CIE<MEMORY_ADDR> *>::const_iterator dw_cie_iter = dw_cies[dw_fst].find(cie_offset);
	
	if(dw_cie_iter != dw_cies[dw_fst].end())
	{
		return (*dw_cie_iter).second;
	}
	
	DWARF_CIE<MEMORY_ADDR> *dw_cie = new DWARF_CIE<MEMORY_ADDR>(this, dw_fst);
	int64_t sz;

	switch(dw_fst)
	{
		case FST_DEBUG_FRAME:
			if((sz = dw_cie->Load((const uint8_t *) debug_frame_section->GetData() + cie_offset, debug_frame_section->GetSize() - cie_offset, cie_offset)) < 0)
			{
				delete dw_cie;
				
				GetDebugWarningStream() << "invalid DWARF v2/v3 debug frame at offset 0x" << std::hex << cie_offset << std::dec << std::endl;
				return 0;
			}
			break;
		case FST_EH_FRAME:
			if((sz = dw_cie->Load((const uint8_t *) eh_frame_section->GetData() + cie_offset, eh_frame_section->GetSize() - cie_offset, cie_offset)) < 0)
			{
				delete dw_cie;
				
				GetDebugWarningStream() << "invalid DWARF v2/v3 eh frame at offset 0x" << std::hex << cie_offset << std::dec << std::endl;
				return 0;
			}
			
			if(dw_cie->IsTerminator())
			{
				delete dw_cie;
				return 0;
			}
			break;
	}
	
	Register(dw_cie);
	//std::cerr << *dw_cie << std::endl;
	return dw_cie;
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

template <class MEMORY_ADDR>
const DWARF_FDE<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindFDEByAddr(MEMORY_ADDR pc) const
{
	typename FDECache::const_iterator it = fde_cache.find(pc);
	if(it != fde_cache.end())
	{
		const DWARF_FDE<MEMORY_ADDR> *dw_fde = (*it).second;
		return dw_fde;
	}
	
	unsigned int i;
	unsigned int j;
	for(j = FST_DEBUG_FRAME; j <= FST_EH_FRAME; j++)
	{
		unsigned int num_fdes = dw_fdes[j].size();
		for(i = 0; i < num_fdes; i++)
		{
			DWARF_FDE<MEMORY_ADDR> *dw_fde = dw_fdes[j][i];
			MEMORY_ADDR initial_location = dw_fde->GetInitialLocation();
			MEMORY_ADDR address_range = dw_fde->GetAddressRange();
			
			if(pc >= initial_location)
			{
				MEMORY_ADDR pc_offset = pc - initial_location;
				if((inclusive_fde_addr_range && (pc_offset <= address_range)) || (!inclusive_fde_addr_range && (pc_offset < address_range)))
				{
					// found FDE
					for(MEMORY_ADDR offset = 0; (inclusive_fde_addr_range && offset <= address_range) || (!inclusive_fde_addr_range && (offset < address_range)); ++offset)
					{
						fde_cache[initial_location + offset] = dw_fde;
					}
					return dw_fde;
				}
			}
		}
	}
	
	fde_cache[pc] = 0;
	return 0;
}

template <class MEMORY_ADDR>
const DWARF_CFI<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindCFIByAddr(MEMORY_ADDR pc) const
{
	const DWARF_CFI<MEMORY_ADDR> *dw_cfi = 0;
	typename CFICache::const_iterator it = cfi_cache.find(pc);
	if(it != cfi_cache.end())
	{
		dw_cfi = (*it).second;
	}
	else
	{
		if(debug)
		{
			GetDebugInfoStream() << "Searching FDE for PC=0x" << std::hex << pc << std::dec << std::endl;
		}
		
		const DWARF_FDE<MEMORY_ADDR> *dw_fde = FindFDEByAddr(pc);
		
		if(dw_fde)
		{
			if(debug)
			{
				GetDebugInfoStream() << "Found FDE for PC=0x" << std::hex << pc << std::dec << std::endl;
			}
			
			DWARF_CallFrameVM<MEMORY_ADDR> dw_call_frame_vm = DWARF_CallFrameVM<MEMORY_ADDR>(this);
			dw_cfi = dw_call_frame_vm.ComputeCFI(dw_fde);
			if(dw_cfi)
			{
				dw_cfis.push_back(dw_cfi);
				MEMORY_ADDR initial_location = dw_fde->GetInitialLocation();
				MEMORY_ADDR address_range = dw_fde->GetAddressRange();
				for(MEMORY_ADDR offset = 0; (inclusive_fde_addr_range && offset <= address_range) || (!inclusive_fde_addr_range && (offset < address_range)); ++offset)
				{
					cfi_cache[initial_location + offset] = dw_cfi;
				}
			}
			else
			{
				GetDebugWarningStream() << "something goes wrong while interpreting call frame information" << std::endl;
			}
		}
		else
		{
			if(debug)
			{
				GetDebugInfoStream() << "No FDE found for PC=0x" << std::hex << pc << std::dec << std::endl;
			}
			cfi_cache[pc] = 0;
		}
	}

	return dw_cfi;
}

template <class MEMORY_ADDR>
const DWARF_Pub<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindPubName(const char *name) const
{
	unsigned int num_pubnames = dw_pubnames.size();
	unsigned int i;
	for(i = 0; i < num_pubnames; i++)
	{
		const DWARF_Pub<MEMORY_ADDR> *dw_pub = dw_pubnames[i]->FindPub(name);
		if(dw_pub) return dw_pub;
	}
	return 0;
}

template <class MEMORY_ADDR>
const DWARF_Pub<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindPubType(const char *name) const
{
	unsigned int num_pubnames = dw_pubtypes.size();
	unsigned int i;
	for(i = 0; i < num_pubnames; i++)
	{
		const DWARF_Pub<MEMORY_ADDR> *dw_pub = dw_pubtypes[i]->FindPub(name);
		if(dw_pub) return dw_pub;
	}
	return 0;
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindDIEByPubName(const char *name) const
{
	const DWARF_Pub<MEMORY_ADDR> *dw_pub = FindPubName(name);
	return dw_pub ? dw_pub->GetDIE() : 0;
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindDIEByPubType(const char *name) const
{
	const DWARF_Pub<MEMORY_ADDR> *dw_pub = FindPubType(name);
	return dw_pub ? dw_pub->GetDIE() : 0;
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindDIEByName(unsigned int dw_tag, const char *name, bool external) const
{
	typename std::map<uint64_t, DWARF_CompilationUnit<MEMORY_ADDR> *>::const_iterator dw_cu_iter;
	
	for(dw_cu_iter = dw_cus.begin(); dw_cu_iter != dw_cus.end(); dw_cu_iter++)
	{
		DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu = (*dw_cu_iter).second;

		const DWARF_DIE<MEMORY_ADDR> *dw_found_die = dw_cu->FindDIEByName(dw_tag, name, external);
		if(dw_found_die) return dw_found_die;
	}
	
	return 0;
}

template <class MEMORY_ADDR>
const DWARF_CompilationUnit<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindCompilationUnitByAddrRange(MEMORY_ADDR addr, MEMORY_ADDR length) const
{
	unsigned int num_aranges = dw_aranges.size();
	unsigned int i;
	for(i = 0; i < num_aranges; i++)
	{
		DWARF_AddressRanges<MEMORY_ADDR> *dw_address_ranges = dw_aranges[i];
		if(dw_address_ranges->HasOverlap(addr, length))
		{
			const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu = dw_address_ranges->GetCompilationUnit();
			if(debug)
			{
				GetDebugInfoStream() << "found CU #" << dw_cu->GetId() << " for address range 0x" << std::hex << addr << "-0x" << (addr + length) << std::dec << std::endl;
			}
			return dw_cu;
		}
	}
	return 0;
}

template <class MEMORY_ADDR>
const DWARF_CompilationUnit<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindCompilationUnitByName(const char *name) const
{
	typename std::map<uint64_t, DWARF_CompilationUnit<MEMORY_ADDR> *>::const_iterator dw_cu_iter;
	
	for(dw_cu_iter = dw_cus.begin(); dw_cu_iter != dw_cus.end(); dw_cu_iter++)
	{
		DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu = (*dw_cu_iter).second;

		const char *dw_cu_name = dw_cu->GetName();
		
		if(dw_cu_name)
		{
			if(strcmp(dw_cu_name, name) == 0) return dw_cu;
		}
	}

	return 0;
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindSubProgramDIEByAddrRange(MEMORY_ADDR addr, MEMORY_ADDR length) const
{
	const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu = FindCompilationUnitByAddrRange(addr, length);
	
	if(!dw_cu) return 0;
	
	return dw_cu->FindDIEByAddrRange(DW_TAG_subprogram, addr, length);
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindSubProgramDIE(MEMORY_ADDR pc) const
{
	return FindSubProgramDIEByAddrRange(pc, 1);
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindSubProgramDIE(const char *subprogram_name, const char *compilation_unit_name) const
{
	if(compilation_unit_name)
	{
		const DWARF_CompilationUnit<MEMORY_ADDR> *dw_found_cu = FindCompilationUnitByName(compilation_unit_name);
		
		return dw_found_cu ? dw_found_cu->FindSubProgramDIE(subprogram_name) : 0;
	}
	else
	{
		typename DIECache::const_iterator it = subprogram_die_cache.find(subprogram_name);
		if(it != subprogram_die_cache.end())
		{
			const DWARF_DIE<MEMORY_ADDR> *dw_die_subprogram = (*it).second;
			return dw_die_subprogram;
		}
		
		const DWARF_DIE<MEMORY_ADDR> *dw_die_subprogram = FindDIEByPubName(subprogram_name);
		
		if(dw_die_subprogram && dw_die_subprogram->HasRanges())
		{
			subprogram_die_cache[subprogram_name] = dw_die_subprogram;
			return dw_die_subprogram;
		}
		
		typename std::map<uint64_t, DWARF_CompilationUnit<MEMORY_ADDR> *>::const_iterator dw_cu_iter;
		
		for(dw_cu_iter = dw_cus.begin(); dw_cu_iter != dw_cus.end(); dw_cu_iter++)
		{
			DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu = (*dw_cu_iter).second;

			const DWARF_DIE<MEMORY_ADDR> *dw_die_subprogram = dw_cu->FindSubProgramDIE(subprogram_name);
			
			if(dw_die_subprogram)
			{
				subprogram_die_cache[subprogram_name] = dw_die_subprogram;
				return dw_die_subprogram;
			}
		}
		
		subprogram_die_cache[subprogram_name] = 0;
	}

	return 0;
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindVariableDIE(const char *variable_name, const char *compilation_unit_name) const
{
	if(compilation_unit_name)
	{
		const DWARF_CompilationUnit<MEMORY_ADDR> *dw_found_cu = FindCompilationUnitByName(compilation_unit_name);
		
		return dw_found_cu ? dw_found_cu->FindVariableDIE(variable_name) : 0;
	}
	else
	{
		typename DIECache::const_iterator it = variable_die_cache.find(variable_name);
		if(it != variable_die_cache.end())
		{
			const DWARF_DIE<MEMORY_ADDR> *dw_die_variable = (*it).second;
			return dw_die_variable;
		}
		
		typename std::map<uint64_t, DWARF_CompilationUnit<MEMORY_ADDR> *>::const_iterator dw_cu_iter;
		
		for(dw_cu_iter = dw_cus.begin(); dw_cu_iter != dw_cus.end(); dw_cu_iter++)
		{
			DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu = (*dw_cu_iter).second;

			const DWARF_DIE<MEMORY_ADDR> *dw_die_variable = dw_cu->FindVariableDIE(variable_name);
			
			if(dw_die_variable)
			{
				variable_die_cache[variable_name] = dw_die_variable;
				return dw_die_variable;
			}
		}
		
		variable_die_cache[variable_name] = 0;
	}

	return 0;
}

template <class MEMORY_ADDR>
const DWARF_SubProgram<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindSubProgram(const char *subprogram_name, const char *filename, const char *compilation_unit_name) const
{
	const char *blob_filename = blob->GetFilename();
	
	if(filename)
	{
		if(!blob_filename) return 0;
		if(strcmp(blob_filename, filename) != 0) return 0;
	}
	
	const DWARF_DIE<MEMORY_ADDR> *dw_die_subprogram = FindSubProgramDIE(subprogram_name, compilation_unit_name);
	
	return dw_die_subprogram ? dw_die_subprogram->GetSubProgram() : 0;
}

template <class MEMORY_ADDR>
const DWARF_SubProgram<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindSubProgram(MEMORY_ADDR pc, const char *filename) const
{
	const char *blob_filename = blob->GetFilename();
	
	if(filename)
	{
		if(!blob_filename) return 0;
		if(strcmp(blob_filename, filename) != 0) return 0;
	}
	
	const DWARF_DIE<MEMORY_ADDR> *dw_die_subprogram = FindSubProgramDIE(pc);
	
	return dw_die_subprogram ? dw_die_subprogram->GetSubProgram() : 0;
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindDataObjectDIE(const char *name, MEMORY_ADDR pc) const
{
	const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu = FindCompilationUnitByAddrRange(pc, 1);
	
	if(!dw_cu)
	{
		if(debug)
		{
			GetDebugInfoStream() << "compilation unit for PC=0x" << std::hex << pc << std::dec << " not found" << std::endl;
		}
		return 0;
	}
	
	return dw_cu->FindDataObjectDIE(name, pc);
}

template <class MEMORY_ADDR>
unisim::util::debug::DataObject<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindDataObject(unsigned int prc_num, const char *data_object_name) const
{
	return FindDataObject(&auto_dw_mach_state, prc_num, data_object_name);
}

template <class MEMORY_ADDR>
unisim::util::debug::DataObject<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindDataObject(const DWARF_MachineState<MEMORY_ADDR> *dw_mach_state, unsigned int prc_num, const char *data_object_name) const
{
	CLocOperationStream *c_loc_operation_stream = new CLocOperationStream(INFIX_NOTATION);
	
	std::stringstream sstr;
	sstr << data_object_name;
	
	bool status = c_loc_expr_parser.Parse(sstr, *c_loc_operation_stream);
	c_loc_expr_parser.Reset();
	if(!status)
	{
		delete c_loc_operation_stream;
		return 0;
	}

	return FindDataObject(dw_mach_state, prc_num, c_loc_operation_stream);
}

template <class MEMORY_ADDR>
unisim::util::debug::DataObject<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindDataObject(const DWARF_MachineState<MEMORY_ADDR> *dw_mach_state, unsigned int prc_num, const CLocOperationStream *c_loc_operation_stream) const
{
	FindDataObjectArguments args;
	args.c_loc_operation_stream = *c_loc_operation_stream;
	
	if(FindDataObject(dw_mach_state, prc_num, args))
	{
		DWARF_DataObject<MEMORY_ADDR> *data_object = new DWARF_DataObject<MEMORY_ADDR>(this, dw_mach_state, prc_num, args.matched_data_object_name.c_str(), c_loc_operation_stream, args.dw_data_object_loc, args.dw_die_type, args.dim, args.dw_data_object_type);
		return new DWARF_DataObjectProxy<MEMORY_ADDR>(data_object);
	}
	
	delete c_loc_operation_stream;
	return 0;
}

template <class MEMORY_ADDR>
bool DWARF_Handler<MEMORY_ADDR>::FindDataObject(const DWARF_MachineState<MEMORY_ADDR> *dw_mach_state, unsigned int prc_num, FindDataObjectArguments& args) const
{
	return FindDataObjectProlog(dw_mach_state, prc_num, args) && (args.match_or_optimized_out || FindDataObjectEpilog(dw_mach_state, prc_num, args));
}

template <class MEMORY_ADDR>
unisim::util::debug::DataObject<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::GetReturnValue(unsigned int prc_num) const
{
	return GetReturnValue(&auto_dw_mach_state, prc_num);
}

template <class MEMORY_ADDR>
unisim::util::debug::DataObject<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::GetReturnValue(const DWARF_MachineState<MEMORY_ADDR> *dw_mach_state, unsigned int prc_num) const
{
	CLocOperationStream *c_loc_operation_stream = new CLocOperationStream(INFIX_NOTATION);
	c_loc_operation_stream->Push(new CLocOperation(OP_RETURN_VALUE));
	return FindDataObject(dw_mach_state, prc_num, c_loc_operation_stream);
}

template <class MEMORY_ADDR>
unisim::util::debug::DataObject<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::GetSubProgramParameter(unsigned int prc_num, unsigned int index) const
{
	return GetSubProgramParameter(&auto_dw_mach_state, prc_num, index);
}

template <class MEMORY_ADDR>
unisim::util::debug::DataObject<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::GetSubProgramParameter(const DWARF_MachineState<MEMORY_ADDR> *dw_mach_state, unsigned int prc_num, unsigned int index) const
{
	CLocOperationStream *c_loc_operation_stream = new CLocOperationStream(INFIX_NOTATION);
	c_loc_operation_stream->Push(new CLocOpLiteralInteger(index));
	c_loc_operation_stream->Push(new CLocOperation(OP_ARG));
	return FindDataObject(dw_mach_state, prc_num, c_loc_operation_stream);
}

template <class MEMORY_ADDR>
bool DWARF_Handler<MEMORY_ADDR>::FindDataObjectProlog(const DWARF_MachineState<MEMORY_ADDR> *dw_mach_state, unsigned int prc_num, FindDataObjectArguments& args) const
{
	CLocOperationStream& c_loc_operation_stream = args.c_loc_operation_stream;
	const DWARF_DIE<MEMORY_ADDR> *& dw_die_type = args.dw_die_type;
	std::string& matched_data_object_name = args.matched_data_object_name;
	DWARF_Location<MEMORY_ADDR> *& dw_data_object_loc = args.dw_data_object_loc;
	const unisim::util::debug::Type *& dw_data_object_type = args.dw_data_object_type;
	bool& match_or_optimized_out = args.match_or_optimized_out = false;
	unsigned int& dim = args.dim;
	
	const DWARF_Frame<MEMORY_ADDR> *dw_curr_frame = dw_mach_state->GetCurrentFrame(prc_num);
	MEMORY_ADDR pc = 0;
	if(!dw_curr_frame || !dw_curr_frame->ReadProgramCounterRegister(pc)) return false;
	dw_data_object_loc = 0;
	matched_data_object_name.clear();
	CLocOperationStream dw_data_object_c_loc_operation_stream = CLocOperationStream(INFIX_NOTATION);
	
	// Base name of data object: for example base name of object is "abc" if object name is "abc[0]", "abc.x", "abc->x", "abc"
	std::string data_object_base_name;

	const CLocOperation *c_loc_op = c_loc_operation_stream.Pop();
	if(!c_loc_op || ((c_loc_op->GetOpcode() != OP_LIT_IDENT) && (c_loc_op->GetOpcode() != OP_RETURN_VALUE) && (c_loc_op->GetOpcode() != OP_LIT_INT)))
	{
		GetDebugErrorStream() << "internal error, can't determine object name (missing OP_LIT_IDENT, OP_RETURN_VALUE, or OP_LIT_INT in stream)" << std::endl;
		delete c_loc_op;
		return false;
	}
	
	bool is_op_return_value = c_loc_op->GetOpcode() == OP_RETURN_VALUE;
	bool is_op_lit_int = c_loc_op->GetOpcode() == OP_LIT_INT;
	bool is_op_lit_ident = c_loc_op->GetOpcode() == OP_LIT_IDENT;
	bool is_op_arg = false;
	int arg_num = 0;
	
	if(is_op_lit_int)
	{
		// possibly got an argument number
		arg_num = ((const CLocOpLiteralInteger *) c_loc_op)->GetValue();
		dw_data_object_c_loc_operation_stream.Push(c_loc_op);
		
		c_loc_op = c_loc_operation_stream.Pop();
		if(!c_loc_op || (c_loc_op->GetOpcode() != OP_ARG))
		{
			GetDebugErrorStream() << "internal error, missing OP_ARG in stream)" << std::endl;
			delete c_loc_op;
			return false;
		}
		
		is_op_arg = true;
	}
	
	if(is_op_lit_ident)
	{
		data_object_base_name = ((const CLocOpLiteralIdentifier *) c_loc_op)->GetIdentifier();
	}
	
	dw_data_object_c_loc_operation_stream.Push(c_loc_op);
	c_loc_op = 0;
	
	const DWARF_DIE<MEMORY_ADDR> *dw_die_data_object = 0;
	dw_die_type = 0;

	if(is_op_return_value || is_op_arg)
	{
		const DWARF_SubProgram<MEMORY_ADDR> *dw_subprogram = FindSubProgram(pc);
		
		if(!dw_subprogram)
		{
			if(debug)
			{
				GetDebugInfoStream() << "can't find subprogram at @0x" << std::hex << pc << std::dec << std::endl;
			}
			return false;
		}
		
		if(is_op_return_value)
		{
			// $return_value
			
			data_object_base_name = "$return_value";
			
			dw_data_object_loc = new DWARF_Location<MEMORY_ADDR>();
			if(!dw_subprogram->GetReturnValueLocation(dw_mach_state, prc_num, *dw_data_object_loc))
			{
				if(debug)
				{
					GetDebugInfoStream() << "can't determine location of data Object \"" << data_object_base_name << "\"" << std::endl;
				}
				if(dw_data_object_loc) delete dw_data_object_loc;
				return false;
			}
			
			if(debug)
			{
				GetDebugInfoStream() << "location of data Object \"" << data_object_base_name << "\" is:" << std::endl << (*dw_data_object_loc) << std::endl;
			}

			matched_data_object_name += data_object_base_name;

			dw_die_type = dw_subprogram->GetReturnTypeDIE();
			
			if(c_loc_operation_stream.Empty() || (dw_data_object_loc->GetType() == DW_LOC_NULL))
			{
				// match or optimized out
				dim = 0;
				dw_data_object_type = dw_die_type->GetType(0);
				match_or_optimized_out = true;
				return true;
			}
			
			if(!dw_die_type)
			{
				if(debug)
				{
					GetDebugInfoStream() << "can't determine type of data Object \"" << data_object_base_name << "\"" << std::endl;
				}
				if(dw_data_object_loc) delete dw_data_object_loc;
				dw_data_object_loc = 0;
				return false;
			}
		}
		else
		{
			// $0, $1, ....
			
			// check argument number range
			if((arg_num < 0) || (arg_num >= (int) dw_subprogram->GetArity()))
			{
				GetDebugErrorStream() << "Subprogram argument number is out of range (shall be >= 0 and < " << dw_subprogram->GetArity() << ")" << std::endl;
				delete c_loc_op;
				return false;
			}
			
			std::stringstream arg_sstr;
			arg_sstr << '$' << arg_num;
			data_object_base_name = arg_sstr.str();
			
			dw_die_data_object = dw_subprogram->GetFormalParameterDIE(arg_num);
		}
	}
	else //if(is_op_lit_ident)
	{
		// identifier
		
		// Find DIE of data object
		// (1) Find DIE from the current scope i.e. current PC
		dw_die_data_object = FindDataObjectDIE(data_object_base_name.c_str(), pc);
		
		if(!dw_die_data_object)
		{
			// (2) if not found, find DIE from the global scope
			dw_die_data_object = FindDIEByPubName(data_object_base_name.c_str());
		
			if(!dw_die_data_object)
			{
				if(debug)
				{
					GetDebugInfoStream() << "DIE of data Object \"" << data_object_base_name << "\" not found" << std::endl;
				}
				return false;
			}
		}
	}
	
	if(is_op_arg || is_op_lit_ident)
	{
		// check that we've really found a data object
		switch(dw_die_data_object->GetTag())
		{
			case DW_TAG_variable:
			case DW_TAG_formal_parameter:
			case DW_TAG_constant:
				break;
			default:
				if(debug)
				{
					GetDebugInfoStream() << "DIE found for data Object \"" << data_object_base_name << "\" does not describe a data Object" << std::endl;
				}
				return false;
		}
		
		// Get the data object location
		if(debug)
		{
			GetDebugInfoStream() << "trying to determine location of data Object \"" << data_object_base_name << "\"" << std::endl;
		}
		dw_data_object_loc = new DWARF_Location<MEMORY_ADDR>();
		if(!dw_die_data_object->GetLocation(dw_curr_frame, *dw_data_object_loc))
		{
			if(debug)
			{
				GetDebugInfoStream() << "can't determine location of data Object \"" << data_object_base_name << "\"" << std::endl;
			}
			if(dw_data_object_loc) delete dw_data_object_loc;
			return false;
		}
		if(debug)
		{
			GetDebugInfoStream() << "location of data Object \"" << data_object_base_name << "\" is:" << std::endl << (*dw_data_object_loc) << std::endl;
		}

		matched_data_object_name += data_object_base_name;

		// Determine the DIE that describes the type of the data object
		dw_die_type = dw_die_data_object->GetTypeDIE();
		if(!dw_die_type)
		{
			if(debug)
			{
				GetDebugInfoStream() << "can't determine type of data Object \"" << data_object_base_name << "\"" << std::endl;
			}
			if(dw_data_object_loc) delete dw_data_object_loc;
			dw_data_object_loc = 0;
			return false;
		}
		
		if(c_loc_operation_stream.Empty() || (dw_data_object_loc->GetType() == DW_LOC_NULL))
		{
			// match or optimized out
			dim = 0;
			dw_data_object_type = dw_die_type->GetType(dw_curr_frame);
			match_or_optimized_out = true;
			return true;
		}
	}
	
	dim = 0;
	return true;
}

template <class MEMORY_ADDR>
bool DWARF_Handler<MEMORY_ADDR>::FindDataObjectEpilog(const DWARF_MachineState<MEMORY_ADDR> *dw_mach_state, unsigned int prc_num, FindDataObjectArguments& args) const
{
	CLocOperationStream& c_loc_operation_stream = args.c_loc_operation_stream;
	const DWARF_DIE<MEMORY_ADDR> *& dw_die_type = args.dw_die_type;
	std::string& matched_data_object_name = args.matched_data_object_name;
	DWARF_Location<MEMORY_ADDR> *& dw_data_object_loc = args.dw_data_object_loc;
	const unisim::util::debug::Type *& dw_data_object_type = args.dw_data_object_type;
	bool& match_or_optimized_out = args.match_or_optimized_out = false;
	unsigned int& dim = args.dim;
	
	if(!dw_die_type) return false;
	
	const DWARF_Frame<MEMORY_ADDR> *dw_curr_frame = dw_mach_state->GetCurrentFrame(prc_num);
	CLocOperationStream dw_data_object_c_loc_operation_stream = CLocOperationStream(INFIX_NOTATION);
	const CLocOperation *c_loc_op = 0;
	DWARF_DataObject<MEMORY_ADDR> *dw_data_object = 0;
	bool is_dereferencing_a_structure = false; // Note: when it is "true", we have one operation speculatively fetched (OP_STRUCT_DEREF)
	                                           // if it is set, we check that the following DIE is really a DW_TAG_structure_type
	bool status = true;

	// Explore the imbricated type definitions
	do
	{
		if(dw_die_type->GetTag() != DW_TAG_array_type)
		{
			dim = 0;
		}
		
		// Several cases for type:
		// (1) structure/class/union
		// (2) multidimensional arrays
		// (3) pointers
		// (4) typedefs
		// (5) basic types
		switch(dw_die_type->GetTag())
		{
			case DW_TAG_structure_type:
			case DW_TAG_class_type:
			case DW_TAG_union_type:
				{
					if(!is_dereferencing_a_structure)
					{
						c_loc_op = c_loc_operation_stream.Pop();
						if(!c_loc_op)
						{
							// not supposed to reach this point
							GetDebugErrorStream() << "internal error (missing OP_STRUCT_REF in stream)" << std::endl;
							status = false;
							break;
						}
						if(c_loc_op->GetOpcode() != OP_STRUCT_REF)
						{
							if(debug)
							{
								GetDebugErrorStream() << "data Object \"" << matched_data_object_name << "\" is a structure/class/union";
								switch(c_loc_op->GetOpcode())
								{
									case OP_DEREF:
										 GetDebugErrorStream() << " not a pointer";
										 break;
									case OP_STRUCT_DEREF:
										 GetDebugErrorStream() << " not a pointer to a structure/class/union";
										 break;
									case OP_ARRAY_SUBSCRIPT:
										 GetDebugErrorStream() << " not an array";
										 break;
									default:
										break;
								}
								GetDebugErrorStream() << std::endl;
							}
							status = false;
							break;
						}
						dw_data_object_c_loc_operation_stream.Push(c_loc_op);
					}

					c_loc_op = c_loc_operation_stream.Pop();
					if(!c_loc_op || (c_loc_op->GetOpcode() != OP_LIT_IDENT))
					{
						// not supposed to reach this point too
						GetDebugErrorStream() << "internal error (missing OP_LIT_IDENT in stream)" << std::endl;
						status = false;
						break;
					}
					
					std::string data_member_name = ((const CLocOpLiteralIdentifier *) c_loc_op)->GetIdentifier();
					dw_data_object_c_loc_operation_stream.Push(c_loc_op);
					c_loc_op = 0;
					
					const DWARF_DIE<MEMORY_ADDR> *dw_die_data_member = dw_die_type->FindDataMemberDIE(data_member_name.c_str());
					if(!dw_die_data_member)
					{
						GetDebugErrorStream() << "can't find data member \"" << data_member_name << "\" in data Object \"" << matched_data_object_name << "\"" << std::endl;
						status = false;
						break;
					}
					
					if(!dw_die_data_member->GetDataMemberLocation(dw_curr_frame, *dw_data_object_loc))
					{
						GetDebugErrorStream() << "can't determine location of data Member \"" << data_member_name << "\" of data Object \"" << matched_data_object_name << "\"" << std::endl;
						status = false;
						break;
					}

					if(is_dereferencing_a_structure)
					{
						matched_data_object_name += "->";
						is_dereferencing_a_structure = false;
					}
					else
					{
						matched_data_object_name += '.';
					}
					matched_data_object_name += data_member_name;

					if(debug)
					{
						GetDebugInfoStream() << "after crawling structure, location of data Object \"" << matched_data_object_name << "\" is:" << std::endl << (*dw_data_object_loc) << std::endl;
					}
							
					// Determine the DIE that describes the type of the data object
					if(!(dw_die_type = dw_die_data_member->GetTypeDIE()))
					{
						GetDebugErrorStream() << "can't determine type of data Object \"" << matched_data_object_name << "\"" << std::endl;
						status = false;
						break;
					}
					
					if(c_loc_operation_stream.Empty() || (dw_data_object_loc->GetType() == DW_LOC_NULL))
					{
						// match or optimized out
						dw_data_object_type = dw_die_type->GetType(dw_curr_frame);
						match_or_optimized_out = true;
						break;
					}
					
					is_dereferencing_a_structure = false;
				}
				break;
			case DW_TAG_array_type:
				{
					if(is_dereferencing_a_structure)
					{
						GetDebugErrorStream() << "\"" << matched_data_object_name << "\" is an array not a pointer to a structure/class/union" << std::endl;
						status = false;
						break;
					}
					uint64_t array_element_bitsize = 0;
					if(!dw_die_type->GetArrayElementPaddedBitSize(dw_curr_frame, array_element_bitsize))
					{
						GetDebugErrorStream() << "can't get element bit size of data array \"" << matched_data_object_name << "\"" << std::endl;
						status = false;
						break;
					}

					uint8_t dw_array_ordering = DW_ORD_row_major;
					if(!dw_die_type->GetOrdering(dw_array_ordering))
					{
						dw_array_ordering = dw_die_type->GetCompilationUnit()->GetDefaultOrdering();
					}
					
					const std::vector<DWARF_DIE<MEMORY_ADDR> *>& children = dw_die_type->GetChildren();
					
					unsigned int num_children = children.size();
					
					if(!num_children)
					{
						GetDebugErrorStream() << "DWARF DIE for data array \"" << matched_data_object_name << "\" has no subrange type DIE" << std::endl;
						status = false;
						break;
					}
					
					dw_data_object_loc->SetEncoding(0);
					unsigned int i = 0;
					unsigned int j = 0;
					
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
						
						if((child->GetTag() == DW_TAG_subrange_type) && (j++ >= dim))
						{
							c_loc_op = c_loc_operation_stream.Pop();
							if(!c_loc_op)
							{
								// not supposed to reach this point
								GetDebugErrorStream() << "internal error (missing OP_ARRAY_SUBSCRIPT in stream)" << std::endl;
								status = false;
								break;
							}
							
							if(c_loc_op->GetOpcode() != OP_ARRAY_SUBSCRIPT)
							{
								GetDebugErrorStream() << "data Object \"" << matched_data_object_name << "\" is an array";
								switch(c_loc_op->GetOpcode())
								{
									case OP_DEREF:
										GetDebugErrorStream() << " not a pointer";
										break;
									case OP_STRUCT_DEREF:
										GetDebugErrorStream() << " not a pointer to a structure/class/union";
										break;
									case OP_STRUCT_REF:
										GetDebugErrorStream() << " not a structure/class/union";
										break;
									default:
										break;
								}
								GetDebugErrorStream() << std::endl;
								status = false;
								break;
							}
							
							dw_data_object_c_loc_operation_stream.Push(c_loc_op);
							c_loc_op = c_loc_operation_stream.Pop();
							
							if(!c_loc_op || (c_loc_op->GetOpcode() != OP_LIT_INT))
							{
								GetDebugErrorStream() << "missing array subscript in indexing of data array \"" << matched_data_object_name << "\"" << std::endl;
								status = false;
								break;
							}
							
							int64_t subscript = ((const CLocOpLiteralInteger *) c_loc_op)->GetValue();
							dw_data_object_c_loc_operation_stream.Push(c_loc_op);
							c_loc_op = 0;
							
							int64_t lower_bound = 0;
							int64_t upper_bound = 0;

							if(!child->GetLowerBound(dw_curr_frame, lower_bound))
							{
								GetDebugErrorStream() << "can't determine subscript lower bound for data array \"" << matched_data_object_name << "\" " << std::endl;
								status = false;
								break;
							}
							
							if(!child->GetUpperBound(dw_curr_frame, upper_bound))
							{
								GetDebugErrorStream() << "can't determine subscript upper bound for data array \"" << matched_data_object_name << "\" " << std::endl;
								status = false;
								break;
							}
							
							if((subscript < lower_bound) || (subscript > upper_bound))
							{
								GetDebugErrorStream() << "subscript (" << subscript << ") is out of range for data array \"" << matched_data_object_name << "\" " << std::endl;
								status = false;
								break;
							}
							
							int64_t normalized_subscript = subscript - lower_bound;
							
							uint64_t array_element_count = 0;
							if(!dw_die_type->GetArrayElementCount(dw_curr_frame, dim + 1, array_element_count))
							{
								GetDebugErrorStream() << "can't get element count of data array \"" << matched_data_object_name << "\"" << std::endl;
								status = false;
								break;
							}
							
							dw_data_object_loc->SetBitSize(array_element_count * array_element_bitsize);
							dw_data_object_loc->IncBitOffset(normalized_subscript * dw_data_object_loc->GetBitSize());
							dw_data_object_loc->SetByteSize((dw_data_object_loc->GetBitSize() + 7) / 8);
							
							matched_data_object_name += '[';
							std::stringstream subscript_sstr;
							subscript_sstr << subscript;
							matched_data_object_name += subscript_sstr.str();
							matched_data_object_name += ']';
							
							if(debug)
							{
								GetDebugInfoStream() << "after indexing array, location of data Object \"" << matched_data_object_name << "\" is:" << std::endl << (*dw_data_object_loc) << std::endl;
							}
							
							dim++;
							
							if(c_loc_operation_stream.Empty() || (dw_data_object_loc->GetType() == DW_LOC_NULL))
							{
								// match or optimized out
								if(array_element_count == 1)
								{
									uint8_t array_element_encoding = 0;
									if(!dw_die_type->GetArrayElementEncoding(array_element_encoding))
									{
										array_element_encoding = 0;
									}
									dw_data_object_loc->SetEncoding(array_element_encoding);
								}
								dw_data_object_type = dw_die_type->GetType(dw_curr_frame, false, dim);
								match_or_optimized_out = true;
								break;
							}
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
					while(status && !match_or_optimized_out);
					
					if(status && !match_or_optimized_out)
					{
						// Determine the DIE that describes the type of the data object
						if(!(dw_die_type = dw_die_type->GetTypeDIE()))
						{
							GetDebugErrorStream() << "can't determine data element type of data array \"" << matched_data_object_name << "\"" << std::endl;
							status = false;
							break;
						}
					}
				}
				break;
			case DW_TAG_pointer_type:
				{
					if(is_dereferencing_a_structure)
					{
						GetDebugErrorStream() << "\"" << matched_data_object_name << "\" is not a pointer to a structure/class/union" << std::endl;
						status = false;
						break;
					}

					c_loc_op = c_loc_operation_stream.Pop();
					if(!c_loc_op)
					{
						// not supposed to reach this point
						GetDebugErrorStream() << "internal error (missing OP_DEREF/OP_ARRAY_SUBSCRIPT/OP_STRUCT_DEREF in stream)" << std::endl;
						status = false;
						break;
					}
					
					if(((c_loc_op->GetOpcode() != OP_DEREF) && (c_loc_op->GetOpcode() != OP_ARRAY_SUBSCRIPT) && (c_loc_op->GetOpcode() != OP_STRUCT_DEREF)))
					{
						GetDebugErrorStream() << "\"" << matched_data_object_name << "\" is a pointer";
						switch(c_loc_op->GetOpcode())
						{
							case OP_STRUCT_REF:
								GetDebugErrorStream() << " not a structure";
								break;
							default:
								break;
						}
						GetDebugErrorStream() << std::endl;
						status = false;
						break;
					}
					
					bool is_indexing_a_pointer = (c_loc_op->GetOpcode() == OP_ARRAY_SUBSCRIPT);
					is_dereferencing_a_structure = (c_loc_op->GetOpcode() == OP_STRUCT_DEREF);
					
					// dereferencing the pointer:
					// (1) create a data object handle for the pointer
					// (2) fetch pointer as raw data
					// (2) read (interpret) pointer value
					dw_data_object_c_loc_operation_stream.Push(c_loc_op);
					c_loc_op = 0;
					
					// (1)
					dw_data_object = new DWARF_DataObject<MEMORY_ADDR>(this, dw_mach_state, prc_num, matched_data_object_name.c_str(), new CLocOperationStream(), dw_data_object_loc, (const DWARF_DIE<MEMORY_ADDR> *) 0, /* dim */ 0, new unisim::util::debug::Type());
					
					// (2)
					if(!dw_data_object->Fetch())
					{
						GetDebugErrorStream() << "can't get value of Pointer \"" << matched_data_object_name << "\"" << std::endl;
						status = false;
						delete dw_data_object;
						break;
					}
					
					// (3)
					MEMORY_ADDR pointer_bit_size = dw_data_object_loc->GetBitSize();
					
					uint64_t pointer_value = 0;
					if(pointer_bit_size > (8 * sizeof(pointer_value))) pointer_bit_size = 8 * sizeof(pointer_value);
					if(!dw_data_object->Read(0, pointer_value, pointer_bit_size))
					{
						GetDebugErrorStream() << "can't read value of Pointer \"" << matched_data_object_name << "\"" << std::endl;
						status = false;
						delete dw_data_object;
						break;
					}
					
					const std::set<std::pair<MEMORY_ADDR, MEMORY_ADDR> > ranges = dw_data_object_loc->GetRanges();
					delete dw_data_object;
					// Note: dw_data_object_loc is also deleted with dw_data_object
					
					// continue further processing with a data object handle for the pointed data object
					dw_data_object_loc = new DWARF_Location<MEMORY_ADDR>();
					dw_data_object_loc->SetRanges(ranges);
					dw_data_object_loc->SetAddress(pointer_value);

					if(is_indexing_a_pointer)
					{
						c_loc_op = c_loc_operation_stream.Pop();
						if(!c_loc_op || (c_loc_op->GetOpcode() != OP_LIT_INT))
						{
							// not supposed to reach this point
							GetDebugErrorStream() << "internal error (missing OP_LIT_INT in stream)" << std::endl;
							status = false;
							break;
						}
						
						int64_t subscript = ((const CLocOpLiteralInteger *) c_loc_op)->GetValue();
						dw_data_object_c_loc_operation_stream.Push(c_loc_op);
						c_loc_op = 0;
						
						// get type of pointed element
						// Determine the DIE that describes the type of the data object
						if(!(dw_die_type = dw_die_type->GetTypeDIE()))
						{
							GetDebugErrorStream() << "can't determine type of element pointed by \"" << matched_data_object_name << "\"" << std::endl;
							status = false;
							break;
						}
						
						// get byte size of pointed element data type
						uint64_t dw_data_object_byte_size = 0;
						if(!dw_die_type->GetByteSize(dw_curr_frame, dw_data_object_byte_size))
						{
							GetDebugErrorStream() << "can't determine byte size (with padding) of element pointed by \"" << matched_data_object_name << "\"" << std::endl;
							status = false;
							break;
						}
						// set byte size of seeked pointed element
						dw_data_object_loc->SetByteSize(dw_data_object_byte_size);

						// get bit size of pointed element data type
						uint64_t dw_data_object_bit_size = 0;
						if(!dw_die_type->GetBitSize(dw_curr_frame, dw_data_object_bit_size))
						{
							GetDebugErrorStream() << "can't determine bit size of element pointed by  \"" << matched_data_object_name << "\"" << std::endl;
							status = false;
							break;
						}
						// set bit size of seeked pointed element
						dw_data_object_loc->SetBitSize(dw_data_object_bit_size);
						
						int64_t normalized_subscript = subscript;

						// advance in pointer buffer to seeked pointed element by shifting bit offset
						dw_data_object_loc->SetBitOffset(normalized_subscript * dw_data_object_loc->GetBitSize());
						
						matched_data_object_name += '[';
						std::stringstream subscript_sstr;
						subscript_sstr << subscript;
						matched_data_object_name += subscript_sstr.str();
						matched_data_object_name += ']';

						if(debug)
						{
							GetDebugInfoStream() << "after indexing pointer, location of data Object \"" << matched_data_object_name << "\" is:" << std::endl << (*dw_data_object_loc) << std::endl;
						}

						if(c_loc_operation_stream.Empty() || (dw_data_object_loc->GetType() == DW_LOC_NULL))
						{
							// match or optimized out
							if(dw_data_object_loc->GetType() != DW_LOC_NULL)
							{
								// Determine the encoding of the data object
								uint8_t dw_data_object_encoding = 0;
								if(!dw_die_type->GetEncoding(dw_data_object_encoding))
								{
									dw_data_object_encoding = 0;
								}
								dw_data_object_loc->SetEncoding(dw_data_object_encoding);
							}
							
							dw_data_object_type = dw_die_type->GetType(dw_curr_frame);
							match_or_optimized_out = true;
						}
					}
					else
					{
						// Determine the DIE that describes the type of the data object
						if(!(dw_die_type = dw_die_type->GetTypeDIE()))
						{
							GetDebugErrorStream() << "can't determine data type of element pointer by \"" << matched_data_object_name << "\"" << std::endl;
							status = false;
							break;
						}
						
						if(is_dereferencing_a_structure)
						{
							// final match of object name is postponed later (see case of structure/class/union above)
							break;
						}

						matched_data_object_name = std::string("(*") + matched_data_object_name + ")";
						
						if(c_loc_operation_stream.Empty() || (dw_data_object_loc->GetType() == DW_LOC_NULL))
						{
							const DWARF_DIE<MEMORY_ADDR> *dw_die_pointed_type = dw_die_type;
							
							// match or optimized out
							if(dw_data_object_loc->GetType() != DW_LOC_NULL)
							{
								// Determine the size in bytes (including padding bits) of the pointed data object
								uint64_t dw_data_object_byte_size = 0;
								if(!dw_die_pointed_type->GetByteSize(dw_curr_frame, dw_data_object_byte_size))
								{
									GetDebugErrorStream() << "can't determine byte size (with padding) of data Object \"" << matched_data_object_name << "\"" << std::endl;
									status = false;
									break;
								}
								dw_data_object_loc->SetByteSize(dw_data_object_byte_size);
								
								// Determine the actual size in bits (excluding padding bits) of the pointed data object
								uint64_t dw_data_object_bit_size = 0;
								if(!dw_die_pointed_type->GetBitSize(dw_curr_frame, dw_data_object_bit_size))
								{
									GetDebugErrorStream() << "can't determine bit size of data Object \"" << matched_data_object_name << "\"" << std::endl;
									status = false;
									break;
								}
								dw_data_object_loc->SetBitSize(dw_data_object_bit_size);
								
								// Determine the encoding of the pointed data object
								uint8_t dw_data_object_encoding = 0;
								if(!dw_die_pointed_type->GetEncoding(dw_data_object_encoding))
								{
									dw_data_object_encoding = 0;
								}
								dw_data_object_loc->SetEncoding(dw_data_object_encoding);
							}
							
							dw_data_object_type = dw_die_pointed_type->GetType(dw_curr_frame);
							match_or_optimized_out = true;
						}
						
						if(debug)
						{
							GetDebugInfoStream() << "after dereferencing a pointer (not to a structure), location of data Object \"" << matched_data_object_name << "\" is:" << std::endl << (*dw_data_object_loc) << std::endl;
						}
					}
				}
				break;
			case DW_TAG_typedef:
				// Determine the DIE that describes the type of the data object
				if(!(dw_die_type = dw_die_type->GetTypeDIE()))
				{
					GetDebugErrorStream() << "can't determine data type defined by a typedef for \"" << matched_data_object_name << "\"" << std::endl;
					status = false;
					break;
				}
				break;
				
			case DW_TAG_const_type:
				// Determine the DIE that describes the type of the data object
				if(!(dw_die_type = dw_die_type->GetTypeDIE()))
				{
					GetDebugErrorStream() << "can't determine data type of const construction for \"" << matched_data_object_name << "\"" << std::endl;
					status = false;
					break;
				}
				break;

			case DW_TAG_volatile_type:
				// Determine the DIE that describes the type of the data object
				if(!(dw_die_type = dw_die_type->GetTypeDIE()))
				{
					GetDebugErrorStream() << "can't determine data type of volatile construction for \"" << matched_data_object_name << "\"" << std::endl;
					status = false;
					break;
				}
				break;

			case DW_TAG_base_type: // Reaching these cases means operator stream does not match any data object
			case DW_TAG_enumeration_type:
			case DW_TAG_subroutine_type:
			case DW_TAG_unspecified_type:
				if(is_dereferencing_a_structure)
				{
					GetDebugErrorStream() << "\"" << matched_data_object_name << "\" is not a pointer to a structure" << std::endl;
					status = false;
					break;
				}
				
				c_loc_op = c_loc_operation_stream.Pop();
				if(!c_loc_op)
				{
					// not supposed to reach this point
					GetDebugErrorStream() << "internal error (missing operator in stream)" << std::endl;
					status = false;
					break;
				}

				GetDebugErrorStream() << "data Object \"" << matched_data_object_name << "\" is not ";
				switch(c_loc_op->GetOpcode())
				{
					case OP_DEREF:
						GetDebugErrorStream() << "a pointer";
						break;
					case OP_STRUCT_REF:
						GetDebugErrorStream() << "a structure";
						break;
					case OP_STRUCT_DEREF:
						GetDebugErrorStream() << "a pointer to a structure";
						break;
					case OP_ARRAY_SUBSCRIPT:
						GetDebugErrorStream() << "an array";
						break;
					default:
						GetDebugErrorStream() << "handled (" << (unsigned int) c_loc_op->GetOpcode() << ")";
						break;
				}
				GetDebugErrorStream() << std::endl;
				status = false;
				delete c_loc_op;
				c_loc_op = 0;
				break;
				
			default:
				GetDebugErrorStream() << "don't know how to handle type (" << DWARF_GetTagName(dw_die_type->GetTag()) << ") of data Object \"" << matched_data_object_name << "\"" << std::endl;
				status = false;
				break;
		}
	}
	while(status && !match_or_optimized_out && dw_die_type);
	
	if(!match_or_optimized_out)
	{
		if(dw_data_object_loc) delete dw_data_object_loc;
		dw_data_object_loc = 0;
		dw_data_object_type = 0;
	}

	if(c_loc_op) delete c_loc_op;
	
	return match_or_optimized_out;
}

template <class MEMORY_ADDR>
void DWARF_Handler<MEMORY_ADDR>::ScanDataObjectNames(unsigned int prc_num, unisim::service::interfaces::DataObjectNameScanner& scanner, typename unisim::service::interfaces::DataObjectLookupBase::Scope scope) const
{
	ScanDataObjectNames(&auto_dw_mach_state, prc_num, scanner, scope);
}

template <class MEMORY_ADDR>
void DWARF_Handler<MEMORY_ADDR>::ScanDataObjectNames(const DWARF_MachineState<MEMORY_ADDR> *dw_mach_state, unsigned int prc_num, unisim::service::interfaces::DataObjectNameScanner& scanner, typename unisim::service::interfaces::DataObjectLookupBase::Scope scope) const
{
	DWARF_Frame<MEMORY_ADDR> *dw_curr_frame = dw_mach_state->GetCurrentFrame(prc_num);
	MEMORY_ADDR pc = 0;
	if(!dw_curr_frame || !dw_curr_frame->ReadProgramCounterRegister(pc)) return;
	
	const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu = FindCompilationUnitByAddrRange(pc, 1);
	
	if(dw_cu)
	{
		dw_cu->ScanDataObjectNames(scanner, pc, (scope & unisim::service::interfaces::DataObjectLookupBase::SCOPE_LOCAL_ONLY) != 0);
	}
	else
	{
		if(debug)
		{
			GetDebugInfoStream() << "compilation unit for PC=0x" << std::hex << pc << std::dec << " not found" << std::endl;
		}
	}
	
	if(scope & unisim::service::interfaces::DataObjectLookupBase::SCOPE_GLOBAL_ONLY)
	{
		unsigned int num_pubnames = dw_pubnames.size();
		unsigned int i;
		for(i = 0; i < num_pubnames; i++)
		{
			dw_pubnames[i]->ScanDataObjectNames(scanner);
		}
	}
}

template <class MEMORY_ADDR>
bool DWARF_Handler<MEMORY_ADDR>::GetCallingConvention(MEMORY_ADDR pc, uint8_t& calling_convention) const
{
	const DWARF_DIE<MEMORY_ADDR> *dw_die = FindSubProgramDIE(pc);
	
	return dw_die && dw_die->GetCallingConvention(calling_convention);
}

template <class MEMORY_ADDR>
unsigned int DWARF_Handler<MEMORY_ADDR>::GetReturnAddressSize(MEMORY_ADDR pc) const
{
	if(strcmp(blob->GetArchitecture(), "68hc12") == 0)
	{
		uint8_t calling_convention;
		if(GetCallingConvention(pc, calling_convention))
		{
			if(calling_convention == hc12::DW_CC_far) return 3;
			if(calling_convention == DW_CC_normal) return 2;
			if(calling_convention == DW_CC_nocall) return 2;
		}
	}
	return arch_address_size;
}

template <class MEMORY_ADDR>
DW_CFA_Specification DWARF_Handler<MEMORY_ADDR>::GetCFA_Specification() const
{
	return (strcmp(blob->GetArchitecture(), "68hc12") == 0) ? DW_CFA_IS_SP_VALUE_ON_ENTRY_TO_THE_CURRENT_FRAME : DW_CFA_IS_SP_AT_THE_CALL_SITE_IN_THE_PREVIOUS_FRAME;
}

template <class MEMORY_ADDR>
DW_CFA_RegRuleOffsetSpecification DWARF_Handler<MEMORY_ADDR>::GetCFA_RegRuleOffsetSpecification() const
{
	return (strcmp(blob->GetArchitecture(), "68hc12") == 0) ? DW_CFA_REG_RULE_OFFSET_IS_SP_RELATIVE : DW_CFA_REG_RULE_OFFSET_IS_CFA_RELATIVE;
}

template <class MEMORY_ADDR>
bool DWARF_Handler<MEMORY_ADDR>::ComputeCFA(const DWARF_Frame<MEMORY_ADDR> *dw_frame, MEMORY_ADDR& cfa) const
{
	if(!dw_frame) return false;
	const DWARF_MachineState<MEMORY_ADDR> *dw_mach_state = dw_frame->GetMachineState();
	if(dw_mach_state == &auto_dw_mach_state)
	{
		auto_dw_mach_state.InvalidateFrames(dw_frame->GetProcessorNumber());
	}
	
	return dw_mach_state->ComputeCFA(dw_frame, cfa);
}

template <class MEMORY_ADDR>
std::vector<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::GetBackTrace(unsigned int prc_num) const
{
	auto_dw_mach_state.InvalidateFrames(prc_num);
	return auto_dw_mach_state.GetBackTrace(prc_num);
}

template <class MEMORY_ADDR>
bool DWARF_Handler<MEMORY_ADDR>::GetReturnAddress(unsigned int prc_num, MEMORY_ADDR& ret_addr) const
{
	auto_dw_mach_state.InvalidateFrames(prc_num);
	return auto_dw_mach_state.GetReturnAddress(prc_num, ret_addr);
}

template <class MEMORY_ADDR>
std::ostream& DWARF_Handler<MEMORY_ADDR>::GetGeneralDebugInfoStream() const
{
	return *debug_info_stream;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_Handler<MEMORY_ADDR>::GetGeneralDebugWarningStream() const
{
	return *debug_warning_stream;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_Handler<MEMORY_ADDR>::GetGeneralDebugErrorStream() const
{
	return *debug_error_stream;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_Handler<MEMORY_ADDR>::GetDebugInfoStream() const
{
	if(HasFilename())
	{
		(*debug_info_stream) << "In File \"" << GetFilename() << "\", ";
	}
	return *debug_info_stream;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_Handler<MEMORY_ADDR>::GetDebugWarningStream() const
{
	if(HasFilename())
	{
		(*debug_warning_stream) << "In File \"" << GetFilename() << "\", ";
	}
	return *debug_warning_stream;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_Handler<MEMORY_ADDR>::GetDebugErrorStream() const
{
	if(HasFilename())
	{
		(*debug_error_stream) << "In File \"" << GetFilename() << "\", ";
	}
	return *debug_error_stream;
}

template <class MEMORY_ADDR>
const unisim::util::debug::SymbolTable<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::GetSymbolTable() const
{
	return symbol_table;
}

template <class MEMORY_ADDR>
const unisim::util::blob::Blob<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::GetBlob() const
{
	return blob;
}

// template <class MEMORY_ADDR>
// template <typename T, typename VISITOR>
// T DWARF_Handler<MEMORY_ADDR>::Scan(VISITOR& visitor) const
// {
// 	typename std::map<uint64_t, DWARF_CompilationUnit<MEMORY_ADDR> *>::const_iterator dw_cu_iter;
// 	for(dw_cu_iter = dw_cus.begin(); dw_cu_iter != dw_cus.end(); dw_cu_iter++)
// 	{
// 		DWARF_CompilationUnit<MEMORY_ADDR> const *dw_cu = (*dw_cu_iter).second;
// 		T ret = visitor.Visit(dw_cu);
// 		if(ret) return ret;
// 	}
// 	
// 	return T();
// }

template <class MEMORY_ADDR>
template <typename VISITOR, typename T>
T DWARF_Handler<MEMORY_ADDR>::Scan(VISITOR& visitor) const
{
	typename std::map<uint64_t, DWARF_CompilationUnit<MEMORY_ADDR> *>::const_iterator dw_cu_iter;
	for(dw_cu_iter = dw_cus.begin(); dw_cu_iter != dw_cus.end(); dw_cu_iter++)
	{
		DWARF_CompilationUnit<MEMORY_ADDR> const *dw_cu = (*dw_cu_iter).second;
		T ret = visitor.Visit(dw_cu);
		if(ret) return ret;
	}
	
	return T();
}

template <class MEMORY_ADDR>
bool DWARF_Handler<MEMORY_ADDR>::GetFrameBase(const DWARF_Frame<MEMORY_ADDR> *dw_curr_frame, MEMORY_ADDR& frame_base) const
{
	struct Visitor
	{
		MEMORY_ADDR pc;
		
		const DWARF_DIE<MEMORY_ADDR> *Visit(const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu)
		{
			return dw_cu->template Scan<Visitor, const DWARF_DIE<MEMORY_ADDR> *>(*this);
		}
		
		const DWARF_DIE<MEMORY_ADDR> *Visit(const DWARF_DIE<MEMORY_ADDR> *dw_die)
		{
			const DWARF_DIE<MEMORY_ADDR> *dw_found_die = dw_die->template Scan<Visitor, const DWARF_DIE<MEMORY_ADDR> *>(*this);
			
			return dw_found_die ? dw_found_die : ((((dw_die->GetTag() == DW_TAG_subprogram) || (dw_die->GetTag() == DW_TAG_entry_point)) && dw_die->HasOverlap(pc, 1)) ? dw_die : 0);
		}
	};
	
	Visitor visitor;
	if(!dw_curr_frame || !dw_curr_frame->ReadProgramCounterRegister(visitor.pc)) return false;

	const DWARF_DIE<MEMORY_ADDR> *dw_found_die = this->template Scan<Visitor, const DWARF_DIE<MEMORY_ADDR> *>(visitor);
	
	return dw_found_die && dw_found_die->GetFrameBase(dw_curr_frame, frame_base);
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
