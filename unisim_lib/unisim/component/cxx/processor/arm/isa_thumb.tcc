#include "isa_thumb.hh"
#include <cassert>
#line 75 "isa/thumb/thumb.isa"

#include "unisim/util/arithmetic/arithmetic.hh"
#include "unisim/component/cxx/processor/arm/exception.hh"
#include "unisim/kernel/logger/logger.hh"

using unisim::component::cxx::processor::arm::UndefinedInstructionException;
using unisim::util::arithmetic::Add32;
using unisim::kernel::logger::DebugInfo;
using unisim::kernel::logger::EndDebugInfo;
using unisim::kernel::logger::DebugWarning;
using unisim::kernel::logger::EndDebugWarning;
using unisim::kernel::logger::DebugError;
using unisim::kernel::logger::EndDebugError;
#line 18 "isa_thumb.tcc"
namespace unisim { namespace component { namespace cxx { namespace processor { namespace arm { namespace isa { namespace thumb {
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 23 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 26 "isa_thumb.tcc"
>
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 31 "isa_thumb.tcc"
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
#line 43 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 46 "isa_thumb.tcc"
>
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 51 "isa_thumb.tcc"
>::~Operation()
{
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 59 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 62 "isa_thumb.tcc"
>
#line 53 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/profiling.isa"
void
#line 66 "isa_thumb.tcc"
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 70 "isa_thumb.tcc"
>::profile(
#line 53 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/profiling.isa"
map<uint64_t, uint32_t> &
#line 74 "isa_thumb.tcc"
#line 53 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/profiling.isa"
prof
#line 77 "isa_thumb.tcc"
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
#line 91 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 96 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 99 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 103 "isa_thumb.tcc"
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 107 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 111 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 114 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 118 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 121 "isa_thumb.tcc"
)
{
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	{
		buffer << "Unknown instruction";
	}
#line 128 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 133 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 136 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 140 "isa_thumb.tcc"
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 144 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 148 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 151 "isa_thumb.tcc"
)
{
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	{
		throw UndefinedInstructionException<CONFIG>();
	}
#line 158 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 163 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 166 "isa_thumb.tcc"
>

void
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 173 "isa_thumb.tcc"
>::initialize_rd_index()
{
#line 89 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rd_index = 0;
	}
#line 180 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 185 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 188 "isa_thumb.tcc"
>

void
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 195 "isa_thumb.tcc"
>::initialize_rn_index()
{
#line 85 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rn_index = 0;
	}
#line 202 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 207 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 210 "isa_thumb.tcc"
>

void
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 217 "isa_thumb.tcc"
>::initialize_rm_index()
{
#line 81 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rm_index = 0;
	}
#line 224 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 229 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 232 "isa_thumb.tcc"
>

void
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 239 "isa_thumb.tcc"
>::initialize_rdp1_init()
{
#line 73 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rdp1_init = 0;
	}
#line 246 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 251 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 254 "isa_thumb.tcc"
>

void
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 261 "isa_thumb.tcc"
>::initialize_rdlo_init()
{
#line 69 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rdlo_init = 0;
	}
#line 268 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 273 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 276 "isa_thumb.tcc"
>

void
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 283 "isa_thumb.tcc"
>::initialize_rdhi_init()
{
#line 65 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rdhi_init = 0;
	}
#line 290 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 295 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 298 "isa_thumb.tcc"
>

void
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 305 "isa_thumb.tcc"
>::initialize_rs_init()
{
#line 61 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rs_init = 0;
	}
#line 312 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 317 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 320 "isa_thumb.tcc"
>

void
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 327 "isa_thumb.tcc"
>::initialize_rd_init()
{
#line 57 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rd_init = 0;
	}
#line 334 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 339 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 342 "isa_thumb.tcc"
>

void
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 349 "isa_thumb.tcc"
>::initialize_rm_init()
{
#line 53 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rm_init = 0;
	}
#line 356 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 361 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 364 "isa_thumb.tcc"
>

void
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 371 "isa_thumb.tcc"
>::initialize_rn_init()
{
#line 49 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rn_init = 0;
	}
#line 378 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 383 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 386 "isa_thumb.tcc"
>

void
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 393 "isa_thumb.tcc"
>::initialize_cond_init()
{
#line 37 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		cond_init = 0;
	}
#line 400 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 405 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 408 "isa_thumb.tcc"
>
class OpB_1_0xxx : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 413 "isa_thumb.tcc"
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
#line 498 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 502 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 505 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 509 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 512 "isa_thumb.tcc"
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
#line 550 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 554 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 557 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 565 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 568 "isa_thumb.tcc"
>
class OpB_1_10xx : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 573 "isa_thumb.tcc"
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
#line 658 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 662 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 665 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 669 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 672 "isa_thumb.tcc"
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
#line 710 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 714 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 717 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 725 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 728 "isa_thumb.tcc"
>
class OpB_1_110x : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 733 "isa_thumb.tcc"
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
#line 818 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 822 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 825 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 829 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 832 "isa_thumb.tcc"
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
#line 870 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 874 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 877 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 885 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 888 "isa_thumb.tcc"
>
class OpB_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 893 "isa_thumb.tcc"
>
{
public:
	OpB_2(CodeType code, typename CONFIG::address_t addr);
	int32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 902 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 906 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 909 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 913 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 916 "isa_thumb.tcc"
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
#line 954 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 958 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 961 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 969 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 972 "isa_thumb.tcc"
>
class OpBl : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 977 "isa_thumb.tcc"
>
{
public:
	OpBl(CodeType code, typename CONFIG::address_t addr);
	uint32_t h2;
	uint32_t offset;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 987 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 991 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 994 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 998 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1001 "isa_thumb.tcc"
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
#line 1039 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1043 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1046 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 1054 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1057 "isa_thumb.tcc"
>
class OpBx : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1062 "isa_thumb.tcc"
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
#line 1073 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1077 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1080 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1084 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1087 "isa_thumb.tcc"
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
#line 1131 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1135 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1138 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 1146 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1149 "isa_thumb.tcc"
>
class OpBlx_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1154 "isa_thumb.tcc"
>
{
public:
	OpBlx_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t offset;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1163 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1167 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1170 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1174 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1177 "isa_thumb.tcc"
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
#line 1215 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1219 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1222 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 1230 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1233 "isa_thumb.tcc"
>
class OpBlx_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1238 "isa_thumb.tcc"
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
#line 1255 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1259 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1262 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1266 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1269 "isa_thumb.tcc"
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
#line 1307 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1311 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1314 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 1322 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1325 "isa_thumb.tcc"
>
class OpAdc : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1330 "isa_thumb.tcc"
>
{
public:
	OpAdc(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1340 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1344 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1347 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1351 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1354 "isa_thumb.tcc"
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
#line 1392 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1396 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1399 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 1407 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1410 "isa_thumb.tcc"
>
class OpAdd_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1415 "isa_thumb.tcc"
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
#line 1426 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1430 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1433 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1437 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1440 "isa_thumb.tcc"
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
#line 1478 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1482 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1485 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 1493 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1496 "isa_thumb.tcc"
>
class OpAdd_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1501 "isa_thumb.tcc"
>
{
public:
	OpAdd_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rd;
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1511 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1515 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1518 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1522 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1525 "isa_thumb.tcc"
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
#line 1563 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1567 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1570 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 1578 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1581 "isa_thumb.tcc"
>
class OpAdd_3 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1586 "isa_thumb.tcc"
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
#line 1597 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1601 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1604 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1608 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1611 "isa_thumb.tcc"
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
#line 1649 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1653 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1656 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 1664 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1667 "isa_thumb.tcc"
>
class OpAdd_4 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1672 "isa_thumb.tcc"
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
#line 1696 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1700 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1703 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1707 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1710 "isa_thumb.tcc"
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
#line 1748 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1752 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1755 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 1763 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1766 "isa_thumb.tcc"
>
class OpAdd_5 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1771 "isa_thumb.tcc"
>
{
public:
	OpAdd_5(CodeType code, typename CONFIG::address_t addr);
	uint32_t rd;
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1781 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1785 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1788 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1792 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1795 "isa_thumb.tcc"
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
#line 1833 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1837 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1840 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 1848 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1851 "isa_thumb.tcc"
>
class OpAdd_6 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1856 "isa_thumb.tcc"
>
{
public:
	OpAdd_6(CodeType code, typename CONFIG::address_t addr);
	uint32_t rd;
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1866 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1870 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1873 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1877 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1880 "isa_thumb.tcc"
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
#line 1918 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1922 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1925 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 1933 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1936 "isa_thumb.tcc"
>
class OpAdd_7 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1941 "isa_thumb.tcc"
>
{
public:
	OpAdd_7(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1950 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1954 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1957 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1961 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1964 "isa_thumb.tcc"
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
#line 2002 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2006 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2009 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2017 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2020 "isa_thumb.tcc"
>
class OpAnd : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2025 "isa_thumb.tcc"
>
{
public:
	OpAnd(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2035 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2039 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2042 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2046 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2049 "isa_thumb.tcc"
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
#line 2087 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2091 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2094 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2102 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2105 "isa_thumb.tcc"
>
class OpAsr_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2110 "isa_thumb.tcc"
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
#line 2121 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2125 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2128 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2132 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2135 "isa_thumb.tcc"
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
#line 2173 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2177 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2180 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2188 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2191 "isa_thumb.tcc"
>
class OpAsr_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2196 "isa_thumb.tcc"
>
{
public:
	OpAsr_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rs;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2206 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2210 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2213 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2217 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2220 "isa_thumb.tcc"
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
#line 2258 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2262 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2265 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2273 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2276 "isa_thumb.tcc"
>
class OpBic : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2281 "isa_thumb.tcc"
>
{
public:
	OpBic(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2291 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2295 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2298 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2302 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2305 "isa_thumb.tcc"
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
#line 2343 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2347 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2350 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2358 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2361 "isa_thumb.tcc"
>
class OpCmn : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2366 "isa_thumb.tcc"
>
{
public:
	OpCmn(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2376 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2380 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2383 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2387 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2390 "isa_thumb.tcc"
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
#line 2428 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2432 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2435 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2443 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2446 "isa_thumb.tcc"
>
class OpCmp_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2451 "isa_thumb.tcc"
>
{
public:
	OpCmp_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t rn;
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2461 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2465 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2468 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2472 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2475 "isa_thumb.tcc"
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
#line 2513 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2517 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2520 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2528 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2531 "isa_thumb.tcc"
>
class OpCmp_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2536 "isa_thumb.tcc"
>
{
public:
	OpCmp_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2546 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2550 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2553 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2557 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2560 "isa_thumb.tcc"
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
#line 2598 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2602 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2605 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2613 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2616 "isa_thumb.tcc"
>
class OpCmp_3 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2621 "isa_thumb.tcc"
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
#line 2645 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2649 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2652 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2656 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2659 "isa_thumb.tcc"
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
#line 2697 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2701 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2704 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2712 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2715 "isa_thumb.tcc"
>
class OpEor : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2720 "isa_thumb.tcc"
>
{
public:
	OpEor(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2730 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2734 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2737 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2741 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2744 "isa_thumb.tcc"
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
#line 2782 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2786 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2789 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2797 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2800 "isa_thumb.tcc"
>
class OpLsl_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2805 "isa_thumb.tcc"
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
#line 2816 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2820 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2823 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2827 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2830 "isa_thumb.tcc"
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
#line 2868 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2872 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2875 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2883 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2886 "isa_thumb.tcc"
>
class OpLsl_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2891 "isa_thumb.tcc"
>
{
public:
	OpLsl_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rs;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2901 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2905 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2908 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2912 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2915 "isa_thumb.tcc"
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
#line 2953 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2957 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2960 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2968 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2971 "isa_thumb.tcc"
>
class OpLsr_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2976 "isa_thumb.tcc"
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
#line 2987 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2991 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2994 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2998 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3001 "isa_thumb.tcc"
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
#line 3039 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3043 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3046 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 3054 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3057 "isa_thumb.tcc"
>
class OpLsr_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3062 "isa_thumb.tcc"
>
{
public:
	OpLsr_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rs;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3072 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3076 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3079 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3083 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3086 "isa_thumb.tcc"
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
#line 3124 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3128 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3131 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 3139 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3142 "isa_thumb.tcc"
>
class OpMov_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3147 "isa_thumb.tcc"
>
{
public:
	OpMov_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t rd;
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3157 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3161 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3164 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3168 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3171 "isa_thumb.tcc"
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
#line 3209 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3213 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3216 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 3224 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3227 "isa_thumb.tcc"
>
class OpMov_3 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3232 "isa_thumb.tcc"
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
#line 3253 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3257 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3260 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3264 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3267 "isa_thumb.tcc"
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
#line 3305 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3309 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3312 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 3320 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3323 "isa_thumb.tcc"
>
class OpMul : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3328 "isa_thumb.tcc"
>
{
public:
	OpMul(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3338 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3342 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3345 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3349 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3352 "isa_thumb.tcc"
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
#line 3390 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3394 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3397 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 3405 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3408 "isa_thumb.tcc"
>
class OpMvn : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3413 "isa_thumb.tcc"
>
{
public:
	OpMvn(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3423 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3427 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3430 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3434 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3437 "isa_thumb.tcc"
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
#line 3475 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3479 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3482 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 3490 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3493 "isa_thumb.tcc"
>
class OpNeg : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3498 "isa_thumb.tcc"
>
{
public:
	OpNeg(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3508 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3512 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3515 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3519 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3522 "isa_thumb.tcc"
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
#line 3560 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3564 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3567 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 3575 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3578 "isa_thumb.tcc"
>
class OpOrr : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3583 "isa_thumb.tcc"
>
{
public:
	OpOrr(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3593 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3597 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3600 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3604 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3607 "isa_thumb.tcc"
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
#line 3645 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3649 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3652 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 3660 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3663 "isa_thumb.tcc"
>
class OpRor : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3668 "isa_thumb.tcc"
>
{
public:
	OpRor(CodeType code, typename CONFIG::address_t addr);
	uint32_t rs;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3678 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3682 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3685 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3689 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3692 "isa_thumb.tcc"
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
#line 3730 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3734 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3737 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 3745 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3748 "isa_thumb.tcc"
>
class OpSbc : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3753 "isa_thumb.tcc"
>
{
public:
	OpSbc(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3763 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3767 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3770 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3774 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3777 "isa_thumb.tcc"
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
#line 3815 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3819 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3822 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 3830 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3833 "isa_thumb.tcc"
>
class OpSub_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3838 "isa_thumb.tcc"
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
#line 3849 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3853 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3856 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3860 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3863 "isa_thumb.tcc"
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
#line 3901 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3905 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3908 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 3916 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3919 "isa_thumb.tcc"
>
class OpSub_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3924 "isa_thumb.tcc"
>
{
public:
	OpSub_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rd;
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3934 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3938 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3941 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3945 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3948 "isa_thumb.tcc"
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
#line 3986 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3990 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3993 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4001 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4004 "isa_thumb.tcc"
>
class OpSub_3 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4009 "isa_thumb.tcc"
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
#line 4020 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4024 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4027 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4031 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4034 "isa_thumb.tcc"
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
#line 4072 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4076 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4079 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4087 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4090 "isa_thumb.tcc"
>
class OpSub_4 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4095 "isa_thumb.tcc"
>
{
public:
	OpSub_4(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4104 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4108 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4111 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4115 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4118 "isa_thumb.tcc"
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
#line 4156 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4160 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4163 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4171 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4174 "isa_thumb.tcc"
>
class OpTst : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4179 "isa_thumb.tcc"
>
{
public:
	OpTst(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4189 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4193 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4196 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4200 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4203 "isa_thumb.tcc"
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
#line 4241 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4245 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4248 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4256 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4259 "isa_thumb.tcc"
>
class OpLdr_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4264 "isa_thumb.tcc"
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
#line 4275 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4279 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4282 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4286 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4289 "isa_thumb.tcc"
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
#line 4327 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4331 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4334 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4342 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4345 "isa_thumb.tcc"
>
class OpLdr_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4350 "isa_thumb.tcc"
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
#line 4361 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4365 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4368 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4372 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4375 "isa_thumb.tcc"
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
#line 4413 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4417 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4420 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4428 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4431 "isa_thumb.tcc"
>
class OpLdr_3 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4436 "isa_thumb.tcc"
>
{
public:
	OpLdr_3(CodeType code, typename CONFIG::address_t addr);
	uint32_t rd;
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4446 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4450 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4453 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4457 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4460 "isa_thumb.tcc"
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
#line 4498 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4502 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4505 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4513 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4516 "isa_thumb.tcc"
>
class OpLdr_4 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4521 "isa_thumb.tcc"
>
{
public:
	OpLdr_4(CodeType code, typename CONFIG::address_t addr);
	uint32_t rd;
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4531 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4535 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4538 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4542 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4545 "isa_thumb.tcc"
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
#line 4583 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4587 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4590 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4598 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4601 "isa_thumb.tcc"
>
class OpLdrb_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4606 "isa_thumb.tcc"
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
#line 4617 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4621 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4624 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4628 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4631 "isa_thumb.tcc"
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
#line 4669 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4673 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4676 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4684 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4687 "isa_thumb.tcc"
>
class OpLdrb_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4692 "isa_thumb.tcc"
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
#line 4703 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4707 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4710 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4714 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4717 "isa_thumb.tcc"
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
#line 4755 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4759 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4762 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4770 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4773 "isa_thumb.tcc"
>
class OpLdrh_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4778 "isa_thumb.tcc"
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
#line 4789 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4793 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4796 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4800 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4803 "isa_thumb.tcc"
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
#line 4841 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4845 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4848 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4856 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4859 "isa_thumb.tcc"
>
class OpLdrh_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4864 "isa_thumb.tcc"
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
#line 4875 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4879 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4882 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4886 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4889 "isa_thumb.tcc"
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
#line 4927 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4931 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4934 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4942 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4945 "isa_thumb.tcc"
>
class OpLdrsb : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4950 "isa_thumb.tcc"
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
#line 4961 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4965 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4968 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4972 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4975 "isa_thumb.tcc"
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
#line 5013 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5017 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5020 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5028 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5031 "isa_thumb.tcc"
>
class OpLdrsh : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5036 "isa_thumb.tcc"
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
#line 5047 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5051 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5054 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5058 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5061 "isa_thumb.tcc"
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
#line 5099 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5103 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5106 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5114 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5117 "isa_thumb.tcc"
>
class OpStr_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5122 "isa_thumb.tcc"
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
#line 5133 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5137 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5140 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5144 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5147 "isa_thumb.tcc"
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
#line 5185 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5189 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5192 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5200 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5203 "isa_thumb.tcc"
>
class OpStr_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5208 "isa_thumb.tcc"
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
#line 5219 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5223 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5226 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5230 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5233 "isa_thumb.tcc"
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
#line 5271 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5275 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5278 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5286 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5289 "isa_thumb.tcc"
>
class OpStr_3 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5294 "isa_thumb.tcc"
>
{
public:
	OpStr_3(CodeType code, typename CONFIG::address_t addr);
	uint32_t rd;
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5304 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5308 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5311 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5315 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5318 "isa_thumb.tcc"
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
#line 5356 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5360 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5363 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5371 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5374 "isa_thumb.tcc"
>
class OpStrb_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5379 "isa_thumb.tcc"
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
#line 5390 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5394 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5397 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5401 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5404 "isa_thumb.tcc"
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
#line 5442 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5446 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5449 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5457 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5460 "isa_thumb.tcc"
>
class OpStrb_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5465 "isa_thumb.tcc"
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
#line 5476 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5480 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5483 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5487 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5490 "isa_thumb.tcc"
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
#line 5528 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5532 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5535 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5543 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5546 "isa_thumb.tcc"
>
class OpStrh_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5551 "isa_thumb.tcc"
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
#line 5562 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5566 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5569 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5573 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5576 "isa_thumb.tcc"
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
#line 5614 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5618 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5621 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5629 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5632 "isa_thumb.tcc"
>
class OpStrh_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5637 "isa_thumb.tcc"
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
#line 5648 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5652 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5655 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5659 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5662 "isa_thumb.tcc"
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
#line 5700 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5704 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5707 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5715 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5718 "isa_thumb.tcc"
>
class OpLdmia : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5723 "isa_thumb.tcc"
>
{
public:
	OpLdmia(CodeType code, typename CONFIG::address_t addr);
	uint32_t rn;
	uint32_t reg_list;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5733 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5737 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5740 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5744 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5747 "isa_thumb.tcc"
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
#line 5785 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5789 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5792 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5800 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5803 "isa_thumb.tcc"
>
class OpPop : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5808 "isa_thumb.tcc"
>
{
public:
	OpPop(CodeType code, typename CONFIG::address_t addr);
	uint32_t r;
	uint32_t reg_list;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5818 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5822 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5825 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5829 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5832 "isa_thumb.tcc"
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
#line 5870 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5874 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5877 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5885 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5888 "isa_thumb.tcc"
>
class OpPush : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5893 "isa_thumb.tcc"
>
{
public:
	OpPush(CodeType code, typename CONFIG::address_t addr);
	uint32_t r;
	uint32_t reg_list;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5903 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5907 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5910 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5914 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5917 "isa_thumb.tcc"
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
#line 5955 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5959 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5962 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5970 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5973 "isa_thumb.tcc"
>
class OpStmia : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5978 "isa_thumb.tcc"
>
{
public:
	OpStmia(CodeType code, typename CONFIG::address_t addr);
	uint32_t rn;
	uint32_t reg_list;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5988 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5992 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5995 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5999 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 6002 "isa_thumb.tcc"
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
#line 6040 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 6044 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 6047 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6055 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6058 "isa_thumb.tcc"
>
class OpBkpt : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6063 "isa_thumb.tcc"
>
{
public:
	OpBkpt(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 6072 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 6076 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 6079 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 6083 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 6086 "isa_thumb.tcc"
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
#line 6124 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 6128 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 6131 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6139 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6142 "isa_thumb.tcc"
>
class OpSwi : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6147 "isa_thumb.tcc"
>
{
public:
	OpSwi(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 6156 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 6160 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 6163 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 6167 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 6170 "isa_thumb.tcc"
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
#line 6208 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 6212 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 6215 "isa_thumb.tcc"
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
#line 6265 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6268 "isa_thumb.tcc"
>

void
OpB_1_0xxx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6275 "isa_thumb.tcc"
>::initialize_cond_init()
{
#line 50 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::cond_init = sub_cond;
	}
#line 6284 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6289 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6292 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6296 "isa_thumb.tcc"
OpB_1_0xxx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6300 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6304 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6307 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 6311 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 6314 "isa_thumb.tcc"
)
{
#line 70 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;
		buffer << "b";
		cpu.DisasmCondition(inherited::cond_init, buffer);
		buffer << " 0x" << hex << cpu.GetGPR(cpu.PC_reg) + 4 + (imm << 1) << dec;
	}
#line 6324 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6329 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6332 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6336 "isa_thumb.tcc"
OpB_1_0xxx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6340 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6344 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6347 "isa_thumb.tcc"
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
#line 6364 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6370 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6373 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6378 "isa_thumb.tcc"
> *DecodeOpB_1_0xxx(CodeType code, typename CONFIG::address_t addr)
{
	return new OpB_1_0xxx<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 6384 "isa_thumb.tcc"
	>(code, addr);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6391 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6394 "isa_thumb.tcc"
>

void
OpB_1_10xx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6401 "isa_thumb.tcc"
>::initialize_cond_init()
{
#line 56 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::cond_init = sub_cond + 8;
	}
#line 6410 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6415 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6418 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6422 "isa_thumb.tcc"
OpB_1_10xx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6426 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6430 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6433 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 6437 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 6440 "isa_thumb.tcc"
)
{
#line 70 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;
		buffer << "b";
		cpu.DisasmCondition(inherited::cond_init, buffer);
		buffer << " 0x" << hex << cpu.GetGPR(cpu.PC_reg) + 4 + (imm << 1) << dec;
	}
#line 6450 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6455 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6458 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6462 "isa_thumb.tcc"
OpB_1_10xx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6466 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6470 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6473 "isa_thumb.tcc"
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
#line 6490 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6496 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6499 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6504 "isa_thumb.tcc"
> *DecodeOpB_1_10xx(CodeType code, typename CONFIG::address_t addr)
{
	return new OpB_1_10xx<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 6510 "isa_thumb.tcc"
	>(code, addr);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6517 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6520 "isa_thumb.tcc"
>

void
OpB_1_110x<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6527 "isa_thumb.tcc"
>::initialize_cond_init()
{
#line 62 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::cond_init = sub_cond + 12;
	}
#line 6536 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6541 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6544 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6548 "isa_thumb.tcc"
OpB_1_110x<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6552 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6556 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6559 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 6563 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 6566 "isa_thumb.tcc"
)
{
#line 70 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;
		buffer << "b";
		cpu.DisasmCondition(inherited::cond_init, buffer);
		buffer << " 0x" << hex << cpu.GetGPR(cpu.PC_reg) + 4 + (imm << 1) << dec;
	}
#line 6576 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6581 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6584 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6588 "isa_thumb.tcc"
OpB_1_110x<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6592 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6596 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6599 "isa_thumb.tcc"
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
#line 6616 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6622 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6625 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6630 "isa_thumb.tcc"
> *DecodeOpB_1_110x(CodeType code, typename CONFIG::address_t addr)
{
	return new OpB_1_110x<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 6636 "isa_thumb.tcc"
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
#line 6650 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6653 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6657 "isa_thumb.tcc"
OpB_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6661 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6665 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6668 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 6672 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 6675 "isa_thumb.tcc"
)
{
#line 102 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		buffer << "b";
		buffer << " 0x" << hex << cpu.GetGPR(cpu.PC_reg) + 4 + (imm << 1) << dec;
	}
#line 6685 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6690 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6693 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6697 "isa_thumb.tcc"
OpB_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6701 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6705 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6708 "isa_thumb.tcc"
)
{
#line 109 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		typename CONFIG::reg_t new_PC;

		new_PC = cpu.GetGPR(cpu.PC_reg) + 4 + (imm << 1);
		cpu.SetGPR(cpu.PC_reg, new_PC);
	}
#line 6720 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6726 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6729 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6734 "isa_thumb.tcc"
> *DecodeOpB_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpB_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 6740 "isa_thumb.tcc"
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
#line 6754 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6757 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6761 "isa_thumb.tcc"
OpBl<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6765 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6769 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6772 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 6776 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 6779 "isa_thumb.tcc"
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
#line 6797 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6802 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6805 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6809 "isa_thumb.tcc"
OpBl<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6813 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6817 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6820 "isa_thumb.tcc"
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
#line 6845 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6851 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6854 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6859 "isa_thumb.tcc"
> *DecodeOpBl(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBl<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 6865 "isa_thumb.tcc"
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
#line 6879 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6882 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6886 "isa_thumb.tcc"
OpBx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6890 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6894 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6897 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 6901 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 6904 "isa_thumb.tcc"
)
{
#line 177 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		buffer << "bx "
		<< "r" << inherited::rm_index << endl;
	}
#line 6914 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6919 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6922 "isa_thumb.tcc"
>

void
OpBx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6929 "isa_thumb.tcc"
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
#line 6941 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6946 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6949 "isa_thumb.tcc"
>

void
OpBx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6956 "isa_thumb.tcc"
>::initialize_rm_index()
{
#line 193 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::rm_index = (h2 << 3) + rm;
	}
#line 6965 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6970 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6973 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6977 "isa_thumb.tcc"
OpBx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6981 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6985 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6988 "isa_thumb.tcc"
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
#line 7007 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7013 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7016 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7021 "isa_thumb.tcc"
> *DecodeOpBx(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBx<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 7027 "isa_thumb.tcc"
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
#line 7041 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7044 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7048 "isa_thumb.tcc"
OpBlx_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7052 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7056 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7059 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 7063 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 7066 "isa_thumb.tcc"
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
#line 7085 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7090 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7093 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7097 "isa_thumb.tcc"
OpBlx_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7101 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7105 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7108 "isa_thumb.tcc"
)
{
#line 242 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typename CONFIG::reg_t new_PC;

		new_PC = (cpu.GetGPR(cpu.LR_reg) + (offset << 1)) & (typename CONFIG::reg_t)0xFFFFFFFC;
		cpu.SetGPR(cpu.LR_reg, cpu.GetGPR(cpu.PC_reg) + 3);
		cpu.SetGPR(cpu.PC_reg, new_PC);
	}
#line 7119 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7125 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7128 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7133 "isa_thumb.tcc"
> *DecodeOpBlx_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBlx_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 7139 "isa_thumb.tcc"
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
#line 7153 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7156 "isa_thumb.tcc"
>

void
OpBlx_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7163 "isa_thumb.tcc"
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
#line 7175 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7180 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7183 "isa_thumb.tcc"
>

void
OpBlx_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7190 "isa_thumb.tcc"
>::initialize_rm_index()
{
#line 270 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::rm_index = (h2 << 3) + rm;
	}
#line 7199 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7204 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7207 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7211 "isa_thumb.tcc"
OpBlx_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7215 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7219 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7222 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 7226 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 7229 "isa_thumb.tcc"
)
{
#line 276 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		buffer << "blx r" << inherited::rm_index;
	}
#line 7238 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7243 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7246 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7250 "isa_thumb.tcc"
OpBlx_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7254 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7258 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7261 "isa_thumb.tcc"
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
#line 7280 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7286 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7289 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7294 "isa_thumb.tcc"
> *DecodeOpBlx_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBlx_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 7300 "isa_thumb.tcc"
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
#line 7351 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7354 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7358 "isa_thumb.tcc"
OpAdc<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7362 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7366 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7369 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 7373 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 7376 "isa_thumb.tcc"
)
{
#line 48 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "adc r" << rd << ", r" << rm;
	}
#line 7383 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7388 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7391 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7395 "isa_thumb.tcc"
OpAdc<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7399 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7403 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7406 "isa_thumb.tcc"
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
#line 7429 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7435 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7438 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7443 "isa_thumb.tcc"
> *DecodeOpAdc(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAdc<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 7449 "isa_thumb.tcc"
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
#line 7463 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7466 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7470 "isa_thumb.tcc"
OpAdd_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7474 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7478 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7481 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 7485 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 7488 "isa_thumb.tcc"
)
{
#line 83 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "add r" << rd << ", r" << rn << ", #" << dec << imm;
	}
#line 7495 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7500 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7503 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7507 "isa_thumb.tcc"
OpAdd_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7511 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7515 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7518 "isa_thumb.tcc"
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
#line 7540 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7546 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7549 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7554 "isa_thumb.tcc"
> *DecodeOpAdd_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAdd_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 7560 "isa_thumb.tcc"
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
#line 7573 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7576 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7580 "isa_thumb.tcc"
OpAdd_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7584 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7588 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7591 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 7595 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 7598 "isa_thumb.tcc"
)
{
#line 116 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "add r" << rd << ", #" << dec << imm;
	}
#line 7605 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7610 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7613 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7617 "isa_thumb.tcc"
OpAdd_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7621 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7625 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7628 "isa_thumb.tcc"
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
#line 7650 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7656 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7659 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7664 "isa_thumb.tcc"
> *DecodeOpAdd_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAdd_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 7670 "isa_thumb.tcc"
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
#line 7683 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7686 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7690 "isa_thumb.tcc"
OpAdd_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7694 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7698 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7701 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 7705 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 7708 "isa_thumb.tcc"
)
{
#line 149 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "add r" << rd << ", r" << rn << ", r" << rm;
	}
#line 7715 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7720 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7723 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7727 "isa_thumb.tcc"
OpAdd_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7731 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7735 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7738 "isa_thumb.tcc"
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
#line 7762 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7768 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7771 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7776 "isa_thumb.tcc"
> *DecodeOpAdd_3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAdd_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 7782 "isa_thumb.tcc"
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
#line 7795 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7798 "isa_thumb.tcc"
>

void
OpAdd_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7805 "isa_thumb.tcc"
>::initialize_rm_index()
{
#line 184 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::rm_index = (h2 << 3) + rm;
	}
#line 7814 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7819 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7822 "isa_thumb.tcc"
>

void
OpAdd_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7829 "isa_thumb.tcc"
>::initialize_rd_index()
{
#line 190 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::rd_index = (h1 << 3) + rd;
	}
#line 7838 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7843 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7846 "isa_thumb.tcc"
>

void
OpAdd_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7853 "isa_thumb.tcc"
>::initialize_rm_init()
{
#line 196 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		if((h2 << 3) + rm == CPU<CONFIG>::PC_reg)
		inherited::rm_init = 4;
	}
#line 7863 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7868 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7871 "isa_thumb.tcc"
>

void
OpAdd_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7878 "isa_thumb.tcc"
>::initialize_rd_init()
{
#line 203 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		if((h1 << 3) + rd == CPU<CONFIG>::PC_reg)
		inherited::rd_init = 4;
	}
#line 7888 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7893 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7896 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7900 "isa_thumb.tcc"
OpAdd_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7904 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7908 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7911 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 7915 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 7918 "isa_thumb.tcc"
)
{
#line 210 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		buffer << "add r" << inherited::rd_index << ", r" << inherited::rm_index;
	}
#line 7927 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7932 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7935 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7939 "isa_thumb.tcc"
OpAdd_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7943 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7947 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7950 "isa_thumb.tcc"
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
#line 7966 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7972 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7975 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7980 "isa_thumb.tcc"
> *DecodeOpAdd_4(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAdd_4<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 7986 "isa_thumb.tcc"
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
#line 7999 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8002 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8006 "isa_thumb.tcc"
OpAdd_5<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8010 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8014 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8017 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8021 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8024 "isa_thumb.tcc"
)
{
#line 239 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "add r" << rd << ", PC, #" << dec << ((unsigned int)imm * 4);
	}
#line 8031 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8036 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8039 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8043 "isa_thumb.tcc"
OpAdd_5<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8047 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8051 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8054 "isa_thumb.tcc"
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
#line 8071 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8077 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8080 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8085 "isa_thumb.tcc"
> *DecodeOpAdd_5(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAdd_5<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 8091 "isa_thumb.tcc"
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
#line 8104 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8107 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8111 "isa_thumb.tcc"
OpAdd_6<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8115 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8119 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8122 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8126 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8129 "isa_thumb.tcc"
)
{
#line 267 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "add r" << rd << ", SP, #" << dec << ((unsigned int)imm * 4);
	}
#line 8136 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8141 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8144 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8148 "isa_thumb.tcc"
OpAdd_6<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8152 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8156 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8159 "isa_thumb.tcc"
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
#line 8174 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8180 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8183 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8188 "isa_thumb.tcc"
> *DecodeOpAdd_6(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAdd_6<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 8194 "isa_thumb.tcc"
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
#line 8207 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8210 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8214 "isa_thumb.tcc"
OpAdd_7<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8218 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8222 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8225 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8229 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8232 "isa_thumb.tcc"
)
{
#line 293 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "add SP, #" << (unsigned int)imm * 4;
	}
#line 8239 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8244 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8247 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8251 "isa_thumb.tcc"
OpAdd_7<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8255 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8259 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8262 "isa_thumb.tcc"
)
{
#line 297 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		cpu.SetGPR(cpu.SP_reg, cpu.GetGPR(cpu.SP_reg) + ((typename CONFIG::reg_t)imm * 4));
	}
#line 8271 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8277 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8280 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8285 "isa_thumb.tcc"
> *DecodeOpAdd_7(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAdd_7<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 8291 "isa_thumb.tcc"
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
#line 8304 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8307 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8311 "isa_thumb.tcc"
OpAnd<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8315 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8319 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8322 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8326 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8329 "isa_thumb.tcc"
)
{
#line 313 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "and r" << rd << ", r" << rm;
	}
#line 8336 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8341 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8344 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8348 "isa_thumb.tcc"
OpAnd<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8352 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8356 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8359 "isa_thumb.tcc"
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
#line 8374 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8380 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8383 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8388 "isa_thumb.tcc"
> *DecodeOpAnd(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAnd<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 8394 "isa_thumb.tcc"
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
#line 8407 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8410 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8414 "isa_thumb.tcc"
OpAsr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8418 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8422 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8425 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8429 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8432 "isa_thumb.tcc"
)
{
#line 339 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "asr r" << rd << ", r" << rm << ", #" << dec << imm;
	}
#line 8439 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8444 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8447 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8451 "isa_thumb.tcc"
OpAsr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8455 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8459 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8462 "isa_thumb.tcc"
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
#line 8490 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8496 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8499 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8504 "isa_thumb.tcc"
> *DecodeOpAsr_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAsr_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 8510 "isa_thumb.tcc"
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
#line 8523 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8526 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8530 "isa_thumb.tcc"
OpAsr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8534 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8538 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8541 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8545 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8548 "isa_thumb.tcc"
)
{
#line 378 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "asr r" << rd << ", r" << rs;
	}
#line 8555 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8560 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8563 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8567 "isa_thumb.tcc"
OpAsr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8571 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8575 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8578 "isa_thumb.tcc"
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
#line 8610 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8616 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8619 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8624 "isa_thumb.tcc"
> *DecodeOpAsr_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAsr_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 8630 "isa_thumb.tcc"
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
#line 8643 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8646 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8650 "isa_thumb.tcc"
OpBic<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8654 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8658 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8661 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8665 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8668 "isa_thumb.tcc"
)
{
#line 421 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "bic r" << rd << ", r" << rm;
	}
#line 8675 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8680 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8683 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8687 "isa_thumb.tcc"
OpBic<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8691 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8695 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8698 "isa_thumb.tcc"
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
#line 8714 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8720 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8723 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8728 "isa_thumb.tcc"
> *DecodeOpBic(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBic<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 8734 "isa_thumb.tcc"
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
#line 8747 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8750 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8754 "isa_thumb.tcc"
OpCmn<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8758 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8762 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8765 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8769 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8772 "isa_thumb.tcc"
)
{
#line 448 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "cmn r" << rn << ", r" << rm;
	}
#line 8779 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8784 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8787 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8791 "isa_thumb.tcc"
OpCmn<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8795 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8799 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8802 "isa_thumb.tcc"
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
#line 8823 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8829 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8832 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8837 "isa_thumb.tcc"
> *DecodeOpCmn(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCmn<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 8843 "isa_thumb.tcc"
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
#line 8856 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8859 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8863 "isa_thumb.tcc"
OpCmp_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8867 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8871 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8874 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8878 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8881 "isa_thumb.tcc"
)
{
#line 479 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "cmp r" << rn << ", #" << dec << imm;
	}
#line 8888 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8893 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8896 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8900 "isa_thumb.tcc"
OpCmp_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8904 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8908 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8911 "isa_thumb.tcc"
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
#line 8929 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8935 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8938 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8943 "isa_thumb.tcc"
> *DecodeOpCmp_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCmp_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 8949 "isa_thumb.tcc"
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
#line 8962 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8965 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8969 "isa_thumb.tcc"
OpCmp_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8973 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8977 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8980 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8984 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8987 "isa_thumb.tcc"
)
{
#line 507 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "cmp r" << rn << ", r" << rm;
	}
#line 8994 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8999 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9002 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9006 "isa_thumb.tcc"
OpCmp_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9010 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9014 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9017 "isa_thumb.tcc"
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
#line 9035 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9041 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9044 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9049 "isa_thumb.tcc"
> *DecodeOpCmp_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCmp_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 9055 "isa_thumb.tcc"
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
#line 9068 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9071 "isa_thumb.tcc"
>

void
OpCmp_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9078 "isa_thumb.tcc"
>::initialize_rm_index()
{
#line 536 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::rm_index = (h2 << 3) + rm;
	}
#line 9087 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9092 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9095 "isa_thumb.tcc"
>

void
OpCmp_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9102 "isa_thumb.tcc"
>::initialize_rn_index()
{
#line 542 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::rn_index = (h1 << 3) + rn;
	}
#line 9111 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9116 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9119 "isa_thumb.tcc"
>

void
OpCmp_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9126 "isa_thumb.tcc"
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
#line 9138 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9143 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9146 "isa_thumb.tcc"
>

void
OpCmp_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9153 "isa_thumb.tcc"
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
#line 9165 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9170 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9173 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9177 "isa_thumb.tcc"
OpCmp_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9181 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9185 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9188 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 9192 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 9195 "isa_thumb.tcc"
)
{
#line 566 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		buffer << "cmp r" << inherited::rn_index << ", r" << inherited::rm_index;
	}
#line 9204 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9209 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9212 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9216 "isa_thumb.tcc"
OpCmp_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9220 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9224 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9227 "isa_thumb.tcc"
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
#line 9246 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9252 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9255 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9260 "isa_thumb.tcc"
> *DecodeOpCmp_3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCmp_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 9266 "isa_thumb.tcc"
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
#line 9279 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9282 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9286 "isa_thumb.tcc"
OpEor<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9290 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9294 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9297 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 9301 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 9304 "isa_thumb.tcc"
)
{
#line 598 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "eor r" << rd << ", r" << rm;
	}
#line 9311 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9316 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9319 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9323 "isa_thumb.tcc"
OpEor<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9327 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9331 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9334 "isa_thumb.tcc"
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
#line 9350 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9356 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9359 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9364 "isa_thumb.tcc"
> *DecodeOpEor(CodeType code, typename CONFIG::address_t addr)
{
	return new OpEor<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 9370 "isa_thumb.tcc"
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
#line 9383 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9386 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9390 "isa_thumb.tcc"
OpLsl_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9394 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9398 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9401 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 9405 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 9408 "isa_thumb.tcc"
)
{
#line 624 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "lsl r" << rd << ", r" << rm << ", #" << dec << imm;
	}
#line 9415 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9420 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9423 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9427 "isa_thumb.tcc"
OpLsl_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9431 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9435 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9438 "isa_thumb.tcc"
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
#line 9458 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9464 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9467 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9472 "isa_thumb.tcc"
> *DecodeOpLsl_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLsl_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 9478 "isa_thumb.tcc"
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
#line 9491 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9494 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9498 "isa_thumb.tcc"
OpLsl_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9502 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9506 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9509 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 9513 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 9516 "isa_thumb.tcc"
)
{
#line 655 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "lsl r" << rd << ", r" << rs;
	}
#line 9523 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9528 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9531 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9535 "isa_thumb.tcc"
OpLsl_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9539 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9543 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9546 "isa_thumb.tcc"
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
#line 9572 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9578 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9581 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9586 "isa_thumb.tcc"
> *DecodeOpLsl_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLsl_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 9592 "isa_thumb.tcc"
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
#line 9605 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9608 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9612 "isa_thumb.tcc"
OpLsr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9616 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9620 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9623 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 9627 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 9630 "isa_thumb.tcc"
)
{
#line 692 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "lsr r" << rd << ", r" << rm << ", #" << dec << imm;
	}
#line 9637 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9642 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9645 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9649 "isa_thumb.tcc"
OpLsr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9653 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9657 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9660 "isa_thumb.tcc"
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
#line 9681 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9687 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9690 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9695 "isa_thumb.tcc"
> *DecodeOpLsr_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLsr_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 9701 "isa_thumb.tcc"
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
#line 9714 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9717 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9721 "isa_thumb.tcc"
OpLsr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9725 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9729 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9732 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 9736 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 9739 "isa_thumb.tcc"
)
{
#line 724 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "lsr r" << rd << ", r" << rs;
	}
#line 9746 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9751 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9754 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9758 "isa_thumb.tcc"
OpLsr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9762 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9766 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9769 "isa_thumb.tcc"
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
#line 9797 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9803 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9806 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9811 "isa_thumb.tcc"
> *DecodeOpLsr_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLsr_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 9817 "isa_thumb.tcc"
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
#line 9830 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9833 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9837 "isa_thumb.tcc"
OpMov_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9841 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9845 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9848 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 9852 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 9855 "isa_thumb.tcc"
)
{
#line 763 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "mov r" << rd << ", #" << dec << imm;
	}
#line 9862 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9867 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9870 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9874 "isa_thumb.tcc"
OpMov_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9878 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9882 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9885 "isa_thumb.tcc"
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
#line 9900 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9906 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9909 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9914 "isa_thumb.tcc"
> *DecodeOpMov_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMov_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 9920 "isa_thumb.tcc"
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
#line 9942 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9945 "isa_thumb.tcc"
>

void
OpMov_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9952 "isa_thumb.tcc"
>::initialize_rm_index()
{
#line 799 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::rm_index = (h2 << 3) + rm;
	}
#line 9961 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9966 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9969 "isa_thumb.tcc"
>

void
OpMov_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9976 "isa_thumb.tcc"
>::initialize_rd_index()
{
#line 805 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::rd_index = (h1 << 3) + rd;
	}
#line 9985 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9990 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9993 "isa_thumb.tcc"
>

void
OpMov_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10000 "isa_thumb.tcc"
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
#line 10012 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10017 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10020 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10024 "isa_thumb.tcc"
OpMov_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10028 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10032 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10035 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10039 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10042 "isa_thumb.tcc"
)
{
#line 820 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		buffer << "mov r" << inherited::rd_index << ", r" << inherited::rm_index;
	}
#line 10051 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10056 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10059 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10063 "isa_thumb.tcc"
OpMov_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10067 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10071 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10074 "isa_thumb.tcc"
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
#line 10088 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10094 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10097 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10102 "isa_thumb.tcc"
> *DecodeOpMov_3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMov_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 10108 "isa_thumb.tcc"
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
#line 10121 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10124 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10128 "isa_thumb.tcc"
OpMul<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10132 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10136 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10139 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10143 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10146 "isa_thumb.tcc"
)
{
#line 846 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "mul r" << rd << ", r" << rm;
	}
#line 10153 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10158 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10161 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10165 "isa_thumb.tcc"
OpMul<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10169 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10173 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10176 "isa_thumb.tcc"
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
#line 10190 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10196 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10199 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10204 "isa_thumb.tcc"
> *DecodeOpMul(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMul<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 10210 "isa_thumb.tcc"
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
#line 10223 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10226 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10230 "isa_thumb.tcc"
OpMvn<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10234 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10238 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10241 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10245 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10248 "isa_thumb.tcc"
)
{
#line 871 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "mvn r" << rd << ", r" << rm;
	}
#line 10255 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10260 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10263 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10267 "isa_thumb.tcc"
OpMvn<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10271 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10275 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10278 "isa_thumb.tcc"
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
#line 10292 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10298 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10301 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10306 "isa_thumb.tcc"
> *DecodeOpMvn(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMvn<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 10312 "isa_thumb.tcc"
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
#line 10325 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10328 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10332 "isa_thumb.tcc"
OpNeg<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10336 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10340 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10343 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10347 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10350 "isa_thumb.tcc"
)
{
#line 896 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "neg r" << rd << ", r" << rm;
	}
#line 10357 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10362 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10365 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10369 "isa_thumb.tcc"
OpNeg<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10373 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10377 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10380 "isa_thumb.tcc"
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
#line 10398 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10404 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10407 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10412 "isa_thumb.tcc"
> *DecodeOpNeg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNeg<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 10418 "isa_thumb.tcc"
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
#line 10431 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10434 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10438 "isa_thumb.tcc"
OpOrr<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10442 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10446 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10449 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10453 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10456 "isa_thumb.tcc"
)
{
#line 925 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "orr r" << rd << ", r" << rm;
	}
#line 10463 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10468 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10471 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10475 "isa_thumb.tcc"
OpOrr<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10479 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10483 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10486 "isa_thumb.tcc"
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
#line 10502 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10508 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10511 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10516 "isa_thumb.tcc"
> *DecodeOpOrr(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOrr<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 10522 "isa_thumb.tcc"
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
#line 10535 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10538 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10542 "isa_thumb.tcc"
OpRor<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10546 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10550 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10553 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10557 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10560 "isa_thumb.tcc"
)
{
#line 952 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "ror r" << rd << ", r" << rs;
	}
#line 10567 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10572 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10575 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10579 "isa_thumb.tcc"
OpRor<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10583 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10587 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10590 "isa_thumb.tcc"
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
#line 10615 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10621 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10624 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10629 "isa_thumb.tcc"
> *DecodeOpRor(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRor<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 10635 "isa_thumb.tcc"
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
#line 10648 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10651 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10655 "isa_thumb.tcc"
OpSbc<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10659 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10663 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10666 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10670 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10673 "isa_thumb.tcc"
)
{
#line 988 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "sbc r" << rd << ", r" << rm;
	}
#line 10680 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10685 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10688 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10692 "isa_thumb.tcc"
OpSbc<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10696 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10700 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10703 "isa_thumb.tcc"
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
#line 10725 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10731 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10734 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10739 "isa_thumb.tcc"
> *DecodeOpSbc(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSbc<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 10745 "isa_thumb.tcc"
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
#line 10758 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10761 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10765 "isa_thumb.tcc"
OpSub_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10769 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10773 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10776 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10780 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10783 "isa_thumb.tcc"
)
{
#line 1021 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "sub r" << rd << ", r" << rn << ", #" << dec << imm;
	}
#line 10790 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10795 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10798 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10802 "isa_thumb.tcc"
OpSub_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10806 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10810 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10813 "isa_thumb.tcc"
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
#line 10833 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10839 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10842 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10847 "isa_thumb.tcc"
> *DecodeOpSub_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSub_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 10853 "isa_thumb.tcc"
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
#line 10866 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10869 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10873 "isa_thumb.tcc"
OpSub_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10877 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10881 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10884 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10888 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10891 "isa_thumb.tcc"
)
{
#line 1052 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "sub r" << rd << ", #" << dec << imm;
	}
#line 10898 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10903 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10906 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10910 "isa_thumb.tcc"
OpSub_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10914 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10918 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10921 "isa_thumb.tcc"
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
#line 10943 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10949 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10952 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10957 "isa_thumb.tcc"
> *DecodeOpSub_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSub_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 10963 "isa_thumb.tcc"
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
#line 10976 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10979 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10983 "isa_thumb.tcc"
OpSub_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10987 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10991 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10994 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10998 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11001 "isa_thumb.tcc"
)
{
#line 1085 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "sub r" << rd << ", r" << rn << ", r" << rm;
	}
#line 11008 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11013 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11016 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11020 "isa_thumb.tcc"
OpSub_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11024 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11028 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11031 "isa_thumb.tcc"
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
#line 11051 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11057 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11060 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11065 "isa_thumb.tcc"
> *DecodeOpSub_3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSub_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 11071 "isa_thumb.tcc"
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
#line 11084 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11087 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11091 "isa_thumb.tcc"
OpSub_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11095 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11099 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11102 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11106 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11109 "isa_thumb.tcc"
)
{
#line 1116 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "sub sp, #" << (imm * 4);
	}
#line 11116 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11121 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11124 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11128 "isa_thumb.tcc"
OpSub_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11132 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11136 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11139 "isa_thumb.tcc"
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
#line 11154 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11160 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11163 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11168 "isa_thumb.tcc"
> *DecodeOpSub_4(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSub_4<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 11174 "isa_thumb.tcc"
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
#line 11187 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11190 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11194 "isa_thumb.tcc"
OpTst<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11198 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11202 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11205 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11209 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11212 "isa_thumb.tcc"
)
{
#line 1142 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "tst r" << rn << ", r" << rm;
	}
#line 11219 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11224 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11227 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11231 "isa_thumb.tcc"
OpTst<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11235 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11239 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11242 "isa_thumb.tcc"
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
#line 11258 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11264 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11267 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11272 "isa_thumb.tcc"
> *DecodeOpTst(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTst<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 11278 "isa_thumb.tcc"
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
#line 11329 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11332 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11336 "isa_thumb.tcc"
OpLdr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11340 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11344 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11347 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11351 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11354 "isa_thumb.tcc"
)
{
#line 47 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldr r" << rd << ", [r" << rn << ", #" << dec << (imm * 4) << "]";
	}
#line 11361 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11366 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11369 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11373 "isa_thumb.tcc"
OpLdr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11377 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11381 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11384 "isa_thumb.tcc"
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
#line 11399 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11405 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11408 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11413 "isa_thumb.tcc"
> *DecodeOpLdr_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdr_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 11419 "isa_thumb.tcc"
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
#line 11432 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11435 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11439 "isa_thumb.tcc"
OpLdr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11443 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11447 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11450 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11454 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11457 "isa_thumb.tcc"
)
{
#line 73 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldr r" << rd << ", [r" << rn << ", r" << rm << "]";
	}
#line 11464 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11469 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11472 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11476 "isa_thumb.tcc"
OpLdr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11480 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11484 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11487 "isa_thumb.tcc"
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
#line 11503 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11509 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11512 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11517 "isa_thumb.tcc"
> *DecodeOpLdr_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdr_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 11523 "isa_thumb.tcc"
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
#line 11536 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11539 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11543 "isa_thumb.tcc"
OpLdr_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11547 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11551 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11554 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11558 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11561 "isa_thumb.tcc"
)
{
#line 100 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldr r" << rd << ", [pc, #" << dec << (imm * 4) << "]";
	}
#line 11568 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11573 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11576 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11580 "isa_thumb.tcc"
OpLdr_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11584 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11588 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11591 "isa_thumb.tcc"
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
#line 11607 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11613 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11616 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11621 "isa_thumb.tcc"
> *DecodeOpLdr_3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdr_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 11627 "isa_thumb.tcc"
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
#line 11640 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11643 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11647 "isa_thumb.tcc"
OpLdr_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11651 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11655 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11658 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11662 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11665 "isa_thumb.tcc"
)
{
#line 127 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldr r" << rd << ", [sp, #" << dec << (imm * 4) << "]";
	}
#line 11672 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11677 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11680 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11684 "isa_thumb.tcc"
OpLdr_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11688 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11692 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11695 "isa_thumb.tcc"
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
#line 11710 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11716 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11719 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11724 "isa_thumb.tcc"
> *DecodeOpLdr_4(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdr_4<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 11730 "isa_thumb.tcc"
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
#line 11743 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11746 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11750 "isa_thumb.tcc"
OpLdrb_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11754 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11758 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11761 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11765 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11768 "isa_thumb.tcc"
)
{
#line 153 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldrb r" << rd << ", [r" << rn << ", #" << dec << imm << "]";
	}
#line 11775 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11780 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11783 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11787 "isa_thumb.tcc"
OpLdrb_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11791 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11795 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11798 "isa_thumb.tcc"
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
#line 11814 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11820 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11823 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11828 "isa_thumb.tcc"
> *DecodeOpLdrb_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdrb_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 11834 "isa_thumb.tcc"
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
#line 11847 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11850 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11854 "isa_thumb.tcc"
OpLdrb_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11858 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11862 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11865 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11869 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11872 "isa_thumb.tcc"
)
{
#line 180 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldrb r" << rd << ", [r" << rn << ", r" << rm << "]";
	}
#line 11879 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11884 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11887 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11891 "isa_thumb.tcc"
OpLdrb_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11895 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11899 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11902 "isa_thumb.tcc"
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
#line 11919 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11925 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11928 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11933 "isa_thumb.tcc"
> *DecodeOpLdrb_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdrb_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 11939 "isa_thumb.tcc"
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
#line 11952 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11955 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11959 "isa_thumb.tcc"
OpLdrh_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11963 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11967 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11970 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11974 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11977 "isa_thumb.tcc"
)
{
#line 208 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldrh r" << rd << ", [r" << rn << ", #" << dec << (imm * 2) << "]";
	}
#line 11984 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11989 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11992 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11996 "isa_thumb.tcc"
OpLdrh_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12000 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12004 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12007 "isa_thumb.tcc"
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
#line 12023 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12029 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12032 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12037 "isa_thumb.tcc"
> *DecodeOpLdrh_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdrh_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 12043 "isa_thumb.tcc"
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
#line 12056 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12059 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12063 "isa_thumb.tcc"
OpLdrh_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12067 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12071 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12074 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12078 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12081 "isa_thumb.tcc"
)
{
#line 235 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldrh r" << rd << ", [r" << rn << ", r" << rm << "]";
	}
#line 12088 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12093 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12096 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12100 "isa_thumb.tcc"
OpLdrh_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12104 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12108 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12111 "isa_thumb.tcc"
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
#line 12128 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12134 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12137 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12142 "isa_thumb.tcc"
> *DecodeOpLdrh_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdrh_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 12148 "isa_thumb.tcc"
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
#line 12161 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12164 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12168 "isa_thumb.tcc"
OpLdrsb<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12172 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12176 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12179 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12183 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12186 "isa_thumb.tcc"
)
{
#line 263 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldrsb r" << rd << ", [r" << rn << ", r" << rm << "]";
	}
#line 12193 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12198 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12201 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12205 "isa_thumb.tcc"
OpLdrsb<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12209 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12213 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12216 "isa_thumb.tcc"
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
#line 12233 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12239 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12242 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12247 "isa_thumb.tcc"
> *DecodeOpLdrsb(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdrsb<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 12253 "isa_thumb.tcc"
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
#line 12266 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12269 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12273 "isa_thumb.tcc"
OpLdrsh<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12277 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12281 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12284 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12288 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12291 "isa_thumb.tcc"
)
{
#line 291 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldrsh r" << rd << ", [r" << rn << ", r" << rm << "]";
	}
#line 12298 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12303 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12306 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12310 "isa_thumb.tcc"
OpLdrsh<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12314 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12318 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12321 "isa_thumb.tcc"
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
#line 12338 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12344 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12347 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12352 "isa_thumb.tcc"
> *DecodeOpLdrsh(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdrsh<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 12358 "isa_thumb.tcc"
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
#line 12371 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12374 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12378 "isa_thumb.tcc"
OpStr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12382 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12386 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12389 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12393 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12396 "isa_thumb.tcc"
)
{
#line 319 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "str r" << rd << ", [r" << rn << ", #" << (imm * 4) << "]";
	}
#line 12403 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12408 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12411 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12415 "isa_thumb.tcc"
OpStr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12419 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12423 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12426 "isa_thumb.tcc"
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
#line 12441 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12447 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12450 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12455 "isa_thumb.tcc"
> *DecodeOpStr_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpStr_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 12461 "isa_thumb.tcc"
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
#line 12474 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12477 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12481 "isa_thumb.tcc"
OpStr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12485 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12489 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12492 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12496 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12499 "isa_thumb.tcc"
)
{
#line 345 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "str r" << rd << ", [r" << rn << ", r" << rm << "]";
	}
#line 12506 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12511 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12514 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12518 "isa_thumb.tcc"
OpStr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12522 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12526 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12529 "isa_thumb.tcc"
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
#line 12545 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12551 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12554 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12559 "isa_thumb.tcc"
> *DecodeOpStr_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpStr_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 12565 "isa_thumb.tcc"
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
#line 12578 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12581 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12585 "isa_thumb.tcc"
OpStr_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12589 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12593 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12596 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12600 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12603 "isa_thumb.tcc"
)
{
#line 372 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "str r" << rd << ", [sp, #" << (imm * 4) << "]";
	}
#line 12610 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12615 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12618 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12622 "isa_thumb.tcc"
OpStr_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12626 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12630 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12633 "isa_thumb.tcc"
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
#line 12648 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12654 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12657 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12662 "isa_thumb.tcc"
> *DecodeOpStr_3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpStr_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 12668 "isa_thumb.tcc"
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
#line 12681 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12684 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12688 "isa_thumb.tcc"
OpStrb_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12692 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12696 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12699 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12703 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12706 "isa_thumb.tcc"
)
{
#line 398 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "strb r" << rd << ", [r" << rn << ", #" << dec << imm << "]";
	}
#line 12713 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12718 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12721 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12725 "isa_thumb.tcc"
OpStrb_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12729 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12733 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12736 "isa_thumb.tcc"
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
#line 12753 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12759 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12762 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12767 "isa_thumb.tcc"
> *DecodeOpStrb_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpStrb_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 12773 "isa_thumb.tcc"
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
#line 12786 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12789 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12793 "isa_thumb.tcc"
OpStrb_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12797 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12801 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12804 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12808 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12811 "isa_thumb.tcc"
)
{
#line 426 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "strb r" << rd << ", [r" << rn << ", r" << rm << "]";
	}
#line 12818 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12823 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12826 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12830 "isa_thumb.tcc"
OpStrb_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12834 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12838 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12841 "isa_thumb.tcc"
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
#line 12859 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12865 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12868 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12873 "isa_thumb.tcc"
> *DecodeOpStrb_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpStrb_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 12879 "isa_thumb.tcc"
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
#line 12892 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12895 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12899 "isa_thumb.tcc"
OpStrh_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12903 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12907 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12910 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12914 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12917 "isa_thumb.tcc"
)
{
#line 455 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "strh r" << rd << ", [r" << rn << ", #" << dec << (imm * 2) << "]";
	}
#line 12924 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12929 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12932 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12936 "isa_thumb.tcc"
OpStrh_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12940 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12944 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12947 "isa_thumb.tcc"
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
#line 12964 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12970 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12973 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12978 "isa_thumb.tcc"
> *DecodeOpStrh_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpStrh_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 12984 "isa_thumb.tcc"
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
#line 12997 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13000 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13004 "isa_thumb.tcc"
OpStrh_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13008 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13012 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13015 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 13019 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 13022 "isa_thumb.tcc"
)
{
#line 483 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "strh r" << rd << ", [r" << rn << ", r" << rm << "]";
	}
#line 13029 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13034 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13037 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13041 "isa_thumb.tcc"
OpStrh_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13045 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13049 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13052 "isa_thumb.tcc"
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
#line 13070 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13076 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13079 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13084 "isa_thumb.tcc"
> *DecodeOpStrh_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpStrh_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 13090 "isa_thumb.tcc"
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
#line 13141 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13144 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13148 "isa_thumb.tcc"
OpLdmia<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13152 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13156 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13159 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 13163 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 13166 "isa_thumb.tcc"
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
#line 13185 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13190 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13193 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13197 "isa_thumb.tcc"
OpLdmia<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13201 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13205 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13208 "isa_thumb.tcc"
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
#line 13238 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13244 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13247 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13252 "isa_thumb.tcc"
> *DecodeOpLdmia(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdmia<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 13258 "isa_thumb.tcc"
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
#line 13271 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13274 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13278 "isa_thumb.tcc"
OpPop<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13282 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13286 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13289 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 13293 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 13296 "isa_thumb.tcc"
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
#line 13315 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13320 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13323 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13327 "isa_thumb.tcc"
OpPop<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13331 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13335 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13338 "isa_thumb.tcc"
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
#line 13374 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13380 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13383 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13388 "isa_thumb.tcc"
> *DecodeOpPop(CodeType code, typename CONFIG::address_t addr)
{
	return new OpPop<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 13394 "isa_thumb.tcc"
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
#line 13407 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13410 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13414 "isa_thumb.tcc"
OpPush<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13418 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13422 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13425 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 13429 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 13432 "isa_thumb.tcc"
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
#line 13455 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13460 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13463 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13467 "isa_thumb.tcc"
OpPush<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13471 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13475 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13478 "isa_thumb.tcc"
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
#line 13517 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13523 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13526 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13531 "isa_thumb.tcc"
> *DecodeOpPush(CodeType code, typename CONFIG::address_t addr)
{
	return new OpPush<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 13537 "isa_thumb.tcc"
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
#line 13550 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13553 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13557 "isa_thumb.tcc"
OpStmia<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13561 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13565 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13568 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 13572 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 13575 "isa_thumb.tcc"
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
#line 13594 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13599 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13602 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13606 "isa_thumb.tcc"
OpStmia<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13610 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13614 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13617 "isa_thumb.tcc"
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
#line 13648 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13654 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13657 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13662 "isa_thumb.tcc"
> *DecodeOpStmia(CodeType code, typename CONFIG::address_t addr)
{
	return new OpStmia<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 13668 "isa_thumb.tcc"
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
#line 13719 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13722 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13726 "isa_thumb.tcc"
OpBkpt<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13730 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13734 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13737 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 13741 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 13744 "isa_thumb.tcc"
)
{
#line 47 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/exception.isa"
	{
		buffer << "bkpt #" << dec << imm;
	}
#line 13751 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13756 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13759 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13763 "isa_thumb.tcc"
OpBkpt<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13767 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13771 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13774 "isa_thumb.tcc"
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
#line 13798 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13804 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13807 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13812 "isa_thumb.tcc"
> *DecodeOpBkpt(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBkpt<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 13818 "isa_thumb.tcc"
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
#line 13831 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13834 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13838 "isa_thumb.tcc"
OpSwi<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13842 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13846 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13849 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 13853 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 13856 "isa_thumb.tcc"
)
{
#line 82 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/exception.isa"
	{
		buffer << "swi #" << dec << imm;
	}
#line 13863 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13868 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13871 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13875 "isa_thumb.tcc"
OpSwi<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13879 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13883 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13886 "isa_thumb.tcc"
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
#line 13915 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13921 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13924 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13929 "isa_thumb.tcc"
> *DecodeOpSwi(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSwi<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 13935 "isa_thumb.tcc"
	>(code, addr);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13942 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13945 "isa_thumb.tcc"
>
OpB_1_0xxx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13950 "isa_thumb.tcc"
>::OpB_1_0xxx(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13954 "isa_thumb.tcc"
>(code, addr, "b_1_0xxx")
{
	sub_cond = ((code >> 8) & 0x7);
	imm = (((((int32_t)(code >> 0)) & 0xff) << 24) >> 24);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13964 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13967 "isa_thumb.tcc"
>
OpB_1_10xx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13972 "isa_thumb.tcc"
>::OpB_1_10xx(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13976 "isa_thumb.tcc"
>(code, addr, "b_1_10xx")
{
	sub_cond = ((code >> 8) & 0x3);
	imm = (((((int32_t)(code >> 0)) & 0xff) << 24) >> 24);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13986 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13989 "isa_thumb.tcc"
>
OpB_1_110x<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13994 "isa_thumb.tcc"
>::OpB_1_110x(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13998 "isa_thumb.tcc"
>(code, addr, "b_1_110x")
{
	sub_cond = ((code >> 8) & 0x1);
	imm = (((((int32_t)(code >> 0)) & 0xff) << 24) >> 24);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14008 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14011 "isa_thumb.tcc"
>
OpB_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14016 "isa_thumb.tcc"
>::OpB_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14020 "isa_thumb.tcc"
>(code, addr, "b_2")
{
	imm = (((((int32_t)(code >> 0)) & 0x7ff) << 21) >> 21);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14029 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14032 "isa_thumb.tcc"
>
OpBl<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14037 "isa_thumb.tcc"
>::OpBl(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14041 "isa_thumb.tcc"
>(code, addr, "bl")
{
	h2 = ((code >> 11) & 0x1);
	offset = ((code >> 0) & 0x7ff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14051 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14054 "isa_thumb.tcc"
>
OpBx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14059 "isa_thumb.tcc"
>::OpBx(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14063 "isa_thumb.tcc"
>(code, addr, "bx")
{
	h2 = ((code >> 6) & 0x1);
	rm = ((code >> 3) & 0x7);
	sbz = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14074 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14077 "isa_thumb.tcc"
>
OpBlx_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14082 "isa_thumb.tcc"
>::OpBlx_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14086 "isa_thumb.tcc"
>(code, addr, "blx_1")
{
	offset = ((code >> 0) & 0x7ff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14095 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14098 "isa_thumb.tcc"
>
OpBlx_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14103 "isa_thumb.tcc"
>::OpBlx_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14107 "isa_thumb.tcc"
>(code, addr, "blx_2")
{
	h2 = ((code >> 6) & 0x1);
	rm = ((code >> 3) & 0x7);
	sbz = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14118 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14121 "isa_thumb.tcc"
>
OpAdc<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14126 "isa_thumb.tcc"
>::OpAdc(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14130 "isa_thumb.tcc"
>(code, addr, "adc")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14140 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14143 "isa_thumb.tcc"
>
OpAdd_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14148 "isa_thumb.tcc"
>::OpAdd_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14152 "isa_thumb.tcc"
>(code, addr, "add_1")
{
	imm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14163 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14166 "isa_thumb.tcc"
>
OpAdd_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14171 "isa_thumb.tcc"
>::OpAdd_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14175 "isa_thumb.tcc"
>(code, addr, "add_2")
{
	rd = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14185 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14188 "isa_thumb.tcc"
>
OpAdd_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14193 "isa_thumb.tcc"
>::OpAdd_3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14197 "isa_thumb.tcc"
>(code, addr, "add_3")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14208 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14211 "isa_thumb.tcc"
>
OpAdd_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14216 "isa_thumb.tcc"
>::OpAdd_4(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14220 "isa_thumb.tcc"
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
#line 14232 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14235 "isa_thumb.tcc"
>
OpAdd_5<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14240 "isa_thumb.tcc"
>::OpAdd_5(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14244 "isa_thumb.tcc"
>(code, addr, "add_5")
{
	rd = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14254 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14257 "isa_thumb.tcc"
>
OpAdd_6<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14262 "isa_thumb.tcc"
>::OpAdd_6(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14266 "isa_thumb.tcc"
>(code, addr, "add_6")
{
	rd = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14276 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14279 "isa_thumb.tcc"
>
OpAdd_7<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14284 "isa_thumb.tcc"
>::OpAdd_7(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14288 "isa_thumb.tcc"
>(code, addr, "add_7")
{
	imm = ((code >> 0) & 0x7f);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14297 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14300 "isa_thumb.tcc"
>
OpAnd<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14305 "isa_thumb.tcc"
>::OpAnd(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14309 "isa_thumb.tcc"
>(code, addr, "and")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14319 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14322 "isa_thumb.tcc"
>
OpAsr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14327 "isa_thumb.tcc"
>::OpAsr_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14331 "isa_thumb.tcc"
>(code, addr, "asr_1")
{
	imm = ((code >> 6) & 0x1f);
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14342 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14345 "isa_thumb.tcc"
>
OpAsr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14350 "isa_thumb.tcc"
>::OpAsr_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14354 "isa_thumb.tcc"
>(code, addr, "asr_2")
{
	rs = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14364 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14367 "isa_thumb.tcc"
>
OpBic<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14372 "isa_thumb.tcc"
>::OpBic(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14376 "isa_thumb.tcc"
>(code, addr, "bic")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14386 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14389 "isa_thumb.tcc"
>
OpCmn<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14394 "isa_thumb.tcc"
>::OpCmn(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14398 "isa_thumb.tcc"
>(code, addr, "cmn")
{
	rm = ((code >> 3) & 0x7);
	rn = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14408 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14411 "isa_thumb.tcc"
>
OpCmp_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14416 "isa_thumb.tcc"
>::OpCmp_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14420 "isa_thumb.tcc"
>(code, addr, "cmp_1")
{
	rn = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14430 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14433 "isa_thumb.tcc"
>
OpCmp_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14438 "isa_thumb.tcc"
>::OpCmp_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14442 "isa_thumb.tcc"
>(code, addr, "cmp_2")
{
	rm = ((code >> 3) & 0x7);
	rn = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14452 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14455 "isa_thumb.tcc"
>
OpCmp_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14460 "isa_thumb.tcc"
>::OpCmp_3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14464 "isa_thumb.tcc"
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
#line 14476 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14479 "isa_thumb.tcc"
>
OpEor<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14484 "isa_thumb.tcc"
>::OpEor(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14488 "isa_thumb.tcc"
>(code, addr, "eor")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14498 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14501 "isa_thumb.tcc"
>
OpLsl_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14506 "isa_thumb.tcc"
>::OpLsl_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14510 "isa_thumb.tcc"
>(code, addr, "lsl_1")
{
	imm = ((code >> 6) & 0x1f);
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14521 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14524 "isa_thumb.tcc"
>
OpLsl_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14529 "isa_thumb.tcc"
>::OpLsl_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14533 "isa_thumb.tcc"
>(code, addr, "lsl_2")
{
	rs = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14543 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14546 "isa_thumb.tcc"
>
OpLsr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14551 "isa_thumb.tcc"
>::OpLsr_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14555 "isa_thumb.tcc"
>(code, addr, "lsr_1")
{
	imm = ((code >> 6) & 0x1f);
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14566 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14569 "isa_thumb.tcc"
>
OpLsr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14574 "isa_thumb.tcc"
>::OpLsr_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14578 "isa_thumb.tcc"
>(code, addr, "lsr_2")
{
	rs = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14588 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14591 "isa_thumb.tcc"
>
OpMov_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14596 "isa_thumb.tcc"
>::OpMov_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14600 "isa_thumb.tcc"
>(code, addr, "mov_1")
{
	rd = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14610 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14613 "isa_thumb.tcc"
>
OpMov_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14618 "isa_thumb.tcc"
>::OpMov_3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14622 "isa_thumb.tcc"
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
#line 14634 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14637 "isa_thumb.tcc"
>
OpMul<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14642 "isa_thumb.tcc"
>::OpMul(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14646 "isa_thumb.tcc"
>(code, addr, "mul")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14656 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14659 "isa_thumb.tcc"
>
OpMvn<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14664 "isa_thumb.tcc"
>::OpMvn(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14668 "isa_thumb.tcc"
>(code, addr, "mvn")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14678 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14681 "isa_thumb.tcc"
>
OpNeg<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14686 "isa_thumb.tcc"
>::OpNeg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14690 "isa_thumb.tcc"
>(code, addr, "neg")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14700 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14703 "isa_thumb.tcc"
>
OpOrr<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14708 "isa_thumb.tcc"
>::OpOrr(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14712 "isa_thumb.tcc"
>(code, addr, "orr")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14722 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14725 "isa_thumb.tcc"
>
OpRor<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14730 "isa_thumb.tcc"
>::OpRor(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14734 "isa_thumb.tcc"
>(code, addr, "ror")
{
	rs = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14744 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14747 "isa_thumb.tcc"
>
OpSbc<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14752 "isa_thumb.tcc"
>::OpSbc(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14756 "isa_thumb.tcc"
>(code, addr, "sbc")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14766 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14769 "isa_thumb.tcc"
>
OpSub_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14774 "isa_thumb.tcc"
>::OpSub_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14778 "isa_thumb.tcc"
>(code, addr, "sub_1")
{
	imm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14789 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14792 "isa_thumb.tcc"
>
OpSub_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14797 "isa_thumb.tcc"
>::OpSub_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14801 "isa_thumb.tcc"
>(code, addr, "sub_2")
{
	rd = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14811 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14814 "isa_thumb.tcc"
>
OpSub_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14819 "isa_thumb.tcc"
>::OpSub_3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14823 "isa_thumb.tcc"
>(code, addr, "sub_3")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14834 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14837 "isa_thumb.tcc"
>
OpSub_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14842 "isa_thumb.tcc"
>::OpSub_4(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14846 "isa_thumb.tcc"
>(code, addr, "sub_4")
{
	imm = ((code >> 0) & 0x7f);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14855 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14858 "isa_thumb.tcc"
>
OpTst<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14863 "isa_thumb.tcc"
>::OpTst(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14867 "isa_thumb.tcc"
>(code, addr, "tst")
{
	rm = ((code >> 3) & 0x7);
	rn = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14877 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14880 "isa_thumb.tcc"
>
OpLdr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14885 "isa_thumb.tcc"
>::OpLdr_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14889 "isa_thumb.tcc"
>(code, addr, "ldr_1")
{
	imm = ((code >> 6) & 0x1f);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14900 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14903 "isa_thumb.tcc"
>
OpLdr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14908 "isa_thumb.tcc"
>::OpLdr_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14912 "isa_thumb.tcc"
>(code, addr, "ldr_2")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14923 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14926 "isa_thumb.tcc"
>
OpLdr_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14931 "isa_thumb.tcc"
>::OpLdr_3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14935 "isa_thumb.tcc"
>(code, addr, "ldr_3")
{
	rd = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14945 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14948 "isa_thumb.tcc"
>
OpLdr_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14953 "isa_thumb.tcc"
>::OpLdr_4(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14957 "isa_thumb.tcc"
>(code, addr, "ldr_4")
{
	rd = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14967 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14970 "isa_thumb.tcc"
>
OpLdrb_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14975 "isa_thumb.tcc"
>::OpLdrb_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14979 "isa_thumb.tcc"
>(code, addr, "ldrb_1")
{
	imm = ((code >> 6) & 0x1f);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14990 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14993 "isa_thumb.tcc"
>
OpLdrb_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14998 "isa_thumb.tcc"
>::OpLdrb_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15002 "isa_thumb.tcc"
>(code, addr, "ldrb_2")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15013 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15016 "isa_thumb.tcc"
>
OpLdrh_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15021 "isa_thumb.tcc"
>::OpLdrh_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15025 "isa_thumb.tcc"
>(code, addr, "ldrh_1")
{
	imm = ((code >> 6) & 0x1f);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15036 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15039 "isa_thumb.tcc"
>
OpLdrh_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15044 "isa_thumb.tcc"
>::OpLdrh_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15048 "isa_thumb.tcc"
>(code, addr, "ldrh_2")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15059 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15062 "isa_thumb.tcc"
>
OpLdrsb<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15067 "isa_thumb.tcc"
>::OpLdrsb(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15071 "isa_thumb.tcc"
>(code, addr, "ldrsb")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15082 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15085 "isa_thumb.tcc"
>
OpLdrsh<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15090 "isa_thumb.tcc"
>::OpLdrsh(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15094 "isa_thumb.tcc"
>(code, addr, "ldrsh")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15105 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15108 "isa_thumb.tcc"
>
OpStr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15113 "isa_thumb.tcc"
>::OpStr_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15117 "isa_thumb.tcc"
>(code, addr, "str_1")
{
	imm = ((code >> 6) & 0x1f);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15128 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15131 "isa_thumb.tcc"
>
OpStr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15136 "isa_thumb.tcc"
>::OpStr_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15140 "isa_thumb.tcc"
>(code, addr, "str_2")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15151 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15154 "isa_thumb.tcc"
>
OpStr_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15159 "isa_thumb.tcc"
>::OpStr_3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15163 "isa_thumb.tcc"
>(code, addr, "str_3")
{
	rd = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15173 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15176 "isa_thumb.tcc"
>
OpStrb_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15181 "isa_thumb.tcc"
>::OpStrb_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15185 "isa_thumb.tcc"
>(code, addr, "strb_1")
{
	imm = ((code >> 6) & 0x1f);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15196 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15199 "isa_thumb.tcc"
>
OpStrb_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15204 "isa_thumb.tcc"
>::OpStrb_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15208 "isa_thumb.tcc"
>(code, addr, "strb_2")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15219 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15222 "isa_thumb.tcc"
>
OpStrh_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15227 "isa_thumb.tcc"
>::OpStrh_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15231 "isa_thumb.tcc"
>(code, addr, "strh_1")
{
	imm = ((code >> 6) & 0x1f);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15242 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15245 "isa_thumb.tcc"
>
OpStrh_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15250 "isa_thumb.tcc"
>::OpStrh_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15254 "isa_thumb.tcc"
>(code, addr, "strh_2")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15265 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15268 "isa_thumb.tcc"
>
OpLdmia<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15273 "isa_thumb.tcc"
>::OpLdmia(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15277 "isa_thumb.tcc"
>(code, addr, "ldmia")
{
	rn = ((code >> 8) & 0x7);
	reg_list = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15287 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15290 "isa_thumb.tcc"
>
OpPop<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15295 "isa_thumb.tcc"
>::OpPop(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15299 "isa_thumb.tcc"
>(code, addr, "pop")
{
	r = ((code >> 8) & 0x1);
	reg_list = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15309 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15312 "isa_thumb.tcc"
>
OpPush<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15317 "isa_thumb.tcc"
>::OpPush(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15321 "isa_thumb.tcc"
>(code, addr, "push")
{
	r = ((code >> 8) & 0x1);
	reg_list = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15331 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15334 "isa_thumb.tcc"
>
OpStmia<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15339 "isa_thumb.tcc"
>::OpStmia(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15343 "isa_thumb.tcc"
>(code, addr, "stmia")
{
	rn = ((code >> 8) & 0x7);
	reg_list = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15353 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15356 "isa_thumb.tcc"
>
OpBkpt<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15361 "isa_thumb.tcc"
>::OpBkpt(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15365 "isa_thumb.tcc"
>(code, addr, "bkpt")
{
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15374 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15377 "isa_thumb.tcc"
>
OpSwi<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15382 "isa_thumb.tcc"
>::OpSwi(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15386 "isa_thumb.tcc"
>(code, addr, "swi")
{
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15395 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15398 "isa_thumb.tcc"
>
DecodeMapPage<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15403 "isa_thumb.tcc"
>::DecodeMapPage(typename CONFIG::address_t key)
{
	this->key = key;
	memset(operation, 0, sizeof(operation));
	next = 0;
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15414 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15417 "isa_thumb.tcc"
>
DecodeMapPage<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15422 "isa_thumb.tcc"
>::~DecodeMapPage()
{
	unsigned int idx;
	for(idx = 0; idx < NUM_OPERATIONS_PER_PAGE; idx++)
	delete operation[idx];
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15433 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15436 "isa_thumb.tcc"
>
DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15441 "isa_thumb.tcc"
>::DecodeTableEntry(CodeType opcode, CodeType opcode_mask, Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15445 "isa_thumb.tcc"
> *(*decode)(CodeType, typename CONFIG::address_t))
{
	this->opcode = opcode;
	this->opcode_mask = opcode_mask;
	this->decode = decode;
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15456 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15459 "isa_thumb.tcc"
>
Decoder<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15464 "isa_thumb.tcc"
>::Decoder()
: is_little_endian( false ), mru_page( 0 )
{
	memset(decode_hash_table, 0, sizeof(decode_hash_table));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15472 "isa_thumb.tcc"
	>(0xdf00U, 0xff00U, DecodeOpSwi<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15476 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15481 "isa_thumb.tcc"
	>(0xbe00U, 0xff00U, DecodeOpBkpt<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15485 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15490 "isa_thumb.tcc"
	>(0xc000U, 0xf800U, DecodeOpStmia<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15494 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15499 "isa_thumb.tcc"
	>(0xb400U, 0xfe00U, DecodeOpPush<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15503 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15508 "isa_thumb.tcc"
	>(0xbc00U, 0xfe00U, DecodeOpPop<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15512 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15517 "isa_thumb.tcc"
	>(0xc800U, 0xf800U, DecodeOpLdmia<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15521 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15526 "isa_thumb.tcc"
	>(0x5200U, 0xfe00U, DecodeOpStrh_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15530 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15535 "isa_thumb.tcc"
	>(0x8000U, 0xf800U, DecodeOpStrh_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15539 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15544 "isa_thumb.tcc"
	>(0x5400U, 0xfe00U, DecodeOpStrb_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15548 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15553 "isa_thumb.tcc"
	>(0x7000U, 0xf800U, DecodeOpStrb_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15557 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15562 "isa_thumb.tcc"
	>(0x9000U, 0xf800U, DecodeOpStr_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15566 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15571 "isa_thumb.tcc"
	>(0x5000U, 0xfe00U, DecodeOpStr_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15575 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15580 "isa_thumb.tcc"
	>(0x6000U, 0xf800U, DecodeOpStr_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15584 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15589 "isa_thumb.tcc"
	>(0x5e00U, 0xfe00U, DecodeOpLdrsh<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15593 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15598 "isa_thumb.tcc"
	>(0x5600U, 0xfe00U, DecodeOpLdrsb<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15602 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15607 "isa_thumb.tcc"
	>(0x5a00U, 0xfe00U, DecodeOpLdrh_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15611 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15616 "isa_thumb.tcc"
	>(0x8800U, 0xf800U, DecodeOpLdrh_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15620 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15625 "isa_thumb.tcc"
	>(0x5c00U, 0xfe00U, DecodeOpLdrb_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15629 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15634 "isa_thumb.tcc"
	>(0x7800U, 0xf800U, DecodeOpLdrb_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15638 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15643 "isa_thumb.tcc"
	>(0x9800U, 0xf800U, DecodeOpLdr_4<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15647 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15652 "isa_thumb.tcc"
	>(0x4800U, 0xf800U, DecodeOpLdr_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15656 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15661 "isa_thumb.tcc"
	>(0x5800U, 0xfe00U, DecodeOpLdr_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15665 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15670 "isa_thumb.tcc"
	>(0x6800U, 0xf800U, DecodeOpLdr_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15674 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15679 "isa_thumb.tcc"
	>(0x4200U, 0xffc0U, DecodeOpTst<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15683 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15688 "isa_thumb.tcc"
	>(0xb080U, 0xff80U, DecodeOpSub_4<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15692 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15697 "isa_thumb.tcc"
	>(0x1a00U, 0xfe00U, DecodeOpSub_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15701 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15706 "isa_thumb.tcc"
	>(0x3800U, 0xf800U, DecodeOpSub_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15710 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15715 "isa_thumb.tcc"
	>(0x1e00U, 0xfe00U, DecodeOpSub_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15719 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15724 "isa_thumb.tcc"
	>(0x4180U, 0xffc0U, DecodeOpSbc<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15728 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15733 "isa_thumb.tcc"
	>(0x41c0U, 0xffc0U, DecodeOpRor<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15737 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15742 "isa_thumb.tcc"
	>(0x4300U, 0xffc0U, DecodeOpOrr<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15746 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15751 "isa_thumb.tcc"
	>(0x4240U, 0xffc0U, DecodeOpNeg<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15755 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15760 "isa_thumb.tcc"
	>(0x43c0U, 0xffc0U, DecodeOpMvn<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15764 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15769 "isa_thumb.tcc"
	>(0x4340U, 0xffc0U, DecodeOpMul<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15773 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15778 "isa_thumb.tcc"
	>(0x4600U, 0xff00U, DecodeOpMov_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15782 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15787 "isa_thumb.tcc"
	>(0x2000U, 0xf800U, DecodeOpMov_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15791 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15796 "isa_thumb.tcc"
	>(0x40c0U, 0xffc0U, DecodeOpLsr_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15800 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15805 "isa_thumb.tcc"
	>(0x800U, 0xf800U, DecodeOpLsr_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15809 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15814 "isa_thumb.tcc"
	>(0x4080U, 0xffc0U, DecodeOpLsl_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15818 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15823 "isa_thumb.tcc"
	>(0x0U, 0xf800U, DecodeOpLsl_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15827 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15832 "isa_thumb.tcc"
	>(0x4040U, 0xffc0U, DecodeOpEor<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15836 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15841 "isa_thumb.tcc"
	>(0x4500U, 0xff00U, DecodeOpCmp_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15845 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15850 "isa_thumb.tcc"
	>(0x4280U, 0xffc0U, DecodeOpCmp_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15854 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15859 "isa_thumb.tcc"
	>(0x2800U, 0xf800U, DecodeOpCmp_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15863 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15868 "isa_thumb.tcc"
	>(0x42c0U, 0xffc0U, DecodeOpCmn<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15872 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15877 "isa_thumb.tcc"
	>(0x4380U, 0xffc0U, DecodeOpBic<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15881 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15886 "isa_thumb.tcc"
	>(0x4100U, 0xffc0U, DecodeOpAsr_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15890 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15895 "isa_thumb.tcc"
	>(0x1000U, 0xf800U, DecodeOpAsr_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15899 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15904 "isa_thumb.tcc"
	>(0x4000U, 0xffc0U, DecodeOpAnd<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15908 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15913 "isa_thumb.tcc"
	>(0xb000U, 0xff80U, DecodeOpAdd_7<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15917 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15922 "isa_thumb.tcc"
	>(0xa800U, 0xf800U, DecodeOpAdd_6<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15926 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15931 "isa_thumb.tcc"
	>(0xa000U, 0xf800U, DecodeOpAdd_5<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15935 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15940 "isa_thumb.tcc"
	>(0x4400U, 0xff00U, DecodeOpAdd_4<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15944 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15949 "isa_thumb.tcc"
	>(0x1800U, 0xfe00U, DecodeOpAdd_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15953 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15958 "isa_thumb.tcc"
	>(0x3000U, 0xf800U, DecodeOpAdd_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15962 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15967 "isa_thumb.tcc"
	>(0x1c00U, 0xfe00U, DecodeOpAdd_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15971 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15976 "isa_thumb.tcc"
	>(0x4140U, 0xffc0U, DecodeOpAdc<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15980 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15985 "isa_thumb.tcc"
	>(0x4780U, 0xff80U, DecodeOpBlx_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15989 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15994 "isa_thumb.tcc"
	>(0xe800U, 0xf800U, DecodeOpBlx_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15998 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16003 "isa_thumb.tcc"
	>(0x4700U, 0xff80U, DecodeOpBx<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16007 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16012 "isa_thumb.tcc"
	>(0xf000U, 0xf000U, DecodeOpBl<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16016 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16021 "isa_thumb.tcc"
	>(0xe000U, 0xf800U, DecodeOpB_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16025 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16030 "isa_thumb.tcc"
	>(0xdc00U, 0xfe00U, DecodeOpB_1_110x<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16034 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16039 "isa_thumb.tcc"
	>(0xd800U, 0xfc00U, DecodeOpB_1_10xx<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16043 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16048 "isa_thumb.tcc"
	>(0xd000U, 0xf800U, DecodeOpB_1_0xxx<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16052 "isa_thumb.tcc"
	>));
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 16059 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16062 "isa_thumb.tcc"
>
Decoder<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16067 "isa_thumb.tcc"
>::~Decoder()
{
	InvalidateDecodingCache();
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 16076 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16079 "isa_thumb.tcc"
>
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16084 "isa_thumb.tcc"
> *Decoder<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16088 "isa_thumb.tcc"
>::NCDecode(typename CONFIG::address_t addr, CodeType code)
{
	Operation<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16094 "isa_thumb.tcc"
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
#line 16120 "isa_thumb.tcc"
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
#line 16139 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16142 "isa_thumb.tcc"
>
void Decoder<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16147 "isa_thumb.tcc"
>::InvalidateDecodingCache()
{
	uint32_t index;
	mru_page = 0;
	for(index = 0; index < NUM_DECODE_HASH_TABLE_ENTRIES; index++)
	{
		DecodeMapPage<
#line 44 "isa/thumb/thumb.isa"
		CONFIG
#line 16157 "isa_thumb.tcc"
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
#line 16176 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16179 "isa_thumb.tcc"
>
void Decoder<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16184 "isa_thumb.tcc"
>::InvalidateDecodingCacheEntry(typename CONFIG::address_t addr)
{
	typename CONFIG::address_t page_key = addr / 2 / NUM_OPERATIONS_PER_PAGE;
	if(mru_page && mru_page->key == page_key) mru_page = 0;
	uint32_t index = page_key % NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key
	DecodeMapPage<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16193 "isa_thumb.tcc"
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
#line 16226 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16229 "isa_thumb.tcc"
>
inline DecodeMapPage<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16234 "isa_thumb.tcc"
> *Decoder<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16238 "isa_thumb.tcc"
>::FindPage(typename CONFIG::address_t page_key)
{
	if(mru_page && mru_page->key == page_key) return mru_page;
	uint32_t index = page_key % NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key
	DecodeMapPage<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16246 "isa_thumb.tcc"
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
#line 16280 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16283 "isa_thumb.tcc"
>
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16288 "isa_thumb.tcc"
> *Decoder<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16292 "isa_thumb.tcc"
>::Decode(typename CONFIG::address_t addr, CodeType insn)
{
	Operation<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16298 "isa_thumb.tcc"
	> *operation;
	typename CONFIG::address_t page_key = addr / 2 / NUM_OPERATIONS_PER_PAGE;
	DecodeMapPage<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16304 "isa_thumb.tcc"
	> *page;
	page = FindPage(page_key);
	if(!page)
	{
		page = new DecodeMapPage<
#line 44 "isa/thumb/thumb.isa"
		CONFIG
#line 16312 "isa_thumb.tcc"
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
#line 16334 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16337 "isa_thumb.tcc"
>
void Decoder<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16342 "isa_thumb.tcc"
>::SetLittleEndian()
{
	is_little_endian = true;
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 16351 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16354 "isa_thumb.tcc"
>
void Decoder<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16359 "isa_thumb.tcc"
>::SetBigEndian()
{
	is_little_endian = false;
}

} } } } } } }
