#include "isa_thumb.hh"
#include <cassert>
#line 78 "isa/thumb/thumb.isa"

#include "unisim/util/arithmetic/arithmetic.hh"
#include "unisim/component/cxx/processor/arm/exception.hh"
#include "unisim/kernel/logger/logger.hh"
#include <stdint.h>

using unisim::component::cxx::processor::arm::UndefinedInstructionException;
using unisim::util::arithmetic::Add32;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;
#line 19 "isa_thumb.tcc"
namespace unisim { namespace component { namespace cxx { namespace processor { namespace arm { namespace isa { namespace thumb {
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 24 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 27 "isa_thumb.tcc"
>
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 32 "isa_thumb.tcc"
>::Operation(CodeType _code, typename CONFIG::address_t _addr, const char *_name)
:
encoding(_code),
addr(_addr),
name(_name)
{
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 44 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 47 "isa_thumb.tcc"
>
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 52 "isa_thumb.tcc"
>::~Operation()
{
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 60 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 63 "isa_thumb.tcc"
>
#line 53 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/profiling.isa"
void
#line 67 "isa_thumb.tcc"
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 71 "isa_thumb.tcc"
>::profile(
#line 53 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/profiling.isa"
map<uint64_t, uint32_t> &
#line 75 "isa_thumb.tcc"
#line 53 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/profiling.isa"
prof
#line 78 "isa_thumb.tcc"
)
{
#line 53 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/profiling.isa"
	{
		uint64_t val = GetEncoding();
		val += (uint64_t)0x0100000000ULL;
		map<uint64_t, uint32_t>::iterator it = prof.find(val);
		if(it != prof.end()) {
			it->second = it->second + 1;
			} else {
			prof[val] = 1;
		}
	}
#line 92 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 97 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 100 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 104 "isa_thumb.tcc"
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 108 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 112 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 115 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 119 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 122 "isa_thumb.tcc"
)
{
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	{
		buffer << "Unknown instruction";
	}
#line 129 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 134 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 137 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 141 "isa_thumb.tcc"
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 145 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 149 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 152 "isa_thumb.tcc"
)
{
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	{
		throw UndefinedInstructionException<CONFIG>();
	}
#line 159 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 164 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 167 "isa_thumb.tcc"
>

void
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 174 "isa_thumb.tcc"
>::initialize_rd_index()
{
#line 89 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rd_index = 0;
	}
#line 181 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 186 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 189 "isa_thumb.tcc"
>

void
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 196 "isa_thumb.tcc"
>::initialize_rn_index()
{
#line 85 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rn_index = 0;
	}
#line 203 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 208 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 211 "isa_thumb.tcc"
>

void
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 218 "isa_thumb.tcc"
>::initialize_rm_index()
{
#line 81 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rm_index = 0;
	}
#line 225 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 230 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 233 "isa_thumb.tcc"
>

void
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 240 "isa_thumb.tcc"
>::initialize_rdp1_init()
{
#line 73 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rdp1_init = 0;
	}
#line 247 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 252 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 255 "isa_thumb.tcc"
>

void
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 262 "isa_thumb.tcc"
>::initialize_rdlo_init()
{
#line 69 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rdlo_init = 0;
	}
#line 269 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 274 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 277 "isa_thumb.tcc"
>

void
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 284 "isa_thumb.tcc"
>::initialize_rdhi_init()
{
#line 65 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rdhi_init = 0;
	}
#line 291 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 296 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 299 "isa_thumb.tcc"
>

void
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 306 "isa_thumb.tcc"
>::initialize_rs_init()
{
#line 61 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rs_init = 0;
	}
#line 313 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 318 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 321 "isa_thumb.tcc"
>

void
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 328 "isa_thumb.tcc"
>::initialize_rd_init()
{
#line 57 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rd_init = 0;
	}
#line 335 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 340 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 343 "isa_thumb.tcc"
>

void
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 350 "isa_thumb.tcc"
>::initialize_rm_init()
{
#line 53 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rm_init = 0;
	}
#line 357 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 362 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 365 "isa_thumb.tcc"
>

void
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 372 "isa_thumb.tcc"
>::initialize_rn_init()
{
#line 49 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rn_init = 0;
	}
#line 379 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 384 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 387 "isa_thumb.tcc"
>

void
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 394 "isa_thumb.tcc"
>::initialize_cond_init()
{
#line 37 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		cond_init = 0;
	}
#line 401 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 406 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 409 "isa_thumb.tcc"
>
class OpB_1_0xxx : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 414 "isa_thumb.tcc"
>
{
public:
	OpB_1_0xxx(CodeType code, typename CONFIG::address_t addr);
	uint32_t sub_cond;
	int32_t imm;
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	/**********************************************

	THUMB ARM EMULATOR ISA

	**********************************************/
	// end of decl
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
	void
	initialize_cond_init();
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 499 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 503 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 506 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 510 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 513 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 551 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 555 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 558 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 566 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 569 "isa_thumb.tcc"
>
class OpB_1_10xx : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 574 "isa_thumb.tcc"
>
{
public:
	OpB_1_10xx(CodeType code, typename CONFIG::address_t addr);
	uint32_t sub_cond;
	int32_t imm;
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	/**********************************************

	THUMB ARM EMULATOR ISA

	**********************************************/
	// end of decl
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
	void
	initialize_cond_init();
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 659 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 663 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 666 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 670 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 673 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 711 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 715 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 718 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 726 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 729 "isa_thumb.tcc"
>
class OpB_1_110x : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 734 "isa_thumb.tcc"
>
{
public:
	OpB_1_110x(CodeType code, typename CONFIG::address_t addr);
	uint32_t sub_cond;
	int32_t imm;
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	/**********************************************

	THUMB ARM EMULATOR ISA

	**********************************************/
	// end of decl
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
	void
	initialize_cond_init();
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 819 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 823 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 826 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 830 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 833 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 871 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 875 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 878 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 886 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 889 "isa_thumb.tcc"
>
class OpB_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 894 "isa_thumb.tcc"
>
{
public:
	OpB_2(CodeType code, typename CONFIG::address_t addr);
	int32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 903 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 907 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 910 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 914 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 917 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 955 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 959 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 962 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 970 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 973 "isa_thumb.tcc"
>
class OpBl : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 978 "isa_thumb.tcc"
>
{
public:
	OpBl(CodeType code, typename CONFIG::address_t addr);
	uint32_t h2;
	uint32_t offset;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 988 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 992 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 995 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 999 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1002 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1040 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1044 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1047 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 1055 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1058 "isa_thumb.tcc"
>
class OpBx : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1063 "isa_thumb.tcc"
>
{
public:
	OpBx(CodeType code, typename CONFIG::address_t addr);
	uint32_t h2;
	uint32_t rm;
	uint32_t sbz;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1074 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1078 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1081 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1085 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1088 "isa_thumb.tcc"
	);
	virtual
	void
	initialize_rm_init();
	virtual
	void
	initialize_rm_index();
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1132 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1136 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1139 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 1147 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1150 "isa_thumb.tcc"
>
class OpBlx_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1155 "isa_thumb.tcc"
>
{
public:
	OpBlx_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t offset;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1164 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1168 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1171 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1175 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1178 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1216 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1220 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1223 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 1231 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1234 "isa_thumb.tcc"
>
class OpBlx_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1239 "isa_thumb.tcc"
>
{
public:
	OpBlx_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t h2;
	uint32_t rm;
	uint32_t sbz;
	virtual
	void
	initialize_rm_init();
	virtual
	void
	initialize_rm_index();
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1256 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1260 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1263 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1267 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1270 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1308 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1312 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1315 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 1323 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1326 "isa_thumb.tcc"
>
class OpAdc : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1331 "isa_thumb.tcc"
>
{
public:
	OpAdc(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1341 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1345 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1348 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1352 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1355 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1393 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1397 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1400 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 1408 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1411 "isa_thumb.tcc"
>
class OpAdd_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1416 "isa_thumb.tcc"
>
{
public:
	OpAdd_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1427 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1431 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1434 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1438 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1441 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1479 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1483 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1486 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 1494 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1497 "isa_thumb.tcc"
>
class OpAdd_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1502 "isa_thumb.tcc"
>
{
public:
	OpAdd_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rd;
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1512 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1516 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1519 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1523 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1526 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1564 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1568 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1571 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 1579 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1582 "isa_thumb.tcc"
>
class OpAdd_3 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1587 "isa_thumb.tcc"
>
{
public:
	OpAdd_3(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1598 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1602 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1605 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1609 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1612 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1650 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1654 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1657 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 1665 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1668 "isa_thumb.tcc"
>
class OpAdd_4 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1673 "isa_thumb.tcc"
>
{
public:
	OpAdd_4(CodeType code, typename CONFIG::address_t addr);
	uint32_t h1;
	uint32_t h2;
	uint32_t rm;
	uint32_t rd;
	virtual
	void
	initialize_rm_index();
	virtual
	void
	initialize_rd_index();
	virtual
	void
	initialize_rm_init();
	virtual
	void
	initialize_rd_init();
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1697 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1701 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1704 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1708 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1711 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1749 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1753 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1756 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 1764 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1767 "isa_thumb.tcc"
>
class OpAdd_5 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1772 "isa_thumb.tcc"
>
{
public:
	OpAdd_5(CodeType code, typename CONFIG::address_t addr);
	uint32_t rd;
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1782 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1786 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1789 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1793 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1796 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1834 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1838 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1841 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 1849 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1852 "isa_thumb.tcc"
>
class OpAdd_6 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1857 "isa_thumb.tcc"
>
{
public:
	OpAdd_6(CodeType code, typename CONFIG::address_t addr);
	uint32_t rd;
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1867 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1871 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1874 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1878 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1881 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1919 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1923 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1926 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 1934 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1937 "isa_thumb.tcc"
>
class OpAdd_7 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1942 "isa_thumb.tcc"
>
{
public:
	OpAdd_7(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1951 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1955 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1958 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1962 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1965 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2003 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2007 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2010 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2018 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2021 "isa_thumb.tcc"
>
class OpAnd : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2026 "isa_thumb.tcc"
>
{
public:
	OpAnd(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2036 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2040 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2043 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2047 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2050 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2088 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2092 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2095 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2103 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2106 "isa_thumb.tcc"
>
class OpAsr_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2111 "isa_thumb.tcc"
>
{
public:
	OpAsr_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2122 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2126 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2129 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2133 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2136 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2174 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2178 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2181 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2189 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2192 "isa_thumb.tcc"
>
class OpAsr_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2197 "isa_thumb.tcc"
>
{
public:
	OpAsr_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rs;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2207 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2211 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2214 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2218 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2221 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2259 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2263 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2266 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2274 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2277 "isa_thumb.tcc"
>
class OpBic : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2282 "isa_thumb.tcc"
>
{
public:
	OpBic(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2292 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2296 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2299 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2303 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2306 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2344 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2348 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2351 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2359 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2362 "isa_thumb.tcc"
>
class OpCmn : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2367 "isa_thumb.tcc"
>
{
public:
	OpCmn(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2377 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2381 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2384 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2388 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2391 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2429 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2433 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2436 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2444 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2447 "isa_thumb.tcc"
>
class OpCmp_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2452 "isa_thumb.tcc"
>
{
public:
	OpCmp_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t rn;
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2462 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2466 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2469 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2473 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2476 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2514 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2518 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2521 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2529 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2532 "isa_thumb.tcc"
>
class OpCmp_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2537 "isa_thumb.tcc"
>
{
public:
	OpCmp_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2547 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2551 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2554 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2558 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2561 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2599 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2603 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2606 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2614 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2617 "isa_thumb.tcc"
>
class OpCmp_3 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2622 "isa_thumb.tcc"
>
{
public:
	OpCmp_3(CodeType code, typename CONFIG::address_t addr);
	uint32_t h1;
	uint32_t h2;
	uint32_t rm;
	uint32_t rn;
	virtual
	void
	initialize_rm_index();
	virtual
	void
	initialize_rn_index();
	virtual
	void
	initialize_rm_init();
	virtual
	void
	initialize_rn_init();
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2646 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2650 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2653 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2657 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2660 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2698 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2702 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2705 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2713 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2716 "isa_thumb.tcc"
>
class OpEor : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2721 "isa_thumb.tcc"
>
{
public:
	OpEor(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2731 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2735 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2738 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2742 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2745 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2783 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2787 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2790 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2798 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2801 "isa_thumb.tcc"
>
class OpLsl_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2806 "isa_thumb.tcc"
>
{
public:
	OpLsl_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2817 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2821 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2824 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2828 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2831 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2869 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2873 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2876 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2884 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2887 "isa_thumb.tcc"
>
class OpLsl_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2892 "isa_thumb.tcc"
>
{
public:
	OpLsl_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rs;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2902 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2906 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2909 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2913 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2916 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2954 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2958 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2961 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2969 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2972 "isa_thumb.tcc"
>
class OpLsr_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2977 "isa_thumb.tcc"
>
{
public:
	OpLsr_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2988 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2992 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2995 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2999 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3002 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3040 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3044 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3047 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 3055 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3058 "isa_thumb.tcc"
>
class OpLsr_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3063 "isa_thumb.tcc"
>
{
public:
	OpLsr_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rs;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3073 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3077 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3080 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3084 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3087 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3125 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3129 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3132 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 3140 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3143 "isa_thumb.tcc"
>
class OpMov_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3148 "isa_thumb.tcc"
>
{
public:
	OpMov_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t rd;
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3158 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3162 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3165 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3169 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3172 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3210 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3214 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3217 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 3225 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3228 "isa_thumb.tcc"
>
class OpMov_3 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3233 "isa_thumb.tcc"
>
{
public:
	OpMov_3(CodeType code, typename CONFIG::address_t addr);
	uint32_t h1;
	uint32_t h2;
	uint32_t rm;
	uint32_t rd;
	virtual
	void
	initialize_rm_index();
	virtual
	void
	initialize_rd_index();
	virtual
	void
	initialize_rm_init();
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3254 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3258 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3261 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3265 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3268 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3306 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3310 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3313 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 3321 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3324 "isa_thumb.tcc"
>
class OpMul : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3329 "isa_thumb.tcc"
>
{
public:
	OpMul(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3339 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3343 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3346 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3350 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3353 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3391 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3395 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3398 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 3406 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3409 "isa_thumb.tcc"
>
class OpMvn : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3414 "isa_thumb.tcc"
>
{
public:
	OpMvn(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3424 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3428 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3431 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3435 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3438 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3476 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3480 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3483 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 3491 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3494 "isa_thumb.tcc"
>
class OpNeg : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3499 "isa_thumb.tcc"
>
{
public:
	OpNeg(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3509 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3513 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3516 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3520 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3523 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3561 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3565 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3568 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 3576 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3579 "isa_thumb.tcc"
>
class OpOrr : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3584 "isa_thumb.tcc"
>
{
public:
	OpOrr(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3594 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3598 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3601 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3605 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3608 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3646 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3650 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3653 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 3661 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3664 "isa_thumb.tcc"
>
class OpRor : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3669 "isa_thumb.tcc"
>
{
public:
	OpRor(CodeType code, typename CONFIG::address_t addr);
	uint32_t rs;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3679 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3683 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3686 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3690 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3693 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3731 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3735 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3738 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 3746 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3749 "isa_thumb.tcc"
>
class OpSbc : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3754 "isa_thumb.tcc"
>
{
public:
	OpSbc(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3764 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3768 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3771 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3775 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3778 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3816 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3820 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3823 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 3831 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3834 "isa_thumb.tcc"
>
class OpSub_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3839 "isa_thumb.tcc"
>
{
public:
	OpSub_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3850 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3854 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3857 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3861 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3864 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3902 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3906 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3909 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 3917 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3920 "isa_thumb.tcc"
>
class OpSub_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3925 "isa_thumb.tcc"
>
{
public:
	OpSub_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rd;
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3935 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3939 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3942 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3946 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3949 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3987 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3991 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3994 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4002 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4005 "isa_thumb.tcc"
>
class OpSub_3 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4010 "isa_thumb.tcc"
>
{
public:
	OpSub_3(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4021 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4025 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4028 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4032 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4035 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4073 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4077 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4080 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4088 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4091 "isa_thumb.tcc"
>
class OpSub_4 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4096 "isa_thumb.tcc"
>
{
public:
	OpSub_4(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4105 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4109 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4112 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4116 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4119 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4157 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4161 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4164 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4172 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4175 "isa_thumb.tcc"
>
class OpTst : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4180 "isa_thumb.tcc"
>
{
public:
	OpTst(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4190 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4194 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4197 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4201 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4204 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4242 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4246 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4249 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4257 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4260 "isa_thumb.tcc"
>
class OpLdr_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4265 "isa_thumb.tcc"
>
{
public:
	OpLdr_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4276 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4280 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4283 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4287 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4290 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4328 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4332 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4335 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4343 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4346 "isa_thumb.tcc"
>
class OpLdr_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4351 "isa_thumb.tcc"
>
{
public:
	OpLdr_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4362 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4366 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4369 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4373 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4376 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4414 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4418 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4421 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4429 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4432 "isa_thumb.tcc"
>
class OpLdr_3 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4437 "isa_thumb.tcc"
>
{
public:
	OpLdr_3(CodeType code, typename CONFIG::address_t addr);
	uint32_t rd;
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4447 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4451 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4454 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4458 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4461 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4499 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4503 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4506 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4514 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4517 "isa_thumb.tcc"
>
class OpLdr_4 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4522 "isa_thumb.tcc"
>
{
public:
	OpLdr_4(CodeType code, typename CONFIG::address_t addr);
	uint32_t rd;
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4532 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4536 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4539 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4543 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4546 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4584 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4588 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4591 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4599 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4602 "isa_thumb.tcc"
>
class OpLdrb_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4607 "isa_thumb.tcc"
>
{
public:
	OpLdrb_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4618 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4622 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4625 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4629 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4632 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4670 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4674 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4677 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4685 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4688 "isa_thumb.tcc"
>
class OpLdrb_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4693 "isa_thumb.tcc"
>
{
public:
	OpLdrb_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4704 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4708 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4711 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4715 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4718 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4756 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4760 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4763 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4771 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4774 "isa_thumb.tcc"
>
class OpLdrh_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4779 "isa_thumb.tcc"
>
{
public:
	OpLdrh_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4790 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4794 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4797 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4801 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4804 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4842 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4846 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4849 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4857 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4860 "isa_thumb.tcc"
>
class OpLdrh_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4865 "isa_thumb.tcc"
>
{
public:
	OpLdrh_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4876 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4880 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4883 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4887 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4890 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4928 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4932 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4935 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4943 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4946 "isa_thumb.tcc"
>
class OpLdrsb : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4951 "isa_thumb.tcc"
>
{
public:
	OpLdrsb(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4962 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4966 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4969 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4973 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4976 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5014 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5018 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5021 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5029 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5032 "isa_thumb.tcc"
>
class OpLdrsh : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5037 "isa_thumb.tcc"
>
{
public:
	OpLdrsh(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5048 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5052 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5055 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5059 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5062 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5100 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5104 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5107 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5115 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5118 "isa_thumb.tcc"
>
class OpStr_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5123 "isa_thumb.tcc"
>
{
public:
	OpStr_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5134 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5138 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5141 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5145 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5148 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5186 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5190 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5193 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5201 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5204 "isa_thumb.tcc"
>
class OpStr_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5209 "isa_thumb.tcc"
>
{
public:
	OpStr_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5220 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5224 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5227 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5231 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5234 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5272 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5276 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5279 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5287 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5290 "isa_thumb.tcc"
>
class OpStr_3 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5295 "isa_thumb.tcc"
>
{
public:
	OpStr_3(CodeType code, typename CONFIG::address_t addr);
	uint32_t rd;
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5305 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5309 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5312 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5316 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5319 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5357 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5361 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5364 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5372 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5375 "isa_thumb.tcc"
>
class OpStrb_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5380 "isa_thumb.tcc"
>
{
public:
	OpStrb_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5391 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5395 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5398 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5402 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5405 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5443 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5447 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5450 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5458 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5461 "isa_thumb.tcc"
>
class OpStrb_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5466 "isa_thumb.tcc"
>
{
public:
	OpStrb_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5477 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5481 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5484 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5488 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5491 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5529 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5533 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5536 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5544 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5547 "isa_thumb.tcc"
>
class OpStrh_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5552 "isa_thumb.tcc"
>
{
public:
	OpStrh_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5563 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5567 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5570 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5574 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5577 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5615 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5619 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5622 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5630 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5633 "isa_thumb.tcc"
>
class OpStrh_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5638 "isa_thumb.tcc"
>
{
public:
	OpStrh_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5649 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5653 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5656 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5660 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5663 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5701 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5705 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5708 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5716 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5719 "isa_thumb.tcc"
>
class OpLdmia : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5724 "isa_thumb.tcc"
>
{
public:
	OpLdmia(CodeType code, typename CONFIG::address_t addr);
	uint32_t rn;
	uint32_t reg_list;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5734 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5738 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5741 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5745 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5748 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5786 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5790 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5793 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5801 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5804 "isa_thumb.tcc"
>
class OpPop : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5809 "isa_thumb.tcc"
>
{
public:
	OpPop(CodeType code, typename CONFIG::address_t addr);
	uint32_t r;
	uint32_t reg_list;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5819 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5823 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5826 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5830 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5833 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5871 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5875 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5878 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5886 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5889 "isa_thumb.tcc"
>
class OpPush : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5894 "isa_thumb.tcc"
>
{
public:
	OpPush(CodeType code, typename CONFIG::address_t addr);
	uint32_t r;
	uint32_t reg_list;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5904 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5908 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5911 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5915 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5918 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5956 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5960 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5963 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5971 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5974 "isa_thumb.tcc"
>
class OpStmia : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5979 "isa_thumb.tcc"
>
{
public:
	OpStmia(CodeType code, typename CONFIG::address_t addr);
	uint32_t rn;
	uint32_t reg_list;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5989 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5993 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5996 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 6000 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 6003 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 6041 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 6045 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 6048 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6056 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6059 "isa_thumb.tcc"
>
class OpBkpt : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6064 "isa_thumb.tcc"
>
{
public:
	OpBkpt(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 6073 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 6077 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 6080 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 6084 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 6087 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 6125 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 6129 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 6132 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6140 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6143 "isa_thumb.tcc"
>
class OpSwi : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6148 "isa_thumb.tcc"
>
{
public:
	OpSwi(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 6157 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 6161 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 6164 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 6168 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 6171 "isa_thumb.tcc"
	);
	/*
	*  Copyright (c) 2007,
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
	* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
	*/
	virtual
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 6209 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 6213 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 6216 "isa_thumb.tcc"
	);
private:
};

/*
*  Copyright (c) 2007,
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
* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
*/
/**********************************************

THUMB BRANCH INSTRUCTIONS

**********************************************/
/*******************************************************************
* b(1) (conditional branch) instruction
* op b_1(0b1101[4]:cond[4]:sext<32> imm[8])
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6266 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6269 "isa_thumb.tcc"
>

void
OpB_1_0xxx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6276 "isa_thumb.tcc"
>::initialize_cond_init()
{
#line 50 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::cond_init = sub_cond;
	}
#line 6285 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6290 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6293 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6297 "isa_thumb.tcc"
OpB_1_0xxx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6301 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6305 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6308 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 6312 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 6315 "isa_thumb.tcc"
)
{
#line 70 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;
		buffer << "b";
		cpu.DisasmCondition(inherited::cond_init, buffer);
		buffer << " 0x" << hex << cpu.GetGPR(cpu.PC_reg) + 4 + (imm << 1) << dec;
	}
#line 6325 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6330 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6333 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6337 "isa_thumb.tcc"
OpB_1_0xxx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6341 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6345 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6348 "isa_thumb.tcc"
)
{
#line 77 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		typename CONFIG::reg_t new_PC;

		if(!cpu.CheckCondition(inherited::cond_init)) {
			cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);
			return;
		}

		new_PC = cpu.GetGPR(cpu.PC_reg) + 4 + (imm << 1);
		cpu.SetGPR(cpu.PC_reg, new_PC);
	}
#line 6365 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6371 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6374 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6379 "isa_thumb.tcc"
> *DecodeOpB_1_0xxx(CodeType code, typename CONFIG::address_t addr)
{
	return new OpB_1_0xxx<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 6385 "isa_thumb.tcc"
	>(code, addr);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6392 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6395 "isa_thumb.tcc"
>

void
OpB_1_10xx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6402 "isa_thumb.tcc"
>::initialize_cond_init()
{
#line 56 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::cond_init = sub_cond + 8;
	}
#line 6411 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6416 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6419 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6423 "isa_thumb.tcc"
OpB_1_10xx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6427 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6431 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6434 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 6438 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 6441 "isa_thumb.tcc"
)
{
#line 70 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;
		buffer << "b";
		cpu.DisasmCondition(inherited::cond_init, buffer);
		buffer << " 0x" << hex << cpu.GetGPR(cpu.PC_reg) + 4 + (imm << 1) << dec;
	}
#line 6451 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6456 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6459 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6463 "isa_thumb.tcc"
OpB_1_10xx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6467 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6471 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6474 "isa_thumb.tcc"
)
{
#line 77 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		typename CONFIG::reg_t new_PC;

		if(!cpu.CheckCondition(inherited::cond_init)) {
			cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);
			return;
		}

		new_PC = cpu.GetGPR(cpu.PC_reg) + 4 + (imm << 1);
		cpu.SetGPR(cpu.PC_reg, new_PC);
	}
#line 6491 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6497 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6500 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6505 "isa_thumb.tcc"
> *DecodeOpB_1_10xx(CodeType code, typename CONFIG::address_t addr)
{
	return new OpB_1_10xx<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 6511 "isa_thumb.tcc"
	>(code, addr);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6518 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6521 "isa_thumb.tcc"
>

void
OpB_1_110x<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6528 "isa_thumb.tcc"
>::initialize_cond_init()
{
#line 62 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::cond_init = sub_cond + 12;
	}
#line 6537 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6542 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6545 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6549 "isa_thumb.tcc"
OpB_1_110x<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6553 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6557 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6560 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 6564 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 6567 "isa_thumb.tcc"
)
{
#line 70 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;
		buffer << "b";
		cpu.DisasmCondition(inherited::cond_init, buffer);
		buffer << " 0x" << hex << cpu.GetGPR(cpu.PC_reg) + 4 + (imm << 1) << dec;
	}
#line 6577 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6582 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6585 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6589 "isa_thumb.tcc"
OpB_1_110x<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6593 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6597 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6600 "isa_thumb.tcc"
)
{
#line 77 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		typename CONFIG::reg_t new_PC;

		if(!cpu.CheckCondition(inherited::cond_init)) {
			cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);
			return;
		}

		new_PC = cpu.GetGPR(cpu.PC_reg) + 4 + (imm << 1);
		cpu.SetGPR(cpu.PC_reg, new_PC);
	}
#line 6617 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6623 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6626 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6631 "isa_thumb.tcc"
> *DecodeOpB_1_110x(CodeType code, typename CONFIG::address_t addr)
{
	return new OpB_1_110x<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 6637 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of b(1) (conditional branch) instruction
*******************************************************************/
/*******************************************************************
* b(2) (unconditional branch) instruction
* op b_2(0b11100[5]:sext<32> imm[11])
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6651 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6654 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6658 "isa_thumb.tcc"
OpB_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6662 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6666 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6669 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 6673 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 6676 "isa_thumb.tcc"
)
{
#line 102 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		buffer << "b";
		buffer << " 0x" << hex << cpu.GetGPR(cpu.PC_reg) + 4 + (imm << 1) << dec;
	}
#line 6686 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6691 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6694 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6698 "isa_thumb.tcc"
OpB_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6702 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6706 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6709 "isa_thumb.tcc"
)
{
#line 109 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		typename CONFIG::reg_t new_PC;

		new_PC = cpu.GetGPR(cpu.PC_reg) + 4 + (imm << 1);
		cpu.SetGPR(cpu.PC_reg, new_PC);
	}
#line 6721 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6727 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6730 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6735 "isa_thumb.tcc"
> *DecodeOpB_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpB_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 6741 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of b(2) (unconditional branch) instruction
*******************************************************************/
/*******************************************************************
* bl (branch with link) instruction
* op bl(0b111[3]:0b1[1]:h2[1]:offset[11])
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6755 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6758 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6762 "isa_thumb.tcc"
OpBl<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6766 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6770 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6773 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 6777 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 6780 "isa_thumb.tcc"
)
{
#line 129 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		typename CONFIG::sreg_t bl_soffset;

		buffer << "bl ";
		if(h2 == 0) {
			buffer << "????";
			return;
			} else {
			bl_soffset = cpu.GetGPR(cpu.LR_reg) + (offset << 1);
			buffer << "0x" << hex << bl_soffset << dec;
		}
	}
#line 6798 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6803 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6806 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6810 "isa_thumb.tcc"
OpBl<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6814 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6818 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6821 "isa_thumb.tcc"
)
{
#line 144 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		typename CONFIG::reg_t new_PC;

		typename CONFIG::sreg_t bl_soffset;

		if(offset & 0x0400)
		bl_soffset = (int32_t)(UINT32_C(0xfffff800) + offset);
		else
		bl_soffset = offset;

		if(h2) {
			new_PC = (cpu.GetGPR(cpu.LR_reg) + (offset << 1));
			cpu.SetGPR(cpu.LR_reg, cpu.GetGPR(cpu.PC_reg) + 3);
			cpu.SetGPR(cpu.PC_reg, new_PC);
			} else {
			cpu.SetGPR(cpu.LR_reg, cpu.GetGPR(cpu.PC_reg) + 4 + (bl_soffset << 12));
			cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);
		}
	}
#line 6846 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6852 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6855 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6860 "isa_thumb.tcc"
> *DecodeOpBl(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBl<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 6866 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of bl (branch with link) instruction
*******************************************************************/
/*******************************************************************
* bx (branch and exchange) instruction
* op bx(0b010001110[9]:h2[1]:rm[3]:sbz[3])
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6880 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6883 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6887 "isa_thumb.tcc"
OpBx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6891 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6895 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6898 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 6902 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 6905 "isa_thumb.tcc"
)
{
#line 177 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		buffer << "bx "
		<< "r" << inherited::rm_index << endl;
	}
#line 6915 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6920 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6923 "isa_thumb.tcc"
>

void
OpBx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6930 "isa_thumb.tcc"
>::initialize_rm_init()
{
#line 184 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		if((h2 << 3) + rm == CPU<CONFIG>::PC_reg)
		inherited::rm_init = 4;
		else
		inherited::rm_init = 0;
	}
#line 6942 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6947 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6950 "isa_thumb.tcc"
>

void
OpBx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6957 "isa_thumb.tcc"
>::initialize_rm_index()
{
#line 193 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::rm_index = (h2 << 3) + rm;
	}
#line 6966 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6971 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6974 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6978 "isa_thumb.tcc"
OpBx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6982 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6986 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6989 "isa_thumb.tcc"
)
{
#line 199 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		typename CONFIG::reg_t rm_value;

		rm_value = inherited::rm_init + cpu.GetGPR(inherited::rm_index);

		if(rm_value & 1) {
			cpu.SetCPSR_T(true);
			} else {
			cpu.SetCPSR_T(false);
		}

		cpu.SetGPR(cpu.PC_reg, rm_value & ~(typename CONFIG::reg_t)1);
	}
#line 7008 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7014 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7017 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7022 "isa_thumb.tcc"
> *DecodeOpBx(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBx<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 7028 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of bx (branch and exchange) instruction
*******************************************************************/
/*******************************************************************
* blx(1) (branch with link and exchange) instruction
* op blx_1(0b111[3]:0b01[2]:sext<32> imm[11])
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7042 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7045 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7049 "isa_thumb.tcc"
OpBlx_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7053 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7057 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7060 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 7064 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 7067 "isa_thumb.tcc"
)
{
#line 226 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		typename CONFIG::sreg_t blx_soffset;

		if(offset & 0x0400)
		blx_soffset = (int32_t)((~(uint32_t)0x0400) + offset);
		else
		blx_soffset = offset;

		buffer << "blx";
		buffer << " 0x" << hex
		<< ((blx_soffset << 12) + cpu.GetGPR(cpu.PC_reg) + 4 + (offset << 1))
		<< dec;
	}
#line 7086 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7091 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7094 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7098 "isa_thumb.tcc"
OpBlx_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7102 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7106 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7109 "isa_thumb.tcc"
)
{
#line 242 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typename CONFIG::reg_t new_PC;

		new_PC = (cpu.GetGPR(cpu.LR_reg) + (offset << 1)) & (typename CONFIG::reg_t)0xFFFFFFFC;
		cpu.SetGPR(cpu.LR_reg, cpu.GetGPR(cpu.PC_reg) + 3);
		cpu.SetGPR(cpu.PC_reg, new_PC);
	}
#line 7120 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7126 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7129 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7134 "isa_thumb.tcc"
> *DecodeOpBlx_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBlx_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 7140 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of blx(1) (branch with link and exchange) instruction
*******************************************************************/
/*******************************************************************
* blx(2) (branch with link and exchange) instruction
* op blx_2(0b111[3]:0b01[2]:sext<32> imm[11])
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7154 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7157 "isa_thumb.tcc"
>

void
OpBlx_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7164 "isa_thumb.tcc"
>::initialize_rm_init()
{
#line 261 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		if((h2 << 3) + rm == CPU<CONFIG>::PC_reg)
		inherited::rm_init = 4;
		else
		inherited::rm_init = 0;
	}
#line 7176 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7181 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7184 "isa_thumb.tcc"
>

void
OpBlx_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7191 "isa_thumb.tcc"
>::initialize_rm_index()
{
#line 270 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::rm_index = (h2 << 3) + rm;
	}
#line 7200 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7205 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7208 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7212 "isa_thumb.tcc"
OpBlx_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7216 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7220 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7223 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 7227 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 7230 "isa_thumb.tcc"
)
{
#line 276 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		buffer << "blx r" << inherited::rm_index;
	}
#line 7239 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7244 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7247 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7251 "isa_thumb.tcc"
OpBlx_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7255 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7259 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7262 "isa_thumb.tcc"
)
{
#line 282 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;
		typename CONFIG::reg_t rm_value;

		rm_value = inherited::rm_init + cpu.GetGPR(inherited::rm_index);

		cpu.SetGPR(cpu.LR_reg, cpu.GetGPR(cpu.PC_reg) + 3);
		if(rm_value & 1) {
			cpu.SetCPSR_T(true);
			} else {
			cpu.SetCPSR_T(false);
		}

		cpu.SetGPR(cpu.PC_reg, rm_value & ~(typename CONFIG::reg_t)1);
	}
#line 7281 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7287 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7290 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7295 "isa_thumb.tcc"
> *DecodeOpBlx_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBlx_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 7301 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of blx(2) (branch with link and exchange) instruction
*******************************************************************/
/*
*  Copyright (c) 2007,
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
* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
*/
/**********************************************

THUMB DATA PROCESSING INSTRUCTIONS

**********************************************/
/*******************************************************************
* adc (add with carry) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7352 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7355 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7359 "isa_thumb.tcc"
OpAdc<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7363 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7367 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7370 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 7374 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 7377 "isa_thumb.tcc"
)
{
#line 48 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "adc r" << rd << ", r" << rm;
	}
#line 7384 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7389 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7392 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7396 "isa_thumb.tcc"
OpAdc<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7400 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7404 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7407 "isa_thumb.tcc"
)
{
#line 52 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res;
		typename CONFIG::reg_t s1, s2;
		uint8_t carry_in, carry_out, overflow_out, sign;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		carry_in = (cpu.GetCPSR_C() ? 1 : 0);
		s1 = cpu.GetGPR(rd);
		s2 = cpu.GetGPR(rm);
		Add32(res, carry_out, overflow_out, sign,
		s1, s2, carry_in);
		cpu.SetGPR(rd, res);

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
		cpu.SetCPSR_C(carry_out); // to check
		cpu.SetCPSR_V(overflow_out); // to check
	}
#line 7430 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7436 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7439 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7444 "isa_thumb.tcc"
> *DecodeOpAdc(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAdc<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 7450 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of adc (add with carry) instruction
*******************************************************************/
/*******************************************************************
* add(1) (add immediate) instruction
* NOTE: mov(2) is equivalent to add(1)
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7464 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7467 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7471 "isa_thumb.tcc"
OpAdd_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7475 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7479 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7482 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 7486 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 7489 "isa_thumb.tcc"
)
{
#line 83 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "add r" << rd << ", r" << rn << ", #" << dec << imm;
	}
#line 7496 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7501 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7504 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7508 "isa_thumb.tcc"
OpAdd_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7512 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7516 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7519 "isa_thumb.tcc"
)
{
#line 87 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res;
		typename CONFIG::reg_t s1;
		uint8_t carry_in, carry_out, overflow_out, sign;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(rn);
		carry_in = 0;
		Add32(res, carry_out, overflow_out, sign,
		s1, imm, carry_in);
		cpu.SetGPR(rd, res);

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
		cpu.SetCPSR_C(carry_out); // to check
		cpu.SetCPSR_V(overflow_out); // to check
	}
#line 7541 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7547 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7550 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7555 "isa_thumb.tcc"
> *DecodeOpAdd_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAdd_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 7561 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of add(1) (add immediate) instruction
*******************************************************************/
/*******************************************************************
* add (add large immediate) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7574 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7577 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7581 "isa_thumb.tcc"
OpAdd_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7585 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7589 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7592 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 7596 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 7599 "isa_thumb.tcc"
)
{
#line 116 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "add r" << rd << ", #" << dec << imm;
	}
#line 7606 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7611 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7614 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7618 "isa_thumb.tcc"
OpAdd_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7622 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7626 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7629 "isa_thumb.tcc"
)
{
#line 120 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res;
		typename CONFIG::reg_t s1;
		uint8_t carry_in, carry_out, overflow_out, sign;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(rd);
		carry_in = 0;
		Add32(res, carry_out, overflow_out, sign,
		s1, imm, carry_in);

		cpu.SetGPR(rd, res);
		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
		cpu.SetCPSR_C(carry_out); // to check
		cpu.SetCPSR_V(overflow_out); // to check
	}
#line 7651 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7657 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7660 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7665 "isa_thumb.tcc"
> *DecodeOpAdd_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAdd_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 7671 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of add (add large immediate) instruction
*******************************************************************/
/*******************************************************************
* add (add register) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7684 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7687 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7691 "isa_thumb.tcc"
OpAdd_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7695 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7699 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7702 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 7706 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 7709 "isa_thumb.tcc"
)
{
#line 149 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "add r" << rd << ", r" << rn << ", r" << rm;
	}
#line 7716 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7721 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7724 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7728 "isa_thumb.tcc"
OpAdd_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7732 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7736 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7739 "isa_thumb.tcc"
)
{
#line 153 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res;
		typename CONFIG::reg_t s1, s2;
		uint8_t carry_in, carry_out, overflow_out, sign;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(rm);
		s2 = cpu.GetGPR(rn);

		carry_in = 0;
		Add32(res, carry_out, overflow_out, sign,
		s1, s2, carry_in);

		cpu.SetGPR(rd, res);
		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
		cpu.SetCPSR_C(carry_out); // to check
		cpu.SetCPSR_V(overflow_out); // to check
	}
#line 7763 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7769 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7772 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7777 "isa_thumb.tcc"
> *DecodeOpAdd_3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAdd_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 7783 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of add (add register) instruction
*******************************************************************/
/*******************************************************************
* add (add high registers) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7796 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7799 "isa_thumb.tcc"
>

void
OpAdd_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7806 "isa_thumb.tcc"
>::initialize_rm_index()
{
#line 184 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::rm_index = (h2 << 3) + rm;
	}
#line 7815 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7820 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7823 "isa_thumb.tcc"
>

void
OpAdd_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7830 "isa_thumb.tcc"
>::initialize_rd_index()
{
#line 190 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::rd_index = (h1 << 3) + rd;
	}
#line 7839 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7844 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7847 "isa_thumb.tcc"
>

void
OpAdd_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7854 "isa_thumb.tcc"
>::initialize_rm_init()
{
#line 196 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		if((h2 << 3) + rm == CPU<CONFIG>::PC_reg)
		inherited::rm_init = 4;
	}
#line 7864 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7869 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7872 "isa_thumb.tcc"
>

void
OpAdd_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7879 "isa_thumb.tcc"
>::initialize_rd_init()
{
#line 203 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		if((h1 << 3) + rd == CPU<CONFIG>::PC_reg)
		inherited::rd_init = 4;
	}
#line 7889 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7894 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7897 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7901 "isa_thumb.tcc"
OpAdd_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7905 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7909 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7912 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 7916 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 7919 "isa_thumb.tcc"
)
{
#line 210 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		buffer << "add r" << inherited::rd_index << ", r" << inherited::rm_index;
	}
#line 7928 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7933 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7936 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7940 "isa_thumb.tcc"
OpAdd_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7944 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7948 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7951 "isa_thumb.tcc"
)
{
#line 216 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		typename CONFIG::reg_t s1, s2, res;
		s1 = cpu.GetGPR(inherited::rd_index) + inherited::rd_init;
		s2 = cpu.GetGPR(inherited::rm_index) + inherited::rm_init;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		res = s1 + s2;
		cpu.SetGPR(inherited::rd_index, res);
	}
#line 7967 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7973 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7976 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7981 "isa_thumb.tcc"
> *DecodeOpAdd_4(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAdd_4<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 7987 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of add (add high registers) instruction
*******************************************************************/
/*******************************************************************
* add (add immediate to program counter) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8000 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8003 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8007 "isa_thumb.tcc"
OpAdd_5<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8011 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8015 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8018 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8022 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8025 "isa_thumb.tcc"
)
{
#line 239 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "add r" << rd << ", PC, #" << dec << ((unsigned int)imm * 4);
	}
#line 8032 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8037 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8040 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8044 "isa_thumb.tcc"
OpAdd_5<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8048 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8052 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8055 "isa_thumb.tcc"
)
{
#line 243 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t s1, s2, res;
		typename CONFIG::reg_t pc;

		pc = cpu.GetGPR(cpu.PC_reg);
		cpu.SetGPR(cpu.PC_reg, pc + 2);

		s1 = pc & (typename CONFIG::reg_t)0xfffffffc;
		s2 = ((typename CONFIG::reg_t)imm) << 2;
		res = s1 + s2;

		cpu.SetGPR(rd, res);
	}
#line 8072 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8078 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8081 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8086 "isa_thumb.tcc"
> *DecodeOpAdd_5(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAdd_5<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 8092 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of add (add immediate to program counter) instruction
*******************************************************************/
/*******************************************************************
* add (add immediate to stack pointer) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8105 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8108 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8112 "isa_thumb.tcc"
OpAdd_6<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8116 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8120 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8123 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8127 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8130 "isa_thumb.tcc"
)
{
#line 267 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "add r" << rd << ", SP, #" << dec << ((unsigned int)imm * 4);
	}
#line 8137 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8142 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8145 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8149 "isa_thumb.tcc"
OpAdd_6<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8153 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8157 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8160 "isa_thumb.tcc"
)
{
#line 271 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t s1, s2, res;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(cpu.SP_reg) & (typename CONFIG::reg_t)0xfffffffc;
		s2 = ((typename CONFIG::reg_t)imm) << 2;
		res = s1 + s2;

		cpu.SetGPR(rd, res);
	}
#line 8175 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8181 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8184 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8189 "isa_thumb.tcc"
> *DecodeOpAdd_6(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAdd_6<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 8195 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of add (add immediate to stack pointer) instruction
*******************************************************************/
/*******************************************************************
* add (increment stack pointer) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8208 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8211 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8215 "isa_thumb.tcc"
OpAdd_7<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8219 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8223 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8226 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8230 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8233 "isa_thumb.tcc"
)
{
#line 293 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "add SP, #" << (unsigned int)imm * 4;
	}
#line 8240 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8245 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8248 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8252 "isa_thumb.tcc"
OpAdd_7<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8256 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8260 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8263 "isa_thumb.tcc"
)
{
#line 297 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		cpu.SetGPR(cpu.SP_reg, cpu.GetGPR(cpu.SP_reg) + ((typename CONFIG::reg_t)imm * 4));
	}
#line 8272 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8278 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8281 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8286 "isa_thumb.tcc"
> *DecodeOpAdd_7(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAdd_7<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 8292 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of add (increment stack pointer) instruction
*******************************************************************/
/*******************************************************************
* and (logical and) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8305 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8308 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8312 "isa_thumb.tcc"
OpAnd<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8316 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8320 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8323 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8327 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8330 "isa_thumb.tcc"
)
{
#line 313 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "and r" << rd << ", r" << rm;
	}
#line 8337 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8342 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8345 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8349 "isa_thumb.tcc"
OpAnd<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8353 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8357 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8360 "isa_thumb.tcc"
)
{
#line 317 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		res = cpu.GetGPR(rd) & cpu.GetGPR(rm);
		cpu.SetGPR(rd,res);

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
	}
#line 8375 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8381 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8384 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8389 "isa_thumb.tcc"
> *DecodeOpAnd(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAnd<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 8395 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of and (logical and) instruction
*******************************************************************/
/*******************************************************************
* asr(1) (arithmetic shift right immediate) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8408 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8411 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8415 "isa_thumb.tcc"
OpAsr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8419 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8423 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8426 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8430 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8433 "isa_thumb.tcc"
)
{
#line 339 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "asr r" << rd << ", r" << rm << ", #" << dec << imm;
	}
#line 8440 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8445 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8448 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8452 "isa_thumb.tcc"
OpAsr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8456 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8460 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8463 "isa_thumb.tcc"
)
{
#line 343 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res, s1;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(rm);
		if(imm == 0) {
			if(s1 & (typename CONFIG::reg_t)0x80000000) {
				cpu.SetCPSR_C(true);
				res = (typename CONFIG::reg_t)0xffffffff;
				} else {
				cpu.SetCPSR_C(false);
				res = 0;
			}
			} else {
			cpu.SetCPSR_C((s1 >> (imm - 1)) & (typename CONFIG::reg_t)0x01);
			res = ((typename CONFIG::sreg_t)s1) >> imm;
		}

		cpu.SetGPR(rd, res);

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
	}
#line 8491 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8497 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8500 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8505 "isa_thumb.tcc"
> *DecodeOpAsr_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAsr_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 8511 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of asr(1) (arithmetic shift right immediate) instruction
*******************************************************************/
/*******************************************************************
* asr(2) (arithmetic shift right register) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8524 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8527 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8531 "isa_thumb.tcc"
OpAsr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8535 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8539 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8542 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8546 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8549 "isa_thumb.tcc"
)
{
#line 378 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "asr r" << rd << ", r" << rs;
	}
#line 8556 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8561 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8564 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8568 "isa_thumb.tcc"
OpAsr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8572 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8576 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8579 "isa_thumb.tcc"
)
{
#line 382 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res, s1;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(rs) & (typename CONFIG::reg_t)0x0ff;

		res = cpu.GetGPR(rd);
		if(s1 == 0) {
			} else if(s1 < 32) {
			cpu.SetCPSR_C((res >> (s1 - 1)) & (typename CONFIG::reg_t)0x01);
			res = ((typename CONFIG::sreg_t)res) >> s1;
			cpu.SetGPR(rd,res);
			} else {
			bool res31;

			res31 = (res & (typename CONFIG::reg_t)0x80000000) != 0;
			cpu.SetCPSR_C(res31);
			if(res31)
			res = (typename CONFIG::reg_t)0xffffffff;
			else
			res = 0;
			cpu.SetGPR(rd,res);
		}

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
	}
#line 8611 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8617 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8620 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8625 "isa_thumb.tcc"
> *DecodeOpAsr_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAsr_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 8631 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of asr(2) (arithmetic shift right register) instruction
*******************************************************************/
/*******************************************************************
* bic (bit clear) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8644 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8647 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8651 "isa_thumb.tcc"
OpBic<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8655 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8659 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8662 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8666 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8669 "isa_thumb.tcc"
)
{
#line 421 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "bic r" << rd << ", r" << rm;
	}
#line 8676 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8681 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8684 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8688 "isa_thumb.tcc"
OpBic<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8692 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8696 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8699 "isa_thumb.tcc"
)
{
#line 425 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res, s1, s2;
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(rd);
		s2 = cpu.GetGPR(rm);

		res = s1 & ~s2;

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
	}
#line 8715 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8721 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8724 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8729 "isa_thumb.tcc"
> *DecodeOpBic(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBic<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 8735 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of bic (bit clear) instruction
*******************************************************************/
/*******************************************************************
* cmn (compare negative) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8748 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8751 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8755 "isa_thumb.tcc"
OpCmn<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8759 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8763 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8766 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8770 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8773 "isa_thumb.tcc"
)
{
#line 448 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "cmn r" << rn << ", r" << rm;
	}
#line 8780 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8785 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8788 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8792 "isa_thumb.tcc"
OpCmn<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8796 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8800 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8803 "isa_thumb.tcc"
)
{
#line 452 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res, s1, s2;
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);
		uint8_t carry_in, carry_out, overflow_out, sign;

		s1 = cpu.GetGPR(rn);
		s2 = cpu.GetGPR(rm);

		carry_in = 0;
		Add32(res, carry_out, overflow_out, sign,
		s1, s2, carry_in);

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
		cpu.SetCPSR_C(carry_out); // to check
		cpu.SetCPSR_V(overflow_out); // to check
	}
#line 8824 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8830 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8833 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8838 "isa_thumb.tcc"
> *DecodeOpCmn(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCmn<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 8844 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of cmn (compare negative) instruction
*******************************************************************/
/*******************************************************************
* cmp(1) (compare immediate) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8857 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8860 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8864 "isa_thumb.tcc"
OpCmp_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8868 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8872 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8875 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8879 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8882 "isa_thumb.tcc"
)
{
#line 479 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "cmp r" << rn << ", #" << dec << imm;
	}
#line 8889 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8894 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8897 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8901 "isa_thumb.tcc"
OpCmp_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8905 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8909 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8912 "isa_thumb.tcc"
)
{
#line 483 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res, s1, s2;
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(rn);
		s2 = imm;

		res = s1 - s2;

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
		cpu.SetCPSR_C(!cpu.BorrowFrom(res, s1, s2));
		cpu.SetCPSR_V(cpu.SubtractionOverflowFrom(res, s1, s2));
	}
#line 8930 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8936 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8939 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8944 "isa_thumb.tcc"
> *DecodeOpCmp_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCmp_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 8950 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of cmp(1) (compare immediate) instruction
*******************************************************************/
/*******************************************************************
* cmp(2) (compare register) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8963 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8966 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8970 "isa_thumb.tcc"
OpCmp_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8974 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8978 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8981 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8985 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8988 "isa_thumb.tcc"
)
{
#line 507 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "cmp r" << rn << ", r" << rm;
	}
#line 8995 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9000 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9003 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9007 "isa_thumb.tcc"
OpCmp_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9011 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9015 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9018 "isa_thumb.tcc"
)
{
#line 511 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res, s1, s2;
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(rn);
		s2 = cpu.GetGPR(rm);

		res = s1 - s2;

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
		cpu.SetCPSR_C(!cpu.BorrowFrom(res, s1, s2));
		cpu.SetCPSR_V(cpu.SubtractionOverflowFrom(res, s1, s2));
	}
#line 9036 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9042 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9045 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9050 "isa_thumb.tcc"
> *DecodeOpCmp_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCmp_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 9056 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of cmp(2) (compare register) instruction
*******************************************************************/
/*******************************************************************
* cmp(3) (compare high registers) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9069 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9072 "isa_thumb.tcc"
>

void
OpCmp_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9079 "isa_thumb.tcc"
>::initialize_rm_index()
{
#line 536 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::rm_index = (h2 << 3) + rm;
	}
#line 9088 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9093 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9096 "isa_thumb.tcc"
>

void
OpCmp_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9103 "isa_thumb.tcc"
>::initialize_rn_index()
{
#line 542 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::rn_index = (h1 << 3) + rn;
	}
#line 9112 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9117 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9120 "isa_thumb.tcc"
>

void
OpCmp_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9127 "isa_thumb.tcc"
>::initialize_rm_init()
{
#line 548 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		if((h2 << 3) + rm == CPU<CONFIG>::PC_reg)
		inherited::rm_init = 4;
		else
		inherited::rm_init = 0;
	}
#line 9139 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9144 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9147 "isa_thumb.tcc"
>

void
OpCmp_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9154 "isa_thumb.tcc"
>::initialize_rn_init()
{
#line 557 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		if((h1 << 3) + rn == CPU<CONFIG>::PC_reg)
		inherited::rn_init = 4;
		else
		inherited::rn_init = 0;
	}
#line 9166 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9171 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9174 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9178 "isa_thumb.tcc"
OpCmp_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9182 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9186 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9189 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 9193 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 9196 "isa_thumb.tcc"
)
{
#line 566 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		buffer << "cmp r" << inherited::rn_index << ", r" << inherited::rm_index;
	}
#line 9205 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9210 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9213 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9217 "isa_thumb.tcc"
OpCmp_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9221 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9225 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9228 "isa_thumb.tcc"
)
{
#line 572 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;
		typename CONFIG::reg_t res, s1, s2;
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(inherited::rn_index) + inherited::rn_init;
		s2 = cpu.GetGPR(inherited::rm_index) + inherited::rm_init;

		res = s1 - s2;

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
		cpu.SetCPSR_C(!cpu.BorrowFrom(res, s1, s2));
		cpu.SetCPSR_V(cpu.SubtractionOverflowFrom(res, s1, s2));
	}
#line 9247 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9253 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9256 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9261 "isa_thumb.tcc"
> *DecodeOpCmp_3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCmp_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 9267 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of cmp(3) (compare high registers) instruction
*******************************************************************/
/*******************************************************************
* eor (exclusive or) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9280 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9283 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9287 "isa_thumb.tcc"
OpEor<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9291 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9295 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9298 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 9302 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 9305 "isa_thumb.tcc"
)
{
#line 598 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "eor r" << rd << ", r" << rm;
	}
#line 9312 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9317 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9320 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9324 "isa_thumb.tcc"
OpEor<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9328 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9332 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9335 "isa_thumb.tcc"
)
{
#line 602 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res, s1, s2;
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(rd);
		s2 = cpu.GetGPR(rm);

		res = s1 ^ s2;

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
	}
#line 9351 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9357 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9360 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9365 "isa_thumb.tcc"
> *DecodeOpEor(CodeType code, typename CONFIG::address_t addr)
{
	return new OpEor<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 9371 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of eor (exclusive or) instruction
*******************************************************************/
/*******************************************************************
* lsl(1) (logical shift left immediate) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9384 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9387 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9391 "isa_thumb.tcc"
OpLsl_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9395 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9399 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9402 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 9406 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 9409 "isa_thumb.tcc"
)
{
#line 624 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "lsl r" << rd << ", r" << rm << ", #" << dec << imm;
	}
#line 9416 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9421 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9424 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9428 "isa_thumb.tcc"
OpLsl_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9432 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9436 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9439 "isa_thumb.tcc"
)
{
#line 628 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res, s1, s2;
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(rm);
		s2 = imm;

		if(imm == 0) {
			res = s1;
			} else {
			cpu.SetCPSR_C((s1 >> (32 - imm)) & (typename CONFIG::reg_t)0x01);
			res = s1 << imm;
		}
		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
	}
#line 9459 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9465 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9468 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9473 "isa_thumb.tcc"
> *DecodeOpLsl_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLsl_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 9479 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of lsl(1) (logical shift left immediate) instruction
*******************************************************************/
/*******************************************************************
* lsl(2) (logical shift left register) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9492 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9495 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9499 "isa_thumb.tcc"
OpLsl_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9503 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9507 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9510 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 9514 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 9517 "isa_thumb.tcc"
)
{
#line 655 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "lsl r" << rd << ", r" << rs;
	}
#line 9524 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9529 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9532 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9536 "isa_thumb.tcc"
OpLsl_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9540 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9544 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9547 "isa_thumb.tcc"
)
{
#line 659 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res, s1, s2;
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(rd);
		s2 = cpu.GetGPR(rs) & (typename CONFIG::reg_t)0x0ff;

		if(rs == 0) {
			res = s1;
			} else if(rs < 32) {
			cpu.SetCPSR_C((s1 >> (32 - rs)) & 0x01);
			res = s1 << s2;
			} else if(rs == 32) {
			cpu.SetCPSR_C(s1 & 0x01);
			res = 0;
			} else {
			cpu.SetCPSR_C(false);
			res = 0;
		}
		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
	}
#line 9573 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9579 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9582 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9587 "isa_thumb.tcc"
> *DecodeOpLsl_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLsl_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 9593 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of lsl(2) (logical shift left register) instruction
*******************************************************************/
/*******************************************************************
* lsr(1) (logical shift right immediate) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9606 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9609 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9613 "isa_thumb.tcc"
OpLsr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9617 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9621 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9624 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 9628 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 9631 "isa_thumb.tcc"
)
{
#line 692 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "lsr r" << rd << ", r" << rm << ", #" << dec << imm;
	}
#line 9638 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9643 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9646 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9650 "isa_thumb.tcc"
OpLsr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9654 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9658 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9661 "isa_thumb.tcc"
)
{
#line 696 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res, s1, s2;
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(rm);
		s2 = imm;

		if(s2 == 0) {
			cpu.SetCPSR_C(s1 & (typename CONFIG::reg_t)0x80000000);
			res = 0;
			} else {
			cpu.SetCPSR_C((s1 >> (imm - 1)) & 0x01);
			res = s1 >> imm;
		}
		cpu.SetCPSR_N(false);
		cpu.SetCPSR_Z(res == 0);
	}
#line 9682 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9688 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9691 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9696 "isa_thumb.tcc"
> *DecodeOpLsr_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLsr_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 9702 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of lsr(1) (logical shift right immediate) instruction
*******************************************************************/
/*******************************************************************
* lsr(2) (logical shift right register) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9715 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9718 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9722 "isa_thumb.tcc"
OpLsr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9726 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9730 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9733 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 9737 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 9740 "isa_thumb.tcc"
)
{
#line 724 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "lsr r" << rd << ", r" << rs;
	}
#line 9747 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9752 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9755 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9759 "isa_thumb.tcc"
OpLsr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9763 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9767 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9770 "isa_thumb.tcc"
)
{
#line 728 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res, s1, s2;
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(rd);
		s2 = cpu.GetGPR(rs) & 0x0ff;

		if(s2 == 0) {
			res = s1;
			} else if(s2 < 32) {
			cpu.SetCPSR_C((s1 >> (s2 - 1)) & 0x01);
			res = s1 >> s2;
			} else if(s2 == 32) {
			cpu.SetCPSR_C((s1 >> 31) & 0x01);
			res = 0;
			} else {
			cpu.SetCPSR_C(false);
			res = 0;
		}
		cpu.SetGPR(rd, res);

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
	}
#line 9798 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9804 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9807 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9812 "isa_thumb.tcc"
> *DecodeOpLsr_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLsr_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 9818 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of lsr(2) (logical shift right register) instruction
*******************************************************************/
/*******************************************************************
* mov(1) (move immediate) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9831 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9834 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9838 "isa_thumb.tcc"
OpMov_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9842 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9846 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9849 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 9853 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 9856 "isa_thumb.tcc"
)
{
#line 763 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "mov r" << rd << ", #" << dec << imm;
	}
#line 9863 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9868 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9871 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9875 "isa_thumb.tcc"
OpMov_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9879 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9883 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9886 "isa_thumb.tcc"
)
{
#line 767 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res;
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		res = imm;

		cpu.SetGPR(rd, res);

		cpu.SetCPSR_N(false);
		cpu.SetCPSR_Z(res == 0);
	}
#line 9901 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9907 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9910 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9915 "isa_thumb.tcc"
> *DecodeOpMov_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMov_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 9921 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of mov(1) (move immediate) instruction
*******************************************************************/
///*******************************************************************
//op mov_2(0b0001110[7]:0b000[3]:rn[3]:rd[3])
// * mov(2) (move a low register to another low register) instruction
// */
//
//
///*
// * end of mov(2) (move register a low register to another low register) instruction
// *******************************************************************/
/*******************************************************************
* mov(3) (move high registers) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9943 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9946 "isa_thumb.tcc"
>

void
OpMov_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9953 "isa_thumb.tcc"
>::initialize_rm_index()
{
#line 799 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::rm_index = (h2 << 3) + rm;
	}
#line 9962 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9967 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9970 "isa_thumb.tcc"
>

void
OpMov_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9977 "isa_thumb.tcc"
>::initialize_rd_index()
{
#line 805 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::rd_index = (h1 << 3) + rd;
	}
#line 9986 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9991 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9994 "isa_thumb.tcc"
>

void
OpMov_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10001 "isa_thumb.tcc"
>::initialize_rm_init()
{
#line 811 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		if((h2 << 3) + rm == CPU<CONFIG>::PC_reg)
		inherited::rm_init = 4;
		else
		inherited::rm_init = 0;
	}
#line 10013 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10018 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10021 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10025 "isa_thumb.tcc"
OpMov_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10029 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10033 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10036 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10040 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10043 "isa_thumb.tcc"
)
{
#line 820 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		buffer << "mov r" << inherited::rd_index << ", r" << inherited::rm_index;
	}
#line 10052 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10057 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10060 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10064 "isa_thumb.tcc"
OpMov_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10068 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10072 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10075 "isa_thumb.tcc"
)
{
#line 826 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;
		typename CONFIG::reg_t res;

		res = inherited::rm_init + cpu.GetGPR(inherited::rm_index);

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		cpu.SetGPR(inherited::rd_index, res);
	}
#line 10089 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10095 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10098 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10103 "isa_thumb.tcc"
> *DecodeOpMov_3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMov_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 10109 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of mov(3) (move high registers) instruction
*******************************************************************/
/*******************************************************************
* mul (multiply) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10122 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10125 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10129 "isa_thumb.tcc"
OpMul<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10133 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10137 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10140 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10144 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10147 "isa_thumb.tcc"
)
{
#line 846 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "mul r" << rd << ", r" << rm;
	}
#line 10154 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10159 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10162 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10166 "isa_thumb.tcc"
OpMul<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10170 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10174 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10177 "isa_thumb.tcc"
)
{
#line 850 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res;
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		res = cpu.GetGPR(rd) * cpu.GetGPR(rm);
		cpu.SetGPR(rd, res);

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
	}
#line 10191 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10197 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10200 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10205 "isa_thumb.tcc"
> *DecodeOpMul(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMul<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 10211 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of mul (multiply) instruction
*******************************************************************/
/*******************************************************************
* mvn (move not register) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10224 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10227 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10231 "isa_thumb.tcc"
OpMvn<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10235 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10239 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10242 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10246 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10249 "isa_thumb.tcc"
)
{
#line 871 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "mvn r" << rd << ", r" << rm;
	}
#line 10256 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10261 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10264 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10268 "isa_thumb.tcc"
OpMvn<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10272 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10276 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10279 "isa_thumb.tcc"
)
{
#line 875 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res;
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		res = ~(cpu.GetGPR(rm));
		cpu.SetGPR(rd, res);

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
	}
#line 10293 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10299 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10302 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10307 "isa_thumb.tcc"
> *DecodeOpMvn(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMvn<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 10313 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of mvn (move not register) instruction
*******************************************************************/
/*******************************************************************
* neg (negate register) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10326 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10329 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10333 "isa_thumb.tcc"
OpNeg<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10337 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10341 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10344 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10348 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10351 "isa_thumb.tcc"
)
{
#line 896 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "neg r" << rd << ", r" << rm;
	}
#line 10358 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10363 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10366 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10370 "isa_thumb.tcc"
OpNeg<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10374 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10378 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10381 "isa_thumb.tcc"
)
{
#line 900 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res, s1, s2;
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = 0;
		s2 = cpu.GetGPR(rm);
		res = s1 - s2;
		cpu.SetGPR(rd, res);

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
		cpu.SetCPSR_C(!cpu.BorrowFrom(res, s1, s2));
		cpu.SetCPSR_V(cpu.SubtractionOverflowFrom(res, s1, s2));
	}
#line 10399 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10405 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10408 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10413 "isa_thumb.tcc"
> *DecodeOpNeg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNeg<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 10419 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of neg (negate register) instruction
*******************************************************************/
/*******************************************************************
* orr (logical or) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10432 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10435 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10439 "isa_thumb.tcc"
OpOrr<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10443 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10447 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10450 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10454 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10457 "isa_thumb.tcc"
)
{
#line 925 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "orr r" << rd << ", r" << rm;
	}
#line 10464 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10469 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10472 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10476 "isa_thumb.tcc"
OpOrr<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10480 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10484 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10487 "isa_thumb.tcc"
)
{
#line 929 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res, s1, s2;
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(rd);
		s2 = cpu.GetGPR(rm);
		res = s1 | s2;
		cpu.SetGPR(rd, res);

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
	}
#line 10503 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10509 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10512 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10517 "isa_thumb.tcc"
> *DecodeOpOrr(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOrr<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 10523 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of orr (logical or) instruction
*******************************************************************/
/*******************************************************************
* ror (rotate right register) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10536 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10539 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10543 "isa_thumb.tcc"
OpRor<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10547 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10551 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10554 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10558 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10561 "isa_thumb.tcc"
)
{
#line 952 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "ror r" << rd << ", r" << rs;
	}
#line 10568 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10573 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10576 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10580 "isa_thumb.tcc"
OpRor<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10584 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10588 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10591 "isa_thumb.tcc"
)
{
#line 956 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res, s1, s2;
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(rd);
		s2 = cpu.GetGPR(rs) & 0x0ff;

		if(s2 == 0) {
			res = s1;
			} else if((s2 & 0x01f) == 0) {
			cpu.SetCPSR_C(s1 & (typename CONFIG::reg_t)0x80000000);
			res = s1;
			} else {
			cpu.SetCPSR_C((s1 >> ((s2 & 0x01f) - 1)) & 0x01);
			res = RotateRight(s1, s2);
		}
		cpu.SetGPR(rd, res);

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
	}
#line 10616 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10622 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10625 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10630 "isa_thumb.tcc"
> *DecodeOpRor(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRor<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 10636 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of ror (rotate right register) instruction
*******************************************************************/
/*******************************************************************
* sbc (subtract with carry register) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10649 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10652 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10656 "isa_thumb.tcc"
OpSbc<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10660 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10664 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10667 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10671 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10674 "isa_thumb.tcc"
)
{
#line 988 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "sbc r" << rd << ", r" << rm;
	}
#line 10681 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10686 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10689 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10693 "isa_thumb.tcc"
OpSbc<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10697 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10701 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10704 "isa_thumb.tcc"
)
{
#line 992 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t s1, s2;
		typename CONFIG::reg_t res;
		typename CONFIG::reg_t in_neg_carry;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(rd);
		s2 = cpu.GetGPR(rm);
		in_neg_carry = (cpu.GetCPSR_C() ? 0 : 1);
		res = s1 - s2 - in_neg_carry;
		cpu.SetGPR(rd, res);

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
		cpu.SetCPSR_C(!cpu.BorrowFrom(res, s1, s2, in_neg_carry));
		cpu.SetCPSR_V(cpu.SubtractionOverflowFrom(res, s1, s2, in_neg_carry));
	}
#line 10726 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10732 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10735 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10740 "isa_thumb.tcc"
> *DecodeOpSbc(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSbc<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 10746 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of sbc (subtract with carry register) instruction
*******************************************************************/
/*******************************************************************
* sub(1) (subtract immediate) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10759 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10762 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10766 "isa_thumb.tcc"
OpSub_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10770 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10774 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10777 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10781 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10784 "isa_thumb.tcc"
)
{
#line 1021 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "sub r" << rd << ", r" << rn << ", #" << dec << imm;
	}
#line 10791 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10796 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10799 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10803 "isa_thumb.tcc"
OpSub_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10807 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10811 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10814 "isa_thumb.tcc"
)
{
#line 1025 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t s1, s2;
		typename CONFIG::reg_t res;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(rn);
		s2 = imm;
		res = s1 - s2;
		cpu.SetGPR(rd, res);

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
		cpu.SetCPSR_C(!cpu.BorrowFrom(res, s1, s2));
		cpu.SetCPSR_V(cpu.SubtractionOverflowFrom(res, s1, s2));
	}
#line 10834 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10840 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10843 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10848 "isa_thumb.tcc"
> *DecodeOpSub_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSub_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 10854 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of sub(1) (subtract immediate) instruction
*******************************************************************/
/*******************************************************************
* sub(2) (subtract large immediate) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10867 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10870 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10874 "isa_thumb.tcc"
OpSub_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10878 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10882 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10885 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10889 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10892 "isa_thumb.tcc"
)
{
#line 1052 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "sub r" << rd << ", #" << dec << imm;
	}
#line 10899 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10904 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10907 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10911 "isa_thumb.tcc"
OpSub_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10915 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10919 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10922 "isa_thumb.tcc"
)
{
#line 1056 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		typename CONFIG::reg_t s1, s2;
		typename CONFIG::reg_t res;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(rd);
		s2 = imm;
		res = s1 - s2;
		cpu.SetGPR(rd, res);

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
		cpu.SetCPSR_C(!cpu.BorrowFrom(res, s1, s2));
		cpu.SetCPSR_V(cpu.SubtractionOverflowFrom(res, s1, s2));
	}
#line 10944 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10950 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10953 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10958 "isa_thumb.tcc"
> *DecodeOpSub_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSub_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 10964 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of sub(2) (subtract large immediate) instruction
*******************************************************************/
/*******************************************************************
* sub(3) (subtract register) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10977 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10980 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10984 "isa_thumb.tcc"
OpSub_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10988 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10992 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10995 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10999 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11002 "isa_thumb.tcc"
)
{
#line 1085 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "sub r" << rd << ", r" << rn << ", r" << rm;
	}
#line 11009 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11014 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11017 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11021 "isa_thumb.tcc"
OpSub_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11025 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11029 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11032 "isa_thumb.tcc"
)
{
#line 1089 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t s1, s2;
		typename CONFIG::reg_t res;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(rn);
		s2 = cpu.GetGPR(rm);
		res = s1 - s2;
		cpu.SetGPR(rd, res);

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
		cpu.SetCPSR_C(!cpu.BorrowFrom(res, s1, s2));
		cpu.SetCPSR_V(cpu.SubtractionOverflowFrom(res, s1, s2));
	}
#line 11052 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11058 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11061 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11066 "isa_thumb.tcc"
> *DecodeOpSub_3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSub_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 11072 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of sub(3) (subtract register) instruction
*******************************************************************/
/*******************************************************************
* sub(4) (decrement stack pointer) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11085 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11088 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11092 "isa_thumb.tcc"
OpSub_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11096 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11100 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11103 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11107 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11110 "isa_thumb.tcc"
)
{
#line 1116 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "sub sp, #" << (imm * 4);
	}
#line 11117 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11122 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11125 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11129 "isa_thumb.tcc"
OpSub_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11133 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11137 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11140 "isa_thumb.tcc"
)
{
#line 1120 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t s1, s2;
		typename CONFIG::reg_t res;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(cpu.SP_reg);
		s2 = imm * 4;
		res = s1 - s2;
		cpu.SetGPR(cpu.SP_reg, res);
	}
#line 11155 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11161 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11164 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11169 "isa_thumb.tcc"
> *DecodeOpSub_4(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSub_4<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 11175 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of sub(4) (decrement stack pointer) instruction
*******************************************************************/
/*******************************************************************
* tst (test register) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11188 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11191 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11195 "isa_thumb.tcc"
OpTst<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11199 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11203 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11206 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11210 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11213 "isa_thumb.tcc"
)
{
#line 1142 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "tst r" << rn << ", r" << rm;
	}
#line 11220 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11225 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11228 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11232 "isa_thumb.tcc"
OpTst<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11236 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11240 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11243 "isa_thumb.tcc"
)
{
#line 1146 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res, s1, s2;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(rn);
		s2 = cpu.GetGPR(rm);
		res = s1 & s2;

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
	}
#line 11259 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11265 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11268 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11273 "isa_thumb.tcc"
> *DecodeOpTst(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTst<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 11279 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of tst (test register) instruction
*******************************************************************/
/*
*  Copyright (c) 2007,
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
* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
*/
/**********************************************

THUMB LOAD STORE REGISTER INSTRUCTIONS

**********************************************/
/*******************************************************************
* ldr(1) (load word immediate offset) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11330 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11333 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11337 "isa_thumb.tcc"
OpLdr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11341 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11345 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11348 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11352 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11355 "isa_thumb.tcc"
)
{
#line 47 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldr r" << rd << ", [r" << rn << ", #" << dec << (imm * 4) << "]";
	}
#line 11362 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11367 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11370 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11374 "isa_thumb.tcc"
OpLdr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11378 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11382 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11385 "isa_thumb.tcc"
)
{
#line 51 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		typename CONFIG::address_t addr;
		typename CONFIG::reg_t rn_value, value;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		rn_value = cpu.GetGPR(rn);
		addr = rn_value + (imm * 4);
		value = 0;
		cpu.Read32toGPRAligned(addr, rd);
	}
#line 11400 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11406 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11409 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11414 "isa_thumb.tcc"
> *DecodeOpLdr_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdr_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 11420 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of ldr(1) (load word immediate offset) instruction
*******************************************************************/
/*******************************************************************
* ldr(2) (load word register offset) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11433 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11436 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11440 "isa_thumb.tcc"
OpLdr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11444 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11448 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11451 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11455 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11458 "isa_thumb.tcc"
)
{
#line 73 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldr r" << rd << ", [r" << rn << ", r" << rm << "]";
	}
#line 11465 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11470 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11473 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11477 "isa_thumb.tcc"
OpLdr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11481 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11485 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11488 "isa_thumb.tcc"
)
{
#line 77 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		typename CONFIG::address_t addr;
		typename CONFIG::reg_t rn_value, rm_value, value;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		rn_value = cpu.GetGPR(rn);
		rm_value = cpu.GetGPR(rm);
		addr = rn_value + rm_value;
		value = 0;
		cpu.Read32toGPRAligned(addr, rd);
	}
#line 11504 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11510 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11513 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11518 "isa_thumb.tcc"
> *DecodeOpLdr_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdr_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 11524 "isa_thumb.tcc"
	>(code, addr);
}

/*
* ldr(2) (load word register offset) instruction
*******************************************************************/
/*******************************************************************
* ldr(3) (load word pc-relative) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11537 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11540 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11544 "isa_thumb.tcc"
OpLdr_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11548 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11552 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11555 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11559 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11562 "isa_thumb.tcc"
)
{
#line 100 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldr r" << rd << ", [pc, #" << dec << (imm * 4) << "]";
	}
#line 11569 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11574 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11577 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11581 "isa_thumb.tcc"
OpLdr_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11585 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11589 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11592 "isa_thumb.tcc"
)
{
#line 104 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		typename CONFIG::address_t addr;
		typename CONFIG::reg_t pc_value, value;

		pc_value = (cpu.GetGPR(cpu.PC_reg) + 4) & ~((typename CONFIG::reg_t)0x03);

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		addr = pc_value + (imm * 4);
		value = 0;
		cpu.Read32toGPRAligned(addr, rd);
	}
#line 11608 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11614 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11617 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11622 "isa_thumb.tcc"
> *DecodeOpLdr_3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdr_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 11628 "isa_thumb.tcc"
	>(code, addr);
}

/*
* ldr(3) (load word pc-relative) instruction
*******************************************************************/
/*******************************************************************
* ldr(4) (load word sp-relative) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11641 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11644 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11648 "isa_thumb.tcc"
OpLdr_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11652 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11656 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11659 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11663 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11666 "isa_thumb.tcc"
)
{
#line 127 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldr r" << rd << ", [sp, #" << dec << (imm * 4) << "]";
	}
#line 11673 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11678 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11681 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11685 "isa_thumb.tcc"
OpLdr_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11689 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11693 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11696 "isa_thumb.tcc"
)
{
#line 131 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		typename CONFIG::address_t addr;
		typename CONFIG::reg_t sp_value, value;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		sp_value = cpu.GetGPR(cpu.SP_reg);
		addr = sp_value + (imm * 4);
		value = 0;
		cpu.Read32toGPRAligned(addr, rd);
	}
#line 11711 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11717 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11720 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11725 "isa_thumb.tcc"
> *DecodeOpLdr_4(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdr_4<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 11731 "isa_thumb.tcc"
	>(code, addr);
}

/*
* ldr(4) (load word sp-relative) instruction
*******************************************************************/
/*******************************************************************
* ldrb(1) (load unsigned byte immediate offset) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11744 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11747 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11751 "isa_thumb.tcc"
OpLdrb_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11755 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11759 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11762 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11766 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11769 "isa_thumb.tcc"
)
{
#line 153 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldrb r" << rd << ", [r" << rn << ", #" << dec << imm << "]";
	}
#line 11776 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11781 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11784 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11788 "isa_thumb.tcc"
OpLdrb_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11792 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11796 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11799 "isa_thumb.tcc"
)
{
#line 157 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		typename CONFIG::address_t addr;
		typename CONFIG::reg_t rn_value;
		uint8_t val8;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		rn_value = cpu.GetGPR(rn);
		addr = rn_value + imm;
		val8 = 0;
		cpu.Read8toGPR(addr, rd);
	}
#line 11815 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11821 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11824 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11829 "isa_thumb.tcc"
> *DecodeOpLdrb_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdrb_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 11835 "isa_thumb.tcc"
	>(code, addr);
}

/*
* ldrb(1) (load unsigned byte immediate offset) instruction
*******************************************************************/
/*******************************************************************
* ldrb(2) (load unsigned byte register offset) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11848 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11851 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11855 "isa_thumb.tcc"
OpLdrb_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11859 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11863 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11866 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11870 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11873 "isa_thumb.tcc"
)
{
#line 180 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldrb r" << rd << ", [r" << rn << ", r" << rm << "]";
	}
#line 11880 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11885 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11888 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11892 "isa_thumb.tcc"
OpLdrb_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11896 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11900 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11903 "isa_thumb.tcc"
)
{
#line 184 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		typename CONFIG::address_t addr;
		typename CONFIG::reg_t rn_value, rm_value;
		uint8_t val8;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		rn_value = cpu.GetGPR(rn);
		rm_value = cpu.GetGPR(rm);
		addr = rn_value + rm_value;
		val8 = 0;
		cpu.Read8toGPR(addr, rd);
	}
#line 11920 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11926 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11929 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11934 "isa_thumb.tcc"
> *DecodeOpLdrb_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdrb_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 11940 "isa_thumb.tcc"
	>(code, addr);
}

/*
* ldrb(2) (load unsigned byte register offset) instruction
*******************************************************************/
/*******************************************************************
* ldrh(1) (load unsigned halfword immediate offset) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11953 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11956 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11960 "isa_thumb.tcc"
OpLdrh_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11964 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11968 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11971 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11975 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11978 "isa_thumb.tcc"
)
{
#line 208 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldrh r" << rd << ", [r" << rn << ", #" << dec << (imm * 2) << "]";
	}
#line 11985 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11990 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11993 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11997 "isa_thumb.tcc"
OpLdrh_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12001 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12005 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12008 "isa_thumb.tcc"
)
{
#line 212 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		typename CONFIG::address_t addr;
		typename CONFIG::reg_t rn_value;
		uint16_t val16;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		rn_value = cpu.GetGPR(rn);
		addr = rn_value + (imm * 2);
		val16 = 0;
		cpu.Read16toGPRAligned(addr, rd);
	}
#line 12024 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12030 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12033 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12038 "isa_thumb.tcc"
> *DecodeOpLdrh_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdrh_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 12044 "isa_thumb.tcc"
	>(code, addr);
}

/*
* ldrh(1) (load unsigned halfword immediate offset) instruction
*******************************************************************/
/*******************************************************************
* ldrh(2) (load unsigned halfword register offset) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12057 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12060 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12064 "isa_thumb.tcc"
OpLdrh_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12068 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12072 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12075 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12079 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12082 "isa_thumb.tcc"
)
{
#line 235 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldrh r" << rd << ", [r" << rn << ", r" << rm << "]";
	}
#line 12089 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12094 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12097 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12101 "isa_thumb.tcc"
OpLdrh_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12105 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12109 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12112 "isa_thumb.tcc"
)
{
#line 239 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		typename CONFIG::address_t addr;
		typename CONFIG::reg_t rn_value, rm_value;
		uint16_t val16;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		rn_value = cpu.GetGPR(rn);
		rm_value = cpu.GetGPR(rm);
		addr = rn_value + rm_value;
		val16 = 0;
		cpu.Read16toGPRAligned(addr, rd);
	}
#line 12129 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12135 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12138 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12143 "isa_thumb.tcc"
> *DecodeOpLdrh_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdrh_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 12149 "isa_thumb.tcc"
	>(code, addr);
}

/*
* ldrh(2) (load unsigned halfword register offset) instruction
*******************************************************************/
/*******************************************************************
* ldrsb (load signed byte register offset) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12162 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12165 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12169 "isa_thumb.tcc"
OpLdrsb<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12173 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12177 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12180 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12184 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12187 "isa_thumb.tcc"
)
{
#line 263 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldrsb r" << rd << ", [r" << rn << ", r" << rm << "]";
	}
#line 12194 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12199 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12202 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12206 "isa_thumb.tcc"
OpLdrsb<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12210 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12214 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12217 "isa_thumb.tcc"
)
{
#line 267 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		typename CONFIG::address_t addr;
		typename CONFIG::reg_t rn_value, rm_value;
		int8_t val8;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		rn_value = cpu.GetGPR(rn);
		rm_value = cpu.GetGPR(rm);
		addr = rn_value + rm_value;
		val8 = 0;
		cpu.ReadS8toGPR(addr, rd);
	}
#line 12234 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12240 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12243 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12248 "isa_thumb.tcc"
> *DecodeOpLdrsb(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdrsb<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 12254 "isa_thumb.tcc"
	>(code, addr);
}

/*
* ldrsb (load signed byte register offset) instruction
*******************************************************************/
/*******************************************************************
* ldrsh (load signed halfword register offset) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12267 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12270 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12274 "isa_thumb.tcc"
OpLdrsh<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12278 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12282 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12285 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12289 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12292 "isa_thumb.tcc"
)
{
#line 291 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldrsh r" << rd << ", [r" << rn << ", r" << rm << "]";
	}
#line 12299 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12304 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12307 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12311 "isa_thumb.tcc"
OpLdrsh<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12315 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12319 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12322 "isa_thumb.tcc"
)
{
#line 295 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		typename CONFIG::address_t addr;
		typename CONFIG::reg_t rn_value, rm_value;
		int16_t val16;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		rn_value = cpu.GetGPR(rn);
		rm_value = cpu.GetGPR(rm);
		addr = rn_value + rm_value;
		val16 = 0;
		cpu.ReadS16toGPRAligned(addr, rd);
	}
#line 12339 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12345 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12348 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12353 "isa_thumb.tcc"
> *DecodeOpLdrsh(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdrsh<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 12359 "isa_thumb.tcc"
	>(code, addr);
}

/*
* ldrsh (load signed halfword register offset) instruction
*******************************************************************/
/*******************************************************************
* str(1) (store word immediate offset) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12372 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12375 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12379 "isa_thumb.tcc"
OpStr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12383 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12387 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12390 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12394 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12397 "isa_thumb.tcc"
)
{
#line 319 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "str r" << rd << ", [r" << rn << ", #" << (imm * 4) << "]";
	}
#line 12404 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12409 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12412 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12416 "isa_thumb.tcc"
OpStr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12420 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12424 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12427 "isa_thumb.tcc"
)
{
#line 323 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		typename CONFIG::address_t addr;
		typename CONFIG::reg_t rd_value, rn_value;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		rn_value = cpu.GetGPR(rn);
		addr = rn_value + (imm * 4);
		rd_value = cpu.GetGPR(rd);
		cpu.Write32(addr, rd_value);
	}
#line 12442 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12448 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12451 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12456 "isa_thumb.tcc"
> *DecodeOpStr_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpStr_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 12462 "isa_thumb.tcc"
	>(code, addr);
}

/*
* str(1) (store word immediate offset) instruction
*******************************************************************/
/*******************************************************************
* str(2) (store word register offset) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12475 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12478 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12482 "isa_thumb.tcc"
OpStr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12486 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12490 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12493 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12497 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12500 "isa_thumb.tcc"
)
{
#line 345 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "str r" << rd << ", [r" << rn << ", r" << rm << "]";
	}
#line 12507 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12512 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12515 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12519 "isa_thumb.tcc"
OpStr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12523 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12527 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12530 "isa_thumb.tcc"
)
{
#line 349 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		typename CONFIG::address_t addr;
		typename CONFIG::reg_t rd_value, rn_value, rm_value;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		rn_value = cpu.GetGPR(rn);
		rm_value = cpu.GetGPR(rm);
		addr = rn_value + rm_value;
		rd_value = cpu.GetGPR(rd);
		cpu.Write32(addr, rd_value);
	}
#line 12546 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12552 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12555 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12560 "isa_thumb.tcc"
> *DecodeOpStr_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpStr_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 12566 "isa_thumb.tcc"
	>(code, addr);
}

/*
* str(2) (store word register offset) instruction
*******************************************************************/
/*******************************************************************
* str(3) (store word sp-relative) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12579 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12582 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12586 "isa_thumb.tcc"
OpStr_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12590 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12594 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12597 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12601 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12604 "isa_thumb.tcc"
)
{
#line 372 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "str r" << rd << ", [sp, #" << (imm * 4) << "]";
	}
#line 12611 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12616 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12619 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12623 "isa_thumb.tcc"
OpStr_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12627 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12631 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12634 "isa_thumb.tcc"
)
{
#line 376 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		typename CONFIG::address_t addr;
		typename CONFIG::reg_t rd_value, sp_value;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		sp_value = cpu.GetGPR(cpu.SP_reg);
		addr = sp_value + (imm * 4);
		rd_value = cpu.GetGPR(rd);
		cpu.Write32(addr, rd_value);
	}
#line 12649 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12655 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12658 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12663 "isa_thumb.tcc"
> *DecodeOpStr_3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpStr_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 12669 "isa_thumb.tcc"
	>(code, addr);
}

/*
* str(3) (store word sp-relative) instruction
*******************************************************************/
/*******************************************************************
* strb(1) (store byte immediate offset) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12682 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12685 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12689 "isa_thumb.tcc"
OpStrb_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12693 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12697 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12700 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12704 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12707 "isa_thumb.tcc"
)
{
#line 398 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "strb r" << rd << ", [r" << rn << ", #" << dec << imm << "]";
	}
#line 12714 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12719 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12722 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12726 "isa_thumb.tcc"
OpStrb_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12730 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12734 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12737 "isa_thumb.tcc"
)
{
#line 402 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		typename CONFIG::address_t addr;
		typename CONFIG::reg_t rd_value, rn_value;
		uint8_t data;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		rn_value = cpu.GetGPR(rn);
		addr = rn_value + imm;
		rd_value = cpu.GetGPR(rd);
		data = rd_value & (typename CONFIG::reg_t)0x0ff;
		cpu.Write8(addr, data);
	}
#line 12754 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12760 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12763 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12768 "isa_thumb.tcc"
> *DecodeOpStrb_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpStrb_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 12774 "isa_thumb.tcc"
	>(code, addr);
}

/*
* strb(1) (store byte immediate offset) instruction
*******************************************************************/
/*******************************************************************
* strb(2) (store byte register offset) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12787 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12790 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12794 "isa_thumb.tcc"
OpStrb_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12798 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12802 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12805 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12809 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12812 "isa_thumb.tcc"
)
{
#line 426 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "strb r" << rd << ", [r" << rn << ", r" << rm << "]";
	}
#line 12819 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12824 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12827 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12831 "isa_thumb.tcc"
OpStrb_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12835 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12839 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12842 "isa_thumb.tcc"
)
{
#line 430 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		typename CONFIG::address_t addr;
		typename CONFIG::reg_t rd_value, rn_value, rm_value;
		uint8_t data;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		rn_value = cpu.GetGPR(rn);
		rm_value = cpu.GetGPR(rm);
		addr = rn_value + rm_value;
		rd_value = cpu.GetGPR(rd);
		data = rd_value & (typename CONFIG::reg_t)0x0ff;
		cpu.Write8(addr, data);
	}
#line 12860 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12866 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12869 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12874 "isa_thumb.tcc"
> *DecodeOpStrb_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpStrb_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 12880 "isa_thumb.tcc"
	>(code, addr);
}

/*
* strb(2) (store byte register offset) instruction
*******************************************************************/
/*******************************************************************
* strh(1) (store halfword immediate offset) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12893 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12896 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12900 "isa_thumb.tcc"
OpStrh_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12904 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12908 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12911 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12915 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12918 "isa_thumb.tcc"
)
{
#line 455 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "strh r" << rd << ", [r" << rn << ", #" << dec << (imm * 2) << "]";
	}
#line 12925 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12930 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12933 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12937 "isa_thumb.tcc"
OpStrh_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12941 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12945 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12948 "isa_thumb.tcc"
)
{
#line 459 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		typename CONFIG::address_t addr;
		typename CONFIG::reg_t rd_value, rn_value;
		uint16_t data;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		rn_value = cpu.GetGPR(rn);
		addr = rn_value + (imm * 2);
		rd_value = cpu.GetGPR(rd);
		data = rd_value & (typename CONFIG::reg_t)0x0ffff;
		cpu.Write16(addr, data);
	}
#line 12965 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12971 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12974 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12979 "isa_thumb.tcc"
> *DecodeOpStrh_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpStrh_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 12985 "isa_thumb.tcc"
	>(code, addr);
}

/*
* strh(1) (store halfword immediate offset) instruction
*******************************************************************/
/*******************************************************************
* strh(2) (store halfword register offset) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12998 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13001 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13005 "isa_thumb.tcc"
OpStrh_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13009 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13013 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13016 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 13020 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 13023 "isa_thumb.tcc"
)
{
#line 483 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "strh r" << rd << ", [r" << rn << ", r" << rm << "]";
	}
#line 13030 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13035 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13038 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13042 "isa_thumb.tcc"
OpStrh_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13046 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13050 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13053 "isa_thumb.tcc"
)
{
#line 487 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		typename CONFIG::address_t addr;
		typename CONFIG::reg_t rd_value, rn_value, rm_value;
		uint16_t data;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		rn_value = cpu.GetGPR(rn);
		rm_value = cpu.GetGPR(rm);
		addr = rn_value + rm_value;
		rd_value = cpu.GetGPR(rd);
		data = rd_value & (typename CONFIG::reg_t)0x0ff;
		cpu.Write16(addr, data);
	}
#line 13071 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13077 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13080 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13085 "isa_thumb.tcc"
> *DecodeOpStrh_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpStrh_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 13091 "isa_thumb.tcc"
	>(code, addr);
}

/*
* strh(2) (store halfword register offset) instruction
*******************************************************************/
/*
*  Copyright (c) 2007,
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
* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
*/
/**********************************************

THUMB LOAD STORE MULTIPLE INSTRUCTIONS

**********************************************/
/*******************************************************************
* ldmia (load multiple) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13142 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13145 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13149 "isa_thumb.tcc"
OpLdmia<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13153 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13157 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13160 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 13164 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 13167 "isa_thumb.tcc"
)
{
#line 47 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_multiple.isa"
	{
		unsigned int mask = 1;
		bool prev = false;

		buffer << "ldmia r" << rn << ", {";
			for(unsigned int i = 0; i < 8; i++) {
				if((reg_list & mask) == mask) {
					if(prev) buffer << ", ";
					else prev = true;
					buffer << "r" << i;
				}
				mask = mask << 1;
			}
			buffer << "}";
	}
#line 13186 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13191 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13194 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13198 "isa_thumb.tcc"
OpLdmia<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13202 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13206 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13209 "isa_thumb.tcc"
)
{
#line 63 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_multiple.isa"
	{
		typename CONFIG::reg_t rn_value, rn_new_val;
		typename CONFIG::address_t start_address, end_address;
		typename CONFIG::address_t cur_address;
		uint32_t num_regs;
		unsigned int mask = 1;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		rn_value = cpu.GetGPR(rn);

		num_regs = cpu.LSMia(rn_value, reg_list,
		&start_address, &end_address,
		&rn_new_val);

		cpu.SetGPR(rn, rn_new_val);

		cur_address = start_address;
		for(unsigned int i = 0; i < 8; i++) {
			if((reg_list & mask) == mask) {
				cpu.Read32toGPRAligned(cur_address, i);
				cur_address += 4;
			}
			mask = mask << 1;
		}
	}
#line 13239 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13245 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13248 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13253 "isa_thumb.tcc"
> *DecodeOpLdmia(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdmia<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 13259 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of ldmia (load multiple) instruction
*******************************************************************/
/*******************************************************************
* pop (pop multiple) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13272 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13275 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13279 "isa_thumb.tcc"
OpPop<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13283 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13287 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13290 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 13294 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 13297 "isa_thumb.tcc"
)
{
#line 100 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_multiple.isa"
	{
		unsigned int mask = 1;
		bool prev = false;

		buffer << "pop {";
			for(unsigned int i = 0; i < 8; i++) {
				if((reg_list & mask) == mask) {
					if(prev) buffer << ", ";
					else prev = true;
					buffer << "r" << i;
				}
				mask = mask << 1;
			}
			buffer << "}";
	}
#line 13316 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13321 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13324 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13328 "isa_thumb.tcc"
OpPop<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13332 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13336 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13339 "isa_thumb.tcc"
)
{
#line 116 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_multiple.isa"
	{
		typename CONFIG::reg_t sp_value, sp_new_val;
		typename CONFIG::address_t start_address, end_address;
		typename CONFIG::address_t cur_address;
		uint32_t num_regs;
		unsigned int mask = 1;

		if(r == 0)
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		sp_value = cpu.GetGPR(cpu.SP_reg);

		num_regs = cpu.LSMia(sp_value, reg_list,
		&start_address, &end_address,
		&sp_new_val);

		cur_address = start_address;
		for(unsigned int i = 0; i < 8; i++) {
			if((reg_list & mask) == mask) {
				cpu.Read32toGPRAligned(cur_address, i);
				cur_address += 4;
			}
			mask = mask << 1;
		}

		if(r) {
			cpu.Read32toPCUpdateT(cur_address);
			cur_address += 4;
		}

		cpu.SetGPR(cpu.SP_reg, cur_address);
	}
#line 13375 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13381 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13384 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13389 "isa_thumb.tcc"
> *DecodeOpPop(CodeType code, typename CONFIG::address_t addr)
{
	return new OpPop<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 13395 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of pop (pop multiple) instruction
*******************************************************************/
/*******************************************************************
* push (push multiple) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13408 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13411 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13415 "isa_thumb.tcc"
OpPush<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13419 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13423 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13426 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 13430 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 13433 "isa_thumb.tcc"
)
{
#line 159 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_multiple.isa"
	{
		unsigned int mask = 1;
		bool prev = false;

		buffer << "push {";
			for(unsigned int i = 0; i < 8; i++) {
				if((reg_list & mask) == mask) {
					if(prev) buffer << ", ";
					else prev = true;
					buffer << "r" << i;
				}
				mask = mask << 1;
			}
			if(r) {
				if(prev) buffer << ", lr";
				else buffer << "lr";
			}
			buffer << "}";
	}
#line 13456 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13461 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13464 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13468 "isa_thumb.tcc"
OpPush<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13472 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13476 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13479 "isa_thumb.tcc"
)
{
#line 179 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_multiple.isa"
	{
		typename CONFIG::reg_t sp_value, sp_new_val, value;
		typename CONFIG::address_t start_address, end_address;
		typename CONFIG::address_t cur_address;
		uint32_t num_regs;
		unsigned int mask = 1;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		sp_value = cpu.GetGPR(cpu.SP_reg);

		num_regs = cpu.LSMdb(sp_value, reg_list,
		&start_address, &end_address,
		&sp_new_val);
		if(r)
		start_address -= 4;

		cur_address = start_address;
		for(unsigned int i = 0; i < 8; i++) {
			if((reg_list & mask) == mask) {
				value = cpu.GetGPR(i);
				cpu.Write32(cur_address, value);
				cur_address += 4;
			}
			mask = mask << 1;
		}

		if(r) {
			value = cpu.GetGPR(cpu.LR_reg);
			cpu.Write32(cur_address, value);
			cur_address += 4;
		}

		cpu.SetGPR(cpu.SP_reg, start_address);
	}
#line 13518 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13524 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13527 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13532 "isa_thumb.tcc"
> *DecodeOpPush(CodeType code, typename CONFIG::address_t addr)
{
	return new OpPush<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 13538 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of push (push multiple) instruction
*******************************************************************/
/*******************************************************************
* stmia (store multiple) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13551 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13554 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13558 "isa_thumb.tcc"
OpStmia<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13562 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13566 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13569 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 13573 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 13576 "isa_thumb.tcc"
)
{
#line 225 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_multiple.isa"
	{
		unsigned int mask = 1;
		bool prev = false;

		buffer << "stmia r" << rn << "!, {";
			for(unsigned int i = 0; i < 8; i++) {
				if((reg_list & mask) == mask) {
					if(prev) buffer << ", ";
					else prev = true;
					buffer << "r" << i;
				}
				mask = mask << 1;
			}
			buffer << "}";
	}
#line 13595 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13600 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13603 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13607 "isa_thumb.tcc"
OpStmia<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13611 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13615 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13618 "isa_thumb.tcc"
)
{
#line 241 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_multiple.isa"
	{
		typename CONFIG::reg_t rn_value, rn_new_val, value;
		typename CONFIG::address_t start_address, end_address;
		typename CONFIG::address_t cur_address;
		uint32_t num_regs;
		unsigned int mask = 1;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		rn_value = cpu.GetGPR(rn);

		num_regs = cpu.LSMia(rn_value, reg_list,
		&start_address, &end_address,
		&rn_new_val);

		cur_address = start_address;
		for(unsigned int i = 0; i < 8; i++) {
			if((reg_list & mask) == mask) {
				value = cpu.GetGPR(i);
				cpu.Write32(cur_address, value);
				cur_address += 4;
			}
			mask = mask << 1;
		}

		cpu.SetGPR(rn, end_address + 4);
	}
#line 13649 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13655 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13658 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13663 "isa_thumb.tcc"
> *DecodeOpStmia(CodeType code, typename CONFIG::address_t addr)
{
	return new OpStmia<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 13669 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of stmia (store multiple) instruction
*******************************************************************/
/*
*  Copyright (c) 2007,
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
* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
*/
/**********************************************

THUMB EXCEPTION INSTRUCTIONS

**********************************************/
/*******************************************************************
* bkpt (breakpoint) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13720 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13723 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13727 "isa_thumb.tcc"
OpBkpt<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13731 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13735 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13738 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 13742 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 13745 "isa_thumb.tcc"
)
{
#line 47 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/exception.isa"
	{
		buffer << "bkpt #" << dec << imm;
	}
#line 13752 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13757 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13760 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13764 "isa_thumb.tcc"
OpBkpt<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13768 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13772 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13775 "isa_thumb.tcc"
)
{
#line 51 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/exception.isa"
	{
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 4);

#ifdef SOCLIB

		throw PrefetchAbortException<CONFIG>();

#else // SOCLIB

		if(cpu.linux_os_import) {
			// we are executing on linux emulation mode
			// what should we do with this kind of call? ignore it
			} else {
			// we are executing on full system mode
			throw PrefetchAbortException<CONFIG>();
		}

#endif // SOCLIB

	}
#line 13799 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13805 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13808 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13813 "isa_thumb.tcc"
> *DecodeOpBkpt(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBkpt<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 13819 "isa_thumb.tcc"
	>(code, addr);
}

/*
* end of bkpt (breakpoint) instruction
*******************************************************************/
/*******************************************************************
* swi (software interrupt) instruction
*/
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13832 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13835 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13839 "isa_thumb.tcc"
OpSwi<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13843 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13847 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13850 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 13854 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 13857 "isa_thumb.tcc"
)
{
#line 82 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/exception.isa"
	{
		buffer << "swi #" << dec << imm;
	}
#line 13864 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13869 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13872 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13876 "isa_thumb.tcc"
OpSwi<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13880 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13884 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13887 "isa_thumb.tcc"
)
{
#line 86 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/exception.isa"
	{
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

#ifdef SOCLIB

		throw SoftwareInterruptException<CONFIG>();

#else // SOCLIB

		if(cpu.linux_os_import) {
			// we are executing on linux emulation mode
			// use linux_os_import
			try {
				cpu.linux_os_import->ExecuteSystemCall(imm);
				} catch(exception &e) {
				cerr << e.what() << endl;
			}
			} else {
			// we are executing on full system mode
			throw SoftwareInterruptException<CONFIG>();
		}

#endif // SOCLIB

	}
#line 13916 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13922 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13925 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13930 "isa_thumb.tcc"
> *DecodeOpSwi(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSwi<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 13936 "isa_thumb.tcc"
	>(code, addr);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13943 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13946 "isa_thumb.tcc"
>
OpB_1_0xxx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13951 "isa_thumb.tcc"
>::OpB_1_0xxx(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13955 "isa_thumb.tcc"
>(code, addr, "b_1_0xxx")
{
	sub_cond = ((code >> 8) & 0x7);
	imm = (((((int32_t)(code >> 0)) & 0xff) << 24) >> 24);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13965 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13968 "isa_thumb.tcc"
>
OpB_1_10xx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13973 "isa_thumb.tcc"
>::OpB_1_10xx(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13977 "isa_thumb.tcc"
>(code, addr, "b_1_10xx")
{
	sub_cond = ((code >> 8) & 0x3);
	imm = (((((int32_t)(code >> 0)) & 0xff) << 24) >> 24);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13987 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13990 "isa_thumb.tcc"
>
OpB_1_110x<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13995 "isa_thumb.tcc"
>::OpB_1_110x(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13999 "isa_thumb.tcc"
>(code, addr, "b_1_110x")
{
	sub_cond = ((code >> 8) & 0x1);
	imm = (((((int32_t)(code >> 0)) & 0xff) << 24) >> 24);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14009 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14012 "isa_thumb.tcc"
>
OpB_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14017 "isa_thumb.tcc"
>::OpB_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14021 "isa_thumb.tcc"
>(code, addr, "b_2")
{
	imm = (((((int32_t)(code >> 0)) & 0x7ff) << 21) >> 21);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14030 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14033 "isa_thumb.tcc"
>
OpBl<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14038 "isa_thumb.tcc"
>::OpBl(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14042 "isa_thumb.tcc"
>(code, addr, "bl")
{
	h2 = ((code >> 11) & 0x1);
	offset = ((code >> 0) & 0x7ff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14052 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14055 "isa_thumb.tcc"
>
OpBx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14060 "isa_thumb.tcc"
>::OpBx(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14064 "isa_thumb.tcc"
>(code, addr, "bx")
{
	h2 = ((code >> 6) & 0x1);
	rm = ((code >> 3) & 0x7);
	sbz = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14075 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14078 "isa_thumb.tcc"
>
OpBlx_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14083 "isa_thumb.tcc"
>::OpBlx_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14087 "isa_thumb.tcc"
>(code, addr, "blx_1")
{
	offset = ((code >> 0) & 0x7ff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14096 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14099 "isa_thumb.tcc"
>
OpBlx_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14104 "isa_thumb.tcc"
>::OpBlx_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14108 "isa_thumb.tcc"
>(code, addr, "blx_2")
{
	h2 = ((code >> 6) & 0x1);
	rm = ((code >> 3) & 0x7);
	sbz = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14119 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14122 "isa_thumb.tcc"
>
OpAdc<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14127 "isa_thumb.tcc"
>::OpAdc(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14131 "isa_thumb.tcc"
>(code, addr, "adc")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14141 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14144 "isa_thumb.tcc"
>
OpAdd_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14149 "isa_thumb.tcc"
>::OpAdd_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14153 "isa_thumb.tcc"
>(code, addr, "add_1")
{
	imm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14164 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14167 "isa_thumb.tcc"
>
OpAdd_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14172 "isa_thumb.tcc"
>::OpAdd_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14176 "isa_thumb.tcc"
>(code, addr, "add_2")
{
	rd = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14186 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14189 "isa_thumb.tcc"
>
OpAdd_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14194 "isa_thumb.tcc"
>::OpAdd_3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14198 "isa_thumb.tcc"
>(code, addr, "add_3")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14209 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14212 "isa_thumb.tcc"
>
OpAdd_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14217 "isa_thumb.tcc"
>::OpAdd_4(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14221 "isa_thumb.tcc"
>(code, addr, "add_4")
{
	h1 = ((code >> 7) & 0x1);
	h2 = ((code >> 6) & 0x1);
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14233 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14236 "isa_thumb.tcc"
>
OpAdd_5<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14241 "isa_thumb.tcc"
>::OpAdd_5(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14245 "isa_thumb.tcc"
>(code, addr, "add_5")
{
	rd = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14255 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14258 "isa_thumb.tcc"
>
OpAdd_6<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14263 "isa_thumb.tcc"
>::OpAdd_6(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14267 "isa_thumb.tcc"
>(code, addr, "add_6")
{
	rd = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14277 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14280 "isa_thumb.tcc"
>
OpAdd_7<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14285 "isa_thumb.tcc"
>::OpAdd_7(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14289 "isa_thumb.tcc"
>(code, addr, "add_7")
{
	imm = ((code >> 0) & 0x7f);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14298 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14301 "isa_thumb.tcc"
>
OpAnd<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14306 "isa_thumb.tcc"
>::OpAnd(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14310 "isa_thumb.tcc"
>(code, addr, "and")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14320 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14323 "isa_thumb.tcc"
>
OpAsr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14328 "isa_thumb.tcc"
>::OpAsr_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14332 "isa_thumb.tcc"
>(code, addr, "asr_1")
{
	imm = ((code >> 6) & 0x1f);
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14343 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14346 "isa_thumb.tcc"
>
OpAsr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14351 "isa_thumb.tcc"
>::OpAsr_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14355 "isa_thumb.tcc"
>(code, addr, "asr_2")
{
	rs = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14365 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14368 "isa_thumb.tcc"
>
OpBic<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14373 "isa_thumb.tcc"
>::OpBic(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14377 "isa_thumb.tcc"
>(code, addr, "bic")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14387 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14390 "isa_thumb.tcc"
>
OpCmn<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14395 "isa_thumb.tcc"
>::OpCmn(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14399 "isa_thumb.tcc"
>(code, addr, "cmn")
{
	rm = ((code >> 3) & 0x7);
	rn = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14409 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14412 "isa_thumb.tcc"
>
OpCmp_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14417 "isa_thumb.tcc"
>::OpCmp_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14421 "isa_thumb.tcc"
>(code, addr, "cmp_1")
{
	rn = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14431 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14434 "isa_thumb.tcc"
>
OpCmp_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14439 "isa_thumb.tcc"
>::OpCmp_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14443 "isa_thumb.tcc"
>(code, addr, "cmp_2")
{
	rm = ((code >> 3) & 0x7);
	rn = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14453 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14456 "isa_thumb.tcc"
>
OpCmp_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14461 "isa_thumb.tcc"
>::OpCmp_3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14465 "isa_thumb.tcc"
>(code, addr, "cmp_3")
{
	h1 = ((code >> 7) & 0x1);
	h2 = ((code >> 6) & 0x1);
	rm = ((code >> 3) & 0x7);
	rn = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14477 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14480 "isa_thumb.tcc"
>
OpEor<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14485 "isa_thumb.tcc"
>::OpEor(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14489 "isa_thumb.tcc"
>(code, addr, "eor")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14499 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14502 "isa_thumb.tcc"
>
OpLsl_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14507 "isa_thumb.tcc"
>::OpLsl_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14511 "isa_thumb.tcc"
>(code, addr, "lsl_1")
{
	imm = ((code >> 6) & 0x1f);
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14522 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14525 "isa_thumb.tcc"
>
OpLsl_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14530 "isa_thumb.tcc"
>::OpLsl_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14534 "isa_thumb.tcc"
>(code, addr, "lsl_2")
{
	rs = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14544 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14547 "isa_thumb.tcc"
>
OpLsr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14552 "isa_thumb.tcc"
>::OpLsr_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14556 "isa_thumb.tcc"
>(code, addr, "lsr_1")
{
	imm = ((code >> 6) & 0x1f);
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14567 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14570 "isa_thumb.tcc"
>
OpLsr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14575 "isa_thumb.tcc"
>::OpLsr_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14579 "isa_thumb.tcc"
>(code, addr, "lsr_2")
{
	rs = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14589 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14592 "isa_thumb.tcc"
>
OpMov_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14597 "isa_thumb.tcc"
>::OpMov_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14601 "isa_thumb.tcc"
>(code, addr, "mov_1")
{
	rd = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14611 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14614 "isa_thumb.tcc"
>
OpMov_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14619 "isa_thumb.tcc"
>::OpMov_3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14623 "isa_thumb.tcc"
>(code, addr, "mov_3")
{
	h1 = ((code >> 7) & 0x1);
	h2 = ((code >> 6) & 0x1);
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14635 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14638 "isa_thumb.tcc"
>
OpMul<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14643 "isa_thumb.tcc"
>::OpMul(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14647 "isa_thumb.tcc"
>(code, addr, "mul")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14657 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14660 "isa_thumb.tcc"
>
OpMvn<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14665 "isa_thumb.tcc"
>::OpMvn(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14669 "isa_thumb.tcc"
>(code, addr, "mvn")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14679 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14682 "isa_thumb.tcc"
>
OpNeg<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14687 "isa_thumb.tcc"
>::OpNeg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14691 "isa_thumb.tcc"
>(code, addr, "neg")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14701 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14704 "isa_thumb.tcc"
>
OpOrr<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14709 "isa_thumb.tcc"
>::OpOrr(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14713 "isa_thumb.tcc"
>(code, addr, "orr")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14723 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14726 "isa_thumb.tcc"
>
OpRor<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14731 "isa_thumb.tcc"
>::OpRor(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14735 "isa_thumb.tcc"
>(code, addr, "ror")
{
	rs = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14745 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14748 "isa_thumb.tcc"
>
OpSbc<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14753 "isa_thumb.tcc"
>::OpSbc(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14757 "isa_thumb.tcc"
>(code, addr, "sbc")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14767 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14770 "isa_thumb.tcc"
>
OpSub_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14775 "isa_thumb.tcc"
>::OpSub_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14779 "isa_thumb.tcc"
>(code, addr, "sub_1")
{
	imm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14790 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14793 "isa_thumb.tcc"
>
OpSub_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14798 "isa_thumb.tcc"
>::OpSub_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14802 "isa_thumb.tcc"
>(code, addr, "sub_2")
{
	rd = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14812 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14815 "isa_thumb.tcc"
>
OpSub_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14820 "isa_thumb.tcc"
>::OpSub_3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14824 "isa_thumb.tcc"
>(code, addr, "sub_3")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14835 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14838 "isa_thumb.tcc"
>
OpSub_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14843 "isa_thumb.tcc"
>::OpSub_4(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14847 "isa_thumb.tcc"
>(code, addr, "sub_4")
{
	imm = ((code >> 0) & 0x7f);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14856 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14859 "isa_thumb.tcc"
>
OpTst<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14864 "isa_thumb.tcc"
>::OpTst(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14868 "isa_thumb.tcc"
>(code, addr, "tst")
{
	rm = ((code >> 3) & 0x7);
	rn = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14878 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14881 "isa_thumb.tcc"
>
OpLdr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14886 "isa_thumb.tcc"
>::OpLdr_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14890 "isa_thumb.tcc"
>(code, addr, "ldr_1")
{
	imm = ((code >> 6) & 0x1f);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14901 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14904 "isa_thumb.tcc"
>
OpLdr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14909 "isa_thumb.tcc"
>::OpLdr_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14913 "isa_thumb.tcc"
>(code, addr, "ldr_2")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14924 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14927 "isa_thumb.tcc"
>
OpLdr_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14932 "isa_thumb.tcc"
>::OpLdr_3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14936 "isa_thumb.tcc"
>(code, addr, "ldr_3")
{
	rd = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14946 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14949 "isa_thumb.tcc"
>
OpLdr_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14954 "isa_thumb.tcc"
>::OpLdr_4(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14958 "isa_thumb.tcc"
>(code, addr, "ldr_4")
{
	rd = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14968 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14971 "isa_thumb.tcc"
>
OpLdrb_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14976 "isa_thumb.tcc"
>::OpLdrb_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14980 "isa_thumb.tcc"
>(code, addr, "ldrb_1")
{
	imm = ((code >> 6) & 0x1f);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14991 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14994 "isa_thumb.tcc"
>
OpLdrb_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14999 "isa_thumb.tcc"
>::OpLdrb_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15003 "isa_thumb.tcc"
>(code, addr, "ldrb_2")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15014 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15017 "isa_thumb.tcc"
>
OpLdrh_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15022 "isa_thumb.tcc"
>::OpLdrh_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15026 "isa_thumb.tcc"
>(code, addr, "ldrh_1")
{
	imm = ((code >> 6) & 0x1f);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15037 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15040 "isa_thumb.tcc"
>
OpLdrh_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15045 "isa_thumb.tcc"
>::OpLdrh_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15049 "isa_thumb.tcc"
>(code, addr, "ldrh_2")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15060 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15063 "isa_thumb.tcc"
>
OpLdrsb<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15068 "isa_thumb.tcc"
>::OpLdrsb(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15072 "isa_thumb.tcc"
>(code, addr, "ldrsb")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15083 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15086 "isa_thumb.tcc"
>
OpLdrsh<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15091 "isa_thumb.tcc"
>::OpLdrsh(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15095 "isa_thumb.tcc"
>(code, addr, "ldrsh")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15106 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15109 "isa_thumb.tcc"
>
OpStr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15114 "isa_thumb.tcc"
>::OpStr_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15118 "isa_thumb.tcc"
>(code, addr, "str_1")
{
	imm = ((code >> 6) & 0x1f);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15129 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15132 "isa_thumb.tcc"
>
OpStr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15137 "isa_thumb.tcc"
>::OpStr_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15141 "isa_thumb.tcc"
>(code, addr, "str_2")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15152 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15155 "isa_thumb.tcc"
>
OpStr_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15160 "isa_thumb.tcc"
>::OpStr_3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15164 "isa_thumb.tcc"
>(code, addr, "str_3")
{
	rd = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15174 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15177 "isa_thumb.tcc"
>
OpStrb_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15182 "isa_thumb.tcc"
>::OpStrb_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15186 "isa_thumb.tcc"
>(code, addr, "strb_1")
{
	imm = ((code >> 6) & 0x1f);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15197 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15200 "isa_thumb.tcc"
>
OpStrb_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15205 "isa_thumb.tcc"
>::OpStrb_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15209 "isa_thumb.tcc"
>(code, addr, "strb_2")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15220 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15223 "isa_thumb.tcc"
>
OpStrh_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15228 "isa_thumb.tcc"
>::OpStrh_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15232 "isa_thumb.tcc"
>(code, addr, "strh_1")
{
	imm = ((code >> 6) & 0x1f);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15243 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15246 "isa_thumb.tcc"
>
OpStrh_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15251 "isa_thumb.tcc"
>::OpStrh_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15255 "isa_thumb.tcc"
>(code, addr, "strh_2")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15266 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15269 "isa_thumb.tcc"
>
OpLdmia<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15274 "isa_thumb.tcc"
>::OpLdmia(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15278 "isa_thumb.tcc"
>(code, addr, "ldmia")
{
	rn = ((code >> 8) & 0x7);
	reg_list = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15288 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15291 "isa_thumb.tcc"
>
OpPop<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15296 "isa_thumb.tcc"
>::OpPop(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15300 "isa_thumb.tcc"
>(code, addr, "pop")
{
	r = ((code >> 8) & 0x1);
	reg_list = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15310 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15313 "isa_thumb.tcc"
>
OpPush<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15318 "isa_thumb.tcc"
>::OpPush(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15322 "isa_thumb.tcc"
>(code, addr, "push")
{
	r = ((code >> 8) & 0x1);
	reg_list = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15332 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15335 "isa_thumb.tcc"
>
OpStmia<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15340 "isa_thumb.tcc"
>::OpStmia(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15344 "isa_thumb.tcc"
>(code, addr, "stmia")
{
	rn = ((code >> 8) & 0x7);
	reg_list = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15354 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15357 "isa_thumb.tcc"
>
OpBkpt<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15362 "isa_thumb.tcc"
>::OpBkpt(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15366 "isa_thumb.tcc"
>(code, addr, "bkpt")
{
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15375 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15378 "isa_thumb.tcc"
>
OpSwi<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15383 "isa_thumb.tcc"
>::OpSwi(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15387 "isa_thumb.tcc"
>(code, addr, "swi")
{
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15396 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15399 "isa_thumb.tcc"
>
DecodeMapPage<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15404 "isa_thumb.tcc"
>::DecodeMapPage(typename CONFIG::address_t key)
{
	this->key = key;
	memset(operation, 0, sizeof(operation));
	next = 0;
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15415 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15418 "isa_thumb.tcc"
>
DecodeMapPage<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15423 "isa_thumb.tcc"
>::~DecodeMapPage()
{
	unsigned int idx;
	for(idx = 0; idx < NUM_OPERATIONS_PER_PAGE; idx++)
	delete operation[idx];
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15434 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15437 "isa_thumb.tcc"
>
DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15442 "isa_thumb.tcc"
>::DecodeTableEntry(CodeType opcode, CodeType opcode_mask, Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15446 "isa_thumb.tcc"
> *(*decode)(CodeType, typename CONFIG::address_t))
{
	this->opcode = opcode;
	this->opcode_mask = opcode_mask;
	this->decode = decode;
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15457 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15460 "isa_thumb.tcc"
>
Decoder<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15465 "isa_thumb.tcc"
>::Decoder()
: is_little_endian( false ), mru_page( 0 )
{
	memset(decode_hash_table, 0, sizeof(decode_hash_table));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15473 "isa_thumb.tcc"
	>(0xdf00U, 0xff00U, DecodeOpSwi<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15477 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15482 "isa_thumb.tcc"
	>(0xbe00U, 0xff00U, DecodeOpBkpt<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15486 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15491 "isa_thumb.tcc"
	>(0xc000U, 0xf800U, DecodeOpStmia<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15495 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15500 "isa_thumb.tcc"
	>(0xb400U, 0xfe00U, DecodeOpPush<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15504 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15509 "isa_thumb.tcc"
	>(0xbc00U, 0xfe00U, DecodeOpPop<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15513 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15518 "isa_thumb.tcc"
	>(0xc800U, 0xf800U, DecodeOpLdmia<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15522 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15527 "isa_thumb.tcc"
	>(0x5200U, 0xfe00U, DecodeOpStrh_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15531 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15536 "isa_thumb.tcc"
	>(0x8000U, 0xf800U, DecodeOpStrh_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15540 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15545 "isa_thumb.tcc"
	>(0x5400U, 0xfe00U, DecodeOpStrb_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15549 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15554 "isa_thumb.tcc"
	>(0x7000U, 0xf800U, DecodeOpStrb_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15558 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15563 "isa_thumb.tcc"
	>(0x9000U, 0xf800U, DecodeOpStr_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15567 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15572 "isa_thumb.tcc"
	>(0x5000U, 0xfe00U, DecodeOpStr_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15576 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15581 "isa_thumb.tcc"
	>(0x6000U, 0xf800U, DecodeOpStr_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15585 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15590 "isa_thumb.tcc"
	>(0x5e00U, 0xfe00U, DecodeOpLdrsh<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15594 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15599 "isa_thumb.tcc"
	>(0x5600U, 0xfe00U, DecodeOpLdrsb<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15603 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15608 "isa_thumb.tcc"
	>(0x5a00U, 0xfe00U, DecodeOpLdrh_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15612 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15617 "isa_thumb.tcc"
	>(0x8800U, 0xf800U, DecodeOpLdrh_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15621 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15626 "isa_thumb.tcc"
	>(0x5c00U, 0xfe00U, DecodeOpLdrb_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15630 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15635 "isa_thumb.tcc"
	>(0x7800U, 0xf800U, DecodeOpLdrb_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15639 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15644 "isa_thumb.tcc"
	>(0x9800U, 0xf800U, DecodeOpLdr_4<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15648 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15653 "isa_thumb.tcc"
	>(0x4800U, 0xf800U, DecodeOpLdr_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15657 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15662 "isa_thumb.tcc"
	>(0x5800U, 0xfe00U, DecodeOpLdr_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15666 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15671 "isa_thumb.tcc"
	>(0x6800U, 0xf800U, DecodeOpLdr_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15675 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15680 "isa_thumb.tcc"
	>(0x4200U, 0xffc0U, DecodeOpTst<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15684 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15689 "isa_thumb.tcc"
	>(0xb080U, 0xff80U, DecodeOpSub_4<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15693 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15698 "isa_thumb.tcc"
	>(0x1a00U, 0xfe00U, DecodeOpSub_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15702 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15707 "isa_thumb.tcc"
	>(0x3800U, 0xf800U, DecodeOpSub_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15711 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15716 "isa_thumb.tcc"
	>(0x1e00U, 0xfe00U, DecodeOpSub_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15720 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15725 "isa_thumb.tcc"
	>(0x4180U, 0xffc0U, DecodeOpSbc<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15729 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15734 "isa_thumb.tcc"
	>(0x41c0U, 0xffc0U, DecodeOpRor<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15738 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15743 "isa_thumb.tcc"
	>(0x4300U, 0xffc0U, DecodeOpOrr<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15747 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15752 "isa_thumb.tcc"
	>(0x4240U, 0xffc0U, DecodeOpNeg<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15756 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15761 "isa_thumb.tcc"
	>(0x43c0U, 0xffc0U, DecodeOpMvn<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15765 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15770 "isa_thumb.tcc"
	>(0x4340U, 0xffc0U, DecodeOpMul<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15774 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15779 "isa_thumb.tcc"
	>(0x4600U, 0xff00U, DecodeOpMov_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15783 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15788 "isa_thumb.tcc"
	>(0x2000U, 0xf800U, DecodeOpMov_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15792 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15797 "isa_thumb.tcc"
	>(0x40c0U, 0xffc0U, DecodeOpLsr_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15801 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15806 "isa_thumb.tcc"
	>(0x800U, 0xf800U, DecodeOpLsr_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15810 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15815 "isa_thumb.tcc"
	>(0x4080U, 0xffc0U, DecodeOpLsl_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15819 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15824 "isa_thumb.tcc"
	>(0x0U, 0xf800U, DecodeOpLsl_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15828 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15833 "isa_thumb.tcc"
	>(0x4040U, 0xffc0U, DecodeOpEor<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15837 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15842 "isa_thumb.tcc"
	>(0x4500U, 0xff00U, DecodeOpCmp_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15846 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15851 "isa_thumb.tcc"
	>(0x4280U, 0xffc0U, DecodeOpCmp_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15855 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15860 "isa_thumb.tcc"
	>(0x2800U, 0xf800U, DecodeOpCmp_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15864 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15869 "isa_thumb.tcc"
	>(0x42c0U, 0xffc0U, DecodeOpCmn<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15873 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15878 "isa_thumb.tcc"
	>(0x4380U, 0xffc0U, DecodeOpBic<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15882 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15887 "isa_thumb.tcc"
	>(0x4100U, 0xffc0U, DecodeOpAsr_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15891 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15896 "isa_thumb.tcc"
	>(0x1000U, 0xf800U, DecodeOpAsr_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15900 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15905 "isa_thumb.tcc"
	>(0x4000U, 0xffc0U, DecodeOpAnd<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15909 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15914 "isa_thumb.tcc"
	>(0xb000U, 0xff80U, DecodeOpAdd_7<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15918 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15923 "isa_thumb.tcc"
	>(0xa800U, 0xf800U, DecodeOpAdd_6<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15927 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15932 "isa_thumb.tcc"
	>(0xa000U, 0xf800U, DecodeOpAdd_5<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15936 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15941 "isa_thumb.tcc"
	>(0x4400U, 0xff00U, DecodeOpAdd_4<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15945 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15950 "isa_thumb.tcc"
	>(0x1800U, 0xfe00U, DecodeOpAdd_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15954 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15959 "isa_thumb.tcc"
	>(0x3000U, 0xf800U, DecodeOpAdd_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15963 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15968 "isa_thumb.tcc"
	>(0x1c00U, 0xfe00U, DecodeOpAdd_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15972 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15977 "isa_thumb.tcc"
	>(0x4140U, 0xffc0U, DecodeOpAdc<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15981 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15986 "isa_thumb.tcc"
	>(0x4780U, 0xff80U, DecodeOpBlx_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15990 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15995 "isa_thumb.tcc"
	>(0xe800U, 0xf800U, DecodeOpBlx_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15999 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16004 "isa_thumb.tcc"
	>(0x4700U, 0xff80U, DecodeOpBx<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16008 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16013 "isa_thumb.tcc"
	>(0xf000U, 0xf000U, DecodeOpBl<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16017 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16022 "isa_thumb.tcc"
	>(0xe000U, 0xf800U, DecodeOpB_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16026 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16031 "isa_thumb.tcc"
	>(0xdc00U, 0xfe00U, DecodeOpB_1_110x<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16035 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16040 "isa_thumb.tcc"
	>(0xd800U, 0xfc00U, DecodeOpB_1_10xx<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16044 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16049 "isa_thumb.tcc"
	>(0xd000U, 0xf800U, DecodeOpB_1_0xxx<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16053 "isa_thumb.tcc"
	>));
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 16060 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16063 "isa_thumb.tcc"
>
Decoder<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16068 "isa_thumb.tcc"
>::~Decoder()
{
	InvalidateDecodingCache();
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 16077 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16080 "isa_thumb.tcc"
>
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16085 "isa_thumb.tcc"
> *Decoder<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16089 "isa_thumb.tcc"
>::NCDecode(typename CONFIG::address_t addr, CodeType code)
{
	Operation<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16095 "isa_thumb.tcc"
	> *operation;
	unsigned int count = decode_table.size();
	unsigned int idx;
	for(idx = 0; idx < count; idx++)
	{
		if((code & decode_table[idx].opcode_mask) == decode_table[idx].opcode)
		{
			operation = decode_table[idx].decode(code, addr);
			operation->initialize_rd_index();
			operation->initialize_rn_index();
			operation->initialize_rm_index();
			operation->initialize_rdp1_init();
			operation->initialize_rdlo_init();
			operation->initialize_rdhi_init();
			operation->initialize_rs_init();
			operation->initialize_rd_init();
			operation->initialize_rm_init();
			operation->initialize_rn_init();
			operation->initialize_cond_init();
			return operation;
		}
	}
	operation = new Operation<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16121 "isa_thumb.tcc"
	>(code, addr, "???");
	operation->initialize_rd_index();
	operation->initialize_rn_index();
	operation->initialize_rm_index();
	operation->initialize_rdp1_init();
	operation->initialize_rdlo_init();
	operation->initialize_rdhi_init();
	operation->initialize_rs_init();
	operation->initialize_rd_init();
	operation->initialize_rm_init();
	operation->initialize_rn_init();
	operation->initialize_cond_init();
	return operation;
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 16140 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16143 "isa_thumb.tcc"
>
void Decoder<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16148 "isa_thumb.tcc"
>::InvalidateDecodingCache()
{
	uint32_t index;
	mru_page = 0;
	for(index = 0; index < NUM_DECODE_HASH_TABLE_ENTRIES; index++)
	{
		DecodeMapPage<
#line 44 "isa/thumb/thumb.isa"
		CONFIG
#line 16158 "isa_thumb.tcc"
		> *page, *next_page;
		page = decode_hash_table[index];
		if(page)
		{
			do
			{
				next_page = page->next;
				delete page;
				page = next_page;
			} while(page);
			decode_hash_table[index] = 0;
		}
	}
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 16177 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16180 "isa_thumb.tcc"
>
void Decoder<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16185 "isa_thumb.tcc"
>::InvalidateDecodingCacheEntry(typename CONFIG::address_t addr)
{
	typename CONFIG::address_t page_key = addr / 2 / NUM_OPERATIONS_PER_PAGE;
	if(mru_page && mru_page->key == page_key) mru_page = 0;
	uint32_t index = page_key % NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key
	DecodeMapPage<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16194 "isa_thumb.tcc"
	> *prev, *cur;
	cur = decode_hash_table[index];
	if(cur)
	{
		if(cur->key == page_key)
		{
			decode_hash_table[index] = cur->next;
			delete cur;
			return;
		}
		prev = cur;
		cur = cur->next;
		if(cur)
		{
			do
			{
				if(cur->key == page_key)
				{
					prev->next = cur->next;
					cur->next = 0;
					delete cur;
					return;
				}
				prev = cur;
			} while((cur = cur->next) != 0);
		}
	}
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 16227 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16230 "isa_thumb.tcc"
>
inline DecodeMapPage<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16235 "isa_thumb.tcc"
> *Decoder<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16239 "isa_thumb.tcc"
>::FindPage(typename CONFIG::address_t page_key)
{
	if(mru_page && mru_page->key == page_key) return mru_page;
	uint32_t index = page_key % NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key
	DecodeMapPage<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16247 "isa_thumb.tcc"
	> *prev, *cur;
	cur = decode_hash_table[index];
	if(cur)
	{
		if(cur->key == page_key)
		{
			mru_page = cur;
			return cur;
		}
		prev = cur;
		cur = cur->next;
		if(cur)
		{
			do
			{
				if(cur->key == page_key)
				{
					prev->next = cur->next;
					cur->next= decode_hash_table[index];
					decode_hash_table[index] = cur;
					mru_page = cur;
					return cur;
				}
				prev = cur;
			} while((cur = cur->next) != 0);
		}
	}
	return 0;
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 16281 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16284 "isa_thumb.tcc"
>
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16289 "isa_thumb.tcc"
> *Decoder<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16293 "isa_thumb.tcc"
>::Decode(typename CONFIG::address_t addr, CodeType insn)
{
	Operation<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16299 "isa_thumb.tcc"
	> *operation;
	typename CONFIG::address_t page_key = addr / 2 / NUM_OPERATIONS_PER_PAGE;
	DecodeMapPage<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16305 "isa_thumb.tcc"
	> *page;
	page = FindPage(page_key);
	if(!page)
	{
		page = new DecodeMapPage<
#line 44 "isa/thumb/thumb.isa"
		CONFIG
#line 16313 "isa_thumb.tcc"
		> (page_key);
		uint32_t index = page_key % NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key
		page->next = decode_hash_table[index];
		decode_hash_table[index] = page;
		mru_page = page;
	}
	operation = page->operation[(addr / 2) & (NUM_OPERATIONS_PER_PAGE - 1)];
	if(operation)
	{
		if(operation->GetEncoding() == insn && operation->GetAddr() == addr)
		return operation;
		delete operation;
	}
	operation = NCDecode(addr, insn);
	page->operation[(addr / 2) & (NUM_OPERATIONS_PER_PAGE - 1)] = operation;
	return operation;
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 16335 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16338 "isa_thumb.tcc"
>
void Decoder<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16343 "isa_thumb.tcc"
>::SetLittleEndian()
{
	is_little_endian = true;
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 16352 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16355 "isa_thumb.tcc"
>
void Decoder<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16360 "isa_thumb.tcc"
>::SetBigEndian()
{
	is_little_endian = false;
}

} } } } } } }
