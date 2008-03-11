#ifndef __MMC_HH__
#define __MMC_HH__

#include <inttypes.h>
#include <iostream>
#include <unisim/component/cxx/processor/hcs12x/types.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace hcs12x {
	
//using namespace std;

class MEMORY {
public:
	/*
	 * GPAGE -> GLOBAL
	 * RPAGE -> RAM
	 * EPAGE -> EEPROM
	 * PPAGE -> FLASH
	 * DIRECt -> Direct page
	 */
	enum MAP {EXTENDED=0, GLOBAL=1, RAM=2, EEPROM=3, FLASH=4, DIRECT=5};
};

class MMC 
{
public:


    MMC(uint8_t gpage, uint8_t rpage, uint8_t epage, uint8_t ppage, uint8_t direct=0x00);
       
	physical_address_t getPhysicalAddress(address_t logicalAddress, MEMORY::MAP type);
	
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

#endif


