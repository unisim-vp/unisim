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
	
MMC::MMC(HC_Registers *regs) : 
	 registers(regs)
{    
	reset();
}


physical_address_t MMC::getPhysicalAddress(address_t logicalAddress, MEMORY::MAP type, bool isGlobal) {

	uint8_t gShift;
	
	physical_address_t address = 0;
	
	switch (type) {
		case MEMORY::DIRECT: {
			address = getDirectAddress((uint8_t) logicalAddress);
			gShift = sizeof(address_t) * 8; 
		} break;

		case MEMORY::EXTENDED: {

			if (logicalAddress <= CONFIG::REG_HIGH_OFFSET) { // Access to registers
				address = logicalAddress;
				gShift = sizeof(address_t) * 8; 

			}
			 
			if ((logicalAddress >= CONFIG::EEPROM_LOW_OFFSET) && (logicalAddress <= CONFIG::EEPROM_HIGH_OFFSET)) { // Access to EEPROM
				address = getEepromAddress(logicalAddress);
				gShift = CONFIG::EEPROM_ADDRESS_SIZE + 8; 

			} 
			
			if ((logicalAddress >= CONFIG::RAM_LOW_OFFSET) && (logicalAddress <= CONFIG::RAM_HIGH_OFFSET)) { // Access to RAM
				address = getRamAddress(logicalAddress);
				gShift = CONFIG::RAM_ADDRESS_SIZE + 8; 

			} 
			
			if (logicalAddress >= CONFIG::FLASH_LOW_OFFSET) { // Access to Flash
				address = getFlashAddress(logicalAddress);
				gShift = CONFIG::FLASH_ADDRESS_SIZE + 8; 
			}
		} break;
	}

	if (isGlobal) {
		address = ((physical_address_t) getGpage() << gShift) | address;
	} 

	return address;
}

void MMC::reset() {

	registers->write(CONFIG::MMCCTL0_REG_ADDRESS, 0);
	registers->write(CONFIG::MMC_MODE_REG_ADDRESS, 0);
	registers->write(CONFIG::GPAGE_REG_ADDRESS, CONFIG::GLOBAL_RESET_PAGE); 
	registers->write(CONFIG::DIRECT_REG_ADDRESS, CONFIG::DIRECT_RESET_PAGE);
	registers->write(CONFIG::MMCCTL1_REG_ADDRESS, 0);
	registers->write(CONFIG::RPAGE_REG_ADDRESS, CONFIG::RAM_RESET_PAGE);
	registers->write(CONFIG::EPAGE_REG_ADDRESS, CONFIG::EEPROM_RESET_PAGE);
	registers->write(CONFIG::PPAGE_REG_ADDRESS, CONFIG::FLASH_RESET_PAGE);
	registers->write(CONFIG::RAMWPC_REG_ADDRESS, 0);
	registers->write(CONFIG::RAMXGU_REG_ADDRESS, 0);
	registers->write(CONFIG::RAMSHL_REG_ADDRESS, 0);
	registers->write(CONFIG::RAMSHU_REG_ADDRESS, 0);
	
}

uint8_t MMC::getMmcctl0 () { return registers->read(CONFIG::MMCCTL0_REG_ADDRESS); }

uint8_t MMC::getMode () { return registers->read(CONFIG::MMC_MODE_REG_ADDRESS); }
	
uint8_t MMC::getMmcctl1 () { return registers->read(CONFIG::MMCCTL1_REG_ADDRESS); }
	
uint8_t MMC::getRamwpc () { return registers->read(CONFIG::RAMWPC_REG_ADDRESS); }

uint8_t MMC::getRamxgu () { return registers->read(CONFIG::RAMXGU_REG_ADDRESS); }

uint8_t MMC::getRamshl () { return registers->read(CONFIG::RAMSHL_REG_ADDRESS); }

uint8_t MMC::getRamshu () { return registers->read(CONFIG::RAMSHU_REG_ADDRESS); }

uint8_t MMC::getGpage () { return registers->read(CONFIG::GPAGE_REG_ADDRESS); }

uint8_t MMC::getDirect () { return registers->read(CONFIG::DIRECT_REG_ADDRESS); }
physical_address_t MMC::getDirectAddress(uint8_t lowByte) {
	
	uint8_t _direct = getDirect (); 
	
	if (_direct != 0)
	{
		return (((address_t) getDirect()) << CONFIG::DIRECT_ADDRESS_SIZE) | ((address_t) lowByte & 0x00FF);
	} else 
	{
		return lowByte;
	}
	
}

uint8_t MMC::getRpage () { return registers->read(CONFIG::RPAGE_REG_ADDRESS); }
physical_address_t MMC::getRamAddress(address_t logicalAddress) {

	uint8_t _rpage = getRpage();
	
	if ((_rpage > 0x00) && (_rpage < CONFIG::RPAGE_LOW)) {
		// throw "non-valid accesses to memory"
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

uint8_t MMC::getEpage () { return registers->read(CONFIG::EPAGE_REG_ADDRESS); }
physical_address_t MMC::getEepromAddress(address_t logicalAddress) {

	uint8_t _epage = getEpage();
	
	if (_epage < CONFIG::EPAGE_LOW) {
		// throw "non-valid accesses to memory"
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

uint8_t MMC::getPpage () { return registers->read(CONFIG::PPAGE_REG_ADDRESS); }
void MMC::setPpage(uint8_t page) { registers->write(CONFIG::PPAGE_REG_ADDRESS, page); }

physical_address_t MMC::getFlashAddress(address_t logicalAddress) {

	uint8_t _ppage = getPpage();
	
	if (_ppage < CONFIG::PPAGE_LOW) {
		// throw "non-valid accesses to memory"
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

}
}
}
}
} // end namespace hcs12x

