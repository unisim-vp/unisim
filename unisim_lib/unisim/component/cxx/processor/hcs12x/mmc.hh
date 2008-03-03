#ifndef __MMC_HH__
#define __MMC_HH__

#include <inttypes.h>
#include <iostream>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace hcs12x {
	
//using namespace std;

#define MAX_ADDRESS     0x007FFFFF // 8 MByte
#define MEMORY_SIZE     (MAX_ADDRESS + 1)    


class MMC 
{
public:
    
    MMC();

    void    setGpage (uint8_t val);
    uint8_t getGpage ();

    void    setRpage (uint8_t val);
    uint8_t getRpage ();

    void    setEpage (uint8_t val);
    uint8_t getEpage ();

    void    setPpage (uint8_t val);
    uint8_t getPpage ();

    void    setDirect (uint8_t val);
    uint8_t getDirect ();

	uint8_t memDirectRead8(uint16_t addr);
	uint8_t memRead8(uint16_t addr);
	void memWrite8(uint16_t addr,uint8_t val);
	void memDirectWrite8(uint16_t addr, uint8_t val);
	
	uint16_t memDirectRead16(uint16_t addr);
	uint16_t memRead16(uint16_t addr);
	void memWrite16(uint16_t addr,uint16_t val);
	void memDirectWrite16(uint16_t addr, uint16_t val);

private:
	// The truth size of {GPage, RPage, EPage, PPage, Direct} is 0x07bits. 
	// They are declared as uint32 to optimise simulation time
	
    uint32_t     gpage;      // MAX_VAL(gpage) == 0x7F
    uint32_t     rpage;      
    uint32_t     epage;      
    uint32_t     ppage;      
    uint32_t     direct;     
    bool        _isDirectSet;

	uint8_t     mem[MEMORY_SIZE];

};

}
}
}
}
} // end namespace hcs12x

#endif


