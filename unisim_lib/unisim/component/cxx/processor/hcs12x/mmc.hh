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

#include <unisim/component/cxx/processor/hcs12x/config.hh>
#include <unisim/component/cxx/processor/hcs12x/types.hh>
#include <unisim/component/cxx/processor/hcs12x/exception.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace hcs12x {

class MEMORY {
public:

	enum MAP {DIRECT=0, EXTENDED=1};
};

class MMC
{
public:

    MMC();
    void reset();

    inline physical_address_t getPhysicalAddress(address_t logicalAddress, MEMORY::MAP type, bool isGlobal);

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
    inline physical_address_t getRamAddress(address_t logicalAddress);
    inline physical_address_t getEepromAddress(address_t logicalAddress);
    inline physical_address_t getFlashAddress(address_t logicalAddress);

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

};

inline physical_address_t MMC::getPhysicalAddress(address_t logicalAddress, MEMORY::MAP type, bool isGlobal) {

	uint8_t gShift;

	physical_address_t address = logicalAddress;

	if (type == MEMORY::DIRECT) {
		address = getDirectAddress((uint8_t) logicalAddress);
		gShift = sizeof(address_t) * 8;
	}

	if (isGlobal) {
		address = ((physical_address_t) (getGpage() & 0x7F) << gShift) | address;
		return address;
	}

	if (type == MEMORY::EXTENDED) {
		if (logicalAddress <= CONFIG::REG_HIGH_OFFSET) { // Access to registers
			address = logicalAddress;
		}

		if ((logicalAddress >= CONFIG::EEPROM_LOW_OFFSET) && (logicalAddress <= CONFIG::EEPROM_HIGH_OFFSET)) { // Access to EEPROM
			address = getEepromAddress(logicalAddress);
		}

		if ((logicalAddress >= CONFIG::RAM_LOW_OFFSET) && (logicalAddress <= CONFIG::RAM_HIGH_OFFSET)) { // Access to RAM
			address = getRamAddress(logicalAddress);
		}

		if (logicalAddress >= CONFIG::FLASH_LOW_OFFSET) { // Access to Flash
			address = getFlashAddress(logicalAddress);
		}
	}

	return address;
}

inline uint8_t MMC::read(address_t address)
{
	switch (address) {
	case CONFIG::MMCCTL0_REG_ADDRESS: return mmcctl0;
	case CONFIG::MMC_MODE_REG_ADDRESS: return mode;
	case CONFIG::GPAGE_REG_ADDRESS: return gpage;
	case CONFIG::DIRECT_REG_ADDRESS: return direct;
	case CONFIG::MMCCTL1_REG_ADDRESS: return mmcctl1;
	case CONFIG::RPAGE_REG_ADDRESS: return rpage;
	case CONFIG::EPAGE_REG_ADDRESS: return epage;
	case CONFIG::PPAGE_REG_ADDRESS: return ppage;
	case CONFIG::RAMWPC_REG_ADDRESS: return ramwpc;
	case CONFIG::RAMXGU_REG_ADDRESS: return ramxgu;
	case CONFIG::RAMSHL_REG_ADDRESS: return ramshl;
	case CONFIG::RAMSHU_REG_ADDRESS: return ramshu;
	default: return 0;
	}
}

inline void MMC::write(address_t address, uint8_t val)
{
	switch (address) {
	case CONFIG::MMCCTL0_REG_ADDRESS: mmcctl0 = val; break;
	case CONFIG::MMC_MODE_REG_ADDRESS: mode = val; break;
	case CONFIG::GPAGE_REG_ADDRESS: gpage = val; break;
	case CONFIG::DIRECT_REG_ADDRESS: {
		if (!directSet) {
			direct = val;
			directSet = true;
		}
	} break;
	case CONFIG::MMCCTL1_REG_ADDRESS: mmcctl1 = val;
	case CONFIG::RPAGE_REG_ADDRESS: rpage = val;
	case CONFIG::EPAGE_REG_ADDRESS: epage = val;
	case CONFIG::PPAGE_REG_ADDRESS: ppage = val;
	case CONFIG::RAMWPC_REG_ADDRESS: ramwpc = val;
	case CONFIG::RAMXGU_REG_ADDRESS: ramxgu = val;
	case CONFIG::RAMSHL_REG_ADDRESS: ramshl = val;
	case CONFIG::RAMSHU_REG_ADDRESS: ramshu = val;
	}

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
		return (((address_t) getDirect()) << CONFIG::DIRECT_ADDRESS_SIZE) | ((address_t) lowByte & 0x00FF);
	} else
	{
		return lowByte;
	}

}

inline uint8_t MMC::getRpage () { return rpage; }
inline physical_address_t MMC::getRamAddress(address_t logicalAddress) {

	uint8_t _rpage = getRpage();

	if ((_rpage > 0x00) && (_rpage < CONFIG::RPAGE_LOW)) {
		throw NonMaskableAccessErrorInterrupt(NonMaskableAccessErrorInterrupt::INVALIDE_RPAGE);
	}

	if (((_rpage == 0x00) && (logicalAddress < 0x0800)) ||
		((_rpage == 0xFE) && (logicalAddress > 0x1FFF) && (logicalAddress < 0x3000)) ||
		((_rpage == 0xFF) && (logicalAddress > 0x2FFF) && (logicalAddress < 0x4000)) ||
		((logicalAddress > 0x0FFF) && (logicalAddress < 0x2000)))
	{
		return CONFIG::RAM_PHYSICAL_ADDRESS_FIXED_BITS | ((physical_address_t) getRpage() << CONFIG::RAM_ADDRESS_SIZE) | ((address_t) CONFIG::RAM_CPU_ADDRESS_BITS & logicalAddress);
	}
	else
	{
		return logicalAddress;
	}
}

inline uint8_t MMC::getEpage () { return epage; }
inline physical_address_t MMC::getEepromAddress(address_t logicalAddress) {

	uint8_t _epage = getEpage();

	if ((_epage > 0) && (_epage < CONFIG::EPAGE_LOW)) {
		throw NonMaskableAccessErrorInterrupt(NonMaskableAccessErrorInterrupt::INVALIDE_EPAGE);
	}

	if (((_epage == 0xFF) && (logicalAddress > 0x0BFF) && (logicalAddress < 0x1000)) ||
		((logicalAddress > 0x07FF) && (logicalAddress < 0x0C00)))
	{
		return CONFIG::EEPROM_PHYSICAL_ADDRESS_FIXED_BITS | ((physical_address_t) getEpage() << CONFIG::EEPROM_ADDRESS_SIZE) | ((address_t) CONFIG::EEPROM_CPU_ADDRESS_BITS & logicalAddress);
	}
	else
	{
		return logicalAddress;
	}
}

inline uint8_t MMC::getPpage () { return ppage; }
inline void MMC::setPpage(uint8_t page) { ppage = page; }

inline physical_address_t MMC::getFlashAddress(address_t logicalAddress) {

	uint8_t _ppage = getPpage();
	
	static const uint8_t ROMHM_MASK = 0x02;
	
	if ((logicalAddress > 0x3FFF) && (logicalAddress < 0x8000)) {
		if ((getMmcctl1() & ROMHM_MASK) == 0) {
		 	return CONFIG::FLASH_PHYSICAL_ADDRESS_FIXED_BITS | ((physical_address_t) 0xFD << CONFIG::FLASH_ADDRESS_SIZE) | ((address_t) CONFIG::FLASH_CPU_ADDRESS_BITS & logicalAddress);
		}
	} 
	

	if (logicalAddress > 0xBFFF) {
		return CONFIG::FLASH_PHYSICAL_ADDRESS_FIXED_BITS | ((physical_address_t) 0xFF << CONFIG::FLASH_ADDRESS_SIZE) | ((address_t) CONFIG::FLASH_CPU_ADDRESS_BITS & logicalAddress);
	} 
	
	if (_ppage == 0) return logicalAddress;

	if ((_ppage > 0) && (_ppage < CONFIG::PPAGE_LOW)) {
		throw NonMaskableAccessErrorInterrupt(NonMaskableAccessErrorInterrupt::INVALIDE_PPAGE);
	}

	if (((_ppage == 0xFD) && (logicalAddress > 0x3FFF) && (logicalAddress < 0x8000)) ||
		((_ppage == 0xFF) && (logicalAddress > 0xBFFF)) ||
		((logicalAddress > 0x7FFF) && (logicalAddress < 0xC000)))
	{
		return CONFIG::FLASH_PHYSICAL_ADDRESS_FIXED_BITS | ((physical_address_t) getPpage() << CONFIG::FLASH_ADDRESS_SIZE) | ((address_t) CONFIG::FLASH_CPU_ADDRESS_BITS & logicalAddress);
	}
	else
	{
		return logicalAddress;
	}

}

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_MMC_HH__


