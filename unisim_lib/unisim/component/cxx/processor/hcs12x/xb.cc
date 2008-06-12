#include "xb.hh"
#include <cassert>
#include <ostream>
#include <cstring>
#line 52 "xb.isa"

using namespace std;
#line 9 "xb.cc"
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

#line 60 "xb.isa"
uint16_t
#line 35 "xb.cc"
Operation::getEAddr(
#line 60 "xb.isa"
CPU *
#line 39 "xb.cc"
#line 60 "xb.isa"
cpu
#line 42 "xb.cc"
)
{
#line 60 "xb.isa"
	{ assert( false ); return 0; }
#line 47 "xb.cc"
}
#line 56 "xb.isa"
void
#line 51 "xb.cc"
Operation::disasm(
#line 56 "xb.isa"
ostream&
#line 55 "xb.cc"
#line 56 "xb.isa"
sink
#line 58 "xb.cc"
)
{
#line 56 "xb.isa"
	{
		sink << "?";
	}
#line 65 "xb.cc"
}
class OpXb_auto : public Operation
{
public:
	OpXb_auto(CodeType const& code, uint16_t addr);
	uint32_t rr;
	uint32_t p;
	uint32_t nnnn;
	virtual
#line 60 "xb.isa"
	uint16_t
#line 77 "xb.cc"
	getEAddr(
#line 60 "xb.isa"
	CPU *
#line 81 "xb.cc"
#line 60 "xb.isa"
	cpu
#line 84 "xb.cc"
	);
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
#line 56 "xb.isa"
	void
#line 122 "xb.cc"
	disasm(
#line 56 "xb.isa"
	ostream&
#line 126 "xb.cc"
#line 56 "xb.isa"
	sink
#line 129 "xb.cc"
	);
private:
};

class OpXb_5bit_cst : public Operation
{
public:
	OpXb_5bit_cst(CodeType const& code, uint16_t addr);
	uint32_t rr;
	int32_t nnnnn;
	virtual
#line 60 "xb.isa"
	uint16_t
#line 143 "xb.cc"
	getEAddr(
#line 60 "xb.isa"
	CPU *
#line 147 "xb.cc"
#line 60 "xb.isa"
	cpu
#line 150 "xb.cc"
	);
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
#line 56 "xb.isa"
	void
#line 188 "xb.cc"
	disasm(
#line 56 "xb.isa"
	ostream&
#line 192 "xb.cc"
#line 56 "xb.isa"
	sink
#line 195 "xb.cc"
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
	virtual
#line 60 "xb.isa"
	uint16_t
#line 210 "xb.cc"
	getEAddr(
#line 60 "xb.isa"
	CPU *
#line 214 "xb.cc"
#line 60 "xb.isa"
	cpu
#line 217 "xb.cc"
	);
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
#line 56 "xb.isa"
	void
#line 255 "xb.cc"
	disasm(
#line 56 "xb.isa"
	ostream&
#line 259 "xb.cc"
#line 56 "xb.isa"
	sink
#line 262 "xb.cc"
	);
private:
};

class OpXb_16bit_cst_z1 : public Operation
{
public:
	OpXb_16bit_cst_z1(CodeType const& code, uint16_t addr);
	uint32_t rr;
	uint32_t s;
	int32_t n16;
	virtual
#line 60 "xb.isa"
	uint16_t
#line 277 "xb.cc"
	getEAddr(
#line 60 "xb.isa"
	CPU *
#line 281 "xb.cc"
#line 60 "xb.isa"
	cpu
#line 284 "xb.cc"
	);
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
#line 56 "xb.isa"
	void
#line 322 "xb.cc"
	disasm(
#line 56 "xb.isa"
	ostream&
#line 326 "xb.cc"
#line 56 "xb.isa"
	sink
#line 329 "xb.cc"
	);
private:
};

class OpXb_acc_offset : public Operation
{
public:
	OpXb_acc_offset(CodeType const& code, uint16_t addr);
	uint32_t rr;
	uint32_t aa;
	virtual
#line 60 "xb.isa"
	uint16_t
#line 343 "xb.cc"
	getEAddr(
#line 60 "xb.isa"
	CPU *
#line 347 "xb.cc"
#line 60 "xb.isa"
	cpu
#line 350 "xb.cc"
	);
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
#line 56 "xb.isa"
	void
#line 388 "xb.cc"
	disasm(
#line 56 "xb.isa"
	ostream&
#line 392 "xb.cc"
#line 56 "xb.isa"
	sink
#line 395 "xb.cc"
	);
private:
};

class OpXb_accD_offset_idx_ind : public Operation
{
public:
	OpXb_accD_offset_idx_ind(CodeType const& code, uint16_t addr);
	uint32_t rr;
	virtual
#line 60 "xb.isa"
	uint16_t
#line 408 "xb.cc"
	getEAddr(
#line 60 "xb.isa"
	CPU *
#line 412 "xb.cc"
#line 60 "xb.isa"
	cpu
#line 415 "xb.cc"
	);
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
#line 56 "xb.isa"
	void
#line 453 "xb.cc"
	disasm(
#line 56 "xb.isa"
	ostream&
#line 457 "xb.cc"
#line 56 "xb.isa"
	sink
#line 460 "xb.cc"
	);
private:
};

#line 60 "xb.isa"
uint16_t
#line 467 "xb.cc"
OpXb_auto::getEAddr(
#line 60 "xb.isa"
CPU *
#line 471 "xb.cc"
#line 60 "xb.isa"
cpu
#line 474 "xb.cc"
)
{
#line 123 "xb.isa"
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
#line 501 "xb.cc"
}
#line 56 "xb.isa"
void
#line 505 "xb.cc"
OpXb_auto::disasm(
#line 56 "xb.isa"
ostream&
#line 509 "xb.cc"
#line 56 "xb.isa"
sink
#line 512 "xb.cc"
)
{
#line 147 "xb.isa"
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
#line 540 "xb.cc"
}

static Operation *DecodeOpXb_auto(CodeType const& code, uint16_t addr)
{
	return new OpXb_auto(code, addr);
}

#line 60 "xb.isa"
uint16_t
#line 550 "xb.cc"
OpXb_5bit_cst::getEAddr(
#line 60 "xb.isa"
CPU *
#line 554 "xb.cc"
#line 60 "xb.isa"
cpu
#line 557 "xb.cc"
)
{
#line 64 "xb.isa"
	{

		address_t addr = (int16_t) cpu->xb_getAddrRegValue(rr) + nnnnn;

		return addr;

		//    return cpu->xb_getAddrRegValue(rr) + nnnnn;
	}
#line 569 "xb.cc"
}
#line 56 "xb.isa"
void
#line 573 "xb.cc"
OpXb_5bit_cst::disasm(
#line 56 "xb.isa"
ostream&
#line 577 "xb.cc"
#line 56 "xb.isa"
sink
#line 580 "xb.cc"
)
{
#line 73 "xb.isa"
	{
		sink << std::dec << nnnnn << "," << CPU::xb_getAddrRegLabel(rr);
	}
#line 587 "xb.cc"
}

static Operation *DecodeOpXb_5bit_cst(CodeType const& code, uint16_t addr)
{
	return new OpXb_5bit_cst(code, addr);
}

// TODO: 20080417 - a revoir
#line 60 "xb.isa"
uint16_t
#line 598 "xb.cc"
OpXb_9_cst_z0::getEAddr(
#line 60 "xb.isa"
CPU *
#line 602 "xb.cc"
#line 60 "xb.isa"
cpu
#line 605 "xb.cc"
)
{
#line 80 "xb.isa"
	{

		address_t addr = (int16_t) cpu->xb_getAddrRegValue(rr) + n8;

		return addr;

		//	return cpu->xb_getAddrRegValue(rr) + n8;
	}
#line 617 "xb.cc"
}
#line 56 "xb.isa"
void
#line 621 "xb.cc"
OpXb_9_cst_z0::disasm(
#line 56 "xb.isa"
ostream&
#line 625 "xb.cc"
#line 56 "xb.isa"
sink
#line 628 "xb.cc"
)
{
#line 89 "xb.isa"
	{

		sink << std::dec << n8 << "," << CPU::xb_getAddrRegLabel(rr);
	}
#line 636 "xb.cc"
}

static Operation *DecodeOpXb_9_cst_z0(CodeType const& code, uint16_t addr)
{
	return new OpXb_9_cst_z0(code, addr);
}

#line 60 "xb.isa"
uint16_t
#line 646 "xb.cc"
OpXb_16bit_cst_z1::getEAddr(
#line 60 "xb.isa"
CPU *
#line 650 "xb.cc"
#line 60 "xb.isa"
cpu
#line 653 "xb.cc"
)
{
#line 96 "xb.isa"
	{

		physical_address_t addr = cpu->mmc->getPhysicalAddress(cpu->xb_getAddrRegValue(rr) + n16, MEMORY::EXTENDED, WO_GLOBAL_ADDRESS);

		if (s==0) {
			// Constant offset 16-bit signed
			return addr;
			} else {
			// 16-bit offset indexed-indirect

			return cpu->memRead16(addr);
		}
	}
#line 670 "xb.cc"
}
#line 56 "xb.isa"
void
#line 674 "xb.cc"
OpXb_16bit_cst_z1::disasm(
#line 56 "xb.isa"
ostream&
#line 678 "xb.cc"
#line 56 "xb.isa"
sink
#line 681 "xb.cc"
)
{
#line 110 "xb.isa"
	{
		if (s==0) {
			// constant offset 16-bit signed
			sink << std::dec << n16 << "," << CPU::xb_getAddrRegLabel(rr);
			} else {
			// 16-bit offset index-indirect
			sink << "[" << std::dec << n16 << "," << CPU::xb_getAddrRegLabel(rr) << "]";
		}
	}
#line 694 "xb.cc"
}

static Operation *DecodeOpXb_16bit_cst_z1(CodeType const& code, uint16_t addr)
{
	return new OpXb_16bit_cst_z1(code, addr);
}

#line 60 "xb.isa"
uint16_t
#line 704 "xb.cc"
OpXb_acc_offset::getEAddr(
#line 60 "xb.isa"
CPU *
#line 708 "xb.cc"
#line 60 "xb.isa"
cpu
#line 711 "xb.cc"
)
{
#line 175 "xb.isa"
	{
		uint16_t addrRegVal = cpu->xb_getAddrRegValue(rr);
		uint16_t accRegVal = cpu->xb_getAccRegValue(aa);

		return addrRegVal + accRegVal;
	}
#line 721 "xb.cc"
}
#line 56 "xb.isa"
void
#line 725 "xb.cc"
OpXb_acc_offset::disasm(
#line 56 "xb.isa"
ostream&
#line 729 "xb.cc"
#line 56 "xb.isa"
sink
#line 732 "xb.cc"
)
{
#line 182 "xb.isa"
	{
		sink << CPU::xb_getAccRegLabel(aa) << "," << CPU::xb_getAddrRegLabel(rr);
	}
#line 739 "xb.cc"
}

static Operation *DecodeOpXb_acc_offset(CodeType const& code, uint16_t addr)
{
	return new OpXb_acc_offset(code, addr);
}

#line 60 "xb.isa"
uint16_t
#line 749 "xb.cc"
OpXb_accD_offset_idx_ind::getEAddr(
#line 60 "xb.isa"
CPU *
#line 753 "xb.cc"
#line 60 "xb.isa"
cpu
#line 756 "xb.cc"
)
{
#line 189 "xb.isa"
	{

		return cpu->memRead16(cpu->mmc->getPhysicalAddress(cpu->getRegD() + cpu->xb_getAddrRegValue(rr), MEMORY::EXTENDED, WO_GLOBAL_ADDRESS));
	}
#line 764 "xb.cc"
}
#line 56 "xb.isa"
void
#line 768 "xb.cc"
OpXb_accD_offset_idx_ind::disasm(
#line 56 "xb.isa"
ostream&
#line 772 "xb.cc"
#line 56 "xb.isa"
sink
#line 775 "xb.cc"
)
{
#line 194 "xb.isa"
	{
		sink << "[D," << CPU::xb_getAddrRegLabel(rr) << "]";
	}
#line 782 "xb.cc"
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

OpXb_16bit_cst_z1::OpXb_16bit_cst_z1(CodeType const& code, uint16_t addr) : Operation(code, addr, "xb_16bit_cst_z1")
{
	CodeType _code_( code );
	{
		uint8_t _subword_ = (uint8_t( _code_.str[0] ) << 0);
		rr = ((_subword_ >> 3) & 0x3ULL);
		s = ((_subword_ >> 0) & 0x1ULL);
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
	decode_table.push_back(DecodeTableEntry(CodeType( (uint8_t*)( "\xe2" ), 1 ), CodeType( (uint8_t*)( "\xe6" ), 1 ), DecodeOpXb_16bit_cst_z1));
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
