
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
		case MEMORY::DIRECT: {
			address = getDirectAddress((uint8_t) logicalAddress); 
		} break;
	}
	return address;
}

void MMC::setGpage (uint8_t val) { _gpage = val;}
uint8_t MMC::getGpage () { return _gpage; }

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

