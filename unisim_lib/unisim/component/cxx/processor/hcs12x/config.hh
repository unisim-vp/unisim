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


#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_CONFIG_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_CONFIG_HH__

#ifdef GCC_INLINE
#undef GCC_INLINE
#endif

#if defined(__GNUC__) && (__GNUC__ >= 3)
#define GCC_INLINE __attribute__((always_inline))
#else
#define GCC_INLINE
#endif

#include <unisim/component/cxx/processor/hcs12x/types.hh>
#include <inttypes.h>

// ToRemove: Used Tempory for development purposes to force MC9S12XDP512 
#define MC9S12XDP512
// *** end to remove ***


namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace hcs12x {

struct CONFIG {
	/*
	 * static initialization may rise problems in SMP architectures !!!
	 */
	static const bool TIMING_ENABLE				= false;

	static const bool DEBUG_ENABLE				= true;
	static const bool DEBUG_EXCEPTION_ENABLE	= false;
	static const bool REGISTERS_INFO			= true;
	static const bool HAS_HARD_RESET			= false;
	static const bool HAS_SOFT_RESET			= false;
	static const bool HAS_SOFTWARE_INTERRUPT	= false;
	static const bool HAS_SYS_CALL_INTERRUPT	= false;
	static const bool HAS_NON_MASKABLE_XIRQ_INTERRUPT	= false;
	static const bool HAS_MASKABLE_INTERRUPT	= false;


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
	
	 
#ifdef MC9S12XDP512

	static const uint16_t REGISTERS_SPACE_SIZE	= 0x800;	// MCS12XDP512 has 2k-bytes of io-register space 
															// mapped at low address of memory

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

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_CONFIG_HH__

