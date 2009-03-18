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

#if defined(__GNUC__) && (__GNUC__ >= 3)
#define PACKED __attribute__ ((packed))
#else
#define PACKED
#endif

const uint16_t TI_TARGET_ID_TMS320_C3X_C4X = 0x0093;

const uint16_t TI_COFF0_MAGIC = TI_TARGET_ID_TMS320_C3X_C4X;
const uint16_t TI_COFF1_MAGIC = 0x00c1;
const uint16_t TI_COFF2_MAGIC = 0x00c2;

// TI's COFF file header
typedef struct PACKED
{
	uint16_t f_magic;     // magic number
	uint16_t f_nscns;     // number of sections
	int32_t f_timdat;     // time and date stamp
	int32_t f_symptr;     // file ptr to symtab
	int32_t f_nsyms;      // number of symtab entries
	uint16_t f_opthdr;    // size of optional header
	uint16_t f_flags;     // flags
	uint16_t f_target_id; // COFF-TI specific: TI target magic number that can execute the file
} ti_filehdr;

typedef union
{
	uint16_t f_magic;     // magic number
	ti_filehdr ti;
} filehdr;

// TI's COFF Optional file header
typedef struct PACKED
{
	int16_t o_magic;        // magic number (0x108)
	int16_t o_vstamp;       // version stamp
	int32_t o_tsize;        // text size in bytes, padded to FW bdry
	int32_t o_dsize;        // initialized data
	int32_t o_bsize;        // uninitialized data
	int32_t o_entry;        // entry point
	int32_t o_text_start;   // base of text used for this file
	int32_t o_data_start;   // base of data used for this file
} ti_aouthdr;

typedef union
{
	int16_t magic;        // magic number
	ti_aouthdr ti;
} aouthdr;

// Section header for TI's COFF0 and COFF1 files
typedef struct PACKED
{
	char s_name[8];        // section name
	int32_t s_paddr;       // physical address
	int32_t s_vaddr;       // virtual address
	int32_t s_size;        // section size
	int32_t s_scnptr;      // file ptr to raw data
	int32_t s_relptr;      // file ptr to relocation
	int32_t s_lnnoptr;     // file ptr to line numbers
	uint16_t s_nreloc;     // number of reloc entries
	uint16_t s_nlnno;      // number of line number entries
	uint16_t s_flags;      // flags
	int8_t s_reserved;     // reserved
	uint8_t s_page;        // memory page number */
} ti_scnhdr_v01;

// Section header for TI's COFF2 files
typedef struct PACKED
{
	char s_name[8];        // section name
	int32_t s_paddr;       // physical address
	int32_t s_vaddr;       // virtual address
	int32_t s_size;        // section size
	int32_t s_scnptr;      // file ptr to raw data
	int32_t s_relptr;      // file ptr to relocation
	int32_t s_lnnoptr;     // file ptr to line numbers
	uint32_t s_nreloc;     // number of reloc entries
	uint32_t s_nlnno;      // number of line number entries
	uint32_t s_flags;      // flags
	int16_t s_reserved;    // reserved
	uint16_t s_page;       // memory page number */
} ti_scnhdr_v2;

typedef union
{
	ti_scnhdr_v01 ti_v01;
	ti_scnhdr_v2 ti_v2;
} scnhdr;

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
	// Run-time parameters
	string filename;
	MEMORY_ADDR entry_point;
	MEMORY_ADDR top_addr;
	MEMORY_ADDR base_addr;
	bool dump_headers;
	endian_type endianness;

	// Run-time parameters (accessors)
	Parameter<string> param_filename;
	Parameter<MEMORY_ADDR> param_base_addr;
	Parameter<bool> param_dump_headers;
	Parameter<endian_type> param_endianness;

	// Target to Host endian conversion
	void Target2Host(filehdr *hdr);
	void Target2Host(ti_filehdr *hdr);
	void Target2Host(const filehdr *hdr, scnhdr *shdr);
	void Target2Host(ti_scnhdr_v01 *shdr);
	void Target2Host(ti_scnhdr_v2 *shdr);

	// File handling
	filehdr *ReadFileHeader(istream& is);
	scnhdr *ReadSectionHeaders(const filehdr *hdr, istream& is);
	unsigned int GetFileHeaderSize(const filehdr *hdr);
	unsigned int GetSectionHeaderSize(const filehdr *hdr);
	bool LoadSection(const filehdr *hdr, const scnhdr *shdr, void *buffer, istream& is);

	// Dump headers
	void DumpFileHeader(const filehdr *hdr, ostream& os);
	void DumpFileHeader(const ti_filehdr *hdr, ostream& os);
	void DumpSectionHeader(const filehdr *hdr, const scnhdr *shdr, ostream& os);
	void DumpSectionHeader(const ti_scnhdr_v01 *shdr, ostream& os);
	void DumpSectionHeader(const ti_scnhdr_v2 *shdr, ostream& os);

	// Navigate into headers
	unsigned int GetNumSectionHeaders(const filehdr *hdr);
	const scnhdr *GetNextSectionHeader(const filehdr *hdr, const scnhdr *shdr);
	const char *GetSectionName(string& name, const filehdr *hdr, const scnhdr *shdr);
	MEMORY_ADDR GetSectionVirtualAddress(const filehdr *hdr, const scnhdr *shdr);
	MEMORY_ADDR GetSectionPhysicalAddress(const filehdr *hdr, const scnhdr *shdr);
	MEMORY_ADDR GetSectionVirtualAddress(const ti_scnhdr_v01 *shdr);
	MEMORY_ADDR GetSectionVirtualAddress(const ti_scnhdr_v2 *shdr);
	MEMORY_ADDR GetSectionPhysicalAddress(const ti_scnhdr_v01 *shdr);
	MEMORY_ADDR GetSectionPhysicalAddress(const ti_scnhdr_v2 *shdr);
	MEMORY_ADDR GetSectionSize(const filehdr *hdr, const scnhdr *shdr);
	MEMORY_ADDR GetSectionSize(const ti_scnhdr_v01 *shdr);
	MEMORY_ADDR GetSectionSize(const ti_scnhdr_v2 *shdr);
	long GetSectionFilePtr(const filehdr *hdr, const scnhdr *shdr);
	long GetSectionFilePtr(const ti_scnhdr_v01 *shdr);
	long GetSectionFilePtr(const ti_scnhdr_v2 *shdr);
};

} // end of namespace coff_loader
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim

#endif
