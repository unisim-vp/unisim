#include "isa_tms320.hh"
#include <cassert>
#line 71 "isa/tms320.isa"

/* code to be included at the beginning of the implementation of the library */
#include <iostream>
#include <string>
#include "unisim/component/cxx/processor/tms320/exception.hh"

namespace unisim {
	namespace component {
		namespace cxx {
			namespace processor {
				namespace tms320 {

					using std::ostream;
					using std::hex;
					using std::dec;
					using std::endl;
					using std::string;

					static const char *REG_NAME[] = {
						"R0",  "R1",  "R2",  "R3",  "R4",  "R5",  "R6",  "R7",
						"AR0", "AR1", "AR2", "AR3", "AR4", "AR5", "AR6", "AR7",
						"DP", "IR0", "IR1",  "BK",  "SP",  "ST",  "IE",  "IF",
						"IOF",  "RS",  "RE",  "RC",    0,     0,     0,     0
					};

					static const char *COND_NAME[] = {
						"U",    // 00000
						"LO",   // 00001
						"LS",   // 00010
						"HI",   // 00011
						"HS",   // 00100
						"EQ",   // 00101
						"NE",   // 00110
						"LT",   // 00111
						"LE",   // 01000
						"GT",   // 01001
						"GE",   // 01010
						0,      // 01011
						"NV",   // 01100
						"V",    // 01101
						"NUF",  // 01110
						"UF",   // 01111
						"NLV",  // 10000
						"LV",   // 10001
						"NLUV", // 10010
						"LUF",  // 10011
						"ZUF",  // 10100
						0,      // 10101
						0,      // 10110
						0,      // 10111
						0,      // 11000
						0,      // 11001
						0,      // 11010
						0,      // 11011
						0,      // 11100
						0,      // 11101
						0,      // 11110
						0       // 11111
					};

				} // end of namespace tms320
			} // end of namespace processor
		} // end of namespace cxx
	} // end of namespace component
} // end of namespace unisim

#line 71 "isa_tms320.tcc"
namespace unisim { namespace component { namespace cxx { namespace processor { namespace tms320 { namespace isa { namespace tms320 {
template <
#line 45 "isa/tms320.isa"
class
#line 76 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 79 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 83 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 86 "isa_tms320.tcc"
>
Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 91 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 95 "isa_tms320.tcc"
>::Operation(CodeType _code, typename CONFIG::address_t _addr, const char *_name)
:
encoding(_code),
addr(_addr),
name(_name)
{
}

template <
#line 45 "isa/tms320.isa"
class
#line 107 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 110 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 114 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 117 "isa_tms320.tcc"
>
Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 122 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 126 "isa_tms320.tcc"
>::~Operation()
{
}

template <
#line 45 "isa/tms320.isa"
class
#line 134 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 137 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 141 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 144 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 148 "isa_tms320.tcc"
Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 152 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 156 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 160 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 163 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 167 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 170 "isa_tms320.tcc"
)
{
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	{
		return false;
	}
#line 177 "isa_tms320.tcc"
}
template <
#line 45 "isa/tms320.isa"
class
#line 182 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 185 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 189 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 192 "isa_tms320.tcc"
>
#line 36 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
void
#line 196 "isa_tms320.tcc"
Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 200 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 204 "isa_tms320.tcc"
>::execute(
#line 36 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 208 "isa_tms320.tcc"
#line 36 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 211 "isa_tms320.tcc"
)
{
#line 36 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	{
		throw UndefinedInstructionException<CONFIG>();
	}
#line 218 "isa_tms320.tcc"
}
template <
#line 45 "isa/tms320.isa"
class
#line 223 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 226 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 230 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 233 "isa_tms320.tcc"
>
class OpLDE_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 238 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 242 "isa_tms320.tcc"
>
{
public:
	OpLDE_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	bool
#line 252 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 256 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 259 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 263 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 266 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 274 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 277 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 281 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 284 "isa_tms320.tcc"
>
class OpLDE_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 289 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 293 "isa_tms320.tcc"
>
{
public:
	OpLDE_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	bool
#line 303 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 307 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 310 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 314 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 317 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 325 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 328 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 332 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 335 "isa_tms320.tcc"
>
class OpLDE_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 340 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 344 "isa_tms320.tcc"
>
{
public:
	OpLDE_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	bool
#line 356 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 360 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 363 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 367 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 370 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 378 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 381 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 385 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 388 "isa_tms320.tcc"
>
class OpLDE_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 393 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 397 "isa_tms320.tcc"
>
{
public:
	OpLDE_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 410 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 413 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 417 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 420 "isa_tms320.tcc"
>
class OpLDF_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 425 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 429 "isa_tms320.tcc"
>
{
public:
	OpLDF_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	bool
#line 439 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 443 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 446 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 450 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 453 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 461 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 464 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 468 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 471 "isa_tms320.tcc"
>
class OpLDF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 476 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 480 "isa_tms320.tcc"
>
{
public:
	OpLDF_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	bool
#line 490 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 494 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 497 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 501 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 504 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 512 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 515 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 519 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 522 "isa_tms320.tcc"
>
class OpLDF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 527 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 531 "isa_tms320.tcc"
>
{
public:
	OpLDF_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	bool
#line 543 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 547 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 550 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 554 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 557 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 565 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 568 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 572 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 575 "isa_tms320.tcc"
>
class OpLDF_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 580 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 584 "isa_tms320.tcc"
>
{
public:
	OpLDF_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 597 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 600 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 604 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 607 "isa_tms320.tcc"
>
class OpLDFcond_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 612 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 616 "isa_tms320.tcc"
>
{
public:
	OpLDFcond_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t cond;
	uint32_t dst;
	uint32_t src;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	bool
#line 627 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 631 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 634 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 638 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 641 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 649 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 652 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 656 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 659 "isa_tms320.tcc"
>
class OpLDFcond_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 664 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 668 "isa_tms320.tcc"
>
{
public:
	OpLDFcond_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t cond;
	uint32_t dst;
	uint32_t direct;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	bool
#line 679 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 683 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 686 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 690 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 693 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 701 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 704 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 708 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 711 "isa_tms320.tcc"
>
class OpLDFcond_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 716 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 720 "isa_tms320.tcc"
>
{
public:
	OpLDFcond_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t cond;
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	bool
#line 733 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 737 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 740 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 744 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 747 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 755 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 758 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 762 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 765 "isa_tms320.tcc"
>
class OpLDFcond_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 770 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 774 "isa_tms320.tcc"
>
{
public:
	OpLDFcond_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t cond;
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 788 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 791 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 795 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 798 "isa_tms320.tcc"
>
class OpLDI_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 803 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 807 "isa_tms320.tcc"
>
{
public:
	OpLDI_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	bool
#line 817 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 821 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 824 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 828 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 831 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 839 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 842 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 846 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 849 "isa_tms320.tcc"
>
class OpLDI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 854 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 858 "isa_tms320.tcc"
>
{
public:
	OpLDI_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	bool
#line 868 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 872 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 875 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 879 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 882 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 890 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 893 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 897 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 900 "isa_tms320.tcc"
>
class OpLDI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 905 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 909 "isa_tms320.tcc"
>
{
public:
	OpLDI_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	bool
#line 921 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 925 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 928 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 932 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 935 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 943 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 946 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 950 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 953 "isa_tms320.tcc"
>
class OpLDI_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 958 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 962 "isa_tms320.tcc"
>
{
public:
	OpLDI_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	bool
#line 972 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 976 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 979 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 983 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 986 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 994 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 997 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1001 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1004 "isa_tms320.tcc"
>
class OpLDIcond_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1009 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1013 "isa_tms320.tcc"
>
{
public:
	OpLDIcond_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t cond;
	uint32_t dst;
	uint32_t src;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	bool
#line 1024 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 1028 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 1031 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 1035 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 1038 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1046 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1049 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1053 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1056 "isa_tms320.tcc"
>
class OpLDIcond_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1061 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1065 "isa_tms320.tcc"
>
{
public:
	OpLDIcond_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t cond;
	uint32_t dst;
	uint32_t direct;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	bool
#line 1076 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 1080 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 1083 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 1087 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 1090 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1098 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1101 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1105 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1108 "isa_tms320.tcc"
>
class OpLDIcond_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1113 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1117 "isa_tms320.tcc"
>
{
public:
	OpLDIcond_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t cond;
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	bool
#line 1130 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 1134 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 1137 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 1141 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 1144 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1152 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1155 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1159 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1162 "isa_tms320.tcc"
>
class OpLDIcond_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1167 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1171 "isa_tms320.tcc"
>
{
public:
	OpLDIcond_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t cond;
	uint32_t dst;
	int32_t imm;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	bool
#line 1182 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 1186 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 1189 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 1193 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 1196 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1204 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1207 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1211 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1214 "isa_tms320.tcc"
>
class OpLDM_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1219 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1223 "isa_tms320.tcc"
>
{
public:
	OpLDM_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	bool
#line 1233 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 1237 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 1240 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 1244 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 1247 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1255 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1258 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1262 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1265 "isa_tms320.tcc"
>
class OpLDM_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1270 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1274 "isa_tms320.tcc"
>
{
public:
	OpLDM_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	bool
#line 1284 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 1288 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 1291 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 1295 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 1298 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1306 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1309 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1313 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1316 "isa_tms320.tcc"
>
class OpLDM_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1321 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1325 "isa_tms320.tcc"
>
{
public:
	OpLDM_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	bool
#line 1337 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 1341 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 1344 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 1348 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 1351 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1359 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1362 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1366 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1369 "isa_tms320.tcc"
>
class OpLDM_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1374 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1378 "isa_tms320.tcc"
>
{
public:
	OpLDM_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1391 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1394 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1398 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1401 "isa_tms320.tcc"
>
class OpLDP : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1406 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1410 "isa_tms320.tcc"
>
{
public:
	OpLDP(CodeType code, typename CONFIG::address_t addr);
	uint32_t src;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	bool
#line 1419 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 1423 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 1426 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 1430 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 1433 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1441 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1444 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1448 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1451 "isa_tms320.tcc"
>
class OpPOP : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1456 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1460 "isa_tms320.tcc"
>
{
public:
	OpPOP(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	bool
#line 1469 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 1473 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 1476 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 1480 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 1483 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1491 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1494 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1498 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1501 "isa_tms320.tcc"
>
class OpPOPF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1506 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1510 "isa_tms320.tcc"
>
{
public:
	OpPOPF(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	bool
#line 1519 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 1523 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 1526 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 1530 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 1533 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1541 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1544 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1548 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1551 "isa_tms320.tcc"
>
class OpPUSH : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1556 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1560 "isa_tms320.tcc"
>
{
public:
	OpPUSH(CodeType code, typename CONFIG::address_t addr);
	uint32_t src;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	bool
#line 1569 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 1573 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 1576 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 1580 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 1583 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1591 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1594 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1598 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1601 "isa_tms320.tcc"
>
class OpPUSHF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1606 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1610 "isa_tms320.tcc"
>
{
public:
	OpPUSHF(CodeType code, typename CONFIG::address_t addr);
	uint32_t src;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	bool
#line 1619 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 1623 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 1626 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 1630 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 1633 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1641 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1644 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1648 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1651 "isa_tms320.tcc"
>
class OpSTF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1656 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1660 "isa_tms320.tcc"
>
{
public:
	OpSTF_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t src;
	uint32_t direct;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	bool
#line 1670 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 1674 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 1677 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 1681 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 1684 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1692 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1695 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1699 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1702 "isa_tms320.tcc"
>
class OpSTF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1707 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1711 "isa_tms320.tcc"
>
{
public:
	OpSTF_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t src;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	bool
#line 1723 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 1727 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 1730 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 1734 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 1737 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1745 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1748 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1752 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1755 "isa_tms320.tcc"
>
class OpSTI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1760 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1764 "isa_tms320.tcc"
>
{
public:
	OpSTI_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t src;
	uint32_t direct;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	bool
#line 1774 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 1778 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 1781 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 1785 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 1788 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1796 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1799 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1803 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1806 "isa_tms320.tcc"
>
class OpSTI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1811 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1815 "isa_tms320.tcc"
>
{
public:
	OpSTI_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t src;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
	virtual
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	bool
#line 1827 "isa_tms320.tcc"
	disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	CPU<CONFIG, DEBUG> &
#line 1831 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	cpu
#line 1834 "isa_tms320.tcc"
	,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	std::ostream &
#line 1838 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
	os
#line 1841 "isa_tms320.tcc"
	);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1849 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1852 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1856 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1859 "isa_tms320.tcc"
>
class OpABSF_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1864 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1868 "isa_tms320.tcc"
>
{
public:
	OpABSF_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1881 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1884 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1888 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1891 "isa_tms320.tcc"
>
class OpABSF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1896 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1900 "isa_tms320.tcc"
>
{
public:
	OpABSF_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1913 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1916 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1920 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1923 "isa_tms320.tcc"
>
class OpABSF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1928 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1932 "isa_tms320.tcc"
>
{
public:
	OpABSF_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1947 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1950 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1954 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1957 "isa_tms320.tcc"
>
class OpABSF_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1962 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1966 "isa_tms320.tcc"
>
{
public:
	OpABSF_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 1979 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 1982 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 1986 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 1989 "isa_tms320.tcc"
>
class OpABSI_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 1994 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 1998 "isa_tms320.tcc"
>
{
public:
	OpABSI_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 2011 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2014 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2018 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2021 "isa_tms320.tcc"
>
class OpABSI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2026 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2030 "isa_tms320.tcc"
>
{
public:
	OpABSI_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 2043 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2046 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2050 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2053 "isa_tms320.tcc"
>
class OpABSI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2058 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2062 "isa_tms320.tcc"
>
{
public:
	OpABSI_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 2077 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2080 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2084 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2087 "isa_tms320.tcc"
>
class OpABSI_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2092 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2096 "isa_tms320.tcc"
>
{
public:
	OpABSI_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 2109 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2112 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2116 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2119 "isa_tms320.tcc"
>
class OpADDC_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2124 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2128 "isa_tms320.tcc"
>
{
public:
	OpADDC_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 2141 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2144 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2148 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2151 "isa_tms320.tcc"
>
class OpADDC_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2156 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2160 "isa_tms320.tcc"
>
{
public:
	OpADDC_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 2173 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2176 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2180 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2183 "isa_tms320.tcc"
>
class OpADDC_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2188 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2192 "isa_tms320.tcc"
>
{
public:
	OpADDC_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 2207 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2210 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2214 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2217 "isa_tms320.tcc"
>
class OpADDC_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2222 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2226 "isa_tms320.tcc"
>
{
public:
	OpADDC_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 2239 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2242 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2246 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2249 "isa_tms320.tcc"
>
class OpADDF_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2254 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2258 "isa_tms320.tcc"
>
{
public:
	OpADDF_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 2271 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2274 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2278 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2281 "isa_tms320.tcc"
>
class OpADDF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2286 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2290 "isa_tms320.tcc"
>
{
public:
	OpADDF_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 2303 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2306 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2310 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2313 "isa_tms320.tcc"
>
class OpADDF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2318 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2322 "isa_tms320.tcc"
>
{
public:
	OpADDF_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 2337 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2340 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2344 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2347 "isa_tms320.tcc"
>
class OpADDF_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2352 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2356 "isa_tms320.tcc"
>
{
public:
	OpADDF_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 2369 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2372 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2376 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2379 "isa_tms320.tcc"
>
class OpADDI_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2384 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2388 "isa_tms320.tcc"
>
{
public:
	OpADDI_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 2401 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2404 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2408 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2411 "isa_tms320.tcc"
>
class OpADDI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2416 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2420 "isa_tms320.tcc"
>
{
public:
	OpADDI_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 2433 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2436 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2440 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2443 "isa_tms320.tcc"
>
class OpADDI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2448 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2452 "isa_tms320.tcc"
>
{
public:
	OpADDI_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 2467 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2470 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2474 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2477 "isa_tms320.tcc"
>
class OpADDI_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2482 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2486 "isa_tms320.tcc"
>
{
public:
	OpADDI_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 2499 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2502 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2506 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2509 "isa_tms320.tcc"
>
class OpAND_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2514 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2518 "isa_tms320.tcc"
>
{
public:
	OpAND_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 2531 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2534 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2538 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2541 "isa_tms320.tcc"
>
class OpAND_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2546 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2550 "isa_tms320.tcc"
>
{
public:
	OpAND_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 2563 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2566 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2570 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2573 "isa_tms320.tcc"
>
class OpAND_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2578 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2582 "isa_tms320.tcc"
>
{
public:
	OpAND_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 2597 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2600 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2604 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2607 "isa_tms320.tcc"
>
class OpAND_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2612 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2616 "isa_tms320.tcc"
>
{
public:
	OpAND_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 2629 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2632 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2636 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2639 "isa_tms320.tcc"
>
class OpANDN_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2644 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2648 "isa_tms320.tcc"
>
{
public:
	OpANDN_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 2661 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2664 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2668 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2671 "isa_tms320.tcc"
>
class OpANDN_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2676 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2680 "isa_tms320.tcc"
>
{
public:
	OpANDN_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 2693 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2696 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2700 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2703 "isa_tms320.tcc"
>
class OpANDN_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2708 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2712 "isa_tms320.tcc"
>
{
public:
	OpANDN_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 2727 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2730 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2734 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2737 "isa_tms320.tcc"
>
class OpANDN_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2742 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2746 "isa_tms320.tcc"
>
{
public:
	OpANDN_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 2759 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2762 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2766 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2769 "isa_tms320.tcc"
>
class OpASH_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2774 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2778 "isa_tms320.tcc"
>
{
public:
	OpASH_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 2791 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2794 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2798 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2801 "isa_tms320.tcc"
>
class OpASH_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2806 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2810 "isa_tms320.tcc"
>
{
public:
	OpASH_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 2823 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2826 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2830 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2833 "isa_tms320.tcc"
>
class OpASH_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2838 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2842 "isa_tms320.tcc"
>
{
public:
	OpASH_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 2857 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2860 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2864 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2867 "isa_tms320.tcc"
>
class OpASH_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2872 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2876 "isa_tms320.tcc"
>
{
public:
	OpASH_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 2889 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2892 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2896 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2899 "isa_tms320.tcc"
>
class OpCMPF_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2904 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2908 "isa_tms320.tcc"
>
{
public:
	OpCMPF_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 2921 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2924 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2928 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2931 "isa_tms320.tcc"
>
class OpCMPF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2936 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2940 "isa_tms320.tcc"
>
{
public:
	OpCMPF_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 2953 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2956 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2960 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2963 "isa_tms320.tcc"
>
class OpCMPF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 2968 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 2972 "isa_tms320.tcc"
>
{
public:
	OpCMPF_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 2987 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 2990 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 2994 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 2997 "isa_tms320.tcc"
>
class OpCMPF_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3002 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3006 "isa_tms320.tcc"
>
{
public:
	OpCMPF_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 3019 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3022 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3026 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3029 "isa_tms320.tcc"
>
class OpCMPI_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3034 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3038 "isa_tms320.tcc"
>
{
public:
	OpCMPI_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 3051 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3054 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3058 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3061 "isa_tms320.tcc"
>
class OpCMPI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3066 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3070 "isa_tms320.tcc"
>
{
public:
	OpCMPI_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 3083 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3086 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3090 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3093 "isa_tms320.tcc"
>
class OpCMPI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3098 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3102 "isa_tms320.tcc"
>
{
public:
	OpCMPI_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 3117 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3120 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3124 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3127 "isa_tms320.tcc"
>
class OpCMPI_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3132 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3136 "isa_tms320.tcc"
>
{
public:
	OpCMPI_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 3149 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3152 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3156 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3159 "isa_tms320.tcc"
>
class OpFIX_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3164 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3168 "isa_tms320.tcc"
>
{
public:
	OpFIX_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 3181 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3184 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3188 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3191 "isa_tms320.tcc"
>
class OpFIX_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3196 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3200 "isa_tms320.tcc"
>
{
public:
	OpFIX_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 3213 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3216 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3220 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3223 "isa_tms320.tcc"
>
class OpFIX_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3228 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3232 "isa_tms320.tcc"
>
{
public:
	OpFIX_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 3247 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3250 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3254 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3257 "isa_tms320.tcc"
>
class OpFIX_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3262 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3266 "isa_tms320.tcc"
>
{
public:
	OpFIX_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 3279 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3282 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3286 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3289 "isa_tms320.tcc"
>
class OpFLOAT_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3294 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3298 "isa_tms320.tcc"
>
{
public:
	OpFLOAT_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 3311 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3314 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3318 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3321 "isa_tms320.tcc"
>
class OpFLOAT_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3326 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3330 "isa_tms320.tcc"
>
{
public:
	OpFLOAT_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 3343 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3346 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3350 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3353 "isa_tms320.tcc"
>
class OpFLOAT_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3358 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3362 "isa_tms320.tcc"
>
{
public:
	OpFLOAT_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 3377 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3380 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3384 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3387 "isa_tms320.tcc"
>
class OpFLOAT_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3392 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3396 "isa_tms320.tcc"
>
{
public:
	OpFLOAT_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 3409 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3412 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3416 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3419 "isa_tms320.tcc"
>
class OpLSH_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3424 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3428 "isa_tms320.tcc"
>
{
public:
	OpLSH_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 3441 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3444 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3448 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3451 "isa_tms320.tcc"
>
class OpLSH_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3456 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3460 "isa_tms320.tcc"
>
{
public:
	OpLSH_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 3473 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3476 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3480 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3483 "isa_tms320.tcc"
>
class OpLSH_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3488 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3492 "isa_tms320.tcc"
>
{
public:
	OpLSH_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 3507 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3510 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3514 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3517 "isa_tms320.tcc"
>
class OpLSH_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3522 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3526 "isa_tms320.tcc"
>
{
public:
	OpLSH_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 3539 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3542 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3546 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3549 "isa_tms320.tcc"
>
class OpMPYF_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3554 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3558 "isa_tms320.tcc"
>
{
public:
	OpMPYF_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 3571 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3574 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3578 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3581 "isa_tms320.tcc"
>
class OpMPYF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3586 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3590 "isa_tms320.tcc"
>
{
public:
	OpMPYF_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 3603 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3606 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3610 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3613 "isa_tms320.tcc"
>
class OpMPYF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3618 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3622 "isa_tms320.tcc"
>
{
public:
	OpMPYF_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 3637 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3640 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3644 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3647 "isa_tms320.tcc"
>
class OpMPYF_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3652 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3656 "isa_tms320.tcc"
>
{
public:
	OpMPYF_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 3669 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3672 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3676 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3679 "isa_tms320.tcc"
>
class OpMPYI_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3684 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3688 "isa_tms320.tcc"
>
{
public:
	OpMPYI_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 3701 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3704 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3708 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3711 "isa_tms320.tcc"
>
class OpMPYI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3716 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3720 "isa_tms320.tcc"
>
{
public:
	OpMPYI_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 3733 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3736 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3740 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3743 "isa_tms320.tcc"
>
class OpMPYI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3748 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3752 "isa_tms320.tcc"
>
{
public:
	OpMPYI_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 3767 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3770 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3774 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3777 "isa_tms320.tcc"
>
class OpMPYI_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3782 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3786 "isa_tms320.tcc"
>
{
public:
	OpMPYI_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 3799 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3802 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3806 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3809 "isa_tms320.tcc"
>
class OpNEGB_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3814 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3818 "isa_tms320.tcc"
>
{
public:
	OpNEGB_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 3831 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3834 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3838 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3841 "isa_tms320.tcc"
>
class OpNEGB_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3846 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3850 "isa_tms320.tcc"
>
{
public:
	OpNEGB_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 3863 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3866 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3870 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3873 "isa_tms320.tcc"
>
class OpNEGB_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3878 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3882 "isa_tms320.tcc"
>
{
public:
	OpNEGB_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 3897 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3900 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3904 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3907 "isa_tms320.tcc"
>
class OpNEGB_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3912 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3916 "isa_tms320.tcc"
>
{
public:
	OpNEGB_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 3929 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3932 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3936 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3939 "isa_tms320.tcc"
>
class OpNEGF_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3944 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3948 "isa_tms320.tcc"
>
{
public:
	OpNEGF_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 3961 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3964 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 3968 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 3971 "isa_tms320.tcc"
>
class OpNEGF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 3976 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 3980 "isa_tms320.tcc"
>
{
public:
	OpNEGF_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 3993 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 3996 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4000 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4003 "isa_tms320.tcc"
>
class OpNEGF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4008 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4012 "isa_tms320.tcc"
>
{
public:
	OpNEGF_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 4027 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4030 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4034 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4037 "isa_tms320.tcc"
>
class OpNEGF_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4042 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4046 "isa_tms320.tcc"
>
{
public:
	OpNEGF_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 4059 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4062 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4066 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4069 "isa_tms320.tcc"
>
class OpNEGI_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4074 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4078 "isa_tms320.tcc"
>
{
public:
	OpNEGI_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 4091 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4094 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4098 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4101 "isa_tms320.tcc"
>
class OpNEGI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4106 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4110 "isa_tms320.tcc"
>
{
public:
	OpNEGI_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 4123 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4126 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4130 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4133 "isa_tms320.tcc"
>
class OpNEGI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4138 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4142 "isa_tms320.tcc"
>
{
public:
	OpNEGI_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 4157 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4160 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4164 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4167 "isa_tms320.tcc"
>
class OpNEGI_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4172 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4176 "isa_tms320.tcc"
>
{
public:
	OpNEGI_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 4189 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4192 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4196 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4199 "isa_tms320.tcc"
>
class OpNORM_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4204 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4208 "isa_tms320.tcc"
>
{
public:
	OpNORM_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 4221 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4224 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4228 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4231 "isa_tms320.tcc"
>
class OpNORM_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4236 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4240 "isa_tms320.tcc"
>
{
public:
	OpNORM_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 4253 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4256 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4260 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4263 "isa_tms320.tcc"
>
class OpNORM_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4268 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4272 "isa_tms320.tcc"
>
{
public:
	OpNORM_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 4287 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4290 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4294 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4297 "isa_tms320.tcc"
>
class OpNORM_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4302 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4306 "isa_tms320.tcc"
>
{
public:
	OpNORM_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 4319 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4322 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4326 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4329 "isa_tms320.tcc"
>
class OpNOT_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4334 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4338 "isa_tms320.tcc"
>
{
public:
	OpNOT_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 4351 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4354 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4358 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4361 "isa_tms320.tcc"
>
class OpNOT_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4366 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4370 "isa_tms320.tcc"
>
{
public:
	OpNOT_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 4383 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4386 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4390 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4393 "isa_tms320.tcc"
>
class OpNOT_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4398 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4402 "isa_tms320.tcc"
>
{
public:
	OpNOT_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 4417 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4420 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4424 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4427 "isa_tms320.tcc"
>
class OpNOT_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4432 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4436 "isa_tms320.tcc"
>
{
public:
	OpNOT_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 4449 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4452 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4456 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4459 "isa_tms320.tcc"
>
class OpOR_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4464 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4468 "isa_tms320.tcc"
>
{
public:
	OpOR_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 4481 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4484 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4488 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4491 "isa_tms320.tcc"
>
class OpOR_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4496 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4500 "isa_tms320.tcc"
>
{
public:
	OpOR_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 4513 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4516 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4520 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4523 "isa_tms320.tcc"
>
class OpOR_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4528 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4532 "isa_tms320.tcc"
>
{
public:
	OpOR_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 4547 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4550 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4554 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4557 "isa_tms320.tcc"
>
class OpOR_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4562 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4566 "isa_tms320.tcc"
>
{
public:
	OpOR_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 4579 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4582 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4586 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4589 "isa_tms320.tcc"
>
class OpRND_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4594 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4598 "isa_tms320.tcc"
>
{
public:
	OpRND_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 4611 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4614 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4618 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4621 "isa_tms320.tcc"
>
class OpRND_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4626 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4630 "isa_tms320.tcc"
>
{
public:
	OpRND_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 4643 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4646 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4650 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4653 "isa_tms320.tcc"
>
class OpRND_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4658 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4662 "isa_tms320.tcc"
>
{
public:
	OpRND_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 4677 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4680 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4684 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4687 "isa_tms320.tcc"
>
class OpRND_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4692 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4696 "isa_tms320.tcc"
>
{
public:
	OpRND_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 4709 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4712 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4716 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4719 "isa_tms320.tcc"
>
class OpROL : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4724 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4728 "isa_tms320.tcc"
>
{
public:
	OpROL(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 4740 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4743 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4747 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4750 "isa_tms320.tcc"
>
class OpROLC : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4755 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4759 "isa_tms320.tcc"
>
{
public:
	OpROLC(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 4771 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4774 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4778 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4781 "isa_tms320.tcc"
>
class OpROR : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4786 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4790 "isa_tms320.tcc"
>
{
public:
	OpROR(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 4802 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4805 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4809 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4812 "isa_tms320.tcc"
>
class OpRORC : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4817 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4821 "isa_tms320.tcc"
>
{
public:
	OpRORC(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 4833 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4836 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4840 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4843 "isa_tms320.tcc"
>
class OpSUBB_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4848 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4852 "isa_tms320.tcc"
>
{
public:
	OpSUBB_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 4865 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4868 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4872 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4875 "isa_tms320.tcc"
>
class OpSUBB_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4880 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4884 "isa_tms320.tcc"
>
{
public:
	OpSUBB_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 4897 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4900 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4904 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4907 "isa_tms320.tcc"
>
class OpSUBB_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4912 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4916 "isa_tms320.tcc"
>
{
public:
	OpSUBB_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 4931 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4934 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4938 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4941 "isa_tms320.tcc"
>
class OpSUBB_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4946 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4950 "isa_tms320.tcc"
>
{
public:
	OpSUBB_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 4963 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4966 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 4970 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 4973 "isa_tms320.tcc"
>
class OpSUBC_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 4978 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 4982 "isa_tms320.tcc"
>
{
public:
	OpSUBC_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 4995 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 4998 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5002 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5005 "isa_tms320.tcc"
>
class OpSUBC_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5010 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5014 "isa_tms320.tcc"
>
{
public:
	OpSUBC_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 5027 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5030 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5034 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5037 "isa_tms320.tcc"
>
class OpSUBC_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5042 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5046 "isa_tms320.tcc"
>
{
public:
	OpSUBC_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 5061 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5064 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5068 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5071 "isa_tms320.tcc"
>
class OpSUBC_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5076 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5080 "isa_tms320.tcc"
>
{
public:
	OpSUBC_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 5093 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5096 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5100 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5103 "isa_tms320.tcc"
>
class OpSUBF_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5108 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5112 "isa_tms320.tcc"
>
{
public:
	OpSUBF_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 5125 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5128 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5132 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5135 "isa_tms320.tcc"
>
class OpSUBF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5140 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5144 "isa_tms320.tcc"
>
{
public:
	OpSUBF_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 5157 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5160 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5164 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5167 "isa_tms320.tcc"
>
class OpSUBF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5172 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5176 "isa_tms320.tcc"
>
{
public:
	OpSUBF_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 5191 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5194 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5198 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5201 "isa_tms320.tcc"
>
class OpSUBF_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5206 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5210 "isa_tms320.tcc"
>
{
public:
	OpSUBF_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 5223 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5226 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5230 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5233 "isa_tms320.tcc"
>
class OpSUBI_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5238 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5242 "isa_tms320.tcc"
>
{
public:
	OpSUBI_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 5255 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5258 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5262 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5265 "isa_tms320.tcc"
>
class OpSUBI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5270 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5274 "isa_tms320.tcc"
>
{
public:
	OpSUBI_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 5287 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5290 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5294 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5297 "isa_tms320.tcc"
>
class OpSUBI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5302 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5306 "isa_tms320.tcc"
>
{
public:
	OpSUBI_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 5321 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5324 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5328 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5331 "isa_tms320.tcc"
>
class OpSUBI_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5336 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5340 "isa_tms320.tcc"
>
{
public:
	OpSUBI_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 5353 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5356 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5360 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5363 "isa_tms320.tcc"
>
class OpSUBRB_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5368 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5372 "isa_tms320.tcc"
>
{
public:
	OpSUBRB_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 5385 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5388 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5392 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5395 "isa_tms320.tcc"
>
class OpSUBRB_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5400 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5404 "isa_tms320.tcc"
>
{
public:
	OpSUBRB_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 5417 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5420 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5424 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5427 "isa_tms320.tcc"
>
class OpSUBRB_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5432 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5436 "isa_tms320.tcc"
>
{
public:
	OpSUBRB_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 5451 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5454 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5458 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5461 "isa_tms320.tcc"
>
class OpSUBRB_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5466 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5470 "isa_tms320.tcc"
>
{
public:
	OpSUBRB_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 5483 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5486 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5490 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5493 "isa_tms320.tcc"
>
class OpSUBRF_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5498 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5502 "isa_tms320.tcc"
>
{
public:
	OpSUBRF_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 5515 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5518 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5522 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5525 "isa_tms320.tcc"
>
class OpSUBRF_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5530 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5534 "isa_tms320.tcc"
>
{
public:
	OpSUBRF_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 5547 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5550 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5554 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5557 "isa_tms320.tcc"
>
class OpSUBRF_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5562 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5566 "isa_tms320.tcc"
>
{
public:
	OpSUBRF_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 5581 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5584 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5588 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5591 "isa_tms320.tcc"
>
class OpSUBRF_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5596 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5600 "isa_tms320.tcc"
>
{
public:
	OpSUBRF_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 5613 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5616 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5620 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5623 "isa_tms320.tcc"
>
class OpSUBRI_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5628 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5632 "isa_tms320.tcc"
>
{
public:
	OpSUBRI_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 5645 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5648 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5652 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5655 "isa_tms320.tcc"
>
class OpSUBRI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5660 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5664 "isa_tms320.tcc"
>
{
public:
	OpSUBRI_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 5677 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5680 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5684 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5687 "isa_tms320.tcc"
>
class OpSUBRI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5692 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5696 "isa_tms320.tcc"
>
{
public:
	OpSUBRI_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 5711 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5714 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5718 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5721 "isa_tms320.tcc"
>
class OpSUBRI_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5726 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5730 "isa_tms320.tcc"
>
{
public:
	OpSUBRI_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 5743 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5746 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5750 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5753 "isa_tms320.tcc"
>
class OpTSTB_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5758 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5762 "isa_tms320.tcc"
>
{
public:
	OpTSTB_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 5775 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5778 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5782 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5785 "isa_tms320.tcc"
>
class OpTSTB_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5790 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5794 "isa_tms320.tcc"
>
{
public:
	OpTSTB_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 5807 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5810 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5814 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5817 "isa_tms320.tcc"
>
class OpTSTB_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5822 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5826 "isa_tms320.tcc"
>
{
public:
	OpTSTB_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 5841 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5844 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5848 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5851 "isa_tms320.tcc"
>
class OpTSTB_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5856 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5860 "isa_tms320.tcc"
>
{
public:
	OpTSTB_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 5873 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5876 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5880 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5883 "isa_tms320.tcc"
>
class OpXOR_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5888 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5892 "isa_tms320.tcc"
>
{
public:
	OpXOR_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 5905 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5908 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5912 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5915 "isa_tms320.tcc"
>
class OpXOR_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5920 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5924 "isa_tms320.tcc"
>
{
public:
	OpXOR_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 5937 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5940 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5944 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5947 "isa_tms320.tcc"
>
class OpXOR_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5952 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5956 "isa_tms320.tcc"
>
{
public:
	OpXOR_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 5971 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 5974 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 5978 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 5981 "isa_tms320.tcc"
>
class OpXOR_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 5986 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 5990 "isa_tms320.tcc"
>
{
public:
	OpXOR_imm(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6003 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6006 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6010 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6013 "isa_tms320.tcc"
>
class OpADDC3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6018 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6022 "isa_tms320.tcc"
>
{
public:
	OpADDC3_reg_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6036 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6039 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6043 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6046 "isa_tms320.tcc"
>
class OpADDC3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6051 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6055 "isa_tms320.tcc"
>
{
public:
	OpADDC3_indir_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6070 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6073 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6077 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6080 "isa_tms320.tcc"
>
class OpADDC3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6085 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6089 "isa_tms320.tcc"
>
{
public:
	OpADDC3_reg_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6104 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6107 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6111 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6114 "isa_tms320.tcc"
>
class OpADDC3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6119 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6123 "isa_tms320.tcc"
>
{
public:
	OpADDC3_indir_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6139 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6142 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6146 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6149 "isa_tms320.tcc"
>
class OpADDF3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6154 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6158 "isa_tms320.tcc"
>
{
public:
	OpADDF3_reg_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6172 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6175 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6179 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6182 "isa_tms320.tcc"
>
class OpADDF3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6187 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6191 "isa_tms320.tcc"
>
{
public:
	OpADDF3_indir_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6206 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6209 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6213 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6216 "isa_tms320.tcc"
>
class OpADDF3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6221 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6225 "isa_tms320.tcc"
>
{
public:
	OpADDF3_reg_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6240 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6243 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6247 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6250 "isa_tms320.tcc"
>
class OpADDF3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6255 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6259 "isa_tms320.tcc"
>
{
public:
	OpADDF3_indir_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6275 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6278 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6282 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6285 "isa_tms320.tcc"
>
class OpADDI3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6290 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6294 "isa_tms320.tcc"
>
{
public:
	OpADDI3_reg_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6308 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6311 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6315 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6318 "isa_tms320.tcc"
>
class OpADDI3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6323 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6327 "isa_tms320.tcc"
>
{
public:
	OpADDI3_indir_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6342 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6345 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6349 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6352 "isa_tms320.tcc"
>
class OpADDI3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6357 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6361 "isa_tms320.tcc"
>
{
public:
	OpADDI3_reg_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6376 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6379 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6383 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6386 "isa_tms320.tcc"
>
class OpADDI3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6391 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6395 "isa_tms320.tcc"
>
{
public:
	OpADDI3_indir_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6411 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6414 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6418 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6421 "isa_tms320.tcc"
>
class OpAND3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6426 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6430 "isa_tms320.tcc"
>
{
public:
	OpAND3_reg_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6444 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6447 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6451 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6454 "isa_tms320.tcc"
>
class OpAND3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6459 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6463 "isa_tms320.tcc"
>
{
public:
	OpAND3_indir_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6478 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6481 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6485 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6488 "isa_tms320.tcc"
>
class OpAND3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6493 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6497 "isa_tms320.tcc"
>
{
public:
	OpAND3_reg_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6512 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6515 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6519 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6522 "isa_tms320.tcc"
>
class OpAND3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6527 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6531 "isa_tms320.tcc"
>
{
public:
	OpAND3_indir_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6547 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6550 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6554 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6557 "isa_tms320.tcc"
>
class OpANDN3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6562 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6566 "isa_tms320.tcc"
>
{
public:
	OpANDN3_reg_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6580 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6583 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6587 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6590 "isa_tms320.tcc"
>
class OpANDN3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6595 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6599 "isa_tms320.tcc"
>
{
public:
	OpANDN3_indir_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6614 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6617 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6621 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6624 "isa_tms320.tcc"
>
class OpANDN3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6629 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6633 "isa_tms320.tcc"
>
{
public:
	OpANDN3_reg_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6648 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6651 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6655 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6658 "isa_tms320.tcc"
>
class OpANDN3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6663 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6667 "isa_tms320.tcc"
>
{
public:
	OpANDN3_indir_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6683 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6686 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6690 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6693 "isa_tms320.tcc"
>
class OpASH3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6698 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6702 "isa_tms320.tcc"
>
{
public:
	OpASH3_reg_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6716 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6719 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6723 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6726 "isa_tms320.tcc"
>
class OpASH3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6731 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6735 "isa_tms320.tcc"
>
{
public:
	OpASH3_indir_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6750 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6753 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6757 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6760 "isa_tms320.tcc"
>
class OpASH3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6765 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6769 "isa_tms320.tcc"
>
{
public:
	OpASH3_reg_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6784 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6787 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6791 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6794 "isa_tms320.tcc"
>
class OpASH3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6799 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6803 "isa_tms320.tcc"
>
{
public:
	OpASH3_indir_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6819 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6822 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6826 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6829 "isa_tms320.tcc"
>
class OpCMPF3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6834 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6838 "isa_tms320.tcc"
>
{
public:
	OpCMPF3_reg_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6852 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6855 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6859 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6862 "isa_tms320.tcc"
>
class OpCMPF3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6867 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6871 "isa_tms320.tcc"
>
{
public:
	OpCMPF3_indir_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6886 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6889 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6893 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6896 "isa_tms320.tcc"
>
class OpCMPF3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6901 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6905 "isa_tms320.tcc"
>
{
public:
	OpCMPF3_reg_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6920 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6923 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6927 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6930 "isa_tms320.tcc"
>
class OpCMPF3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6935 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6939 "isa_tms320.tcc"
>
{
public:
	OpCMPF3_indir_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6955 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6958 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6962 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6965 "isa_tms320.tcc"
>
class OpCMPI3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 6970 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 6974 "isa_tms320.tcc"
>
{
public:
	OpCMPI3_reg_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 6988 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 6991 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 6995 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 6998 "isa_tms320.tcc"
>
class OpCMPI3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7003 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7007 "isa_tms320.tcc"
>
{
public:
	OpCMPI3_indir_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7022 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7025 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7029 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7032 "isa_tms320.tcc"
>
class OpCMPI3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7037 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7041 "isa_tms320.tcc"
>
{
public:
	OpCMPI3_reg_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7056 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7059 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7063 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7066 "isa_tms320.tcc"
>
class OpCMPI3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7071 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7075 "isa_tms320.tcc"
>
{
public:
	OpCMPI3_indir_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7091 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7094 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7098 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7101 "isa_tms320.tcc"
>
class OpLSH3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7106 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7110 "isa_tms320.tcc"
>
{
public:
	OpLSH3_reg_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7124 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7127 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7131 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7134 "isa_tms320.tcc"
>
class OpLSH3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7139 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7143 "isa_tms320.tcc"
>
{
public:
	OpLSH3_indir_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7158 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7161 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7165 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7168 "isa_tms320.tcc"
>
class OpLSH3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7173 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7177 "isa_tms320.tcc"
>
{
public:
	OpLSH3_reg_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7192 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7195 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7199 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7202 "isa_tms320.tcc"
>
class OpLSH3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7207 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7211 "isa_tms320.tcc"
>
{
public:
	OpLSH3_indir_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7227 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7230 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7234 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7237 "isa_tms320.tcc"
>
class OpMPYF3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7242 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7246 "isa_tms320.tcc"
>
{
public:
	OpMPYF3_reg_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7260 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7263 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7267 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7270 "isa_tms320.tcc"
>
class OpMPYF3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7275 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7279 "isa_tms320.tcc"
>
{
public:
	OpMPYF3_indir_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7294 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7297 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7301 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7304 "isa_tms320.tcc"
>
class OpMPYF3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7309 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7313 "isa_tms320.tcc"
>
{
public:
	OpMPYF3_reg_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7328 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7331 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7335 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7338 "isa_tms320.tcc"
>
class OpMPYF3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7343 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7347 "isa_tms320.tcc"
>
{
public:
	OpMPYF3_indir_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7363 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7366 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7370 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7373 "isa_tms320.tcc"
>
class OpMPYI3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7378 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7382 "isa_tms320.tcc"
>
{
public:
	OpMPYI3_reg_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7396 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7399 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7403 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7406 "isa_tms320.tcc"
>
class OpMPYI3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7411 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7415 "isa_tms320.tcc"
>
{
public:
	OpMPYI3_indir_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7430 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7433 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7437 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7440 "isa_tms320.tcc"
>
class OpMPYI3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7445 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7449 "isa_tms320.tcc"
>
{
public:
	OpMPYI3_reg_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7464 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7467 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7471 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7474 "isa_tms320.tcc"
>
class OpMPYI3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7479 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7483 "isa_tms320.tcc"
>
{
public:
	OpMPYI3_indir_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7499 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7502 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7506 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7509 "isa_tms320.tcc"
>
class OpOR3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7514 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7518 "isa_tms320.tcc"
>
{
public:
	OpOR3_reg_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7532 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7535 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7539 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7542 "isa_tms320.tcc"
>
class OpOR3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7547 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7551 "isa_tms320.tcc"
>
{
public:
	OpOR3_indir_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7566 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7569 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7573 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7576 "isa_tms320.tcc"
>
class OpOR3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7581 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7585 "isa_tms320.tcc"
>
{
public:
	OpOR3_reg_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7600 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7603 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7607 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7610 "isa_tms320.tcc"
>
class OpOR3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7615 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7619 "isa_tms320.tcc"
>
{
public:
	OpOR3_indir_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7635 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7638 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7642 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7645 "isa_tms320.tcc"
>
class OpSUBB3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7650 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7654 "isa_tms320.tcc"
>
{
public:
	OpSUBB3_reg_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7668 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7671 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7675 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7678 "isa_tms320.tcc"
>
class OpSUBB3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7683 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7687 "isa_tms320.tcc"
>
{
public:
	OpSUBB3_indir_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7702 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7705 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7709 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7712 "isa_tms320.tcc"
>
class OpSUBB3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7717 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7721 "isa_tms320.tcc"
>
{
public:
	OpSUBB3_reg_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7736 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7739 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7743 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7746 "isa_tms320.tcc"
>
class OpSUBB3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7751 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7755 "isa_tms320.tcc"
>
{
public:
	OpSUBB3_indir_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7771 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7774 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7778 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7781 "isa_tms320.tcc"
>
class OpSUBF3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7786 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7790 "isa_tms320.tcc"
>
{
public:
	OpSUBF3_reg_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7804 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7807 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7811 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7814 "isa_tms320.tcc"
>
class OpSUBF3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7819 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7823 "isa_tms320.tcc"
>
{
public:
	OpSUBF3_indir_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7838 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7841 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7845 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7848 "isa_tms320.tcc"
>
class OpSUBF3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7853 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7857 "isa_tms320.tcc"
>
{
public:
	OpSUBF3_reg_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7872 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7875 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7879 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7882 "isa_tms320.tcc"
>
class OpSUBF3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7887 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7891 "isa_tms320.tcc"
>
{
public:
	OpSUBF3_indir_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7907 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7910 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7914 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7917 "isa_tms320.tcc"
>
class OpSUBI3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7922 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7926 "isa_tms320.tcc"
>
{
public:
	OpSUBI3_reg_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7940 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7943 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7947 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7950 "isa_tms320.tcc"
>
class OpSUBI3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7955 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7959 "isa_tms320.tcc"
>
{
public:
	OpSUBI3_indir_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 7974 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 7977 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 7981 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 7984 "isa_tms320.tcc"
>
class OpSUBI3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 7989 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 7993 "isa_tms320.tcc"
>
{
public:
	OpSUBI3_reg_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8008 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8011 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8015 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8018 "isa_tms320.tcc"
>
class OpSUBI3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8023 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8027 "isa_tms320.tcc"
>
{
public:
	OpSUBI3_indir_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8043 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8046 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8050 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8053 "isa_tms320.tcc"
>
class OpTSTB3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8058 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8062 "isa_tms320.tcc"
>
{
public:
	OpTSTB3_reg_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8076 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8079 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8083 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8086 "isa_tms320.tcc"
>
class OpTSTB3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8091 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8095 "isa_tms320.tcc"
>
{
public:
	OpTSTB3_indir_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8110 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8113 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8117 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8120 "isa_tms320.tcc"
>
class OpTSTB3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8125 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8129 "isa_tms320.tcc"
>
{
public:
	OpTSTB3_reg_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8144 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8147 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8151 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8154 "isa_tms320.tcc"
>
class OpTSTB3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8159 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8163 "isa_tms320.tcc"
>
{
public:
	OpTSTB3_indir_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8179 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8182 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8186 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8189 "isa_tms320.tcc"
>
class OpXOR3_reg_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8194 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8198 "isa_tms320.tcc"
>
{
public:
	OpXOR3_reg_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8212 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8215 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8219 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8222 "isa_tms320.tcc"
>
class OpXOR3_indir_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8227 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8231 "isa_tms320.tcc"
>
{
public:
	OpXOR3_indir_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8246 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8249 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8253 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8256 "isa_tms320.tcc"
>
class OpXOR3_reg_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8261 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8265 "isa_tms320.tcc"
>
{
public:
	OpXOR3_reg_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t src1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8280 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8283 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8287 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8290 "isa_tms320.tcc"
>
class OpXOR3_indir_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8295 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8299 "isa_tms320.tcc"
>
{
public:
	OpXOR3_indir_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod1;
	uint32_t ar1;
	uint32_t mod2;
	uint32_t ar2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8315 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8318 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8322 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8325 "isa_tms320.tcc"
>
class OpBcond_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8330 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8334 "isa_tms320.tcc"
>
{
public:
	OpBcond_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t cond;
	uint32_t reg;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8347 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8350 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8354 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8357 "isa_tms320.tcc"
>
class OpBcond_disp : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8362 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8366 "isa_tms320.tcc"
>
{
public:
	OpBcond_disp(CodeType code, typename CONFIG::address_t addr);
	uint32_t cond;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8379 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8382 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8386 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8389 "isa_tms320.tcc"
>
class OpBcondD_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8394 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8398 "isa_tms320.tcc"
>
{
public:
	OpBcondD_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t cond;
	uint32_t reg;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8411 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8414 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8418 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8421 "isa_tms320.tcc"
>
class OpBcondD_disp : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8426 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8430 "isa_tms320.tcc"
>
{
public:
	OpBcondD_disp(CodeType code, typename CONFIG::address_t addr);
	uint32_t cond;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8443 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8446 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8450 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8453 "isa_tms320.tcc"
>
class OpBR : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8458 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8462 "isa_tms320.tcc"
>
{
public:
	OpBR(CodeType code, typename CONFIG::address_t addr);
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8474 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8477 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8481 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8484 "isa_tms320.tcc"
>
class OpBRD : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8489 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8493 "isa_tms320.tcc"
>
{
public:
	OpBRD(CodeType code, typename CONFIG::address_t addr);
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8505 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8508 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8512 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8515 "isa_tms320.tcc"
>
class OpCALL : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8520 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8524 "isa_tms320.tcc"
>
{
public:
	OpCALL(CodeType code, typename CONFIG::address_t addr);
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8536 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8539 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8543 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8546 "isa_tms320.tcc"
>
class OpCALLcond_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8551 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8555 "isa_tms320.tcc"
>
{
public:
	OpCALLcond_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t cond;
	uint32_t reg;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8568 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8571 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8575 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8578 "isa_tms320.tcc"
>
class OpCALLcond_disp : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8583 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8587 "isa_tms320.tcc"
>
{
public:
	OpCALLcond_disp(CodeType code, typename CONFIG::address_t addr);
	uint32_t cond;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8600 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8603 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8607 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8610 "isa_tms320.tcc"
>
class OpDBcond_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8615 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8619 "isa_tms320.tcc"
>
{
public:
	OpDBcond_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t ar;
	uint32_t cond;
	uint32_t reg;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8633 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8636 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8640 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8643 "isa_tms320.tcc"
>
class OpDBcond_disp : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8648 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8652 "isa_tms320.tcc"
>
{
public:
	OpDBcond_disp(CodeType code, typename CONFIG::address_t addr);
	uint32_t ar;
	uint32_t cond;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8666 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8669 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8673 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8676 "isa_tms320.tcc"
>
class OpDBcondD_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8681 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8685 "isa_tms320.tcc"
>
{
public:
	OpDBcondD_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t ar;
	uint32_t cond;
	uint32_t reg;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8699 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8702 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8706 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8709 "isa_tms320.tcc"
>
class OpDBcondD_disp : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8714 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8718 "isa_tms320.tcc"
>
{
public:
	OpDBcondD_disp(CodeType code, typename CONFIG::address_t addr);
	uint32_t ar;
	uint32_t cond;
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8732 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8735 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8739 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8742 "isa_tms320.tcc"
>
class OpIACK_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8747 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8751 "isa_tms320.tcc"
>
{
public:
	OpIACK_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8763 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8766 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8770 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8773 "isa_tms320.tcc"
>
class OpIACK_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8778 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8782 "isa_tms320.tcc"
>
{
public:
	OpIACK_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8796 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8799 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8803 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8806 "isa_tms320.tcc"
>
class OpIDLE : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8811 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8815 "isa_tms320.tcc"
>
{
public:
	OpIDLE(CodeType code, typename CONFIG::address_t addr);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8826 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8829 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8833 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8836 "isa_tms320.tcc"
>
class OpNOP_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8841 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8845 "isa_tms320.tcc"
>
{
public:
	OpNOP_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8857 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8860 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8864 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8867 "isa_tms320.tcc"
>
class OpNOP_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8872 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8876 "isa_tms320.tcc"
>
{
public:
	OpNOP_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8890 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8893 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8897 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8900 "isa_tms320.tcc"
>
class OpRETIcond : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8905 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8909 "isa_tms320.tcc"
>
{
public:
	OpRETIcond(CodeType code, typename CONFIG::address_t addr);
	uint32_t cond;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8921 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8924 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8928 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8931 "isa_tms320.tcc"
>
class OpRETScond : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8936 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8940 "isa_tms320.tcc"
>
{
public:
	OpRETScond(CodeType code, typename CONFIG::address_t addr);
	uint32_t cond;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8952 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8955 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8959 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8962 "isa_tms320.tcc"
>
class OpRPTB : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8967 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 8971 "isa_tms320.tcc"
>
{
public:
	OpRPTB(CodeType code, typename CONFIG::address_t addr);
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 8983 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 8986 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 8990 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 8993 "isa_tms320.tcc"
>
class OpRPTS_reg : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 8998 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9002 "isa_tms320.tcc"
>
{
public:
	OpRPTS_reg(CodeType code, typename CONFIG::address_t addr);
	uint32_t src;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 9014 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9017 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9021 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9024 "isa_tms320.tcc"
>
class OpRPTS_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9029 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9033 "isa_tms320.tcc"
>
{
public:
	OpRPTS_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 9045 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9048 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9052 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9055 "isa_tms320.tcc"
>
class OpRPTS_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9060 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9064 "isa_tms320.tcc"
>
{
public:
	OpRPTS_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 9078 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9081 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9085 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9088 "isa_tms320.tcc"
>
class OpRPTS_imm : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9093 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9097 "isa_tms320.tcc"
>
{
public:
	OpRPTS_imm(CodeType code, typename CONFIG::address_t addr);
	int32_t imm;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 9109 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9112 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9116 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9119 "isa_tms320.tcc"
>
class OpSWI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9124 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9128 "isa_tms320.tcc"
>
{
public:
	OpSWI(CodeType code, typename CONFIG::address_t addr);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 9139 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9142 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9146 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9149 "isa_tms320.tcc"
>
class OpTRAPcond : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9154 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9158 "isa_tms320.tcc"
>
{
public:
	OpTRAPcond(CodeType code, typename CONFIG::address_t addr);
	uint32_t cond;
	uint32_t n;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 9171 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9174 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9178 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9181 "isa_tms320.tcc"
>
class OpIDLE2 : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9186 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9190 "isa_tms320.tcc"
>
{
public:
	OpIDLE2(CodeType code, typename CONFIG::address_t addr);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 9201 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9204 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9208 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9211 "isa_tms320.tcc"
>
class OpLOPOWER : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9216 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9220 "isa_tms320.tcc"
>
{
public:
	OpLOPOWER(CodeType code, typename CONFIG::address_t addr);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 9231 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9234 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9238 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9241 "isa_tms320.tcc"
>
class OpMAXSPEED : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9246 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9250 "isa_tms320.tcc"
>
{
public:
	OpMAXSPEED(CodeType code, typename CONFIG::address_t addr);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 9261 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9264 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9268 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9271 "isa_tms320.tcc"
>
class OpLDFI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9276 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9280 "isa_tms320.tcc"
>
{
public:
	OpLDFI_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 9293 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9296 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9300 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9303 "isa_tms320.tcc"
>
class OpLDFI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9308 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9312 "isa_tms320.tcc"
>
{
public:
	OpLDFI_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 9327 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9330 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9334 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9337 "isa_tms320.tcc"
>
class OpLDII_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9342 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9346 "isa_tms320.tcc"
>
{
public:
	OpLDII_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 9359 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9362 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9366 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9369 "isa_tms320.tcc"
>
class OpLDII_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9374 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9378 "isa_tms320.tcc"
>
{
public:
	OpLDII_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 9393 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9396 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9400 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9403 "isa_tms320.tcc"
>
class OpSIGI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9408 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9412 "isa_tms320.tcc"
>
{
public:
	OpSIGI(CodeType code, typename CONFIG::address_t addr);
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 9423 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9426 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9430 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9433 "isa_tms320.tcc"
>
class OpSTFI_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9438 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9442 "isa_tms320.tcc"
>
{
public:
	OpSTFI_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t src;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 9455 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9458 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9462 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9465 "isa_tms320.tcc"
>
class OpSTFI_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9470 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9474 "isa_tms320.tcc"
>
{
public:
	OpSTFI_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t src;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 9489 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9492 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9496 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9499 "isa_tms320.tcc"
>
class OpSTII_dir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9504 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9508 "isa_tms320.tcc"
>
{
public:
	OpSTII_dir(CodeType code, typename CONFIG::address_t addr);
	uint32_t src;
	uint32_t direct;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 9521 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9524 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9528 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9531 "isa_tms320.tcc"
>
class OpSTII_indir : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9536 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9540 "isa_tms320.tcc"
>
{
public:
	OpSTII_indir(CodeType code, typename CONFIG::address_t addr);
	uint32_t src;
	uint32_t mod;
	uint32_t ar;
	uint32_t disp;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 9555 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9558 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9562 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9565 "isa_tms320.tcc"
>
class OpFLOAT_STF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9570 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9574 "isa_tms320.tcc"
>
{
public:
	OpFLOAT_STF(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst1;
	uint32_t src3;
	uint32_t d2_mod;
	uint32_t d2_ar;
	uint32_t s2_mod;
	uint32_t s2_ar;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 9591 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9594 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9598 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9601 "isa_tms320.tcc"
>
class OpFLOAT_STF_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9606 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9610 "isa_tms320.tcc"
>
{
public:
	OpFLOAT_STF_ext(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst1;
	uint32_t src3;
	uint32_t d2_mod;
	uint32_t d2_ar;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 9626 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9629 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9633 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9636 "isa_tms320.tcc"
>
class OpLDF_STF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9641 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9645 "isa_tms320.tcc"
>
{
public:
	OpLDF_STF(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst1;
	uint32_t src3;
	uint32_t d2_mod;
	uint32_t d2_ar;
	uint32_t s2_mod;
	uint32_t s2_ar;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 9662 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9665 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9669 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9672 "isa_tms320.tcc"
>
class OpLDF_STF_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9677 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9681 "isa_tms320.tcc"
>
{
public:
	OpLDF_STF_ext(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst1;
	uint32_t src3;
	uint32_t d2_mod;
	uint32_t d2_ar;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 9697 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9700 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9704 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9707 "isa_tms320.tcc"
>
class OpLDI_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9712 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9716 "isa_tms320.tcc"
>
{
public:
	OpLDI_STI(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst1;
	uint32_t src3;
	uint32_t d2_mod;
	uint32_t d2_ar;
	uint32_t s2_mod;
	uint32_t s2_ar;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 9733 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9736 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9740 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9743 "isa_tms320.tcc"
>
class OpLDI_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9748 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9752 "isa_tms320.tcc"
>
{
public:
	OpLDI_STI_ext(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst1;
	uint32_t src3;
	uint32_t d2_mod;
	uint32_t d2_ar;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 9768 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9771 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9775 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9778 "isa_tms320.tcc"
>
class OpLSH3_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9783 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9787 "isa_tms320.tcc"
>
{
public:
	OpLSH3_STI(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst1;
	uint32_t count;
	uint32_t src3;
	uint32_t d2_mod;
	uint32_t d2_ar;
	uint32_t s2_mod;
	uint32_t s2_ar;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 9805 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9808 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9812 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9815 "isa_tms320.tcc"
>
class OpLSH3_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9820 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9824 "isa_tms320.tcc"
>
{
public:
	OpLSH3_STI_ext(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst1;
	uint32_t count;
	uint32_t src3;
	uint32_t d2_mod;
	uint32_t d2_ar;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 9841 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9844 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9848 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9851 "isa_tms320.tcc"
>
class OpMPYF3_STF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9856 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9860 "isa_tms320.tcc"
>
{
public:
	OpMPYF3_STF(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst1;
	uint32_t src1;
	uint32_t src3;
	uint32_t d2_mod;
	uint32_t d2_ar;
	uint32_t s2_mod;
	uint32_t s2_ar;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 9878 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9881 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9885 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9888 "isa_tms320.tcc"
>
class OpMPYF3_STF_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9893 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9897 "isa_tms320.tcc"
>
{
public:
	OpMPYF3_STF_ext(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst1;
	uint32_t src1;
	uint32_t src3;
	uint32_t d2_mod;
	uint32_t d2_ar;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 9914 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9917 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9921 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9924 "isa_tms320.tcc"
>
class OpMPYI3_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9929 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9933 "isa_tms320.tcc"
>
{
public:
	OpMPYI3_STI(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst1;
	uint32_t src1;
	uint32_t src3;
	uint32_t d2_mod;
	uint32_t d2_ar;
	uint32_t s2_mod;
	uint32_t s2_ar;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 9951 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9954 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9958 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9961 "isa_tms320.tcc"
>
class OpMPYI3_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 9966 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 9970 "isa_tms320.tcc"
>
{
public:
	OpMPYI3_STI_ext(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst1;
	uint32_t src1;
	uint32_t src3;
	uint32_t d2_mod;
	uint32_t d2_ar;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 9987 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 9990 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 9994 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 9997 "isa_tms320.tcc"
>
class OpNEGF_STF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10002 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10006 "isa_tms320.tcc"
>
{
public:
	OpNEGF_STF(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst1;
	uint32_t src3;
	uint32_t d2_mod;
	uint32_t d2_ar;
	uint32_t s2_mod;
	uint32_t s2_ar;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 10023 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10026 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10030 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10033 "isa_tms320.tcc"
>
class OpNEGF_STF_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10038 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10042 "isa_tms320.tcc"
>
{
public:
	OpNEGF_STF_ext(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst1;
	uint32_t src3;
	uint32_t d2_mod;
	uint32_t d2_ar;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 10058 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10061 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10065 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10068 "isa_tms320.tcc"
>
class OpNEGI_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10073 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10077 "isa_tms320.tcc"
>
{
public:
	OpNEGI_STI(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst1;
	uint32_t src3;
	uint32_t d2_mod;
	uint32_t d2_ar;
	uint32_t s2_mod;
	uint32_t s2_ar;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 10094 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10097 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10101 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10104 "isa_tms320.tcc"
>
class OpNEGI_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10109 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10113 "isa_tms320.tcc"
>
{
public:
	OpNEGI_STI_ext(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst1;
	uint32_t src3;
	uint32_t d2_mod;
	uint32_t d2_ar;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 10129 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10132 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10136 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10139 "isa_tms320.tcc"
>
class OpNOT_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10144 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10148 "isa_tms320.tcc"
>
{
public:
	OpNOT_STI(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst1;
	uint32_t src3;
	uint32_t d2_mod;
	uint32_t d2_ar;
	uint32_t s2_mod;
	uint32_t s2_ar;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 10165 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10168 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10172 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10175 "isa_tms320.tcc"
>
class OpNOT_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10180 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10184 "isa_tms320.tcc"
>
{
public:
	OpNOT_STI_ext(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst1;
	uint32_t src3;
	uint32_t d2_mod;
	uint32_t d2_ar;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 10200 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10203 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10207 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10210 "isa_tms320.tcc"
>
class OpOR3_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10215 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10219 "isa_tms320.tcc"
>
{
public:
	OpOR3_STI(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst1;
	uint32_t src1;
	uint32_t src3;
	uint32_t d2_mod;
	uint32_t d2_ar;
	uint32_t s2_mod;
	uint32_t s2_ar;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 10237 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10240 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10244 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10247 "isa_tms320.tcc"
>
class OpOR3_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10252 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10256 "isa_tms320.tcc"
>
{
public:
	OpOR3_STI_ext(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst1;
	uint32_t src1;
	uint32_t src3;
	uint32_t d2_mod;
	uint32_t d2_ar;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 10273 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10276 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10280 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10283 "isa_tms320.tcc"
>
class OpSTF_STF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10288 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10292 "isa_tms320.tcc"
>
{
public:
	OpSTF_STF(CodeType code, typename CONFIG::address_t addr);
	uint32_t src2;
	uint32_t src1;
	uint32_t d1_mod;
	uint32_t d1_ar;
	uint32_t d2_mod;
	uint32_t d2_ar;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 10309 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10312 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10316 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10319 "isa_tms320.tcc"
>
class OpSTF_STF_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10324 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10328 "isa_tms320.tcc"
>
{
public:
	OpSTF_STF_ext(CodeType code, typename CONFIG::address_t addr);
	uint32_t src2;
	uint32_t src1;
	uint32_t d1_mod;
	uint32_t d1_ar;
	uint32_t dst2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 10344 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10347 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10351 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10354 "isa_tms320.tcc"
>
class OpSTI_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10359 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10363 "isa_tms320.tcc"
>
{
public:
	OpSTI_STI(CodeType code, typename CONFIG::address_t addr);
	uint32_t src2;
	uint32_t src1;
	uint32_t d1_mod;
	uint32_t d1_ar;
	uint32_t d2_mod;
	uint32_t d2_ar;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 10380 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10383 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10387 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10390 "isa_tms320.tcc"
>
class OpSTI_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10395 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10399 "isa_tms320.tcc"
>
{
public:
	OpSTI_STI_ext(CodeType code, typename CONFIG::address_t addr);
	uint32_t src2;
	uint32_t src1;
	uint32_t d1_mod;
	uint32_t d1_ar;
	uint32_t dst2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 10415 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10418 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10422 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10425 "isa_tms320.tcc"
>
class OpSUBF3_STF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10430 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10434 "isa_tms320.tcc"
>
{
public:
	OpSUBF3_STF(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst1;
	uint32_t src1;
	uint32_t src3;
	uint32_t d2_mod;
	uint32_t d2_ar;
	uint32_t s2_mod;
	uint32_t s2_ar;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 10452 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10455 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10459 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10462 "isa_tms320.tcc"
>
class OpSUBF3_STF_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10467 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10471 "isa_tms320.tcc"
>
{
public:
	OpSUBF3_STF_ext(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst1;
	uint32_t src1;
	uint32_t src3;
	uint32_t d2_mod;
	uint32_t d2_ar;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 10488 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10491 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10495 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10498 "isa_tms320.tcc"
>
class OpSUBI3_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10503 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10507 "isa_tms320.tcc"
>
{
public:
	OpSUBI3_STI(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst1;
	uint32_t src1;
	uint32_t src3;
	uint32_t d2_mod;
	uint32_t d2_ar;
	uint32_t s2_mod;
	uint32_t s2_ar;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 10525 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10528 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10532 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10535 "isa_tms320.tcc"
>
class OpSUBI3_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10540 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10544 "isa_tms320.tcc"
>
{
public:
	OpSUBI3_STI_ext(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst1;
	uint32_t src1;
	uint32_t src3;
	uint32_t d2_mod;
	uint32_t d2_ar;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 10561 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10564 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10568 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10571 "isa_tms320.tcc"
>
class OpXOR3_STI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10576 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10580 "isa_tms320.tcc"
>
{
public:
	OpXOR3_STI(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst1;
	uint32_t src1;
	uint32_t src3;
	uint32_t d2_mod;
	uint32_t d2_ar;
	uint32_t s2_mod;
	uint32_t s2_ar;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 10598 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10601 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10605 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10608 "isa_tms320.tcc"
>
class OpXOR3_STI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10613 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10617 "isa_tms320.tcc"
>
{
public:
	OpXOR3_STI_ext(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst1;
	uint32_t src1;
	uint32_t src3;
	uint32_t d2_mod;
	uint32_t d2_ar;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 10634 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10637 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10641 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10644 "isa_tms320.tcc"
>
class OpLDF_LDF : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10649 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10653 "isa_tms320.tcc"
>
{
public:
	OpLDF_LDF(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst1;
	uint32_t dst2;
	uint32_t s1_mod;
	uint32_t s1_ar;
	uint32_t s2_mod;
	uint32_t s2_ar;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 10670 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10673 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10677 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10680 "isa_tms320.tcc"
>
class OpLDF_LDF_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10685 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10689 "isa_tms320.tcc"
>
{
public:
	OpLDF_LDF_ext(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst1;
	uint32_t dst2;
	uint32_t s1_mod;
	uint32_t s1_ar;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 10705 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10708 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10712 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10715 "isa_tms320.tcc"
>
class OpLDI_LDI : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10720 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10724 "isa_tms320.tcc"
>
{
public:
	OpLDI_LDI(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst1;
	uint32_t dst2;
	uint32_t s1_mod;
	uint32_t s1_ar;
	uint32_t s2_mod;
	uint32_t s2_ar;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 10741 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10744 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10748 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10751 "isa_tms320.tcc"
>
class OpLDI_LDI_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10756 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10760 "isa_tms320.tcc"
>
{
public:
	OpLDI_LDI_ext(CodeType code, typename CONFIG::address_t addr);
	uint32_t dst1;
	uint32_t dst2;
	uint32_t s1_mod;
	uint32_t s1_ar;
	uint32_t src2;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 10776 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10779 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10783 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10786 "isa_tms320.tcc"
>
class OpMPYF3_ADDF3 : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10791 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10795 "isa_tms320.tcc"
>
{
public:
	OpMPYF3_ADDF3(CodeType code, typename CONFIG::address_t addr);
	uint32_t p;
	uint32_t d1;
	uint32_t d2;
	uint32_t src1;
	uint32_t src2;
	uint32_t s3_mod;
	uint32_t s3_ar;
	uint32_t s4_mod;
	uint32_t s4_ar;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 10815 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10818 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10822 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10825 "isa_tms320.tcc"
>
class OpMPYF3_ADDF3_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10830 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10834 "isa_tms320.tcc"
>
{
public:
	OpMPYF3_ADDF3_ext(CodeType code, typename CONFIG::address_t addr);
	uint32_t p;
	uint32_t d1;
	uint32_t d2;
	uint32_t src1;
	uint32_t src2;
	uint32_t src3;
	uint32_t src4;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 10852 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10855 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10859 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10862 "isa_tms320.tcc"
>
class OpMPYF3_SUBF3 : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10867 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10871 "isa_tms320.tcc"
>
{
public:
	OpMPYF3_SUBF3(CodeType code, typename CONFIG::address_t addr);
	uint32_t p;
	uint32_t d1;
	uint32_t d2;
	uint32_t src1;
	uint32_t src2;
	uint32_t s3_mod;
	uint32_t s3_ar;
	uint32_t s4_mod;
	uint32_t s4_ar;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 10891 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10894 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10898 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10901 "isa_tms320.tcc"
>
class OpMPYF3_SUBF3_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10906 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10910 "isa_tms320.tcc"
>
{
public:
	OpMPYF3_SUBF3_ext(CodeType code, typename CONFIG::address_t addr);
	uint32_t p;
	uint32_t d1;
	uint32_t d2;
	uint32_t src1;
	uint32_t src2;
	uint32_t src3;
	uint32_t src4;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 10928 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10931 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10935 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10938 "isa_tms320.tcc"
>
class OpMPYI3_ADDI3 : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10943 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10947 "isa_tms320.tcc"
>
{
public:
	OpMPYI3_ADDI3(CodeType code, typename CONFIG::address_t addr);
	uint32_t p;
	uint32_t d1;
	uint32_t d2;
	uint32_t src1;
	uint32_t src2;
	uint32_t s3_mod;
	uint32_t s3_ar;
	uint32_t s4_mod;
	uint32_t s4_ar;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 10967 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 10970 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 10974 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 10977 "isa_tms320.tcc"
>
class OpMPYI3_ADDI3_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 10982 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 10986 "isa_tms320.tcc"
>
{
public:
	OpMPYI3_ADDI3_ext(CodeType code, typename CONFIG::address_t addr);
	uint32_t p;
	uint32_t d1;
	uint32_t d2;
	uint32_t src1;
	uint32_t src2;
	uint32_t src3;
	uint32_t src4;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 11004 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11007 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11011 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11014 "isa_tms320.tcc"
>
class OpMPYI3_SUBI3 : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11019 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11023 "isa_tms320.tcc"
>
{
public:
	OpMPYI3_SUBI3(CodeType code, typename CONFIG::address_t addr);
	uint32_t p;
	uint32_t d1;
	uint32_t d2;
	uint32_t src1;
	uint32_t src2;
	uint32_t s3_mod;
	uint32_t s3_ar;
	uint32_t s4_mod;
	uint32_t s4_ar;
private:
};

template <
#line 45 "isa/tms320.isa"
class
#line 11043 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11046 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11050 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11053 "isa_tms320.tcc"
>
class OpMPYI3_SUBI3_ext : public Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11058 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11062 "isa_tms320.tcc"
>
{
public:
	OpMPYI3_SUBI3_ext(CodeType code, typename CONFIG::address_t addr);
	uint32_t p;
	uint32_t d1;
	uint32_t d2;
	uint32_t src1;
	uint32_t src2;
	uint32_t src3;
	uint32_t src4;
private:
};

/*
*  Copyright (c) 2009,
*  Commissariat a l'Energie Atomique (CEA)
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without modification,
*  are permitted provided that the following conditions are met:
*
*   - Redistributions of source code must retain the above copyright notice, this
*     list of conditions and the following disclaimer.
*
*   - Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*     and/or other materials provided with the distribution.
*
*   - Neither the name of CEA nor the names of its contributors may be used to
*     endorse or promote products derived from this software without specific prior
*     written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
*  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED.
*  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
*  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
*  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
*  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
*  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
*          Gilles Mouchard (gilles.mouchard@cea.fr)
*/
/* List of load/store operations:
* - LDE          load floating-point exponent
* - LDF          load floating-point value
* - LDFcond      load floating-point value conditionally
* - LDI          load integer
* - LDIcond      load integer conditionally
* - LDM          load floating-point mantissa
* - LDP          load data page pointer
* - POP          pop integer from stack
* - POPF         pop floating-point value from stack
* - PUSH         push integer on stack
* - PUSHF        push floating-point value on stack
* - STF          store floating-point value
* - STI          store integer
*/
/************************************************
* LDE (load floating-point exponent)
* op LDE(0b000[3]:0b001101[6]:g[2]:dst[5]:src[16])
*/
template <
#line 45 "isa/tms320.isa"
class
#line 11133 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11136 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11140 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11143 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 11147 "isa_tms320.tcc"
OpLDE_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 11151 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11155 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 11159 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 11162 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 11166 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 11169 "isa_tms320.tcc"
)
{
#line 59 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		const char *dst_reg_name = REG_NAME[dst];
		const char *src_reg_name = REG_NAME[src];
		if(!dst_reg_name || !src_reg_name) return false;

		os << "LDE " << src_reg_name << ", " << dst_reg_name;
		return true;
	}
#line 11181 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 11187 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11190 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11194 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11197 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11202 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11206 "isa_tms320.tcc"
> *DecodeOpLDE_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDE_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11212 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11216 "isa_tms320.tcc"
	>(code, addr);
}

template <
#line 45 "isa/tms320.isa"
class
#line 11223 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11226 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11230 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11233 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 11237 "isa_tms320.tcc"
OpLDE_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 11241 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11245 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 11249 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 11252 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 11256 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 11259 "isa_tms320.tcc"
)
{
#line 70 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		const char *dst_reg_name = REG_NAME[dst];
		if(!dst_reg_name) return false;

		os << "LDE @0x" << hex << direct << dec << ", " << dst_reg_name;
		return true;
	}
#line 11270 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 11276 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11279 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11283 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11286 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11291 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11295 "isa_tms320.tcc"
> *DecodeOpLDE_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDE_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11301 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11305 "isa_tms320.tcc"
	>(code, addr);
}

template <
#line 45 "isa/tms320.isa"
class
#line 11312 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11315 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11319 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11322 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 11326 "isa_tms320.tcc"
OpLDE_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 11330 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11334 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 11338 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 11341 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 11345 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 11348 "isa_tms320.tcc"
)
{
#line 80 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		string disasm_src_indir;
		const char *dst_reg_name = REG_NAME[dst];
		if(!dst_reg_name || !cpu.DisasmIndir(disasm_src_indir, mod, ar, disp)) return false;

		os << "LDE " << disasm_src_indir << ", " << dst_reg_name;
		return true;
	}
#line 11360 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 11366 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11369 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11373 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11376 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11381 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11385 "isa_tms320.tcc"
> *DecodeOpLDE_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDE_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11391 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11395 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11403 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11406 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11410 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11413 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11418 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11422 "isa_tms320.tcc"
> *DecodeOpLDE_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDE_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11428 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11432 "isa_tms320.tcc"
	>(code, addr);
}

/* end of LDE (load floating-point exponent)
*************************************************/
/************************************************
* LDF (load floating-point value)
* op LDF(0b000[3]:0b001110[6]:g[2]:dst[5]:ext<32> src[16])
*/
template <
#line 45 "isa/tms320.isa"
class
#line 11445 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11448 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11452 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11455 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 11459 "isa_tms320.tcc"
OpLDF_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 11463 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11467 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 11471 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 11474 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 11478 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 11481 "isa_tms320.tcc"
)
{
#line 101 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		const char *dst_reg_name = REG_NAME[dst];
		const char *src_reg_name = REG_NAME[src];
		if(!dst_reg_name || !src_reg_name) return false;

		os << "LDF " << src_reg_name << ", " << dst_reg_name;
		return true;
	}
#line 11493 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 11499 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11502 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11506 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11509 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11514 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11518 "isa_tms320.tcc"
> *DecodeOpLDF_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11524 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11528 "isa_tms320.tcc"
	>(code, addr);
}

template <
#line 45 "isa/tms320.isa"
class
#line 11535 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11538 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11542 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11545 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 11549 "isa_tms320.tcc"
OpLDF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 11553 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11557 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 11561 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 11564 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 11568 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 11571 "isa_tms320.tcc"
)
{
#line 112 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		const char *dst_reg_name = REG_NAME[dst];
		if(!dst_reg_name) return false;

		os << "LDF @0x" << hex << direct << dec << ", " << dst_reg_name;
		return true;
	}
#line 11582 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 11588 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11591 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11595 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11598 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11603 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11607 "isa_tms320.tcc"
> *DecodeOpLDF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11613 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11617 "isa_tms320.tcc"
	>(code, addr);
}

template <
#line 45 "isa/tms320.isa"
class
#line 11624 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11627 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11631 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11634 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 11638 "isa_tms320.tcc"
OpLDF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 11642 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11646 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 11650 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 11653 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 11657 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 11660 "isa_tms320.tcc"
)
{
#line 122 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		string disasm_src_indir;
		const char *dst_reg_name = REG_NAME[dst];
		if(!dst_reg_name || !cpu.DisasmIndir(disasm_src_indir, mod, ar, disp)) return false;

		os << "LDF " << disasm_src_indir << ", " << dst_reg_name;
		return true;
	}
#line 11672 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 11678 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11681 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11685 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11688 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11693 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11697 "isa_tms320.tcc"
> *DecodeOpLDF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11703 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11707 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 11715 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11718 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11722 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11725 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11730 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11734 "isa_tms320.tcc"
> *DecodeOpLDF_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11740 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11744 "isa_tms320.tcc"
	>(code, addr);
}

/* end of LDF (load floating-point value)
*************************************************/
/************************************************
* LDFcond (load floating-point value conditionally)
* op LDFcond(0b0100[4]:cond[4]:g[2]:dst[5]:ext<32> src[16])
*/
template <
#line 45 "isa/tms320.isa"
class
#line 11757 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11760 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11764 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11767 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 11771 "isa_tms320.tcc"
OpLDFcond_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 11775 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11779 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 11783 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 11786 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 11790 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 11793 "isa_tms320.tcc"
)
{
#line 143 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		const char *cond_name = COND_NAME[cond];
		const char *dst_reg_name = REG_NAME[dst];
		const char *src_reg_name = REG_NAME[src];
		if(!cond_name || !dst_reg_name || !src_reg_name) return false;

		os << "LDF" << cond_name << " " << src_reg_name << ", " << dst_reg_name;
		return true;
	}
#line 11806 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 11812 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11815 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11819 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11822 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11827 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11831 "isa_tms320.tcc"
> *DecodeOpLDFcond_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDFcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11837 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11841 "isa_tms320.tcc"
	>(code, addr);
}

template <
#line 45 "isa/tms320.isa"
class
#line 11848 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11851 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11855 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11858 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 11862 "isa_tms320.tcc"
OpLDFcond_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 11866 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11870 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 11874 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 11877 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 11881 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 11884 "isa_tms320.tcc"
)
{
#line 155 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		const char *cond_name = COND_NAME[cond];
		const char *dst_reg_name = REG_NAME[dst];
		if(!cond_name || !dst_reg_name) return false;

		os << "LDF" << cond_name << " @0x" << hex << direct << dec << ", " << dst_reg_name;
		return true;
	}
#line 11896 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 11902 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11905 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11909 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11912 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 11917 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11921 "isa_tms320.tcc"
> *DecodeOpLDFcond_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDFcond_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 11927 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 11931 "isa_tms320.tcc"
	>(code, addr);
}

template <
#line 45 "isa/tms320.isa"
class
#line 11938 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11941 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 11945 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 11948 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 11952 "isa_tms320.tcc"
OpLDFcond_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 11956 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 11960 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 11964 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 11967 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 11971 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 11974 "isa_tms320.tcc"
)
{
#line 166 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		string disasm_src_indir;
		const char *cond_name = COND_NAME[cond];
		const char *dst_reg_name = REG_NAME[dst];
		if(!cond_name || !dst_reg_name || !cpu.DisasmIndir(disasm_src_indir, mod, ar, disp)) return false;

		os << "LDF" << cond_name << " " << disasm_src_indir << ", " << dst_reg_name;
		return true;
	}
#line 11987 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 11993 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 11996 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12000 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12003 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12008 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12012 "isa_tms320.tcc"
> *DecodeOpLDFcond_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDFcond_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12018 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12022 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 12030 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12033 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12037 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12040 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12045 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12049 "isa_tms320.tcc"
> *DecodeOpLDFcond_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDFcond_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12055 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12059 "isa_tms320.tcc"
	>(code, addr);
}

/* end of LDFcond (load floating-point value conditionally)
*************************************************/
/*************************************************
* LDI (load integer)
* op LDI(0b000[3]:0b010000[6]:g[2]:dst[5]:ext<32> src[16])
*/
template <
#line 45 "isa/tms320.isa"
class
#line 12072 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12075 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12079 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12082 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 12086 "isa_tms320.tcc"
OpLDI_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 12090 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12094 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 12098 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 12101 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 12105 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 12108 "isa_tms320.tcc"
)
{
#line 188 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		const char *dst_reg_name = REG_NAME[dst];
		const char *src_reg_name = REG_NAME[src];
		if(!dst_reg_name || !src_reg_name) return false;

		os << "LDI " << src_reg_name << ", " << dst_reg_name;
		return true;
	}
#line 12120 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 12126 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12129 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12133 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12136 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12141 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12145 "isa_tms320.tcc"
> *DecodeOpLDI_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12151 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12155 "isa_tms320.tcc"
	>(code, addr);
}

template <
#line 45 "isa/tms320.isa"
class
#line 12162 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12165 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12169 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12172 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 12176 "isa_tms320.tcc"
OpLDI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 12180 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12184 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 12188 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 12191 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 12195 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 12198 "isa_tms320.tcc"
)
{
#line 199 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		const char *dst_reg_name = REG_NAME[dst];
		if(!dst_reg_name) return false;

		os << "LDI @0x" << hex << direct << dec << ", " << dst_reg_name;
		return true;
	}
#line 12209 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 12215 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12218 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12222 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12225 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12230 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12234 "isa_tms320.tcc"
> *DecodeOpLDI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12240 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12244 "isa_tms320.tcc"
	>(code, addr);
}

template <
#line 45 "isa/tms320.isa"
class
#line 12251 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12254 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12258 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12261 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 12265 "isa_tms320.tcc"
OpLDI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 12269 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12273 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 12277 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 12280 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 12284 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 12287 "isa_tms320.tcc"
)
{
#line 209 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		string disasm_src_indir;
		const char *dst_reg_name = REG_NAME[dst];
		if(!dst_reg_name || !cpu.DisasmIndir(disasm_src_indir, mod, ar, disp)) return false;

		os << "LDI " << disasm_src_indir << ", " << dst_reg_name;
		return true;
	}
#line 12299 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 12305 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12308 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12312 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12315 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12320 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12324 "isa_tms320.tcc"
> *DecodeOpLDI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12330 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12334 "isa_tms320.tcc"
	>(code, addr);
}

template <
#line 45 "isa/tms320.isa"
class
#line 12341 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12344 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12348 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12351 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 12355 "isa_tms320.tcc"
OpLDI_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 12359 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12363 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 12367 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 12370 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 12374 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 12377 "isa_tms320.tcc"
)
{
#line 220 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		const char *dst_reg_name = REG_NAME[dst];
		if(!dst_reg_name) return false;

		os << "LDI " << imm << ", " << dst_reg_name;
		return true;
	}
#line 12388 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 12394 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12397 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12401 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12404 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12409 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12413 "isa_tms320.tcc"
> *DecodeOpLDI_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12419 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12423 "isa_tms320.tcc"
	>(code, addr);
}

/* end of LDI (load integer)
*************************************************/
/*************************************************
* LDIcond (load integer conditionally)
* op LDIcond(0b0101[4]:cond[4]:g[2]:dst[5]:src[16])
*/
template <
#line 45 "isa/tms320.isa"
class
#line 12436 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12439 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12443 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12446 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 12450 "isa_tms320.tcc"
OpLDIcond_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 12454 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12458 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 12462 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 12465 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 12469 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 12472 "isa_tms320.tcc"
)
{
#line 238 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		const char *cond_name = COND_NAME[cond];
		const char *src_reg_name = REG_NAME[src];
		const char *dst_reg_name = REG_NAME[dst];
		if(!cond_name || !src_reg_name || !dst_reg_name) return false;

		os << "LDI" << cond_name << " " << src_reg_name << ", " << dst_reg_name;
		return true;
	}
#line 12485 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 12491 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12494 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12498 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12501 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12506 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12510 "isa_tms320.tcc"
> *DecodeOpLDIcond_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDIcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12516 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12520 "isa_tms320.tcc"
	>(code, addr);
}

template <
#line 45 "isa/tms320.isa"
class
#line 12527 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12530 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12534 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12537 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 12541 "isa_tms320.tcc"
OpLDIcond_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 12545 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12549 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 12553 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 12556 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 12560 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 12563 "isa_tms320.tcc"
)
{
#line 250 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		const char *cond_name = COND_NAME[cond];
		const char *dst_reg_name = REG_NAME[dst];
		if(!cond_name || !dst_reg_name) return false;

		os << "LDI" << cond_name << " @0x" << hex << direct << dec << ", " << dst_reg_name;
		return true;
	}
#line 12575 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 12581 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12584 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12588 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12591 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12596 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12600 "isa_tms320.tcc"
> *DecodeOpLDIcond_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDIcond_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12606 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12610 "isa_tms320.tcc"
	>(code, addr);
}

template <
#line 45 "isa/tms320.isa"
class
#line 12617 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12620 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12624 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12627 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 12631 "isa_tms320.tcc"
OpLDIcond_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 12635 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12639 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 12643 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 12646 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 12650 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 12653 "isa_tms320.tcc"
)
{
#line 261 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		string disasm_src_indir;
		const char *cond_name = COND_NAME[cond];
		const char *dst_reg_name = REG_NAME[dst];

		if(!cond_name || !dst_reg_name || !cpu.DisasmIndir(disasm_src_indir, mod, ar, disp)) return false;

		os << "LDI" << cond_name << " " << disasm_src_indir << ", " << dst_reg_name;
		return true;
	}
#line 12667 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 12673 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12676 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12680 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12683 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12688 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12692 "isa_tms320.tcc"
> *DecodeOpLDIcond_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDIcond_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12698 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12702 "isa_tms320.tcc"
	>(code, addr);
}

template <
#line 45 "isa/tms320.isa"
class
#line 12709 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12712 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12716 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12719 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 12723 "isa_tms320.tcc"
OpLDIcond_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 12727 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12731 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 12735 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 12738 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 12742 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 12745 "isa_tms320.tcc"
)
{
#line 274 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		const char *cond_name = COND_NAME[cond];
		const char *dst_reg_name = REG_NAME[dst];

		if(!cond_name || !dst_reg_name) return false;

		os << "LDI" << cond_name << " " << imm << ", " << dst_reg_name;
		return true;
	}
#line 12758 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 12764 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12767 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12771 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12774 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12779 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12783 "isa_tms320.tcc"
> *DecodeOpLDIcond_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDIcond_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12789 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12793 "isa_tms320.tcc"
	>(code, addr);
}

/* end of LDIcond (load integer conditionally)
*************************************************/
/*************************************************
* LDM (load floating-point mantissa)
* op LDM(0b000[3]:0b010010[6]:g[2]:dst[5]:src[16])
*/
template <
#line 45 "isa/tms320.isa"
class
#line 12806 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12809 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12813 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12816 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 12820 "isa_tms320.tcc"
OpLDM_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 12824 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12828 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 12832 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 12835 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 12839 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 12842 "isa_tms320.tcc"
)
{
#line 294 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		const char *dst_reg_name = REG_NAME[dst];
		const char *src_reg_name = REG_NAME[src];
		if(!dst_reg_name || !src_reg_name) return false;

		os << "LDM " << src_reg_name << ", " << dst_reg_name;
		return true;
	}
#line 12854 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 12860 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12863 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12867 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12870 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12875 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12879 "isa_tms320.tcc"
> *DecodeOpLDM_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDM_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12885 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12889 "isa_tms320.tcc"
	>(code, addr);
}

template <
#line 45 "isa/tms320.isa"
class
#line 12896 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12899 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12903 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12906 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 12910 "isa_tms320.tcc"
OpLDM_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 12914 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12918 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 12922 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 12925 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 12929 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 12932 "isa_tms320.tcc"
)
{
#line 305 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		const char *dst_reg_name = REG_NAME[dst];
		if(!dst_reg_name) return false;

		os << "LDM @0x" << hex << direct << dec << ", " << dst_reg_name;
		return true;
	}
#line 12943 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 12949 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12952 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12956 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12959 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 12964 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 12968 "isa_tms320.tcc"
> *DecodeOpLDM_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDM_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 12974 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 12978 "isa_tms320.tcc"
	>(code, addr);
}

template <
#line 45 "isa/tms320.isa"
class
#line 12985 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 12988 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 12992 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 12995 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 12999 "isa_tms320.tcc"
OpLDM_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 13003 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13007 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 13011 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 13014 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 13018 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 13021 "isa_tms320.tcc"
)
{
#line 315 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		string disasm_src_indir;
		const char *dst_reg_name = REG_NAME[dst];
		if(!dst_reg_name || !cpu.DisasmIndir(disasm_src_indir, mod, ar, disp)) return false;

		os << "LDM " << disasm_src_indir << ", " << dst_reg_name;
		return true;
	}
#line 13033 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 13039 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13042 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13046 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13049 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13054 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13058 "isa_tms320.tcc"
> *DecodeOpLDM_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDM_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13064 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13068 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 13076 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13079 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13083 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13086 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13091 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13095 "isa_tms320.tcc"
> *DecodeOpLDM_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDM_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13101 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13105 "isa_tms320.tcc"
	>(code, addr);
}

/* end of LDM (load floating-point mantissa)
*************************************************/
/*************************************************
* LDP (load data page pointer)
* op LDP(0b000[3]:0b010000[6]:0b11[2]:0b10000[5]:0b00000000[8]:src[8])
*/
template <
#line 45 "isa/tms320.isa"
class
#line 13118 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13121 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13125 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13128 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 13132 "isa_tms320.tcc"
OpLDP<
#line 45 "isa/tms320.isa"
CONFIG
#line 13136 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13140 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 13144 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 13147 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 13151 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 13154 "isa_tms320.tcc"
)
{
#line 336 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		os << "LDP @0x" << hex << (src << 16) << dec;
		return true;
	}
#line 13162 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 13168 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13171 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13175 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13178 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13183 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13187 "isa_tms320.tcc"
> *DecodeOpLDP(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDP<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13193 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13197 "isa_tms320.tcc"
	>(code, addr);
}

/* end of LDP (load data page pointer)
*************************************************/
/*************************************************
* POP (pop integer from stack)
* op POP(0b000[3]:0b011100[6]:0b01[2]:dst[5]:0[16])
*/
template <
#line 45 "isa/tms320.isa"
class
#line 13210 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13213 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13217 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13220 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 13224 "isa_tms320.tcc"
OpPOP<
#line 45 "isa/tms320.isa"
CONFIG
#line 13228 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13232 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 13236 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 13239 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 13243 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 13246 "isa_tms320.tcc"
)
{
#line 352 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		const char *dst_reg_name = REG_NAME[dst];

		if(!dst_reg_name) return false;

		os << "POP " << dst_reg_name;
		return true;
	}
#line 13258 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 13264 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13267 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13271 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13274 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13279 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13283 "isa_tms320.tcc"
> *DecodeOpPOP(CodeType code, typename CONFIG::address_t addr)
{
	return new OpPOP<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13289 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13293 "isa_tms320.tcc"
	>(code, addr);
}

/* end of POP (pop integer from stack)
*************************************************/
/*************************************************
* POPF (pop floating-point value from stack)
* op POPF(0b000[3]:0b011101[6]:0b01[2]:dst[5]:0[16])
*/
template <
#line 45 "isa/tms320.isa"
class
#line 13306 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13309 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13313 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13316 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 13320 "isa_tms320.tcc"
OpPOPF<
#line 45 "isa/tms320.isa"
CONFIG
#line 13324 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13328 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 13332 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 13335 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 13339 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 13342 "isa_tms320.tcc"
)
{
#line 371 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		const char *dst_reg_name = REG_NAME[dst];

		if(!dst_reg_name) return false;

		os << "POPF " << dst_reg_name;
		return true;
	}
#line 13354 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 13360 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13363 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13367 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13370 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13375 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13379 "isa_tms320.tcc"
> *DecodeOpPOPF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpPOPF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13385 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13389 "isa_tms320.tcc"
	>(code, addr);
}

/* end of POPF (pop floating-point value from stack)
*************************************************/
/*************************************************
* PUSH (push integer on stack)
* op PUSH(0b000[3]:0b011110[6]:0b01[2]:src[5]:0[16])
*/
template <
#line 45 "isa/tms320.isa"
class
#line 13402 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13405 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13409 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13412 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 13416 "isa_tms320.tcc"
OpPUSH<
#line 45 "isa/tms320.isa"
CONFIG
#line 13420 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13424 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 13428 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 13431 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 13435 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 13438 "isa_tms320.tcc"
)
{
#line 390 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		const char *src_reg_name = REG_NAME[src];

		if(!src_reg_name) return false;

		os << "PUSH " << src_reg_name;
		return true;
	}
#line 13450 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 13456 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13459 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13463 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13466 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13471 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13475 "isa_tms320.tcc"
> *DecodeOpPUSH(CodeType code, typename CONFIG::address_t addr)
{
	return new OpPUSH<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13481 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13485 "isa_tms320.tcc"
	>(code, addr);
}

/* end of POPF (pop floating-point value from stack)
*************************************************/
/*************************************************
* PUSHF (push floating-point value on stack)
* op PUSHF(0b000[3]:0b011111[6]:0b01[2]:src[5]:0[16])
*/
template <
#line 45 "isa/tms320.isa"
class
#line 13498 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13501 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13505 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13508 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 13512 "isa_tms320.tcc"
OpPUSHF<
#line 45 "isa/tms320.isa"
CONFIG
#line 13516 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13520 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 13524 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 13527 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 13531 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 13534 "isa_tms320.tcc"
)
{
#line 409 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		const char *src_reg_name = REG_NAME[src];

		if(!src_reg_name) return false;

		os << "PUSHF " << src_reg_name;
		return true;
	}
#line 13546 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 13552 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13555 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13559 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13562 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13567 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13571 "isa_tms320.tcc"
> *DecodeOpPUSHF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpPUSHF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13577 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13581 "isa_tms320.tcc"
	>(code, addr);
}

/* end of PUSHF (push floating-point value on stack)
*************************************************/
/*************************************************
* STF (store floating-point value)
* op STF(0b000[3]:0b101000[6]:g[2]:src[5]:dst[16])
*/
template <
#line 45 "isa/tms320.isa"
class
#line 13594 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13597 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13601 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13604 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 13608 "isa_tms320.tcc"
OpSTF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 13612 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13616 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 13620 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 13623 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 13627 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 13630 "isa_tms320.tcc"
)
{
#line 428 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		const char *src_reg_name = REG_NAME[src];
		if(!src_reg_name) return false;

		os << "STF " << src_reg_name << ", @0x" << direct;
		return true;
	}
#line 13641 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 13647 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13650 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13654 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13657 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13662 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13666 "isa_tms320.tcc"
> *DecodeOpSTF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13672 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13676 "isa_tms320.tcc"
	>(code, addr);
}

template <
#line 45 "isa/tms320.isa"
class
#line 13683 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13686 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13690 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13693 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 13697 "isa_tms320.tcc"
OpSTF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 13701 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13705 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 13709 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 13712 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 13716 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 13719 "isa_tms320.tcc"
)
{
#line 438 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		string disasm_dst_indir;
		const char *src_reg_name = REG_NAME[src];
		if(!src_reg_name || !cpu.DisasmIndir(disasm_dst_indir, mod, ar, disp)) return false;

		os << "STF " << src_reg_name << ", " << disasm_dst_indir;
		return true;
	}
#line 13731 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 13737 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13740 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13744 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13747 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13752 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13756 "isa_tms320.tcc"
> *DecodeOpSTF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13762 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13766 "isa_tms320.tcc"
	>(code, addr);
}

/* end of STF (store floating-point value)
*************************************************/
/*************************************************
* STI (store integer)
* op STI(0b000[3]:0b101010[6]:g[2]:src[5]:dst[16])
*/
template <
#line 45 "isa/tms320.isa"
class
#line 13779 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13782 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13786 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13789 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 13793 "isa_tms320.tcc"
OpSTI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 13797 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13801 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 13805 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 13808 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 13812 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 13815 "isa_tms320.tcc"
)
{
#line 458 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		const char *src_reg_name = REG_NAME[src];
		if(!src_reg_name) return false;

		os << "STI " << src_reg_name << ", @0x" << direct;
		return true;
	}
#line 13826 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 13832 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13835 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13839 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13842 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13847 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13851 "isa_tms320.tcc"
> *DecodeOpSTI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13857 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13861 "isa_tms320.tcc"
	>(code, addr);
}

template <
#line 45 "isa/tms320.isa"
class
#line 13868 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13871 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13875 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13878 "isa_tms320.tcc"
>
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
bool
#line 13882 "isa_tms320.tcc"
OpSTI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 13886 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13890 "isa_tms320.tcc"
>::disasm(
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
CPU<CONFIG, DEBUG> &
#line 13894 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
cpu
#line 13897 "isa_tms320.tcc"
,
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
std::ostream &
#line 13901 "isa_tms320.tcc"
#line 40 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/actions_dec.isa"
os
#line 13904 "isa_tms320.tcc"
)
{
#line 468 "/local/home/gmouchard/unisim/svn/devel/unisim_lib/unisim/component/cxx/processor/tms320/isa/load_store.isa"
	{
		string disasm_dst_indir;
		const char *src_reg_name = REG_NAME[src];
		if(!src_reg_name || !cpu.DisasmIndir(disasm_dst_indir, mod, ar, disp)) return false;

		os << "STI " << src_reg_name << ", " << disasm_dst_indir;
		return true;
	}
#line 13916 "isa_tms320.tcc"
}

template <
#line 45 "isa/tms320.isa"
class
#line 13922 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 13925 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 13929 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 13932 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 13937 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 13941 "isa_tms320.tcc"
> *DecodeOpSTI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 13947 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 13951 "isa_tms320.tcc"
	>(code, addr);
}

/* end of STI (store integer)
*************************************************/
/*
*  Copyright (c) 2009,
*  Commissariat a l'Energie Atomique (CEA)
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without modification,
*  are permitted provided that the following conditions are met:
*
*   - Redistributions of source code must retain the above copyright notice, this
*     list of conditions and the following disclaimer.
*
*   - Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*     and/or other materials provided with the distribution.
*
*   - Neither the name of CEA nor the names of its contributors may be used to
*     endorse or promote products derived from this software without specific prior
*     written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
*  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED.
*  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
*  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
*  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
*  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
*  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
*          Gilles Mouchard (gilles.mouchard@cea.fr)
*/
/* List of 2 operands operations:
* - ABSF         absolute value of a floating-point number
* - ABSI         absolute value of an integer
* - ADDC         add integers with carry
* - ADDF         add floating-point values
* - ADDI         add integers
* - AND          bitwise-logical AND
* - ANDN         bitwise-logical AND with complement
* - ASH          arithmetic shift
* - CMPF         compare floating-point values
* - CMPI         compare integers
* - FIX          convert floating-point value to integer
* - FLOAT        convert integer to floating-point value
* - LSH          logical shift
* - MPYF         multiply floating-point values
* - MPYI         multiply integers
* - NEGB         negate integer with borrow
* - NEGF         negate floating-point value
* - NEGI         negate integer
* - NORM         normalize floating-point value
* - NOT          bitwise-logical complement
* - OR           bitwise-logical OR
* - RND          round floating-point value
* - ROL          rotate left
* - ROLC         rotate left through carry
* - ROR          rotate right
* - RORC         rotate right through carry
* - SUBB         subtract integers with borrow
* - SUBC         subtract integers conditionally
* - SUBF         subtract floating-point values
* - SUBI         subtract integer
* - SUBRB        subtract reverse integer with borrow
* - SUBRF        subtract reverse floating-point value
* - SUBRI        subtract reverse integer
* - TSTB         test bit fields
* - XOR          bitwise-exclusive OR
*/
/**********************************************************
* ABSF (absolute value of a floating-point number)
* op ABSF(0b000[3]:0b000000[6]:g[2]:dst[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 14036 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14039 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14043 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14046 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14051 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14055 "isa_tms320.tcc"
> *DecodeOpABSF_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpABSF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14061 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14065 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14073 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14076 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14080 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14083 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14088 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14092 "isa_tms320.tcc"
> *DecodeOpABSF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpABSF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14098 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14102 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14110 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14113 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14117 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14120 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14125 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14129 "isa_tms320.tcc"
> *DecodeOpABSF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpABSF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14135 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14139 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14147 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14150 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14154 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14157 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14162 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14166 "isa_tms320.tcc"
> *DecodeOpABSF_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpABSF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14172 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14176 "isa_tms320.tcc"
	>(code, addr);
}

/* end of ABSF (absolute value of a floating-point number)
**********************************************************/
/**********************************************************
* ABSI (absolute value of an integer)
* op ABSI(0b000[3]:0b000001[6]:g[2]:dst[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 14190 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14193 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14197 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14200 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14205 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14209 "isa_tms320.tcc"
> *DecodeOpABSI_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpABSI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14215 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14219 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14227 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14230 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14234 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14237 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14242 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14246 "isa_tms320.tcc"
> *DecodeOpABSI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpABSI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14252 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14256 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14264 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14267 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14271 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14274 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14279 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14283 "isa_tms320.tcc"
> *DecodeOpABSI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpABSI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14289 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14293 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14301 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14304 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14308 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14311 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14316 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14320 "isa_tms320.tcc"
> *DecodeOpABSI_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpABSI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14326 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14330 "isa_tms320.tcc"
	>(code, addr);
}

/* end of ABSI (absolute value of an integer)
**********************************************************/
/**********************************************************
* ADDC (add integers with carry)
* op ADDC(0b000[3]:0b000010[6]:g[2]:dst[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 14344 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14347 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14351 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14354 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14359 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14363 "isa_tms320.tcc"
> *DecodeOpADDC_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDC_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14369 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14373 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14381 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14384 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14388 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14391 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14396 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14400 "isa_tms320.tcc"
> *DecodeOpADDC_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDC_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14406 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14410 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14418 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14421 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14425 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14428 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14433 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14437 "isa_tms320.tcc"
> *DecodeOpADDC_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDC_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14443 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14447 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14455 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14458 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14462 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14465 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14470 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14474 "isa_tms320.tcc"
> *DecodeOpADDC_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDC_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14480 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14484 "isa_tms320.tcc"
	>(code, addr);
}

/* end of ADDC (add integers with carry)
**********************************************************/
/**********************************************************
* ADDF (add floating-point values)
* op ADDF(0b000[3]:0b000011[6]:g[2]:dst[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 14498 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14501 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14505 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14508 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14513 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14517 "isa_tms320.tcc"
> *DecodeOpADDF_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14523 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14527 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14535 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14538 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14542 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14545 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14550 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14554 "isa_tms320.tcc"
> *DecodeOpADDF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14560 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14564 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14572 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14575 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14579 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14582 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14587 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14591 "isa_tms320.tcc"
> *DecodeOpADDF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14597 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14601 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14609 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14612 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14616 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14619 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14624 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14628 "isa_tms320.tcc"
> *DecodeOpADDF_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14634 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14638 "isa_tms320.tcc"
	>(code, addr);
}

/* end of ADDF (add floating-point values)
**********************************************************/
/**********************************************************
* ADDI (add integers)
* op ADDI(0b000[3]:0b000100[6]:g[2]:dst[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 14652 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14655 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14659 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14662 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14667 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14671 "isa_tms320.tcc"
> *DecodeOpADDI_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14677 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14681 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14689 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14692 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14696 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14699 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14704 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14708 "isa_tms320.tcc"
> *DecodeOpADDI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14714 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14718 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14726 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14729 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14733 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14736 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14741 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14745 "isa_tms320.tcc"
> *DecodeOpADDI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14751 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14755 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14763 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14766 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14770 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14773 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14778 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14782 "isa_tms320.tcc"
> *DecodeOpADDI_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14788 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14792 "isa_tms320.tcc"
	>(code, addr);
}

/* end of ADDI (add integers)
**********************************************************/
/**********************************************************
* AND (bitwise-logical AND)
* op AND(0b000[3]:0b000101[6]:g[2]:dst[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 14806 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14809 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14813 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14816 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14821 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14825 "isa_tms320.tcc"
> *DecodeOpAND_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAND_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14831 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14835 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14843 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14846 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14850 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14853 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14858 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14862 "isa_tms320.tcc"
> *DecodeOpAND_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAND_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14868 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14872 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14880 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14883 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14887 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14890 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14895 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14899 "isa_tms320.tcc"
> *DecodeOpAND_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAND_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14905 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14909 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14917 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14920 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14924 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14927 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14932 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14936 "isa_tms320.tcc"
> *DecodeOpAND_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAND_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14942 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14946 "isa_tms320.tcc"
	>(code, addr);
}

/* end of AND (bitwise-logical AND)
**********************************************************/
/**********************************************************
* ANDN (bitwise-logical AND with complement)
* op ANDN(0b000[3]:0b000110[6]:g[2]:dst[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 14960 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 14963 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 14967 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 14970 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 14975 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 14979 "isa_tms320.tcc"
> *DecodeOpANDN_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpANDN_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 14985 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 14989 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 14997 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15000 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15004 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15007 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15012 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15016 "isa_tms320.tcc"
> *DecodeOpANDN_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpANDN_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15022 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15026 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15034 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15037 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15041 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15044 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15049 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15053 "isa_tms320.tcc"
> *DecodeOpANDN_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpANDN_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15059 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15063 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15071 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15074 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15078 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15081 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15086 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15090 "isa_tms320.tcc"
> *DecodeOpANDN_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpANDN_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15096 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15100 "isa_tms320.tcc"
	>(code, addr);
}

/* end of ANDN (bitwise-logical AND with complement)
**********************************************************/
/**********************************************************
* ASH (arithmetic shift)
* op ASH(0b000[3]:0b000111[6]:g[2]:dst[5]:count[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 15114 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15117 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15121 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15124 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15129 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15133 "isa_tms320.tcc"
> *DecodeOpASH_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpASH_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15139 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15143 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15151 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15154 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15158 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15161 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15166 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15170 "isa_tms320.tcc"
> *DecodeOpASH_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpASH_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15176 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15180 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15188 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15191 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15195 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15198 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15203 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15207 "isa_tms320.tcc"
> *DecodeOpASH_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpASH_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15213 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15217 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15225 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15228 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15232 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15235 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15240 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15244 "isa_tms320.tcc"
> *DecodeOpASH_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpASH_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15250 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15254 "isa_tms320.tcc"
	>(code, addr);
}

/* end of ASH (arithmetic shift)
**********************************************************/
/**********************************************************
* CMPF (compare floating-point values)
* op CMPF(0b000[3]:0b001000[6]:g[2]:dst[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 15268 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15271 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15275 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15278 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15283 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15287 "isa_tms320.tcc"
> *DecodeOpCMPF_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15293 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15297 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15305 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15308 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15312 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15315 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15320 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15324 "isa_tms320.tcc"
> *DecodeOpCMPF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15330 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15334 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15342 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15345 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15349 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15352 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15357 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15361 "isa_tms320.tcc"
> *DecodeOpCMPF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15367 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15371 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15379 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15382 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15386 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15389 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15394 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15398 "isa_tms320.tcc"
> *DecodeOpCMPF_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15404 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15408 "isa_tms320.tcc"
	>(code, addr);
}

/* end of CMPF (compare floating-point values)
**********************************************************/
/**********************************************************
* CMPI (compare integers)
* op CMPI(0b000[3]:0b001001[6]:g[2]:dst[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 15422 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15425 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15429 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15432 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15437 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15441 "isa_tms320.tcc"
> *DecodeOpCMPI_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15447 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15451 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15459 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15462 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15466 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15469 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15474 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15478 "isa_tms320.tcc"
> *DecodeOpCMPI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15484 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15488 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15496 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15499 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15503 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15506 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15511 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15515 "isa_tms320.tcc"
> *DecodeOpCMPI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15521 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15525 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15533 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15536 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15540 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15543 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15548 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15552 "isa_tms320.tcc"
> *DecodeOpCMPI_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15558 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15562 "isa_tms320.tcc"
	>(code, addr);
}

/* end of CMPI (compare integers)
**********************************************************/
/**********************************************************
* FIX (convert floating-point value to integer)
* op FIX(0b000[3]:0b001010[6]:g[2]:dst[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 15576 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15579 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15583 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15586 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15591 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15595 "isa_tms320.tcc"
> *DecodeOpFIX_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFIX_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15601 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15605 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15613 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15616 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15620 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15623 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15628 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15632 "isa_tms320.tcc"
> *DecodeOpFIX_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFIX_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15638 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15642 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15650 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15653 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15657 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15660 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15665 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15669 "isa_tms320.tcc"
> *DecodeOpFIX_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFIX_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15675 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15679 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15687 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15690 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15694 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15697 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15702 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15706 "isa_tms320.tcc"
> *DecodeOpFIX_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFIX_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15712 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15716 "isa_tms320.tcc"
	>(code, addr);
}

/* end of FIX (convert floating-point value to integer)
**********************************************************/
/**********************************************************
* FLOAT (convert integer to floating-point value)
* op FLOAT(0b000[3]:0b001011[6]:g[2]:dst[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 15730 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15733 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15737 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15740 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15745 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15749 "isa_tms320.tcc"
> *DecodeOpFLOAT_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFLOAT_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15755 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15759 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15767 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15770 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15774 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15777 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15782 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15786 "isa_tms320.tcc"
> *DecodeOpFLOAT_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFLOAT_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15792 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15796 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15804 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15807 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15811 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15814 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15819 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15823 "isa_tms320.tcc"
> *DecodeOpFLOAT_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFLOAT_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15829 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15833 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15841 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15844 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15848 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15851 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15856 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15860 "isa_tms320.tcc"
> *DecodeOpFLOAT_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFLOAT_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15866 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15870 "isa_tms320.tcc"
	>(code, addr);
}

/* end of FLOAT (convert integer to floating-point value)
**********************************************************/
/**********************************************************
* LSH (logical shift)
* op LSH(0b000[3]:0b010011[6]:g[2]:dst[5]:count[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 15884 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15887 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15891 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15894 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15899 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15903 "isa_tms320.tcc"
> *DecodeOpLSH_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15909 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15913 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15921 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15924 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15928 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15931 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15936 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15940 "isa_tms320.tcc"
> *DecodeOpLSH_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15946 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15950 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15958 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15961 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 15965 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 15968 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 15973 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 15977 "isa_tms320.tcc"
> *DecodeOpLSH_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 15983 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 15987 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 15995 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 15998 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16002 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16005 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16010 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16014 "isa_tms320.tcc"
> *DecodeOpLSH_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16020 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16024 "isa_tms320.tcc"
	>(code, addr);
}

/* end of LSH (logical shift)
**********************************************************/
/**********************************************************
* MPYF (multiply floating-point values)
* op MPYF(0b000[3]:0b010100[6]:g[2]:dst[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 16038 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16041 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16045 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16048 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16053 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16057 "isa_tms320.tcc"
> *DecodeOpMPYF_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16063 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16067 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16075 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16078 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16082 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16085 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16090 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16094 "isa_tms320.tcc"
> *DecodeOpMPYF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16100 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16104 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16112 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16115 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16119 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16122 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16127 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16131 "isa_tms320.tcc"
> *DecodeOpMPYF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16137 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16141 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16149 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16152 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16156 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16159 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16164 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16168 "isa_tms320.tcc"
> *DecodeOpMPYF_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16174 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16178 "isa_tms320.tcc"
	>(code, addr);
}

/* end of MPYF (multiply floating-point values)
**********************************************************/
/**********************************************************
* MPYI (multiply integers)
* op MPYI(0b000[3]:0b010101[6]:g[2]:dst[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 16192 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16195 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16199 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16202 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16207 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16211 "isa_tms320.tcc"
> *DecodeOpMPYI_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16217 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16221 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16229 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16232 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16236 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16239 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16244 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16248 "isa_tms320.tcc"
> *DecodeOpMPYI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16254 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16258 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16266 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16269 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16273 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16276 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16281 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16285 "isa_tms320.tcc"
> *DecodeOpMPYI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16291 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16295 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16303 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16306 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16310 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16313 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16318 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16322 "isa_tms320.tcc"
> *DecodeOpMPYI_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16328 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16332 "isa_tms320.tcc"
	>(code, addr);
}

/* end of MPYI (multiply integers)
**********************************************************/
/**********************************************************
* NEGB (negate integer with borrow)
* op NEGB(0b000[3]:0b010110[6]:g[2]:dst[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 16346 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16349 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16353 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16356 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16361 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16365 "isa_tms320.tcc"
> *DecodeOpNEGB_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGB_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16371 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16375 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16383 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16386 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16390 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16393 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16398 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16402 "isa_tms320.tcc"
> *DecodeOpNEGB_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGB_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16408 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16412 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16420 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16423 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16427 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16430 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16435 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16439 "isa_tms320.tcc"
> *DecodeOpNEGB_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGB_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16445 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16449 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16457 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16460 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16464 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16467 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16472 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16476 "isa_tms320.tcc"
> *DecodeOpNEGB_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGB_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16482 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16486 "isa_tms320.tcc"
	>(code, addr);
}

/* end of NEGB (negate integer with borrow)
**********************************************************/
/**********************************************************
* NEGF (negate floating-point value)
* op NEGF(0b000[3]:0b010111[6]:g[2]:dst[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 16500 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16503 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16507 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16510 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16515 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16519 "isa_tms320.tcc"
> *DecodeOpNEGF_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16525 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16529 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16537 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16540 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16544 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16547 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16552 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16556 "isa_tms320.tcc"
> *DecodeOpNEGF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16562 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16566 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16574 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16577 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16581 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16584 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16589 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16593 "isa_tms320.tcc"
> *DecodeOpNEGF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16599 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16603 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16611 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16614 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16618 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16621 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16626 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16630 "isa_tms320.tcc"
> *DecodeOpNEGF_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16636 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16640 "isa_tms320.tcc"
	>(code, addr);
}

/* end of NEGF (negate floating-point value)
**********************************************************/
/**********************************************************
* NEGI (negate integer)
* op NEGI(0b000[3]:0b011000[6]:g[2]:dst[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 16654 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16657 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16661 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16664 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16669 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16673 "isa_tms320.tcc"
> *DecodeOpNEGI_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16679 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16683 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16691 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16694 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16698 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16701 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16706 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16710 "isa_tms320.tcc"
> *DecodeOpNEGI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16716 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16720 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16728 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16731 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16735 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16738 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16743 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16747 "isa_tms320.tcc"
> *DecodeOpNEGI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16753 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16757 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16765 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16768 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16772 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16775 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16780 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16784 "isa_tms320.tcc"
> *DecodeOpNEGI_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16790 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16794 "isa_tms320.tcc"
	>(code, addr);
}

/* end of NEGI (negate integer)
**********************************************************/
/**********************************************************
* NORM (normalize floating-point value)
* op NORM(0b000[3]:0b011010[6]:g[2]:dst[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 16808 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16811 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16815 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16818 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16823 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16827 "isa_tms320.tcc"
> *DecodeOpNORM_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNORM_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16833 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16837 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16845 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16848 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16852 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16855 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16860 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16864 "isa_tms320.tcc"
> *DecodeOpNORM_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNORM_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16870 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16874 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16882 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16885 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16889 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16892 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16897 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16901 "isa_tms320.tcc"
> *DecodeOpNORM_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNORM_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16907 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16911 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16919 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16922 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16926 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16929 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16934 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16938 "isa_tms320.tcc"
> *DecodeOpNORM_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNORM_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16944 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16948 "isa_tms320.tcc"
	>(code, addr);
}

/* end of NORM (normalize floating-point value)
**********************************************************/
/**********************************************************
* NOT (bitwise-logical complement)
* op NOT(0b000[3]:0b011011[6]:g[2]:dst[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 16962 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 16965 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 16969 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 16972 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 16977 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 16981 "isa_tms320.tcc"
> *DecodeOpNOT_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNOT_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 16987 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 16991 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 16999 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17002 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17006 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17009 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17014 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17018 "isa_tms320.tcc"
> *DecodeOpNOT_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNOT_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17024 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17028 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17036 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17039 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17043 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17046 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17051 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17055 "isa_tms320.tcc"
> *DecodeOpNOT_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNOT_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17061 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17065 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17073 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17076 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17080 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17083 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17088 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17092 "isa_tms320.tcc"
> *DecodeOpNOT_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNOT_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17098 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17102 "isa_tms320.tcc"
	>(code, addr);
}

/* end of NOT (bitwise-logical complement)
**********************************************************/
/**********************************************************
* OR (bitwise-logical OR)
* op OR(0b000[3]:0b100000[6]:g[2]:dst[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 17116 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17119 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17123 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17126 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17131 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17135 "isa_tms320.tcc"
> *DecodeOpOR_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17141 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17145 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17153 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17156 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17160 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17163 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17168 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17172 "isa_tms320.tcc"
> *DecodeOpOR_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17178 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17182 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17190 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17193 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17197 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17200 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17205 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17209 "isa_tms320.tcc"
> *DecodeOpOR_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17215 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17219 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17227 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17230 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17234 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17237 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17242 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17246 "isa_tms320.tcc"
> *DecodeOpOR_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17252 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17256 "isa_tms320.tcc"
	>(code, addr);
}

/* end of OR (bitwise-logical OR)
**********************************************************/
/**********************************************************
* RND (round floating-point value)
* op RND(0b000[3]:0b100010[6]:g[2]:dst[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 17270 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17273 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17277 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17280 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17285 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17289 "isa_tms320.tcc"
> *DecodeOpRND_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRND_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17295 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17299 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17307 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17310 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17314 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17317 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17322 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17326 "isa_tms320.tcc"
> *DecodeOpRND_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRND_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17332 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17336 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17344 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17347 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17351 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17354 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17359 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17363 "isa_tms320.tcc"
> *DecodeOpRND_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRND_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17369 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17373 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17381 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17384 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17388 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17391 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17396 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17400 "isa_tms320.tcc"
> *DecodeOpRND_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRND_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17406 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17410 "isa_tms320.tcc"
	>(code, addr);
}

/* end of RND (round floating-point value)
**********************************************************/
/**********************************************************
* ROL (rotate left)
* op ROL(0b000[3]:0b100011[6]:0b11[2]:dst[5]:0b0000000000000001[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 17424 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17427 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17431 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17434 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17439 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17443 "isa_tms320.tcc"
> *DecodeOpROL(CodeType code, typename CONFIG::address_t addr)
{
	return new OpROL<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17449 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17453 "isa_tms320.tcc"
	>(code, addr);
}

/* end of ROL (rotate left)
**********************************************************/
/**********************************************************
* ROLC (rotate left through carry)
* op ROLC(0b000[3]:0b100100[6]:0b11[2]:dst[5]:0b0000000000000001[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 17467 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17470 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17474 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17477 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17482 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17486 "isa_tms320.tcc"
> *DecodeOpROLC(CodeType code, typename CONFIG::address_t addr)
{
	return new OpROLC<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17492 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17496 "isa_tms320.tcc"
	>(code, addr);
}

/* end of ROLC (rotate left through carry)
**********************************************************/
/**********************************************************
* ROR (rotate right)
* op ROR(0b000[3]:0b100101[6]:0b11[2]:dst[5]:0b1111111111111111[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 17510 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17513 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17517 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17520 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17525 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17529 "isa_tms320.tcc"
> *DecodeOpROR(CodeType code, typename CONFIG::address_t addr)
{
	return new OpROR<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17535 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17539 "isa_tms320.tcc"
	>(code, addr);
}

/* end of ROR (rotate right)
**********************************************************/
/**********************************************************
* RORC (rotate right through carry)
* op RORC(0b000[3]:0b100110[6]:0b11[2]:dst[5]:0b1111111111111111[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 17553 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17556 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17560 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17563 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17568 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17572 "isa_tms320.tcc"
> *DecodeOpRORC(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRORC<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17578 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17582 "isa_tms320.tcc"
	>(code, addr);
}

/* end of RORC (rotate right through carry)
**********************************************************/
/**********************************************************
* SUBB (subtract integers with borrow)
* op SUBB(0b000[3]:0b101101[6]:g[2]:dst[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 17596 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17599 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17603 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17606 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17611 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17615 "isa_tms320.tcc"
> *DecodeOpSUBB_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBB_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17621 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17625 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17633 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17636 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17640 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17643 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17648 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17652 "isa_tms320.tcc"
> *DecodeOpSUBB_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBB_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17658 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17662 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17670 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17673 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17677 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17680 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17685 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17689 "isa_tms320.tcc"
> *DecodeOpSUBB_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBB_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17695 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17699 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17707 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17710 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17714 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17717 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17722 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17726 "isa_tms320.tcc"
> *DecodeOpSUBB_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBB_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17732 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17736 "isa_tms320.tcc"
	>(code, addr);
}

/* end of SUBB (subtract integers with borrow)
**********************************************************/
/**********************************************************
* SUBC (subtract integers conditionally)
* op SUBC(0b000[3]:0b101110[6]:g[2]:dst[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 17750 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17753 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17757 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17760 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17765 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17769 "isa_tms320.tcc"
> *DecodeOpSUBC_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBC_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17775 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17779 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17787 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17790 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17794 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17797 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17802 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17806 "isa_tms320.tcc"
> *DecodeOpSUBC_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBC_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17812 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17816 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17824 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17827 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17831 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17834 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17839 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17843 "isa_tms320.tcc"
> *DecodeOpSUBC_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBC_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17849 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17853 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17861 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17864 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17868 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17871 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17876 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17880 "isa_tms320.tcc"
> *DecodeOpSUBC_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBC_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17886 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17890 "isa_tms320.tcc"
	>(code, addr);
}

/* end of SUBC (subtract integers conditionally)
**********************************************************/
/**********************************************************
* SUBF (subtract floating-point values)
* op SUBF(0b000[3]:0b101111[6]:g[2]:dst[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 17904 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17907 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17911 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17914 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17919 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17923 "isa_tms320.tcc"
> *DecodeOpSUBF_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17929 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17933 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17941 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17944 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17948 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17951 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17956 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17960 "isa_tms320.tcc"
> *DecodeOpSUBF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 17966 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 17970 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 17978 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 17981 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 17985 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 17988 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 17993 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 17997 "isa_tms320.tcc"
> *DecodeOpSUBF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18003 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18007 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18015 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18018 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18022 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18025 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18030 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18034 "isa_tms320.tcc"
> *DecodeOpSUBF_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18040 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18044 "isa_tms320.tcc"
	>(code, addr);
}

/* end of SUBF (subtract floating-point values)
**********************************************************/
/**********************************************************
* SUBI (subtract integer)
* op SUBI(0b000[3]:0b110000[6]:g[2]:dst[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 18058 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18061 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18065 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18068 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18073 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18077 "isa_tms320.tcc"
> *DecodeOpSUBI_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18083 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18087 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18095 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18098 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18102 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18105 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18110 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18114 "isa_tms320.tcc"
> *DecodeOpSUBI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18120 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18124 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18132 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18135 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18139 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18142 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18147 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18151 "isa_tms320.tcc"
> *DecodeOpSUBI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18157 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18161 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18169 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18172 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18176 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18179 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18184 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18188 "isa_tms320.tcc"
> *DecodeOpSUBI_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18194 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18198 "isa_tms320.tcc"
	>(code, addr);
}

/* end of SUBI (subtract integer)
**********************************************************/
/**********************************************************
* SUBRB (subtract reverse integer with borrow)
* op SUBRB(0b000[3]:0b110001[6]:g[2]:dst[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 18212 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18215 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18219 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18222 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18227 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18231 "isa_tms320.tcc"
> *DecodeOpSUBRB_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRB_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18237 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18241 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18249 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18252 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18256 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18259 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18264 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18268 "isa_tms320.tcc"
> *DecodeOpSUBRB_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRB_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18274 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18278 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18286 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18289 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18293 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18296 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18301 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18305 "isa_tms320.tcc"
> *DecodeOpSUBRB_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRB_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18311 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18315 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18323 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18326 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18330 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18333 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18338 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18342 "isa_tms320.tcc"
> *DecodeOpSUBRB_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRB_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18348 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18352 "isa_tms320.tcc"
	>(code, addr);
}

/* end of SUBRB (subtract reverse integer with borrow)
**********************************************************/
/**********************************************************
* SUBRF (subtract reverse floating-point value)
* op SUBRF(0b000[3]:0b110010[6]:g[2]:dst[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 18366 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18369 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18373 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18376 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18381 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18385 "isa_tms320.tcc"
> *DecodeOpSUBRF_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18391 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18395 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18403 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18406 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18410 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18413 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18418 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18422 "isa_tms320.tcc"
> *DecodeOpSUBRF_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18428 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18432 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18440 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18443 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18447 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18450 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18455 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18459 "isa_tms320.tcc"
> *DecodeOpSUBRF_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18465 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18469 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18477 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18480 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18484 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18487 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18492 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18496 "isa_tms320.tcc"
> *DecodeOpSUBRF_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18502 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18506 "isa_tms320.tcc"
	>(code, addr);
}

/* end of SUBRF (subtract reverse floating-point value)
**********************************************************/
/**********************************************************
* SUBRI (subtract reverse integer)
* op SUBRI(0b000[3]:0b110011[6]:g[2]:dst[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 18520 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18523 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18527 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18530 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18535 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18539 "isa_tms320.tcc"
> *DecodeOpSUBRI_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18545 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18549 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18557 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18560 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18564 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18567 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18572 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18576 "isa_tms320.tcc"
> *DecodeOpSUBRI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18582 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18586 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18594 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18597 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18601 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18604 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18609 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18613 "isa_tms320.tcc"
> *DecodeOpSUBRI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18619 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18623 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18631 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18634 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18638 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18641 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18646 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18650 "isa_tms320.tcc"
> *DecodeOpSUBRI_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBRI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18656 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18660 "isa_tms320.tcc"
	>(code, addr);
}

/* end of SUBRI (subtract reverse integer)
**********************************************************/
/**********************************************************
* TSTB (test bit fields)
* op TSTB(0b000[3]:0b110100[6]:g[2]:dst[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 18674 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18677 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18681 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18684 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18689 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18693 "isa_tms320.tcc"
> *DecodeOpTSTB_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTSTB_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18699 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18703 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18711 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18714 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18718 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18721 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18726 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18730 "isa_tms320.tcc"
> *DecodeOpTSTB_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTSTB_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18736 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18740 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18748 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18751 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18755 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18758 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18763 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18767 "isa_tms320.tcc"
> *DecodeOpTSTB_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTSTB_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18773 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18777 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18785 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18788 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18792 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18795 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18800 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18804 "isa_tms320.tcc"
> *DecodeOpTSTB_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTSTB_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18810 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18814 "isa_tms320.tcc"
	>(code, addr);
}

/* end of TSTB (test bit fields)
**********************************************************/
/**********************************************************
* XOR (bitwise-exclusive OR)
* op XOR(0b000[3]:0b110101[6]:g[2]:dst[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 18828 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18831 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18835 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18838 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18843 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18847 "isa_tms320.tcc"
> *DecodeOpXOR_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18853 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18857 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18865 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18868 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18872 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18875 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18880 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18884 "isa_tms320.tcc"
> *DecodeOpXOR_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18890 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18894 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18902 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18905 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18909 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18912 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18917 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18921 "isa_tms320.tcc"
> *DecodeOpXOR_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18927 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18931 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 18939 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 18942 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 18946 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 18949 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 18954 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 18958 "isa_tms320.tcc"
> *DecodeOpXOR_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 18964 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 18968 "isa_tms320.tcc"
	>(code, addr);
}

/* end of XOR (bitwise-exclusive OR)
**********************************************************/
/*
*  Copyright (c) 2009,
*  Commissariat a l'Energie Atomique (CEA)
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without modification,
*  are permitted provided that the following conditions are met:
*
*   - Redistributions of source code must retain the above copyright notice, this
*     list of conditions and the following disclaimer.
*
*   - Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*     and/or other materials provided with the distribution.
*
*   - Neither the name of CEA nor the names of its contributors may be used to
*     endorse or promote products derived from this software without specific prior
*     written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
*  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED.
*  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
*  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
*  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
*  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
*  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
*          Gilles Mouchard (gilles.mouchard@cea.fr)
*/
/* List of 3 operands operations:
* - ADDC3        add with carry
* - ADDF3        add floating-point values
* - ADDI3        add integers
* - AND3         bitwise-logical AND
* - ANDN3        bitwise-logical AND with complement
* - ASH3         arithmetic shift
* - CMPF3        compare floating-point values
* - CMPI3        compare integers
* - LSH3         logical shift
* - MPYF3        multiply floating-point values
* - MPYI3        multiply integers
* - OR3          bitwise-logical OR
* - SUBB3        subtract integers with borrow
* - SUBF3        subtract floating-point values
* - SUBI3        subtract integers
* - TSTB3        test bit fields
* - XOR3         bitwise-exclusive OR
*/
/**********************************************************
* ADDC3 (add with carry)
* op ADDC3(0b001[3]:0b000000[6]:t[2]:dst[5]:src1[8]:src2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 19035 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19038 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19042 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19045 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19050 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19054 "isa_tms320.tcc"
> *DecodeOpADDC3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDC3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19060 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19064 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19072 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19075 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19079 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19082 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19087 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19091 "isa_tms320.tcc"
> *DecodeOpADDC3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDC3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19097 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19101 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19109 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19112 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19116 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19119 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19124 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19128 "isa_tms320.tcc"
> *DecodeOpADDC3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDC3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19134 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19138 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19146 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19149 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19153 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19156 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19161 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19165 "isa_tms320.tcc"
> *DecodeOpADDC3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDC3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19171 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19175 "isa_tms320.tcc"
	>(code, addr);
}

/* end of ADDC3 (add with carry)
**********************************************************/
/**********************************************************
* ADDF3 (add floating-point values)
* op ADDF3(0b001[3]:0b000001[6]:t[2]:dst[5]:src1[8]:src2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 19189 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19192 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19196 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19199 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19204 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19208 "isa_tms320.tcc"
> *DecodeOpADDF3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDF3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19214 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19218 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19226 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19229 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19233 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19236 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19241 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19245 "isa_tms320.tcc"
> *DecodeOpADDF3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDF3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19251 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19255 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19263 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19266 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19270 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19273 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19278 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19282 "isa_tms320.tcc"
> *DecodeOpADDF3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDF3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19288 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19292 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19300 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19303 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19307 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19310 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19315 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19319 "isa_tms320.tcc"
> *DecodeOpADDF3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDF3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19325 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19329 "isa_tms320.tcc"
	>(code, addr);
}

/* end of ADDF3 (add floating-point values)
**********************************************************/
/**********************************************************
* ADDI3 (add integers)
* op ADDI3(0b001[3]:0b000010[6]:t[2]:dst[5]:src1[8]:src2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 19343 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19346 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19350 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19353 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19358 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19362 "isa_tms320.tcc"
> *DecodeOpADDI3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDI3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19368 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19372 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19380 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19383 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19387 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19390 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19395 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19399 "isa_tms320.tcc"
> *DecodeOpADDI3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDI3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19405 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19409 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19417 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19420 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19424 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19427 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19432 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19436 "isa_tms320.tcc"
> *DecodeOpADDI3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDI3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19442 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19446 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19454 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19457 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19461 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19464 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19469 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19473 "isa_tms320.tcc"
> *DecodeOpADDI3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpADDI3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19479 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19483 "isa_tms320.tcc"
	>(code, addr);
}

/* end of ADDI3 (add integers)
**********************************************************/
/**********************************************************
* AND3 (bitwise-logical AND)
* op AND3(0b001[3]:0b000011[6]:t[2]:dst[5]:src1[8]:src2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 19497 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19500 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19504 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19507 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19512 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19516 "isa_tms320.tcc"
> *DecodeOpAND3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAND3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19522 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19526 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19534 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19537 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19541 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19544 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19549 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19553 "isa_tms320.tcc"
> *DecodeOpAND3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAND3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19559 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19563 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19571 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19574 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19578 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19581 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19586 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19590 "isa_tms320.tcc"
> *DecodeOpAND3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAND3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19596 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19600 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19608 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19611 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19615 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19618 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19623 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19627 "isa_tms320.tcc"
> *DecodeOpAND3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpAND3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19633 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19637 "isa_tms320.tcc"
	>(code, addr);
}

/* end of AND3 (bitwise-logical AND)
**********************************************************/
/**********************************************************
* ANDN3 (bitwise-logical AND with complement)
* op ANDN3(0b001[3]:0b000100[6]:t[2]:dst[5]:src1[8]:src2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 19651 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19654 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19658 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19661 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19666 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19670 "isa_tms320.tcc"
> *DecodeOpANDN3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpANDN3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19676 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19680 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19688 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19691 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19695 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19698 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19703 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19707 "isa_tms320.tcc"
> *DecodeOpANDN3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpANDN3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19713 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19717 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19725 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19728 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19732 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19735 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19740 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19744 "isa_tms320.tcc"
> *DecodeOpANDN3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpANDN3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19750 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19754 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19762 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19765 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19769 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19772 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19777 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19781 "isa_tms320.tcc"
> *DecodeOpANDN3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpANDN3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19787 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19791 "isa_tms320.tcc"
	>(code, addr);
}

/* end of ANDN3 (bitwise-logical AND with complement)
**********************************************************/
/**********************************************************
* ASH3 (arithmetic shift)
* op ASH3(0b001[3]:0b000101[6]:t[2]:dst[5]:src1[8]:src2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 19805 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19808 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19812 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19815 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19820 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19824 "isa_tms320.tcc"
> *DecodeOpASH3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpASH3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19830 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19834 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19842 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19845 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19849 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19852 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19857 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19861 "isa_tms320.tcc"
> *DecodeOpASH3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpASH3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19867 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19871 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19879 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19882 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19886 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19889 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19894 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19898 "isa_tms320.tcc"
> *DecodeOpASH3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpASH3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19904 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19908 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19916 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19919 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19923 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19926 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19931 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19935 "isa_tms320.tcc"
> *DecodeOpASH3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpASH3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19941 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19945 "isa_tms320.tcc"
	>(code, addr);
}

/* end of ASH3 (arithmetic shift)
**********************************************************/
/**********************************************************
* CMPF3 (compare floating-point values)
* op CMPF3(0b001[3]:0b000110[6]:t[2]:dst[5]:src1[8]:src2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 19959 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19962 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 19966 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 19969 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 19974 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 19978 "isa_tms320.tcc"
> *DecodeOpCMPF3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPF3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 19984 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 19988 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 19996 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 19999 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20003 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20006 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20011 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20015 "isa_tms320.tcc"
> *DecodeOpCMPF3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPF3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20021 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20025 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20033 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20036 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20040 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20043 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20048 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20052 "isa_tms320.tcc"
> *DecodeOpCMPF3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPF3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20058 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20062 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20070 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20073 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20077 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20080 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20085 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20089 "isa_tms320.tcc"
> *DecodeOpCMPF3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPF3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20095 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20099 "isa_tms320.tcc"
	>(code, addr);
}

/* end of CMPF3 (compare floating-point values)
**********************************************************/
/**********************************************************
* CMPI3 (compare integers)
* op CMPI3(0b001[3]:0b000111[6]:t[2]:dst[5]:src1[8]:src2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 20113 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20116 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20120 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20123 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20128 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20132 "isa_tms320.tcc"
> *DecodeOpCMPI3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPI3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20138 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20142 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20150 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20153 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20157 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20160 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20165 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20169 "isa_tms320.tcc"
> *DecodeOpCMPI3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPI3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20175 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20179 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20187 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20190 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20194 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20197 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20202 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20206 "isa_tms320.tcc"
> *DecodeOpCMPI3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPI3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20212 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20216 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20224 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20227 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20231 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20234 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20239 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20243 "isa_tms320.tcc"
> *DecodeOpCMPI3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCMPI3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20249 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20253 "isa_tms320.tcc"
	>(code, addr);
}

/* end of CMPI3 (compare integers)
**********************************************************/
/**********************************************************
* LSH3 (logical shift)
* op LSH3(0b001[3]:0b001000[6]:t[2]:dst[5]:src1[8]:src2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 20267 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20270 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20274 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20277 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20282 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20286 "isa_tms320.tcc"
> *DecodeOpLSH3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20292 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20296 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20304 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20307 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20311 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20314 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20319 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20323 "isa_tms320.tcc"
> *DecodeOpLSH3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20329 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20333 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20341 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20344 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20348 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20351 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20356 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20360 "isa_tms320.tcc"
> *DecodeOpLSH3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20366 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20370 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20378 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20381 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20385 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20388 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20393 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20397 "isa_tms320.tcc"
> *DecodeOpLSH3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20403 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20407 "isa_tms320.tcc"
	>(code, addr);
}

/* end of LSH3 (logical shift)
**********************************************************/
/**********************************************************
* MPYF3 (multiply floating-point values)
* op MPYF3(0b001[3]:0b001001[6]:t[2]:dst[5]:src1[8]:src2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 20421 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20424 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20428 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20431 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20436 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20440 "isa_tms320.tcc"
> *DecodeOpMPYF3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20446 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20450 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20458 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20461 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20465 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20468 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20473 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20477 "isa_tms320.tcc"
> *DecodeOpMPYF3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20483 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20487 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20495 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20498 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20502 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20505 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20510 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20514 "isa_tms320.tcc"
> *DecodeOpMPYF3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20520 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20524 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20532 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20535 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20539 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20542 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20547 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20551 "isa_tms320.tcc"
> *DecodeOpMPYF3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20557 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20561 "isa_tms320.tcc"
	>(code, addr);
}

/* end of MPYF3 (multiply floating-point values)
**********************************************************/
/**********************************************************
* MPYI3 (multiply integers)
* op MPYI3(0b001[3]:0b001010[6]:t[2]:dst[5]:src1[8]:src2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 20575 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20578 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20582 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20585 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20590 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20594 "isa_tms320.tcc"
> *DecodeOpMPYI3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20600 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20604 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20612 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20615 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20619 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20622 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20627 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20631 "isa_tms320.tcc"
> *DecodeOpMPYI3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20637 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20641 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20649 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20652 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20656 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20659 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20664 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20668 "isa_tms320.tcc"
> *DecodeOpMPYI3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20674 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20678 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20686 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20689 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20693 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20696 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20701 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20705 "isa_tms320.tcc"
> *DecodeOpMPYI3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20711 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20715 "isa_tms320.tcc"
	>(code, addr);
}

/* end of MPYI3 (multiply integers)
**********************************************************/
/**********************************************************
* OR3 (bitwise-logical OR)
* op OR3(0b001[3]:0b001011[6]:t[2]:dst[5]:src1[8]:src2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 20729 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20732 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20736 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20739 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20744 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20748 "isa_tms320.tcc"
> *DecodeOpOR3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20754 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20758 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20766 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20769 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20773 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20776 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20781 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20785 "isa_tms320.tcc"
> *DecodeOpOR3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20791 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20795 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20803 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20806 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20810 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20813 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20818 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20822 "isa_tms320.tcc"
> *DecodeOpOR3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20828 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20832 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20840 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20843 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20847 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20850 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20855 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20859 "isa_tms320.tcc"
> *DecodeOpOR3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20865 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20869 "isa_tms320.tcc"
	>(code, addr);
}

/* end of OR3 (bitwise-logical OR)
**********************************************************/
/**********************************************************
* SUBB3 (subtract integers with borrow)
* op SUBB3(0b001[3]:0b001100[6]:t[2]:dst[5]:src1[8]:src2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 20883 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20886 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20890 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20893 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20898 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20902 "isa_tms320.tcc"
> *DecodeOpSUBB3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBB3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20908 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20912 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20920 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20923 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20927 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20930 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20935 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20939 "isa_tms320.tcc"
> *DecodeOpSUBB3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBB3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20945 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20949 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20957 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20960 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 20964 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 20967 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 20972 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 20976 "isa_tms320.tcc"
> *DecodeOpSUBB3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBB3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 20982 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 20986 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 20994 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 20997 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21001 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21004 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21009 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21013 "isa_tms320.tcc"
> *DecodeOpSUBB3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBB3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21019 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21023 "isa_tms320.tcc"
	>(code, addr);
}

/* end of SUBB3 (subtract integers with borrow)
**********************************************************/
/**********************************************************
* SUBF3 (subtract floating-point values)
* op SUBF3(0b001[3]:0b001101[6]:t[2]:dst[5]:src1[8]:src2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 21037 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21040 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21044 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21047 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21052 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21056 "isa_tms320.tcc"
> *DecodeOpSUBF3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21062 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21066 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21074 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21077 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21081 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21084 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21089 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21093 "isa_tms320.tcc"
> *DecodeOpSUBF3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21099 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21103 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21111 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21114 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21118 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21121 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21126 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21130 "isa_tms320.tcc"
> *DecodeOpSUBF3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21136 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21140 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21148 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21151 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21155 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21158 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21163 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21167 "isa_tms320.tcc"
> *DecodeOpSUBF3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21173 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21177 "isa_tms320.tcc"
	>(code, addr);
}

/* end of SUBF3 (subtract floating-point values)
**********************************************************/
/**********************************************************
* SUBI3 (subtract integers)
* op SUBI3(0b001[3]:0b001110[6]:t[2]:dst[5]:src1[8]:src2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 21191 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21194 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21198 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21201 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21206 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21210 "isa_tms320.tcc"
> *DecodeOpSUBI3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21216 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21220 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21228 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21231 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21235 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21238 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21243 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21247 "isa_tms320.tcc"
> *DecodeOpSUBI3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21253 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21257 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21265 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21268 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21272 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21275 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21280 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21284 "isa_tms320.tcc"
> *DecodeOpSUBI3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21290 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21294 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21302 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21305 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21309 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21312 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21317 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21321 "isa_tms320.tcc"
> *DecodeOpSUBI3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21327 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21331 "isa_tms320.tcc"
	>(code, addr);
}

/* end of SUBI3 (subtract integers)
**********************************************************/
/**********************************************************
* TSTB3 (test bit fields)
* op TSTB3(0b001[3]:0b001111[6]:t[2]:dst[5]:src1[8]:src2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 21345 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21348 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21352 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21355 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21360 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21364 "isa_tms320.tcc"
> *DecodeOpTSTB3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTSTB3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21370 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21374 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21382 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21385 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21389 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21392 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21397 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21401 "isa_tms320.tcc"
> *DecodeOpTSTB3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTSTB3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21407 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21411 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21419 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21422 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21426 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21429 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21434 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21438 "isa_tms320.tcc"
> *DecodeOpTSTB3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTSTB3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21444 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21448 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21456 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21459 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21463 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21466 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21471 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21475 "isa_tms320.tcc"
> *DecodeOpTSTB3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTSTB3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21481 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21485 "isa_tms320.tcc"
	>(code, addr);
}

/* end of TSTB3 (test bit fields)
**********************************************************/
/**********************************************************
* XOR3 (bitwise-exclusive OR)
* op XOR3(0b001[3]:0b010000[6]:t[2]:dst[5]:src1[8]:src2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 21499 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21502 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21506 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21509 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21514 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21518 "isa_tms320.tcc"
> *DecodeOpXOR3_reg_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21524 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21528 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21536 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21539 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21543 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21546 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21551 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21555 "isa_tms320.tcc"
> *DecodeOpXOR3_indir_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21561 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21565 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21573 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21576 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21580 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21583 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21588 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21592 "isa_tms320.tcc"
> *DecodeOpXOR3_reg_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21598 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21602 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21610 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21613 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21617 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21620 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21625 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21629 "isa_tms320.tcc"
> *DecodeOpXOR3_indir_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21635 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21639 "isa_tms320.tcc"
	>(code, addr);
}

/* end of XOR3 (bitwise-exclusive OR)
**********************************************************/
/*
*  Copyright (c) 2009,
*  Commissariat a l'Energie Atomique (CEA)
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without modification,
*  are permitted provided that the following conditions are met:
*
*   - Redistributions of source code must retain the above copyright notice, this
*     list of conditions and the following disclaimer.
*
*   - Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*     and/or other materials provided with the distribution.
*
*   - Neither the name of CEA nor the names of its contributors may be used to
*     endorse or promote products derived from this software without specific prior
*     written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
*  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED.
*  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
*  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
*  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
*  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
*  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
*          Gilles Mouchard (gilles.mouchard@cea.fr)
*/
/* List of program-control operations:
* - Bcond        branch conditionally (standard)
* - BcondD       branch conditionally (delayed)
* - BR           branch unconditionally (standard)
* - BRD          branch unconditionally (delayed)
* - CALL         call subroutine
* - CALLcond     call subroutine conditionally
* - DBcond       decrement and branch conditionally (standard)
* - DBcondD      decrement and branch conditionally (delayed)
* - IACK         interrupt acknowledge
* - IDLE         idle until interrupt
* - NOP          no operation
* - RETIcond     return from interrupt conditionally
* - RETScond     return form subroutine conditionally
* - RPTB         repeat block of instructions
* - RPTS         repeat single instruction
* - SWI          software interrupt
* - TRAPcond     trap conditionally
*/
/**********************************************************
* Bcond (branch conditionally (standard))
* op Bcond(0b011010[6]:b[1]:0b000[3]:0b0[1]:cond[5]:reg_or_disp[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 21706 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21709 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21713 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21716 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21721 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21725 "isa_tms320.tcc"
> *DecodeOpBcond_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21731 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21735 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21743 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21746 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21750 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21753 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21758 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21762 "isa_tms320.tcc"
> *DecodeOpBcond_disp(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBcond_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21768 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21772 "isa_tms320.tcc"
	>(code, addr);
}

/* end of Bcond (branch conditionally (standard))
**********************************************************/
/**********************************************************
* BcondD (branch conditionally (delayed))
* op BcondD(0b011010[6]:b[1]:0b000[3]:0b1[1]:cond[5]:reg_or_disp[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 21786 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21789 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21793 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21796 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21801 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21805 "isa_tms320.tcc"
> *DecodeOpBcondD_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBcondD_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21811 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21815 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 21823 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21826 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21830 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21833 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21838 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21842 "isa_tms320.tcc"
> *DecodeOpBcondD_disp(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBcondD_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21848 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21852 "isa_tms320.tcc"
	>(code, addr);
}

/* end of BcondD (branch conditionally (delayed))
**********************************************************/
/**********************************************************
* BR (branch unconditionally (standard))
* op BR(0b0110000[7]:0b0[1]:src[24])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 21866 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21869 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21873 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21876 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21881 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21885 "isa_tms320.tcc"
> *DecodeOpBR(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBR<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21891 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21895 "isa_tms320.tcc"
	>(code, addr);
}

/* end of BR (branch unconditionally (standard))
**********************************************************/
/**********************************************************
* BRD (branch unconditionally (delayed))
* op BRD(0b0110000[7]:0b1[1]:src[24])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 21909 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21912 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21916 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21919 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21924 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21928 "isa_tms320.tcc"
> *DecodeOpBRD(CodeType code, typename CONFIG::address_t addr)
{
	return new OpBRD<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21934 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21938 "isa_tms320.tcc"
	>(code, addr);
}

/* end of BRD (branch unconditionally (delayed))
**********************************************************/
/**********************************************************
* CALL (call subroutine)
* op CALL(0b0110001[7]:0b0[1]:src[24])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 21952 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21955 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 21959 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 21962 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 21967 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 21971 "isa_tms320.tcc"
> *DecodeOpCALL(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCALL<
#line 45 "isa/tms320.isa"
	CONFIG
#line 21977 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 21981 "isa_tms320.tcc"
	>(code, addr);
}

/* end of CALL (call subroutine)
**********************************************************/
/**********************************************************
* CALLcond (call subroutine conditionally)
* op CALLcond(0b011100[6]:b[1]:0b0000[4]:cond[5]:reg_or_disp[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 21995 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 21998 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22002 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22005 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22010 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22014 "isa_tms320.tcc"
> *DecodeOpCALLcond_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCALLcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22020 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22024 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22032 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22035 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22039 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22042 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22047 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22051 "isa_tms320.tcc"
> *DecodeOpCALLcond_disp(CodeType code, typename CONFIG::address_t addr)
{
	return new OpCALLcond_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22057 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22061 "isa_tms320.tcc"
	>(code, addr);
}

/* end of CALLcond (call subroutine conditionally)
**********************************************************/
/**********************************************************
* DBcond (decrement and branch conditionally (standard))
* op DBcond(0b011011[6]:b[1]:ar[3]:0b0[1]:cond[5]:reg_or_disp[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 22075 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22078 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22082 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22085 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22090 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22094 "isa_tms320.tcc"
> *DecodeOpDBcond_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpDBcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22100 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22104 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22112 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22115 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22119 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22122 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22127 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22131 "isa_tms320.tcc"
> *DecodeOpDBcond_disp(CodeType code, typename CONFIG::address_t addr)
{
	return new OpDBcond_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22137 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22141 "isa_tms320.tcc"
	>(code, addr);
}

/* end of DBcond (decrement and branch conditionally (standard))
**********************************************************/
/**********************************************************
* DBcondD (decrement and branch conditionally (delayed))
* op DBcondD(0b011011[6]:b[1]:ar[3]:0b1[1]:cond[5]:reg_or_disp[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 22155 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22158 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22162 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22165 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22170 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22174 "isa_tms320.tcc"
> *DecodeOpDBcondD_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpDBcondD_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22180 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22184 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22192 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22195 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22199 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22202 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22207 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22211 "isa_tms320.tcc"
> *DecodeOpDBcondD_disp(CodeType code, typename CONFIG::address_t addr)
{
	return new OpDBcondD_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22217 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22221 "isa_tms320.tcc"
	>(code, addr);
}

/* end of DBcondD (decrement and branch conditionally (delayed))
**********************************************************/
/**********************************************************
* IACK (interrupt acknowledge)
* op IACK(0b000[3]:0b110110[6]:g[2]:0b00000[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 22235 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22238 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22242 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22245 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22250 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22254 "isa_tms320.tcc"
> *DecodeOpIACK_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpIACK_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22260 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22264 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22272 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22275 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22279 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22282 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22287 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22291 "isa_tms320.tcc"
> *DecodeOpIACK_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpIACK_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22297 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22301 "isa_tms320.tcc"
	>(code, addr);
}

/* end of IACK (interrupt acknowledge)
**********************************************************/
/**********************************************************
* IDLE (idle until interrupt)
* op IDLE(0b000[3]:0b001100[6]:0[23])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 22315 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22318 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22322 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22325 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22330 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22334 "isa_tms320.tcc"
> *DecodeOpIDLE(CodeType code, typename CONFIG::address_t addr)
{
	return new OpIDLE<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22340 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22344 "isa_tms320.tcc"
	>(code, addr);
}

/* end of IDLE (idle until interrupt)
**********************************************************/
/**********************************************************
* NOP (no operation)
* op NOP(0b000[3]:0b011001[6]:g[2]:0b00000[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 22358 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22361 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22365 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22368 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22373 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22377 "isa_tms320.tcc"
> *DecodeOpNOP_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNOP_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22383 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22387 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22395 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22398 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22402 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22405 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22410 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22414 "isa_tms320.tcc"
> *DecodeOpNOP_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNOP_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22420 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22424 "isa_tms320.tcc"
	>(code, addr);
}

/* end of NOP (no operation)
**********************************************************/
/**********************************************************
* RETIcond (return from interrupt conditionally)
* op RETIcond(0b01111[5]:0b0000[4]:0b00[2]:cond[5]:0[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 22438 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22441 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22445 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22448 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22453 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22457 "isa_tms320.tcc"
> *DecodeOpRETIcond(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRETIcond<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22463 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22467 "isa_tms320.tcc"
	>(code, addr);
}

/* end of RETIcond (return from interrupt conditionally)
**********************************************************/
/**********************************************************
* RETScond (return form subroutine conditionally)
* op RETScond(0b01111[5]:0b0001[4]:0b00[2]:cond[5]:0[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 22481 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22484 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22488 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22491 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22496 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22500 "isa_tms320.tcc"
> *DecodeOpRETScond(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRETScond<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22506 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22510 "isa_tms320.tcc"
	>(code, addr);
}

/* end of RETScond (return form subroutine conditionally)
**********************************************************/
/**********************************************************
* RPTB (repeat block of instructions)
* op RPTB(0b01100100[8]:src[24])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 22524 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22527 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22531 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22534 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22539 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22543 "isa_tms320.tcc"
> *DecodeOpRPTB(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRPTB<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22549 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22553 "isa_tms320.tcc"
	>(code, addr);
}

/* end of RPTB (repeat block of instructions)
**********************************************************/
/**********************************************************
* RPTS (repeat single instruction)
* op RPTS(0b000[3]:0b100111[6]:g[2]:0b11011[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 22567 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22570 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22574 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22577 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22582 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22586 "isa_tms320.tcc"
> *DecodeOpRPTS_reg(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRPTS_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22592 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22596 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22604 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22607 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22611 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22614 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22619 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22623 "isa_tms320.tcc"
> *DecodeOpRPTS_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRPTS_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22629 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22633 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22641 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22644 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22648 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22651 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22656 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22660 "isa_tms320.tcc"
> *DecodeOpRPTS_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRPTS_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22666 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22670 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 22678 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22681 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22685 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22688 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22693 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22697 "isa_tms320.tcc"
> *DecodeOpRPTS_imm(CodeType code, typename CONFIG::address_t addr)
{
	return new OpRPTS_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22703 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22707 "isa_tms320.tcc"
	>(code, addr);
}

/* end of RPTS (repeat single instruction)
**********************************************************/
/**********************************************************
* SWI (software interrupt)
* op SWI(0b0110011[7]:0b0000[4]:0[21])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 22721 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22724 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22728 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22731 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22736 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22740 "isa_tms320.tcc"
> *DecodeOpSWI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSWI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22746 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22750 "isa_tms320.tcc"
	>(code, addr);
}

/* end of SWI (software interrupt)
**********************************************************/
/**********************************************************
* TRAPcond (trap conditionally)
* op TRAPcond(0b0111010[7]:0b0000[4]:cond[5]:0b01[11]:n[5])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 22764 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22767 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22771 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22774 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22779 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22783 "isa_tms320.tcc"
> *DecodeOpTRAPcond(CodeType code, typename CONFIG::address_t addr)
{
	return new OpTRAPcond<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22789 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22793 "isa_tms320.tcc"
	>(code, addr);
}

/* end of TRAPcond (trap conditionally)
**********************************************************/
/*
*  Copyright (c) 2009,
*  Commissariat a l'Energie Atomique (CEA)
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without modification,
*  are permitted provided that the following conditions are met:
*
*   - Redistributions of source code must retain the above copyright notice, this
*     list of conditions and the following disclaimer.
*
*   - Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*     and/or other materials provided with the distribution.
*
*   - Neither the name of CEA nor the names of its contributors may be used to
*     endorse or promote products derived from this software without specific prior
*     written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
*  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED.
*  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
*  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
*  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
*  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
*  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
*          Gilles Mouchard (gilles.mouchard@cea.fr)
*/
/* List of low-power control operations:
* - IDLE2        low-power idle
* - LOPOWER      divide clock by 16
* - MAXSPEED     restore clock to regular speed
*/
/**********************************************************
* IDLE2 (low-power idle)
* op IDLE2(0b000[3]:0b001100[6]:0b001[23])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 22846 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22849 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22853 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22856 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22861 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22865 "isa_tms320.tcc"
> *DecodeOpIDLE2(CodeType code, typename CONFIG::address_t addr)
{
	return new OpIDLE2<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22871 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22875 "isa_tms320.tcc"
	>(code, addr);
}

/* end of IDLE2 (low-power idle)
**********************************************************/
/**********************************************************
* LOPOWER (divide clock by 16)
* op LOPOWER(0b000[3]:0b100001[6]:0b001[23])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 22889 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22892 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22896 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22899 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22904 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22908 "isa_tms320.tcc"
> *DecodeOpLOPOWER(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLOPOWER<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22914 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22918 "isa_tms320.tcc"
	>(code, addr);
}

/* end of LOPOWER (divide clock by 16)
**********************************************************/
/**********************************************************
* MAXSPEED (restore clock to regular speed)
* op MAXSPEED(0b000[3]:0b100001[6]:0[23])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 22932 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 22935 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 22939 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 22942 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 22947 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 22951 "isa_tms320.tcc"
> *DecodeOpMAXSPEED(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMAXSPEED<
#line 45 "isa/tms320.isa"
	CONFIG
#line 22957 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 22961 "isa_tms320.tcc"
	>(code, addr);
}

/* end of MAXSPEED (restore clock to regular speed)
**********************************************************/
/*
*  Copyright (c) 2009,
*  Commissariat a l'Energie Atomique (CEA)
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without modification,
*  are permitted provided that the following conditions are met:
*
*   - Redistributions of source code must retain the above copyright notice, this
*     list of conditions and the following disclaimer.
*
*   - Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*     and/or other materials provided with the distribution.
*
*   - Neither the name of CEA nor the names of its contributors may be used to
*     endorse or promote products derived from this software without specific prior
*     written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
*  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED.
*  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
*  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
*  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
*  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
*  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
*          Gilles Mouchard (gilles.mouchard@cea.fr)
*/
/* List of interlocked-operations:
* - LDFI         load floating-point value, interlocked
* - LDII         load integer, interlocked
* - SIGI         signal, interlocked
* - STFI         store floating-point value, interlocked
* - STII         store integer, interflocked
*/
/**********************************************************
* LDFI (load floating-point value, interlocked)
* op LDFI(0b000[3]:0b001111[6]:g[2]:dst[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 23016 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23019 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23023 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23026 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23031 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23035 "isa_tms320.tcc"
> *DecodeOpLDFI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDFI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23041 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23045 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 23053 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23056 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23060 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23063 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23068 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23072 "isa_tms320.tcc"
> *DecodeOpLDFI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDFI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23078 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23082 "isa_tms320.tcc"
	>(code, addr);
}

/* end of LDFI (load floating-point value, interlocked)
**********************************************************/
/**********************************************************
* LDII (load integer, interlocked)
* op LDII(0b000[3]:0b010001[6]:g[2]:dst[5]:src[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 23096 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23099 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23103 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23106 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23111 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23115 "isa_tms320.tcc"
> *DecodeOpLDII_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDII_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23121 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23125 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 23133 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23136 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23140 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23143 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23148 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23152 "isa_tms320.tcc"
> *DecodeOpLDII_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDII_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23158 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23162 "isa_tms320.tcc"
	>(code, addr);
}

/* end of LDII (load integer, interlocked)
**********************************************************/
/**********************************************************
* SIGI (signal, interlocked)
* op SIGI(0b000[3]:0b101100[6]:0b00[2]:0[21])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 23176 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23179 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23183 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23186 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23191 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23195 "isa_tms320.tcc"
> *DecodeOpSIGI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSIGI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23201 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23205 "isa_tms320.tcc"
	>(code, addr);
}

/* end of SIGI (signal, interlocked)
**********************************************************/
/**********************************************************
* STFI (store floating-point value, interlocked)
* op STFI(0b000[3]:0b101001[6]:g[2]:src[5]:dst[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 23219 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23222 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23226 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23229 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23234 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23238 "isa_tms320.tcc"
> *DecodeOpSTFI_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTFI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23244 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23248 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 23256 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23259 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23263 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23266 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23271 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23275 "isa_tms320.tcc"
> *DecodeOpSTFI_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTFI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23281 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23285 "isa_tms320.tcc"
	>(code, addr);
}

/* end of STFI (store floating-point value, interlocked)
**********************************************************/
/**********************************************************
* STII (store integer, interflocked)
* op STII(0b000[3]:0b101011[6]:g[2]:src[5]:dst[16])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 23299 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23302 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23306 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23309 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23314 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23318 "isa_tms320.tcc"
> *DecodeOpSTII_dir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTII_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23324 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23328 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 23336 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23339 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23343 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23346 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23351 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23355 "isa_tms320.tcc"
> *DecodeOpSTII_indir(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTII_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23361 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23365 "isa_tms320.tcc"
	>(code, addr);
}

/* end of STII (store integer, interflocked)
**********************************************************/
/*
*  Copyright (c) 2009,
*  Commissariat a l'Energie Atomique (CEA)
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without modification,
*  are permitted provided that the following conditions are met:
*
*   - Redistributions of source code must retain the above copyright notice, this
*     list of conditions and the following disclaimer.
*
*   - Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*     and/or other materials provided with the distribution.
*
*   - Neither the name of CEA nor the names of its contributors may be used to
*     endorse or promote products derived from this software without specific prior
*     written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
*  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED.
*  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
*  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
*  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
*  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
*  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Authors: Daniel Gracia Perez (daniel.gracia-perez@cea.fr)
*          Gilles Mouchard (gilles.mouchard@cea.fr)
*/
/* List of parallel operations:
* - FLOAT || STF		convert integer to floating-point value and store floating-point value
* - LDF || STF			load floating-point value and store floating-point value
* - LDI || STI			load integer and store integer
* - LSH3 || STI		logical shift and store integer
* - MPYF3 || STF       multiply floating-point values and store floating-point value
* - MPYI3 || STI		multiply integer and store integer
* - NEGF || STF		negate floating-point value and store floating-point value
* - NEGI || STI		negate integer and store integer
* - NOT || STI			complement value and store integer
* - OR3 || STI			bitwise-logical OR value and store integer
* - STF || STF			store floating-point values
* - STI || STI			sotre integers
* - SUBF3 || STF		subtract floating-point value and store floating-point value
* - SUBI3 || STI		subtract integer and store integer
* - XOR3 || STI		bitwise-exclusive OR values and store integer
* - LDF || LDF			load floating-point value
* - LDI || LDI			load integer
* - MPYF3 || ADDF3		multiply and add floating-point value
* - MPYF3 || SUBF3		multiply and subtract floating-point value
* - MPYI3 || ADDI3		multiply and add integer
* - MPYI3 || SUBI3		multiply and subtract integer
*/
/**********************************************************
* FLOAT || STF (convert integer to floating-point value and store floating-point value)
* op FLOAT_STF(0b11[2]:0b01011[5]:dst1[3]:0b000[3]:src3[3]:dst2[8]:src2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 23436 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23439 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23443 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23446 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23451 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23455 "isa_tms320.tcc"
> *DecodeOpFLOAT_STF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFLOAT_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23461 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23465 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 23473 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23476 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23480 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23483 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23488 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23492 "isa_tms320.tcc"
> *DecodeOpFLOAT_STF_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpFLOAT_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23498 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23502 "isa_tms320.tcc"
	>(code, addr);
}

/* end of FLOAT || STF (convert integer to floating-point value and store floating-point value)
**********************************************************/
/**********************************************************
* LDF || STF (load floating-point value and store floating-point value)
* op LDF_STF(0b11[2]:0b01100[5]:dst1[3]:0b000[3]:src3[3]:dst2[8]:src2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 23516 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23519 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23523 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23526 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23531 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23535 "isa_tms320.tcc"
> *DecodeOpLDF_STF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDF_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23541 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23545 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 23553 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23556 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23560 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23563 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23568 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23572 "isa_tms320.tcc"
> *DecodeOpLDF_STF_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDF_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23578 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23582 "isa_tms320.tcc"
	>(code, addr);
}

/* end of LDF || STF (load floating-point value and store floating-point value)
**********************************************************/
/**********************************************************
* LDI || STI (load integer and store integer)
* op LDI_STI(0b11[2]:0b01101[5]:dst1[3]:0b000[3]:src3[3]:dst2[8]:src2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 23596 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23599 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23603 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23606 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23611 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23615 "isa_tms320.tcc"
> *DecodeOpLDI_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDI_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23621 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23625 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 23633 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23636 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23640 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23643 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23648 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23652 "isa_tms320.tcc"
> *DecodeOpLDI_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDI_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23658 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23662 "isa_tms320.tcc"
	>(code, addr);
}

/* end of LDI || STI (load integer and store integer)
**********************************************************/
/**********************************************************
* LSH3 || STI (logical shift and store integer)
* op LSH3_STI(0b11[2]:0b01110[5]:dst1[3]:count[3]:src3[3]:dst2[8]:src2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 23676 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23679 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23683 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23686 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23691 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23695 "isa_tms320.tcc"
> *DecodeOpLSH3_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23701 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23705 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 23713 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23716 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23720 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23723 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23728 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23732 "isa_tms320.tcc"
> *DecodeOpLSH3_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLSH3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23738 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23742 "isa_tms320.tcc"
	>(code, addr);
}

/* end of LSH3 || STI (logical shift and store integer)
**********************************************************/
/**********************************************************
* MPYF3 || STF (multiply floating-point values and store floating-point value)
* op MPYF3_STF(0b11[2]:0b01111[5]:dst1[3]:src1[3]:src3[3]:dst2[8]:src2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 23756 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23759 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23763 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23766 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23771 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23775 "isa_tms320.tcc"
> *DecodeOpMPYF3_STF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23781 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23785 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 23793 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23796 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23800 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23803 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23808 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23812 "isa_tms320.tcc"
> *DecodeOpMPYF3_STF_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23818 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23822 "isa_tms320.tcc"
	>(code, addr);
}

/* end of MPYF3 || STF (multiply floating-point values and store floating-point value)
**********************************************************/
/**********************************************************
* MPYI3 || STI (multiply integer and store integer)
* op MPYI3_STI(0b11[2]:0b10000[5]:dst1[3]:src1[3]:src3[3]:dst2[8]:src2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 23836 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23839 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23843 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23846 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23851 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23855 "isa_tms320.tcc"
> *DecodeOpMPYI3_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23861 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23865 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 23873 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23876 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23880 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23883 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23888 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23892 "isa_tms320.tcc"
> *DecodeOpMPYI3_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23898 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23902 "isa_tms320.tcc"
	>(code, addr);
}

/* end of MPYI3 || STI (multiply integer and store integer)
**********************************************************/
/**********************************************************
* NEGF || STF (negate floating-point value and store floating-point value)
* op NEGF_STF(0b11[2]:0b10001[5]:dst1[3]:0b000[3]:src3[3]:dst2[8]:src2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 23916 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23919 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23923 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23926 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23931 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23935 "isa_tms320.tcc"
> *DecodeOpNEGF_STF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGF_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23941 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23945 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 23953 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23956 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 23960 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 23963 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 23968 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 23972 "isa_tms320.tcc"
> *DecodeOpNEGF_STF_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGF_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 23978 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 23982 "isa_tms320.tcc"
	>(code, addr);
}

/* end of NEGF || STF (negate floating-point value and store floating-point value)
**********************************************************/
/**********************************************************
* NEGI || STI (negate integer and store integer)
* op NEGI_STI(0b11[2]:0b10010[5]:dst1[3]:0b000[3]:src3[3]:dst2[8]:src2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 23996 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 23999 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24003 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24006 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24011 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24015 "isa_tms320.tcc"
> *DecodeOpNEGI_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGI_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 24021 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 24025 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 24033 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24036 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24040 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24043 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24048 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24052 "isa_tms320.tcc"
> *DecodeOpNEGI_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNEGI_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 24058 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 24062 "isa_tms320.tcc"
	>(code, addr);
}

/* end of NEGI || STI (negate integer and store integer)
**********************************************************/
/**********************************************************
* NOT || STI (complement value and store integer)
* op NOT_STI(0b11[2]:0b10011[5]:dst1[3]:0b000[3]:src3[3]:dst2[8]:src2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 24076 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24079 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24083 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24086 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24091 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24095 "isa_tms320.tcc"
> *DecodeOpNOT_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNOT_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 24101 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 24105 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 24113 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24116 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24120 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24123 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24128 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24132 "isa_tms320.tcc"
> *DecodeOpNOT_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpNOT_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 24138 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 24142 "isa_tms320.tcc"
	>(code, addr);
}

/* end of NOT || STI (complement value and store integer)
**********************************************************/
/**********************************************************
* OR3 || STI (bitwise-logical OR value and store integer)
* op OR3_STI(0b11[2]:0b10100[5]:dst1[3]:src1[3]:src3[3]:dst2[8]:src2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 24156 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24159 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24163 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24166 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24171 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24175 "isa_tms320.tcc"
> *DecodeOpOR3_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 24181 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 24185 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 24193 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24196 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24200 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24203 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24208 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24212 "isa_tms320.tcc"
> *DecodeOpOR3_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpOR3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 24218 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 24222 "isa_tms320.tcc"
	>(code, addr);
}

/* end of OR3 || STI (bitwise-logical OR value and store integer)
**********************************************************/
/**********************************************************
* STF || STF (store floating-point values)
* op STF_STF(0b11[2]:0b00000[5]:src2[3]:0b000[3]:src1[3]:dst1[8]:dst2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 24236 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24239 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24243 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24246 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24251 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24255 "isa_tms320.tcc"
> *DecodeOpSTF_STF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTF_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 24261 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 24265 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 24273 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24276 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24280 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24283 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24288 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24292 "isa_tms320.tcc"
> *DecodeOpSTF_STF_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTF_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 24298 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 24302 "isa_tms320.tcc"
	>(code, addr);
}

/* end of STF || STF (store floating-point values)
**********************************************************/
/**********************************************************
* STI || STI (sotre integers)
* op STI_STI(0b11[2]:0b00001[5]:src2[3]:0b000[3]:src1[3]:dst1[8]:dst2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 24316 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24319 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24323 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24326 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24331 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24335 "isa_tms320.tcc"
> *DecodeOpSTI_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTI_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 24341 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 24345 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 24353 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24356 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24360 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24363 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24368 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24372 "isa_tms320.tcc"
> *DecodeOpSTI_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSTI_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 24378 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 24382 "isa_tms320.tcc"
	>(code, addr);
}

/* end of STI || STI (sotre integers)
**********************************************************/
/**********************************************************
* SUBF3 || STF (subtract floating-point value and store floating-point value)
* op SUBF3_STF(0b11[2]:0b10101[5]:dst1[3]:src1[3]:src3[3]:dst2[8]:src2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 24396 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24399 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24403 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24406 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24411 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24415 "isa_tms320.tcc"
> *DecodeOpSUBF3_STF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF3_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 24421 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 24425 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 24433 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24436 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24440 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24443 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24448 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24452 "isa_tms320.tcc"
> *DecodeOpSUBF3_STF_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBF3_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 24458 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 24462 "isa_tms320.tcc"
	>(code, addr);
}

/* end of SUBF3 || STF (subtract floating-point value and store floating-point value)
**********************************************************/
/**********************************************************
* SUBI3 || STI (subtract integer and store integer)
* op SUBI3_STI(0b11[2]:0b10110[5]:dst1[3]:src1[3]:src3[3]:dst2[8]:src2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 24476 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24479 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24483 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24486 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24491 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24495 "isa_tms320.tcc"
> *DecodeOpSUBI3_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 24501 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 24505 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 24513 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24516 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24520 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24523 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24528 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24532 "isa_tms320.tcc"
> *DecodeOpSUBI3_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpSUBI3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 24538 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 24542 "isa_tms320.tcc"
	>(code, addr);
}

/* end of SUBI3 || STI (subtract integer and store integer)
**********************************************************/
/**********************************************************
* XOR3 || STI (bitwise-exclusive OR values and store integer)
* op XOR3_STI(0b11[2]:0b10111[5]:dst1[3]:src1[3]:src3[3]:dst2[8]:src2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 24556 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24559 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24563 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24566 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24571 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24575 "isa_tms320.tcc"
> *DecodeOpXOR3_STI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 24581 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 24585 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 24593 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24596 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24600 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24603 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24608 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24612 "isa_tms320.tcc"
> *DecodeOpXOR3_STI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpXOR3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 24618 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 24622 "isa_tms320.tcc"
	>(code, addr);
}

/* end of XOR3 || STI (bitwise-exclusive OR values and store integer)
**********************************************************/
/**********************************************************
* LDF || LDF (load floating-point value)
* op LDF_LDF(0b11[2]:0b00010[5]:dst1[3]:dst2[3]:0b000[3]:src1[8]:src2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 24636 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24639 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24643 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24646 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24651 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24655 "isa_tms320.tcc"
> *DecodeOpLDF_LDF(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDF_LDF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 24661 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 24665 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 24673 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24676 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24680 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24683 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24688 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24692 "isa_tms320.tcc"
> *DecodeOpLDF_LDF_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDF_LDF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 24698 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 24702 "isa_tms320.tcc"
	>(code, addr);
}

/* end of LDF || LDF (load floating-point value)
**********************************************************/
/**********************************************************
* LDI || LDI (load integer)
* op LDI_LDI(0b11[2]:0b00011[5]:dst1[3]:dst2[3]:0b000[3]:src1[8]:src2[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 24716 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24719 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24723 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24726 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24731 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24735 "isa_tms320.tcc"
> *DecodeOpLDI_LDI(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDI_LDI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 24741 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 24745 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 24753 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24756 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24760 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24763 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24768 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24772 "isa_tms320.tcc"
> *DecodeOpLDI_LDI_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpLDI_LDI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 24778 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 24782 "isa_tms320.tcc"
	>(code, addr);
}

/* end of LDI || LDI (load integer)
**********************************************************/
/**********************************************************
* MPYF3 || ADDF3 (multiply and add floating-point value)
* op MPYF3_ADDF3(0b10[2]:0b0000[4]:p[2]:d1[1]:d2[1]:src1[3]:src2[3]:src3[8]:src4[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 24796 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24799 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24803 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24806 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24811 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24815 "isa_tms320.tcc"
> *DecodeOpMPYF3_ADDF3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_ADDF3<
#line 45 "isa/tms320.isa"
	CONFIG
#line 24821 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 24825 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 24833 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24836 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24840 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24843 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24848 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24852 "isa_tms320.tcc"
> *DecodeOpMPYF3_ADDF3_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_ADDF3_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 24858 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 24862 "isa_tms320.tcc"
	>(code, addr);
}

/* end of MPYF3 || ADDF3 (multiply and add floating-point value)
**********************************************************/
/**********************************************************
* MPYF3 || SUBF3 (multiply and subtract floating-point value)
* op MPYF3_SUBF3(0b10[2]:0b0001[4]:p[2]:d1[1]:d2[1]:src1[3]:src2[3]:src3[8]:src4[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 24876 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24879 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24883 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24886 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24891 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24895 "isa_tms320.tcc"
> *DecodeOpMPYF3_SUBF3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_SUBF3<
#line 45 "isa/tms320.isa"
	CONFIG
#line 24901 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 24905 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 24913 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24916 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24920 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24923 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24928 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24932 "isa_tms320.tcc"
> *DecodeOpMPYF3_SUBF3_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYF3_SUBF3_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 24938 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 24942 "isa_tms320.tcc"
	>(code, addr);
}

/* end of MPYF3 || SUBF3 (multiply and subtract floating-point value)
**********************************************************/
/**********************************************************
* MPYI3 || ADDI3 (multiply and add integer)
* op MPYI3_ADDI3(0b10[2]:0b0010[4]:p[2]:d1[1]:d2[1]:src1[3]:src2[3]:src3[8]:src4[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 24956 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24959 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 24963 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 24966 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 24971 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 24975 "isa_tms320.tcc"
> *DecodeOpMPYI3_ADDI3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_ADDI3<
#line 45 "isa/tms320.isa"
	CONFIG
#line 24981 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 24985 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 24993 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 24996 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25000 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25003 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25008 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25012 "isa_tms320.tcc"
> *DecodeOpMPYI3_ADDI3_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_ADDI3_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 25018 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 25022 "isa_tms320.tcc"
	>(code, addr);
}

/* end of MPYI3 || ADDI3 (multiply and add integer)
**********************************************************/
/**********************************************************
* MPYI3 || SUBI3 (multiply and subtract integer)
* op MPYI3_SUBI3(0b10[2]:0b0011[4]:p[2]:d1[1]:d2[1]:src1[3]:src2[3]:src3[8]:src4[8])
*/

template <
#line 45 "isa/tms320.isa"
class
#line 25036 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25039 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25043 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25046 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25051 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25055 "isa_tms320.tcc"
> *DecodeOpMPYI3_SUBI3(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_SUBI3<
#line 45 "isa/tms320.isa"
	CONFIG
#line 25061 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 25065 "isa_tms320.tcc"
	>(code, addr);
}


template <
#line 45 "isa/tms320.isa"
class
#line 25073 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25076 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25080 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25083 "isa_tms320.tcc"
>
static Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25088 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25092 "isa_tms320.tcc"
> *DecodeOpMPYI3_SUBI3_ext(CodeType code, typename CONFIG::address_t addr)
{
	return new OpMPYI3_SUBI3_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 25098 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 25102 "isa_tms320.tcc"
	>(code, addr);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25109 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25112 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25116 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25119 "isa_tms320.tcc"
>
OpLDE_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 25124 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25128 "isa_tms320.tcc"
>::OpLDE_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25132 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25136 "isa_tms320.tcc"
>(code, addr, "LDE_reg")
{
	dst = ((code >> 16) & 0x7);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25146 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25149 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25153 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25156 "isa_tms320.tcc"
>
OpLDE_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25161 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25165 "isa_tms320.tcc"
>::OpLDE_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25169 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25173 "isa_tms320.tcc"
>(code, addr, "LDE_dir")
{
	dst = ((code >> 16) & 0x7);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25183 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25186 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25190 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25193 "isa_tms320.tcc"
>
OpLDE_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25198 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25202 "isa_tms320.tcc"
>::OpLDE_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25206 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25210 "isa_tms320.tcc"
>(code, addr, "LDE_indir")
{
	dst = ((code >> 16) & 0x7);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25222 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25225 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25229 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25232 "isa_tms320.tcc"
>
OpLDE_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 25237 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25241 "isa_tms320.tcc"
>::OpLDE_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25245 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25249 "isa_tms320.tcc"
>(code, addr, "LDE_imm")
{
	dst = ((code >> 16) & 0x7);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25259 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25262 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25266 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25269 "isa_tms320.tcc"
>
OpLDF_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 25274 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25278 "isa_tms320.tcc"
>::OpLDF_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25282 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25286 "isa_tms320.tcc"
>(code, addr, "LDF_reg")
{
	dst = ((code >> 16) & 0x7);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25296 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25299 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25303 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25306 "isa_tms320.tcc"
>
OpLDF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25311 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25315 "isa_tms320.tcc"
>::OpLDF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25319 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25323 "isa_tms320.tcc"
>(code, addr, "LDF_dir")
{
	dst = ((code >> 16) & 0x7);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25333 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25336 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25340 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25343 "isa_tms320.tcc"
>
OpLDF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25348 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25352 "isa_tms320.tcc"
>::OpLDF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25356 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25360 "isa_tms320.tcc"
>(code, addr, "LDF_indir")
{
	dst = ((code >> 16) & 0x7);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25372 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25375 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25379 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25382 "isa_tms320.tcc"
>
OpLDF_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 25387 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25391 "isa_tms320.tcc"
>::OpLDF_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25395 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25399 "isa_tms320.tcc"
>(code, addr, "LDF_imm")
{
	dst = ((code >> 16) & 0x7);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25409 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25412 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25416 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25419 "isa_tms320.tcc"
>
OpLDFcond_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 25424 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25428 "isa_tms320.tcc"
>::OpLDFcond_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25432 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25436 "isa_tms320.tcc"
>(code, addr, "LDFcond_reg")
{
	cond = ((code >> 23) & 0x1f);
	dst = ((code >> 16) & 0x7);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25447 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25450 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25454 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25457 "isa_tms320.tcc"
>
OpLDFcond_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25462 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25466 "isa_tms320.tcc"
>::OpLDFcond_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25470 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25474 "isa_tms320.tcc"
>(code, addr, "LDFcond_dir")
{
	cond = ((code >> 23) & 0x1f);
	dst = ((code >> 16) & 0x7);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25485 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25488 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25492 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25495 "isa_tms320.tcc"
>
OpLDFcond_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25500 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25504 "isa_tms320.tcc"
>::OpLDFcond_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25508 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25512 "isa_tms320.tcc"
>(code, addr, "LDFcond_indir")
{
	cond = ((code >> 23) & 0x1f);
	dst = ((code >> 16) & 0x7);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25525 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25528 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25532 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25535 "isa_tms320.tcc"
>
OpLDFcond_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 25540 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25544 "isa_tms320.tcc"
>::OpLDFcond_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25548 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25552 "isa_tms320.tcc"
>(code, addr, "LDFcond_imm")
{
	cond = ((code >> 23) & 0x1f);
	dst = ((code >> 16) & 0x7);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25563 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25566 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25570 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25573 "isa_tms320.tcc"
>
OpLDI_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 25578 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25582 "isa_tms320.tcc"
>::OpLDI_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25586 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25590 "isa_tms320.tcc"
>(code, addr, "LDI_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25600 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25603 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25607 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25610 "isa_tms320.tcc"
>
OpLDI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25615 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25619 "isa_tms320.tcc"
>::OpLDI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25623 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25627 "isa_tms320.tcc"
>(code, addr, "LDI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25637 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25640 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25644 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25647 "isa_tms320.tcc"
>
OpLDI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25652 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25656 "isa_tms320.tcc"
>::OpLDI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25660 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25664 "isa_tms320.tcc"
>(code, addr, "LDI_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25676 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25679 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25683 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25686 "isa_tms320.tcc"
>
OpLDI_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 25691 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25695 "isa_tms320.tcc"
>::OpLDI_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25699 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25703 "isa_tms320.tcc"
>(code, addr, "LDI_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25713 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25716 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25720 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25723 "isa_tms320.tcc"
>
OpLDIcond_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 25728 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25732 "isa_tms320.tcc"
>::OpLDIcond_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25736 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25740 "isa_tms320.tcc"
>(code, addr, "LDIcond_reg")
{
	cond = ((code >> 23) & 0x1f);
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25751 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25754 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25758 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25761 "isa_tms320.tcc"
>
OpLDIcond_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25766 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25770 "isa_tms320.tcc"
>::OpLDIcond_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25774 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25778 "isa_tms320.tcc"
>(code, addr, "LDIcond_dir")
{
	cond = ((code >> 23) & 0x1f);
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25789 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25792 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25796 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25799 "isa_tms320.tcc"
>
OpLDIcond_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25804 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25808 "isa_tms320.tcc"
>::OpLDIcond_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25812 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25816 "isa_tms320.tcc"
>(code, addr, "LDIcond_indir")
{
	cond = ((code >> 23) & 0x1f);
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25829 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25832 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25836 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25839 "isa_tms320.tcc"
>
OpLDIcond_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 25844 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25848 "isa_tms320.tcc"
>::OpLDIcond_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25852 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25856 "isa_tms320.tcc"
>(code, addr, "LDIcond_imm")
{
	cond = ((code >> 23) & 0x1f);
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25867 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25870 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25874 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25877 "isa_tms320.tcc"
>
OpLDM_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 25882 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25886 "isa_tms320.tcc"
>::OpLDM_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25890 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25894 "isa_tms320.tcc"
>(code, addr, "LDM_reg")
{
	dst = ((code >> 16) & 0x7);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25904 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25907 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25911 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25914 "isa_tms320.tcc"
>
OpLDM_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25919 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25923 "isa_tms320.tcc"
>::OpLDM_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25927 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25931 "isa_tms320.tcc"
>(code, addr, "LDM_dir")
{
	dst = ((code >> 16) & 0x7);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25941 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25944 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25948 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25951 "isa_tms320.tcc"
>
OpLDM_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 25956 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25960 "isa_tms320.tcc"
>::OpLDM_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 25964 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25968 "isa_tms320.tcc"
>(code, addr, "LDM_indir")
{
	dst = ((code >> 16) & 0x7);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 25980 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 25983 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 25987 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 25990 "isa_tms320.tcc"
>
OpLDM_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 25995 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 25999 "isa_tms320.tcc"
>::OpLDM_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26003 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26007 "isa_tms320.tcc"
>(code, addr, "LDM_imm")
{
	dst = ((code >> 16) & 0x7);
	imm = ((code >> 0) & 0xfff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26017 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26020 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26024 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26027 "isa_tms320.tcc"
>
OpLDP<
#line 45 "isa/tms320.isa"
CONFIG
#line 26032 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26036 "isa_tms320.tcc"
>::OpLDP(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26040 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26044 "isa_tms320.tcc"
>(code, addr, "LDP")
{
	src = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26053 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26056 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26060 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26063 "isa_tms320.tcc"
>
OpPOP<
#line 45 "isa/tms320.isa"
CONFIG
#line 26068 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26072 "isa_tms320.tcc"
>::OpPOP(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26076 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26080 "isa_tms320.tcc"
>(code, addr, "POP")
{
	dst = ((code >> 16) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26089 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26092 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26096 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26099 "isa_tms320.tcc"
>
OpPOPF<
#line 45 "isa/tms320.isa"
CONFIG
#line 26104 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26108 "isa_tms320.tcc"
>::OpPOPF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26112 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26116 "isa_tms320.tcc"
>(code, addr, "POPF")
{
	dst = ((code >> 16) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26125 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26128 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26132 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26135 "isa_tms320.tcc"
>
OpPUSH<
#line 45 "isa/tms320.isa"
CONFIG
#line 26140 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26144 "isa_tms320.tcc"
>::OpPUSH(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26148 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26152 "isa_tms320.tcc"
>(code, addr, "PUSH")
{
	src = ((code >> 16) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26161 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26164 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26168 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26171 "isa_tms320.tcc"
>
OpPUSHF<
#line 45 "isa/tms320.isa"
CONFIG
#line 26176 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26180 "isa_tms320.tcc"
>::OpPUSHF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26184 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26188 "isa_tms320.tcc"
>(code, addr, "PUSHF")
{
	src = ((code >> 16) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26197 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26200 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26204 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26207 "isa_tms320.tcc"
>
OpSTF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26212 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26216 "isa_tms320.tcc"
>::OpSTF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26220 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26224 "isa_tms320.tcc"
>(code, addr, "STF_dir")
{
	src = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26234 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26237 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26241 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26244 "isa_tms320.tcc"
>
OpSTF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26249 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26253 "isa_tms320.tcc"
>::OpSTF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26257 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26261 "isa_tms320.tcc"
>(code, addr, "STF_indir")
{
	src = ((code >> 16) & 0x7);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26273 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26276 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26280 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26283 "isa_tms320.tcc"
>
OpSTI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26288 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26292 "isa_tms320.tcc"
>::OpSTI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26296 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26300 "isa_tms320.tcc"
>(code, addr, "STI_dir")
{
	src = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26310 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26313 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26317 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26320 "isa_tms320.tcc"
>
OpSTI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26325 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26329 "isa_tms320.tcc"
>::OpSTI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26333 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26337 "isa_tms320.tcc"
>(code, addr, "STI_indir")
{
	src = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26349 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26352 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26356 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26359 "isa_tms320.tcc"
>
OpABSF_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 26364 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26368 "isa_tms320.tcc"
>::OpABSF_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26372 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26376 "isa_tms320.tcc"
>(code, addr, "ABSF_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26386 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26389 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26393 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26396 "isa_tms320.tcc"
>
OpABSF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26401 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26405 "isa_tms320.tcc"
>::OpABSF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26409 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26413 "isa_tms320.tcc"
>(code, addr, "ABSF_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26423 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26426 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26430 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26433 "isa_tms320.tcc"
>
OpABSF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26438 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26442 "isa_tms320.tcc"
>::OpABSF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26446 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26450 "isa_tms320.tcc"
>(code, addr, "ABSF_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26462 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26465 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26469 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26472 "isa_tms320.tcc"
>
OpABSF_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 26477 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26481 "isa_tms320.tcc"
>::OpABSF_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26485 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26489 "isa_tms320.tcc"
>(code, addr, "ABSF_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26499 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26502 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26506 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26509 "isa_tms320.tcc"
>
OpABSI_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 26514 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26518 "isa_tms320.tcc"
>::OpABSI_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26522 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26526 "isa_tms320.tcc"
>(code, addr, "ABSI_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26536 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26539 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26543 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26546 "isa_tms320.tcc"
>
OpABSI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26551 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26555 "isa_tms320.tcc"
>::OpABSI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26559 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26563 "isa_tms320.tcc"
>(code, addr, "ABSI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26573 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26576 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26580 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26583 "isa_tms320.tcc"
>
OpABSI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26588 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26592 "isa_tms320.tcc"
>::OpABSI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26596 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26600 "isa_tms320.tcc"
>(code, addr, "ABSI_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26612 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26615 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26619 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26622 "isa_tms320.tcc"
>
OpABSI_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 26627 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26631 "isa_tms320.tcc"
>::OpABSI_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26635 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26639 "isa_tms320.tcc"
>(code, addr, "ABSI_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26649 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26652 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26656 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26659 "isa_tms320.tcc"
>
OpADDC_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 26664 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26668 "isa_tms320.tcc"
>::OpADDC_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26672 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26676 "isa_tms320.tcc"
>(code, addr, "ADDC_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26686 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26689 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26693 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26696 "isa_tms320.tcc"
>
OpADDC_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26701 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26705 "isa_tms320.tcc"
>::OpADDC_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26709 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26713 "isa_tms320.tcc"
>(code, addr, "ADDC_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26723 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26726 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26730 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26733 "isa_tms320.tcc"
>
OpADDC_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26738 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26742 "isa_tms320.tcc"
>::OpADDC_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26746 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26750 "isa_tms320.tcc"
>(code, addr, "ADDC_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26762 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26765 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26769 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26772 "isa_tms320.tcc"
>
OpADDC_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 26777 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26781 "isa_tms320.tcc"
>::OpADDC_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26785 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26789 "isa_tms320.tcc"
>(code, addr, "ADDC_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26799 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26802 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26806 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26809 "isa_tms320.tcc"
>
OpADDF_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 26814 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26818 "isa_tms320.tcc"
>::OpADDF_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26822 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26826 "isa_tms320.tcc"
>(code, addr, "ADDF_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26836 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26839 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26843 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26846 "isa_tms320.tcc"
>
OpADDF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26851 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26855 "isa_tms320.tcc"
>::OpADDF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26859 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26863 "isa_tms320.tcc"
>(code, addr, "ADDF_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26873 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26876 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26880 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26883 "isa_tms320.tcc"
>
OpADDF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 26888 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26892 "isa_tms320.tcc"
>::OpADDF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26896 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26900 "isa_tms320.tcc"
>(code, addr, "ADDF_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26912 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26915 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26919 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26922 "isa_tms320.tcc"
>
OpADDF_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 26927 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26931 "isa_tms320.tcc"
>::OpADDF_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26935 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26939 "isa_tms320.tcc"
>(code, addr, "ADDF_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26949 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26952 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26956 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26959 "isa_tms320.tcc"
>
OpADDI_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 26964 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26968 "isa_tms320.tcc"
>::OpADDI_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 26972 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 26976 "isa_tms320.tcc"
>(code, addr, "ADDI_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 26986 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 26989 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 26993 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 26996 "isa_tms320.tcc"
>
OpADDI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27001 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27005 "isa_tms320.tcc"
>::OpADDI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27009 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27013 "isa_tms320.tcc"
>(code, addr, "ADDI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27023 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27026 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27030 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27033 "isa_tms320.tcc"
>
OpADDI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27038 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27042 "isa_tms320.tcc"
>::OpADDI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27046 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27050 "isa_tms320.tcc"
>(code, addr, "ADDI_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27062 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27065 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27069 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27072 "isa_tms320.tcc"
>
OpADDI_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 27077 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27081 "isa_tms320.tcc"
>::OpADDI_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27085 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27089 "isa_tms320.tcc"
>(code, addr, "ADDI_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27099 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27102 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27106 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27109 "isa_tms320.tcc"
>
OpAND_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 27114 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27118 "isa_tms320.tcc"
>::OpAND_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27122 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27126 "isa_tms320.tcc"
>(code, addr, "AND_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27136 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27139 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27143 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27146 "isa_tms320.tcc"
>
OpAND_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27151 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27155 "isa_tms320.tcc"
>::OpAND_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27159 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27163 "isa_tms320.tcc"
>(code, addr, "AND_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27173 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27176 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27180 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27183 "isa_tms320.tcc"
>
OpAND_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27188 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27192 "isa_tms320.tcc"
>::OpAND_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27196 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27200 "isa_tms320.tcc"
>(code, addr, "AND_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27212 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27215 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27219 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27222 "isa_tms320.tcc"
>
OpAND_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 27227 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27231 "isa_tms320.tcc"
>::OpAND_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27235 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27239 "isa_tms320.tcc"
>(code, addr, "AND_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27249 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27252 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27256 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27259 "isa_tms320.tcc"
>
OpANDN_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 27264 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27268 "isa_tms320.tcc"
>::OpANDN_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27272 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27276 "isa_tms320.tcc"
>(code, addr, "ANDN_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27286 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27289 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27293 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27296 "isa_tms320.tcc"
>
OpANDN_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27301 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27305 "isa_tms320.tcc"
>::OpANDN_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27309 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27313 "isa_tms320.tcc"
>(code, addr, "ANDN_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27323 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27326 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27330 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27333 "isa_tms320.tcc"
>
OpANDN_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27338 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27342 "isa_tms320.tcc"
>::OpANDN_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27346 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27350 "isa_tms320.tcc"
>(code, addr, "ANDN_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27362 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27365 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27369 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27372 "isa_tms320.tcc"
>
OpANDN_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 27377 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27381 "isa_tms320.tcc"
>::OpANDN_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27385 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27389 "isa_tms320.tcc"
>(code, addr, "ANDN_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27399 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27402 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27406 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27409 "isa_tms320.tcc"
>
OpASH_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 27414 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27418 "isa_tms320.tcc"
>::OpASH_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27422 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27426 "isa_tms320.tcc"
>(code, addr, "ASH_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27436 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27439 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27443 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27446 "isa_tms320.tcc"
>
OpASH_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27451 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27455 "isa_tms320.tcc"
>::OpASH_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27459 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27463 "isa_tms320.tcc"
>(code, addr, "ASH_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27473 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27476 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27480 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27483 "isa_tms320.tcc"
>
OpASH_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27488 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27492 "isa_tms320.tcc"
>::OpASH_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27496 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27500 "isa_tms320.tcc"
>(code, addr, "ASH_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27512 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27515 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27519 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27522 "isa_tms320.tcc"
>
OpASH_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 27527 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27531 "isa_tms320.tcc"
>::OpASH_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27535 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27539 "isa_tms320.tcc"
>(code, addr, "ASH_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27549 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27552 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27556 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27559 "isa_tms320.tcc"
>
OpCMPF_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 27564 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27568 "isa_tms320.tcc"
>::OpCMPF_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27572 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27576 "isa_tms320.tcc"
>(code, addr, "CMPF_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27586 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27589 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27593 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27596 "isa_tms320.tcc"
>
OpCMPF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27601 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27605 "isa_tms320.tcc"
>::OpCMPF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27609 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27613 "isa_tms320.tcc"
>(code, addr, "CMPF_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27623 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27626 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27630 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27633 "isa_tms320.tcc"
>
OpCMPF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27638 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27642 "isa_tms320.tcc"
>::OpCMPF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27646 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27650 "isa_tms320.tcc"
>(code, addr, "CMPF_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27662 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27665 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27669 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27672 "isa_tms320.tcc"
>
OpCMPF_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 27677 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27681 "isa_tms320.tcc"
>::OpCMPF_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27685 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27689 "isa_tms320.tcc"
>(code, addr, "CMPF_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27699 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27702 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27706 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27709 "isa_tms320.tcc"
>
OpCMPI_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 27714 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27718 "isa_tms320.tcc"
>::OpCMPI_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27722 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27726 "isa_tms320.tcc"
>(code, addr, "CMPI_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27736 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27739 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27743 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27746 "isa_tms320.tcc"
>
OpCMPI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27751 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27755 "isa_tms320.tcc"
>::OpCMPI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27759 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27763 "isa_tms320.tcc"
>(code, addr, "CMPI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27773 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27776 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27780 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27783 "isa_tms320.tcc"
>
OpCMPI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27788 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27792 "isa_tms320.tcc"
>::OpCMPI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27796 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27800 "isa_tms320.tcc"
>(code, addr, "CMPI_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27812 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27815 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27819 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27822 "isa_tms320.tcc"
>
OpCMPI_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 27827 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27831 "isa_tms320.tcc"
>::OpCMPI_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27835 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27839 "isa_tms320.tcc"
>(code, addr, "CMPI_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27849 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27852 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27856 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27859 "isa_tms320.tcc"
>
OpFIX_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 27864 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27868 "isa_tms320.tcc"
>::OpFIX_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27872 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27876 "isa_tms320.tcc"
>(code, addr, "FIX_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27886 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27889 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27893 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27896 "isa_tms320.tcc"
>
OpFIX_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27901 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27905 "isa_tms320.tcc"
>::OpFIX_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27909 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27913 "isa_tms320.tcc"
>(code, addr, "FIX_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27923 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27926 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27930 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27933 "isa_tms320.tcc"
>
OpFIX_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 27938 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27942 "isa_tms320.tcc"
>::OpFIX_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27946 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27950 "isa_tms320.tcc"
>(code, addr, "FIX_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27962 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 27965 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 27969 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 27972 "isa_tms320.tcc"
>
OpFIX_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 27977 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27981 "isa_tms320.tcc"
>::OpFIX_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 27985 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 27989 "isa_tms320.tcc"
>(code, addr, "FIX_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 27999 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28002 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28006 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28009 "isa_tms320.tcc"
>
OpFLOAT_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 28014 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28018 "isa_tms320.tcc"
>::OpFLOAT_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28022 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28026 "isa_tms320.tcc"
>(code, addr, "FLOAT_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28036 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28039 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28043 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28046 "isa_tms320.tcc"
>
OpFLOAT_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28051 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28055 "isa_tms320.tcc"
>::OpFLOAT_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28059 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28063 "isa_tms320.tcc"
>(code, addr, "FLOAT_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28073 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28076 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28080 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28083 "isa_tms320.tcc"
>
OpFLOAT_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28088 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28092 "isa_tms320.tcc"
>::OpFLOAT_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28096 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28100 "isa_tms320.tcc"
>(code, addr, "FLOAT_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28112 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28115 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28119 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28122 "isa_tms320.tcc"
>
OpFLOAT_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 28127 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28131 "isa_tms320.tcc"
>::OpFLOAT_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28135 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28139 "isa_tms320.tcc"
>(code, addr, "FLOAT_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28149 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28152 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28156 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28159 "isa_tms320.tcc"
>
OpLSH_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 28164 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28168 "isa_tms320.tcc"
>::OpLSH_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28172 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28176 "isa_tms320.tcc"
>(code, addr, "LSH_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28186 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28189 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28193 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28196 "isa_tms320.tcc"
>
OpLSH_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28201 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28205 "isa_tms320.tcc"
>::OpLSH_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28209 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28213 "isa_tms320.tcc"
>(code, addr, "LSH_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28223 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28226 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28230 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28233 "isa_tms320.tcc"
>
OpLSH_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28238 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28242 "isa_tms320.tcc"
>::OpLSH_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28246 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28250 "isa_tms320.tcc"
>(code, addr, "LSH_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28262 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28265 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28269 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28272 "isa_tms320.tcc"
>
OpLSH_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 28277 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28281 "isa_tms320.tcc"
>::OpLSH_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28285 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28289 "isa_tms320.tcc"
>(code, addr, "LSH_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28299 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28302 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28306 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28309 "isa_tms320.tcc"
>
OpMPYF_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 28314 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28318 "isa_tms320.tcc"
>::OpMPYF_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28322 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28326 "isa_tms320.tcc"
>(code, addr, "MPYF_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28336 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28339 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28343 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28346 "isa_tms320.tcc"
>
OpMPYF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28351 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28355 "isa_tms320.tcc"
>::OpMPYF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28359 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28363 "isa_tms320.tcc"
>(code, addr, "MPYF_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28373 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28376 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28380 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28383 "isa_tms320.tcc"
>
OpMPYF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28388 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28392 "isa_tms320.tcc"
>::OpMPYF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28396 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28400 "isa_tms320.tcc"
>(code, addr, "MPYF_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28412 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28415 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28419 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28422 "isa_tms320.tcc"
>
OpMPYF_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 28427 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28431 "isa_tms320.tcc"
>::OpMPYF_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28435 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28439 "isa_tms320.tcc"
>(code, addr, "MPYF_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28449 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28452 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28456 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28459 "isa_tms320.tcc"
>
OpMPYI_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 28464 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28468 "isa_tms320.tcc"
>::OpMPYI_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28472 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28476 "isa_tms320.tcc"
>(code, addr, "MPYI_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28486 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28489 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28493 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28496 "isa_tms320.tcc"
>
OpMPYI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28501 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28505 "isa_tms320.tcc"
>::OpMPYI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28509 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28513 "isa_tms320.tcc"
>(code, addr, "MPYI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28523 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28526 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28530 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28533 "isa_tms320.tcc"
>
OpMPYI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28538 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28542 "isa_tms320.tcc"
>::OpMPYI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28546 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28550 "isa_tms320.tcc"
>(code, addr, "MPYI_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28562 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28565 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28569 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28572 "isa_tms320.tcc"
>
OpMPYI_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 28577 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28581 "isa_tms320.tcc"
>::OpMPYI_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28585 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28589 "isa_tms320.tcc"
>(code, addr, "MPYI_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28599 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28602 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28606 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28609 "isa_tms320.tcc"
>
OpNEGB_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 28614 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28618 "isa_tms320.tcc"
>::OpNEGB_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28622 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28626 "isa_tms320.tcc"
>(code, addr, "NEGB_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28636 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28639 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28643 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28646 "isa_tms320.tcc"
>
OpNEGB_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28651 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28655 "isa_tms320.tcc"
>::OpNEGB_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28659 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28663 "isa_tms320.tcc"
>(code, addr, "NEGB_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28673 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28676 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28680 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28683 "isa_tms320.tcc"
>
OpNEGB_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28688 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28692 "isa_tms320.tcc"
>::OpNEGB_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28696 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28700 "isa_tms320.tcc"
>(code, addr, "NEGB_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28712 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28715 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28719 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28722 "isa_tms320.tcc"
>
OpNEGB_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 28727 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28731 "isa_tms320.tcc"
>::OpNEGB_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28735 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28739 "isa_tms320.tcc"
>(code, addr, "NEGB_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28749 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28752 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28756 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28759 "isa_tms320.tcc"
>
OpNEGF_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 28764 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28768 "isa_tms320.tcc"
>::OpNEGF_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28772 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28776 "isa_tms320.tcc"
>(code, addr, "NEGF_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28786 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28789 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28793 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28796 "isa_tms320.tcc"
>
OpNEGF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28801 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28805 "isa_tms320.tcc"
>::OpNEGF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28809 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28813 "isa_tms320.tcc"
>(code, addr, "NEGF_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28823 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28826 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28830 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28833 "isa_tms320.tcc"
>
OpNEGF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28838 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28842 "isa_tms320.tcc"
>::OpNEGF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28846 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28850 "isa_tms320.tcc"
>(code, addr, "NEGF_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28862 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28865 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28869 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28872 "isa_tms320.tcc"
>
OpNEGF_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 28877 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28881 "isa_tms320.tcc"
>::OpNEGF_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28885 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28889 "isa_tms320.tcc"
>(code, addr, "NEGF_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28899 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28902 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28906 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28909 "isa_tms320.tcc"
>
OpNEGI_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 28914 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28918 "isa_tms320.tcc"
>::OpNEGI_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28922 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28926 "isa_tms320.tcc"
>(code, addr, "NEGI_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28936 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28939 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28943 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28946 "isa_tms320.tcc"
>
OpNEGI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28951 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28955 "isa_tms320.tcc"
>::OpNEGI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28959 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28963 "isa_tms320.tcc"
>(code, addr, "NEGI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 28973 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 28976 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 28980 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 28983 "isa_tms320.tcc"
>
OpNEGI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 28988 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 28992 "isa_tms320.tcc"
>::OpNEGI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 28996 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29000 "isa_tms320.tcc"
>(code, addr, "NEGI_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29012 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29015 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29019 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29022 "isa_tms320.tcc"
>
OpNEGI_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 29027 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29031 "isa_tms320.tcc"
>::OpNEGI_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29035 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29039 "isa_tms320.tcc"
>(code, addr, "NEGI_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29049 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29052 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29056 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29059 "isa_tms320.tcc"
>
OpNORM_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29064 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29068 "isa_tms320.tcc"
>::OpNORM_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29072 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29076 "isa_tms320.tcc"
>(code, addr, "NORM_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29086 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29089 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29093 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29096 "isa_tms320.tcc"
>
OpNORM_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29101 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29105 "isa_tms320.tcc"
>::OpNORM_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29109 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29113 "isa_tms320.tcc"
>(code, addr, "NORM_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29123 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29126 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29130 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29133 "isa_tms320.tcc"
>
OpNORM_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29138 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29142 "isa_tms320.tcc"
>::OpNORM_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29146 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29150 "isa_tms320.tcc"
>(code, addr, "NORM_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29162 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29165 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29169 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29172 "isa_tms320.tcc"
>
OpNORM_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 29177 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29181 "isa_tms320.tcc"
>::OpNORM_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29185 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29189 "isa_tms320.tcc"
>(code, addr, "NORM_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29199 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29202 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29206 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29209 "isa_tms320.tcc"
>
OpNOT_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29214 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29218 "isa_tms320.tcc"
>::OpNOT_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29222 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29226 "isa_tms320.tcc"
>(code, addr, "NOT_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29236 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29239 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29243 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29246 "isa_tms320.tcc"
>
OpNOT_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29251 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29255 "isa_tms320.tcc"
>::OpNOT_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29259 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29263 "isa_tms320.tcc"
>(code, addr, "NOT_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29273 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29276 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29280 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29283 "isa_tms320.tcc"
>
OpNOT_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29288 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29292 "isa_tms320.tcc"
>::OpNOT_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29296 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29300 "isa_tms320.tcc"
>(code, addr, "NOT_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29312 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29315 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29319 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29322 "isa_tms320.tcc"
>
OpNOT_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 29327 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29331 "isa_tms320.tcc"
>::OpNOT_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29335 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29339 "isa_tms320.tcc"
>(code, addr, "NOT_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29349 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29352 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29356 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29359 "isa_tms320.tcc"
>
OpOR_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29364 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29368 "isa_tms320.tcc"
>::OpOR_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29372 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29376 "isa_tms320.tcc"
>(code, addr, "OR_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29386 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29389 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29393 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29396 "isa_tms320.tcc"
>
OpOR_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29401 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29405 "isa_tms320.tcc"
>::OpOR_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29409 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29413 "isa_tms320.tcc"
>(code, addr, "OR_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29423 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29426 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29430 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29433 "isa_tms320.tcc"
>
OpOR_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29438 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29442 "isa_tms320.tcc"
>::OpOR_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29446 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29450 "isa_tms320.tcc"
>(code, addr, "OR_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29462 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29465 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29469 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29472 "isa_tms320.tcc"
>
OpOR_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 29477 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29481 "isa_tms320.tcc"
>::OpOR_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29485 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29489 "isa_tms320.tcc"
>(code, addr, "OR_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29499 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29502 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29506 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29509 "isa_tms320.tcc"
>
OpRND_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29514 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29518 "isa_tms320.tcc"
>::OpRND_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29522 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29526 "isa_tms320.tcc"
>(code, addr, "RND_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29536 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29539 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29543 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29546 "isa_tms320.tcc"
>
OpRND_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29551 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29555 "isa_tms320.tcc"
>::OpRND_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29559 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29563 "isa_tms320.tcc"
>(code, addr, "RND_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29573 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29576 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29580 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29583 "isa_tms320.tcc"
>
OpRND_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29588 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29592 "isa_tms320.tcc"
>::OpRND_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29596 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29600 "isa_tms320.tcc"
>(code, addr, "RND_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29612 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29615 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29619 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29622 "isa_tms320.tcc"
>
OpRND_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 29627 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29631 "isa_tms320.tcc"
>::OpRND_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29635 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29639 "isa_tms320.tcc"
>(code, addr, "RND_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29649 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29652 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29656 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29659 "isa_tms320.tcc"
>
OpROL<
#line 45 "isa/tms320.isa"
CONFIG
#line 29664 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29668 "isa_tms320.tcc"
>::OpROL(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29672 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29676 "isa_tms320.tcc"
>(code, addr, "ROL")
{
	dst = ((code >> 16) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29685 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29688 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29692 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29695 "isa_tms320.tcc"
>
OpROLC<
#line 45 "isa/tms320.isa"
CONFIG
#line 29700 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29704 "isa_tms320.tcc"
>::OpROLC(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29708 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29712 "isa_tms320.tcc"
>(code, addr, "ROLC")
{
	dst = ((code >> 16) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29721 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29724 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29728 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29731 "isa_tms320.tcc"
>
OpROR<
#line 45 "isa/tms320.isa"
CONFIG
#line 29736 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29740 "isa_tms320.tcc"
>::OpROR(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29744 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29748 "isa_tms320.tcc"
>(code, addr, "ROR")
{
	dst = ((code >> 16) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29757 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29760 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29764 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29767 "isa_tms320.tcc"
>
OpRORC<
#line 45 "isa/tms320.isa"
CONFIG
#line 29772 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29776 "isa_tms320.tcc"
>::OpRORC(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29780 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29784 "isa_tms320.tcc"
>(code, addr, "RORC")
{
	dst = ((code >> 16) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29793 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29796 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29800 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29803 "isa_tms320.tcc"
>
OpSUBB_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29808 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29812 "isa_tms320.tcc"
>::OpSUBB_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29816 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29820 "isa_tms320.tcc"
>(code, addr, "SUBB_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29830 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29833 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29837 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29840 "isa_tms320.tcc"
>
OpSUBB_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29845 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29849 "isa_tms320.tcc"
>::OpSUBB_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29853 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29857 "isa_tms320.tcc"
>(code, addr, "SUBB_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29867 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29870 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29874 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29877 "isa_tms320.tcc"
>
OpSUBB_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29882 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29886 "isa_tms320.tcc"
>::OpSUBB_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29890 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29894 "isa_tms320.tcc"
>(code, addr, "SUBB_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29906 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29909 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29913 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29916 "isa_tms320.tcc"
>
OpSUBB_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 29921 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29925 "isa_tms320.tcc"
>::OpSUBB_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29929 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29933 "isa_tms320.tcc"
>(code, addr, "SUBB_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29943 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29946 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29950 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29953 "isa_tms320.tcc"
>
OpSUBC_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 29958 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29962 "isa_tms320.tcc"
>::OpSUBC_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 29966 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29970 "isa_tms320.tcc"
>(code, addr, "SUBC_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 29980 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 29983 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 29987 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 29990 "isa_tms320.tcc"
>
OpSUBC_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 29995 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 29999 "isa_tms320.tcc"
>::OpSUBC_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30003 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30007 "isa_tms320.tcc"
>(code, addr, "SUBC_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30017 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30020 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30024 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30027 "isa_tms320.tcc"
>
OpSUBC_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30032 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30036 "isa_tms320.tcc"
>::OpSUBC_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30040 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30044 "isa_tms320.tcc"
>(code, addr, "SUBC_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30056 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30059 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30063 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30066 "isa_tms320.tcc"
>
OpSUBC_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 30071 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30075 "isa_tms320.tcc"
>::OpSUBC_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30079 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30083 "isa_tms320.tcc"
>(code, addr, "SUBC_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30093 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30096 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30100 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30103 "isa_tms320.tcc"
>
OpSUBF_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30108 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30112 "isa_tms320.tcc"
>::OpSUBF_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30116 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30120 "isa_tms320.tcc"
>(code, addr, "SUBF_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30130 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30133 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30137 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30140 "isa_tms320.tcc"
>
OpSUBF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30145 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30149 "isa_tms320.tcc"
>::OpSUBF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30153 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30157 "isa_tms320.tcc"
>(code, addr, "SUBF_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30167 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30170 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30174 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30177 "isa_tms320.tcc"
>
OpSUBF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30182 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30186 "isa_tms320.tcc"
>::OpSUBF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30190 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30194 "isa_tms320.tcc"
>(code, addr, "SUBF_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30206 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30209 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30213 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30216 "isa_tms320.tcc"
>
OpSUBF_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 30221 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30225 "isa_tms320.tcc"
>::OpSUBF_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30229 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30233 "isa_tms320.tcc"
>(code, addr, "SUBF_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30243 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30246 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30250 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30253 "isa_tms320.tcc"
>
OpSUBI_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30258 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30262 "isa_tms320.tcc"
>::OpSUBI_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30266 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30270 "isa_tms320.tcc"
>(code, addr, "SUBI_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30280 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30283 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30287 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30290 "isa_tms320.tcc"
>
OpSUBI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30295 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30299 "isa_tms320.tcc"
>::OpSUBI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30303 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30307 "isa_tms320.tcc"
>(code, addr, "SUBI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30317 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30320 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30324 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30327 "isa_tms320.tcc"
>
OpSUBI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30332 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30336 "isa_tms320.tcc"
>::OpSUBI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30340 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30344 "isa_tms320.tcc"
>(code, addr, "SUBI_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30356 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30359 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30363 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30366 "isa_tms320.tcc"
>
OpSUBI_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 30371 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30375 "isa_tms320.tcc"
>::OpSUBI_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30379 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30383 "isa_tms320.tcc"
>(code, addr, "SUBI_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30393 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30396 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30400 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30403 "isa_tms320.tcc"
>
OpSUBRB_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30408 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30412 "isa_tms320.tcc"
>::OpSUBRB_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30416 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30420 "isa_tms320.tcc"
>(code, addr, "SUBRB_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30430 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30433 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30437 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30440 "isa_tms320.tcc"
>
OpSUBRB_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30445 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30449 "isa_tms320.tcc"
>::OpSUBRB_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30453 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30457 "isa_tms320.tcc"
>(code, addr, "SUBRB_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30467 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30470 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30474 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30477 "isa_tms320.tcc"
>
OpSUBRB_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30482 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30486 "isa_tms320.tcc"
>::OpSUBRB_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30490 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30494 "isa_tms320.tcc"
>(code, addr, "SUBRB_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30506 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30509 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30513 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30516 "isa_tms320.tcc"
>
OpSUBRB_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 30521 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30525 "isa_tms320.tcc"
>::OpSUBRB_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30529 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30533 "isa_tms320.tcc"
>(code, addr, "SUBRB_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30543 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30546 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30550 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30553 "isa_tms320.tcc"
>
OpSUBRF_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30558 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30562 "isa_tms320.tcc"
>::OpSUBRF_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30566 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30570 "isa_tms320.tcc"
>(code, addr, "SUBRF_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30580 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30583 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30587 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30590 "isa_tms320.tcc"
>
OpSUBRF_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30595 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30599 "isa_tms320.tcc"
>::OpSUBRF_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30603 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30607 "isa_tms320.tcc"
>(code, addr, "SUBRF_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30617 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30620 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30624 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30627 "isa_tms320.tcc"
>
OpSUBRF_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30632 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30636 "isa_tms320.tcc"
>::OpSUBRF_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30640 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30644 "isa_tms320.tcc"
>(code, addr, "SUBRF_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30656 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30659 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30663 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30666 "isa_tms320.tcc"
>
OpSUBRF_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 30671 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30675 "isa_tms320.tcc"
>::OpSUBRF_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30679 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30683 "isa_tms320.tcc"
>(code, addr, "SUBRF_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30693 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30696 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30700 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30703 "isa_tms320.tcc"
>
OpSUBRI_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30708 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30712 "isa_tms320.tcc"
>::OpSUBRI_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30716 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30720 "isa_tms320.tcc"
>(code, addr, "SUBRI_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30730 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30733 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30737 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30740 "isa_tms320.tcc"
>
OpSUBRI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30745 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30749 "isa_tms320.tcc"
>::OpSUBRI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30753 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30757 "isa_tms320.tcc"
>(code, addr, "SUBRI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30767 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30770 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30774 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30777 "isa_tms320.tcc"
>
OpSUBRI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30782 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30786 "isa_tms320.tcc"
>::OpSUBRI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30790 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30794 "isa_tms320.tcc"
>(code, addr, "SUBRI_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30806 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30809 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30813 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30816 "isa_tms320.tcc"
>
OpSUBRI_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 30821 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30825 "isa_tms320.tcc"
>::OpSUBRI_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30829 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30833 "isa_tms320.tcc"
>(code, addr, "SUBRI_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30843 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30846 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30850 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30853 "isa_tms320.tcc"
>
OpTSTB_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 30858 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30862 "isa_tms320.tcc"
>::OpTSTB_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30866 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30870 "isa_tms320.tcc"
>(code, addr, "TSTB_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30880 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30883 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30887 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30890 "isa_tms320.tcc"
>
OpTSTB_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30895 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30899 "isa_tms320.tcc"
>::OpTSTB_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30903 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30907 "isa_tms320.tcc"
>(code, addr, "TSTB_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30917 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30920 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30924 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30927 "isa_tms320.tcc"
>
OpTSTB_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 30932 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30936 "isa_tms320.tcc"
>::OpTSTB_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30940 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30944 "isa_tms320.tcc"
>(code, addr, "TSTB_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30956 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30959 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 30963 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 30966 "isa_tms320.tcc"
>
OpTSTB_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 30971 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30975 "isa_tms320.tcc"
>::OpTSTB_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 30979 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 30983 "isa_tms320.tcc"
>(code, addr, "TSTB_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 30993 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 30996 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31000 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31003 "isa_tms320.tcc"
>
OpXOR_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31008 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31012 "isa_tms320.tcc"
>::OpXOR_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31016 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31020 "isa_tms320.tcc"
>(code, addr, "XOR_reg")
{
	dst = ((code >> 16) & 0x1f);
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31030 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31033 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31037 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31040 "isa_tms320.tcc"
>
OpXOR_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31045 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31049 "isa_tms320.tcc"
>::OpXOR_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31053 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31057 "isa_tms320.tcc"
>(code, addr, "XOR_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31067 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31070 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31074 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31077 "isa_tms320.tcc"
>
OpXOR_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31082 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31086 "isa_tms320.tcc"
>::OpXOR_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31090 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31094 "isa_tms320.tcc"
>(code, addr, "XOR_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31106 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31109 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31113 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31116 "isa_tms320.tcc"
>
OpXOR_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 31121 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31125 "isa_tms320.tcc"
>::OpXOR_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31129 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31133 "isa_tms320.tcc"
>(code, addr, "XOR_imm")
{
	dst = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31143 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31146 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31150 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31153 "isa_tms320.tcc"
>
OpADDC3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31158 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31162 "isa_tms320.tcc"
>::OpADDC3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31166 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31170 "isa_tms320.tcc"
>(code, addr, "ADDC3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31181 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31184 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31188 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31191 "isa_tms320.tcc"
>
OpADDC3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31196 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31200 "isa_tms320.tcc"
>::OpADDC3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31204 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31208 "isa_tms320.tcc"
>(code, addr, "ADDC3_indir_reg")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31220 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31223 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31227 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31230 "isa_tms320.tcc"
>
OpADDC3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31235 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31239 "isa_tms320.tcc"
>::OpADDC3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31243 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31247 "isa_tms320.tcc"
>(code, addr, "ADDC3_reg_indir")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31259 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31262 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31266 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31269 "isa_tms320.tcc"
>
OpADDC3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31274 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31278 "isa_tms320.tcc"
>::OpADDC3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31282 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31286 "isa_tms320.tcc"
>(code, addr, "ADDC3_indir_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31299 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31302 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31306 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31309 "isa_tms320.tcc"
>
OpADDF3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31314 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31318 "isa_tms320.tcc"
>::OpADDF3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31322 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31326 "isa_tms320.tcc"
>(code, addr, "ADDF3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31337 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31340 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31344 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31347 "isa_tms320.tcc"
>
OpADDF3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31352 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31356 "isa_tms320.tcc"
>::OpADDF3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31360 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31364 "isa_tms320.tcc"
>(code, addr, "ADDF3_indir_reg")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31376 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31379 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31383 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31386 "isa_tms320.tcc"
>
OpADDF3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31391 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31395 "isa_tms320.tcc"
>::OpADDF3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31399 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31403 "isa_tms320.tcc"
>(code, addr, "ADDF3_reg_indir")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31415 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31418 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31422 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31425 "isa_tms320.tcc"
>
OpADDF3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31430 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31434 "isa_tms320.tcc"
>::OpADDF3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31438 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31442 "isa_tms320.tcc"
>(code, addr, "ADDF3_indir_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31455 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31458 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31462 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31465 "isa_tms320.tcc"
>
OpADDI3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31470 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31474 "isa_tms320.tcc"
>::OpADDI3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31478 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31482 "isa_tms320.tcc"
>(code, addr, "ADDI3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31493 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31496 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31500 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31503 "isa_tms320.tcc"
>
OpADDI3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31508 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31512 "isa_tms320.tcc"
>::OpADDI3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31516 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31520 "isa_tms320.tcc"
>(code, addr, "ADDI3_indir_reg")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31532 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31535 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31539 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31542 "isa_tms320.tcc"
>
OpADDI3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31547 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31551 "isa_tms320.tcc"
>::OpADDI3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31555 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31559 "isa_tms320.tcc"
>(code, addr, "ADDI3_reg_indir")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31571 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31574 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31578 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31581 "isa_tms320.tcc"
>
OpADDI3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31586 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31590 "isa_tms320.tcc"
>::OpADDI3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31594 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31598 "isa_tms320.tcc"
>(code, addr, "ADDI3_indir_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31611 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31614 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31618 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31621 "isa_tms320.tcc"
>
OpAND3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31626 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31630 "isa_tms320.tcc"
>::OpAND3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31634 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31638 "isa_tms320.tcc"
>(code, addr, "AND3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31649 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31652 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31656 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31659 "isa_tms320.tcc"
>
OpAND3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31664 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31668 "isa_tms320.tcc"
>::OpAND3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31672 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31676 "isa_tms320.tcc"
>(code, addr, "AND3_indir_reg")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31688 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31691 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31695 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31698 "isa_tms320.tcc"
>
OpAND3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31703 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31707 "isa_tms320.tcc"
>::OpAND3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31711 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31715 "isa_tms320.tcc"
>(code, addr, "AND3_reg_indir")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31727 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31730 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31734 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31737 "isa_tms320.tcc"
>
OpAND3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31742 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31746 "isa_tms320.tcc"
>::OpAND3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31750 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31754 "isa_tms320.tcc"
>(code, addr, "AND3_indir_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31767 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31770 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31774 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31777 "isa_tms320.tcc"
>
OpANDN3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31782 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31786 "isa_tms320.tcc"
>::OpANDN3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31790 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31794 "isa_tms320.tcc"
>(code, addr, "ANDN3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31805 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31808 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31812 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31815 "isa_tms320.tcc"
>
OpANDN3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31820 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31824 "isa_tms320.tcc"
>::OpANDN3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31828 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31832 "isa_tms320.tcc"
>(code, addr, "ANDN3_indir_reg")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31844 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31847 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31851 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31854 "isa_tms320.tcc"
>
OpANDN3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31859 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31863 "isa_tms320.tcc"
>::OpANDN3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31867 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31871 "isa_tms320.tcc"
>(code, addr, "ANDN3_reg_indir")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31883 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31886 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31890 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31893 "isa_tms320.tcc"
>
OpANDN3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 31898 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31902 "isa_tms320.tcc"
>::OpANDN3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31906 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31910 "isa_tms320.tcc"
>(code, addr, "ANDN3_indir_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31923 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31926 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31930 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31933 "isa_tms320.tcc"
>
OpASH3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31938 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31942 "isa_tms320.tcc"
>::OpASH3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31946 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31950 "isa_tms320.tcc"
>(code, addr, "ASH3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 31961 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 31964 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 31968 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 31971 "isa_tms320.tcc"
>
OpASH3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 31976 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31980 "isa_tms320.tcc"
>::OpASH3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 31984 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 31988 "isa_tms320.tcc"
>(code, addr, "ASH3_indir_reg")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32000 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32003 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32007 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32010 "isa_tms320.tcc"
>
OpASH3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32015 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32019 "isa_tms320.tcc"
>::OpASH3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32023 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32027 "isa_tms320.tcc"
>(code, addr, "ASH3_reg_indir")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32039 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32042 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32046 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32049 "isa_tms320.tcc"
>
OpASH3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32054 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32058 "isa_tms320.tcc"
>::OpASH3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32062 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32066 "isa_tms320.tcc"
>(code, addr, "ASH3_indir_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32079 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32082 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32086 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32089 "isa_tms320.tcc"
>
OpCMPF3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 32094 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32098 "isa_tms320.tcc"
>::OpCMPF3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32102 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32106 "isa_tms320.tcc"
>(code, addr, "CMPF3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32117 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32120 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32124 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32127 "isa_tms320.tcc"
>
OpCMPF3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 32132 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32136 "isa_tms320.tcc"
>::OpCMPF3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32140 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32144 "isa_tms320.tcc"
>(code, addr, "CMPF3_indir_reg")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32156 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32159 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32163 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32166 "isa_tms320.tcc"
>
OpCMPF3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32171 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32175 "isa_tms320.tcc"
>::OpCMPF3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32179 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32183 "isa_tms320.tcc"
>(code, addr, "CMPF3_reg_indir")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32195 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32198 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32202 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32205 "isa_tms320.tcc"
>
OpCMPF3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32210 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32214 "isa_tms320.tcc"
>::OpCMPF3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32218 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32222 "isa_tms320.tcc"
>(code, addr, "CMPF3_indir_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32235 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32238 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32242 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32245 "isa_tms320.tcc"
>
OpCMPI3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 32250 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32254 "isa_tms320.tcc"
>::OpCMPI3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32258 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32262 "isa_tms320.tcc"
>(code, addr, "CMPI3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32273 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32276 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32280 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32283 "isa_tms320.tcc"
>
OpCMPI3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 32288 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32292 "isa_tms320.tcc"
>::OpCMPI3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32296 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32300 "isa_tms320.tcc"
>(code, addr, "CMPI3_indir_reg")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32312 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32315 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32319 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32322 "isa_tms320.tcc"
>
OpCMPI3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32327 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32331 "isa_tms320.tcc"
>::OpCMPI3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32335 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32339 "isa_tms320.tcc"
>(code, addr, "CMPI3_reg_indir")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32351 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32354 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32358 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32361 "isa_tms320.tcc"
>
OpCMPI3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32366 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32370 "isa_tms320.tcc"
>::OpCMPI3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32374 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32378 "isa_tms320.tcc"
>(code, addr, "CMPI3_indir_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32391 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32394 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32398 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32401 "isa_tms320.tcc"
>
OpLSH3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 32406 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32410 "isa_tms320.tcc"
>::OpLSH3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32414 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32418 "isa_tms320.tcc"
>(code, addr, "LSH3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32429 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32432 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32436 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32439 "isa_tms320.tcc"
>
OpLSH3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 32444 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32448 "isa_tms320.tcc"
>::OpLSH3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32452 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32456 "isa_tms320.tcc"
>(code, addr, "LSH3_indir_reg")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32468 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32471 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32475 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32478 "isa_tms320.tcc"
>
OpLSH3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32483 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32487 "isa_tms320.tcc"
>::OpLSH3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32491 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32495 "isa_tms320.tcc"
>(code, addr, "LSH3_reg_indir")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32507 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32510 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32514 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32517 "isa_tms320.tcc"
>
OpLSH3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32522 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32526 "isa_tms320.tcc"
>::OpLSH3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32530 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32534 "isa_tms320.tcc"
>(code, addr, "LSH3_indir_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32547 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32550 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32554 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32557 "isa_tms320.tcc"
>
OpMPYF3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 32562 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32566 "isa_tms320.tcc"
>::OpMPYF3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32570 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32574 "isa_tms320.tcc"
>(code, addr, "MPYF3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32585 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32588 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32592 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32595 "isa_tms320.tcc"
>
OpMPYF3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 32600 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32604 "isa_tms320.tcc"
>::OpMPYF3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32608 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32612 "isa_tms320.tcc"
>(code, addr, "MPYF3_indir_reg")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32624 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32627 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32631 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32634 "isa_tms320.tcc"
>
OpMPYF3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32639 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32643 "isa_tms320.tcc"
>::OpMPYF3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32647 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32651 "isa_tms320.tcc"
>(code, addr, "MPYF3_reg_indir")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32663 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32666 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32670 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32673 "isa_tms320.tcc"
>
OpMPYF3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32678 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32682 "isa_tms320.tcc"
>::OpMPYF3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32686 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32690 "isa_tms320.tcc"
>(code, addr, "MPYF3_indir_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32703 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32706 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32710 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32713 "isa_tms320.tcc"
>
OpMPYI3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 32718 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32722 "isa_tms320.tcc"
>::OpMPYI3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32726 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32730 "isa_tms320.tcc"
>(code, addr, "MPYI3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32741 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32744 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32748 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32751 "isa_tms320.tcc"
>
OpMPYI3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 32756 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32760 "isa_tms320.tcc"
>::OpMPYI3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32764 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32768 "isa_tms320.tcc"
>(code, addr, "MPYI3_indir_reg")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32780 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32783 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32787 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32790 "isa_tms320.tcc"
>
OpMPYI3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32795 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32799 "isa_tms320.tcc"
>::OpMPYI3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32803 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32807 "isa_tms320.tcc"
>(code, addr, "MPYI3_reg_indir")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32819 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32822 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32826 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32829 "isa_tms320.tcc"
>
OpMPYI3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32834 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32838 "isa_tms320.tcc"
>::OpMPYI3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32842 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32846 "isa_tms320.tcc"
>(code, addr, "MPYI3_indir_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32859 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32862 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32866 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32869 "isa_tms320.tcc"
>
OpOR3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 32874 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32878 "isa_tms320.tcc"
>::OpOR3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32882 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32886 "isa_tms320.tcc"
>(code, addr, "OR3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32897 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32900 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32904 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32907 "isa_tms320.tcc"
>
OpOR3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 32912 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32916 "isa_tms320.tcc"
>::OpOR3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32920 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32924 "isa_tms320.tcc"
>(code, addr, "OR3_indir_reg")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32936 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32939 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32943 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32946 "isa_tms320.tcc"
>
OpOR3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32951 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32955 "isa_tms320.tcc"
>::OpOR3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32959 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32963 "isa_tms320.tcc"
>(code, addr, "OR3_reg_indir")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 32975 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 32978 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 32982 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 32985 "isa_tms320.tcc"
>
OpOR3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 32990 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 32994 "isa_tms320.tcc"
>::OpOR3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 32998 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33002 "isa_tms320.tcc"
>(code, addr, "OR3_indir_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33015 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33018 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33022 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33025 "isa_tms320.tcc"
>
OpSUBB3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 33030 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33034 "isa_tms320.tcc"
>::OpSUBB3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33038 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33042 "isa_tms320.tcc"
>(code, addr, "SUBB3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33053 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33056 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33060 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33063 "isa_tms320.tcc"
>
OpSUBB3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 33068 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33072 "isa_tms320.tcc"
>::OpSUBB3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33076 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33080 "isa_tms320.tcc"
>(code, addr, "SUBB3_indir_reg")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33092 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33095 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33099 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33102 "isa_tms320.tcc"
>
OpSUBB3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 33107 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33111 "isa_tms320.tcc"
>::OpSUBB3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33115 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33119 "isa_tms320.tcc"
>(code, addr, "SUBB3_reg_indir")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33131 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33134 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33138 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33141 "isa_tms320.tcc"
>
OpSUBB3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 33146 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33150 "isa_tms320.tcc"
>::OpSUBB3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33154 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33158 "isa_tms320.tcc"
>(code, addr, "SUBB3_indir_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33171 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33174 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33178 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33181 "isa_tms320.tcc"
>
OpSUBF3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 33186 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33190 "isa_tms320.tcc"
>::OpSUBF3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33194 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33198 "isa_tms320.tcc"
>(code, addr, "SUBF3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33209 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33212 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33216 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33219 "isa_tms320.tcc"
>
OpSUBF3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 33224 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33228 "isa_tms320.tcc"
>::OpSUBF3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33232 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33236 "isa_tms320.tcc"
>(code, addr, "SUBF3_indir_reg")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33248 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33251 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33255 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33258 "isa_tms320.tcc"
>
OpSUBF3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 33263 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33267 "isa_tms320.tcc"
>::OpSUBF3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33271 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33275 "isa_tms320.tcc"
>(code, addr, "SUBF3_reg_indir")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33287 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33290 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33294 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33297 "isa_tms320.tcc"
>
OpSUBF3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 33302 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33306 "isa_tms320.tcc"
>::OpSUBF3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33310 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33314 "isa_tms320.tcc"
>(code, addr, "SUBF3_indir_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33327 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33330 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33334 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33337 "isa_tms320.tcc"
>
OpSUBI3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 33342 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33346 "isa_tms320.tcc"
>::OpSUBI3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33350 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33354 "isa_tms320.tcc"
>(code, addr, "SUBI3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33365 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33368 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33372 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33375 "isa_tms320.tcc"
>
OpSUBI3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 33380 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33384 "isa_tms320.tcc"
>::OpSUBI3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33388 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33392 "isa_tms320.tcc"
>(code, addr, "SUBI3_indir_reg")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33404 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33407 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33411 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33414 "isa_tms320.tcc"
>
OpSUBI3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 33419 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33423 "isa_tms320.tcc"
>::OpSUBI3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33427 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33431 "isa_tms320.tcc"
>(code, addr, "SUBI3_reg_indir")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33443 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33446 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33450 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33453 "isa_tms320.tcc"
>
OpSUBI3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 33458 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33462 "isa_tms320.tcc"
>::OpSUBI3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33466 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33470 "isa_tms320.tcc"
>(code, addr, "SUBI3_indir_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33483 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33486 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33490 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33493 "isa_tms320.tcc"
>
OpTSTB3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 33498 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33502 "isa_tms320.tcc"
>::OpTSTB3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33506 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33510 "isa_tms320.tcc"
>(code, addr, "TSTB3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33521 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33524 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33528 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33531 "isa_tms320.tcc"
>
OpTSTB3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 33536 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33540 "isa_tms320.tcc"
>::OpTSTB3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33544 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33548 "isa_tms320.tcc"
>(code, addr, "TSTB3_indir_reg")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33560 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33563 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33567 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33570 "isa_tms320.tcc"
>
OpTSTB3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 33575 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33579 "isa_tms320.tcc"
>::OpTSTB3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33583 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33587 "isa_tms320.tcc"
>(code, addr, "TSTB3_reg_indir")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33599 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33602 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33606 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33609 "isa_tms320.tcc"
>
OpTSTB3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 33614 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33618 "isa_tms320.tcc"
>::OpTSTB3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33622 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33626 "isa_tms320.tcc"
>(code, addr, "TSTB3_indir_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33639 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33642 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33646 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33649 "isa_tms320.tcc"
>
OpXOR3_reg_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 33654 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33658 "isa_tms320.tcc"
>::OpXOR3_reg_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33662 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33666 "isa_tms320.tcc"
>(code, addr, "XOR3_reg_reg")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33677 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33680 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33684 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33687 "isa_tms320.tcc"
>
OpXOR3_indir_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 33692 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33696 "isa_tms320.tcc"
>::OpXOR3_indir_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33700 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33704 "isa_tms320.tcc"
>(code, addr, "XOR3_indir_reg")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33716 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33719 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33723 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33726 "isa_tms320.tcc"
>
OpXOR3_reg_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 33731 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33735 "isa_tms320.tcc"
>::OpXOR3_reg_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33739 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33743 "isa_tms320.tcc"
>(code, addr, "XOR3_reg_indir")
{
	dst = ((code >> 16) & 0x1f);
	src1 = ((code >> 8) & 0x1f);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33755 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33758 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33762 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33765 "isa_tms320.tcc"
>
OpXOR3_indir_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 33770 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33774 "isa_tms320.tcc"
>::OpXOR3_indir_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33778 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33782 "isa_tms320.tcc"
>(code, addr, "XOR3_indir_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod1 = ((code >> 11) & 0x1f);
	ar1 = ((code >> 8) & 0x7);
	mod2 = ((code >> 3) & 0x1f);
	ar2 = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33795 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33798 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33802 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33805 "isa_tms320.tcc"
>
OpBcond_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 33810 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33814 "isa_tms320.tcc"
>::OpBcond_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33818 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33822 "isa_tms320.tcc"
>(code, addr, "Bcond_reg")
{
	cond = ((code >> 16) & 0x1f);
	reg = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33832 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33835 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33839 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33842 "isa_tms320.tcc"
>
OpBcond_disp<
#line 45 "isa/tms320.isa"
CONFIG
#line 33847 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33851 "isa_tms320.tcc"
>::OpBcond_disp(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33855 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33859 "isa_tms320.tcc"
>(code, addr, "Bcond_disp")
{
	cond = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33869 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33872 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33876 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33879 "isa_tms320.tcc"
>
OpBcondD_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 33884 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33888 "isa_tms320.tcc"
>::OpBcondD_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33892 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33896 "isa_tms320.tcc"
>(code, addr, "BcondD_reg")
{
	cond = ((code >> 16) & 0x1f);
	reg = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33906 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33909 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33913 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33916 "isa_tms320.tcc"
>
OpBcondD_disp<
#line 45 "isa/tms320.isa"
CONFIG
#line 33921 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33925 "isa_tms320.tcc"
>::OpBcondD_disp(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33929 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33933 "isa_tms320.tcc"
>(code, addr, "BcondD_disp")
{
	cond = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33943 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33946 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33950 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33953 "isa_tms320.tcc"
>
OpBR<
#line 45 "isa/tms320.isa"
CONFIG
#line 33958 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33962 "isa_tms320.tcc"
>::OpBR(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 33966 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33970 "isa_tms320.tcc"
>(code, addr, "BR")
{
	src = ((code >> 0) & 0xffffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 33979 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 33982 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 33986 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 33989 "isa_tms320.tcc"
>
OpBRD<
#line 45 "isa/tms320.isa"
CONFIG
#line 33994 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 33998 "isa_tms320.tcc"
>::OpBRD(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34002 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34006 "isa_tms320.tcc"
>(code, addr, "BRD")
{
	src = ((code >> 0) & 0xffffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 34015 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34018 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34022 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34025 "isa_tms320.tcc"
>
OpCALL<
#line 45 "isa/tms320.isa"
CONFIG
#line 34030 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34034 "isa_tms320.tcc"
>::OpCALL(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34038 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34042 "isa_tms320.tcc"
>(code, addr, "CALL")
{
	src = ((code >> 0) & 0xffffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 34051 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34054 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34058 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34061 "isa_tms320.tcc"
>
OpCALLcond_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 34066 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34070 "isa_tms320.tcc"
>::OpCALLcond_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34074 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34078 "isa_tms320.tcc"
>(code, addr, "CALLcond_reg")
{
	cond = ((code >> 16) & 0x1f);
	reg = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 34088 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34091 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34095 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34098 "isa_tms320.tcc"
>
OpCALLcond_disp<
#line 45 "isa/tms320.isa"
CONFIG
#line 34103 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34107 "isa_tms320.tcc"
>::OpCALLcond_disp(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34111 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34115 "isa_tms320.tcc"
>(code, addr, "CALLcond_disp")
{
	cond = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 34125 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34128 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34132 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34135 "isa_tms320.tcc"
>
OpDBcond_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 34140 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34144 "isa_tms320.tcc"
>::OpDBcond_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34148 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34152 "isa_tms320.tcc"
>(code, addr, "DBcond_reg")
{
	ar = ((code >> 22) & 0x7);
	cond = ((code >> 16) & 0x1f);
	reg = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 34163 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34166 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34170 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34173 "isa_tms320.tcc"
>
OpDBcond_disp<
#line 45 "isa/tms320.isa"
CONFIG
#line 34178 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34182 "isa_tms320.tcc"
>::OpDBcond_disp(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34186 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34190 "isa_tms320.tcc"
>(code, addr, "DBcond_disp")
{
	ar = ((code >> 22) & 0x7);
	cond = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 34201 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34204 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34208 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34211 "isa_tms320.tcc"
>
OpDBcondD_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 34216 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34220 "isa_tms320.tcc"
>::OpDBcondD_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34224 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34228 "isa_tms320.tcc"
>(code, addr, "DBcondD_reg")
{
	ar = ((code >> 22) & 0x7);
	cond = ((code >> 16) & 0x1f);
	reg = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 34239 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34242 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34246 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34249 "isa_tms320.tcc"
>
OpDBcondD_disp<
#line 45 "isa/tms320.isa"
CONFIG
#line 34254 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34258 "isa_tms320.tcc"
>::OpDBcondD_disp(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34262 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34266 "isa_tms320.tcc"
>(code, addr, "DBcondD_disp")
{
	ar = ((code >> 22) & 0x7);
	cond = ((code >> 16) & 0x1f);
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 34277 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34280 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34284 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34287 "isa_tms320.tcc"
>
OpIACK_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 34292 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34296 "isa_tms320.tcc"
>::OpIACK_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34300 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34304 "isa_tms320.tcc"
>(code, addr, "IACK_dir")
{
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 34313 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34316 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34320 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34323 "isa_tms320.tcc"
>
OpIACK_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 34328 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34332 "isa_tms320.tcc"
>::OpIACK_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34336 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34340 "isa_tms320.tcc"
>(code, addr, "IACK_indir")
{
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 34351 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34354 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34358 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34361 "isa_tms320.tcc"
>
OpIDLE<
#line 45 "isa/tms320.isa"
CONFIG
#line 34366 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34370 "isa_tms320.tcc"
>::OpIDLE(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34374 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34378 "isa_tms320.tcc"
>(code, addr, "IDLE")
{
}

template <
#line 45 "isa/tms320.isa"
class
#line 34386 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34389 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34393 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34396 "isa_tms320.tcc"
>
OpNOP_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 34401 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34405 "isa_tms320.tcc"
>::OpNOP_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34409 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34413 "isa_tms320.tcc"
>(code, addr, "NOP_dir")
{
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 34422 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34425 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34429 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34432 "isa_tms320.tcc"
>
OpNOP_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 34437 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34441 "isa_tms320.tcc"
>::OpNOP_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34445 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34449 "isa_tms320.tcc"
>(code, addr, "NOP_indir")
{
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 34460 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34463 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34467 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34470 "isa_tms320.tcc"
>
OpRETIcond<
#line 45 "isa/tms320.isa"
CONFIG
#line 34475 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34479 "isa_tms320.tcc"
>::OpRETIcond(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34483 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34487 "isa_tms320.tcc"
>(code, addr, "RETIcond")
{
	cond = ((code >> 16) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 34496 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34499 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34503 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34506 "isa_tms320.tcc"
>
OpRETScond<
#line 45 "isa/tms320.isa"
CONFIG
#line 34511 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34515 "isa_tms320.tcc"
>::OpRETScond(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34519 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34523 "isa_tms320.tcc"
>(code, addr, "RETScond")
{
	cond = ((code >> 16) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 34532 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34535 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34539 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34542 "isa_tms320.tcc"
>
OpRPTB<
#line 45 "isa/tms320.isa"
CONFIG
#line 34547 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34551 "isa_tms320.tcc"
>::OpRPTB(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34555 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34559 "isa_tms320.tcc"
>(code, addr, "RPTB")
{
	src = ((code >> 0) & 0xffffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 34568 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34571 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34575 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34578 "isa_tms320.tcc"
>
OpRPTS_reg<
#line 45 "isa/tms320.isa"
CONFIG
#line 34583 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34587 "isa_tms320.tcc"
>::OpRPTS_reg(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34591 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34595 "isa_tms320.tcc"
>(code, addr, "RPTS_reg")
{
	src = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 34604 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34607 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34611 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34614 "isa_tms320.tcc"
>
OpRPTS_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 34619 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34623 "isa_tms320.tcc"
>::OpRPTS_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34627 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34631 "isa_tms320.tcc"
>(code, addr, "RPTS_dir")
{
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 34640 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34643 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34647 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34650 "isa_tms320.tcc"
>
OpRPTS_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 34655 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34659 "isa_tms320.tcc"
>::OpRPTS_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34663 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34667 "isa_tms320.tcc"
>(code, addr, "RPTS_indir")
{
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 34678 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34681 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34685 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34688 "isa_tms320.tcc"
>
OpRPTS_imm<
#line 45 "isa/tms320.isa"
CONFIG
#line 34693 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34697 "isa_tms320.tcc"
>::OpRPTS_imm(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34701 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34705 "isa_tms320.tcc"
>(code, addr, "RPTS_imm")
{
	imm = (((((int32_t)(code >> 0)) & 0xffff) << 16) >> 16);
}

template <
#line 45 "isa/tms320.isa"
class
#line 34714 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34717 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34721 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34724 "isa_tms320.tcc"
>
OpSWI<
#line 45 "isa/tms320.isa"
CONFIG
#line 34729 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34733 "isa_tms320.tcc"
>::OpSWI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34737 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34741 "isa_tms320.tcc"
>(code, addr, "SWI")
{
}

template <
#line 45 "isa/tms320.isa"
class
#line 34749 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34752 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34756 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34759 "isa_tms320.tcc"
>
OpTRAPcond<
#line 45 "isa/tms320.isa"
CONFIG
#line 34764 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34768 "isa_tms320.tcc"
>::OpTRAPcond(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34772 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34776 "isa_tms320.tcc"
>(code, addr, "TRAPcond")
{
	cond = ((code >> 16) & 0x1f);
	n = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 34786 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34789 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34793 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34796 "isa_tms320.tcc"
>
OpIDLE2<
#line 45 "isa/tms320.isa"
CONFIG
#line 34801 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34805 "isa_tms320.tcc"
>::OpIDLE2(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34809 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34813 "isa_tms320.tcc"
>(code, addr, "IDLE2")
{
}

template <
#line 45 "isa/tms320.isa"
class
#line 34821 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34824 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34828 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34831 "isa_tms320.tcc"
>
OpLOPOWER<
#line 45 "isa/tms320.isa"
CONFIG
#line 34836 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34840 "isa_tms320.tcc"
>::OpLOPOWER(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34844 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34848 "isa_tms320.tcc"
>(code, addr, "LOPOWER")
{
}

template <
#line 45 "isa/tms320.isa"
class
#line 34856 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34859 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34863 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34866 "isa_tms320.tcc"
>
OpMAXSPEED<
#line 45 "isa/tms320.isa"
CONFIG
#line 34871 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34875 "isa_tms320.tcc"
>::OpMAXSPEED(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34879 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34883 "isa_tms320.tcc"
>(code, addr, "MAXSPEED")
{
}

template <
#line 45 "isa/tms320.isa"
class
#line 34891 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34894 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34898 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34901 "isa_tms320.tcc"
>
OpLDFI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 34906 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34910 "isa_tms320.tcc"
>::OpLDFI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34914 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34918 "isa_tms320.tcc"
>(code, addr, "LDFI_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 34928 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34931 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34935 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34938 "isa_tms320.tcc"
>
OpLDFI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 34943 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34947 "isa_tms320.tcc"
>::OpLDFI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34951 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34955 "isa_tms320.tcc"
>(code, addr, "LDFI_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 34967 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 34970 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 34974 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 34977 "isa_tms320.tcc"
>
OpLDII_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 34982 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34986 "isa_tms320.tcc"
>::OpLDII_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 34990 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 34994 "isa_tms320.tcc"
>(code, addr, "LDII_dir")
{
	dst = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 35004 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35007 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35011 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35014 "isa_tms320.tcc"
>
OpLDII_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 35019 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35023 "isa_tms320.tcc"
>::OpLDII_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35027 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35031 "isa_tms320.tcc"
>(code, addr, "LDII_indir")
{
	dst = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 35043 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35046 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35050 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35053 "isa_tms320.tcc"
>
OpSIGI<
#line 45 "isa/tms320.isa"
CONFIG
#line 35058 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35062 "isa_tms320.tcc"
>::OpSIGI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35066 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35070 "isa_tms320.tcc"
>(code, addr, "SIGI")
{
}

template <
#line 45 "isa/tms320.isa"
class
#line 35078 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35081 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35085 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35088 "isa_tms320.tcc"
>
OpSTFI_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 35093 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35097 "isa_tms320.tcc"
>::OpSTFI_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35101 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35105 "isa_tms320.tcc"
>(code, addr, "STFI_dir")
{
	src = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 35115 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35118 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35122 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35125 "isa_tms320.tcc"
>
OpSTFI_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 35130 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35134 "isa_tms320.tcc"
>::OpSTFI_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35138 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35142 "isa_tms320.tcc"
>(code, addr, "STFI_indir")
{
	src = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 35154 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35157 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35161 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35164 "isa_tms320.tcc"
>
OpSTII_dir<
#line 45 "isa/tms320.isa"
CONFIG
#line 35169 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35173 "isa_tms320.tcc"
>::OpSTII_dir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35177 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35181 "isa_tms320.tcc"
>(code, addr, "STII_dir")
{
	src = ((code >> 16) & 0x1f);
	direct = ((code >> 0) & 0xffff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 35191 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35194 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35198 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35201 "isa_tms320.tcc"
>
OpSTII_indir<
#line 45 "isa/tms320.isa"
CONFIG
#line 35206 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35210 "isa_tms320.tcc"
>::OpSTII_indir(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35214 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35218 "isa_tms320.tcc"
>(code, addr, "STII_indir")
{
	src = ((code >> 16) & 0x1f);
	mod = ((code >> 11) & 0x1f);
	ar = ((code >> 8) & 0x7);
	disp = ((code >> 0) & 0xff);
}

template <
#line 45 "isa/tms320.isa"
class
#line 35230 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35233 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35237 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35240 "isa_tms320.tcc"
>
OpFLOAT_STF<
#line 45 "isa/tms320.isa"
CONFIG
#line 35245 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35249 "isa_tms320.tcc"
>::OpFLOAT_STF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35253 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35257 "isa_tms320.tcc"
>(code, addr, "FLOAT_STF")
{
	dst1 = ((code >> 22) & 0x7);
	src3 = ((code >> 16) & 0x7);
	d2_mod = ((code >> 11) & 0x1f);
	d2_ar = ((code >> 8) & 0x7);
	s2_mod = ((code >> 3) & 0x1f);
	s2_ar = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 35271 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35274 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35278 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35281 "isa_tms320.tcc"
>
OpFLOAT_STF_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 35286 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35290 "isa_tms320.tcc"
>::OpFLOAT_STF_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35294 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35298 "isa_tms320.tcc"
>(code, addr, "FLOAT_STF_ext")
{
	dst1 = ((code >> 22) & 0x7);
	src3 = ((code >> 16) & 0x7);
	d2_mod = ((code >> 11) & 0x1f);
	d2_ar = ((code >> 8) & 0x7);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 35311 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35314 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35318 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35321 "isa_tms320.tcc"
>
OpLDF_STF<
#line 45 "isa/tms320.isa"
CONFIG
#line 35326 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35330 "isa_tms320.tcc"
>::OpLDF_STF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35334 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35338 "isa_tms320.tcc"
>(code, addr, "LDF_STF")
{
	dst1 = ((code >> 22) & 0x7);
	src3 = ((code >> 16) & 0x7);
	d2_mod = ((code >> 11) & 0x1f);
	d2_ar = ((code >> 8) & 0x7);
	s2_mod = ((code >> 3) & 0x1f);
	s2_ar = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 35352 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35355 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35359 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35362 "isa_tms320.tcc"
>
OpLDF_STF_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 35367 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35371 "isa_tms320.tcc"
>::OpLDF_STF_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35375 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35379 "isa_tms320.tcc"
>(code, addr, "LDF_STF_ext")
{
	dst1 = ((code >> 22) & 0x7);
	src3 = ((code >> 16) & 0x7);
	d2_mod = ((code >> 11) & 0x1f);
	d2_ar = ((code >> 8) & 0x7);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 35392 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35395 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35399 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35402 "isa_tms320.tcc"
>
OpLDI_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 35407 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35411 "isa_tms320.tcc"
>::OpLDI_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35415 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35419 "isa_tms320.tcc"
>(code, addr, "LDI_STI")
{
	dst1 = ((code >> 22) & 0x7);
	src3 = ((code >> 16) & 0x7);
	d2_mod = ((code >> 11) & 0x1f);
	d2_ar = ((code >> 8) & 0x7);
	s2_mod = ((code >> 3) & 0x1f);
	s2_ar = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 35433 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35436 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35440 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35443 "isa_tms320.tcc"
>
OpLDI_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 35448 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35452 "isa_tms320.tcc"
>::OpLDI_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35456 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35460 "isa_tms320.tcc"
>(code, addr, "LDI_STI_ext")
{
	dst1 = ((code >> 22) & 0x7);
	src3 = ((code >> 16) & 0x7);
	d2_mod = ((code >> 11) & 0x1f);
	d2_ar = ((code >> 8) & 0x7);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 35473 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35476 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35480 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35483 "isa_tms320.tcc"
>
OpLSH3_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 35488 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35492 "isa_tms320.tcc"
>::OpLSH3_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35496 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35500 "isa_tms320.tcc"
>(code, addr, "LSH3_STI")
{
	dst1 = ((code >> 22) & 0x7);
	count = ((code >> 19) & 0x7);
	src3 = ((code >> 16) & 0x7);
	d2_mod = ((code >> 11) & 0x1f);
	d2_ar = ((code >> 8) & 0x7);
	s2_mod = ((code >> 3) & 0x1f);
	s2_ar = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 35515 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35518 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35522 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35525 "isa_tms320.tcc"
>
OpLSH3_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 35530 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35534 "isa_tms320.tcc"
>::OpLSH3_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35538 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35542 "isa_tms320.tcc"
>(code, addr, "LSH3_STI_ext")
{
	dst1 = ((code >> 22) & 0x7);
	count = ((code >> 19) & 0x7);
	src3 = ((code >> 16) & 0x7);
	d2_mod = ((code >> 11) & 0x1f);
	d2_ar = ((code >> 8) & 0x7);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 35556 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35559 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35563 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35566 "isa_tms320.tcc"
>
OpMPYF3_STF<
#line 45 "isa/tms320.isa"
CONFIG
#line 35571 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35575 "isa_tms320.tcc"
>::OpMPYF3_STF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35579 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35583 "isa_tms320.tcc"
>(code, addr, "MPYF3_STF")
{
	dst1 = ((code >> 22) & 0x7);
	src1 = ((code >> 19) & 0x7);
	src3 = ((code >> 16) & 0x7);
	d2_mod = ((code >> 11) & 0x1f);
	d2_ar = ((code >> 8) & 0x7);
	s2_mod = ((code >> 3) & 0x1f);
	s2_ar = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 35598 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35601 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35605 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35608 "isa_tms320.tcc"
>
OpMPYF3_STF_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 35613 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35617 "isa_tms320.tcc"
>::OpMPYF3_STF_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35621 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35625 "isa_tms320.tcc"
>(code, addr, "MPYF3_STF_ext")
{
	dst1 = ((code >> 22) & 0x7);
	src1 = ((code >> 19) & 0x7);
	src3 = ((code >> 16) & 0x7);
	d2_mod = ((code >> 11) & 0x1f);
	d2_ar = ((code >> 8) & 0x7);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 35639 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35642 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35646 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35649 "isa_tms320.tcc"
>
OpMPYI3_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 35654 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35658 "isa_tms320.tcc"
>::OpMPYI3_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35662 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35666 "isa_tms320.tcc"
>(code, addr, "MPYI3_STI")
{
	dst1 = ((code >> 22) & 0x7);
	src1 = ((code >> 19) & 0x7);
	src3 = ((code >> 16) & 0x7);
	d2_mod = ((code >> 11) & 0x1f);
	d2_ar = ((code >> 8) & 0x7);
	s2_mod = ((code >> 3) & 0x1f);
	s2_ar = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 35681 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35684 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35688 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35691 "isa_tms320.tcc"
>
OpMPYI3_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 35696 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35700 "isa_tms320.tcc"
>::OpMPYI3_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35704 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35708 "isa_tms320.tcc"
>(code, addr, "MPYI3_STI_ext")
{
	dst1 = ((code >> 22) & 0x7);
	src1 = ((code >> 19) & 0x7);
	src3 = ((code >> 16) & 0x7);
	d2_mod = ((code >> 11) & 0x1f);
	d2_ar = ((code >> 8) & 0x7);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 35722 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35725 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35729 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35732 "isa_tms320.tcc"
>
OpNEGF_STF<
#line 45 "isa/tms320.isa"
CONFIG
#line 35737 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35741 "isa_tms320.tcc"
>::OpNEGF_STF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35745 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35749 "isa_tms320.tcc"
>(code, addr, "NEGF_STF")
{
	dst1 = ((code >> 22) & 0x7);
	src3 = ((code >> 16) & 0x7);
	d2_mod = ((code >> 11) & 0x1f);
	d2_ar = ((code >> 8) & 0x7);
	s2_mod = ((code >> 3) & 0x1f);
	s2_ar = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 35763 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35766 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35770 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35773 "isa_tms320.tcc"
>
OpNEGF_STF_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 35778 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35782 "isa_tms320.tcc"
>::OpNEGF_STF_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35786 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35790 "isa_tms320.tcc"
>(code, addr, "NEGF_STF_ext")
{
	dst1 = ((code >> 22) & 0x7);
	src3 = ((code >> 16) & 0x7);
	d2_mod = ((code >> 11) & 0x1f);
	d2_ar = ((code >> 8) & 0x7);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 35803 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35806 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35810 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35813 "isa_tms320.tcc"
>
OpNEGI_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 35818 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35822 "isa_tms320.tcc"
>::OpNEGI_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35826 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35830 "isa_tms320.tcc"
>(code, addr, "NEGI_STI")
{
	dst1 = ((code >> 22) & 0x7);
	src3 = ((code >> 16) & 0x7);
	d2_mod = ((code >> 11) & 0x1f);
	d2_ar = ((code >> 8) & 0x7);
	s2_mod = ((code >> 3) & 0x1f);
	s2_ar = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 35844 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35847 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35851 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35854 "isa_tms320.tcc"
>
OpNEGI_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 35859 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35863 "isa_tms320.tcc"
>::OpNEGI_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35867 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35871 "isa_tms320.tcc"
>(code, addr, "NEGI_STI_ext")
{
	dst1 = ((code >> 22) & 0x7);
	src3 = ((code >> 16) & 0x7);
	d2_mod = ((code >> 11) & 0x1f);
	d2_ar = ((code >> 8) & 0x7);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 35884 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35887 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35891 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35894 "isa_tms320.tcc"
>
OpNOT_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 35899 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35903 "isa_tms320.tcc"
>::OpNOT_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35907 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35911 "isa_tms320.tcc"
>(code, addr, "NOT_STI")
{
	dst1 = ((code >> 22) & 0x7);
	src3 = ((code >> 16) & 0x7);
	d2_mod = ((code >> 11) & 0x1f);
	d2_ar = ((code >> 8) & 0x7);
	s2_mod = ((code >> 3) & 0x1f);
	s2_ar = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 35925 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35928 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35932 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35935 "isa_tms320.tcc"
>
OpNOT_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 35940 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35944 "isa_tms320.tcc"
>::OpNOT_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35948 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35952 "isa_tms320.tcc"
>(code, addr, "NOT_STI_ext")
{
	dst1 = ((code >> 22) & 0x7);
	src3 = ((code >> 16) & 0x7);
	d2_mod = ((code >> 11) & 0x1f);
	d2_ar = ((code >> 8) & 0x7);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 35965 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 35968 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 35972 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 35975 "isa_tms320.tcc"
>
OpOR3_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 35980 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35984 "isa_tms320.tcc"
>::OpOR3_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 35988 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 35992 "isa_tms320.tcc"
>(code, addr, "OR3_STI")
{
	dst1 = ((code >> 22) & 0x7);
	src1 = ((code >> 19) & 0x7);
	src3 = ((code >> 16) & 0x7);
	d2_mod = ((code >> 11) & 0x1f);
	d2_ar = ((code >> 8) & 0x7);
	s2_mod = ((code >> 3) & 0x1f);
	s2_ar = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 36007 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 36010 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 36014 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 36017 "isa_tms320.tcc"
>
OpOR3_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 36022 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36026 "isa_tms320.tcc"
>::OpOR3_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 36030 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36034 "isa_tms320.tcc"
>(code, addr, "OR3_STI_ext")
{
	dst1 = ((code >> 22) & 0x7);
	src1 = ((code >> 19) & 0x7);
	src3 = ((code >> 16) & 0x7);
	d2_mod = ((code >> 11) & 0x1f);
	d2_ar = ((code >> 8) & 0x7);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 36048 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 36051 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 36055 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 36058 "isa_tms320.tcc"
>
OpSTF_STF<
#line 45 "isa/tms320.isa"
CONFIG
#line 36063 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36067 "isa_tms320.tcc"
>::OpSTF_STF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 36071 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36075 "isa_tms320.tcc"
>(code, addr, "STF_STF")
{
	src2 = ((code >> 22) & 0x7);
	src1 = ((code >> 16) & 0x7);
	d1_mod = ((code >> 11) & 0x1f);
	d1_ar = ((code >> 8) & 0x7);
	d2_mod = ((code >> 3) & 0x1f);
	d2_ar = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 36089 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 36092 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 36096 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 36099 "isa_tms320.tcc"
>
OpSTF_STF_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 36104 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36108 "isa_tms320.tcc"
>::OpSTF_STF_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 36112 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36116 "isa_tms320.tcc"
>(code, addr, "STF_STF_ext")
{
	src2 = ((code >> 22) & 0x7);
	src1 = ((code >> 16) & 0x7);
	d1_mod = ((code >> 11) & 0x1f);
	d1_ar = ((code >> 8) & 0x7);
	dst2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 36129 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 36132 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 36136 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 36139 "isa_tms320.tcc"
>
OpSTI_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 36144 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36148 "isa_tms320.tcc"
>::OpSTI_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 36152 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36156 "isa_tms320.tcc"
>(code, addr, "STI_STI")
{
	src2 = ((code >> 22) & 0x7);
	src1 = ((code >> 16) & 0x7);
	d1_mod = ((code >> 11) & 0x1f);
	d1_ar = ((code >> 8) & 0x7);
	d2_mod = ((code >> 3) & 0x1f);
	d2_ar = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 36170 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 36173 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 36177 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 36180 "isa_tms320.tcc"
>
OpSTI_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 36185 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36189 "isa_tms320.tcc"
>::OpSTI_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 36193 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36197 "isa_tms320.tcc"
>(code, addr, "STI_STI_ext")
{
	src2 = ((code >> 22) & 0x7);
	src1 = ((code >> 16) & 0x7);
	d1_mod = ((code >> 11) & 0x1f);
	d1_ar = ((code >> 8) & 0x7);
	dst2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 36210 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 36213 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 36217 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 36220 "isa_tms320.tcc"
>
OpSUBF3_STF<
#line 45 "isa/tms320.isa"
CONFIG
#line 36225 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36229 "isa_tms320.tcc"
>::OpSUBF3_STF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 36233 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36237 "isa_tms320.tcc"
>(code, addr, "SUBF3_STF")
{
	dst1 = ((code >> 22) & 0x7);
	src1 = ((code >> 19) & 0x7);
	src3 = ((code >> 16) & 0x7);
	d2_mod = ((code >> 11) & 0x1f);
	d2_ar = ((code >> 8) & 0x7);
	s2_mod = ((code >> 3) & 0x1f);
	s2_ar = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 36252 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 36255 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 36259 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 36262 "isa_tms320.tcc"
>
OpSUBF3_STF_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 36267 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36271 "isa_tms320.tcc"
>::OpSUBF3_STF_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 36275 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36279 "isa_tms320.tcc"
>(code, addr, "SUBF3_STF_ext")
{
	dst1 = ((code >> 22) & 0x7);
	src1 = ((code >> 19) & 0x7);
	src3 = ((code >> 16) & 0x7);
	d2_mod = ((code >> 11) & 0x1f);
	d2_ar = ((code >> 8) & 0x7);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 36293 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 36296 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 36300 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 36303 "isa_tms320.tcc"
>
OpSUBI3_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 36308 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36312 "isa_tms320.tcc"
>::OpSUBI3_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 36316 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36320 "isa_tms320.tcc"
>(code, addr, "SUBI3_STI")
{
	dst1 = ((code >> 22) & 0x7);
	src1 = ((code >> 19) & 0x7);
	src3 = ((code >> 16) & 0x7);
	d2_mod = ((code >> 11) & 0x1f);
	d2_ar = ((code >> 8) & 0x7);
	s2_mod = ((code >> 3) & 0x1f);
	s2_ar = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 36335 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 36338 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 36342 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 36345 "isa_tms320.tcc"
>
OpSUBI3_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 36350 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36354 "isa_tms320.tcc"
>::OpSUBI3_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 36358 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36362 "isa_tms320.tcc"
>(code, addr, "SUBI3_STI_ext")
{
	dst1 = ((code >> 22) & 0x7);
	src1 = ((code >> 19) & 0x7);
	src3 = ((code >> 16) & 0x7);
	d2_mod = ((code >> 11) & 0x1f);
	d2_ar = ((code >> 8) & 0x7);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 36376 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 36379 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 36383 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 36386 "isa_tms320.tcc"
>
OpXOR3_STI<
#line 45 "isa/tms320.isa"
CONFIG
#line 36391 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36395 "isa_tms320.tcc"
>::OpXOR3_STI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 36399 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36403 "isa_tms320.tcc"
>(code, addr, "XOR3_STI")
{
	dst1 = ((code >> 22) & 0x7);
	src1 = ((code >> 19) & 0x7);
	src3 = ((code >> 16) & 0x7);
	d2_mod = ((code >> 11) & 0x1f);
	d2_ar = ((code >> 8) & 0x7);
	s2_mod = ((code >> 3) & 0x1f);
	s2_ar = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 36418 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 36421 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 36425 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 36428 "isa_tms320.tcc"
>
OpXOR3_STI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 36433 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36437 "isa_tms320.tcc"
>::OpXOR3_STI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 36441 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36445 "isa_tms320.tcc"
>(code, addr, "XOR3_STI_ext")
{
	dst1 = ((code >> 22) & 0x7);
	src1 = ((code >> 19) & 0x7);
	src3 = ((code >> 16) & 0x7);
	d2_mod = ((code >> 11) & 0x1f);
	d2_ar = ((code >> 8) & 0x7);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 36459 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 36462 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 36466 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 36469 "isa_tms320.tcc"
>
OpLDF_LDF<
#line 45 "isa/tms320.isa"
CONFIG
#line 36474 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36478 "isa_tms320.tcc"
>::OpLDF_LDF(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 36482 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36486 "isa_tms320.tcc"
>(code, addr, "LDF_LDF")
{
	dst1 = ((code >> 22) & 0x7);
	dst2 = ((code >> 19) & 0x7);
	s1_mod = ((code >> 11) & 0x1f);
	s1_ar = ((code >> 8) & 0x7);
	s2_mod = ((code >> 3) & 0x1f);
	s2_ar = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 36500 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 36503 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 36507 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 36510 "isa_tms320.tcc"
>
OpLDF_LDF_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 36515 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36519 "isa_tms320.tcc"
>::OpLDF_LDF_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 36523 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36527 "isa_tms320.tcc"
>(code, addr, "LDF_LDF_ext")
{
	dst1 = ((code >> 22) & 0x7);
	dst2 = ((code >> 19) & 0x7);
	s1_mod = ((code >> 11) & 0x1f);
	s1_ar = ((code >> 8) & 0x7);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 36540 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 36543 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 36547 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 36550 "isa_tms320.tcc"
>
OpLDI_LDI<
#line 45 "isa/tms320.isa"
CONFIG
#line 36555 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36559 "isa_tms320.tcc"
>::OpLDI_LDI(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 36563 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36567 "isa_tms320.tcc"
>(code, addr, "LDI_LDI")
{
	dst1 = ((code >> 22) & 0x7);
	dst2 = ((code >> 19) & 0x7);
	s1_mod = ((code >> 11) & 0x1f);
	s1_ar = ((code >> 8) & 0x7);
	s2_mod = ((code >> 3) & 0x1f);
	s2_ar = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 36581 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 36584 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 36588 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 36591 "isa_tms320.tcc"
>
OpLDI_LDI_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 36596 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36600 "isa_tms320.tcc"
>::OpLDI_LDI_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 36604 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36608 "isa_tms320.tcc"
>(code, addr, "LDI_LDI_ext")
{
	dst1 = ((code >> 22) & 0x7);
	dst2 = ((code >> 19) & 0x7);
	s1_mod = ((code >> 11) & 0x1f);
	s1_ar = ((code >> 8) & 0x7);
	src2 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 36621 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 36624 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 36628 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 36631 "isa_tms320.tcc"
>
OpMPYF3_ADDF3<
#line 45 "isa/tms320.isa"
CONFIG
#line 36636 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36640 "isa_tms320.tcc"
>::OpMPYF3_ADDF3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 36644 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36648 "isa_tms320.tcc"
>(code, addr, "MPYF3_ADDF3")
{
	p = ((code >> 24) & 0x3);
	d1 = ((code >> 23) & 0x1);
	d2 = ((code >> 22) & 0x1);
	src1 = ((code >> 19) & 0x7);
	src2 = ((code >> 16) & 0x7);
	s3_mod = ((code >> 11) & 0x1f);
	s3_ar = ((code >> 8) & 0x7);
	s4_mod = ((code >> 3) & 0x1f);
	s4_ar = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 36665 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 36668 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 36672 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 36675 "isa_tms320.tcc"
>
OpMPYF3_ADDF3_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 36680 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36684 "isa_tms320.tcc"
>::OpMPYF3_ADDF3_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 36688 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36692 "isa_tms320.tcc"
>(code, addr, "MPYF3_ADDF3_ext")
{
	p = ((code >> 24) & 0x3);
	d1 = ((code >> 23) & 0x1);
	d2 = ((code >> 22) & 0x1);
	src1 = ((code >> 19) & 0x7);
	src2 = ((code >> 16) & 0x7);
	src3 = ((code >> 8) & 0x1f);
	src4 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 36707 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 36710 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 36714 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 36717 "isa_tms320.tcc"
>
OpMPYF3_SUBF3<
#line 45 "isa/tms320.isa"
CONFIG
#line 36722 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36726 "isa_tms320.tcc"
>::OpMPYF3_SUBF3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 36730 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36734 "isa_tms320.tcc"
>(code, addr, "MPYF3_SUBF3")
{
	p = ((code >> 24) & 0x3);
	d1 = ((code >> 23) & 0x1);
	d2 = ((code >> 22) & 0x1);
	src1 = ((code >> 19) & 0x7);
	src2 = ((code >> 16) & 0x7);
	s3_mod = ((code >> 11) & 0x1f);
	s3_ar = ((code >> 8) & 0x7);
	s4_mod = ((code >> 3) & 0x1f);
	s4_ar = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 36751 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 36754 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 36758 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 36761 "isa_tms320.tcc"
>
OpMPYF3_SUBF3_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 36766 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36770 "isa_tms320.tcc"
>::OpMPYF3_SUBF3_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 36774 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36778 "isa_tms320.tcc"
>(code, addr, "MPYF3_SUBF3_ext")
{
	p = ((code >> 24) & 0x3);
	d1 = ((code >> 23) & 0x1);
	d2 = ((code >> 22) & 0x1);
	src1 = ((code >> 19) & 0x7);
	src2 = ((code >> 16) & 0x7);
	src3 = ((code >> 8) & 0x1f);
	src4 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 36793 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 36796 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 36800 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 36803 "isa_tms320.tcc"
>
OpMPYI3_ADDI3<
#line 45 "isa/tms320.isa"
CONFIG
#line 36808 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36812 "isa_tms320.tcc"
>::OpMPYI3_ADDI3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 36816 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36820 "isa_tms320.tcc"
>(code, addr, "MPYI3_ADDI3")
{
	p = ((code >> 24) & 0x3);
	d1 = ((code >> 23) & 0x1);
	d2 = ((code >> 22) & 0x1);
	src1 = ((code >> 19) & 0x7);
	src2 = ((code >> 16) & 0x7);
	s3_mod = ((code >> 11) & 0x1f);
	s3_ar = ((code >> 8) & 0x7);
	s4_mod = ((code >> 3) & 0x1f);
	s4_ar = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 36837 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 36840 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 36844 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 36847 "isa_tms320.tcc"
>
OpMPYI3_ADDI3_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 36852 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36856 "isa_tms320.tcc"
>::OpMPYI3_ADDI3_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 36860 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36864 "isa_tms320.tcc"
>(code, addr, "MPYI3_ADDI3_ext")
{
	p = ((code >> 24) & 0x3);
	d1 = ((code >> 23) & 0x1);
	d2 = ((code >> 22) & 0x1);
	src1 = ((code >> 19) & 0x7);
	src2 = ((code >> 16) & 0x7);
	src3 = ((code >> 8) & 0x1f);
	src4 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 36879 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 36882 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 36886 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 36889 "isa_tms320.tcc"
>
OpMPYI3_SUBI3<
#line 45 "isa/tms320.isa"
CONFIG
#line 36894 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36898 "isa_tms320.tcc"
>::OpMPYI3_SUBI3(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 36902 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36906 "isa_tms320.tcc"
>(code, addr, "MPYI3_SUBI3")
{
	p = ((code >> 24) & 0x3);
	d1 = ((code >> 23) & 0x1);
	d2 = ((code >> 22) & 0x1);
	src1 = ((code >> 19) & 0x7);
	src2 = ((code >> 16) & 0x7);
	s3_mod = ((code >> 11) & 0x1f);
	s3_ar = ((code >> 8) & 0x7);
	s4_mod = ((code >> 3) & 0x1f);
	s4_ar = ((code >> 0) & 0x7);
}

template <
#line 45 "isa/tms320.isa"
class
#line 36923 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 36926 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 36930 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 36933 "isa_tms320.tcc"
>
OpMPYI3_SUBI3_ext<
#line 45 "isa/tms320.isa"
CONFIG
#line 36938 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36942 "isa_tms320.tcc"
>::OpMPYI3_SUBI3_ext(CodeType code, typename CONFIG::address_t addr) : Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 36946 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36950 "isa_tms320.tcc"
>(code, addr, "MPYI3_SUBI3_ext")
{
	p = ((code >> 24) & 0x3);
	d1 = ((code >> 23) & 0x1);
	d2 = ((code >> 22) & 0x1);
	src1 = ((code >> 19) & 0x7);
	src2 = ((code >> 16) & 0x7);
	src3 = ((code >> 8) & 0x1f);
	src4 = ((code >> 0) & 0x1f);
}

template <
#line 45 "isa/tms320.isa"
class
#line 36965 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 36968 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 36972 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 36975 "isa_tms320.tcc"
>
DecodeMapPage<
#line 45 "isa/tms320.isa"
CONFIG
#line 36980 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 36984 "isa_tms320.tcc"
>::DecodeMapPage(typename CONFIG::address_t key)
{
	this->key = key;
	memset(operation, 0, sizeof(operation));
	next = 0;
}

template <
#line 45 "isa/tms320.isa"
class
#line 36995 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 36998 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 37002 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 37005 "isa_tms320.tcc"
>
DecodeMapPage<
#line 45 "isa/tms320.isa"
CONFIG
#line 37010 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 37014 "isa_tms320.tcc"
>::~DecodeMapPage()
{
	unsigned int idx;
	for(idx = 0; idx < NUM_OPERATIONS_PER_PAGE; idx++)
	delete operation[idx];
}

template <
#line 45 "isa/tms320.isa"
class
#line 37025 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 37028 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 37032 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 37035 "isa_tms320.tcc"
>
DecodeTableEntry<
#line 45 "isa/tms320.isa"
CONFIG
#line 37040 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 37044 "isa_tms320.tcc"
>::DecodeTableEntry(CodeType opcode, CodeType opcode_mask, Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 37048 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 37052 "isa_tms320.tcc"
> *(*decode)(CodeType, typename CONFIG::address_t))
{
	this->opcode = opcode;
	this->opcode_mask = opcode_mask;
	this->decode = decode;
}

template <
#line 45 "isa/tms320.isa"
class
#line 37063 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 37066 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 37070 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 37073 "isa_tms320.tcc"
>
Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 37078 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 37082 "isa_tms320.tcc"
>::Decoder()
{
	little_endian = true;
	mru_page = 0;
	memset(decode_hash_table, 0, sizeof(decode_hash_table));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37091 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37095 "isa_tms320.tcc"
	>(0x8c00e0e0UL, 0xfc00e0e0UL, DecodeOpMPYI3_SUBI3_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37099 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37103 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37108 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37112 "isa_tms320.tcc"
	>(0x8c000000UL, 0xfc000000UL, DecodeOpMPYI3_SUBI3<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37116 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37120 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37125 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37129 "isa_tms320.tcc"
	>(0x8800e0e0UL, 0xfc00e0e0UL, DecodeOpMPYI3_ADDI3_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37133 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37137 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37142 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37146 "isa_tms320.tcc"
	>(0x88000000UL, 0xfc000000UL, DecodeOpMPYI3_ADDI3<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37150 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37154 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37159 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37163 "isa_tms320.tcc"
	>(0x8400e0e0UL, 0xfc00e0e0UL, DecodeOpMPYF3_SUBF3_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37167 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37171 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37176 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37180 "isa_tms320.tcc"
	>(0x84000000UL, 0xfc000000UL, DecodeOpMPYF3_SUBF3<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37184 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37188 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37193 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37197 "isa_tms320.tcc"
	>(0x8000e0e0UL, 0xfc00e0e0UL, DecodeOpMPYF3_ADDF3_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37201 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37205 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37210 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37214 "isa_tms320.tcc"
	>(0x80000000UL, 0xfc000000UL, DecodeOpMPYF3_ADDF3<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37218 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37222 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37227 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37231 "isa_tms320.tcc"
	>(0xc60000e0UL, 0xfe0700e0UL, DecodeOpLDI_LDI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37235 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37239 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37244 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37248 "isa_tms320.tcc"
	>(0xc6000000UL, 0xfe070000UL, DecodeOpLDI_LDI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37252 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37256 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37261 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37265 "isa_tms320.tcc"
	>(0xc40000e0UL, 0xfe0700e0UL, DecodeOpLDF_LDF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37269 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37273 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37278 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37282 "isa_tms320.tcc"
	>(0xc4000000UL, 0xfe070000UL, DecodeOpLDF_LDF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37286 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37290 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37295 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37299 "isa_tms320.tcc"
	>(0xee0000e0UL, 0xfe0000e0UL, DecodeOpXOR3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37303 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37307 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37312 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37316 "isa_tms320.tcc"
	>(0xee000000UL, 0xfe000000UL, DecodeOpXOR3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37320 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37324 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37329 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37333 "isa_tms320.tcc"
	>(0xec0000e0UL, 0xfe0000e0UL, DecodeOpSUBI3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37337 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37341 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37346 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37350 "isa_tms320.tcc"
	>(0xec000000UL, 0xfe000000UL, DecodeOpSUBI3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37354 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37358 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37363 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37367 "isa_tms320.tcc"
	>(0xea0000e0UL, 0xfe0000e0UL, DecodeOpSUBF3_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37371 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37375 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37380 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37384 "isa_tms320.tcc"
	>(0xea000000UL, 0xfe000000UL, DecodeOpSUBF3_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37388 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37392 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37397 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37401 "isa_tms320.tcc"
	>(0xc20000e0UL, 0xfe3800e0UL, DecodeOpSTI_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37405 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37409 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37414 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37418 "isa_tms320.tcc"
	>(0xc2000000UL, 0xfe380000UL, DecodeOpSTI_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37422 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37426 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37431 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37435 "isa_tms320.tcc"
	>(0xc00000e0UL, 0xfe3800e0UL, DecodeOpSTF_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37439 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37443 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37448 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37452 "isa_tms320.tcc"
	>(0xc0000000UL, 0xfe380000UL, DecodeOpSTF_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37456 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37460 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37465 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37469 "isa_tms320.tcc"
	>(0xe80000e0UL, 0xfe0000e0UL, DecodeOpOR3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37473 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37477 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37482 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37486 "isa_tms320.tcc"
	>(0xe8000000UL, 0xfe000000UL, DecodeOpOR3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37490 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37494 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37499 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37503 "isa_tms320.tcc"
	>(0xe60000e0UL, 0xfe3800e0UL, DecodeOpNOT_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37507 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37511 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37516 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37520 "isa_tms320.tcc"
	>(0xe6000000UL, 0xfe380000UL, DecodeOpNOT_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37524 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37528 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37533 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37537 "isa_tms320.tcc"
	>(0xe40000e0UL, 0xfe3800e0UL, DecodeOpNEGI_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37541 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37545 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37550 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37554 "isa_tms320.tcc"
	>(0xe4000000UL, 0xfe380000UL, DecodeOpNEGI_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37558 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37562 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37567 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37571 "isa_tms320.tcc"
	>(0xe20000e0UL, 0xfe3800e0UL, DecodeOpNEGF_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37575 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37579 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37584 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37588 "isa_tms320.tcc"
	>(0xe2000000UL, 0xfe380000UL, DecodeOpNEGF_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37592 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37596 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37601 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37605 "isa_tms320.tcc"
	>(0xe00000e0UL, 0xfe0000e0UL, DecodeOpMPYI3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37609 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37613 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37618 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37622 "isa_tms320.tcc"
	>(0xe0000000UL, 0xfe000000UL, DecodeOpMPYI3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37626 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37630 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37635 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37639 "isa_tms320.tcc"
	>(0xde0000e0UL, 0xfe0000e0UL, DecodeOpMPYF3_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37643 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37647 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37652 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37656 "isa_tms320.tcc"
	>(0xde000000UL, 0xfe000000UL, DecodeOpMPYF3_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37660 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37664 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37669 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37673 "isa_tms320.tcc"
	>(0xdc0000e0UL, 0xfe0000e0UL, DecodeOpLSH3_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37677 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37681 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37686 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37690 "isa_tms320.tcc"
	>(0xdc000000UL, 0xfe000000UL, DecodeOpLSH3_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37694 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37698 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37703 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37707 "isa_tms320.tcc"
	>(0xda0000e0UL, 0xfe3800e0UL, DecodeOpLDI_STI_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37711 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37715 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37720 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37724 "isa_tms320.tcc"
	>(0xda000000UL, 0xfe380000UL, DecodeOpLDI_STI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37728 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37732 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37737 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37741 "isa_tms320.tcc"
	>(0xd80000e0UL, 0xfe3800e0UL, DecodeOpLDF_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37745 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37749 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37754 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37758 "isa_tms320.tcc"
	>(0xd8000000UL, 0xfe380000UL, DecodeOpLDF_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37762 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37766 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37771 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37775 "isa_tms320.tcc"
	>(0xd60000e0UL, 0xfe3800e0UL, DecodeOpFLOAT_STF_ext<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37779 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37783 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37788 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37792 "isa_tms320.tcc"
	>(0xd6000000UL, 0xfe380000UL, DecodeOpFLOAT_STF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37796 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37800 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37805 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37809 "isa_tms320.tcc"
	>(0x15c00000UL, 0xffe00000UL, DecodeOpSTII_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37813 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37817 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37822 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37826 "isa_tms320.tcc"
	>(0x15a00000UL, 0xffe00000UL, DecodeOpSTII_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37830 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37834 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37839 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37843 "isa_tms320.tcc"
	>(0x14c00000UL, 0xffe00000UL, DecodeOpSTFI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37847 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37851 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37856 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37860 "isa_tms320.tcc"
	>(0x14a00000UL, 0xffe00000UL, DecodeOpSTFI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37864 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37868 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37873 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37877 "isa_tms320.tcc"
	>(0x16000000UL, 0xffffffffUL, DecodeOpSIGI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37881 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37885 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37890 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37894 "isa_tms320.tcc"
	>(0x8c00000UL, 0xffe00000UL, DecodeOpLDII_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37898 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37902 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37907 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37911 "isa_tms320.tcc"
	>(0x8a00000UL, 0xffe00000UL, DecodeOpLDII_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37915 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37919 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37924 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37928 "isa_tms320.tcc"
	>(0x7c00000UL, 0xffe00000UL, DecodeOpLDFI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37932 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37936 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37941 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37945 "isa_tms320.tcc"
	>(0x7a00000UL, 0xffe00000UL, DecodeOpLDFI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37949 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37953 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37958 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37962 "isa_tms320.tcc"
	>(0x10800000UL, 0xffffffffUL, DecodeOpMAXSPEED<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37966 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37970 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37975 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37979 "isa_tms320.tcc"
	>(0x10800001UL, 0xffffffffUL, DecodeOpLOPOWER<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37983 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37987 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 37992 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 37996 "isa_tms320.tcc"
	>(0x6000001UL, 0xffffffffUL, DecodeOpIDLE2<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38000 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38004 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38009 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38013 "isa_tms320.tcc"
	>(0x74000020UL, 0xffe0ffe0UL, DecodeOpTRAPcond<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38017 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38021 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38026 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38030 "isa_tms320.tcc"
	>(0x66000000UL, 0xffffffffUL, DecodeOpSWI<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38034 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38038 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38043 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38047 "isa_tms320.tcc"
	>(0x13fb0000UL, 0xffff0000UL, DecodeOpRPTS_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38051 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38055 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38060 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38064 "isa_tms320.tcc"
	>(0x13db0000UL, 0xffff0000UL, DecodeOpRPTS_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38068 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38072 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38077 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38081 "isa_tms320.tcc"
	>(0x13bb0000UL, 0xffff0000UL, DecodeOpRPTS_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38085 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38089 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38094 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38098 "isa_tms320.tcc"
	>(0x139b0000UL, 0xffffffe0UL, DecodeOpRPTS_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38102 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38106 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38111 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38115 "isa_tms320.tcc"
	>(0x64000000UL, 0xff000000UL, DecodeOpRPTB<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38119 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38123 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38128 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38132 "isa_tms320.tcc"
	>(0x78800000UL, 0xffe0ffffUL, DecodeOpRETScond<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38136 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38140 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38145 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38149 "isa_tms320.tcc"
	>(0x78000000UL, 0xffe0ffffUL, DecodeOpRETIcond<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38153 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38157 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38162 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38166 "isa_tms320.tcc"
	>(0xcc00000UL, 0xffff0000UL, DecodeOpNOP_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38170 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38174 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38179 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38183 "isa_tms320.tcc"
	>(0xca00000UL, 0xffff0000UL, DecodeOpNOP_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38187 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38191 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38196 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38200 "isa_tms320.tcc"
	>(0x6000000UL, 0xffffffffUL, DecodeOpIDLE<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38204 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38208 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38213 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38217 "isa_tms320.tcc"
	>(0x1b400000UL, 0xffff0000UL, DecodeOpIACK_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38221 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38225 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38230 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38234 "isa_tms320.tcc"
	>(0x1b200000UL, 0xffff0000UL, DecodeOpIACK_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38238 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38242 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38247 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38251 "isa_tms320.tcc"
	>(0x6e200000UL, 0xfe200000UL, DecodeOpDBcondD_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38255 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38259 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38264 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38268 "isa_tms320.tcc"
	>(0x6c200000UL, 0xfe20ffe0UL, DecodeOpDBcondD_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38272 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38276 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38281 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38285 "isa_tms320.tcc"
	>(0x6e000000UL, 0xfe200000UL, DecodeOpDBcond_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38289 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38293 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38298 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38302 "isa_tms320.tcc"
	>(0x6c000000UL, 0xfe20ffe0UL, DecodeOpDBcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38306 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38310 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38315 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38319 "isa_tms320.tcc"
	>(0x72000000UL, 0xffe00000UL, DecodeOpCALLcond_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38323 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38327 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38332 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38336 "isa_tms320.tcc"
	>(0x70000000UL, 0xffe0ffe0UL, DecodeOpCALLcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38340 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38344 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38349 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38353 "isa_tms320.tcc"
	>(0x62000000UL, 0xff000000UL, DecodeOpCALL<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38357 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38361 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38366 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38370 "isa_tms320.tcc"
	>(0x61000000UL, 0xff000000UL, DecodeOpBRD<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38374 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38378 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38383 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38387 "isa_tms320.tcc"
	>(0x60000000UL, 0xff000000UL, DecodeOpBR<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38391 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38395 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38400 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38404 "isa_tms320.tcc"
	>(0x6a200000UL, 0xffe00000UL, DecodeOpBcondD_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38408 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38412 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38417 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38421 "isa_tms320.tcc"
	>(0x68200000UL, 0xffe0ffe0UL, DecodeOpBcondD_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38425 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38429 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38434 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38438 "isa_tms320.tcc"
	>(0x6a000000UL, 0xffe00000UL, DecodeOpBcond_disp<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38442 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38446 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38451 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38455 "isa_tms320.tcc"
	>(0x68000000UL, 0xffe0ffe0UL, DecodeOpBcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38459 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38463 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38468 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38472 "isa_tms320.tcc"
	>(0x28600000UL, 0xffe00000UL, DecodeOpXOR3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38476 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38480 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38485 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38489 "isa_tms320.tcc"
	>(0x28400000UL, 0xffe0e000UL, DecodeOpXOR3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38493 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38497 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38502 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38506 "isa_tms320.tcc"
	>(0x28200000UL, 0xffe000e0UL, DecodeOpXOR3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38510 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38514 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38519 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38523 "isa_tms320.tcc"
	>(0x28000000UL, 0xffe0e0e0UL, DecodeOpXOR3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38527 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38531 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38536 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38540 "isa_tms320.tcc"
	>(0x27e00000UL, 0xffe00000UL, DecodeOpTSTB3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38544 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38548 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38553 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38557 "isa_tms320.tcc"
	>(0x27c00000UL, 0xffe0e000UL, DecodeOpTSTB3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38561 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38565 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38570 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38574 "isa_tms320.tcc"
	>(0x27a00000UL, 0xffe000e0UL, DecodeOpTSTB3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38578 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38582 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38587 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38591 "isa_tms320.tcc"
	>(0x27800000UL, 0xffe0e0e0UL, DecodeOpTSTB3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38595 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38599 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38604 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38608 "isa_tms320.tcc"
	>(0x27600000UL, 0xffe00000UL, DecodeOpSUBI3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38612 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38616 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38621 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38625 "isa_tms320.tcc"
	>(0x27400000UL, 0xffe0e000UL, DecodeOpSUBI3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38629 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38633 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38638 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38642 "isa_tms320.tcc"
	>(0x27200000UL, 0xffe000e0UL, DecodeOpSUBI3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38646 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38650 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38655 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38659 "isa_tms320.tcc"
	>(0x27000000UL, 0xffe0e0e0UL, DecodeOpSUBI3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38663 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38667 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38672 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38676 "isa_tms320.tcc"
	>(0x26e00000UL, 0xffe00000UL, DecodeOpSUBF3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38680 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38684 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38689 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38693 "isa_tms320.tcc"
	>(0x26c00000UL, 0xffe0e000UL, DecodeOpSUBF3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38697 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38701 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38706 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38710 "isa_tms320.tcc"
	>(0x26a00000UL, 0xffe000e0UL, DecodeOpSUBF3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38714 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38718 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38723 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38727 "isa_tms320.tcc"
	>(0x26800000UL, 0xffe0e0e0UL, DecodeOpSUBF3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38731 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38735 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38740 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38744 "isa_tms320.tcc"
	>(0x26600000UL, 0xffe00000UL, DecodeOpSUBB3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38748 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38752 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38757 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38761 "isa_tms320.tcc"
	>(0x26400000UL, 0xffe0e000UL, DecodeOpSUBB3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38765 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38769 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38774 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38778 "isa_tms320.tcc"
	>(0x26200000UL, 0xffe000e0UL, DecodeOpSUBB3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38782 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38786 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38791 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38795 "isa_tms320.tcc"
	>(0x26000000UL, 0xffe0e0e0UL, DecodeOpSUBB3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38799 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38803 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38808 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38812 "isa_tms320.tcc"
	>(0x25e00000UL, 0xffe00000UL, DecodeOpOR3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38816 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38820 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38825 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38829 "isa_tms320.tcc"
	>(0x25c00000UL, 0xffe0e000UL, DecodeOpOR3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38833 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38837 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38842 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38846 "isa_tms320.tcc"
	>(0x25a00000UL, 0xffe000e0UL, DecodeOpOR3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38850 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38854 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38859 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38863 "isa_tms320.tcc"
	>(0x25800000UL, 0xffe0e0e0UL, DecodeOpOR3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38867 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38871 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38876 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38880 "isa_tms320.tcc"
	>(0x25600000UL, 0xffe00000UL, DecodeOpMPYI3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38884 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38888 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38893 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38897 "isa_tms320.tcc"
	>(0x25400000UL, 0xffe0e000UL, DecodeOpMPYI3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38901 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38905 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38910 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38914 "isa_tms320.tcc"
	>(0x25200000UL, 0xffe000e0UL, DecodeOpMPYI3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38918 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38922 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38927 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38931 "isa_tms320.tcc"
	>(0x25000000UL, 0xffe0e0e0UL, DecodeOpMPYI3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38935 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38939 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38944 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38948 "isa_tms320.tcc"
	>(0x24e00000UL, 0xffe00000UL, DecodeOpMPYF3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38952 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38956 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38961 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38965 "isa_tms320.tcc"
	>(0x24c00000UL, 0xffe0e000UL, DecodeOpMPYF3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38969 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38973 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38978 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38982 "isa_tms320.tcc"
	>(0x24a00000UL, 0xffe000e0UL, DecodeOpMPYF3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38986 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38990 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 38995 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 38999 "isa_tms320.tcc"
	>(0x24800000UL, 0xffe0e0e0UL, DecodeOpMPYF3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39003 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39007 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39012 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39016 "isa_tms320.tcc"
	>(0x24600000UL, 0xffe00000UL, DecodeOpLSH3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39020 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39024 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39029 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39033 "isa_tms320.tcc"
	>(0x24400000UL, 0xffe0e000UL, DecodeOpLSH3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39037 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39041 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39046 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39050 "isa_tms320.tcc"
	>(0x24200000UL, 0xffe000e0UL, DecodeOpLSH3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39054 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39058 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39063 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39067 "isa_tms320.tcc"
	>(0x24000000UL, 0xffe0e0e0UL, DecodeOpLSH3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39071 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39075 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39080 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39084 "isa_tms320.tcc"
	>(0x23e00000UL, 0xffe00000UL, DecodeOpCMPI3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39088 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39092 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39097 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39101 "isa_tms320.tcc"
	>(0x23c00000UL, 0xffe0e000UL, DecodeOpCMPI3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39105 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39109 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39114 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39118 "isa_tms320.tcc"
	>(0x23a00000UL, 0xffe000e0UL, DecodeOpCMPI3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39122 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39126 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39131 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39135 "isa_tms320.tcc"
	>(0x23800000UL, 0xffe0e0e0UL, DecodeOpCMPI3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39139 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39143 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39148 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39152 "isa_tms320.tcc"
	>(0x23600000UL, 0xffe00000UL, DecodeOpCMPF3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39156 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39160 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39165 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39169 "isa_tms320.tcc"
	>(0x23400000UL, 0xffe0e000UL, DecodeOpCMPF3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39173 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39177 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39182 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39186 "isa_tms320.tcc"
	>(0x23200000UL, 0xffe000e0UL, DecodeOpCMPF3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39190 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39194 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39199 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39203 "isa_tms320.tcc"
	>(0x23000000UL, 0xffe0e0e0UL, DecodeOpCMPF3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39207 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39211 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39216 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39220 "isa_tms320.tcc"
	>(0x22e00000UL, 0xffe00000UL, DecodeOpASH3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39224 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39228 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39233 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39237 "isa_tms320.tcc"
	>(0x22c00000UL, 0xffe0e000UL, DecodeOpASH3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39241 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39245 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39250 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39254 "isa_tms320.tcc"
	>(0x22a00000UL, 0xffe000e0UL, DecodeOpASH3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39258 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39262 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39267 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39271 "isa_tms320.tcc"
	>(0x22800000UL, 0xffe0e0e0UL, DecodeOpASH3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39275 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39279 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39284 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39288 "isa_tms320.tcc"
	>(0x22600000UL, 0xffe00000UL, DecodeOpANDN3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39292 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39296 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39301 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39305 "isa_tms320.tcc"
	>(0x22400000UL, 0xffe0e000UL, DecodeOpANDN3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39309 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39313 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39318 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39322 "isa_tms320.tcc"
	>(0x22200000UL, 0xffe000e0UL, DecodeOpANDN3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39326 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39330 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39335 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39339 "isa_tms320.tcc"
	>(0x22000000UL, 0xffe0e0e0UL, DecodeOpANDN3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39343 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39347 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39352 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39356 "isa_tms320.tcc"
	>(0x21e00000UL, 0xffe00000UL, DecodeOpAND3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39360 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39364 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39369 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39373 "isa_tms320.tcc"
	>(0x21c00000UL, 0xffe0e000UL, DecodeOpAND3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39377 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39381 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39386 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39390 "isa_tms320.tcc"
	>(0x21a00000UL, 0xffe000e0UL, DecodeOpAND3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39394 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39398 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39403 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39407 "isa_tms320.tcc"
	>(0x21800000UL, 0xffe0e0e0UL, DecodeOpAND3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39411 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39415 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39420 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39424 "isa_tms320.tcc"
	>(0x21600000UL, 0xffe00000UL, DecodeOpADDI3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39428 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39432 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39437 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39441 "isa_tms320.tcc"
	>(0x21400000UL, 0xffe0e000UL, DecodeOpADDI3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39445 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39449 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39454 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39458 "isa_tms320.tcc"
	>(0x21200000UL, 0xffe000e0UL, DecodeOpADDI3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39462 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39466 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39471 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39475 "isa_tms320.tcc"
	>(0x21000000UL, 0xffe0e0e0UL, DecodeOpADDI3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39479 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39483 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39488 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39492 "isa_tms320.tcc"
	>(0x20e00000UL, 0xffe00000UL, DecodeOpADDF3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39496 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39500 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39505 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39509 "isa_tms320.tcc"
	>(0x20c00000UL, 0xffe0e000UL, DecodeOpADDF3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39513 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39517 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39522 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39526 "isa_tms320.tcc"
	>(0x20a00000UL, 0xffe000e0UL, DecodeOpADDF3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39530 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39534 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39539 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39543 "isa_tms320.tcc"
	>(0x20800000UL, 0xffe0e0e0UL, DecodeOpADDF3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39547 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39551 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39556 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39560 "isa_tms320.tcc"
	>(0x20600000UL, 0xffe00000UL, DecodeOpADDC3_indir_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39564 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39568 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39573 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39577 "isa_tms320.tcc"
	>(0x20400000UL, 0xffe0e000UL, DecodeOpADDC3_reg_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39581 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39585 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39590 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39594 "isa_tms320.tcc"
	>(0x20200000UL, 0xffe000e0UL, DecodeOpADDC3_indir_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39598 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39602 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39607 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39611 "isa_tms320.tcc"
	>(0x20000000UL, 0xffe0e0e0UL, DecodeOpADDC3_reg_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39615 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39619 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39624 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39628 "isa_tms320.tcc"
	>(0x1ae00000UL, 0xffe00000UL, DecodeOpXOR_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39632 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39636 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39641 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39645 "isa_tms320.tcc"
	>(0x1ac00000UL, 0xffe00000UL, DecodeOpXOR_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39649 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39653 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39658 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39662 "isa_tms320.tcc"
	>(0x1aa00000UL, 0xffe00000UL, DecodeOpXOR_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39666 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39670 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39675 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39679 "isa_tms320.tcc"
	>(0x1a800000UL, 0xffe0ffe0UL, DecodeOpXOR_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39683 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39687 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39692 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39696 "isa_tms320.tcc"
	>(0x1a600000UL, 0xffe00000UL, DecodeOpTSTB_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39700 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39704 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39709 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39713 "isa_tms320.tcc"
	>(0x1a400000UL, 0xffe00000UL, DecodeOpTSTB_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39717 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39721 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39726 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39730 "isa_tms320.tcc"
	>(0x1a200000UL, 0xffe00000UL, DecodeOpTSTB_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39734 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39738 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39743 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39747 "isa_tms320.tcc"
	>(0x1a000000UL, 0xffe0ffe0UL, DecodeOpTSTB_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39751 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39755 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39760 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39764 "isa_tms320.tcc"
	>(0x19e00000UL, 0xffe00000UL, DecodeOpSUBRI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39768 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39772 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39777 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39781 "isa_tms320.tcc"
	>(0x19c00000UL, 0xffe00000UL, DecodeOpSUBRI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39785 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39789 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39794 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39798 "isa_tms320.tcc"
	>(0x19a00000UL, 0xffe00000UL, DecodeOpSUBRI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39802 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39806 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39811 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39815 "isa_tms320.tcc"
	>(0x19800000UL, 0xffe0ffe0UL, DecodeOpSUBRI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39819 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39823 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39828 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39832 "isa_tms320.tcc"
	>(0x19600000UL, 0xffe00000UL, DecodeOpSUBRF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39836 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39840 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39845 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39849 "isa_tms320.tcc"
	>(0x19400000UL, 0xffe00000UL, DecodeOpSUBRF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39853 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39857 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39862 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39866 "isa_tms320.tcc"
	>(0x19200000UL, 0xffe00000UL, DecodeOpSUBRF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39870 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39874 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39879 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39883 "isa_tms320.tcc"
	>(0x19000000UL, 0xffe0ffe0UL, DecodeOpSUBRF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39887 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39891 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39896 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39900 "isa_tms320.tcc"
	>(0x18e00000UL, 0xffe00000UL, DecodeOpSUBRB_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39904 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39908 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39913 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39917 "isa_tms320.tcc"
	>(0x18c00000UL, 0xffe00000UL, DecodeOpSUBRB_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39921 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39925 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39930 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39934 "isa_tms320.tcc"
	>(0x18a00000UL, 0xffe00000UL, DecodeOpSUBRB_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39938 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39942 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39947 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39951 "isa_tms320.tcc"
	>(0x18800000UL, 0xffe0ffe0UL, DecodeOpSUBRB_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39955 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39959 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39964 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39968 "isa_tms320.tcc"
	>(0x18600000UL, 0xffe00000UL, DecodeOpSUBI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39972 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39976 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39981 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39985 "isa_tms320.tcc"
	>(0x18400000UL, 0xffe00000UL, DecodeOpSUBI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39989 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 39993 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 39998 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40002 "isa_tms320.tcc"
	>(0x18200000UL, 0xffe00000UL, DecodeOpSUBI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40006 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40010 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40015 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40019 "isa_tms320.tcc"
	>(0x18000000UL, 0xffe0ffe0UL, DecodeOpSUBI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40023 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40027 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40032 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40036 "isa_tms320.tcc"
	>(0x17e00000UL, 0xffe00000UL, DecodeOpSUBF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40040 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40044 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40049 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40053 "isa_tms320.tcc"
	>(0x17c00000UL, 0xffe00000UL, DecodeOpSUBF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40057 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40061 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40066 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40070 "isa_tms320.tcc"
	>(0x17a00000UL, 0xffe00000UL, DecodeOpSUBF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40074 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40078 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40083 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40087 "isa_tms320.tcc"
	>(0x17800000UL, 0xffe0ffe0UL, DecodeOpSUBF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40091 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40095 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40100 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40104 "isa_tms320.tcc"
	>(0x17600000UL, 0xffe00000UL, DecodeOpSUBC_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40108 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40112 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40117 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40121 "isa_tms320.tcc"
	>(0x17400000UL, 0xffe00000UL, DecodeOpSUBC_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40125 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40129 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40134 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40138 "isa_tms320.tcc"
	>(0x17200000UL, 0xffe00000UL, DecodeOpSUBC_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40142 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40146 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40151 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40155 "isa_tms320.tcc"
	>(0x17000000UL, 0xffe0ffe0UL, DecodeOpSUBC_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40159 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40163 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40168 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40172 "isa_tms320.tcc"
	>(0x16e00000UL, 0xffe00000UL, DecodeOpSUBB_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40176 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40180 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40185 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40189 "isa_tms320.tcc"
	>(0x16c00000UL, 0xffe00000UL, DecodeOpSUBB_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40193 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40197 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40202 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40206 "isa_tms320.tcc"
	>(0x16a00000UL, 0xffe00000UL, DecodeOpSUBB_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40210 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40214 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40219 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40223 "isa_tms320.tcc"
	>(0x16800000UL, 0xffe0ffe0UL, DecodeOpSUBB_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40227 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40231 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40236 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40240 "isa_tms320.tcc"
	>(0x1360ffffUL, 0xffe0ffffUL, DecodeOpRORC<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40244 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40248 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40253 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40257 "isa_tms320.tcc"
	>(0x12e0ffffUL, 0xffe0ffffUL, DecodeOpROR<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40261 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40265 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40270 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40274 "isa_tms320.tcc"
	>(0x12600001UL, 0xffe0ffffUL, DecodeOpROLC<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40278 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40282 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40287 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40291 "isa_tms320.tcc"
	>(0x11e00001UL, 0xffe0ffffUL, DecodeOpROL<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40295 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40299 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40304 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40308 "isa_tms320.tcc"
	>(0x11600000UL, 0xffe00000UL, DecodeOpRND_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40312 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40316 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40321 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40325 "isa_tms320.tcc"
	>(0x11400000UL, 0xffe00000UL, DecodeOpRND_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40329 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40333 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40338 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40342 "isa_tms320.tcc"
	>(0x11200000UL, 0xffe00000UL, DecodeOpRND_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40346 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40350 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40355 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40359 "isa_tms320.tcc"
	>(0x11000000UL, 0xffe0ffe0UL, DecodeOpRND_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40363 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40367 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40372 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40376 "isa_tms320.tcc"
	>(0x10600000UL, 0xffe00000UL, DecodeOpOR_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40380 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40384 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40389 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40393 "isa_tms320.tcc"
	>(0x10400000UL, 0xffe00000UL, DecodeOpOR_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40397 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40401 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40406 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40410 "isa_tms320.tcc"
	>(0x10200000UL, 0xffe00000UL, DecodeOpOR_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40414 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40418 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40423 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40427 "isa_tms320.tcc"
	>(0x10000000UL, 0xffe0ffe0UL, DecodeOpOR_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40431 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40435 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40440 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40444 "isa_tms320.tcc"
	>(0xde00000UL, 0xffe00000UL, DecodeOpNOT_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40448 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40452 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40457 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40461 "isa_tms320.tcc"
	>(0xdc00000UL, 0xffe00000UL, DecodeOpNOT_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40465 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40469 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40474 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40478 "isa_tms320.tcc"
	>(0xda00000UL, 0xffe00000UL, DecodeOpNOT_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40482 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40486 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40491 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40495 "isa_tms320.tcc"
	>(0xd800000UL, 0xffe0ffe0UL, DecodeOpNOT_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40499 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40503 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40508 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40512 "isa_tms320.tcc"
	>(0xd600000UL, 0xffe00000UL, DecodeOpNORM_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40516 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40520 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40525 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40529 "isa_tms320.tcc"
	>(0xd400000UL, 0xffe00000UL, DecodeOpNORM_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40533 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40537 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40542 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40546 "isa_tms320.tcc"
	>(0xd200000UL, 0xffe00000UL, DecodeOpNORM_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40550 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40554 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40559 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40563 "isa_tms320.tcc"
	>(0xd000000UL, 0xffe0ffe0UL, DecodeOpNORM_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40567 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40571 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40576 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40580 "isa_tms320.tcc"
	>(0xc600000UL, 0xffe00000UL, DecodeOpNEGI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40584 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40588 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40593 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40597 "isa_tms320.tcc"
	>(0xc400000UL, 0xffe00000UL, DecodeOpNEGI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40601 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40605 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40610 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40614 "isa_tms320.tcc"
	>(0xc200000UL, 0xffe00000UL, DecodeOpNEGI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40618 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40622 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40627 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40631 "isa_tms320.tcc"
	>(0xc000000UL, 0xffe0ffe0UL, DecodeOpNEGI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40635 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40639 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40644 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40648 "isa_tms320.tcc"
	>(0xbe00000UL, 0xffe00000UL, DecodeOpNEGF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40652 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40656 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40661 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40665 "isa_tms320.tcc"
	>(0xbc00000UL, 0xffe00000UL, DecodeOpNEGF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40669 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40673 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40678 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40682 "isa_tms320.tcc"
	>(0xba00000UL, 0xffe00000UL, DecodeOpNEGF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40686 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40690 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40695 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40699 "isa_tms320.tcc"
	>(0xb800000UL, 0xffe0ffe0UL, DecodeOpNEGF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40703 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40707 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40712 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40716 "isa_tms320.tcc"
	>(0xb600000UL, 0xffe00000UL, DecodeOpNEGB_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40720 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40724 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40729 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40733 "isa_tms320.tcc"
	>(0xb400000UL, 0xffe00000UL, DecodeOpNEGB_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40737 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40741 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40746 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40750 "isa_tms320.tcc"
	>(0xb200000UL, 0xffe00000UL, DecodeOpNEGB_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40754 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40758 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40763 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40767 "isa_tms320.tcc"
	>(0xb000000UL, 0xffe0ffe0UL, DecodeOpNEGB_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40771 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40775 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40780 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40784 "isa_tms320.tcc"
	>(0xae00000UL, 0xffe00000UL, DecodeOpMPYI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40788 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40792 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40797 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40801 "isa_tms320.tcc"
	>(0xac00000UL, 0xffe00000UL, DecodeOpMPYI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40805 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40809 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40814 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40818 "isa_tms320.tcc"
	>(0xaa00000UL, 0xffe00000UL, DecodeOpMPYI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40822 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40826 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40831 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40835 "isa_tms320.tcc"
	>(0xa800000UL, 0xffe0ffe0UL, DecodeOpMPYI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40839 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40843 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40848 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40852 "isa_tms320.tcc"
	>(0xa600000UL, 0xffe00000UL, DecodeOpMPYF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40856 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40860 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40865 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40869 "isa_tms320.tcc"
	>(0xa400000UL, 0xffe00000UL, DecodeOpMPYF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40873 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40877 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40882 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40886 "isa_tms320.tcc"
	>(0xa200000UL, 0xffe00000UL, DecodeOpMPYF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40890 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40894 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40899 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40903 "isa_tms320.tcc"
	>(0xa000000UL, 0xffe0ffe0UL, DecodeOpMPYF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40907 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40911 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40916 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40920 "isa_tms320.tcc"
	>(0x9e00000UL, 0xffe00000UL, DecodeOpLSH_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40924 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40928 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40933 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40937 "isa_tms320.tcc"
	>(0x9c00000UL, 0xffe00000UL, DecodeOpLSH_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40941 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40945 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40950 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40954 "isa_tms320.tcc"
	>(0x9a00000UL, 0xffe00000UL, DecodeOpLSH_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40958 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40962 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40967 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40971 "isa_tms320.tcc"
	>(0x9800000UL, 0xffe0ffe0UL, DecodeOpLSH_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40975 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40979 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40984 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40988 "isa_tms320.tcc"
	>(0x5e00000UL, 0xffe00000UL, DecodeOpFLOAT_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 40992 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 40996 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41001 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41005 "isa_tms320.tcc"
	>(0x5c00000UL, 0xffe00000UL, DecodeOpFLOAT_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41009 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41013 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41018 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41022 "isa_tms320.tcc"
	>(0x5a00000UL, 0xffe00000UL, DecodeOpFLOAT_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41026 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41030 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41035 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41039 "isa_tms320.tcc"
	>(0x5800000UL, 0xffe0ffe0UL, DecodeOpFLOAT_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41043 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41047 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41052 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41056 "isa_tms320.tcc"
	>(0x5600000UL, 0xffe00000UL, DecodeOpFIX_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41060 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41064 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41069 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41073 "isa_tms320.tcc"
	>(0x5400000UL, 0xffe00000UL, DecodeOpFIX_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41077 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41081 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41086 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41090 "isa_tms320.tcc"
	>(0x5200000UL, 0xffe00000UL, DecodeOpFIX_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41094 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41098 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41103 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41107 "isa_tms320.tcc"
	>(0x5000000UL, 0xffe0ffe0UL, DecodeOpFIX_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41111 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41115 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41120 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41124 "isa_tms320.tcc"
	>(0x4e00000UL, 0xffe00000UL, DecodeOpCMPI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41128 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41132 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41137 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41141 "isa_tms320.tcc"
	>(0x4c00000UL, 0xffe00000UL, DecodeOpCMPI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41145 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41149 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41154 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41158 "isa_tms320.tcc"
	>(0x4a00000UL, 0xffe00000UL, DecodeOpCMPI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41162 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41166 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41171 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41175 "isa_tms320.tcc"
	>(0x4800000UL, 0xffe0ffe0UL, DecodeOpCMPI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41179 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41183 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41188 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41192 "isa_tms320.tcc"
	>(0x4600000UL, 0xffe00000UL, DecodeOpCMPF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41196 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41200 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41205 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41209 "isa_tms320.tcc"
	>(0x4400000UL, 0xffe00000UL, DecodeOpCMPF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41213 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41217 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41222 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41226 "isa_tms320.tcc"
	>(0x4200000UL, 0xffe00000UL, DecodeOpCMPF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41230 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41234 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41239 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41243 "isa_tms320.tcc"
	>(0x4000000UL, 0xffe0ffe0UL, DecodeOpCMPF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41247 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41251 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41256 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41260 "isa_tms320.tcc"
	>(0x3e00000UL, 0xffe00000UL, DecodeOpASH_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41264 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41268 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41273 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41277 "isa_tms320.tcc"
	>(0x3c00000UL, 0xffe00000UL, DecodeOpASH_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41281 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41285 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41290 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41294 "isa_tms320.tcc"
	>(0x3a00000UL, 0xffe00000UL, DecodeOpASH_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41298 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41302 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41307 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41311 "isa_tms320.tcc"
	>(0x3800000UL, 0xffe0ffe0UL, DecodeOpASH_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41315 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41319 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41324 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41328 "isa_tms320.tcc"
	>(0x3600000UL, 0xffe00000UL, DecodeOpANDN_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41332 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41336 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41341 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41345 "isa_tms320.tcc"
	>(0x3400000UL, 0xffe00000UL, DecodeOpANDN_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41349 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41353 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41358 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41362 "isa_tms320.tcc"
	>(0x3200000UL, 0xffe00000UL, DecodeOpANDN_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41366 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41370 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41375 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41379 "isa_tms320.tcc"
	>(0x3000000UL, 0xffe0ffe0UL, DecodeOpANDN_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41383 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41387 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41392 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41396 "isa_tms320.tcc"
	>(0x2e00000UL, 0xffe00000UL, DecodeOpAND_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41400 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41404 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41409 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41413 "isa_tms320.tcc"
	>(0x2c00000UL, 0xffe00000UL, DecodeOpAND_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41417 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41421 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41426 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41430 "isa_tms320.tcc"
	>(0x2a00000UL, 0xffe00000UL, DecodeOpAND_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41434 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41438 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41443 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41447 "isa_tms320.tcc"
	>(0x2800000UL, 0xffe0ffe0UL, DecodeOpAND_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41451 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41455 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41460 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41464 "isa_tms320.tcc"
	>(0x2600000UL, 0xffe00000UL, DecodeOpADDI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41468 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41472 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41477 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41481 "isa_tms320.tcc"
	>(0x2400000UL, 0xffe00000UL, DecodeOpADDI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41485 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41489 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41494 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41498 "isa_tms320.tcc"
	>(0x2200000UL, 0xffe00000UL, DecodeOpADDI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41502 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41506 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41511 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41515 "isa_tms320.tcc"
	>(0x2000000UL, 0xffe0ffe0UL, DecodeOpADDI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41519 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41523 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41528 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41532 "isa_tms320.tcc"
	>(0x1e00000UL, 0xffe00000UL, DecodeOpADDF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41536 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41540 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41545 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41549 "isa_tms320.tcc"
	>(0x1c00000UL, 0xffe00000UL, DecodeOpADDF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41553 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41557 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41562 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41566 "isa_tms320.tcc"
	>(0x1a00000UL, 0xffe00000UL, DecodeOpADDF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41570 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41574 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41579 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41583 "isa_tms320.tcc"
	>(0x1800000UL, 0xffe0ffe0UL, DecodeOpADDF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41587 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41591 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41596 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41600 "isa_tms320.tcc"
	>(0x1600000UL, 0xffe00000UL, DecodeOpADDC_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41604 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41608 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41613 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41617 "isa_tms320.tcc"
	>(0x1400000UL, 0xffe00000UL, DecodeOpADDC_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41621 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41625 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41630 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41634 "isa_tms320.tcc"
	>(0x1200000UL, 0xffe00000UL, DecodeOpADDC_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41638 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41642 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41647 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41651 "isa_tms320.tcc"
	>(0x1000000UL, 0xffe0ffe0UL, DecodeOpADDC_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41655 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41659 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41664 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41668 "isa_tms320.tcc"
	>(0xe00000UL, 0xffe00000UL, DecodeOpABSI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41672 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41676 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41681 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41685 "isa_tms320.tcc"
	>(0xc00000UL, 0xffe00000UL, DecodeOpABSI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41689 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41693 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41698 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41702 "isa_tms320.tcc"
	>(0xa00000UL, 0xffe00000UL, DecodeOpABSI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41706 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41710 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41715 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41719 "isa_tms320.tcc"
	>(0x800000UL, 0xffe0ffe0UL, DecodeOpABSI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41723 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41727 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41732 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41736 "isa_tms320.tcc"
	>(0x600000UL, 0xffe00000UL, DecodeOpABSF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41740 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41744 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41749 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41753 "isa_tms320.tcc"
	>(0x400000UL, 0xffe00000UL, DecodeOpABSF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41757 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41761 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41766 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41770 "isa_tms320.tcc"
	>(0x200000UL, 0xffe00000UL, DecodeOpABSF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41774 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41778 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41783 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41787 "isa_tms320.tcc"
	>(0x0UL, 0xffe0ffe0UL, DecodeOpABSF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41791 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41795 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41800 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41804 "isa_tms320.tcc"
	>(0x15400000UL, 0xffe00000UL, DecodeOpSTI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41808 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41812 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41817 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41821 "isa_tms320.tcc"
	>(0x15200000UL, 0xffe00000UL, DecodeOpSTI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41825 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41829 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41834 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41838 "isa_tms320.tcc"
	>(0x14400000UL, 0xfff80000UL, DecodeOpSTF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41842 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41846 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41851 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41855 "isa_tms320.tcc"
	>(0x14200000UL, 0xffe00000UL, DecodeOpSTF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41859 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41863 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41868 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41872 "isa_tms320.tcc"
	>(0xfa00000UL, 0xfff8ffffUL, DecodeOpPUSHF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41876 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41880 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41885 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41889 "isa_tms320.tcc"
	>(0xf200000UL, 0xfff8ffffUL, DecodeOpPUSH<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41893 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41897 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41902 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41906 "isa_tms320.tcc"
	>(0xea00000UL, 0xfff8ffffUL, DecodeOpPOPF<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41910 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41914 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41919 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41923 "isa_tms320.tcc"
	>(0xe200000UL, 0xffe0ffffUL, DecodeOpPOP<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41927 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41931 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41936 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41940 "isa_tms320.tcc"
	>(0x8700000UL, 0xffffff00UL, DecodeOpLDP<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41944 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41948 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41953 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41957 "isa_tms320.tcc"
	>(0x9600000UL, 0xfff8f000UL, DecodeOpLDM_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41961 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41965 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41970 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41974 "isa_tms320.tcc"
	>(0x9400000UL, 0xfff80000UL, DecodeOpLDM_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41978 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41982 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41987 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41991 "isa_tms320.tcc"
	>(0x9200000UL, 0xfff80000UL, DecodeOpLDM_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 41995 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 41999 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42004 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42008 "isa_tms320.tcc"
	>(0x9000000UL, 0xfff8ffe0UL, DecodeOpLDM_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42012 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42016 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42021 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42025 "isa_tms320.tcc"
	>(0x50600000UL, 0xf0600000UL, DecodeOpLDIcond_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42029 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42033 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42038 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42042 "isa_tms320.tcc"
	>(0x50400000UL, 0xf0600000UL, DecodeOpLDIcond_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42046 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42050 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42055 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42059 "isa_tms320.tcc"
	>(0x50200000UL, 0xf0600000UL, DecodeOpLDIcond_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42063 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42067 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42072 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42076 "isa_tms320.tcc"
	>(0x50000000UL, 0xf060ffe0UL, DecodeOpLDIcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42080 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42084 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42089 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42093 "isa_tms320.tcc"
	>(0x8600000UL, 0xffe00000UL, DecodeOpLDI_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42097 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42101 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42106 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42110 "isa_tms320.tcc"
	>(0x8400000UL, 0xffe00000UL, DecodeOpLDI_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42114 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42118 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42123 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42127 "isa_tms320.tcc"
	>(0x8200000UL, 0xffe00000UL, DecodeOpLDI_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42131 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42135 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42140 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42144 "isa_tms320.tcc"
	>(0x8000000UL, 0xffe0ffe0UL, DecodeOpLDI_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42148 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42152 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42157 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42161 "isa_tms320.tcc"
	>(0x40600000UL, 0xf0780000UL, DecodeOpLDFcond_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42165 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42169 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42174 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42178 "isa_tms320.tcc"
	>(0x40400000UL, 0xf0780000UL, DecodeOpLDFcond_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42182 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42186 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42191 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42195 "isa_tms320.tcc"
	>(0x40200000UL, 0xf0780000UL, DecodeOpLDFcond_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42199 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42203 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42208 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42212 "isa_tms320.tcc"
	>(0x40000000UL, 0xf078ffe0UL, DecodeOpLDFcond_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42216 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42220 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42225 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42229 "isa_tms320.tcc"
	>(0x7600000UL, 0xfff80000UL, DecodeOpLDF_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42233 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42237 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42242 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42246 "isa_tms320.tcc"
	>(0x7400000UL, 0xfff80000UL, DecodeOpLDF_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42250 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42254 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42259 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42263 "isa_tms320.tcc"
	>(0x7200000UL, 0xfff80000UL, DecodeOpLDF_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42267 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42271 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42276 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42280 "isa_tms320.tcc"
	>(0x7000000UL, 0xfff8ffe0UL, DecodeOpLDF_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42284 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42288 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42293 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42297 "isa_tms320.tcc"
	>(0x6e00000UL, 0xfff80000UL, DecodeOpLDE_imm<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42301 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42305 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42310 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42314 "isa_tms320.tcc"
	>(0x6c00000UL, 0xfff80000UL, DecodeOpLDE_indir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42318 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42322 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42327 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42331 "isa_tms320.tcc"
	>(0x6a00000UL, 0xfff80000UL, DecodeOpLDE_dir<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42335 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42339 "isa_tms320.tcc"
	>));
	decode_table.push_back(DecodeTableEntry<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42344 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42348 "isa_tms320.tcc"
	>(0x6800000UL, 0xfff8ffe0UL, DecodeOpLDE_reg<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42352 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42356 "isa_tms320.tcc"
	>));
}

template <
#line 45 "isa/tms320.isa"
class
#line 42363 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 42366 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 42370 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 42373 "isa_tms320.tcc"
>
Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 42378 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 42382 "isa_tms320.tcc"
>::~Decoder()
{
	InvalidateDecodingCache();
}

template <
#line 45 "isa/tms320.isa"
class
#line 42391 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 42394 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 42398 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 42401 "isa_tms320.tcc"
>
void Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 42406 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 42410 "isa_tms320.tcc"
>::Fetch(void *, typename CONFIG::address_t, uint32_t)
{
	assert( ! "Calling unimplemented virtual method Fetch" );
}

template <
#line 45 "isa/tms320.isa"
class
#line 42419 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 42422 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 42426 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 42429 "isa_tms320.tcc"
>
Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 42434 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 42438 "isa_tms320.tcc"
> *Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 42442 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 42446 "isa_tms320.tcc"
>::NCDecode(typename CONFIG::address_t addr)
{
	Operation<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42452 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42456 "isa_tms320.tcc"
	> *operation;
	CodeType code;
	Fetch(&code, addr, sizeof(code));
#if BYTE_ORDER == LITTLE_ENDIAN
	if(!little_endian)
#else
	if(little_endian)
#endif
	{
		code = ((code & 0xff000000UL) >> 24) | ((code & 0x00ff0000UL) >> 8) | ((code & 0x0000ff00UL) << 8) | ((code & 0x000000ffUL) << 24);
	}
	unsigned int count = decode_table.size();
	unsigned int idx;
	for(idx = 0; idx < count; idx++)
	{
		if((code & decode_table[idx].opcode_mask) == decode_table[idx].opcode)
		{
			operation = decode_table[idx].decode(code, addr);
			return operation;
		}
	}
	operation = new Operation<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42481 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42485 "isa_tms320.tcc"
	>(code, addr, "???");
	return operation;
}

template <
#line 45 "isa/tms320.isa"
class
#line 42493 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 42496 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 42500 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 42503 "isa_tms320.tcc"
>
Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 42508 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 42512 "isa_tms320.tcc"
> *Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 42516 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 42520 "isa_tms320.tcc"
>::NCDecode(typename CONFIG::address_t addr, CodeType code)
{
	Operation<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42526 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42530 "isa_tms320.tcc"
	> *operation;
	unsigned int count = decode_table.size();
	unsigned int idx;
	for(idx = 0; idx < count; idx++)
	{
		if((code & decode_table[idx].opcode_mask) == decode_table[idx].opcode)
		{
			operation = decode_table[idx].decode(code, addr);
			return operation;
		}
	}
	operation = new Operation<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42545 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42549 "isa_tms320.tcc"
	>(code, addr, "???");
	return operation;
}

template <
#line 45 "isa/tms320.isa"
class
#line 42557 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 42560 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 42564 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 42567 "isa_tms320.tcc"
>
void Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 42572 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 42576 "isa_tms320.tcc"
>::InvalidateDecodingCache()
{
	uint32_t index;
	mru_page = 0;
	for(index = 0; index < NUM_DECODE_HASH_TABLE_ENTRIES; index++)
	{
		DecodeMapPage<
#line 45 "isa/tms320.isa"
		CONFIG
#line 42586 "isa_tms320.tcc"
		,
#line 45 "isa/tms320.isa"
		DEBUG
#line 42590 "isa_tms320.tcc"
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
#line 45 "isa/tms320.isa"
class
#line 42609 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 42612 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 42616 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 42619 "isa_tms320.tcc"
>
void Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 42624 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 42628 "isa_tms320.tcc"
>::InvalidateDecodingCacheEntry(typename CONFIG::address_t addr)
{
	typename CONFIG::address_t page_key = addr / 4 / NUM_OPERATIONS_PER_PAGE;
	if(mru_page && mru_page->key == page_key) mru_page = 0;
	uint32_t index = page_key % NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key
	DecodeMapPage<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42637 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42641 "isa_tms320.tcc"
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
#line 45 "isa/tms320.isa"
class
#line 42674 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 42677 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 42681 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 42684 "isa_tms320.tcc"
>
inline DecodeMapPage<
#line 45 "isa/tms320.isa"
CONFIG
#line 42689 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 42693 "isa_tms320.tcc"
> *Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 42697 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 42701 "isa_tms320.tcc"
>::FindPage(typename CONFIG::address_t page_key)
{
	if(mru_page && mru_page->key == page_key) return mru_page;
	uint32_t index = page_key % NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key
	DecodeMapPage<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42709 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42713 "isa_tms320.tcc"
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
#line 45 "isa/tms320.isa"
class
#line 42747 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 42750 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 42754 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 42757 "isa_tms320.tcc"
>
Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 42762 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 42766 "isa_tms320.tcc"
> *Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 42770 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 42774 "isa_tms320.tcc"
>::Decode(typename CONFIG::address_t addr)
{
	Operation<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42780 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42784 "isa_tms320.tcc"
	> *operation;
	typename CONFIG::address_t page_key = addr / 4 / NUM_OPERATIONS_PER_PAGE;
	DecodeMapPage<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42790 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42794 "isa_tms320.tcc"
	> *page;
	page = FindPage(page_key);
	if(!page)
	{
		page = new DecodeMapPage<
#line 45 "isa/tms320.isa"
		CONFIG
#line 42802 "isa_tms320.tcc"
		,
#line 45 "isa/tms320.isa"
		DEBUG
#line 42806 "isa_tms320.tcc"
		>(page_key);
		uint32_t index = page_key % NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key
		page->next = decode_hash_table[index];
		decode_hash_table[index] = page;
		mru_page = page;
	}
	operation = page->operation[(addr / 4) & (NUM_OPERATIONS_PER_PAGE - 1)];
	if(operation)
	{
		return operation;
	}
	operation = NCDecode(addr);
	page->operation[(addr / 4) & (NUM_OPERATIONS_PER_PAGE - 1)] = operation;
	return operation;
}

template <
#line 45 "isa/tms320.isa"
class
#line 42826 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 42829 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 42833 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 42836 "isa_tms320.tcc"
>
Operation<
#line 45 "isa/tms320.isa"
CONFIG
#line 42841 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 42845 "isa_tms320.tcc"
> *Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 42849 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 42853 "isa_tms320.tcc"
>::Decode(typename CONFIG::address_t addr, CodeType insn)
{
	Operation<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42859 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42863 "isa_tms320.tcc"
	> *operation;
	typename CONFIG::address_t page_key = addr / 4 / NUM_OPERATIONS_PER_PAGE;
	DecodeMapPage<
#line 45 "isa/tms320.isa"
	CONFIG
#line 42869 "isa_tms320.tcc"
	,
#line 45 "isa/tms320.isa"
	DEBUG
#line 42873 "isa_tms320.tcc"
	> *page;
	page = FindPage(page_key);
	if(!page)
	{
		page = new DecodeMapPage<
#line 45 "isa/tms320.isa"
		CONFIG
#line 42881 "isa_tms320.tcc"
		,
#line 45 "isa/tms320.isa"
		DEBUG
#line 42885 "isa_tms320.tcc"
		> (page_key);
		uint32_t index = page_key % NUM_DECODE_HASH_TABLE_ENTRIES; // hash the key
		page->next = decode_hash_table[index];
		decode_hash_table[index] = page;
		mru_page = page;
	}
	operation = page->operation[(addr / 4) & (NUM_OPERATIONS_PER_PAGE - 1)];
	if(operation)
	{
		if(operation->GetEncoding() == insn && operation->GetAddr() == addr)
		return operation;
		delete operation;
	}
	operation = NCDecode(addr, insn);
	page->operation[(addr / 4) & (NUM_OPERATIONS_PER_PAGE - 1)] = operation;
	return operation;
}

template <
#line 45 "isa/tms320.isa"
class
#line 42907 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 42910 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 42914 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 42917 "isa_tms320.tcc"
>
void Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 42922 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 42926 "isa_tms320.tcc"
>::SetLittleEndian()
{
	little_endian = true;
}

template <
#line 45 "isa/tms320.isa"
class
#line 42935 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
CONFIG
#line 42938 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
bool
#line 42942 "isa_tms320.tcc"
#line 45 "isa/tms320.isa"
DEBUG
#line 42945 "isa_tms320.tcc"
>
void Decoder<
#line 45 "isa/tms320.isa"
CONFIG
#line 42950 "isa_tms320.tcc"
,
#line 45 "isa/tms320.isa"
DEBUG
#line 42954 "isa_tms320.tcc"
>::SetBigEndian()
{
	little_endian = false;
}

} } } } } } }
