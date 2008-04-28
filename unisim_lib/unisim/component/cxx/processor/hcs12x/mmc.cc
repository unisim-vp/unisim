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
	
MMC::MMC(uint8_t gpage, uint8_t rpage, uint8_t epage, uint8_t ppage, uint8_t direct) {
    setGpage(gpage);    
    setRpage(rpage);
    setEpage(epage);
    setPpage(ppage);
    setDirect(direct);
    
}


physical_address_t MMC::getPhysicalAddress(address_t logicalAddress, MEMORY::MAP type) {
// TODO:
// understand how to use mmc during compute of the physicalAddress !!!!
	
	physical_address_t address = 0;
	
	switch (type) {
		case MEMORY::EXTENDED: { 
			address = logicalAddress; 
		} break;
		case MEMORY::GLOBAL: {
			address = getGlobalAddress(logicalAddress);
		} break;
		case MEMORY::DIRECT: {
			address = getDirectAddress((uint8_t) logicalAddress); 
		} break;
	}
	return address;
}

void MMC::setGpage (uint8_t val) { _gpage = val;}
uint8_t MMC::getGpage () { return _gpage; }

physical_address_t MMC::getGlobalAddress(address_t logicalAddress) {
	return ((physical_address_t) getGpage() << sizeof(address_t)*8) | logicalAddress;	
}

void MMC::setDirect (uint8_t val) { 
 
	if (!_isDirectSet) {
    	_direct = val; 
    	_isDirectSet = true;
	} 
}
uint8_t MMC::getDirect () { return _direct; }

address_t MMC::getDirectAddress(uint8_t lowByte) {

	return (((address_t) getDirect()) << 8) | ((address_t) lowByte & 0x00FF);
}


void MMC::setRpage (uint8_t val) { _rpage = val;}
uint8_t MMC::getRpage () { return _rpage; }

void MMC::setEpage (uint8_t val) { _epage = val;}
uint8_t MMC::getEpage () { return _epage; }

void MMC::setPpage (uint8_t val) { _ppage = val;}
uint8_t MMC::getPpage () { return _ppage; }

	
}
}
}
}
} // end namespace hcs12x

