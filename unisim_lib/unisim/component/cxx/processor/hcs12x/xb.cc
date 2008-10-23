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

#line 131 "xb.isa"
void
#line 36 "xb.cc"
Operation::post_execute(
#line 131 "xb.isa"
ostream&
#line 40 "xb.cc"
#line 131 "xb.isa"
sink
#line 43 "xb.cc"
)
{
#line 131 "xb.isa"
	{

		vector<string> vect;
		getRegsLabel(vect);

		vector<string>::iterator iter1 = vect.end();

		while( iter1 != vect.begin()) {
			iter1--;

			if (((string) *iter1).compare("X") == 0) {
				sink << "asm(\"" << "PULX \");\n";
			}
			if (((string) *iter1).compare("Y") == 0) {
				sink << "asm(\"" << "PULY \");\n";
			}
			if (((string) *iter1).compare("SP") == 0) {
				//			sink << "asm(\"" << "STX SP \");\n";
			}
			if (((string) *iter1).compare("PC") == 0) {
				//			sink << "asm(\"" << "JMP Y \");\n";
			}
			if (((string) *iter1).compare("A") == 0) {
				sink << "asm(\"" << "PULA \");\n";
			}
			if (((string) *iter1).compare("B") == 0) {
				sink << "asm(\"" << "PULB \");\n";
			}
			if (((string) *iter1).compare("D") == 0) {
				sink << "asm(\"" << "PULD \");\n";
			}
		}

	}
#line 81 "xb.cc"
}
#line 95 "xb.isa"
void
#line 85 "xb.cc"
Operation::pre_execute(
#line 95 "xb.isa"
ostream&
#line 89 "xb.cc"
#line 95 "xb.isa"
sink
#line 92 "xb.cc"
)
{
#line 95 "xb.isa"
	{

		vector<string> vect;
		getRegsLabel(vect);

		vector<string>::iterator iter1 = vect.begin();

		while( iter1 != vect.end()) {

			if (((string) *iter1).compare("X") == 0) {
				sink << "asm(\"" << "PSHX \");\n";
			}
			if (((string) *iter1).compare("Y") == 0) {
				sink << "asm(\"" << "PSHY \");\n";
			}
			if (((string) *iter1).compare("SP") == 0) {
				//			sink << "asm(\"" << "LDX SP \");\n";
			}
			if (((string) *iter1).compare("PC") == 0) {
				//			sink << "asm(\"" << "LDY PC \");\n";
			}
			if (((string) *iter1).compare("A") == 0) {
				sink << "asm(\"" << "PSHA \");\n";
			}
			if (((string) *iter1).compare("B") == 0) {
				sink << "asm(\"" << "PSHB \");\n";
			}
			if (((string) *iter1).compare("D") == 0) {
				sink << "asm(\"" << "PSHD \");\n";
			}

			iter1++;
		}

	}
#line 131 "xb.cc"
}
#line 92 "xb.isa"
void
#line 135 "xb.cc"
Operation::getRegsLabel(
#line 92 "xb.isa"
vector<string>
#line 139 "xb.cc"
#line 92 "xb.isa"
&vect
#line 142 "xb.cc"
)
{
#line 92 "xb.isa"
	{ return; }
#line 147 "xb.cc"
}
#line 90 "xb.isa"
uint16_t
#line 151 "xb.cc"
Operation::getEAddr(
#line 90 "xb.isa"
CPU *
#line 155 "xb.cc"
#line 90 "xb.isa"
cpu
#line 158 "xb.cc"
)
{
#line 90 "xb.isa"
	{ assert( false ); return 0; }
#line 163 "xb.cc"
}
#line 86 "xb.isa"
void
#line 167 "xb.cc"
Operation::disasm(
#line 86 "xb.isa"
ostream&
#line 171 "xb.cc"
#line 86 "xb.isa"
sink
#line 174 "xb.cc"
)
{
#line 86 "xb.isa"
	{
		sink << "?";
	}
#line 181 "xb.cc"
}
#line 81 "xb.isa"
uint8_t
#line 185 "xb.cc"
Operation::getXbMode()
{
#line 82 "xb.isa"
	{
		return XbModes::Unknown;
	}
#line 192 "xb.cc"
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
#line 237 "xb.cc"
	getXbMode();
	virtual
#line 90 "xb.isa"
	uint16_t
#line 242 "xb.cc"
	getEAddr(
#line 90 "xb.isa"
	CPU *
#line 246 "xb.cc"
#line 90 "xb.isa"
	cpu
#line 249 "xb.cc"
	);
	virtual
#line 86 "xb.isa"
	void
#line 254 "xb.cc"
	disasm(
#line 86 "xb.isa"
	ostream&
#line 258 "xb.cc"
#line 86 "xb.isa"
	sink
#line 261 "xb.cc"
	);
	virtual
#line 92 "xb.isa"
	void
#line 266 "xb.cc"
	getRegsLabel(
#line 92 "xb.isa"
	vector<string>
#line 270 "xb.cc"
#line 92 "xb.isa"
	&vect
#line 273 "xb.cc"
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
#line 320 "xb.cc"
	getXbMode();
	virtual
#line 90 "xb.isa"
	uint16_t
#line 325 "xb.cc"
	getEAddr(
#line 90 "xb.isa"
	CPU *
#line 329 "xb.cc"
#line 90 "xb.isa"
	cpu
#line 332 "xb.cc"
	);
	virtual
#line 86 "xb.isa"
	void
#line 337 "xb.cc"
	disasm(
#line 86 "xb.isa"
	ostream&
#line 341 "xb.cc"
#line 86 "xb.isa"
	sink
#line 344 "xb.cc"
	);
	virtual
#line 92 "xb.isa"
	void
#line 349 "xb.cc"
	getRegsLabel(
#line 92 "xb.isa"
	vector<string>
#line 353 "xb.cc"
#line 92 "xb.isa"
	&vect
#line 356 "xb.cc"
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
#line 404 "xb.cc"
	getXbMode();
	virtual
#line 90 "xb.isa"
	uint16_t
#line 409 "xb.cc"
	getEAddr(
#line 90 "xb.isa"
	CPU *
#line 413 "xb.cc"
#line 90 "xb.isa"
	cpu
#line 416 "xb.cc"
	);
	virtual
#line 86 "xb.isa"
	void
#line 421 "xb.cc"
	disasm(
#line 86 "xb.isa"
	ostream&
#line 425 "xb.cc"
#line 86 "xb.isa"
	sink
#line 428 "xb.cc"
	);
	virtual
#line 92 "xb.isa"
	void
#line 433 "xb.cc"
	getRegsLabel(
#line 92 "xb.isa"
	vector<string>
#line 437 "xb.cc"
#line 92 "xb.isa"
	&vect
#line 440 "xb.cc"
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
#line 487 "xb.cc"
	getXbMode();
	virtual
#line 90 "xb.isa"
	uint16_t
#line 492 "xb.cc"
	getEAddr(
#line 90 "xb.isa"
	CPU *
#line 496 "xb.cc"
#line 90 "xb.isa"
	cpu
#line 499 "xb.cc"
	);
	virtual
#line 86 "xb.isa"
	void
#line 504 "xb.cc"
	disasm(
#line 86 "xb.isa"
	ostream&
#line 508 "xb.cc"
#line 86 "xb.isa"
	sink
#line 511 "xb.cc"
	);
	virtual
#line 92 "xb.isa"
	void
#line 516 "xb.cc"
	getRegsLabel(
#line 92 "xb.isa"
	vector<string>
#line 520 "xb.cc"
#line 92 "xb.isa"
	&vect
#line 523 "xb.cc"
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
#line 570 "xb.cc"
	getXbMode();
	virtual
#line 90 "xb.isa"
	uint16_t
#line 575 "xb.cc"
	getEAddr(
#line 90 "xb.isa"
	CPU *
#line 579 "xb.cc"
#line 90 "xb.isa"
	cpu
#line 582 "xb.cc"
	);
	virtual
#line 86 "xb.isa"
	void
#line 587 "xb.cc"
	disasm(
#line 86 "xb.isa"
	ostream&
#line 591 "xb.cc"
#line 86 "xb.isa"
	sink
#line 594 "xb.cc"
	);
	virtual
#line 92 "xb.isa"
	void
#line 599 "xb.cc"
	getRegsLabel(
#line 92 "xb.isa"
	vector<string>
#line 603 "xb.cc"
#line 92 "xb.isa"
	&vect
#line 606 "xb.cc"
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
#line 653 "xb.cc"
	getXbMode();
	virtual
#line 90 "xb.isa"
	uint16_t
#line 658 "xb.cc"
	getEAddr(
#line 90 "xb.isa"
	CPU *
#line 662 "xb.cc"
#line 90 "xb.isa"
	cpu
#line 665 "xb.cc"
	);
	virtual
#line 86 "xb.isa"
	void
#line 670 "xb.cc"
	disasm(
#line 86 "xb.isa"
	ostream&
#line 674 "xb.cc"
#line 86 "xb.isa"
	sink
#line 677 "xb.cc"
	);
	virtual
#line 92 "xb.isa"
	void
#line 682 "xb.cc"
	getRegsLabel(
#line 92 "xb.isa"
	vector<string>
#line 686 "xb.cc"
#line 92 "xb.isa"
	&vect
#line 689 "xb.cc"
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
#line 735 "xb.cc"
	getXbMode();
	virtual
#line 90 "xb.isa"
	uint16_t
#line 740 "xb.cc"
	getEAddr(
#line 90 "xb.isa"
	CPU *
#line 744 "xb.cc"
#line 90 "xb.isa"
	cpu
#line 747 "xb.cc"
	);
	virtual
#line 86 "xb.isa"
	void
#line 752 "xb.cc"
	disasm(
#line 86 "xb.isa"
	ostream&
#line 756 "xb.cc"
#line 86 "xb.isa"
	sink
#line 759 "xb.cc"
	);
	virtual
#line 92 "xb.isa"
	void
#line 764 "xb.cc"
	getRegsLabel(
#line 92 "xb.isa"
	vector<string>
#line 768 "xb.cc"
#line 92 "xb.isa"
	&vect
#line 771 "xb.cc"
	);
private:
};

#line 81 "xb.isa"
uint8_t
#line 778 "xb.cc"
OpXb_auto::getXbMode()
{
#line 270 "xb.isa"
	{ return XbModes::IDX; }
#line 783 "xb.cc"
}
#line 90 "xb.isa"
uint16_t
#line 787 "xb.cc"
OpXb_auto::getEAddr(
#line 90 "xb.isa"
CPU *
#line 791 "xb.cc"
#line 90 "xb.isa"
cpu
#line 794 "xb.cc"
)
{
#line 272 "xb.isa"
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
#line 821 "xb.cc"
}
#line 86 "xb.isa"
void
#line 825 "xb.cc"
OpXb_auto::disasm(
#line 86 "xb.isa"
ostream&
#line 829 "xb.cc"
#line 86 "xb.isa"
sink
#line 832 "xb.cc"
)
{
#line 296 "xb.isa"
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
#line 860 "xb.cc"
}
#line 92 "xb.isa"
void
#line 864 "xb.cc"
OpXb_auto::getRegsLabel(
#line 92 "xb.isa"
vector<string>
#line 868 "xb.cc"
#line 92 "xb.isa"
&vect
#line 871 "xb.cc"
)
{
#line 321 "xb.isa"
	{

		string	regLabel(CPU::xb_getAddrRegLabel(rr));
		vect.push_back(regLabel);
	}
#line 880 "xb.cc"
}

static Operation *DecodeOpXb_auto(CodeType const& code, uint16_t addr)
{
	return new OpXb_auto(code, addr);
}

#line 81 "xb.isa"
uint8_t
#line 890 "xb.cc"
OpXb_5bit_cst::getXbMode()
{
#line 169 "xb.isa"
	{ return XbModes::IDX; }
#line 895 "xb.cc"
}
#line 90 "xb.isa"
uint16_t
#line 899 "xb.cc"
OpXb_5bit_cst::getEAddr(
#line 90 "xb.isa"
CPU *
#line 903 "xb.cc"
#line 90 "xb.isa"
cpu
#line 906 "xb.cc"
)
{
#line 171 "xb.isa"
	{

		address_t addr = (int16_t) cpu->xb_getAddrRegValue(rr) + nnnnn;

		return addr;

	}
#line 917 "xb.cc"
}
#line 86 "xb.isa"
void
#line 921 "xb.cc"
OpXb_5bit_cst::disasm(
#line 86 "xb.isa"
ostream&
#line 925 "xb.cc"
#line 86 "xb.isa"
sink
#line 928 "xb.cc"
)
{
#line 179 "xb.isa"
	{

		string regLabel = CPU::xb_getAddrRegLabel(rr);

		sink << std::dec << nnnnn << "," << regLabel;

	}
#line 939 "xb.cc"
}
#line 92 "xb.isa"
void
#line 943 "xb.cc"
OpXb_5bit_cst::getRegsLabel(
#line 92 "xb.isa"
vector<string>
#line 947 "xb.cc"
#line 92 "xb.isa"
&vect
#line 950 "xb.cc"
)
{
#line 187 "xb.isa"
	{

		string regLabel(CPU::xb_getAddrRegLabel(rr));
		vect.push_back(regLabel);
	}
#line 959 "xb.cc"
}

static Operation *DecodeOpXb_5bit_cst(CodeType const& code, uint16_t addr)
{
	return new OpXb_5bit_cst(code, addr);
}

// TODO: 20080417 - a revoir
#line 81 "xb.isa"
uint8_t
#line 970 "xb.cc"
OpXb_9_cst_z0::getXbMode()
{
#line 196 "xb.isa"
	{ return XbModes::IDX1; }
#line 975 "xb.cc"
}
#line 90 "xb.isa"
uint16_t
#line 979 "xb.cc"
OpXb_9_cst_z0::getEAddr(
#line 90 "xb.isa"
CPU *
#line 983 "xb.cc"
#line 90 "xb.isa"
cpu
#line 986 "xb.cc"
)
{
#line 198 "xb.isa"
	{

		address_t addr = (int16_t) cpu->xb_getAddrRegValue(rr) + n8;

		return addr;

	}
#line 997 "xb.cc"
}
#line 86 "xb.isa"
void
#line 1001 "xb.cc"
OpXb_9_cst_z0::disasm(
#line 86 "xb.isa"
ostream&
#line 1005 "xb.cc"
#line 86 "xb.isa"
sink
#line 1008 "xb.cc"
)
{
#line 206 "xb.isa"
	{

		string regLabel = CPU::xb_getAddrRegLabel(rr);
		sink << std::dec << n8 << "," << regLabel;
	}
#line 1017 "xb.cc"
}
#line 92 "xb.isa"
void
#line 1021 "xb.cc"
OpXb_9_cst_z0::getRegsLabel(
#line 92 "xb.isa"
vector<string>
#line 1025 "xb.cc"
#line 92 "xb.isa"
&vect
#line 1028 "xb.cc"
)
{
#line 212 "xb.isa"
	{

		string regLabel(CPU::xb_getAddrRegLabel(rr));
		vect.push_back(regLabel);
	}
#line 1037 "xb.cc"
}

static Operation *DecodeOpXb_9_cst_z0(CodeType const& code, uint16_t addr)
{
	return new OpXb_9_cst_z0(code, addr);
}

#line 81 "xb.isa"
uint8_t
#line 1047 "xb.cc"
OpXb_16bit_cst_z1_s0::getXbMode()
{
#line 220 "xb.isa"
	{ return XbModes::IDX2; }
#line 1052 "xb.cc"
}
#line 90 "xb.isa"
uint16_t
#line 1056 "xb.cc"
OpXb_16bit_cst_z1_s0::getEAddr(
#line 90 "xb.isa"
CPU *
#line 1060 "xb.cc"
#line 90 "xb.isa"
cpu
#line 1063 "xb.cc"
)
{
#line 222 "xb.isa"
	{

		physical_address_t addr = cpu->xb_getAddrRegValue(rr) + n16;

		// Constant offset 16-bit signed
		return addr;
	}
#line 1074 "xb.cc"
}
#line 86 "xb.isa"
void
#line 1078 "xb.cc"
OpXb_16bit_cst_z1_s0::disasm(
#line 86 "xb.isa"
ostream&
#line 1082 "xb.cc"
#line 86 "xb.isa"
sink
#line 1085 "xb.cc"
)
{
#line 230 "xb.isa"
	{

		// constant offset 16-bit signed
		string regLabel = CPU::xb_getAddrRegLabel(rr);
		sink << std::dec << n16 << "," << regLabel;
	}
#line 1095 "xb.cc"
}
#line 92 "xb.isa"
void
#line 1099 "xb.cc"
OpXb_16bit_cst_z1_s0::getRegsLabel(
#line 92 "xb.isa"
vector<string>
#line 1103 "xb.cc"
#line 92 "xb.isa"
&vect
#line 1106 "xb.cc"
)
{
#line 237 "xb.isa"
	{

		string regLabel(CPU::xb_getAddrRegLabel(rr));
		vect.push_back(regLabel);
	}
#line 1115 "xb.cc"
}

static Operation *DecodeOpXb_16bit_cst_z1_s0(CodeType const& code, uint16_t addr)
{
	return new OpXb_16bit_cst_z1_s0(code, addr);
}

#line 81 "xb.isa"
uint8_t
#line 1125 "xb.cc"
OpXb_16bit_cst_z1_s1::getXbMode()
{
#line 245 "xb.isa"
	{ return XbModes::IDX2_Ind; }
#line 1130 "xb.cc"
}
#line 90 "xb.isa"
uint16_t
#line 1134 "xb.cc"
OpXb_16bit_cst_z1_s1::getEAddr(
#line 90 "xb.isa"
CPU *
#line 1138 "xb.cc"
#line 90 "xb.isa"
cpu
#line 1141 "xb.cc"
)
{
#line 247 "xb.isa"
	{

		physical_address_t addr = cpu->xb_getAddrRegValue(rr) + n16;

		// 16-bit offset indexed-indirect
		return cpu->memRead16(addr);
	}
#line 1152 "xb.cc"
}
#line 86 "xb.isa"
void
#line 1156 "xb.cc"
OpXb_16bit_cst_z1_s1::disasm(
#line 86 "xb.isa"
ostream&
#line 1160 "xb.cc"
#line 86 "xb.isa"
sink
#line 1163 "xb.cc"
)
{
#line 255 "xb.isa"
	{

		// 16-bit offset index-indirect
		string regLabel = CPU::xb_getAddrRegLabel(rr);
		sink << "[" << std::dec << n16 << "," << regLabel << "]";
	}
#line 1173 "xb.cc"
}
#line 92 "xb.isa"
void
#line 1177 "xb.cc"
OpXb_16bit_cst_z1_s1::getRegsLabel(
#line 92 "xb.isa"
vector<string>
#line 1181 "xb.cc"
#line 92 "xb.isa"
&vect
#line 1184 "xb.cc"
)
{
#line 262 "xb.isa"
	{

		string regLabel(CPU::xb_getAddrRegLabel(rr));
		vect.push_back(regLabel);
	}
#line 1193 "xb.cc"
}

static Operation *DecodeOpXb_16bit_cst_z1_s1(CodeType const& code, uint16_t addr)
{
	return new OpXb_16bit_cst_z1_s1(code, addr);
}

#line 81 "xb.isa"
uint8_t
#line 1203 "xb.cc"
OpXb_acc_offset::getXbMode()
{
#line 329 "xb.isa"
	{ return XbModes::IDX; }
#line 1208 "xb.cc"
}
#line 90 "xb.isa"
uint16_t
#line 1212 "xb.cc"
OpXb_acc_offset::getEAddr(
#line 90 "xb.isa"
CPU *
#line 1216 "xb.cc"
#line 90 "xb.isa"
cpu
#line 1219 "xb.cc"
)
{
#line 331 "xb.isa"
	{
		uint16_t addrRegVal = cpu->xb_getAddrRegValue(rr);
		uint16_t accRegVal = cpu->xb_getAccRegValue(aa);

		return addrRegVal + accRegVal;
	}
#line 1229 "xb.cc"
}
#line 86 "xb.isa"
void
#line 1233 "xb.cc"
OpXb_acc_offset::disasm(
#line 86 "xb.isa"
ostream&
#line 1237 "xb.cc"
#line 86 "xb.isa"
sink
#line 1240 "xb.cc"
)
{
#line 338 "xb.isa"
	{

		string addrRegLabel = CPU::xb_getAddrRegLabel(rr);
		string accRegLabel = CPU::xb_getAccRegLabel(aa);

		sink << accRegLabel << "," << addrRegLabel;
	}
#line 1251 "xb.cc"
}
#line 92 "xb.isa"
void
#line 1255 "xb.cc"
OpXb_acc_offset::getRegsLabel(
#line 92 "xb.isa"
vector<string>
#line 1259 "xb.cc"
#line 92 "xb.isa"
&vect
#line 1262 "xb.cc"
)
{
#line 346 "xb.isa"
	{

		string addrRegLabel(CPU::xb_getAddrRegLabel(rr));
		string accRegLabel(CPU::xb_getAccRegLabel(aa));

		vect.push_back(accRegLabel);
		vect.push_back(addrRegLabel);
	}
#line 1274 "xb.cc"
}

static Operation *DecodeOpXb_acc_offset(CodeType const& code, uint16_t addr)
{
	return new OpXb_acc_offset(code, addr);
}

#line 81 "xb.isa"
uint8_t
#line 1284 "xb.cc"
OpXb_accD_offset_idx_ind::getXbMode()
{
#line 357 "xb.isa"
	{ return XbModes::IDXD_Ind; }
#line 1289 "xb.cc"
}
#line 90 "xb.isa"
uint16_t
#line 1293 "xb.cc"
OpXb_accD_offset_idx_ind::getEAddr(
#line 90 "xb.isa"
CPU *
#line 1297 "xb.cc"
#line 90 "xb.isa"
cpu
#line 1300 "xb.cc"
)
{
#line 359 "xb.isa"
	{

		return cpu->memRead16(cpu->getRegD() + cpu->xb_getAddrRegValue(rr));
	}
#line 1308 "xb.cc"
}
#line 86 "xb.isa"
void
#line 1312 "xb.cc"
OpXb_accD_offset_idx_ind::disasm(
#line 86 "xb.isa"
ostream&
#line 1316 "xb.cc"
#line 86 "xb.isa"
sink
#line 1319 "xb.cc"
)
{
#line 364 "xb.isa"
	{
		string regLabel = CPU::xb_getAddrRegLabel(rr);
		sink << "[D," << regLabel << "]";
	}
#line 1327 "xb.cc"
}
#line 92 "xb.isa"
void
#line 1331 "xb.cc"
OpXb_accD_offset_idx_ind::getRegsLabel(
#line 92 "xb.isa"
vector<string>
#line 1335 "xb.cc"
#line 92 "xb.isa"
&vect
#line 1338 "xb.cc"
)
{
#line 369 "xb.isa"
	{
		string regLabel(CPU::xb_getAddrRegLabel(rr));
		vect.push_back(regLabel);
	}
#line 1346 "xb.cc"
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
