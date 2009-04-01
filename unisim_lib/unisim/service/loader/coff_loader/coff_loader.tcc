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
#include <stdlib.h>
#include <unisim/util/endian/endian.hh>

#if defined(__GNUC__) && (__GNUC__ >= 3)
#define PACKED __attribute__ ((packed))
#else
#define PACKED
#endif

#include <unisim/service/loader/coff_loader/ti/ti.hh>
#include <unisim/service/loader/coff_loader/ti/ti.tcc>

namespace unisim {
namespace service {
namespace loader {
namespace coff_loader {

template <class MEMORY_ADDR>
CoffLoader<MEMORY_ADDR>::CoffLoader(const char *name, Object *parent)
	: Object(name, parent)
	, Client<Memory<MEMORY_ADDR> >(name, parent)
	, Client<SymbolTableBuild<MEMORY_ADDR> >(name, parent)
	, Service<Loader<MEMORY_ADDR> >(name, parent)
	, memory_import("memory-import", this)
	, symbol_table_build_import("symbol-table-build-import", this)
	, loader_export("loader-export", this)
	, filename()
	, entry_point(0)
	, base_addr(0)
	, top_addr(0)
	, dump_headers(false)
	, param_filename("filename", this, filename)
	, param_base_addr("base-addr", this, base_addr)
	, param_dump_headers("dump-headers", this, dump_headers)
{
	Object::SetupDependsOn(memory_import);
	Object::SetupDependsOn(symbol_table_build_import);

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

	if(is.seekg(0, ios::beg).fail())
	{
		return false;
	}

	// Read the magic number
	uint16_t magic;

	if(is.read((char *) &magic, sizeof(magic)).fail())
	{
		cerr << Object::GetName() << ": ERROR! Could not read file header or \"" << filename << "\" is not a COFF file." << endl;
		return false;
	}

	FileHandler<MEMORY_ADDR> *file_handler = file_handler_registry[unisim::util::endian::LittleEndian2Host(magic)];

	if(!file_handler)
	{
		cerr << Object::GetName() << ": ERROR! Can't handle format of \"" << filename << "\"." << endl;
		cerr << "Supported formats are:" << endl;
		file_handler_registry.DumpFileHandlers(cerr);
		return false;
	}

	File<MEMORY_ADDR> *file = file_handler->GetFile();

	if(!file)
	{
		cerr << Object::GetName() << ": INTERNAL ERROR! " << file_handler->What() << " can't handle format of \"" << filename << "\"" << endl;
		return false;
	}

	// Read the remaining bytes in file header
	unsigned int file_hdr_size = file->GetFileHeaderSize();

	char hdr[file_hdr_size];

	*(uint16_t *) hdr = magic;
	if(is.read(hdr + sizeof(magic), file_hdr_size - sizeof(magic)).fail())
	{
		cerr << Object::GetName() << ": ERROR! Could not read file header or \"" << filename << "\" is not a COFF file." << endl;
		delete file;
		return false;
	}

	if(!file->ParseFileHeader(hdr))
	{
		cerr << Object::GetName() << ": ERROR! File header is not supported or \"" << filename << "\" is not a COFF file." << endl;
		delete file;
		return false;
	}

	if(dump_headers)
	{
		file->DumpFileHeader(cerr);
	}

	cerr << Object::GetName() << ": File \"" << filename << "\" is for " << file->GetArchitectureName() << endl;

	if(!file->IsExecutable())
	{
		cerr << Object::GetName() << ": ERROR! File \"" << filename << "\" is not an executable COFF file." << endl;
		delete file;
		return false;
	}

	cerr << Object::GetName() << ": File \"" << filename << "\" is executable" << endl;

	unsigned int aout_hdr_size = file->GetAoutHeaderSize();

	if(aout_hdr_size > 0)
	{
		char aout_hdr[aout_hdr_size];

		if(is.read(aout_hdr, aout_hdr_size).fail())
		{
			cerr << Object::GetName() << ": ERROR! Could not read optional header." << endl;
			delete file;
			return false;
		}

		if(!file->ParseAoutHeader(aout_hdr))
		{
			cerr << Object::GetName() << ": ERROR! optional header is invalid or unsupported." << endl;
			delete file;
			return false;
		}

		entry_point = file->GetEntryPoint();

		cerr << Object::GetName() << ": program entry point at 0x" << hex << entry_point << dec << endl;

		if(dump_headers)
		{
			file->DumpAoutHeader(cerr);
		}
	}
	else
	{
		cerr << Object::GetName() << ": WARNING! File \"" << filename << "\" has no optional header." << endl;
		cerr << Object::GetName() << ": program entry point is unknown" << endl;
	}

	unsigned int num_sections = file->GetNumSections();
	unsigned int shdr_size = file->GetSectionHeaderSize();
	unsigned int memory_atom_size = file->GetMemoryAtomSize();
	unsigned int section_num;

	for(section_num = 0; section_num < num_sections; section_num++)
	{
		char shdr[shdr_size];

		if(is.read(shdr, shdr_size).fail())
		{
			cerr << Object::GetName() << ": ERROR! Can't read section headers" << endl;
			delete file;
			return false;
		}
		
		if(!file->ParseSectionHeader(section_num, shdr))
		{
			cerr << Object::GetName() << ": ERROR! section header #" << section_num << " is invalid or unsupported" << endl;
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
			section->DumpHeader(cerr);
		}

		if(section->IsLoadable())
		{
			const char *section_name = section->GetName();
			MEMORY_ADDR section_addr = section->GetPhysicalAddress();
			MEMORY_ADDR section_size = section->GetSize();
			long section_content_file_ptr = section->GetContentFilePtr();

			cerr << Object::GetName() << ": Loading section " << section_name << " at 0x" << hex << section_addr << dec << " (" << (section_size * memory_atom_size) << " bytes) " << endl;

			if(section_size > 0)
			{
				void *section_content = calloc(section_size, memory_atom_size);

				if(is.seekg(section_content_file_ptr, ios::beg).fail() || is.read((char *) section_content, section_size * memory_atom_size).fail())
				{
					cerr << Object::GetName() << ": WARNING! Can't load section " << section_name << endl;
					success = false;
				}
				else
				{
					if(!memory_import->WriteMemory(section_addr * memory_atom_size, section_content, section_size * memory_atom_size))
					{
						cerr << Object::GetName() << ": Can't write into memory (@0x" << hex << section_addr << " - @0x" << (section_addr +  section_size - 1) << dec << ")" << endl;
						success = false;
					}
					else 
					{
						cerr << Object::GetName() << ": write into memory (@0x" << hex << section_addr << " - @0x" << (section_addr +  section_size - 1) << dec << ")" << endl;
					}

					free(section_content);
				}
			}
		}
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
