/*
 *  Copyright (c) 2009,
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
 
#ifndef __UNISIM_SERVICE_LOADER_COFF_LOADER_COFF_LOADER_TCC__
#define __UNISIM_SERVICE_LOADER_COFF_LOADER_COFF_LOADER_TCC__

#include <unisim/util/endian/endian.hh>
#include <iostream>
#include <fstream>
#include <stdlib.h>

namespace unisim {
namespace service {
namespace loader {
namespace coff_loader {

template <class MEMORY_ADDR>
CoffLoader<MEMORY_ADDR>::CoffLoader(const char *name, Object *parent) :
	Object(name, parent),
	Client<Memory<MEMORY_ADDR> >(name, parent),
	Client<SymbolTableBuild<MEMORY_ADDR> >(name, parent),
	Service<Loader<MEMORY_ADDR> >(name, parent),
	memory_import("memory-import", this),
	symbol_table_build_import("symbol-table-build-import", this),
	loader_export("loader-export", this),
	filename(),
	entry_point(0),
	base_addr(0),
	top_addr(0),
	dump_headers(false),
	endianness(E_LITTLE_ENDIAN),
	param_filename("filename", this, filename),
	param_base_addr("base-addr", this, base_addr),
	param_dump_headers("dump-headers", this, dump_headers),
	param_endianness("endianness", this, endianness)
{
	Object::SetupDependsOn(memory_import);
	Object::SetupDependsOn(symbol_table_build_import);
}

template <class MEMORY_ADDR>
CoffLoader<MEMORY_ADDR>::~CoffLoader()
{
}

template <class MEMORY_ADDR>
void CoffLoader<MEMORY_ADDR>::OnDisconnect()
{
}

template <class MEMORY_ADDR>
MEMORY_ADDR CoffLoader<MEMORY_ADDR>::GetEntryPoint() const
{
	return (MEMORY_ADDR) entry_point;
}

template <class MEMORY_ADDR>
MEMORY_ADDR CoffLoader<MEMORY_ADDR>::GetTopAddr() const
{
	return (MEMORY_ADDR) top_addr;
}

template <class MEMORY_ADDR>
MEMORY_ADDR CoffLoader<MEMORY_ADDR>::GetStackBase() const
{
  return 0;
}

template <class MEMORY_ADDR>
bool CoffLoader<MEMORY_ADDR>::Setup()
{
	filehdr *hdr = 0;
	scnhdr *shdr_table = 0;

	if(symbol_table_build_import) symbol_table_build_import->Reset();

	bool success = true;

	if(filename.empty()) return true;
	
	ifstream is(filename.c_str(), ifstream::in | ifstream::binary);
	if(is.fail())
	{
		cerr << Object::GetName() << ": ERROR! Can't open executable \"" << filename << "\"" << endl;
		return false;
	}
	
	cerr << Object::GetName() << ": Opening \"" << filename << "\"" << endl;

	hdr = ReadFileHeader(is);

	if(!hdr)
	{
		cerr << Object::GetName() << ": ERROR! Could not read file header or \"" << filename << "\" is not a COFF file." << endl;
		return false;
	}

	if(dump_headers)
	{
		DumpFileHeader(hdr, cerr);
	}

	shdr_table = ReadSectionHeaders(hdr, is);

	if(!shdr_table)
	{
		cerr << Object::GetName() << ": ERROR! Can't read section headers" << endl;
	}
	else
	{

		unsigned int nscns = GetNumSectionHeaders(hdr);
		const scnhdr *shdr = 0;

		for(shdr = shdr_table; nscns > 0; nscns--, shdr = GetNextSectionHeader(hdr, shdr))
		{
			void *section = 0;
			DumpSectionHeader(hdr, shdr, cerr);
			MEMORY_ADDR section_addr = GetSectionPhysicalAddress(hdr, shdr);
			string section_name;
			GetSectionName(section_name, hdr, shdr);
			MEMORY_ADDR section_size = GetSectionSize(hdr, shdr);
			if(section_size == 0)
			{
				cerr << Object::GetName() << ": WARNING! section " << section_name << " is empty" << endl;
			}
			else
			{
				cerr << Object::GetName() << ": Loading section " << section_name << " at 0x" << hex << section_addr << dec << " (" << section_size << " bytes) " << endl;
				section = calloc(section_size, 1);
				if(!section)
				{
					cerr << Object::GetName() << ": WARNING! Can't load section " << section_name << endl;
					success = false;
				}
				else
				{
					if(!LoadSection(hdr, shdr, section, is))
					{
						cerr << Object::GetName() << ": WARNING! Can't load section " << section_name << endl;
					}
					else
					{
						if(!memory_import->WriteMemory(section_addr, section, section_size))
						{
							cerr << Object::GetName() << ": Can't write into memory (@0x" << hex << section_addr << " - @0x" << (section_addr +  section_size - 1) << dec << ")" << endl;
							success = false;
						}
						else 
						{
							cerr << Object::GetName() << ": write into memory (@0x" << hex << section_addr << " - @0x" << (section_addr +  section_size - 1) << dec << ")" << endl;
						}
					}

					free(section);
				}
			}
		}
	}

	if(hdr) free(hdr);
	if(shdr_table) free(shdr_table);

	return success;
}

template <class MEMORY_ADDR>
void CoffLoader<MEMORY_ADDR>::Reset()
{
	if(memory_import) memory_import->Reset();
}

template <class MEMORY_ADDR>
void CoffLoader<MEMORY_ADDR>::Target2Host(ti_filehdr *hdr)
{
	hdr->f_magic = unisim::util::endian::Target2Host(endianness, hdr->f_magic);
	hdr->f_nscns = unisim::util::endian::Target2Host(endianness, hdr->f_nscns);
	hdr->f_timdat = unisim::util::endian::Target2Host(endianness, hdr->f_timdat);
	hdr->f_symptr = unisim::util::endian::Target2Host(endianness, hdr->f_symptr);
	hdr->f_nsyms = unisim::util::endian::Target2Host(endianness, hdr->f_nsyms);
	hdr->f_flags = unisim::util::endian::Target2Host(endianness, hdr->f_flags);
	hdr->f_target_id = unisim::util::endian::Target2Host(endianness, hdr->f_target_id);
}

template <class MEMORY_ADDR>
void CoffLoader<MEMORY_ADDR>::Target2Host(filehdr *hdr)
{
	switch(unisim::util::endian::Target2Host(endianness, hdr->f_magic))
	{
		case TI_COFF0_MAGIC:
		case TI_COFF1_MAGIC:
		case TI_COFF2_MAGIC:
			Target2Host((ti_filehdr *) hdr);
			break;
	}
}

template <class MEMORY_ADDR>
void CoffLoader<MEMORY_ADDR>::Target2Host(ti_scnhdr_v01 *shdr)
{
	shdr->s_paddr = unisim::util::endian::Target2Host(endianness, shdr->s_paddr);
	shdr->s_vaddr = unisim::util::endian::Target2Host(endianness, shdr->s_vaddr);
	shdr->s_size = unisim::util::endian::Target2Host(endianness, shdr->s_size);
	shdr->s_scnptr = unisim::util::endian::Target2Host(endianness, shdr->s_scnptr);
	shdr->s_relptr = unisim::util::endian::Target2Host(endianness, shdr->s_relptr);
	shdr->s_lnnoptr = unisim::util::endian::Target2Host(endianness, shdr->s_lnnoptr);
	shdr->s_nreloc = unisim::util::endian::Target2Host(endianness, shdr->s_nreloc);
	shdr->s_nlnno = unisim::util::endian::Target2Host(endianness, shdr->s_nlnno);
	shdr->s_flags = unisim::util::endian::Target2Host(endianness, shdr->s_flags);
	shdr->s_reserved = unisim::util::endian::Target2Host(endianness, shdr->s_reserved);
	shdr->s_page = unisim::util::endian::Target2Host(endianness, shdr->s_page);
}

template <class MEMORY_ADDR>
void CoffLoader<MEMORY_ADDR>::Target2Host(ti_scnhdr_v2 *shdr)
{
	shdr->s_paddr = unisim::util::endian::Target2Host(endianness, shdr->s_paddr);
	shdr->s_vaddr = unisim::util::endian::Target2Host(endianness, shdr->s_vaddr);
	shdr->s_size = unisim::util::endian::Target2Host(endianness, shdr->s_size);
	shdr->s_scnptr = unisim::util::endian::Target2Host(endianness, shdr->s_scnptr);
	shdr->s_relptr = unisim::util::endian::Target2Host(endianness, shdr->s_relptr);
	shdr->s_lnnoptr = unisim::util::endian::Target2Host(endianness, shdr->s_lnnoptr);
	shdr->s_nreloc = unisim::util::endian::Target2Host(endianness, shdr->s_nreloc);
	shdr->s_nlnno = unisim::util::endian::Target2Host(endianness, shdr->s_nlnno);
	shdr->s_flags = unisim::util::endian::Target2Host(endianness, shdr->s_flags);
	shdr->s_reserved = unisim::util::endian::Target2Host(endianness, shdr->s_reserved);
	shdr->s_page = unisim::util::endian::Target2Host(endianness, shdr->s_page);
}

template <class MEMORY_ADDR>
void CoffLoader<MEMORY_ADDR>::Target2Host(const filehdr *hdr, scnhdr *shdr)
{
	switch(hdr->f_magic)
	{
		case TI_COFF0_MAGIC:
		case TI_COFF1_MAGIC:
			Target2Host((ti_scnhdr_v01 *) hdr);
			break;
		case TI_COFF2_MAGIC:
			Target2Host((ti_scnhdr_v2 *) hdr);
			break;
	}
}

template <class MEMORY_ADDR>
unsigned int CoffLoader<MEMORY_ADDR>::GetFileHeaderSize(const filehdr *hdr)
{
	switch(hdr->f_magic)
	{
		case TI_COFF0_MAGIC:
		case TI_COFF1_MAGIC:
		case TI_COFF2_MAGIC:
			return sizeof(ti_filehdr);
	}
	return 0;
}

template <class MEMORY_ADDR>
filehdr *CoffLoader<MEMORY_ADDR>::ReadFileHeader(istream& is)
{
	if(is.seekg(0, ios::beg).fail())
	{
		return 0;
	}

	// Read the magic number
	filehdr *hdr_magic = (filehdr *) malloc(sizeof(uint16_t));
	if(!hdr_magic) return 0;

	if(is.read((char *) hdr_magic, sizeof(uint16_t)).fail())
	{
		free(hdr_magic);
		return 0;
	}

	// Read the remaining bytes in file header
	unsigned int sizeof_hdr = GetFileHeaderSize(hdr_magic);
	if(!sizeof_hdr)
	{
		free(hdr_magic);
		return 0;
	}

	filehdr *hdr = (filehdr *) realloc(hdr_magic, sizeof_hdr);
	if(!hdr)
	{
		free(hdr_magic);
		return 0;
	}

	if(is.read((char *) hdr + sizeof(uint16_t), sizeof_hdr - sizeof(uint16_t)).fail())
	{
		free(hdr);
		return 0;
	}

	Target2Host(hdr);

	return hdr;
}

template <class MEMORY_ADDR>
unsigned int CoffLoader<MEMORY_ADDR>::GetSectionHeaderSize(const filehdr *hdr)
{
	switch(hdr->f_magic)
	{
		case TI_COFF0_MAGIC:
		case TI_COFF1_MAGIC:
			return sizeof(ti_scnhdr_v01);
		case TI_COFF2_MAGIC:
			return sizeof(ti_scnhdr_v2);
	}
	return 0;
}

template <class MEMORY_ADDR>
unsigned int CoffLoader<MEMORY_ADDR>::GetNumSectionHeaders(const filehdr *hdr)
{
	switch(hdr->f_magic)
	{
		case TI_COFF0_MAGIC:
		case TI_COFF1_MAGIC:
		case TI_COFF2_MAGIC:
			return hdr->ti.f_nscns;
			break;
	}
	return 0;
}

template <class MEMORY_ADDR>
scnhdr *CoffLoader<MEMORY_ADDR>::ReadSectionHeaders(const filehdr *hdr, istream& is)
{
	unsigned int sizeof_scnhdr = GetSectionHeaderSize(hdr);
	if(!sizeof_scnhdr) return 0;

	unsigned int nscns = GetNumSectionHeaders(hdr);
	if(!nscns) return 0;

	scnhdr *shdr_table = (scnhdr *) malloc(nscns * sizeof_scnhdr);
	if(!shdr_table) return 0;

	scnhdr *shdr;

	for(shdr = shdr_table; nscns > 0; nscns--, shdr = (scnhdr *)((char *) shdr + sizeof_scnhdr))
	{
		if(is.read((char *) shdr, sizeof_scnhdr).fail())
		{
			free(shdr_table);
			return 0;
		}

		Target2Host(hdr, shdr);
	}

	return shdr_table;
}

template <class MEMORY_ADDR>
void CoffLoader<MEMORY_ADDR>::DumpFileHeader(const filehdr *hdr, ostream& os)
{
	switch(hdr->f_magic)
	{
		case TI_COFF0_MAGIC:
		case TI_COFF1_MAGIC:
		case TI_COFF2_MAGIC:
			DumpFileHeader((const ti_filehdr *) hdr, os);
			break;
	}
}

template <class MEMORY_ADDR>
void CoffLoader<MEMORY_ADDR>::DumpFileHeader(const ti_filehdr *hdr, ostream& os)
{
	os << "--- File Header ---";
	os << endl << "magic number: 0x" << hex << hdr->f_magic << dec;
	os << endl << "number of sections: ";
	os << hdr->f_nscns;
	os << endl << "time and date stamp: ";
	os << hdr->f_timdat;
	os << endl << "file pointer to symbol table: ";
	os << hdr->f_symptr;
	os << endl << "number of symbol table entries: ";
	os << hdr->f_nsyms;
	os << endl << "size of optional header: ";
	os << hdr->f_opthdr;
	os << endl << "flags: ";
	os << hdr->f_flags;
	os << endl << "TI's target id: 0x" << hex << hdr->f_target_id << dec;
	os << endl;
}

template <class MEMORY_ADDR>
void CoffLoader<MEMORY_ADDR>::DumpSectionHeader(const filehdr *hdr, const scnhdr *shdr, ostream& os)
{
	switch(hdr->f_magic)
	{
		case TI_COFF0_MAGIC:
		case TI_COFF1_MAGIC:
			DumpSectionHeader((const ti_scnhdr_v01 *) shdr, os);
			break;
		case TI_COFF2_MAGIC:
			DumpSectionHeader((const ti_scnhdr_v2 *) shdr, os);
			break;
	}
}

template <class MEMORY_ADDR>
void CoffLoader<MEMORY_ADDR>::DumpSectionHeader(const ti_scnhdr_v01 *shdr, ostream& os)
{
	os << " --- Section Header (TI's COFF V0/V1) ---";
	char name[sizeof(shdr->s_name) + 1];
	memcpy(name, shdr->s_name, sizeof(shdr->s_name));
	name[sizeof(shdr->s_name)] = 0;
	os << endl << "Section name: " << name;
	os << endl << "Physical address: 0x" << hex << shdr->s_paddr << dec;
	os << endl << "Virtual address: 0x" << hex << shdr->s_vaddr << dec;
	os << endl << "Section size: " << shdr->s_size;
	os << endl << "File pointer to raw data: " << shdr->s_scnptr;
	os << endl << "File pointer to relocation: " << shdr->s_relptr;
	os << endl << "File pointer to line number: " << shdr->s_lnnoptr;
	os << endl << "Number of relocation entries: " << shdr->s_nreloc;
	os << endl << "Number of line number entries: " << shdr->s_nlnno;
	os << endl << "Flags: " << shdr->s_flags;
	os << endl << "Memory page number: " << (unsigned int) shdr->s_page;
	os << endl;
}

template <class MEMORY_ADDR>
void CoffLoader<MEMORY_ADDR>::DumpSectionHeader(const ti_scnhdr_v2 *shdr, ostream& os)
{
	os << " --- Section Header  (TI's COFF V2) ---";
	char name[sizeof(shdr->s_name) + 1];
	memcpy(name, shdr->s_name, sizeof(shdr->s_name));
	name[sizeof(shdr->s_name)] = 0;
	os << endl << "Section name: " << name;
	os << endl << "Physical address: 0x" << hex << shdr->s_paddr << dec;
	os << endl << "Virtual address: 0x" << hex << shdr->s_vaddr << dec;
	os << endl << "Section size: " << shdr->s_size;
	os << endl << "File pointer to raw data: " << shdr->s_scnptr;
	os << endl << "File pointer to relocation: " << shdr->s_relptr;
	os << endl << "File pointer to line number: " << shdr->s_lnnoptr;
	os << endl << "Number of relocation entries: " << shdr->s_nreloc;
	os << endl << "Number of line number entries: " << shdr->s_nlnno;
	os << endl << "Flags: " << shdr->s_flags;
	os << endl << "Memory page number: " << shdr->s_page;
	os << endl;
}

template <class MEMORY_ADDR>
const scnhdr *CoffLoader<MEMORY_ADDR>::GetNextSectionHeader(const filehdr *hdr, const scnhdr *shdr)
{
	return (const scnhdr *)((const char *) shdr + GetSectionHeaderSize(hdr));
}

template <class MEMORY_ADDR>
const char *CoffLoader<MEMORY_ADDR>::GetSectionName(string& name, const filehdr *hdr, const scnhdr *shdr)
{
	switch(hdr->f_magic)
	{
		case TI_COFF0_MAGIC:
		case TI_COFF1_MAGIC:
			{
				static char buffer[sizeof(shdr->ti_v01.s_name) + 1];
				memcpy(buffer, shdr->ti_v01.s_name, sizeof(shdr->ti_v01.s_name));
				buffer[sizeof(shdr->ti_v01.s_name)] = 0;
				name = buffer;
			}
			break;
		case TI_COFF2_MAGIC:
			{
				static char buffer[sizeof(shdr->ti_v2.s_name) + 1];
				memcpy(buffer, shdr->ti_v2.s_name, sizeof(shdr->ti_v2.s_name));
				buffer[sizeof(shdr->ti_v2.s_name)] = 0;
				name = buffer;
			}
			break;
		default:
			name = "?";
	}
	return name.c_str();
}

template <class MEMORY_ADDR>
MEMORY_ADDR CoffLoader<MEMORY_ADDR>::GetSectionVirtualAddress(const filehdr *hdr, const scnhdr *shdr)
{
	switch(hdr->f_magic)
	{
		case TI_COFF0_MAGIC:
		case TI_COFF1_MAGIC:
			return GetSectionVirtualAddress((const ti_scnhdr_v01 *) shdr);
		case TI_COFF2_MAGIC:
			return GetSectionVirtualAddress((const ti_scnhdr_v2 *) shdr);
	}
	return 0;
}

template <class MEMORY_ADDR>
MEMORY_ADDR CoffLoader<MEMORY_ADDR>::GetSectionVirtualAddress(const ti_scnhdr_v01 *shdr)
{
	return (MEMORY_ADDR) shdr->s_vaddr;
}

template <class MEMORY_ADDR>
MEMORY_ADDR CoffLoader<MEMORY_ADDR>::GetSectionVirtualAddress(const ti_scnhdr_v2 *shdr)
{
	return (MEMORY_ADDR) shdr->s_vaddr;
}

template <class MEMORY_ADDR>
MEMORY_ADDR CoffLoader<MEMORY_ADDR>::GetSectionPhysicalAddress(const filehdr *hdr, const scnhdr *shdr)
{
	switch(hdr->f_magic)
	{
		case TI_COFF0_MAGIC:
		case TI_COFF1_MAGIC:
			return GetSectionPhysicalAddress((const ti_scnhdr_v01 *) shdr);
		case TI_COFF2_MAGIC:
			return GetSectionPhysicalAddress((const ti_scnhdr_v2 *) shdr);
	}
	return 0;
}

template <class MEMORY_ADDR>
MEMORY_ADDR CoffLoader<MEMORY_ADDR>::GetSectionPhysicalAddress(const ti_scnhdr_v01 *shdr)
{
	return (MEMORY_ADDR) shdr->s_paddr;
}

template <class MEMORY_ADDR>
MEMORY_ADDR CoffLoader<MEMORY_ADDR>::GetSectionPhysicalAddress(const ti_scnhdr_v2 *shdr)
{
	return (MEMORY_ADDR) shdr->s_paddr;
}

template <class MEMORY_ADDR>
MEMORY_ADDR CoffLoader<MEMORY_ADDR>::GetSectionSize(const filehdr *hdr, const scnhdr *shdr)
{
	switch(hdr->f_magic)
	{
		case TI_COFF0_MAGIC:
		case TI_COFF1_MAGIC:
			return GetSectionSize((const ti_scnhdr_v01 *) shdr);
		case TI_COFF2_MAGIC:
			return GetSectionSize((const ti_scnhdr_v2 *) shdr);
	}
	return 0;
}

template <class MEMORY_ADDR>
MEMORY_ADDR CoffLoader<MEMORY_ADDR>::GetSectionSize(const ti_scnhdr_v01 *shdr)
{
	return (MEMORY_ADDR)(shdr->s_size * 4); // s_size is the number of words, word size is 4
}

template <class MEMORY_ADDR>
MEMORY_ADDR CoffLoader<MEMORY_ADDR>::GetSectionSize(const ti_scnhdr_v2 *shdr)
{
	return (MEMORY_ADDR)(shdr->s_size * 4); // s_size is the number of words, word size is 4
}

template <class MEMORY_ADDR>
long CoffLoader<MEMORY_ADDR>::GetSectionFilePtr(const filehdr *hdr, const scnhdr *shdr)
{
	switch(hdr->f_magic)
	{
		case TI_COFF0_MAGIC:
		case TI_COFF1_MAGIC:
			return GetSectionFilePtr((const ti_scnhdr_v01 *) shdr);
		case TI_COFF2_MAGIC:
			return GetSectionFilePtr((const ti_scnhdr_v2 *) shdr);
	}
	return 0;
}

template <class MEMORY_ADDR>
long CoffLoader<MEMORY_ADDR>::GetSectionFilePtr(const ti_scnhdr_v01 *shdr)
{
	return (long) shdr->s_scnptr;
}

template <class MEMORY_ADDR>
long CoffLoader<MEMORY_ADDR>::GetSectionFilePtr(const ti_scnhdr_v2 *shdr)
{
	return (long) shdr->s_scnptr;
}

template <class MEMORY_ADDR>
bool CoffLoader<MEMORY_ADDR>::LoadSection(const filehdr *hdr, const scnhdr *shdr, void *buffer, istream& is)
{
	long section_file_ptr = GetSectionFilePtr(hdr, shdr);
	MEMORY_ADDR section_size = GetSectionSize(hdr, shdr);

	if(is.seekg(section_file_ptr, ios::beg).fail()) return false;
	if(is.read((char *) buffer, section_size).fail()) return false;
	
	return true;
}

} // end of namespace coff_loader
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim

#endif
