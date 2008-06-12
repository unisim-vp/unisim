#include "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.hh"
#include <cassert>
#line 75 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"

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
#line 24 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
namespace unisim { namespace component { namespace cxx { namespace processor { namespace arm { namespace isa { namespace thumb {
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 29 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 32 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 37 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::Operation(CodeType _code, typename CONFIG::address_t _addr, const char *_name)
:
encoding(_code),
addr(_addr),
name(_name)
{
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 49 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 52 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 57 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::~Operation()
{
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 65 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 68 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 53 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/profiling.isa"
void
#line 72 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 76 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::profile(
#line 53 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/profiling.isa"
map<uint64_t, uint32_t> &
#line 80 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 53 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/profiling.isa"
prof
#line 83 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 53 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/profiling.isa"
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
#line 97 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 102 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 105 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 109 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 113 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 117 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 120 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 124 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 127 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	{
		buffer << "Unknown instruction";
	}
#line 134 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 139 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 142 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 146 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 150 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 154 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 157 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	{
		throw UndefinedInstructionException<CONFIG>();
	}
#line 164 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 169 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 172 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>

void
Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 179 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::initialize_rd_index()
{
#line 89 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rd_index = 0;
	}
#line 186 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 191 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 194 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>

void
Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 201 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::initialize_rn_index()
{
#line 85 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rn_index = 0;
	}
#line 208 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 213 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 216 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>

void
Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 223 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::initialize_rm_index()
{
#line 81 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rm_index = 0;
	}
#line 230 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 235 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 238 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>

void
Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 245 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::initialize_rdp1_init()
{
#line 73 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rdp1_init = 0;
	}
#line 252 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 257 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 260 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>

void
Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 267 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::initialize_rdlo_init()
{
#line 69 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rdlo_init = 0;
	}
#line 274 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 279 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 282 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>

void
Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 289 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::initialize_rdhi_init()
{
#line 65 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rdhi_init = 0;
	}
#line 296 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 301 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 304 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>

void
Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 311 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::initialize_rs_init()
{
#line 61 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rs_init = 0;
	}
#line 318 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 323 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 326 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>

void
Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 333 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::initialize_rd_init()
{
#line 57 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rd_init = 0;
	}
#line 340 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 345 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 348 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>

void
Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 355 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::initialize_rm_init()
{
#line 53 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rm_init = 0;
	}
#line 362 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 367 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 370 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>

void
Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 377 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::initialize_rn_init()
{
#line 49 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		rn_init = 0;
	}
#line 384 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 389 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 392 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>

void
Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 399 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::initialize_cond_init()
{
#line 37 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/constructors_dec.isa"
	{
		cond_init = 0;
	}
#line 406 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 411 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 414 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpB_1_0xxx : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 419 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 504 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 508 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 511 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 515 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 518 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 556 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 560 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 563 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 571 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 574 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpB_1_10xx : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 579 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 664 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 668 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 671 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 675 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 678 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 716 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 720 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 723 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 731 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 734 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpB_1_110x : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 739 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 824 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 828 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 831 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 835 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 838 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 876 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 880 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 883 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 891 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 894 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpB_2 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 899 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpB_2(CodeType code, typename CONFIG::address_t addr);
	int32_t imm;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 908 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 912 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 915 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 919 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 922 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 960 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 964 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 967 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 975 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 978 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpBl : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 983 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpBl(CodeType code, typename CONFIG::address_t addr);
	uint32_t h2;
	uint32_t offset;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 993 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 997 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1000 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1004 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1007 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1045 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1049 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1052 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 1060 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 1063 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpBx : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 1068 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpBx(CodeType code, typename CONFIG::address_t addr);
	uint32_t h2;
	uint32_t rm;
	uint32_t sbz;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1079 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1083 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1086 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1090 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1093 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1137 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1141 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1144 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 1152 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 1155 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpBlx_1 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 1160 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpBlx_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t offset;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1169 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1173 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1176 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1180 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1183 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1221 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1225 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1228 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 1236 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 1239 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpBlx_2 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 1244 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1261 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1265 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1268 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1272 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1275 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1313 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1317 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1320 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 1328 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 1331 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpAdc : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 1336 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpAdc(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1346 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1350 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1353 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1357 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1360 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1398 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1402 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1405 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 1413 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 1416 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpAdd_1 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 1421 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpAdd_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1432 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1436 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1439 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1443 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1446 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1484 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1488 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1491 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 1499 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 1502 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpAdd_2 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 1507 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpAdd_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rd;
	uint32_t imm;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1517 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1521 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1524 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1528 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1531 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1569 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1573 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1576 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 1584 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 1587 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpAdd_3 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 1592 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpAdd_3(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1603 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1607 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1610 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1614 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1617 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1655 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1659 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1662 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 1670 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 1673 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpAdd_4 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 1678 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1702 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1706 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1709 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1713 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1716 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1754 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1758 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1761 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 1769 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 1772 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpAdd_5 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 1777 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpAdd_5(CodeType code, typename CONFIG::address_t addr);
	uint32_t rd;
	uint32_t imm;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1787 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1791 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1794 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1798 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1801 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1839 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1843 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1846 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 1854 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 1857 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpAdd_6 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 1862 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpAdd_6(CodeType code, typename CONFIG::address_t addr);
	uint32_t rd;
	uint32_t imm;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1872 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1876 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1879 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1883 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1886 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1924 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1928 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1931 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 1939 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 1942 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpAdd_7 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 1947 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpAdd_7(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 1956 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 1960 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 1963 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 1967 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 1970 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2008 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2012 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2015 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 2023 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 2026 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpAnd : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 2031 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpAnd(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2041 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2045 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2048 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2052 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2055 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2093 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2097 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2100 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 2108 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 2111 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpAsr_1 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 2116 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpAsr_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2127 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2131 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2134 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2138 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2141 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2179 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2183 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2186 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 2194 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 2197 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpAsr_2 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 2202 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpAsr_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rs;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2212 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2216 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2219 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2223 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2226 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2264 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2268 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2271 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 2279 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 2282 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpBic : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 2287 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpBic(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2297 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2301 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2304 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2308 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2311 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2349 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2353 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2356 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 2364 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 2367 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpCmn : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 2372 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpCmn(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2382 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2386 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2389 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2393 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2396 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2434 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2438 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2441 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 2449 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 2452 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpCmp_1 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 2457 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpCmp_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t rn;
	uint32_t imm;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2467 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2471 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2474 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2478 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2481 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2519 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2523 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2526 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 2534 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 2537 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpCmp_2 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 2542 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpCmp_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2552 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2556 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2559 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2563 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2566 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2604 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2608 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2611 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 2619 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 2622 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpCmp_3 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 2627 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2651 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2655 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2658 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2662 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2665 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2703 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2707 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2710 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 2718 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 2721 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpEor : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 2726 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpEor(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2736 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2740 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2743 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2747 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2750 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2788 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2792 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2795 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 2803 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 2806 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpLsl_1 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 2811 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpLsl_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2822 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2826 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2829 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2833 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2836 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2874 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2878 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2881 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 2889 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 2892 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpLsl_2 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 2897 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpLsl_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rs;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2907 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2911 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2914 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 2918 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 2921 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2959 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2963 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 2966 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 2974 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 2977 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpLsr_1 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 2982 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpLsr_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 2993 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 2997 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3000 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3004 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3007 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3045 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3049 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3052 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 3060 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 3063 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpLsr_2 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 3068 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpLsr_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rs;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3078 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3082 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3085 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3089 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3092 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3130 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3134 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3137 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 3145 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 3148 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpMov_1 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 3153 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpMov_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t rd;
	uint32_t imm;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3163 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3167 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3170 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3174 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3177 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3215 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3219 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3222 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 3230 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 3233 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpMov_3 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 3238 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3259 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3263 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3266 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3270 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3273 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3311 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3315 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3318 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 3326 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 3329 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpMul : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 3334 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpMul(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3344 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3348 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3351 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3355 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3358 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3396 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3400 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3403 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 3411 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 3414 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpMvn : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 3419 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpMvn(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3429 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3433 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3436 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3440 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3443 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3481 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3485 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3488 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 3496 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 3499 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpNeg : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 3504 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpNeg(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3514 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3518 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3521 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3525 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3528 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3566 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3570 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3573 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 3581 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 3584 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpOrr : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 3589 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpOrr(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3599 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3603 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3606 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3610 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3613 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3651 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3655 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3658 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 3666 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 3669 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpRor : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 3674 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpRor(CodeType code, typename CONFIG::address_t addr);
	uint32_t rs;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3684 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3688 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3691 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3695 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3698 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3736 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3740 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3743 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 3751 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 3754 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpSbc : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 3759 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpSbc(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3769 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3773 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3776 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3780 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3783 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3821 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3825 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3828 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 3836 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 3839 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpSub_1 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 3844 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpSub_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3855 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3859 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3862 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3866 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3869 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3907 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3911 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3914 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 3922 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 3925 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpSub_2 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 3930 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpSub_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rd;
	uint32_t imm;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3940 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3944 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3947 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 3951 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 3954 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 3992 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 3996 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 3999 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 4007 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 4010 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpSub_3 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 4015 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpSub_3(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4026 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4030 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4033 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4037 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4040 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4078 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4082 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4085 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 4093 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 4096 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpSub_4 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 4101 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpSub_4(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4110 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4114 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4117 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4121 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4124 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4162 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4166 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4169 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 4177 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 4180 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpTst : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 4185 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpTst(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4195 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4199 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4202 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4206 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4209 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4247 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4251 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4254 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 4262 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 4265 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpLdr_1 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 4270 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpLdr_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4281 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4285 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4288 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4292 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4295 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4333 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4337 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4340 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 4348 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 4351 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpLdr_2 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 4356 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpLdr_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4367 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4371 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4374 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4378 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4381 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4419 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4423 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4426 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 4434 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 4437 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpLdr_3 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 4442 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpLdr_3(CodeType code, typename CONFIG::address_t addr);
	uint32_t rd;
	uint32_t imm;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4452 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4456 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4459 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4463 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4466 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4504 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4508 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4511 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 4519 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 4522 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpLdr_4 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 4527 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpLdr_4(CodeType code, typename CONFIG::address_t addr);
	uint32_t rd;
	uint32_t imm;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4537 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4541 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4544 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4548 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4551 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4589 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4593 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4596 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 4604 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 4607 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpLdrb_1 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 4612 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpLdrb_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4623 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4627 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4630 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4634 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4637 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4675 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4679 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4682 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 4690 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 4693 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpLdrb_2 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 4698 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpLdrb_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4709 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4713 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4716 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4720 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4723 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4761 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4765 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4768 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 4776 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 4779 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpLdrh_1 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 4784 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpLdrh_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4795 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4799 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4802 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4806 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4809 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4847 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4851 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4854 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 4862 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 4865 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpLdrh_2 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 4870 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpLdrh_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4881 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4885 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4888 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4892 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4895 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4933 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4937 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4940 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 4948 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 4951 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpLdrsb : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 4956 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpLdrsb(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 4967 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 4971 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 4974 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 4978 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 4981 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5019 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5023 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5026 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 5034 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 5037 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpLdrsh : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 5042 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpLdrsh(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5053 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5057 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5060 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5064 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5067 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5105 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5109 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5112 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 5120 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 5123 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpStr_1 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 5128 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpStr_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5139 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5143 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5146 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5150 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5153 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5191 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5195 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5198 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 5206 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 5209 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpStr_2 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 5214 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpStr_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5225 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5229 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5232 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5236 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5239 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5277 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5281 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5284 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 5292 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 5295 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpStr_3 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 5300 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpStr_3(CodeType code, typename CONFIG::address_t addr);
	uint32_t rd;
	uint32_t imm;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5310 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5314 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5317 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5321 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5324 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5362 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5366 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5369 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 5377 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 5380 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpStrb_1 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 5385 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpStrb_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5396 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5400 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5403 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5407 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5410 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5448 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5452 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5455 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 5463 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 5466 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpStrb_2 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 5471 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpStrb_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5482 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5486 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5489 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5493 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5496 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5534 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5538 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5541 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 5549 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 5552 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpStrh_1 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 5557 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpStrh_1(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5568 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5572 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5575 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5579 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5582 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5620 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5624 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5627 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 5635 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 5638 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpStrh_2 : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 5643 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpStrh_2(CodeType code, typename CONFIG::address_t addr);
	uint32_t rm;
	uint32_t rn;
	uint32_t rd;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5654 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5658 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5661 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5665 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5668 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5706 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5710 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5713 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 5721 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 5724 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpLdmia : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 5729 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpLdmia(CodeType code, typename CONFIG::address_t addr);
	uint32_t rn;
	uint32_t reg_list;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5739 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5743 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5746 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5750 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5753 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5791 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5795 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5798 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 5806 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 5809 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpPop : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 5814 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpPop(CodeType code, typename CONFIG::address_t addr);
	uint32_t r;
	uint32_t reg_list;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5824 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5828 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5831 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5835 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5838 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5876 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5880 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5883 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 5891 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 5894 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpPush : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 5899 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpPush(CodeType code, typename CONFIG::address_t addr);
	uint32_t r;
	uint32_t reg_list;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5909 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5913 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5916 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 5920 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 5923 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5961 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5965 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 5968 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 5976 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 5979 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpStmia : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 5984 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpStmia(CodeType code, typename CONFIG::address_t addr);
	uint32_t rn;
	uint32_t reg_list;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 5994 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 5998 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 6001 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 6005 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 6008 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 6046 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 6050 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 6053 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 6061 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6064 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpBkpt : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6069 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpBkpt(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 6078 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 6082 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 6085 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 6089 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 6092 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 6130 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 6134 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 6137 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	);
private:
};

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 6145 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6148 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
class OpSwi : public Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6153 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
{
public:
	OpSwi(CodeType code, typename CONFIG::address_t addr);
	uint32_t imm;
	virtual
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 6162 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 6166 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 6169 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	stringstream &
#line 6173 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	buffer
#line 6176 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	void
#line 6214 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	CPU<CONFIG> &
#line 6218 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
	cpu
#line 6221 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 6271 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6274 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>

void
OpB_1_0xxx<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6281 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::initialize_cond_init()
{
#line 50 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::cond_init = sub_cond;
	}
#line 6290 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 6295 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6298 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6302 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpB_1_0xxx<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6306 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6310 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6313 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 6317 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 6320 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 70 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;
		buffer << "b";
		cpu.DisasmCondition(inherited::cond_init, buffer);
		buffer << " 0x" << hex << cpu.GetGPR(cpu.PC_reg) + 4 + (imm << 1) << dec;
	}
#line 6330 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 6335 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6338 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6342 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpB_1_0xxx<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6346 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6350 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6353 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 77 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
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
#line 6370 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 6376 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6379 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6384 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpB_1_0xxx(CodeType code, typename CONFIG::address_t addr)
{
	return new OpB_1_0xxx<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 6390 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 6397 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6400 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>

void
OpB_1_10xx<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6407 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::initialize_cond_init()
{
#line 56 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::cond_init = sub_cond + 8;
	}
#line 6416 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 6421 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6424 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6428 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpB_1_10xx<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6432 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6436 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6439 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 6443 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 6446 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 70 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;
		buffer << "b";
		cpu.DisasmCondition(inherited::cond_init, buffer);
		buffer << " 0x" << hex << cpu.GetGPR(cpu.PC_reg) + 4 + (imm << 1) << dec;
	}
#line 6456 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 6461 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6464 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6468 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpB_1_10xx<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6472 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6476 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6479 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 77 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
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
#line 6496 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 6502 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6505 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6510 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpB_1_10xx(CodeType code, typename CONFIG::address_t addr)
{
	return new OpB_1_10xx<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 6516 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 6523 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6526 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>

void
OpB_1_110x<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6533 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::initialize_cond_init()
{
#line 62 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::cond_init = sub_cond + 12;
	}
#line 6542 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 6547 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6550 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6554 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpB_1_110x<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6558 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6562 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6565 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 6569 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 6572 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 70 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;
		buffer << "b";
		cpu.DisasmCondition(inherited::cond_init, buffer);
		buffer << " 0x" << hex << cpu.GetGPR(cpu.PC_reg) + 4 + (imm << 1) << dec;
	}
#line 6582 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 6587 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6590 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6594 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpB_1_110x<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6598 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6602 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6605 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 77 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
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
#line 6622 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 6628 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6631 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6636 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpB_1_110x(CodeType code, typename CONFIG::address_t addr)
{
	return new OpB_1_110x<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 6642 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 6656 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6659 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6663 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpB_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6667 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6671 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6674 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 6678 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 6681 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 102 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		buffer << "b";
		buffer << " 0x" << hex << cpu.GetGPR(cpu.PC_reg) + 4 + (imm << 1) << dec;
	}
#line 6691 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 6696 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6699 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6703 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpB_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6707 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6711 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6714 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 109 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		typename CONFIG::reg_t new_PC;

		new_PC = cpu.GetGPR(cpu.PC_reg) + 4 + (imm << 1);
		cpu.SetGPR(cpu.PC_reg, new_PC);
	}
#line 6726 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 6732 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6735 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6740 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpB_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpB_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 6746 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 6760 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6763 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6767 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpBl<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6771 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6775 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6778 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 6782 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 6785 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 129 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
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
#line 6803 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 6808 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6811 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6815 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpBl<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6819 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6823 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6826 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 144 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
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
#line 6851 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 6857 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6860 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6865 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpBl(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBl<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 6871 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 6885 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6888 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6892 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpBx<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6896 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6900 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6903 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 6907 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 6910 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 177 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		buffer << "bx "
		<< "r" << inherited::rm_index << endl;
	}
#line 6920 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 6925 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6928 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>

void
OpBx<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6935 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::initialize_rm_init()
{
#line 184 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		if((h2 << 3) + rm == CPU<CONFIG>::PC_reg)
		inherited::rm_init = 4;
		else
		inherited::rm_init = 0;
	}
#line 6947 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 6952 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6955 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>

void
OpBx<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6962 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::initialize_rm_index()
{
#line 193 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::rm_index = (h2 << 3) + rm;
	}
#line 6971 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 6976 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6979 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 6983 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpBx<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 6987 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 6991 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 6994 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 199 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
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
#line 7013 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 7019 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7022 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7027 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpBx(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBx<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 7033 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 7047 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7050 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7054 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpBlx_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7058 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7062 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7065 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 7069 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 7072 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 226 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
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
#line 7091 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 7096 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7099 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7103 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpBlx_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7107 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7111 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7114 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 242 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typename CONFIG::reg_t new_PC;

		new_PC = (cpu.GetGPR(cpu.LR_reg) + (offset << 1)) & (typename CONFIG::reg_t)0xFFFFFFFC;
		cpu.SetGPR(cpu.LR_reg, cpu.GetGPR(cpu.PC_reg) + 3);
		cpu.SetGPR(cpu.PC_reg, new_PC);
	}
#line 7125 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 7131 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7134 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7139 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpBlx_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBlx_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 7145 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 7159 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7162 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>

void
OpBlx_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7169 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::initialize_rm_init()
{
#line 261 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		if((h2 << 3) + rm == CPU<CONFIG>::PC_reg)
		inherited::rm_init = 4;
		else
		inherited::rm_init = 0;
	}
#line 7181 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 7186 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7189 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>

void
OpBlx_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7196 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::initialize_rm_index()
{
#line 270 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::rm_index = (h2 << 3) + rm;
	}
#line 7205 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 7210 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7213 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7217 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpBlx_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7221 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7225 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7228 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 7232 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 7235 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 276 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
	{
		typedef Operation<CONFIG> inherited;

		buffer << "blx r" << inherited::rm_index;
	}
#line 7244 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 7249 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7252 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7256 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpBlx_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7260 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7264 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7267 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 282 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/branch.isa"
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
#line 7286 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 7292 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7295 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7300 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpBlx_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBlx_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 7306 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 7357 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7360 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7364 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpAdc<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7368 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7372 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7375 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 7379 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 7382 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 48 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "adc r" << rd << ", r" << rm;
	}
#line 7389 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 7394 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7397 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7401 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpAdc<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7405 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7409 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7412 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 52 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 7435 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 7441 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7444 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7449 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpAdc(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAdc<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 7455 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 7469 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7472 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7476 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpAdd_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7480 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7484 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7487 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 7491 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 7494 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 83 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "add r" << rd << ", r" << rn << ", #" << dec << imm;
	}
#line 7501 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 7506 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7509 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7513 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpAdd_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7517 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7521 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7524 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 87 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 7546 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 7552 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7555 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7560 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpAdd_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAdd_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 7566 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of add(1) (add immediate) instruction
*******************************************************************/
/*******************************************************************
* add (add large immediate) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 7579 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7582 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7586 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpAdd_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7590 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7594 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7597 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 7601 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 7604 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 116 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "add r" << rd << ", #" << dec << imm;
	}
#line 7611 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 7616 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7619 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7623 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpAdd_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7627 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7631 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7634 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 120 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 7656 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 7662 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7665 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7670 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpAdd_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAdd_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 7676 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of add (add large immediate) instruction
*******************************************************************/
/*******************************************************************
* add (add register) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 7689 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7692 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7696 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpAdd_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7700 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7704 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7707 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 7711 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 7714 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 149 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "add r" << rd << ", r" << rn << ", r" << rm;
	}
#line 7721 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 7726 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7729 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7733 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpAdd_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7737 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7741 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7744 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 153 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 7768 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 7774 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7777 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7782 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpAdd_3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAdd_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 7788 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of add (add register) instruction
*******************************************************************/
/*******************************************************************
* add (add high registers) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 7801 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7804 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>

void
OpAdd_4<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7811 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::initialize_rm_index()
{
#line 184 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::rm_index = (h2 << 3) + rm;
	}
#line 7820 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 7825 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7828 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>

void
OpAdd_4<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7835 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::initialize_rd_index()
{
#line 190 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::rd_index = (h1 << 3) + rd;
	}
#line 7844 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 7849 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7852 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>

void
OpAdd_4<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7859 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::initialize_rm_init()
{
#line 196 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		if((h2 << 3) + rm == CPU<CONFIG>::PC_reg)
		inherited::rm_init = 4;
	}
#line 7869 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 7874 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7877 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>

void
OpAdd_4<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7884 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::initialize_rd_init()
{
#line 203 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		if((h1 << 3) + rd == CPU<CONFIG>::PC_reg)
		inherited::rd_init = 4;
	}
#line 7894 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 7899 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7902 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7906 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpAdd_4<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7910 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7914 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7917 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 7921 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 7924 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 210 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		buffer << "add r" << inherited::rd_index << ", r" << inherited::rm_index;
	}
#line 7933 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 7938 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7941 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 7945 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpAdd_4<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7949 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 7953 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 7956 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 216 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		typename CONFIG::reg_t s1, s2, res;
		s1 = cpu.GetGPR(inherited::rd_index) + inherited::rd_init;
		s2 = cpu.GetGPR(inherited::rm_index) + inherited::rm_init;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		res = s1 + s2;
		cpu.SetGPR(inherited::rd_index, res);
	}
#line 7972 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 7978 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7981 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 7986 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpAdd_4(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAdd_4<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 7992 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of add (add high registers) instruction
*******************************************************************/
/*******************************************************************
* add (add immediate to program counter) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 8005 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8008 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8012 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpAdd_5<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8016 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8020 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8023 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8027 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8030 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 239 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "add r" << rd << ", PC, #" << dec << ((unsigned int)imm * 4);
	}
#line 8037 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 8042 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8045 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8049 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpAdd_5<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8053 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8057 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8060 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 243 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 8077 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 8083 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8086 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8091 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpAdd_5(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAdd_5<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 8097 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of add (add immediate to program counter) instruction
*******************************************************************/
/*******************************************************************
* add (add immediate to stack pointer) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 8110 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8113 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8117 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpAdd_6<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8121 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8125 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8128 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8132 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8135 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 267 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "add r" << rd << ", SP, #" << dec << ((unsigned int)imm * 4);
	}
#line 8142 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 8147 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8150 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8154 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpAdd_6<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8158 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8162 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8165 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 271 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t s1, s2, res;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(cpu.SP_reg) & (typename CONFIG::reg_t)0xfffffffc;
		s2 = ((typename CONFIG::reg_t)imm) << 2;
		res = s1 + s2;

		cpu.SetGPR(rd, res);
	}
#line 8180 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 8186 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8189 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8194 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpAdd_6(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAdd_6<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 8200 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of add (add immediate to stack pointer) instruction
*******************************************************************/
/*******************************************************************
* add (increment stack pointer) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 8213 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8216 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8220 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpAdd_7<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8224 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8228 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8231 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8235 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8238 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 293 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "add SP, #" << (unsigned int)imm * 4;
	}
#line 8245 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 8250 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8253 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8257 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpAdd_7<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8261 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8265 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8268 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 297 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		cpu.SetGPR(cpu.SP_reg, cpu.GetGPR(cpu.SP_reg) + ((typename CONFIG::reg_t)imm * 4));
	}
#line 8277 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 8283 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8286 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8291 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpAdd_7(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAdd_7<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 8297 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of add (increment stack pointer) instruction
*******************************************************************/
/*******************************************************************
* and (logical and) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 8310 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8313 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8317 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpAnd<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8321 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8325 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8328 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8332 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8335 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 313 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "and r" << rd << ", r" << rm;
	}
#line 8342 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 8347 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8350 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8354 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpAnd<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8358 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8362 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8365 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 317 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		res = cpu.GetGPR(rd) & cpu.GetGPR(rm);
		cpu.SetGPR(rd,res);

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
	}
#line 8380 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 8386 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8389 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8394 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpAnd(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAnd<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 8400 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of and (logical and) instruction
*******************************************************************/
/*******************************************************************
* asr(1) (arithmetic shift right immediate) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 8413 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8416 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8420 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpAsr_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8424 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8428 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8431 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8435 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8438 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 339 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "asr r" << rd << ", r" << rm << ", #" << dec << imm;
	}
#line 8445 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 8450 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8453 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8457 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpAsr_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8461 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8465 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8468 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 343 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 8496 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 8502 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8505 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8510 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpAsr_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAsr_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 8516 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of asr(1) (arithmetic shift right immediate) instruction
*******************************************************************/
/*******************************************************************
* asr(2) (arithmetic shift right register) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 8529 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8532 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8536 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpAsr_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8540 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8544 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8547 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8551 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8554 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 378 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "asr r" << rd << ", r" << rs;
	}
#line 8561 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 8566 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8569 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8573 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpAsr_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8577 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8581 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8584 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 382 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 8616 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 8622 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8625 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8630 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpAsr_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAsr_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 8636 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of asr(2) (arithmetic shift right register) instruction
*******************************************************************/
/*******************************************************************
* bic (bit clear) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 8649 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8652 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8656 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpBic<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8660 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8664 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8667 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8671 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8674 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 421 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "bic r" << rd << ", r" << rm;
	}
#line 8681 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 8686 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8689 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8693 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpBic<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8697 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8701 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8704 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 425 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res, s1, s2;
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(rd);
		s2 = cpu.GetGPR(rm);

		res = s1 & ~s2;

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
	}
#line 8720 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 8726 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8729 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8734 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpBic(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBic<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 8740 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of bic (bit clear) instruction
*******************************************************************/
/*******************************************************************
* cmn (compare negative) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 8753 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8756 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8760 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpCmn<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8764 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8768 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8771 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8775 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8778 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 448 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "cmn r" << rn << ", r" << rm;
	}
#line 8785 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 8790 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8793 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8797 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpCmn<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8801 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8805 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8808 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 452 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 8829 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 8835 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8838 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8843 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpCmn(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCmn<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 8849 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of cmn (compare negative) instruction
*******************************************************************/
/*******************************************************************
* cmp(1) (compare immediate) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 8862 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8865 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8869 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpCmp_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8873 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8877 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8880 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8884 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8887 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 479 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "cmp r" << rn << ", #" << dec << imm;
	}
#line 8894 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 8899 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8902 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8906 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpCmp_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8910 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8914 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8917 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 483 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 8935 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 8941 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8944 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8949 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpCmp_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCmp_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 8955 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of cmp(1) (compare immediate) instruction
*******************************************************************/
/*******************************************************************
* cmp(2) (compare register) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 8968 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8971 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 8975 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpCmp_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 8979 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 8983 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 8986 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 8990 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 8993 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 507 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "cmp r" << rn << ", r" << rm;
	}
#line 9000 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 9005 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9008 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9012 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpCmp_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9016 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9020 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9023 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 511 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 9041 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 9047 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9050 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9055 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpCmp_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCmp_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 9061 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of cmp(2) (compare register) instruction
*******************************************************************/
/*******************************************************************
* cmp(3) (compare high registers) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 9074 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9077 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>

void
OpCmp_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9084 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::initialize_rm_index()
{
#line 536 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::rm_index = (h2 << 3) + rm;
	}
#line 9093 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 9098 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9101 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>

void
OpCmp_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9108 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::initialize_rn_index()
{
#line 542 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::rn_index = (h1 << 3) + rn;
	}
#line 9117 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 9122 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9125 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>

void
OpCmp_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9132 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::initialize_rm_init()
{
#line 548 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		if((h2 << 3) + rm == CPU<CONFIG>::PC_reg)
		inherited::rm_init = 4;
		else
		inherited::rm_init = 0;
	}
#line 9144 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 9149 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9152 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>

void
OpCmp_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9159 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::initialize_rn_init()
{
#line 557 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		if((h1 << 3) + rn == CPU<CONFIG>::PC_reg)
		inherited::rn_init = 4;
		else
		inherited::rn_init = 0;
	}
#line 9171 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 9176 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9179 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9183 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpCmp_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9187 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9191 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9194 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 9198 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 9201 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 566 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		buffer << "cmp r" << inherited::rn_index << ", r" << inherited::rm_index;
	}
#line 9210 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 9215 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9218 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9222 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpCmp_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9226 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9230 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9233 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 572 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 9252 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 9258 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9261 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9266 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpCmp_3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCmp_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 9272 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of cmp(3) (compare high registers) instruction
*******************************************************************/
/*******************************************************************
* eor (exclusive or) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 9285 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9288 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9292 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpEor<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9296 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9300 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9303 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 9307 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 9310 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 598 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "eor r" << rd << ", r" << rm;
	}
#line 9317 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 9322 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9325 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9329 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpEor<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9333 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9337 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9340 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 602 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res, s1, s2;
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(rd);
		s2 = cpu.GetGPR(rm);

		res = s1 ^ s2;

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
	}
#line 9356 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 9362 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9365 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9370 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpEor(CodeType code, typename CONFIG::address_t addr)
{
	return new OpEor<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 9376 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of eor (exclusive or) instruction
*******************************************************************/
/*******************************************************************
* lsl(1) (logical shift left immediate) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 9389 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9392 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9396 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpLsl_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9400 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9404 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9407 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 9411 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 9414 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 624 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "lsl r" << rd << ", r" << rm << ", #" << dec << imm;
	}
#line 9421 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 9426 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9429 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9433 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpLsl_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9437 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9441 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9444 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 628 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 9464 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 9470 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9473 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9478 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpLsl_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLsl_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 9484 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of lsl(1) (logical shift left immediate) instruction
*******************************************************************/
/*******************************************************************
* lsl(2) (logical shift left register) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 9497 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9500 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9504 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpLsl_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9508 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9512 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9515 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 9519 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 9522 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 655 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "lsl r" << rd << ", r" << rs;
	}
#line 9529 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 9534 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9537 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9541 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpLsl_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9545 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9549 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9552 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 659 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 9578 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 9584 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9587 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9592 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpLsl_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLsl_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 9598 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of lsl(2) (logical shift left register) instruction
*******************************************************************/
/*******************************************************************
* lsr(1) (logical shift right immediate) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 9611 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9614 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9618 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpLsr_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9622 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9626 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9629 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 9633 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 9636 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 692 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "lsr r" << rd << ", r" << rm << ", #" << dec << imm;
	}
#line 9643 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 9648 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9651 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9655 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpLsr_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9659 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9663 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9666 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 696 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 9687 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 9693 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9696 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9701 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpLsr_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLsr_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 9707 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of lsr(1) (logical shift right immediate) instruction
*******************************************************************/
/*******************************************************************
* lsr(2) (logical shift right register) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 9720 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9723 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9727 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpLsr_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9731 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9735 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9738 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 9742 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 9745 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 724 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "lsr r" << rd << ", r" << rs;
	}
#line 9752 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 9757 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9760 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9764 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpLsr_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9768 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9772 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9775 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 728 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 9803 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 9809 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9812 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9817 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpLsr_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLsr_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 9823 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of lsr(2) (logical shift right register) instruction
*******************************************************************/
/*******************************************************************
* mov(1) (move immediate) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 9836 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9839 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9843 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpMov_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9847 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9851 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9854 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 9858 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 9861 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 763 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "mov r" << rd << ", #" << dec << imm;
	}
#line 9868 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 9873 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9876 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 9880 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpMov_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9884 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 9888 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 9891 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 767 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res;
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		res = imm;

		cpu.SetGPR(rd, res);

		cpu.SetCPSR_N(false);
		cpu.SetCPSR_Z(res == 0);
	}
#line 9906 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 9912 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9915 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9920 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpMov_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMov_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 9926 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 9948 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9951 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>

void
OpMov_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9958 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::initialize_rm_index()
{
#line 799 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::rm_index = (h2 << 3) + rm;
	}
#line 9967 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 9972 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9975 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>

void
OpMov_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9982 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::initialize_rd_index()
{
#line 805 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		inherited::rd_index = (h1 << 3) + rd;
	}
#line 9991 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 9996 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 9999 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>

void
OpMov_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10006 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::initialize_rm_init()
{
#line 811 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		if((h2 << 3) + rm == CPU<CONFIG>::PC_reg)
		inherited::rm_init = 4;
		else
		inherited::rm_init = 0;
	}
#line 10018 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 10023 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10026 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10030 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpMov_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10034 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10038 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10041 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10045 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10048 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 820 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;

		buffer << "mov r" << inherited::rd_index << ", r" << inherited::rm_index;
	}
#line 10057 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 10062 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10065 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10069 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpMov_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10073 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10077 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10080 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 826 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typedef Operation<CONFIG> inherited;
		typename CONFIG::reg_t res;

		res = inherited::rm_init + cpu.GetGPR(inherited::rm_index);

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		cpu.SetGPR(inherited::rd_index, res);
	}
#line 10094 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 10100 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10103 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10108 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpMov_3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMov_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 10114 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of mov(3) (move high registers) instruction
*******************************************************************/
/*******************************************************************
* mul (multiply) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 10127 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10130 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10134 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpMul<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10138 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10142 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10145 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10149 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10152 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 846 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "mul r" << rd << ", r" << rm;
	}
#line 10159 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 10164 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10167 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10171 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpMul<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10175 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10179 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10182 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 850 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res;
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		res = cpu.GetGPR(rd) * cpu.GetGPR(rm);
		cpu.SetGPR(rd, res);

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
	}
#line 10196 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 10202 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10205 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10210 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpMul(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMul<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 10216 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of mul (multiply) instruction
*******************************************************************/
/*******************************************************************
* mvn (move not register) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 10229 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10232 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10236 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpMvn<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10240 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10244 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10247 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10251 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10254 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 871 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "mvn r" << rd << ", r" << rm;
	}
#line 10261 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 10266 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10269 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10273 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpMvn<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10277 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10281 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10284 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 875 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res;
		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		res = ~(cpu.GetGPR(rm));
		cpu.SetGPR(rd, res);

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
	}
#line 10298 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 10304 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10307 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10312 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpMvn(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMvn<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 10318 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of mvn (move not register) instruction
*******************************************************************/
/*******************************************************************
* neg (negate register) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 10331 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10334 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10338 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpNeg<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10342 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10346 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10349 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10353 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10356 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 896 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "neg r" << rd << ", r" << rm;
	}
#line 10363 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 10368 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10371 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10375 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpNeg<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10379 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10383 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10386 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 900 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 10404 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 10410 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10413 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10418 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpNeg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNeg<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 10424 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of neg (negate register) instruction
*******************************************************************/
/*******************************************************************
* orr (logical or) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 10437 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10440 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10444 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpOrr<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10448 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10452 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10455 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10459 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10462 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 925 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "orr r" << rd << ", r" << rm;
	}
#line 10469 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 10474 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10477 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10481 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpOrr<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10485 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10489 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10492 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 929 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 10508 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 10514 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10517 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10522 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpOrr(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOrr<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 10528 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of orr (logical or) instruction
*******************************************************************/
/*******************************************************************
* ror (rotate right register) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 10541 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10544 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10548 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpRor<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10552 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10556 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10559 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10563 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10566 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 952 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "ror r" << rd << ", r" << rs;
	}
#line 10573 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 10578 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10581 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10585 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpRor<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10589 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10593 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10596 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 956 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 10621 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 10627 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10630 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10635 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpRor(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRor<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 10641 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of ror (rotate right register) instruction
*******************************************************************/
/*******************************************************************
* sbc (subtract with carry register) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 10654 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10657 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10661 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpSbc<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10665 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10669 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10672 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10676 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10679 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 988 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "sbc r" << rd << ", r" << rm;
	}
#line 10686 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 10691 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10694 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10698 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpSbc<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10702 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10706 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10709 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 992 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 10731 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 10737 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10740 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10745 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpSbc(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSbc<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 10751 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of sbc (subtract with carry register) instruction
*******************************************************************/
/*******************************************************************
* sub(1) (subtract immediate) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 10764 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10767 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10771 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpSub_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10775 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10779 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10782 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10786 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10789 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 1021 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "sub r" << rd << ", r" << rn << ", #" << dec << imm;
	}
#line 10796 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 10801 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10804 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10808 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpSub_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10812 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10816 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10819 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 1025 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 10839 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 10845 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10848 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10853 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpSub_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSub_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 10859 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of sub(1) (subtract immediate) instruction
*******************************************************************/
/*******************************************************************
* sub(2) (subtract large immediate) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 10872 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10875 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10879 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpSub_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10883 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10887 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10890 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 10894 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 10897 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 1052 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "sub r" << rd << ", #" << dec << imm;
	}
#line 10904 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 10909 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10912 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10916 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpSub_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10920 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10924 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 10927 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 1056 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 10949 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 10955 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10958 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10963 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpSub_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSub_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 10969 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of sub(2) (subtract large immediate) instruction
*******************************************************************/
/*******************************************************************
* sub(3) (subtract register) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 10982 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10985 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 10989 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpSub_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 10993 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 10997 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11000 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11004 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11007 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 1085 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "sub r" << rd << ", r" << rn << ", r" << rm;
	}
#line 11014 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 11019 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11022 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11026 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpSub_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11030 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11034 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11037 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 1089 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
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
#line 11057 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 11063 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11066 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11071 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpSub_3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSub_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 11077 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of sub(3) (subtract register) instruction
*******************************************************************/
/*******************************************************************
* sub(4) (decrement stack pointer) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 11090 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11093 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11097 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpSub_4<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11101 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11105 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11108 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11112 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11115 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 1116 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "sub sp, #" << (imm * 4);
	}
#line 11122 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 11127 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11130 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11134 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpSub_4<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11138 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11142 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11145 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 1120 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t s1, s2;
		typename CONFIG::reg_t res;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(cpu.SP_reg);
		s2 = imm * 4;
		res = s1 - s2;
		cpu.SetGPR(cpu.SP_reg, res);
	}
#line 11160 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 11166 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11169 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11174 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpSub_4(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSub_4<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 11180 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of sub(4) (decrement stack pointer) instruction
*******************************************************************/
/*******************************************************************
* tst (test register) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 11193 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11196 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11200 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpTst<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11204 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11208 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11211 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11215 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11218 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 1142 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		buffer << "tst r" << rn << ", r" << rm;
	}
#line 11225 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 11230 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11233 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11237 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpTst<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11241 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11245 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11248 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 1146 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/data_processing.isa"
	{
		typename CONFIG::reg_t res, s1, s2;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		s1 = cpu.GetGPR(rn);
		s2 = cpu.GetGPR(rm);
		res = s1 & s2;

		cpu.SetCPSR_N(res & (typename CONFIG::reg_t)0x80000000);
		cpu.SetCPSR_Z(res == 0);
	}
#line 11264 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 11270 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11273 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11278 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpTst(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTst<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 11284 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 11335 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11338 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11342 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpLdr_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11346 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11350 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11353 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11357 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11360 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 47 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldr r" << rd << ", [r" << rn << ", #" << dec << (imm * 4) << "]";
	}
#line 11367 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 11372 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11375 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11379 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpLdr_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11383 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11387 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11390 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 51 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		typename CONFIG::address_t addr;
		typename CONFIG::reg_t rn_value, value;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		rn_value = cpu.GetGPR(rn);
		addr = rn_value + (imm * 4);
		value = 0;
		cpu.Read32toGPRAligned(addr, rd);
	}
#line 11405 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 11411 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11414 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11419 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpLdr_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdr_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 11425 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of ldr(1) (load word immediate offset) instruction
*******************************************************************/
/*******************************************************************
* ldr(2) (load word register offset) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 11438 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11441 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11445 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpLdr_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11449 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11453 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11456 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11460 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11463 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 73 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldr r" << rd << ", [r" << rn << ", r" << rm << "]";
	}
#line 11470 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 11475 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11478 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11482 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpLdr_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11486 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11490 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11493 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 77 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
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
#line 11509 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 11515 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11518 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11523 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpLdr_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdr_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 11529 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* ldr(2) (load word register offset) instruction
*******************************************************************/
/*******************************************************************
* ldr(3) (load word pc-relative) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 11542 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11545 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11549 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpLdr_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11553 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11557 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11560 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11564 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11567 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 100 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldr r" << rd << ", [pc, #" << dec << (imm * 4) << "]";
	}
#line 11574 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 11579 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11582 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11586 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpLdr_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11590 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11594 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11597 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 104 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		typename CONFIG::address_t addr;
		typename CONFIG::reg_t pc_value, value;

		pc_value = (cpu.GetGPR(cpu.PC_reg) + 4) & ~((typename CONFIG::reg_t)0x03);

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		addr = pc_value + (imm * 4);
		value = 0;
		cpu.Read32toGPRAligned(addr, rd);
	}
#line 11613 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 11619 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11622 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11627 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpLdr_3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdr_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 11633 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* ldr(3) (load word pc-relative) instruction
*******************************************************************/
/*******************************************************************
* ldr(4) (load word sp-relative) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 11646 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11649 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11653 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpLdr_4<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11657 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11661 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11664 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11668 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11671 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 127 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldr r" << rd << ", [sp, #" << dec << (imm * 4) << "]";
	}
#line 11678 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 11683 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11686 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11690 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpLdr_4<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11694 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11698 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11701 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 131 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		typename CONFIG::address_t addr;
		typename CONFIG::reg_t sp_value, value;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		sp_value = cpu.GetGPR(cpu.SP_reg);
		addr = sp_value + (imm * 4);
		value = 0;
		cpu.Read32toGPRAligned(addr, rd);
	}
#line 11716 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 11722 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11725 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11730 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpLdr_4(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdr_4<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 11736 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* ldr(4) (load word sp-relative) instruction
*******************************************************************/
/*******************************************************************
* ldrb(1) (load unsigned byte immediate offset) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 11749 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11752 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11756 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpLdrb_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11760 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11764 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11767 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11771 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11774 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 153 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldrb r" << rd << ", [r" << rn << ", #" << dec << imm << "]";
	}
#line 11781 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 11786 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11789 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11793 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpLdrb_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11797 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11801 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11804 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 157 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
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
#line 11820 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 11826 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11829 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11834 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpLdrb_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdrb_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 11840 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* ldrb(1) (load unsigned byte immediate offset) instruction
*******************************************************************/
/*******************************************************************
* ldrb(2) (load unsigned byte register offset) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 11853 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11856 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11860 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpLdrb_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11864 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11868 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11871 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11875 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11878 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 180 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldrb r" << rd << ", [r" << rn << ", r" << rm << "]";
	}
#line 11885 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 11890 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11893 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11897 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpLdrb_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11901 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11905 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11908 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 184 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
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
#line 11925 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 11931 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11934 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11939 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpLdrb_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdrb_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 11945 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* ldrb(2) (load unsigned byte register offset) instruction
*******************************************************************/
/*******************************************************************
* ldrh(1) (load unsigned halfword immediate offset) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 11958 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11961 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 11965 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpLdrh_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11969 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 11973 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 11976 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 11980 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 11983 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 208 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldrh r" << rd << ", [r" << rn << ", #" << dec << (imm * 2) << "]";
	}
#line 11990 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 11995 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 11998 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12002 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpLdrh_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12006 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12010 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12013 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 212 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
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
#line 12029 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 12035 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12038 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12043 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpLdrh_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdrh_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 12049 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* ldrh(1) (load unsigned halfword immediate offset) instruction
*******************************************************************/
/*******************************************************************
* ldrh(2) (load unsigned halfword register offset) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 12062 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12065 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12069 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpLdrh_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12073 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12077 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12080 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12084 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12087 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 235 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldrh r" << rd << ", [r" << rn << ", r" << rm << "]";
	}
#line 12094 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 12099 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12102 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12106 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpLdrh_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12110 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12114 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12117 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 239 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
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
#line 12134 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 12140 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12143 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12148 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpLdrh_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdrh_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 12154 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* ldrh(2) (load unsigned halfword register offset) instruction
*******************************************************************/
/*******************************************************************
* ldrsb (load signed byte register offset) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 12167 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12170 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12174 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpLdrsb<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12178 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12182 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12185 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12189 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12192 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 263 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldrsb r" << rd << ", [r" << rn << ", r" << rm << "]";
	}
#line 12199 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 12204 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12207 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12211 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpLdrsb<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12215 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12219 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12222 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 267 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
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
#line 12239 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 12245 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12248 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12253 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpLdrsb(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdrsb<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 12259 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* ldrsb (load signed byte register offset) instruction
*******************************************************************/
/*******************************************************************
* ldrsh (load signed halfword register offset) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 12272 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12275 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12279 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpLdrsh<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12283 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12287 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12290 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12294 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12297 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 291 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "ldrsh r" << rd << ", [r" << rn << ", r" << rm << "]";
	}
#line 12304 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 12309 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12312 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12316 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpLdrsh<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12320 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12324 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12327 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 295 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
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
#line 12344 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 12350 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12353 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12358 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpLdrsh(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdrsh<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 12364 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* ldrsh (load signed halfword register offset) instruction
*******************************************************************/
/*******************************************************************
* str(1) (store word immediate offset) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 12377 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12380 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12384 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpStr_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12388 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12392 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12395 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12399 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12402 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 319 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "str r" << rd << ", [r" << rn << ", #" << (imm * 4) << "]";
	}
#line 12409 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 12414 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12417 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12421 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpStr_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12425 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12429 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12432 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 323 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		typename CONFIG::address_t addr;
		typename CONFIG::reg_t rd_value, rn_value;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		rn_value = cpu.GetGPR(rn);
		addr = rn_value + (imm * 4);
		rd_value = cpu.GetGPR(rd);
		cpu.Write32(addr, rd_value);
	}
#line 12447 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 12453 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12456 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12461 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpStr_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpStr_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 12467 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* str(1) (store word immediate offset) instruction
*******************************************************************/
/*******************************************************************
* str(2) (store word register offset) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 12480 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12483 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12487 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpStr_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12491 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12495 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12498 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12502 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12505 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 345 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "str r" << rd << ", [r" << rn << ", r" << rm << "]";
	}
#line 12512 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 12517 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12520 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12524 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpStr_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12528 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12532 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12535 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 349 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
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
#line 12551 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 12557 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12560 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12565 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpStr_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpStr_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 12571 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* str(2) (store word register offset) instruction
*******************************************************************/
/*******************************************************************
* str(3) (store word sp-relative) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 12584 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12587 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12591 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpStr_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12595 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12599 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12602 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12606 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12609 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 372 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "str r" << rd << ", [sp, #" << (imm * 4) << "]";
	}
#line 12616 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 12621 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12624 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12628 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpStr_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12632 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12636 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12639 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 376 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		typename CONFIG::address_t addr;
		typename CONFIG::reg_t rd_value, sp_value;

		cpu.SetGPR(cpu.PC_reg, cpu.GetGPR(cpu.PC_reg) + 2);

		sp_value = cpu.GetGPR(cpu.SP_reg);
		addr = sp_value + (imm * 4);
		rd_value = cpu.GetGPR(rd);
		cpu.Write32(addr, rd_value);
	}
#line 12654 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 12660 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12663 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12668 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpStr_3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpStr_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 12674 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* str(3) (store word sp-relative) instruction
*******************************************************************/
/*******************************************************************
* strb(1) (store byte immediate offset) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 12687 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12690 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12694 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpStrb_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12698 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12702 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12705 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12709 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12712 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 398 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "strb r" << rd << ", [r" << rn << ", #" << dec << imm << "]";
	}
#line 12719 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 12724 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12727 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12731 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpStrb_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12735 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12739 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12742 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 402 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
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
#line 12759 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 12765 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12768 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12773 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpStrb_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpStrb_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 12779 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* strb(1) (store byte immediate offset) instruction
*******************************************************************/
/*******************************************************************
* strb(2) (store byte register offset) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 12792 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12795 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12799 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpStrb_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12803 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12807 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12810 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12814 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12817 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 426 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "strb r" << rd << ", [r" << rn << ", r" << rm << "]";
	}
#line 12824 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 12829 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12832 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12836 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpStrb_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12840 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12844 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12847 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 430 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
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
#line 12865 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 12871 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12874 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12879 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpStrb_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpStrb_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 12885 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* strb(2) (store byte register offset) instruction
*******************************************************************/
/*******************************************************************
* strh(1) (store halfword immediate offset) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 12898 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12901 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12905 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpStrh_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12909 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12913 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12916 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 12920 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 12923 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 455 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "strh r" << rd << ", [r" << rn << ", #" << dec << (imm * 2) << "]";
	}
#line 12930 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 12935 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12938 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 12942 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpStrh_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12946 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 12950 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 12953 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 459 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
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
#line 12970 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 12976 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12979 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 12984 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpStrh_1(CodeType code, typename CONFIG::address_t addr)
{
	return new OpStrh_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 12990 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* strh(1) (store halfword immediate offset) instruction
*******************************************************************/
/*******************************************************************
* strh(2) (store halfword register offset) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 13003 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13006 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13010 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpStrh_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13014 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13018 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13021 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 13025 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 13028 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 483 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
	{
		buffer << "strh r" << rd << ", [r" << rn << ", r" << rm << "]";
	}
#line 13035 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 13040 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13043 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13047 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpStrh_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13051 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13055 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13058 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 487 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_reg.isa"
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
#line 13076 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 13082 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13085 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13090 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpStrh_2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpStrh_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 13096 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 13147 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13150 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13154 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpLdmia<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13158 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13162 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13165 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 13169 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 13172 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 47 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_multiple.isa"
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
#line 13191 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 13196 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13199 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13203 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpLdmia<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13207 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13211 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13214 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 63 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_multiple.isa"
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
#line 13244 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 13250 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13253 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13258 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpLdmia(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLdmia<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 13264 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of ldmia (load multiple) instruction
*******************************************************************/
/*******************************************************************
* pop (pop multiple) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 13277 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13280 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13284 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpPop<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13288 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13292 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13295 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 13299 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 13302 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 100 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_multiple.isa"
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
#line 13321 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 13326 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13329 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13333 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpPop<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13337 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13341 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13344 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 116 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_multiple.isa"
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
#line 13380 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 13386 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13389 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13394 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpPop(CodeType code, typename CONFIG::address_t addr)
{
	return new OpPop<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 13400 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of pop (pop multiple) instruction
*******************************************************************/
/*******************************************************************
* push (push multiple) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 13413 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13416 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13420 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpPush<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13424 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13428 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13431 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 13435 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 13438 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 159 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_multiple.isa"
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
#line 13461 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 13466 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13469 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13473 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpPush<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13477 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13481 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13484 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 179 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_multiple.isa"
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
#line 13523 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 13529 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13532 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13537 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpPush(CodeType code, typename CONFIG::address_t addr)
{
	return new OpPush<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 13543 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of push (push multiple) instruction
*******************************************************************/
/*******************************************************************
* stmia (store multiple) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 13556 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13559 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13563 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpStmia<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13567 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13571 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13574 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 13578 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 13581 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 225 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_multiple.isa"
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
#line 13600 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 13605 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13608 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13612 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpStmia<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13616 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13620 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13623 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 241 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/load_store_multiple.isa"
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
#line 13654 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 13660 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13663 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13668 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpStmia(CodeType code, typename CONFIG::address_t addr)
{
	return new OpStmia<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 13674 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 13725 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13728 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13732 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpBkpt<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13736 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13740 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13743 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 13747 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 13750 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 47 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/exception.isa"
	{
		buffer << "bkpt #" << dec << imm;
	}
#line 13757 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 13762 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13765 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13769 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpBkpt<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13773 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13777 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13780 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 51 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/exception.isa"
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
#line 13804 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 13810 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13813 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13818 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpBkpt(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBkpt<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 13824 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

/*
* end of bkpt (breakpoint) instruction
*******************************************************************/
/*******************************************************************
* swi (software interrupt) instruction
*/
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 13837 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13840 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13844 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpSwi<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13848 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::disasm(
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13852 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13855 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
,
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
stringstream &
#line 13859 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 39 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
buffer
#line 13862 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 82 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/exception.isa"
	{
		buffer << "swi #" << dec << imm;
	}
#line 13869 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}
template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 13874 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13877 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
void
#line 13881 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
OpSwi<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13885 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::execute(
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
CPU<CONFIG> &
#line 13889 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 35 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/actions_dec.isa"
cpu
#line 13892 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
)
{
#line 86 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/exception.isa"
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
#line 13921 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 13927 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13930 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
static Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13935 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *DecodeOpSwi(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSwi<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 13941 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(code, addr);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 13948 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13951 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpB_1_0xxx<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13956 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpB_1_0xxx(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13960 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "b_1_0xxx")
{
	sub_cond = ((code >> 8) & 0x7);
	imm = (((((int32_t)(code >> 0)) & 0xff) << 24) >> 24);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 13970 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13973 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpB_1_10xx<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13978 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpB_1_10xx(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13982 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "b_1_10xx")
{
	sub_cond = ((code >> 8) & 0x3);
	imm = (((((int32_t)(code >> 0)) & 0xff) << 24) >> 24);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 13992 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 13995 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpB_1_110x<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14000 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpB_1_110x(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14004 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "b_1_110x")
{
	sub_cond = ((code >> 8) & 0x1);
	imm = (((((int32_t)(code >> 0)) & 0xff) << 24) >> 24);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14014 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14017 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpB_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14022 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpB_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14026 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "b_2")
{
	imm = (((((int32_t)(code >> 0)) & 0x7ff) << 21) >> 21);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14035 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14038 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpBl<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14043 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpBl(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14047 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "bl")
{
	h2 = ((code >> 11) & 0x1);
	offset = ((code >> 0) & 0x7ff);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14057 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14060 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpBx<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14065 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpBx(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14069 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "bx")
{
	h2 = ((code >> 6) & 0x1);
	rm = ((code >> 3) & 0x7);
	sbz = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14080 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14083 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpBlx_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14088 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpBlx_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14092 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "blx_1")
{
	offset = ((code >> 0) & 0x7ff);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14101 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14104 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpBlx_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14109 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpBlx_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14113 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "blx_2")
{
	h2 = ((code >> 6) & 0x1);
	rm = ((code >> 3) & 0x7);
	sbz = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14124 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14127 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpAdc<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14132 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpAdc(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14136 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "adc")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14146 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14149 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpAdd_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14154 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpAdd_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14158 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "add_1")
{
	imm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14169 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14172 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpAdd_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14177 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpAdd_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14181 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "add_2")
{
	rd = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14191 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14194 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpAdd_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14199 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpAdd_3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14203 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "add_3")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14214 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14217 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpAdd_4<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14222 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpAdd_4(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14226 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "add_4")
{
	h1 = ((code >> 7) & 0x1);
	h2 = ((code >> 6) & 0x1);
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14238 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14241 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpAdd_5<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14246 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpAdd_5(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14250 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "add_5")
{
	rd = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14260 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14263 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpAdd_6<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14268 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpAdd_6(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14272 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "add_6")
{
	rd = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14282 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14285 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpAdd_7<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14290 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpAdd_7(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14294 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "add_7")
{
	imm = ((code >> 0) & 0x7f);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14303 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14306 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpAnd<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14311 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpAnd(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14315 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "and")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14325 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14328 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpAsr_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14333 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpAsr_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14337 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "asr_1")
{
	imm = ((code >> 6) & 0x1f);
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14348 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14351 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpAsr_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14356 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpAsr_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14360 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "asr_2")
{
	rs = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14370 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14373 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpBic<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14378 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpBic(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14382 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "bic")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14392 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14395 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpCmn<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14400 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpCmn(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14404 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "cmn")
{
	rm = ((code >> 3) & 0x7);
	rn = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14414 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14417 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpCmp_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14422 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpCmp_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14426 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "cmp_1")
{
	rn = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14436 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14439 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpCmp_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14444 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpCmp_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14448 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "cmp_2")
{
	rm = ((code >> 3) & 0x7);
	rn = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14458 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14461 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpCmp_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14466 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpCmp_3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14470 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "cmp_3")
{
	h1 = ((code >> 7) & 0x1);
	h2 = ((code >> 6) & 0x1);
	rm = ((code >> 3) & 0x7);
	rn = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14482 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14485 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpEor<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14490 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpEor(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14494 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "eor")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14504 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14507 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpLsl_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14512 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpLsl_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14516 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "lsl_1")
{
	imm = ((code >> 6) & 0x1f);
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14527 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14530 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpLsl_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14535 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpLsl_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14539 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "lsl_2")
{
	rs = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14549 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14552 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpLsr_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14557 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpLsr_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14561 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "lsr_1")
{
	imm = ((code >> 6) & 0x1f);
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14572 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14575 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpLsr_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14580 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpLsr_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14584 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "lsr_2")
{
	rs = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14594 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14597 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpMov_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14602 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpMov_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14606 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "mov_1")
{
	rd = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14616 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14619 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpMov_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14624 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpMov_3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14628 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "mov_3")
{
	h1 = ((code >> 7) & 0x1);
	h2 = ((code >> 6) & 0x1);
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14640 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14643 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpMul<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14648 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpMul(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14652 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "mul")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14662 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14665 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpMvn<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14670 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpMvn(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14674 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "mvn")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14684 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14687 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpNeg<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14692 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpNeg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14696 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "neg")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14706 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14709 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpOrr<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14714 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpOrr(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14718 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "orr")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14728 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14731 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpRor<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14736 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpRor(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14740 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "ror")
{
	rs = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14750 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14753 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpSbc<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14758 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpSbc(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14762 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "sbc")
{
	rm = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14772 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14775 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpSub_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14780 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpSub_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14784 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "sub_1")
{
	imm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14795 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14798 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpSub_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14803 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpSub_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14807 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "sub_2")
{
	rd = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14817 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14820 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpSub_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14825 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpSub_3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14829 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "sub_3")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14840 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14843 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpSub_4<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14848 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpSub_4(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14852 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "sub_4")
{
	imm = ((code >> 0) & 0x7f);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14861 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14864 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpTst<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14869 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpTst(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14873 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "tst")
{
	rm = ((code >> 3) & 0x7);
	rn = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14883 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14886 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpLdr_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14891 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpLdr_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14895 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "ldr_1")
{
	imm = ((code >> 6) & 0x1f);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14906 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14909 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpLdr_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14914 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpLdr_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14918 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "ldr_2")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14929 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14932 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpLdr_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14937 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpLdr_3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14941 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "ldr_3")
{
	rd = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14951 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14954 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpLdr_4<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14959 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpLdr_4(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14963 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "ldr_4")
{
	rd = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14973 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14976 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpLdrb_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14981 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpLdrb_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14985 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "ldrb_1")
{
	imm = ((code >> 6) & 0x1f);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 14996 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 14999 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpLdrb_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15004 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpLdrb_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15008 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "ldrb_2")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 15019 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15022 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpLdrh_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15027 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpLdrh_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15031 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "ldrh_1")
{
	imm = ((code >> 6) & 0x1f);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 15042 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15045 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpLdrh_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15050 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpLdrh_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15054 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "ldrh_2")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 15065 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15068 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpLdrsb<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15073 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpLdrsb(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15077 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "ldrsb")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 15088 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15091 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpLdrsh<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15096 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpLdrsh(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15100 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "ldrsh")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 15111 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15114 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpStr_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15119 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpStr_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15123 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "str_1")
{
	imm = ((code >> 6) & 0x1f);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 15134 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15137 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpStr_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15142 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpStr_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15146 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "str_2")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 15157 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15160 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpStr_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15165 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpStr_3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15169 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "str_3")
{
	rd = ((code >> 8) & 0x7);
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 15179 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15182 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpStrb_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15187 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpStrb_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15191 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "strb_1")
{
	imm = ((code >> 6) & 0x1f);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 15202 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15205 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpStrb_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15210 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpStrb_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15214 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "strb_2")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 15225 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15228 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpStrh_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15233 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpStrh_1(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15237 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "strh_1")
{
	imm = ((code >> 6) & 0x1f);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 15248 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15251 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpStrh_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15256 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpStrh_2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15260 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "strh_2")
{
	rm = ((code >> 6) & 0x7);
	rn = ((code >> 3) & 0x7);
	rd = ((code >> 0) & 0x7);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 15271 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15274 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpLdmia<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15279 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpLdmia(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15283 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "ldmia")
{
	rn = ((code >> 8) & 0x7);
	reg_list = ((code >> 0) & 0xff);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 15293 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15296 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpPop<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15301 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpPop(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15305 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "pop")
{
	r = ((code >> 8) & 0x1);
	reg_list = ((code >> 0) & 0xff);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 15315 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15318 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpPush<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15323 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpPush(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15327 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "push")
{
	r = ((code >> 8) & 0x1);
	reg_list = ((code >> 0) & 0xff);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 15337 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15340 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpStmia<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15345 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpStmia(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15349 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "stmia")
{
	rn = ((code >> 8) & 0x7);
	reg_list = ((code >> 0) & 0xff);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 15359 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15362 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpBkpt<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15367 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpBkpt(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15371 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "bkpt")
{
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 15380 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15383 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
OpSwi<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15388 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::OpSwi(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15392 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>(code, addr, "swi")
{
	imm = ((code >> 0) & 0xff);
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 15401 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15404 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
DecodeMapPage<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15409 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::DecodeMapPage(typename CONFIG::address_t key)
{
	this->key = key;
	memset(operation, 0, sizeof(operation));
	next = 0;
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 15420 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15423 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
DecodeMapPage<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15428 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::~DecodeMapPage()
{
	unsigned int idx;
	for(idx = 0; idx < NUM_OPERATIONS_PER_PAGE; idx++)
	delete operation[idx];
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 15439 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15442 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15447 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::DecodeTableEntry(CodeType opcode, CodeType opcode_mask, Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15451 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *(*decode)(CodeType, typename CONFIG::address_t))
{
	this->opcode = opcode;
	this->opcode_mask = opcode_mask;
	this->decode = decode;
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 15462 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15465 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
Decoder<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 15470 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::Decoder()
{
	little_endian = false;
	mru_page = 0;
	memset(decode_hash_table, 0, sizeof(decode_hash_table));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15479 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0xdf00U, 0xff00U, DecodeOpSwi<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15483 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15488 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0xbe00U, 0xff00U, DecodeOpBkpt<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15492 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15497 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0xc000U, 0xf800U, DecodeOpStmia<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15501 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15506 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0xb400U, 0xfe00U, DecodeOpPush<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15510 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15515 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0xbc00U, 0xfe00U, DecodeOpPop<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15519 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15524 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0xc800U, 0xf800U, DecodeOpLdmia<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15528 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15533 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x5200U, 0xfe00U, DecodeOpStrh_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15537 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15542 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x8000U, 0xf800U, DecodeOpStrh_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15546 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15551 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x5400U, 0xfe00U, DecodeOpStrb_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15555 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15560 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x7000U, 0xf800U, DecodeOpStrb_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15564 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15569 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x9000U, 0xf800U, DecodeOpStr_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15573 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15578 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x5000U, 0xfe00U, DecodeOpStr_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15582 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15587 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x6000U, 0xf800U, DecodeOpStr_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15591 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15596 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x5e00U, 0xfe00U, DecodeOpLdrsh<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15600 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15605 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x5600U, 0xfe00U, DecodeOpLdrsb<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15609 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15614 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x5a00U, 0xfe00U, DecodeOpLdrh_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15618 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15623 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x8800U, 0xf800U, DecodeOpLdrh_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15627 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15632 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x5c00U, 0xfe00U, DecodeOpLdrb_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15636 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15641 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x7800U, 0xf800U, DecodeOpLdrb_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15645 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15650 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x9800U, 0xf800U, DecodeOpLdr_4<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15654 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15659 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x4800U, 0xf800U, DecodeOpLdr_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15663 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15668 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x5800U, 0xfe00U, DecodeOpLdr_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15672 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15677 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x6800U, 0xf800U, DecodeOpLdr_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15681 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15686 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x4200U, 0xffc0U, DecodeOpTst<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15690 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15695 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0xb080U, 0xff80U, DecodeOpSub_4<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15699 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15704 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x1a00U, 0xfe00U, DecodeOpSub_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15708 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15713 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x3800U, 0xf800U, DecodeOpSub_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15717 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15722 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x1e00U, 0xfe00U, DecodeOpSub_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15726 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15731 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x4180U, 0xffc0U, DecodeOpSbc<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15735 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15740 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x41c0U, 0xffc0U, DecodeOpRor<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15744 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15749 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x4300U, 0xffc0U, DecodeOpOrr<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15753 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15758 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x4240U, 0xffc0U, DecodeOpNeg<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15762 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15767 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x43c0U, 0xffc0U, DecodeOpMvn<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15771 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15776 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x4340U, 0xffc0U, DecodeOpMul<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15780 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15785 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x4600U, 0xff00U, DecodeOpMov_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15789 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15794 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x2000U, 0xf800U, DecodeOpMov_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15798 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15803 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x40c0U, 0xffc0U, DecodeOpLsr_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15807 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15812 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x800U, 0xf800U, DecodeOpLsr_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15816 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15821 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x4080U, 0xffc0U, DecodeOpLsl_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15825 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15830 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x0U, 0xf800U, DecodeOpLsl_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15834 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15839 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x4040U, 0xffc0U, DecodeOpEor<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15843 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15848 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x4500U, 0xff00U, DecodeOpCmp_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15852 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15857 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x4280U, 0xffc0U, DecodeOpCmp_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15861 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15866 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x2800U, 0xf800U, DecodeOpCmp_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15870 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15875 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x42c0U, 0xffc0U, DecodeOpCmn<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15879 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15884 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x4380U, 0xffc0U, DecodeOpBic<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15888 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15893 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x4100U, 0xffc0U, DecodeOpAsr_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15897 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15902 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x1000U, 0xf800U, DecodeOpAsr_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15906 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15911 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x4000U, 0xffc0U, DecodeOpAnd<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15915 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15920 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0xb000U, 0xff80U, DecodeOpAdd_7<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15924 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15929 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0xa800U, 0xf800U, DecodeOpAdd_6<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15933 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15938 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0xa000U, 0xf800U, DecodeOpAdd_5<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15942 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15947 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x4400U, 0xff00U, DecodeOpAdd_4<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15951 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15956 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x1800U, 0xfe00U, DecodeOpAdd_3<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15960 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15965 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x3000U, 0xf800U, DecodeOpAdd_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15969 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15974 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x1c00U, 0xfe00U, DecodeOpAdd_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15978 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15983 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x4140U, 0xffc0U, DecodeOpAdc<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15987 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15992 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x4780U, 0xff80U, DecodeOpBlx_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 15996 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 16001 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0xe800U, 0xf800U, DecodeOpBlx_1<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 16005 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 16010 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0x4700U, 0xff80U, DecodeOpBx<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 16014 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 16019 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0xf000U, 0xf000U, DecodeOpBl<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 16023 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 16028 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0xe000U, 0xf800U, DecodeOpB_2<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 16032 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 16037 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0xdc00U, 0xfe00U, DecodeOpB_1_110x<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 16041 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 16046 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0xd800U, 0xfc00U, DecodeOpB_1_10xx<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 16050 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 16055 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>(0xd000U, 0xf800U, DecodeOpB_1_0xxx<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 16059 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	>));
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 16066 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 16069 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
Decoder<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 16074 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::~Decoder()
{
	InvalidateDecodingCache();
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 16083 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 16086 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
void Decoder<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 16091 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::Fetch(void *, typename CONFIG::address_t, uint32_t)
{
	assert( ! "Calling unimplemented virtual method Fetch" );
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 16100 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 16103 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 16108 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *Decoder<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 16112 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::NCDecode(typename CONFIG::address_t addr)
{
	Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 16118 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	> *operation;
	CodeType code;
	Fetch(&code, addr, sizeof(code));
#if BYTE_ORDER == LITTLE_ENDIAN
	if(!little_endian)
#else
	if(little_endian)
#endif
	{
		code = ((code & 0xff00U) >> 8) | ((code & 0x00ffU) << 8);
	}
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
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 16154 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 16173 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 16176 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 16181 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *Decoder<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 16185 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::NCDecode(typename CONFIG::address_t addr, CodeType code)
{
	Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 16191 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 16217 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 16236 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 16239 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
void Decoder<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 16244 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::InvalidateDecodingCache()
{
	uint32_t index;
	mru_page = 0;
	for(index = 0; index < NUM_DECODE_HASH_TABLE_ENTRIES; index++)
	{
		DecodeMapPage<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
		CONFIG
#line 16254 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 16273 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 16276 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
void Decoder<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 16281 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::InvalidateDecodingCacheEntry(typename CONFIG::address_t addr)
{
	typename CONFIG::address_t page_key = addr / 2 / NUM_OPERATIONS_PER_PAGE;
	if(mru_page && mru_page->key == page_key) mru_page = 0;
	uint32_t index = page_key % NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key
	DecodeMapPage<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 16290 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 16323 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 16326 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
inline DecodeMapPage<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 16331 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *Decoder<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 16335 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::FindPage(typename CONFIG::address_t page_key)
{
	if(mru_page && mru_page->key == page_key) return mru_page;
	uint32_t index = page_key % NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key
	DecodeMapPage<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 16343 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 16377 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 16380 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 16385 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *Decoder<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 16389 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::Decode(typename CONFIG::address_t addr)
{
	Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 16395 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	> *operation;
	typename CONFIG::address_t page_key = addr / 2 / NUM_OPERATIONS_PER_PAGE;
	DecodeMapPage<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 16401 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	> *page;
	page = FindPage(page_key);
	if(!page)
	{
		page = new DecodeMapPage<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
		CONFIG
#line 16409 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
		>(page_key);
		uint32_t index = page_key % NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key
		page->next = decode_hash_table[index];
		decode_hash_table[index] = page;
		mru_page = page;
	}
	operation = page->operation[(addr / 2) & (NUM_OPERATIONS_PER_PAGE - 1)];
	if(operation)
	{
		return operation;
	}
	operation = NCDecode(addr);
	page->operation[(addr / 2) & (NUM_OPERATIONS_PER_PAGE - 1)] = operation;
	return operation;
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 16429 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 16432 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 16437 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
> *Decoder<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 16441 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::Decode(typename CONFIG::address_t addr, CodeType insn)
{
	Operation<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 16447 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	> *operation;
	typename CONFIG::address_t page_key = addr / 2 / NUM_OPERATIONS_PER_PAGE;
	DecodeMapPage<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
	CONFIG
#line 16453 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
	> *page;
	page = FindPage(page_key);
	if(!page)
	{
		page = new DecodeMapPage<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
		CONFIG
#line 16461 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
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
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 16483 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 16486 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
void Decoder<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 16491 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::SetLittleEndian()
{
	little_endian = true;
}

template <
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
class
#line 16500 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 16503 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>
void Decoder<
#line 44 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa/thumb/thumb.isa"
CONFIG
#line 16508 "/local/home/gracia/svn/unisim/devel/unisim_lib/unisim/component/cxx/processor/arm/isa_thumb.tcc"
>::SetBigEndian()
{
	little_endian = false;
}

} } } } } } }
