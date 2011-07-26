/*
 *  Copyright (c) 2007-2011,
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
 
#ifndef __UNISIM_UTIL_DEBUG_ELF_SYMTAB_ELF_SYMTAB_TCC__
#define __UNISIM_UTIL_DEBUG_ELF_SYMTAB_ELF_SYMTAB_TCC__

#include <unisim/util/loader/elf_loader/elf_common.h>
#include <unisim/util/loader/elf_loader/elf32.h>
#include <unisim/util/loader/elf_loader/elf64.h>
#include <unisim/util/endian/endian.hh>

namespace unisim {
namespace util {
namespace debug {
namespace elf_symtab {

using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class MEMORY_ADDR, class Elf_Sym>
ELF_SymtabHandler<MEMORY_ADDR, Elf_Sym>::ELF_SymtabHandler(unisim::kernel::logger::Logger& _logger, const unisim::util::debug::blob::Blob<MEMORY_ADDR> *_blob)
	: logger(_logger)
	, blob(_blob)
	, symbol_table(0)
{
	if(blob) blob->Catch();
}

template <class MEMORY_ADDR, class Elf_Sym>
ELF_SymtabHandler<MEMORY_ADDR, Elf_Sym>::~ELF_SymtabHandler()
{
	if(blob) blob->Release();
	if(symbol_table) delete symbol_table;
}

template <class MEMORY_ADDR, class Elf_Sym>
void ELF_SymtabHandler<MEMORY_ADDR, Elf_Sym>::Parse()
{
	if(symbol_table) delete symbol_table;
	
	unisim::util::endian::endian_type endian = blob->GetEndian();
	
	symbol_table = new SymbolTable<MEMORY_ADDR>();
	
	const typename std::vector<const unisim::util::debug::blob::Section<MEMORY_ADDR> *>& sections = blob->GetSections();
	typename std::vector<const unisim::util::debug::blob::Section<MEMORY_ADDR> *>::const_iterator section_iter;
	for(section_iter = sections.begin(); section_iter != sections.end(); section_iter++)
	{
		const unisim::util::debug::blob::Section<MEMORY_ADDR> *section = *section_iter;
		
		if(section->GetType() == unisim::util::debug::blob::Section<MEMORY_ADDR>::TY_ELF_SYMTAB)
		{
			const typename unisim::util::debug::blob::Section<MEMORY_ADDR> *string_table_section = blob->GetSection(section->GetLink());
			
			if(!string_table_section || (string_table_section->GetType() != unisim::util::debug::blob::Section<MEMORY_ADDR>::TY_STRTAB))
			{
				logger << DebugWarning << " Found a symbol table section but no string table section. Cannot build symbol table." << EndDebugWarning;
				break;
			}
			const char *string_table = (const char *) string_table_section->GetData();
			
			int nsymbols = section->GetSize() / sizeof(Elf_Sym);
			int i;
			Elf_Sym *sym;

			for(sym = (Elf_Sym *) section->GetData(), i = 0; i < nsymbols; sym++, i++)
			{
				typename Symbol<MEMORY_ADDR>::Type type;

				switch(ELF32_ST_TYPE(unisim::util::endian::Target2Host(endian, sym->st_info)))
				{
					case STT_NOTYPE: type = unisim::util::debug::Symbol<MEMORY_ADDR>::SYM_NOTYPE; break;
					case STT_OBJECT: type = unisim::util::debug::Symbol<MEMORY_ADDR>::SYM_OBJECT; break;
					case STT_FUNC: type = unisim::util::debug::Symbol<MEMORY_ADDR>::SYM_FUNC; break;
					case STT_SECTION: type = unisim::util::debug::Symbol<MEMORY_ADDR>::SYM_SECTION; break;
					case STT_FILE: type = unisim::util::debug::Symbol<MEMORY_ADDR>::SYM_FILE; break;
					case STT_COMMON: type = unisim::util::debug::Symbol<MEMORY_ADDR>::SYM_COMMON; break;
					case STT_TLS: type = unisim::util::debug::Symbol<MEMORY_ADDR>::SYM_TLS; break;
					case STT_NUM: type = unisim::util::debug::Symbol<MEMORY_ADDR>::SYM_NUM; break;
					case STT_LOOS: type = unisim::util::debug::Symbol<MEMORY_ADDR>::SYM_LOOS; break;
					case STT_HIOS: type = unisim::util::debug::Symbol<MEMORY_ADDR>::SYM_HIOS; break;
					case STT_LOPROC: type = unisim::util::debug::Symbol<MEMORY_ADDR>::SYM_LOPROC; break;
					case STT_HIPROC: type = unisim::util::debug::Symbol<MEMORY_ADDR>::SYM_HIPROC; break;
					default: type = unisim::util::debug::Symbol<MEMORY_ADDR>::SYM_NOTYPE;
				}

				std::string symbol_name(string_table + unisim::util::endian::Target2Host(endian, sym->st_name));

				symbol_table->AddSymbol(symbol_name.c_str(), (MEMORY_ADDR) unisim::util::endian::Target2Host(endian, sym->st_value), (MEMORY_ADDR) unisim::util::endian::Target2Host(endian, sym->st_size), type);
			}
		}
	}
	//symbol_table->Dump(cerr);
}

template <class MEMORY_ADDR, class Elf_Sym>
const list<unisim::util::debug::Symbol<MEMORY_ADDR> *> *ELF_SymtabHandler<MEMORY_ADDR, Elf_Sym>::GetSymbols() const
{
	return symbol_table ? symbol_table->GetSymbols() : 0;
}

template <class MEMORY_ADDR, class Elf_Sym>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *ELF_SymtabHandler<MEMORY_ADDR, Elf_Sym>::FindSymbol(const char *name, MEMORY_ADDR addr, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const
{
	return symbol_table ? symbol_table->FindSymbol(name, addr, type) : 0;
}

template <class MEMORY_ADDR, class Elf_Sym>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *ELF_SymtabHandler<MEMORY_ADDR, Elf_Sym>::FindSymbolByAddr(MEMORY_ADDR addr) const
{
	return symbol_table ? symbol_table->FindSymbolByAddr(addr) : 0;
}

template <class MEMORY_ADDR, class Elf_Sym>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *ELF_SymtabHandler<MEMORY_ADDR, Elf_Sym>::FindSymbolByName(const char *name) const
{
	return symbol_table ? symbol_table->FindSymbolByName(name) : 0;
}

template <class MEMORY_ADDR, class Elf_Sym>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *ELF_SymtabHandler<MEMORY_ADDR, Elf_Sym>::FindSymbolByName(const char *name, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const
{
	return symbol_table ? symbol_table->FindSymbolByName(name, type) : 0;
}

template <class MEMORY_ADDR, class Elf_Sym>
const typename unisim::util::debug::Symbol<MEMORY_ADDR> *ELF_SymtabHandler<MEMORY_ADDR, Elf_Sym>::FindSymbolByAddr(MEMORY_ADDR addr, typename unisim::util::debug::Symbol<MEMORY_ADDR>::Type type) const
{
	return symbol_table ? symbol_table->FindSymbolByAddr(addr, type) : 0;
}

} // end of namespace elf_symtab
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
