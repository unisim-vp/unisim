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

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace hcs12x {

#define WO_GLOBAL_ADDRESS	false	// without global addressing (64KB address space)
#define W_GLOBAL_ADDRESS	true	// with global addressing (8MB address space)

class MEMORY {
public:

	enum MAP {DIRECT=0, EXTENDED=1};
};

class MMC 
{
public:

    MMC();
       
	physical_address_t getPhysicalAddress(address_t logicalAddress, MEMORY::MAP type, bool isGlobal);


    void setMmcctl0 (uint8_t val);
    uint8_t getMmcctl0 ();

	void setMode (uint8_t val);
	uint8_t getMode ();
	
    void    setGpage (uint8_t val);
    uint8_t getGpage ();
	
    void    setDirect (uint8_t val);
    uint8_t getDirect ();

	void setMmcctl1 (uint8_t val);
	uint8_t getMmcctl1 ();
	
    void    setRpage (uint8_t val);
    uint8_t getRpage ();

    void    setEpage (uint8_t val);
    uint8_t getEpage ();

    void    setPpage (uint8_t val);
    uint8_t getPpage ();

	void setRamwpc (uint8_t val);
	uint8_t getRamwpc ();

	void setRamxgu (uint8_t val);
	uint8_t getRamxgu ();

	void setRamshl (uint8_t val);
	uint8_t getRamshl ();

	void setRamshu (uint8_t val);
	uint8_t getRamshu ();

protected:
	physical_address_t getDirectAddress(uint8_t lowByte);
	physical_address_t getRamAddress(address_t logicalAddress);
	physical_address_t getEepromAddress(address_t logicalAddress);
	physical_address_t getFlashAddress(address_t logicalAddress);

	physical_address_t computePhysicalAddress (address_t logicalAddress, uint16_t pageSize, uint8_t lowRegVal, uint8_t highRegVal,
												address_t lowOffset, physical_address_t gMask, uint8_t memAddressSize,
												address_t cpuAddressMask, uint8_t pageReg);

private:
	
	//=============================================
	//=            MMC REGISTERS                  =
	//=============================================
    uint8_t _mmcctl0;
	uint8_t _mode;
	uint8_t _gpage;      // The truth size of GPage is 0x07bits. MAX_VAL(gpage) == 0x7F
    uint8_t _direct;
	uint8_t _mmcctl1;
    uint8_t _rpage;      
    uint8_t _epage;      
    uint8_t _ppage;      
	uint8_t _ramwpc;
	uint8_t _ramxgu;
	uint8_t _ramshl;
	uint8_t _ramshu;

    bool        _isDirectSet;

public: 

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

	//=====================================================================
	//=   RESET VALUES OF MMC REGISTERS and Logical Memories Offsets      =
	//=====================================================================

	static const address_t REG_LOW_OFFSET	= 0x0000;
	static const address_t REG_HIGH_OFFSET	= 0x07FF;
	static const address_t EEPROM_LOW_OFFSET= 0x0800;
	static const address_t EEPROM_HIGH_OFFSET=0x0FFF;
	static const address_t RAM_LOW_OFFSET	= 0x1000;
	static const address_t RAM_HIGH_OFFSET	= 0x3FFF;
	static const address_t FLASH_LOW_OFFSET	= 0x4000;
	static const address_t FLASH_HIGH_OFFSET=0xFFFF;

	static const address_t MMCCTL0_REG_ADDRESS	= 0x000A;
	static const address_t MODE_REG_ADDRESS		= 0x000B;
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
	
	 
#ifdef MC9S12XDP512

	static const uint8_t GPAGE_LOW			= 0x00;		// low gpage register value
	static const uint8_t GPAGE_HIGH			= 0x7F;		// high gpage register value

//	static const uint8_t RPAGE_LOW			= 0xFD;		// low rpage (ram page) register value
	static const uint8_t RPAGE_LOW			= 0xF8;		// low rpage (ram page) register value
	static const uint8_t RPAGE_HIGH			= 0xFF;		// high rpage register value
//	static const uint8_t EPAGE_LOW			= 0xFE;		// low epage (eeprom page) register value
	static const uint8_t EPAGE_LOW			= 0xFC;		// low epage (eeprom page) register value
	static const uint8_t EPAGE_HIGH			= 0xFF;		// high epage register value 
//	static const uint8_t PPAGE_LOW			= 0xFD;		// low ppage (flash page) register value
	static const uint8_t PPAGE_LOW			= 0xE0;		// low ppage (flash page) register value
	static const uint8_t PPAGE_HIGH			= 0xFF;		// high ppage register value

	static const uint8_t GLOBAL_RESET_PAGE	= GPAGE_LOW;// reset gpage register value 
	static const uint8_t DIRECT_RESET_PAGE	= 0x00;		// reset direct register value 

	static const uint8_t RAM_RESET_PAGE		= 0xFD;		// reset rpage register value
	static const uint8_t EEPROM_RESET_PAGE	= 0xFE;		// reset epage register value  
	static const uint8_t FLASH_RESET_PAGE	= 0xFE;		// reset ppage register value 

#else // unknown and will cause compilation errors

#endif
	

};

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_MMC_HH__


