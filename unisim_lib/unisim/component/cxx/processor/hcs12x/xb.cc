#include "xb.hh"
#include <cassert>
#include <ostream>
#include <cstring>
#line 81 "xb.isa"

using namespace std;

#line 10 "xb.cc"
namespace unisim { namespace component { namespace cxx { namespace processor { namespace hcs12x { namespace XB {
unsigned int const CodeType::capacity;
std::ostream& operator << ( std::ostream& _sink, CodeType const& _ct ) {
	if (_ct.size        ) {
		for (unsigned int idx = 0; idx < _ct.size; ++idx ) {
			_sink << (((_ct.str[idx/8] >> ((7-idx)       )) & 1) ? '1' : '0');  }
		} else {
		char const* xrepr = "0123456789abcdef";
		for (unsigned int idx = 0; idx < _ct.size/8; ++idx ) {
			uint8_t byte = _ct.str[idx/8];
			_sink << xrepr[(byte >> 4) & 0xf] << xrepr[(byte >> 0) & 0xf];
		}
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

#line 136 "xb.isa"
void
#line 40 "xb.cc"
Operation::post_execute(
#line 136 "xb.isa"
ostream&
#line 44 "xb.cc"
#line 136 "xb.isa"
sink
#line 47 "xb.cc"
)
{
#line 136 "xb.isa"
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
#line 85 "xb.cc"
}
#line 100 "xb.isa"
void
#line 89 "xb.cc"
Operation::pre_execute(
#line 100 "xb.isa"
ostream&
#line 93 "xb.cc"
#line 100 "xb.isa"
sink
#line 96 "xb.cc"
)
{
#line 100 "xb.isa"
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
#line 135 "xb.cc"
}
#line 97 "xb.isa"
void
#line 139 "xb.cc"
Operation::getRegsLabel(
#line 97 "xb.isa"
vector<string>
#line 143 "xb.cc"
#line 97 "xb.isa"
&vect
#line 146 "xb.cc"
)
{
#line 97 "xb.isa"
	{ return; }
#line 151 "xb.cc"
}
#line 95 "xb.isa"
uint16_t
#line 155 "xb.cc"
Operation::getEAddr(
#line 95 "xb.isa"
CPU *
#line 159 "xb.cc"
#line 95 "xb.isa"
cpu
#line 162 "xb.cc"
)
{
#line 95 "xb.isa"
	{ assert( false ); return 0; }
#line 167 "xb.cc"
}
#line 91 "xb.isa"
void
#line 171 "xb.cc"
Operation::disasm(
#line 91 "xb.isa"
ostream&
#line 175 "xb.cc"
#line 91 "xb.isa"
sink
#line 178 "xb.cc"
)
{
#line 91 "xb.isa"
	{
		sink << "?";
	}
#line 185 "xb.cc"
}
#line 86 "xb.isa"
uint8_t
#line 189 "xb.cc"
Operation::getXbMode()
{
#line 87 "xb.isa"
	{
		return XbModes::Unknown;
	}
#line 196 "xb.cc"
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
	// decoder( cisc, sub )
	// address {uint16_t}
	// big_endian
	virtual
#line 86 "xb.isa"
	uint8_t
#line 244 "xb.cc"
	getXbMode();
	virtual
#line 95 "xb.isa"
	uint16_t
#line 249 "xb.cc"
	getEAddr(
#line 95 "xb.isa"
	CPU *
#line 253 "xb.cc"
#line 95 "xb.isa"
	cpu
#line 256 "xb.cc"
	);
	virtual
#line 91 "xb.isa"
	void
#line 261 "xb.cc"
	disasm(
#line 91 "xb.isa"
	ostream&
#line 265 "xb.cc"
#line 91 "xb.isa"
	sink
#line 268 "xb.cc"
	);
	virtual
#line 97 "xb.isa"
	void
#line 273 "xb.cc"
	getRegsLabel(
#line 97 "xb.isa"
	vector<string>
#line 277 "xb.cc"
#line 97 "xb.isa"
	&vect
#line 280 "xb.cc"
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
	// decoder( cisc, sub )
	// address {uint16_t}
	// big_endian
	virtual
#line 86 "xb.isa"
	uint8_t
#line 330 "xb.cc"
	getXbMode();
	virtual
#line 95 "xb.isa"
	uint16_t
#line 335 "xb.cc"
	getEAddr(
#line 95 "xb.isa"
	CPU *
#line 339 "xb.cc"
#line 95 "xb.isa"
	cpu
#line 342 "xb.cc"
	);
	virtual
#line 91 "xb.isa"
	void
#line 347 "xb.cc"
	disasm(
#line 91 "xb.isa"
	ostream&
#line 351 "xb.cc"
#line 91 "xb.isa"
	sink
#line 354 "xb.cc"
	);
	virtual
#line 97 "xb.isa"
	void
#line 359 "xb.cc"
	getRegsLabel(
#line 97 "xb.isa"
	vector<string>
#line 363 "xb.cc"
#line 97 "xb.isa"
	&vect
#line 366 "xb.cc"
	);
private:
};

class OpXb_9_cst_z0 : public Operation
{
public:
	OpXb_9_cst_z0(CodeType const& code, uint16_t addr);
	uint32_t rr;
	uint32_t s;
	uint32_t n8;
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
	// decoder( cisc, sub )
	// address {uint16_t}
	// big_endian
	virtual
#line 86 "xb.isa"
	uint8_t
#line 417 "xb.cc"
	getXbMode();
	virtual
#line 95 "xb.isa"
	uint16_t
#line 422 "xb.cc"
	getEAddr(
#line 95 "xb.isa"
	CPU *
#line 426 "xb.cc"
#line 95 "xb.isa"
	cpu
#line 429 "xb.cc"
	);
	virtual
#line 91 "xb.isa"
	void
#line 434 "xb.cc"
	disasm(
#line 91 "xb.isa"
	ostream&
#line 438 "xb.cc"
#line 91 "xb.isa"
	sink
#line 441 "xb.cc"
	);
	virtual
#line 97 "xb.isa"
	void
#line 446 "xb.cc"
	getRegsLabel(
#line 97 "xb.isa"
	vector<string>
#line 450 "xb.cc"
#line 97 "xb.isa"
	&vect
#line 453 "xb.cc"
	);
private:
};

class OpXb_16bit_cst_z1_s0 : public Operation
{
public:
	OpXb_16bit_cst_z1_s0(CodeType const& code, uint16_t addr);
	uint32_t rr;
	uint32_t n16;
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
	// decoder( cisc, sub )
	// address {uint16_t}
	// big_endian
	virtual
#line 86 "xb.isa"
	uint8_t
#line 503 "xb.cc"
	getXbMode();
	virtual
#line 95 "xb.isa"
	uint16_t
#line 508 "xb.cc"
	getEAddr(
#line 95 "xb.isa"
	CPU *
#line 512 "xb.cc"
#line 95 "xb.isa"
	cpu
#line 515 "xb.cc"
	);
	virtual
#line 91 "xb.isa"
	void
#line 520 "xb.cc"
	disasm(
#line 91 "xb.isa"
	ostream&
#line 524 "xb.cc"
#line 91 "xb.isa"
	sink
#line 527 "xb.cc"
	);
	virtual
#line 97 "xb.isa"
	void
#line 532 "xb.cc"
	getRegsLabel(
#line 97 "xb.isa"
	vector<string>
#line 536 "xb.cc"
#line 97 "xb.isa"
	&vect
#line 539 "xb.cc"
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
	// decoder( cisc, sub )
	// address {uint16_t}
	// big_endian
	virtual
#line 86 "xb.isa"
	uint8_t
#line 589 "xb.cc"
	getXbMode();
	virtual
#line 95 "xb.isa"
	uint16_t
#line 594 "xb.cc"
	getEAddr(
#line 95 "xb.isa"
	CPU *
#line 598 "xb.cc"
#line 95 "xb.isa"
	cpu
#line 601 "xb.cc"
	);
	virtual
#line 91 "xb.isa"
	void
#line 606 "xb.cc"
	disasm(
#line 91 "xb.isa"
	ostream&
#line 610 "xb.cc"
#line 91 "xb.isa"
	sink
#line 613 "xb.cc"
	);
	virtual
#line 97 "xb.isa"
	void
#line 618 "xb.cc"
	getRegsLabel(
#line 97 "xb.isa"
	vector<string>
#line 622 "xb.cc"
#line 97 "xb.isa"
	&vect
#line 625 "xb.cc"
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
	// decoder( cisc, sub )
	// address {uint16_t}
	// big_endian
	virtual
#line 86 "xb.isa"
	uint8_t
#line 675 "xb.cc"
	getXbMode();
	virtual
#line 95 "xb.isa"
	uint16_t
#line 680 "xb.cc"
	getEAddr(
#line 95 "xb.isa"
	CPU *
#line 684 "xb.cc"
#line 95 "xb.isa"
	cpu
#line 687 "xb.cc"
	);
	virtual
#line 91 "xb.isa"
	void
#line 692 "xb.cc"
	disasm(
#line 91 "xb.isa"
	ostream&
#line 696 "xb.cc"
#line 91 "xb.isa"
	sink
#line 699 "xb.cc"
	);
	virtual
#line 97 "xb.isa"
	void
#line 704 "xb.cc"
	getRegsLabel(
#line 97 "xb.isa"
	vector<string>
#line 708 "xb.cc"
#line 97 "xb.isa"
	&vect
#line 711 "xb.cc"
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
	// decoder( cisc, sub )
	// address {uint16_t}
	// big_endian
	virtual
#line 86 "xb.isa"
	uint8_t
#line 760 "xb.cc"
	getXbMode();
	virtual
#line 95 "xb.isa"
	uint16_t
#line 765 "xb.cc"
	getEAddr(
#line 95 "xb.isa"
	CPU *
#line 769 "xb.cc"
#line 95 "xb.isa"
	cpu
#line 772 "xb.cc"
	);
	virtual
#line 91 "xb.isa"
	void
#line 777 "xb.cc"
	disasm(
#line 91 "xb.isa"
	ostream&
#line 781 "xb.cc"
#line 91 "xb.isa"
	sink
#line 784 "xb.cc"
	);
	virtual
#line 97 "xb.isa"
	void
#line 789 "xb.cc"
	getRegsLabel(
#line 97 "xb.isa"
	vector<string>
#line 793 "xb.cc"
#line 97 "xb.isa"
	&vect
#line 796 "xb.cc"
	);
private:
};

#line 86 "xb.isa"
uint8_t
#line 803 "xb.cc"
OpXb_auto::getXbMode()
{
#line 300 "xb.isa"
	{ return XbModes::IDX; }
#line 808 "xb.cc"
}
#line 95 "xb.isa"
uint16_t
#line 812 "xb.cc"
OpXb_auto::getEAddr(
#line 95 "xb.isa"
CPU *
#line 816 "xb.cc"
#line 95 "xb.isa"
cpu
#line 819 "xb.cc"
)
{
#line 302 "xb.isa"
	{

		address_t regOldVal = cpu->xb_getAddrRegValue(rr);
		address_t regNewVal;

		short val = nnnn;
		// if (val > 7) then decrement else increment
		if (val < 8) // increment index register
		{
			regNewVal = regOldVal + val + 1;
			} else { // decrement index register
			regNewVal = regOldVal + val - 16;
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
#line 848 "xb.cc"
}
#line 91 "xb.isa"
void
#line 852 "xb.cc"
OpXb_auto::disasm(
#line 91 "xb.isa"
ostream&
#line 856 "xb.cc"
#line 91 "xb.isa"
sink
#line 859 "xb.cc"
)
{
#line 328 "xb.isa"
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
#line 887 "xb.cc"
}
#line 97 "xb.isa"
void
#line 891 "xb.cc"
OpXb_auto::getRegsLabel(
#line 97 "xb.isa"
vector<string>
#line 895 "xb.cc"
#line 97 "xb.isa"
&vect
#line 898 "xb.cc"
)
{
#line 353 "xb.isa"
	{

		string	regLabel(CPU::xb_getAddrRegLabel(rr));
		vect.push_back(regLabel);
	}
#line 907 "xb.cc"
}

static Operation *DecodeOpXb_auto(CodeType const& code, uint16_t addr)
{
	return new OpXb_auto(code, addr);
}

#line 86 "xb.isa"
uint8_t
#line 917 "xb.cc"
OpXb_5bit_cst::getXbMode()
{
#line 174 "xb.isa"
	{ return XbModes::IDX; }
#line 922 "xb.cc"
}
#line 95 "xb.isa"
uint16_t
#line 926 "xb.cc"
OpXb_5bit_cst::getEAddr(
#line 95 "xb.isa"
CPU *
#line 930 "xb.cc"
#line 95 "xb.isa"
cpu
#line 933 "xb.cc"
)
{
#line 176 "xb.isa"
	{

		address_t addr = cpu->xb_getAddrRegValue(rr) + nnnnn;

		return addr;

	}
#line 944 "xb.cc"
}
#line 91 "xb.isa"
void
#line 948 "xb.cc"
OpXb_5bit_cst::disasm(
#line 91 "xb.isa"
ostream&
#line 952 "xb.cc"
#line 91 "xb.isa"
sink
#line 955 "xb.cc"
)
{
#line 184 "xb.isa"
	{

		string regLabel = CPU::xb_getAddrRegLabel(rr);

		sink << std::dec << nnnnn << "," << regLabel;

	}
#line 966 "xb.cc"
}
#line 97 "xb.isa"
void
#line 970 "xb.cc"
OpXb_5bit_cst::getRegsLabel(
#line 97 "xb.isa"
vector<string>
#line 974 "xb.cc"
#line 97 "xb.isa"
&vect
#line 977 "xb.cc"
)
{
#line 192 "xb.isa"
	{

		string regLabel(CPU::xb_getAddrRegLabel(rr));
		vect.push_back(regLabel);
	}
#line 986 "xb.cc"
}

static Operation *DecodeOpXb_5bit_cst(CodeType const& code, uint16_t addr)
{
	return new OpXb_5bit_cst(code, addr);
}

#line 86 "xb.isa"
uint8_t
#line 996 "xb.cc"
OpXb_9_cst_z0::getXbMode()
{
#line 200 "xb.isa"
	{ return XbModes::IDX1; }
#line 1001 "xb.cc"
}
#line 95 "xb.isa"
uint16_t
#line 1005 "xb.cc"
OpXb_9_cst_z0::getEAddr(
#line 95 "xb.isa"
CPU *
#line 1009 "xb.cc"
#line 95 "xb.isa"
cpu
#line 1012 "xb.cc"
)
{
#line 202 "xb.isa"
	{

		address_t addr;
		/*
		if (s==0) {
			addr = cpu->xb_getAddrRegValue(rr) + (n8 & 0xFF);
			} else {
			addr = cpu->xb_getAddrRegValue(rr) + (n8 & 0xFF) - 256;
		}
		*/
		int32_t val = (int32_t) ((n8 << 24) >> 24);
		if (s==0) {
			addr = cpu->xb_getAddrRegValue(rr) + val;
			} else {
			addr = cpu->xb_getAddrRegValue(rr) + val - 256;
		}

		return addr;

	}
#line 1036 "xb.cc"
}
#line 91 "xb.isa"
void
#line 1040 "xb.cc"
OpXb_9_cst_z0::disasm(
#line 91 "xb.isa"
ostream&
#line 1044 "xb.cc"
#line 91 "xb.isa"
sink
#line 1047 "xb.cc"
)
{
#line 223 "xb.isa"
	{

		string regLabel = CPU::xb_getAddrRegLabel(rr);
		/*
		if (s==0) {
			sink << std::dec << (n8 & 0xFF) << "," << regLabel;
			} else {
			sink << std::dec << (n8 & 0xFF) - 256 << "," << regLabel;
		}
		*/
		int32_t val = (int32_t) ((n8 << 24) >> 24);
		if (s==0) {
			sink << std::dec << val << "," << regLabel;
			} else {
			sink << std::dec << val - 256 << "," << regLabel;
		}

	}
#line 1069 "xb.cc"
}
#line 97 "xb.isa"
void
#line 1073 "xb.cc"
OpXb_9_cst_z0::getRegsLabel(
#line 97 "xb.isa"
vector<string>
#line 1077 "xb.cc"
#line 97 "xb.isa"
&vect
#line 1080 "xb.cc"
)
{
#line 242 "xb.isa"
	{

		string regLabel(CPU::xb_getAddrRegLabel(rr));
		vect.push_back(regLabel);
	}
#line 1089 "xb.cc"
}

static Operation *DecodeOpXb_9_cst_z0(CodeType const& code, uint16_t addr)
{
	return new OpXb_9_cst_z0(code, addr);
}

#line 86 "xb.isa"
uint8_t
#line 1099 "xb.cc"
OpXb_16bit_cst_z1_s0::getXbMode()
{
#line 250 "xb.isa"
	{ return XbModes::IDX2; }
#line 1104 "xb.cc"
}
#line 95 "xb.isa"
uint16_t
#line 1108 "xb.cc"
OpXb_16bit_cst_z1_s0::getEAddr(
#line 95 "xb.isa"
CPU *
#line 1112 "xb.cc"
#line 95 "xb.isa"
cpu
#line 1115 "xb.cc"
)
{
#line 252 "xb.isa"
	{

		physical_address_t addr = cpu->xb_getAddrRegValue(rr) + (n16 & 0xFFFF);

		// Constant offset 16-bit signed
		return addr;
	}
#line 1126 "xb.cc"
}
#line 91 "xb.isa"
void
#line 1130 "xb.cc"
OpXb_16bit_cst_z1_s0::disasm(
#line 91 "xb.isa"
ostream&
#line 1134 "xb.cc"
#line 91 "xb.isa"
sink
#line 1137 "xb.cc"
)
{
#line 260 "xb.isa"
	{

		// constant offset 16-bit signed
		string regLabel = CPU::xb_getAddrRegLabel(rr);
		sink << std::dec << (n16 & 0xFFFF) << "," << regLabel;
	}
#line 1147 "xb.cc"
}
#line 97 "xb.isa"
void
#line 1151 "xb.cc"
OpXb_16bit_cst_z1_s0::getRegsLabel(
#line 97 "xb.isa"
vector<string>
#line 1155 "xb.cc"
#line 97 "xb.isa"
&vect
#line 1158 "xb.cc"
)
{
#line 267 "xb.isa"
	{

		string regLabel(CPU::xb_getAddrRegLabel(rr));
		vect.push_back(regLabel);
	}
#line 1167 "xb.cc"
}

static Operation *DecodeOpXb_16bit_cst_z1_s0(CodeType const& code, uint16_t addr)
{
	return new OpXb_16bit_cst_z1_s0(code, addr);
}

#line 86 "xb.isa"
uint8_t
#line 1177 "xb.cc"
OpXb_16bit_cst_z1_s1::getXbMode()
{
#line 275 "xb.isa"
	{ return XbModes::IDX2_Ind; }
#line 1182 "xb.cc"
}
#line 95 "xb.isa"
uint16_t
#line 1186 "xb.cc"
OpXb_16bit_cst_z1_s1::getEAddr(
#line 95 "xb.isa"
CPU *
#line 1190 "xb.cc"
#line 95 "xb.isa"
cpu
#line 1193 "xb.cc"
)
{
#line 277 "xb.isa"
	{

		physical_address_t addr = cpu->xb_getAddrRegValue(rr) + n16;

		// 16-bit offset indexed-indirect
		return cpu->memRead16(addr);
	}
#line 1204 "xb.cc"
}
#line 91 "xb.isa"
void
#line 1208 "xb.cc"
OpXb_16bit_cst_z1_s1::disasm(
#line 91 "xb.isa"
ostream&
#line 1212 "xb.cc"
#line 91 "xb.isa"
sink
#line 1215 "xb.cc"
)
{
#line 285 "xb.isa"
	{

		// 16-bit offset index-indirect
		string regLabel = CPU::xb_getAddrRegLabel(rr);
		sink << "[" << std::dec << n16 << "," << regLabel << "]";
	}
#line 1225 "xb.cc"
}
#line 97 "xb.isa"
void
#line 1229 "xb.cc"
OpXb_16bit_cst_z1_s1::getRegsLabel(
#line 97 "xb.isa"
vector<string>
#line 1233 "xb.cc"
#line 97 "xb.isa"
&vect
#line 1236 "xb.cc"
)
{
#line 292 "xb.isa"
	{

		string regLabel(CPU::xb_getAddrRegLabel(rr));
		vect.push_back(regLabel);
	}
#line 1245 "xb.cc"
}

static Operation *DecodeOpXb_16bit_cst_z1_s1(CodeType const& code, uint16_t addr)
{
	return new OpXb_16bit_cst_z1_s1(code, addr);
}

#line 86 "xb.isa"
uint8_t
#line 1255 "xb.cc"
OpXb_acc_offset::getXbMode()
{
#line 361 "xb.isa"
	{ return XbModes::IDX; }
#line 1260 "xb.cc"
}
#line 95 "xb.isa"
uint16_t
#line 1264 "xb.cc"
OpXb_acc_offset::getEAddr(
#line 95 "xb.isa"
CPU *
#line 1268 "xb.cc"
#line 95 "xb.isa"
cpu
#line 1271 "xb.cc"
)
{
#line 363 "xb.isa"
	{
		uint16_t addrRegVal = cpu->xb_getAddrRegValue(rr);
		uint16_t accRegVal = cpu->xb_getAccRegValue(aa);

		return addrRegVal + accRegVal;
	}
#line 1281 "xb.cc"
}
#line 91 "xb.isa"
void
#line 1285 "xb.cc"
OpXb_acc_offset::disasm(
#line 91 "xb.isa"
ostream&
#line 1289 "xb.cc"
#line 91 "xb.isa"
sink
#line 1292 "xb.cc"
)
{
#line 370 "xb.isa"
	{

		string addrRegLabel = CPU::xb_getAddrRegLabel(rr);
		string accRegLabel = CPU::xb_getAccRegLabel(aa);

		sink << accRegLabel << "," << addrRegLabel;
	}
#line 1303 "xb.cc"
}
#line 97 "xb.isa"
void
#line 1307 "xb.cc"
OpXb_acc_offset::getRegsLabel(
#line 97 "xb.isa"
vector<string>
#line 1311 "xb.cc"
#line 97 "xb.isa"
&vect
#line 1314 "xb.cc"
)
{
#line 378 "xb.isa"
	{

		string addrRegLabel(CPU::xb_getAddrRegLabel(rr));
		string accRegLabel(CPU::xb_getAccRegLabel(aa));

		vect.push_back(accRegLabel);
		vect.push_back(addrRegLabel);
	}
#line 1326 "xb.cc"
}

static Operation *DecodeOpXb_acc_offset(CodeType const& code, uint16_t addr)
{
	return new OpXb_acc_offset(code, addr);
}

#line 86 "xb.isa"
uint8_t
#line 1336 "xb.cc"
OpXb_accD_offset_idx_ind::getXbMode()
{
#line 389 "xb.isa"
	{ return XbModes::IDXD_Ind; }
#line 1341 "xb.cc"
}
#line 95 "xb.isa"
uint16_t
#line 1345 "xb.cc"
OpXb_accD_offset_idx_ind::getEAddr(
#line 95 "xb.isa"
CPU *
#line 1349 "xb.cc"
#line 95 "xb.isa"
cpu
#line 1352 "xb.cc"
)
{
#line 391 "xb.isa"
	{

		return cpu->memRead16(cpu->getRegD() + cpu->xb_getAddrRegValue(rr));
	}
#line 1360 "xb.cc"
}
#line 91 "xb.isa"
void
#line 1364 "xb.cc"
OpXb_accD_offset_idx_ind::disasm(
#line 91 "xb.isa"
ostream&
#line 1368 "xb.cc"
#line 91 "xb.isa"
sink
#line 1371 "xb.cc"
)
{
#line 396 "xb.isa"
	{
		string regLabel = CPU::xb_getAddrRegLabel(rr);
		sink << "[D," << regLabel << "]";
	}
#line 1379 "xb.cc"
}
#line 97 "xb.isa"
void
#line 1383 "xb.cc"
OpXb_accD_offset_idx_ind::getRegsLabel(
#line 97 "xb.isa"
vector<string>
#line 1387 "xb.cc"
#line 97 "xb.isa"
&vect
#line 1390 "xb.cc"
)
{
#line 401 "xb.isa"
	{
		string regLabel(CPU::xb_getAddrRegLabel(rr));
		vect.push_back(regLabel);
	}
#line 1398 "xb.cc"
}

static Operation *DecodeOpXb_accD_offset_idx_ind(CodeType const& code, uint16_t addr)
{
	return new OpXb_accD_offset_idx_ind(code, addr);
}

OpXb_auto::OpXb_auto(CodeType const& code, uint16_t addr) : Operation(code, addr, "xb_auto")
{
	this->encoding.size = 8;
	rr = (((uint32_t( code.str[0] ) >> 6) % 4) << 0);
	p = (((uint32_t( code.str[0] ) >> 4) % 2) << 0);
	nnnn = (((uint32_t( code.str[0] ) >> 0) % 16) << 0);
}

OpXb_5bit_cst::OpXb_5bit_cst(CodeType const& code, uint16_t addr) : Operation(code, addr, "xb_5bit_cst")
{
	this->encoding.size = 8;
	rr = (((uint32_t( code.str[0] ) >> 6) % 4) << 0);
	nnnnn = (((uint32_t( code.str[0] ) >> 0) % 32) << 0);
	nnnnn = (nnnnn << 27) >> 27;
}

OpXb_9_cst_z0::OpXb_9_cst_z0(CodeType const& code, uint16_t addr) : Operation(code, addr, "xb_9_cst_z0")
{
	this->encoding.size = 16;
	rr = (((uint32_t( code.str[0] ) >> 3) % 4) << 0);
	s = (((uint32_t( code.str[0] ) >> 0) % 2) << 0);
	n8 = (((uint32_t( code.str[1] ) >> 0) % 256) << 0);
}

OpXb_16bit_cst_z1_s0::OpXb_16bit_cst_z1_s0(CodeType const& code, uint16_t addr) : Operation(code, addr, "xb_16bit_cst_z1_s0")
{
	this->encoding.size = 24;
	rr = (((uint32_t( code.str[0] ) >> 3) % 4) << 0);
	n16 = (((uint32_t( code.str[1] ) >> 0) % 256) << 8) | (((uint32_t( code.str[2] ) >> 0) % 256) << 0);
}

OpXb_16bit_cst_z1_s1::OpXb_16bit_cst_z1_s1(CodeType const& code, uint16_t addr) : Operation(code, addr, "xb_16bit_cst_z1_s1")
{
	this->encoding.size = 24;
	rr = (((uint32_t( code.str[0] ) >> 3) % 4) << 0);
	n16 = (((uint32_t( code.str[1] ) >> 0) % 256) << 8) | (((uint32_t( code.str[2] ) >> 0) % 256) << 0);
	n16 = (n16 << 16) >> 16;
}

OpXb_acc_offset::OpXb_acc_offset(CodeType const& code, uint16_t addr) : Operation(code, addr, "xb_acc_offset")
{
	this->encoding.size = 8;
	rr = (((uint32_t( code.str[0] ) >> 3) % 4) << 0);
	aa = (((uint32_t( code.str[0] ) >> 0) % 4) << 0);
}

OpXb_accD_offset_idx_ind::OpXb_accD_offset_idx_ind(CodeType const& code, uint16_t addr) : Operation(code, addr, "xb_accD_offset_idx_ind")
{
	this->encoding.size = 8;
	rr = (((uint32_t( code.str[0] ) >> 3) % 4) << 0);
}

DecodeTableEntry::DecodeTableEntry(CodeType const& opcode, CodeType const& opcode_mask, Operation *(*decode)(CodeType const&, uint16_t))
{
	this->opcode = opcode;
	this->opcode_mask = opcode_mask;
	this->decode = decode;
}

Decoder::Decoder()
: is_little_endian( false )
{
	decode_table.push_back(DecodeTableEntry(CodeType( (uint8_t*)( "\xe7" ), 8 ), CodeType( (uint8_t*)( "\xe7" ), 8 ), DecodeOpXb_accD_offset_idx_ind));
	decode_table.push_back(DecodeTableEntry(CodeType( (uint8_t*)( "\xe4" ), 6 ), CodeType( (uint8_t*)( "\xe4" ), 6 ), DecodeOpXb_acc_offset));
	decode_table.push_back(DecodeTableEntry(CodeType( (uint8_t*)( "\xe3" ), 8 ), CodeType( (uint8_t*)( "\xe7" ), 8 ), DecodeOpXb_16bit_cst_z1_s1));
	decode_table.push_back(DecodeTableEntry(CodeType( (uint8_t*)( "\xe2" ), 8 ), CodeType( (uint8_t*)( "\xe7" ), 8 ), DecodeOpXb_16bit_cst_z1_s0));
	decode_table.push_back(DecodeTableEntry(CodeType( (uint8_t*)( "\xe0" ), 7 ), CodeType( (uint8_t*)( "\xe6" ), 7 ), DecodeOpXb_9_cst_z0));
	decode_table.push_back(DecodeTableEntry(CodeType( (uint8_t*)( "\x00" ), 3 ), CodeType( (uint8_t*)( "\x20" ), 3 ), DecodeOpXb_5bit_cst));
	decode_table.push_back(DecodeTableEntry(CodeType( (uint8_t*)( "\x20" ), 3 ), CodeType( (uint8_t*)( "\x20" ), 3 ), DecodeOpXb_auto));
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
	is_little_endian = true;
}

void Decoder::SetBigEndian()
{
	is_little_endian = false;
}

} } } } } }
