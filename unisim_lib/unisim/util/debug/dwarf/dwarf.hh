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
#include <unisim/util/blob/blob.hh>
#include <unisim/util/debug/stmt.hh>
#include <unisim/util/debug/data_object.hh>
#include <unisim/util/debug/subprogram.hh>
#include <unisim/util/debug/variable.hh>
#include <unisim/util/debug/symbol_table.hh>
#include <unisim/util/endian/endian.hh>

#include <unisim/util/debug/dwarf/fmt.hh>
#include <unisim/util/debug/dwarf/version.hh>
#include <unisim/util/debug/dwarf/option.hh>
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
#include <unisim/util/debug/dwarf/c_loc_expr_parser.hh>
#include <unisim/util/debug/dwarf/cfa.hh>
#include <unisim/util/debug/dwarf/frame.hh>
#include <unisim/util/debug/dwarf/data_object.hh>
#include <unisim/util/debug/dwarf/subprogram.hh>
#include <unisim/util/debug/dwarf/variable.hh>
#include <unisim/util/debug/dwarf/machine_state.hh>

#include <unisim/service/interfaces/registers.hh>
#include <unisim/service/interfaces/memory.hh>
#include <unisim/service/interfaces/stmt_lookup.hh>
#include <unisim/service/interfaces/data_object_lookup.hh>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

using unisim::util::endian::endian_type;

template <class MEMORY_ADDR>
class DWARF_Handler
{
public:
	// Constructor
	DWARF_Handler(const unisim::util::blob::Blob<MEMORY_ADDR> *blob, const unisim::util::debug::SymbolTable<MEMORY_ADDR> *symbol_table);
	
	// Destructor
	~DWARF_Handler();
	
	// Logging interface
	void SetDebugInfoStream(std::ostream& debug_info_stream);
	void SetDebugWarningStream(std::ostream& debug_warning_stream);
	void SetDebugErrorStream(std::ostream& debug_error_stream);
	
	// Architecture state interfaces
	void SetRegistersInterface(unsigned int prc_num, unisim::service::interfaces::Registers *regs_if);
	void SetMemoryInterface(unsigned int prc_num, unisim::service::interfaces::Memory<MEMORY_ADDR> *mem_if);
	
	// Option management
	bool SetOption(Option opt, const char *s);
	bool SetOption(Option opt, bool flag);
	bool SetOption(Option opt, int value);

	bool GetOption(Option opt, std::string& s) const;
	bool GetOption(Option opt, bool& flag) const;
	bool GetOption(Option opt, int& value) const;
	
	const bool& GetOptionFlag(Option opt) const;
	const std::string& GetOptionString(Option opt) const;
	const int& GetOptionInt(Option opt) const;

	// Parsing/Deserialization of DWARF debug infos
	void Parse();
	
	// Dump
	void to_XML(const char *output_filename);
	void to_HTML(const char *output_dir);

	// Visitor pattern (CU and DIE)
// 	template <typename T, typename VISITOR> T Scan(VISITOR& visitor) const;
	template <typename VISITOR, typename T = bool> T Scan(VISITOR& visitor) const;
	
	// Usefull Low level methods for visitors
	const DWARF_DIE<MEMORY_ADDR> *FindSubProgramDIE(const char *sub_program_name, const char *compilation_unit_name) const;
	const DWARF_DIE<MEMORY_ADDR> *FindVariableDIE(const char *variable_name, const char *compilation_unit_name) const;
	
	// High level interface
	bool HasDebugInfo() const;
	void ScanStatements(unisim::service::interfaces::StatementScanner<MEMORY_ADDR>& scanner) const;
	const unisim::util::debug::Statement<MEMORY_ADDR> *FindStatement(MEMORY_ADDR addr, typename unisim::service::interfaces::StatementLookup<MEMORY_ADDR>::Scope scope) const;
	const unisim::util::debug::Statement<MEMORY_ADDR> *FindStatements(unisim::service::interfaces::StatementScanner<MEMORY_ADDR>& scanner, MEMORY_ADDR addr, typename unisim::service::interfaces::StatementLookup<MEMORY_ADDR>::Scope scope) const;
	const unisim::util::debug::Statement<MEMORY_ADDR> *FindStatement(const unisim::util::debug::SourceCodeLocation& source_code_location) const;
	const unisim::util::debug::Statement<MEMORY_ADDR> *FindStatements(unisim::service::interfaces::StatementScanner<MEMORY_ADDR>& scanner, const unisim::util::debug::SourceCodeLocation& source_code_location) const;
	std::vector<MEMORY_ADDR> *GetBackTrace(unsigned int prc_num) const;
	bool GetReturnAddress(unsigned int prc_num, MEMORY_ADDR& ret_addr) const;
	unisim::util::debug::DataObject<MEMORY_ADDR> *FindDataObject(unsigned int prc_num, const char *data_object_name) const;
	unisim::util::debug::DataObject<MEMORY_ADDR> *FindDataObject(const DWARF_MachineState<MEMORY_ADDR> *dw_mach_state, unsigned int prc_num, const char *data_object_name) const;
	void ScanDataObjectNames(unsigned int prc_num, unisim::service::interfaces::DataObjectNameScanner& scanner, typename unisim::service::interfaces::DataObjectLookupBase::Scope scope = unisim::service::interfaces::DataObjectLookupBase::SCOPE_BOTH_GLOBAL_AND_LOCAL) const;
	void ScanDataObjectNames(const DWARF_MachineState<MEMORY_ADDR> *dw_mach_state, unsigned int prc_num, unisim::service::interfaces::DataObjectNameScanner& scanner, typename unisim::service::interfaces::DataObjectLookupBase::Scope scope = unisim::service::interfaces::DataObjectLookupBase::SCOPE_BOTH_GLOBAL_AND_LOCAL) const;
	const DWARF_SubProgram<MEMORY_ADDR> *FindSubProgram(const char *subprogram_name, const char *filename = 0, const char *compilation_unit_name = 0) const;
	const DWARF_SubProgram<MEMORY_ADDR> *FindSubProgram(MEMORY_ADDR pc, const char *filename = 0) const;
	unisim::util::debug::DataObject<MEMORY_ADDR> *GetReturnValue(unsigned int prc_num) const;
	unisim::util::debug::DataObject<MEMORY_ADDR> *GetReturnValue(const DWARF_MachineState<MEMORY_ADDR> *dw_mach_state, unsigned int prc_num) const;
	unisim::util::debug::DataObject<MEMORY_ADDR> *GetSubProgramParameter(unsigned int prc_num, unsigned int index) const;
	unisim::util::debug::DataObject<MEMORY_ADDR> *GetSubProgramParameter(const DWARF_MachineState<MEMORY_ADDR> *dw_mach_state, unsigned int prc_num, unsigned int index) const;
	
private:
	friend class DWARF_StatementProgram<MEMORY_ADDR>;
	friend class DWARF_StatementVM<MEMORY_ADDR>;
	friend class DWARF_AttributeValue<MEMORY_ADDR>;
	friend class DWARF_LinePtr<MEMORY_ADDR>;
	friend class DWARF_LocListPtr<MEMORY_ADDR>;
	friend class DWARF_MacPtr<MEMORY_ADDR>;
	friend class DWARF_RangeListPtr<MEMORY_ADDR>;
	friend class DWARF_Reference<MEMORY_ADDR>;
	friend class DWARF_Attribute<MEMORY_ADDR>;
	friend class DWARF_DIE<MEMORY_ADDR>;
	friend class DWARF_CompilationUnit<MEMORY_ADDR>;
	friend class DWARF_Location<MEMORY_ADDR>;
	friend class DWARF_ExpressionVM<MEMORY_ADDR>;
	friend class DWARF_CallFrameProgram<MEMORY_ADDR>;
	friend class DWARF_CIE<MEMORY_ADDR>;
	friend class DWARF_FDE<MEMORY_ADDR>;
	friend class DWARF_CallFrameVM<MEMORY_ADDR>;
	friend class DWARF_RegSet<MEMORY_ADDR>;
	friend class DWARF_Frame<MEMORY_ADDR>;
	friend class DWARF_RangeListEntry<MEMORY_ADDR>;
	friend class DWARF_MacInfoListEntry<MEMORY_ADDR>;
	friend class DWARF_MacInfoListEntryDefine<MEMORY_ADDR>;
	friend class DWARF_MacInfoListEntryUndef<MEMORY_ADDR>;
	friend class DWARF_MacInfoListEntryStartFile<MEMORY_ADDR>;
	friend class DWARF_MacInfoListEntryEndFile<MEMORY_ADDR>;
	friend class DWARF_MacInfoListEntryVendorExtension<MEMORY_ADDR>;
	friend class DWARF_AddressRanges<MEMORY_ADDR>;
	friend class DWARF_Pub<MEMORY_ADDR>;
	friend class DWARF_Pubs<MEMORY_ADDR>;
	friend class DWARF_LocListEntry<MEMORY_ADDR>;
	friend class DWARF_DataObject<MEMORY_ADDR>;
	friend class DWARF_SubProgram<MEMORY_ADDR>;
	friend class DWARF_MachineState<MEMORY_ADDR>;
	
	endian_type file_endianness;
	endian_type arch_endianness;
	uint8_t arch_address_size;
	bool inclusive_fde_addr_range;
	
	// Raw data
	const unisim::util::blob::Section<MEMORY_ADDR> *debug_line_section;     // .debug_line section (raw data)
	const unisim::util::blob::Section<MEMORY_ADDR> *debug_info_section;     // .debug_info section (raw data)
	const unisim::util::blob::Section<MEMORY_ADDR> *debug_abbrev_section;   // .debug_abbrev section (raw data)
	const unisim::util::blob::Section<MEMORY_ADDR> *debug_aranges_section;  // .debug_aranges section (raw data)
	const unisim::util::blob::Section<MEMORY_ADDR> *debug_pubnames_section; // .debug_pubnames section (raw data)
	const unisim::util::blob::Section<MEMORY_ADDR> *debug_pubtypes_section; // .debug_pubtypes section (raw data)
	const unisim::util::blob::Section<MEMORY_ADDR> *debug_macinfo_section;  // .debug_macinfo section (raw data)
	const unisim::util::blob::Section<MEMORY_ADDR> *debug_frame_section;    // .debug_frame section (raw data)
	const unisim::util::blob::Section<MEMORY_ADDR> *eh_frame_section;       // .eh_frame section (raw data)
	const unisim::util::blob::Section<MEMORY_ADDR> *debug_str_section;      // .debug_str section (raw data)
	const unisim::util::blob::Section<MEMORY_ADDR> *debug_loc_section;      // .debug_loc section (raw data)
	const unisim::util::blob::Section<MEMORY_ADDR> *debug_ranges_section;   // .debug_ranges section (raw data)
	
	// Sumbol Table
	const unisim::util::debug::SymbolTable<MEMORY_ADDR> *symbol_table;
	
	// Deserialized DWARF debug infos
	std::map<uint64_t, DWARF_StatementProgram<MEMORY_ADDR> *> dw_stmt_progs;   // statement programs from section .debug_line indexed by .debug_line section offset
	std::multimap<MEMORY_ADDR, const Statement<MEMORY_ADDR> *> stmt_matrix;    // Result of running dw_stmt_progs on dw_stmt_vms
	std::map<uint64_t, DWARF_CompilationUnit<MEMORY_ADDR> *> dw_cus;           // compilation units contributions to section .debug_info indexed by .debug_info section offset
	std::map<uint64_t, DWARF_DIE<MEMORY_ADDR> *> dw_dies;                      // debug info entries in section .debug_info indexed by .debug_info section offset
	std::map<uint64_t, DWARF_Abbrev *> dw_abbrevs;                             // from section .debug_abbrev indexed by .debug_abbrev section offset
	std::map<uint64_t, DWARF_CIE<MEMORY_ADDR> *> dw_cies[2];                   // from section .debug_frame/.eh_frame indexed by .debug_frame/.eh_frame section offset
	std::vector<DWARF_FDE<MEMORY_ADDR> *> dw_fdes[2];                          // from section .debug_frame/.eh_frame
	std::map<uint64_t, DWARF_RangeListEntry<MEMORY_ADDR> *> dw_range_list;     // range list entries in section .debug_ranges indexed by .debug_ranges section offset
	std::map<uint64_t, DWARF_MacInfoListEntry<MEMORY_ADDR> *> dw_macinfo_list; // macinfo list entries in section .debug_macinfo indexed by .debug_macinfo section offset
	std::vector<DWARF_AddressRanges<MEMORY_ADDR> *> dw_aranges;                // from section .debug_aranges
	std::vector<DWARF_Pubs<MEMORY_ADDR> *> dw_pubnames;                        // from section .debug_pubnames
	std::vector<DWARF_Pubs<MEMORY_ADDR> *> dw_pubtypes;                        // from section .debug_pubtypes
	std::map<uint64_t, DWARF_LocListEntry<MEMORY_ADDR> * > dw_loc_list;        // location lists in section .debug_loc indexed by .debug_loc section offset
	
	// Blob with raw debug infos
	const unisim::util::blob::Blob<MEMORY_ADDR> *blob;
	
	// DIECache (name -> DIE)
	typedef std::map<std::string, const DWARF_DIE<MEMORY_ADDR> *> DIECache;
	mutable DIECache subprogram_die_cache;
	mutable DIECache variable_die_cache;
	
	// FDECache (pc -> FDE)
	typedef std::map<MEMORY_ADDR, const DWARF_FDE<MEMORY_ADDR> *> FDECache;
	mutable FDECache fde_cache;
	// CFI
	typedef std::vector<const DWARF_CFI<MEMORY_ADDR> *> CFIs;
	mutable CFIs dw_cfis;
	// CFICache (pc -> CFI)
	typedef std::map<MEMORY_ADDR, const DWARF_CFI<MEMORY_ADDR> *> CFICache;
	mutable CFICache cfi_cache;
	
	// Debug stuf
	std::ostream *debug_info_stream;
	std::ostream *debug_warning_stream;
	std::ostream *debug_error_stream;
	std::string reg_num_mapping_filename;
	bool verbose;
	bool debug;
	
	// Machine state
	mutable DWARF_MachineState<MEMORY_ADDR> auto_dw_mach_state;
	
	// C location expression parser
	mutable CLocExprParser c_loc_expr_parser;
	
	// Registration methods of deserialized DWARF debug infos classes
	void Register(DWARF_StatementProgram<MEMORY_ADDR> *dw_stmt_prog);
	void Register(DWARF_DIE<MEMORY_ADDR> *dw_die);
	void Register(DWARF_RangeListEntry<MEMORY_ADDR> *dw_range_list_entry);
	void Register(DWARF_MacInfoListEntry<MEMORY_ADDR> *dw_macinfo_list_entry);
	void Register(DWARF_LocListEntry<MEMORY_ADDR> *dw_loc_list_entry);
	void Register(DWARF_CIE<MEMORY_ADDR> *dw_cie);
	void UnRegister(DWARF_DIE<MEMORY_ADDR> *dw_die);

	// Searching methods for deserialized DWARF debug infos class instance by DWARF debug infos raw data offset
	const DWARF_StatementProgram<MEMORY_ADDR> *FindStatementProgram(uint64_t debug_line_offset);
	const DWARF_DIE<MEMORY_ADDR> *FindDIE(uint64_t debug_info_offset) const;
	const DWARF_RangeListEntry<MEMORY_ADDR> *FindRangeListEntry(const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu, uint64_t debug_ranges_offset);
	const DWARF_MacInfoListEntry<MEMORY_ADDR> *FindMacInfoListEntry(uint64_t debug_macinfo_offset);
	const DWARF_CompilationUnit<MEMORY_ADDR> *FindCompilationUnit(uint64_t debug_info_offset) const;
	const DWARF_LocListEntry<MEMORY_ADDR> *FindLocListEntry(const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu, uint64_t debug_loc_offset);
	const DWARF_CIE<MEMORY_ADDR> *FindCIE(uint64_t debug_frame_offset, DWARF_FrameSectionType fst);
	const DWARF_Abbrev *FindAbbrev(uint64_t debug_abbrev_offset, const DWARF_LEB128& dw_abbrev_code) const;
	const char *GetString(uint64_t debug_str_offset) const;
	
	// Various searching methods by address, name, C location expression
	const DWARF_Pub<MEMORY_ADDR> *FindPubName(const char *name) const;
	const DWARF_Pub<MEMORY_ADDR> *FindPubType(const char *name) const;
	const DWARF_DIE<MEMORY_ADDR> *FindDIEByPubName(const char *name) const;
	const DWARF_DIE<MEMORY_ADDR> *FindDIEByPubType(const char *name) const;
	const DWARF_DIE<MEMORY_ADDR> *FindDIEByName(unsigned int dw_tag, const char *name, bool external) const;
	const DWARF_CompilationUnit<MEMORY_ADDR> *FindCompilationUnitByAddrRange(MEMORY_ADDR addr, MEMORY_ADDR length) const;
	const DWARF_CompilationUnit<MEMORY_ADDR> *FindCompilationUnitByName(const char *name) const;
	const DWARF_DIE<MEMORY_ADDR> *FindSubProgramDIEByAddrRange(MEMORY_ADDR addr, MEMORY_ADDR length) const;
	const DWARF_DIE<MEMORY_ADDR> *FindSubProgramDIE(MEMORY_ADDR pc) const;
	const DWARF_DIE<MEMORY_ADDR> *FindDataObjectDIE(const char *name, MEMORY_ADDR pc) const;
	unisim::util::debug::DataObject<MEMORY_ADDR> *FindDataObject(const DWARF_MachineState<MEMORY_ADDR> *dw_mach_state, unsigned int prc_num, const CLocOperationStream *c_loc_operation_stream) const;
	struct FindDataObjectArguments
	{
		// inputs
		CLocOperationStream c_loc_operation_stream; // Input/Output
		
		// intermediate result
		const DWARF_DIE<MEMORY_ADDR> *dw_die_type; // Input/Output
		bool match_or_optimized_out;               // Output
		unsigned int dim;                          // Input/Output
		
		// outputs
		std::string matched_data_object_name;
		DWARF_Location<MEMORY_ADDR> *dw_data_object_loc; // Input/Output
		const unisim::util::debug::Type *dw_data_object_type;
		
		FindDataObjectArguments() : c_loc_operation_stream(), dw_die_type(0), match_or_optimized_out(false), dim(0), matched_data_object_name(), dw_data_object_loc(0), dw_data_object_type(0) {}
	};
	bool FindDataObject(const DWARF_MachineState<MEMORY_ADDR> *dw_mach_state, unsigned int prc_num, FindDataObjectArguments& args) const;
	bool FindDataObjectProlog(const DWARF_MachineState<MEMORY_ADDR> *dw_mach_state, unsigned int prc_num, FindDataObjectArguments& args) const;
	bool FindDataObjectEpilog(const DWARF_MachineState<MEMORY_ADDR> *dw_mach_state, unsigned int prc_num, FindDataObjectArguments& args) const;
	const unisim::util::debug::Statement<MEMORY_ADDR> *FindStatements(unisim::service::interfaces::StatementScanner<MEMORY_ADDR> *scanner, MEMORY_ADDR addr, typename unisim::service::interfaces::StatementLookup<MEMORY_ADDR>::Scope scope) const;
	const unisim::util::debug::Statement<MEMORY_ADDR> *FindStatements(unisim::service::interfaces::StatementScanner<MEMORY_ADDR> *scanner, const unisim::util::debug::SourceCodeLocation& source_code_location) const;
	
	bool GetFrameBase(const DWARF_Frame<MEMORY_ADDR> *dw_curr_frame, MEMORY_ADDR& frame_base) const;
	
	// Architecture meta info (from blob)
	endian_type GetFileEndianness() const;
	endian_type GetArchEndianness() const;
	uint8_t GetFileAddressSize() const;
	uint8_t GetArchAddressSize() const;
	bool HasFilename() const;
	const char *GetFilename() const;
	const char *GetArchitecture() const;
	
	// Various helper methods
	bool GetCallingConvention(MEMORY_ADDR pc, uint8_t& calling_convention) const;
	unsigned int GetReturnAddressSize(MEMORY_ADDR pc) const;
	bool ComputeCFA(const DWARF_Frame<MEMORY_ADDR> *dw_frame, MEMORY_ADDR& cfa) const;
	const DWARF_FDE<MEMORY_ADDR> *FindFDEByAddr(MEMORY_ADDR pc) const;
	const DWARF_CFI<MEMORY_ADDR> *FindCFIByAddr(MEMORY_ADDR pc) const;
	DW_CFA_Specification GetCFA_Specification() const;
	DW_CFA_RegRuleOffsetSpecification GetCFA_RegRuleOffsetSpecification() const;
	
	// Symbol table
	const unisim::util::debug::SymbolTable<MEMORY_ADDR> *GetSymbolTable() const;
	
	// Blob
	const unisim::util::blob::Blob<MEMORY_ADDR> *GetBlob() const;

	// Statement matrix methods
	void DumpStatementMatrix();
	void BuildStatementMatrix();
	
	// Various helper methods
	bool IsAbsolutePath(const char *filename) const;
	
	// Architectural state methods per processor
	unisim::service::interfaces::Memory<MEMORY_ADDR> *GetMemoryInterface(unsigned int prc_num) const;

	// Debug stuf
	std::ostream& GetGeneralDebugInfoStream() const;
	std::ostream& GetGeneralDebugWarningStream() const;
	std::ostream& GetGeneralDebugErrorStream() const;
	std::ostream& GetDebugInfoStream() const;
	std::ostream& GetDebugWarningStream() const;
	std::ostream& GetDebugErrorStream() const;
};

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif
