/*
 *  Copyright (c) 2007,
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

#include <string.h>
#include <iostream>
#include "unisim/util/endian/endian.hh"

#ifndef __UNISIM_COMPONENT_CXX_MEMORY_FLASH_AM29LV_AM29LV_TCC__
#define __UNISIM_COMPONENT_CXX_MEMORY_FLASH_AM29LV_AM29LV_TCC__

namespace unisim {
namespace component {
namespace cxx {
namespace memory {
namespace flash {
namespace am29lv {

using namespace std;
using unisim::util::endian::E_LITTLE_ENDIAN;
using unisim::util::endian::E_BIG_ENDIAN;
using unisim::kernel::service::Object;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::EndDebugError;

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
AM29LV<CONFIG, BYTESIZE, IO_WIDTH>::AM29LV(const char *name, Object *parent) :
	Object(name, parent),
	Service<Memory<typename CONFIG::ADDRESS> >(name, parent),
	memory_export("memory-export", this),
	logger(*this),
	verbose(false),
	param_verbose("verbose", this, verbose, "enable/disable verbosity"),
	org(0),
	bytesize(BYTESIZE),
	endian(E_LITTLE_ENDIAN),
	param_org("org", this, org, "flash memory base address"),
	param_bytesize("bytesize", this, bytesize, "flash memory size in bytes"),
	param_endian("endian", this, endian, "endianness of flash memory"),
	param_sector_protect("sector-protect", this, sector_protect, CONFIG::NUM_SECTORS, "enable/disable sector write protection")
{
	uint32_t chip_io_width;
	for(config_addr_shift = 0, chip_io_width = CHIP_IO_WIDTH; chip_io_width > 1; chip_io_width >>= 1, config_addr_shift++);

	uint32_t io_width;
	for(addr_shift = 0, io_width = IO_WIDTH; io_width > 1; io_width >>= 1, addr_shift++);

	unsigned int chip_num;
	for(chip_num = 0; chip_num < NUM_CHIPS; chip_num++)
	{
		state[chip_num] = CONFIG::STATE_INITIAL;
		cycle[chip_num] = 0;
	}

	unsigned int sector_num;
	for(sector_num = 0; sector_num < CONFIG::NUM_SECTORS; sector_num++)
	{
		sector_protect[sector_num] = false;
	}

	storage = new uint8_t[BYTESIZE];
	memset(storage, 0, BYTESIZE);
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
AM29LV<CONFIG, BYTESIZE, IO_WIDTH>::~AM29LV()
{
	delete[] storage;
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
bool AM29LV<CONFIG, BYTESIZE, IO_WIDTH>::Setup()
{
	unsigned int chip_num;

	for(chip_num = 0; chip_num < NUM_CHIPS; chip_num++)
	{
		state[chip_num] = CONFIG::STATE_INITIAL;
		cycle[chip_num] = 0;
	}

	memset(storage, 0, BYTESIZE);

	if((IO_WIDTH / NUM_CHIPS) > CONFIG::MAX_IO_WIDTH)
	{
		if(unlikely(verbose))
		{
			logger << DebugError;
			logger << "bad I/O Width (currently: " << IO_WIDTH << ", suggested: " << (NUM_CHIPS * CONFIG::MAX_IO_WIDTH) << ")" << std::endl;
			logger << EndDebugError;
		}
		return false;
	}
	
	if(!(CONFIG::MODE_SUPPORT & (IO_WIDTH / NUM_CHIPS)))
	{
		if(unlikely(verbose))
		{
			logger << DebugError;
			logger << "Chip does not support " << (IO_WIDTH / NUM_CHIPS) << " bits I/O.std::decrease or increase either BYTESIZE or IO_WIDTH" << std::endl;
			logger << EndDebugError;
		}
		return false;
	}

	return true;
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
void AM29LV<CONFIG, BYTESIZE, IO_WIDTH>::Reset()
{
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
bool AM29LV<CONFIG, BYTESIZE, IO_WIDTH>::ReverseCompare(const uint8_t *data1, const uint8_t *data2, uint32_t size)
{
	if(size > 0)
	{
		const uint8_t *p1 = data1;
		const uint8_t *p2 = data2 + size - 1;
		do
		{
			if(*p1 != *p2) return false;
		} while(++p1, --p2, --size);
	}
	return true;
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
bool AM29LV<CONFIG, BYTESIZE, IO_WIDTH>::ReverseCopy(uint8_t *dest, const uint8_t *source, uint32_t size)
{
	if(size > 0)
	{
		uint8_t *dst = dest;
		const uint8_t *src = source + size - 1;
		do
		{
			*dst = *src;
		} while(++dst, --src, --size);
	}
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
int AM29LV<CONFIG, BYTESIZE, IO_WIDTH>::GetSector(typename CONFIG::ADDRESS addr)
{
	unsigned int sector_num;

	for(sector_num = 0; sector_num < CONFIG::NUM_SECTORS; sector_num++)
	{
		if(addr >= CONFIG::SECTOR_MAP[sector_num].addr &&
		   addr <= CONFIG::SECTOR_MAP[sector_num].addr + CONFIG::SECTOR_MAP[sector_num].size)
		{
			return sector_num;
		}
	}
	return -1;
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
void AM29LV<CONFIG, BYTESIZE, IO_WIDTH>::FSM(unsigned int chip_num, COMMAND command, typename CONFIG::ADDRESS addr, uint8_t *data, uint32_t size)
{
	unsigned int i;

	typename CONFIG::ADDRESS offset = (addr - org) & ADDR_MASK; // keep only needed address bits.
	typename CONFIG::ADDRESS chip_addr = offset >> addr_shift;

	for(i = 0; i < CONFIG::NUM_TRANSITIONS; i++)
	{
		const TRANSITION<typename CONFIG::ADDRESS, CONFIG::MAX_IO_WIDTH, typename CONFIG::STATE> *transition = &CONFIG::FSM[i];

		if(transition->initial_state == state[chip_num] &&
		   transition->initial_cycle == cycle[chip_num] &&
		   transition->command == command &&
		   (transition->wildcard_addr || (!transition->wildcard_addr && ((transition->addr >> config_addr_shift) == chip_addr))) &&
		   (transition->wildcard_data || (!transition->wildcard_data && size >= CHIP_IO_WIDTH && (endian == E_LITTLE_ENDIAN ? memcmp(transition->data, data, CHIP_IO_WIDTH) == 0 : ReverseCompare(transition->data, data, CHIP_IO_WIDTH)))))
		{
			state[chip_num] = transition->final_state;
			cycle[chip_num] = transition->final_cycle;
			switch(transition->action)
			{
				case ACT_NOP: return;
				case ACT_READ: Read(chip_num, offset, data, size); return;
				case ACT_READ_AUTOSELECT: ReadAutoselect(chip_num, offset, data, size); return;
				case ACT_PROGRAM: Program(chip_num, offset, data, size); return;
				case ACT_CHIP_ERASE: ChipErase(chip_num); return;
				case ACT_SECTOR_ERASE: SectorErase(chip_num, offset); return;
			}
		}
	}
	if(unlikely(verbose))
	{
		logger << DebugError;
		logger << "No transition found. You should check the FSM" << std::endl;
		logger << EndDebugError;
	}
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
bool AM29LV<CONFIG, BYTESIZE, IO_WIDTH>::FSM(COMMAND command, typename CONFIG::ADDRESS addr, uint8_t *data, uint32_t size)
{
	if(addr < org || (addr + size - 1) > (org + bytesize - 1) || (addr + size) < addr)
	{
		if(unlikely(verbose))
		{
			logger << DebugWarning;
			logger << "out of range address" << std::endl;
			logger << EndDebugWarning;
		}
		return false;
	}

	if(size > IO_WIDTH)
	{
		if(unlikely(verbose))
		{
			logger << DebugWarning;
			logger << "invalid transfer size (" << size << " bytes). Transfer size should be <= " << IO_WIDTH << std::endl;
			logger << EndDebugWarning;
		}
		return false;
	}

	unsigned int chip_num = (addr / CHIP_IO_WIDTH) % NUM_CHIPS;
	int32_t sz = (int32_t) size;

	do
	{
		FSM(chip_num, command, addr, data, sz > CHIP_IO_WIDTH ? CHIP_IO_WIDTH : sz);
	} while(data += CHIP_IO_WIDTH, addr += CHIP_IO_WIDTH, chip_num = (chip_num + 1) % NUM_CHIPS, (sz -= CHIP_IO_WIDTH) > 0);

	return true;
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
void AM29LV<CONFIG, BYTESIZE, IO_WIDTH>::Read(unsigned int chip_num, typename CONFIG::ADDRESS addr, uint8_t *data, uint32_t size)
{
	memcpy(data, storage + addr, size > CHIP_IO_WIDTH ? CHIP_IO_WIDTH : size);
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
void AM29LV<CONFIG, BYTESIZE, IO_WIDTH>::ReadAutoselect(unsigned int chip_num, typename CONFIG::ADDRESS addr, uint8_t *data, uint32_t size)
{
	typename CONFIG::ADDRESS chip_addr = addr >> addr_shift;

	switch((chip_addr & 0xff) << config_addr_shift)
	{
		case CONFIG::MANUFACTURER_ID_ADDR:
			if(unlikely(verbose))
			{
				logger << DebugInfo;
				logger << "Chip #" << chip_num << ": Reading Manufacturer ID" << std::endl;
				logger << EndDebugInfo;
			}
			if(endian == E_LITTLE_ENDIAN)
				memcpy(data, CONFIG::MANUFACTURER_ID, size > CHIP_IO_WIDTH ? CHIP_IO_WIDTH : size);
			else
				ReverseCopy(data, CONFIG::MANUFACTURER_ID, size > CHIP_IO_WIDTH ? CHIP_IO_WIDTH : size);
			return;
		case CONFIG::DEVICE_ID_ADDR:
			if(unlikely(verbose))
			{
				logger << DebugInfo;
				logger << "Chip #" << chip_num << ": Reading Device ID" << std::endl;
				logger << EndDebugInfo;
			}
			if(endian == E_LITTLE_ENDIAN)
				memcpy(data, CONFIG::DEVICE_ID, size > CHIP_IO_WIDTH ? CHIP_IO_WIDTH : size);
			else
				ReverseCopy(data, CONFIG::DEVICE_ID, size > CHIP_IO_WIDTH ? CHIP_IO_WIDTH : size);
			return;
		case CONFIG::SECTOR_PROTECT_VERIFY_ADDR:
			{
				if(unlikely(verbose))
				{
					logger << DebugInfo;
					logger << "Chip #" << chip_num << ": Sector protected verify" << std::endl;
					logger << EndDebugInfo;
				}
				typename CONFIG::ADDRESS sector_addr = (chip_addr >> 8) << config_addr_shift;
				int sector_num = GetSector(sector_addr);
				if(sector_num >= 0 && sector_protect[sector_num])
				{
					if(endian == E_LITTLE_ENDIAN)
						memcpy(data, CONFIG::PROTECTED, size > CHIP_IO_WIDTH ? CHIP_IO_WIDTH : size);
					else
						ReverseCopy(data, CONFIG::PROTECTED, size > CHIP_IO_WIDTH ? CHIP_IO_WIDTH : size);
				}
				else
				{
					if(endian == E_LITTLE_ENDIAN)
						memcpy(data, CONFIG::UNPROTECTED, size > CHIP_IO_WIDTH ? CHIP_IO_WIDTH : size);
					else
						ReverseCopy(data, CONFIG::UNPROTECTED, size > CHIP_IO_WIDTH ? CHIP_IO_WIDTH : size);
				}
			}
			return;
	}

	Read(chip_num, addr, data, size);
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
void AM29LV<CONFIG, BYTESIZE, IO_WIDTH>::Program(unsigned int chip_num, typename CONFIG::ADDRESS addr, const uint8_t *data, uint32_t size)
{
	typename CONFIG::ADDRESS chip_addr = addr >> addr_shift;
	typename CONFIG::ADDRESS sector_addr = chip_addr << config_addr_shift;
	int sector_num = GetSector(sector_addr);

	if(sector_num < 0)
	{
		logger << DebugWarning;
		logger << "Chip #" << chip_num << ": Sector #" << sector_num << " does not exist" << std::endl;
		logger << EndDebugWarning;
		return;
	}

	if(sector_protect[sector_num])
	{
		logger << DebugWarning;
		logger << "Chip #" << chip_num << ", Sector #" << sector_num << ", Addr 0x" << std::hex << chip_addr << std::dec << ": Attempting to program at byte address 0x" << std::hex << addr << std::dec << " while sector is protected" << std::endl;
		logger << EndDebugWarning;
		return;
	}

	if(unlikely(verbose))
	{
		logger << DebugInfo;
		logger << "Chip #" << chip_num << ", Sector #" << sector_num << ", Addr 0x" << std::hex << chip_addr << ": Programming ";
		uint32_t i;
		uint32_t n = size > CHIP_IO_WIDTH ? CHIP_IO_WIDTH : size;
		for(i = 0; i < n; i++)
		{
			logger << "0x" << (unsigned int) data[i];
			if(i < n - 1) logger << " ";
		}
		logger << std::dec << std::endl;
		logger << EndDebugInfo;
	}

	memcpy(storage + addr, data, size > CHIP_IO_WIDTH ? CHIP_IO_WIDTH : size);
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
void AM29LV<CONFIG, BYTESIZE, IO_WIDTH>::ChipErase(unsigned int chip_num)
{
	if(unlikely(verbose))
	{
		logger << DebugInfo;
		logger << "Erasing Chip #" << chip_num << std::endl;
		logger << EndDebugInfo;
	}

	uint32_t addr;
	for(addr = chip_num * CHIP_IO_WIDTH; addr < BYTESIZE; addr += IO_WIDTH)
	{
		memset(&storage[addr], 0, CHIP_IO_WIDTH);
	}
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
void AM29LV<CONFIG, BYTESIZE, IO_WIDTH>::SectorErase(unsigned int chip_num, typename CONFIG::ADDRESS addr)
{
	typename CONFIG::ADDRESS chip_addr = addr >> addr_shift;
	typename CONFIG::ADDRESS sector_addr = chip_addr << config_addr_shift;
	int sector_num = GetSector(sector_addr);

	if(sector_num < 0)
	{
		logger << DebugWarning;
		logger << "Sector #" << sector_num << " does not exist" << std::endl;
		logger << EndDebugWarning;
		return;
	}

	if(sector_protect[sector_num])
	{
		logger << DebugWarning;
		logger << "Attempting to erase sector #" << sector_num << " at 0x" << std::hex << addr << std::dec << " while sector is protected" << std::endl;
		logger << EndDebugWarning;
		return;
	}

	if(unlikely(verbose))
	{
		logger << DebugInfo;
		logger << "Erasing sector at 0x" << std::hex << addr << std::dec << " of chip #" << chip_num << std::endl;
		logger << EndDebugInfo;
	}

	memset(&storage[CONFIG::SECTOR_MAP[sector_num].addr * IO_WIDTH], 0, IO_WIDTH * CONFIG::SECTOR_MAP[sector_num].size);
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
bool AM29LV<CONFIG, BYTESIZE, IO_WIDTH>::WriteMemory(typename CONFIG::ADDRESS addr, const void *buffer, uint32_t size)
{
	if(addr < org || (addr + size - 1) > (org + bytesize - 1) || (addr + size) < addr)
	{
		if(unlikely(verbose))
		{
			logger << DebugWarning;
			logger << "out of range address" << std::endl;
			logger << EndDebugWarning;
		}
		return false;
	}
	// the third condition is for testing overwrapping (gdb did it !)

	typename CONFIG::ADDRESS offset = (addr - org) & ADDR_MASK; // keep only needed bits address
	memcpy(storage + offset, buffer, size);
	return true;
}

template <class CONFIG, uint32_t BYTESIZE, uint32_t IO_WIDTH>
bool AM29LV<CONFIG, BYTESIZE, IO_WIDTH>::ReadMemory(typename CONFIG::ADDRESS addr, void *buffer, uint32_t size)
{
	if(addr < org || (addr + size - 1) > (org + bytesize - 1) || (addr + size) < addr)
	{
		if(unlikely(verbose))
		{
			logger << DebugWarning;
			logger << "out of range address" << std::endl;
			logger << EndDebugWarning;
		}
		return false;
	}
	// the third condition is for testing overwrapping (gdb did it !)

	typename CONFIG::ADDRESS offset = (addr - org) & ADDR_MASK;
	memcpy(buffer, storage + offset, size);
	return true;
}

} // end of namespace am29lv
} // end of namespace flash
} // end of namespace memory
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_MEMORY_FLASH_AM29LV_AM29LV_TCC__
