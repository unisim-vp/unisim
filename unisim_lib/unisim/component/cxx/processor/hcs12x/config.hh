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
	static const bool TIMING_ENABLE					= true;

	static const bool DEBUG_ENABLE					= false;
	static const bool DEBUG_EXCEPTION_ENABLE		= false;
	static const bool REGISTERS_INFO				= false;

	static const bool HAS_RESET						= false;
	static const bool HAS_NON_MASKABLE_XIRQ_INTERRUPT	= false;
	static const bool HAS_NON_MASKABLE_ACCESS_ERROR_INTERRUPT = false;
	static const bool HAS_MASKABLE_IBIT_INTERRUPT	= false;
	static const bool HAS_NONMASKABLE_SWI_INTERRUPT	= false;
	static const bool HAS_TRAP_INTERRUPT			= false;
	static const bool HAS_SYSCALL_INTERRUPT			= false;
	static const bool HAS_SPURIOUS_INTERRUPT		= false;

	//======================================================================
	//=   XINT Registers, Reset Values and Fixed Interrupt vectors Offsets =
	//======================================================================

	static const address_t	IVBR_ADDRESS			= 0x0121;	// S12XINT: Address of the Interrupt Vector Base Register
	static const uint8_t	IVBR_RESET_VALUE		= 0xFF;		// IVBR is only one.

	static const address_t	INT_XGPRIO				= 0x0126;
	static const uint8_t	INT_XGPRIO_RESET_VALUE	= 0x01;

	static const address_t	INT_CFADDR				= 0x0127;
	static const uint8_t	INT_CFADDR_RESET_VALUE	= 0x10;

	static const address_t	INT_CFDATA0				= 0x0128;
	static const uint8_t	INT_CFDATA0_RESET_VALUE	= 0x01;

	static const address_t	INT_CFDATA1				= 0x0129;
	static const uint8_t	INT_CFDATA1_RESET_VALUE	= 0x01;

	static const address_t	INT_CFDATA2				= 0x012A;
	static const uint8_t	INT_CFDATA2_RESET_VALUE	= 0x01;

	static const address_t	INT_CFDATA3				= 0x012B;
	static const uint8_t	INT_CFDATA3_RESET_VALUE	= 0x01;

	static const address_t	INT_CFDATA4				= 0x012C;
	static const uint8_t	INT_CFDATA4_RESET_VALUE	= 0x01;

	static const address_t	INT_CFDATA5				= 0x012D;
	static const uint8_t	INT_CFDATA5_RESET_VALUE	= 0x01;

	static const address_t	INT_CFDATA6				= 0x012E;
	static const uint8_t	INT_CFDATA6_RESET_VALUE	= 0x01;

	static const address_t	INT_CFDATA7				= 0x012F;
	static const uint8_t	INT_CFDATA7_RESET_VALUE	= 0x01;

	// Fixed Interrupt vectors Offsets
	static const uint8_t	INT_SYS_RESET_OFFSET			= 0xFE;
	static const uint8_t	INT_ILLEGAL_ACCESS_RESET_OFFSET	= 0xFE;
	static const uint8_t	INT_CLK_MONITOR_RESET_OFFSET	= 0xFC;
	static const uint8_t	INT_COP_WATCHDOG_RESET_OFFSET	= 0xFA;
	static const uint8_t	INT_TRAP_OFFSET					= 0xF8;
	static const uint8_t	INT_SWI_OFFSET					= 0xF6;
	static const uint8_t	INT_XIRQ_OFFSET					= 0xF4;
	static const uint8_t	INT_IRQ_OFFSET					= 0xF2;
	static const uint8_t	INT_RAM_ACCESS_VIOLATION_OFFSET	= 0x60;
	static const uint8_t	INT_SYSCALL_OFFSET				= 0x12;
	static const uint8_t	INT_SPURIOUS_OFFSET				= 0x10;

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


#ifdef MC9S12XDP512

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

#else // unknown and will cause compilation errors

#endif


};


} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_CONFIG_HH__

