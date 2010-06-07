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
#include <limits>

namespace unisim {
namespace service {
namespace loader {
namespace elf_loader {

template <typename T>
bool DWARF_LEB128::Fit(const T *t) const
{
	return 8 * sizeof(T) >= GetBitLength();
}

template <typename T>
DWARF_LEB128::operator T() const
{
	T result = 0;
	unsigned int shift = 0;
	unsigned int size = 8 * sizeof(T);
	uint8_t *p = leb128;
	uint8_t byte;
	do
	{
		byte = *p++;
		result |= (byte & 0x7f) << shift;
		shift += 7;
	}
	while(byte & 0x80);

	if(std::numeric_limits<T>::is_signed)
	{
		if((shift < size) && (byte & 0x40))
		result |= -(1 << shift);
	}
	return result;
}

template <class MEMORY_ADDR>
DWARF_StatementProgram<MEMORY_ADDR>::DWARF_StatementProgram(endian_type _endianness)
	: endianness(_endianness)
	, total_length(0)
	, version(0)
	, prologue_length(0)
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
	if(program)
	{
		delete[] program;
	}
}

template <class MEMORY_ADDR>
int DWARF_StatementProgram<MEMORY_ADDR>::Load(const uint8_t *rawdata, unsigned int max_size)
{
	if(program)
	{
		delete[] program;
	}
	standard_opcode_lengths.clear();
	include_directories.clear();
	filenames.clear();
	
	int size = 0;

	// padding should be 0 in fully compliant DWARF2 .debug_line section
	unsigned int padding = 0;
	do
	{
		if(max_size < sizeof(total_length)) return -1;
		memcpy(&total_length, rawdata, sizeof(total_length));
		total_length = Target2Host(endianness, total_length);
		rawdata += sizeof(total_length);
		max_size -= sizeof(total_length);
		size += sizeof(total_length);
		
		if(max_size < sizeof(version)) return -1;
		memcpy(&version, rawdata, sizeof(version));
		version = Target2Host(endianness, version);
		rawdata += sizeof(version);
		max_size -= sizeof(version);
		size += sizeof(version);
		
		if(max_size < sizeof(prologue_length)) return -1;
		memcpy(&prologue_length, rawdata, sizeof(prologue_length));
		prologue_length = Target2Host(endianness, prologue_length);
		rawdata += sizeof(prologue_length);
		max_size -= sizeof(prologue_length);
		size += sizeof(prologue_length);

		if((version == 2) && (prologue_length <= (total_length - sizeof(version) - sizeof(prologue_length))) && ((total_length - sizeof(version) - sizeof(prologue_length)) <= max_size)) break;
		// retry with a different padding
		rawdata -= size - 1;
		max_size += size - 1;
		size = 0;
	} while(++padding < 4);

	if((version != 2) || (prologue_length > (total_length - sizeof(version) - sizeof(prologue_length))) || ((total_length - sizeof(version) - sizeof(prologue_length)) > max_size)) return -1;

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
		std::string directory_name = std::string((const char *) rawdata);
		int sz = directory_name.length() + 1;
		size += sz;
		rawdata += sz;
		max_size -= sz;
		
		if(directory_name.empty()) break;
		
		include_directories.push_back(directory_name);
	} while(1);

	do
	{
		if(!max_size) return -1;
		if(!(*rawdata)) break;
		
		DWARF_Filename filename;
			
		int sz;
		if((sz = filename.Load(rawdata, max_size)) < 0) return -1;
		size += sz;
		rawdata += sz;
		max_size -= sz;
		
		filenames.push_back(filename);
	} while(1);


	if(size > prologue_length + sizeof(prologue_length) + sizeof(version) + sizeof(total_length)) return -1;
	rawdata += prologue_length + sizeof(prologue_length) + sizeof(version) + sizeof(total_length) - size;
	max_size -= prologue_length + sizeof(prologue_length) + sizeof(version) + sizeof(total_length) - size;
	size = prologue_length + sizeof(prologue_length) + sizeof(version) + sizeof(total_length);
	program_length = total_length + sizeof(total_length) - size;
	if(program_length > max_size) return -1;
	program = new uint8_t[program_length];
	memcpy(program, rawdata, program_length);
	size += program_length;

	return size + padding;
}

template <class MEMORY_ADDR>
std::ostream& operator << (std::ostream& os, const DWARF_StatementProgram<MEMORY_ADDR>& dw_stmt_prog)
{
	unsigned int i;

	os << "Statement program:" << std::endl
	   << " - Total Length: " << dw_stmt_prog.total_length << std::endl
	   << " - DWARF Version: " << dw_stmt_prog.version << std::endl
	   << " - Prologue Length: " << dw_stmt_prog.prologue_length << std::endl
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
	
	DWARF_StatementVM<MEMORY_ADDR> dw_stmt_vm = DWARF_StatementVM<MEMORY_ADDR>(dw_stmt_prog.endianness);
	
	dw_stmt_vm.Run(&dw_stmt_prog, &os, 0);
	return os;
	/*
	const uint8_t *program = dw_stmt_prog.program;
	unsigned int count = dw_stmt_prog.program_length;
	uint8_t opcode;

	if(count > 0)
	{
		do
		{
			opcode = *program;
			program++;
			count--;
			if(opcode)
			{
				if(opcode < dw_stmt_prog.opcode_base)
				{
					unsigned int num_args = dw_stmt_prog.standard_opcode_lengths[opcode - 1];
					unsigned int i;
					DWARF_LEB128 args[num_args];
					uint16_t uhalf_arg;
					
					if(opcode != DW_LNS_fixed_advance_pc)
					{
						for(i = 0; i < num_args; i++)
						{
							int sz;
							if((sz = args[i].Load(program, count)) < 0)
							{
								std::cerr << "Bad LEB128" << std::endl;
								return os;
							}
							program += sz;
							count -= sz;
						}
					}
					else
					{
						if(count < sizeof(uhalf_arg))
						{
							std::cerr << "Bad uhalf operand" << std::endl;
							return os;
						}
						memcpy(&uhalf_arg, program, sizeof(uhalf_arg));
						uhalf_arg = Target2Host(dw_stmt_prog.endianness, uhalf_arg);
						program += sizeof(uhalf_arg);
						count -= sizeof(uhalf_arg);
					}
					
					// Standard opcode
					os << "   - Standard opcode " << (unsigned int) opcode << ": ";
					switch(opcode)
					{
						case DW_LNS_copy:
							os << "Copy" << std::endl;
							break;
						case DW_LNS_advance_pc:
							{
								uint64_t pc_increment = (uint64_t) args[0] * dw_stmt_prog.minimum_instruction_length;

								os << "Advance PC by " << pc_increment << std::endl;
							}
							break;
						case DW_LNS_advance_line:
							{
								const DWARF_LEB128& line_increment = args[0];

								os << "Advance Line by " << line_increment.to_string(true) << std::endl;
							}
							break;
						case DW_LNS_set_file:
							{
								const DWARF_LEB128& file_entry = args[0];
								
								os << "Set File to entry #" << file_entry.to_string(false) << " in Filenames Table" << std::endl;
							}
							break;
						case DW_LNS_set_column:
							{
								const DWARF_LEB128& column = args[0];
								
								os << "Set Column to " << column.to_string(false) << std::endl;
							}
							break;
						case DW_LNS_negate_stmt:
							os << "Negate is_stmt" << std::endl;
							break;
						case DW_LNS_set_basic_block:
							os << "Set basic block" << std::endl;
							break;
						case DW_LNS_const_add_pc:
							{
								int address_increment = ((255 - dw_stmt_prog.opcode_base) / dw_stmt_prog.line_range) * dw_stmt_prog.minimum_instruction_length;
								os << "Advance PC by constant " << address_increment << std::endl;
							}
							break;
						case DW_LNS_fixed_advance_pc:
							os << "Advance PC by fixed " << uhalf_arg << std::endl;
							break;
						default:
							os << "Skipped unknown" << std::endl;
					}
				}
				else
				{
					// Special opcode
					uint8_t adjusted_opcode = opcode - dw_stmt_prog.opcode_base;
					int line_increment = dw_stmt_prog.line_base + (adjusted_opcode % dw_stmt_prog.line_range);
					int address_increment = (adjusted_opcode / dw_stmt_prog.line_range) * dw_stmt_prog.minimum_instruction_length;
					os << "   - Special opcode " << (unsigned int) adjusted_opcode << ": advance Address by " << address_increment << " and Line by " << line_increment << std::endl;
				}
			}
			else
			{
				// Extended opcode
				DWARF_LEB128 leb128_insn_length;
				int sz;
				if((sz = leb128_insn_length.Load(program, count)) < 0)
				{
					std::cerr << "Bad LEB128" << std::endl;
					return os;
				}
				program += sz;
				count -= sz;
				uint64_t insn_length = (uint64_t) leb128_insn_length;
				opcode = *program;
				program++;
				count--;
				os << "   - Extended opcode " << (unsigned int) opcode << ": ";
				
				switch(opcode)
				{
					case DW_LNE_end_sequence:
						os << "End of Sequence";
						return os;
						break;
					case DW_LNE_set_address:
						{
							os << "Set Address to ";
							uint64_t addr_length = insn_length - 1;
							switch(addr_length)
							{
								case sizeof(uint32_t):
									{
										uint32_t addr;
										memcpy(&addr, program, sizeof(uint32_t));
										addr = Target2Host(dw_stmt_prog.endianness, addr);
										os << std::hex << "0x" << addr << std::dec;
									}
									break;
								case sizeof(uint64_t):
									{
										uint64_t addr;
										memcpy(&addr, program, sizeof(uint64_t));
										addr = Target2Host(dw_stmt_prog.endianness, addr);
										os << std::hex << "0x" << addr << std::dec;
									}
									break;
								default:
									std::cerr << "Bad address" << std::endl;
									return os;
							}
						}
						break;
					case DW_LNE_define_file:
						{
							DWARF_Filename dw_filename;
							int sz;
							if((sz = dw_filename.Load(program, count)) < 0)
							{
								std::cerr << "Bad DWARF filename entry" << std::endl;
								return os;
							}
							os << "Define File " << dw_filename << std::endl;
						}
						break;
					default:
						std::cerr << "Unknown extended opcode " << (unsigned int) opcode << std::endl;
						return os;
				}
				program += insn_length - 1;
				count -= insn_length - 1;
				std::cerr << std::endl;
			}
		}
		while(count);
	}

	return os;*/
}

template <class MEMORY_ADDR>
DWARF_StatementVM<MEMORY_ADDR>::DWARF_StatementVM(endian_type _endianness)
	: endianness(_endianness)
	, address(0)
	, file(1)
	, line(1)
	, column(0)
	, is_stmt(false)
	, basic_block(false)
	, end_sequence(false)
	, filenames()
{
}

template <class MEMORY_ADDR>
DWARF_StatementVM<MEMORY_ADDR>::~DWARF_StatementVM()
{
}

template <class MEMORY_ADDR>
void DWARF_StatementVM<MEMORY_ADDR>::AddRow(const DWARF_StatementProgram<MEMORY_ADDR> *dw_stmt_prog, std::map<MEMORY_ADDR, Statement<MEMORY_ADDR> *>& matrix)
{
	const DWARF_Filename *dw_filename = (file >= 1 && file <= filenames.size()) ? &filenames[file - 1] : 0;
	const std::string *filename = dw_filename ? dw_filename->GetFilename() : 0;
	const std::string *dirname = 0;
	if(dw_filename)
	{
		const DWARF_LEB128& leb128_dir = dw_filename->GetDirectoryIndex();
		unsigned int dir = (unsigned int) leb128_dir;
		dirname = (dir >= 1 && dir <= dw_stmt_prog->include_directories.size()) ? &dw_stmt_prog->include_directories[dir - 1] : 0;
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
	filenames = dw_stmt_prog->filenames;

	// Run the program
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
							int sz;
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
					if(os) (*os) << "   - Special opcode " << (unsigned int) adjusted_opcode << ": advance Address by " << address_increment << " to 0x" << std::hex << address << std::dec << " and Line by " << line_increment << " to " << line << std::endl;
				}
			}
			else
 			{
				// Extended opcode
				DWARF_LEB128 leb128_insn_length;
				int sz;
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
							DWARF_Filename dw_filename;
							int sz;
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

} // end of namespace elf_loader
} // end of namespace loader
} // end of namespace service
} // end of namespace unisim

#endif
