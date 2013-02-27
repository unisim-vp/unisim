/*
 *  Copyright (c) 2010,
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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_DWARF_HH__
#define __UNISIM_UTIL_DEBUG_DWARF_DWARF_HH__

#include <inttypes.h>
#include <map>
#include <vector>
#include <iosfwd>
#include <unisim/util/debug/dwarf/fwd.hh>
#include <unisim/util/debug/blob/blob.hh>
#include <unisim/util/debug/stmt.hh>
#include <unisim/util/debug/data_object.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/kernel/logger/logger.hh>

#include <unisim/util/debug/dwarf/fmt.hh>
#include <unisim/util/debug/dwarf/abbrev.hh>
#include <unisim/util/debug/dwarf/attr.hh>
#include <unisim/util/debug/dwarf/call_frame_vm.hh>
#include <unisim/util/debug/dwarf/class.hh>
#include <unisim/util/debug/dwarf/die.hh>
#include <unisim/util/debug/dwarf/encoding.hh>
#include <unisim/util/debug/dwarf/fde.hh>
#include <unisim/util/debug/dwarf/leb128.hh>
#include <unisim/util/debug/dwarf/macinfo.hh>
#include <unisim/util/debug/dwarf/pub.hh>
#include <unisim/util/debug/dwarf/stmt_prog.hh>
#include <unisim/util/debug/dwarf/addr_range.hh>
#include <unisim/util/debug/dwarf/call_frame_prog.hh>
#include <unisim/util/debug/dwarf/cie.hh>
#include <unisim/util/debug/dwarf/cu.hh>
#include <unisim/util/debug/dwarf/expr_vm.hh>
#include <unisim/util/debug/dwarf/filename.hh>
#include <unisim/util/debug/dwarf/loc.hh>
#include <unisim/util/debug/dwarf/ml.hh>
#include <unisim/util/debug/dwarf/range.hh>
#include <unisim/util/debug/dwarf/stmt_vm.hh>

#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/stmt_lookup.hh>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

using unisim::util::endian::endian_type;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;

typedef enum
{
	OPT_REG_NUM_MAPPING_FILENAME,
	OPT_VERBOSE
} Option;

template <class MEMORY_ADDR>
class DWARF_Handler
{
public:
	DWARF_Handler(const unisim::util::debug::blob::Blob<MEMORY_ADDR> *blob, unisim::kernel::logger::Logger& logger, unisim::service::interfaces::Registers *regs_if, unisim::service::interfaces::Memory<MEMORY_ADDR> *mem_if);
	~DWARF_Handler();

	void SetOption(Option opt, const char *s);
	void SetOption(Option opt, bool flag);

	void GetOption(Option opt, std::string& s);
	void GetOption(Option opt, bool& flag);

	void Parse();
	void to_XML(const char *output_filename);
	void to_HTML(const char *output_dir);
	
	void Register(DWARF_StatementProgram<MEMORY_ADDR> *dw_stmt_prog);
	void Register(DWARF_DIE<MEMORY_ADDR> *dw_die);
	void Register(DWARF_RangeListEntry<MEMORY_ADDR> *dw_range_list_entry);
	void Register(DWARF_MacInfoListEntry<MEMORY_ADDR> *dw_macinfo_list_entry);
	void Register(DWARF_LocListEntry<MEMORY_ADDR> *dw_loc_list_entry);
	
	const DWARF_StatementProgram<MEMORY_ADDR> *FindStatementProgram(uint64_t debug_line_offset);
	const DWARF_DIE<MEMORY_ADDR> *FindDIE(uint64_t debug_info_offset) const;
	const DWARF_RangeListEntry<MEMORY_ADDR> *FindRangeListEntry(const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu, uint64_t debug_ranges_offset);
	const DWARF_MacInfoListEntry<MEMORY_ADDR> *FindMacInfoListEntry(uint64_t debug_macinfo_offset);
	const DWARF_CompilationUnit<MEMORY_ADDR> *FindCompilationUnit(uint64_t debug_info_offset) const;
	const DWARF_LocListEntry<MEMORY_ADDR> *FindLocListEntry(const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu, uint64_t debug_loc_offset);
	const DWARF_CIE<MEMORY_ADDR> *FindCIE(uint64_t debug_frame_offset) const;
	
	const DWARF_Pub<MEMORY_ADDR> *FindPubName(const char *name) const;
	const DWARF_Pub<MEMORY_ADDR> *FindPubType(const char *name) const;
	const DWARF_DIE<MEMORY_ADDR> *FindDIEByPubName(const char *name) const;
	const DWARF_DIE<MEMORY_ADDR> *FindDIEByPubType(const char *name) const;
	const DWARF_CompilationUnit<MEMORY_ADDR> *FindCompilationUnitByAddrRange(MEMORY_ADDR addr, MEMORY_ADDR length) const;
	const DWARF_DIE<MEMORY_ADDR> *FindSubProgramByAddrRange(MEMORY_ADDR addr, MEMORY_ADDR length) const;
	const DWARF_DIE<MEMORY_ADDR> *FindSubProgram(MEMORY_ADDR pc) const;
	//bool FindDataObjectLocation(const char *name, MEMORY_ADDR pc, DWARF_Location<MEMORY_ADDR>& data_object_loc, MEMORY_ADDR& data_object_bit_size) const;
	const DWARF_DIE<MEMORY_ADDR> *FindDataObjectDIE(const char *name, MEMORY_ADDR pc) const;
	unisim::util::debug::DataObject<MEMORY_ADDR> *FindDataObject(const char *data_object_name, MEMORY_ADDR pc) const;
	
	endian_type GetFileEndianness() const;
	endian_type GetArchEndianness() const;
	uint8_t GetFileAddressSize() const;
	uint8_t GetArchAddressSize() const;
	const DWARF_Abbrev *FindAbbrev(uint64_t debug_abbrev_offset, const DWARF_LEB128& dw_abbrev_code) const;
	const char *GetString(uint64_t debug_str_offset) const;

	const std::map<MEMORY_ADDR, const Statement<MEMORY_ADDR> *>& GetStatements() const;
	const unisim::util::debug::Statement<MEMORY_ADDR> *FindStatement(MEMORY_ADDR addr, typename unisim::service::interfaces::StatementLookup<MEMORY_ADDR>::FindStatementOption opt) const;
	const unisim::util::debug::Statement<MEMORY_ADDR> *FindStatement(const char *filename, unsigned int lineno, unsigned int colno) const;
	
	bool GetCallingConvention(MEMORY_ADDR pc, uint8_t& calling_convention) const;
	unsigned int GetReturnAddressSize(MEMORY_ADDR pc) const;
	std::vector<MEMORY_ADDR> *GetBackTrace(MEMORY_ADDR pc) const;
	const DWARF_FDE<MEMORY_ADDR> *FindFDEByAddr(MEMORY_ADDR pc) const;
	bool GetReturnAddress(MEMORY_ADDR pc, MEMORY_ADDR& ret_addr) const;
	
	DWARF_RegisterNumberMapping *GetRegisterNumberMapping() const;
	unisim::service::interfaces::Memory<MEMORY_ADDR> *GetMemoryInterface() const;

	unisim::kernel::logger::Logger& GetLogger() const;
private:
	endian_type file_endianness;
	endian_type arch_endianness;
	uint8_t file_address_size;
	uint8_t arch_address_size;
	
	// Raw data
	const unisim::util::debug::blob::Section<MEMORY_ADDR> *debug_line_section;     // .debug_line section (raw data)
	const unisim::util::debug::blob::Section<MEMORY_ADDR> *debug_info_section;     // .debug_info section (raw data)
	const unisim::util::debug::blob::Section<MEMORY_ADDR> *debug_abbrev_section;   // .debug_abbrev section (raw data)
	const unisim::util::debug::blob::Section<MEMORY_ADDR> *debug_aranges_section;  // .debug_aranges section (raw data)
	const unisim::util::debug::blob::Section<MEMORY_ADDR> *debug_pubnames_section; // .debug_pubnames section (raw data)
	const unisim::util::debug::blob::Section<MEMORY_ADDR> *debug_pubtypes_section; // .debug_pubtypes section (raw data)
	const unisim::util::debug::blob::Section<MEMORY_ADDR> *debug_macinfo_section;  // .debug_macinfo section (raw data)
	const unisim::util::debug::blob::Section<MEMORY_ADDR> *debug_frame_section;    // .debug_frame section (raw data)
	const unisim::util::debug::blob::Section<MEMORY_ADDR> *debug_str_section;      // .debug_str section (raw data)
	const unisim::util::debug::blob::Section<MEMORY_ADDR> *debug_loc_section;      // .debug_loc section (raw data)
	const unisim::util::debug::blob::Section<MEMORY_ADDR> *debug_ranges_section;   // .debug_ranges section (raw data)
	
	std::map<uint64_t, DWARF_StatementProgram<MEMORY_ADDR> *> dw_stmt_progs;   // statement programs from section .debug_line indexed by .debug_line section offset
	std::map<MEMORY_ADDR, const Statement<MEMORY_ADDR> *> stmt_matrix;               // Result of running dw_stmt_progs on dw_stmt_vms
	std::map<uint64_t, DWARF_CompilationUnit<MEMORY_ADDR> *> dw_cus;           // compilation units contributions to section .debug_info indexed by .debug_info section offset
	std::map<uint64_t, DWARF_DIE<MEMORY_ADDR> *> dw_dies;                      // debug info entries in section .debug_info indexed by .debug_info section offset
	std::map<uint64_t, DWARF_Abbrev *> dw_abbrevs;                             // from section .debug_abbrev indexed by .debug_abbrev section offset
	std::map<uint64_t, DWARF_CIE<MEMORY_ADDR> *> dw_cies;                      // from section .debug_frame indexed by .debug_frame section offset
	std::vector<DWARF_FDE<MEMORY_ADDR> *> dw_fdes;                             // from section .debug_frame
	std::map<uint64_t, DWARF_RangeListEntry<MEMORY_ADDR> *> dw_range_list;     // range list entries in section .debug_ranges indexed by .debug_ranges section offset
	std::map<uint64_t, DWARF_MacInfoListEntry<MEMORY_ADDR> *> dw_macinfo_list; // macinfo list entries in section .debug_macinfo indexed by .debug_macinfo section offset
	std::vector<DWARF_AddressRanges<MEMORY_ADDR> *> dw_aranges;                // from section .debug_aranges
	std::vector<DWARF_Pubs<MEMORY_ADDR> *> dw_pubnames;                        // from section .debug_pubnames
	std::vector<DWARF_Pubs<MEMORY_ADDR> *> dw_pubtypes;                        // from section .debug_pubtypes
	std::map<uint64_t, DWARF_LocListEntry<MEMORY_ADDR> * > dw_loc_list;        // location lists in section .debug_loc indexed by .debug_loc section offset

	unisim::kernel::logger::Logger& logger;
	const unisim::util::debug::blob::Blob<MEMORY_ADDR> *blob;
	std::string reg_num_mapping_filename;
	bool verbose;
	DWARF_RegisterNumberMapping *dw_reg_num_mapping;
	unisim::service::interfaces::Registers *regs_if;
	unisim::service::interfaces::Memory<MEMORY_ADDR> *mem_if;
	
	void DumpStatementMatrix();
	bool IsAbsolutePath(const char *filename) const;
	void BuildStatementMatrix();
};

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
