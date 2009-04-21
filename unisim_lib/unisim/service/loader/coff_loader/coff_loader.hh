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
 
#ifndef __UNISIM_SERVICE_LOADER_COFF_LOADER_COFF_LOADER_HH__
#define __UNISIM_SERVICE_LOADER_COFF_LOADER_COFF_LOADER_HH__

#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/loader.hh>
#include <unisim/service/interfaces/symbol_table_build.hh>

#include <unisim/service/loader/elf_loader/elf32.h>
#include <unisim/service/loader/elf_loader/elf64.h>

#include <unisim/util/endian/endian.hh>
#include <unisim/kernel/service/service.hh>

#include <iosfwd>
#include <inttypes.h>
#include <vector>
#include <map>


namespace unisim {
namespace service {
namespace loader {
namespace coff_loader {

using namespace std;
using unisim::service::interfaces::Memory;
using namespace unisim::util::endian;
using unisim::kernel::service::Service;
using unisim::kernel::service::Client;
using unisim::kernel::service::Object;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::Parameter;
using unisim::util::debug::Symbol;
using unisim::service::interfaces::SymbolTableBuild;
using unisim::service::interfaces::Loader;

template <class MEMORY_ADDR> class FileHandler;
template <class MEMORY_ADDR> class File;
template <class MEMORY_ADDR> class Section;
template <class MEMORY_ADDR> class SectionTable;
template <class MEMORY_ADDR> class FileHandlerRegister;

template <class MEMORY_ADDR>
class FileHandler
{
public:
	virtual uint16_t GetMagic() const = 0;
	virtual const char *What() const = 0;
	virtual File<MEMORY_ADDR> *GetFile() const = 0;
};

template <class MEMORY_ADDR>
class File
{
public:
	virtual bool ParseFileHeader(const void *hdr_raw_data) = 0;
	virtual bool ParseAoutHeader(const void *aout_hdr_raw_data) = 0;
	virtual bool ParseSectionHeader(unsigned int section_num, const void *shdr_raw_data) = 0;

	virtual unsigned int GetMemoryAtomSize() const = 0;
	virtual const char *GetArchitectureName() const = 0;
	virtual bool IsExecutable() const = 0;
	virtual unsigned int GetFileHeaderSize() const = 0;
	virtual unsigned int GetAoutHeaderSize() const = 0;
	virtual unsigned int GetSectionHeaderSize() const = 0;
	virtual unsigned int GetNumSections() const = 0;
	virtual MEMORY_ADDR GetEntryPoint() const = 0;
	virtual MEMORY_ADDR GetTextBase() const = 0;
	virtual MEMORY_ADDR GetDataBase() const = 0;
	virtual MEMORY_ADDR GetTextSize() const = 0;
	virtual MEMORY_ADDR GetDataSize() const = 0;
	virtual MEMORY_ADDR GetBssSize() const = 0;
	virtual void DumpFileHeader(ostream& os) const = 0;
	virtual void DumpAoutHeader(ostream& os) const = 0;

	virtual const SectionTable<MEMORY_ADDR> *GetSectionTable() const = 0;
private:
};

template <class MEMORY_ADDR>
class OutputInterface
{
public:
	virtual bool Write(MEMORY_ADDR addr, const void *content, uint32_t size) = 0;
	virtual void AddSymbol(const char *name, MEMORY_ADDR addr, MEMORY_ADDR size, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) = 0;
};

template <class MEMORY_ADDR>
class Section
{
public:
	typedef enum { ST_LOADABLE_RAWDATA, ST_SPECIFIC_CONTENT, ST_NOT_LOADABLE } Type;
	virtual const char *GetName() const = 0;
	virtual MEMORY_ADDR GetVirtualAddress() const = 0;
	virtual MEMORY_ADDR GetPhysicalAddress() const = 0;
	virtual MEMORY_ADDR GetSize() const = 0;
	virtual long GetContentFilePtr() const = 0;
	virtual void DumpHeader(ostream& os) const = 0;
	virtual Type GetType() const = 0;
	virtual bool LoadSpecificContent(OutputInterface<MEMORY_ADDR> *output, const void *content, uint32_t size) const = 0;
};

template <class MEMORY_ADDR>
class SectionTable
{
public:
	SectionTable(unsigned int num_sections);
	virtual ~SectionTable();
	void Insert(unsigned int section_num, Section<MEMORY_ADDR> *section);
	const Section<MEMORY_ADDR> *operator [] (unsigned int section_num) const;
	unsigned int GetSize() const;
private:
	std::vector<Section<MEMORY_ADDR> *> sections;
};

template <class MEMORY_ADDR>
class FileHandlerRegistry
{
public:
	FileHandlerRegistry();
	virtual ~FileHandlerRegistry();
	void Register(FileHandler<MEMORY_ADDR> *file_handler);
	FileHandler<MEMORY_ADDR> *operator [] (uint16_t magic);
	void DumpFileHandlers(ostream& os);
	void Reset();
private:
	std::map<uint16_t, FileHandler<MEMORY_ADDR> *> file_handlers;
};

template <class MEMORY_ADDR>
class CoffLoader :
	public Client<Memory<MEMORY_ADDR> >,
	public Service<Loader<MEMORY_ADDR> >,
	public Client<SymbolTableBuild<MEMORY_ADDR> >,
	public OutputInterface<MEMORY_ADDR>
{
public:
	ServiceImport<Memory<MEMORY_ADDR> > memory_import;
	ServiceImport<SymbolTableBuild<MEMORY_ADDR> > symbol_table_build_import;
	ServiceExport<Loader<MEMORY_ADDR> > loader_export;

	CoffLoader(const char *name, Object *parent = 0);
	virtual ~CoffLoader();

	virtual void OnDisconnect();
	virtual bool Setup();

	virtual void Reset();
	virtual MEMORY_ADDR GetEntryPoint() const;
	virtual MEMORY_ADDR GetTopAddr() const;
	virtual MEMORY_ADDR GetStackBase() const;

	virtual bool Write(MEMORY_ADDR addr, const void *content, uint32_t size);
	virtual void AddSymbol(const char *name, MEMORY_ADDR addr, MEMORY_ADDR size, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type);

private:
	// Run-time parameters
	string filename;
	MEMORY_ADDR entry_point;
	MEMORY_ADDR top_addr;
	MEMORY_ADDR base_addr;
	bool dump_headers;

	// Run-time parameters (accessors)
	Parameter<string> param_filename;
	Parameter<MEMORY_ADDR> param_base_addr;
	Parameter<bool> param_dump_headers;

	// File handler registry
	FileHandlerRegistry<MEMORY_ADDR> file_handler_registry;

	// Memory atom size of file being loaded
	unsigned int memory_atom_size;
};

} // end of namespace coff_loader
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim

#endif
