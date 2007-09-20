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
 * Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
 */
 
#include "unisim/kernel/service/service.hh"
#include "unisim/service/interfaces/logger.hh"
#include "unisim/component/cxx/chipset/mpc107/config_regs.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace chipset {
namespace mpc107 {

using unisim::service::interfaces::Logger;
using unisim::service::interfaces::operator<<;
using unisim::service::interfaces::Hex;
using unisim::service::interfaces::Dec;
using unisim::service::interfaces::Endl;
using unisim::service::interfaces::DebugInfo;
using unisim::service::interfaces::DebugWarning;
using unisim::service::interfaces::DebugError;
using unisim::service::interfaces::EndDebugInfo;
using unisim::service::interfaces::EndDebugWarning;
using unisim::service::interfaces::EndDebugError;
using unisim::service::interfaces::Function;
using unisim::service::interfaces::File;
using unisim::service::interfaces::Line;

#define LOCATION Function << __FUNCTION__ << File << __FILE__ << Line << __LINE__ 

template <bool DEBUG>
AddressMap<DEBUG>::AddressMap(ConfigurationRegisters &_config_regs,
	const char *name, Object *parent) :
	Object(name, parent),
	Client<Logger>(name, parent),
	logger_import("logger_import", this), 
	proc_list(NULL), pci_list(NULL),
	config_regs(&_config_regs) {
}

template <bool DEBUG>
AddressMap<DEBUG>::~AddressMap() {
	DeleteEntryList(&proc_list);
	DeleteEntryList(&pci_list);
}

template <bool DEBUG>
bool
AddressMap<DEBUG>::Setup() {
	return true;
}

template <bool DEBUG>
void
AddressMap<DEBUG>::OnDisconnect() {
}

template <bool DEBUG>
void 
AddressMap<DEBUG>::AddEntryNode(AddressMapEntryNode **list, AddressMapEntryNode *node) {
	if(*list == NULL) {
		*list = node;
		node->prev = NULL;
		node->next = NULL;
	} else {
		node->prev = NULL;
		(*list)->prev = node;
		node->next = *list;
		*list = node;
	}
}

template <bool DEBUG>
void
AddressMap<DEBUG>::CreateAddressMapEntry(AddressMapEntryNode **list,
								const char *name,
								address_t orig,
								address_t end,
								AddressMapEntry::Type type) {
	AddressMapEntry *entry;
	AddressMapEntryNode *node;

	entry = new AddressMapEntry(name,
								orig,
								end,
								type);
	node = new AddressMapEntryNode(entry);
	AddEntryNode(list, node);
}

template <bool DEBUG>
bool
AddressMap<DEBUG>::SetEmbeddedUtilities() {
	uint32_t eu_addr = 0;

	/* getting base address */
	eu_addr = config_regs->eumbbar.value;

	if(DEBUG && logger_import) 
		(*logger_import) << DebugInfo
			<< LOCATION
			<< "Setting the embedded utilities at address 0x"
			<< Hex << eu_addr << Dec 
			<< Endl
			<< EndDebugInfo;

	CreateAddressMapEntry(&proc_list,
		"EUMB message unit space",
		eu_addr,
		eu_addr + (uint32_t)0x0fff,
		AddressMapEntry::EUMB_MSG_UNIT_SPACE);
	CreateAddressMapEntry(&proc_list,
		"EUMB DMA space",
		eu_addr + (uint32_t)0x01000,
		eu_addr + (uint32_t)0x01fff,
		AddressMapEntry::EUMB_DMA_SPACE);
	CreateAddressMapEntry(&proc_list,
		"EUMB ATU space",
		eu_addr + (uint32_t)0x02000,
		eu_addr + (uint32_t)0x02fff,
		AddressMapEntry::EUMB_ATU_SPACE);
	CreateAddressMapEntry(&proc_list,
		"EUMB I2C space",
		eu_addr + (uint32_t)0x03000,
		eu_addr + (uint32_t)0x03fff,
		AddressMapEntry::EUMB_I2C_SPACE);
	CreateAddressMapEntry(&proc_list,
		"Reserved",
		eu_addr + (uint32_t)0x04000,
		eu_addr + (uint32_t)0x03ffff,
		AddressMapEntry::RESERVED);
	CreateAddressMapEntry(&proc_list,
		"EUMB EPIC space",
		eu_addr + (uint32_t)0x040000,
		eu_addr + (uint32_t)0x07ffff,
		AddressMapEntry::EUMB_EPIC_SPACE);
	CreateAddressMapEntry(&proc_list,
		"Reserved",
		eu_addr + (uint32_t)0x080000,
		eu_addr + (uint32_t)0x0fefff,
		AddressMapEntry::RESERVED);
	CreateAddressMapEntry(&proc_list,
		"EUMB data path diagnostics space",
		eu_addr + (uint32_t)0x0ff000,
		eu_addr + (uint32_t)0x0ff047,
		AddressMapEntry::EUMB_DATA_PATH_DIAGNOSTICS_SPACE);
	CreateAddressMapEntry(&proc_list,
		"Reserved",
		eu_addr + (uint32_t)0x0ff048,
		eu_addr + (uint32_t)0x0fffff,
		AddressMapEntry::RESERVED);
	
	CreateAddressMapEntry(&pci_list,
		"EUMB message unit space",
		eu_addr,
		eu_addr + (uint32_t)0x0fff,
		AddressMapEntry::EUMB_MSG_UNIT_SPACE);
	CreateAddressMapEntry(&pci_list,
		"EUMB DMA space",
		eu_addr + (uint32_t)0x01000,
		eu_addr + (uint32_t)0x01fff,
		AddressMapEntry::EUMB_DMA_SPACE);
	CreateAddressMapEntry(&pci_list,
		"EUMB ATU space",
		eu_addr + (uint32_t)0x02000,
		eu_addr + (uint32_t)0x02fff,
		AddressMapEntry::EUMB_ATU_SPACE);
	CreateAddressMapEntry(&pci_list,
		"EUMB I2C space",
		eu_addr + (uint32_t)0x03000,
		eu_addr + (uint32_t)0x03fff,
		AddressMapEntry::EUMB_I2C_SPACE);
	CreateAddressMapEntry(&pci_list,
		"Reserved",
		eu_addr + (uint32_t)0x04000,
		eu_addr + (uint32_t)0x03ffff,
		AddressMapEntry::RESERVED);
	CreateAddressMapEntry(&pci_list,
		"EUMB EPIC space",
		eu_addr + (uint32_t)0x040000,
		eu_addr + (uint32_t)0x07ffff,
		AddressMapEntry::EUMB_EPIC_SPACE);
	CreateAddressMapEntry(&pci_list,
		"Reserved",
		eu_addr + (uint32_t)0x080000,
		eu_addr + (uint32_t)0x0fefff,
		AddressMapEntry::RESERVED);
	CreateAddressMapEntry(&pci_list,
		"EUMB data path diagnostics space",
		eu_addr + (uint32_t)0x0ff000,
		eu_addr + (uint32_t)0x0ff047,
		AddressMapEntry::EUMB_DATA_PATH_DIAGNOSTICS_SPACE);
	CreateAddressMapEntry(&pci_list,
		"Reserved",
		eu_addr + (uint32_t)0x0ff048,
		eu_addr + (uint32_t)0x0fffff,
		AddressMapEntry::RESERVED);

	return true;
}

template <bool DEBUG>
bool 
AddressMap<DEBUG>::SetAddressMapA() {
	if(logger_import) {
		(*logger_import) << DebugError
			<< LOCATION
			<< "Address map A not implemented" 
			<< Endl
			<< EndDebugError;
	}
	return false;
}

template <bool DEBUG>
bool 
AddressMap<DEBUG>::SetAddressMapB() {
	uint32_t val;
	uint32_t eu_addr = 0;
	bool host_mode;

	/* checking the host mode */	
	host_mode = ((config_regs->pci_command_reg.value >> 2) & (uint32_t)1) == (uint32_t)1;
	/* checking if the embedded utilities are activated */
	val = config_regs->eumbbar.value;
	if((val >= 0x80000000) && (val <= 0xfdf00000))
		eu_addr = val;
		
	if(!host_mode) {
		if(logger_import) {
			(*logger_import) << DebugError
				<< LOCATION
				<< "Address map B only supported in host mode" 
				<< Endl
				<< EndDebugError;
		}
		return false;
	}
	
	if(DEBUG && logger_import) 
		(*logger_import) << DebugInfo
			<< LOCATION
			<< "Setting address map B" 
			<< Endl
			<< EndDebugInfo;

	/* Create the processor view in host mode */
	CreateAddressMapEntry(&proc_list,
						"Local memory space",
						0,
						0x3fffffff,
						AddressMapEntry::LOCAL_MEMORY_SPACE);
	CreateAddressMapEntry(&proc_list,
						"Reserved",
						0x40000000,
						0x77ffffff,
						AddressMapEntry::RESERVED);
	CreateAddressMapEntry(&proc_list,
						"64-bit extended ROM/Flash (64 Mbytes) [1]",
						0x78000000,
						0x7bffffff,
						AddressMapEntry::EXTENDED_ROM_1);
	CreateAddressMapEntry(&proc_list,
						"64-bit extended ROM/Flash (64 Mbytes) [2]",
						0x7c000000,
						0x7fffffff,
						AddressMapEntry::EXTENDED_ROM_2);
	if(eu_addr == 0)
		/* the embedded utilities are not activated */
		CreateAddressMapEntry(&proc_list,
			"PCI memory space",
			0x80000000,
			0xfdffffff,
			AddressMapEntry::PCI_MEMORY_SPACE);
	else {
		/* the embedded utilities are activated */
		if(eu_addr > (uint32_t)0x80000000)
			CreateAddressMapEntry(&proc_list,
				"PCI memory space",
				(uint32_t)0x80000000,
				eu_addr - 1,
				AddressMapEntry::PCI_MEMORY_SPACE);
		if(eu_addr < (uint32_t)0xfdf00000)
			CreateAddressMapEntry(&proc_list,
				"PCI memory space",
				eu_addr + (uint32_t)0x00100000,
				0xfdffffff,
				AddressMapEntry::PCI_MEMORY_SPACE);
	}
	CreateAddressMapEntry(&proc_list,
						"PCI I/O space (8 Mbytes) [1]",
						0xfe000000,
						0xfe7fffff,
						AddressMapEntry::PCI_IO_SPACE_1);
	CreateAddressMapEntry(&proc_list,
						"PCI I/O space (4 Mbytes) [2]",
						0xfe800000,
						0xfebfffff,
						AddressMapEntry::PCI_IO_SPACE_2);
	CreateAddressMapEntry(&proc_list,
						"PCI configuration address register",
						0xfec00000,
						0xfedfffff,
						AddressMapEntry::PCI_CONFIG_ADDRESS_REG);
	CreateAddressMapEntry(&proc_list,
						"PCI configuration data register",
						0xfee00000,
						0xfeefffff,
						AddressMapEntry::PCI_CONFIG_DATA_REG);
	CreateAddressMapEntry(&proc_list,
						"PCI interrupt acknowledge",
						0xfef00000,
						0xfeffffff,
						AddressMapEntry::PCI_INT_ACK);
	CreateAddressMapEntry(&proc_list,
						"8-, 32- or 64-bit Flash/ROM space (8 Mbytes) [1]",
						0xff000000,
						0xff7fffff,
						AddressMapEntry::ROM_SPACE_1);
	CreateAddressMapEntry(&proc_list,
						"8-, 32- or 64-bit Flash/ROM space (8 Mbytes) [2]",
						0xff800000,
						0xffffffff,
						AddressMapEntry::ROM_SPACE_2);
												
	/* Create the PCI view in host mode */
	CreateAddressMapEntry(&pci_list,
						"Local memory space",
						0x00000000,
						0x3fffffff,
						AddressMapEntry::LOCAL_MEMORY_SPACE);
	CreateAddressMapEntry(&pci_list,
						"Reserved",
						0x40000000,
						0x77ffffff,
						AddressMapEntry::RESERVED);
	CreateAddressMapEntry(&pci_list,
						"64-bit extended ROM/Flash (64 Mbytes) [1]",
						0x78000000,
						0x7bffffff,
						AddressMapEntry::EXTENDED_ROM_1);
	CreateAddressMapEntry(&pci_list,
						"64-bit extended ROM/Flash (64 Mbytes) [2]",
						0x7c000000,
						0x7fffffff,
						AddressMapEntry::EXTENDED_ROM_2);
	if(eu_addr == 0)
		/* the embedded utilities are not activated */
		CreateAddressMapEntry(&pci_list,
			"PCI memory space",
			0x80000000,
			0xfdffffff,
			AddressMapEntry::PCI_MEMORY_SPACE);
	else {
		/* the embedded utilities are activated */
		if(eu_addr > (uint32_t)0x80000000)
			CreateAddressMapEntry(&pci_list,
				"PCI memory space",
				(uint32_t)0x80000000,
				eu_addr - 1,
				AddressMapEntry::PCI_MEMORY_SPACE);
		if(eu_addr < (uint32_t)0xfdf00000)
			CreateAddressMapEntry(&pci_list,
				"PCI memory space",
				eu_addr + (uint32_t)0x00100000,
				0xfdffffff,
				AddressMapEntry::PCI_MEMORY_SPACE);
	}
	CreateAddressMapEntry(&pci_list,
						"Reserved",
						0xfe000000,
						0xfeffffff,
						AddressMapEntry::RESERVED);
	CreateAddressMapEntry(&pci_list,
						"8-, 32- or 64-bit Flash/ROM space (8 Mbytes), read-only area (writes causes Flash write error) [1]",
						0xff000000,
						0xff7fffff,
						AddressMapEntry::ROM_SPACE_1);
	CreateAddressMapEntry(&pci_list,
						"8-, 32- or 64-bit Flash/ROM space (8 Mbytes), read-only area (writes causes Flash write error) [2]",
						0xff800000,
						0xffffffff,
						AddressMapEntry::ROM_SPACE_1);

	if(eu_addr != 0)
		SetEmbeddedUtilities();
						
	return true;
}

template <bool DEBUG>
AddressMapEntry *
AddressMap<DEBUG>::GetEntry(AddressMapEntryNode *list, address_t addr, const char *list_name) {
	/* this condition is only needed for debugging,
	 *   it should be removed later */
	if(list == NULL) {
		if(logger_import)
			(*logger_import) << DebugError
				<< LOCATION
				<< "Something went wrong, the " << list_name
				<< " address map doesn't exist"
				<< Endl
				<< EndDebugError;
		return NULL;
	}

	AddressMapEntryNode *entry;
	/* the test could be replaced to remove the test for entry,
	 *   because all the address range should be covered by the 
	 *   address table */
	for(entry = list;
		(entry != NULL) && ((entry->entry->orig > addr) || entry->entry->end < addr); 
		entry = entry->next);
	if(entry == NULL) {
		if(logger_import)
			(*logger_import) << DebugError
				<< LOCATION
				<< "Something went wrong, address 0x" << Hex << addr << Dec 
				<< " was not find in the " << list_name
				<< " address table"
				<< Endl
				<< EndDebugError;
		return NULL;
	}
	
	/* replace the entry to the head of the list,
	 *   this should enhance future searches */
	if(entry != list) {
		entry->prev->next = entry->next;
		if(entry->next != NULL) {
			entry->next->prev = entry->prev;
		}
		entry->next = proc_list;
		entry->prev = NULL;
		proc_list->prev = entry;
		proc_list = entry;
	}

	return entry->entry;
}

template <bool DEBUG>
AddressMapEntry *
AddressMap<DEBUG>::GetEntryProc(address_t addr) {
	return GetEntry(proc_list, addr, "processor");
}

template <bool DEBUG>
AddressMapEntry *
AddressMap<DEBUG>::GetEntryPCI(address_t addr) {
	return GetEntry(pci_list, addr, "pci");
}

template <bool DEBUG>
bool
AddressMap<DEBUG>::DeleteEntryList(AddressMapEntryNode **list) {
	while(*list) {
		AddressMapEntryNode *node = *list;
		AddressMapEntry *entry = node->entry;
		*list  = node->next;
		delete entry;
		delete node;
	}
	*list = NULL;
}

template <bool DEBUG>
bool
AddressMap<DEBUG>::Reset() {
	/* first the proc and pci list must be resetted */
	DeleteEntryList(&proc_list);
	DeleteEntryList(&pci_list);
	
	/* check which address map must be set up */
	bool a_address_map;
	a_address_map = (config_regs->picr1.value & (((uint32_t)1) << 16)) != 0;
	if(a_address_map)
		return SetAddressMapA();
	else
		return SetAddressMapB();
}

#undef LOCATION

} // end of namespace mpc107
} // end of namespace chipset
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim
