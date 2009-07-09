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
#include <unisim/util/debug/register.hh>

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

class CCR_t : public unisim::util::debug::Register
{
public:

	CCR_t() { };
	~CCR_t() { };

	inline uint8_t getC();
	inline void 	setC();
	inline void 	clrC();

	inline uint8_t getV();
	inline void 	setV();
	inline void 	clrV();

	inline uint8_t getZ();
	inline void 	setZ();
	inline void 	clrZ();

	inline uint8_t getN();
	inline void 	setN();
	inline void 	clrN();

	inline uint8_t getI();
	inline void 	setI();
	inline void 	clrI();

	inline uint8_t getH();
	inline void 	setH();
	inline void 	clrH();

	inline uint8_t getX();
	inline void	setX();
	inline void 	clrX();

	inline uint8_t getS();
	inline void 	setS();
	inline void 	clrS();

	inline uint8_t getIPL();
	inline void 	setIPL(uint8_t newIPL);
	inline void 	clrIPL();

	inline uint8_t getCCRLow();
	inline void setCCRLow(uint8_t val);

	inline uint8_t getCCRHigh();
	inline void setCCRHigh(uint8_t val);

	inline uint16_t getCCR();
	inline void setCCR(uint16_t val);

	void reset() { ccrVal = 0x00D0; /* S=1 X=1 I=1 */ }

	virtual const char *GetName() const;
	virtual void GetValue(void *buffer) const;
	virtual void SetValue(const void *buffer);
	virtual int GetSize() const;

	unisim::util::debug::Register *GetLowRegister();
	unisim::util::debug::Register *GetHighRegister();
private:

	uint16_t ccrVal; // u----ipl(3bits) SXHI NZVC

}; // end class CCR_t


inline uint8_t CCR_t::getC() { return ccrVal & SETC;};
inline void 	CCR_t::setC() { ccrVal |= SETC;};
inline void 	CCR_t::clrC() { ccrVal &= CLRC;};

inline uint8_t CCR_t::getV() { return (ccrVal & SETV) >> 1;};
inline void 	CCR_t::setV() { ccrVal |= SETV;};
inline void 	CCR_t::clrV() { ccrVal &= CLRV;};

inline uint8_t CCR_t::getZ() { return (ccrVal & SETZ) >> 2;};
inline void 	CCR_t::setZ() { ccrVal |= SETZ;};
inline void 	CCR_t::clrZ() { ccrVal &= CLRZ;};

inline uint8_t CCR_t::getN() { return (ccrVal & SETN) >> 3;};
inline void 	CCR_t::setN() { ccrVal |= SETN;};
inline void 	CCR_t::clrN() { ccrVal &= CLRN;};

inline uint8_t CCR_t::getI() { return (ccrVal & SETI) >> 4;};
inline void 	CCR_t::setI() { ccrVal |= SETI;};
inline void 	CCR_t::clrI() { ccrVal &= CLRI;};

inline uint8_t CCR_t::getH() { return (ccrVal & SETH) >> 5;};
inline void 	CCR_t::setH() { ccrVal |= SETH;};
inline void 	CCR_t::clrH() { ccrVal &= CLRH;};

inline uint8_t CCR_t::getX() { return (ccrVal & SETX) >> 6;};
inline void	CCR_t::setX() { ccrVal |= SETX;};
inline void 	CCR_t::clrX() { ccrVal &= CLRX;};

inline uint8_t CCR_t::getS() { return (ccrVal & SETS) >> 7;};
inline void 	CCR_t::setS() { ccrVal |= SETS;};
inline void 	CCR_t::clrS() { ccrVal &= CLRS;};

// IPL is 3-bits
inline uint8_t CCR_t::getIPL() { return (ccrVal & SETIPL) >> 8;};
inline void 	CCR_t::setIPL(uint8_t newIPL) { clrIPL(); ccrVal |= (((uint16_t) newIPL) & 0x00FF) << 8;};
inline void 	CCR_t::clrIPL() { ccrVal &= CLRIPL;};

inline uint8_t CCR_t::getCCRLow() {
	uint8_t val = (uint8_t) (ccrVal & 0x00FF);

	return val;
};
inline void CCR_t::setCCRLow(uint8_t val) {
	// before check the X-bit: once cleared it cannot be set by program instructions
	if (getX() == 0) val &= 0xBF;
	ccrVal = (ccrVal & 0xFF00) | val;
};

inline uint8_t CCR_t::getCCRHigh() {
	uint8_t val = (uint8_t) ((ccrVal >> 8) & 0x00FF);

	return val;
};

inline void CCR_t::setCCRHigh(uint8_t val) {
	ccrVal = (ccrVal & 0x00FF) | ((uint16_t) val << 8);
};


inline uint16_t CCR_t::getCCR() {
	return ccrVal;
};

inline void CCR_t::setCCR(uint16_t val) {
	// before check the X-bit: once cleared it cannot be set by program instructions
	if (getX() == 0) val &= 0xFFBF;

	ccrVal = val;
};


} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

#endif
