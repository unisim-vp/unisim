#include "isa_thumb.hh"
#include <cassert>
#line 75 "isa/thumb/thumb.isa"

#include "unisim/util/arithmetic/arithmetic.hh"
#include "unisim/component/cxx/processor/arm/exception.hh"
#include "unisim/service/interfaces/logger.hh"

using unisim::component::cxx::processor::arm::UndefinedInstructionException;
using unisim::service::interfaces::DebugInfo;
using unisim::service::interfaces::DebugWarning;
using unisim::service::interfaces::DebugError;
using unisim::service::interfaces::EndDebugInfo;
using unisim::service::interfaces::EndDebugWarning;
using unisim::service::interfaces::EndDebugError;
using unisim::service::interfaces::Endl;
using unisim::service::interfaces::Hex;
using unisim::service::interfaces::Dec;
using unisim::service::interfaces::File;
using unisim::service::interfaces::Function;
using unisim::service::interfaces::Line;
using unisim::util::arithmetic::Add32;
#line 24 "isa_thumb.tcc"
namespace unisim { namespace component { namespace cxx { namespace processor { namespace arm { namespace isa { namespace thumb {
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 29 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 32 "isa_thumb.tcc"
>
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 37 "isa_thumb.tcc"
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
#line 49 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 52 "isa_thumb.tcc"
>
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 57 "isa_thumb.tcc"
>::~Operation()
{
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 65 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 68 "isa_thumb.tcc"
>
#line 53 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/profiling.isa"
void
#line 72 "isa_thumb.tcc"
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 76 "isa_thumb.tcc"
>::profile(
#line 53 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/profiling.isa"
map<uint64_t, uint32_t> &
#line 80 "isa_thumb.tcc"
#line 53 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/profiling.isa"
prof
#line 83 "isa_thumb.tcc"
)
{
#line 53 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/profiling.isa"
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
#line 97 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 102 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 105 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 109 "isa_thumb.tcc"
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 113 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 117 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 120 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 124 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 127 "isa_thumb.tcc"
)
{
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	{
		buffer << "Unknown instruction";
	}
#line 134 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 139 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 142 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 146 "isa_thumb.tcc"
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 150 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 154 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 157 "isa_thumb.tcc"
)
{
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	{
		throw UndefinedInstructionException<CONFIG>();
	}
#line 164 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 169 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 172 "isa_thumb.tcc"
>

void
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 179 "isa_thumb.tcc"
>::initialize_rd_index()
{
#line 89 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rd_index = 0;
	}
#line 186 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 191 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 194 "isa_thumb.tcc"
>

void
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 201 "isa_thumb.tcc"
>::initialize_rn_index()
{
#line 85 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rn_index = 0;
	}
#line 208 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 213 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 216 "isa_thumb.tcc"
>

void
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 223 "isa_thumb.tcc"
>::initialize_rm_index()
{
#line 81 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rm_index = 0;
	}
#line 230 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 235 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 238 "isa_thumb.tcc"
>

void
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 245 "isa_thumb.tcc"
>::initialize_rdp1_init()
{
#line 73 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rdp1_init = 0;
	}
#line 252 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 257 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 260 "isa_thumb.tcc"
>

void
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 267 "isa_thumb.tcc"
>::initialize_rdlo_init()
{
#line 69 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rdlo_init = 0;
	}
#line 274 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 279 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 282 "isa_thumb.tcc"
>

void
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 289 "isa_thumb.tcc"
>::initialize_rdhi_init()
{
#line 65 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rdhi_init = 0;
	}
#line 296 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 301 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 304 "isa_thumb.tcc"
>

void
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 311 "isa_thumb.tcc"
>::initialize_rs_init()
{
#line 61 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rs_init = 0;
	}
#line 318 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 323 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 326 "isa_thumb.tcc"
>

void
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 333 "isa_thumb.tcc"
>::initialize_rd_init()
{
#line 57 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rd_init = 0;
	}
#line 340 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 345 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 348 "isa_thumb.tcc"
>

void
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 355 "isa_thumb.tcc"
>::initialize_rm_init()
{
#line 53 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rm_init = 0;
	}
#line 362 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 367 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 370 "isa_thumb.tcc"
>

void
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 377 "isa_thumb.tcc"
>::initialize_rn_init()
{
#line 49 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rn_init = 0;
	}
#line 384 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 389 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 392 "isa_thumb.tcc"
>

void
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 399 "isa_thumb.tcc"
>::initialize_cond_init()
{
#line 37 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		cond_init = 0;
	}
#line 406 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 411 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 414 "isa_thumb.tcc"
>
class OpB_1_0xxx : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 419 "isa_thumb.tcc"
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
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 504 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 508 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 511 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 515 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 518 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 556 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 560 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 563 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 571 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 574 "isa_thumb.tcc"
>
class OpB_1_10xx : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 579 "isa_thumb.tcc"
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
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 664 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 668 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 671 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 675 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 678 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 716 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 720 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 723 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 731 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 734 "isa_thumb.tcc"
>
class OpB_1_110x : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 739 "isa_thumb.tcc"
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
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 824 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 828 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 831 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 835 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 838 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 876 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 880 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 883 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 891 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 894 "isa_thumb.tcc"
>
class OpB_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 899 "isa_thumb.tcc"
>
{
public:
	OpB_2(CodeType code, typename CONFIG::address_t addr);
	int32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 908 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 912 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 915 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 919 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 922 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 960 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 964 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 967 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 975 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 978 "isa_thumb.tcc"
>
class OpBl : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 983 "isa_thumb.tcc"
>
{
public:
	OpBl(CodeType code, typename CONFIG::address_t addr);
	uint32_t h2;
	uint32_t offset;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 993 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 997 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1000 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1004 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1007 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1045 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1049 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1052 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 1060 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1063 "isa_thumb.tcc"
>
class OpBx : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1068 "isa_thumb.tcc"
>
{
public:
	OpBx(CodeType code, typename CONFIG::address_t addr);
	uint32_t h2;
	uint32_t rm;
	uint32_t sbz;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1079 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1083 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1086 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1090 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1093 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1137 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1141 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1144 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 1152 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1155 "isa_thumb.tcc"
>
class OpBlx_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1160 "isa_thumb.tcc"
>
{
public:
	OpBlx_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t offset;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1169 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1173 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1176 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1180 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1183 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1221 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1225 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1228 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 1236 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1239 "isa_thumb.tcc"
>
class OpBlx_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1244 "isa_thumb.tcc"
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
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1261 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1265 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1268 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1272 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1275 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1313 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1317 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1320 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 1328 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1331 "isa_thumb.tcc"
>
class OpAdc : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1336 "isa_thumb.tcc"
>
{
public:
	OpAdc(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1346 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1350 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1353 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1357 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1360 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1398 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1402 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1405 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 1413 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1416 "isa_thumb.tcc"
>
class OpAdd_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1421 "isa_thumb.tcc"
>
{
public:
	OpAdd_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1432 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1436 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1439 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1443 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1446 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1484 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1488 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1491 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 1499 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1502 "isa_thumb.tcc"
>
class OpAdd_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1507 "isa_thumb.tcc"
>
{
public:
	OpAdd_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rd;
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1517 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1521 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1524 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1528 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1531 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1569 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1573 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1576 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 1584 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1587 "isa_thumb.tcc"
>
class OpAdd_3 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1592 "isa_thumb.tcc"
>
{
public:
	OpAdd_3(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1603 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1607 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1610 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1614 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1617 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1655 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1659 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1662 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 1670 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1673 "isa_thumb.tcc"
>
class OpAdd_4 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1678 "isa_thumb.tcc"
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
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1702 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1706 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1709 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1713 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1716 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1754 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1758 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1761 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 1769 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1772 "isa_thumb.tcc"
>
class OpAdd_5 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1777 "isa_thumb.tcc"
>
{
public:
	OpAdd_5(CodeType code, typename CONFIG::address_t addr);
	uint32_t rd;
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1787 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1791 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1794 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1798 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1801 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1839 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1843 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1846 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 1854 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1857 "isa_thumb.tcc"
>
class OpAdd_6 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1862 "isa_thumb.tcc"
>
{
public:
	OpAdd_6(CodeType code, typename CONFIG::address_t addr);
	uint32_t rd;
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1872 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1876 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1879 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1883 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1886 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1924 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1928 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1931 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 1939 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1942 "isa_thumb.tcc"
>
class OpAdd_7 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 1947 "isa_thumb.tcc"
>
{
public:
	OpAdd_7(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1956 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1960 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1963 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1967 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1970 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2008 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2012 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2015 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2023 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2026 "isa_thumb.tcc"
>
class OpAnd : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2031 "isa_thumb.tcc"
>
{
public:
	OpAnd(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2041 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2045 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2048 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2052 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2055 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2093 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2097 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2100 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2108 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2111 "isa_thumb.tcc"
>
class OpAsr_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2116 "isa_thumb.tcc"
>
{
public:
	OpAsr_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2127 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2131 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2134 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2138 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2141 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2179 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2183 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2186 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2194 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2197 "isa_thumb.tcc"
>
class OpAsr_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2202 "isa_thumb.tcc"
>
{
public:
	OpAsr_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rs;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2212 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2216 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2219 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2223 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2226 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2264 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2268 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2271 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2279 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2282 "isa_thumb.tcc"
>
class OpBic : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2287 "isa_thumb.tcc"
>
{
public:
	OpBic(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2297 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2301 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2304 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2308 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2311 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2349 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2353 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2356 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2364 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2367 "isa_thumb.tcc"
>
class OpCmn : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2372 "isa_thumb.tcc"
>
{
public:
	OpCmn(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2382 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2386 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2389 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2393 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2396 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2434 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2438 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2441 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2449 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2452 "isa_thumb.tcc"
>
class OpCmp_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2457 "isa_thumb.tcc"
>
{
public:
	OpCmp_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t rn;
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2467 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2471 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2474 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2478 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2481 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2519 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2523 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2526 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2534 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2537 "isa_thumb.tcc"
>
class OpCmp_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2542 "isa_thumb.tcc"
>
{
public:
	OpCmp_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2552 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2556 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2559 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2563 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2566 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2604 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2608 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2611 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2619 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2622 "isa_thumb.tcc"
>
class OpCmp_3 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2627 "isa_thumb.tcc"
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
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2651 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2655 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2658 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2662 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2665 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2703 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2707 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2710 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2718 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2721 "isa_thumb.tcc"
>
class OpEor : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2726 "isa_thumb.tcc"
>
{
public:
	OpEor(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2736 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2740 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2743 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2747 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2750 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2788 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2792 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2795 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2803 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2806 "isa_thumb.tcc"
>
class OpLsl_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2811 "isa_thumb.tcc"
>
{
public:
	OpLsl_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2822 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2826 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2829 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2833 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2836 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2874 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2878 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2881 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2889 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2892 "isa_thumb.tcc"
>
class OpLsl_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2897 "isa_thumb.tcc"
>
{
public:
	OpLsl_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rs;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2907 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2911 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2914 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2918 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2921 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2959 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2963 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2966 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 2974 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2977 "isa_thumb.tcc"
>
class OpLsr_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 2982 "isa_thumb.tcc"
>
{
public:
	OpLsr_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2993 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2997 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3000 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3004 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3007 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3045 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3049 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3052 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 3060 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3063 "isa_thumb.tcc"
>
class OpLsr_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3068 "isa_thumb.tcc"
>
{
public:
	OpLsr_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rs;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3078 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3082 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3085 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3089 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3092 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3130 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3134 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3137 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 3145 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3148 "isa_thumb.tcc"
>
class OpMov_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3153 "isa_thumb.tcc"
>
{
public:
	OpMov_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t rd;
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3163 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3167 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3170 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3174 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3177 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3215 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3219 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3222 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 3230 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3233 "isa_thumb.tcc"
>
class OpMov_3 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3238 "isa_thumb.tcc"
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
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3259 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3263 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3266 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3270 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3273 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3311 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3315 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3318 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 3326 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3329 "isa_thumb.tcc"
>
class OpMul : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3334 "isa_thumb.tcc"
>
{
public:
	OpMul(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3344 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3348 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3351 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3355 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3358 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3396 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3400 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3403 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 3411 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3414 "isa_thumb.tcc"
>
class OpMvn : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3419 "isa_thumb.tcc"
>
{
public:
	OpMvn(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3429 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3433 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3436 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3440 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3443 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3481 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3485 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3488 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 3496 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3499 "isa_thumb.tcc"
>
class OpNeg : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3504 "isa_thumb.tcc"
>
{
public:
	OpNeg(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3514 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3518 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3521 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3525 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3528 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3566 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3570 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3573 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 3581 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3584 "isa_thumb.tcc"
>
class OpOrr : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3589 "isa_thumb.tcc"
>
{
public:
	OpOrr(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3599 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3603 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3606 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3610 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3613 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3651 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3655 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3658 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 3666 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3669 "isa_thumb.tcc"
>
class OpRor : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3674 "isa_thumb.tcc"
>
{
public:
	OpRor(CodeType code, typename CONFIG::address_t addr);
	uint32_t rs;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3684 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3688 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3691 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3695 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3698 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3736 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3740 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3743 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 3751 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3754 "isa_thumb.tcc"
>
class OpSbc : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3759 "isa_thumb.tcc"
>
{
public:
	OpSbc(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3769 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3773 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3776 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3780 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3783 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3821 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3825 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3828 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 3836 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3839 "isa_thumb.tcc"
>
class OpSub_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3844 "isa_thumb.tcc"
>
{
public:
	OpSub_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3855 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3859 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3862 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3866 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3869 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3907 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3911 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3914 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 3922 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3925 "isa_thumb.tcc"
>
class OpSub_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 3930 "isa_thumb.tcc"
>
{
public:
	OpSub_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rd;
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3940 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3944 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3947 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3951 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3954 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3992 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3996 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3999 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4007 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4010 "isa_thumb.tcc"
>
class OpSub_3 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4015 "isa_thumb.tcc"
>
{
public:
	OpSub_3(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4026 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4030 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4033 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4037 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4040 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4078 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4082 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4085 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4093 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4096 "isa_thumb.tcc"
>
class OpSub_4 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4101 "isa_thumb.tcc"
>
{
public:
	OpSub_4(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4110 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4114 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4117 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4121 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4124 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4162 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4166 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4169 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4177 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4180 "isa_thumb.tcc"
>
class OpTst : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4185 "isa_thumb.tcc"
>
{
public:
	OpTst(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4195 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4199 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4202 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4206 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4209 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4247 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4251 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4254 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4262 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4265 "isa_thumb.tcc"
>
class OpLdr_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4270 "isa_thumb.tcc"
>
{
public:
	OpLdr_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4281 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4285 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4288 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4292 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4295 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4333 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4337 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4340 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4348 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4351 "isa_thumb.tcc"
>
class OpLdr_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4356 "isa_thumb.tcc"
>
{
public:
	OpLdr_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4367 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4371 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4374 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4378 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4381 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4419 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4423 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4426 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4434 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4437 "isa_thumb.tcc"
>
class OpLdr_3 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4442 "isa_thumb.tcc"
>
{
public:
	OpLdr_3(CodeType code, typename CONFIG::address_t addr);
	uint32_t rd;
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4452 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4456 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4459 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4463 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4466 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4504 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4508 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4511 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4519 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4522 "isa_thumb.tcc"
>
class OpLdr_4 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4527 "isa_thumb.tcc"
>
{
public:
	OpLdr_4(CodeType code, typename CONFIG::address_t addr);
	uint32_t rd;
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4537 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4541 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4544 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4548 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4551 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4589 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4593 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4596 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4604 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4607 "isa_thumb.tcc"
>
class OpLdrb_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4612 "isa_thumb.tcc"
>
{
public:
	OpLdrb_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4623 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4627 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4630 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4634 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4637 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4675 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4679 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4682 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4690 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4693 "isa_thumb.tcc"
>
class OpLdrb_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4698 "isa_thumb.tcc"
>
{
public:
	OpLdrb_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4709 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4713 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4716 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4720 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4723 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4761 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4765 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4768 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4776 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4779 "isa_thumb.tcc"
>
class OpLdrh_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4784 "isa_thumb.tcc"
>
{
public:
	OpLdrh_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4795 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4799 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4802 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4806 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4809 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4847 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4851 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4854 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4862 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4865 "isa_thumb.tcc"
>
class OpLdrh_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4870 "isa_thumb.tcc"
>
{
public:
	OpLdrh_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4881 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4885 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4888 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4892 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4895 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4933 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4937 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4940 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 4948 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4951 "isa_thumb.tcc"
>
class OpLdrsb : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 4956 "isa_thumb.tcc"
>
{
public:
	OpLdrsb(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4967 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4971 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4974 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4978 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4981 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5019 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5023 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5026 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5034 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5037 "isa_thumb.tcc"
>
class OpLdrsh : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5042 "isa_thumb.tcc"
>
{
public:
	OpLdrsh(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5053 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5057 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5060 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5064 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5067 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5105 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5109 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5112 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5120 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5123 "isa_thumb.tcc"
>
class OpStr_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5128 "isa_thumb.tcc"
>
{
public:
	OpStr_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5139 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5143 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5146 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5150 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5153 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5191 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5195 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5198 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5206 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5209 "isa_thumb.tcc"
>
class OpStr_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5214 "isa_thumb.tcc"
>
{
public:
	OpStr_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5225 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5229 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5232 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5236 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5239 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5277 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5281 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5284 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5292 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5295 "isa_thumb.tcc"
>
class OpStr_3 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5300 "isa_thumb.tcc"
>
{
public:
	OpStr_3(CodeType code, typename CONFIG::address_t addr);
	uint32_t rd;
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5310 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5314 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5317 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5321 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5324 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5362 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5366 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5369 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5377 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5380 "isa_thumb.tcc"
>
class OpStrb_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5385 "isa_thumb.tcc"
>
{
public:
	OpStrb_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5396 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5400 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5403 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5407 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5410 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5448 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5452 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5455 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5463 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5466 "isa_thumb.tcc"
>
class OpStrb_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5471 "isa_thumb.tcc"
>
{
public:
	OpStrb_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5482 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5486 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5489 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5493 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5496 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5534 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5538 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5541 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5549 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5552 "isa_thumb.tcc"
>
class OpStrh_1 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5557 "isa_thumb.tcc"
>
{
public:
	OpStrh_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5568 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5572 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5575 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5579 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5582 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5620 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5624 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5627 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5635 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5638 "isa_thumb.tcc"
>
class OpStrh_2 : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5643 "isa_thumb.tcc"
>
{
public:
	OpStrh_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5654 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5658 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5661 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5665 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5668 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5706 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5710 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5713 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5721 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5724 "isa_thumb.tcc"
>
class OpLdmia : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5729 "isa_thumb.tcc"
>
{
public:
	OpLdmia(CodeType code, typename CONFIG::address_t addr);
	uint32_t rn;
	uint32_t reg_list;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5739 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5743 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5746 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5750 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5753 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5791 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5795 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5798 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5806 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5809 "isa_thumb.tcc"
>
class OpPop : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5814 "isa_thumb.tcc"
>
{
public:
	OpPop(CodeType code, typename CONFIG::address_t addr);
	uint32_t r;
	uint32_t reg_list;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5824 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5828 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5831 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5835 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5838 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5876 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5880 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5883 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5891 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5894 "isa_thumb.tcc"
>
class OpPush : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5899 "isa_thumb.tcc"
>
{
public:
	OpPush(CodeType code, typename CONFIG::address_t addr);
	uint32_t r;
	uint32_t reg_list;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5909 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5913 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5916 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5920 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5923 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5961 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5965 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5968 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 5976 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5979 "isa_thumb.tcc"
>
class OpStmia : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 5984 "isa_thumb.tcc"
>
{
public:
	OpStmia(CodeType code, typename CONFIG::address_t addr);
	uint32_t rn;
	uint32_t reg_list;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5994 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5998 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 6001 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 6005 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 6008 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 6046 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 6050 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 6053 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6061 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6064 "isa_thumb.tcc"
>
class OpBkpt : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6069 "isa_thumb.tcc"
>
{
public:
	OpBkpt(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 6078 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 6082 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 6085 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 6089 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 6092 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 6130 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 6134 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 6137 "isa_thumb.tcc"
	);
private:
};

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6145 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6148 "isa_thumb.tcc"
>
class OpSwi : public Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6153 "isa_thumb.tcc"
>
{
public:
	OpSwi(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	virtual
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 6162 "isa_thumb.tcc"
	disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 6166 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 6169 "isa_thumb.tcc"
	,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 6173 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 6176 "isa_thumb.tcc"
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
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 6214 "isa_thumb.tcc"
	execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 6218 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 6221 "isa_thumb.tcc"
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
#line 6271 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6274 "isa_thumb.tcc"
>

void
OpB_1_0xxx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6281 "isa_thumb.tcc"
>::initialize_cond_init()
{
#line 50 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::cond_init = sub_cond;
	}
#line 6290 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6295 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6298 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6302 "isa_thumb.tcc"
OpB_1_0xxx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6306 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6310 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6313 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 6317 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 6320 "isa_thumb.tcc"
)
{
#line 70 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;
		buffer << "b";
		cpu.DisasmCondition(inherited::cond_init, buffer);
		buffer << " 0x" << hex << cpu.GetGPR(cpu.PC_reg) + 4 + (imm << 1) << dec;
	}
#line 6330 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6335 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6338 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6342 "isa_thumb.tcc"
OpB_1_0xxx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6346 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6350 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6353 "isa_thumb.tcc"
)
{
#line 77 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
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
#line 6370 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6376 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6379 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6384 "isa_thumb.tcc"
> *DecodeOpB_1_0xxx(CodeType code, typename CONFIG::address_t addr)
{
	return new OpB_1_0xxx<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 6390 "isa_thumb.tcc"
	>(code, addr);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6397 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6400 "isa_thumb.tcc"
>

void
OpB_1_10xx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6407 "isa_thumb.tcc"
>::initialize_cond_init()
{
#line 56 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::cond_init = sub_cond + 8;
	}
#line 6416 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6421 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6424 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6428 "isa_thumb.tcc"
OpB_1_10xx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6432 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6436 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6439 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 6443 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 6446 "isa_thumb.tcc"
)
{
#line 70 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;
		buffer << "b";
		cpu.DisasmCondition(inherited::cond_init, buffer);
		buffer << " 0x" << hex << cpu.GetGPR(cpu.PC_reg) + 4 + (imm << 1) << dec;
	}
#line 6456 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6461 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6464 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6468 "isa_thumb.tcc"
OpB_1_10xx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6472 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6476 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6479 "isa_thumb.tcc"
)
{
#line 77 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
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
#line 6496 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6502 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6505 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6510 "isa_thumb.tcc"
> *DecodeOpB_1_10xx(CodeType code, typename CONFIG::address_t addr)
{
	return new OpB_1_10xx<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 6516 "isa_thumb.tcc"
	>(code, addr);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6523 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6526 "isa_thumb.tcc"
>

void
OpB_1_110x<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6533 "isa_thumb.tcc"
>::initialize_cond_init()
{
#line 62 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::cond_init = sub_cond + 12;
	}
#line 6542 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6547 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6550 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6554 "isa_thumb.tcc"
OpB_1_110x<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6558 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6562 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6565 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 6569 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 6572 "isa_thumb.tcc"
)
{
#line 70 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;
		buffer << "b";
		cpu.DisasmCondition(inherited::cond_init, buffer);
		buffer << " 0x" << hex << cpu.GetGPR(cpu.PC_reg) + 4 + (imm << 1) << dec;
	}
#line 6582 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6587 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6590 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6594 "isa_thumb.tcc"
OpB_1_110x<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6598 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6602 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6605 "isa_thumb.tcc"
)
{
#line 77 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
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
#line 6622 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6628 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6631 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6636 "isa_thumb.tcc"
> *DecodeOpB_1_110x(CodeType code, typename CONFIG::address_t addr)
{
	return new OpB_1_110x<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 6642 "isa_thumb.tcc"
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
#line 6656 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6659 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6663 "isa_thumb.tcc"
OpB_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6667 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6671 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6674 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 6678 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 6681 "isa_thumb.tcc"
)
{
#line 102 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		buffer << "b";
		buffer << " 0x" << hex << cpu.GetGPR(cpu.PC_reg) + 4 + (imm << 1) << dec;
	}
#line 6691 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6696 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6699 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6703 "isa_thumb.tcc"
OpB_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6707 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6711 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6714 "isa_thumb.tcc"
)
{
#line 109 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		typename CONFIG::reg_t new_PC;

		new_PC = cpu.GetGPR(cpu.PC_reg) + 4 + (imm << 1);
		cpu.SetGPR(cpu.PC_reg, new_PC);
	}
#line 6726 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6732 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6735 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6740 "isa_thumb.tcc"
> *DecodeOpB_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpB_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 6746 "isa_thumb.tcc"
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
#line 6760 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6763 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6767 "isa_thumb.tcc"
OpBl<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6771 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6775 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6778 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 6782 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 6785 "isa_thumb.tcc"
)
{
#line 129 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
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
#line 6803 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6808 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6811 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6815 "isa_thumb.tcc"
OpBl<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6819 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6823 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6826 "isa_thumb.tcc"
)
{
#line 144 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
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
#line 6851 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6857 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6860 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6865 "isa_thumb.tcc"
> *DecodeOpBl(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBl<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 6871 "isa_thumb.tcc"
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
#line 6885 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6888 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6892 "isa_thumb.tcc"
OpBx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6896 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6900 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6903 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 6907 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 6910 "isa_thumb.tcc"
)
{
#line 177 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		buffer << "bx "
		<< "r" << inherited::rm_index << endl;
	}
#line 6920 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6925 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6928 "isa_thumb.tcc"
>

void
OpBx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6935 "isa_thumb.tcc"
>::initialize_rm_init()
{
#line 184 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		if((h2 << 3) + rm == CPU<CONFIG>::PC_reg)
		inherited::rm_init = 4;
		else
		inherited::rm_init = 0;
	}
#line 6947 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6952 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6955 "isa_thumb.tcc"
>

void
OpBx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6962 "isa_thumb.tcc"
>::initialize_rm_index()
{
#line 193 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::rm_index = (h2 << 3) + rm;
	}
#line 6971 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 6976 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6979 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6983 "isa_thumb.tcc"
OpBx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 6987 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6991 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6994 "isa_thumb.tcc"
)
{
#line 199 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
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
#line 7013 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7019 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7022 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7027 "isa_thumb.tcc"
> *DecodeOpBx(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBx<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 7033 "isa_thumb.tcc"
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
#line 7047 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7050 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7054 "isa_thumb.tcc"
OpBlx_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7058 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7062 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7065 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 7069 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 7072 "isa_thumb.tcc"
)
{
#line 226 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
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
#line 7091 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7096 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7099 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7103 "isa_thumb.tcc"
OpBlx_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7107 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7111 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7114 "isa_thumb.tcc"
)
{
#line 242 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typename CONFIG::reg_t new_PC;

		new_PC = (cpu.GetGPR(cpu.LR_reg) + (offset << 1)) & (typename CONFIG::reg_t)0xFFFFFFFC;
		cpu.SetGPR(cpu.LR_reg, cpu.GetGPR(cpu.PC_reg) + 3);
		cpu.SetGPR(cpu.PC_reg, new_PC);
	}
#line 7125 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7131 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7134 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7139 "isa_thumb.tcc"
> *DecodeOpBlx_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBlx_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 7145 "isa_thumb.tcc"
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
#line 7159 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7162 "isa_thumb.tcc"
>

void
OpBlx_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7169 "isa_thumb.tcc"
>::initialize_rm_init()
{
#line 261 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		if((h2 << 3) + rm == CPU<CONFIG>::PC_reg)
		inherited::rm_init = 4;
		else
		inherited::rm_init = 0;
	}
#line 7181 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7186 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7189 "isa_thumb.tcc"
>

void
OpBlx_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7196 "isa_thumb.tcc"
>::initialize_rm_index()
{
#line 270 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::rm_index = (h2 << 3) + rm;
	}
#line 7205 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7210 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7213 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7217 "isa_thumb.tcc"
OpBlx_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7221 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7225 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7228 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 7232 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 7235 "isa_thumb.tcc"
)
{
#line 276 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		buffer << "blx r" << inherited::rm_index;
	}
#line 7244 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7249 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7252 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7256 "isa_thumb.tcc"
OpBlx_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7260 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7264 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7267 "isa_thumb.tcc"
)
{
#line 282 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
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
#line 7286 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7292 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7295 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7300 "isa_thumb.tcc"
> *DecodeOpBlx_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBlx_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 7306 "isa_thumb.tcc"
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
#line 7357 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7360 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7364 "isa_thumb.tcc"
OpAdc<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7368 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7372 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7375 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 7379 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 7382 "isa_thumb.tcc"
)
{
#line 48 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "adc r" << rd << ", r" << rm;
	}
#line 7389 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7394 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7397 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7401 "isa_thumb.tcc"
OpAdc<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7405 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7409 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7412 "isa_thumb.tcc"
)
{
#line 52 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res;
		typename CONFIG::reg_t s1, s2;
		uint8_t carry_in, carry_out, overflow_out;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		carry_in = (cpu.GetCPSR_C() ? 1 : 0);
		s1 = cpu.GetGPR(rd);
		s2 = cpu.GetGPR(rm);
		Add32(res, carry_out, overflow_out,
		s1, s2, carry_in);
		cpu.SetGPR(rd, res);

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
		cpu.SetCPSR_C(carry_out); // to check
		cpu.SetCPSR_V(overflow_out); // to check
	}
#line 7435 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7441 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7444 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7449 "isa_thumb.tcc"
> *DecodeOpAdc(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAdc<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 7455 "isa_thumb.tcc"
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
#line 7469 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7472 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7476 "isa_thumb.tcc"
OpAdd_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7480 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7484 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7487 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 7491 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 7494 "isa_thumb.tcc"
)
{
#line 83 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "add r" << rd << ", r" << rn << ", #" << dec << imm;
	}
#line 7501 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7506 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7509 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7513 "isa_thumb.tcc"
OpAdd_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7517 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7521 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7524 "isa_thumb.tcc"
)
{
#line 87 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res;
		typename CONFIG::reg_t s1;
		uint8_t carry_in, carry_out, overflow_out;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(rn);
		carry_in = 0;
		Add32(res, carry_out, overflow_out,
		s1, imm, carry_in);
		cpu.SetGPR(rd, res);

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
		cpu.SetCPSR_C(carry_out); // to check
		cpu.SetCPSR_V(overflow_out); // to check
	}
#line 7546 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7552 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7555 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7560 "isa_thumb.tcc"
> *DecodeOpAdd_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAdd_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 7566 "isa_thumb.tcc"
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
#line 7579 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7582 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7586 "isa_thumb.tcc"
OpAdd_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7590 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7594 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7597 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 7601 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 7604 "isa_thumb.tcc"
)
{
#line 116 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "add r" << rd << ", #" << dec << imm;
	}
#line 7611 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7616 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7619 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7623 "isa_thumb.tcc"
OpAdd_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7627 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7631 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7634 "isa_thumb.tcc"
)
{
#line 120 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res;
		typename CONFIG::reg_t s1;
		uint8_t carry_in, carry_out, overflow_out;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(rd);
		carry_in = 0;
		Add32(res, carry_out, overflow_out,
		s1, imm, carry_in);

		cpu.SetGPR(rd, res);
		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
		cpu.SetCPSR_C(carry_out); // to check
		cpu.SetCPSR_V(overflow_out); // to check
	}
#line 7656 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7662 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7665 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7670 "isa_thumb.tcc"
> *DecodeOpAdd_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAdd_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 7676 "isa_thumb.tcc"
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
#line 7689 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7692 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7696 "isa_thumb.tcc"
OpAdd_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7700 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7704 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7707 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 7711 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 7714 "isa_thumb.tcc"
)
{
#line 149 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "add r" << rd << ", r" << rn << ", r" << rm;
	}
#line 7721 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7726 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7729 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7733 "isa_thumb.tcc"
OpAdd_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7737 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7741 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7744 "isa_thumb.tcc"
)
{
#line 153 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res;
		typename CONFIG::reg_t s1, s2;
		uint8_t carry_in, carry_out, overflow_out;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(rm);
		s2 = cpu.GetGPR(rn);

		carry_in = 0;
		Add32(res, carry_out, overflow_out,
		s1, s2, carry_in);

		cpu.SetGPR(rd, res);
		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
		cpu.SetCPSR_C(carry_out); // to check
		cpu.SetCPSR_V(overflow_out); // to check
	}
#line 7768 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7774 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7777 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7782 "isa_thumb.tcc"
> *DecodeOpAdd_3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAdd_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 7788 "isa_thumb.tcc"
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
#line 7801 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7804 "isa_thumb.tcc"
>

void
OpAdd_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7811 "isa_thumb.tcc"
>::initialize_rm_index()
{
#line 184 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::rm_index = (h2 << 3) + rm;
	}
#line 7820 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7825 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7828 "isa_thumb.tcc"
>

void
OpAdd_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7835 "isa_thumb.tcc"
>::initialize_rd_index()
{
#line 190 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::rd_index = (h1 << 3) + rd;
	}
#line 7844 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7849 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7852 "isa_thumb.tcc"
>

void
OpAdd_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7859 "isa_thumb.tcc"
>::initialize_rm_init()
{
#line 196 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		if((h2 << 3) + rm == CPU<CONFIG>::PC_reg)
		inherited::rm_init = 4;
	}
#line 7869 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7874 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7877 "isa_thumb.tcc"
>

void
OpAdd_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7884 "isa_thumb.tcc"
>::initialize_rd_init()
{
#line 203 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		if((h1 << 3) + rd == CPU<CONFIG>::PC_reg)
		inherited::rd_init = 4;
	}
#line 7894 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7899 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7902 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7906 "isa_thumb.tcc"
OpAdd_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7910 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7914 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7917 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 7921 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 7924 "isa_thumb.tcc"
)
{
#line 210 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		buffer << "add r" << inherited::rd_index << ", r" << inherited::rm_index;
	}
#line 7933 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7938 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7941 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7945 "isa_thumb.tcc"
OpAdd_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7949 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7953 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7956 "isa_thumb.tcc"
)
{
#line 216 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		typename CONFIG::reg_t s1, s2, res;
		s1 = cpu.GetGPR(inherited::rd_index) + inherited::rd_init;
		s2 = cpu.GetGPR(inherited::rm_index) + inherited::rm_init;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		res = s1 + s2;
		cpu.SetGPR(inherited::rd_index, res);
	}
#line 7972 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 7978 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7981 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 7986 "isa_thumb.tcc"
> *DecodeOpAdd_4(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAdd_4<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 7992 "isa_thumb.tcc"
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
#line 8005 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8008 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8012 "isa_thumb.tcc"
OpAdd_5<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8016 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8020 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8023 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8027 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8030 "isa_thumb.tcc"
)
{
#line 239 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "add r" << rd << ", PC, #" << dec << ((unsigned int)imm * 4);
	}
#line 8037 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8042 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8045 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8049 "isa_thumb.tcc"
OpAdd_5<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8053 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8057 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8060 "isa_thumb.tcc"
)
{
#line 243 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 8077 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8083 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8086 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8091 "isa_thumb.tcc"
> *DecodeOpAdd_5(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAdd_5<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 8097 "isa_thumb.tcc"
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
#line 8110 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8113 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8117 "isa_thumb.tcc"
OpAdd_6<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8121 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8125 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8128 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8132 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8135 "isa_thumb.tcc"
)
{
#line 267 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "add r" << rd << ", SP, #" << dec << ((unsigned int)imm * 4);
	}
#line 8142 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8147 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8150 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8154 "isa_thumb.tcc"
OpAdd_6<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8158 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8162 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8165 "isa_thumb.tcc"
)
{
#line 271 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t s1, s2, res;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(cpu.SP_reg) & (typename CONFIG::reg_t)0xfffffffc;
		s2 = ((typename CONFIG::reg_t)imm) << 2;
		res = s1 + s2;

		cpu.SetGPR(rd, res);
	}
#line 8180 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8186 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8189 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8194 "isa_thumb.tcc"
> *DecodeOpAdd_6(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAdd_6<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 8200 "isa_thumb.tcc"
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
#line 8213 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8216 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8220 "isa_thumb.tcc"
OpAdd_7<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8224 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8228 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8231 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8235 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8238 "isa_thumb.tcc"
)
{
#line 293 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "add SP, #" << (unsigned int)imm * 4;
	}
#line 8245 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8250 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8253 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8257 "isa_thumb.tcc"
OpAdd_7<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8261 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8265 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8268 "isa_thumb.tcc"
)
{
#line 297 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		cpu.SetGPR(cpu.SP_reg, cpu.GetGPR(cpu.SP_reg) + ((typename CONFIG::reg_t)imm * 4));
	}
#line 8277 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8283 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8286 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8291 "isa_thumb.tcc"
> *DecodeOpAdd_7(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAdd_7<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 8297 "isa_thumb.tcc"
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
#line 8310 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8313 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8317 "isa_thumb.tcc"
OpAnd<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8321 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8325 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8328 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8332 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8335 "isa_thumb.tcc"
)
{
#line 313 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "and r" << rd << ", r" << rm;
	}
#line 8342 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8347 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8350 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8354 "isa_thumb.tcc"
OpAnd<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8358 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8362 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8365 "isa_thumb.tcc"
)
{
#line 317 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		res = cpu.GetGPR(rd) & cpu.GetGPR(rm);
		cpu.SetGPR(rd,res);

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
	}
#line 8380 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8386 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8389 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8394 "isa_thumb.tcc"
> *DecodeOpAnd(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAnd<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 8400 "isa_thumb.tcc"
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
#line 8413 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8416 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8420 "isa_thumb.tcc"
OpAsr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8424 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8428 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8431 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8435 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8438 "isa_thumb.tcc"
)
{
#line 339 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "asr r" << rd << ", r" << rm << ", #" << dec << imm;
	}
#line 8445 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8450 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8453 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8457 "isa_thumb.tcc"
OpAsr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8461 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8465 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8468 "isa_thumb.tcc"
)
{
#line 343 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 8496 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8502 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8505 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8510 "isa_thumb.tcc"
> *DecodeOpAsr_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAsr_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 8516 "isa_thumb.tcc"
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
#line 8529 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8532 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8536 "isa_thumb.tcc"
OpAsr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8540 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8544 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8547 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8551 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8554 "isa_thumb.tcc"
)
{
#line 378 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "asr r" << rd << ", r" << rs;
	}
#line 8561 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8566 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8569 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8573 "isa_thumb.tcc"
OpAsr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8577 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8581 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8584 "isa_thumb.tcc"
)
{
#line 382 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 8616 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8622 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8625 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8630 "isa_thumb.tcc"
> *DecodeOpAsr_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAsr_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 8636 "isa_thumb.tcc"
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
#line 8649 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8652 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8656 "isa_thumb.tcc"
OpBic<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8660 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8664 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8667 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8671 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8674 "isa_thumb.tcc"
)
{
#line 421 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "bic r" << rd << ", r" << rm;
	}
#line 8681 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8686 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8689 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8693 "isa_thumb.tcc"
OpBic<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8697 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8701 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8704 "isa_thumb.tcc"
)
{
#line 425 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res, s1, s2;
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(rd);
		s2 = cpu.GetGPR(rm);

		res = s1 & ~s2;

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
	}
#line 8720 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8726 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8729 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8734 "isa_thumb.tcc"
> *DecodeOpBic(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBic<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 8740 "isa_thumb.tcc"
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
#line 8753 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8756 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8760 "isa_thumb.tcc"
OpCmn<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8764 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8768 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8771 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8775 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8778 "isa_thumb.tcc"
)
{
#line 448 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "cmn r" << rn << ", r" << rm;
	}
#line 8785 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8790 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8793 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8797 "isa_thumb.tcc"
OpCmn<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8801 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8805 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8808 "isa_thumb.tcc"
)
{
#line 452 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res, s1, s2;
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);
		uint8_t carry_in, carry_out, overflow_out;

		s1 = cpu.GetGPR(rn);
		s2 = cpu.GetGPR(rm);

		carry_in = 0;
		Add32(res, carry_out, overflow_out,
		s1, s2, carry_in);

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
		cpu.SetCPSR_C(carry_out); // to check
		cpu.SetCPSR_V(overflow_out); // to check
	}
#line 8829 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8835 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8838 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8843 "isa_thumb.tcc"
> *DecodeOpCmn(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCmn<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 8849 "isa_thumb.tcc"
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
#line 8862 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8865 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8869 "isa_thumb.tcc"
OpCmp_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8873 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8877 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8880 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8884 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8887 "isa_thumb.tcc"
)
{
#line 479 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "cmp r" << rn << ", #" << dec << imm;
	}
#line 8894 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8899 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8902 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8906 "isa_thumb.tcc"
OpCmp_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8910 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8914 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8917 "isa_thumb.tcc"
)
{
#line 483 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 8935 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 8941 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8944 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8949 "isa_thumb.tcc"
> *DecodeOpCmp_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCmp_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 8955 "isa_thumb.tcc"
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
#line 8968 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8971 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8975 "isa_thumb.tcc"
OpCmp_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 8979 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8983 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8986 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8990 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8993 "isa_thumb.tcc"
)
{
#line 507 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "cmp r" << rn << ", r" << rm;
	}
#line 9000 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9005 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9008 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9012 "isa_thumb.tcc"
OpCmp_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9016 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9020 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9023 "isa_thumb.tcc"
)
{
#line 511 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 9041 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9047 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9050 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9055 "isa_thumb.tcc"
> *DecodeOpCmp_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCmp_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 9061 "isa_thumb.tcc"
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
#line 9074 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9077 "isa_thumb.tcc"
>

void
OpCmp_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9084 "isa_thumb.tcc"
>::initialize_rm_index()
{
#line 536 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::rm_index = (h2 << 3) + rm;
	}
#line 9093 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9098 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9101 "isa_thumb.tcc"
>

void
OpCmp_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9108 "isa_thumb.tcc"
>::initialize_rn_index()
{
#line 542 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::rn_index = (h1 << 3) + rn;
	}
#line 9117 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9122 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9125 "isa_thumb.tcc"
>

void
OpCmp_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9132 "isa_thumb.tcc"
>::initialize_rm_init()
{
#line 548 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		if((h2 << 3) + rm == CPU<CONFIG>::PC_reg)
		inherited::rm_init = 4;
		else
		inherited::rm_init = 0;
	}
#line 9144 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9149 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9152 "isa_thumb.tcc"
>

void
OpCmp_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9159 "isa_thumb.tcc"
>::initialize_rn_init()
{
#line 557 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		if((h1 << 3) + rn == CPU<CONFIG>::PC_reg)
		inherited::rn_init = 4;
		else
		inherited::rn_init = 0;
	}
#line 9171 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9176 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9179 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9183 "isa_thumb.tcc"
OpCmp_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9187 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9191 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9194 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 9198 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 9201 "isa_thumb.tcc"
)
{
#line 566 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		buffer << "cmp r" << inherited::rn_index << ", r" << inherited::rm_index;
	}
#line 9210 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9215 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9218 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9222 "isa_thumb.tcc"
OpCmp_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9226 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9230 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9233 "isa_thumb.tcc"
)
{
#line 572 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 9252 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9258 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9261 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9266 "isa_thumb.tcc"
> *DecodeOpCmp_3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCmp_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 9272 "isa_thumb.tcc"
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
#line 9285 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9288 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9292 "isa_thumb.tcc"
OpEor<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9296 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9300 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9303 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 9307 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 9310 "isa_thumb.tcc"
)
{
#line 598 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "eor r" << rd << ", r" << rm;
	}
#line 9317 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9322 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9325 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9329 "isa_thumb.tcc"
OpEor<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9333 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9337 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9340 "isa_thumb.tcc"
)
{
#line 602 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res, s1, s2;
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(rd);
		s2 = cpu.GetGPR(rm);

		res = s1 ^ s2;

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
	}
#line 9356 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9362 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9365 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9370 "isa_thumb.tcc"
> *DecodeOpEor(CodeType code, typename CONFIG::address_t addr)
{
	return new OpEor<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 9376 "isa_thumb.tcc"
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
#line 9389 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9392 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9396 "isa_thumb.tcc"
OpLsl_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9400 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9404 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9407 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 9411 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 9414 "isa_thumb.tcc"
)
{
#line 624 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "lsl r" << rd << ", r" << rm << ", #" << dec << imm;
	}
#line 9421 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9426 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9429 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9433 "isa_thumb.tcc"
OpLsl_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9437 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9441 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9444 "isa_thumb.tcc"
)
{
#line 628 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 9464 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9470 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9473 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9478 "isa_thumb.tcc"
> *DecodeOpLsl_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLsl_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 9484 "isa_thumb.tcc"
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
#line 9497 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9500 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9504 "isa_thumb.tcc"
OpLsl_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9508 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9512 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9515 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 9519 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 9522 "isa_thumb.tcc"
)
{
#line 655 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "lsl r" << rd << ", r" << rs;
	}
#line 9529 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9534 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9537 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9541 "isa_thumb.tcc"
OpLsl_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9545 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9549 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9552 "isa_thumb.tcc"
)
{
#line 659 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 9578 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9584 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9587 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9592 "isa_thumb.tcc"
> *DecodeOpLsl_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLsl_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 9598 "isa_thumb.tcc"
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
#line 9611 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9614 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9618 "isa_thumb.tcc"
OpLsr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9622 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9626 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9629 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 9633 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 9636 "isa_thumb.tcc"
)
{
#line 692 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "lsr r" << rd << ", r" << rm << ", #" << dec << imm;
	}
#line 9643 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9648 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9651 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9655 "isa_thumb.tcc"
OpLsr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9659 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9663 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9666 "isa_thumb.tcc"
)
{
#line 696 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 9687 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9693 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9696 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9701 "isa_thumb.tcc"
> *DecodeOpLsr_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLsr_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 9707 "isa_thumb.tcc"
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
#line 9720 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9723 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9727 "isa_thumb.tcc"
OpLsr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9731 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9735 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9738 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 9742 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 9745 "isa_thumb.tcc"
)
{
#line 724 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "lsr r" << rd << ", r" << rs;
	}
#line 9752 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9757 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9760 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9764 "isa_thumb.tcc"
OpLsr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9768 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9772 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9775 "isa_thumb.tcc"
)
{
#line 728 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 9803 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9809 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9812 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9817 "isa_thumb.tcc"
> *DecodeOpLsr_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLsr_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 9823 "isa_thumb.tcc"
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
#line 9836 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9839 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9843 "isa_thumb.tcc"
OpMov_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9847 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9851 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9854 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 9858 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 9861 "isa_thumb.tcc"
)
{
#line 763 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "mov r" << rd << ", #" << dec << imm;
	}
#line 9868 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9873 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9876 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9880 "isa_thumb.tcc"
OpMov_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9884 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9888 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9891 "isa_thumb.tcc"
)
{
#line 767 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res;
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		res = imm;

		cpu.SetGPR(rd, res);

		cpu.SetCPSR_N(false);
		cpu.SetCPSR_Z(res == 0);
	}
#line 9906 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9912 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9915 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9920 "isa_thumb.tcc"
> *DecodeOpMov_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMov_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 9926 "isa_thumb.tcc"
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
#line 9948 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9951 "isa_thumb.tcc"
>

void
OpMov_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9958 "isa_thumb.tcc"
>::initialize_rm_index()
{
#line 799 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::rm_index = (h2 << 3) + rm;
	}
#line 9967 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9972 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9975 "isa_thumb.tcc"
>

void
OpMov_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9982 "isa_thumb.tcc"
>::initialize_rd_index()
{
#line 805 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::rd_index = (h1 << 3) + rd;
	}
#line 9991 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 9996 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 9999 "isa_thumb.tcc"
>

void
OpMov_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10006 "isa_thumb.tcc"
>::initialize_rm_init()
{
#line 811 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		if((h2 << 3) + rm == CPU<CONFIG>::PC_reg)
		inherited::rm_init = 4;
		else
		inherited::rm_init = 0;
	}
#line 10018 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10023 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10026 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10030 "isa_thumb.tcc"
OpMov_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10034 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10038 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10041 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10045 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10048 "isa_thumb.tcc"
)
{
#line 820 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		buffer << "mov r" << inherited::rd_index << ", r" << inherited::rm_index;
	}
#line 10057 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10062 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10065 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10069 "isa_thumb.tcc"
OpMov_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10073 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10077 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10080 "isa_thumb.tcc"
)
{
#line 826 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;
		typename CONFIG::reg_t res;

		res = inherited::rm_init + cpu.GetGPR(inherited::rm_index);

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		cpu.SetGPR(inherited::rd_index, res);
	}
#line 10094 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10100 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10103 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10108 "isa_thumb.tcc"
> *DecodeOpMov_3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMov_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 10114 "isa_thumb.tcc"
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
#line 10127 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10130 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10134 "isa_thumb.tcc"
OpMul<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10138 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10142 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10145 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10149 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10152 "isa_thumb.tcc"
)
{
#line 846 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "mul r" << rd << ", r" << rm;
	}
#line 10159 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10164 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10167 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10171 "isa_thumb.tcc"
OpMul<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10175 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10179 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10182 "isa_thumb.tcc"
)
{
#line 850 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res;
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		res = cpu.GetGPR(rd) * cpu.GetGPR(rm);
		cpu.SetGPR(rd, res);

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
	}
#line 10196 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10202 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10205 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10210 "isa_thumb.tcc"
> *DecodeOpMul(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMul<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 10216 "isa_thumb.tcc"
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
#line 10229 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10232 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10236 "isa_thumb.tcc"
OpMvn<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10240 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10244 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10247 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10251 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10254 "isa_thumb.tcc"
)
{
#line 871 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "mvn r" << rd << ", r" << rm;
	}
#line 10261 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10266 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10269 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10273 "isa_thumb.tcc"
OpMvn<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10277 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10281 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10284 "isa_thumb.tcc"
)
{
#line 875 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res;
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		res = ~(cpu.GetGPR(rm));
		cpu.SetGPR(rd, res);

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
	}
#line 10298 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10304 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10307 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10312 "isa_thumb.tcc"
> *DecodeOpMvn(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMvn<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 10318 "isa_thumb.tcc"
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
#line 10331 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10334 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10338 "isa_thumb.tcc"
OpNeg<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10342 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10346 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10349 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10353 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10356 "isa_thumb.tcc"
)
{
#line 896 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "neg r" << rd << ", r" << rm;
	}
#line 10363 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10368 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10371 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10375 "isa_thumb.tcc"
OpNeg<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10379 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10383 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10386 "isa_thumb.tcc"
)
{
#line 900 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 10404 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10410 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10413 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10418 "isa_thumb.tcc"
> *DecodeOpNeg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNeg<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 10424 "isa_thumb.tcc"
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
#line 10437 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10440 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10444 "isa_thumb.tcc"
OpOrr<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10448 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10452 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10455 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10459 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10462 "isa_thumb.tcc"
)
{
#line 925 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "orr r" << rd << ", r" << rm;
	}
#line 10469 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10474 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10477 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10481 "isa_thumb.tcc"
OpOrr<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10485 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10489 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10492 "isa_thumb.tcc"
)
{
#line 929 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 10508 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10514 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10517 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10522 "isa_thumb.tcc"
> *DecodeOpOrr(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOrr<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 10528 "isa_thumb.tcc"
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
#line 10541 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10544 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10548 "isa_thumb.tcc"
OpRor<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10552 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10556 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10559 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10563 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10566 "isa_thumb.tcc"
)
{
#line 952 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "ror r" << rd << ", r" << rs;
	}
#line 10573 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10578 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10581 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10585 "isa_thumb.tcc"
OpRor<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10589 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10593 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10596 "isa_thumb.tcc"
)
{
#line 956 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 10621 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10627 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10630 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10635 "isa_thumb.tcc"
> *DecodeOpRor(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRor<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 10641 "isa_thumb.tcc"
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
#line 10654 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10657 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10661 "isa_thumb.tcc"
OpSbc<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10665 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10669 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10672 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10676 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10679 "isa_thumb.tcc"
)
{
#line 988 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "sbc r" << rd << ", r" << rm;
	}
#line 10686 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10691 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10694 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10698 "isa_thumb.tcc"
OpSbc<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10702 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10706 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10709 "isa_thumb.tcc"
)
{
#line 992 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 10731 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10737 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10740 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10745 "isa_thumb.tcc"
> *DecodeOpSbc(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSbc<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 10751 "isa_thumb.tcc"
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
#line 10764 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10767 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10771 "isa_thumb.tcc"
OpSub_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10775 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10779 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10782 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10786 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10789 "isa_thumb.tcc"
)
{
#line 1021 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "sub r" << rd << ", r" << rn << ", #" << dec << imm;
	}
#line 10796 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10801 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10804 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10808 "isa_thumb.tcc"
OpSub_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10812 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10816 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10819 "isa_thumb.tcc"
)
{
#line 1025 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 10839 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10845 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10848 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10853 "isa_thumb.tcc"
> *DecodeOpSub_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSub_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 10859 "isa_thumb.tcc"
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
#line 10872 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10875 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10879 "isa_thumb.tcc"
OpSub_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10883 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10887 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10890 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10894 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10897 "isa_thumb.tcc"
)
{
#line 1052 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "sub r" << rd << ", #" << dec << imm;
	}
#line 10904 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10909 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10912 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10916 "isa_thumb.tcc"
OpSub_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10920 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10924 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10927 "isa_thumb.tcc"
)
{
#line 1056 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 10949 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 10955 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10958 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10963 "isa_thumb.tcc"
> *DecodeOpSub_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSub_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 10969 "isa_thumb.tcc"
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
#line 10982 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10985 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10989 "isa_thumb.tcc"
OpSub_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 10993 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10997 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11000 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11004 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11007 "isa_thumb.tcc"
)
{
#line 1085 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "sub r" << rd << ", r" << rn << ", r" << rm;
	}
#line 11014 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11019 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11022 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11026 "isa_thumb.tcc"
OpSub_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11030 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11034 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11037 "isa_thumb.tcc"
)
{
#line 1089 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 11057 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11063 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11066 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11071 "isa_thumb.tcc"
> *DecodeOpSub_3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSub_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 11077 "isa_thumb.tcc"
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
#line 11090 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11093 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11097 "isa_thumb.tcc"
OpSub_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11101 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11105 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11108 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11112 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11115 "isa_thumb.tcc"
)
{
#line 1116 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "sub sp, #" << (imm * 4);
	}
#line 11122 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11127 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11130 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11134 "isa_thumb.tcc"
OpSub_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11138 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11142 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11145 "isa_thumb.tcc"
)
{
#line 1120 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t s1, s2;
		typename CONFIG::reg_t res;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(cpu.SP_reg);
		s2 = imm * 4;
		res = s1 - s2;
		cpu.SetGPR(cpu.SP_reg, res);
	}
#line 11160 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11166 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11169 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11174 "isa_thumb.tcc"
> *DecodeOpSub_4(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSub_4<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 11180 "isa_thumb.tcc"
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
#line 11193 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11196 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11200 "isa_thumb.tcc"
OpTst<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11204 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11208 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11211 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11215 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11218 "isa_thumb.tcc"
)
{
#line 1142 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "tst r" << rn << ", r" << rm;
	}
#line 11225 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11230 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11233 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11237 "isa_thumb.tcc"
OpTst<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11241 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11245 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11248 "isa_thumb.tcc"
)
{
#line 1146 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res, s1, s2;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(rn);
		s2 = cpu.GetGPR(rm);
		res = s1 & s2;

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
	}
#line 11264 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11270 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11273 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11278 "isa_thumb.tcc"
> *DecodeOpTst(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTst<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 11284 "isa_thumb.tcc"
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
#line 11335 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11338 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11342 "isa_thumb.tcc"
OpLdr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11346 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11350 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11353 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11357 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11360 "isa_thumb.tcc"
)
{
#line 47 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldr r" << rd << ", [r" << rn << ", #" << dec << (imm * 4) << "]";
	}
#line 11367 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11372 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11375 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11379 "isa_thumb.tcc"
OpLdr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11383 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11387 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11390 "isa_thumb.tcc"
)
{
#line 51 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		typename CONFIG::address_t addr;
		typename CONFIG::reg_t rn_value, value;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		rn_value = cpu.GetGPR(rn);
		addr = rn_value + (imm * 4);
		value = 0;
		cpu.Read32toGPRAligned(addr, rd);
	}
#line 11405 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11411 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11414 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11419 "isa_thumb.tcc"
> *DecodeOpLdr_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdr_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 11425 "isa_thumb.tcc"
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
#line 11438 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11441 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11445 "isa_thumb.tcc"
OpLdr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11449 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11453 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11456 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11460 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11463 "isa_thumb.tcc"
)
{
#line 73 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldr r" << rd << ", [r" << rn << ", r" << rm << "]";
	}
#line 11470 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11475 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11478 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11482 "isa_thumb.tcc"
OpLdr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11486 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11490 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11493 "isa_thumb.tcc"
)
{
#line 77 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
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
#line 11509 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11515 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11518 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11523 "isa_thumb.tcc"
> *DecodeOpLdr_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdr_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 11529 "isa_thumb.tcc"
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
#line 11542 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11545 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11549 "isa_thumb.tcc"
OpLdr_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11553 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11557 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11560 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11564 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11567 "isa_thumb.tcc"
)
{
#line 100 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldr r" << rd << ", [pc, #" << dec << (imm * 4) << "]";
	}
#line 11574 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11579 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11582 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11586 "isa_thumb.tcc"
OpLdr_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11590 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11594 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11597 "isa_thumb.tcc"
)
{
#line 104 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		typename CONFIG::address_t addr;
		typename CONFIG::reg_t pc_value, value;

		pc_value = (cpu.GetGPR(cpu.PC_reg) + 4) & ~((typename CONFIG::reg_t)0x03);

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		addr = pc_value + (imm * 4);
		value = 0;
		cpu.Read32toGPRAligned(addr, rd);
	}
#line 11613 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11619 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11622 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11627 "isa_thumb.tcc"
> *DecodeOpLdr_3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdr_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 11633 "isa_thumb.tcc"
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
#line 11646 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11649 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11653 "isa_thumb.tcc"
OpLdr_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11657 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11661 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11664 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11668 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11671 "isa_thumb.tcc"
)
{
#line 127 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldr r" << rd << ", [sp, #" << dec << (imm * 4) << "]";
	}
#line 11678 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11683 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11686 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11690 "isa_thumb.tcc"
OpLdr_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11694 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11698 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11701 "isa_thumb.tcc"
)
{
#line 131 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		typename CONFIG::address_t addr;
		typename CONFIG::reg_t sp_value, value;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		sp_value = cpu.GetGPR(cpu.SP_reg);
		addr = sp_value + (imm * 4);
		value = 0;
		cpu.Read32toGPRAligned(addr, rd);
	}
#line 11716 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11722 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11725 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11730 "isa_thumb.tcc"
> *DecodeOpLdr_4(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdr_4<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 11736 "isa_thumb.tcc"
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
#line 11749 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11752 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11756 "isa_thumb.tcc"
OpLdrb_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11760 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11764 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11767 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11771 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11774 "isa_thumb.tcc"
)
{
#line 153 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldrb r" << rd << ", [r" << rn << ", #" << dec << imm << "]";
	}
#line 11781 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11786 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11789 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11793 "isa_thumb.tcc"
OpLdrb_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11797 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11801 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11804 "isa_thumb.tcc"
)
{
#line 157 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
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
#line 11820 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11826 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11829 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11834 "isa_thumb.tcc"
> *DecodeOpLdrb_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdrb_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 11840 "isa_thumb.tcc"
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
#line 11853 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11856 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11860 "isa_thumb.tcc"
OpLdrb_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11864 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11868 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11871 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11875 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11878 "isa_thumb.tcc"
)
{
#line 180 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldrb r" << rd << ", [r" << rn << ", r" << rm << "]";
	}
#line 11885 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11890 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11893 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11897 "isa_thumb.tcc"
OpLdrb_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11901 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11905 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11908 "isa_thumb.tcc"
)
{
#line 184 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
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
#line 11925 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11931 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11934 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11939 "isa_thumb.tcc"
> *DecodeOpLdrb_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdrb_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 11945 "isa_thumb.tcc"
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
#line 11958 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11961 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11965 "isa_thumb.tcc"
OpLdrh_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11969 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11973 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11976 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11980 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11983 "isa_thumb.tcc"
)
{
#line 208 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldrh r" << rd << ", [r" << rn << ", #" << dec << (imm * 2) << "]";
	}
#line 11990 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 11995 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 11998 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12002 "isa_thumb.tcc"
OpLdrh_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12006 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12010 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12013 "isa_thumb.tcc"
)
{
#line 212 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
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
#line 12029 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12035 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12038 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12043 "isa_thumb.tcc"
> *DecodeOpLdrh_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdrh_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 12049 "isa_thumb.tcc"
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
#line 12062 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12065 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12069 "isa_thumb.tcc"
OpLdrh_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12073 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12077 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12080 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12084 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12087 "isa_thumb.tcc"
)
{
#line 235 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldrh r" << rd << ", [r" << rn << ", r" << rm << "]";
	}
#line 12094 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12099 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12102 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12106 "isa_thumb.tcc"
OpLdrh_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12110 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12114 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12117 "isa_thumb.tcc"
)
{
#line 239 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
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
#line 12134 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12140 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12143 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12148 "isa_thumb.tcc"
> *DecodeOpLdrh_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdrh_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 12154 "isa_thumb.tcc"
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
#line 12167 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12170 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12174 "isa_thumb.tcc"
OpLdrsb<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12178 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12182 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12185 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12189 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12192 "isa_thumb.tcc"
)
{
#line 263 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldrsb r" << rd << ", [r" << rn << ", r" << rm << "]";
	}
#line 12199 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12204 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12207 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12211 "isa_thumb.tcc"
OpLdrsb<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12215 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12219 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12222 "isa_thumb.tcc"
)
{
#line 267 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
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
#line 12239 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12245 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12248 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12253 "isa_thumb.tcc"
> *DecodeOpLdrsb(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdrsb<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 12259 "isa_thumb.tcc"
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
#line 12272 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12275 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12279 "isa_thumb.tcc"
OpLdrsh<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12283 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12287 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12290 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12294 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12297 "isa_thumb.tcc"
)
{
#line 291 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldrsh r" << rd << ", [r" << rn << ", r" << rm << "]";
	}
#line 12304 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12309 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12312 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12316 "isa_thumb.tcc"
OpLdrsh<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12320 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12324 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12327 "isa_thumb.tcc"
)
{
#line 295 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
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
#line 12344 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12350 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12353 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12358 "isa_thumb.tcc"
> *DecodeOpLdrsh(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdrsh<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 12364 "isa_thumb.tcc"
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
#line 12377 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12380 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12384 "isa_thumb.tcc"
OpStr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12388 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12392 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12395 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12399 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12402 "isa_thumb.tcc"
)
{
#line 319 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "str r" << rd << ", [r" << rn << ", #" << (imm * 4) << "]";
	}
#line 12409 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12414 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12417 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12421 "isa_thumb.tcc"
OpStr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12425 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12429 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12432 "isa_thumb.tcc"
)
{
#line 323 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		typename CONFIG::address_t addr;
		typename CONFIG::reg_t rd_value, rn_value;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		rn_value = cpu.GetGPR(rn);
		addr = rn_value + (imm * 4);
		rd_value = cpu.GetGPR(rd);
		cpu.Write32(addr, rd_value);
	}
#line 12447 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12453 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12456 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12461 "isa_thumb.tcc"
> *DecodeOpStr_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpStr_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 12467 "isa_thumb.tcc"
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
#line 12480 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12483 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12487 "isa_thumb.tcc"
OpStr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12491 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12495 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12498 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12502 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12505 "isa_thumb.tcc"
)
{
#line 345 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "str r" << rd << ", [r" << rn << ", r" << rm << "]";
	}
#line 12512 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12517 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12520 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12524 "isa_thumb.tcc"
OpStr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12528 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12532 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12535 "isa_thumb.tcc"
)
{
#line 349 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
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
#line 12551 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12557 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12560 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12565 "isa_thumb.tcc"
> *DecodeOpStr_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpStr_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 12571 "isa_thumb.tcc"
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
#line 12584 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12587 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12591 "isa_thumb.tcc"
OpStr_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12595 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12599 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12602 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12606 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12609 "isa_thumb.tcc"
)
{
#line 372 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "str r" << rd << ", [sp, #" << (imm * 4) << "]";
	}
#line 12616 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12621 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12624 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12628 "isa_thumb.tcc"
OpStr_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12632 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12636 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12639 "isa_thumb.tcc"
)
{
#line 376 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		typename CONFIG::address_t addr;
		typename CONFIG::reg_t rd_value, sp_value;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		sp_value = cpu.GetGPR(cpu.SP_reg);
		addr = sp_value + (imm * 4);
		rd_value = cpu.GetGPR(rd);
		cpu.Write32(addr, rd_value);
	}
#line 12654 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12660 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12663 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12668 "isa_thumb.tcc"
> *DecodeOpStr_3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpStr_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 12674 "isa_thumb.tcc"
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
#line 12687 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12690 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12694 "isa_thumb.tcc"
OpStrb_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12698 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12702 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12705 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12709 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12712 "isa_thumb.tcc"
)
{
#line 398 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "strb r" << rd << ", [r" << rn << ", #" << dec << imm << "]";
	}
#line 12719 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12724 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12727 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12731 "isa_thumb.tcc"
OpStrb_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12735 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12739 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12742 "isa_thumb.tcc"
)
{
#line 402 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
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
#line 12759 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12765 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12768 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12773 "isa_thumb.tcc"
> *DecodeOpStrb_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpStrb_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 12779 "isa_thumb.tcc"
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
#line 12792 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12795 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12799 "isa_thumb.tcc"
OpStrb_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12803 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12807 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12810 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12814 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12817 "isa_thumb.tcc"
)
{
#line 426 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "strb r" << rd << ", [r" << rn << ", r" << rm << "]";
	}
#line 12824 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12829 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12832 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12836 "isa_thumb.tcc"
OpStrb_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12840 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12844 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12847 "isa_thumb.tcc"
)
{
#line 430 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
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
#line 12865 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12871 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12874 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12879 "isa_thumb.tcc"
> *DecodeOpStrb_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpStrb_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 12885 "isa_thumb.tcc"
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
#line 12898 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12901 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12905 "isa_thumb.tcc"
OpStrh_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12909 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12913 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12916 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12920 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12923 "isa_thumb.tcc"
)
{
#line 455 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "strh r" << rd << ", [r" << rn << ", #" << dec << (imm * 2) << "]";
	}
#line 12930 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12935 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12938 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12942 "isa_thumb.tcc"
OpStrh_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12946 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12950 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12953 "isa_thumb.tcc"
)
{
#line 459 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
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
#line 12970 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 12976 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12979 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 12984 "isa_thumb.tcc"
> *DecodeOpStrh_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpStrh_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 12990 "isa_thumb.tcc"
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
#line 13003 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13006 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13010 "isa_thumb.tcc"
OpStrh_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13014 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13018 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13021 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 13025 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 13028 "isa_thumb.tcc"
)
{
#line 483 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "strh r" << rd << ", [r" << rn << ", r" << rm << "]";
	}
#line 13035 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13040 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13043 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13047 "isa_thumb.tcc"
OpStrh_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13051 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13055 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13058 "isa_thumb.tcc"
)
{
#line 487 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
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
#line 13076 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13082 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13085 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13090 "isa_thumb.tcc"
> *DecodeOpStrh_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpStrh_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 13096 "isa_thumb.tcc"
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
#line 13147 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13150 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13154 "isa_thumb.tcc"
OpLdmia<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13158 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13162 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13165 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 13169 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 13172 "isa_thumb.tcc"
)
{
#line 47 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_multiple.isa"
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
#line 13191 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13196 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13199 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13203 "isa_thumb.tcc"
OpLdmia<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13207 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13211 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13214 "isa_thumb.tcc"
)
{
#line 63 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_multiple.isa"
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
#line 13244 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13250 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13253 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13258 "isa_thumb.tcc"
> *DecodeOpLdmia(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdmia<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 13264 "isa_thumb.tcc"
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
#line 13277 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13280 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13284 "isa_thumb.tcc"
OpPop<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13288 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13292 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13295 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 13299 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 13302 "isa_thumb.tcc"
)
{
#line 100 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_multiple.isa"
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
#line 13321 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13326 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13329 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13333 "isa_thumb.tcc"
OpPop<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13337 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13341 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13344 "isa_thumb.tcc"
)
{
#line 116 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_multiple.isa"
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
#line 13380 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13386 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13389 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13394 "isa_thumb.tcc"
> *DecodeOpPop(CodeType code, typename CONFIG::address_t addr)
{
	return new OpPop<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 13400 "isa_thumb.tcc"
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
#line 13413 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13416 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13420 "isa_thumb.tcc"
OpPush<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13424 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13428 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13431 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 13435 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 13438 "isa_thumb.tcc"
)
{
#line 159 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_multiple.isa"
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
#line 13461 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13466 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13469 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13473 "isa_thumb.tcc"
OpPush<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13477 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13481 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13484 "isa_thumb.tcc"
)
{
#line 179 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_multiple.isa"
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
#line 13523 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13529 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13532 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13537 "isa_thumb.tcc"
> *DecodeOpPush(CodeType code, typename CONFIG::address_t addr)
{
	return new OpPush<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 13543 "isa_thumb.tcc"
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
#line 13556 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13559 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13563 "isa_thumb.tcc"
OpStmia<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13567 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13571 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13574 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 13578 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 13581 "isa_thumb.tcc"
)
{
#line 225 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_multiple.isa"
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
#line 13600 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13605 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13608 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13612 "isa_thumb.tcc"
OpStmia<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13616 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13620 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13623 "isa_thumb.tcc"
)
{
#line 241 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_multiple.isa"
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
#line 13654 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13660 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13663 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13668 "isa_thumb.tcc"
> *DecodeOpStmia(CodeType code, typename CONFIG::address_t addr)
{
	return new OpStmia<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 13674 "isa_thumb.tcc"
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
#line 13725 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13728 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13732 "isa_thumb.tcc"
OpBkpt<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13736 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13740 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13743 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 13747 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 13750 "isa_thumb.tcc"
)
{
#line 47 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/exception.isa"
	{
		buffer << "bkpt #" << dec << imm;
	}
#line 13757 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13762 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13765 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13769 "isa_thumb.tcc"
OpBkpt<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13773 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13777 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13780 "isa_thumb.tcc"
)
{
#line 51 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/exception.isa"
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
#line 13804 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13810 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13813 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13818 "isa_thumb.tcc"
> *DecodeOpBkpt(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBkpt<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 13824 "isa_thumb.tcc"
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
#line 13837 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13840 "isa_thumb.tcc"
>
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13844 "isa_thumb.tcc"
OpSwi<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13848 "isa_thumb.tcc"
>::disasm(
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13852 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13855 "isa_thumb.tcc"
,
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 13859 "isa_thumb.tcc"
#line 39 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 13862 "isa_thumb.tcc"
)
{
#line 82 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/exception.isa"
	{
		buffer << "swi #" << dec << imm;
	}
#line 13869 "isa_thumb.tcc"
}
template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13874 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13877 "isa_thumb.tcc"
>
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13881 "isa_thumb.tcc"
OpSwi<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13885 "isa_thumb.tcc"
>::execute(
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13889 "isa_thumb.tcc"
#line 35 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13892 "isa_thumb.tcc"
)
{
#line 86 "/Users/gracia/Developer/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/exception.isa"
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
#line 13921 "isa_thumb.tcc"
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13927 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13930 "isa_thumb.tcc"
>
static Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13935 "isa_thumb.tcc"
> *DecodeOpSwi(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSwi<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 13941 "isa_thumb.tcc"
	>(code, addr);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13948 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13951 "isa_thumb.tcc"
>
OpB_1_0xxx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13956 "isa_thumb.tcc"
>::OpB_1_0xxx(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13960 "isa_thumb.tcc"
>(code, addr, "b_1_0xxx")
{
	sub_cond = ((code >> 8) & 0x7);
	imm = (((((int32_t)(code >> 0)) & 0xff) << 24) >> 24);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13970 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13973 "isa_thumb.tcc"
>
OpB_1_10xx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13978 "isa_thumb.tcc"
>::OpB_1_10xx(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13982 "isa_thumb.tcc"
>(code, addr, "b_1_10xx")
{
	sub_cond = ((code >> 8) & 0x3);
	imm = (((((int32_t)(code >> 0)) & 0xff) << 24) >> 24);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 13992 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 13995 "isa_thumb.tcc"
>
OpB_1_110x<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14000 "isa_thumb.tcc"
>::OpB_1_110x(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14004 "isa_thumb.tcc"
>(code, addr, "b_1_110x")
{
	sub_cond = ((code >> 8) & 0x1);
	imm = (((((int32_t)(code >> 0)) & 0xff) << 24) >> 24);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14014 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14017 "isa_thumb.tcc"
>
OpB_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14022 "isa_thumb.tcc"
>::OpB_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14026 "isa_thumb.tcc"
>(code, addr, "b_2")
{
	imm = (((((int32_t)(code >> 0)) & 0x7ff) << 21) >> 21);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14035 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14038 "isa_thumb.tcc"
>
OpBl<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14043 "isa_thumb.tcc"
>::OpBl(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14047 "isa_thumb.tcc"
>(code, addr, "bl")
{
	h2 = ((code >> 11) & 0x1);
	offset = ((code >> 0) & 0x7ff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14057 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14060 "isa_thumb.tcc"
>
OpBx<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14065 "isa_thumb.tcc"
>::OpBx(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14069 "isa_thumb.tcc"
>(code, addr, "bx")
{
	h2 = ((code >> 6) & 0x1);
	rm = ((code >> 3) & 0x7);
	sbz = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14080 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14083 "isa_thumb.tcc"
>
OpBlx_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14088 "isa_thumb.tcc"
>::OpBlx_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14092 "isa_thumb.tcc"
>(code, addr, "blx_1")
{
	offset = ((code >> 0) & 0x7ff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14101 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14104 "isa_thumb.tcc"
>
OpBlx_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14109 "isa_thumb.tcc"
>::OpBlx_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14113 "isa_thumb.tcc"
>(code, addr, "blx_2")
{
	h2 = ((code >> 6) & 0x1);
	rm = ((code >> 3) & 0x7);
	sbz = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14124 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14127 "isa_thumb.tcc"
>
OpAdc<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14132 "isa_thumb.tcc"
>::OpAdc(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14136 "isa_thumb.tcc"
>(code, addr, "adc")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14146 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14149 "isa_thumb.tcc"
>
OpAdd_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14154 "isa_thumb.tcc"
>::OpAdd_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14158 "isa_thumb.tcc"
>(code, addr, "add_1")
{
	imm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14169 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14172 "isa_thumb.tcc"
>
OpAdd_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14177 "isa_thumb.tcc"
>::OpAdd_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14181 "isa_thumb.tcc"
>(code, addr, "add_2")
{
	rd = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14191 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14194 "isa_thumb.tcc"
>
OpAdd_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14199 "isa_thumb.tcc"
>::OpAdd_3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14203 "isa_thumb.tcc"
>(code, addr, "add_3")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14214 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14217 "isa_thumb.tcc"
>
OpAdd_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14222 "isa_thumb.tcc"
>::OpAdd_4(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14226 "isa_thumb.tcc"
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
#line 14238 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14241 "isa_thumb.tcc"
>
OpAdd_5<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14246 "isa_thumb.tcc"
>::OpAdd_5(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14250 "isa_thumb.tcc"
>(code, addr, "add_5")
{
	rd = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14260 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14263 "isa_thumb.tcc"
>
OpAdd_6<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14268 "isa_thumb.tcc"
>::OpAdd_6(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14272 "isa_thumb.tcc"
>(code, addr, "add_6")
{
	rd = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14282 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14285 "isa_thumb.tcc"
>
OpAdd_7<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14290 "isa_thumb.tcc"
>::OpAdd_7(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14294 "isa_thumb.tcc"
>(code, addr, "add_7")
{
	imm = ((code >> 0) & 0x7f);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14303 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14306 "isa_thumb.tcc"
>
OpAnd<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14311 "isa_thumb.tcc"
>::OpAnd(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14315 "isa_thumb.tcc"
>(code, addr, "and")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14325 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14328 "isa_thumb.tcc"
>
OpAsr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14333 "isa_thumb.tcc"
>::OpAsr_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14337 "isa_thumb.tcc"
>(code, addr, "asr_1")
{
	imm = ((code >> 6) & 0x1f);
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14348 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14351 "isa_thumb.tcc"
>
OpAsr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14356 "isa_thumb.tcc"
>::OpAsr_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14360 "isa_thumb.tcc"
>(code, addr, "asr_2")
{
	rs = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14370 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14373 "isa_thumb.tcc"
>
OpBic<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14378 "isa_thumb.tcc"
>::OpBic(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14382 "isa_thumb.tcc"
>(code, addr, "bic")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14392 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14395 "isa_thumb.tcc"
>
OpCmn<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14400 "isa_thumb.tcc"
>::OpCmn(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14404 "isa_thumb.tcc"
>(code, addr, "cmn")
{
	rm = ((code >> 3) & 0x7);
	rn = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14414 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14417 "isa_thumb.tcc"
>
OpCmp_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14422 "isa_thumb.tcc"
>::OpCmp_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14426 "isa_thumb.tcc"
>(code, addr, "cmp_1")
{
	rn = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14436 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14439 "isa_thumb.tcc"
>
OpCmp_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14444 "isa_thumb.tcc"
>::OpCmp_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14448 "isa_thumb.tcc"
>(code, addr, "cmp_2")
{
	rm = ((code >> 3) & 0x7);
	rn = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14458 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14461 "isa_thumb.tcc"
>
OpCmp_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14466 "isa_thumb.tcc"
>::OpCmp_3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14470 "isa_thumb.tcc"
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
#line 14482 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14485 "isa_thumb.tcc"
>
OpEor<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14490 "isa_thumb.tcc"
>::OpEor(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14494 "isa_thumb.tcc"
>(code, addr, "eor")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14504 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14507 "isa_thumb.tcc"
>
OpLsl_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14512 "isa_thumb.tcc"
>::OpLsl_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14516 "isa_thumb.tcc"
>(code, addr, "lsl_1")
{
	imm = ((code >> 6) & 0x1f);
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14527 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14530 "isa_thumb.tcc"
>
OpLsl_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14535 "isa_thumb.tcc"
>::OpLsl_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14539 "isa_thumb.tcc"
>(code, addr, "lsl_2")
{
	rs = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14549 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14552 "isa_thumb.tcc"
>
OpLsr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14557 "isa_thumb.tcc"
>::OpLsr_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14561 "isa_thumb.tcc"
>(code, addr, "lsr_1")
{
	imm = ((code >> 6) & 0x1f);
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14572 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14575 "isa_thumb.tcc"
>
OpLsr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14580 "isa_thumb.tcc"
>::OpLsr_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14584 "isa_thumb.tcc"
>(code, addr, "lsr_2")
{
	rs = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14594 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14597 "isa_thumb.tcc"
>
OpMov_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14602 "isa_thumb.tcc"
>::OpMov_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14606 "isa_thumb.tcc"
>(code, addr, "mov_1")
{
	rd = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14616 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14619 "isa_thumb.tcc"
>
OpMov_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14624 "isa_thumb.tcc"
>::OpMov_3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14628 "isa_thumb.tcc"
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
#line 14640 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14643 "isa_thumb.tcc"
>
OpMul<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14648 "isa_thumb.tcc"
>::OpMul(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14652 "isa_thumb.tcc"
>(code, addr, "mul")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14662 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14665 "isa_thumb.tcc"
>
OpMvn<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14670 "isa_thumb.tcc"
>::OpMvn(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14674 "isa_thumb.tcc"
>(code, addr, "mvn")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14684 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14687 "isa_thumb.tcc"
>
OpNeg<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14692 "isa_thumb.tcc"
>::OpNeg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14696 "isa_thumb.tcc"
>(code, addr, "neg")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14706 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14709 "isa_thumb.tcc"
>
OpOrr<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14714 "isa_thumb.tcc"
>::OpOrr(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14718 "isa_thumb.tcc"
>(code, addr, "orr")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14728 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14731 "isa_thumb.tcc"
>
OpRor<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14736 "isa_thumb.tcc"
>::OpRor(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14740 "isa_thumb.tcc"
>(code, addr, "ror")
{
	rs = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14750 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14753 "isa_thumb.tcc"
>
OpSbc<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14758 "isa_thumb.tcc"
>::OpSbc(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14762 "isa_thumb.tcc"
>(code, addr, "sbc")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14772 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14775 "isa_thumb.tcc"
>
OpSub_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14780 "isa_thumb.tcc"
>::OpSub_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14784 "isa_thumb.tcc"
>(code, addr, "sub_1")
{
	imm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14795 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14798 "isa_thumb.tcc"
>
OpSub_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14803 "isa_thumb.tcc"
>::OpSub_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14807 "isa_thumb.tcc"
>(code, addr, "sub_2")
{
	rd = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14817 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14820 "isa_thumb.tcc"
>
OpSub_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14825 "isa_thumb.tcc"
>::OpSub_3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14829 "isa_thumb.tcc"
>(code, addr, "sub_3")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14840 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14843 "isa_thumb.tcc"
>
OpSub_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14848 "isa_thumb.tcc"
>::OpSub_4(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14852 "isa_thumb.tcc"
>(code, addr, "sub_4")
{
	imm = ((code >> 0) & 0x7f);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14861 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14864 "isa_thumb.tcc"
>
OpTst<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14869 "isa_thumb.tcc"
>::OpTst(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14873 "isa_thumb.tcc"
>(code, addr, "tst")
{
	rm = ((code >> 3) & 0x7);
	rn = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14883 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14886 "isa_thumb.tcc"
>
OpLdr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14891 "isa_thumb.tcc"
>::OpLdr_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14895 "isa_thumb.tcc"
>(code, addr, "ldr_1")
{
	imm = ((code >> 6) & 0x1f);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14906 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14909 "isa_thumb.tcc"
>
OpLdr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14914 "isa_thumb.tcc"
>::OpLdr_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14918 "isa_thumb.tcc"
>(code, addr, "ldr_2")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14929 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14932 "isa_thumb.tcc"
>
OpLdr_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14937 "isa_thumb.tcc"
>::OpLdr_3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14941 "isa_thumb.tcc"
>(code, addr, "ldr_3")
{
	rd = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14951 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14954 "isa_thumb.tcc"
>
OpLdr_4<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14959 "isa_thumb.tcc"
>::OpLdr_4(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14963 "isa_thumb.tcc"
>(code, addr, "ldr_4")
{
	rd = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14973 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14976 "isa_thumb.tcc"
>
OpLdrb_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14981 "isa_thumb.tcc"
>::OpLdrb_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14985 "isa_thumb.tcc"
>(code, addr, "ldrb_1")
{
	imm = ((code >> 6) & 0x1f);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 14996 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 14999 "isa_thumb.tcc"
>
OpLdrb_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15004 "isa_thumb.tcc"
>::OpLdrb_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15008 "isa_thumb.tcc"
>(code, addr, "ldrb_2")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15019 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15022 "isa_thumb.tcc"
>
OpLdrh_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15027 "isa_thumb.tcc"
>::OpLdrh_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15031 "isa_thumb.tcc"
>(code, addr, "ldrh_1")
{
	imm = ((code >> 6) & 0x1f);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15042 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15045 "isa_thumb.tcc"
>
OpLdrh_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15050 "isa_thumb.tcc"
>::OpLdrh_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15054 "isa_thumb.tcc"
>(code, addr, "ldrh_2")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15065 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15068 "isa_thumb.tcc"
>
OpLdrsb<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15073 "isa_thumb.tcc"
>::OpLdrsb(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15077 "isa_thumb.tcc"
>(code, addr, "ldrsb")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15088 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15091 "isa_thumb.tcc"
>
OpLdrsh<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15096 "isa_thumb.tcc"
>::OpLdrsh(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15100 "isa_thumb.tcc"
>(code, addr, "ldrsh")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15111 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15114 "isa_thumb.tcc"
>
OpStr_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15119 "isa_thumb.tcc"
>::OpStr_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15123 "isa_thumb.tcc"
>(code, addr, "str_1")
{
	imm = ((code >> 6) & 0x1f);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15134 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15137 "isa_thumb.tcc"
>
OpStr_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15142 "isa_thumb.tcc"
>::OpStr_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15146 "isa_thumb.tcc"
>(code, addr, "str_2")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15157 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15160 "isa_thumb.tcc"
>
OpStr_3<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15165 "isa_thumb.tcc"
>::OpStr_3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15169 "isa_thumb.tcc"
>(code, addr, "str_3")
{
	rd = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15179 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15182 "isa_thumb.tcc"
>
OpStrb_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15187 "isa_thumb.tcc"
>::OpStrb_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15191 "isa_thumb.tcc"
>(code, addr, "strb_1")
{
	imm = ((code >> 6) & 0x1f);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15202 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15205 "isa_thumb.tcc"
>
OpStrb_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15210 "isa_thumb.tcc"
>::OpStrb_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15214 "isa_thumb.tcc"
>(code, addr, "strb_2")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15225 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15228 "isa_thumb.tcc"
>
OpStrh_1<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15233 "isa_thumb.tcc"
>::OpStrh_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15237 "isa_thumb.tcc"
>(code, addr, "strh_1")
{
	imm = ((code >> 6) & 0x1f);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15248 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15251 "isa_thumb.tcc"
>
OpStrh_2<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15256 "isa_thumb.tcc"
>::OpStrh_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15260 "isa_thumb.tcc"
>(code, addr, "strh_2")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15271 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15274 "isa_thumb.tcc"
>
OpLdmia<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15279 "isa_thumb.tcc"
>::OpLdmia(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15283 "isa_thumb.tcc"
>(code, addr, "ldmia")
{
	rn = ((code >> 8) & 0x7);
	reg_list = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15293 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15296 "isa_thumb.tcc"
>
OpPop<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15301 "isa_thumb.tcc"
>::OpPop(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15305 "isa_thumb.tcc"
>(code, addr, "pop")
{
	r = ((code >> 8) & 0x1);
	reg_list = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15315 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15318 "isa_thumb.tcc"
>
OpPush<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15323 "isa_thumb.tcc"
>::OpPush(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15327 "isa_thumb.tcc"
>(code, addr, "push")
{
	r = ((code >> 8) & 0x1);
	reg_list = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15337 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15340 "isa_thumb.tcc"
>
OpStmia<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15345 "isa_thumb.tcc"
>::OpStmia(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15349 "isa_thumb.tcc"
>(code, addr, "stmia")
{
	rn = ((code >> 8) & 0x7);
	reg_list = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15359 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15362 "isa_thumb.tcc"
>
OpBkpt<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15367 "isa_thumb.tcc"
>::OpBkpt(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15371 "isa_thumb.tcc"
>(code, addr, "bkpt")
{
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15380 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15383 "isa_thumb.tcc"
>
OpSwi<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15388 "isa_thumb.tcc"
>::OpSwi(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15392 "isa_thumb.tcc"
>(code, addr, "swi")
{
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15401 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15404 "isa_thumb.tcc"
>
DecodeMapPage<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15409 "isa_thumb.tcc"
>::DecodeMapPage(typename CONFIG::address_t key)
{
	this->key = key;
	memset(operation, 0, sizeof(operation));
	next = 0;
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15420 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15423 "isa_thumb.tcc"
>
DecodeMapPage<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15428 "isa_thumb.tcc"
>::~DecodeMapPage()
{
	unsigned int idx;
	for(idx = 0; idx < NUM_OPERATIONS_PER_PAGE; idx++)
	delete operation[idx];
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15439 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15442 "isa_thumb.tcc"
>
DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15447 "isa_thumb.tcc"
>::DecodeTableEntry(CodeType opcode, CodeType opcode_mask, Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15451 "isa_thumb.tcc"
> *(*decode)(CodeType, typename CONFIG::address_t))
{
	this->opcode = opcode;
	this->opcode_mask = opcode_mask;
	this->decode = decode;
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 15462 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15465 "isa_thumb.tcc"
>
Decoder<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 15470 "isa_thumb.tcc"
>::Decoder()
: is_little_endian( false ), mru_page( 0 )
{
	memset(decode_hash_table, 0, sizeof(decode_hash_table));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15478 "isa_thumb.tcc"
	>(0xdf00U, 0xff00U, DecodeOpSwi<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15482 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15487 "isa_thumb.tcc"
	>(0xbe00U, 0xff00U, DecodeOpBkpt<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15491 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15496 "isa_thumb.tcc"
	>(0xc000U, 0xf800U, DecodeOpStmia<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15500 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15505 "isa_thumb.tcc"
	>(0xb400U, 0xfe00U, DecodeOpPush<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15509 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15514 "isa_thumb.tcc"
	>(0xbc00U, 0xfe00U, DecodeOpPop<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15518 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15523 "isa_thumb.tcc"
	>(0xc800U, 0xf800U, DecodeOpLdmia<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15527 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15532 "isa_thumb.tcc"
	>(0x5200U, 0xfe00U, DecodeOpStrh_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15536 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15541 "isa_thumb.tcc"
	>(0x8000U, 0xf800U, DecodeOpStrh_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15545 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15550 "isa_thumb.tcc"
	>(0x5400U, 0xfe00U, DecodeOpStrb_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15554 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15559 "isa_thumb.tcc"
	>(0x7000U, 0xf800U, DecodeOpStrb_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15563 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15568 "isa_thumb.tcc"
	>(0x9000U, 0xf800U, DecodeOpStr_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15572 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15577 "isa_thumb.tcc"
	>(0x5000U, 0xfe00U, DecodeOpStr_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15581 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15586 "isa_thumb.tcc"
	>(0x6000U, 0xf800U, DecodeOpStr_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15590 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15595 "isa_thumb.tcc"
	>(0x5e00U, 0xfe00U, DecodeOpLdrsh<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15599 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15604 "isa_thumb.tcc"
	>(0x5600U, 0xfe00U, DecodeOpLdrsb<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15608 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15613 "isa_thumb.tcc"
	>(0x5a00U, 0xfe00U, DecodeOpLdrh_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15617 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15622 "isa_thumb.tcc"
	>(0x8800U, 0xf800U, DecodeOpLdrh_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15626 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15631 "isa_thumb.tcc"
	>(0x5c00U, 0xfe00U, DecodeOpLdrb_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15635 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15640 "isa_thumb.tcc"
	>(0x7800U, 0xf800U, DecodeOpLdrb_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15644 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15649 "isa_thumb.tcc"
	>(0x9800U, 0xf800U, DecodeOpLdr_4<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15653 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15658 "isa_thumb.tcc"
	>(0x4800U, 0xf800U, DecodeOpLdr_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15662 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15667 "isa_thumb.tcc"
	>(0x5800U, 0xfe00U, DecodeOpLdr_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15671 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15676 "isa_thumb.tcc"
	>(0x6800U, 0xf800U, DecodeOpLdr_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15680 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15685 "isa_thumb.tcc"
	>(0x4200U, 0xffc0U, DecodeOpTst<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15689 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15694 "isa_thumb.tcc"
	>(0xb080U, 0xff80U, DecodeOpSub_4<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15698 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15703 "isa_thumb.tcc"
	>(0x1a00U, 0xfe00U, DecodeOpSub_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15707 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15712 "isa_thumb.tcc"
	>(0x3800U, 0xf800U, DecodeOpSub_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15716 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15721 "isa_thumb.tcc"
	>(0x1e00U, 0xfe00U, DecodeOpSub_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15725 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15730 "isa_thumb.tcc"
	>(0x4180U, 0xffc0U, DecodeOpSbc<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15734 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15739 "isa_thumb.tcc"
	>(0x41c0U, 0xffc0U, DecodeOpRor<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15743 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15748 "isa_thumb.tcc"
	>(0x4300U, 0xffc0U, DecodeOpOrr<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15752 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15757 "isa_thumb.tcc"
	>(0x4240U, 0xffc0U, DecodeOpNeg<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15761 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15766 "isa_thumb.tcc"
	>(0x43c0U, 0xffc0U, DecodeOpMvn<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15770 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15775 "isa_thumb.tcc"
	>(0x4340U, 0xffc0U, DecodeOpMul<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15779 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15784 "isa_thumb.tcc"
	>(0x4600U, 0xff00U, DecodeOpMov_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15788 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15793 "isa_thumb.tcc"
	>(0x2000U, 0xf800U, DecodeOpMov_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15797 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15802 "isa_thumb.tcc"
	>(0x40c0U, 0xffc0U, DecodeOpLsr_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15806 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15811 "isa_thumb.tcc"
	>(0x800U, 0xf800U, DecodeOpLsr_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15815 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15820 "isa_thumb.tcc"
	>(0x4080U, 0xffc0U, DecodeOpLsl_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15824 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15829 "isa_thumb.tcc"
	>(0x0U, 0xf800U, DecodeOpLsl_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15833 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15838 "isa_thumb.tcc"
	>(0x4040U, 0xffc0U, DecodeOpEor<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15842 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15847 "isa_thumb.tcc"
	>(0x4500U, 0xff00U, DecodeOpCmp_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15851 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15856 "isa_thumb.tcc"
	>(0x4280U, 0xffc0U, DecodeOpCmp_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15860 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15865 "isa_thumb.tcc"
	>(0x2800U, 0xf800U, DecodeOpCmp_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15869 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15874 "isa_thumb.tcc"
	>(0x42c0U, 0xffc0U, DecodeOpCmn<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15878 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15883 "isa_thumb.tcc"
	>(0x4380U, 0xffc0U, DecodeOpBic<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15887 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15892 "isa_thumb.tcc"
	>(0x4100U, 0xffc0U, DecodeOpAsr_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15896 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15901 "isa_thumb.tcc"
	>(0x1000U, 0xf800U, DecodeOpAsr_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15905 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15910 "isa_thumb.tcc"
	>(0x4000U, 0xffc0U, DecodeOpAnd<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15914 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15919 "isa_thumb.tcc"
	>(0xb000U, 0xff80U, DecodeOpAdd_7<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15923 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15928 "isa_thumb.tcc"
	>(0xa800U, 0xf800U, DecodeOpAdd_6<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15932 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15937 "isa_thumb.tcc"
	>(0xa000U, 0xf800U, DecodeOpAdd_5<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15941 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15946 "isa_thumb.tcc"
	>(0x4400U, 0xff00U, DecodeOpAdd_4<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15950 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15955 "isa_thumb.tcc"
	>(0x1800U, 0xfe00U, DecodeOpAdd_3<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15959 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15964 "isa_thumb.tcc"
	>(0x3000U, 0xf800U, DecodeOpAdd_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15968 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15973 "isa_thumb.tcc"
	>(0x1c00U, 0xfe00U, DecodeOpAdd_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15977 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15982 "isa_thumb.tcc"
	>(0x4140U, 0xffc0U, DecodeOpAdc<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15986 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15991 "isa_thumb.tcc"
	>(0x4780U, 0xff80U, DecodeOpBlx_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 15995 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16000 "isa_thumb.tcc"
	>(0xe800U, 0xf800U, DecodeOpBlx_1<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16004 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16009 "isa_thumb.tcc"
	>(0x4700U, 0xff80U, DecodeOpBx<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16013 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16018 "isa_thumb.tcc"
	>(0xf000U, 0xf000U, DecodeOpBl<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16022 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16027 "isa_thumb.tcc"
	>(0xe000U, 0xf800U, DecodeOpB_2<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16031 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16036 "isa_thumb.tcc"
	>(0xdc00U, 0xfe00U, DecodeOpB_1_110x<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16040 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16045 "isa_thumb.tcc"
	>(0xd800U, 0xfc00U, DecodeOpB_1_10xx<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16049 "isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16054 "isa_thumb.tcc"
	>(0xd000U, 0xf800U, DecodeOpB_1_0xxx<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16058 "isa_thumb.tcc"
	>));
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 16065 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16068 "isa_thumb.tcc"
>
Decoder<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16073 "isa_thumb.tcc"
>::~Decoder()
{
	InvalidateDecodingCache();
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 16082 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16085 "isa_thumb.tcc"
>
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16090 "isa_thumb.tcc"
> *Decoder<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16094 "isa_thumb.tcc"
>::NCDecode(typename CONFIG::address_t addr, CodeType code)
{
	Operation<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16100 "isa_thumb.tcc"
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
#line 16126 "isa_thumb.tcc"
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
#line 16145 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16148 "isa_thumb.tcc"
>
void Decoder<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16153 "isa_thumb.tcc"
>::InvalidateDecodingCache()
{
	uint32_t index;
	mru_page = 0;
	for(index = 0; index < NUM_DECODE_HASH_TABLE_ENTRIES; index++)
	{
		DecodeMapPage<
#line 44 "isa/thumb/thumb.isa"
		CONFIG
#line 16163 "isa_thumb.tcc"
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
#line 16182 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16185 "isa_thumb.tcc"
>
void Decoder<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16190 "isa_thumb.tcc"
>::InvalidateDecodingCacheEntry(typename CONFIG::address_t addr)
{
	typename CONFIG::address_t page_key = addr / 2 / NUM_OPERATIONS_PER_PAGE;
	if(mru_page && mru_page->key == page_key) mru_page = 0;
	uint32_t index = page_key % NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key
	DecodeMapPage<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16199 "isa_thumb.tcc"
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
#line 16232 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16235 "isa_thumb.tcc"
>
inline DecodeMapPage<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16240 "isa_thumb.tcc"
> *Decoder<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16244 "isa_thumb.tcc"
>::FindPage(typename CONFIG::address_t page_key)
{
	if(mru_page && mru_page->key == page_key) return mru_page;
	uint32_t index = page_key % NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key
	DecodeMapPage<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16252 "isa_thumb.tcc"
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
#line 16286 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16289 "isa_thumb.tcc"
>
Operation<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16294 "isa_thumb.tcc"
> *Decoder<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16298 "isa_thumb.tcc"
>::Decode(typename CONFIG::address_t addr, CodeType insn)
{
	Operation<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16304 "isa_thumb.tcc"
	> *operation;
	typename CONFIG::address_t page_key = addr / 2 / NUM_OPERATIONS_PER_PAGE;
	DecodeMapPage<
#line 44 "isa/thumb/thumb.isa"
	CONFIG
#line 16310 "isa_thumb.tcc"
	> *page;
	page = FindPage(page_key);
	if(!page)
	{
		page = new DecodeMapPage<
#line 44 "isa/thumb/thumb.isa"
		CONFIG
#line 16318 "isa_thumb.tcc"
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
#line 16340 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16343 "isa_thumb.tcc"
>
void Decoder<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16348 "isa_thumb.tcc"
>::SetLittleEndian()
{
	is_little_endian = true;
}

template <
#line 44 "isa/thumb/thumb.isa"
class
#line 16357 "isa_thumb.tcc"
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16360 "isa_thumb.tcc"
>
void Decoder<
#line 44 "isa/thumb/thumb.isa"
CONFIG
#line 16365 "isa_thumb.tcc"
>::SetBigEndian()
{
	is_little_endian = false;
}

} } } } } } }
