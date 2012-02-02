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

#include "unisim/util/debug/simple_register.hh"

#include <unisim/component/cxx/processor/hcs12x/mmc.hh>

#include <stdio.h>
#include <assert.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace hcs12x {

using unisim::util::debug::SimpleRegister;

address_t MMC::MMC_REGS_ADDRESSES[MMC::MMC_MEMMAP_SIZE];

uint8_t MMC::gpage;
uint8_t MMC::direct;
uint8_t MMC::mmcctl1;
uint8_t MMC::rpage;
uint8_t MMC::epage;
uint8_t MMC::ppage;

MMC::MMC(const char *name, Object *parent):
	Object(name, parent),
	Service<Memory<service_address_t> >(name, parent),
	Client<Memory<service_address_t> >(name, parent),
	Service<Registers>(name, parent),
	memory_export("memory_export", this),
	memory_import("memory_import", this),
	registers_export("registers_export", this),
	debug_enabled(false),
	param_debug_enabled("debug-enabled", this, debug_enabled),
	mode_int(MMC_MODE_RESET),
	mmcctl1_int(MMCCTL1_RESET),
	param_mode("mode", this, mode_int),
	param_mmcctl1("mmcctl1", this, mmcctl1_int),
	address_encoding(ADDRESS::BANKED),
	param_address_encoding("address-encoding",this,address_encoding)

{

	MMC_REGS_ADDRESSES[MMCCTL0] = 0x000A;
	MMC_REGS_ADDRESSES[MODE] = 0x000B;
	MMC_REGS_ADDRESSES[GPAGE] = 0x0010;
	MMC_REGS_ADDRESSES[DIRECT] = 0x0011;
	MMC_REGS_ADDRESSES[MMCCTL1] = 0x0013;
	MMC_REGS_ADDRESSES[RPAGE] = 0x0016;
	MMC_REGS_ADDRESSES[EPAGE] = 0x0017;
	MMC_REGS_ADDRESSES[PPAGE] = 0x0030;
	MMC_REGS_ADDRESSES[RAMWPC] = 0x011C;
	MMC_REGS_ADDRESSES[RAMXGU] = 0x011D;
	MMC_REGS_ADDRESSES[RAMSHL] = 0x011E;
	MMC_REGS_ADDRESSES[RAMSHU] = 0x011F;


	Reset();
}

MMC::~MMC() {

	// Release registers_registry
	std::map<string, unisim::util::debug::Register *>::iterator reg_iter;

	for(reg_iter = registers_registry.begin(); reg_iter != registers_registry.end(); reg_iter++)
	{
		if(reg_iter->second)
			delete reg_iter->second;
	}

	registers_registry.clear();

	unsigned int i;
	unsigned int n = extended_registers_registry.size();
	for (i=0; i<n; i++) {
		delete extended_registers_registry[i];
	}

}

void MMC::Reset() {

	directSet = false;

	mmcctl0 = MMCCTL0_RESET;
	mode = mode_int;
	gpage = GLOBAL_RESET_PAGE;
	direct = DIRECT_RESET_PAGE;
	mmcctl1 = mmcctl1_int;

	rpage = RAM_RESET_PAGE;
	epage = EEPROM_RESET_PAGE;
	ppage = FLASH_RESET_PAGE;

	ramwpc = RAMWPC_RESET;
	ramxgu = RAMXGU_RESET;
	ramshl = RAMSHL_RESET;
	ramshu = RAMSHU_RESET;

}

bool MMC::BeginSetup() {


	char buf[80];

	sprintf(buf, "%s.MMCCTL0", GetName());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &mmcctl0);
	extended_registers_registry.push_back(new unisim::kernel::service::Register<uint8_t>("MMCCTL0", this, mmcctl0, "MMC Control Register (MMCCTL0)"));

	sprintf(buf, "%s.MODE", GetName());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &mode);
	extended_registers_registry.push_back(new unisim::kernel::service::Register<uint8_t>("MODE", this, mode, "Mode Register (MODE)"));

	sprintf(buf, "%s.GPAGE", GetName());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &gpage);
	extended_registers_registry.push_back(new unisim::kernel::service::Register<uint8_t>("GPAGE", this, gpage, "Global Page Index Register (GPAGE)"));

	sprintf(buf, "%s.DIRECT", GetName());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &direct);
	extended_registers_registry.push_back(new unisim::kernel::service::Register<uint8_t>("DIRECT", this, direct, "Direct Page Register (DIRECT)"));

	sprintf(buf, "%s.MMCCTL1", GetName());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &mmcctl1);
	extended_registers_registry.push_back(new unisim::kernel::service::Register<uint8_t>("MMCCTL1", this, mmcctl1, "MMC Control Register (MMCCTL1)"));

	sprintf(buf, "%s.RPAGE", GetName());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &rpage);
	extended_registers_registry.push_back(new unisim::kernel::service::Register<uint8_t>("RPAGE", this, rpage, "RAM Page Index Register (RPAGE)"));

	sprintf(buf, "%s.EPAGE", GetName());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &epage);
	extended_registers_registry.push_back(new unisim::kernel::service::Register<uint8_t>("EPAGE", this, epage, "EEPROM Page Index Register (EPAGE)"));

	sprintf(buf, "%s.PPAGE", GetName());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &ppage);
	extended_registers_registry.push_back(new unisim::kernel::service::Register<uint8_t>("PPAGE", this, ppage, "Program Page Index Register (PPAGE)"));

	sprintf(buf, "%s.RAMWPC", GetName());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &ramwpc);
	extended_registers_registry.push_back(new unisim::kernel::service::Register<uint8_t>("RAMWPC", this, ramwpc, "RAM Write Protection Control Register (RAMWPC)"));

	sprintf(buf, "%s.RAMXGU", GetName());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &ramxgu);
	extended_registers_registry.push_back(new unisim::kernel::service::Register<uint8_t>("RAMXGU", this, ramxgu, "RAM XGATE Upper Boundary Register (RAMXGU)"));

	sprintf(buf, "%s.RAMSHL", GetName());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &ramshl);
	extended_registers_registry.push_back(new unisim::kernel::service::Register<uint8_t>("RAMSHL", this, ramshl, "RAM Shared Region Lower Boundary Register (RAMSHL)"));

	sprintf(buf, "%s.RAMSHU", GetName());
	registers_registry[buf] = new SimpleRegister<uint8_t>(buf, &ramshu);
	extended_registers_registry.push_back(new unisim::kernel::service::Register<uint8_t>("RAMSHU", this, ramshu, "RAM Shared Region Upper Boundary Register (RAMSHU)"));

	return true;
}

bool MMC::Setup(ServiceExportBase *srv_export) {
	return true;
}

bool MMC::EndSetup() {
	return true;
}

Register* MMC::GetRegister(const char *name)
{
	if(registers_registry.find(string(name)) != registers_registry.end())
		return registers_registry[string(name)];
	else
		return NULL;

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


	SplitPagedAddress(paged_addr, page, cpu_address);
	addr = getPhysicalAddress(cpu_address, ADDRESS::EXTENDED, false, true, page);

	if (addr <= REG_HIGH_OFFSET) {
		for (uint8_t i=0; i<MMC_MEMMAP_SIZE; i++) {
			if (MMC_REGS_ADDRESSES[i] == addr) {
				*((uint8_t *) buffer) = read(addr);
				return true;
			}
		}

	}


	if (memory_import) {
		return memory_import->ReadMemory(addr, (uint8_t *) buffer, size);
	}

	return false;
}

// TODO: review this methods for dump and disassemble commands
bool MMC::WriteMemory(service_address_t paged_addr, const void *buffer, uint32_t size) {

	page_t page;
	address_t cpu_address;
	physical_address_t addr;

	if (size == 0) {
		return true;
	}

	SplitPagedAddress(paged_addr, page, cpu_address);
	addr = getPhysicalAddress(cpu_address, ADDRESS::EXTENDED, false, true, page);

	if (addr <= REG_HIGH_OFFSET) {
		for (uint8_t i=0; i<MMC_MEMMAP_SIZE; i++) {
			if (MMC_REGS_ADDRESSES[i] == addr) {
				write(addr, *((uint8_t *) buffer));
				return true;
			}
		}
	}


	if (memory_import) {
		return memory_import->WriteMemory(addr, (uint8_t *) buffer, size);
	}

	return false;
}

}
}
}
}
} // end namespace hcs12x

