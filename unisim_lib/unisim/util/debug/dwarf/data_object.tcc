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

namespace unisim {
namespace util {
namespace debug {
namespace dwarf {

template <class MEMORY_ADDR>
DWARF_DataObject<MEMORY_ADDR>::DWARF_DataObject(const DWARF_Handler<MEMORY_ADDR> *dw_handler, DWARF_Location<MEMORY_ADDR> *_dw_data_object_loc, MEMORY_ADDR _dw_data_object_bit_size)
	: dw_data_object_loc(_dw_data_object_loc)
	, dw_data_object_bit_size(_dw_data_object_bit_size)
	, arch_endianness(dw_handler->GetArchEndianness())
	, arch_address_size(dw_handler->GetArchAddressSize())
	, dw_reg_num_mapping(dw_handler->GetRegisterNumberMapping())
	, mem_if(dw_handler->GetMemoryInterface())
	, bv(arch_endianness)
{
}

template <class MEMORY_ADDR>
DWARF_DataObject<MEMORY_ADDR>::~DWARF_DataObject()
{
	if(dw_data_object_loc) delete dw_data_object_loc;
}

template <class MEMORY_ADDR>
MEMORY_ADDR DWARF_DataObject<MEMORY_ADDR>::GetBitSize() const
{
	return dw_data_object_bit_size;
}

template <class MEMORY_ADDR>
bool DWARF_DataObject<MEMORY_ADDR>::Fetch()
{
	bv.Clear();
	
	switch(dw_data_object_loc->GetType())
	{
		case DW_LOC_SIMPLE_MEMORY:
			{
				MEMORY_ADDR dw_addr = dw_data_object_loc->GetAddress();
				MEMORY_ADDR dw_byte_size = (dw_data_object_bit_size + 7) / 8;
				std::cerr << "DW_LOC_SIMPLE_MEMORY: dw_addr=0x" << std::hex << dw_addr << std::dec << ", dw_byte_size=" << dw_byte_size << std::endl;
				uint8_t buffer[dw_byte_size];
				memset(buffer, 0, dw_byte_size);
				if(!mem_if->ReadMemory(dw_addr, buffer, dw_byte_size)) return false;
				
				bv.Append(buffer, 0, dw_data_object_bit_size);
			}
			return true;
		case DW_LOC_SIMPLE_REGISTER:
			{
				unsigned int dw_reg_num = dw_data_object_loc->GetRegisterNumber();
				const unisim::util::debug::Register *arch_reg = dw_reg_num_mapping->GetArchReg(dw_reg_num);
				std::cerr << "DW_LOC_SIMPLE_REGISTER: dw_reg_num=" << dw_reg_num << std::endl;
				if(!arch_reg) return false;
				
				unsigned int reg_size = arch_reg->GetSize(); // FIXME: Get true bit size
				
				switch(reg_size)
				{
					case 1:
						{
							uint8_t value = 0;
							arch_reg->GetValue(&value);
							bv.Append(value, dw_data_object_bit_size);
						}
						break;
					case 2:
						{
							uint16_t value = 0;
							arch_reg->GetValue(&value);
							bv.Append(value, dw_data_object_bit_size);
						}
						break;
					case 4:
						{
							uint32_t value = 0;
							arch_reg->GetValue(&value);
							bv.Append(value, dw_data_object_bit_size);
						}
						break;
					case 8:
						{
							uint64_t value = 0;
							arch_reg->GetValue(&value);
							bv.Append(value, dw_data_object_bit_size);
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
								std::cerr << "DW_LOC_PIECE_REGISTER: dw_reg_num=" << dw_reg_num << ", dw_bit_offset=" << dw_bit_offset << ", dw_bit_size=" << dw_bit_size << std::endl;
								
								const unisim::util::debug::Register *arch_reg = dw_reg_num_mapping->GetArchReg(dw_reg_num);
								if(!arch_reg) return false;
								
								unsigned int reg_size = arch_reg->GetSize(); // FIXME: Get true bit size
								
								switch(reg_size)
								{
									case 1:
										{
											uint8_t value = 0;
											arch_reg->GetValue(&value);
											value = value >> dw_bit_offset;
											bv.Append(value, dw_bit_size ? dw_bit_size : 8);
										}
										break;
									case 2:
										{
											uint16_t value = 0;
											arch_reg->GetValue(&value);
											value = value >> dw_bit_offset;
											bv.Append(value, dw_bit_size ? dw_bit_size : 16);
										}
										break;
									case 4:
										{
											uint32_t value = 0;
											arch_reg->GetValue(&value);
											value = value >> dw_bit_offset;
											bv.Append(value, dw_bit_size ? dw_bit_size : 32);
										}
										break;
									case 8:
										{
											uint64_t value = 0;
											arch_reg->GetValue(&value);
											value = value >> dw_bit_offset;
											bv.Append(value, dw_bit_size ? dw_bit_size : 64);
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
								std::cerr << "DW_LOC_PIECE_MEMORY: dw_addr=0x" << std::hex << dw_addr << std::dec << ", dw_bit_offset=" << dw_bit_offset << ", dw_bit_size=" << dw_bit_size << std::endl;
								
								MEMORY_ADDR dw_byte_size = dw_bit_size ? (dw_bit_size + dw_bit_offset + 7) / 8 : arch_address_size;
								uint8_t buffer[dw_byte_size];
								memset(buffer, 0, dw_byte_size);
								if(!mem_if->ReadMemory(dw_addr, buffer, dw_byte_size)) return false;
								
								bv.Append(buffer, dw_bit_size ? dw_bit_offset : 0, dw_bit_size ? dw_bit_size : dw_byte_size * 8);
							}
							break;
					}
					
				}
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
	unsigned int source_bit_offset = 0;
	
	switch(dw_data_object_loc->GetType())
	{
		case DW_LOC_SIMPLE_MEMORY:
			{
				MEMORY_ADDR dw_addr = dw_data_object_loc->GetAddress();
				MEMORY_ADDR dw_byte_size = (dw_data_object_bit_size + 7) / 8;
				std::cerr << "DW_LOC_SIMPLE_MEMORY: dw_addr=0x" << std::hex << dw_addr << std::dec << ", dw_byte_size=" << dw_byte_size << std::endl;
				uint8_t buffer[dw_byte_size];
				memset(buffer, 0, dw_byte_size);
				if(!bv.Read(source_bit_offset, buffer, 0, dw_data_object_bit_size)) return false;
				source_bit_offset += dw_data_object_bit_size;
				if(!mem_if->WriteMemory(dw_addr, buffer, dw_byte_size)) return false;
			}
			return true;
		case DW_LOC_SIMPLE_REGISTER:
			{
				unsigned int dw_reg_num = dw_data_object_loc->GetRegisterNumber();
				unisim::util::debug::Register *arch_reg = dw_reg_num_mapping->GetArchReg(dw_reg_num);
				std::cerr << "DW_LOC_SIMPLE_REGISTER: dw_reg_num=" << dw_reg_num << std::endl;
				if(!arch_reg) return false;
				
				unsigned int reg_size = arch_reg->GetSize(); // FIXME: Get true bit size
				
				switch(reg_size)
				{
					case 1:
						{
							uint64_t value = 0;
							if(!bv.Read(source_bit_offset, value, dw_data_object_bit_size)) return false;
							source_bit_offset += dw_data_object_bit_size;
							uint8_t value8 = value;
							arch_reg->SetValue(&value8);
						}
						break;
					case 2:
						{
							uint64_t value = 0;
							if(!bv.Read(source_bit_offset, value, dw_data_object_bit_size)) return false;
							source_bit_offset += dw_data_object_bit_size;
							uint16_t value16 = value;
							arch_reg->SetValue(&value16);
						}
						break;
					case 4:
						{
							uint64_t value = 0;
							if(!bv.Read(source_bit_offset, value, dw_data_object_bit_size)) return false;
							source_bit_offset += dw_data_object_bit_size;
							uint32_t value32 = value;
							arch_reg->SetValue(&value32);
						}
						break;
					case 8:
						{
							uint64_t value = 0;
							if(!bv.Read(source_bit_offset, value, dw_data_object_bit_size)) return false;
							source_bit_offset += dw_data_object_bit_size;
							arch_reg->SetValue(&value);
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
								std::cerr << "DW_LOC_PIECE_REGISTER: dw_reg_num=" << dw_reg_num << ", dw_bit_offset=" << dw_bit_offset << ", dw_bit_size=" << dw_bit_size << std::endl;
								
								unisim::util::debug::Register *arch_reg = dw_reg_num_mapping->GetArchReg(dw_reg_num);
								if(!arch_reg) return false;
								
								unsigned int reg_size = arch_reg->GetSize(); // FIXME: Get true bit size
								
								switch(reg_size)
								{
									case 1:
										{
											uint8_t value8 = 0;
											arch_reg->GetValue(&value8);
											uint64_t value = 0;
											if(!bv.Read(source_bit_offset, value, dw_bit_size)) return false;
											source_bit_offset += dw_bit_size;
											uint8_t mask = ((1 << dw_bit_size) - 1) << dw_bit_offset;
											value8 = (value & ~mask) | (value << dw_bit_offset);
											arch_reg->SetValue(&value8);
										}
										break;
									case 2:
										{
											uint16_t value16 = 0;
											arch_reg->GetValue(&value16);
											uint64_t value = 0;
											if(!bv.Read(source_bit_offset, value, dw_bit_size)) return false;
											source_bit_offset += dw_bit_size;
											uint16_t mask = ((1 << dw_bit_size) - 1) << dw_bit_offset;
											value16 = (value & ~mask) | (value << dw_bit_offset);
											arch_reg->SetValue(&value16);
										}
										break;
									case 4:
										{
											uint8_t value32 = 0;
											arch_reg->GetValue(&value32);
											uint64_t value = 0;
											if(!bv.Read(source_bit_offset, value, dw_bit_size)) return false;
											source_bit_offset += dw_bit_size;
											uint16_t mask = ((1 << dw_bit_size) - 1) << dw_bit_offset;
											value32 = (value & ~mask) | (value << dw_bit_offset);
											arch_reg->SetValue(&value32);
										}
										break;
									case 8:
										{
											uint8_t value64 = 0;
											arch_reg->GetValue(&value64);
											uint64_t value = 0;
											if(!bv.Read(source_bit_offset, value, dw_bit_size)) return false;
											source_bit_offset += dw_bit_size;
											uint16_t mask = ((1 << dw_bit_size) - 1) << dw_bit_offset;
											value64 = (value & ~mask) | (value << dw_bit_offset);
											arch_reg->SetValue(&value64);
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
								std::cerr << "DW_LOC_PIECE_MEMORY: dw_addr=0x" << std::hex << dw_addr << std::dec << ", dw_bit_offset=" << dw_bit_offset << ", dw_bit_size=" << dw_bit_size << std::endl;
								
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
		default:
			return false;
	}
	return false;
}

template <class MEMORY_ADDR>
bool DWARF_DataObject<MEMORY_ADDR>::Read(MEMORY_ADDR _bit_offset, void *_buffer, MEMORY_ADDR _bit_size) const
{
	return bv.Read(_bit_offset, (uint8_t *) _buffer, 0, _bit_size);
}

template <class MEMORY_ADDR>
bool DWARF_DataObject<MEMORY_ADDR>::Write(MEMORY_ADDR _bit_offset, const void *_buffer, MEMORY_ADDR _bit_size)
{
	return bv.Write(_bit_offset, (uint8_t *) _buffer, 0, _bit_size);
}

} // end of namespace dwarf
} // end of namespace debug
} // end of namespace util
} // end of namespace unisim

#endif // __UNISIM_UTIL_DEBUG_DWARF_DATA_OBJECT_TCC__
