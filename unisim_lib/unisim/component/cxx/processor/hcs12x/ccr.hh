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

#define SETIPL	0x0700
#define CLRIPL	0xF8FF

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
	void 	setIPL(uint8_t newIPL);
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
