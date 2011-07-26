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
 
#ifndef __UNISIM_UTIL_LOADER_ELF_LOADER_ELF_LOADER_HH__
#define __UNISIM_UTIL_LOADER_ELF_LOADER_ELF_LOADER_HH__

#include <unisim/kernel/logger/logger.hh>
#include <unisim/util/loader/elf_loader/elf32.h>
#include <unisim/util/loader/elf_loader/elf64.h>
#include <unisim/util/debug/dwarf/dwarf.hh>
#include <unisim/util/debug/blob/blob.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/debug/symbol_table.hh>

#include <iosfwd>

namespace unisim {
namespace util {
namespace loader {
namespace elf_loader {

using namespace std;
using namespace unisim::util::endian;
using unisim::util::debug::Statement;
using unisim::util::debug::Symbol;
using unisim::util::debug::SymbolTable;
using unisim::util::debug::blob::Blob;

typedef enum
{
	OPT_FILENAME,
	OPT_FORCE_BASE_ADDR,
	OPT_FORCE_USE_VIRTUAL_ADDRESS,
	OPT_BASE_ADDR,
	OPT_DUMP_HEADERS,
	OPT_VERBOSE,
	OPT_PARSE_DWARF,
	OPT_DWARF_TO_HTML_OUTPUT_DIRECTORY
} Option;
	
template <class MEMORY_ADDR, unsigned int ElfClass, class Elf_Ehdr, class Elf_Phdr, class Elf_Shdr, class Elf_Sym>
class ElfLoaderImpl
{
public:
	
	ElfLoaderImpl(unisim::kernel::logger::Logger& _logger);
	virtual ~ElfLoaderImpl();
	
	bool Load();
	
	void SetOption(Option opt, MEMORY_ADDR addr);
	void SetOption(Option opt, const char *s);
	void SetOption(Option opt, bool flag);
	
	
	const unisim::util::debug::blob::Blob<MEMORY_ADDR> *GetBlob() const;

	const list<unisim::util::debug::Symbol<MEMORY_ADDR> *> *GetSymbols() const;
	const typename unisim::util::debug::Symbol<MEMORY_ADDR> *FindSymbol(const char *name, MEMORY_ADDR addr, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const;
	const typename unisim::util::debug::Symbol<MEMORY_ADDR> *FindSymbolByAddr(MEMORY_ADDR addr) const;
	const typename unisim::util::debug::Symbol<MEMORY_ADDR> *FindSymbolByName(const char *name) const;
	const typename unisim::util::debug::Symbol<MEMORY_ADDR> *FindSymbolByName(const char *name, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const;
	const typename unisim::util::debug::Symbol<MEMORY_ADDR> *FindSymbolByAddr(MEMORY_ADDR addr, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const;
	
	const unisim::util::debug::Statement<MEMORY_ADDR> *FindStatement(MEMORY_ADDR addr) const;
	const unisim::util::debug::Statement<MEMORY_ADDR> *FindStatement(const char *filename, unsigned int lineno, unsigned int colno) const;
	
	std::vector<MEMORY_ADDR> *GetBackTrace(MEMORY_ADDR pc) const;
private:
	unisim::kernel::logger::Logger& logger;
	string filename;
	MEMORY_ADDR base_addr;
	bool force_base_addr;
	bool force_use_virtual_address;
	bool dump_headers;
	unisim::util::debug::blob::Blob<MEMORY_ADDR> *blob;
	SymbolTable<MEMORY_ADDR> *symbol_table;
	unisim::util::debug::dwarf::DWARF_Handler<MEMORY_ADDR> *dw_handler;
	string dwarf_to_html_output_directory;
	bool verbose;
	endian_type endianness;
	bool parse_dwarf;
	
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
	MEMORY_ADDR GetSectionAlignment(const Elf_Shdr *shdr);
	MEMORY_ADDR GetSectionLink(const Elf_Shdr *shdr);
	bool LoadSection(const Elf_Ehdr *hdr, const Elf_Shdr *shdr, void *buffer, istream& is);
	MEMORY_ADDR GetSegmentType(const Elf_Phdr *phdr);
	MEMORY_ADDR GetSegmentFlags(const Elf_Phdr *phdr);
	MEMORY_ADDR GetSegmentMemSize(const Elf_Phdr *phdr);
	MEMORY_ADDR GetSegmentFileSize(const Elf_Phdr *phdr);
	MEMORY_ADDR GetSegmentAddr(const Elf_Phdr *phdr);
	MEMORY_ADDR GetSegmentAlignment(const Elf_Phdr *phdr);
	bool LoadSegment(const Elf_Ehdr *hdr, const Elf_Phdr *phdr, void *buffer, istream& is);
	MEMORY_ADDR GetSectionFlags(const Elf_Shdr *shdr);
	const char *GetSectionName(const Elf_Shdr *shdr, const char *string_table);
	void DumpRawData(const void *content, MEMORY_ADDR size);
	const char *GetArchitecture(const Elf_Ehdr *hdr) const;
	uint8_t GetAddressSize(const Elf_Ehdr *hdr) const;
};

} // end of namespace elf_loader
} // end of namespace loader
} // end of namespace util
} // end of namespace unisim

#endif
