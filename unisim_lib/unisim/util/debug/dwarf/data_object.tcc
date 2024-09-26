/*
 *  Copyright (c) 2012,
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

#ifndef __UNISIM_UTIL_DEBUG_DWARF_DATA_OBJECT_TCC__
#define __UNISIM_UTIL_DEBUG_DWARF_DATA_OBJECT_TCC__

#include <unisim/util/endian/endian.hh>
#include <cstring>

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
DWARF_DataObject<MEMORY_ADDR>::DWARF_DataObject(const DWARF_Handler<MEMORY_ADDR> *_dw_handler, const DWARF_MachineState<MEMORY_ADDR> *_dw_mach_state, unsigned int _prc_num, const char *_data_object_name, const CLocOperationStream *_c_loc_operation_stream, const DWARF_Location<MEMORY_ADDR> *_dw_data_object_loc, const DWARF_DIE<MEMORY_ADDR> *_dw_die_type, unsigned int _dim, const unisim::util::debug::Type *_dw_data_object_type)
	: dw_handler(_dw_handler)
	, dw_mach_state(_dw_mach_state)
	, prc_num(_prc_num)
	, data_object_name(_data_object_name)
	, c_loc_operation_stream(_c_loc_operation_stream)
	, dw_data_object_loc(_dw_data_object_loc)
	, dw_die_type(_dw_die_type)
	, dim(_dim)
	, dw_data_object_type(_dw_data_object_type)
	, arch_endianness(dw_handler->GetArchEndianness())
	, arch_address_size(dw_handler->GetArchAddressSize())
	, mem_if(dw_mach_state->GetMemoryInterface(prc_num))
	, bv(arch_endianness)
	, hold(false)
	, debug(dw_handler->GetOptionFlag(OPT_DEBUG))
{
	if(dw_data_object_type)
	{
		dw_data_object_type->Catch();
	}
}
	
template <class MEMORY_ADDR>
DWARF_DataObject<MEMORY_ADDR>::~DWARF_DataObject()
{
	if(c_loc_operation_stream) delete c_loc_operation_stream;
	if(dw_data_object_loc) delete dw_data_object_loc;
	if(dw_data_object_type)
	{
		dw_data_object_type->Release();
	}
}

template <class MEMORY_ADDR>
const char *DWARF_DataObject<MEMORY_ADDR>::GetName() const
{
	return data_object_name.c_str();
}

template <class MEMORY_ADDR>
unsigned int DWARF_DataObject<MEMORY_ADDR>::GetProcessorNumber() const
{
	return prc_num;
}

template <class MEMORY_ADDR>
MEMORY_ADDR DWARF_DataObject<MEMORY_ADDR>::GetBitSize() const
{
	return dw_data_object_loc ? dw_data_object_loc->GetBitSize() : 0;
}

template <class MEMORY_ADDR>
const unisim::util::debug::Type *DWARF_DataObject<MEMORY_ADDR>::GetType() const
{
	return dw_data_object_type;
}

template <class MEMORY_ADDR>
unisim::util::endian::endian_type DWARF_DataObject<MEMORY_ADDR>::GetEndian() const
{
	return arch_endianness;
}

template <class MEMORY_ADDR>
bool DWARF_DataObject<MEMORY_ADDR>::IsOptimizedOut() const
{
	return !dw_data_object_loc || (dw_data_object_loc->GetType() == DW_LOC_NULL);
}

template <class MEMORY_ADDR>
bool DWARF_DataObject<MEMORY_ADDR>::GetAddress(MEMORY_ADDR& addr) const
{
	if(!dw_data_object_loc || (dw_data_object_loc->GetType() != DW_LOC_SIMPLE_MEMORY)) return false;
	
	addr = dw_data_object_loc->GetAddress();
	addr += dw_data_object_loc->GetBitOffset() / 8;
	
	return true;
}

template <class MEMORY_ADDR>
bool DWARF_DataObject<MEMORY_ADDR>::Seek() const
{
	if(hold) return true;
	
	if(dw_data_object_loc)
	{
		delete dw_data_object_loc;
		dw_data_object_loc = 0;
	}
	if(dw_data_object_type)
	{
		dw_data_object_type->Release();
		dw_data_object_type = 0;
	}
	
	typename DWARF_Handler<MEMORY_ADDR>::FindDataObjectArguments args;
	args.c_loc_operation_stream = *c_loc_operation_stream;
	args.dim = dim;

	if(dw_handler->FindDataObject(dw_mach_state, prc_num, args))
	{
		dw_data_object_loc = args.dw_data_object_loc;
		dw_data_object_type = args.dw_data_object_type;
		dw_die_type = args.dw_die_type;
		dim = args.dim;
		if(dw_data_object_type)
		{
			dw_data_object_type->Catch();
		}
		return true;
	}
	
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DataObject<MEMORY_ADDR>::Fetch() const
{
	if(debug)
	{
		const std::set<std::pair<MEMORY_ADDR, MEMORY_ADDR> >& ranges = dw_data_object_loc->GetRanges();
		
		std::ostream& debug_info_stream = dw_handler->GetDebugInfoStream() << "PC ranges for which data object location is valid are ";
		typename std::set<std::pair<MEMORY_ADDR, MEMORY_ADDR> >::const_iterator it;
		for(it = ranges.begin(); it != ranges.end(); it++)
		{
			debug_info_stream << "[0x" << std::hex << it->first << std::dec << "-0x" << std::hex << it->second << std::dec << "[ ";
		}
		debug_info_stream << std::endl;
	}
	
	bv.Clear();
	
	// Note: bit offset is applied later at Read/Write for composite location descriptions, while it is applied immediately at fetch/commit for simpler location descriptions
	switch(dw_data_object_loc->GetType())
	{
		case DW_LOC_SIMPLE_MEMORY:
			{
				MEMORY_ADDR dw_addr = dw_data_object_loc->GetAddress();
				uint64_t dw_data_object_byte_size = dw_data_object_loc->GetByteSize();
				int64_t dw_data_object_bit_offset = dw_data_object_loc->GetBitOffset();
				uint64_t dw_data_object_bit_size = dw_data_object_loc->GetBitSize();
				MEMORY_ADDR dw_byte_size = dw_data_object_byte_size;
				if(debug)
				{
					dw_handler->GetDebugInfoStream() << "DW_LOC_SIMPLE_MEMORY: addr=0x" << std::hex << dw_addr << std::dec << ", bit_offset=" << dw_data_object_bit_offset << ", bit_size=" << dw_data_object_bit_size << ", byte_size=" << dw_data_object_byte_size << std::endl;
				}
				uint8_t buffer[dw_byte_size];
				memset(buffer, 0, dw_byte_size);
				if(dw_data_object_bit_offset >= 0)
				{
					MEMORY_ADDR addr = dw_addr + (dw_data_object_bit_offset / 8);
					if(!mem_if->ReadMemory(addr, buffer, dw_byte_size))
					{
						dw_handler->GetGeneralDebugErrorStream() << "Can't read memory at 0x" << std::hex << addr << std::dec << " (" << dw_byte_size << " bytes)" << std::endl;
						return false;
					}
					
					bv.Append(buffer, dw_data_object_bit_offset % 8, dw_data_object_bit_size);
				}
				else
				{
					MEMORY_ADDR addr = dw_addr + ((dw_data_object_bit_offset - 7) / 8);
					if(!mem_if->ReadMemory(addr, buffer, dw_byte_size))
					{
						dw_handler->GetGeneralDebugErrorStream() << "Can't read memory at 0x" << addr << std::dec << " (" << dw_byte_size << " bytes)" << std::endl;
						return false;
					}

					bv.Append(buffer, (dw_data_object_bit_offset % 8) ? (8 + (dw_data_object_bit_offset % 8)) : (dw_data_object_bit_offset % 8), dw_data_object_bit_size);
				}
			}
			return true;
		case DW_LOC_SIMPLE_REGISTER:
			{
				int64_t dw_data_object_bit_offset = dw_data_object_loc->GetBitOffset();
				uint64_t dw_data_object_bit_size = dw_data_object_loc->GetBitSize();

				unsigned int dw_reg_num = dw_data_object_loc->GetRegisterNumber();
				DWARF_Frame<MEMORY_ADDR> *dw_curr_frame = dw_mach_state->GetCurrentFrame(prc_num);
				const DWARF_Register<MEMORY_ADDR> *dw_reg = dw_curr_frame->GetRegister(dw_reg_num);
				if(debug)
				{
					dw_handler->GetDebugInfoStream() << "DW_LOC_SIMPLE_REGISTER: dw_reg_num=" << dw_reg_num << " (" << (dw_reg ? dw_reg->GetName() : "?") << ")" << std::endl;
				}
				if(!dw_reg) return false;
				
				unsigned int reg_size = dw_reg->GetSize(); // FIXME: Get true bit size
				
				switch(reg_size)
				{
					case 1:
						{
							uint8_t value = 0;
							dw_reg->GetValue(&value);
							bv.Append(value, (arch_endianness == unisim::util::endian::E_BIG_ENDIAN) ? 8 - dw_data_object_bit_size - dw_data_object_bit_offset : dw_data_object_bit_offset, dw_data_object_bit_size);
						}
						break;
					case 2:
						{
							uint16_t value = 0;
							dw_reg->GetValue(&value);
							bv.Append(value, (arch_endianness == unisim::util::endian::E_BIG_ENDIAN) ? 16 - dw_data_object_bit_size - dw_data_object_bit_offset : dw_data_object_bit_offset, dw_data_object_bit_size);
						}
						break;
					case 4:
						{
							uint32_t value = 0;
							dw_reg->GetValue(&value);
							bv.Append(value, (arch_endianness == unisim::util::endian::E_BIG_ENDIAN) ? 32 - dw_data_object_bit_size - dw_data_object_bit_offset : dw_data_object_bit_offset, dw_data_object_bit_size);
						}
						break;
					case 8:
						{
							uint64_t value = 0;
							dw_reg->GetValue(&value);
							bv.Append(value, (arch_endianness == unisim::util::endian::E_BIG_ENDIAN) ? 64 - dw_data_object_bit_size - dw_data_object_bit_offset : dw_data_object_bit_offset, dw_data_object_bit_size);
						}
						break;
					default:
						return false;
				}
			}
			return true;
		case DW_LOC_COMPOSITE:
			{
				const std::vector<DWARF_LocationPiece<MEMORY_ADDR> *>& dw_loc_pieces = dw_data_object_loc->GetLocationPieces();

				typename std::vector<DWARF_LocationPiece<MEMORY_ADDR> *>::const_iterator dw_loc_piece_iter;
				for(dw_loc_piece_iter = dw_loc_pieces.begin(); dw_loc_piece_iter != dw_loc_pieces.end(); dw_loc_piece_iter++)
				{
					DWARF_LocationPiece<MEMORY_ADDR> *dw_loc_piece = *dw_loc_piece_iter;
					
					switch(dw_loc_piece->GetType())
					{
						case DW_LOC_PIECE_REGISTER:
							{
								DWARF_RegisterLocationPiece<MEMORY_ADDR> *dw_reg_loc_piece = (DWARF_RegisterLocationPiece<MEMORY_ADDR> *) dw_loc_piece;
								
								unsigned int dw_reg_num = dw_reg_loc_piece->GetRegisterNumber();
								unsigned int dw_bit_offset = dw_reg_loc_piece->GetBitOffset();
								unsigned int dw_bit_size = dw_reg_loc_piece->GetBitSize();
								DWARF_Frame<MEMORY_ADDR> *dw_curr_frame = dw_mach_state->GetCurrentFrame(prc_num);
								const DWARF_Register<MEMORY_ADDR> *dw_reg = dw_curr_frame->GetRegister(dw_reg_num);
								
								if(debug)
								{
									dw_handler->GetDebugInfoStream() << "DW_LOC_PIECE_REGISTER: dw_reg_num=" << dw_reg_num << " (" << (dw_reg ? dw_reg->GetName() : "?") << "), dw_bit_offset=" << dw_bit_offset << ", dw_bit_size=" << dw_bit_size << std::endl;
								}
								
								if(!dw_reg) return false;
								
								unsigned int reg_size = dw_reg->GetSize(); // FIXME: Get true bit size
								
								switch(reg_size)
								{
									case 1:
										{
											uint8_t value = 0;
											dw_reg->GetValue(&value);
											bv.Append(value, (arch_endianness == unisim::util::endian::E_BIG_ENDIAN) ? 8 - dw_bit_size - dw_bit_offset: dw_bit_offset, dw_bit_size);
										}
										break;
									case 2:
										{
											uint16_t value = 0;
											dw_reg->GetValue(&value);
											bv.Append(value, (arch_endianness == unisim::util::endian::E_BIG_ENDIAN) ? 16 - dw_bit_size - dw_bit_offset: dw_bit_offset, dw_bit_size);
										}
										break;
									case 4:
										{
											uint32_t value = 0;
											dw_reg->GetValue(&value);
											bv.Append(value, (arch_endianness == unisim::util::endian::E_BIG_ENDIAN) ? 32 - dw_bit_size - dw_bit_offset: dw_bit_offset, dw_bit_size);
										}
										break;
									case 8:
										{
											uint64_t value = 0;
											dw_reg->GetValue(&value);
											bv.Append(value, (arch_endianness == unisim::util::endian::E_BIG_ENDIAN) ? 64 - dw_bit_size - dw_bit_offset: dw_bit_offset, dw_bit_size);
										}
										break;
									default:
										return false;
								}
							}
							break;
						case DW_LOC_PIECE_MEMORY:
							{
								DWARF_MemoryLocationPiece<MEMORY_ADDR> *dw_mem_loc_piece = (DWARF_MemoryLocationPiece<MEMORY_ADDR> *) dw_loc_piece;
								
								MEMORY_ADDR dw_addr = dw_mem_loc_piece->GetAddress();
								unsigned int dw_bit_offset = dw_mem_loc_piece->GetBitOffset();
								unsigned int dw_bit_size = dw_mem_loc_piece->GetBitSize();
								if(debug)
								{
									dw_handler->GetDebugInfoStream() << "DW_LOC_PIECE_MEMORY: dw_addr=0x" << std::hex << dw_addr << std::dec << ", dw_bit_offset=" << dw_bit_offset << ", dw_bit_size=" << dw_bit_size << std::endl;
								}
								
								MEMORY_ADDR dw_byte_size = dw_bit_size ? (dw_bit_size + dw_bit_offset + 7) / 8 : arch_address_size;
								uint8_t buffer[dw_byte_size];
								memset(buffer, 0, dw_byte_size);
								if(!mem_if->ReadMemory(dw_addr, buffer, dw_byte_size)) return false;
								
								bv.Append(buffer, dw_bit_offset, dw_bit_size);
							}
							break;
					}
					
				}
				return true;
			}
			break;
			
		case DW_LOC_IMPLICIT_SIMPLE_VALUE:
			{
				int64_t dw_data_object_bit_offset = dw_data_object_loc->GetBitOffset();
				uint64_t dw_data_object_bit_size = dw_data_object_loc->GetBitSize();
				MEMORY_ADDR dw_implicit_simple_value = dw_data_object_loc->GetImplicitSimpleValue();
				
				if(debug)
				{
					dw_handler->GetDebugInfoStream() << "DW_LOC_IMPLICIT_SIMPLE_VALUE: dw_implicit_simple_value=0x" << std::hex << dw_implicit_simple_value << std::dec << std::endl;
				}
				
				bv.Append(dw_implicit_simple_value, dw_data_object_bit_offset, dw_data_object_bit_size);
				
				return true;
			}
			break;
		case DW_LOC_IMPLICIT_BLOCK_VALUE:
			{
				const DWARF_Block<MEMORY_ADDR> *dw_implicit_block_value = dw_data_object_loc->GetImplicitBlockValue();
				
				int64_t dw_data_object_bit_offset = dw_data_object_loc->GetBitOffset();
				
				// compute min(die bit size, block bit length)
				uint64_t dw_data_object_bit_size = dw_data_object_loc->GetBitSize();
				uint64_t dw_block_bit_size = 8 * dw_implicit_block_value->GetLength();
				if(dw_data_object_bit_size > dw_block_bit_size) dw_data_object_bit_size = dw_block_bit_size;
				
				if(debug)
				{
					dw_handler->GetDebugInfoStream() << "DW_LOC_IMPLICIT_BLOCK_VALUE: dw_implicit_block_value=" << (*dw_implicit_block_value) << std::endl;
				}
				
				bv.Append(dw_implicit_block_value->GetValue(), dw_data_object_bit_offset, dw_data_object_bit_size);
				
				return true;
			}
			break;
		default:
			return false;
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DataObject<MEMORY_ADDR>::Commit()
{
	if(!dw_data_object_loc) return false;
	
	unsigned int source_bit_offset = 0;
	
	switch(dw_data_object_loc->GetType())
	{
		case DW_LOC_SIMPLE_MEMORY:
			{
				uint64_t dw_data_object_byte_size = dw_data_object_loc->GetByteSize();
				int64_t dw_data_object_bit_offset = dw_data_object_loc->GetBitOffset();
				uint64_t dw_data_object_bit_size = dw_data_object_loc->GetBitSize();

				MEMORY_ADDR dw_addr = dw_data_object_loc->GetAddress();
				MEMORY_ADDR dw_byte_size = dw_data_object_byte_size;
				if(debug)
				{
					dw_handler->GetDebugInfoStream() << "DW_LOC_SIMPLE_MEMORY: addr=0x" << std::hex << dw_addr << std::dec << ", bit_offset=" << dw_data_object_bit_offset << ", bit_size=" << dw_data_object_bit_size << ", byte_size=" << dw_data_object_byte_size << std::endl;
				}
				uint8_t buffer[dw_byte_size];
				memset(buffer, 0, dw_byte_size);
				if(dw_data_object_bit_offset >= 0)
				{
					if(!bv.Read(source_bit_offset, buffer, dw_data_object_bit_offset % 8, dw_data_object_bit_size)) return false;
					source_bit_offset += dw_data_object_bit_size;
					if(!mem_if->WriteMemory(dw_addr + (dw_data_object_bit_offset / 8), buffer, dw_byte_size)) return false;
				}
				else
				{
					if(!bv.Read(source_bit_offset, buffer, 8 + (dw_data_object_bit_offset % 8), dw_data_object_bit_size)) return false;
					source_bit_offset += dw_data_object_bit_size;
					if(!mem_if->WriteMemory(dw_addr + (dw_data_object_bit_offset / 8) - 1, buffer, dw_byte_size)) return false;
				}
			}
			return true;
		case DW_LOC_SIMPLE_REGISTER:
			{
				int64_t dw_data_object_bit_offset = dw_data_object_loc->GetBitOffset();
				uint64_t dw_data_object_bit_size = dw_data_object_loc->GetBitSize();

				unsigned int dw_reg_num = dw_data_object_loc->GetRegisterNumber();
				DWARF_Frame<MEMORY_ADDR> *dw_curr_frame = dw_mach_state->GetCurrentFrame(prc_num);
				DWARF_Register<MEMORY_ADDR> *dw_reg = dw_curr_frame->GetRegister(dw_reg_num);
				if(debug)
				{
					dw_handler->GetDebugInfoStream() << "DW_LOC_SIMPLE_REGISTER: dw_reg_num=" << dw_reg_num << std::endl;
				}
				if(!dw_reg) return false;
				
				unsigned int reg_size = dw_reg->GetSize(); // FIXME: Get true bit size
				
				switch(reg_size)
				{
					case 1:
						{
							uint64_t value = 0;
							if(!bv.Read(source_bit_offset, value, (arch_endianness == unisim::util::endian::E_BIG_ENDIAN) ? 8 - dw_data_object_bit_size - dw_data_object_bit_offset : dw_data_object_bit_offset, dw_data_object_bit_size)) return false;
							source_bit_offset += dw_data_object_bit_size;
							uint8_t value8 = value;
							dw_reg->SetValue(&value8);
						}
						break;
					case 2:
						{
							uint64_t value = 0;
							if(!bv.Read(source_bit_offset, value, (arch_endianness == unisim::util::endian::E_BIG_ENDIAN) ? 16 - dw_data_object_bit_size - dw_data_object_bit_offset : dw_data_object_bit_offset, dw_data_object_bit_size)) return false;
							source_bit_offset += dw_data_object_bit_size;
							uint16_t value16 = value;
							dw_reg->SetValue(&value16);
						}
						break;
					case 4:
						{
							uint64_t value = 0;
							if(!bv.Read(source_bit_offset, value, (arch_endianness == unisim::util::endian::E_BIG_ENDIAN) ? 32 - dw_data_object_bit_size - dw_data_object_bit_offset : dw_data_object_bit_offset, dw_data_object_bit_size)) return false;
							source_bit_offset += dw_data_object_bit_size;
							uint32_t value32 = value;
							dw_reg->SetValue(&value32);
						}
						break;
					case 8:
						{
							uint64_t value = 0;
							if(!bv.Read(source_bit_offset, value, (arch_endianness == unisim::util::endian::E_BIG_ENDIAN) ? 64 - dw_data_object_bit_size - dw_data_object_bit_offset : dw_data_object_bit_offset, dw_data_object_bit_size)) return false;
							source_bit_offset += dw_data_object_bit_size;
							dw_reg->SetValue(&value);
						}
						break;
					default:
						return false;
				}
			}
			return true;
		case DW_LOC_COMPOSITE:
			{
				const std::vector<DWARF_LocationPiece<MEMORY_ADDR> *>& dw_loc_pieces = dw_data_object_loc->GetLocationPieces();

				typename std::vector<DWARF_LocationPiece<MEMORY_ADDR> *>::const_iterator dw_loc_piece_iter;
				for(dw_loc_piece_iter = dw_loc_pieces.begin(); dw_loc_piece_iter != dw_loc_pieces.end(); dw_loc_piece_iter++)
				{
					DWARF_LocationPiece<MEMORY_ADDR> *dw_loc_piece = *dw_loc_piece_iter;
					
					switch(dw_loc_piece->GetType())
					{
						case DW_LOC_PIECE_REGISTER:
							{
								DWARF_RegisterLocationPiece<MEMORY_ADDR> *dw_reg_loc_piece = (DWARF_RegisterLocationPiece<MEMORY_ADDR> *) dw_loc_piece;
								
								unsigned int dw_reg_num = dw_reg_loc_piece->GetRegisterNumber();
								unsigned int dw_bit_offset = dw_reg_loc_piece->GetBitOffset();
								unsigned int dw_bit_size = dw_reg_loc_piece->GetBitSize();
								if(debug)
								{
									dw_handler->GetDebugInfoStream() << "DW_LOC_PIECE_REGISTER: dw_reg_num=" << dw_reg_num << ", dw_bit_offset=" << dw_bit_offset << ", dw_bit_size=" << dw_bit_size << std::endl;
								}
								
								DWARF_Frame<MEMORY_ADDR> *dw_curr_frame = dw_mach_state->GetCurrentFrame(prc_num);
								DWARF_Register<MEMORY_ADDR> *dw_reg = dw_curr_frame->GetRegister(dw_reg_num);
								if(!dw_reg) return false;
								
								unsigned int reg_size = dw_reg->GetSize(); // FIXME: Get true bit size
								
								switch(reg_size)
								{
									case 1:
										{
											uint8_t value8 = 0;
											dw_reg->GetValue(&value8);
											uint64_t value = 0;
											if(!bv.Read(source_bit_offset, value, (arch_endianness == unisim::util::endian::E_BIG_ENDIAN) ? 8 - dw_bit_size - dw_bit_offset : dw_bit_offset, dw_bit_size)) return false;
											source_bit_offset += dw_bit_size;
											uint8_t mask = ((1 << dw_bit_size) - 1) << dw_bit_offset;
											value8 = (value & ~mask) | (value & mask);
											dw_reg->SetValue(&value8);
										}
										break;
									case 2:
										{
											uint16_t value16 = 0;
											dw_reg->GetValue(&value16);
											uint64_t value = 0;
											if(!bv.Read(source_bit_offset, value, (arch_endianness == unisim::util::endian::E_BIG_ENDIAN) ? 16 - dw_bit_size - dw_bit_offset : dw_bit_offset, dw_bit_size)) return false;
											source_bit_offset += dw_bit_size;
											uint16_t mask = ((1 << dw_bit_size) - 1) << dw_bit_offset;
											value16 = (value & ~mask) | (value & mask);
											dw_reg->SetValue(&value16);
										}
										break;
									case 4:
										{
											uint8_t value32 = 0;
											dw_reg->GetValue(&value32);
											uint64_t value = 0;
											if(!bv.Read(source_bit_offset, value, (arch_endianness == unisim::util::endian::E_BIG_ENDIAN) ? 32 - dw_bit_size - dw_bit_offset : dw_bit_offset, dw_bit_size)) return false;
											source_bit_offset += dw_bit_size;
											uint16_t mask = ((1 << dw_bit_size) - 1) << dw_bit_offset;
											value32 = (value & ~mask) | (value & mask);
											dw_reg->SetValue(&value32);
										}
										break;
									case 8:
										{
											uint8_t value64 = 0;
											dw_reg->GetValue(&value64);
											uint64_t value = 0;
											if(!bv.Read(source_bit_offset, value, (arch_endianness == unisim::util::endian::E_BIG_ENDIAN) ? 64 - dw_bit_size - dw_bit_offset : dw_bit_offset, dw_bit_size)) return false;
											source_bit_offset += dw_bit_size;
											uint16_t mask = ((1 << dw_bit_size) - 1) << dw_bit_offset;
											value64 = (value & ~mask) | (value & mask);
											dw_reg->SetValue(&value64);
										}
										break;
									default:
										return false;
								}
							}
							break;
						case DW_LOC_PIECE_MEMORY:
							{
								DWARF_MemoryLocationPiece<MEMORY_ADDR> *dw_mem_loc_piece = (DWARF_MemoryLocationPiece<MEMORY_ADDR> *) dw_loc_piece;
								
								MEMORY_ADDR dw_addr = dw_mem_loc_piece->GetAddress();
								unsigned int dw_bit_offset = dw_mem_loc_piece->GetBitOffset();
								unsigned int dw_bit_size = dw_mem_loc_piece->GetBitSize();
								if(debug)
								{
									dw_handler->GetDebugInfoStream() << "DW_LOC_PIECE_MEMORY: dw_addr=0x" << std::hex << dw_addr << std::dec << ", dw_bit_offset=" << dw_bit_offset << ", dw_bit_size=" << dw_bit_size << std::endl;
								}
								
								MEMORY_ADDR dw_byte_size = dw_bit_size ? (dw_bit_size + dw_bit_offset + 7) / 8 : arch_address_size;
								uint8_t buffer[dw_byte_size];
								memset(buffer, 0, dw_byte_size);
								if(!mem_if->ReadMemory(dw_addr, buffer, dw_byte_size)) return false;
								if(!bv.Read(source_bit_offset, buffer, dw_bit_offset, dw_bit_size)) return false;
								source_bit_offset += dw_bit_size;
								if(!mem_if->WriteMemory(dw_addr, buffer, dw_byte_size)) return false;
							}
							break;
					}
					
				}
				return true;
			}
			break;
		case DW_LOC_IMPLICIT_SIMPLE_VALUE:
			{
				// implicit value cannot be modified: silently skip source bits
				uint64_t dw_data_object_bit_size = dw_data_object_loc->GetBitSize();
				source_bit_offset += dw_data_object_bit_size;
			}
			break;
		case DW_LOC_IMPLICIT_BLOCK_VALUE:
			{
				// implicit value cannot be modified: silently skip source bits
				const DWARF_Block<MEMORY_ADDR> *dw_implicit_block_value = dw_data_object_loc->GetImplicitBlockValue();
				
				uint64_t dw_data_object_bit_size = dw_data_object_loc->GetBitSize();
				uint64_t dw_block_bit_size = 8 * dw_implicit_block_value->GetLength();
				if(dw_data_object_bit_size > dw_block_bit_size) dw_data_object_bit_size = dw_block_bit_size;
					
				source_bit_offset += dw_data_object_bit_size;
			}
			break;
		default:
			return false;
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DataObject<MEMORY_ADDR>::Read(MEMORY_ADDR obj_bit_offset, uint64_t& value, MEMORY_ADDR bit_size) const
{
	return bv.Read((dw_data_object_loc->GetType() == DW_LOC_COMPOSITE) ? (obj_bit_offset + dw_data_object_loc->GetBitOffset()) : obj_bit_offset, value, 0, bit_size);
}

template <class MEMORY_ADDR>
bool DWARF_DataObject<MEMORY_ADDR>::Write(MEMORY_ADDR obj_bit_offset, uint64_t value, MEMORY_ADDR bit_size)
{
	return bv.Write((dw_data_object_loc->GetType() == DW_LOC_COMPOSITE) ? (obj_bit_offset + dw_data_object_loc->GetBitOffset()) : obj_bit_offset, value, 0, bit_size);
}

template <class MEMORY_ADDR>
bool DWARF_DataObject<MEMORY_ADDR>::Read(MEMORY_ADDR obj_bit_offset, void *buffer, MEMORY_ADDR buf_bit_offset, MEMORY_ADDR bit_size) const
{
	return bv.Read((dw_data_object_loc->GetType() == DW_LOC_COMPOSITE) ? (obj_bit_offset + dw_data_object_loc->GetBitOffset()) : obj_bit_offset, (uint8_t *) buffer, buf_bit_offset, bit_size);
}

template <class MEMORY_ADDR>
bool DWARF_DataObject<MEMORY_ADDR>::Write(MEMORY_ADDR obj_bit_offset, const void *buffer, MEMORY_ADDR buf_bit_offset, MEMORY_ADDR bit_size)
{
	return bv.Write((dw_data_object_loc->GetType() == DW_LOC_COMPOSITE) ? (obj_bit_offset + dw_data_object_loc->GetBitOffset()) : obj_bit_offset, (uint8_t *) buffer, buf_bit_offset, bit_size);
}

template <class MEMORY_ADDR>
template <typename T>
T DWARF_DataObject<MEMORY_ADDR>::ToBaseType() const
{
	const unisim::util::debug::Type *resolved_type = unisim::util::debug::TypeResolver::Resolve(dw_data_object_type);
	
	if(!resolved_type || (!resolved_type->IsBase() && !resolved_type->IsPointer())) throw typename unisim::util::debug::DataObject<MEMORY_ADDR>::TypeError(data_object_name.c_str(), "Can't convert because data object type is neither a base type nor a pointer type");
	
	T ret_value = T();
	
	if(resolved_type->IsFloat())
	{
		uint64_t data_object_bit_size = GetBitSize();
		uint64_t data_object_byte_size = (data_object_bit_size + 7 ) / 8;
		
		uint8_t data_object_raw_value[data_object_byte_size];
		if(!Read(0, data_object_raw_value, 0, data_object_bit_size)) throw typename unisim::util::debug::DataObject<MEMORY_ADDR>::ReadError(data_object_name.c_str());
		
		switch(data_object_bit_size)
		{
			case 32:
				{
					float float_value = 0.0;
					uint32_t raw_float_value = 0;
					::memcpy(&raw_float_value, data_object_raw_value, 4);
					raw_float_value = unisim::util::endian::Target2Host(arch_endianness, raw_float_value);
					::memcpy(&float_value, &raw_float_value, 4);
					ret_value = (T) float_value;
				}
				break;
			case 64:
				{
					double double_value = 0.0;
					uint64_t raw_double_value = 0;
					::memcpy(&raw_double_value, data_object_raw_value, 8);
					raw_double_value = unisim::util::endian::Target2Host(arch_endianness, raw_double_value);
					::memcpy(&double_value, &raw_double_value, 8);
					ret_value = (T) double_value;
				}
				break;
			default:
				throw typename unisim::util::debug::DataObject<MEMORY_ADDR>::TypeError(data_object_name.c_str(), "Can't convert because data object floating-point format is unsupported (neither 32-bit nor 64-bit long)");
				break;
		}
	}
	else
	{
		uint64_t uint_value;
		if(!Read(0, uint_value, GetBitSize())) throw typename unisim::util::debug::DataObject<MEMORY_ADDR>::ReadError(data_object_name.c_str());
		if(!resolved_type->IsPointer() && std::numeric_limits<T>::is_signed)
		{
			int64_t int_value = unisim::util::arithmetic::SignExtend(uint_value, GetBitSize());
			ret_value = (T) int_value;
		}
		else
		{
			ret_value = (T) uint_value;
		}
	}
	
	return ret_value;
}

template <class MEMORY_ADDR>
std::string DWARF_DataObject<MEMORY_ADDR>::ToString() const
{
	std::string ret_value;
	
	if(!unisim::util::debug::TypeIsCharPointer::Test(dw_data_object_type)) throw typename unisim::util::debug::DataObject<MEMORY_ADDR>::TypeError(data_object_name.c_str(), "Can't convert to string because data object type is not char pointer");
	
	MEMORY_ADDR pointer_bit_size = GetBitSize();
	
	uint64_t pointer_value = 0;
	if(pointer_bit_size > (8 * sizeof(pointer_value))) pointer_bit_size = 8 * sizeof(pointer_value);
	if(!Read(0, pointer_value, pointer_bit_size))
	{
		throw typename unisim::util::debug::DataObject<MEMORY_ADDR>::ReadError(data_object_name.c_str());
	}
	
	char char_value = 0;
	do
	{
		if(!mem_if->ReadMemory(pointer_value, &char_value, 1))
		{
			throw typename unisim::util::debug::DataObject<MEMORY_ADDR>::ReadError(data_object_name.c_str());
		}
		if(char_value)
		{
			ret_value += char_value;
		}
	}
	while(++pointer_value, char_value);
	
	return ret_value;
}

template <class MEMORY_ADDR>
template <typename T>
DWARF_DataObject<MEMORY_ADDR>& DWARF_DataObject<MEMORY_ADDR>::AssignBaseType(T value)
{
	const unisim::util::debug::Type *resolved_type = unisim::util::debug::TypeResolver::Resolve(dw_data_object_type);
	
	if(!resolved_type || (!resolved_type->IsBase() && !resolved_type->IsPointer())) throw typename unisim::util::debug::DataObject<MEMORY_ADDR>::TypeError(data_object_name.c_str(), "Can't assign because data object type is neither a base type nor a pointer type");

	if(resolved_type->IsFloat())
	{
		double value = 0.0;
		uint64_t data_object_bit_size = GetBitSize();
		uint64_t data_object_byte_size = (data_object_bit_size + 7 ) / 8;
		uint8_t data_object_raw_value[data_object_byte_size];
		if(Read(0, data_object_raw_value, 0, data_object_bit_size))
		{
			if((data_object_bit_size == 32) || (data_object_bit_size == 64))
			{
				if(data_object_bit_size == 32)
				{
					float float_value = (float) value;
					uint32_t raw_float_value;
					::memcpy(&raw_float_value, &float_value, 4);
					raw_float_value = unisim::util::endian::Host2Target(arch_endianness, raw_float_value);
					::memcpy(data_object_raw_value, &raw_float_value, 4);
				}
				else if(data_object_bit_size == 64)
				{
					double double_value = value;
					uint64_t raw_double_value;
					::memcpy(&raw_double_value, &double_value, 8);
					raw_double_value = unisim::util::endian::Host2Target(arch_endianness, raw_double_value);
					::memcpy(data_object_raw_value, &raw_double_value, 8);
				}
				
				if(!Write(0, data_object_raw_value, 0, data_object_bit_size))
				{
					throw typename unisim::util::debug::DataObject<MEMORY_ADDR>::WriteError(data_object_name.c_str());
				}
			}
			else
			{
				throw typename unisim::util::debug::DataObject<MEMORY_ADDR>::TypeError(data_object_name.c_str(), "Can't assign because data object floating-point format is unsupported (neither 32-bit nor 64-bit long)");
			}
		}
		else
		{
			throw typename unisim::util::debug::DataObject<MEMORY_ADDR>::ReadError(data_object_name.c_str());
		}
	}
	else
	{
		uint64_t data_object_bit_size = GetBitSize();
		uint64_t data_object_value = value;
		if(!Write(0, data_object_value, data_object_bit_size))
		{
			throw typename unisim::util::debug::DataObject<MEMORY_ADDR>::WriteError(data_object_name.c_str());
		}
	}
	
	return *this;
}

template <class MEMORY_ADDR>
DWARF_DataObject<MEMORY_ADDR>& DWARF_DataObject<MEMORY_ADDR>::AssignString(const std::string& value)
{
	std::string ret_value;
	
	if(!unisim::util::debug::TypeIsCharPointer::Test(dw_data_object_type)) throw typename unisim::util::debug::DataObject<MEMORY_ADDR>::TypeError(data_object_name.c_str());
	
	MEMORY_ADDR pointer_bit_size = GetBitSize();
	
	uint64_t pointer_value = 0;
	if(pointer_bit_size > (8 * sizeof(pointer_value))) pointer_bit_size = 8 * sizeof(pointer_value);
	if(!Read(0, pointer_value, pointer_bit_size))
	{
		throw typename unisim::util::debug::DataObject<MEMORY_ADDR>::ReadError(data_object_name.c_str());
	}
	
	std::size_t n = value.length();
	for(std::size_t i = 0; i <= n; ++i, ++pointer_value)
	{
		char char_value = (i < n) ? value[i] : 0;
		if(!mem_if->WriteMemory(pointer_value, &char_value, 1))
		{
			throw typename unisim::util::debug::DataObject<MEMORY_ADDR>::WriteError(data_object_name.c_str());
		}
	}
	
	return *this;
}

template <class MEMORY_ADDR>
DWARF_DataObject<MEMORY_ADDR> *DWARF_DataObject<MEMORY_ADDR>::Dereference()
{
	if(!unisim::util::debug::TypeIsPointer::Test(dw_data_object_type)) throw typename unisim::util::debug::DataObject<MEMORY_ADDR>::TypeError(data_object_name.c_str(), "Can't dereference because data object is not a pointer");
	
	CLocOperationStream *deref_c_loc_operation_stream = new CLocOperationStream(*c_loc_operation_stream);
	deref_c_loc_operation_stream->Push(new CLocOperation(OP_DEREF));
	
	typename DWARF_Handler<MEMORY_ADDR>::FindDataObjectArguments args;
	args.c_loc_operation_stream.Push(new CLocOperation(OP_DEREF));
	args.dw_die_type = dw_die_type;
	args.matched_data_object_name = data_object_name;
	args.dw_data_object_loc = new DWARF_Location<MEMORY_ADDR>(*dw_data_object_loc);
	
	if(dw_handler->FindDataObjectEpilog(dw_mach_state, prc_num, args))
	{
		return new DWARF_DataObject(dw_handler, dw_mach_state, prc_num, args.matched_data_object_name.c_str(), deref_c_loc_operation_stream, args.dw_data_object_loc, args.dw_die_type, args.dim, args.dw_data_object_type);
	}
	
	return 0;
}

template <class MEMORY_ADDR>
DWARF_DataObject<MEMORY_ADDR> *DWARF_DataObject<MEMORY_ADDR>::GetProperty(const char *property_name)
{
	if(!unisim::util::debug::TypeIsComposite::Test(dw_data_object_type)) throw typename unisim::util::debug::DataObject<MEMORY_ADDR>::TypeError(data_object_name.c_str(), "Data object is not an object");
	
	CLocOperationStream *property_c_loc_operation_stream = new CLocOperationStream(*c_loc_operation_stream);
	property_c_loc_operation_stream->Push(new CLocOperation(OP_STRUCT_REF));
	property_c_loc_operation_stream->Push(new CLocOpLiteralIdentifier(property_name));
	
	typename DWARF_Handler<MEMORY_ADDR>::FindDataObjectArguments args;
	args.c_loc_operation_stream.Push(new CLocOperation(OP_STRUCT_REF));
	args.c_loc_operation_stream.Push(new CLocOpLiteralIdentifier(property_name));
	args.dw_die_type = dw_die_type;
	args.matched_data_object_name = data_object_name;
	args.dw_data_object_loc = new DWARF_Location<MEMORY_ADDR>(*dw_data_object_loc);
	
	if(dw_handler->FindDataObjectEpilog(dw_mach_state, prc_num, args))
	{
		return new DWARF_DataObject(dw_handler, dw_mach_state, prc_num, args.matched_data_object_name.c_str(), property_c_loc_operation_stream, args.dw_data_object_loc, args.dw_die_type, args.dim, args.dw_data_object_type);
	}
	
	return 0;
}

template <class MEMORY_ADDR>
DWARF_DataObject<MEMORY_ADDR> *DWARF_DataObject<MEMORY_ADDR>::GetItem(int64_t subscript)
{
	const unisim::util::debug::Type *resolved_type = unisim::util::debug::TypeResolver::Resolve(dw_data_object_type);
	if(!resolved_type->IsArray() && (!resolved_type->IsPointer() || resolved_type->AsPointer().GetTypeOfDereferencedObject()->IsUnspecified()))
	{
		throw typename unisim::util::debug::DataObject<MEMORY_ADDR>::TypeError(data_object_name.c_str(), "Data object is neither an array nor a non-void pointer");
	}
	
	CLocOperationStream *item_c_loc_operation_stream = new CLocOperationStream(*c_loc_operation_stream);
	item_c_loc_operation_stream->Push(new CLocOperation(OP_ARRAY_SUBSCRIPT));
	item_c_loc_operation_stream->Push(new CLocOpLiteralInteger(subscript));
	
	typename DWARF_Handler<MEMORY_ADDR>::FindDataObjectArguments args;
	args.c_loc_operation_stream.Push(new CLocOperation(OP_ARRAY_SUBSCRIPT));
	args.c_loc_operation_stream.Push(new CLocOpLiteralInteger(subscript));
	args.dw_die_type = dw_die_type;
	args.matched_data_object_name = data_object_name;
	args.dw_data_object_loc = new DWARF_Location<MEMORY_ADDR>(*dw_data_object_loc);
	
	if(dw_handler->FindDataObjectEpilog(dw_mach_state, prc_num, args))
	{
		return new DWARF_DataObject(dw_handler, dw_mach_state, prc_num, args.matched_data_object_name.c_str(), item_c_loc_operation_stream, args.dw_data_object_loc, args.dw_die_type, args.dim, args.dw_data_object_type);
	}
	
	return 0;
}

template <class ADDRESS>
DWARF_DataObjectProxy<ADDRESS>::DWARF_DataObjectProxy(DWARF_DataObject<ADDRESS> *_dw_data_object)
	: dw_data_object(_dw_data_object)
{
	assert(dw_data_object);
}

template <class ADDRESS>
DWARF_DataObjectProxy<ADDRESS>::~DWARF_DataObjectProxy()
{
	delete dw_data_object;
}

template <class ADDRESS>
const char *DWARF_DataObjectProxy<ADDRESS>::GetName() const
{
	return dw_data_object->GetName();
}

template <class ADDRESS>
unsigned int DWARF_DataObjectProxy<ADDRESS>::GetProcessorNumber() const
{
	return dw_data_object->GetProcessorNumber();
}

template <class ADDRESS>
ADDRESS DWARF_DataObjectProxy<ADDRESS>::GetBitSize() const
{
	return dw_data_object->Seek() ? dw_data_object->GetBitSize() : 0;
}

template <class ADDRESS>
const Type *DWARF_DataObjectProxy<ADDRESS>::GetType() const
{
	return dw_data_object->Seek() ? dw_data_object->GetType() : 0;
}

template <class ADDRESS>
unisim::util::endian::endian_type DWARF_DataObjectProxy<ADDRESS>::GetEndian() const
{
	return dw_data_object->GetEndian();
}

template <class ADDRESS>
bool DWARF_DataObjectProxy<ADDRESS>::Exists() const
{
	return dw_data_object->Seek();
}

template <class ADDRESS>
bool DWARF_DataObjectProxy<ADDRESS>::IsOptimizedOut() const
{
	return !dw_data_object->Seek() || dw_data_object->IsOptimizedOut();
}

template <class ADDRESS>
bool DWARF_DataObjectProxy<ADDRESS>::GetAddress(ADDRESS& addr) const
{
	return !dw_data_object->Seek() || dw_data_object->GetAddress(addr);
}

template <class ADDRESS>
bool DWARF_DataObjectProxy<ADDRESS>::Read(ADDRESS obj_bit_offset, uint64_t& value, ADDRESS bit_size) const
{
	return dw_data_object->Fetch() && dw_data_object->Read(obj_bit_offset, value, bit_size);
}

template <class ADDRESS>
bool DWARF_DataObjectProxy<ADDRESS>::Write(ADDRESS obj_bit_offset, uint64_t value, ADDRESS bit_size)
{
	return dw_data_object->Fetch() && dw_data_object->Write(obj_bit_offset, value, bit_size) && dw_data_object->Commit();
}

template <class ADDRESS>
bool DWARF_DataObjectProxy<ADDRESS>::Read(ADDRESS obj_bit_offset, void *buffer, ADDRESS buf_bit_offset, ADDRESS bit_size) const
{
	return dw_data_object->Fetch() && dw_data_object->Read(obj_bit_offset, buffer, buf_bit_offset, bit_size);
}

template <class ADDRESS>
bool DWARF_DataObjectProxy<ADDRESS>::Write(ADDRESS obj_bit_offset, const void *buffer, ADDRESS buf_bit_offset, ADDRESS bit_size)
{
	return dw_data_object->Fetch() && dw_data_object->Write(obj_bit_offset, buffer, buf_bit_offset, bit_size) && dw_data_object->Commit();
}

template <class ADDRESS>
template <typename T>
T DWARF_DataObjectProxy<ADDRESS>::ToBaseType() const
{
	if(!Exists()) throw typename unisim::util::debug::DataObject<ADDRESS>::DoesNotExistError(dw_data_object->GetName());
	if(dw_data_object->IsOptimizedOut()) throw typename unisim::util::debug::DataObject<ADDRESS>::OptimizedOutError(dw_data_object->GetName());
	if(!dw_data_object->Fetch()) throw typename unisim::util::debug::DataObject<ADDRESS>::ReadError(dw_data_object->GetName());
	return dw_data_object->template ToBaseType<T>();
}

template <class ADDRESS>
std::string DWARF_DataObjectProxy<ADDRESS>::ToString() const
{
	if(!Exists()) throw typename unisim::util::debug::DataObject<ADDRESS>::DoesNotExistError(dw_data_object->GetName());
	if(dw_data_object->IsOptimizedOut()) throw typename unisim::util::debug::DataObject<ADDRESS>::OptimizedOutError(dw_data_object->GetName());
	if(!dw_data_object->Fetch()) throw typename unisim::util::debug::DataObject<ADDRESS>::ReadError(dw_data_object->GetName());
	return dw_data_object->ToString();
}

template <class ADDRESS>
template <typename T>
DWARF_DataObjectProxy<ADDRESS>& DWARF_DataObjectProxy<ADDRESS>::AssignBaseType(T value)
{
	if(!Exists()) throw typename unisim::util::debug::DataObject<ADDRESS>::DoesNotExistError(dw_data_object->GetName());
	if(dw_data_object->IsOptimizedOut()) throw typename unisim::util::debug::DataObject<ADDRESS>::OptimizedOutError(dw_data_object->GetName());
	if(!dw_data_object->Fetch()) throw typename unisim::util::debug::DataObject<ADDRESS>::ReadError(dw_data_object->GetName());
	dw_data_object->template AssignBaseType<T>(value);
	if(!dw_data_object->Commit()) throw typename unisim::util::debug::DataObject<ADDRESS>::WriteError(dw_data_object->GetName());
	return *this;
}

template <class ADDRESS>
DWARF_DataObjectProxy<ADDRESS>& DWARF_DataObjectProxy<ADDRESS>::AssignString(const std::string& value)
{
	if(!Exists()) throw typename unisim::util::debug::DataObject<ADDRESS>::DoesNotExistError(dw_data_object->GetName());
	if(dw_data_object->IsOptimizedOut()) throw typename unisim::util::debug::DataObject<ADDRESS>::OptimizedOutError(dw_data_object->GetName());
	if(!dw_data_object->Fetch()) throw typename unisim::util::debug::DataObject<ADDRESS>::ReadError(dw_data_object->GetName());
	dw_data_object->AssignString(value);
	return *this;
}

template <class ADDRESS>
DWARF_DataObjectProxy<ADDRESS>::operator bool() const
{
	return this->template ToBaseType<bool>();
}

template <class ADDRESS>
DWARF_DataObjectProxy<ADDRESS>::operator char() const
{
	return this->template ToBaseType<char>();
}

template <class ADDRESS>
DWARF_DataObjectProxy<ADDRESS>::operator signed char() const
{
	return this->template ToBaseType<char>();
}

template <class ADDRESS>
DWARF_DataObjectProxy<ADDRESS>::operator short() const
{
	return this->template ToBaseType<short>();
}

template <class ADDRESS>
DWARF_DataObjectProxy<ADDRESS>::operator int() const
{
	return this->template ToBaseType<int>();
}

template <class ADDRESS>
DWARF_DataObjectProxy<ADDRESS>::operator long() const
{
	return this->template ToBaseType<long>();
}

template <class ADDRESS>
DWARF_DataObjectProxy<ADDRESS>::operator long long() const
{
	return this->template ToBaseType<long long>();
}

template <class ADDRESS>
DWARF_DataObjectProxy<ADDRESS>::operator unsigned char() const
{
	return this->template ToBaseType<unsigned char>();
}

template <class ADDRESS>
DWARF_DataObjectProxy<ADDRESS>::operator unsigned short() const
{
	return this->template ToBaseType<unsigned short>();
}

template <class ADDRESS>
DWARF_DataObjectProxy<ADDRESS>::operator unsigned int() const
{
	return this->template ToBaseType<unsigned int>();
}

template <class ADDRESS>
DWARF_DataObjectProxy<ADDRESS>::operator unsigned long() const
{
	return this->template ToBaseType<unsigned long>();
}

template <class ADDRESS>
DWARF_DataObjectProxy<ADDRESS>::operator unsigned long long() const
{
	return this->template ToBaseType<unsigned long long>();
}

template <class ADDRESS>
DWARF_DataObjectProxy<ADDRESS>::operator float() const
{
	return this->template ToBaseType<float>();
}

template <class ADDRESS>
DWARF_DataObjectProxy<ADDRESS>::operator double() const
{
	return this->template ToBaseType<double>();
}

template <class ADDRESS>
DWARF_DataObjectProxy<ADDRESS>::operator std::string() const
{
	return ToString();
}

template <class ADDRESS>
unisim::util::debug::DataObject<ADDRESS>& DWARF_DataObjectProxy<ADDRESS>::operator = (bool value)
{
	return this->template AssignBaseType<bool>(value);
}

template <class ADDRESS>
unisim::util::debug::DataObject<ADDRESS>& DWARF_DataObjectProxy<ADDRESS>::operator = (signed char value)
{
	return this->template AssignBaseType<signed char>(value);
}

template <class ADDRESS>
unisim::util::debug::DataObject<ADDRESS>& DWARF_DataObjectProxy<ADDRESS>::operator = (short value)
{
	return this->template AssignBaseType<short>(value);
}

template <class ADDRESS>
unisim::util::debug::DataObject<ADDRESS>& DWARF_DataObjectProxy<ADDRESS>::operator = (int value)
{
	return this->template AssignBaseType<int>(value);
}

template <class ADDRESS>
unisim::util::debug::DataObject<ADDRESS>& DWARF_DataObjectProxy<ADDRESS>::operator = (long value)
{
	return this->template AssignBaseType<long>(value);
}

template <class ADDRESS>
unisim::util::debug::DataObject<ADDRESS>& DWARF_DataObjectProxy<ADDRESS>::operator = (long long value)
{
	return this->template AssignBaseType<long long>(value);
}

template <class ADDRESS>
unisim::util::debug::DataObject<ADDRESS>& DWARF_DataObjectProxy<ADDRESS>::operator = (unsigned char value)
{
	return this->template AssignBaseType<unsigned char>(value);
}

template <class ADDRESS>
unisim::util::debug::DataObject<ADDRESS>& DWARF_DataObjectProxy<ADDRESS>::operator = (unsigned short value)
{
	return this->template AssignBaseType<unsigned short>(value);
}

template <class ADDRESS>
unisim::util::debug::DataObject<ADDRESS>& DWARF_DataObjectProxy<ADDRESS>::operator = (unsigned int value)
{
	return this->template AssignBaseType<unsigned int>(value);
}

template <class ADDRESS>
unisim::util::debug::DataObject<ADDRESS>& DWARF_DataObjectProxy<ADDRESS>::operator = (unsigned long value)
{
	return this->template AssignBaseType<unsigned long>(value);
}

template <class ADDRESS>
unisim::util::debug::DataObject<ADDRESS>& DWARF_DataObjectProxy<ADDRESS>::operator = (unsigned long long value)
{
	return this->template AssignBaseType<unsigned long long>(value);
}

template <class ADDRESS>
unisim::util::debug::DataObject<ADDRESS>& DWARF_DataObjectProxy<ADDRESS>::operator = (float value)
{
	return this->template AssignBaseType<float>(value);
}

template <class ADDRESS>
unisim::util::debug::DataObject<ADDRESS>& DWARF_DataObjectProxy<ADDRESS>::operator = (double value)
{
	return this->template AssignBaseType<double>(value);
}

template <class ADDRESS>
unisim::util::debug::DataObject<ADDRESS>& DWARF_DataObjectProxy<ADDRESS>::operator = (const std::string& value)
{
	return AssignString(value);
}

template <class ADDRESS>
unisim::util::debug::DataObjectRef<ADDRESS> DWARF_DataObjectProxy<ADDRESS>::operator * ()
{
	DWARF_DataObject<ADDRESS> *derefenced_object = dw_data_object->Dereference();
	return unisim::util::debug::DataObjectRef<ADDRESS>(derefenced_object ? new DWARF_DataObjectProxy<ADDRESS>(derefenced_object) : 0);
}

template <class ADDRESS>
const unisim::util::debug::DataObjectRef<ADDRESS> DWARF_DataObjectProxy<ADDRESS>::operator * () const
{
	DWARF_DataObject<ADDRESS> *derefenced_object = dw_data_object->Dereference();
	return unisim::util::debug::DataObjectRef<ADDRESS>(derefenced_object ? new DWARF_DataObjectProxy<ADDRESS>(derefenced_object) : 0);
}

template <class ADDRESS>
unisim::util::debug::DataObjectRef<ADDRESS> DWARF_DataObjectProxy<ADDRESS>::operator [] (const char *property_name)
{
	DWARF_DataObject<ADDRESS> *property_object = dw_data_object->GetProperty(property_name);
	return unisim::util::debug::DataObjectRef<ADDRESS>(property_object ? new DWARF_DataObjectProxy<ADDRESS>(property_object) : 0);
}

template <class ADDRESS>
const unisim::util::debug::DataObjectRef<ADDRESS> DWARF_DataObjectProxy<ADDRESS>::operator [] (const char *property_name) const
{
	DWARF_DataObject<ADDRESS> *property_object = dw_data_object->GetProperty(property_name);
	return unisim::util::debug::DataObjectRef<ADDRESS>(property_object ? new DWARF_DataObjectProxy<ADDRESS>(property_object) : 0);
}

template <class ADDRESS>
unisim::util::debug::DataObjectRef<ADDRESS> DWARF_DataObjectProxy<ADDRESS>::operator [] (int64_t subscript)
{
	DWARF_DataObject<ADDRESS> *array_element_object = dw_data_object->GetItem(subscript);
	return unisim::util::debug::DataObjectRef<ADDRESS>(array_element_object ? new DWARF_DataObjectProxy<ADDRESS>(array_element_object) : 0);
}

template <class ADDRESS>
const unisim::util::debug::DataObjectRef<ADDRESS> DWARF_DataObjectProxy<ADDRESS>::operator [] (int64_t subscript) const
{
	DWARF_DataObject<ADDRESS> *array_element_object = dw_data_object->GetItem(subscript);
	return unisim::util::debug::DataObjectRef<ADDRESS>(array_element_object ? new DWARF_DataObjectProxy<ADDRESS>(array_element_object) : 0);
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_DEBUG_DWARF_DATA_OBJECT_TCC__
