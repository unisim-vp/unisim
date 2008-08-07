#include "xb.hh"
#include <cassert>
#include <ostream>
#include <cstring>
#line 76 "xb.isa"

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

#line 109 "xb.isa"
void
#line 36 "xb.cc"
Operation::post_execute(
#line 109 "xb.isa"
ostream&
#line 40 "xb.cc"
#line 109 "xb.isa"
sink
#line 43 "xb.cc"
)
{
#line 109 "xb.isa"
	{

		vector<string> vect;
		getRegsLabel(vect);

		vector<string>::iterator iter1 = vect.end();

		while( iter1 != vect.begin()) {
			iter1--;
			sink << "asm(\"" << "PUL " << *iter1 << "\");\n";
		}

	}
#line 60 "xb.cc"
}
#line 95 "xb.isa"
void
#line 64 "xb.cc"
Operation::pre_execute(
#line 95 "xb.isa"
ostream&
#line 68 "xb.cc"
#line 95 "xb.isa"
sink
#line 71 "xb.cc"
)
{
#line 95 "xb.isa"
	{

		vector<string> vect;
		getRegsLabel(vect);

		vector<string>::iterator iter1 = vect.begin();

		while( iter1 != vect.end()) {
			sink << "asm(\"" << "PUSH " << *iter1 << "\");\n";
			iter1++;
		}

	}
#line 88 "xb.cc"
}
#line 92 "xb.isa"
void
#line 92 "xb.cc"
Operation::getRegsLabel(
#line 92 "xb.isa"
vector<string>
#line 96 "xb.cc"
#line 92 "xb.isa"
&vect
#line 99 "xb.cc"
)
{
#line 92 "xb.isa"
	{ return; }
#line 104 "xb.cc"
}
#line 90 "xb.isa"
uint16_t
#line 108 "xb.cc"
Operation::getEAddr(
#line 90 "xb.isa"
CPU *
#line 112 "xb.cc"
#line 90 "xb.isa"
cpu
#line 115 "xb.cc"
)
{
#line 90 "xb.isa"
	{ assert( false ); return 0; }
#line 120 "xb.cc"
}
#line 86 "xb.isa"
void
#line 124 "xb.cc"
Operation::disasm(
#line 86 "xb.isa"
ostream&
#line 128 "xb.cc"
#line 86 "xb.isa"
sink
#line 131 "xb.cc"
)
{
#line 86 "xb.isa"
	{
		sink << "?";
	}
#line 138 "xb.cc"
}
#line 81 "xb.isa"
uint8_t
#line 142 "xb.cc"
Operation::getXbMode()
{
#line 82 "xb.isa"
	{
		return XbModes::Unknown;
	}
#line 149 "xb.cc"
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
#line 81 "xb.isa"
	uint8_t
#line 194 "xb.cc"
	getXbMode();
	virtual
#line 90 "xb.isa"
	uint16_t
#line 199 "xb.cc"
	getEAddr(
#line 90 "xb.isa"
	CPU *
#line 203 "xb.cc"
#line 90 "xb.isa"
	cpu
#line 206 "xb.cc"
	);
	virtual
#line 86 "xb.isa"
	void
#line 211 "xb.cc"
	disasm(
#line 86 "xb.isa"
	ostream&
#line 215 "xb.cc"
#line 86 "xb.isa"
	sink
#line 218 "xb.cc"
	);
	virtual
#line 92 "xb.isa"
	void
#line 223 "xb.cc"
	getRegsLabel(
#line 92 "xb.isa"
	vector<string>
#line 227 "xb.cc"
#line 92 "xb.isa"
	&vect
#line 230 "xb.cc"
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
#line 81 "xb.isa"
	uint8_t
#line 277 "xb.cc"
	getXbMode();
	virtual
#line 90 "xb.isa"
	uint16_t
#line 282 "xb.cc"
	getEAddr(
#line 90 "xb.isa"
	CPU *
#line 286 "xb.cc"
#line 90 "xb.isa"
	cpu
#line 289 "xb.cc"
	);
	virtual
#line 86 "xb.isa"
	void
#line 294 "xb.cc"
	disasm(
#line 86 "xb.isa"
	ostream&
#line 298 "xb.cc"
#line 86 "xb.isa"
	sink
#line 301 "xb.cc"
	);
	virtual
#line 92 "xb.isa"
	void
#line 306 "xb.cc"
	getRegsLabel(
#line 92 "xb.isa"
	vector<string>
#line 310 "xb.cc"
#line 92 "xb.isa"
	&vect
#line 313 "xb.cc"
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
#line 81 "xb.isa"
	uint8_t
#line 361 "xb.cc"
	getXbMode();
	virtual
#line 90 "xb.isa"
	uint16_t
#line 366 "xb.cc"
	getEAddr(
#line 90 "xb.isa"
	CPU *
#line 370 "xb.cc"
#line 90 "xb.isa"
	cpu
#line 373 "xb.cc"
	);
	virtual
#line 86 "xb.isa"
	void
#line 378 "xb.cc"
	disasm(
#line 86 "xb.isa"
	ostream&
#line 382 "xb.cc"
#line 86 "xb.isa"
	sink
#line 385 "xb.cc"
	);
	virtual
#line 92 "xb.isa"
	void
#line 390 "xb.cc"
	getRegsLabel(
#line 92 "xb.isa"
	vector<string>
#line 394 "xb.cc"
#line 92 "xb.isa"
	&vect
#line 397 "xb.cc"
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
#line 81 "xb.isa"
	uint8_t
#line 444 "xb.cc"
	getXbMode();
	virtual
#line 90 "xb.isa"
	uint16_t
#line 449 "xb.cc"
	getEAddr(
#line 90 "xb.isa"
	CPU *
#line 453 "xb.cc"
#line 90 "xb.isa"
	cpu
#line 456 "xb.cc"
	);
	virtual
#line 86 "xb.isa"
	void
#line 461 "xb.cc"
	disasm(
#line 86 "xb.isa"
	ostream&
#line 465 "xb.cc"
#line 86 "xb.isa"
	sink
#line 468 "xb.cc"
	);
	virtual
#line 92 "xb.isa"
	void
#line 473 "xb.cc"
	getRegsLabel(
#line 92 "xb.isa"
	vector<string>
#line 477 "xb.cc"
#line 92 "xb.isa"
	&vect
#line 480 "xb.cc"
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
#line 81 "xb.isa"
	uint8_t
#line 527 "xb.cc"
	getXbMode();
	virtual
#line 90 "xb.isa"
	uint16_t
#line 532 "xb.cc"
	getEAddr(
#line 90 "xb.isa"
	CPU *
#line 536 "xb.cc"
#line 90 "xb.isa"
	cpu
#line 539 "xb.cc"
	);
	virtual
#line 86 "xb.isa"
	void
#line 544 "xb.cc"
	disasm(
#line 86 "xb.isa"
	ostream&
#line 548 "xb.cc"
#line 86 "xb.isa"
	sink
#line 551 "xb.cc"
	);
	virtual
#line 92 "xb.isa"
	void
#line 556 "xb.cc"
	getRegsLabel(
#line 92 "xb.isa"
	vector<string>
#line 560 "xb.cc"
#line 92 "xb.isa"
	&vect
#line 563 "xb.cc"
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
#line 81 "xb.isa"
	uint8_t
#line 610 "xb.cc"
	getXbMode();
	virtual
#line 90 "xb.isa"
	uint16_t
#line 615 "xb.cc"
	getEAddr(
#line 90 "xb.isa"
	CPU *
#line 619 "xb.cc"
#line 90 "xb.isa"
	cpu
#line 622 "xb.cc"
	);
	virtual
#line 86 "xb.isa"
	void
#line 627 "xb.cc"
	disasm(
#line 86 "xb.isa"
	ostream&
#line 631 "xb.cc"
#line 86 "xb.isa"
	sink
#line 634 "xb.cc"
	);
	virtual
#line 92 "xb.isa"
	void
#line 639 "xb.cc"
	getRegsLabel(
#line 92 "xb.isa"
	vector<string>
#line 643 "xb.cc"
#line 92 "xb.isa"
	&vect
#line 646 "xb.cc"
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
#line 81 "xb.isa"
	uint8_t
#line 692 "xb.cc"
	getXbMode();
	virtual
#line 90 "xb.isa"
	uint16_t
#line 697 "xb.cc"
	getEAddr(
#line 90 "xb.isa"
	CPU *
#line 701 "xb.cc"
#line 90 "xb.isa"
	cpu
#line 704 "xb.cc"
	);
	virtual
#line 86 "xb.isa"
	void
#line 709 "xb.cc"
	disasm(
#line 86 "xb.isa"
	ostream&
#line 713 "xb.cc"
#line 86 "xb.isa"
	sink
#line 716 "xb.cc"
	);
	virtual
#line 92 "xb.isa"
	void
#line 721 "xb.cc"
	getRegsLabel(
#line 92 "xb.isa"
	vector<string>
#line 725 "xb.cc"
#line 92 "xb.isa"
	&vect
#line 728 "xb.cc"
	);
private:
};

#line 81 "xb.isa"
uint8_t
#line 735 "xb.cc"
OpXb_auto::getXbMode()
{
#line 227 "xb.isa"
	{ return XbModes::IDX; }
#line 740 "xb.cc"
}
#line 90 "xb.isa"
uint16_t
#line 744 "xb.cc"
OpXb_auto::getEAddr(
#line 90 "xb.isa"
CPU *
#line 748 "xb.cc"
#line 90 "xb.isa"
cpu
#line 751 "xb.cc"
)
{
#line 229 "xb.isa"
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
#line 778 "xb.cc"
}
#line 86 "xb.isa"
void
#line 782 "xb.cc"
OpXb_auto::disasm(
#line 86 "xb.isa"
ostream&
#line 786 "xb.cc"
#line 86 "xb.isa"
sink
#line 789 "xb.cc"
)
{
#line 253 "xb.isa"
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
#line 817 "xb.cc"
}
#line 92 "xb.isa"
void
#line 821 "xb.cc"
OpXb_auto::getRegsLabel(
#line 92 "xb.isa"
vector<string>
#line 825 "xb.cc"
#line 92 "xb.isa"
&vect
#line 828 "xb.cc"
)
{
#line 278 "xb.isa"
	{

		string	regLabel(CPU::xb_getAddrRegLabel(rr));
		vect.push_back(regLabel);
	}
#line 837 "xb.cc"
}

static Operation *DecodeOpXb_auto(CodeType const& code, uint16_t addr)
{
	return new OpXb_auto(code, addr);
}

#line 81 "xb.isa"
uint8_t
#line 847 "xb.cc"
OpXb_5bit_cst::getXbMode()
{
#line 126 "xb.isa"
	{ return XbModes::IDX; }
#line 852 "xb.cc"
}
#line 90 "xb.isa"
uint16_t
#line 856 "xb.cc"
OpXb_5bit_cst::getEAddr(
#line 90 "xb.isa"
CPU *
#line 860 "xb.cc"
#line 90 "xb.isa"
cpu
#line 863 "xb.cc"
)
{
#line 128 "xb.isa"
	{

		address_t addr = (int16_t) cpu->xb_getAddrRegValue(rr) + nnnnn;

		return addr;

	}
#line 874 "xb.cc"
}
#line 86 "xb.isa"
void
#line 878 "xb.cc"
OpXb_5bit_cst::disasm(
#line 86 "xb.isa"
ostream&
#line 882 "xb.cc"
#line 86 "xb.isa"
sink
#line 885 "xb.cc"
)
{
#line 136 "xb.isa"
	{

		string regLabel = CPU::xb_getAddrRegLabel(rr);

		sink << std::dec << nnnnn << "," << regLabel;

	}
#line 896 "xb.cc"
}
#line 92 "xb.isa"
void
#line 900 "xb.cc"
OpXb_5bit_cst::getRegsLabel(
#line 92 "xb.isa"
vector<string>
#line 904 "xb.cc"
#line 92 "xb.isa"
&vect
#line 907 "xb.cc"
)
{
#line 144 "xb.isa"
	{

		string regLabel(CPU::xb_getAddrRegLabel(rr));
		vect.push_back(regLabel);
	}
#line 916 "xb.cc"
}

static Operation *DecodeOpXb_5bit_cst(CodeType const& code, uint16_t addr)
{
	return new OpXb_5bit_cst(code, addr);
}

// TODO: 20080417 - a revoir
#line 81 "xb.isa"
uint8_t
#line 927 "xb.cc"
OpXb_9_cst_z0::getXbMode()
{
#line 153 "xb.isa"
	{ return XbModes::IDX1; }
#line 932 "xb.cc"
}
#line 90 "xb.isa"
uint16_t
#line 936 "xb.cc"
OpXb_9_cst_z0::getEAddr(
#line 90 "xb.isa"
CPU *
#line 940 "xb.cc"
#line 90 "xb.isa"
cpu
#line 943 "xb.cc"
)
{
#line 155 "xb.isa"
	{

		address_t addr = (int16_t) cpu->xb_getAddrRegValue(rr) + n8;

		return addr;

	}
#line 954 "xb.cc"
}
#line 86 "xb.isa"
void
#line 958 "xb.cc"
OpXb_9_cst_z0::disasm(
#line 86 "xb.isa"
ostream&
#line 962 "xb.cc"
#line 86 "xb.isa"
sink
#line 965 "xb.cc"
)
{
#line 163 "xb.isa"
	{

		string regLabel = CPU::xb_getAddrRegLabel(rr);
		sink << std::dec << n8 << "," << regLabel;
	}
#line 974 "xb.cc"
}
#line 92 "xb.isa"
void
#line 978 "xb.cc"
OpXb_9_cst_z0::getRegsLabel(
#line 92 "xb.isa"
vector<string>
#line 982 "xb.cc"
#line 92 "xb.isa"
&vect
#line 985 "xb.cc"
)
{
#line 169 "xb.isa"
	{

		string regLabel(CPU::xb_getAddrRegLabel(rr));
		vect.push_back(regLabel);
	}
#line 994 "xb.cc"
}

static Operation *DecodeOpXb_9_cst_z0(CodeType const& code, uint16_t addr)
{
	return new OpXb_9_cst_z0(code, addr);
}

#line 81 "xb.isa"
uint8_t
#line 1004 "xb.cc"
OpXb_16bit_cst_z1_s0::getXbMode()
{
#line 177 "xb.isa"
	{ return XbModes::IDX2; }
#line 1009 "xb.cc"
}
#line 90 "xb.isa"
uint16_t
#line 1013 "xb.cc"
OpXb_16bit_cst_z1_s0::getEAddr(
#line 90 "xb.isa"
CPU *
#line 1017 "xb.cc"
#line 90 "xb.isa"
cpu
#line 1020 "xb.cc"
)
{
#line 179 "xb.isa"
	{

		physical_address_t addr = cpu->mmc->getPhysicalAddress(cpu->xb_getAddrRegValue(rr) + n16, MEMORY::EXTENDED, WO_GLOBAL_ADDRESS);

		// Constant offset 16-bit signed
		return addr;
	}
#line 1031 "xb.cc"
}
#line 86 "xb.isa"
void
#line 1035 "xb.cc"
OpXb_16bit_cst_z1_s0::disasm(
#line 86 "xb.isa"
ostream&
#line 1039 "xb.cc"
#line 86 "xb.isa"
sink
#line 1042 "xb.cc"
)
{
#line 187 "xb.isa"
	{

		// constant offset 16-bit signed
		string regLabel = CPU::xb_getAddrRegLabel(rr);
		sink << std::dec << n16 << "," << regLabel;
	}
#line 1052 "xb.cc"
}
#line 92 "xb.isa"
void
#line 1056 "xb.cc"
OpXb_16bit_cst_z1_s0::getRegsLabel(
#line 92 "xb.isa"
vector<string>
#line 1060 "xb.cc"
#line 92 "xb.isa"
&vect
#line 1063 "xb.cc"
)
{
#line 194 "xb.isa"
	{

		string regLabel(CPU::xb_getAddrRegLabel(rr));
		vect.push_back(regLabel);
	}
#line 1072 "xb.cc"
}

static Operation *DecodeOpXb_16bit_cst_z1_s0(CodeType const& code, uint16_t addr)
{
	return new OpXb_16bit_cst_z1_s0(code, addr);
}

#line 81 "xb.isa"
uint8_t
#line 1082 "xb.cc"
OpXb_16bit_cst_z1_s1::getXbMode()
{
#line 202 "xb.isa"
	{ return XbModes::IDX2_Ind; }
#line 1087 "xb.cc"
}
#line 90 "xb.isa"
uint16_t
#line 1091 "xb.cc"
OpXb_16bit_cst_z1_s1::getEAddr(
#line 90 "xb.isa"
CPU *
#line 1095 "xb.cc"
#line 90 "xb.isa"
cpu
#line 1098 "xb.cc"
)
{
#line 204 "xb.isa"
	{

		physical_address_t addr = cpu->mmc->getPhysicalAddress(cpu->xb_getAddrRegValue(rr) + n16, MEMORY::EXTENDED, WO_GLOBAL_ADDRESS);

		// 16-bit offset indexed-indirect
		return cpu->memRead16(addr);
	}
#line 1109 "xb.cc"
}
#line 86 "xb.isa"
void
#line 1113 "xb.cc"
OpXb_16bit_cst_z1_s1::disasm(
#line 86 "xb.isa"
ostream&
#line 1117 "xb.cc"
#line 86 "xb.isa"
sink
#line 1120 "xb.cc"
)
{
#line 212 "xb.isa"
	{

		// 16-bit offset index-indirect
		string regLabel = CPU::xb_getAddrRegLabel(rr);
		sink << "[" << std::dec << n16 << "," << regLabel << "]";
	}
#line 1130 "xb.cc"
}
#line 92 "xb.isa"
void
#line 1134 "xb.cc"
OpXb_16bit_cst_z1_s1::getRegsLabel(
#line 92 "xb.isa"
vector<string>
#line 1138 "xb.cc"
#line 92 "xb.isa"
&vect
#line 1141 "xb.cc"
)
{
#line 219 "xb.isa"
	{

		string regLabel(CPU::xb_getAddrRegLabel(rr));
		vect.push_back(regLabel);
	}
#line 1150 "xb.cc"
}

static Operation *DecodeOpXb_16bit_cst_z1_s1(CodeType const& code, uint16_t addr)
{
	return new OpXb_16bit_cst_z1_s1(code, addr);
}

#line 81 "xb.isa"
uint8_t
#line 1160 "xb.cc"
OpXb_acc_offset::getXbMode()
{
#line 286 "xb.isa"
	{ return XbModes::IDX; }
#line 1165 "xb.cc"
}
#line 90 "xb.isa"
uint16_t
#line 1169 "xb.cc"
OpXb_acc_offset::getEAddr(
#line 90 "xb.isa"
CPU *
#line 1173 "xb.cc"
#line 90 "xb.isa"
cpu
#line 1176 "xb.cc"
)
{
#line 288 "xb.isa"
	{
		uint16_t addrRegVal = cpu->xb_getAddrRegValue(rr);
		uint16_t accRegVal = cpu->xb_getAccRegValue(aa);

		return addrRegVal + accRegVal;
	}
#line 1186 "xb.cc"
}
#line 86 "xb.isa"
void
#line 1190 "xb.cc"
OpXb_acc_offset::disasm(
#line 86 "xb.isa"
ostream&
#line 1194 "xb.cc"
#line 86 "xb.isa"
sink
#line 1197 "xb.cc"
)
{
#line 295 "xb.isa"
	{

		string addrRegLabel = CPU::xb_getAddrRegLabel(rr);
		string accRegLabel = CPU::xb_getAccRegLabel(aa);

		sink << accRegLabel << "," << addrRegLabel;
	}
#line 1208 "xb.cc"
}
#line 92 "xb.isa"
void
#line 1212 "xb.cc"
OpXb_acc_offset::getRegsLabel(
#line 92 "xb.isa"
vector<string>
#line 1216 "xb.cc"
#line 92 "xb.isa"
&vect
#line 1219 "xb.cc"
)
{
#line 303 "xb.isa"
	{

		string addrRegLabel(CPU::xb_getAddrRegLabel(rr));
		string accRegLabel(CPU::xb_getAccRegLabel(aa));

		vect.push_back(accRegLabel);
		vect.push_back(addrRegLabel);
	}
#line 1231 "xb.cc"
}

static Operation *DecodeOpXb_acc_offset(CodeType const& code, uint16_t addr)
{
	return new OpXb_acc_offset(code, addr);
}

#line 81 "xb.isa"
uint8_t
#line 1241 "xb.cc"
OpXb_accD_offset_idx_ind::getXbMode()
{
#line 314 "xb.isa"
	{ return XbModes::IDXD_Ind; }
#line 1246 "xb.cc"
}
#line 90 "xb.isa"
uint16_t
#line 1250 "xb.cc"
OpXb_accD_offset_idx_ind::getEAddr(
#line 90 "xb.isa"
CPU *
#line 1254 "xb.cc"
#line 90 "xb.isa"
cpu
#line 1257 "xb.cc"
)
{
#line 316 "xb.isa"
	{

		return cpu->memRead16(cpu->mmc->getPhysicalAddress(cpu->getRegD() + cpu->xb_getAddrRegValue(rr), MEMORY::EXTENDED, WO_GLOBAL_ADDRESS));
	}
#line 1265 "xb.cc"
}
#line 86 "xb.isa"
void
#line 1269 "xb.cc"
OpXb_accD_offset_idx_ind::disasm(
#line 86 "xb.isa"
ostream&
#line 1273 "xb.cc"
#line 86 "xb.isa"
sink
#line 1276 "xb.cc"
)
{
#line 321 "xb.isa"
	{
		string regLabel = CPU::xb_getAddrRegLabel(rr);
		sink << "[D," << regLabel << "]";
	}
#line 1284 "xb.cc"
}
#line 92 "xb.isa"
void
#line 1288 "xb.cc"
OpXb_accD_offset_idx_ind::getRegsLabel(
#line 92 "xb.isa"
vector<string>
#line 1292 "xb.cc"
#line 92 "xb.isa"
&vect
#line 1295 "xb.cc"
)
{
#line 326 "xb.isa"
	{
		string regLabel(CPU::xb_getAddrRegLabel(rr));
		vect.push_back(regLabel);
	}
#line 1303 "xb.cc"
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
