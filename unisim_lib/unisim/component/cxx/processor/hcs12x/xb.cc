#include "xb.hh"
#include <cassert>
#include <ostream>
#include <cstring>
#line 81 "xb.isa"

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

#line 136 "xb.isa"
void
#line 36 "xb.cc"
Operation::post_execute(
#line 136 "xb.isa"
ostream&
#line 40 "xb.cc"
#line 136 "xb.isa"
sink
#line 43 "xb.cc"
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
#line 81 "xb.cc"
}
#line 100 "xb.isa"
void
#line 85 "xb.cc"
Operation::pre_execute(
#line 100 "xb.isa"
ostream&
#line 89 "xb.cc"
#line 100 "xb.isa"
sink
#line 92 "xb.cc"
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
#line 131 "xb.cc"
}
#line 97 "xb.isa"
void
#line 135 "xb.cc"
Operation::getRegsLabel(
#line 97 "xb.isa"
vector<string>
#line 139 "xb.cc"
#line 97 "xb.isa"
&vect
#line 142 "xb.cc"
)
{
#line 97 "xb.isa"
	{ return; }
#line 147 "xb.cc"
}
#line 95 "xb.isa"
uint16_t
#line 151 "xb.cc"
Operation::getEAddr(
#line 95 "xb.isa"
CPU *
#line 155 "xb.cc"
#line 95 "xb.isa"
cpu
#line 158 "xb.cc"
)
{
#line 95 "xb.isa"
	{ assert( false ); return 0; }
#line 163 "xb.cc"
}
#line 91 "xb.isa"
void
#line 167 "xb.cc"
Operation::disasm(
#line 91 "xb.isa"
ostream&
#line 171 "xb.cc"
#line 91 "xb.isa"
sink
#line 174 "xb.cc"
)
{
#line 91 "xb.isa"
	{
		sink << "?";
	}
#line 181 "xb.cc"
}
#line 86 "xb.isa"
uint8_t
#line 185 "xb.cc"
Operation::getXbMode()
{
#line 87 "xb.isa"
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
	// decoder( cisc, sub )
	// address {uint16_t}
	// big_endian
	virtual
#line 86 "xb.isa"
	uint8_t
#line 240 "xb.cc"
	getXbMode();
	virtual
#line 95 "xb.isa"
	uint16_t
#line 245 "xb.cc"
	getEAddr(
#line 95 "xb.isa"
	CPU *
#line 249 "xb.cc"
#line 95 "xb.isa"
	cpu
#line 252 "xb.cc"
	);
	virtual
#line 91 "xb.isa"
	void
#line 257 "xb.cc"
	disasm(
#line 91 "xb.isa"
	ostream&
#line 261 "xb.cc"
#line 91 "xb.isa"
	sink
#line 264 "xb.cc"
	);
	virtual
#line 97 "xb.isa"
	void
#line 269 "xb.cc"
	getRegsLabel(
#line 97 "xb.isa"
	vector<string>
#line 273 "xb.cc"
#line 97 "xb.isa"
	&vect
#line 276 "xb.cc"
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
#line 326 "xb.cc"
	getXbMode();
	virtual
#line 95 "xb.isa"
	uint16_t
#line 331 "xb.cc"
	getEAddr(
#line 95 "xb.isa"
	CPU *
#line 335 "xb.cc"
#line 95 "xb.isa"
	cpu
#line 338 "xb.cc"
	);
	virtual
#line 91 "xb.isa"
	void
#line 343 "xb.cc"
	disasm(
#line 91 "xb.isa"
	ostream&
#line 347 "xb.cc"
#line 91 "xb.isa"
	sink
#line 350 "xb.cc"
	);
	virtual
#line 97 "xb.isa"
	void
#line 355 "xb.cc"
	getRegsLabel(
#line 97 "xb.isa"
	vector<string>
#line 359 "xb.cc"
#line 97 "xb.isa"
	&vect
#line 362 "xb.cc"
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
#line 413 "xb.cc"
	getXbMode();
	virtual
#line 95 "xb.isa"
	uint16_t
#line 418 "xb.cc"
	getEAddr(
#line 95 "xb.isa"
	CPU *
#line 422 "xb.cc"
#line 95 "xb.isa"
	cpu
#line 425 "xb.cc"
	);
	virtual
#line 91 "xb.isa"
	void
#line 430 "xb.cc"
	disasm(
#line 91 "xb.isa"
	ostream&
#line 434 "xb.cc"
#line 91 "xb.isa"
	sink
#line 437 "xb.cc"
	);
	virtual
#line 97 "xb.isa"
	void
#line 442 "xb.cc"
	getRegsLabel(
#line 97 "xb.isa"
	vector<string>
#line 446 "xb.cc"
#line 97 "xb.isa"
	&vect
#line 449 "xb.cc"
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
#line 499 "xb.cc"
	getXbMode();
	virtual
#line 95 "xb.isa"
	uint16_t
#line 504 "xb.cc"
	getEAddr(
#line 95 "xb.isa"
	CPU *
#line 508 "xb.cc"
#line 95 "xb.isa"
	cpu
#line 511 "xb.cc"
	);
	virtual
#line 91 "xb.isa"
	void
#line 516 "xb.cc"
	disasm(
#line 91 "xb.isa"
	ostream&
#line 520 "xb.cc"
#line 91 "xb.isa"
	sink
#line 523 "xb.cc"
	);
	virtual
#line 97 "xb.isa"
	void
#line 528 "xb.cc"
	getRegsLabel(
#line 97 "xb.isa"
	vector<string>
#line 532 "xb.cc"
#line 97 "xb.isa"
	&vect
#line 535 "xb.cc"
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
#line 585 "xb.cc"
	getXbMode();
	virtual
#line 95 "xb.isa"
	uint16_t
#line 590 "xb.cc"
	getEAddr(
#line 95 "xb.isa"
	CPU *
#line 594 "xb.cc"
#line 95 "xb.isa"
	cpu
#line 597 "xb.cc"
	);
	virtual
#line 91 "xb.isa"
	void
#line 602 "xb.cc"
	disasm(
#line 91 "xb.isa"
	ostream&
#line 606 "xb.cc"
#line 91 "xb.isa"
	sink
#line 609 "xb.cc"
	);
	virtual
#line 97 "xb.isa"
	void
#line 614 "xb.cc"
	getRegsLabel(
#line 97 "xb.isa"
	vector<string>
#line 618 "xb.cc"
#line 97 "xb.isa"
	&vect
#line 621 "xb.cc"
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
#line 671 "xb.cc"
	getXbMode();
	virtual
#line 95 "xb.isa"
	uint16_t
#line 676 "xb.cc"
	getEAddr(
#line 95 "xb.isa"
	CPU *
#line 680 "xb.cc"
#line 95 "xb.isa"
	cpu
#line 683 "xb.cc"
	);
	virtual
#line 91 "xb.isa"
	void
#line 688 "xb.cc"
	disasm(
#line 91 "xb.isa"
	ostream&
#line 692 "xb.cc"
#line 91 "xb.isa"
	sink
#line 695 "xb.cc"
	);
	virtual
#line 97 "xb.isa"
	void
#line 700 "xb.cc"
	getRegsLabel(
#line 97 "xb.isa"
	vector<string>
#line 704 "xb.cc"
#line 97 "xb.isa"
	&vect
#line 707 "xb.cc"
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
#line 756 "xb.cc"
	getXbMode();
	virtual
#line 95 "xb.isa"
	uint16_t
#line 761 "xb.cc"
	getEAddr(
#line 95 "xb.isa"
	CPU *
#line 765 "xb.cc"
#line 95 "xb.isa"
	cpu
#line 768 "xb.cc"
	);
	virtual
#line 91 "xb.isa"
	void
#line 773 "xb.cc"
	disasm(
#line 91 "xb.isa"
	ostream&
#line 777 "xb.cc"
#line 91 "xb.isa"
	sink
#line 780 "xb.cc"
	);
	virtual
#line 97 "xb.isa"
	void
#line 785 "xb.cc"
	getRegsLabel(
#line 97 "xb.isa"
	vector<string>
#line 789 "xb.cc"
#line 97 "xb.isa"
	&vect
#line 792 "xb.cc"
	);
private:
};

#line 86 "xb.isa"
uint8_t
#line 799 "xb.cc"
OpXb_auto::getXbMode()
{
#line 284 "xb.isa"
	{ return XbModes::IDX; }
#line 804 "xb.cc"
}
#line 95 "xb.isa"
uint16_t
#line 808 "xb.cc"
OpXb_auto::getEAddr(
#line 95 "xb.isa"
CPU *
#line 812 "xb.cc"
#line 95 "xb.isa"
cpu
#line 815 "xb.cc"
)
{
#line 286 "xb.isa"
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
#line 844 "xb.cc"
}
#line 91 "xb.isa"
void
#line 848 "xb.cc"
OpXb_auto::disasm(
#line 91 "xb.isa"
ostream&
#line 852 "xb.cc"
#line 91 "xb.isa"
sink
#line 855 "xb.cc"
)
{
#line 312 "xb.isa"
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
#line 883 "xb.cc"
}
#line 97 "xb.isa"
void
#line 887 "xb.cc"
OpXb_auto::getRegsLabel(
#line 97 "xb.isa"
vector<string>
#line 891 "xb.cc"
#line 97 "xb.isa"
&vect
#line 894 "xb.cc"
)
{
#line 337 "xb.isa"
	{

		string	regLabel(CPU::xb_getAddrRegLabel(rr));
		vect.push_back(regLabel);
	}
#line 903 "xb.cc"
}

static Operation *DecodeOpXb_auto(CodeType const& code, uint16_t addr)
{
	return new OpXb_auto(code, addr);
}

#line 86 "xb.isa"
uint8_t
#line 913 "xb.cc"
OpXb_5bit_cst::getXbMode()
{
#line 174 "xb.isa"
	{ return XbModes::IDX; }
#line 918 "xb.cc"
}
#line 95 "xb.isa"
uint16_t
#line 922 "xb.cc"
OpXb_5bit_cst::getEAddr(
#line 95 "xb.isa"
CPU *
#line 926 "xb.cc"
#line 95 "xb.isa"
cpu
#line 929 "xb.cc"
)
{
#line 176 "xb.isa"
	{

		address_t addr = (int16_t) cpu->xb_getAddrRegValue(rr) + nnnnn;

		return addr;

	}
#line 940 "xb.cc"
}
#line 91 "xb.isa"
void
#line 944 "xb.cc"
OpXb_5bit_cst::disasm(
#line 91 "xb.isa"
ostream&
#line 948 "xb.cc"
#line 91 "xb.isa"
sink
#line 951 "xb.cc"
)
{
#line 184 "xb.isa"
	{

		string regLabel = CPU::xb_getAddrRegLabel(rr);

		sink << std::dec << nnnnn << "," << regLabel;

	}
#line 962 "xb.cc"
}
#line 97 "xb.isa"
void
#line 966 "xb.cc"
OpXb_5bit_cst::getRegsLabel(
#line 97 "xb.isa"
vector<string>
#line 970 "xb.cc"
#line 97 "xb.isa"
&vect
#line 973 "xb.cc"
)
{
#line 192 "xb.isa"
	{

		string regLabel(CPU::xb_getAddrRegLabel(rr));
		vect.push_back(regLabel);
	}
#line 982 "xb.cc"
}

static Operation *DecodeOpXb_5bit_cst(CodeType const& code, uint16_t addr)
{
	return new OpXb_5bit_cst(code, addr);
}

#line 86 "xb.isa"
uint8_t
#line 992 "xb.cc"
OpXb_9_cst_z0::getXbMode()
{
#line 200 "xb.isa"
	{ return XbModes::IDX1; }
#line 997 "xb.cc"
}
#line 95 "xb.isa"
uint16_t
#line 1001 "xb.cc"
OpXb_9_cst_z0::getEAddr(
#line 95 "xb.isa"
CPU *
#line 1005 "xb.cc"
#line 95 "xb.isa"
cpu
#line 1008 "xb.cc"
)
{
#line 202 "xb.isa"
	{

		address_t addr;
		if (s==0) {
			addr = (int16_t) cpu->xb_getAddrRegValue(rr) + n8;
			} else {
			addr = (int16_t) cpu->xb_getAddrRegValue(rr) + (n8 - 256);
		}

		return addr;

	}
#line 1024 "xb.cc"
}
#line 91 "xb.isa"
void
#line 1028 "xb.cc"
OpXb_9_cst_z0::disasm(
#line 91 "xb.isa"
ostream&
#line 1032 "xb.cc"
#line 91 "xb.isa"
sink
#line 1035 "xb.cc"
)
{
#line 215 "xb.isa"
	{

		string regLabel = CPU::xb_getAddrRegLabel(rr);
		if (s==0) {
			sink << std::dec << n8 << "," << regLabel;
			} else {
			sink << std::dec << (n8 - 256) << "," << regLabel;
		}

	}
#line 1049 "xb.cc"
}
#line 97 "xb.isa"
void
#line 1053 "xb.cc"
OpXb_9_cst_z0::getRegsLabel(
#line 97 "xb.isa"
vector<string>
#line 1057 "xb.cc"
#line 97 "xb.isa"
&vect
#line 1060 "xb.cc"
)
{
#line 226 "xb.isa"
	{

		string regLabel(CPU::xb_getAddrRegLabel(rr));
		vect.push_back(regLabel);
	}
#line 1069 "xb.cc"
}

static Operation *DecodeOpXb_9_cst_z0(CodeType const& code, uint16_t addr)
{
	return new OpXb_9_cst_z0(code, addr);
}

#line 86 "xb.isa"
uint8_t
#line 1079 "xb.cc"
OpXb_16bit_cst_z1_s0::getXbMode()
{
#line 234 "xb.isa"
	{ return XbModes::IDX2; }
#line 1084 "xb.cc"
}
#line 95 "xb.isa"
uint16_t
#line 1088 "xb.cc"
OpXb_16bit_cst_z1_s0::getEAddr(
#line 95 "xb.isa"
CPU *
#line 1092 "xb.cc"
#line 95 "xb.isa"
cpu
#line 1095 "xb.cc"
)
{
#line 236 "xb.isa"
	{

		physical_address_t addr = cpu->xb_getAddrRegValue(rr) + ((uint16_t) n16);

		// Constant offset 16-bit signed
		return addr;
	}
#line 1106 "xb.cc"
}
#line 91 "xb.isa"
void
#line 1110 "xb.cc"
OpXb_16bit_cst_z1_s0::disasm(
#line 91 "xb.isa"
ostream&
#line 1114 "xb.cc"
#line 91 "xb.isa"
sink
#line 1117 "xb.cc"
)
{
#line 244 "xb.isa"
	{

		// constant offset 16-bit signed
		string regLabel = CPU::xb_getAddrRegLabel(rr);
		sink << std::dec << ((uint16_t) n16) << "," << regLabel;
	}
#line 1127 "xb.cc"
}
#line 97 "xb.isa"
void
#line 1131 "xb.cc"
OpXb_16bit_cst_z1_s0::getRegsLabel(
#line 97 "xb.isa"
vector<string>
#line 1135 "xb.cc"
#line 97 "xb.isa"
&vect
#line 1138 "xb.cc"
)
{
#line 251 "xb.isa"
	{

		string regLabel(CPU::xb_getAddrRegLabel(rr));
		vect.push_back(regLabel);
	}
#line 1147 "xb.cc"
}

static Operation *DecodeOpXb_16bit_cst_z1_s0(CodeType const& code, uint16_t addr)
{
	return new OpXb_16bit_cst_z1_s0(code, addr);
}

#line 86 "xb.isa"
uint8_t
#line 1157 "xb.cc"
OpXb_16bit_cst_z1_s1::getXbMode()
{
#line 259 "xb.isa"
	{ return XbModes::IDX2_Ind; }
#line 1162 "xb.cc"
}
#line 95 "xb.isa"
uint16_t
#line 1166 "xb.cc"
OpXb_16bit_cst_z1_s1::getEAddr(
#line 95 "xb.isa"
CPU *
#line 1170 "xb.cc"
#line 95 "xb.isa"
cpu
#line 1173 "xb.cc"
)
{
#line 261 "xb.isa"
	{

		physical_address_t addr = cpu->xb_getAddrRegValue(rr) + n16;

		// 16-bit offset indexed-indirect
		return cpu->memRead16(addr);
	}
#line 1184 "xb.cc"
}
#line 91 "xb.isa"
void
#line 1188 "xb.cc"
OpXb_16bit_cst_z1_s1::disasm(
#line 91 "xb.isa"
ostream&
#line 1192 "xb.cc"
#line 91 "xb.isa"
sink
#line 1195 "xb.cc"
)
{
#line 269 "xb.isa"
	{

		// 16-bit offset index-indirect
		string regLabel = CPU::xb_getAddrRegLabel(rr);
		sink << "[" << std::dec << n16 << "," << regLabel << "]";
	}
#line 1205 "xb.cc"
}
#line 97 "xb.isa"
void
#line 1209 "xb.cc"
OpXb_16bit_cst_z1_s1::getRegsLabel(
#line 97 "xb.isa"
vector<string>
#line 1213 "xb.cc"
#line 97 "xb.isa"
&vect
#line 1216 "xb.cc"
)
{
#line 276 "xb.isa"
	{

		string regLabel(CPU::xb_getAddrRegLabel(rr));
		vect.push_back(regLabel);
	}
#line 1225 "xb.cc"
}

static Operation *DecodeOpXb_16bit_cst_z1_s1(CodeType const& code, uint16_t addr)
{
	return new OpXb_16bit_cst_z1_s1(code, addr);
}

#line 86 "xb.isa"
uint8_t
#line 1235 "xb.cc"
OpXb_acc_offset::getXbMode()
{
#line 345 "xb.isa"
	{ return XbModes::IDX; }
#line 1240 "xb.cc"
}
#line 95 "xb.isa"
uint16_t
#line 1244 "xb.cc"
OpXb_acc_offset::getEAddr(
#line 95 "xb.isa"
CPU *
#line 1248 "xb.cc"
#line 95 "xb.isa"
cpu
#line 1251 "xb.cc"
)
{
#line 347 "xb.isa"
	{
		uint16_t addrRegVal = cpu->xb_getAddrRegValue(rr);
		uint16_t accRegVal = cpu->xb_getAccRegValue(aa);

		return addrRegVal + accRegVal;
	}
#line 1261 "xb.cc"
}
#line 91 "xb.isa"
void
#line 1265 "xb.cc"
OpXb_acc_offset::disasm(
#line 91 "xb.isa"
ostream&
#line 1269 "xb.cc"
#line 91 "xb.isa"
sink
#line 1272 "xb.cc"
)
{
#line 354 "xb.isa"
	{

		string addrRegLabel = CPU::xb_getAddrRegLabel(rr);
		string accRegLabel = CPU::xb_getAccRegLabel(aa);

		sink << accRegLabel << "," << addrRegLabel;
	}
#line 1283 "xb.cc"
}
#line 97 "xb.isa"
void
#line 1287 "xb.cc"
OpXb_acc_offset::getRegsLabel(
#line 97 "xb.isa"
vector<string>
#line 1291 "xb.cc"
#line 97 "xb.isa"
&vect
#line 1294 "xb.cc"
)
{
#line 362 "xb.isa"
	{

		string addrRegLabel(CPU::xb_getAddrRegLabel(rr));
		string accRegLabel(CPU::xb_getAccRegLabel(aa));

		vect.push_back(accRegLabel);
		vect.push_back(addrRegLabel);
	}
#line 1306 "xb.cc"
}

static Operation *DecodeOpXb_acc_offset(CodeType const& code, uint16_t addr)
{
	return new OpXb_acc_offset(code, addr);
}

#line 86 "xb.isa"
uint8_t
#line 1316 "xb.cc"
OpXb_accD_offset_idx_ind::getXbMode()
{
#line 373 "xb.isa"
	{ return XbModes::IDXD_Ind; }
#line 1321 "xb.cc"
}
#line 95 "xb.isa"
uint16_t
#line 1325 "xb.cc"
OpXb_accD_offset_idx_ind::getEAddr(
#line 95 "xb.isa"
CPU *
#line 1329 "xb.cc"
#line 95 "xb.isa"
cpu
#line 1332 "xb.cc"
)
{
#line 375 "xb.isa"
	{

		return cpu->memRead16(cpu->getRegD() + cpu->xb_getAddrRegValue(rr));
	}
#line 1340 "xb.cc"
}
#line 91 "xb.isa"
void
#line 1344 "xb.cc"
OpXb_accD_offset_idx_ind::disasm(
#line 91 "xb.isa"
ostream&
#line 1348 "xb.cc"
#line 91 "xb.isa"
sink
#line 1351 "xb.cc"
)
{
#line 380 "xb.isa"
	{
		string regLabel = CPU::xb_getAddrRegLabel(rr);
		sink << "[D," << regLabel << "]";
	}
#line 1359 "xb.cc"
}
#line 97 "xb.isa"
void
#line 1363 "xb.cc"
OpXb_accD_offset_idx_ind::getRegsLabel(
#line 97 "xb.isa"
vector<string>
#line 1367 "xb.cc"
#line 97 "xb.isa"
&vect
#line 1370 "xb.cc"
)
{
#line 385 "xb.isa"
	{
		string regLabel(CPU::xb_getAddrRegLabel(rr));
		vect.push_back(regLabel);
	}
#line 1378 "xb.cc"
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
		n8 = ((_subword_ >> 0) & 0xffULL);
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
		n16 = ((_subword_ >> 0) & 0xffffULL);
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
: is_little_endian( false )
{
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
	is_little_endian = true;
}

void Decoder::SetBigEndian()
{
	is_little_endian = false;
}

} } } } } }
