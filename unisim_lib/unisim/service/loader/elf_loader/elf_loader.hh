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
#include <unisim/service/interfaces/symbol_table_build.hh>

#include <unisim/service/loader/elf_loader/elf32.h>
#include <unisim/service/loader/elf_loader/elf64.h>

#include <unisim/util/endian/endian.hh>
#include <unisim/kernel/service/service.hh>

#include <iostream>
#include <fstream>

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
using unisim::service::interfaces::SymbolTableBuild;
using unisim::service::interfaces::Loader;

template <class MEMORY_ADDR, class T, unsigned int ElfClass, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
class ElfLoaderImpl :
	public Client<Memory<MEMORY_ADDR> >,
	public Service<Loader<MEMORY_ADDR> >,
	public Client<SymbolTableBuild<MEMORY_ADDR> >
{
public:
	ServiceImport<Memory<MEMORY_ADDR> > memory_import;
	ServiceImport<SymbolTableBuild<MEMORY_ADDR> > symbol_table_build_import;
	ServiceExport<Loader<MEMORY_ADDR> > loader_export;

	ElfLoaderImpl(const char *name, Object *parent = 0);
	virtual ~ElfLoaderImpl();

	virtual void OnDisconnect();
	virtual bool Setup();

	virtual void Reset();
	virtual MEMORY_ADDR GetEntryPoint() const;
	virtual MEMORY_ADDR GetTopAddr() const;
	virtual MEMORY_ADDR GetStackBase() const;

private:
	string filename;
	T entry_point;
	T top_addr;
	T base_addr;
	bool force_use_virtual_address;
	Parameter<string> param_filename;
	Parameter<T> param_base_addr;
	Parameter<bool> param_force_use_virtual_address;

	void SwapElfHeader(Elf_Ehdr *hdr);
	void SwapProgramHeader(Elf_Phdr *phdr);
	void SwapSectionHeader(Elf_Shdr *shdr);
	void SwapSymbolEntry(Elf_Sym *sym);
	void AdjustElfHeader(Elf_Ehdr *hdr);
	void AdjustProgramHeader(Elf_Ehdr *hdr, Elf_Phdr *phdr);
	void AdjustSectionHeader(Elf_Ehdr *hdr, Elf_Shdr *shdr);
	void AdjustSymbolEntry(Elf_Ehdr *hdr, Elf_Sym *sym);
	void AdjustSymbolTable(Elf_Ehdr *hdr, Elf_Shdr *shdr, Elf_Sym *sym);
	Elf_Ehdr *ReadElfHeader(istream& is);
	Elf_Phdr *ReadProgramHeaders(Elf_Ehdr *hdr, istream& is);
	Elf_Shdr *ReadSectionHeaders(Elf_Ehdr *hdr, istream& is);
	Elf_Shdr *GetNextSectionHeader(Elf_Ehdr *hdr, Elf_Shdr *shdr);
	char *LoadSectionHeaderStringTable(Elf_Ehdr *hdr, Elf_Shdr *shdr_table, istream& is);
	void DumpElfHeader(Elf_Ehdr *hdr, ostream& os);
	void DumpProgramHeader(Elf_Phdr *phdr, ostream& os);
	void DumpSectionHeader(Elf_Shdr *shdr, char *string_table, ostream& os);
	void DumpSymbol(Elf_Sym *sym, char *string_table, ostream& os);
	void DumpSymbolTable(Elf_Shdr *shdr, char *content, char *string_table, ostream& os);
	T GetSectionSize(Elf_Shdr *shdr);
	T GetSectionAddr(Elf_Shdr *shdr);
	T GetSectionType(Elf_Shdr *shdr);
	bool LoadSection(Elf_Ehdr *hdr, Elf_Shdr *shdr, void *buffer, istream& is);
	bool LoadSegment(Elf_Ehdr *hdr, Elf_Phdr *phdr, void *buffer, istream& is);
	T GetSectionFlags(Elf_Shdr *shdr);
	char *GetSectionName(Elf_Shdr *shdr, char *string_table);
	void BuildSymbolTable(Elf_Shdr *shdr, void *content, char *string_table);
};

typedef ElfLoaderImpl<uint32_t, uint32_t, ELFCLASS32, Elf32_Ehdr, Elf32_Phdr, Elf32_Shdr, Elf32_Sym> Elf32Loader;
typedef ElfLoaderImpl<uint64_t, uint64_t, ELFCLASS64, Elf64_Ehdr, Elf64_Phdr, Elf64_Shdr, Elf64_Sym> Elf64Loader;

} // end of namespace elf_loader
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim

#endif
