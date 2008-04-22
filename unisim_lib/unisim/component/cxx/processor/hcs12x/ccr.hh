#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_CCR_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_HCS12X_CCR_HH__

#include <inttypes.h>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace hcs12x {

#define SETC	0x0001
#define CLRC	0xFFFE

#define SETV	0x0002
#define CLRV	0xFFFD

#define SETZ	0x0004
#define CLRZ	0xFFFB

#define SETN	0x0008
#define CLRN	0xFFF7

#define SETI	0x0010
#define CLRI	0xFFEF

#define SETH	0x0020
#define CLRH	0xFFDF

#define SETX	0x0040
#define CLRX	0xFFBF

#define SETS	0x0080
#define CLRS	0xFF7F

#define SETIPL	0x0100
#define CLRIPL	0xFEFF

/* I think it's better to declare the CCR as uint16_t and then use mask to set/get each bit */
 
class CCR_t
{
public:
	uint8_t getC();
	void 	setC();
	void 	clrC();

	uint8_t getV();
	void 	setV();
	void 	clrV();
	
	uint8_t getZ();
	void 	setZ();
	void 	clrZ();
	
	uint8_t getN();
	void 	setN();
	void 	clrN();
	
	uint8_t getI();
	void 	setI();
	void 	clrI();

	uint8_t getH();
	void 	setH();
	void 	clrH();

	uint8_t getX();
	void 	setX();
	void 	clrX();

	uint8_t getS();
	void 	setS();
	void 	clrS();

	uint8_t getIPL();
	void 	setIPL();
	void 	clrIPL();

	uint8_t getCCRLow();	
	void setCCRLow(uint8_t val);

	uint8_t getCCRHigh();
	void setCCRHigh(uint8_t val);

	uint16_t getCCR();
	void setCCR(uint16_t val);
	
private:

	uint16_t ccrVal; // -------ipl SXHI NZVC

}; // end class CCR_t 

} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
