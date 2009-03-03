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

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_MMC_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_MMC_HH__

#include <inttypes.h>
#include <iostream>
#include <cmath>


#include "unisim/kernel/service/service.hh"
#include "unisim/service/interfaces/memory.hh"


#include <unisim/component/cxx/processor/hcs12x/config.hh>
#include <unisim/component/cxx/processor/hcs12x/types.hh>
#include <unisim/component/cxx/processor/hcs12x/exception.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace hcs12x {

using unisim::kernel::service::Object;
using unisim::kernel::service::Client;
using unisim::kernel::service::Service;
using unisim::kernel::service::ServiceExport;
using unisim::kernel::service::ServiceImport;
using unisim::kernel::service::Parameter;

using unisim::service::interfaces::Memory;

class MMC : public Service<Memory<service_address_t> >,
	public Client<Memory<service_address_t> >
{
public:

	enum MODE {SS = 0x00, ES = 0x20, ST = 0x40, EX = 0x60, NS = 0x80, NX = 0xA0};

	//==============================================================================
	//=   MEMORY MAP (Logical Memories Offsets) and RESET VALUES OF MMC REGISTERS  =
	//==============================================================================

	static const uint8_t DIRECT_ADDRESS_SIZE	= 8;	// Number of bits used by the CPU to address DIRECT (max=8)
	static const uint8_t RAM_ADDRESS_SIZE		= 12;	// Number of bits used by the CPU to address RAM (max=16)
	static const uint8_t EEPROM_ADDRESS_SIZE	= 10;	// Number of bits used by the CPU to address EEPROM (max=16)
	static const uint8_t FLASH_ADDRESS_SIZE		= 14;	// Number of bits used by the CPU to address FLASH (max=16)

	static const uint16_t DIRECT_PAGE_SIZE	= 0x100;
	static const uint16_t RAM_PAGE_SIZE		= 0x1000;
	static const uint16_t EEPROM_PAGE_SIZE	= 0x400;
	static const uint16_t FLASH_PAGE_SIZE	= 0x4000;

	static const address_t RAM_CPU_ADDRESS_BITS		= 0x0FFF;
	static const address_t EEPROM_CPU_ADDRESS_BITS	= 0x03FF;
	static const address_t FLASH_CPU_ADDRESS_BITS	= 0x3FFF;

	static const physical_address_t RAM_PHYSICAL_ADDRESS_FIXED_BITS		= 0x00000000;
	static const physical_address_t EEPROM_PHYSICAL_ADDRESS_FIXED_BITS	= 0x00100000;
	static const physical_address_t FLASH_PHYSICAL_ADDRESS_FIXED_BITS	= 0x00400000;

	static const address_t REG_LOW_OFFSET	= 0x0000;
	static const address_t REG_HIGH_OFFSET	= 0x07FF;
	static const address_t EEPROM_LOW_OFFSET= 0x0800;
	static const address_t EEPROM_HIGH_OFFSET=0x0FFF;
	static const address_t RAM_LOW_OFFSET	= 0x1000;
	static const address_t RAM_HIGH_OFFSET	= 0x3FFF;
	static const address_t FLASH_LOW_OFFSET	= 0x4000;
	static const address_t FLASH_HIGH_OFFSET=0xFFFF;

	static const address_t MMC_LOW_ADDRESS		= 0x000A;
	static const address_t MMC_HIGH_ADDRESS		= 0x011F;

	static const address_t MMCCTL0_REG_ADDRESS	= 0x000A;
	static const address_t MMC_MODE_REG_ADDRESS	= 0x000B;
	static const address_t GPAGE_REG_ADDRESS	= 0x0010;
	static const address_t DIRECT_REG_ADDRESS	= 0x0011;
	static const address_t MMCCTL1_REG_ADDRESS	= 0x0013;
	static const address_t RPAGE_REG_ADDRESS	= 0x0016;
	static const address_t EPAGE_REG_ADDRESS	= 0x0017;
	static const address_t PPAGE_REG_ADDRESS	= 0x0030;
	static const address_t RAMWPC_REG_ADDRESS	= 0x011C;
	static const address_t RAMXGU_REG_ADDRESS	= 0x011D;
	static const address_t RAMSHL_REG_ADDRESS	= 0x011E;
	static const address_t RAMSHU_REG_ADDRESS	= 0x011F;

	static const uint16_t REGISTERS_SPACE_SIZE	= 0x800;	// MCS12XDP512 has 2k-bytes of io-register space
															// mapped at low address of memory

	static const uint8_t GPAGE_LOW			= 0x00;		// low gpage register value
	static const uint8_t GPAGE_HIGH			= 0x7F;		// high gpage register value

	static const uint8_t RPAGE_LOW			= 0xF8;		// low rpage (ram page) register value
	static const uint8_t RPAGE_HIGH			= 0xFF;		// high rpage register value
	static const uint8_t EPAGE_LOW			= 0xFC;		// low epage (eeprom page) register value
	static const uint8_t EPAGE_HIGH			= 0xFF;		// high epage register value
	static const uint8_t PPAGE_LOW			= 0xE0;		// low ppage (flash page) register value
	static const uint8_t PPAGE_HIGH			= 0xFF;		// high ppage register value

	static const uint8_t MMCCTL0_RESET		= 0x00;
	// MODC=1, MODB=0, MODA=0 => Normal single-chip
	static const uint8_t MMC_MODE_RESET		= 0x80;

	static const uint8_t GLOBAL_RESET_PAGE	= GPAGE_LOW;// reset gpage register value
	static const uint8_t DIRECT_RESET_PAGE	= 0x00;		// reset direct register value
	// EROMON=EROMCTL=1	=> Enable the emulated flash or ROM in memoryMap
	// ROMHM=0  => The fixed page of Flash or ROM can be accessed in the lower half of the memory map.
	//             Accesses to $4000–$7FFF will be mapped to $7F_4000-$7F_7FFF in the global memory space
	// ROMHM=1  => Disables access to the Flash or ROM in the lower half of the memory map.
	//             These physical locations of the Flash or ROM can still be accessed through the program page window.
	//             Accesses to $4000–$7FFF will be mapped to $14_4000-$14_7FFF in the global memory space (external access).
	// ROMON=ROMCTL=1 => Enable the flash or ROM in memoryMap
	static const uint8_t MMCCTL1_RESET		= 0x05;

	static const uint8_t RAM_RESET_PAGE		= 0xFD;		// reset rpage register value
	static const uint8_t EEPROM_RESET_PAGE	= 0xFE;		// reset epage register value
	static const uint8_t FLASH_RESET_PAGE	= 0xFE;		// reset ppage register value

	static const uint8_t RAMWPC_RESET		= 0x00;
	static const uint8_t RAMXGU_RESET		= 0xFF;
	static const uint8_t RAMSHL_RESET		= 0xFF;
	static const uint8_t RAMSHU_RESET		= 0xFF;


	// ***************************************

	ServiceExport<Memory<service_address_t> > memory_export;
	ServiceImport<Memory<service_address_t> > internal_memory_import;
	ServiceImport<Memory<service_address_t> > external_memory_import;

    MMC(const char *name, Object *parent = 0);

    inline physical_address_t getPhysicalAddress(address_t logicalAddress, ADDRESS::MODE type, bool isGlobal, bool debugload = false, uint8_t debug_page = 0xFF);
	inline bool isPaged(address_t addr, page_t page, bool debugload);

	//=====================================================================
	//=                  Client/Service setup methods                     =
	//=====================================================================

	virtual bool Setup();
	virtual void OnDisconnect();
	virtual void Reset();


	//=====================================================================
	//=             memory interface methods                              =
	//=====================================================================

	virtual bool ReadMemory(service_address_t addr, void *buffer, uint32_t size);
	virtual bool WriteMemory(service_address_t addr, const void *buffer, uint32_t size);
	void SplitPagedAddress(physical_address_t paged_addr, page_t &page, address_t &cpu_address);

	//=====================================================================
	//=             Internal Registers Accessors                          =
	//=====================================================================

    inline uint8_t getMmcctl0 ();
    inline uint8_t getMode ();
    inline uint8_t getGpage ();
    inline uint8_t getDirect ();
    inline uint8_t getMmcctl1 ();
    inline uint8_t getRpage ();
    inline uint8_t getEpage ();
    inline uint8_t getPpage ();
    inline void setPpage(uint8_t page);
    inline uint8_t getRamwpc ();
    inline uint8_t getRamxgu ();
    inline uint8_t getRamshl ();
    inline uint8_t getRamshu ();

    inline physical_address_t getDirectAddress(uint8_t lowByte);
    inline physical_address_t getRamAddress(address_t logicalAddress, bool debugload, uint8_t debug_page);
    inline physical_address_t getEepromAddress(address_t logicalAddress, bool debugload, uint8_t debug_page);
    inline physical_address_t getFlashAddress(address_t logicalAddress, bool debugload, uint8_t debug_page);

    inline uint8_t read(address_t address);
    inline void write(address_t address, uint8_t val);

private:
	//=============================================
	//=            MMC REGISTERS                  =
	//=============================================

    uint8_t mmcctl0;
	uint8_t mode;
    uint8_t gpage;
    uint8_t direct;
	uint8_t mmcctl1;
    uint8_t rpage;
    uint8_t epage;
    uint8_t ppage;
	uint8_t ramwpc;
	uint8_t ramxgu;
	uint8_t ramshl;
	uint8_t ramshu;

	bool directSet;

	uint8_t mode_int;
	uint8_t mmcctl1_int;
	Parameter<uint8_t> param_mode;
	Parameter<uint8_t> param_mmcctl1;

	uint8_t	address_encoding;
	Parameter<uint8_t> param_address_encoding;

};

inline physical_address_t MMC::getPhysicalAddress(address_t logicalAddress, ADDRESS::MODE type, bool isGlobal, bool debugload, uint8_t debug_page) {

	uint8_t gShift;

	physical_address_t address = logicalAddress;

	if (type == ADDRESS::DIRECT) {
		address = getDirectAddress((uint8_t) logicalAddress);
		gShift = sizeof(address_t) * 8;
	}

	if (isGlobal) {
		address = ((physical_address_t) (getGpage() & 0x7F) << gShift) | address;
		return address;
	}

	if (type == ADDRESS::EXTENDED) {
		if (logicalAddress <= REG_HIGH_OFFSET) { // Access to registers
			address = logicalAddress;
		}

		if ((logicalAddress >= EEPROM_LOW_OFFSET) && (logicalAddress <= EEPROM_HIGH_OFFSET)) { // Access to EEPROM
			address = getEepromAddress(logicalAddress, debugload, debug_page);
		}

		if ((logicalAddress >= RAM_LOW_OFFSET) && (logicalAddress <= RAM_HIGH_OFFSET)) { // Access to RAM
			address = getRamAddress(logicalAddress, debugload, debug_page);
		}

		if (logicalAddress >= FLASH_LOW_OFFSET) { // Access to Flash
			address = getFlashAddress(logicalAddress, debugload, debug_page);
		}
	}

	return address;
}

inline uint8_t MMC::read(address_t address)
{
	switch (address) {
	case MMCCTL0_REG_ADDRESS: return mmcctl0;
	case MMC_MODE_REG_ADDRESS: return mode;
	case GPAGE_REG_ADDRESS: return gpage;
	case DIRECT_REG_ADDRESS: return direct;
	case MMCCTL1_REG_ADDRESS: return mmcctl1;
	case RPAGE_REG_ADDRESS: return rpage;
	case EPAGE_REG_ADDRESS: return epage;
	case PPAGE_REG_ADDRESS: return ppage;
	case RAMWPC_REG_ADDRESS: return ramwpc;
	case RAMXGU_REG_ADDRESS: return ramxgu;
	case RAMSHL_REG_ADDRESS: return ramshl;
	case RAMSHU_REG_ADDRESS: return ramshu;
	default: return 0;
	}
}

inline void MMC::write(address_t address, uint8_t val)
{
	switch (address) {
	case MMCCTL0_REG_ADDRESS: mmcctl0 = val; break;
	case MMC_MODE_REG_ADDRESS: mode = val; break;
	case GPAGE_REG_ADDRESS: gpage = val; break;
	case DIRECT_REG_ADDRESS: {
		if (!directSet) {
			direct = val;
			directSet = true;
		}
	} break;
	case MMCCTL1_REG_ADDRESS: mmcctl1 = val;
	case RPAGE_REG_ADDRESS: rpage = val;
	case EPAGE_REG_ADDRESS: epage = val;
	case PPAGE_REG_ADDRESS: ppage = val;
	case RAMWPC_REG_ADDRESS: ramwpc = val;
	case RAMXGU_REG_ADDRESS: ramxgu = val;
	case RAMSHL_REG_ADDRESS: ramshl = val;
	case RAMSHU_REG_ADDRESS: ramshu = val;
	}

}


inline bool MMC::isPaged(address_t addr, page_t page, bool debugload) {

	// EEPROM window
	if ((addr > 0x07FF) && (addr < 0x0C00)) {
		if (!debugload) {
			page = getEpage();
		}
		if (page != 0xFF) {
			return true;
		}
	}

	// RAM window
	if ((addr > 0x0FFF) && (addr < 0x2000)) {
		if (!debugload) {
			page = getRpage();
		}
		if ((page != 0xFE) && (page != 0xFF)) {
			return true;
		}
	}

	// FLASH window
	if ((addr > 0x7FFF) && (addr < 0xC000)) {
		if (!debugload) {
			page = getPpage();
		}
		if ((page != 0xFD) && (page != 0xFF)) {
			return true;
		}
	}

	return false;
}

inline uint8_t MMC::getMmcctl0 () { return mmcctl0; }

inline uint8_t MMC::getMode () { return mode; }

inline uint8_t MMC::getMmcctl1 () { return mmcctl1; }

inline uint8_t MMC::getRamwpc () { return ramwpc; }

inline uint8_t MMC::getRamxgu () { return ramxgu; }

inline uint8_t MMC::getRamshl () { return ramshl; }

inline uint8_t MMC::getRamshu () { return ramshu; }

inline uint8_t MMC::getGpage () { return gpage; }

inline uint8_t MMC::getDirect () { return direct; }
inline physical_address_t MMC::getDirectAddress(uint8_t lowByte) {

	uint8_t _direct = getDirect ();

	if (_direct != 0)
	{
		return (((address_t) getDirect()) << DIRECT_ADDRESS_SIZE) | ((address_t) lowByte & 0x00FF);
	} else
	{
		return lowByte;
	}

}

inline uint8_t MMC::getRpage () { return rpage; }
inline physical_address_t MMC::getRamAddress(address_t logicalAddress, bool debugload, uint8_t debug_page) {

	uint8_t _rpage;

	if (debugload && (debug_page != 0x00)) _rpage = debug_page; else  _rpage = getRpage();

	if ((_rpage > 0x00) && (_rpage < RPAGE_LOW)) {
		throw NonMaskableAccessErrorInterrupt(NonMaskableAccessErrorInterrupt::INVALIDE_RPAGE);
	}

	if (isPaged(logicalAddress, _rpage, debugload)) {
		return RAM_PHYSICAL_ADDRESS_FIXED_BITS | ((physical_address_t) _rpage << RAM_ADDRESS_SIZE) | ((address_t) RAM_CPU_ADDRESS_BITS & logicalAddress);
	} else {
		if ((logicalAddress > 0x0FFF) && (logicalAddress < 0x2000)) {
			if (_rpage == 0xFE) return (logicalAddress - 0x1000) + 0x2000;
			if (_rpage == 0xFF) return (logicalAddress - 0x1000) + 0x3000;
		}
	}

	return logicalAddress;
}

inline uint8_t MMC::getEpage () { return epage; }
inline physical_address_t MMC::getEepromAddress(address_t logicalAddress, bool debugload, uint8_t debug_page) {

	uint8_t _epage;

	if (debugload && (debug_page != 0x00)) _epage = debug_page; else _epage = getEpage();

	if ((_epage > 0) && (_epage < EPAGE_LOW)) {
		throw NonMaskableAccessErrorInterrupt(NonMaskableAccessErrorInterrupt::INVALIDE_EPAGE);
	}

	if (isPaged(logicalAddress, _epage, debugload)) {
		return EEPROM_PHYSICAL_ADDRESS_FIXED_BITS | ((physical_address_t) _epage << EEPROM_ADDRESS_SIZE) | ((address_t) EEPROM_CPU_ADDRESS_BITS & logicalAddress);
	} else {
		if ((logicalAddress > 0x07FF) && (logicalAddress < 0x0C00) && (epage == 0xFF))
			return (logicalAddress -0x0800) + 0x0C00;
	}

	return logicalAddress;
}

inline uint8_t MMC::getPpage () { return ppage; }
inline void MMC::setPpage(uint8_t page) { ppage = page; }

inline physical_address_t MMC::getFlashAddress(address_t logicalAddress, bool debugload, uint8_t debug_page) {

	static const uint8_t ROMHM_MASK = 0x02;

	uint8_t _ppage;

	if (debugload && (debug_page != 0x00)) _ppage = debug_page; else _ppage = getPpage();

	if (isPaged(logicalAddress, _ppage, debugload)) {
		return FLASH_PHYSICAL_ADDRESS_FIXED_BITS | ((physical_address_t) _ppage << FLASH_ADDRESS_SIZE) | ((address_t) FLASH_CPU_ADDRESS_BITS & logicalAddress);
	} else {
		if ((logicalAddress > 0x7FFF) && (logicalAddress < 0xC000)) {
			if (_ppage == 0xFD) return (logicalAddress - 0x8000) + 0x4000;
			if (_ppage == 0xFF) return (logicalAddress - 0x8000) + 0xC000;
		}
	}

// TODO: ne pas oublier d'intégrer le cas ci-dessous du ROMHM
/*
	if ((logicalAddress > 0x3FFF) && (logicalAddress < 0x8000)) {
		if ((getMmcctl1() & ROMHM_MASK) == 1) { // 0x4000-0x7FFF is mapped to 0x14_4000-0x14_7FFF (external access)
			return (physical_address_t) (0x14 << 16) | logicalAddress;
		} else { // 0x4000-0x7FFF is mapped to 0x7F_4000-0x7F_7FFF (page = 0xFD)
			_ppage = 0xFD;
		}
	}
*/

	return logicalAddress;

}

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_MMC_HH__


