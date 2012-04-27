/*
 *  Copyright (c) 2007,
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
 
#ifndef __UNISIM_UTIL_LOADER_ELF_LOADER_ELF_LOADER_TCC__
#define __UNISIM_UTIL_LOADER_ELF_LOADER_ELF_LOADER_TCC__

#include <unisim/util/endian/endian.hh>
#include <iostream>
#include <fstream>
#include <stdlib.h>

namespace unisim {
namespace util {
namespace loader {
namespace elf_loader {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::ElfLoaderImpl(unisim::kernel::logger::Logger& _logger, unisim::service::interfaces::Registers *_regs_if, unisim::service::interfaces::Memory<MEMORY_ADDR> *_mem_if, const unisim::util::debug::blob::Blob<MEMORY_ADDR> *_blob)
	: logger(_logger)
	, filename()
	, base_addr(0)
	, force_base_addr(false)
	, force_use_virtual_address(true)
	, dump_headers(false)
	, blob(0)
	, const_blob(_blob)
	, symtab_handler(0)
	, dw_handler(0)
	, regs_if(_regs_if)
	, mem_if(_mem_if)
	, verbose(false)
	, endianness(E_LITTLE_ENDIAN)
	, parse_dwarf(false)
{
	if(const_blob)
	{
		const_blob->Catch();
		ParseSymbols();
	}
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::~ElfLoaderImpl()
{
	if(dw_handler)
	{
		delete dw_handler;
	}
	
	if(blob)
	{
		blob->Release();
	}
	
	if(const_blob)
	{
		const_blob->Release();
	}
	
	if(symtab_handler)
	{
		delete symtab_handler;
	}
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
void ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::SetOption(Option opt, MEMORY_ADDR addr)
{
	switch(opt)
	{
		case OPT_BASE_ADDR:
			base_addr = addr;
			break;
		default:
			break;
	}
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
void ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::SetOption(Option opt, const char *s)
{
	switch(opt)
	{
		case OPT_FILENAME:
			filename = s;
			break;
		case OPT_DWARF_TO_HTML_OUTPUT_DIRECTORY:
			dwarf_to_html_output_directory = s;
			break;
		case OPT_DWARF_REGISTER_NUMBER_MAPPING_FILENAME:
			dwarf_register_number_mapping_filename = s;
			break;
		default:
			break;
	}
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
void ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::SetOption(Option opt, bool flag)
{
	switch(opt)
	{
		case OPT_FORCE_BASE_ADDR:
			force_base_addr = flag;
			break;
		case OPT_FORCE_USE_VIRTUAL_ADDRESS:
			force_use_virtual_address = flag;
			break;
		case OPT_DUMP_HEADERS:
			dump_headers = flag;
			break;
		case OPT_VERBOSE:
			verbose = flag;
			break;
		case OPT_PARSE_DWARF:
			parse_dwarf = flag;
			break;
		default:
			break;
	}
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
void ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::GetOption(Option opt, MEMORY_ADDR& addr)
{
	switch(opt)
	{
		case OPT_BASE_ADDR:
			addr = base_addr;
			break;
		default:
			addr = 0;
			break;
	}
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
void ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::GetOption(Option opt, std::string& s)
{
	switch(opt)
	{
		case OPT_FILENAME:
			s = filename;
			break;
		case OPT_DWARF_TO_HTML_OUTPUT_DIRECTORY:
			s = dwarf_to_html_output_directory;
			break;
		case OPT_DWARF_REGISTER_NUMBER_MAPPING_FILENAME:
			s = dwarf_register_number_mapping_filename;
			break;
		default:
			s.clear();
			break;
	}
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
void ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::GetOption(Option opt, bool& flag)
{
	switch(opt)
	{
		case OPT_FORCE_BASE_ADDR:
			flag = force_base_addr;
			break;
		case OPT_FORCE_USE_VIRTUAL_ADDRESS:
			flag = force_use_virtual_address;
			break;
		case OPT_DUMP_HEADERS:
			flag = dump_headers;
			break;
		case OPT_VERBOSE:
			flag = verbose;
			break;
		case OPT_PARSE_DWARF:
			flag = parse_dwarf;
			break;
		default:
			flag = false;
			break;
	}
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
bool ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::Load()
{
	if(blob)
	{
		blob->Release();
	}

	Elf_Ehdr *hdr = 0;
	Elf_Phdr *phdr = 0;
	Elf_Phdr *phdr_table = 0;
	const Elf_Shdr *shdr = 0;
	Elf_Shdr *shdr_table = 0;
	char *sh_string_table = 0;
	int i;
	bool success = true;

	if(filename.empty())
	{
		logger << DebugError << "Don't know which executable file to load." << EndDebugError;
		return false;
	}
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << "Opening \"" << filename << "\"" << EndDebugInfo;
	}

	ifstream is(filename.c_str(), ifstream::in | ifstream::binary);
	
	if(is.fail())
	{
		logger << DebugError << "Can't open executable \"" << filename << "\"" << EndDebugError;
		return false;
	}
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << "Reading ELF header" << EndDebugInfo;
	}

	hdr = ReadElfHeader(is);

	if(!hdr)
	{
		logger << DebugError << "Could not read ELF header or \"" << filename << "\" is not an ELF file." << EndDebugError;
		return false;
	}

	switch(hdr->e_ident[EI_DATA])
	{
		case ELFDATA2LSB:
			endianness = E_LITTLE_ENDIAN;
			break;
		case ELFDATA2MSB:
			endianness = E_BIG_ENDIAN;
			break;
	}

	const char *architecture_name = GetArchitecture(hdr);
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << "File \"" << filename << "\" is for \"" << architecture_name << "\"" << EndDebugInfo;
	}

	phdr_table = ReadProgramHeaders(hdr, is);
	if(!phdr_table)
	{
		logger << DebugError << "Can't read program headers" << EndDebugError;
		free(hdr);
		return false;
	}
	
	shdr_table = ReadSectionHeaders(hdr, is);
	if(!shdr_table)
	{
		logger << DebugWarning << " Can't read section headers" << EndDebugWarning;
	}
	else
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << "Loading section header string table" << EndDebugInfo;
		}
		sh_string_table = LoadSectionHeaderStringTable(hdr, shdr_table, is);
		if(!sh_string_table)
		{
			logger << DebugWarning << " Can't load section header string table" << EndDebugWarning;
		}
	}

	if(dump_headers)
	{
		stringstream hdr_sstr;
		DumpElfHeader(hdr, hdr_sstr);
		logger << DebugInfo << hdr_sstr.str() << EndDebugInfo;
		for(i = 0, phdr = phdr_table; i < hdr->e_phnum; i++, phdr++)
		{
			stringstream phdr_sstr;
			DumpProgramHeader(phdr, phdr_sstr);
			logger << DebugInfo << phdr_sstr.str() << EndDebugInfo;
		}
		for(i = 0, shdr = shdr_table; i < hdr->e_shnum; shdr = GetNextSectionHeader(hdr, shdr),i++)
		{
			stringstream shdr_sstr;
			DumpSectionHeader(shdr, sh_string_table, shdr_sstr);
			logger << DebugInfo << shdr_sstr.str() << EndDebugInfo;
		}
	}

	// Check that forcing base address is possible
	MEMORY_ADDR unique_segment_addr = 0;
	if(force_base_addr)
	{
		unsigned int num_loadable_segment = 0;
		
		for(i = 0, phdr = phdr_table; i < hdr->e_phnum; phdr++, i++)
		{
			if(GetSegmentType(phdr) == PT_LOAD) /* Loadable Program Segment */
			{
				num_loadable_segment++;
				unique_segment_addr = GetSegmentAddr(phdr);
			}
		}
		
		if(num_loadable_segment > 1)
		{
			logger << DebugWarning << " More than one loadable segments...ignoring specified base address" << EndDebugWarning;
			force_base_addr = false;
		}
		else
		{
			if(unlikely(verbose))
			{
				logger << DebugInfo << "Forcing base address to 0x" << std::hex << base_addr << std::dec << EndDebugInfo;
			}
		}
	}
	
	MEMORY_ADDR entry_point = force_base_addr ? base_addr + hdr->e_entry - unique_segment_addr : hdr->e_entry;

	if(unlikely(verbose))
	{
		logger << DebugInfo << "Program entry point at 0x" << hex << entry_point << dec << EndDebugInfo;
	}

	blob = new typename unisim::util::debug::blob::Blob<MEMORY_ADDR>();
	blob->Catch();
	
	blob->SetEntryPoint(entry_point);
	blob->SetFilename(filename.c_str());
	blob->SetArchitecture(architecture_name);
	blob->SetEndian(endianness);
	blob->SetAddressSize(GetAddressSize(hdr));
	if(Elf_Class == ELFCLASS32)
		blob->SetFileFormat(unisim::util::debug::blob::Blob<MEMORY_ADDR>::FFMT_ELF32);
	else if(Elf_Class == ELFCLASS64)
		blob->SetFileFormat(unisim::util::debug::blob::Blob<MEMORY_ADDR>::FFMT_ELF64);

	if(shdr_table && sh_string_table)
	{
		for(i = 0, shdr = shdr_table; i < hdr->e_shnum; shdr = GetNextSectionHeader(hdr, shdr),i++)
		{
			//DumpSectionHeader(shdr, sh_string_table, cerr);

			MEMORY_ADDR sh_type = GetSectionType(shdr);
			
			MEMORY_ADDR sh_size = GetSectionSize(shdr);
			MEMORY_ADDR sh_addr = force_base_addr ? base_addr + (GetSectionAddr(shdr) - unique_segment_addr) : GetSectionAddr(shdr);
			MEMORY_ADDR sh_flags = GetSectionFlags(shdr);
			MEMORY_ADDR sh_alignment = GetSectionAlignment(shdr);
			MEMORY_ADDR sh_link = GetSectionLink(shdr);
			
			const char *sh_name = GetSectionName(shdr, sh_string_table);
			
			typename unisim::util::debug::blob::Section<MEMORY_ADDR>::Type section_type = unisim::util::debug::blob::Section<MEMORY_ADDR>::TY_UNKNOWN;
			switch(sh_type)
			{
				case SHT_NULL:
					section_type = unisim::util::debug::blob::Section<MEMORY_ADDR>::TY_NULL;
					sh_size = 0;
					sh_addr = 0;
					sh_flags = 0;
					sh_alignment = 0;
					sh_link = 0;
					break;
				case SHT_PROGBITS:
					section_type = unisim::util::debug::blob::Section<MEMORY_ADDR>::TY_PROGBITS;
					break;
				case SHT_SYMTAB:
					section_type = unisim::util::debug::blob::Section<MEMORY_ADDR>::TY_ELF_SYMTAB;
					break;
				case SHT_STRTAB:
					section_type = unisim::util::debug::blob::Section<MEMORY_ADDR>::TY_STRTAB;
					break;
				case SHT_NOBITS:
					section_type = unisim::util::debug::blob::Section<MEMORY_ADDR>::TY_NOBITS;
					break;
			}
			
			typename unisim::util::debug::blob::Section<MEMORY_ADDR>::Attribute section_attr = unisim::util::debug::blob::Section<MEMORY_ADDR>::SA_NULL;
			if(sh_flags & SHF_WRITE) section_attr = (typename unisim::util::debug::blob::Section<MEMORY_ADDR>::Attribute)(section_attr | unisim::util::debug::blob::Section<MEMORY_ADDR>::SA_W);
			if(sh_flags & SHF_ALLOC) section_attr = (typename unisim::util::debug::blob::Section<MEMORY_ADDR>::Attribute)(section_attr | unisim::util::debug::blob::Section<MEMORY_ADDR>::SA_A);
			if(sh_flags & SHF_EXECINSTR) section_attr = (typename unisim::util::debug::blob::Section<MEMORY_ADDR>::Attribute)(section_attr | unisim::util::debug::blob::Section<MEMORY_ADDR>::SA_X);
			
			if((sh_type != SHT_NULL) && verbose)
			{
				logger << DebugInfo << "Loading section " << sh_name;
				if(sh_flags & SHF_ALLOC) logger << " at 0x" << hex << sh_addr << dec;
				logger << " (" << sh_size << " bytes) ";
				logger << EndDebugInfo;
			}

			void *sh_data = (sh_size && (sh_type != SHT_NOBITS)) ? calloc(sh_size + 1, 1) : 0; // Allocate one additional byte for zero-terminated strings
			
			if((sh_type != SHT_NULL) && (sh_type != SHT_NOBITS))
			{
				if(!LoadSection(hdr, shdr, sh_data, is))
				{
					logger << DebugWarning << " Can't load section " << sh_name << EndDebugWarning;
					success = false;
				}
			}
			
			unisim::util::debug::blob::Section<MEMORY_ADDR> *section = new unisim::util::debug::blob::Section<MEMORY_ADDR>(
				section_type,
				section_attr,
				sh_name,
				sh_alignment,
				sh_link,
				sh_addr,
				sh_size,
				sh_data
			);
			
			blob->AddSection(section);
		}
	}

	for(i = 0, phdr = phdr_table; i < hdr->e_phnum; i++, phdr++)
	{
		if(GetSegmentType(phdr) == PT_LOAD) /* Loadable Program Segment */
		{
			MEMORY_ADDR ph_type = GetSegmentType(phdr);
			MEMORY_ADDR segment_addr = force_base_addr ? base_addr : GetSegmentAddr(phdr);
			MEMORY_ADDR segment_mem_size = GetSegmentMemSize(phdr);
			MEMORY_ADDR segment_file_size = GetSegmentFileSize(phdr);
			MEMORY_ADDR ph_flags = GetSegmentFlags(phdr);
			MEMORY_ADDR segment_alignment = GetSegmentAlignment(phdr);
			
		
			typename unisim::util::debug::blob::Segment<MEMORY_ADDR>::Type segment_type = unisim::util::debug::blob::Segment<MEMORY_ADDR>::TY_UNKNOWN;
			
			switch(ph_type)
			{
				case PT_LOAD:
					segment_type = unisim::util::debug::blob::Segment<MEMORY_ADDR>::TY_LOADABLE;
					break;
				default:
					segment_type = unisim::util::debug::blob::Segment<MEMORY_ADDR>::TY_UNKNOWN;
					break;
			}
			
			
			typename unisim::util::debug::blob::Segment<MEMORY_ADDR>::Attribute segment_attr = unisim::util::debug::blob::Segment<MEMORY_ADDR>::SA_NULL;
			if(ph_flags & PF_W) segment_attr = (typename unisim::util::debug::blob::Segment<MEMORY_ADDR>::Attribute)(segment_attr | unisim::util::debug::blob::Segment<MEMORY_ADDR>::SA_W);
			if(ph_flags & PF_R) segment_attr = (typename unisim::util::debug::blob::Segment<MEMORY_ADDR>::Attribute)(segment_attr | unisim::util::debug::blob::Segment<MEMORY_ADDR>::SA_R);
			if(ph_flags & PF_X) segment_attr = (typename unisim::util::debug::blob::Segment<MEMORY_ADDR>::Attribute)(segment_attr | unisim::util::debug::blob::Segment<MEMORY_ADDR>::SA_X);
			
			if(unlikely(verbose))
			{
				logger << DebugInfo << "Loading segment at 0x" << hex << segment_addr << dec << " (" << segment_file_size << " bytes) " << EndDebugInfo;
			}
			
			void *segment_data = calloc(segment_mem_size + 1, 1); // Allocate one additional byte for zero-terminated strings
			
			if(!LoadSegment(hdr, phdr, segment_data, is))
			{
				logger << DebugError << "Can't load segment" << EndDebugError;
				success = false;
			}

			unisim::util::debug::blob::Segment<MEMORY_ADDR> *segment = new unisim::util::debug::blob::Segment<MEMORY_ADDR>(
				segment_type,
				segment_attr,
				segment_alignment,
				segment_addr,
				segment_mem_size,
				segment_file_size,
				segment_data
			);
			
			blob->AddSegment(segment);
		}
	}

	if(sh_string_table) free(sh_string_table);
	if(shdr_table) free(shdr_table);
	if(phdr_table) free(phdr_table);
	if(hdr) free(hdr);
	
	if(const_blob)
	{
		const_blob->Release();
	}
	const_blob = blob;
	if(const_blob)
	{
		const_blob->Catch();
	}
	ParseSymbols();
	
	return success;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
void ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::ParseSymbols()
{
	if(dw_handler)
	{
		delete dw_handler;
	}
	
	if(symtab_handler)
	{
		delete symtab_handler;
	}

	symtab_handler = new ELF_SymtabHandler<MEMORY_ADDR, Elf_Sym>(logger, const_blob);
	
	if(symtab_handler)
	{
		if(unlikely(verbose))
		{
			logger << DebugInfo << "Building symbol table" << EndDebugInfo;
		}
		symtab_handler->Parse();
	}
	
	if(parse_dwarf)
	{
		dw_handler = new unisim::util::debug::dwarf::DWARF_Handler<MEMORY_ADDR>(const_blob, logger, regs_if, mem_if);

		if(dw_handler)
		{
			dw_handler->SetOption(unisim::util::debug::dwarf::OPT_REG_NUM_MAPPING_FILENAME, dwarf_register_number_mapping_filename.c_str());
			dw_handler->SetOption(unisim::util::debug::dwarf::OPT_VERBOSE, verbose);
			
			if(verbose)
			{
				logger << DebugInfo << "Parsing DWARF debugging informations" << EndDebugInfo;
			}
			dw_handler->Parse();
			if(!dwarf_to_html_output_directory.empty())
			{
				if(unlikely(verbose))
				{
					logger << DebugInfo << "Dumping DWARF debugging informations as HTML into directory " << dwarf_to_html_output_directory << EndDebugInfo;
				}
				dw_handler->to_HTML(dwarf_to_html_output_directory.c_str());
			}
		}
	}
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
const typename unisim::util::debug::blob::Blob<MEMORY_ADDR> *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::GetBlob() const
{
	return blob ? blob : const_blob;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
void ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::GetSymbols(typename std::list<const unisim::util::debug::Symbol<MEMORY_ADDR> *>& lst, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const
{
	if(symtab_handler)
	{
		symtab_handler->GetSymbols(lst, type);
	}
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::FindSymbol(const char *name, MEMORY_ADDR addr, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const
{
	return symtab_handler ? symtab_handler->FindSymbol(name, addr, type) : 0;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::FindSymbolByAddr(MEMORY_ADDR addr) const
{
	return symtab_handler ? symtab_handler->FindSymbolByAddr(addr) : 0;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::FindSymbolByName(const char *name) const
{
	return symtab_handler ? symtab_handler->FindSymbolByName(name) : 0;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::FindSymbolByName(const char *name, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const
{
	return symtab_handler ? symtab_handler->FindSymbolByName(name, type) : 0;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::FindSymbolByAddr(MEMORY_ADDR addr, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const
{
	return symtab_handler ? symtab_handler->FindSymbolByAddr(addr, type) : 0;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
void ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::SwapElfHeader(Elf_Ehdr *hdr)
{
	BSwap(hdr->e_type);
	BSwap(hdr->e_machine);
	BSwap(hdr->e_version);
	BSwap(hdr->e_entry);
	BSwap(hdr->e_phoff);
	BSwap(hdr->e_shoff);
	BSwap(hdr->e_flags);
	BSwap(hdr->e_ehsize);
	BSwap(hdr->e_phentsize);
	BSwap(hdr->e_phnum);
	BSwap(hdr->e_shentsize);
	BSwap(hdr->e_shnum);
	BSwap(hdr->e_shstrndx);
}


template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
void ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::SwapProgramHeader(Elf_Phdr *phdr)
{
	BSwap(phdr->p_type);
	BSwap(phdr->p_offset);
	BSwap(phdr->p_vaddr);
	BSwap(phdr->p_paddr);
	BSwap(phdr->p_filesz);
	BSwap(phdr->p_memsz);
	BSwap(phdr->p_flags);
	BSwap(phdr->p_align);
}


template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
void ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::SwapSectionHeader(Elf_Shdr *shdr)
{
	BSwap(shdr->sh_name);
	BSwap(shdr->sh_type);
	BSwap(shdr->sh_flags);
	BSwap(shdr->sh_addr);
	BSwap(shdr->sh_offset);
	BSwap(shdr->sh_size);
	BSwap(shdr->sh_link);
	BSwap(shdr->sh_info);
	BSwap(shdr->sh_addralign);
	BSwap(shdr->sh_entsize);
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
void ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::AdjustElfHeader(Elf_Ehdr *hdr)
{
	switch(hdr->e_ident[EI_DATA])
	{
		case ELFDATA2LSB:
#if BYTE_ORDER == BIG_ENDIAN
			SwapElfHeader(hdr);
#endif
			break;
		case ELFDATA2MSB:
#if BYTE_ORDER == LITTLE_ENDIAN
			SwapElfHeader(hdr);
#endif
			break;
	}
}


template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
void ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::AdjustProgramHeader(const Elf_Ehdr *hdr, Elf_Phdr *phdr)
{
	switch(hdr->e_ident[EI_DATA])
	{
		case ELFDATA2LSB:
#if BYTE_ORDER == BIG_ENDIAN
			SwapProgramHeader(phdr);
#endif
			break;
		case ELFDATA2MSB:
#if BYTE_ORDER == LITTLE_ENDIAN
			SwapProgramHeader(phdr);
#endif
			break;
	}
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
void ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::AdjustSectionHeader(const Elf_Ehdr *hdr, Elf_Shdr *shdr)
{
	switch(hdr->e_ident[EI_DATA])
	{
		case ELFDATA2LSB:
#if BYTE_ORDER == BIG_ENDIAN
			SwapSectionHeader(shdr);
#endif
			break;
		case ELFDATA2MSB:
#if BYTE_ORDER == LITTLE_ENDIAN
			SwapSectionHeader(shdr);
#endif
			break;
	}
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
Elf_Ehdr *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::ReadElfHeader(istream& is)
{
	if(is.seekg(0, ios::beg).fail())
	{
		return 0;
	}

	Elf_Ehdr *hdr = (Elf_Ehdr *) malloc(sizeof(Elf_Ehdr));

	if(is.read((char *) hdr, sizeof(Elf_Ehdr)).fail())
	{
		free(hdr);
		return 0;
	}

	if(hdr->e_ident[EI_MAG0] != ELFMAG0 ||
	   hdr->e_ident[EI_MAG1] != ELFMAG1 ||
	   hdr->e_ident[EI_MAG2] != ELFMAG2 ||
	   hdr->e_ident[EI_MAG3] != ELFMAG3)
	{
		free(hdr);
		return 0;
	}

	AdjustElfHeader(hdr);

	return hdr;
}


template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
Elf_Phdr *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::ReadProgramHeaders(const Elf_Ehdr *hdr, istream& is)
{
	if(hdr->e_phnum > 0)
	{
		int i;
		unsigned long sz;
		Elf_Phdr *phdr;

		if(is.seekg(hdr->e_phoff, ios::beg).fail())
		{
			return 0;
		}
		
		sz = hdr->e_phnum * hdr->e_phentsize;
		phdr = (Elf_Phdr *) malloc(sz);
		if(is.read((char *) phdr, sz).fail())
		{
			free(phdr);
			return 0;
		}
	
		for(i = 0; i < hdr->e_phnum; i++)
			AdjustProgramHeader(hdr, phdr + i);
	
		return phdr;
	}
	return 0;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
Elf_Shdr *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::ReadSectionHeaders(const Elf_Ehdr *hdr, istream& is)
{
	int i;
	if(hdr->e_shnum)
	{
		unsigned long sz = hdr->e_shnum * hdr->e_shentsize;
		Elf_Shdr *shdr = (Elf_Shdr *) malloc(sz);
	
		if(is.seekg(hdr->e_shoff, ios::beg).fail())
		{
			free(shdr);
			return 0;
		}
		if(is.read((char *) shdr, sz).fail())
		{
			free(shdr);
			return 0;
		}
	
		for(i = 0; i < hdr->e_shnum; i++)
			AdjustSectionHeader(hdr, shdr + i); // FIXME: "shdr + i" is not a way to walk through the table if hdr->e_shentsize != sizeof(Elf_Shdr)
	
		return shdr;
	}
	return 0;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
const Elf_Shdr *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::GetNextSectionHeader(const Elf_Ehdr *hdr, const Elf_Shdr *shdr)
{
	return (const Elf_Shdr *)((const char *) shdr + hdr->e_shentsize);
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
char *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::LoadSectionHeaderStringTable(const Elf_Ehdr *hdr, const Elf_Shdr *shdr_table, istream& is)
{
	const Elf_Shdr *shdr = (const Elf_Shdr *)((const char *) shdr_table + hdr->e_shentsize * hdr->e_shstrndx);
	char *string_table;

	if(is.seekg(shdr->sh_offset, ios::beg).fail())
	{
		return 0;
	}
	string_table = (char *) malloc(shdr->sh_size);
	if(is.read((char *) string_table, shdr->sh_size).fail())
	{
		free(string_table);
		return 0;
	}

	return string_table;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
void ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::DumpElfHeader(const Elf_Ehdr *hdr, ostream& os)
{
	os << "--- Elf Header ---";
	os << endl << "File Class : ";
	switch(hdr->e_ident[EI_CLASS])
	{
		case ELFCLASSNONE : os << "Invalid class"; break;
		case ELFCLASS32 : os << "32-bit objects"; break;
		case ELFCLASS64 : os << "64-bit objects"; break;
		default : os << "?"; break;
	}

	os << endl << "Data encoding : ";
	switch(hdr->e_ident[EI_DATA])
	{
		case ELFDATANONE : os << "Invalid data encoding"; break;
		case ELFDATA2LSB : os << "2's complement, little endian"; break;
		case ELFDATA2MSB : os << "2's complement, big endian"; break;
		default: os << "Unknown (" << hdr->e_ident[EI_DATA] << ")";
	}

	os << endl << "Version : " << (unsigned) hdr->e_ident[EI_VERSION];

	os << endl << "OS ABI identification : ";
	switch(hdr->e_ident[EI_OSABI])
	{
		case ELFOSABI_SYSV:os << "UNIX System V ABI"; break;
		case ELFOSABI_HPUX:os << "HP-UX"; break;
		case ELFOSABI_NETBSD:os << "NetBSD"; break;
		case ELFOSABI_LINUX:os << "Linux"; break;
		case ELFOSABI_SOLARIS:os << "Sun Solaris"; break;
		case ELFOSABI_MONTEREY:os << "Monterey"; break;
		case ELFOSABI_IRIX:os << "SGI Irix"; break;
		case ELFOSABI_FREEBSD:os << "FreeBSD"; break;
		case ELFOSABI_TRU64:os << "Compaq TRU64 UNIX"; break;
		case ELFOSABI_MODESTO:os << "Novell Modesto"; break;
		case ELFOSABI_OPENBSD:os << "OpenBSD"; break;
		case ELFOSABI_ARM:os << "ARM"; break;
		case ELFOSABI_STANDALONE:os << "Standalone (embedded) application"; break;
		default : os << "Unknown (" << hdr->e_ident[EI_OSABI] << ")";
	}

	os << endl << "Object File Type : ";

	if(hdr->e_type == ET_NONE) os << "No file type"; else
	if(hdr->e_type == ET_REL) os << "Relocatable file"; else
	if(hdr->e_type == ET_EXEC) os << "Executable file"; else
	if(hdr->e_type == ET_DYN) os << "Shared object file"; else
	if(hdr->e_type == ET_CORE) os << "Core file"; else
	if(hdr->e_type == ET_NUM) os << "Number of defined types"; else
	if(hdr->e_type >= ET_LOOS && hdr->e_type <= ET_HIOS) os << "OS-specific"; else
	//if(hdr->e_type >= ET_LOPROC && hdr->e_type <= ET_HIPROC) os << "Processor-specific"; else os << "Unknown (" << hdr->e_type << ")";
	if(hdr->e_type >= ET_LOPROC) os << "Processor-specific"; else os << "Unknown (" << hdr->e_type << ")";

	os << endl << "Target machine : ";

	switch(hdr->e_machine)
	{
		case EM_NONE: os << "No machine"; break;
		case EM_M32: os << "AT&T WE 32100"; break;
		case EM_SPARC: os << "SUN SPARC"; break;
		case EM_386: os << "Intel 80386"; break;
		case EM_68K: os << "Motorola m68k family"; break;
		case EM_88K: os << "Motorola m88k family"; break;
		case EM_860: os << "Intel 80860"; break;
		case EM_MIPS: os << "MIPS R3000 big-endian"; break;
		case EM_S370: os << "IBM System/370"; break;
		case EM_MIPS_RS4_BE: os << "MIPS R4000 Big-Endian"; break;
	
		case EM_PARISC: os << "HPPA"; break;
		case EM_VPP500: os << "Fujitsu VPP500"; break;
		case EM_SPARC32PLUS: os << "Sun's \"v8plus\""; break;
		case EM_960: os << "Intel 80960"; break;
		case EM_PPC: os << "PowerPC"; break;
		case EM_PPC64: os << "PowerPC 64-bit"; break;
		case EM_S390: os << "IBM S390"; break;
	
		case EM_V800: os << "NEC V800 series"; break;
		case EM_FR20: os << "Fujitsu FR20"; break;
		case EM_RH32: os << "TRW RH-32"; break;
		case EM_RCE: os << "Motorola RCE"; break;
		case EM_ARM: os << "ARM"; break;
		case EM_FAKE_ALPHA: os << "Digital Alpha"; break;
		case EM_SH: os << "Hitachi SH"; break;
		case EM_SPARCV9: os << "SPARC v9 64-bit"; break;
		case EM_TRICORE: os << "Siemens Tricore"; break;
		case EM_ARC: os << "Argonaut RISC Core"; break;
		case EM_H8_300: os << "Hitachi H8/300"; break;
		case EM_H8_300H: os << "Hitachi H8/300H"; break;
		case EM_H8S: os << "Hitachi H8S"; break;
		case EM_H8_500: os << "Hitachi H8/500"; break;
		case EM_IA_64: os << "Intel Merced"; break;
		case EM_MIPS_X: os << "Stanford MIPS-X"; break;
		case EM_COLDFIRE: os << "Motorola Coldfire"; break;
		case EM_68HC12: os << "Motorola M68HC12"; break;
		case EM_MMA: os << "Fujitsu MMA Multimedia Accelerator"; break;
		case EM_PCP: os << "Siemens PCP"; break;
		case EM_NCPU: os << "Sony nCPU embeeded RISC"; break;
		case EM_NDR1: os << "Denso NDR1 microprocessor"; break;
		case EM_STARCORE: os << "Motorola Start*Core processor"; break;
		case EM_ME16: os << "Toyota ME16 processor"; break;
		case EM_ST100: os << "STMicroelectronic ST100 processor"; break;
		case EM_TINYJ: os << "Advanced Logic Corp. Tinyj emb.fam"; break;
		case EM_X86_64: os << "AMD x86-64 architecture"; break;
		case EM_PDSP: os << "Sony DSP Processor"; break;

		case EM_FX66: os << "Siemens FX66 microcontroller"; break;
		case EM_ST9PLUS: os << "STMicroelectronics ST9+ 8/16 mc"; break;
		case EM_ST7: os << "STmicroelectronics ST7 8 bit mc"; break;
		case EM_68HC16: os << "Motorola MC68HC16 microcontroller"; break;
		case EM_68HC11: os << "Motorola MC68HC11 microcontroller"; break;
		case EM_68HC08: os << "Motorola MC68HC08 microcontroller"; break;
		case EM_68HC05: os << "Motorola MC68HC05 microcontroller"; break;
		case EM_SVX: os << "Silicon Graphics SVx"; break;
		case EM_ST19: os << "STMicroelectronics ST19 8 bit mc"; break;
		case EM_VAX: os << "Digital VAX"; break;
		case EM_CRIS: os << "Axis Communications 32-bit embedded processor"; break;
		case EM_JAVELIN: os << "Infineon Technologies 32-bit embedded processor"; break;
		case EM_FIREPATH: os << "Element 14 64-bit DSP Processor"; break;
		case EM_ZSP: os << "LSI Logic 16-bit DSP Processor"; break;
		case EM_MMIX: os << "Donald Knuth's educational 64-bit processor"; break;
		case EM_HUANY: os << "Harvard University machine-independent object files"; break;
		case EM_PRISM: os << "SiTera Prism"; break;
		case EM_AVR: os << "Atmel AVR 8-bit microcontroller"; break;
		case EM_FR30: os << "Fujitsu FR30"; break;
		case EM_D10V: os << "Mitsubishi D10V"; break;
		case EM_D30V: os << "Mitsubishi D30V"; break;
		case EM_V850: os << "NEC v850"; break;
		case EM_M32R: os << "Mitsubishi M32R"; break;
		case EM_MN10300: os << "Matsushita MN10300"; break;
		case EM_MN10200: os << "Matsushita MN10200"; break;
		case EM_PJ: os << "picoJava"; break;
		case EM_OPENRISC: os << "OpenRISC 32-bit embedded processor"; break;
		case EM_ARC_A5: os << "ARC Cores Tangent-A5"; break;
		case EM_XTENSA: os << "Tensilica Xtensa Architecture"; break;
		case EM_ALPHA: os << "Alpha"; break;
		default: os << "Unknown (" << hdr->e_machine << ")";
	}
	os << endl;
	os << endl << "Number of program headers : " << hdr->e_phnum;

	os << endl << "Number of section headers : " << hdr->e_shnum;

	os << endl << "section headers file offset : " << hdr->e_shoff;
	os << endl << "section header size : " << hdr->e_shentsize << " bytes";

	os << endl;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
void ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::DumpProgramHeader(const Elf_Phdr *phdr, ostream& os)
{
	os << "--- Program Header ---";
	os << endl << "Segment type : ";
	switch(phdr->p_type)
	{
		case PT_NULL: os << "Program header table entry unused"; break;
		case PT_LOAD: os << "Loadable program segment"; break;
		case PT_DYNAMIC: os << "Dynamic linking information"; break;
		case PT_INTERP: os << "Program interpreter"; break;
		case PT_NOTE: os << "Auxiliary information"; break;
		case PT_SHLIB: os << "Reserved (SHLIB)"; break;
		case PT_PHDR: os << "Entry for header table itself"; break;
		case PT_TLS: os << "Thread-local storage segment"; break;
		case PT_NUM: os << "Number of defined types"; break;
		case PT_GNU_EH_FRAME: os << "GCC .eh_frame_hdr segment"; break;
		case PT_GNU_STACK: os << "Stack executability"; break;
		case PT_SUNWBSS: os << "Sun Specific segment"; break;
		case PT_SUNWSTACK: os << "Stack segment"; break;
		case PT_HISUNW: os << "HiSUNW"; break;
		default: os << "Unknown (" << phdr->p_type << ")";
	}

	os << endl << "File offset : " << phdr->p_offset;
	os << endl << "Virtual Address : 0x" << hex << phdr->p_vaddr;
	os << endl << "Physical Address : 0x" << phdr->p_paddr << dec;
	os << endl << "File size : " << phdr->p_filesz;
	os << endl << "Memory size : " << phdr->p_memsz;
	os << endl << "Segment flags (rwx) : ";
	os << (phdr->p_flags & PF_R ? "r" : "-");
	os << (phdr->p_flags & PF_W ? "w" : "-");
	os << (phdr->p_flags & PF_X ? "x" : "-");
	os << std::hex << " (0x" << phdr->p_flags << ")" << std::dec;
//	os << (phdr->p_flags & PF_MASKOS ? " [OS-specific flags]" : "");
//	os << (phdr->p_flags & PF_MASKPROC ? " [Processor-specific flags]" : "");

	os << endl << "Memory/File alignment : " << phdr->p_align;

	os << endl;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
void ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::DumpSectionHeader(const Elf_Shdr *shdr, const char *string_table, ostream& os)
{
	os << "--- Section header ---";
	os << endl << "Section name : \"" << (string_table ? (string_table + shdr->sh_name) : "unknown")<< "\"";
	os << endl << "Section type : ";

	switch(shdr->sh_type)
	{
		case SHT_NULL:  os << "Unused"; break;
		case SHT_PROGBITS:  os << "Program data"; break;
		case SHT_SYMTAB:  os << "Symbol table"; break;
		case SHT_STRTAB: os << "String table"; break;
		case SHT_RELA: os << "Relocation entries with addends"; break;
		case SHT_HASH: os << "Symbol hash table"; break;
		case SHT_DYNAMIC: os << "Dynamic linking information"; break;
		case SHT_NOTE: os << "Notes"; break;
		case SHT_NOBITS: os << "Program space with no data (bss)"; break;
		case SHT_REL: os << "Relocation entries, no addends"; break;
		case SHT_SHLIB: os << "Reserved (SHLIB)"; break;
		case SHT_DYNSYM: os << "Dynamic linker symbol table"; break;
		case SHT_INIT_ARRAY: os << "Array of constructors"; break;
		case SHT_FINI_ARRAY: os << "Array of destructors"; break;
		case SHT_PREINIT_ARRAY: os << "Array of pre-constructors"; break;
		case SHT_GROUP: os << "Section group"; break;
		case SHT_SYMTAB_SHNDX: os << "Extended section indeces"; break;
		case SHT_NUM: os << "Number of defined types"; break;
		case SHT_GNU_LIBLIST: os << "Prelink library list"; break;
		case SHT_CHECKSUM:os << "Checksum for DSO content"; break;
		case SHT_SUNW_move:os << "SUNW move"; break;
		case SHT_SUNW_COMDAT:os << "SUNW COMDAT"; break;
		case SHT_SUNW_syminfo:os << "SUNW syminfo"; break;
		case SHT_GNU_verdef:os << "Version definition section"; break;
		case SHT_GNU_verneed:os << "Version needs section"; break;
		case SHT_GNU_versym:os << "Version symbol table"; break;
		default : os << "Unknown";
	}

	os << endl << "Link to section :" << shdr->sh_link;
	
	os << endl << "Section flags : " << shdr->sh_flags;

	if(!shdr->sh_flags) os << " (none)";
	if(shdr->sh_flags & SHF_WRITE)	os << endl << "  - Writable";
	if(shdr->sh_flags & SHF_ALLOC)	os << endl << "  - Occupies memory during execution";
	if(shdr->sh_flags & SHF_EXECINSTR)	os << endl << "  - Executable";
	if(shdr->sh_flags & SHF_MERGE)	os << endl << "  - Might be merged";
	if(shdr->sh_flags & SHF_STRINGS)	os << endl << "  - Contains nul-terminated strings";
	if(shdr->sh_flags & SHF_INFO_LINK)	os << endl << "  - `sh_info' contains SHT index";
	if(shdr->sh_flags & SHF_LINK_ORDER)	os << endl << "  - Preserve order after combining";
	if(shdr->sh_flags & SHF_OS_NONCONFORMING)	os << endl << "  - Non-standard OS specific handling required";
	if(shdr->sh_flags & SHF_GROUP)	os << endl << "  - Section is member of a group.";
	if(shdr->sh_flags & SHF_TLS)	os << endl << "  - Section hold thread-local data.";
	if(shdr->sh_flags & SHF_MASKOS) os << endl << "  - OS-specific.";
	if(shdr->sh_flags & SHF_MASKPROC) os << endl << "  - Processor-specific";

	os << endl << "File offset : " << shdr->sh_offset;
	os << endl << "Size : " << shdr->sh_size << " bytes";
	os << endl << "Virtual Address : 0x" << hex << shdr->sh_addr << dec;
	os << endl << "Section alignment : " << shdr->sh_addralign;

	os << endl;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
void ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::DumpSymbol(const Elf_Sym *sym, const char *string_table, ostream& os)
{
	os << "---- Symbol ---";
	os << endl << "Name : \"" << (string_table + sym->st_name) << "\"";
	os << endl << "Value : 0x" << hex << sym->st_value << dec;
	os << endl << "Size : " << sym->st_size;
	os << endl << "Binding : ";
	switch(ELF32_ST_BIND(unisim::util::endian::Target2Host(endianness, sym->st_info)))
	{
		case STB_LOCAL: os << " Local symbol"; break;
		case STB_GLOBAL: os << " Global symbol"; break;
		case STB_WEAK: os << " Weak symbol"; break;
		default: os << "Unknown";
	}
	os << endl << "Type : ";
	switch(ELF32_ST_TYPE(unisim::util::endian::Target2Host(endianness, sym->st_info)))
	{
		case STT_NOTYPE: os << "Symbol type is unspecified"; break;
		case STT_OBJECT: os << "Symbol is a data object"; break;
		case STT_FUNC: os << "Symbol is a code object"; break;
		case STT_SECTION: os << "Symbol associated with a section"; break;
		case STT_FILE: os << "Symbol's name is file name"; break;
		case STT_COMMON: os << "Symbol is a common data object"; break;
		case STT_TLS: os << "Symbol is thread-local data object"; break;
		default: os << "Unknown";
	}
	os << endl << "Visibility : ";
	switch(ELF_ST_VISIBILITY(unisim::util::endian::Target2Host(endianness, sym->st_other)))
	{
		case STV_DEFAULT: os << "Default symbol visibility rules"; break;
		case STV_INTERNAL: os << "Processor specific hidden class"; break;
		case STV_HIDDEN: os << "Sym unavailable in other modules"; break;
		case STV_PROTECTED: os << "Not preemptible, not exported"; break;
		default: os << "Unknown";
	}
	os << endl << "Section index : " << unisim::util::endian::Target2Host(endianness, sym->st_shndx);
	os << endl;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
void ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::DumpSymbolTable(const Elf_Shdr *shdr, const char *content, const char *string_table, ostream& os)
{
	int nsymbols = shdr->sh_size / sizeof(Elf_Sym);
	int i;
	Elf_Sym *sym;

	os << "---- Symbol Table Content ---" << endl;
	for(sym = (Elf_Sym *) content, i = 0; i < nsymbols; sym++, i++)
		DumpSymbol(sym, string_table, os);
}


template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
MEMORY_ADDR ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::GetSectionSize(const Elf_Shdr *shdr)
{
	return shdr->sh_size;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
MEMORY_ADDR ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::GetSectionAddr(const Elf_Shdr *shdr)
{
	return shdr->sh_addr;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
MEMORY_ADDR ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::GetSectionType(const Elf_Shdr *shdr)
{
	return shdr->sh_type;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
MEMORY_ADDR ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::GetSectionAlignment(const Elf_Shdr *shdr)
{
	return shdr->sh_addralign;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
MEMORY_ADDR ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::GetSectionLink(const Elf_Shdr *shdr)
{
	return shdr->sh_link;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
bool ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::LoadSection(const Elf_Ehdr *hdr, const Elf_Shdr *shdr, void *buffer, istream& is)
{
	if(is.seekg(shdr->sh_offset, ios::beg).fail()) return false;
	if(is.read((char *) buffer, shdr->sh_size).fail()) return false;
/*	if(shdr->sh_type == SHT_SYMTAB)
	{
		AdjustSymbolTable(hdr, shdr, (Elf_Sym *) buffer);
	}*/
	return true;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
MEMORY_ADDR ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::GetSegmentType(const Elf_Phdr *phdr)
{
	return phdr->p_type;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
MEMORY_ADDR ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::GetSegmentFlags(const Elf_Phdr *phdr)
{
	return phdr->p_flags;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
MEMORY_ADDR ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::GetSegmentMemSize(const Elf_Phdr *phdr)
{
	return phdr->p_memsz;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
MEMORY_ADDR ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::GetSegmentFileSize(const Elf_Phdr *phdr)
{
	return phdr->p_filesz;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
MEMORY_ADDR ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::GetSegmentAddr(const Elf_Phdr *phdr)
{
	return force_use_virtual_address ? phdr->p_vaddr : phdr->p_paddr;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
MEMORY_ADDR ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::GetSegmentAlignment(const Elf_Phdr *phdr)
{
	return phdr->p_align;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
bool ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::LoadSegment(const Elf_Ehdr *hdr, const Elf_Phdr *phdr, void *buffer, istream& is)
{
	if(is.seekg(phdr->p_offset, ios::beg).fail()) return false;
	if(is.read((char *) buffer, phdr->p_filesz).fail()) return false;
	return true;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
MEMORY_ADDR ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::GetSectionFlags(const Elf_Shdr *shdr)
{
	return shdr->sh_flags;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
const char *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::GetSectionName(const Elf_Shdr *shdr, const char *string_table)
{
	return string_table ? string_table + shdr->sh_name : "unknown";
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
void ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::DumpRawData(const void *content, MEMORY_ADDR size)
{
	int i, j;
	MEMORY_ADDR offset = 0;
	streamsize width = cout.width();

	cout.fill(' ');
	cout.width(2 * sizeof(offset));
	cout << "offset" << hex;
	cout.width(0);
	for(i = 0; i < 16; i++)
	{
		cout << "  " << i;
	}
	cout << endl;
	
	while(offset < size)
	{
		cout << hex; cout.fill('0'); cout.width(2 * sizeof(offset)); cout << offset << " "; cout.fill(' ');
		for(j = 0; j < 16; j++, offset++)
		{
			if(offset < size)
			{
				uint8_t value = ((uint8_t *) content)[offset];
				cout << (uint32_t)(value >> 4);
				cout << (uint32_t)(value & 15);
			}
			else
			{
				cout << "  ";
			}
			if(j < 15) cout << " ";
		}
		offset -= 16;
		cout << dec << "  ";
		for(j = 0; j < 16; j++, offset++)
		{
			if(offset < size)
			{
				uint8_t value = ((uint8_t *) content)[offset];
				cout << (char)((value >= ' ' && value < 128) ? value : '.');
			}
			else
			{
				cout << ' ';
			}
		}
		cout << endl;
	}
	cout.width(width);
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
const std::map<MEMORY_ADDR, const Statement<MEMORY_ADDR> *>& ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::GetStatements() const
{
	return dw_handler ? dw_handler->GetStatements() : no_stmts;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
const unisim::util::debug::Statement<MEMORY_ADDR> *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::FindStatement(MEMORY_ADDR addr, typename unisim::service::interfaces::StatementLookup<MEMORY_ADDR>::FindStatementOption opt) const
{
	return dw_handler ? dw_handler->FindStatement(addr, opt) : 0;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
const unisim::util::debug::Statement<MEMORY_ADDR> *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::FindStatement(const char *filename, unsigned int lineno, unsigned int colno) const
{
	return dw_handler ? dw_handler->FindStatement(filename, lineno, colno) : 0;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
std::vector<MEMORY_ADDR> *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::GetBackTrace(MEMORY_ADDR pc) const
{
	return dw_handler ? dw_handler->GetBackTrace(pc) : 0;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
bool ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::GetReturnAddress(MEMORY_ADDR pc, MEMORY_ADDR& ret_addr) const
{
	return dw_handler ? dw_handler->GetReturnAddress(pc, ret_addr) : false;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
const char *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::GetArchitecture(const Elf_Ehdr *hdr) const
{
	switch(hdr->e_machine)
	{
		case EM_NONE: return "";
		case EM_M32: return "m32";
		case EM_SPARC: return "sparc";
		case EM_386: return "i386";
		case EM_68K: return "68k";
		case EM_88K: return "88k";
		case EM_860: return "860";
		case EM_MIPS: return "mips";
		case EM_S370: return "s370";
		case EM_MIPS_RS4_BE: return "mips_rs4_be";
	
		case EM_PARISC: return "parisc";
		case EM_VPP500: return "vpp500";
		case EM_SPARC32PLUS: return "sparc32+";
		case EM_960: return "960";
		case EM_PPC: return "powerpc";
		case EM_PPC64: return "powerpc64";
		case EM_S390: return "s390";
	
		case EM_V800: return "v800";
		case EM_FR20: return "fr20";
		case EM_RH32: return "rh32";
		case EM_RCE: return "rce";
		case EM_ARM: return "arm";
		case EM_FAKE_ALPHA: return "fake_alpha";
		case EM_SH: return "sh";
		case EM_SPARCV9: return "sparcv9";
		case EM_TRICORE: return "tricore";
		case EM_ARC: return "arc";
		case EM_H8_300: return "h8_300";
		case EM_H8_300H: return "h8_300h";
		case EM_H8S: return "h8s";
		case EM_H8_500: return "h8_500";
		case EM_IA_64: return "ia64";
		case EM_MIPS_X: return "mips_x";
		case EM_COLDFIRE: return "coldfire";
		case EM_68HC12: return "68hc12";
		case EM_MMA: return "mma";
		case EM_PCP: return "pcp";
		case EM_NCPU: return "ncpu";
		case EM_NDR1: return "ndr1";
		case EM_STARCORE: return "starcore";
		case EM_ME16: return "me16";
		case EM_ST100: return "st100";
		case EM_TINYJ: return "tinyj";
		case EM_X86_64: return "x86_64";
		case EM_PDSP: return "pdsp";

		case EM_FX66: return "fx66";
		case EM_ST9PLUS: return "st9+";
		case EM_ST7: return "st7";
		case EM_68HC16: return "68hc16";
		case EM_68HC11: return "68hc11";
		case EM_68HC08: return "68hc08";
		case EM_68HC05: return "68hc05";
		case EM_SVX: return "svx";
		case EM_ST19: return "st19";
		case EM_VAX: return "vax";
		case EM_CRIS: return "cris";
		case EM_JAVELIN: return "javelin";
		case EM_FIREPATH: return "firepath";
		case EM_ZSP: return "zsp";
		case EM_MMIX: return "mmix";
		case EM_HUANY: return "huany";
		case EM_PRISM: return "prism";
		case EM_AVR: return "avr";
		case EM_FR30: return "fr30";
		case EM_D10V: return "d10v";
		case EM_D30V: return "d30v";
		case EM_V850: return "v850";
		case EM_M32R: return "m32r";
		case EM_MN10300: return "mn10300";
		case EM_MN10200: return "mn101200";
		case EM_PJ: return "pj";
		case EM_OPENRISC: return "openrisc";
		case EM_ARC_A5: return "arc_a5";
		case EM_XTENSA: return "xtensa";
		case EM_ALPHA: return "alpha";
	}
	return "";
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
uint8_t ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::GetAddressSize(const Elf_Ehdr *hdr) const
{
	switch(hdr->e_machine)
	{
		case EM_NONE: return sizeof(uint32_t);
		case EM_M32: return sizeof(uint32_t);
		case EM_SPARC: return sizeof(uint32_t);
		case EM_386: return sizeof(uint32_t);
		case EM_68K: return sizeof(uint32_t);
		case EM_88K: return sizeof(uint32_t);
		case EM_860: return sizeof(uint32_t);
		case EM_MIPS: return sizeof(uint32_t);
		case EM_S370: return sizeof(uint32_t);
		case EM_MIPS_RS4_BE: return sizeof(uint32_t);
	
		case EM_PARISC: return sizeof(uint32_t);
		case EM_VPP500: return sizeof(uint32_t);
		case EM_SPARC32PLUS: return sizeof(uint32_t);
		case EM_960: return sizeof(uint32_t);
		case EM_PPC: return sizeof(uint32_t);
		case EM_PPC64: return sizeof(uint64_t);
		case EM_S390: return sizeof(uint32_t);
	
		case EM_V800: return sizeof(uint32_t);
		case EM_FR20: return sizeof(uint32_t);
		case EM_RH32: return sizeof(uint32_t);
		case EM_RCE: return sizeof(uint32_t);
		case EM_ARM: return sizeof(uint32_t);
		case EM_FAKE_ALPHA: return sizeof(uint64_t);
		case EM_SH: return sizeof(uint32_t);
		case EM_SPARCV9: return sizeof(uint64_t);
		case EM_TRICORE: return sizeof(uint32_t);
		case EM_ARC: return sizeof(uint32_t);
		case EM_H8_300: return sizeof(uint32_t);
		case EM_H8_300H: return sizeof(uint32_t);
		case EM_H8S: return sizeof(uint32_t);
		case EM_H8_500: return sizeof(uint32_t);
		case EM_IA_64: return sizeof(uint64_t);
		case EM_MIPS_X: return sizeof(uint32_t);
		case EM_COLDFIRE: return sizeof(uint32_t);
		case EM_68HC12: return sizeof(uint16_t);
		case EM_MMA: return sizeof(uint32_t);
		case EM_PCP: return sizeof(uint32_t);
		case EM_NCPU: return sizeof(uint32_t);
		case EM_NDR1: return sizeof(uint32_t);
		case EM_STARCORE: return sizeof(uint32_t);
		case EM_ME16: return sizeof(uint32_t);
		case EM_ST100: return sizeof(uint32_t);
		case EM_TINYJ: return sizeof(uint32_t);
		case EM_X86_64: return sizeof(uint64_t);
		case EM_PDSP: return sizeof(uint32_t);

		case EM_FX66: return sizeof(uint32_t);
		case EM_ST9PLUS: return sizeof(uint32_t);
		case EM_ST7: return sizeof(uint32_t);
		case EM_68HC16: return sizeof(uint16_t);
		case EM_68HC11: return sizeof(uint16_t);
		case EM_68HC08: return sizeof(uint16_t);
		case EM_68HC05: return sizeof(uint16_t);
		case EM_SVX: return sizeof(uint32_t);
		case EM_ST19: return sizeof(uint32_t);
		case EM_VAX: return sizeof(uint32_t);
		case EM_CRIS: return sizeof(uint32_t);
		case EM_JAVELIN: return sizeof(uint32_t);
		case EM_FIREPATH: return sizeof(uint64_t);
		case EM_ZSP: return sizeof(uint16_t);
		case EM_MMIX: return sizeof(uint64_t);
		case EM_HUANY: return sizeof(uint32_t);
		case EM_PRISM: return sizeof(uint32_t);
		case EM_AVR: return sizeof(uint32_t);
		case EM_FR30: return sizeof(uint32_t);
		case EM_D10V: return sizeof(uint32_t);
		case EM_D30V: return sizeof(uint32_t);
		case EM_V850: return sizeof(uint32_t);
		case EM_M32R: return sizeof(uint32_t);
		case EM_MN10300: return sizeof(uint32_t);
		case EM_MN10200: return sizeof(uint32_t);
		case EM_PJ: return sizeof(uint32_t);
		case EM_OPENRISC: return sizeof(uint32_t);
		case EM_ARC_A5: return sizeof(uint32_t);
		case EM_XTENSA: return sizeof(uint32_t);
		case EM_ALPHA: return sizeof(uint64_t);
	}
	return sizeof(uint32_t);
}

} // end of namespace elf_loader
} // end of namespace loader
} // end of namespace util
} // end of namespace unisim

#endif
