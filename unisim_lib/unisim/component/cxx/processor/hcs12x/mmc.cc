
#include <mmc.hh>
#include <stdio.h>
#include <assert.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace hcs12x {
	
MMC::MMC() {
    gpage   = 0;    
    rpage   = 0;
    epage   = 0;
    ppage   = 0;
    direct  = 0;
    
    memset(mem, 0, sizeof(mem));
}

void MMC::setGpage (uint8_t val) { gpage = (uint32_t) (val << 16);}
uint8_t MMC::getGpage () { return gpage; }

void MMC::setRpage (uint8_t val) { rpage = (uint32_t) (val << 16);}
uint8_t MMC::getRpage () { return rpage; }

void MMC::setEpage (uint8_t val) { epage = (uint32_t) (val << 16);}
uint8_t MMC::getEpage () { return epage; }

void MMC::setPpage (uint8_t val) { ppage = (uint32_t) (val << 16);}
uint8_t MMC::getPpage () { return ppage; }

void MMC::setDirect (uint8_t val) { 
 
	if (!_isDirectSet) {
    	direct = (uint32_t) (val << 16); 
    	_isDirectSet = true;
	} 
}
uint8_t MMC::getDirect () { return direct; }

uint8_t MMC::memDirectRead8(uint16_t addr) {
    
    uint32_t address;
    
    address = direct + addr;
    
    return mem[address];
}

uint8_t MMC::memRead8(uint16_t addr) {

    return mem[addr];
}

void MMC::memDirectWrite8(uint16_t addr, uint8_t val) {

	uint32_t address;
    
    address = direct + addr;
    
    mem[address] = val;
}
	
uint16_t MMC::memDirectRead16(uint16_t addr) {
    
    uint32_t address;
    
    address = direct + addr;
    
    return (mem[address] << 8) | mem[address+1];
}

uint16_t MMC::memRead16(uint16_t addr) {
	return (mem[addr] << 8) + mem[addr+1];
}

void MMC::memDirectWrite16(uint16_t addr, uint16_t val) {

    uint32_t address;
    
    address = direct + addr;

	mem[address] = (uint8_t) val >> 8;
	mem[address+1] = (uint8_t) (val & 0x00FF);    
	
}

void MMC::memWrite8(uint16_t addr,uint8_t val) {
	mem[addr] = val;
}
	
void MMC::memWrite16(uint16_t addr,uint16_t val) {
	
	mem[addr] = (uint8_t) (val >> 8);
	mem[addr] = (uint8_t) (val & 0x00FF);
}

	
}
}
}
}
} // end namespace hcs12x

