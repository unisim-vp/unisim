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
#include <unisim/component/cxx/processor/hcs12x/hc_registers.hh>

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

    MMC(HC_Registers *regs);
    void reset();
       
	physical_address_t getPhysicalAddress(address_t logicalAddress, MEMORY::MAP type, bool isGlobal);

    uint8_t getMmcctl0 ();
	uint8_t getMode ();
    uint8_t getGpage ();
    uint8_t getDirect ();
	uint8_t getMmcctl1 ();
    uint8_t getRpage ();
    uint8_t getEpage ();
    uint8_t getPpage ();
    void setPpage(uint8_t page);
	uint8_t getRamwpc ();
	uint8_t getRamxgu ();
	uint8_t getRamshl ();
	uint8_t getRamshu ();

	physical_address_t getDirectAddress(uint8_t lowByte);
	physical_address_t getRamAddress(address_t logicalAddress);
	physical_address_t getEepromAddress(address_t logicalAddress);
	physical_address_t getFlashAddress(address_t logicalAddress);

private:	
	//=============================================
	//=            MMC REGISTERS                  =
	//=============================================

	HC_Registers	*registers;		// registers is a reference to registers address space


};

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_MMC_HH__


