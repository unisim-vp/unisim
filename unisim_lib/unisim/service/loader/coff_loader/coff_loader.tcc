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

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <unisim/util/endian/endian.hh>

#include <unisim/service/loader/coff_loader/ti/ti.hh>
#include <unisim/service/loader/coff_loader/ti/ti.tcc>

namespace unisim {
namespace service {
namespace loader {
namespace coff_loader {

using std::stringstream;
using namespace unisim::kernel::logger;

template <class MEMORY_ADDR>
CoffLoader<MEMORY_ADDR>::CoffLoader(const char *name, Object *parent)
	: Object(name, parent)
	, Client<Memory<MEMORY_ADDR> >(name, parent)
	, Service<SymbolTableLookup<MEMORY_ADDR> >(name, parent)
	, Service<Loader<MEMORY_ADDR> >(name, parent)
	, memory_import("memory-import", this)
	, symbol_table_lookup_export("symbol-table-lookup-export", this)
	, loader_export("loader-export", this)
	, filename()
	, entry_point(0)
	, top_addr(0)
	, stack_base(0)
	, dump_headers(false)
	, verbose_write(false)
	, logger(*this)
	, param_filename("filename", this, filename)
	, param_dump_headers("dump-headers", this, dump_headers)
	, param_verbose_write("verbose-write", this, verbose_write)
{
	Object::SetupDependsOn(memory_import);

	unisim::service::loader::coff_loader::ti::FileHandler<MEMORY_ADDR>::Register(&file_handler_registry);
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
	return (MEMORY_ADDR) entry_point * memory_atom_size;
}

template <class MEMORY_ADDR>
MEMORY_ADDR CoffLoader<MEMORY_ADDR>::GetTopAddr() const
{
	return (MEMORY_ADDR) top_addr * memory_atom_size;
}

template <class MEMORY_ADDR>
MEMORY_ADDR CoffLoader<MEMORY_ADDR>::GetStackBase() const
{
  return (MEMORY_ADDR) stack_base * memory_atom_size;
}

template <class MEMORY_ADDR>
bool CoffLoader<MEMORY_ADDR>::Write(MEMORY_ADDR addr, const void *content, uint32_t size)
{
	if(!memory_import->WriteMemory(addr * memory_atom_size, content, size * memory_atom_size))
	{
		logger << DebugError << "Can't write into memory (@0x" << hex << addr << " - @0x" << (addr +  size - 1) << dec << ")" << EndDebugError;
		return false;
	}

	if(verbose_write) logger << DebugInfo << "Write into memory (@0x" << hex << addr << " - @0x" << (addr +  size - 1) << dec << ")" << EndDebugInfo;
	return true;
}

template <class MEMORY_ADDR>
bool CoffLoader<MEMORY_ADDR>::Setup()
{
	bool success = true;

	if(filename.empty()) return true;
	
	ifstream is(filename.c_str(), ifstream::in | ifstream::binary);
	if(is.fail())
	{
		logger << DebugError << "Can't open executable \"" << filename << "\"" << EndDebugError;
		return false;
	}
	
	logger << DebugInfo << "Opening \"" << filename << "\"" << EndDebugInfo;

	if(is.seekg(0, ios::beg).fail())
	{
		return false;
	}

	// Read the magic number
	uint16_t magic;

	if(is.read((char *) &magic, sizeof(magic)).fail())
	{
		logger << DebugError << "Could not read file header or \"" << filename << "\" is not a COFF file." << EndDebugError;
		return false;
	}

	magic = unisim::util::endian::Host2LittleEndian(magic);

	FileHandler<MEMORY_ADDR> *file_handler = file_handler_registry[magic];

	if(!file_handler)
	{
		logger << DebugError << "Can't handle format of \"" << filename << "\"." << EndDebugError;
		logger << DebugInfo << "Supported formats are:" << endl;
		stringstream sstr;
		file_handler_registry.DumpFileHandlers(sstr);
		logger << sstr.str() << EndDebugInfo;
		return false;
	}

	File<MEMORY_ADDR> *file = file_handler->GetFile();

	if(!file)
	{
		logger << DebugError<< file_handler->What() << " can't handle format of \"" << filename << "\"" << EndDebugError;
		return false;
	}

	// Read the remaining bytes in file header
	unsigned int file_hdr_size = file->GetFileHeaderSize();

	char hdr[file_hdr_size];

	*(uint16_t *) hdr = magic;
	if(is.read(hdr + sizeof(magic), file_hdr_size - sizeof(magic)).fail())
	{
		logger << DebugError << "Could not read file header or \"" << filename << "\" is not a COFF file." << EndDebugError;
		delete file;
		return false;
	}

	if(!file->ParseFileHeader(hdr))
	{
		logger << DebugError << "File header is not supported or \"" << filename << "\" is not a COFF file." << EndDebugError;
		delete file;
		return false;
	}

	file_endianness = file->GetFileEndian();
	file->GetBasicTypeSizes(basic_type_sizes);

	if(dump_headers)
	{
		stringstream sstr;
		file->DumpFileHeader(sstr);
		logger << DebugInfo << sstr.str() << EndDebugInfo;
	}

	logger << DebugInfo << "File \"" << filename << "\" has " << (file->GetFileEndian() == E_LITTLE_ENDIAN ? "little" : "big") << "-endian headers" << EndDebugInfo;
	logger << DebugInfo << "File \"" << filename << "\" is for " << file->GetArchitectureName() << EndDebugInfo;

	if(!file->IsExecutable())
	{
		logger << DebugError << "File \"" << filename << "\" is not an executable COFF file." << EndDebugError;
		delete file;
		return false;
	}

	logger << DebugInfo << "File \"" << filename << "\" is executable" << EndDebugInfo;

	unsigned int aout_hdr_size = file->GetAoutHeaderSize();

	if(aout_hdr_size > 0)
	{
		char aout_hdr[aout_hdr_size];

		if(is.read(aout_hdr, aout_hdr_size).fail())
		{
			logger << DebugError << "Could not read optional header." << EndDebugInfo;
			delete file;
			return false;
		}

		if(!file->ParseAoutHeader(aout_hdr))
		{
			logger << DebugError << "optional header is invalid or unsupported." << EndDebugError;
			delete file;
			return false;
		}

		entry_point = file->GetEntryPoint();

		logger << DebugInfo << "Program entry point at 0x" << hex << entry_point << dec << EndDebugInfo;

		if(dump_headers)
		{
			stringstream sstr;
			file->DumpAoutHeader(sstr);
			logger << DebugInfo << sstr.str() << EndDebugInfo;
		}
	}
	else
	{
		logger << DebugWarning << "File \"" << filename << "\" has no optional header." << EndDebugWarning;
		logger << DebugWarning << "Program entry point is unknown" << EndDebugWarning;
	}

	unsigned int num_sections = file->GetNumSections();
	unsigned int shdr_size = file->GetSectionHeaderSize();
	memory_atom_size = file->GetMemoryAtomSize();
	unsigned int section_num;

	for(section_num = 0; section_num < num_sections; section_num++)
	{
		char shdr[shdr_size];

		if(is.read(shdr, shdr_size).fail())
		{
			logger << DebugError << "Can't read section headers" << EndDebugError;
			delete file;
			return false;
		}
		
		if(!file->ParseSectionHeader(section_num, shdr))
		{
			logger << DebugError << "Section header #" << section_num << " is invalid or unsupported" << EndDebugError;
			delete file;
			return false;
		}
	}

	const SectionTable<MEMORY_ADDR> *section_table = file->GetSectionTable();

	for(section_num = 0; section_num < num_sections; section_num++)
	{
		const Section<MEMORY_ADDR> *section = (*section_table)[section_num];

		if(dump_headers)
		{
			stringstream sstr;
			section->DumpHeader(sstr);
			logger << DebugInfo << sstr.str() << EndDebugInfo;
		}

		typename Section<MEMORY_ADDR>::Type section_type = section->GetType();
		const char *section_name = section->GetName();
		MEMORY_ADDR section_addr = section->GetPhysicalAddress();
		MEMORY_ADDR section_size = section->GetSize();
		long section_content_file_ptr = section->GetContentFilePtr();

		if(section_size > 0 && section_type != Section<MEMORY_ADDR>::ST_NOT_LOADABLE)
		{
			void *section_content = 0;

			switch(section_type)
			{
				case Section<MEMORY_ADDR>::ST_LOADABLE_RAWDATA:
				case Section<MEMORY_ADDR>::ST_SPECIFIC_CONTENT:
				{
					logger << DebugInfo << "Loading section " << section_name << EndDebugInfo;
					section_content = calloc(section_size, memory_atom_size);

					if(!section_content || is.seekg(section_content_file_ptr, ios::beg).fail() || is.read((char *) section_content, section_size * memory_atom_size).fail())
					{
						logger << DebugError << "Can't load section " << section_name << EndDebugError;
						success = false;
					}
					break;
				}
			}

			if(success)
			{
				switch(section_type)
				{
					case Section<MEMORY_ADDR>::ST_LOADABLE_RAWDATA:
						if(!Write(section_addr, section_content, section_size))
						{
							logger << DebugError << "Can't write raw data of section " << section_name << " into memory" << EndDebugError;
							success = false;
						}
						break;
					case Section<MEMORY_ADDR>::ST_STACK:
						stack_base = section_addr;
						logger << DebugInfo << "Stack base at 0x" << hex << stack_base << dec << EndDebugInfo;
						break;
					case Section<MEMORY_ADDR>::ST_SPECIFIC_CONTENT:
						if(!section->LoadSpecificContent(this, section_content, section_size))
						{
							logger << DebugError << "Can't load specific content of section " << section_name << EndDebugError;
							success = false;
						}
						break;
				}
			}

			if(section_content) free(section_content);
		}
	}

	long symtab_file_ptr = file->GetSymbolTableFilePtr();
	unsigned long num_symbols = file->GetNumSymbols();
	if(symtab_file_ptr && num_symbols)
	{
		logger << DebugInfo << "Loading symbol table" << EndDebugInfo;

		unsigned long symtab_size = num_symbols * sizeof(syment);

		void *symtab_content = calloc(symtab_size, 1);

		if(!symtab_content || is.seekg(symtab_file_ptr, ios::beg).fail() || is.read((char *) symtab_content, symtab_size).fail())
		{
			logger << DebugError << "Can't load symbol table" << EndDebugError;
			success = false;
		}
		else
		{
			logger << DebugInfo << "Loading string table" << EndDebugInfo;
			uint32_t string_table_size;

			if(is.read((char *) &string_table_size, sizeof(string_table_size)).fail())
			{
				logger << DebugError << "Can't load string table" << EndDebugError;
				success = false;
			}
			else
			{
				string_table_size = unisim::util::endian::Target2Host(file_endianness, string_table_size);
				void *string_table = calloc(string_table_size, 1);

				*(uint32_t *) string_table = 0;
				if(!string_table || is.read((char *) string_table + 4, string_table_size - 4).fail())
				{
					logger << DebugError << "Can't load string table" << EndDebugError;
					success = false;
				}
				else
				{
					// Parse the symbol table
					if(!ParseSymbolTable((syment *) symtab_content, num_symbols, (const char *) string_table, string_table_size))
					{
						logger << DebugError << "Symbol table is broken" << EndDebugError;
						success = false;
					}
				}
				if(string_table) free(string_table);
			}
		}

		if(symtab_content) free(symtab_content);
	}
	
	delete file;
	return success;
}

template <class MEMORY_ADDR>
void CoffLoader<MEMORY_ADDR>::Reset()
{
	if(memory_import) memory_import->Reset();
}

template <class MEMORY_ADDR>
const char *CoffLoader<MEMORY_ADDR>::GetStorageClassName(uint8_t sclass) const
{
	switch(sclass)
	{
		case C_NULL: return "C_NULL";
		case C_AUTO: return "C_AUTO";
		case C_EXT: return "C_EXT";
		case C_STAT: return "C_STAT";
		case C_REG: return "C_REG";
		case C_EXTDEF: return "C_EXTDEF";
		case C_LABEL: return "C_LABEL";
		case C_ULABEL: return "C_ULABEL";
		case C_MOS: return "C_MOS";
		case C_ARG: return "C_ARG";
		case C_STRTAG: return "C_STRTAG";
		case C_MOU: return "C_MOU";
		case C_UNTAG: return "C_UNTAG";
		case C_TPDEF: return "C_TPDEF";
		case C_USTATIC: return "C_USTATIC";
		case C_ENTAG: return "C_ENTAG";
		case C_MOE: return "C_MOE";
		case C_REGPARM: return "C_REGPARM";
		case C_FIELD: return "C_FIELD";
		case C_AUTOARG: return "C_AUTOARG";
		case C_LASTENT: return "C_LASTENT";
		case C_BLOCK: return "C_BLOCK";
		case C_FCN: return "C_FCN";
		case C_EOS: return "C_EOS";
		case C_FILE: return "C_FILE";
		case C_LINE: return "C_LINE";
		case C_ALIAS: return "C_ALIAS";
		case C_HIDDEN: return "C_HIDDEN";
		case C_EFCN: return "C_EFCN";
	}
	return "?";
}

template <class MEMORY_ADDR>
const char *CoffLoader<MEMORY_ADDR>::GetStorageClassFriendlyName(uint8_t sclass) const
{
	switch(sclass)
	{
		case C_NULL: return "no storage class";
		case C_AUTO: return "automatic variable";
		case C_EXT: return "external symbol";
		case C_STAT: return "static";
		case C_REG: return "register variable";
		case C_EXTDEF: return "external definition";
		case C_LABEL: return "label";
		case C_ULABEL: return "undefined label";
		case C_MOS: return "member of structure";
		case C_ARG: return "function argument";
		case C_STRTAG: return "structure tag";
		case C_MOU: return "member of union";
		case C_UNTAG: return "union tag";
		case C_TPDEF: return "type definition";
		case C_USTATIC: return "undefined static";
		case C_ENTAG: return "enumeration tag";
		case C_MOE: return "member of enumeration";
		case C_REGPARM: return "register parameter";
		case C_FIELD: return "bit field";
		case C_AUTOARG: return "auto argument";
		case C_LASTENT: return "dummy entry (end of block)";
		case C_BLOCK: return "beginning or end of a block: \".bb\" or \".eb\"";
		case C_FCN: return "beginning or end of a function: \".bf\" or \".ef\"";
		case C_EOS: return "end of structure";
		case C_FILE: return "file name";
		case C_LINE: return "line # reformatted as symbol table entry";
		case C_ALIAS: return "duplicate tag";
		case C_HIDDEN: return "ext symbol in dmert public lib";
		case C_EFCN: return "physical end of function";
	}
	return "?";
}

template <class MEMORY_ADDR>
const char *CoffLoader<MEMORY_ADDR>::GetBasicTypeName(uint16_t basic_type) const
{
	switch(basic_type)
	{
		case T_NULL: return "T_NULL";
		case T_VOID: return "T_VOID";
		case T_CHAR: return "T_CHAR";
		case T_SHORT: return "T_SHORT";
		case T_INT: return "T_INT";
		case T_LONG: return "T_LONG";
		case T_FLOAT: return "T_FLOAT";
		case T_DOUBLE: return "T_DOUBLE";
		case T_STRUCT: return "T_STRUCT";
		case T_UNION: return "T_UNION";
		case T_ENUM: return "T_ENUM";
		case T_MOE: return "T_NOE";
		case T_UCHAR: return "T_UCHAR";
		case T_USHORT: return "T_USHORT";
		case T_UINT: return "T_UINT";
		case T_ULONG: return "T_ULONG";
	}
	return "?";
}

template <class MEMORY_ADDR>
const char *CoffLoader<MEMORY_ADDR>::GetBasicTypeFriendlyName(uint16_t basic_type) const
{
	switch(basic_type)
	{
		case T_VOID: return "void";
		case T_CHAR: return "char";
		case T_SHORT: return "short";
		case T_INT: return "int";
		case T_LONG: return "long";
		case T_FLOAT: return "float";
		case T_DOUBLE: return "double";
		case T_STRUCT: return "struct";
		case T_UNION: return "union";
		case T_ENUM: return "enum";
		case T_MOE: return "enum member";
		case T_UCHAR: return "unsigned char";
		case T_USHORT: return "unsigned short";
		case T_UINT: return "unsigned int";
		case T_ULONG: return "unsigned long";
	}
	return "?";
}

template <class MEMORY_ADDR>
const char *CoffLoader<MEMORY_ADDR>::GetDerivedTypeName(uint16_t derived_type) const
{
	switch(derived_type)
	{
		case DT_NON: return "DT_NON";
		case DT_PTR: return "DT_PTR";
		case DT_FCN: return "DT_FCN";
		case DT_ARY: return "DT_ARY";
	}
	return "?";
}

template <class MEMORY_ADDR>
const char *CoffLoader<MEMORY_ADDR>::GetDerivedTypeFriendlyName(uint16_t derived_type) const
{
	switch(derived_type)
	{
		case DT_PTR: return "*";
		case DT_FCN: return "()";
		case DT_ARY: return "[]";
	}
	return "?";
}

template <class MEMORY_ADDR>
string CoffLoader<MEMORY_ADDR>::GetTypeName(uint16_t type) const
{
	stringstream sstr;

	uint16_t ty = type;

	uint16_t basic_type = ty & ((1 << N_BTSZ) - 1);
	ty >>= N_BTSZ;

	sstr << GetBasicTypeName(basic_type);

	unsigned int i;
	for(i = 0; i < 6; i++, ty >>= N_DTSZ)
	{
		uint16_t derived_type = ty & ((1 << N_DTSZ) - 1);
		//if(derived_type == DT_NON) break;
		sstr << " " << GetDerivedTypeName(derived_type);
	}

	sstr << " => ";
	for(ty = type >> N_BTSZ, i = 0; i < 6; i++, ty >>= N_DTSZ)
	{
		uint16_t derived_type = ty & ((1 << N_DTSZ) - 1);
		//if(derived_type == DT_NON) break;
		sstr << GetDerivedTypeFriendlyName(derived_type);
	}

	sstr << " " << GetBasicTypeFriendlyName(basic_type);

	return sstr.str();
}

template <class MEMORY_ADDR>
bool CoffLoader<MEMORY_ADDR>::ParseSymbolTable(syment *symtab, unsigned long nsyms, const char *strtab, uint32_t strtab_size)
{
	unsigned long i;
	int numaux = 0;
	char name_buf[E_SYMNMLEN + 1];
	syment *coff_sym;
	const char *sym_name = 0;

	for(i = 0; i < nsyms; i++)
	{
		if(dump_headers)
		{
			logger << DebugInfo << "[" << i << "] ";
		}

		if(numaux)
		{
			uint8_t sclass = coff_sym->e_sclass;
			uint16_t basic_type = coff_sym->e_type & ((1 << N_BTSZ) - 1);
			uint16_t derived_type1 = (coff_sym->e_type >> N_BTSZ) & ((1 << N_DTSZ) - 1);

			if((sclass == C_EXT || sclass == C_STAT) && derived_type1 == DT_FCN && basic_type != T_MOE)
			{
				fcn_auxent *fcn_aux = (fcn_auxent *) &symtab[i];

				fcn_aux->x_tagndx = unisim::util::endian::Target2Host(file_endianness, fcn_aux->x_tagndx);
				fcn_aux->x_fsize = unisim::util::endian::Target2Host(file_endianness, fcn_aux->x_fsize);
				fcn_aux->x_lnnoptr = unisim::util::endian::Target2Host(file_endianness, fcn_aux->x_lnnoptr);
				fcn_aux->x_endndx = unisim::util::endian::Target2Host(file_endianness, fcn_aux->x_endndx);

				if(dump_headers)
				{
					logger << "FCNAUX: ";
					logger << "size=" << fcn_aux->x_fsize << ",linenoptr=" << fcn_aux->x_lnnoptr << ", nextentry_index=" << fcn_aux->x_endndx;
				}

				symbol_table.AddSymbol(sym_name, coff_sym->e_value * memory_atom_size, fcn_aux->x_fsize * memory_atom_size, unisim::util::debug::Symbol<MEMORY_ADDR>::SYM_FUNC, memory_atom_size);
			}
			else if(sclass == C_STAT && derived_type1 == DT_NON && basic_type == T_NULL)
			{
				scn_auxent *scn_aux = (scn_auxent *) &symtab[i];

				scn_aux->x_scnlen = unisim::util::endian::Target2Host(file_endianness, scn_aux->x_scnlen);
				scn_aux->x_nreloc = unisim::util::endian::Target2Host(file_endianness, scn_aux->x_nreloc);
				scn_aux->x_nlnno = unisim::util::endian::Target2Host(file_endianness, scn_aux->x_nlnno);

				if(dump_headers)
				{
					logger << "SCNAUX: ";
					logger << "length=" << scn_aux->x_scnlen;
				}

				symbol_table.AddSymbol(sym_name, coff_sym->e_value * memory_atom_size, scn_aux->x_scnlen * memory_atom_size, unisim::util::debug::Symbol<MEMORY_ADDR>::SYM_SECTION, memory_atom_size);
			}
			else if(sclass == C_FILE && derived_type1 == DT_NON && basic_type == T_NULL)
			{
				file_auxent *file_aux = (file_auxent *) &symtab[i];

				const char *filename;
				char filename_buf[E_FILNMLEN + 1];

				if(file_aux->x.x.x_zeroes == 0)
				{
					// file name is in string table
					file_aux->x.x.x_offset = unisim::util::endian::Target2Host(file_endianness, file_aux->x.x.x_offset);
					uint32_t strtab_ofs = file_aux->x.x.x_offset;
					filename = strtab + strtab_ofs;
				}
				else
				{
					memcpy(filename_buf, file_aux->x.x_fname, E_FILNMLEN);
					filename_buf[E_FILNMLEN] = 0;
					filename = filename_buf;
				}

				if(dump_headers)
				{
					logger << "FILAUX: ";
					logger << "filename=" << filename;
				}

				symbol_table.AddSymbol(filename, coff_sym->e_value * memory_atom_size, 0, unisim::util::debug::Symbol<MEMORY_ADDR>::SYM_FILE, memory_atom_size);
			}
			else if((sclass == C_EXT || sclass == C_STAT) && derived_type1 == DT_ARY)
			{
				ary_auxent *ary_aux = (ary_auxent *) &symtab[i];

				ary_aux->x_tagndx = unisim::util::endian::Target2Host(file_endianness, ary_aux->x_tagndx);
				ary_aux->x_lnno = unisim::util::endian::Target2Host(file_endianness, ary_aux->x_lnno);
				ary_aux->x_arylen = unisim::util::endian::Target2Host(file_endianness, ary_aux->x_arylen);

				unsigned int i;
				for(i = 0; i < E_DIMNUM; i++)
				{
					ary_aux->x_dim[i] = unisim::util::endian::Target2Host(file_endianness, ary_aux->x_dim[i]);
				}

				if(dump_headers)
				{
					logger << "ARYAUX: ";
					logger << "length=" << ary_aux->x_arylen << ", dim1=" << ary_aux->x_dim[0];
				}

				symbol_table.AddSymbol(sym_name, coff_sym->e_value * memory_atom_size, basic_type_sizes[basic_type] * ary_aux->x_arylen * memory_atom_size, unisim::util::debug::Symbol<MEMORY_ADDR>::SYM_OBJECT, memory_atom_size);
			}
			else
			{
				if(dump_headers)
				{
					logger << "AUX: <?>";
				}
			}

			numaux--;
		}
		else
		{
			coff_sym = &symtab[i];

			if(coff_sym->e.e.e_zeroes == 0)
			{
				// name is in string table
				coff_sym->e.e.e_offset = unisim::util::endian::Target2Host(file_endianness, coff_sym->e.e.e_offset);
				uint32_t strtab_ofs = coff_sym->e.e.e_offset;
				sym_name = strtab + strtab_ofs;
			}
			else
			{
				memcpy(name_buf, coff_sym->e.e_name, E_SYMNMLEN);
				name_buf[E_SYMNMLEN] = 0;
				sym_name = name_buf;
			}

			coff_sym->e_value = unisim::util::endian::Target2Host(file_endianness, coff_sym->e_value);
			coff_sym->e_scnum = unisim::util::endian::Target2Host(file_endianness, coff_sym->e_scnum);
			coff_sym->e_sclass = unisim::util::endian::Target2Host(file_endianness, coff_sym->e_sclass);
			coff_sym->e_type = unisim::util::endian::Target2Host(file_endianness, coff_sym->e_type);
			coff_sym->e_numaux = unisim::util::endian::Target2Host(file_endianness, coff_sym->e_numaux);

			uint8_t sclass = coff_sym->e_sclass;
			uint16_t type = coff_sym->e_type;
			uint16_t basic_type = type & ((1 << N_BTSZ) - 1);
			uint16_t derived_type1 = (type >> N_BTSZ) & ((1 << N_DTSZ) - 1);
			numaux = coff_sym->e_numaux;

			if(dump_headers)
			{
				logger << "SYMBOL: ";
				logger << "name=\"" << sym_name << "\", value=0x" << hex << coff_sym->e_value << dec;
				logger << ", storage class=" << GetStorageClassName(sclass) << " (" << GetStorageClassFriendlyName(sclass) << ")";
				logger << ", type=" << GetTypeName(type);

				if(coff_sym->e_numaux)
				{
					logger << ", " << (unsigned int) coff_sym->e_numaux << " aux entries follow";
				}
			}

			if(sclass == C_EXT && derived_type1 == DT_NON)
			{
				symbol_table.AddSymbol(sym_name, coff_sym->e_value * memory_atom_size, basic_type_sizes[basic_type] * memory_atom_size, unisim::util::debug::Symbol<MEMORY_ADDR>::SYM_OBJECT, memory_atom_size);
			}
			else if(sclass == C_FILE && !numaux)
			{
				symbol_table.AddSymbol(sym_name, 0, 0, unisim::util::debug::Symbol<MEMORY_ADDR>::SYM_FILE, memory_atom_size);
			}
		}

		if(dump_headers)
		{
			logger << EndDebugInfo;
		}
	}

	return true;
}

template <class MEMORY_ADDR>
const list<unisim::util::debug::Symbol<MEMORY_ADDR> *> *CoffLoader<MEMORY_ADDR>::GetSymbols() const {
	return symbol_table.GetSymbols();
}

template <class MEMORY_ADDR>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *CoffLoader<MEMORY_ADDR>::FindSymbol(const char *name, MEMORY_ADDR addr, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const
{
	return symbol_table.FindSymbol(name, addr, type);
}

template <class MEMORY_ADDR>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *CoffLoader<MEMORY_ADDR>::FindSymbolByAddr(MEMORY_ADDR addr) const
{
	return symbol_table.FindSymbolByAddr(addr);
}

template <class MEMORY_ADDR>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *CoffLoader<MEMORY_ADDR>::FindSymbolByName(const char *name) const
{
	return symbol_table.FindSymbolByName(name);
}

template <class MEMORY_ADDR>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *CoffLoader<MEMORY_ADDR>::FindSymbolByName(const char *name, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const
{
	return symbol_table.FindSymbolByName(name, type);
}

template <class MEMORY_ADDR>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *CoffLoader<MEMORY_ADDR>::FindSymbolByAddr(MEMORY_ADDR addr, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const
{
	return symbol_table.FindSymbolByAddr(addr, type);
}

template <class MEMORY_ADDR>
SectionTable<MEMORY_ADDR>::SectionTable(unsigned int num_sections)
	: sections(num_sections)
{
}

template <class MEMORY_ADDR>
SectionTable<MEMORY_ADDR>::~SectionTable()
{
	unsigned int section_num;
	unsigned int num_sections = sections.size();

	for(section_num = 0; section_num < num_sections; section_num++)
	{
		Section<MEMORY_ADDR> *section = sections[section_num];
		if(section) delete section;
	}
}

template <class MEMORY_ADDR>
void SectionTable<MEMORY_ADDR>::Insert(unsigned int section_num, Section<MEMORY_ADDR> *section)
{
	sections[section_num] = section;
}

template <class MEMORY_ADDR>
const Section<MEMORY_ADDR> *SectionTable<MEMORY_ADDR>::operator [] (unsigned int section_num) const
{
	return sections[section_num];
}

template <class MEMORY_ADDR>
unsigned int SectionTable<MEMORY_ADDR>::GetSize() const
{
	return sections.size();
}

template <class MEMORY_ADDR>
FileHandlerRegistry<MEMORY_ADDR>::FileHandlerRegistry()
{
	Reset();
}

template <class MEMORY_ADDR>
FileHandlerRegistry<MEMORY_ADDR>::~FileHandlerRegistry()
{
	Reset();
}

template <class MEMORY_ADDR>
void FileHandlerRegistry<MEMORY_ADDR>::Register(FileHandler<MEMORY_ADDR> *file_handler)
{
	uint16_t magic = file_handler->GetMagic();

	if(file_handlers[magic])
	{
		delete file_handlers[magic];
	}

	file_handlers[magic] = file_handler;
}

template <class MEMORY_ADDR>
FileHandler<MEMORY_ADDR> *FileHandlerRegistry<MEMORY_ADDR>::operator [] (uint16_t magic)
{
	return file_handlers[magic];
}

template <class MEMORY_ADDR>
void FileHandlerRegistry<MEMORY_ADDR>::DumpFileHandlers(ostream& os)
{
	typename std::map<uint16_t, FileHandler<MEMORY_ADDR> *>::iterator it;

	for(it = file_handlers.begin(); it != file_handlers.end(); it++)
	{
		FileHandler<MEMORY_ADDR> *file_handler = it->second;
		if(file_handler != 0)
			os << " - " << file_handler->What() << endl;
	}
}

template <class MEMORY_ADDR>
void FileHandlerRegistry<MEMORY_ADDR>::Reset()
{
	typename std::map<uint16_t, FileHandler<MEMORY_ADDR> *>::iterator it;

	for(it = file_handlers.begin(); it != file_handlers.end(); it++)
	{
		FileHandler<MEMORY_ADDR> *file_handler = it->second;
		if(file_handler) delete file_handler;
	}
}


} // end of namespace coff_loader
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim

#endif
