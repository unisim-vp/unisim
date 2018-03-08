/*
 *  Copyright (c) 2016,
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
 * Authors: Yves Lhuillier (yves.lhuillier@cea.fr)
 */

#ifndef __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_ISA_BOOK_VLE_VLE_HH__
#define __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_ISA_BOOK_VLE_VLE_HH__

/***************************************/
/*** Convenience disassembly methods ***/
/***************************************/

#include <iostream>

namespace unisim {
namespace component {
namespace cxx {
namespace processor {
namespace powerpc {

struct DASMPrint
{
	virtual void Print( std::ostream& sink ) const = 0;
	friend std::ostream& operator << (std::ostream& sink, DASMPrint const& dap)
	{
		dap.Print(sink);
		return sink;
	}
};

struct GPRPrint : public DASMPrint
{
	GPRPrint(unsigned _reg) : reg(_reg) {} unsigned reg;
	void Print( std::ostream& sink ) const { sink << "r" << std::dec << reg; }
};

struct FPRPrint : public DASMPrint
{
	FPRPrint(unsigned _reg) : reg(_reg) {} unsigned reg;
	void Print( std::ostream& sink ) const { sink << "r" << std::dec << reg; }
};

struct HexPrint : public DASMPrint
{
	HexPrint(uint32_t _num) : num(_num) {} unsigned num;
	void Print( std::ostream& sink ) const { sink << "0x" << std::hex << num; }
};

struct CRPrint : public DASMPrint
{
	CRPrint(unsigned _reg) : reg(_reg) {} unsigned reg;
	void Print( std::ostream& sink ) const { sink << "cr" << std::dec << reg; }
};

struct CondPrint : public DASMPrint
{
	CondPrint(unsigned _crb, bool _expect = true) : crb(_crb), expect(_expect) {} unsigned crb; bool expect;
	void Print( std::ostream& sink ) const
	{
		char const* condnames[] = {"ge","le","ne","ns","lt","gt","eq","so"};
		char const* condname = condnames[(crb&3)|(expect?4:0)];
		if (crb >= 4)
		sink << "4*" << CRPrint(crb>>2) << '+';
		sink << condname;
	}
};

struct EAPrint : public DASMPrint
{
	EAPrint(int32_t _idx, unsigned _reg) : idx(_idx), reg(_reg) {} int32_t idx; unsigned reg;
	void Print( std::ostream& sink ) const
	{
		sink << std::dec << idx << '(';
		if (reg)
		sink << GPRPrint(reg);
		else
		sink << '0';
		sink << ')';
	}
};

inline uint32_t scaled_immediate( uint32_t s11 )
{
	int scale = (s11 >> 5) & 24;
	uint32_t msk = int32_t(s11 << 21) >> 31;
	return (((s11 ^ msk) & 255) << scale) ^ msk;
}
  
} /* end of namespace powerpc */
} /* end of namespace processor */
} /* end of namespace cxx */
} /* end of namespace component */
} /* end of namespace unisim */

#endif // __UNISIM_COMPONENT_CXX_PROCESSOR_POWERPC_ISA_BOOK_VLE_VLE_HH__
