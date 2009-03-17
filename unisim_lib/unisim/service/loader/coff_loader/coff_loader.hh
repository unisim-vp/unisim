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

template <class MEMORY_ADDR>
class CoffLoader :
	public Client<Memory<MEMORY_ADDR> >,
	public Service<Loader<MEMORY_ADDR> >,
	public Client<SymbolTableBuild<MEMORY_ADDR> >
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

private:
	string filename;
	MEMORY_ADDR entry_point;
	MEMORY_ADDR top_addr;
	MEMORY_ADDR base_addr;
	bool dump_headers;
	Parameter<string> param_filename;
	Parameter<MEMORY_ADDR> param_base_addr;
	Parameter<bool> param_dump_headers;

	struct filehdr
	{
		uint16_t f_magic;   /* magic number */
		uint16_t f_nscns;   /* number of sections */
		uint32_t f_timdat;  /* time and date stamp */
		uint32_t f_symptr;  /* file ptr to symtab */
		uint32_t f_nsyms;   /* # symtab entries */
		uint16_t f_opthdr;  /* sizeof(opt hdr) */
		uint16_t f_flags;   /* flags */
	};

	static const unsigned int SYMNMLEN = 8;
	static const unsigned int FILNMLEN = 14;
	static const unsigned int DIMNUM = 4;
	
	struct scnhdr
	{
		char s_name[SYMNMLEN];/* section name */
		uint32_t s_paddr;    /* physical address */
		uint32_t s_vaddr;    /* virtual address */
		uint32_t s_size;     /* section size */
		uint32_t s_scnptr;   /* file ptr to raw data */
		uint32_t s_relptr;   /* file ptr to relocation */
		uint32_t s_lnnoptr;  /* file ptr to line numbers */
		uint16_t s_nreloc;   /* # reloc entries */
		uint16_t s_nlnno;    /* # line number entries */
		uint32_t s_flags;    /* flags */
	};

	struct reloc
	{
		uint32_t r_vaddr;   /* (virtual) address of reference */
		uint32_t r_symndx;  /* index into symbol table */
		uint16_t r_type;    /* relocation type */
	};

	struct lineno
	{
		union
		{
			uint32_t l_symndx;
			uint32_t l_paddr;
		} l_addr;
		uint16_t l_lnno;
	} ;

};

} // end of namespace coff_loader
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim

#endif
