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

#ifndef __UNISIM_COMPONENT_CXX_PCI_DEBUG_PCI_STUB_TCC__
#define __UNISIM_COMPONENT_CXX_PCI_DEBUG_PCI_STUB_TCC__

#include <sstream>

namespace unisim {
namespace component {
namespace cxx {
namespace pci {
namespace debug {

using namespace std;
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

using unisim::util::debug::Symbol;
	
template <class ADDRESS>
PCIStub<ADDRESS>::PCIStub(const char *name, Object *parent) :
	NetStub<ADDRESS>("localhost", 10000, false),
	Object(name, parent),
	Service<Memory<ADDRESS> >(name, parent),
	Service<MemoryAccessReporting<ADDRESS> >(name, parent),
	Client<Logger>(name, parent),
	Client<SymbolTableLookup<ADDRESS> >(name, parent),
	Client<Synchronizable>(name, parent),
	memory_export("memory_export", this),
	logger_import("logger-import", this),
	symbol_table_lookup_import("symbol-table-lookup-import", this),
	memory_access_reporting_export("memory-access-reporting-export", this),
	synchronizable_import("synchronizable-import", this),
	bytesize(0),
	storage(0),
	param_bytesize("bytesize", this, bytesize),
	pci_device_number(0),
	pci_bus_frequency(33),
	bus_frequency(0),
	initial_base_addr(0),
	breakpoint_registry(),
	watchpoint_registry(),

	// PCI configuration registers initialization
	pci_conf_device_id("pci_conf_device_id", "PCI config Device ID", 0x0, 0x0000), // TBD
	pci_conf_vendor_id("pci_conf_vendor_id", "PCI config Vendor ID", 0x0, 0x0000), // TBD
	pci_conf_status("pci_conf_status", "PCI Config Status", 0x0, 0x0),
	pci_conf_command("pci_conf_command", "PCI Config Command", 0x007f, 0x0),
	pci_conf_class_code("pci_conf_class_code", "PCI Config Class Code", 0x0, 0x0),
	pci_conf_revision_id("pci_conf_revision_id", "PCI Config Revision ID", 0x0, 0x0),
	pci_conf_bist("pci_conf_bist", "PCI Config BIST", 0x0, 0x0),
	pci_conf_header_type("pci_conf_header_type", "PCI Config Header Type", 0x0, 0x0),
	pci_conf_latency_timer("pci_conf_latency_timer", "PCI Config Latency Timer", 0x0, 0x0),
	pci_conf_cache_line_size("pci_conf_cache_line_size", "PCI Config Cache Line Size", 0x0, 0x0),

	pci_conf_carbus_cis_pointer("pci_conf_carbus_cis_pointer", "PCI Config Carbus CIS Pointer", 0x0, 0x0),
	pci_conf_subsystem_id("pci_conf_subsystem_id", "PCI Config Subsystem ID", 0x0, 0x0),
	pci_conf_subsystem_vendor_id("pci_conf_subsystem_vendor_id", "PCI Config Subsystem Vendor ID", 0x0, 0x0),

	// Parameters initialization
	param_server_name("server-name", this, inherited::server_name),
	param_tcp_port("tcp-port", this, inherited::tcp_port),
	param_is_server("is-server", this, inherited::is_server),
	param_initial_base_addr("initial-base-addr", this, initial_base_addr),
	param_pci_device_number("pci-device-number", this, pci_device_number),
	param_pci_bus_frequency("pci-bus-frequency", this, pci_bus_frequency),
	param_bus_frequency("bus-frequency", this, bus_frequency)
{
}

template <class ADDRESS>
bool PCIStub<ADDRESS>::Setup()
{
	// PCI configuration registers initialization	
	pci_conf_base_addr.Initialize("pci_conf_base_addr", "PCI Config Base Address", 0xfffffff0UL, initial_base_addr);

	pci_conf_command = pci_conf_command | 0x2; // active memory space

	storage = new uint8_t[bytesize];
	memset(storage, 0, bytesize);

	inherited::Initialize();
	
	return true;
}

template <class ADDRESS>
PCIStub<ADDRESS>::~PCIStub()
{
	if(storage)
	{
		delete[] storage;
		storage = 0;
	}
}

template <class ADDRESS>
void PCIStub<ADDRESS>::OnDisconnect()
{
}

template <class ADDRESS>
void PCIStub<ADDRESS>::Reset()
{
}

template <class ADDRESS>
bool PCIStub<ADDRESS>::WriteMemory(ADDRESS physical_addr, const void *buffer, uint32_t size)
{
	if(physical_addr < pci_conf_base_addr || (physical_addr + (size - 1)) >= (pci_conf_base_addr + bytesize) || (physical_addr + size) < physical_addr) return false;
	// the third condition is for testing overwrapping (gdb did it !)

	if(logger_import)
	{
		(*logger_import) << DebugInfo;
		(*logger_import) << "Writing " << size << " bytes at 0x" << Hex << physical_addr << Dec << Endl;
		(*logger_import) << EndDebugInfo;
	}
	memcpy(storage + physical_addr - pci_conf_base_addr, buffer, size);
	return true;
}

template <class ADDRESS>
bool PCIStub<ADDRESS>::ReadMemory(ADDRESS physical_addr, void *buffer, uint32_t size)
{
	if(physical_addr < pci_conf_base_addr || (physical_addr + (size - 1)) >= (pci_conf_base_addr + bytesize) || (physical_addr + size) < physical_addr) return false;
	// the third condition is for testing overwrapping (gdb did it !)
	
	if(logger_import)
	{
		(*logger_import) << DebugInfo;
		(*logger_import) << "Reading " << size << " bytes at 0x" << Hex << physical_addr << Dec << Endl;
		(*logger_import) << EndDebugInfo;
	}
	memcpy(buffer, storage + physical_addr - pci_conf_base_addr, size);
	return true;
}

template <class ADDRESS>
uint8_t PCIStub<ADDRESS>::ReadConfigByte(unsigned int offset)
{
	switch(offset)
	{
		case 0x00:
			return pci_conf_vendor_id;
			break;
		case 0x01:
			return pci_conf_vendor_id >> 8;
			break;
		case 0x02:
			return pci_conf_device_id;
			break;
		case 0x03:
			return pci_conf_device_id >> 8;
			break;
		case 0x04:
			return pci_conf_command;
			break;
		case 0x05:
			return pci_conf_command >> 8;
			break;
		case 0x06:
			return pci_conf_status;
			break;
		case 0x07:
			return pci_conf_status >> 8;
			break;
		case 0x08:
			return pci_conf_revision_id;
			break;
		case 0x09:
			return pci_conf_class_code;
			break;
		case 0x0a:
			return pci_conf_class_code >> 8;
			break;
		case 0x0b:
			return pci_conf_class_code >> 16;
			break;
		case 0x0c:
			return pci_conf_cache_line_size;
			break;
		case 0x0d:
			return pci_conf_latency_timer;
			break;
		case 0x0e:
			return pci_conf_header_type;
			break;
		case 0x0f:
			return pci_conf_bist;
			break;
		case 0x10:
			return pci_conf_base_addr;
			break;
		case 0x11:
			return pci_conf_base_addr >> 8;
			break;
		case 0x12:
			return pci_conf_base_addr >> 16;
			break;
		case 0x13:
			return pci_conf_base_addr >> 24;
			break;
		case 0x28:
			return 0xf1;
		case 0x29:
			return 0xff;
		case 0x2a:
			return 0xff;
		case 0x2b:
			return 0xff;
	}
	return 0x00;
}

template <class ADDRESS>
void PCIStub<ADDRESS>::WriteConfigByte(unsigned int offset, uint8_t value)
{
	switch(offset)
	{
		case 0x04:
			pci_conf_command = (pci_conf_command & 0xff00) | value;
			break;
		case 0x05:
			pci_conf_command = (pci_conf_command & 0x00ff) | ((uint16_t) value << 8);
			break;
		case 0x0f:
			pci_conf_bist = value;
			break;
		case 0x10:
			pci_conf_base_addr = ((pci_conf_base_addr & 0xffffff00UL) == 0xffffff00UL && value == 0xff) ?
				                     (~bytesize + 1) :
				(pci_conf_base_addr & 0xffffff00UL) | value;
			break;
		case 0x11:
			pci_conf_base_addr = ((pci_conf_base_addr & 0xffff00f0UL) == 0xffff00f0UL && value == 0xff) ?
				                     (~bytesize + 1) :
				(pci_conf_base_addr & 0xffff00f0UL) | ((uint16_t) value << 8);
			break;
		case 0x12:
			pci_conf_base_addr = ((pci_conf_base_addr & 0xff00fff0UL) == 0xff00fff0UL && value == 0xff) ?
				                     (~bytesize + 1) :
				(pci_conf_base_addr & 0xff00fff0UL) | ((uint16_t) value << 16);
			break;
		case 0x13:
			pci_conf_base_addr = ((pci_conf_base_addr & 0x00fffff0UL) == 0x00fffff0UL && value == 0xff) ?
				                     (~bytesize + 1) :
				(pci_conf_base_addr & 0x00fffff0UL) | ((uint16_t) value << 24);
			break;
	}
}

template <class ADDRESS>
bool PCIStub<ADDRESS>::Read(ADDRESS addr, void *buffer, uint32_t size)
{
	return ReadMemory(addr, buffer, size);
}

template <class ADDRESS>
bool PCIStub<ADDRESS>::Read(const char *symbol_name, void *buffer, uint32_t size)
{
	if(!symbol_table_lookup_import)
	{
		if(logger_import)
		{
			(*logger_import) << DebugWarning;
			(*logger_import) << "No symbol table connected. Can't lookup symbol \"" << symbol_name << "\"" << Endl;
			(*logger_import) << EndDebugWarning;
		}
		return false;
	}

	const Symbol<ADDRESS> *symbol = symbol_table_lookup_import->FindSymbolByName(symbol_name);

	if(!symbol)
	{
		if(logger_import)
		{
			(*logger_import) << DebugWarning;
			(*logger_import) << "Symbol \"" << symbol_name << "\" not found" << Endl;
			(*logger_import) << EndDebugWarning;
		}
		return false;
	}

	return ReadMemory(symbol->GetAddress(), buffer, size);
}

template <class ADDRESS>
bool PCIStub<ADDRESS>::Write(ADDRESS addr, const void *buffer, uint32_t size)
{
	return WriteMemory(addr, buffer, size);
}

template <class ADDRESS>
bool PCIStub<ADDRESS>::Write(const char *symbol_name, const void *buffer, uint32_t size)
{
	if(!symbol_table_lookup_import)
	{
		if(logger_import)
		{
			(*logger_import) << DebugWarning;
			(*logger_import) << "No symbol table connected. Can't lookup symbol \"" << symbol_name << "\"" << Endl;
			(*logger_import) << EndDebugWarning;
		}
		return false;
	}

	const Symbol<ADDRESS> *symbol = symbol_table_lookup_import->FindSymbolByName(symbol_name);

	if(!symbol)
	{
		if(logger_import)
		{
			(*logger_import) << DebugWarning;
			(*logger_import) << "Symbol \"" << symbol_name << "\" not found" << Endl;
			(*logger_import) << EndDebugWarning;
		}
		return false;
	}

	return WriteMemory(symbol->GetAddress(), buffer, size);
}

template <class ADDRESS>
void PCIStub<ADDRESS>::ReportMemoryAccess(typename MemoryAccessReporting<ADDRESS>::MemoryAccessType mat, typename MemoryAccessReporting<ADDRESS>::MemoryType mt, ADDRESS addr, uint32_t size)
{
	if(watchpoint_registry.HasWatchpoint(mat, mt, addr, size))
	{
		typename inherited::TIME_UNIT tu;
		uint64_t t;

		synchronizable_import->Synchronize();
		Trap(t, tu);
		NetStub<ADDRESS>::PutTrapPacket(t, tu);
	}
}

template <class ADDRESS>
void PCIStub<ADDRESS>::ReportFinishedInstruction(ADDRESS next_addr)
{
	if(breakpoint_registry.HasBreakpoint(next_addr))
	{
		typename inherited::TIME_UNIT tu;
		uint64_t t;

		synchronizable_import->Synchronize();
		Trap(t, tu);
		NetStub<ADDRESS>::PutTrapPacket(t, tu);
	}
}

template <class ADDRESS>
bool PCIStub<ADDRESS>::SetBreakpoint(ADDRESS addr)
{
	return breakpoint_registry.SetBreakpoint(addr);
}

template <class ADDRESS>
bool PCIStub<ADDRESS>::SetBreakpoint(const char *symbol_name)
{
	if(!symbol_table_lookup_import)
	{
		if(logger_import)
		{
			(*logger_import) << DebugWarning;
			(*logger_import) << "No symbol table connected. Can't lookup symbol \"" << symbol_name << "\"" << Endl;
			(*logger_import) << EndDebugWarning;
		}
		return false;
	}

	const Symbol<ADDRESS> *symbol = symbol_table_lookup_import->FindSymbolByName(symbol_name);

	if(!symbol)
	{
		if(logger_import)
		{
			(*logger_import) << DebugWarning;
			(*logger_import) << "Symbol \"" << symbol_name << "\" not found" << Endl;
			(*logger_import) << EndDebugWarning;
		}
		return false;
	}
	return breakpoint_registry.SetBreakpoint(symbol->GetAddress());
}

template <class ADDRESS>
bool PCIStub<ADDRESS>::RemoveBreakpoint(ADDRESS addr)
{
	return breakpoint_registry.RemoveBreakpoint(addr);
}

template <class ADDRESS>
bool PCIStub<ADDRESS>::RemoveBreakpoint(const char *symbol_name)
{
	if(!symbol_table_lookup_import)
	{
		if(logger_import)
		{
			(*logger_import) << DebugWarning;
			(*logger_import) << "No symbol table connected. Can't lookup symbol \"" << symbol_name << "\"" << Endl;
			(*logger_import) << EndDebugWarning;
		}
		return false;
	}

	const Symbol<ADDRESS> *symbol = symbol_table_lookup_import->FindSymbolByName(symbol_name);

	if(!symbol)
	{
		if(logger_import)
		{
			(*logger_import) << DebugWarning;
			(*logger_import) << "Symbol \"" << symbol_name << "\" not found" << Endl;
			(*logger_import) << EndDebugWarning;
		}
		return false;
	}

	return breakpoint_registry.RemoveBreakpoint(symbol->GetAddress());
}

template <class ADDRESS>
void PCIStub<ADDRESS>::Trap(uint64_t& t, typename inherited::TIME_UNIT& tu)
{
	t = 0;
	tu = inherited::TU_MS;
}

} // end of namespace debug
} // end of namespace pci
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
