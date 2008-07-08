#include "xb.hh"
#include <cassert>
#include <ostream>
#include <cstring>
#line 75 "xb.isa"

using namespace std;

#line 10 "xb.cc"
namespace unisim { namespace component { namespace cxx { namespace processor { namespace hcs12x { namespace XB {
unsigned int const CodeType::maxsize;
std::ostream& operator << ( std::ostream& _sink, CodeType const& _ct ) {
	char const* xrepr = "0123456789abcdef";
	char const* sep = "";
	for( unsigned int idx = 0; idx < _ct.size; ++idx, sep = " " ) {
		uint32_t byte = _ct.str[idx];
		_sink << sep << xrepr[byte/16] << xrepr[byte%16];
	}
	return _sink;
}
Operation::Operation(CodeType const& _code, uint16_t _addr, const char *_name)
:
encoding(_code),
addr(_addr),
name(_name)
{
}

Operation::~Operation()
{
}

#line 89 "xb.isa"
uint16_t
#line 36 "xb.cc"
Operation::getEAddr(
#line 89 "xb.isa"
CPU *
#line 40 "xb.cc"
#line 89 "xb.isa"
cpu
#line 43 "xb.cc"
)
{
#line 89 "xb.isa"
	{ assert( false ); return 0; }
#line 48 "xb.cc"
}
#line 85 "xb.isa"
void
#line 52 "xb.cc"
Operation::disasm(
#line 85 "xb.isa"
ostream&
#line 56 "xb.cc"
#line 85 "xb.isa"
sink
#line 59 "xb.cc"
)
{
#line 85 "xb.isa"
	{
		sink << "?";
	}
#line 66 "xb.cc"
}
#line 80 "xb.isa"
uint8_t
#line 70 "xb.cc"
Operation::getXbMode()
{
#line 81 "xb.isa"
	{
		return XbModes::Unknown;
	}
#line 77 "xb.cc"
}
class OpXb_auto : public Operation
{
public:
	OpXb_auto(CodeType const& code, uint16_t addr);
	uint32_t rr;
	uint32_t p;
	uint32_t nnnn;
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
	virtual
#line 80 "xb.isa"
	uint8_t
#line 122 "xb.cc"
	getXbMode();
	virtual
#line 89 "xb.isa"
	uint16_t
#line 127 "xb.cc"
	getEAddr(
#line 89 "xb.isa"
	CPU *
#line 131 "xb.cc"
#line 89 "xb.isa"
	cpu
#line 134 "xb.cc"
	);
	virtual
#line 85 "xb.isa"
	void
#line 139 "xb.cc"
	disasm(
#line 85 "xb.isa"
	ostream&
#line 143 "xb.cc"
#line 85 "xb.isa"
	sink
#line 146 "xb.cc"
	);
private:
};

class OpXb_5bit_cst : public Operation
{
public:
	OpXb_5bit_cst(CodeType const& code, uint16_t addr);
	uint32_t rr;
	int32_t nnnnn;
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
	virtual
#line 80 "xb.isa"
	uint8_t
#line 193 "xb.cc"
	getXbMode();
	virtual
#line 89 "xb.isa"
	uint16_t
#line 198 "xb.cc"
	getEAddr(
#line 89 "xb.isa"
	CPU *
#line 202 "xb.cc"
#line 89 "xb.isa"
	cpu
#line 205 "xb.cc"
	);
	virtual
#line 85 "xb.isa"
	void
#line 210 "xb.cc"
	disasm(
#line 85 "xb.isa"
	ostream&
#line 214 "xb.cc"
#line 85 "xb.isa"
	sink
#line 217 "xb.cc"
	);
private:
};

class OpXb_9_cst_z0 : public Operation
{
public:
	OpXb_9_cst_z0(CodeType const& code, uint16_t addr);
	uint32_t rr;
	uint32_t s;
	int32_t n8;
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
	virtual
#line 80 "xb.isa"
	uint8_t
#line 265 "xb.cc"
	getXbMode();
	virtual
#line 89 "xb.isa"
	uint16_t
#line 270 "xb.cc"
	getEAddr(
#line 89 "xb.isa"
	CPU *
#line 274 "xb.cc"
#line 89 "xb.isa"
	cpu
#line 277 "xb.cc"
	);
	virtual
#line 85 "xb.isa"
	void
#line 282 "xb.cc"
	disasm(
#line 85 "xb.isa"
	ostream&
#line 286 "xb.cc"
#line 85 "xb.isa"
	sink
#line 289 "xb.cc"
	);
private:
};

class OpXb_16bit_cst_z1_s0 : public Operation
{
public:
	OpXb_16bit_cst_z1_s0(CodeType const& code, uint16_t addr);
	uint32_t rr;
	int32_t n16;
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
	virtual
#line 80 "xb.isa"
	uint8_t
#line 336 "xb.cc"
	getXbMode();
	virtual
#line 89 "xb.isa"
	uint16_t
#line 341 "xb.cc"
	getEAddr(
#line 89 "xb.isa"
	CPU *
#line 345 "xb.cc"
#line 89 "xb.isa"
	cpu
#line 348 "xb.cc"
	);
	virtual
#line 85 "xb.isa"
	void
#line 353 "xb.cc"
	disasm(
#line 85 "xb.isa"
	ostream&
#line 357 "xb.cc"
#line 85 "xb.isa"
	sink
#line 360 "xb.cc"
	);
private:
};

class OpXb_16bit_cst_z1_s1 : public Operation
{
public:
	OpXb_16bit_cst_z1_s1(CodeType const& code, uint16_t addr);
	uint32_t rr;
	int32_t n16;
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
	virtual
#line 80 "xb.isa"
	uint8_t
#line 407 "xb.cc"
	getXbMode();
	virtual
#line 89 "xb.isa"
	uint16_t
#line 412 "xb.cc"
	getEAddr(
#line 89 "xb.isa"
	CPU *
#line 416 "xb.cc"
#line 89 "xb.isa"
	cpu
#line 419 "xb.cc"
	);
	virtual
#line 85 "xb.isa"
	void
#line 424 "xb.cc"
	disasm(
#line 85 "xb.isa"
	ostream&
#line 428 "xb.cc"
#line 85 "xb.isa"
	sink
#line 431 "xb.cc"
	);
private:
};

class OpXb_acc_offset : public Operation
{
public:
	OpXb_acc_offset(CodeType const& code, uint16_t addr);
	uint32_t rr;
	uint32_t aa;
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
	virtual
#line 80 "xb.isa"
	uint8_t
#line 478 "xb.cc"
	getXbMode();
	virtual
#line 89 "xb.isa"
	uint16_t
#line 483 "xb.cc"
	getEAddr(
#line 89 "xb.isa"
	CPU *
#line 487 "xb.cc"
#line 89 "xb.isa"
	cpu
#line 490 "xb.cc"
	);
	virtual
#line 85 "xb.isa"
	void
#line 495 "xb.cc"
	disasm(
#line 85 "xb.isa"
	ostream&
#line 499 "xb.cc"
#line 85 "xb.isa"
	sink
#line 502 "xb.cc"
	);
private:
};

class OpXb_accD_offset_idx_ind : public Operation
{
public:
	OpXb_accD_offset_idx_ind(CodeType const& code, uint16_t addr);
	uint32_t rr;
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
	virtual
#line 80 "xb.isa"
	uint8_t
#line 548 "xb.cc"
	getXbMode();
	virtual
#line 89 "xb.isa"
	uint16_t
#line 553 "xb.cc"
	getEAddr(
#line 89 "xb.isa"
	CPU *
#line 557 "xb.cc"
#line 89 "xb.isa"
	cpu
#line 560 "xb.cc"
	);
	virtual
#line 85 "xb.isa"
	void
#line 565 "xb.cc"
	disasm(
#line 85 "xb.isa"
	ostream&
#line 569 "xb.cc"
#line 85 "xb.isa"
	sink
#line 572 "xb.cc"
	);
private:
};

#line 80 "xb.isa"
uint8_t
#line 579 "xb.cc"
OpXb_auto::getXbMode()
{
#line 169 "xb.isa"
	{ return XbModes::IDX; }
#line 584 "xb.cc"
}
#line 89 "xb.isa"
uint16_t
#line 588 "xb.cc"
OpXb_auto::getEAddr(
#line 89 "xb.isa"
CPU *
#line 592 "xb.cc"
#line 89 "xb.isa"
cpu
#line 595 "xb.cc"
)
{
#line 171 "xb.isa"
	{

		address_t regOldVal = cpu->xb_getAddrRegValue(rr);
		address_t regNewVal;

		short val = nnnn;
		// if (val > 7) then decrement else increment
		if (val < 8) // increment index register
		{
			regNewVal = regOldVal + val + 1;
			} else { // decrement index register
			regNewVal = (int16_t) regOldVal + val - 16;
		}

		cpu->xb_setAddrRegValue(rr,regNewVal);
		if (p == 0) {
			// Auto pre-
			return regNewVal;
			} else {
			// Auto post-
			return regOldVal;
		}
	}
#line 622 "xb.cc"
}
#line 85 "xb.isa"
void
#line 626 "xb.cc"
OpXb_auto::disasm(
#line 85 "xb.isa"
ostream&
#line 630 "xb.cc"
#line 85 "xb.isa"
sink
#line 633 "xb.cc"
)
{
#line 195 "xb.isa"
	{

		string	regLabel = CPU::xb_getAddrRegLabel(rr);
		char	sign = '+';
		short  val = nnnn;

		if (val > 7) {
			val = val - 16;
			} else {
			val = val + 1;
		}

		if (val < 0) {
			sign = '-';
			val = -1*val;
		}

		sink << std::dec << val << ",";
		if (p == 0) {
			sink << sign << regLabel;
			} else {
			sink << regLabel << sign;
		}
	}
#line 661 "xb.cc"
}

static Operation *DecodeOpXb_auto(CodeType const& code, uint16_t addr)
{
	return new OpXb_auto(code, addr);
}

#line 80 "xb.isa"
uint8_t
#line 671 "xb.cc"
OpXb_5bit_cst::getXbMode()
{
#line 94 "xb.isa"
	{ return XbModes::IDX; }
#line 676 "xb.cc"
}
#line 89 "xb.isa"
uint16_t
#line 680 "xb.cc"
OpXb_5bit_cst::getEAddr(
#line 89 "xb.isa"
CPU *
#line 684 "xb.cc"
#line 89 "xb.isa"
cpu
#line 687 "xb.cc"
)
{
#line 96 "xb.isa"
	{

		address_t addr = (int16_t) cpu->xb_getAddrRegValue(rr) + nnnnn;

		return addr;

		//    return cpu->xb_getAddrRegValue(rr) + nnnnn;
	}
#line 699 "xb.cc"
}
#line 85 "xb.isa"
void
#line 703 "xb.cc"
OpXb_5bit_cst::disasm(
#line 85 "xb.isa"
ostream&
#line 707 "xb.cc"
#line 85 "xb.isa"
sink
#line 710 "xb.cc"
)
{
#line 105 "xb.isa"
	{
		sink << std::dec << nnnnn << "," << CPU::xb_getAddrRegLabel(rr);
	}
#line 717 "xb.cc"
}

static Operation *DecodeOpXb_5bit_cst(CodeType const& code, uint16_t addr)
{
	return new OpXb_5bit_cst(code, addr);
}

// TODO: 20080417 - a revoir
#line 80 "xb.isa"
uint8_t
#line 728 "xb.cc"
OpXb_9_cst_z0::getXbMode()
{
#line 113 "xb.isa"
	{ return XbModes::IDX1; }
#line 733 "xb.cc"
}
#line 89 "xb.isa"
uint16_t
#line 737 "xb.cc"
OpXb_9_cst_z0::getEAddr(
#line 89 "xb.isa"
CPU *
#line 741 "xb.cc"
#line 89 "xb.isa"
cpu
#line 744 "xb.cc"
)
{
#line 115 "xb.isa"
	{

		address_t addr = (int16_t) cpu->xb_getAddrRegValue(rr) + n8;

		return addr;

		//	return cpu->xb_getAddrRegValue(rr) + n8;
	}
#line 756 "xb.cc"
}
#line 85 "xb.isa"
void
#line 760 "xb.cc"
OpXb_9_cst_z0::disasm(
#line 85 "xb.isa"
ostream&
#line 764 "xb.cc"
#line 85 "xb.isa"
sink
#line 767 "xb.cc"
)
{
#line 124 "xb.isa"
	{

		sink << std::dec << n8 << "," << CPU::xb_getAddrRegLabel(rr);
	}
#line 775 "xb.cc"
}

static Operation *DecodeOpXb_9_cst_z0(CodeType const& code, uint16_t addr)
{
	return new OpXb_9_cst_z0(code, addr);
}

#line 80 "xb.isa"
uint8_t
#line 785 "xb.cc"
OpXb_16bit_cst_z1_s0::getXbMode()
{
#line 132 "xb.isa"
	{ return XbModes::IDX2; }
#line 790 "xb.cc"
}
#line 89 "xb.isa"
uint16_t
#line 794 "xb.cc"
OpXb_16bit_cst_z1_s0::getEAddr(
#line 89 "xb.isa"
CPU *
#line 798 "xb.cc"
#line 89 "xb.isa"
cpu
#line 801 "xb.cc"
)
{
#line 134 "xb.isa"
	{

		physical_address_t addr = cpu->mmc->getPhysicalAddress(cpu->xb_getAddrRegValue(rr) + n16, MEMORY::EXTENDED, WO_GLOBAL_ADDRESS);

		// Constant offset 16-bit signed
		return addr;
	}
#line 812 "xb.cc"
}
#line 85 "xb.isa"
void
#line 816 "xb.cc"
OpXb_16bit_cst_z1_s0::disasm(
#line 85 "xb.isa"
ostream&
#line 820 "xb.cc"
#line 85 "xb.isa"
sink
#line 823 "xb.cc"
)
{
#line 142 "xb.isa"
	{

		// constant offset 16-bit signed
		sink << std::dec << n16 << "," << CPU::xb_getAddrRegLabel(rr);
	}
#line 832 "xb.cc"
}

static Operation *DecodeOpXb_16bit_cst_z1_s0(CodeType const& code, uint16_t addr)
{
	return new OpXb_16bit_cst_z1_s0(code, addr);
}

#line 80 "xb.isa"
uint8_t
#line 842 "xb.cc"
OpXb_16bit_cst_z1_s1::getXbMode()
{
#line 150 "xb.isa"
	{ return XbModes::IDX2_Ind; }
#line 847 "xb.cc"
}
#line 89 "xb.isa"
uint16_t
#line 851 "xb.cc"
OpXb_16bit_cst_z1_s1::getEAddr(
#line 89 "xb.isa"
CPU *
#line 855 "xb.cc"
#line 89 "xb.isa"
cpu
#line 858 "xb.cc"
)
{
#line 152 "xb.isa"
	{

		physical_address_t addr = cpu->mmc->getPhysicalAddress(cpu->xb_getAddrRegValue(rr) + n16, MEMORY::EXTENDED, WO_GLOBAL_ADDRESS);

		// 16-bit offset indexed-indirect
		return cpu->memRead16(addr);
	}
#line 869 "xb.cc"
}
#line 85 "xb.isa"
void
#line 873 "xb.cc"
OpXb_16bit_cst_z1_s1::disasm(
#line 85 "xb.isa"
ostream&
#line 877 "xb.cc"
#line 85 "xb.isa"
sink
#line 880 "xb.cc"
)
{
#line 160 "xb.isa"
	{

		// 16-bit offset index-indirect
		sink << "[" << std::dec << n16 << "," << CPU::xb_getAddrRegLabel(rr) << "]";
	}
#line 889 "xb.cc"
}

static Operation *DecodeOpXb_16bit_cst_z1_s1(CodeType const& code, uint16_t addr)
{
	return new OpXb_16bit_cst_z1_s1(code, addr);
}

#line 80 "xb.isa"
uint8_t
#line 899 "xb.cc"
OpXb_acc_offset::getXbMode()
{
#line 223 "xb.isa"
	{ return XbModes::IDX; }
#line 904 "xb.cc"
}
#line 89 "xb.isa"
uint16_t
#line 908 "xb.cc"
OpXb_acc_offset::getEAddr(
#line 89 "xb.isa"
CPU *
#line 912 "xb.cc"
#line 89 "xb.isa"
cpu
#line 915 "xb.cc"
)
{
#line 225 "xb.isa"
	{
		uint16_t addrRegVal = cpu->xb_getAddrRegValue(rr);
		uint16_t accRegVal = cpu->xb_getAccRegValue(aa);

		return addrRegVal + accRegVal;
	}
#line 925 "xb.cc"
}
#line 85 "xb.isa"
void
#line 929 "xb.cc"
OpXb_acc_offset::disasm(
#line 85 "xb.isa"
ostream&
#line 933 "xb.cc"
#line 85 "xb.isa"
sink
#line 936 "xb.cc"
)
{
#line 232 "xb.isa"
	{
		sink << CPU::xb_getAccRegLabel(aa) << "," << CPU::xb_getAddrRegLabel(rr);
	}
#line 943 "xb.cc"
}

static Operation *DecodeOpXb_acc_offset(CodeType const& code, uint16_t addr)
{
	return new OpXb_acc_offset(code, addr);
}

#line 80 "xb.isa"
uint8_t
#line 953 "xb.cc"
OpXb_accD_offset_idx_ind::getXbMode()
{
#line 239 "xb.isa"
	{ return XbModes::IDXD_Ind; }
#line 958 "xb.cc"
}
#line 89 "xb.isa"
uint16_t
#line 962 "xb.cc"
OpXb_accD_offset_idx_ind::getEAddr(
#line 89 "xb.isa"
CPU *
#line 966 "xb.cc"
#line 89 "xb.isa"
cpu
#line 969 "xb.cc"
)
{
#line 241 "xb.isa"
	{

		return cpu->memRead16(cpu->mmc->getPhysicalAddress(cpu->getRegD() + cpu->xb_getAddrRegValue(rr), MEMORY::EXTENDED, WO_GLOBAL_ADDRESS));
	}
#line 977 "xb.cc"
}
#line 85 "xb.isa"
void
#line 981 "xb.cc"
OpXb_accD_offset_idx_ind::disasm(
#line 85 "xb.isa"
ostream&
#line 985 "xb.cc"
#line 85 "xb.isa"
sink
#line 988 "xb.cc"
)
{
#line 246 "xb.isa"
	{
		sink << "[D," << CPU::xb_getAddrRegLabel(rr) << "]";
	}
#line 995 "xb.cc"
}

static Operation *DecodeOpXb_accD_offset_idx_ind(CodeType const& code, uint16_t addr)
{
	return new OpXb_accD_offset_idx_ind(code, addr);
}

OpXb_auto::OpXb_auto(CodeType const& code, uint16_t addr) : Operation(code, addr, "xb_auto")
{
	CodeType _code_( code );
	{
		uint8_t _subword_ = (uint8_t( _code_.str[0] ) << 0);
		rr = ((_subword_ >> 6) & 0x3ULL);
		p = ((_subword_ >> 4) & 0x1ULL);
		nnnn = ((_subword_ >> 0) & 0xfULL);
	}
	_code_.pop( 1 );
	this->encoding.size -= _code_.size;
}

OpXb_5bit_cst::OpXb_5bit_cst(CodeType const& code, uint16_t addr) : Operation(code, addr, "xb_5bit_cst")
{
	CodeType _code_( code );
	{
		uint8_t _subword_ = (uint8_t( _code_.str[0] ) << 0);
		rr = ((_subword_ >> 6) & 0x3ULL);
		nnnnn = (((int32_t((_subword_ >> 0) & 0x1fLL)) << 27) >> 27);
	}
	_code_.pop( 1 );
	this->encoding.size -= _code_.size;
}

OpXb_9_cst_z0::OpXb_9_cst_z0(CodeType const& code, uint16_t addr) : Operation(code, addr, "xb_9_cst_z0")
{
	CodeType _code_( code );
	{
		uint8_t _subword_ = (uint8_t( _code_.str[0] ) << 0);
		rr = ((_subword_ >> 3) & 0x3ULL);
		s = ((_subword_ >> 0) & 0x1ULL);
	}
	_code_.pop( 1 );
	{
		uint8_t _subword_ = (uint8_t( _code_.str[0] ) << 0);
		n8 = (((int32_t((_subword_ >> 0) & 0xffLL)) << 24) >> 24);
	}
	_code_.pop( 1 );
	this->encoding.size -= _code_.size;
}

OpXb_16bit_cst_z1_s0::OpXb_16bit_cst_z1_s0(CodeType const& code, uint16_t addr) : Operation(code, addr, "xb_16bit_cst_z1_s0")
{
	CodeType _code_( code );
	{
		uint8_t _subword_ = (uint8_t( _code_.str[0] ) << 0);
		rr = ((_subword_ >> 3) & 0x3ULL);
	}
	_code_.pop( 1 );
	{
		uint16_t _subword_ = (uint16_t( _code_.str[0] ) << 8) | (uint16_t( _code_.str[1] ) << 0);
		n16 = (((int32_t((_subword_ >> 0) & 0xffffLL)) << 16) >> 16);
	}
	_code_.pop( 2 );
	this->encoding.size -= _code_.size;
}

OpXb_16bit_cst_z1_s1::OpXb_16bit_cst_z1_s1(CodeType const& code, uint16_t addr) : Operation(code, addr, "xb_16bit_cst_z1_s1")
{
	CodeType _code_( code );
	{
		uint8_t _subword_ = (uint8_t( _code_.str[0] ) << 0);
		rr = ((_subword_ >> 3) & 0x3ULL);
	}
	_code_.pop( 1 );
	{
		uint16_t _subword_ = (uint16_t( _code_.str[0] ) << 8) | (uint16_t( _code_.str[1] ) << 0);
		n16 = (((int32_t((_subword_ >> 0) & 0xffffLL)) << 16) >> 16);
	}
	_code_.pop( 2 );
	this->encoding.size -= _code_.size;
}

OpXb_acc_offset::OpXb_acc_offset(CodeType const& code, uint16_t addr) : Operation(code, addr, "xb_acc_offset")
{
	CodeType _code_( code );
	{
		uint8_t _subword_ = (uint8_t( _code_.str[0] ) << 0);
		rr = ((_subword_ >> 3) & 0x3ULL);
		aa = ((_subword_ >> 0) & 0x3ULL);
	}
	_code_.pop( 1 );
	this->encoding.size -= _code_.size;
}

OpXb_accD_offset_idx_ind::OpXb_accD_offset_idx_ind(CodeType const& code, uint16_t addr) : Operation(code, addr, "xb_accD_offset_idx_ind")
{
	CodeType _code_( code );
	{
		uint8_t _subword_ = (uint8_t( _code_.str[0] ) << 0);
		rr = ((_subword_ >> 3) & 0x3ULL);
	}
	_code_.pop( 1 );
	this->encoding.size -= _code_.size;
}

DecodeTableEntry::DecodeTableEntry(CodeType const& opcode, CodeType const& opcode_mask, Operation *(*decode)(CodeType const&, uint16_t))
{
	this->opcode = opcode;
	this->opcode_mask = opcode_mask;
	this->decode = decode;
}

Decoder::Decoder()
{
	little_endian = false;
	decode_table.push_back(DecodeTableEntry(CodeType( (uint8_t*)( "\xe7" ), 1 ), CodeType( (uint8_t*)( "\xe7" ), 1 ), DecodeOpXb_accD_offset_idx_ind));
	decode_table.push_back(DecodeTableEntry(CodeType( (uint8_t*)( "\xe4" ), 1 ), CodeType( (uint8_t*)( "\xe4" ), 1 ), DecodeOpXb_acc_offset));
	decode_table.push_back(DecodeTableEntry(CodeType( (uint8_t*)( "\xe3" ), 1 ), CodeType( (uint8_t*)( "\xe7" ), 1 ), DecodeOpXb_16bit_cst_z1_s1));
	decode_table.push_back(DecodeTableEntry(CodeType( (uint8_t*)( "\xe2" ), 1 ), CodeType( (uint8_t*)( "\xe7" ), 1 ), DecodeOpXb_16bit_cst_z1_s0));
	decode_table.push_back(DecodeTableEntry(CodeType( (uint8_t*)( "\xe0" ), 1 ), CodeType( (uint8_t*)( "\xe6" ), 1 ), DecodeOpXb_9_cst_z0));
	decode_table.push_back(DecodeTableEntry(CodeType( (uint8_t*)( "\x00" ), 1 ), CodeType( (uint8_t*)( "\x20" ), 1 ), DecodeOpXb_5bit_cst));
	decode_table.push_back(DecodeTableEntry(CodeType( (uint8_t*)( "\x20" ), 1 ), CodeType( (uint8_t*)( "\x20" ), 1 ), DecodeOpXb_auto));
}

Decoder::~Decoder()
{
}

Operation *Decoder::NCDecode(uint16_t addr, CodeType const& code)
{
	Operation *operation;
	unsigned int count = decode_table.size();
	unsigned int idx;
	for(idx = 0; idx < count; idx++)
	{
		if( code.match( decode_table[idx].opcode, decode_table[idx].opcode_mask) )
		{
			operation = decode_table[idx].decode(code, addr);
			return operation;
		}
	}
	operation = new Operation(code, addr, "???");
	return operation;
}

void Decoder::SetLittleEndian()
{
	little_endian = true;
}

void Decoder::SetBigEndian()
{
	little_endian = false;
}

} } } } } }
