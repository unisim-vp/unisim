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
 
#ifndef __UNISIM_SERVICE_LOADER_ELF_LOADER_ELF_LOADER_HH__
#define __UNISIM_SERVICE_LOADER_ELF_LOADER_ELF_LOADER_HH__

#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/loader.hh>
#include <unisim/service/interfaces/symbol_table_lookup.hh>

#include <unisim/service/loader/elf_loader/elf32.h>
#include <unisim/service/loader/elf_loader/elf64.h>

#include <unisim/util/endian/endian.hh>
#include <unisim/util/debug/symbol_table.hh>
#include <unisim/kernel/service/service.hh>

#include <iosfwd>

namespace unisim {
namespace service {
namespace loader {
namespace elf_loader {

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
using unisim::util::debug::SymbolTable;
using unisim::service::interfaces::SymbolTableLookup;
using unisim::service::interfaces::Loader;

template <class MEMORY_ADDR, unsigned int ElfClass, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
class ElfLoaderImpl :
	public Client<Memory<MEMORY_ADDR> >,
	public Service<Loader<MEMORY_ADDR> >,
	public Service<SymbolTableLookup<MEMORY_ADDR> >
{
public:
	ServiceImport<Memory<MEMORY_ADDR> > memory_import;
	ServiceExport<SymbolTableLookup<MEMORY_ADDR> > symbol_table_lookup_export;
	ServiceExport<Loader<MEMORY_ADDR> > loader_export;

	ElfLoaderImpl(const char *name, Object *parent = 0);
	virtual ~ElfLoaderImpl();

	virtual void OnDisconnect();
	virtual bool Setup();

	virtual void Reset();
	virtual MEMORY_ADDR GetEntryPoint() const;
	virtual MEMORY_ADDR GetTopAddr() const;
	virtual MEMORY_ADDR GetStackBase() const;

	virtual const typename unisim::util::debug::Symbol<MEMORY_ADDR> *FindSymbol(const char *name, MEMORY_ADDR addr, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const;
	virtual const typename unisim::util::debug::Symbol<MEMORY_ADDR> *FindSymbolByAddr(MEMORY_ADDR addr) const;
	virtual const typename unisim::util::debug::Symbol<MEMORY_ADDR> *FindSymbolByName(const char *name) const;
	virtual const typename unisim::util::debug::Symbol<MEMORY_ADDR> *FindSymbolByName(const char *name, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const;
	virtual const typename unisim::util::debug::Symbol<MEMORY_ADDR> *FindSymbolByAddr(MEMORY_ADDR addr, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const;

private:
	string filename;
	MEMORY_ADDR entry_point;
	MEMORY_ADDR top_addr;
	MEMORY_ADDR base_addr;
	bool force_use_virtual_address;
	bool dump_headers;
	SymbolTable<MEMORY_ADDR> symbol_table;
	Parameter<string> param_filename;
	Parameter<MEMORY_ADDR> param_base_addr;
	Parameter<bool> param_force_use_virtual_address;
	Parameter<bool> param_dump_headers;

	void SwapElfHeader(Elf_Ehdr *hdr);
	void SwapProgramHeader(Elf_Phdr *phdr);
	void SwapSectionHeader(Elf_Shdr *shdr);
	void SwapSymbolEntry(Elf_Sym *sym);
	void AdjustElfHeader(Elf_Ehdr *hdr);
	void AdjustProgramHeader(const Elf_Ehdr *hdr, Elf_Phdr *phdr);
	void AdjustSectionHeader(const Elf_Ehdr *hdr, Elf_Shdr *shdr);
	void AdjustSymbolEntry(const Elf_Ehdr *hdr, Elf_Sym *sym);
	void AdjustSymbolTable(const Elf_Ehdr *hdr, const Elf_Shdr *shdr, Elf_Sym *sym);
	Elf_Ehdr *ReadElfHeader(istream& is);
	Elf_Phdr *ReadProgramHeaders(const Elf_Ehdr *hdr, istream& is);
	Elf_Shdr *ReadSectionHeaders(const Elf_Ehdr *hdr, istream& is);
	const Elf_Shdr *GetNextSectionHeader(const Elf_Ehdr *hdr, const Elf_Shdr *shdr);
	char *LoadSectionHeaderStringTable(const Elf_Ehdr *hdr, const Elf_Shdr *shdr_table, istream& is);
	void DumpElfHeader(const Elf_Ehdr *hdr, ostream& os);
	void DumpProgramHeader(const Elf_Phdr *phdr, ostream& os);
	void DumpSectionHeader(const Elf_Shdr *shdr, const char *string_table, ostream& os);
	void DumpSymbol(const Elf_Sym *sym, const char *string_table, ostream& os);
	void DumpSymbolTable(const Elf_Shdr *shdr, const char *content, const char *string_table, ostream& os);
	MEMORY_ADDR GetSectionSize(const Elf_Shdr *shdr);
	MEMORY_ADDR GetSectionAddr(const Elf_Shdr *shdr);
	MEMORY_ADDR GetSectionType(const Elf_Shdr *shdr);
	bool LoadSection(const Elf_Ehdr *hdr, const Elf_Shdr *shdr, void *buffer, istream& is);
	bool LoadSegment(const Elf_Ehdr *hdr, const Elf_Phdr *phdr, void *buffer, istream& is);
	MEMORY_ADDR GetSectionFlags(const Elf_Shdr *shdr);
	const char *GetSectionName(const Elf_Shdr *shdr, const char *string_table);
	void BuildSymbolTable(const Elf_Shdr *shdr, const void *content, const char *string_table);
};

} // end of namespace elf_loader
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim

#endif
