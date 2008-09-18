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

#include <mmc.hh>
#include <stdio.h>
#include <assert.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace hcs12x {
	
MMC::MMC() : 
	_isDirectSet(false),
	_gpage(GLOBAL_RESET_PAGE),
    _direct(DIRECT_RESET_PAGE),
    _rpage(RAM_RESET_PAGE),      
    _epage(EEPROM_RESET_PAGE),      
    _ppage(FLASH_RESET_PAGE),
    _mmcctl0(0),
	_mode(0),
	_mmcctl1(0),
	_ramwpc(0),
	_ramxgu(0),
	_ramshl(0),
	_ramshu(0)
{    
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

			if (logicalAddress <= REG_HIGH_OFFSET) { // Access to registers
				address = logicalAddress;
				gShift = sizeof(address_t) * 8; 

			}
			 
			if ((logicalAddress >= EEPROM_LOW_OFFSET) && (logicalAddress <= EEPROM_HIGH_OFFSET)) { // Access to EEPROM
				address = getEepromAddress(logicalAddress);
				gShift = EEPROM_ADDRESS_SIZE + 8; 

			} 
			
			if ((logicalAddress >= RAM_LOW_OFFSET) && (logicalAddress <= RAM_HIGH_OFFSET)) { // Access to RAM
				address = getRamAddress(logicalAddress);
				gShift = RAM_ADDRESS_SIZE + 8; 

			} 
			
			if (logicalAddress >= FLASH_LOW_OFFSET) { // Access to Flash
				address = getFlashAddress(logicalAddress);
				gShift = FLASH_ADDRESS_SIZE + 8; 
			}
		} break;
	}

	if (isGlobal) {
		address = ((physical_address_t) getGpage() << gShift) | address;
	} 

	return address;
}

void MMC::setMmcctl0 (uint8_t val) { _mmcctl0 = val; }
uint8_t MMC::getMmcctl0 () { return _mmcctl0; }

void MMC::setMode (uint8_t val) { _mode = val; }
uint8_t MMC::getMode () { return _mode; }
	
void MMC::setMmcctl1 (uint8_t val) { _mmcctl1 = val; }
uint8_t MMC::getMmcctl1 () { return _mmcctl1; }
	
void MMC::setRamwpc (uint8_t val) { _ramwpc = val; }
uint8_t MMC::getRamwpc () { return _ramwpc; }

void MMC::setRamxgu (uint8_t val) { _ramxgu = val; }
uint8_t MMC::getRamxgu () { return _ramxgu; }

void MMC::setRamshl (uint8_t val) { _ramshl = val; }
uint8_t MMC::getRamshl () { return _ramshl; }

void MMC::setRamshu (uint8_t val) { _ramshu = val; }
uint8_t MMC::getRamshu () { return _ramshu; }

void MMC::setGpage (uint8_t val) { _gpage = val;}
uint8_t MMC::getGpage () { return _gpage; }

void MMC::setDirect (uint8_t val) { 
 
	if (!_isDirectSet) {
    	_direct = val; 
    	_isDirectSet = true;
	} 
}
uint8_t MMC::getDirect () { return _direct; }

physical_address_t MMC::getDirectAddress(uint8_t lowByte) {
	if (_direct != 0)
	{
		return (((address_t) getDirect()) << DIRECT_ADDRESS_SIZE) | ((address_t) lowByte & 0x00FF);
	} else 
	{
		return lowByte;
	}
	
}


void MMC::setRpage (uint8_t val) { _rpage = val;}
uint8_t MMC::getRpage () { return _rpage; }
physical_address_t MMC::getRamAddress(address_t logicalAddress) {
/*
	if (_rpage != 0)
	{
		return RAM_PHYSICAL_ADDRESS_FIXED_BITS | ((physical_address_t) getRpage() << RAM_ADDRESS_SIZE) | ((address_t) RAM_CPU_ADDRESS_BITS & logicalAddress);
	} else 
	{
		return logicalAddress;
	}
*/	
	return computePhysicalAddress(logicalAddress, RAM_PAGE_SIZE, RPAGE_LOW, RPAGE_HIGH, RAM_LOW_OFFSET,
								 RAM_PHYSICAL_ADDRESS_FIXED_BITS, RAM_ADDRESS_SIZE, RAM_CPU_ADDRESS_BITS, _rpage);
	
}

void MMC::setEpage (uint8_t val) { _epage = val;}
uint8_t MMC::getEpage () { return _epage; }
physical_address_t MMC::getEepromAddress(address_t logicalAddress) {
/*
	if (_epage != 0)
	{
		return EEPROM_PHYSICAL_ADDRESS_FIXED_BITS | ((physical_address_t) getEpage() << EEPROM_ADDRESS_SIZE) | ((address_t) EEPROM_CPU_ADDRESS_BITS & logicalAddress);
	} else 
	{
		return logicalAddress;
	}
*/
	return computePhysicalAddress(logicalAddress, EEPROM_PAGE_SIZE, EPAGE_LOW, EPAGE_HIGH, EEPROM_LOW_OFFSET,
								 EEPROM_PHYSICAL_ADDRESS_FIXED_BITS, EEPROM_ADDRESS_SIZE, EEPROM_CPU_ADDRESS_BITS, _epage);

}

void MMC::setPpage (uint8_t val) { _ppage = val;}
uint8_t MMC::getPpage () { return _ppage; }
physical_address_t MMC::getFlashAddress(address_t logicalAddress) {
/*
	if (_ppage != 0)
	{
		return FLASH_PHYSICAL_ADDRESS_FIXED_BITS | ((physical_address_t) getPpage() << FLASH_ADDRESS_SIZE) | ((address_t) FLASH_CPU_ADDRESS_BITS & logicalAddress);
	} else 
	{
		return logicalAddress;
	}
*/

	return computePhysicalAddress(logicalAddress, FLASH_PAGE_SIZE, PPAGE_LOW, PPAGE_HIGH, FLASH_LOW_OFFSET,
								 FLASH_PHYSICAL_ADDRESS_FIXED_BITS, FLASH_ADDRESS_SIZE, FLASH_CPU_ADDRESS_BITS, _ppage);
}

physical_address_t MMC::computePhysicalAddress (address_t logicalAddress, uint16_t pageSize, uint8_t lowRegVal, uint8_t highRegVal,
												address_t lowOffset, physical_address_t gMask, uint8_t memAddressSize,
												address_t cpuAddressMask, uint8_t pageReg)
{
	
	uint8_t numberPage = (highRegVal - lowRegVal + 1);
	
	for (uint8_t i = 0; i < numberPage; i++)
	{
		if (pageReg == lowRegVal+i) {
			if (logicalAddress < (lowOffset + pageSize * (i+1)))
			{
				return gMask |
						((physical_address_t) pageReg << memAddressSize) |
						((address_t) cpuAddressMask & logicalAddress);
			}
		} 
		
	}
	
	return logicalAddress;
	
}

}
}
}
}
} // end namespace hcs12x

