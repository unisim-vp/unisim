/*
 *  Copyright (c) 2008,
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
 * Authors: Reda   Nouacer  (reda.nouacer@cea.fr)
 */

#include <unisim/component/cxx/processor/hcs12x/mmc.hh>

#include <stdio.h>
#include <assert.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace hcs12x {

MMC::MMC(const char *name, Object *parent):
	Object(name, parent),
	Service<Memory<service_address_t> >(name, parent),
	Client<Memory<service_address_t> >(name, parent),
	memory_export("memory_export", this),
	internal_memory_import("internal_memory_import", this),
	external_memory_import("external_memory_import", this),
	address_encoding(ADDRESS::BANKED),
	param_address_encoding("address-encoding",this,address_encoding),
	mode_int(MMC_MODE_RESET),
	mmcctl1_int(MMCCTL1_RESET),
	debug_enabled(false),
	param_debug_enabled("debug-enabled", this, debug_enabled),
	param_mode("mode", this, mode_int),
	param_mmcctl1("mmcctl1", this, mmcctl1_int)

{

	Reset();
}

void MMC::Reset() {

	directSet = false;
	write(MMCCTL0_REG_ADDRESS, MMCCTL0_RESET);
	write(MMC_MODE_REG_ADDRESS, mode_int);
	write(GPAGE_REG_ADDRESS, GLOBAL_RESET_PAGE);
	write(DIRECT_REG_ADDRESS, DIRECT_RESET_PAGE);
	write(MMCCTL1_REG_ADDRESS, mmcctl1_int);

	write(RPAGE_REG_ADDRESS, RAM_RESET_PAGE);
	write(EPAGE_REG_ADDRESS, EEPROM_RESET_PAGE);
	write(PPAGE_REG_ADDRESS, FLASH_RESET_PAGE);

	write(RAMWPC_REG_ADDRESS, RAMWPC_RESET);
	write(RAMXGU_REG_ADDRESS, RAMXGU_RESET);
	write(RAMSHL_REG_ADDRESS, RAMSHL_RESET);
	write(RAMSHU_REG_ADDRESS, RAMSHU_RESET);

}

bool MMC::Setup() {

	return true;
}

void MMC::OnDisconnect() {
}

void MMC::SplitPagedAddress(physical_address_t paged_addr, page_t &page, address_t &cpu_address)
{
	if (address_encoding == ADDRESS::GNUGCC)
	{
		if (paged_addr < 0x10000) {
			page = 0;
			cpu_address = (address_t) paged_addr;
		} else {
			page = (page_t) ((paged_addr - 0x10000) / 0x4000);
			cpu_address = (address_t) ((paged_addr - 0x10000) % 0x4000);
		}
	}
	else
	{
		if (address_encoding == ADDRESS::LINEAR) {
			page = (page_t) (paged_addr / 0x4000);
			cpu_address = (address_t) ((paged_addr % 0x4000) + 0x8000);
		} else { // BANKED
			page = (page_t) (paged_addr / 0x10000);
			cpu_address = (address_t) (paged_addr % 0x10000);
		}
	}

}

// TODO: review this methods for dump and disassemble commands
bool MMC::ReadMemory(service_address_t paged_addr, void *buffer, uint32_t size) {

	page_t page;
	address_t cpu_address;
	physical_address_t addr;

	// TODO: read MMC registers

	SplitPagedAddress(paged_addr, page, cpu_address);
	addr = getPhysicalAddress(cpu_address, ADDRESS::EXTENDED, false, true, page);

	if (isPaged(cpu_address, page, false, true)) {
		if (external_memory_import) {
			return external_memory_import->ReadMemory(addr, (uint8_t *) buffer, size);
		}
	} else {
		if (internal_memory_import) {
			return internal_memory_import->ReadMemory(addr, (uint8_t *) buffer, size);
		}
	}

	return false;
}

// TODO: review this methods for dump and disassemble commands
bool MMC::WriteMemory(service_address_t paged_addr, const void *buffer, uint32_t size) {

	page_t page;
	address_t cpu_address;
	physical_address_t addr;

	// TODO: write to MMC registers

	SplitPagedAddress(paged_addr, page, cpu_address);
	addr = getPhysicalAddress(cpu_address, ADDRESS::EXTENDED, false, true, page);

	if (isPaged(cpu_address, page, false, true)) {
		if (external_memory_import) {
			return external_memory_import->WriteMemory(addr, (uint8_t *) buffer, size);
		}
	} else {
		if (internal_memory_import) {
			return internal_memory_import->WriteMemory(addr, (uint8_t *) buffer, size);
		}
	}

	return false;
}

}
}
}
}
} // end namespace hcs12x

