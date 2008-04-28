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

#include <unisim/component/cxx/processor/hcs12x/ccr.hh>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace hcs12x {


uint8_t CCR_t::getC() { return ccrVal & SETC;};
void 	CCR_t::setC() { ccrVal |= SETC;};
void 	CCR_t::clrC() { ccrVal &= CLRC;};            

uint8_t CCR_t::getV() { return (ccrVal & SETV) >> 1;};
void 	CCR_t::setV() { ccrVal |= SETV;};
void 	CCR_t::clrV() { ccrVal &= CLRV;};            

uint8_t CCR_t::getZ() { return (ccrVal & SETZ) >> 2;};
void 	CCR_t::setZ() { ccrVal |= SETZ;};
void 	CCR_t::clrZ() { ccrVal &= CLRZ;};            

uint8_t CCR_t::getN() { return (ccrVal & SETN) >> 3;};
void 	CCR_t::setN() { ccrVal |= SETN;};
void 	CCR_t::clrN() { ccrVal &= CLRN;};            

uint8_t CCR_t::getI() { return (ccrVal & SETI) >> 4;};
void 	CCR_t::setI() { ccrVal |= SETI;};
void 	CCR_t::clrI() { ccrVal &= CLRI;};            

uint8_t CCR_t::getH() { return (ccrVal & SETH) >> 5;};
void 	CCR_t::setH() { ccrVal |= SETH;};
void 	CCR_t::clrH() { ccrVal &= CLRH;};            

uint8_t CCR_t::getX() { return (ccrVal & SETX) >> 6;};
void 	CCR_t::setX() { ccrVal |= SETX;};
void 	CCR_t::clrX() { ccrVal &= CLRX;};            

uint8_t CCR_t::getS() { return (ccrVal & SETS) >> 7;};
void 	CCR_t::setS() { ccrVal |= SETS;};
void 	CCR_t::clrS() { ccrVal &= CLRS;};            

uint8_t CCR_t::getIPL() { return (ccrVal & SETIPL) >> 8;};
void 	CCR_t::setIPL() { ccrVal |= SETIPL;};
void 	CCR_t::clrIPL() { ccrVal &= CLRIPL;};            

uint8_t CCR_t::getCCRLow() {
	uint8_t val = (uint8_t) (ccrVal & 0x00FF);
	
	return val;
};	
void CCR_t::setCCRLow(uint8_t val) {
	ccrVal = (ccrVal & 0xFF00) | val;
};

uint8_t CCR_t::getCCRHigh() {
	uint8_t val = (uint8_t) ((ccrVal >> 8) & 0x00FF); 
	
	return val;
};

void CCR_t::setCCRHigh(uint8_t val) {
	ccrVal = (ccrVal & 0x00FF) | ((uint16_t) val << 8);
};


uint16_t CCR_t::getCCR() {
	return ccrVal;
};

void CCR_t::setCCR(uint16_t val) {
	ccrVal = val;
};


} // end of namespace hcs12x
} // end of namespace processor
} // end of namespace cxx
} // end of namespace component
} // end of namespace unisim

