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

#ifndef __UNISIM_SERVICE_LOADER_ELF_LOADER_DWARF_H__
#define __UNISIM_SERVICE_LOADER_ELF_LOADER_DWARF_H__

#include <vector>
#include <unisim/util/endian/endian.hh>
#include <unisim/util/debug/stmt.hh>
#include <iosfwd>
#include <string>
#include <inttypes.h>
#include <map>

namespace unisim {
namespace service {
namespace loader {
namespace elf_loader {

using unisim::util::endian::endian_type;
using unisim::util::debug::Statement;

class DWARF_Filename;
template <class MEMORY_ADDR> class DWARF_StatementProgram;
template <class MEMORY_ADDR> class DWARF_StatementVM;

class DWARF_LEB128
{
public:
	DWARF_LEB128();
	DWARF_LEB128(const DWARF_LEB128& leb128);
	~DWARF_LEB128();
	
	unsigned int GetByteSize() const;
	unsigned int GetBitLength() const;
	template <typename T> bool Fit(const T *t = 0) const;
	template <typename T> operator T() const;
	DWARF_LEB128& operator = (const uint8_t *leb128);
	DWARF_LEB128& operator = (const DWARF_LEB128& leb128);
	
	int Load(const uint8_t *rawdata, unsigned int max_size);
	
	std::string to_hex(bool is_signed) const;
	std::string to_string(bool is_signed) const;
private:
	uint8_t *leb128;
};

std::ostream& operator << (std::ostream& os, const DWARF_Filename& dw_filename);

class DWARF_Filename
{
public:
	DWARF_Filename();
	DWARF_Filename(const DWARF_Filename& dw_filename);
	~DWARF_Filename();
	const std::string *GetFilename() const;
	const DWARF_LEB128& GetDirectoryIndex() const;
	const DWARF_LEB128& GetLastModificationTime() const;
	const DWARF_LEB128& GetByteLength() const;
	
	DWARF_Filename& operator = (const DWARF_Filename& dw_filename);
	int Load(const uint8_t *rawdata, unsigned int max_size);
	
	friend std::ostream& operator << (std::ostream& os, const DWARF_Filename& dw_filename);
private:
	std::string filename;
	DWARF_LEB128 directory_index;         // unsigned, directory index of the directory in which file was found
	DWARF_LEB128 last_modification_time;  // unsigned, time of last modification for the file
	DWARF_LEB128 byte_length;             // unsigned, length in bytes of the file
};

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_StatementProgram<MEMORY_ADDR>& dw_stmt_prog);

const uint8_t DW_LNS_copy = 1;
const uint8_t DW_LNS_advance_pc = 2;
const uint8_t DW_LNS_advance_line = 3;
const uint8_t DW_LNS_set_file = 4;
const uint8_t DW_LNS_set_column = 5;
const uint8_t DW_LNS_negate_stmt = 6;
const uint8_t DW_LNS_set_basic_block = 7;
const uint8_t DW_LNS_const_add_pc = 8;
const uint8_t DW_LNS_fixed_advance_pc = 9;

const uint8_t DW_LNE_end_sequence = 1;
const uint8_t DW_LNE_set_address = 2;
const uint8_t DW_LNE_define_file = 3;

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_StatementProgram<MEMORY_ADDR>& dw_stmt_prog);

template <class MEMORY_ADDR>
class DWARF_StatementProgram
{
public:
	DWARF_StatementProgram(endian_type _endianness);
	~DWARF_StatementProgram();
	int Load(const uint8_t *rawdata, unsigned int max_size);
	friend std::ostream& operator << <MEMORY_ADDR>(std::ostream& os, const DWARF_StatementProgram<MEMORY_ADDR>& dw_stmt_prog);
private:
	friend class DWARF_StatementVM<MEMORY_ADDR>;
	
	endian_type endianness;
	
	uint32_t total_length;                // The size in bytes of the statement information for this compilation unit
	                                      // (not including the total_length field itself).

	uint16_t version;                     // Version identifier for the statement information format.
	
	uint32_t prologue_length;             // The number of bytes following the prologue_length field to the beginning of the first
	                                      // byte of the statement program itself.

	uint8_t minimum_instruction_length;   // The size in bytes of the smallest target machine instruction. Statement program opcodes
	                                      // that alter the address register first multiply their operands by this value.

	uint8_t default_is_stmt;              // The initial value of the is_stmt register.
	                                      // A simple code generator that emits machine instructions in the order implied by the source
	                                      // program would set this to "true", and every entry in the matrix would represent a
	                                      // statement boundary. A pipeline scheduling code generator would set this to "false" and
	                                      // emit a specific statement program opcode for each instruction that represented a statement
	                                      // boundary.

	int8_t line_base;                     // This parameter affects the meaning of the special opcodes. See below.

	uint8_t line_range;                   // This parameter affects the meaning of the special opcodes. See below.
	
	uint8_t opcode_base;                  // The number assigned to the first special opcode.

	std::vector<uint8_t> standard_opcode_lengths;   // This array specifies the number of LEB128 operands for each of the standard opcodes. The
	                                                // first element of the array corresponds to the opcode whose value is 1, and the last element
	                                                // corresponds to the opcode whose value is opcode_base - 1. By increasing
	                                                // opcode_base, and adding elements to this array, new standard opcodes can be added,
	                                                // while allowing consumers who do not know about these new opcodes to be able to skip
	                                                // them.
	
	std::vector<std::string> include_directories; // The sequence contains an entry for each path that was searched for included source files in
	                                              // this compilation. (The paths include those directories specified explicitly by the user for
	                                              // the compiler to search and those the compiler searches without explicit direction). Each
	                                              // path entry is either a full path name or is relative to the current directory of the compilation.
	                                              // The current directory of the compilation is understood to be the first entry and is not
	                                              // explicitly represented. Each entry is a null-terminated string containing a full path name.
	                                              // The last entry is followed by a single null byte.

	std::vector<DWARF_Filename> filenames;        // The sequence contains an entry for each source file that contributed to the statement
	                                              // information for this compilation unit or is used in other contexts, such as in a declaration
	                                              // coordinate or a macro file inclusion. Each entry has a null-terminated string containing the
	                                              // file name, an unsigned LEB128 number representing the directory index of the directory in
	                                              // which the file was found, an unsigned LEB128 number representing the time of last
	                                              // modification for the file and an unsigned LEB128 number representing the length in bytes
	                                              // of the file. A compiler may choose to emit LEB128(0) for the time and length fields to
	                                              // indicate that this information is not available. The last entry is followed by a single null
	                                              // byte.

	uint32_t program_length;
	uint8_t *program;                             // the program itself
};

template <class MEMORY_ADDR>
class DWARF_StatementVM
{
public:
	DWARF_StatementVM(endian_type _endianness);
	~DWARF_StatementVM();

	bool Run(const DWARF_StatementProgram<MEMORY_ADDR> *dw_stmt_prog, std::ostream *os, std::map<MEMORY_ADDR, Statement<MEMORY_ADDR> *> *matrix);
private:
	endian_type endianness;
	
	// Machine state
	MEMORY_ADDR address;   // The program-counter value corresponding to a machine instruction generated by the compiler.
	unsigned int file;     // An unsigned integer indicating the identity of the source file corresponding to a machine instruction.
	unsigned int line;     // An unsigned integer indicating a source line number. Lines are numbered beginning at 1. The compiler may emit the value 0 in cases where an instruction cannot be attributed to any source line.
	unsigned int column;   // An unsigned integer indicating a column number within a source line. Columns are numbered beginning at 1. The value 0 is reserved to indicate that a statement begins at the ‘‘left edge’’ of the line.
	bool is_stmt;          // A boolean indicating that the current instruction is the beginning of a statement.
	bool basic_block;      // A boolean indicating that the current instruction is the beginning of a basic block.
	bool end_sequence;     // A boolean indicating that the current address is that of the first byte after the end of a sequence of target machine instructions.
	std::vector<DWARF_Filename> filenames; // filenames (filenames in statement program prologue + those added by extended opcode DW_LNE_define_file)

	void AddRow(const DWARF_StatementProgram<MEMORY_ADDR> *dw_stmt_prog, std::map<MEMORY_ADDR, Statement<MEMORY_ADDR> *>& matrix);
};

} // end of namespace elf_loader
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim

#endif
