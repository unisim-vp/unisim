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
 
#ifndef __UNISIM_COMPONENT_CXX_MEMORY_FLASH_AM29LV160D_CONFIG_HH__
#define __UNISIM_COMPONENT_CXX_MEMORY_FLASH_AM29LV160D_CONFIG_HH__

#include "unisim/component/cxx/memory/flash/am29lv/types.hh"

namespace unisim {
namespace component {
namespace cxx {
namespace memory {
namespace flash {
namespace am29lv {

class AM29LV160DConfig
{
public:
	typedef uint64_t ADDRESS;
	typedef enum
	{
		STATE_INITIAL,
		STATE_AUTOSELECT,
		STATE_PROGRAM,
		STATE_UNLOCKED,
		STATE_UNLOCKED_PROGRAM,
		STATE_UNLOCKED_RESET,
		STATE_ERASE,
		STATE_CHIP_ERASE,
		STATE_SECTOR_ERASE
	} STATE;

	static const uint32_t BYTESIZE = 2097152; // 16 Mbits / 2 MBytes
	static const unsigned int NUM_SECTORS = 35;
	static const unsigned int MAX_IO_WIDTH = 2; // 16-bit I/O
	static const MODE MODE_SUPPORT = MODE_X8_X16; // x8/x16
	static const uint64_t ACCESS_TIME = 70ULL; // in ns (70 ns)
	static const uint64_t PROGRAMMING_TIME[MAX_IO_WIDTH]; // in ns
	static const uint64_t SECTOR_ERASING_TIME = 700000000ULL; // in ns (0.7 s)
	static const uint64_t CHIP_ERASING_TIME = 25000000000ULL; // in ns (25 s)
	static const uint8_t MANUFACTURER_ID[MAX_IO_WIDTH];
	static const uint8_t DEVICE_ID[MAX_IO_WIDTH];
	static const uint8_t PROTECTED[MAX_IO_WIDTH];
	static const uint8_t UNPROTECTED[MAX_IO_WIDTH];
	static const ADDRESS MANUFACTURER_ID_ADDR = 0x00;
	static const ADDRESS DEVICE_ID_ADDR = 0x02;
	static const ADDRESS SECTOR_PROTECT_VERIFY_ADDR = 0x04;
	static const unsigned NUM_TRANSITIONS = 30;
	static const TRANSITION<ADDRESS, MAX_IO_WIDTH, STATE> FSM[NUM_TRANSITIONS];
};

// AM29LV160D (top boot device)
class AM29LV160DTConfig : public AM29LV160DConfig
{
public:
	static const char *DEVICE_NAME;
	static const SECTOR_ADDRESS_RANGE<ADDRESS> SECTOR_MAP[NUM_SECTORS];
};

// AM29LV160D (bottom boot device)
class AM29LV160DBConfig : public AM29LV160DConfig
{
public:
	static const char *DEVICE_NAME;
	static const SECTOR_ADDRESS_RANGE<ADDRESS> SECTOR_MAP[NUM_SECTORS];
};

} // end of namespace am29lv
} // end of namespace flash
} // end of namespace memory
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_MEMORY_FLASH_AM29LV160D_CONFIG_HH__
