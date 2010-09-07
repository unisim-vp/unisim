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

#ifndef __UNISIM_SERVICE_LOADER_ELF_LOADER_DWARF_TCC__
#define __UNISIM_SERVICE_LOADER_ELF_LOADER_DWARF_TCC__

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <stack>
#include <unisim/util/arithmetic/arithmetic.hh>
#include <unisim/util/debug/register.hh>

// TODO: verify on each host platform what is needed for mkdir. Below is for linux.
#include <sys/stat.h>
#include <sys/types.h>


namespace unisim {
namespace service {
namespace loader {
namespace elf_loader {

using unisim::util::arithmetic::SignExtend;

template <class MEMORY_ADDR>
DWARF_StatementProgram<MEMORY_ADDR>::DWARF_StatementProgram(DWARF_Handler<MEMORY_ADDR> *_dw_handler)
	: dw_handler(_dw_handler)
	, unit_length(0)
	, version(0)
	, header_length(0)
	, minimum_instruction_length(0)
	, default_is_stmt(0)
	, line_base(0)
	, line_range(0)
	, opcode_base(0)
	, program(0)
{
}

template <class MEMORY_ADDR>
DWARF_StatementProgram<MEMORY_ADDR>::~DWARF_StatementProgram()
{
}

template <class MEMORY_ADDR>
endian_type DWARF_StatementProgram<MEMORY_ADDR>::GetEndianness() const
{
	return dw_handler->GetEndianness();
}

template <class MEMORY_ADDR>
int64_t DWARF_StatementProgram<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size, uint64_t _offset)
{
	offset = _offset;
	endian_type endianness = dw_handler->GetEndianness();
	standard_opcode_lengths.clear();
	include_directories.clear();
	filenames.clear();
	
	uint32_t unit_length32;
	
	uint64_t size = 0;

	if(max_size < sizeof(unit_length32)) return -1;
	memcpy(&unit_length32, rawdata, sizeof(unit_length32));
	unit_length32 = Target2Host(endianness, unit_length32);
	rawdata += sizeof(unit_length32);
	max_size -= sizeof(unit_length32);
	size += sizeof(unit_length32);
	
	DWARF_Format dw_fmt = (unit_length32 == 0xffffffffUL) ? FMT_DWARF64 : FMT_DWARF32;

	// 64-bit DWARF format ?
	if(dw_fmt == FMT_DWARF64)
	{
		uint64_t unit_length64;
		if(max_size < sizeof(unit_length64)) return -1;
		memcpy(&unit_length64, rawdata, sizeof(unit_length64));
		unit_length64 = Target2Host(endianness, unit_length64);
		rawdata += sizeof(unit_length64);
		max_size -= sizeof(unit_length64);
		size += sizeof(unit_length64);
		unit_length = unit_length64;
	}
	else
	{
		unit_length = unit_length32;
	}
	
	if(max_size < unit_length) return -1;

	if(max_size < sizeof(version)) return -1;
	memcpy(&version, rawdata, sizeof(version));
	version = Target2Host(endianness, version);
	rawdata += sizeof(version);
	max_size -= sizeof(version);
	size += sizeof(version);

	// 64-bit DWARF format ?
	if(dw_fmt == FMT_DWARF64)
	{
		uint64_t header_length64;
		
		if(max_size < sizeof(header_length64)) return -1;
		memcpy(&header_length64, rawdata, sizeof(header_length64));
		header_length64 = Target2Host(endianness, header_length64);
		rawdata += sizeof(header_length64);
		max_size -= sizeof(header_length64);
		size += sizeof(header_length64);
		header_length = header_length64;
	}
	else
	{
		uint32_t header_length32;
		
		if(max_size < sizeof(header_length32)) return -1;
		memcpy(&header_length32, rawdata, sizeof(header_length32));
		header_length32 = Target2Host(endianness, header_length32);
		rawdata += sizeof(header_length32);
		max_size -= sizeof(header_length32);
		size += sizeof(header_length32);
		header_length = header_length32;
	}


	if((version != 2) && (version != 3)) return -1;

	if(dw_fmt == FMT_DWARF64)
	{
		if((header_length > (unit_length - sizeof(version) - sizeof(uint32_t) - sizeof(uint64_t))) || ((unit_length - sizeof(version) - sizeof(uint32_t) - sizeof(uint64_t)) > max_size)) return -1;
	}
	else
	{
		if((header_length > (unit_length - sizeof(version) - sizeof(uint32_t))) || ((unit_length - sizeof(version) - sizeof(uint32_t)) > max_size)) return -1;
	}

	if(max_size < sizeof(minimum_instruction_length)) return -1;
	memcpy(&minimum_instruction_length, rawdata, sizeof(minimum_instruction_length));
	minimum_instruction_length = Target2Host(endianness, minimum_instruction_length);
	rawdata += sizeof(minimum_instruction_length);
	max_size -= sizeof(minimum_instruction_length);
	size += sizeof(minimum_instruction_length);

	if(max_size < sizeof(default_is_stmt)) return -1;
	memcpy(&default_is_stmt, rawdata, sizeof(default_is_stmt));
	default_is_stmt = Target2Host(endianness, default_is_stmt);
	rawdata += sizeof(default_is_stmt);
	max_size -= sizeof(default_is_stmt);
	size += sizeof(default_is_stmt);

	if(max_size < sizeof(line_base)) return -1;
	memcpy(&line_base, rawdata, sizeof(line_base));
	line_base = Target2Host(endianness, line_base);
	rawdata += sizeof(line_base);
	max_size -= sizeof(line_base);
	size += sizeof(line_base);

	if(max_size < sizeof(line_range)) return -1;
	memcpy(&line_range, rawdata, sizeof(line_range));
	line_range = Target2Host(endianness, line_range);
	rawdata += sizeof(line_range);
	max_size -= sizeof(line_range);
	size += sizeof(line_range);
	
	if(max_size < sizeof(opcode_base)) return -1;
	memcpy(&opcode_base, rawdata, sizeof(opcode_base));
	opcode_base = Target2Host(endianness, opcode_base);
	rawdata += sizeof(opcode_base);
	max_size -= sizeof(opcode_base);
	size += sizeof(opcode_base);

	unsigned int i;
	for(i = 1; i < opcode_base; i++)
	{
		if(!max_size) return -1;
		standard_opcode_lengths.push_back(*rawdata);
		
		rawdata++;
		size++;
		max_size--;
	}

	do
	{
		if(!max_size) return -1;
		const char *directory_name = (const char *) rawdata;
		int64_t sz = strlen(directory_name) + 1;
		size += sz;
		rawdata += sz;
		max_size -= sz;
		
		if(*directory_name == 0) break; // empty directory name
		
		include_directories.push_back(directory_name);
	} while(1);

	unsigned int filename_idx = 0;
	do
	{
		if(!max_size) return -1;
		if(!(*rawdata)) break;
		
		DWARF_Filename filename(filename_idx);
			
		int64_t sz;
		if((sz = filename.Load(rawdata, max_size)) < 0) return -1;
		size += sz;
		rawdata += sz;
		max_size -= sz;
		
		filenames.push_back(filename);
		filename_idx++;
	} while(1);

	if(dw_fmt == FMT_DWARF64)
	{
		if(size > header_length + sizeof(uint32_t) + sizeof(uint64_t) + sizeof(version) + sizeof(uint64_t)) return -1;
		rawdata += header_length + sizeof(uint32_t) + sizeof(uint64_t) + sizeof(version) + sizeof(uint64_t) - size;
		max_size -= header_length + sizeof(uint32_t) + sizeof(uint64_t) + sizeof(version) + sizeof(uint64_t) - size;
		size = header_length + sizeof(uint32_t) + sizeof(uint64_t) + sizeof(version) + sizeof(uint64_t);
		program_length = unit_length + sizeof(uint32_t) + sizeof(uint64_t) - size;
	}
	else
	{
		if(size > header_length + sizeof(uint32_t) + sizeof(version) + sizeof(uint32_t)) return -1;
		rawdata += header_length + sizeof(uint32_t) + sizeof(version) + sizeof(uint32_t) - size;
		max_size -= header_length + sizeof(uint32_t) + sizeof(version) + sizeof(uint32_t) - size;
		size = header_length + sizeof(uint32_t) + sizeof(version) + sizeof(uint32_t);
		program_length = unit_length + sizeof(uint32_t) - size;
	}
	
	if(program_length > max_size) return -1;
	program = rawdata;
	size += program_length;

	return size;
}

template <class MEMORY_ADDR>
void DWARF_StatementProgram<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int _id)
{
	id = _id;
}

template <class MEMORY_ADDR>
std::string DWARF_StatementProgram<MEMORY_ADDR>::GetHREF() const
{
	std::stringstream sstr;
	sstr << "debug_line/" << (id / debug_line_per_file) << ".html#stmt-prog-" << id;
	return sstr.str();
}

template <class MEMORY_ADDR>
unsigned int DWARF_StatementProgram<MEMORY_ADDR>::GetId() const
{
	return id;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_StatementProgram<MEMORY_ADDR>::to_XML(std::ostream& os) const
{
	unsigned int i;
	
	os << "<DW_STMT_PROG unit_length=\"" << unit_length << "\" version=\"" << version << "\" header_length=\"" << header_length << "\"";
	os << " minimum_instruction_length=\"" << (uint32_t) minimum_instruction_length << "\" default_is_stmt=\"" << (uint32_t) default_is_stmt << "\"";
	os << " line_base=\"" << (int32_t) line_base << "\" line_range=\"" << (uint32_t) line_range << "\" opcode_base=\"" << (uint32_t) opcode_base << "\"";
	os << " standard_opcode_lengths=\"";
	unsigned int num_standard_opcode_lengths = standard_opcode_lengths.size();
	for(i = 0; i < num_standard_opcode_lengths; i++)
	{
		if(i != 0) os << " ";
		os << (uint32_t) standard_opcode_lengths[i];
	}
	os << "\">" << std::endl;
	os << "<DW_INCLUDE_DIRECTORIES>" << std::endl;
	unsigned int num_include_directories = include_directories.size();
	for(i = 0; i < num_include_directories; i++)
	{
		os << "<DW_INLCUDE_DIRECTORY name=\"";
		c_string_to_XML(os, include_directories[i]);
		os << "\"/>" << std::endl;
	}
	os << "</DW_INCLUDE_DIRECTORIES>" << std::endl;
	os << "<DW_FILENAMES>" << std::endl;
	unsigned int num_filenames = filenames.size();
	for(i = 0; i < num_filenames; i++)
	{
		filenames[i].to_XML(os) << std::endl;
	}
	os << "</FILENAMES>" << std::endl;
	os << "<DW_STMT_PROG_OPCODES>" << std::endl;
	std::stringstream sstr;
	DWARF_StatementVM<MEMORY_ADDR> dw_stmt_vm = DWARF_StatementVM<MEMORY_ADDR>();
	dw_stmt_vm.Run(this, &sstr, 0);
	c_string_to_XML(os, sstr.str().c_str());
	os << "</DW_STMT_PROG_OPCODES>" << std::endl;
	os << "</DW_STMT_PROG>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_StatementProgram<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	unsigned int i;

	os << "<tr id=\"stmt-prog-" << id << "\"><td>" << std::endl;
	os << "<ul>" << std::endl;
	os << "<li>Id: " << id << "</li>" << std::endl;
	os << "<li>Offset: " << offset << "</li>" << std::endl;
	os << "<li>Version: " << version << "</li>" << std::endl;
	os << "<li>Minimum Instruction Length: " << (uint32_t) minimum_instruction_length << "</li>" << std::endl;
	os << "<li>Initial value of 'is_stmt': " << (uint32_t) default_is_stmt << "</li>" << std::endl;
	os << "<li>Line Base: " << (int32_t) line_base << "</li>" << std::endl;
	os << "<li>Line Range: " << (uint32_t) line_range << "</li>" << std::endl;
	os << "<li>Opcode Base: " << (uint32_t) opcode_base << "</li>" << std::endl;
	os << "<li>Opcodes:" << std::endl;
	os << "<ul>" << std::endl;
	unsigned int num_standard_opcode_lengths = standard_opcode_lengths.size();
	for(i = 0; i < num_standard_opcode_lengths; i++)
	{
		os << "<li>Opcode " << (i + 1) << " has " << (uint32_t) standard_opcode_lengths[i] << " args" << "</li>" << std::endl;
	}
	os << "</ul>" << std::endl;
	os << "</ul>" << std::endl;
	
	os << "Directory Table:<br><table><tr><th>Id</th><th>Directory name</th></tr>" << std::endl;
	unsigned int num_include_directories = include_directories.size();
	for(i = 0; i < num_include_directories; i++)
	{
		os << "<tr><td>" << i << "</td><td>";
		c_string_to_HTML(os, include_directories[i]);
		os << "</td></tr>" << std::endl;
	}
	os << "</table>" << std::endl;
	os << "<br>" << std::endl;
	os << "Filenames Table:<br><table><tr><th>Id</th><th>Filename</th><th>Directory index</th><th>Last modification time</th><th>Byte size</th></tr>" << std::endl;
	unsigned int num_filenames = filenames.size();
	for(i = 0; i < num_filenames; i++)
	{
		filenames[i].to_HTML(os) << std::endl;
	}
	os << "</table>" << std::endl;
	os << "<br>" << std::endl;
	os << "Program (" << program_length << " bytes):<br>" << std::endl;
	
	std::stringstream sstr;
	DWARF_StatementVM<MEMORY_ADDR> dw_stmt_vm = DWARF_StatementVM<MEMORY_ADDR>();
	dw_stmt_vm.Run(this, &sstr, 0);
	c_string_to_HTML(os, sstr.str().c_str());
	os << "</td>" << std::endl;
	os << "</tr>" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_StatementProgram<MEMORY_ADDR>& dw_stmt_prog)
{
	unsigned int i;

	os << "Statement program:" << std::endl
	   << " - Total Length: " << dw_stmt_prog.unit_length << std::endl
	   << " - DWARF Version: " << dw_stmt_prog.version << std::endl
	   << " - Prologue Length: " << dw_stmt_prog.header_length << std::endl
	   << " - Minimum Instruction Length: " << (uint32_t) dw_stmt_prog.minimum_instruction_length << std::endl
	   << " - Initial value of 'is_stmt': " << (uint32_t) dw_stmt_prog.default_is_stmt << std::endl
	   << " - Line Base: " << (int32_t) dw_stmt_prog.line_base << std::endl
	   << " - Line Range: " << (uint32_t) dw_stmt_prog.line_range << std::endl
	   << " - Opcode Base: " << (uint32_t) dw_stmt_prog.opcode_base << std::endl;
	
	os << " - Opcodes:" << std::endl;
	unsigned int num_standard_opcode_lengths = dw_stmt_prog.standard_opcode_lengths.size();
	for(i = 0; i < num_standard_opcode_lengths; i++)
	{
		os << "   - Opcode " << (i + 1) << " has " << (uint32_t) dw_stmt_prog.standard_opcode_lengths[i] << " args" << std::endl;
	}
	
	os << " - Directory Table:" << std::endl;
	unsigned int num_include_directories = dw_stmt_prog.include_directories.size();
	for(i = 0; i < num_include_directories; i++)
	{
		os << "   - Entry #" << (i + 1) << ": \"" << dw_stmt_prog.include_directories[i] << "\"" << std::endl;
	}
	os << " - Filenames Table:" << std::endl;
	unsigned int num_filenames = dw_stmt_prog.filenames.size();
	for(i = 0; i < num_filenames; i++)
	{
		os << "   - Entry #" << (i + 1) << ": " << dw_stmt_prog.filenames[i] << std::endl;
	}
	os << " - Program (" << dw_stmt_prog.program_length << " bytes):" << std::endl;
	
	DWARF_StatementVM<MEMORY_ADDR> dw_stmt_vm = DWARF_StatementVM<MEMORY_ADDR>();
	
	dw_stmt_vm.Run(&dw_stmt_prog, &os, 0);
	return os;
}

template <class MEMORY_ADDR>
uint64_t DWARF_StatementProgram<MEMORY_ADDR>::GetOffset() const
{
	return offset;
}

template <class MEMORY_ADDR>
DWARF_StatementVM<MEMORY_ADDR>::DWARF_StatementVM()
	: address(0)
	, file(1)
	, line(1)
	, column(0)
	, is_stmt(false)
	, basic_block(false)
	, end_sequence(false)
	, prologue_end(false)
	, prologue_begin(false)
	, isa(0)
	, filenames()
{
}

template <class MEMORY_ADDR>
DWARF_StatementVM<MEMORY_ADDR>::~DWARF_StatementVM()
{
}

template <class MEMORY_ADDR>
bool DWARF_StatementVM<MEMORY_ADDR>::IsAbsolutePath(const char *filename) const
{
	// filename starts '/' or 'drive letter':\ or 'driver letter':/
	return (((filename[0] >= 'a' && filename[0] <= 'z') || (filename[0] >= 'A' && filename[0] <= 'Z')) && (filename[1] == ':') && ((filename[2] == '\\') || (filename[2] == '/'))) || (*filename == '/');
}

template <class MEMORY_ADDR>
void DWARF_StatementVM<MEMORY_ADDR>::AddRow(const DWARF_StatementProgram<MEMORY_ADDR> *dw_stmt_prog, std::map<MEMORY_ADDR, Statement<MEMORY_ADDR> *>& matrix)
{
	const DWARF_Filename *dw_filename = (file >= 1 && file <= filenames.size()) ? &filenames[file - 1] : 0;
	const char *filename = dw_filename ? dw_filename->GetFilename() : 0;
	const char *dirname = 0;
	if(filename)
	{
		if(!IsAbsolutePath(filename))
		{
			const DWARF_LEB128& leb128_dir = dw_filename->GetDirectoryIndex();
			unsigned int dir = (unsigned int) leb128_dir;
			dirname = (dir >= 1 && dir <= dw_stmt_prog->include_directories.size()) ? dw_stmt_prog->include_directories[dir - 1] : 0;
		}
	}
	if(matrix.find(address) != matrix.end())
	{
		//std::cerr << "Row for address 0x" << std::hex << address << std::dec << " already exists" << std::endl;
		return;
	}
	
	Statement<MEMORY_ADDR> *stmt = new Statement<MEMORY_ADDR>(address, basic_block, dirname, filename, line, column);
	matrix.insert(std::pair<MEMORY_ADDR, Statement<MEMORY_ADDR> *>(address, stmt));
}

template <class MEMORY_ADDR>
bool DWARF_StatementVM<MEMORY_ADDR>::Run(const DWARF_StatementProgram<MEMORY_ADDR> *dw_stmt_prog, std::ostream *os, std::map<MEMORY_ADDR, Statement<MEMORY_ADDR> *> *matrix)
{
	// Initialize machine state
	address = 0;
	file = 1;
	line = 1;
	column = 0;
	is_stmt = dw_stmt_prog->default_is_stmt;
	basic_block = false;
	end_sequence = false;
	prologue_end = false;
	prologue_begin = false;
	isa = 0;
	filenames = dw_stmt_prog->filenames;

	// Run the program
	endian_type endianness = dw_stmt_prog->GetEndianness();
	const uint8_t *prg = dw_stmt_prog->program;
	unsigned int count = dw_stmt_prog->program_length;
	uint8_t opcode;

	if(count > 0)
	{
		do
		{
			opcode = *prg;
			prg++;
			count--;
			if(opcode)
			{
				if(opcode < dw_stmt_prog->opcode_base)
				{
					unsigned int num_args = dw_stmt_prog->standard_opcode_lengths[opcode - 1];
					unsigned int i;
					DWARF_LEB128 args[num_args];
					uint16_t uhalf_arg;
					
					if(opcode != DW_LNS_fixed_advance_pc)
					{
						for(i = 0; i < num_args; i++)
						{
							int64_t sz;
							if((sz = args[i].Load(prg, count)) < 0)
							{
								std::cerr << "Bad LEB128" << std::endl;
								return false;
							}
							prg += sz;
							count -= sz;
						}
					}
					else
					{
						if(count < sizeof(uhalf_arg))
						{
							std::cerr << "Bad uhalf operand" << std::endl;
							return false;
						}
						memcpy(&uhalf_arg, prg, sizeof(uhalf_arg));
						uhalf_arg = Target2Host(endianness, uhalf_arg);
						prg += sizeof(uhalf_arg);
						count -= sizeof(uhalf_arg);
					}
					
					// Standard opcode
					if(os) (*os) << "   - Standard opcode " << (unsigned int) opcode << ": ";
					switch(opcode)
					{
						case DW_LNS_copy:
							// Copy
							if(os) (*os) << "Copy" << std::endl;
							if(matrix) AddRow(dw_stmt_prog, *matrix);
							break;
						case DW_LNS_advance_pc:
							{
								unsigned int pc_increment = (unsigned int) args[0] * dw_stmt_prog->minimum_instruction_length;
								// Advance PC by 'pc_increment'
								address += pc_increment;
								if(os) (*os) << "Advance PC by " << pc_increment << " to 0x" << std::hex << address << std::dec << std::endl;
							}
							break;
						case DW_LNS_advance_line:
							{
								const DWARF_LEB128& line_increment = args[0];

								// Advance Line by 'line_increment'
								line += (int) line_increment;
								if(os) (*os) << "Advance Line by " << line_increment.to_string(true) << " to " << line << std::endl;
							}
							break;
						case DW_LNS_set_file:
							{
								const DWARF_LEB128& file_entry = args[0];
								
								// Set File to entry # 'file_entry' in Filenames Table
								file = (unsigned int) file_entry;
								if(os) (*os) << "Set File to entry #" << file << " in Filenames Table" << std::endl;
							}
							break;
						case DW_LNS_set_column:
							{
								const DWARF_LEB128& col = args[0];
								
								// Set Column to 'column'
								column = (unsigned int) col;
								if(os) (*os) << "Set Column to " << column << std::endl;
							}
							break;
						case DW_LNS_negate_stmt:
							// Negate is_stmt
							is_stmt = !is_stmt;
							if(os) (*os) << "Negate is_stmt" << std::endl;
							break;
						case DW_LNS_set_basic_block:
							// Set basic block
							basic_block = true;
							if(os) (*os) << "Set basic block" << std::endl;
							break;
						case DW_LNS_const_add_pc:
							{
								int address_increment = ((255 - dw_stmt_prog->opcode_base) / dw_stmt_prog->line_range) * dw_stmt_prog->minimum_instruction_length;
								// Advance PC by constant 'address_increment'
								address += address_increment;
								if(os) (*os) << "Advance PC by constant " << address_increment << " to 0x" << std::hex << address << std::dec << std::endl;
							}
							break;
						case DW_LNS_fixed_advance_pc:
							// Advance PC by fixed 'uhalf_arg'
							address += uhalf_arg;
							if(os) (*os) << "Advance PC by fixed " << uhalf_arg << " to 0x" << std::hex << address << std::dec << std::endl;
							break;
						case DW_LNS_set_prologue_end:
							prologue_end = true;
							if(os) (*os) << "DW_LNS_set_prologue_end" << std::endl;
							break;
						case DW_LNS_set_epilogue_begin:
							prologue_end = false;
							if(os) (*os) << "DW_LNS_set_epilogue_begin" << std::endl;
							break;
						case DW_LNS_set_isa:
							{
								const DWARF_LEB128& isa_val = args[0];
								
								// Set isa to 'isa_val'
								isa = (unsigned int) isa_val;
								if(os) (*os) << "Set isa to " << isa << std::endl;
							}
							break;
						default:
							// Skipped unknown
							if(os) (*os) << "Skipped unknown" << std::endl;
							break;
					}
				}
				else
				{
					// Special opcode
					uint8_t adjusted_opcode = opcode - dw_stmt_prog->opcode_base;
					int line_increment = dw_stmt_prog->line_base + (adjusted_opcode % dw_stmt_prog->line_range);
					unsigned int address_increment = (adjusted_opcode / dw_stmt_prog->line_range) * dw_stmt_prog->minimum_instruction_length;
					// Special opcode 'adjusted_opcode': advance Address by 'address_increment' and Line by 'line_increment'
					address += address_increment;
					line += line_increment;
					// Add a row to matrix
					if(matrix) AddRow(dw_stmt_prog, *matrix);
					// Reset basic_block
					basic_block = false;
					// Reset prologue_end
					prologue_end = false;
					// Reset prologue_begin
					prologue_begin = false;
					if(os) (*os) << "   - Special opcode " << (unsigned int) adjusted_opcode << ": advance Address by " << address_increment << " to 0x" << std::hex << address << std::dec << " and Line by " << line_increment << " to " << line << std::endl;
				}
			}
			else
 			{
				// Extended opcode
				DWARF_LEB128 leb128_insn_length;
				int64_t sz;
				if((sz = leb128_insn_length.Load(prg, count)) < 0)
				{
					std::cerr << "Bad LEB128" << std::endl;
					return false;
				}
				prg += sz;
				count -= sz;
				uint64_t insn_length = (uint64_t) leb128_insn_length;
				opcode = *prg;
				prg++;
				count--;
				// Extended opcode 'opcode'
				if(os) (*os) << "   - Extended opcode " << (unsigned int) opcode << ": ";
				
				switch(opcode)
				{
					case DW_LNE_end_sequence:
						// End of Sequence
						end_sequence = true;
						// Add a row to matrix
						if(matrix) AddRow(dw_stmt_prog, *matrix);
						// Reset machine state
						address = 0;
						file = 1;
						line = 1;
						column = 0;
						is_stmt = dw_stmt_prog->default_is_stmt;
						basic_block = false;
						end_sequence = false;
						if(os) (*os) << "End of Sequence";
						break;
					case DW_LNE_set_address:
						{
							if(os) (*os) << "Set Address to ";
							uint64_t addr_length = insn_length - 1;
							switch(addr_length)
							{
								case sizeof(uint32_t):
									{
										uint32_t addr;
										memcpy(&addr, prg, sizeof(uint32_t));
										addr = Target2Host(endianness, addr);
										// Set Address to 'addr'
										address = addr;
									}
									break;
								case sizeof(uint64_t):
									{
										uint64_t addr;
										memcpy(&addr, prg, sizeof(uint64_t));
										addr = Target2Host(endianness, addr);
										// Set Address to 'addr'
										address = addr;
									}
									break;
								default:
									std::cerr << "Bad address" << std::endl;
									return false;
							}
							if(os) (*os) << std::hex << "0x" << address << std::dec;
						}
						break;
					case DW_LNE_define_file:
						{
							unsigned int filename_idx = filenames.size();
							DWARF_Filename dw_filename(filename_idx);
							int64_t sz;
							if((sz = dw_filename.Load(prg, count)) < 0)
							{
								std::cerr << "Bad DWARF filename entry" << std::endl;
								return false;
							}
							// Define File 'dw_filename'
							filenames.push_back(dw_filename);
							if(os) (*os) << "Define File " << dw_filename << std::endl;
						}
						break;
					default:
						std::cerr << "Unknown extended opcode " << (unsigned int) opcode << std::endl;
						return false;
				}
				prg += insn_length - 1;
				count -= insn_length - 1;
				if(os) (*os) << std::endl;
			}
		}
		while(count && !end_sequence);
	}
	return true;
}

template <class MEMORY_ADDR>
DWARF_AttributeValue<MEMORY_ADDR>::DWARF_AttributeValue(unsigned int _dw_class)
	: dw_class(_dw_class)
{
}

template <class MEMORY_ADDR>
DWARF_AttributeValue<MEMORY_ADDR>::~DWARF_AttributeValue()
{
}

template <class MEMORY_ADDR>
unsigned int DWARF_AttributeValue<MEMORY_ADDR>::DWARF_AttributeValue::GetClass() const
{
	return dw_class;
}

template <class MEMORY_ADDR>
const char *DWARF_AttributeValue<MEMORY_ADDR>::GetClassName() const
{
	return DWARF_GetCLASSName(dw_class);
}

template <class MEMORY_ADDR>
uint64_t DWARF_AttributeValue<MEMORY_ADDR>::to_int() const
{
	return 0;
}

template <class MEMORY_ADDR>
void DWARF_AttributeValue<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler)
{
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_AttributeValue<MEMORY_ADDR>& dw_value)
{
	return os << dw_value.to_string();
}

template <class MEMORY_ADDR>
DWARF_Block<MEMORY_ADDR>::DWARF_Block(uint64_t _length, const uint8_t *_value)
	: DWARF_AttributeValue<MEMORY_ADDR>(DW_CLASS_BLOCK)
	, length(_length)
	, value(_value)
{
}

template <class MEMORY_ADDR>
DWARF_Block<MEMORY_ADDR>::~DWARF_Block()
{
}

template <class MEMORY_ADDR>
uint64_t DWARF_Block<MEMORY_ADDR>::GetLength() const
{
	return length;
}

template <class MEMORY_ADDR>
const uint8_t *DWARF_Block<MEMORY_ADDR>::GetValue() const
{
	return value;
}

template <class MEMORY_ADDR>
std::string DWARF_Block<MEMORY_ADDR>::to_string() const
{
	std::stringstream sstr;
	uint64_t i;
	for(i = 0; i < length; i++)
	{
		if(i != 0) sstr << " ";
		sstr << std::hex << (unsigned int) value[i] << std::dec;
	}
	return std::string(sstr.str());
}

template <class MEMORY_ADDR>
DWARF_UnsignedConstant<MEMORY_ADDR>::DWARF_UnsignedConstant(uint64_t _value)
	: DWARF_AttributeValue<MEMORY_ADDR>(DW_CLASS_UNSIGNED_CONSTANT)
	, value(_value)
{
}

template <class MEMORY_ADDR>
DWARF_UnsignedConstant<MEMORY_ADDR>::~DWARF_UnsignedConstant()
{
}

template <class MEMORY_ADDR>
uint64_t DWARF_UnsignedConstant<MEMORY_ADDR>::GetValue() const
{
	return value;
}

template <class MEMORY_ADDR>
std::string DWARF_UnsignedConstant<MEMORY_ADDR>::to_string() const
{
	std::stringstream sstr;
	sstr << value;
	return std::string(sstr.str());
}

template <class MEMORY_ADDR>
uint64_t DWARF_UnsignedConstant<MEMORY_ADDR>::to_int() const
{
	return value;
}

template <class MEMORY_ADDR>
DWARF_SignedConstant<MEMORY_ADDR>::DWARF_SignedConstant(int64_t _value)
	: DWARF_AttributeValue<MEMORY_ADDR>(DW_CLASS_SIGNED_CONSTANT)
	, value(_value)
{
}

template <class MEMORY_ADDR>
DWARF_SignedConstant<MEMORY_ADDR>::~DWARF_SignedConstant()
{
}

template <class MEMORY_ADDR>
int64_t DWARF_SignedConstant<MEMORY_ADDR>::GetValue() const
{
	return value;
}

template <class MEMORY_ADDR>
std::string DWARF_SignedConstant<MEMORY_ADDR>::to_string() const
{
	std::stringstream sstr;
	sstr << value;
	return std::string(sstr.str());
}

template <class MEMORY_ADDR>
uint64_t DWARF_SignedConstant<MEMORY_ADDR>::to_int() const
{
	return (uint64_t) value;
}

template <class MEMORY_ADDR>
DWARF_UnsignedLEB128Constant<MEMORY_ADDR>::DWARF_UnsignedLEB128Constant(const DWARF_LEB128& leb128)
	: DWARF_AttributeValue<MEMORY_ADDR>(DW_CLASS_UNSIGNED_LEB128_CONSTANT)
	, value(leb128)
{
}

template <class MEMORY_ADDR>
DWARF_UnsignedLEB128Constant<MEMORY_ADDR>::~DWARF_UnsignedLEB128Constant()
{
}

template <class MEMORY_ADDR>
const DWARF_LEB128& DWARF_UnsignedLEB128Constant<MEMORY_ADDR>::GetValue() const
{
	return value;
}

template <class MEMORY_ADDR>
std::string DWARF_UnsignedLEB128Constant<MEMORY_ADDR>::to_string() const
{
	return value.to_string(false);
}

template <class MEMORY_ADDR>
uint64_t DWARF_UnsignedLEB128Constant<MEMORY_ADDR>::to_int() const
{
	return (uint64_t) value;
}

template <class MEMORY_ADDR>
DWARF_SignedLEB128Constant<MEMORY_ADDR>::DWARF_SignedLEB128Constant(const DWARF_LEB128& leb128)
	: DWARF_AttributeValue<MEMORY_ADDR>(DW_CLASS_UNSIGNED_LEB128_CONSTANT)
	, value(leb128)
{
}

template <class MEMORY_ADDR>
DWARF_SignedLEB128Constant<MEMORY_ADDR>::~DWARF_SignedLEB128Constant()
{
}

template <class MEMORY_ADDR>
const DWARF_LEB128& DWARF_SignedLEB128Constant<MEMORY_ADDR>::GetValue() const
{
	return value;
}

template <class MEMORY_ADDR>
std::string DWARF_SignedLEB128Constant<MEMORY_ADDR>::to_string() const
{
	return value.to_string(true);
}

template <class MEMORY_ADDR>
uint64_t DWARF_SignedLEB128Constant<MEMORY_ADDR>::to_int() const
{
	return (uint64_t) (int64_t) value;
}

template <class MEMORY_ADDR>
DWARF_Flag<MEMORY_ADDR>::DWARF_Flag(bool _value)
	: DWARF_AttributeValue<MEMORY_ADDR>(DW_CLASS_FLAG)
	, value(_value)
{
}

template <class MEMORY_ADDR>
DWARF_Flag<MEMORY_ADDR>::~DWARF_Flag()
{
}

template <class MEMORY_ADDR>
bool DWARF_Flag<MEMORY_ADDR>::GetValue() const
{
	return value;
}

template <class MEMORY_ADDR>
std::string DWARF_Flag<MEMORY_ADDR>::to_string() const
{
	return value ? std::string("1") : std::string("0");
}

template <class MEMORY_ADDR>
uint64_t DWARF_Flag<MEMORY_ADDR>::to_int() const
{
	return value ? 1 : 0;
}

template <class MEMORY_ADDR>
DWARF_String<MEMORY_ADDR>::DWARF_String(uint64_t _debug_strp_offset)
	: DWARF_AttributeValue<MEMORY_ADDR>(DW_CLASS_STRING)
	, debug_strp_offset(_debug_strp_offset)
	, value("")
{
}

template <class MEMORY_ADDR>
DWARF_String<MEMORY_ADDR>::DWARF_String(const char *_value)
	: DWARF_AttributeValue<MEMORY_ADDR>(DW_CLASS_STRING)
	, debug_strp_offset(0xffffffffffffffffULL)
	, value(_value)
{
}

template <class MEMORY_ADDR>
DWARF_String<MEMORY_ADDR>::~DWARF_String()
{
}

template <class MEMORY_ADDR>
unsigned int DWARF_String<MEMORY_ADDR>::GetLength() const
{
	return strlen(value);
}

template <class MEMORY_ADDR>
const char *DWARF_String<MEMORY_ADDR>::GetValue() const
{
	return value;
}

template <class MEMORY_ADDR>
std::string DWARF_String<MEMORY_ADDR>::to_string() const
{
	return std::string(value);
}

template <class MEMORY_ADDR>
DWARF_Address<MEMORY_ADDR>::DWARF_Address(MEMORY_ADDR addr)
	: DWARF_AttributeValue<MEMORY_ADDR>(DW_CLASS_ADDRESS)
	, value(addr)
{
}

template <class MEMORY_ADDR>
DWARF_Address<MEMORY_ADDR>::~DWARF_Address()
{
}

template <class MEMORY_ADDR>
MEMORY_ADDR DWARF_Address<MEMORY_ADDR>::GetValue() const
{
	return value;
}

template <class MEMORY_ADDR>
std::string DWARF_Address<MEMORY_ADDR>::to_string() const
{
	std::stringstream sstr;
	sstr << "0x" << std::hex << value << std::dec;
	return std::string(sstr.str());
}

template <class MEMORY_ADDR>
uint64_t DWARF_Address<MEMORY_ADDR>::to_int() const
{
	return (uint64_t) value;
}

template <class MEMORY_ADDR>
DWARF_Reference<MEMORY_ADDR>::DWARF_Reference(uint64_t _debug_info_offset)
	: DWARF_AttributeValue<MEMORY_ADDR>(DW_CLASS_REFERENCE)
	, debug_info_offset(_debug_info_offset)
	, dw_die(0)
{
}

template <class MEMORY_ADDR>
DWARF_Reference<MEMORY_ADDR>::~DWARF_Reference()
{
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_Reference<MEMORY_ADDR>::GetValue() const
{
	return dw_die;
}

template <class MEMORY_ADDR>
uint64_t DWARF_Reference<MEMORY_ADDR>::GetDebugInfoOffset() const
{
	return debug_info_offset;
}

template <class MEMORY_ADDR>
std::string DWARF_Reference<MEMORY_ADDR>::to_string() const
{
	std::stringstream sstr;
	sstr << debug_info_offset;
	return std::string(sstr.str());
}

template <class MEMORY_ADDR>
void DWARF_Reference<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler)
{
	dw_die = dw_handler->FindDIE(debug_info_offset);
	if(!dw_die)
	{
		std::cerr << "Can't find DIE at offset " << debug_info_offset << std::endl;
	}
}

template <class MEMORY_ADDR>
DWARF_LinePtr<MEMORY_ADDR>::DWARF_LinePtr(uint64_t _debug_line_offset)
	: DWARF_AttributeValue<MEMORY_ADDR>(DW_CLASS_LINEPTR)
	, debug_line_offset(_debug_line_offset)
	, dw_stmt_prog(0)
{
}

template <class MEMORY_ADDR>
DWARF_LinePtr<MEMORY_ADDR>::~DWARF_LinePtr()
{
}

template <class MEMORY_ADDR>
const DWARF_StatementProgram<MEMORY_ADDR> *DWARF_LinePtr<MEMORY_ADDR>::GetValue() const
{
	return dw_stmt_prog;
}

template <class MEMORY_ADDR>
uint64_t DWARF_LinePtr<MEMORY_ADDR>::GetDebugLineOffset() const
{
	return debug_line_offset;
}

template <class MEMORY_ADDR>
std::string DWARF_LinePtr<MEMORY_ADDR>::to_string() const
{
	std::stringstream sstr;
	sstr << debug_line_offset;
	return std::string(sstr.str());
}

template <class MEMORY_ADDR>
void DWARF_LinePtr<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler)
{
	dw_stmt_prog = dw_handler->FindStatementProgram(debug_line_offset);
}

template <class MEMORY_ADDR>
DWARF_LocListPtr<MEMORY_ADDR>::DWARF_LocListPtr(const DWARF_CompilationUnit<MEMORY_ADDR> *_dw_cu, uint64_t _debug_loc_offset)
	: DWARF_AttributeValue<MEMORY_ADDR>(DW_CLASS_LOCLISTPTR)
	, dw_cu(_dw_cu)
	, debug_loc_offset(_debug_loc_offset)
	, dw_loc_list_entry(0)
{
}

template <class MEMORY_ADDR>
DWARF_LocListPtr<MEMORY_ADDR>::~DWARF_LocListPtr()
{
}

template <class MEMORY_ADDR>
const DWARF_LocListEntry<MEMORY_ADDR> *DWARF_LocListPtr<MEMORY_ADDR>::GetValue() const
{
	return dw_loc_list_entry;
}

template <class MEMORY_ADDR>
uint64_t DWARF_LocListPtr<MEMORY_ADDR>::GetDebugLocOffset() const
{
	return debug_loc_offset;
}

template <class MEMORY_ADDR>
std::string DWARF_LocListPtr<MEMORY_ADDR>::to_string() const
{
	std::stringstream sstr;
	sstr << debug_loc_offset;
	return std::string(sstr.str());
}

template <class MEMORY_ADDR>
void DWARF_LocListPtr<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler)
{
	dw_loc_list_entry = dw_handler->FindLocListEntry(dw_cu, debug_loc_offset);
	if(!dw_loc_list_entry)
	{
		std::cerr << "Can't find Loc list entry at offset " << debug_loc_offset << std::endl;
	}
}


template <class MEMORY_ADDR>
DWARF_MacPtr<MEMORY_ADDR>::DWARF_MacPtr(uint64_t _debug_macinfo_offset)
	: DWARF_AttributeValue<MEMORY_ADDR>(DW_CLASS_MACPTR)
	, debug_macinfo_offset(_debug_macinfo_offset)
{
}

template <class MEMORY_ADDR>
DWARF_MacPtr<MEMORY_ADDR>::~DWARF_MacPtr()
{
}

template <class MEMORY_ADDR>
const DWARF_MacInfoListEntry<MEMORY_ADDR> *DWARF_MacPtr<MEMORY_ADDR>::GetValue() const
{
	return dw_macinfo_list_entry;
}

template <class MEMORY_ADDR>
uint64_t DWARF_MacPtr<MEMORY_ADDR>::GetDebugMacInfoOffset() const
{
	return debug_macinfo_offset;
}

template <class MEMORY_ADDR>
std::string DWARF_MacPtr<MEMORY_ADDR>::to_string() const
{
	std::stringstream sstr;
	sstr << debug_macinfo_offset;
	return std::string(sstr.str());
}

template <class MEMORY_ADDR>
void DWARF_MacPtr<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler)
{
	dw_macinfo_list_entry = dw_handler->FindMacInfoListEntry(debug_macinfo_offset);
	if(!dw_macinfo_list_entry)
	{
		std::cerr << "Can't find macinfo list entry at offset " << debug_macinfo_offset << std::endl;
	}
}

template <class MEMORY_ADDR>
DWARF_RangeListPtr<MEMORY_ADDR>::DWARF_RangeListPtr(const DWARF_CompilationUnit<MEMORY_ADDR> *_dw_cu, uint64_t _debug_ranges_offset)
	: DWARF_AttributeValue<MEMORY_ADDR>(DW_CLASS_RANGELISTPTR)
	, dw_cu(_dw_cu)
	, debug_ranges_offset(_debug_ranges_offset)
	, dw_range_list_entry(0)
{
}

template <class MEMORY_ADDR>
DWARF_RangeListPtr<MEMORY_ADDR>::~DWARF_RangeListPtr()
{
}

template <class MEMORY_ADDR>
const DWARF_RangeListEntry<MEMORY_ADDR> *DWARF_RangeListPtr<MEMORY_ADDR>::GetValue() const
{
	return dw_range_list_entry;
}

template <class MEMORY_ADDR>
uint64_t DWARF_RangeListPtr<MEMORY_ADDR>::GetDebugRangesOffset() const
{
	return debug_ranges_offset;
}

template <class MEMORY_ADDR>
std::string DWARF_RangeListPtr<MEMORY_ADDR>::to_string() const
{
	std::stringstream sstr;
	sstr << debug_ranges_offset;
	return std::string(sstr.str());
}

template <class MEMORY_ADDR>
void DWARF_RangeListPtr<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler)
{
	dw_range_list_entry = dw_handler->FindRangeListEntry(dw_cu, debug_ranges_offset);
	if(!dw_range_list_entry)
	{
		std::cerr << "Can't find Range list entry at offset " << debug_ranges_offset << std::endl;
	}
}

template <class MEMORY_ADDR>
DWARF_Expression<MEMORY_ADDR>::DWARF_Expression(const DWARF_CompilationUnit<MEMORY_ADDR> *_dw_cu, uint64_t _length, const uint8_t *_value)
	: DWARF_AttributeValue<MEMORY_ADDR>(DW_CLASS_EXPRESSION)
	, dw_cu(_dw_cu)
	, dw_cfp(0)
	, length(_length)
	, value(_value)
{
}

template <class MEMORY_ADDR>
DWARF_Expression<MEMORY_ADDR>::DWARF_Expression(const DWARF_CallFrameProgram<MEMORY_ADDR> *_dw_cfp, uint64_t _length, const uint8_t *_value)
	: DWARF_AttributeValue<MEMORY_ADDR>(DW_CLASS_EXPRESSION)
	, dw_cu(0)
	, dw_cfp(_dw_cfp)
	, length(_length)
	, value(_value)
{
}

template <class MEMORY_ADDR>
DWARF_Expression<MEMORY_ADDR>::~DWARF_Expression()
{
}

template <class MEMORY_ADDR>
uint64_t DWARF_Expression<MEMORY_ADDR>::GetLength() const
{
	return length;
}

template <class MEMORY_ADDR>
const uint8_t *DWARF_Expression<MEMORY_ADDR>::GetValue() const
{
	return value;
}

template <class MEMORY_ADDR>
std::string DWARF_Expression<MEMORY_ADDR>::to_string() const
{
	std::stringstream sstr;
	
	if(dw_cu)
	{
		DWARF_ExpressionVM<MEMORY_ADDR> expr_vm = DWARF_ExpressionVM<MEMORY_ADDR>(dw_cu);
		return expr_vm.Disasm(sstr, this) ? sstr.str() : std::string();
	}
	
	if(dw_cfp)
	{
		DWARF_ExpressionVM<MEMORY_ADDR> expr_vm = DWARF_ExpressionVM<MEMORY_ADDR>(dw_cfp);
		return expr_vm.Disasm(sstr, this) ? sstr.str() : std::string();
	}
	
	return std::string();
}

template <class MEMORY_ADDR>
DWARF_Attribute<MEMORY_ADDR>::DWARF_Attribute(const DWARF_DIE<MEMORY_ADDR> *_dw_die, DWARF_AbbrevAttribute *_dw_abbrev_attribute, DWARF_AttributeValue<MEMORY_ADDR> *_dw_value)
	: dw_die(_dw_die)
	, dw_abbrev_attribute(_dw_abbrev_attribute)
	, dw_value(_dw_value)
{
}

template <class MEMORY_ADDR>
DWARF_Attribute<MEMORY_ADDR>::~DWARF_Attribute()
{
	if(dw_value)
	{
		delete dw_value;
	}
}

template <class MEMORY_ADDR>
const DWARF_AbbrevAttribute *DWARF_Attribute<MEMORY_ADDR>::GetAbbrevAttribute() const
{
	return dw_abbrev_attribute;
}

template <class MEMORY_ADDR>
const DWARF_AttributeValue<MEMORY_ADDR> *DWARF_Attribute<MEMORY_ADDR>::GetValue() const
{
	return dw_value;
}

template <class MEMORY_ADDR>
void DWARF_Attribute<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler)
{
	uint16_t dw_tag = (uint16_t) dw_abbrev_attribute->GetTag();
	unsigned int dw_class = dw_value->GetClass();
	
	uint64_t ref;
	
	switch(dw_class)
	{
		case DW_CLASS_LINEPTR:
			ref = ((DWARF_LinePtr<MEMORY_ADDR> *) dw_value)->GetDebugLineOffset();
			break;
		case DW_CLASS_LOCLISTPTR:
			ref = ((DWARF_LocListPtr<MEMORY_ADDR> *) dw_value)->GetDebugLocOffset();
			break;
		case DW_CLASS_MACPTR:
			ref = ((DWARF_MacPtr<MEMORY_ADDR> *) dw_value)->GetDebugMacInfoOffset();
			break;
		case DW_CLASS_RANGELISTPTR:
			ref = ((DWARF_RangeListPtr<MEMORY_ADDR> *) dw_value)->GetDebugRangesOffset();
			break;
		case DW_CLASS_REFERENCE:
			ref = ((DWARF_Reference<MEMORY_ADDR> *) dw_value)->GetDebugInfoOffset();
			break;
		default:
			return;
	}
	
	const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu = dw_die->GetCompilationUnit();
	
	switch(dw_tag)
	{
		case DW_AT_sibling:
		case DW_AT_byte_size:
		case DW_AT_bit_offset:
		case DW_AT_bit_size:
		case DW_AT_discr:
		case DW_AT_import:
		case DW_AT_common_reference:
		case DW_AT_containing_type:
		case DW_AT_default_value:
		case DW_AT_lower_bound:
		case DW_AT_upper_bound:
		case DW_AT_abstract_origin:
		case DW_AT_base_types:
		case DW_AT_count:
		case DW_AT_friend:
		case DW_AT_priority:
		case DW_AT_specification:
		case DW_AT_type:
		case DW_AT_allocated:
		case DW_AT_associated:
		case DW_AT_byte_stride:
		case DW_AT_extension:
		case DW_AT_trampoline:
		case DW_AT_small:
		case DW_AT_object_pointer:
			delete dw_value;
			dw_value = new DWARF_Reference<MEMORY_ADDR>(ref);
			break;
		case DW_AT_location:
		case DW_AT_string_length:
		case DW_AT_return_addr:
		case DW_AT_data_member_location:
		case DW_AT_frame_base:
		case DW_AT_segment:
		case DW_AT_static_link:
		case DW_AT_use_location:
		case DW_AT_vtable_elem_location:
			delete dw_value;
			dw_value = new DWARF_LocListPtr<MEMORY_ADDR>(dw_cu, ref);
			break;
		case DW_AT_stmt_list:
			delete dw_value;
			dw_value = new DWARF_LinePtr<MEMORY_ADDR>(ref);
			break;
		case DW_AT_macro_info:
			delete dw_value;
			dw_value = new DWARF_MacPtr<MEMORY_ADDR>(ref);
			break;
		case DW_AT_ranges:
			delete dw_value;
			dw_value = new DWARF_RangeListPtr<MEMORY_ADDR>(dw_cu, ref);
			break;
	}

	dw_value->Fix(dw_handler);
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_Attribute<MEMORY_ADDR>& dw_attribute)
{
	os << dw_attribute.dw_abbrev_attribute->GetName() << ": ";
	
	unsigned int dw_class = dw_attribute.dw_value->GetClass();
	switch(dw_class)
	{
		case DW_CLASS_UNSIGNED_CONSTANT:
		case DW_CLASS_SIGNED_CONSTANT:
		case DW_CLASS_UNSIGNED_LEB128_CONSTANT:
		case DW_CLASS_SIGNED_LEB128_CONSTANT:
			{
				uint64_t int_value = dw_attribute.dw_value->to_int();
				uint16_t dw_at = (uint16_t) dw_attribute.dw_abbrev_attribute->GetTag();
				
				switch(dw_at)
				{
					case DW_AT_encoding:
						os << DWARF_GetATEName(int_value);
						break;
					case DW_AT_language:
						os << DWARF_GetLANGName(int_value);
						break;
					case DW_AT_virtuality:
						os << DWARF_GetVIRTUALITYName(int_value);
						break;
					case DW_AT_visibility:
						os << DWARF_GetVISName(int_value);
						break;
					case DW_AT_accessibility:
						os << DWARF_GetACCESSName(int_value);
						break;
					case DW_AT_endianity:
						os << DWARF_GetENDName(int_value);
						break;
					case DW_AT_decimal_sign:
						os << DWARF_GetDSName(int_value);
						break;
					case DW_AT_identifier_case:
						os << DWARF_GetIDName(int_value);
						break;
					case DW_AT_calling_convention:
						os << DWARF_GetCCName(int_value);
						break;
					case DW_AT_inline:
						os << DWARF_GetINLName(int_value);
						break;
					case DW_AT_ordering:
						os << DWARF_GetORDName(int_value);
						break;
					case DW_AT_discr_list:
						os << DWARF_GetDSCName(int_value);
						break;
					default:
						os << dw_attribute.dw_value->to_string();
						break;
				}
			}
			break;
		default:
			os << dw_attribute.dw_value->to_string();
			break;
	}
	
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_Attribute<MEMORY_ADDR>::to_XML(std::ostream& os)
{
	os << "<DW_AT name=\"";
	c_string_to_XML(os, dw_abbrev_attribute->GetName());
	os << "\" class=\"";
	c_string_to_XML(os, dw_value->GetClassName());
	os << "\" value=\"";

	unsigned int dw_class = dw_value->GetClass();
	switch(dw_class)
	{
		case DW_CLASS_UNSIGNED_CONSTANT:
		case DW_CLASS_SIGNED_CONSTANT:
		case DW_CLASS_UNSIGNED_LEB128_CONSTANT:
		case DW_CLASS_SIGNED_LEB128_CONSTANT:
			{
				uint64_t int_value = dw_value->to_int();
				uint16_t dw_at = (uint16_t) dw_abbrev_attribute->GetTag();
				
				switch(dw_at)
				{
					case DW_AT_encoding:
						c_string_to_XML(os, DWARF_GetATEName(int_value));
						break;
					case DW_AT_language:
						c_string_to_XML(os, DWARF_GetLANGName(int_value));
						break;
					case DW_AT_virtuality:
						c_string_to_XML(os, DWARF_GetVIRTUALITYName(int_value));
						break;
					case DW_AT_visibility:
						c_string_to_XML(os, DWARF_GetVISName(int_value));
						break;
					case DW_AT_accessibility:
						c_string_to_XML(os, DWARF_GetACCESSName(int_value));
						break;
					case DW_AT_endianity:
						c_string_to_XML(os, DWARF_GetENDName(int_value));
						break;
					case DW_AT_decimal_sign:
						c_string_to_XML(os, DWARF_GetDSName(int_value));
						break;
					case DW_AT_identifier_case:
						c_string_to_XML(os, DWARF_GetIDName(int_value));
						break;
					case DW_AT_calling_convention:
						c_string_to_XML(os, DWARF_GetCCName(int_value));
						break;
					case DW_AT_inline:
						c_string_to_XML(os, DWARF_GetINLName(int_value));
						break;
					case DW_AT_ordering:
						c_string_to_XML(os, DWARF_GetORDName(int_value));
						break;
					case DW_AT_discr_list:
						c_string_to_XML(os, DWARF_GetDSCName(int_value));
						break;
					default:
						c_string_to_XML(os, dw_value->to_string().c_str());
						break;
				}
			}
			break;
		default:
			c_string_to_XML(os, dw_value->to_string().c_str());
			break;
	}
	os << "\"/>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_Attribute<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	os << "<tr>" << std::endl;
	os << "<td>" << std::endl;
	c_string_to_HTML(os, dw_abbrev_attribute->GetName());
	os << "</td>" << std::endl;
	os << "<td>" << std::endl;
	c_string_to_HTML(os, dw_value->GetClassName());
	os << "</td>" << std::endl;
	os << "<td>" << std::endl;
	unsigned int dw_class = dw_value->GetClass();
	switch(dw_class)
	{
		case DW_CLASS_REFERENCE:
			{
				DWARF_Reference<MEMORY_ADDR> *dw_ref = (DWARF_Reference<MEMORY_ADDR> *) dw_value;
				const DWARF_DIE<MEMORY_ADDR> *dw_ref_die = dw_ref->GetValue();
				os << "<a href=\"../../" << dw_ref_die->GetHREF() << "\">die-" << dw_ref_die->GetId() << "</a>" << std::endl;
			}
			break;
		case DW_CLASS_MACPTR:
			{
				DWARF_MacPtr<MEMORY_ADDR> *dw_macptr = (DWARF_MacPtr<MEMORY_ADDR> *) dw_value;
				const DWARF_MacInfoListEntry<MEMORY_ADDR> *dw_macinfo_list_entry = dw_macptr->GetValue();
				os << "<a href=\"../../" << dw_macinfo_list_entry->GetHREF() << "\">mac-" << dw_macinfo_list_entry->GetId() << "</a>" << std::endl;
			}
			break;
		case DW_CLASS_LOCLISTPTR:
			{
				DWARF_LocListPtr<MEMORY_ADDR> *dw_loclistptr = (DWARF_LocListPtr<MEMORY_ADDR> *) dw_value;
				const DWARF_LocListEntry<MEMORY_ADDR> *dw_loc_list_entry = dw_loclistptr->GetValue();
				os << "<a href=\"../../" << dw_loc_list_entry->GetHREF() << "\">loc-" << dw_loc_list_entry->GetId() << "</a>" << std::endl;
			}
			break;
		case DW_CLASS_LINEPTR:
			{
				DWARF_LinePtr<MEMORY_ADDR> *dw_lineptr = (DWARF_LinePtr<MEMORY_ADDR> *) dw_value;
				const DWARF_StatementProgram<MEMORY_ADDR> *dw_stmt_prog = dw_lineptr->GetValue();
				os << "<a href=\"../../" << dw_stmt_prog->GetHREF() << "\">stmt-prog-" << dw_stmt_prog->GetId() << "</a>" << std::endl;
			}
			break;
		case DW_CLASS_UNSIGNED_CONSTANT:
		case DW_CLASS_SIGNED_CONSTANT:
		case DW_CLASS_UNSIGNED_LEB128_CONSTANT:
		case DW_CLASS_SIGNED_LEB128_CONSTANT:
			{
				uint64_t int_value = dw_value->to_int();
				uint16_t dw_at = (uint16_t) dw_abbrev_attribute->GetTag();
				
				switch(dw_at)
				{
					case DW_AT_encoding:
						c_string_to_HTML(os, DWARF_GetATEName(int_value));
						break;
					case DW_AT_language:
						c_string_to_HTML(os, DWARF_GetLANGName(int_value));
						break;
					case DW_AT_virtuality:
						c_string_to_HTML(os, DWARF_GetVIRTUALITYName(int_value));
						break;
					case DW_AT_visibility:
						c_string_to_HTML(os, DWARF_GetVISName(int_value));
						break;
					case DW_AT_accessibility:
						c_string_to_HTML(os, DWARF_GetACCESSName(int_value));
						break;
					case DW_AT_endianity:
						c_string_to_HTML(os, DWARF_GetENDName(int_value));
						break;
					case DW_AT_decimal_sign:
						c_string_to_HTML(os, DWARF_GetDSName(int_value));
						break;
					case DW_AT_identifier_case:
						c_string_to_HTML(os, DWARF_GetIDName(int_value));
						break;
					case DW_AT_calling_convention:
						c_string_to_HTML(os, DWARF_GetCCName(int_value));
						break;
					case DW_AT_inline:
						c_string_to_HTML(os, DWARF_GetINLName(int_value));
						break;
					case DW_AT_ordering:
						c_string_to_HTML(os, DWARF_GetORDName(int_value));
						break;
					case DW_AT_discr_list:
						c_string_to_HTML(os, DWARF_GetDSCName(int_value));
						break;
					default:
						c_string_to_HTML(os, dw_value->to_string().c_str());
						break;
				}
			}
			break;
		default:
			c_string_to_HTML(os, dw_value->to_string().c_str());
			break;
	}
	os << "</tr>" << std::endl;

	return os;
}

template <class MEMORY_ADDR>
DWARF_DIE<MEMORY_ADDR>::DWARF_DIE(DWARF_CompilationUnit<MEMORY_ADDR> *_dw_cu, DWARF_DIE<MEMORY_ADDR> *_dw_parent_die)
	: dw_cu(_dw_cu)
	, dw_parent_die(_dw_parent_die)
	, offset(0xffffffffffffffffULL)
	, abbrev(0)
	, children()
	, attributes()
{
}

template <class MEMORY_ADDR>
DWARF_DIE<MEMORY_ADDR>::~DWARF_DIE()
{
	unsigned int num_attributes = attributes.size();
	unsigned int i;
	for(i = 0; i < num_attributes; i++)
	{
		delete attributes[i];
	}
	unsigned int num_children = children.size();
	for(i = 0; i < num_children; i++)
	{
		delete children[i];
	}
}

template <class MEMORY_ADDR>
const DWARF_CompilationUnit<MEMORY_ADDR> *DWARF_DIE<MEMORY_ADDR>::GetCompilationUnit() const
{
	return dw_cu;
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_DIE<MEMORY_ADDR>::GetParentDIE() const
{
	return dw_parent_die;
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_DIE<MEMORY_ADDR>::GetRootDIE() const
{
	return dw_parent_die ? dw_parent_die->GetRootDIE() : this;
}

template <class MEMORY_ADDR>
const DWARF_Abbrev *DWARF_DIE<MEMORY_ADDR>::GetAbbrev() const
{
	return abbrev;
}

template <class MEMORY_ADDR>
const std::vector<DWARF_DIE<MEMORY_ADDR> *>& DWARF_DIE<MEMORY_ADDR>::GetChildren() const
{
	return children;
}

template <class MEMORY_ADDR>
const std::vector<DWARF_Attribute<MEMORY_ADDR> *>& DWARF_DIE<MEMORY_ADDR>::GetAttributes() const
{
	return attributes;
}

template <class MEMORY_ADDR>
const DWARF_Attribute<MEMORY_ADDR> *DWARF_DIE<MEMORY_ADDR>::GetAttribute(uint16_t dw_at) const
{
	return 0;
}

template <class MEMORY_ADDR>
uint16_t DWARF_DIE<MEMORY_ADDR>::GetTag() const
{
	return abbrev ? (uint16_t) abbrev->GetTag() : 0;
}

template <class MEMORY_ADDR>
uint64_t DWARF_DIE<MEMORY_ADDR>::GetOffset() const
{
	return offset;
}

template <class MEMORY_ADDR>
void DWARF_DIE<MEMORY_ADDR>::Add(DWARF_Attribute<MEMORY_ADDR> *dw_attribute)
{
	attributes.push_back(dw_attribute);
}

template <class MEMORY_ADDR>
void DWARF_DIE<MEMORY_ADDR>::Add(DWARF_DIE<MEMORY_ADDR> *child)
{
	children.push_back(child);
}

template <class MEMORY_ADDR>
int64_t DWARF_DIE<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size, uint64_t _offset)
{
	offset = _offset;
	uint8_t address_size = dw_cu->GetAddressSize();
	uint8_t offset_size = dw_cu->GetOffsetSize();
	endian_type endianness = dw_cu->GetEndianness();
	uint64_t size = 0;
	int64_t sz;
	DWARF_LEB128 dw_abbrev_code;
	if((sz = dw_abbrev_code.Load(rawdata, max_size)) < 0) return -1;
	rawdata += sz;
	max_size -= sz;
	size += sz;
	if(dw_abbrev_code.IsZero())
	{
		abbrev = 0;
		return size;
	}

	//std::cerr << ".debug_info: at " << size << " got " << dw_abbrev_code.to_string(false) << std::endl;

	
	//std::cerr << "Looking for abbrevation " << dw_abbrev_code.to_string(false) << " starting offset " << debug_abbrev_offset << std::endl;
	
	const DWARF_Abbrev *dw_abbrev = dw_cu->FindAbbrev(dw_abbrev_code);
	
	if(!dw_abbrev)
	{
		std::cerr << "Can't find abbrevation " << dw_abbrev_code.to_string(false) << std::endl;
		return -1;
	}
	
	const std::vector<DWARF_AbbrevAttribute *>& dw_abbrev_attributes = dw_abbrev->GetAbbrevAttributes();
	std::vector<DWARF_AbbrevAttribute *>::const_iterator dw_abbrev_attribute_iter;
			
	for(dw_abbrev_attribute_iter = dw_abbrev_attributes.begin(); dw_abbrev_attribute_iter != dw_abbrev_attributes.end(); dw_abbrev_attribute_iter++)
	{
		DWARF_AbbrevAttribute *dw_abbrev_attribute = *dw_abbrev_attribute_iter;
		
		//std::cerr << *dw_abbrev_attribute << std::endl;
		
		const DWARF_LEB128& dw_at_leb128 = dw_abbrev_attribute->GetTag();
		const DWARF_LEB128& dw_form_leb128 = dw_abbrev_attribute->GetForm();
		
		
		DWARF_Attribute<MEMORY_ADDR> *dw_attribute = 0;
		
		uint16_t dw_form = (uint16_t) dw_form_leb128;
		uint16_t dw_at = (uint16_t) dw_at_leb128;
		
		if(dw_form == DW_FORM_indirect)
		{
			DWARF_LEB128 dw_actual_form_leb128;
			int64_t sz;
			if((sz = dw_actual_form_leb128.Load(rawdata, max_size)) < 0) return -1;
			size += sz;
			rawdata += sz;
			max_size -= sz;
			
			dw_form = (uint16_t) dw_actual_form_leb128;
		}
		
		switch(dw_form)
		{
			case DW_FORM_addr:
				{
					MEMORY_ADDR addr;
					
					switch(address_size)
					{
						case sizeof(uint8_t):
							{
								if(max_size < sizeof(uint8_t)) return -1;
								uint8_t value;
								memcpy(&value, rawdata, sizeof(uint8_t));
								addr = Target2Host(endianness, value);
								size += sizeof(uint8_t);
								rawdata += sizeof(uint8_t);
								max_size -= sizeof(uint8_t);
							}
							break;
						case sizeof(uint16_t):
							{
								if(max_size < sizeof(uint16_t)) return -1;
								uint16_t value;
								memcpy(&value, rawdata, sizeof(uint16_t));
								addr = Target2Host(endianness, value);
								size += sizeof(uint16_t);
								rawdata += sizeof(uint16_t);
								max_size -= sizeof(uint16_t);
							}
							break;
						case sizeof(uint32_t):
							{
								if(max_size < sizeof(uint32_t)) return -1;
								uint32_t value;
								memcpy(&value, rawdata, sizeof(uint32_t));
								addr = Target2Host(endianness, value);
								size += sizeof(uint32_t);
								rawdata += sizeof(uint32_t);
								max_size -= sizeof(uint32_t);
							}
							break;
						case sizeof(uint64_t):
							{
								if(max_size < sizeof(uint64_t)) return -1;
								uint64_t value;
								memcpy(&value, rawdata, sizeof(uint64_t));
								addr = Target2Host(endianness, value);
								size += sizeof(uint64_t);
								rawdata += sizeof(uint64_t);
								max_size -= sizeof(uint64_t);
							}
							break;
						default:
							return -1;
					}
					
					DWARF_Address<MEMORY_ADDR> *dw_address = new DWARF_Address<MEMORY_ADDR>(addr);
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_address);
				}
				break;
			case DW_FORM_block2:
				{
					if(max_size < sizeof(uint16_t)) return -1;
					uint16_t block_length;
					memcpy(&block_length, rawdata, sizeof(block_length));
					block_length = Target2Host(endianness, block_length);
					size += sizeof(block_length);
					rawdata += sizeof(block_length);
					max_size -= sizeof(block_length);
					if(max_size < block_length) return -1;
					
					DWARF_AttributeValue<MEMORY_ADDR> *dw_value;
					
					switch(dw_at)
					{
						case DW_AT_location:
						case DW_AT_byte_size:
						case DW_AT_bit_offset:
						case DW_AT_bit_size:
						case DW_AT_string_length:
						case DW_AT_const_value:
						case DW_AT_lower_bound:
						case DW_AT_return_addr:
						case DW_AT_upper_bound:
						case DW_AT_count:
						case DW_AT_data_member_location:
						case DW_AT_frame_base:
						case DW_AT_segment:
						case DW_AT_static_link:
						case DW_AT_use_location:
						case DW_AT_vtable_elem_location:
						case DW_AT_allocated:
						case DW_AT_associated:
						case DW_AT_data_location:
						case DW_AT_byte_stride:
							dw_value = new DWARF_Expression<MEMORY_ADDR>(dw_cu, block_length, rawdata);
							break;
						default:
							dw_value = new DWARF_Block<MEMORY_ADDR>(block_length, rawdata);
							break;
					}
					
					rawdata += block_length;
					size += block_length;
					max_size -= block_length;
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_value);
				}
				break;
			case DW_FORM_block4:
				{
					if(max_size < sizeof(uint32_t)) return -1;
					uint32_t block_length;
					memcpy(&block_length, rawdata, sizeof(block_length));
					block_length = Target2Host(endianness, block_length);
					size += sizeof(block_length);
					rawdata += sizeof(block_length);
					max_size -= sizeof(block_length);
					if(max_size < block_length) return -1;
					
					DWARF_AttributeValue<MEMORY_ADDR> *dw_value;
					
					switch(dw_at)
					{
						case DW_AT_location:
						case DW_AT_byte_size:
						case DW_AT_bit_offset:
						case DW_AT_bit_size:
						case DW_AT_string_length:
						case DW_AT_const_value:
						case DW_AT_lower_bound:
						case DW_AT_return_addr:
						case DW_AT_upper_bound:
						case DW_AT_count:
						case DW_AT_data_member_location:
						case DW_AT_frame_base:
						case DW_AT_segment:
						case DW_AT_static_link:
						case DW_AT_use_location:
						case DW_AT_vtable_elem_location:
						case DW_AT_allocated:
						case DW_AT_associated:
						case DW_AT_data_location:
						case DW_AT_byte_stride:
							dw_value = new DWARF_Expression<MEMORY_ADDR>(dw_cu, block_length, rawdata);
							break;
						default:
							dw_value = new DWARF_Block<MEMORY_ADDR>(block_length, rawdata);
							break;
					}
					
					rawdata += block_length;
					size += block_length;
					max_size -= block_length;
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_value);
				}
				break;
			case DW_FORM_data2:
				{
					if(max_size < sizeof(uint16_t)) return -1;
					uint16_t value;
					memcpy(&value, rawdata, sizeof(uint16_t));
					value = Target2Host(endianness, value);
					size += sizeof(uint16_t);
					rawdata += sizeof(uint16_t);
					max_size -= sizeof(uint16_t);
					DWARF_UnsignedConstant<MEMORY_ADDR> *dw_const = new DWARF_UnsignedConstant<MEMORY_ADDR>((uint64_t) value);
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_const);
				}
				break;
			case DW_FORM_data4:
				{
					if(max_size < sizeof(uint32_t)) return -1;
					uint32_t value;
					memcpy(&value, rawdata, sizeof(uint32_t));
					value = Target2Host(endianness, value);
					size += sizeof(uint32_t);
					rawdata += sizeof(uint32_t);
					max_size -= sizeof(uint32_t);
					
					DWARF_AttributeValue<MEMORY_ADDR> *dw_value;
					
					switch(dw_at)
					{
						case DW_AT_stmt_list:
							dw_value = new DWARF_LinePtr<MEMORY_ADDR>((uint64_t) value);
							break;
						case DW_AT_ranges:
							dw_value = new DWARF_RangeListPtr<MEMORY_ADDR>(dw_cu, (uint64_t) value);
							break;
						case DW_AT_macro_info:
							dw_value = new DWARF_MacPtr<MEMORY_ADDR>((uint64_t) value);
							break;
						case DW_AT_location:
						case DW_AT_string_length:
						case DW_AT_return_addr:
						case DW_AT_data_member_location:
						case DW_AT_frame_base:
						case DW_AT_segment:
						case DW_AT_static_link:
						case DW_AT_use_location:
						case DW_AT_vtable_elem_location:
							dw_value = new DWARF_LocListPtr<MEMORY_ADDR>(dw_cu, (uint64_t) value);
							break;
						default:
							dw_value = new DWARF_UnsignedConstant<MEMORY_ADDR>((uint64_t) value);
							break;
					}
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_value);
				}
				break;
			case DW_FORM_data8:
				{
					if(max_size < sizeof(uint64_t)) return -1;
					uint64_t value;
					memcpy(&value, rawdata, sizeof(uint64_t));
					value = Target2Host(endianness, value);
					size += sizeof(uint64_t);
					rawdata += sizeof(uint64_t);
					max_size -= sizeof(uint64_t);

					DWARF_AttributeValue<MEMORY_ADDR> *dw_value;
					
					switch(dw_at)
					{
						case DW_AT_stmt_list:
							dw_value = new DWARF_LinePtr<MEMORY_ADDR>((uint64_t) value);
							break;
						case DW_AT_ranges:
							dw_value = new DWARF_RangeListPtr<MEMORY_ADDR>(dw_cu, (uint64_t) value);
							break;
						case DW_AT_macro_info:
							dw_value = new DWARF_MacPtr<MEMORY_ADDR>((uint64_t) value);
							break;
						case DW_AT_location:
						case DW_AT_string_length:
						case DW_AT_return_addr:
						case DW_AT_data_member_location:
						case DW_AT_frame_base:
						case DW_AT_segment:
						case DW_AT_static_link:
						case DW_AT_use_location:
						case DW_AT_vtable_elem_location:
							dw_value = new DWARF_LocListPtr<MEMORY_ADDR>(dw_cu, (uint64_t) value);
							break;
						default:
							dw_value = new DWARF_UnsignedConstant<MEMORY_ADDR>((uint64_t) value);
							break;
					}
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_value);
				}
				break;
			case DW_FORM_string:
				{
					const char *c_string = (const char *) rawdata;
					unsigned int string_length = strlen(c_string);
					if(max_size < (string_length + 1)) return -1;
					size += string_length + 1;
					rawdata += string_length + 1;
					max_size -= string_length + 1;
					DWARF_String<MEMORY_ADDR> *dw_string = new DWARF_String<MEMORY_ADDR>(c_string);
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_string);
				}
				break;
			case DW_FORM_block:
				{
					DWARF_LEB128 leb128_block_length;
					int64_t sz;
					if((sz = leb128_block_length.Load(rawdata, max_size)) < 0) return -1;
					rawdata += sz;
					size += sz;
					max_size -= sz;

					uint64_t block_length = (uint64_t) leb128_block_length;
					if(max_size < block_length) return -1;

					DWARF_AttributeValue<MEMORY_ADDR> *dw_value;
					
					switch(dw_at)
					{
						case DW_AT_location:
						case DW_AT_byte_size:
						case DW_AT_bit_offset:
						case DW_AT_bit_size:
						case DW_AT_string_length:
						case DW_AT_const_value:
						case DW_AT_lower_bound:
						case DW_AT_return_addr:
						case DW_AT_upper_bound:
						case DW_AT_count:
						case DW_AT_data_member_location:
						case DW_AT_frame_base:
						case DW_AT_segment:
						case DW_AT_static_link:
						case DW_AT_use_location:
						case DW_AT_vtable_elem_location:
						case DW_AT_allocated:
						case DW_AT_associated:
						case DW_AT_data_location:
						case DW_AT_byte_stride:
							dw_value = new DWARF_Expression<MEMORY_ADDR>(dw_cu, block_length, rawdata);
							break;
						default:
							dw_value = new DWARF_Block<MEMORY_ADDR>(block_length, rawdata);
							break;
					}
					
					rawdata += block_length;
					size += block_length;
					max_size -= block_length;
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_value);
				}
				break;
			case DW_FORM_block1:
				{
					if(max_size < sizeof(uint8_t)) return -1;
					uint8_t block_length;
					memcpy(&block_length, rawdata, sizeof(block_length));
					block_length = Target2Host(endianness, block_length);
					size += sizeof(block_length);
					rawdata += sizeof(block_length);
					max_size -= sizeof(block_length);
					
					if(max_size < block_length) return -1;

					DWARF_AttributeValue<MEMORY_ADDR> *dw_value;
					
					switch(dw_at)
					{
						case DW_AT_location:
						case DW_AT_byte_size:
						case DW_AT_bit_offset:
						case DW_AT_bit_size:
						case DW_AT_string_length:
						case DW_AT_const_value:
						case DW_AT_lower_bound:
						case DW_AT_return_addr:
						case DW_AT_upper_bound:
						case DW_AT_count:
						case DW_AT_data_member_location:
						case DW_AT_frame_base:
						case DW_AT_segment:
						case DW_AT_static_link:
						case DW_AT_use_location:
						case DW_AT_vtable_elem_location:
						case DW_AT_allocated:
						case DW_AT_associated:
						case DW_AT_data_location:
						case DW_AT_byte_stride:
							dw_value = new DWARF_Expression<MEMORY_ADDR>(dw_cu, block_length, rawdata);
							break;
						default:
							dw_value = new DWARF_Block<MEMORY_ADDR>(block_length, rawdata);
							break;
					}
					
					rawdata += block_length;
					size += block_length;
					max_size -= block_length;
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_value);
				}
				break;
			case DW_FORM_data1:
				{
					if(max_size < sizeof(uint8_t)) return -1;
					uint8_t value;
					memcpy(&value, rawdata, sizeof(uint8_t));
					value = Target2Host(endianness, value);
					size += sizeof(uint8_t);
					rawdata += sizeof(uint8_t);
					max_size -= sizeof(uint8_t);
					DWARF_UnsignedConstant<MEMORY_ADDR> *dw_const = new DWARF_UnsignedConstant<MEMORY_ADDR>((uint64_t) value);
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_const);
				}
				break;
			case DW_FORM_flag:
				{
					if(max_size < sizeof(uint8_t)) return -1;
					uint8_t value;
					memcpy(&value, rawdata, sizeof(uint8_t));
					size += sizeof(uint8_t);
					rawdata += sizeof(uint8_t);
					max_size -= sizeof(uint8_t);
					
					DWARF_Flag<MEMORY_ADDR> *dw_flag = new DWARF_Flag<MEMORY_ADDR>(value ? true : false);
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_flag);
				}
				break;
			case DW_FORM_sdata:
				{
					DWARF_LEB128 leb128;
					int64_t sz;
					if((sz = leb128.Load(rawdata, max_size)) < 0) return -1;
					size += sz;
					rawdata += sz;
					max_size -= sz;
					
					DWARF_SignedLEB128Constant<MEMORY_ADDR> *dw_const = new DWARF_SignedLEB128Constant<MEMORY_ADDR>(leb128);
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_const);
				}
				break;
			case DW_FORM_strp:
				{
					uint64_t debug_str_offset;
					switch(offset_size)
					{
						case 2:
							{
								uint16_t debug_str_offset16;
							
								if(max_size < sizeof(debug_str_offset16)) return -1;
								memcpy(&debug_str_offset16, rawdata, sizeof(debug_str_offset16));
								debug_str_offset16 = Target2Host(endianness, debug_str_offset16);
								rawdata += sizeof(debug_str_offset16);
								max_size -= sizeof(debug_str_offset16);
								size += sizeof(debug_str_offset16);
								debug_str_offset = debug_str_offset16;
							}
							break;
						case 4:
							{
								uint32_t debug_str_offset32;
							
								if(max_size < sizeof(debug_str_offset32)) return -1;
								memcpy(&debug_str_offset32, rawdata, sizeof(debug_str_offset32));
								debug_str_offset32 = Target2Host(endianness, debug_str_offset32);
								rawdata += sizeof(debug_str_offset32);
								max_size -= sizeof(debug_str_offset32);
								size += sizeof(debug_str_offset32);
								debug_str_offset = debug_str_offset32;
							}
							break;
						case 8:
							{
								uint64_t debug_str_offset64;
							
								if(max_size < sizeof(debug_str_offset64)) return -1;
								memcpy(&debug_str_offset, rawdata, sizeof(debug_str_offset64));
								debug_str_offset64 = Target2Host(endianness, debug_str_offset64);
								rawdata += sizeof(debug_str_offset64);
								max_size -= sizeof(debug_str_offset64);
								size += sizeof(debug_str_offset64);
								debug_str_offset = debug_str_offset64;
							}
							break;
						default:
							return -1;
					}
					const char *c_str = dw_cu->GetString(debug_str_offset);
					if(!c_str) return -1;
					DWARF_String<MEMORY_ADDR> *dw_string = new DWARF_String<MEMORY_ADDR>(c_str);
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_string);
				}
				break;
			case DW_FORM_udata:
				{
					DWARF_LEB128 leb128;
					int64_t sz;
					if((sz = leb128.Load(rawdata, max_size)) < 0) return -1;
					size += sz;
					rawdata += sz;
					max_size -= sz;
					
					DWARF_UnsignedLEB128Constant<MEMORY_ADDR> *dw_const = new DWARF_UnsignedLEB128Constant<MEMORY_ADDR>(leb128);
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_const);
				}
				break;
			case DW_FORM_ref_addr:
				{
					uint64_t debug_info_offset;
					switch(offset_size)
					{
						case 2:
							{
								uint16_t debug_info_offset16;
							
								if(max_size < sizeof(debug_info_offset16)) return -1;
								memcpy(&debug_info_offset16, rawdata, sizeof(debug_info_offset16));
								debug_info_offset16 = Target2Host(endianness, debug_info_offset16);
								rawdata += sizeof(debug_info_offset16);
								max_size -= sizeof(debug_info_offset16);
								size += sizeof(debug_info_offset16);
								debug_info_offset = debug_info_offset16;
							}
							break;
						case 4:
							{
								uint32_t debug_info_offset32;
							
								if(max_size < sizeof(debug_info_offset32)) return -1;
								memcpy(&debug_info_offset32, rawdata, sizeof(debug_info_offset32));
								debug_info_offset32 = Target2Host(endianness, debug_info_offset32);
								rawdata += sizeof(debug_info_offset32);
								max_size -= sizeof(debug_info_offset32);
								size += sizeof(debug_info_offset32);
								debug_info_offset = debug_info_offset32;
							}
							break;
						case 8:
							{
								uint64_t debug_info_offset64;
							
								if(max_size < sizeof(debug_info_offset64)) return -1;
								memcpy(&debug_info_offset, rawdata, sizeof(debug_info_offset64));
								debug_info_offset64 = Target2Host(endianness, debug_info_offset64);
								rawdata += sizeof(debug_info_offset64);
								max_size -= sizeof(debug_info_offset64);
								size += sizeof(debug_info_offset64);
								debug_info_offset = debug_info_offset64;
							}
							break;
						default:
							return -1;
					}
					DWARF_Reference<MEMORY_ADDR> *dw_ref = new DWARF_Reference<MEMORY_ADDR>(debug_info_offset);
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_ref);
				}
				break;
			case DW_FORM_ref1:
				{
					uint8_t debug_info_offset;
					
					if(max_size < sizeof(debug_info_offset)) return -1;
					memcpy(&debug_info_offset, rawdata, sizeof(debug_info_offset));
					debug_info_offset = Target2Host(endianness, debug_info_offset);
					rawdata += sizeof(debug_info_offset);
					max_size -= sizeof(debug_info_offset);
					size += sizeof(debug_info_offset);

					DWARF_Reference<MEMORY_ADDR> *dw_ref = new DWARF_Reference<MEMORY_ADDR>(dw_cu->GetOffset() + (uint64_t) debug_info_offset);
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_ref);
				}
				break;
			case DW_FORM_ref2:
				{
					uint16_t debug_info_offset;
					
					if(max_size < sizeof(debug_info_offset)) return -1;
					memcpy(&debug_info_offset, rawdata, sizeof(debug_info_offset));
					debug_info_offset = Target2Host(endianness, debug_info_offset);
					rawdata += sizeof(debug_info_offset);
					max_size -= sizeof(debug_info_offset);
					size += sizeof(debug_info_offset);

					DWARF_Reference<MEMORY_ADDR> *dw_ref = new DWARF_Reference<MEMORY_ADDR>(dw_cu->GetOffset() + (uint64_t) debug_info_offset);
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_ref);
				}
				break;
			case DW_FORM_ref4:
				{
					uint32_t debug_info_offset;
					
					if(max_size < sizeof(debug_info_offset)) return -1;
					memcpy(&debug_info_offset, rawdata, sizeof(debug_info_offset));
					debug_info_offset = Target2Host(endianness, debug_info_offset);
					rawdata += sizeof(debug_info_offset);
					max_size -= sizeof(debug_info_offset);
					size += sizeof(debug_info_offset);

					DWARF_Reference<MEMORY_ADDR> *dw_ref = new DWARF_Reference<MEMORY_ADDR>(dw_cu->GetOffset() + (uint64_t) debug_info_offset);
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_ref);
				}
				break;
			case DW_FORM_ref8:
				{
					uint64_t debug_info_offset;
					
					if(max_size < sizeof(debug_info_offset)) return -1;
					memcpy(&debug_info_offset, rawdata, sizeof(debug_info_offset));
					debug_info_offset = Target2Host(endianness, debug_info_offset);
					rawdata += sizeof(debug_info_offset);
					max_size -= sizeof(debug_info_offset);
					size += sizeof(debug_info_offset);

					DWARF_Reference<MEMORY_ADDR> *dw_ref = new DWARF_Reference<MEMORY_ADDR>(dw_cu->GetOffset() + debug_info_offset);
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_ref);
				}
				break;
			case DW_FORM_ref_udata:
				{
					DWARF_LEB128 debug_info_offset;
					int64_t sz;
					if((sz = debug_info_offset.Load(rawdata, max_size)) < 0) return -1;
					size += sz;
					rawdata += sz;
					max_size -= sz;

					DWARF_Reference<MEMORY_ADDR> *dw_ref = new DWARF_Reference<MEMORY_ADDR>(dw_cu->GetOffset() + (uint64_t) debug_info_offset);
					dw_attribute = new DWARF_Attribute<MEMORY_ADDR>(this, dw_abbrev_attribute, dw_ref);
				}
				break;
			default:
				return -1;
		}
		
		if(dw_attribute)
		{
			Add(dw_attribute);
		}
	}
	
	if(dw_abbrev->HasChildren())
	{
		DWARF_DIE<MEMORY_ADDR> *dw_die = 0;
		do
		{
			dw_die = new DWARF_DIE<MEMORY_ADDR>(dw_cu, this);
			
			if((sz = dw_die->Load(rawdata, max_size, offset + size)) < 0)
			{
				delete dw_die;
				return -1;
			}
			rawdata += sz;
			max_size -= sz;
			size += sz;
			
			children.push_back(dw_die);
			dw_cu->Register(dw_die);
		}
		while(!dw_die->IsNull());
	}
	
	abbrev = dw_abbrev;
	
	return size;
}

template <class MEMORY_ADDR>
bool DWARF_DIE<MEMORY_ADDR>::IsNull() const
{
	return abbrev == 0;
}

template <class MEMORY_ADDR>
void DWARF_DIE<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int _id)
{
	id = _id;
	unsigned int num_attributes = attributes.size();
	unsigned int i;

	for(i = 0; i < num_attributes; i++)
	{
		attributes[i]->Fix(dw_handler);
	}
}

template <class MEMORY_ADDR>
std::string DWARF_DIE<MEMORY_ADDR>::GetHREF() const
{
	std::stringstream sstr;
	sstr << "debug_info/dies/" << (id / dies_per_file) << ".html#die-" << id;
	return sstr.str();
}

template <class MEMORY_ADDR>
unsigned int DWARF_DIE<MEMORY_ADDR>::GetId() const
{
	return id;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_DIE<MEMORY_ADDR>::to_XML(std::ostream& os)
{
	unsigned int i;

	os << "<DW_DIE offset=\"" << offset << "\"" << ">" << std::endl;
	unsigned int num_attributes = attributes.size();
	for(i = 0; i < num_attributes; i++)
	{
		attributes[i]->to_XML(os) << std::endl;
	}
	unsigned int num_children = children.size();
	for(i = 0; i < num_children; i++)
	{
		children[i]->to_XML(os) << std::endl;
	}
	os << "</DW_DIE>" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_DIE<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	unsigned int i;

	os << "<tr id=\"die-" << id << "\">" << std::endl;
	os << "<td>" << id << "</td><td>" << offset << "</td><td><a href=\"../../" << dw_cu->GetHREF() << "\">cu-" << dw_cu->GetId() << "</a></td><td>";
	if(dw_parent_die)
	{
		os << "<a href=\"../../" << dw_parent_die->GetHREF() << "\">die-" << dw_parent_die->GetId() << "</a>";
	}
	else
	{
		os << "-";
	}
	os << std::endl;
	os << "<td>" << std::endl;
	unsigned int num_children = children.size();
	for(i = 0; i < num_children; i++)
	{
		if(i != 0) os << "&nbsp;";//"<br>";
		os << "<a href=\"../../" << children[i]->GetHREF() << "\">die-" << children[i]->GetId() << "</a>" << std::endl;
	}
	os << "</td>" << std::endl;
	os << "<td>" << std::endl;
	os << "<table>" << std::endl;
	os << "<tr><th>Name</th><th>Class</th><th>Value</th></tr>" << std::endl;
	unsigned int num_attributes = attributes.size();
	for(i = 0; i < num_attributes; i++)
	{
		attributes[i]->to_HTML(os) << std::endl;
	}
	os << "</table>" << std::endl;
	os << "</td>" << std::endl;
	os << "</tr>" << std::endl;
	return os;
}


template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_DIE<MEMORY_ADDR>& dw_die)
{
	unsigned int i;

	os << "   - DIE:" << std::endl;
	os << "     - Offset: " << dw_die.offset;
	unsigned int num_attributes = dw_die.attributes.size();
	if(num_attributes)
	{
		os << std::endl << "     - Attributes:";
		for(i = 0; i < num_attributes; i++)
		{
			os << std::endl << "       - " << *dw_die.attributes[i];
		}
	}
	unsigned int num_children = dw_die.children.size();
	if(num_children)
	{
		os << std::endl << "     - Children DIE:";
		for(i = 0; i < num_children; i++)
		{
			os << std::endl << "  " << *dw_die.children[i];
		}
	}
	return os;
}

template <class MEMORY_ADDR>
DWARF_CompilationUnit<MEMORY_ADDR>::DWARF_CompilationUnit(DWARF_Handler<MEMORY_ADDR> *_dw_handler)
	: dw_handler(_dw_handler)
	, offset(0xffffffffffffffffULL)
	, unit_length(0)
	, version(0)
	, debug_abbrev_offset(0)
	, address_size(0)
{
}

template <class MEMORY_ADDR>
DWARF_CompilationUnit<MEMORY_ADDR>::~DWARF_CompilationUnit()
{
	unsigned int num_debug_info_entries = debug_info_entries.size();
	unsigned int i;
	for(i = 0; i < num_debug_info_entries; i++)
	{
		delete debug_info_entries[i];
	}
}

template <class MEMORY_ADDR>
endian_type DWARF_CompilationUnit<MEMORY_ADDR>::GetEndianness() const
{
	return dw_handler->GetEndianness();
}

template <class MEMORY_ADDR>
DWARF_Format DWARF_CompilationUnit<MEMORY_ADDR>::GetFormat() const
{
	return dw_fmt;
}

template <class MEMORY_ADDR>
uint8_t DWARF_CompilationUnit<MEMORY_ADDR>::GetAddressSize() const
{
	return address_size;
}

template <class MEMORY_ADDR>
uint16_t DWARF_CompilationUnit<MEMORY_ADDR>::GetVersion() const
{
	return version;
}

template <class MEMORY_ADDR>
uint8_t DWARF_CompilationUnit<MEMORY_ADDR>::GetOffsetSize() const
{
	return (version == 3) ? ((dw_fmt == FMT_DWARF64) ? 8 : 4) : address_size;
}

template <class MEMORY_ADDR>
uint64_t DWARF_CompilationUnit<MEMORY_ADDR>::GetOffset() const
{
	return offset;
}

template <class MEMORY_ADDR>
DWARF_Handler<MEMORY_ADDR> *DWARF_CompilationUnit<MEMORY_ADDR>::GetHandler() const
{
	return dw_handler;
}

template <class MEMORY_ADDR>
const char *DWARF_CompilationUnit<MEMORY_ADDR>::GetString(uint64_t debug_str_offset) const
{
	return dw_handler->GetString(debug_str_offset);
}

template <class MEMORY_ADDR>
const DWARF_Abbrev *DWARF_CompilationUnit<MEMORY_ADDR>::FindAbbrev(const DWARF_LEB128& dw_abbrev_code) const
{
	return dw_handler->FindAbbrev(debug_abbrev_offset, dw_abbrev_code);
}

template <class MEMORY_ADDR>
int64_t DWARF_CompilationUnit<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size, uint64_t _offset)
{
	offset = _offset;
	endian_type endianness = dw_handler->GetEndianness();
	uint32_t unit_length32;
	
	uint64_t size = 0;
	if(max_size < sizeof(unit_length32)) return -1;
	memcpy(&unit_length32, rawdata, sizeof(unit_length32));
	unit_length32 = Target2Host(endianness, unit_length32);
	rawdata += sizeof(unit_length32);
	max_size -= sizeof(unit_length32);
	size += sizeof(unit_length32);
	
	dw_fmt = (unit_length32 == 0xffffffffUL) ? FMT_DWARF64 : FMT_DWARF32;
	
	if(dw_fmt == FMT_DWARF64)
	{
		uint64_t unit_length64;
		if(max_size < sizeof(unit_length64)) return -1;
		memcpy(&unit_length64, rawdata, sizeof(unit_length64));
		unit_length64 = Target2Host(endianness, unit_length64);
		rawdata += sizeof(unit_length64);
		max_size -= sizeof(unit_length64);
		size += sizeof(unit_length64);
		unit_length = unit_length64;
	}
	else
	{
		unit_length = unit_length32;
	}
		
	if(max_size < unit_length) return -1;
	
	if(max_size < sizeof(version)) return -1;
	memcpy(&version, rawdata, sizeof(version));
	version = Target2Host(endianness, version);
	rawdata += sizeof(version);
	max_size -= sizeof(version);
	size += sizeof(version);

	if(version != 2 && version != 3) return -1;
	
	if(dw_fmt == FMT_DWARF64)
	{
		uint64_t debug_abbrev_offset64;
	
		if(max_size < sizeof(debug_abbrev_offset64)) return -1;
		memcpy(&debug_abbrev_offset64, rawdata, sizeof(debug_abbrev_offset64));
		debug_abbrev_offset64 = Target2Host(endianness, debug_abbrev_offset64);
		rawdata += sizeof(debug_abbrev_offset64);
		max_size -= sizeof(debug_abbrev_offset64);
		size += sizeof(debug_abbrev_offset64);
		debug_abbrev_offset = debug_abbrev_offset64;
	}
	else
	{
		uint32_t debug_abbrev_offset32;
	
		if(max_size < sizeof(debug_abbrev_offset32)) return -1;
		memcpy(&debug_abbrev_offset32, rawdata, sizeof(debug_abbrev_offset32));
		debug_abbrev_offset32 = Target2Host(endianness, debug_abbrev_offset32);
		rawdata += sizeof(debug_abbrev_offset32);
		max_size -= sizeof(debug_abbrev_offset32);
		size += sizeof(debug_abbrev_offset32);
		debug_abbrev_offset = debug_abbrev_offset32;
	}

	if(max_size < sizeof(address_size)) return -1;
	memcpy(&address_size, rawdata, sizeof(address_size));
	address_size = Target2Host(endianness, address_size);
	rawdata += sizeof(address_size);
	max_size -= sizeof(address_size);
	size += sizeof(address_size);
	
 	do
 	{
		DWARF_DIE<MEMORY_ADDR> *dw_die = new DWARF_DIE<MEMORY_ADDR>(this);
		
		int64_t sz;
		if((sz = dw_die->Load(rawdata, max_size, offset + size)) < 0)
		{
			delete dw_die;
			return -1;
		}
		rawdata += sz;
		max_size -= sz;
		size += sz;
		
		debug_info_entries.push_back(dw_die);
		Register(dw_die);
 	}
 	while(size < ((dw_fmt == FMT_DWARF64) ? (unit_length + sizeof(uint32_t) + sizeof(uint64_t)) : (unit_length + sizeof(uint32_t))));
	return size;
}

template <class MEMORY_ADDR>
void DWARF_CompilationUnit<MEMORY_ADDR>::Register(DWARF_DIE<MEMORY_ADDR> *dw_die)
{
	dw_handler->Register(dw_die);
}

template <class MEMORY_ADDR>
void DWARF_CompilationUnit<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int _id)
{
	id = _id;
}

template <class MEMORY_ADDR>
std::string DWARF_CompilationUnit<MEMORY_ADDR>::GetHREF() const
{
	std::stringstream sstr;
	sstr << "debug_info/cus/" << (id / cus_per_file) << ".html#cu-" << id;
	return sstr.str();
}

template <class MEMORY_ADDR>
unsigned int DWARF_CompilationUnit<MEMORY_ADDR>::GetId() const
{
	return id;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_CompilationUnit<MEMORY_ADDR>::to_XML(std::ostream& os)
{
	os << "<DW_CU offset=\"" << offset
	   << "\" unit_length=\"" << unit_length
	   << "\" version=\"" << version
	   << "\" debug_abbrev_offset=\"" << debug_abbrev_offset
	   << "\" address_size=\"" << (uint32_t) address_size
	   << "\">" << std::endl;
	unsigned int num_debug_info_entries = debug_info_entries.size();
	unsigned int i;
	for(i = 0; i < num_debug_info_entries; i++)
	{
		debug_info_entries[i]->to_XML(os) << std::endl;
	}
	os << "</DW_CU>" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_CompilationUnit<MEMORY_ADDR>::to_HTML(std::ostream& os)
{
	os << "<tr id=\"cu-" << id << "\">" << std::endl;
	os << "<td>" << id << "</td><td>" << offset << "</td><td>" << (uint32_t) version << "</td><td>" << (uint32_t) address_size << "</td>" << std::endl;
	os << "<td>";
	unsigned int num_debug_info_entries = debug_info_entries.size();
	unsigned int i;
	for(i = 0; i < num_debug_info_entries; i++)
	{
		if(i != 0) os << "&nbsp;";
		os << "<a href=\"../../" << debug_info_entries[i]->GetHREF() << "\">die #" << debug_info_entries[i]->GetId() << "</a>";
	}
	os << "</td>" << std::endl;
	os << "</tr>" << std::endl;

	return os;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_CompilationUnit<MEMORY_ADDR>& dw_cu)
{
	os << "Compilation Unit Header:" << std::endl
	   << " - Offset: " << dw_cu.offset << std::endl
	   << " - Unit Length: " << dw_cu.unit_length << std::endl
	   << " - DWARF Version: " << dw_cu.version << std::endl
	   << " - Offset in .debug_abbrev: " << dw_cu.debug_abbrev_offset << std::endl
	   << " - Address Size: " << (uint32_t) dw_cu.address_size << std::endl
	   << " - Debug Info Entries:" << std::endl;
	unsigned int num_debug_info_entries = dw_cu.debug_info_entries.size();
	unsigned int i;
	for(i = 0; i < num_debug_info_entries; i++)
	{
		os << *dw_cu.debug_info_entries[i] << std::endl;
	}
	return os;
}

template <class MEMORY_ADDR>
DWARF_LocationPiece<MEMORY_ADDR>::DWARF_LocationPiece(unsigned int _dw_loc_type)
	: dw_loc_type(_dw_loc_type)
{
}

template <class MEMORY_ADDR>
DWARF_LocationPiece<MEMORY_ADDR>::~DWARF_LocationPiece()
{
}

template <class MEMORY_ADDR>
unsigned int DWARF_LocationPiece<MEMORY_ADDR>::GetType() const
{
	return dw_loc_type;
}

template <class MEMORY_ADDR>
DWARF_MemoryLocationPiece<MEMORY_ADDR>::DWARF_MemoryLocationPiece(MEMORY_ADDR _dw_addr, unsigned int _dw_bit_offset, unsigned int _dw_bit_size)
	: dw_addr(_dw_addr)
	, dw_bit_offset(_dw_bit_offset)
	, dw_bit_size(_dw_bit_size)
{
}

template <class MEMORY_ADDR>
DWARF_MemoryLocationPiece<MEMORY_ADDR>::~DWARF_MemoryLocationPiece()
{
}

template <class MEMORY_ADDR>
MEMORY_ADDR DWARF_MemoryLocationPiece<MEMORY_ADDR>::GetAddress() const
{
	return dw_addr;
}

template <class MEMORY_ADDR>
unsigned int DWARF_MemoryLocationPiece<MEMORY_ADDR>::GetBitOffset() const
{
	return dw_bit_offset;
}

template <class MEMORY_ADDR>
unsigned int DWARF_MemoryLocationPiece<MEMORY_ADDR>::GetBitSize() const
{
	return dw_bit_size;
}

template <class MEMORY_ADDR>
DWARF_RegisterLocationPiece<MEMORY_ADDR>::DWARF_RegisterLocationPiece(unisim::util::debug::Register *_dw_reg, unsigned int _dw_bit_offset, unsigned int _dw_bit_size)
	: dw_reg(_dw_reg)
	, dw_bit_offset(_dw_bit_offset)
	, dw_bit_size(_dw_bit_size)
{
}

template <class MEMORY_ADDR>
DWARF_RegisterLocationPiece<MEMORY_ADDR>::~DWARF_RegisterLocationPiece()
{
}

template <class MEMORY_ADDR>
unsigned int DWARF_RegisterLocationPiece<MEMORY_ADDR>::GetRegisterNumber() const
{
	return dw_reg;
}

template <class MEMORY_ADDR>
unsigned int DWARF_RegisterLocationPiece<MEMORY_ADDR>::GetBitOffset() const
{
	return dw_bit_offset;
}

template <class MEMORY_ADDR>
unsigned int DWARF_RegisterLocationPiece<MEMORY_ADDR>::GetBitSize() const
{
	return dw_bit_size;
}

template <class MEMORY_ADDR>
DWARF_Location<MEMORY_ADDR>::DWARF_Location()
{
}

template <class MEMORY_ADDR>
DWARF_Location<MEMORY_ADDR>::~DWARF_Location()
{
}

template <class MEMORY_ADDR>
void DWARF_Location<MEMORY_ADDR>::Add(DWARF_LocationPiece<MEMORY_ADDR> *dw_loc_piece)
{
	dw_location_pieces.push_back(dw_loc_piece);
}

template <class MEMORY_ADDR>
const std::vector<DWARF_LocationPiece<MEMORY_ADDR> *>& DWARF_Location<MEMORY_ADDR>::GetLocationPieces() const
{
	return dw_location_pieces;
}

template <class MEMORY_ADDR>
DWARF_ExpressionVM<MEMORY_ADDR>::DWARF_ExpressionVM(const DWARF_CompilationUnit<MEMORY_ADDR> *_dw_cu)
	: dw_cu(_dw_cu)
	, dw_cfp(0)
{
	//memcpy(registers, _registers, sizeof(registers));
}

template <class MEMORY_ADDR>
DWARF_ExpressionVM<MEMORY_ADDR>::DWARF_ExpressionVM(const DWARF_CallFrameProgram<MEMORY_ADDR> *_dw_cfp)
	: dw_cu(0)
	, dw_cfp(_dw_cfp)
{
	//memcpy(registers, _registers, sizeof(registers));
}

template <class MEMORY_ADDR>
DWARF_ExpressionVM<MEMORY_ADDR>::~DWARF_ExpressionVM()
{
}


template <class MEMORY_ADDR>
bool DWARF_ExpressionVM<MEMORY_ADDR>::Disasm(std::ostream& os, const DWARF_Expression<MEMORY_ADDR> *dw_expr)
{
	return Run(dw_expr, &os, 0);
}

template <class MEMORY_ADDR>
bool DWARF_ExpressionVM<MEMORY_ADDR>::Execute(const DWARF_Expression<MEMORY_ADDR> *dw_expr, DWARF_Location<MEMORY_ADDR> *dw_location)
{
	return Run(dw_expr, 0, dw_location);
}

template <class MEMORY_ADDR>
bool DWARF_ExpressionVM<MEMORY_ADDR>::Run(const DWARF_Expression<MEMORY_ADDR> *dw_expr, std::ostream *os, DWARF_Location<MEMORY_ADDR> *dw_location)
{
	std::stack<MEMORY_ADDR> dw_stack;
	uint8_t address_size = dw_cu ? dw_cu->GetAddressSize() : dw_cfp->GetAddressSize();
	endian_type endianness = dw_cu ? dw_cu->GetEndianness() : dw_cfp->GetEndianness();
//	uint8_t offset_size = dw_cu ? dw_cu->GetOffsetSize() : 0; // offsets are unused for expressions in a call frame program
	
	uint64_t expr_length = dw_expr->GetLength();
	const uint8_t *expr = dw_expr->GetValue();
	
	uint8_t opcode;
	
	if(expr_length)
	{
		do
		{
			opcode = *expr;
			expr++;
			expr_length--;
			
			switch(opcode)
			{
				case DW_OP_lit0:
				case DW_OP_lit1:
				case DW_OP_lit2:
				case DW_OP_lit3:
				case DW_OP_lit4:
				case DW_OP_lit5:
				case DW_OP_lit6:
				case DW_OP_lit7:
				case DW_OP_lit8:
				case DW_OP_lit9:
				case DW_OP_lit10:
				case DW_OP_lit11:
				case DW_OP_lit12:
				case DW_OP_lit13:
				case DW_OP_lit14:
				case DW_OP_lit15:
				case DW_OP_lit16:
				case DW_OP_lit17:
				case DW_OP_lit18:
				case DW_OP_lit19:
				case DW_OP_lit20:
				case DW_OP_lit21:
				case DW_OP_lit22:
				case DW_OP_lit23:
				case DW_OP_lit24:
				case DW_OP_lit25:
				case DW_OP_lit26:
				case DW_OP_lit27:
				case DW_OP_lit28:
				case DW_OP_lit29:
				case DW_OP_lit30:
				case DW_OP_lit31:
					{
						MEMORY_ADDR literal_value = (opcode - DW_OP_lit0);
						dw_stack.push(literal_value);
						if(os) *os << "DW_OP_lit" << literal_value;
					}
					break;
				case DW_OP_addr:
					{
						MEMORY_ADDR addr;
						if(expr_length < address_size) return false;
						switch(address_size)
						{
							case 1:
								{
									uint8_t addr8;
									memcpy(&addr8, expr, address_size);
									addr8 = Target2Host(endianness, addr8);
									addr = addr8;
								}
								break;
							case 2:
								{
									// *** REDA *** replace uint8_t by uint16_t
									uint16_t addr16;
									memcpy(&addr16, expr, address_size);
									addr16 = Target2Host(endianness, addr16);
									addr = addr16;
								}
								break;
							case 4:
								{
									// *** REDA *** replace uint8_t by uint32_t
									uint32_t addr32;
									memcpy(&addr32, expr, address_size);
									addr32 = Target2Host(endianness, addr32);
									addr = addr32;
								}
								break;
							case 8:
								{
									uint64_t addr64;
									memcpy(&addr64, expr, address_size);
									addr64 = Target2Host(endianness, addr64);
									addr = addr64;
								}
								break;
							default:
								return -1;
						}
						expr += address_size;
						expr_length -= address_size;
						dw_stack.push(addr);
						if(os) *os << "DW_OP_addr 0x" << std::hex << addr << std::dec;
					}
					break;
				case DW_OP_const1u:
					{
						uint8_t dw_const;
						if(expr_length < sizeof(dw_const)) return -1;
						memcpy(&dw_const, expr, sizeof(dw_const));
						dw_const = Target2Host(endianness, dw_const);
						expr += sizeof(dw_const);
						expr_length -= sizeof(dw_const);
						MEMORY_ADDR value = dw_const;
						dw_stack.push(value);
						if(os) *os << "DW_OP_const1u " << (uint32_t) dw_const;
					}
					break;
				case DW_OP_const1s:
					{
						uint8_t dw_const;
						if(expr_length < sizeof(dw_const)) return -1;
						memcpy(&dw_const, expr, sizeof(dw_const));
						dw_const = Target2Host(endianness, dw_const);
						expr += sizeof(dw_const);
						expr_length -= sizeof(dw_const);
						MEMORY_ADDR value = SignExtend((MEMORY_ADDR) dw_const, 8 * sizeof(dw_const));
						dw_stack.push(value);
						if(os) *os << "DW_OP_const1s " << (int64_t)(int8_t) dw_const;
					}
					break;
				case DW_OP_const2u:
					{
						uint16_t dw_const;
						if(expr_length < sizeof(dw_const)) return -1;
						memcpy(&dw_const, expr, sizeof(dw_const));
						dw_const = Target2Host(endianness, dw_const);
						expr += sizeof(dw_const);
						expr_length -= sizeof(dw_const);
						MEMORY_ADDR value = dw_const;
						dw_stack.push(value);
						if(os) *os << "DW_OP_const2u " << (uint32_t) dw_const;
					}
					break;
				case DW_OP_const2s:
					{
						uint16_t dw_const;
						if(expr_length < sizeof(dw_const)) return -1;
						memcpy(&dw_const, expr, sizeof(dw_const));
						dw_const = Target2Host(endianness, dw_const);
						expr += sizeof(dw_const);
						expr_length -= sizeof(dw_const);
						MEMORY_ADDR value = SignExtend((MEMORY_ADDR) dw_const, 8 * sizeof(dw_const));
						dw_stack.push(value);
						if(os) *os << "DW_OP_const2s " << (int64_t)(int16_t) dw_const;
					}
					break;
				case DW_OP_const4u:
					{
						uint32_t dw_const;
						if(expr_length < sizeof(dw_const)) return -1;
						memcpy(&dw_const, expr, sizeof(dw_const));
						dw_const = Target2Host(endianness, dw_const);
						expr += sizeof(dw_const);
						expr_length -= sizeof(dw_const);
						MEMORY_ADDR value = dw_const;
						dw_stack.push(value);
						if(os) *os << "DW_OP_const4u " << dw_const;
					}
					break;
				case DW_OP_const4s:
					{
						uint32_t dw_const;
						if(expr_length < sizeof(dw_const)) return -1;
						memcpy(&dw_const, expr, sizeof(dw_const));
						dw_const = Target2Host(endianness, dw_const);
						expr += sizeof(dw_const);
						expr_length -= sizeof(dw_const);
						MEMORY_ADDR value = SignExtend((MEMORY_ADDR) dw_const, 8 * sizeof(dw_const));
						dw_stack.push(value);
						if(os) *os << "DW_OP_const4s " << (int64_t)(int32_t) dw_const;
					}
					break;
				case DW_OP_const8u:
					{
						uint64_t dw_const;
						if(expr_length < sizeof(dw_const)) return -1;
						memcpy(&dw_const, expr, sizeof(dw_const));
						dw_const = Target2Host(endianness, dw_const);
						expr += sizeof(dw_const);
						expr_length -= sizeof(dw_const);
						MEMORY_ADDR value = dw_const;
						dw_stack.push(value);
						if(os) *os << "DW_OP_const8u " << dw_const;
					}
					break;
				case DW_OP_const8s:
					{
						uint64_t dw_const;
						if(expr_length < sizeof(dw_const)) return -1;
						memcpy(&dw_const, expr, sizeof(dw_const));
						dw_const = Target2Host(endianness, dw_const);
						expr += sizeof(dw_const);
						expr_length -= sizeof(dw_const);
						MEMORY_ADDR value = SignExtend((MEMORY_ADDR) dw_const, 8 * sizeof(dw_const));
						dw_stack.push(value);
						if(os) *os << "DW_OP_const8s " << (int64_t) dw_const;
					}
					break;
				case DW_OP_constu:
					{
						DWARF_LEB128 dw_const_leb128;
						int64_t sz;
						if((sz = dw_const_leb128.Load(expr, expr_length)) < 0) return -1;
						expr += sz;
						expr_length -= sz;
						
						uint64_t value = (uint64_t) dw_const_leb128;
						MEMORY_ADDR addr = value;
						dw_stack.push(addr);
						if(os) *os << "DW_OP_constu " << value;
					}
					break;
				case DW_OP_consts:
					{
						DWARF_LEB128 dw_const_leb128;
						int64_t sz;
						if((sz = dw_const_leb128.Load(expr, expr_length)) < 0) return -1;
						expr += sz;
						expr_length -= sz;
						
						int64_t value = (int64_t) dw_const_leb128;
						MEMORY_ADDR addr = value;
						dw_stack.push(addr);
						if(os) *os << "DW_OP_consts " << value;
					}
					break;
				case DW_OP_fbreg:
					{
						DWARF_LEB128 dw_offset_leb128;
						int64_t sz;
						if((sz = dw_offset_leb128.Load(expr, expr_length)) < 0) return -1;
						expr += sz;
						expr_length -= sz;
						
						int64_t offset = (int64_t) dw_offset_leb128;
						// TODO: push onto the stack frame pointer + offset
						if(os) *os << "DW_OP_fbreg " << offset;
					}
					break;
				case DW_OP_breg0:
				case DW_OP_breg1:
				case DW_OP_breg2:
				case DW_OP_breg3:
				case DW_OP_breg4:
				case DW_OP_breg5:
				case DW_OP_breg6:
				case DW_OP_breg7:
				case DW_OP_breg8:
				case DW_OP_breg9:
				case DW_OP_breg10:
				case DW_OP_breg11:
				case DW_OP_breg12:
				case DW_OP_breg13:
				case DW_OP_breg14:
				case DW_OP_breg15:
				case DW_OP_breg16:
				case DW_OP_breg17:
				case DW_OP_breg18:
				case DW_OP_breg19:
				case DW_OP_breg20:
				case DW_OP_breg21:
				case DW_OP_breg22:
				case DW_OP_breg23:
				case DW_OP_breg24:
				case DW_OP_breg25:
				case DW_OP_breg26:
				case DW_OP_breg27:
				case DW_OP_breg28:
				case DW_OP_breg29:
				case DW_OP_breg30:
				case DW_OP_breg31:
					{
						uint8_t reg_num = opcode - DW_OP_breg0;
						DWARF_LEB128 dw_offset_leb128;
						int64_t sz;
						if((sz = dw_offset_leb128.Load(expr, expr_length)) < 0) return -1;
						expr += sz;
						expr_length -= sz;
						
						int64_t offset = (int64_t) dw_offset_leb128;
						// TODO: push onto the stack reg value + offset
						if(os) *os << "DW_OP_breg" << (unsigned int) reg_num << " " << offset;
					}
					break;
				case DW_OP_dup:
					if(os) *os << "DW_OP_dup";
					break;
				case DW_OP_drop:
					if(os) *os << "DW_OP_drop";
					break;
				case DW_OP_pick:
					{
						uint8_t dw_index;
						if(expr_length < sizeof(dw_index)) return -1;
						memcpy(&dw_index, expr, sizeof(dw_index));
						dw_index = Target2Host(endianness, dw_index);
						expr += sizeof(dw_index);
						expr_length -= sizeof(dw_index);
						if(os) *os << "DW_OP_pick " << (uint32_t) dw_index;
					}
					break;
				case DW_OP_over:
					if(os) *os << "DW_OP_over";
					break;
				case DW_OP_swap:
					if(os) *os << "DW_OP_swap";
					break;
				case DW_OP_rot:
					if(os) *os << "DW_OP_rot";
					break;
				case DW_OP_deref:
					if(os) *os << "DW_OP_deref";
					break;
				case DW_OP_deref_size:
					{
						uint8_t dw_size;
						if(expr_length < sizeof(dw_size)) return -1;
						memcpy(&dw_size, expr, sizeof(dw_size));
						dw_size = Target2Host(endianness, dw_size);
						expr += sizeof(dw_size);
						expr_length -= sizeof(dw_size);
						if(os) *os << "DW_OP_deref_size " << (uint32_t) dw_size;
					}
					break;
				case DW_OP_xderef:
					if(os) *os << "DW_OP_xderef";
					break;
				case DW_OP_xderef_size:
					{
						uint8_t dw_size;
						if(expr_length < sizeof(dw_size)) return -1;
						memcpy(&dw_size, expr, sizeof(dw_size));
						dw_size = Target2Host(endianness, dw_size);
						expr += sizeof(dw_size);
						expr_length -= sizeof(dw_size);
						if(os) *os << "DW_OP_xderef_size " << (uint32_t) dw_size;
					}
					break;
				case DW_OP_push_object_address:
					if(!dw_cu) return false;
					if(os) *os << "DW_OP_push_object_address";
					break;
				case DW_OP_form_tls_address:
					if(os) *os << "DW_OP_form_tls_address";
					break;
				case DW_OP_call_frame_cfa:
					if(os) *os << "DW_OP_call_frame_cfa";
					break;
				case DW_OP_abs:
					if(os) *os << "DW_OP_abs";
					break;
				case DW_OP_and:
					if(os) *os << "DW_OP_and";
					break;
				case DW_OP_div:
					if(os) *os << "DW_OP_div";
					break;
				case DW_OP_minus:
					if(os) *os << "DW_OP_minus";
					break;
				case DW_OP_mod:
					if(os) *os << "DW_OP_mod";
					break;
				case DW_OP_mul:
					if(os) *os << "DW_OP_mul";
					break;
				case DW_OP_neg:
					if(os) *os << "DW_OP_neg";
					break;
				case DW_OP_not:
					if(os) *os << "DW_OP_not";
					break;
				case DW_OP_or:
					if(os) *os << "DW_OP_or";
					break;
				case DW_OP_plus:
					if(os) *os << "DW_OP_plus";
					break;
				case DW_OP_plus_uconst:
					{
						DWARF_LEB128 dw_uconst_leb128;
						int64_t sz;
						if((sz = dw_uconst_leb128.Load(expr, expr_length)) < 0) return -1;
						expr += sz;
						expr_length -= sz;
						
						uint64_t dw_uconst = (uint64_t) dw_uconst_leb128;
						if(os) *os << "DW_OP_plus_uconst " << dw_uconst;
					}
					break;
				case DW_OP_shl:
					if(os) *os << "DW_OP_shl";
					break;
				case DW_OP_shr:
					if(os) *os << "DW_OP_shr";
					break;
				case DW_OP_shra:
					if(os) *os << "DW_OP_shra";
					break;
				case DW_OP_xor:
					if(os) *os << "DW_OP_xor";
					break;
				case DW_OP_le:
					if(os) *os << "DW_OP_le";
					break;
				case DW_OP_ge:
					if(os) *os << "DW_OP_ge";
					break;
				case DW_OP_eq:
					if(os) *os << "DW_OP_eq";
					break;
				case DW_OP_lt:
					if(os) *os << "DW_OP_lt";
					break;
				case DW_OP_gt:
					if(os) *os << "DW_OP_gt";
					break;
				case DW_OP_ne:
					if(os) *os << "DW_OP_ne";
					break;
				case DW_OP_skip:
					{
						int16_t dw_skip_amount;
						if(expr_length < sizeof(dw_skip_amount)) return -1;
						memcpy(&dw_skip_amount, expr, sizeof(dw_skip_amount));
						dw_skip_amount = Target2Host(endianness, dw_skip_amount);
						expr += sizeof(dw_skip_amount);
						expr_length -= sizeof(dw_skip_amount);
						if(os) *os << "DW_OP_skip " << dw_skip_amount;
					}
					break;
				case DW_OP_bra:
					{
						int16_t dw_skip_amount;
						if(expr_length < sizeof(dw_skip_amount)) return -1;
						memcpy(&dw_skip_amount, expr, sizeof(dw_skip_amount));
						dw_skip_amount = Target2Host(endianness, dw_skip_amount);
						expr += sizeof(dw_skip_amount);
						expr_length -= sizeof(dw_skip_amount);
						if(os) *os << "DW_OP_bra " << dw_skip_amount;
					}
					break;
				case DW_OP_call2:
					{
						if(!dw_cu) return false;
						uint16_t cu_offset;
						if(expr_length < sizeof(cu_offset)) return -1;
						memcpy(&cu_offset, expr, sizeof(cu_offset));
						cu_offset = Target2Host(endianness, cu_offset);
						expr += sizeof(cu_offset);
						expr_length -= sizeof(cu_offset);
						uint64_t debug_info_offset = dw_cu->GetOffset() + cu_offset;
						if(os) *os << "DW_OP_call2 " << debug_info_offset;
					}
					break;
				case DW_OP_call4:
					{
						if(!dw_cu) return false;
						uint32_t cu_offset;
						if(expr_length < sizeof(cu_offset)) return -1;
						memcpy(&cu_offset, expr, sizeof(cu_offset));
						cu_offset = Target2Host(endianness, cu_offset);
						expr += sizeof(cu_offset);
						expr_length -= sizeof(cu_offset);
						uint64_t debug_info_offset = dw_cu->GetOffset() + cu_offset;
						if(os) *os << "DW_OP_call4 " << debug_info_offset;
					}
					break;
				case DW_OP_call_ref:
					{
						if(!dw_cu) return false;
						uint64_t debug_info_offset;
						switch(dw_cu->GetOffsetSize())
						{
							case 2:
								{
									uint16_t debug_info_offset16;
								
									if(expr_length < sizeof(debug_info_offset16)) return -1;
									memcpy(&debug_info_offset16, expr, sizeof(debug_info_offset16));
									debug_info_offset16 = Target2Host(endianness, debug_info_offset16);
									expr += sizeof(debug_info_offset16);
									expr_length -= sizeof(debug_info_offset16);
									debug_info_offset = debug_info_offset16;
								}
								break;
							case 4:
								{
									uint32_t debug_info_offset32;
								
									if(expr_length < sizeof(debug_info_offset32)) return -1;
									memcpy(&debug_info_offset32, expr, sizeof(debug_info_offset32));
									debug_info_offset32 = Target2Host(endianness, debug_info_offset32);
									expr += sizeof(debug_info_offset32);
									expr_length -= sizeof(debug_info_offset32);
									debug_info_offset = debug_info_offset32;
								}
								break;
							case 8:
								{
									uint64_t debug_info_offset64;
								
									if(expr_length < sizeof(debug_info_offset64)) return -1;
									memcpy(&debug_info_offset, expr, sizeof(debug_info_offset64));
									debug_info_offset64 = Target2Host(endianness, debug_info_offset64);
									expr += sizeof(debug_info_offset64);
									expr_length -= sizeof(debug_info_offset64);
									debug_info_offset = debug_info_offset64;
								}
								break;
							default:
								return -1;
						}
						if(os) *os << "DW_OP_call_ref " << debug_info_offset;
					}
					break;
				case DW_OP_nop:
					if(os) *os << "DW_OP_nop";
					break;
				case DW_OP_reg0:
				case DW_OP_reg1:
				case DW_OP_reg2:
				case DW_OP_reg3:
				case DW_OP_reg4:
				case DW_OP_reg5:
				case DW_OP_reg6:
				case DW_OP_reg7:
				case DW_OP_reg8:
				case DW_OP_reg9:
				case DW_OP_reg10:
				case DW_OP_reg11:
				case DW_OP_reg12:
				case DW_OP_reg13:
				case DW_OP_reg14:
				case DW_OP_reg15:
				case DW_OP_reg16:
				case DW_OP_reg17:
				case DW_OP_reg18:
				case DW_OP_reg19:
				case DW_OP_reg20:
				case DW_OP_reg21:
				case DW_OP_reg22:
				case DW_OP_reg23:
				case DW_OP_reg24:
				case DW_OP_reg25:
				case DW_OP_reg26:
				case DW_OP_reg27:
				case DW_OP_reg28:
				case DW_OP_reg29:
				case DW_OP_reg30:
				case DW_OP_reg31:
					{
						uint8_t reg_num = opcode - DW_OP_reg0;
						if(os) *os << "DW_OP_reg" << (unsigned int) reg_num;
					}
					break;
				case DW_OP_regx:
					{
						DWARF_LEB128 dw_reg_num_leb128;
						int64_t sz;
						if((sz = dw_reg_num_leb128.Load(expr, expr_length)) < 0) return -1;
						expr += sz;
						expr_length -= sz;
						
						unsigned int dw_reg_num = (unsigned int) dw_reg_num_leb128;
						if(os) *os << "DW_OP_regx " << dw_reg_num;
					}
					break;
				case DW_OP_piece:
					{
						DWARF_LEB128 dw_byte_size_leb128;
						int64_t sz;
						if((sz = dw_byte_size_leb128.Load(expr, expr_length)) < 0) return -1;
						expr += sz;
						expr_length -= sz;
						
						uint64_t dw_byte_size = (uint64_t) dw_byte_size_leb128;
						if(os) *os << "DW_OP_piece " << dw_byte_size;
					}
					break;
				case DW_OP_bit_piece:
					{
						DWARF_LEB128 dw_bit_size_leb128;
						int64_t sz;
						if((sz = dw_bit_size_leb128.Load(expr, expr_length)) < 0) return -1;
						expr += sz;
						expr_length -= sz;
						
						uint64_t dw_bit_size = (uint64_t) dw_bit_size_leb128;
						if(os) *os << "DW_OP_bit_piece " << dw_bit_size;
					}
					break;
				default:
					return false;
			}
			
			if(os && expr_length)
			{
				*os << "; "; // add a separator
			}
		}
		while(expr_length);
	}
	
	return true;
}

template <class MEMORY_ADDR>
DWARF_CallFrameProgram<MEMORY_ADDR>::DWARF_CallFrameProgram(DWARF_Handler<MEMORY_ADDR> *_dw_handler, uint64_t _length, const uint8_t *_program)
	: dw_handler(_dw_handler)
	, length(_length)
	, program(_program)
{
}

template <class MEMORY_ADDR>
DWARF_CallFrameProgram<MEMORY_ADDR>::~DWARF_CallFrameProgram()
{
}

template <class MEMORY_ADDR>
endian_type DWARF_CallFrameProgram<MEMORY_ADDR>::GetEndianness() const
{
	return dw_handler->GetEndianness();
}

template <class MEMORY_ADDR>
uint8_t DWARF_CallFrameProgram<MEMORY_ADDR>::GetAddressSize() const
{
	return dw_handler->GetAddressSize();
}


template <class MEMORY_ADDR>
uint64_t DWARF_CallFrameProgram<MEMORY_ADDR>::GetLength() const
{
	return length;
}

template <class MEMORY_ADDR>
const uint8_t *DWARF_CallFrameProgram<MEMORY_ADDR>::GetProgram() const
{
	return program;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_CallFrameProgram<MEMORY_ADDR>& dw_call_frame_prog)
{
	DWARF_CallFrameVM<MEMORY_ADDR> dw_call_frame_vm;
	if(!dw_call_frame_vm.Disasm(os, dw_call_frame_prog))
	{
		os << "Invalid opcode";
	}
	return os;
}

template <class MEMORY_ADDR>
DWARF_CIE<MEMORY_ADDR>::DWARF_CIE(DWARF_Handler<MEMORY_ADDR> *_dw_handler)
	: dw_handler(_dw_handler)
	, dw_initial_call_frame_prog(0)
{
}

template <class MEMORY_ADDR>
DWARF_CIE<MEMORY_ADDR>::~DWARF_CIE()
{
	if(dw_initial_call_frame_prog)
	{
		delete dw_initial_call_frame_prog;
	}
}

template <class MEMORY_ADDR>
int64_t DWARF_CIE<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size, uint64_t _offset)
{
	offset = _offset;
	endian_type endianness = dw_handler->GetEndianness();
	uint32_t length32;
	
	uint64_t size = 0;
	int64_t sz;
	if(max_size < sizeof(length32)) return -1;
	memcpy(&length32, rawdata, sizeof(length32));
	length32 = Target2Host(endianness, length32);
	rawdata += sizeof(length32);
	max_size -= sizeof(length32);
	size += sizeof(length32);
	
	dw_fmt = (length32 == 0xffffffffUL) ? FMT_DWARF64 : FMT_DWARF32;
	
	if(dw_fmt == FMT_DWARF64)
	{
		uint64_t length64;
		if(max_size < sizeof(length64)) return -1;
		memcpy(&length64, rawdata, sizeof(length64));
		length64 = Target2Host(endianness, length64);
		rawdata += sizeof(length64);
		max_size -= sizeof(length64);
		size += sizeof(length64);
		length = length64;
	}
	else
	{
		length = length32;
	}
	
	if(dw_fmt == FMT_DWARF64)
	{
		uint64_t cie_id64;
		if(max_size < sizeof(cie_id64)) return -1;
		memcpy(&cie_id64, rawdata, sizeof(cie_id64));
		cie_id64 = Target2Host(endianness, cie_id64);
		rawdata += sizeof(cie_id64);
		max_size -= sizeof(cie_id64);
		size += sizeof(cie_id64);
		if(cie_id64 != 0xffffffffffffffffULL) return -1;
		cie_id = cie_id64;
	}
	else
	{
		uint32_t cie_id32;
		if(max_size < sizeof(cie_id32)) return -1;
		memcpy(&cie_id32, rawdata, sizeof(cie_id32));
		cie_id32 = Target2Host(endianness, cie_id32);
		rawdata += sizeof(cie_id32);
		max_size -= sizeof(cie_id32);
		size += sizeof(cie_id32);
		if(cie_id32 != 0xffffffffULL) return -1;
		cie_id = cie_id32;
	}

	if(max_size < sizeof(version)) return -1;
	memcpy(&version, rawdata, sizeof(version));
	version = Target2Host(endianness, version);
	rawdata += sizeof(version);
	max_size -= sizeof(version);
	size += sizeof(version);

	if((version != 1) && (version != 3)) return -1;
	
	augmentation = (const char *) rawdata;
	unsigned int augmentation_length = strlen(augmentation);
	if(max_size < (augmentation_length + 1)) return -1;
	size += augmentation_length + 1;
	rawdata += augmentation_length + 1;
	max_size -= augmentation_length + 1;
	
	if((sz = code_alignment_factor.Load(rawdata, max_size)) < 0) return -1;
	rawdata += sz;
	max_size -= sz;
	size += sz;
	
	if((sz = data_alignment_factor.Load(rawdata, max_size)) < 0) return -1;
	rawdata += sz;
	max_size -= sz;
	size += sz;
	
	if(version == 1)
	{
		// DWARF v2
		if(max_size < sizeof(dw2_return_address_register)) return -1;
		memcpy(&dw2_return_address_register, rawdata, sizeof(dw2_return_address_register));
		dw2_return_address_register = Target2Host(endianness, dw2_return_address_register);
		rawdata += sizeof(dw2_return_address_register);
		max_size -= sizeof(dw2_return_address_register);
		size += sizeof(dw2_return_address_register);
	}
	else
	{
		// DWARF v3
		if((sz = dw3_return_address_register.Load(rawdata, max_size)) < 0) return -1;
		rawdata += sz;
		max_size -= sz;
		size += sz;
	}
	
	uint64_t initial_instructions_length;
	if(dw_fmt == FMT_DWARF64)
	{
		if(size > length + sizeof(uint32_t) + sizeof(uint64_t)) return -1;
		initial_instructions_length = length + sizeof(uint32_t) + sizeof(uint64_t) - size; 
	}
	else
	{
		if(size > length + sizeof(uint32_t)) return -1;
		initial_instructions_length = length + sizeof(uint32_t) - size; 
	}
	
	if(initial_instructions_length > max_size) return -1;
	dw_initial_call_frame_prog = new DWARF_CallFrameProgram<MEMORY_ADDR>(dw_handler, initial_instructions_length, rawdata);
	size += initial_instructions_length;

	return size;
}

template <class MEMORY_ADDR>
void DWARF_CIE<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int _id)
{
	id = _id;
}

template <class MEMORY_ADDR>
std::string DWARF_CIE<MEMORY_ADDR>::GetHREF() const
{
	std::stringstream sstr;
	sstr << "debug_frame/cies/" << (id / cies_per_file) << ".html#cie-" << id;
	return sstr.str();
}

template <class MEMORY_ADDR>
unsigned int DWARF_CIE<MEMORY_ADDR>::GetId() const
{
	return id;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_CIE<MEMORY_ADDR>::to_XML(std::ostream& os) const
{
	os << "<DW_CIE offset=\"" << offset << "\" version=\"" << (uint32_t) version << "\" augmentation=\"";
	c_string_to_XML(os, augmentation);
	os << "\" code_alignment_factor=\"" << code_alignment_factor.to_string(false) << "\" data_alignment_factor=\"" << data_alignment_factor.to_string(true) << "\" return_address_register=\"";
	if(version == 1)
	{
		os << (uint32_t) dw2_return_address_register;
	}
	else
	{
		os << dw3_return_address_register.to_string(false);
	}
	os << "\" initial_call_frame_program=\"";
	std::stringstream sstr;
	sstr << *dw_initial_call_frame_prog;
	c_string_to_XML(os, sstr.str().c_str());
	os << "\"/>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_CIE<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	os << "<tr id=\"cie-" << id << "\">" << std::endl;
	os << "<td>" << id << "</td><td>" << offset << "</td><td>" << (uint32_t) version << "</td><td>";
	c_string_to_HTML(os, augmentation);
	os << "</td><td>" << code_alignment_factor.to_string(false) << "</td><td>" << data_alignment_factor.to_string(true) << "</td><td>";
	if(version == 1)
	{
		os << (uint32_t) dw2_return_address_register;
	}
	else
	{
		os << dw3_return_address_register.to_string(false);
	}
	os << "</td><td>";
	std::stringstream sstr;
	sstr << *dw_initial_call_frame_prog;
	c_string_to_HTML(os, sstr.str().c_str());
	os << "</td></tr>" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_CIE<MEMORY_ADDR>& dw_cie)
{
	os << "Common Information Entry:" << std::endl;
	os << " - Offset: " << dw_cie.offset << std::endl;
	os << " - Version: " << (uint32_t) dw_cie.version << std::endl;          // Independent of DWARF version number. DWARF v2/version=1; DWARF v3/version=3
	os << " - Augmentation: " << dw_cie.augmentation << std::endl;
	os << " - Code alignment factor: " << dw_cie.code_alignment_factor.to_string(false) << std::endl;
	os << " - Data alignment factor: " << dw_cie.data_alignment_factor.to_string(true) << std::endl;
	os << " - Return address register: ";
	if(dw_cie.version == 1)
	{
		os << (uint32_t) dw_cie.dw2_return_address_register;
	}
	else
	{
		os << dw_cie.dw3_return_address_register.to_string(false);
	}
	os << std::endl;
	os << " - Initial instructions (" << dw_cie.dw_initial_call_frame_prog->GetLength() << " bytes): " << *dw_cie.dw_initial_call_frame_prog << std::endl;
	return os;
}

template <class MEMORY_ADDR>
DWARF_FDE<MEMORY_ADDR>::DWARF_FDE(DWARF_Handler<MEMORY_ADDR> *_dw_handler)
	: dw_handler(_dw_handler)
	, dw_call_frame_prog(0)
{
}

template <class MEMORY_ADDR>
DWARF_FDE<MEMORY_ADDR>::~DWARF_FDE()
{
	if(dw_call_frame_prog)
	{
		delete dw_call_frame_prog;
	}
}

template <class MEMORY_ADDR>
int64_t DWARF_FDE<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size, uint64_t _offset)
{
	offset = _offset;
	endian_type endianness = dw_handler->GetEndianness();
	uint8_t address_size = dw_handler->GetAddressSize();
	uint32_t length32;
	
	uint64_t size = 0;
	if(max_size < sizeof(length32)) return -1;
	memcpy(&length32, rawdata, sizeof(length32));
	length32 = Target2Host(endianness, length32);
	rawdata += sizeof(length32);
	max_size -= sizeof(length32);
	size += sizeof(length32);
	
	dw_fmt = (length32 == 0xffffffffUL) ? FMT_DWARF64 : FMT_DWARF32;
	
	if(dw_fmt == FMT_DWARF64)
	{
		uint64_t length64;
		if(max_size < sizeof(length64)) return -1;
		memcpy(&length64, rawdata, sizeof(length64));
		length64 = Target2Host(endianness, length64);
		rawdata += sizeof(length64);
		max_size -= sizeof(length64);
		size += sizeof(length64);
		length = length64;
	}
	else
	{
		length = length32;
	}
	
	if(dw_fmt == FMT_DWARF64)
	{
		uint64_t cie_pointer64;
		if(max_size < sizeof(cie_pointer64)) return -1;
		memcpy(&cie_pointer64, rawdata, sizeof(cie_pointer64));
		cie_pointer64 = Target2Host(endianness, cie_pointer64);
		rawdata += sizeof(cie_pointer64);
		max_size -= sizeof(cie_pointer64);
		size += sizeof(cie_pointer64);
		if(cie_pointer64 == 0xffffffffffffffffULL) return -1;
		cie_pointer = cie_pointer64;
	}
	else
	{
		uint32_t cie_pointer32;
		if(max_size < sizeof(cie_pointer32)) return -1;
		memcpy(&cie_pointer32, rawdata, sizeof(cie_pointer32));
		cie_pointer32 = Target2Host(endianness, cie_pointer32);
		rawdata += sizeof(cie_pointer32);
		max_size -= sizeof(cie_pointer32);
		size += sizeof(cie_pointer32);
		if(cie_pointer32 == 0xffffffffULL) return -1;
		cie_pointer = cie_pointer32;
	}

	switch(address_size)
	{
		case sizeof(uint16_t):
			{
				if(max_size < sizeof(uint16_t)) return -1;
				uint16_t value;
				memcpy(&value, rawdata, sizeof(uint16_t));
				initial_location = Target2Host(endianness, value);
				size += sizeof(uint16_t);
				rawdata += sizeof(uint16_t);
				max_size -= sizeof(uint16_t);
			}
			break;
		case sizeof(uint32_t):
			{
				if(max_size < sizeof(uint32_t)) return -1;
				uint32_t value;
				memcpy(&value, rawdata, sizeof(uint32_t));
				initial_location = Target2Host(endianness, value);
				size += sizeof(uint32_t);
				rawdata += sizeof(uint32_t);
				max_size -= sizeof(uint32_t);
			}
			break;
		case sizeof(uint64_t):
			{
				if(max_size < sizeof(uint64_t)) return -1;
				uint64_t value;
				memcpy(&value, rawdata, sizeof(uint64_t));
				initial_location = Target2Host(endianness, value);
				size += sizeof(uint64_t);
				rawdata += sizeof(uint64_t);
				max_size -= sizeof(uint64_t);
			}
			break;
		default:
			return -1;
	}

	switch(address_size)
	{
		case sizeof(uint16_t):
			{
				if(max_size < sizeof(uint16_t)) return -1;
				uint16_t value;
				memcpy(&value, rawdata, sizeof(uint16_t));
				address_range = Target2Host(endianness, value);
				size += sizeof(uint16_t);
				rawdata += sizeof(uint16_t);
				max_size -= sizeof(uint16_t);
			}
			break;
		case sizeof(uint32_t):
			{
				if(max_size < sizeof(uint32_t)) return -1;
				uint32_t value;
				memcpy(&value, rawdata, sizeof(uint32_t));
				address_range = Target2Host(endianness, value);
				size += sizeof(uint32_t);
				rawdata += sizeof(uint32_t);
				max_size -= sizeof(uint32_t);
			}
			break;
		case sizeof(uint64_t):
			{
				if(max_size < sizeof(uint64_t)) return -1;
				uint64_t value;
				memcpy(&value, rawdata, sizeof(uint64_t));
				address_range = Target2Host(endianness, value);
				size += sizeof(uint64_t);
				rawdata += sizeof(uint64_t);
				max_size -= sizeof(uint64_t);
			}
			break;
		default:
			return -1;
	}
	
	uint64_t instructions_length;
	if(dw_fmt == FMT_DWARF64)
	{
		if(size > length + sizeof(uint32_t) + sizeof(uint64_t)) return -1;
		instructions_length = length + sizeof(uint32_t) + sizeof(uint64_t) - size; 
	}
	else
	{
		if(size > length + sizeof(uint32_t)) return -1;
		instructions_length = length + sizeof(uint32_t) - size; 
	}
	
	if(instructions_length > max_size) return -1;
	dw_call_frame_prog = new DWARF_CallFrameProgram<MEMORY_ADDR>(dw_handler, instructions_length, rawdata);
	size += instructions_length;

	return size;
}

template <class MEMORY_ADDR>
void DWARF_FDE<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler)
{
	dw_cie = dw_handler->FindCIE(cie_pointer);
	if(!dw_cie)
	{
		std::cerr << "Can't find CIE at offset " << cie_pointer << std::endl;
	}
}

// template <class MEMORY_ADDR>
// std::string DWARF_FDE<MEMORY_ADDR>::GetHREF() const
// {
// 	std::stringstream sstr;
// 	sstr << "debug_frame/fdes/" << (id / cies_per_file) << ".html#fde-" << id;
// 	return sstr.str();
// }
// 
// template <class MEMORY_ADDR>
// unsigned int DWARF_FDE<MEMORY_ADDR>::GetId() const
// {
// 	return id;
// }

template <class MEMORY_ADDR>
std::ostream& DWARF_FDE<MEMORY_ADDR>::to_XML(std::ostream& os) const
{
	os << "<DW_FDE offset=\"" << offset << "\" length=\"" << length << "\" cie_pointer=\"" << cie_pointer << "\" initial_location=\"0x" << std::hex << initial_location << std::dec << "\" address_range=\"0x" << std::hex << address_range << std::dec << "\" call_frame_program=\"";
	std::stringstream sstr;
	sstr << *dw_call_frame_prog;
	c_string_to_XML(os, sstr.str().c_str());
	os << "\"/>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_FDE<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	os << "<tr>" << std::endl;
	os << "<td>" << offset << "</td><td><a href=\"../../" << dw_cie->GetHREF() << "\">cie-" << dw_cie->GetId() << "</a></td>";
	os << "<td>0x" << std::hex << initial_location << std::dec << "</td><td>0x" << std::hex << address_range << std::dec << "</td><td>";
	std::stringstream sstr;
	sstr << *dw_call_frame_prog;
	c_string_to_HTML(os, sstr.str().c_str());
	os << "</td></tr>" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_FDE<MEMORY_ADDR>& dw_fde)
{
	os << "Frame Description Entry:" << std::endl;
	os << " - Offset: " << dw_fde.offset << std::endl;
	os << " - Length: " << dw_fde.length << std::endl;
	os << " - Pointer to CIE: " << dw_fde.cie_pointer << std::endl;
	os << " - Initial location: 0x" << std::hex <<  dw_fde.initial_location << std::dec << std::endl;
	os << " - Address range: 0x" << std::hex <<  dw_fde.address_range << std::dec << std::endl;
	os << " - Instructions (" << dw_fde.dw_call_frame_prog->GetLength()  << " bytes): " << *dw_fde.dw_call_frame_prog << std::endl;
	return os;
}

template <class MEMORY_ADDR>
DWARF_CallFrameVM<MEMORY_ADDR>::DWARF_CallFrameVM()
{
}

template <class MEMORY_ADDR>
DWARF_CallFrameVM<MEMORY_ADDR>::~DWARF_CallFrameVM()
{
}

template <class MEMORY_ADDR>
bool DWARF_CallFrameVM<MEMORY_ADDR>::Disasm(std::ostream& os, const DWARF_CallFrameProgram<MEMORY_ADDR>& dw_call_frame_prog)
{
	return Run(dw_call_frame_prog, &os, 0);
}

template <class MEMORY_ADDR>
bool DWARF_CallFrameVM<MEMORY_ADDR>::Execute(const DWARF_CallFrameProgram<MEMORY_ADDR>& dw_call_frame_prog, std::map<MEMORY_ADDR, RuleMatrixRow<MEMORY_ADDR> *>& rule_matrix)
{
	return Run(dw_call_frame_prog, 0, &rule_matrix);
}

template <class MEMORY_ADDR>
bool DWARF_CallFrameVM<MEMORY_ADDR>::Run(const DWARF_CallFrameProgram<MEMORY_ADDR>& dw_call_frame_prog, std::ostream *os, std::map<MEMORY_ADDR, RuleMatrixRow<MEMORY_ADDR> *> *rule_matrix)
{
	uint64_t program_length = dw_call_frame_prog.length;
	endian_type endianness = dw_call_frame_prog.GetEndianness();
	uint8_t address_size = dw_call_frame_prog.GetAddressSize();
	const uint8_t *program = dw_call_frame_prog.program;
	
	if(program_length)
	{
		do
		{
			uint8_t opcode = *program;
			program_length--;
			
			switch(opcode & 0xc0) // high 2 bits
			{
				case DW_CFA_advance_loc:
					{
						uint8_t delta = opcode & 0x3f;
						
						if(os) *os << "DW_CFA_advance_loc " << (uint32_t) delta;
					}
					break;
				case DW_CFA_offset:
					{
						uint8_t reg_num = opcode & 0x3f;
						DWARF_LEB128 offset;
						int64_t sz;
						
						if((sz = offset.Load(program, program_length)) < 0) return false;
						program += sz;
						program_length -= sz;
						if(os) *os << "DW_CFA_offset " << (uint32_t) reg_num << ", " << offset.to_string(false);
					}
					break;
				case DW_CFA_restore:
					{
						uint8_t reg_num = opcode & 0x3f;
						if(os) *os << "DW_CFA_restore " << (uint32_t) reg_num;
					}
					break;
				case 0x00:
					switch(opcode)
					{
						case DW_CFA_nop:
							if(os) *os << "DW_CFA_nop";
							break;
						case DW_CFA_set_loc:
							{
								MEMORY_ADDR addr;
								
								switch(address_size)
								{
									case sizeof(uint16_t):
										{
											if(program_length < sizeof(uint16_t)) return false;
											uint16_t value;
											memcpy(&value, program, sizeof(uint16_t));
											addr = Target2Host(endianness, value);
											program += sizeof(uint16_t);
											program_length -= sizeof(uint16_t);
										}
										break;
									case sizeof(uint32_t):
										{
											if(program_length < sizeof(uint32_t)) return false;
											uint32_t value;
											memcpy(&value, program, sizeof(uint32_t));
											addr = Target2Host(endianness, value);
											program += sizeof(uint32_t);
											program_length -= sizeof(uint32_t);
										}
										break;
									case sizeof(uint64_t):
										{
											if(program_length < sizeof(uint64_t)) return false;
											uint64_t value;
											memcpy(&value, program, sizeof(uint64_t));
											addr = Target2Host(endianness, value);
											program += sizeof(uint64_t);
											program_length -= sizeof(uint64_t);
										}
										break;
									default:
										return false;
								}
								if(os) *os << "DW_CFA_set_loc 0x" << std::hex << addr << std::dec;
							}
							break;
						case DW_CFA_advance_loc1:
							{
								uint8_t delta;
								
								if(program_length < sizeof(delta)) return false;
								memcpy(&delta, program, sizeof(delta));
								delta = Target2Host(dw_call_frame_prog.GetEndianness(), delta);
								program += sizeof(delta);
								program_length -= sizeof(delta);
								if(os) *os << "DW_CFA_advance_loc1 " << std::hex << (uint32_t) delta << std::dec;
							}
							break;
						case DW_CFA_advance_loc2:
							{
								uint16_t delta;
								
								if(program_length < sizeof(delta)) return false;
								memcpy(&delta, program, sizeof(delta));
								delta = Target2Host(dw_call_frame_prog.GetEndianness(), delta);
								program += sizeof(delta);
								program_length -= sizeof(delta);
								if(os) *os << "DW_CFA_advance_loc2 " << std::hex << delta << std::dec;
							}
							break;
						case DW_CFA_advance_loc4:
							{
								uint32_t delta;
								
								if(program_length < sizeof(delta)) return false;
								memcpy(&delta, program, sizeof(delta));
								delta = Target2Host(dw_call_frame_prog.GetEndianness(), delta);
								program += sizeof(delta);
								program_length -= sizeof(delta);
								if(os) *os << "DW_CFA_advance_loc4 " << std::hex << delta << std::dec;
							}
							break;
						case DW_CFA_offset_extended:
							{
								DWARF_LEB128 reg_num;
								DWARF_LEB128 offset;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if((sz = offset.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if(os) *os << "DW_CFA_offset_extended " << reg_num.to_string(false) << ", " << offset.to_string(false);
							}
							break;
						case DW_CFA_restore_extended:
							{
								DWARF_LEB128 reg_num;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if(os) *os << "DW_CFA_restore_extended " << reg_num.to_string(false);
							}
							break;
						case DW_CFA_undefined:
							{
								DWARF_LEB128 reg_num;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if(os) *os << "DW_CFA_undefined " << reg_num.to_string(false);
							}
							break;
						case DW_CFA_same_value:
							{
								DWARF_LEB128 reg_num;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if(os) *os << "DW_CFA_same_value " << reg_num.to_string(false);
							}
							break;
						case DW_CFA_register:
							{
								DWARF_LEB128 reg_num1;
								DWARF_LEB128 reg_num2;
								int64_t sz;
								
								if((sz = reg_num1.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if((sz = reg_num2.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if(os) *os << "DW_CFA_register " << reg_num1.to_string(false) << ", " << reg_num2.to_string(false);
							}
							break;
						case DW_CFA_remember_state:
							if(os) *os << "DW_CFA_remember_state";
							break;
						case DW_CFA_restore_state:
							if(os) *os << "DW_CFA_restore_state";
							break;
						case DW_CFA_def_cfa:
							{
								DWARF_LEB128 reg_num;
								DWARF_LEB128 offset;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if((sz = offset.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if(os) *os << "DW_CFA_def_cfa " << reg_num.to_string(false) << ", " << offset.to_string(false);
							}
							break;
						case DW_CFA_def_cfa_register:
							{
								DWARF_LEB128 reg_num;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if(os) *os << "DW_CFA_def_cfa_register " << reg_num.to_string(false);
							}
							break;
						case DW_CFA_def_cfa_offset:
							{
								DWARF_LEB128 offset;
								int64_t sz;
								
								if((sz = offset.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if(os) *os << "DW_CFA_def_cfa_offset " << offset.to_string(false);
							}
							break;
						case DW_CFA_def_cfa_expression:
							{
								DWARF_LEB128 leb128_block_length;
								int64_t sz;
								if((sz = leb128_block_length.Load(program, program_length)) < 0) return -1;
								program += sz;
								program_length -= sz;

								uint64_t block_length = (uint64_t) leb128_block_length;
								if(program_length < block_length) return -1;

								DWARF_Expression<MEMORY_ADDR> dw_expr = DWARF_Expression<MEMORY_ADDR>(&dw_call_frame_prog, block_length, program);
								program += block_length;
								program_length -= block_length;
								if(os) *os << "DW_CFA_def_cfa_expression {" << dw_expr.to_string() << "}";
							}
							break;
						case DW_CFA_expression:
							{
								DWARF_LEB128 reg_num;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;

								DWARF_LEB128 leb128_block_length;
								if((sz = leb128_block_length.Load(program, program_length)) < 0) return -1;
								program += sz;
								program_length -= sz;

								uint64_t block_length = (uint64_t) leb128_block_length;
								if(program_length < block_length) return -1;

								DWARF_Expression<MEMORY_ADDR> dw_expr = DWARF_Expression<MEMORY_ADDR>(&dw_call_frame_prog, block_length, program);
								program += block_length;
								program_length -= block_length;
								if(os) *os << "DW_CFA_expression " << reg_num.to_string(false) << " {" << dw_expr.to_string() << "}";
							}
							break;
						case DW_CFA_offset_extended_sf:
							{
								DWARF_LEB128 reg_num;
								DWARF_LEB128 offset;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if((sz = offset.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if(os) *os << "DW_CFA_offset_extended_sf " << reg_num.to_string(false) << ", " << offset.to_string(true);
							}
							break;
						case DW_CFA_def_cfa_sf:
							{
								DWARF_LEB128 reg_num;
								DWARF_LEB128 offset;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if((sz = offset.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if(os) *os << "DW_CFA_def_cfa_sf " << reg_num.to_string(false) << ", " << offset.to_string(true);
							}
							break;
						case DW_CFA_def_cfa_offset_sf:
							{
								DWARF_LEB128 offset;
								int64_t sz;
								
								if((sz = offset.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if(os) *os << "DW_CFA_def_cfa_offset_sf " << offset.to_string(true);
							}
							break;
						case DW_CFA_val_offset:
							{
								DWARF_LEB128 reg_num;
								DWARF_LEB128 offset;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if((sz = offset.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if(os) *os << "DW_CFA_val_offset " << reg_num.to_string(false) << ", " << offset.to_string(false);
							}
							break;
						case DW_CFA_val_offset_sf:
							{
								DWARF_LEB128 reg_num;
								DWARF_LEB128 offset;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if((sz = offset.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;
								if(os) *os << "DW_CFA_val_offset_sf " << reg_num.to_string(false) << ", " << offset.to_string(true);
							}
							break;
						case DW_CFA_val_expression:
							{
								DWARF_LEB128 reg_num;
								int64_t sz;
								
								if((sz = reg_num.Load(program, program_length)) < 0) return false;
								program += sz;
								program_length -= sz;

								DWARF_LEB128 leb128_block_length;
								if((sz = leb128_block_length.Load(program, program_length)) < 0) return -1;
								program += sz;
								program_length -= sz;

								uint64_t block_length = (uint64_t) leb128_block_length;
								if(program_length < block_length) return -1;

								DWARF_Expression<MEMORY_ADDR> dw_expr = DWARF_Expression<MEMORY_ADDR>(&dw_call_frame_prog, block_length, program);
								program += block_length;
								program_length -= block_length;
								if(os) *os << "DW_CFA_val_expression " << reg_num.to_string(false) << " {" << dw_expr.to_string() << "}";
							}
							break;
						default:
							return false;
					}
					break;
				default:
					return false;
			}
			if(os && program_length) *os << "; ";
		}
		while(program_length);
	}

	return true;
}

template <class MEMORY_ADDR>
DWARF_RangeListEntry<MEMORY_ADDR>::DWARF_RangeListEntry(const DWARF_CompilationUnit<MEMORY_ADDR> *_dw_cu)
	: dw_cu(_dw_cu)
	, next(0)
	, offset(0xffffffffffffffffULL)
	, begin(0)
	, end(0)
{
}

template <class MEMORY_ADDR>
DWARF_RangeListEntry<MEMORY_ADDR>::~DWARF_RangeListEntry()
{
}

template <class MEMORY_ADDR>
bool DWARF_RangeListEntry<MEMORY_ADDR>::IsBaseAddressSelection() const
{
	return begin > end;
}

template <class MEMORY_ADDR>
bool DWARF_RangeListEntry<MEMORY_ADDR>::IsEndOfList() const
{
	return (begin == 0) && (end == 0);
}

template <class MEMORY_ADDR>
MEMORY_ADDR DWARF_RangeListEntry<MEMORY_ADDR>::GetBegin() const
{
	return begin;
}

template <class MEMORY_ADDR>
MEMORY_ADDR DWARF_RangeListEntry<MEMORY_ADDR>::GetEnd() const
{
	return end;
}

template <class MEMORY_ADDR>
const DWARF_RangeListEntry<MEMORY_ADDR> *DWARF_RangeListEntry<MEMORY_ADDR>::GetNext() const
{
	return next;
}

template <class MEMORY_ADDR>
uint64_t DWARF_RangeListEntry<MEMORY_ADDR>::GetOffset() const
{
	return offset;
}

template <class MEMORY_ADDR>
void DWARF_RangeListEntry<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int _id)
{
	id = _id;
}

template <class MEMORY_ADDR>
std::string DWARF_RangeListEntry<MEMORY_ADDR>::GetHREF() const
{
	std::stringstream sstr;
	sstr << "debug_ranges/" << (id / debug_range_per_file) << ".html#range-" << id;
	return sstr.str();
}

template <class MEMORY_ADDR>
unsigned int DWARF_RangeListEntry<MEMORY_ADDR>::GetId() const
{
	return id;
}

template <class MEMORY_ADDR>
int64_t DWARF_RangeListEntry<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size, uint64_t _offset)
{
	offset = _offset;
	int64_t size = 0;
	
	endian_type endianness = dw_cu->GetEndianness();
	
	switch(dw_cu->GetAddressSize())
	{
		case 2:
			{
				uint16_t begin16;
				if(max_size < sizeof(begin16)) return -1;
				memcpy(&begin16, rawdata, sizeof(begin16));
				begin16 = Target2Host(endianness, begin16);
				rawdata += sizeof(begin16);
				max_size -= sizeof(begin16);
				size += sizeof(begin16);
				begin = (MEMORY_ADDR) begin16;

				uint16_t end16;
				if(max_size < sizeof(end16)) return -1;
				memcpy(&end16, rawdata, sizeof(end16));
				end16 = Target2Host(endianness, end16);
				rawdata += sizeof(end16);
				max_size -= sizeof(end16);
				size += sizeof(end16);
				end = (MEMORY_ADDR) end16;
			}
			break;
		case 4:
			{
				uint32_t begin32;
				if(max_size < sizeof(begin32)) return -1;
				memcpy(&begin32, rawdata, sizeof(begin32));
				begin32 = Target2Host(endianness, begin32);
				rawdata += sizeof(begin32);
				max_size -= sizeof(begin32);
				size += sizeof(begin32);
				begin = (MEMORY_ADDR) begin32;

				uint32_t end32;
				if(max_size < sizeof(end32)) return -1;
				memcpy(&end32, rawdata, sizeof(end32));
				end32 = Target2Host(endianness, end32);
				rawdata += sizeof(end32);
				max_size -= sizeof(end32);
				size += sizeof(end32);
				end = (MEMORY_ADDR) end32;
			}
			break;
		case 8:
			{
				uint64_t begin64;
				if(max_size < sizeof(begin64)) return -1;
				memcpy(&begin64, rawdata, sizeof(begin64));
				begin64 = Target2Host(endianness, begin64);
				rawdata += sizeof(begin64);
				max_size -= sizeof(begin64);
				size += sizeof(begin64);
				begin = (MEMORY_ADDR) begin64;

				uint64_t end64;
				if(max_size < sizeof(end64)) return -1;
				memcpy(&end64, rawdata, sizeof(end64));
				end64 = Target2Host(endianness, end64);
				rawdata += sizeof(end64);
				max_size -= sizeof(end64);
				size += sizeof(end64);
				end = (MEMORY_ADDR) end64;
			}
			break;
		default:
			return -1;
	}

	return size;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_RangeListEntry<MEMORY_ADDR>::to_XML(std::ostream& os) const
{
	if(IsEndOfList()) return os << "<DW_EOL offset=\"" << offset << "\"/>";
	if(IsBaseAddressSelection())
	{
		os << "<DW_BASE_ADDRESS_SELECTION offset=\"" << offset << "\" largest_address_offset=\"0x" << std::hex << begin << std::dec << "\" address=\"0x" << std::hex << end << std::dec << "\"/>";
		return os;
	}
	os << "<DW_RANGE offset=\"" << offset << "\" begin_address_offset=\"0x" << std::hex << begin << std::dec << "\" end_address_offset=\"0x" << std::hex << end << std::dec << "\"/>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_RangeListEntry<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	os << "<tr id=\"range-" << id << "\">" << std::endl;
	if(IsEndOfList())
	{
		os << "<td>End-of-List</td>";
	}
	else if(IsBaseAddressSelection())
	{
		os << "<td>Base address selection</td>";
	}
	else
	{
		os << "<td>Address Range</td>";
	}
	os << "<td>" << id << "</td>" << std::endl;
	os << "<td>" << offset << "</td>" << std::endl;
	os << "<td>";
	const DWARF_RangeListEntry<MEMORY_ADDR> *next = DWARF_RangeListEntry<MEMORY_ADDR>::GetNext();
	if(next)
	{
		os << "<a href=\"../" << next->GetHREF() << "\">range #" << next->GetId() << "</a>" << std::endl;
	}
	else
	{
		os << "-";
	}
	os << "</td>";
	if(IsEndOfList())
	{
		os << "<td>-</td>";
	}
	else if(IsBaseAddressSelection())
	{
		os << "<td><table><tr><th>Largest address offset</th><th>Address</th></tr>";
		os << "<tr><td>0x" << std::hex << begin << std::dec << "</td><td>0x" << std::hex << end << std::dec << "</td></tr></table></td>";
	}
	else
	{
		os << "<td><table><tr><th>Beginning address offset</th><th>End Address offset</th></tr>";
		os << "<tr><td>0x" << std::hex << begin << std::dec << "</td><td>0x" << std::hex << end << std::dec << "</td></tr></table></td>";
	}
	os << "</tr>" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_RangeListEntry<MEMORY_ADDR>& dw_range_list_entry)
{
	if(dw_range_list_entry.IsEndOfList()) return os << "EOL";
	if(dw_range_list_entry.IsBaseAddressSelection()) os << "Base";
	return os << "[0x" << std::hex << dw_range_list_entry.begin << "-0x" << dw_range_list_entry.end << "]";
}

template <class MEMORY_ADDR>
DWARF_MacInfoListEntry<MEMORY_ADDR>::DWARF_MacInfoListEntry(uint8_t _dw_mac_info_type)
	: next(0)
	, offset(0xffffffffffffffffULL)
	, dw_mac_info_type(_dw_mac_info_type)
{
}

template <class MEMORY_ADDR>
DWARF_MacInfoListEntry<MEMORY_ADDR>::~DWARF_MacInfoListEntry()
{
}

template <class MEMORY_ADDR>
void DWARF_MacInfoListEntry<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int _id)
{
	id = _id;
}

template <class MEMORY_ADDR>
uint8_t DWARF_MacInfoListEntry<MEMORY_ADDR>::GetType() const
{
	return dw_mac_info_type;
}

template <class MEMORY_ADDR>
uint64_t DWARF_MacInfoListEntry<MEMORY_ADDR>::GetOffset() const
{
	return offset;
}

template <class MEMORY_ADDR>
std::string DWARF_MacInfoListEntry<MEMORY_ADDR>::GetHREF() const
{
	std::stringstream sstr;
	sstr << "debug_macinfo/" << (id / debug_macinfo_per_file) << ".html#mac-" << id;
	return sstr.str();
}

template <class MEMORY_ADDR>
unsigned int DWARF_MacInfoListEntry<MEMORY_ADDR>::GetId() const
{
	return id;
}

template <class MEMORY_ADDR>
const DWARF_MacInfoListEntry<MEMORY_ADDR> *DWARF_MacInfoListEntry<MEMORY_ADDR>::GetNext() const
{
	return next;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_MacInfoListEntry<MEMORY_ADDR>& dw_macinfo_list_entry)
{
	return os << dw_macinfo_list_entry.to_string();
}

template <class MEMORY_ADDR>
DWARF_MacInfoListEntryDefine<MEMORY_ADDR>::DWARF_MacInfoListEntryDefine()
	: DWARF_MacInfoListEntry<MEMORY_ADDR>(DW_MACINFO_define)
	, lineno()
	, preprocessor_symbol_name(0)
{
}

template <class MEMORY_ADDR>
DWARF_MacInfoListEntryDefine<MEMORY_ADDR>::~DWARF_MacInfoListEntryDefine()
{
}

template <class MEMORY_ADDR>
int64_t DWARF_MacInfoListEntryDefine<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size, uint64_t _offset)
{
	DWARF_MacInfoListEntry<MEMORY_ADDR>::offset = _offset;
	int64_t size = 0;
	
	uint8_t type;
	if(max_size < sizeof(type)) return -1;
	memcpy(&type, rawdata, sizeof(type));
	rawdata += sizeof(type);
	size += sizeof(type);
	max_size -= sizeof(type);
	
	if(DWARF_MacInfoListEntry<MEMORY_ADDR>::dw_mac_info_type != type) return -1;
	
	int64_t sz;
	if((sz = lineno.Load(rawdata, max_size)) < 0) return -1;
	rawdata += sz;
	max_size -= sz;
	size += sz;
	
	if(!max_size) return -1;
	preprocessor_symbol_name = (const char *) rawdata;
	sz = strlen(preprocessor_symbol_name) + 1;
	size += sz;
	rawdata += sz;
	max_size -= sz;
	
	return size;
}

template <class MEMORY_ADDR>
const DWARF_LEB128& DWARF_MacInfoListEntryDefine<MEMORY_ADDR>::GetLineNo() const
{
	return lineno;
}

template <class MEMORY_ADDR>
const char *DWARF_MacInfoListEntryDefine<MEMORY_ADDR>::GetPreprocessorSymbolName() const
{
	return preprocessor_symbol_name;
}

template <class MEMORY_ADDR>
std::string DWARF_MacInfoListEntryDefine<MEMORY_ADDR>::to_string() const
{
	std::stringstream sstr;
	sstr << "DW_MACINFO_define " << lineno.to_string(false) << ", \"" << preprocessor_symbol_name << "\"";
	return std::string(sstr.str());
}

template <class MEMORY_ADDR>
std::ostream& DWARF_MacInfoListEntryDefine<MEMORY_ADDR>::to_XML(std::ostream& os) const
{
	os << "<DW_MACINFO_define offset=\"" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetOffset() << "\" lineno=\"" << lineno.to_string(false) << "\" preprocessor_symbol_name=\"";
	c_string_to_XML(os, preprocessor_symbol_name);
	os << "\"/>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_MacInfoListEntryDefine<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	os << "<tr id=\"mac-" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetId() << "\">" << std::endl;
	os << "<td>DW_MACINFO_define</td>" << std::endl;
	os << "<td>" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetId() << "</td>" << std::endl;
	os << "<td>" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetOffset() << "</td>" << std::endl;
	os << "<td>";
	const DWARF_MacInfoListEntry<MEMORY_ADDR> *next = DWARF_MacInfoListEntry<MEMORY_ADDR>::GetNext();
	if(next)
	{
		os << "<a href=\"../" << next->GetHREF() << "\">mac-" << next->GetId() << "</a>" << std::endl;
	}
	else
	{
		os << "-";
	}
	os << "</td>" << std::endl;
	
	os << "<td>" << std::endl;
	os << "<table class=\"macinfo\">" << std::endl;
	os << "<tr>" << std::endl;
	os << "<th>Line number</th><th>Preprocessor symbol name</th>" << std::endl;
	os << "</tr>" << std::endl;
	os << "<tr>" << std::endl;
	os << "<td>" << lineno.to_string(false) << "</td><td>&quot;";
	c_string_to_HTML(os, preprocessor_symbol_name);
	os << "&quot;</td>" << std::endl;
	os << "</tr>" << std::endl;
	os << "</table>" << std::endl;
	os << "</td>" << std::endl;
	os << "</tr>" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
DWARF_MacInfoListEntryUndef<MEMORY_ADDR>::DWARF_MacInfoListEntryUndef()
	: DWARF_MacInfoListEntry<MEMORY_ADDR>(DW_MACINFO_undef)
	, lineno()
	, preprocessor_symbol_name(0)
{
}

template <class MEMORY_ADDR>
DWARF_MacInfoListEntryUndef<MEMORY_ADDR>::~DWARF_MacInfoListEntryUndef()
{
}

template <class MEMORY_ADDR>
int64_t DWARF_MacInfoListEntryUndef<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size, uint64_t _offset)
{
	DWARF_MacInfoListEntry<MEMORY_ADDR>::offset = _offset;
	int64_t size = 0;
	
	uint8_t type;
	if(max_size < sizeof(type)) return -1;
	memcpy(&type, rawdata, sizeof(type));
	rawdata += sizeof(type);
	size += sizeof(type);
	max_size -= sizeof(type);
	
	if(DWARF_MacInfoListEntry<MEMORY_ADDR>::dw_mac_info_type != type) return -1;

	int64_t sz;
	if((sz = lineno.Load(rawdata, max_size)) < 0) return -1;
	rawdata += sz;
	max_size -= sz;
	size += sz;
	
	if(!max_size) return -1;
	preprocessor_symbol_name = (const char *) rawdata;
	sz = strlen(preprocessor_symbol_name) + 1;
	size += sz;
	rawdata += sz;
	max_size -= sz;
	
	return size;
}

template <class MEMORY_ADDR>
const DWARF_LEB128& DWARF_MacInfoListEntryUndef<MEMORY_ADDR>::GetLineNo() const
{
	return lineno;
}

template <class MEMORY_ADDR>
const char *DWARF_MacInfoListEntryUndef<MEMORY_ADDR>::GetPreprocessorSymbolName() const
{
	return preprocessor_symbol_name;
}

template <class MEMORY_ADDR>
std::string DWARF_MacInfoListEntryUndef<MEMORY_ADDR>::to_string() const
{
	std::stringstream sstr;
	sstr << "DW_MACINFO_undef " << lineno.to_string(false) << ", \"" << preprocessor_symbol_name << "\"";
	return std::string(sstr.str());
}

template <class MEMORY_ADDR>
std::ostream& DWARF_MacInfoListEntryUndef<MEMORY_ADDR>::to_XML(std::ostream& os) const
{
	os << "<DW_MACINFO_undef offset=\"" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetOffset() << "\" lineno=\"" << lineno.to_string(false) << "\" preprocessor_symbol_name=\"";
	c_string_to_XML(os, preprocessor_symbol_name);
	os << "\"/>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_MacInfoListEntryUndef<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	os << "<tr id=\"mac-" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetId() << "\">" << std::endl;
	os << "<td>DW_MACINFO_Undef</td>" << std::endl;
	os << "<td>" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetId() << "</td>" << std::endl;
	os << "<td>" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetOffset() << "</td>" << std::endl;
	os << "<td>";
	const DWARF_MacInfoListEntry<MEMORY_ADDR> *next = DWARF_MacInfoListEntry<MEMORY_ADDR>::GetNext();
	if(next)
	{
		os << "<a href=\"../" << next->GetHREF() << "\">mac-" << next->GetId() << "</a>" << std::endl;
	}
	else
	{
		os << "-";
	}
	os << "</td>" << std::endl;
	os << "<td>" << std::endl;
	os << "<table class=\"macinfo\">" << std::endl;
	os << "<tr>" << std::endl;
	os << "<th>LineNo</th><th>Preprocessor symbol name</th>" << std::endl;
	os << "</tr>" << std::endl;
	os << "<tr>" << std::endl;
	os << "<td>" << lineno.to_string(false) << "</td><td>&quot;";
	c_string_to_HTML(os, preprocessor_symbol_name);
	os << "&quot;</td>" << std::endl;
	os << "</tr>" << std::endl;
	os << "</table>" << std::endl;
	os << "</td>" << std::endl;
	os << "</tr>" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
DWARF_MacInfoListEntryStartFile<MEMORY_ADDR>::DWARF_MacInfoListEntryStartFile()
	: DWARF_MacInfoListEntry<MEMORY_ADDR>(DW_MACINFO_start_file)
	, lineno()
	, file_idx()
{
}

template <class MEMORY_ADDR>
DWARF_MacInfoListEntryStartFile<MEMORY_ADDR>::~DWARF_MacInfoListEntryStartFile()
{
}

template <class MEMORY_ADDR>
int64_t DWARF_MacInfoListEntryStartFile<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size, uint64_t _offset)
{
	DWARF_MacInfoListEntry<MEMORY_ADDR>::offset = _offset;
	int64_t size = 0;
	
	uint8_t type;
	if(max_size < sizeof(type)) return -1;
	memcpy(&type, rawdata, sizeof(type));
	rawdata += sizeof(type);
	size += sizeof(type);
	max_size -= sizeof(type);
	
	if(DWARF_MacInfoListEntry<MEMORY_ADDR>::dw_mac_info_type != type) return -1;

	int64_t sz;
	if((sz = lineno.Load(rawdata, max_size)) < 0) return -1;
	rawdata += sz;
	max_size -= sz;
	size += sz;
	
	if((sz = file_idx.Load(rawdata, max_size)) < 0) return -1;
	rawdata += sz;
	max_size -= sz;
	size += sz;
	
	return size;
}

template <class MEMORY_ADDR>
const DWARF_LEB128& DWARF_MacInfoListEntryStartFile<MEMORY_ADDR>::GetLineNo() const
{
	return lineno;
}

template <class MEMORY_ADDR>
const DWARF_LEB128& DWARF_MacInfoListEntryStartFile<MEMORY_ADDR>::GetFileIndex() const
{
	return file_idx;
}

template <class MEMORY_ADDR>
std::string DWARF_MacInfoListEntryStartFile<MEMORY_ADDR>::to_string() const
{
	std::stringstream sstr;
	sstr << "DW_MACINFO_start_file " << lineno.to_string(false) << ", " << file_idx.to_string(false);
	return std::string(sstr.str());
}

template <class MEMORY_ADDR>
std::ostream& DWARF_MacInfoListEntryStartFile<MEMORY_ADDR>::to_XML(std::ostream& os) const
{
	os << "<DW_MACINFO_start_file offset=\"" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetOffset() << "\" lineno=\"" << lineno.to_string(false) << "\" file_idx=\"" << file_idx.to_string(false) << "\"/>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_MacInfoListEntryStartFile<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	os << "<tr id=\"mac-" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetId() << "\">" << std::endl;
	os << "<td>DW_MACINFO_start_file</td>" << std::endl;
	os << "<td>" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetId() << "</td>" << std::endl;
	os << "<td>" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetOffset() << "</td>" << std::endl;
	os << "<td>";
	const DWARF_MacInfoListEntry<MEMORY_ADDR> *next = DWARF_MacInfoListEntry<MEMORY_ADDR>::GetNext();
	if(next)
	{
		os << "<a href=\"../" << next->GetHREF() << "\">mac-" << next->GetId() << "</a>" << std::endl;
	}
	else
	{
		os << "-";
	}
	os << "</td>" << std::endl;
	os << "<td>" << std::endl;
	os << "<table class=\"macinfo\">" << std::endl;
	os << "<tr>" << std::endl;
	os << "<th>LineNo</th><th>File index</th>" << std::endl;
	os << "</tr>" << std::endl;
	os << "<tr>" << std::endl;
	os << "<td>" << lineno.to_string(false) << "</td><td>" << file_idx.to_string(false) << "</td>" << std::endl;
	os << "</tr>" << std::endl;
	os << "</table>" << std::endl;
	os << "</td>" << std::endl;
	os << "</tr>" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
DWARF_MacInfoListEntryEndFile<MEMORY_ADDR>::DWARF_MacInfoListEntryEndFile()
	: DWARF_MacInfoListEntry<MEMORY_ADDR>(DW_MACINFO_end_file)
{
}

template <class MEMORY_ADDR>
DWARF_MacInfoListEntryEndFile<MEMORY_ADDR>::~DWARF_MacInfoListEntryEndFile()
{
}

template <class MEMORY_ADDR>
int64_t DWARF_MacInfoListEntryEndFile<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size, uint64_t _offset)
{
	DWARF_MacInfoListEntry<MEMORY_ADDR>::offset = _offset;
	int64_t size = 0;
	
	uint8_t type;
	if(max_size < sizeof(type)) return -1;
	memcpy(&type, rawdata, sizeof(type));
	rawdata += sizeof(type);
	size += sizeof(type);
	max_size -= sizeof(type);
	
	if(DWARF_MacInfoListEntry<MEMORY_ADDR>::dw_mac_info_type != type) return -1;
	
	return size;
}

template <class MEMORY_ADDR>
std::string DWARF_MacInfoListEntryEndFile<MEMORY_ADDR>::to_string() const
{
	return std::string("DW_MACINFO_end_file");
}

template <class MEMORY_ADDR>
std::ostream& DWARF_MacInfoListEntryEndFile<MEMORY_ADDR>::to_XML(std::ostream& os) const
{
	os << "<DW_MACINFO_end_file offset=\"" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetOffset() << "\"/>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_MacInfoListEntryEndFile<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	os << "<tr id=\"mac-" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetId() << "\">" << std::endl;
	os << "<td>DW_MACINFO_end_file</td>" << std::endl;
	os << "<td>" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetId() << "</td>" << std::endl;
	os << "<td>" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetOffset() << "</td>" << std::endl;
	os << "<td>";
	const DWARF_MacInfoListEntry<MEMORY_ADDR> *next = DWARF_MacInfoListEntry<MEMORY_ADDR>::GetNext();
	if(next)
	{
		os << "<a href=\"../" << next->GetHREF() << "\">mac-" << next->GetId() << "</a>" << std::endl;
	}
	else
	{
		os << "-";
	}
	os << "</td>" << std::endl;
	os << "<td></td>" << std::endl;
	os << "</tr>" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
DWARF_MacInfoListEntryVendorExtension<MEMORY_ADDR>::DWARF_MacInfoListEntryVendorExtension()
	: DWARF_MacInfoListEntry<MEMORY_ADDR>(DW_MACINFO_define)
	, vendor_ext_constant()
	, vendor_ext_string(0)
{
}

template <class MEMORY_ADDR>
DWARF_MacInfoListEntryVendorExtension<MEMORY_ADDR>::~DWARF_MacInfoListEntryVendorExtension()
{
}

template <class MEMORY_ADDR>
int64_t DWARF_MacInfoListEntryVendorExtension<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size, uint64_t _offset)
{
	DWARF_MacInfoListEntry<MEMORY_ADDR>::offset = _offset;
	int64_t size = 0;
	
	uint8_t type;
	if(max_size < sizeof(type)) return -1;
	memcpy(&type, rawdata, sizeof(type));
	rawdata += sizeof(type);
	size += sizeof(type);
	max_size -= sizeof(type);
	
	if(DWARF_MacInfoListEntry<MEMORY_ADDR>::dw_mac_info_type != type) return -1;
	
	int64_t sz;
	if((sz = vendor_ext_constant.Load(rawdata, max_size)) < 0) return -1;
	rawdata += sz;
	max_size -= sz;
	size += sz;
	
	if(!max_size) return -1;
	vendor_ext_string = (const char *) rawdata;
	sz = strlen(vendor_ext_string) + 1;
	size += sz;
	rawdata += sz;
	max_size -= sz;
	
	return size;
}

template <class MEMORY_ADDR>
const DWARF_LEB128& DWARF_MacInfoListEntryVendorExtension<MEMORY_ADDR>::GetConstant() const
{
	return vendor_ext_constant;
}

template <class MEMORY_ADDR>
const char *DWARF_MacInfoListEntryVendorExtension<MEMORY_ADDR>::GetString() const
{
	return vendor_ext_string;
}

template <class MEMORY_ADDR>
std::string DWARF_MacInfoListEntryVendorExtension<MEMORY_ADDR>::to_string() const
{
	std::stringstream sstr;
	sstr << "DW_MACINFO_vendor_ext " << vendor_ext_constant.to_string(false) << ", \"" << vendor_ext_string << "\"";
	return std::string(sstr.str());
}

template <class MEMORY_ADDR>
std::ostream& DWARF_MacInfoListEntryVendorExtension<MEMORY_ADDR>::to_XML(std::ostream& os) const
{
	os << "<DW_MACINFO_vendor_ext offset=\"" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetOffset() << "\" vendor_ext_constant=\"" << vendor_ext_constant.to_string(false) << "\" vendor_ext_string=\"";
	c_string_to_XML(os, vendor_ext_string);
	os << "\"/>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_MacInfoListEntryVendorExtension<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	os << "<tr id=\"mac-" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetId() << "\">" << std::endl;
	os << "<td>DW_MACINFO_vendor_ext</td>" << std::endl;
	os << "<td>" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetId() << "</td>" << std::endl;
	os << "<td>" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetOffset() << "</td>" << std::endl;
	os << "<td>";
	const DWARF_MacInfoListEntry<MEMORY_ADDR> *next = DWARF_MacInfoListEntry<MEMORY_ADDR>::GetNext();
	if(next)
	{
		os << "<a href=\"../" << next->GetHREF() << "\">mac-" << next->GetId() << "</a>" << std::endl;
	}
	else
	{
		os << "-";
	}
	os << "</td>" << std::endl;
	os << "<td>" << std::endl;
	os << "<table class=\"macinfo\">" << std::endl;
	os << "<tr>" << std::endl;
	os << "<th>Vendor Ext. Constant</th><th>Vendor Ext. String</th>" << std::endl;
	os << "</tr>" << std::endl;
	os << "<tr>" << std::endl;
	os << "<td>" << vendor_ext_constant.to_string(false) << "</td><td>";
	c_string_to_HTML(os, vendor_ext_string);
	os << "</td>" << std::endl;
	os << "</tr>" << std::endl;
	os << "</table>" << std::endl;
	os << "</td>" << std::endl;
	os << "</tr>" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
DWARF_MacInfoListEntryNull<MEMORY_ADDR>::DWARF_MacInfoListEntryNull()
	: DWARF_MacInfoListEntry<MEMORY_ADDR>(0)
{
}

template <class MEMORY_ADDR>
DWARF_MacInfoListEntryNull<MEMORY_ADDR>::~DWARF_MacInfoListEntryNull()
{
}

template <class MEMORY_ADDR>
int64_t DWARF_MacInfoListEntryNull<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size, uint64_t _offset)
{
	DWARF_MacInfoListEntry<MEMORY_ADDR>::offset = _offset;
	int64_t size = 0;
	
	uint8_t type;
	if(max_size < sizeof(type)) return -1;
	memcpy(&type, rawdata, sizeof(type));
	rawdata += sizeof(type);
	size += sizeof(type);
	max_size -= sizeof(type);
	
	if(DWARF_MacInfoListEntry<MEMORY_ADDR>::dw_mac_info_type != type) return -1;
	
	return size;
}

template <class MEMORY_ADDR>
std::string DWARF_MacInfoListEntryNull<MEMORY_ADDR>::to_string() const
{
	return std::string("DW_MACINFO_null");
}

template <class MEMORY_ADDR>
std::ostream& DWARF_MacInfoListEntryNull<MEMORY_ADDR>::to_XML(std::ostream& os) const
{
	os << "<DW_MACINFO_null offset=\"" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetOffset() << "\"/>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_MacInfoListEntryNull<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	os << "<tr id=\"mac-" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetId() << "\">" << std::endl;
	os << "<td>DW_MACINFO_null</td>" << std::endl;
	os << "<td>" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetId() << "</td>" << std::endl;
	os << "<td>" << DWARF_MacInfoListEntry<MEMORY_ADDR>::GetOffset() << "</td>" << std::endl;
	os << "<td>";
	const DWARF_MacInfoListEntry<MEMORY_ADDR> *next = DWARF_MacInfoListEntry<MEMORY_ADDR>::GetNext();
	if(next)
	{
		os << "<a href=\"../" << next->GetHREF() << "\">mac-" << next->GetId() << "</a>" << std::endl;
	}
	else
	{
		os << "-";
	}
	os << "</td>" << std::endl;
	os << "<td></td>" << std::endl;
	os << "</tr>" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
DWARF_AddressRangeDescriptor<MEMORY_ADDR>::DWARF_AddressRangeDescriptor(const DWARF_AddressRanges<MEMORY_ADDR> *_dw_aranges)
	: dw_aranges(_dw_aranges)
	, addr(0)
	, length(0)
{
}

template <class MEMORY_ADDR>
DWARF_AddressRangeDescriptor<MEMORY_ADDR>::~DWARF_AddressRangeDescriptor()
{
}

template <class MEMORY_ADDR>
int64_t DWARF_AddressRangeDescriptor<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size)
{
	int64_t size = 0;
	endian_type endianness = dw_aranges->GetEndianness();
	
	uint8_t address_size = dw_aranges->GetAddressSize();
	if(max_size < address_size) return -1;
	
	switch(address_size)
	{
		case sizeof(uint16_t):
			{
				uint16_t addr16;
				memcpy(&addr16, rawdata, sizeof(addr16));
				addr16 = Target2Host(endianness, addr16);
				addr = addr16;
			}
			break;
		case sizeof(uint32_t):
			{
				uint32_t addr32;
				memcpy(&addr32, rawdata, sizeof(addr32));
				addr32 = Target2Host(endianness, addr32);
				addr = addr32;
			}
			break;
		case sizeof(uint64_t):
			{
				uint64_t addr64;
				memcpy(&addr64, rawdata, sizeof(addr64));
				addr64 = Target2Host(endianness, addr64);
				addr = addr64;
			}
			break;
		default:
			return -1;
	}
	
	size += address_size;
	rawdata += address_size;
	max_size -= address_size;

	if(max_size < address_size) return -1;
	
	switch(address_size)
	{
		case sizeof(uint16_t):
			{
				uint16_t length16;
				memcpy(&length16, rawdata, sizeof(length16));
				length16 = Target2Host(endianness, length16);
				length = length16;
			}
			break;
		case sizeof(uint32_t):
			{
				uint32_t length32;
				memcpy(&length32, rawdata, sizeof(length32));
				length32 = Target2Host(endianness, length32);
				length = length32;
			}
			break;
		case sizeof(uint64_t):
			{
				uint64_t length64;
				memcpy(&length64, rawdata, sizeof(length64));
				length64 = Target2Host(endianness, length64);
				length = length64;
			}
			break;
		default:
			return -1;
	}
	
	size += address_size;
	rawdata += address_size;
	max_size -= address_size;
	
	return size;
}

template <class MEMORY_ADDR>
MEMORY_ADDR DWARF_AddressRangeDescriptor<MEMORY_ADDR>::GetAddress() const
{
	return addr;
}

template <class MEMORY_ADDR>
MEMORY_ADDR DWARF_AddressRangeDescriptor<MEMORY_ADDR>::GetLength() const
{
	return length;
}

template <class MEMORY_ADDR>
bool DWARF_AddressRangeDescriptor<MEMORY_ADDR>::IsNull() const
{
	return (addr == 0) && (length == 0);
}

template <class MEMORY_ADDR>
std::ostream& DWARF_AddressRangeDescriptor<MEMORY_ADDR>::to_XML(std::ostream& os) const
{
	os << "<DW_ARANGE address=\"0x" << std::hex << addr << std::dec << "\" length=\"0x" << std::hex << length << std::dec << "\"/>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_AddressRangeDescriptor<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	os << "<tr><td>0x" << std::hex << addr << std::dec << "</td><td>0x" << std::hex << length << std::dec << "</td></tr>" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_AddressRangeDescriptor<MEMORY_ADDR>& dw_addr_range_desc)
{
	os << "      - Address: 0x" << std::hex << dw_addr_range_desc.addr << std::dec << std::endl;
	os << "      - Length: 0x" << std::hex << dw_addr_range_desc.length << std::dec;
	return os;
}

template <class MEMORY_ADDR>
DWARF_AddressRanges<MEMORY_ADDR>::DWARF_AddressRanges(DWARF_Handler<MEMORY_ADDR> *_dw_handler)
	: dw_handler(_dw_handler)
	, unit_length(0)
	, version(0)
	, debug_info_offset(0)
	, address_size(0)
	, segment_size(0)
	, dw_addr_range_descriptors()
{
}

template <class MEMORY_ADDR>
DWARF_AddressRanges<MEMORY_ADDR>::~DWARF_AddressRanges()
{
	unsigned int num_addr_range_descriptors = dw_addr_range_descriptors.size();
	
	unsigned int i;
	
	for(i = 0; i < num_addr_range_descriptors; i++)
	{
		delete dw_addr_range_descriptors[i];
	}
}

template <class MEMORY_ADDR>
endian_type DWARF_AddressRanges<MEMORY_ADDR>::GetEndianness() const
{
	return dw_handler->GetEndianness();
}

template <class MEMORY_ADDR>
uint8_t DWARF_AddressRanges<MEMORY_ADDR>::GetAddressSize() const
{
	return address_size;
}

template <class MEMORY_ADDR>
uint8_t DWARF_AddressRanges<MEMORY_ADDR>::GetSegmentSize() const
{
	return segment_size;
}

template <class MEMORY_ADDR>
const std::vector<DWARF_AddressRangeDescriptor<MEMORY_ADDR> *>& DWARF_AddressRanges<MEMORY_ADDR>::GetDescriptors() const
{
	return dw_addr_range_descriptors;
}

template <class MEMORY_ADDR>
const DWARF_CompilationUnit<MEMORY_ADDR> *DWARF_AddressRanges<MEMORY_ADDR>::GetCompilationUnit() const
{
	return dw_cu;
}

template <class MEMORY_ADDR>
void DWARF_AddressRanges<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler)
{
	dw_cu = dw_handler->FindCompilationUnit(debug_info_offset);
	if(!dw_cu)
	{
		std::cerr << "Can't find .debug_info compilation unit contribution at offset " << debug_info_offset << std::endl;
	}
}

template <class MEMORY_ADDR>
int64_t DWARF_AddressRanges<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size)
{
	endian_type endianness = dw_handler->GetEndianness();
	DWARF_Format dw_fmt;
	uint32_t unit_length32;
	
	uint64_t size = 0;
	if(max_size < sizeof(unit_length32)) return -1;
	memcpy(&unit_length32, rawdata, sizeof(unit_length32));
	unit_length32 = Target2Host(endianness, unit_length32);
	rawdata += sizeof(unit_length32);
	max_size -= sizeof(unit_length32);
	size += sizeof(unit_length32);
	
	dw_fmt = (unit_length32 == 0xffffffffUL) ? FMT_DWARF64 : FMT_DWARF32;
	
	if(dw_fmt == FMT_DWARF64)
	{
		uint64_t unit_length64;
		if(max_size < sizeof(unit_length64)) return -1;
		memcpy(&unit_length64, rawdata, sizeof(unit_length64));
		unit_length64 = Target2Host(endianness, unit_length64);
		rawdata += sizeof(unit_length64);
		max_size -= sizeof(unit_length64);
		size += sizeof(unit_length64);
		unit_length = unit_length64;
	}
	else
	{
		unit_length = unit_length32;
	}
	
	if(max_size < unit_length) return -1;
	
	if(max_size < sizeof(version)) return -1;
	memcpy(&version, rawdata, sizeof(version));
	version = Target2Host(endianness, version);
	rawdata += sizeof(version);
	max_size -= sizeof(version);
	size += sizeof(version);

	if(version != 2) return -1; // 2=(DWARF2 or DWARF3)
	
	if(dw_fmt == FMT_DWARF64)
	{
		uint64_t debug_info_offset64;
	
		if(max_size < sizeof(debug_info_offset64)) return -1;
		memcpy(&debug_info_offset64, rawdata, sizeof(debug_info_offset64));
		debug_info_offset64 = Target2Host(endianness, debug_info_offset64);
		rawdata += sizeof(debug_info_offset64);
		max_size -= sizeof(debug_info_offset64);
		size += sizeof(debug_info_offset64);
		debug_info_offset = debug_info_offset64;
	}
	else
	{
		uint32_t debug_info_offset32;
	
		if(max_size < sizeof(debug_info_offset32)) return -1;
		memcpy(&debug_info_offset32, rawdata, sizeof(debug_info_offset32));
		debug_info_offset32 = Target2Host(endianness, debug_info_offset32);
		rawdata += sizeof(debug_info_offset32);
		max_size -= sizeof(debug_info_offset32);
		size += sizeof(debug_info_offset32);
		debug_info_offset = debug_info_offset32;
	}

	if(max_size < sizeof(address_size)) return -1;
	memcpy(&address_size, rawdata, sizeof(address_size));
	address_size = Target2Host(endianness, address_size);
	rawdata += sizeof(address_size);
	max_size -= sizeof(address_size);
	size += sizeof(address_size);
	
	if(max_size < sizeof(segment_size)) return -1;
	memcpy(&segment_size, rawdata, sizeof(segment_size));
	segment_size = Target2Host(endianness, segment_size);
	rawdata += sizeof(segment_size);
	max_size -= sizeof(segment_size);
	size += sizeof(segment_size);
	
	uint64_t tuple_size = 2 * address_size;
	uint64_t header_padding = tuple_size - (size % tuple_size); // quite inefficient
	if(max_size < header_padding) return -1;
	rawdata += header_padding;
	max_size -= header_padding;
	size += header_padding;

	do
	{
		DWARF_AddressRangeDescriptor<MEMORY_ADDR> *dw_addr_range_descriptor = new DWARF_AddressRangeDescriptor<MEMORY_ADDR>(this);
		
		int64_t sz;
		if((sz = dw_addr_range_descriptor->Load(rawdata, max_size)) < 0)
		{
			delete dw_addr_range_descriptor;
			return -1;
		}
		rawdata += sz;
		max_size -= sz;
		size += sz;
		
		if(dw_addr_range_descriptor->IsNull())
		{
			delete dw_addr_range_descriptor;
			break;
		}
		
		dw_addr_range_descriptors.push_back(dw_addr_range_descriptor);
	}
	while(size < ((dw_fmt == FMT_DWARF64) ? (unit_length + sizeof(uint32_t) + sizeof(uint64_t)) : (unit_length + sizeof(uint32_t))));
	
	return size;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_AddressRanges<MEMORY_ADDR>::to_XML(std::ostream& os) const
{
	os << "<DW_RANGES unit_length=\"" << unit_length << "\" version=\"" << version << "\" debug_info_offset=\"" << debug_info_offset << "\" address_size=\"" << (uint32_t) address_size << "\" segment_size=\"" << (uint32_t) segment_size << "\">" << std::endl;
	unsigned int num_addr_range_descriptors = dw_addr_range_descriptors.size();
	
	unsigned int i;
	
	for(i = 0; i < num_addr_range_descriptors; i++)
	{
		 dw_addr_range_descriptors[i]->to_XML(os) << std::endl;
	}
	os << "</DW_RANGES>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_AddressRanges<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	os << "<tr>" << std::endl;
	os << "<td>" << version << "</td><td><a href=\"../" << dw_cu->GetHREF() << "\">cu-" << dw_cu->GetId() << "</a></td>";
	os << "<td>" <<  (uint32_t) address_size << "</td><td>" << (uint32_t) segment_size << "</td>" << std::endl;
	os << "<td><table><tr><th>Address</th><th>Length</th></tr>";
	unsigned int num_addr_range_descriptors = dw_addr_range_descriptors.size();
	
	unsigned int i;
	
	for(i = 0; i < num_addr_range_descriptors; i++)
	{
		 dw_addr_range_descriptors[i]->to_HTML(os) << std::endl;
	}
	os << "</table></td>";
	os << "</tr>" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_AddressRanges<MEMORY_ADDR>& dw_aranges)
{
	os << "Address Ranges:" << std::endl;
	os << " - Unit length: " << dw_aranges.unit_length << std::endl;
	os << " - Version: " << dw_aranges.version << std::endl;
	os << " - Offset in .debug_info: " << dw_aranges.debug_info_offset << std::endl;
	os << " - Address size: " << (uint32_t) dw_aranges.address_size << std::endl;
	os << " - Segment size: " << (uint32_t) dw_aranges.segment_size << std::endl;
	os << " - Address range descriptors: " << std::endl;

	unsigned int num_addr_range_descriptors = dw_aranges.dw_addr_range_descriptors.size();
	
	unsigned int i;
	
	for(i = 0; i < num_addr_range_descriptors; i++)
	{
		 os << "   - Descriptor #" << i << ":" << std::endl;
		 os << *dw_aranges.dw_addr_range_descriptors[i] << std::endl;
	}
	return os;
}

template <class MEMORY_ADDR>
DWARF_Pub<MEMORY_ADDR>::DWARF_Pub(const DWARF_Pubs<MEMORY_ADDR> *_dw_pubs)
	: dw_pubs(_dw_pubs)
	, dw_die(0)
	, debug_info_offset(0xffffffffffffffffULL)
	, name(0)
{
}

template <class MEMORY_ADDR>
DWARF_Pub<MEMORY_ADDR>::~DWARF_Pub()
{
}

template <class MEMORY_ADDR>
int64_t DWARF_Pub<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size)
{
	DWARF_Format dw_fmt = dw_pubs->GetFormat();
	endian_type endianness = dw_pubs->GetEndianness();
	int64_t size = 0;
	
	if(dw_fmt == FMT_DWARF64)
	{
		uint64_t debug_info_offset64;
	
		if(max_size < sizeof(debug_info_offset64)) return -1;
		memcpy(&debug_info_offset64, rawdata, sizeof(debug_info_offset64));
		debug_info_offset64 = Target2Host(endianness, debug_info_offset64);
		rawdata += sizeof(debug_info_offset64);
		max_size -= sizeof(debug_info_offset64);
		size += sizeof(debug_info_offset64);
		debug_info_offset = debug_info_offset64;
	}
	else
	{
		uint32_t debug_info_offset32;
	
		if(max_size < sizeof(debug_info_offset32)) return -1;
		memcpy(&debug_info_offset32, rawdata, sizeof(debug_info_offset32));
		debug_info_offset32 = Target2Host(endianness, debug_info_offset32);
		rawdata += sizeof(debug_info_offset32);
		max_size -= sizeof(debug_info_offset32);
		size += sizeof(debug_info_offset32);
		debug_info_offset = debug_info_offset32;
	}

	if(debug_info_offset == 0) return size;
	
	name = (const char *) rawdata;
	uint64_t sz = strlen(name) + 1;
	if(max_size < sz) return -1;
	size += sz;
	rawdata += sz;
	max_size -= sz;

	return size;
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_Pub<MEMORY_ADDR>::GetDIE() const
{
	return dw_die;
}

template <class MEMORY_ADDR>
bool DWARF_Pub<MEMORY_ADDR>::IsNull() const
{
	return debug_info_offset == 0;
}

template <class MEMORY_ADDR>
void DWARF_Pub<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler)
{
	uint64_t cu_debug_info_offset = dw_pubs->GetDebugInfoOffset();
	dw_die = dw_handler->FindDIE(cu_debug_info_offset + debug_info_offset);
	if(!dw_die)
	{
		std::cerr << "Can't find DIE at offset " << (cu_debug_info_offset + debug_info_offset) << std::endl;
	}
}

template <class MEMORY_ADDR>
std::ostream& DWARF_Pub<MEMORY_ADDR>::to_XML(std::ostream& os) const
{
	uint64_t cu_debug_info_offset = dw_pubs->GetDebugInfoOffset();
	os << "<DW_PUB abs_debug_info_offset=\"" << (cu_debug_info_offset + debug_info_offset) << "\" debug_info_offset=\"" << debug_info_offset << "\" name=\"" << name << "\"/>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_Pub<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	os << "<tr><td>" << name << "</td><td><a href=\"../" << dw_die->GetHREF() << "\">die-" << dw_die->GetId() << "</a></td></tr>" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_Pub<MEMORY_ADDR>& dw_pub)
{
	os << "   - Offset in .debug_info (rel. to CU): " << dw_pub.debug_info_offset << std::endl;
	os << "   - Name: \"" << dw_pub.name << "\"";
	return os;
}

template <class MEMORY_ADDR>
DWARF_Pubs<MEMORY_ADDR>::DWARF_Pubs(DWARF_Handler<MEMORY_ADDR> *_dw_handler)
	: dw_handler(_dw_handler)
	, dw_fmt(FMT_DWARF32)
	, offset(0xffffffffffffffffULL)
	, dw_cu(0)
	, unit_length(0)
	, version(0)
	, debug_info_offset(0xffffffffffffffffULL)
	, debug_info_length(0)
	, dw_pubs()
{
}

template <class MEMORY_ADDR>
DWARF_Pubs<MEMORY_ADDR>::~DWARF_Pubs()
{
	unsigned int num_pubs = dw_pubs.size();
	
	unsigned int i;
	for(i = 0; i < num_pubs; i++)
	{
		delete dw_pubs[i];
	}
}

template <class MEMORY_ADDR>
uint64_t DWARF_Pubs<MEMORY_ADDR>::GetDebugInfoOffset() const
{
	return debug_info_offset;
}

template <class MEMORY_ADDR>
endian_type DWARF_Pubs<MEMORY_ADDR>::GetEndianness() const
{
	return dw_handler->GetEndianness();
}

template <class MEMORY_ADDR>
DWARF_Format DWARF_Pubs<MEMORY_ADDR>::GetFormat() const
{
	return dw_fmt;
}

template <class MEMORY_ADDR>
int64_t DWARF_Pubs<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size)
{
	endian_type endianness = dw_handler->GetEndianness();
	uint32_t unit_length32;
	
	uint64_t size = 0;
	if(max_size < sizeof(unit_length32)) return -1;
	memcpy(&unit_length32, rawdata, sizeof(unit_length32));
	unit_length32 = Target2Host(endianness, unit_length32);
	rawdata += sizeof(unit_length32);
	max_size -= sizeof(unit_length32);
	size += sizeof(unit_length32);
	
	dw_fmt = (unit_length32 == 0xffffffffUL) ? FMT_DWARF64 : FMT_DWARF32;
	
	if(dw_fmt == FMT_DWARF64)
	{
		uint64_t unit_length64;
		if(max_size < sizeof(unit_length64)) return -1;
		memcpy(&unit_length64, rawdata, sizeof(unit_length64));
		unit_length64 = Target2Host(endianness, unit_length64);
		rawdata += sizeof(unit_length64);
		max_size -= sizeof(unit_length64);
		size += sizeof(unit_length64);
		unit_length = unit_length64;
	}
	else
	{
		unit_length = unit_length32;
	}
	
	if(max_size < unit_length) return -1;
	
	if(max_size < sizeof(version)) return -1;
	memcpy(&version, rawdata, sizeof(version));
	version = Target2Host(endianness, version);
	rawdata += sizeof(version);
	max_size -= sizeof(version);
	size += sizeof(version);

	if(version != 2) return -1; // 2=(DWARF2 or DWARF3)
	
	if(dw_fmt == FMT_DWARF64)
	{
		uint64_t debug_info_offset64;
	
		if(max_size < sizeof(debug_info_offset64)) return -1;
		memcpy(&debug_info_offset64, rawdata, sizeof(debug_info_offset64));
		debug_info_offset64 = Target2Host(endianness, debug_info_offset64);
		rawdata += sizeof(debug_info_offset64);
		max_size -= sizeof(debug_info_offset64);
		size += sizeof(debug_info_offset64);
		debug_info_offset = debug_info_offset64;

		uint64_t debug_info_length64;
	
		if(max_size < sizeof(debug_info_length64)) return -1;
		memcpy(&debug_info_length64, rawdata, sizeof(debug_info_length64));
		debug_info_length64 = Target2Host(endianness, debug_info_length64);
		rawdata += sizeof(debug_info_length64);
		max_size -= sizeof(debug_info_length64);
		size += sizeof(debug_info_length64);
		debug_info_length = debug_info_length64;
	}
	else
	{
		uint32_t debug_info_offset32;
	
		if(max_size < sizeof(debug_info_offset32)) return -1;
		memcpy(&debug_info_offset32, rawdata, sizeof(debug_info_offset32));
		debug_info_offset32 = Target2Host(endianness, debug_info_offset32);
		rawdata += sizeof(debug_info_offset32);
		max_size -= sizeof(debug_info_offset32);
		size += sizeof(debug_info_offset32);
		debug_info_offset = debug_info_offset32;

		uint32_t debug_info_length32;
	
		if(max_size < sizeof(debug_info_length32)) return -1;
		memcpy(&debug_info_length32, rawdata, sizeof(debug_info_length32));
		debug_info_length32 = Target2Host(endianness, debug_info_length32);
		rawdata += sizeof(debug_info_length32);
		max_size -= sizeof(debug_info_length32);
		size += sizeof(debug_info_length32);
		debug_info_length = debug_info_length32;
	}

	do
	{
		DWARF_Pub<MEMORY_ADDR> *dw_pub = new DWARF_Pub<MEMORY_ADDR>(this);
		
		int64_t sz;
		if((sz = dw_pub->Load(rawdata, max_size)) < 0)
		{
			delete dw_pub;
			return -1;
		}
		rawdata += sz;
		max_size -= sz;
		size += sz;
		
		if(dw_pub->IsNull())
		{
			delete dw_pub;
			break;
		}
		
		dw_pubs.push_back(dw_pub);
	}
	while(size < ((dw_fmt == FMT_DWARF64) ? (unit_length + sizeof(uint32_t) + sizeof(uint64_t)) : (unit_length + sizeof(uint32_t))));
	
	size = (dw_fmt == FMT_DWARF64) ? (unit_length + sizeof(uint32_t) + sizeof(uint64_t)) : (unit_length + sizeof(uint32_t));
	
	return size;
}

template <class MEMORY_ADDR>
void DWARF_Pubs<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler)
{
	dw_cu = dw_handler->FindCompilationUnit(debug_info_offset);
	if(!dw_cu)
	{
		std::cerr << "Can't find compilation unit at offset " << debug_info_offset << std::endl;
	}
	
	unsigned int num_pubs = dw_pubs.size();
	
	unsigned int i;
	for(i = 0; i < num_pubs; i++)
	{
		dw_pubs[i]->Fix(dw_handler);
	}
}

template <class MEMORY_ADDR>
std::ostream& DWARF_Pubs<MEMORY_ADDR>::to_XML(std::ostream& os) const
{
	os << "<DW_PUBS unit_length=\"" << unit_length << "\" version=\"" << version << "\" debug_info_offset=\"" << debug_info_offset << "\" debug_info_length=\"" << debug_info_length << "\">" << std::endl;
	unsigned int num_pubs = dw_pubs.size();
	
	unsigned int i;
	for(i = 0; i < num_pubs; i++)
	{
		dw_pubs[i]->to_XML(os);
		os << std::endl;
	}
	os << "</DW_PUBS>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_Pubs<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	os << "<tr><td>" << version << "</td><td><a href=\"../" << dw_cu->GetHREF() << "\">cu-" << dw_cu->GetId() << "</a></td><td><table>";
	unsigned int num_pubs = dw_pubs.size();
	
	unsigned int i;
	for(i = 0; i < num_pubs; i++)
	{
		dw_pubs[i]->to_HTML(os);
		os << std::endl;
	}
	os << "</table></td></tr>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_Pubs<MEMORY_ADDR>& dw_pubs)
{
	os << "Pubs:" << std::endl;
	os << " - Unit length: " << dw_pubs.unit_length << std::endl;
	os << " - Version: " << dw_pubs.version << std::endl;
	os << " - Offset in .debug_info: " << dw_pubs.debug_info_offset << std::endl;
	os << " - Length in .debug_info: " << dw_pubs.debug_info_length << std::endl;
	unsigned int num_pubs = dw_pubs.dw_pubs.size();
	
	unsigned int i;
	for(i = 0; i < num_pubs; i++)
	{
		os << *dw_pubs.dw_pubs[i] << std::endl;
	}
	return os;
}

template <class MEMORY_ADDR>
DWARF_LocListEntry<MEMORY_ADDR>::DWARF_LocListEntry(const DWARF_CompilationUnit<MEMORY_ADDR> *_dw_cu)
	: offset(0xffffffffffffffffULL)
	, dw_cu(_dw_cu)
	, next(0)
	, begin_addr_offset(0)
	, end_addr_offset(0)
	, dw_expr(0)
{
}

template <class MEMORY_ADDR>
DWARF_LocListEntry<MEMORY_ADDR>::~DWARF_LocListEntry()
{
	if(dw_expr)
	{
		delete dw_expr;
	}
}

template <class MEMORY_ADDR>
MEMORY_ADDR DWARF_LocListEntry<MEMORY_ADDR>::GetBeginAddressOffset() const
{
	return begin_addr_offset;
}

template <class MEMORY_ADDR>
MEMORY_ADDR DWARF_LocListEntry<MEMORY_ADDR>::GetEndAddressOffset() const
{
	return end_addr_offset;
}

template <class MEMORY_ADDR>
bool DWARF_LocListEntry<MEMORY_ADDR>::IsBaseAddressSelection() const
{
	return begin_addr_offset > end_addr_offset;
}

template <class MEMORY_ADDR>
bool DWARF_LocListEntry<MEMORY_ADDR>::IsEndOfList() const
{
	return (begin_addr_offset == 0) && (end_addr_offset == 0);
}

template <class MEMORY_ADDR>
uint64_t DWARF_LocListEntry<MEMORY_ADDR>::GetOffset() const
{
	return offset;
}

template <class MEMORY_ADDR>
void DWARF_LocListEntry<MEMORY_ADDR>::Fix(DWARF_Handler<MEMORY_ADDR> *dw_handler, unsigned int _id)
{
	id = _id;
}

template <class MEMORY_ADDR>
std::string DWARF_LocListEntry<MEMORY_ADDR>::GetHREF() const
{
	std::stringstream sstr;
	sstr << "debug_loc/" << (id / debug_range_per_file) << ".html#loc-" << id;
	return sstr.str();
}

template <class MEMORY_ADDR>
unsigned int DWARF_LocListEntry<MEMORY_ADDR>::GetId() const
{
	return id;
}

template <class MEMORY_ADDR>
const DWARF_LocListEntry<MEMORY_ADDR> *DWARF_LocListEntry<MEMORY_ADDR>::GetNext() const
{
	return next;
}

template <class MEMORY_ADDR>
int64_t DWARF_LocListEntry<MEMORY_ADDR>::Load(const uint8_t *rawdata, uint64_t max_size, uint64_t _offset)
{
	offset = _offset;
	int64_t size = 0;
	
	endian_type endianness = dw_cu->GetEndianness();
	
	switch(dw_cu->GetAddressSize())
	{
		case 2:
			{
				uint16_t begin_addr_offset16;
				if(max_size < sizeof(begin_addr_offset16)) return -1;
				memcpy(&begin_addr_offset16, rawdata, sizeof(begin_addr_offset16));
				begin_addr_offset16 = Target2Host(endianness, begin_addr_offset16);
				rawdata += sizeof(begin_addr_offset16);
				max_size -= sizeof(begin_addr_offset16);
				size += sizeof(begin_addr_offset16);
				begin_addr_offset = (MEMORY_ADDR) begin_addr_offset16;

				uint16_t end_addr_offset16;
				if(max_size < sizeof(end_addr_offset16)) return -1;
				memcpy(&end_addr_offset16, rawdata, sizeof(end_addr_offset16));
				end_addr_offset16 = Target2Host(endianness, end_addr_offset16);
				rawdata += sizeof(end_addr_offset16);
				max_size -= sizeof(end_addr_offset16);
				size += sizeof(end_addr_offset16);
				end_addr_offset = (MEMORY_ADDR) end_addr_offset16;
			}
			break;
		case 4:
			{
				uint32_t begin_addr_offset32;
				if(max_size < sizeof(begin_addr_offset32)) return -1;
				memcpy(&begin_addr_offset32, rawdata, sizeof(begin_addr_offset32));
				begin_addr_offset32 = Target2Host(endianness, begin_addr_offset32);
				rawdata += sizeof(begin_addr_offset32);
				max_size -= sizeof(begin_addr_offset32);
				size += sizeof(begin_addr_offset32);
				begin_addr_offset = (MEMORY_ADDR) begin_addr_offset32;

				uint32_t end_addr_offset32;
				if(max_size < sizeof(end_addr_offset32)) return -1;
				memcpy(&end_addr_offset32, rawdata, sizeof(end_addr_offset32));
				end_addr_offset32 = Target2Host(endianness, end_addr_offset32);
				rawdata += sizeof(end_addr_offset32);
				max_size -= sizeof(end_addr_offset32);
				size += sizeof(end_addr_offset32);
				end_addr_offset = (MEMORY_ADDR) end_addr_offset32;
			}
			break;
		case 8:
			{
				uint64_t begin_addr_offset64;
				if(max_size < sizeof(begin_addr_offset64)) return -1;
				memcpy(&begin_addr_offset64, rawdata, sizeof(begin_addr_offset64));
				begin_addr_offset64 = Target2Host(endianness, begin_addr_offset64);
				rawdata += sizeof(begin_addr_offset64);
				max_size -= sizeof(begin_addr_offset64);
				size += sizeof(begin_addr_offset64);
				begin_addr_offset = (MEMORY_ADDR) begin_addr_offset64;

				uint64_t end_addr_offset64;
				if(max_size < sizeof(end_addr_offset64)) return -1;
				memcpy(&end_addr_offset64, rawdata, sizeof(end_addr_offset64));
				end_addr_offset64 = Target2Host(endianness, end_addr_offset64);
				rawdata += sizeof(end_addr_offset64);
				max_size -= sizeof(end_addr_offset64);
				size += sizeof(end_addr_offset64);
				end_addr_offset = (MEMORY_ADDR) end_addr_offset64;
			}
			break;
		default:
			return -1;
	}

	if(IsBaseAddressSelection() || IsEndOfList()) return size;
	
	// Warning! whether block_length is a 2-byte unsigned integer or an unsigned LEB128 as I initially expected is undocumented both in DWARF v2 and v3 specifications.
	uint16_t block_length;
	if(max_size < sizeof(block_length)) return -1;
	memcpy(&block_length, rawdata, sizeof(block_length));
	block_length = Target2Host(endianness, block_length);
	rawdata += sizeof(block_length);
	max_size -= sizeof(block_length);
	size += sizeof(block_length);

	if(max_size < block_length) return -1;
	dw_expr = new DWARF_Expression<MEMORY_ADDR>(dw_cu, block_length, rawdata);
	rawdata += block_length;
	size += block_length;
	max_size -= block_length;
	
	return size;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_LocListEntry<MEMORY_ADDR>::to_XML(std::ostream& os) const
{
	if(IsEndOfList()) return os << "<DW_EOL offset=\"" << offset << "\"/>";
	if(IsBaseAddressSelection())
	{
		os << "<DW_BASE_ADDRESS_SELECTION offset=\"" << offset << "\" largest_address_offset=\"0x" << std::hex << begin_addr_offset << std::dec << "\" address=\"0x" << std::hex << end_addr_offset << std::dec << "\"/>";
		return os;
	}
	os << "<DW_LOC offset=\"" << offset << "\" begin_address_offset=\"0x" << std::hex << begin_addr_offset << std::dec << "\" end_address_offset=\"0x" << std::hex << end_addr_offset << std::dec << "\"";
	if(dw_expr)
	{
		os << " expression=\"";
		c_string_to_XML(os, dw_expr->to_string().c_str());
		os << "\"";
	}
	os << "/>";
	return os;
}

template <class MEMORY_ADDR>
std::ostream& DWARF_LocListEntry<MEMORY_ADDR>::to_HTML(std::ostream& os) const
{
	os << "<tr id=\"loc-" << id << "\">" << std::endl;
	if(IsEndOfList())
	{
		os << "<td>End-of-List</td>";
	}
	else if(IsBaseAddressSelection())
	{
		os << "<td>Base address selection</td>";
	}
	else
	{
		os << "<td>Location</td>";
	}
	os << "<td>" << id << "</td>" << std::endl;
	os << "<td>" << offset << "</td>" << std::endl;
	os << "<td>";
	if(next)
	{
		os << "<a href=\"../" << next->GetHREF() << "\">loc-" << next->GetId() << "</a>" << std::endl;
	}
	else
	{
		os << "-";
	}
	os << "</td>";
	if(IsEndOfList())
	{
		os << "<td>-</td>";
	}
	else if(IsBaseAddressSelection())
	{
		os << "<td><table><tr><th>Largest address offset</th><th>Address</th></tr>";
		os << "<tr><td>0x" << std::hex << begin_addr_offset << std::dec << "</td><td>0x" << std::hex << end_addr_offset << std::dec << "</td></tr></table></td>";
	}
	else
	{
		os << "<td><table><tr><th>Beginning address offset</th><th>End Address offset</th><th>Expression</th></tr>";
		os << "<tr><td>0x" << std::hex << begin_addr_offset << std::dec << "</td><td>0x" << std::hex << end_addr_offset << std::dec << "</td><td>";
		c_string_to_HTML(os, dw_expr->to_string().c_str());
		os << "</td></tr></table></td>";
	}
	os << "</tr>" << std::endl;
	return os;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_LocListEntry<MEMORY_ADDR>& dw_loc_list_entry)
{
	if(dw_loc_list_entry.IsEndOfList()) return os << "EOL";
	if(dw_loc_list_entry.IsBaseAddressSelection()) os << "Base";
	os << "[0x" << std::hex << dw_loc_list_entry.begin_addr_offset << "-0x" << dw_loc_list_entry.end_addr_offset;
	if(dw_loc_list_entry.dw_expr)
	{
		os << ":" << *dw_loc_list_entry.dw_expr;
	}
	os << "]";
	return os;
}

template <class MEMORY_ADDR>
DWARF_Handler<MEMORY_ADDR>::DWARF_Handler(endian_type _endianness, uint8_t _address_size)
	: endianness(_endianness)
	, address_size(_address_size)
	, debug_line_section(0)
	, debug_line_section_size(0)
	, debug_info_section(0)
	, debug_info_section_size(0)
	, debug_abbrev_section(0)
	, debug_abbrev_section_size(0)
	, debug_aranges_section(0)
	, debug_aranges_section_size(0)
	, debug_pubnames_section(0)
	, debug_pubnames_section_size(0)
	, debug_pubtypes_section(0)
	, debug_pubtypes_section_size(0)
	, debug_macinfo_section(0)
	, debug_macinfo_section_size(0)
	, debug_frame_section(0)
	, debug_frame_section_size(0)
	, debug_str_section(0)
	, debug_str_section_size(0)
	, debug_loc_section(0)
	, debug_loc_section_size(0)
	, debug_ranges_section(0)
	, debug_ranges_section_size(0)
{
}

template <class MEMORY_ADDR>
DWARF_Handler<MEMORY_ADDR>::~DWARF_Handler()
{
	unsigned int i;

	typename std::map<MEMORY_ADDR, Statement<MEMORY_ADDR> *>::const_iterator stmt_iter;

	for(stmt_iter = stmt_matrix.begin(); stmt_iter != stmt_matrix.end(); stmt_iter++)
	{
		if((*stmt_iter).second)
		{
			delete (*stmt_iter).second;
		}
	}

	typename std::map<uint64_t, DWARF_StatementProgram<MEMORY_ADDR> *>::iterator dw_stmt_prog_iter;
	for(dw_stmt_prog_iter = dw_stmt_progs.begin(); dw_stmt_prog_iter != dw_stmt_progs.end(); dw_stmt_prog_iter++)
	{
		if((*dw_stmt_prog_iter).second)
		{
			delete (*dw_stmt_prog_iter).second;
		}
	}

	typename std::map<uint64_t, DWARF_Abbrev *>::iterator dw_abbrev_iter;
	for(dw_abbrev_iter = dw_abbrevs.begin(); dw_abbrev_iter != dw_abbrevs.end(); dw_abbrev_iter++)
	{
		if((*dw_abbrev_iter).second)
		{
			delete (*dw_abbrev_iter).second;
		}
	}

	typename std::map<uint64_t, DWARF_CompilationUnit<MEMORY_ADDR> *>::iterator dw_cu_iter;
	for(dw_cu_iter = dw_cus.begin(); dw_cu_iter != dw_cus.end(); dw_cu_iter++)
	{
		if((*dw_cu_iter).second)
		{
			delete (*dw_cu_iter).second;
		}
	}

	unsigned int num_fdes = dw_fdes.size();
	for(i = 0; i < num_fdes; i++)
	{
		delete dw_fdes[i];
	}

	typename std::map<uint64_t, DWARF_CIE<MEMORY_ADDR> *>::iterator dw_cie_iter;
	for(dw_cie_iter = dw_cies.begin(); dw_cie_iter != dw_cies.end(); dw_cie_iter++)
	{
		if((*dw_cie_iter).second)
		{
			delete (*dw_cie_iter).second;
		}
	}

	typename std::map<uint64_t, DWARF_RangeListEntry<MEMORY_ADDR> *>::iterator dw_range_list_entry_iter;
	for(dw_range_list_entry_iter = dw_range_list.begin(); dw_range_list_entry_iter != dw_range_list.end(); dw_range_list_entry_iter++)
	{
		if((*dw_range_list_entry_iter).second)
		{
			delete (*dw_range_list_entry_iter).second;
		}
	}

	typename std::map<uint64_t, DWARF_MacInfoListEntry<MEMORY_ADDR> *>::iterator dw_macinfo_list_entry_iter;
	for(dw_macinfo_list_entry_iter = dw_macinfo_list.begin(); dw_macinfo_list_entry_iter != dw_macinfo_list.end(); dw_macinfo_list_entry_iter++)
	{
		if((*dw_macinfo_list_entry_iter).second)
		{
			delete (*dw_macinfo_list_entry_iter).second;
		}
	}

	unsigned int num_aranges = dw_aranges.size();
	for(i = 0; i < num_aranges; i++)
	{
		delete dw_aranges[i];
	}

	unsigned int num_pubnames = dw_pubnames.size();
	for(i = 0; i < num_pubnames; i++)
	{
		delete dw_pubnames[i];
	}

	unsigned int num_pubtypes = dw_pubtypes.size();
	for(unsigned int i = 0; i < num_pubtypes; i++)
	{
		delete dw_pubtypes[i];
	}

	typename std::map<uint64_t, DWARF_LocListEntry<MEMORY_ADDR> *>::iterator dw_loc_list_entry_iter;
	for(dw_loc_list_entry_iter = dw_loc_list.begin(); dw_loc_list_entry_iter != dw_loc_list.end(); dw_loc_list_entry_iter++)
	{
		if((*dw_loc_list_entry_iter).second)
		{
			delete (*dw_loc_list_entry_iter).second;
		}
	}

	if(debug_line_section)
	{
		free(debug_line_section);
	}
	
	if(debug_info_section)
	{
		free(debug_info_section);
	}
	
	if(debug_abbrev_section)
	{
		free(debug_abbrev_section);
	}
	
	if(debug_aranges_section)
	{
		free(debug_aranges_section);
	}
	
	if(debug_pubnames_section)
	{
		free(debug_pubnames_section);
	}
	
	if(debug_pubtypes_section)
	{
		free(debug_pubtypes_section);
	}
	
	if(debug_macinfo_section)
	{
		free(debug_macinfo_section);
	}
	
	if(debug_frame_section)
	{
		free(debug_frame_section);
	}
	
	if(debug_str_section)
	{
		free(debug_str_section);
	}
	
	if(debug_loc_section)
	{
		free(debug_loc_section);
	}
	
	if(debug_ranges_section)
	{
		free(debug_ranges_section);
	}
}

template <class MEMORY_ADDR>
const DWARF_Abbrev *DWARF_Handler<MEMORY_ADDR>::FindAbbrev(uint64_t debug_abbrev_offset, const DWARF_LEB128& dw_abbrev_code) const
{
	std::map<uint64_t, DWARF_Abbrev *>::const_iterator abbrev_iter;

	for(abbrev_iter = dw_abbrevs.find(debug_abbrev_offset); abbrev_iter != dw_abbrevs.end(); abbrev_iter++)
	{
		const DWARF_Abbrev *dw_abbrev = (*abbrev_iter).second;
		
		if(dw_abbrev->GetAbbrevCode() == dw_abbrev_code)
		{
			return dw_abbrev;
		}
	}
	return 0;
}

template <class MEMORY_ADDR>
const char *DWARF_Handler<MEMORY_ADDR>::GetString(uint64_t debug_str_offset) const
{
	return (debug_str_offset < debug_str_section_size) ? (const char *) debug_str_section + debug_str_offset : 0;
}

template <class MEMORY_ADDR>
void DWARF_Handler<MEMORY_ADDR>::Handle(const char *section_name, uint8_t *section, uint64_t section_size)
{
	if(!debug_line_section && strcmp(section_name, ".debug_line") == 0)
	{
		debug_line_section = section;
		debug_line_section_size = section_size;
	}
	else if(!debug_info_section && strcmp(section_name, ".debug_info") == 0)
	{
		debug_info_section = section;
		debug_info_section_size = section_size;
	}
	else if(!debug_abbrev_section && strcmp(section_name, ".debug_abbrev") == 0)
	{
		debug_abbrev_section = section;
		debug_abbrev_section_size = section_size;
	}
	else if(!debug_aranges_section && strcmp(section_name, ".debug_aranges") == 0)
	{
		debug_aranges_section = section;
		debug_aranges_section_size = section_size;
	}
	else if(!debug_pubnames_section && strcmp(section_name, ".debug_pubnames") == 0)
	{
		debug_pubnames_section = section;
		debug_pubnames_section_size = section_size;
	}
	else if(!debug_pubtypes_section && strcmp(section_name, ".debug_pubtypes") == 0)
	{
		debug_pubtypes_section = section;
		debug_pubtypes_section_size = section_size;
	}
	else if(!debug_macinfo_section && strcmp(section_name, ".debug_macinfo") == 0)
	{
		debug_macinfo_section = section;
		debug_macinfo_section_size = section_size;
	}
	else if(!debug_str_section && strcmp(section_name, ".debug_str") == 0)
	{
		debug_str_section = section;
		debug_str_section_size = section_size;
	}
	else if(!debug_loc_section && strcmp(section_name, ".debug_loc") == 0)
	{
		debug_loc_section = section;
		debug_loc_section_size = section_size;
	}
	else if(!debug_ranges_section && strcmp(section_name, ".debug_ranges") == 0)
	{
		debug_ranges_section = section;
		debug_ranges_section_size = section_size;
	}
	else if(!debug_frame_section && strcmp(section_name, ".debug_frame") == 0)
	{
		debug_frame_section = section;
		debug_frame_section_size = section_size;
	}
}

template <class MEMORY_ADDR>
void DWARF_Handler<MEMORY_ADDR>::Initialize()
{
	unsigned int i;
	
/*	if(debug_line_section)
	{
		uint64_t debug_line_offset = 0;
		do
		{
			DWARF_StatementProgram<MEMORY_ADDR> *dw_stmt_prog = new DWARF_StatementProgram<MEMORY_ADDR>(this);
			int64_t sz;
			if((sz = dw_stmt_prog->Load((const uint8_t *) debug_line_section + debug_line_offset, debug_line_section_size - debug_line_offset, debug_line_offset)) < 0)
			{
				std::cerr << "Invalid DWARF2 statement program prologue at offset 0x" << std::hex << debug_line_offset << std::dec << std::endl;
				delete dw_stmt_prog;
				break;
			}
			else
			{
				Register(dw_stmt_prog);

				//std::cerr << *dw_stmt_prog << std::endl;
				debug_line_offset += sz;

				DWARF_StatementVM<MEMORY_ADDR> dw_stmt_vm = DWARF_StatementVM<MEMORY_ADDR>();

				if(!dw_stmt_vm.Run(dw_stmt_prog, 0, &stmt_matrix))
				{
					std::cerr << "Invalid DWARF2 statement program. Statement matrix may be incomplete." << std::endl;
				}
			}
		}
		while(debug_line_offset < debug_line_section_size);
	}*/
	
	if(debug_abbrev_section)
	{
		uint64_t debug_abbrev_offset = 0;
		do
		{
			DWARF_Abbrev *dw_abbrev = new DWARF_Abbrev(endianness);
			int64_t sz;
			if((sz = dw_abbrev->Load((const uint8_t *) debug_abbrev_section + debug_abbrev_offset, debug_abbrev_section_size - debug_abbrev_offset, debug_abbrev_offset)) < 0)
			{
				std::cerr << "Invalid DWARF2 abbreviation informations at offset 0x" << std::hex << debug_abbrev_offset << std::dec << std::endl;
				delete dw_abbrev;
				break;
			}
			else
			{
				if(dw_abbrev->IsNull()) // Null entries are for padding to align section on some architectures
				{
					delete dw_abbrev;
				}
				else
				{
					//std::cerr << *dw_abbrev << std::endl;
					dw_abbrevs.insert(std::pair<uint64_t, DWARF_Abbrev *>(debug_abbrev_offset, dw_abbrev));
				}
				debug_abbrev_offset += sz;
			}
		}
		while(debug_abbrev_offset < debug_abbrev_section_size);
	}

	if(debug_info_section)
	{
		uint64_t debug_info_offset = 0;
		do
		{
			DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu = new DWARF_CompilationUnit<MEMORY_ADDR>(this);
			int64_t sz;
			if((sz = dw_cu->Load((const uint8_t *) debug_info_section + debug_info_offset, debug_info_section_size - debug_info_offset, debug_info_offset)) < 0)
			{
				std::cerr << "Invalid DWARF2 debug informations at offset 0x" << std::hex << debug_info_offset << std::dec << std::endl;
				delete dw_cu;
				break;
			}
			else
			{
				//std::cerr << *dw_cu << std::endl;
				dw_cus.insert(std::pair<uint64_t, DWARF_CompilationUnit<MEMORY_ADDR> *>(dw_cu->GetOffset(), dw_cu));
				debug_info_offset += sz;
			}
		}
		while(debug_info_offset < debug_info_section_size);
	}

	if(debug_frame_section)
	{
		uint64_t debug_frame_offset = 0;
		int64_t sz;
		do
		{
			DWARF_FDE<MEMORY_ADDR> *dw_fde = new DWARF_FDE<MEMORY_ADDR>(this);
			
			if((sz = dw_fde->Load(debug_frame_section + debug_frame_offset, debug_frame_section_size - debug_frame_offset, debug_frame_offset)) < 0)
			{
				delete dw_fde;
				DWARF_CIE<MEMORY_ADDR> *dw_cie = new DWARF_CIE<MEMORY_ADDR>(this);
			
				if((sz = dw_cie->Load(debug_frame_section + debug_frame_offset, debug_frame_section_size - debug_frame_offset, debug_frame_offset)) < 0)
				{
					delete dw_cie;
					
					std::cerr << "Invalid DWARF2 debug frame at offset 0x" << std::hex << debug_frame_offset << std::dec << std::endl;
					break;
				}
				
				//std::cerr << *dw_cie << std::endl;
				dw_cies.insert(std::pair<uint64_t, DWARF_CIE<MEMORY_ADDR> *>(debug_frame_offset, dw_cie));
			}
			else
			{
				//std::cerr << *dw_fde << std::endl;
				dw_fdes.push_back(dw_fde);
			}
			
			debug_frame_offset += sz;
		}
		while(debug_frame_offset < debug_frame_section_size);
	}

	if(debug_aranges_section)
	{
		uint64_t debug_aranges_offset = 0;
		do
		{
			DWARF_AddressRanges<MEMORY_ADDR> *dw_address_ranges = new DWARF_AddressRanges<MEMORY_ADDR>(this);
			int64_t sz;
			if((sz = dw_address_ranges->Load((const uint8_t *) debug_aranges_section + debug_aranges_offset, debug_aranges_section_size - debug_aranges_offset)) < 0)
			{
				std::cerr << "Invalid DWARF2 debug aranges at offset 0x" << std::hex << debug_aranges_offset << std::dec << std::endl;
				delete dw_address_ranges;
				break;
			}
			else
			{
				//std::cerr << *dw_address_ranges << std::endl;
				dw_aranges.push_back(dw_address_ranges);
				debug_aranges_offset += sz;
			}
		}
		while(debug_aranges_offset < debug_aranges_section_size);
	}

	if(debug_pubnames_section)
	{
		uint64_t debug_pubnames_offset = 0;
		do
		{
			DWARF_Pubs<MEMORY_ADDR> *dw_public_names = new DWARF_Pubs<MEMORY_ADDR>(this);
			int64_t sz;
			if((sz = dw_public_names->Load((const uint8_t *) debug_pubnames_section + debug_pubnames_offset, debug_pubnames_section_size - debug_pubnames_offset)) < 0)
			{
				std::cerr << "Invalid DWARF2 debug pubnames at offset 0x" << std::hex << debug_pubnames_offset << std::dec << std::endl;
				delete dw_public_names;
				break;
			}
			else
			{
				//std::cerr << *dw_public_names << std::endl;
				dw_pubnames.push_back(dw_public_names);
				debug_pubnames_offset += sz;
			}
		}
		while(debug_pubnames_offset < debug_pubnames_section_size);
	}

	if(debug_pubtypes_section)
	{
		uint64_t debug_pubtypes_offset = 0;
		do
		{
			DWARF_Pubs<MEMORY_ADDR> *dw_public_types = new DWARF_Pubs<MEMORY_ADDR>(this);
			int64_t sz;
			if((sz = dw_public_types->Load((const uint8_t *) debug_pubtypes_section + debug_pubtypes_offset, debug_pubtypes_section_size - debug_pubtypes_offset)) < 0)
			{
				std::cerr << "Invalid DWARF2 debug pubtypes at offset 0x" << std::hex << debug_pubtypes_offset << std::dec << std::endl;
				delete dw_public_types;
				break;
			}
			else
			{
				//std::cerr << *dw_public_types << std::endl;
				dw_pubtypes.push_back(dw_public_types);
				debug_pubtypes_offset += sz;
			}
		}
		while(debug_pubtypes_offset < debug_pubtypes_section_size);
	}

	typename std::map<uint64_t, DWARF_CompilationUnit<MEMORY_ADDR> *>::const_iterator dw_cu_iter;
	
	unsigned int dw_cu_id = 0;
	for(dw_cu_iter = dw_cus.begin(); dw_cu_iter != dw_cus.end(); dw_cu_iter++, dw_cu_id++)
	{
		DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu = (*dw_cu_iter).second;
		dw_cu->Fix(this, dw_cu_id);
	}

	typename std::map<uint64_t, DWARF_DIE<MEMORY_ADDR> *>::const_iterator dw_die_iter;
	
	unsigned int dw_die_id = 0;
	for(dw_die_iter = dw_dies.begin(); dw_die_iter != dw_dies.end(); dw_die_iter++, dw_die_id++)
	{
		DWARF_DIE<MEMORY_ADDR> *dw_die = (*dw_die_iter).second;
		dw_die->Fix(this, dw_die_id);
	}

	typename std::map<uint64_t, DWARF_StatementProgram<MEMORY_ADDR> *>::const_iterator dw_stmt_prog_iter;
	
	unsigned int dw_stmt_prog_id = 0;
	for(dw_stmt_prog_iter = dw_stmt_progs.begin(); dw_stmt_prog_iter != dw_stmt_progs.end(); dw_stmt_prog_iter++, dw_stmt_prog_id++)
	{
		DWARF_StatementProgram<MEMORY_ADDR> *dw_stmt_prog = (*dw_stmt_prog_iter).second;
		dw_stmt_prog->Fix(this, dw_stmt_prog_id);
	}

	typename std::map<uint64_t, DWARF_CIE<MEMORY_ADDR> *>::const_iterator dw_cie_iter;
	
	unsigned int dw_cie_id = 0;
	for(dw_cie_iter = dw_cies.begin(); dw_cie_iter != dw_cies.end(); dw_cie_iter++, dw_cie_id++)
	{
		DWARF_CIE<MEMORY_ADDR> *dw_cie = (*dw_cie_iter).second;
		dw_cie->Fix(this, dw_cie_id);
	}

	unsigned int num_fdes = dw_fdes.size();
	for(i = 0; i < num_fdes; i++)
	{
		dw_fdes[i]->Fix(this);
	}

	unsigned int num_aranges = dw_aranges.size();
	for(i = 0; i < num_aranges; i++)
	{
		dw_aranges[i]->Fix(this);
	}

	unsigned int num_pubnames = dw_pubnames.size();
	for(i = 0; i < num_pubnames; i++)
	{
		dw_pubnames[i]->Fix(this);
	}

	unsigned int num_pubtypes = dw_pubtypes.size();
	for(i = 0; i < num_pubtypes; i++)
	{
		dw_pubtypes[i]->Fix(this);
	}
	
	typename std::map<uint64_t, DWARF_RangeListEntry<MEMORY_ADDR> *>::iterator dw_range_list_entry_iter;
	unsigned int dw_range_list_entry_id = 0;
	DWARF_RangeListEntry<MEMORY_ADDR> *dw_prev_range_list_entry = 0;
	for(dw_range_list_entry_iter = dw_range_list.begin(); dw_range_list_entry_iter != dw_range_list.end(); dw_range_list_entry_iter++, dw_range_list_entry_id++)
	{
		DWARF_RangeListEntry<MEMORY_ADDR> *dw_range_list_entry = (*dw_range_list_entry_iter).second;
		
		if(dw_prev_range_list_entry)
		{
			dw_prev_range_list_entry->next = dw_range_list_entry;
		}
		
		dw_range_list_entry->Fix(this, dw_range_list_entry_id);
		
		dw_prev_range_list_entry = dw_range_list_entry;
	}

	typename std::map<uint64_t, DWARF_MacInfoListEntry<MEMORY_ADDR> *>::iterator dw_macinfo_list_entry_iter;
	unsigned int dw_macinfo_list_entry_id = 0;
	DWARF_MacInfoListEntry<MEMORY_ADDR> *dw_prev_macinfo_list_entry = 0;
	for(dw_macinfo_list_entry_iter = dw_macinfo_list.begin(); dw_macinfo_list_entry_iter != dw_macinfo_list.end(); dw_macinfo_list_entry_iter++, dw_macinfo_list_entry_id++)
	{
		DWARF_MacInfoListEntry<MEMORY_ADDR> *dw_macinfo_list_entry = (*dw_macinfo_list_entry_iter).second;
		
		if(dw_prev_macinfo_list_entry)
		{
			dw_prev_macinfo_list_entry->next = dw_macinfo_list_entry;
		}
		
		dw_macinfo_list_entry->Fix(this, dw_macinfo_list_entry_id);
		
		dw_prev_macinfo_list_entry = dw_macinfo_list_entry;
	}

	typename std::map<uint64_t, DWARF_LocListEntry<MEMORY_ADDR> *>::iterator dw_loc_list_entry_iter;
	unsigned int dw_loc_list_entry_id = 0;
	DWARF_LocListEntry<MEMORY_ADDR> *dw_prev_loc_list_entry = 0;
	for(dw_loc_list_entry_iter = dw_loc_list.begin(); dw_loc_list_entry_iter != dw_loc_list.end(); dw_loc_list_entry_iter++, dw_loc_list_entry_id++)
	{
		DWARF_LocListEntry<MEMORY_ADDR> *dw_loc_list_entry = (*dw_loc_list_entry_iter).second;
		
		if(dw_prev_loc_list_entry)
		{
			dw_prev_loc_list_entry->next = dw_loc_list_entry;
		}
		
		dw_loc_list_entry->Fix(this, dw_loc_list_entry_id);

		dw_prev_loc_list_entry = dw_loc_list_entry;
	}
	
	//std::ofstream f("out.xml", std::ios::out);
	//to_XML(f);
}

template <class MEMORY_ADDR>
void DWARF_Handler<MEMORY_ADDR>::to_XML(std::ostream& os)
{
	unsigned int i;
	
	os << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
	os << "<DWARF>" << std::endl;
	os << "<DW_DEBUG_PUBNAMES>" << std::endl;
	unsigned int num_pubnames = dw_pubnames.size();
	for(i = 0; i < num_pubnames; i++)
	{
		dw_pubnames[i]->to_XML(os) << std::endl;
	}
	os << "</DW_DEBUG_PUBNAMES>" << std::endl;
	os << "<DW_DEBUG_PUBTYPES>" << std::endl;
	unsigned int num_pubtypes = dw_pubtypes.size();
	for(i = 0; i < num_pubtypes; i++)
	{
		dw_pubtypes[i]->to_XML(os) << std::endl;
	}
	os << "</DW_DEBUG_PUBTYPESS>" << std::endl;
	os << "<DW_DEBUG_ARANGES>" << std::endl;
	unsigned int num_aranges = dw_aranges.size();
	for(i = 0; i < num_aranges; i++)
	{
		dw_aranges[i]->to_XML(os) << std::endl;
	}
	os << "</DW_DEBUG_ARANGESS>" << std::endl;
	os << "<DW_DEBUG_ABBREV>" << std::endl;
	typename std::map<uint64_t, DWARF_Abbrev *>::iterator dw_abbrev_iter;
	for(dw_abbrev_iter = dw_abbrevs.begin(); dw_abbrev_iter != dw_abbrevs.end(); dw_abbrev_iter++)
	{
		DWARF_Abbrev *dw_abbrev = (*dw_abbrev_iter).second;
		
		dw_abbrev->to_XML(os) << std::endl;
	}
	os << "</DW_DEBUG_ABBREV>" << std::endl;
	os << "<DW_DEBUG_INFO>" << std::endl;
	typename std::map<uint64_t, DWARF_CompilationUnit<MEMORY_ADDR> *>::iterator dw_cu_iter;
	for(dw_cu_iter = dw_cus.begin(); dw_cu_iter != dw_cus.end(); dw_cu_iter++)
	{
		DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu = (*dw_cu_iter).second;
		dw_cu->to_XML(os) << std::endl;
	}
	os << "</DW_DEBUG_INFO>" << std::endl;
	os << "<DW_DEBUG_MACINFO>" << std::endl;
	typename std::map<uint64_t, DWARF_MacInfoListEntry<MEMORY_ADDR> *>::iterator dw_macinfo_list_entry_iter;
	for(dw_macinfo_list_entry_iter = dw_macinfo_list.begin(); dw_macinfo_list_entry_iter != dw_macinfo_list.end(); dw_macinfo_list_entry_iter++)
	{
		DWARF_MacInfoListEntry<MEMORY_ADDR> *dw_macinfo_list_entry = (*dw_macinfo_list_entry_iter).second;
		dw_macinfo_list_entry->to_XML(os) << std::endl;
	}
	os << "</DW_DEBUG_MACINFO>" << std::endl;
	os << "<DW_DEBUG_RANGES>" << std::endl;
	typename std::map<uint64_t, DWARF_RangeListEntry<MEMORY_ADDR> *>::iterator dw_range_list_entry_iter;
	for(dw_range_list_entry_iter = dw_range_list.begin(); dw_range_list_entry_iter != dw_range_list.end(); dw_range_list_entry_iter++)
	{
		DWARF_RangeListEntry<MEMORY_ADDR> *dw_range_list_entry = (*dw_range_list_entry_iter).second;
		dw_range_list_entry->to_XML(os) << std::endl;
	}
	os << "</DW_DEBUG_RANGES>" << std::endl;
	os << "<DW_DEBUG_LOC>" << std::endl;
	typename std::map<uint64_t, DWARF_LocListEntry<MEMORY_ADDR> *>::iterator dw_loc_list_entry_iter;
	for(dw_loc_list_entry_iter = dw_loc_list.begin(); dw_loc_list_entry_iter != dw_loc_list.end(); dw_loc_list_entry_iter++)
	{
		DWARF_LocListEntry<MEMORY_ADDR> *dw_loc_list_entry = (*dw_loc_list_entry_iter).second;
		dw_loc_list_entry->to_XML(os) << std::endl;
	}
	os << "</DW_DEBUG_LOC>" << std::endl;
	os << "<DW_DEBUG_FRAME>" << std::endl;
	typename std::map<uint64_t, DWARF_CIE<MEMORY_ADDR> *>::iterator dw_cie_iter;
	for(dw_cie_iter = dw_cies.begin(); dw_cie_iter != dw_cies.end(); dw_cie_iter++)
	{
		DWARF_CIE<MEMORY_ADDR> *dw_cie = (*dw_cie_iter).second;
		
		dw_cie->to_XML(os) << std::endl;
	}

	unsigned int num_fdes = dw_fdes.size();
	for(i = 0; i < num_fdes; i++)
	{
		dw_fdes[i]->to_XML(os) << std::endl;
	}
	os << "</DW_DEBUG_FRAME>" << std::endl;
	os << "<DW_DEBUG_LINE>" << std::endl;
	typename std::map<uint64_t, DWARF_StatementProgram<MEMORY_ADDR> *>::iterator dw_stmt_prog_iter;
	for(dw_stmt_prog_iter = dw_stmt_progs.begin(); dw_stmt_prog_iter != dw_stmt_progs.end(); dw_stmt_prog_iter++)
	{
		DWARF_StatementProgram<MEMORY_ADDR> *dw_stmt_prog = (*dw_stmt_prog_iter).second;
		
		dw_stmt_prog->to_XML(os) << std::endl;
	}
	os << "</DW_DEBUG_LINE>" << std::endl;
	os << "</DWARF>" << std::endl;
}

template <class MEMORY_ADDR>
void DWARF_Handler<MEMORY_ADDR>::to_HTML(const char *output_dir)
{
	mkdir(output_dir, S_IRWXU);
	std::string index_filename(std::string(output_dir) + "/index.html");
	std::ofstream index(index_filename.c_str(), std::ios::out);
	
	index << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">" << std::endl;
	index << "<html>" << std::endl;
	index << "<head>" << std::endl;
	index << "<meta name=\"description\" content=\"DWARF v2/v3 navigation\">" << std::endl;
	index << "<meta name=\"keywords\" content=\"DWARF\">" << std::endl;
	index << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
	index << "<title>DWARF v2/v3 navigation</title>" << std::endl;
	index << "<style type=\"text/css\">" << std::endl;
	index << "<!--" << std::endl;
	index << "table th { text-align:center; }" << std::endl;
	index << "table th { font-weight:bold; }" << std::endl;
	index << "table td { text-align:left; }" << std::endl;
	index << "table { border-style:solid; }" << std::endl;
	index << "table { border-width:1px; }" << std::endl;
	index << "th, td { border-style:solid; }" << std::endl;
	index << "th, td { border-width:1px; }" << std::endl;
	index << "th, td { border-width:1px; }" << std::endl;
	index << "-->" << std::endl;
	index << "</style>" << std::endl;
	index << "</head>" << std::endl;
	index << "<body>" << std::endl;
	index << "<h1>DWARF v2/v3 navigation</h1>" << std::endl;
	index << "<table>" << std::endl;
	index << "<tr><th>Section</th><th>Description</th></tr>" << std::endl;
	index << "<tr>" << std::endl;
	index << "<td><a href=\"debug_abbrev/0.html\">.debug_abbrev</a></td>" << std::endl;
	index << "<td>Abbreviations Tables</td>" << std::endl;
	index << "</tr>" << std::endl;
	index << "<tr>" << std::endl;
	index << "<td><a href=\"debug_info/cus/0.html\">.debug_info</a></td>" << std::endl;
	index << "<td>Debugging Information</td>" << std::endl;
	index << "</tr>" << std::endl;
	index << "<tr>" << std::endl;
	index << "<td><a href=\"debug_pubnames/0.html\">.debug_pubnames</a>&nbsp;<a href=\"debug_pubtypes/0.html\">.debug_pubtypes</a></td>" << std::endl;
	index << "<td>Lookup by Name</td>" << std::endl;
	index << "</tr>" << std::endl;
	index << "<tr>" << std::endl;
	index << "<td><a href=\"debug_aranges/0.html\">.debug_aranges</a></td>" << std::endl;
	index << "<td>Lookup by Address</td>" << std::endl;
	index << "</tr>" << std::endl;
	index << "<tr>" << std::endl;
	index << "<td><a href=\"debug_ranges/0.html\">.debug_ranges</a></td>" << std::endl;
	index << "<td> Non-Contiguous Address Ranges</td>" << std::endl;
	index << "</tr>" << std::endl;
	index << "<tr>" << std::endl;
	index << "<td><a href=\"debug_frame/fdes/0.html\">.debug_frame</a></td>" << std::endl;
	index << "<td>Call Frame Information</td>" << std::endl;
	index << "</tr>" << std::endl;
	index << "<tr>" << std::endl;
	index << "<td><a href=\"debug_line/0.html\">.debug_line</a></td>" << std::endl;
	index << "<td>Line Number Information</td>" << std::endl;
	index << "</tr>" << std::endl;
	index << "<tr>" << std::endl;
	index << "<td><a href=\"debug_macinfo/0.html\">.debug_macinfo</a></td>" << std::endl;
	index << "<td>Macro Information</td>" << std::endl;
	index << "</tr>" << std::endl;
	index << "<tr>" << std::endl;
	index << "<td><a href=\"debug_loc/0.html\">.debug_loc</a></td>" << std::endl;
	index << "<td> Location Lists</td>" << std::endl;
	index << "</tr>" << std::endl;
	index << "</table>" << std::endl;
	index << "</body>" << std::endl;
	index << "</html>" << std::endl;

	// Abbreviations Table
	std::stringstream debug_abbrev_output_dir_sstr;
	debug_abbrev_output_dir_sstr << output_dir << "/debug_abbrev";
	std::string debug_abbrev_output_dir(debug_abbrev_output_dir_sstr.str());
	mkdir(debug_abbrev_output_dir.c_str(), S_IRWXU);

	unsigned int debug_abbrev_filename_idx = 0;
	unsigned int debug_abbrev_per_file = 512;
	unsigned int num_debug_abbrevs = dw_abbrevs.size();
	unsigned int num_debug_abbrev_filenames = ((num_debug_abbrevs + debug_abbrev_per_file - 1) / debug_abbrev_per_file);
	typename std::map<uint64_t, DWARF_Abbrev *>::iterator dw_abbrev_iter = dw_abbrevs.begin();
	do
	{
		std::stringstream debug_abbrev_filename_sstr;
		debug_abbrev_filename_sstr << debug_abbrev_output_dir << "/" << debug_abbrev_filename_idx << ".html";
		std::string debug_abbrev_filename = debug_abbrev_filename_sstr.str().c_str();
		std::ofstream debug_abbrev(debug_abbrev_filename.c_str(), std::ios::out);
		debug_abbrev << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">" << std::endl;
		debug_abbrev << "<html>" << std::endl;
		debug_abbrev << "<head>" << std::endl;
		debug_abbrev << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		debug_abbrev << "<style type=\"text/css\">" << std::endl;
		debug_abbrev << "table th { text-align:center; }" << std::endl;
		debug_abbrev << "table th { font-weight:bold; }" << std::endl;
		debug_abbrev << "table td { text-align:left; }" << std::endl;
		debug_abbrev << "table { border-style:solid; }" << std::endl;
		debug_abbrev << "table { border-width:1px; }" << std::endl;
		debug_abbrev << "th, td { border-style:solid; }" << std::endl;
		debug_abbrev << "th, td { border-width:1px; }" << std::endl;
		debug_abbrev << "th, td { border-width:1px; }" << std::endl;
		debug_abbrev << "table.abbrev_attr { border-width:0px; }" << std::endl;
		debug_abbrev << "table.abbrev_attr th { border-width:0px; }" << std::endl;
		debug_abbrev << "table.abbrev_attr td { border-width:0px; }" << std::endl;
		debug_abbrev << "</style>" << std::endl;
		debug_abbrev << "</head>" << std::endl;
		debug_abbrev << "<body>" << std::endl;
		debug_abbrev << "<h1>Abbreviations Tables (.debug_abbrev)</h1>" << std::endl;
		debug_abbrev << "<a href=\"../index.html\">Index</a>&nbsp;" << std::endl;
		unsigned int i;
		for(i = 0; i < num_debug_abbrev_filenames; i++)
		{
			if(i != 0) debug_abbrev << "&nbsp;";
			if(i == debug_abbrev_filename_idx)
			{
				debug_abbrev << i << std::endl;
			}
			else
			{
				std::stringstream sstr;
				sstr << i << ".html";
				debug_abbrev << "<a href=\"" << sstr.str() << "\">" << i << "</a>" << std::endl;
			}
		}
		debug_abbrev << "<table>" << std::endl;
		debug_abbrev << "<tr>" << std::endl;
		debug_abbrev << "<th>Offset</th>" << std::endl;
		debug_abbrev << "<th>Code</th>" << std::endl;
		debug_abbrev << "<th>Tag</th>" << std::endl;
		debug_abbrev << "<th>Children</th>" << std::endl;
		debug_abbrev << "<th>Attributes</th>" << std::endl;
		debug_abbrev << "</tr>" << std::endl;
		unsigned int count = 0;
		while(count < debug_abbrev_per_file && dw_abbrev_iter != dw_abbrevs.end())
		{
			DWARF_Abbrev *dw_abbrev = (*dw_abbrev_iter).second;
			dw_abbrev->to_HTML(debug_abbrev) << std::endl;
			
			dw_abbrev_iter++;
			count++;
		}
		debug_abbrev << "</table>" << std::endl;
		debug_abbrev << "</body>" << std::endl;
		debug_abbrev << "</html>" << std::endl;
		debug_abbrev_filename_idx++;
	}
	while(dw_abbrev_iter != dw_abbrevs.end());
	
	// Macro Information
	std::stringstream debug_macinfo_ouput_dir_sstr;
	debug_macinfo_ouput_dir_sstr << output_dir << "/debug_macinfo";
	std::string debug_macinfo_output_dir(debug_macinfo_ouput_dir_sstr.str());
	mkdir(debug_macinfo_output_dir.c_str(), S_IRWXU);
	
	unsigned int debug_macinfo_filename_idx = 0;
	unsigned int debug_macinfo_per_file = 2048;
	unsigned int num_debug_macinfos = dw_macinfo_list.size();
	unsigned int num_debug_macinfo_filenames = ((num_debug_macinfos + debug_macinfo_per_file - 1) / debug_macinfo_per_file);
	typename std::map<uint64_t, DWARF_MacInfoListEntry<MEMORY_ADDR> *>::iterator dw_macinfo_list_iter = dw_macinfo_list.begin();
	do
	{
		std::stringstream debug_macinfo_filename_sstr;
		debug_macinfo_filename_sstr << debug_macinfo_output_dir << "/" << debug_macinfo_filename_idx << ".html";
		std::string debug_macinfo_filename = debug_macinfo_filename_sstr.str().c_str();
		std::ofstream debug_macinfo(debug_macinfo_filename.c_str(), std::ios::out);
		debug_macinfo << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">" << std::endl;
		debug_macinfo << "<html>" << std::endl;
		debug_macinfo << "<head>" << std::endl;
		debug_macinfo << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		debug_macinfo << "<style type=\"text/css\">" << std::endl;
		debug_macinfo << "<!--" << std::endl;
		debug_macinfo << "table th { text-align:center; }" << std::endl;
		debug_macinfo << "table th { font-weight:bold; }" << std::endl;
		debug_macinfo << "table td { text-align:left; }" << std::endl;
		debug_macinfo << "table { border-style:solid; }" << std::endl;
		debug_macinfo << "table { border-width:1px; }" << std::endl;
		debug_macinfo << "th, td { border-style:solid; }" << std::endl;
		debug_macinfo << "th, td { border-width:1px; }" << std::endl;
		debug_macinfo << "th, td { border-width:1px; }" << std::endl;
		debug_macinfo << "table.macinfo { border-width:0px; }" << std::endl;
		debug_macinfo << "table.macinfo th { border-width:0px; }" << std::endl;
		debug_macinfo << "table.macinfo td { border-width:0px; }" << std::endl;
		debug_macinfo << "-->" << std::endl;
		debug_macinfo << "</style>" << std::endl;
		debug_macinfo << "</head>" << std::endl;
		debug_macinfo << "<body>" << std::endl;
		debug_macinfo << "<h1>Macro Information (.debug_macinfo)</h1>" << std::endl;
		debug_macinfo << "<a href=\"../index.html\">Index</a>&nbsp;" << std::endl;
		unsigned int i;
		for(i = 0; i < num_debug_macinfo_filenames; i++)
		{
			if(i != 0) debug_macinfo << "&nbsp;";
			if(i == debug_macinfo_filename_idx)
			{
				debug_macinfo << i << std::endl;
			}
			else
			{
				std::stringstream sstr;
				sstr << i << ".html";
				debug_macinfo << "<a href=\"" << sstr.str() << "\">" << i << "</a>" << std::endl;
			}
		}
		debug_macinfo << "<table>" << std::endl;
		debug_macinfo << "<tr>" << std::endl;
		debug_macinfo << "<th>Entry type</th>" << std::endl;
		debug_macinfo << "<th>Id</th>" << std::endl;
		debug_macinfo << "<th>Offset</th>" << std::endl;
		debug_macinfo << "<th>Next</th>" << std::endl;
		debug_macinfo << "<th>Entry Information</th>" << std::endl;
		debug_macinfo << "</tr>" << std::endl;
		unsigned int count = 0;
		while(count < debug_macinfo_per_file && dw_macinfo_list_iter != dw_macinfo_list.end())
		{
			DWARF_MacInfoListEntry<MEMORY_ADDR> *dw_macinfo = (*dw_macinfo_list_iter).second;

			dw_macinfo->to_HTML(debug_macinfo) << std::endl;
			
			dw_macinfo_list_iter++;
			count++;
		}
		debug_macinfo << "</table>" << std::endl;
		debug_macinfo << "</body>" << std::endl;
		debug_macinfo << "</html>" << std::endl;
		debug_macinfo_filename_idx++;
	}
	while(dw_macinfo_list_iter != dw_macinfo_list.end());

	
	// Line Number Information
	std::stringstream debug_line_ouput_dir_sstr;
	debug_line_ouput_dir_sstr << output_dir << "/debug_line";
	std::string debug_line_output_dir(debug_line_ouput_dir_sstr.str());
	mkdir(debug_line_output_dir.c_str(), S_IRWXU);

	
	unsigned int debug_line_filename_idx = 0;
	unsigned int num_debug_line = dw_stmt_progs.size();
	unsigned int num_debug_line_filenames = ((num_debug_line + debug_line_per_file - 1) / debug_line_per_file);
	typename std::map<uint64_t, DWARF_StatementProgram<MEMORY_ADDR> *>::iterator dw_stmt_prog_iter = dw_stmt_progs.begin();
	do
	{
		std::stringstream debug_line_filename_sstr;
		debug_line_filename_sstr << debug_line_output_dir << "/" << debug_line_filename_idx << ".html";
		std::string debug_line_filename = debug_line_filename_sstr.str().c_str();
		std::ofstream debug_line(debug_line_filename.c_str(), std::ios::out);
		debug_line << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">" << std::endl;
		debug_line << "<html>" << std::endl;
		debug_line << "<head>" << std::endl;
		debug_line << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		debug_line << "<style type=\"text/css\">" << std::endl;
		debug_line << "<!--" << std::endl;
		debug_line << "table th { text-align:center; }" << std::endl;
		debug_line << "table th { font-weight:bold; }" << std::endl;
		debug_line << "table td { text-align:left; }" << std::endl;
		debug_line << "table { border-style:solid; }" << std::endl;
		debug_line << "table { border-width:1px; }" << std::endl;
		debug_line << "th, td { border-style:solid; }" << std::endl;
		debug_line << "th, td { border-width:1px; }" << std::endl;
		debug_line << "th, td { border-width:1px; }" << std::endl;
		debug_line << "table.range { border-width:0px; }" << std::endl;
		debug_line << "table.range th { border-width:0px; }" << std::endl;
		debug_line << "table.range td { border-width:0px; }" << std::endl;
		debug_line << "-->" << std::endl;
		debug_line << "</style>" << std::endl;
		debug_line << "</head>" << std::endl;
		debug_line << "<body>" << std::endl;
		debug_line << "<h1>Line number information (.debug_line)</h1>" << std::endl;
		debug_line << "<a href=\"../index.html\">Index</a>&nbsp;" << std::endl;
		unsigned int i;
		for(i = 0; i < num_debug_line_filenames; i++)
		{
			if(i != 0) debug_line << "&nbsp;";
			if(i == debug_line_filename_idx)
			{
				debug_line << i << std::endl;
			}
			else
			{
				std::stringstream sstr;
				sstr << i << ".html";
				debug_line << "<a href=\"" << sstr.str() << "\">" << i << "</a>" << std::endl;
			}
		}
		debug_line << "<table>" << std::endl;
		debug_line << "<tr>" << std::endl;
		debug_line << "<th>Statement Program</th>" << std::endl;
		debug_line << "</tr>" << std::endl;
		unsigned int count = 0;
		while(count < debug_line_per_file && dw_stmt_prog_iter != dw_stmt_progs.end())
		{
			DWARF_StatementProgram<MEMORY_ADDR> *dw_stmt_prog = (*dw_stmt_prog_iter).second;

			dw_stmt_prog->to_HTML(debug_line) << std::endl;
			
			dw_stmt_prog_iter++;
			count++;
		}
		debug_line << "</table>" << std::endl;
		debug_line << "</body>" << std::endl;
		debug_line << "</html>" << std::endl;
		debug_line_filename_idx++;
	}
	while(dw_stmt_prog_iter != dw_stmt_progs.end());

	// Lookup by Address
	std::stringstream debug_aranges_ouput_dir_sstr;
	debug_aranges_ouput_dir_sstr << output_dir << "/debug_aranges";
	std::string debug_aranges_output_dir(debug_aranges_ouput_dir_sstr.str());
	mkdir(debug_aranges_output_dir.c_str(), S_IRWXU);

	
	unsigned int debug_aranges_filename_idx = 0;
	unsigned int debug_aranges_per_file = 2048;
	unsigned int num_debug_aranges = dw_aranges.size();
	unsigned int num_debug_aranges_filenames = ((num_debug_aranges + debug_aranges_per_file - 1) / debug_aranges_per_file);
	typename std::vector<DWARF_AddressRanges<MEMORY_ADDR> *>::iterator dw_aranges_iter = dw_aranges.begin();
	do
	{
		std::stringstream debug_aranges_filename_sstr;
		debug_aranges_filename_sstr << debug_aranges_output_dir << "/" << debug_aranges_filename_idx << ".html";
		std::string debug_aranges_filename = debug_aranges_filename_sstr.str().c_str();
		std::ofstream debug_aranges(debug_aranges_filename.c_str(), std::ios::out);
		debug_aranges << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">" << std::endl;
		debug_aranges << "<html>" << std::endl;
		debug_aranges << "<head>" << std::endl;
		debug_aranges << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		debug_aranges << "<style type=\"text/css\">" << std::endl;
		debug_aranges << "<!--" << std::endl;
		debug_aranges << "table th { text-align:center; }" << std::endl;
		debug_aranges << "table th { font-weight:bold; }" << std::endl;
		debug_aranges << "table td { text-align:left; }" << std::endl;
		debug_aranges << "table { border-style:solid; }" << std::endl;
		debug_aranges << "table { border-width:1px; }" << std::endl;
		debug_aranges << "th, td { border-style:solid; }" << std::endl;
		debug_aranges << "th, td { border-width:1px; }" << std::endl;
		debug_aranges << "th, td { border-width:1px; }" << std::endl;
		debug_aranges << "table.range { border-width:0px; }" << std::endl;
		debug_aranges << "table.range th { border-width:0px; }" << std::endl;
		debug_aranges << "table.range td { border-width:0px; }" << std::endl;
		debug_aranges << "-->" << std::endl;
		debug_aranges << "</style>" << std::endl;
		debug_aranges << "</head>" << std::endl;
		debug_aranges << "<body>" << std::endl;
		debug_aranges << "<h1>Lookup by Address (.debug_aranges)</h1>" << std::endl;
		debug_aranges << "<a href=\"../index.html\">Index</a>&nbsp;" << std::endl;
		unsigned int i;
		for(i = 0; i < num_debug_aranges_filenames; i++)
		{
			if(i != 0) debug_aranges << "&nbsp;";
			if(i == debug_aranges_filename_idx)
			{
				debug_aranges << i << std::endl;
			}
			else
			{
				std::stringstream sstr;
				sstr << i << ".html";
				debug_aranges << "<a href=\"" << sstr.str() << "\">" << i << "</a>" << std::endl;
			}
		}
		debug_aranges << "<table>" << std::endl;
		debug_aranges << "<tr>" << std::endl;
		debug_aranges << "<th>Version</th>" << std::endl;
		debug_aranges << "<th>Compilation unit</th>" << std::endl;
		debug_aranges << "<th>Address size</th>" << std::endl;
		debug_aranges << "<th>Segment size</th>" << std::endl;
		debug_aranges << "<th>Address descriptors</th>" << std::endl;
		debug_aranges << "</tr>" << std::endl;
		unsigned int count = 0;
		while(count < debug_aranges_per_file && dw_aranges_iter != dw_aranges.end())
		{
			DWARF_AddressRanges<MEMORY_ADDR> *dw_arange = (*dw_aranges_iter);

			dw_arange->to_HTML(debug_aranges) << std::endl;
			
			dw_aranges_iter++;
			count++;
		}
		debug_aranges << "</table>" << std::endl;
		debug_aranges << "</body>" << std::endl;
		debug_aranges << "</html>" << std::endl;
		debug_aranges_filename_idx++;
	}
	while(dw_aranges_iter != dw_aranges.end());

	// Lookup by name (.debug_pubnames)
	std::stringstream debug_pubnames_ouput_dir_sstr;
	debug_pubnames_ouput_dir_sstr << output_dir << "/debug_pubnames";
	std::string debug_pubnames_output_dir(debug_pubnames_ouput_dir_sstr.str());
	mkdir(debug_pubnames_output_dir.c_str(), S_IRWXU);

	
	unsigned int debug_pubnames_filename_idx = 0;
	unsigned int debug_pubnames_per_file = 2048;
	unsigned int num_debug_pubnames = dw_pubnames.size();
	unsigned int num_debug_pubnames_filenames = ((num_debug_pubnames + debug_pubnames_per_file - 1) / debug_pubnames_per_file);
	typename std::vector<DWARF_Pubs<MEMORY_ADDR> *>::iterator dw_pubnames_iter = dw_pubnames.begin();
	do
	{
		std::stringstream debug_pubnames_filename_sstr;
		debug_pubnames_filename_sstr << debug_pubnames_output_dir << "/" << debug_pubnames_filename_idx << ".html";
		std::string debug_pubnames_filename = debug_pubnames_filename_sstr.str().c_str();
		std::ofstream debug_pubnames(debug_pubnames_filename.c_str(), std::ios::out);
		debug_pubnames << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">" << std::endl;
		debug_pubnames << "<html>" << std::endl;
		debug_pubnames << "<head>" << std::endl;
		debug_pubnames << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		debug_pubnames << "<style type=\"text/css\">" << std::endl;
		debug_pubnames << "<!--" << std::endl;
		debug_pubnames << "table th { text-align:center; }" << std::endl;
		debug_pubnames << "table th { font-weight:bold; }" << std::endl;
		debug_pubnames << "table td { text-align:left; }" << std::endl;
		debug_pubnames << "table { border-style:solid; }" << std::endl;
		debug_pubnames << "table { border-width:1px; }" << std::endl;
		debug_pubnames << "th, td { border-style:solid; }" << std::endl;
		debug_pubnames << "th, td { border-width:1px; }" << std::endl;
		debug_pubnames << "th, td { border-width:1px; }" << std::endl;
		debug_pubnames << "table.range { border-width:0px; }" << std::endl;
		debug_pubnames << "table.range th { border-width:0px; }" << std::endl;
		debug_pubnames << "table.range td { border-width:0px; }" << std::endl;
		debug_pubnames << "-->" << std::endl;
		debug_pubnames << "</style>" << std::endl;
		debug_pubnames << "</head>" << std::endl;
		debug_pubnames << "<body>" << std::endl;
		debug_pubnames << "<h1>Lookup by name (.debug_pubnames)</h1>" << std::endl;
		debug_pubnames << "<a href=\"../index.html\">Index</a>&nbsp;" << std::endl;
		unsigned int i;
		for(i = 0; i < num_debug_pubnames_filenames; i++)
		{
			if(i != 0) debug_pubnames << "&nbsp;";
			if(i == debug_pubnames_filename_idx)
			{
				debug_pubnames << i << std::endl;
			}
			else
			{
				std::stringstream sstr;
				sstr << i << ".html";
				debug_pubnames << "<a href=\"" << sstr.str() << "\">" << i << "</a>" << std::endl;
			}
		}
		debug_pubnames << "<table>" << std::endl;
		debug_pubnames << "<tr>" << std::endl;
		debug_pubnames << "<th>Version</th>" << std::endl;
		debug_pubnames << "<th>Compilation unit</th>" << std::endl;
		debug_pubnames << "<th>Public names</th>" << std::endl;
		debug_pubnames << "</tr>" << std::endl;
		unsigned int count = 0;
		while(count < debug_pubnames_per_file && dw_pubnames_iter != dw_pubnames.end())
		{
			DWARF_Pubs<MEMORY_ADDR> *dw_pubname = (*dw_pubnames_iter);

			dw_pubname->to_HTML(debug_pubnames) << std::endl;
			
			dw_pubnames_iter++;
			count++;
		}
		debug_pubnames << "</table>" << std::endl;
		debug_pubnames << "</body>" << std::endl;
		debug_pubnames << "</html>" << std::endl;
		debug_pubnames_filename_idx++;
	}
	while(dw_pubnames_iter != dw_pubnames.end());

	// Lookup by name (.debug_pubtypes)
	std::stringstream debug_pubtypes_ouput_dir_sstr;
	debug_pubtypes_ouput_dir_sstr << output_dir << "/debug_pubtypes";
	std::string debug_pubtypes_output_dir(debug_pubtypes_ouput_dir_sstr.str());
	mkdir(debug_pubtypes_output_dir.c_str(), S_IRWXU);

	
	unsigned int debug_pubtypes_filename_idx = 0;
	unsigned int debug_pubtypes_per_file = 2048;
	unsigned int num_debug_pubtypes = dw_pubtypes.size();
	unsigned int num_debug_pubtypes_filenames = ((num_debug_pubtypes + debug_pubtypes_per_file - 1) / debug_pubtypes_per_file);
	typename std::vector<DWARF_Pubs<MEMORY_ADDR> *>::iterator dw_pubtypes_iter = dw_pubtypes.begin();
	do
	{
		std::stringstream debug_pubtypes_filename_sstr;
		debug_pubtypes_filename_sstr << debug_pubtypes_output_dir << "/" << debug_pubtypes_filename_idx << ".html";
		std::string debug_pubtypes_filename = debug_pubtypes_filename_sstr.str().c_str();
		std::ofstream debug_pubtypes(debug_pubtypes_filename.c_str(), std::ios::out);
		debug_pubtypes << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">" << std::endl;
		debug_pubtypes << "<html>" << std::endl;
		debug_pubtypes << "<head>" << std::endl;
		debug_pubtypes << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		debug_pubtypes << "<style type=\"text/css\">" << std::endl;
		debug_pubtypes << "<!--" << std::endl;
		debug_pubtypes << "table th { text-align:center; }" << std::endl;
		debug_pubtypes << "table th { font-weight:bold; }" << std::endl;
		debug_pubtypes << "table td { text-align:left; }" << std::endl;
		debug_pubtypes << "table { border-style:solid; }" << std::endl;
		debug_pubtypes << "table { border-width:1px; }" << std::endl;
		debug_pubtypes << "th, td { border-style:solid; }" << std::endl;
		debug_pubtypes << "th, td { border-width:1px; }" << std::endl;
		debug_pubtypes << "th, td { border-width:1px; }" << std::endl;
		debug_pubtypes << "table.range { border-width:0px; }" << std::endl;
		debug_pubtypes << "table.range th { border-width:0px; }" << std::endl;
		debug_pubtypes << "table.range td { border-width:0px; }" << std::endl;
		debug_pubtypes << "-->" << std::endl;
		debug_pubtypes << "</style>" << std::endl;
		debug_pubtypes << "</head>" << std::endl;
		debug_pubtypes << "<body>" << std::endl;
		debug_pubtypes << "<h1>Lookup by name (.debug_pubtypes)</h1>" << std::endl;
		debug_pubtypes << "<a href=\"../index.html\">Index</a>&nbsp;" << std::endl;
		unsigned int i;
		for(i = 0; i < num_debug_pubtypes_filenames; i++)
		{
			if(i != 0) debug_pubtypes << "&nbsp;";
			if(i == debug_pubtypes_filename_idx)
			{
				debug_pubtypes << i << std::endl;
			}
			else
			{
				std::stringstream sstr;
				sstr << i << ".html";
				debug_pubtypes << "<a href=\"" << sstr.str() << "\">" << i << "</a>" << std::endl;
			}
		}
		debug_pubtypes << "<table>" << std::endl;
		debug_pubtypes << "<tr>" << std::endl;
		debug_pubtypes << "<th>Version</th>" << std::endl;
		debug_pubtypes << "<th>Compilation unit</th>" << std::endl;
		debug_pubtypes << "<th>Public names</th>" << std::endl;
		debug_pubtypes << "</tr>" << std::endl;
		unsigned int count = 0;
		while(count < debug_pubtypes_per_file && dw_pubtypes_iter != dw_pubtypes.end())
		{
			DWARF_Pubs<MEMORY_ADDR> *dw_pubtype = (*dw_pubtypes_iter);

			dw_pubtype->to_HTML(debug_pubtypes) << std::endl;
			
			dw_pubtypes_iter++;
			count++;
		}
		debug_pubtypes << "</table>" << std::endl;
		debug_pubtypes << "</body>" << std::endl;
		debug_pubtypes << "</html>" << std::endl;
		debug_pubtypes_filename_idx++;
	}
	while(dw_pubtypes_iter != dw_pubtypes.end());

	// Non-Contiguous Address Ranges
	
	std::stringstream debug_range_ouput_dir_sstr;
	debug_range_ouput_dir_sstr << output_dir << "/debug_ranges";
	std::string debug_range_output_dir(debug_range_ouput_dir_sstr.str());
	mkdir(debug_range_output_dir.c_str(), S_IRWXU);
	
	unsigned int debug_range_filename_idx = 0;
	unsigned int debug_range_per_file = 2048;
	unsigned int num_debug_ranges = dw_range_list.size();
	unsigned int num_debug_range_filenames = ((num_debug_ranges + debug_range_per_file - 1) / debug_range_per_file);
	typename std::map<uint64_t, DWARF_RangeListEntry<MEMORY_ADDR> *>::iterator dw_range_list_iter = dw_range_list.begin();
	do
	{
		std::stringstream debug_range_filename_sstr;
		debug_range_filename_sstr << debug_range_output_dir << "/" << debug_range_filename_idx << ".html";
		std::string debug_range_filename = debug_range_filename_sstr.str().c_str();
		std::ofstream debug_range(debug_range_filename.c_str(), std::ios::out);
		debug_range << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">" << std::endl;
		debug_range << "<html>" << std::endl;
		debug_range << "<head>" << std::endl;
		debug_range << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		debug_range << "<style type=\"text/css\">" << std::endl;
		debug_range << "<!--" << std::endl;
		debug_range << "table th { text-align:center; }" << std::endl;
		debug_range << "table th { font-weight:bold; }" << std::endl;
		debug_range << "table td { text-align:left; }" << std::endl;
		debug_range << "table { border-style:solid; }" << std::endl;
		debug_range << "table { border-width:1px; }" << std::endl;
		debug_range << "th, td { border-style:solid; }" << std::endl;
		debug_range << "th, td { border-width:1px; }" << std::endl;
		debug_range << "th, td { border-width:1px; }" << std::endl;
		debug_range << "table.range { border-width:0px; }" << std::endl;
		debug_range << "table.range th { border-width:0px; }" << std::endl;
		debug_range << "table.range td { border-width:0px; }" << std::endl;
		debug_range << "-->" << std::endl;
		debug_range << "</style>" << std::endl;
		debug_range << "</head>" << std::endl;
		debug_range << "<body>" << std::endl;
		debug_range << "<h1>Non-Contiguous Address Ranges (.debug_ranges)</h1>" << std::endl;
		debug_range << "<a href=\"../index.html\">Index</a>&nbsp;" << std::endl;
		unsigned int i;
		for(i = 0; i < num_debug_range_filenames; i++)
		{
			if(i != 0) debug_range << "&nbsp;";
			if(i == debug_range_filename_idx)
			{
				debug_range << i << std::endl;
			}
			else
			{
				std::stringstream sstr;
				sstr << i << ".html";
				debug_range << "<a href=\"" << sstr.str() << "\">" << i << "</a>" << std::endl;
			}
		}
		debug_range << "<table>" << std::endl;
		debug_range << "<tr>" << std::endl;
		debug_range << "<th>Entry type</th>" << std::endl;
		debug_range << "<th>Id</th>" << std::endl;
		debug_range << "<th>Offset</th>" << std::endl;
		debug_range << "<th>Next</th>" << std::endl;
		debug_range << "<th>Entry Information</th>" << std::endl;
		debug_range << "</tr>" << std::endl;
		unsigned int count = 0;
		while(count < debug_range_per_file && dw_range_list_iter != dw_range_list.end())
		{
			DWARF_RangeListEntry<MEMORY_ADDR> *dw_range = (*dw_range_list_iter).second;

			dw_range->to_HTML(debug_range) << std::endl;
			
			dw_range_list_iter++;
			count++;
		}
		debug_range << "</table>" << std::endl;
		debug_range << "</body>" << std::endl;
		debug_range << "</html>" << std::endl;
		debug_range_filename_idx++;
	}
	while(dw_range_list_iter != dw_range_list.end());

	// Location Lists
	
	std::stringstream debug_loc_ouput_dir_sstr;
	debug_loc_ouput_dir_sstr << output_dir << "/debug_loc";
	std::string debug_loc_output_dir(debug_loc_ouput_dir_sstr.str());
	mkdir(debug_loc_output_dir.c_str(), S_IRWXU);
	
	unsigned int debug_loc_filename_idx = 0;
	unsigned int debug_loc_per_file = 2048;
	unsigned int num_debug_locs = dw_loc_list.size();
	unsigned int num_debug_loc_filenames = ((num_debug_locs + debug_loc_per_file - 1) / debug_loc_per_file);
	typename std::map<uint64_t, DWARF_LocListEntry<MEMORY_ADDR> *>::iterator dw_loc_list_iter = dw_loc_list.begin();
	do
	{
		std::stringstream debug_loc_filename_sstr;
		debug_loc_filename_sstr << debug_loc_output_dir << "/" << debug_loc_filename_idx << ".html";
		std::string debug_loc_filename = debug_loc_filename_sstr.str().c_str();
		std::ofstream debug_loc(debug_loc_filename.c_str(), std::ios::out);
		debug_loc << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">" << std::endl;
		debug_loc << "<html>" << std::endl;
		debug_loc << "<head>" << std::endl;
		debug_loc << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		debug_loc << "<style type=\"text/css\">" << std::endl;
		debug_loc << "<!--" << std::endl;
		debug_loc << "table th { text-align:center; }" << std::endl;
		debug_loc << "table th { font-weight:bold; }" << std::endl;
		debug_loc << "table td { text-align:left; }" << std::endl;
		debug_loc << "table { border-style:solid; }" << std::endl;
		debug_loc << "table { border-width:1px; }" << std::endl;
		debug_loc << "th, td { border-style:solid; }" << std::endl;
		debug_loc << "th, td { border-width:1px; }" << std::endl;
		debug_loc << "th, td { border-width:1px; }" << std::endl;
		debug_loc << "table.loc { border-width:0px; }" << std::endl;
		debug_loc << "table.loc th { border-width:0px; }" << std::endl;
		debug_loc << "table.loc td { border-width:0px; }" << std::endl;
		debug_loc << "-->" << std::endl;
		debug_loc << "</style>" << std::endl;
		debug_loc << "</head>" << std::endl;
		debug_loc << "<body>" << std::endl;
		debug_loc << "<h1>Non-Contiguous Address Ranges (.debug_locs)</h1>" << std::endl;
		debug_loc << "<a href=\"../index.html\">Index</a>&nbsp;" << std::endl;
		unsigned int i;
		for(i = 0; i < num_debug_loc_filenames; i++)
		{
			if(i != 0) debug_loc << "&nbsp;";
			if(i == debug_loc_filename_idx)
			{
				debug_loc << i << std::endl;
			}
			else
			{
				std::stringstream sstr;
				sstr << i << ".html";
				debug_loc << "<a href=\"" << sstr.str() << "\">" << i << "</a>" << std::endl;
			}
		}
		debug_loc << "<table>" << std::endl;
		debug_loc << "<tr>" << std::endl;
		debug_loc << "<th>Entry type</th>" << std::endl;
		debug_loc << "<th>Id</th>" << std::endl;
		debug_loc << "<th>Offset</th>" << std::endl;
		debug_loc << "<th>Next</th>" << std::endl;
		debug_loc << "<th>Entry Information</th>" << std::endl;
		debug_loc << "</tr>" << std::endl;
		unsigned int count = 0;
		while(count < debug_loc_per_file && dw_loc_list_iter != dw_loc_list.end())
		{
			DWARF_LocListEntry<MEMORY_ADDR> *dw_loc = (*dw_loc_list_iter).second;

			dw_loc->to_HTML(debug_loc) << std::endl;
			
			dw_loc_list_iter++;
			count++;
		}
		debug_loc << "</table>" << std::endl;
		debug_loc << "</body>" << std::endl;
		debug_loc << "</html>" << std::endl;
		debug_loc_filename_idx++;
	}
	while(dw_loc_list_iter != dw_loc_list.end());

	// Call Frame Information
	std::stringstream debug_frame_ouput_dir_sstr;
	debug_frame_ouput_dir_sstr << output_dir << "/debug_frame";
	std::string debug_frame_output_dir(debug_frame_ouput_dir_sstr.str());
	mkdir(debug_frame_output_dir.c_str(), S_IRWXU);

	std::stringstream debug_frame_cies_ouput_dir_sstr;
	debug_frame_cies_ouput_dir_sstr << debug_frame_output_dir << "/cies";
	std::string debug_frame_cies_output_dir(debug_frame_cies_ouput_dir_sstr.str());
	mkdir(debug_frame_cies_output_dir.c_str(), S_IRWXU);

	std::stringstream debug_frame_fdes_ouput_dir_sstr;
	debug_frame_fdes_ouput_dir_sstr << debug_frame_output_dir << "/fdes";
	std::string debug_frame_fdes_output_dir(debug_frame_fdes_ouput_dir_sstr.str());
	mkdir(debug_frame_fdes_output_dir.c_str(), S_IRWXU);

	unsigned int debug_frame_cies_filename_idx = 0;
	unsigned int cies_per_file = 2048;
	unsigned int num_debug_cies = dw_cies.size();
	unsigned int num_debug_frame_cies_filenames = ((num_debug_cies + cies_per_file - 1) /cies_per_file);
	typename std::map<uint64_t, DWARF_CIE<MEMORY_ADDR> *>::iterator dw_cie_iter = dw_cies.begin();
	do
	{
		std::stringstream debug_frame_cies_filename_sstr;
		debug_frame_cies_filename_sstr << debug_frame_cies_output_dir << "/" << debug_frame_cies_filename_idx << ".html";
		std::string debug_frame_cies_filename = debug_frame_cies_filename_sstr.str().c_str();
		std::ofstream debug_frame_cies(debug_frame_cies_filename.c_str(), std::ios::out);
		debug_frame_cies << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">" << std::endl;
		debug_frame_cies << "<html>" << std::endl;
		debug_frame_cies << "<head>" << std::endl;
		debug_frame_cies << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		debug_frame_cies << "<style type=\"text/css\">" << std::endl;
		debug_frame_cies << "<!--" << std::endl;
		debug_frame_cies << "table th { text-align:center; }" << std::endl;
		debug_frame_cies << "table th { font-weight:bold; }" << std::endl;
		debug_frame_cies << "table td { text-align:left; }" << std::endl;
		debug_frame_cies << "table { border-style:solid; }" << std::endl;
		debug_frame_cies << "table { border-width:1px; }" << std::endl;
		debug_frame_cies << "th, td { border-style:solid; }" << std::endl;
		debug_frame_cies << "th, td { border-width:1px; }" << std::endl;
		debug_frame_cies << "th, td { border-width:1px; }" << std::endl;
		debug_frame_cies << "-->" << std::endl;
		debug_frame_cies << "</style>" << std::endl;
		debug_frame_cies << "</head>" << std::endl;
		debug_frame_cies << "<body>" << std::endl;
		debug_frame_cies << "<h1>Call Frame Information/Common Information Entries (CIE) (.debug_frame)</h1>" << std::endl;
		debug_frame_cies << "<a href=\"../../index.html\">Index</a>&nbsp;" << std::endl;
		unsigned int i;
		for(i = 0; i < num_debug_frame_cies_filenames; i++)
		{
			if(i != 0) debug_frame_cies << "&nbsp;";
			if(i == debug_frame_cies_filename_idx)
			{
				debug_frame_cies << i << std::endl;
			}
			else
			{
				std::stringstream sstr;
				sstr << i << ".html";
				debug_frame_cies << "<a href=\"" << sstr.str() << "\">" << i << "</a>" << std::endl;
			}
		}
		debug_frame_cies << "<hr>" << std::endl;
		debug_frame_cies << "<table>" << std::endl;
		debug_frame_cies << "<tr>" << std::endl;
		debug_frame_cies << "<th>Id</th>" << std::endl;
		debug_frame_cies << "<th>Offset</th>" << std::endl;
		debug_frame_cies << "<th>Version</th>" << std::endl;
		debug_frame_cies << "<th>Augmentation</th>" << std::endl;
		debug_frame_cies << "<th>Code alignment factor</th>" << std::endl;
		debug_frame_cies << "<th>Data alignment factor</th>" << std::endl;
		debug_frame_cies << "<th>Return address register</th>" << std::endl;
		debug_frame_cies << "<th>Initial call frame program</th>" << std::endl;
		debug_frame_cies << "</tr>" << std::endl;
		unsigned int count = 0;
		while(count < cies_per_file && dw_cie_iter != dw_cies.end())
		{
			DWARF_CIE<MEMORY_ADDR> *dw_cie = (*dw_cie_iter).second;

			dw_cie->to_HTML(debug_frame_cies) << std::endl;
			
			dw_cie_iter++;
			count++;
		}
		debug_frame_cies << "</table>" << std::endl;
		debug_frame_cies << "<hr>" << std::endl;
		for(i = 0; i < num_debug_frame_cies_filenames; i++)
		{
			if(i != 0) debug_frame_cies << "&nbsp;";
			if(i == debug_frame_cies_filename_idx)
			{
				debug_frame_cies << i << std::endl;
			}
			else
			{
				std::stringstream sstr;
				sstr << i << ".html";
				debug_frame_cies << "<a href=\"" << sstr.str() << "\">" << i << "</a>" << std::endl;
			}
		}
		debug_frame_cies << "</body>" << std::endl;
		debug_frame_cies << "</html>" << std::endl;
		debug_frame_cies_filename_idx++;
	}
	while(dw_cie_iter != dw_cies.end());
	
	unsigned int debug_frame_fdes_filename_idx = 0;
	unsigned int fdes_per_file = 2048;
	unsigned int num_debug_fdes = dw_fdes.size();
	unsigned int num_debug_frame_fdes_filenames = ((num_debug_fdes + fdes_per_file - 1) /fdes_per_file);
	typename std::vector<DWARF_FDE<MEMORY_ADDR> *>::iterator dw_fde_iter = dw_fdes.begin();
	do
	{
		std::stringstream debug_frame_fdes_filename_sstr;
		debug_frame_fdes_filename_sstr << debug_frame_fdes_output_dir << "/" << debug_frame_fdes_filename_idx << ".html";
		std::string debug_frame_fdes_filename = debug_frame_fdes_filename_sstr.str().c_str();
		std::ofstream debug_frame_fdes(debug_frame_fdes_filename.c_str(), std::ios::out);
		debug_frame_fdes << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">" << std::endl;
		debug_frame_fdes << "<html>" << std::endl;
		debug_frame_fdes << "<head>" << std::endl;
		debug_frame_fdes << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		debug_frame_fdes << "<style type=\"text/css\">" << std::endl;
		debug_frame_fdes << "<!--" << std::endl;
		debug_frame_fdes << "table th { text-align:center; }" << std::endl;
		debug_frame_fdes << "table th { font-weight:bold; }" << std::endl;
		debug_frame_fdes << "table td { text-align:left; }" << std::endl;
		debug_frame_fdes << "table { border-style:solid; }" << std::endl;
		debug_frame_fdes << "table { border-width:1px; }" << std::endl;
		debug_frame_fdes << "th, td { border-style:solid; }" << std::endl;
		debug_frame_fdes << "th, td { border-width:1px; }" << std::endl;
		debug_frame_fdes << "th, td { border-width:1px; }" << std::endl;
		debug_frame_fdes << "-->" << std::endl;
		debug_frame_fdes << "</style>" << std::endl;
		debug_frame_fdes << "</head>" << std::endl;
		debug_frame_fdes << "<body>" << std::endl;
		debug_frame_fdes << "<h1>Call Frame Information/Frame Description Entries (FDE) (.debug_frame)</h1>" << std::endl;
		debug_frame_fdes << "<a href=\"../../index.html\">Index</a>&nbsp;" << std::endl;
		unsigned int i;
		for(i = 0; i < num_debug_frame_fdes_filenames; i++)
		{
			if(i != 0) debug_frame_fdes << "&nbsp;";
			if(i == debug_frame_fdes_filename_idx)
			{
				debug_frame_fdes << i << std::endl;
			}
			else
			{
				std::stringstream sstr;
				sstr << i << ".html";
				debug_frame_fdes << "<a href=\"" << sstr.str() << "\">" << i << "</a>" << std::endl;
			}
		}
		debug_frame_fdes << "<hr>" << std::endl;
		debug_frame_fdes << "<table>" << std::endl;
		debug_frame_fdes << "<tr>" << std::endl;
		debug_frame_fdes << "<th>Offset</th>" << std::endl;
		debug_frame_fdes << "<th>CIE</th>" << std::endl;
		debug_frame_fdes << "<th>Initial location</th>" << std::endl;
		debug_frame_fdes << "<th>Address range</th>" << std::endl;
		debug_frame_fdes << "<th>Call frame program</th>" << std::endl;
		debug_frame_fdes << "</tr>" << std::endl;
		unsigned int count = 0;
		while(count < fdes_per_file && dw_fde_iter != dw_fdes.end())
		{
			DWARF_FDE<MEMORY_ADDR> *dw_fde = (*dw_fde_iter);

			dw_fde->to_HTML(debug_frame_fdes) << std::endl;
			
			dw_fde_iter++;
			count++;
		}
		debug_frame_fdes << "</table>" << std::endl;
		debug_frame_fdes << "<hr>" << std::endl;
		for(i = 0; i < num_debug_frame_fdes_filenames; i++)
		{
			if(i != 0) debug_frame_fdes << "&nbsp;";
			if(i == debug_frame_fdes_filename_idx)
			{
				debug_frame_fdes << i << std::endl;
			}
			else
			{
				std::stringstream sstr;
				sstr << i << ".html";
				debug_frame_fdes << "<a href=\"" << sstr.str() << "\">" << i << "</a>" << std::endl;
			}
		}
		debug_frame_fdes << "</body>" << std::endl;
		debug_frame_fdes << "</html>" << std::endl;
		debug_frame_fdes_filename_idx++;
	}
	while(dw_fde_iter != dw_fdes.end());
	
	// Debugging Information
	std::stringstream debug_info_ouput_dir_sstr;
	debug_info_ouput_dir_sstr << output_dir << "/debug_info";
	std::string debug_info_output_dir(debug_info_ouput_dir_sstr.str());
	mkdir(debug_info_output_dir.c_str(), S_IRWXU);

	std::stringstream debug_info_cus_ouput_dir_sstr;
	debug_info_cus_ouput_dir_sstr << debug_info_output_dir << "/cus";
	std::string debug_info_cus_output_dir(debug_info_cus_ouput_dir_sstr.str());
	mkdir(debug_info_cus_output_dir.c_str(), S_IRWXU);

	std::stringstream debug_info_dies_ouput_dir_sstr;
	debug_info_dies_ouput_dir_sstr << debug_info_output_dir << "/dies";
	std::string debug_info_dies_output_dir(debug_info_dies_ouput_dir_sstr.str());
	mkdir(debug_info_dies_output_dir.c_str(), S_IRWXU);

	unsigned int debug_info_cus_filename_idx = 0;
	unsigned int num_debug_cus = dw_cus.size();
	unsigned int num_debug_info_cus_filenames = ((num_debug_cus + cus_per_file - 1) / cus_per_file);
	typename std::map<uint64_t, DWARF_CompilationUnit<MEMORY_ADDR> *>::iterator dw_cu_iter = dw_cus.begin();
	do
	{
		std::stringstream debug_info_cus_filename_sstr;
		debug_info_cus_filename_sstr << debug_info_cus_output_dir << "/" << debug_info_cus_filename_idx << ".html";
		std::string debug_info_cus_filename = debug_info_cus_filename_sstr.str().c_str();
		std::ofstream debug_info_cus(debug_info_cus_filename.c_str(), std::ios::out);
		debug_info_cus << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">" << std::endl;
		debug_info_cus << "<html>" << std::endl;
		debug_info_cus << "<head>" << std::endl;
		debug_info_cus << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		debug_info_cus << "<style type=\"text/css\">" << std::endl;
		debug_info_cus << "<!--" << std::endl;
		debug_info_cus << "table th { text-align:center; }" << std::endl;
		debug_info_cus << "table th { font-weight:bold; }" << std::endl;
		debug_info_cus << "table td { text-align:left; }" << std::endl;
		debug_info_cus << "table { border-style:solid; }" << std::endl;
		debug_info_cus << "table { border-width:1px; }" << std::endl;
		debug_info_cus << "th, td { border-style:solid; }" << std::endl;
		debug_info_cus << "th, td { border-width:1px; }" << std::endl;
		debug_info_cus << "th, td { border-width:1px; }" << std::endl;
		debug_info_cus << "-->" << std::endl;
		debug_info_cus << "</style>" << std::endl;
		debug_info_cus << "</head>" << std::endl;
		debug_info_cus << "<body>" << std::endl;
		debug_info_cus << "<h1>Debug Information/Compilation Units (.debug_info)</h1>" << std::endl;
		debug_info_cus << "<a href=\"../../index.html\">Index</a>&nbsp;" << std::endl;
		unsigned int i;
		for(i = 0; i < num_debug_info_cus_filenames; i++)
		{
			if(i != 0) debug_info_cus << "&nbsp;";
			if(i == debug_info_cus_filename_idx)
			{
				debug_info_cus << i << std::endl;
			}
			else
			{
				std::stringstream sstr;
				sstr << i << ".html";
				debug_info_cus << "<a href=\"" << sstr.str() << "\">" << i << "</a>" << std::endl;
			}
		}
		debug_info_cus << "<table>" << std::endl;
		debug_info_cus << "<tr>" << std::endl;
		debug_info_cus << "<th>Id</th>" << std::endl;
		debug_info_cus << "<th>Offset</th>" << std::endl;
		debug_info_cus << "<th>Version</th>" << std::endl;
		debug_info_cus << "<th>Address size</th>" << std::endl;
		debug_info_cus << "<th>DIEs</th>" << std::endl;
		debug_info_cus << "</tr>" << std::endl;
		unsigned int count = 0;
		while(count < cus_per_file && dw_cu_iter != dw_cus.end())
		{
			DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu = (*dw_cu_iter).second;

			dw_cu->to_HTML(debug_info_cus) << std::endl;
			
			dw_cu_iter++;
			count++;
		}
		debug_info_cus << "</table>" << std::endl;
		debug_info_cus << "</body>" << std::endl;
		debug_info_cus << "</html>" << std::endl;
		debug_info_cus_filename_idx++;
	}
	while(dw_cu_iter != dw_cus.end());
	
	unsigned int debug_info_dies_filename_idx = 0;
	unsigned int dies_per_file = 2048;
	unsigned int num_debug_dies = dw_dies.size();
	unsigned int num_debug_info_dies_filenames = ((num_debug_dies + dies_per_file - 1) /dies_per_file);
	typename std::map<uint64_t, DWARF_DIE<MEMORY_ADDR> *>::iterator dw_die_iter = dw_dies.begin();
	do
	{
		std::stringstream debug_info_dies_filename_sstr;
		debug_info_dies_filename_sstr << debug_info_dies_output_dir << "/" << debug_info_dies_filename_idx << ".html";
		std::string debug_info_dies_filename = debug_info_dies_filename_sstr.str().c_str();
		std::ofstream debug_info_dies(debug_info_dies_filename.c_str(), std::ios::out);
		debug_info_dies << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">" << std::endl;
		debug_info_dies << "<html>" << std::endl;
		debug_info_dies << "<head>" << std::endl;
		debug_info_dies << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">" << std::endl;
		debug_info_dies << "<style type=\"text/css\">" << std::endl;
		debug_info_dies << "<!--" << std::endl;
		debug_info_dies << "table th { text-align:center; }" << std::endl;
		debug_info_dies << "table th { font-weight:bold; }" << std::endl;
		debug_info_dies << "table td { text-align:left; }" << std::endl;
		debug_info_dies << "table { border-style:solid; }" << std::endl;
		debug_info_dies << "table { border-width:1px; }" << std::endl;
		debug_info_dies << "th, td { border-style:solid; }" << std::endl;
		debug_info_dies << "th, td { border-width:1px; }" << std::endl;
		debug_info_dies << "th, td { border-width:1px; }" << std::endl;
		debug_info_dies << "-->" << std::endl;
		debug_info_dies << "</style>" << std::endl;
		debug_info_dies << "</head>" << std::endl;
		debug_info_dies << "<body>" << std::endl;
		debug_info_dies << "<h1>Debug Information/Debug Information Entries (.debug_info)</h1>" << std::endl;
		debug_info_dies << "<a href=\"../../index.html\">Index</a>&nbsp;" << std::endl;
		unsigned int i;
		for(i = 0; i < num_debug_info_dies_filenames; i++)
		{
			if(i != 0) debug_info_dies << "&nbsp;";
			if(i == debug_info_dies_filename_idx)
			{
				debug_info_dies << i << std::endl;
			}
			else
			{
				std::stringstream sstr;
				sstr << i << ".html";
				debug_info_dies << "<a href=\"" << sstr.str() << "\">" << i << "</a>" << std::endl;
			}
		}
		debug_info_dies << "<hr>" << std::endl;
		debug_info_dies << "<table>" << std::endl;
		debug_info_dies << "<tr>" << std::endl;
		debug_info_dies << "<th>Id</th>" << std::endl;
		debug_info_dies << "<th>Offset</th>" << std::endl;
		debug_info_dies << "<th>Compilation Unit</th>" << std::endl;
		debug_info_dies << "<th>Parent DIE</th>" << std::endl;
		debug_info_dies << "<th>Children</th>" << std::endl;
		debug_info_dies << "<th>Entry Information</th>" << std::endl;
		debug_info_dies << "</tr>" << std::endl;
		unsigned int count = 0;
		while(count < dies_per_file && dw_die_iter != dw_dies.end())
		{
			DWARF_DIE<MEMORY_ADDR> *dw_die = (*dw_die_iter).second;

			dw_die->to_HTML(debug_info_dies) << std::endl;
			
			dw_die_iter++;
			count++;
		}
		debug_info_dies << "</table>" << std::endl;
		debug_info_dies << "<hr>" << std::endl;
		for(i = 0; i < num_debug_info_dies_filenames; i++)
		{
			if(i != 0) debug_info_dies << "&nbsp;";
			if(i == debug_info_dies_filename_idx)
			{
				debug_info_dies << i << std::endl;
			}
			else
			{
				std::stringstream sstr;
				sstr << i << ".html";
				debug_info_dies << "<a href=\"" << sstr.str() << "\">" << i << "</a>" << std::endl;
			}
		}
		debug_info_dies << "</body>" << std::endl;
		debug_info_dies << "</html>" << std::endl;
		debug_info_dies_filename_idx++;
	}
	while(dw_die_iter != dw_dies.end());
}

template <class MEMORY_ADDR>
void DWARF_Handler<MEMORY_ADDR>::Register(DWARF_StatementProgram<MEMORY_ADDR> *dw_stmt_prog)
{
	dw_stmt_progs.insert(std::pair<uint64_t, DWARF_StatementProgram<MEMORY_ADDR> *>(dw_stmt_prog->GetOffset(), dw_stmt_prog));
}

template <class MEMORY_ADDR>
void DWARF_Handler<MEMORY_ADDR>::Register(DWARF_DIE<MEMORY_ADDR> *dw_die)
{
	//std::cerr << "Registering " << dw_die << " at offset " << dw_die->GetOffset() << std::endl;
	dw_dies.insert(std::pair<uint64_t, DWARF_DIE<MEMORY_ADDR> *>(dw_die->GetOffset(), dw_die));
}

template <class MEMORY_ADDR>
void DWARF_Handler<MEMORY_ADDR>::Register(DWARF_RangeListEntry<MEMORY_ADDR> *dw_range_list_entry)
{
	dw_range_list.insert(std::pair<uint64_t, DWARF_RangeListEntry<MEMORY_ADDR> *>(dw_range_list_entry->GetOffset(), dw_range_list_entry));
}

template <class MEMORY_ADDR>
void DWARF_Handler<MEMORY_ADDR>::Register(DWARF_MacInfoListEntry<MEMORY_ADDR> *dw_macinfo_list_entry)
{
	dw_macinfo_list.insert(std::pair<uint64_t, DWARF_MacInfoListEntry<MEMORY_ADDR> *>(dw_macinfo_list_entry->GetOffset(), dw_macinfo_list_entry));
}

template <class MEMORY_ADDR>
void DWARF_Handler<MEMORY_ADDR>::Register(DWARF_LocListEntry<MEMORY_ADDR> *dw_loc_list_entry)
{
	dw_loc_list.insert(std::pair<uint64_t, DWARF_LocListEntry<MEMORY_ADDR> *>(dw_loc_list_entry->GetOffset(), dw_loc_list_entry));
}

template <class MEMORY_ADDR>
void DWARF_Handler<MEMORY_ADDR>::DumpStatementMatrix()
{
	typename std::map<MEMORY_ADDR, Statement<MEMORY_ADDR> *>::const_iterator stmt_iter;

	for(stmt_iter = stmt_matrix.begin(); stmt_iter != stmt_matrix.end(); stmt_iter++)
	{
		if((*stmt_iter).second)
		{
			std::cout << *(*stmt_iter).second << std::endl;
		}
	}
}

template <class MEMORY_ADDR>
const unisim::util::debug::Statement<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindStatement(MEMORY_ADDR addr) const
{
	typename std::map<MEMORY_ADDR, Statement<MEMORY_ADDR> *>::const_iterator stmt_iter = stmt_matrix.find(addr);
	
	return (stmt_iter != stmt_matrix.end()) ? (*stmt_iter).second : 0;
}

template <class MEMORY_ADDR>
const unisim::util::debug::Statement<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindStatement(const char *filename, unsigned int lineno, unsigned int colno) const
{
	bool requested_filename_is_absolute = IsAbsolutePath(filename);
	std::vector<std::string> hierarchical_requested_filename;
	
	std::string s;
	const char *p = filename;
	do
	{
		if(*p == 0 || *p == '/' || *p == '\\')
		{
			hierarchical_requested_filename.push_back(s);
			s.clear();
		}
		else
		{
			s += *p;
		}
	} while(*(p++));
	int hierarchical_requested_filename_depth = hierarchical_requested_filename.size();

	typename std::map<MEMORY_ADDR, Statement<MEMORY_ADDR> *>::const_iterator stmt_iter;
	
	for(stmt_iter = stmt_matrix.begin(); stmt_iter != stmt_matrix.end(); stmt_iter++)
	{
		Statement<MEMORY_ADDR> *stmt = (*stmt_iter).second;
		
		if(stmt)
		{
			if(stmt->GetLineNo() == lineno && (!colno || (stmt->GetColNo() == colno)))
			{
				std::string source_path;
				const char *source_filename = stmt->GetSourceFilename();
				if(source_filename)
				{
					const char *source_dirname = stmt->GetSourceDirname();
					if(source_dirname)
					{
						source_path += source_dirname;
						source_path += '/';
					}
					source_path += source_filename;

					std::vector<std::string> hierarchical_source_path;
					
					s.clear();
					p = source_path.c_str();
					do
					{
						if(*p == 0 || *p == '/' || *p == '\\')
						{
							hierarchical_source_path.push_back(s);
							s.clear();
						}
						else
						{
							s += *p;
						}
					} while(*(p++));

					int hierarchical_source_path_depth = hierarchical_source_path.size();
					
					if((!requested_filename_is_absolute && hierarchical_source_path_depth >= hierarchical_requested_filename_depth) ||
					   (requested_filename_is_absolute && hierarchical_source_path_depth == hierarchical_requested_filename_depth))
					{
						int i;
						bool match = true;
						
						for(i = 0; i < hierarchical_requested_filename_depth; i++)
						{
							if(hierarchical_source_path[hierarchical_source_path_depth - 1 - i] != hierarchical_requested_filename[hierarchical_requested_filename_depth - 1 - i])
							{
								match = false;
								break;
							}
						}
						
						if(match) return stmt;
					}
				}
			}
		}
	}
	return 0;
}

template <class MEMORY_ADDR>
bool DWARF_Handler<MEMORY_ADDR>::IsAbsolutePath(const char *filename) const
{
	// filename starts '/' or 'drive letter':\ or 'driver letter':/
	return (((filename[0] >= 'a' && filename[0] <= 'z') || (filename[0] >= 'A' && filename[0] <= 'Z')) && (filename[1] == ':') && ((filename[2] == '\\') || (filename[2] == '/'))) || (*filename == '/');
}

template <class MEMORY_ADDR>
endian_type DWARF_Handler<MEMORY_ADDR>::GetEndianness() const
{
	return endianness;
}

template <class MEMORY_ADDR>
uint8_t DWARF_Handler<MEMORY_ADDR>::GetAddressSize() const
{
	return address_size;
}

template <class MEMORY_ADDR>
const DWARF_StatementProgram<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindStatementProgram(uint64_t debug_line_offset)
{
	if(!debug_line_section || debug_line_offset >= debug_line_section_size) return 0;
	
	typename std::map<uint64_t, DWARF_StatementProgram<MEMORY_ADDR> *>::const_iterator dw_stmt_prog_iter = dw_stmt_progs.find(debug_line_offset);
		
	if(dw_stmt_prog_iter != dw_stmt_progs.end())
	{
		return (*dw_stmt_prog_iter).second;
	}

	DWARF_StatementProgram<MEMORY_ADDR> *dw_stmt_prog = new DWARF_StatementProgram<MEMORY_ADDR>(this);
	int64_t sz;
	if((sz = dw_stmt_prog->Load((const uint8_t *) debug_line_section + debug_line_offset, debug_line_section_size - debug_line_offset, debug_line_offset)) < 0)
	{
		std::cerr << "Invalid DWARF2 statement program prologue at offset 0x" << std::hex << debug_line_offset << std::dec << std::endl;
		delete dw_stmt_prog;
		return 0;
	}

	Register(dw_stmt_prog);
	//std::cerr << *dw_stmt_prog << std::endl;
	return dw_stmt_prog;
}

template <class MEMORY_ADDR>
const DWARF_DIE<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindDIE(uint64_t debug_info_offset) const
{
	typename std::map<uint64_t, DWARF_DIE<MEMORY_ADDR> *>::const_iterator dw_die_iter = dw_dies.find(debug_info_offset);
	
	return dw_die_iter != dw_dies.end() ? (*dw_die_iter).second : 0;
}

template <class MEMORY_ADDR>
const DWARF_RangeListEntry<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindRangeListEntry(const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu, uint64_t debug_ranges_offset)
{
	if(!debug_ranges_section || debug_ranges_offset >= debug_ranges_section_size) return 0;
	
	DWARF_RangeListEntry<MEMORY_ADDR> *head = 0;
	do
	{
		typename std::map<uint64_t, DWARF_RangeListEntry<MEMORY_ADDR> *>::const_iterator dw_range_list_iter = dw_range_list.find(debug_ranges_offset);
		
		if(dw_range_list_iter != dw_range_list.end())
		{
			return head ? head : (*dw_range_list_iter).second;
		}
		
		DWARF_RangeListEntry<MEMORY_ADDR> *dw_range_list_entry = new DWARF_RangeListEntry<MEMORY_ADDR>(dw_cu);
		int64_t sz;
			
		if((sz = dw_range_list_entry->Load((const uint8_t *) debug_ranges_section + debug_ranges_offset, debug_ranges_section_size - debug_ranges_offset, debug_ranges_offset)) < 0)
		{
			std::cerr << "Invalid DWARF2 debug ranges at offset 0x" << std::hex << debug_ranges_offset << std::dec << std::endl;
			delete dw_range_list_entry;
			return head;
		}

		//std::cerr << *dw_range_list_entry << std::endl;
		Register(dw_range_list_entry);
		debug_ranges_offset += sz;
		if(!head) head = dw_range_list_entry;
		if(dw_range_list_entry->IsEndOfList()) break; // End of list
	} while(debug_ranges_offset < debug_ranges_section_size);
	
	return head;
}

template <class MEMORY_ADDR>
const DWARF_MacInfoListEntry<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindMacInfoListEntry(uint64_t debug_macinfo_offset)
{
	if(!debug_macinfo_section || debug_macinfo_offset >= debug_macinfo_section_size) return 0;

	DWARF_MacInfoListEntry<MEMORY_ADDR> *head = 0;
	do
	{
		typename std::map<uint64_t, DWARF_MacInfoListEntry<MEMORY_ADDR> *>::const_iterator dw_macinfo_list_iter = dw_macinfo_list.find(debug_macinfo_offset);
		
		if(dw_macinfo_list_iter != dw_macinfo_list.end())
		{
			return head ? head : (*dw_macinfo_list_iter).second;
		}
		
		uint8_t dw_mac_info_type = *((const uint8_t *) debug_macinfo_section + debug_macinfo_offset);

		DWARF_MacInfoListEntry<MEMORY_ADDR> *dw_macinfo_list_entry = 0;

		switch(dw_mac_info_type)
		{
			case 0: // Null entry
				dw_macinfo_list_entry = new DWARF_MacInfoListEntryNull<MEMORY_ADDR>();
				break;
			case DW_MACINFO_define:
				dw_macinfo_list_entry = new DWARF_MacInfoListEntryDefine<MEMORY_ADDR>();
				break;
			case DW_MACINFO_undef:
				dw_macinfo_list_entry = new DWARF_MacInfoListEntryUndef<MEMORY_ADDR>();
				break;
			case DW_MACINFO_start_file:
				dw_macinfo_list_entry = new DWARF_MacInfoListEntryStartFile<MEMORY_ADDR>();
				break;
			case DW_MACINFO_end_file:
				dw_macinfo_list_entry = new DWARF_MacInfoListEntryEndFile<MEMORY_ADDR>();
				break;
			case DW_MACINFO_vendor_ext:
				dw_macinfo_list_entry = new DWARF_MacInfoListEntryVendorExtension<MEMORY_ADDR>();
				break;
			default:
				std::cerr << "Invalid DWARF2 debug macinfo at offset 0x" << std::hex << debug_macinfo_offset << std::dec << " (unknown type " << ((unsigned int) dw_mac_info_type) << ")" << std::endl;
				return 0;
		}
			
		if(!dw_macinfo_list_entry) return head;

		int64_t sz;
			
		if((sz = dw_macinfo_list_entry->Load((const uint8_t *) debug_macinfo_section + debug_macinfo_offset, debug_macinfo_section_size - debug_macinfo_offset, debug_macinfo_offset)) < 0)
		{
			std::cerr << "Invalid DWARF2 debug macinfo at offset 0x" << std::hex << debug_macinfo_offset << std::dec << " (type " << ((unsigned int) dw_mac_info_type) << ")" << std::endl;
			delete dw_macinfo_list_entry;
			return head;
		}

		//std::cerr << *dw_macinfo_list_entry << std::endl;
		Register(dw_macinfo_list_entry);
		debug_macinfo_offset += sz;
		if(!head) head = dw_macinfo_list_entry;
		if(dw_macinfo_list_entry->GetType() == 0) break; // Null entry, i.e. End of list
	} while(debug_macinfo_offset < debug_macinfo_section_size);
	
	return head;
}

template <class MEMORY_ADDR>
const DWARF_CompilationUnit<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindCompilationUnit(uint64_t debug_info_offset) const
{
	typename std::map<uint64_t, DWARF_CompilationUnit<MEMORY_ADDR> *>::const_iterator dw_cu_iter = dw_cus.find(debug_info_offset);
	
	return dw_cu_iter != dw_cus.end() ? (*dw_cu_iter).second : 0;
}

template <class MEMORY_ADDR>
const DWARF_LocListEntry<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindLocListEntry(const DWARF_CompilationUnit<MEMORY_ADDR> *dw_cu, uint64_t debug_loc_offset)
{
	if(!debug_loc_section || debug_loc_offset >= debug_loc_section_size) return 0;
	
	DWARF_LocListEntry<MEMORY_ADDR> *head = 0;
	
	do
	{
		typename std::map<uint64_t, DWARF_LocListEntry<MEMORY_ADDR> *>::const_iterator dw_loc_list_iter = dw_loc_list.find(debug_loc_offset);
			
		if(dw_loc_list_iter != dw_loc_list.end())
		{
			return head ? head : (*dw_loc_list_iter).second;
		}
			
		DWARF_LocListEntry<MEMORY_ADDR> *dw_loc_list_entry = new DWARF_LocListEntry<MEMORY_ADDR>(dw_cu);
		int64_t sz;
			
		if((sz = dw_loc_list_entry->Load((const uint8_t *) debug_loc_section + debug_loc_offset, debug_loc_section_size - debug_loc_offset, debug_loc_offset)) < 0)
		{
			std::cerr << "Invalid DWARF2 debug loc at offset 0x" << std::hex << debug_loc_offset << std::dec << std::endl;
			delete dw_loc_list_entry;
			return head;
		}

		//std::cerr << *dw_loc_list_entry << std::endl;
		Register(dw_loc_list_entry);
		debug_loc_offset += sz;
		if(!head) head = dw_loc_list_entry;
		if(dw_loc_list_entry->IsEndOfList())
		{
			break; // End of list
		}
	} while(debug_loc_offset < debug_loc_section_size);
	
	return head;
}

template <class MEMORY_ADDR>
const DWARF_CIE<MEMORY_ADDR> *DWARF_Handler<MEMORY_ADDR>::FindCIE(uint64_t debug_frame_offset) const
{
	typename std::map<uint64_t, DWARF_CIE<MEMORY_ADDR> *>::const_iterator dw_cie_iter = dw_cies.find(debug_frame_offset);
	
	return dw_cie_iter != dw_cies.end() ? (*dw_cie_iter).second : 0;
}

} // end of namespace elf_loader
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim

#endif
