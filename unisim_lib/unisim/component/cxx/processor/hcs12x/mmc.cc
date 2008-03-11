
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
	
	physical_address_t address;
	
	switch (type) {
		case MEMORY::EXTENDED: { 
			address = logicalAddress; 
		} break;
		case MEMORY::GLOBAL: {
			address = _gpage + logicalAddress; 
		} break;
		case MEMORY::RAM: break;
		case MEMORY::EEPROM: break;
		case MEMORY::FLASH: break;
		case MEMORY::DIRECT: {
			address = _direct + logicalAddress;
		} break;
	}
	return address;
}

void MMC::setGpage (uint8_t val) { _gpage = (uint32_t) (val << 16);}
uint8_t MMC::getGpage () { return _gpage >> 16; }

void MMC::setDirect (uint8_t val) { 
 
	if (!_isDirectSet) {
    	_direct = (uint32_t) (val << 16); 
    	_isDirectSet = true;
	} 
}
uint8_t MMC::getDirect () { return _direct >> 16; }

void MMC::setRpage (uint8_t val) { _rpage = (uint32_t) (val << 16);}
uint8_t MMC::getRpage () { return _rpage >> 16; }

void MMC::setEpage (uint8_t val) { _epage = (uint32_t) (val << 16);}
uint8_t MMC::getEpage () { return _epage >> 16; }

void MMC::setPpage (uint8_t val) { _ppage = (uint32_t) (val << 16);}
uint8_t MMC::getPpage () { return _ppage >> 16; }

	
}
}
}
}
} // end namespace hcs12x

