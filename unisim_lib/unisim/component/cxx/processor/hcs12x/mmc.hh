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

#define MASK2RAM (pow(2, CONFIG::CPU2RAM_ADDRESS_SIZE) -1)
#define	MASK2EEPROM (pow(2, CONFIG::CPU2EEPROM_ADDRESS_SIZE) -1)
#define MASK2FLASH (pow(2, CONFIG::CPU2FLASH_ADDRESS_SIZE) -1)

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace hcs12x {

#define WO_GLOBAL_ADDRESS	false	// without global addressing (64KB address space)
#define W_GLOBAL_ADDRESS	true	// with global addressung (8MB address space)

class MEMORY {
public:
	/*
	 * GPAGE -> GLOBAL
	 * RPAGE -> RAM
	 * EPAGE -> EEPROM
	 * PPAGE -> FLASH
	 * DIRECt -> Direct page
	 */
	enum MAP {DIRECT=0, EXTENDED=1, RAM=2, EEPROM=3, FLASH=4};
};

class MMC 
{
public:


    MMC(uint8_t gpage, uint8_t rpage, uint8_t epage, uint8_t ppage, uint8_t direct);
       
	physical_address_t getPhysicalAddress(address_t logicalAddress, MEMORY::MAP type, bool isGlobal);
	
    void    setGpage (uint8_t val);
    uint8_t getGpage ();
	
    void    setDirect (uint8_t val);
    uint8_t getDirect ();
	
    void    setRpage (uint8_t val);
    uint8_t getRpage ();

    void    setEpage (uint8_t val);
    uint8_t getEpage ();

    void    setPpage (uint8_t val);
    uint8_t getPpage ();

protected:
	physical_address_t getDirectAddress(uint8_t lowByte);
	physical_address_t getRamAddress(address_t logicalAddress);
	physical_address_t getEepromAddress(address_t logicalAddress);
	physical_address_t getFlashAddress(address_t logicalAddress);

private:
	// The truth size of {GPage, RPage, EPage, PPage, Direct} is 0x07bits. 
	
    uint8_t     _gpage;      // MAX_VAL(gpage) == 0x7F
    uint8_t     _direct;

    bool        _isDirectSet;

    uint8_t     _rpage;      
    uint8_t     _epage;      
    uint8_t     _ppage;      

};

}
}
}
}
} // end namespace hcs12x

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_MMC_HH__


