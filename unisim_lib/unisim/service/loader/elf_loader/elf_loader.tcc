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
 
#ifndef __UNISIM_SERVICE_LOADER_ELF_LOADER_ELF_LOADER_TCC__
#define __UNISIM_SERVICE_LOADER_ELF_LOADER_ELF_LOADER_TCC__

#include <unisim/util/endian/endian.hh>
#include <iostream>
#include <fstream>
#include <stdlib.h>

namespace unisim {
namespace service {
namespace loader {
namespace elf_loader {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;


template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::ElfLoaderImpl(const char *name, Object *parent) :
	Object(name, parent, "ELF Loader"),
	Client<Memory<MEMORY_ADDR> >(name, parent),
	Client<Registers>(name, parent),
	Service<SymbolTableLookup<MEMORY_ADDR> >(name, parent),
	Service<Loader<MEMORY_ADDR> >(name, parent),
	Service<StatementLookup<MEMORY_ADDR> >(name, parent),
	memory_import("memory-import", this),
	registers_import("registers-import", this),
	symbol_table_lookup_export("symbol-table-lookup-export", this),
	loader_export("loader-export", this),
	stmt_lookup_export("stmt-lookup-export", this),
	filename(),
	entry_point(0),
	base_addr(0),
	top_addr(0),
	force_use_virtual_address(false),
	dump_headers(false),
	dw_handler(0),
	logger(*this),
	verbose(false),
	param_filename("filename", this, filename, "the ELF filename to load into memory"),
	param_base_addr("base-addr", this, base_addr, "if not null, a forced base address for a unique program segment"),
	param_force_use_virtual_address("force-use-virtual-address", this, force_use_virtual_address, "force use of virtual addresses instead of physical addresses"),
	param_dump_headers("dump-headers", this, dump_headers, "dump headers while loading ELF file"),
	param_verbose("verbose", this, verbose, "enable/disable verbosity")
{
	Object::SetupDependsOn(memory_import);
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::~ElfLoaderImpl()
{
	if(dw_handler)
	{
		delete dw_handler;
	}
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
void ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::OnDisconnect()
{
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
MEMORY_ADDR ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::GetEntryPoint() const
{
	return (MEMORY_ADDR) entry_point;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
MEMORY_ADDR ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::GetTopAddr() const
{
	return (MEMORY_ADDR) top_addr;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
MEMORY_ADDR ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::GetStackBase() const
{
  return 0;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
bool ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::Load(const char *_filename)
{
	if(memory_import) memory_import->Reset();
	filename = _filename;
	return Load();
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
bool ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::Load()
{
	symbol_table.Reset();

	Elf_Ehdr *hdr = 0;
	Elf_Phdr *phdr = 0;
	const Elf_Shdr *shdr = 0;
	Elf_Shdr *shdr_table = 0;
	char *sh_string_table = 0;
	MEMORY_ADDR section_size, section_type, section_flags, section_vaddr;
	void *section;
	const char *section_name;
	int i, j;
	bool success = true;

	if(filename.empty()) return true;
	
	ifstream is(Object::GetSimulator()->SearchSharedDataFile(filename.c_str()).c_str(), ifstream::in | ifstream::binary);
	if(is.fail())
	{
		logger << DebugError << "Can't open executable \"" << filename << "\"" << EndDebugError;
		return false;
	}
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << "Opening \"" << filename << "\"" << EndDebugInfo;
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

	if(unlikely(verbose))
	{
		logger << DebugInfo << "Reading ELF header" << EndDebugInfo;
	}

	phdr = ReadProgramHeaders(hdr, is);
	if(!phdr)
	{
		logger << DebugError << "Can't read program headers" << EndDebugError;
		free(hdr);
		return false;
	}
	
	entry_point = hdr->e_entry;

	shdr_table = ReadSectionHeaders(hdr, is);
	if(!shdr_table)
	{
		logger << DebugWarning << "Can't read section headers" << EndDebugWarning;
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
			logger << DebugWarning << "Can't load section header string table" << EndDebugWarning;
		}
	}

	if(dump_headers)
	{
		stringstream hdr_sstr;
		DumpElfHeader(hdr, hdr_sstr);
		logger << DebugInfo << hdr_sstr.str() << EndDebugInfo;
		for(i = 0; i < hdr->e_phnum; i++)
		{
			stringstream phdr_sstr;
			DumpProgramHeader(&phdr[i], phdr_sstr);
			logger << DebugInfo << phdr_sstr.str() << EndDebugInfo;
		}
		for(i = 0, shdr = shdr_table; i < hdr->e_shnum; shdr = GetNextSectionHeader(hdr, shdr),i++)
		{
			stringstream shdr_sstr;
			DumpSectionHeader(shdr, sh_string_table, shdr_sstr);
			logger << DebugInfo << shdr_sstr.str() << EndDebugInfo;
		}
	}

	top_addr = 0;
					
	if(memory_import)
	{
		if(base_addr)
		{
			unsigned int num_loadable_segment = 0;
			
			for(i = 0; i < hdr->e_phnum; i++)
			{
				if(phdr[i].p_type == PT_LOAD) /* Loadable Program Segment */
				{
					num_loadable_segment++;
				}
			}
			if(num_loadable_segment > 1)
			{
				logger << DebugWarning << "More than one loadable segments...ignoring specified base address" << EndDebugWarning;
				base_addr = 0;
			}
			else
			{
				entry_point = base_addr;
			}
		}

		for(i = 0; i < hdr->e_phnum; i++)
		{
			if(phdr[i].p_type == PT_LOAD) /* Loadable Program Segment */
			{
				MEMORY_ADDR segment_addr = 0;
				MEMORY_ADDR segment_size = 0;
				
				if(base_addr)
				{
					segment_addr = base_addr;
				}
				else
				{
					segment_addr = force_use_virtual_address ? phdr[i].p_vaddr : phdr[i].p_paddr;
				}
				
				segment_size = phdr[i].p_memsz;
				
				if(unlikely(verbose))
				{
					logger << DebugInfo << "Loading segment at 0x" << hex << segment_addr << dec << " (" << phdr[i].p_filesz << " bytes) " << EndDebugInfo;
				}
				
				if(segment_addr + segment_size > top_addr)
				{
					top_addr = segment_addr + segment_size;
				}

				char *segment = (char *) calloc(phdr[i].p_memsz, 1);
				
				if(!LoadSegment(hdr, &phdr[i], segment, is))
				{
					logger << DebugError << "Can't load segment" << EndDebugError;
					success = false;
				}
				else
				{
					if(segment_size > 0) 
					{
						if(!memory_import->WriteMemory(segment_addr, segment, segment_size))
						{
							logger << DebugError << "Can't write into memory (@0x" << hex << segment_addr << " - @0x" << (segment_addr +  segment_size - 1) << dec << ")" << EndDebugError;
							success = false;
						}
						else 
						{
							if(unlikely(verbose))
							{
								logger << DebugInfo << "Write into memory (@0x" << hex << segment_addr << " - @0x" << (segment_addr +  segment_size - 1) << dec << ")" << EndDebugInfo;
							}
						}
					}
				}

				free(segment);

				if(unlikely(verbose))
				{
					if(shdr_table && sh_string_table)
					{
						for(j = 0, shdr = shdr_table; j < hdr->e_shnum; shdr = GetNextSectionHeader(hdr, shdr),j++)
						{
							section_type = GetSectionType(shdr);
							section_size = GetSectionSize(shdr);
							section_vaddr = GetSectionAddr(shdr);
							section_flags = GetSectionFlags(shdr);

							if((section_flags & SHF_ALLOC))
							{
								if(section_vaddr >= phdr[i].p_vaddr && section_vaddr + section_size <= phdr[i].p_vaddr + segment_size)
								{
									logger << DebugInfo << "Section " << GetSectionName(shdr, sh_string_table) << " loaded (" << section_size << " bytes) at 0x" << hex << (section_vaddr - phdr[i].p_vaddr + segment_addr) << dec << EndDebugInfo;
								}
							}
						}
					}
				}

			}
		}
	}
	
	if(unlikely(verbose))
	{
		logger << DebugInfo << "Program entry point at 0x" << hex << entry_point << dec << EndDebugInfo;
	}
	
	if(shdr_table && sh_string_table)
	{
		for(i = 0, shdr = shdr_table; i < hdr->e_shnum; shdr = GetNextSectionHeader(hdr, shdr),i++)
		{
			//DumpSectionHeader(shdr, sh_string_table, cerr);

			section_type = GetSectionType(shdr);
			section_size = GetSectionSize(shdr);
			section_vaddr = GetSectionAddr(shdr);
			section_flags = GetSectionFlags(shdr);
			section_name = GetSectionName(shdr, sh_string_table);

			if((section_type != SHT_NULL) && (section_type != SHT_STRTAB) && !(section_flags & SHF_ALLOC))
			{
				if(section_type == SHT_SYMTAB)
				{
					Elf_Shdr *sh_strtab;
					section = malloc(section_size + 1);
					*((char *) section + section_size) = 0;
					if(!LoadSection(hdr, shdr, section, is))
					{
						logger << DebugWarning << "Can't load section " << section_name << EndDebugWarning;
					}
					else
					{
						int strtab_size;
						const char *strtab_section_name;
						char *string_table;
						if(unlikely(verbose))
						{
							logger << DebugInfo << "Loading section " << section_name << " (" << section_size << " bytes)" << EndDebugInfo;
						}
						sh_strtab = (Elf_Shdr *)((char *) shdr_table + shdr->sh_link * hdr->e_shentsize);
						strtab_size = GetSectionSize(sh_strtab);
						strtab_section_name = GetSectionName(sh_strtab, sh_string_table);
						string_table = (char *) malloc(strtab_size + 1);
						*(string_table + strtab_size) = 0;
						if(!LoadSection(hdr, sh_strtab, string_table, is))
						{
							logger << DebugWarning << "Can't load section " << strtab_section_name << EndDebugWarning;
						}
						else
						{
							if(unlikely(verbose))
							{
								logger << DebugInfo << "Building symbol table from sections " << section_name << " and " << strtab_section_name << EndDebugInfo;
							}
							BuildSymbolTable(shdr, section, string_table);
						}
						free(string_table);
					}
					free(section);
				}
				else if(strcmp(section_name, ".debug_line") == 0 || strcmp(section_name, ".debug_info") == 0 || strcmp(section_name, ".debug_abbrev") == 0
				     || strcmp(section_name, ".debug_aranges") == 0 || strcmp(section_name, ".debug_pubnames") == 0 || strcmp(section_name, ".debug_pubtypes") == 0
				     || strcmp(section_name, ".debug_macinfo") == 0 || strcmp(section_name, ".debug_str") == 0 || strcmp(section_name, ".debug_loc") == 0
				     || strcmp(section_name, ".debug_ranges") == 0 || strcmp(section_name, ".debug_frame") == 0)
				{
					if(unlikely(verbose))
					{
						logger << DebugInfo << "Loading section " << section_name << " (" << section_size << " bytes)" << EndDebugInfo;
					}
					section = malloc(section_size + 1);
					*((char *) section + section_size) = 0;
					if(!LoadSection(hdr, shdr, section, is))
					{
						logger << DebugWarning << "Can't load section " << section_name << EndDebugWarning;
						free(section);
					}
					else
					{
						if(!dw_handler) dw_handler = new DWARF_Handler<MEMORY_ADDR>(endianness);
						dw_handler->Handle(section_name, (uint8_t *) section, section_size);
					}
				}
				else
				{
					logger << DebugWarning << "Ignoring section " << section_name << " (" << section_size << " bytes)" << EndDebugWarning;
				}
			}
		}
	}

	if(sh_string_table) free(sh_string_table);
	if(shdr_table) free(shdr_table);
	if(phdr) free(phdr);
	if(hdr) free(hdr);
	
	if(dw_handler)
	{
		if(verbose)
		{
			logger << DebugInfo << "Parsing DWARF debugging informations" << EndDebugInfo;
		}
		dw_handler->Initialize();
	}

	return success;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
bool ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::Setup()
{
	return Load();
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
void ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::Reset()
{
	if(memory_import) memory_import->Reset();
/*	ClientIndependentSetup();
	ClientDependentSetup();*/
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
const list<unisim::util::debug::Symbol<MEMORY_ADDR> *> *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::GetSymbols() const {
	return symbol_table.GetSymbols();
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::FindSymbol(const char *name, MEMORY_ADDR addr, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const
{
	return symbol_table.FindSymbol(name, addr, type);
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::FindSymbolByAddr(MEMORY_ADDR addr) const
{
	return symbol_table.FindSymbolByAddr(addr);
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::FindSymbolByName(const char *name) const
{
	return symbol_table.FindSymbolByName(name);
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::FindSymbolByName(const char *name, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const
{
	return symbol_table.FindSymbolByName(name, type);
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::FindSymbolByAddr(MEMORY_ADDR addr, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const
{
	return symbol_table.FindSymbolByAddr(addr, type);
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
void ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::SwapSymbolEntry(Elf_Sym *sym)
{
	BSwap(sym->st_name);
	BSwap(sym->st_value);
	BSwap(sym->st_size);
	BSwap(sym->st_shndx);
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
void ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::AdjustSymbolEntry(const Elf_Ehdr *hdr, Elf_Sym *sym)
{
	switch(hdr->e_ident[EI_DATA])
	{
		case ELFDATA2LSB:
#if BYTE_ORDER == BIG_ENDIAN
			SwapSymbolEntry(sym);
#endif
			break;
		case ELFDATA2MSB:
#if BYTE_ORDER == LITTLE_ENDIAN
			SwapSymbolEntry(sym);
#endif
			break;
	}
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
void ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::AdjustSymbolTable(const Elf_Ehdr *hdr, const Elf_Shdr *shdr, Elf_Sym *sym)
{
	int nsymbols = shdr->sh_size / sizeof(Elf_Sym);
	int i;

	for(i = 0; i < nsymbols; i++, sym++)
	{
		AdjustSymbolEntry(hdr, sym);
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
	if(hdr->e_type >= ET_LOPROC && hdr->e_type <= ET_HIPROC) os << "Processor-specific"; else os << "Unknown (" << hdr->e_type << ")";

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
	switch(ELF32_ST_BIND(sym->st_info))
	{
		case STB_LOCAL: os << " Local symbol"; break;
		case STB_GLOBAL: os << " Global symbol"; break;
		case STB_WEAK: os << " Weak symbol"; break;
		default: os << "Unknown";
	}
	os << endl << "Type : ";
	switch(ELF32_ST_TYPE(sym->st_info))
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
	switch(ELF_ST_VISIBILITY(sym->st_other))
	{
		case STV_DEFAULT: os << "Default symbol visibility rules"; break;
		case STV_INTERNAL: os << "Processor specific hidden class"; break;
		case STV_HIDDEN: os << "Sym unavailable in other modules"; break;
		case STV_PROTECTED: os << "Not preemptible, not exported"; break;
		default: os << "Unknown";
	}
	os << endl << "Section index : " << sym->st_shndx;
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
bool ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::LoadSection(const Elf_Ehdr *hdr, const Elf_Shdr *shdr, void *buffer, istream& is)
{
	if(is.seekg(shdr->sh_offset, ios::beg).fail()) return false;
	if(is.read((char *) buffer, shdr->sh_size).fail()) return false;
	if(shdr->sh_type == SHT_SYMTAB)
	{
		AdjustSymbolTable(hdr, shdr, (Elf_Sym *) buffer);
	}
	return true;
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
void ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::BuildSymbolTable(const Elf_Shdr *shdr, const void *content, const char *string_table)
{
	int nsymbols = shdr->sh_size / sizeof(Elf_Sym);
	int i;
	Elf_Sym *sym;

	for(sym = (Elf_Sym *) content, i = 0; i < nsymbols; sym++, i++)
	{
		typename Symbol<MEMORY_ADDR>::Type type;

		switch(ELF32_ST_TYPE(sym->st_info))
		{
			case STT_NOTYPE: type = Symbol<MEMORY_ADDR>::SYM_NOTYPE; break;
			case STT_OBJECT: type = Symbol<MEMORY_ADDR>::SYM_OBJECT; break;
			case STT_FUNC: type = Symbol<MEMORY_ADDR>::SYM_FUNC; break;
			case STT_SECTION: type = Symbol<MEMORY_ADDR>::SYM_SECTION; break;
			case STT_FILE: type = Symbol<MEMORY_ADDR>::SYM_FILE; break;
			case STT_COMMON: type = Symbol<MEMORY_ADDR>::SYM_COMMON; break;
			case STT_TLS: type = Symbol<MEMORY_ADDR>::SYM_TLS; break;
			case STT_NUM: type = Symbol<MEMORY_ADDR>::SYM_NUM; break;
			case STT_LOOS: type = Symbol<MEMORY_ADDR>::SYM_LOOS; break;
			case STT_HIOS: type = Symbol<MEMORY_ADDR>::SYM_HIOS; break;
			case STT_LOPROC: type = Symbol<MEMORY_ADDR>::SYM_LOPROC; break;
			case STT_HIPROC: type = Symbol<MEMORY_ADDR>::SYM_HIPROC; break;
			default: type = Symbol<MEMORY_ADDR>::SYM_NOTYPE;
		}

		string symbol_name(string_table + sym->st_name);

		symbol_table.AddSymbol(symbol_name.c_str(), (MEMORY_ADDR) sym->st_value, (MEMORY_ADDR) sym->st_size, type);
	}
	//symbol_table.Dump(cerr);
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
const unisim::util::debug::Statement<MEMORY_ADDR> *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::FindStatement(MEMORY_ADDR addr) const
{
	return dw_handler ? dw_handler->FindStatement(addr) : 0;
}

template <class MEMORY_ADDR, unsigned int Elf_Class, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
const unisim::util::debug::Statement<MEMORY_ADDR> *ElfLoaderImpl<MEMORY_ADDR, Elf_Class, Elf_Ehdr, Elf_Phdr, Elf_Shdr, Elf_Sym>::FindStatement(const char *filename, unsigned int lineno, unsigned int colno) const
{
	return dw_handler ? dw_handler->FindStatement(filename, lineno, colno) : 0;
}

} // end of namespace elf_loader
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim

#endif
