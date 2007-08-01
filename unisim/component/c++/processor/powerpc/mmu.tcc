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
 
#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_MMU_TCC__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_MMU_TCC__

#include <unisim/service/interfaces/memory.hh>
#include <unisim/component/c++/tlb/tlb.hh>
#include <unisim/service/interfaces/registers.hh>
#include <unisim/util/debug/simple_register.hh>
#include <unisim/component/c++/cache/cache_interface.hh>
#include <unisim/component/c++/processor/powerpc/exception.hh>
#include <unisim/util/endian/endian.hh>
#include <unisim/kernel/service/service.hh>
#include <unisim/service/interfaces/cache_power_estimator.hh>
#include <unisim/service/interfaces/power_mode.hh>
#include <iostream>
#include <sstream>
#include <map>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {

template <class VIRTUAL_ADDRESS, class PHYSICAL_ADDRESS>
ostream& operator << (ostream& os, const MemoryPageTableEntry<VIRTUAL_ADDRESS, PHYSICAL_ADDRESS>& pte)
{
	os << "PTE(V=" << pte.valid
		<< ",way=" << pte.way
		<< ",base_virt_addr=0x" << std::hex << pte.base_virtual_addr
		<< ",pte_addr=0x" << pte.pte_addr
		<< ",base_phys_addr=0x" << pte.base_physical_addr << std::dec
		<< ",c=" << pte.c
		<< ",wimg=" << pte.wimg
		<< ",pp=" << pte.pp
		<< ")";
	return os;
}

template <class CONFIG>
MMU<CONFIG>::MMU(const char *name, CacheInterface<physical_address_t> *icache_interface, CacheInterface<physical_address_t> *dcache_interface, BusInterface<physical_address_t> *bus_interface, Object *parent) :
	Object(name, parent),
	Client<MemoryInterface<address_t> >(name, parent),
	Client<LoggerInterface>(name, parent),
	to_mem("to-mem", this),
	to_dcache("to-dcache", this),
	to_icache("to-icache", this),
	itlb_power_estimator_import("itlb-power-estimator-import", this),
	dtlb_power_estimator_import("dtlb-power-estimator-import", this),
	itlb_power_mode_import("itlb-power-mode-import", this),
	dtlb_power_mode_import("dtlb-power-mode-import", this),
	itlb_power_mode_export("itlb-power-mode-export", this),
	dtlb_power_mode_export("dtlb-power-mode-export", this),
	itlb("itlb", this),
	dtlb("dtlb", this),
	logger_import("logger-import", this)
{
	itlb.power_estimator_import >> itlb_power_estimator_import;
	dtlb.power_estimator_import >> dtlb_power_estimator_import;
	itlb_power_mode_export >> itlb.power_mode_export;
	dtlb_power_mode_export >> dtlb.power_mode_export;
	itlb.power_mode_import >> itlb_power_mode_import;
	dtlb.power_mode_import >> dtlb_power_mode_import;

	int i;

	this->bus_interface = bus_interface;
	this->icache_interface = icache_interface;
	this->dcache_interface = dcache_interface;
	this->mmc = MMC_NONE;

	for(i = 0; i < CONFIG::NUM_BATS; i++)
	{
		stringstream sstr;
		sstr << "dbat" << i << "u";
		registers_registry[sstr.str().c_str()] = new SimpleRegisterInterface<uint32_t>(sstr.str().c_str(), &dbatu[i]);
	}

	for(i = 0; i < CONFIG::NUM_BATS; i++)
	{
		stringstream sstr;
		sstr << "dbat" << i << "l";
		registers_registry[sstr.str().c_str()] = new SimpleRegisterInterface<uint32_t>(sstr.str().c_str(), &dbatl[i]);
	}

	for(i = 0; i < CONFIG::NUM_BATS; i++)
	{
		stringstream sstr;
		sstr << "ibat" << i << "u";
		registers_registry[sstr.str().c_str()] = new SimpleRegisterInterface<uint32_t>(sstr.str().c_str(), &ibatu[i]);
	}

	for(i = 0; i < CONFIG::NUM_BATS; i++)
	{
		stringstream sstr;
		sstr << "ibat" << i << "l";
		registers_registry[sstr.str().c_str()] = new SimpleRegisterInterface<uint32_t>(sstr.str().c_str(), &ibatl[i]);
	}

	for(i = 0; i < 16; i++)
	{
		stringstream sstr;
		sstr << "sr" << i;
		registers_registry[sstr.str().c_str()] = new SimpleRegisterInterface<uint32_t>(sstr.str().c_str(), &sr[i]);
	}

	registers_registry["sdr1"] = new SimpleRegisterInterface<uint32_t>("sdr1", &sdr1);
}

template <class CONFIG>
MMU<CONFIG>::~MMU()
{
	typename map<string, RegisterInterface *>::iterator reg_iter;

	for(reg_iter = registers_registry.begin(); reg_iter != registers_registry.end(); reg_iter++)
	{
		delete reg_iter->second;
	}
}

template <class CONFIG>
void MMU<CONFIG>::Reset()
{
	unsigned int i;
	for(i = 0; i < CONFIG::NUM_BATS; i++)
	{
		SetDBATU(i, 0);
		SetDBATL(i, 0);
		SetIBATU(i, 0);
		SetIBATL(i, 0);
	}
	for(i = 0; i < 16; i++)
	{
		SetSR(i, 0);
	}
	SetSDR1(0);
	dtlb.Invalidate();
	itlb.Invalidate();
	mmc = MMC_NONE;
}

template <class CONFIG>
void MMU<CONFIG>::SetControl(MMUControl mmc)
{
	this->mmc = mmc;
}

template <class CONFIG>
MMUControl MMU<CONFIG>::GetControl()
{
	return mmc;
}

template <class CONFIG>
void MMU<CONFIG>::EnableDebug()
{
	mmc = (MMUControl)(mmc | MMC_DEBUG_ENABLE);
}

template <class CONFIG>
void MMU<CONFIG>::DisableDebug()
{
	mmc = (MMUControl)(mmc & ~MMC_DEBUG_ENABLE);
}

template <class CONFIG>
void MMU<CONFIG>::EnableIMMU()
{
	mmc = (MMUControl)(mmc | MMC_IMMU_ENABLE);
}

template <class CONFIG>
void MMU<CONFIG>::DisableIMMU()
{
	mmc = (MMUControl)(mmc & ~MMC_IMMU_ENABLE);
}

template <class CONFIG>
void MMU<CONFIG>::EnableDMMU()
{
	mmc = (MMUControl)(mmc | MMC_DMMU_ENABLE);
}

template <class CONFIG>
void MMU<CONFIG>::DisableDMMU()
{
	mmc = (MMUControl)(mmc & ~MMC_DMMU_ENABLE);
}

template <class CONFIG>
void MMU<CONFIG>::EnableAddressOnlyBroadcast()
{
	mmc = (MMUControl)(mmc | MMC_ADDRESS_BROADCAST_ENABLE);
}

template <class CONFIG>
void MMU<CONFIG>::DisableAddressOnlyBroadcast()
{
	mmc = (MMUControl)(mmc & ~MMC_ADDRESS_BROADCAST_ENABLE);
}

template <class CONFIG>
void MMU<CONFIG>::EnableSoftwareTableSearch()
{
	mmc = (MMUControl)(mmc | MMC_SOFTWARE_TABLE_SEARCH_ENABLE);
}

template <class CONFIG>
void MMU<CONFIG>::DisableSoftwareTableSearch()
{
	mmc = (MMUControl)(mmc & ~MMC_SOFTWARE_TABLE_SEARCH_ENABLE);
}

template <class CONFIG>
void MMU<CONFIG>::EnableHighBAT()
{
	mmc = (MMUControl)(mmc | MMC_HIGH_BAT_ENABLE);
}

template <class CONFIG>
void MMU<CONFIG>::DisableHighBAT()
{
	mmc = (MMUControl)(mmc & ~MMC_HIGH_BAT_ENABLE);
}

template <class CONFIG>
bool MMU<CONFIG>::ReadMemory(address_t addr, void *buffer, uint32_t size)
{
	if(!to_mem) return false;

	bool ret = false;
	MMUControl prev_mmc = mmc;
	EnableDebug();

	physical_address_t physical_addr;
	address_t protection_boundary;
	WIMG wimg;

	if(mmc & MMC_DMMU_ENABLE)
	{
		// Data MMU is enabled
		uint32_t read_offset = 0;

		do
		{
			physical_addr = TranslateAddress(addr, PR_SUPERVISOR, MAT_READ, MT_DATA, protection_boundary, wimg);
			uint32_t size_to_protection_boundary = protection_boundary - addr;
			if(to_dcache && !(wimg & WIMG_CACHE_INHIBITED))
			{
				if(size_to_protection_boundary >= size)
				{
					// Memory access does not cross a protection boundary
					ret = ret || to_dcache->ReadMemory(physical_addr, (uint8_t *) buffer + read_offset, size);
					size = 0;
				}
				else
				{
					// Memory access crosses a protection boundary
					ret = ret || to_dcache->ReadMemory(physical_addr, (uint8_t *) buffer + read_offset, size_to_protection_boundary);
					size -= size_to_protection_boundary;
					read_offset += size_to_protection_boundary;
					addr += size_to_protection_boundary;
				}
			}
			else
			{
				ret = ret || to_mem->ReadMemory(physical_addr, buffer, size);
				size = 0;
			}
		} while(size > 0);
	}
	else
	{
		// Data MMU is disabled
		physical_addr = addr;
		ret = ret || (to_dcache ? to_dcache->ReadMemory(physical_addr, buffer, size) : to_mem->ReadMemory(physical_addr, buffer, size));
	}

	mmc = prev_mmc;
	return ret;
}

template <class CONFIG>
bool MMU<CONFIG>::WriteMemory(address_t addr, const void *buffer, uint32_t size)
{
	if(!to_mem) return false;

	bool ret = false;
	MMUControl prev_mmc = mmc;
	EnableDebug();

	physical_address_t physical_addr;
	address_t protection_boundary;
	WIMG wimg;

	if(mmc & MMC_DMMU_ENABLE)
	{
		// Data MMU is enabled
		uint32_t write_offset = 0;

		do
		{
			physical_addr = TranslateAddress(addr, PR_SUPERVISOR, MAT_WRITE, MT_DATA, protection_boundary, wimg);
			uint32_t size_to_protection_boundary = protection_boundary - addr;
			if(to_dcache && !(wimg & WIMG_CACHE_INHIBITED))
			{
				if(size_to_protection_boundary >= size)
				{
					// Memory access does not cross a protection boundary
					ret = ret || to_dcache->WriteMemory(physical_addr, (uint8_t *) buffer + write_offset, size);
					size = 0;
				}
				else
				{
					// Memory access crosses a protection boundary
					ret = ret || to_dcache->WriteMemory(physical_addr, (uint8_t *) buffer + write_offset, size_to_protection_boundary);
					size -= size_to_protection_boundary;
					write_offset += size_to_protection_boundary;
					addr += size_to_protection_boundary;
				}
			}
			else
			{
				ret = ret || to_mem->WriteMemory(physical_addr, buffer, size);
				size = 0;
			}
		} while(size > 0);
	}
	else
	{
		// Data MMU is disabled
		physical_addr = addr;
		ret = ret || (to_dcache ? to_dcache->WriteMemory(physical_addr, buffer, size) : to_mem->WriteMemory(physical_addr, buffer, size));
	}

	mmc = prev_mmc;
	return ret;
}

template <class CONFIG>
void MMU<CONFIG>::ReadDataMemory(address_t ea, void *buffer, uint32_t size, PrivilegeLevel privilege_level)
{
	physical_address_t physical_addr;
	address_t protection_boundary;
	WIMG wimg;
	CacheStatus cs;

	if(mmc & MMC_DMMU_ENABLE)
	{
		// Data MMU is enabled

		physical_addr = TranslateAddress(ea, privilege_level, MAT_READ, MT_DATA, protection_boundary, wimg);
		if(dcache_interface && !(wimg & WIMG_CACHE_INHIBITED))
		{
			// The system has a data cache. It is enabled and not inhibited
			CacheControl cc = (CacheControl)(wimg & 0xe); // This is an optimization

			// Memory access does not cross a protection boundary
			dcache_interface->PrRead(physical_addr, buffer, size, cc);
			// if a bus error occured then raise a machine check exception
		}
		else
		{
			// The system has no data cache or it is disabled or inhibited
			BusControl bc = (wimg & WIMG_MEMORY_COHERENCY_ENFORCED) ? BC_GLOBAL : BC_NONE;

			// Memory access does not cross a page boundary
			bus_interface->BusRead(physical_addr, buffer, size, bc, cs);
		}
	}
	else
	{
		// Data MMU is disabled

		// The physical address is just the effective address
		physical_addr = ea;
		// These are WIMG bits in real addressing mode
		// W=0: Write through disabled
		// I=0: Cache not inhibited
		// M=1: Memory coherency enforced
		// G=1: Guarded memory access (speculative fetch forbidden)
		if(dcache_interface)
		{
			// The system has a data cache
			dcache_interface->PrRead(physical_addr, buffer, size, CC_MEMORY_COHERENCY_ENFORCED);
		}
		else
		{
			// The system has no data cache
			bus_interface->BusRead(physical_addr, buffer, size, BC_GLOBAL, cs);
		}
	}
}

template <class CONFIG>
uint32_t MMU<CONFIG>::ReadInsnMemory(address_t ea, PrivilegeLevel privilege_level, physical_address_t& physical_addr)
{
	uint32_t insn;
	address_t protection_boundary;
	WIMG wimg;
	CacheStatus cs;

	if(mmc & MMC_IMMU_ENABLE)
	{
		// Instruction MMU is enabled
		physical_addr = TranslateAddress(ea, privilege_level, MAT_READ, MT_INSN, protection_boundary, wimg);
		if(!(mmc & MMC_DEBUG_ENABLE))
		{
			if(icache_interface && !(wimg & WIMG_CACHE_INHIBITED))
			{
				// The system has an instruction cache. It is enabled and not inhibited
				CacheControl cc = (CacheControl)(wimg & 0xe); // This is an optimization
	
				// Instruction memory access does not cross a protection boundary
				icache_interface->PrRead(physical_addr, &insn, 4, cc);
			}
			else
			{
				// The system has no instruction cache or it is disabled or inhibited
				BusControl bc = (wimg & WIMG_MEMORY_COHERENCY_ENFORCED) ? BC_GLOBAL : BC_NONE;
	
				// Instruction memory access does not cross a page boundary
				bus_interface->BusRead(physical_addr, &insn, 4, bc, cs);
			}
		}
		else
		{
			if(to_icache && !(wimg & WIMG_CACHE_INHIBITED))
			{
				// Instruction memory access does not cross a protection boundary
				to_icache->ReadMemory(physical_addr, &insn, 4);
			}
			else
			{
				// Instruction memory access does not cross a page boundary
				to_mem->ReadMemory(physical_addr, &insn, 4);
			}
		}
	}
	else
	{
		// Instruction MMU is disabled

		// The physical address is just the effective address
		physical_addr = ea;
		// These are WIMG bits in real addressing mode
		// W=0: Write through disabled
		// I=0: Cache not inhibited
		// M=1: Memory coherency enforced
		// G=1: Guarded memory access (speculative fetch forbidden)
		if(!(mmc & MMC_DEBUG_ENABLE))
		{
			if(icache_interface)
			{
				// The system has an instruction cache
				icache_interface->PrRead(physical_addr, &insn, 4, CC_MEMORY_COHERENCY_ENFORCED);
			}
			else
			{
				// The system has no data cache
				bus_interface->BusRead(physical_addr, &insn, 4, BC_GLOBAL, cs);
			}
		}
		else
		{
			if(to_icache)
			{
				// The system has an instruction cache
				to_icache->ReadMemory(physical_addr, &insn, 4);
			}
			else
			{
				// The system has no data cache
				to_mem->ReadMemory(physical_addr, &insn, 4);
			}
		}
	}
#if BYTE_ORDER == LITTLE_ENDIAN
	BSwap(insn);
#endif
	return insn;
}

template <class CONFIG>
uint32_t MMU<CONFIG>::ReadInsnMemory(address_t ea, PrivilegeLevel privilege_level, void *buffer, uint32_t size, physical_address_t& physical_addr)
{
	address_t protection_boundary;
	WIMG wimg;
	CacheStatus cs;

	if(mmc & MMC_IMMU_ENABLE)
	{
		// Instruction MMU is enabled
		physical_addr = TranslateAddress(ea, privilege_level, MAT_READ, MT_INSN, protection_boundary, wimg);
		uint32_t size_to_protection_boundary = protection_boundary - physical_addr;
		if(size > size_to_protection_boundary) size = size_to_protection_boundary;

		if(!(mmc & MMC_DEBUG_ENABLE))
		{
			if(icache_interface && !(wimg & WIMG_CACHE_INHIBITED))
			{
				// The system has an instruction cache. It is enabled and not inhibited
				CacheControl cc = (CacheControl)(wimg & 0xe); // This is an optimization
	
				// Instruction memory access does not cross a protection boundary
				icache_interface->PrRead(physical_addr, buffer, size, cc);
			}
			else
			{
				// The system has no instruction cache or it is disabled or inhibited
				BusControl bc = (wimg & WIMG_MEMORY_COHERENCY_ENFORCED) ? BC_GLOBAL : BC_NONE;
	
				// Instruction memory access does not cross a page boundary

				uint32_t offset = 0;
				do
				{
					if(size <= FSB_WIDTH)
					{
						bus_interface->BusRead(physical_addr + offset, (uint8_t *) buffer + offset, size, bc, cs);
						break;
					}
					bus_interface->BusRead(physical_addr + offset, (uint8_t *) buffer + offset, FSB_WIDTH, bc, cs);
					size -= FSB_WIDTH;
					offset += FSB_WIDTH;
				} while(1);
			}
		}
		else
		{
			if(to_icache && !(wimg & WIMG_CACHE_INHIBITED))
			{
				// Instruction memory access does not cross a protection boundary
				to_icache->ReadMemory(physical_addr, buffer, size);
			}
			else
			{
				// Instruction memory access does not cross a page boundary
				to_mem->ReadMemory(physical_addr, buffer, size);
			}
		}
	}
	else
	{
		// Instruction MMU is disabled

		// The physical address is just the effective address
		physical_addr = ea;
		// These are WIMG bits in real addressing mode
		// W=0: Write through disabled
		// I=0: Cache not inhibited
		// M=1: Memory coherency enforced
		// G=1: Guarded memory access (speculative fetch forbidden)
		if(!(mmc & MMC_DEBUG_ENABLE))
		{
			if(icache_interface)
			{
				// The system has an instruction cache
				icache_interface->PrRead(physical_addr, buffer, size, CC_MEMORY_COHERENCY_ENFORCED);
			}
			else
			{
				// The system has no data cache
				uint32_t offset = 0;
				do
				{
					if(size <= FSB_WIDTH)
					{
						bus_interface->BusRead(physical_addr + offset, (uint8_t *) buffer + offset, size, BC_GLOBAL, cs);
						break;
					}
					bus_interface->BusRead(physical_addr + offset, (uint8_t *) buffer + offset, FSB_WIDTH, BC_GLOBAL, cs);
					size -= FSB_WIDTH;
					offset += FSB_WIDTH;
				} while(1);
			}
		}
		else
		{
			if(to_icache)
			{
				// The system has an instruction cache
				to_icache->ReadMemory(physical_addr, buffer, size);
			}
			else
			{
				// The system has no data cache
				to_mem->ReadMemory(physical_addr, buffer, size);
			}
		}
	}

	return size;
}

template <class CONFIG>
void MMU<CONFIG>::WriteDataMemory(address_t ea, const void *buffer, uint32_t size, PrivilegeLevel privilege_level)
{
	physical_address_t physical_addr;
	address_t protection_boundary;
	WIMG wimg;

	if(mmc & MMC_DMMU_ENABLE)
	{
		// Data MMU is enabled
		physical_addr = TranslateAddress(ea, privilege_level, MAT_WRITE, MT_DATA, protection_boundary, wimg);
		if(dcache_interface && !(wimg & WIMG_CACHE_INHIBITED))
		{
			// The system has a data cache. It is enabled and not inhibited
			CacheControl cc = (CacheControl)(wimg & 0xe); // This is an optimization

			// Memory access does not cross a protection boundary
			dcache_interface->PrWrite(physical_addr, buffer, size, cc);
		}
		else
		{
			// The system has no data cache or it is disabled or inhibited
			BusControl bc = (wimg & WIMG_MEMORY_COHERENCY_ENFORCED) ? BC_GLOBAL : BC_NONE;

			// Memory access does not cross a page boundary
			bus_interface->BusWrite(physical_addr, buffer, size, bc);
		}
	}
	else
	{
		// Data MMU is disabled

		// The physical address is just the effective address
		physical_addr = ea;
		// These are WIMG bits in real addressing mode
		// W=0: Write through disabled
		// I=0: Cache not inhibited
		// M=1: Memory coherency enforced
		// G=1: Guarded memory access (speculative fetch forbidden)
		if(dcache_interface)
		{
			// The system has a data cache
			dcache_interface->PrWrite(physical_addr, buffer, size, CC_MEMORY_COHERENCY_ENFORCED);
		}
		else
		{
			// The system has no data cache
			bus_interface->BusWrite(physical_addr, buffer, size, BC_GLOBAL);
		}
	}
}

template <class CONFIG>
typename CONFIG::physical_address_t MMU<CONFIG>::TranslateAddress(typename CONFIG::address_t addr, PrivilegeLevel privilege_level, MemoryAccessType memory_access_type, MemoryType memory_type, typename CONFIG::address_t& protection_boundary, WIMG& wimg)
{
	uint32_t bat_num;						// BAT register number
	uint32_t bepi;							// BEPI bit field
	uint32_t sr_num;						// Segment register number
	uint32_t virtual_segment_id;			// Virtual segment id
	uint32_t sr_ks;							// Supervisor key of a segment register
	uint32_t sr_kp;							// User key of a segment register
	uint32_t sr_noexecute;					// No-execute bit of a segment register
	virtual_address_t virtual_addr;			// Virtual address
	virtual_address_t base_virtual_addr;	// Base virtual address
	MemoryPageTableEntry<typename CONFIG::virtual_address_t, typename CONFIG::physical_address_t> *pte;				// A Memory page table entry (in DTLB/ITLB)
	uint32_t tlb_index;						// An index in DTLB/ITLB
	uint32_t h;
	uint32_t page_index;					// page index
	uint32_t api;							// Abreviated page index
	uint32_t hash;							// hash key
	uint32_t pteg_select;
	physical_address_t pte_addr;			// a page table entry address
	uint32_t pte_num;
	bool key;
	
	if(memory_type == MT_INSN)
	{
		if(!(mmc & MMC_IMMU_ENABLE))
		{
			// W=0: Write through disabled
			// I=0: Cache not inhibited
			// M=1: Memory coherency enforced
			// G=1: Guarded memory access (speculative fetch forbidden)
			wimg = (WIMG)(WIMG_MEMORY_COHERENCY_ENFORCED | WIMG_GUARDED_MEMORY);
			return addr;
		}
	}
	else
	{
		if(!(mmc & MMC_DMMU_ENABLE))
		{
			// W=0: Write through disabled
			// I=0: Cache not inhibited
			// M=1: Memory coherency enforced
			// G=1: Guarded memory access (speculative fetch forbidden)
			wimg = (WIMG)(WIMG_MEMORY_COHERENCY_ENFORCED | WIMG_GUARDED_MEMORY);
			return addr;
		}
	}
	//------------------------------
	//-  Block Address Translation -
	//------------------------------

	// Compute BEPI from the effective address
	bepi = (addr >> 17) & 0x7fff;

	if(memory_type == MT_INSN)
	{
		// Memory access is an instruction fetch
		uint32_t num_ibats = (mmc & MMC_HIGH_BAT_ENABLE) ? CONFIG::NUM_BATS : 4;

		// Scan each IBAT register to find a translation
		for(bat_num = 0; bat_num < num_ibats; bat_num++)
		{
/*			if(addr == 0xc0174524)
			{
				cerr << "BEPI = 0x" << hex << bepi << endl;
				cerr << "IBAT" << dec << bat_num << "U = 0x" << hex << GetIBATU(bat_num) << endl;
				cerr << "IBAT" << dec <<bat_num << "L = 0x" << hex << GetIBATL(bat_num) << endl;
				cerr << "IBAT" << dec << bat_num << "U_BEPI = 0x" << hex << (GetIBATU(bat_num) >> 17) << endl;
				cerr << "IBAT" << dec << bat_num << "U_BL = 0x" << hex << GetIBATU_BL(bat_num) << endl;
				if((bepi & 0x7800) == (GetIBATU_BEPI(bat_num) & 0x7800))
				{
					cerr << "BEPI match" << endl;
				}
			}*/
			// Check if IBAT register match
			if((bepi & 0x7800) == (GetIBATU_BEPI(bat_num) & 0x7800) // compare the 4 most significative bits of
				// the 15 most significative bits in EA and 15-bit IBAT BEPI field
						 && (bepi & ~GetIBATU_BL(bat_num) & 0x7ff) == (GetIBATU_BEPI(bat_num) & 0x7ff)) // compare the 11 least significative
				// bits of the 15 most significative bits in EA and IBAT BEPI field according to the IBAT block size mask (BL)
			{
				//cerr << "privilege level = " << ((privilege_level == PR_SUPERVISOR) ? "supervisor" : "user") << endl;
				//cerr << "IBATU_VP = " << GetIBATU_VP(bat_num) << endl;
				//cerr << "IBATU_VS = " << GetIBATU_VS(bat_num) << endl;
				if((GetIBATU_VP(bat_num) && (privilege_level & PR_USER)) || (GetIBATU_VS(bat_num) && (privilege_level & PR_SUPERVISOR)))
				{
					//cerr << "IBAT match for 0x" << hex << addr << endl;
					// A BAT match occurs
					protection_boundary = ((GetIBATU_BEPI(bat_num) & 0x7ffUL) + GetIBATU_BL(bat_num) + 1) << 17;
					wimg = (WIMG)(GetIBATL_WIMG(bat_num) & 0x6); // W and G are not defined in IBATs;

					// Check if access is guarded
					if(wimg & WIMG_GUARDED_MEMORY)
					{
						if(!(mmc & MMC_DEBUG_ENABLE)) throw ISIGuardedMemoryException<CONFIG>(addr);
					}

					// Check access rights
					uint32_t pp = GetIBATL_PP(bat_num);
					if(!pp) // PP=00: No access
					{
						// Raise a protection violation
						if(!(mmc & MMC_DEBUG_ENABLE)) throw ISIProtectionViolationException<CONFIG>(addr);
					}
					// Access is allowed
				
					// Compute the physical address
					return ((((bepi & 0x7ff) & GetIBATU_BL(bat_num)) | (GetIBATL_BRPN(bat_num) & 0x7ff) | (GetIBATL_BRPN(bat_num) & 0x7800)) << 17) | (addr & 0x1ffff);
				}
			}
		}
	}
	else
	{
		// Memory access is a data memory access
		uint32_t num_dbats = (mmc & MMC_HIGH_BAT_ENABLE) ? CONFIG::NUM_BATS : 4;
		// Scan each DBAT register to find a translation
		for(bat_num = 0; bat_num < num_dbats; bat_num++)
		{
			// Check if DBAT register match
			if((bepi & 0x7800) == (GetDBATU_BEPI(bat_num) & 0x7800) // compare the 4 most significative bits of
				// the 15 most significative bits in EA and 15-bit DBAT BEPI field
						 && (bepi & ~GetDBATU_BL(bat_num) & 0x7ff) == (GetDBATU_BEPI(bat_num) & 0x7ff)) // compare the 11 least significative
				// bits of the 15 most significative bits in EA and DBAT BEPI field according to the DBAT block size mask (BL)
			{
				if((GetDBATU_VP(bat_num) && (privilege_level & PR_USER)) || (GetDBATU_VS(bat_num) && (privilege_level & PR_SUPERVISOR)))
				{
					// A DBAT match occurs
					protection_boundary = ((GetDBATU_BEPI(bat_num) & 0x7ffUL) + GetDBATU_BL(bat_num)) << 17;
					wimg = (WIMG) GetDBATL_WIMG(bat_num);
	
					// Check access rights
					uint32_t pp = GetDBATL_PP(bat_num);
					if(!pp || // PP=00: No access
					  ((pp & 1) && (memory_access_type == MAT_WRITE))) // PP=x1: Read-only
					{
						if(!(mmc & MMC_DEBUG_ENABLE))
						{
							cerr << "BAT Hit" << endl;
							throw DSIProtectionViolationException<CONFIG>(addr, memory_access_type);
						}
					}
					// Access is allowed
					
					// Compute the physical address
					return ((((bepi & 0x7ff) & GetDBATU_BL(bat_num)) | (GetDBATL_BRPN(bat_num) & 0x7ff) | (GetDBATL_BRPN(bat_num) & 0x7800)) << 17) | (addr & 0x1ffff);
				}
			}
		}
	}
	// Miss in BAT registers

	// Compute the protection boundary
	protection_boundary = (addr + (MEMORY_PAGE_SIZE)) & ~(MEMORY_PAGE_SIZE - 1);

	//-------------------------------
	//- Compute the virtual address -
	//-------------------------------

	// Get the segment register number from the most significative bits in the effective address
	sr_num = (addr >> 28) & 0xf;
	
	// Get the No-execute bit from the segment register
	sr_noexecute = GetSR_N(sr_num);

	if((memory_type == MT_INSN) && sr_noexecute)
	{
		// Raise an exception because the program is trying to execute instructions within a No-execute segment
		if(!(mmc & MMC_DEBUG_ENABLE)) throw ISINoExecuteException<CONFIG>(addr);
	}
	
	if(GetSR_T(sr_num) == 1)
	{
		// Raise an exception because the program is trying to access to a direct-store segment (not supported by PPC750)
		if(memory_type == MT_INSN)
		{
			if(!(mmc & MMC_DEBUG_ENABLE)) throw ISIDirectStoreException<CONFIG>(addr);
		}
		else
		{
			if(!(mmc & MMC_DEBUG_ENABLE)) throw DSIDirectStoreException<CONFIG>(addr, memory_access_type);
		}
	}
	
	// Get the key bits from the segment register
	sr_ks = GetSR_KS(sr_num);
	sr_kp = GetSR_KP(sr_num);
	key = (sr_kp && (privilege_level & PR_USER)) || (sr_ks && (privilege_level & PR_SUPERVISOR));
	
	// Get the virtual segment id from the segment register
	virtual_segment_id = GetSR_VSID(sr_num);
	
	// Compute the virtual address from the virtual segment id and the least significant bits of the effective address
	virtual_addr = ((virtual_address_t) virtual_segment_id << 28) | (virtual_address_t)(addr & 0x0fffffffUL);
	
	//------------------------------
	//-    Search in ITLB/DTLB     -
	//------------------------------
	if(memory_type == MT_INSN)
	{
		if(CONFIG::ITLB_ENABLE)
		{
			// Memory access is an instruction fetch
			// Decode the address to do a lookup into the ITLB
			itlb.DecodeAddress(virtual_addr, base_virtual_addr, tlb_index);
			//goto no_tlb;
			// search for the page table entry in the set
			pte = itlb.SearchEntry(tlb_index, base_virtual_addr);
	
			if(pte)
			{
				// ITLB Hit
				// return WIMG bits
				wimg = pte->wimg;
				
				if(wimg & WIMG_GUARDED_MEMORY)
				{
					if(!(mmc & MMC_DEBUG_ENABLE)) throw ISIGuardedMemoryException<CONFIG>(addr);
				}
	
				// Check access rights
				if(key && !pte->pp)
				{
					if(!(mmc & MMC_DEBUG_ENABLE)) throw ISIProtectionViolationException<CONFIG>(addr);
				}
	
				// Update the replacement policy
				if(!(mmc & MMC_DEBUG_ENABLE)) itlb.UpdateReplacementPolicy(pte, tlb_index);
				
				// compute the physical address
				return pte->base_physical_addr | (addr & 0x00000fffUL);
			}
		
			// Choose a page table entry to replace
			if(!pte) pte = itlb.ChooseEntryToReplace(tlb_index);
		}
		else
		{
			base_virtual_addr = virtual_addr & ~((virtual_address_t) MEMORY_PAGE_SIZE - 1);
		}
	}
	else
	{
		if(CONFIG::DTLB_ENABLE)
		{
			// Memory access is a data memory access
			// Decode the address to do a lookup into the DTLB
			dtlb.DecodeAddress(virtual_addr, base_virtual_addr, tlb_index);
			// Search the page table entry in the set
			pte = dtlb.SearchEntry(tlb_index, base_virtual_addr);
	
			if(pte)
			{
				// DTLB Hit
				// return WIMG bits
				wimg = pte->wimg;
	
				// Check access rights
				if((key && (!pte->pp || ((pte->pp & 1) && (memory_access_type == MAT_WRITE)))) || (!key && (pte->pp==3 && (memory_access_type == MAT_WRITE))))
				{
					// Raise a protection violation
					if(!(mmc & MMC_DEBUG_ENABLE))
					{
						throw DSIProtectionViolationException<CONFIG>(addr, memory_access_type);
					}
				}
				
				if(pte->c || memory_access_type != MAT_WRITE)
				{
					// Update the replacement policy
					if(!(mmc & MMC_DEBUG_ENABLE)) dtlb.UpdateReplacementPolicy(pte, tlb_index);
					
					// compute the physical address
					return pte->base_physical_addr | (addr & 0x00000fffUL);
				}
				
				// Force a page table walk if C=0 and it's a data write
			}
			
			// Choose a page table entry to replace if a TLB miss occured
			if(!pte) pte = dtlb.ChooseEntryToReplace(tlb_index);
		}
		else
		{
			base_virtual_addr = virtual_addr & ~((virtual_address_t) MEMORY_PAGE_SIZE - 1);
		}
	}
	// Miss in ITLB/DTLB
			
	// Compute the page index from the virtual address
	page_index = (base_virtual_addr >> 12) & 0xffff;

	// Compute API from the page index
	api = page_index >> 10;

	if(CONFIG::HAS_SOFTWARE_TABLE_SEARCH_SUPPORT)
	{
		if(!(mmc & MMC_DEBUG_ENABLE) && (mmc & MMC_SOFTWARE_TABLE_SEARCH_ENABLE))
		{
			// Software page table search is enabled
			
			/* primary PTEG address */
			hash = (virtual_segment_id & 0x7ffff) ^ page_index;
			pteg_select = (((GetSDR1_HTABORG() & 0x1ff) | (GetSDR1_HTABMASK() & ((hash >> 10) & 0x1ff))) << 10) | (hash & 0x3ff);
			physical_address_t primary_pteg = (((GetSDR1_HTABORG() >> 9) & 0x7f) << 25) | (pteg_select << 6);
				
			/* secondary PTEG address */
			hash = (~hash) & 0x7ffff;
			pteg_select = (((GetSDR1_HTABORG() & 0x1ff) | (GetSDR1_HTABMASK() & ((hash >> 10) & 0x1ff))) << 10) | (hash & 0x3ff);
			physical_address_t secondary_pteg = (((GetSDR1_HTABORG() >> 9) & 0x7f) << 25) | (pteg_select << 6);
			
			// raise a TLB miss
			throw TLBMissException<CONFIG>(
				memory_access_type,
				memory_type,
				addr,
				((memory_type == MT_INSN && CONFIG::ITLB_ENABLE) ||
				(memory_type == MT_DATA && CONFIG::DTLB_ENABLE)) ? pte->way : 0,
				(privilege_level & PR_USER) ? GetSR_KP(sr_num) : GetSR_KS(sr_num),
				virtual_segment_id,
				api,
				primary_pteg,
				secondary_pteg);
		}
	}
	
	//-------------------------------------
	//- Hardware search in the page table -
	//-------------------------------------

//	cerr << "api = 0x" << hex << api << endl;
//	cerr << "virtual_segment_id = 0x" << hex << virtual_segment_id << endl;

	// search with the primary hash key and if needed with the secondary hash key
	for(h = 0, hash = (virtual_segment_id & 0x7ffff) ^ page_index; h < 2; h++, hash = (~hash) & 0x7ffff)
	{
		// Compute the base address of a group of 8 page table entries
		pteg_select = (((GetSDR1_HTABORG() & 0x1ff) | (GetSDR1_HTABMASK() & ((hash >> 10) & 0x1ff))) << 10) | (hash & 0x3ff);
		pte_addr = (((GetSDR1_HTABORG() >> 9) & 0x7f) << 25) | (pteg_select << 6);

		// Scan each page table entry in the group
		for(pte_num = 0; pte_num < 8; pte_num++, pte_addr += 8)
		{
			uint64_t pte_value;
			uint32_t pte_virtual_segment_id;
			uint32_t pte_api;
			uint32_t pte_valid;
			uint32_t pte_h;
			uint32_t pte_pp;
			WIMG pte_wimg;
			uint32_t pte_c;
			physical_address_t base_physical_addr;


//			cerr << "pte_addr = 0x" << hex << pte_addr << dec << endl;
			// Read the page table entry from memory
			if((mmc & MMC_DEBUG_ENABLE))
			{
				if(to_dcache)
					to_dcache->ReadMemory(pte_addr, &pte_value, 8);
				else
					to_mem->ReadMemory(pte_addr, &pte_value, 8); // to_mem should be != 0 (enforced by ReadMemory/WriteMemory)
			}
			else
			{
				if(dcache_interface)
				{
					dcache_interface->PrRead(pte_addr, &pte_value, 8, CC_MEMORY_COHERENCY_ENFORCED);
				}
				else
				{
					CacheStatus cs;
					bus_interface->BusRead(pte_addr, &pte_value, 8, BC_GLOBAL, cs);
				}
			}

			pte_value = BigEndian2Host(pte_value);
//			cerr << "pte_value = 0x" << hex << pte_value << dec << endl;

			pte_valid = (pte_value >> 63) & 1;
//			cerr << "pte_valid = " << dec << pte_valid << endl;
			if(!pte_valid) // check valid bit
				continue;
			pte_h = (pte_value >> 38) & 1;
//			cerr << "pte_h = " << dec << pte_h << endl;
			if(pte_h != h) // good hash key ?
				continue;

			pte_virtual_segment_id = (pte_value >> 39) & 0xffffffUL;
//			cerr << "pte_virtual_segment_id = 0x" << hex << pte_virtual_segment_id << endl;
			pte_api = (pte_value >> 32) & 0x3f;
			
			if(pte_virtual_segment_id == virtual_segment_id && // compare VSIDs
						pte_api == api) // compare APIs
			{
				// Hit in page table
				pte_wimg = (WIMG)((pte_value >> 3) & 0xf);
				wimg = pte_wimg;
				
				// Check if access is guarded
				if((memory_type == MT_INSN) && (wimg & WIMG_GUARDED_MEMORY))
				{
					if(!(mmc & MMC_DEBUG_ENABLE)) throw ISIGuardedMemoryException<CONFIG>(addr);
				}

				pte_pp = (pte_value & 3);
				
				// Check access rights
				if((key && (!pte_pp || ((pte_pp & 1) && (memory_access_type == MAT_WRITE)))) || (!key && (pte_pp==3 && (memory_access_type == MAT_WRITE))))
				{
					// Raise a protection violation
					if(memory_type == MT_INSN)
					{
						if(!(mmc & MMC_DEBUG_ENABLE)) throw ISIProtectionViolationException<CONFIG>(addr);
					}
					else
					{
						if(!(mmc & MMC_DEBUG_ENABLE)) throw DSIProtectionViolationException<CONFIG>(addr, memory_access_type);
					}
				}
				
				// Update bit R and C in PTE
				pte_value |= 0x100ULL; // set accessed bit (R)
				if((memory_access_type == MAT_WRITE)) pte_value |= 0x80ULL; // set change bit (C)

				base_physical_addr = pte_value & 0xfffff000UL;
				//			cerr << "base_physical_addr = 0x" << hex << base_physical_addr << endl;
				
				// Build an access attribute for the page	
				pte_c = (pte_value >> 7) & 1;

				// Update the page table entry into memory
				pte_value = Host2BigEndian(pte_value);

				if(!(mmc & MMC_DEBUG_ENABLE))
				{
					if(dcache_interface)
					{
						dcache_interface->PrWrite(pte_addr, &pte_value, 8, CC_MEMORY_COHERENCY_ENFORCED);
					}
					else
					{
						bus_interface->BusWrite(pte_addr, &pte_value, 8, BC_GLOBAL);
					}
				}

				if((memory_type == MT_INSN && CONFIG::ITLB_ENABLE) ||
				   (memory_type == MT_DATA && CONFIG::DTLB_ENABLE))
				{
					// Refill ITLB/DTLB
					pte->valid = true;
					pte->base_virtual_addr = base_virtual_addr;
					pte->c = pte_c;
					pte->wimg = pte_wimg;
					pte->pp = pte_pp;
					pte->base_physical_addr = base_physical_addr;
					pte->pte_addr = pte_addr;
					
					if(!(mmc & MMC_DEBUG_ENABLE))
					{
						if(memory_type == MT_INSN)
						{
							// Refill the TLB entry
							itlb.Refill(pte, tlb_index);
	
							// Update the ITLB replacement policy
							itlb.UpdateReplacementPolicy(pte, tlb_index);
						}
						else
						{
							// Refill the TLB entry
							dtlb.Refill(pte, tlb_index);
	
							// Update the ITLB replacement policy
							dtlb.UpdateReplacementPolicy(pte, tlb_index);
						}
					}
				}
				
				// compute the physical address
				return base_physical_addr | (addr & 0x00000fffUL);
			}
		}
	}

	// Miss in the page table
	// Raise a page fault
	//DumpPageTable(cerr);
	//cerr << "================================================" << endl;
	
	if(memory_type == MT_INSN)
	{
		if(!(mmc & MMC_DEBUG_ENABLE))
		{
			throw ISIPageFaultException<CONFIG>(addr);
		}
	}
	else
	{
		if(!(mmc & MMC_DEBUG_ENABLE))
		{
			throw DSIPageFaultException<CONFIG>(addr, memory_access_type);
		}
	}
	return 0;
}

template <class CONFIG>
void MMU<CONFIG>::CheckTLBConsistency(MemoryType mt)
{
	if((mt == MT_INSN && CONFIG::ITLB_ENABLE) ||
	   (mt == MT_DATA && CONFIG::DTLB_ENABLE))
	{
		uint32_t tlb_index;
		uint32_t way;
		uint32_t h;
		uint32_t page_index;					// page index
		uint32_t api;							// Abreviated page index
		uint32_t hash;							// hash key
		uint32_t pteg_select;
		uint32_t virtual_segment_id;
		physical_address_t pte_addr;			// a page table entry address
		uint32_t pte_num;
		bool key;
		
		uint32_t tlb_sets = MT_INSN ? (CONFIG::NUM_ITLB_ENTRIES / CONFIG::ITLB_ASSOCIATIVITY) : (CONFIG::NUM_DTLB_ENTRIES / CONFIG::DTLB_ASSOCIATIVITY); 
		for(tlb_index = 0; tlb_index < tlb_sets; tlb_index++)
		{
			uint32_t tlb_associativity = MT_INSN ? CONFIG::ITLB_ASSOCIATIVITY : CONFIG::DTLB_ASSOCIATIVITY;
			for(way = 0; way < tlb_associativity; way++)
			{
				MemoryPageTableEntry<typename CONFIG::virtual_address_t, typename CONFIG::physical_address_t> *pte = mt == MT_INSN ? itlb.GetEntry(tlb_index, way) : dtlb.GetEntry(tlb_index, way);
				if(pte->valid)
				{
					bool hit = false;
					
					virtual_segment_id = pte->base_virtual_addr >> 28;
					// Compute the page index from the virtual address
					page_index = (pte->base_virtual_addr >> 12) & 0xffff;
					// Compute API from the page index
					api = page_index >> 10;
					
					//cerr << "VSID=0x" << std::hex << virtual_segment_id << ", page_index=0x" << page_index << std::dec << endl;
					// search with the primary hash key and if needed with the secondary hash key
					for(h = 0, hash = (virtual_segment_id & 0x7ffff) ^ page_index; h < 2 && !hit; h++, hash = (~hash) & 0x7ffff)
					{
						// Compute the base address of a group of 8 page table entries
						pteg_select = (((GetSDR1_HTABORG() & 0x1ff) | (GetSDR1_HTABMASK() & ((hash >> 10) & 0x1ff))) << 10) | (hash & 0x3ff);
						pte_addr = (((GetSDR1_HTABORG() >> 9) & 0x7f) << 25) | (pteg_select << 6);
		
						// Scan each page table entry in the group
						for(pte_num = 0; pte_num < 8 && !hit; pte_num++, pte_addr += 8)
						{
							uint64_t pte_value;
							uint32_t pte_virtual_segment_id;
							uint32_t pte_api;
							uint32_t pte_valid;
							uint32_t pte_h;
							uint32_t pte_pp;
							WIMG pte_wimg;
							uint32_t pte_c;
							physical_address_t base_physical_addr;
		
							if(to_dcache)
								to_dcache->ReadMemory(pte_addr, &pte_value, 8);
							else
								to_mem->ReadMemory(pte_addr, &pte_value, 8); // to_mem should be != 0 (enforced by ReadMemory/WriteMemory)
		
							pte_value = BigEndian2Host(pte_value);
		
							pte_valid = (pte_value >> 63) & 1;
							if(!pte_valid) // check valid bit
								continue;
							pte_h = (pte_value >> 38) & 1;
							if(pte_h != h) // good hash key ?
								continue;
		
							pte_virtual_segment_id = (pte_value >> 39) & 0xffffffUL;
							pte_api = (pte_value >> 32) & 0x3f;
					
							if(pte_virtual_segment_id == virtual_segment_id && // compare VSIDs
											pte_api == api) // compare APIs
							{
								// Hit in page table
								pte_wimg = (WIMG)((pte_value >> 3) & 0xf);
								pte_pp = (pte_value & 3);
								base_physical_addr = pte_value & 0xfffff000UL;
								pte_c = (pte_value >> 7) & 1;
								
								if(pte->base_physical_addr != base_physical_addr || pte->c != pte_c || pte->pp != pte_pp || pte->wimg != pte_wimg)
								{
									cerr << Object::GetName() << ": inconsistency detected" << endl;
									cerr << "base_physical_addr=0x" << std::hex << base_physical_addr << ", c=" << pte_c << ", pp=" << pte_pp << ", wimg=" << pte_wimg << std::dec << endl;
									cerr << *pte << endl;
								}
								
								hit = true;
							}
						}
					}
					if(!hit)
					{
						cerr << Object::GetName() << ": TLB Hit but page fault" << endl;
						cerr << *pte << endl;
					}
				}
			}
		}
	}
}


template <class CONFIG>
void MMU<CONFIG>::InvalidateITLB()
{
	if(CONFIG::ITLB_ENABLE)
	{
		itlb.Invalidate();
	}
}

template <class CONFIG>
void MMU<CONFIG>::InvalidateDTLB()
{
	if(CONFIG::DTLB_ENABLE)
	{
		dtlb.Invalidate();
	}
}

template <class CONFIG>
void MMU<CONFIG>::InvalidateITLBEntry(address_t addr)
{
	if(CONFIG::ITLB_ENABLE)
	{
		uint32_t sr_num;				/* the segment register number */
		uint32_t virtual_segment_id;	/* the virtual segment id */
		uint64_t virtual_addr;			/* the 52-bit virtual address */
		
		/* get the segment register number from the effective address */
		sr_num = (addr >> 28) & 0xf;
	
		/* read the VSID in the corresponding segment register */
		virtual_segment_id = GetSR_VSID(sr_num);
		
		/* build the virtual address from the VSID and the least significant bits of the effective address */
		virtual_addr = ((uint64_t) virtual_segment_id << 28) | (uint64_t)(addr & 0x0fffffffUL);
	
		itlb.InvalidateEntry(virtual_addr);
	}
}

template <class CONFIG>
void MMU<CONFIG>::InvalidateITLBSet(typename CONFIG::address_t addr)
{
	if(CONFIG::ITLB_ENABLE)
	{
		itlb.InvalidateSet((addr / MEMORY_PAGE_SIZE) & ((CONFIG::NUM_ITLB_ENTRIES / CONFIG::ITLB_ASSOCIATIVITY) - 1));
	}
}

template <class CONFIG>
void MMU<CONFIG>::InvalidateDTLBEntry(typename CONFIG::address_t addr)
{
	if(CONFIG::DTLB_ENABLE)
	{
		uint32_t sr_num;				/* the segment register number */
		uint32_t virtual_segment_id;	/* the virtual segment id */
		uint64_t virtual_addr;			/* the 52-bit virtual address */
		
		/* get the segment register number from the effective address */
		sr_num = (addr >> 28) & 0xf;
	
		/* read the VSID in the corresponding segment register */
		virtual_segment_id = GetSR_VSID(sr_num);
		
		/* build the virtual address from the VSID and the least significant bits of the effective address */
		virtual_addr = ((uint64_t) virtual_segment_id << 28) | (uint64_t)(addr & 0x0fffffffUL);
	
		dtlb.InvalidateEntry(virtual_addr);
	}
}

template <class CONFIG>
void MMU<CONFIG>::InvalidateDTLBSet(typename CONFIG::address_t addr)
{
	if(CONFIG::DTLB_ENABLE)
	{
		dtlb.InvalidateSet((addr / MEMORY_PAGE_SIZE) & ((CONFIG::NUM_DTLB_ENTRIES / CONFIG::DTLB_ASSOCIATIVITY) - 1));
	}
}

template <class CONFIG>
void MMU<CONFIG>::LoadITLBEntry(typename CONFIG::address_t addr, uint32_t way, uint32_t pte_hi, uint32_t pte_lo)
{
	if(CONFIG::ITLB_ENABLE)
	{
		MemoryPageTableEntry<typename CONFIG::virtual_address_t, typename CONFIG::physical_address_t> *pte;
		uint32_t index = addr / MEMORY_PAGE_SIZE;
		uint32_t virtual_segment_id = (pte_hi >> 7) & 0xffffffUL;
		address_t base_physical_addr = pte_lo & ~(MEMORY_PAGE_SIZE - 1);
		virtual_address_t base_virtual_addr = ((virtual_address_t) virtual_segment_id << 28) | (virtual_address_t)(addr & 0x0fffffffUL & ~(MEMORY_PAGE_SIZE - 1));
		
		pte = itlb.GetEntry(index, way);
	
		pte->valid = ((pte_hi >> 31) & 1) ? true : false;
		pte->base_virtual_addr = base_virtual_addr;
		pte->c = (pte_lo >> 7) & 1;
		pte->wimg = (WIMG)((pte_lo >> 3) & 0xf);
		pte->pp = (pte_lo & 3);
		pte->base_physical_addr = base_physical_addr;
	}
}

template <class CONFIG>
void MMU<CONFIG>::LoadDTLBEntry(typename CONFIG::address_t addr, uint32_t way, uint32_t pte_hi, uint32_t pte_lo)
{
	if(CONFIG::DTLB_ENABLE)
	{
		MemoryPageTableEntry<typename CONFIG::virtual_address_t, typename CONFIG::physical_address_t> *pte;
		uint32_t index = addr / MEMORY_PAGE_SIZE;
		uint32_t virtual_segment_id = (pte_hi >> 7) & 0xffffffUL;
		address_t base_physical_addr = pte_lo & ~(MEMORY_PAGE_SIZE - 1);
		virtual_address_t base_virtual_addr = ((virtual_address_t) virtual_segment_id << 28) | (virtual_address_t)(addr & 0x0fffffffUL & ~(MEMORY_PAGE_SIZE - 1));
		
		pte = dtlb.GetEntry(index, way);
	
		pte->valid = ((pte_hi >> 31) & 1) ? true : false;
		pte->base_virtual_addr = base_virtual_addr;
		pte->c = (pte_lo >> 7) & 1;
		pte->wimg = (WIMG)((pte_lo >> 3) & 0xf); // this is an optimization
		pte->pp = (pte_lo & 3);
		pte->base_physical_addr = base_physical_addr;
	}
}

template <class CONFIG>
void MMU<CONFIG>::ZeroDataCacheBlock(typename CONFIG::address_t addr, PrivilegeLevel privilege_level)
{
	physical_address_t physical_addr;
	address_t protection_boundary;
	WIMG wimg;

	physical_addr = TranslateAddress(addr, privilege_level, MAT_WRITE, MT_DATA, protection_boundary, wimg);
	if(dcache_interface)
	{
		// The system has a data cache
		if((wimg & WIMG_CACHE_INHIBITED) || (wimg & WIMG_WRITE_THROUGH))
		{
			if(!(mmc & MMC_DEBUG_ENABLE)) throw AlignmentException<CONFIG>(addr);
		}

		CacheControl cc = (CacheControl)(wimg & 0xe); // This is an optimization
		dcache_interface->PrZeroBlock(physical_addr, cc);
	}
	else
	{
		// The system has no data cache
		BusControl bc = (wimg & WIMG_MEMORY_COHERENCY_ENFORCED) ? BC_GLOBAL : BC_NONE;
		bus_interface->BusZeroBlock(physical_addr, bc);
		
		// If there is no data cache, ensure that dcbz clear 32 bytes
		{
			uint8_t zeros[32];
			memset(zeros, 0, 32);
			bus_interface->BusWrite(physical_addr & 0xffffffe0, zeros, 32, bc);
		}

	}
}

template <class CONFIG>
void MMU<CONFIG>::FlushDataCacheBlock(typename CONFIG::address_t addr, PrivilegeLevel privilege_level)
{
	physical_address_t physical_addr;
	address_t protection_boundary;
	WIMG wimg;

	physical_addr = TranslateAddress(addr, privilege_level, MAT_READ, MT_DATA, protection_boundary, wimg);
	if(dcache_interface)
	{
		// The system has a data cache
		dcache_interface->PrFlushBlock(physical_addr, CC_NONE);
	}
	else
	{
		// The system has no data cache
		if(mmc & MMC_ADDRESS_BROADCAST_ENABLE)
			bus_interface->BusFlushBlock(physical_addr, BC_GLOBAL);
	}
}

template <class CONFIG>
void MMU<CONFIG>::InvalidateDataCacheBlock(typename CONFIG::address_t addr, PrivilegeLevel privilege_level)
{
	physical_address_t physical_addr;
	address_t protection_boundary;
	WIMG wimg;

	physical_addr = TranslateAddress(addr, privilege_level, MAT_WRITE, MT_DATA, protection_boundary, wimg);
	if(dcache_interface)
	{
		// The system has a data cache
		dcache_interface->PrInvalidateBlock(physical_addr, CC_NONE);
	}
	else
	{
		// The system has no data cache
		if(mmc & MMC_ADDRESS_BROADCAST_ENABLE)
			bus_interface->BusFlushBlock(physical_addr, BC_GLOBAL);
	}
}

template <class CONFIG>
void MMU<CONFIG>::InvalidateInsnCacheBlock(typename CONFIG::address_t addr, PrivilegeLevel privilege_level)
{
	physical_address_t physical_addr;
	address_t protection_boundary;
	WIMG wimg;

	// effective address is not translated
	if(icache_interface)
	{
		// The system has an instruction cache
		icache_interface->PrInvalidateBlock(addr, CC_NONE);
	}
	// icbi is not broadcasted on the bus
}

template <class CONFIG>
void MMU<CONFIG>::DumpPageTable(ostream& os)
{
	uint32_t h;
	uint32_t page_index;					// page index
	uint32_t api;							// Abreviated page index
	uint32_t hash[2];						// hash keys
	physical_address_t pte_addr;			// a page table entry address
	uint32_t pte_num;
	uint32_t mask = GetSDR1_HTABMASK();
	uint32_t hash_lo, hash_hi;
	physical_address_t pteg_select;
	
	os << "HTABORG = 0x" << hex << GetSDR1_HTABORG() << dec << endl;
	os << "HTABMASK = 0x" << hex << GetSDR1_HTABMASK() << dec << endl;
	
	
	for(hash_hi = 0; hash_hi <= mask; hash_hi++)
	{
		for(hash_lo = 0; hash_lo < 1024; hash_lo++)
		{
			hash[0] = (hash_hi << 10) | hash_lo;
			hash[1] = (~hash[0]) & 0x7ffff;
			
			for(h = 0; h < 2; h++)
			{
				pteg_select = (((GetSDR1_HTABORG() & 0x1ff) | (GetSDR1_HTABMASK() & ((hash[h] >> 10) & 0x1ff))) << 10) | (hash[h] & 0x3ff);
				pte_addr = (((GetSDR1_HTABORG() >> 9) & 0x7f) << 25) | (pteg_select << 6);
		
				for(pte_num = 0; pte_num < 8; pte_num++, pte_addr += 8)
				{
					uint64_t pte_value;
					virtual_address_t base_virtual_addr;
					uint32_t pte_valid, pte_h, pte_virtual_segment_id, pte_api, pte_pp, pte_r, pte_c, pte_wimg;
					uint32_t page_index, api;
					address_t base_paddr;
					CacheStatus cs;
					if(to_dcache)
						to_dcache->ReadMemory(pte_addr, &pte_value, 8);
					else
						to_mem->ReadMemory(pte_addr, &pte_value, 8); // to_mem should be != 0 (enforced by ReadMemory/WriteMemory)
		
					pte_value = BigEndian2Host(pte_value);
		
					pte_valid = (pte_value >> 63) & 1;
					if(!pte_valid) continue;
					pte_h = (pte_value >> 38) & 1;
					if(pte_h != h) continue;
					
					pte_virtual_segment_id = (pte_value >> 39) & 0xffffffUL;
					pte_api = (pte_value >> 32) & 0x3f;
					
					page_index = (pte_virtual_segment_id & 0x7ffff) ^ hash[0];
					api = page_index >> 10; 
					base_virtual_addr = (pte_virtual_segment_id << 28) | (page_index << 12);
					if(pte_valid && pte_api == api)
					{
						pte_r = (pte_value >> 8) & 1;
						pte_c = (pte_value >> 7) & 1;
						pte_pp = (pte_value & 3);
						pte_wimg = (pte_value >> 3) & 0xfUL;
						base_paddr = pte_value & 0xfffff000UL;
						
						os << "0x" << hex << pte_addr << ": base_virt_addr=0x" << base_virtual_addr << " hash=0x" << hash << " V=" << pte_valid << " VSID=0x" << pte_virtual_segment_id << " H=" << pte_h << " API=0x" << pte_api << " pp=" << pte_pp << " r=" << pte_r << " c=" << pte_c << " base_paddr=0x" << base_paddr << " wimg=" << pte_wimg << dec << endl;
					}
				}
			}
		}
	}
}

template <class CONFIG>
RegisterInterface *MMU<CONFIG>::GetRegister(const char *name)
{
	typename map<string, RegisterInterface *>::iterator reg_iter = registers_registry.find(name);
	return reg_iter != registers_registry.end() ? (*reg_iter).second : 0;
}


} // end of namespace powerpc
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif

